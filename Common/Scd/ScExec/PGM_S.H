//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __PGM_S_H
#define  __PGM_S_H

#include "noise.h"
#include "pgm_e.h"

#ifdef __PGM_S_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

class GCSDB : public GCClassVar
  {
  public:
    GCSDB(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB, pvoid pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB, pvoid pSubClass);
  };

//===========================================================================

#undef DllImportExport

#endif
