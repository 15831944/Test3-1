//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "..\resource.h"
#include "gpwfuncs.h"
#include "tagvdoc.h"
#include "tagvtrnd.h"
#include "tagvtext.h"
#include "tagvsplt.h"
#include "helpinfo.h"
#include "tagvdlgs.h"
#include "scd_wm.h"
#include "msgwnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================

CTrndDlg::CTrndDlg(char* pFirstOption, CWnd* pParent /*=NULL*/)
  : CDialog(CTrndDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CTrndDlg)
  m_XDivs = 0;
  m_YDivs = 0;
  m_ST = FALSE;
  m_SD = FALSE;
  m_ET = FALSE;
  m_ED = FALSE;
  m_Dur = FALSE;
  m_HS = FALSE;
  m_ScrollOpt = -1;
  //}}AFX_DATA_INIT
  sFirstOption = pFirstOption;
  TrendWnds = NULL;
  iPrevSel = -1;
  }

//---------------------------------------------------------------------------

CTrndDlg::~CTrndDlg()
  {
  if (TrendWnds)
    delete []TrendWnds;
  }

//---------------------------------------------------------------------------

void CTrndDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTrndDlg)
  DDX_Text(pDX, IDC_XDIVS, m_XDivs);
  DDX_Text(pDX, IDC_YDIVS, m_YDivs);
  DDX_Check(pDX, IDC_STARTTM, m_ST);
  DDX_Check(pDX, IDC_STARTDT, m_SD);
  DDX_Check(pDX, IDC_ENDTM, m_ET);
  DDX_Check(pDX, IDC_ENDDT, m_ED);
  DDX_Check(pDX, IDC_DURATION, m_Dur);
  DDX_Check(pDX, IDC_HS, m_HS);
  DDX_Radio(pDX, IDC_SCROLLOPT, m_ScrollOpt);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTrndDlg, CDialog)
  //{{AFX_MSG_MAP(CTrndDlg)
  ON_CBN_SELCHANGE(IDC_TRNDSLIST, OnSelchangeTrndslist)
  ON_BN_CLICKED(IDC_APPLYALL, OnApplyall)
  ON_BN_CLICKED(IDC_APPLYIT, OnApply)
  ON_BN_CLICKED(IDC_QUERY, OnQuery)
  //}}AFX_MSG_MAP
  ON_MESSAGE(WMU_COMBOBOXMSG, OnComboBoxMsg)
  ON_UPDATE_COMMAND_UI(IDC_APPLYALL, OnUpdateApplyAll)
  ON_UPDATE_COMMAND_UI(IDC_APPLYIT, OnUpdateApply)
  ON_UPDATE_COMMAND_UI(IDC_QUERY, OnUpdateBtn)
  ON_UPDATE_COMMAND_UI(IDC_STARTDT, OnUpdateFlags)
  ON_UPDATE_COMMAND_UI(IDC_ENDDT, OnUpdateFlags)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_STARTTM, IDC_HS, OnChange)
  ON_CONTROL_RANGE(EN_CHANGE, IDC_XDIVS, IDC_YDIVS, OnChange)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_SCROLLOPT, IDC_REDRAWOPT, OnChange)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

const int MaxStoreTrendWnds = 64;

void CTrndDlg::StoreWndList(CWnd* w)
  {
  if (iWndCnt>=MaxStoreTrendWnds)
    return;
  if (w->IsKindOf(RUNTIME_CLASS(CTagVwSplit)))
    TrendWnds[iWndCnt++] = (CTagVwSplit*)w;

  CWnd* w1 = w->GetTopWindow();
  CWnd* w1x = w1;
  while (w1)
    {
    StoreWndList(w1);
    w1 = w1->GetNextWindow();
    if (w1==w1x) 
      w1 = NULL;
    }
  }

//--------------------------------------------------------------------------

