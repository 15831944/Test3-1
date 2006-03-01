
#ifndef  __RTDLGS_H
#define  __RTDLGS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "resource.h"
#include "MdlCfg.h"

//===========================================================================

class CRTTSCfgPage : public CMdlCfgBase
  {
  public:
    CRTTSCfgPage(CMdlCfgSheet * Sheet);
    ~CRTTSCfgPage();
    //{{AFX_DATA(CRTTSCfgPage)
    enum { IDD = IDD_PP_RTTS_CFG };
    CCustomListBox m_AssayList;
    CString m_DBName;
    CString m_TableName;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CRTTSCfgPage)
    public:
    virtual void OnOK();
    virtual BOOL OnKillActive();
    virtual BOOL OnSetActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CRTTSCfgPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnAddAssay();
    afx_msg void OnChangeAssay();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//{{AFX_INSERT_LOCATION}}

#endif
