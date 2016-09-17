
#include "LxString.hpp"
#include "DemMap.h"
#include "meta/xml_1_50000_dem_w.h"
#include "meta/xml_1_50000_dsm_w.h"

inline int CompareStr(const char *s1, const char *s2,BOOL bIgnoreCaps = TRUE){
	if(!bIgnoreCaps) return strcmp(s1,s2);
	else{
		const char *pS1,*pS2;	pS1 = s1;	pS2 = s2;
		int deta;	static int d = (int)('a'-'A');
		while(*pS1&&*pS2){
			deta = (int)(*pS1-*pS2);
			if( !deta||deta == d||deta == -d) { pS1++;pS2++;continue;}
			else return deta;
		}
		return 0;
	}
	
}

inline void Cvt2BLH(double x,double y,double z,double* lon,double* lat,double* h,CSpGeoCvt* m_geoCvt)
{
	int Ellipsoid;           //椭球类型
	int Projectid;           //投影类型  
    double EllipsoA;         //椭球长半轴
    double EllipsoB;	     //椭球短半轴      
    double ProjectOriLat;    //中心纬度(弧度)
    double ProjectCenMeri;   //中心经度(弧度)
    double ProjectFalseEas;  //东偏移
   	double ProjectFalseNor;  //北偏移
    double ProjectScale;     //中心经线比例尺
   	int eleType;
	
	m_geoCvt->Get_Cvt_Par(&Ellipsoid, &Projectid, &EllipsoA, &EllipsoB, &ProjectOriLat, &ProjectCenMeri, 
		&ProjectFalseEas, &ProjectFalseNor, &ProjectScale, &eleType );
	
	if (Projectid==GEODETIC_LONLAT){
		*lon = x;
		*lat = y;
		*h = z;
	}else if (Projectid==GEODETIC_CENTRXYZ){
		m_geoCvt->Cvt_CXYZ2LBH(x,y,z,lon,lat,h);
		*lon = *lon*SPGC_R2D;	*lat = *lat*SPGC_R2D;
	}else{
		m_geoCvt->Cvt_Prj2LBH(x,y,z,lon,lat,h);
		*lon = *lon*SPGC_R2D;	*lat = *lat*SPGC_R2D;
	}
}

inline double Max(double* a,int n)
{
	double maxA = a[0];
	for (int i=1; i<n ; i++){
		if( maxA<a[i] ) maxA = a[i];
	}
	return maxA;
}
inline double Min(double* a,int n)
{
	double minA = a[0];
	for (int i=1; i<n ; i++){
		if( minA>a[i] ) minA = a[i];
	}
	return minA;
}

inline void exch (float *array, int i, int j)
{
    if (i!=j)
    {
//        array[i] ^= array[j];
//       array[j] = array[i] ^ array[j];
//       array[i] ^= array[j];
		float tmp;
		tmp = array[i];	
		array[i] = array[j];
		array[j] = tmp;
    }
}

float select_middle (float *array, int beg, int end, int n)
{
    if (n == 1)
        return array[0];
    int i = beg, j;
    for (j=i+1; j<=end; ++j)
        if (array[j]<=array[beg])
        {
            ++i;
            exch (array, i, j);
        }
		exch (array, beg, i);
		
		if (i < n/2)
			return select_middle (array, i+1, end, n);
		else if (i > n/2)
			return select_middle (array, beg, i-1, n);
		else
		{
			if (n%2)
				return array[i];
			else
			{
				int j, m=array[0];
				for (j=1; j<i; ++j)
					if (array[j] > m)
						m=array[j];
					return (array[i]+m)/2;
			}
		}
}

#define ELEVATION_RAGNE_FLATLAND	120
#define SRTM_PRECISE				120

inline float GetZ(float* z,int num,short srtmZ){
	float data = NOVALUE;

	if ( srtmZ != -9999 ){
		int idx;
		for(idx=0; idx<num; idx++){
			if( fabs(z[idx]-srtmZ)>SRTM_PRECISE ) continue; 
			if( data == NOVALUE || (fabs(z[idx]-srtmZ)<fabs(data-srtmZ)) ) data = z[idx];
		}
		if( data == NOVALUE ) data = srtmZ;
	}
	else{
//		data = select_middle(z,0,num-1,num);
		data = 0;
		for(int i=0; i<num; i++)	data += z[i];
		data /= num;
	}

	return data;
}

