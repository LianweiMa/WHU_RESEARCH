// DemMap.cpp : Defines the entry point for the console application.
//

#include <stdlib.h>
#include "DemMap.h"

void Usage(){
	printf(
		"\n%s",
		"Usage: DemMap <DemListFile> <gcdpath> \n"
		"Opt:\n"
		"DemListFile:the full path of file which records the dem files path.\n"
		);

	exit(1);
}

int main(int argc, char* argv[])
{
	if (argc != 3)Usage();
	const char* lpstrDemListFile = NULL;
	const char* lpstrGcdPath = NULL;
	char strMergeDem[512];	memset(strMergeDem, 0, sizeof(strMergeDem));
	char strOutPath[512];	memset(strOutPath, 0, sizeof(strOutPath));
	
	
	lpstrDemListFile = argv[1];
	lpstrGcdPath = argv[2];

	CDemMap demMap;

	if (!strMergeDem[0]) {
		strcpy(strMergeDem, lpstrDemListFile);
		char* pS = strrchr(strMergeDem, '.');	if (!pS) pS = strMergeDem + strlen(strMergeDem);
		strcpy(pS, "_merge.tif");
	}
	if (!strOutPath[0]){
		strcpy(strOutPath, lpstrDemListFile);
		char *pS=strrchr(strOutPath,'/');
		if (!pS)pS = strrchr(strOutPath, '\\');
		*pS = 0;
	}

	int ret;
	demMap.SetGSD(25, 25, true);
	demMap.SetMetaMode(0);	demMap.SetSRTMMode(false);
	demMap.SetOutputPath(strOutPath);

	demMap.LoadGcdFile(lpstrGcdPath);

	demMap.Load4DemListFile(lpstrDemListFile);

	
	double xy[4];
	demMap.GetAreaGeoRange_degree(xy, xy + 1, xy + 2, xy + 3);
	printf("Area:\t[%lf,%lf]<=>[%lf,%lf]\n", xy[0], xy[1], xy[2], xy[3]);

	CWuSRTM locSrtm;
	CWuGlbHei srtmHei;
	printf("Start Getting SRTM for The Area...\n");
	int nRows, nCols;
	nRows = int((xy[3] - xy[1]) / SRTM_GSD) + 1;	nCols = int((xy[2] - xy[0]) / SRTM_GSD) + 1;
	float *pLocZ = new float[nRows*nCols];
	srtmHei.GetSRTM(xy[0], xy[1], nCols, nRows, pLocZ);
	locSrtm.Init(xy[0], xy[1], nCols, nRows, pLocZ);
	delete[] pLocZ;
	printf("End Getting SRTM\n");


	demMap.MergeDem(strMergeDem, X_GSD, Y_GSD, &locSrtm);

	return true;
}