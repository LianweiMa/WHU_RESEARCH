// DEM_CompareDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DEM_Compare.h"
#include "DEM_CompareDlg.h"
#include "BasicStructer.h"
#include "SpDem.hpp"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDEM_CompareDlg dialog

CDEM_CompareDlg::CDEM_CompareDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDEM_CompareDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDEM_CompareDlg)
	m_CmpPath = _T("");
	m_StdPath = _T("");
	m_Max = 0.0f;
	m_Mean = 0.0f;
	m_Min = 0.0f;
	m_Mse = 0.0f;
	m_CmpCol = 0;
	m_CmpRow = 0;
	m_CmpStaX = 0.0;
	m_CmpStaY = 0.0;
	m_MaxX = 0.0;
	m_MaxY = 0.0;
	m_MinY = 0.0;
	m_StdCol = 0;
	m_StdRow = 0;
	m_StdStaX = 0.0;
	m_StdStaY = 0.0;
	m_MinX = 0.0;
	m_StdDx = 0.0;
	m_CmpDx = 0.0;
	m_CmpDy = 0.0;
	m_StdDy = 0.0;
	m_GridNum = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDEM_CompareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDEM_CompareDlg)
	DDX_Text(pDX, IDC_EDIT_CmpPath, m_CmpPath);
	DDX_Text(pDX, IDC_EDIT_StdPath, m_StdPath);
	DDX_Text(pDX, IDC_EDIT_MAX, m_Max);
	DDX_Text(pDX, IDC_EDIT_MEAN, m_Mean);
	DDX_Text(pDX, IDC_EDIT_MIN, m_Min);
	DDX_Text(pDX, IDC_EDIT_MSE, m_Mse);
	DDX_Text(pDX, IDC_EDIT_CMPCOL, m_CmpCol);
	DDX_Text(pDX, IDC_EDIT_CMPROW, m_CmpRow);
	DDX_Text(pDX, IDC_EDIT_CMPSTAX, m_CmpStaX);
	DDX_Text(pDX, IDC_EDIT_CMPSTAY, m_CmpStaY);
	DDX_Text(pDX, IDC_EDIT_MAXX, m_MaxX);
	DDX_Text(pDX, IDC_EDIT_MAXY, m_MaxY);
	DDX_Text(pDX, IDC_EDIT_MINY, m_MinY);
	DDX_Text(pDX, IDC_EDIT_STDCOL, m_StdCol);
	DDX_Text(pDX, IDC_EDIT_STDROW, m_StdRow);
	DDX_Text(pDX, IDC_EDIT_STDSTAX, m_StdStaX);
	DDX_Text(pDX, IDC_EDIT_STDSTAY, m_StdStaY);
	DDX_Text(pDX, IDC_EDIT_MINX, m_MinX);
	DDX_Text(pDX, IDC_EDIT_STDDX, m_StdDx);
	DDX_Text(pDX, IDC_EDIT_CMPDX, m_CmpDx);
	DDX_Text(pDX, IDC_EDIT_CMPDY, m_CmpDy);
	DDX_Text(pDX, IDC_EDIT_STDDY, m_StdDy);
	DDX_Text(pDX, IDC_EDIT_GRIDNUM, m_GridNum);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDEM_CompareDlg, CDialog)
//{{AFX_MSG_MAP(CDEM_CompareDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_BUTTON_STDDEM, OnButtonStddem)
ON_BN_CLICKED(IDC_BUTTON_CMPDEM, OnButtonCmpdem)
ON_BN_CLICKED(IDC_BUTTON_OK, OnButtonOk)
ON_BN_CLICKED(IDC_BUTTON_GETALL, OnButtonGetall)
ON_BN_CLICKED(IDC_BUTTON_GETREG, OnButtonGetreg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDEM_CompareDlg message handlers

BOOL CDEM_CompareDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Add "About..." menu item to system menu.
	
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDEM_CompareDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CDEM_CompareDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDEM_CompareDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDEM_CompareDlg::OnButtonStddem() 
{
	CFileDialog filedlg(TRUE);
	filedlg.m_ofn.lpstrTitle="Open";
	filedlg.m_ofn.lpstrFilter="Text Files(*.dem)\0*.dem\0Text Files(*.bem)\0*.bem\0All Files(*.*)\0*.*\0\0";
	if(IDOK==filedlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT_StdPath,filedlg.GetPathName());
		m_StdPath= filedlg.GetPathName();
	}
	
	if(!m_StdPath.IsEmpty())
	{
		CSpDem StdDem;
		SPDEMHDR StdDemHdr;
		//StdDem.Open(m_StdPath,CSpDem::modeRead,&StdDemHdr);
		StdDem.Load4File(m_StdPath);
		StdDemHdr=StdDem.GetDemHeader();

		m_StdRow = StdDemHdr.row;
		m_StdCol = StdDemHdr.column;
		m_StdStaX= StdDemHdr.startX;
		m_StdStaY= StdDemHdr.startY;
		m_StdDx  = StdDemHdr.intervalX;
		m_StdDy  = StdDemHdr.intervalY;		
		
		UpdateData(FALSE);
	}
	else 
		MessageBox("请输入DEM文件！","错误提示");	
}

