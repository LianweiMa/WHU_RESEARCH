// BasicStructer.h: interface for the CBasicStructer class.
//
//////////////////////////////////////////////////////////////////////
// modified by KY.F - 2013.7.3
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASICSTRUCTER_H__1AC46295_5380_4F70_8E31_233FEA895573__INCLUDED_)
#define AFX_BASICSTRUCTER_H__1AC46295_5380_4F70_8E31_233FEA895573__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef PI
#define PI 3.141592653589793f
#endif

#ifndef EI
#define EI 2.718281828459045f
#endif

//------------------------------方形------------------------------
struct RECTBOX
{
	double l,r,t,b;
	RECTBOX()
	{
		l=r=t=b=0.0;
	}
	RECTBOX(double L,double R,double T,double B)
	{
		l=L;
		r=R;
		t=T;
		b=B;
	}
	double Width()
	{
		return r-l;
	}
	double Height()
	{
		return t-b;
	}
};

//------------------------------影像------------------------------
struct PHOTOINFO
{
	char path[512]; //影像路径
	char name[512];	//影像名
	int row;		//影像行号
	int col;		//影像列号
	int rotate;		//旋转标记，0：不旋；1：拟时针90度；2：拟时针180度；3：拟时针270度；
	BOOL sel;		//是否选中(已经完成快速镶嵌)

	RECTBOX rectBox;

	PHOTOINFO()
	{
		memset(&path,0,512*sizeof(char));
		memset(&name,0,512*sizeof(char));
		row=col=-1;
		rotate=0;
		sel=false;
	}
	
	PHOTOINFO(char Path[512],char Name[512],int Row,int Col,int Rotate=0,BOOL Sel=false)
	{
		strcpy(path,Path);
		strcpy(name,Name);
		row=Row;
		col=Col;
		rotate=Rotate;
		sel=Sel;
	}
};

//----------------------------相机参数----------------------------
struct CAMPARA
{
	double f,u0,v0;			//焦距，主点		Necessary!

	double w,h;				//像幅宽，高		Not necessary!
	double pxy;				//像元的横纵比		Not necessary!
	double psize;			//像元大小			Not necessary!

	int unit,origin;		//坐标系参数		unit 单位: 0,象素; 1,毫米		origin 原点:0,左下角; 1,中心点

	int distortionnum;		//畸变模型			3,3参数畸变模型;	4,4参数畸变模型 
	double a0,a1,r;			//3参数畸变差		Not necessary!
	double k1,k2,p1,p2;		//4参数畸变差		Not necessary!

	CAMPARA()
	{
		f=u0=v0=0.0;
		w=h=0;
		k1=k2=p1=p2=0.0;
		pxy=1.0;
		psize=1.0;
		unit=1;
		origin=1;
		a0=a1=r=0.0;
		distortionnum=0;
	}

	CAMPARA(double F,double U0,double V0, double W=0.0,double H=0.0, double K1=0.0,double K2=0.0,double P1=0.0,double P2=0.0,
			double PXY=1.0,double PS=1.0,int Unit=1,int Origin=1,int Distnum=0,double A0=0.0, double A1=0.0,double R=0.0)
	{
		f=F;
		u0=U0;
		v0=V0;
		w=W;
		h=H;
		k1=K1;
		k2=K2;
		p1=P1;
		p2=P2;
		pxy=PXY;
		psize=PS;
		unit=Unit;
		origin=Origin;
		distortionnum=Distnum;
		a0=A0;
		a1=A1;
		r=R;
	}
};

//-------------------------影像外方位元素-------------------------
struct OUTPARA		
{
	double Xs,Ys,Zs;		//摄站坐标
	double fai,wmg,kav;		//转角参数 step1.绕Y轴转fai, step2.绕X轴转wmg, step3.绕Z轴转kav

	OUTPARA()
	{
		Xs=Ys=Zs=0.0;
		fai=wmg=kav=0.0;
	}
	OUTPARA(double XS,double YS,double ZS,double FAI,double WMG,double KAV)
	{
		Xs=XS;
		Ys=YS;
		Zs=ZS;
		fai=FAI;
		wmg=WMG;
		kav=KAV;
	}
};

