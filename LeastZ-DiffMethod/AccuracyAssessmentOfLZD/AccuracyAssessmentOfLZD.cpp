#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/CGdalDem.hpp"
int main(int argc, char**argv)
{
	if (false == NotePrint(argv, argc, 2)){ printf("Argument: exe z_difference_dem_file\n"); return false; }
	printf("Loading...\n");
	char strInputfile[FILE_PN];
	strcpy(strInputfile, argv[1]);
	CGdalDem *pDiffZDem = new CGdalDem;
	GDALDEMHDR *pDiffZDemHead = new GDALDEMHDR;
	if (false == pDiffZDem->LoadFile(strInputfile, pDiffZDemHead)){ return false; }
	float*pDiffZ = new float[pDiffZDemHead->iRow*pDiffZDemHead->iCol];
	pDiffZDem->ReadBlock(pDiffZ, 0, 0, pDiffZDemHead->iCol, pDiffZDemHead->iRow);
	double lfSum = 0.0;
	float fMean = 0.0f, fMinError = 99999.0f, fMaxError = -99999.0f, fREMS = 0.0f;
	int iNums = 0;
	for (int i = 0; i < pDiffZDemHead->iRow; i++)
	{
		for (int j = 0; j < pDiffZDemHead->iCol; j++)
		{
			if (*(pDiffZ + i*pDiffZDemHead->iCol + j) != NODATA)
			{
				iNums++;
				fREMS += pow(*(pDiffZ + i*pDiffZDemHead->iCol + j) - fMean, 2.0f);
				lfSum += *(pDiffZ + i*pDiffZDemHead->iCol + j);
				if (fMinError >(*(pDiffZ + i*pDiffZDemHead->iCol + j)))fMinError = (*(pDiffZ + i*pDiffZDemHead->iCol + j));//don't use 'fabs()'
				if (fMaxError < (*(pDiffZ + i*pDiffZDemHead->iCol + j)))fMaxError = (*(pDiffZ + i*pDiffZDemHead->iCol + j));
			}
		}
	}

	fMean = float(lfSum / iNums);
	fREMS = pow(fREMS /= iNums, 0.5f);
	printf("REMS=%f\tMinError=%f\tMaxError=%f\n", fREMS, fMinError, fMaxError);
	delete[]pDiffZ;
	delete pDiffZDemHead, pDiffZDem;
	return true;
}