//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"

#include "sc_defs.h"
#include "datacnvs.h"

#define  __SFE_INIT_CPP
#include "sfe_init.h"


//===========================================================================
//
//
//
//===========================================================================

SfeInitClass* SfeInitClass::pFirst=NULL;

SfeInitClass::SfeInitClass()
  {
  if (pFirst==NULL)
    {
    pFirst=this; 
    pNext=NULL;
    }
  else
    {
    SfeInitClass* p=pFirst;
    while (p->pNext)
      p=p->pNext; 
    p->pNext=this; 
    pNext=NULL; 
    }
  }

SfeInitClass::~SfeInitClass()
  {
  if (pFirst==this)
    pFirst=pNext;
  else
    {
    SfeInitClass* p=pFirst;
    while (p && (p->pNext!=this)) 
      p=p->pNext;
    ASSERT(p);
    p->pNext=pNext;
    }
  }

//===========================================================================
//
//
//
//===========================================================================
