#pragma once
#include "afxwin.h"


// CSelectImport dialog

class CSelectImport : public CDialog
{
	DECLARE_DYNAMIC(CSelectImport)

public:
	CSelectImport(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectImport();

// Dialog Data
	enum { IDD = IDD_SELECTIMPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  // Select type of data to Import 
  afx_msg void OnBnClickedOk();
  CComboBox m_List;
  virtual BOOL OnInitDialog(void);
  long m_lImportWhat;
  afx_msg void OnCbnSelchangeImportwhat();
};

// CSelectImport dialog

class CSelectExport : public CDialog
{
	DECLARE_DYNAMIC(CSelectExport)

public:
	CSelectExport(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectExport();

// Dialog Data
	enum { IDD = IDD_SELECTEXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  // Select type of data to Export 
  afx_msg void OnBnClickedOk();
  CComboBox m_List;
  virtual BOOL OnInitDialog(void);
  long m_lExportWhat;
  afx_msg void OnCbnSelchangeExportwhat();
};
