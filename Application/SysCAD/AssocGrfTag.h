#pragma once
#include "afxcmn.h"


// CAssocGrfTag dialog

class CAssocGrfTag : public CDialog
  {
  DECLARE_DYNAMIC(CAssocGrfTag)

  public:
    CAssocGrfTag(LPCSTR NdTag, CWnd* pParent = NULL);   // standard constructor
    virtual ~CAssocGrfTag();

    // Dialog Data
    enum { IDD = IDD_ASSOCGRAPHICS_TAG };

  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL  OnInitDialog();
    virtual void  OnOK();
    virtual void  OnCancel();

    DECLARE_MESSAGE_MAP()

  public:
    CString m_NdTag;
    afx_msg void OnBnClickedApply();
  };
