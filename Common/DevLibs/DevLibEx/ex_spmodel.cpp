//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __EX_SPMODEL_CPP
#include "ex_headers.h"

#include "..\\devlib\\bbbase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
//
//
//===========================================================================

MSpModelDefBaseEx::MSpModelDefBaseEx(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR Desc, unsigned long Flgs, LPCTSTR DLL) : \
MSpModelDefBase(pClass, pSubClass, ShortDesc, Desc, Flgs, DLL) 
  {
  m_pClassDef->dwCategory = m_pClassDef->dwCategory &~TOC_SMDKRUNTIME; //Do we want to force this off!?
  };

MSpModelDefBaseEx::~MSpModelDefBaseEx()
  {
  };


//===========================================================================
//
//
//
//===========================================================================
