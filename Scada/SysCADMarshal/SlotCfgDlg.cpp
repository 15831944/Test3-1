// SlotCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Slot.h"
#include "syscadmarshal.h"
#include "SlotCfgDlg.h"
#include "SlotMngr.h"
#include "listview.h"
#include "syscadmarshaldoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSlotCfgDlg dialog


CSlotCfgDlg::CSlotCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSlotCfgDlg::IDD, pParent)
  {
  m_pTagView=NULL;
	//{{AFX_DATA_INIT(CSlotCfgDlg)
	m_sConnects = "";
	m_sDesc     = "";
	m_sDriver   = "";
	m_sModifier = "";
	m_sRange    = "";
	m_sTag      = "";
	m_sType     = "";
	//}}AFX_DATA_INIT
  }


void CSlotCfgDlg::DoDataExchange(CDataExchange* pDX)
  {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSlotCfgDlg)
	DDX_Text(pDX, IDC_CONNECTS, m_sConnects);
	DDX_Text(pDX, IDC_DESC, m_sDesc);
	DDX_Text(pDX, IDC_DEVICE, m_sDriver);
	DDX_Text(pDX, IDC_MODIFIER, m_sModifier);
	DDX_Text(pDX, IDC_RANGE, m_sRange);
	DDX_Text(pDX, IDC_TAG, m_sTag);
	DDX_Text(pDX, IDC_TYPE, m_sType);
	//}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CSlotCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CSlotCfgDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlotCfgDlg message handlers

//---------------------------------------------------------------------------

//CSlotCfgDlg * CSlotCfgDlg::Start(CFullValue & Value, eConnSrcDst Dst, long Item, RECT & Rct, CTagView*pTagView)
CSlotCfgDlg * CSlotCfgDlg::Start(CSlotCfgStrings & Cfg, CTagView *pTagView)
  {
//  if (Value.Type()==VT_BOOL)
//    {
//    CFullValue FV(OPC_QUALITY_GOOD);
//    COleVariant &V=FV;
//    V=Value.m_vValue;
//    V.boolVal = !V.boolVal;
//    gs_SlotMngr.AppendChange(eCSD_Manual, -1, Dst, Item, gs_SlotMngr.GetTransactionID(), FV, NULL, true);
//    gs_SlotMngr.RequestQueueFlush();
//    return NULL;
//    }

  //OnEditProperties();
  CSlotCfgDlg * pDlg = new CSlotCfgDlg;

	pDlg->m_sConnects = Cfg.m_sConnects;
	pDlg->m_sDesc     = Cfg.m_sDesc;
	pDlg->m_sDriver   = Cfg.m_sDriver;
	pDlg->m_sModifier = Cfg.m_sModifier;
	pDlg->m_sRange    = Cfg.m_sRange;
	pDlg->m_sTag      = Cfg.m_sTag;
	pDlg->m_sType     = Cfg.m_sType;

  pDlg->Create((UINT)IDD, NULL);
  pDlg->m_pTagView=pTagView;
  
  //pDlg->UpdateData(false);
  pDlg->CenterWindow();//(CWnd*)pDlg->m_pTagView);
  pDlg->ShowWindow(SW_NORMAL);
  return pDlg;
  }


void AddCrLF(CString &C)
  {
  int i;
  while ((i=C.Find(" ,"))>=0)
    C.Delete(i);
  while ((i=C.Find("),"))>=0)
    {
    C.SetAt(i+1, '\r');
    C.Insert(i+2, '\n');
    }
  }

void RemoveCrLF(CString &C)
  {
  int i;
  while ((i=C.Find("\r\n"))>=0)
    {
    C.Delete(i);
    C.SetAt(i,',');
    }
  }

BOOL CSlotCfgDlg::OnInitDialog() 
{
  AddCrLF(m_sConnects);

  CDialog::OnInitDialog();

  


//    int Pos=0;
//    while (strstr(&Mods[Pos],"),"))
//      {
//      if (i>=3)
//        {
//        SetError(SErr_Modifier, "Too many Modifier functions found");
//        return false;
//        }
//      Pos = (strstr(&Mods[Pos],"),") - Mods) + 1;
//      Mods[Pos++]=0;
//      ff[i++]=&Mods[Pos];
//      }


	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSlotCfgDlg::AddEdit(eSlotCfgEdits What) 
  {
  CSysCADMarshalDoc *pDoc = m_pTagView->GetDocument();

  RemoveCrLF(m_sConnects);

  CSlotCfgStrings *pCfg=new CSlotCfgStrings;

  pCfg->m_eDoWhat   = What;
	pCfg->m_sConnects = m_sConnects;
	pCfg->m_sDesc     = m_sDesc;
	pCfg->m_sDriver   = m_sDriver;
	pCfg->m_sModifier = m_sModifier;
	pCfg->m_sRange    = m_sRange;
	pCfg->m_sTag      = m_sTag;
	pCfg->m_sType     = m_sType;
  
  pDoc->AddEdit(pCfg);

  CDialog::OnOK();
  }

void CSlotCfgDlg::OnOK() 
  {
	UpdateData(true);
  
  AddEdit(SlotCfgUpdate);

  CDialog::OnOK();
  }


void CSlotCfgDlg::OnAdd() 
  {
	UpdateData(true);
  AddEdit(SlotCfgAdd);
  }

void CSlotCfgDlg::OnDelete() 
  {
	UpdateData(true);
  AddEdit(SlotCfgDelete);
  }
