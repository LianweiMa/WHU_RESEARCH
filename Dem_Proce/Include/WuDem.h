// WuDem.h : main header file for the WUDEM DLL
/*----------------------------------------------------------------------+
|		WuDem														    |
|       Author:     DuanYanSong  2013/09/15   		                    |
|            Ver 1.0													|
|       Copyright (c)2013, WHU RSGIS DPGrid Group                       |
|	         All rights reserved.                                       |
|		ysduan@whu.edu.cn; ysduan@sohu.com              				|
+----------------------------------------------------------------------*/
#ifndef WUDEM_H_DUANYANSONG_2013_09_15_15_08_26116
#define WUDEM_H_DUANYANSONG_2013_09_15_15_08_26116


#ifndef WUDEM_LIB

#define WUDEM_LIB  __declspec(dllimport)
#ifdef _DEBUG_WUDEM
#pragma comment(lib,"WuDemD.lib") 
#pragma message("Automatically linking with WuDemD.lib") 
#else
#pragma comment(lib,"WuDem.lib") 
#pragma message("Automatically linking with WuDem.lib") 
#endif

#else

#endif


#define		NOVALUE				-99999
enum  VERSION
{	
        VER_31	=	1,
        VER_32	=	2,
        VER_BN  =   3,
        VER_TIF	=	4,
        VER_BS  =   5, // bin format and z value is short .
        VER_BC  =   6, // bin format and z value is char .
};

#ifndef _SPDEMHDR
#define _SPDEMHDR
typedef struct tagSPDEMHDR
{
    double	startX;
    double	startY;   
    double	kapa;
    double	intervalX;
    double	intervalY;
    int		column;
    int		row;	
}SPDEMHDR,WUDEMHDR,*LPSPDEMHDR,*LPWUDEMHDR;
#else
#pragma message("WuDem.h, Warning: SPDEMHDR alread define, be sure it was define as: struct tagSPDEMHDR{ double startX,startY,kapa;double intervalX,intervalY;int column;int row; }. \
               \nWuDem.h, 警告:类型 SPDEMHDR 已经定义过,请确保其定义为: struct tagSPDEMHDR{ double startX,startY,kapa;double intervalX,intervalY;int column;int row; }. ") 
#endif

#define FMT_DEM     "VZDEM"
#define FMT_FDEM    "VZDEMF"
#define FMT_BEM     "VZBEM"
#define FMT_SBEM    "VZBEMS"
#define FMT_BIL     "BIL"
#define FMT_CSDTF   "CSDTF"
#define FMT_NSDTF   "NSDTF"
#define FMT_GRID    "ArcGrid"
#define FMT_GJBDEM  "GEM"
#define FMT_TIF     "TIF"

#include "math.h"


class WUDEM_LIB CWuDem
{
public:
    enum OUTMSG{
        PROG_MSG   =   10,
        PROG_START =   11,
        PROG_STEP  =   12,
        PROG_OVER  =   13,
    };	
    enum OPENFLAGS { modeRead= 0x0000,modeWrite=0x0001,modeReadWrite=0x0002,modeCreate=0x1000 };
public:
    CWuDem();
    virtual ~CWuDem();

    void	                    SetDemHeader( SPDEMHDR demHdr   );
    void	                    Reset();
    
    virtual bool				Load4File( LPCSTR lpstrPathName,BOOL bMsgBox=TRUE );
    virtual bool				Save2File( LPCSTR lpstrPathName,VERSION ver=VER_32,BOOL bMsgBox=TRUE );
    virtual SPDEMHDR            GetDemHdr(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE,char *strFmt=NULL);
    virtual bool	            GetDemRgn(LPCSTR lpstrPathName,double x[],double y[],LPSPDEMHDR pDemHdr,BOOL bMsgBox=TRUE );

    ///////////////////////////////////////////////////////////////////////////
    //  Following is for File Access
    ///////////////////////////////////////////////////////////////////////////
    virtual BOOL                Open(LPCSTR lpstrPathName,int mode,SPDEMHDR *pHdr=NULL);
    virtual void                Close();
    virtual BOOL                ReadRow(float *pBuf,int row);
    virtual BOOL                WriteRow(float *pBuf,int row);
    virtual BOOL                ReadBlock(float *pBuf,int sRow,int sCol,int rows,int cols);
    virtual BOOL                WriteBlock(float *pBuf,int sRow,int sCol,int rows,int cols);
    ///////////////////////////////////////////////////////////////////////////
    //File Access Over
    ///////////////////////////////////////////////////////////////////////////
    
