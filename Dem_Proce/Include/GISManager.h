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

//��ʾ��Ľṹ�壺
typedef struct tagVertex2D			//�������
{
	double x;						//����
	double y;						//γ��
}Vertex2D;

//��ʾGIS�������ԵĽṹ�壺
typedef struct  tagGISINFO				//GIS���ݵ�����
{
	char	 filepath[512];				//�ļ���ַ
	short	 filetype;					//�������ͣ�DEM����DOM
	short	 subsourcetype;				//������Դ����SRTM��ASTGTM
	int		 gsdunit;					//����ֱ��ʵĵ�λ
	double	 gsd;						//����ֱ���
	double	 precison;					//���ξ���
	Vertex2D edgepoint[4];				//4���ǵ�ľ�γ��
	Vertex2D centerpoint;				//���ĵ�ľ�γ��(��������)
	char	 shoottime[16];				//����ʱ��
	char	 updatetime[16];			//����ʱ��
	char	 sensor[16];				//�������ݵĴ���������(���Ըĳ�int)
	char	 coord[256];				//����ϵ�����ļ�
	char	 productor[256];			//������λ�͵���
	char	 propertyone[256];			//Ԥ���ֶ�1
	char	 propertytwo[256];			//Ԥ���ֶ�2
}GisInfo;

//GIS��������
enum GIS_DATA_TYPE
{
	GDT_DOM = 0,
	GDT_DEM
};

enum GIS_DATA_SOURCE
{
	GDS_UNKNOWN = 0,					// ��ѯ��ʱ����Բ�Ҫ����
	GDS_SRTM,							// SRTM��DEM
	GDS_ASTGTM,							// ASTGTM��DEM
	GDS_NFDEM100K,						// 10�������DEM
	GDS_NFDEM50K,						//  5�������DEM
	GDS_NFDEM25K,						// 2��5������DEM
	GDS_NFDEM10K,						//  1�������DEM
	GDS_NFDEM5K,						//  5ǧ������DEM
	GDS_NFDEM2K,						//  2ǧ������DEM

	GDS_GEDOM = 100,					// GoogleEarth���ص�DOM
	GDS_ZY02BDOM,						// �а���Դ����02BӰ�����ɵ�DOM
	GDS_ZY02CDOM,						// �а���Դ����02CӰ�����ɵ�DOM
	GDS_ZY03DOM,						// ��Դ����Ӱ�����ɵ�DOM
	GDS_NFDOM100K,						// 10�������DOM
	GDS_NFDOM50K,						//  5�������DOM
	GDS_NFDOM25K,						// 2��5������DOM
	GDS_NFDOM10K,						//  1�������DOM
	GDS_NFDOM5K,						//  5ǧ������DOM
	GDS_NFDOM2K,						//  2ǧ������DOM
};

//�ֱ��ʵĵ�λ
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

	//�ӿ�ʵ��
public:
	//д�����ݿ�
	virtual int  SubmitToDB(GisInfo* pInfo, int nInfo) = 0;

	//��ѯ���ݿ�
	virtual bool QueryFromDB(GisInfo sampleInfo, GisInfo** pInfo, int* nInfo) = 0;

	//�ͷ�����
	static void FreeGisInfo(GisInfo* pInfo);
};

//��ʼ��GIS
GISMANGER_LIB CGISManager* InitialGISManager(LinkPattern Linktype);

GISMANGER_LIB void	FreeGISManager(CGISManager* pGISManager);

GISMANGER_LIB bool  GetDataSourceName(GIS_DATA_SOURCE gds, char* strSourceName);

#endif // !defined(AFX_GISMANAGERS_H__D13A3F38_0215_40A5_8119_B283E3AED206__INCLUDED_)
