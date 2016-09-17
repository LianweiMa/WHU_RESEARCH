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

//------------------------------����------------------------------
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

//------------------------------Ӱ��------------------------------
struct PHOTOINFO
{
	char path[512]; //Ӱ��·��
	char name[512];	//Ӱ����
	int row;		//Ӱ���к�
	int col;		//Ӱ���к�
	int rotate;		//��ת��ǣ�0��������1����ʱ��90�ȣ�2����ʱ��180�ȣ�3����ʱ��270�ȣ�
	BOOL sel;		//�Ƿ�ѡ��(�Ѿ���ɿ�����Ƕ)

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

//----------------------------�������----------------------------
struct CAMPARA
{
	double f,u0,v0;			//���࣬����		Necessary!

	double w,h;				//�������		Not necessary!
	double pxy;				//��Ԫ�ĺ��ݱ�		Not necessary!
	double psize;			//��Ԫ��С			Not necessary!

	int unit,origin;		//����ϵ����		unit ��λ: 0,����; 1,����		origin ԭ��:0,���½�; 1,���ĵ�

	int distortionnum;		//����ģ��			3,3��������ģ��;	4,4��������ģ�� 
	double a0,a1,r;			//3���������		Not necessary!
	double k1,k2,p1,p2;		//4���������		Not necessary!

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

//-------------------------Ӱ���ⷽλԪ��-------------------------
struct OUTPARA		
{
	double Xs,Ys,Zs;		//��վ����
	double fai,wmg,kav;		//ת�ǲ��� step1.��Y��תfai, step2.��X��תwmg, step3.��Z��תkav

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

//----------------------------����任----------------------------
struct INORPARA
{
    double a0,a1,a2; // ����任ϵ��(X����)
    double b0,b1,b2; // ����任ϵ��(Y����)
	double tt;       // ��任����

	INORPARA()
	{
		a0=a1=a2=0.0;
		b0=b1=b2=0.0;
		tt=0.0;
	}
};

//------------------------------���------------------------------
struct IMGPOINT	
{
	long id;				//Ӱ���ĵ��
	int imgid;				//Ӱ���λ�ڵ�Ӱ���
	double x,y;				//��������

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

//---------------------------�ռ���Ƶ�---------------------------
struct SCONTROLPOINT		
{
	char name[10];			//���Ƶ���
	int attrbute;			//0 һ��㣻1 ƽ�߿��Ƶ㣻2 ���㣻3 ƽ����Ƶ㣻4 �߳̿��Ƶ�
	double X,Y,Z;			//�ռ�����

	SCONTROLPOINT()
	{
		name[0]='\0';
		X=Y=Z=0.0;
		attrbute=0;
	}
};

//---------------------------Ӱ����Ƶ�---------------------------
struct PCONTROLPOINT	
{
	char name[10];			//���Ƶ���
	int pno;				//����Ӱ���
	double x,y;				//ƽ������

	PCONTROLPOINT()
	{
		name[0]='\0';
		pno=-1;
		x=y=0.0;
	}
};

//-----------------------------�ռ��-----------------------------
struct SPACEPOINT
{
	long id;				//�ռ��ĵ��
	double X,Y,Z;			//�ռ������

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

//---------------------------��ͨ��ά��---------------------------
struct POINT2D
{
	double x,y;		//�ѿ�������ϵ�У��ֱ�ΪX,Y���ꣻ��������ϵ�У��ֱ�Ϊ�Ƕ�(x)�����(y)

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

//----------------------��̬��ά����,add by LX-------------------
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

//---------------------------��ͨ��ά��---------------------------
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

//------------------------��׼��,add by LX------------------------
typedef struct tag_MATCH_POINT
{
	POINT2D ptSrc;			// ƥ��ǰ�����۵�λ
	POINT2D ptDst;			// ƥ����ʵ�ʵ�λ
	double dX;				// X�в�
	double dY;				// Y�в�
	double dXY;				// XY�в�
	BOOL bIsValid;			// �Ƿ�����������
	
	tag_MATCH_POINT()
	{
		dX = dY = dXY = 0.0;
		bIsValid = TRUE;
	}
	
}MATCH_POINT;

//--------------------------��ͨһάֱ��--------------------------
struct LINEVERTEX
{
	int verbeg; //ֱ����㶥����� 
	int verend; //ֱ���յ㶥�����

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

//--------------------------��ͨ��άֱ��--------------------------
struct LINE2D
{
	POINT2D begine;	//���
	POINT2D end;	//�յ�

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

//--------------------------��ͨ��άֱ��--------------------------
struct LINE3D	
{
	POINT3D begine;	//���
	POINT3D end;	//�յ�

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

//-------------------------��ͨ��ά�����-------------------------
struct POLYGON2D
{
	int vnum;			//������
	int mkpro;			//��������Ա��
	int pro1;			//����1
	int pro2;			//����2
	POINT2D *pvertex;	//��������
	POLYGON2D *next;	//��һ�������

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

//----------------------------�߽�����----------------------------
struct ECOIMGIDX
{
	int alb;      //������������
	int art;      //������������
	ECOIMGIDX()
	{
		alb=-1;
		art=-1;
	}
};

//----------------------�߽�������,add by LX----------------------
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

//-----------------------------�߽���-----------------------------
struct ATTRILINE
{
	POINT2D b;  //���
	POINT2D e;  //�յ�
	int alb;    //������������
	int art;    //������������

