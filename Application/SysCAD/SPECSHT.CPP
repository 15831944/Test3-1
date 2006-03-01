//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#if WITHSPECSHEETS
#include "sc_defs.h"
#include "resource.h"
#include "specsht.h"
#include "accnode.h"
#include "msgwnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------

CSpecSheet::CSpecSheet(CWnd* pParent /*=NULL*/)
  : CDialog(CSpecSheet::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CSpecSheet)
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CSpecSheet::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSpecSheet)
  DDX_Control(pDX, IDC_SHEETS, m_Sheets);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSpecSheet, CDialog)
  //{{AFX_MSG_MAP(CSpecSheet)
  ON_BN_CLICKED(ID_Delete, OnDelete)
  ON_BN_CLICKED(ID_Load, OnLoad)
  ON_BN_CLICKED(ID_SAVE, OnSave)
  ON_CBN_SELCHANGE(IDC_SHEETS, OnSelchangeSheets)
  ON_CBN_EDITCHANGE(IDC_SHEETS, OnEditchangeSheets)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(ID_SAVE, OnUpdateBtn)
  ON_UPDATE_COMMAND_UI(ID_Load, OnUpdateBtn)
  ON_UPDATE_COMMAND_UI(ID_Delete, OnUpdateBtn)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CSpecSheet::OnDelete() 
  {
  CString S;
  m_Sheets.GetWindowText(S);
  if (gs_pAccWnd->DeleteSpecSheet((const char*)S))
    {
    Strng_List Names;
    gs_pAccWnd->SSNames(Names, ReadOnlyList);
    m_Sheets.ResetContent();
    for (pStrng p=Names.First(); p; p=Names.Next())
      m_Sheets.AddString(p->Str());
    UpdateData(False);
    UpdateDialogControls(this, FALSE);
    }
  }

//---------------------------------------------------------------------------

void CSpecSheet::OnLoad() 
  {
  CString S;
  m_Sheets.GetWindowText(S);
  if (gs_pAccWnd->LoadSpecSheet((const char *)S))
    OnOK();
  }

//---------------------------------------------------------------------------

void CSpecSheet::OnSave() 
  {
  CString S;
  m_Sheets.GetWindowText(S);
  Strng Name = (const char*)S;
  Name.Trim(" \t.[]");
  if (Name.Len()>0 && Name.Find('.')<0 && Name.Find('[')<0 && Name.Find(']')<0)
    {
    if (gs_pAccWnd->SaveSpecSheet((const char *)Name()))
    //if (gs_pAccWnd->SSLoad(S.GetBuffer(0)))
      OnOK();
    }
  else
    {
    LogError("Access", LF_Exclamation, "Bad Specsheet Name '%s'", (const char*)S);
    GetDlgItem(IDC_SHEETS)->SetFocus();
    m_Sheets.SetWindowText(Name());
    UpdateDialogControls(this, FALSE);
    }
  }

//---------------------------------------------------------------------------

BOOL CSpecSheet::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  Strng_List S;
  gs_pAccWnd->SSNames(S, ReadOnlyList);
  m_Sheets.ResetContent();
  for (pStrng p=S.First(); p; p=S.Next())
    m_Sheets.AddString(p->Str() ? p->Str() : "");
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CSpecSheet::OnSelchangeSheets() 
  {
  CString S;
  //m_Sheets.GetWindowText(S);
  int index = m_Sheets.GetCurSel();
  if (index>=0)
    {
    m_Sheets.GetLBText(index, S);
    m_Sheets.SetWindowText((const char*)S);
    }
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CSpecSheet::OnEditchangeSheets() 
  {
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CSpecSheet::OnUpdateBtn(CCmdUI* pCmdUi)
  {
  
  CString S;
  m_Sheets.GetWindowText(S);
  if (S.GetLength()>0)
    {
    int index = m_Sheets.FindStringExact(-1, (const char*)S);
    switch (pCmdUi->m_nID)
      {
      case ID_SAVE: pCmdUi->Enable(index<0 || ReadOnlyList[index]==0); break;
      case ID_Load: pCmdUi->Enable(index>=0); break;
      case ID_Delete: pCmdUi->Enable(index>=0 && ReadOnlyList[index]==0); break;
      }
    }
  else
    pCmdUi->Enable(0);
  }

//---------------------------------------------------------------------------
#endif