//----------------------------仿射变换----------------------------
struct INORPARA
{
    double a0,a1,a2; // 仿射变换系数(X坐标)
    double b0,b1,b2; // 仿射变换系数(Y坐标)
	double tt;       // 逆变换因子

	INORPARA()
	{
		a0=a1=a2=0.0;
		b0=b1=b2=0.0;
		tt=0.0;
	}
};

//------------------------------像点------------------------------
struct IMGPOINT	
{
	long id;				//影像点的点号
	int imgid;				//影像点位于的影像号
	double x,y;				//横纵坐标

	IMGPOINT()
	{
		id=-1;
		imgid=-1;
		x=y=0.0;
	}
	IMGPOINT(long Id,int Imgid,double X,double Y)
	{
		id=Id;
		imgid=Imgid;
		x=X;
		y=Y;
	}
};

//---------------------------空间控制点---------------------------
struct SCONTROLPOINT		
{
	char name[10];			//控制点名
	int attrbute;			//0 一般点；1 平高控制点；2 检查点；3 平面控制点；4 高程控制点
	double X,Y,Z;			//空间坐标

	SCONTROLPOINT()
	{
		name[0]='\0';
		X=Y=Z=0.0;
		attrbute=0;
	}
};

//---------------------------影像控制点---------------------------
struct PCONTROLPOINT	
{
	char name[10];			//控制点名
	int pno;				//所在影像号
	double x,y;				//平面坐标

	PCONTROLPOINT()
	{
		name[0]='\0';
		pno=-1;
		x=y=0.0;
	}
};

//-----------------------------空间点-----------------------------
struct SPACEPOINT
{
	long id;				//空间点的点号
	double X,Y,Z;			//空间点坐标

	SPACEPOINT()
	{
		id=-1;
		X=Y=Z=0.0;
	}
	SPACEPOINT(long Id,double x,double y,double z)
	{
		id=Id;
		X=x;
		Y=y;
		Z=z;
	} 
};

//---------------------------普通二维点---------------------------
struct POINT2D
{
	double x,y;		//笛卡尔坐标系中，分别为X,Y坐标；向量坐标系中，分别为角度(x)与距离(y)

	POINT2D()
	{
		x=y=0.0;
	}
	POINT2D(double X,double Y)
	{
		x=X;
		y=Y;
	}
	POINT2D(IMGPOINT pt)
	{
		x=pt.x;
		y=pt.y;
	}
};

//----------------------动态二维点类,add by LX-------------------
class CArray_POINT2D
{
public:
	CArray_POINT2D( POINT2D* pBuf=NULL,int size=0 )
	{	
		m_maxSize = m_size = size; 
		m_pBuf = NULL;
		if ( m_size ) 
			m_pBuf = new POINT2D[m_size];
		if ( pBuf	) 
			memcpy( m_pBuf,pBuf,sizeof(POINT2D)*m_size );
	};
	virtual ~CArray_POINT2D()
	{ 
		if (m_pBuf) 
			delete []m_pBuf;
	};
	
	inline	POINT2D*	GetData(int &size){ size=m_size; return m_pBuf; };
	inline	POINT2D*	GetData(void){  return m_pBuf; };
	inline	int			Append( POINT2D uint ){ if ( m_size >= m_maxSize ){  m_maxSize += 256; POINT2D* pOld = m_pBuf; m_pBuf	 = new POINT2D[m_maxSize]; memset( m_pBuf,0,sizeof(POINT2D)*m_maxSize ); memcpy( m_pBuf,pOld,sizeof(POINT2D)*m_size );	delete []pOld; } m_pBuf[m_size]=uint;m_size++;return (m_size-1); };
	inline	void		SetSize(int size ){ if (size<m_maxSize) m_size=size; else Reset(NULL,size);  };
    inline	void		Reset( POINT2D* pBuf=NULL,int size=0 ){ if (m_pBuf) delete []m_pBuf; m_pBuf = NULL; m_maxSize = m_size = size; if ( m_maxSize ){ m_pBuf = new POINT2D[m_maxSize]; memset( m_pBuf,0,sizeof(POINT2D)*m_maxSize ); } if ( pBuf ) memcpy( m_pBuf,pBuf,sizeof(POINT2D)*m_size ); }
    inline  POINT2D&    operator[](int idx){ return m_pBuf[idx];   };
	inline	POINT2D	    GetAt(int idx){ return m_pBuf[idx];		   };
	inline	void		RemoveAt(int idx){ if(idx==m_size-1) {m_size--;return;} memcpy(m_pBuf+idx,m_pBuf+idx+1,sizeof(POINT2D)*(m_size-idx-1)); m_size--;}
	inline	int			GetSize(){ return m_size;				   };
	inline	int		    InsertAt(int idx,POINT2D uint){ if(idx==m_size-1) {return Append(uint); }if ( m_size >= m_maxSize ){  m_maxSize += 256; POINT2D* pOld = m_pBuf; m_pBuf	 = new POINT2D[m_maxSize]; memset( m_pBuf,0,sizeof(POINT2D)*m_maxSize ); memcpy( m_pBuf,pOld,sizeof(POINT2D)*m_size );	delete []pOld; } memcpy(m_pBuf+idx+1,m_pBuf+idx,(m_size-idx)*sizeof(POINT2D));memcpy(m_pBuf+idx,&uint,sizeof(POINT2D));m_size++;return (m_size-1);}
private:
	POINT2D*		m_pBuf;
	int				m_size;
	int				m_maxSize;
};

