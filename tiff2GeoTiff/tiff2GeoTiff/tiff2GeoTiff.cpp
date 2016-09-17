#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gdal_1_10_1/gdal_priv.h>
typedef  unsigned char BYTE;

int main(int argc, char**argv)
{
	printf("\n-----Strat Convert tiff to Geotiff-----\n");
	//read image file
	char strInputFile[512];strcpy(strInputFile, argv[1]);
	GDALAllRegister();
	GDALDataset *gdalInputDataSet = (GDALDataset*)GDALOpen(strInputFile, GA_ReadOnly);
	if (gdalInputDataSet == NULL){ printf("ERROR:\n\tOpen file failed: %s\n", strInputFile); return false; }

	printf("\n-----Inputfile info-----\n");
	printf("--%d--:%s\n", 1, strInputFile);
	int nWidth, nHeight, nBands;
	nWidth = gdalInputDataSet->GetRasterXSize();
	nHeight = gdalInputDataSet->GetRasterYSize();
	nBands = gdalInputDataSet->GetRasterCount();
	printf("Bands:%d\nWidth:%d\tHeight:%d\n", nBands, nWidth, nHeight);
	double lfGeoTranf[6];
	gdalInputDataSet->GetGeoTransform(lfGeoTranf);
	const char *strProjectionRef = gdalInputDataSet->GetProjectionRef();
	
	char strOutputFile[512]; strcpy(strOutputFile, argv[2]);
	GDALDataset *gdalOutputDataSet = (GDALDataset*)GDALOpen(strOutputFile, GA_Update);
	if (gdalOutputDataSet == NULL){ printf("ERROR:\n\tOpen file failed: %s\n", strOutputFile); return false; }
	gdalOutputDataSet->SetGeoTransform(lfGeoTranf);

	GDALClose(gdalInputDataSet);
	GDALClose(gdalOutputDataSet);
	return true;
}