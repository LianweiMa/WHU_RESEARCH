#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include "mlw_1_0_0/type_define.h"
#include "wu/WuDem.h"
using namespace std;
int main(int argc, char**argv)
{
	if (argc != 3){
		{printf("Wrong argument!\ndem_file point_number\n"); return false; }
	}{
		for (int i = 1; i < argc; i++){
			printf("argv[%d]: %s\n", i, argv[i]);
		}
	}
	char strInputFile[FILE_PN];
	int iFeaturePtNums = 0.0;
	strcpy(strInputFile, argv[1]);//第1个参数是待定向的DEM文件
	iFeaturePtNums = atoi(argv[2]);
	/************************************************************************/
	/* 读取并输出数据                                                             */
	/************************************************************************/
	char strOutputFile[FILE_PN];
	strcpy(strOutputFile,strInputFile);
	sprintf(strrchr(strOutputFile, '.'), "%s", "_特征点.txt");
	FILE * fp = fopen(strOutputFile, "w");
	if (NULL == fp){ printf("File can't open: %s\n.", strOutputFile); return false; }
	fprintf(fp, "%d\n", iFeaturePtNums);
	CWuDem demSourceDem;
	WUDEMHDR headSourceDemHdr;
	demSourceDem.Load4File(strInputFile);
	headSourceDemHdr = demSourceDem.GetDemHeader();
	int iCols = headSourceDemHdr.column;
	int iRows = headSourceDemHdr.row;
	int iColInterval = iCols / pow(iFeaturePtNums, 0.5);
	int iRowInterval = iRows / pow(iFeaturePtNums, 0.5);
	int iPtIndex = 1;
	for (int i = 0; i < pow(iFeaturePtNums, 0.5); i++){
		for (int j = 0; j < pow(iFeaturePtNums, 0.5);j++){
			double lfX, lfY, lfZ;
			int iColIndex, iRowIndex;
			iColIndex = iColInterval / 2 + i*iColInterval;
			iRowIndex = iRowInterval / 2 + j*iRowInterval;
			demSourceDem.GetGlobalXYZValue(iColIndex, iRowIndex, &lfX, &lfY, &lfZ);
			if (lfZ==NOVALUE){
				if (iColIndex<iCols/2&&iRowIndex<iRows/2){
					while (iRows / 2-iRowIndex++)
					{
						demSourceDem.GetGlobalXYZValue(iColIndex, iRowIndex, &lfX, &lfY, &lfZ);
						if (lfZ != NOVALUE){ demSourceDem.GetGlobalXYZValue(iColIndex, iRowIndex+10, &lfX, &lfY, &lfZ); break; }
					}
				}
				if (iColIndex < iCols / 2 && iRowIndex > iRows / 2){
					while (iCols / 2-iColIndex++)
					{
						demSourceDem.GetGlobalXYZValue(iColIndex, iRowIndex, &lfX, &lfY, &lfZ);
						if (lfZ != NOVALUE){ demSourceDem.GetGlobalXYZValue(iColIndex+10, iRowIndex, &lfX, &lfY, &lfZ); break; }
					}
				}
				if (iColIndex > iCols / 2 && iRowIndex > iRows / 2){
					while (iRowIndex-- - iRows / 2)
					{
						demSourceDem.GetGlobalXYZValue(iColIndex, iRowIndex, &lfX, &lfY, &lfZ);
						if (lfZ != NOVALUE){ demSourceDem.GetGlobalXYZValue(iColIndex, iRowIndex-10, &lfX, &lfY, &lfZ); break; }
					}
				}
				if (iColIndex > iCols / 2 && iRowIndex < iRows / 2){
					while (iColIndex-- - iCols / 2)
					{
						demSourceDem.GetGlobalXYZValue(iColIndex, iRowIndex, &lfX, &lfY, &lfZ);
						if (lfZ != NOVALUE){ demSourceDem.GetGlobalXYZValue(iColIndex-10, iRowIndex, &lfX, &lfY, &lfZ); break; }
					}
				}
			}
			fprintf(fp, "%d\t%lf\t%lf\t%lf\n", iPtIndex++, lfX, lfY, lfZ);
		}

	}
	fclose(fp);
	return true;
}