//---------------------------普通三维点---------------------------
struct POINT3D
{
	double X,Y,Z;

	POINT3D()
	{
		X=Y=Z=0.0;
	}
	POINT3D(double x,double y,double z)
	{
		X=x;
		Y=y;
		Z=z;
	}
	POINT3D(SPACEPOINT sp)
	{
		X=sp.X;
		Y=sp.Y;
		Z=sp.Z;
	}
};

//------------------------配准点,add by LX------------------------
typedef struct tag_MATCH_POINT
{
	POINT2D ptSrc;			// 匹配前的理论点位
	POINT2D ptDst;			// 匹配后的实际点位
	double dX;				// X残差
	double dY;				// Y残差
	double dXY;				// XY残差
	BOOL bIsValid;			// 是否参与迭代计算
	
	tag_MATCH_POINT()
	{
		dX = dY = dXY = 0.0;
		bIsValid = TRUE;
	}
	
}MATCH_POINT;

//--------------------------普通一维直线--------------------------
struct LINEVERTEX
{
	int verbeg; //直线起点顶点序号 
	int verend; //直线终点顶点序号

	LINEVERTEX()
	{
		verbeg=-1;
		verend=-1;
	}
	LINEVERTEX(int Beg,int End)
	{
		verbeg=Beg;
		verend=End;
	}
};

//--------------------------普通二维直线--------------------------
struct LINE2D
{
	POINT2D begine;	//起点
	POINT2D end;	//终点

	LINE2D()
	{
		begine=begine=POINT2D();
	};
	LINE2D(POINT2D b,POINT2D e)
	{
		begine=b;
		end=e;
	};
	LINE2D(double x1,double y1,double x2,double y2)
	{
		begine=POINT2D(x1,y1);
		end=POINT2D(x2,y2);
	};
};

//--------------------------普通三维直线--------------------------
struct LINE3D	
{
	POINT3D begine;	//起点
	POINT3D end;	//终点

	LINE3D()
	{
		begine=begine=POINT3D();
	};
	LINE3D(POINT3D b,POINT3D e)
	{
		begine=b;
		end=e;
	};
	LINE3D(double x1,double y1,double z1,double x2,double y2,double z2)
	{
		begine=POINT3D(x1,y1,z1);
		end=POINT3D(x2,y2,z2);
	};
};

//-------------------------普通二维多边形-------------------------
struct POLYGON2D
{
	int vnum;			//顶点数
	int mkpro;			//多边形属性标记
	int pro1;			//属性1
	int pro2;			//属性2
	POINT2D *pvertex;	//顶点数据
	POLYGON2D *next;	//下一个多边形

	POLYGON2D()
	{
		vnum=0;
		mkpro=0;
		pro1=0;
		pro2=0;
		pvertex=NULL;
		next=NULL;
	};

	~POLYGON2D()
	{
		if (pvertex)
		{
			delete pvertex;
			pvertex=NULL;
		}
		if (next)
		{
			delete next;
			next=NULL;
		}
	};
};

//----------------------------边界属性----------------------------
struct ECOIMGIDX
{
	int alb;      //左下区域属性
	int art;      //右上区域属性
	ECOIMGIDX()
	{
		alb=-1;
		art=-1;
	}
};

