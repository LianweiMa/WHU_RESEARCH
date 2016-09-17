#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/CGdalDem.hpp"
#include "ResearchCode/TypeDef.h"
#include "Whu/WuDem.h"
int main(int argc, char**argv)
{
	if (false == NotePrint(argv, argc, 3)){ printf("Argument: Exe dem_file new_novalue\n"); return false; }
	char strInputfile[FILE_PN]; strcpy(strInputfile, argv[1]);
	double lfNewNoValue = atof(argv[2]);
	CGdalDem *pOldDemFile = new CGdalDem;
	GDALDEMHDR *pOldDemHead = new GDALDEMHDR;
	if (false == pOldDemFile->LoadFile(strInputfile, pOldDemHead))return false;
	printf("Current NoValue£º %lf\n", pOldDemFile->GetDemNoDataValue());
	float*pOldZ = new float[pOldDemHead->iCol*pOldDemHead->iRow];
	pOldDemFile->ReadBlock(pOldZ, 0, 0, pOldDemHead->iCol, pOldDemHead->iRow);
	char strOutputfile[FILE_PN]; strcpy(strOutputfile, strInputfile);
	sprintf(strrchr(strOutputfile, '.'), "_%.0lf%s", lfNewNoValue,".tif");
	CGdalDem *pNewDemFile = new CGdalDem;
	if (false == pNewDemFile->CreatFile(strOutputfile, pOldDemHead))return false;
	for (int i = 0; i < pOldDemHead->iRow; i++)
	{
		for (int j = 0; j < pOldDemHead->iCol; j++)
		{
			if (*(pOldZ + i*pOldDemHead->iCol + j) == pOldDemFile->GetDemNoDataValue() || 
				(*(pOldZ + i*pOldDemHead->iCol + j) == -99999)){
				*(pOldZ + i*pOldDemHead->iCol + j) = lfNewNoValue;
			}
		}
	}
	pNewDemFile->SetDemNoDataValue(lfNewNoValue);
	pNewDemFile->WriteBlock(pOldZ, 0, 0, pOldDemHead->iCol, pOldDemHead->iRow);
	printf("New NoValue: %lf\n", lfNewNoValue);
	printf("Saved File: %s\n", strOutputfile);
	printf("End\n");
/*
	CWuDem wuDem;
	WUDEMHDR wuDemHead;
	wuDemHead.column = pOldDemHead->iCol;
	wuDemHead.row = pOldDemHead->iRow;
	wuDemHead.intervalX = pOldDemHead->lfGsdX;
	wuDemHead.intervalY = pOldDemHead->lfGsdY;
	wuDemHead.kapa = 0;
	wuDemHead.startX = pOldDemHead->lfStartX;
	wuDemHead.startY = pOldDemHead->lfStartY;
	wuDem.Attach(wuDemHead, pOldZ);
	wuDem.Save2File(strOutputfile);
	wuDem.Detach();
	wuDem.Close();*/
	delete pNewDemFile;
	delete pOldDemFile;
	delete pOldDemHead;
	delete[]pOldZ;

	return true;
}