    virtual bool                CovertLargDem( LPCSTR lpstrPathName,LPCSTR lpstrFmt,LPCSTR lpstrPathNameBEM );
    virtual bool                Covert2BEM( LPCSTR lpstrPathName,LPCSTR lpstrPathNameBEM,BOOL bMsgBox=TRUE );
    virtual bool                SaveAsNSDTF(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE);
    virtual bool				SaveAsCSDTF(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE);
    virtual bool				SaveAsArcGRID(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE);
    virtual bool				SaveAsBIL(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE);;
    virtual bool				SaveAsGJB(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE);
    virtual bool				Load4GTIF(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE,BOOL bJustHdr=FALSE);
    virtual bool				SaveAsGTIF(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE);

public:
	inline  void	Attach( SPDEMHDR demHdr,float *pGrid,bool reset=true  ){
						m_demHdr = demHdr; m_pGrid = pGrid; 
                        m_cosKap = cos(m_demHdr.kapa); m_sinKap = sin(m_demHdr.kapa);
                        if (reset){
                            float minZ=99999999.f,maxZ=-99999999.f;
                            for ( int i=0;i<m_demHdr.column*m_demHdr.row;i++ ){
                                if ( *(m_pGrid+i)<-9999.8 ) *(m_pGrid+i) =  NOVALUE;
                                else{ if ( minZ>*(m_pGrid+i) ) minZ=*(m_pGrid+i); if ( maxZ<*(m_pGrid+i) ) maxZ=*(m_pGrid+i); }
                            }
							m_midZ = (minZ+maxZ)/2;
							m_minZ = minZ; m_maxZ = maxZ;
                        }
					};
	inline  void	Detach(){ m_pGrid=NULL;memset( &m_demHdr,0,sizeof(m_demHdr) ); };
	inline	BOOL	IsLoaded(){ return m_pGrid&&m_demHdr.column&&m_demHdr.row; };
	inline  void	InitDem( SPDEMHDR demHdr,float *pGrid ){	
						SetDemHeader( demHdr );
						memcpy( m_pGrid,pGrid,sizeof(float)*m_demHdr.column*m_demHdr.row );
						float minZ=99999999.f,maxZ=-99999999.f;double midZ=0,sum=1; 
						for ( int i=0;i<m_demHdr.column*m_demHdr.row;i++ ){
							if ( *(m_pGrid+i)<-9999.8 ) *(m_pGrid+i) =  NOVALUE;
							else{ sum+=1; midZ += *(m_pGrid+i); if ( minZ>*(m_pGrid+i) ) minZ=*(m_pGrid+i); if ( maxZ<*(m_pGrid+i) ) maxZ=*(m_pGrid+i); }
						}
						if ( sum>1 ) sum -= 1;
						m_midZ = (float)(midZ/sum);
						m_minZ = minZ; m_maxZ = maxZ;
					};
	inline  void	SetDemHeader( LPSPDEMHDR demHdr ){ SetDemHeader(*demHdr); };	
	inline  float	SetDemZValue(int column,int row,float  * DemZValue){ return SetDemZValue(column,row,*DemZValue);			};
	inline  float	SetDemZValue(int column,int row,double * DemZValue){ return SetDemZValue(column,row,float(*DemZValue) );	};
	inline  float	SetDemZValue(int column,int row,float z){ 
						if ( column>=m_demHdr.column || column<0 || row>=m_demHdr.row || row<0 ) return NOVALUE;
						m_pGrid[ row*m_demHdr.column + column ] = z; 
                        return z;
					};
    inline void     CutOutEdge(int edgeGrd){
                        int c,nCols = m_demHdr.column/edgeGrd;
                        int r,nRows = m_demHdr.row/edgeGrd;
                        BYTE *pM,*pMsk = new BYTE[nCols*nRows *2 ];
                        memset( pMsk,0,sizeof(BYTE)*nCols*nRows*2 );
                        for( pM=pMsk,r=0;r<nRows;r++ ){
                            for( c=0;c<nCols;c++,pM++ ){
                                *pM = m_pGrid[ (r*edgeGrd)*m_demHdr.column + (c*edgeGrd) ]<-999?255:0;
                            }
                        }
                        for( pM=pMsk+nCols*nRows,r=0;r<nRows;r++ ){
                            for( c=0;c<nCols;c++,pM++ ){
                                if ( r==0 || c==0 || r==nRows-1 || c==nCols-1 ) *pM = 255;
                                else{
                                    if ( pMsk[ (r-1)*nCols+c ] ||
                                        pMsk[ (r+1)*nCols+c ] ||
                                        pMsk[ r*nCols+(c-1) ] ||
                                        pMsk[ r*nCols+(c+1) ] ||                                         
                                        pMsk[ (r-1)*nCols+(c-1) ] ||
                                        pMsk[ (r-1)*nCols+(c+1) ] ||
                                        pMsk[ (r+1)*nCols+(c-1) ] ||
                                        pMsk[ (r+1)*nCols+(c+1) ] ) *pM = 255;
                                }
                            }
                        }
                        pM=pMsk+nCols*nRows; float *pG = m_pGrid;
                        for( r=0;r<m_demHdr.row;r++ ){
                            for( c=0;c<m_demHdr.column;c++,pG++ ){
                                if ( pM[ (r/edgeGrd)*nCols+(c/edgeGrd) ] ) *pG = NOVALUE;
                            }
                        }
                        delete pMsk;
                    }
	inline	int		GetCols(){ return m_demHdr.column; }
	inline  int		GetRows(){ return m_demHdr.row;	   }

