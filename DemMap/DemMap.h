//DemMap.h
/********************************************************************
	DemMap
	created:	2014/06/28
	author:		LX_whu 
	purpose:	This file is for DemMap function
*********************************************************************/
#if !defined DemMap_h__LX_whu_2014_6_28
#define DemMap_h__LX_whu_2014_6_28

#include "PrjLog.hpp"
#include "SpDem.hpp"
#include "SpGeoCvt.h"
#include "WuGlbHei.h"
#include "SpMapNumber.h"


#ifndef	_CLASS_ARRAY_PATH
#define _CLASS_ARRAY_PATH 

#define MAX_PATHNAME	512
	class  CArray_Path
	{
	public:
		CArray_Path( char* pBuf=NULL,int size=0 ){	m_maxSize = m_size = size; m_pBuf = NULL;if ( m_size ) m_pBuf = new char[m_size*MAX_PATHNAME]; if ( pBuf	) memcpy( m_pBuf,pBuf,sizeof(char)*m_size*MAX_PATHNAME ); };
		virtual ~CArray_Path(){ if (m_pBuf) delete []m_pBuf; };
		
		inline	char*	GetData(int &size){ size=m_size; return m_pBuf; };
		inline	int			Append(const char* uint ){ if ( m_size >= m_maxSize ){  m_maxSize += 30; char* pOld = m_pBuf; m_pBuf	 = new char[m_maxSize*MAX_PATHNAME]; memset( m_pBuf,0,sizeof(char)*m_maxSize*MAX_PATHNAME ); memcpy( m_pBuf,pOld,sizeof(char)*m_size*MAX_PATHNAME );	delete []pOld; } strcpy(m_pBuf+m_size*MAX_PATHNAME,uint);m_size++;return (m_size-1); };
		inline	void		SetSize(int size ){ if (size<m_maxSize) m_size=size; else Reset(NULL,size);  };
		inline	void		Reset( char* pBuf=NULL,int size=0 ){ if (m_pBuf) delete []m_pBuf; m_pBuf = NULL; m_maxSize	= size; m_size = 0; if ( m_maxSize ){ m_pBuf = new char[m_maxSize*MAX_PATHNAME]; memset( m_pBuf,0,sizeof(char)*m_maxSize*MAX_PATHNAME ); } if ( pBuf ) { m_size = size;	memcpy( m_pBuf,pBuf,sizeof(char)*m_size*MAX_PATHNAME ); } }
		inline  char*   operator[](int idx){ return m_pBuf+idx*MAX_PATHNAME;   };
		inline	char*	GetAt(int idx){ if(idx>=m_size) { printf("Warning:Ask for too much image!\n"); return m_pBuf+(m_size-1)*MAX_PATHNAME; } return m_pBuf+idx*MAX_PATHNAME;		   };
		inline	int			GetSize(){ return m_size;				   };
		inline	void		RemoveAt(int idx){ if(idx==m_size-1) {m_size--;return;} memcpy(m_pBuf+idx*MAX_PATHNAME,m_pBuf+(idx+1)*MAX_PATHNAME,sizeof(char)*(m_size-idx-1)*MAX_PATHNAME); m_size--;}
		// 	inline	int			InsertAt(int idx,char* uint){ if(idx==m_size-1) {return Append(uint); }if ( m_size >= m_maxSize ){  m_maxSize += 256; ATTRILINE* pOld = m_pBuf; m_pBuf	 = new ATTRILINE[m_maxSize]; memset( m_pBuf,0,sizeof(ATTRILINE)*m_maxSize ); memcpy( m_pBuf,pOld,sizeof(ATTRILINE)*m_size );	delete []pOld; } memcpy(m_pBuf+idx+1,m_pBuf+idx,(m_size-idx)*sizeof(ATTRILINE));memcpy(m_pBuf+idx,&uint,sizeof(ATTRILINE));m_size++;return (m_size-1);}
		
		
	protected:
		char*		m_pBuf;
		int				m_size;
		int				m_maxSize;
	};
