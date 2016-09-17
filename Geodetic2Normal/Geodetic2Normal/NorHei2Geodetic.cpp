#include <stdio.h>
#include <string.h>
#include "gdal_1_10_1/gdal_priv.h"
#include "mlw_1_0_0/type_define.h"
#include "sp_1_0_0/WuEgm.h"

void Usage(){
	printf("Usage:\n\tNorHei2Geodetic.exe dem_file or control_file\ndem_file:*.tif\ncontrol_file:*.txt\tformat:first line:GCP number,second line:ID X Y Z\nnote:The coordinate system is only fit for longitude and latitude.\n");
}
int main(int argc, char**argv)
{
	if (argc != 2){ Usage(); return false; }
	else{
		for (int i = 0; i < argc;i++)
		{
			printf("argument %d: %s\n", i + 1, argv[i]);
		}
	}
	printf("-----Normal to Geodetic height-----\n");
	char strInputFile[FILE_PN]; strcpy(strInputFile, argv[1]);
	char *pS = strrchr(strInputFile, '.');
	char strOutputFile[FILE_PN]; strcpy(strOutputFile, strInputFile);
	if (strcmp(pS, ".txt") == 0){
		pS = strrchr(strOutputFile, '.'); sprintf(pS, "%s", "_GeodeticHei.txt");
		FILE *fpInputFile = fopen(strInputFile, "r"); 
		if (fpInputFile == NULL){printf("ERROR_FILE_OPEN: %s\n", strInputFile); return FALSE;}
		int iGcpNums=0;
		fscanf(fpInputFile, "%d\n", &iGcpNums);
		CWuEgm WuEgm;
		FILE *fpOutputFile = fopen(strOutputFile, "w");
		if (fpOutputFile == NULL){ printf("ERROR_FILE_OPEN: %s\n", strOutputFile); return FALSE; }
		fprintf(fpOutputFile, "%d\n", iGcpNums);
		for (int i = 0; i < iGcpNums; i++){
			char strID[FILE_PN];
			double lfGeoX = 0.0, lfGeoY = 0.0;
			float fGeoZ=0.0f;
			fscanf(fpInputFile, "%s %lf %lf %f\n", strID,&lfGeoX,&lfGeoY,&fGeoZ);
			fGeoZ+=WuEgm.GetGVal(lfGeoX, lfGeoY);
			fprintf(fpOutputFile, "%s %lf %lf %f\n",strID, lfGeoX, lfGeoY, fGeoZ);
		}
		fclose(fpInputFile);
		fclose(fpOutputFile);
	}
	else{	
		pS = strrchr(strOutputFile, '.'); sprintf(pS, "%s", "_GeodeticHei.tif");
		GDALAllRegister();
		GDALDataset *pGdalData = (GDALDataset *)GDALOpen(strInputFile, GA_ReadOnly);
		if (!pGdalData){ printf("ERROR_FILE_OPEN: %s\n", strInputFile); return FALSE; }
		int iDemWidth = pGdalData->GetRasterXSize();
		int iDemHeight = pGdalData->GetRasterYSize();
		double lfGeoTransform[6] = { 0.0 }; pGdalData->GetGeoTransform(lfGeoTransform);
		const char*pszSrcWKT = pGdalData->GetProjectionRef();

		GDALDriver *pDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
		if (!pDriver){ printf("ERROR_GET_DRIVER: GTiff"); return FALSE; }
		GDALDataset *pGdalNorData = pDriver->Create(strOutputFile, iDemWidth, iDemHeight, 1, GDT_Float32, NULL);
		pGdalNorData->SetGeoTransform(lfGeoTransform);
		pGdalNorData->SetProjection(pszSrcWKT);

		CWuEgm WuEgm;
		int iBlockHeight = BlOCK_MEMERY / (iDemWidth*sizeof(float));
		int iStep = iDemHeight / iBlockHeight;
		if (iDemHeight%iBlockHeight)iStep++;
		int yBeg = 0, yEnd = 0;
		//get geo info
		double lfGeoX = 0.0, lfGeoY = 0.0;
		printf("Progress: %d / %d\n", 0, iDemHeight);
		for (int i = 0; i < iStep; i++)
		{
			yBeg = i*iBlockHeight;
			if ((i + 1)*iBlockHeight > iDemHeight)yEnd = iDemHeight;
			else yEnd = (i + 1)*iBlockHeight;
			float *pData = new float[(yEnd - yBeg)*iDemWidth];
			memset(pData, 0, (yEnd - yBeg)*iDemWidth*sizeof(float));
			pGdalData->RasterIO(GF_Read, 0, yBeg, iDemWidth, yEnd - yBeg, pData, iDemWidth, yEnd - yBeg, GDT_Float32, 1, NULL, 0, 0, 0);
			for (int y = yBeg; y < yEnd; y++)
			{
				for (int x = 0; x < iDemWidth; x++)
				{
					lfGeoX = lfGeoTransform[0] + x * lfGeoTransform[1] + y * lfGeoTransform[2];
					lfGeoY = lfGeoTransform[3] + x * lfGeoTransform[4] + y * lfGeoTransform[5];
					if (*(pData + (y - yBeg)*iDemWidth + x) != NOVALUE_MLW)
						*(pData + (y - yBeg)*iDemWidth + x) += WuEgm.GetGVal(lfGeoX, lfGeoY);
				}
			}
			pGdalNorData->RasterIO(GF_Write, 0, yBeg, iDemWidth, yEnd - yBeg, pData, iDemWidth, yEnd - yBeg, GDT_Float32, 1, NULL, 0, 0, 0);
			GDALFlushCache(pGdalNorData);
			delete[]pData;
			printf("Progress: %d / %d\n", yEnd, iDemHeight);
		}
		GDALClose(pGdalNorData);
		GDALClose(pGdalData);
	}
	
	printf("Saved converted file: %s\n", strOutputFile);
	printf("\n-----End-----\n");
}