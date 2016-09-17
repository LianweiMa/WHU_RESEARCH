#include <stdio.h>
#include <string.h>
#include "mlw_1_0_0/type_define.h"
#include "mlw_1_0_0/file_operation.hpp"
#include "mlw_1_0_0/SpDem.hpp"
#include "mlw_1_0_0/dem.hpp"
#include "gdal_1_10_1/gdal_priv.h"


int main(int argc, char**argv)
{
	printf("\n-----Start fusDem (Self-consistency)-----\n");
	//read dem file list
	char strDemList[MAX_FILE_NAME_LENGTH_MLW],*pS; int nDemNum = 0; strcpy(strDemList, argv[1]);
	FILE*fpDemListFile = fopen(strDemList, "r"); if (!fpDemListFile){ printf("ERROR_FILE_OPEN:%s\n", strDemList); return false; }
	fscanf(fpDemListFile, "%d\n", &nDemNum); if (nDemNum < 2){ printf("ERROR_FILE_NUMBER:%d\n", nDemNum); return false; }
	printf("The number of inputed dem file:	%d\n", nDemNum);
	char**pStrDemList = new char *[nDemNum];
	for (int DemNum_i = 0; DemNum_i < nDemNum;DemNum_i++)
	{
		pStrDemList[DemNum_i] = new char[MAX_FILE_NAME_LENGTH_MLW];
		fscanf(fpDemListFile, "%s\n", pStrDemList[DemNum_i]);
		if (!fFileIsExist(pStrDemList[DemNum_i])){ printf("ERROR_FILE_NO_EXIST:-%d-%s\n", DemNum_i, pStrDemList[DemNum_i]); return false; }
		printf("-%d-%s\n", DemNum_i, pStrDemList[DemNum_i]);
	}
	fclose(fpDemListFile);


	//read dem
	printf("\n*************************************\n");
	  printf("*************-1-Read dem*************\n");
	  printf("*************************************\n");
	//CSpDem *pSpDem = new CSpDem[nDemNum]; if (!pSpDem){ printf("ERROR_MEMORY_ALLOCATION:%s\n", "CSpDem *pSpDem = new CSpDem[nDemNum]"); return false; }
	CDem *pDemList = new CDem[nDemNum]; if (!pDemList){ printf("ERROR_MEMORY_ALLOCATION:%s\n", "CDem *pDemList = new CDem[nDemNum]"); return false; }
	double lfXmin = NOVALUE_MLW, lfXmax = NOVALUE_MLW, lfYmin = NOVALUE_MLW, lfYmax = NOVALUE_MLW;//peak value
	double lfGsdX = 0.0, lfGsdY = 0.0;//Resolution	
	for (int DemNum_i = 0; DemNum_i < nDemNum;DemNum_i++)
	{
		if (!pDemList[DemNum_i].Open(pStrDemList[DemNum_i])){ printf("ERROR_FILE_OPEN:%s\n", pStrDemList[DemNum_i]); return false; }
		printf("\n--Dem info -%d-%s--\n", DemNum_i, pStrDemList[DemNum_i]);
		double lfGsdX_tmp = pDemList[DemNum_i].GetDemHeader().intervalX;
		double lfGsdY_tmp = pDemList[DemNum_i].GetDemHeader().intervalY;
		printf("GSD_X:%lf\tGSD_Y:%lf\n", lfGsdX_tmp, lfGsdY_tmp);
		if (lfGsdX < lfGsdX_tmp)lfGsdX = lfGsdX_tmp;
		if (lfGsdY < lfGsdY_tmp)lfGsdY = lfGsdY_tmp;
		printf("Z_min:%f\tZ_max:%f\n", pDemList[DemNum_i].GetMinZ(), pDemList[DemNum_i].GetMaxZ());
		double lfX[4] = { NOVALUE_MLW }, lfY[4] = { NOVALUE_MLW };
		pDemList[DemNum_i].GetDemRegion(lfX, lfY);
		printf("--Georange--\n");
		printf("\tLB-[%lf,%lf]\n", lfX[0], lfY[0]);
		printf("\tRB-[%lf,%lf]\n", lfX[1], lfY[1]);
		printf("\tRT-[%lf,%lf]\n", lfX[2], lfY[2]);
		printf("\tLT-[%lf,%lf]\n", lfX[3], lfY[3]);
		if (lfXmin == NOVALUE_MLW)lfXmin = lfX[0];
		if (lfXmax == NOVALUE_MLW)lfXmax = lfX[1];
		if (lfYmin == NOVALUE_MLW)lfYmin = lfY[2];
		if (lfYmax == NOVALUE_MLW)lfYmax = lfY[1];
		for (int PeakValue_i = 0; PeakValue_i < 4;PeakValue_i++)
		{
			if (lfXmin > lfX[PeakValue_i])lfXmin = lfX[PeakValue_i];
			if (lfXmax < lfX[PeakValue_i])lfXmax = lfX[PeakValue_i];
			if (lfYmin > lfY[PeakValue_i])lfYmin = lfY[PeakValue_i];
			if (lfYmax < lfY[PeakValue_i])lfYmax = lfY[PeakValue_i];
		}
	}
	printf("--Georange of overlay--\n");
	printf("X_min:%lf\tX_max:%lf\n", lfXmin, lfXmax);
	printf("Y_min:%lf\tY_max:%lf\n", lfYmin, lfYmax);


	//calc statistic
	printf("\n*************************************\n");
	printf("*************-2-Calc std*************\n");
	printf("*************************************\n");
	//new file info
	char strDemDiff[MAX_FILE_NAME_LENGTH_MLW]; strcpy(strDemDiff, strDemList);
	pS = strrchr(strDemDiff, '\\'); if (!pS)pS = strrchr(strDemDiff, '/');
	mlw::Dos2Unix(pStrDemList[0]); mlw::Dos2Unix(pStrDemList[1]);
	sprintf(pS, "\\%s_%s%s", strrchr(pStrDemList[0],'/')+1, strrchr(pStrDemList[1],'/')+1, "_diff.tif"); 
	int iCols = 0, iRows = 0;
	iCols = int((lfXmax - lfXmin)/lfGsdX)+1;
	iRows = int((lfYmax - lfYmin)/lfGsdY)+1;
	//overlay xStart,yStart
	int iOverlayStartX = int((pDemList[0].GetDemHeader().startX - lfXmin) / pDemList[0].GetDemHeader().intervalX);
	int iOverlayStartY = int((pDemList[0].GetDemHeader().startY - lfYmin) / pDemList[0].GetDemHeader().intervalY);
	CDem demDiff;//diff dem
	DEMHDR DemHdr_Tmp;
	DemHdr_Tmp.startX = lfXmin; DemHdr_Tmp.startY = lfYmin;
	DemHdr_Tmp.intervalX = lfGsdX; DemHdr_Tmp.intervalY = lfGsdY;
	DemHdr_Tmp.column = iCols;   DemHdr_Tmp.row = iRows;
	DemHdr_Tmp.kapa = 0;
	if (!demDiff.Open(strDemDiff, CDem::modeWrite, &DemHdr_Tmp)){ printf("ERROR_FILE_OPEN:%s\n", strDemDiff); return false; }
	//block processing
	float fDiffStdDev = 0.0, fDiffSum = 0.0, fDiffMeanValue = 0.0;//statistic value
	int iDiffNum = 0;
	for (int i = 0; i < iRows; i+=BLOCK_HEIGHT_MLW)
	{	
		for (int j = 0; j < iCols; j+=BLOCK_WIDTH_MLW)
		{
			int Block_Height = 0, Block_Width = 0;
			if (i + BLOCK_HEIGHT_MLW <= iRows)
			{
				if (j + BLOCK_WIDTH_MLW <= iCols)
				{
					Block_Height = BLOCK_HEIGHT_MLW;
					Block_Width = BLOCK_WIDTH_MLW;
				}
				else
				{
					Block_Height = BLOCK_HEIGHT_MLW;
					Block_Width = (iCols - j);
				}
			}
			else
			{
				if (j + BLOCK_WIDTH_MLW <= iCols)
				{
					Block_Height = (iRows - i);
					Block_Width = BLOCK_WIDTH_MLW;
				}
				else
				{
					Block_Height = (iRows - i);
					Block_Width = (iCols - j);
				}
			}
			float *pfOriBlock_Z = new float[Block_Height*Block_Width];
			memset(pfOriBlock_Z, 0, sizeof(float)*Block_Height*Block_Width);
			float *pfDiff_Z = new float[Block_Height*Block_Width];
			memset(pfDiff_Z, 0, sizeof(float)*Block_Height*Block_Width);
			pDemList[0].ReadBlock(pfOriBlock_Z, iOverlayStartX + j, iOverlayStartY + i, Block_Width, Block_Height);
			double lfStartGeoX=0.0,lfStartGeoY=0.0,lfGeoX = 0.0, lfGeoY = 0.0;
			lfStartGeoX = (j + iOverlayStartX)*pDemList[0].GetDemHeader().intervalX + pDemList[0].GetDemHeader().startX;
			lfStartGeoY = -(i + iOverlayStartY)*pDemList[0].GetDemHeader().intervalY + pDemList[0].GetDemHeader().startY + (pDemList[0].GetDemHeader().row-1)*pDemList[0].GetDemHeader().intervalY;
			float fZ1=NOVALUE_MLW, fZ2=NOVALUE_MLW;
			for (int i_Block = 0; i_Block < Block_Height; i_Block++)
			{
				for (int j_Block = 0; j_Block < Block_Width; j_Block++)
				{
					lfGeoX = j_Block*pDemList[0].GetDemHeader().intervalX + lfStartGeoX;
					lfGeoY = -i_Block*pDemList[0].GetDemHeader().intervalY + lfStartGeoY;
					fZ1 = *(pfOriBlock_Z + i_Block*Block_Width + j_Block);
					fZ2 = pDemList[1].GetDemZValue(lfGeoX, lfGeoY);
					if (fZ1 != NOVALUE_MLW && fZ2 != NOVALUE_MLW){
						*(pfDiff_Z + i_Block*Block_Width + j_Block) = fZ1 - fZ2;
						fDiffSum += (fZ1 - fZ2);
						iDiffNum++;

					}else
						*(pfDiff_Z + i_Block*Block_Width + j_Block) = NOVALUE_MLW;
				}
			}
			demDiff.WriteBlock(pfDiff_Z, j, i, Block_Width, Block_Height);
			delete[]pfOriBlock_Z;
			delete[]pfDiff_Z;
		}
	}
	fDiffMeanValue = fDiffSum / iDiffNum;
	iDiffNum = 0;
	for (int i = 0; i < iRows; i += BLOCK_HEIGHT_MLW)
	{
		for (int j = 0; j < iCols; j += BLOCK_WIDTH_MLW)
		{
			int Block_Height = 0, Block_Width = 0;
			if (i + BLOCK_HEIGHT_MLW <= iRows)
			{
				if (j + BLOCK_WIDTH_MLW <= iCols)
				{
					Block_Height = BLOCK_HEIGHT_MLW;
					Block_Width = BLOCK_WIDTH_MLW;
				}
				else
				{
					Block_Height = BLOCK_HEIGHT_MLW;
					Block_Width = (iCols - j);
				}
			}
			else
			{
				if (j + BLOCK_WIDTH_MLW <= iCols)
				{
					Block_Height = (iRows - i);
					Block_Width = BLOCK_WIDTH_MLW;
				}
				else
				{
					Block_Height = (iRows - i);
					Block_Width = (iCols - j);
				}
			}
			float *pfOriBlock_Z = new float[Block_Height*Block_Width];
			memset(pfOriBlock_Z, 0, sizeof(float)*Block_Height*Block_Width);
			pDemList[0].ReadBlock(pfOriBlock_Z, j, i, Block_Width, Block_Height);
			float fZ_Tmp = NOVALUE_MLW;
			for (int i_Block = 0; i_Block < Block_Height; i_Block++)
			{
				for (int j_Block = 0; j_Block < Block_Width; j_Block++)
				{
					fZ_Tmp = demDiff.GetDemZValue(j+j_Block, i+i_Block);
					if (fZ_Tmp != NOVALUE_MLW){ fDiffStdDev += pow((fZ_Tmp - fDiffMeanValue), 2.0f); iDiffNum++; }
				}
			}
			delete[]pfOriBlock_Z;
		}
	}
	fDiffStdDev = pow(fDiffStdDev / iDiffNum, 0.5f);
	printf("--Statistic info of dem difference--\n");
	printf("Diff_Zmin:%f\tDiff_Zmax:%f\n", demDiff.GetMinZ(), demDiff.GetMaxZ());
	printf("Diff_Zmean:%f\n", fDiffMeanValue);
	printf("Diff_StdDev:%f\n", fDiffStdDev);

	//marked unreliable point
	printf("\n*************************************\n");
	printf("*************- 3-Marked *************\n");
	printf("*************************************\n");
	//new file info
	char strDemMarked[MAX_FILE_NAME_LENGTH_MLW]; strcpy(strDemMarked, strDemDiff);
	sprintf(strrchr(strDemMarked,'.'), "%s", "_Marked.tif");
	CDem demMarked;//marked dem
	if (!demMarked.Open(strDemMarked, CDem::modeWrite, &DemHdr_Tmp)){ printf("ERROR_FILE_OPEN:%s\n", strDemMarked); return false; }
	int iMarkedNums = 0;
	for (int i = 0; i < iRows; i += BLOCK_HEIGHT_MLW)
	{
		for (int j = 0; j < iCols; j += BLOCK_WIDTH_MLW)
		{
			int Block_Height = 0, Block_Width = 0;
			if (i + BLOCK_HEIGHT_MLW <= iRows)
			{
				if (j + BLOCK_WIDTH_MLW <= iCols)
				{
					Block_Height = BLOCK_HEIGHT_MLW;
					Block_Width = BLOCK_WIDTH_MLW;
				}
				else
				{
					Block_Height = BLOCK_HEIGHT_MLW;
					Block_Width = (iCols - j);
				}
			}
			else
			{
				if (j + BLOCK_WIDTH_MLW <= iCols)
				{
					Block_Height = (iRows - i);
					Block_Width = BLOCK_WIDTH_MLW;
				}
				else
				{
					Block_Height = (iRows - i);
					Block_Width = (iCols - j);
				}
			}
			float *pfDemDiffBlock_Z = new float[Block_Height*Block_Width];
			memset(pfDemDiffBlock_Z, 0, sizeof(float)*Block_Height*Block_Width);
			demDiff.ReadBlock(pfDemDiffBlock_Z, j, i, Block_Width, Block_Height);
			float *pfMarkedBlock_Z = new float[Block_Height*Block_Width];
			memset(pfMarkedBlock_Z, 0, sizeof(float)*Block_Height*Block_Width);
			float fZ_Tmp = NOVALUE_MLW;
			for (int i_Block = 0; i_Block < Block_Height; i_Block++)
			{
				for (int j_Block = 0; j_Block < Block_Width; j_Block++)
				{
					fZ_Tmp = *(pfDemDiffBlock_Z + i_Block*Block_Width + j_Block);
					if (fZ_Tmp!=NOVALUE_MLW && fZ_Tmp >(2 * fDiffStdDev))
					{
						*(pfMarkedBlock_Z + i_Block*Block_Width + j_Block) = MARK_VALUE_MLW;
						iMarkedNums++;
					}
				}
			}
			demMarked.WriteBlock(pfMarkedBlock_Z, j, i, Block_Width, Block_Height);
			delete[]pfDemDiffBlock_Z;
			delete[]pfMarkedBlock_Z;
		}
	}
	printf("Marked unreliabled point numbers:%d,percent:%.2f%%\n", iMarkedNums, (iMarkedNums*100.0f)/(iCols*iRows));
	
	//marked unreliable point
	printf("\n*************************************\n");
	printf("*************- 4- Fused *************\n");
	printf("*************************************\n");
	//new file info
	char strDemFused[MAX_FILE_NAME_LENGTH_MLW]; strcpy(strDemFused, strDemList);
	pS = strrchr(strDemFused, '\\'); if (!pS)pS = strrchr(strDemFused, '/');
	mlw::Dos2Unix(pStrDemList[0]); mlw::Dos2Unix(pStrDemList[1]);
	sprintf(pS, "\\%s_%s%s", strrchr(pStrDemList[0], '/') + 1, strrchr(pStrDemList[1], '/') + 1, "_Fused.tif");
	CDem demFused;//marked dem
	if (!demFused.Open(strDemFused, CDem::modeWrite, &DemHdr_Tmp)){ printf("ERROR_FILE_OPEN:%s\n", strDemFused); return false; }
	//1//write fused dem for reliable point//proceing no marked value
	for (int i = 0; i < iRows; i += BLOCK_HEIGHT_MLW)
	{
		for (int j = 0; j < iCols; j += BLOCK_WIDTH_MLW)
		{
			int Block_Height = 0, Block_Width = 0;
			if (i + BLOCK_HEIGHT_MLW <= iRows)
			{
				if (j + BLOCK_WIDTH_MLW <= iCols)
				{
					Block_Height = BLOCK_HEIGHT_MLW;
					Block_Width = BLOCK_WIDTH_MLW;
				}
				else
				{
					Block_Height = BLOCK_HEIGHT_MLW;
					Block_Width = (iCols - j);
				}
			}
			else
			{
				if (j + BLOCK_WIDTH_MLW <= iCols)
				{
					Block_Height = (iRows - i);
					Block_Width = BLOCK_WIDTH_MLW;
				}
				else
				{
					Block_Height = (iRows - i);
					Block_Width = (iCols - j);
				}
			}
			float *pfFusedBlock_Z = new float[Block_Height*Block_Width];
			memset(pfFusedBlock_Z, 0, sizeof(float)*Block_Height*Block_Width);
			float *pfMarkedBlock_Z = new float[Block_Height*Block_Width];
			memset(pfMarkedBlock_Z, 0, sizeof(float)*Block_Height*Block_Width);
			demMarked.ReadBlock(pfMarkedBlock_Z, j, i, Block_Width, Block_Height);
			float fZ_Tmp = NOVALUE_MLW,fNoMarked=0.0f;
			double lfFusedDemGeoX = 0.0, lfFusedDemGeoY = 0.0;
			double lfStartFusedDemGeoX = 0.0, lfStartFusedDemGeoY = 0.0;
			lfStartFusedDemGeoX = demFused.GetDemHeader().startX + demFused.GetDemHeader().intervalX*j;
			lfStartFusedDemGeoY = demFused.GetDemHeader().startY + demFused.GetDemHeader().intervalY*(demFused.GetDemHeader().row - 1) - demFused.GetDemHeader().intervalY*i;
			for (int i_Block = 0; i_Block < Block_Height; i_Block++)
			{
				for (int j_Block = 0; j_Block < Block_Width; j_Block++)
				{
					lfFusedDemGeoX = lfStartFusedDemGeoX + j_Block*demFused.GetDemHeader().intervalX;
					lfFusedDemGeoY = lfStartFusedDemGeoY - i_Block*demFused.GetDemHeader().intervalY;
					fZ_Tmp = *(pfMarkedBlock_Z + i_Block*Block_Width + j_Block);
					if (fZ_Tmp != MARK_VALUE_MLW){//no mark
						float fZ1 = pDemList[0].GetDemZValue(lfFusedDemGeoX, lfFusedDemGeoY);
						float fZ2 = pDemList[1].GetDemZValue(lfFusedDemGeoX, lfFusedDemGeoY);
						if (fZ1 != NOVALUE_MLW && fZ2!=NOVALUE_MLW)*(pfFusedBlock_Z + i_Block*Block_Width + j_Block) = (fZ1 + fZ2)/2;
						else { *(pfFusedBlock_Z + i_Block*Block_Width + j_Block) = NOVALUE_MLW;  }
					}
					else{ *(pfFusedBlock_Z + i_Block*Block_Width + j_Block) = MARK_VALUE_MLW; }
				}
			}
			demFused.WriteBlock(pfFusedBlock_Z, j, i, Block_Width, Block_Height);
			delete[]pfFusedBlock_Z;
			delete[]pfMarkedBlock_Z;
		}
	}
	//2//fill novalue for fused dem//processing marked value
	int iIterationNums = 1; 	
	int iUpdateMarkedNums = 0;
	while (iMarkedNums > 0 && iUpdateMarkedNums != iMarkedNums)
	{
		iUpdateMarkedNums = iMarkedNums;
		printf("--IterationNums:%d\t", iIterationNums++);
		for (int i = 0; i < iRows; i += BLOCK_HEIGHT_MLW)
		{
			for (int j = 0; j < iCols; j += BLOCK_WIDTH_MLW)
			{
				int Block_Height = 0, Block_Width = 0;
				if (i + BLOCK_HEIGHT_MLW <= iRows)
				{
					if (j + BLOCK_WIDTH_MLW <= iCols)
					{
						Block_Height = BLOCK_HEIGHT_MLW;
						Block_Width = BLOCK_WIDTH_MLW;
					}
					else
					{
						Block_Height = BLOCK_HEIGHT_MLW;
						Block_Width = (iCols - j);
					}
				}
				else
				{
					if (j + BLOCK_WIDTH_MLW <= iCols)
					{
						Block_Height = (iRows - i);
						Block_Width = BLOCK_WIDTH_MLW;
					}
					else
					{
						Block_Height = (iRows - i);
						Block_Width = (iCols - j);
					}
				}
				float *pfFilledBlock_Z = new float[Block_Height*Block_Width];
				memset(pfFilledBlock_Z, 0, sizeof(float)*Block_Height*Block_Width);
				demFused.ReadBlock(pfFilledBlock_Z, j, i, Block_Width, Block_Height);
				float fZ_Tmp = NOVALUE_MLW;
				double lfFusedDemGeoX = 0.0, lfFusedDemGeoY = 0.0;
				double lfStartFusedDemGeoX = 0.0, lfStartFusedDemGeoY = 0.0;
				lfStartFusedDemGeoX = demFused.GetDemHeader().startX + demFused.GetDemHeader().intervalX*j;
				lfStartFusedDemGeoY = demFused.GetDemHeader().startY + demFused.GetDemHeader().intervalY*(demFused.GetDemHeader().row - 1) - demFused.GetDemHeader().intervalY*i;
				for (int i_Block = 0; i_Block < Block_Height; i_Block++)
				{
					for (int j_Block = 0; j_Block < Block_Width; j_Block++)
					{
						lfFusedDemGeoX = lfStartFusedDemGeoX + j_Block*demFused.GetDemHeader().intervalX;
						lfFusedDemGeoY = lfStartFusedDemGeoY - i_Block*demFused.GetDemHeader().intervalY;
						fZ_Tmp = *(pfFilledBlock_Z + i_Block*Block_Width + j_Block);
						if (fZ_Tmp == MARK_VALUE_MLW){//mark//processing for this step
						//	//linear interpolated//near interpolated
							int iNeiborNums = 0; float fZ = 0.0f;
							float z1 = NOVALUE_MLW, z2 = NOVALUE_MLW, z3 = NOVALUE_MLW, z4 = NOVALUE_MLW,
								z5 = NOVALUE_MLW, z6 = NOVALUE_MLW, z7 = NOVALUE_MLW, z8 = NOVALUE_MLW;
							z1 = demFused.GetDemZValue(j_Block + j, i + i_Block-1); 
							z2 = demFused.GetDemZValue(j_Block + j+1, i + i_Block-1);
							z3 = demFused.GetDemZValue(j_Block + j+1, i + i_Block);
							z4 = demFused.GetDemZValue(j_Block + j+1, i + i_Block+1);
							z5 = demFused.GetDemZValue(j_Block + j, i + i_Block+1);
							z6 = demFused.GetDemZValue(j_Block + j-1, i + i_Block+1);
							z7 = demFused.GetDemZValue(j_Block + j-1, i + i_Block);
							z8 = demFused.GetDemZValue(j_Block + j-1, i + i_Block-1);
							if (z1 != NOVALUE_MLW&&z1 != MARK_VALUE_MLW){ iNeiborNums++; fZ += z1; }
							if (z2 != NOVALUE_MLW&&z2 != MARK_VALUE_MLW){ iNeiborNums++; fZ += z2; }
							if (z3 != NOVALUE_MLW&&z3 != MARK_VALUE_MLW){ iNeiborNums++; fZ += z3; }
							if (z4 != NOVALUE_MLW&&z4 != MARK_VALUE_MLW){ iNeiborNums++; fZ += z4; }
							if (z5 != NOVALUE_MLW&&z5 != MARK_VALUE_MLW){ iNeiborNums++; fZ += z5; }
							if (z6 != NOVALUE_MLW&&z6 != MARK_VALUE_MLW){ iNeiborNums++; fZ += z6; }
							if (z7 != NOVALUE_MLW&&z7 != MARK_VALUE_MLW){ iNeiborNums++; fZ += z7; }
							if (z8 != NOVALUE_MLW&&z8 != MARK_VALUE_MLW){ iNeiborNums++; fZ += z8; }
							if (iNeiborNums>=4){
								*(pfFilledBlock_Z + i_Block*Block_Width + j_Block) = fZ/iNeiborNums;
								iMarkedNums--;
							}
							//float z00 = NOVALUE_MLW, z01 = NOVALUE_MLW, z10 = NOVALUE_MLW, z11 = NOVALUE_MLW, fZ = NOVALUE_MLW;
							//if (z00 == NOVALUE_MLW || z01 == NOVALUE_MLW ||
							//	z10 == NOVALUE_MLW || z11 == NOVALUE_MLW || 
							//	z00 == MARK_VALUE_MLW || z01 == MARK_VALUE_MLW ||
							//	z10 == MARK_VALUE_MLW || z11 == MARK_VALUE_MLW){
							//	if (z00 != NOVALUE_MLW&&z00 != MARK_VALUE_MLW) fZ = z00;
							//	else if (z01 != NOVALUE_MLW&&z01 != MARK_VALUE_MLW) fZ = z01;
							//	else if (z10 != NOVALUE_MLW&&z10 != MARK_VALUE_MLW) fZ = z10;
							//	else if (z11 != NOVALUE_MLW&&z11 != MARK_VALUE_MLW) fZ = z11;
							//	else fZ = MARK_VALUE_MLW;
							//}
							//else fZ = (z00 + z10 + z01 + z11) / 4;


							////float fZ = pDemList[0].GetDemZValue(j_Block + j, i + i_Block);
							//if (fZ != MARK_VALUE_MLW){
							//	*(pfFilledBlock_Z + i_Block*Block_Width + j_Block) = fZ;
							//	iMarkedNums--;
							//}
						}
					}
				}
				demFused.WriteBlock(pfFilledBlock_Z, j, i, Block_Width, Block_Height);
				delete[]pfFilledBlock_Z;
			}
		}
		printf("MarkedNums:%d--\n", iMarkedNums);
	}
	/*if (iUpdateMarkedNums > 0)//this step
	{
		for (int i = 0; i < iRows; i += BLOCK_HEIGHT_MLW)
		{
			for (int j = 0; j < iCols; j += BLOCK_WIDTH_MLW)
			{
				int Block_Height = 0, Block_Width = 0;
				if (i + BLOCK_HEIGHT_MLW <= iRows)
				{
					if (j + BLOCK_WIDTH_MLW <= iCols)
					{
						Block_Height = BLOCK_HEIGHT_MLW;
						Block_Width = BLOCK_WIDTH_MLW;
					}
					else
					{
						Block_Height = BLOCK_HEIGHT_MLW;
						Block_Width = (iCols - j);
					}
				}
				else
				{
					if (j + BLOCK_WIDTH_MLW <= iCols)
					{
						Block_Height = (iRows - i);
						Block_Width = BLOCK_WIDTH_MLW;
					}
					else
					{
						Block_Height = (iRows - i);
						Block_Width = (iCols - j);
					}
				}
				float *pfFilledBlock_Z = new float[Block_Height*Block_Width];
				memset(pfFilledBlock_Z, 0, sizeof(float)*Block_Height*Block_Width);
				demFused.ReadBlock(pfFilledBlock_Z, j, i, Block_Width, Block_Height);
				float fZ_Tmp = NOVALUE_MLW;
				double lfFusedDemGeoX = 0.0, lfFusedDemGeoY = 0.0;
				double lfStartFusedDemGeoX = 0.0, lfStartFusedDemGeoY = 0.0;
				lfStartFusedDemGeoX = demFused.GetDemHeader().startX + demFused.GetDemHeader().intervalX*j;
				lfStartFusedDemGeoY = demFused.GetDemHeader().startY + demFused.GetDemHeader().intervalY*(demFused.GetDemHeader().row - 1) - demFused.GetDemHeader().intervalY*i;
				for (int i_Block = 0; i_Block < Block_Height; i_Block++)
				{
					for (int j_Block = 0; j_Block < Block_Width; j_Block++)
					{
						fZ_Tmp = *(pfFilledBlock_Z + i_Block*Block_Width + j_Block);
						if (fZ_Tmp == MARK_VALUE_MLW){//mark//processing for this step
							*(pfFilledBlock_Z + i_Block*Block_Width + j_Block) = 300.0f;
							}
					}
				}
				demFused.WriteBlock(pfFilledBlock_Z, j, i, Block_Width, Block_Height);
				delete[]pfFilledBlock_Z;
			}
		}
	}*/

	//3//update marked value from source dem
	iUpdateMarkedNums = 0;
	for (int i = 0; i < iRows; i += BLOCK_HEIGHT_MLW)
	{
		for (int j = 0; j < iCols; j += BLOCK_WIDTH_MLW)
		{
			int Block_Height = 0, Block_Width = 0;
			if (i + BLOCK_HEIGHT_MLW <= iRows)
			{
				if (j + BLOCK_WIDTH_MLW <= iCols)
				{
					Block_Height = BLOCK_HEIGHT_MLW;
					Block_Width = BLOCK_WIDTH_MLW;
				}
				else
				{
					Block_Height = BLOCK_HEIGHT_MLW;
					Block_Width = (iCols - j);
				}
			}
			else
			{
				if (j + BLOCK_WIDTH_MLW <= iCols)
				{
					Block_Height = (iRows - i);
					Block_Width = BLOCK_WIDTH_MLW;
				}
				else
				{
					Block_Height = (iRows - i);
					Block_Width = (iCols - j);
				}
			}
			float *pfFusedBlock_Z = new float[Block_Height*Block_Width];
			memset(pfFusedBlock_Z, 0, sizeof(float)*Block_Height*Block_Width);
			demFused.ReadBlock(pfFusedBlock_Z, j, i, Block_Width, Block_Height);
			float *pfMarkedBlock_Z = new float[Block_Height*Block_Width];
			memset(pfMarkedBlock_Z, 0, sizeof(float)*Block_Height*Block_Width);
			demMarked.ReadBlock(pfMarkedBlock_Z, j, i, Block_Width, Block_Height);
			float fZ_Tmp = NOVALUE_MLW, fNoMarked = 0.0f;
			double lfFusedDemGeoX = 0.0, lfFusedDemGeoY = 0.0;
			double lfStartFusedDemGeoX = 0.0, lfStartFusedDemGeoY = 0.0;
			lfStartFusedDemGeoX = demFused.GetDemHeader().startX + demFused.GetDemHeader().intervalX*j;
			lfStartFusedDemGeoY = demFused.GetDemHeader().startY + demFused.GetDemHeader().intervalY*(demFused.GetDemHeader().row - 1) - demFused.GetDemHeader().intervalY*i;
			for (int i_Block = 0; i_Block < Block_Height; i_Block++)
			{
				for (int j_Block = 0; j_Block < Block_Width; j_Block++)
				{
					lfFusedDemGeoX = lfStartFusedDemGeoX + j_Block*demFused.GetDemHeader().intervalX;
					lfFusedDemGeoY = lfStartFusedDemGeoY - i_Block*demFused.GetDemHeader().intervalY;
					fZ_Tmp = *(pfMarkedBlock_Z + i_Block*Block_Width + j_Block);
					if (fZ_Tmp == MARK_VALUE_MLW){//no mark
						float fZ = *(pfFusedBlock_Z + i_Block*Block_Width + j_Block);
						float fZ1 = pDemList[0].GetDemZValue(lfFusedDemGeoX, lfFusedDemGeoY);
						float fZ2 = pDemList[1].GetDemZValue(lfFusedDemGeoX, lfFusedDemGeoY);
						if (fZ != NOVALUE_MLW && fZ1 != NOVALUE_MLW && fZ2 != NOVALUE_MLW)
						{
							if (fabs(fZ - fZ1) < 2 * fDiffStdDev){ *(pfFusedBlock_Z + i_Block*Block_Width + j_Block) = fZ1; iUpdateMarkedNums++; }
							else if (fabs(fZ - fZ2)<2 * fDiffStdDev){ *(pfFusedBlock_Z + i_Block*Block_Width + j_Block) = fZ2; iUpdateMarkedNums++; }
						}										
					}
				}
			}
			demFused.WriteBlock(pfFusedBlock_Z, j, i, Block_Width, Block_Height);
			delete[]pfFusedBlock_Z;
			delete[]pfMarkedBlock_Z;
		}
	}
	printf("Update marked nums from source dem:%d--\n", iUpdateMarkedNums);
	demFused.FilterSmooth();
	printf("\n-----End fusDem (Self-consistency)-----\n");

	//delete memory
	delete[]pDemList;
	for (int DemNum_i = 0; DemNum_i < nDemNum; DemNum_i++)
	{
		delete []pStrDemList[DemNum_i];
	}
	delete[]pStrDemList;
	return true;
}