// WuGlbHei.h : main header file for the WUGLBHEI DLL
/*----------------------------------------------------------------------+
|		WuGlbHei											 		    |
|       Author:     DuanYanSong  2013/04/08								|
|            Ver 1.0													|
|       Copyright (c) 2013, Supresoft Corporation                       |
|	         All rights reserved.                                       |
|       http://www.supresoft.com.cn                                     |
|		ysduan@supresoft.com.cn;ysduan@sohu.com                         |
+----------------------------------------------------------------------*/
#ifndef WUGLBHEI_H_DUANYANSONG_2013_04_08_08_47_58593
#define WUGLBHEI_H_DUANYANSONG_2013_04_08_08_47_58593

#ifndef WUGLBHEI_LIB

#define WUGLBHEI_LIB  __declspec(dllimport)
#ifdef _DEBUG_WUGLBHEI
#pragma comment(lib,"WuGlbHeiD.lib") 
#pragma message("Automatically linking with WuGlbHeiD.lib") 
#else
#pragma comment(lib,"WuGlbHei.lib") 
#pragma message("Automatically linking with WuGlbHei.lib") 
#endif

#endif

#define SRTM_GSD     (3.0/3600.0)   
#define HRDEM_GSD    (1.0/3600.0)   

/*
Usage:
1. define objects for CWuGlbHei and CWuSRTM
2. call GetSRTM
3. attach data to CWuSRTM
4. get any value .
example
    CWuGlbHei glbSrtm; CWuSRTM locSrtm;
    float *pLocZ = new float[ 3600*3600 ];
    glbSrtm.GetSRTM( 112,38,3600,3600,pLocZ );
    locSrtm.Init( 112,38,3600,3600,pLocZ );
    delete pLocZ;

    // Get Value    
    locSrtm.GetZVal( 112.4,38.7 );
    locSrtm.GetZVal( 113.5,39.2 );
    locSrtm.GetZVal( 114.9,40.9 );
    // ....
*/

class WUGLBHEI_LIB CWuGlbHei
{
public:
    CWuGlbHei(){ m_pHrgGrd=NULL; m_pZ=NULL; };
    virtual ~CWuGlbHei(){ ResetHeightRange(); UnLockHrDEM(); };

    // Get Globe's Hei by Lon and Lat position ,Lon and Lat unit is degree, range is : ( -180 ~ +180, -90 ~ 90 ). 
    BOOL GetHeightRange(double lon, double lat, float* minHgt, float* maxHgt);
    BOOL GetHeightRange(double minLon, double minLat, double maxLon, double maxLat, float* minHgt, float* maxHgt);
    void ResetHeightRange();
    
    // Get SRTM Hei value ,by lon0,lato with cols,rows. 
    // The srtm's interval is 3 secends. No data area be full to 0.
    // the grid range is : dlon = cols*(3 secends), dlat=rows*(3 secends)
    // So the lon0 and lat0 must be times of 3 secends.
    // Lon and Lat unit is degree, range is : ( -180 ~ +180, -90 ~ 90 ). 
    // the lpstrSrtmFile is a WuSRTM.dat file , user can set the file pathname ,if not be set ,
    // the default filepathname is used , default pathname will be get from  WuGlbHei.ini .
    BOOL GetSRTM( double lon0,double lat0,int cols,int rows,float *pZ,LPCSTR lpstrSrtmFile=NULL );

    // Load the best DEM in memory for Using by calss CWuSRTM.
    // Lon and Lat unit is degree, range is : ( -180 ~ +180, -90 ~ 90 ).
    // The interval will be the best resolution in the lpstrDemFile or in SRTM dat
    const short*  LockHrDEM( double minLon,double minLat,double maxLon,double maxLat,
                             double *lon0,double *lat0,double *dlon,double *dlat,int *cols,int *rows, 
                             BOOL fillBySrtm=TRUE,LPCSTR lpstrHrdFile=NULL );
    void  UnLockHrDEM();


protected:
    BOOL    InitHeightRange();
    BOOL    UpdateSrtm(double lon0,double lat0,double dlon,double dlat,int cols,int rows,LPCSTR lpstrHrdFile=NULL);
    BOOL    UpdateHrDem(double lon0,double lat0,double dlon,double dlat,int cols,int rows,LPCSTR lpstrHrdFile=NULL);

