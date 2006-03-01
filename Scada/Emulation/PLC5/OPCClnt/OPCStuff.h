
//#define VC_EXTRALEAN    // Exclude rarely-used stuff from Windows headers
//#define _WIN32_DCOM     // This includes new DCOM calls

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#include <afxcmn.h>         // MFC support for Windows Common Controls
#include <afxtempl.h>       // MFC template collections
#include <afxcview.h>
#include <atlbase.h>        // ATL conversion functions

#include "OPCWrap.h"    // wrapper classes around the OPC interface

const int MAX_KEYLEN = 256;

#define DATATIMEFORMAT  // define this to use OPCSTMFORMATDATATIME
                        // otherwise, uses OPCSTMFORMATDATA

extern UINT OPCSTMFORMATDATA;
extern UINT OPCSTMFORMATDATATIME;
extern UINT OPCSTMFORMATWRITECOMPLETE;

#define     WM_OPCDATA     WM_USER + 50
#define     WM_OPCWRITE    WM_USER + 51
#define     WM_OPCSHUTDOWN WM_USER + 52

void VariantToString(VARIANT value, CString& strText);