// int partition(int *array, int left, int right)  
// {  
//     if (array == NULL)  
//         return -1;  
// 	
//     int pos = right;  
//     right--;  
//     while (left <= right)  
//     {  
//         while (left < pos && array[left] <= array[pos])  
//             left++;  
// 		
//         while (right >= 0 && array[right] > array[pos])  
//             right--;  
// 		
//         if (left >= right)  
//             break;  
// 		
//         swap(array[left], array[right]);  
//     }  
//     swap(array[left], array[pos]);  
// 	
//     return left;  
// }  
// 
// int getMidIndex(int *array, int size)  
// {  
//     if (array == NULL || size <= 0)  
//         return -1;  
// 	
//     int left = 0;  
//     int right = size - 1;  
//     int midPos = right >> 1;  
//     int index = -1;  
// 	
//     while (index != midPos)  
//     {  
//         index = partition(array, left, right);  
// 		
//         if (index < midPos)  
//         {  
//             left = index + 1;  
//         }  
//         else if (index > midPos)  
//         {  
//             right = index - 1;  
//         }   
//         else  
//         {  
//             break;  
//         }  
//     }  
// 	
//     assert(index == midPos);  
//     return array[index];  
// }  

// ///////////////////////////////////////////////////////////////////////////////////
//
void CDemMap::Reset()
{
	ReleaseDemBuf();
	ReleaseMapBuf();

	m_nDemNum = 0;
	m_arDemPath.Reset();	

	m_xmax_degree=-1e15,m_xmin_degree=1e15,m_ymax_degree=-1e15,m_ymin_degree=1e15;
	m_xmax_meter=-1e15,m_xmin_meter=1e15,m_ymax_meter=-1e15,m_ymin_meter=1e15;
	m_bGsdIsMeter = true;

	m_xgsd_map = -1;	m_ygsd_map = -1;	m_mdMeta = META_UNDO;
	m_bSRTM = true;

	memset(m_strGcdPath,0,512);
	memset(m_strOutPath,0,512);
}

CDemMap::~CDemMap()
{
	Reset();
}

void CDemMap::ReleaseDemBuf()
{
	if(m_dem) 
	{
		for (int i=0; i<m_nDemNum; i++){
			if ( m_dem[i].IsLoaded() ){
				m_dem[i].Close();
			}
		}
		delete[] m_dem;	m_dem = NULL;
	}	

	if(m_dem_prj) delete[] m_dem_prj;m_dem_prj = NULL;
}

int CDemMap::AppendDem(const char* lpstrDemPath)
{
	CSpDem* pDem = m_dem+m_nDemNum;
	if ( !pDem->Load4File(lpstrDemPath) ){
		LogPrint(ERR_FILE_READ,"Warning:%s is Unsupport Dem Type!",lpstrDemPath);
		return ERR_FILE_OPEN;
	}

	double x[4],y[4];
	double z_max,z_min,x_max,x_min,y_max,y_min;
	GEOPROJECTION* pDem_Prj = m_dem_prj+m_nDemNum;
	CWuGlbHei srtmHei;	float h_srtm_max,h_srtm_min;

	LogPrint(ERR_NONE,"-----%s-----",lpstrDemPath);
	z_max = pDem->GetMaxZ();	z_min = pDem->GetMinZ();
	LogPrint(ERR_NONE,"Zmax:%lf\tZmin:%lf\t",z_max,z_min);
	pDem->GetDemRgn(x,y);	LogPrint(ERR_NONE,"GeoRange:[%lf,%lf]\t[%lf,%lf]\t[%lf,%lf]\t[%lf,%lf]\t",x[0],y[0],x[1],y[1],x[2],y[2],x[3],y[3]);
	
	double h = z_min;
	x_max = Max(x,4);	y_max = Max(y,4);
	x_min = Min(x,4);	y_min = Min(y,4);
	if( x_max>180 || x_min<-180 || y_max>90 || y_min<-90 ) *pDem_Prj = GP_PRJ;else *pDem_Prj = GP_LONLAT;
	LogPrint(ERR_NONE,"Projection Type : %s",*pDem_Prj==GP_PRJ?"Projection":"BLH");
	
	if( *pDem_Prj==GP_PRJ )	
	{
		Cvt2BLH(x_max,y_max,h,&x_max,&y_max,&h,&m_geoCvt);	Cvt2BLH(x_min,y_min,h,&x_min,&y_min,&h,&m_geoCvt);
		if( x_max>180 || x_min<-180 || y_max>90 || y_min<-90 ) { LogPrint(ERR_NONE,"Warning:This file is not match to the gcd file!");	LogPrint(ERR_NONE,"-----Remove-----\n"); pDem->Close();	return ERR_FILE_READ;}
	}
	
	if (m_bSRTM){
		srtmHei.GetHeightRange(x_min,y_min,x_max,y_max,&h_srtm_min,&h_srtm_max);
		LogPrint(ERR_NONE,"GetSRTM:%f=>%f",h_srtm_min,h_srtm_max);
		
		if ( (h_srtm_min-z_max)>SRTM_PRECISE || (z_min-h_srtm_max)>SRTM_PRECISE ){
			LogPrint(ERR_NONE,"Warning:This dem file is invalid according to SRTM Elevation Range");
			LogPrint(ERR_NONE,"-----Remove-----\n");
			pDem->Close();
			return ERR_FILE_READ;
		}
		if ( z_max-z_min < ELEVATION_RAGNE_FLATLAND &&( fabs(h_srtm_min-z_min)>SRTM_PRECISE || fabs(h_srtm_max-z_max)>SRTM_PRECISE ) ){
			LogPrint(ERR_NONE,"Warning:This dem file is invalid according to SRTM Elevation Range");
			LogPrint(ERR_NONE,"-----Remove-----\n");
			pDem->Close();
			return ERR_FILE_READ;
		}
	}
	
	if( m_xmin_degree>x_min ) m_xmin_degree=x_min;	if( m_xmax_degree<x_max ) m_xmax_degree=x_max;
	if( m_ymin_degree>y_min ) m_ymin_degree=y_min;	if( m_ymax_degree<y_max ) m_ymax_degree=y_max;
	
	if( *pDem_Prj==GP_PRJ )
	{
		x_max = Max(x,4);	y_max = Max(y,4);
		x_min = Min(x,4);	y_min = Min(y,4);
		
	}else{
		m_geoCvt.Cvt_LBH2Prj(x_min*SPGC_D2R,y_min*SPGC_D2R,z_min,&x_min,&y_min,&h);
		m_geoCvt.Cvt_LBH2Prj(x_max*SPGC_D2R,y_max*SPGC_D2R,z_max,&x_max,&y_max,&h);
	}
	if( m_xmin_meter>x_min ) m_xmin_meter=x_min;	if( m_xmax_meter<x_max ) m_xmax_meter=x_max;
	if( m_ymin_meter>y_min ) m_ymin_meter=y_min;	if( m_ymax_meter<y_max ) m_ymax_meter=y_max;
	
	LogPrint(ERR_NONE,"-----Append-----\n");
	
	if ( m_xgsd_map<0 || m_ygsd_map<0){
		SPDEMHDR* pHdr = pDem->GetDemHeaderPr();
		m_xgsd_map = pHdr->intervalX;
		m_ygsd_map = pHdr->intervalY;
		
		if( m_xgsd_map<1 ) m_bGsdIsMeter = false;
	}
	m_nDemNum++;
	return ERR_NONE;
}

