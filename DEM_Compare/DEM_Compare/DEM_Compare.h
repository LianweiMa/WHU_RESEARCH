
// DEM_Compare.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDEM_CompareApp:
// See DEM_Compare.cpp for the implementation of this class
//

class CDEM_CompareApp : public CWinApp
{
public:
	CDEM_CompareApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDEM_CompareApp theApp;