
// DEM_CompareDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DEM_Compare.h"
#include "DEM_CompareDlg.h"
#include "afxdialogex.h"
#include "ResearchCode/FileOP.hpp"
#include "ResearchCode/CommonOP.hpp"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDEM_CompareDlg dialog



CDEM_CompareDlg::CDEM_CompareDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDEM_CompareDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//add
	m_Xmax = m_Xmin = m_Ymin = m_Ymax = NODATA;
	m_GsdX = m_GsdY = 0.0;
}

void CDEM_CompareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDEM_CompareDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BT_REFF, &CDEM_CompareDlg::OnBnClickedBtReff)
	ON_BN_CLICKED(IDC_BT_COMPARE, &CDEM_CompareDlg::OnBnClickedBtCompare)
	ON_BN_CLICKED(IDC_BT_COMPUTE_INTERSECTION, &CDEM_CompareDlg::OnBnClickedBtComputeIntersection)
	ON_BN_CLICKED(IDC_BT_COMPUTE_STATISTIC, &CDEM_CompareDlg::OnBnClickedBtComputeStatistic)
END_MESSAGE_MAP()


// CDEM_CompareDlg message handlers

BOOL CDEM_CompareDlg::OnInitDialog()
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

void CDEM_CompareDlg::OnPaint()
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
HCURSOR CDEM_CompareDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDEM_CompareDlg::OnBnClickedBtReff()
{
	// TODO: Add your control notification handler code here
	SetDlgItemText(IDC_STATIC_REFF, "");
	SetDlgItemText(IDC_STATIC_IN, "");
	SetDlgItemText(IDC_STATIC_STASTS, "");

	CFileDialog filedlg(TRUE);
	filedlg.m_ofn.lpstrTitle = "Open Reference DEM...";
	filedlg.m_ofn.lpstrFilter = "Text Files(*.tif)\0*.tif\0Text Files(*.dem)\0*.dem\0All Files(*.*)\0*.*\0\0";
	if (IDOK == filedlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT_REFF, filedlg.GetPathName());
		m_StringRefPath = filedlg.GetPathName();
	}

	if (!m_StringRefPath.IsEmpty())
	{
		GDALDEMHDR StdDemHdr;
		m_RefDem.LoadFile(m_StringRefPath.GetBuffer(0), &StdDemHdr);

		char strMessage[1024];
		sprintf_s(strMessage, "%s%d\t%s%f\t%s%lf\n%s%d\t%s%f\t%s%lf", 
			"行数：", StdDemHdr.iRow, "X方向格网间距：", StdDemHdr.lfGsdX, "起始点X坐标：", StdDemHdr.lfStartX,
			"列数：", StdDemHdr.iCol, "Y方向格网间距：", StdDemHdr.lfGsdY, "起始点Y坐标：", StdDemHdr.lfStartY );
		SetDlgItemText(IDC_STATIC_REFF, strMessage);
	}
	else
		MessageBox("请输入DEM文件！", "错误提示");
}


void CDEM_CompareDlg::OnBnClickedBtCompare()
{
	// TODO: Add your control notification handler code here
	SetDlgItemText(IDC_STATIC_COMPARE, "");
	SetDlgItemText(IDC_STATIC_IN, "");
	SetDlgItemText(IDC_STATIC_STASTS, "");
	CFileDialog filedlg(TRUE);
	filedlg.m_ofn.lpstrTitle = "Open Compared DEM...";
	filedlg.m_ofn.lpstrFilter = "Text Files(*.tif)\0*.tif\0Text Files(*.dem)\0*.dem\0All Files(*.*)\0*.*\0\0";
	if (IDOK == filedlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT_COMPARE, filedlg.GetPathName());
		m_StringCmpPath = filedlg.GetPathName();
	}

	if (!m_StringCmpPath.IsEmpty())
	{
		GDALDEMHDR CmpDemHdr;
		m_CmpDem.LoadFile(m_StringCmpPath.GetBuffer(0), &CmpDemHdr);

		char strMessage[1024];
		sprintf_s(strMessage, "%s%d\t%s%f\t%s%lf\n%s%d\t%s%f\t%s%lf",
			"行数：", CmpDemHdr.iRow, "X方向格网间距：", CmpDemHdr.lfGsdX, "起始点X坐标：", CmpDemHdr.lfStartX,
			"列数：", CmpDemHdr.iCol, "Y方向格网间距：", CmpDemHdr.lfGsdY, "起始点Y坐标：", CmpDemHdr.lfStartY);
		SetDlgItemText(IDC_STATIC_COMPARE, strMessage);

	}
	else
		MessageBox("请输入DEM文件！", "错误提示");
}