void CDEM_CompareDlg::OnButtonCmpdem() 
{
	CFileDialog filedlg(TRUE);
	filedlg.m_ofn.lpstrTitle="Open";
	filedlg.m_ofn.lpstrFilter="Text Files(*.dem)\0*.dem\0Text Files(*.bem)\0*.bem\0All Files(*.*)\0*.*\0\0";
	if(IDOK==filedlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT_CmpPath,filedlg.GetPathName());
		m_CmpPath= filedlg.GetPathName();
	}
	
	if(!m_CmpPath.IsEmpty())
	{
		CSpDem CmpDem;
		SPDEMHDR CmpDemHdr;
		//CmpDem.Open(m_CmpPath,CSpDem::modeRead,&CmpDemHdr);
		CmpDem.Load4File(m_CmpPath);
		CmpDemHdr = CmpDem.GetDemHeader();

		m_CmpRow = CmpDemHdr.row;
		m_CmpCol = CmpDemHdr.column;		
		m_CmpStaX= CmpDemHdr.startX;
		m_CmpStaY= CmpDemHdr.startY;		
		m_CmpDx  = CmpDemHdr.intervalX;
		m_CmpDy  = CmpDemHdr.intervalY;
		
		UpdateData(FALSE);
	}
	else 
		MessageBox("请输入DEM文件！","错误提示");	
}

void CDEM_CompareDlg::OnButtonGetreg() 
{
	if( m_StdStaX && m_CmpStaX )
	{
		double StdEndX = m_StdStaX+(m_StdCol-1)*m_StdDx;
		double StdEndY = m_StdStaY+(m_StdRow-1)*m_StdDy;
		double CmpEndX = m_CmpStaX+(m_CmpCol-1)*m_CmpDx;
		double CmpEndY = m_CmpStaY+(m_CmpRow-1)*m_CmpDy;
		
		//-------------------------------------确定两DEM坐标范围的交集--------------------------------------
		if ((m_StdStaX>=CmpEndX)||(m_CmpStaX>=StdEndX)||(m_StdStaY>=CmpEndY)||(m_CmpStaY>=StdEndY))
		{
			MessageBox("输入的两个DEM之间无交集！","错误提示");
		}	
		else
		{
			m_MinX = max(m_StdStaX,m_CmpStaX);
			m_MaxX = min(StdEndX,CmpEndX);
			m_MinY = max(m_StdStaY,m_CmpStaY);
			m_MaxY = min(StdEndY,CmpEndY);

			int i,j;

			//为保证标准DEM上参与计算的点均在标准格网上，在交集范围内取标准DEM格网的最大范围
			for (i=0;i<m_StdRow;i++)
			{
				if ((m_MinY-(m_StdStaY+i*m_StdDy))<m_StdDy)
				{
					m_MixStaRow=i+1;
					m_MinY=m_StdStaY+(i+1)*m_StdDy;
					break;
				}
			}
			for (i=0;i<m_StdRow;i++)
			{
				if (((StdEndY-i*m_StdDy)-m_MaxY)<m_StdDy)
				{
					m_MixEndRow=m_StdRow-2-i;
					m_MaxY=StdEndY-(i+1)*m_StdDy;
					break;
				}
			}	
			for (j=0;j<m_StdCol;j++)
			{
				if ((m_MinX-(m_StdStaX+j*m_StdDx))<m_StdDx)
				{
					m_MixStaCol=j+1;
					m_MinX=m_StdStaX+(j+1)*m_StdDx;
					break;
				}
			}
			for (j=0;j<m_StdCol;j++)
			{
				if (((StdEndX-j*m_StdDx)-m_MaxX)<m_StdDx)
				{
					m_MixEndCol=m_StdCol-2-j;
					m_MaxX=StdEndX-(j+1)*m_StdDx;
					break;
				}
			}
			//计算格网点数
			m_GridNum=(m_MixEndRow-m_MixStaRow+1)*(m_MixEndCol-m_MixStaCol+1);

			UpdateData(FALSE);
		}
	}
	else
		MessageBox("首先获取dem头信息！","错误提示");
}