int CDemMap::Load4DemListFile(const char* lpstrListFile)
{
	FILE* fp = fopen(lpstrListFile,"rt");	if(!fp) {	LogPrint(ERR_FILE_OPEN,"Error:Can't Open %s!",lpstrListFile);	return ERR_FILE_OPEN;	}
	
	char strLine[1024];	
	int nTotalNum = 0;
	fgets(strLine,1024,fp);	sscanf(strLine,"%d",&nTotalNum);
	if(nTotalNum==0) { fclose(fp);	LogPrint(ERR_FILE_READ,"Please Enter the Total Number of DemFile to the first line of the DemListFile!"); return ERR_FILE_READ;	}
			
	MallocDemBuf(nTotalNum);
	int i;
	for (i=0; i<nTotalNum; i++){
		fgets(strLine,1024,fp);	VERF_LINE(strLine);
		
		if( AppendDem(strLine) == ERR_NONE ) AppendDemPath(strLine);
	}
	fclose(fp);

	LogPrint(ERR_NONE,"Num of DEM=	%d",m_nDemNum);

	return (m_nDemNum>0)?ERR_NONE:ERR_FILE_READ;
}

MapInfo* CDemMap::GetMapsByRange(MAPFORMAT format,MAPSCALE scale,int* n)
{
	MapInfo infoIn; memset(&infoIn, 0, sizeof(MapInfo));
	infoIn.format = format;
	infoIn.scale  = scale;
	infoIn.Lon0 = m_xmin_degree; infoIn.dLon = m_xmax_degree-m_xmin_degree;
	infoIn.Lat0 = m_ymin_degree; infoIn.dLat = m_ymax_degree-m_ymin_degree;
	
	int sz_map = 0;	
	
	CStdMap stdMap;
	MapInfo* mapInfo = stdMap.GetMapsByRange(infoIn, &sz_map);
	LogPrint(ERR_NONE,"\nTotal Map Num=	%d",sz_map);

	MallocMapBuf(sz_map);
	memcpy(m_pMapInfo,mapInfo,sz_map*sizeof(MapInfo));

	*n = sz_map;
	return m_pMapInfo;
}

