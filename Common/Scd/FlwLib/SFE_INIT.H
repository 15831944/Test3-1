//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

//===========================================================================

// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SFE_INIT_H
#define  __SFE_INIT_H

#ifndef __AFXTEMPL_H__
  #include <afxtempl.h> // "xafxtempl.h"
#endif
#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __TKNPARSE_H
  #include "tknparse.h"
#endif

#ifdef __SFE_INIT_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport SfeInitClass
  {
  private:
    //static CList <SfeInitClass *, SfeInitClass *> List;
    static SfeInitClass *pFirst;
    SfeInitClass *pNext;

  public:
    SfeInitClass();
    virtual ~SfeInitClass();

    virtual flag   Parse(Strng & What, Strng & CurTkn, CTokenFile &Tkns){ return false; }; // Only Old Qualities
    virtual flag   LoadConfiguration(CProfINIFile & CfgPF)=0;
    virtual flag   Initialise()=0;
    virtual flag   Terminate()=0;
    static SfeInitClass*  First() { return pFirst; };
    SfeInitClass*  Next() { return pNext; };
  };


// ==========================================================================
//
//
//
// ==========================================================================

#undef DllImportExport

#endif



