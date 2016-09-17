#include <windows.h>
#include <stdio.h>
#include<time.h>
#include <string.h>
#include "Tin.h"
#include <vector>
int main(int argc, char* argv[])
{
	if (argc!=2)
	{
		printf("Usage:\n\tTin.exe SrcPointsFile[*.txt]\n");return 0;
	}
	printf("Running...\n");
	printf("\tPoints file loading...\n");
	char strPointsFile[512],strMValueFile[512];
	strcpy(strPointsFile,argv[1]);strcpy(strMValueFile,argv[1]);
	char*pS=strrchr(strMValueFile,'.');
	sprintf(pS,"%s","_MValue.txt");
	//////////////////////////read data///////////////////////////////////////
	double Xmin,Xmax,Ymin,Ymax,Zmin,Zmax;
	FILE *fp=fopen(strMValueFile,"r");
	if (NULL==fp) {printf("\tOpen file of %s failed.\n",strMValueFile);return 0;}
	fscanf(fp,"%lf,%lf",&Xmin,&Xmax);
	fscanf(fp,"%lf,%lf",&Ymin,&Ymax);
	fscanf(fp,"%lf,%lf",&Zmin,&Zmax);
	fclose(fp);
	fp=fopen(strPointsFile,"r");
	if (NULL==fp) {printf("\tOpen file failed: %s.\n",strPointsFile);exit(-1);}
	unsigned int uPointNum=0;fscanf(fp,"%d\n",&uPointNum);
	DPT3D *pDPT3D=new DPT3D[uPointNum+4];
	(pDPT3D+0)->x=Xmin;(pDPT3D+0)->y=Ymin;
	(pDPT3D+1)->x=Xmin;(pDPT3D+1)->y=Ymax;
	(pDPT3D+2)->x=Xmax;(pDPT3D+2)->y=Ymax;
	(pDPT3D+3)->x=Xmax;(pDPT3D+3)->y=Ymin;
	for (unsigned int i=4;i<uPointNum+4;i++)
	{
		fscanf(fp,"%lf,%lf\n",&(pDPT3D+i)->x,&(pDPT3D+i)->y,&(pDPT3D+i)->z);
	}
	fclose(fp);
	printf("\tPoints file loaded success: %s\n",strPointsFile);

	printf("\n\t正在构Tin...\n");
	/************************************************************************/
	/*  对所有数据点进行循环，构造Tin                                                                    */
	/************************************************************************/
	static unsigned int uTriCount=0,uCurrTriIndex=0;
	Tri triTemp;
	std::vector<Tri>triangle;
//初始三角形1
	triTemp.uTriIndex=uTriCount++;
	for (unsigned int i=0;i<3;i++)
		triTemp.uVertexIndex[i]=i;
	triTemp.uAdjacentTriIndex[2]=1;
	triangle.push_back(triTemp);
//初始三角形2
	triTemp.uTriIndex=uTriCount++;
	triTemp.uVertexIndex[0]=2;triTemp.uVertexIndex[1]=3;triTemp.uVertexIndex[2]=0;
	triTemp.uAdjacentTriIndex[2]=0;
	triangle.push_back(triTemp);
//插入点
	DPT3D InsertPoint;
	for (unsigned int i=4;i<1000;i++)//uPointNum
	{
		InsertPoint=pDPT3D[i];
		unsigned int uQueryTriIndex=99999;
		int iJudgePtInWhTri=JudgePtInWhTri(InsertPoint,uQueryTriIndex,pDPT3D,triangle);
		if (iJudgePtInWhTri==-2)continue;//插入点不在三角形内
		if (iJudgePtInWhTri==-1)//插入点在三角形内
		{
			Tri tri[3];
			//剖分后3个三角形顶点赋值
			tri[0].uVertexIndex[0]=triangle[uQueryTriIndex].uVertexIndex[0];
			tri[0].uVertexIndex[1]=triangle[uQueryTriIndex].uVertexIndex[1];
			tri[0].uVertexIndex[2]=i;//第一个三角形
			tri[1].uVertexIndex[0]=triangle[uQueryTriIndex].uVertexIndex[1];
			tri[1].uVertexIndex[1]=triangle[uQueryTriIndex].uVertexIndex[2];
			tri[1].uVertexIndex[2]=i;//第二个三角形
			tri[2].uVertexIndex[0]=triangle[uQueryTriIndex].uVertexIndex[2];
			tri[2].uVertexIndex[1]=triangle[uQueryTriIndex].uVertexIndex[0];
			tri[2].uVertexIndex[2]=i;//第三个三角形
			//剖分后3个三角形编号
			tri[0].uTriIndex=uTriCount++;
			tri[1].uTriIndex=uTriCount++;
			tri[2].uTriIndex=uQueryTriIndex;
			//剖分后3个三角形拓扑关系的建立
			RebirthNewTriTopology(uQueryTriIndex,tri,triangle);
			//三角形的ID号和在三角形数组中的存储位置一致
			DelVectorElement(triangle,uQueryTriIndex);
			InsertVectorElement(triangle,uQueryTriIndex,tri[2]);
			triangle.push_back(tri[0]);
			triangle.push_back(tri[1]);
			//重新建立三角形拓扑关系
			//RebirthTriTopology(tri,triangle);
			//剖分后相邻三角形拓扑的建立
			RebirthAdjaTriTopology(uQueryTriIndex,tri,triangle);
			uCurrTriIndex=triangle.size();
			//局部优化过程
			Tri newTri;
			for (unsigned int j=0;j<3;j++)
			{
				if (j<2) newTri=triangle[uCurrTriIndex-2+j];
				else newTri=triangle[uQueryTriIndex];
				if(newTri.uAdjacentTriIndex[0]!=99999)
				{
					DTLocalOptimalPro(newTri,pDPT3D,triangle); 					
				}		
			}
		}
		if (iJudgePtInWhTri!=-2&&iJudgePtInWhTri!=-1)//插入点在三角形的第一条边上
		{
			unsigned int uCommonVetexIndex[2]={0};
			unsigned int uCurrThirdVetexIndex=0;
			if(iJudgePtInWhTri==0)
			{
				uCommonVetexIndex[0]=triangle[uQueryTriIndex].uVertexIndex[0];
				uCommonVetexIndex[1]=triangle[uQueryTriIndex].uVertexIndex[1];
				uCurrThirdVetexIndex=triangle[uQueryTriIndex].uVertexIndex[2];
			}
			if(iJudgePtInWhTri==1)
			{
				uCommonVetexIndex[0]=triangle[uQueryTriIndex].uVertexIndex[1];
				uCommonVetexIndex[1]=triangle[uQueryTriIndex].uVertexIndex[2];
				uCurrThirdVetexIndex=triangle[uQueryTriIndex].uVertexIndex[0];
			}
			if(iJudgePtInWhTri==2)
			{
				uCommonVetexIndex[0]=triangle[uQueryTriIndex].uVertexIndex[2];
				uCommonVetexIndex[1]=triangle[uQueryTriIndex].uVertexIndex[0];
				uCurrThirdVetexIndex=triangle[uQueryTriIndex].uVertexIndex[1];
			}
			unsigned int uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(uCommonVetexIndex[0],uCommonVetexIndex[1],uQueryTriIndex,triangle);			
			if (uGetTriIndexOfComnEdge!=99999)
			{
				unsigned int uAdjThirdVetexIndex=0;
				Tri AdjTri=triangle[uGetTriIndexOfComnEdge];
				for(unsigned int k=0;k<3;k++)//获取邻接三角形的第三个顶点索引
				{
					if (AdjTri.uVertexIndex[k]!=uCommonVetexIndex[0]&&
						AdjTri.uVertexIndex[k]!=uCommonVetexIndex[1])
						uAdjThirdVetexIndex=AdjTri.uVertexIndex[k];
				}
				Tri lopTri[4];//存储新形成的三角形
				lopTri[0].uVertexIndex[0]=uCommonVetexIndex[0];
				lopTri[0].uVertexIndex[1]=uCurrThirdVetexIndex;
				lopTri[0].uVertexIndex[2]=i;
				lopTri[1].uVertexIndex[0]=uCommonVetexIndex[1];
				lopTri[1].uVertexIndex[1]=uCurrThirdVetexIndex;
				lopTri[1].uVertexIndex[2]=i;
				lopTri[2].uVertexIndex[0]=uCommonVetexIndex[0];
				lopTri[2].uVertexIndex[1]=uAdjThirdVetexIndex;
				lopTri[2].uVertexIndex[2]=i;
				lopTri[3].uVertexIndex[0]=uCommonVetexIndex[1];
				lopTri[3].uVertexIndex[1]=uAdjThirdVetexIndex;
				lopTri[3].uVertexIndex[2]=i;
				//为新形成的三角形编号
				lopTri[0].uTriIndex=uTriCount++;
				lopTri[1].uTriIndex=triangle[uQueryTriIndex].uTriIndex;
				lopTri[2].uTriIndex=uTriCount++;
				lopTri[3].uTriIndex=uGetTriIndexOfComnEdge;
				DelVectorElement(triangle,uQueryTriIndex);
				InsertVectorElement(triangle,uQueryTriIndex,lopTri[1]);
				DelVectorElement(triangle,uGetTriIndexOfComnEdge);
				InsertVectorElement(triangle,uGetTriIndexOfComnEdge,lopTri[3]);
				triangle.push_back(lopTri[0]);
				triangle.push_back(lopTri[2]);
				//为新形成的三角形建立拓扑关系
				//lopTri[0]
				unsigned int uGetTriIndexOfComnEdge=0,uGetComnEdgeIndex=0;
				uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[0].uVertexIndex[0],lopTri[0].uVertexIndex[1],lopTri[0].uTriIndex,triangle);
				lopTri[0].uAdjacentTriIndex[0]=uGetTriIndexOfComnEdge;
				if (uGetTriIndexOfComnEdge!=99999)
				{
					uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[0].uVertexIndex[0],lopTri[0].uVertexIndex[1],triangle[uGetTriIndexOfComnEdge]);
					triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[0].uTriIndex;
				}
				lopTri[0].uAdjacentTriIndex[1]=lopTri[1].uTriIndex;
				uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[0].uVertexIndex[2],lopTri[0].uVertexIndex[0],lopTri[0].uTriIndex,triangle);
				lopTri[0].uAdjacentTriIndex[2]=uGetTriIndexOfComnEdge;
				//lopTri[1]
				uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[1].uVertexIndex[0],lopTri[1].uVertexIndex[1],lopTri[1].uTriIndex,triangle);
				lopTri[1].uAdjacentTriIndex[0]=uGetTriIndexOfComnEdge;
				if (uGetTriIndexOfComnEdge!=99999)
				{
					uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[1].uVertexIndex[0],lopTri[1].uVertexIndex[1],triangle[uGetTriIndexOfComnEdge]);
					triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[1].uTriIndex;
				}
				lopTri[1].uAdjacentTriIndex[1]=lopTri[0].uTriIndex;
				uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[1].uVertexIndex[2],lopTri[1].uVertexIndex[0],lopTri[1].uTriIndex,triangle);
				lopTri[1].uAdjacentTriIndex[2]=uGetTriIndexOfComnEdge;
				//lopTri[2]
				uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[2].uVertexIndex[0],lopTri[2].uVertexIndex[1],lopTri[2].uTriIndex,triangle);
				lopTri[2].uAdjacentTriIndex[0]=uGetTriIndexOfComnEdge;
				if (uGetTriIndexOfComnEdge!=99999)
				{
					uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[2].uVertexIndex[0],lopTri[2].uVertexIndex[1],triangle[uGetTriIndexOfComnEdge]);
					triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[2].uTriIndex;
				}
				lopTri[2].uAdjacentTriIndex[1]=lopTri[3].uTriIndex;
				uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[2].uVertexIndex[2],lopTri[2].uVertexIndex[0],lopTri[2].uTriIndex,triangle);
				lopTri[2].uAdjacentTriIndex[2]=uGetTriIndexOfComnEdge;
				//lopTri[3]
				uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[3].uVertexIndex[0],lopTri[3].uVertexIndex[1],lopTri[3].uTriIndex,triangle);
				lopTri[3].uAdjacentTriIndex[0]=uGetTriIndexOfComnEdge;
				if (uGetTriIndexOfComnEdge!=99999)
				{
					uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[3].uVertexIndex[0],lopTri[3].uVertexIndex[1],triangle[uGetTriIndexOfComnEdge]);
					triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[3].uTriIndex;
				}
				lopTri[3].uAdjacentTriIndex[1]=lopTri[2].uTriIndex;
				uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[3].uVertexIndex[2],lopTri[3].uVertexIndex[0],lopTri[3].uTriIndex,triangle);
				lopTri[3].uAdjacentTriIndex[2]=uGetTriIndexOfComnEdge;
				//局部优化过程
				for (unsigned int j=0;j<4;j++)
				{
					if(lopTri[j].uAdjacentTriIndex[0]!=99999)
					{
						DTLocalOptimalPro(lopTri[j],pDPT3D,triangle); 					
					}		
				}
			}
			else
			{
				Tri lopTri[2];//存储新形成的三角形
				lopTri[0].uVertexIndex[0]=uCommonVetexIndex[0];
				lopTri[0].uVertexIndex[1]=uCurrThirdVetexIndex;
				lopTri[0].uVertexIndex[2]=i;
				lopTri[1].uVertexIndex[0]=uCommonVetexIndex[1];
				lopTri[1].uVertexIndex[1]=uCurrThirdVetexIndex;
				lopTri[1].uVertexIndex[2]=i;
				//为新形成的三角形编号
				lopTri[0].uTriIndex=uTriCount++;
				lopTri[1].uTriIndex=triangle[uQueryTriIndex].uTriIndex;
				DelVectorElement(triangle,uQueryTriIndex);
				InsertVectorElement(triangle,uQueryTriIndex,lopTri[1]);
				triangle.push_back(lopTri[0]);
//为新形成的三角形建立拓扑关系
				//lopTri[0]
				unsigned int uGetTriIndexOfComnEdge=0,uGetComnEdgeIndex=0;
				uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[0].uVertexIndex[0],lopTri[0].uVertexIndex[1],lopTri[0].uTriIndex,triangle);
				lopTri[0].uAdjacentTriIndex[0]=uGetTriIndexOfComnEdge;
				if (uGetTriIndexOfComnEdge!=99999)
				{
					uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[0].uVertexIndex[0],lopTri[0].uVertexIndex[1],triangle[uGetTriIndexOfComnEdge]);
					triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[0].uTriIndex;
				}
				lopTri[0].uAdjacentTriIndex[1]=lopTri[1].uTriIndex;
				lopTri[0].uAdjacentTriIndex[2]=99999;
				//lopTri[1]
				uGetTriIndexOfComnEdge=GetTriIndexOfComnEdge(lopTri[1].uVertexIndex[0],lopTri[1].uVertexIndex[1],lopTri[1].uTriIndex,triangle);
				lopTri[1].uAdjacentTriIndex[0]=uGetTriIndexOfComnEdge;
				if (uGetTriIndexOfComnEdge!=99999)
				{
					uGetComnEdgeIndex=GetComnEdgeIndex(lopTri[1].uVertexIndex[0],lopTri[1].uVertexIndex[1],triangle[uGetTriIndexOfComnEdge]);
					triangle[uGetTriIndexOfComnEdge].uAdjacentTriIndex[uGetComnEdgeIndex]=lopTri[1].uTriIndex;
				}
				lopTri[1].uAdjacentTriIndex[1]=lopTri[0].uTriIndex;
				lopTri[1].uAdjacentTriIndex[2]=99999;
				DelVectorElement(triangle,lopTri[0].uTriIndex);
				InsertVectorElement(triangle,lopTri[0].uTriIndex,lopTri[0]);
				DelVectorElement(triangle,lopTri[1].uTriIndex);
				InsertVectorElement(triangle,lopTri[1].uTriIndex,lopTri[1]);
				//局部优化过程
				uCurrTriIndex=triangle.size();
				Tri newTri;
				for (unsigned int j=0;j<2;j++)
				{
					if (j<1) newTri=triangle[uCurrTriIndex-1+j];
					else newTri=triangle[uQueryTriIndex];
					if(newTri.uAdjacentTriIndex[0]!=99999)
					{
						DTLocalOptimalPro(newTri,pDPT3D,triangle); 					
					}		
				}
			}
		}
		printf("\r\t构Tin完成: %d%%",(int)(i*100/uPointNum));		
	}
	printf("\r\t构Tin完成: 100%%\n");
	char strTinFile[512];strcpy(strTinFile,strPointsFile);
	pS=strrchr(strTinFile,'.');
	sprintf(pS,"%s","_tin.txt");
	fp=fopen(strTinFile,"w");
	if (NULL==fp) {printf("\tOpen file failed: %s.\n",strTinFile);exit(-1);}
	unsigned int uTriNum=(unsigned int)triangle.size();
	fprintf(fp,"%d\n",uTriNum);
	for (unsigned int i=0;i<uTriNum;i++)
	{
		fprintf(fp,"%d,%d,%d\n",triangle[i].uVertexIndex[0],triangle[i].uVertexIndex[1],triangle[i].uVertexIndex[2]);
	}
	fclose(fp);
	printf("\r\t构Tin成功: %s\n",strTinFile);
	delete []pDPT3D;
	return 0;
}

