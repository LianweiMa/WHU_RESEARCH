#ifndef _WHU_PLANE_GEOMETRY_2014_07_06_MLW
#define _WHU_PLANE_GEOMETRY_2014_07_06_MLW
#include <stdio.h>
#include <math.h>
//////////////////////////////////////////////////////////////////////////
//ƽ�漸������
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*     ����ֱ��б��                                                         */
/*     author:malianwei,  date:2014-07-06                      */
/*     [in]:pt[2]����������                                             */
/*     [out]:resultLineSlope ���б��                           */
/*     ���óɹ�����true������ʧ�ܷ���false       */
/*     ��ע: ��ֱ�ߴ�ֱ��X��ʱ,����б��ֵΪ99999.0Ϊ��Чֵ      */
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
		resultLineSlope=99999.0;//��Чֵ
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
/*     �����߶��е�                                                         */
/*     author:malianwei,  date:2014-07-06                      */
/*     [in]:pt[2]����������                                             */
/*     [out]:resultMidpoint ����߶��е�����         */
/*     ���óɹ�����true������ʧ�ܷ���false       */
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
/* �����������                                                                     */
/* author:malianwei,  date:2014-07-06                      */
/* [in]  firstPt:�����һ������                                                                    */
/* [in] SecondPt: ����ڶ�������                                                                     */
/* [retern] �����������                                                                     */
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
/* ��������,������ȷ����Բ�Ĳ���(Բ��,�뾶)                                                                     */
/* author:malianwei,  date:2014-07-06                      */
/* [in]  pt[3]:��������                                                                   */
/* [out] resultCirclePara:���Բ�Ĳ���                                                                    */
/* [retern]����ɹ�����true�����򷵻�false                                                                     */
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
	if (bComputeLineSlope1&&lfLineSlope1!=0.0&&bComputeLineSlope2&&lfLineSlope2!=0.0)//1//k1!=0�Ҵ���,k2!=0�Ҵ���
	{
		lfLineSlope1=-1/lfLineSlope1;lfLineSlope2=-1/lfLineSlope2;
		lfIntercept1=ptMidpoint1.y-lfLineSlope1*ptMidpoint1.x;
		lfIntercept2=ptMidpoint2.y-lfLineSlope2*ptMidpoint2.x;
		resultCirclePara.CircleCenter.x=(lfIntercept2-lfIntercept1)/(lfLineSlope1-lfLineSlope2);//x=(b2-b1)/(k1-k2)
		resultCirclePara.CircleCenter.y=lfLineSlope1*resultCirclePara.CircleCenter.x+lfIntercept1;//y=k1*x+b1
	}
	if (bComputeLineSlope1&&lfLineSlope1!=0.0&&lfLineSlope2==0.0)//2//k1!=0�Ҵ���,k2==0
	{
		lfLineSlope1=-1/lfLineSlope1;
		lfIntercept1=ptMidpoint1.y-lfLineSlope1*ptMidpoint1.x;
		resultCirclePara.CircleCenter.x=ptMidpoint2.x;
		resultCirclePara.CircleCenter.y=lfLineSlope1*resultCirclePara.CircleCenter.x+lfIntercept1;//y=k*x+b
	}	
	if (bComputeLineSlope1&&lfLineSlope1!=0.0&&!bComputeLineSlope2)//3//k1!=0�Ҵ���,k2������
	{
		lfLineSlope1=-1/lfLineSlope1;
		lfIntercept1=ptMidpoint1.y-lfLineSlope1*ptMidpoint1.x;
		resultCirclePara.CircleCenter.y=ptMidpoint2.y;
		resultCirclePara.CircleCenter.x=(resultCirclePara.CircleCenter.y-lfIntercept1)/lfLineSlope1;//x=(y-b)/k
	}	
	if (!bComputeLineSlope1&&lfLineSlope2==0.0)//4//k1������,k2==0
	{
		resultCirclePara.CircleCenter.y=ptMidpoint1.y;
		resultCirclePara.CircleCenter.x=ptMidpoint2.x;
	}
	if (!bComputeLineSlope1&&bComputeLineSlope2&&lfLineSlope2!=0.0)//5//k1������,k2!=0�Ҵ���
	{
		lfLineSlope2=-1/lfLineSlope2;
		resultCirclePara.CircleCenter.y=ptMidpoint1.y;
		lfIntercept2=ptMidpoint2.y-lfLineSlope2*ptMidpoint2.x;
		resultCirclePara.CircleCenter.x=(resultCirclePara.CircleCenter.y-lfIntercept2)/lfLineSlope2;//x=(y-b)/k
	}
	if (lfLineSlope1==0.0&&bComputeLineSlope2&&lfLineSlope2!=0.0)//6//k1==0,k2!=0�Ҵ���
	{
		resultCirclePara.CircleCenter.x=ptMidpoint1.x;
		lfLineSlope2=-1/lfLineSlope2;
		lfIntercept2=ptMidpoint2.y-lfLineSlope2*ptMidpoint2.x;
		resultCirclePara.CircleCenter.y=lfLineSlope2*resultCirclePara.CircleCenter.x+lfIntercept2;//y=k*x+b
	}	
	if (bComputeLineSlope1&&lfLineSlope1==0.0&&!bComputeLineSlope2)//7//k1==0,k2������
	{
		resultCirclePara.CircleCenter.x=ptMidpoint1.x;
		resultCirclePara.CircleCenter.y=ptMidpoint2.y;
	}	
	resultCirclePara.CircleRadius=ComputeDistanceOfPts(pt[0],resultCirclePara.CircleCenter);
	//printf("\tCompute circle parameter success.\n");
	return true;
}
/************************************************************************/
/* �жϵ��Ƿ���Բ��                                                                     */
/* author:malianwei,  date:2014-07-06                      */
/* [in]pt:����ĵ�����                                                                     */
/* [in]CirclePara:�����Բ�Ĳ���                                                                    */
/* [retern]��Բ�ڷ���true�����򷵻�false                                                                     */
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
/* ����ֱ�ߵ�һ�㷽��                                                                     */
/* author:malianwei,  date:2014-07-06                      */
/* [in]pt[2]:������������                                                                     */
/* [out]A,B,C:���ֱ�߷��̲���(Ax+By+C=0)                                */
/* [return]����ɹ�����true,���򷵻�false                                         */
/************************************************************************/
template <typename T1,typename T2>
bool ComputeLineEquation(const T1 pt[2],T2 &A,T2 &B,T2 &C)
{
	//printf("\tCompute line equation...\n");
	T2 slope=0.0;
	bool bState=ComputeLineSlope(pt,slope);
	if (bState&&slope!=0)//б�ʴ����Ҳ�Ϊ0
	{
		A=slope;
		B=-1;
		C=pt[0].y-slope*pt[0].x;
	}
	if (slope==0)//б��Ϊ0
	{
		A=slope;
		B=-1;
		C=pt[0].y;
	}
	if (!bState)//б�ʲ�����
	{
		A=1;
		B=0;
		C=-pt[0].x;
	}
	//printf("\tCompute line equation success.\n");
	return true;
}
/************************************************************************/
/* �жϲ�������һ������ֱ�ߣ�����������һ��                                                                     */
/* author:malianwei,  date:2014-07-06                      */
/* [in] pt[2]:ֱ��������                                                                                           */
/* [in] insetPt:���жϵĵ�                                                                                      */
/* [return] 1:��ֱ��ͬ��,0:��ֱ����,-1:��ֱ�����                                    */
/************************************************************************/
template <typename T>
int JudgePtOfLineSide(const T pt[3],const T &insertPt)
{
	T lineVector1,lineVector2;
	lineVector1.x=pt[1].x-pt[0].x;//pt[0]Ϊ��㣬pt[1]Ϊ�յ�//AB
	lineVector1.y=pt[1].y-pt[0].y;
	lineVector1.z=pt[1].z-pt[0].z;
	lineVector2.x=pt[2].x-pt[0].x;//pt[0]Ϊ��㣬pt[2]Ϊ�յ�//AC
	lineVector2.y=pt[2].y-pt[0].y;
	lineVector2.z=pt[2].z-pt[0].z;
	T insertLineVector;//AP
	insertLineVector.x=insertPt.x-pt[0].x;
	insertLineVector.y=insertPt.y-pt[0].y;
	insertLineVector.z=insertPt.z-pt[0].z;
	T crossProduct1=CorssProduct(lineVector1,insertLineVector);
	T crossProduct2=CorssProduct(lineVector1,lineVector2);
	double dotProduct=DotProduct(crossProduct1,crossProduct2);
	if (dotProduct>0)return 1;//ͬ��
	else if (dotProduct==0)return 0;//��ֱ����
	else return -1;///���
}
#endif