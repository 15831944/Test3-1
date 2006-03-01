//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __DLGBUSY_H
#define __DLGBUSY_H

#ifdef __DLGBUSY_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

class DllImportExport CDlgBusy
  {
  public:
    static void Startup();
    static void Shutdown();
    static void Open(pchar Msg, flag CanIt=false);
    static void Close();
    static void SetLine(int x, pchar fmt, ...);
    static BOOL IsOpen();
    static BOOL IsCancelled();
    static BOOL ShowWindow(int CmdShow);
  };

//===========================================================================

#undef DllImportExport 

#endif
