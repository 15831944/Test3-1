// ScdCOMClassFactory.h: interface for the CScdCOMClassFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ScdCOMClassFactory_H__EFD1884B_FE2F_4CA8_9597_F261B4AC8152__INCLUDED_)
#define AFX_ScdCOMClassFactory_H__EFD1884B_FE2F_4CA8_9597_F261B4AC8152__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _AFXTEMPL_H__
  #include <afxtempl.h> // "xafxtempl.h"
#endif
#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#include <stdio.h>
#include "sc_defs.h"
#include "gpfuncs.h"
#include "errorlog.h"
#include "datacnvs.h"
#include "datatype.h"
#include "tagobj.h"

#ifdef __COMSTUFF_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

class DllImportExport CCOMObjManager : public CCOMObjManagerRoot
  {
  public :
    static bool AddClass(LPCTSTR Cls1, LPCTSTR ProgID);
#if 0
    virtual TaggedObject * Construct(LPCTSTR ProgID, LPCTSTR pRqdTag, TaggedObject *pAttach, TagObjAttachment eAttach);
#endif
    virtual TaggedObject * Construct(TagObjClass * pClass, LPCTSTR pRqdTag, TaggedObject *pAttach, TagObjAttachment eAttach);
    virtual void Destroy(TaggedObject * pObj);
  };


#undef DllImportExport

#endif // !defined(AFX_ScdCOMClassFactory_H__EFD1884B_FE2F_4CA8_9597_F261B4AC8152__INCLUDED_)
