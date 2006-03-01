//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "syscad.h"
#include "project.h"
#include "mainfrm.h"
#if WITHNETSERVER
#include "cs_mngr.h"
#endif
#include "tagvdoc.h"
#include "helpinfo.h"
#include "prjdlgs.h"
#include "grfdoc.h"
#include "licbase.h"
#include "scd_wm.h"
#include "scdver.h"
#include "tknparse.h"
#include "msgwnd.h"
#include "..\schist\hstmain.h"
#include ".\OPCSrvrWrapper.h"
#include "neutralgrf.h"
#include "neutralmdl.h"
#include "neutraldlgs.h"
#include "afxwin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

class CGenPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CGenPage)
  public:
    CGenPage();
    ~CGenPage();

    flag    m_bTimeChanged:1,
            m_bTagNamingChanged:1;
    double  m_CurrentTimeAtInit;

    //{{AFX_DATA(CGenPage)
    enum { IDD = IDD_PRJGENDLG };
    CString m_CurrentTimeHMS;
    CString m_TimeText;
    BOOL m_NumTagsOK;
    BOOL m_NumStartingOK;
    CString m_NumChar;
    //int m_SolveMd;
    BOOL	m_SyncWithClock;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CGenPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CGenPage)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimeChanged();
    afx_msg void OnDTimeChanged();
    afx_msg void OnKillfocusPrjCurrenttime();
    //afx_msg void OnPrjProbalOrDynamic();
    afx_msg void OnNumTagsChanged();
    afx_msg void OnNumStTagsChanged();
    afx_msg void OnSynchronise();
    //}}AFX_MSG
    afx_msg void OnUpdateDynamic(CCmdUI* pCmdUi);
    afx_msg void OnUpdateCurrentTime(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()

    void DoSyncChange();
    void SetLibraryOptions();
    void SetFrameOptions();
    //void SetGraphicsOptions();
    //void GetGraphicsOptions();
  public:
    long m_RunMode;
    long m_HeatMode;
    DDBValueLstMem m_RunModes;
    DDBValueLstMem m_HeatModes;
    CComboBox m_RunModeCB;
    CComboBox m_HeatModeCB;
    CEdit m_Library;
    afx_msg void OnBnClickedLibrarybtn();
    CComboBox m_PageFrame;
    afx_msg void OnEnChangeLibrary();

    Strng m_sDefGrpLib;
    Strng m_sGrfFrameName;
    Strng m_sGrfFrameFilter;
    CEdit m_Filter;
    afx_msg void OnEnChangeGrpfilter();
  };

//===========================================================================

class CCommsPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CCommsPage)
  public:
    CCommsPage();
    ~CCommsPage();

    flag bDrvChanged:1,
         bArcChanged:1,
         bIOMChanged:1,
         bOPCChanged:1,
         bDDEChanged:1;

    //{{AFX_DATA(CCommsPage)
	enum { IDD = IDD_PRJCOMMSDLG };
    BOOL  m_DrvOn;
    BOOL  m_DrvLclTagSrvrOK;
    CString m_DrvName;
    BOOL  m_DrvReadAll;
    BOOL  m_OPCOn;
    BOOL	m_OPCServerNo;
    BOOL  m_OPCResetReg;
    BOOL  m_DDEOn;
    BOOL  m_ArcOn;
    CString m_ArcName;
    BOOL  m_ArcOpenDBOnRun;
  	BOOL	m_IOMOn;
	  CString	m_IOMName;
	CString	m_IOMNode;
	//}}AFX_DATA
    //{{AFX_VIRTUAL(CCommsPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CCommsPage)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnDriverOn();
    afx_msg void OnDriverChanged();
    afx_msg void OnDriverReload();
    afx_msg void OnDriverBrowse();
    afx_msg void OnOPCReload();
    afx_msg void OnOpcChanged();
    afx_msg void OnDdeChanged();
    afx_msg void OnArchiveBrowse();
    afx_msg void OnArchiveOn();
    afx_msg void OnArchiveReload();
    afx_msg void OnArchiveChanged();
    afx_msg void OnPrjarcopenonrun();
	  afx_msg void OnIOMBrowse();
	  afx_msg void OnPrjIOMOn();
	  afx_msg void OnIOMChanged();
  	afx_msg void OnCommsapply();
	//}}AFX_MSG
    afx_msg void OnUpdateReLoadDrv(CCmdUI* pCmdUi);
    afx_msg void OnUpdateDriverOn(CCmdUI* pCmdUi);
    afx_msg void OnUpdateReLoadArc(CCmdUI* pCmdUi);
    afx_msg void OnUpdateArchiveOn(CCmdUI* pCmdUi);
    afx_msg void OnUpdateIOMOn(CCmdUI* pCmdUi);
    afx_msg void OnUpdateReLoadOPC(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CHistPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CHistPage)
  public:
    CHistPage();
    ~CHistPage();

    flag bHstChanged;

    //{{AFX_DATA(CHistPage)
    enum { IDD = IDD_PRJHISTDLG };
    CString m_HstName;
    BOOL m_HstOn;
    CString m_CatDir;
    CString m_DataDir;
    CString m_ScenName;
    UINT m_FilesMin;
    UINT m_FilesMax;
    long m_SlotNoEst;
    long m_ForceCnt;
    long m_HstSize;
    BOOL m_PermHist;
    BOOL m_UseTol;
    double m_ATol;
    double m_RTol;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CHistPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    void LoadInfo();
    void SaveInfo();
    //{{AFX_MSG(CHistPage)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnPrjhston();
    afx_msg void OnChangeHist();
    afx_msg void OnKillfocusPrjhstsize();
    afx_msg void OnKillfocusPrjhstfilesmin();
    afx_msg void OnKillfocusPrjhstfilesmax();
    afx_msg void OnKillfocusPrjhstforcecnt();
    afx_msg void OnKillfocusPrjhstslotnoest();
    afx_msg void OnPermhist();
    afx_msg void OnRestart();
    afx_msg void OnDelete();
    //}}AFX_MSG
    afx_msg void OnUpdateRestart(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnBnClickedPrjhstusetol();
    double m_StopGap;
    double m_MidTimeFactor;
  };

//===========================================================================

#if WITHNETSERVER
class CNetPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CNetPage)
  public:
    CNetPage();
    ~CNetPage();

    flag bNetChanged;
    ExecCoupling ClientCoupling;

    //{{AFX_DATA(CNetPage)
    enum { IDD = IDD_PRJNETWORKDLG };
    int m_ServerConns;
    BOOL m_ServerEnabled;
    CString m_ClientConnServer;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CNetPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    void GetClientCoupling();
    //{{AFX_MSG(CNetPage)
    virtual void OnOK();
    virtual void OnCancel();
    virtual BOOL OnInitDialog();
    afx_msg void OnChangeClntconnectsrv();
    afx_msg void OnSrvenable();
    afx_msg void OnRefreshsrv();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnLoopback();
    afx_msg void OnClientNone();
    afx_msg void OnClientLoose();
    afx_msg void OnClientTight();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };
#endif

//===========================================================================

IMPLEMENT_DYNAMIC(CPrjSheet, CPropertySheet)

CPrjSheet::CPrjSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
  : CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
  {
  ASSERT(FALSE); //DO NOT EXPECT TO GET HERE !!!
  }

//---------------------------------------------------------------------------

CPrjSheet::CPrjSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
  : CPropertySheet(pszCaption, pParentWnd, iSelectPage)
  {
  gs_pPrj->bConfigBusy = 1;
  pGeneralPage = new CGenPage;
  AddPage(pGeneralPage);
  pCommsPage = new CCommsPage;
  AddPage(pCommsPage);
  pHistoryPage = new CHistPage;
  AddPage(pHistoryPage);
#if WITHNETSERVER
  pNetworkPage = new CNetPage;
  AddPage(pNetworkPage);
#endif
  if (!Create(pParentWnd, WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION, WS_EX_DLGMODALFRAME/* | WS_EX_SMCAPTION*/))
    {
    TRACE("Failed to create Project PropertySheet\n");
    }
  }

//---------------------------------------------------------------------------

CPrjSheet::~CPrjSheet()
  {
  gs_pPrj->bConfigBusy = 0;
  MainWnd()->m_pPrjSheet = NULL; //set global pointer to property sheet to null
  delete pGeneralPage;
  delete pCommsPage;
  delete pHistoryPage;
#if WITHNETSERVER
  delete pNetworkPage;
#endif
  //dialog just closed, ensure main SysCAD window becomes active ...
  MainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_UPDATE, 0);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CPrjSheet, CPropertySheet)
  //{{AFX_MSG_MAP(CPrjSheet)
  ON_WM_NCDESTROY()
  ON_WM_CLOSE()
  ON_COMMAND(IDOK,OnOK)
  ON_COMMAND(IDCANCEL,OnCancel)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CPrjSheet::OnInitDialog()
  {
  ASSERT(m_bModeless); //this code only needed when creating modeless property sheet!

  PROPSHEETTRICK0()
  BOOL b = CPropertySheet::OnInitDialog();
  PROPSHEETTRICK1()

  GetDlgItem(ID_APPLY_NOW)->ShowWindow(SW_HIDE);
  GetDlgItem(IDHELP)->ShowWindow(SW_HIDE);
  return b;
  }

//---------------------------------------------------------------------------

void CPrjSheet::StoreCurPageNo()
  {
  CPropertyPage* pPage = GetActivePage();
  if (pPage)
    {
    for (int i=0; i<GetPageCount(); i++)
      if (GetPage(i)==pPage)
        {
        CProfINIFile PF(PrjIniFile());
        PF.WrInt("General", "PrjOptionsPageNo", i);
        }
    }
  }

//---------------------------------------------------------------------------
// overridden to ensure that a modeless property sheet behaves as a modal
void CPrjSheet::OnOK()
  {
  ASSERT_VALID(this);
  StoreCurPageNo();
  for (int i=0; i<GetPageCount(); i++)
    GetPage(i)->OnOK();
  if (GetActivePage()->OnKillActive())
    {
    //GetActivePage()->OnOK();
    EndDialog(IDOK);
    MainWnd()->SetToolBarSolveMode();
    }
  }

//---------------------------------------------------------------------------
// overridden to ensure that a modeless property sheet behaves as a modal
void CPrjSheet::OnCancel()
  {
  ASSERT_VALID(this);
  StoreCurPageNo();
  for (int i=0; i<GetPageCount(); i++)
    GetPage(i)->OnCancel();
  //GetActivePage()->OnCancel();
  EndDialog(IDCANCEL);
  }

//---------------------------------------------------------------------------
// overridden to ensure that a modeless property sheet behaves as a modal
void CPrjSheet::OnClose()
  {
  ASSERT_VALID(this);
  OnCancel();
  }

//---------------------------------------------------------------------------

void CPrjSheet::OnNcDestroy()
  {
  CPropertySheet::OnNcDestroy();
  delete this;
  }

//---------------------------------------------------------------------------

//===========================================================================

IMPLEMENT_DYNCREATE(CHistPage, CCustomPropertyPage)

