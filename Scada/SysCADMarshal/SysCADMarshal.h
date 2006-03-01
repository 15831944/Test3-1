// SysCADMarshal.h : main header file for the SYSCADMARSHAL application
//

#if !defined(AFX_SYSCADMARSHAL_H__D11C371F_634C_4BF1_BAAC_FDF6EBB7D795__INCLUDED_)
#define AFX_SYSCADMARSHAL_H__D11C371F_634C_4BF1_BAAC_FDF6EBB7D795__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "SysCADMarshal_i.h"

/////////////////////////////////////////////////////////////////////////////
// CSysCADMarshalApp:
// See SysCADMarshal.cpp for the implementation of this class
//

class CSysCADMarshalApp : public CWinApp
  {
  public:
    CSysCADMarshalApp();

  // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSysCADMarshalApp)
    public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    virtual int Run();
    //}}AFX_VIRTUAL

  // Implementation
    COleTemplateServer m_server;
      // Server object for document creation
    //{{AFX_MSG(CSysCADMarshalApp)
    afx_msg void OnAppAbout();
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG

    afx_msg LRESULT OnAddError(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnOpenDoc(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCloseDoc(WPARAM wParam, LPARAM lParam);

//    afx_msg LRESULT OnUpdateAll(WPARAM wParam, LPARAM lParam);
//    afx_msg LRESULT OnUpdateTagValues(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnSetSlotCfg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSetLinkCfg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSetCdBlkCfg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSetDeviceCfg(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnUpdateOneSlot(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateOneLink(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateOneSlotState(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateOneLinkState(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateStatus(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnXferChangeBlock(WPARAM wParam, LPARAM lParam);
//    afx_msg LRESULT OnSetSelection(WPARAM wParam, LPARAM lParam);

    CSemaphore  m_OpenCloseSema;
    
    DECLARE_MESSAGE_MAP()

  private:
    BOOL m_bATLInited;
  private:
    BOOL InitATL();
  public:
  };

extern CSysCADMarshalApp theApp;

class COpenCloseWait : CSingleLock
  {
  public:
    COpenCloseWait() :
      CSingleLock(&theApp.m_OpenCloseSema, true) 
      {};
  };


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSCADMARSHAL_H__D11C371F_634C_4BF1_BAAC_FDF6EBB7D795__INCLUDED_)
