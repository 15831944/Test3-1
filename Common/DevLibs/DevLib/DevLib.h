//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#pragma once

#ifndef  __DEVLIB_H
#define  __DEVLIB_H

#ifdef __DEVLIB_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(DEVLIB)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//===========================================================================

//.....

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
