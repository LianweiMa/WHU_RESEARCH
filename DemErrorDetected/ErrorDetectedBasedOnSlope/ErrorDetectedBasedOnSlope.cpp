#include<stdio.h>
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/CGdalDem.hpp"
#include "ErrorDetectedBasedOnSlope.h"
int main(int argc,char**argv){
	if (false == NotePrint(argv, argc, 2)){ printf("Argument: Exe dem_file\n"); return false; }
	printf("Loading...\n");
	char strInputfile[FILE_PN], strOutputfile[FILE_PN];
	strcpy(strInputfile, argv[1]); strcpy(strOutputfile, strInputfile);
	sprintf(strrchr(strOutputfile, '.'), "%s", "_Error_detected.tif");
	printf("File name: %s\n", strInputfile);
	CGdalDem *pDemFile = new CGdalDem;
	GDALDEMHDR *pDemHeader = new GDALDEMHDR;
	if (false == pDemFile->LoadFile(strInputfile, pDemHeader))return false;
	int iRows = pDemHeader->iRow;
	int iCols = pDemHeader->iCol;
	float *pSourceDemData = new float[iRows*iCols];
	pDemFile->ReadBlock(pSourceDemData, 0, 0, iCols, iRows);
	float *pDestDemData = new float[iRows*iCols];
	DeleteErrorOfDem(pSourceDemData, iRows, iCols, pDemHeader->lfGsdX, pDestDemData);
	//save file
	CGdalDem *pOutputDemFile = new CGdalDem;
	if (false == pOutputDemFile->CreatFile(strOutputfile, pDemHeader))return false;
	pOutputDemFile->WriteBlock(pDestDemData, 0, 0, iCols, iRows);
	printf("Saved file: %s\nOver!\n", strOutputfile);
	delete[]pSourceDemData;
	delete[]pDestDemData;
	return true;
}