CHistPage::CHistPage() : CCustomPropertyPage(CHistPage::IDD)
, m_StopGap(0), m_MidTimeFactor(1.0/60.0)
  {
  bHstChanged = 0;
  //{{AFX_DATA_INIT(CHistPage)
  m_HstName = _T("");
  m_HstOn = FALSE;
  m_CatDir = _T("");
  m_DataDir = _T("");
  m_ScenName = _T("");
  m_FilesMin = 0;
  m_FilesMax = 0;
  m_SlotNoEst = 0;
  m_ForceCnt = 0;
  m_HstSize = 0;
  m_PermHist = FALSE;
  m_UseTol = FALSE;
  m_ATol = 0.0;
  m_RTol = 0.0;
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CHistPage::~CHistPage()
  {
  }

//---------------------------------------------------------------------------

void CHistPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CHistPage)
  DDX_Text(pDX, IDC_PRJHSTNAME, m_HstName);
  DDX_Check(pDX, IDC_PRJHSTON, m_HstOn);
  DDX_Text(pDX, IDC_PRJHSTCATDIR, m_CatDir);
  DDX_Text(pDX, IDC_PRJHSTDATADIR, m_DataDir);
  DDX_Text(pDX, IDC_PRJHSTSCENNAME, m_ScenName);
  DDV_MaxChars(pDX, m_ScenName, 8);
  DDX_Text(pDX, IDC_PRJHSTFILESMIN, m_FilesMin);
  DDX_Text(pDX, IDC_PRJHSTFILESMAX, m_FilesMax);
  DDX_Text(pDX, IDC_PRJHSTSLOTNOEST, m_SlotNoEst);
  DDX_Text(pDX, IDC_PRJHSTFORCECNT, m_ForceCnt);
  DDX_Text(pDX, IDC_PRJHSTSIZE, m_HstSize);
  DDX_Check(pDX, IDC_PERMHIST, m_PermHist);
  DDX_Check(pDX, IDC_PRJHSTUSETOL, m_UseTol);
  DDX_Text(pDX, IDC_PRJHSTATOL, m_ATol);
  DDX_Text(pDX, IDC_PRJHSTRTOL, m_RTol);
  //}}AFX_DATA_MAP
  GetDlgItem(IDC_PRJHSTNAME)->EnableWindow(m_HstOn);
  GetDlgItem(IDC_PRJHSTSIZE)->EnableWindow(m_HstOn);
  GetDlgItem(IDC_PRJHSTSCENNAME)->EnableWindow(m_HstOn);
  GetDlgItem(IDC_PRJHSTDATADIR)->EnableWindow(m_HstOn);
  GetDlgItem(IDC_PRJHSTSLOTNOEST)->EnableWindow(m_HstOn);
  GetDlgItem(IDC_PRJHSTFORCECNT)->EnableWindow(m_HstOn);
  GetDlgItem(IDC_PRJHSTFILESMIN)->EnableWindow(m_HstOn);
  GetDlgItem(IDC_PERMHIST)->EnableWindow(/*gs_License.AllowFullHist() && */m_HstOn);
  const flag state = (/*gs_License.AllowFullHist() && */m_HstOn && m_FilesMax>0);
  GetDlgItem(IDC_PRJHSTCATDIR)->EnableWindow(state);
  GetDlgItem(IDC_PRJHSTFILESMAX)->EnableWindow(state);
  GetDlgItem(IDC_TXTCATDIR)->EnableWindow(state);
  GetDlgItem(IDC_TXTMAXFILES)->EnableWindow(state);
  DDX_Text(pDX, IDC_STOPGAP, m_StopGap);
  DDX_Text(pDX, IDC_MIDTIMEFACTOR, m_MidTimeFactor);
  }

//---------------------------------------------------------------------------

void CHistPage::LoadInfo()
  {
  Strng s;
  m_HstName      = HstMngr.sHistorianName();
  m_ScenName     = HstMngr.sHstScenName();
  if (SymbolicPaths())
    s.FnContract(HstMngr.sHstDataDir());
  else
    s.FnExpand(HstMngr.sHstDataDir());
  m_DataDir      = s();
  if (SymbolicPaths())
    s.FnContract(HstMngr.sHstCatDir());
  else
    s.FnExpand(HstMngr.sHstCatDir());
  m_CatDir       = s();
  m_HstOn        = HstMngr.HstOn();
  m_FilesMin     = HstMngr.iHstFilesMin;
  m_FilesMax     = Max((UINT)0, HstMngr.iHstFilesMax - HstMngr.iHstFilesMin);
  m_PermHist     = (m_FilesMax>0);
  m_ForceCnt     = HstMngr.lHstForceCnt;
  m_HstSize      = HstMngr.lHstEvFileSize;
  m_SlotNoEst    = HstMngr.lHstSlotNoEstimate;
  m_UseTol       = HstMngr.bHstUseTol;
  m_ATol         = HstMngr.dHstRecordATol;
  m_RTol         = HstMngr.dHstRecordRTol;
  m_StopGap      = HstMngr.dHstStopGap;
  m_MidTimeFactor= HstMngr.dHstMidTimeFactor;
  }

//---------------------------------------------------------------------------

void CHistPage::SaveInfo()
  {
  HstMngr.sHistorianName = m_HstName.GetBuffer(0);
  HstMngr.SetHstOn(m_HstOn);
  HstMngr.lHstEvFileSize = Max(64L, (((m_HstSize-1)/64L)+1)*64L);
  HstMngr.lHstSlotNoEstimate = Max(100L, m_SlotNoEst);
  HstMngr.sHstCatDir.FnContract((char*)(const char*)m_CatDir);
  HstMngr.sHstDataDir.FnContract((char*)(const char*)m_DataDir);
  HstMngr.sHstScenName = m_ScenName;
  HstMngr.iHstFilesMin = Max(m_FilesMin, (UINT)2);
  HstMngr.iHstFilesMax = m_FilesMin + m_FilesMax;
  HstMngr.lHstForceCnt = Max(m_ForceCnt, 100L);
  //if (!gs_License.AllowFullHist())
  //  gs_pPrj->iHstFilesMin = gs_pPrj->iHstFilesMax;
  if (HstMngr.iHstFilesMin>gs_License.MaxHistFilesAllowed() || HstMngr.iHstFilesMax>gs_License.MaxHistFilesAllowed() || HstMngr.lHstEvFileSize>gs_License.MaxHistSizeAllowed())
    {
    LogError("History", LF_Exclamation, "Historian file size reduced to that allowed by the license");
    HstMngr.iHstFilesMax = Min(HstMngr.iHstFilesMax, gs_License.MaxHistFilesAllowed());
    HstMngr.iHstFilesMin = Min(HstMngr.iHstFilesMin, HstMngr.iHstFilesMax);
    HstMngr.lHstEvFileSize = Min(HstMngr.lHstEvFileSize, gs_License.MaxHistSizeAllowed());
    }
  if (stricmp(HstMngr.sHistorianName(), HstMngr.sHstScenName())==0)
    {
    LogError("History", LF_Exclamation, "Historian name and scenario name must NOT be the same");
    //HstMngr.sHstScenName += "_0";
    //m_ScenName = HstMngr.sHstScenName();
    }
  HstMngr.bHstUseTol = m_UseTol;
  HstMngr.dHstRecordATol = Min(fabs(m_ATol), 0.1);
  HstMngr.dHstRecordRTol = Min(fabs(m_RTol), 0.1);
  HstMngr.dHstStopGap= Range(0.0, fabs(m_StopGap), 1000.0);
  HstMngr.dHstMidTimeFactor= Range(0.001, fabs(m_MidTimeFactor), 0.999);
  }

//---------------------------------------------------------------------------

BOOL CHistPage::OnInitDialog()
  {
  LoadInfo();

  CCustomPropertyPage::OnInitDialog();
  UpdateDialogControls(this, FALSE);
  return TRUE;  // return TRUE  unless you set the focus to a control
  }

//---------------------------------------------------------------------------

void CHistPage::OnOK()
  {
  if (bDidInit)
    {
    UpdateData(true);
    if (bHstChanged)
      {
      gs_pPrj->bHstChanged = bHstChanged;
      gs_pPrj->CloseHistorian();
      SaveInfo();
      gs_pPrj->OpenHistorian();
      CTagVwDoc::RebuildAll();
      }
    bHstChanged = 0;
    UpdateData(false);
    }
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CHistPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CHistPage)
  ON_BN_CLICKED(IDC_PRJHSTON, OnPrjhston)
  ON_EN_CHANGE(IDC_PRJHSTCATDIR, OnChangeHist)
  ON_EN_CHANGE(IDC_PRJHSTDATADIR, OnChangeHist)
  ON_EN_CHANGE(IDC_PRJHSTNAME, OnChangeHist)
  ON_EN_CHANGE(IDC_PRJHSTSCENNAME, OnChangeHist)
  ON_EN_CHANGE(IDC_PRJHSTSIZE, OnChangeHist)
  ON_EN_CHANGE(IDC_PRJHSTSLOTNOEST, OnChangeHist)
  ON_EN_CHANGE(IDC_PRJHSTFORCECNT, OnChangeHist)
  ON_EN_CHANGE(IDC_PRJHSTFILESMAX, OnChangeHist)
  ON_EN_CHANGE(IDC_PRJHSTFILESMIN, OnChangeHist)
  ON_EN_CHANGE(IDC_PRJHSTUSETOL, OnChangeHist)
  ON_EN_CHANGE(IDC_PRJHSTATOL, OnChangeHist)
  ON_EN_CHANGE(IDC_PRJHSTRTOL, OnChangeHist)
  ON_EN_KILLFOCUS(IDC_PRJHSTSIZE, OnKillfocusPrjhstsize)
  ON_EN_KILLFOCUS(IDC_PRJHSTFILESMIN, OnKillfocusPrjhstfilesmin)
  ON_EN_KILLFOCUS(IDC_PRJHSTFILESMAX, OnKillfocusPrjhstfilesmax)
  ON_EN_KILLFOCUS(IDC_PRJHSTFORCECNT, OnKillfocusPrjhstforcecnt)
  ON_EN_KILLFOCUS(IDC_PRJHSTSLOTNOEST, OnKillfocusPrjhstslotnoest)
  ON_BN_CLICKED(IDC_PERMHIST, OnPermhist)
  ON_BN_CLICKED(IDC_RESTART, OnRestart)
  ON_BN_CLICKED(IDC_DELETE, OnDelete)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDC_RESTART, OnUpdateRestart)
  ON_BN_CLICKED(IDC_PRJHSTUSETOL, OnBnClickedPrjhstusetol)
  ON_EN_CHANGE(IDC_STOPGAP, OnChangeHist)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CHistPage::OnPrjhston()
  {
  UpdateData(true);
  bHstChanged = 1;
  }

//---------------------------------------------------------------------------

void CHistPage::OnChangeHist()
  {
  bHstChanged = 1;
  }

//---------------------------------------------------------------------------

void CHistPage::OnKillfocusPrjhstfilesmin()
  {
  UpdateData(true);
  if (m_FilesMin<2)
    {
    bHstChanged = 1;
    m_FilesMin = 2;
    UpdateData(false);
    }
  }

//---------------------------------------------------------------------------

void CHistPage::OnKillfocusPrjhstfilesmax()
  {
  UpdateData(true);
  if (m_FilesMax<1)
    {
    bHstChanged = 1;
    m_FilesMax = 1;
    UpdateData(false);
    }
  }

//---------------------------------------------------------------------------

