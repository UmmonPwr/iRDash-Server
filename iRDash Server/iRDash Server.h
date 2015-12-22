
// iRDash Server.h : main header file for the iRDash application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CiRDashServerApp:
// See iRDash Server.cpp for the implementation of this class
//

class CiRDashServerApp : public CWinApp
{
public:
	CiRDashServerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CiRDashServerApp theApp;