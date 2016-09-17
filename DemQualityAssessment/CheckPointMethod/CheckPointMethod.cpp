#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/CGdalDem.hpp"
typedef struct tagPOINT3D
{
	double x, y;
	float z;
	char strID[CHAR_LEN];
}Point3D;
int main(int argc, char**argv)
{
	if (false == NotePrint(argv, argc, 3)){ printf("Argument: source_dem_file check_point_file\n"); return false; }
	printf("Loading...\n");
	char strInputfile[FILE_PN], strCheckPtfile[FILE_PN];
	strcpy(strInputfile, argv[1]); strcpy(strCheckPtfile, argv[2]);
	FILE *fp = fopen(strCheckPtfile, "r"); if (!fp){ printf("Error_FileOpen:%s\n", strCheckPtfile); return false; }
	int iCheckPtNums = 0;
	fscanf(fp, "%d\n", &iCheckPtNums);
	Point3D *pChedkPtList = new Point3D[iCheckPtNums];
	for (int i = 0; i < iCheckPtNums; i++)
	{
		fscanf(fp, "%s %lf %lf %f\n", pChedkPtList[i].strID, &pChedkPtList[i].x, &pChedkPtList[i].y, &pChedkPtList[i].z);
	}
	fclose(fp);
	char strReportFile[FILE_PN]; strcpy(strReportFile, strInputfile);
	sprintf(strrchr(strReportFile, '.'), "%s", "_Report.txt");
	fp = fopen(strReportFile, "w"); if (!fp){ printf("Error_FileWrite:%s\n", strCheckPtfile); return false; }
	CGdalDem *pSourceDem = new CGdalDem;GDALDEMHDR *pSrcDemHead = new GDALDEMHDR;
	if (false == pSourceDem->LoadFile(strInputfile, pSrcDemHead)){ return false; }
	float*pDiffZ = new float[iCheckPtNums];//g*-orth*
	Memset(pDiffZ, NODATA, iCheckPtNums);
	int iUsedCheckPtNums = 0;
	float fMean = 0.0f, fMinError = 99999.0f, fMaxError = -99999.0f;
	for (int i = 0; i < iCheckPtNums; i++)
	{
		float fDemZ = pSourceDem->GetDemZValue(pChedkPtList[i].x, pChedkPtList[i].y);
		if (fDemZ!=NODATA)
		{
			pDiffZ[i] = pChedkPtList[i].z - fDemZ; 
			fMean += pDiffZ[i];
			if (fMinError >pDiffZ[i])fMinError = pDiffZ[i];//don't use 'fabs()'
			if (fMaxError < pDiffZ[i])fMaxError = pDiffZ[i];		
			iUsedCheckPtNums++;
		}
		else pDiffZ[i] = NODATA;
		fprintf(fp, "%s\t%f\n", pChedkPtList[i].strID, pDiffZ[i]);
	}	
	fMean /= iUsedCheckPtNums;
	float fREMS = 0.0f;
	for (int i = 0; i < iCheckPtNums; i++)
	{
		if (pDiffZ[i] != NODATA)
			fREMS += pow(pDiffZ[i], 2.0f);
	}
	fREMS = pow(fREMS / iUsedCheckPtNums, 0.5f);
	printf("REMS=%f\tMeanError=%f\tMinError=%f\tMaxError=%f\n", fREMS, fMean, fMinError, fMaxError);
	fprintf(fp, "REMS=%f\tMeanError=%f\tMinError=%f\tMaxError=%f\n", fREMS, fMean, fMinError, fMaxError);
	fclose(fp);
	delete[]pDiffZ;
	delete pSrcDemHead, pSourceDem;
	return true;
}