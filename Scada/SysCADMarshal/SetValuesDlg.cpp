// SetValues.cpp : implementation file
//

#include "stdafx.h"
#include "SysCADMarshal.h"
#include "SetValuesDlg.h"
#include ".\setvaluesdlg.h"

#include "slotmngr.h"
#include "syscadmarshaldoc.h"

// CSetValuesDlg dialog

CSetValuesDlg* CSetValuesDlg::sm_pTheOne=NULL;
IMPLEMENT_DYNAMIC(CSetValuesDlg, CDialog)
CSetValuesDlg::CSetValuesDlg(CSysCADMarshalDoc * pDoc, CWnd* pParent /*=NULL*/)
: CDialog(CSetValuesDlg::IDD, pParent)
  {
  m_pDoc=pDoc;
  ASSERT_ALWAYS(sm_pTheOne==NULL, "sm_pTheOne!=NULL", __FILE__, __LINE__);
  Create(CSetValuesDlg::IDD, pParent);

  }

CSetValuesDlg::~CSetValuesDlg()
  {
  sm_pTheOne=NULL;
  }

void CSetValuesDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_SETALL, m_DoWhich);
  }


BEGIN_MESSAGE_MAP(CSetValuesDlg, CDialog)
  ON_BN_CLICKED(IDC_SET, OnBnClickedSet)
  ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
END_MESSAGE_MAP()

BOOL CSetValuesDlg::OnInitDialog( )
  {
  CDialog::OnInitDialog();
  CString S;
  

  m_DoWhich.SetCheck(1);
  //m_How.AddString(eFilterHowNames[eFilterContains]);
  //m_How.AddString(eFilterHowNames[eFilterWild]);
  //m_How.AddString(eFilterHowNames[eFilterRegExp]);

  //m_Ignore.SetCheck(ScdPFUser.RdInt("SetValuesDlg", "IgnoreCase", true)?1:0);
  //m_AndClauses.SetCheck(ScdPFUser.RdInt("SetValuesDlg", "AndClauses", true)?1:0);
  //m_ErrorsOnly.SetCheck(ScdPFUser.RdInt("SetValuesDlg", "ErrorsOnly", false)?1:0);
  //m_HoldsOnly.SetCheck(ScdPFUser.RdInt("SetValuesDlg", "HoldsOnly", false)?1:0);
  //S=ScdPFUser.RdStr("SetValuesDlg", "Interpretation", eFilterHowNames[eFilterContains]);
  //if (m_How.SelectString(-1, S)<0)
  //  m_How.SelectString(-1, eFilterHowNames[eFilterContains]);
  CheckRadioButton(IDC_SETALL, IDC_SETGROUPS, IDC_SETALL+ScdPFUser.RdInt("SetValuesDlg", "Which", 0));

  //CComboBox *CBs[eMaxSelectStrings] = { &m_FilterTag,&m_FilterSimTag,&m_FilterOPCTag,&m_FilterType };

  //for (int i=0; i<eMaxSelectStrings; i++)
  //  {
  //  CComboBox &CB=*CBs[i];
  //  //CB.AddString("");
  //  CString S,T;
  //  for (int j=0; j<MaxLBItems; j++)
  //    {
  //    T.Format("%s.%i", SelNames[i], j);
  //    S=ScdPFUser.RdStr("SetValuesDlg", T, "");
  //    S=S.TrimLeft(" ");
  //    //if (S.GetLength()>0)
  //    CB.AddString(S);
  //    }
  //  S=ScdPFUser.RdStr("SetValuesDlg", SelNames[i], "");
  //  //if (S.GetLength()>0)
  //  CB.SetWindowText(S);
  //  }

  WINDOWPLACEMENT WP;
  WP.length=sizeof(WP);
  if (GetWindowPlacement(&WP))
    {
    int t=ScdPFUser.RdInt("SetValuesDlg", "Win.Top", WP.rcNormalPosition.top);
    int l=ScdPFUser.RdInt("SetValuesDlg", "Win.Left", WP.rcNormalPosition.left);
    WP.rcNormalPosition.right=l+(WP.rcNormalPosition.right-WP.rcNormalPosition.left);
    WP.rcNormalPosition.bottom=t+(WP.rcNormalPosition.bottom-WP.rcNormalPosition.top);
    WP.rcNormalPosition.left=l;
    WP.rcNormalPosition.top=t;

    SetWindowPlacement(&WP);
    }

  //m_Off=ScdPFUser.RdInt("Set", "Off", false)!=0;
  //m_OffBtn.EnableWindow(m_Off?0:1);

  return TRUE;  // return TRUE unless you set the focus to a control
  }