///////////////////////大区域问题
int	CDemMap::MergeDem(const char* lpstrMergePath,float xgsd,float ygsd,CWuSRTM* pLocSrtm)
{
	int i,j;
	LogPrint(ERR_NONE,"-----Merge Dem File-----");

	float* z = new float[m_nDemNum];	int numZ;

	int xmax,xmin,ymax,ymin;
	int nRows,nCols;	double lat,lon,x,y,z_tmp;	
	if(m_bGsdIsMeter) { xmax = m_xmax_meter; xmin = m_xmin_meter; ymax=m_ymax_meter;	ymin=m_ymin_meter;	}
	else { xmax = m_xmax_degree; xmin = m_xmin_degree; ymax=m_ymax_degree;	ymin=m_ymin_degree; }

	nRows = int( (ymax-ymin)/ygsd )+1;	nCols = int( (xmax-xmin)/xgsd )+1;
	
	LogPrint(ERR_NONE,"Rows=	%d;Cols=	%d",nRows,nCols);
	float* data = new float[nRows*nCols+128];	float* pZ = data;
	
	printf("Merge Dem ...  0%%");	int step_inter = nRows/10+1;	int step_cur = 1;
	y = ymin;	
	for (i=0; i<nRows; i++){
		x = xmin;
		for (j=0; j<nCols; j++){
			
			*pZ = NOVALUE;
			if( GetDemZValue(x,y,z,&numZ,m_bGsdIsMeter) ) {
				short  srtmZ = -9999;
				if(pLocSrtm){
					if(m_bGsdIsMeter) { m_geoCvt.Cvt_Prj2LBH(x,y,0,&lon,&lat,&z_tmp);	lon *= SPGC_R2D;	lat *= SPGC_R2D; }
					else { lon = x; lat = y; }
					srtmZ = pLocSrtm->GetZVal(lon,lat);
				}
				
				*pZ = GetZ(z,numZ,srtmZ);
			}
			
			pZ++;
			x += xgsd;
			
		}
		y += ygsd;
		if( (i+1)>=(step_cur*step_inter) ) {	printf("\b\b\b\b%2.0d0%%",step_cur); step_cur+=1;	}
	}
	delete[] z;
	printf("\b\b\b\b100%%\n");
	
	LogPrint(ERR_NONE,"Saving merge_dem ...");
	
	CSpDem dem_merge;
	SPDEMHDR DemHdr;
	DemHdr.startX	= m_xmin_meter; DemHdr.startY	  = m_ymin_meter;
	DemHdr.intervalX= xgsd; DemHdr.intervalY= ygsd;
	DemHdr.column	= nCols;   DemHdr.row=nRows;
	DemHdr.kapa		= 0;
	dem_merge.Attach( DemHdr,data );	dem_merge.FilterSmooth();
	
	bool fail = false;
	char strMergeDem[512];	strcpy(strMergeDem,lpstrMergePath);
	if( !CompareStr(strrchr(strMergeDem,'.'),"tif") )	fail = !dem_merge.Save2File( strMergeDem  ,VER_TIF );
	else		fail = !dem_merge.Save2File( strMergeDem,VER_BN  );
	
	if(!fail) LogPrint(ERR_NONE,"%s is Saved",strMergeDem);
	else {	LogPrint(ERR_FILE_WRITE,"Fail to save %s!",strMergeDem); }
	LogPrint(ERR_NONE,"-----End Merge Dem File-----");
		
	return ERR_NONE;
}

int CDemMap::MergeDem(const char* lpstrMergePath,float xgsd,float ygsd)
{
	CWuSRTM locSrtm;
	GetSRTMBuf(locSrtm);
	
	return MergeDem(lpstrMergePath,xgsd,ygsd,&locSrtm);
}

