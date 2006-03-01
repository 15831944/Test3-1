//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __GPWFUNCS_H
#define  __GPWFUNCS_H

typedef word           Cursor_Types;

#ifndef __SCDCTRLS_H
#include "scdctrls.h"
#endif 

//===========================================================================

extern void GPWFuncs_Entry();

//===========================================================================

inline BOOL SendOrPostMessage(CWnd* pWnd, BOOL DoSend, UINT message, WPARAM wParam = 0, LPARAM lParam = 0 )
  {
  if (DoSend)
    return pWnd->SendMessage(message, wParam, lParam);
  return pWnd->PostMessage(message, wParam, lParam);
  }

//===========================================================================

class CCustomToolTipCtrl : public CToolTipCtrl
  {
  public:
    CCustomToolTipCtrl(CWnd* pParentWnd);
    virtual ~CCustomToolTipCtrl() { };
    BOOL OnToolTipNotify(NMHDR* pNMHDR, UINT ParentID);
    //{{AFX_VIRTUAL(CCustomToolTipCtrl)
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CCustomToolTipCtrl)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CCustomPropertyPage : public CPropertyPage
  {
  DECLARE_DYNCREATE(CCustomPropertyPage)
  public:
    CCustomPropertyPage(UINT nIDTemplate, UINT nIDCaption = 0);
    CCustomPropertyPage() { ASSERT(FALSE); };
    virtual ~CCustomPropertyPage();

    BOOL     bDidInit;
    BOOL     bActive;
    BOOL Inited() { return bDidInit; };
    BOOL Active() { return bActive; };

    void OnDestroy() { bDidInit = 0; CPropertyPage::OnDestroy(); };
    virtual BOOL OnSetActive() { bActive = CPropertyPage::OnSetActive(); return bActive; };
    virtual BOOL OnKillActive() { bActive = 0; return CPropertyPage::OnKillActive(); };

    //{{AFX_VIRTUAL(CCustomPropertyPage)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL
  protected:
    UINT iDialogID;
    CCustomToolTipCtrl* pTTC;
    //{{AFX_MSG(CCustomPropertyPage)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

extern int StatusBarDriverIndicator;
extern int StatusBarCmdIndicator;
const int MaxStatusBarMsgs = 16;

class CCustomStatusBar : public CStatusBar
  {
  public:
    CCustomStatusBar();
    virtual ~CCustomStatusBar();
    void UseLocks();
    void SetNoOfIndicators(int NoOfIndicators);
    void UpdateIndicator(int index, char* pTxt, BOOL bUpdate=TRUE);
    void SetMsg(char* fmt, ...);       //place temporary message on status bar
    void SetMsg1(char* pStatusBarMsg); //place temporary message on status bar

  protected:
    CString Msgs[MaxStatusBarMsgs];   //array of status bar messages
    int iMsgPos;                      //top message in array
    int iNoOfIndicators;              //number of panes or indicators
    CStringArray Indicators;          //array of indicator strings
    BOOL bUseLocks;                   //must critical section locks be used
    CRITICAL_SECTION LockSection;     //used for locks

    inline void Lock() { if (bUseLocks) EnterCriticalSection(&LockSection); };
    inline void Unlock() { if (bUseLocks) LeaveCriticalSection(&LockSection); };
    //{{AFX_MSG(CCustomStatusBar)
    //}}AFX_MSG
    afx_msg void OnUpdateInd(CCmdUI* pCmdUI);
    afx_msg LRESULT OnUpdateIndicator(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateStatusBar(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDoUpdateStatusBar(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
  };

extern CCustomStatusBar* pStatusBar; //global pointer to the application status bar

//===========================================================================
//status bar message types
const WORD SBM_POSTMSG  = 0x01;
const WORD SBM_ADD      = 0x02;
const WORD SBM_UPDATE   = 0x04;
const WORD SBM_REMOVE   = 0x08;

class CStatusBarMsg
  {
  public:
    CStatusBarMsg(char* pStatusBarMsg);
    virtual ~CStatusBarMsg();
    void UpdateMsg(char* fmt, ...);
    void UpdateMsg1(char* pStatusBarMsg);
  };

//===========================================================================

class CWaitMsgCursor : public CWaitCursor
  {
  public:
    CWaitMsgCursor(char* pStatusBarMsg);
    virtual ~CWaitMsgCursor();
    void UpdateMsg(char* fmt, ...);
    void UpdateMsg1(char* pStatusBarMsg) { StatusBarMsg.UpdateMsg1(pStatusBarMsg); };
  protected:
    CStatusBarMsg StatusBarMsg;
  };

//===========================================================================
#endif


