#ifndef _WHU_PLANE_GEOMETRY_2014_07_06_MLW
#define _WHU_PLANE_GEOMETRY_2014_07_06_MLW
#include <stdio.h>
#include <math.h>
//////////////////////////////////////////////////////////////////////////
//平面几何运算
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*     计算直线斜率                                                         */
/*     author:malianwei,  date:2014-07-06                      */
/*     [in]:pt[2]输入两个点                                             */
/*     [out]:resultLineSlope 输出斜率                           */
/*     调用成功返回true，调用失败返回false       */
/*     备注: 当直线垂直于X轴时,定义斜率值为99999.0为无效值      */
/************************************************************************/
template <typename T1,typename T2>
bool ComputeLineSlope(const T1 pt[2],T2 &resultLineSlope)
{
	//printf("\tCompute line slope...\n");
	T1 firstPt,SecondPt;
	firstPt.x=pt[0].x;firstPt.y=pt[0].y;firstPt.z=pt[0].z;
	SecondPt.x=pt[1].x;SecondPt.y=pt[1].y;SecondPt.z=pt[1].z;
	T2 deltaX=(T2)(firstPt.x-SecondPt.x),deltaY=(T2)(firstPt.y-SecondPt.y),deltaZ=(T2)(firstPt.z-SecondPt.z);
	if (deltaX==0.0)
	{
		//printf("\tThis line parallels Y axis.\n");
		resultLineSlope=99999.0;//无效值
		return false;
	}
	else
	{
		resultLineSlope=deltaY/deltaX;
		//printf("\tThis line slope: %lf\n",resultLineSlope);
		return true;
	}
}
/************************************************************************/
/*     计算线段中点                                                         */
/*     author:malianwei,  date:2014-07-06                      */
/*     [in]:pt[2]输入两个点                                             */
/*     [out]:resultMidpoint 输出线段中点坐标         */
/*     调用成功返回true，调用失败返回false       */
/************************************************************************/
template <typename T>
bool ComputeLineMidpoint(const T pt[2],T &resultMidpoint)
{
	//printf("\tCompute line midpoint...\n");
	T firstPt,SecondPt;
	firstPt.x=pt[0].x;firstPt.y=pt[0].y;firstPt.z=pt[0].z;
	SecondPt.x=pt[1].x;SecondPt.y=pt[1].y;SecondPt.z=pt[1].z;
	resultMidpoint.x=(firstPt.x+SecondPt.x)/2;
	resultMidpoint.y=(firstPt.y+SecondPt.y)/2;
	resultMidpoint.z=(firstPt.z+SecondPt.z)/2;
	//printf("\tThis line midpoint: %lf,%lf,%lf\n",resultMidpoint.x,resultMidpoint.y,resultMidpoint.z);
	return true;
}
/************************************************************************/
/* 计算两点距离                                                                     */
/* author:malianwei,  date:2014-07-06                      */
/* [in]  firstPt:输入第一点坐标                                                                    */
/* [in] SecondPt: 输入第二点坐标                                                                     */
/* [retern] 返回两点距离                                                                     */
/************************************************************************/
template <typename T>
double ComputeDistanceOfPts(const T firstPt,const T SecondPt)
{
	//printf("\tCompute distance between two points...\n");
	double deltaX=(firstPt.x-SecondPt.x),deltaY=(firstPt.y-SecondPt.y),deltaZ=(firstPt.z-SecondPt.z);
	double resultDistance2=deltaX*deltaX+deltaY*deltaY;
	double resultDistance=pow(resultDistance2,0.5);
	//printf("\tThis distance between two points:%lf\n",resultDistance);
	return resultDistance;
}
/************************************************************************/
/* 给定三点,计算所确定的圆的参数(圆心,半径)                                                                     */
/* author:malianwei,  date:2014-07-06                      */
/* [in]  pt[3]:三点坐标                                                                   */
/* [out] resultCirclePara:输出圆的参数                                                                    */
/* [retern]计算成功返回true，否则返回false                                                                     */
/************************************************************************/
template <typename T1,typename T2>
bool ComputeCirclePara(const T1 pt[3],T2 &resultCirclePara)
{
	//printf("\tCompute circle parameter...\n");
	T1 ptMidpoint1,ptMidpoint2;
	ComputeLineMidpoint(pt,ptMidpoint1);
	ComputeLineMidpoint(pt+1,ptMidpoint2);
	double lfLineSlope1=0.0,lfLineSlope2=0.0,lfIntercept1=0.0,lfIntercept2=0.0;
	bool bComputeLineSlope1=ComputeLineSlope(pt,lfLineSlope1);
	bool bComputeLineSlope2=ComputeLineSlope(pt+1,lfLineSlope2);
	if (bComputeLineSlope1&&lfLineSlope1!=0.0&&bComputeLineSlope2&&lfLineSlope2!=0.0)//1//k1!=0且存在,k2!=0且存在
	{
		lfLineSlope1=-1/lfLineSlope1;lfLineSlope2=-1/lfLineSlope2;
		lfIntercept1=ptMidpoint1.y-lfLineSlope1*ptMidpoint1.x;
		lfIntercept2=ptMidpoint2.y-lfLineSlope2*ptMidpoint2.x;
		resultCirclePara.CircleCenter.x=(lfIntercept2-lfIntercept1)/(lfLineSlope1-lfLineSlope2);//x=(b2-b1)/(k1-k2)
		resultCirclePara.CircleCenter.y=lfLineSlope1*resultCirclePara.CircleCenter.x+lfIntercept1;//y=k1*x+b1
	}
	if (bComputeLineSlope1&&lfLineSlope1!=0.0&&lfLineSlope2==0.0)//2//k1!=0且存在,k2==0
	{
		lfLineSlope1=-1/lfLineSlope1;
		lfIntercept1=ptMidpoint1.y-lfLineSlope1*ptMidpoint1.x;
		resultCirclePara.CircleCenter.x=ptMidpoint2.x;
		resultCirclePara.CircleCenter.y=lfLineSlope1*resultCirclePara.CircleCenter.x+lfIntercept1;//y=k*x+b
	}	
	if (bComputeLineSlope1&&lfLineSlope1!=0.0&&!bComputeLineSlope2)//3//k1!=0且存在,k2不存在
	{
		lfLineSlope1=-1/lfLineSlope1;
		lfIntercept1=ptMidpoint1.y-lfLineSlope1*ptMidpoint1.x;
		resultCirclePara.CircleCenter.y=ptMidpoint2.y;
		resultCirclePara.CircleCenter.x=(resultCirclePara.CircleCenter.y-lfIntercept1)/lfLineSlope1;//x=(y-b)/k
	}	
	if (!bComputeLineSlope1&&lfLineSlope2==0.0)//4//k1不存在,k2==0
	{
		resultCirclePara.CircleCenter.y=ptMidpoint1.y;
		resultCirclePara.CircleCenter.x=ptMidpoint2.x;
	}
	if (!bComputeLineSlope1&&bComputeLineSlope2&&lfLineSlope2!=0.0)//5//k1不存在,k2!=0且存在
	{
		lfLineSlope2=-1/lfLineSlope2;
		resultCirclePara.CircleCenter.y=ptMidpoint1.y;
		lfIntercept2=ptMidpoint2.y-lfLineSlope2*ptMidpoint2.x;
		resultCirclePara.CircleCenter.x=(resultCirclePara.CircleCenter.y-lfIntercept2)/lfLineSlope2;//x=(y-b)/k
	}
	if (lfLineSlope1==0.0&&bComputeLineSlope2&&lfLineSlope2!=0.0)//6//k1==0,k2!=0且存在
	{
		resultCirclePara.CircleCenter.x=ptMidpoint1.x;
		lfLineSlope2=-1/lfLineSlope2;
		lfIntercept2=ptMidpoint2.y-lfLineSlope2*ptMidpoint2.x;
		resultCirclePara.CircleCenter.y=lfLineSlope2*resultCirclePara.CircleCenter.x+lfIntercept2;//y=k*x+b
	}	
	if (bComputeLineSlope1&&lfLineSlope1==0.0&&!bComputeLineSlope2)//7//k1==0,k2不存在
	{
		resultCirclePara.CircleCenter.x=ptMidpoint1.x;
		resultCirclePara.CircleCenter.y=ptMidpoint2.y;
	}	
	resultCirclePara.CircleRadius=ComputeDistanceOfPts(pt[0],resultCirclePara.CircleCenter);
	//printf("\tCompute circle parameter success.\n");
	return true;
}
/************************************************************************/
/* 判断点是否在圆内                                                                     */
/* author:malianwei,  date:2014-07-06                      */
/* [in]pt:输入的点坐标                                                                     */
/* [in]CirclePara:输入的圆的参数                                                                    */
/* [retern]在圆内返回true，否则返回false                                                                     */
/************************************************************************/
template <typename T1,typename T2>
bool IsInCircle(const T1 &pt,const T2 CirclePara)
{
	double lfDistance=ComputeDistanceOfPts(pt,CirclePara.CircleCenter);
	if (lfDistance<CirclePara.CircleRadius)
		return true;
	else return false;
}
/************************************************************************/
/* 计算直线的一般方程                                                                     */
/* author:malianwei,  date:2014-07-06                      */
/* [in]pt[2]:输入两点坐标                                                                     */
/* [out]A,B,C:输出直线方程参数(Ax+By+C=0)                                */
/* [return]计算成功返回true,否则返回false                                         */
/************************************************************************/
template <typename T1,typename T2>
bool ComputeLineEquation(const T1 pt[2],T2 &A,T2 &B,T2 &C)
{
	//printf("\tCompute line equation...\n");
	T2 slope=0.0;
	bool bState=ComputeLineSlope(pt,slope);
	if (bState&&slope!=0)//斜率存在且不为0
	{
		A=slope;
		B=-1;
		C=pt[0].y-slope*pt[0].x;
	}
	if (slope==0)//斜率为0
	{
		A=slope;
		B=-1;
		C=pt[0].y;
	}
	if (!bState)//斜率不存在
	{
		A=1;
		B=0;
		C=-pt[0].x;
	}
	//printf("\tCompute line equation success.\n");
	return true;
}
/************************************************************************/
/* 判断插入点和另一顶点在直线（向量）的哪一侧                                                                     */
/* author:malianwei,  date:2014-07-06                      */
/* [in] pt[2]:直线上两点                                                                                           */
/* [in] insetPt:待判断的点                                                                                      */
/* [return] 1:在直线同侧,0:在直线上,-1:在直线异侧                                    */
/************************************************************************/
template <typename T>
int JudgePtOfLineSide(const T pt[3],const T &insertPt)
{
	T lineVector1,lineVector2;
	lineVector1.x=pt[1].x-pt[0].x;//pt[0]为起点，pt[1]为终点//AB
	lineVector1.y=pt[1].y-pt[0].y;
	lineVector1.z=pt[1].z-pt[0].z;
	lineVector2.x=pt[2].x-pt[0].x;//pt[0]为起点，pt[2]为终点//AC
	lineVector2.y=pt[2].y-pt[0].y;
	lineVector2.z=pt[2].z-pt[0].z;
	T insertLineVector;//AP
	insertLineVector.x=insertPt.x-pt[0].x;
	insertLineVector.y=insertPt.y-pt[0].y;
	insertLineVector.z=insertPt.z-pt[0].z;
	T crossProduct1=CorssProduct(lineVector1,insertLineVector);
	T crossProduct2=CorssProduct(lineVector1,lineVector2);
	double dotProduct=DotProduct(crossProduct1,crossProduct2);
	if (dotProduct>0)return 1;//同侧
	else if (dotProduct==0)return 0;//在直线上
	else return -1;///异侧
}
#endif