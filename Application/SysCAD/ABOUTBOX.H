//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __ABOUTBOX_H
#define __ABOUTBOX_H

#include "sc_defs.h"
#include "resource.h"

//#ifdef __ABOUTBOX_CPP
//  #define DllImportExport DllExport
//#elif !defined(SCAPP1)
//  #define DllImportExport DllImport
//#else
  #define DllImportExport
//#endif

//===========================================================================

class DllImportExport CAboutBox : public CDialog
  {
  public:
    CAboutBox(CWnd* pParent = NULL);    // standard constructor
    //{{AFX_DATA(CAboutBox)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA
  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //CBigIcon m_icon; // self-draw button
    //{{AFX_MSG(CAboutBox)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };


//void PASCAL CenterWindow(CWnd* pWnd);   // center window relative to parent

//===========================================================================

#undef DllImportExport
#endif