void CHistPage::OnKillfocusPrjhstsize()
  {
  UpdateData(true);
  m_HstSize = Max(64L, (((m_HstSize-1)/64L)+1)*64L);
  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CHistPage::OnKillfocusPrjhstslotnoest()
  {
  UpdateData(true);
  if (m_SlotNoEst<100)
    {
    bHstChanged = 1;
    m_SlotNoEst = 100;
    UpdateData(false);
    }
  }

//---------------------------------------------------------------------------

void CHistPage::OnKillfocusPrjhstforcecnt()
  {
  UpdateData(true);
  if (m_ForceCnt<100)
    {
    bHstChanged = 1;
    m_ForceCnt = 100;
    UpdateData(false);
    }
  }

//---------------------------------------------------------------------------

void CHistPage::OnPermhist()
  {
  UpdateData(true);
  bHstChanged = 1;
  if (m_PermHist)
    m_FilesMax = Max(m_FilesMax, (UINT)1);
  else
    m_FilesMax = 0;
  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CHistPage::OnUpdateRestart(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(1);
  }

//---------------------------------------------------------------------------

void CHistPage::OnRestart()
  {
  if (bDidInit)
    {
    UpdateData(true);
    gs_pPrj->bHstChanged = bHstChanged;
    gs_pPrj->CloseHistorian();
    SaveInfo();
    gs_pPrj->RestartHistorian();
    gs_pPrj->OpenHistorian();
    LoadInfo();
    bHstChanged = 0;
    UpdateData(false);
    CTagVwDoc::AdjustTimebaseToEndAll();
    }
  }

//---------------------------------------------------------------------------

void CHistPage::OnDelete()
  {
  if (bDidInit)
    {
    UpdateData(true);
    gs_pPrj->bHstChanged = bHstChanged;
    gs_pPrj->CloseHistorian();
    SaveInfo();
    gs_pPrj->DeleteHistorian();
    gs_pPrj->OpenHistorian();
    LoadInfo();
    bHstChanged = 0;
    UpdateData(false);
    CTagVwDoc::RebuildAll();
    }
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CGenPage, CCustomPropertyPage)

CGenPage::CGenPage() : CCustomPropertyPage(CGenPage::IDD)
  {
  m_bTimeChanged = 0;
  m_bTagNamingChanged = 0;
  //{{AFX_DATA_INIT(CGenPage)
  m_CurrentTimeHMS = _T("");
  m_TimeText = _T("Time");
  m_NumTagsOK = !FALSE;
  m_NumStartingOK = !FALSE;
  m_NumChar = _T("");
  //m_SolveMd = 0;
	m_SyncWithClock = FALSE;
	//}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CGenPage::~CGenPage()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CGenPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CGenPage)
  ON_EN_CHANGE(IDC_PRJCURRENTTIME, OnTimeChanged)
  ON_EN_KILLFOCUS(IDC_PRJCURRENTTIME, OnKillfocusPrjCurrenttime)
  //ON_BN_CLICKED(IDC_PRJPROBAL, OnPrjProbalOrDynamic)
  //ON_BN_CLICKED(IDC_PRJDYNAMICFLOW, OnPrjProbalOrDynamic)
  //ON_BN_CLICKED(IDC_PRJDYNAMICFULL, OnPrjProbalOrDynamic)
  ON_BN_CLICKED(IDC_PRJNUMERICOK, OnNumTagsChanged)
  ON_BN_CLICKED(IDC_PRJNUMERICSTARTOK, OnNumStTagsChanged)
  ON_BN_CLICKED(IDC_SYNCHRONISE, OnSynchronise)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDC_PRJCURRENTTIME, OnUpdateCurrentTime)
  ON_UPDATE_COMMAND_UI(IDC_SYNCHRONISE, OnUpdateDynamic)
  ON_BN_CLICKED(IDC_LIBRARYBTN, OnBnClickedLibrarybtn)
  ON_EN_CHANGE(IDC_LIBRARY, OnEnChangeLibrary)
  ON_EN_CHANGE(IDC_GRPFILTER, OnEnChangeGrpfilter)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CGenPage::OnInitDialog()
  {
  m_RunMode=TaggedObject::GlblSolveMode();
  m_HeatMode=TaggedObject::GlblHeatMode();
  m_SyncWithClock = gs_Exec.SyncWithClock()!=0;

  char buff[256];
  SecstoHMSDate(gs_Exec.Time(), buff, true/*m_SyncWithClock*/);
  m_CurrentTimeHMS = buff;
  m_CurrentTimeAtInit = gs_Exec.Time();
  SecstoHMSDate(gs_Exec.Time(), buff, false);
  SetDlgItemText(IDC_DATETIME_TXT, m_RunMode==0 ? "" : buff);

  m_NumTagsOK = !TaggedObject::NumericTagsBad;
  m_NumStartingOK = !TaggedObject::NumericStartingTagsBad;
  m_NumStartingOK = m_NumStartingOK || m_NumTagsOK;
  m_NumChar = TaggedObject::NonNumericTagChr;

  CCustomPropertyPage::OnInitDialog();
  UpdateDialogControls(this, FALSE);

  TaggedObject::GetGlblModeValueLst(m_RunModes, TaggedObject::GetPermissableGlblRunModes(SM_All));
  TaggedObject::GetGlblModeValueLst(m_HeatModes, TaggedObject::GetPermissableGlblRunModes(HM_All));

  int ir=-1;
  int ih=-1;

  for (int i=0; i<m_RunModes.Length(); i++)
    {
    m_RunModeCB.AddString(m_RunModes.Item(i)->m_pStr);
    if (m_RunModes.Item(i)->m_lVal==m_RunMode)
      ir=i;
    }
  for (int i=0; i<m_HeatModes.Length(); i++)
    {
    m_HeatModeCB.AddString(m_HeatModes.Item(i)->m_pStr);
    if (m_HeatModes.Item(i)->m_lVal==m_HeatMode)
      ih=i;
    }

  m_RunModeCB.SetCurSel(ir);
  m_HeatModeCB.SetCurSel(ih);

#if WITHGRFDOCFRAME
  m_sDefGrpLib=gs_pPrj->m_sDefGrpLib;
  m_sGrfFrameName=gs_pPrj->m_sGrfFrameName;
  m_sGrfFrameFilter=gs_pPrj->m_sGrfFrameFilter;

  m_sDefGrpLib.FnContract();
  m_Filter.SetWindowText(m_sGrfFrameFilter());
  m_Library.SetWindowText(m_sDefGrpLib()); // this will invoke the SetLibraryOptions
#else
  m_Filter.EnableWindow(FALSE);
  m_Library.EnableWindow(FALSE); // this will invoke the SetLibraryOptions
  m_PageFrame.EnableWindow(FALSE); // this will invoke the SetLibraryOptions
#endif

  return TRUE;
  }

//---------------------------------------------------------------------------

void CGenPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CGenPage)
  DDX_Text(pDX, IDC_PRJCURRENTTIME, m_CurrentTimeHMS);
  DDX_Text(pDX, IDC_TIMETEXT, m_TimeText);
  DDX_Check(pDX, IDC_PRJNUMERICOK, m_NumTagsOK);
  DDX_Check(pDX, IDC_PRJNUMERICSTARTOK, m_NumStartingOK);
  DDX_Text(pDX, IDC_PRJNUMERICCHAR, m_NumChar);
  //DDX_Radio(pDX, IDC_PRJPROBAL, m_SolveMd);
  DDX_Check(pDX, IDC_SYNCHRONISE, m_SyncWithClock);
  //}}AFX_DATA_MAP

  GetDlgItem(IDC_PRJCURRENTTIME)->EnableWindow(!m_SyncWithClock);
  DDX_Control(pDX, IDC_PRJSOLVEMODECOMBO, m_RunModeCB);
  DDX_Control(pDX, IDC_PRJHEATMODECOMBO, m_HeatModeCB);
  DDX_Control(pDX, IDC_LIBRARY, m_Library);
  DDX_Control(pDX, IDC_PAGEFRAME, m_PageFrame);
  DDX_Control(pDX, IDC_GRPFILTER, m_Filter);
  }

//---------------------------------------------------------------------------

//void CGenPage::GetGraphicsOptions()
//  {
//  CString S;
//  m_Library.GetWindowText(S);
//  gs_pPrj->m_sDefGrpLib=S;
//  gs_pPrj->m_sDefGrpLib.FnContract();
//
//  m_PageFrame.GetWindowText(S);
//  gs_pPrj->m_sGrfFrameName=S;
//  }

//---------------------------------------------------------------------------

void CGenPage::SetLibraryOptions()
  {
  CString S;
  m_Library.GetWindowText(S);
  Strng Fn(S);
  Fn.FnExpand();
  flag LibOK=FileExists(Fn());
  if (LibOK)
    m_sDefGrpLib.FnContract(Fn());

  m_PageFrame.ResetContent();
  m_PageFrame.AddString("Extents-A1");
  m_PageFrame.AddString("Extents-A2");
  m_PageFrame.AddString("Extents-A3");
  m_PageFrame.AddString("Extents-A4");
  m_PageFrame.AddString("Extents-A5");
  m_PageFrame.AddString("None");
  m_PageFrame.AddString("----------");

  if (LibOK)
    {
    Strng Fn(m_sDefGrpLib);
    Fn.FnExpand();
    CNeutralImportExport NImport;
    CNeutralGrpDescList List;
    //NImport.SetImportFilter(m_sGrfFrameFilter());
    if (NImport.GetGroups(List, Fn())>0)
      {
      POSITION Pos=List.GetHeadPosition();
      while (Pos)
        {
        CNeutralGrpDescItem &I=List.GetNext(Pos);
        if (strnicmp(I.m_sGroup, m_sGrfFrameFilter(), m_sGrfFrameFilter.GetLength())==0)
          m_PageFrame.AddString(I.m_sGroup);
        }
      int xxx=0;
      }
    }

  int i=m_PageFrame.SelectString(0, m_sGrfFrameName());
  if (i<0)
    m_PageFrame.SetCurSel(2);
  }

//---------------------------------------------------------------------------
//
//void CGenPage::SetFrameOptions()
//  {
//  CString S;
//  m_Library.GetWindowText(S);
//  gs_pPrj->m_sDefGrpLib=S;
//  gs_pPrj->m_sDefGrpLib.FnContract();
//  //m_Library.SetWindowText(gs_pPrj->m_sDefGrpLib());
//  //m_PageFrame.SetWindowText(gs_pPrj->m_sGrfFrameName());
//  }

//---------------------------------------------------------------------------

void CGenPage::OnOK()
  {
  if (bDidInit)
    {
    UpdateData(true);
    //GetGraphicsOptions();


    long RqdRM=m_RunModeCB.GetCurSel()>=0 ? m_RunModes.Item(m_RunModeCB.GetCurSel())->m_lVal : -1;
    long RqdHM=m_HeatModeCB.GetCurSel()>=0 ? m_HeatModes.Item(m_HeatModeCB.GetCurSel())->m_lVal : -1;

    if (RqdRM!=m_RunMode || RqdHM!=m_HeatMode)
      {

      gs_Exec.SetGlblRunModes(RqdRM|RqdHM, SHM_All);

      if (!CnmVersion())
        {
        //Force a PrjSaveAs conditional on there being units in flowsheet;
        CModelStatusCriterion Criteria;
        CModelStatusArray Status;
        gs_pPrj->RequestModelStatus(Criteria, Status);
        if (Status.GetCount()>1)
          {
          gs_pPrj->bForceSaveAsNV=true;
          AfxMessageBox("Solve Mode has been changed:\nProject will be saved as a new version", MB_OK);
          }
        }

      m_bTimeChanged=1;
      m_SyncWithClock=0;
      DoSyncChange();
      }

    if (m_NumChar.GetLength()==0 || !TaggedObject::CheckNonNumericTagChr(m_NumChar[0]))
      {
      LogWarning("Project", 0, "Invalid non-numeric character (%s) specified, changed to '#'.", (const char*)m_NumChar);
      m_NumChar = "#";
      }
    TaggedObject::NonNumericTagChr = m_NumChar[0];

    if (m_bTimeChanged || m_bTagNamingChanged)
      {
      gs_pPrj->bTimeChanged = m_bTimeChanged;
      double RqdTime;
      flag OK1 = HMSDatetoSecs(m_CurrentTimeHMS.GetBuffer(0), RqdTime);
      if (RqdTime<m_CurrentTimeAtInit && RqdTime>=m_CurrentTimeAtInit-0.002) // rounding errors in (2 msecs)
        RqdTime=m_CurrentTimeAtInit;
      if (OK1)
        {
        flag ScrollTrendsToEnd = false;
        if (HstMngr.HstOn() && m_bTimeChanged && (RqdTime<m_CurrentTimeAtInit))
          {
          switch (AfxMessageBox("Moving time Backwards!\n\nHistorian will be cleared", MB_ICONQUESTION|MB_OKCANCEL))
            {
            case IDOK:
//gs_pPrj->CloseHistorian();
//gs_pPrj->RestartHistorian();
//gs_pPrj->OpenHistorian();
              ScrollTrendsToEnd = true;
              break;
            case IDCANCEL:
              m_bTimeChanged=false;
              break;
            }
          }

        if (m_bTimeChanged)
          {
          gs_Exec.SetTime(RqdTime);
          gs_Exec.SetSyncWithClock(m_SyncWithClock);
          if (m_SyncWithClock)
            {
            gs_Exec.SetRealTime(true);
            gs_Exec.SetRealTimeMult(1.0);
            }
          ScrollTrendsToEnd = true;
          }

        if (m_bTagNamingChanged)
          {
          int TagsChngd = gs_Exec.CheckAllTags(!m_NumTagsOK, !m_NumStartingOK);
          if (TagsChngd<0)
            {
            TaggedObject::NumericTagsBad = 0;
            TaggedObject::NumericStartingTagsBad = 0;
            }
          else
            {
            TaggedObject::NumericTagsBad = !m_NumTagsOK;
            TaggedObject::NumericStartingTagsBad = !m_NumStartingOK;
            if (TagsChngd>0)
              {
              CWaitCursor Wait;
              gs_pPrj->CloseIOMarshal();
              gs_pPrj->CloseArcManager();
              gs_pPrj->CloseDrvManager();
              gs_pPrj->OpenDrvManager();
              gs_pPrj->OpenArcManager();
              CTagVwDoc::RebuildAll();
              }
            }
          }
        if (ScrollTrendsToEnd)
          CTagVwDoc::AdjustTimebaseToEndAll();
        }
      else
        AfxMessageBox("Invalid CurrentTime");
      }

    CString S;
    m_Library.GetWindowText(S);
    m_sDefGrpLib=S;
    m_Filter.GetWindowText(S);
    m_sGrfFrameFilter=S;
    m_PageFrame.GetWindowText(S);
    m_sGrfFrameName=S;

    gs_pPrj->m_sDefGrpLib=m_sDefGrpLib;
    gs_pPrj->m_sGrfFrameName=m_sGrfFrameName;
    gs_pPrj->m_sGrfFrameFilter=m_sGrfFrameFilter;

    }
  }

