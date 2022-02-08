
// sample04_simple_mfc_application.h : main header file for the sample04_simple_mfc_application application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CSimpleApp:
// See sample04_simple_mfc_application.cpp for the implementation of this class
//

class CSimpleApp : public CWinApp
{
public:
	CSimpleApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSimpleApp theApp;