void CDEM_CompareDlg::OnButtonGetall() 
{
	if(m_GridNum)
	{
		CSpDem StdDem,CmpDem;
		SPDEMHDR StdDemHdr;
		//StdDem.Open(m_StdPath,CSpDem::modeRead,&StdDemHdr);
		StdDem.Load4File(m_StdPath);
		StdDemHdr = StdDem.GetDemHeader();
		//CmpDem.Open(m_CmpPath,CSpDem::modeRead,&CmpDemHdr);
		CmpDem.Load4File(m_CmpPath);

		//-------------------------------------建立新DEM文件--------------------------------------
		SPDEMHDR RetDemHdr;
		RetDemHdr.intervalX = StdDemHdr.intervalX;
		RetDemHdr.intervalY = StdDemHdr.intervalY;
		RetDemHdr.kapa = StdDemHdr.kapa;
		RetDemHdr.row =  m_MixEndRow-m_MixStaRow+1;
		RetDemHdr.column = m_MixEndCol-m_MixStaCol+1;
		RetDemHdr.startX = m_MinX;
		RetDemHdr.startY = m_MinY;
			
		//---------------------------------------计算开始-----------------------------------------	
		//新DEM分块
		int stepH = 300;
		int stepW = 300;		
		int stepCntY = RetDemHdr.row/stepH;
		int stepCntX = RetDemHdr.column/stepW;
		if (RetDemHdr.row%stepH) stepCntY++;
		if (RetDemHdr.column%stepW) stepCntX++;
					
		float *pOriBlock = new float[stepH*stepW];   //原始DEM每块存储的高程值
/*
		for (int i = 0; i < stepH;i++)
		{
			for (int j = 0; j < stepW;j++)
			{
				pOriBlock[i*stepW+j] = NOVALUE;
			}
		}*/
		//memset(pOriBlock,NOVALUE,sizeof(float)*stepH*stepW);
			
		double sumdif=0,sumdif2=0;
		m_Min = 10000;
		m_Max =-10000;
			
		for (int ky=0;ky<stepCntY;ky++)
		{
			int ybeg=max(0, min(stepH*ky, RetDemHdr.row-1));
			int yend=max(0, min(stepH*(ky+1)-1,RetDemHdr.row));
					
			for (int kx=0; kx<stepCntX; kx++)
			{
				int xbeg=max(0, min(stepW*kx, RetDemHdr.column-1));	
				int xend=max(0, min(stepW*(kx+1)-1,RetDemHdr.column));
						
				//StdDem.ReadBlock(pOriBlock,ybeg+m_MixStaRow,xbeg+m_MixStaCol,stepH,stepW);
/*
				for (int i = ybeg; i <=yend;i++)
				{
					for (int j = xbeg; j <=xend;j++)
					{
						*pOriBlock = StdDem.GetDemZVal(j + m_MixStaCol, i + m_MixStaRow);
						pOriBlock++;
					}
				}*/
						
				double xGeo=0.0, yGeo=0.0;
				float zNewVal=0.0;
				float zOriVal=0.0;
				float diff=0;

				for (int j=ybeg; j<=yend; j++)
				{
					for (int i=xbeg; i<=xend; i++)
					{
						//获取dem新老值
						xGeo = i*RetDemHdr.intervalX+RetDemHdr.startX;
				    	yGeo = j*RetDemHdr.intervalY+RetDemHdr.startY;

						//zOriVal = pOriBlock[(j-ybeg)*stepH+(i-xbeg)];
						zOriVal = StdDem.GetDemZValue(xGeo, yGeo);
						zNewVal = CmpDem.GetDemZValue(xGeo,yGeo);
						if (zNewVal==NOVALUE || zOriVal==NOVALUE) continue;

						//统计误差的最值、平均值、均方误差
						diff = zNewVal-zOriVal;
    					if (diff>m_Max) m_Max=diff;
						if (diff<m_Min) m_Min=diff;

						sumdif += diff;
						sumdif2+= diff*diff;
					}
				}
			}
		}

		m_Mean=sumdif/((RetDemHdr.row)*(RetDemHdr.column));
		m_Mse=sqrt(sumdif2/((RetDemHdr.row)*(RetDemHdr.column)));
		
		StdDem.Close();
		CmpDem.Close();			
		pOriBlock=NULL; delete pOriBlock;
        MessageBox("参数获取成功！","成功提示");
		
		UpdateData(FALSE);
	}
	else 
		MessageBox("请输入DEM文件！","错误提示");	
}

void CDEM_CompareDlg::OnButtonOk() 
{
	CFileDialog filedlg(FALSE);
	filedlg.m_ofn.lpstrTitle="Save";
	filedlg.m_ofn.lpstrDefExt="txt"; 
	filedlg.m_ofn.lpstrFilter="Text Files(*.txt)\0*.*\0\0";
	
	if(m_StdPath.IsEmpty()==0)
	{
		char m_TempPath[512];  
		strcpy(m_TempPath,m_StdPath); 
		char *pS = strrchr( m_TempPath,'.' );	
		strcpy( pS,"_result.txt" );
		filedlg.m_ofn.lpstrFile=m_TempPath;
		FILE* fp;
		fp = fopen(m_TempPath,"w");
		fprintf(fp,"误差最大值：%f\n",m_Max);
		fprintf(fp,"误差最小值：%f\n",m_Min);
		fprintf(fp,"误差平均值：%f\n",m_Mean);
		fprintf(fp,"均方误差：%f\n",m_Mse);
		fclose(fp);
		MessageBox("输出比较结果文件成功！","提示信息");
	}
	else MessageBox("请输入DEM文件！","错误提示");
}
