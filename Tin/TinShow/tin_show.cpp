#include"opengl\glut.h"
#include <stdio.h>
#include<time.h>
#include <string.h>
#include "Tin.h"
#include <vector>
char strPointsFile[512];
double Xmin,Xmax,Ymin,Ymax,Zmin,Zmax;
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);//把整个窗口清除为当前清除颜色
	clock_t start_time,end_time;
	start_time=clock();	
	FILE*fp=fopen(strPointsFile,"r");
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
	char strTriFile[512];strcpy(strTriFile,strPointsFile);
	char *pS=strrchr(strTriFile,'.');
	sprintf(pS,"%s","_tin.txt");
	fp=fopen(strTriFile,"r");
	if (NULL==fp) {printf("\tOpen file failed: %s.\n",strTriFile);exit(-1);}
	unsigned int uTriNum=0;
	fscanf(fp,"%d\n",&uTriNum);
	Tri triTemp;
	std::vector<Tri> triangle;
	for (unsigned int i=0;i<uTriNum;i++)
	{
		fscanf(fp,"%d,%d,%d\n",&triTemp.uVertexIndex[0],&triTemp.uVertexIndex[1],&triTemp.uVertexIndex[2]);
		triangle.push_back(triTemp);
	}
	fclose(fp);
	glTranslated(Xmin,Ymin,0.0);
	glColor3f(1.0,0.0,0.0);
	{
		for (unsigned int i=0;i<triangle.size();i++)
		{
			if (triangle.at(i).uVertexIndex[0]>uPointNum||triangle.at(i).uVertexIndex[1]>uPointNum||triangle.at(i).uVertexIndex[2]>uPointNum)
				continue;
			glBegin(GL_LINE_LOOP);
			glVertex2d(pDPT3D[triangle.at(i).uVertexIndex[0]].x,pDPT3D[triangle.at(i).uVertexIndex[0]].y);
			glVertex2d(pDPT3D[triangle.at(i).uVertexIndex[1]].x,pDPT3D[triangle.at(i).uVertexIndex[1]].y);
			glVertex2d(pDPT3D[triangle.at(i).uVertexIndex[2]].x,pDPT3D[triangle.at(i).uVertexIndex[2]].y);
			glEnd();
		}
	}
	glFlush();
	end_time=clock();
	printf("\tRun time: %fs\n",(float)(end_time-start_time)/1000.0);	
}
void reshape(int width, int height)
{
	glScaled(0.75,0.75,1.0);
	glTranslated(20,20,0.0);
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,(GLdouble)width,0,(GLdouble)height);
}
unsigned int uTriCount=0,uCurrTriIndex=0;
int main(int argc, char* argv[])
{
	if (argc!=2)
	{
		printf("Usage:\n\tTin_Show.exe PointsFile[*.txt]\n");return 0;
	}
	printf("Running...\n");
	printf("\tPoints file loading...\n");
	char strMValueFile[512];
	strcpy(strPointsFile,argv[1]);strcpy(strMValueFile,argv[1]);
	char*pS=strrchr(strMValueFile,'.');
	sprintf(pS,"%s","_MValue.txt");
	//////////////////////////read data///////////////////////////////////////
	FILE *fp=fopen(strMValueFile,"r");
	if (NULL==fp) {printf("\tOpen file of %s failed.\n",strMValueFile);return 0;}
	fscanf(fp,"%lf,%lf",&Xmin,&Xmax);
	fscanf(fp,"%lf,%lf",&Ymin,&Ymax);
	fscanf(fp,"%lf,%lf",&Zmin,&Zmax);
	fclose(fp);

	printf("\n\tDisplay...\n");
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glClearColor(0.0,0.0,0.0,0.0);//set background color:black
	glutInitWindowSize((int)(Xmax-Xmin),(int)(Ymax-Ymin));
	glutInitWindowPosition(0,0);
	glutCreateWindow("OpenGL--Tin");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}

