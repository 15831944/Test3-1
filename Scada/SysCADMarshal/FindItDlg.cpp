// FindItDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SysCADMarshal.h"
#include "FindItDlg.h"
#include ".\finditdlg.h"
#include "slotmngr.h"
#include "syscadmarshaldoc.h"

// CFindItDlg dialog

CFindItDlg * CFindItDlg::sm_pTheOne=NULL;
const int MaxLBItems=20;
static LPCTSTR SelNames[eMaxSelectStrings] = { "FilterTag","FilterSimTag","FilterOPCTag","FilterType" };

IMPLEMENT_DYNAMIC(CFindItDlg, CDialog)
CFindItDlg::CFindItDlg(CSysCADMarshalDoc * pDoc, CWnd* pParent /*=NULL*/)
: CDialog(CFindItDlg::IDD, pParent)
  {
  m_pDoc=pDoc;
  ASSERT_ALWAYS(sm_pTheOne==NULL, "sm_pTheOne!=NULL")
  Create(CFindItDlg::IDD, pParent);

  m_Off=false;
  }

CFindItDlg::~CFindItDlg()
  {
  sm_pTheOne=NULL;
  }

void CFindItDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_FILTERTAG, m_FilterTag);
  DDX_Control(pDX, IDC_FILTERSIMTAG, m_FilterSimTag);
  DDX_Control(pDX, IDC_FILTEROPCTAG, m_FilterOPCTag);
  DDX_Control(pDX, IDC_FILTERTYPE, m_FilterType);
  DDX_Control(pDX, IDC_HOW, m_How);

  DDX_Control(pDX, ID_FILTEROFF, m_OffBtn);
  m_OffBtn.EnableWindow(m_Off?0:1);
  DDX_Control(pDX, IDC_IGNORECASE, m_Ignore);
  DDX_Control(pDX, IDC_CHECK1, m_AndClauses);
  DDX_Control(pDX, IDC_ERRORSONLY, m_ErrorsOnly);
  DDX_Control(pDX, IDC_HOLDSONLY, m_HoldsOnly);
  }


BEGIN_MESSAGE_MAP(CFindItDlg, CDialog)
  //ON_BN_CLICKED(IDOK, OnBnClickedOk)
 // ON_BN_CLICKED(ID_GO, OnBnClickedGo)
  //ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(ID_FILTERON, OnBnClickedFilteron)
  ON_BN_CLICKED(ID_FILTEROFF, OnBnClickedFilteroff)
END_MESSAGE_MAP()


BOOL CFindItDlg::OnInitDialog( )
  {
  CDialog::OnInitDialog();
  CString S;

  m_How.AddString(eFilterHowNames[eFilterContains]);
  m_How.AddString(eFilterHowNames[eFilterWild]);
  m_How.AddString(eFilterHowNames[eFilterRegExp]);

  m_Ignore.SetCheck(ScdPFUser.RdInt("FindDlg", "IgnoreCase", true)?1:0);
  m_AndClauses.SetCheck(ScdPFUser.RdInt("FindDlg", "AndClauses", true)?1:0);
  m_ErrorsOnly.SetCheck(ScdPFUser.RdInt("FindDlg", "ErrorsOnly", false)?1:0);
  m_HoldsOnly.SetCheck(ScdPFUser.RdInt("FindDlg", "HoldsOnly", false)?1:0);
  S=ScdPFUser.RdStr("FindDlg", "Interpretation", eFilterHowNames[eFilterContains]);
  if (m_How.SelectString(-1, S)<0)
    m_How.SelectString(-1, eFilterHowNames[eFilterContains]);

  CComboBox *CBs[eMaxSelectStrings] = { &m_FilterTag,&m_FilterSimTag,&m_FilterOPCTag,&m_FilterType };

  for (int i=0; i<eMaxSelectStrings; i++)
    {
    CComboBox &CB=*CBs[i];
    //CB.AddString("");
    CString S,T;
    for (int j=0; j<MaxLBItems; j++)
      {
      T.Format("%s.%i", SelNames[i], j);
      S=ScdPFUser.RdStr("FindDlg", T, "");
      S=S.TrimLeft(" ");
      //if (S.GetLength()>0)
      CB.AddString(S);
      }
    S=ScdPFUser.RdStr("FindDlg", SelNames[i], "");
    //if (S.GetLength()>0)
    CB.SetWindowText(S);
    }

  WINDOWPLACEMENT WP;
  WP.length=sizeof(WP);
  if (GetWindowPlacement(&WP))
    {
    int t=ScdPFUser.RdInt("FindDlg", "Win.Top", WP.rcNormalPosition.top);
    int l=ScdPFUser.RdInt("FindDlg", "Win.Left", WP.rcNormalPosition.left);
    WP.rcNormalPosition.right=l+(WP.rcNormalPosition.right-WP.rcNormalPosition.left);
    WP.rcNormalPosition.bottom=t+(WP.rcNormalPosition.bottom-WP.rcNormalPosition.top);
    WP.rcNormalPosition.left=l;
    WP.rcNormalPosition.top=t;

    SetWindowPlacement(&WP);
    }

  m_Off=ScdPFUser.RdInt("FindDlg", "Off", false)!=0;
  m_OffBtn.EnableWindow(m_Off?0:1);
  
  return TRUE;  // return TRUE unless you set the focus to a control
  };

