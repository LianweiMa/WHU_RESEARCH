// DemMap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DemMap.h"

void Usage(){
	printf( 
        "\n%s", 
        "Usage: DemMap <DemListFile <-gcd gcdpath> or -task task file > [-task task directory] [-srtm] [-m/-ml dempath] [-outpath outpath] [-scale 500~1000000] [-format formatname] [-gsd(m/d) value or xgsd ygsd ] [-exp value or expWidth expHeight] [-meta no/dsm/dem ]\n"
		"Opt:\n"
		"DemListFile:the full path of file which records the dem files path.\n"
		"-task: if the follow path is directory,then create map task into it,else if the follow path is a file,then run the task file to create a map.\n "
		"-srtm: use srtm to promote the precise of dem.\n"
		"-m dempath:Merge all dem files.if dempath is NULL,the default path is the same as DemListFile.\n"
		"-ml: only merge all dem files,don't split it to maps.\n "
		"500~1000000: 1000000 500000 250000 100000 50000 25000 10000 5000 2000 1000 500.The default is 50000\n"
		"formatname: GBNEW GBOLD GJB GJBNEW.The default is GBNEW \n"
		"-gsd(m/d):if gsd,value>=1 the value will be meter,value<1 the value will be degree.if gsdm,the value is meter.if gsdd the value will be degree.\n"
		);
	
    exit( 1 );
}

