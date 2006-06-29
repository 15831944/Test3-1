//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  TS_SMDK_H_
#define  TS_SMDK_H_

#ifdef __TS_SMDK_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(UserMethods)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_UserMethods();

//===========================================================================

#undef DllImportExport

#endif // TS_MODELLIB2_H_BBE18A9C_4645_4D84_92F2_1D533EF49149
