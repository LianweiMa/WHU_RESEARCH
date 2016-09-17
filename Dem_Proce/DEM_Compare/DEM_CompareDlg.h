// DEM_CompareDlg.h : header file
//

#if !defined(AFX_DEM_COMPAREDLG_H__11240D7E_E1A0_4314_AA4F_3852D80566EA__INCLUDED_)
#define AFX_DEM_COMPAREDLG_H__11240D7E_E1A0_4314_AA4F_3852D80566EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDEM_CompareDlg dialog

class CDEM_CompareDlg : public CDialog
{
// Construction
public:
	CDEM_CompareDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDEM_CompareDlg)
	enum { IDD = IDD_DEM_COMPARE_DIALOG };
	CString	m_CmpPath;
	CString	m_StdPath;
	CString	m_RetPath;
	float	m_Max;
	float	m_Mean;
	float	m_Min;
	float	m_Mse;
	int		m_CmpCol;
	int		m_CmpRow;
	double	m_CmpStaX;
	double	m_CmpStaY;
	double	m_MaxX;
	double	m_MaxY;
	double	m_MinY;
	int		m_StdCol;
	int		m_StdRow;
	double	m_StdStaX;
	double	m_StdStaY;
	double	m_MinX;
	double	m_StdDx;
	double	m_CmpDx;
	double	m_CmpDy;
	double	m_StdDy;

	int     m_MixStaRow;
	int     m_MixEndRow;
	int     m_MixStaCol;
	int     m_MixEndCol;

	long	m_GridNum;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDEM_CompareDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDEM_CompareDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonStddem();
	afx_msg void OnButtonCmpdem();
	afx_msg void OnButtonOk();
	afx_msg void OnButtonGetall();
	afx_msg void OnButtonMax();
	afx_msg void OnButtonMean();
	afx_msg void OnButtonMin();
	afx_msg void OnButtonMse();
	afx_msg void OnButtonGetinf();
	afx_msg void OnButtonGetreg();
	afx_msg void OnButtonGetstd();
	afx_msg void OnButtonGetcmp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEM_COMPAREDLG_H__11240D7E_E1A0_4314_AA4F_3852D80566EA__INCLUDED_)