	ATTRILINE()
	{
		b=POINT2D();
		e=POINT2D();
		alb=-1;
		art=-1;
	}
};

//--------------------��̬��ά�߽�����,add by LX------------------
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




//-----------------------------������δʹ��-----------------------------


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

struct FRAMEBOX       //ͼ��������
{
	double l,r,b,t;		//ͼ����Χ
	char FrmName[64];   //ͼ����
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

struct ATIMAGE		//����Ӱ���
{
	int imgid;		//Ӱ���
	double x,y;		//x,y����
	ATIMAGE *next;	//��һ���ַ

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

struct ATPOINT		//���������������ݽṹ
{
	POINT3D spapoint;	//�ռ������
	int imgpnum;		//Ӱ�������
	ATIMAGE *imgpoint;	//Ӱ������������Ӧ��Ӱ��Ƭ��,�����ӵ���һӰ���

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

struct ATLINK		//������������
{
	long id;			//���
	ATPOINT *point;		//���ݵ�ַ

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

struct MODTRANSLATE		//ģ�ͱ任���������ݽṹ
{
	double scale;				//����Scale
	double Xm,Ym,Zm;			//ƽ��Offset
	double fai,wmg,kav;			//��תRotation

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

struct GRAYDATA  //�Ҷ�ͼ�������ݽṹ
{
	BYTE *data;			//�Ҷ�ֵ���������µ�Ϊԭ��
	int w,h;			//�ҶȾ�����
	double xset,yset;	//�ҶȾ�������½���ԭӰ���е�����(�ں�������лָ��Ҷȵ���������)

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

struct COLORPOINT  //��С����ƽ���ȹ����ݽṹ
{
	int mvid;			//���ͺţ���ֵ:0; ����:1��
	int bandid;			//���κţ�R:0; G:1; B:2; Gray:0�� 
	int imgid1,imgid2;	//������Ӱ���ϵ�Ӱ���
	float data1,data2;  //����������ֵ
	long weight;		//��Ч��Ԫ����Ȩ��

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

struct COLORPARA	//Ӱ�����ɫ�任����
{
	int mvid;			//���ͺţ���ֵ:0; ����:1��
	int bandid;			//���κţ�R:0; G:1; B:2; Gray:0�� 
	int imgid;			//Ӱ���
	int rowid;			//������
	int colid;			//��Ƭ��
	double b;			//��������

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

struct MATCHPOINT		//һ��ƥ���Ľṹ
{
	long	id;				//���
	int		linkmark;		//������
	int		lpno,rpno;		//����Ӱ���Ӱ���
	double	lx,ly,rx,ry;	//����Ӱ���ϵĵ������
	int		cor;			//��ض�
	double	res;			//�в�
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
	int row1,col1;	//��Ӱ��ĺ����ţ���Ƭ��
	double x1,y1;	//��Ӱ���ϵ�x��y����

	int row2,col2;	//��Ӱ��ĺ����ţ���Ƭ��
	double x2,y2;	//��Ӱ���ϵ�x��y����

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
//Ϊ����β��ж���Ľṹ
struct POLYVERTEX  //˫������Ķ���ζ���ṹ
{
	int idno; //�����id��id��Ϊ0����Ϊ��������
	double x,y;//���������

	POLYVERTEX *front; //ǰ��ڵ�ָ��
	POLYVERTEX *after; //����ڵ�ָ��

	//�ڸýڵ�Ϊ�½ڵ�ʱ�����½ṹ������
	POLYVERTEX *goalfr; //ָ�򱻲ö���ε�ǰ��ڵ�ָ��
	POLYVERTEX *goalaf; //ָ�򱻲ö���εĺ���ڵ�ָ��

	POLYVERTEX *cutfr; //ָ����ж���ε�ǰ��ڵ�ָ��
	POLYVERTEX *cutaf; //ָ����ж���εĺ���ڵ�ָ��

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

struct SUPPOLYGON   //˫���������εĽṹ
{
	int polyid;				//����α��
	int pnum;				//�����
	POLYVERTEX *pvertex;	//�׽��ָ��
	POLYVERTEX *endvert;	//ĩ���ָ��

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
//Ϊ����β��ж���Ľṹ
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
