//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

//#include "scdtempl.h"

#ifndef __STATSWND_H
#define __STATSWND_H

#define DllImportExport

class DllImportExport CStatsWindow
  {
  public:
    static void Register();
    
    static int  Show(flag DoBringToTop);
    static int Refresh();
    
    static CFormView * GetWnd();
    static CMDIChildWnd * GetFrame();
  };


#undef DllImportExport

#endif