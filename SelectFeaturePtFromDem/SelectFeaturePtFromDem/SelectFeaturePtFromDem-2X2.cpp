#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/string_operation.hpp"
#include "ResearchCode/CGdalDem.hpp"
typedef struct tagPOINT3D
{
	double lfX, lfY; float fZ;
	char strID[CHAR_LEN];
}Point3D;
typedef struct tagZID
{
	float fZ;
	int iID;
}ZID;
using namespace std;
int main(int argc, char**argv)
{
	if (false == NotePrint(argv, argc, 3)){ printf("Argument: dem_file point_number\n"); return false; }
	printf("Loading...\n");
	char strInputFile[FILE_PN];int iFeaturePtNums = 0;
	strcpy(strInputFile, argv[1]);iFeaturePtNums = atoi(argv[2]);
	//read data
	CGdalDem *pSourceDemFile = new CGdalDem; GDALDEMHDR*pSourceDemHead = new GDALDEMHDR;
	if (false == pSourceDemFile->LoadFile(strInputFile, pSourceDemHead))return false;
	int iCols = pSourceDemHead->iCol;
	int iRows = pSourceDemHead->iRow;
	float*pDemData = new float[iCols*iRows];
	pSourceDemFile->ReadBlock(pDemData, 0, 0, iCols, iRows);
	BYTE*pMarkedData = new BYTE[iCols*iRows];
	Memset(pMarkedData, 128, iCols*iRows);//山谷线标记为0，山脊线标记为255
	for (int i = 0; i < iRows - 1; i++)
	{
		for (int j = 0; j < iCols - 1; j++)
		{
			ZID grid[4];
			grid[0].fZ = *(pDemData + i*iCols + j);  grid[0].iID = i*iCols + j;
			grid[1].fZ = *(pDemData + i*iCols + j + 1); grid[1].iID = i*iCols + j + 1;
			grid[2].fZ = *(pDemData + (i + 1)*iCols + j); grid[2].iID = (i + 1)*iCols + j;
			grid[3].fZ = *(pDemData + (i + 1)*iCols + j + 1); grid[3].iID = (i + 1)*iCols + j + 1;
			if (grid[0].fZ == NODATA || grid[1].fZ == NODATA || grid[2].fZ == NODATA || grid[3].fZ == NODATA)continue;
			{ ZID t; for (int m = 0; m<4; m++){ for (int n = m + 1; n<4; n++){ if (grid[m].fZ<grid[n].fZ){ t = grid[n]; grid[n] = grid[m]; grid[m] = t; } } } }
			*(pMarkedData + grid[0].iID) = 255;
			*(pMarkedData + grid[3].iID) = 0;
		}
	}
	char strMarkFile[FILE_PN]; strcpy(strMarkFile, strInputFile); Dos2Unix(strMarkFile);
	sprintf(strrchr(strMarkFile, '.'), "%s", "_FeatPt_Marked.tif");
	GDALDriver *pDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
	GDALDataset *pGdalMark = pDriver->Create(strMarkFile, iCols, iRows, 1, GDT_Byte, NULL);
	if (!pGdalMark){ printf("ERROR_FILE_OPEN: %s\n", strMarkFile); return FALSE; }
	double lfGeo[6] = { pSourceDemHead->lfStartX, pSourceDemHead->lfGsdX, 0.0, pSourceDemHead->lfStartY + pSourceDemHead->lfGsdY*pSourceDemHead->iRow, 0.0, -pSourceDemHead->lfGsdY };
	pGdalMark->SetGeoTransform(lfGeo);
	pGdalMark->SetProjection(pSourceDemFile->GetDemProj());
	pGdalMark->RasterIO(GF_Write, 0, 0, iCols, iRows, pMarkedData, iCols, iRows, GDT_Byte, 1, NULL, 0, 0, 0);
	pGdalMark->FlushCache();
	GDALClose(pGdalMark);

	/*int iColInterval = int(iCols / pow(iFeaturePtNums, 0.5));
	int iRowInterval = int(iRows / pow(iFeaturePtNums, 0.5));
	int iPtIndex = 1;
	vector<Point3D> ptFeaturePt;
	for (int i = 0; i < pow(iFeaturePtNums, 0.5); i++){
		for (int j = 0; j < pow(iFeaturePtNums, 0.5);j++){
			Point3D ptTmp;;
			int iColIndex, iRowIndex;
			iColIndex = iColInterval / 2 + i*iColInterval;
			iRowIndex = iRowInterval / 2 + j*iRowInterval;
			pSourceDemFile->GetXYZValue(iColIndex, iRowIndex, ptTmp.lfX, ptTmp.lfY, ptTmp.fZ);
			if (ptTmp.fZ != NODATA){
				ptFeaturePt.push_back(ptTmp);
			}		
		}
	}
	//output feature points
	char strOutputFile[FILE_PN];
	strcpy(strOutputFile, strInputFile);
	sprintf(strrchr(strOutputFile, '.'), "%s", "_特征点.txt");
	FILE * fp = fopen(strOutputFile, "w");
	if (NULL == fp){ printf("File can't open: %s\n.", strOutputFile); return false; }
	fprintf(fp, "%d\n", ptFeaturePt.size());
	for (unsigned int i = 0; i < ptFeaturePt.size(); i++){
		fprintf(fp, "%d\t%lf\t%lf\t%f\n", i+1, ptFeaturePt[i].lfX, ptFeaturePt[i].lfY, ptFeaturePt[i].fZ);
	}
	printf("提取出的特征点数: %d\nOver!\n", ptFeaturePt.size());
	fclose(fp);*/

	delete[]pMarkedData;
	delete[]pDemData;
	delete pSourceDemHead, pSourceDemFile;
	return true;
}