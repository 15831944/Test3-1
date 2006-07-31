//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "resource.h"
#include "drv_base.h"
#include "drv_dlgs.h"
#include "drv_mngr.h"
#include "project.h"
#include "scd_wm.h"
#include "msgwnd.h"

#if WITHDRVMAN
//===========================================================================

extern CDriverSheet* pDrvSheet = NULL;

//===========================================================================

IMPLEMENT_DYNAMIC(CDriverSheet, CPropertySheet)

CDriverSheet::CDriverSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
  : CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
  {
  ASSERT(FALSE);
  bInited=false;
  }

//---------------------------------------------------------------------------

CDriverSheet::CDriverSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
  : CPropertySheet(pszCaption, pParentWnd, iSelectPage)
  {
  bInited=false;
  }

//---------------------------------------------------------------------------

CDriverSheet::~CDriverSheet()
  {
  pDrvSheet = NULL; //set global pointer to property sheet to null
  //dialog just closed, ensure main SysCAD window becomes active ...
  AfxGetMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_UPDATE, 0);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDriverSheet, CPropertySheet)
  //{{AFX_MSG_MAP(CDriverSheet)
  ON_WM_NCDESTROY()
  ON_WM_CLOSE()
  ON_COMMAND(IDOK,OnOK)
  ON_COMMAND(IDCANCEL,OnCancel)
  //}}AFX_MSG_MAP
  ON_MESSAGE(WMU_DRVUPDATESTATS, OnUpdateStats)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CDriverSheet::OnInitDialog() 
  {
  ASSERT(m_bModeless); //this code only needed when creating modeless property sheet!
  
  PROPSHEETTRICK0()
  BOOL b = CPropertySheet::OnInitDialog();
  PROPSHEETTRICK1()

  GetDlgItem(ID_APPLY_NOW)->ShowWindow(SW_HIDE);
  GetDlgItem(IDHELP)->ShowWindow(SW_HIDE);
  bInited=true;
  return b;
  }

//---------------------------------------------------------------------------

void CDriverSheet::OnOK() 
  {
  ASSERT_VALID(this);
  //StoreCurPageNo();
  for (int i=0; i<GetPageCount(); i++)
    GetPage(i)->OnOK();
  if (GetActivePage()->OnKillActive())
    {
    //GetActivePage()->OnOK();
    DeletePages();
    EndDialog(IDOK);
    }
  else
    DeletePages();
  }

//---------------------------------------------------------------------------

void CDriverSheet::OnCancel() 
  {
  ASSERT_VALID(this);
  //StoreCurPageNo();
  for (int i=0; i<GetPageCount(); i++)
    GetPage(i)->OnCancel();
  DeletePages();
  EndDialog(IDCANCEL);
  }

//---------------------------------------------------------------------------

void CDriverSheet::OnClose()
  {
  ASSERT_VALID(this);
  OnCancel();
  }

//---------------------------------------------------------------------------

void CDriverSheet::OnNcDestroy() 
  {
  CPropertySheet::OnNcDestroy();
  delete this;
  }

//---------------------------------------------------------------------------

void CDriverSheet::DeletePages()
  {
  for (int i=0; i<GetPageCount(); i++)
    delete GetPage(i);
  }

//---------------------------------------------------------------------------

void CDriverSheet::UpdateStats()
  {
  if (bInited)
    {
    PostMessage(WMU_DRVUPDATESTATS, (WPARAM)0, (LPARAM)0);
    CPropertyPage* p = GetPage(0);
    if (p && p->GetSafeHwnd())
      p->PostMessage(WMU_DRVUPDATEBTNS, (WPARAM)1, (LPARAM)0);
    }
  }

//---------------------------------------------------------------------------

void CDriverSheet::Stop()
  {
  if (bInited)
    {
    CPropertyPage* p = GetPage(0);
    if (p && p->GetSafeHwnd())
      p->PostMessage(WMU_DRVUPDATEBTNS, (WPARAM)0, (LPARAM)0);
    }
  }

// --------------------------------------------------------------------------

