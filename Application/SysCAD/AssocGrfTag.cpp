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

static const int Ids[] = 
  {
  IDC_CLASS_SHOW0,
  IDC_CLASS_SHOW1,
  IDC_CLASS_SHOW2,
  IDC_CLASS_SHOW3,
  IDC_CLASS_SHOW4,
  //IDC_CLASS_SHOW5,
  IDC_TAG_SHOW0,
  IDC_TAG_SHOW1,
  IDC_TAG_SHOW2,
  IDC_TAG_SHOW3,
  IDC_TAG_SHOW4,
  //IDC_TAG_SHOW5,
  -1,
  };

BOOL CAssocGrfTag::OnInitDialog()
  {
  CDialog::OnInitDialog();

  Strng ClassId;
  if (gs_pPrj->RequestModelClassId((LPSTR)(LPCSTR)m_NdTag, ClassId))
    GetDlgItem(IDC_CLASS_FRAME)->SetWindowText(ClassId());
  GetDlgItem(IDC_TAG_FRAME)->SetWindowText(m_NdTag);
  
  for (int i=0; Ids[i]>0; i++)
    {
    CComboBox * pBox=(CComboBox *)GetDlgItem(Ids[i]);

    if (Ids[i]>=IDC_TAG_SHOW0)
      pBox->AddString("<default>");
    
    pBox->AddString("Hide");
    pBox->AddString("Show");
    pBox->AddString("In Use");
    pBox->AddString("Ever Used");
    }

  CModelAssocGrfMasks Msks;
  if (gs_pPrj->RequestModelAssocGrfsMasks(false, false, (LPSTR)(LPCSTR)m_NdTag, Msks))
    {
    CModelAssocGrfMask & Msk=Msks.GetHead();

    ((CComboBox *)GetDlgItem(IDC_CLASS_SHOW0))->SetCurSel(Msk.m_ShowClass[eAG_Makeup]-1);
    ((CComboBox *)GetDlgItem(IDC_CLASS_SHOW1))->SetCurSel(Msk.m_ShowClass[eAG_Bleed]-1);
    ((CComboBox *)GetDlgItem(IDC_CLASS_SHOW2))->SetCurSel(Msk.m_ShowClass[eAG_Spill]-1);
    ((CComboBox *)GetDlgItem(IDC_CLASS_SHOW3))->SetCurSel(Msk.m_ShowClass[eAG_Vent]-1);
    ((CComboBox *)GetDlgItem(IDC_CLASS_SHOW4))->SetCurSel(Msk.m_ShowClass[eAG_Leak]-1);
    //((CComboBox *)GetDlgItem(IDC_CLASS_SHOW5))->SetCurSel(Msk.m_ShowClass[eAG_RB]-1);

    ((CComboBox *)GetDlgItem(IDC_TAG_SHOW0))->SetCurSel(Msk.m_ShowNode[eAG_Makeup]);
    ((CComboBox *)GetDlgItem(IDC_TAG_SHOW1))->SetCurSel(Msk.m_ShowNode[eAG_Bleed]);
    ((CComboBox *)GetDlgItem(IDC_TAG_SHOW2))->SetCurSel(Msk.m_ShowNode[eAG_Spill]);
    ((CComboBox *)GetDlgItem(IDC_TAG_SHOW3))->SetCurSel(Msk.m_ShowNode[eAG_Vent]);
    ((CComboBox *)GetDlgItem(IDC_TAG_SHOW4))->SetCurSel(Msk.m_ShowNode[eAG_Leak]);
    //((CComboBox *)GetDlgItem(IDC_TAG_SHOW5))->SetCurSel(Msk.m_ShowNode[eAG_RB]);
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
  CModelAssocGrfMask Msk(m_NdTag);

  Msk.m_ShowClass[eAG_Makeup] = ((CComboBox *)GetDlgItem(IDC_CLASS_SHOW0))->GetCurSel()+1;
  Msk.m_ShowClass[eAG_Bleed ] = ((CComboBox *)GetDlgItem(IDC_CLASS_SHOW1))->GetCurSel()+1;
  Msk.m_ShowClass[eAG_Spill ] = ((CComboBox *)GetDlgItem(IDC_CLASS_SHOW2))->GetCurSel()+1;
  Msk.m_ShowClass[eAG_Vent  ] = ((CComboBox *)GetDlgItem(IDC_CLASS_SHOW3))->GetCurSel()+1;
  Msk.m_ShowClass[eAG_Leak  ] = ((CComboBox *)GetDlgItem(IDC_CLASS_SHOW4))->GetCurSel()+1;
  //Msk.m_ShowClass[eAG_RB    ] = ((CComboBox *)GetDlgItem(IDC_CLASS_SHOW5))->GetCurSel()+1;

  Msk.m_ShowNode[eAG_Makeup] = ((CComboBox *)GetDlgItem(IDC_TAG_SHOW0))->GetCurSel();
  Msk.m_ShowNode[eAG_Bleed ] = ((CComboBox *)GetDlgItem(IDC_TAG_SHOW1))->GetCurSel();
  Msk.m_ShowNode[eAG_Spill ] = ((CComboBox *)GetDlgItem(IDC_TAG_SHOW2))->GetCurSel();
  Msk.m_ShowNode[eAG_Vent  ] = ((CComboBox *)GetDlgItem(IDC_TAG_SHOW3))->GetCurSel();
  Msk.m_ShowNode[eAG_Leak  ] = ((CComboBox *)GetDlgItem(IDC_TAG_SHOW4))->GetCurSel();
  //Msk.m_ShowNode[eAG_RB    ] = ((CComboBox *)GetDlgItem(IDC_TAG_SHOW5))->GetCurSel();

  CModelAssocGrfMasks Msks;
  Msks.AddTail(Msk);

  if (gs_pPrj->RequestModelAssocGrfsMasks(true, true, NULL, Msks))
    {
    gs_pTheSFELib->SetLoadAllModelAssocGrfs();
    XUpdateDisplays(UDO_ForceUpd);
    }
  }