int CDemMap::RunTask(MapInfo mapInfo,float fExpWidth,float fExpHeight,CWuSRTM* pLocSrtm,const char* lpstrDir/* =m_strOutPath */,float xgsd/* =m_xgsd_map */,float ygsd/* =m_ygsd_map */,bool bMeter/* =m_bGsdIsMeter */,int meta/* =m_mdMeta */)
{
	if( fExpWidth/xgsd>300 || fExpHeight/ygsd>300 ) return ERR_EXE_PARERR;

	double gx[4],gy[4];	double tz;
	LogPrint(ERR_NONE,"-----map[%s]-----",mapInfo.pszName);
	
	m_geoCvt.Cvt_LBH2Prj( (mapInfo.Lon0*SPGC_D2R),(mapInfo.Lat0*SPGC_D2R),0,gx,gy,&tz );
	gx[0] -= fExpWidth ; gy[0] -= fExpHeight;
	m_geoCvt.Cvt_LBH2Prj( (mapInfo.Lon0+mapInfo.dLon)*SPGC_D2R,(mapInfo.Lat0*SPGC_D2R),0,gx+1,gy+1,&tz );
	gx[1] += fExpWidth ; gy[1] -= fExpHeight;
	m_geoCvt.Cvt_LBH2Prj( (mapInfo.Lon0+mapInfo.dLon)*SPGC_D2R,(mapInfo.Lat0+mapInfo.dLat)*SPGC_D2R,0,gx+2,gy+2,&tz );
	gx[2] += fExpWidth ; gy[2] += fExpHeight;
	m_geoCvt.Cvt_LBH2Prj( mapInfo.Lon0*SPGC_D2R,(mapInfo.Lat0+mapInfo.dLat)*SPGC_D2R,0,gx+3,gy+3,&tz );
	gx[3] -= fExpWidth ; gy[3] += fExpHeight;
	
	if( !GetDemZValue((gx[0]+gx[2])/2,(gy[0]+gy[2])/2,NULL,NULL,bMeter) ) 
	{
		LogPrint(ERR_NONE,"at least half of this map is invaild value,so remove it");
		LogPrint(ERR_NONE,"-----Remove-----\n");
		return ERR_DEM_FLAG;
	}
	
	double x_min,y_min;
	if(bMeter) { x_min = gx[0];	y_min = gy[0];	}
	else { x_min = mapInfo.Lon0;	y_min = mapInfo.Lat0;	}

	int nRows, nCols;
	if(bMeter)	{ nCols = int((gx[1]-gx[0])/xgsd)+1;	nRows = int((gy[2]-gy[0])/ygsd)+1; }
	else { nCols = int((mapInfo.dLon+2*fExpWidth)/xgsd)+1;	nRows = int((mapInfo.dLat+2*fExpHeight)/ygsd)+1; }

	CSpDem demFileW; SPDEMHDR DemHdr;
	float *pDem = new float[ nCols*nRows+8 ];	float* pZ = pDem;
	float* z = new float[m_nDemNum];	int numZ;

	LogPrint(ERR_NONE,"Start Building %d*%d Grid...",nRows,nCols);
	double x,y,lat,lon;	int r,c;
	y = y_min;
	for (r=0; r<nRows; r++){
		x = x_min;
		for (c=0; c<nCols; c++){
			*pZ = NOVALUE;
			if( GetDemZValue(x,y,z,&numZ,bMeter) ) {
				short  srtmZ = -9999;
				if(pLocSrtm){
					m_geoCvt.Cvt_Prj2LBH(x,y,0,&lon,&lat,&tz);	lon *= SPGC_R2D;	lat *= SPGC_R2D;
					srtmZ = pLocSrtm->GetZVal(lon,lat);
				}
				
				*pZ = GetZ(z,numZ,srtmZ);
			}
			x += xgsd;	pZ++;
		}
		y += ygsd;
	}
	
	DemHdr.startX	= x_min; DemHdr.startY	  = y_min;
	DemHdr.intervalX= xgsd; DemHdr.intervalY= ygsd;
	DemHdr.kapa		= 0;
	DemHdr.column	= nCols;   DemHdr.row=nRows;
	
	char strPath[512];	strcpy(strPath,lpstrDir);	char* pPath = AddEndSlash(strPath);

	if( meta == META_DSM ) sprintf(pPath+1,"DSM_%s",mapInfo.pszName);
	else if( meta == META_DEM ) sprintf(pPath+1,"DEM_%s",mapInfo.pszName);
	else sprintf(pPath+1,"%s",mapInfo.pszName);
	CreateDir(strPath);	strcat(pPath,"/");	strcat(pPath,mapInfo.pszName);	strcat(pPath,".tif");
	
	LogPrint(ERR_NONE,"Save dem file to %s...",strPath);
	
	demFileW.Attach( DemHdr,pDem,false );	demFileW.FilterSmooth();
	demFileW.Save2File(strPath, VER_TIF);
	demFileW.Detach();
	
	char strXml[512];	strcpy(strXml,strPath);	strcpy(strrchr(strXml,'.'),".xml");
	char strGridSize[50];	sprintf(strGridSize,"%d,%d",int(xgsd),int(ygsd));
	if ( meta == META_DSM ){
		write_1_50000DSMXML(strPath,mapInfo.pszName,strGridSize,nRows,nCols,gx[0],gy[0],gx[2],gy[2],m_geoCvt,strXml);
	}else if ( meta == META_DEM ){
		write_1_50000DEMXML(strPath,mapInfo.pszName,strGridSize,nRows,nCols,gx[0],gy[0],gx[2],gy[2],m_geoCvt,strXml);
	}
	LogPrint(ERR_NONE,"-----Done-----\n");
	delete[] pDem;
	delete[] z;

	return ERR_NONE;
}

int CDemMap::Map(const char* lpstrDir, MAPFORMAT mapformat,MAPSCALE mapscale,float fExpWidth,float fExpHeight)
{
	CWuSRTM locSrtm;
	GetSRTMBuf(locSrtm);

	return Map(lpstrDir,mapformat,mapscale,fExpWidth,fExpHeight,&locSrtm);
}