#endif

#define META_UNDO	0
#define META_DSM	1
#define META_DEM	2
#define META_END	3

class CDemMap{
public:
	CDemMap() { 
		m_dem = NULL; m_dem_prj = NULL;	m_pMapInfo = NULL;
		Reset();
	}
	~CDemMap();
	void Reset() ;
	int Load4DemListFile(const char* lpstrListFile);
	int LoadGcdFile(const char* lpstrPath){
		if ( !m_geoCvt.Load4File(lpstrPath) ){
			LogPrint(ERR_FILE_READ,"Error:Can't Load Gcd File:%s!",lpstrPath);
			return ERR_FILE_READ;
		}
		strcpy(m_strGcdPath,lpstrPath);
		return ERR_NONE;
	}
	int	MergeDem(const char* lpstrMergePath,float xgsd,float ygsd);
	int	MergeDem(const char* lpstrMergePath,float xgsd,float ygsd,CWuSRTM* pLocSrtm);
	int Map(const char* lpstrDir,	MAPFORMAT mapformat,MAPSCALE mapscale,float fExpWidth,float fExpHeight);
	int Map(const char* lpstrDir,	MAPFORMAT mapformat,MAPSCALE mapscale,float fExpWidth,float fExpHeight,CWuSRTM* pLocSrtm);

	const char* GetDemPath(int idx){ return m_arDemPath.GetAt(idx);	}
	void GetDemPath(int idx,char* path) { strcpy(path,m_arDemPath.GetAt(idx));	}
	bool IsDemBLH(int idx) { if(!m_dem_prj || idx>=m_nDemNum || m_dem_prj[idx]!=GP_LONLAT)	return false; return true; }

	void SetMetaMode(int mode) {  m_mdMeta = mode;	};
	void SetGSD(float gsd,bool bGsdIsMeter = true) { SetGSD(gsd,gsd,bGsdIsMeter);	};
	void SetGSD(float xgsd,float ygsd,bool bGsdIsMeter = true) { m_xgsd_map = xgsd;	m_ygsd_map = ygsd; m_bGsdIsMeter = bGsdIsMeter;	} ;

	void GetGSD(float* xgsd,float* ygsd){ *xgsd = m_xgsd_map;	*ygsd = m_ygsd_map;	}
	void GetAreaGeoRange_degree(double* xmin,double* ymin,double* xmax,double* ymax){
		*xmin = m_xmin_degree;	*ymin = m_ymin_degree;
		*xmax = m_xmax_degree;	*ymax = m_ymax_degree;
	}

	void SetOutputPath(const char* lpstrDir) { strcpy(m_strOutPath,lpstrDir);	}
	const char* GetOutputPath() { return m_strOutPath;	}

	void SetSRTMMode(bool use) { m_bSRTM = use;	}

	bool SaveTaskFile(const char* lpstrTskFile,MapInfo* pm,float fExpWidth,float fExpHeight){
		return SaveTaskFile(lpstrTskFile,pm,fExpWidth,fExpHeight,m_xgsd_map,m_ygsd_map,m_bGsdIsMeter,m_bSRTM,m_mdMeta,m_strOutPath);
	}
	bool SaveTaskFile(const char* lpstrTskFile,MapInfo* pm,float fExpWidth,float fExpHeight,float xgsd,float ygsd,bool bMeter,bool bSRTM,int meta,const char* lpstrOutput);
	bool LoadTaskFile(const char* lpstrTskFile,MapInfo* pm,float* fExpWidth,float* fExpHeight){
		return LoadTaskFile(lpstrTskFile, pm, fExpWidth, fExpHeight,&m_xgsd_map,&m_ygsd_map,&m_bGsdIsMeter,&m_bSRTM,&m_mdMeta,m_strOutPath);
	}
	bool LoadTaskFile(const char* lpstrTskFile,MapInfo* pm,float* fExpWidth,float* fExpHeight,float* xgsd,float* ygsd,bool* bMeter,bool* bSRTM,int* meta ,char* strOutput );