int main(int argc, char* argv[])
{
	const char* lpstrDemListFile	= NULL;
	const char* lpstrGcdPath		= NULL;
	const char* lpstrTaskPath		= NULL;	bool bTaskDir = false;
	char strMergeDem[512];	memset(strMergeDem,0,sizeof(strMergeDem));
	char strOutPath[512];	memset(strOutPath,0,sizeof(strOutPath));
	enum ProcModel{ PM_MAPONLY,PM_MERGEONLY,PM_MAPMERGE } pm = PM_MAPONLY;
	const char* lpstrScaleTag[] = {"1000000","500000","250000","100000","50000","25000","10000","5000","2000","1000","500"};	int sz_ScaleTag = sizeof(lpstrScaleTag)/sizeof(const char*);
	const char* lpstrMapFormatTag[] = {"GBNEW","GBOLD","GJB","GJBNEW"};	int sz_FormatTag = sizeof(lpstrMapFormatTag)/sizeof(const char*);
	MAPSCALE	mapscale = MS_50000;
	MAPFORMAT	mapformat = MF_GBNEW;
	float fxGSD_map = -1 ,fyGSD_map = -1 ;	
	bool	bGsdIsMeter = true;
	float expWidth = 0,expHeight = 0;
	const char* lpstrMetaTag[] = {"no","dsm","dem"};
	int mdMeta = META_UNDO;
	bool bUseSRTM = false;

	int i=0,j=0;
	for (i=1; i<argc; i++){
		if ( argv[i][0] != '-' ) lpstrDemListFile = argv[i];
		else{
			if ( !strcmp(argv[i],"-m") )	{ pm = PM_MAPMERGE;	if( i+1<argc && argv[i+1][0] != '-' ) { strcpy(strMergeDem,argv[++i]);	} }
			else if ( !strcmp(argv[i],"-ml") )	{	pm=PM_MERGEONLY;	if( i+1<argc && argv[i+1][0] != '-' ) { strcpy(strMergeDem,argv[++i]);	} }
			else if ( !strcmp(argv[i],"-task") ) {	if( i+1<argc && argv[i+1][0] != '-' ) { lpstrTaskPath = argv[++i];	}	}
			else if ( !strcmp(argv[i],"-outpath") ) {	if( i+1<argc && argv[i+1][0] != '-' ) { strcpy(strOutPath,argv[++i]);	}	}
			else if ( !strcmp(argv[i],"-gcd")	)	{	if( i+1<argc && argv[i+1][0] != '-' ) { lpstrGcdPath = argv[++i];	}			}
			else if ( !strcmp(argv[i],"-scale") ){	
				if( i+1<argc && argv[i+1][0] != '-' ) { 
					i++; 
					for(j=0;j<sz_ScaleTag;j++) if( !strcmp(argv[i],lpstrScaleTag[j]) ) { mapscale = (MAPSCALE)j; break;} 
					if(j>=sz_ScaleTag) { printf("Scale must be one of "); for(j=0;j<sz_ScaleTag;j++) printf("%s\t",lpstrScaleTag[j]); return ERR_EXE_PARERR; }
				} 
			}
			else if (  !strcmp(argv[i],"-srtm") ) bUseSRTM = true;
			else if ( !strcmp(argv[i],"-format") )	{
				if( i+1<argc && argv[i+1][0] != '-' ) { 
					i++; 
					for(j=0;j<sz_FormatTag;j++) if( !strcmp(argv[i],lpstrMapFormatTag[j]) ) { mapformat = (MAPFORMAT)j; break;} 
					if(j>=sz_FormatTag) { printf("Format must be one of "); for(j=0;j<sz_FormatTag;j++) printf("%s\t",lpstrMapFormatTag[j]); return ERR_EXE_PARERR; }
				}
			}
			else if( !memcmp(argv[i],"-gsd",4*sizeof(char)) ){
					if( argv[i][4]=='m' )	bGsdIsMeter = true;
					else if(  argv[i][4]=='d' )	bGsdIsMeter = false;
					else Usage();
					
					if( i+1<argc && argv[i+1][0] != '-' && strlen(argv[i+1])<5 ) { fxGSD_map = (float)atof(argv[++i]);	}
					if( i+1<argc && argv[i+1][0] != '-' && strlen(argv[i+1])<5 ) { fyGSD_map = (float)atof(argv[++i]);	}else fyGSD_map = fxGSD_map;
				} 
			else if( !strcmp(argv[i],"-exp" ) ){
				
				if( i+1<argc && argv[i+1][0] != '-' && strlen(argv[i+1])<5 ) { expWidth = (float)atof(argv[++i]);	}
				if( i+1<argc && argv[i+1][0] != '-' && strlen(argv[i+1])<5 ) { expHeight = (float)atof(argv[++i]);	}else expHeight = expWidth;
				} 
			else if ( !strcmp(argv[i],"-meta") )
			{
				if( i+1<argc && argv[i+1][0] != '-' ) { 
					i++; 
					for(j=0;j<3;j++) if( !strcmp(argv[i],lpstrMetaTag[j]) ) { mdMeta = j; break;} 
					if(j>=3) { printf("meta model must be one of "); for(j=0;j<3;j++) printf("%s\t",lpstrMetaTag[j]); return ERR_EXE_PARERR; }
				}
			}
			else Usage();
		}
	}

	CDemMap demMap;

	if(lpstrTaskPath)
	{
		if( IsDir(lpstrTaskPath) ) bTaskDir = true;else 
		{
			return demMap.RunTask(lpstrTaskPath);	
		}
	}

	if ( !lpstrDemListFile ){
		LogPrint(0,"Error: No input dem list file !");
		Usage();
	}
	if ( !lpstrGcdPath ){
		LogPrint(0,"Error: No input gcd file !");
		Usage();
	}

	if(expWidth<0) expWidth = 0;	if(expHeight<0) expHeight = 0;

	if( !strMergeDem[0] ) {
		strcpy(strMergeDem,lpstrDemListFile);	
		char* pS = strrchr(strMergeDem,'.');	if(!pS) pS = strMergeDem+strlen(strMergeDem);
		strcpy(pS,"_merge.tif");
	}
	if ( !strOutPath[0] ){
		strcpy(strOutPath,lpstrDemListFile);
		*strrpath(strOutPath) = 0;	
	}

	LogPrint(ERR_NONE,"MapFormat=	%s",lpstrMapFormatTag[mapformat]);
	LogPrint(ERR_NONE,"MapScale=	%s",lpstrScaleTag[mapscale]);


	int ret;
	demMap.SetGSD(fxGSD_map,fyGSD_map, bGsdIsMeter);
	demMap.SetMetaMode(mdMeta);	demMap.SetSRTMMode(bUseSRTM);
	demMap.SetOutputPath(strOutPath);

	if( (ret=demMap.LoadGcdFile(lpstrGcdPath))!= ERR_NONE ){
		return ret;
	}

	if( (ret=demMap.Load4DemListFile(lpstrDemListFile)) != ERR_NONE ) {
		LogPrint(ERR_NONE,"Please check the dem list file. make sure it's all right");
		return ret ;
	}

	demMap.GetGSD(&fxGSD_map,&fyGSD_map);
	LogPrint(ERR_NONE,"GSD[%s]:\tdx=%lf\tdy=%lf",bGsdIsMeter?"meter":"degree",fyGSD_map,fxGSD_map);
	
	double xy[4];
	demMap.GetAreaGeoRange_degree(xy,xy+1,xy+2,xy+3);
	LogPrint(ERR_NONE,"Area:\t[%lf,%lf]<=>[%lf,%lf]",xy[0],xy[1],xy[2],xy[3]);
    
	CWuSRTM locSrtm;
	if(bUseSRTM){
		CWuGlbHei srtmHei;
		LogPrint(ERR_NONE,"Start Getting SRTM for The Area...");
		int nRows,nCols;
		nRows = int( (xy[3]-xy[1])/SRTM_GSD )+1;	nCols = int( (xy[2]-xy[0])/SRTM_GSD )+1;
		float *pLocZ = new float[ nRows*nCols ];
		srtmHei.GetSRTM( xy[0],xy[1],nCols,nRows,pLocZ );
		locSrtm.Init( xy[0],xy[1],nCols,nRows,pLocZ );
		delete[] pLocZ;
		LogPrint(ERR_NONE,"End Getting SRTM");
	}

	if ( pm==PM_MERGEONLY|| pm==PM_MAPMERGE ){
		demMap.MergeDem(strMergeDem,fxGSD_map,fyGSD_map,bUseSRTM?&locSrtm:NULL);
	}

	if (lpstrTaskPath){
		return demMap.CreateTask(lpstrTaskPath,mapformat,mapscale,expWidth,expHeight);
	}

	if( pm==PM_MAPONLY|| pm==PM_MAPMERGE )
	{
		demMap.Map(strOutPath,mapformat,mapscale,expWidth,expHeight,bUseSRTM?&locSrtm:NULL);
	}

	
	return ERR_NONE;
}


