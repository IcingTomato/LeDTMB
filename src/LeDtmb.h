
// LeDtmb.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "DtmbUsb.h"

// CLeDtmbApp:
// See LeDtmb.cpp for the implementation of this class
//

class CLeDtmbApp : public CWinApp
{
public:
	CLeDtmbApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CLeDtmbApp theApp;
extern DtmbUsb theUsb;