//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#pragma once

#ifndef  __MD_BASE_H
#define  __MD_BASE_H

#ifdef __MD_BASE_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(DEVLIB)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

#undef DllImportExport

#endif
