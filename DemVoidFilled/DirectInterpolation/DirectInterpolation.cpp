#include <stdio.h>
#include <vector>
#include "ResearchCode/TypeDef.h"
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/CGdalDem.hpp"
using namespace std;
#define RANGE_NUM 5
#define PIXEL_NUM 10
typedef struct tagVOIDSET
{
	int iRowID, iColID;
}VOIDSET,EDGESET;
/*true: dem edge*/
bool IsDemEdge(int &iRowID_i, int &iColID_i,int &iRows_i,int &iCols_i, float *pZ_i){
	int iCount = 0;
	//Top
	for (int i = iRowID_i; i >= 0; i--)
	{
		float fZTemp = *(pZ_i + i*iCols_i + iColID_i);
		if (NODATA != fZTemp){break;}
		iCount++;
	}
	if (iCount == iRowID_i + 1)return true;
	//Bottom
	iCount = 0;
	for (int i = iRowID_i; i < iRows_i; i++)
	{
		float fZTemp = *(pZ_i + i*iCols_i + iColID_i);
		if (NODATA != fZTemp)break;
		iCount++;
	}
	if (iCount == iRows_i-iRowID_i)return true;
	//Right
	iCount = 0;
	for (int j = iColID_i; j < iCols_i;j++)
	{
		float fZTemp = *(pZ_i + iRowID_i*iCols_i + j);
		if (NODATA != fZTemp)break;
		iCount++;
	}
	if (iCount == iCols_i - iColID_i)return true;
	//Left
	iCount = 0;
	for (int j = iColID_i; j >=0; j--)
	{
		float fZTemp = *(pZ_i + iRowID_i*iCols_i + j);
		if (NODATA != fZTemp)break;
		iCount++;
	}
	if (iCount == iColID_i+1)return true;

	return false;
}
/*true: edge pixel of void set*/
bool IsEdgePixel(int &iVoidRowID_i, int &iVoidColID_i, int &iRows_i, int &iCols_i, float *pZ_i,int iRangeNums_i){
	int iCount = 0;
	//8 neighbor
	float*p8NeighbourStart = pZ_i + (iVoidRowID_i - 1)*iCols_i + iVoidColID_i - 1;
	for (int i = 0; i < 3;i++)
	{
		for (int j = 0; j < 3;j++)
		{
			float fZTemp = *(p8NeighbourStart + i*iCols_i + j);
			if (NODATA != fZTemp)iCount++;
		}
	}
	if (iCount >= iRangeNums_i)return true;
	else return false;
}
int main(int argc, char**argv)
{
	if (false == NotePrint(argv, argc, 2)){ printf("Argument: Exe dem_file\n"); return false; }
	printf("Loading...\n");
	char strInputfile[FILE_PN],strOutputfile[FILE_PN]; 
	strcpy(strInputfile, argv[1]); strcpy(strOutputfile, strInputfile);
	sprintf(strrchr(strOutputfile, '.'), "%s", "_VoidFilled.tif");
	printf("File name: %s\n", strInputfile);
	CGdalDem *pDemFile = new CGdalDem;
	GDALDEMHDR *pDemHeader = new GDALDEMHDR;
	if (false == pDemFile->LoadFile(strInputfile,pDemHeader))return false;
	int iRows = pDemHeader->iRow;
	int iCols = pDemHeader->iCol;
	float *pZ = new float[iRows*iCols];
	pDemFile->ReadBlock(pZ, 0, 0, iCols, iRows);
	printf("Filling...\n");
	//1//void set
	vector<VOIDSET>vVoidSet;
	for (int i = 0; i < iRows;i++)
	{
		for (int j = 0; j < iCols;j++)
		{
			VOIDSET *pVoidTemp = new VOIDSET;
			float pZTemp = *(pZ + i*iCols + j);
			if (NODATA == pZTemp){
				if (false==IsDemEdge(i,j,iRows,iCols,pZ))
				{
					pVoidTemp->iRowID = i;
					pVoidTemp->iColID = j;
					vVoidSet.push_back(*pVoidTemp);
				}
			}
			delete[]pVoidTemp;
		}
	}

	int iRangeNums = RANGE_NUM;
	vector<EDGESET>vEdgeSet;
	while (0!=vVoidSet.size())
	{
		//2//edge pixel
		vEdgeSet.clear();
		for (unsigned i = 0; i < vVoidSet.size(); i++)
		{
			if (true == IsEdgePixel(vVoidSet[i].iRowID, vVoidSet[i].iColID, iRows, iCols, pZ,iRangeNums)){
				vEdgeSet.push_back(vVoidSet[i]);
				vVoidSet.erase(vVoidSet.begin() + i);
			}
		}
		if (0 == vEdgeSet.size()){ 
			if (iRangeNums <= 0)break;
			iRangeNums--; continue;
		}
		//3//direct interpolation
		float fWeightedValue[5][5] = { 
			0.125f, 0.2f, 0.25f, 0.2f, 0.125f,
			0.2f, 0.5f, 1.0f, 0.5f, 0.2f,
			0.25f, 1.0f, 0.0f, 1.0f, 0.25f,
			0.2f, 0.5f, 1.0f, 0.5f, 0.2f,
			0.125f, 0.2f, 0.25f, 0.2f, 0.125f
		};
		for (unsigned k = 0; k < vEdgeSet.size(); k++)
		{
			float fZSum = 0.0f, fWeightedSum = 0.0f;
			int iCurrRowID = vEdgeSet[k].iRowID;
			int iCurrColID = vEdgeSet[k].iColID;
			float *p25NeighbourStart = (pZ + (iCurrRowID - 2)*iCols + iCurrColID - 2);
			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < 5; j++)
				{
					float fZTemp = *(p25NeighbourStart + i*iCols + j);
					if (NODATA != fZTemp){
						fZSum += fZTemp*fWeightedValue[i][j];
						fWeightedSum += fWeightedValue[i][j];
					}
				}
			}
			*(pZ + iCurrRowID*iCols + iCurrColID) = fZSum / fWeightedSum;
		}
	}
	//save file
	CGdalDem *pOutputDemFile = new CGdalDem;
	if (false == pOutputDemFile->CreatFile(strOutputfile, pDemHeader))return false;
	pOutputDemFile->WriteBlock(pZ, 0, 0, iCols, iRows);
	printf("Saved file: %s\nOver!", strOutputfile);
	//release memory
	delete pOutputDemFile;
	delete[]pZ;
	delete pDemFile;
	delete pDemHeader;
	return true;
}