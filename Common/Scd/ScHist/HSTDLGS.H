//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#if ForSysCAD

#include "resource.h"
#ifndef __SCDCTRLS_H
#include "scdctrls.h"
#endif

class CHstSlotDlg : public CDialog
  {
  public:
    CHstSlotDlg(CSlot* Slot, CWnd* pParent = NULL);
    //{{AFX_DATA(CHstSlotDlg)
    enum { IDD = IDD_HSTSLOTDLG };
    CString m_Tag;
    CString m_Desc;
    CString m_Type;
    CString m_Eng;
    BOOL m_RecordIt;
    int m_BoxcarAlg;
    double m_Decrease;
    double m_DeltaLo;
    double m_DeltaHi;
    double m_FltTau;
    double m_FltDelta;
    UINT m_NoNotRec;
    UINT m_NoRec;
    int m_FilterAlg;
    double m_FltPeriod;
    UINT m_WinFltCount;
    //}}AFX_DATA
    byte  bChanged:1,
          bCanChange:1;
    CSlot* pSlot;
    //{{AFX_VIRTUAL(CHstSlotDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    void Changed(flag HasChanged = True);
    void Enable();
    void Init();
    //{{AFX_MSG(CHstSlotDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnRecordit();
    //}}AFX_MSG
    afx_msg void OnRadio(UINT nID);
    afx_msg void OnChange(UINT nID);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CHstSlotInfoDlg : public CDialog
  {
  public:
    CHstSlotInfoDlg(char* pFirstTag, CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CHstSlotInfoDlg)
    enum { IDD = IDD_HSTSLOTINFODLG };
    CString m_Desc;
    CString m_Eng;
    CString m_Source;
    CString m_Type;
    CString m_Recording;
    double m_ChgFreq;
    double m_RecFreq;
    BOOL m_SelAll;
    int m_Rec;
    int m_Cmp;
    int m_Flt;
    int m_Drv;
    int m_Frq;
    //}}AFX_DATA
    CComboCombo m_TagList;
    byte  bFilterChanged:1;
    CString sFirstTag;
    //{{AFX_VIRTUAL(CHstSlotInfoDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    void FilterChanged(flag HasChanged = True);
    void BuildList();
    void Enable();
    //{{AFX_MSG(CHstSlotInfoDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnSelchangeTag();
    afx_msg void OnApply();
    afx_msg void OnSelall();
    afx_msg void OnEdit();
    //}}AFX_MSG
    afx_msg LRESULT OnComboBoxMsg(WPARAM wParam, LPARAM lParam);
    afx_msg void OnApplyChanged(UINT nID);
    afx_msg void OnUpdateEdit(CCmdUI* pCmdUi);
    afx_msg void OnUpdateApply(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================
#endif
