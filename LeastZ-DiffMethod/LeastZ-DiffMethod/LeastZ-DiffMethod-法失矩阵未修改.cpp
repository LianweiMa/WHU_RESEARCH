#include <stdio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include "mlw_1_0_0/type_define.h"
#include "wu/WuDem.h"
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
	char strFeaturePtFile[FILE_PN],strReferenceSurfaceFile[FILE_PN];
	strcpy(strFeaturePtFile, argv[1]);//第1个参数是待匹配表面特征点文件
	strcpy(strReferenceSurfaceFile, argv[2]);//第2个参数是基准表面文件
	// 读取数据 
	vector<Point3D> ptSourceFeaturePt, ptCenteredFeaturePt, ptCenteredFeaturePtBackup;//待匹配表面特征点坐标
	FILE * fp = fopen(strFeaturePtFile, "r");if (NULL == fp){printf("File can't open: %s\n.",strFeaturePtFile);return false;}
	int iFeaturePtNums = 0;fscanf(fp, "%d\n", &iFeaturePtNums);
	double lfFeaturePtCenterX = 0.0, lfFeaturePtCenterY = 0.0, lfFeaturePtCenterZ = 0.0;//待匹配表面重心化坐标
	for (int i = 0; i < iFeaturePtNums; i++)
	{
		Point3D PtTmp;
		fscanf(fp, "%s %lf %lf %lf\n", PtTmp.strID, &PtTmp.x,&PtTmp.y,&PtTmp.z);
		lfFeaturePtCenterX += PtTmp.x;
		lfFeaturePtCenterY += PtTmp.y;
		lfFeaturePtCenterZ += PtTmp.z;
		ptSourceFeaturePt.push_back(PtTmp);
		ptCenteredFeaturePt.push_back(PtTmp);//初始化
	}
	//待匹配表面特征点重心化
	lfFeaturePtCenterX /= iFeaturePtNums;
	lfFeaturePtCenterY /= iFeaturePtNums;
	lfFeaturePtCenterZ /= iFeaturePtNums;
	for (int i = 0; i < iFeaturePtNums; i++)
	{
		ptCenteredFeaturePt[i].x -= lfFeaturePtCenterX;
		ptCenteredFeaturePt[i].y -= lfFeaturePtCenterY;
		ptCenteredFeaturePt[i].z -= lfFeaturePtCenterZ;
		ptCenteredFeaturePtBackup.push_back(ptCenteredFeaturePt[i]);//原始特征点重心化后的坐标备份
	}
	fclose(fp);
	CWuDem ReferenceSurface;
	if(ReferenceSurface.Load4File(strReferenceSurfaceFile)==false)return false;
	double lfXGsd = ReferenceSurface.GetDemHeader().intervalX;
	double lfYGsd = ReferenceSurface.GetDemHeader().intervalY;
	vector<Point3D> ptReferenceSurfacePt,ptCenteredReferenceSurfacePt;//基准表面上的点
	for (int i = 0; i < iFeaturePtNums; i++)
	{
		Point3D PtTmp = ptSourceFeaturePt[i];
		PtTmp.z = ReferenceSurface.GetDemZValue(PtTmp.x, PtTmp.y);
		ptReferenceSurfacePt.push_back(PtTmp);
		ptCenteredReferenceSurfacePt.push_back(PtTmp);//初始化
	}
	/************************************************************************/
	/* 确定未知数的初始值                                                   */
	/************************************************************************/
	double lfOmig = 0.0, lfFai = 0.0, lfKafa = 0.0, lfScale = 1.0, lfTx = 0.0, lfTy = 0.0, lfTz = 0.0;
	printf("\n七参数的初始值：\n");
	printf("Rx=%lf\tRy=%lf\tRz=%lf\tlfScale=%lf\tlfTx=%lf\tlfTy=%lf\tlfTz=%lf\n", lfOmig, lfFai, lfKafa, lfScale, lfTx, lfTy, lfTz);
	/************************************************************************/
	/* 进入迭代过程                                                         */
	/************************************************************************/
	MatrixXd matrixR = MatrixXd::Random(3, 3);//旋转矩阵
	MatrixXd matrixL = MatrixXd::Random(iFeaturePtNums, 1);//常数项矩阵
	MatrixXd matrixA = MatrixXd::Random(iFeaturePtNums, 3 * iFeaturePtNums);//法矢矩阵
	for (int i = 0; i < iFeaturePtNums;i++){
		for (int j = 0; j < 3 * iFeaturePtNums;j++){
			matrixA(i, j) = 0.0;
		}
	}
	MatrixXd matrixB = MatrixXd::Random(3 * iFeaturePtNums, 7);//常系数矩阵
	int iIterator = 1;//迭代次序
	while (true)
	{
		//* 计算旋转矩阵 
		matrixR(0, 0) = cos(lfFai)*cos(lfKafa);
		matrixR(0, 1) = -cos(lfFai)*sin(lfKafa);
		matrixR(0, 2) = sin(lfFai);
		matrixR(1, 0) = cos(lfOmig)*sin(lfKafa) + sin(lfOmig)*sin(lfFai)*cos(lfKafa);
		matrixR(1, 1) = cos(lfOmig)*cos(lfKafa) - sin(lfOmig)*sin(lfFai)*sin(lfKafa);
		matrixR(1, 2) = -sin(lfOmig)*cos(lfFai);
		matrixR(2, 0) = sin(lfOmig)*sin(lfKafa) - cos(lfOmig)*sin(lfFai)*cos(lfKafa);
		matrixR(2, 1) = sin(lfOmig)*cos(lfKafa) + cos(lfOmig)*sin(lfFai)*sin(lfKafa);
		matrixR(2, 2) = cos(lfOmig)*cos(lfFai);

		for (int i = 0; i < iFeaturePtNums; i++)
		{
			Point3D PtTmp;
			PtTmp.x = lfScale*(matrixR(0, 0)*ptCenteredFeaturePtBackup[i].x + matrixR(0, 1)*ptCenteredFeaturePtBackup[i].y + matrixR(0, 2)*ptCenteredFeaturePtBackup[i].z) + lfTx;
			PtTmp.y = lfScale*(matrixR(1, 0)*ptCenteredFeaturePtBackup[i].x + matrixR(1, 1)*ptCenteredFeaturePtBackup[i].y + matrixR(1, 2)*ptCenteredFeaturePtBackup[i].z) + lfTy;
			PtTmp.z = lfScale*(matrixR(2, 0)*ptCenteredFeaturePtBackup[i].x + matrixR(2, 1)*ptCenteredFeaturePtBackup[i].y + matrixR(2, 2)*ptCenteredFeaturePtBackup[i].z) + lfTz;
			ptCenteredFeaturePt[i].x = PtTmp.x;
			ptCenteredFeaturePt[i].y = PtTmp.y;
			ptCenteredFeaturePt[i].z = PtTmp.z;

			PtTmp.x += lfFeaturePtCenterX;
			PtTmp.y += lfFeaturePtCenterY;
			//PtTmp.z += lfFeaturePtCenterZ;
			PtTmp.z = ReferenceSurface.GetDemZValue(PtTmp.x, PtTmp.y);//更新基准表面上的Z坐标
			ptReferenceSurfacePt[i].x = PtTmp.x;
			ptReferenceSurfacePt[i].y = PtTmp.y;
			ptReferenceSurfacePt[i].z = PtTmp.z;
		}
		//基准表面点重心化
		double lfReferenceSurfaceCenterX = 0.0, lfReferenceSurfaceCenterY = 0.0, lfReferenceSurfaceCenterZ = 0.0;	
		for (int i = 0; i < iFeaturePtNums; i++)
		{
			lfReferenceSurfaceCenterX += ptReferenceSurfacePt[i].x;
			lfReferenceSurfaceCenterY += ptReferenceSurfacePt[i].y;
			lfReferenceSurfaceCenterZ += ptReferenceSurfacePt[i].z;
		}
		lfReferenceSurfaceCenterX /= iFeaturePtNums;
		lfReferenceSurfaceCenterY /= iFeaturePtNums;
		lfReferenceSurfaceCenterZ /= iFeaturePtNums;
		for (int i = 0; i < iFeaturePtNums; i++)
		{
			ptCenteredReferenceSurfacePt[i].x -= lfReferenceSurfaceCenterX;
			ptCenteredReferenceSurfacePt[i].y -= lfReferenceSurfaceCenterY;
			ptCenteredReferenceSurfacePt[i].z -= lfReferenceSurfaceCenterZ;
		}
		/************************************************************************/
		/* 计算像点坐标的近似值、常数项矩阵、系数矩阵                           */
		/************************************************************************/
		for (int i = 0; i < iFeaturePtNums; i++)
		{
			//常数项矩阵
			matrixL(i, 0) = ptCenteredFeaturePtBackup[i].z - ptCenteredReferenceSurfacePt[i].z;
		}
		for (int i = 0; i < iFeaturePtNums; i++)
		{
			//法矢矩阵
			matrixA(i, 3 * i + 0) = (ReferenceSurface.GetDemZValue(ptReferenceSurfacePt[i].x + lfXGsd, ptReferenceSurfacePt[i].y) - ReferenceSurface.GetDemZValue(ptReferenceSurfacePt[i].x - lfXGsd, ptReferenceSurfacePt[i].y)) / (2 * lfXGsd);
			matrixA(i, 3 * i + 1) = (ReferenceSurface.GetDemZValue(ptReferenceSurfacePt[i].x, ptReferenceSurfacePt[i].y + lfYGsd) - ReferenceSurface.GetDemZValue(ptReferenceSurfacePt[i].x, ptReferenceSurfacePt[i].y - lfYGsd)) / (2 * lfYGsd);
			matrixA(i, 3 * i + 2) = -1;
		}
		for (int i = 0; i < iFeaturePtNums; i++)
		{
			//常系数矩阵
			matrixB(i * 3, 0) = 0.0; matrixB(i * 3, 1) = ptCenteredFeaturePt[i].z; matrixB(i * 3, 2) = -ptCenteredFeaturePt[i].y; matrixB(i * 3, 3) = ptCenteredFeaturePt[i].x; matrixB(i * 3, 4) = 1.0; matrixB(i * 3, 5) = 0.0; matrixB(i * 3, 6) = 0.0;
			matrixB(i * 3 + 1, 0) = -ptCenteredFeaturePt[i].z; matrixB(i * 3 + 1, 1) = 0.0; matrixB(i * 3 + 1, 2) = ptCenteredFeaturePt[i].x; matrixB(i * 3 + 1, 3) = ptCenteredFeaturePt[i].y; matrixB(i * 3 + 1, 4) = 0.0; matrixB(i * 3 + 1, 5) = 1.0; matrixB(i * 3 + 1, 6) = 0.0;
			matrixB(i * 3 + 2, 0) = ptCenteredFeaturePt[i].y; matrixB(i * 3 + 2, 1) = -ptCenteredFeaturePt[i].x; matrixB(i * 3 + 2, 2) = 0.0; matrixB(i * 3 + 2, 3) = ptCenteredFeaturePt[i].z; matrixB(i * 3 + 2, 4) = 0.0; matrixB(i * 3 + 2, 5) = 0.0; matrixB(i * 3 + 2, 6) = 1.0;
			//matrixB(i * 3, 0) = 0.0; matrixB(i * 3, 1) = ptCenteredFeaturePtBackup[i].z; matrixB(i * 3, 2) = -ptCenteredFeaturePtBackup[i].y; matrixB(i * 3, 3) = ptCenteredFeaturePtBackup[i].x; matrixB(i * 3, 4) = 1.0; matrixB(i * 3, 5) = 0.0; matrixB(i * 3, 6) = 0.0;
			//matrixB(i * 3 + 1, 0) = -ptCenteredFeaturePtBackup[i].z; matrixB(i * 3 + 1, 1) = 0.0; matrixB(i * 3 + 1, 2) = ptCenteredFeaturePtBackup[i].x; matrixB(i * 3 + 1, 3) = ptCenteredFeaturePtBackup[i].y; matrixB(i * 3 + 1, 4) = 0.0; matrixB(i * 3 + 1, 5) = 1.0; matrixB(i * 3 + 1, 6) = 0.0;
			//matrixB(i * 3 + 2, 0) = ptCenteredFeaturePtBackup[i].y; matrixB(i * 3 + 2, 1) = -ptCenteredFeaturePtBackup[i].x; matrixB(i * 3 + 2, 2) = 0.0; matrixB(i * 3 + 2, 3) = ptCenteredFeaturePtBackup[i].z; matrixB(i * 3 + 2, 4) = 0.0; matrixB(i * 3 + 2, 5) = 0.0; matrixB(i * 3 + 2, 6) = 1.0;
		}
		//cout << "A\n" << matrixA << endl;
		//cout << "B\n" << matrixB << endl;
		//cout << "L\n" << matrixL << endl;
		/************************************************************************/
		/* 解法方程                                                             */
		/************************************************************************/
		MatrixXd matrixAB = MatrixXd::Random(iFeaturePtNums, 7);//中间暂存矩阵
		matrixAB = matrixA*matrixB;
		//cout << "AB\n" << matrixAB << endl;

		MatrixXd matrixABT = MatrixXd::Random(7,iFeaturePtNums);//中间暂存矩阵
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
		/************************************************************************/
		/* 检查计算是否收敛                                                     */
		/************************************************************************/
		if (iIterator>30 || (fabs(matrixX(4, 0)) < 0.01 && fabs(matrixX(5, 0)) < 0.01 && fabs(matrixX(6, 0)) < 0.01&&fabs(matrixX(0, 0))<0.5 && fabs(matrixX(1, 0))<0.5 && fabs(matrixX(2, 0))<0.5 && fabs(matrixX(3, 0))<0.05))
		{
			break;
		}
		/************************************************************************/
		/* 累加                                                                 */
		/************************************************************************/
		lfOmig += matrixX(0, 0);
		lfFai += matrixX(1, 0);
		lfKafa += matrixX(2, 0);
		lfScale += matrixX(3, 0);
		lfTx += matrixX(4, 0);
		lfTy += matrixX(5, 0);
		lfTz += matrixX(6, 0);
		printf("\n第%d次七参数改正量：\n", iIterator++);
		printf("dOmig=%lf\tdFai=%lf\tdKafa=%lf\tdScale=%lf\tdTx=%lf\tdTy=%lf\tdTz=%lf\n", matrixX(0, 0), matrixX(1, 0), matrixX(2, 0), matrixX(3, 0), matrixX(4, 0), matrixX(5, 0), matrixX(6, 0));
		printf("七参数：\n");
		printf("lfOmig=%lf\tlfFai=%lf\tlfKafa=%lf\tlfScale=%lf\tlfTx=%lf\tlfTy=%lf\tlfTz=%lf\n", lfOmig, lfFai, lfKafa, lfScale, lfTx, lfTy, lfTz);
	}
	/************************************************************************/
	/* 输出旋转矩阵、七参数                                     */
	/************************************************************************/
	printf("\n七参数：\n");
	printf("lfTx=%lf\nlfTy=%lf\nlfTz=%lf\nlfFai=%lf\nlfOmig=%lf\nlfKafa=%lf\nlfScale=%lf\n", lfTx, lfTy, lfTz, lfFai, lfOmig, lfKafa, lfScale);
	printf("\n");
	cout << "旋转矩阵为:\n" << matrixR << endl;
	return true;
}