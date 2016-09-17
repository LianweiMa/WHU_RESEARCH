#include <stdio.h>
#include <string.h>
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/CGdalDem.hpp"
#include "ResearchCode/TypeDef.h"
#define  SRTM_PRECISE 120.0f//unit:meter
float GetMosaicZvalue(double &lfX_i, double &lfY_i, CGdalDem*pDemList_i, int &iDemNum_i, CGdalDem*pSrtm_i)//arithmetic weighted mean
{
	float fSrtmZ = pSrtm_i->GetDemZValue(lfX_i, lfY_i);
	float fZ = NODATA, fSum = 0.0; int iNumZ = 0;
	for (int i = 0; i < iDemNum_i; i++)
	{
		fZ = pDemList_i[i].GetDemZValue(lfX_i, lfY_i);
		if (fZ != NODATA && fabs(fSrtmZ - fZ) < SRTM_PRECISE){
			iNumZ++;
			fSum += fZ;
		}
	}
	if (iNumZ > 0)return fSum / iNumZ;
	else return NODATA;
}
int main(int argc, char**argv)
{
	if (!NotePrint(argv, argc, 3)){ printf("Argument: demlist_file srtm_file\n"); return false; }
	//read dem file list
	char strInputfile[512], strSrtmFile[512];
	strcpy(strInputfile, argv[1]); strcpy(strSrtmFile, argv[2]);
	int iDemNum = 0;//nDemNum:the number of dem file
	FILE *fp = fopen(strInputfile, "r"); if (!fp){ printf("Error_FileOpen:%s\n", strInputfile); return false; }
	fscanf(fp, "%d\n", &iDemNum);
	char **pDemfile = new char *[iDemNum];
	for (int i = 0; i < iDemNum; i++)
	{
		pDemfile[i] = new char[512];
		fscanf(fp, "%s\n", pDemfile[i]);
	}
	fclose(fp);

	//read dem//print dem info
	printf("\n-----Dem info-----\n");
	CGdalDem *pSrtm = new CGdalDem; GDALDEMHDR *PSrtmHead = new GDALDEMHDR;
	if (!pSrtm->LoadFile(strSrtmFile,PSrtmHead))
	{
		printf("Error_FileOpen:%s\n", strSrtmFile);
		return false;
	}
	printf("--SRTM--:%s\n", strSrtmFile);
	printf("Xgsd:%lf\tYgsd:%lf\n", PSrtmHead->lfGsdX, PSrtmHead->lfGsdY);
	printf("Colum:%d\tRow:%d\n", PSrtmHead->iCol, PSrtmHead->iRow);
	printf("Zmin:%lf\tZmax:%lf\n", pSrtm->GetMinZ(), pSrtm->GetMaxZ());
	double lfXSrtm[4], lfYSrtm[4];
	pSrtm->GetDemRegion(lfXSrtm, lfYSrtm);
	printf("GeoRange:\n\tLB-[%lf,%lf]\n\tRB-[%lf,%lf]\n\tRT-[%lf,%lf]\n\tLT-[%lf,%lf]\n",
		lfXSrtm[0], lfYSrtm[0], lfXSrtm[1], lfYSrtm[1], lfXSrtm[2], lfYSrtm[2], lfXSrtm[3], lfYSrtm[3]);
	CGdalDem *pDemList = new CGdalDem[iDemNum]; if (!pDemList)return false;
	double lfXmin = NODATA, lfXmax = NODATA, lfYmin = NODATA, lfYmax = NODATA;
	double lfXgsd = 0.0, lfYgsd = 0.0;
	for (int i = 0; i < iDemNum; i++)
	{
		if (!pDemList[i].LoadFile(pDemfile[i]))
		{
			printf("Error_FileOpen:%s\n", pDemfile[i]);
			return false;
		}
		printf("--Dem %d--:%s\n", i + 1, pDemfile[i]);
		GDALDEMHDR pDemListHdr = pDemList[i].GetDemHeader();
		printf("Xgsd:%lf\tYgsd:%lf\n", pDemListHdr.lfGsdX, pDemListHdr.lfGsdY);
		printf("Colum:%d\tRow:%d\n", pDemListHdr.iCol, pDemListHdr.iRow);
		if (lfXgsd == 0.0)lfXgsd = pDemListHdr.lfGsdX;
		if (lfYgsd == 0.0)lfYgsd = pDemListHdr.lfGsdY;
		if (lfXgsd < pDemListHdr.lfGsdX)lfXgsd = pDemListHdr.lfGsdX;
		if (lfYgsd < pDemListHdr.lfGsdY)lfYgsd = pDemListHdr.lfGsdY;
		double lfZmin = NODATA, lfZmax = NODATA;
		lfZmax = pDemList[i].GetMaxZ(); lfZmin = pDemList[i].GetMinZ();
		printf("Zmin:%lf\tZmax:%lf\n", lfZmin, lfZmax);
		double lfX[4], lfY[4];
		pDemList[i].GetDemRegion(lfX, lfY);
		printf("GeoRange:\n\tLB-[%lf,%lf]\n\tRB-[%lf,%lf]\n\tRT-[%lf,%lf]\n\tLT-[%lf,%lf]\n", lfX[0], lfY[0], lfX[1], lfY[1], lfX[2], lfY[2], lfX[3], lfY[3]);
		if (lfXmin == NODATA) lfXmin = lfX[0];
		if (lfXmax == NODATA) lfXmax = lfX[0];
		if (lfYmin == NODATA) lfYmin = lfY[0];
		if (lfYmax == NODATA) lfYmax = lfY[0];
		for (int i = 0; i < 4; i++)
		{
			if (lfXmin > lfX[i]) lfXmin = lfX[i];
			if (lfXmax < lfX[i]) lfXmax = lfX[i];
			if (lfYmin > lfY[i]) lfYmin = lfY[i];
			if (lfYmax < lfY[i]) lfYmax = lfY[i];
		}
	}

	//Mosaic dem
	printf("\n-----Mosaic dem-----\n");
	int iCols = 0, iRows = 0;
	iCols = int((lfXmax - lfXmin) / lfXgsd) + 1;
	iRows = int((lfYmax - lfYmin) / lfYgsd) + 1;
	printf("Rows:%d\tCols:%d\n", iRows, iCols);
	printf("Mosaic Dem ...  0%%");
	float *pMosaicDemData = new float[iRows*iCols];
	double x = lfXmin, y = lfYmax;
	for (int i = 0; i < iRows; i++)
	{
		x = lfXmin;
		for (int j = 0; j < iCols; j++)
		{
			*(pMosaicDemData + i*iCols + j) = GetMosaicZvalue(x, y, pDemList, iDemNum, pSrtm);
			x += lfXgsd;
		}
		y -= lfYgsd;
		printf("\b\b\b%2d%%", i * 100 / iRows);
	}
	printf("\b\b\b100%%\n");
	printf("\n-----Save mosaic Dem-----\n");
	CGdalDem *pMosaicDemFile = new CGdalDem; GDALDEMHDR *pMosaicDemHead = new GDALDEMHDR;
	pMosaicDemHead->lfStartX = lfXmin; pMosaicDemHead->lfStartY = lfYmin;
	pMosaicDemHead->lfGsdX = lfXgsd; pMosaicDemHead->lfGsdY = lfYgsd;
	pMosaicDemHead->iCol = iCols;   pMosaicDemHead->iRow = iRows;
	char strOutputfile[512]; strcpy(strOutputfile, strInputfile);
	char *pS = strrchr(strOutputfile, '/'); if (!pS)pS = strrchr(strOutputfile, '\\');
	sprintf(pS, "%s", "\\Dem_Mosaic.tif");
	if (true != pMosaicDemFile->CreatFile(strOutputfile, pMosaicDemHead))return false;
	pMosaicDemFile->WriteBlock(pMosaicDemData, 0, 0, iCols, iRows);
	printf("Saved mosaic Dem file: %s\n", strOutputfile);

	//delete memory
	delete pMosaicDemFile, pMosaicDemHead;
	delete[]pMosaicDemData;
	delete pSrtm; delete PSrtmHead;
	delete[]pDemList;
	for (int i = 0; i < iDemNum; i++)
	{
		delete[]pDemfile[i];
	}
	delete[]pDemfile;
	return true;
}