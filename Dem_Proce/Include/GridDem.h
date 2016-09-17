//////////////////////////////////////////////////////////////////////////
// GridDem.h

#ifndef _GRIDDEM_H_2011_07_11_1505
#define _GRIDDEM_H_2011_07_11_1505

#include "windows.h"

#ifndef GRIDDEM_LIB
	#define GRIDDEM_LIB  __declspec(dllimport)
	#ifdef _DEBUG_GRIDDEM
	#pragma comment(lib,"GridDemD.lib") 
	#pragma message("Automatically linking with GridDemD.lib") 
	#else
	#pragma comment(lib,"GridDem.lib") 
	#pragma message("Automatically linking with GridDem.lib") 
	#endif
#endif

const float INVALID_HEIGHT = -9999.90f;

// DEM格式类型枚举
#ifndef _DEM_FORMAT
#define _DEM_FORMAT
enum DPGDemFormat{
	NSDTF_DEM = 0,
	RASTERT_DEM,
	ASCII_DEM,
	VIRTUOZO_DEM,
	SRTM,
	ASTGTM,
	UNKNOWN_DEM
};
#endif

#ifndef _DATA_FRAME
#define _DATA_FRAME
typedef struct tagDATAFRAME{
	double Lon[4], Lat[4];       // 影像四角的概略经纬度，依次为：左上、右上、右下和左下
	double CenterLon, CenterLat; // 影像中心的概略经纬度
	double AverageAlt;           // 影像覆盖范围的平均高程（近似值）
}DataFrame;
#endif

typedef struct tagDEMINFO{
	double x0, y0;
	double x1, y1;
	double dx, dy;
	int cols, rows;
	float denum;
}DemInfo;

class GRIDDEM_LIB CGridDem
{
public:
	CGridDem(void);
	virtual ~CGridDem(void);

	typedef struct tagBinDemInfo
	{
		double	startX;
		double	startY;
		double	kapa;
		double	intervalX;
		double	intervalY;
		int		column;
		int		row;
	}BinDemInfo;

//Operations
public:
	virtual BOOL	Open(const char* lpszPathName);
	virtual BOOL	Save();
	virtual BOOL	ReadDemInfo(const char* lpszPathName);
	virtual BOOL	LoadData() = 0;
	virtual BOOL	Allocate();
	virtual void	Release();
	virtual BOOL	UpdateDemInfo();
	virtual float	Interpolation(double x, double y);
	virtual float	InterpolationForWGS84(double Lon84, double Lat84);

// Attributes
public:
	virtual LPCTSTR GetPathName(){return m_strPathName;};
	virtual LPCTSTR GetDemTitle(){return m_strDemTitle;};
	virtual void	SetGeoPathName(const char* lpszPathName);
	virtual void	GetGeoRefTitle(char* strTitle);
	virtual DemInfo* GetDemInfo(){return &m_demInfo;};
	virtual float*	GetHeightBuffer(){return m_pHeight;};
	virtual int		GetCols() {return m_demInfo.cols;};
	virtual int		GetRows() {return m_demInfo.rows;};
	virtual double	GetLeftBottomX() {return m_demInfo.x0;};
	virtual double	GetLeftBottomY() {return m_demInfo.y0;};
	virtual double	GetGrdResolutionX() {return m_demInfo.dx;};
	virtual double	GetGrdResolutionY() {return m_demInfo.dy;};
	virtual double* GetDemFrmLongitude() {return m_DemFrm.Lon;};
	virtual double* GetDemFrmLatitude()  {return m_DemFrm.Lat;};
	virtual double  GetDemCenterLongitude() {return m_DemFrm.CenterLon;};
	virtual double  GetDemCenterLatitude()  {return m_DemFrm.CenterLat;};
	virtual BOOL	IsOverlapWGS84(double minLon, double maxLon, double minLat, double maxLat);

protected:
	virtual BOOL	ReadBinaryData();
	virtual BOOL	ReadBinaryHead();
	virtual BOOL	SaveBinaryData();

protected:
	char m_strPathName[512];
	char m_strDemTitle[256];

	int m_nDemType;
	DataFrame m_DemFrm;
	DemInfo   m_demInfo;
	float*  m_pHeight;
	void* m_pGeoRef;
};

class GRIDDEM_LIB CHugeDem : public CGridDem{
public:
	CHugeDem(void);
	virtual ~CHugeDem(void);
	
	virtual BOOL Open(const char* lpszPathName);
	virtual BOOL Save();
	virtual BOOL ReadDemInfo(const char* lpszPathName);
	virtual BOOL LoadData();
	
	virtual float Interpolation(double x, double y);
	
	BOOL ReadData(double minLon, double minLat, double maxLon, double maxLat);
	BOOL Create(const char* lpszPathName, DemInfo& demInfo);
	BOOL ReadData(float* data, int beginCol, int beginRow, int cols, int rows);
	BOOL WriteData(float* data, int beginCol, int beginRow, int cols, int rows);
	
	BOOL MergeDemInWGS84(const char* lpszPathName, CGridDem** pDem, int nDem);
	BOOL LocalInterpolation(void* pGeoRefObject, double gx, double gy, double* Lon84, double* Lat84, double* Alt84);
	
protected:
	void SubCalculateMergeRange(CGridDem** pDem, int nDem, double& minLon, double& minLat, double& maxLon, double& maxLat);
	void SubCalculateIntervalDEM(CGridDem** pDem, int nDem, double minLat, double& dLon, double& dLat);
	void SubPredictDemLoadFlag(CGridDem** pDem, int nDem, double minLon, double maxLon, double minLat, double maxLat, BYTE* pHit);
	BOOL SubLoadDemData(CGridDem** pDem, int nDem, BYTE* pHit);
	void SubMergeDem(CGridDem** pDem, int nDem, int bc, int br, int cols, int rows);
	
	DemInfo m_tmpInfo;
	float   m_fAvgHeight;
};

// 创建DEM对象
GRIDDEM_LIB CGridDem* CreateDemObject(const char* lpszPathName, DPGDemFormat nType = UNKNOWN_DEM);

// 释放DEM对象
GRIDDEM_LIB void ReleaseDemObject(CGridDem* pDem);

inline GRIDDEM_LIB DPGDemFormat INT_2_DPGDemFormat(int nType){
	switch (nType){
		case 0: return NSDTF_DEM;
		case 1: return RASTERT_DEM; 
		case 2: return ASCII_DEM;
		case 3: return VIRTUOZO_DEM;
		case 4: return SRTM;
		case 5: return ASTGTM;
	}
	return UNKNOWN_DEM;
};

inline GRIDDEM_LIB int DPGDemFormat_2_INT(DPGDemFormat nType){
	switch (nType){
		case NSDTF_DEM:    return 0;
		case RASTERT_DEM:  return 1;
		case ASCII_DEM:    return 2;
		case VIRTUOZO_DEM: return 3;
		case SRTM:         return 4;
		case ASTGTM:	   return 5;
	}
	return 3;
};

#endif