#pragma once
#include "afxwin.h"


// CSetValuesDlg dialog
class CSysCADMarshalDoc;
class CSetValuesDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetValuesDlg)

public:
	CSetValuesDlg(CSysCADMarshalDoc * pDoc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetValuesDlg();

// Dialog Data
	enum { IDD = IDD_SETVALUES };

  static bool OpenIt(CWnd * pParent, CSysCADMarshalDoc * pDoc);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL    OnInitDialog( );
  void            SaveState();
  virtual void    OnOK();
  virtual void    OnCancel();
  afx_msg void OnBnClickedRead();
  afx_msg void OnBnClickedWrite();

  static CSetValuesDlg * sm_pTheOne;

  CSysCADMarshalDoc * m_pDoc;
  CButton m_DoWhich;
};