bool CFindItDlg::OpenIt(CWnd * pParent, CSysCADMarshalDoc * pDoc)
  {
  if (sm_pTheOne==NULL)
    {
    sm_pTheOne=new CFindItDlg(pDoc, pParent);
    return true;
    }
  return false;
  }

// CFindItDlg message handlers
void CFindItDlg::SetSelection(bool Off)
  {
  CSelection Selection;
  CString S;

  m_Off=Off;
  Selection.m_bOn=!Off;
  Selection.m_bCaseSens=m_Ignore.GetCheck()==0;
  Selection.m_bAndClause=m_AndClauses.GetCheck()!=0;
  Selection.m_bErrorsOnly=m_ErrorsOnly.GetCheck()!=0;
  Selection.m_bHoldsOnly=m_HoldsOnly.GetCheck()!=0;

  m_FilterTag.GetWindowText(Selection.m_sStr[eFilterTheTag]);             
  m_FilterSimTag.GetWindowText(Selection.m_sStr[eFilterSimTag]);          
  m_FilterOPCTag.GetWindowText(Selection.m_sStr[eFilterOPCTag]);          
  m_FilterType.GetWindowText(Selection.m_sStr[eFilterType]);            

  m_How.GetWindowText(S);
  for (int i=0; i<eFilterRuleCount; i++)
    {
    Selection.m_sStr[i]=Selection.m_sStr[i].TrimLeft(" ");
    Selection.m_sStr[i]=Selection.m_sStr[i].TrimRight(" ");
    if (S==eFilterHowNames[i])
      {
      Selection.m_eHow = (eFilterHow)i;
      break;
      }
    }
  ASSERT(i<eFilterRuleCount);

  CComboBox *CBs[eMaxSelectStrings] = { &m_FilterTag,&m_FilterSimTag,&m_FilterOPCTag,&m_FilterType };
  for (int i=0; i<eMaxSelectStrings; i++)
    {
    CComboBox &CB=*CBs[i];
    CB.GetWindowText(S);
    S=S.TrimLeft();
    S=S.TrimRight();
    if (S.GetLength()>0)
      {
      int j=CB.FindString(-1, S);
      if (j>=0)
        {
        CB.DeleteString(j);
        CB.InsertString(0, S);
        }
      else
        CB.InsertString(0, S);
      }
    if (CB.FindString(-1, "")<0)
      CB.InsertString(MaxLBItems-1, "");
      
    while (CB.GetCount()>MaxLBItems)
      CB.DeleteString(MaxLBItems);
    CB.SetWindowText(S);
    }
  m_pDoc->SetSelection(Selection);
  }

void CFindItDlg::OnOK()
  {
  //SetSelection(m_Off);
  SaveState();
  CDialog::OnOK();
  delete this;
  }

void CFindItDlg::OnCancel()
  {
  SaveState();
  CDialog::OnCancel();
  delete this;
  }


void CFindItDlg::SaveState()
  {
  CString S,T;

  CComboBox *CBs[eMaxSelectStrings] = { &m_FilterTag,&m_FilterSimTag,&m_FilterOPCTag,&m_FilterType };
  for (int i=0; i<eMaxSelectStrings; i++)
    {
    CComboBox &CB=*CBs[i];
    CB.GetWindowText(S);
    ScdPFUser.WrStr("FindDlg", SelNames[i], S);
    for (int j=0; j<CB.GetCount(); j++)
      {
      CB.GetLBText(j, S);
      if (S.GetLength()>0)
        {
        T.Format("%s.%i", SelNames[i], j);
        ScdPFUser.WrStr("FindDlg", T, S);
        }
      }
    }

  m_How.GetWindowText(S);
  ScdPFUser.WrStr("FindDlg", "Interpretation", S);
  ScdPFUser.WrInt("FindDlg", "Off", m_Off?1:0);
  ScdPFUser.WrInt("FindDlg", "IgnoreCase", m_Ignore.GetCheck()?1:0);
  ScdPFUser.WrInt("FindDlg", "AndClauses", m_AndClauses.GetCheck()?1:0);
  ScdPFUser.WrInt("FindDlg", "ErrorsOnly", m_ErrorsOnly.GetCheck()?1:0);
  ScdPFUser.WrInt("FindDlg", "HoldsOnly", m_HoldsOnly.GetCheck()?1:0);

  WINDOWPLACEMENT WP;
  WP.length=sizeof(WP);
  if (GetWindowPlacement(&WP))
    {
    ScdPFUser.WrInt("FindDlg", "Win.Top", WP.rcNormalPosition.top);
    ScdPFUser.WrInt("FindDlg", "Win.Left", WP.rcNormalPosition.left);
    }
  }

void CFindItDlg::OnBnClickedFilteron()
  {
  SetSelection(false);
  SaveState();
  }

void CFindItDlg::OnBnClickedFilteroff()
  {
  SetSelection(true);
  SaveState();
  }
