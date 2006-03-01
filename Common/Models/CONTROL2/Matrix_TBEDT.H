//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef __Matrix_TBEDT_H
#define __Matrix_TBEDT_H

#ifndef __FIXEDIT_H
#include "fixedit.h"
#endif  
//#ifndef __GPWNDS_H
//#include "gpwnds.h"
//#endif  

#ifdef __Matrix_TBEDT_CPP
  #define DllImportExport DllExport
#elif !defined(Control2)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================

_FWDDEF(CMatrixTBEdt);
class DllImportExport CMatrixTBEdt : public FxdEdtBookRef
  {
  public :
    pCMatrixTB         pFn;
    CMatrixTBEdt(pFxdEdtView pView_, pCMatrixTB pFn);//, rStrng Tag, rStrng Name);
    virtual ~CMatrixTBEdt();

    virtual void StartBuild();
    virtual void Build();
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

  };

// ==========================================================================

#undef DllImportExport

#endif
