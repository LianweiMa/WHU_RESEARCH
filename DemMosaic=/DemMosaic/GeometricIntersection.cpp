#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>

#include "GeometricInsection.h"

using namespace std;

bool operator < (const PT& p1, const PT& p2)
{
	bool flag1 = (p1.x < p2.x) ? true : false;
	if (p1.x == p2.x)
	{
		flag1 = (p1.y < p2.y) ? true : false;
	}
	return flag1;
}

double cross(PT sp, PT ep, PT op)
{
	return((sp.x-op.x)*(ep.y-op.y)-(ep.x-op.x)*(sp.y-op.y)); 
}
bool LineIntersection(Line l1, Line l2)
{

	bool br = 
		(xxwMax(l1.s.x, l1.e.x) >= xxwMin(l2.s.x, l2.e.x) &&//排斥实验
		xxwMax(l2.s.x, l2.e.x) >= xxwMin(l1.s.x, l1.e.x) &&
		xxwMax(l1.s.y, l1.e.y) >= xxwMin(l2.s.y, l2.e.y) &&
		xxwMax(l2.s.y, l2.e.y) >= xxwMin(l1.s.y, l1.e.y) &&
			 (cross(l2.s, l1.e, l1.s) * cross(l1.e, l2.e, l1.s) >= 0) &&//跨立实验
			 (cross(l1.s, l2.e, l2.s) * cross(l1.e, l1.e, l1.s) >= 0));
	return br;
}
Line MakeLine(PT p1, PT p2)
{
	Line l;
	int sign = 1;
	l.s = p1;
	l.e = p2;
	if (p1.y == p2.y)// l//x-axis
	{
		l.a = 0.0;
		l.b = 1;
		l.c = -p1.y;
		return l;
	}

	if (p1.x == p2.x)//l//y-axis
	{
		l.a = 1;
		l.b = 0.0;
		l.c = -p1.x;
		return l;
	}
	l.a = p2.y - p1.y;
	l.b = -1*(p2.x - p1.x);
	l.c = p1.y * p2.x - p1.x * p2.y;
	return l;
}

double slope(Line l)
{
	if (fabs(l.a) < 1e-20)
	{
		return 0;
	}
	if (fabs(l.b) < 1e-20)
	{
		return MAXFLOAT;
	}
	return -(l.a / l.b);
}

