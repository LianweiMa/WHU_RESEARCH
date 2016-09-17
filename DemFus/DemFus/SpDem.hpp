// SpDem.hpp : main  file for the CSpDem
//
/*----------------------------------------------------------------------+
|		CSpDem														    |
|       Author:     duanyansong  2002.10 								|
|            Ver 1.0													|
|       Copyright (c) 2002, Supresoft Corporation                       |
|	         All rights reserved Duanyansong.                           |
|		a class for dem process										    |
+----------------------------------------------------------------------*/
/*
	class name : CSpDem	
member function:
	inline  void	SetRevMsgWnd( HWND hWnd,UINT msgID );
	inline  void	Reset();
	inline  void	Attach( SPDEMHDR demHdr,float *pGrid  );
	inline  void	Detach();
    inline  void	InitDem( SPDEMHDR demHdr,float *pGrid );
    inline  void    GetDemRgn(LPCSTR lpstrPathName,double x[4],y[4],LPSPDEMHDR pDemHdr );
    inline  void	SetDemHeader( LPSPDEMHDR pDemHdr );
	inline  void	SetDemHeader( SPDEMHDR demHdr   );
	inline  void	SetDemZValue(int column,int row,float  * DemZValue);
	inline  void	SetDemZValue(int column,int row,double * DemZValue);
	inline  void	SetDemZValue(int column,int row,float z);

    inline  SPDEMHDR            GetDemHdr(LPCSTR lpstrPathName);
	inline	const   LPSPDEMHDR	GetDemHeaderPr();
	inline			SPDEMHDR&	GetDemHeader();
	inline			void		GetDemHeader( LPSPDEMHDR pHdr);
	inline  const   float*		GetZGridBuf();
	inline			void		GetDemZValue( int column,int row,float* DemZValue );
	inline			void		GetDemZValue( int column,int row,double* DemZValue);
	inline			float		GetDemZVal( int column,int row )
	inline void					GetDemZValue(double x,double y ,double *z);
    inline void					GetDemZValue(double x,double y ,float  *z);
    float						GetDemZValue(double x,double y,bool bLi = true);
	inline void					GetLocalXYZValue (int column,int row,double * DemXValue,double * DemYValue,float * DemZValue );
	inline void					GetGlobalXYZValue(int column,int row,double * DemXValue,double * DemYValue,float * DemZValue );
	virtual bool				Load4File( LPCSTR lpstrPathName );
	virtual bool				Save2File( LPCSTR lpstrPathName );
    virtual bool                Covert2BEM( LPCSTR lpstrPathName,LPCSTR lpstrPathNameBEM );

    
	inline	bool				SaveAsCSDTF(LPCSTR lpstrPathName);
	inline  bool				SaveAsArcGRID(LPCSTR lpstrPathName);
	inline  bool				SaveAsBIL(LPCSTR lpstrPathName);
    inline  bool				SaveAsGTIF(LPCSTR lpstrPathName);

    //For File Access ,Part Read and Write
    BOOL                Open(LPCSTR lpstrPathName,int mode,SPDEMHDR *pHdr=NULL);
    void                Close();
    BOOL                ReadRow(float *pBuf,int row);
    BOOL                WriteRow(float *pBuf,int row);
    BOOL                ReadBlock(float *pBuf,int sRow,int sCol,int rows,int cols);
    BOOL                WriteBlock(float *pBuf,int sRow,int sCol,int rows,int cols);
*/

#ifndef SPDEM_H_SUPRESOFT_INC_DUANYANSONG_2002_10_17_873495
#define SPDEM_H_SUPRESOFT_INC_DUANYANSONG_2002_10_17_873495

#include "math.h"


//#include "SpBmpFile.hpp"


#ifdef WIN32

#include "windows.h"
#ifdef _CONSOLE

#define	SpMessageBox(str)			printf( "%s\n",str )
static bool IsWindowE(HWND hWnd){ return false; };
static void SendMessageE(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam){ };

#else

#ifdef _AFXDLL
#define		SpMessageBox(str)		AfxMessageBox(str)
#else
#define		SpMessageBox(str)		::MessageBox( ::GetFocus(),str,"Error",MB_OK )
#endif // _AFXDLL
#define IsWindowE                   IsWindow
#define SendMessageE                SendMessage

#endif // _CONSOLE

#define _sleep100						Sleep(100)
#ifndef _CreateFileE
#define _CreateFileE
static HANDLE CreateFileE(LPCSTR lpstrPathName,UINT dwDesiredAccess ){
	if ( dwDesiredAccess==GENERIC_READ ) 
		 return ::CreateFile( lpstrPathName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,NULL );
	else return ::CreateFile( lpstrPathName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL );
}
#endif
#else

#define __USE_LARGEFILE64
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define _sleep100					sleep(1)
#define	SpMessageBox(str)			printf( "%s\n",str )

typedef union _LARGE_INTEGER {
    struct { DWORD LowPart; unsigned int HighPart; };
    struct { DWORD LowPart; unsigned int HighPart; }u;
    LONGLONG QuadPart;
}LARGE_INTEGER;

#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define FILE_BEGIN						 SEEK_SET	
#define FILE_CURRENT					 SEEK_CUR
#define FILE_END						 SEEK_END
#define INVALID_HANDLE_VALUE			 (HANDLE)-1

inline char* strlwr( char *str ){
	char *orig = str; char d = 'a'-'A';
	for( ;*str!='\0';str++ ){ if ( *str>='A' && *str<='Z' ) *str = *str+d; }
	return orig;
}
inline static void CloseHandle(int hFile){ close(hFile); }
inline static int WriteFile( int hFile,void *pBuf,unsigned int bufSize,void *opSz,int *lpOverlapped ){ 
	return write( hFile,pBuf,bufSize ); }
inline static int ReadFile( int hFile,void *pBuf,unsigned int bufSize,void *opSz,unsigned int *lpOverlapped ){ 
	return read( hFile,pBuf,bufSize ); }
inline static unsigned int SetFilePointer( int hFile,DWORD lDistanceToMove,DWORD *lpDistanceToMoveHigh,int dwMoveMethod ){
	LONGLONG ret,dis=0; if ( lpDistanceToMoveHigh ) dis = *lpDistanceToMoveHigh;
	dis = (dis<<32)|lDistanceToMove;
	ret = lseek64( hFile,dis,dwMoveMethod );
	return (unsigned int)ret;
}
inline static HANDLE CreateFileE(LPCSTR lpstrPathName,UINT dwDesiredAccess ){
	if ( dwDesiredAccess==GENERIC_READ ) return open64( lpstrPathName,O_RDWR );
	return open64( lpstrPathName,O_CREAT|O_RDWR,S_IRUSR|S_IWUSR );
}

#endif // end for WIN32


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
}SPDEMHDR,*LPSPDEMHDR;
#else
#pragma message("SpDem.hpp, Warning: SPDEMHDR alread define, be sure it was define as: struct tagSPDEMHDR{ double startX,startY,kapa;double intervalX,intervalY;int column;int row; }. \
               \nSpDem.hpp, 警告:类型 SPDEMHDR 已经定义过,请确保其定义为: struct tagSPDEMHDR{ double startX,startY,kapa;double intervalX,intervalY;int column;int row; }. ") 
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

