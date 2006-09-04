#pragma once


// CRenamePageDlg dialog

class CRenamePageDlg : public CDialog
  {
  DECLARE_DYNAMIC(CRenamePageDlg)

  public:
    CRenamePageDlg(LPCTSTR OldName, CWnd* pParent = NULL);   // standard constructor
    virtual ~CRenamePageDlg();

    // Dialog Data
    enum { IDD = IDD_RENAMEPAGE };

  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
  public:
    CString m_NewName;
  public:
    CString m_OldName;
  };
