#include <stdio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/CGdalDem.hpp"
#include "Eigen_3.0.5/include/Eigen/Dense"
using namespace Eigen;
using namespace std;
typedef struct tagPOINT3D
{
	double x, y, z;
	char strID[CHAR_LEN];
}Point3D;
int main(int argc,char**argv)
{
	if (false==NotePrint(argv,argc,3)){printf("Argument： point_file dem_file\n"); return false; }
	char strFeaturePtFile[FILE_PN],strReferenceSurfaceFile[FILE_PN];
	strcpy(strFeaturePtFile, argv[1]);//feature point file
	strcpy(strReferenceSurfaceFile, argv[2]);//reference dem
	//load data
	vector<Point3D> ptSourceFeaPt, ptIniCenteredFeaPt, ptFeaPt, ptCenteredFeaPt;
	FILE * fp = fopen(strFeaturePtFile, "r");if (NULL == fp){printf("File can't open: %s\n.",strFeaturePtFile);return false;}
	int iFeaturePtNums = 0;
	fscanf(fp, "%d\n", &iFeaturePtNums);
	printf("Feature points: %d\n", iFeaturePtNums);
	for (int i = 0; i < iFeaturePtNums; i++)
	{
		Point3D PtTmp;
		fscanf(fp, "%s %lf %lf %lf\n", PtTmp.strID, &PtTmp.x,&PtTmp.y,&PtTmp.z);
		ptSourceFeaPt.push_back(PtTmp);
		ptIniCenteredFeaPt.push_back(PtTmp);
	}
	fclose(fp);
	CGdalDem *pRefDemFile = new CGdalDem; GDALDEMHDR *pRefDemHead = new GDALDEMHDR;
	if (false == pRefDemFile->LoadFile(strReferenceSurfaceFile, pRefDemHead)){ printf("%s: %s\n", ERROR_FILE_OPEN, strReferenceSurfaceFile); return false; }
	double lfXGsd = pRefDemHead->lfGsdX;
	double lfYGsd = pRefDemHead->lfGsdY;
	int iCols = pRefDemHead->iCol, iRows = pRefDemHead->iRow;
	vector<Point3D> ptReferenceSurfacePt, ptCenteredReferenceSurfacePt;//后续使用

	//待匹配表面特征点的重心化//包含平移和缩放
	double lfFeaPtXmin = ptSourceFeaPt[0].x, lfFeaPtYmin = ptSourceFeaPt[0].y, lfFeaPtZmin = ptSourceFeaPt[0].z;
	for (int i = 1; i < iFeaturePtNums; i++)
	{
		if (lfFeaPtXmin>ptSourceFeaPt[i].x)lfFeaPtXmin = ptSourceFeaPt[i].x;
		if (lfFeaPtYmin>ptSourceFeaPt[i].y)lfFeaPtYmin = ptSourceFeaPt[i].y;
		if (lfFeaPtZmin > ptSourceFeaPt[i].z)lfFeaPtZmin = ptSourceFeaPt[i].z;
	}
	double lfFeaturePtCenterX = 0.0, lfFeaturePtCenterY = 0.0, lfFeaturePtCenterZ = 0.0, lfFeaturePtScaleX = 0.0, lfFeaturePtScaleY = 0.0;//待匹配表面重心化参数
	//首先进行平移
	for (int i = 0; i < iFeaturePtNums; i++)
	{
		lfFeaturePtCenterX += ptSourceFeaPt[i].x;
		lfFeaturePtCenterY += ptSourceFeaPt[i].y;
		lfFeaturePtCenterZ += ptSourceFeaPt[i].z;
	}
	lfFeaturePtCenterX /= iFeaturePtNums;
	lfFeaturePtCenterY /= iFeaturePtNums;
	lfFeaturePtCenterZ /= iFeaturePtNums;
	for (int i = 0; i < iFeaturePtNums; i++)
	{
		ptIniCenteredFeaPt[i].x -= lfFeaturePtCenterX;
		ptIniCenteredFeaPt[i].y -= lfFeaturePtCenterY;
		ptIniCenteredFeaPt[i].z -= lfFeaturePtCenterZ;
	}
	//然后进行缩放
	lfFeaturePtScaleX = iCols*lfXGsd / 2 / lfFeaturePtCenterZ;
	lfFeaturePtScaleY = iRows*lfYGsd / 2 / lfFeaturePtCenterZ;
	for (int i = 0; i < iFeaturePtNums; i++)
	{
		ptIniCenteredFeaPt[i].x /= lfFeaturePtScaleX;
		ptIniCenteredFeaPt[i].y /= lfFeaturePtScaleY;
	}

	double lfOmig = 0.0, lfFai = 0.0, lfKafa = 0.0, lfScale = 1.0, lfTx = 0.0, lfTy = 0.0, lfTz = 0.0;
	printf("\nPara7 initialize：\n");
	printf("Rx=%lf Ry=%lf Rz=%lf lfScale=%lf lfTx=%lf lfTy=%lf lfTz=%lf\n", lfOmig, lfFai, lfKafa, lfScale, lfTx, lfTy, lfTz);
	//iterator
	MatrixXd matrixR = MatrixXd::Random(3, 3);
	int iIterator = 1;
	while (true)
	{
		printf("Iteration %d：\n", iIterator++);
		//1
		matrixR(0, 0) = cos(lfFai)*cos(lfKafa);
		matrixR(0, 1) = -cos(lfFai)*sin(lfKafa);
		matrixR(0, 2) = sin(lfFai);
		matrixR(1, 0) = cos(lfOmig)*sin(lfKafa) + sin(lfOmig)*sin(lfFai)*cos(lfKafa);
		matrixR(1, 1) = cos(lfOmig)*cos(lfKafa) - sin(lfOmig)*sin(lfFai)*sin(lfKafa);
		matrixR(1, 2) = -sin(lfOmig)*cos(lfFai);
		matrixR(2, 0) = sin(lfOmig)*sin(lfKafa) - cos(lfOmig)*sin(lfFai)*cos(lfKafa);
		matrixR(2, 1) = sin(lfOmig)*cos(lfKafa) + cos(lfOmig)*sin(lfFai)*sin(lfKafa);
		matrixR(2, 2) = cos(lfOmig)*cos(lfFai);

		ptReferenceSurfacePt.clear(); ptCenteredReferenceSurfacePt.clear(); ptCenteredFeaPt.clear();
		//寻找同名点
		for (int i = 0; i < iFeaturePtNums; i++)
		{
			Point3D FeaPtTmp,RefPtTmp;
			FeaPtTmp.x = lfScale*(matrixR(0, 0)*ptIniCenteredFeaPt[i].x + matrixR(0, 1)*ptIniCenteredFeaPt[i].y + matrixR(0, 2)*ptIniCenteredFeaPt[i].z) + lfTx;
			FeaPtTmp.y = lfScale*(matrixR(1, 0)*ptIniCenteredFeaPt[i].x + matrixR(1, 1)*ptIniCenteredFeaPt[i].y + matrixR(1, 2)*ptIniCenteredFeaPt[i].z) + lfTy;
			FeaPtTmp.z = lfScale*(matrixR(2, 0)*ptIniCenteredFeaPt[i].x + matrixR(2, 1)*ptIniCenteredFeaPt[i].y + matrixR(2, 2)*ptIniCenteredFeaPt[i].z) + lfTz;
			strcpy(FeaPtTmp.strID, ptIniCenteredFeaPt[i].strID);
			//重心化逆变换
			RefPtTmp.x = FeaPtTmp.x*lfFeaturePtScaleX;
			RefPtTmp.y = FeaPtTmp.y*lfFeaturePtScaleY;
			RefPtTmp.x += lfFeaturePtCenterX;
			RefPtTmp.y += lfFeaturePtCenterY;
			RefPtTmp.z = pRefDemFile->GetDemZValue(RefPtTmp.x, RefPtTmp.y);
			strcpy(RefPtTmp.strID, FeaPtTmp.strID);
			if (RefPtTmp.z != NODATA)
			{
				ptReferenceSurfacePt.push_back(RefPtTmp);
				ptCenteredReferenceSurfacePt.push_back(RefPtTmp);
				ptCenteredFeaPt.push_back(FeaPtTmp);
			}
			//printf("%lf %lf %lf\n", ptFeaturePt[i].x, ptFeaturePt[i].y, ptFeaturePt[i].z);
			//printf("%lf %lf %lf\t%lf\n\n", ptReferenceSurfacePt[i].x, ptReferenceSurfacePt[i].y, ptReferenceSurfacePt[i].z, fabs(ptFeaturePt[i].z - ptReferenceSurfacePt[i].z));
		}
		printf("同名点点数: %d\n", ptReferenceSurfacePt.size());
		if (ptReferenceSurfacePt.size() < 7){ printf("同名点点数少于7个，终止迭代！\n"); break; }
		
		//基准表面得到的同名点重心化
		double lfRefPtXmin = ptReferenceSurfacePt[0].x, lfRefPtYmin = ptReferenceSurfacePt[0].y, lfRefPtZmin = ptReferenceSurfacePt[0].z;
		for (int i = 1; i < ptReferenceSurfacePt.size(); i++)
		{
			if (lfRefPtXmin > ptReferenceSurfacePt[i].x)lfRefPtXmin = ptReferenceSurfacePt[i].x;
			if (lfRefPtYmin > ptReferenceSurfacePt[i].y)lfRefPtYmin = ptReferenceSurfacePt[i].y;
			if (lfRefPtZmin > ptReferenceSurfacePt[i].z)lfRefPtZmin = ptReferenceSurfacePt[i].z;
		}
		double lfReferenceSurfaceCenterX = 0.0, lfReferenceSurfaceCenterY = 0.0, lfReferenceSurfaceCenterZ = 0.0, lfRefPtScaleX = 0.0, lfRefPtScaleY = 0.0;//基准表面重心化参数
		//首先进行平移
		for (int i = 0; i < ptReferenceSurfacePt.size(); i++)
		{
			lfReferenceSurfaceCenterX+=ptCenteredReferenceSurfacePt[i].x;
			lfReferenceSurfaceCenterY+=ptCenteredReferenceSurfacePt[i].y;
			lfReferenceSurfaceCenterZ+=ptCenteredReferenceSurfacePt[i].z;
		}
		lfReferenceSurfaceCenterX /= ptReferenceSurfacePt.size();
		lfReferenceSurfaceCenterY /= ptReferenceSurfacePt.size();
		lfReferenceSurfaceCenterZ /= ptReferenceSurfacePt.size();
		for (int i = 0; i < ptReferenceSurfacePt.size(); i++)
		{
			ptCenteredReferenceSurfacePt[i].x -= lfReferenceSurfaceCenterX;
			ptCenteredReferenceSurfacePt[i].y -= lfReferenceSurfaceCenterY;
			ptCenteredReferenceSurfacePt[i].z -= lfReferenceSurfaceCenterZ;
		}
		//然后进行缩放
		lfRefPtScaleX = iCols*lfXGsd / 2 / lfReferenceSurfaceCenterZ;
		lfRefPtScaleY = iRows*lfYGsd / 2 / lfReferenceSurfaceCenterZ;
		for (int i = 0; i < ptReferenceSurfacePt.size(); i++)
		{
			ptCenteredReferenceSurfacePt[i].x /= lfRefPtScaleX;
			ptCenteredReferenceSurfacePt[i].y /= lfRefPtScaleY;
		}

		//compute
		MatrixXd matrixL = MatrixXd::Random(ptReferenceSurfacePt.size(), 1);
		MatrixXd matrixA = MatrixXd::Random(ptReferenceSurfacePt.size(), 3 * ptReferenceSurfacePt.size());
		for (int i = 0; i < ptReferenceSurfacePt.size(); i++){
			for (int j = 0; j < 3 * ptReferenceSurfacePt.size(); j++){
				matrixA(i, j) = 0.0;
			}
		}
		MatrixXd matrixB = MatrixXd::Random(3 * ptReferenceSurfacePt.size(), 7);	
		for (int i = 0; i < ptReferenceSurfacePt.size(); i++)
		{
			//常数项矩阵
			matrixL(i, 0) = ptCenteredFeaPt[i].z - ptCenteredReferenceSurfacePt[i].z;
		}
		for (int i = 0; i < ptReferenceSurfacePt.size(); i++)
		{
			//法矢矩阵
			matrixA(i, 3 * i + 0) = (pRefDemFile->GetDemZValue(ptReferenceSurfacePt[i].x + lfXGsd, ptReferenceSurfacePt[i].y) - pRefDemFile->GetDemZValue(ptReferenceSurfacePt[i].x - lfXGsd, ptReferenceSurfacePt[i].y)) / (2 * lfXGsd);
			matrixA(i, 3 * i + 1) = (pRefDemFile->GetDemZValue(ptReferenceSurfacePt[i].x, ptReferenceSurfacePt[i].y + lfYGsd) - pRefDemFile->GetDemZValue(ptReferenceSurfacePt[i].x, ptReferenceSurfacePt[i].y - lfYGsd)) / (2 * lfYGsd);
			matrixA(i, 3 * i + 2) = -1;
		}
		for (int i = 0; i < ptReferenceSurfacePt.size(); i++)
		{
			//常系数矩阵
			matrixB(i * 3, 0) = 0.0; matrixB(i * 3, 1) = ptCenteredFeaPt[i].z; matrixB(i * 3, 2) = -ptCenteredFeaPt[i].y; matrixB(i * 3, 3) = ptCenteredFeaPt[i].x; matrixB(i * 3, 4) = 1.0; matrixB(i * 3, 5) = 0.0; matrixB(i * 3, 6) = 0.0;
			matrixB(i * 3 + 1, 0) = -ptCenteredFeaPt[i].z; matrixB(i * 3 + 1, 1) = 0.0; matrixB(i * 3 + 1, 2) = ptCenteredFeaPt[i].x; matrixB(i * 3 + 1, 3) = ptCenteredFeaPt[i].y; matrixB(i * 3 + 1, 4) = 0.0; matrixB(i * 3 + 1, 5) = 1.0; matrixB(i * 3 + 1, 6) = 0.0;
			matrixB(i * 3 + 2, 0) = ptCenteredFeaPt[i].y; matrixB(i * 3 + 2, 1) = -ptCenteredFeaPt[i].x; matrixB(i * 3 + 2, 2) = 0.0; matrixB(i * 3 + 2, 3) = ptCenteredFeaPt[i].z; matrixB(i * 3 + 2, 4) = 0.0; matrixB(i * 3 + 2, 5) = 0.0; matrixB(i * 3 + 2, 6) = 1.0;
		}
		//cout << "A\n" << matrixA << endl;
		//cout << "B\n" << matrixB << endl;
		//cout << "L\n" << matrixL << endl;
		//* 解法方程  */
		MatrixXd matrixAB = MatrixXd::Random(ptReferenceSurfacePt.size(), 7);//中间暂存矩阵
		matrixAB = matrixA*matrixB;
		//cout << "AB\n" << matrixAB << endl;

		MatrixXd matrixABT = MatrixXd::Random(7, ptReferenceSurfacePt.size());//中间暂存矩阵
		matrixABT = matrixAB.transpose();//矩阵转置
		//cout << "ABT\n" << matrixABT << endl;

		MatrixXd matrixABTAB = MatrixXd::Random(7, 7);//中间暂存矩阵
		matrixABTAB = matrixABT*matrixAB;
		//cout << "ABTAB\n" << matrixABTAB << endl;

		MatrixXd matrixABTABInverse = MatrixXd::Random(7, 7);//中间暂存矩阵
		matrixABTABInverse = matrixABTAB.inverse();//矩阵求逆
		//cout << "ABTAB_inverse\n" << matrixABTABInverse << endl;
		
		MatrixXd matrixABTL = MatrixXd::Random(7, 1);//中间暂存矩阵
		matrixABTL = matrixABT*matrixL;
		//cout << "ABTL\n" << matrixABTL << endl;

		MatrixXd matrixX = MatrixXd::Random(7, 1);//未知数矩阵
		matrixX = matrixABTABInverse*matrixABTL;
		//cout << "X\n" << X << endl;
		
		printf("dOmig=%lf dFai=%lf dKafa=%lf dScale=%lf dTx=%lf dTy=%lf dTz=%lf\n", matrixX(0, 0), matrixX(1, 0), matrixX(2, 0), matrixX(3, 0), matrixX(4, 0), matrixX(5, 0), matrixX(6, 0));

		//* 检查计算是否收敛 */
		if (iIterator>100 || 
			(fabs(matrixX(0, 0))<0.5*1.0e-3 && fabs(matrixX(1, 0))<0.5*1.0e-3 && fabs(matrixX(2, 0))<0.5*1.0e-3 && 
			fabs(matrixX(3, 0)) < 0.05*1.0e-2 && 
			fabs(matrixX(4, 0)) < 0.01*lfXGsd && fabs(matrixX(5, 0)) < 0.01*lfYGsd && fabs(matrixX(6, 0)) < 0.01*lfXGsd)
			) break;
		//* 累加 */
		lfOmig += matrixX(0, 0);
		lfFai += matrixX(1, 0);
		lfKafa += matrixX(2, 0);
		lfScale += matrixX(3, 0);
		lfTx += matrixX(4, 0);
		lfTy += matrixX(5, 0);
		lfTz += matrixX(6, 0);
		//printf("七参数：\n");
		//printf("lfOmig=%lf\tlfFai=%lf\tlfKafa=%lf\tlfScale=%lf\tlfTx=%lf\tlfTy=%lf\tlfTz=%lf\n", lfOmig, lfFai, lfKafa, lfScale, lfTx, lfTy, lfTz);
		MatrixXd matrixV = MatrixXd::Random(7, 1);//残差矩阵
		matrixV = matrixAB*matrixX - matrixL;
		//cout << "matrixV\n" << matrixV << endl;
	}
	//* 输出七参数 */
	printf("\nPara7 result：\n");
	printf("lfTx=%lf lfTy=%lf lfTz=%lf lfScale=%lf Rx=%lf Ry=%lf Rz=%lf\n", lfTx, lfTy, lfTz, lfScale, lfOmig, lfFai, lfKafa);
	char strPara7Resultfile[FILE_PN]; strcpy(strPara7Resultfile, strFeaturePtFile);
	char*pS = strrchr(strPara7Resultfile, '\\'); if (NULL == pS)strrchr(strPara7Resultfile, '/');
	sprintf(pS, "%s", "\\Para7_Result.txt");
	fp = fopen(strPara7Resultfile, "w"); if (NULL == fp)return false;
	fprintf(fp, "%d\t%.3lf  %.3lf  %.3lf  %.6lf  %.6lf  %.6lf  %.6lf\n", iFeaturePtNums, lfTx, lfTy, lfTz, lfScale, lfOmig, lfFai, lfKafa);
	fclose(fp);
	delete pRefDemFile, pRefDemHead;
	return true;
}