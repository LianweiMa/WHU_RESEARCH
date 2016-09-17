#ifndef DEM_DELETE_ERROR_HPP_WHU_MLW_20151106_12938948
#define DEM_DELETE_ERROR_HPP_WHU_MLW_20151106_12938948
#include <windows.h>
#include <math.h>
#define NOVALUE -99999.0f
typedef struct tagSLOPE
{
	float slopex, slopey;
}SLOPE;
typedef struct tagSLOPC
{
	float slopcx, slopcy;
}SLOPC;//C
typedef struct tagDSLOPC
{
	float dslopcx[2], dslopcy[2];
}DSLOPC;//D**C
bool DeleteErrorOfDem(float*pInputData,INT&iRows,INT&iCols,float&lfGsd, float*pOutputData){
	if (!pInputData || !pOutputData)return false;
	//calculate slope of input data
	SLOPE *pSlope = new SLOPE[iRows*iCols];
	memset(pSlope, 0, iRows*iCols*sizeof(float));
	for (int iStartRow = 0; iStartRow < iRows - 1; iStartRow++){
		for (int iStartCol = 0; iStartCol < iCols - 1; iStartCol++){	
			float z1 = *(pInputData + iStartRow*iCols + iStartCol);
			float z2 = *(pInputData + iStartRow*iCols + iStartCol + 1);
			float z3 = *(pInputData + (iStartRow + 1)*iCols + iStartCol);
			if (z1!=NOVALUE&&z2!=NOVALUE&&z3!=NOVALUE){
				(pSlope + iStartRow*iCols + iStartCol)->slopex = (z2 - z1) / lfGsd;
				(pSlope + iStartRow*iCols + iStartCol)->slopey = (z3 - z1) / lfGsd;
			}
			else{
				(pSlope + iStartRow*iCols + iStartCol)->slopex = (pSlope + iStartRow*iCols + iStartCol)->slopey = NOVALUE;
			}	
		}
	}
	//calculate change of slope
	SLOPC *pSlopc = new SLOPC[iRows*iCols];
	memset(pSlopc, 0, iRows*iCols*sizeof(float));
	for (int iStartRow = 1; iStartRow < iRows - 1; iStartRow++){
		for (int iStartCol = 1; iStartCol < iCols - 1; iStartCol++){
			SLOPE slope1 = *(pSlope + iStartRow*iCols + iStartCol);
			SLOPE slope2 = *(pSlope + iStartRow*iCols + iStartCol - 1);
			SLOPE slope3 = *(pSlope + (iStartRow - 1)*iCols + iStartCol);
			if (slope1.slopex!=NOVALUE&&slope2.slopex!=NOVALUE&&slope3.slopex!=NOVALUE){
				(pSlopc + iStartRow*iCols + iStartCol)->slopcx = slope1.slopex-slope2.slopex;
				(pSlopc + iStartRow*iCols + iStartCol)->slopcy = slope1.slopey - slope3.slopey;
			}
			else{
				(pSlopc + iStartRow*iCols + iStartCol)->slopcx = (pSlopc + iStartRow*iCols + iStartCol)->slopcy = NOVALUE;
			}
		}
	}
	//calculate difference of slope change
	int iCountx = 0, iCounty = 0;
	float fSumDSlopcx = 0.0f, fSumDSlopcy = 0;
	DSLOPC *pDSlopc = new DSLOPC[iRows*iCols];
	memset(pDSlopc, 0, iRows*iCols*sizeof(float));
	for (int iStartRow = 2; iStartRow < iRows - 2; iStartRow++){
		for (int iStartCol = 2; iStartCol < iCols - 2; iStartCol++){
			SLOPC slopc1 = *(pSlopc + iStartRow*iCols + iStartCol);
			SLOPC slopc2 = *(pSlopc + iStartRow*iCols + iStartCol-1);
			SLOPC slopc3 = *(pSlopc + iStartRow*iCols + iStartCol+1);
			SLOPC slopc4 = *(pSlopc + (iStartRow-1)*iCols + iStartCol);
			SLOPC slopc5 = *(pSlopc + (iStartRow+1)*iCols + iStartCol);
			if (slopc1.slopcx != NOVALUE&&slopc2.slopcx != NOVALUE&&slopc3.slopcx != NOVALUE&&slopc4.slopcx != NOVALUE&&slopc5.slopcx != NOVALUE){
				(pDSlopc + iStartRow*iCols + iStartCol)->dslopcx[0] = slopc1.slopcx - slopc2.slopcx;
				(pDSlopc + iStartRow*iCols + iStartCol)->dslopcx[1] = slopc1.slopcx - slopc3.slopcx;
				fSumDSlopcx += (pDSlopc + iStartRow*iCols + iStartCol)->dslopcx[0] + (pDSlopc + iStartRow*iCols + iStartCol)->dslopcx[1];
				iCountx += 2;
				(pDSlopc + iStartRow*iCols + iStartCol)->dslopcy[0] = slopc1.slopcy - slopc4.slopcy;
				(pDSlopc + iStartRow*iCols + iStartCol)->dslopcy[1] = slopc1.slopcy - slopc5.slopcy;
				fSumDSlopcy += (pDSlopc + iStartRow*iCols + iStartCol)->dslopcy[0] + (pDSlopc + iStartRow*iCols + iStartCol)->dslopcy[1];
				iCounty += 2;
			}
			else{
				(pDSlopc + iStartRow*iCols + iStartCol)->dslopcx[0] = 
					(pDSlopc + iStartRow*iCols + iStartCol)->dslopcx[1] = 
					(pDSlopc + iStartRow*iCols + iStartCol)->dslopcy[0] = 
					(pDSlopc + iStartRow*iCols + iStartCol)->dslopcy[1] = NOVALUE;
			}		
		}
	}
	//calculate RMSE of direction x and y
	float fAvgDSlopcx = 0.0f, fAvgDSlopcy = 0.0f,fRMSEx=0.0f,fRMSEy=0.0f,fSumDiffx=0.0f,fSumDiffy=0.0f;
	fAvgDSlopcx = fSumDSlopcx / iCountx;
	fAvgDSlopcy = fSumDSlopcy / iCounty;
	for (int iStartRow = 2; iStartRow < iRows - 2; iStartRow++){
		for (int iStartCol = 2; iStartCol < iCols - 2; iStartCol++){
			DSLOPC dslopc = *(pDSlopc + iStartRow*iCols + iStartCol);
			fSumDiffx += pow(dslopc.dslopcx[0] - fAvgDSlopcx, 2) + pow(dslopc.dslopcx[1] - fAvgDSlopcx, 2.0f);
			fSumDiffy += pow(dslopc.dslopcy[0] - fAvgDSlopcy, 2) + pow(dslopc.dslopcy[1] - fAvgDSlopcy, 2.0f);
		}
	}
	fRMSEx = pow(fSumDiffx / iCountx, 0.5f);
	fRMSEy = pow(fSumDiffy / iCounty, 0.5f);
	//delete error
	memcpy(pOutputData, pInputData, iRows*iCols*sizeof(float));
	float fThresholdx = 3 * fRMSEx;
	float fThresholdy = 3 * fRMSEy;
	for (int iStartRow = 2; iStartRow < iRows - 2; iStartRow++){
		for (int iStartCol = 2; iStartCol < iCols - 2; iStartCol++){
			DSLOPC dslopc = *(pDSlopc + iStartRow*iCols + iStartCol);
			if (dslopc.dslopcx[0]>fThresholdx&&dslopc.dslopcx[1]>fThresholdx&&dslopc.dslopcy[0]>fThresholdy&&dslopc.dslopcy[1]>fThresholdy){
				*(pOutputData + iStartRow*iCols + iStartCol) = NOVALUE;
			}
		}
	}
	//release memory
	delete[]pDSlopc;
	delete[]pSlopc;
	delete[]pSlope;
	return true;
}
#endif

