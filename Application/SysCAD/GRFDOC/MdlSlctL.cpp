//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "scdver.h"
#include "..\resource.h"
#include "helpinfo.h"
#include "mdlslctl.h"
#include "project.h"
#include "licbase.h"
#include "msgwnd.h"
#include "sfe_base.h"
#include ".\mdlslctl.h"

#ifdef _DEBUG
#undef THIS_FILE    
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
//
//
//===========================================================================

WINDOWPLACEMENT CInsertLinkDlg::sm_WP;
bool CInsertLinkDlg::sm_bWPOk = 0;
bool CInsertLinkDlg::sm_bDrawOrth = 1;
bool CInsertLinkDlg::sm_bDrawSingleSeg = 0;
int CInsertLinkDlg::sm_iLinkDrawMethod = 0;
int CInsertLinkDlg::sm_iArrowRuleMethod = 3;
int CInsertLinkDlg::sm_iLineWidth = 0;
double CInsertLinkDlg::sm_LineWidthMap[3] = {0, 1, 1.5};
int CInsertLinkDlg::sm_LastConn = -1;

//---------------------------------------------------------------------------

CInsertLinkDlg::CInsertLinkDlg(LPCTSTR Group, GrfComCmdBlk* Mdl, LPCTSTR SrcTS, LPCTSTR DstTS, CWnd* pParent/*=NULL*/)
	: CDialog(CInsertLinkDlg::IDD, pParent)
  {
  gs_pCmd->SetDlgBusy();
  m_pMdl = Mdl;
  m_Group = Group;  
  m_bIsCable = strcmp(Group, ElecLinkGrp)==0;
  m_AutoWire=true;
  m_SrcTS=SrcTS?SrcTS:"";
  m_DstTS=DstTS?DstTS:"", 
  m_bRedraw=false;
  nPoints = 0;
  m_bTagChgd = 0;
  nModels = 0;
  //{{AFX_DATA_INIT(CInsertLinkDlg)
	m_HideTag = FALSE;
	m_ArrowScale = 2.5;
	//}}AFX_DATA_INIT
  Create(CInsertLinkDlg::IDD, pParent);
  }

//---------------------------------------------------------------------------

CInsertLinkDlg::CInsertLinkDlg(GrfComCmdBlk* Mdl, LPCTSTR LnkTag, CWnd* pParent/*=NULL*/)
	: CDialog(CInsertLinkDlg::IDD, pParent)
  {
  gs_pCmd->SetDlgBusy();
  m_pMdl = Mdl;
  m_Tag = LnkTag;
  m_bIsCable=false;
  m_AutoWire=true;
  m_bRedraw=true;
  nPoints = 0;
  m_bTagChgd = 0;
  nModels = 0;
  Create(CInsertLinkDlg::IDD, pParent);
  }

//---------------------------------------------------------------------------

