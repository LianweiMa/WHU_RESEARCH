#include <stdio.h>
#include <windows.h>
#include "mlw_1_0_0/type_define.h"
#include "sp_1_0_0/SpGeoCvt.h"
int main(int argc, char**argv)
{
	CSpPar7Cvt Para7Cvt;
	Para7Cvt.Set_Cvt_Par_ByGUI();
	char strInputFile[FILE_PN], strOutputFile[FILE_PN];
	strcpy(strInputFile, argv[1]); strcpy(strOutputFile, strInputFile);
	sprintf(strrchr(strOutputFile, '.'), "%s", "_T.txt");
	int iPtNums = 0; char strTmp[CHAR_LEN];
	FILE *fp = fopen(strInputFile, "r"), *fp1 = fopen(strOutputFile, "w");
	fscanf(fp, "%d\n", &iPtNums); fprintf(fp1, "%d\n", iPtNums);
	DPT3D *p3DPt = new DPT3D[iPtNums];
	for (int i = 0; i < iPtNums;i++)
	{
		fscanf(fp, "%s %lf %lf %lf\n",strTmp, &(p3DPt + i)->x, &(p3DPt + i)->y, &(p3DPt + i)->z);
		double x, y, z;
		//Para7Cvt.Cvt_WCXYZ2LCXYZ((p3DPt + i)->x, (p3DPt + i)->y, (p3DPt + i)->z, &x, &y, &z);
		Para7Cvt.Cvt_LCXYZ2WCXYZ((p3DPt + i)->x, (p3DPt + i)->y, (p3DPt + i)->z, &x, &y, &z);
		fprintf(fp1, "%s %lf %lf %lf\n",strTmp, x, y, z);
	}
	fclose(fp);
	fclose(fp1);

	delete[]p3DPt;
	return true;
}