
// CrdShiftDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CrdShift.h"
#include "CrdShiftDlg.h"
#include "afxdialogex.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCrdShiftDlg dialog



CCrdShiftDlg::CCrdShiftDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCrdShiftDlg::IDD, pParent)
	, m_Shift_X(0)
	, m_Shift_Y(0)
	, m_Shift_Z(0)
	, m_Rotation_X(0)
	, m_Rotation_Y(0)
	, m_Rotation_Z(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCrdShiftDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SHIFT_X, m_Shift_X);
	DDX_Text(pDX, IDC_EDIT_SHIFT_Y, m_Shift_Y);
	DDX_Text(pDX, IDC_EDIT_SHIFT_Z, m_Shift_Z);
	DDX_Text(pDX, IDC_EDIT_ROTATION_X, m_Rotation_X);
	DDX_Text(pDX, IDC_EDIT_ROTATION_Y, m_Rotation_Y);
	DDX_Text(pDX, IDC_EDIT_ROTATION_Z, m_Rotation_Z);
}

BEGIN_MESSAGE_MAP(CCrdShiftDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BT_OPEN_FILE, &CCrdShiftDlg::OnBnClickedBtOpenFile)
	ON_BN_CLICKED(IDC_BT_SAVE_FILE, &CCrdShiftDlg::OnBnClickedBtSaveFile)
END_MESSAGE_MAP()


// CCrdShiftDlg message handlers

BOOL CCrdShiftDlg::OnInitDialog()
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

void CCrdShiftDlg::OnPaint()
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
HCURSOR CCrdShiftDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCrdShiftDlg::OnBnClickedBtOpenFile()
{
	// TODO: Add your control notification handler code here
	CFileDialog filedlg(TRUE);
	filedlg.m_ofn.lpstrTitle = "Open Reference DEM...";
	filedlg.m_ofn.lpstrFilter = "Text Files(*.tif)\0*.tif\0Text Files(*.dem)\0*.dem\0All Files(*.*)\0*.*\0\0";
	if (IDOK == filedlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT_OPEN_FILE, filedlg.GetPathName());
		m_InputFilePath = filedlg.GetPathName();
	}

	if (!m_InputFilePath.IsEmpty())
	{
		if (m_InputDem.Open(m_InputFilePath)){
			m_StringMessage = "Open file successful:"+m_InputFilePath;
			SetDlgItemText(IDC_STATIC_MESSAGE, m_StringMessage);
		}
		else{
			m_StringMessage = "Open file failed:" + m_InputFilePath;
			SetDlgItemText(IDC_STATIC_MESSAGE, m_StringMessage);
			return;
		}
	}
	else
		MessageBox("请输入DEM文件！", "错误提示");
}


void CCrdShiftDlg::OnBnClickedBtSaveFile()
{
	// TODO: Add your control notification handler code here
	CFileDialog filedlg(TRUE);
	filedlg.m_ofn.lpstrTitle = "Save DEM...";
	filedlg.m_ofn.lpstrFilter = "Text Files(*.tif)\0*.tif\0Text Files(*.dem)\0*.dem\0All Files(*.*)\0*.*\0\0";
	if (IDOK == filedlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT_SAVE_FILE, filedlg.GetPathName());
		m_OutputFilePath = filedlg.GetPathName();
	}

	if (!m_OutputFilePath.IsEmpty())
	{
		CDem OutputDem;
		DEMHDR DemHdr;
		UpdateData(TRUE);
		DemHdr = m_InputDem.GetDemHeader();
		DemHdr.intervalX += m_Shift_X;
		DemHdr.intervalY += m_Shift_Y;
		if (OutputDem.Open(m_OutputFilePath, CDem::modeWrite, &DemHdr)){
			for (int i = 0; i < DemHdr.row; i += BLOCK_HEIGHT_MLW)
			{
				for (int j = 0; j < DemHdr.column; j += BLOCK_WIDTH_MLW)
				{
					int Block_Height = 0, Block_Width = 0;
					if (i + BLOCK_HEIGHT_MLW <= DemHdr.row)
					{
						if (j + BLOCK_WIDTH_MLW <= DemHdr.column)
						{
							Block_Height = BLOCK_HEIGHT_MLW;
							Block_Width = BLOCK_WIDTH_MLW;
						}
						else
						{
							Block_Height = BLOCK_HEIGHT_MLW;
							Block_Width = (DemHdr.column - j);
						}
					}
					else
					{
						if (j + BLOCK_WIDTH_MLW <= DemHdr.column)
						{
							Block_Height = (DemHdr.row - i);
							Block_Width = BLOCK_WIDTH_MLW;
						}
						else
						{
							Block_Height = (DemHdr.row - i);
							Block_Width = (DemHdr.column - j);
						}
					}
					float *pfShift_Z = new float[Block_Height*Block_Width];
					memset(pfShift_Z, 0, sizeof(float)*Block_Height*Block_Width);
					OutputDem.ReadBlock(pfShift_Z, j, i, Block_Width, Block_Height);

					float fZ_Tmp = NOVALUE_MLW;
					for (int i_Block = 0; i_Block < Block_Height; i_Block++)
					{
						for (int j_Block = 0; j_Block < Block_Width; j_Block++)
						{
							fZ_Tmp = m_InputDem.GetDemZValue(j + j_Block, i + i_Block);
							if (fZ_Tmp != NOVALUE_MLW){ *(pfShift_Z + i_Block*Block_Width + j_Block) = fZ_Tmp+m_Shift_Z; }
							else
								*(pfShift_Z + i_Block*Block_Width + j_Block) = NOVALUE_MLW;
						}
					}
					OutputDem.WriteBlock(pfShift_Z, j, i, Block_Width, Block_Height);
					delete[]pfShift_Z;
				}
			}
			m_StringMessage = "Save file successful:" + m_OutputFilePath;
			SetDlgItemText(IDC_STATIC_MESSAGE, m_StringMessage);

		}
		else{
			m_StringMessage = "Open file failed:" + m_OutputFilePath;
			SetDlgItemText(IDC_STATIC_MESSAGE, m_StringMessage);
			return;
		}
	}
	else
		MessageBox("请输入DEM文件！", "错误提示");

}