LRESULT CDriverSheet::OnUpdateStats(WPARAM wParam, LPARAM lParam)
  {
  if (wParam)
    SetActivePage(1);
  CPropertyPage* p = GetActivePage();
  if (p && p->GetSafeHwnd())
    p->SendMessage(WMU_DRVUPDATESTATS, wParam, lParam);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CDriverSheet::DisplayTag(char* pGotoTag)
  {
  if (bInited && pGotoTag)
    {
    char* pTxt = new char[strlen(pGotoTag)+1];
    strcpy(pTxt, pGotoTag);
    PostMessage(WMU_DRVUPDATESTATS, (WPARAM)1, (LPARAM)pTxt);
    }
//  SetActivePage(1);
//  CPropertyPage* p = GetActivePage();
//  if (p && pGotoTag)
//    {
//    char* pTxt = new char[strlen(pGotoTag)+1];
//    strcpy(pTxt, pGotoTag);
//    p->PostMessage(WMU_DRVUPDATESTATS, (WPARAM)0, (LPARAM)pTxt);
//    }
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CSlotsPage, CPropertyPage)

CSlotsPage::CSlotsPage() : CPropertyPage(CSlotsPage::IDD)
  {
  bDidInit = False;
  //{{AFX_DATA_INIT(CSlotsPage)
  m_DrvDesc = _T("");
  m_SlotDesc = _T("");
  m_Desc = _T("");
  m_IO = _T("");
  m_IgnoreRead = FALSE;
  m_IgnoreWrite = FALSE;
  m_ForceValue = 0.0;
  m_EnableForces = FALSE;
  m_RawValue = _T("");
  m_Connects = _T("");
  m_Value = _T("");
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CSlotsPage::~CSlotsPage()
  {
  }

//---------------------------------------------------------------------------

void CSlotsPage::DoDataExchange(CDataExchange* pDX)
  {
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSlotsPage)
  DDX_Control(pDX, IDC_SLOTCOMBO, m_SlotList);
  DDX_Text(pDX, IDC_DRVDESC, m_DrvDesc);
  DDX_Text(pDX, IDC_SLOTDESC, m_SlotDesc);
  DDX_Text(pDX, IDC_DESC, m_Desc);
  DDX_Text(pDX, IDC_IO, m_IO);
  DDX_Check(pDX, IDC_IGNOREREAD, m_IgnoreRead);
  DDX_Check(pDX, IDC_IGNOREWRITE, m_IgnoreWrite);
  DDX_Text(pDX, IDC_FORCEVALUE, m_ForceValue);
  DDX_Check(pDX, IDC_ENABLEFORCES, m_EnableForces);
  DDX_Text(pDX, IDC_RAWVALUE, m_RawValue);
  DDX_Text(pDX, IDC_CONNECT, m_Connects);
  DDX_Text(pDX, IDC_VALUE, m_Value);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSlotsPage, CPropertyPage)
  //{{AFX_MSG_MAP(CSlotsPage)
  ON_CBN_SELCHANGE(IDC_SLOTCOMBO, OnSelchangeSlotcombo)
  ON_BN_CLICKED(IDC_APPLYBTN, OnApplybtn)
  ON_BN_CLICKED(IDC_IGNOREREAD, OnIgnoreread)
  ON_BN_CLICKED(IDC_IGNOREWRITE, OnIgnorewrite)
  ON_BN_CLICKED(IDC_ENABLEFORCES, OnEnableforces)
  ON_EN_CHANGE(IDC_FORCEVALUE, OnChangeForcevalue)
	ON_BN_CLICKED(IDC_DRVSLOTSREFRESHIO, OnDrvslotsRefreshIO)
	ON_BN_CLICKED(IDC_DRVSLOTSREFRESHCONNS, OnDrvslotsRefreshConns)
	//}}AFX_MSG_MAP
  ON_MESSAGE(WMU_DRVUPDATESTATS, OnUpdateStats)
  ON_UPDATE_COMMAND_UI(IDC_APPLYBTN, OnUpdateApplyBtn)
  ON_UPDATE_COMMAND_UI(IDC_IGNOREREAD, OnUpdateIgnoreRead)
  ON_UPDATE_COMMAND_UI(IDC_IGNOREWRITE, OnUpdateIgnoreWrite)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CSlotsPage::OnInitDialog() 
  {
  CPropertyPage::OnInitDialog();
  bDidInit = True;
  bChanged = False;
  RefreshSlotList();
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

// --------------------------------------------------------------------------

void CSlotsPage::OnOK()
  {
  if (bDidInit)
    {
    if (bChanged && AfxMessageBox("Apply changes?", MB_YESNO)==IDYES)
      OnApplybtn();
    if (CurSlotNo>=0)
      {
      CDrvSlot& S = *(gs_pDrvMan->Slots[CurSlotNo]);
      CProfINIFile PF(PrjIniFile());
      PF.WrStr("DrvMngr", "PrevSlot", S.sTag());
      }
    }
  }

// --------------------------------------------------------------------------

void CSlotsPage::OnCancel()
  {
  if (bDidInit)
    {
    if (CurSlotNo>=0)
      {
      CDrvSlot& S = *(gs_pDrvMan->Slots[CurSlotNo]);
      CProfINIFile PF(PrjIniFile());
      PF.WrStr("DrvMngr", "PrevSlot", S.sTag());
      }
    }
  }

// --------------------------------------------------------------------------

void CSlotsPage::RefreshSlotList()
  {
  m_SlotList.ResetContent();
  for (long i=0; i<gs_pDrvMan->Slots.GetSize(); i++)
    {
    CDrvSlot& S = *(gs_pDrvMan->Slots[i]);
    m_SlotList.AddString(S.sTag());
    }
  int Sel = 0;
  CProfINIFile PF(PrjIniFile());
  Strng sPrevSlotTag = PF.RdStr("DrvMngr", "PrevSlot", "");
  if (sPrevSlotTag.Length()>0)
    {
    int j = m_SlotList.FindStringExact(-1, sPrevSlotTag());
    if (j>=0)
      Sel = j;
    }
  m_SlotList.SetCurSel(Sel);
  UpdateData(FALSE);
  OnSelchangeSlotcombo();
  }

//---------------------------------------------------------------------------

void CSlotsPage::OnSelchangeSlotcombo() 
  {
  char Buff[256];
  if (bChanged)
    {
    if (CurSlotNo>=0)
      sprintf(Buff, "Apply changes for %s?", gs_pDrvMan->Slots[CurSlotNo]->sTag());
    else
      sprintf(Buff, "Apply changes?");
    if (AfxMessageBox(Buff, MB_YESNO)==IDYES)
      OnApplybtn();
    }
  UpdateData(TRUE);
  m_DrvDesc = "";
  m_SlotDesc = "";
  m_Connects = "";
  m_Value = "";
  int i = m_SlotList.GetCurSel();
  if (i!=CB_ERR)
    {
    CString Txt;
    m_SlotList.GetLBText(i, Txt);
    CurSlotNo = gs_pDrvMan->FindChanForTag((char*)(const char*)Txt);
    CDrvSlot& S = *(gs_pDrvMan->Slots[CurSlotNo]);
    int Base,WordWidth;
    S.pDrv->BitBase(Base, WordWidth);
    S.GetDrvDesc(m_DrvDesc);
    m_SlotDesc = tt_TypeString(S.iTyp);
    if (S.pSpan)
      {
      if (!IsFloatData(S.iTyp))//!=tt_float)
        sprintf(Buff, " Bit(%d)", S.pSpan->Offset+Base);
      else 
        sprintf(Buff, " Span(%d,%d)", S.pSpan->Span, S.pSpan->Offset);
      m_SlotDesc += Buff;
      }
    m_SlotDesc += " ";
    m_SlotDesc += S.m_sCnvTxt();
    if (S.pRange && S.iTyp!=tt_Bool)
      {
      sprintf(Buff, " Range(%.3f to %.3f)", S.pRange->RngLo, S.pRange->RngHi);
      m_SlotDesc += Buff;
      } 
    if (S.m_pValLst)
      {
      m_SlotDesc += " {";
      Strng X;
      S.m_pValLst->SaveToCSV(X, true);
      m_SlotDesc += X();
      m_SlotDesc += "}";
      }

    m_Desc = S.sDescription();
    m_Connects = S.sConnection();
    if ((S.iAction & XIO_Out) && (S.iAction & XIO_In))
      m_IO = "R/W";
    else if ((S.iAction & XIO_Out))
      m_IO = "R";
    else if (S.iAction & XIO_In)
      m_IO = "W";
    else 
      m_IO = "";
    m_IgnoreRead = S.bIgnoreReads;
    m_IgnoreWrite = S.bIgnoreWrites;
    m_EnableForces = S.bEnableForces;
    m_ForceValue = Cnvs[S.m_iCnv]->Human(S.dForceVal, S.m_sCnvTxt());
    PkDataUnion Pk;
    S.GetValue(Pk);
int xx1=0;
    short iCnv= (S.m_pConnects ? S.m_pConnects->m_iCnv : 0);
int xx2=0;
    LPTSTR pCnv= (S.m_pConnects ? S.m_pConnects->m_sCnvTxt() : "");
int xx3=0;
    if (pCnv==NULL) pCnv="";
    if (IsFloatData(S.iTyp))
      m_Value.Format("%g %s", Pk.GetDouble(iCnv, pCnv), pCnv);
    else
      m_Value.Format("%i %s", Pk.GetLong(iCnv, pCnv), pCnv);
    }
  Change(False);
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CSlotsPage::Change(flag chg)
  {
  bChanged = chg;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CSlotsPage::OnUpdateApplyBtn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(bChanged);
  }

//---------------------------------------------------------------------------

void CSlotsPage::OnApplybtn() 
  {
  if (bChanged)
    {
    UpdateData(True);
    if (CurSlotNo>=0)
      {
      CDrvSlot& S = *(gs_pDrvMan->Slots[CurSlotNo]);
      S.bIgnoreReads = m_IgnoreRead;
      S.bIgnoreWrites = m_IgnoreWrite;
      double NewForceVal = Cnvs[S.m_iCnv]->Normal(m_ForceValue, S.m_sCnvTxt());;
      flag DoForcesNow = ((flag)S.bEnableForces!=(flag)m_EnableForces || S.dForceVal!=NewForceVal);
      S.bEnableForces = m_EnableForces;
      S.dForceVal = NewForceVal;
      if (DoForcesNow)
        gs_pDrvMan->ForceSlot(S);
      CProfINIFile PF(PrjIniFile());
      PF.WrStr("DrvMngr", "PrevSlot", S.sTag());
      }
    Change(False);
    }
  }

//---------------------------------------------------------------------------

void CSlotsPage::OnUpdateIgnoreRead(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(CurSlotNo>=0 && gs_pDrvMan->Slots[CurSlotNo]->iAction & XIO_Out);
  }

//---------------------------------------------------------------------------

void CSlotsPage::OnIgnoreread() 
  {
  Change();
  }

//---------------------------------------------------------------------------

void CSlotsPage::OnUpdateIgnoreWrite(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(CurSlotNo>=0 && gs_pDrvMan->Slots[CurSlotNo]->iAction & XIO_In);
  }

//---------------------------------------------------------------------------

void CSlotsPage::OnIgnorewrite() 
  {
  Change();
  }

//---------------------------------------------------------------------------

void CSlotsPage::OnEnableforces() 
  {
  Change();
  }

//---------------------------------------------------------------------------

void CSlotsPage::OnChangeForcevalue() 
  {
  Change();
  }

// --------------------------------------------------------------------------

LRESULT CSlotsPage::OnUpdateStats(WPARAM wParam, LPARAM lParam)
  {
  if (bDidInit)
    {
    if (lParam)
      {
      char* pTxt = (char*)lParam;
      if (bChanged)
        {
        char Buff[256];
        if (CurSlotNo>=0)
          sprintf(Buff, "Apply changes for %s?", gs_pDrvMan->Slots[CurSlotNo]->sTag());
        else
          sprintf(Buff, "Apply changes?");
        if (AfxMessageBox(Buff, MB_YESNO)==IDYES)
          OnApplybtn();
        }
      int i = m_SlotList.FindStringExact(-1, pTxt);
      if (i>=0)
        {
        i = m_SlotList.SetCurSel(i);
        if (i>=0)
          OnSelchangeSlotcombo();
        }
      delete []pTxt;
      }
    if (CurSlotNo>=0)
      {
      CDrvSlot& S = *(gs_pDrvMan->Slots[CurSlotNo]);
      Strng ss,BinStr;
      if (IsFloatData(S.iTyp))
        {
        const double Raw = S.GetRawValueDble();
        ss.Set("%d   0x%04X   %g", (long)Raw, (long)Raw, Raw);
        }
      else
        {
        const long Raw = S.GetRawValueLong();
        ss.Set("%d   0x%04X   %s", Raw, Raw, BinStr.MakeBinary(Raw, 16, 4));
        }

      m_RawValue = ss();
      if (S.m_pConnects && (!gs_pDrvMan->bIgnoresOn || !S.bIgnoreReads))
        {
        short iCnv= S.m_pConnects->m_iCnv;
        LPTSTR pCnv= S.m_pConnects->m_sCnvTxt();
        if (pCnv==NULL) pCnv="";

        if (gs_pDrvMan->bForcesOn && S.bEnableForces)
          m_Value.Format("%g %s", Cnvs[S.m_iCnv]->Human(S.dForceVal, S.m_sCnvTxt()), pCnv);
        else
          {
          m_Value = "";//-999.999;
          CXM_ObjectData Data(CurSlotNo);
          CPkDataItem * pPItem = Data.List.FirstItem();
          CPkDataItem * pPrevItem = pPItem;
          if (S.GetTagValue(Data.List, pPItem, False))
            m_Value.Format("%g %s", Cnvs[S.m_iCnv]->Human(pPrevItem->Value()->GetDouble(), S.m_sCnvTxt()), pCnv);
          }
        }
      CDataExchange DX(this, False);
      DDX_Text(&DX, IDC_VALUE, m_Value); //update one control only
      DDX_Text(&DX, IDC_RAWVALUE, m_RawValue); //update one control only
      }
    }
  return TRUE;
  }

//---------------------------------------------------------------------------

void CSlotsPage::OnDrvslotsRefreshIO() 
  {
	gs_pDrvMan->RefreshAllSlotData(true, true);
  }

//---------------------------------------------------------------------------

void CSlotsPage::OnDrvslotsRefreshConns() 
  {
	gs_pDrvMan->RefreshConnections();
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CDriverPage, CPropertyPage)

CDriverPage::CDriverPage() : CPropertyPage(CDriverPage::IDD)
  {
  bDidInit = False;
  //{{AFX_DATA_INIT(CDriverPage)
  m_ForcesOn = FALSE;
  m_IgnoresOn = FALSE;
  m_NoDrivers = 0;
  m_NoSlots = 0;
	m_NoiseOn = TRUE;
	//}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CDriverPage::~CDriverPage()
  {
  }

//---------------------------------------------------------------------------

void CDriverPage::DoDataExchange(CDataExchange* pDX)
  {
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDriverPage)
	DDX_Control(pDX, IDC_DRVSTARTREFRESH, m_StartRefresh);
  DDX_Check(pDX, IDC_DRVFORCESON, m_ForcesOn);
  DDX_Check(pDX, IDC_DRVIGNORESON, m_IgnoresOn);
  DDX_Text(pDX, IDC_NODRIVERS, m_NoDrivers);
  DDX_Text(pDX, IDC_NOSLOTS, m_NoSlots);
	DDX_Check(pDX, IDC_DRVNOISEON, m_NoiseOn);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDriverPage, CPropertyPage)
  //{{AFX_MSG_MAP(CDriverPage)
  ON_BN_CLICKED(IDC_APPLYBTN, OnApplybtn)
  ON_BN_CLICKED(IDC_DRVFORCESON, OnDrvforceson)
  ON_BN_CLICKED(IDC_DRVIGNORESON, OnDrvignoreson)
  ON_BN_CLICKED(IDC_FORCEALL, OnForceall)
  ON_BN_CLICKED(IDC_FORCENONE, OnForcenone)
  ON_BN_CLICKED(IDC_RELOADBTN, OnReloadbtn)
	ON_BN_CLICKED(IDC_DRVNOISEON, OnDrvnoiseon)
	ON_CBN_SELCHANGE(IDC_DRVSTARTREFRESH, OnSelchangeDrvstartrefresh)
	ON_BN_CLICKED(IDC_DRVSLOTSREFRESHIO, OnDrvslotsRefreshIO)
	ON_BN_CLICKED(IDC_DRVSLOTSREFRESHIO_IN, OnDrvslotsRefreshIO_In)
	ON_BN_CLICKED(IDC_DRVSLOTSREFRESHIO_OUT, OnDrvslotsRefreshIO_Out)
	ON_BN_CLICKED(IDC_DRVSLOTSREFRESHCONNS, OnDrvslotsRefreshConns)
  ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDC_APPLYBTN, OnUpdateApplyBtn)
  ON_MESSAGE(WMU_DRVUPDATEBTNS, OnUpdateBtns)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CDriverPage::OnInitDialog() 
  {
  CPropertyPage::OnInitDialog();
  bDidInit = True;
  bChanged = False;
  m_IgnoresOn = gs_pDrvMan->bIgnoresOn;
  m_ForcesOn = gs_pDrvMan->bForcesOn;
  m_NoiseOn = gs_pDrvMan->bNoiseOn;
  m_NoDrivers = gs_pDrvMan->Drivers.InUse();
  m_NoSlots = gs_pDrvMan->Slots.GetSize();

  m_StartRefresh.AddString("Never");
  m_StartRefresh.AddString("Once");
  m_StartRefresh.AddString("SkipOnce");
  m_StartRefresh.AddString("Always");
  m_StartRefresh.SetCurSel(gs_pDrvMan->bDrvStartFBKCheck);

  UpdateDialogControls(this, FALSE);
  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

// --------------------------------------------------------------------------

void CDriverPage::OnOK()
  {
  if (bDidInit)
    OnApplybtn();
  }

//---------------------------------------------------------------------------

void CDriverPage::Change(flag chg)
  {
  bChanged = chg;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CDriverPage::OnUpdateApplyBtn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(bChanged);
  }

//---------------------------------------------------------------------------

void CDriverPage::OnApplybtn() 
  {
  if (bChanged)
    {
    UpdateData(True);
    gs_pDrvMan->bIgnoresOn = m_IgnoresOn;
    gs_pDrvMan->bForcesOn = m_ForcesOn;
    gs_pDrvMan->bNoiseOn = m_NoiseOn;
    gs_pDrvMan->bDrvStartFBKCheck=m_StartRefresh.GetCurSel();
    gs_pPrj->bDrvIgnoresOn = m_IgnoresOn;
    flag DoForcesNow = (gs_pPrj->bDrvForcesOn != (byte)m_ForcesOn);
    gs_pPrj->bDrvForcesOn = m_ForcesOn;
    Change(False);
    if (DoForcesNow)
      {
      for (long i=0; i<gs_pDrvMan->Slots.GetSize(); i++)
        gs_pDrvMan->ForceSlot(*(gs_pDrvMan->Slots[i]));
      }
    }
  }

//---------------------------------------------------------------------------

void CDriverPage::OnDrvforceson() 
  {
  Change();
  }

//---------------------------------------------------------------------------

void CDriverPage::OnDrvignoreson() 
  {
  Change();
  }

//---------------------------------------------------------------------------

void CDriverPage::OnDrvnoiseon() 
  {
	Change();
  }

//---------------------------------------------------------------------------

void CDriverPage::OnForceall() 
  {
  for (long i=0; i<gs_pDrvMan->Slots.GetSize(); i++)
    {
    gs_pDrvMan->Slots[i]->bEnableForces = 1;
    gs_pDrvMan->ForceSlot(*(gs_pDrvMan->Slots[i]));
    }
  }

//---------------------------------------------------------------------------

void CDriverPage::OnForcenone() 
  {
  for (long i=0; i<gs_pDrvMan->Slots.GetSize(); i++)
    {
    gs_pDrvMan->Slots[i]->bEnableForces = 0;
    gs_pDrvMan->ForceSlot(*(gs_pDrvMan->Slots[i]));
    }
  }

//---------------------------------------------------------------------------

void CDriverPage::OnReloadbtn() 
  {
  // TODO: Add your control notification handler code here
  if (XBusy())
    AfxMessageBox("Must be Stopped", MB_OK);
  else
    {
    if (bDidInit)
      OnApplybtn();
    AfxGetMainWnd()->PostMessage(WMU_RELOADDRIVER);
//    flag OK=gs_pPrj->ReloadDrvManager(TRUE);
    OnOK();
    }
  }

//---------------------------------------------------------------------------

LRESULT CDriverPage::OnUpdateBtns(WPARAM wParam, LPARAM lParam)
  {
  GetDlgItem(IDC_RELOADBTN)->EnableWindow(wParam==0);
  return 0;
  };

//---------------------------------------------------------------------------

void CDriverPage::OnSelchangeDrvstartrefresh() 
  { 
	Change();
  }

//---------------------------------------------------------------------------

void CDriverPage::OnDrvslotsRefreshIO() 
  {
	gs_pDrvMan->RefreshAllSlotData(true, true);
  }

void CDriverPage::OnDrvslotsRefreshIO_In() 
  {
	gs_pDrvMan->RefreshAllSlotData(true, false);
  }

void CDriverPage::OnDrvslotsRefreshIO_Out() 
  {
	gs_pDrvMan->RefreshAllSlotData(false, true);
  }

//---------------------------------------------------------------------------

void CDriverPage::OnDrvslotsRefreshConns() 
  {
	gs_pDrvMan->RefreshConnections();
  }

//---------------------------------------------------------------------------

#endif