    inline  float   GetMidZ(){ return m_midZ; };    
    inline  float   GetMinZ(){ return m_minZ; };
    inline  float   GetMaxZ(){ return m_maxZ; };
	
	inline	const   LPSPDEMHDR	GetDemHeaderPr(){ return &m_demHdr; };
	inline			SPDEMHDR&	GetDemHeader()	{ return m_demHdr;  };
	inline			void		GetDemHeader( LPSPDEMHDR pHdr){ *pHdr=GetDemHeader(); };

	inline  const   float*		GetZGridBuf()	{ return m_pGrid;	};
	inline			void		GetDemZValue( int column,int row,float* DemZValue ){ *DemZValue=GetDemZVal(column,row); };
	inline			void		GetDemZValue( int column,int row,double* DemZValue){ *DemZValue=GetDemZVal(column,row); };
    inline  		float		GetDemZValue(double x,double y,bool bBL = true){
                                    double dx = (x - m_demHdr.startX);
                                    double dy = (y - m_demHdr.startY);
        
                                    x = ( dx * m_cosKap + dy *  m_sinKap ) / m_demHdr.intervalX ;
                                    y = (-dx * m_sinKap + dy *  m_cosKap ) / m_demHdr.intervalY ;
        
                                    if ( !bBL ){
                                        int lbGridx = int(x+0.5), lbGridy = int(y+0.5);
                                        if( lbGridx < 0 || lbGridx > m_demHdr.column-1  || 
                                            lbGridy < 0 || lbGridy > m_demHdr.row   -1 ) return( NOVALUE ); 
                                        else{
                                            float tz = m_pGrid[ lbGridy * m_demHdr.column + lbGridx ];
                                            // search in near(3*3) grid's
                                            if ( tz==NOVALUE ){
                                                if ( lbGridx<1 ) lbGridx = 1;
                                                if ( lbGridy<1 ) lbGridy = 1;
                                                if ( lbGridx>m_demHdr.column-2 ) lbGridx=m_demHdr.column-2;
                                                if ( lbGridy>m_demHdr.row   -2 ) lbGridy=m_demHdr.row   -2;
                                                if ( tz==NOVALUE ) tz = m_pGrid[ (lbGridy+1) * m_demHdr.column + (lbGridx) ];
                                                if ( tz==NOVALUE ) tz = m_pGrid[ (lbGridy-1) * m_demHdr.column + (lbGridx) ];
                                                if ( tz==NOVALUE ) tz = m_pGrid[ (lbGridy) * m_demHdr.column + (lbGridx+1) ];
                                                if ( tz==NOVALUE ) tz = m_pGrid[ (lbGridy) * m_demHdr.column + (lbGridx-1) ];
                                                if ( tz==NOVALUE ) tz = m_pGrid[ (lbGridy+1) * m_demHdr.column + (lbGridx+1) ];
                                                if ( tz==NOVALUE ) tz = m_pGrid[ (lbGridy-1) * m_demHdr.column + (lbGridx+1) ];
                                                if ( tz==NOVALUE ) tz = m_pGrid[ (lbGridy+1) * m_demHdr.column + (lbGridx-1) ];
                                                if ( tz==NOVALUE ) tz = m_pGrid[ (lbGridy-1) * m_demHdr.column + (lbGridx-1) ];                                                
                                            }
                                            return tz;
                                        }
                                    }        
                                    int lbGridx = int(x);	dx = (x - lbGridx);
                                    int lbGridy = int(y);	dy = (y - lbGridy);
                                    if( lbGridx < 0 || lbGridx >= m_demHdr.column-1  || 
                                        lbGridy < 0 || lbGridy >= m_demHdr.row   -1 ) return( NOVALUE ); 
        
                                    int lbOffset = lbGridy * m_demHdr.column + lbGridx;
                                    int ltOffset = lbOffset + m_demHdr.column;        
                                    double z00 = m_pGrid[lbOffset];
                                    double z01 = m_pGrid[lbOffset+1];
                                    double z10 = m_pGrid[ltOffset];
                                    double z11 = m_pGrid[ltOffset+1];        
                                    if( z00 == NOVALUE || z01 == NOVALUE || 
                                        z10 == NOVALUE || z11 == NOVALUE ){
                                        if (z00!=NOVALUE) return (float)z00; if (z01!=NOVALUE) return (float)z01;
                                        if (z10!=NOVALUE) return (float)z10; if (z11!=NOVALUE) return (float)z11;
                                        return NOVALUE;
                                    }        
                                    z00 += dx*(z01-z00);
                                    z10 += dx*(z11-z10);       
                                    return float( z00 + dy*(z10 - z00) );
								};
	inline			float		GetDemZVal( int column,int row ){
									if ( column<0||column>=m_demHdr.column || row<0|| row>=m_demHdr.row ) return NOVALUE;
									return m_pGrid[ row*m_demHdr.column + column ];
								};
	inline void					GetDemZValue(double x,double y ,double *z){ *z=GetDemZValue(x,y); };
    inline void					GetDemZValue(double x,double y ,float  *z){ *z=GetDemZValue(x,y); };
    inline void					GetLocalXYZValue (int column,int row,double * DemXValue,double * DemYValue,double * DemZValue ){
									static float fDemZValue;  GetLocalXYZValue ( column,row,DemXValue,DemYValue,&fDemZValue );
									*DemZValue=fDemZValue; 
								};    
	inline void					GetLocalXYZValue (int column,int row,double * DemXValue,double * DemYValue,float * DemZValue ){
									double tempX=m_demHdr.startX;	
                                    double tempY=m_demHdr.startY;
									GlobalToLocal( &tempX,&tempY,m_demHdr.kapa );
									*DemXValue = tempX+m_demHdr.intervalX*column;
									*DemYValue = tempY+m_demHdr.intervalY*row;
									*DemZValue = GetDemZVal(column,row);
								};
	inline void					GetGlobalXYZValue(int column,int row,double * DemXValue,double * DemYValue,double * DemZValue ){ 
									static float fDemZValue; GetGlobalXYZValue(column,row,DemXValue,DemYValue,&fDemZValue );
									*DemZValue=fDemZValue;  
								};
    inline void					GetGlobalXYZValue(int column,int row,double * DemXValue,double * DemYValue,float * DemZValue ){
                                    double tempX=column*m_demHdr.intervalX; 
                                    double tempY=row   *m_demHdr.intervalY;
                                    LocalToGlobal( &tempX,&tempY,m_demHdr.kapa );        
                                    *DemXValue = tempX+m_demHdr.startX;
                                    *DemYValue = tempY+m_demHdr.startY;
                                    *DemZValue = GetDemZVal(column,row);
                                };
    inline         bool         GetDemRgn( double x[],double y[] ){
                                    SPDEMHDR demHdr = m_demHdr;
                                    double tempX=0*demHdr.intervalX,tempY=0*demHdr.intervalY; LocalToGlobal( &tempX,&tempY,demHdr.kapa );
                                    x[0] = tempX+demHdr.startX;  y[0] = tempY+demHdr.startY;
        
                                    tempX=(demHdr.column-1)*demHdr.intervalX; tempY=0*demHdr.intervalY; LocalToGlobal( &tempX,&tempY,demHdr.kapa );
                                    x[1] = tempX+demHdr.startX;  y[1] = tempY+demHdr.startY;
        
                                    tempX=(demHdr.column-1)*demHdr.intervalX; tempY=(demHdr.row-1)*demHdr.intervalY; LocalToGlobal( &tempX,&tempY,demHdr.kapa );
                                    x[2] = tempX+demHdr.startX;  y[2] = tempY+demHdr.startY;
        
                                    tempX=0*demHdr.intervalX; tempY=(demHdr.row-1)*demHdr.intervalY; LocalToGlobal( &tempX,&tempY,demHdr.kapa );
                                    x[3] = tempX+demHdr.startX;  y[3] = tempY+demHdr.startY;
                                    return true;
                                };

protected:
	SPDEMHDR	m_demHdr;
	float*		m_pGrid;
    float       m_midZ,m_minZ,m_maxZ;

protected:
    HANDLE      m_hFile;

protected:
	inline static void			LocalToGlobal( double * x, double * y,double kapa ){
									static double lastKap = 0,cosKap = cos(0),sinKap=sin(0);
									if ( (fabs(kapa-lastKap))>0.00000001 ){ lastKap = kapa; cosKap = cos(lastKap); sinKap = sin(lastKap); }

									double tempx=*x;	double tempy=*y;
									*x= tempx* cosKap - tempy * sinKap;
									*y= tempy* cosKap + tempx * sinKap;
								};
	inline static void			GlobalToLocal( double * x, double * y,double kapa ){
									static double lastKap = 0,cosKap = cos(0),sinKap=sin(0);
									if ( (fabs(kapa-lastKap))>0.00000001 ){ lastKap = kapa; cosKap = cos(lastKap); sinKap = sin(lastKap); }

									double tempx=*x; double tempy=*y;
									*x= tempx* cosKap + tempy * sinKap;
									*y= tempy* cosKap - tempx * sinKap;
								};    
	inline static BOOL			IsGeodetic(double angle, BOOL bLongitude = true){
									double Min = (bLongitude ? -180 : -90);
									double Max = (bLongitude ?  360 :  90);
									return (angle>=Min&&angle<=Max);
								}
	inline static void			DegMinSec2Deg(double* angle){
									double degree = int(*angle);
									double minute = int((*angle - degree) * 100);
									double second = ((*angle -degree)* 100 - minute) * 100;
									if (second >= 60) { minute++; second = ((*angle -degree)* 100 - minute) * 100; }
									*angle = degree + minute / 60 + second / 3600;
								};
	inline static void			Deg2DegMinSec(double* angle){
									double degree = int(*angle);
									double minute = int((*angle - degree) * 60);
									double second = (*angle - degree - minute/60) * 3600;
									if (second >= 60) { minute++; second -= 60; }
									*angle = degree + minute / 100 + second / 10000;
								};
    
private:
	double			m_cosKap,m_sinKap;

public:
    inline  void SetRevMsgWnd( HWND hWnd,UINT msgID ){   m_hWndRec=hWnd; m_msgID=msgID; };
protected:	
    virtual void ProgBegin(int range)       {if ( IsWindow(m_hWndRec) ) SendMessage( m_hWndRec,m_msgID,PROG_START,range );          };
    virtual void ProgStep(int step)         {if ( IsWindow(m_hWndRec) ) SendMessage( m_hWndRec,m_msgID,PROG_STEP ,step );           };
    virtual void ProgEnd()                  {if ( IsWindow(m_hWndRec) ) SendMessage( m_hWndRec,m_msgID,PROG_OVER ,0 );              };
    virtual void PrintMsg(LPCSTR lpstrMsg ) {if ( IsWindow(m_hWndRec) ) SendMessage( m_hWndRec,m_msgID,PROG_MSG  ,UINT(lpstrMsg) ); };
private:
    HWND            m_hWndRec;
    UINT            m_msgID;

};

typedef class CWuDem CSpDem;



#endif