//---------------------------------------------------------------------------

void CGenPage::OnTimeChanged()
  {
  m_bTimeChanged = 1;
  }

//---------------------------------------------------------------------------

void CGenPage::OnKillfocusPrjCurrenttime()
  {
  UpdateData(true);
  char buff[256];
  double t;
  if (HMSDatetoSecs(m_CurrentTimeHMS.GetBuffer(0), t))
    {
    if (t>=0.0)
      {
      SecstoHMSDate(t, buff, true/*m_SyncWithClock*/);
      m_CurrentTimeHMS = buff;
      SecstoHMSDate(t, buff, false);
      SetDlgItemText(IDC_DATETIME_TXT, buff);
      UpdateData(false);
      }
    }
  }

void CGenPage::OnUpdateCurrentTime(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(!gs_Exec.GlblProbalMode() && !m_SyncWithClock);
  }

//---------------------------------------------------------------------------

//void CGenPage::OnPrjProbalOrDynamic()
//  {
//  int Prev=m_SolveMd;
//  UpdateData(true);
//  if (m_SolveMd!=Prev)
//    {
//    long SMd=0;
//    switch (m_SolveMd)
//      {
//      case 0: SMd=SM_Probal; break;
//      case 1: SMd=SM_DynXfer; break;
//      case 2: SMd=SM_DynFull; break;
//      }
//    long HMd=HM_Full;
//
//    gs_Exec.SetGlblRunModes(SMd|HMd, SHM_All);
//
//    m_bTimeChanged=1;
//    m_SyncWithClock=0;
//    DoSyncChange();
//    UpdateData(false);
//    UpdateDialogControls(this, FALSE);
//    }
//  }

void CGenPage::OnUpdateDynamic(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(gs_Exec.GlblDynamicMode());
  }

//---------------------------------------------------------------------------