    BYTE*   m_pHrgGrd;
    double  m_sLon,m_sLat,m_dLon,m_dLat;
    int     m_cols,m_rows;
    short*  m_pZ;
};

class CWuSRTM
{
public:
    CWuSRTM(){ m_pZ = NULL; m_sLon=m_sLat=0;m_cols=m_rows=0;m_dLon=m_dLat=SRTM_GSD;};
    ~CWuSRTM(){ if(m_pZ) delete []m_pZ; m_pZ = NULL; };
    
    double m_sLon,m_sLat;
    double m_dLon,m_dLat;
    int    m_cols,m_rows;
    short  *m_pZ;

    short* Init( double lon0,double lat0,int cols,int rows,double dlon=SRTM_GSD,double dlat=SRTM_GSD ){
            m_sLon=lon0; m_sLat=lat0;  m_cols=cols; m_rows=rows; m_dLon=dlon; m_dLat=dlat;
            if (m_pZ) delete []m_pZ; m_pZ = new short[m_cols*m_rows]; SetZero(); return m_pZ;
        };
    void  Init( double lon0,double lat0,int cols,int rows,float *pZ,double dlon=SRTM_GSD,double dlat=SRTM_GSD ){
            int i,sz = cols*rows; 
            short *pS = Init( lon0,lat0,cols,rows,dlon,dlat );
            if (pS){ for( i=0;i<sz;i++ ) *pS++ = short( *pZ++ ); }
        };
    void SetZero(){ memset(m_pZ,0,sizeof(short)*m_cols*m_rows); };
    void Attach( double lon0,double lat0,int cols,int rows,float *pZ,double dlon=SRTM_GSD,double dlat=SRTM_GSD){
            Init( lon0,lat0,cols,rows,pZ,dlon,dlat ); };
    void Attach( double lon0,double lat0,int cols,int rows,short *pZ,double dlon=SRTM_GSD,double dlat=SRTM_GSD){
            m_sLon=lon0; m_sLat=lat0; m_cols=cols; m_rows=rows; m_pZ = pZ; m_dLon=dlon; m_dLat=dlat; };
    short GetZVal( int col,int row ){ if(col<0||col>m_cols||row<0||row>m_rows) return -9999; return *(m_pZ+row*m_cols+col); };
    short GetZVal( double lon,double lat ){
            double x = (lon - m_sLon)/m_dLon;
			double y = (lat - m_sLat)/m_dLat;

			int lbGridx = int(x);double	dx = (x - lbGridx);
			int lbGridy = int(y);double	dy = (y - lbGridy);
			if( lbGridx < 0 || lbGridx >= m_cols-1  || 
				lbGridy < 0 || lbGridy >= m_rows-1 ) return( -9999 ); 
			
			int lbOffset = lbGridy * m_cols + lbGridx;
			int ltOffset = lbOffset + m_cols;
			
			double z00 = m_pZ[lbOffset  ];
			double z01 = m_pZ[lbOffset+1];
			double z10 = m_pZ[ltOffset  ];
			double z11 = m_pZ[ltOffset+1];
			z00 += dx*(z01-z00);
			z10 += dx*(z11-z10);
			return (short)( z00 + dy*(z10 - z00) );
        };
    BOOL Save2File(LPCSTR lpstrDem){
        int i,sz = m_cols*m_rows;  short *pS=m_pZ;
        float *pD,*pZ = new float[ sz ];
        for( pD=pZ,i=0;i<sz;i++,pD++,pS++) *pD = *pS; 
        struct SPDEMHDR
        {
            double	startX;
            double	startY;   
            double	kapa;
            double	intervalX;
            double	intervalY;
            int		column;
            int		row;	
        }demHdr; char str[512]; DWORD rw;
        demHdr.startX = m_sLon; demHdr.startY = m_sLat; demHdr.kapa = 0;
        demHdr.intervalX = m_dLon; demHdr.intervalY = m_dLat;
        demHdr.column = m_cols; demHdr.row = m_rows;
        sprintf( str,"DEMBIN VER:1.0 Supresoft Inc. Format: Tag(512Bytes),minX(double),minY(double),Kap(double),Dx(double),Dy(double),Cols(int),Rows(int),Z(float)... %.3lf %.3lf %.3lf %.12f %.12f %d %d",
                demHdr.startX,demHdr.startY,demHdr.kapa,demHdr.intervalX,demHdr.intervalY,demHdr.column,demHdr.row );
        HANDLE hFile = ::CreateFile( lpstrDem,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL ); 
        if ( !hFile || hFile==INVALID_HANDLE_VALUE ){ delete [] pZ; return FALSE; }
        ::WriteFile( hFile,str,512,&rw,NULL ); 
        ::WriteFile( hFile,&demHdr,sizeof(demHdr),&rw,NULL );                                        
        ::WriteFile( hFile,pZ,sz*sizeof(float),&rw,NULL );
        ::CloseHandle( hFile );    
        delete [] pZ;
        return TRUE;
    };
};


