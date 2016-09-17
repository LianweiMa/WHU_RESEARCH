#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gdal_1_10_1/gdal_priv.h>
#include "SpDem.hpp"
#define NOVALUE_GLOBAL_MAPPER -32767.0f
#define NOVALUE_WHU_DPGRID NOVALUE

int main(int argc, char**argv)
{
	printf("\n-----Strat Convert Dem-----\n");
	//read dem file
	char strInputDem[512];
	strcpy(strInputDem, argv[1]);
	GDALAllRegister();
	int nWidth, nHeight;
	double lfGeoTransform[6] = { 0.0 };
	GDALDataset *gdalInputDataSet = (GDALDataset*)GDALOpen(strInputDem, GA_ReadOnly);
	if (gdalInputDataSet == NULL){ printf("ERROR:\n\tOpen file failed: %s\n", strInputDem); return false; }
	nWidth = gdalInputDataSet->GetRasterXSize();
	nHeight = gdalInputDataSet->GetRasterYSize();
	gdalInputDataSet->GetGeoTransform(lfGeoTransform);
	printf("\n-----Dem info-----\n");
	printf("--%d--:%s\n", 1, strInputDem);
	printf("Xgsd:%lf\tYgsd:%lf\n", lfGeoTransform[1], -lfGeoTransform[5]);
	float  *pData = new float[nWidth*nHeight];
	float *pNewData = new float[nWidth*nHeight];
	gdalInputDataSet->RasterIO(GF_Read, 0, 0, nWidth, nHeight, pData, nWidth, nHeight, GDT_Float32, 1, 0, 0, 0, 0);
	for (int i = 0; i < nHeight;i++)
	{
		for (int j = 0; j < nWidth;j++)
		{
			if (*(pData + i*nWidth + j) == NOVALUE_GLOBAL_MAPPER)
				*(pNewData + (nHeight-i-1)*nWidth + j) = NOVALUE_WHU_DPGRID;
			else 
				*(pNewData + (nHeight - i-1)*nWidth + j) = *(pData + i*nWidth + j);
		}
	}

	CSpDem spOutputDem;
	SPDEMHDR DemHdr;
	DemHdr.startX = lfGeoTransform[0] + lfGeoTransform[1] / 2; DemHdr.startY = lfGeoTransform[3] + lfGeoTransform[5] * nHeight - lfGeoTransform[5]/2;
	DemHdr.intervalX = lfGeoTransform[1]; DemHdr.intervalY = -lfGeoTransform[5];
	DemHdr.column = nWidth;   DemHdr.row = nHeight;
	DemHdr.kapa = 0;
	spOutputDem.Attach(DemHdr, pNewData);
	char strOutputDem[512]; strcpy(strOutputDem, strInputDem);
	char *pS = strrchr(strOutputDem, '.');
	sprintf(pS, "%s", "_Sp.tif");
	if (spOutputDem.Save2File(strOutputDem, VER_TIF))printf("Saved convertDem file: %s\n", strOutputDem);
	else printf("Error_FileSaved: %s\n", strOutputDem);
	spOutputDem.Reset();
	delete[]pData;
	printf("\n-----End Convert Dem-----\n");
	return true;
}