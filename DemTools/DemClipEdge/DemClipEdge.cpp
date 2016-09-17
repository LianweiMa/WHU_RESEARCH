#include <stdio.h>
#include <string.h>
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/CGdalDem.hpp"
#include "ResearchCode/TypeDef.h"
int main(int argc, char**argv)
{
	if (false == NotePrint(argv, argc, 2)){ printf("Argument: Exe dem_file\n"); return false; }
	char strInputfile[FILE_PN]; strcpy(strInputfile, argv[1]);
	CGdalDem *pOldDemFile = new CGdalDem;
	GDALDEMHDR *pOldDemHead = new GDALDEMHDR;
	if (false == pOldDemFile->LoadFile(strInputfile, pOldDemHead))return false;
	float*pOldZ = new float[pOldDemHead->iCol*pOldDemHead->iRow];
	pOldDemFile->ReadBlock(pOldZ, 0, 0, pOldDemHead->iCol, pOldDemHead->iRow);

	char strOutputfile[FILE_PN]; strcpy(strOutputfile, strInputfile);
	sprintf(strrchr(strOutputfile, '.'), "%s", "_EdgeClip.tif");
	CGdalDem *pNewDemFile = new CGdalDem;
	if (false == pNewDemFile->CreatFile(strOutputfile, pOldDemHead))return false;
	pNewDemFile->SetDemNoDataValue(NODATA);
	float*pNewZ = new float[pOldDemHead->iCol*pOldDemHead->iRow];
	Memset(pNewZ, NODATA, pOldDemHead->iCol*pOldDemHead->iRow);

	for (int iteration = 1; iteration <= 20; iteration++)
	{
		for (int i = 1; i < pOldDemHead->iRow-1; i++)
		{
			for (int j = 1; j < pOldDemHead->iCol-1; j++)
			{
				float z[8] = { *(pOldZ + (i - 1)*pOldDemHead->iCol + j - 1),
					*(pOldZ + (i - 1)*pOldDemHead->iCol + j),
					*(pOldZ + (i - 1)*pOldDemHead->iCol + j + 1),
					*(pOldZ + (i)*pOldDemHead->iCol + j - 1),
					*(pOldZ + (i)*pOldDemHead->iCol + j + 1),
					*(pOldZ + (i + 1)*pOldDemHead->iCol + j - 1),
					*(pOldZ + (i + 1)*pOldDemHead->iCol + j),
					*(pOldZ + (i + 1)*pOldDemHead->iCol + j + 1),
				};
				int iNums = 0;
				for (int k = 0; k < 8; k++)
				{
					if (z[k] != NODATA)iNums++;
				}
				if (iNums == 8)*(pNewZ + i*pOldDemHead->iCol + j) = *(pOldZ + i*pOldDemHead->iCol + j);
			}
		}
		memcpy(pOldZ, pNewZ, sizeof(float)*pOldDemHead->iCol*pOldDemHead->iRow);
		Memset(pNewZ, NODATA, pOldDemHead->iCol*pOldDemHead->iRow);
	}
	pNewDemFile->WriteBlock(pOldZ, 0, 0, pOldDemHead->iCol, pOldDemHead->iRow);
	delete pNewDemFile;
	delete pOldDemFile, pOldDemHead;
	delete[]pNewZ;
	delete[]pOldZ;
	printf("Saved File: %s\n", strOutputfile);
	printf("End\n");
	return true;
}