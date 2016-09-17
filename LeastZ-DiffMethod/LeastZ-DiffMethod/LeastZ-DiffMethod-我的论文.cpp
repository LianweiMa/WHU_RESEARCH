#include <stdio.h>
#include <iostream>
#include <vector>
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
	if (false==NotePrint(argv,argc,3)){printf("Argument: point_file dem_file\n"); return false; }
	char strFeaturePtFile[FILE_PN],strReferenceSurfaceFile[FILE_PN];
	strcpy(strFeaturePtFile, argv[1]);//feature point file
	strcpy(strReferenceSurfaceFile, argv[2]);//reference dem
	//read data
	vector<Point3D> ptSourceFeaturePt, ptTransFeaturePt;
	FILE * fp = fopen(strFeaturePtFile, "r");if (NULL == fp){printf("File can't open: %s\n.",strFeaturePtFile);return false;}
	int iFeaturePtNums = 0;
	fscanf(fp, "%d\n", &iFeaturePtNums);
	for (int i = 0; i < iFeaturePtNums; i++)
	{
		Point3D PtTmp;
		fscanf(fp, "%s %lf %lf %lf\n", PtTmp.strID, &PtTmp.x,&PtTmp.y,&PtTmp.z);
		ptSourceFeaturePt.push_back(PtTmp);
		ptTransFeaturePt.push_back(PtTmp);
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
	double lfTx = 0.0, lfTy = 0.0, lfTz = 0.0, lfScale = 1.0, lfFai = 0.0, lfOmig = 0.0, lfKafa = 0.0;
	printf("\nInitialize para£º\n");
	printf("Tx=%lf Ty=%lf Tz=%lf Scale=%lf Fai=%lf Omig=%lf Kapa=%lf\n", lfTx, lfTy, lfTz, lfScale, lfFai, lfOmig, lfKafa);
	//iterator
	MatrixXd matrixR = MatrixXd::Random(3, 3);
	MatrixXd matrixA = MatrixXd::Random(iFeaturePtNums, 7);
	MatrixXd matrixL = MatrixXd::Random(iFeaturePtNums, 1);
	int iIterator = 1;
	while (true){
		//1
		matrixR(0, 0) = cos(lfFai)*cos(lfKafa) - sin(lfFai)*sin(lfOmig)*sin(lfKafa);
		matrixR(0, 1) = -cos(lfFai)*sin(lfKafa) - sin(lfFai)*sin(lfOmig)*cos(lfKafa);
		matrixR(0, 2) = -sin(lfFai)*cos(lfOmig);
		matrixR(1, 0) = cos(lfOmig)*sin(lfKafa);
		matrixR(1, 1) = cos(lfOmig)*cos(lfKafa);
		matrixR(1, 2) = -sin(lfOmig);
		matrixR(2, 0) = sin(lfFai)*cos(lfKafa) + cos(lfFai)*sin(lfOmig)*sin(lfKafa);
		matrixR(2, 1) = -sin(lfFai)*sin(lfKafa) + cos(lfFai)*sin(lfOmig)*cos(lfKafa);
		matrixR(2, 2) = cos(lfFai)*cos(lfOmig);

		for (int i = 0; i < iFeaturePtNums; i++){
			Point3D TransPt;
			TransPt.x = lfScale*(matrixR(0, 0)*ptSourceFeaturePt[i].x + matrixR(0, 1)*ptSourceFeaturePt[i].y + matrixR(0, 2)*ptSourceFeaturePt[i].z) + lfTx;
			TransPt.y = lfScale*(matrixR(1, 0)*ptSourceFeaturePt[i].x + matrixR(1, 1)*ptSourceFeaturePt[i].y + matrixR(1, 2)*ptSourceFeaturePt[i].z) + lfTy;
			TransPt.z = lfScale*(matrixR(2, 0)*ptSourceFeaturePt[i].x + matrixR(2, 1)*ptSourceFeaturePt[i].y + matrixR(2, 2)*ptSourceFeaturePt[i].z) + lfTz;
			ptTransFeaturePt[i].x = TransPt.x;
			ptTransFeaturePt[i].y = TransPt.y;
			ptTransFeaturePt[i].z = TransPt.z;

			TransPt.z = pReffDemFile->GetDemZValue(TransPt.x, TransPt.y);
			ptReferenceSurfacePt[i].x = TransPt.x;
			ptReferenceSurfacePt[i].y = TransPt.y;
			ptReferenceSurfacePt[i].z = TransPt.z;
			printf("%lf %lf %lf\n", ptTransFeaturePt[i].x, ptTransFeaturePt[i].y, ptTransFeaturePt[i].z);
			printf("%lf %lf %lf\t%lf\n\n", ptReferenceSurfacePt[i].x, ptReferenceSurfacePt[i].y, ptReferenceSurfacePt[i].z, fabs(ptTransFeaturePt[i].z - ptReferenceSurfacePt[i].z));
		}
		//compute
		for (int i = 0; i < iFeaturePtNums; i++){
			double lfDfx = 0.0, lfDfy = 0.0;
			lfDfx = (pReffDemFile->GetDemZValue(ptReferenceSurfacePt[i].x + lfXGsd, ptReferenceSurfacePt[i].y) - pReffDemFile->GetDemZValue(ptReferenceSurfacePt[i].x - lfXGsd, ptReferenceSurfacePt[i].y)) / (2 * lfXGsd);
			lfDfy = (pReffDemFile->GetDemZValue(ptReferenceSurfacePt[i].x, ptReferenceSurfacePt[i].y + lfYGsd) - pReffDemFile->GetDemZValue(ptReferenceSurfacePt[i].x, ptReferenceSurfacePt[i].y - lfYGsd)) / (2 * lfYGsd);
			matrixA(i , 0) = lfDfx; 
			matrixA(i , 1) = lfDfy;
			matrixA(i , 2) = -1.0;
			matrixA(i , 3) = lfDfx*ptSourceFeaturePt[i].x+lfDfy*ptSourceFeaturePt[i].y-ptSourceFeaturePt[i].z; 
			matrixA(i , 4) = -ptSourceFeaturePt[i].x-lfDfx*ptSourceFeaturePt[i].z;
			matrixA(i , 5) = -ptSourceFeaturePt[i].y-lfDfy*ptSourceFeaturePt[i].z;
			matrixA(i , 6) = lfDfy*ptSourceFeaturePt[i].x-lfDfx*ptSourceFeaturePt[i].y;

			matrixL(i, 0) = (ptReferenceSurfacePt[i].z - ptTransFeaturePt[i].z);
		}
		cout << "A\n" << matrixA << endl;
		cout << "L\n" << matrixL << endl;
		//jie fangcheng
		MatrixXd matrixAT = MatrixXd::Random(7,iFeaturePtNums);
		matrixAT = matrixA.transpose();
		//cout << "AT\n" << matrixAT << endl;

		MatrixXd matrixATA = MatrixXd::Random(7, 7);
		matrixATA = matrixAT*matrixA;
		//cout << "ATA\n" << matrixATA << endl;

		MatrixXd matrixATAInverse = MatrixXd::Random(7, 7);
		matrixATAInverse = matrixATA.inverse();
		//cout << "ATA_inverse\n" << matrixATAInverse << endl;
		
		MatrixXd matrixATL = MatrixXd::Random(7, 1);
		matrixATL = matrixAT*matrixL;
		//cout << "ATL\n" << matrixATL << endl;

		MatrixXd matrixX = MatrixXd::Random(7, 1);//X
		matrixX = matrixATAInverse*matrixATL;
		//cout << "X\n" << X << endl;
		printf("Iteration %d£º\n", iIterator++);
		printf("DTx=%lf DTy=%lf DTz=%lf DScale=%lf DFai=%lf DOmig=%lf DKapa=%lf\n", matrixX(0, 0), matrixX(1, 0), matrixX(2, 0), matrixX(3, 0), matrixX(4, 0), matrixX(5, 0), matrixX(6, 0));

		//break
		if (iIterator>3 || 
			(fabs(matrixX(0, 0))< 0.01*lfXGsd && fabs(matrixX(1, 0))< 0.01*lfYGsd && fabs(matrixX(2, 0))< 0.01*lfXGsd &&
			fabs(matrixX(3, 0)) < 0.05*1.0e-2 && 
			fabs(matrixX(4, 0)) < 0.5*1.0e-3 && fabs(matrixX(5, 0)) < 0.5*1.0e-3 && fabs(matrixX(6, 0)) < 0.5*1.0e-3)
			) break;
		lfTx += matrixX(0, 0);
		lfTy += matrixX(1, 0);
		lfTz += matrixX(2, 0);
		lfScale += matrixX(3, 0);
		lfFai += matrixX(4, 0);
		lfOmig += matrixX(5, 0);
		lfKafa += matrixX(6, 0);		
		MatrixXd matrixV = MatrixXd::Random(7, 1);//Vz
		matrixV = matrixA*matrixX - matrixL;
		cout << "matrixV\n" << matrixV << endl;
	}
	//output
	printf("\nSeven Para result£º\n");
	printf("Tx=%lf Ty=%lf Tz=%lf Scale=%lf Fai=%lf Omig=%lf Kapa=%lf\n", lfTx, lfTy, lfTz, lfScale, lfFai, lfOmig, lfKafa);
	
	delete pReffDemFile, pReffDemHead;
	return true;
}