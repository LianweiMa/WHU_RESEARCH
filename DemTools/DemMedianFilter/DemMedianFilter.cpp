#include <stdio.h>
#include <string.h>
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/CGdalDem.hpp"
#include "ResearchCode/TypeDef.h"
int main(int argc, char**argv)
{
	if (false == NotePrint(argv, argc, 2)){ printf("Argument: Exe dem_file\n"); return false; }
	printf("Filtering...\n");
	char strInputfile[FILE_PN]; strcpy(strInputfile, argv[1]);
	CGdalDem *pOldDemFile = new CGdalDem;
	GDALDEMHDR *pOldDemHead = new GDALDEMHDR;
	if (false == pOldDemFile->LoadFile(strInputfile, pOldDemHead))return false;
	float*pOldZ = new float[pOldDemHead->iCol*pOldDemHead->iRow];
	pOldDemFile->ReadBlock(pOldZ, 0, 0, pOldDemHead->iCol, pOldDemHead->iRow);
	int iCols = pOldDemHead->iCol, iRows = pOldDemHead->iRow;
	char strOutputfile[FILE_PN]; strcpy(strOutputfile, strInputfile);
	sprintf(strrchr(strOutputfile, '.'), "%s", "_MedianFiltering.tif");
	CGdalDem *pNewDemFile = new CGdalDem;
	if (false == pNewDemFile->CreatFile(strOutputfile, pOldDemHead))return false;
	pNewDemFile->SetDemNoDataValue(NODATA);
	float*pNewZ = new float[pOldDemHead->iCol*pOldDemHead->iRow];
	memcpy(pNewZ, pOldZ, pOldDemHead->iCol*pOldDemHead->iRow*sizeof(float));

	for (int i = 0; i < iRows-4; i++)
	{
		for (int j = 0; j <  iCols- 4; j++)
		{
			float grid[25];
			grid[0] = *(pOldZ + i*iCols + j);  
			grid[1] = *(pOldZ + i*iCols + j + 1); 
			grid[2] = *(pOldZ + i*iCols + j + 2); 
			grid[3] = *(pOldZ + i*iCols + j + 2); 
			grid[4] = *(pOldZ + i*iCols + j + 3); 

			grid[5] = *(pOldZ + (i + 1)*iCols + j); 
			grid[6] = *(pOldZ + (i + 1)*iCols + j + 1); 
			grid[7] = *(pOldZ + (i + 1)*iCols + j + 2); 
			grid[8] = *(pOldZ + (i + 1)*iCols + j + 3); 
			grid[9] = *(pOldZ + (i + 1)*iCols + j + 4); 

			grid[10] = *(pOldZ + (i + 2)*iCols + j); 
			grid[11] = *(pOldZ + (i + 2)*iCols + j + 1); 
			grid[12] = *(pOldZ + (i + 2)*iCols + j + 2); 
			grid[13] = *(pOldZ + (i + 2)*iCols + j + 3); 
			grid[14] = *(pOldZ + (i + 2)*iCols + j + 4); 

			grid[15] = *(pOldZ + (i + 3)*iCols + j); 
			grid[16] = *(pOldZ + (i + 3)*iCols + j + 1); 
			grid[17] = *(pOldZ + (i + 3)*iCols + j + 2); 
			grid[18] = *(pOldZ + (i + 3)*iCols + j + 3); 
			grid[19] = *(pOldZ + (i + 3)*iCols + j + 4); 

			grid[20] = *(pOldZ + (i + 4)*iCols + j);
			grid[21] = *(pOldZ + (i + 4)*iCols + j + 1); 
			grid[22] = *(pOldZ + (i + 4)*iCols + j + 2); 
			grid[23] = *(pOldZ + (i + 4)*iCols + j + 3); 
			grid[24] = *(pOldZ + (i + 4)*iCols + j + 4); 

			SORTF(grid, 25); int k = 0; for (k = 0; k < 25; k++){ if (grid[k] == NODATA) break; }
			*(pNewZ + (i + 2)*iCols + j + 2) = grid[k / 2];
		}
	}
	pNewDemFile->WriteBlock(pNewZ, 0, 0, iCols, iRows);
	delete pNewDemFile;
	delete pOldDemFile, pOldDemHead;
	delete[]pNewZ;
	delete[]pOldZ;
	printf("Saved File: %s\n", strOutputfile);
	printf("End\n");
	return true;
}