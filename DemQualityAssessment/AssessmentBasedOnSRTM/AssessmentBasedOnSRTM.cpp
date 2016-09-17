#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/CGdalDem.hpp"
int main(int argc, char**argv)
{
	if (false == NotePrint(argv, argc, 3)){ printf("Argument: source_dem_file srtm_file\n"); return false; }
	printf("Loading...\n");
	char strInputfile[FILE_PN], strSrtmfile[FILE_PN];
	strcpy(strInputfile, argv[1]); strcpy(strSrtmfile, argv[2]);
	CGdalDem *pSourceDem = new CGdalDem, *pSrtm = new CGdalDem;
	GDALDEMHDR *pSrcDemHead = new GDALDEMHDR, *pSrtmHead = new GDALDEMHDR;
	if (false == pSourceDem->LoadFile(strInputfile, pSrcDemHead) || false == pSrtm->LoadFile(strSrtmfile, pSrtmHead)){ return false; }
	float*pSrcDemData = new float[pSrcDemHead->iRow*pSrcDemHead->iCol];
	pSourceDem->ReadBlock(pSrcDemData, 0, 0, pSrcDemHead->iCol, pSrcDemHead->iRow);
	float*pSrtmData = new float[pSrtmHead->iRow*pSrtmHead->iCol];
	pSrtm->ReadBlock(pSrtmData, 0, 0, pSrtmHead->iCol, pSrtmHead->iRow);
	float*pDiffZ = new float[pSrcDemHead->iRow*pSrcDemHead->iCol];
	for (int i = 0; i < pSrcDemHead->iRow; i++)
	{
		for (int j = 0; j < pSrcDemHead->iCol; j++)
		{
			if (0 == i || 0 == j || pSrcDemHead->iRow - 1 == i || pSrcDemHead->iCol - 1 == j ||
				1 == i || 1 == j || pSrcDemHead->iRow - 2 == i || pSrcDemHead->iCol - 2 == j){
				*(pDiffZ + i*pSrcDemHead->iCol + j) = NODATA; continue;
			}
			//compute slope from srtm
			if (*(pSrcDemData + i*pSrcDemHead->iCol + j - 1) != NODATA){
				double lfX, lfY; float fZ;
				pSourceDem->GetXYZValue(j, i, lfX, lfY, fZ);
				float fSlopeX = float((pSrtm->GetDemZValue(lfX + pSrtmHead->lfGsdX, lfY) - pSrtm->GetDemZValue(lfX - pSrtmHead->lfGsdX, lfY)) / (2 * pSrtmHead->lfGsdX));
				*(pDiffZ + i*pSrcDemHead->iCol + j) = float(*(pSrcDemData + i*pSrcDemHead->iCol + j - 1) + fSlopeX*pSrcDemHead->lfGsdX);
			}
			else *(pDiffZ + i*pSrcDemHead->iCol + j) = NODATA;
		}
	}
	//CGdalDem*pOutput = new CGdalDem;
	//pOutput->CreatFile("D:\\H_Research\\9_DEM_Fusion\\E-TEST\\Test1\\Output.tif", pSrcDemHead);
	//pOutput->WriteBlock(pDiffZ, 0, 0, pSrcDemHead->iCol, pSrcDemHead->iRow);
	//printf("%s\n", *(pOutput->GetFileName()));
	//delete pOutput;
	double lfSum = 0.0;
	float fMean = 0.0f, fMinError = 99999.0f, fMaxError = -99999.0f, fREMS = 0.0f;
	int iNums = 0;
	for (int i = 0; i < pSrcDemHead->iRow; i++)
	{
		for (int j = 0; j < pSrcDemHead->iCol; j++)
		{
			if (*(pDiffZ + i*pSrcDemHead->iCol + j) != NODATA&&*(pSrcDemData + i*pSrcDemHead->iCol + j) != NODATA)
			{
				*(pDiffZ + i*pSrcDemHead->iCol + j) -= *(pSrcDemData + i*pSrcDemHead->iCol + j);
				fREMS += pow(*(pDiffZ + i*pSrcDemHead->iCol + j), 2.0f);
				iNums++;
				lfSum += *(pDiffZ + i*pSrcDemHead->iCol + j);
				if (fMinError >(*(pDiffZ + i*pSrcDemHead->iCol + j)))fMinError = (*(pDiffZ + i*pSrcDemHead->iCol + j));//don't use 'fabs()'
				if (fMaxError < (*(pDiffZ + i*pSrcDemHead->iCol + j)))fMaxError = (*(pDiffZ + i*pSrcDemHead->iCol + j));
			}
			else *(pDiffZ + i*pSrcDemHead->iCol + j) = NODATA;
		}
	}
	fMean = float(lfSum / iNums);
	fREMS = pow(fREMS / iNums, 0.5f);
	printf("REMS=%f\tMeanError=%f\tMinError=%f\tMaxError=%f\n", fREMS,fMean, fMinError, fMaxError);
	delete[]pSrtmData; delete[]pSrcDemData; delete[]pDiffZ;
	delete pSrcDemHead, pSrtmHead, pSourceDem, pSrtm;
	return true;
}