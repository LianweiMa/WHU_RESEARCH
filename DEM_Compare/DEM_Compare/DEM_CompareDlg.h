
// DEM_CompareDlg.h : header file
//

#pragma once
#include "ResearchCode/CGdalDem.hpp"


// CDEM_CompareDlg dialog
class CDEM_CompareDlg : public CDialogEx
{
// Construction
public:
	CDEM_CompareDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DEM_COMPARE_DIALOG };
	CString m_StringRefPath, m_StringCmpPath;
	CGdalDem m_RefDem;
	CGdalDem m_CmpDem;
	int m_Cols, m_Rows;
	double m_GsdX, m_GsdY;
	double m_Xmin, m_Ymin, m_Xmax, m_Ymax;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtReff();
	afx_msg void OnBnClickedBtCompare();
	afx_msg void OnBnClickedBtComputeIntersection();
	afx_msg void OnBnClickedBtComputeStatistic();
};
