#include <stdio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/CGdalDem.hpp"
#include "Whu/WuLasFile.h"
#include "Whu/WuPt2Dem.h"
using namespace std;
int main(int argc, char**argv)
{
	if (!NotePrint(argv,argc,3)){printf("Argument: dem_file para7_file\n"); return false; }

	char strInputFile[FILE_PN],strSimilarityPara[FILE_PN];
	strcpy(strInputFile, argv[1]);//第1个参数是待变换的DEM文件
	strcpy(strSimilarityPara, argv[2]);//第2个参数是变换的七参数
	// 读取数据 
	FILE * fp = fopen(strSimilarityPara, "r");
	if (NULL == fp){ printf("File can't open: %s\n.", strSimilarityPara); return false; }
	double lfOmig = 0.0, lfFai = 0.0, lfKafa = 0.0, lfScale = 1.0, lfTx = 0.0, lfTy = 0.0, lfTz = 0.0;
	fscanf(fp, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", &lfOmig, &lfFai, &lfKafa, &lfScale, &lfTx, &lfTy, &lfTz);
	fclose(fp);
	CGdalDem *pOldDemFile = new CGdalDem;
	GDALDEMHDR *pOldDemHead = new GDALDEMHDR;
	if (false==pOldDemFile->LoadFile(strInputFile,pOldDemHead)){ printf("加载DEM失败！\n"); return false; }
	int iCols = pOldDemHead->iCol;
	int iRows = pOldDemHead->iRow;

	double rotate[3][3] = { 0.0 },rotateT[3][3] = { 0.0 };//旋转矩阵及其转置矩阵
	//计算旋转矩阵
	rotate[0][0] = cos(lfFai)*cos(lfKafa);
	rotate[0][1] = -cos(lfFai)*sin(lfKafa);
	rotate[0][2] = sin(lfFai);
	rotate[1][0] = cos(lfOmig)*sin(lfKafa) + sin(lfOmig)*sin(lfFai)*cos(lfKafa);
	rotate[1][1] = cos(lfOmig)*cos(lfKafa) - sin(lfOmig)*sin(lfFai)*sin(lfKafa);
	rotate[1][2] = -sin(lfOmig)*cos(lfFai);
	rotate[2][0] = sin(lfOmig)*sin(lfKafa) - cos(lfOmig)*sin(lfFai)*cos(lfKafa);
	rotate[2][1] = sin(lfOmig)*cos(lfKafa) + cos(lfOmig)*sin(lfFai)*sin(lfKafa);
	rotate[2][2] = cos(lfOmig)*cos(lfFai);
	//旋转矩阵转置
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			rotateT[j][i] = rotate[i][j];
		}
	}

	//新旧坐标转换
	DPT3D *pNewPt3D = new DPT3D[iCols*iRows];
	int iPtNum = 0;//自加记录点数
	float fMinZ=99999.0f, fMaxZ=-99999.0f;//存储坐标转换后的高程最值
	double lfStartX = 0.0, lfStartY = 0.0;
	for (int i = 0; i < iRows;i++){
		for (int j = 0; j < iCols;j++){
			double lfOldX = 0.0, lfOldY = 0.0;float fOldZ = 0.0f;
			pOldDemFile->GetXYZValue(j, i, lfOldX, lfOldY, fOldZ);
			if (fOldZ != NODATA){
				(pNewPt3D + iPtNum)->x = (1.0 / lfScale)*(rotateT[0][0] * (lfOldX - lfTx) + rotateT[0][1] * (lfOldY - lfTy) + rotateT[0][2] * (fOldZ - lfTz));
				(pNewPt3D + iPtNum)->y = (1.0 / lfScale)*(rotateT[1][0] * (lfOldX - lfTx) + rotateT[1][1] * (lfOldY - lfTy) + rotateT[1][2] * (fOldZ - lfTz));
				(pNewPt3D + iPtNum)->z = (1.0 / lfScale)*(rotateT[2][0] * (lfOldX - lfTx) + rotateT[2][1] * (lfOldY - lfTy) + rotateT[2][2] * (fOldZ - lfTz));

				iPtNum++;

				if (fMaxZ < fOldZ)fMaxZ = fOldZ;
				if (fMinZ > fOldZ)fMinZ = fOldZ;
			}
		}
	}

	//将坐标转换后形成的离散点云转换成DEM	
	CWuPt2Dem wuPt2Dem;T2D_DEM t2dDem;
	if (pOldDemHead->lfStartX>180 || pOldDemHead->lfStartX < -180){//投影类型的坐标形式
		t2dDem = wuPt2Dem.Pt2Dem((float)pOldDemHead->lfGsdX, (float)pOldDemHead->lfGsdY, pNewPt3D, iPtNum, CWuPt2Dem::MOD_TIN, fMinZ, fMaxZ);
	}
	else{//经纬度坐标形式
		double lfOffX = pNewPt3D[0].x, lfOffY = pNewPt3D[0].y, lfZoom = 100000;
		for (int i = 1; i<iPtNum; i++){
			if (lfOffX>pNewPt3D[i].x) lfOffX = pNewPt3D[i].x;
			if (lfOffY > pNewPt3D[i].y) lfOffY = pNewPt3D[i].y;
		}
		lfOffX = pOldDemHead->lfGsdX * int(lfOffX / pOldDemHead->lfGsdX);
		lfOffY = pOldDemHead->lfGsdY * int(lfOffY / pOldDemHead->lfGsdY);
		for (int i = 0; i < iPtNum; i++){
			pNewPt3D[i].x = (pNewPt3D[i].x - lfOffX)*lfZoom;
			pNewPt3D[i].y = (pNewPt3D[i].y - lfOffY)*lfZoom;
		}
		t2dDem = wuPt2Dem.Pt2Dem((float)(pOldDemHead->lfGsdX*lfZoom), (float)(pOldDemHead->lfGsdY*lfZoom), pNewPt3D, iPtNum, CWuPt2Dem::MOD_TIN, fMinZ, fMaxZ);
		t2dDem.x0 = t2dDem.x0 / lfZoom + lfOffX;
		t2dDem.y0 = t2dDem.y0 / lfZoom + lfOffY;
	}
	//新DEM文件写入头信息
	char strOutputFile[FILE_PN]; strcpy(strOutputFile, strInputFile);
	sprintf(strrchr(strOutputFile, '.'), "%s", "_Para7_Transform.tif");
	CGdalDem *pNewDemFile = new CGdalDem;
	GDALDEMHDR *pNewDemHead = new GDALDEMHDR;
	pNewDemHead->iCol = t2dDem.col;
	pNewDemHead->iRow = t2dDem.row;
	pNewDemHead->lfGsdX = t2dDem.dx;
	pNewDemHead->lfGsdY = t2dDem.dy;
	pNewDemHead->lfStartX = t2dDem.x0;
	pNewDemHead->lfStartY = t2dDem.y0;
	float*pNewZ = new float[pNewDemHead->iCol*pNewDemHead->iRow];
	memset(pNewZ, 0, sizeof(float)*pNewDemHead->iRow*pNewDemHead->iCol);
	for (int i = 0; i < pNewDemHead->iRow;i++)
	{
		for (int j = 0; j < pNewDemHead->iCol;j++)
		{
			*(pNewZ + i*pNewDemHead->iCol + j) = *(t2dDem.pGrid + (t2dDem.row - i - 1)*t2dDem.col + j);
		}
		
		//memcpy(pNewZ + i*pNewDemHead->iCol, , sizeof(float)*pNewDemHead->iCol);
	}
	if (false == pNewDemFile->CreatFile(strOutputFile, pNewDemHead))return false;
	pNewDemFile->WriteBlock(pNewZ, 0, 0, pNewDemHead->iCol, pNewDemHead->iRow);

	delete[]pNewPt3D; delete[]pNewZ;
	delete pOldDemFile, pOldDemHead, pNewDemFile, pNewDemHead;
	return true;
}