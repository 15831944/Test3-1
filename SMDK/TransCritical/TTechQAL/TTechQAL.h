//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//   Time-stamp: <2006-11-01 11:56:22 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#ifndef  __TTECHQAL_H
#define  __TTECHQAL_H

#ifdef __TTECHQAL_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_TTechQAL();

//===========================================================================

//.....

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
