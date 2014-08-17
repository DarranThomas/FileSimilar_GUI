// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <afxdisp.h>        // MFC Automation classes

///////////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <math.h>

///////////////////////////////////////////////////////////////////////////////////

#include <utility>
#include <map>
#include <vector>
#include <set>

///////////////////////////////////////////////////////////////////////////////////

#include <emmintrin.h>

///////////////////////////////////////////////////////////////////////////////////

#include <windows.h>

///////////////////////////////////////////////////////////////////////////////////

#include <Win32/macro.h>
#include <Win32/CMemory.h>
#include <Win32/THandle.h>
#include <Win32/CSynchronizationObject.h>
#include <Win32/CThread.h>
#include <Win32/CProcess.h>
#include <Win32/CFile.h>
#include <Win32/CDiskFile.h>
#include <Win32/CFileMapping.h>

///////////////////////////////////////////////////////////////////////////////////

#define MAX_NUMBER_OF_CPU_UTILIZED_SIMULTANEOUSLY	16

///////////////////////////////////////////////////////////////////////////////////

// Following are preprocessor switches which turns on/off optional features.
// Replace appropriate '#if 0' to '#if 1' then rebuild to enable specific feature;
// replace '#if 1' to '#if 0' then rebuild to disable specific feature.

///////////////////////////////////////////////////////////////////////////////////

#if 0
// Use SSE2-optimized code.
//
// Note. Currently it does not works as expected.

#define USE_SSE2
#endif

///////////////////////////////////////////////////////////////////////////////////

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#pragma warning(disable:4503)

