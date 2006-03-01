//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

//#include "scdtempl.h"

#ifndef __WIREPANEL_H
#define __WIREPANELD_H

#define DllImportExport

class DllImportExport CWirePanelWindow
  {
  public:
    static void Register();
    
    static int  Show(LPCTSTR pRootTag, flag DoBringToTop);
    static int Refresh();
    
    static CFormView * GetWnd();
    static CMDIChildWnd * GetFrame();
  };


#undef DllImportExport

#endif