void CDEM_CompareDlg::OnBnClickedBtComputeIntersection()
{
	// TODO: Add your control notification handler code here
	SetDlgItemText(IDC_STATIC_IN, "");
	double lfGsdX_tmp = m_RefDem.GetDemHeader().lfGsdX;
	double lfGsdY_tmp = m_RefDem.GetDemHeader().lfGsdY;
	if (m_GsdX < lfGsdX_tmp)m_GsdX = lfGsdX_tmp;
	if (m_GsdY < lfGsdY_tmp)m_GsdY = lfGsdY_tmp;
	lfGsdX_tmp = m_CmpDem.GetDemHeader().lfGsdX;
	lfGsdY_tmp = m_CmpDem.GetDemHeader().lfGsdY;
	if (m_GsdX < lfGsdX_tmp)m_GsdX = lfGsdX_tmp;
	if (m_GsdY < lfGsdY_tmp)m_GsdY = lfGsdY_tmp;

	double lfX[4] = { NODATA }, lfY[4] = { NODATA };
	m_RefDem.GetDemRegion(lfX, lfY);
	if (m_Xmin == NODATA)m_Xmin = lfX[0];
	if (m_Xmax == NODATA)m_Xmax = lfX[1];
	if (m_Ymin == NODATA)m_Ymin = lfY[2];
	if (m_Ymax == NODATA)m_Ymax = lfY[1];
	for (int PeakValue_i = 0; PeakValue_i < 4; PeakValue_i++)
	{
		if (m_Xmin > lfX[PeakValue_i])m_Xmin = lfX[PeakValue_i];
		if (m_Xmax < lfX[PeakValue_i])m_Xmax = lfX[PeakValue_i];
		if (m_Ymin > lfY[PeakValue_i])m_Ymin = lfY[PeakValue_i];
		if (m_Ymax < lfY[PeakValue_i])m_Ymax = lfY[PeakValue_i];
	}
	m_CmpDem.GetDemRegion(lfX, lfY);
	for (int PeakValue_i = 0; PeakValue_i < 4; PeakValue_i++)
	{
		if (m_Xmin > lfX[PeakValue_i])m_Xmin = lfX[PeakValue_i];
		if (m_Xmax < lfX[PeakValue_i])m_Xmax = lfX[PeakValue_i];
		if (m_Ymin > lfY[PeakValue_i])m_Ymin = lfY[PeakValue_i];
		if (m_Ymax < lfY[PeakValue_i])m_Ymax = lfY[PeakValue_i];
	}
	m_Cols = int((m_Xmax - m_Xmin) / m_GsdX) + 1;
	m_Rows = int((m_Ymax - m_Ymin) / m_GsdY) + 1;

	char strMessage[1024];
	sprintf_s(strMessage, "%s%d\n%s%lf\t%lf\n%s%lf\t%lf",
		"格网点数：", m_Cols*m_Rows,
		"X坐标范围：", m_Xmin, m_Xmax,
		"Y坐标范围：", m_Ymin, m_Ymax);
	SetDlgItemText(IDC_STATIC_IN, strMessage);

}


void CDEM_CompareDlg::OnBnClickedBtComputeStatistic()
{
	// TODO: Add your control notification handler code here
	AfxGetApp()->BeginWaitCursor();
	SetDlgItemText(IDC_STATIC_STASTS, "");
	float fDiffSum = 0.0f, fDiffMeanValue = 0.0f, fDiffStdDev=0.0f;
	int iDiffNums = 0;
	CGdalDem demDiff;//diff dem
	GDALDEMHDR DemHdr_Tmp;
	DemHdr_Tmp.lfStartX = m_Xmin; DemHdr_Tmp.lfStartY = m_Ymin;
	DemHdr_Tmp.lfGsdX = m_GsdX; DemHdr_Tmp.lfGsdY = m_GsdY;
	DemHdr_Tmp.iCol = m_Cols;   DemHdr_Tmp.iRow = m_Rows;
	char strMessage[1024];
	char strDemDiff[FILE_PN], strRef[FILE_PN], strCmp[FILE_PN];
	strcpy(strDemDiff, m_StringCmpPath); strcpy(strRef, m_StringRefPath); strcpy(strCmp, m_StringCmpPath);
	char *pS = strrchr(strDemDiff, '\\'); if (!pS)pS = strrchr(strDemDiff, '/');
	Dos2Unix(strRef); Dos2Unix(strCmp);
	sprintf(pS, "\\%s_%s%s", strrchr(strRef, '/') + 1, strrchr(strCmp, '/') + 1, "_diff.tif");

	if (!demDiff.CreatFile(strDemDiff, &DemHdr_Tmp)){ sprintf_s(strMessage, "%s%s", "ERROR_FILE_OPEN:",strDemDiff); MessageBox(strMessage, "ERROR_FILE_OPEN"); }
	float *pfDiff_Z = new float[m_Cols*m_Rows];
	Memset(pfDiff_Z, NODATA, m_Cols*m_Rows);
	//overlay xStart,yStart
	double lfGeoX = m_Xmin, lfGeoY = m_Ymax;
	for (int i = 0; i < m_Rows; i++)
	{
		lfGeoX = m_Xmin;
		for (int j = 0; j < m_Cols; j++)
		{		
			float fZ1 = m_CmpDem.GetDemZValue(lfGeoX,lfGeoY), fZ2 = m_RefDem.GetDemZValue(lfGeoX,lfGeoY);
			if (fZ1 != NODATA && fZ2 != NODATA){
				*(pfDiff_Z + i*m_Cols + j) = fZ1 - fZ2;
				fDiffSum += (fZ1 - fZ2);
				iDiffNums++;
			}
			lfGeoX += m_GsdX;
		}
		lfGeoY -= m_GsdY;
	}
	demDiff.WriteBlock(pfDiff_Z, 0, 0, m_Cols, m_Rows);
	fDiffMeanValue = fDiffSum / iDiffNums;
	for (int i = 0; i < m_Rows; i++)
	{
		for (int j = 0; j < m_Cols; j++)
		{
			float fZ_Tmp = *(pfDiff_Z + i*m_Cols + j);
			if (fZ_Tmp != NODATA)
				fDiffStdDev += pow((fZ_Tmp - fDiffMeanValue), 2.0f);
		}
	}
	fDiffStdDev = pow(fDiffStdDev / iDiffNums, 0.5f);
	sprintf_s(strMessage, "%s%.3f\t%.3f\n%s%f\n%s%f",
		"范围：", demDiff.GetMinZ(), demDiff.GetMaxZ(),
		"均值：", fDiffMeanValue,
		"标准差：", fDiffStdDev);
	SetDlgItemText(IDC_STATIC_STASTS, strMessage);
	AfxGetApp()->EndWaitCursor();
}
