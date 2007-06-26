//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#pragma once

#ifndef  __DEVLIBEX_H
#define  __DEVLIBEX_H

#ifdef __DEVLIBEX_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(DEVLIBEX)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//===========================================================================

//.....

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