void CGenPage::OnNumTagsChanged()
  {
  UpdateData(true);
  m_bTagNamingChanged = 1;
  m_NumStartingOK = m_NumStartingOK || m_NumTagsOK;
  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CGenPage::OnNumStTagsChanged()
  {
  UpdateData(true);
  m_bTagNamingChanged = 1;
//  m_NumStartingOK = m_NumStartingOK || m_NumTagsOK;
  m_NumTagsOK = m_NumTagsOK && m_NumStartingOK;
  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CGenPage::OnSynchronise()
  {
  UpdateData(true);
  DoSyncChange();
  UpdateData(false);
  UpdateDialogControls(this, FALSE);
  }

void CGenPage::DoSyncChange()
  {
  m_bTimeChanged = 1;
  char buff[256];
  if (m_SyncWithClock)
    {
    __time64_t t;
    _time64(&t); //this Initialisation must occur
    SecstoHMSDate((double)t, buff, true);
    m_CurrentTimeHMS = buff;
    SecstoHMSDate((double)t, buff, false);
    SetDlgItemText(IDC_DATETIME_TXT, m_RunMode==0 ? "" : buff);
    }
  else
    {
    SecstoHMSDate(m_CurrentTimeAtInit, buff, true/*m_SyncWithClock*/);
    m_CurrentTimeHMS = buff;
    SecstoHMSDate(m_CurrentTimeAtInit, buff, false);
    SetDlgItemText(IDC_DATETIME_TXT, m_RunMode==0 ? "" : buff);
    }
  }

void CGenPage::OnBnClickedLibrarybtn()
  {
  INCOMPLETECODE()
  //GetGraphicsOptions();
  //SetGraphicsOptions();
  }

void CGenPage::OnEnChangeLibrary()
  {
  SetLibraryOptions();
  //GetGraphicsOptions();
 // SetGraphicsOptions();
  }


void CGenPage::OnEnChangeGrpfilter()
  {
  CString S;
  m_Filter.GetWindowText(S);
  m_sGrfFrameFilter=S;
  SetLibraryOptions();
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CCommsPage, CCustomPropertyPage)

CCommsPage::CCommsPage() : CCustomPropertyPage(CCommsPage::IDD)
  {
  bDrvChanged = 0;
  bArcChanged = 0;
  bIOMChanged = 0;
  bOPCChanged = 0;
  bDDEChanged = 0;
  //{{AFX_DATA_INIT(CCommsPage)
  m_DrvOn = FALSE;
  m_DrvLclTagSrvrOK= TRUE;
  m_DrvName = _T("");
  m_DrvReadAll = FALSE;
  m_OPCOn = FALSE;
	m_OPCServerNo = FALSE;
  m_OPCResetReg = FALSE;
  m_DDEOn = FALSE;
  m_ArcOn = FALSE;
  m_ArcName = _T("");
  m_ArcOpenDBOnRun = FALSE;
	m_IOMOn = FALSE;
	m_IOMName = _T("");
	m_IOMNode = _T("");
	//}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CCommsPage::~CCommsPage()
  {
  }

//---------------------------------------------------------------------------

void CCommsPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCommsPage)
  DDX_Check(pDX, IDC_PRJDRVON, m_DrvOn);
  DDX_Check(pDX, IDC_PRJDRVLOCALTAGSRVR, m_DrvLclTagSrvrOK);
  DDX_Text(pDX, IDD_PRJDRVNAME, m_DrvName);
  DDX_Check(pDX, IDC_PRJDRVREADALL, m_DrvReadAll);
  DDX_Check(pDX, IDC_PRJOPCON, m_OPCOn);
	DDX_Check(pDX, IDC_PRJOPCSECONDSERVER, m_OPCServerNo);
  DDX_Check(pDX, IDC_PRJOPCRESETREG, m_OPCResetReg);
  DDX_Check(pDX, IDC_PRJDDEON, m_DDEOn);
  DDX_Check(pDX, IDC_PRJARCON, m_ArcOn);
  DDX_Text(pDX, IDD_PRJARCNAME, m_ArcName);
  DDX_Check(pDX, IDC_PRJARCOPENONRUN, m_ArcOpenDBOnRun);
	DDX_Check(pDX, IDC_PRJIOMON, m_IOMOn);
	DDX_Text(pDX, IDD_PRJIOMNAME, m_IOMName);
	DDX_Text(pDX, IDD_PRJIOMNODE, m_IOMNode);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCommsPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CCommsPage)
  ON_BN_CLICKED(IDC_PRJDRVON, OnDriverOn)
  ON_EN_CHANGE(IDD_PRJDRVNAME, OnDriverChanged)
  ON_BN_CLICKED(IDC_PRJRELOADDRV, OnDriverReload)
  ON_BN_CLICKED(IDC_DRVBROWSE, OnDriverBrowse)
	ON_BN_CLICKED(IDC_PRJOPCRELOAD, OnOPCReload)
  ON_BN_CLICKED(IDC_PRJOPCON, OnOpcChanged)
  ON_BN_CLICKED(IDC_PRJDDEON, OnDdeChanged)
  ON_BN_CLICKED(IDC_ARCBROWSE, OnArchiveBrowse)
  ON_BN_CLICKED(IDC_PRJARCON, OnArchiveOn)
  ON_BN_CLICKED(IDC_PRJRELOADARC, OnArchiveReload)
  ON_EN_CHANGE(IDD_PRJARCNAME, OnArchiveChanged)
  ON_BN_CLICKED(IDC_PRJARCOPENONRUN, OnPrjarcopenonrun)
	ON_BN_CLICKED(IDC_IOMBROWSE, OnIOMBrowse)
	ON_BN_CLICKED(IDC_PRJIOMON, OnPrjIOMOn)
	ON_EN_CHANGE(IDD_PRJIOMNAME, OnIOMChanged)
	ON_BN_CLICKED(IDC_COMMSAPPLY, OnCommsapply)
  ON_BN_CLICKED(IDC_PRJDRVLOCALTAGSRVR, OnDriverOn)
  ON_BN_CLICKED(IDC_PRJDRVREADALL, OnDriverChanged)
  ON_BN_CLICKED(IDC_PRJOPCSECONDSERVER, OnOpcChanged)
  ON_BN_CLICKED(IDC_PRJOPCRESETREG, OnOpcChanged)
	ON_EN_CHANGE(IDD_PRJIOMNODE, OnIOMChanged)
	//}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDC_PRJRELOADDRV, OnUpdateReLoadDrv)
  ON_UPDATE_COMMAND_UI(IDC_PRJDRVREADALL, OnUpdateDriverOn)
  ON_UPDATE_COMMAND_UI(IDD_PRJDRVNAME, OnUpdateDriverOn)
  ON_UPDATE_COMMAND_UI(IDC_DRVBROWSE, OnUpdateDriverOn)
  ON_UPDATE_COMMAND_UI(IDC_PRJRELOADARC, OnUpdateReLoadArc)
  ON_UPDATE_COMMAND_UI(IDD_PRJARCNAME, OnUpdateArchiveOn)
  ON_UPDATE_COMMAND_UI(IDC_ARCBROWSE, OnUpdateArchiveOn)
  ON_UPDATE_COMMAND_UI(IDD_PRJIOMNAME, OnUpdateIOMOn)
  ON_UPDATE_COMMAND_UI(IDC_IOMBROWSE, OnUpdateIOMOn)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CCommsPage::OnInitDialog()
  {
  Strng Fn;

  m_DrvName    = gs_pPrj->sDrvManagerName();
  m_DrvOn      = gs_pPrj->bDrvOn;
  m_DrvLclTagSrvrOK= gs_pPrj->bDrvLclTagSrvrOK;
  m_DrvReadAll = gs_pPrj->bDrvReadAll;
  if (SymbolicPaths())
    Fn.FnContract(gs_pPrj->sDrvManagerName());
  else
    Fn.FnExpand(gs_pPrj->sDrvManagerName());
  m_DrvName=Fn();

  m_ArcName        = gs_pPrj->sArcManagerName();
  m_ArcOn          = gs_pPrj->bArcOn;
  m_ArcOpenDBOnRun = gs_pPrj->bArcOpenDBOnRun;
  if (SymbolicPaths())
    Fn.FnContract(gs_pPrj->sArcManagerName());
  else
    Fn.FnExpand(gs_pPrj->sArcManagerName());
  m_ArcName=Fn();

  m_OPCOn = gs_pPrj->m_bOPCOn;
  m_OPCServerNo = (gs_pPrj->m_iOPCServerNo!=0);
  m_OPCResetReg = gs_pPrj->m_bOPCResetReg;

  m_DDEOn      = gs_pPrj->bDDEOn;

  m_IOMName        = gs_pPrj->sIOMarshalName();
  m_IOMNode        = gs_pPrj->sIOMarshalNode();
  m_IOMOn          = gs_pPrj->bIOMOn;
  if (SymbolicPaths())
    Fn.FnContract(gs_pPrj->sIOMarshalName());
  else
    Fn.FnExpand(gs_pPrj->sIOMarshalName());
  m_IOMName=Fn();

  CCustomPropertyPage::OnInitDialog();
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CCommsPage::OnOK()
  {
  if (bDidInit)
    {
    UpdateData(true);

    Strng Fn=m_DrvName;
    Fn.FnContract();
    m_DrvName=Fn();

    Fn=m_ArcName;
    Fn.FnContract();
    m_ArcName=Fn();

    Fn=m_IOMName;
    Fn.FnContract();
    m_IOMName=Fn();

    if (bDrvChanged || bArcChanged || bIOMChanged || bOPCChanged || bDDEChanged)
      {
      gs_pPrj->bDrvChanged = bDrvChanged;
      if (bDrvChanged)
        {
        gs_pPrj->CloseDrvManager();
        gs_pPrj->sDrvManagerName.FnContract((char*)(const char*)m_DrvName);
        gs_pPrj->bDrvOn          = m_DrvOn;
        gs_pPrj->bDrvLclTagSrvrOK = m_DrvLclTagSrvrOK;
        gs_pPrj->bDrvReadAll     = m_DrvReadAll;
        gs_pPrj->OpenDrvManager();
        CTagVwDoc::RebuildAll();
        }

      gs_pPrj->bArcChanged = bArcChanged;
      if (bArcChanged)
        {
        gs_pPrj->CloseArcManager();
        gs_pPrj->sArcManagerName.FnContract((char*)(const char*)m_ArcName);
        gs_pPrj->bArcOn          = m_ArcOn;
        gs_pPrj->bArcOpenDBOnRun = m_ArcOpenDBOnRun;
        gs_pPrj->OpenArcManager();
        CTagVwDoc::RebuildAll();
        }

      gs_pPrj->bIOMChanged = bIOMChanged;
      if (bIOMChanged)
        {
        gs_pPrj->CloseIOMarshal();
        gs_pPrj->sIOMarshalName.FnContract((LPSTR)(LPCSTR)m_IOMName);
        gs_pPrj->sIOMarshalNode=(LPSTR)(LPCSTR)m_IOMNode;
        gs_pPrj->bIOMOn          = m_IOMOn;
        //gs_pPrj->bArcOpenDBOnRun = m_ArcOpenDBOnRun;
        gs_pPrj->OpenIOMarshal();
        CTagVwDoc::RebuildAll();
        }

      if (bOPCChanged)
        {
        DisableMainOPCSrvr();
        //gs_pPrj->CloseOPCManager();
        gs_pPrj->m_bOPCOn = m_OPCOn;
        gs_pPrj->m_iOPCServerNo = m_OPCServerNo;
        gs_pPrj->m_bOPCResetReg = m_OPCResetReg;
        //gs_pPrj->OpenOPCManager();
        EnableMainOPCSrvr(gs_pPrj->m_bOPCOn);
        }

      if (bDDEChanged)
        {
        gs_pPrj->CloseDDEManager();
        gs_pPrj->bDDEOn = m_DDEOn;
        gs_pPrj->OpenDDEManager();
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CCommsPage::OnDriverOn()
  {
  UpdateData(true);
  bDrvChanged = 1;
  UpdateDialogControls(this, FALSE);
  }

void CCommsPage::OnUpdateDriverOn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_DrvOn);
  }

//---------------------------------------------------------------------------

void CCommsPage::OnDriverChanged()
  {
  bDrvChanged = 1;
  }

//---------------------------------------------------------------------------

void CCommsPage::OnDriverReload()
  {
  UpdateData(true);
  Strng Fn(m_DrvName);
  if (SymbolicPaths())
    {
    Fn.FnContract();
    m_DrvName=Fn();
    Fn.FnExpand();
    }
  else
    {
    Fn.FnExpand();
    m_DrvName=Fn();
    }
  UpdateData(false);
  gs_pPrj->bDrvLclTagSrvrOK = m_DrvLclTagSrvrOK;
  if (gs_pPrj->ReloadDrvManager(FALSE, Fn()))
    {
    gs_pPrj->sDrvManagerName = m_DrvName;
    bDrvChanged = 0;
    UpdateDialogControls(this, FALSE);
    }
  }

void CCommsPage::OnUpdateReLoadDrv(CCmdUI* pCmdUi)
  {//can only reload if the driver was on
  pCmdUi->Enable(gs_pPrj->bDrvOn);
  }

//---------------------------------------------------------------------------

void CCommsPage::OnDriverBrowse()
  {
  UpdateData(true);
  Strng FName,FPath;

  if (m_DrvName.GetLength()>0)
    {
    FPath.FnDrivePath((char*)(const char*)m_DrvName);
    FName.FnNameExt((char*)(const char*)m_DrvName);
    }
  else
    {
    FName = "*.mdb;*.xls;*.csv";
    FPath = "..\\";//PrjPrevDirectory();
    }

  CSCDFileDialog Dlg(TRUE, NULL, FName(), OFN_NOCHANGEDIR | OFN_HIDEREADONLY,
                      "SysCAD IO List (*.mdb;*.xls;*.csv)|*.mdb;*.xls;*.csv||", this);
  Dlg.m_ofn.lpstrInitialDir = FPath();
  Dlg.m_ofn.lpstrTitle = "Browse";
  if (Dlg.DoModal()==IDOK)
    {
    m_DrvName=Dlg.GetPathName();
    Strng Fn(m_DrvName);
    Fn.FnContract();
    m_DrvName=Fn();
    Fn.FnExpand();
    bDrvChanged=1;
    if (gs_pPrj->ReloadDrvManager(FALSE, Fn()))
      {
      gs_pPrj->sDrvManagerName = m_DrvName;
      bDrvChanged = 0;
      UpdateDialogControls(this, FALSE);
      }
    }
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CCommsPage::OnArchiveBrowse()
  {
  UpdateData(true);
  Strng FName,FPath;

  if (m_ArcName.GetLength()>0)
    {
    FPath.FnDrivePath((char*)(const char*)m_ArcName);
    FName.FnNameExt((char*)(const char*)m_ArcName);
    }
  else
    {
    FName = "*.sac";
    FPath = "..\\";//PrjPrevDirectory();
    }

  CSCDFileDialog Dlg(TRUE, NULL, FName(), OFN_NOCHANGEDIR | OFN_HIDEREADONLY,
    "SysCAD Archive Configuration (*.sac)|*.sac||", this);
  Dlg.m_ofn.lpstrInitialDir = FPath();
  Dlg.m_ofn.lpstrTitle = "Browse";
  if (Dlg.DoModal()==IDOK)
    {
    m_ArcName=Dlg.GetPathName();
    Strng Fn(m_ArcName);
    Fn.FnContract();
    m_ArcName=Fn();
    Fn.FnExpand();
    bArcChanged=1;
    if (gs_pPrj->ReloadArcManager(FALSE, Fn()))
      {
      gs_pPrj->sArcManagerName = m_ArcName;
      bArcChanged = 0;
      UpdateDialogControls(this, FALSE);
      }
    }
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CCommsPage::OnArchiveOn()
  {
  UpdateData(true);
  bArcChanged = 1;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CCommsPage::OnArchiveReload()
  {
  UpdateData(true);
  Strng Fn(m_ArcName);
  if (SymbolicPaths())
    {
    Fn.FnContract();
    m_ArcName=Fn();
    Fn.FnExpand();
    }
  else
    {
    Fn.FnExpand();
    m_ArcName=Fn();
    }
  UpdateData(false);

  gs_pPrj->bArcChanged = bArcChanged;
  if (1)// bArcChanged)
    {
    gs_pPrj->CloseArcManager();
    gs_pPrj->sArcManagerName.FnContract((char*)(const char*)m_ArcName);
    gs_pPrj->bArcOn          = m_ArcOn;
    gs_pPrj->bArcOpenDBOnRun = m_ArcOpenDBOnRun;
    gs_pPrj->OpenArcManager();
    CTagVwDoc::RebuildAll();
    bArcChanged = 0;
    UpdateDialogControls(this, FALSE);
    }
  //if (gs_pPrj->ReloadArcManager(FALSE, Fn()))
  //  {
  //  gs_pPrj->sArcManagerName = m_ArcName;
  //  bArcChanged = 0;
  //  UpdateDialogControls(this, FALSE);
  //  }
  }

void CCommsPage::OnUpdateReLoadArc(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_ArcOn);
  }

//---------------------------------------------------------------------------

void CCommsPage::OnArchiveChanged()
  {
  bArcChanged=1;
  }

//---------------------------------------------------------------------------

void CCommsPage::OnPrjarcopenonrun()
  {
  UpdateData(true);
  bArcChanged = 1;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CCommsPage::OnUpdateArchiveOn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_ArcOn);
  }

//---------------------------------------------------------------------------

void CCommsPage::OnOPCReload()
  {
  //UpdateData(true);
  //gs_pPrj->ReloadOPCServer();
//  gs_pPrj->CloseOPCManager();
  //gs_pPrj->bOPCOn = m_OPCOn;
  //gs_pPrj->iOPCServerNo = m_OPCServerNo;
  //gs_pPrj->bOPCResetReg = m_OPCResetReg;
  //bOPCChanged = 0;
//  gs_pPrj->OpenOPCManager();
  }

void CCommsPage::OnUpdateReLoadOPC(CCmdUI* pCmdUi)
  {//can only reload if the driver was on
  pCmdUi->Enable(false);//gs_pPrj->bOPCOn);
  }

//---------------------------------------------------------------------------

void CCommsPage::OnOpcChanged()
  {
  bOPCChanged = 1;
  }

//---------------------------------------------------------------------------

void CCommsPage::OnDdeChanged()
  {
  bDDEChanged = 1;
  }

//---------------------------------------------------------------------------

void CCommsPage::OnIOMBrowse()
  {
  UpdateData(true);
  Strng FName,FPath;

  if (m_IOMName.GetLength()>0)
    {
    FPath.FnDrivePath((char*)(const char*)m_IOMName);
    FName.FnNameExt((char*)(const char*)m_IOMName);
    }
  else
    {
    FName = "*.scm";
    FPath = "..\\";
    }

  CSCDFileDialog Dlg(TRUE, NULL, FName(), OFN_NOCHANGEDIR | OFN_HIDEREADONLY,
                     "SysCAD IOMarshal(*.scm)|*.scm||", this);
  Dlg.m_ofn.lpstrInitialDir = FPath();
  Dlg.m_ofn.lpstrTitle = "Browse";
  if (Dlg.DoModal()==IDOK)
    {
    m_IOMName=Dlg.GetPathName();
    Strng Fn(m_IOMName);
    Fn.FnContract();
    m_IOMName=Fn();
    Fn.FnExpand();
    bIOMChanged=1;
    if (gs_pPrj->ReloadIOMarshal(FALSE, Fn(), NULL))
      {
      gs_pPrj->sIOMarshalName = m_IOMName;
      gs_pPrj->sIOMarshalNode = m_IOMNode;
      bIOMChanged = 0;
      UpdateDialogControls(this, FALSE);
      }
    }
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CCommsPage::OnPrjIOMOn()
  {
  UpdateData(true);
  bIOMChanged = 1;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CCommsPage::OnIOMChanged()
  {
  bIOMChanged = 1;
  }

//---------------------------------------------------------------------------

void CCommsPage::OnUpdateIOMOn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_IOMOn);
  }

//---------------------------------------------------------------------------

void CCommsPage::OnCommsapply()
  {
  if (bDrvChanged || bArcChanged || bIOMChanged || bOPCChanged || bDDEChanged)
  	OnOK();
  bDrvChanged=0;
  bArcChanged=0;
  bIOMChanged=0;
  bOPCChanged=0;
  bDDEChanged=0;
  }

//===========================================================================

#if WITHNETSERVER

const UINT TIMER_ID = 11;

IMPLEMENT_DYNCREATE(CNetPage, CCustomPropertyPage)

CNetPage::CNetPage() : CCustomPropertyPage(CNetPage::IDD)
  {
  bNetChanged = 0;
  ClientCoupling = XC_None;
  //{{AFX_DATA_INIT(CNetPage)
  m_ServerConns = 0;
  m_ServerEnabled = FALSE;
  m_ClientConnServer = _T("");
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CNetPage::~CNetPage()
  {
  }

//---------------------------------------------------------------------------

void CNetPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CNetPage)
  DDX_Text(pDX, IDC_SRVCONNS, m_ServerConns);
  DDX_Check(pDX, IDC_SRVENABLE, m_ServerEnabled);
  DDX_Text(pDX, IDC_CLNTCONNECTSRV, m_ClientConnServer);
  //}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
    switch(GetCheckedRadioButton(IDC_CLIENT_NONE, IDC_CLIENT_TIGHT))
      {
      case IDC_CLIENT_LOOSE: ClientCoupling = XC_ASync; break;
      case IDC_CLIENT_TIGHT: ClientCoupling = XC_Sync ; break;
      case IDC_CLIENT_NONE:
      default:               ClientCoupling = XC_None;  break;
      }
  else
    {
    int Id=0;
    switch(ClientCoupling)
      {
      case XC_ASync: Id=IDC_CLIENT_LOOSE; break;
      case XC_Sync : Id=IDC_CLIENT_TIGHT; break;
      case XC_None:
      default:       Id=IDC_CLIENT_NONE;  break;
      }
    CheckRadioButton(IDC_CLIENT_NONE, IDC_CLIENT_TIGHT, Id);
    }

  if (!pDX->m_bSaveAndValidate)
    {
    CButton * pButton=(CButton *)GetDlgItem(ID_LOOPBACK);
    if (pCS_Mngr->ClientConnected() && pButton)
      pButton->SetCheck(pCS_Mngr->ClientLoopBackTest());

    CEdit *pEdit=(CEdit *)GetDlgItem(IDC_CLNTCONNECTSRV);
    pEdit->EnableWindow(!pCS_Mngr->ClientConnected());
    }
  }

//---------------------------------------------------------------------------

BOOL CNetPage::OnInitDialog()
  {
  m_ServerConns      = pCS_Mngr->ServerConnects();
  m_ServerEnabled    = pCS_Mngr->ServerEnabled();
  GetClientCoupling();
  m_ClientConnServer = pCS_Mngr->ClientServer();

  CCustomPropertyPage::OnInitDialog();
  SetTimer(TIMER_ID, 1000, NULL);
  return TRUE;  // return TRUE  unless you set the focus to a control
  }

//---------------------------------------------------------------------------

void CNetPage::OnOK()
  {
  if (bDidInit)
    {
    KillTimer(TIMER_ID);
    UpdateData(true);
    if (bNetChanged)
      {
      gs_pPrj->bNetChanged = bNetChanged;
      pCS_Mngr->SetServer(m_ServerEnabled);
      pCS_Mngr->SetClientConnect(ClientCoupling!=XC_None, m_ClientConnServer.GetBuffer(0), ClientCoupling==XC_Sync);
      }
    }
  }

//---------------------------------------------------------------------------

void CNetPage::OnCancel()
  {
  if (bDidInit)
    KillTimer(TIMER_ID);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CNetPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CNetPage)
  ON_EN_CHANGE(IDC_CLNTCONNECTSRV, OnChangeClntconnectsrv)
  ON_BN_CLICKED(IDC_SRVENABLE, OnSrvenable)
  ON_BN_CLICKED(ID_REFRESHSRV, OnRefreshsrv)
  ON_WM_TIMER()
  ON_BN_CLICKED(ID_LOOPBACK, OnLoopback)
  ON_BN_CLICKED(IDC_CLIENT_NONE, OnClientNone)
  ON_BN_CLICKED(IDC_CLIENT_LOOSE, OnClientLoose)
  ON_BN_CLICKED(IDC_CLIENT_TIGHT, OnClientTight)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
/*
void CNetPage::OnClientconnect()
  {
  UpdateData(true);
  bNetChanged=1;

  CheckDlgButton(IDC_CLIENTCONNECT, m_ClientConnect ? 0 : 1); // Prevent update until done

  if (m_ClientConnect)
    {
    CNetClntSrvr Dlg(gs_pPrj, this);
    Dlg.DoModal();
    }
  else
    {
    pCS_Mngr->SetClientConnect(false, pCS_Mngr->ClientServer());
    }
  GetClientCoupling();
  m_ClientConnServer = pCS_Mngr->ClientServer();
  m_ServerConns      = pCS_Mngr->ServerConnects();
  m_ServerEnabled    = pCS_Mngr->ServerEnabled();

  UpdateData(false);
  }
*/
//---------------------------------------------------------------------------

void CNetPage::OnChangeClntconnectsrv()
  {
  bNetChanged = 1;
  }

//---------------------------------------------------------------------------

void CNetPage::OnSrvenable()
  {
  UpdateData(true);
  flag DoIt = 1;
  if (!m_ServerEnabled)
    if (pCS_Mngr->ServerConnects()>0)
      if (AfxMessageBox("Server still has active Connections\nShut it Down?", MB_YESNO|MB_ICONQUESTION)==IDNO)
        DoIt = 0;
  if (DoIt)
    pCS_Mngr->SetServer(m_ServerEnabled);

  m_ServerConns      = pCS_Mngr->ServerConnects();
  m_ServerEnabled    = pCS_Mngr->ServerEnabled();
  GetClientCoupling();//m_ClientConnect    = pCS_Mngr->ClientConnected();
  m_ClientConnServer = pCS_Mngr->ClientServer();

  UpdateData(false);
  bNetChanged = 1;
  }

//---------------------------------------------------------------------------

void CNetPage::OnRefreshsrv()
  {
  m_ServerConns      = pCS_Mngr->ServerConnects();
  m_ServerEnabled    = pCS_Mngr->ServerEnabled();
  GetClientCoupling();//m_ClientConnect    = pCS_Mngr->ClientConnected();
  m_ClientConnServer = pCS_Mngr->ClientServer();

  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CNetPage::OnTimer(UINT nIDEvent)
  {
  if (nIDEvent==TIMER_ID)
    {
    m_ServerConns = pCS_Mngr->ServerConnects();
    SetDlgItemInt(IDC_SRVCONNS, m_ServerConns);
    }
  CCustomPropertyPage::OnTimer(nIDEvent);
  }

//---------------------------------------------------------------------------

void CNetPage::OnLoopback()
  {
  if (pCS_Mngr->ClientConnected())
    {
    if (pCS_Mngr->ClientLoopBackTest())
      pCS_Mngr->ClientLoopBackTest(true, false);
    else
      pCS_Mngr->ClientLoopBackTest(false, true);
    }
  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CNetPage::OnClientNone()
  {
  UpdateData(true);
  bNetChanged = 1;
  gs_Exec.SetCoupling(XC_None);
  if (pCS_Mngr->ClientConnected())
    pCS_Mngr->SetClientConnect(false, pCS_Mngr->ClientServer(), false);

  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CNetPage::OnClientLoose()
  {
  UpdateData(true);
  bNetChanged = 1;

  if (!pCS_Mngr->ServerEnabled())
    {
    pCS_Mngr->SetServer(true); // Enable the server
    //Sleep(5000);

    m_ServerConns      = pCS_Mngr->ServerConnects();
    m_ServerEnabled    = pCS_Mngr->ServerEnabled();
    GetClientCoupling();//m_ClientConnect    = pCS_Mngr->ClientConnected();
    m_ClientConnServer = pCS_Mngr->ClientServer();
    }

  if (!pCS_Mngr->ClientConnected())
    {
    int err=pCS_Mngr->SetClientConnect(true, m_ClientConnServer.GetBuffer(0), false/*Sync*/);
    //CNetClntSrvr Dlg(false /*ASync*/, this);
    //Dlg.DoModal();
    }

  gs_Exec.SetCoupling(XC_ASync);

  GetClientCoupling();
  m_ClientConnServer = pCS_Mngr->ClientServer();

  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CNetPage::OnClientTight()
  {
  UpdateData(true);
  AfxMessageBox("Tightly Coupled Syscad not yet available", MB_OK);

  if (0)
    {
    bNetChanged = 1;

    if (!pCS_Mngr->ClientConnected())
      {
      int err=pCS_Mngr->SetClientConnect(true, m_ClientConnServer.GetBuffer(0), true/*Sync*/);
      //CNetClntSrvr Dlg(true /*Sync*/, this);
      //Dlg.DoModal();
      }

    // Set ClientCoupling=XC_Tight;
    gs_Exec.SetCoupling(XC_Sync);
    }

  GetClientCoupling();
  m_ClientConnServer = pCS_Mngr->ClientServer();

  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CNetPage::GetClientCoupling()
  {
  if (pCS_Mngr->ClientConnected())
    {
    ClientCoupling=gs_Exec.Coupling();
    }
  else
    ClientCoupling=XC_None;
  }
#endif

//===========================================================================

CMergeDlg::CMergeDlg(CMergeProjectsInfo* MPI, CWnd* pParent /*=NULL*/)
  : CDialog(CMergeDlg::IDD, pParent)
  {
  pTTC = NULL;
  pMPI = MPI;
  bTagNamingChanged = FALSE;
  //{{AFX_DATA_INIT(CMergeDlg)
  m_MergeReportFile = _T("");
  m_ModelCfg = _T("");
  m_SlavePrj = _T("");
  m_MasterRenameType = -1;
  m_SlaveRenameType = -1;
  m_MasterPrefixSuffix = _T("");
  m_SlavePrefixSuffix = _T("");
  m_MasterChangeType = -1;
  m_SlaveChangeType = -1;
  m_TagAppendChars = _T("");
  m_MasterNumTagsOK = !FALSE;
  m_MasterStNumTagsOK = !FALSE;
  m_SlaveNumTagsOK = !FALSE;
  m_SlaveStNumTagsOK = !FALSE;
  //}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

CMergeDlg::~CMergeDlg()
  {
  delete pTTC;
  }

//--------------------------------------------------------------------------

void CMergeDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMergeDlg)
  DDX_Text(pDX, IDC_MRGREPORTFILE, m_MergeReportFile);
  DDX_Text(pDX, IDC_MDLCFG, m_ModelCfg);
  DDX_Text(pDX, IDC_SLAVEPRJ, m_SlavePrj);
  DDX_Radio(pDX, IDC_MSTRPREFIX, m_MasterRenameType);
  DDX_Radio(pDX, IDC_SLVPREFIX, m_SlaveRenameType);
  DDX_Text(pDX, IDC_MSTRPRESUF, m_MasterPrefixSuffix);
  DDX_Text(pDX, IDC_SLVPRESUF, m_SlavePrefixSuffix);
  DDX_Radio(pDX, IDC_MSTRCHANGEALL, m_MasterChangeType);
  DDX_Radio(pDX, IDC_SLVCHANGEALL, m_SlaveChangeType);
  DDX_Text(pDX, IDC_APPENDCHARS, m_TagAppendChars);
  DDX_Check(pDX, IDC_MSTRNUMTAGSOK, m_MasterNumTagsOK);
  DDX_Check(pDX, IDC_MSTRSTNUMTAGSOK, m_MasterStNumTagsOK);
  DDX_Check(pDX, IDC_SLVNUMTAGSOK, m_SlaveNumTagsOK);
  DDX_Check(pDX, IDC_SLVSTNUMTAGSOK, m_SlaveStNumTagsOK);
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CMergeDlg, CDialog)
  //{{AFX_MSG_MAP(CMergeDlg)
  ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
  ON_BN_CLICKED(IDC_MERGE, OnMerge)
  ON_EN_CHANGE(IDC_SLAVEPRJ, OnChangeSlaveprj)
  ON_BN_CLICKED(IDC_MSTRSTNUMTAGSOK, OnMstrstnumtags)
  ON_BN_CLICKED(IDC_MSTRNUMTAGSOK, OnMstrnumtags)
  //}}AFX_MSG_MAP
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
  ON_BN_CLICKED(IDC_MSTRPREFIX, OnMasterPrefixSuffix)
  ON_BN_CLICKED(IDC_MSTRSUFFIX, OnMasterPrefixSuffix)
  ON_BN_CLICKED(IDC_SLVPREFIX, OnSlavePrefixSuffix)
  ON_BN_CLICKED(IDC_SLVSUFFIX, OnSlavePrefixSuffix)
  ON_BN_CLICKED(IDC_MSTRCHANGEALL, OnChangeType)
  ON_BN_CLICKED(IDC_MSTRCHANGEDUP, OnChangeType)
  ON_BN_CLICKED(IDC_MSTRCHANGENONE, OnChangeType)
  ON_BN_CLICKED(IDC_SLVCHANGEALL, OnChangeType)
  ON_BN_CLICKED(IDC_SLVCHANGEDUP, OnChangeType)
  ON_BN_CLICKED(IDC_SLVCHANGENONE, OnChangeType)
  ON_UPDATE_COMMAND_UI(IDC_MSTRPREFIX, OnUpdateMasterPrefixSuffix)
  ON_UPDATE_COMMAND_UI(IDC_MSTRSUFFIX, OnUpdateMasterPrefixSuffix)
  ON_UPDATE_COMMAND_UI(IDC_MSTRPRESUF, OnUpdateMasterPrefixSuffix)
  ON_UPDATE_COMMAND_UI(IDC_SLVPREFIX, OnUpdateSlavePrefixSuffix)
  ON_UPDATE_COMMAND_UI(IDC_SLVSUFFIX, OnUpdateSlavePrefixSuffix)
  ON_UPDATE_COMMAND_UI(IDC_SLVPRESUF, OnUpdateSlavePrefixSuffix)
  ON_UPDATE_COMMAND_UI(IDC_MSTRNUMTAGSOK,   OnUpdateMasterNumTags)
  ON_UPDATE_COMMAND_UI(IDC_MSTRSTNUMTAGSOK, OnUpdateMasterStNumTags)
  ON_UPDATE_COMMAND_UI(IDC_SLVNUMTAGSOK,    OnUpdateSlaveNumTags)
  ON_UPDATE_COMMAND_UI(IDC_SLVSTNUMTAGSOK,  OnUpdateSlaveStNumTags)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CMergeDlg::OnInitDialog()
  {
  PopulateDialog();
  CDialog::OnInitDialog();
  pTTC = new CCustomToolTipCtrl(this);
  SetDlgItemText(IDC_TXTERR, "");
  Strng s;
  s.Set("Current project : %s", PrjName());
  SetDlgItemText(IDC_TXTMSTRPRJ, s());
  UpdateDialogControls(this, FALSE);
  UpdateSlaveNumTagsOK();
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CMergeDlg::PreTranslateMessage(MSG* pMsg)
  {
  if (pTTC && HelpMngr.ShowToolTips())
    pTTC->RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CMergeDlg::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
  if (pTTC && HelpMngr.ShowToolTips())
    return pTTC->OnToolTipNotify(pNMHDR, CMergeDlg::IDD);
  return FALSE;
  }

//--------------------------------------------------------------------------

void CMergeDlg::PopulateDialog()
  {
  m_MergeReportFile = pMPI->sReportFile();
  m_ModelCfg = pMPI->sMasterModelCfg();
  m_SlavePrj = pMPI->sSlavePrj();
  m_TagAppendChars = pMPI->sTagAppendChars();
  m_MasterChangeType = pMPI->iMasterChangeType;
  m_SlaveChangeType = pMPI->iSlaveChangeType;
  m_MasterRenameType = pMPI->iMasterRenameType;
  m_SlaveRenameType = pMPI->iSlaveRenameType;
  m_MasterPrefixSuffix = m_MasterRenameType==MRG_Prefix ? pMPI->sMasterPrefix() : pMPI->sMasterSuffix();
  m_SlavePrefixSuffix = m_SlaveRenameType==MRG_Prefix ? pMPI->sSlavePrefix() : pMPI->sSlaveSuffix();

  m_MasterNumTagsOK = !pMPI->bMasterNumTagsBAD;
  m_MasterStNumTagsOK = !pMPI->bMasterStNumTagsBAD;
  }

//--------------------------------------------------------------------------

void CMergeDlg::OnBrowse()
  {
  UpdateData(TRUE);
  Strng FName,FPath;
  if (m_SlavePrj.GetLength()>0)
    {
    FPath.FnDrivePath((char*)(const char*)m_SlavePrj);
    FName.FnNameExt((char*)(const char*)m_SlavePrj);
    }
  else
    {
    FName = "*.spj";
    FPath = "..\\";//PrjPrevDirectory();
    }

  CSCDFileDialog Dlg(TRUE, NULL, FName(), OFN_NOCHANGEDIR | OFN_HIDEREADONLY, "SysCAD project (*.spj)|*.spj||", this);
  Dlg.m_ofn.lpstrInitialDir = FPath();
  Dlg.m_ofn.lpstrTitle = "Browse";
  if (Dlg.DoModal()==IDOK)
    {
    m_SlavePrj = Dlg.GetPathName();
    UpdateSlaveNumTags();
    UpdateSlaveNumTagsOK();
    }

  UpdateData(FALSE);
  }

//--------------------------------------------------------------------------

void CMergeDlg::UpdateSlaveNumTags()
  {
  if (FileExists((char*)(LPCTSTR)m_SlavePrj))
    {
    CProfINIFile SlavePF((char*)(LPCTSTR)m_SlavePrj);
    m_SlaveNumTagsOK = !(SlavePF.RdLong("General", "NumericTagsBad", TaggedObject::NumericTagsBad) != 0);
    m_SlaveStNumTagsOK = !(SlavePF.RdLong("General", "NumericStartingTagsBad", TaggedObject::NumericStartingTagsBad) != 0);
    }
  }

void CMergeDlg::UpdateSlaveNumTagsOK()
  {
  flag OK=FileExists((char*)(LPCTSTR)m_SlavePrj);
  if (!OK)
    SetDlgItemText(IDC_TXTERR, "ERROR : No merge project");
  else
    {
    if (m_SlaveStNumTagsOK && !m_MasterStNumTagsOK)
      OK=FALSE;
    if (m_SlaveNumTagsOK && !m_MasterNumTagsOK)
      OK=FALSE;
    if (!OK)
      SetDlgItemText(IDC_TXTERR, "ERROR : Numeric tag setting's Not Valid");
    else
      SetDlgItemText(IDC_TXTERR, "");
    }
  GetDlgItem(IDC_MERGE)->EnableWindow(OK);
  }

//--------------------------------------------------------------------------

void CMergeDlg::OnMerge()
  {
  UpdateData(TRUE);
  pMPI->sReportFile = (const char*)m_MergeReportFile;
  pMPI->sSlavePrj = (const char*)m_SlavePrj;
  pMPI->sTagAppendChars = (const char*)m_TagAppendChars;
  pMPI->iMasterChangeType = m_MasterChangeType;
  pMPI->iSlaveChangeType = m_SlaveChangeType;
  pMPI->iMasterRenameType = m_MasterRenameType;
  pMPI->iSlaveRenameType = m_SlaveRenameType;
  if (m_MasterRenameType==MRG_Prefix)
    pMPI->sMasterPrefix = (const char*)m_MasterPrefixSuffix;
  else
    pMPI->sMasterSuffix = (const char*)m_MasterPrefixSuffix;
  if (m_SlaveRenameType==MRG_Prefix)
    pMPI->sSlavePrefix = (const char*)m_SlavePrefixSuffix;
  else
    pMPI->sSlaveSuffix = (const char*)m_SlavePrefixSuffix;


  int TagsOK=true;
  if (bTagNamingChanged)
    {
    int TagsChngd = gs_Exec.CheckAllTags(!m_MasterNumTagsOK, !m_MasterStNumTagsOK);
    if (TagsChngd<0)
      {
      TagsOK=false;
      pMPI->sError="Numeric tags need to be changed";
      //TaggedObject::NumericTagsBad = 0;
      //TaggedObject::NumericStartingTagsBad = 0;
      }
    else
      {
      pMPI->bMasterNumTagsBAD   = !m_MasterNumTagsOK;
      pMPI->bMasterStNumTagsBAD = !m_MasterStNumTagsOK;
//      TaggedObject::NumericTagsBad = !m_MasterNumTagsOK;
//      TaggedObject::NumericStartingTagsBad = !m_MasterStNumTagsOK;
//      if (TagsChngd>0)
//        {
//        CWaitCursor Wait;
//        gs_pPrj->CloseDrvManager();
//        gs_pPrj->OpenDrvManager();
//        CTagVwDoc::RebuildAll();
//        }
      }
    }
  else
    {
    pMPI->bMasterNumTagsBAD   = !m_MasterNumTagsOK;
    pMPI->bMasterStNumTagsBAD = !m_MasterStNumTagsOK;
    }

  //check validity of merge...
  int err =0;
  if (TagsOK)
    err=gs_Exec.CheckMerge(*pMPI);
  PopulateDialog();
  UpdateData(FALSE);
  if (!TagsOK || err!=0)
    {
    //Beep(1000, 20);
    Strng s("ERROR : ");
    s += pMPI->sError;
    SetDlgItemText(IDC_TXTERR, s());
    LogError("Merge", 0, pMPI->sError());
    }
  else
    CDialog::OnOK();
  }

//--------------------------------------------------------------------------

void CMergeDlg::OnChangeType()
  {
  UpdateData(TRUE);
  UpdateDialogControls(this, FALSE);
  }

void CMergeDlg::OnUpdateMasterPrefixSuffix(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_MasterChangeType!=MRG_ChangeNone);
  }

void CMergeDlg::OnUpdateSlavePrefixSuffix(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_SlaveChangeType!=MRG_ChangeNone);
  }

void CMergeDlg::OnUpdateMasterNumTags(CCmdUI* pCmdUi)
  {
  };
void CMergeDlg::OnUpdateMasterStNumTags(CCmdUI* pCmdUi)
  {
  };
void CMergeDlg::OnUpdateSlaveNumTags(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(FALSE);
  };
void CMergeDlg::OnUpdateSlaveStNumTags(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(FALSE);
  };
//--------------------------------------------------------------------------

void CMergeDlg::OnMasterPrefixSuffix()
  {
  byte PrevRenameType = m_MasterRenameType;
  UpdateData(TRUE);
  if (PrevRenameType==MRG_Prefix)
    pMPI->sMasterPrefix = (const char*)m_MasterPrefixSuffix;
  else
    pMPI->sMasterSuffix = (const char*)m_MasterPrefixSuffix;
  m_MasterPrefixSuffix = m_MasterRenameType==MRG_Prefix ? pMPI->sMasterPrefix() : pMPI->sMasterSuffix();
  UpdateData(FALSE);
  }

//--------------------------------------------------------------------------

void CMergeDlg::OnSlavePrefixSuffix()
  {
  byte PrevRenameType = m_SlaveRenameType;
  UpdateData(TRUE);
  if (PrevRenameType==MRG_Prefix)
    pMPI->sSlavePrefix = (const char*)m_SlavePrefixSuffix;
  else
    pMPI->sSlaveSuffix = (const char*)m_SlavePrefixSuffix;
  m_SlavePrefixSuffix = m_SlaveRenameType==MRG_Prefix ? pMPI->sSlavePrefix() : pMPI->sSlaveSuffix();
  UpdateData(FALSE);
  }

//--------------------------------------------------------------------------

void CMergeDlg::OnChangeSlaveprj()
  {
  UpdateData(TRUE);
  UpdateSlaveNumTags();
  UpdateSlaveNumTagsOK();
  UpdateData(FALSE);
  }

//--------------------------------------------------------------------------

void CMergeDlg::OnMstrstnumtags()
  {
  UpdateData(TRUE);
  bTagNamingChanged = 1;
  m_MasterNumTagsOK;
  m_MasterStNumTagsOK;
  m_MasterNumTagsOK = m_MasterNumTagsOK && m_MasterStNumTagsOK;
  UpdateSlaveNumTagsOK();
  UpdateData(FALSE);
  }

//--------------------------------------------------------------------------

void CMergeDlg::OnMstrnumtags()
  {
  UpdateData(TRUE);
  bTagNamingChanged = 1;
  m_MasterStNumTagsOK = m_MasterStNumTagsOK || m_MasterNumTagsOK;
  UpdateSlaveNumTagsOK();
  UpdateData(FALSE);
  }

//===========================================================================

CMergeReportDlg::CMergeReportDlg(CMergeProjectsInfo* MPI, CWnd* pParent /*=NULL*/)
  : CDialog(CMergeReportDlg::IDD, pParent)
  {
  pMPI = MPI;
  //{{AFX_DATA_INIT(CMergeReportDlg)
  //}}AFX_DATA_INIT
  bMergeAllowed = 1;
  sMessage = "Press 'Continue' to merge projects";
  }

//--------------------------------------------------------------------------

void CMergeReportDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMergeReportDlg)
  DDX_Control(pDX, IDC_REPLIST, m_List);
  DDX_Control(pDX, IDC_REPORTTAB, m_Tab);
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CMergeReportDlg, CDialog)
  //{{AFX_MSG_MAP(CMergeReportDlg)
  ON_NOTIFY(TCN_SELCHANGE, IDC_REPORTTAB, OnSelchangeReportTab)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOK)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CMergeReportDlg::OnInitDialog()
  {
  iPrevTab = -2;
  CDialog::OnInitDialog();
  TC_ITEM Item;
  Item.mask = TCIF_TEXT;
  Item.pszText = "Summary";
  m_Tab.InsertItem(0, &Item);
  //Item.pszText = "Current";
  Item.pszText = "Tags changed in current";
  m_Tab.InsertItem(1, &Item);
  //Item.pszText = "Imported";
  Item.pszText = "Tags changed in imported";
  m_Tab.InsertItem(2, &Item);
  Item.pszText = "Referenced Files";
  m_Tab.InsertItem(3, &Item);
  Item.pszText = "Other";
  m_Tab.InsertItem(4, &Item);
  Item.pszText = "Report File";
  m_Tab.InsertItem(5, &Item);
  m_Tab.SetCurSel(0);
  m_List.SetHeaderCount(1);
  m_List.SetHeaderItem(0, "?", 20);
  m_List.MakeHeader(34765);
  PopulateTabList();
  SetDlgItemText(IDC_TXTMESSAGE, sMessage());
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//--------------------------------------------------------------------------

void CMergeReportDlg::PopulateTabList()
  {
  int CurTab = m_Tab.GetCurSel();
  if (CurTab!=iPrevTab)
    {
    Strng s;
    m_List.ResetContent();
    switch (CurTab)
      {
      case -1:
      case 0:
        {
        m_List.SetHeaderCount(3);
        RECT Rect;
        m_List.GetWindowRect(&Rect);
        const int Width = Rect.right - Rect.left;
        m_List.SetHeaderItem(0, "Item", (int)(Width*0.54));
        m_List.SetHeaderItem(1, "Current", (int)(Width*0.23));
        m_List.SetHeaderItem(2, "Imported", (int)(Width*0.23));
        m_List.UpdateHeader();
        Strng s1,s2;
        s1.FnNameExt(PrjName());
        s2.FnNameExt(pMPI->sSlavePrj());
        s.Set("Project\t%s\t%s", s1(), s2());
        m_List.AddString(s());
        s.Set("Tags (units/models)\t%d\t%d", pMPI->iMasterTagCnt, pMPI->iSlaveTagCnt);
        m_List.AddString(s());
        s.Set("Tags to be changed\t%d\t%d", pMPI->NoPreChangeTags(), pMPI->NoPostChangeTags());
        m_List.AddString(s());
        s.Set("Changed tags requiring additional characters\t%d\t%d", pMPI->iMasterExtraChngCnt, pMPI->iSlaveExtraChngCnt);
        m_List.AddString(s());
        s.Set("Trend windows\t%d\t%d", pMPI->MasterTrends.GetSize(), pMPI->SlaveTrends.GetSize());
        m_List.AddString(s());
        s.Set("Graphics windows\t%d\t%d", pMPI->MasterGraphics.GetSize(), pMPI->SlaveGraphics.GetSize());
        m_List.AddString(s());
        s.Set("Trend windows to be changed\t0\t%d", pMPI->PostChangeTrends.GetSize());
        m_List.AddString(s());
        s.Set("Graphics windows to be changed\t0\t%d", pMPI->PostChangeGraphics.GetSize());
        m_List.AddString(s());
        break;
        }
      case 1:
      case 2:
        {
        m_List.SetHeaderCount(3);
        RECT Rect;
        m_List.GetWindowRect(&Rect);
        const int Width = Rect.right - Rect.left;
        m_List.SetHeaderItem(0, CurTab==1 ? "Current tag" : "Imported tag", (int)(Width*0.37));
        m_List.SetHeaderItem(1, "New tag", (int)(Width*0.37));
        m_List.SetHeaderItem(2, "Extra characters appended", (int)(Width*0.26));
        m_List.UpdateHeader();
        if (CurTab==1)
          {
          if (pMPI->NoPreChangeTags()>0)
            {
            for (int i=0; i<pMPI->PreNewTags.GetSize(); i++)
              {
              s.Set("%s\t%s\t%s", pMPI->PreChangeTags[i](), pMPI->PreNewTags[i](), pMPI->PreFlags[i] ? "Yes" : "");
              m_List.AddString(s());
              }
            }
          else
            m_List.AddString("No tags in the current project are being changed");
          }
        else
          {
          if (pMPI->NoPostChangeTags()>0)
            {
            for (int i=0; i<pMPI->PostNewTags.GetSize(); i++)
              {
              s.Set("%s\t%s\t%s", pMPI->PostChangeTags[i](), pMPI->PostNewTags[i](), pMPI->PostFlags[i] ? "Yes" : "");
              m_List.AddString(s());
              }
            }
          else
            m_List.AddString("No tags in the imported project are being changed");
          }
        break;
        }
      case 3:
        {
        int i;
        RECT Rect;
        m_List.GetWindowRect(&Rect);
        const int Width = Rect.right - Rect.left;
        if (pMPI->iProblemRefFiles>0)
          {
          if (pMPI->iRefFilesUnknownCnt>0)
            {
            m_List.SetHeaderCount(3);
            m_List.SetHeaderItem(0, "Problem filename", (int)(Width*0.44));
            m_List.SetHeaderItem(1, "Current Models", (int)(Width*0.28));
            m_List.SetHeaderItem(2, "Imported Models", (int)(Width*0.28));
            }
          else
            {
            m_List.SetHeaderCount(1);
            m_List.SetHeaderItem(0, "Info", Width);
            }
          }
        else
          {
          m_List.SetHeaderCount(1);
          m_List.SetHeaderItem(0, "Files to be copied into current project", Width);
          }
        m_List.UpdateHeader();
        if (pMPI->iProblemRefFiles>0)
          {
          if (pMPI->iRefFilesUnknownCnt>0)
            {
            Strng s1,s2;
            for (int i=0; i<pMPI->PostChangeRefFiles.GetSize(); i++)
              if (stricmp(pMPI->PostNewRefFiles[i](), "Unknown")==0)
                {
                s1.FnNameExt(pMPI->PostChangeRefFiles[i]());
                s.Set("%s\t", pMPI->PostChangeRefFiles[i]());
                int MasterCnt = 0;
                int SlaveCnt = 0;
                while (MasterCnt<pMPI->MasterRefFiles.GetSize() || SlaveCnt<pMPI->SlaveRefFiles.GetSize())
                  {
                  Strng ss;
                  while (ss.Len()==0 && MasterCnt<pMPI->MasterRefFiles.GetSize())
                    {
                    s2.FnNameExt(pMPI->MasterRefFiles[MasterCnt]());
                    if (stricmp(s1(), s2())==0)
                      ss = pMPI->MasterRefModels[MasterCnt]();
                    MasterCnt++;
                    }
                  if (ss.Len()>0)
                    {
                    s.Append("%s\t", ss());
                    ss = "";
                    }
                  else
                    s += "\t";
                  while (ss.Len()==0 && SlaveCnt<pMPI->SlaveRefFiles.GetSize())
                    {
                    s2.FnNameExt(pMPI->SlaveRefFiles[SlaveCnt]());
                    if (stricmp(s1(), s2())==0)
                      ss = pMPI->SlaveRefModels[SlaveCnt]();
                    SlaveCnt++;
                    }
                  s += ss;
                  m_List.AddString(s());
                  s.Set("\t");
                  }
                }
            m_List.AddString("");
            }
          if (pMPI->iRefFilesExistsCnt>0)
            {
            m_List.AddString("Import files that cannot be copied because files with the same name allready exist in the current project:");
            for (i=0; i<pMPI->PostChangeRefFiles.GetSize(); i++)
              if (stricmp(pMPI->PostNewRefFiles[i](), "Exists")==0)
                m_List.AddString(pMPI->PostChangeRefFiles[i]());
            m_List.AddString("");
            }
          if (pMPI->bIgnoreProbRefFiles && pMPI->iRefFilesUnknownCnt>0)
            {
            m_List.AddString("Warning: Referenced files from imported project that will NOT be copied to current project:");
            for (i=0; i<pMPI->PostChangeRefFiles.GetSize(); i++)
              if (stricmp(pMPI->PostNewRefFiles[i](), "Unknown")==0)
                m_List.AddString(pMPI->PostChangeRefFiles[i]());
            m_List.AddString("");
            }
          if (pMPI->iRefFilesUnknownCnt+pMPI->iRefFilesExistsCnt<pMPI->PostChangeRefFiles.GetSize())
            m_List.AddString("Files to be copied into current project:");
          }
        for (i=0; i<pMPI->PostChangeRefFiles.GetSize(); i++)
          if (stricmp(pMPI->PostNewRefFiles[i](), "Unknown")!=0 && stricmp(pMPI->PostNewRefFiles[i](), "Exists")!=0)
            m_List.AddString(pMPI->PostChangeRefFiles[i]());
        break;
        }
      case 4:
        {
        m_List.SetHeaderCount(2);
        RECT Rect;
        m_List.GetWindowRect(&Rect);
        const int Width = Rect.right - Rect.left;
        m_List.SetHeaderItem(0, "Item", (int)(Width*0.24));
        m_List.SetHeaderItem(1, "Info", (int)(Width*0.76));
        m_List.UpdateHeader();
        s.Set("Merge report file\t%s", pMPI->sReportFile());
        m_List.AddString(s());
        s.Set("Current project\t%s", PrjName());
        m_List.AddString(s());
        s.Set("Import project\t%s", pMPI->sSlavePrj());
        m_List.AddString(s());
        Strng s1,s2;
        if (pMPI->PostChangeTrends.GetSize()>0)
          {
          m_List.AddString("");
          m_List.AddString("Trend window names changed in import project:");
          for (int i=0; i<pMPI->PostChangeTrends.GetSize(); i++)
            {
            s1.FnNameExt(pMPI->PostChangeTrends[i]());
            s2.FnNameExt(pMPI->PostNewTrends[i]());
            s.Set("%s\t%s", s1(), s2());
            m_List.AddString(s());
            }
          }
        if (pMPI->PostChangeGraphics.GetSize()>0)
          {
          m_List.AddString("");
          m_List.AddString("Graphic window names changed in import project:");
          for (int i=0; i<pMPI->PostChangeGraphics.GetSize(); i++)
            {
            s1.FnNameExt(pMPI->PostChangeGraphics[i]());
            s2.FnNameExt(pMPI->PostNewGraphics[i]());
            s.Set("%s\t%s", s1(), s2());
            m_List.AddString(s());
            }
          }
        break;
        }
      case 5:
        {
        m_List.SetHeaderCount(1);
        RECT Rect;
        m_List.GetWindowRect(&Rect);
        const int Width = Rect.right - Rect.left;
        m_List.SetHeaderItem(0, "Info", Width);
        m_List.UpdateHeader();
        CStdioFile F;
        CFileException e;
        if (!F.Open(pMPI->sReportFile(), CFile::modeRead | CFile::typeText | CFile::shareDenyNone, &e))
          {
          s.Set("Cannot open report file %s to read. Error %d", pMPI->sReportFile(), e.m_cause);
          LogError("Merge", 0, s());
          m_List.AddString(s());
          break;
          }
        CString ss;
        while (F.ReadString(ss))
          m_List.AddString(ss);
        F.Close();
        break;
        }
      }
    }
  iPrevTab = CurTab;
  }

//--------------------------------------------------------------------------

void CMergeReportDlg::OnOK()
  {
  if (bMergeAllowed)
    CDialog::OnOK();
  else
    CDialog::OnCancel();
  }

void CMergeReportDlg::OnUpdateOK(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(bMergeAllowed);
  }

//--------------------------------------------------------------------------

void CMergeReportDlg::OnSelchangeReportTab(NMHDR* pNMHDR, LRESULT* pResult)
  {
  PopulateTabList();
  *pResult = 0;
  }


void CHistPage::OnBnClickedPrjhstusetol()
  {
  }


