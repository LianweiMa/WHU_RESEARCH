// GISManager.h: interface for the CGISManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GISMANAGERS_H__D13A3F38_0215_40A5_8119_B283E3AED206__INCLUDED_)
#define AFX_GISMANAGERS_H__D13A3F38_0215_40A5_8119_B283E3AED206__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef GISMANGER_LIB
#define GISMANGER_LIB _declspec(dllimport)
#ifdef _DEBUG_GISMANAGER
#pragma comment(lib,"GISManagerD.lib")
#pragma message("Automatically linking with GISManagerD.lib")
#else
#pragma comment(lib,"GISManager.lib")
#pragma message("Automatically linking with GISManager.lib")
#endif
#endif

enum LinkPattern
{
	LINKORACLE = 0,
	LINKFILE
};

//表示点的结构体：
typedef struct tagVertex2D			//点的坐标
{
	double x;						//经度
	double y;						//纬度
}Vertex2D;

//表示GIS数据属性的结构体：
typedef struct  tagGISINFO				//GIS数据的属性
{
	char	 filepath[512];				//文件地址
	short	 filetype;					//数据类型，DEM或者DOM
	short	 subsourcetype;				//数据来源，如SRTM、ASTGTM
	int		 gsdunit;					//地面分辨率的单位
	double	 gsd;						//地面分辨率
	double	 precison;					//几何精度
	Vertex2D edgepoint[4];				//4个角点的经纬度
	Vertex2D centerpoint;				//中心点的经纬度(重心坐标)
	char	 shoottime[16];				//拍摄时间
	char	 updatetime[16];			//更新时间
	char	 sensor[16];				//生成数据的传感器类型(可以改成int)
	char	 coord[256];				//坐标系参数文件
	char	 productor[256];			//生产单位和地区
	char	 propertyone[256];			//预留字段1
	char	 propertytwo[256];			//预留字段2
}GisInfo;

//GIS数据类型
enum GIS_DATA_TYPE
{
	GDT_DOM = 0,
	GDT_DEM
};

enum GIS_DATA_SOURCE
{
	GDS_UNKNOWN = 0,					// 查询的时候可以不要输入
	GDS_SRTM,							// SRTM的DEM
	GDS_ASTGTM,							// ASTGTM的DEM
	GDS_NFDEM100K,						// 10万比例尺DEM
	GDS_NFDEM50K,						//  5万比例尺DEM
	GDS_NFDEM25K,						// 2万5比例尺DEM
	GDS_NFDEM10K,						//  1万比例尺DEM
	GDS_NFDEM5K,						//  5千比例尺DEM
	GDS_NFDEM2K,						//  2千比例尺DEM

	GDS_GEDOM = 100,					// GoogleEarth下载的DOM
	GDS_ZY02BDOM,						// 中巴资源卫星02B影像生成的DOM
	GDS_ZY02CDOM,						// 中巴资源卫星02C影像生成的DOM
	GDS_ZY03DOM,						// 资源三号影像生成的DOM
	GDS_NFDOM100K,						// 10万比例尺DOM
	GDS_NFDOM50K,						//  5万比例尺DOM
	GDS_NFDOM25K,						// 2万5比例尺DOM
	GDS_NFDOM10K,						//  1万比例尺DOM
	GDS_NFDOM5K,						//  5千比例尺DOM
	GDS_NFDOM2K,						//  2千比例尺DOM
};

//分辨率的单位
enum GSD_Unit
{
	GU_Meter = 0,
	GU_Degree,
};

class GISMANGER_LIB CGISManager
{
public:
	CGISManager();
	virtual ~CGISManager();

	//接口实现
public:
	//写入数据库
	virtual int  SubmitToDB(GisInfo* pInfo, int nInfo) = 0;

	//查询数据库
	virtual bool QueryFromDB(GisInfo sampleInfo, GisInfo** pInfo, int* nInfo) = 0;

	//释放数组
	static void FreeGisInfo(GisInfo* pInfo);
};

//初始化GIS
GISMANGER_LIB CGISManager* InitialGISManager(LinkPattern Linktype);

GISMANGER_LIB void	FreeGISManager(CGISManager* pGISManager);

GISMANGER_LIB bool  GetDataSourceName(GIS_DATA_SOURCE gds, char* strSourceName);

#endif // !defined(AFX_GISMANAGERS_H__D13A3F38_0215_40A5_8119_B283E3AED206__INCLUDED_)
