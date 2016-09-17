#include <stdio.h>
#include "vector_mlw.h"
#include "plane_geometry_mlw.h"
#include "algebra_mlw.h"
#include <math.h>
typedef struct tagDPT3D
{
	double x,y,z;
	struct tagDPT3D()
	{
		x=0.0;y=0.0;z=0.0;
	}
}DPT3D,LineVector;
typedef struct tagTriangle
{
	unsigned int uTriIndex;//该三角形的ID
	unsigned int uVertexIndex[3];//该三角形的顶点ID
	unsigned int uAdjacentTriIndex[3];//该三角形的邻接三角形ID
	struct tagTriangle()
	{
		uTriIndex=99999;
		for (int i=0;i<3;i++)
		{
			uVertexIndex[i]=99999;//无效值
			uAdjacentTriIndex[i]=99999;//无效值
		}
	}
}Tri;
typedef struct tagCircle
{
	DPT3D CircleCenter;
	double CircleRadius;
	struct tagCircle()
	{
		CircleRadius=0.0;
	}
}Circle;
/************************************************************************/
/* 判断点在哪个三角形内                                                                     */
/* [in]queryPt:输入需要查询的点                                                         */
/* [in]uTriIndex:查询点所在的三角形的索引                                     */
/* [in]pSrcPtData:源点数据                                                                     */
/* [in]triangle:当前总的三角形                                                               */
/* [return] -2:表示点不在在三角形内,-1:表示点在三角形内,0:表示点在三角形第一条边上,1:表示点在三角形第二条边上,2:表示点在三角形第三条边上*/
/************************************************************************/
int JudgePtInWhTri(const DPT3D &queryPt,unsigned int &uTriIndex,const DPT3D*pSrcPtData,const std::vector<Tri>&triangle)
{
	int iLineEdgeIndex=-2;
	uTriIndex=99999;
	DPT3D A,B,C;
	for (unsigned int i=0;i<(unsigned int)triangle.size();i++)//遍历所有三角形
	{
		//获取三角形的三个顶点
		A=pSrcPtData[triangle[i].uVertexIndex[0]];
		B=pSrcPtData[triangle[i].uVertexIndex[1]];
		C=pSrcPtData[triangle[i].uVertexIndex[2]];
		DPT3D pt[3];
		//以AB边为基准//判断插入点和C点是否同侧
		pt[0]=A;pt[1]=B;pt[2]=C;
		int judgeAB=JudgePtOfLineSide(pt,queryPt);
		if(judgeAB<0) continue;
		if(judgeAB==0) {uTriIndex=i;return 0;}
		//以BC边为基准//判断插入点和A点是否同侧
		pt[0]=B;pt[1]=C;pt[2]=A;
		int judgeBC=JudgePtOfLineSide(pt,queryPt);
		if(judgeBC<0) continue;
		if(judgeBC==0) {uTriIndex=i;return 1;}
		//以AC边为基准//判断插入点和B点是否同侧
		pt[0]=A;pt[1]=C;pt[2]=B;
		int judgeAC=JudgePtOfLineSide(pt,queryPt);
		if(judgeAC<0) continue;
		if(judgeAC==0) {uTriIndex=i;return 2;}

		if (judgeAB==1&&judgeAC==1&&judgeBC==1)
		{uTriIndex=i;return -1;}
	}
	return -2;
}
unsigned int GetTriIndexOfComnEdge(const unsigned int firstPtIndex,const unsigned int SecondPtIndex,const unsigned int &uCurrTriIndex,const std::vector<Tri>&triangle)
{
	unsigned int uTriIndex=99999;
	for (unsigned int i=0;i<triangle.size();i++)
	{
		if (i==uCurrTriIndex)continue;
		unsigned int uCurrTriVetexIndex[3];//取出当前三角形的三个顶点
		for (unsigned int j=0;j<3;j++)
			uCurrTriVetexIndex[j]=triangle[i].uVertexIndex[j];
		for (unsigned int j=0;j<3;j++)
		{
			if (firstPtIndex==uCurrTriVetexIndex[j])
			{
				if (j==0)
				{
					if (SecondPtIndex==uCurrTriVetexIndex[j+1])return i;
					else if (SecondPtIndex==uCurrTriVetexIndex[j+2])return i;		
				}
				if (j==1)
				{
					if (SecondPtIndex==uCurrTriVetexIndex[j-1])return i;
					else if (SecondPtIndex==uCurrTriVetexIndex[j+1])return i;		
				}
				if (j==2)
				{
					if (SecondPtIndex==uCurrTriVetexIndex[j-1])return i;
					else if (SecondPtIndex==uCurrTriVetexIndex[j-2])return i;		
				}
			}
		}
	}
	return uTriIndex;
}
/************************************************************************/
/* 对新插入的三角形建立拓扑关系                                                                     */
/************************************************************************/
bool RebirthNewTriTopology(const unsigned int &uCurrTriIndex, Tri newTri[3],std::vector<Tri>&triangle)//新生成的三角形还未插入到网中
{
	/*// * /获取新插入三角形的索引
	unsigned int uNewTriIndex[3];
	for(unsigned int i=0;i<3;i++)
		uNewTriIndex[i]=newTri[i].uTriIndex;
	//获取相邻三角形的索引
	unsigned int uAdjaTriIndex[3]={0};
	for (unsigned int i=0;i<3;i++)
		uAdjaTriIndex[i]=triangle[uCurrTriIndex].uAdjacentTriIndex[i];* /*/
	unsigned int uGetComnEdgeInTriIndex;
	uGetComnEdgeInTriIndex=GetTriIndexOfComnEdge(newTri[0].uVertexIndex[0],newTri[0].uVertexIndex[1],uCurrTriIndex,triangle);
	newTri[0].uAdjacentTriIndex[0]=uGetComnEdgeInTriIndex;
	newTri[0].uAdjacentTriIndex[1]=newTri[1].uTriIndex;
	newTri[0].uAdjacentTriIndex[2]=newTri[2].uTriIndex;
	uGetComnEdgeInTriIndex=GetTriIndexOfComnEdge(newTri[1].uVertexIndex[0],newTri[1].uVertexIndex[1],uCurrTriIndex,triangle);
	newTri[1].uAdjacentTriIndex[0]=uGetComnEdgeInTriIndex;
	newTri[1].uAdjacentTriIndex[1]=newTri[2].uTriIndex;
	newTri[1].uAdjacentTriIndex[2]=newTri[0].uTriIndex;
	uGetComnEdgeInTriIndex=GetTriIndexOfComnEdge(newTri[2].uVertexIndex[0],newTri[2].uVertexIndex[1],uCurrTriIndex,triangle);
	newTri[2].uAdjacentTriIndex[0]=uGetComnEdgeInTriIndex;
	newTri[2].uAdjacentTriIndex[1]=newTri[0].uTriIndex;
	newTri[2].uAdjacentTriIndex[2]=newTri[1].uTriIndex;
	return true;
}
unsigned int GetComnEdgeIndex(const unsigned int firstPtIndex,const unsigned int SecondPtIndex,const Tri &CurrTri)
{
	unsigned int ComnEdgeIndex=0;
	unsigned int fistPtIndexPosition=0,SecondPtIndexPosition=0;
	unsigned int uCurrTriVetexIndex[3];//取出当前三角形三个顶点索引
	for (unsigned int i=0;i<3;i++)
		uCurrTriVetexIndex[i]=CurrTri.uVertexIndex[i];
	for (unsigned int i=0;i<3;i++)
	{
		if (firstPtIndex==uCurrTriVetexIndex[i])
		{
			fistPtIndexPosition=i;
			if (i==0)
			{
				if (SecondPtIndex==uCurrTriVetexIndex[i+1])
				{
					SecondPtIndexPosition=i+1;
					switch(fistPtIndexPosition+SecondPtIndexPosition)
					{
					case 1:ComnEdgeIndex=0;//第一条边(返回索引0)
					case 3:ComnEdgeIndex= 1;//第二条边(返回索引1)
					case 2:ComnEdgeIndex= 2;//第三条边(返回索引2)
					}
					break;
				}
				if (SecondPtIndex==uCurrTriVetexIndex[i+2])
				{
					SecondPtIndexPosition=i+2;
					switch(fistPtIndexPosition+SecondPtIndexPosition)
					{
					case 1:ComnEdgeIndex=0;//第一条边(返回索引0)
					case 3:ComnEdgeIndex= 1;//第二条边(返回索引1)
					case 2:ComnEdgeIndex= 2;//第三条边(返回索引2)
					}
					break;
				}
			}
			if (i==1)
			{
				if (SecondPtIndex==uCurrTriVetexIndex[i+1])
				{
					SecondPtIndexPosition=i+1;
					switch(fistPtIndexPosition+SecondPtIndexPosition)
					{
					case 1:ComnEdgeIndex=0;//第一条边(返回索引0)
					case 3:ComnEdgeIndex= 1;//第二条边(返回索引1)
					case 2:ComnEdgeIndex= 2;//第三条边(返回索引2)
					}
					break;
				}
				if (SecondPtIndex==uCurrTriVetexIndex[i-1])
				{
					SecondPtIndexPosition=i-1;
					switch(fistPtIndexPosition+SecondPtIndexPosition)
					{
					case 1:ComnEdgeIndex=0;//第一条边(返回索引0)
					case 3:ComnEdgeIndex= 1;//第二条边(返回索引1)
					case 2:ComnEdgeIndex= 2;//第三条边(返回索引2)
					}
					break;
				}
			}
			if (i==2)
			{
				if (SecondPtIndex==uCurrTriVetexIndex[i-1])
				{
					SecondPtIndexPosition=i-1;
					switch(fistPtIndexPosition+SecondPtIndexPosition)
					{
					case 1:ComnEdgeIndex=0;//第一条边(返回索引0)
					case 3:ComnEdgeIndex= 1;//第二条边(返回索引1)
					case 2:ComnEdgeIndex= 2;//第三条边(返回索引2)
					}
					break;
				}
				if (SecondPtIndex==uCurrTriVetexIndex[i-2])
				{
					SecondPtIndexPosition=i-2;
					switch(fistPtIndexPosition+SecondPtIndexPosition)
					{
					case 1:ComnEdgeIndex=0;//第一条边(返回索引0)
					case 3:ComnEdgeIndex= 1;//第二条边(返回索引1)
					case 2:ComnEdgeIndex= 2;//第三条边(返回索引2)
					}
					break;
				}
			}
		}
	}
	return ComnEdgeIndex;
}
/************************************************************************/
/* 重新建立三角形拓扑关系                                                                     */
/************************************************************************/
bool RebirthTriTopology(Tri newTri[3],std::vector<Tri>&triangle)//新生成的三角形已插入到网中
{
	 unsigned int uCurrTriIndex=0,uGetComnEdgeInTriIndex=0,uGetComnEdgeIndex=0;
//为newTri[0]建立拓扑关系
	 uCurrTriIndex=newTri[0].uTriIndex;	
	uGetComnEdgeInTriIndex=GetTriIndexOfComnEdge(newTri[0].uVertexIndex[0],newTri[0].uVertexIndex[1],uCurrTriIndex,triangle);
	newTri[0].uAdjacentTriIndex[0]=uGetComnEdgeInTriIndex;
	newTri[0].uAdjacentTriIndex[1]=newTri[1].uTriIndex;
	newTri[0].uAdjacentTriIndex[2]=newTri[2].uTriIndex;
	//更新newTri[0]的邻接三角形拓扑关系
	if(uGetComnEdgeInTriIndex!=99999)
	{
		uGetComnEdgeIndex=GetComnEdgeIndex(newTri[0].uVertexIndex[0],newTri[0].uVertexIndex[1],triangle[uGetComnEdgeInTriIndex]);
		triangle[uGetComnEdgeInTriIndex].uAdjacentTriIndex[uGetComnEdgeIndex]=newTri[0].uTriIndex;
	}
//为newTri[1]建立拓扑关系
	uCurrTriIndex=newTri[1].uTriIndex;	
	uGetComnEdgeInTriIndex=GetTriIndexOfComnEdge(newTri[1].uVertexIndex[0],newTri[1].uVertexIndex[1],uCurrTriIndex,triangle);
	newTri[1].uAdjacentTriIndex[0]=uGetComnEdgeInTriIndex;
	newTri[1].uAdjacentTriIndex[1]=newTri[2].uTriIndex;
	newTri[1].uAdjacentTriIndex[2]=newTri[0].uTriIndex;
	//更新newTri[1]的邻接三角形拓扑关系
	if(uGetComnEdgeInTriIndex!=99999)
	{
		uGetComnEdgeIndex=GetComnEdgeIndex(newTri[1].uVertexIndex[0],newTri[1].uVertexIndex[1],triangle[uGetComnEdgeInTriIndex]);
		triangle[uGetComnEdgeInTriIndex].uAdjacentTriIndex[uGetComnEdgeIndex]=newTri[1].uTriIndex;
	}
//为newTri[2]建立拓扑关系
	uCurrTriIndex=newTri[2].uTriIndex;	
	uGetComnEdgeInTriIndex=GetTriIndexOfComnEdge(newTri[2].uVertexIndex[0],newTri[2].uVertexIndex[1],uCurrTriIndex,triangle);
	newTri[2].uAdjacentTriIndex[0]=uGetComnEdgeInTriIndex;
	newTri[2].uAdjacentTriIndex[1]=newTri[0].uTriIndex;
	newTri[2].uAdjacentTriIndex[2]=newTri[1].uTriIndex;	
	//更新newTri[2]的邻接三角形拓扑关系
	if(uGetComnEdgeInTriIndex!=99999)
	{
		uGetComnEdgeIndex=GetComnEdgeIndex(newTri[2].uVertexIndex[0],newTri[2].uVertexIndex[1],triangle[uGetComnEdgeInTriIndex]);
		triangle[uGetComnEdgeInTriIndex].uAdjacentTriIndex[uGetComnEdgeIndex]=newTri[2].uTriIndex;
	}
	return true;
}
bool RebirthAdjaTriTopology(const unsigned int &uCurrTriIndex,const Tri newTri[3],std::vector<Tri>&triangle)
{
	//寻找相邻三角形
	unsigned int uAdjaTriIndex[3]={0};
	unsigned int uNewTriIndex[3]={0};
	for(unsigned int i=0;i<3;i++)
		uNewTriIndex[i]=newTri[i].uTriIndex;
	for (unsigned int i=0;i<3;i++)//newTri
	{
		for (unsigned int j=0;j<3;j++)//newTri[i].uAdjacentTriIndex
		{
			for (unsigned int k=0;k<3;k++)//uNewTriIndex
			{
				if (newTri[i].uAdjacentTriIndex[j]==uNewTriIndex[k])break;
				if(k==2) uAdjaTriIndex[i]=newTri[i].uAdjacentTriIndex[j];//只有索引都与新加的三角形索引不相同时才会保存
			}		
		}
		unsigned int uGetComnEdgeInTriIndex;
		if (uAdjaTriIndex[i]!=99999)
		{
			uGetComnEdgeInTriIndex=GetComnEdgeIndex(newTri[i].uVertexIndex[0],newTri[i].uVertexIndex[1],triangle[uAdjaTriIndex[i]]);
			triangle[uAdjaTriIndex[i]].uAdjacentTriIndex[uGetComnEdgeInTriIndex]=newTri[i].uTriIndex;
		}
	}
	return true;
}
bool DTLocalOptimalPro1(Tri &CurrTri,Tri&AdjTri,const DPT3D*pSrcPtData,std::vector<Tri>&triangle)//版本1.0
{
	DPT3D CurrPt[3];//获取当前三角形的三个顶点的实际坐标
	CurrPt[0]=pSrcPtData[CurrTri.uVertexIndex[0]];
	CurrPt[1]=pSrcPtData[CurrTri.uVertexIndex[1]];
	CurrPt[2]=pSrcPtData[CurrTri.uVertexIndex[2]];
	Circle CurrCirPara;//计算当前三角形的外接圆参数
	ComputeCirclePara(CurrPt,CurrCirPara);
	unsigned int uAdjThirdVetexIndex=0,uCurrThirdVetexIndex=0;
	std::vector<unsigned int> uCommonVetexIndex;
	for(unsigned int k=0;k<3;k++)//获取邻接三角形的第三个顶点索引
	{
		if (AdjTri.uVertexIndex[k]!=CurrTri.uVertexIndex[0]&&
			AdjTri.uVertexIndex[k]!=CurrTri.uVertexIndex[1]&&
			AdjTri.uVertexIndex[k]!=CurrTri.uVertexIndex[2])
			uAdjThirdVetexIndex=AdjTri.uVertexIndex[k];
		else
		{
			uCommonVetexIndex.push_back(AdjTri.uVertexIndex[k]);//获取公共顶点
		}
	}
	if (uCommonVetexIndex.size()!=2) return false;
	for(unsigned int k=0;k<3;k++)//获取当前三角形的第三个顶点索引
	{
		if (CurrTri.uVertexIndex[k]!=uCommonVetexIndex[0]&&
			CurrTri.uVertexIndex[k]!=uCommonVetexIndex[1])
			uCurrThirdVetexIndex=CurrTri.uVertexIndex[k];
	}
	double lfDistance=ComputeDistanceOfPts(CurrCirPara.CircleCenter,pSrcPtData[uAdjThirdVetexIndex]);
	if (lfDistance<CurrCirPara.CircleRadius)//交换对角线
	{
		Tri lopTri[2];//存储新形成的三角形
//为新形成的三角形的顶点赋值
		lopTri[0].uVertexIndex[0]=uCurrThirdVetexIndex;
		lopTri[0].uVertexIndex[1]=uAdjThirdVetexIndex;
		lopTri[0].uVertexIndex[2]=uCommonVetexIndex[1];
		lopTri[1].uVertexIndex[0]=uAdjThirdVetexIndex;
		lopTri[1].uVertexIndex[1]=uCurrThirdVetexIndex;
		lopTri[1].uVertexIndex[2]=uCommonVetexIndex[0];
//为新形成的三角形编号
		lopTri[0].uTriIndex=CurrTri.uTriIndex;
		lopTri[1].uTriIndex=AdjTri.uTriIndex;
//更新三角网中的元素
		DelVectorElement(triangle,lopTri[0].uTriIndex);
		InsertVectorElement(triangle,lopTri[0].uTriIndex,lopTri[0]);
		DelVectorElement(triangle,lopTri[1].uTriIndex);
		InsertVectorElement(triangle,lopTri[1].uTriIndex,lopTri[1]);
//为新形成的三角形建立拓扑关系
		//lopTri[0]
		unsigned int uGetTriIndexOfComnEdge=0,uGetComnEdgeIndex=0;
		lopTri[0].uAdjacentTriIndex[0]=lopTri[1].uTriIndex;
		/*uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[0].uVertexIndex[0],lopTri[0].uVertexIndex[2],lopTri[1].uTriIndex,triangle);
		lopTri[0].uAdjacentTriIndex[0]=uGetTriIndexOfComnEdge;
		if(uGetTriIndexOfComnEdge!=99999)
		{
			uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[0].uVertexIndex[0],lopTri[0].uVertexIndex[1],triangle[uGetTriIndexOfComnEdge]);
			triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[0].uTriIndex;
		}*/
//////////////////////////////////////////////////////////////////////////
		uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[0].uVertexIndex[1],lopTri[0].uVertexIndex[2],lopTri[0].uTriIndex,triangle);
		lopTri[0].uAdjacentTriIndex[1]=uGetTriIndexOfComnEdge;
		if(uGetTriIndexOfComnEdge!=99999)
		{
			uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[0].uVertexIndex[1],lopTri[0].uVertexIndex[2],triangle[uGetTriIndexOfComnEdge]);
			triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[0].uTriIndex;
		}
		uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[0].uVertexIndex[2],lopTri[0].uVertexIndex[0],lopTri[0].uTriIndex,triangle);
		lopTri[0].uAdjacentTriIndex[2]=uGetTriIndexOfComnEdge;
		if(uGetTriIndexOfComnEdge!=99999)
		{
			uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[0].uVertexIndex[2],lopTri[0].uVertexIndex[0],triangle[uGetTriIndexOfComnEdge]);
			triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[0].uTriIndex;
		}
		//lopTri[1]
		lopTri[1].uAdjacentTriIndex[0]=lopTri[0].uTriIndex;
		/*uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[1].uVertexIndex[0],lopTri[1].uVertexIndex[1],lopTri[1].uTriIndex,triangle);
		lopTri[1].uAdjacentTriIndex[0]=uGetTriIndexOfComnEdge;
		if(uGetTriIndexOfComnEdge!=99999)
		{
			uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[1].uVertexIndex[0],lopTri[1].uVertexIndex[1],triangle[uGetTriIndexOfComnEdge]);
			triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[1].uTriIndex;
		}*/