bool isparallelLine(Line l1, Line l2)
{
	Line u, v;
	double k1 = slope(l1);
	double k2 = slope(l2);
	double k = fabs(k1 - k2);
	int flag = (k < EP) ? 1 : 0;
	if (flag)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool isPTonLine(PT p, Line l)
{
	/*double x = cross(l.e, p, l.s);

	printf("cross : %lf\n", x);*/

#define eps 0.0005
#if 0
return( (cross(l.e,p,l.s)==0) &&
		    ( ( (p.x-l.s.x) * (p.x-l.e.x) <=0 ) &&
			  ( (p.y-l.s.y) * (p.y-l.e.y) <=0 ) ) ); 
#endif

return ( (fabs( cross(l.e,p,l.s) ) <= eps) &&
	( ( (p.x-l.s.x) * (p.x-l.e.x) <=0 ) &&
	( (p.y-l.s.y) * (p.y-l.e.y) <=0 ) ) ); 
}
PT LineIntersectPoint(Line l1, Line l2)
{
	PT p;
	double d = l1.a * l2.b - l1.b * l2.a;
	if (fabs(d) < 1.0e-10)
	{
		p.x = -1;
		p.y = -1;
	}
	else
	{
		p.x = (l2.c * l1.b - l1.c * l2.b)/d;
		p.y = (l2.a * l1.c - l1.a * l2.c)/d;
	}
	return p;
}
double angle(PT p1, PT p2)
{
	if (p2.x == p1.x)
	{
		if (p2.y < p1.y)
		{
			return - PI/2;
		}
		if (p2.y >= p1.y)
		{
			return PI/2;
		}
	}
	double a = atan2(p2.y - p1.y, p2.x - p1.x);
	return a;
}
int io_polygon(PT p, int n, vector<PT> vPts)
{
	PT p1, p2;
	double sum_angle = 0.0, a;
	for (int i = 0; i < n; i++)
	{
		p1.x = vPts[(i + 1) % n].x;
		p1.y = vPts[(i + 1) % n].y;
		p2.x = vPts[i % n].x;
		p2.y = vPts[i % n].y;
		if ( (abs(p.x - p1.x) < EP && abs(p.y - p1.y) < EP) || 
			 (abs(p.x - p2.x) < EP && abs(p.y - p2.y) < EP) )
		{
			return 2;
			break;
		}
		//计算p1p,p2p矢量的夹角
		a = angle(p, p1) - angle(p, p2);
		if (fabs(fabs(a) - PI) < 0.0000001)
		{
			return 3;//点在多边形边上，不含端点
			break;
		}

		if ( a > PI)
		{
			a = a - 2*PI;
		}
		if (a < -PI)
		{
			a = 2 * PI + a;
		}
		sum_angle += a;
	}

	if (fabs(sum_angle) < EP)
	{
		return 0;//点在多边形外
	}
	else if (fabs(fabs(sum_angle) - 2 * PI) < EP)
	{
		return 1;//点在多边形内
	}
	else
	{
		return 2;//点在多边形端点上
	}
}


void pinsertPts(Line l, int n, vector<PT> vPt, vector<PT> &vPTcor)
{
	PT s, e;
	s = l.s; e = l.e;

	//判断直线与多边形的关系, 0-外， 1-内，2-端点，3-边上
	int ns = io_polygon(s, n, vPt);
	int ne = io_polygon(e, n, vPt);

	if (ns == 0 && ne == 0)//直线在多边形外
	{
		return;
	}

	else if (ns == 1 && ne == 1)//直线在多边形内，则保留两个端点
	{
		vPTcor.push_back(s);
		vPTcor.push_back(e);
		return;
	}
	else//直线与多边形相交
	{

		if (ns == 1 && ne == 0)//端点s在多边形内
		{
			vPTcor.push_back(s);
		}
		if (ns == 0 && ne == 1)//端点e在多边形内
		{
			vPTcor.push_back(e);
		}
		//分别判断直线与多边形边的交点
		for (int i = 0; i < n; i++)
		{
			PT p1, p2;
			p1.x=vPt[(i+1)%n].x;
			p1.y=vPt[(i+1)%n].y;
			p2.x=vPt[i%n].x;
		    p2.y=vPt[i%n].y;
			Line l2 = MakeLine(p2, p1);

			if (LineIntersection(l, l2))//两条线相交
			{

				if (isparallelLine(l, l2)){//两条线平行
					//1. s 在l2上， e不在
					if (isPTonLine(l.s, l2) && (!isPTonLine(l.e, l2))){
						vPTcor.push_back(l.s);
					}
					//2. s不在l2上，e在
					if ((!isPTonLine(l.s, l2)) && isPTonLine(l.e, l2)){
						vPTcor.push_back(l.e);
					}
					//3. s和e都在l2上
					if (isPTonLine(l.s, l2) && isPTonLine(l.e, l2)){
						vPTcor.push_back(l.s);
						vPTcor.push_back(l.e);
					}
					//4. s和e都不在l2上，说明l2是l的子集
					if ((!isPTonLine(l.s, l2)) && (!isPTonLine(l.e, l2))){
						vPTcor.push_back(l2.s);
						vPTcor.push_back(l2.e);
					}
				}
				else{//求交点
					PT p = LineIntersectPoint(l, l2);//求交点
					//判断交点是否在l2上,同时该点必须在l1上
					bool ncheck1 = isPTonLine(p, l2);
					bool ncheck2 = isPTonLine(p, l);
					if (ncheck1 && ncheck2){
						vPTcor.push_back(p);
					}
				}
			}
		}
	}
}

void IntersectionAB(vector<PT> vPtA, vector<PT> vPtB, vector<PT> &vPTcor)
{
	vPTcor.clear();

	int nA = vPtA.size();
	int nB = vPtB.size();

	for (int i = 0; i < nA; i++)
	{

		PT p1, p2;
		p1.x=vPtA[(i+1)%nA].x;
		p1.y=vPtA[(i+1)%nA].y;
		p2.x=vPtA[i%nA].x;
		p2.y=vPtA[i%nA].y;
		Line l = MakeLine(p2, p1);
		pinsertPts(l, nB, vPtB, vPTcor);
	}

	for (int i = 0; i < nB; i++)
	{
		PT p1, p2;
		p1.x=vPtB[(i+1)%nA].x;
		p1.y=vPtB[(i+1)%nA].y;
		p2.x=vPtB[i%nA].x;
		p2.y=vPtB[i%nA].y;
		Line l = MakeLine(p2, p1);
		pinsertPts(l, nA, vPtA, vPTcor);
	}

}

double CalculateArea(vector<PT> vPt)
{
	double sum = 0;
	int n = vPt.size();
	if (n <= 2)
	{
		return 0;
	}
	else
	{
		for (int i = 0; i < n - 1; i++)
		{
			sum += (vPt[i].x - vPt[0].x) * (vPt[i + 1].y - vPt[0].y) - 
				(vPt[i + 1].x - vPt[0].x) * (vPt[i].y - vPt[0].y);
		}

		sum = 0.5 * fabs(sum);
		return sum;
	}

}

void Segclockwise(vector<PT> &vPt)
{
	vector<PT> vPt_tmp(vPt);
	int pt_num = vPt_tmp.size();

	vPt.clear();
	vPt.resize(pt_num);
	if (pt_num == 0)
	{
		return;
	}
	vPt[0].x = vPt_tmp[0].x;
	vPt[0].y = vPt_tmp[0].y;

	//double *ang = new double[pt_num];
	vector<double> vang(pt_num - 1);
	int *vmask = new int[pt_num - 1];
	memset(vmask, 0, (pt_num - 1) * sizeof(int));

	PT ps;
	ps.x = vPt_tmp[0].x; ps.y = vPt_tmp[0].y;
	for (int i = 0; i < pt_num - 1; i++)
	{
		PT pe;
		pe.x = vPt_tmp[i + 1].x;
		pe.y = vPt_tmp[i + 1].y;

		vang[i] = angle(ps, pe);
	}

	double amin = vang[0];
	//int adder[3];
	int *adder = new int[pt_num - 1];

	for (int i = 0; i < pt_num - 1; i++)
	{
		for(int j = 0; j < pt_num - 1; j++)
		{
			if(vmask[j] == 0)
			{
				amin = vang[j];
				break;
			}
		}

		//寻找最小的数的位置
		for (int j = 0; j < pt_num - 1; j++)
		{
			if (vmask[j] == 1)
			{
				continue;
			}
			if(vang[j] <= amin)
			{
				amin = vang[j];
				adder[i] = j;
			}
		}
		//printf("find : %d\n", adder[i]);
		vmask[adder[i]] = 1;//已经访问

		vPt[i + 1].x = vPt_tmp[adder[i] + 1].x;
		vPt[i + 1].y = vPt_tmp[adder[i] + 1].y;
	}
	
	vPt_tmp.clear();
	vang.clear();
	delete [] vmask; vmask = NULL;
	delete [] adder; adder = NULL;

	fflush(stdout);
}
void DelRepeatPt(vector<PT> &vsame)
{
	std::set<PT> setPT(vsame.begin(), vsame.end());
	vector<PT> vInterAB;
	for (std::set<PT>::iterator p = setPT.begin(); p != setPT.end(); p++)
	{
		vInterAB.push_back(*p);
	}
	vsame.clear();
	for (int i = 0; i < vInterAB.size(); i++)
	{
		vsame.push_back(vInterAB.at(i));
	}
	setPT.clear();
}
double DistaceAB(PT &pt1, PT &pt2)//求两点距离
{
	return pow(pow(pt1.x - pt2.x, 2.0) + pow(pt1.y - pt2.y,2.0), 0.5);
}