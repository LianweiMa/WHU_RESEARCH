#include <stdio.h>
#include <string.h>
#include "gdal_1_10_1/gdal_priv.h"
#include "mlw_1_0_0/type_define.h"
#include "mlw_1_0_0/sting_operation.hpp"
void Usage(){
	printf(
		"\n%s",
		"\tUsage: 16Bit28Bit <InputFile> [Output directory]\n"
		);
	exit(1);
}
int main(int argc, char**argv)
{
	if (argc != 2 && argc != 3) { Usage(); return false; }
	printf("\n-----Start 16Bit to 8Bit-----\n");
	//read image file
	char strInputFile[512];strcpy(strInputFile, argv[1]);
	GDALAllRegister();
	int iCols, iRows, iBands,iBits;
	GDALDataset *gdalInputDataSet = (GDALDataset*)GDALOpen(strInputFile, GA_ReadOnly);
	if (gdalInputDataSet == NULL){ printf("ERROR:\n\tOpen file failed: %s\n", strInputFile); return false; }
	iCols = gdalInputDataSet->GetRasterXSize();
	iRows = gdalInputDataSet->GetRasterYSize();
	iBands = gdalInputDataSet->GetRasterCount();
	iBits = gdalInputDataSet->GetRasterBand(1)->GetRasterDataType();
	//image info
	printf("\n-----Inputfile info-----\n");
	printf("--Open--:%s\n", strInputFile);
	printf("Bands:%d\nWidth:%d\tHeight:%d\tBits:%d\n", iBands, iCols, iRows, iBits * 8);
	if (iBits == 1){ printf("WARNING:\n\tThe image is already 8 bit: %s\n", strInputFile); return false; }

	printf("converting...");
	char strOutputFile[512];
	if (argc==3)
	{
		strcpy(strOutputFile, argv[2]);
		AddEndSlash(strOutputFile);
		sprintf(strOutputFile + strlen(strOutputFile), "%s", GetFileName(strInputFile));
	}
	else
	{
		strcpy(strOutputFile, strInputFile);
	}
	sprintf(strrchr(strOutputFile, '.'), "%s", "_8.tif");
	
	GDALDriver *poDriver;
	const char *pszFormat = "GTiff";
	poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	if (poDriver == NULL) { printf("ERROR:Get GDAL Driver manager failed!\n"); return 0; }
	GDALDataset *gdalOutputDataSet = poDriver->Create(strOutputFile, iCols, iRows, iBands, GDT_Byte, NULL);
	if (gdalOutputDataSet == NULL){ printf("ERROR:\n\tOpen file failed: %s\n", strOutputFile); return false; }
	//write geotransform//write projection
	double lfGeoTrans[6]; gdalInputDataSet->GetGeoTransform(lfGeoTrans);
	gdalOutputDataSet->SetGeoTransform(lfGeoTrans);
	char strProjection[MAX_FILE_NAME_LENGTH_MLW];
	strcpy(strProjection,gdalInputDataSet->GetProjectionRef());
	gdalOutputDataSet->SetProjection(strProjection);
	int iProgress=0;
	printf("%3d%%", 0);
	//Block processing
	int iStep = BlOCK_MEMERY / (iCols*iBits);;
	int iStep_Height = iRows / iStep;//times
	if (iRows%iStep)iStep_Height++;
	int step_length = iBands*iStep/10+1;
	int step_curr = 0;
	for (int iIndex = 1; iIndex <= iBands;iIndex++)
	{
		//printf("Calculating histogram...");
		double lfMValue[2];
		gdalInputDataSet->GetRasterBand(iBands)->ComputeRasterMinMax(false, lfMValue);
		int iBuckets = int(lfMValue[1] - lfMValue[0]);
		int *pHistogram = new int[iBuckets];
		gdalInputDataSet->GetRasterBand(iBands)->GetHistogram(lfMValue[0] - 0.5, lfMValue[1] + 0.5, iBuckets, pHistogram, false, false, NULL, NULL);
		//printf("Finished.");
		double lfMin_16 = lfMValue[1], lfMax_16 = lfMValue[0];
		//min max statistic
		int iSumTmp = 0;
		for (int i = 0; i < iBuckets; i++)
		{
			iSumTmp += pHistogram[i];
			if (iSumTmp / (iRows*iCols*1.0) >= 0.001)
			{
				lfMin_16 = i + lfMValue[0];
				break;
			}
		}
		iSumTmp = 0;
		for (int i = iBuckets - 1; i >= 0; i--)
		{
			iSumTmp += pHistogram[i];
			if (iSumTmp / (iRows*iCols*1.0) >= 0.001)
			{
				lfMax_16 = i + lfMValue[0];
				break;
			}
		}

		//converting
		for (int i = 0; i < iStep_Height; i++)
		{
			int iBlockH = iStep;
			if ((i+1)*iStep>iRows)iBlockH = iRows - (i+1)*iStep;
			WORD *pUintOriBlock_16 = new WORD[iBlockH*iCols];
			memset(pUintOriBlock_16, 0, sizeof(WORD)*iBlockH*iCols);
			BYTE *pByteOriBlock_8 = new BYTE[iBlockH*iCols];
			memset(pByteOriBlock_8, 0, sizeof(BYTE)*iBlockH*iCols);
			int *panBandMap = &iIndex;
			gdalInputDataSet->RasterIO(GF_Read, 0, i*iStep, iCols, iBlockH, pUintOriBlock_16, iCols, iBlockH, GDT_UInt16, 1, panBandMap, 0, 0, 0);
			unsigned int uTmp;
			BYTE byteTmp;
			for (int i_Block = 0; i_Block < iBlockH; i_Block++)
			{
				for (int j_Block = 0; j_Block < iCols; j_Block++)
				{
					uTmp = *(pUintOriBlock_16 + i_Block*iCols + j_Block);
					byteTmp = ((uTmp - lfMValue[0]) / (lfMValue[1] - lfMValue[0])) * 255;
					*(pByteOriBlock_8 + i_Block*iCols + j_Block) = byteTmp;
				}
			}
			gdalOutputDataSet->RasterIO(GF_Write, 0, i*iStep, iCols, iBlockH, pByteOriBlock_8, iCols, iBlockH, GDT_Byte, 1, panBandMap, 0, 0, 0);
			gdalOutputDataSet->FlushCache();
			delete[]pUintOriBlock_16;
			delete[]pByteOriBlock_8;
			iProgress++;
			if (iProgress > step_length)
			{
				step_curr++;
				iProgress = 0;
				printf("\b\b\b\b%2d0%%", step_curr);
			}
		}
		delete[]pHistogram;
	}
	printf("\b\b\b\b%3d%%\n", 100);
	printf("--Saved--:%s\n", strOutputFile);
	GDALClose(gdalInputDataSet);
	GDALClose(gdalOutputDataSet);
	printf("\n-----End 16Bit to 8Bit-----\n");
	return true;
}