//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __SLVSTATE_H
#define __SLVSTATE_H
//
#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __EXECUTIV_H
  #include "executiv.h"
#endif
#ifndef __SLVTOOL_H
  #include "slvtool.h"
#endif
#ifndef __GPWFUNCS_H
  #include "gpwfuncs.h"
#endif
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CSolveState dialog

class CSolveState : public CDialog
  {
  public:
    CSolveTool* pSolveTool;

    CSolveState(CSolveTool * SolveTool, CWnd* pParent = NULL);   // standard constructor
    ~CSolveState();
    void ToggleWindowWidth(flag RePaint=True);

    //{{AFX_DATA(CSolveState)
    enum { IDD = IDD_SOLVESTATE };
    CCustomListCtrl	m_ErrList;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CSolveState)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CSolveState)
    virtual BOOL OnInitDialog();
    afx_msg void OnSlvAdv();
    afx_msg void OnSlvPinned();
    afx_msg void OnDblclkWorsterrs(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRclickWorsterrs(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    afx_msg LRESULT OnUpdateDataByMsg(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

    //EqnSlvCtrlBlk & SlvCB() { return gs_Exec.SlvCB.EqnCB; };//l->EqnCB; };

    CBitmap PinnedUpBitmap;
    CBitmap PinnedDownBitmap;
    static CSolveState* pState;
    int    WndRightShift;
    byte   iExtraInfoStyle;
  public:
    //void DoByMsg(WMU_PB_UPDATE);
    static void Open(CSolveTool * SolveTool);
    static void Close();
    static void Show();
    static void Hide(int ForceHide);
    static void Update();

    virtual void OnCancel();
  
  protected:  
    int GetHitTag(CCustomListCtrl& Ctrl, CString& sTag);
    BOOL AccessableTag(CString& sTag);
    void PopupMenuForTag(CString& sTag, int x, int y);
  };

#endif
