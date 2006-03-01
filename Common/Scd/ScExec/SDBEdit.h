//{{AFX_INCLUDES()
#include "ssdw3b32.h"
#include "ssdbgrid.h"
//#include "mschart.h"
//}}AFX_INCLUDES
#if !defined(AFX_SDBEDIT_H__EDB3BF3C_FD03_11D2_BDFE_00A0C9ED9512__INCLUDED_)
#define AFX_SDBEDIT_H__EDB3BF3C_FD03_11D2_BDFE_00A0C9ED9512__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SDBEdit.h : header file
//
//#include "resource.h"
#include "sc_defs.h"

#ifdef __SDBEDIT_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

class DllImportExport CSCDDatabase
  {
  public:
    static flag DoSDBEdit(const char* DBFn);
    static flag DoSDBCreate(const char* DBFn);
//    static BOOL UpgradeCSVDB(LPCTSTR CSVPath, CUpGrdCompletion &UGC);
  };


#undef DllImportExport

#endif // !defined(AFX_SDBEDIT_H__EDB3BF3C_FD03_11D2_BDFE_00A0C9ED9512__INCLUDED_)

