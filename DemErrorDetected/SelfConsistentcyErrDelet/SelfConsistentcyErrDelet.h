#ifndef DEM_DELETE_ERROR_HPP_WHU_MLW_20151106_12938948
#define DEM_DELETE_ERROR_HPP_WHU_MLW_20151106_12938948
#include <math.h>
#define NOVALUE -32767.0f
typedef struct tagDSLOPC
{
	double dslopcx[2], dslopcy[2];
}DSLOPC;//D**C
bool DeleteErrorOfDem(float*pInputData, int&iRows, int&iCols, double lfGsd, float*pOutputData){
	if (!pInputData || !pOutputData)return false;

	int iCountx = 0, iCounty = 0;
	double lfSumDSlopcx = 0.0, lfSumDSlopcy = 0.0;
	DSLOPC *pDSlopc = new DSLOPC[iRows*iCols];
	memset(pDSlopc, 0, iRows*iCols*sizeof(DSLOPC));
	for (int iStartRow = 2; iStartRow < iRows - 2; iStartRow++){
		for (int iStartCol = 2; iStartCol < iCols - 2; iStartCol++){
			//calculate slope of input data
			double slopex[3][2], slopey[2][3]; float grid[3][3];
			grid[0][0] = *(pInputData + (iStartRow - 1)*iCols + iStartCol - 1);
			grid[0][1] = *(pInputData + (iStartRow - 1)*iCols + iStartCol);
			grid[0][2] = *(pInputData + (iStartRow - 1)*iCols + iStartCol + 1);
			grid[1][0] = *(pInputData + (iStartRow)*iCols + iStartCol - 1);
			grid[1][1] = *(pInputData + (iStartRow)*iCols + iStartCol);
			grid[1][2] = *(pInputData + (iStartRow)*iCols + iStartCol + 1);
			grid[2][0] = *(pInputData + (iStartRow + 1)*iCols + iStartCol - 1);
			grid[2][1] = *(pInputData + (iStartRow + 1)*iCols + iStartCol);
			grid[2][2] = *(pInputData + (iStartRow + 1)*iCols + iStartCol + 1);

			if (grid[0][0] != NOVALUE&&grid[0][1] != NOVALUE&&grid[0][2] != NOVALUE&&
				grid[1][0] != NOVALUE&&grid[1][1] != NOVALUE&&grid[1][2] != NOVALUE&&
				grid[2][0] != NOVALUE&&grid[2][1] != NOVALUE&&grid[2][2] != NOVALUE)
			{
				slopex[0][0] = (grid[0][0] - grid[0][1]) / lfGsd;
				slopex[0][1] = (grid[0][1] - grid[0][2]) / lfGsd;
				slopex[1][0] = (grid[1][0] - grid[1][1]) / lfGsd;
				slopex[1][1] = (grid[1][1] - grid[1][2]) / lfGsd;
				slopex[2][0] = (grid[2][0] - grid[2][1]) / lfGsd;
				slopex[2][1] = (grid[2][1] - grid[2][2]) / lfGsd;

				slopey[0][0] = (grid[0][0] - grid[1][0]) / lfGsd;
				slopey[0][1] = (grid[0][1] - grid[1][1]) / lfGsd;
				slopey[0][2] = (grid[0][2] - grid[1][2]) / lfGsd;
				slopey[1][0] = (grid[1][0] - grid[2][0]) / lfGsd;
				slopey[1][1] = (grid[1][1] - grid[2][1]) / lfGsd;
				slopey[1][2] = (grid[1][2] - grid[2][2]) / lfGsd;

				//calculate change of slope
				double slopecx[3], slopecy[3];
				slopecx[0] = slopex[0][0] - slopex[0][1];
				slopecx[1] = slopex[1][0] - slopex[1][1];
				slopecx[2] = slopex[2][0] - slopex[2][1];

				slopecy[0] = slopey[0][0] - slopey[1][0];
				slopecy[1] = slopey[0][1] - slopey[1][1];
				slopecy[2] = slopey[0][2] - slopey[1][2];

				//calculate difference of slope change
				double dslopecx[2], dslopecy[2];
				dslopecx[0] = slopecy[0] - slopecy[1];
				dslopecx[1] = slopecy[1] - slopecy[2];
				(pDSlopc + iStartRow*iCols + iStartCol)->dslopcx[0] = dslopecx[0];
				(pDSlopc + iStartRow*iCols + iStartCol)->dslopcx[1] = dslopecx[1];
				lfSumDSlopcx += dslopecx[0]; //iCountx++;
				lfSumDSlopcx += dslopecx[1]; iCountx++;

				dslopecy[0] = slopecx[0] - slopecx[1];
				dslopecy[1] = slopecx[1] - slopecx[2];
				(pDSlopc + iStartRow*iCols + iStartCol)->dslopcy[0] = dslopecy[0];
				(pDSlopc + iStartRow*iCols + iStartCol)->dslopcy[1] = dslopecy[1];
				lfSumDSlopcy += dslopecy[0]; //iCounty++;
				lfSumDSlopcy += dslopecy[1]; iCounty++;
			}
			else
			{
				(pDSlopc + iStartRow*iCols + iStartCol)->dslopcx[0] = NOVALUE;
				(pDSlopc + iStartRow*iCols + iStartCol)->dslopcx[1] = NOVALUE;
				(pDSlopc + iStartRow*iCols + iStartCol)->dslopcy[0] = NOVALUE;
				(pDSlopc + iStartRow*iCols + iStartCol)->dslopcy[1] = NOVALUE;

			}
		}
	}
	//calculate RMSE of direction x and y
	double lfAvgDSlopcx = 0.0, lfAvgDSlopcy = 0.0;
	lfAvgDSlopcx = lfSumDSlopcx / iCountx;
	lfAvgDSlopcy = lfSumDSlopcy / iCounty;
	double lfSumDiffx = 0.0, lfSumDiffy = 0.0;
	for (int iStartRow = 2; iStartRow < iRows - 2; iStartRow++){
		for (int iStartCol = 2; iStartCol < iCols - 2; iStartCol++){
			DSLOPC dslopc = *(pDSlopc + iStartRow*iCols + iStartCol);
			if (dslopc.dslopcx[0] != NOVALUE&&dslopc.dslopcx[1] != NOVALUE&&dslopc.dslopcy[0] != NOVALUE&&dslopc.dslopcy[1] != NOVALUE)
			{
				lfSumDiffx += pow(dslopc.dslopcx[0] + dslopc.dslopcx[1] - lfAvgDSlopcx, 2.0);
				lfSumDiffy += pow(dslopc.dslopcy[0] + dslopc.dslopcy[1] - lfAvgDSlopcy, 2.0);
			}
		}
	}
	double lfRMSEx = 0.0, lfRMSEy = 0.0;
	lfRMSEx = pow(lfSumDiffx / iCountx, 0.5f);
	lfRMSEy = pow(lfSumDiffy / iCounty, 0.5f);
	//delete error
	memcpy(pOutputData, pInputData, iRows*iCols*sizeof(float));
	double lfThresholdx = 3 * lfRMSEx;
	double lfThresholdy = 3 * lfRMSEy;
	int iDeletedNums = 0;
	for (int iStartRow = 2; iStartRow < iRows - 2; iStartRow++){
		for (int iStartCol = 2; iStartCol < iCols - 2; iStartCol++){
			DSLOPC dslopc = *(pDSlopc + iStartRow*iCols + iStartCol);
			if (dslopc.dslopcx[0] != NOVALUE&&dslopc.dslopcx[1] != NOVALUE&&dslopc.dslopcy[0] != NOVALUE&&dslopc.dslopcy[1] != NOVALUE)
			{
				if (dslopc.dslopcx[0] > lfThresholdx && dslopc.dslopcx[1] > lfThresholdx && dslopc.dslopcy[0] > lfThresholdy && dslopc.dslopcy[1] > lfThresholdy){
					*(pOutputData + iStartRow*iCols + iStartCol) = NOVALUE;
					iDeletedNums++;
				}
			}
		}
	}
	printf("Delete point: %d%%\n", iDeletedNums / (iCountx / 2));
	//release memory
	delete[]pDSlopc;
	return true;
}
#endif