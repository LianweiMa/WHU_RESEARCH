//无重心化
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
	vector<Point3D> ptSourceFeaturePt, ptFeaturePt;
	FILE * fp = fopen(strFeaturePtFile, "r");if (NULL == fp){printf("File can't open: %s\n.",strFeaturePtFile);return false;}
	int iFeaturePtNums = 0;
	fscanf(fp, "%d\n", &iFeaturePtNums);
	printf("Feature points: %d\n", iFeaturePtNums);
	for (int i = 0; i < iFeaturePtNums; i++)
	{
		Point3D PtTmp;
		fscanf(fp, "%s %lf %lf %lf\n", PtTmp.strID, &PtTmp.x,&PtTmp.y,&PtTmp.z);
		ptSourceFeaturePt.push_back(PtTmp);
		ptFeaturePt.push_back(PtTmp);
	}
	fclose(fp);
	CGdalDem *pReffDemFile=new CGdalDem; GDALDEMHDR *pReffDemHead=new GDALDEMHDR;
	if (false == pReffDemFile->LoadFile(strReferenceSurfaceFile, pReffDemHead)){ printf("%s: %s\n",ERROR_FILE_OPEN,strReferenceSurfaceFile); return false; }
	double lfXGsd = pReffDemHead->lfGsdX;
	double lfYGsd = pReffDemHead->lfGsdY;
	vector<Point3D> ptReferenceSurfacePt;
	for (int i = 0; i < iFeaturePtNums; i++)
	{
		Point3D PtTmp = ptSourceFeaturePt[i];
		PtTmp.z = pReffDemFile->GetDemZValue(PtTmp.x, PtTmp.y);
		ptReferenceSurfacePt.push_back(PtTmp);
	}
	double lfOmig = 0.0, lfFai = 0.0, lfKafa = 0.0, lfScale = 1.0, lfTx = 0.0, lfTy = 0.0, lfTz = 0.0;
	printf("\nPara7 initialize：\n");
	printf("Rx=%lf Ry=%lf Rz=%lf lfScale=%lf lfTx=%lf lfTy=%lf lfTz=%lf\n", lfOmig, lfFai, lfKafa, lfScale, lfTx, lfTy, lfTz);
	//iterator
	MatrixXd matrixR = MatrixXd::Random(3, 3);
	MatrixXd matrixL = MatrixXd::Random(iFeaturePtNums, 1);
	MatrixXd matrixA = MatrixXd::Random(iFeaturePtNums, 3 * iFeaturePtNums);
	for (int i = 0; i < iFeaturePtNums;i++){
		for (int j = 0; j < 3 * iFeaturePtNums;j++){
			matrixA(i, j) = 0.0;
		}
	}
	MatrixXd matrixB = MatrixXd::Random(3 * iFeaturePtNums, 7);
	int iIterator = 1;
	while (true)
	{
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

		for (int i = 0; i < iFeaturePtNums; i++)
		{
			Point3D PtTmp;
			PtTmp.x = lfScale*(matrixR(0, 0)*ptSourceFeaturePt[i].x + matrixR(0, 1)*ptSourceFeaturePt[i].y + matrixR(0, 2)*ptSourceFeaturePt[i].z) + lfTx;
			PtTmp.y = lfScale*(matrixR(1, 0)*ptSourceFeaturePt[i].x + matrixR(1, 1)*ptSourceFeaturePt[i].y + matrixR(1, 2)*ptSourceFeaturePt[i].z) + lfTy;
			PtTmp.z = lfScale*(matrixR(2, 0)*ptSourceFeaturePt[i].x + matrixR(2, 1)*ptSourceFeaturePt[i].y + matrixR(2, 2)*ptSourceFeaturePt[i].z) + lfTz;
			ptFeaturePt[i].x = PtTmp.x;
			ptFeaturePt[i].y = PtTmp.y;
			ptFeaturePt[i].z = PtTmp.z;

			PtTmp.z = pReffDemFile->GetDemZValue(PtTmp.x, PtTmp.y);
			ptReferenceSurfacePt[i].x = PtTmp.x;
			ptReferenceSurfacePt[i].y = PtTmp.y;
			ptReferenceSurfacePt[i].z = PtTmp.z;
			//printf("%lf %lf %lf\n", ptFeaturePt[i].x, ptFeaturePt[i].y, ptFeaturePt[i].z);
			//printf("%lf %lf %lf\t%lf\n\n", ptReferenceSurfacePt[i].x, ptReferenceSurfacePt[i].y, ptReferenceSurfacePt[i].z, fabs(ptFeaturePt[i].z - ptReferenceSurfacePt[i].z));
		}
		//compute
		for (int i = 0; i < iFeaturePtNums; i++)
		{
			matrixL(i, 0) = (ptFeaturePt[i].z - ptReferenceSurfacePt[i].z);
		}
		for (int i = 0; i < iFeaturePtNums; i++)
		{
			matrixA(i, 3 * i + 0) = (pReffDemFile->GetDemZValue(ptReferenceSurfacePt[i].x + lfXGsd, ptReferenceSurfacePt[i].y) - pReffDemFile->GetDemZValue(ptReferenceSurfacePt[i].x - lfXGsd, ptReferenceSurfacePt[i].y)) / (2 * lfXGsd);
			matrixA(i, 3 * i + 1) = (pReffDemFile->GetDemZValue(ptReferenceSurfacePt[i].x, ptReferenceSurfacePt[i].y + lfYGsd) - pReffDemFile->GetDemZValue(ptReferenceSurfacePt[i].x, ptReferenceSurfacePt[i].y - lfYGsd)) / (2 * lfYGsd);
			matrixA(i, 3 * i + 2) = -1;
		}
		for (int i = 0; i < iFeaturePtNums; i++)
		{
			matrixB(i * 3, 0) = 0.0; matrixB(i * 3, 1) = ptFeaturePt[i].z; matrixB(i * 3, 2) = -ptFeaturePt[i].y; matrixB(i * 3, 3) = ptFeaturePt[i].x; matrixB(i * 3, 4) = 1.0; matrixB(i * 3, 5) = 0.0; matrixB(i * 3, 6) = 0.0;
			matrixB(i * 3 + 1, 0) = -ptFeaturePt[i].z; matrixB(i * 3 + 1, 1) = 0.0; matrixB(i * 3 + 1, 2) = ptFeaturePt[i].x; matrixB(i * 3 + 1, 3) = ptFeaturePt[i].y; matrixB(i * 3 + 1, 4) = 0.0; matrixB(i * 3 + 1, 5) = 1.0; matrixB(i * 3 + 1, 6) = 0.0;
			matrixB(i * 3 + 2, 0) = ptFeaturePt[i].y; matrixB(i * 3 + 2, 1) = -ptFeaturePt[i].x; matrixB(i * 3 + 2, 2) = 0.0; matrixB(i * 3 + 2, 3) = ptFeaturePt[i].z; matrixB(i * 3 + 2, 4) = 0.0; matrixB(i * 3 + 2, 5) = 0.0; matrixB(i * 3 + 2, 6) = 1.0;

			//matrixB(i * 3, 0) = 0.0; matrixB(i * 3, 1) = ptSourceFeaturePt[i].z; matrixB(i * 3, 2) = -ptSourceFeaturePt[i].y; matrixB(i * 3, 3) = ptSourceFeaturePt[i].x; matrixB(i * 3, 4) = 1.0; matrixB(i * 3, 5) = 0.0; matrixB(i * 3, 6) = 0.0;
			//matrixB(i * 3 + 1, 0) = -ptSourceFeaturePt[i].z; matrixB(i * 3 + 1, 1) = 0.0; matrixB(i * 3 + 1, 2) = ptSourceFeaturePt[i].x; matrixB(i * 3 + 1, 3) = ptSourceFeaturePt[i].y; matrixB(i * 3 + 1, 4) = 0.0; matrixB(i * 3 + 1, 5) = 1.0; matrixB(i * 3 + 1, 6) = 0.0;
			//matrixB(i * 3 + 2, 0) = ptSourceFeaturePt[i].y; matrixB(i * 3 + 2, 1) = -ptSourceFeaturePt[i].x; matrixB(i * 3 + 2, 2) = 0.0; matrixB(i * 3 + 2, 3) = ptSourceFeaturePt[i].z; matrixB(i * 3 + 2, 4) = 0.0; matrixB(i * 3 + 2, 5) = 0.0; matrixB(i * 3 + 2, 6) = 1.0;
		}
		//cout << "A\n" << matrixA << endl;
		//cout << "B\n" << matrixB << endl;
		//cout << "L\n" << matrixL << endl;
		//* 解法方程  */
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
		printf("Iteration %d：\n", iIterator++);
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
	//fprintf(fp, "%d\t%.3lf  %.3lf  %.3lf  %.6lf  %.6lf  %.6lf  %.6lf\n", iFeaturePtNums, lfTx, lfTy, lfTz, lfScale, lfOmig, lfFai, lfKafa);
	fprintf(fp, "%d\t%.3lf  %.3lf  %.3lf  %.6lf  %.6lf  %.6lf  %.6lf\n", iFeaturePtNums, lfTx, lfTy, lfTz, lfScale, lfOmig, lfFai, lfKafa);
	fclose(fp);
	delete pReffDemFile, pReffDemHead;
	return true;
}