void CTrndDlg::GetTrendWndList()
  {
  iWndCnt = 0;
  if (TrendWnds)
    delete []TrendWnds;
  TrendWnds = new CWnd*[MaxStoreTrendWnds];
  StoreWndList(AfxGetMainWnd());
  m_TrendList.ResetContent();
  int i = 0;
  if (iWndCnt>0)
    {
    for (i=0; i<iWndCnt; i++)
      {
      CString Txt;
      TrendWnds[i]->GetWindowText(Txt);
      if (Txt.Find('.')>=0)
        Txt = Txt.Left(Txt.Find('.'));
      m_TrendList.AddString((const char*)Txt);
      }
    i = 0;
    if (sFirstOption.GetLength()>0)
      {
      i = m_TrendList.FindStringExact(0, (const char*)sFirstOption);
      if (i==CB_ERR)
        i = 0;
      sFirstOption = "";
      }
    }
  m_TrendList.SetCurSel(i);
  OnSelchangeTrndslist();
  m_TrendList.UpdateButtons();
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

BOOL CTrndDlg::OnInitDialog()
  {
  CDialog::OnInitDialog();
  if (!m_TrendList.SubclassPrevNextControls(this, IDC_TRNDSLIST, IDC_PREV, IDC_NEXT))
    TRACE("Failed to subclass combo box.\n");
  GetTrendWndList();
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//--------------------------------------------------------------------------

void CTrndDlg::SavePrev()
  {
  if (iPrevSel>=0)
    {
    pCTagVwDoc pDoc = ((CTagVwSplit*)TrendWnds[iPrevSel])->Doc();
    pDoc->iNXGridDivs = m_XDivs;
    pDoc->iNYGridDivs = m_YDivs;
    if (/*pExec &&*/ !gs_Exec.SyncWithClock())
      {
      pDoc->SD = 0;
      pDoc->ED = 0;
      }
    else
      {
      pDoc->SD = m_SD;
      pDoc->ED = m_ED;
      }
    pDoc->ST = m_ST;
    pDoc->ET = m_ET;
    pDoc->Dur = m_Dur;
    pDoc->HS = m_HS;
    if (m_ScrollOpt==0)
      pDoc->Scroll = 1;
    else
      pDoc->Scroll = 0;
    pDoc->UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay, NULL);
    }
  Changed(False);
  }

//--------------------------------------------------------------------------

void CTrndDlg::OnSelchangeTrndslist() 
  {
  UpdateData(True);
  if (bChanged && iPrevSel>=0 && AfxMessageBox("Apply changes?", MB_YESNO)==IDYES)
    SavePrev();
  iPrevSel = m_TrendList.GetCurSel();
  if (iPrevSel>=0)
    {
    pCTagVwDoc pDoc = ((CTagVwSplit*)TrendWnds[iPrevSel])->Doc();
    m_XDivs = pDoc->iNXGridDivs;
    m_YDivs = pDoc->iNYGridDivs;
    if (gs_Exec.Initialised() && !gs_Exec.SyncWithClock())
      {
      m_SD = False;
      m_ED = False;
      }
    else
      {
      m_SD = pDoc->SD;
      m_ED = pDoc->ED;
      }
    m_ST = pDoc->ST;
    m_ET = pDoc->ET;
    m_Dur = pDoc->Dur;
    m_HS = pDoc->HS;
    if (pDoc->Scroll)
      m_ScrollOpt = 0;
    else
      m_ScrollOpt = 1;
    }
  Changed(False);
  UpdateData(False);
  }

//---------------------------------------------------------------------------

void CTrndDlg::Changed(flag HasChanged)
  {
  bChanged = HasChanged;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CTrndDlg::OnChange(UINT nID) 
  {
  Changed();
  }

//--------------------------------------------------------------------------

void CTrndDlg::OnOK() 
  {
  if (bChanged && iPrevSel>=0 && AfxMessageBox("Apply changes?", MB_YESNO)==IDYES)
    {
    UpdateData(True);
    SavePrev();
    }
  CDialog::OnOK();
  }

//--------------------------------------------------------------------------

void CTrndDlg::OnUpdateApplyAll(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(iWndCnt>1);
  }

//--------------------------------------------------------------------------

void CTrndDlg::OnApplyall() 
  {
  UpdateData(True);
  int OldPrevSel = iPrevSel;
  for (int i=0; i<iWndCnt; i++)
    {
    iPrevSel = i;
    SavePrev();
    }
  iPrevSel = OldPrevSel;
  }

//---------------------------------------------------------------------------

void CTrndDlg::OnUpdateApply(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(bChanged && iPrevSel>=0);
  }

//--------------------------------------------------------------------------

void CTrndDlg::OnApply() 
  {
  if (bChanged && iPrevSel>=0)
    {
    UpdateData(True);
    SavePrev();
    }
  Changed(False);
  }

//---------------------------------------------------------------------------

void CTrndDlg::OnUpdateBtn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(iPrevSel>=0);
  }

//---------------------------------------------------------------------------

void CTrndDlg::OnUpdateFlags(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(gs_Exec.GlblDynamicMode());
//flag Project::GetProBalMode()
  }

//---------------------------------------------------------------------------

void CTrndDlg::OnQuery() 
  {
  if (iPrevSel>=0)
    {
    pCTagVwDoc pDoc = ((CTagVwSplit*)TrendWnds[iPrevSel])->Doc();
    pDoc->TrendQueryDlg();
    }
  }

//---------------------------------------------------------------------------

LRESULT CTrndDlg::OnComboBoxMsg(WPARAM wParam, LPARAM lParam)
  {
  UINT BoxID = (UINT)wParam;
  UINT ButtonID = (UINT)LOWORD(lParam);
  UINT BoxCmd = (UINT)HIWORD(lParam);
  switch (BoxCmd)
    {
    case CBN_SELCHANGE: OnSelchangeTrndslist(); break;
    }
  return True;
  }

//===========================================================================
#if UseTrendInfoDlg

CTrndInfoDlg::CTrndInfoDlg(pCTagVwDoc pDoc_, char* pFirstTag, CWnd* pParent /*=NULL*/)
  : CDialog(CTrndInfoDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CTrndInfoDlg)
  m_Desc = _T("");
  m_Type = _T("");
  m_Eng = _T("");
  m_DispMin = 0.0;
  m_DispMax = 0.0;
  m_Recording = _T("");
  m_TrendTitle = _T("");
  m_Tag = _T("");
  //}}AFX_DATA_INIT
  pDoc = pDoc_;
  sFirstTag = pFirstTag;
  bDidChange = 0;
  bDidTagChange = 0;
  CurSel = -1;
  bHistOn=0;
  bHistLocal=1;
  }

