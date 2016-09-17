/************************************************************************/
/* Function :  -- Find the intersection of two polygons
   Author   :  -- Xie Xunwei
   Date     :  -- 2014/12/09                                            */
/************************************************************************/
#ifndef GEOMETRICINTERSECTION_H
#define GEOMETRICINTERSECTION_H

#include "math.h"
#include <vector>
#include <set>
using std::vector;

#define MAXFLOAT 1.0e10
#define MINFLOAT -1.0e10
#define EP       1.0e-10
#define PI      3.141592653589793


#define xxwMax(a, b) ((a) > (b) ) ? (a) : (b)
#define xxwMin(a, b) ((a) < (b) ) ? (a) : (b)


typedef struct tagPT{
	double x, y;
	tagPT()
	{
		x = y = MINFLOAT;
	}

	tagPT& operator = (const tagPT& p)
	{
		x = p.x;
		y = p.y;
		return *this;
	}
	
}PT;

bool operator < (const PT& p1, const PT& p2);

typedef struct tagLine{
	PT s, e;
	double a, b, c;//ax + by + c = 0

	tagLine& operator = (const tagLine& l)
	{
		s = l.s;
		e = l.e;
		a = l.a;
		b = l.b;
		c = l.c;
		return *this;
	}
}Line;


double cross(PT sp, PT ep, PT op);//calculate the cross product
  bool LineIntersection(Line l1, Line l2);// whether two lines intersection or not
  Line MakeLine(PT p1, PT p2);// make a line via two points
double slope(Line l);//calculate the slope of the line
  bool isparallelLine(Line l1, Line l2);//whether two lines parallel or not
  bool isPTonLine(PT p, Line l);//whether point is on the line or not
    PT LineIntersectPoint(Line l1, Line l2);//calculate the intersect point of two lines
double angle(PT p1, PT p2);//atan(p1/p2)
   int io_polygon(PT p, int n, vector<PT> vPts);//the relationship between the point and the polygon
  
  void pinsertPts(Line l, int n, vector<PT> vPt, vector<PT> &vPTcor);//判断直线l和多边形vPT的交点，将交点存入候选点集vPTcor
  void IntersectionAB(vector<PT> vPtA, vector<PT> vPtB, vector<PT> &vPTcor);//判断两个多边形的交集点集，A、B中的点均按逆时针排列

double CalculateArea(vector<PT> vPt);//计算任意多边形的面积

void Segclockwise(vector<PT> &vPt);//将一堆点按顺时针排序
void DelRepeatPt(vector<PT> &vsame);//删除重复点
double DistaceAB(PT &pt1, PT &pt2);//求两点距离
#endif