//----------------------边界属性类,add by LX----------------------
class CArray_ECOIMGIDX
{
public:
	CArray_ECOIMGIDX( ECOIMGIDX* pBuf=NULL,int size=0 )
	{	
		m_maxSize = m_size = size; 
		m_pBuf = NULL;
		if ( m_size ) 
			m_pBuf = new ECOIMGIDX[m_size];
		if ( pBuf	) 
			memcpy( m_pBuf,pBuf,sizeof(ECOIMGIDX)*m_size );
	};
	virtual ~CArray_ECOIMGIDX()
	{ 
		if (m_pBuf) 
			delete []m_pBuf;
	};
	
	inline ECOIMGIDX* GetData(int &size) { size=m_size;return m_pBuf; };
	inline int        Append( ECOIMGIDX uint ){	if ( m_size >= m_maxSize ){	m_maxSize += 256;ECOIMGIDX* pOld = m_pBuf;m_pBuf = new ECOIMGIDX[m_maxSize]; memset( m_pBuf,0,sizeof(ECOIMGIDX)*m_maxSize );memcpy( m_pBuf,pOld,sizeof(ECOIMGIDX)*m_size );delete []pOld; }m_pBuf[m_size]=uint;m_size++;return (m_size-1); };
	inline void	      SetSize(int size)	 { if (size<m_maxSize)m_size=size;else Reset(NULL,size); };
    inline void	      Reset( ECOIMGIDX* pBuf=NULL,int size=0 ){	if (m_pBuf)delete []m_pBuf;m_pBuf = NULL;m_maxSize = m_size = size;if(m_maxSize){m_pBuf = new ECOIMGIDX[m_maxSize];memset( m_pBuf,0,sizeof(ECOIMGIDX)*m_maxSize );}if (pBuf)memcpy( m_pBuf,pBuf,sizeof(ECOIMGIDX)*m_size ); }
    inline ECOIMGIDX& operator[](int idx){ return m_pBuf[idx]; };
	inline ECOIMGIDX  GetAt(int idx)	 { return m_pBuf[idx]; };
	inline void	      RemoveAt(int idx)  { if(idx==m_size-1){ m_size--;return; }memcpy(m_pBuf+idx,m_pBuf+idx+1,sizeof(ECOIMGIDX)*(m_size-idx-1));m_size--; }
	inline int        GetSize()          { return m_size; };
private:
	ECOIMGIDX*		m_pBuf;
	int				m_size;
	int				m_maxSize;
};

//-----------------------------边界线-----------------------------
struct ATTRILINE
{
	POINT2D b;  //起点
	POINT2D e;  //终点
	int alb;    //左下区域属性
	int art;    //右上区域属性

	ATTRILINE()
	{
		b=POINT2D();
		e=POINT2D();
		alb=-1;
		art=-1;
	}
};

//--------------------动态二维边界线类,add by LX------------------
class CArray_ATTRILINE
{
public:
	CArray_ATTRILINE( ATTRILINE* pBuf=NULL,int size=0 )
	{	
		m_maxSize = m_size = size; 
		m_pBuf = NULL;
		if ( m_size ) 
			m_pBuf = new ATTRILINE[m_size];
		if ( pBuf	) 
			memcpy( m_pBuf,pBuf,sizeof(ATTRILINE)*m_size );
	};
	virtual ~CArray_ATTRILINE()
	{ 
		if (m_pBuf) 
			delete []m_pBuf;
	};
	