int CDemMap::Map(const char* lpstrDir, MAPFORMAT mapformat,MAPSCALE mapscale,float fExpWidth,float fExpHeight,CWuSRTM* pLocSrtm)
{
	float fxgsd,fygsd;	fxgsd = m_xgsd_map;	fygsd = m_ygsd_map;
	
	if( fExpWidth/fxgsd>300 || fExpHeight/fygsd>300 ) return ERR_EXE_PARERR;
	
	int sz_map;
	MapInfo* mapInfo = GetMapsByRange(mapformat,mapscale,&sz_map);
	
	MapInfo* pm = mapInfo;
	for (int i=0; i<sz_map; i++,pm++){
		RunTask(*pm,fExpWidth,fExpHeight,pLocSrtm,lpstrDir,fxgsd,fygsd,m_bGsdIsMeter,m_mdMeta);
	}
	
	return ERR_NONE;
		
}


inline bool CDemMap::GetDemZValue(double x,double y,float* z,int* numZ,bool bPrjCoor /* = true */)
{
	int idx,sz = 0;	float z_tmp;
	double lat,lon;	double tz;
	if( bPrjCoor ) {	m_geoCvt.Cvt_Prj2LBH(x,y,0,&lon,&lat,&tz);	lon *= SPGC_R2D;	lat *= SPGC_R2D;	}
	else { lat = y;	lon = x;	m_geoCvt.Cvt_LBH2Prj(lon*SPGC_D2R,lat*SPGC_D2R,0,&x,&y,&tz);	 }

	for(idx=0; idx<m_nDemNum; idx++)
	{
		if( m_dem_prj[idx] == GP_PRJ ){	z_tmp = m_dem[idx].GetDemZValue(x,y);	}
		else if( m_dem_prj[idx] == GP_LONLAT	) { z_tmp = m_dem[idx].GetDemZValue(lon,lat);	}
		else continue;
		if( z_tmp == NOVALUE ) continue;
		
		if(z) z[sz] = z_tmp;	
		sz += 1;
	}

	if(numZ) *numZ = sz;

	if( sz == 0) return false; else return true;
}

inline bool CDemMap::GetDemFileIdx(double x,double y,int* idx,int* num,bool bPrjCoor /* = true */)
{
	int i,sz=0;
	double lat,lon;	double tz;	float z_tmp;
	if( bPrjCoor ) {	m_geoCvt.Cvt_Prj2LBH(x,y,0,&lon,&lat,&tz);	lon *= SPGC_R2D;	lat *= SPGC_R2D;	}
	else { lat = y;	lon = x;	m_geoCvt.Cvt_LBH2Prj(lon*SPGC_D2R,lat*SPGC_D2R,0,&x,&y,&tz);	 }
	
	for(i=0; i<m_nDemNum; i++)
	{
		if( m_dem_prj[i] == GP_PRJ ){	z_tmp = m_dem[i].GetDemZValue(x,y);	}
		else if( m_dem_prj[i] == GP_LONLAT	) { z_tmp = m_dem[i].GetDemZValue(lon,lat);	}
		else continue;
		if( z_tmp == NOVALUE ) continue;
		
		idx[sz] = i;
		sz += 1;
	}

	*num = sz;
	if( sz == 0) return false; else return true; 
}

#define DEMMAP_TASKFILE_VERSION "DEMMAP_TASKFILE_V1.0"

void AppendArray(int* a1,int& n1,int* a2,int n2)
{
	if( n1==0 ) { memcpy(a1,a2,sizeof(int)*n2); n1=n2; return; }
	int i=0,j=0;
	while(1)
	{
		if ( a1[i]==a2[j] ){
			i++; j++;
		}else if( a1[i] < a2[j] )
		{
			i++;
		}else{
			for(int k=n1; k>=i; k--) { a1[k+1] = a1[k]; }
			n1++;	a1[i] = a2[j];
			j++; i++;
		}
		if ( j>=n2 ){
			break;
		}
		if ( i>=n1 ){
			memcpy(a1+i,a2+j,sizeof(int)*(n2-j));
		}
	}
}

