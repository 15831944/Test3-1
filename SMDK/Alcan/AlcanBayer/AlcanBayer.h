//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ALCANBAYER_H
#define  __ALCANBAYER_H

#ifdef __ALCANSPUSR_CPP
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
