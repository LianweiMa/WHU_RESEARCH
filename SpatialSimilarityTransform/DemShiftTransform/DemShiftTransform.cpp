#include <stdio.h>
#include <string.h>
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/CGdalDem.hpp"
using namespace std;
int main(int argc, char**argv)
{
	if (!NotePrint(argv, argc, 5)){ printf("Argument: dem_file para_tx para_ty para_tz\n"); return false; }

	char strInputFile[FILE_PN];
	strcpy(strInputFile, argv[1]);//��1�������Ǵ��任��DEM�ļ�
	// ��ȡ���� 
	double lfTx = atof(argv[2]), lfTy = atof(argv[3]), lfTz = atof(argv[4]);
	CGdalDem *pOldDemFile = new CGdalDem;
	GDALDEMHDR *pOldDemHead = new GDALDEMHDR;
	if (false == pOldDemFile->LoadFile(strInputFile, pOldDemHead)){ printf("����DEMʧ�ܣ�\n"); return false; }
	int iCols = pOldDemHead->iCol;
	int iRows = pOldDemHead->iRow;

	//�¾�����ת��
	float *pData = new float[iCols*iRows];
	pOldDemFile->ReadBlock(pData, 0, 0, iCols, iRows);
	for (int i = 0; i < iRows; i++){
		for (int j = 0; j < iCols; j++){
			float fOldZ = *(pData+i*iCols+j);
			if (fOldZ != NODATA)*(pData + i*iCols + j) += (float)lfTz;
		}
	}
	//��DEM�ļ�д��ͷ��Ϣ
	char strOutputFile[FILE_PN]; strcpy(strOutputFile, strInputFile);
	sprintf(strrchr(strOutputFile, '.'), "%s", "_Shift_Transform.tif");
	CGdalDem *pNewDemFile = new CGdalDem;
	GDALDEMHDR *pNewDemHead = new GDALDEMHDR;
	pNewDemHead->iCol = iCols;
	pNewDemHead->iRow = iRows;
	pNewDemHead->lfGsdX = pOldDemHead->lfGsdX;
	pNewDemHead->lfGsdY = pOldDemHead->lfGsdY;
	pNewDemHead->lfStartX = pOldDemHead->lfStartX+lfTx;
	pNewDemHead->lfStartY = pOldDemHead->lfStartY+lfTy;
	if (false == pNewDemFile->CreatFile(strOutputFile, pNewDemHead))return false;
	pNewDemFile->WriteBlock(pData, 0, 0, pNewDemHead->iCol, pNewDemHead->iRow);
	printf("Saved file: %s\n", strOutputFile);
	delete[]pData;
	delete pOldDemFile, pOldDemHead, pNewDemFile, pNewDemHead;
	return true;
}