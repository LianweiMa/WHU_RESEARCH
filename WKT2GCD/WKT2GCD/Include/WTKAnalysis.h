
#include "windows.h"
#include "SpGeoCvt.h"
#include "gdal_1_10_1/ogr_geometry.h"

typedef struct tagGEOREFF
{
	int    elpsType;          //椭球类型
	int    projType;          //投影类型
	double a, b;              //椭球长短半径
	double Origin_Latitude;   //初始纬度
	double Central_Meridian;  //中央经线
	double False_Easting;     //东偏
	double False_Northing;    //北偏
	double Scale_Factor;      //投影比例
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
