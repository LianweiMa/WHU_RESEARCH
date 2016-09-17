
// Dem_ChangeBgVal.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDem_ChangeBgValApp:
// See Dem_ChangeBgVal.cpp for the implementation of this class
//

class CDem_ChangeBgValApp : public CWinApp
{
public:
	CDem_ChangeBgValApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDem_ChangeBgValApp theApp;