	inline	ATTRILINE*	GetData(int &size){ size=m_size; return m_pBuf; };
	inline	ATTRILINE*	GetData(void){  return m_pBuf; };
	inline	int			Append( ATTRILINE uint ){ if ( m_size >= m_maxSize ){  m_maxSize += 256; ATTRILINE* pOld = m_pBuf; m_pBuf	 = new ATTRILINE[m_maxSize]; memset( m_pBuf,0,sizeof(ATTRILINE)*m_maxSize ); memcpy( m_pBuf,pOld,sizeof(ATTRILINE)*m_size );	delete []pOld; } m_pBuf[m_size]=uint;m_size++;return (m_size-1); };
	inline	void		SetSize(int size ){ if (size<m_maxSize) m_size=size; else Reset(NULL,size);  };
    inline	void		Reset( ATTRILINE* pBuf=NULL,int size=0 ){ if (m_pBuf) delete []m_pBuf; m_pBuf = NULL; m_maxSize = m_size = size; if ( m_maxSize ){ m_pBuf = new ATTRILINE[m_maxSize]; memset( m_pBuf,0,sizeof(ATTRILINE)*m_maxSize ); } if ( pBuf ) memcpy( m_pBuf,pBuf,sizeof(ATTRILINE)*m_size ); }
    inline  ATTRILINE&   operator[](int idx){ return m_pBuf[idx];   };
	inline	ATTRILINE	GetAt(int idx){ return m_pBuf[idx];		   };
	inline	void		RemoveAt(int idx){ if(idx==m_size-1) {m_size--;return;} memcpy(m_pBuf+idx,m_pBuf+idx+1,sizeof(ATTRILINE)*(m_size-idx-1)); m_size--;}
	inline	int			GetSize(){ return m_size;				   };
	inline	int		InsertAt(int idx,ATTRILINE uint){ if(idx==m_size-1) {return Append(uint); }if ( m_size >= m_maxSize ){  m_maxSize += 256; ATTRILINE* pOld = m_pBuf; m_pBuf	 = new ATTRILINE[m_maxSize]; memset( m_pBuf,0,sizeof(ATTRILINE)*m_maxSize ); memcpy( m_pBuf,pOld,sizeof(ATTRILINE)*m_size );	delete []pOld; } memcpy(m_pBuf+idx+1,m_pBuf+idx,(m_size-idx)*sizeof(ATTRILINE));memcpy(m_pBuf+idx,&uint,sizeof(ATTRILINE));m_size++;return (m_size-1);}
private:
	ATTRILINE*		m_pBuf;
	int				m_size;
	int				m_maxSize;
};




//-----------------------------以下暂未使用-----------------------------


struct TIN3D
{
	POINT3D *pt3d;
	int pt3dnum;

	int *v123;
	int tinnum;

	TIN3D()
	{
		pt3d=NULL;
		v123=NULL;

		pt3dnum=0;
		tinnum=0;
	}
	~TIN3D()
	{
		if (pt3d) 
		{
			delete pt3d;
			pt3d=NULL;
		}
		if (v123) 
		{
			delete v123;
			v123=NULL;
		}
		pt3dnum=0;
		tinnum=0;
	}
	double GetMeanZ()
	{
		if (pt3dnum<=0) return 0.0;
		double meanz=0.0;
		for (int i=0;i<pt3dnum;i++)
		{
			meanz+=pt3d[i].Z;
		}
		meanz/=pt3dnum;
		return meanz;
	}
};

struct DEM 
{
    double x0,y0;
    double kapa;
    float  noVal;
    float  dx,dy;
    int    col,row;
    float  *pGrid;	

	DEM()
	{
		x0=y0=0.0;
		kapa=0.0;
		noVal=-9999.9f;
		dx=dy=0.0;
		col=row=0;
		pGrid=NULL;	
	}
	~DEM()
	{
		x0=y0=0.0;
		kapa=0.0;
		noVal=-9999.9f;
		dx=dy=0.0;
		col=row=0;
		if (pGrid) 
		{
			delete pGrid;
			pGrid=NULL;	
		}
	}
};

struct FRAMEBOX       //图幅数据类
{
	double l,r,b,t;		//图幅范围
	char FrmName[64];   //图幅名
	char ZyyName[64];
	char ZjName[64];

	FRAMEBOX()
	{
		l=r=b=t=0.0;
		memset(FrmName,0,64*sizeof(char));
		memset(ZyyName,0,64*sizeof(char));
		memset(ZjName ,0,64*sizeof(char));
	}
};

struct ATIMAGE		//空三影像点
{
	int imgid;		//影像号
	double x,y;		//x,y坐标
	ATIMAGE *next;	//下一点地址

	ATIMAGE()
	{
		imgid=0;
		x=y=0.0;
		next=NULL;
	}

	ATIMAGE(int Id,double X,double Y,ATIMAGE *Next=NULL)
	{
		imgid=Id;
		x=X;
		y=Y;
		next=Next;
	}

	~ATIMAGE()
	{
		if (next!=NULL)
		{
			delete next;
			next=NULL;
 		}
	}
};

