//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __OLEDLGS_H
#define __OLEDLGS_H

class COleInfoWnd : public CDialog
  {
  public:
    COleInfoWnd(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(COleInfoWnd)
    enum { IDD = IDD_OLEINFOWND };
    //}}AFX_DATA
    //{{AFX_VIRTUAL(COleInfoWnd)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    int m_Count;
    CString m_Desc;
    CString m_Name;
    //{{AFX_MSG(COleInfoWnd)
    //}}AFX_MSG
    afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================
#endif
