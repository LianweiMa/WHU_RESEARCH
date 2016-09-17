#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <windows.h>
#include "Whu/SpGeoCvt.h"
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/CommonOP.hpp"
#include "Eigen_3.0.5/include/Eigen/Dense"
#include "Eigen_3.0.5/include/Eigen/StdVector"
#include "ResearchCode/CGdalDem.hpp"
#define FEATURE_PT_NUM 6000
using namespace std;
using namespace Eigen;
typedef struct Point3D
{
	Vector3d pt;
	int id;
} Point3D;
int main(int argc, char**argv)
{
	if (NotePrint(argv, argc, 3) == false){ printf("Argument : exe source_dem_file  ref_dem_file\n"); return false; }
	printf("Loading...\n");
	char strSrcDemfile[FILE_PN], strRefDemFile[FILE_PN];
	strcpy(strSrcDemfile, argv[1]); strcpy(strRefDemFile, argv[2]);
	//load dem file
	CGdalDem *pSrcDemFile = new CGdalDem, *pRefDemFile = new CGdalDem;
	GDALDEMHDR *pSrcDemHead = new GDALDEMHDR, *pRefDemHead = new GDALDEMHDR;
	if (false == pSrcDemFile->LoadFile(strSrcDemfile, pSrcDemHead)||false==pRefDemFile->LoadFile(strRefDemFile,pRefDemHead))return false;
	
	//compute overlay para
	//Source DEM最值
	double lfXmaxSrc = pSrcDemHead->lfStartX + pSrcDemHead->lfGsdX*pSrcDemHead->iCol;
	double lfYmaxSrc = pSrcDemHead->lfStartY + pSrcDemHead->lfGsdY*pSrcDemHead->iRow;
	double lfXminSrc = pSrcDemHead->lfStartX;
	double lfYminSrc = pSrcDemHead->lfStartY;
	//Ref DEM最值
	double lfXmaxRef = pRefDemHead->lfStartX + pRefDemHead->lfGsdX*pRefDemHead->iCol;
	double lfYmaxRef = pRefDemHead->lfStartY + pRefDemHead->lfGsdY*pRefDemHead->iRow;
	double lfXminRef = pRefDemHead->lfStartX;
	double lfYminRef = pRefDemHead->lfStartY;
	//overlay para
	double lfOverlayXmin = 0.0, lfOverlayXmax = 0.0, lfOverlayYmin = 0.0, lfOverlayYmax = 0.0;
	if (lfXminSrc > lfXminRef)lfOverlayXmin = lfXminSrc;
	else lfOverlayXmin = lfXminRef;
	if (lfXmaxSrc > lfXmaxRef)lfOverlayXmax = lfXmaxRef;
	else lfOverlayXmax = lfXmaxSrc;
	if (lfYminSrc > lfYminRef)lfOverlayYmin = lfYminSrc;
	else lfOverlayYmin = lfYminRef;
	if (lfYmaxSrc > lfYmaxRef) lfOverlayYmax = lfYmaxRef;
	else lfOverlayYmax = lfYmaxSrc;
	//find feature point of source overlay dem file
	double lfK = (lfOverlayXmax - lfOverlayXmin) / (lfOverlayYmax - lfOverlayYmin);
	int iYPtNums = int(pow(FEATURE_PT_NUM / lfK, 0.5));
	int iXPtNums = int(lfK*iYPtNums);
	double lfXInterval = int((lfOverlayXmax - lfOverlayXmin) / iXPtNums);
	double lfYInterval = int((lfOverlayYmax - lfOverlayYmin) / iYPtNums);
	int iPtIndex = 1;
	vector<Point3D, Eigen::aligned_allocator<Eigen::Vector3d>> ptFeaturePt, ptSourceFeaturePt, ptUsedFeaturePt, ptReferenceSurfacePt, ptPrjReferenceSurfacePt;
	CSpGeoCvt SpGeoCvt;
	printf("Set input coordinate system...\n");
	SpGeoCvt.Set_Cvt_Par_ByGUI();
	printf("Over\n");
	printf("Finding feature point...\n");
	bool bFlag = false;
	for (int i = 0; i < iYPtNums; i++){
		for (int j = 0; j < iXPtNums; j++){
			Point3D ptTmp, ptCvt;
			double lfX = lfOverlayXmin + j*lfXInterval, lfY = lfOverlayYmin + i*lfYInterval;
			/*for (double y = lfY; y < lfY + lfYInterval; y+=pSrcDemHead->lfGsdY)
			{
				for (double x = lfX; x < lfX + lfXInterval; x+=pSrcDemHead->lfGsdX)
				{
					float fZ1 = pSrcDemFile->GetDemZValue(x - pSrcDemHead->lfGsdX, y + pSrcDemHead->lfGsdY);
					float fZ2 = pSrcDemFile->GetDemZValue(x, y + pSrcDemHead->lfGsdY);
					float fZ3 = pSrcDemFile->GetDemZValue(x + pSrcDemHead->lfGsdX, y + pSrcDemHead->lfGsdY);
					float fZ4 = pSrcDemFile->GetDemZValue(x - pSrcDemHead->lfGsdX, y);
					float fZ5 = pSrcDemFile->GetDemZValue(x, y);
					float fZ6 = pSrcDemFile->GetDemZValue(x + pSrcDemHead->lfGsdX, y);
					float fZ7 = pSrcDemFile->GetDemZValue(x - pSrcDemHead->lfGsdX, y - pSrcDemHead->lfGsdY);
					float fZ8 = pSrcDemFile->GetDemZValue(x, y - pSrcDemHead->lfGsdY);
					float fZ9 = pSrcDemFile->GetDemZValue(x + pSrcDemHead->lfGsdX, y - pSrcDemHead->lfGsdY);
				
					if (false==bFlag)//谷点
					{
						if (fZ1 != NODATA&&fZ2 != NODATA&&fZ3 != NODATA&&fZ4 != NODATA&&fZ5 != NODATA&&fZ6 != NODATA&&fZ7 != NODATA&&fZ8 != NODATA&&fZ9 != NODATA)
						{
							if (fZ5<fZ1&&fZ5<fZ2&&fZ5<fZ3&&fZ5<fZ4&&fZ5<fZ6&&fZ5<fZ7&&fZ5<fZ8&&fZ5<fZ9)
							{
								ptTmp.pt(0) = x; ptTmp.pt(1) = y; ptTmp.pt(2) = fZ5; ptTmp.id = iPtIndex; bFlag = true;
								ptCvt = ptTmp;
								ptFeaturePt.push_back(ptTmp);
								//UTM-to-CXYZ
								SpGeoCvt.Cvt_Prj2LBH(ptTmp.pt(0), ptTmp.pt(1), ptTmp.pt(2), &ptCvt.pt(0), &ptCvt.pt(1), &ptCvt.pt(2));
								SpGeoCvt.Cvt_LBH2CXYZ(ptCvt.pt(0), ptCvt.pt(1), ptCvt.pt(2), &ptTmp.pt(0), &ptTmp.pt(1), &ptTmp.pt(2));
								ptSourceFeaturePt.push_back(ptTmp); / *CXYZ* / iPtIndex++;
								x = lfX + lfXInterval; y = lfY + lfYInterval;//break;
							}
						}
					}
					else//山顶
					{
						if (fZ1 != NODATA&&fZ2 != NODATA&&fZ3 != NODATA&&fZ4 != NODATA&&fZ5 != NODATA&&fZ6 != NODATA&&fZ7 != NODATA&&fZ8 != NODATA&&fZ9 != NODATA)
						{
							if (fZ5 > fZ1&&fZ5 > fZ2&&fZ5 > fZ3&&fZ5 > fZ4&&fZ5 > fZ6&&fZ5 > fZ7&&fZ5 > fZ8&&fZ5 > fZ9)
							{
								ptTmp.pt(0) = x; ptTmp.pt(1) = y; ptTmp.pt(2) = fZ5; ptTmp.id = iPtIndex; bFlag = false;
								ptCvt = ptTmp;
								ptFeaturePt.push_back(ptTmp);
								//UTM-to-CXYZ
								SpGeoCvt.Cvt_Prj2LBH(ptTmp.pt(0), ptTmp.pt(1), ptTmp.pt(2), &ptCvt.pt(0), &ptCvt.pt(1), &ptCvt.pt(2));
								SpGeoCvt.Cvt_LBH2CXYZ(ptCvt.pt(0), ptCvt.pt(1), ptCvt.pt(2), &ptTmp.pt(0), &ptTmp.pt(1), &ptTmp.pt(2));
								ptSourceFeaturePt.push_back(ptTmp);/ *CXYZ* / iPtIndex++;
								x = lfX + lfXInterval; y = lfY + lfYInterval;//break;
							}
						}
					}
				}
			}*/
			float fZ = pSrcDemFile->GetDemZValue(lfX, lfY);
			if (fZ != NODATA){
				ptTmp.pt(0) = lfX; ptTmp.pt(1) = lfY; ptTmp.pt(2) = fZ; ptTmp.id = iPtIndex;
				ptCvt = ptTmp;
				ptFeaturePt.push_back(ptTmp);
				//UTM-to-CXYZ
				SpGeoCvt.Cvt_Prj2LBH(ptTmp.pt(0), ptTmp.pt(1), ptTmp.pt(2), &ptCvt.pt(0), &ptCvt.pt(1), &ptCvt.pt(2));
				SpGeoCvt.Cvt_LBH2CXYZ(ptCvt.pt(0), ptCvt.pt(1), ptCvt.pt(2), &ptTmp.pt(0), &ptTmp.pt(1), &ptTmp.pt(2));
				ptSourceFeaturePt.push_back(ptTmp); iPtIndex++;
			}	
		}
	}
	char strOutputFile[FILE_PN];
	strcpy(strOutputFile, strSrcDemfile);
	sprintf(strrchr(strOutputFile, '.'), "%s", "_特征点.txt");
	FILE * fp = fopen(strOutputFile, "w");
	if (NULL == fp){ printf("File can't open: %s\n.", strOutputFile); return false; }
	fprintf(fp, "%d\n", ptFeaturePt.size());
	for (unsigned int i = 0; i < ptFeaturePt.size(); i++){
		fprintf(fp, "%d\t%lf\t%lf\t%f\n", i + 1, ptFeaturePt[i].pt(0), ptFeaturePt[i].pt(1), ptFeaturePt[i].pt(2));
	}
	printf("提取出的特征点数: %d\nOver!\n", ptFeaturePt.size());
	fclose(fp);
	//iteration
	VectorXd vecPara(7);//omig,fai,kafa,scale,tx,ty,tz
	vecPara << 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0;
	Matrix3d matR;
	matR << 1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0;
	int iIter = 1;
	Vector3d vecMinxyz(0.0,0.0,0.0);
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
		//find corresponding points
		ptUsedFeaturePt.clear(); ptReferenceSurfacePt.clear();
		for (unsigned i = 0; i < ptSourceFeaturePt.size(); i++)
		{
			Point3D ptTmp = ptSourceFeaturePt[i], ptCvt = ptSourceFeaturePt[i];
			Vector3d vecT(vecPara(4), vecPara(5), vecPara(6));//shift para
			ptTmp.pt = vecPara(3)*matR*(ptSourceFeaturePt[i].pt - vecMinxyz) + vecT;
			ptTmp.pt += vecMinxyz;
			SpGeoCvt.Cvt_CXYZ2LBH(ptTmp.pt(0), ptTmp.pt(1), ptTmp.pt(2), &ptCvt.pt(0), &ptCvt.pt(1), &ptCvt.pt(2));//ptCvt-LBH	
			SpGeoCvt.Cvt_LBH2Prj(ptCvt.pt(0), ptCvt.pt(1), ptCvt.pt(2), &ptTmp.pt(0), &ptTmp.pt(1), &ptTmp.pt(2));//ptTmp-Prj
			ptTmp.pt(2) = pRefDemFile->GetDemZValue(ptTmp.pt(0), ptTmp.pt(1));
			if (ptTmp.pt(2) != NODATA)
			{
				ptUsedFeaturePt.push_back(ptSourceFeaturePt[i]);//CXYZ
				ptPrjReferenceSurfacePt.push_back(ptTmp);//Prj

				SpGeoCvt.Cvt_Prj2LBH(ptTmp.pt(0), ptTmp.pt(1), ptTmp.pt(2), &ptCvt.pt(0), &ptCvt.pt(1), &ptCvt.pt(2));
				SpGeoCvt.Cvt_LBH2CXYZ(ptCvt.pt(0), ptCvt.pt(1), ptCvt.pt(2), &ptTmp.pt(0), &ptTmp.pt(1), &ptTmp.pt(2));
				ptReferenceSurfacePt.push_back(ptTmp);//CXYZ
			}
		}
		printf("Feature point numbers: %d\n", ptUsedFeaturePt.size());
		if (ptUsedFeaturePt.size() < 7){ printf("Too few points, stop iteration!\n"); break; }
		/*vecMinxyz<<ptUsedFeaturePt[0].pt(0), ptUsedFeaturePt[0].pt(1), ptUsedFeaturePt[0].pt(2);
		for (unsigned i = 1; i < ptUsedFeaturePt.size(); i++)
		{
			if (vecMinxyz(0) > ptUsedFeaturePt[i].pt(0))vecMinxyz(0) = ptUsedFeaturePt[i].pt(0);
			if (vecMinxyz(1) > ptUsedFeaturePt[i].pt(1))vecMinxyz(1) = ptUsedFeaturePt[i].pt(1);
			if (vecMinxyz(2) > ptUsedFeaturePt[i].pt(2))vecMinxyz(2) = ptUsedFeaturePt[i].pt(2);
		}*/
		for (unsigned i = 0; i < ptUsedFeaturePt.size(); i++)
		{
			ptUsedFeaturePt[i].pt -= vecMinxyz;
			ptReferenceSurfacePt[i].pt -= vecMinxyz;
		}
		//Least z-difference
		VectorXd vecL(ptUsedFeaturePt.size());
		MatrixXd matA(ptUsedFeaturePt.size(), 3 * ptUsedFeaturePt.size());
		for (unsigned i = 0; i < ptUsedFeaturePt.size(); i++)
		{
			for (unsigned j = 0; j < 3 * ptUsedFeaturePt.size(); j++)
			{
				matA(i, j) = 0.0;
			}
		}
		MatrixXd matB(3 * ptUsedFeaturePt.size(), 7);
		//compute matA matB vecL
		for (unsigned int i = 0; i < ptUsedFeaturePt.size(); i++)
		{
			//vecL
			vecL(i) = ptUsedFeaturePt[i].pt(2) - ptReferenceSurfacePt[i].pt(2);
			//matA
			matA(i, 3 * i + 0) = (pRefDemFile->GetDemZValue(ptPrjReferenceSurfacePt[i].pt(0) + pRefDemHead->lfGsdX, ptPrjReferenceSurfacePt[i].pt(1)) - pRefDemFile->GetDemZValue(ptPrjReferenceSurfacePt[i].pt(0) - pRefDemHead->lfGsdX, ptPrjReferenceSurfacePt[i].pt(1))) / (2 * pRefDemHead->lfGsdX);
			matA(i, 3 * i + 1) = (pRefDemFile->GetDemZValue(ptPrjReferenceSurfacePt[i].pt(0), ptPrjReferenceSurfacePt[i].pt(1) + pRefDemHead->lfGsdY) - pRefDemFile->GetDemZValue(ptPrjReferenceSurfacePt[i].pt(0), ptPrjReferenceSurfacePt[i].pt(1) - pRefDemHead->lfGsdY)) / (2 * pRefDemHead->lfGsdY);
			matA(i, 3 * i + 2) = -1.0;
			//matB
			matB(i * 3, 0) = 0.0;
			matB(i * 3, 1) = ptUsedFeaturePt[i].pt(2);
			matB(i * 3, 2) = -ptUsedFeaturePt[i].pt(1);
			matB(i * 3, 3) = ptUsedFeaturePt[i].pt(0);
			matB(i * 3, 4) = 1.0;
			matB(i * 3, 5) = 0.0;
			matB(i * 3, 6) = 0.0;
			matB(i * 3 + 1, 0) = -ptUsedFeaturePt[i].pt(2);
			matB(i * 3 + 1, 1) = 0.0;
			matB(i * 3 + 1, 2) = ptUsedFeaturePt[i].pt(0);
			matB(i * 3 + 1, 3) = ptUsedFeaturePt[i].pt(1);
			matB(i * 3 + 1, 4) = 0.0;
			matB(i * 3 + 1, 5) = 1.0;
			matB(i * 3 + 1, 6) = 0.0;
			matB(i * 3 + 2, 0) = ptUsedFeaturePt[i].pt(1);
			matB(i * 3 + 2, 1) = -ptUsedFeaturePt[i].pt(0);
			matB(i * 3 + 2, 2) = 0.0;
			matB(i * 3 + 2, 3) = ptUsedFeaturePt[i].pt(2);
			matB(i * 3 + 2, 4) = 0.0;
			matB(i * 3 + 2, 5) = 0.0;
			matB(i * 3 + 2, 6) = 1.0;
		}
		//cout << "A" << matA << endl;
		//cout << "B" << matB << endl;
		//cout << "L" << vecL << endl;
		//A*B
		MatrixXd matAB(ptUsedFeaturePt.size(), 7);
		matAB = matA*matB;
		//ABT
		MatrixXd matABT(7, ptUsedFeaturePt.size());
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
		//if (iIter>100 ||(fabs(vecX(6))<0.5)) break;
		if (iIter > 30 ||
			(fabs(vecX(0)) < 0.5*1.0e-3 && fabs(vecX(1)) < 0.5*1.0e-3 && fabs(vecX(2)) < 0.5*1.0e-3 &&
			fabs(vecX(3)) < 0.05*1.0e-2 &&
			fabs(vecX(4)) < 0.01*pSrcDemHead->lfGsdX && fabs(vecX(5)) < 0.01*pSrcDemHead->lfGsdY && fabs(vecX(6)) < 0.01*pSrcDemHead->lfGsdX)) break;
		//continue
		vecPara += vecX;
	}
	printf("\nPara7 result:\nomig\t\tfai\t\tkafa\t\tScale\t\tTx\t\tTy\t\tTz\n%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", vecPara(0), vecPara(1), vecPara(2), vecPara(3), vecPara(4), vecPara(5), vecPara(6));
	delete pSrcDemHead, pRefDemHead,pSrcDemFile,pRefDemFile;
	return true;
}