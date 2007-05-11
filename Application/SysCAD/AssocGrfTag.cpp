// AssocGrfTag.cpp : implementation file
//

#include "stdafx.h"
#include "sc_defs.h"
#include "gpwfuncs.h"
#include "resource.h"
#include "helpinfo.h"
#include "chngtag.h"
#include "accnode.h"
#include "selctwnd.h"
#include "copyblk.h"
#include "project.h"
#include "scd_wm.h"
#include "msgwnd.h"

#include "AssocGrfTag.h"


// CAssocGrfTag dialog

IMPLEMENT_DYNAMIC(CAssocGrfTag, CDialog)

CAssocGrfTag::CAssocGrfTag(LPCSTR NdTag, CWnd* pParent /*=NULL*/)
: CDialog(CAssocGrfTag::IDD, pParent),
m_NdTag(NdTag)
  {

  }

CAssocGrfTag::~CAssocGrfTag()
  {
  }

void CAssocGrfTag::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  }


BEGIN_MESSAGE_MAP(CAssocGrfTag, CDialog)
  ON_BN_CLICKED(IDAPPLY, &CAssocGrfTag::OnBnClickedApply)
END_MESSAGE_MAP()

BOOL CAssocGrfTag::OnInitDialog()
  {
  CDialog::OnInitDialog();

  Strng ClassId;
  if (gs_pPrj->RequestModelClassId((LPSTR)(LPCSTR)m_NdTag, ClassId))
    GetDlgItem(IDC_CLASS_FRAME)->SetWindowText(ClassId());
  GetDlgItem(IDC_TAG_FRAME)->SetWindowText(m_NdTag);
  
  CModelAssocGraphicMasks Msks;
  if (gs_pPrj->RequestModelAssocGraphicsMasks(false, (LPSTR)(LPCSTR)m_NdTag, Msks))
    {
    CModelAssocGraphicMask & Msk=Msks.GetHead();

    CheckDlgButton(IDC_CLASS_SHOW0, (Msk.m_ClassShow & AGM_Makeup)?1:0);
    CheckDlgButton(IDC_CLASS_SHOW1, (Msk.m_ClassShow & AGM_Bleed)?1:0);
    CheckDlgButton(IDC_CLASS_SHOW2, (Msk.m_ClassShow & AGM_Spill)?1:0);
    CheckDlgButton(IDC_CLASS_SHOW3, (Msk.m_ClassShow & AGM_Vent)?1:0);
    CheckDlgButton(IDC_CLASS_SHOW4, (Msk.m_ClassShow & AGM_Leak)?1:0);
    CheckDlgButton(IDC_CLASS_SHOW5, (Msk.m_ClassShow & AGM_RB)?1:0);

    CheckDlgButton(IDC_TAG_SHOW0, (Msk.m_TagShow & AGM_Makeup)?1:0);
    CheckDlgButton(IDC_TAG_SHOW1, (Msk.m_TagShow & AGM_Bleed)?1:0);
    CheckDlgButton(IDC_TAG_SHOW2, (Msk.m_TagShow & AGM_Spill)?1:0);
    CheckDlgButton(IDC_TAG_SHOW3, (Msk.m_TagShow & AGM_Vent)?1:0);
    CheckDlgButton(IDC_TAG_SHOW4, (Msk.m_TagShow & AGM_Leak)?1:0);
    CheckDlgButton(IDC_TAG_SHOW5, (Msk.m_TagShow & AGM_RB)?1:0);

    CheckDlgButton(IDC_TAG_HIDE0, (Msk.m_TagHide & AGM_Makeup)?1:0);
    CheckDlgButton(IDC_TAG_HIDE1, (Msk.m_TagHide & AGM_Bleed)?1:0);
    CheckDlgButton(IDC_TAG_HIDE2, (Msk.m_TagHide & AGM_Spill)?1:0);
    CheckDlgButton(IDC_TAG_HIDE3, (Msk.m_TagHide & AGM_Vent)?1:0);
    CheckDlgButton(IDC_TAG_HIDE4, (Msk.m_TagHide & AGM_Leak)?1:0);
    CheckDlgButton(IDC_TAG_HIDE5, (Msk.m_TagHide & AGM_RB)?1:0);
    }


  ShowWindow(SW_SHOW);
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

void CAssocGrfTag::OnOK()
  {
  OnBnClickedApply();

  CDialog::OnOK();
  }

void CAssocGrfTag::OnCancel()
  {
  CDialog::OnCancel();
  };


void CAssocGrfTag::OnBnClickedApply()
  {
  CModelAssocGraphicMask Msk(m_NdTag,0,0,0);

  Msk.m_ClassShow |= IsDlgButtonChecked(IDC_CLASS_SHOW0) ? AGM_Makeup:0;
  Msk.m_ClassShow |= IsDlgButtonChecked(IDC_CLASS_SHOW1) ? AGM_Bleed:0;
  Msk.m_ClassShow |= IsDlgButtonChecked(IDC_CLASS_SHOW2) ? AGM_Spill:0;
  Msk.m_ClassShow |= IsDlgButtonChecked(IDC_CLASS_SHOW3) ? AGM_Vent:0;
  Msk.m_ClassShow |= IsDlgButtonChecked(IDC_CLASS_SHOW4) ? AGM_Leak:0;
  Msk.m_ClassShow |= IsDlgButtonChecked(IDC_CLASS_SHOW5) ? AGM_RB:0;

  Msk.m_TagShow |= IsDlgButtonChecked(IDC_TAG_SHOW0) ? AGM_Makeup:0;
  Msk.m_TagShow |= IsDlgButtonChecked(IDC_TAG_SHOW1) ? AGM_Bleed:0;
  Msk.m_TagShow |= IsDlgButtonChecked(IDC_TAG_SHOW2) ? AGM_Spill:0;
  Msk.m_TagShow |= IsDlgButtonChecked(IDC_TAG_SHOW3) ? AGM_Vent:0;
  Msk.m_TagShow |= IsDlgButtonChecked(IDC_TAG_SHOW4) ? AGM_Leak:0;
  Msk.m_TagShow |= IsDlgButtonChecked(IDC_TAG_SHOW5) ? AGM_RB:0;

  Msk.m_TagHide |= IsDlgButtonChecked(IDC_TAG_HIDE0) ? AGM_Makeup:0;
  Msk.m_TagHide |= IsDlgButtonChecked(IDC_TAG_HIDE1) ? AGM_Bleed:0;
  Msk.m_TagHide |= IsDlgButtonChecked(IDC_TAG_HIDE2) ? AGM_Spill:0;
  Msk.m_TagHide |= IsDlgButtonChecked(IDC_TAG_HIDE3) ? AGM_Vent:0;
  Msk.m_TagHide |= IsDlgButtonChecked(IDC_TAG_HIDE4) ? AGM_Leak:0;
  Msk.m_TagHide |= IsDlgButtonChecked(IDC_TAG_HIDE5) ? AGM_RB:0;

  CModelAssocGraphicMasks Msks;
  Msks.AddTail(Msk);

  if (gs_pPrj->RequestModelAssocGraphicsMasks(true, NULL, Msks))
    {
    gs_Exec.MaintainModelAssocGraphics(NULL);
    }
  }
