
// Dem_ChangeBgValDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Dem_ChangeBgVal.h"
#include "Dem_ChangeBgValDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDem_ChangeBgValDlg dialog



CDem_ChangeBgValDlg::CDem_ChangeBgValDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDem_ChangeBgValDlg::IDD, pParent)
	, m_DemNoDataValue(0)
	, m_ChangeValue(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDem_ChangeBgValDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NO_DATA_VALUE, m_DemNoDataValue);
	DDX_Text(pDX, IDC_EDIT_CHANGE_VALUE, m_ChangeValue);
}

BEGIN_MESSAGE_MAP(CDem_ChangeBgValDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BT_OPEN_FILE, &CDem_ChangeBgValDlg::OnBnClickedBtOpenFile)
	ON_BN_CLICKED(IDC_BT_CHANGE_BG_VALUE, &CDem_ChangeBgValDlg::OnBnClickedBtChangeBgValue)
END_MESSAGE_MAP()


// CDem_ChangeBgValDlg message handlers

BOOL CDem_ChangeBgValDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDem_ChangeBgValDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDem_ChangeBgValDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDem_ChangeBgValDlg::OnBnClickedBtOpenFile()
{
	// TODO: Add your control notification handler code here
	CFileDialog filedlg(TRUE);
	filedlg.m_ofn.lpstrTitle = _T("Open DEM...");
	filedlg.m_ofn.lpstrFilter = _T("Text Files(*.tif)\0*.tif\0Text Files(*.dem)\0*.dem\0All Files(*.*)\0*.*\0\0");
	if (IDOK == filedlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT_OPEN_FILE, filedlg.GetPathName());
		m_InputFilePath = filedlg.GetPathName();
	}

	if (!m_InputFilePath.IsEmpty())
	{
		GDALDEMHDR StdDemHdr;
		char strTmp[FILE_PN];
		strcpy(strTmp, m_InputFilePath);
		m_InputDem.LoadFile(strTmp, &StdDemHdr);
		StdDemHdr = m_InputDem.GetDemHeader();

		m_DemNoDataValue=m_InputDem.GetDemNoDataValue();
		UpdateData(FALSE);
	}
	else
		MessageBox("请输入DEM文件！", "错误提示");

}


void CDem_ChangeBgValDlg::OnBnClickedBtChangeBgValue()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CFileDialog filedlg(FALSE);
	filedlg.m_ofn.lpstrTitle = "Save DEM...";
	filedlg.m_ofn.lpstrFilter = "Tiff Files(*.tif)\0*.tif\0DEM Files(*.dem)\0*.dem\0All Files(*.*)\0*.*\0\0";
	if (IDOK == filedlg.DoModal())
	{
		AfxGetApp()->BeginWaitCursor();
		CGdalDem outDem; char strTmp[FILE_PN]; strcpy(strTmp, filedlg.GetPathName());
		outDem.CreatFile(strTmp, &m_InputDem.GetDemHeader());
		int iBlockH = BlOCK_MEMERY / (sizeof(float)*m_InputDem.GetDemHeader().iCol);
		if (iBlockH > m_InputDem.GetDemHeader().iRow)iBlockH = m_InputDem.GetDemHeader().iRow;
		int iStep = m_InputDem.GetDemHeader().iRow / iBlockH;
		if (m_InputDem.GetDemHeader().iRow % iBlockH) iStep++;
		for (int i = 0; i < iStep; i ++)
		{
			int Block_Height = iBlockH;
			int Block_Width = m_InputDem.GetDemHeader().iCol;
			float *pfOriBlock_Z = new float[Block_Height*Block_Width];
			memset(pfOriBlock_Z, 0, sizeof(float)*Block_Height*Block_Width);
			float *pfNewBlock_Z = new float[Block_Height*Block_Width];
			memset(pfNewBlock_Z, 0, sizeof(float)*Block_Height*Block_Width);
			int iSartRow = i*iBlockH;
			if (iSartRow >= m_InputDem.GetDemHeader().iCol) { Block_Height = m_InputDem.GetDemHeader().iRow - (i - 1)*iBlockH; }
			m_InputDem.ReadBlock(pfOriBlock_Z, 0, iSartRow, Block_Width, Block_Height);
			float fZ1 = NODATA, fZ2 = NODATA;
			for (int i_Block = 0; i_Block < Block_Height; i_Block++)
			{
				for (int j_Block = 0; j_Block < Block_Width; j_Block++)
				{
					fZ1 = *(pfOriBlock_Z + i_Block*Block_Width + j_Block);
					if (fZ1 != m_DemNoDataValue){
						*(pfNewBlock_Z + i_Block*Block_Width + j_Block) = fZ1;
					}
					else
						*(pfNewBlock_Z + i_Block*Block_Width + j_Block) = (float)m_ChangeValue;
				}
			}
			outDem.WriteBlock(pfNewBlock_Z, 0, iSartRow, Block_Width, Block_Height);
			delete[]pfOriBlock_Z;
			delete[]pfNewBlock_Z;
		}
		outDem.SetDemNoDataValue(m_ChangeValue);
		outDem.SetDemProj(m_InputDem.GetDemProj());
		AfxGetApp()->EndWaitCursor();
	}
}
