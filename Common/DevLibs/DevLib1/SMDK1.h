//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#pragma once

#ifndef  __SMDK1_H
#define  __SMDK1_H

#ifdef __SMDK1_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDK1)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//===========================================================================

//.....

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