bool CDemMap::SaveTaskFile(const char* lpstrTskFile,MapInfo* pm,float fExpWidth,float fExpHeight,float xgsd/* =m_xgsd_map */,float ygsd/* =m_ygsd_map */,bool bMeter/* =m_bGsdIsMeter */,bool bSRTM/* =m_bSRTM */,int meta/* =m_mdMeta */,const char* lpstrOutput)
{
	LogPrint(0,"-----Create Task[%s]-----",pm->pszName);
	double gx[4],gy[4],tz;
	m_geoCvt.Cvt_LBH2Prj( (pm->Lon0*SPGC_D2R),(pm->Lat0*SPGC_D2R),0,gx,gy,&tz );
	gx[0] -= fExpWidth ; gy[0] -= fExpHeight;
	m_geoCvt.Cvt_LBH2Prj( (pm->Lon0+pm->dLon)*SPGC_D2R,(pm->Lat0*SPGC_D2R),0,gx+1,gy+1,&tz );
	gx[1] += fExpWidth ; gy[1] -= fExpHeight;
	m_geoCvt.Cvt_LBH2Prj( (pm->Lon0+pm->dLon)*SPGC_D2R,(pm->Lat0+pm->dLat)*SPGC_D2R,0,gx+2,gy+2,&tz );
	gx[2] += fExpWidth ; gy[2] += fExpHeight;
	m_geoCvt.Cvt_LBH2Prj( pm->Lon0*SPGC_D2R,(pm->Lat0+pm->dLat)*SPGC_D2R,0,gx+3,gy+3,&tz );
	gx[3] -= fExpWidth ; gy[3] += fExpHeight;
	
	if( !GetDemZValue((gx[0]+gx[2])/2,(gy[0]+gy[2])/2,NULL,NULL,bMeter) ) 
	{
		LogPrint(ERR_NONE,"at least half of this map is invaild value,so remove it");
		LogPrint(ERR_NONE,"-----Remove-----\n");
		return false;
	}

	FILE* fp = fopen(lpstrTskFile,"w");	if(!fp) return false;

	fprintf(fp,"%s\n",DEMMAP_TASKFILE_VERSION);

	fprintf(fp,"%s\n",lpstrOutput);

	fprintf(fp,"%s\n",pm->pszName);
	fprintf(fp,"%d %d\n",pm->format,pm->scale);
	fprintf(fp,"%lf %lf\n",pm->Lon0,pm->Lat0);
	fprintf(fp,"%lf %lf\n",pm->dLon,pm->dLat);

	fprintf(fp,"%f %f\n",fExpWidth,fExpHeight);
	fprintf(fp,"%f %f\n",xgsd,ygsd);
	fprintf(fp,"%s\n",bMeter?"meter":"degree");

	fprintf(fp,"%s\n",m_geoCvt.GetFilePN());

	fprintf(fp,"%s\n",bSRTM?"srtm":"no-srtm");
	fprintf(fp,"%d\n",meta);

	int* idx0 = new int[m_nDemNum+8];	memset(idx0,0,sizeof(int)*(m_nDemNum+8));
	int* idx1 = new int[m_nDemNum];	int sz1;

	int i,j;
	for (i=0; i<4; i++){
		if( GetDemFileIdx(gx[i],gy[i],idx1,&sz1) )
		{
			for(j=0; j<sz1; j++) { idx0[idx1[j]]++; }
//			AppendArray(idx0,sz0,idx1,sz1);
		}
	}

	sz1 = 0;
	for (i=0; i<m_nDemNum; i++){
		if(idx0[i]>0) idx1[sz1++] = i;
	}

	fprintf(fp,"%d\n",sz1);
	LogPrint(ERR_NONE,"cover dem num=%d",sz1);
	for (i=0; i<sz1; i++){
		fprintf(fp,"%s\n",m_arDemPath[idx1[i]]);
	}

	fclose(fp);
	
	LogPrint(0,"-----Task END-----\n");
	delete[] idx0;
	delete[] idx1;

	return true;
}