//---------------------------------------------------------------------------

void CTrndInfoDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTrndInfoDlg)
  DDX_Text(pDX, IDC_DESC, m_Desc);
  DDX_Text(pDX, IDC_TYPE, m_Type);
  DDX_Text(pDX, IDC_ENG, m_Eng);
  DDX_Text(pDX, IDC_DISPMIN, m_DispMin);
  DDX_Text(pDX, IDC_DISPMAX, m_DispMax);
  DDX_Text(pDX, IDC_RECORDING, m_Recording);
  DDX_Text(pDX, IDC_TRENDTITLE, m_TrendTitle);
  DDX_Text(pDX, IDC_TAG, m_Tag);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTrndInfoDlg, CDialog)
  //{{AFX_MSG_MAP(CTrndInfoDlg)
  ON_CBN_SELCHANGE(IDC_TAGLIST, OnSelchangeTag)
  ON_BN_CLICKED(IDC_APPLY, OnApply)
  ON_BN_CLICKED(IDC_EDIT, OnEdit)
  ON_EN_CHANGE(IDC_TAG, OnChangeTag)
  //}}AFX_MSG_MAP
  ON_MESSAGE(WMU_COMBOBOXMSG, OnComboBoxMsg)
  ON_UPDATE_COMMAND_UI(IDC_APPLY, OnUpdateApply)
  ON_UPDATE_COMMAND_UI(IDC_EDIT, OnUpdateEdit)
  ON_CONTROL_RANGE(EN_CHANGE, IDC_DISPMIN, IDC_DISPMAX, OnChange)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CTrndInfoDlg::OnInitDialog() 
  {
  CXM_Route HRoute;
  bHistOn = (pDoc->XFindObject(pExecName_Historian, HRoute));
  bHistLocal = !HRoute.AcrossNetwork();

  m_TrendTitle = pDoc->GetTitle();
  CDialog::OnInitDialog();
  if (!m_TagList.SubclassPrevNextControls(this, IDC_TAGLIST, IDC_PREV, IDC_NEXT))
    TRACE("Failed to subclass combo box.\n");
  BuildList();
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------

void CTrndInfoDlg::BuildList()
  {
  m_TagList.ResetContent();
  for (int i=0; i<pDoc->NSlots; i++)
    if (pDoc->DS[i].sTag.Length()>0 && pDoc->DS[i].sTag[0]!='\'')
      m_TagList.AddString(pDoc->DS[i].sTag());
  i = 0;
  if (sFirstTag.GetLength()>0)
    {
    i = m_TagList.FindStringExact(0, (const char*)sFirstTag);
    if (i<0)
      i = 0;
    }
  m_TagList.SetCurSel(i);
  OnSelchangeTag();
  m_TagList.UpdateButtons();
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

LRESULT CTrndInfoDlg::OnComboBoxMsg(WPARAM wParam, LPARAM lParam)
  {
  UINT BoxID = (UINT)wParam;
  UINT ButtonID = (UINT)LOWORD(lParam);
  UINT BoxCmd = (UINT)HIWORD(lParam);
  switch (BoxCmd)
    {
    case CBN_SELCHANGE: OnSelchangeTag(); break;
    }
  return True;
  }

//---------------------------------------------------------------------------

void CTrndInfoDlg::OnSelchangeTag() 
  {
  UpdateData(True);
  if (bChanged && CurSel>=0 && AfxMessageBox("Apply changes?", MB_YESNO)==IDYES)
    OnApply();
  CurSel = m_TagList.GetCurSel();
  if (CurSel>=0)
    {
    CString Txt;
    m_TagList.GetLBText(CurSel, Txt);
    FindTrendTagHelper Pos((pchar)(const char*)Txt);
    int Index = pDoc->FindTrendTag(Pos);
    if (Index>=0)
      {
    /*char           cType;
    Strng          sTag, sDesc;
    Strng          sMinVal, sMaxVal, sValue;
    long           nTrendNo;
    LOGBRUSH       Brush;
    DWORD          dwWidth;
    StyleBlk       Style;
    flag           bTagValid:1,
                   bValueValid:1,
                   bChgd:1,
                   bHex:1,
                   bRecorded:1;
    int            iNextChgdFld;
    CCnvIndex      iCnv;
    Strng          sCnv;*/
      m_Desc = pDoc->DS[Index].sDesc();
      m_Eng = pDoc->DS[Index].Cnv.Text();
      m_Type = tt_TypeString(pDoc->DS[Index].cType);
      m_Recording = (pDoc->DS[Index].bRecorded ? "Yes" : "No");
      m_DispMin = (pDoc->DS[Index].sMinVal.Length()>0 ? atof(pDoc->DS[Index].sMinVal()) : 0.0);
      m_DispMax = Max(m_DispMin, (pDoc->DS[Index].sMaxVal.Length()>0 ? atof(pDoc->DS[Index].sMaxVal()) : 0.0));
      m_Tag = pDoc->DS[Index].Tag();
      //bHistExists = 
      CXM_Route HRoute;
      bHistOn = (pDoc->XFindObject(pExecName_Historian, HRoute));
      if (bHistOn)
        {
        Strng WrkTag, WrkCnvTxt;
        TaggedObject::SplitTagCnv(pDoc->DS[Index].sTag(), WrkTag, WrkCnvTxt);
        CXM_HistoryExists xb(WrkTag());
        CXMsgLst XM;
        XM.PackMsg(xb);
        DWORD RetCode = pDoc->XSendMessage(XM, HRoute);
        bHistExists = ((RetCode & RC_SM_HistExists)!=0);
        //bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
        }
      }
    }
  Changed(False);
  UpdateData(False);
  }

//---------------------------------------------------------------------------

void CTrndInfoDlg::Changed(flag HasChanged)
  {
  bChanged = HasChanged;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CTrndInfoDlg::OnChange(UINT nID) 
  {
  Changed();
  }

//---------------------------------------------------------------------------

void CTrndInfoDlg::OnUpdateApply(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(bChanged && CurSel>=0);
  }

//---------------------------------------------------------------------------

void CTrndInfoDlg::OnApply() 
  {
  if (bChanged && CurSel>=0)
    {
    UpdateData(True);
    CString Txt;
    m_TagList.GetLBText(CurSel, Txt);
    FindTrendTagHelper Pos((pchar)(const char*)Txt);
    int Index = pDoc->FindTrendTag(Pos);
    if (Index>=0)
      {
      m_DispMax = Max(m_DispMin, m_DispMax);
      pDoc->DS[Index].sMinVal.Set("%g", m_DispMin);
      pDoc->DS[Index].sMaxVal.Set("%g", m_DispMax);
      bDidChange = 1;
      if (stricmp(pDoc->DS[Index].Tag(), (const char*)m_Tag)!=0)
        {
        pDoc->DS[Index].SetTag((char*)(const char*)m_Tag);
        bDidTagChange = 1;
        Changed(False);
        m_TagList.DeleteString(CurSel);
        m_TagList.AddString((const char*)m_Tag);
        CurSel = m_TagList.FindStringExact(-1, (const char*)m_Tag);
        m_TagList.SetCurSel(CurSel);
        OnSelchangeTag();
        m_TagList.UpdateButtons();
        }
      }
    }
  Changed(False);
  }

//---------------------------------------------------------------------------

void CTrndInfoDlg::OnUpdateEdit(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(bHistOn && bHistLocal && bHistExists && m_TagList.GetCurSel()>=0);
  }

//---------------------------------------------------------------------------

void CTrndInfoDlg::OnEdit() 
  {
  CXM_Route HRoute;
  bHistOn = (pDoc->XFindObject(pExecName_Historian, HRoute));

  ASSERT(CurSel==m_TagList.GetCurSel());
  if (bHistOn && bHistExists && CurSel>=0)
    {
    CString Txt;
    m_TagList.GetLBText(CurSel, Txt);
    FindTrendTagHelper Pos((pchar)(const char*)Txt);
    int Index = pDoc->FindTrendTag(Pos);
    if (Index>=0)
      {
      Strng WrkTag, WrkCnvTxt;
      TaggedObject::SplitTagCnv(pDoc->DS[Index].sTag(), WrkTag, WrkCnvTxt);
      CXM_HistorySlotDlg xb(WrkTag(), 1);
      CXMsgLst XM;
      XM.PackMsg(xb);
      pDoc->XSendMessage(XM, HRoute);
      OnSelchangeTag();
      }
    }
  }

//---------------------------------------------------------------------------

void CTrndInfoDlg::OnOK() 
  {
  if (bChanged && CurSel>=0 && AfxMessageBox("Apply changes?", MB_YESNO)==IDYES)
    OnApply();
  CDialog::OnOK();
  }

//---------------------------------------------------------------------------

void CTrndInfoDlg::OnChangeTag() 
  {
  Changed();
  }

//---------------------------------------------------------------------------
#endif
//===========================================================================
#if UseNewTrendDlg

CNewTrndSlotDlg::CNewTrndSlotDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CNewTrndSlotDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CNewTrndSlotDlg)
  m_Tag = _T("");
  m_Desc = _T("");
  m_Type = _T("");
  m_Eng = _T("");
  m_DispMin = 0.0;
  m_DispMax = 0.0;
  m_RecordIt = FALSE;
  //}}AFX_DATA_INIT
  bHistOn = 0;
  }

//---------------------------------------------------------------------------

void CNewTrndSlotDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CNewTrndSlotDlg)
  DDX_Text(pDX, IDC_TAG, m_Tag);
  DDX_Text(pDX, IDC_DESC, m_Desc);
  DDX_Text(pDX, IDC_TYPE, m_Type);
  DDX_Text(pDX, IDC_ENG, m_Eng);
  DDX_Text(pDX, IDC_DISPMIN, m_DispMin);
  DDX_Text(pDX, IDC_DISPMAX, m_DispMax);
  DDX_Check(pDX, IDC_RECORDIT, m_RecordIt);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CNewTrndSlotDlg, CDialog)
  //{{AFX_MSG_MAP(CNewTrndSlotDlg)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDC_RECORDIT, OnUpdateRecordIt)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CNewTrndSlotDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  UpdateDialogControls(this, FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------

