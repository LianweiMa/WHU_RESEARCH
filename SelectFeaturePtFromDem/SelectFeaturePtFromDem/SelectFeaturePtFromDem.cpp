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
	/*float*pDemData = new float[iCols*iRows];
	pSourceDemFile->ReadBlock(pDemData, 0, 0, iCols, iRows);
	BYTE*pMarkedData = new BYTE[iCols*iRows];
	Memset(pMarkedData, 128, iCols*iRows);//山谷点标记为0山脊线标记为255
	for (int i = 0; i < iRows - 4; i++)
	{
		for (int j = 0; j < iCols - 4; j++)
		{
			ZID grid[25];
			grid[0].fZ = *(pDemData + i*iCols + j);  grid[0].iID = i*iCols + j;
			grid[1].fZ = *(pDemData + i*iCols + j + 1); grid[1].iID = i*iCols + j + 1;
			grid[2].fZ = *(pDemData + i*iCols + j + 2); grid[2].iID = i*iCols + j + 2;
			grid[3].fZ = *(pDemData + i*iCols + j + 2); grid[3].iID = i*iCols + j + 3;
			grid[4].fZ = *(pDemData + i*iCols + j + 3); grid[4].iID = i*iCols + j + 4;

			grid[5].fZ = *(pDemData + (i + 1)*iCols + j); grid[5].iID = (i + 1)*iCols + j;
			grid[6].fZ = *(pDemData + (i + 1)*iCols + j + 1); grid[6].iID = (i + 1)*iCols + j + 1;
			grid[7].fZ = *(pDemData + (i + 1)*iCols + j + 2); grid[7].iID = (i + 1)*iCols + j + 2;
			grid[8].fZ = *(pDemData + (i + 1)*iCols + j + 3); grid[8].iID = (i + 1)*iCols + j + 3;
			grid[9].fZ = *(pDemData + (i + 1)*iCols + j + 4); grid[9].iID = (i + 1)*iCols + j + 4;

			grid[10].fZ = *(pDemData + (i + 2)*iCols + j); grid[10].iID = (i + 2)*iCols + j;
			grid[11].fZ = *(pDemData + (i + 2)*iCols + j + 1); grid[11].iID = (i + 2)*iCols + j + 1;
			grid[12].fZ = *(pDemData + (i + 2)*iCols + j + 2); grid[12].iID = (i + 2)*iCols + j + 2;
			grid[13].fZ = *(pDemData + (i + 2)*iCols + j + 3); grid[13].iID = (i + 2)*iCols + j + 3;
			grid[14].fZ = *(pDemData + (i + 2)*iCols + j + 4); grid[14].iID = (i + 2)*iCols + j + 4;

			grid[15].fZ = *(pDemData + (i + 3)*iCols + j); grid[15].iID = (i + 3)*iCols + j;
			grid[16].fZ = *(pDemData + (i + 3)*iCols + j + 1); grid[16].iID = (i + 3)*iCols + j + 1;
			grid[17].fZ = *(pDemData + (i + 3)*iCols + j + 2); grid[17].iID = (i + 3)*iCols + j + 2;
			grid[18].fZ = *(pDemData + (i + 3)*iCols + j + 3); grid[18].iID = (i + 3)*iCols + j + 3;
			grid[19].fZ = *(pDemData + (i + 3)*iCols + j + 4); grid[19].iID = (i + 3)*iCols + j + 4;

			grid[20].fZ = *(pDemData + (i + 4)*iCols + j); grid[20].iID = (i + 4)*iCols + j;
			grid[21].fZ = *(pDemData + (i + 4)*iCols + j + 1); grid[21].iID = (i + 4)*iCols + j + 1;
			grid[22].fZ = *(pDemData + (i + 4)*iCols + j + 2); grid[22].iID = (i + 4)*iCols + j + 2;
			grid[23].fZ = *(pDemData + (i + 4)*iCols + j + 3); grid[23].iID = (i + 4)*iCols + j + 3;
			grid[24].fZ = *(pDemData + (i + 4)*iCols + j + 4); grid[24].iID = (i + 4)*iCols + j + 4;

			if (grid[0].fZ == NODATA || grid[1].fZ == NODATA || grid[2].fZ == NODATA || grid[3].fZ == NODATA || grid[4].fZ == NODATA ||
				grid[5].fZ == NODATA || grid[6].fZ == NODATA || grid[7].fZ == NODATA || grid[8].fZ == NODATA || grid[9].fZ == NODATA ||
				grid[10].fZ == NODATA || grid[11].fZ == NODATA || grid[12].fZ == NODATA || grid[13].fZ == NODATA || grid[14].fZ == NODATA ||
				grid[15].fZ == NODATA || grid[16].fZ == NODATA || grid[17].fZ == NODATA || grid[18].fZ == NODATA || grid[19].fZ == NODATA ||
				grid[20].fZ == NODATA || grid[21].fZ == NODATA || grid[22].fZ == NODATA || grid[23].fZ == NODATA || grid[24].fZ == NODATA)continue;
			{ ZID t; for (int m = 0; m<25; m++){ for (int n = m + 1; n<4; n++){ if (grid[m].fZ<grid[n].fZ){ t = grid[n]; grid[n] = grid[m]; grid[m] = t; } } } }
			*(pMarkedData + grid[0].iID) = 255;
			*(pMarkedData + grid[24].iID) = 0;
		}
	}
	for (int i = 0; i < iRows - 4; i++)
	{
		for (int j = 0; j < iCols - 4; j++)
		{
			BYTE grid[25];
			grid[0] = *(pMarkedData+ i*iCols + j); 
			grid[1] = *(pMarkedData+ i*iCols + j + 1);
			grid[2] = *(pMarkedData+ i*iCols + j + 2);
			grid[3] = *(pMarkedData+ i*iCols + j + 2);
			grid[4] = *(pMarkedData+ i*iCols + j + 3);
						
			grid[5] = *(pMarkedData+ (i + 1)*iCols + j);
			grid[7] = *(pMarkedData+ (i + 1)*iCols + j + 2); 
			grid[8] = *(pMarkedData+ (i + 1)*iCols + j + 3); 
			grid[9] = *(pMarkedData+ (i + 1)*iCols + j + 4); 
						
			grid[10] = *(pMarkedData + (i + 2)*iCols + j);
			grid[11] = *(pMarkedData + (i + 2)*iCols + j + 1); 
			grid[12] = *(pMarkedData + (i + 2)*iCols + j + 2); 
			grid[13] = *(pMarkedData + (i + 2)*iCols + j + 3); 
			grid[14] = *(pMarkedData + (i + 2)*iCols + j + 4); 
						
			grid[15] = *(pMarkedData + (i + 3)*iCols + j); 
			grid[16] = *(pMarkedData + (i + 3)*iCols + j + 1); 
			grid[17] = *(pMarkedData + (i + 3)*iCols + j + 2); 
			grid[18] = *(pMarkedData + (i + 3)*iCols + j + 3); 
			grid[19] = *(pMarkedData + (i + 3)*iCols + j + 4); 
						
			grid[20] = *(pMarkedData + (i + 4)*iCols + j); 
			grid[21] = *(pMarkedData + (i + 4)*iCols + j + 1);
			grid[22] = *(pMarkedData + (i + 4)*iCols + j + 2);
			grid[23] = *(pMarkedData + (i + 4)*iCols + j + 3);
			grid[24] = *(pMarkedData + (i + 4)*iCols + j + 4);

			if (grid[0] == 255 && grid[1] == 255 && grid[2] == 255 && grid[3] == 255 && grid[4] == 255 &&
				grid[5] == 255 && grid[6] == 255 && grid[7] == 255 && grid[8] == 255 && grid[9] == 255 &&
				grid[10] == 255 && grid[11] == 255 && grid[12] == 0 && grid[13] == 255 && grid[14] == 255 &&
				grid[15] == 255 && grid[16] == 255 && grid[17] == 255 && grid[18] == 255 && grid[19] == 255 &&
				grid[20] == 255 && grid[21] == 255 && grid[22] == 255 && grid[23] == 255 && grid[24] == 255)
				*(pMarkedData + (i + 2)*iCols + j + 2)=255;
			if (grid[0] == 0 && grid[1] == 0 && grid[2] == 0 && grid[3] == 0 && grid[4] == 0 &&
				grid[5] == 0 && grid[6] == 0 && grid[7] == 0 && grid[8] == 0 && grid[9] == 0 &&
				grid[10] == 0 && grid[11] == 0 && grid[12] == 255 && grid[13] == 0 && grid[14] == 0 &&
				grid[15] == 0 && grid[16] == 0 && grid[17] == 0 && grid[18] == 0 && grid[19] == 0 &&
				grid[20] == 0 && grid[21] == 0 && grid[22] == 0 && grid[23] == 0 && grid[24] == 0)
				*(pMarkedData + (i + 2)*iCols + j + 2) = 0;
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
	GDALClose(pGdalMark);*/

	int iColInterval = int(iCols / pow(iFeaturePtNums, 0.5));
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
	fclose(fp);

	//delete[]pMarkedData;
	//delete[]pDemData;
	delete pSourceDemHead, pSourceDemFile;
	return true;
}