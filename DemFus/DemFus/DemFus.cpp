#include <stdio.h>
#include <string.h>
#include "SpDem.hpp"

float GetFusedZvalue(double x,double y,CSpDem*pSpDem,int &nDemNum)//平均值法
{
	//float *pfZ_Temp = new float[nDemNum];
	float fZ_Temp = NOVALUE, fSum = 0.0; int numZ = 0;
	for (int kDemIndex = 0; kDemIndex < nDemNum; kDemIndex++)
	{
		fZ_Temp = pSpDem[kDemIndex].GetDemZValue(x, y, true);
		if (fZ_Temp != NOVALUE){ fSum += fZ_Temp; numZ++; }
	}
	if (numZ>0)return fSum / numZ;
	else return NOVALUE;
	//delete[]pfZ_Temp;
}
float GetFusedZvalue(double x, double y, CSpDem*pSpDem, int &nDemNum, double &lfXmin, double &lfYmin, double &lfXmax, double &lfYmax)//距离反向加权平均法//只适用于两度重叠的DEM融合
{
	if (nDemNum != 2)printf("Error_InputDemNUms:this method is for 2 dems\n");
	float *pfZ_Temp = new float[nDemNum]; float *pfZ_Old = pfZ_Temp;
	if (lfXmax-lfXmin<=lfYmax-lfYmin)//dX
	{
		int numZ = 0;
		for (int kDemIndex = 0; kDemIndex < nDemNum; kDemIndex++)
		{
			*pfZ_Temp = pSpDem[kDemIndex].GetDemZValue(x, y, true);
			if (*pfZ_Temp != NOVALUE){ pfZ_Temp++; numZ++; }
		}
		pfZ_Temp = pfZ_Old;
		if (numZ==2)return ((x-lfXmin)*pfZ_Temp[0]+(lfXmax-x)*pfZ_Temp[1])/(lfXmax-lfXmin);
		else if (numZ == 1)return pfZ_Temp[0];
		else return NOVALUE;
	}
	else
	{
		int numZ = 0;
		for (int kDemIndex = 0; kDemIndex < nDemNum; kDemIndex++)
		{
			*pfZ_Temp = pSpDem[kDemIndex].GetDemZValue(x, y, true);
			if (*pfZ_Temp != NOVALUE){ pfZ_Temp++; numZ++; }
		}
		pfZ_Temp = pfZ_Old;
		if (numZ == 2)return ((y - lfYmin)*pfZ_Temp[0] + (lfYmax - y)*pfZ_Temp[1]) / (lfYmax - lfYmin);
		else if (numZ == 1)return pfZ_Temp[0];
		else return NOVALUE;
	}
	delete[]pfZ_Temp;
}
int main(int argc, char**argv)
{
	printf("\n-----Strat fusDem-----\n");
	//read dem file list
	char strDemList[512]; int nDemNum = 0;//nDemNum:the number of dem file
	strcpy(strDemList, argv[1]);
	FILE *fpDemListFile = fopen(strDemList, "r"); if (!fpDemListFile){ printf("Error_FileOpen:%s\n", strDemList); return false; }
	fscanf(fpDemListFile, "%d\n", &nDemNum); if (nDemNum < 2){ printf("Error_DemFileInputNumber:%d\n", nDemNum); return false; }
	else printf("Input dem file:%d\n", nDemNum);
	char **pSDemList = new char *[nDemNum];
	for (int i = 0; i < nDemNum;i++)
	{
		pSDemList[i] = new char[512];
		fscanf(fpDemListFile, "%s\n", pSDemList[i]);		
	}
	fclose(fpDemListFile);
	
	//read dem//print dem info
	printf("\n-----Dem info-----\n");	
	CSpDem *pSpDem = new CSpDem[nDemNum]; if (!pSpDem)return false;
	double lfXmin = -99999.0, lfXmax = -99999.0, lfYmin = -99999.0, lfYmax = -99999.0;
	double lfXgsd = 0.0, lfYgsd = 0.0;
	for (int i = 0; i < nDemNum;i++)
	{
		if (!pSpDem[i].Load4File(pSDemList[i]))
		{
			printf("Error_FileOpen:%s\n", pSDemList[i]);
			return false;
		}
		printf("--%d--:%s\n", i + 1, pSDemList[i]);
		SPDEMHDR pSpDemHdr = pSpDem[i].GetDemHeader();
		printf("Xgsd:%lf\tYgsd:%lf\n", pSpDemHdr.intervalX, pSpDemHdr.intervalY);
		if (lfXgsd == 0.0)lfXgsd = pSpDemHdr.intervalX;
		if (lfYgsd == 0.0)lfYgsd = pSpDemHdr.intervalY;
		if (lfXgsd < pSpDemHdr.intervalX)lfXgsd = pSpDemHdr.intervalX;
		if (lfYgsd < pSpDemHdr.intervalY)lfYgsd = pSpDemHdr.intervalY;
		double lfZmin = NOVALUE, lfZmax = NOVALUE;
		lfZmax = pSpDem[i].GetMaxZ(); lfZmin = pSpDem[i].GetMinZ();
		printf("Zmin:%lf\tZmax:%lf\n", lfZmin, lfZmax);
		double lfX[4], lfY[4];
		pSpDem[i].GetDemRgn(lfX, lfY);
		printf("GeoRange:\n\tLT-[%lf,%lf]\n\tRT-[%lf,%lf]\n\tRB-[%lf,%lf]\n\tLB-[%lf,%lf]\n", lfX[0], lfY[0], lfX[1], lfY[1], lfX[2], lfY[2], lfX[3], lfY[3]);
		if (lfXmin == -99999) lfXmin = lfX[0];
		if (lfXmax == -99999) lfXmax = lfX[0];
		if (lfYmin == -99999) lfYmin = lfY[0];
		if (lfYmax == -99999) lfYmax = lfY[0];
		for (int i = 0; i < 4;i++)
		{
			if (lfXmin > lfX[i]) lfXmin = lfX[i];
			if (lfXmax < lfX[i]) lfXmax = lfX[i];
			if (lfYmin > lfY[i]) lfYmin = lfY[i];
			if (lfYmax < lfY[i]) lfYmax = lfY[i];
		}	
	}

	//Fus dem
	printf("\n-----Fus dem-----\n");
	int nCols=0, nRows=0;
	nCols = int((lfXmax - lfXmin) / lfXgsd) + 1; 
	nRows = int((lfYmax - lfYmin) / lfYgsd) + 1;
	printf("Rows:%d\tCols:%d\n", nRows, nCols);
	printf("Fus Dem ...  0%%");
	float *pfFusDem = new float[nRows*nCols];
	double x=lfXmin, y=lfYmin;
	for (int i = 0; i < nRows;i++)
	{
		x = lfXmin;
		for (int j = 0; j < nCols;j++)
		{
			//*(pfFusDem+i*nCols+j)=GetFusedZvalue(x, y, pSpDem, nDemNum);
			*(pfFusDem + i*nCols + j) = GetFusedZvalue(x, y, pSpDem, nDemNum,lfXmin,lfYmin,lfXmax,lfYmax);
			x += lfXgsd;		
		}
		y += lfYgsd;
		printf("\b\b\b%2d%%", i*100/nRows);
	}
	printf("\b\b\b100%%\n");
	printf("\n-----Save fusDem-----\n");
	CSpDem fusDem;
	SPDEMHDR DemHdr;
	DemHdr.startX = lfXmin; DemHdr.startY = lfYmin;
	DemHdr.intervalX = lfXgsd; DemHdr.intervalY = lfYgsd;
	DemHdr.column = nCols;   DemHdr.row = nRows;
	DemHdr.kapa = 0;
	fusDem.Attach(DemHdr, pfFusDem);
	char strFusDem[512]; strcpy(strFusDem, strDemList);
	char *pS = strrchr(strFusDem, '/'); if (!pS)pS = strrchr(strFusDem, '\\');
	sprintf(pS, "%s", "\\Dem_Fused.tif");
	if (fusDem.Save2File(strFusDem, VER_TIF))printf("Saved fusDem file: %s\n", strFusDem);
	else printf("Error_FileSaved: %s\n", strFusDem);
	printf("\n-----End fusDem-----\n");

	//delete memory
	delete[]pSpDem;
	for (int i = 0; i < nDemNum; i++)
	{
		delete[]pSDemList[i];
	}
	delete []pSDemList;

	return true;
}