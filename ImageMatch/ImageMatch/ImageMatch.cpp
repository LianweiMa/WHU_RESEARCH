#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "ResearchCode/CommonOP.hpp"
#include "ResearchCode/TypeDef.h"
#include "Whu/WuSift.h"
#include "ResearchCode/CGdalImage.hpp"
int main(int argc, char**argv)
{
	if (false == NotePrint(argv, argc, 4)){ printf("Argument: left_image right_image match_point_file\n"); return false; }
	printf("Running...\n");
	char strLeftImgfile[FILE_PN], strRightImgfile[FILE_PN], strMchPtFile[FILE_PN];
	strcpy(strLeftImgfile, argv[1]); strcpy(strRightImgfile, argv[2]); strcpy(strMchPtFile, argv[3]);
	CGdalImage *pLeftImgFile = new CGdalImage,*pRightImgFile=new CGdalImage;
	if (false == pLeftImgFile->LoadFile(strLeftImgfile) || false == pRightImgFile->LoadFile(strRightImgfile))return false;
	int iLeftImgCols = pLeftImgFile->GetCols(), iLeftImgRows = pLeftImgFile->GetRows();
	int iRigthImgCols = pRightImgFile->GetCols(), iRightImgRows = pRightImgFile->GetRows();
	BYTE*pLeftImgData = new BYTE[iLeftImgCols*iLeftImgRows], *pRightImgData = new BYTE[iRightImgRows*iRigthImgCols];
	pLeftImgFile->ReadBlock(pLeftImgData, 0, 0, iLeftImgCols, iLeftImgRows);
	pRightImgFile->ReadBlock(pRightImgData, 0, 0, iRigthImgCols, iRightImgRows);
	int iMchPtNums = 0;
	CWuSift *pWuSift = new CWuSift;
	const FPT4D *pMchPtList = pWuSift->Sift_Match(&iMchPtNums, pLeftImgData, iLeftImgCols, iLeftImgRows, pRightImgData, iRigthImgCols, iRightImgRows);
	FILE*fp = fopen(strMchPtFile, "w");if (NULL == fp)return false;
	fprintf(fp, "%d\n", iMchPtNums);
	for (int i = 0; i < iMchPtNums;i++)
	{
		fprintf(fp, "%d\t\t\t%f\t\t%f\t\t%f\t\t%f\n", i + 1, pMchPtList[i].xl, pMchPtList[i].yl, pMchPtList[i].xr, pMchPtList[i].yr);
	}
	pWuSift->FreePtsMem();
	fclose(fp);
	delete[]pLeftImgData; delete[]pRightImgData;
	delete pLeftImgFile, pRightImgFile, pWuSift;
	return true;
}