#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gdal_1_10_1/gdal_priv.h>
typedef unsigned char BYTE;
#define BLOCK 512

int main(int argc, char**argv)
{
	printf("\n-----Strat image binary-----\n");
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
	double lfGeoTrans[6] = { 0.0 };
	gdalInputDataSet->GetGeoTransform(lfGeoTrans);
	printf("\n-----Inputfile info-----\n");
	printf("--%d--:%s\n", 1, strInputFile);
	printf("Bands:%d\nWidth:%d\tHeight:%d\n", nBands, nWidth, nHeight);
	
	char strOutputFile[512];
	strcpy(strOutputFile, strInputFile);
	sprintf(strrchr(strOutputFile, '.'), "%s", "_result.tif");
	GDALDriver *poDriver;
	const char *pszFormat = "GTiff";
	poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	if (poDriver == NULL) { printf("Get GDAL Driver manager failed!\n"); return 0; }
	GDALDataset *gdalOutputDataSet = poDriver->Create(strOutputFile, nWidth, nHeight, nBands, GDT_Byte, NULL);
	if (gdalOutputDataSet == NULL){ printf("ERROR:\n\tOpen file failed: %s\n", strOutputFile); return false; }
	gdalOutputDataSet->SetGeoTransform(lfGeoTrans);
	for (int i = 0; i < nHeight; i += BLOCK)
	{
		int num = 0;
		for (int j = 0; j < nWidth; j += BLOCK)
		{
			int Block_Height = 0, Block_Width = 0;
			float *pData = NULL;
			BYTE *pNewData = NULL;
			if (i+BLOCK <= nHeight)
			{
				if (j+BLOCK <= nWidth)
				{
					pData = new float[BLOCK*BLOCK];
					Block_Height = BLOCK;
					Block_Width = BLOCK;
					pNewData = new BYTE[BLOCK*BLOCK];
					memset(pNewData, 0, sizeof(BYTE)*BLOCK*BLOCK);
				}
				else
				{
					pData = new float[BLOCK*(nWidth - j)];
					Block_Height = BLOCK;
					Block_Width = (nWidth - j);
					pNewData = new BYTE[BLOCK*(nWidth - j)];
					memset(pNewData, 0, sizeof(BYTE)*BLOCK*(nWidth - j));
				}
			}
			else
			{
				if (j + BLOCK <= nWidth)
				{
					pData = new float[(nHeight - i)*BLOCK];
					Block_Height = (nHeight - i);
					Block_Width = BLOCK;
					pNewData = new BYTE[(nHeight - i)*BLOCK];
					memset(pNewData, 0, sizeof(BYTE)*(nHeight - i)*BLOCK);
				}
				else
				{
					pData = new float[(nHeight - i)*(nWidth - j)];
					Block_Height = (nHeight - i);
					Block_Width = (nWidth - j);
					pNewData = new BYTE[(nHeight - i)*(nWidth - j)];
					memset(pNewData, 0, sizeof(BYTE)*(nHeight - i)*(nWidth - j));
				}
			}

			gdalInputDataSet->RasterIO(GF_Read, j, i, Block_Width, Block_Height, pData, Block_Width, Block_Height, GDT_Float32, 1, 0, 0, 0, 0);
			for (int i_Block = 0; i_Block < Block_Height; i_Block++)
			{
				for (int j_Block = 0; j_Block < Block_Width; j_Block++)
				{
					if (*(pData + i_Block*Block_Width + j_Block) == 0)
						*(pNewData + i_Block*Block_Width + j_Block) = 255;
				}
			}
			gdalOutputDataSet->RasterIO(GF_Write, j, i, Block_Width, Block_Height, pNewData, Block_Width, Block_Height, GDT_Byte, 1, 0, 0, 0, 0);
			delete[]pData;
			delete[]pNewData;
			num++;
		}
	}
				
			
	GDALClose(gdalInputDataSet);
	GDALClose(gdalOutputDataSet);

	return true;
}