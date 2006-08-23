#pragma once
#include "resource.h"
#include "afxwin.h"


// CBadLicLocation dialog

class CBadLicLocation : public CDialog
  {
  DECLARE_DYNAMIC(CBadLicLocation)

  public:
    CBadLicLocation(LPCTSTR Location, CWnd* pParent = NULL);   // standard constructor
    virtual ~CBadLicLocation();

    // Dialog Data
    enum { IDD = IDD_BADLICVERSION };

  public:
    CString m_Location; 
    int     m_Return;

  protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnBnClickedChangelocation();
  public:
    afx_msg void OnBnClickedContinueindemo();
  public:
    afx_msg void OnBnClickedCancel();
  public:
    CStatic m_CurLocation;
  };
