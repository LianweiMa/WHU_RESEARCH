#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gdal_1_10_1/gdal_priv.h>
#define  BG_VAL1 -999.0f
#define BG_VAL2 -32767.0f
typedef  unsigned char BYTE;

int main(int argc, char**argv)
{
	printf("\n-----Strat Convert Background Value-----\n");
	//read image file
	char strInputFile[512];
	strcpy(strInputFile, argv[1]);
	GDALAllRegister();
	int nWidth, nHeight, nBands;
	GDALDataset *gdalInputDataSet = (GDALDataset*)GDALOpen(strInputFile, GA_ReadOnly);
	if (gdalInputDataSet == NULL){ printf("ERROR:\n\tOpen file failed: %s\n", strInputFile); return false; }
	nWidth = gdalInputDataSet->GetRasterXSize();
	nHeight = gdalInputDataSet->GetRasterYSize();
	nBands = gdalInputDataSet->GetRasterCount();
	printf("\n-----Inputfile info-----\n");
	printf("--%d--:%s\n", 1, strInputFile);
	printf("Bands:%d\nWidth:%d\tHeight:%d\n", nBands, nWidth, nHeight);
	float  *pData = new float[nWidth*nHeight*nBands];
	//gdalInputDataSet->RasterIO(GF_Read, 0, 0, nWidth, nHeight, pData, nWidth, nHeight, GDT_Byte, nBands, 0, sizeof(GDT_Byte)*nBands, sizeof(GDT_Byte)*nBands*nWidth, 0);
	gdalInputDataSet->RasterIO(GF_Read, 0, 0, nWidth, nHeight, pData, nWidth, nHeight, GDT_Float32, nBands, 0, 0, 0, 0);
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			if (*(pData + i*nWidth + j) == BG_VAL1)*(pData + i*nWidth + j) = BG_VAL2;
			///*if (*(pData + nWidth*nHeight + i*nWidth + j) == 0)*/*(pData + nWidth*nHeight + i*nWidth + j) = 255;
			///*if (*(pData + nWidth*nHeight * 2 + i*nWidth + j) == 0)*/*(pData + nWidth*nHeight*2 + i*nWidth + j) = 255;
			/*if (*(pData + nWidth*nHeight * 2 + i*nWidth + j) < 80)
				*(pData + nWidth*nHeight + i*nWidth + j) = *(pData + nWidth*nHeight * 2 + i*nWidth + j) = *(pData + i*nWidth + j) = 255;*/
		}
	}
	/*gdalInputDataSet->RasterIO(GF_Write, 0, 0, nWidth, nHeight, pData, nWidth, nHeight, GDT_Byte, nBands, 0, 0, 0, 0);
	GDALClose(gdalInputDataSet);*/
	char strOutputFile[512];
	strcpy(strOutputFile, strInputFile);
	sprintf(strrchr(strOutputFile, '.'), "%s", "_result.tif");
	GDALDriver *poDriver;
	const char *pszFormat = "GTiff";
	poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	if (poDriver == NULL) { printf("Get GDAL Driver manager failed!\n"); return 0; }
	GDALDataset *gdalOutputDataSet = poDriver->Create(strOutputFile, nWidth, nHeight, nBands, GDT_Float32, NULL);
	if (gdalOutputDataSet == NULL){ printf("ERROR:\n\tOpen file failed: %s\n", strOutputFile); return false; }
	gdalOutputDataSet->RasterIO(GF_Write, 0, 0, nWidth, nHeight, pData, nWidth, nHeight, GDT_Float32, nBands, 0, 0, 0, 0);
	GDALClose(gdalOutputDataSet);
	delete[]pData;
	return true;
}