struct ATPOINT		//空三、自由网数据结构
{
	POINT3D spapoint;	//空间点坐标
	int imgpnum;		//影像点数量
	ATIMAGE *imgpoint;	//影像点坐标与其对应的影像片号,并连接到下一影像点

	ATPOINT()
	{
		spapoint = SPACEPOINT();
		imgpnum=0;
		imgpoint=NULL;
	}
	ATPOINT(POINT3D sp,int ipnum,ATIMAGE *imgp)
	{
		spapoint=sp;
		imgpnum=ipnum;

		ATIMAGE *curpt=imgpoint=new ATIMAGE;
		ATIMAGE *curp=imgp;

		for (int i=0;i<imgpnum;i++)
		{
			memcpy(curpt,curp,sizeof(ATIMAGE));
			curp=curp->next;
			curpt->next=new ATIMAGE;
			curpt=curpt->next;
		}
		delete curpt;
	}

	~ATPOINT()
	{
		if (imgpoint!=NULL) 
		{
			delete imgpoint;
			imgpoint=NULL;
		}
	}
};

struct ATLINK		//空三点连接码
{
	long id;			//点号
	ATPOINT *point;		//数据地址

	ATLINK()
	{
		id=0;
		point=NULL;
	}

	~ATLINK()
	{
		if (point!=NULL)
		{
			delete point;
			point=NULL;
		}
	}
};

struct MODTRANSLATE		//模型变换参数的数据结构
{
	double scale;				//比例Scale
	double Xm,Ym,Zm;			//平移Offset
	double fai,wmg,kav;			//旋转Rotation

	MODTRANSLATE()
	{
		scale=1.0;
		Xm=Ym=Zm=0.0;
		fai=wmg=kav=0.0;
	}
	MODTRANSLATE(double s,double x,double y,double z,double f,double w,double k)
	{
		scale=s;
		Xm=x;
		Ym=y;
		Zm=z;
		fai=f;
		wmg=w;
		kav=k;		
	}
};

struct GRAYDATA  //灰度图像块的数据结构
{
	BYTE *data;			//灰度值矩阵，以左下点为原点
	int w,h;			//灰度矩阵宽高
	double xset,yset;	//灰度矩阵的左下角在原影像中的坐标(在核线相关中恢复灰度的坐标有用)

	GRAYDATA()
	{
		data=NULL;
		w=h=0;
		xset=yset=0;
	}
	GRAYDATA(int W,int H,double X=0,double Y=0)
	{
		w=W;
		h=H;
		xset=X;
		yset=Y;
		data=new BYTE[w*h];

		memset(data, 0, w*h*sizeof(BYTE));
	}
	GRAYDATA(int W,int H,BYTE *Data,double X=0,double Y=0)
	{
		w=W;
		h=H;
		xset=X;
		yset=Y;
		data=new BYTE[w*h];

		memcpy(data, Data, w*h*sizeof(BYTE));
	}
};

struct COLORPOINT  //最小二乘平差匀光数据结构
{
	int mvid;			//类型号（均值:0; 方差:1）
	int bandid;			//波段号（R:0; G:1; B:2; Gray:0） 
	int imgid1,imgid2;	//在两张影像上的影像号
	float data1,data2;  //两个具体数值
	long weight;		//有效像元数（权）

	COLORPOINT()
	{
		mvid=-1;			
		bandid=-1;			
		imgid1=-1;
		imgid2=-1;	
		data1=0.0;
		data2=0.0; 
		weight=0;
	}
};

struct COLORPARA	//影像的颜色变换参数
{
	int mvid;			//类型号（均值:0; 方差:1）
	int bandid;			//波段号（R:0; G:1; B:2; Gray:0） 
	int imgid;			//影像号
	int rowid;			//航带号
	int colid;			//航片号
	double b;			//补偿因子

	COLORPARA()
	{
		mvid=-1;			
		bandid=-1;			
		imgid=-1;		
		rowid=-1;
		colid=-1;
		b=0.0;			
	}
};

struct MATCHPOINT		//一对匹配点的结构
{
	long	id;				//点号
	int		linkmark;		//链接码
	int		lpno,rpno;		//左右影像的影像号
	double	lx,ly,rx,ry;	//左右影像上的点的坐标
	int		cor;			//相关度
	double	res;			//残差
	MATCHPOINT()
	{
		id=-1;
		linkmark=-1;
		lpno=rpno=-1;
		lx=ly=rx=ry=0.0;
		cor=0;
		res=0;
	}

