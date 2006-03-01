//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __QRYDLG_H
#define  __QRYDLG_H

#ifndef __HELPINFO_H
#include "helpinfo.h"
#endif
//-------------------------------------------------------------------------

_FWDDEF(CTagVwDoc)
_FWDDEF(CQueryDlg)
class CQueryDlg : public CDialog
  {
  public:
    CQueryDlg(CWnd* pParent, pCTagVwDoc Doc, double StartTime, double EndTime);
    ~CQueryDlg();
    //{{AFX_DATA(CQueryDlg)
    enum { IDD = IDD_QUERYDLG };
    CListBox	m_TagList;
    CString	m_Filename;
    CString	m_StartTime;
    CString	m_EndTime;
    long	m_NoPts;
    CString	m_Tag;
    int		m_Option;
    BOOL	m_Headings;
    BOOL	m_TimeOptFull;
    int		m_TimeOptUnits;
    //}}AFX_DATA
    pCTagVwDoc pDoc;
    void AddTag(pchar Tag);
    //{{AFX_VIRTUAL(CQueryDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL
  protected:
    CCustomToolTipCtrl* pTTC;
    void SaveOptions();
    //{{AFX_MSG(CQueryDlg)
    virtual void OnOK();
    virtual void OnCancel();
    virtual BOOL OnInitDialog();
    afx_msg void OnAddtag();
    afx_msg void OnRemovetag();
    afx_msg void OnSelchangeTaglist();
    afx_msg void OnChangeTag();
    afx_msg void OnOption();
    afx_msg void OnOption1();
    //}}AFX_MSG
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()
  };

#endif

//-------------------------------------------------------------------------
