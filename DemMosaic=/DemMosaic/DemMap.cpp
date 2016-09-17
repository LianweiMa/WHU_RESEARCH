
//#include "LxString.hpp"
#include "DemMap.h"

inline int CompareStr(const char *s1, const char *s2,bool bIgnoreCaps = true){
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

inline float GetZ(float* z, int num, short srtmZ){
	float data = NOVALUE;

	/*if (srtmZ !=NOVALUE){
		int idx;
		for (idx = 0; idx < num; idx++){
			if (fabs(z[idx] - srtmZ) > SRTM_PRECISE) continue;
			if (data == NOVALUE || (fabs(z[idx] - srtmZ) < fabs(data - srtmZ))) data = z[idx];
		}
		if (data == NOVALUE) data = srtmZ;
	}
	else{
		//		data = select_middle(z,0,num-1,num);
		data = 0;
		for (int i = 0; i < num; i++)	data += z[i];
		data /= num;
	}*/
	int idx = 0;
	for (idx = 0; idx < num; idx++){
		if (z[idx] != NOVALUE) { data = z[idx]; break; }
	}
	if (data == NOVALUE) data = srtmZ;
	return data;
}


// ///////////////////////////////////////////////////////////////////////////////////
//
void CDemMap::Reset()
{
	ReleaseDemBuf();

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
	if ( !pDem->Load4File(lpstrDemPath)){
		printf("Warning:%s is Unsupport Dem Type!\n",lpstrDemPath);
		return false;
	}

	double x[4],y[4];
	double z_max,z_min,x_max,x_min,y_max,y_min;
	GEOPROJECTION* pDem_Prj = m_dem_prj+m_nDemNum;
	float h_srtm_max,h_srtm_min;

	printf("-----%s-----\n",lpstrDemPath);
	z_max = pDem->GetMaxZ();	z_min = pDem->GetMinZ();
	printf("Zmax:%lf\tZmin:%lf\t\n",z_max,z_min);
	pDem->GetDemRgn(x,y);	printf("GeoRange:[%lf,%lf]\t[%lf,%lf]\t[%lf,%lf]\t[%lf,%lf]\t\n",x[0],y[0],x[1],y[1],x[2],y[2],x[3],y[3]);
	
	double h = z_min;
	x_max = Max(x,4);	y_max = Max(y,4);
	x_min = Min(x,4);	y_min = Min(y,4);
	if( x_max>180 || x_min<-180 || y_max>90 || y_min<-90 ) *pDem_Prj = GP_PRJ;else *pDem_Prj = GP_LONLAT;
	printf("Projection Type : %s\n",*pDem_Prj==GP_PRJ?"Projection":"BLH");
	
	if( *pDem_Prj==GP_PRJ )	
	{
		Cvt2BLH(x_max,y_max,h,&x_max,&y_max,&h,&m_geoCvt);	Cvt2BLH(x_min,y_min,h,&x_min,&y_min,&h,&m_geoCvt);
		if( x_max>180 || x_min<-180 || y_max>90 || y_min<-90 ) { printf("Warning:This file is not match to the gcd file!");	printf("-----Remove-----\n"); pDem->Close();	return false;}
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
	
	printf("-----Append-----\n");
	
	if ( m_xgsd_map<0 || m_ygsd_map<0){
		SPDEMHDR* pHdr = pDem->GetDemHeaderPr();
		m_xgsd_map = pHdr->intervalX;
		m_ygsd_map = pHdr->intervalY;
		
		if( m_xgsd_map<1 ) m_bGsdIsMeter = false;
	}
	m_nDemNum++;
	return true;
}

int CDemMap::Load4DemListFile(const char* lpstrListFile)
{
	FILE* fp = fopen(lpstrListFile,"r");	if(!fp) {	printf("Error:Can't Open %s!",lpstrListFile);	return false;	}
	
	char strLine[1024];	
	int nTotalNum = 0;
	fgets(strLine,1024,fp);	sscanf(strLine,"%d",&nTotalNum);
	if(nTotalNum==0) { fclose(fp);	printf("Please Enter the Total Number of DemFile to the first line of the DemListFile!"); return false;	}
			
	MallocDemBuf(nTotalNum);
	int i;
	for (i=0; i<nTotalNum; i++){
		fscanf(fp,"%s\n",strLine);	
		
		if( AppendDem(strLine) == true ) AppendDemPath(strLine);
	}
	fclose(fp);

	printf("Num of DEM=	%d\n",m_nDemNum);

	return (m_nDemNum>0)?true:false;
}

///////////////////////大区域问题
int	CDemMap::MergeDem(const char* lpstrMergePath, float xgsd, float ygsd, CWuSRTM* pLocSrtm)
{
	int i,j;
	printf("-----Merge Dem File-----\n");

	float* z = new float[m_nDemNum];	int numZ;

	int xmax,xmin,ymax,ymin;
	int nRows,nCols;	double lat,lon,x,y,z_tmp;	
	if(m_bGsdIsMeter) { xmax = m_xmax_meter; xmin = m_xmin_meter; ymax=m_ymax_meter;	ymin=m_ymin_meter;	}
	else { xmax = m_xmax_degree; xmin = m_xmin_degree; ymax=m_ymax_degree;	ymin=m_ymin_degree; }

	nRows = int( (ymax-ymin)/ygsd )+1;	nCols = int( (xmax-xmin)/xgsd )+1;
	
	printf("Rows=	%d;Cols=	%d\n",nRows,nCols);
	float* data = new float[nRows*nCols+128];	float* pZ = data;
	
	printf("Merge Dem ...  0%%");	int step_inter = nRows/10+1;	int step_cur = 1;
	y = ymin;	
	for (i=0; i<nRows; i++){
		x = xmin;
		for (j=0; j<nCols; j++){
			
			*pZ = NOVALUE;
			/*if (GetDemZValue(x, y, z, &numZ, m_bGsdIsMeter)) {
				short  srtmZ = -9999;
				if (pLocSrtm){
					if (m_bGsdIsMeter) { m_geoCvt.Cvt_Prj2LBH(x, y, 0, &lon, &lat, &z_tmp);	lon *= SPGC_R2D;	lat *= SPGC_R2D; }
					else { lon = x; lat = y; }
					srtmZ = pLocSrtm->GetZVal(lon, lat);
				}
				*pZ = GetZ(z, numZ, srtmZ);
			}			*/
			*pZ = mlwGetDemZValue(x, y);
			pZ++;
			x += xgsd;
			
		}
		y += ygsd;
		if( (i+1)>=(step_cur*step_inter) ) {	
			printf("\b\b\b\b%2d0%%",step_cur); step_cur+=1;	
		}
	}
	delete[] z;
	printf("\b\b\b\b100%%\n");
	
	printf("Saving merge_dem ...");
	
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
	
	if(!fail) printf("%s is Saved\n",strMergeDem);
	else {	printf("Fail to save %s!\n",strMergeDem); }
	printf("-----End Merge Dem File-----\n");
		
	return true;
}

inline bool CDemMap::GetDemZValue(double x,double y,float* z,int* numZ,bool bPrjCoor /* = true */)
{
	int idx,sz = 0;	float z_tmp=NOVALUE;
	double lat,lon;	double tz;
	if( bPrjCoor ) {	m_geoCvt.Cvt_Prj2LBH(x,y,0,&lon,&lat,&tz);	lon *= SPGC_R2D;	lat *= SPGC_R2D;	}
	else { lat = y;	lon = x;	m_geoCvt.Cvt_LBH2Prj(lon*SPGC_D2R,lat*SPGC_D2R,0,&x,&y,&tz);	 }

	for(idx=0; idx<m_nDemNum; idx++)
	{
		if( m_dem_prj[idx] == GP_PRJ ){
			double xRgn[4], yRgn[4];
			m_dem[idx].GetDemRgn(xRgn, yRgn);
			double  x_max, x_min, y_max, y_min;
			x_max = Max(xRgn, 4);	y_max = Max(yRgn, 4);
			x_min = Min(xRgn, 4);	y_min = Min(yRgn, 4);
			if (x >= x_min + X_GSD * 10 && x <= x_max - X_GSD * 10&&y>=y_min+Y_GSD*10&&y<=y_max-Y_GSD*10)
				z_tmp = m_dem[idx].GetDemZValue(x, y);
			else
				z_tmp = NOVALUE;
		}
		else if( m_dem_prj[idx] == GP_LONLAT	) { z_tmp = m_dem[idx].GetDemZValue(lon,lat);	}
		else continue;
		if( z_tmp == NOVALUE ) continue;
		
		if(z) z[sz] = z_tmp;	
		sz += 1;
	}

	if(numZ) *numZ = sz;

	if( sz == 0) return false; else return true;
}
int mlwnum = 0;
inline float CDemMap::mlwGetDemZValue(double x, double y)
{
	float z_tmp = NOVALUE, mlwz = NOVALUE; int num = 0;
	for (int i = 0; i < m_nDemNum; i++)
	{
		z_tmp=m_dem[i].GetDemZValue(x, y);
		if (z_tmp != NOVALUE){
			num++;
			mlwz = z_tmp;
		}
	}
	//double xOverlay[4] = { 0.0 }, yOverlay[4] = { 0.0 };
	if (num>1)
	{
		FILE *fp = fopen("D:\\H_Research\\0_ResearchTest\\12_DSM\\MergeDSM\\DiffResult.txt.txt", "a");
		fprintf(fp, "%f\t", m_dem[0].GetDemZValue(x, y) - m_dem[1].GetDemZValue(x, y)); mlwnum++;
		if (mlwnum > 16384){
			fprintf(fp, "\n"); mlwnum = 0;
		}
		fclose(fp);
		double xRgn1[4], yRgn1[4];
		m_dem[0].GetDemGeoRgn(xRgn1, yRgn1);
		double xRgn2[4], yRgn2[4];
		m_dem[1].GetDemGeoRgn(xRgn2, yRgn2);
		vector<PT> vPtA, vPtB, vPTcor;
		for (int i = 0; i < 4; i++)
		{
			PT pt;
			pt.x = xRgn1[i]; pt.y = yRgn1[i];
			vPtA.push_back(pt);
			pt.x = xRgn2[i]; pt.y = yRgn2[i];
			vPtB.push_back(pt);
		}
		IntersectionAB(vPtA, vPtB, vPTcor);
		DelRepeatPt(vPTcor);
		Segclockwise(vPTcor);
		/*double xRgn3[4], yRgn3[4];
		m_dem[0].GetDemGeoRgn(xRgn3, yRgn3);
		double xRgn4[4], yRgn4[4];
		m_dem[1].GetDemGeoRgn(xRgn4, yRgn4);
		double  x_max, x_min, y_max, y_min;
		int idx = 0;
		x_max = Max(xRgn3, 4);	y_max = Max(yRgn3, 4);
		x_min = Min(xRgn3, 4);	y_min = Min(yRgn3, 4);

		for (int i = 0; i < 4;i++)
		{
		if (xRgn2[i]<=x_max&&xRgn2[i]>=x_min&&yRgn2[i]<=y_max&&yRgn2[i]>=y_min)
		{
		xOverlay[idx] = xRgn2[i]; yOverlay[idx] = yRgn2[i];
		idx++;
		}
		}
		x_max = Max(xRgn4, 4);	y_max = Max(yRgn4, 4);
		x_min = Min(xRgn4, 4);	y_min = Min(yRgn4, 4);
		for (int i = 0; i < 4; i++)
		{
		if (xRgn1[i] <= x_max&&xRgn1[i] >= x_min&&yRgn1[i] <= y_max&&yRgn1[i] >= y_min)
		{
		xOverlay[idx] = xRgn1[i]; yOverlay[idx] = yRgn1[i];
		idx++;
		}
		}*/
		double x1 = 0.0, y1 = 0.0, x2 = 0.0, y2 = 0.0;
		/*
		if (DistaceAB(vPTcor[0], vPTcor[1])>DistaceAB(vPTcor[0], vPTcor[3]))
		{
		x1 = (xOverlay[0] + xOverlay[3]) / 2;
		y1 = (yOverlay[0] + yOverlay[3]) / 2;
		x2 = (xOverlay[1] + xOverlay[2]) / 2;
		y2 = (yOverlay[1] + yOverlay[2]) / 2;
		}
		else
		{
		x1 = (xOverlay[0] + xOverlay[1]) / 2;
		y1 = (yOverlay[0] + yOverlay[1]) / 2;
		x2 = (xOverlay[2] + xOverlay[3]) / 2;
		y2 = (yOverlay[2] + yOverlay[3]) / 2;
		}*/
		if (DistaceAB(vPTcor[0], vPTcor[1]) > DistaceAB(vPTcor[0], vPTcor[3]))
		{
			x1 = (vPTcor[0].x + vPTcor[3].x) / 2;
			y1 = (vPTcor[0].y + vPTcor[3].y) / 2;
			x2 = (vPTcor[1].x + vPTcor[2].x) / 2;
			y2 = (vPTcor[1].y + vPTcor[2].y) / 2;
		}
		else
		{
			x1 = (vPTcor[0].x + vPTcor[1].x) / 2;
			y1 = (vPTcor[0].y + vPTcor[1].y) / 2;
			x2 = (vPTcor[3].x + vPTcor[2].x) / 2;
			y2 = (vPTcor[3].y + vPTcor[2].y) / 2;
		}
		double a, b;
		a = (x2 - x1) / (y1 - y2); b = -x1 - a*y1;
		double s = x + a*y + b;
		if (s < -0.5) z_tmp = m_dem[0].GetDemZValue(x, y);
		else	if (s > 0.5) z_tmp = m_dem[1].GetDemZValue(x, y);
		else z_tmp = (m_dem[0].GetDemZValue(x, y) + m_dem[1].GetDemZValue(x, y)) / 2;
	}
	else z_tmp = mlwz;
	
	return z_tmp;
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