#include "math.h"
inline BOOL GetZValue(float *hei,double lon,double lat){
    double lon0 = floor(lon/SRTM_GSD)*SRTM_GSD -SRTM_GSD*16;
    double lat0 = floor(lat/SRTM_GSD)*SRTM_GSD -SRTM_GSD*16;
    float pZ[256]; memset( pZ,0,sizeof(pZ) ); CWuGlbHei glbHei; int i;
    if ( !glbHei.GetSRTM( lon0,lat0,16,16,pZ ) ) return FALSE; 
    for( *hei=0,i=0;i<256;i++ ) *hei += pZ[i]/256; 
    return TRUE;
}

inline BOOL GetSRTM( double minLon,double minLat,double maxLon,double maxLat,LPCSTR lpstrDem )
{
    CWuGlbHei glbSrtm; int cols,rows; float *pZ=NULL;
    minLon = int(minLon/SRTM_GSD)*SRTM_GSD;
    minLat = int(minLat/SRTM_GSD)*SRTM_GSD;
    cols = int( (maxLon-minLon)/SRTM_GSD +1);
    rows = int( (maxLat-minLat)/SRTM_GSD +1);
    pZ = new float[ cols*rows ];
    memset( pZ,0,sizeof(float)*cols*rows );
    glbSrtm.GetSRTM( minLon,minLat,cols,rows,pZ );
    struct SPDEMHDR
    {
        double	startX;
        double	startY;   
        double	kapa;
        double	intervalX;
        double	intervalY;
        int		column;
        int		row;	
    }demHdr; char str[512]; DWORD rw;
    demHdr.startX = minLon; demHdr.startY = minLat; demHdr.kapa = 0;
    demHdr.intervalX = SRTM_GSD; demHdr.intervalY = SRTM_GSD;
    demHdr.column = cols; demHdr.row = rows;
    sprintf( str,"DEMBIN VER:1.0 Supresoft Inc. Format: Tag(512Bytes),minX(double),minY(double),Kap(double),Dx(double),Dy(double),Cols(int),Rows(int),Z(float)... %.3lf %.3lf %.3lf %.12f %.12f %d %d",
    demHdr.startX,demHdr.startY,demHdr.kapa,demHdr.intervalX,demHdr.intervalY,demHdr.column,demHdr.row );
    HANDLE hFile = ::CreateFile( lpstrDem,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL ); 
    if ( !hFile || hFile==INVALID_HANDLE_VALUE ){ delete [] pZ; return FALSE; }
    ::WriteFile( hFile,str,512,&rw,NULL ); 
    ::WriteFile( hFile,&demHdr,sizeof(demHdr),&rw,NULL );                                        
    ::WriteFile( hFile,pZ,cols*rows*sizeof(float),&rw,NULL );
    ::CloseHandle( hFile );    
    delete [] pZ;

	return TRUE;
}

#endif
