#include <stdio.h>
#include <string.h>
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/CGdalDem.hpp"
#include "ResearchCode/TypeDef.h"
int main(int argc, char**argv)
{
	if (false == NotePrint(argv, argc, 2)){ printf("Argument: Exe dem_file\n"); return false; }
	printf("Dem Info...\n");
	char strInputfile[FILE_PN]; strcpy(strInputfile, argv[1]);
	CGdalDem *pDemFile = new CGdalDem;
	GDALDEMHDR *pDemHead = new GDALDEMHDR;
	if (false == pDemFile->LoadFile(strInputfile, pDemHead))return false;
	int iCols = pDemHead->iCol, iRows = pDemHead->iRow;
	double lfGsdX = pDemHead->lfGsdX, lfGsdY = pDemHead->lfGsdY;
	double lfMinZ = pDemFile->GetMinZ(), lfMaxZ = pDemFile->GetMaxZ(), lfMeanZ = pDemFile->GetMeanZ();
	char strProj[CHAR_LEN];
	strcpy(strProj,pDemFile->GetDemProj());
	double fNoDataValue = pDemFile->GetDemNoDataValue();

	char strOutputfile[FILE_PN]; strcpy(strOutputfile, strInputfile);
	sprintf(strrchr(strOutputfile, '.'), "%s", ".info");
	FILE*fp = fopen(strOutputfile, "w");	
	fprintf(fp,"FilePN: %s\n", strInputfile);
	fprintf(fp,"Projection: %s\n", strProj);
	fprintf(fp,"Column = %d\tRow = %d\n", iCols, iRows);
	fprintf(fp,"GsdX = %lf\tGsdY = %lf\n", lfGsdX, lfGsdY);
	fprintf(fp,"MinZ = %lf\tMeanZ = %lf\tMaxZ = %lf\n", lfMinZ, lfMeanZ, lfMaxZ);
	fprintf(fp,"NoDataValue = %lf\n", fNoDataValue);
	fclose(fp);
	printf("FilePN: %s\n", strInputfile);
	printf("Projection: %s\n", strProj);
	printf("Column = %d\tRow = %d\n", iCols, iRows);
	printf("GsdX = %lf\tGsdY = %lf\n", lfGsdX, lfGsdY);
	printf("MinZ = %lf\tMeanZ = %lf\tMaxZ = %lf\n", lfMinZ, lfMeanZ, lfMaxZ);
	printf("NoDataValue = %lf\n", fNoDataValue);
	printf("End\n");
	delete pDemFile,pDemHead;
	return true;
}