//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __GASSEP_H
#define  __GASSEP_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#ifdef __GASSEP_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SKOMDLS)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

#endif