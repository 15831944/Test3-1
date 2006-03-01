//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "scdtempl.h"

#ifndef __MSGWND_H
#define __MSGWND_H

#define DllImportExport

const int MWShowNotes  = 0;
const int MWShowWarns  = 1;
const int MWShowErrors = 2;

class DllImportExport CMsgWindow
  {
  public:
    static void Register();
    static void StartUp();
    static void ShutDown();
    static void ClearAll();

    static UINT GetShowCmd();
    static void SetShowCmd(UINT Cmd);
    static int  Show(flag DoBringToTop);
    static int  Hide();
    static int  Reduce();
    
    static int  DoModalDlg();
    static CFormView * GetWnd();
    static CMDIChildWnd * GetFrame();
    static void SetSplitPos(int SplitPos);
    static int GetSplitPos();

    static void SetMsgsIncludeFlag(flag On);
    static long NErrorMsgs();
    static long NWarningMsgs();
    static long NNoteMsgs();

  };


#undef DllImportExport

#endif