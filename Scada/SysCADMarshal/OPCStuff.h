#ifndef OPCSTUFF_H
#define OPCSTUFF_H

//#define VC_EXTRALEAN    // Exclude rarely-used stuff from Windows headers
//#define _WIN32_DCOM     // This includes new DCOM calls
#include <afxwin.h>     // MFC core and standard components
#include <afxext.h>     // MFC extensions
#include <afxdisp.h>    // MFC OLE automation classes
#include <afxcmn.h>     // MFC support for Windows Common Controls

//#include "xafxtempl.h"       // MFC template collections
//#include <afxcview.h>
#include <atlbase.h>    // ATL conversion functions

//#include "scdver.h"         // OPC on / off
#include "OPCWrap.h"    // wrapper classes around the OPC interface
const int MAX_KEYLEN = 256;

// The following does not work beause the m_Lcl memebrs get overwritten
#define SEPARATETHREADSELECT  0

#define DATATIMEFORMAT  // define this to use OPCSTMFORMATDATATIME

// otherwise, uses OPCSTMFORMATDATA
extern UINT OPCSTMFORMATDATA;
extern UINT OPCSTMFORMATDATATIME;
extern UINT OPCSTMFORMATWRITECOMPLETE;

#define WM_OPCDATA      WM_USER + 50
#define WM_OPCWRITE     WM_USER + 51
#define WM_OPCSHUTDOWN  WM_USER + 52

LPCTSTR     OPCQualityAsString(long q);
LPCTSTR     OPCQualityAsShortString(long q);
#endif
