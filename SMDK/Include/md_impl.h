//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#pragma once

#ifndef  __MD_IMPL_H
#define  __MD_IMPL_H

#ifndef __MD_BASE_H
#include "md_base.h"
#endif
#ifndef __MD_DEFN_H
#include "md_defn.h"
#endif
#ifndef __MD_VECTOR_H
#include "md_vector.h"
#endif

#include "flwnode.h"


#ifdef __MD_METHOD_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(DEVLIB)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

class MdlNode;

class DllImportExport MBaseMethodImpl
  {
  friend class MBaseMethod;
  friend class BlackBoxBase;
  //protected:
  public:
    //CString       sMyTag;
    DWORD m_dwMethOptions;
    
    MBaseMethodImpl()
      {
      m_dwMethOptions=0;
      }
  };

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