//////////////////////////////////////////////////////////////////////////
		uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[1].uVertexIndex[1],lopTri[1].uVertexIndex[2],lopTri[1].uTriIndex,triangle);
		lopTri[1].uAdjacentTriIndex[1]=uGetTriIndexOfComnEdge;
		if(uGetTriIndexOfComnEdge!=99999)
		{
			uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[1].uVertexIndex[1],lopTri[1].uVertexIndex[2],triangle[uGetTriIndexOfComnEdge]);
			triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[1].uTriIndex;
		}
		uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[1].uVertexIndex[2],lopTri[1].uVertexIndex[0],lopTri[1].uTriIndex,triangle);
		lopTri[1].uAdjacentTriIndex[2]=uGetTriIndexOfComnEdge;
		if(uGetTriIndexOfComnEdge!=99999)
		{
			uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[1].uVertexIndex[2],lopTri[1].uVertexIndex[0],triangle[uGetTriIndexOfComnEdge]);
			triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[1].uTriIndex;
		}
		//由于更新了邻接三角形的拓扑关系，因此再次更新三角网中的元素
		DelVectorElement(triangle,lopTri[0].uTriIndex);
		InsertVectorElement(triangle,lopTri[0].uTriIndex,lopTri[0]);
		DelVectorElement(triangle,lopTri[1].uTriIndex);
		InsertVectorElement(triangle,lopTri[1].uTriIndex,lopTri[1]);
		return true;
	}
	return false;
}
bool DTLocalOptimalPro(Tri &CurrTri,const DPT3D*pSrcPtData,std::vector<Tri>&triangle)//版本2.0
{
	if (CurrTri.uAdjacentTriIndex[0]==99999) return false;
	DPT3D CurrPt[3];//获取当前三角形的三个顶点的实际坐标
	CurrPt[0]=pSrcPtData[CurrTri.uVertexIndex[0]];
	CurrPt[1]=pSrcPtData[CurrTri.uVertexIndex[1]];
	CurrPt[2]=pSrcPtData[CurrTri.uVertexIndex[2]];
	Circle CurrCirPara;//计算当前三角形的外接圆参数
	ComputeCirclePara(CurrPt,CurrCirPara);
	unsigned int uCommonVetexIndex[2];//获取公共顶点
	uCommonVetexIndex[0]=CurrTri.uVertexIndex[0];uCommonVetexIndex[1]=CurrTri.uVertexIndex[1];
	unsigned int uAdjThirdVetexIndex=0,uCurrThirdVetexIndex=CurrTri.uVertexIndex[2];//获取当前三角形的第三个顶点索引
	Tri AdjTri=triangle[CurrTri.uAdjacentTriIndex[0]];
	for(unsigned int k=0;k<3;k++)//获取邻接三角形的第三个顶点索引
	{
		if (AdjTri.uVertexIndex[k]!=uCommonVetexIndex[0]&&
			AdjTri.uVertexIndex[k]!=uCommonVetexIndex[1])
			uAdjThirdVetexIndex=AdjTri.uVertexIndex[k];
	}
	double lfDistance=ComputeDistanceOfPts(CurrCirPara.CircleCenter,pSrcPtData[uAdjThirdVetexIndex]);
	if (lfDistance<CurrCirPara.CircleRadius)//交换对角线
	{
		Tri lopTri[2];//存储新形成的三角形
//为新形成的三角形的顶点赋值
		lopTri[0].uVertexIndex[0]=uCurrThirdVetexIndex;
		lopTri[0].uVertexIndex[1]=uAdjThirdVetexIndex;
		lopTri[0].uVertexIndex[2]=uCommonVetexIndex[1];
		lopTri[1].uVertexIndex[0]=uAdjThirdVetexIndex;
		lopTri[1].uVertexIndex[1]=uCurrThirdVetexIndex;
		lopTri[1].uVertexIndex[2]=uCommonVetexIndex[0];
//为新形成的三角形编号
		lopTri[0].uTriIndex=CurrTri.uTriIndex;
		lopTri[1].uTriIndex=AdjTri.uTriIndex;
//更新三角网中的元素
		DelVectorElement(triangle,lopTri[0].uTriIndex);
		InsertVectorElement(triangle,lopTri[0].uTriIndex,lopTri[0]);
		DelVectorElement(triangle,lopTri[1].uTriIndex);
		InsertVectorElement(triangle,lopTri[1].uTriIndex,lopTri[1]);
//为新形成的三角形建立拓扑关系
		//lopTri[0]
		unsigned int uGetTriIndexOfComnEdge=0,uGetComnEdgeIndex=0;
		lopTri[0].uAdjacentTriIndex[0]=lopTri[1].uTriIndex;
		uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[0].uVertexIndex[1],lopTri[0].uVertexIndex[2],lopTri[0].uTriIndex,triangle);
		lopTri[0].uAdjacentTriIndex[1]=uGetTriIndexOfComnEdge;
		if(uGetTriIndexOfComnEdge!=99999)
		{
			uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[0].uVertexIndex[1],lopTri[0].uVertexIndex[2],triangle[uGetTriIndexOfComnEdge]);
			triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[0].uTriIndex;
		}
		uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[0].uVertexIndex[2],lopTri[0].uVertexIndex[0],lopTri[0].uTriIndex,triangle);
		lopTri[0].uAdjacentTriIndex[2]=uGetTriIndexOfComnEdge;
		if(uGetTriIndexOfComnEdge!=99999)
		{
			uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[0].uVertexIndex[2],lopTri[0].uVertexIndex[0],triangle[uGetTriIndexOfComnEdge]);
			triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[0].uTriIndex;
		}
		//lopTri[1]
		lopTri[1].uAdjacentTriIndex[0]=lopTri[0].uTriIndex;
		uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[1].uVertexIndex[1],lopTri[1].uVertexIndex[2],lopTri[1].uTriIndex,triangle);
		lopTri[1].uAdjacentTriIndex[1]=uGetTriIndexOfComnEdge;
		if(uGetTriIndexOfComnEdge!=99999)
		{
			uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[1].uVertexIndex[1],lopTri[1].uVertexIndex[2],triangle[uGetTriIndexOfComnEdge]);
			triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[1].uTriIndex;
		}
		uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[1].uVertexIndex[2],lopTri[1].uVertexIndex[0],lopTri[1].uTriIndex,triangle);
		lopTri[1].uAdjacentTriIndex[2]=uGetTriIndexOfComnEdge;
		if(uGetTriIndexOfComnEdge!=99999)
		{
			uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[1].uVertexIndex[2],lopTri[1].uVertexIndex[0],triangle[uGetTriIndexOfComnEdge]);
			triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[1].uTriIndex;
		}
		//由于更新了邻接三角形的拓扑关系，因此再次更新三角网中的元素
		DelVectorElement(triangle,lopTri[0].uTriIndex);
		InsertVectorElement(triangle,lopTri[0].uTriIndex,lopTri[0]);
		DelVectorElement(triangle,lopTri[1].uTriIndex);
		InsertVectorElement(triangle,lopTri[1].uTriIndex,lopTri[1]);
		return true;
	}
	return false;
}
/*
class Tin
{
public:
	unsigned int uPointsNum;
	DPT3D*pDPT3D;
	Tin()
	{

	};
	 ~Tin();
protected:
private:
};*/