bool CSetValuesDlg::OpenIt(CWnd * pParent, CSysCADMarshalDoc * pDoc)
  {
  if (sm_pTheOne==NULL)
    {
    sm_pTheOne=new CSetValuesDlg(pDoc, pParent);
    return true;
    }
  return false;
  }

void CSetValuesDlg::OnOK()
  {
  //SetSelection(m_Off);
  SaveState();
  CDialog::OnOK();
  delete this;
  }

void CSetValuesDlg::OnCancel()
  {
  SaveState();
  CDialog::OnCancel();
  delete this;
  }

// CSetValuesDlg message handlers
void CSetValuesDlg::SaveState()
  {
  CString S,T;

  //CComboBox *CBs[eMaxSelectStrings] = { &m_FilterTag,&m_FilterSimTag,&m_FilterOPCTag,&m_FilterType };
  //for (int i=0; i<eMaxSelectStrings; i++)
  //  {
  //  CComboBox &CB=*CBs[i];
  //  CB.GetWindowText(S);
  //  ScdPFUser.WrStr("SetValuesDlg", SelNames[i], S);
  //  for (int j=0; j<CB.GetCount(); j++)
  //    {
  //    CB.GetLBText(j, S);
  //    if (S.GetLength()>0)
  //      {
  //      T.Format("%s.%i", SelNames[i], j);
  //      ScdPFUser.WrStr("SetValuesDlg", T, S);
  //      }
  //    }
  //  }

  //m_How.GetWindowText(S);
  //ScdPFUser.WrStr("SetValuesDlg", "Interpretation", S);
  //ScdPFUser.WrInt("SetValuesDlg", "Off", m_Off?1:0);
  //ScdPFUser.WrInt("SetValuesDlg", "IgnoreCase", m_Ignore.GetCheck()?1:0);
  //ScdPFUser.WrInt("SetValuesDlg", "AndClauses", m_AndClauses.GetCheck()?1:0);
  //ScdPFUser.WrInt("SetValuesDlg", "ErrorsOnly", m_ErrorsOnly.GetCheck()?1:0);
  //ScdPFUser.WrInt("SetValuesDlg", "HoldsOnly", m_HoldsOnly.GetCheck()?1:0);
  ScdPFUser.WrInt("SetValuesDlg", "Which", GetCheckedRadioButton(IDC_SETALL, IDC_SETGROUPS)-IDC_SETALL);

  WINDOWPLACEMENT WP;
  WP.length=sizeof(WP);
  if (GetWindowPlacement(&WP))
    {
    ScdPFUser.WrInt("SetValuesDlg", "Win.Top", WP.rcNormalPosition.top);
    ScdPFUser.WrInt("SetValuesDlg", "Win.Left", WP.rcNormalPosition.left);
    }
  }

void CSetValuesDlg::OnBnClickedSet()
  {
  UpdateData(true);

  CSetValuesOptions *pBlk=new CSetValuesOptions;
  pBlk->m_eAction=CSetValuesOptions::eSet; 
  int Chk=GetCheckedRadioButton(IDC_SETALL, IDC_SETGROUPS);
  pBlk->m_eWhich=(CSetValuesOptions::eWhich)(Chk-IDC_SETALL);
  PostThreadMessage(gs_SlotMngr.ThreadId(), WMU_SETVALUES, 0, (LPARAM)pBlk);
  }

void CSetValuesDlg::OnBnClickedRefresh()
  {
  UpdateData(true);

  CSetValuesOptions *pBlk=new CSetValuesOptions;
  pBlk->m_eAction=CSetValuesOptions::eRefresh; 
  int Chk=GetCheckedRadioButton(IDC_SETALL, IDC_SETGROUPS);
  pBlk->m_eWhich=(CSetValuesOptions::eWhich)(Chk-IDC_SETALL);
  PostThreadMessage(gs_SlotMngr.ThreadId(), WMU_SETVALUES, 0, (LPARAM)pBlk);
  }
