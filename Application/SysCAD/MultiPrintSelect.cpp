// MultiPrintSelect.cpp : implementation file
//

#include "stdafx.h"
#include "MultiPrintSelect.h"


// CMultiPrintSelect dialog

IMPLEMENT_DYNAMIC(CMultiPrintSelect, CDialog)

CMultiPrintSelect::CMultiPrintSelect(CWnd* pParent /*=NULL*/)
: CDialog(CMultiPrintSelect::IDD, pParent)
  {
  pStartDoc=NULL;

  }

CMultiPrintSelect::~CMultiPrintSelect()
  {
  }

void CMultiPrintSelect::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_DOCLIST, m_DocList);
  }


BEGIN_MESSAGE_MAP(CMultiPrintSelect, CDialog)
END_MESSAGE_MAP()


void CMultiPrintSelect::AddDocument(CDocument * pDoc)
  {
  m_Documents.AddTail(pDoc);
  }

void CMultiPrintSelect::AddStartDocument(CDocument * pDoc)
  {
  pStartDoc=pDoc;
  }

BOOL CMultiPrintSelect::OnInitDialog()
  {
  CDialog::OnInitDialog();

  m_DocList.InsertColumn(0, "Document", LVCFMT_LEFT, 240);
  m_DocList.SetExtendedStyle(m_DocList.GetExtendedStyle()|LVS_EX_CHECKBOXES);

  POSITION Pos=m_Documents.GetHeadPosition();
  while (Pos)
    {
    CDocument *pDoc=m_Documents.GetNext(Pos);
    int i=m_DocList.InsertItem(0, pDoc->GetTitle());
    m_DocList.SetItemData(i, (DWORD_PTR)pDoc);
    if (pStartDoc==pDoc)
      m_DocList.SetCheck(i);
    }

  return TRUE;
  }

// CMultiPrintSelect message handlers

void CMultiPrintSelect::OnOK()
  {
  m_Documents.RemoveAll();

  for (int i=0; i<m_DocList.GetItemCount(); i++)
    {
    if (m_DocList.GetCheck(i))
      {
      m_Documents.AddTail((CDocument*)m_DocList.GetItemData(i));
      }
    }

  CDialog::OnOK();
  };

