//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __TTMAD_BAYER_H
#define  __TTMAD_BAYER_H

#ifdef __TTMAD_BAYER_CPP
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
