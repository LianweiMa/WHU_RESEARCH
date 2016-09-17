
#include "windows.h"
#include "SpGeoCvt.h"
#include "gdal_1_10_1/ogr_geometry.h"

typedef struct tagGEOREFF
{
	int    elpsType;          //��������
	int    projType;          //ͶӰ����
	double a, b;              //���򳤶̰뾶
	double Origin_Latitude;   //��ʼγ��
	double Central_Meridian;  //���뾭��
	double False_Easting;     //��ƫ
	double False_Northing;    //��ƫ
	double Scale_Factor;      //ͶӰ����
	int    eleType;

	struct tagGEOREFF()
	{
		elpsType        = ET_WGS84;
		projType        = GAUSS_PROJECTION;
		a               = SEMIMAJOR_WGS84; 
		b               = SEMIMINOR_WGS84;
		Origin_Latitude = 0.0;
		Central_Meridian= 117 * SPGC_D2R;
		False_Easting   = 500000.0;
		False_Northing  = 0.0;
		Scale_Factor    = 1.0;
		eleType         = 0;
	};
}GeoRef;


BOOL WKT2GCD(char **sWKT,char *strGcd);
BOOL GetGeoFromWTK(GeoRef &geoRef, char **lpstrWTK, char *lpstrDescType = "WTK");
BOOL SetEllipsoidMode (char *strVal, GeoRef &geoRef, char*strDescType);
BOOL SetProjectionMode(char *strVal, GeoRef &geoRef, char*strDescType);
BOOL SetWTKFormGeo(GeoRef &geoRef, char **lpstrWTK, char *lpstrDescType = "Standard");
