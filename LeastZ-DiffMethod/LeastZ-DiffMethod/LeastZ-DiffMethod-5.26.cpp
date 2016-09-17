#include <stdio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/CGdalDem.hpp"
#include "Eigen_3.0.5/include/Eigen/Dense"
#include "Eigen_3.0.5/include/Eigen/StdVector"
using namespace Eigen;
using namespace std;
typedef struct tagPOINT3D
{
	Vector4d pt;
	char strID[CHAR_LEN];
}Point3D;
int main(int argc,char**argv)
{
	if (false==NotePrint(argv,argc,3)){printf("Argument： point_file dem_file\n"); return false; }
	char strFeaturePtFile[FILE_PN],strRefSurfaceFile[FILE_PN];
	strcpy(strFeaturePtFile, argv[1]);//feature point file
	strcpy(strRefSurfaceFile, argv[2]);//reference dem
	//load data
	//Fea pt
	FILE * fp = fopen(strFeaturePtFile, "r");if (NULL == fp){printf("File can't open: %s\n.",strFeaturePtFile);return false;}
	vector<Point3D, Eigen::aligned_allocator<Eigen::Vector4d>> ptSourceFeaPt, ptTransFeaPt, ptCenteredFeaPt;
	//Ref pt
	CGdalDem *pRefDemFile = new CGdalDem; GDALDEMHDR *pRefDemHead = new GDALDEMHDR;
	if (false == pRefDemFile->LoadFile(strRefSurfaceFile, pRefDemHead)){ printf("%s: %s\n", ERROR_FILE_OPEN, strRefSurfaceFile); return false; }
	double lfXGsd = pRefDemHead->lfGsdX,lfYGsd = pRefDemHead->lfGsdY;
	int iCols = pRefDemHead->iCol, iRows = pRefDemHead->iRow;
	vector<Point3D,Eigen::aligned_allocator<Eigen::Vector4d>> ptRefSurfacePt, ptCenterRefSurfacePt;
	
	int iFeaturePtNums = 0;fscanf(fp, "%d\n", &iFeaturePtNums);
	printf("Feature points: %d\n", iFeaturePtNums);
	double lfIniTx = 15.0, lfIniTy = 15.0, lfIniTz = 5.0;
	MatrixXd matrixHIniFeaPt = MatrixXd::Random(4, 4);
	matrixHIniFeaPt << 1.0, 0.0, 0.0, lfIniTx,
		0.0, 1.0, 0.0, lfIniTy,
		0.0, 0.0, 1.0, lfIniTz,
		0.0, 0.0, 0.0, 1.0;
	for (int i = 0; i < iFeaturePtNums; i++)
	{
		Point3D FeaPtTmp,RefPtTmp;
		fscanf(fp, "%s %lf %lf %lf\n", FeaPtTmp.strID, &FeaPtTmp.pt(0), &FeaPtTmp.pt(1), &FeaPtTmp.pt(2));FeaPtTmp.pt(3) = 1.0;
		ptSourceFeaPt.push_back(FeaPtTmp);
		ptTransFeaPt.push_back(FeaPtTmp);
		RefPtTmp.pt = matrixHIniFeaPt*FeaPtTmp.pt;
		RefPtTmp.pt(2) = pRefDemFile->GetDemZValue(RefPtTmp.pt(0), RefPtTmp.pt(1));
		if (RefPtTmp.pt(2) != NODATA)
		{
			ptCenteredFeaPt.push_back(FeaPtTmp);//初始化第一个点集
			RefPtTmp.pt(2) += lfIniTz;
			ptCenterRefSurfacePt.push_back(RefPtTmp);//初始化第二个点集
			ptRefSurfacePt.push_back(RefPtTmp);//备份基准表面点集，后续计算法矢时使用
		}
	}
	fclose(fp);
	printf("同名点点数: %d\n", ptCenterRefSurfacePt.size());
	if (ptCenterRefSurfacePt.size() < 7){ printf("同名点点数少于7个，终止迭代！\n"); return false; }

	//两点集重心化//包含平移和缩放
	double lfFeaPtCenterX = 0.0, lfFeaPtCenterY = 0.0, lfFeaPtCenterZ = 0.0, lfFeaPtScaleX = 0.0, lfFeaPtScaleY = 0.0;//待匹配表面重心化参数
	double lfRefSurfaceCenterX = 0.0, lfRefSurfaceCenterY = 0.0, lfRefSurfaceCenterZ = 0.0, lfRefPtScaleX = 0.0, lfRefPtScaleY = 0.0;//基准表面重心化参数
	//H矩阵
	MatrixXd matrixHFeaPt = MatrixXd::Random(4, 4), matrixHRefPt = MatrixXd::Random(4, 4);
	matrixHFeaPt<<1.0,0.0,0.0,0.0,
		0.0,1.0,0.0,0.0,
		0.0,0.0,1.0,0.0,
		0.0, 0.0, 0.0, 1.0;
	matrixHRefPt << 1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0;
	//缩放参数
	for (int i = 0; i < ptCenterRefSurfacePt.size(); i++)
	{
		lfFeaPtCenterX += ptCenteredFeaPt[i].pt(0);
		lfFeaPtCenterY += ptCenteredFeaPt[i].pt(1);
		lfFeaPtCenterZ += ptCenteredFeaPt[i].pt(2);

		lfRefSurfaceCenterX += ptCenterRefSurfacePt[i].pt(0);
		lfRefSurfaceCenterY += ptCenterRefSurfacePt[i].pt(1);
		lfRefSurfaceCenterZ += ptCenterRefSurfacePt[i].pt(2);
	}
	lfFeaPtCenterX /= ptCenterRefSurfacePt.size();
	lfFeaPtCenterY /= ptCenterRefSurfacePt.size();
	lfFeaPtCenterZ /= ptCenterRefSurfacePt.size();

	lfRefSurfaceCenterX /= ptRefSurfacePt.size();
	lfRefSurfaceCenterY /= ptRefSurfacePt.size();
	lfRefSurfaceCenterZ /= ptRefSurfacePt.size();	
	//compute H
	matrixHFeaPt(0, 3) = -lfFeaPtCenterX;
	matrixHFeaPt(1, 3) = -lfFeaPtCenterY;
	matrixHFeaPt(2, 3) = -lfFeaPtCenterZ;

	matrixHRefPt(0, 3) = -lfRefSurfaceCenterX;
	matrixHRefPt(1, 3) = -lfRefSurfaceCenterY;
	matrixHRefPt(2, 3) = -lfRefSurfaceCenterZ;
	
	for (int i = 0; i < ptCenterRefSurfacePt.size(); i++)
	{
		ptCenteredFeaPt[i].pt = matrixHFeaPt*ptCenteredFeaPt[i].pt;
		ptCenterRefSurfacePt[i].pt = matrixHRefPt*ptCenterRefSurfacePt[i].pt;
	}
	cout << "matrixHRefPt\n" << matrixHRefPt << endl;
	cout << "matrixHFeaPt\n" << matrixHFeaPt << endl;

	//iterator
	double lfOmig = 0.0, lfFai = 0.0, lfKafa = 0.0, lfScale = 1.0, lfTx = 0.0, lfTy = 0.0, lfTz = 0.0;
	printf("\nPara7 initialize：\n");
	printf("Rx\t\tRy\t\tRz\t\tScale\t\tTx\t\tTy\t\tTz\n%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", lfOmig, lfFai, lfKafa, lfScale, lfTx, lfTy, lfTz);
	int iIterator = 1;
	Matrix3d matrixR = Matrix3d::Random();
	Matrix4d matrixHR = Matrix4d::Random(), matrixHCR = Matrix4d::Random();
	while (true)
	{
		printf("Iteration %d：\n", iIterator++);
		//compute
		MatrixXd matrixL = MatrixXd::Random(ptRefSurfacePt.size(), 1);
		MatrixXd matrixA = MatrixXd::Random(ptRefSurfacePt.size(), 3 * ptRefSurfacePt.size());
		for (int i = 0; i < ptRefSurfacePt.size(); i++){
			for (int j = 0; j < 3 * ptRefSurfacePt.size(); j++){
				matrixA(i, j) = 0.0;
			}
		}
		MatrixXd matrixB = MatrixXd::Random(3 * ptRefSurfacePt.size(), 7);	
		for (int i = 0; i < ptRefSurfacePt.size(); i++)
		{
			//常数项矩阵
			matrixL(i, 0) = ptCenteredFeaPt[i].pt(2) - ptCenterRefSurfacePt[i].pt(2);
		}
		for (int i = 0; i < ptRefSurfacePt.size(); i++)
		{
			//法矢矩阵
			matrixA(i, 3 * i + 0) = (pRefDemFile->GetDemZValue(ptRefSurfacePt[i].pt(0) + lfXGsd, ptRefSurfacePt[i].pt(1)) - pRefDemFile->GetDemZValue(ptRefSurfacePt[i].pt(0) - lfXGsd, ptRefSurfacePt[i].pt(1))) / (2 * lfXGsd);
			matrixA(i, 3 * i + 1) = (pRefDemFile->GetDemZValue(ptRefSurfacePt[i].pt(0), ptRefSurfacePt[i].pt(1) + lfYGsd) - pRefDemFile->GetDemZValue(ptRefSurfacePt[i].pt(0), ptRefSurfacePt[i].pt(1) - lfYGsd)) / (2 * lfYGsd);
			matrixA(i, 3 * i + 2) = -1.0;
		}
		for (int i = 0; i < ptRefSurfacePt.size(); i++)
		{
			//常系数矩阵
			matrixB(i * 3, 0) = 0.0; matrixB(i * 3, 1) = ptCenteredFeaPt[i].pt(2); matrixB(i * 3, 2) = -ptCenteredFeaPt[i].pt(1); matrixB(i * 3, 3) = ptCenteredFeaPt[i].pt(0); matrixB(i * 3, 4) = 1.0; matrixB(i * 3, 5) = 0.0; matrixB(i * 3, 6) = 0.0;
			matrixB(i * 3 + 1, 0) = -ptCenteredFeaPt[i].pt(2); matrixB(i * 3 + 1, 1) = 0.0; matrixB(i * 3 + 1, 2) = ptCenteredFeaPt[i].pt(0); matrixB(i * 3 + 1, 3) = ptCenteredFeaPt[i].pt(1); matrixB(i * 3 + 1, 4) = 0.0; matrixB(i * 3 + 1, 5) = 1.0; matrixB(i * 3 + 1, 6) = 0.0;
			matrixB(i * 3 + 2, 0) = ptCenteredFeaPt[i].pt(1); matrixB(i * 3 + 2, 1) = -ptCenteredFeaPt[i].pt(0); matrixB(i * 3 + 2, 2) = 0.0; matrixB(i * 3 + 2, 3) = ptCenteredFeaPt[i].pt(2); matrixB(i * 3 + 2, 4) = 0.0; matrixB(i * 3 + 2, 5) = 0.0; matrixB(i * 3 + 2, 6) = 1.0;
		}
		//cout << "A\n" << matrixA << endl;
		//cout << "B\n" << matrixB << endl;
		//cout << "L\n" << matrixL << endl;
		//* 解法方程  */
		MatrixXd matrixAB = MatrixXd::Random(ptRefSurfacePt.size(), 7);//中间暂存矩阵
		matrixAB = matrixA*matrixB;
		MatrixXd matrixABT = MatrixXd::Random(7, ptRefSurfacePt.size());//中间暂存矩阵
		matrixABT = matrixAB.transpose();//矩阵转置
		MatrixXd matrixABTAB = MatrixXd::Random(7, 7);//中间暂存矩阵
		matrixABTAB = matrixABT*matrixAB;
		MatrixXd matrixABTABInverse = MatrixXd::Random(7, 7);//中间暂存矩阵
		matrixABTABInverse = matrixABTAB.inverse();//矩阵求逆		
		VectorXd vectorABTL(7);//中间暂存矩阵
		vectorABTL = matrixABT*matrixL;
		VectorXd vectorX(7);//未知数矩阵
		vectorX = matrixABTABInverse*vectorABTL;
		/*cout << "AB\n" << matrixAB << endl;
		cout << "ABT\n" << matrixABT << endl;
		cout << "ABTAB\n" << matrixABTAB << endl;
		cout << "ABTAB_inverse\n" << matrixABTABInverse << endl;
		cout << "ABTL\n" << vectorABTL << endl;
		cout << "X\n" << vectorX << endl;*/
		printf("domig\t\tdfai\t\tdkafa\t\tdScale\t\tdTx\t\tdTy\t\tdTz\n%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", vectorX(0, 0), vectorX(1, 0), vectorX(2, 0), vectorX(3, 0), vectorX(4, 0), vectorX(5, 0), vectorX(6, 0));
		//检查计算是否收敛
		if (iIterator>100 || 
			(fabs(vectorX(0))<0.5*1.0e-3 && fabs(vectorX(1))<0.5*1.0e-3 && fabs(vectorX(2))<0.5*1.0e-3 &&
			fabs(vectorX(3)) < 0.05*1.0e-2 &&
			fabs(vectorX(4)) < 0.01*lfXGsd && fabs(vectorX(5)) < 0.01*lfYGsd && fabs(vectorX(6)) < 0.01*lfXGsd)
			) break;
		ptCenteredFeaPt.clear(); ptCenterRefSurfacePt.clear(); ptRefSurfacePt.clear();
		lfOmig = vectorX(0);
		lfFai = vectorX(1);
		lfKafa = vectorX(2);
		lfScale = 1.0 + vectorX(3);
		lfTx = vectorX(4);
		lfTy = vectorX(5);
		lfTz = vectorX(6);
		//R
		matrixR(0, 0) = cos(lfFai)*cos(lfKafa);
		matrixR(0, 1) = -cos(lfFai)*sin(lfKafa);
		matrixR(0, 2) = sin(lfFai);
		matrixR(1, 0) = cos(lfOmig)*sin(lfKafa) + sin(lfOmig)*sin(lfFai)*cos(lfKafa);
		matrixR(1, 1) = cos(lfOmig)*cos(lfKafa) - sin(lfOmig)*sin(lfFai)*sin(lfKafa);
		matrixR(1, 2) = -sin(lfOmig)*cos(lfFai);
		matrixR(2, 0) = sin(lfOmig)*sin(lfKafa) - cos(lfOmig)*sin(lfFai)*cos(lfKafa);
		matrixR(2, 1) = sin(lfOmig)*cos(lfKafa) + cos(lfOmig)*sin(lfFai)*sin(lfKafa);
		matrixR(2, 2) = cos(lfOmig)*cos(lfFai);
		matrixHCR(0, 0) = lfScale*matrixR(0, 0); matrixHCR(0, 1) = lfScale*matrixR(0, 1); matrixHCR(0, 2) = lfScale*matrixR(0, 2); matrixHCR(0, 3) = lfTx;
		matrixHCR(1, 0) = lfScale*matrixR(1, 0); matrixHCR(1, 1) = lfScale*matrixR(1, 1); matrixHCR(1, 2) = lfScale*matrixR(1, 2); matrixHCR(1, 3) = lfTy;
		matrixHCR(2, 0) = lfScale*matrixR(2, 0); matrixHCR(2, 1) = lfScale*matrixR(2, 1); matrixHCR(2, 2) = lfScale*matrixR(2, 2); matrixHCR(2, 3) = lfTz;
		matrixHCR(3, 0) = 0.0; matrixHCR(3, 1) = 0.0; matrixHCR(3, 2) = 0.0; matrixHCR(3, 3) = 1.0;
		MatrixXd matrixHRefPt_Inverse = MatrixXd::Random(4, 4);
		matrixHRefPt_Inverse = matrixHRefPt.inverse();
		matrixHR = matrixHRefPt_Inverse*matrixHCR*matrixHFeaPt*matrixHIniFeaPt;
		cout << "matrixHRefPt\n" << matrixHRefPt << endl;
		cout << "matrixHRefPt_Inverse\n" << matrixHRefPt_Inverse << endl;
		cout << "matrixHCR\n" << matrixHCR << endl;
		cout << "matrixHFeaPt\n" << matrixHFeaPt << endl;
		cout << "matrixHR\n" << matrixHR << endl;
		for (int i = 0; i < iFeaturePtNums;i++)
		{
			ptTransFeaPt[i].pt = matrixHR*ptTransFeaPt[i].pt;
			Point3D RefPtTmp;
			RefPtTmp.pt = ptTransFeaPt[i].pt;
			RefPtTmp.pt(2) = pRefDemFile->GetDemZValue(RefPtTmp.pt(0), RefPtTmp.pt(1));
			if (RefPtTmp.pt(2) != NODATA)
			{
				ptCenteredFeaPt.push_back(ptSourceFeaPt[i]);//第一个点集
				ptCenterRefSurfacePt.push_back(RefPtTmp);//第二个点集
				ptRefSurfacePt.push_back(RefPtTmp);//备份基准表面点集，后续计算法矢时使用
			}
		}
		printf("参与计算点数: %d\n", ptCenteredFeaPt.size());
		if (ptCenteredFeaPt.size() < 7){ printf("参与计算点数少于7个，终止迭代！\n"); break; }
		//两点集重心化
		lfFeaPtCenterX = lfFeaPtCenterY = lfFeaPtCenterZ = 0.0;
		lfRefSurfaceCenterX = lfRefSurfaceCenterY = lfRefSurfaceCenterZ = 0.0;
		//缩放参数
		for (int i = 0; i < ptCenterRefSurfacePt.size(); i++)
		{
			lfFeaPtCenterX += ptCenteredFeaPt[i].pt(0);
			lfFeaPtCenterY += ptCenteredFeaPt[i].pt(1);
			lfFeaPtCenterZ += ptCenteredFeaPt[i].pt(2);

			lfRefSurfaceCenterX += ptCenterRefSurfacePt[i].pt(0);
			lfRefSurfaceCenterY += ptCenterRefSurfacePt[i].pt(1);
			lfRefSurfaceCenterZ += ptCenterRefSurfacePt[i].pt(2);
		}
		lfFeaPtCenterX /= ptCenterRefSurfacePt.size();
		lfFeaPtCenterY /= ptCenterRefSurfacePt.size();
		lfFeaPtCenterZ /= ptCenterRefSurfacePt.size();

		lfRefSurfaceCenterX /= ptRefSurfacePt.size();
		lfRefSurfaceCenterY /= ptRefSurfacePt.size();
		lfRefSurfaceCenterZ /= ptRefSurfacePt.size();
		//compute H
		matrixHFeaPt(0, 0) = 1.0; matrixHFeaPt(0, 3) = -lfFeaPtCenterX;
		matrixHFeaPt(1, 1) = 1.0; matrixHFeaPt(1, 3) = -lfFeaPtCenterY;
		matrixHFeaPt(2, 2) = 1.0; matrixHFeaPt(2, 3) = -lfFeaPtCenterZ;
		matrixHFeaPt(3, 3) = 1.0;

		matrixHRefPt(0, 0) = 1.0; matrixHRefPt(0, 3) = -lfRefSurfaceCenterX;
		matrixHRefPt(1, 1) = 1.0; matrixHRefPt(1, 3) = -lfRefSurfaceCenterY;
		matrixHRefPt(2, 2) = 1.0; matrixHRefPt(2, 3) = -lfRefSurfaceCenterZ;
		matrixHRefPt(3, 3) = 1.0;
		for (int i = 0; i < ptCenterRefSurfacePt.size(); i++)
		{
			ptCenteredFeaPt[i].pt = matrixHFeaPt*ptCenteredFeaPt[i].pt;
			ptCenterRefSurfacePt[i].pt = matrixHRefPt*ptCenterRefSurfacePt[i].pt;
		}
		//printf("Para7:\n");
		//printf("omig\t\tfai\t\tkafa\t\tScale\t\tTx\t\tTy\t\tTz\n%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", lfOmig, lfFai, lfKafa, lfScale, lfTx, lfTy, lfTz);
		//cout << "matrixHR\n" << matrixHR << endl;
		//MatrixXd matrixV = MatrixXd::Random(7, 1);//残差矩阵
		//matrixV = matrixAB*matrixX - matrixL;
		//cout << "matrixV\n" << matrixV << endl;
	}
	//* 输出七参数 */
	//printf("\nPara7 result：\n");
	//printf("omig\t\tfai\t\tkafa\t\tScale\t\tTx\t\tTy\t\tTz\n%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", lfOmig, lfFai, lfKafa, lfScale, lfTx, lfTy, lfTz);
	cout << "matrixHR\n" << matrixHR << endl;
	/*char strPara7Resultfile[FILE_PN]; strcpy(strPara7Resultfile, strFeaturePtFile);
	char*pS = strrchr(strPara7Resultfile, '\\'); if (NULL == pS)strrchr(strPara7Resultfile, '/');
	sprintf(pS, "%s", "\\Para7_Result.txt");
	fp = fopen(strPara7Resultfile, "w"); if (NULL == fp)return false;
	fprintf(fp, "%d\t%.3lf  %.3lf  %.3lf  %.6lf  %.6lf  %.6lf  %.6lf\n", iFeaturePtNums, lfTx, lfTy, lfTz, lfScale, lfOmig, lfFai, lfKafa);
	fclose(fp);*/
	delete pRefDemFile, pRefDemHead;
	return true;
}