#include <stdio.h>
#include <string.h>
#include "ResearchCode/CGdalDem.hpp"
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/TypeDef.h"
#define  SRTM_PRECISE 120.0f//unit:meter
float GetFusedZvalue(double x, double y, CGdalDem*pDemList_i, int &nDemNum, double &lfXmin, double &lfYmin, double &lfXmax, double &lfYmax)//distance as weight
{
	if (nDemNum != 2)printf("Error_InputDemNUms:this method is for 2 dems\n");
	float *pfZ_Temp = new float[nDemNum]; float *pfZ_Old = pfZ_Temp;
	if (lfXmax - lfXmin <= lfYmax - lfYmin)
	{//left-right
		int numZ = 0;
		for (int kDemIndex = 0; kDemIndex < nDemNum; kDemIndex++)
		{
			*pfZ_Temp = pDemList_i[kDemIndex].GetDemZValue(x, y);
			if (*pfZ_Temp != NODATA){ pfZ_Temp++; numZ++; }
		}
		pfZ_Temp = pfZ_Old;
		if (numZ == 2)return float(((x - lfXmin)*pfZ_Temp[0] + (lfXmax - x)*pfZ_Temp[1]) / (lfXmax - lfXmin));
		else if (numZ == 1)
		{
			if (pfZ_Temp[0] != NODATA)return pfZ_Temp[0];
			else return pfZ_Temp[1];
		}		
		else return NODATA;
	}
	else
	{//top-bottom
		int numZ = 0;
		for (int kDemIndex = 0; kDemIndex < nDemNum; kDemIndex++)
		{
			*pfZ_Temp = pDemList_i[kDemIndex].GetDemZValue(x, y);
			if (*pfZ_Temp != NODATA){ pfZ_Temp++; numZ++; }
		}
		pfZ_Temp = pfZ_Old;
		if (numZ == 2)return float(((y - lfYmin)*pfZ_Temp[0] + (lfYmax - y)*pfZ_Temp[1]) / (lfYmax - lfYmin));
		else if (numZ == 1)
		{
			if (pfZ_Temp[0] != NODATA)return pfZ_Temp[0];
			else return pfZ_Temp[1];
		}
		else return NODATA;
	}
	delete[]pfZ_Temp;
}
int main(int argc, char**argv)
{
	if (!NotePrint(argv, argc, 2)){ printf("Argument: demlist_file\n"); return false; }
	printf("Loading...\n");
	//read dem file list
	char strInputfile[FILE_PN];
	strcpy(strInputfile, argv[1]); 
	int iDemNum = 0;//nDemNum:the number of dem file
	FILE *fp = fopen(strInputfile, "r"); if (!fp){ printf("Error_FileOpen:%s\n", strInputfile); return false; }
	fscanf(fp, "%d\n", &iDemNum);
	char **pDemfile = new char *[iDemNum];
	for (int i = 0; i < iDemNum; i++)
	{
		pDemfile[i] = new char[FILE_PN];
		fscanf(fp, "%s\n", pDemfile[i]);
	}
	fclose(fp);

	//read dem//print dem info
	printf("\n-----Dem info-----\n");
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

	//Fuse dem
	printf("\n-----Fuse dem-----\n");
	int iCols = 0, iRows = 0;
	iCols = int((lfXmax - lfXmin) / lfXgsd) + 1;
	iRows = int((lfYmax - lfYmin) / lfYgsd) + 1;
	printf("Rows:%d\tCols:%d\n", iRows, iCols);
	printf("Fuse Dem ...  0%%");
	float *pFusedDemData = new float[iRows*iCols];
	double x = lfXmin, y = lfYmax;
	for (int i = 0; i < iRows; i++)
	{
		x = lfXmin;
		for (int j = 0; j < iCols; j++)
		{
			*(pFusedDemData + i*iCols + j) = GetFusedZvalue(x, y, pDemList, iDemNum, lfXmin, lfYmin, lfXmax, lfYmax);
			x += lfXgsd;
		}
		y -= lfYgsd;
		printf("\b\b\b%2d%%", i * 100 / iRows);
	}
	printf("\b\b\b100%%\n");
	printf("\n-----Save Fused Dem-----\n");
	CGdalDem *pFusedDemFile = new CGdalDem; GDALDEMHDR *pFusedDemHead = new GDALDEMHDR;
	pFusedDemHead->lfStartX = lfXmin; pFusedDemHead->lfStartY = lfYmin;
	pFusedDemHead->lfGsdX = lfXgsd; pFusedDemHead->lfGsdY = lfYgsd;
	pFusedDemHead->iCol = iCols;   pFusedDemHead->iRow = iRows;
	char strOutputfile[FILE_PN]; strcpy(strOutputfile, strInputfile);
	char *pS = strrchr(strOutputfile, '/'); if (!pS)pS = strrchr(strOutputfile, '\\');
	sprintf(pS, "%s", "\\Dem_Fused.tif");
	if (true != pFusedDemFile->CreatFile(strOutputfile, pFusedDemHead))return false;
	pFusedDemFile->WriteBlock(pFusedDemData, 0, 0, iCols, iRows);
	pFusedDemFile->MedianFilter();
	printf("Saved fused Dem file: %s\n", strOutputfile);


	//delete memory
	delete pFusedDemFile, pFusedDemHead;
	delete[]pFusedDemData;
	delete[]pDemList;
	for (int i = 0; i < iDemNum; i++)
	{
		delete[]pDemfile[i];
	}
	delete[]pDemfile;
	return true;
}