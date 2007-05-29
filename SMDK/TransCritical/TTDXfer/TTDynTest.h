//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//   Time-stamp: <2007-05-28 03:23:21 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#ifndef  __TTDYNTEST_H
#define  __TTDYNTEST_H

#ifdef __TTDYNTEST_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_TTDynTest();

//===========================================================================

//.....

//---------------------------------------------------------------------------
#undef DllImportExport

#endif // #ifndef __TTDYNTEST_H
