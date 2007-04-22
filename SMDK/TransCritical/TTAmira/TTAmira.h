//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __TTAMIRA_H
#define  __TTAMIRA_H

#ifdef __TTAMIRA_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//===========================================================================

//...

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
