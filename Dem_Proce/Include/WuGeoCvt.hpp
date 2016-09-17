// WuGeoCvt.hpp
/*----------------------------------------------------------------------+
|		WuGeoCvt											 		    |
|       Author:     DuanYanSong  2013/12/03								|
|            Ver 1.0													|
|       Copyright (c) 2013, Supresoft Corporation                       |
|	         All rights reserved.                                       |
|       http://www.supresoft.com.cn                                     |
|		ysduan@supresoft.com.cn;ysduan@sohu.com                         |
+----------------------------------------------------------------------*/

//////////////////////////////////////////////////////////////////////////
//
// modified by KYF - 2013 - 12 - 23
// 增加不做高程异常改正的 Loc_To_WGS84LBH 和 WGS84LBH_To_Loc
//
//////////////////////////////////////////////////////////////////////////

#ifndef WUGEOCVT_H_DUANYANSONG_2013_12_03_16_45_432897329804798326149732
#define WUGEOCVT_H_DUANYANSONG_2013_12_03_16_45_432897329804798326149732

#include "SpGeoCvt.h"
#include "WuEgm.h"

class CWuGeoCvt 
{ 
public:
    CWuGeoCvt(){ m_bEgm96=FALSE; };
    virtual ~CWuGeoCvt(){   };

	virtual CWuGeoCvt*	Clone(){ CWuGeoCvt *p = new CWuGeoCvt; p->Copy(this); return p; };
	virtual void	    Copy( CWuGeoCvt* p){ memcpy( this,p,sizeof(CWuGeoCvt) ); };
	
    bool	Init(LPCSTR lpstrLocGCD,LPCSTR lpstr84GCD,LPCSTR lpstrPar7,LPCSTR lpstrAlt=NULL,BOOL bEgm96=FALSE )
	{
        if ( !m_geoCvt.Load4File(lpstrLocGCD) ) return false;
        if ( !m_wgs84Cvt.Load4File(lpstr84GCD) ) return false;
        if ( !m_par7Cvt.Load4File(lpstrPar7) ) return false;
        if ( lpstrAlt && IsExist(lpstrAlt) ) m_altCvt.Load4File(lpstrAlt);
        m_bEgm96 = bEgm96;
        return true;
    }

	bool    Loc_To_WGS84LBH( double *gx,double *gy,double *gz )
	{
        m_geoCvt.Cvt_Prj2LBH( *gx,*gy,*gz,gx,gy,gz );
        if ( m_wgs84Cvt.m_ElpsType != m_geoCvt.m_ElpsType )
		{
            m_geoCvt.Cvt_LBH2CXYZ( *gx,*gy,*gz,gx,gy,gz );
            m_par7Cvt.Cvt_LCXYZ2WCXYZ( *gx,*gy,*gz,gx,gy,gz );
            m_wgs84Cvt.Cvt_CXYZ2LBH( *gx,*gy,*gz,gx,gy,gz );
        }

		return TRUE;
    };
    bool    WGS84LBH_To_Loc( double *gx,double *gy,double *gz )
	{
        if ( m_wgs84Cvt.m_ElpsType!= m_geoCvt.m_ElpsType )
		{
            m_wgs84Cvt.Cvt_LBH2CXYZ( *gx,*gy,*gz,gx,gy,gz ); 
            m_par7Cvt.Cvt_WCXYZ2LCXYZ( *gx,*gy,*gz,gx,gy,gz );
            m_geoCvt.Cvt_CXYZ2LBH( *gx,*gy,*gz,gx,gy,gz );
        }
        m_geoCvt.Cvt_LBH2Prj( *gx,*gy,*gz,gx,gy,gz ); 

		return TRUE;
    };
	
    bool    LocToWGS84_bak( double *gx,double *gy,double *gz )
	{
        m_geoCvt.Cvt_Prj2LBH( *gx,*gy,*gz,gx,gy,gz );
        if ( m_wgs84Cvt.m_ElpsType!= m_geoCvt.m_ElpsType )
		{
            double gx0=*gx,gy0=*gy,gz0=*gz,dz;
            m_geoCvt.Cvt_LBH2CXYZ( *gx,*gy,*gz,gx,gy,gz );
            m_par7Cvt.Cvt_LCXYZ2WCXYZ( *gx,*gy,*gz,gx,gy,gz );
            m_wgs84Cvt.Cvt_CXYZ2LBH( *gx,*gy,*gz,gx,gy,gz );

            dz = m_bEgm96?m_egm96.GetGVal( *gx*SPGC_R2D,*gy*SPGC_R2D ):m_altCvt.Get_Alt2Geo( *gx*SPGC_R2D,*gy*SPGC_R2D );
            if ( (dz*dz)>0.000001 )
			{
                *gx = gx0; *gy = gy0; *gz = gz0 + dz;
                m_geoCvt.Cvt_LBH2CXYZ( *gx,*gy,*gz,gx,gy,gz );
                m_par7Cvt.Cvt_LCXYZ2WCXYZ( *gx,*gy,*gz,gx,gy,gz );
                m_wgs84Cvt.Cvt_CXYZ2LBH( *gx,*gy,*gz,gx,gy,gz );
            }
        }
		else
//          *gz = *gz + m_bEgm96?m_egm96.GetGVal( *gx*SPGC_R2D,*gy*SPGC_R2D ):m_altCvt.Get_Alt2Geo( *gx*SPGC_R2D,*gy*SPGC_R2D );
            *gz = *gz + ( m_bEgm96?m_egm96.GetGVal( *gx*SPGC_R2D,*gy*SPGC_R2D ):m_altCvt.Get_Alt2Geo( *gx*SPGC_R2D,*gy*SPGC_R2D ) );

		return TRUE;
    };
    bool    WGS84ToLoc_bak( double *gx,double *gy,double *gz )
	{
        float dz = m_bEgm96?m_egm96.GetGVal( *gx*SPGC_R2D,*gy*SPGC_R2D ):m_altCvt.Get_Alt2Geo( *gx*SPGC_R2D,*gy*SPGC_R2D );
        if ( m_wgs84Cvt.m_ElpsType!= m_geoCvt.m_ElpsType )
		{
            m_wgs84Cvt.Cvt_LBH2CXYZ( *gx,*gy,*gz,gx,gy,gz ); 
            m_par7Cvt.Cvt_WCXYZ2LCXYZ( *gx,*gy,*gz,gx,gy,gz );
            m_geoCvt.Cvt_CXYZ2LBH( *gx,*gy,*gz,gx,gy,gz );
        }
        m_geoCvt.Cvt_LBH2Prj( *gx,*gy,*gz,gx,gy,gz ); 
        *gz = *gz - dz;

		return TRUE;
    };
    BOOL            m_bEgm96;
    CWuEgm          m_egm96;
    CSpPar7Cvt 	    m_par7Cvt;
    CSpAltCvt       m_altCvt;
    class CSpGeoCvtFrd: public CSpGeoCvt{ friend class CWuGeoCvt; }m_wgs84Cvt,m_geoCvt;
    
protected:

private:
    inline static BOOL  IsExist(LPCSTR lpstrPathName){
        WIN32_FIND_DATA fd; HANDLE hFind=INVALID_HANDLE_VALUE;
        hFind = ::FindFirstFile(lpstrPathName,&fd);
        if ( hFind==INVALID_HANDLE_VALUE ) return FALSE;
        ::FindClose(hFind); return TRUE;
    };
};


#endif
