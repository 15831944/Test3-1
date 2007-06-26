//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#pragma once

#ifndef  __DEVLIB1_H
#define  __DEVLIB1_H

#ifdef __DEVLIB1_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(DEVLIB1)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//===========================================================================

//.....

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
