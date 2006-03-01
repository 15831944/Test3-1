//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __PGM_N_H
#define  __PGM_N_H

#ifndef __NOISE_H
#include "noise.h"
#endif  

#ifndef __PGM_E_H
#include "pgm_e.h"
#endif  

#ifdef __PGM_N_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

class GCNoise : public GCClassVar
  {
  public:
    GCNoise(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB, pvoid pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB, pvoid pSubClass);
  };

//===========================================================================

#undef DllImportExport

#endif
