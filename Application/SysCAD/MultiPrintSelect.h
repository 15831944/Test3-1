#pragma once
#include "afxcmn.h"
#include "resource.h"


// CMultiPrintSelect dialog

class CMultiPrintSelect : public CDialog
{
	DECLARE_DYNAMIC(CMultiPrintSelect)

public:
	CMultiPrintSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMultiPrintSelect();
	virtual void OnOK();

// Dialog Data
	enum { IDD = IDD_MULTIPRINTSELECT };

  void AddDocument(CDocument * pDoc);
  void AddStartDocument(CDocument * pDoc);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

  BOOL OnInitDialog();
  CListCtrl       m_DocList;

public:
  CList<CDocument*, CDocument*> m_Documents;
  CDocument * pStartDoc;


  };