	int CreateTask(const char* lpstrTskDir, MAPFORMAT mapformat,MAPSCALE mapscale,float fExpWidth,float fExpHeight)
	{
		return CreateTask( lpstrTskDir,  mapformat, mapscale, fExpWidth, fExpHeight,m_xgsd_map,m_ygsd_map,m_bGsdIsMeter,m_bSRTM,m_mdMeta,m_strOutPath);
	};
	int CreateTask(const char* lpstrTskDir, MAPFORMAT mapformat,MAPSCALE mapscale,float fExpWidth,float fExpHeight,float xgsd,float ygsd,bool bMeter,bool bSRTM,int meta,const char* lpstrOutput);
	
	int RunTask(const char* lpstrTskFile);
	int RunTask(MapInfo mapInfo,float fExpWidth,float fExpHeight,CWuSRTM* pLocSrtm,const char* lpstrDir,float xgsd,float ygsd,bool bMeter,int meta);
protected:
	int AppendDem(const char* lpstrDemPath);
	void AppendDemPath(const char* lpstrDemPath){ m_arDemPath.Append(lpstrDemPath);	}

	MapInfo* GetMapsByRange(MAPFORMAT format,MAPSCALE scale,int* n);


	bool GetDemZValue(double x,double y,float* z=NULL,int* numZ=NULL,bool bPrjCoor = true);
	bool GetDemFileIdx(double x,double y,int* idx,int* num,bool bPrjCoor = true);

	bool MallocDemBuf(int sz){ 
		ReleaseDemBuf(); 
		m_dem = new CSpDem[sz];	if(!m_dem) return false; 
		m_dem_prj = new GEOPROJECTION[sz];	if(!m_dem_prj) return false; 
		return true;	
	}
	void ReleaseDemBuf();

	void ReleaseMapBuf()
	{
		if(m_pMapInfo) delete[] m_pMapInfo; m_pMapInfo = NULL;
		m_nMapInfo = 0;
	}
	bool MallocMapBuf(int sz)
	{
		ReleaseMapBuf();
		m_pMapInfo = new MapInfo[sz];	if(!m_pMapInfo) return false;
		m_nMapInfo = sz;
		return true;
	}

	void GetSRTMBuf(CWuSRTM& locSrtm){
		CWuGlbHei srtmHei;	
		{
			LogPrint(ERR_NONE,"Start Getting SRTM for The Area...");
			int nRows,nCols;
			nRows = int( (m_ymax_degree-m_ymin_degree)/SRTM_GSD )+1;	nCols = int( (m_xmax_degree-m_xmin_degree)/SRTM_GSD )+1;
			float *pLocZ = new float[ nRows*nCols ];
			srtmHei.GetSRTM( m_xmin_degree,m_ymin_degree,nCols,nRows,pLocZ );
			locSrtm.Init( m_xmin_degree,m_ymin_degree,nCols,nRows,pLocZ );
			delete[] pLocZ;
			LogPrint(ERR_NONE,"End Getting SRTM");
		}
	}
protected:

	CArray_Path m_arDemPath;
	CSpDem*		m_dem;
	enum GEOPROJECTION{ GP_UNKNOWN,GP_LONLAT,GP_PRJ} *m_dem_prj;
	int			m_nDemNum;
	char		m_strGcdPath[512];
	char		m_strOutPath[512];
	CSpGeoCvt	m_geoCvt;

	double		m_xmin_degree;
	double		m_ymin_degree;
	double		m_xmax_degree;
	double		m_ymax_degree;
	
	double		m_xmin_meter;
	double		m_ymin_meter;
	double		m_xmax_meter;
	double		m_ymax_meter;

	float		m_xgsd_map;
	float		m_ygsd_map;
	bool		m_bGsdIsMeter;

	int			m_mdMeta;	//undo dsm dem
	bool		m_bSRTM;

	MapInfo* m_pMapInfo;
	int m_nMapInfo;
};

#endif // DemMap_h__LX_whu_2014_6_28