#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/CGdalDem.hpp"

int main(int argc, char**argv){
	if (!NotePrint(argv, argc, 3)){ printf("Argument: first_dem_file second_dem_file\n"); return false; }
	printf("Loading...\n");
	//read dem file 
	char strFirstDemFile[FILE_PN],strSecondDemFile[FILE_PN];
	strcpy(strFirstDemFile, argv[1]); strcpy(strSecondDemFile, argv[2]);
	CGdalDem *pFirstDemFile = new CGdalDem,*pSecondDemFile=new CGdalDem;
	GDALDEMHDR *pFirstDemHead = new GDALDEMHDR, *pSecondDemHead = new GDALDEMHDR;
	if (pFirstDemFile->LoadFile(strFirstDemFile,pFirstDemHead) == false || pSecondDemFile->LoadFile(strSecondDemFile,pSecondDemHead) == false)return false;
	char strFirstOutputfile[FILE_PN],strSecondOutputfile[FILE_PN];
	strcpy(strFirstOutputfile, strFirstDemFile); strcpy(strSecondOutputfile, strSecondDemFile);
	sprintf(strrchr(strFirstOutputfile, '.'), "%s", "_Error_detected.tif");
	sprintf(strrchr(strSecondOutputfile, '.'), "%s", "_Error_detected.tif");
	printf("File name: %s\n", strFirstDemFile); printf("File name: %s\n", strSecondDemFile);
	//calculate Mx,My
	double lfFirstXmin = pFirstDemHead->lfStartX;
	double lfFirstXmax = pFirstDemHead->lfStartX + pFirstDemHead->lfGsdX*pFirstDemHead->iCol;
	double lfFirstYmin = pFirstDemHead->lfStartY;
	double lfFirstYmax = pFirstDemHead->lfStartY + pFirstDemHead->lfGsdY*pFirstDemHead->iRow;
	double lfSecondXmin = pSecondDemHead->lfStartX;
	double lfSecondXmax = pSecondDemHead->lfStartX + pSecondDemHead->lfGsdX*pSecondDemHead->iCol;
	double lfSecondYmin = pSecondDemHead->lfStartY;
	double lfSecondYmax = pSecondDemHead->lfStartY + pSecondDemHead->lfGsdY*pSecondDemHead->iRow;
	double lfOverlayXmin = MAX(lfFirstXmin,lfSecondXmin), lfOverlayXmax=MIN(lfFirstXmax,lfSecondXmax), 
		lfOverlayYmin=MAX(lfFirstYmin,lfSecondYmin), lfOverlayYmax=MIN(lfFirstYmax,lfSecondYmax);
	int iCols = 0, iRows = 0;
	iCols = int((lfOverlayXmax - lfOverlayXmin) / pFirstDemHead->lfGsdX) + 1;
	iRows = int((lfOverlayYmax - lfOverlayYmin) / pFirstDemHead->lfGsdY) + 1;
	float *pDiffZ = new float[iRows*iCols];
	Memset(pDiffZ, NODATA, iRows*iCols);
	double x = lfOverlayXmin, y = lfOverlayYmax;
	float fSumDiffZ = 0.0f; int iCount = 0;
	for (int i = 0; i < iRows; i++)
	{
		x = lfOverlayXmin;
		for (int j = 0; j < iCols; j++)
		{
			float z1 = pFirstDemFile->GetDemZValue(x, y);
			float z2 = pSecondDemFile->GetDemZValue(x, y);
			if (z1 != NODATA&&z2 != NODATA)
			{
				*(pDiffZ + i*iCols + j) = z1 - z2;
				fSumDiffZ += z1 - z2;
				iCount++;
			}
				
			x += pFirstDemHead->lfGsdX;
		}
		y -= pFirstDemHead->lfGsdY;
	}
	float fMeanDiffZ = fSumDiffZ / iCount;
	float fSum = 0.0f;
	iCount = 0;
	for (int i = 0; i < iRows; i++)
	{
		for (int j = 0; j < iCols; j++)
		{
			float z = *(pDiffZ + i*iCols + j);
			if (z != NODATA)
			{
				fSum += (z - fMeanDiffZ)*(z - fMeanDiffZ);
				iCount++;
			}
		}
	}
	float fRMSE = pow(fSum / iCount, 0.5f);
	float fThreshold = 3 * fRMSE;
	float *pFirstDemData = new float[pFirstDemHead->iCol*pFirstDemHead->iRow];
	pFirstDemFile->ReadBlock(pFirstDemData, 0, 0, pFirstDemHead->iCol, pFirstDemHead->iRow);
	float *pSecondDemData = new float[pSecondDemHead->iCol*pSecondDemHead->iRow];
	pSecondDemFile->ReadBlock(pSecondDemData, 0, 0, pSecondDemHead->iCol, pSecondDemHead->iRow);
	for (int i = 0; i < iRows; i++)
	{
		for (int j = 0; j < iCols; j++)
		{
			float z = *(pDiffZ + i*iCols + j);
			if (z != NODATA)
			{
				if (z>fThreshold)
				{
					int iFirstColID, iFirstRowID, iSecondColID, iSecondRowID;
					iFirstColID = int((lfOverlayXmin + j*pFirstDemHead->lfGsdX - pFirstDemHead->lfStartX)/pFirstDemHead->lfGsdX+0.5);
					iFirstRowID = int((pFirstDemHead->lfStartY+pFirstDemHead->iRow*pFirstDemHead->lfGsdY-(lfOverlayYmax - i*pFirstDemHead->lfGsdX)) / pFirstDemHead->lfGsdY+0.5);
					iSecondColID = int((lfOverlayXmin + j*pSecondDemHead->lfGsdX - pSecondDemHead->lfStartX) / pSecondDemHead->lfGsdX+0.5);
					iSecondRowID = int((pSecondDemHead->lfStartY + pSecondDemHead->iRow*pSecondDemHead->lfGsdY - (lfOverlayYmax - i*pSecondDemHead->lfGsdX)) / pSecondDemHead->lfGsdY+0.5);
					*(pFirstDemData + iFirstRowID*pFirstDemHead->iCol + iFirstColID) = NODATA;
					*(pSecondDemData + iSecondRowID*pSecondDemHead->iCol + iSecondColID) = NODATA;
				}
			}
		}
	}
	//save file
	CGdalDem *pFirstOutputFile = new CGdalDem, *pSecondOutputFile = new CGdalDem;
	if (false == pFirstOutputFile->CreatFile(strFirstOutputfile, pFirstDemHead)||false==pSecondOutputFile->CreatFile(strSecondOutputfile,pSecondDemHead))return false;
	pFirstOutputFile->WriteBlock(pFirstDemData, 0, 0, pFirstDemHead->iCol, pFirstDemHead->iRow);
	pSecondOutputFile->WriteBlock(pSecondDemData, 0, 0, pSecondDemHead->iCol, pSecondDemHead->iRow);
	
	delete[]pFirstDemData;
	delete[]pSecondDemData;
	delete pFirstOutputFile, pSecondOutputFile;
	delete pFirstDemFile, pFirstDemHead, pSecondDemFile, pSecondDemHead;
	return true;
}