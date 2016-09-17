
#include "WTKAnalysis.h"

BOOL WKT2GCD(char **sWKT,char *strGcd)
{
	GeoRef pGeoRef;
	GetGeoFromWTK(pGeoRef, sWKT, "WKT");

	CSpGeoCvt pGeo;
    CSpPar7Cvt pGeo7;

    pGeo.Set_Cvt_Par( pGeoRef.elpsType,        //int elpsType,
				      pGeoRef.projType,        //int projType,
				      pGeoRef.a,               //double a,
				      pGeoRef.b,               //double b
				      pGeoRef.Origin_Latitude, //double Origin_Latitude,  // rad
				      pGeoRef.Central_Meridian,//double Central_Meridian, // rad
				      pGeoRef.False_Easting,   //double False_Easting,
				      pGeoRef.False_Northing,  //double False_Northing,
				      pGeoRef.Scale_Factor,    //double Scale_Factor,
				      pGeoRef.elpsType         //int eleType,
				      );	
    pGeo7.Set_Cvt_Par( 0.0, //double dx
	      			   0.0, //double dy
				       0.0, //double dz
				       0.0, //double rx
				       0.0, //double ry
				       0.0, //double rz
				       0.0  //double scale
				      );
	
	pGeo.Save2File(strGcd);
	pGeo7.Save2File(strGcd);

	return TRUE;
}

BOOL GetGeoFromWTK(GeoRef &geoRef, char **lpstrWTK, char *lpstrDescType)
{
	OGRSpatialReference geographicReference;
	geographicReference.importFromWkt(lpstrWTK);//引入WKT
	
	char strVal[1024];
	
	strcpy(strVal, geographicReference.GetAttrValue("GEOGCS")); 
	SetEllipsoidMode(strVal, geoRef, lpstrDescType);                                             //椭球类型
	CSpGeoCvt::GetEplsDefPar(geoRef.elpsType, &geoRef.a, &geoRef.b);                             //椭球长短半径	

	if (geographicReference.IsGeocentric()){
		geoRef.projType = GEODETIC_CENTRXYZ;
		return TRUE;
	}
	if (geographicReference.IsGeographic()){
		geoRef.projType = GEODETIC_LONLAT;
		return TRUE;
	}
	strcpy(strVal, geographicReference.GetAttrValue("PROJECTION"));
	SetProjectionMode(strVal, geoRef, lpstrDescType);                                            //投影类型
	
	geoRef.Scale_Factor     = geographicReference.GetProjParm(SRS_PP_SCALE_FACTOR);
	
	CSpGeoCvt::GetProjDefPar(geoRef.projType, &geoRef.Scale_Factor);                             //投影比例
	
	geoRef.Central_Meridian = geographicReference.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*SPGC_D2R; //中央经线
	geoRef.False_Easting    = geographicReference.GetProjParm(SRS_PP_FALSE_EASTING);             //东偏
	geoRef.False_Northing   = geographicReference.GetProjParm(SRS_PP_FALSE_NORTHING);            //北偏
	geoRef.Origin_Latitude  = geographicReference.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN);        //Origin_Latitude
	return TRUE;
}

BOOL SetEllipsoidMode(char *strVal, GeoRef &geoRef, char*strDescType)
{
	if (!strcmp(strDescType, "Standard"))
	{
		if      (!strcmp(strVal, "WGS84"))     { geoRef.elpsType = ET_WGS84;     return TRUE; }
		else if (!strcmp(strVal, "CGCS2000"))  { geoRef.elpsType = ET_CN2000;    return TRUE; }
		else if (!strcmp(strVal, "BeiJing54")) { geoRef.elpsType = ET_BEIJING54; return TRUE; }
		else if (!strcmp(strVal, "XiAn80"))    { geoRef.elpsType = ET_XIAN80;    return TRUE; }
		else return FALSE;
	}
	else if (!strcmp(strDescType, "WKT"))
	{
		if      (!strcmp(strVal, "GCS_WGS_1984"))     { geoRef.elpsType = ET_WGS84;     return TRUE; }
		else if (!strcmp(strVal, "GCS_CGCS_2000"))    { geoRef.elpsType = ET_CN2000;    return TRUE; }
		else if (!strcmp(strVal, "GCS_Beijing_1954")) { geoRef.elpsType = ET_BEIJING54; return TRUE; }
		else if (!strcmp(strVal, "GCS_Xian_1980"))    { geoRef.elpsType = ET_XIAN80;    return TRUE; }
		else return FALSE;
	}
	else return FALSE;
}

