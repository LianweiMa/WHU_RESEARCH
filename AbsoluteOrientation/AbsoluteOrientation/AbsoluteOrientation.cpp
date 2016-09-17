#include <stdio.h>
#include <iostream>
#include <vector>
#include "mlw_1_0_0/type_define.h"
#include "mlw_1_0_0/dem.hpp"
#include "Eigen_3.0.5/include/Eigen/Dense"
using namespace Eigen;
using namespace std;
typedef struct tagPOINT3D
{
	double x, y, z;
	char strID[CHAR_LEN];
}Point3D;
int main(int argc, char**argv)
{
	char strGCPFile[FILE_PN], strImagePtFile[FILE_PN];
	strcpy(strGCPFile, argv[1]);//第1个参数是控制点文件
	strcpy(strImagePtFile, argv[2]);//第2个参数是像点物坐标
	/************************************************************************/
	/* 读取数据                                                             */
	/************************************************************************/
	vector<Point3D> PtGCP, PtGCPTmp;//控制点坐标
	FILE * fp = fopen(strGCPFile, "r");
	if (NULL == fp){ printf("image.txt can't open."); return false; }
	int iGCPNums = 0;
	fscanf(fp, "%d\n", &iGCPNums);
	double lfSigmaGCPX = 0.0, lfSigmaGCPY = 0.0, lfSigmaGCPZ = 0.0;
	for (int i = 0; i < iGCPNums; i++)
	{
		Point3D PtTmp;
		fscanf(fp, "%s %lf %lf %lf\n", PtTmp.strID, &PtTmp.x, &PtTmp.y, &PtTmp.z);
		lfSigmaGCPX += PtTmp.x;
		lfSigmaGCPY += PtTmp.y;
		lfSigmaGCPZ += PtTmp.z;
		PtGCP.push_back(PtTmp);
		PtGCPTmp.push_back(PtTmp);
	}
	//重心化
	lfSigmaGCPX /= iGCPNums;
	lfSigmaGCPY /= iGCPNums;
	lfSigmaGCPZ /= iGCPNums;
	/*for (int i = 0; i < iGCPNums; i++)
	{
		PtGCP[i].x -= lfSigmaGCPX;
		PtGCP[i].y -= lfSigmaGCPY;
		PtGCP[i].z -= lfSigmaGCPZ;
	}*/
	fclose(fp);
	fp = fopen(strImagePtFile, "r");
	if (NULL == fp){ printf("image.txt can't open."); return false; }
	fscanf(fp, "%d\n", &iGCPNums);
	vector<Point3D> PtImage, PtImageTmp;//影像上的坐标
	double lfSigmaImageX = 0.0, lfSigmaImageY = 0.0, lfSigmaImageZ = 0.0;
	for (int i = 0; i < iGCPNums; i++)
	{
		Point3D PtTmp;
		fscanf(fp, "%s %lf %lf %lf\n", PtTmp.strID, &PtTmp.x, &PtTmp.y, &PtTmp.z);
		lfSigmaImageX += PtTmp.x;
		lfSigmaImageY += PtTmp.y;
		lfSigmaImageZ += PtTmp.z;
		PtImage.push_back(PtTmp);
		PtImageTmp.push_back(PtTmp);
	}
	lfSigmaImageX /= iGCPNums;
	lfSigmaImageY /= iGCPNums;
	lfSigmaImageZ /= iGCPNums;;
	/*for (int i = 0; i < iGCPNums; i++)
	{
		PtImage[i].x -= lfSigmaImageX;
		PtImage[i].y -= lfSigmaImageY;
		PtImage[i].z -= lfSigmaImageZ;
	}*/
	/************************************************************************/
	/* 确定未知数的初始值                                                   */
	/************************************************************************/
	double Tx = 0.0, Ty = 0.0, Tz = 0.0, fai = 0.0, omig = 0.0, kafa = 0.0, Scale = 1.0;
	printf("\n七参数的初始值：\n");
	printf("Tx=%lf\nTy=%lf\nTz=%lf\nfai=%lf\nomig=%lf\nkafa=%lf\nScale=%lf\n", Tx, Ty, Tz, fai, omig, kafa, Scale);
	/************************************************************************/
	/* 进入迭代过程                                                         */
	/************************************************************************/
	MatrixXd rotate = MatrixXd::Random(3, 3);//旋转矩阵
	MatrixXd L = MatrixXd::Random(iGCPNums*3, 1);//常数项矩阵
	MatrixXd B = MatrixXd::Random(3 * iGCPNums, 7);//常系数矩阵
	int iteratorID = 1;
	while (1)
	{
		/************************************************************************/
		/* 计算旋转矩阵                                            */
		/************************************************************************/
		rotate(0, 0) = cos(fai)*cos(kafa) - sin(fai)*sin(omig)*sin(kafa);
		rotate(0, 1) = -cos(fai)*sin(kafa) - sin(fai)*sin(omig)*cos(kafa);
		rotate(0, 2) = -sin(fai)*cos(omig);
		rotate(1, 0) = cos(omig)*sin(kafa);
		rotate(1, 1) = cos(omig)*cos(kafa);
		rotate(1, 2) = -sin(omig);
		rotate(2, 0) = sin(fai)*cos(kafa) + cos(fai)*sin(omig)*sin(kafa);
		rotate(2, 1) = -sin(fai)*sin(kafa) + cos(fai)*sin(omig)*cos(kafa);
		rotate(2, 2) = cos(fai)*cos(omig);
		/*rotate(0, 0) = cos(fai)*cos(kafa);
		rotate(0, 1) = -cos(fai)*sin(kafa);
		rotate(0, 2) = sin(fai);
		rotate(1, 0) = cos(omig)*sin(kafa) + sin(omig)*sin(fai)*cos(kafa);
		rotate(1, 1) = cos(omig)*cos(kafa) - sin(omig)*sin(fai)*sin(kafa);
		rotate(1, 2) = -sin(omig)*cos(fai);
		rotate(2, 0) = sin(omig)*sin(kafa) - cos(omig)*sin(fai)*cos(kafa);
		rotate(2, 1) = sin(omig)*cos(kafa) + cos(omig)*sin(fai)*sin(kafa);
		rotate(2, 2) = cos(omig)*cos(fai);*/
		/*MatrixXd matrixRT = MatrixXd::Random(3, 3);
		matrixRT = rotate.transpose();
		MatrixXd matrixRTR = MatrixXd::Random(3, 3);
		matrixRTR = rotate*matrixRT;
		cout << "R\n" << rotate << endl;
		cout << "RT\n" << matrixRT << endl;
		cout << "RTR\n" << matrixRTR << endl; */
		/************************************************************************/
		/* 计算像点坐标的近似值、常数项矩阵、系数矩阵                           */
		/************************************************************************/
		for (int i = 0; i < iGCPNums; i++)
		{
			//常数项矩阵
			L(3 * i, 0) = PtGCP[i].x - Scale*(rotate(0, 0)*PtImage[i].x + rotate(0, 1)*PtImage[i].y + rotate(0, 2)*PtImage[i].z) - Tx;
			L(3 * i + 1, 0) = PtGCP[i].y - Scale*(rotate(1, 0)*PtImage[i].x + rotate(1, 1)*PtImage[i].y + rotate(1, 2)*PtImage[i].z) - Ty;
			L(3 * i + 2, 0) = PtGCP[i].z - Scale*(rotate(2, 0)*PtImage[i].x + rotate(2, 1)*PtImage[i].y + rotate(2, 2)*PtImage[i].z) - Tz;

			//常系数矩阵
			double XTemp, YTemp, ZTemp;
			XTemp = rotate(0, 0)*PtImage[i].x + rotate(0, 1)*PtImage[i].y + rotate(0, 2)*PtImage[i].z;
			YTemp = rotate(1, 0)*PtImage[i].x + rotate(1, 1)*PtImage[i].y + rotate(1, 2)*PtImage[i].z;
			ZTemp = rotate(2, 0)*PtImage[i].x + rotate(2, 1)*PtImage[i].y + rotate(2, 2)*PtImage[i].z;
			//B(i * 3, 0) = 1.0; B(i * 3, 1) = 0; B(i * 3, 2) = 0; B(i * 3, 3) = PtImage[i].x; B(i * 3, 4) = -PtImage[i].z; B(i * 3, 5) = 0; B(i * 3, 6) = -PtImage[i].y;
			//B(i * 3 + 1, 0) = 0.0; B(i * 3 + 1, 1) = 1.0; B(i * 3 + 1, 2) = 0.0; B(i * 3 + 1, 3) = PtImage[i].y; B(i * 3 + 1, 4) = 0.0; B(i * 3 + 1, 5) = -PtImage[i].z; B(i * 3 + 1, 6) = PtImage[i].x;
			//B(i * 3 + 2, 0) = 0.0; B(i * 3 + 2, 1) = 0.0; B(i * 3 + 2, 2) = 1.0; B(i * 3 + 2, 3) = PtImage[i].z; B(i * 3 + 2, 4) = PtImage[i].x; B(i * 3 + 2, 5) = PtImage[i].y; B(i * 3 + 2, 6) = 0;
			B(i * 3, 0) = 1.0; B(i * 3, 1) = 0; B(i * 3, 2) = 0; B(i * 3, 3) = XTemp; B(i * 3, 4) = -ZTemp; B(i * 3, 5) = 0; B(i * 3, 6) = -YTemp;
			B(i * 3 + 1, 0) = 0.0; B(i * 3 + 1, 1) = 1.0; B(i * 3 + 1, 2) = 0.0; B(i * 3 + 1, 3) = YTemp; B(i * 3 + 1, 4) = 0.0; B(i * 3 + 1, 5) = -ZTemp; B(i * 3 + 1, 6) =XTemp;
			B(i * 3 + 2, 0) = 0.0; B(i * 3 + 2, 1) = 0.0; B(i * 3 + 2, 2) = 1.0; B(i * 3 + 2, 3) = ZTemp; B(i * 3 + 2, 4) = XTemp; B(i * 3 + 2, 5) = YTemp; B(i * 3 + 2, 6) = 0;

		}
		cout << "B\n" << B << endl;
		cout << "L\n" << L << endl;
		/************************************************************************/
		/* 解法方程                                                             */
		/************************************************************************/
		MatrixXd BT = MatrixXd::Random(7, 3*iGCPNums);//中间暂存矩阵
		BT = B.transpose();
		cout << "BT\n" << BT << endl;
		MatrixXd BTB = MatrixXd::Random(7, 7);//中间暂存矩阵
		BTB = BT*B;
		cout << "BTB\n" << BTB << endl;
		MatrixXd BTB_inverse = MatrixXd::Random(7, 7);//中间暂存矩阵
		BTB_inverse = BTB.inverse();
		cout << "BTB_inverse\n" << BTB_inverse << endl;
		MatrixXd BTL = MatrixXd::Random(7, 1);//中间暂存矩阵
		BTL = BT*L;
		cout << "BTL\n" << BTL << endl;
		MatrixXd X = MatrixXd::Random(7, 1);//未知数矩阵
		X = BTB_inverse*BTL;
		cout << "X\n" << X << endl;
		/************************************************************************/
		/* 检查计算是否收敛                                                     */
		/************************************************************************/
		if (fabs(X(4, 0)) < 0.00003 && fabs(X(5, 0)) < 0.00003 && fabs(X(6, 0)) < 0.00003)
		{
			break;
		}
		/************************************************************************/
		/* 累加                                                                 */
		/************************************************************************/
		Tx += X(0, 0);
		Ty += X(1, 0);
		Tz += X(2, 0);
		fai += X(4, 0);
		omig += X(5, 0);
		kafa += X(6, 0);
		Scale += X(3, 0);
		printf("\n第%d次七参数改正量：\n", iteratorID++);
		printf("dTx=%lf\ndTy=%lf\ndTz=%lf\ndfai=%lf\ndomig=%lf\ndkafa=%lf\ndScale=%lf\n", X(0, 0), X(1, 0), X(2, 0), X(4, 0), X(5, 0), X(6, 0), X(3, 0));
	}
	/************************************************************************/
	/* 重新计算旋转矩阵                                               */
	/************************************************************************/
	/*rotate[0][0] = cos(fai)*cos(kafa) - sin(fai)*sin(omig)*sin(kafa);
	rotate[0][1] = -cos(fai)*sin(kafa) - sin(fai)*sin(omig)*cos(kafa);
	rotate[0][2] = -sin(fai)*cos(omig);
	rotate[1][0] = cos(omig)*sin(kafa);
	rotate[1][1] = cos(omig)*cos(kafa);
	rotate[1][2] = -sin(omig);
	rotate[2][0] = sin(fai)*cos(kafa) + cos(fai)*sin(omig)*sin(kafa);
	rotate[2][1] = -sin(fai)*sin(kafa) + cos(fai)*sin(omig)*cos(kafa);
	rotate[2][2] = cos(fai)*cos(omig);*/
	/************************************************************************/
	/* 精度估算                                                             */
	/************************************************************************/
	/*double vv = 0.0, v[8] = { 0.0 };
	double m0 = 0.0;//单位权中误差
	double m[6] = { 0.0 };//未知数中误差
	multiply(&A[0][0], &delta[0], &v[0], 8, 6, 1);
	for (i = 0; i < 8; i++)
	{
	vv += (v[i] - L[i])*(v[i] - L[i]);
	}
	m0 = sqrt(vv / (2 * 4 - 6));
	for (i = 0; i < 6; i++)
	{
	m[i] = m0*sqrt(ATA[i][i]);
	}*/
	/************************************************************************/
	/* 输出旋转矩阵、外方位元素及其精度                                     */
	/************************************************************************/
	//fp = fopen("ResultOut.txt", "w");
	printf("\n像片的外方位元素及其精度为：\n");
	//fprintf(fp, "%s", "\n像片的外方位元素及其精度为：\n");
	printf("Tx=%lf\nTy=%lf\nTz=%lf\nfai=%lf\nomig=%lf\nkafa=%lf\nScale=%lf\n", Tx, Ty, Tz, fai, omig, kafa, Scale);
	//fprintf(fp, "Xs=%lf\tm=%lf\nYs=%lf\tm=%lf\nZs=%lf\tm=%lf\nfai=%lf\tm=%lf\nomig=%lf\tm=%lf\nkafa=%lf\tm=%lf\n",
	//Xs, m[0], Ys, m[1], Zs, m[2], fai, m[3], omig, m[4], kafa, m[5]);
	printf("\n旋转矩阵为:\n");
	//fprintf(fp, "%s", "\n旋转矩阵为:\n");
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			printf("%lf\t", rotate(i, j));
			//fprintf(fp, "%lf\t", rotate[i][j]);
		}
		printf("\n");
		//fputc(10, fp);
	}
	//fclose(fp);
	return true;
}