
// CrdShift.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CCrdShiftApp:
// See CrdShift.cpp for the implementation of this class
//

class CCrdShiftApp : public CWinApp
{
public:
	CCrdShiftApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CCrdShiftApp theApp;