void CNewTrndSlotDlg::OnUpdateRecordIt(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(bHistOn);
  }

#endif
//===========================================================================

CTBaseSetDlg::CTBaseSetDlg(pCTagVwDoc pDoc_, CWnd* pParent /*=NULL*/)
  : CDialog(CTBaseSetDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CTBaseSetDlg)
  m_Duration = "";
  m_Start = "";
  m_TrendTitle = _T("");
  //}}AFX_DATA_INIT
  pDoc = pDoc_;
  }

//---------------------------------------------------------------------------

CTBaseSetDlg::~CTBaseSetDlg()
  {
  delete pTTC;
  }

//---------------------------------------------------------------------------

void CTBaseSetDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTBaseSetDlg)
  DDX_Text(pDX, IDC_EDITDURATION, m_Duration);
  DDX_Text(pDX, IDC_EDITSTART, m_Start);
  DDX_Text(pDX, IDC_TRENDTITLE, m_TrendTitle);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTBaseSetDlg, CDialog)
  //{{AFX_MSG_MAP(CTBaseSetDlg)
  ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
  //}}AFX_MSG_MAP
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CTBaseSetDlg::OnInitDialog()
  {
  pTTC = new CCustomToolTipCtrl(this);
  char buff[256];
  SecstoHMSDate(pDoc->TimeBaseStart(), buff);
  m_Start = buff;
  SecstoHMSCnv(pDoc->TimeBaseDuration(), buff);
  m_Duration = buff;
  m_TrendTitle = pDoc->GetTitle();
  CDialog::OnInitDialog();
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CTBaseSetDlg::PreTranslateMessage(MSG* pMsg) 
  {
  if (pTTC && HelpMngr.ShowToolTips())
    pTTC->RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CTBaseSetDlg::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
  if (pTTC && HelpMngr.ShowToolTips())
    return pTTC->OnToolTipNotify(pNMHDR, CTBaseSetDlg::IDD);
  return FALSE;
  }

//---------------------------------------------------------------------------

void CTBaseSetDlg::OnOK()
  {
  UpdateData(TRUE);
  int i = 0;
  double ts, td;
  flag OK1 = HMSDatetoSecs(m_Start.GetBuffer(0), ts);
  flag OK2 = HMStoSecs(m_Duration.GetBuffer(0), td);
  if (OK1 && OK2)
    {
    pDoc->SetTimebase(ts, td, true);
    CDialog::OnOK();
    }
  else 
    AfxMessageBox("Invalid Time", MB_ICONEXCLAMATION|MB_OK); 
  }

//---------------------------------------------------------------------------

void CTBaseSetDlg::OnOptions() 
  {
  CString s = pDoc->GetTitle();
  if (s.Find('.')>=0)
    s = s.Left(s.Find('.'));
  CTrndDlg Dlg((char*)(const char*)s, this);
  Dlg.DoModal();
  }

//---------------------------------------------------------------------------
//===========================================================================

CTrndInfo* CTrndInfo::pTheTrndInfo=NULL;

//---------------------------------------------------------------------------

CTrndInfo::CTrndInfo(CWnd* pParent /*=NULL*/)
  : CDialog(CTrndInfo::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CTrndInfo)
  m_Tag = _T("");
  m_Time = _T("");
  m_Value = _T("");
	m_Range = _T("");
	m_Desc = _T("");
	//}}AFX_DATA_INIT

  Create(CTrndInfo::IDD, pParent); // NonModal Create
  }

