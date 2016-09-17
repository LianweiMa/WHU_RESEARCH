// WuEgm.h : main header file for the WUEGM DLL
/*----------------------------------------------------------------------+
|		WuEgm														    |
|       Author:     DuanYanSong  2013/08/01   		                    |
|            Ver 1.0													|
|       Copyright (c)2013, WHU RSGIS DPGrid Group                       |
|	         All rights reserved.                                       |
|		ysduan@whu.edu.cn; ysduan@sohu.com              				|
+----------------------------------------------------------------------*/
#ifndef WUEGM_H_DUANYANSONG_2013_08_01_05_52_27039
#define WUEGM_H_DUANYANSONG_2013_08_01_05_52_27039

#ifndef WUEGM_LIB

#define WUEGM_LIB  __declspec(dllimport)
#ifdef _DEBUG_WUEGM
#pragma comment(lib,"WuEgmD.lib") 
#pragma message("Automatically linking with WuEgmD.lib") 
#else
#pragma comment(lib,"WuEgm.lib") 
#pragma message("Automatically linking with WuEgm.lib") 
#endif

#else

#endif

class WUEGM_LIB CWuEgm
{
public:
    CWuEgm(){ m_pEgmGrd=NULL; m_zoom=1; };
    virtual ~CWuEgm(){ if (m_pEgmGrd) delete []m_pEgmGrd; };

    // Get Globe's Gravity by Lon and Lat position ,Lon and Lat unit is degree, range is : ( 0 ~ 360, -90 ~ 90 ). 
    float GetGVal(double lon,double lat){
        if ( m_pEgmGrd==NULL ) Init();
        if ( lon<0 ) lon += 360; // just for -180 <-> +180 .
        if ( lon>=360 ) lon -= 360;
        double x = (lon - m_sLon)/m_dLon;
        double y = (lat - m_sLat)/m_dLat;
        
        int lbGridx = int(x);double	dx = (x - lbGridx);
        int lbGridy = int(y);double	dy = (y - lbGridy);
        if( lbGridx < 0 || lbGridx > m_cols-1 || 
            lbGridy < 0 || lbGridy > m_rows-1 || m_pEgmGrd==NULL ) return( 0 ); 
        int lbOffset = lbGridy * m_cols + lbGridx;
        int ltOffset = lbOffset + m_cols;

        if ( lbGridx==m_cols-1||lbGridy==m_rows-1 ) return m_pEgmGrd[lbOffset];
        double z00 = m_pEgmGrd[lbOffset  ]/m_zoom;
        double z01 = m_pEgmGrd[lbOffset+1]/m_zoom;
        double z10 = m_pEgmGrd[ltOffset  ]/m_zoom;
        double z11 = m_pEgmGrd[ltOffset+1]/m_zoom;
        z00 += dx*(z01-z00);
        z10 += dx*(z11-z10);
        return (float)( z00 + dy*(z10 - z00) );
    };
    BOOL  InitExtDat(LPCSTR lpstrPathName );

protected:
    BOOL    Init();
    short   *m_pEgmGrd;
    short   m_cols,m_rows;
    float   m_zoom;
    double  m_sLon,m_sLat,m_dLon,m_dLat;
};

#endif
