#include <stdio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include "mlw_1_0_0/type_define.h"
#include "wu/WuDem.h"
#include "wu/WuLasFile.h"
#include "wu/WuPt2Dem.h"
#include "Eigen_3.0.5/include/Eigen/Dense"
using namespace Eigen;
using namespace std;
//typedef Matrix<double, Dynamic, Dynamic> MatrixXd;
/************************************************************************/
/*    GetLasMz                                                          */
/*    pPts:三维点                                                       */
/*    iPtNums:三维点数                                                  */
/*    pMinz,pMaxz:高程最值                                              */
/************************************************************************/
bool GetLasMz(DPT3D*pPts, int &iPtNums, double*pMinZ, double*pMaxZ){
	for (int i = 0; i < iPtNums; i++){
		if (i == 0){//赋初值
			*pMinZ = pPts[0].z;
			*pMaxZ = pPts[1].z;
		}
		else{//get min/max value
			if (pPts[i].z < *pMinZ)*pMinZ = pPts[i].z;
			if (pPts[i].z > *pMaxZ)*pMaxZ = pPts[i].z;
		}
	}
	return true;
}
int main(int argc, char**argv)
{
	char strInputFile[FILE_PN],strSimilarityPara[FILE_PN];
	strcpy(strInputFile, argv[1]);//第1个参数是待定向的DEM文件
	strcpy(strSimilarityPara, argv[2]);
	/************************************************************************/
	/* 读取数据                                                             */
	/************************************************************************/
	FILE * fp = fopen(strSimilarityPara, "r");
	if (NULL == fp){ printf("File can't open: %s\n.", strSimilarityPara); return false; }
	double lfOmig = 0.0, lfFai = 0.0, lfKafa = 0.0, lfScale = 1.0, lfTx = 0.0, lfTy = 0.0, lfTz = 0.0;
	fscanf(fp,"%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", &lfOmig, &lfFai, &lfKafa, &lfScale, &lfTx, &lfTy, &lfTz);
	fclose(fp);
	CWuDem demSourceDem;
	WUDEMHDR headSourceDemHdr;
	//demSourceDem.Open(strInputFile, CWuDem::modeRead, &headSourceDemHdr);
	demSourceDem.Load4File(strInputFile);
	headSourceDemHdr = demSourceDem.GetDemHeader();
	int iCols = headSourceDemHdr.column;
	int iRows = headSourceDemHdr.row;
	DPT3D *pPt3D = new DPT3D[iCols*iRows];
	int iPtNum = 0;
	/************************************************************************/
	/* 计算旋转矩阵 更新Dem上的坐标                                           */
	/************************************************************************/
	double rotate[3][3] = { 0.0 };//旋转矩阵
	/************************************************************************/
	/* 计算旋转矩阵                                                         */
	/************************************************************************/
	rotate[0][0] = cos(lfFai)*cos(lfKafa);
	rotate[0][1] = -cos(lfFai)*sin(lfKafa);
	rotate[0][2] = sin(lfFai);
	rotate[1][0] = cos(lfOmig)*sin(lfKafa) + sin(lfOmig)*sin(lfFai)*cos(lfKafa);
	rotate[1][1] = cos(lfOmig)*cos(lfKafa) - sin(lfOmig)*sin(lfFai)*sin(lfKafa);
	rotate[1][2] = -sin(lfOmig)*cos(lfFai);
	rotate[2][0] = sin(lfOmig)*sin(lfKafa) - cos(lfOmig)*sin(lfFai)*cos(lfKafa);
	rotate[2][1] = sin(lfOmig)*cos(lfKafa) + cos(lfOmig)*sin(lfFai)*sin(lfKafa);
	rotate[2][2] = cos(lfOmig)*cos(lfFai);
	double rotateT[3][3] = { 0.0 };//旋转矩阵
	for (int i = 0; i < 3; i++)//求转置矩阵
	{
		for (int j = 0; j < 3; j++)
		{
			rotateT[j][i] = rotate[i][j];
		}
	}
	double lfCenterX = 0.0, lfCenterY = 0.0, lfCenterZ = 0.0;
	lfCenterX = headSourceDemHdr.startX + headSourceDemHdr.intervalX*(iCols / 2);
	lfCenterY = headSourceDemHdr.startY + headSourceDemHdr.intervalY*(iCols / 2);
	lfCenterZ = demSourceDem.GetMidZ();
	double lfStartX = 0.0, lfStartY = 0.0;
	for (int i = 0; i < iRows;i++){
		for (int j = 0; j < iCols;j++){
			double lfSourceX = 0.0, lfSourceY=0.0, lfSourceZ=0.0;
			demSourceDem.GetGlobalXYZValue(j, i, &lfSourceX, &lfSourceY, &lfSourceZ);
			double lfCenteredPtX, lfCenteredPtY, lfCenteredPtZ;
			lfCenteredPtX = lfSourceX - lfCenterX;
			lfCenteredPtY = lfSourceY - lfCenterY;
			lfCenteredPtZ = lfSourceZ - lfCenterZ;
			if (lfSourceZ != NOVALUE){
				(pPt3D + iPtNum)->x = (1.0 / lfScale)*(rotateT[0][0] * (lfCenteredPtX - lfTx) + rotateT[0][1] * (lfCenteredPtY - lfTy) + rotateT[0][2] * (lfCenteredPtZ - lfTz)) + lfCenterX;
				(pPt3D + iPtNum)->y = (1.0 / lfScale)*(rotateT[1][0] * (lfCenteredPtX - lfTx) + rotateT[1][1] * (lfCenteredPtY - lfTy) + rotateT[1][2] * (lfCenteredPtZ - lfTz)) + lfCenterY;
				(pPt3D + iPtNum)->z = (1.0 / lfScale)*(rotateT[2][0] * (lfCenteredPtX - lfTx) + rotateT[2][1] * (lfCenteredPtY - lfTy) + rotateT[2][2] * (lfCenteredPtZ - lfTz)) + lfCenterZ;
				iPtNum++;
			}
		}
	}
	//char strLasFile[FILE_PN]; strcpy(strLasFile, strInputFile);
	//sprintf(strrchr(strLasFile, '.'), "%s", "_transfer.las");
	//CWuLasFile lasFile;
	//lasFile.Open(strLasFile, CWuLasFile::modeCreate);
	//Append2Las(strLasFile, pPt3D, iPtNum);
	double lfMinZ, lfMaxZ;
	GetLasMz(pPt3D, iPtNum, &lfMinZ, &lfMaxZ);
	CWuPt2Dem wuPt2Dem;T2D_DEM t2dDem;
	t2dDem = wuPt2Dem.Pt2Dem(headSourceDemHdr.intervalX, headSourceDemHdr.intervalY, pPt3D, iPtNum, CWuPt2Dem::MOD_TIN,lfMinZ,lfMaxZ);
	headSourceDemHdr.startX = t2dDem.x0;
	headSourceDemHdr.startY = t2dDem.y0;
	headSourceDemHdr.column = t2dDem.col;
	headSourceDemHdr.row = t2dDem.row;
	headSourceDemHdr.intervalX = t2dDem.dx;
	headSourceDemHdr.intervalY = t2dDem.dy;
	char strOutputFile[FILE_PN]; strcpy(strOutputFile, strInputFile);
	sprintf(strrchr(strOutputFile, '.') , "%s", "_transfer.tif");
	CWuDem demOutputDem;
	demOutputDem.Attach(headSourceDemHdr, t2dDem.pGrid);
	demOutputDem.Save2File(strOutputFile);

	delete[]pPt3D;
	demOutputDem.Detach();
	demOutputDem.Reset();
	return true;
}