//---------------------------------------------------------------------------

void CTrndInfo::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTrndInfo)
  DDX_Text(pDX, IDC_TAG, m_Tag);
  DDX_Text(pDX, IDC_TIME, m_Time);
  DDX_Text(pDX, IDC_VALUE, m_Value);
	DDX_Text(pDX, IDC_VALUERANGE, m_Range);
	DDX_Text(pDX, IDC_DESC, m_Desc);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTrndInfo, CDialog)
  //{{AFX_MSG_MAP(CTrndInfo)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CTrndInfo::Open(CWnd* pParent, POINT pt, const char * pTrendName)
  {
  //1/11/96 TrendName no longer used
  if (pTheTrndInfo==NULL)
    pTheTrndInfo=new CTrndInfo;
  
  ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CTagVwTrend)));

  WINDOWPLACEMENT WP;
  WP.length=sizeof(WP);

  if (pTheTrndInfo && ::IsWindow(pTheTrndInfo->m_hWnd))
    {
    if (pTheTrndInfo->GetWindowPlacement(&WP))
      {
      POINT Mv;
      RECT R;
      long w=WP.rcNormalPosition.right-WP.rcNormalPosition.left;

      pParent->GetWindowRect(&R);

      Mv.x=R.left-WP.rcNormalPosition.left+pt.x-w/2;
      if ((WP.rcNormalPosition.left+Mv.x)<R.left)
        Mv.x=R.left-WP.rcNormalPosition.left;
      else if ((WP.rcNormalPosition.right+Mv.x)>R.right)
        Mv.x=R.right-WP.rcNormalPosition.right;

      Mv.y=R.top -WP.rcNormalPosition.bottom;
      if ((WP.rcNormalPosition.top+Mv.y)<0)
        Mv.y=0-WP.rcNormalPosition.top;
      
      WP.rcNormalPosition.left+=Mv.x;
      WP.rcNormalPosition.right+=Mv.x;
      WP.rcNormalPosition.top+=Mv.y;
      WP.rcNormalPosition.bottom+=Mv.y;
      WP.showCmd=SW_SHOWNOACTIVATE;
      WP.length=sizeof(WP);
      pTheTrndInfo->SetWindowPlacement(&WP);
      }
    pTheTrndInfo->UpdateWindow();
    }
  }

