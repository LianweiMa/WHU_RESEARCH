#include <stdio.h>
#include <string.h>
#include <math.h>
#include "SpDem.hpp"

int main(int argc, char**argv)
{
	printf("\n-----Strat Compare Dem-----\n");
	//read dem file list
	char strDemList[512]; int nDemNum = 0;//nDemNum:the number of dem file
	strcpy(strDemList, argv[1]);
	FILE *fpDemListFile = fopen(strDemList, "r"); if (!fpDemListFile){ printf("Error_FileOpen:%s\n", strDemList); return false; }
	fscanf(fpDemListFile, "%d\n", &nDemNum); if (nDemNum < 2){ printf("Error_DemFileInputNumber:%d\n", nDemNum); return false; }
	else printf("Input dem file:%d\n", nDemNum);
	char **pSDemList = new char *[nDemNum];
	for (int i = 0; i < nDemNum; i++)
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
	for (int i = 0; i < nDemNum; i++)
	{
		if (!pSpDem->Load4File(pSDemList[i]))
		{
			printf("Error_FileOpen:%s\n", pSDemList[i]);
			return false;
		}
		printf("--%d--:%s\n", i + 1, pSDemList[i]);
		SPDEMHDR pSpDemHdr = pSpDem->GetDemHeader();
		printf("Xgsd:%lf\tYgsd:%lf\n", pSpDemHdr.intervalX, pSpDemHdr.intervalY);
		if (lfXgsd == 0.0)lfXgsd = pSpDemHdr.intervalX;
		if (lfYgsd == 0.0)lfYgsd = pSpDemHdr.intervalY;
		if (lfXgsd < pSpDemHdr.intervalX)lfXgsd = pSpDemHdr.intervalX;
		if (lfYgsd < pSpDemHdr.intervalY)lfYgsd = pSpDemHdr.intervalY;
		double lfZmin = NOVALUE, lfZmax = NOVALUE;
		lfZmax = pSpDem->GetMaxZ(); lfZmin = pSpDem->GetMinZ();
		printf("Zmin:%lf\tZmax:%lf\n", lfZmin, lfZmax);
		double lfX[4], lfY[4];
		pSpDem->GetDemRgn(lfX, lfY);
		printf("GeoRange:\n\tLT-[%lf,%lf]\n\tRT-[%lf,%lf]\n\tRB-[%lf,%lf]\n\tLB-[%lf,%lf]\n", lfX[0], lfY[0], lfX[1], lfY[1], lfX[2], lfY[2], lfX[3], lfY[3]);
		if (lfXmin == -99999) lfXmin = lfX[0];
		if (lfXmax == -99999) lfXmax = lfX[0];
		if (lfYmin == -99999) lfYmin = lfY[0];
		if (lfYmax == -99999) lfYmax = lfY[0];
		for (int i = 0; i < 4; i++)
		{
			if (lfXmin > lfX[i]) lfXmin = lfX[i];                
			if (lfXmax < lfX[i]) lfXmax = lfX[i];
			if (lfYmin > lfY[i]) lfYmin = lfY[i];
			if (lfYmax < lfY[i]) lfYmax = lfY[i];
		}

	}

	//Cmpare dem
	printf("\n-----Compare dem-----\n");
	int nCols = 0, nRows = 0;
	nCols = int((lfXmax - lfXmin) / lfXgsd) + 1;
	nRows = int((lfYmax - lfYmin) / lfYgsd) + 1;
	printf("Rows:%d\tCols:%d\n", nRows, nCols);
	printf("Compare Dem ...  0%%");
	float *pfCompareDem = new float[nRows*nCols];
	double x = lfXmin, y = lfYmin;
	for (int i = 0; i < nRows; i++)
	{
		x = lfXmin;
		for (int j = 0; j < nCols; j++)
		{
			float *pfZ_Temp = new float[nDemNum]; float *pfZ_Old = pfZ_Temp;
			float fSum = 0.0; int numZ = 0;
			for (int kDemIndex = 0; kDemIndex < nDemNum; kDemIndex++)
			{

				*pfZ_Old = pSpDem->GetDemZValue(x, y, true);
				if (*pfZ_Temp != NOVALUE){ fSum += *pfZ_Temp; numZ++; pfZ_Old++; }
			}
			if (numZ > 0)
			{
				/**/float fZmean = fSum / numZ;
				float fSquareSum = 0.0;
				for (int i = 0; i < numZ;i++)
				{
					fSquareSum += pow(pfZ_Temp[i] - fSum, 2.0f);
				}
				*(pfCompareDem + i*nCols + j) = pow(fSquareSum / numZ, 0.5f);
			}
			else *(pfCompareDem + i*nCols + j) = NOVALUE;
			x += lfXgsd;
			delete[]pfZ_Temp;
		}
		y += lfYgsd;
		printf("\b\b\b\b%2d0%%", i * 100 / nRows);
	}
	printf("\b\b\b\b100%%\n");
	printf("\n-----Save compareDem-----\n");
	CSpDem compareDem;
	SPDEMHDR DemHdr;
	DemHdr.startX = lfXmin; DemHdr.startY = lfYmin;
	DemHdr.intervalX = lfXgsd; DemHdr.intervalY = lfYgsd;
	DemHdr.column = nCols;   DemHdr.row = nRows;
	DemHdr.kapa = 0;
	compareDem.Attach(DemHdr, pfCompareDem);
	char strCompareDem[512]; strcpy(strCompareDem, strDemList);
	char *pS = strrchr(strCompareDem, '.');
	sprintf(pS, "%s", "_Compared.tif");
	if (compareDem.Save2File(strCompareDem, VER_TIF))printf("Saved fusDem file: %s\n", strCompareDem);
	else printf("Error_FileSaved: %s\n", strCompareDem);
	printf("\n-----End compareDem-----\n");

	//delete memory
	delete[]pSpDem;
	for (int i = 0; i < nDemNum; i++)
	{
		delete[]pSDemList[i];
	}
	delete[]pSDemList;

	return true;
}