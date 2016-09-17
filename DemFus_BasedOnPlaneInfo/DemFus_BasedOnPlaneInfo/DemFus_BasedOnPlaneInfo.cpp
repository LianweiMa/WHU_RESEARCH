#include <stdio.h>
#include "opencv_2_4_7/opencv/cv.h"
#include "opencv_2_4_7/opencv/highgui.h"
#include "opencv_2_4_7/opencv/cvaux.h" //必须引此头文件
#include <string.h>
#include "SpDem.hpp"
#include "gdal_1_10_1/gdal_priv.h"
#include <vector>
#include <math.h>
#include "WuEgm.h"
#include "SpGeoCvt.h"
typedef unsigned char BYTE;

inline void Cvt2BLH(double x, double y, double z, double* lon, double* lat, double* h, CSpGeoCvt* m_geoCvt)
{
	int Ellipsoid;           //椭球类型
	int Projectid;           //投影类型  
	double EllipsoA;         //椭球长半轴
	double EllipsoB;	     //椭球短半轴      
	double ProjectOriLat;    //中心纬度(弧度)
	double ProjectCenMeri;   //中心经度(弧度)
	double ProjectFalseEas;  //东偏移
	double ProjectFalseNor;  //北偏移
	double ProjectScale;     //中心经线比例尺
	int eleType;

	m_geoCvt->Get_Cvt_Par(&Ellipsoid, &Projectid, &EllipsoA, &EllipsoB, &ProjectOriLat, &ProjectCenMeri,
		&ProjectFalseEas, &ProjectFalseNor, &ProjectScale, &eleType);

	if (Projectid == GEODETIC_LONLAT){
		*lon = x;
		*lat = y;
		*h = z;
	}
	else if (Projectid == GEODETIC_CENTRXYZ){
		m_geoCvt->Cvt_CXYZ2LBH(x, y, z, lon, lat, h);
		*lon = *lon*SPGC_R2D;	*lat = *lat*SPGC_R2D;
	}
	else{
		m_geoCvt->Cvt_Prj2LBH(x, y, z, lon, lat, h);
		*lon = *lon*SPGC_R2D;	*lat = *lat*SPGC_R2D;
	}
}
float GetFusedZvalue(double x, double y, CSpDem*pSpDem, int &nDemNum)//平均值法
{
	//float *pfZ_Temp = new float[nDemNum];
	float fZ_Temp = NOVALUE, fSum = 0.0; int numZ = 0;
	for (int kDemIndex = 0; kDemIndex < nDemNum; kDemIndex++)
	{
		fZ_Temp = pSpDem[kDemIndex].GetDemZValue(x, y, true);
		if (fZ_Temp != NOVALUE){ fSum += fZ_Temp; numZ++; }
	}
	if (numZ>0)return fSum / numZ;
	else return NOVALUE;
	//delete[]pfZ_Temp;
}
float GetFusedZvalue(double x, double y, CSpDem*pSpDem, int &nDemNum, double &lfXmin, double &lfYmin, double &lfXmax, double &lfYmax)//距离反向加权平均法//只适用于两度重叠的DEM融合
{
	if (nDemNum != 2)printf("Error_InputDemNUms:this method is for 2 dems\n");
	float *pfZ_Temp = new float[nDemNum]; float *pfZ_Old = pfZ_Temp;
	if (lfXmax - lfXmin <= lfYmax - lfYmin)//dX
	{
		int numZ = 0;
		for (int kDemIndex = 0; kDemIndex < nDemNum; kDemIndex++)
		{
			*pfZ_Temp = pSpDem[kDemIndex].GetDemZValue(x, y, true);
			if (*pfZ_Temp != NOVALUE){ pfZ_Temp++; numZ++; }
		}
		pfZ_Temp = pfZ_Old;
		if (numZ == 2)return ((x - lfXmin)*pfZ_Temp[0] + (lfXmax - x)*pfZ_Temp[1]) / (lfXmax - lfXmin);
		else if (numZ == 1)return pfZ_Temp[0];
		else return NOVALUE;
	}
	else
	{
		int numZ = 0;
		for (int kDemIndex = 0; kDemIndex < nDemNum; kDemIndex++)
		{
			*pfZ_Temp = pSpDem[kDemIndex].GetDemZValue(x, y, true);
			if (*pfZ_Temp != NOVALUE){ pfZ_Temp++; numZ++; }
		}
		pfZ_Temp = pfZ_Old;
		if (numZ == 2)return ((y - lfYmin)*pfZ_Temp[0] + (lfYmax - y)*pfZ_Temp[1]) / (lfYmax - lfYmin);
		else if (numZ == 1)return pfZ_Temp[0];
		else return NOVALUE;
	}
	delete[]pfZ_Temp;
}
float NormalHei2Geodetic(double x,double y,CSpDem*pSrtm,const char *pGcdfile)
{
	CWuEgm WuEgm; CSpGeoCvt SpGeoCvt;
	SpGeoCvt.Load4File(pGcdfile);
	float z_temp = 0.0f; double lon, lat, hei;
	Cvt2BLH(x, y, z_temp, &lon, &lat, &hei, &SpGeoCvt);
	z_temp = pSrtm->GetDemZValue(x, y);
	return z_temp + WuEgm.GetGVal(lon, lat);
}
int main(int argc, char**argv)
{
	printf("\n-----Strat fusDem-----\n");
	//read dem file list
	char strDemList[512],*pS; int nDemNum = 0;//nDemNum:the number of dem file
	strcpy(strDemList, argv[1]);
	FILE *fpDemListFile = fopen(strDemList, "r"); if (!fpDemListFile){ printf("Error_FileOpen:%s\n", strDemList); return false; }
	fscanf(fpDemListFile, "%d\n", &nDemNum); if (nDemNum < 2){ printf("Error_DemFileInputNumber:%d\n", nDemNum); return false; }
	else printf("Input dem file:%d\n", nDemNum);
	char **pSDemList = new char *[nDemNum];
	for (int i = 0; i < nDemNum; i++)
	{
		pSDemList[i] = new char[512];
		fscanf(fpDemListFile, "%s\n", pSDemList[i]);
	}
	fclose(fpDemListFile);
	//write excel
	char strOutputInfo[512]; strcpy(strOutputInfo, argv[1]);
	pS = strrchr(strOutputInfo, '\\'); if (!pS)pS = strrchr(strOutputInfo, '/');
	sprintf(pS, "%s", "\\OutputInfo.xls");
	FILE *fpOutputInfo = fopen(strOutputInfo, "w");
	fprintf(fpOutputInfo, "%s\t", "ContoursIndex");
	fprintf(fpOutputInfo, "%s\t%s\t%s\t%s\t%s\t", "Area", "MinZ", "MaxZ", "Z_meanValue","StdDev");
	fprintf(fpOutputInfo, "%s\n", "SrtmZ_meanValue");
	//read strm
	CSpDem *pSrtm = new CSpDem; if (!pSrtm)return false;
	char strSrtm[512]; strcpy(strSrtm, argv[3]);
	char strGcdfile[512]; strcpy(strGcdfile, strSrtm);
	pS = strrchr(strGcdfile, '\\'); if (!pS)pS = strrchr(strGcdfile, '/');
	sprintf(pS, "%s", "\\ProdGeoSys.gcd");
	if (!pSrtm->Load4File(strSrtm))
	{
		printf("Error_FileOpen:%s\n", strSrtm);
		return false;
	}
	//read dem//print dem info
	printf("\n-----Dem info based on plane info-----\n");
	char strPlaneInfo[512]; strcpy(strPlaneInfo, argv[2]);
	GDALAllRegister();
	GDALDataset *gdalPlaneInfo = (GDALDataset *)GDALOpen(strPlaneInfo,GA_ReadOnly);	
	if (!gdalPlaneInfo){ printf("Error_FileOpen:%s\n", strPlaneInfo); return false;}
	int nPlaneWidth = gdalPlaneInfo->GetRasterXSize();
	int nPlaneHeight = gdalPlaneInfo->GetRasterYSize();
	double lfGeoTrans[6]; gdalPlaneInfo->GetGeoTransform(lfGeoTrans);
	BYTE *pPlanData = new BYTE[nPlaneHeight*nPlaneWidth];
	gdalPlaneInfo->RasterIO(GF_Read, 0, 0, nPlaneWidth, nPlaneHeight, pPlanData, nPlaneWidth, nPlaneHeight, GDT_Byte, 1, 0, 0, 0, 0);
	
	IplImage *imgPlane = cvLoadImage(strPlaneInfo, 0);
	if (!imgPlane){ printf("image load failed!\n"); return false; }
	CvSeq* contour = NULL;
	CvMemStorage* storage = cvCreateMemStorage(0);
	

	CSpDem *pSpDem = new CSpDem[nDemNum]; if (!pSpDem)return false;
	double lfXmin = -99999.0, lfXmax = -99999.0, lfYmin = -99999.0, lfYmax = -99999.0;
	double lfXgsd = 0.0, lfYgsd = 0.0;

	float fZmin = NOVALUE, fZmax = NOVALUE, fMean = NOVALUE, fStdDev = 0.0;
	double xGeo = 0.0, yGeo = 0.0; std::vector<float> vecZ,vecSrtmZ;
	float z_tmp=NOVALUE,z_strm_tmp=NOVALUE,fsumZ=0.0f,fsumSrtmZ=0.0f;
	int nTotalArea = 0; float fSumStdDev = 0.0f;
	for (int i = 0; i < nDemNum; i++)
	{
		if (!pSpDem[i].Load4File(pSDemList[i]))
		{
			printf("Error_FileOpen:%s\n", pSDemList[i]);
			return false;
		}
		printf("\n--%d--:%s\n", i + 1, pSDemList[i]);
		SPDEMHDR pSpDemHdr = pSpDem[i].GetDemHeader();
		printf("Xgsd:%lf\tYgsd:%lf\n", pSpDemHdr.intervalX, pSpDemHdr.intervalY);
		if (lfXgsd == 0.0)lfXgsd = pSpDemHdr.intervalX;
		if (lfYgsd == 0.0)lfYgsd = pSpDemHdr.intervalY;
		if (lfXgsd < pSpDemHdr.intervalX)lfXgsd = pSpDemHdr.intervalX;
		if (lfYgsd < pSpDemHdr.intervalY)lfYgsd = pSpDemHdr.intervalY;
		double lfZmin = NOVALUE, lfZmax = NOVALUE;
		lfZmax = pSpDem[i].GetMaxZ(); lfZmin = pSpDem[i].GetMinZ();
		printf("Zmin:%lf\tZmax:%lf\n", lfZmin, lfZmax);
		double lfX[4], lfY[4];
		pSpDem[i].GetDemRgn(lfX, lfY);
		printf("GeoRange:\n\tLT-[%lf,%lf]\n\tRT-[%lf,%lf]\n\tRB-[%lf,%lf]\n\tLB-[%lf,%lf]\n", lfX[0], lfY[0], lfX[1], lfY[1], lfX[2], lfY[2], lfX[3], lfY[3]);
		if (lfXmin == -99999) lfXmin = lfX[0];
		if (lfXmax == -99999) lfXmax = lfX[0];
		if (lfYmin == -99999) lfYmin = lfY[0];
		if (lfYmax == -99999) lfYmax = lfY[0];
		for (int i = 0; i < 4; i++)
		{
			if (lfXmin > lfX[i]) lfXmin = lfX[i];
			if (lfXmax < lfX[i]) lfXmax = lfX[i];
			if (lfYmin > lfY[i]) lfYmin = lfY[i];
			if (lfYmax < lfY[i]) lfYmax = lfY[i];
		}

		printf("\n-----Dem --%d-- contour info-----\n", i);
		int contourIndex = 1;
		IplImage *imgContours = cvCloneImage(imgPlane);
		int contourNum = cvFindContours(imgContours, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
		for (; contour; contour = contour->h_next)
		{
			
			vecZ.clear(); vecSrtmZ.clear(); fsumZ = 0.0f; fsumSrtmZ = 0.0f; fZmin = fZmax = NOVALUE;
			CvRect rect = cvBoundingRect(contour, 0);
			for (int y = rect.y; y < rect.y + rect.height; y++)
			{
				for (int x = rect.x; x < rect.x + rect.width; x++)
				{
					CvPoint2D32f temp = cvPoint2D32f(y, x);
					if (cvPointPolygonTest(contour, temp, 0))
					{
						xGeo = lfGeoTrans[0] + x*lfGeoTrans[1] + y*lfGeoTrans[2];
						yGeo = lfGeoTrans[3] + x*lfGeoTrans[4] + y*lfGeoTrans[5];

						z_tmp = pSpDem[i].GetDemZValue(xGeo, yGeo);
						z_strm_tmp = NormalHei2Geodetic(xGeo, yGeo, pSrtm, strGcdfile);
						if (z_tmp != NOVALUE)
						{
							if (fZmin == NOVALUE || fZmax == NOVALUE){ fZmin = z_tmp; }
							if (fZmin > z_tmp)fZmin = z_tmp;
							if (fZmax < z_tmp)fZmax = z_tmp;
							vecZ.push_back(z_tmp);
							vecSrtmZ.push_back(z_strm_tmp);
							fsumZ += z_tmp;
							fsumSrtmZ += z_strm_tmp;
						}
					}
					
				}
			}
			if (vecZ.size() == 0||vecSrtmZ.size()==0){ printf("ERROR_Plane area:%d\n", vecZ.size()); return false; }
			fMean = fsumZ / vecZ.size(); fStdDev = 0.0f;
			for (int zIndex = 0; zIndex < vecZ.size(); zIndex++)
			{
				fStdDev += pow(vecZ[zIndex] - fMean, 2.0);
			}
			fStdDev = pow(fStdDev / vecZ.size(), 0.5);
			if (true/*vecZ.size()>=100&&fStdDev>1.0f*/)
			{
				printf("\ncontour --%d-- info\n", contourIndex++); fprintf(fpOutputInfo, "%d\t", contourIndex);
				printf("Area:%d\n", vecZ.size()); fprintf(fpOutputInfo, "%d\t", vecZ.size());

				printf("Zmin:%f\tZmax:%f\n", fZmin, fZmax); fprintf(fpOutputInfo, "%f\t%f\t", fZmin, fZmax);
				printf("meanVale:%f\tSrtmMeanValue:%f\n", fMean, fsumSrtmZ / vecSrtmZ.size()); fprintf(fpOutputInfo, "%f\t", fMean);
				printf("stdDevVale:%f\n", fStdDev); fprintf(fpOutputInfo, "%f\t", fStdDev); fprintf(fpOutputInfo, "%f\n", fsumSrtmZ / vecSrtmZ.size());
				if (fsumSrtmZ / vecSrtmZ.size()<fMean)
					fSumStdDev -= fStdDev*vecZ.size();
				else
					fSumStdDev += fStdDev*vecZ.size();
				nTotalArea += vecZ.size();
			}
			
		}
		cvReleaseImageData(imgContours);		
/*
		vecZ.clear(); fsumZ = 0.0f; fZmin = fZmax = NOVALUE;
		for (int iPlane = 0; iPlane < nPlaneHeight;iPlane++)
		{
			for (int jPlane = 0; jPlane < nPlaneWidth;jPlane++)
			{				
				xGeo = lfGeoTrans[0] + jPlane*lfGeoTrans[1] + iPlane*lfGeoTrans[2];
				yGeo = lfGeoTrans[3] + jPlane*lfGeoTrans[4] + iPlane*lfGeoTrans[5];
				z_tmp = pSpDem[i].GetDemZValue(xGeo, yGeo);
				if (*(pPlanData+iPlane*nPlaneWidth+jPlane)==255)
				{
					if (z_tmp != NOVALUE)
					{
						if (fZmin == NOVALUE || fZmax == NOVALUE){ fZmin = z_tmp; }
						if (fZmin>z_tmp)fZmin = z_tmp;
						if (fZmax < z_tmp)fZmax = z_tmp;
						vecZ.push_back(z_tmp);
						fsumZ += z_tmp;
					}
				}
				
			}
		}
		if (vecZ.size() == 0){ printf("ERROR_Plane area:%d\n", vecZ.size()); return false; }
		fMean = fsumZ / vecZ.size(); fStdDev = 0.0f;
		for (int i = 0; i < vecZ.size();i++)
		{
			fStdDev += pow(vecZ[i] - fMean, 2.0);
		}
		fStdDev = pow(fStdDev / vecZ.size(), 0.5);
		printf("Zmin:%f\tZmax:%f\n", fZmin, fZmax);
		printf("meanVale:%f\n", fMean);
		printf("stdDevVale:%f\n", fStdDev);*/
	}
	fclose(fpOutputInfo);
	GDALClose(gdalPlaneInfo);
	float fSysError = fSumStdDev / nTotalArea;
	printf("\nMeanStdDev:\t%f\n", fSysError);
	//Fus dem
	printf("\n-----Fus dem-----\n");
	int nCols = 0, nRows = 0;
	nCols = int((lfXmax - lfXmin) / lfXgsd) + 1;
	nRows = int((lfYmax - lfYmin) / lfYgsd) + 1;
	printf("Rows:%d\tCols:%d\n", nRows, nCols);
	printf("Fus Dem ...  0%%");
	float *pfFusDem = new float[nRows*nCols];
	double x = lfXmin, y = lfYmin;
	for (int i = 0; i < nRows; i++)
	{
		x = lfXmin;
		for (int j = 0; j < nCols; j++)
		{
			//*(pfFusDem+i*nCols+j)=GetFusedZvalue(x, y, pSpDem, nDemNum);
			*(pfFusDem + i*nCols + j) = GetFusedZvalue(x, y, pSpDem, nDemNum, lfXmin, lfYmin, lfXmax, lfYmax)+fSysError;
			x += lfXgsd;
		}
		y += lfYgsd;
		printf("\b\b\b%2d%%", i * 100 / nRows);
	}
	printf("\b\b\b100%%\n");
	printf("\n-----Save fusDem-----\n");
	CSpDem fusDem;
	SPDEMHDR DemHdr;
	DemHdr.startX = lfXmin; DemHdr.startY = lfYmin;
	DemHdr.intervalX = lfXgsd; DemHdr.intervalY = lfYgsd;
	DemHdr.column = nCols;   DemHdr.row = nRows;
	DemHdr.kapa = 0;
	fusDem.Attach(DemHdr, pfFusDem);
	char strFusDem[512]; strcpy(strFusDem, strDemList);
	pS = strrchr(strFusDem, '/'); if (!pS)pS = strrchr(strFusDem, '\\');
	sprintf(pS, "%s", "\\Dem_Fused.tif");
	if (fusDem.Save2File(strFusDem, VER_TIF))printf("Saved fusDem file: %s\n", strFusDem);
	else printf("Error_FileSaved: %s\n", strFusDem);
	printf("\n-----End fusDem-----\n");

	//delete memory
	cvReleaseImageData(imgPlane);
	delete[]pSrtm;
	delete[]pSpDem;
	for (int i = 0; i < nDemNum; i++)
	{
		delete[]pSDemList[i];
	}
	delete[]pSDemList;

	return true;
}