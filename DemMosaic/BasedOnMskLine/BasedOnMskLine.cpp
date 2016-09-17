#include <stdio.h>
#include <string.h>
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/CGdalDem.hpp"
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/StringOp.hpp"
#include "ResearchCode/CGdalShape.hpp"
struct SEARCH
{
	float fZ; int iPos;
	bool bIsDel;/*ֵΪtrue��ʾ����ɾ���ĵ�*/
	SEARCH(){
		bIsDel = false;
	}
};
int main(int argc, char**argv)
{
	if (false == NotePrint(argv, argc, 2)){ printf("Argument: Exe dem_file_list\n"); return false; }
	char strDemListFile[FILE_PN]; strcpy(strDemListFile, argv[1]);
	char *pS = NULL;
	//read dem list file
	FILE *fp = fopen(strDemListFile, "r"); if (!fp){ printf("ERROR_FILE_OPEN: %s\n", strDemListFile); return false; }
	int iDemFileNums = 0; fscanf(fp, "%d\n", &iDemFileNums);
	char **pDemfile = new char *[iDemFileNums];
	for (int i = 0; i < iDemFileNums; i++)
	{
		pDemfile[i] = new char[512];
		fscanf(fp, "%s\n", pDemfile[i]);
	}
	fclose(fp);

	//read dem//print dem info
	printf("\n-----Dem info-----\n");
	CGdalDem *pDemList = new CGdalDem[iDemFileNums]; if (!pDemList)return false;
	double lfXmin = NODATA, lfXmax = NODATA, lfYmin = NODATA, lfYmax = NODATA;
	double lfXgsd = 0.0, lfYgsd = 0.0;
	for (int i = 0; i < iDemFileNums; i++)
	{
		if (!pDemList[i].LoadFile(pDemfile[i]))
		{
			printf("Error_FileOpen:%s\n", pDemfile[i]);
			return false;
		}
		printf("--Dem %d--:%s\n", i + 1, pDemfile[i]);
		GDALDEMHDR pDemListHdr = pDemList[i].GetDemHeader();
		printf("Xgsd:%lf\tYgsd:%lf\n", pDemListHdr.lfGsdX, pDemListHdr.lfGsdY);
		printf("Colum:%d\tRow:%d\n", pDemListHdr.iCol, pDemListHdr.iRow);
		if (lfXgsd == 0.0)lfXgsd = pDemListHdr.lfGsdX;
		if (lfYgsd == 0.0)lfYgsd = pDemListHdr.lfGsdY;
		if (lfXgsd < pDemListHdr.lfGsdX)lfXgsd = pDemListHdr.lfGsdX;
		if (lfYgsd < pDemListHdr.lfGsdY)lfYgsd = pDemListHdr.lfGsdY;
		double lfZmin = NODATA, lfZmax = NODATA;
		lfZmax = pDemList[i].GetMaxZ(); lfZmin = pDemList[i].GetMinZ();
		printf("Zmin:%lf\tZmax:%lf\n", lfZmin, lfZmax);
		double lfX[4], lfY[4];
		pDemList[i].GetDemRegion(lfX, lfY);
		printf("GeoRange:\n\tLB-[%lf,%lf]\n\tRB-[%lf,%lf]\n\tRT-[%lf,%lf]\n\tLT-[%lf,%lf]\n", lfX[0], lfY[0], lfX[1], lfY[1], lfX[2], lfY[2], lfX[3], lfY[3]);
		if (lfXmin == NODATA) lfXmin = lfX[0];
		if (lfXmax == NODATA) lfXmax = lfX[0];
		if (lfYmin == NODATA) lfYmin = lfY[0];
		if (lfYmax == NODATA) lfYmax = lfY[0];
		for (int i = 0; i < 4; i++)
		{
			if (lfXmin > lfX[i]) lfXmin = lfX[i];
			if (lfXmax < lfX[i]) lfXmax = lfX[i];
			if (lfYmin > lfY[i]) lfYmin = lfY[i];
			if (lfYmax < lfY[i]) lfYmax = lfY[i];
		}
	}

	//Dem mosaicking
	printf("\n-----Start mosaic Dem-----\n");
	int iMosaicDemWidth = 0, iMosaicDemHeight = 0;
	iMosaicDemWidth = int((lfXmax - lfXmin) / lfXgsd) + 1;
	iMosaicDemHeight = int((lfYmax - lfYmin) / lfYgsd) + 1;
	printf("Mosaic dem: %d - %d\n", iMosaicDemWidth, iMosaicDemHeight);
	printf("X region: %lf - %lf\n", lfXmin, lfXmax);
	printf("Y region: %lf - %lf\n", lfYmin, lfYmax);

	CGdalDem *pMosaicDemFile = new CGdalDem; GDALDEMHDR *pMosaicDemHead = new GDALDEMHDR;
	pMosaicDemHead->lfStartX = lfXmin; pMosaicDemHead->lfStartY = lfYmin;
	pMosaicDemHead->lfGsdX = lfXgsd; pMosaicDemHead->lfGsdY = lfYgsd;
	pMosaicDemHead->iCol = iMosaicDemWidth;   pMosaicDemHead->iRow = iMosaicDemHeight;
	char strMosaicDemFile[FILE_PN]; strcpy(strMosaicDemFile, strDemListFile);
	pS = strrchr(strMosaicDemFile, '/'); if (!pS)pS = strrchr(strMosaicDemFile, '\\');
	sprintf(pS, "%s", "\\Dem_Mosaic_basedMskLine.tif");
	char strMarkLineFile[FILE_PN];

	if (true != pMosaicDemFile->CreatFile(strMosaicDemFile, pMosaicDemHead))return false;
	pMosaicDemFile->SetDemNoDataValue(NODATA);
	//////////////////////////////////////////////////////////////////////////
	for (int iPairIndex = 1; iPairIndex <= iDemFileNums - 1; iPairIndex++)
	{
		//�ɶԱ��
		CGdalDem*pFirstDem = pDemList + iPairIndex - 1;
		CGdalDem*pSecondDem = pDemList + iPairIndex;
		//�Ե�һƬDEM����Ϊʵ�壬�г���1��2ƬDEM���ص���
		//��һƬDEM��ֵ
		double lfXmaxFirst = pFirstDem->GetDemHeader().lfStartX + pFirstDem->GetDemHeader().lfGsdX*pFirstDem->GetDemHeader().iCol;
		double lfYmaxFirst = pFirstDem->GetDemHeader().lfStartY + pFirstDem->GetDemHeader().lfGsdY*pFirstDem->GetDemHeader().iRow;
		double lfXminFirst = pFirstDem->GetDemHeader().lfStartX;
		double lfYminFirst = pFirstDem->GetDemHeader().lfStartY;
		//�ڶ�ƬDEM��ֵ
		double lfXmaxSecond = pSecondDem->GetDemHeader().lfStartX + pSecondDem->GetDemHeader().lfGsdX*pSecondDem->GetDemHeader().iCol;
		double lfYmaxSecond = pSecondDem->GetDemHeader().lfStartY + pSecondDem->GetDemHeader().lfGsdY*pSecondDem->GetDemHeader().iRow;
		double lfXminSecond = pSecondDem->GetDemHeader().lfStartX;
		double lfYminSecond = pSecondDem->GetDemHeader().lfStartY;
		//�����ص���
		double lfOverlayXmin = 0.0, lfOverlayXmax = 0.0, lfOverlayYmin = 0.0, lfOverlayYmax = 0.0;
		if (lfXminFirst > lfXminSecond)lfOverlayXmin = lfXminFirst;
		else lfOverlayXmin = lfXminSecond;
		if (lfXmaxFirst > lfXmaxSecond)lfOverlayXmax = lfXmaxSecond;
		else lfOverlayXmax = lfXmaxFirst;
		if (lfYminFirst > lfYminSecond)lfOverlayYmin = lfYminFirst;
		else lfOverlayYmin = lfYminSecond;
		if (lfYmaxFirst > lfYmaxSecond) lfOverlayYmax = lfYmaxSecond;
		else lfOverlayYmax = lfYmaxFirst;
		if (lfOverlayXmax - lfOverlayXmin <= 0 || lfOverlayYmax - lfOverlayYmin <= 0){//no overlay: return
			printf("No overlay: %s & %s\n", pFirstDem->GetFileName(), pSecondDem->GetFileName());
			break;
		}
		//���ƴ����
		double lfOverlayGeo[6] = { lfOverlayXmin, pFirstDem->GetDemHeader().lfGsdX, 0.0, lfOverlayYmax, 0.0, -pFirstDem->GetDemHeader().lfGsdX };//�����ģ����Χ//Ҳ���ص�������Χ
		int iOverlayWidth = int((lfOverlayXmax - lfOverlayXmin) / lfOverlayGeo[1] + 0.5);
		int iOverlayHeight = int((lfOverlayYmin - lfOverlayYmax) / lfOverlayGeo[5] + 0.5);
		//�����ص�
		if (iOverlayHeight >= iOverlayWidth){
			//��ȡ�ص�������DEM�ϵ�����
			float*pDataOverlayL = new float[iOverlayWidth*iOverlayHeight]; Memset(pDataOverlayL, NODATA, iOverlayWidth*iOverlayHeight);
			int iOverlayStartXFirst = int((lfOverlayXmin - pFirstDem->GetDemHeader().lfStartX) / pFirstDem->GetDemHeader().lfGsdX + 0.5);
			int iOverlayStartYFirst = int(((pFirstDem->GetDemHeader().lfStartY + pFirstDem->GetDemHeader().lfGsdY*pFirstDem->GetDemHeader().iRow) - lfOverlayYmax) /
				pFirstDem->GetDemHeader().lfGsdY + 0.5);
			pFirstDem->ReadBlock(pDataOverlayL, iOverlayStartXFirst, iOverlayStartYFirst, iOverlayWidth, iOverlayHeight);
			//����ƬDEMΪģ�壬��ʼ���ƴ����
			BYTE *pMarkData = new BYTE[iOverlayWidth*iOverlayHeight];
			memset(pMarkData, 0, iOverlayWidth*iOverlayHeight*sizeof(BYTE));
			int iSeedPos = iOverlayWidth / 2;//��ʼ����һ�е����ӵ�
			*(pMarkData + iSeedPos) = 255;//��ǵ�һ��//���ֵĬ��Ϊ255
			SEARCH fSearch[3], fSearchTmp;
			for (int i = 0; i < iOverlayHeight - 1; i++)
			{
				//����Ѱ��ǰ��ǵ㴦���¡����¡�����3����
				fSearch[0].iPos = iSeedPos + iOverlayWidth; fSearch[0].fZ = fabs(*(pDataOverlayL + iSeedPos) - *(pDataOverlayL + iSeedPos + iOverlayWidth)); //��
				if (iSeedPos % iOverlayWidth == 0 /*��߽�����*/)fSearch[1] = fSearch[0];
				else{
					fSearch[1].iPos = iSeedPos - 1 + iOverlayWidth; fSearch[1].fZ = fabs(*(pDataOverlayL + iSeedPos) - *(pDataOverlayL + iSeedPos - 1 + iOverlayWidth)); //����
				}
				if ((iSeedPos + 1) % iOverlayWidth == 0 /*�ұ߽�����*/)fSearch[2] = fSearch[0];
				else{
					fSearch[2].iPos = iSeedPos + 1 + iOverlayWidth; fSearch[2].fZ = fabs(*(pDataOverlayL + iSeedPos) - *(pDataOverlayL + iSeedPos + 1 + iOverlayWidth)); //����
				}
				//�Ƚ�3���¶ȣ��Բ��ֵ���棩������¶ȱ仯ƽ����λ��
				fSearchTmp = fSearch[0]; int iSearchIndex = 0;
				for (int k = 1; k < 3; k++){
					if (fSearchTmp.fZ > fSearch[k].fZ){ fSearchTmp = fSearch[k]; iSearchIndex = k; }//ͨ��ȡ��3�����в����С��λ��
				}
				//���ձ�ǣ������µ�ǰ���λ�ñ���iSeedPos
				*(pMarkData + fSearchTmp.iPos) = 255; iSeedPos = fSearchTmp.iPos;
			}
			//���tif��ʽ�ı����ģͼ��
			strcpy(strMarkLineFile, strMosaicDemFile); Dos2Unix(strMarkLineFile);
			pS = strrchr(strMarkLineFile, '/');
			char strFirstDemName[CHAR_LEN];strcpy(strFirstDemName, GetFileName(pDemfile[iPairIndex - 1]));
			char strSecondDemName[CHAR_LEN];strcpy(strSecondDemName, GetFileName(pDemfile[iPairIndex]));
			sprintf(pS + 1, "%s-%s%s", strFirstDemName, strSecondDemName, "_mark_3-neibor.shp");
			WriteShpFile(pMarkData, iOverlayWidth, iOverlayHeight, lfOverlayGeo, pFirstDem->GetDemProj(), strMarkLineFile);

			//����ǶDEM��д������
			float *pMosaicDemData = new float[iMosaicDemHeight*iMosaicDemWidth];
			Memset(pMosaicDemData, NODATA, iMosaicDemHeight*iMosaicDemWidth);
			float pFirstZ = 0.0f, pSecondZ = 0.0f;
			double lfX = pMosaicDemHead->lfStartX, lfY = pMosaicDemHead->lfStartY + pMosaicDemHead->lfGsdY*pMosaicDemHead->iRow;
			for (int i = 0; i < iMosaicDemHeight; i++)
			{
				lfX = pMosaicDemHead->lfStartX;
				for (int j = 0; j < iMosaicDemWidth; j++)
				{
					pFirstZ = pFirstDem->GetDemZValue(lfX, lfY);
					pSecondZ = pSecondDem->GetDemZValue(lfX, lfY);
					if (NODATA != pFirstZ && NODATA == pSecondZ)
					{ 
						*(pMosaicDemData + i*iMosaicDemWidth + j) = pFirstZ;
						lfX += pMosaicDemHead->lfGsdX;
						continue;
					}
					if (NODATA == pFirstZ && NODATA != pSecondZ)
					{
						*(pMosaicDemData + i*iMosaicDemWidth + j) = pSecondZ;
						lfX += pMosaicDemHead->lfGsdX;
						continue;
					}
					if (NODATA != pFirstZ && NODATA != pSecondZ)
					{
						int iMarkedColID = int((lfX - lfOverlayXmin) / pMosaicDemHead->lfGsdX + 0.5);
						int iMarkedRowID = int((lfOverlayYmax - lfY) / pMosaicDemHead->lfGsdY + 0.5);
						for (int k = 0; k < iOverlayWidth; k++)
						{
							if (255 == *(pMarkData + iMarkedRowID*iOverlayWidth + k))
							{
								if (iMarkedColID == k){ *(pMosaicDemData + i*iMosaicDemWidth + j) = (pFirstZ + pSecondZ) / 2; break; }
								if (iMarkedColID < k)//��Ƕ�����
								{ 
									if (pFirstDem->GetDemHeader().lfStartX<=pSecondDem->GetDemHeader().lfStartX)
										*(pMosaicDemData + i*iMosaicDemWidth + j) = pFirstZ; 
									else
										*(pMosaicDemData + i*iMosaicDemWidth + j) = pSecondZ;
									break; 
								}
								if (iMarkedColID > k)//��Ƕ���Ҳ�
								{ 
									if (pFirstDem->GetDemHeader().lfStartX >= pSecondDem->GetDemHeader().lfStartX)
										*(pMosaicDemData + i*iMosaicDemWidth + j) = pFirstZ;
									else
										*(pMosaicDemData + i*iMosaicDemWidth + j) = pSecondZ;
									break;
								}
							}
						}
						lfX += pMosaicDemHead->lfGsdX;
						continue;
					}
					lfX += pMosaicDemHead->lfGsdX;
				}
				lfY -= pMosaicDemHead->lfGsdY;
			}
			//��Ƕ����
			for (int i = 0; i < iOverlayHeight; i++)
			{
				for (int j = 0; j < iOverlayWidth; j++)
				{
					if (255 == *(pMarkData + i*iOverlayWidth + j))
					{
						double lfXTemp = 0.0, lfYTemp = 0.0;//�ڱ��������Ѱ�ҵ���Ƕ��λ��
						lfXTemp = lfOverlayXmin + j*pMosaicDemHead->lfGsdX;
						lfYTemp = lfOverlayYmax - i*pMosaicDemHead->lfGsdY;
						int iColID = 0, iRowID = 0;//������Ƕ������ǶDEM�е�λ��
						iColID = int((lfXTemp - pMosaicDemHead->lfStartX) / pMosaicDemHead->lfGsdX + 0.5);
						iRowID = int((pMosaicDemHead->lfStartY + pMosaicDemHead->lfGsdY*pMosaicDemHead->iRow - lfYTemp) / pMosaicDemHead->lfGsdY + 0.5);
						if (iColID - 2 >= 0 && iRowID - 2 >= 0 && iColID + 2 < pMosaicDemHead->iCol && iRowID + 2 < pMosaicDemHead->iRow)
						{
							float*pZ, grid[25] = {NODATA};
							pZ = pMosaicDemData + (iRowID - 2)*pMosaicDemHead->iCol + iColID - 2;
							grid[0] = *pZ++; grid[1] = *pZ++; grid[2] = *pZ++; grid[3] = *pZ++; grid[4] = *pZ++;
							pZ = pMosaicDemData + (iRowID - 1)*pMosaicDemHead->iCol + iColID - 2;
							grid[5] = *pZ++; grid[6] = *pZ++; grid[7] = *pZ++; grid[8] = *pZ++; grid[9] = *pZ++;
							pZ = pMosaicDemData + iRowID*pMosaicDemHead->iCol + iColID - 2;
							grid[10] = *pZ++; grid[11] = *pZ++; grid[12] = *pZ++; grid[13] = *pZ++; grid[14] = *pZ++;
							pZ = pMosaicDemData + (iRowID + 1)*pMosaicDemHead->iCol + iColID - 2;
							grid[15] = *pZ++; grid[16] = *pZ++; grid[17] = *pZ++;grid[18] = *pZ++; grid[19] = *pZ++;
							pZ = pMosaicDemData + (iRowID + 2)*pMosaicDemHead->iCol + iColID - 2;
							grid[20] = *pZ++; grid[21] = *pZ++; grid[22] = *pZ++; grid[23] = *pZ++; grid[24] = *pZ++;
							SORTF(grid, 25); int k = 0; for (k = 0; k < 25; k++){ if (grid[k] == NODATA) break; }k -= 1;
							if (k>=14)
								*(pMosaicDemData + iRowID*pMosaicDemHead->iCol + iColID) = *(grid + k / 2);
						}
						break;
					}
				}
			}
			pMosaicDemFile->WriteBlock(pMosaicDemData, 0, 0, pMosaicDemHead->iCol, pMosaicDemHead->iRow);
			delete[]pDataOverlayL;
			delete[]pMarkData;
			delete[]pMosaicDemData;
		}
		else
		{//�����ص�
			float*pDataOverlayT = new float[iOverlayWidth*iOverlayHeight]; Memset(pDataOverlayT, NODATA, iOverlayWidth*iOverlayHeight);
			int iOverlayStartXFirst = int((lfOverlayXmin - pFirstDem->GetDemHeader().lfStartX) / pFirstDem->GetDemHeader().lfGsdX + 0.5);
			int iOverlayStartYFirst = int((pFirstDem->GetDemHeader().lfStartY + pFirstDem->GetDemHeader().iRow*pFirstDem->GetDemHeader().lfGsdY-lfOverlayYmax) 
				/ pFirstDem->GetDemHeader().lfGsdY + 0.5);
			pFirstDem->ReadBlock(pDataOverlayT, iOverlayStartXFirst, iOverlayStartYFirst, iOverlayWidth, iOverlayHeight);
			//����ƬDEMΪģ�壬��ʼ���ƴ����
			BYTE *pMarkData = new BYTE[iOverlayWidth*iOverlayHeight];
			memset(pMarkData, 0, iOverlayWidth*iOverlayHeight*sizeof(BYTE));
			int iSeedPos = iOverlayWidth*(iOverlayHeight / 2);//��ʼ����һ�е����ӵ�
			*(pMarkData + iSeedPos) = 255;//��ǵ�һ��//���ֵĬ��Ϊ255
			SEARCH fSearch[3], fSearchTmp;
			for (int j = 0; j < iOverlayWidth - 1; j++)
			{
				//printf("j = %d\n", j);
				//3.1.2.1//����Ѱ��ǰ��ǵ㴦���ҡ����¡�����3����
				fSearch[0].iPos = iSeedPos + 1; fSearch[0].fZ = fabs(*(pDataOverlayT + iSeedPos) - *(pDataOverlayT + fSearch[0].iPos)); //��
				if (iSeedPos >= 0 && iSeedPos < iOverlayWidth /*�ϱ߽�����*/)fSearch[1] = fSearch[0];
				else{
					fSearch[1].iPos = iSeedPos + 1 - iOverlayWidth; fSearch[1].fZ = fabs(*(pDataOverlayT + iSeedPos) - *(pDataOverlayT + fSearch[1].iPos)); //����
				}
				if ((iSeedPos) >= iOverlayWidth*(iOverlayHeight - 2) && (iSeedPos) < iOverlayWidth*(iOverlayHeight - 1) /*�±߽�����*/)fSearch[2] = fSearch[0];
				else{
					fSearch[2].iPos = iSeedPos + 1 + iOverlayWidth; fSearch[2].fZ = fabs(*(pDataOverlayT + iSeedPos) - *(pDataOverlayT + fSearch[2].iPos)); //����
				}
				//3.1.2.2//�Ƚ�3���¶ȣ��Բ��ֵ���棩������¶ȱ仯ƽ����λ��
				fSearchTmp = fSearch[0]; int iSearchIndex = 0;
				for (int k = 1; k < 3; k++){
					if (fSearchTmp.fZ > fSearch[k].fZ){ fSearchTmp = fSearch[k]; iSearchIndex = k; }//ͨ��ȡ��3�����в����С��λ��
				}
				//3.1.2.3//���ձ�ǣ������µ�ǰ���λ�ñ���iSeedPos
				*(pMarkData + fSearchTmp.iPos) = 255; iSeedPos = fSearchTmp.iPos;
			}
			//���tif��ʽ�ı����ģͼ��
			strcpy(strMarkLineFile, strMosaicDemFile); Dos2Unix(strMarkLineFile);
			pS = strrchr(strMarkLineFile, '/');
			char strFirstDemName[CHAR_LEN]; strcpy(strFirstDemName, GetFileName(pDemfile[iPairIndex - 1]));
			char strSecondDemName[CHAR_LEN]; strcpy(strSecondDemName, GetFileName(pDemfile[iPairIndex]));
			sprintf(pS + 1, "%s-%s%s", strFirstDemName, strSecondDemName, "_mark_3-neibor.shp");
			WriteShpFile(pMarkData, iOverlayWidth, iOverlayHeight, lfOverlayGeo, pFirstDem->GetDemProj(), strMarkLineFile);

			//����ǶDEM��д������
			float *pMosaicDemData = new float[iMosaicDemHeight*iMosaicDemWidth];
			Memset(pMosaicDemData, NODATA, iMosaicDemHeight*iMosaicDemWidth);
			float pTopZ = 0.0f, pBottomZ = 0.0f;
			double lfX = pMosaicDemHead->lfStartX, lfY = pMosaicDemHead->lfStartY + pMosaicDemHead->lfGsdY*pMosaicDemHead->iRow;
			for (int i = 0; i < iMosaicDemHeight; i++)
			{
				lfX = pMosaicDemHead->lfStartX;
				for (int j = 0; j < iMosaicDemWidth; j++)
				{
					pTopZ = pFirstDem->GetDemZValue(lfX, lfY);
					pBottomZ = pSecondDem->GetDemZValue(lfX, lfY);
					if (NODATA != pTopZ && NODATA == pBottomZ)
					{
						*(pMosaicDemData + i*iMosaicDemWidth + j) = pTopZ;
						lfX += pMosaicDemHead->lfGsdX;
						continue;
					}
					if (NODATA == pTopZ && NODATA != pBottomZ)
					{
						*(pMosaicDemData + i*iMosaicDemWidth + j) = pBottomZ;
						lfX += pMosaicDemHead->lfGsdX;
						continue;
					}
					if (NODATA != pTopZ && NODATA != pBottomZ)
					{
						int iMarkedColID = int((lfX - lfOverlayXmin) / pMosaicDemHead->lfGsdX + 0.5);
						int iMarkedRowID = int((lfOverlayYmax - lfY) / pMosaicDemHead->lfGsdY + 0.5);
						for (int k = 0; k < iOverlayHeight; k++)//��Ѱ��
						{
							if (255 == *(pMarkData + k*iOverlayWidth + iMarkedColID))
							{
								if (iMarkedRowID == k){ *(pMosaicDemData + i*iMosaicDemWidth + j) = (pTopZ + pBottomZ) / 2; break; }
								if (iMarkedRowID < k){ *(pMosaicDemData + i*iMosaicDemWidth + j) = pTopZ; break; }
								if (iMarkedRowID > k){ *(pMosaicDemData + i*iMosaicDemWidth + j) = pBottomZ; break; }
							}
						}
						lfX += pMosaicDemHead->lfGsdX;
						continue;
					}
					lfX += pMosaicDemHead->lfGsdX;
				}
				lfY -= pMosaicDemHead->lfGsdY;
			}
			//��Ƕ����
			for (int i = 0; i < iOverlayHeight; i++)
			{
				for (int j = 0; j < iOverlayWidth; j++)
				{
					if (255 == *(pMarkData + i*iOverlayWidth + j))
					{
						double lfXTemp = 0.0, lfYTemp = 0.0;//�ڱ��������Ѱ�ҵ���Ƕ��λ��
						lfXTemp = lfOverlayXmin + j*pMosaicDemHead->lfGsdX;
						lfYTemp = lfOverlayYmax - i*pMosaicDemHead->lfGsdY;
						int iColID = 0, iRowID = 0;//������Ƕ������ǶDEM�е�λ��
						iColID = int((lfXTemp - pMosaicDemHead->lfStartX) / pMosaicDemHead->lfGsdX + 0.5);
						iRowID = int((pMosaicDemHead->lfStartY + pMosaicDemHead->lfGsdY*pMosaicDemHead->iRow - lfYTemp) / pMosaicDemHead->lfGsdY + 0.5);
						if (iColID - 1 >= 0 && iRowID - 1 >= 0 && iColID + 1 < pMosaicDemHead->iCol && iRowID + 1 < pMosaicDemHead->iRow)
						{
							float*pZ, grid[9] = { NODATA };
							pZ = pMosaicDemData + (iRowID - 1)*pMosaicDemHead->iCol + iColID - 1;
							grid[0] = *pZ++; grid[1] = *pZ++; grid[2] = *pZ++;
							pZ = pMosaicDemData + iRowID*pMosaicDemHead->iCol + iColID - 1;
							grid[3] = *pZ++; grid[4] = *pZ++; grid[5] = *pZ++;
							pZ = pMosaicDemData + (iRowID + 1)*pMosaicDemHead->iCol + iColID - 1;
							grid[6] = *pZ++; grid[7] = *pZ++; grid[8] = *pZ++;
							SORTF(grid, 9); int k = 0; for (k = 0; k < 9; k++){ if (grid[k] == NODATA) break; }k -= 1;
							if (k >= 4)
								*(pMosaicDemData + iRowID*pMosaicDemHead->iCol + iColID) = *(grid + k / 2);
						}
						break;
					}
				}
			}
			pMosaicDemFile->WriteBlock(pMosaicDemData, 0, 0, pMosaicDemHead->iCol, pMosaicDemHead->iRow);
			delete[]pDataOverlayT;
			delete[]pMarkData;
			delete[]pMosaicDemData;
		}
	}

	printf("Saved mosaic Dem file: %s\n", strMosaicDemFile);
	printf("Saved mosaic line file: %s\n", strMarkLineFile);
	return true;
}