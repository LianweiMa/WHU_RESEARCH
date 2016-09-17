
// CrdShiftDlg.h : header file
//

#pragma once
#include "mlw_1_0_0/dem.hpp"

// CCrdShiftDlg dialog
class CCrdShiftDlg : public CDialogEx
{
// Construction
public:
	CCrdShiftDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CRDSHIFT_DIALOG };
	CString m_StringMessage;
	CString m_InputFilePath,m_OutputFilePath;
	CDem m_InputDem;

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
	afx_msg void OnBnClickedBtSaveFile();
	double m_Shift_X;
	double m_Shift_Y;
	double m_Shift_Z;
	double m_Rotation_X;
	double m_Rotation_Y;
	double m_Rotation_Z;
};
