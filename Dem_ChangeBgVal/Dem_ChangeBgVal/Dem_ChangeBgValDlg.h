
// Dem_ChangeBgValDlg.h : header file
//

#pragma once
#include "ResearchCode/CGdalDem.hpp"

// CDem_ChangeBgValDlg dialog
class CDem_ChangeBgValDlg : public CDialogEx
{
// Construction
public:
	CDem_ChangeBgValDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DEM_CHANGEBGVAL_DIALOG };

	CString m_InputFilePath;
	CGdalDem m_InputDem;

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
	afx_msg void OnBnClickedBtOpenFile();
	double m_DemNoDataValue;
	afx_msg void OnBnClickedBtChangeBgValue();
	double m_ChangeValue;
};