CInsertLinkDlg::~CInsertLinkDlg()
  {
  gs_pCmd->SetDlgBusy(0);
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::Reset()
  {
  sm_LastConn = -1;
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CInsertLinkDlg)
  DDX_Check(pDX, IDC_HideTag, m_HideTag);
  DDX_Text(pDX, IDC_MDLARROWSCL, m_ArrowScale);
  //}}AFX_DATA_MAP
  DDX_Check(pDX, IDC_ROUTEWIRES, m_AutoWire);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CInsertLinkDlg, CDialog)
  //{{AFX_MSG_MAP(CInsertLinkDlg)
  ON_WM_DESTROY()
  ON_BN_CLICKED(IDC_BACKPOINT, OnBack)
  ON_BN_CLICKED(IDB_ORTHO, OnOrtho)
  ON_BN_CLICKED(IDB_FREE, OnFree)
  ON_BN_CLICKED(IDB_BREAK, OnBreak)
  ON_CBN_SELCHANGE(IDC_MODEL_LIST, OnSelchangeModelList)
  ON_CBN_SELCHANGE(IDC_DRAWMETHOD_LIST, OnSelchangeDrawMethodList)
  ON_CBN_SELCHANGE(IDC_ARROWRULE_LIST, OnSelchangeArrowRuleList)
  ON_BN_CLICKED(ID_MDLDOC, OnMdldoc)
  ON_EN_CHANGE(IDC_MDLTAG, OnChangeMdltag)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOKBtn)
  ON_UPDATE_COMMAND_UI(IDC_BACKPOINT, OnUpdateBackBtn)
  ON_UPDATE_COMMAND_UI(IDB_BREAK, OnUpdateBreakBtn)
  //ON_WM_MOUSEMOVE()
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_CBN_SELCHANGE(IDC_LINKWIDTH, OnCbnSelchangeLinkwidth)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CInsertLinkDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  CProfINIFile PF(PrjIniFile());
  sm_bDrawOrth        = (PF.RdInt("InsertLink", "$DrawOrth", sm_iArrowRuleMethod)!=0);
  sm_bDrawSingleSeg   = (PF.RdInt("InsertLink", "$DrawSingleSeg", sm_iArrowRuleMethod)!=0);
  sm_iLinkDrawMethod  = PF.RdInt("InsertLink", "$DrawMethod", sm_iLinkDrawMethod);
  sm_iArrowRuleMethod = PF.RdInt("InsertLink", "$ArrowRuleMethod", sm_iArrowRuleMethod);
  sm_iLineWidth       = PF.RdInt("InsertLink", "$LineWidth", sm_iLineWidth);
  m_HideTag           = (PF.RdInt("InsertLink", "$HideTag", 1)!=0);
  m_ArrowScale        = PF.RdDouble("InsertLink", "$ArrowScale", 2.5);
  m_AutoWire          = PF.RdInt("InsertLink", "$AutoWire", 1);
  UpdateData(FALSE);

  OrthoBitmap.LoadMappedBitmap(IDB_BTN_ORTHO);
  FreeBitmap.LoadMappedBitmap(IDB_BTN_FREE);
  BreakBitmap.LoadMappedBitmap(IDB_BTN_BREAK);
  OrthoOffBitmap.LoadMappedBitmap(IDB_BTN_ORTHO_OFF);
  FreeOffBitmap.LoadMappedBitmap(IDB_BTN_FREE_OFF);
  //BreakOffBitmap.LoadMappedBitmap(IDB_BTN_BREAK_OFF);
  ((CButton*)GetDlgItem(IDB_ORTHO))->SetBitmap((HBITMAP)(sm_bDrawOrth ? OrthoOffBitmap : OrthoBitmap));
  ((CButton*)GetDlgItem(IDB_FREE))->SetBitmap((HBITMAP)(sm_bDrawOrth ? FreeBitmap : FreeOffBitmap));
  ((CButton*)GetDlgItem(IDB_BREAK))->SetBitmap((HBITMAP)BreakBitmap);

  CComboBox* pMdlCB = NULL;
  if (m_bRedraw)
    {
    SetWindowText("Redraw Link");
    GetDlgItem(IDC_MODEL_LIST)->EnableWindow(FALSE);
    GetDlgItem(IDC_DRAWMETHOD_LIST)->EnableWindow(FALSE);
    GetDlgItem(IDC_MDLTAG)->EnableWindow(FALSE);
    GetDlgItem(IDC_MDLTAG)->SetWindowText((LPCSTR)m_Tag());
    }
  else
    {
    pMdlCB = (CComboBox*)GetDlgItem(IDC_MODEL_LIST);
    pMdlCB->ResetContent();
    nModels = m_MLH.BuildList("InsertLink", m_Group(), pMdlCB, NULL, NULL);
    if (nModels==0)
      {
      LogError("GrfCmds", 0, "No '%s' Models Available", m_Group());
      EndDialog(IDCANCEL);
      return FALSE;
      }

    CComboBox* pDrwMethCB = (CComboBox*)GetDlgItem(IDC_DRAWMETHOD_LIST);
    pDrwMethCB->AddString("Digitized points");
    pDrwMethCB->AddString("Select points (single segment)");
    pDrwMethCB->AddString("Select points (orthoganal)");
    pDrwMethCB->AddString("Mid points (single segment)");
    pDrwMethCB->AddString("Mid points (orthoganal)");
    m_iPrevSel = 0;
    if (sm_iLinkDrawMethod==1)
      m_iPrevSel = (sm_bDrawSingleSeg ? 1 : 2);
    else if (sm_iLinkDrawMethod==2)
      m_iPrevSel = (sm_bDrawSingleSeg ? 3 : 4);
    pDrwMethCB->SetCurSel(m_iPrevSel);
    }

  CComboBox* pLineWidthRuleCB = (CComboBox*)GetDlgItem(IDC_LINKWIDTH);
  pLineWidthRuleCB->AddString("Narrow - 0");
  pLineWidthRuleCB->AddString("1 mm");
  pLineWidthRuleCB->AddString("1.5 mm");
  pLineWidthRuleCB->SetCurSel(sm_iLineWidth);

  CComboBox* pArrowRuleCB = (CComboBox*)GetDlgItem(IDC_ARROWRULE_LIST);
  pArrowRuleCB->AddString("None");
  pArrowRuleCB->AddString("Centre of each dirn change");
  pArrowRuleCB->AddString("End of each dirn change");
  pArrowRuleCB->AddString("End of each dirn chng & last segment");
  pArrowRuleCB->AddString("Centre of longest segment");
  pArrowRuleCB->AddString("At end");
  pArrowRuleCB->AddString("At end and centre (simple)");
  pArrowRuleCB->AddString("At end and centre");
  pArrowRuleCB->SetCurSel(sm_iArrowRuleMethod);

  if (m_bIsCable)
    {// Move Ctrls
    int CtrlIds[] = 
      {
      IDC_ROUTEWIRES,
      IDC_STATICTERMSTRIPS
      };
    for (int i=0; i<sizeof(CtrlIds)/sizeof(CtrlIds[0]); i++)
      GetDlgItem(CtrlIds[i])->EnableWindow(1);
    CString S;
    if (m_SrcTS() && m_DstTS())
      S.Format("%s -> %s", m_SrcTS(), m_DstTS());
    else
      {
      m_AutoWire=false;
      GetDlgItem(IDC_ROUTEWIRES)->EnableWindow(FALSE);
      S.Format("%s -> %s", m_SrcTS()?m_SrcTS():"[N/A]", m_DstTS()?m_DstTS():"[N/A]");
      }
    GetDlgItem(IDC_STATICTERMSTRIPS)->SetWindowText(S);
    }
  else
    {
    GetDlgItem(IDC_ROUTEWIRES)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATICTERMSTRIPS)->SetWindowText("");
    }

  if (!m_bRedraw)
    {
    if (sm_LastConn<0)
      {
      Strng LstMdl;
      LstMdl = PF.RdStr("InsertLink", "$LastInsert", (char*)DefaultLastLink);
      if (LstMdl.Len()>0)
        sm_LastConn = pMdlCB->FindString(-1, LstMdl());
      }
    if (sm_LastConn>=0)
      pMdlCB->SetCurSel(sm_LastConn);
    else
      pMdlCB->SetCurSel(0);
    OnSelchangeModelList();
    }

  UpdateDialogControls(this, FALSE);

  int Left=PF.RdInt("InsertLink", "$Window.Left", -3000);
  int Top=PF.RdInt("InsertLink", "$Window.Top", -3000);
  if (Left>-3000)
    SetVisibleWindowPos(this, Left, Top);
    //SetWindowPos(NULL, Left, Top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
  else
    CenterWindow();

  ShowWindow(SW_SHOW);  
  
  if (!m_bRedraw)
    {
    CEdit* pTag = (CEdit*)GetDlgItem(IDC_MDLTAG);
    char buff[512];

    pTag->GetWindowText(buff, sizeof(buff));
    XStrTrim(buff);
    if (strlen(buff)>0)
      {
      pTag->SetFocus();
      return TRUE;
      }
    return FALSE;
    }

  return TRUE;
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnDestroy() 
  {
  CProfINIFile PF(PrjIniFile());

  bool SaveSettings = false;
  if (m_bRedraw)
    {
    SaveSettings = true;
    }
  else
    {
    CComboBox* pMdlCB = (CComboBox*)GetDlgItem(IDC_MODEL_LIST);
    CString T;
    pMdlCB->GetWindowText(T);
    T.TrimLeft();
    if (T.GetLength()>=0)
      {
      sm_LastConn = pMdlCB->GetCurSel();
      m_Model = T;
      PF.WrStr("InsertLink", "$LastInsert", m_Model);
      PF.WrInt("InsertLink", "$AutoWire", m_AutoWire);
      PF.WrInt("InsertLink", "$DrawMethod", sm_iLinkDrawMethod);
      SaveSettings = true;
      }
    }

  if (SaveSettings)
    {
    PF.WrInt("InsertLink", "$DrawOrth", sm_bDrawOrth);
    PF.WrInt("InsertLink", "$DrawSingleSeg", sm_bDrawSingleSeg);
    PF.WrInt("InsertLink", "$HideTag", m_HideTag);
    PF.WrDouble("InsertLink", "$ArrowScale", m_ArrowScale);
    PF.WrInt("InsertLink", "$ArrowRuleMethod", sm_iArrowRuleMethod);
    PF.WrInt("InsertLink", "$LineWidth", sm_iLineWidth);
    }

  sm_WP.length = sizeof(sm_WP);
  sm_bWPOk = (GetWindowPlacement(&sm_WP)!=0);

  PF.WrInt("InsertLink", "$Window.Left", sm_WP.rcNormalPosition.left);
  PF.WrInt("InsertLink", "$Window.Top", sm_WP.rcNormalPosition.top);

  CDialog::OnDestroy();
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::SetData(double ArrowScale, bool Hide)
  {
  UpdateData(TRUE);
  m_ArrowScale = ArrowScale;
  m_HideTag = Hide;
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::SetPoints(int Points)
  {
  nPoints = Points;
  if (m_hWnd)
    UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnOK()
  {
  UpdateData(TRUE);
  gs_pCmd->ExtendCmdLine("\r");
  }

void CInsertLinkDlg::OnUpdateOKBtn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(nPoints>1);
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnCancel()
  {
  UpdateData(TRUE);
  gs_pCmd->ExtendCmdLine("\x1b\x1b"); //send twice to ensure Cancel works
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::SetTag(char* Tag)
  {
  CEdit* pTag = (CEdit*)GetDlgItem(IDC_MDLTAG);
  pTag->SetWindowText(Tag);
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnBack()
  {
  //gs_pCmd->ExtendCmdLine("\b");
  //gs_pCmd->ExtendCmdLine("\b");//must do again
  gs_pCmd->ExtendCmdLine("Back");
  }

void CInsertLinkDlg::OnUpdateBackBtn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(nPoints>0);
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnOrtho()
  {
  if (!sm_bDrawOrth)
    {
    sm_bDrawOrth = true;
    ((CButton*)GetDlgItem(IDB_ORTHO))->SetBitmap((HBITMAP)OrthoOffBitmap);
    ((CButton*)GetDlgItem(IDB_FREE))->SetBitmap((HBITMAP)FreeBitmap);
    }
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnFree()
  {
  if (sm_bDrawOrth)
    {
    sm_bDrawOrth = false;
    ((CButton*)GetDlgItem(IDB_ORTHO))->SetBitmap((HBITMAP)OrthoBitmap);
    ((CButton*)GetDlgItem(IDB_FREE))->SetBitmap((HBITMAP)FreeOffBitmap);
    }
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnBreak()
  {
  gs_pCmd->ExtendCmdLine("Brk");
  }

void CInsertLinkDlg::OnUpdateBreakBtn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(nPoints>1);
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::CompleteForUse() 
  {
  UpdateData(true);
  CString T;

  if (!m_bRedraw)
    {
    CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_MODEL_LIST);
    pCB->GetWindowText(T);
    T.TrimLeft();
    m_Model = T;

    CEdit * pEd = (CEdit*)GetDlgItem(IDC_MDLTAG);
    pEd->GetWindowText(T);
    T.TrimLeft();
    m_Tag = T;
    }

  m_ArrowScale = Max(0.01, m_ArrowScale);
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnSelchangeDrawMethodList() 
  {
  if (m_bRedraw)
    return;
  CComboBox* pDrwMethCB = (CComboBox*)GetDlgItem(IDC_DRAWMETHOD_LIST);
  int iLstIndex = pDrwMethCB->GetCurSel();
  if (iLstIndex!=m_iPrevSel)
    {
    switch (iLstIndex)
      {
      case 0 : sm_iLinkDrawMethod=0; break;
      case 1 : sm_iLinkDrawMethod=1; sm_bDrawSingleSeg=1; break;
      case 2 : sm_iLinkDrawMethod=1; sm_bDrawSingleSeg=0; break;
      case 3 : sm_iLinkDrawMethod=2; sm_bDrawSingleSeg=1; break;
      case 4 : sm_iLinkDrawMethod=2; sm_bDrawSingleSeg=0; break;
      }
    m_iPrevSel=iLstIndex;
    gs_pCmd->ExtendCmdLine("method");
    }
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnSelchangeArrowRuleList() 
  {
  CComboBox* pArrowRuleCB = (CComboBox*)GetDlgItem(IDC_ARROWRULE_LIST);
  sm_iArrowRuleMethod = pArrowRuleCB->GetCurSel();
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnCbnSelchangeLinkwidth()
  {
  CComboBox* pLineWidthCB = (CComboBox*)GetDlgItem(IDC_LINKWIDTH);
  sm_iLineWidth = pLineWidthCB ->GetCurSel();
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnSelchangeModelList() 
  {
  if (m_bRedraw)
    return;
  CComboBox* pMdlCB = (CComboBox*)GetDlgItem(IDC_MODEL_LIST);
  int iLstIndex = pMdlCB->GetCurSel();
  CString MdlName;
  if (iLstIndex>=0)
    { 
    MdlName=m_MLH.m_ModelClassLst.AtIndex(iLstIndex)->Str();
    m_MLH.Find((LPSTR)(LPCTSTR)MdlName);
    m_ModelName = (m_MLH.m_pModelClass ? m_MLH.m_pModelClass->Str() : "");
    }
  if (m_MLH.m_pDescStr)
    {
    if (m_MLH.m_pBaseTag)
      m_BaseTag = *m_MLH.m_pBaseTag; 
    else
      {                              
      m_BaseTag = "";
      for (int i=0,c=0; i<MdlName.GetLength(); i++)
        if (isupper(MdlName[i]))
          {
          m_BaseTag.SetLength(c+1);
          m_BaseTag[c++] = MdlName[i];
          }
      }
    }
  else
    {
    m_BaseTag = "";
    m_ModelName = "";
    }

  if (!m_bTagChgd)
    {
    CUniqueTagRulesBlk TRB(m_ModelName(), m_BaseTag());
    Strng Tag(TRB.BaseTag()), RetTag;
    Tag += TRB.SepChar();
    Tag += "1";
    if (!gs_pPrj->TestModelTagUnique(Tag(), TRB, RetTag, False))
      {
      Tag = RetTag();
      if (m_BaseTag.Len()==0 && RetTag.XStrRChr('_'))
        {
        m_BaseTag = RetTag();
        m_BaseTag.SetLength(RetTag.XStrRChr('_')-RetTag());
        }
      }

    SetTag(Tag());
    m_bTagChgd = 0;
    }
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnMdldoc() 
  {
  CComboBox* pMdlCB = (CComboBox*)GetDlgItem(IDC_MODEL_LIST);
  char buff[512];
  pMdlCB->GetWindowText(buff, sizeof(buff));
  if (m_pMdl && gs_pPrj)
    gs_pPrj->ModelHelp(buff);
  //ScdMainWnd()->PostMessage(WM_COMMAND, ID_HELP_SYSCAD, 0);
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnChangeMdltag() 
  {
  if (!m_bRedraw)
    {
    CEdit* pTag = (CEdit*)GetDlgItem(IDC_MDLTAG);
    char buff[512];
    pTag->GetWindowText(buff, sizeof(buff));
    XStrTrim(buff);
    m_bTagChgd = strlen(buff)>0;
    if (!m_bTagChgd)
      OnSelchangeModelList();
    }
  }

//---------------------------------------------------------------------------

void CInsertLinkDlg::OnBnClickedOk()
  {
  // TODO: Add your control notification handler code here
  OnOK();
  }

//===========================================================================
//
//
//
//===========================================================================

