//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//   Time-stamp: <2007-05-24 07:08:09 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#ifndef  __TTDX_H
#define  __TTDX_H

#ifdef __TTDX_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_TTDX();

//===========================================================================

//.....

//---------------------------------------------------------------------------
#undef DllImportExport

#endif // #ifndef __TTDX_H