//---------------------------------------------------------------------------

void CTrndInfo::Close(UINT CloseDelay/*=0*/)
  {
  if (pTheTrndInfo && ::IsWindow(pTheTrndInfo->m_hWnd))
    pTheTrndInfo->ShowWindow(SW_HIDE);
  }

//---------------------------------------------------------------------------

void CTrndInfo::ShutDown()
  {
  if (pTheTrndInfo)
    {
    pTheTrndInfo->DestroyWindow();
    delete pTheTrndInfo;
    }
  pTheTrndInfo=NULL;
  }

//---------------------------------------------------------------------------

void CTrndInfo::SetCurrentData(const char * pTime, const char * pTag, const char* pDesc, const char* pValue, const char* pRange)
  {
  if (pTheTrndInfo)
    {
    pTheTrndInfo->m_Tag=pTag;
    pTheTrndInfo->m_Time=pTime;
    pTheTrndInfo->m_Value=pValue;
    pTheTrndInfo->m_Range=pRange;
    pTheTrndInfo->m_Desc=pDesc;
    pTheTrndInfo->UpdateData(False);
    }
  }

//---------------------------------------------------------------------------
//===========================================================================

CTrendPrintDlg::CTrendPrintDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrendPrintDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CTrendPrintDlg)
	m_PrintTrendLine = TRUE;
	m_PrintTrendText = TRUE;
	m_TrueColors = FALSE;
	//}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CTrendPrintDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTrendPrintDlg)
	DDX_Check(pDX, IDC_TRNDLINE_PRINT, m_PrintTrendLine);
	DDX_Check(pDX, IDC_TRNDTEXT_PRINT, m_PrintTrendText);
	DDX_Check(pDX, IDC_TRND_COLORS, m_TrueColors);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTrendPrintDlg, CDialog)
	//{{AFX_MSG_MAP(CTrendPrintDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
//===========================================================================