bool CDemMap::LoadTaskFile(const char* lpstrTskFile,MapInfo* pm,float* fExpWidth,float* fExpHeight,float* xgsd,float* ygsd,bool* bMeter,bool* bSRTM,int* meta,char* strOutput)
{
	FILE* fp = fopen(lpstrTskFile,"r"); if(!fp) return false;
	
	char strLine[1024],strVal[512];
	fgets(strLine,1024,fp);	sscanf(strLine,"%s",strVal);

	if ( strcmp(strVal,DEMMAP_TASKFILE_VERSION) ){
		LogPrint(0,"Unsupport DEMMAP TASKFILE VERSION!");
		fclose(fp);
		return false;
	}
	const char* lpstrScaleTag[] = {"1000000","500000","250000","100000","50000","25000","10000","5000","2000","1000","500"};	int sz_ScaleTag = sizeof(lpstrScaleTag)/sizeof(const char*);
	const char* lpstrMapFormatTag[] = {"GBNEW","GBOLD","GJB","GJBNEW"};	int sz_FormatTag = sizeof(lpstrMapFormatTag)/sizeof(const char*);
	const char* lpstrMetaTag[] = {"no","dsm","dem"};

	LogPrint(0,"-----Read Task File-----");
	fgets(strLine,1024,fp);	sscanf(strLine,"%s",strOutput);

	fgets(strLine,1024,fp);	sscanf(strLine,"%s",pm->pszName);						LogPrint(0,"Map Name=%s",pm->pszName);
	fgets(strLine,1024,fp);	sscanf(strLine,"%d%d",&(pm->format),&(pm->scale));		LogPrint(0,"Map Format= %s\nMap Scale=%s",lpstrMapFormatTag[pm->format],lpstrScaleTag[pm->scale]);
	fgets(strLine,1024,fp);	sscanf(strLine,"%lf%lf",&(pm->Lon0),&(pm->Lat0));		LogPrint(0,"Map Start: lon=%lf lat=%lf",pm->Lon0,pm->Lat0);
	fgets(strLine,1024,fp);	sscanf(strLine,"%lf%lf",&(pm->dLon),&(pm->dLat));		LogPrint(0,"Map Size: dlon=%lf dlat=%lf",pm->dLon,pm->dLat);
	fgets(strLine,1024,fp);	sscanf(strLine,"%f%f",fExpWidth,fExpHeight);			LogPrint(0,"Map Exp: width=%f height=%f",*fExpWidth,*fExpHeight);
	fgets(strLine,1024,fp);	sscanf(strLine,"%f%f",xgsd,ygsd);						LogPrint(0,"Map GSD: dx=%f dy=%f",*xgsd,*ygsd);

	fgets(strLine,1024,fp);	sscanf(strLine,"%s",strVal);	if( !strcmp(strVal,"meter") ) *bMeter = true;else *bMeter = false;
	LogPrint(0,"GSD Type=%s",*bMeter?"meter":"degree");
	
	fgets(strLine,1024,fp);	sscanf(strLine,"%s",strVal);

	if( LoadGcdFile(strVal) != ERR_NONE ) { fclose(fp); return false; }
	LogPrint(0,"gcd File=%s",strVal);

	fgets(strLine,1024,fp);	sscanf(strLine,"%s",strVal);	if( !strcmp(strVal,"srtm") ) *bSRTM = true;else *bSRTM = false;
	LogPrint(0,"Use SRTM=%s",*bSRTM?"yes":"no");

	fgets(strLine,1024,fp);	sscanf(strLine,"%d",meta);		if( *meta > META_END ) *meta = META_UNDO;
	LogPrint(0,"Meta Data=%s",lpstrMetaTag[*meta]);

	int nTotalNum;
	fgets(strLine,1024,fp);	sscanf(strLine,"%d",&nTotalNum);
	
	MallocDemBuf(nTotalNum);
	int i;
	for (i=0; i<nTotalNum; i++){
		fgets(strLine,1024,fp);	VERF_LINE(strLine);
		
		if( AppendDem(strLine) == ERR_NONE ) AppendDemPath(strLine);
	}
	fclose(fp);
	
	LogPrint(ERR_NONE,"Num of DEM=	%d",m_nDemNum);

	LogPrint(0,"-----Task File END-----\n");
	return (m_nDemNum>0)?true:false;
}

int CDemMap::CreateTask(const char* lpstrTskDir, MAPFORMAT mapformat,MAPSCALE mapscale,float fExpWidth,float fExpHeight,float xgsd/* =m_xgsd_map */,float ygsd/* =m_ygsd_map */,bool bMeter/* =m_bGsdIsMeter */,bool bSRTM/* =m_bSRTM */,int meta/* =m_mdMeta */,const char* lpstrOutput/* =m_strOutPath */)
{
	char strPath[512];	
	strcpy(strPath,lpstrTskDir);
	char* pS = AddEndSlash(strPath);	CreateDir(strPath);
	strcpy(pS,"/DemMap_TaskForEachMap.TSK");
	FILE* fp = fopen(strPath,"w");	if(!fp) { LogPrint(ERR_FILE_WRITE,"Error:Can't Create Task List File:%s!",strPath);	return ERR_FILE_WRITE;	}
	strcpy(pS,"/TSK/");	CreateDir(strPath);	pS = strPath+strlen(strPath);

	int num_map,sz;
	MapInfo* pm = GetMapsByRange(mapformat,mapscale,&num_map);	sz = num_map;

	fprintf(fp,"0      \n");

	char execname[256]="";  readlink( "/proc/self/exe",execname,256 );
	for (int i=0; i<num_map; i++,pm++){
		sprintf(pS,"%s.tsk",pm->pszName);
		if( !SaveTaskFile(strPath,pm,fExpWidth,fExpHeight,xgsd,ygsd,bMeter,bSRTM,meta,lpstrOutput) ) { sz--; continue; }
		fprintf(fp,"%s -task %s\n",execname,strPath);
	}
	rewind(fp);
	fprintf(fp,"%d",sz);
	fclose(fp);

	return ERR_NONE;

}

int CDemMap::RunTask(const char* lpstrTskFile)
{
	MapInfo mapInfo;
	float fExpWidth, fExpHeight;
	if( !LoadTaskFile(lpstrTskFile,&mapInfo,&fExpWidth,&fExpHeight) )
	{
		LogPrint(ERR_FILE_READ,"Error:Fail to Load Task File:%s!",lpstrTskFile);
		return ERR_FILE_READ;
	}

	CWuSRTM locSRTM;
	if (m_bSRTM){
		GetSRTMBuf(locSRTM);
	}
	return RunTask(mapInfo,fExpWidth,fExpHeight,&locSRTM,m_strOutPath,m_xgsd_map,m_ygsd_map,m_bGsdIsMeter,m_mdMeta);
}