	MATCHPOINT(long ID,int Link,int Lpno,int Rpno,double Lx,double Ly,double Rx,double Ry,int Cor,double Res)
	{
		id=ID;
		linkmark=Link;
		lpno=Lpno;
		rpno=Rpno;
		lx=Lx;
		ly=Ly;
		rx=Rx;
		ry=Ry;
		cor=Cor;
		res=Res;
	}
};

struct SEEDPOINT
{
	int row1,col1;	//左影像的航带号，航片号
	double x1,y1;	//左影像上的x，y坐标

	int row2,col2;	//右影像的航带号，航片号
	double x2,y2;	//右影像上的x，y坐标

	SEEDPOINT()
	{
		row1=col1=-1;
		x1=y1=0;

		row2=col2=-1;
		x2=y2=0;	
	}
	SEEDPOINT(int R1,int C1,double X1,double Y1,int R2,int C2,double X2,double Y2)
	{
		row1=R1;
		col1=C1;
		x1=X1;
		y1=Y1;

		row2=R2;
		col2=C2;
		x2=X2;
		y2=Y2;	
	}
};

struct STRIPOFFSET
{
	int row;
	int offset;

	STRIPOFFSET()
	{
		row=0;
		offset=0;
	}
};

struct IPADDRESS
{
	int id1,id2,id3,id4;

	IPADDRESS()
	{
		id1=id2=id3=id4=0;
	}

	IPADDRESS(int Id1,int Id2,int Id3,int Id4)
	{
		id1=Id1;
		id2=Id2;
		id3=Id3;
		id4=Id4;
	}
};

//
//为多边形裁切定义的结构
struct POLYVERTEX  //双向链表的多边形顶点结构
{
	int idno; //顶点的id，id号为0，则为新增顶点
	double x,y;//顶点的坐标

	POLYVERTEX *front; //前向节点指针
	POLYVERTEX *after; //后向节点指针

	//在该节点为新节点时，以下结构起作用
	POLYVERTEX *goalfr; //指向被裁多边形的前向节点指针
	POLYVERTEX *goalaf; //指向被裁多边形的后向节点指针

	POLYVERTEX *cutfr; //指向裁切多边形的前向节点指针
	POLYVERTEX *cutaf; //指向裁切多边形的后向节点指针

	POLYVERTEX()
	{
		int idno=0;
		x=y=0;
		front=after=goalfr=goalaf=cutfr=cutaf=NULL;
	}

	~POLYVERTEX()
	{
		if (after&&after->idno) 
		{
			delete after;
			after=NULL;
		}
		front=after=goalfr=goalaf=cutfr=cutaf=NULL;
	}
};

struct SUPPOLYGON   //双向链表多边形的结构
{
	int polyid;				//多边形编号
	int pnum;				//结点数
	POLYVERTEX *pvertex;	//首结点指针
	POLYVERTEX *endvert;	//末结点指针

	SUPPOLYGON()
	{
		polyid=-1;
		pnum=0;
		pvertex=NULL;
		endvert=NULL;
	}

	~SUPPOLYGON()
	{
		if (pvertex)
		{
			delete pvertex;
			pvertex=NULL;
		}
	}
};
//为多边形裁切定义的结构
//



//------------------------------------2013.8.21------------------------------
typedef struct tagRGBQUAD_16 
{
	WORD    rgbBlue;
	WORD    rgbGreen;
	WORD    rgbRed;
	WORD    rgbReserved;
} RGBQUAD_16;

typedef struct tag_AFFINEPARA
{
	double R[2][2];
	double T[2];
	
	tag_AFFINEPARA()
	{
		R[0][0] = 0.0;
		R[0][1] = 0.0;
		R[1][0] = 0.0;
		R[1][1] = 0.0;
		
		T[0] = 0.0;
		T[1] = 0.0;
	};
	
}AFFINEPARA;




class CBasicStructer  
{
public:
	CBasicStructer(){};
	virtual ~CBasicStructer(){};
};

#endif // !defined(AFX_BASICSTRUCTER_H__1AC46295_5380_4F70_8E31_233FEA895573__INCLUDED_)