#define SORTF( pList,size ) { float t; for ( int m=0;m<size;m++ ){ for ( int n=m+1;n<size;n++ ){ if (pList[m]<pList[n]){ t=pList[n]; pList[n]=pList[m];pList[m]=t; } } } }
class CSpDem 
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
    CSpDem(){ m_pGrid = NULL; memset( &m_demHdr,0,sizeof(m_demHdr) );  m_midZ=m_minZ=m_maxZ=0; m_hFile=NULL;  };
    virtual ~CSpDem(){ Reset(); Close(); };
	
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
    inline  void    FilterSmooth(){
                        float *pZ,grid[32],*pZN0 = new float[ m_demHdr.column*m_demHdr.row +128 ];                        
                        memcpy( pZN0,m_pGrid,sizeof(float)*m_demHdr.column*m_demHdr.row );
						int r, c, i;
                        for ( r=2;r<m_demHdr.row-2;r++ ){
                            for (  c=2;c<m_demHdr.column-2;c++ ){
                                pZ = m_pGrid+r*m_demHdr.column+c; if ( *pZ==NOVALUE ) continue;
                
                                pZ= m_pGrid+(r-2)*m_demHdr.column+c-2;
                                grid[0 ]= *pZ++; grid[ 1]= *pZ++;grid[ 2]= *pZ++; grid[ 3]= *pZ++;grid[ 4]= *pZ++;
                                pZ= m_pGrid+(r-1)*m_demHdr.column+c-2;
                                grid[5 ]= *pZ++; grid[ 6]= *pZ++;grid[ 7]= *pZ++; grid[ 8]= *pZ++;grid[ 9]= *pZ++;
                                pZ= m_pGrid+(  r)*m_demHdr.column+c-2;
                                grid[10]= *pZ++; grid[11]= *pZ++;grid[12]= *pZ++; grid[13]= *pZ++;grid[14]= *pZ++;
                                pZ= m_pGrid+(r+1)*m_demHdr.column+c-2;
                                grid[15]= *pZ++; grid[16]= *pZ++;grid[17]= *pZ++; grid[18]= *pZ++;grid[19]= *pZ++;								
                                pZ= m_pGrid+(r+2)*m_demHdr.column+c-2;
                                grid[20]= *pZ++; grid[21]= *pZ++;grid[22]= *pZ++; grid[23]= *pZ++;grid[24]= *pZ++;
                
                                SORTF( grid,25 ); for ( i=0;i<25;i++){ if(grid[i]==NOVALUE) break; }
                                pZN0[r*m_demHdr.column+c] = grid[i/2]; 
                            }
                        }
                        memcpy( m_pGrid,pZN0,sizeof(float)*m_demHdr.column*m_demHdr.row  );
                        delete pZN0;
                    };

	inline	BOOL	IsLoaded(){ return m_pGrid&&m_demHdr.column&&m_demHdr.row; };
	inline  void	Reset(){ if (m_pGrid) delete m_pGrid; m_pGrid=NULL;memset( &m_demHdr,0,sizeof(m_demHdr) ); Close(); };
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
	inline  void	SetDemHeader( SPDEMHDR demHdr   ){
                        if ( m_demHdr.column*m_demHdr.row==demHdr.column*demHdr.row && m_pGrid ){
                            m_demHdr = demHdr; m_cosKap = cos(m_demHdr.kapa);m_sinKap = sin(m_demHdr.kapa);
                        }else{
                            m_demHdr = demHdr; m_cosKap = cos(m_demHdr.kapa);m_sinKap = sin(m_demHdr.kapa);
						    if (m_pGrid) delete m_pGrid; m_pGrid = NULL;
                            if ( m_demHdr.column*m_demHdr.row>0 )
                            {  m_pGrid = new float[ m_demHdr.column*m_demHdr.row ]; for (int i=0;i<m_demHdr.column*m_demHdr.row;i++ ) m_pGrid[i]=NOVALUE; }
                        }
                     };

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
                    };

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
	inline			float		GetDemZVal( int column,int row ){
									if ( column<0||column>=m_demHdr.column || row<0|| row>=m_demHdr.row ) return NOVALUE;
									return m_pGrid[ row*m_demHdr.column + column ];
								};

	///Add By SMW (Begin)///
	virtual BOOL				CutBinDem(double l,double r,double b,double t,CSpDem &demres)
	{
		if (!m_hFile) return FALSE;
		
		SPDEMHDR demhard;
		demhard.kapa=0.0;
		demhard.intervalX=m_demHdr.intervalX;
		demhard.intervalY=m_demHdr.intervalY;
		
		int sCol=int((l-m_demHdr.startX)/demhard.intervalX);
		int sRow=int((b-m_demHdr.startY)/demhard.intervalY);
		
		demhard.startX=sCol*demhard.intervalX+m_demHdr.startX;
		demhard.startY=sRow*demhard.intervalY+m_demHdr.startY;
		
		demhard.column=int((r-demhard.startX)/demhard.intervalX+1.0);
		if (demhard.startX+(demhard.column-1)*demhard.intervalX < r) demhard.column++;
		
		demhard.row=int((t-demhard.startY)/demhard.intervalY+1.0);
		if (demhard.startY+(demhard.row-1)*demhard.intervalY < t) demhard.row++;
		
		if (demhard.row<=1||demhard.column<=1) return FALSE;
		if (sCol>=m_demHdr.column || sRow>=m_demHdr.row || sCol+demhard.column<=0 || sRow+demhard.row<=0) return FALSE;
		
		float *pGrid=new float[demhard.row*demhard.column];
		int lsize=demhard.row*demhard.column;
		float *pPos=pGrid;
		int i=0;
		for (i=0;i<lsize;i++)
		{
			*pPos=float(-9999.9);
			pPos++;
		}
		
		int rscol=max(sCol,0);
		int rsrow=max(sRow,0);
		
		int rcols=demhard.column;
		if (sCol<0) rcols+=sCol;
		if (rcols+rscol>m_demHdr.column) rcols=m_demHdr.column-rscol;
		
		int rrows=demhard.row;
		if (sRow<0) rrows+=sRow;
		if (rrows+rsrow>m_demHdr.row) rrows=m_demHdr.row-rsrow;
		
		pPos=pGrid+(rsrow-sRow)*demhard.column+(rscol-sCol);
		for (i=0;i<rrows;i++)
		{
			ReadBlock(pPos,rsrow+i,rscol,1,rcols);
			pPos+=demhard.column;
		}
		demres.InitDem(demhard,pGrid);
		
		delete pGrid;
		pGrid=NULL;
		return TRUE;
	};
	
	virtual double				GetBinZVal(double x,double y)
	{
		if (x<m_demHdr.startX||y<m_demHdr.startY) return -9999.9;
		
		int sCol=int((x-m_demHdr.startX)/m_demHdr.intervalX);
		int sRow=int((y-m_demHdr.startY)/m_demHdr.intervalY);
		if (sCol<0||sCol+1>=m_demHdr.column || sRow<0||sRow+1>=m_demHdr.row) return -9999.9;
		
		float z[4]={0.0};
		ReadBlock(z,sRow,sCol,2,2);
		if (z[0]<-9999||z[1]<-9999||z[2]<-9999||z[3]<-9999) return -9999.9;
		
		double x0=m_demHdr.startX+m_demHdr.intervalX*sCol;
		double y0=m_demHdr.startY+m_demHdr.intervalY*sRow;
		double x1=x0+m_demHdr.intervalX;
		double y1=y0+m_demHdr.intervalY;
		double XY=m_demHdr.intervalX*m_demHdr.intervalY;
		
		return   z[0]*(x1-x)*(y1-y)/XY
			+z[1]*(x-x0)*(y1-y)/XY
			+z[2]*(x1-x)*(y-y0)/XY
			+z[3]*(x-x0)*(y-y0)/XY;
	};
	
		///Add By SMW (End)///               

	///////////////////////////////////////////////////////////////////////////////
    /*
    Function GetDemZValue
    Par:    
            Ground Coord x & y value
    Return: 
            The Z value of the ground point (x,y)
            if Z == DemNoData then the point(x,y) is out of side or have some error
    */
	inline void					GetDemZValue(double x,double y ,double *z){ *z=GetDemZValue(x,y); };
    inline void					GetDemZValue(double x,double y ,float  *z){ *z=GetDemZValue(x,y); };
    float						GetDemZValue(double x,double y,bool bLi = true){
									double dx = (x - m_demHdr.startX);
									double dy = (y - m_demHdr.startY);

									x = ( dx * m_cosKap + dy *  m_sinKap ) / m_demHdr.intervalX ;
									y = (-dx * m_sinKap + dy *  m_cosKap ) / m_demHdr.intervalY ;

									if ( !bLi ){
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
									
									z00 += dx*(z01-z00);	/* px0 */
									z10 += dx*(z11-z10);	/* px1 */
									
									return float( z00 + dy*(z10 - z00) );
								};

    inline void					GetLocalXYZValue (int column,int row,double * DemXValue,double * DemYValue,double * DemZValue ){
									static float fDemZValue;  GetLocalXYZValue ( column,row,DemXValue,DemYValue,&fDemZValue );
									*DemZValue=fDemZValue; 
								};
	inline void					GetLocalXYZValue (int column,int row,double * DemXValue,double * DemYValue,float * DemZValue ){
									double tempX=m_demHdr.startX;	double tempY=m_demHdr.startY;
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
									double tempX=column*m_demHdr.intervalX; double tempY=row   *m_demHdr.intervalY;
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
	inline         bool         GetDemGeoRgn(double x[], double y[]){//add by mlw
		for (int i = 0; i < m_demHdr.row;i++)
		{
			//fixed x[0]
			if (GetDemZVal(1,i)!=NOVALUE)
			{
				x[0] = m_demHdr.startX; y[0] = m_demHdr.startY+i*m_demHdr.intervalY;
				break;
			}
		}
		for (int i = 0; i < m_demHdr.column; i++)
		{
			//fixed y[0]
			if (GetDemZVal(i, 1) != NOVALUE)
			{
				x[1] = m_demHdr.startX+i*m_demHdr.intervalX; y[1] = m_demHdr.startY;
				break;
			}
		}
		for (int i = 0; i < m_demHdr.row; i++)
		{
			//fixed x[2]
			if (GetDemZVal(m_demHdr.column-2, i) != NOVALUE)
			{
				x[2] = m_demHdr.startX + (m_demHdr.column - 1)*m_demHdr.intervalX; y[2] = m_demHdr.startY + i*m_demHdr.intervalY;
				break;
			}
		}
		for (int i = 0; i < m_demHdr.column; i++)
		{
			//fixed y[2]
			if (GetDemZVal(i, m_demHdr.row-2) != NOVALUE)
			{
				x[3] = m_demHdr.startX + i*m_demHdr.intervalX; y[3] = m_demHdr.startY + (m_demHdr.row - 1)*m_demHdr.intervalY;
				break;
			}
		}
		return true;
	}

    inline static  SPDEMHDR     GetDemHdr(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE,char *strFmt=NULL){
                                    FILE *fDem = fopen( lpstrPathName,"rt" ); SPDEMHDR demHdr; memset( &demHdr,0,sizeof(demHdr) ); char str[512]; 
                                    if ( !fDem ){ char strMsg[512]="Can not open file:"; strcat( strMsg,lpstrPathName ); if (bMsgBox) SpMessageBox( strMsg ); return demHdr; };
                                    fscanf( fDem,"%32s",str );
                                    if ( str[0]==0x49&&str[1]==0x49&&str[2]==0x2A ){ // Is GeoTIF DEM	
										fclose(fDem); CSpDem dem; 									
										if ( dem.Load4GTIF( lpstrPathName,bMsgBox,TRUE ) ) 
											demHdr = dem.m_demHdr;
                                        if ( strFmt ) strcpy( strFmt,FMT_TIF );
									}else
                                    if ( strncmp(str,"DEMBIN",6)==0 || strncmp(str,"DEMBSH",6)==0 || strncmp(str,"DEMBCH",6)==0 )
                                    {
                                        fclose( fDem ); fDem = fopen( lpstrPathName,"rb" );
                                        fread( str,512,1,fDem );
                                        fread( &demHdr,sizeof(demHdr),1,fDem ); 
                                        if ( strFmt ) strcpy( strFmt,FMT_BEM );
                                    }else
                                    if ( strstr( str,"SDTF" ) )
                                    {
                                        double Ex=0,Ny=0; float zoom=100; int hrLine=0; char *pV;
                                        if ( strstr( str,"DataMark" ) )
                                        {
                                            pV = strchr( str,':' );
                                            while( pV ){
                                                if ( strstr(str,"Alpha" ) ) demHdr.kapa      = atof(pV+1); else
                                                if ( strstr(str,"X0"    ) ) Ex               = atof(pV+1); else
                                                if ( strstr(str,"Y0"    ) ) Ny               = atof(pV+1); else
                                                if ( strstr(str,"DX"    ) ) demHdr.intervalX = atof(pV+1); else
                                                if ( strstr(str,"DY"    ) ) demHdr.intervalY = atof(pV+1); else
                                                if ( strstr(str,"Row"   ) ) demHdr.row       = int( atof(pV+1) +0.9); else
                                                if ( strstr(str,"Col"   ) ) demHdr.column    = int( atof(pV+1) +0.9); else
                                                if ( strstr(str,"Hzoom" ) ) zoom             = float( atof(pV+1) );
                                                hrLine ++; fgets( str,256,fDem ); pV = strchr( str,':' );
                                            }
                                            fclose(fDem); 
                                            if ( strFmt ) strcpy( strFmt,FMT_CSDTF );
                                        }else
                                        {                                            
                                            fscanf( fDem,"%s",str );
                                            fscanf( fDem,"%s",str );
                                            fscanf( fDem,"%lf",&demHdr.kapa );
                                            fscanf( fDem,"%s",str );
                                            fscanf( fDem,"%lf",&Ex );
                                            fscanf( fDem,"%lf",&Ny );
                                            fscanf( fDem,"%lf",&demHdr.intervalX );
                                            fscanf( fDem,"%lf",&demHdr.intervalY );
                                            fscanf( fDem,"%d",&demHdr.row     ); 
                                            fscanf( fDem,"%d",&demHdr.column  ); 
                                            fscanf( fDem,"%f",&zoom );
                                            fclose( fDem );
                                            if ( strFmt ) strcpy( strFmt,FMT_NSDTF );
                                        }
                                        double cosKap=cos(demHdr.kapa),sinKap=sin(demHdr.kapa); 
                                        demHdr.startX = Ex+sinKap* (demHdr.intervalY)*(demHdr.row-1);
                                        demHdr.startY = Ny-cosKap* (demHdr.intervalY)*(demHdr.row-1);
                                    }else
                                    {
                                        char strType[8]; memset(strType, 0, sizeof(strType));
                                        fseek(fDem, 8, SEEK_SET); fscanf(fDem, "%8s", strType); strType[3] = '\0';
                                        BOOL bGJBFormat = (_stricmp(strType, "MGM") == 0 || _stricmp(strType, "GEM") == 0 || _stricmp(strType, "MDM") == 0 || _stricmp(strType, "DEM") == 0);
                                        if (bGJBFormat)
                                        {
                                            struct HEAD
                                            {
                                                char ver[8], type[8], pathname[256];
                                                char mapTool[256], edtTool[256];
                                                char precision[8], mangNote[1024];
                                                double xCutConst, yCutConst;
                                                double minX, minY, maxX, maxY;
                                                double dX, dY;
                                                int secNum;
                                            };
                                            HEAD hdr;  memset(&hdr, 0, sizeof(hdr));
                                            BOOL bGeodetic = (_stricmp(strType, "MDM") == 0 || _stricmp(strType, "DEM") == 0);
                                            fseek( fDem, 0, SEEK_SET);                                            
                                            fread( hdr.ver,		 sizeof(hdr.ver),		1,fDem);
                                            fread( hdr.type,	 sizeof(hdr.type),		1,fDem);
                                            fread( hdr.pathname, sizeof(hdr.pathname),	1,fDem);
                                            fread( hdr.mapTool,	 sizeof(hdr.mapTool),	1,fDem);
                                            fread( hdr.edtTool,	 sizeof(hdr.edtTool),	1,fDem);
                                            fread( hdr.precision,sizeof(hdr.precision),	1,fDem);
                                            fread( hdr.mangNote, sizeof(hdr.mangNote),	1,fDem);
                                            fread(&hdr.xCutConst,sizeof(hdr.xCutConst),	1,fDem); 
                                            fread(&hdr.yCutConst,sizeof(hdr.yCutConst),	1,fDem); 
                                            fread(&hdr.minX,	 sizeof(hdr.minX),		1,fDem); 
                                            fread(&hdr.minY,	 sizeof(hdr.minY),		1,fDem); 
                                            fread(&hdr.maxX,	 sizeof(hdr.maxX),		1,fDem); 
                                            fread(&hdr.maxY,	 sizeof(hdr.maxY),		1,fDem); 
                                            fread(&hdr.dX,		 sizeof(hdr.dX),		1,fDem); 
                                            fread(&hdr.dY,		 sizeof(hdr.dY),		1,fDem); 
                                            fread(&hdr.secNum,	 sizeof(hdr.secNum),	1,fDem);                                                                                                                             
                                            fclose( fDem );
                                            if (bGeodetic) DegMinSec2Deg(&hdr.xCutConst);
                                            if (bGeodetic) DegMinSec2Deg(&hdr.yCutConst);
                                            if (bGeodetic) DegMinSec2Deg(&hdr.minX);
                                            if (bGeodetic) DegMinSec2Deg(&hdr.minY);
                                            if (bGeodetic) DegMinSec2Deg(&hdr.maxX);
                                            if (bGeodetic) DegMinSec2Deg(&hdr.maxY);
                                            if (bGeodetic) { hdr.dX /= 10000; DegMinSec2Deg(&hdr.dX); };
                                            if (bGeodetic) { hdr.dY /= 10000; DegMinSec2Deg(&hdr.dY); };
                                            if (hdr.secNum < 1) return demHdr;
                                            
                                            demHdr.startX = hdr.minX + hdr.xCutConst;
                                            demHdr.startY = hdr.minY + hdr.yCutConst;
                                            demHdr.kapa = 0;
                                            demHdr.intervalX = hdr.dX; 
                                            demHdr.intervalY = hdr.dY;
                                            demHdr.column = int(ceil((hdr.maxX - hdr.minX)/hdr.dX) + 1);
                                            demHdr.row	  = int(ceil((hdr.maxY - hdr.minY)/hdr.dY) + 1); 
                                            
                                            if ( strFmt ) strcpy( strFmt,FMT_GJBDEM );
                                        }else{
                                            fclose( fDem ); fDem = fopen( lpstrPathName,"rt" );
                                            if( fscanf( fDem,"%lf%lf%lf%lf%lf%d%d",&demHdr.startX,
                                                &demHdr.startY,&demHdr.kapa,&demHdr.intervalX ,
                                                &demHdr.intervalY,&demHdr.column,&demHdr.row)<7 )
                                                { char strMsg[512]="Invalidate VirtuoZo Dem format"; strcat( strMsg,lpstrPathName ); if (bMsgBox) SpMessageBox( strMsg ); fclose( fDem ); return demHdr; }
                                            if ( strFmt ) strcpy( strFmt,FMT_DEM );
                                        }
                                    }
                                    fclose( fDem );
                                    return demHdr;
                                };
    inline static  bool	        GetDemRgn(LPCSTR lpstrPathName,double x[],double y[],LPSPDEMHDR pDemHdr,BOOL bMsgBox=TRUE ){
                                    SPDEMHDR demHdr = GetDemHdr(lpstrPathName,bMsgBox); if (demHdr.column==0) return false;

                                    double tempX=0*demHdr.intervalX,tempY=0*demHdr.intervalY; LocalToGlobal( &tempX,&tempY,demHdr.kapa );
									x[0] = tempX+demHdr.startX;  y[0] = tempY+demHdr.startY;
                                    
                                    tempX=(demHdr.column-1)*demHdr.intervalX; tempY=0*demHdr.intervalY; LocalToGlobal( &tempX,&tempY,demHdr.kapa );
									x[1] = tempX+demHdr.startX;  y[1] = tempY+demHdr.startY;

                                    tempX=(demHdr.column-1)*demHdr.intervalX; tempY=(demHdr.row-1)*demHdr.intervalY; LocalToGlobal( &tempX,&tempY,demHdr.kapa );
									x[2] = tempX+demHdr.startX;  y[2] = tempY+demHdr.startY;

                                    tempX=0*demHdr.intervalX; tempY=(demHdr.row-1)*demHdr.intervalY; LocalToGlobal( &tempX,&tempY,demHdr.kapa );
									x[3] = tempX+demHdr.startX;  y[3] = tempY+demHdr.startY;
                                    
                                    if ( pDemHdr ) *pDemHdr = demHdr;
                                    return true;
                                };

	virtual bool				Load4File( LPCSTR lpstrPathName,BOOL bMsgBox=TRUE )
								{								
                                    double minZ=99999999.f,maxZ=-99999999.f; float zval; double midZ=0,sum=1;
									FILE *fDem = fopen( lpstrPathName,"rt" ); SPDEMHDR demHdr; char str[512];
									if ( !fDem ){ char strMsg[512]="Can not open file:"; strcat( strMsg,lpstrPathName ); if (bMsgBox) SpMessageBox( strMsg ); return FALSE; };
                                    fgets( str,512,fDem ); while( str[0]=='\n' ) fgets( str,512,fDem );
									if ( str[0]==0x49&&str[1]==0x49&&str[2]==0x2A ){
										fclose(fDem);  // Is TIF
										return Load4GTIF( lpstrPathName,bMsgBox );										
									}else
                                    if ( strncmp(str,"DEMBIN",6)==0 || strncmp(str,"DEMBSH",6)==0 ||strncmp(str,"DEMBCH",6)==0  ){
                                        fclose(fDem); fDem = fopen( lpstrPathName,"rb" );
                                        fread( str,512,1,fDem );
                                        fread( &demHdr,sizeof(demHdr),1,fDem );
                                        
                                        if ( demHdr.column*demHdr.row*sizeof(float)>512*1024*1024 ) return false;
                                        SetDemHeader( demHdr ); if ( demHdr.column*demHdr.row<=0 ){ fclose( fDem ); return true; }

                                        int i,bufSize = demHdr.column*demHdr.row; float *pT,*pZ = m_pGrid;
                                        if ( strncmp(str,"DEMBSH",6)==0||strncmp(str,"DEMBCH",6)==0 ){
                                            if ( strncmp(str,"DEMBSH",6)==0 ){                                            
                                                short *pGs,*pG = new short[ demHdr.column ];
                                                for ( i=0;i<demHdr.row;i++ ){
                                                    fread( pG,demHdr.column,sizeof(short),fDem );
                                                    pT = m_pGrid+i*demHdr.column; pGs = pG;
                                                    for( int c=0;c<demHdr.column;c++,pT++,pGs++ ) 
                                                        *pT = *pGs;                                                
                                                }
                                                delete pG;
                                            }else{
                                                char *pGs,*pG = new char[ demHdr.column ];
                                                for ( i=0;i<demHdr.row;i++ ){
                                                    fread( pG,demHdr.column,sizeof(char),fDem );
                                                    pT = m_pGrid+i*demHdr.column; pGs = pG;
                                                    for( int c=0;c<demHdr.column;c++,pT++,pGs++ ) 
                                                        *pT = *pGs;                                                
                                                }
                                                delete pG;
                                            }
                                        }else{
                                            for ( i=0;i<demHdr.row;i++ )
                                                fread( m_pGrid+i*demHdr.column,demHdr.column,sizeof(float),fDem );                                            
                                        }
                                        for ( i=0;i<bufSize;i++,pZ++ ){
                                            if ( *pZ>(-9999.8) ){
                                                if ( minZ>*pZ ) minZ=*pZ;
                                                if ( maxZ<*pZ ) maxZ=*pZ;
                                                midZ += *pZ; sum += 1;
                                            }else
                                                *pZ = NOVALUE;
                                        }
                                    }else
                                    {
										char strType[8]; memset(strType, 0, sizeof(strType));
										fseek(fDem, 8, SEEK_SET); fscanf(fDem, "%8s", strType); strType[3] = '\0';
										BOOL bGJBFormat = (_stricmp(strType, "MGM") == 0 || _stricmp(strType, "GEM") == 0 || _stricmp(strType, "MDM") == 0 || _stricmp(strType, "DEM") == 0);
                                        if (!bGJBFormat) { fseek(fDem, 0, SEEK_SET); fgets( str,512,fDem ); while( str[0]=='\n' ) fgets( str,512,fDem ); }

										if (bGJBFormat)
										{
											struct HEAD
											{
												char ver[8], type[8], pathname[256];
												char mapTool[256], edtTool[256];
												char precision[8], mangNote[1024];
												double xCutConst, yCutConst;
												double minX, minY, maxX, maxY;
												double dX, dY;
												int secNum;
											};
											struct SECTION
											{
												int no;
												double sx, sy, ex, ey;
												int pn;
												float *elevation;
											};
											
											HEAD hdr; SECTION sec; memset(&sec, 0, sizeof(SECTION));
											fclose(fDem); fDem = fopen(lpstrPathName, "rb");
											fread( hdr.ver,		 sizeof(hdr.ver),		1,fDem);
											fread( hdr.type,	 sizeof(hdr.type),		1,fDem);
											fread( hdr.pathname, sizeof(hdr.pathname),	1,fDem);
											fread( hdr.mapTool,	 sizeof(hdr.mapTool),	1,fDem);
											fread( hdr.edtTool,	 sizeof(hdr.edtTool),	1,fDem);
											fread( hdr.precision,sizeof(hdr.precision),	1,fDem);
											fread( hdr.mangNote, sizeof(hdr.mangNote),	1,fDem);
											fread(&hdr.xCutConst,sizeof(hdr.xCutConst),	1,fDem); 
											fread(&hdr.yCutConst,sizeof(hdr.yCutConst),	1,fDem); 
											fread(&hdr.minX,	 sizeof(hdr.minX),		1,fDem); 
											fread(&hdr.minY,	 sizeof(hdr.minY),		1,fDem); 
											fread(&hdr.maxX,	 sizeof(hdr.maxX),		1,fDem); 
											fread(&hdr.maxY,	 sizeof(hdr.maxY),		1,fDem); 
											fread(&hdr.dX,		 sizeof(hdr.dX),		1,fDem); 
											fread(&hdr.dY,		 sizeof(hdr.dY),		1,fDem); 
											fread(&hdr.secNum,	 sizeof(hdr.secNum),	1,fDem);
											if (hdr.secNum < 1) { fclose(fDem); return FALSE; }

											BOOL bGeodetic = (_stricmp(strType, "MDM") == 0 || _stricmp(strType, "DEM") == 0);
											if (bGeodetic) 
											{
												if (!IsGeodetic(hdr.xCutConst+hdr.minX,true ) || !IsGeodetic(hdr.xCutConst+hdr.maxX,true ) ||
													!IsGeodetic(hdr.yCutConst+hdr.minY,false) || !IsGeodetic(hdr.yCutConst+hdr.maxY,false)){
													fclose(fDem); return false;
												}
												DegMinSec2Deg(&hdr.xCutConst);
												DegMinSec2Deg(&hdr.yCutConst);
												DegMinSec2Deg(&hdr.minX);
												DegMinSec2Deg(&hdr.minY);
												DegMinSec2Deg(&hdr.maxX);
												DegMinSec2Deg(&hdr.maxY);
												hdr.dX /= 10000; DegMinSec2Deg(&hdr.dX);
												hdr.dY /= 10000; DegMinSec2Deg(&hdr.dY);
											}

											demHdr.startX = hdr.minX + hdr.xCutConst;
											demHdr.startY = hdr.minY + hdr.yCutConst;
											demHdr.kapa = 0;
											demHdr.intervalX = hdr.dX; 
											demHdr.intervalY = hdr.dY;
											demHdr.column = int(ceil((hdr.maxX - hdr.minX)/hdr.dX) + 1);
											demHdr.row	  = int(ceil((hdr.maxY - hdr.minY)/hdr.dY) + 1);

											SetDemHeader(demHdr);

											int i = 0, j = 0;
											int nMaxSize = 1024;
											float* pBuf = new float[nMaxSize]; memset(pBuf, 0, sizeof(float)*nMaxSize);
											float* pZ = NULL;
											ProgBegin(demHdr.row);
											for (i = 0; i < hdr.secNum; i++, ProgStep(1))
											{
												memset(&sec, 0, sizeof(SECTION));
												fread(&sec.no, sizeof(int),	   1, fDem);
												fread(&sec.sx, sizeof(double), 1, fDem); if (bGeodetic) DegMinSec2Deg(&sec.sx);
												fread(&sec.sy, sizeof(double), 1, fDem); if (bGeodetic) DegMinSec2Deg(&sec.sy);
												fread(&sec.ex, sizeof(double), 1, fDem); if (bGeodetic) DegMinSec2Deg(&sec.ex);
												fread(&sec.ey, sizeof(double), 1, fDem); if (bGeodetic) DegMinSec2Deg(&sec.ey);
												fread(&sec.pn, sizeof(int),    1, fDem);

												if (sec.pn > nMaxSize)
												{
													float* po = pBuf; nMaxSize = sec.pn; pBuf = new float[sec.pn];
													delete [] po; po = NULL;
												}
												fread(pBuf, sizeof(float)*sec.pn, 1, fDem);
												for (j = 0, pZ = pBuf; j < sec.pn; j++, pZ++)
												{
													if ( *pZ>(-9999.8) )
													{
														if ( minZ>*pZ ) minZ=*pZ;
														if ( maxZ<*pZ ) maxZ=*pZ;
														midZ += *pZ; sum += 1;
													}else
														*pZ = NOVALUE;
												}

												int sr = int((sec.sy - hdr.minY)/hdr.dY), nr = int((sec.ey - sec.sy)/hdr.dY + 1);
												int sc = int((sec.sx - hdr.minX)/hdr.dX), nc = /*int((sec.ex - sec.sx)/hdr.dX + 1)*/sec.pn/nr;
												for (j = 0; j < nr; j++)
												{
													float* p = m_pGrid+(demHdr.column*(sr+j)+sc);
													memcpy(p, pBuf+nc*j, sizeof(float)*nc);
												}
											}

											if (pBuf != NULL) delete [] pBuf; pBuf = NULL;
											ProgEnd();
										}else 
                                            if ( strstr( str,"SDTF" ) ){
                                            double Ex=0,Ny=0;
                                            float t,zoom=100; int hrLine=0; char *pV;

                                            if ( strstr( str,"DataMark" ) )
                                            {
                                                pV = strchr( str,':' );
                                                while( pV )
                                                {
                                                    if ( strstr(str,"Alpha" ) ) demHdr.kapa      = atof(pV+1); else
                                                    if ( strstr(str,"X0"    ) ) Ex               = atof(pV+1); else
                                                    if ( strstr(str,"Y0"    ) ) Ny               = atof(pV+1); else
                                                    if ( strstr(str,"DX"    ) ) demHdr.intervalX = atof(pV+1); else
                                                    if ( strstr(str,"DY"    ) ) demHdr.intervalY = atof(pV+1); else
                                                    if ( strstr(str,"Row"   ) ) demHdr.row       = int( atof(pV+1)+0.9); else
                                                    if ( strstr(str,"Col"   ) ) demHdr.column    = int( atof(pV+1)+0.9); else
                                                    if ( strstr(str,"Hzoom" ) ) zoom             = float( atof(pV+1) );
                                                    hrLine ++; fgets( str,256,fDem ); while( str[0]=='\n' ) fgets( str,256,fDem );
                                                    pV = strchr( str,':' );
                                                }
                                                fclose(fDem); fDem = fopen( lpstrPathName,"rt" );
                                                while( hrLine-- ) fgets( str,256,fDem );
                                            }else
                                            {
                                                fscanf( fDem,"%s",str );
                                                fscanf( fDem,"%s",str );
                                                fscanf( fDem,"%lf",&demHdr.kapa );
                                                fscanf( fDem,"%s",str );
                                                fscanf( fDem,"%lf",&Ex );
                                                fscanf( fDem,"%lf",&Ny );
                                                fscanf( fDem,"%lf",&demHdr.intervalX );
                                                fscanf( fDem,"%lf",&demHdr.intervalY );
                                                fscanf( fDem,"%d",&demHdr.row     ); 
                                                fscanf( fDem,"%d",&demHdr.column  ); 
                                                fscanf( fDem,"%f",&zoom );
                                            }
                                            double cosKap=cos(demHdr.kapa),sinKap=sin(demHdr.kapa); 
                                            demHdr.startX = Ex+sinKap* (demHdr.intervalY)*(demHdr.row-1);
                                            demHdr.startY = Ny-cosKap* (demHdr.intervalY)*(demHdr.row-1);
                                            
                                            SetDemHeader( demHdr ); if ( demHdr.column*demHdr.row<=0 ){ fclose( fDem ); return true; }
                                            ProgBegin( demHdr.row ); 
                                            for ( int i=demHdr.row-1;i>=0;i--,ProgStep(1) )
										    {
											    for ( int j=0;j<demHdr.column;j++ )
											    { 
												    fscanf( fDem,"%f",&t );
												    if ( t>-9998 )
                                                    {
														*(m_pGrid+demHdr.column*i+j) = zval = t/zoom;
                                                        if ( minZ>zval ) minZ=zval;
                                                        if ( maxZ<zval ) maxZ=zval;
														midZ += zval; sum += 1;
                                                    }else
                                                        *(m_pGrid+demHdr.column*i+j) = NOVALUE;
											    }
										    }
                                            ProgEnd();

                                        }else
                                        {
                                            fclose(fDem); fDem = fopen( lpstrPathName,"rt" );
                                            if ( fscanf( fDem,"%lf%lf%lf%lf%lf%d%d",&demHdr.startX,
											           &demHdr.startY,&demHdr.kapa,&demHdr.intervalX ,
											           &demHdr.intervalY,&demHdr.column,&demHdr.row)<7 )
									        { if (bMsgBox) SpMessageBox("Invalidate VirtuoZo Dem format"); fclose( fDem ); return false; }
                                            SetDemHeader( demHdr ); if ( demHdr.column*demHdr.row<=0 ){ fclose( fDem ); return true; }
									        
                                            ProgBegin( demHdr.row );
									        memset(str,0,64); fscanf( fDem,"%s",str ); 
									        if ( strchr( str,'.' )!=NULL ) // is Vir 3.2
									        {
										        m_pGrid[0] = float( atof( str ) ); if ( m_pGrid[0]<-9999.8 ) m_pGrid[0] = NOVALUE;
										        for ( int i=0;i<demHdr.row;i++,ProgStep(1) )
										        {
											        for ( int j=0;j<demHdr.column;j++ )
											        {
												        if ( i==0&&j==0 ) continue;
												        fscanf( fDem,"%f",&zval ); 
														
														if ( zval<-9999.8 ) *(m_pGrid+demHdr.column*i+j) =  NOVALUE;
                                                        else{
															*(m_pGrid+demHdr.column*i+j) = zval;	
                                                            if ( minZ>zval ) minZ= zval;
                                                            if ( maxZ<zval ) maxZ= zval;
															midZ += zval; sum += 1;
                                                        }
											        }
										        }
									        }else
									        {
										        float t = float( atof( str ) );
										        if ( t>-99998 ) m_pGrid[0] = t/10; else	m_pGrid[0] = NOVALUE;
										        
										        for ( int i=0;i<demHdr.row;i++,ProgStep(1) )
										        {
											        for ( int j=0;j<demHdr.column;j++ )
											        { 
												        if ( i==0&&j==0 ) continue;

												        fscanf( fDem,"%f",&t );
												        
												        if ( t>-9999.8 ){
                                                            *(m_pGrid+demHdr.column*i+j) = zval = t/10;
                                                            if ( minZ>zval ) minZ=zval;
                                                            if ( maxZ<zval ) maxZ=zval;
															midZ += zval; sum += 1;
                                                        }else	
                                                            *(m_pGrid+demHdr.column*i+j) = NOVALUE;
											        }
										        }
									        }
                                            ProgEnd();
                                        }
                                    }
									fclose( fDem );

									if ( sum>1 ) sum -= 1;
                                    m_midZ = (float)(midZ/sum);
									m_minZ = (float)minZ; m_maxZ = (float)maxZ;
									
									return true;
								};

	virtual bool				Save2File( LPCSTR lpstrPathName,VERSION ver=VER_32,BOOL bMsgBox=TRUE )
								{
                                    if (strlen(lpstrPathName)<3 ) return false;
									FILE *fDem = fopen( lpstrPathName,"wt" );
									if ( !fDem ){ char strMsg[512]="Can not open file:"; strcat( strMsg,lpstrPathName ); if (bMsgBox) SpMessageBox( strMsg ); return false; };
									
									char strPN[512]; strcpy(strPN,lpstrPathName); _strlwr(strPN);
									if ( ver==VER_TIF || strcmp(strrchr(strPN,'.'),".tif")==0 ){
										fclose( fDem ); fDem = NULL;
										return SaveAsGTIF( lpstrPathName,bMsgBox );
									}else
                                     if ( ver==VER_BC ){
                                            fclose( fDem ); fDem = NULL; DWORD rw;
                                            char str[512]; sprintf( str,"DEMBCH VER:1.0 Supresoft Inc. Format: Tag(512Bytes),minX(double),minY(double),Kap(double),Dx(double),Dy(double),Cols(int),Rows(int),Z(char)... %.3lf %.3lf %.3lf %.12f %.12f %d %d",
                                                m_demHdr.startX,m_demHdr.startY,m_demHdr.kapa,
                                                m_demHdr.intervalX,m_demHdr.intervalY,m_demHdr.column,m_demHdr.row );
                                            HANDLE hFile = CreateFileE( lpstrPathName,GENERIC_READ|GENERIC_WRITE ); 
                                            if ( !hFile || hFile==INVALID_HANDLE_VALUE ) return FALSE; 
                                            WriteFile( hFile,str,512,&rw,NULL ); 
                                            WriteFile( hFile,&m_demHdr,sizeof(m_demHdr),&rw,NULL );                                        
                                            char *pG,*pGrid = new char[ m_demHdr.column ];
                                            UINT sz = m_demHdr.column*sizeof(char);
                                            for ( int r=0;r<m_demHdr.row;r++ ){
                                                float *pZ = m_pGrid+r*m_demHdr.column; pG = pGrid;
                                                for( int c=0;c<m_demHdr.column;c++,pZ++,pG++ ){
                                                    if ( *pZ>0 ){ *pG = *pZ>126?126:(char)(*pZ+0.5); }
                                                    else{ *pG =  *pZ<-126?-126:(char)(*pZ-0.5); }
                                                }
                                                WriteFile( hFile,pGrid,sz,&rw,NULL );
                                            }
                                            delete pGrid;
                                            CloseHandle( hFile );  _sleep100;
                                            if ( m_demHdr.column*m_demHdr.row*sizeof(char)>(64*1024*1024) ){
                                                strcpy( str,lpstrPathName );  strcat(str,".hdr");  
                                                FILE *fHdr = fopen( str,"wt" );
                                                if ( fHdr ){
                                                    fprintf( fHdr,"%.12lf %.12lf %.12lf %.12lf %.12lf %d %d",m_demHdr.startX,m_demHdr.startY,
                                                        m_demHdr.kapa,m_demHdr.intervalX,m_demHdr.intervalY,m_demHdr.column,m_demHdr.row );
                                                    fclose(fHdr);
                                                }
                                            }
                                    }else
                                    if ( ver==VER_BS ){
                                        fclose( fDem ); fDem = NULL; DWORD rw;
                                        char str[512]; sprintf( str,"DEMBSH VER:1.0 Supresoft Inc. Format: Tag(512Bytes),minX(double),minY(double),Kap(double),Dx(double),Dy(double),Cols(int),Rows(int),Z(short)... %.3lf %.3lf %.3lf %.12f %.12f %d %d",
                                                                m_demHdr.startX,m_demHdr.startY,m_demHdr.kapa,
                                                                m_demHdr.intervalX,m_demHdr.intervalY,m_demHdr.column,m_demHdr.row );
                                        HANDLE hFile = CreateFileE( lpstrPathName,GENERIC_READ|GENERIC_WRITE ); 
                                        if ( !hFile || hFile==INVALID_HANDLE_VALUE ) return FALSE; 
                                        WriteFile( hFile,str,512,&rw,NULL ); 
                                        WriteFile( hFile,&m_demHdr,sizeof(m_demHdr),&rw,NULL );                                        
                                        short *pG,*pGrid = new short[ m_demHdr.column ];
                                        UINT sz = m_demHdr.column*sizeof(short);
                                        for ( int r=0;r<m_demHdr.row;r++ ){
                                            float *pZ = m_pGrid+r*m_demHdr.column; pG = pGrid;
                                            for( int c=0;c<m_demHdr.column;c++,pZ++,pG++ ) *pG = (short)(*pZ+0.5);
                                            WriteFile( hFile,pGrid,sz,&rw,NULL );
                                        }
                                        delete pGrid;
                                        CloseHandle( hFile );  _sleep100;
                                        if ( m_demHdr.column*m_demHdr.row*sizeof(short)>(64*1024*1024) ){
                                            strcpy( str,lpstrPathName );  strcat(str,".hdr");  
                                            FILE *fHdr = fopen( str,"wt" );
                                            if ( fHdr ){
                                                fprintf( fHdr,"%.12lf %.12lf %.12lf %.12lf %.12lf %d %d",m_demHdr.startX,m_demHdr.startY,
                                                         m_demHdr.kapa,m_demHdr.intervalX,m_demHdr.intervalY,m_demHdr.column,m_demHdr.row );
                                                fclose(fHdr);
                                            }
                                        }
                                    }else
                                    if ( ver==VER_BN ){
                                        fclose( fDem ); fDem = NULL; DWORD rw;
                                        char str[512]; sprintf( str,"DEMBIN VER:1.0 Supresoft Inc. Format: Tag(512Bytes),minX(double),minY(double),Kap(double),Dx(double),Dy(double),Cols(int),Rows(int),Z(float)... %.3lf %.3lf %.3lf %.12f %.12f %d %d",
                                                                m_demHdr.startX,m_demHdr.startY,m_demHdr.kapa,
                                                                m_demHdr.intervalX,m_demHdr.intervalY,m_demHdr.column,m_demHdr.row );
                                        HANDLE hFile = CreateFileE( lpstrPathName,GENERIC_READ|GENERIC_WRITE ); 
                                        if ( !hFile || hFile==INVALID_HANDLE_VALUE ) return FALSE; 
                                        WriteFile( hFile,str,512,&rw,NULL ); 
                                        WriteFile( hFile,&m_demHdr,sizeof(m_demHdr),&rw,NULL );
                                        BYTE *pGrid = (BYTE*)m_pGrid; UINT sz = m_demHdr.column*sizeof(float);
                                        for ( int r=0;r<m_demHdr.row;r++,pGrid+=sz ) WriteFile( hFile,pGrid,sz,&rw,NULL );
                                        CloseHandle( hFile );  _sleep100;
                                        if ( m_demHdr.column*m_demHdr.row*sizeof(float)>(64*1024*1024) ){
                                            strcpy( str,lpstrPathName );  strcat(str,".hdr");  
                                            FILE *fHdr = fopen( str,"wt" );
                                            if ( fHdr ){
                                                fprintf( fHdr,"%.12lf %.12lf %.12lf %.12lf %.12lf %d %d",m_demHdr.startX,m_demHdr.startY,
                                                         m_demHdr.kapa,m_demHdr.intervalX,m_demHdr.intervalY,m_demHdr.column,m_demHdr.row );
                                                fclose(fHdr);
                                            }
                                        }
                                    }else{
                                        fprintf( fDem,"%.12lf %.12lf %.12lf %.12lf %.12lf %d %d\n",m_demHdr.startX,
													    m_demHdr.startY,m_demHdr.kapa,m_demHdr.intervalX,
													    m_demHdr.intervalY,m_demHdr.column,m_demHdr.row );
									    
                                        ProgBegin(m_demHdr.row);
                                        for ( int i=0;i<m_demHdr.row;i++,ProgStep(1) )
									    {
											int r,j;
										    for (  r=0,j=0;j<m_demHdr.column;j++,r++ )
										    { 
											    if ( r==10 ){ fprintf( fDem,"\n" ); r=0; }
											    if ( *(m_pGrid+m_demHdr.column*i+j)==NOVALUE )
											    { 
												    if (ver==VER_32) fprintf( fDem,"-9999.9 " );  
												    else fprintf( fDem,"-99999 " );  
											    }else
											    { 
												    if (ver==VER_32) fprintf( fDem,"%.3f ",*(m_pGrid+m_demHdr.column*i+j) ); 
												    else fprintf( fDem,"%.0f ",(*(m_pGrid+m_demHdr.column*i+j)*10) ); 
											    }
										    }
										    if (r<=10) fprintf( fDem,"\n");
									    }
                                        ProgEnd();
                                    }
									if(fDem) fclose(fDem);
									return true;
								};


    ///////////////////////////////////////////////////////////////////////////
    //  Following is for File Access
    ///////////////////////////////////////////////////////////////////////////
    virtual BOOL                Open(LPCSTR lpstrPathName,int mode,SPDEMHDR *pHdr=NULL){                                    
                                    DWORD rw; char str[512] = "DEMBIN VER:1.0 Supresoft Inc. Format: Tag(512Bytes),minX(double),minY(double),Kap(double),Dx(double),Dy(double),Cols(int),Rows(int),Z(float)...";
                                    if ( mode==modeCreate ){
                                        if ( pHdr ) m_demHdr = *pHdr;
                                        sprintf( str,"DEMBIN VER:1.0 Supresoft Inc. Format: Tag(512Bytes),minX(double),minY(double),Kap(double),Dx(double),Dy(double),Cols(int),Rows(int),Z(float)... %.3lf %.3lf %.3lf %.3f %.3f %d %d",
                                                     m_demHdr.startX,m_demHdr.startY,m_demHdr.kapa,
                                                     m_demHdr.intervalX,m_demHdr.intervalY,m_demHdr.column,m_demHdr.row );                                        

                                        m_hFile = CreateFileE( lpstrPathName,GENERIC_READ|GENERIC_WRITE );
                                        if ( !m_hFile || m_hFile==INVALID_HANDLE_VALUE ) return FALSE;   
                                        WriteFile( m_hFile,str,512,&rw,NULL ); 
                                        WriteFile( m_hFile,&m_demHdr,sizeof(m_demHdr),&rw,NULL );                                         
                                    
                                    }else
                                    {
                                        if (mode==modeRead){                                        
                                            FILE *fDem = fopen( lpstrPathName,"rt" ); 
									        if ( !fDem ){ char strMsg[512]="Can not open file:"; strcat( strMsg,lpstrPathName ); SpMessageBox( strMsg ); return FALSE; };
                                            fscanf( fDem,"%s",str ); fclose(fDem);
                                            DWORD nFlag= FILE_FLAG_RANDOM_ACCESS;
                                            if ( strncmp(str,"DEMBIN",6)!=0 ){
                                                ::GetTempPath( 512,str ); ::GetTempFileName(str,"dem",0,str );
                                                Covert2BEM( lpstrPathName,str ); 
                                                nFlag = FILE_FLAG_RANDOM_ACCESS|FILE_FLAG_DELETE_ON_CLOSE;
                                            }else
                                                strcpy( str,lpstrPathName );
                                        
                                            m_hFile = CreateFileE( str,GENERIC_READ );
                                            if ( !m_hFile || m_hFile==INVALID_HANDLE_VALUE ) return FALSE; 
                                            ReadFile( m_hFile,str,512,&rw,NULL ); 
                                            ReadFile( m_hFile,&m_demHdr,sizeof(m_demHdr),&rw,NULL );
                                            if ( pHdr ) *pHdr = m_demHdr;
                                        }else{
                                            if ( !IsExist(lpstrPathName) ){
                                                if ( pHdr ) m_demHdr = *pHdr;
                                                sprintf( str,"DEMBIN VER:1.0 Supresoft Inc. Format: Tag(512Bytes),minX(double),minY(double),Kap(double),Dx(double),Dy(double),Cols(int),Rows(int),Z(float)... %.3lf %.3lf %.3lf %.3f %.3f %d %d",
                                                     m_demHdr.startX,m_demHdr.startY,m_demHdr.kapa,
                                                     m_demHdr.intervalX,m_demHdr.intervalY,m_demHdr.column,m_demHdr.row );
                                                m_hFile = CreateFileE( lpstrPathName,GENERIC_READ|GENERIC_WRITE );
                                                if ( !m_hFile || m_hFile==INVALID_HANDLE_VALUE ) return FALSE;   
                                                WriteFile( m_hFile,str,512,&rw,NULL ); 
                                                WriteFile( m_hFile,&m_demHdr,sizeof(m_demHdr),&rw,NULL );
                                            }else{
                                                FILE *fDem = fopen( lpstrPathName,"rt" ); 
									            if ( !fDem ){ char strMsg[512]="Can not open file:"; strcat( strMsg,lpstrPathName ); SpMessageBox( strMsg ); return FALSE; };
                                                fscanf( fDem,"%s",str ); fclose(fDem);
                                                if ( strncmp(str,"DEMBIN",6)!=0 ){
                                                    ::GetTempPath( 512,str ); ::GetTempFileName(str,"dem",0,str );
                                                    Covert2BEM( lpstrPathName,str ); 
                                                    ::CopyFile( str,lpstrPathName,FALSE ); ::DeleteFile( str );
                                                }
                                                m_hFile = CreateFileE( lpstrPathName,GENERIC_READ|GENERIC_WRITE );
                                                if ( !m_hFile || m_hFile==INVALID_HANDLE_VALUE ) return FALSE;  
                                                ReadFile( m_hFile,str,512,&rw,NULL ); 
                                                ReadFile( m_hFile,&m_demHdr,sizeof(m_demHdr),&rw,NULL );
                                                if ( pHdr ) *pHdr = m_demHdr;
                                            }
                                        }
                                    }                                      
                                    return TRUE;
                                };
    virtual void                Close(){
                                    if (m_hFile) CloseHandle(m_hFile); m_hFile=NULL;
                                };
    virtual BOOL                ReadRow(float *pBuf,int row){
                                    if (!m_hFile) return FALSE;
                                    DWORD rw; LARGE_INTEGER li; li.HighPart=0; 
                                    li.QuadPart = 512+sizeof(m_demHdr)+LONGLONG(row)*LONGLONG(m_demHdr.column)*sizeof(float); 
                                    SetFilePointer( m_hFile,li.LowPart,&li.HighPart,FILE_BEGIN );                                     
                                    return ReadFile( m_hFile,pBuf,m_demHdr.column*sizeof(float),&rw,NULL );                                    
                                };
    virtual BOOL                WriteRow(float *pBuf,int row){
                                    if (!m_hFile) return FALSE;
                                    DWORD rw; LARGE_INTEGER li; li.HighPart=0; 
                                    li.QuadPart = 512+sizeof(m_demHdr)+LONGLONG(row)*LONGLONG(m_demHdr.column)*sizeof(float); 
                                    SetFilePointer( m_hFile,li.LowPart,&li.HighPart,FILE_BEGIN );    
                                    return WriteFile( m_hFile,pBuf,m_demHdr.column*sizeof(float),&rw,NULL );  
                                };
    virtual BOOL                ReadBlock(float *pBuf,int sRow,int sCol,int rows,int cols){
                                    if ( m_hFile==NULL ) return FALSE;
                                    DWORD rw; LARGE_INTEGER li; li.HighPart=0; 
                                    for( int r=0;r<rows;r++,pBuf +=cols ){
                                        li.QuadPart = 512+sizeof(m_demHdr)+(LONGLONG(sRow+r)*LONGLONG(m_demHdr.column)+sCol)*sizeof(float); 
                                        SetFilePointer( m_hFile,li.LowPart,&li.HighPart,FILE_BEGIN );
                                        ReadFile( m_hFile,pBuf,cols*sizeof(float),&rw,NULL ); 
                                    }
                                    return TRUE;
                                };
    virtual BOOL                WriteBlock(float *pBuf,int sRow,int sCol,int rows,int cols){
                                    if ( m_hFile==NULL ||sRow>m_demHdr.row || sCol>m_demHdr.column ) return FALSE;
                                    if ( sRow<0 ){ pBuf += (-sRow)*cols*sizeof(float); rows = sRow; sRow = 0;  }
                                    if ( sRow+rows>m_demHdr.row ) rows = m_demHdr.row-sRow;
                                    
                                    DWORD rw; LARGE_INTEGER li; li.HighPart=0; int skipCol=0,wCol=0;
                                    li.QuadPart = 512+sizeof(m_demHdr)+(LONGLONG(sRow)*LONGLONG(m_demHdr.column)+sCol)*sizeof(float);
                                    SetFilePointer( m_hFile,li.LowPart,&li.HighPart,FILE_BEGIN );
                                    for( int r=0;r<rows;r++,pBuf +=cols ){
                                        
                                        if ( sCol<0 ) skipCol = (-sCol);
                                        if ( (max(skipCol,sCol)+cols)>m_demHdr.column )
                                             wCol = m_demHdr.column-max(skipCol,sCol);
                                        else wCol = cols-skipCol;

                                        WriteFile( m_hFile,pBuf+skipCol*sizeof(float),wCol*sizeof(float),&rw,NULL );
                                        li.HighPart = 0; li.LowPart = (m_demHdr.column-wCol)*sizeof(float);
                                        SetFilePointer( m_hFile,li.LowPart,&li.HighPart,FILE_CURRENT );
                                    }
                                    return TRUE;
                                };
    ///////////////////////////////////////////////////////////////////////////
    //File Access Over
    ///////////////////////////////////////////////////////////////////////////
    static bool                 CovertLargDem( LPCSTR lpstrPathName,LPCSTR lpstrFmt,LPCSTR lpstrPathNameBEM ){
                                    char strFmtS[64];  GetDemHdr( lpstrPathName,FALSE,strFmtS );
                                    if ( strcmp(strFmtS,FMT_DEM)==0 && strcmp(lpstrFmt,FMT_BEM)==0 )
                                         return Covert2BEM( lpstrPathName,lpstrPathNameBEM ); 
                                    return false;
                                };
    static bool                 Covert2BEM( LPCSTR lpstrPathName,LPCSTR lpstrPathNameBEM,BOOL bMsgBox=TRUE )
                                {       
                                    WIN32_FIND_DATA find_data; HANDLE find_handle = ::FindFirstFile(lpstrPathName,&find_data);
			                        if ( find_handle==INVALID_HANDLE_VALUE ){ ::FindClose(find_handle); return false; }
                                    if ( find_data.nFileSizeLow<512*1024*1024 ){   
                                        CSpDem demFile; if ( !demFile.Load4File( lpstrPathName ) ) return false;
                                        return demFile.Save2File( lpstrPathNameBEM,VER_BN );
                                    }else
                                    {
                                        char str[512];  strcpy( str,lpstrPathNameBEM );  strcat(str,".hdr");       
                                        int sum=0; SPDEMHDR demHdr; memset(&demHdr,0,sizeof(demHdr)); float z=0; 
                                        FILE *fBem = fopen( lpstrPathNameBEM,"wb" ); if ( !fBem ){ char strMsg[512]="Can not open file:"; strcat( strMsg,lpstrPathNameBEM ); if (bMsgBox) SpMessageBox( strMsg ); return false; };
                                        float *bufZ=new float[1024*1024]; 
                                    
                                        FILE *fDem = fopen( lpstrPathName   ,"rt" ); if ( !fDem ){ char strMsg[512]="Can not open file:"; strcat( strMsg,lpstrPathName    ); if (bMsgBox) SpMessageBox( strMsg ); fclose(fBem);  return false; };
                                        if ( fscanf( fDem,"%lf%lf%lf%lf%lf%d%d",&demHdr.startX,
											         &demHdr.startY,&demHdr.kapa,&demHdr.intervalX ,
											         &demHdr.intervalY,&demHdr.column,&demHdr.row )<7 )
									    { if (bMsgBox) SpMessageBox("Invalidate VirtuoZo Dem format"); fclose( fDem ); fclose(fBem); return false; }
                                        if ( demHdr.intervalX<=0 || demHdr.intervalY<=0 ||demHdr.column<=0 || demHdr.row<=0 ){ if (bMsgBox) SpMessageBox("Invalidate VirtuoZo Dem format"); fclose(fDem); fclose(fBem); return false; }

                                        memset( str,0,sizeof(str) );
                                        strcpy( str,"DEMBIN VER:1.0 Supresoft Inc. Format: Tag(512Bytes),minX(double),minY(double),Kap(double),Dx(double),Dy(double),Cols(int),Rows(int),Z(float)..." );
                                        fwrite( str,512,1,fBem );
                                        fwrite( &demHdr,sizeof(demHdr),1,fBem );
                                    
                                        FILE *fHdr = fopen( str,"wt" );
                                        if ( fHdr ){
                                            fprintf( fHdr,"%.12lf %.12lf %.12lf %.12lf %.12lf %d %d",demHdr.startX,demHdr.startY,
                                                     demHdr.kapa,demHdr.intervalX,demHdr.intervalY,demHdr.column,demHdr.row );
                                            fclose(fHdr);
                                        }
                                    
                                        memset( str,0,64 );
                                        fscanf( fDem,"%s",str ); 
                                        if ( strchr( str,'.' )!=NULL ) // is Vir 3.2
                                        {
                                            z = float( atof( str ) ); bufZ[sum++]=z;
                                            for ( int i=0;i<demHdr.row;i++ )
										    {
											    for ( int j=0;j<demHdr.column;j++ )
											    {
												    if ( i==0&&j==0 ) continue;
                                                    
                                                    fscanf( fDem,"%f",&z );
                                                    if ( z<-9999.8 ) z = NOVALUE;
                                                
                                                    bufZ[sum++] = z;
                                                    if ( sum>=1024*1024 ){  fwrite( bufZ,sizeof(z)*sum,1,fBem ); sum=0; }
											    }
										    }
                                        
                                        }else
                                        {
                                            z = float( atof( str ) )/10.f; bufZ[sum++]=z;
                                            for ( int i=0;i<demHdr.row;i++ )
										    {
											    for ( int j=0;j<demHdr.column;j++ )
											    { 
												    if ( i==0&&j==0 ) continue;
                                                
												    fscanf( fDem,"%f",&z );
                                                
												    if ( z<-99998 )  z = NOVALUE;
                                                    else z /= 10.f; 

                                                    bufZ[sum++]=z;
                                                    if ( sum>=1024*1024 ){  fwrite( bufZ,sizeof(z)*sum,1,fBem ); sum=0; }
											    }
										    }
                                        }
                                        fwrite( bufZ,sizeof(z)*sum,1,fBem ); sum=0; 
                                        delete bufZ;

                                        fclose(fDem);                                    
                                        fclose(fBem);

                                        return true;
                                    }
                                };
    inline  bool                SaveAsNSDTF(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE){
                                    if (strlen(lpstrPathName)<3 ) return false;
									FILE *fDem = fopen( lpstrPathName,"wt" );
									if ( !fDem ){ char strMsg[512]="Can not open file:"; strcat( strMsg,lpstrPathName ); if (bMsgBox) SpMessageBox( strMsg ); return false; };
									
									double cosKap=cos(m_demHdr.kapa),sinKap=sin(m_demHdr.kapa);
									double ULX = ( m_demHdr.startX-sinKap*m_demHdr.intervalY*(m_demHdr.row-1) );
									double ULY = ( m_demHdr.startY+cosKap*m_demHdr.intervalY*(m_demHdr.row-1) );
									
									fprintf( fDem,"NSDTF-DEM\n" );
									fprintf( fDem,"1.0\n" );
									fprintf( fDem,"M\n" );
									fprintf( fDem,"%.12lf\n",m_demHdr.kapa );
									fprintf( fDem,"0.0\n" );
									fprintf( fDem,"%.12lf\n",ULX );
									fprintf( fDem,"%.12lf\n",ULY );
									fprintf( fDem,"%.12lf\n",m_demHdr.intervalX );
									fprintf( fDem,"%.12lf\n",m_demHdr.intervalY );
									fprintf( fDem,"%d\n",m_demHdr.row );
									fprintf( fDem,"%d\n",m_demHdr.column );
									fprintf( fDem,"1000\n" );
					    
                                    ProgBegin(m_demHdr.row);
                                    for ( int i=m_demHdr.row-1;i>=0;i--,ProgStep(1) )
									{
										int r, j;
										for (  r=0,j=0;j<m_demHdr.column;j++,r++ )
										{ 
											if ( r==10 ){ fprintf( fDem,"\n" ); r=0; }
											if ( *(m_pGrid+m_demHdr.column*i+j)==NOVALUE )
											{ 
												fprintf( fDem,"-99999 " );  
											}else
											{ 
												fprintf( fDem,"%.0f ",(*(m_pGrid+m_demHdr.column*i+j)*1000 +0.5f) );
											}
										}
										if (r<=10) fprintf( fDem,"\n");
									}
                                    ProgEnd(); fclose(fDem);

									return true;
                                };
	inline	bool				SaveAsCSDTF(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE){
									if (strlen(lpstrPathName)<3 ) return false;
									FILE *fDem = fopen( lpstrPathName,"wt" );
									if ( !fDem ){ char strMsg[512]="Can not open file:"; strcat( strMsg,lpstrPathName ); if (bMsgBox) SpMessageBox( strMsg ); return false; };
									
									double cosKap=cos(m_demHdr.kapa),sinKap=sin(m_demHdr.kapa);
									double ULX = ( m_demHdr.startX-sinKap*m_demHdr.intervalY*(m_demHdr.row-1) );
									double ULY = ( m_demHdr.startY+cosKap*m_demHdr.intervalY*(m_demHdr.row-1) );
									
									fprintf( fDem,"DataMark:CSDTF-DEM\n" );
									fprintf( fDem,"Version:1.0\n" );
									fprintf( fDem,"Unit:M\n" );
									fprintf( fDem,"Alpha:%.12lf\n",m_demHdr.kapa );
									fprintf( fDem,"Compress:0\n" );
									fprintf( fDem,"X0:%.12lf\n",ULX );
									fprintf( fDem,"Y0:%.12lf\n",ULY );
									fprintf( fDem,"DX:%.12lf\n",m_demHdr.intervalX );
									fprintf( fDem,"DY:%.12lf\n",m_demHdr.intervalY );
									fprintf( fDem,"Row:%d\n",m_demHdr.row );
									fprintf( fDem,"Col:%d\n",m_demHdr.column );
									fprintf( fDem,"ValueType:Integer\n" );
									fprintf( fDem,"Hzoom:100\n" );
									fprintf( fDem,"MinV:%.0f\n",(m_minZ*100-0.5f) );
									fprintf( fDem,"MaxV:%.0f\n",(m_maxZ*100+0.5f) );
					    
                                    ProgBegin(m_demHdr.row);
                                    for ( int i=m_demHdr.row-1;i>=0;i--,ProgStep(1) )
									{
										int r,j;
										for (  r=0,j=0;j<m_demHdr.column;j++,r++ )
										{ 
											if ( r==10 ){ fprintf( fDem,"\n" ); r=0; }
											if ( *(m_pGrid+m_demHdr.column*i+j)==NOVALUE )
											{ 
												fprintf( fDem,"-99999 " );  
											}else
											{ 
												fprintf( fDem,"%.0f ",(*(m_pGrid+m_demHdr.column*i+j)*100 +0.5f) ); 
											}
										}
										if (r<=10) fprintf( fDem,"\n");
									}
                                    ProgEnd(); fclose(fDem);

									return true;
								};
	inline  bool				SaveAsArcGRID(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE){
									if (strlen(lpstrPathName)<3 ) return false;
									FILE *fDem = fopen( lpstrPathName,"wt" );
									if ( !fDem ){ char strMsg[512]="Can not open file:"; strcat( strMsg,lpstrPathName ); if (bMsgBox) SpMessageBox( strMsg ); return false; };

									double cosKap=cos(m_demHdr.kapa),sinKap=sin(m_demHdr.kapa);
									double ULX = ( m_demHdr.startX-sinKap*m_demHdr.intervalY*(m_demHdr.row-1) );
									double ULY = ( m_demHdr.startY+cosKap*m_demHdr.intervalY*(m_demHdr.row-1) );
									
									fprintf(fDem,"ncols			%d\n" ,m_demHdr.column	  );
									fprintf(fDem,"nrows			%d\n" ,m_demHdr.row		  );									
									fprintf(fDem,"cellsize		%.12f\n" ,m_demHdr.intervalX );
									fprintf(fDem,"xllcenter		%.12lf\n",m_demHdr.startX	  );
									fprintf(fDem,"yllcenter		%.12lf\n",m_demHdr.startY	  );
									fprintf(fDem,"NODATA_value  %d\n" ,NOVALUE			  );

									ProgBegin(m_demHdr.row);
                                    for ( int i=m_demHdr.row-1;i>=0;i--,ProgStep(1) )
									{
										int r,j;
										for (  r=0,j=0;j<m_demHdr.column;j++,r++ )
										{
											if ( r==10 ){ fprintf( fDem,"\n" ); r=0; }
											if ( *(m_pGrid+m_demHdr.column*i+j)==NOVALUE )
											{ 
												fprintf( fDem,"-99999 " );  
											}else
											{ 
												fprintf( fDem,"%.0f ",(*(m_pGrid+m_demHdr.column*i+j)*100) ); 
											}
										}
										if (r<=10) fprintf( fDem,"\n");
									}
                                    ProgEnd(); fclose(fDem);

									return true;
								};
	inline  bool				SaveAsBIL(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE){
									if (strlen(lpstrPathName)<3 ) return false;
									FILE *fDem = fopen( lpstrPathName,"wb" );
									if ( !fDem ){ char strMsg[512]="Can not open file:"; strcat( strMsg,lpstrPathName ); if (bMsgBox) SpMessageBox( strMsg ); return false; };

									ProgBegin(m_demHdr.row); UINT *pRow = new UINT[m_demHdr.column];
                                    for ( int i=m_demHdr.row-1;i>=0;i--,ProgStep(1) ){
										for ( int j=0;j<m_demHdr.column;j++ ){
											pRow[j] = *(m_pGrid+m_demHdr.column*i+j)==NOVALUE?0:int( *(m_pGrid+m_demHdr.column*i+j)*10 +0.5 )+1000;
										}
										fwrite(pRow,m_demHdr.column,sizeof(UINT),fDem );
									}
									delete pRow;
                                    ProgEnd(); fclose(fDem);

									double cosKap=cos(m_demHdr.kapa),sinKap=sin(m_demHdr.kapa);
									double ULX = ( m_demHdr.startX-sinKap*m_demHdr.intervalY*(m_demHdr.row-1) );
									double ULY = ( m_demHdr.startY+cosKap*m_demHdr.intervalY*(m_demHdr.row-1) );
									double Dx =  cosKap*m_demHdr.intervalX; 
									double Rx =  sinKap*m_demHdr.intervalX; 
									double Ry =  sinKap*m_demHdr.intervalY; 
									double Dy = -cosKap*m_demHdr.intervalY; 
									
									char strHdr[256]; strcpy( strHdr,lpstrPathName ); strcpy( strrchr(strHdr,'.'),".hdr" );
									FILE *fHdr = fopen( strHdr,"wt" );
									if ( !fHdr ){ char strMsg[512]="Can not open file:"; strcat( strMsg,strHdr ); if (bMsgBox) SpMessageBox( strMsg ); return false; };

									fprintf(fHdr,"BYTEORDER		I\n");
									fprintf(fHdr,"LAYOUT		BIL\n");
									fprintf(fHdr,"NROWS			%d\n",m_demHdr.row		);
									fprintf(fHdr,"NCOLS			%d\n",m_demHdr.column	);
									fprintf(fHdr,"NBANDS		1\n");
									fprintf(fHdr,"NBITS			%d\n",32 );
									fprintf(fHdr,"BANDROWBYTE		%d\n",4*m_demHdr.column );
									fprintf(fHdr,"TOTALROWBYTES		%d\n",4*m_demHdr.column );
									fprintf(fHdr,"BANDCOLBYTE		0\n");
									fprintf(fHdr,"ULXMAP			%.12lf\n",ULX );
									fprintf(fHdr,"ULYMAP			%.12lf\n",ULY );
									fprintf(fHdr,"XDIM				%.12lf\n" ,m_demHdr.intervalX );
									fprintf(fHdr,"YDIM				%.12lf\n" ,m_demHdr.intervalY );
									fclose(fHdr);

									strcpy( strHdr,lpstrPathName ); strcpy( strrchr(strHdr,'.'),".blw" );
									FILE *fBlw = fopen( strHdr,"wt" );
									if ( !fBlw ){ char strMsg[512]="Can not open file:"; strcat( strMsg,strHdr ); if (bMsgBox) SpMessageBox( strMsg ); return false; };
									fprintf( fBlw,"\t%lf\n",Dx  );
									fprintf( fBlw,"\t%lf\n",Rx  );
									fprintf( fBlw,"\t%lf\n",Ry  );
									fprintf( fBlw,"\t%lf\n",Dy  );
									fprintf( fBlw,"\t%lf\n",ULX );
									fprintf( fBlw,"\t%lf\n",ULY );
									fclose(fBlw); 

									return true;
								};
	inline  bool				SaveAsGJB(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE){
									if (strlen(lpstrPathName)<3 ) return false;
									const char* p = strrchr(lpstrPathName, '.'); if (p==NULL) return FALSE;
									BOOL bGeodetic = (_stricmp(p, ".MDM") == 0 || _stricmp(p, ".DEM") == 0);
									BOOL bGaussPrj = (_stricmp(p, ".MGM") == 0 || _stricmp(p, ".GEM") == 0);
									if (!bGeodetic && !bGaussPrj) { if (bMsgBox) {char strMsg[512]="The extension of GJB DEM file must be .MGM or .GEM or .MDM or .DEM!"; SpMessageBox(strMsg);} return false; };
									struct HEAD{
										char ver[8], type[8], pathname[256];
										char mapTool[256], edtTool[256];
										char precision[8], mangNote[1024];
										double xCutConst, yCutConst;
										double minX, minY, maxX, maxY;
										double dX, dY;
										int secNum;
									};
									struct SECTION{
										int no;
										double sx, sy, ex, ey;
										int pn;
										float *elevation;
									};

									HEAD hdr; memset(&hdr, 0, sizeof(HEAD));
									strcpy(hdr.ver, "2.000");
									strcpy(hdr.type,p+1); _strupr(hdr.type);
									strcpy(hdr.pathname, lpstrPathName);
									strcpy(hdr.mapTool, "VirtuoZo");
									strcpy(hdr.edtTool, "Supresoft VirtuoZo");
									hdr.minX = m_demHdr.startX;
									hdr.minY = m_demHdr.startY;
									hdr.maxX = m_demHdr.startX + (m_demHdr.column-1)*m_demHdr.intervalX;
									hdr.maxY = m_demHdr.startY + (m_demHdr.row-1)*m_demHdr.intervalY;
									hdr.dX = m_demHdr.intervalX; if (bGeodetic) Deg2DegMinSec(&hdr.dX);
									hdr.dY = m_demHdr.intervalY; if (bGeodetic) Deg2DegMinSec(&hdr.dY);
									hdr.secNum = 0;
		
									if (bGeodetic && (!IsGeodetic(hdr.minX,true) || !IsGeodetic(hdr.maxX,true) || !IsGeodetic(hdr.minY,false) || !IsGeodetic(hdr.maxY,false))){
										if (bMsgBox) {char strMsg[512]="Can't be saved as geodetic coordinate DEM(*.dem or *.mdm)!"; SpMessageBox(strMsg);} return false;
									}
									FILE *fDem = fopen( lpstrPathName,"wb+" );
									if ( !fDem ){ char strMsg[512]="Can not open file:"; strcat( strMsg,lpstrPathName ); if (bMsgBox) SpMessageBox( strMsg ); return false; };
									fwrite( hdr.ver,	  sizeof(hdr.ver),		1,fDem);
									fwrite( hdr.type,	  sizeof(hdr.type),		1,fDem);
									fwrite( hdr.pathname, sizeof(hdr.pathname),	1,fDem);
									fwrite( hdr.mapTool,  sizeof(hdr.mapTool),	1,fDem);
									fwrite( hdr.edtTool,  sizeof(hdr.edtTool),	1,fDem);
									fwrite( hdr.precision,sizeof(hdr.precision),1,fDem);
									fwrite( hdr.mangNote, sizeof(hdr.mangNote),	1,fDem);
									fwrite(&hdr.xCutConst,sizeof(hdr.xCutConst),1,fDem);
									fwrite(&hdr.yCutConst,sizeof(hdr.yCutConst),1,fDem);
									fwrite(&hdr.minX,	  sizeof(hdr.minX),		1,fDem);
									fwrite(&hdr.minY,	  sizeof(hdr.minY),		1,fDem);
									fwrite(&hdr.maxX,	  sizeof(hdr.maxX),		1,fDem);
									fwrite(&hdr.maxY,	  sizeof(hdr.maxY),		1,fDem);
									fwrite(&hdr.dX,		  sizeof(hdr.dX),		1,fDem);
									fwrite(&hdr.dY,		  sizeof(hdr.dY),		1,fDem);
									fwrite(&hdr.secNum,	  sizeof(hdr.secNum),	1,fDem);

									SECTION sec; memset(&sec, 0, sizeof(SECTION)); sec.no = 1;
									float* pBuf = new float[m_demHdr.column]; memset(pBuf, 0, sizeof(float)*m_demHdr.column);

									int i =0, j =0, sc = 0, ec = 0, nc = 0;
									for (i = m_demHdr.row-1, sec.sy = hdr.maxY; i > -1; i--, sec.sy -= hdr.dY)
									{
										sec.ey = sec.sy;
										float *pRow = m_pGrid + (m_demHdr.column*i), *pR = NULL;
										sc = -1;
										for (j = 0, pR = pRow; j < m_demHdr.column; j++, pR++)
										{
											bool bLastC = (j==m_demHdr.column-1);
											if (sc != -1 && (*pR == NOVALUE||bLastC))
											{
												ec = j - (bLastC?0:1);
												nc = ec - sc + 1;
												sec.sx = hdr.minX + hdr.dX*sc;
												sec.ex = hdr.minX + hdr.dX*ec;
												sec.pn = nc;
												memcpy(pBuf, pRow+sc, sizeof(float)*nc);

												fwrite(&sec.no, sizeof(sec.no), 1, fDem);
												fwrite(&sec.sx, sizeof(sec.sx), 1, fDem);
												fwrite(&sec.sy, sizeof(sec.sy), 1, fDem);
												fwrite(&sec.ex, sizeof(sec.ex), 1, fDem);
												fwrite(&sec.ey, sizeof(sec.ey), 1, fDem);
												fwrite(&sec.pn, sizeof(sec.pn), 1, fDem);
												fwrite( pBuf, sizeof(float)*nc, 1, fDem);

												sec.no++;
												sc = -1;
											}
											if (*pR != NOVALUE && sc == -1)
											{
												sc = j;
											}
										}
									}			
									hdr.secNum = sec.no-1;
									delete [] pBuf; pBuf = NULL;
		
									long pos = 8*2+256*3+8+1024+8*8;
									fseek(fDem, pos, SEEK_SET);
									fwrite(&hdr.secNum, sizeof(hdr.secNum), 1, fDem);
									fclose(fDem);
									
									return true;
								};
	inline  bool				Load4GTIF(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE,BOOL bJustHdr=FALSE){
									#define	_TIFFTAG_SUBFILETYPE		254	// subfile data descriptor 
									#define	_TIFFTAG_IMAGEWIDTH			256	// image width in pixels 
									#define	_TIFFTAG_IMAGELENGTH		257	// image height in pixels 
									#define	_TIFFTAG_BITSPERSAMPLE		258	// bits per channel (sample) 
									#define	_TIFFTAG_COMPRESSION		259	// data compression technique 
									#define	    COMPRESSION_NONE		1	// dump mode 
									#define	_TIFFTAG_PHOTOMETRIC		262	// photometric interpretation 
									#define	    PHOTOMETRIC_MINISBLACK	1	// min value is black 
									#define	_TIFFTAG_STRIPOFFSETS		273	// offsets to data strips 
									#define	_TIFFTAG_ORIENTATION		274	// +image orientation 
									#define	    ORIENTATION_TOPLEFT		1	// row 0 top, col 0 lhs 
									#define	_TIFFTAG_SAMPLESPERPIXEL	277	// samples per pixel 
									#define	_TIFFTAG_ROWSPERSTRIP		278	// rows per strip of data 
									#define	_TIFFTAG_STRIPBYTECOUNTS	279	// bytes counts for strips 
                                    #define	TIFFTAG_XRESOLUTION		282	/* pixels/resolution in x */
                                    #define	TIFFTAG_YRESOLUTION		283	/* pixels/resolution in y */
                                    #define	TIFFTAG_PLANARCONFIG		284	/* storage organization */
                                    #define	TIFFTAG_RESOLUTIONUNIT		296	/* units of resolutions */
                                    #define	    RESUNIT_NONE		1	/* no meaningful units */
                                    #define	    RESUNIT_INCH		2	/* english */
                                    #define	    RESUNIT_CENTIMETER		3	/* metric */
                                    #define	TIFFTAG_TILEWIDTH		322	/* !rows/data tile */
                                    #define	TIFFTAG_TILELENGTH		323	/* !cols/data tile */
                                    #define TIFFTAG_TILEOFFSETS		324	/* !offsets to data tiles */
                                    #define TIFFTAG_TILEBYTECOUNTS	325	/* !byte counts for tiles */
									#define	_TIFFTAG_SAMPLEFORMAT		339	// !data sample format 
									#define	    SAMPLEFORMAT_UINT		1	// !unsigned integer data ()
									#define	    SAMPLEFORMAT_INT		2	// !signed integer data ()
									#define		SAMPLEFORMAT_IEEEFP		3	// !IEEE floating point data (float)
									#define	_TIFF_SHORT					3	// 16-bit unsigned integer (unsigned short)
									#define _TIFF_LONG					4	// 32-bit unsigned integer (unsigned int )
									
                                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////    
                                    // GeoTiff    
                                    #define TIFFTAG_GEOPIXELSCALE        33550    // double xyzScale[3]
                                    #define TIFFTAG_GEOTIEPOINTS         33922    // double tiePts[][6]
                                    #define TIFFTAG_GEOTRANSMATRIX       34264    // double matrix[16]
                                    #define TIFFTAG_GEOKEYDIRECTORY      34735    // short geoKey[][4]
        
                                    #define   GTModelTypeGeoKey           1024 
                                            typedef enum {
                                                ModelTypeProjected  = 1,  /* Projection Coordinate System */
                                                ModelTypeGeographic = 2,  /* Geographic latitude-longitude System */
                                                ModelTypeGeocentric = 3,  /* Geocentric (X,Y,Z) Coordinate System */
                                            }modeltype_t;    
                                    #define   GTRasterTypeGeoKey          1025
                                    #define         RasterPixelIsArea        1
                                    #define         RasterPixelIsPoint       2
                                    #define   GTCitationGeoKey            1026 // ascii information
        
                                    #define GeographicTypeGeoKey			2048
                                    #define GeogGeodeticDatumGeoKey			2050
                                    #define GeogPrimeMeridianGeoKey			2051
                                    #define GeogLinearUnitsGeoKey			2052
                                    #define GeogLinearUnitSizeGeoKey		2053
                                    #define GeogAngularUnitsGeoKey			2054
                                    #define GeogAngularUnitSizeGeoKey		2055
                                    #define GeogEllipsoidGeoKey				2056
                                    #define GeogSemiMajorAxisGeoKey			2057
                                    #define GeogSemiMinorAxisGeoKey			2058
                                    #define GeogPrimeMeridianLongGeoKey		2059
        
                                    #define ProjectedCSTypeGeoKey			3072
                                    #define ProjectionGeoKey				3074
                                    #define ProjCoordTransGeoKey			3075
        
                                    #define ProjFalseEastingGeoKey			3082  
                                    #define ProjFalseNorthingGeoKey			3083  
                                    #define ProjFalseOriginLongGeoKey		3084  
                                    #define ProjFalseOriginLatGeoKey		3085  
                                    #define ProjFalseOriginEastingGeoKey	3086  
                                    #define ProjFalseOriginNorthingGeoKey	3087  
                                    #define ProjCenterLongGeoKey			3088  
                                    #define ProjCenterLatGeoKey 			3089  
                                    #define ProjCenterEastingGeoKey			3090  
                                    #define ProjCenterNorthingGeoKey		3091  
                                    #define ProjScaleAtNatOriginGeoKey		3092  
                                    #define ProjScaleAtCenterGeoKey			3093  
        
                                    #define TIFFTAG_GEODOUBLEPARAMS      34736
                                    #define TIFFTAG_GEOASCIIPARAMS       34737

									HANDLE hFile = CreateFileE( lpstrPathName,GENERIC_READ ); 
									if ( !hFile || hFile==INVALID_HANDLE_VALUE ) return false;
									
									SPDEMHDR demHdr; memset( &demHdr,0,sizeof(demHdr) ); 
									DWORD PxlSams=0,PxlBits=0,RowsPerStrip=1,PxlFmt=SAMPLEFORMAT_UINT,DatOff=0,*pOff=NULL; 

									DWORD rwByte; WORD wflag; DWORD lflag; LONGLONG offset; 
									wflag=0x4949; ReadFile( hFile,&wflag,2,&rwByte,NULL ); // 'II'
									if ( wflag != 0x4949 ){ CloseHandle( hFile ); return false; }
									wflag=0x002A; ReadFile( hFile,&wflag,2,&rwByte,NULL ); // 42
									if ( wflag != 0x002A ){ CloseHandle( hFile ); return false; }
									lflag=0x0008; ReadFile( hFile,&lflag,4,&rwByte,NULL ); // Offset
									offset = lflag; SeekTif( hFile,offset,FILE_BEGIN );
									WORD tagSum = 0x000D; ReadFile( hFile,&tagSum,2,&rwByte,NULL ); // TagSum
									
                                    // geo inf
                                    double Ex=0,Ny=0,Dx=1,Dy=1,Rx=0,Ry=0; bool bCorner=false,bTfw=true;

                                    int tileCol=0,tileRow=0,tileSz=0;
									WORD tagId; WORD tagType; DWORD tagSize,tagVal; 
									for( WORD i=0;i<tagSum;i++ ){
										SeekTif( hFile,offset+2+i*12,FILE_BEGIN );
                                        tagId=0; tagType=0; tagSize = 0; tagVal = 0;
										ReadTifTag( hFile,tagId,tagType,tagSize,tagVal );
										switch( tagId ){
											case _TIFFTAG_IMAGEWIDTH:
												demHdr.column = tagVal;
												break;
											case _TIFFTAG_IMAGELENGTH:
												demHdr.row   = tagVal;
												break;
											case _TIFFTAG_BITSPERSAMPLE:
												PxlBits = (char)tagVal;
												break;
											case _TIFFTAG_SAMPLESPERPIXEL:
												PxlSams = tagVal;
												break;
											case _TIFFTAG_ROWSPERSTRIP:
												RowsPerStrip = tagVal==0xffffffff?1:tagVal;
												break;
											case _TIFFTAG_STRIPOFFSETS:
												DatOff = tagVal;
												if ( tagSize>1 ){
													pOff = new DWORD[tagSize];
													SeekTif( hFile,DatOff,FILE_BEGIN );
													ReadFile( hFile,pOff,tagSize*sizeof(DWORD),&rwByte,NULL );
												}
												break;	
											case _TIFFTAG_SAMPLEFORMAT:
												PxlFmt = (char)tagVal;
												break;
                                            case TIFFTAG_TILEWIDTH:
                                                tileCol = tagVal;
                                                break;
                                            case TIFFTAG_TILELENGTH:
                                                tileRow = tagVal;
                                                break;
                                            case TIFFTAG_TILEBYTECOUNTS:
                                                tileSz = tagVal;
                                                break;
                                            case TIFFTAG_TILEOFFSETS:
                                                DatOff = tagVal;
                                                if ( tagSize>1 ){
													pOff = new DWORD[tagSize];
													SeekTif( hFile,DatOff,FILE_BEGIN );
													ReadFile( hFile,pOff,tagSize*sizeof(DWORD),&rwByte,NULL );
												}
                                                break;
                                                
                                            ///////////////////////////////////////////////////////////
                                            case TIFFTAG_GEOTRANSMATRIX:
                                                DatOff = tagVal;
                                                if ( tagSize>1 ){
                                                    double padfMatrix[16];
                                                    SeekTif( hFile,DatOff,FILE_BEGIN );
                                                    ReadFile( hFile,padfMatrix,sizeof(padfMatrix),&rwByte,NULL ); 
                                                    Dx = padfMatrix[0];
                                                    Dy = padfMatrix[5];
                                                    Ex = padfMatrix[3];
                                                    Ny = padfMatrix[7];
                                                    bTfw = false;
                                                }
                                                break;
                                            case TIFFTAG_GEOPIXELSCALE:
                                                DatOff = tagVal;
                                                if ( tagSize>1 ){
                                                    double pxlScale[3];
                                                    SeekTif( hFile,DatOff,FILE_BEGIN ); 
                                                    ReadFile( hFile,pxlScale,sizeof(pxlScale),&rwByte,NULL );
                                                    Dx =  pxlScale[0];
                                                    Dy = -pxlScale[1];
                                                }
                                                break;
                                            case TIFFTAG_GEOTIEPOINTS:
                                                DatOff = tagVal;
                                                if ( tagSize>1 ){
                                                    double tiePts[6];
                                                    SeekTif( hFile,DatOff,FILE_BEGIN ); 
                                                    ReadFile( hFile,tiePts,sizeof(tiePts),&rwByte,NULL );  
                                                    Ex = tiePts[3];
                                                    Ny = tiePts[4];
                                                    bTfw = false;
                                                }
                                                break;
                                                
                                            case TIFFTAG_GEOKEYDIRECTORY:
                                                DatOff = tagVal;
                                                if ( tagSize>1 ){
                                                    struct geoKeyDirs{
                                                        unsigned short ent_key;        // Key id           
                                                        unsigned short ent_location;   // key loca  , if ent_location==0 value is ent_val_offset , if ent_location==34736 vlue is TIFFTAG_GEODOUBLEPARAMS [ent_val_offset]
                                                        unsigned short ent_count;      // key count , the value's size .  
                                                        unsigned short ent_val_offset; // key value or tag offset  
                                                    }geoKey[32]; if (tagSize>sizeof(geoKey)) tagSize=sizeof(geoKey);
                                                    SeekTif( hFile,DatOff,FILE_BEGIN ); 
                                                    ReadFile( hFile,geoKey,tagSize,&rwByte,NULL );    
                                                    for( int k=1;k<geoKey[0].ent_val_offset;k++ ){
                                                        switch( geoKey[k].ent_key ){
                                                        case GTModelTypeGeoKey:
                                                            break;
                                                        case GTRasterTypeGeoKey:
                                                            if ( geoKey[k].ent_val_offset==1 ){
                                                                bCorner = false;
                                                            }else{
                                                                bCorner = true;
                                                            }
                                                            break;
                                                        }
                                                    }
                                                }
                                                break;
                                                
                                            case TIFFTAG_GEODOUBLEPARAMS:
                                                break;
                                            case TIFFTAG_GEOASCIIPARAMS:
                                                break;
                                        }
									}
									if ( demHdr.column*demHdr.row<=0 || PxlBits<16 || PxlBits>32 || PxlFmt>SAMPLEFORMAT_IEEEFP || RowsPerStrip<1 || RowsPerStrip>1000){ CloseHandle( hFile ); return false; }
									if ( PxlFmt==SAMPLEFORMAT_IEEEFP && PxlBits!=32 ) { CloseHandle( hFile ); return false; }
						
                                    if ( bTfw ){
                                        char strTfw[256]; strcpy( strTfw,lpstrPathName ); strcpy( strrchr(strTfw,'.'),".tfw" );
                                        FILE *ftfw = fopen( strTfw,"rt" ); 
                                        if ( ftfw ){
                                            fscanf( ftfw,"%lf",&Dx );
                                            fscanf( ftfw,"%lf",&Rx );
                                            fscanf( ftfw,"%lf",&Ry );
                                            fscanf( ftfw,"%lf",&Dy );
                                            fscanf( ftfw,"%lf",&Ex );
                                            fscanf( ftfw,"%lf",&Ny );
                                            fclose( ftfw );
                                            if (bCorner){ Ex += Dx/2; Ny += Dy/2; }// pixel corner to center. (the start pos in vz is center of pixel, so must be sub half of dx and dy) 
                                            demHdr.kapa  = atan2( Rx,Dx ); 
                                        }
                                    }else{
                                        demHdr.kapa  = 0;
                                    }

                                    double cosKap,sinKap; int orthRows = demHdr.row;
									cosKap=cos(demHdr.kapa ); sinKap=sin(demHdr.kapa ); 
                                    if ( cosKap==0 ){
                                        demHdr.intervalX = Ry/sinKap;
                                        demHdr.intervalY = Rx/sinKap;		
                                    }else{
                                        demHdr.intervalX = Dx/cosKap;
                                        demHdr.intervalY = -Dy/cosKap;
                                    }
									demHdr.startX = (Dy>=0)?(Ex):(Ex+sinKap* (demHdr.intervalY)*(orthRows-1));
									demHdr.startY = (Dy>=0)?(Ny):(Ny-cosKap* (demHdr.intervalY)*(orthRows-1));
                                    if ( bJustHdr ){ m_demHdr = demHdr; CloseHandle( hFile ); return true; }

									SetDemHeader( demHdr ); 
                                    if ( tileRow==0 || tileCol==0 ){
                                        BYTE *pGrid = (BYTE *)(m_pGrid+(m_demHdr.column*(m_demHdr.row-RowsPerStrip))); 
                                        UINT sz = m_demHdr.column*sizeof(float)*RowsPerStrip;
									    BYTE *pBuf = new BYTE[ demHdr.column*(PxlBits/8)*RowsPerStrip +8];
                                        for ( int r=0;r*(int)RowsPerStrip<m_demHdr.row;r++,pGrid-=sz ){
										    offset = pOff?pOff[r]:DatOff+(LONGLONG)r*(LONGLONG)demHdr.column*(PxlBits/8);
										    SeekTif( hFile,offset,FILE_BEGIN );
										    ReadFile( hFile,pBuf,demHdr.column*(PxlBits/8)*RowsPerStrip,&rwByte,NULL );

										    switch( PxlFmt ){
											    case SAMPLEFORMAT_IEEEFP:
												    memcpy( pGrid,pBuf,sizeof(float)*demHdr.column*RowsPerStrip );
												    break;
											    case SAMPLEFORMAT_INT   :
												    if ( PxlBits==16 ){
													    short *pS = (short*)pBuf; float *pD = (float *)pGrid;
                                                        for( int c=0;c<demHdr.column*(int)RowsPerStrip;c++,pS++,pD++ ){ 
                                                            *pD = *pS>-1000?(float)*pS:0; // Just For DEM in Earth . eg. SRTM4.1.
                                                        }
												    }else{
													    int *pS = (int*)pBuf; float *pD = (float *)pGrid;
													    for( int c=0;c<demHdr.column*(int)RowsPerStrip;c++,pS++,pD++ ) *pD = (float)*pS;
												    }
												    break;
											    case SAMPLEFORMAT_UINT	:
												    if ( PxlBits==16 ){
													    WORD *pS = (WORD*)pBuf; float *pD = (float *)pGrid;
													    for( int c=0;c<demHdr.column*(int)RowsPerStrip;c++,pS++,pD++ ) *pD = (float)*pS;
												    }else{
													    DWORD *pS = (DWORD*)pBuf; float *pD = (float *)pGrid;
													    for( int c=0;c<demHdr.column*(int)RowsPerStrip;c++,pS++,pD++ ) *pD = (float)*pS;
												    }
												    break;
										    }
									    }
									    delete pBuf;          

                                    }else{
                                        
                                        BYTE *pTile = new BYTE[tileCol*tileRow*(PxlBits/8)*PxlSams +8];
                                        float *pTF = new float[ tileCol*tileRow ];
                                        int c,tileCs = ((demHdr.column+(tileCol-1))/tileCol);
                                        int i,r,tileRs = ((demHdr.row+(tileRow-1))/tileRow);
                                        for ( r=0;r<tileRs;r++ ){
                                            for ( c=0;c<tileCs;c++ ){
                                                offset = pOff?pOff[r*tileCs+c]:DatOff + (r*tileCs+c)*(tileCol*tileRow*(PxlBits/8)*PxlSams);
                                                SeekTif( hFile,offset,FILE_BEGIN );
                                                ReadFile( hFile,pTile,tileCol*tileRow*(PxlBits/8)*PxlSams,&rwByte,NULL );
                                                switch( PxlFmt ){
                                                    case SAMPLEFORMAT_IEEEFP:
                                                        memcpy( pTF,pTile,tileCol*tileRow*sizeof(float)  );
                                                        break;
                                                    case SAMPLEFORMAT_INT   :
                                                        if ( PxlBits==16 ){
                                                            short *pS = (short*)pTile; float *pD = (float *)pTF;
                                                            for( i=0;i<tileCol*tileRow;i++,pS++,pD++ ) *pD = *pS;
                                                        }else{
                                                            int *pS = (int*)pTile; float *pD = (float *)pTF;
                                                            for( i=0;i<tileCol*tileRow;i++,pS++,pD++ ) *pD = (float)(*pS);
                                                        }
                                                        break;
                                                    case SAMPLEFORMAT_UINT	:
                                                        if ( PxlBits==16 ){
                                                            WORD *pS = (WORD*)pTile; float *pD = (float *)pTF;
                                                            for( i=0;i<tileCol*tileRow;i++,pS++,pD++ ) *pD = *pS;
                                                        }else{
                                                            DWORD *pS = (DWORD*)pTile; float *pD = (float *)pTF;
                                                            for( i=0;i<tileCol*tileRow;i++,pS++,pD++ ) *pD = (float)(*pS);
                                                        }
                                                        break;
                                                }
                                                float *pD = m_pGrid + (r*tileRow)*demHdr.column+c*tileCol;
                                                float *pS = pTF; int cpyCols=tileCol,cpyRows=tileRow;
                                                if ( (c+1)*tileCol>demHdr.column ) cpyCols = (demHdr.column%tileCol);
                                                if ( (r+1)*tileRow>demHdr.row    ) cpyRows = (demHdr.row%tileRow   );
                                                for( i=0;i<cpyRows;i++,pD+=demHdr.column,pS+=tileCol )
                                                    memcpy( pD,pS,cpyCols*sizeof(float) );                                                
                                            }
                                        }
                                        delete pTF;
                                        delete pTile; 
                                        
                                        // Flip Up -> Down
                                        float *pRow = new float[demHdr.column +8];
                                        float *pH=m_pGrid,*pR=m_pGrid+(m_demHdr.column*(m_demHdr.row-1));
                                        for ( r=0;r<demHdr.row/2;r++,pH+=demHdr.column,pR-=demHdr.column ){
                                            memcpy( pRow,pH,m_demHdr.column*sizeof(float) );
                                            memcpy( pH,pR,m_demHdr.column*sizeof(float) );
                                            memcpy( pR,pRow,m_demHdr.column*sizeof(float) );
                                        }
                                    }
									if ( pOff ) delete pOff; pOff = NULL;
									CloseHandle( hFile );

									// Set DEM Range
									float minZ=99999999.f,maxZ=-99999999.f,*pG=m_pGrid;
									for (int r=0;r<m_demHdr.column*m_demHdr.row;r++,pG++ ){
										if ( *pG<-9999.8 ) *pG =  NOVALUE;
										else{ if ( minZ>*pG ) minZ=*pG; if ( maxZ<*pG ) maxZ=*pG; }
									}
									m_midZ = (minZ+maxZ)/2;
									m_minZ = minZ; m_maxZ = maxZ;

									return true;
								};
	inline  bool				SaveAsGTIF(LPCSTR lpstrPathName,BOOL bMsgBox=TRUE){
									#define	_TIFFTAG_SUBFILETYPE		254	// subfile data descriptor 
									#define	_TIFFTAG_IMAGEWIDTH			256	// image width in pixels 
									#define	_TIFFTAG_IMAGELENGTH		257	// image height in pixels 
									#define	_TIFFTAG_BITSPERSAMPLE		258	// bits per channel (sample) 
									#define	_TIFFTAG_COMPRESSION		259	// data compression technique 
									#define	    COMPRESSION_NONE		1	// dump mode 
									#define	_TIFFTAG_PHOTOMETRIC		262	// photometric interpretation 
									#define	    PHOTOMETRIC_MINISBLACK	1	// min value is black 
									#define	_TIFFTAG_STRIPOFFSETS		273	// offsets to data strips 
									#define	_TIFFTAG_ORIENTATION		274	// +image orientation 
									#define	    ORIENTATION_TOPLEFT		1	// row 0 top, col 0 lhs 
									#define	_TIFFTAG_SAMPLESPERPIXEL	277	// samples per pixel 
									#define	_TIFFTAG_ROWSPERSTRIP		278	// rows per strip of data 
									#define	_TIFFTAG_STRIPBYTECOUNTS	279	// bytes counts for strips 
									#define	_TIFFTAG_SAMPLEFORMAT		339	// !data sample format 
									#define	    SAMPLEFORMAT_UINT		1	// !unsigned integer data ()
									#define	    SAMPLEFORMAT_INT		2	// !signed integer data ()
									#define		SAMPLEFORMAT_IEEEFP		3	// !IEEE floating point data (float)
									#define	_TIFF_SHORT					3	// 16-bit unsigned integer (unsigned short)
									#define _TIFF_LONG					4	// 32-bit unsigned integer (unsigned int )
									
									struct TIFDEMHDR{
										WORD	strFlg;
										WORD	strVer;
										DWORD	tagOff;	
										WORD	tagVal[79];										
										char    strTag[16];		// "TIFDEM_V1.0"
										char    strInf[180];	// "C:%d R:%d Geo:%.9lf %.9lf %.6lf %.9lf"
										char	strRev[648];	// Just For 1024 Byte;
										BYTE    strFlag[12];	// "###ID_DEM@@@"
									}tfdemHdr; memset( &tfdemHdr,0,sizeof(tfdemHdr) );
									DWORD datOffSet     = sizeof(tfdemHdr),datSz = m_demHdr.column*m_demHdr.row*sizeof(float),rw=0;
									tfdemHdr.strFlg		= 0x4949; 
									tfdemHdr.strVer		= 0x002A; 
									tfdemHdr.tagOff		= 0x0008; 
									tfdemHdr.tagVal[0]	= 0x000D;
									SetTagVal( tfdemHdr.tagVal+1+6* 0,_TIFFTAG_SUBFILETYPE    ,_TIFF_SHORT,1,			             1 );
									SetTagVal( tfdemHdr.tagVal+1+6* 1,_TIFFTAG_COMPRESSION    ,_TIFF_SHORT,1,		  COMPRESSION_NONE );
									SetTagVal( tfdemHdr.tagVal+1+6* 2,_TIFFTAG_IMAGEWIDTH     ,_TIFF_LONG ,1,		   m_demHdr.column );
									SetTagVal( tfdemHdr.tagVal+1+6* 3,_TIFFTAG_IMAGELENGTH    ,_TIFF_LONG ,1,			  m_demHdr.row );
									SetTagVal( tfdemHdr.tagVal+1+6* 4,_TIFFTAG_BITSPERSAMPLE  ,_TIFF_SHORT,1,		   sizeof(float)*8 );
									SetTagVal( tfdemHdr.tagVal+1+6* 5,_TIFFTAG_SAMPLESPERPIXEL,_TIFF_SHORT,1,						 1 );
									SetTagVal( tfdemHdr.tagVal+1+6* 6,_TIFFTAG_SAMPLEFORMAT   ,_TIFF_SHORT,1,	   SAMPLEFORMAT_IEEEFP );
									SetTagVal( tfdemHdr.tagVal+1+6* 7,_TIFFTAG_PHOTOMETRIC    ,_TIFF_SHORT,1, PHOTOMETRIC_MINISBLACK );
									SetTagVal( tfdemHdr.tagVal+1+6* 8,_TIFFTAG_ORIENTATION    ,_TIFF_SHORT,1, ORIENTATION_TOPLEFT    );
									SetTagVal( tfdemHdr.tagVal+1+6* 9,_TIFFTAG_ROWSPERSTRIP   ,_TIFF_LONG ,1,			     0xffffffff);
									SetTagVal( tfdemHdr.tagVal+1+6*10,_TIFFTAG_STRIPOFFSETS   ,_TIFF_LONG ,1,				 datOffSet );
									SetTagVal( tfdemHdr.tagVal+1+6*11,_TIFFTAG_STRIPBYTECOUNTS,_TIFF_LONG ,1,				LONG(datSz));
									SetTagVal( tfdemHdr.tagVal+1+6*12,0,0,0,0 );
									sprintf( tfdemHdr.strInf,"C:%d R:%d X0:%.9lf Y0:%.9lf Gsd:%.6lf Kap:%.9lf",m_demHdr.column,m_demHdr.row,m_demHdr.startX,m_demHdr.startY,m_demHdr.intervalX,m_demHdr.kapa );
									
									HANDLE hFile = CreateFileE( lpstrPathName,GENERIC_READ|GENERIC_WRITE );
									if ( !hFile || hFile==INVALID_HANDLE_VALUE ) return FALSE;   
									WriteFile( hFile,&tfdemHdr,sizeof(tfdemHdr),&rw,NULL );   
									BYTE *pGrid = (BYTE*)(m_pGrid+(m_demHdr.column*(m_demHdr.row-1))); 
									UINT sz = m_demHdr.column*sizeof(float);
                                    for ( int r=0;r<m_demHdr.row;r++,pGrid-=sz )
										WriteFile( hFile,pGrid,sz,&rw,NULL );
                                    CloseHandle( hFile );  _sleep100;
									
									char strTfw[256]; strcpy( strTfw,lpstrPathName ); strcpy( strrchr(strTfw,'.'),".tfw" );
									FILE *ftfw = fopen( strTfw,"wt" ); if (!ftfw) return false;
									double Dx,Dy,Rx,Ry,Ex,Ny,cosKap,sinKap; int orthRows = m_demHdr.row; bool bCorner=false;
									cosKap=cos(m_demHdr.kapa); sinKap=sin(m_demHdr.kapa);
									Dx =  cosKap*m_demHdr.intervalX; //  cosA*dx
									Rx =  sinKap*m_demHdr.intervalX; //  sinA*dx
									Ry =  sinKap*m_demHdr.intervalY; //  sinA*dy
									Dy = -cosKap*m_demHdr.intervalY; // -cosA*dy
									Ex = (Dy>=0)?( m_demHdr.startX ):( m_demHdr.startX-sinKap*m_demHdr.intervalY*(orthRows-1) );
									Ny = (Dy>=0)?( m_demHdr.startY ):( m_demHdr.startY+cosKap*m_demHdr.intervalY*(orthRows-1) );
									if (bCorner){ Ex -= Dx/2; Ny -= Dy/2; }// pixel center to corner.
									fprintf( ftfw,"\t%.12lf\n",Dx );
									fprintf( ftfw,"\t%.12lf\n",Rx );
									fprintf( ftfw,"\t%.12lf\n",Ry );
									fprintf( ftfw,"\t%.12lf\n",Dy );
									fprintf( ftfw,"\t%.12lf\n",Ex );
									fprintf( ftfw,"\t%.12lf\n",Ny );
									fclose(ftfw); 
									
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
									static double lastKap = 0,cosKap = cos(0.0f),sinKap=sin(0.0f);
									if ( (fabs(kapa-lastKap))>0.00000001 ){ lastKap = kapa; cosKap = cos(lastKap); sinKap = sin(lastKap); }

									double tempx=*x;	double tempy=*y;
									*x= tempx* cosKap - tempy * sinKap;
									*y= tempy* cosKap + tempx * sinKap;
								};

	inline static void			GlobalToLocal( double * x, double * y,double kapa ){
									static double lastKap = 0,cosKap = cos(0.0f),sinKap=sin(0.0f);
									if ( (fabs(kapa-lastKap))>0.00000001 ){ lastKap = kapa; cosKap = cos(lastKap); sinKap = sin(lastKap); }

									double tempx=*x; double tempy=*y;
									*x= tempx* cosKap + tempy * sinKap;
									*y= tempy* cosKap - tempx * sinKap;
								};
    inline static BOOL          IsExist(LPCSTR lpstrPathName){
                                    WIN32_FIND_DATA fd; HANDLE hFind=INVALID_HANDLE_VALUE;
                                    hFind = ::FindFirstFile(lpstrPathName,&fd);
                                    if ( hFind==INVALID_HANDLE_VALUE ) return FALSE;
                                    ::FindClose(hFind); return TRUE;
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
	inline static void	 SetTagVal( WORD *pTag,WORD tagId,WORD tagType,DWORD tagSize,DWORD tagVal ){ 
		pTag[0] = tagId  ; pTag[1] = tagType; *((DWORD*)(pTag+2)) = tagSize; *((DWORD*)(pTag+4)) = tagVal ; 
	};
	inline static DWORD  SeekTif( HANDLE hFile, LONGLONG distance, DWORD MoveMethod ){
		LARGE_INTEGER li; li.QuadPart = distance;
		return SetFilePointer( hFile,li.LowPart,&li.HighPart,MoveMethod );    
	};
	inline static void  ReadTifTag( HANDLE hFile,WORD &tagId,WORD &tagType,DWORD &tagSize,DWORD &tagVal ){
		DWORD rwByte=0;
		ReadFile( hFile,&tagId     ,2,&rwByte,NULL );
		ReadFile( hFile,&tagType   ,2,&rwByte,NULL );
		ReadFile( hFile,&tagSize   ,4,&rwByte,NULL );
		ReadFile( hFile,&tagVal    ,4,&rwByte,NULL );	
	};

private:
	double			m_cosKap,m_sinKap;

public:
	inline  void SetRevMsgWnd( HWND hWnd,UINT msgID ){   m_hWndRec=hWnd; m_msgID=msgID; };
protected:	
    virtual void ProgBegin(int range)       {if ( IsWindowE(m_hWndRec) ) SendMessageE( m_hWndRec,m_msgID,PROG_START,range );          };
    virtual void ProgStep(int step)         {if ( IsWindowE(m_hWndRec) ) SendMessageE( m_hWndRec,m_msgID,PROG_STEP ,step );           };
    virtual void ProgEnd()                  {if ( IsWindowE(m_hWndRec) ) SendMessageE( m_hWndRec,m_msgID,PROG_OVER ,0 );              };
    virtual void PrintMsg(LPCSTR lpstrMsg ) {if ( IsWindowE(m_hWndRec) ) SendMessageE( m_hWndRec,m_msgID,PROG_MSG  ,UINT(lpstrMsg) ); };
private:
    HWND            m_hWndRec;
    UINT            m_msgID;
};


#endif // !defined(SPDEM_H_SUPRESOFT_INC_DUANYANSONG_2002_10_17_873495)
