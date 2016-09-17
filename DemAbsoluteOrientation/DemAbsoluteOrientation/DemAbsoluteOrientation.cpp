#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <windows.h>
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/CommonOP.hpp"
#include "Eigen_3.0.5/include/Eigen/Dense"
#include "Eigen_3.0.5/include/Eigen/StdVector"
#include "ResearchCode/CGdalDem.hpp"
#include "Whu/SpGeoCvt.h"
using namespace std;
using namespace Eigen;
typedef struct Point3D
{
	Vector3d pt;
	char id[CHAR_LEN];
} Point3D;
int main(int argc, char**argv)
{
	if (NotePrint(argv, argc, 3) == false){ printf("Argument : exe gcp_file dem_file\n"); return false; }
	printf("Loading...\n");
	char strGCPfile[FILE_PN], strAbsoOrieDemFile[FILE_PN];
	strcpy(strGCPfile, argv[1]); strcpy(strAbsoOrieDemFile, argv[2]);
	//load gcp file
	FILE *fp = fopen(strGCPfile, "r"); if (NULL == fp)return false;
	int iGcpNums = 0; fscanf(fp, "%d\n", &iGcpNums);
	vector<Point3D, Eigen::aligned_allocator<Eigen::Vector3d>> ptGcp, ptGcpUsed, ptCorrPt, ptUTMCorrPt;
	CSpGeoCvt SpGeoCvt;
	printf("Set input coordinate...\n");
	SpGeoCvt.Set_Cvt_Par_ByGUI();
	for (int i = 0; i < iGcpNums; i++)
	{
		Point3D ptTmp,ptCvt;
		fscanf(fp, "%s %lf %lf %lf\n", ptTmp.id, &ptTmp.pt(0), &ptTmp.pt(1), &ptTmp.pt(2));
		ptCvt = ptTmp;
		//UTM-to-CXYZ
		SpGeoCvt.Cvt_Prj2LBH(ptTmp.pt(0), ptTmp.pt(1), ptTmp.pt(2), &ptCvt.pt(0), &ptCvt.pt(1), &ptCvt.pt(2));
		SpGeoCvt.Cvt_LBH2CXYZ(ptCvt.pt(0), ptCvt.pt(1), ptCvt.pt(2), &ptTmp.pt(0), &ptTmp.pt(1), &ptTmp.pt(2));
		ptGcp.push_back(ptTmp);
	}
	fclose(fp);
	//load dem file
	CGdalDem *pAbsoOrieDemFile=new CGdalDem;
	GDALDEMHDR *pAbsoOrieDemHead = new GDALDEMHDR;
	if (false == pAbsoOrieDemFile->LoadFile(strAbsoOrieDemFile, pAbsoOrieDemHead))return false;
	int iCols = pAbsoOrieDemHead->iCol, iRows = pAbsoOrieDemHead->iRow;
	double lfGsdX = pAbsoOrieDemHead->lfGsdX, lfGsdY = pAbsoOrieDemHead->lfGsdY;
	
	//least z-difference method
	VectorXd vecPara(7);//omig,fai,kafa,scale,tx,ty,tz
	vecPara << 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0;
	Matrix3d matR;
	matR << 1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0;
	int iIter = 1;
	while (true)
	{
		printf("Iterator: %d\n", iIter++);
		//rotate matrix
		matR(0, 0) = cos(vecPara(1))*cos(vecPara(2));
		matR(0, 1) = -cos(vecPara(1))*sin(vecPara(2));
		matR(0, 2) = sin(vecPara(1));
		matR(1, 0) = cos(vecPara(0))*sin(vecPara(2)) + sin(vecPara(0))*sin(vecPara(1))*cos(vecPara(2));
		matR(1, 1) = cos(vecPara(0))*cos(vecPara(2)) - sin(vecPara(0))*sin(vecPara(1))*sin(vecPara(2));
		matR(1, 2) = -sin(vecPara(0))*cos(vecPara(1));
		matR(2, 0) = sin(vecPara(0))*sin(vecPara(2)) - cos(vecPara(0))*sin(vecPara(1))*cos(vecPara(2));
		matR(2, 1) = sin(vecPara(0))*cos(vecPara(2)) + cos(vecPara(0))*sin(vecPara(1))*sin(vecPara(2));
		matR(2, 2) = cos(vecPara(0))*cos(vecPara(1));
		Matrix3d matInvR = matR.inverse();
		//find corresponding points
		ptGcpUsed.clear(); ptCorrPt.clear();
		//printf("Set output coordinate...\n");
		//SpGeoCvt.Set_Cvt_Par_ByGUI();
		for (int i = 0; i < iGcpNums; i++)
		{
			Point3D ptTmp = ptGcp[i], ptCvt = ptGcp[i];//CXYZ
			Vector3d vecT(vecPara(4), vecPara(5), vecPara(6));
			ptTmp.pt = (1 / vecPara(3))*matInvR*(ptGcp[i].pt - vecT);
			SpGeoCvt.Cvt_CXYZ2LBH(ptTmp.pt(0), ptTmp.pt(1), ptTmp.pt(2), &ptCvt.pt(0), &ptCvt.pt(1), &ptCvt.pt(2));//ptCvt-LBH	
			SpGeoCvt.Cvt_LBH2Prj(ptCvt.pt(0), ptCvt.pt(1), ptCvt.pt(2), &ptTmp.pt(0), &ptTmp.pt(1), &ptTmp.pt(2));//ptTmp-Prj
			ptTmp.pt(2) = pAbsoOrieDemFile->GetDemZValue(ptTmp.pt(0), ptTmp.pt(1));
			if (ptTmp.pt(2) != NODATA)
			{
				ptGcpUsed.push_back(ptGcp[i]);//CXYZ
				ptUTMCorrPt.push_back(ptTmp);//Prj

				SpGeoCvt.Cvt_Prj2LBH(ptTmp.pt(0), ptTmp.pt(1), ptTmp.pt(2), &ptCvt.pt(0), &ptCvt.pt(1), &ptCvt.pt(2));
				SpGeoCvt.Cvt_LBH2CXYZ(ptCvt.pt(0), ptCvt.pt(1), ptCvt.pt(2), &ptTmp.pt(0), &ptTmp.pt(1), &ptTmp.pt(2));
				ptCorrPt.push_back(ptTmp);//CXYZ
			}
		}
		if (ptGcpUsed.size() <= 0){printf("Too few points, stop iteration!\n"); break;	}
		//coordinate preprocess
		Vector3d vecMinxyz(ptGcpUsed[0].pt(0), ptGcpUsed[0].pt(1), ptGcpUsed[0].pt(2));
		for (unsigned i = 1; i < ptGcpUsed.size(); i++)
		{
			if (vecMinxyz(0) > ptGcpUsed[i].pt(0))vecMinxyz(0) = ptGcpUsed[i].pt(0);
			if (vecMinxyz(1) > ptGcpUsed[i].pt(1))vecMinxyz(1) = ptGcpUsed[i].pt(1);
			if (vecMinxyz(2) > ptGcpUsed[i].pt(2))vecMinxyz(2) = ptGcpUsed[i].pt(2);
		}
		/*Vector3d vecCenterxyz(0.0, 0.0, 0.0);
		for (unsigned i = 1; i < ptGcpUsed.size(); i++)
		{
			vecCenterxyz(0) += ptGcpUsed[i].pt(0);
			vecCenterxyz(1) += ptGcpUsed[i].pt(1);
			vecCenterxyz(2) += ptGcpUsed[i].pt(2);
		}
		vecCenterxyz(0) /= ptGcpUsed.size();
		vecCenterxyz(1) /= ptGcpUsed.size();
		vecCenterxyz(2) /= ptGcpUsed.size();*/
		for (unsigned i = 0; i < ptGcpUsed.size();i++)
		{
			ptCorrPt[i].pt -= vecMinxyz;
			ptGcpUsed[i].pt -= vecMinxyz;
		}
		//Least z-difference
		VectorXd vecL(ptGcpUsed.size());
		MatrixXd matA(ptGcpUsed.size(), 3 * ptGcpUsed.size());
		for (unsigned i = 0; i < ptGcpUsed.size(); i++)
		{
			for (unsigned j = 0; j < 3*ptGcpUsed.size(); j++)
			{
				matA(i, j) = 0.0;
			}
		}
		MatrixXd matB(3 * ptGcpUsed.size(), 7);
		//compute matA matB vecL
		for (unsigned int i = 0; i < ptGcpUsed.size(); i++)
		{
			//vecL
			vecL(i) = ptCorrPt[i].pt(2) - ptGcpUsed[i].pt(2);
			//matA
			matA(i, 3 * i + 0) = (pAbsoOrieDemFile->GetDemZValue(ptUTMCorrPt[i].pt(0) + lfGsdX, ptUTMCorrPt[i].pt(1)) - pAbsoOrieDemFile->GetDemZValue(ptUTMCorrPt[i].pt(0) - lfGsdX, ptUTMCorrPt[i].pt(1))) / (2 * lfGsdX);
			matA(i, 3 * i + 1) = (pAbsoOrieDemFile->GetDemZValue(ptUTMCorrPt[i].pt(0), ptUTMCorrPt[i].pt(1) + lfGsdY) - pAbsoOrieDemFile->GetDemZValue(ptUTMCorrPt[i].pt(0), ptUTMCorrPt[i].pt(1) - lfGsdY)) / (2 * lfGsdY);
			matA(i, 3 * i + 2) = -1.0;
			//matB
			matB(i * 3, 0) = 0.0;
			matB(i * 3, 1) = ptCorrPt[i].pt(2);
			matB(i * 3, 2) = -ptCorrPt[i].pt(1);
			matB(i * 3, 3) = ptCorrPt[i].pt(0);
			matB(i * 3, 4) = 1.0;
			matB(i * 3, 5) = 0.0;
			matB(i * 3, 6) = 0.0;
			matB(i * 3 + 1, 0) = -ptCorrPt[i].pt(2);
			matB(i * 3 + 1, 1) = 0.0;
			matB(i * 3 + 1, 2) = ptCorrPt[i].pt(0);
			matB(i * 3 + 1, 3) = ptCorrPt[i].pt(1);
			matB(i * 3 + 1, 4) = 0.0;
			matB(i * 3 + 1, 5) = 1.0;
			matB(i * 3 + 1, 6) = 0.0;
			matB(i * 3 + 2, 0) = ptCorrPt[i].pt(1);
			matB(i * 3 + 2, 1) = -ptCorrPt[i].pt(0);
			matB(i * 3 + 2, 2) = 0.0;
			matB(i * 3 + 2, 3) = ptCorrPt[i].pt(2);
			matB(i * 3 + 2, 4) = 0.0;
			matB(i * 3 + 2, 5) = 0.0;
			matB(i * 3 + 2, 6) = 1.0;
		}
		//cout << "A" << matA << endl;
		//cout << "B" << matB << endl;
		//cout << "L" << vecL << endl;
		//A*B
		MatrixXd matAB(ptGcpUsed.size(), 7);
		matAB = matA*matB;
		//ABT
		MatrixXd matABT(7, ptGcpUsed.size());
		matABT = matAB.transpose();
		//ABT*AB
		MatrixXd matABTAB(7, 7);
		matABTAB = matABT*matAB;
		//inverse ABTAB
		MatrixXd matInvABTAB(7, 7);
		matInvABTAB = matABTAB.inverse();
		//ABT*L
		VectorXd vecABTL(7);
		vecABTL = matABT*vecL;
		//X
		VectorXd vecX(7);
		vecX = matInvABTAB*vecABTL;
		printf("domig\t\tdfai\t\tdkafa\t\tdScale\t\tdTx\t\tdTy\t\tdTz\n%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", vecX(0), vecX(1), vecX(2), vecX(3), vecX(4), vecX(5), vecX(6));
		//judge convergence
		if (iIter>100||
			(fabs(vecX(0))<0.5*1.0e-3 && fabs(vecX(1))<0.5*1.0e-3 && fabs(vecX(2))<0.5*1.0e-3 &&
			fabs(vecX(3))<0.05*1.0e-2 &&
			fabs(vecX(4))<0.01*lfGsdX && fabs(vecX(5))<0.01*lfGsdY && fabs(vecX(6))<0.01)) break;
		//continue
		vecPara += vecX;
	}
	printf("\nPara7 result:\nomig\t\tfai\t\tkafa\t\tScale\t\tTx\t\tTy\t\tTz\n%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", vecPara(0), vecPara(1), vecPara(2), vecPara(3), vecPara(4), vecPara(5), vecPara(6));
	delete pAbsoOrieDemFile, pAbsoOrieDemHead;
	return true;
}