BOOL SetProjectionMode(char *strVal, GeoRef &geoRef, char*strDescType)
{
	if (!strcmp(strDescType, "Standard"))
	{
		if      (!strcmp(strVal, "Gauss"))  { geoRef.projType = GAUSS_PROJECTION;   return TRUE; }
		else if (!strcmp(strVal, "UTM"))    { geoRef.projType = UTM_PROJECTION;     return TRUE; }
		else if (!strcmp(strVal, "Albert")) { geoRef.projType = LAMBERT_PROJECTION; return TRUE; }
		else return FALSE;
	}
	else if (!strcmp(strDescType, "WKT"))
	{
		if      (!strcmp(strVal, "Gauss_Kruger"))        { geoRef.projType = GAUSS_PROJECTION;   return TRUE; }
		else if (!strcmp(strVal, "Transverse_Mercator")) { geoRef.projType = UTM_PROJECTION;     return TRUE; }
		else if (!strcmp(strVal, "Lambert"))             { geoRef.projType = LAMBERT_PROJECTION; return TRUE; }
		else return FALSE;
	}
	else return FALSE;
}

BOOL SetWTKFormGeo(GeoRef &geoRef, char **lpstrWTK,char *lpstrDescType)
{
	OGRSpatialReference geographicReference;

	//投影类型
	switch (geoRef.projType)
	{
	case GEODETIC_LONLAT:
		geographicReference.SetProjCS("Longitude_Latitude");
		break;
	case TM_PROJECTION:
		geographicReference.SetProjCS("TM");
		break;
	case GAUSS_PROJECTION:
		geographicReference.SetProjCS("Gauss");
		break;
	case UTM_PROJECTION:
		geographicReference.SetProjCS( "UTM");
		break;
	case LAMBERT_PROJECTION:
		geographicReference.SetProjCS("Albert");
		break;
	case GEODETIC_CENTRXYZ:  
		geographicReference.SetProjCS("Cen_XYZ");
		break;
	default:
		geographicReference.SetProjCS("Gauss");
		break;
	}

	//椭球类型
	switch (geoRef.elpsType)
	{
	case ET_WGS84:
		geographicReference.SetGeogCS("GCS_WGS_1984",
			"WGS_1984",
			"WGS_1984",
			geoRef.a,geoRef.a/geoRef.a-geoRef.b);
		break;
	case ET_BEIJING54:
		geographicReference.SetGeogCS("GCS_Beijing_1954",
			"Beijing_1954",
			"Beijing_1954",
			geoRef.a, geoRef.a / geoRef.a - geoRef.b);
		break;
	case ET_XIAN80:
		geographicReference.SetGeogCS("GCS_Xian_1980",
			"Xian_1980",
			"Xian_1980",
			geoRef.a, geoRef.a / geoRef.a - geoRef.b);
		break;
	case ET_CN2000:
		geographicReference.SetGeogCS("GCS_CGCS_2000",
			"CGCS_2000",
			"CGCS_2000",
			geoRef.a, geoRef.a / geoRef.a - geoRef.b);
		break;
	default:
		geographicReference.SetGeogCS("GCS_WGS_1984",
			"WGS_1984",
			"WGS_1984",
			geoRef.a, geoRef.a / geoRef.a - geoRef.b);
		break;
	}

	geographicReference.SetProjParm(SRS_PP_SCALE_FACTOR, geoRef.Scale_Factor);                  //投影比例
	geographicReference.SetProjParm(SRS_PP_CENTRAL_MERIDIAN, geoRef.Central_Meridian/SPGC_D2R); //中央经线
	geographicReference.SetProjParm(SRS_PP_FALSE_EASTING, geoRef.False_Easting);                //东偏
	geographicReference.SetProjParm(SRS_PP_FALSE_NORTHING, geoRef.False_Northing);              //北偏
	geographicReference.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN, geoRef.Origin_Latitude);         //Origin_Latitude

	geographicReference.exportToWkt(lpstrWTK);
	return TRUE;
}
