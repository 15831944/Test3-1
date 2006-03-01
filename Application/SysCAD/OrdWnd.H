//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

//#include "scdtempl.h"

#ifndef __ORDWND_H
#define __ORDWND_H

#define DllImportExport

class DllImportExport COrdWindow
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