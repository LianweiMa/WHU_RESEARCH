// WKT2GCD.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "WTKAnalysis.h"

int main(int argc, char* argv[])
{
	if ( argc!=2 && argc!=3 )
	{
		printf("Wrong!");
	    return 0;
	}
  
	char *pS;
	char strWKT[1024];
	FILE *fp;
	if ((fp=fopen(argv[1],"r"))==NULL)
	{
	    printf("Err:\nOpen txt File Failed\n");
		return 0;
	}
    fgets(strWKT, 1024, fp);
	fclose(fp);

	// 创建gcd文件
    char strGcd[256];       strcpy(strGcd,argv[1]);
	pS=strrchr(strGcd,'.'); strcpy(pS,".gcd");

	char *lpstrWTK=strWKT;
    WKT2GCD((char**)&lpstrWTK,strGcd);

	return 1;
}

