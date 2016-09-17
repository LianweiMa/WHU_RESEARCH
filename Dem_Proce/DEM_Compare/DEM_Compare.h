// DEM_Compare.h : main header file for the DEM_COMPARE application
//

#if !defined(AFX_DEM_COMPARE_H__ABD4EE71_3231_4F31_A84E_32318A89D2F2__INCLUDED_)
#define AFX_DEM_COMPARE_H__ABD4EE71_3231_4F31_A84E_32318A89D2F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDEM_CompareApp:
// See DEM_Compare.cpp for the implementation of this class
//

class CDEM_CompareApp : public CWinApp
{
public:
	CDEM_CompareApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDEM_CompareApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDEM_CompareApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEM_COMPARE_H__ABD4EE71_3231_4F31_A84E_32318A89D2F2__INCLUDED_)
