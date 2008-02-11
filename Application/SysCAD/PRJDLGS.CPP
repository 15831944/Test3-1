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
#include "archmngr.h"
//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

class CModesPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CModesPage )
  public:
    CModesPage();
    ~CModesPage();

    CProject         * m_pPrj;
    CProjectSettings * m_pSet;

    flag    m_bTimeChanged:1;
    CTimeValue m_CurrentTimeAtInit;

    //{{AFX_DATA(CGenPage)
    enum { IDD = IDD_PRJMODESDLG };
    CString     m_CurrentTimeHMS;
    CString     m_TimeText;
    BOOL        m_SyncWithClock;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CGenPage)
  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CGenPage)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG

    void           DoSyncChange();
    void           LoadCBs(bool DoSwap);

  public:
    long           m_NetMode;

    long           m_PBNodeMode;
    long           m_PBLinkMode;
    long           m_PBHeatMode;
    long           m_PBFlowMode;

    long           m_DynNodeMode;
    long           m_DynLinkMode;
    long           m_DynHeatMode;
    long           m_DynFlowMode;

    long           m_PBMaxNodeMode;
    long           m_PBMaxLinkMode;
    long           m_PBMaxHeatMode;
    long           m_PBMaxFlowMode;

    long           m_DynMaxNodeMode;
    long           m_DynMaxLinkMode;
    long           m_DynMaxHeatMode;
    long           m_DynMaxFlowMode;

    long           m_NodeMode;
    long           m_LinkMode;
    long           m_HeatMode;
    long           m_FlowMode;

    long           m_MaxNodeMode;
    long           m_MaxLinkMode;
    long           m_MaxHeatMode;
    long           m_MaxFlowMode;


    DDBValueLstMem m_NetModes;
    DDBValueLstMem m_NodeModes;
    DDBValueLstMem m_LinkModes;
    DDBValueLstMem m_HeatModes;
    DDBValueLstMem m_FlowModes;

    CComboBox      m_NetModeCB;
    CComboBox      m_NodeModeCB;
    CComboBox      m_LinkModeCB;
    CComboBox      m_HeatModeCB;
    CComboBox      m_FlowModeCB;
  public:
    afx_msg void OnCbnSelchangePrjnetmodecombo();
    afx_msg void OnTimeChanged();
    afx_msg void OnDTimeChanged();
    afx_msg void OnKillfocusPrjCurrenttime();
    afx_msg void OnSynchronise();
    afx_msg void OnUpdateDynamic(CCmdUI* pCmdUi);
    afx_msg void OnUpdateCurrentTime(CCmdUI* pCmdUi);

    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CGrfTagsPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CGrfTagsPage)
  public:
    CGrfTagsPage();
    ~CGrfTagsPage();

    CProject         * m_pPrj;
    CProjectSettings * m_pSet;

    flag    m_bTagNamingChanged:1;
    CTimeValue m_CurrentTimeAtInit;

    //{{AFX_DATA(CGenPage)
    enum { IDD = IDD_PRJGRFTAGSDLG };
#if WithNumericTags
    BOOL        m_NumTagsOK;
#endif
    BOOL        m_NumStartingOK;
    CString     m_NumChar;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CGenPage)
  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CGenPage)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG

    void           SetLibraryOptions();
    void           SetFrameOptions();

  public:
    CEdit          m_Library;
    CComboBox      m_PageFrame;

    Strng          m_sDefGrpLib;
    Strng          m_sGrfFrameName;
    Strng          m_sGrfFrameFilter;
    CEdit          m_Filter;
  public:
    afx_msg void OnBnClickedLibrarybtn();
    afx_msg void OnEnChangeLibrary();
    afx_msg void OnEnChangeGrpfilter();
    afx_msg void OnNumTagsChanged();
    afx_msg void OnNumStTagsChanged();

    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CCommsPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CCommsPage)
  public:
    CCommsPage();
    ~CCommsPage();

    CProject         * m_pPrj;
    CProjectSettings * m_pSet;

    flag bDrvChanged,
      bArcChanged,
      bIOMChanged,
      bOPCChanged,
      bDDEChanged;

    //{{AFX_DATA(CCommsPage)
    enum { IDD = IDD_PRJCOMMSDLG };
#if WITHDRVMAN
    BOOL  m_DrvOn;
    BOOL  m_DrvLclTagSrvrOK;
    CString m_DrvName;
    BOOL  m_DrvReadAll;
#endif
    BOOL  m_OPCOn;
    BOOL  m_OPCServerNo;
    BOOL  m_OPCResetReg;
    BOOL  m_DDEOn;
    BOOL  m_ArcOn;
    BOOL  m_IOMOn;
    CString m_IOMName;
    CString m_IOMNode;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CCommsPage)
  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CCommsPage)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
#if WITHDRVMAN
    afx_msg void OnDriverOn();
    afx_msg void OnDriverChanged();
    afx_msg void OnDriverReload();
    afx_msg void OnDriverBrowse();
#endif
    afx_msg void OnOPCReload();
    afx_msg void OnOpcChanged();
    afx_msg void OnDdeChanged();
    //afx_msg void OnArchiveBrowse();
    //afx_msg void OnArchiveOn();
    //afx_msg void OnArchiveReload();
    //afx_msg void OnArchiveChanged();
    //afx_msg void OnPrjarcopenonrun();
    afx_msg void OnIOMBrowse();
    afx_msg void OnPrjIOMOn();
    afx_msg void OnIOMChanged();
    afx_msg void OnCommsApply();
    //}}AFX_MSG
#if WITHDRVMAN
    afx_msg void OnUpdateReLoadDrv(CCmdUI* pCmdUi);
    afx_msg void OnUpdateDriverOn(CCmdUI* pCmdUi);
#endif
    //afx_msg void OnUpdateReLoadArc(CCmdUI* pCmdUi);
    //afx_msg void OnUpdateArchiveOn(CCmdUI* pCmdUi);
    afx_msg void OnUpdateIOMOn(CCmdUI* pCmdUi);
    afx_msg void OnUpdateReLoadOPC(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CArchivePage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CArchivePage)
  public:
    CArchivePage();
    ~CArchivePage();


    //{{AFX_DATA(CArchivePage)
    enum { IDD = IDD_PRJARCHIVEDLG };
    CProject         * m_pPrj;
    CProjectSettings * m_pSet;

    HANDLE        m_hCfgProcess;            //process handle for the editor
    DWORD         m_dwCfgProcessId;          //process ID for the editor

    flag bArcChanged;
    CString m_ArcName;
    BOOL    m_ArcOn;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CArchivePage)
  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CArchivePage)
    virtual void OnOK();
    virtual BOOL OnInitDialog();

    void    GetOptions(LPCTSTR FileName);
    void    SetOptions(LPCTSTR FileName);

    afx_msg void OnArchiveBrowse();
    afx_msg void OnArchiveOn();
    afx_msg void OnArchiveReload();
    afx_msg void OnArchiveChanged();
    afx_msg void OnPrjarcopenonrun();
    afx_msg void OnArchiveApply();
    afx_msg void OnUpdateReLoadArc(CCmdUI* pCmdUi);
    afx_msg void OnUpdateArchiveOn(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  public:

    CButton m_MLTime;
    CButton m_MLTimeDate;
    CButton m_MLTimeTime;
    CButton m_MLElapsedHMS;
    CComboBox m_MLElapsedDbl;
    CButton m_MLCommand;
    CButton m_MLType;
    CButton m_MLTag;
    CButton m_MLSource;
    CButton m_MLIDNo;
    CButton m_MLIterNo;
    CButton m_MLCallNo;
    CButton m_MLSeqNo;
    CButton m_MLMessage;
    CButton m_MessageLogOn;
    CButton m_EventLogOn;

    afx_msg void OnBnClickedArcMLTime();
    afx_msg void OnBnClickedArcMLTimeDate();
    afx_msg void OnBnClickedArcMLTimeTime();
    afx_msg void OnBnClickedArcMLElapsedHMS();
    //afx_msg void OnBnClickedArcMLElapsedDbl();
    afx_msg void OnBnClickedArcMLCommand();
    afx_msg void OnBnClickedArcMLType();
    afx_msg void OnBnClickedArcMLTag();
    afx_msg void OnBnClickedArcMLSource();
    afx_msg void OnBnClickedArcMLIDNo();
    afx_msg void OnBnClickedArcMLIterNo();
    afx_msg void OnBnClickedArcMLCallNo();
    afx_msg void OnBnClickedArcMLSeqNo();
    afx_msg void OnBnClickedArcMLMessage();
  public:
    afx_msg void OnCbnSelchangeArcmlElapseddbl();
  public:
    afx_msg void OnBnClickedPrjeditarc();
  public:
    afx_msg void OnBnClickedArcmlMsglogon();
  public:
    afx_msg void OnBnClickedArcmlEvtlogon();
};

//===========================================================================

class CHistPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CHistPage)
  public:
    CHistPage();
    ~CHistPage();

    CProject         * m_pPrj;
    CProjectSettings * m_pSet;

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
    long m_FileSize;
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

IMPLEMENT_DYNAMIC(CProjectSettingsDlg, CPropertySheet)

//CProjectSettingsDlg::CProjectSettingsDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
//  : CPropertySheet(nIDCaption, pParentWnd, iSelectPage),
//  m_Sets(NULL)
//  {
//  ASSERT(FALSE); //DO NOT EXPECT TO GET HERE !!!
//  }

//---------------------------------------------------------------------------

CProjectSettingsDlg::CProjectSettingsDlg(bool AsModal,  CProject * Prj, CProjectSettings * Set, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
: CPropertySheet(pszCaption, pParentWnd, iSelectPage), 
  m_pPrj(Prj), m_pSet(Set)
  {
  m_pSet->m_bConfigBusy = 1;
  m_bAsModal = AsModal;

  m_pModesPage = new CModesPage;
  ((CModesPage*)m_pModesPage)->m_pSet=m_pSet;
  ((CModesPage*)m_pModesPage)->m_pPrj=m_pPrj;
  AddPage(m_pModesPage);

  m_pGrfTagsPage = new CGrfTagsPage;
  ((CGrfTagsPage*)m_pGrfTagsPage)->m_pSet=m_pSet;
  ((CGrfTagsPage*)m_pGrfTagsPage)->m_pPrj=m_pPrj;
  AddPage(m_pGrfTagsPage);

  m_pArchivePage = new CArchivePage;
  ((CArchivePage*)m_pArchivePage)->m_pSet=m_pSet;
  ((CArchivePage*)m_pArchivePage)->m_pPrj=m_pPrj;
  AddPage(m_pArchivePage);

  m_pHistoryPage = new CHistPage;
  ((CHistPage*)m_pHistoryPage)->m_pSet=m_pSet;
  ((CHistPage*)m_pHistoryPage)->m_pPrj=m_pPrj;
  AddPage(m_pHistoryPage);

  m_pCommsPage = new CCommsPage;
  ((CCommsPage*)m_pCommsPage)->m_pSet=m_pSet;
  ((CCommsPage*)m_pCommsPage)->m_pPrj=m_pPrj;
  AddPage(m_pCommsPage);

#if WITHNETSERVER
  pNetworkPage = new CNetPage;
  AddPage(pNetworkPage);
#endif
  if (!m_bAsModal)
    {
    if (!Create(pParentWnd, WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION, WS_EX_DLGMODALFRAME/* | WS_EX_SMCAPTION*/))
      {
      TRACE("Failed to create Project PropertySheet\n");
      }
    }

  if (m_pPrj)
    m_pPrj->GetSettings();
  }

//---------------------------------------------------------------------------

CProjectSettingsDlg::~CProjectSettingsDlg()
  {
  m_pSet->m_bConfigBusy = 0;
  MainWnd()->m_pPrjSheet = NULL; //set global pointer to property sheet to null
  delete m_pModesPage;
  delete m_pGrfTagsPage;
  delete m_pHistoryPage;
  delete m_pArchivePage;
  delete m_pCommsPage;
#if WITHNETSERVER
  delete pNetworkPage;
#endif
  //dialog just closed, ensure main SysCAD window becomes active ...
  MainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_UPDATE, 0);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CProjectSettingsDlg, CPropertySheet)
  //{{AFX_MSG_MAP(CProjectSettingsDlg)
  ON_WM_NCDESTROY()
  ON_WM_CLOSE()
  ON_COMMAND(IDOK,OnOK)
  ON_COMMAND(IDCANCEL,OnCancel)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CProjectSettingsDlg::OnInitDialog()
  {
  //ASSERT(m_bModeless); //this code only needed when creating modeless property sheet!

  PROPSHEETTRICK0()
    BOOL b = CPropertySheet::OnInitDialog();
  PROPSHEETTRICK1()

    GetDlgItem(ID_APPLY_NOW)->ShowWindow(SW_HIDE);
  GetDlgItem(IDHELP)->ShowWindow(SW_HIDE);
  return b;
  }

//---------------------------------------------------------------------------

void CProjectSettingsDlg::StoreCurPageNo()
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
void CProjectSettingsDlg::OnOK()
  {
  ASSERT_VALID(this);
  StoreCurPageNo();
  for (int i=0; i<GetPageCount(); i++)
    GetPage(i)->OnOK();
  if (GetActivePage()->OnKillActive())
    {
    //GetActivePage()->OnOK();
    MainWnd()->SetToolBarSolveMode();
    gs_AccessWnds.CloseAccessData(-1, False, False, True);
    gs_AccessWnds.CloseWnd(-1);
    }

  if (m_pPrj)
    m_pPrj->PutSettings();

  EndDialog(IDOK);
  }

//---------------------------------------------------------------------------
// overridden to ensure that a modeless property sheet behaves as a modal
void CProjectSettingsDlg::OnCancel()
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
void CProjectSettingsDlg::OnClose()
  {
  ASSERT_VALID(this);
  OnCancel();
  }

//---------------------------------------------------------------------------

void CProjectSettingsDlg::OnNcDestroy()
  {
  CPropertySheet::OnNcDestroy();
  if (!m_bAsModal)
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
  m_FileSize = 0;
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
  DDX_Text(pDX, IDC_PRJHSTSIZE, m_FileSize);
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
  m_HstName      = m_pSet->m_Hst.sHistorianName();
  m_ScenName     = m_pSet->m_Hst.sHstScenName();
  if (SymbolicPaths())
    s.FnContract(m_pSet->m_Hst.sHstDataDir());
  else
    s.FnExpand(m_pSet->m_Hst.sHstDataDir());
  m_DataDir      = s();
  if (SymbolicPaths())
    s.FnContract(m_pSet->m_Hst.sHstCatDir());
  else
    s.FnExpand(m_pSet->m_Hst.sHstCatDir());
  m_CatDir       = s();
  m_HstOn        = m_pSet->m_Hst.m_OnRqd;
  m_FilesMin     = m_pSet->m_Hst.iHstFilesMin;
  m_FilesMax     = Max((UINT)0, m_pSet->m_Hst.iHstFilesMax - m_pSet->m_Hst.iHstFilesMin);
  m_PermHist     = (m_FilesMax>0);
  m_ForceCnt     = m_pSet->m_Hst.lHstForceCnt;
  m_FileSize     = m_pSet->m_Hst.lHstEvFileSize;
  m_SlotNoEst    = m_pSet->m_Hst.lHstSlotNoEstimate;
  m_UseTol       = m_pSet->m_Hst.bHstUseTol;
  m_ATol         = m_pSet->m_Hst.dHstRecordATol;
  m_RTol         = m_pSet->m_Hst.dHstRecordRTol;
  //m_StopGap      = m_pSet->m_Hst.dHstStopGap;
  m_MidTimeFactor= m_pSet->m_Hst.dHstMidTimeFactor;
  }

//---------------------------------------------------------------------------

void CHistPage::SaveInfo()
  {
  m_pSet->m_Hst.sHistorianName = m_HstName.GetBuffer(0);
  m_pSet->m_Hst.m_OnRqd=m_HstOn;
  m_pSet->m_Hst.lHstEvFileSize = Max(64L, (((m_FileSize-1)/64L)+1)*64L);
  m_pSet->m_Hst.lHstSlotNoEstimate = Max(100L, m_SlotNoEst);
  m_pSet->m_Hst.sHstCatDir.FnContract((char*)(const char*)m_CatDir);
  m_pSet->m_Hst.sHstDataDir.FnContract((char*)(const char*)m_DataDir);
  m_pSet->m_Hst.sHstScenName = m_ScenName;
  m_pSet->m_Hst.iHstFilesMin = Max(m_FilesMin, (UINT)2);
  m_pSet->m_Hst.iHstFilesMax = m_FilesMin + m_FilesMax;
  m_pSet->m_Hst.lHstForceCnt = Max(m_ForceCnt, 100L);
  //if (!gs_License.AllowFullHist())
  //  gs_pPrj->iHstFilesMin = gs_pPrj->iHstFilesMax;
  if (m_pSet->m_Hst.iHstFilesMin >gs_License.MaxHistFilesAllowed() || 
    m_pSet->m_Hst.iHstFilesMax>gs_License.MaxHistFilesAllowed() || 
    m_pSet->m_Hst.lHstEvFileSize>gs_License.MaxHistSizeAllowed())
    {
    LogError("History", LF_Exclamation, "Historian file size reduced to that allowed by the license");
    m_pSet->m_Hst.iHstFilesMax = Min(m_pSet->m_Hst.iHstFilesMax, gs_License.MaxHistFilesAllowed());
    m_pSet->m_Hst.iHstFilesMin = Min(m_pSet->m_Hst.iHstFilesMin, m_pSet->m_Hst.iHstFilesMax);
    m_pSet->m_Hst.lHstEvFileSize = Min(m_pSet->m_Hst.lHstEvFileSize, gs_License.MaxHistSizeAllowed());
    }
  if (_stricmp(m_pSet->m_Hst.sHistorianName(), m_pSet->m_Hst.sHstScenName())==0)
    {
    LogError("History", LF_Exclamation, "Historian name and scenario name must NOT be the same");
    //m_pSet->m_Hst.sHstScenName += "_0";
    //m_ScenName = m_pSet->m_Hst.sHstScenName();
    }
  m_pSet->m_Hst.bHstUseTol = m_UseTol;
  m_pSet->m_Hst.dHstRecordATol = Min(fabs(m_ATol), 0.1);
  m_pSet->m_Hst.dHstRecordRTol = Min(fabs(m_RTol), 0.1);
  //m_pSet->m_Hst.dHstStopGap= Range(0.0, fabs(m_StopGap), 1000.0);
  m_pSet->m_Hst.dHstMidTimeFactor = Range(0.001, fabs(m_MidTimeFactor), 0.999);
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
  m_FileSize = Max(64L, (((m_FileSize-1)/64L)+1)*64L);
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

IMPLEMENT_DYNCREATE(CModesPage, CCustomPropertyPage)

CModesPage::CModesPage() : CCustomPropertyPage(CModesPage::IDD)
  {
  m_bTimeChanged = 0;
  //{{AFX_DATA_INIT(CModesPage)
  m_CurrentTimeHMS = _T("");
  m_TimeText = _T("Time");
  m_SyncWithClock = FALSE;
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CModesPage::~CModesPage()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CModesPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CModesPage)
  ON_EN_CHANGE(IDC_PRJCURRENTTIME, OnTimeChanged)
  ON_EN_KILLFOCUS(IDC_PRJCURRENTTIME, OnKillfocusPrjCurrenttime)
  //ON_BN_CLICKED(IDC_PRJPROBAL, OnPrjProbalOrDynamic)
  //ON_BN_CLICKED(IDC_PRJDYNAMICFLOW, OnPrjProbalOrDynamic)
  //ON_BN_CLICKED(IDC_PRJDYNAMICFULL, OnPrjProbalOrDynamic)
  //ON_BN_CLICKED(IDC_PRJNUMERICOK, OnNumTagsChanged)
  //ON_BN_CLICKED(IDC_PRJNUMERICSTARTOK, OnNumStTagsChanged)
  ON_BN_CLICKED(IDC_SYNCHRONISE, OnSynchronise)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDC_PRJCURRENTTIME, OnUpdateCurrentTime)
  ON_UPDATE_COMMAND_UI(IDC_SYNCHRONISE, OnUpdateDynamic)
  //ON_BN_CLICKED(IDC_LIBRARYBTN, OnBnClickedLibrarybtn)
  //ON_EN_CHANGE(IDC_LIBRARY, OnEnChangeLibrary)
  //ON_EN_CHANGE(IDC_GRPFILTER, OnEnChangeGrpfilter)
  ON_CBN_SELCHANGE(IDC_PRJNETMODECOMBO, &CModesPage::OnCbnSelchangePrjnetmodecombo)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CModesPage::OnInitDialog()
  {
  //Strng sModelCfg = PF.RdStr("General", "ModelLibrary", "");
  //sModelCfg = PF.RdStr("General", "ModelConfiguration", sOldModelCfg());
  //sModelCfg = PF.RdStr("General", "CfgFile", sOldModelCfg());
  //sModelCfg.FnExpand();

  //CProfINIFile Cfg(sModelCfg());

  //long MaxNodeMode;
  //long MaxLinkMode;
  //long MaxHeatMode;
  //long MaxFlowMode;


  CCustomPropertyPage::OnInitDialog();

  m_NetMode   = m_pSet->m_NetMode;

  m_PBNodeMode    = m_pSet->m_PBNodeMode;
  m_PBLinkMode    = m_pSet->m_PBLinkMode;
  m_PBHeatMode    = m_pSet->m_PBHeatMode;
  m_PBFlowMode    = m_pSet->m_PBFlowMode;
  m_PBMaxNodeMode = SM_Direct;
  m_PBMaxLinkMode = SM_Direct;
  m_PBMaxHeatMode = m_pSet->m_MaxHeatMode;
  m_PBMaxFlowMode = LFM_Xfer;

  m_DynNodeMode    = m_pSet->m_DynNodeMode;
  m_DynLinkMode    = m_pSet->m_DynLinkMode;
  m_DynHeatMode    = m_pSet->m_DynHeatMode;
  m_DynFlowMode    = m_pSet->m_DynFlowMode;
  m_DynMaxNodeMode = SM_Buffered;// m_pSet->m_MaxNodeMode;
  m_DynMaxLinkMode = m_pSet->m_MaxLinkMode;
  m_DynMaxHeatMode = m_pSet->m_MaxHeatMode;
  m_DynMaxFlowMode = m_pSet->m_MaxFlowMode;

  UpdateDialogControls(this, FALSE);

  LoadCBs(false);

  m_SyncWithClock = m_pSet->m_SyncWithClock!=0;

  m_CurrentTimeHMS = m_pSet->m_TheTime.Format(TD_TimeDate);
  m_CurrentTimeAtInit = m_pSet->m_TheTime;
  LPCTSTR Tm=m_pSet->m_TheTime.Format(TD_Time);

  SetDlgItemText(IDC_DATETIME_TXT, m_NetMode==0 ? "" : Tm);


  //m_NumTagsOK = !m_pSet->m_NumericTagsBad;
  //m_NumStartingOK = !m_pSet->m_NumericStartingTagsBad;
  //m_NumStartingOK = m_NumStartingOK || m_NumTagsOK;
  //m_NumChar = m_pSet->m_NonNumericTagChr;

#if WITHGRFDOCFRAME
  //m_sDefGrpLib=m_pSet->m_sDefGrpLib;
  //m_sGrfFrameName=m_pSet->m_sGrfFrameName;
  //m_sGrfFrameFilter=m_pSet->m_sGrfFrameFilter;
  //
  //m_sDefGrpLib.FnContract();
  //m_Filter.SetWindowText(m_sGrfFrameFilter());
  //m_Library.SetWindowText(m_sDefGrpLib()); // this will invoke the SetLibraryOptions
#else
  //m_Filter.EnableWindow(FALSE);
  //m_Library.EnableWindow(FALSE); // this will invoke the SetLibraryOptions
  //m_PageFrame.EnableWindow(FALSE); // this will invoke the SetLibraryOptions
#endif

  return TRUE;
  }

//---------------------------------------------------------------------------

void CModesPage::LoadCBs(bool DoSwap)
  {
  if (m_NetMode==NM_Probal)
    {
    if (DoSwap)
      {
      m_DynNodeMode     = m_NodeMode;
      m_DynLinkMode     = m_LinkMode;
      m_DynHeatMode     = m_HeatMode;
      m_DynFlowMode     = m_FlowMode;
      m_DynMaxNodeMode  = m_MaxNodeMode;
      m_DynMaxLinkMode  = m_MaxLinkMode;
      m_DynMaxHeatMode  = m_MaxHeatMode;
      m_DynMaxFlowMode  = m_MaxFlowMode;
      }
    m_NodeMode        = m_PBNodeMode;
    m_LinkMode        = m_PBLinkMode;
    m_HeatMode        = m_PBHeatMode;
    m_FlowMode        = m_PBFlowMode;
    m_MaxNodeMode     = m_PBMaxNodeMode;
    m_MaxLinkMode     = m_PBMaxLinkMode;
    m_MaxHeatMode     = m_PBMaxHeatMode;
    m_MaxFlowMode     = m_PBMaxFlowMode;
    }
  else
    {
    if (DoSwap)
      {
      m_PBNodeMode      = m_NodeMode;
      m_PBLinkMode      = m_LinkMode;
      m_PBHeatMode      = m_HeatMode;
      m_PBFlowMode      = m_FlowMode;
      m_PBMaxNodeMode   = m_MaxNodeMode;
      m_PBMaxLinkMode   = m_MaxLinkMode;
      m_PBMaxHeatMode   = m_MaxHeatMode;
      m_PBMaxFlowMode   = m_MaxFlowMode;
      }
    m_NodeMode        = m_DynNodeMode;
    m_LinkMode        = m_DynLinkMode;
    m_HeatMode        = m_DynHeatMode;
    m_FlowMode        = m_DynFlowMode;
    m_MaxNodeMode     = m_DynMaxNodeMode;
    m_MaxLinkMode     = m_DynMaxLinkMode;
    m_MaxHeatMode     = m_DynMaxHeatMode;
    m_MaxFlowMode     = m_DynMaxFlowMode;
    }

  GetGlblModeValueLst(m_NetModes,  GetPermissableModes(NM_All, NULL), 0, false);
  GetGlblModeValueLst(m_NodeModes, GetPermissableModes(MaxModes2Mask(m_MaxNodeMode), NULL), 0, false);
  GetGlblModeValueLst(m_LinkModes, GetPermissableModes(MaxModes2Mask(m_MaxLinkMode), NULL), 0, false);
  GetGlblModeValueLst(m_HeatModes, GetPermissableModes(MaxModes2Mask(m_MaxHeatMode), NULL), 0, false);
  GetGlblModeValueLst(m_FlowModes, GetPermissableModes(MaxModes2Mask(m_MaxFlowMode), NULL), 0, false);

  m_NetModeCB.ResetContent();
  m_NodeModeCB.ResetContent();
  m_LinkModeCB.ResetContent();
  m_HeatModeCB.ResetContent();
  m_FlowModeCB.ResetContent();

  int iCurSel=-1;
  for (int iMd=0; iMd<m_NetModes.Length(); iMd++)
    {
    m_NetModeCB.AddString(m_NetModes.Item(iMd)->m_pStr);
    if (m_NetModes.Item(iMd)->m_lVal==m_NetMode)
      iCurSel=iMd;
    }
  m_NetModeCB.SetCurSel(iCurSel>=0 ? iCurSel:iMd-1);
  m_NetModeCB.EnableWindow(m_pPrj==NULL);

  iCurSel = -1;
  for (int iMd=0; iMd<m_NodeModes.Length(); iMd++)
    {
    m_NodeModeCB.AddString(m_NodeModes.Item(iMd)->m_pStr);
    if (m_NodeModes.Item(iMd)->m_lVal==m_NodeMode)
      iCurSel=iMd;
    }
  m_NodeModeCB.SetCurSel(iCurSel>=0 ? iCurSel:iMd-1);

  iCurSel = -1;
  for (int iMd=0; iMd<m_LinkModes.Length(); iMd++)
    {
    m_LinkModeCB.AddString(m_LinkModes.Item(iMd)->m_pStr);
    if (m_LinkModes.Item(iMd)->m_lVal==m_LinkMode)
      iCurSel=iMd;
    }
  m_LinkModeCB.SetCurSel(iCurSel>=0 ? iCurSel:iMd-1);

  iCurSel = -1;
  for (int iMd=0; iMd<m_HeatModes.Length(); iMd++)
    {
    m_HeatModeCB.AddString(m_HeatModes.Item(iMd)->m_pStr);
    if (m_HeatModes.Item(iMd)->m_lVal==m_HeatMode)
      iCurSel=iMd;
    }
  m_HeatModeCB.SetCurSel(iCurSel>=0 ? iCurSel:iMd-1);

  iCurSel = -1;
  for (int iMd=0; iMd<m_FlowModes.Length(); iMd++)
    {
    m_FlowModeCB.AddString(m_FlowModes.Item(iMd)->m_pStr);
    if (m_FlowModes.Item(iMd)->m_lVal==m_FlowMode)
      iCurSel=iMd;
    }
  m_FlowModeCB.SetCurSel(iCurSel>=0 ? iCurSel:iMd-1);
  }

//---------------------------------------------------------------------------

void CModesPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CModesPage)
  DDX_Text(pDX, IDC_PRJCURRENTTIME, m_CurrentTimeHMS);
  DDX_Text(pDX, IDC_TIMETEXT, m_TimeText);
  DDX_Check(pDX, IDC_SYNCHRONISE, m_SyncWithClock);
  //}}AFX_DATA_MAP

  GetDlgItem(IDC_PRJCURRENTTIME)->EnableWindow(!m_SyncWithClock);
  DDX_Control(pDX, IDC_PRJNETMODECOMBO, m_NetModeCB);
  DDX_Control(pDX, IDC_PRJNODEMODECOMBO, m_NodeModeCB);
  DDX_Control(pDX, IDC_PRJLINKMODECOMBO, m_LinkModeCB);
  DDX_Control(pDX, IDC_PRJHEATMODECOMBO, m_HeatModeCB);
  DDX_Control(pDX, IDC_PRJFLOWMODECOMBO, m_FlowModeCB);
  }

//---------------------------------------------------------------------------

void CModesPage::OnOK()
  {
  if (bDidInit)
    {
    UpdateData(true);
    //GetGraphicsOptions();

    long RqdNetM=m_NetModeCB.GetCurSel()>=0 ? m_NetModes.Item(m_NetModeCB.GetCurSel())->m_lVal : -1;
    long RqdNodeM=m_NodeModeCB.GetCurSel()>=0 ? m_NodeModes.Item(m_NodeModeCB.GetCurSel())->m_lVal : -1;
    long RqdLinkM=m_LinkModeCB.GetCurSel()>=0 ? m_LinkModes.Item(m_LinkModeCB.GetCurSel())->m_lVal : -1;
    long RqdHeatM=m_HeatModeCB.GetCurSel()>=0 ? m_HeatModes.Item(m_HeatModeCB.GetCurSel())->m_lVal : -1;
    long RqdFlowM=m_FlowModeCB.GetCurSel()>=0 ? m_FlowModes.Item(m_FlowModeCB.GetCurSel())->m_lVal : -1;

    if (1)
      //RqdNetM!=m_NetMode || 
      //RqdNodeM!=m_NodeMode || 
      //RqdLinkM!=m_LinkMode || 
      //RqdHeatM!=m_HeatMode || 
      //RqdFlowM!=m_FlowMode)
      {

      m_pSet->m_NetMode=RqdNetM;
      if (RqdNetM==NM_Probal)
        {
        m_pSet->m_PBNodeMode=RqdNodeM;
        m_pSet->m_PBLinkMode=RqdLinkM;
        m_pSet->m_PBHeatMode=RqdHeatM;
        m_pSet->m_PBFlowMode=RqdFlowM;
        }
      else
        {
        m_pSet->m_DynNodeMode=RqdNodeM;
        m_pSet->m_DynLinkMode=RqdLinkM;
        m_pSet->m_DynHeatMode=RqdHeatM;
        m_pSet->m_DynFlowMode=RqdFlowM;
        }
      
      m_bTimeChanged=1;
      m_SyncWithClock=0;
      DoSyncChange();
      }

    //if (m_NumChar.GetLength()==0 || !TaggedObject::CheckNonNumericTagChr(m_NumChar[0]))
    //  {
    //  LogWarning("Project", 0, "Invalid non-numeric character (%s) specified, changed to '#'.", (const char*)m_NumChar);
    //  m_NumChar = "#";
    //  }
    //m_pSet->m_NonNumericTagChr = m_NumChar[0];

    if (m_bTimeChanged /*|| m_bTagNamingChanged*/)
      {
      if (m_pPrj)
        m_pPrj->bTimeChanged = m_bTimeChanged;
      CTimeValue RqdTime;
      flag OK1 = RqdTime.Parse(m_CurrentTimeHMS);
      if (RqdTime<m_CurrentTimeAtInit && RqdTime>=m_CurrentTimeAtInit-CTimeValue(0.002)) // rounding errors in (2 msecs)
        RqdTime=m_CurrentTimeAtInit;
      if (OK1)
        {
        flag ScrollTrendsToEnd = false;

        if (m_pPrj && gs_HstMngr.HstOpen()/*HstOn()*/ && m_bTimeChanged && (RqdTime<m_CurrentTimeAtInit))
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
          m_pSet->m_TheTime=RqdTime;
          m_pSet->m_SyncWithClock=m_SyncWithClock;
          if (m_SyncWithClock)
            {
            m_pSet->m_RealTime=true;
            m_pSet->m_RealTimeMult=1.0;
            }
          ScrollTrendsToEnd = true;
          }

        //        if (m_pPrj && m_bTagNamingChanged)
        //          {
        //          _asm int 3;
        //          int TagsChngd = gs_Exec.CheckAllTags(!m_NumTagsOK, !m_NumStartingOK);
        //          if (TagsChngd<0)
        //            {
        //            m_pSet->m_NumericTagsBad = 0;
        //            m_pSet->m_NumericStartingTagsBad = 0;
        //            }
        //          else
        //            {
        //            m_pSet->m_NumericTagsBad = !m_NumTagsOK;
        //            m_pSet->m_NumericStartingTagsBad = !m_NumStartingOK;
        //            if (TagsChngd>0)
        //              {
        //              CWaitCursor Wait;
        //              m_pPrj->CloseIOMarshal();
        //              m_pPrj->CloseArcManager();
        //#if WITHDRVMAN
        //              m_pPrj->CloseDrvManager();
        //              m_pPrj->OpenDrvManager();
        //#endif 
        //              m_pPrj->OpenArcManager(false);
        //              CTagVwDoc::RebuildAll();
        //              }
        //            }
        //          }
        if (ScrollTrendsToEnd)
          CTagVwDoc::AdjustTimebaseToEndAll();
        }
      else
        AfxMessageBox("Invalid CurrentTime");
      }

    }
  }

//---------------------------------------------------------------------------

void CModesPage::OnTimeChanged()
  {
  m_bTimeChanged = 1;
  }

//---------------------------------------------------------------------------

void CModesPage::OnKillfocusPrjCurrenttime()
  {
  UpdateData(true);
  CTimeValue t;
  if (t.Parse(m_CurrentTimeHMS))
    {
    if (t>=0.0)
      {
      LPCTSTR Tm=t.Format(TD_TimeDate);
      m_CurrentTimeHMS = Tm;
      Tm=t.Format(TD_Time);
      SetDlgItemText(IDC_DATETIME_TXT, Tm);
      UpdateData(false);
      }
    }
  }

void CModesPage::OnUpdateCurrentTime(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(!DefNetProbalMode() && !m_SyncWithClock);
  }

//---------------------------------------------------------------------------

void CModesPage::OnCbnSelchangePrjnetmodecombo()
  {
  UpdateData(true);

  m_NetMode = m_NetModeCB.GetCurSel()>=0 ? m_NetModes.Item(m_NetModeCB.GetCurSel())->m_lVal : 0;
  LoadCBs(true);

  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CModesPage::OnUpdateDynamic(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(DefNetDynamicMode());
  }

//---------------------------------------------------------------------------

void CModesPage::OnSynchronise()
  {
  UpdateData(true);
  DoSyncChange();
  UpdateData(false);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CModesPage::DoSyncChange()
  {
  m_bTimeChanged = 1;
  if (m_SyncWithClock)
    {
    __time64_t t;
    _time64(&t); //this Initialisation must occur
    CTimeValue tv((double)t);
    m_CurrentTimeHMS = tv.Format(TD_TimeDate);
    SetDlgItemText(IDC_DATETIME_TXT, m_NetMode==0 ? "" : tv.Format(TD_Time));
    }
  else
    {
    LPCTSTR Tm=m_CurrentTimeAtInit.Format(TD_TimeDate);
    m_CurrentTimeHMS = Tm;
    Tm=m_CurrentTimeAtInit.Format(TD_Time);
    SetDlgItemText(IDC_DATETIME_TXT, m_NetMode==0 ? "" : Tm);
    }
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CGrfTagsPage, CCustomPropertyPage)

CGrfTagsPage::CGrfTagsPage() : CCustomPropertyPage(CGrfTagsPage::IDD)
  {
  //m_bTimeChanged = 0;
  m_bTagNamingChanged = 0;
  //{{AFX_DATA_INIT(CGrfTagsPage)
#if WithNumericTags
  m_NumTagsOK = !FALSE;
#endif
  m_NumStartingOK = TRUE;
  m_NumChar = _T("");
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CGrfTagsPage::~CGrfTagsPage()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CGrfTagsPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CGrfTagsPage)
  ON_BN_CLICKED(IDC_PRJNUMERICOK, OnNumTagsChanged)
  ON_BN_CLICKED(IDC_PRJNUMERICSTARTOK, OnNumStTagsChanged)
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_LIBRARYBTN, OnBnClickedLibrarybtn)
  ON_EN_CHANGE(IDC_LIBRARY, OnEnChangeLibrary)
  ON_EN_CHANGE(IDC_GRPFILTER, OnEnChangeGrpfilter)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CGrfTagsPage::OnInitDialog()
  {
  CCustomPropertyPage::OnInitDialog();

#if WithNumericTags
  m_NumTagsOK = (m_pSet->m_NumericTagsBad!=0 ? 0 : 1);
#endif
  m_NumStartingOK = (m_pSet->m_NumericStartingTagsBad!=0 ? 0 : 1);
#if WithNumericTags
  m_NumStartingOK = ((m_NumStartingOK || m_NumTagsOK) ? 1 : 0);
#else
  GetDlgItem(IDC_PRJNUMERICOK)->EnableWindow(false);
#endif
  m_NumChar = m_pSet->m_NonNumericTagChr;

#if WITHGRFDOCFRAME
  m_sDefGrpLib=m_pSet->m_sDefGrpLib;
  m_sGrfFrameName=m_pSet->m_sGrfFrameName;
  m_sGrfFrameFilter=m_pSet->m_sGrfFrameFilter;

  m_sDefGrpLib.FnContract();
  m_Filter.SetWindowText(m_sGrfFrameFilter());
  m_Library.SetWindowText(m_sDefGrpLib()); // this will invoke the SetLibraryOptions
#else
  m_Filter.EnableWindow(FALSE);
  m_Library.EnableWindow(FALSE); // this will invoke the SetLibraryOptions
  m_PageFrame.EnableWindow(FALSE); // this will invoke the SetLibraryOptions
#endif

  UpdateData(false);
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CGrfTagsPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CGrfTagsPage)
#if WithNumericTags
  DDX_Check(pDX, IDC_PRJNUMERICOK, m_NumTagsOK);
#endif
  DDX_Check(pDX, IDC_PRJNUMERICSTARTOK, m_NumStartingOK);
  DDX_Text(pDX, IDC_PRJNUMERICCHAR, m_NumChar);
  //}}AFX_DATA_MAP

  DDX_Control(pDX, IDC_LIBRARY, m_Library);
  DDX_Control(pDX, IDC_PAGEFRAME, m_PageFrame);
  DDX_Control(pDX, IDC_GRPFILTER, m_Filter);
  }

//---------------------------------------------------------------------------

void CGrfTagsPage::SetLibraryOptions()
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
        if (_strnicmp(I.m_sGroup, m_sGrfFrameFilter(), m_sGrfFrameFilter.GetLength())==0)
          m_PageFrame.AddString(I.m_sGroup);
        }
      }
    }

  int i=m_PageFrame.SelectString(0, m_sGrfFrameName());
  if (i<0)
    m_PageFrame.SetCurSel(2);
  }

//---------------------------------------------------------------------------

void CGrfTagsPage::OnOK()
  {
  if (bDidInit)
    {
    UpdateData(true);

    if (m_NumChar.GetLength()==0 || !TaggedObject::CheckNonNumericTagChr(m_NumChar[0]))
      {
      LogWarning("Project", 0, "Invalid non-numeric character (%s) specified, changed to '#'.", (const char*)m_NumChar);
      m_NumChar = "#";
      }
    m_pSet->m_NonNumericTagChr = m_NumChar[0];

    if (/*m_bTimeChanged ||*/ m_bTagNamingChanged)
      {
      //if (m_pPrj)
      //  m_pPrj->bTimeChanged = m_bTimeChanged;
      //CTimeValue RqdTime;
      //flag OK1 = RqdTime.Parse(m_CurrentTimeHMS);
      //if (RqdTime<m_CurrentTimeAtInit && RqdTime>=m_CurrentTimeAtInit-CTimeValue(0.002)) // rounding errors in (2 msecs)
      //  RqdTime=m_CurrentTimeAtInit;
      //if (1)//OK1)
      //  {
      //        flag ScrollTrendsToEnd = false;
      //       
      //        if (m_pPrj && gs_HstMngr.HstOn() && m_bTimeChanged && (RqdTime<m_CurrentTimeAtInit))
      //          {
      //          switch (AfxMessageBox("Moving time Backwards!\n\nHistorian will be cleared", MB_ICONQUESTION|MB_OKCANCEL))
      //            {
      //            case IDOK:
      ////gs_pPrj->CloseHistorian();
      ////gs_pPrj->RestartHistorian();
      ////gs_pPrj->OpenHistorian();
      //              ScrollTrendsToEnd = true;
      //              break;
      //            case IDCANCEL:
      //              m_bTimeChanged=false;
      //              break;
      //            }
      //          }
      //
      //        if (m_bTimeChanged)
      //          {
      //          m_pSet->m_TheTime=RqdTime;
      //          m_pSet->m_SyncWithClock=m_SyncWithClock;
      //          if (m_SyncWithClock)
      //            {
      //            m_pSet->m_RealTime=true;
      //            m_pSet->m_RealTimeMult=1.0;
      //            }
      //          ScrollTrendsToEnd = true;
      //          }

      if (m_pPrj && m_bTagNamingChanged)
        {
        //_asm int 3;
#if WithNumericTags
        int TagsChngd = gs_Exec.CheckAllTags(!m_NumTagsOK, !m_NumStartingOK);
#else
        int TagsChngd = gs_Exec.CheckAllTags(!m_NumStartingOK);
#endif
        if (TagsChngd<0)
          {
#if WithNumericTags
          m_pSet->m_NumericTagsBad = 0;
#endif
          m_pSet->m_NumericStartingTagsBad = 0;
          }
        else
          {
#if WithNumericTags
          m_pSet->m_NumericTagsBad = !m_NumTagsOK;
#endif
          m_pSet->m_NumericStartingTagsBad = (m_NumStartingOK!=0 ? 0 : 1);
          if (TagsChngd>0)
            {
            CWaitCursor Wait;
            m_pPrj->CloseIOMarshal();
            m_pPrj->CloseArcManager();
#if WITHDRVMAN
            m_pPrj->CloseDrvManager();
            m_pPrj->OpenDrvManager();
#endif 
            m_pPrj->OpenArcManager(false);
            CTagVwDoc::RebuildAll();
            }
          }
        }
      //if (ScrollTrendsToEnd)
      //  CTagVwDoc::AdjustTimebaseToEndAll();
      //  }
      //else
      //  AfxMessageBox("Invalid CurrentTime");
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

void CGrfTagsPage::OnNumTagsChanged()
  {
  UpdateData(true);
  m_bTagNamingChanged = 1;
#if WithNumericTags
  m_NumStartingOK = ((m_NumStartingOK || m_NumTagsOK) ? 1 : 0);
#endif
  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CGrfTagsPage::OnNumStTagsChanged()
  {
  UpdateData(true);
  m_bTagNamingChanged = 1;
#if WithNumericTags
  m_NumTagsOK = ((m_NumTagsOK && m_NumStartingOK) ? 1 : 0);
#endif
  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CGrfTagsPage::OnBnClickedLibrarybtn()
  {
  INCOMPLETECODE(__FILE__, __LINE__)
    //GetGraphicsOptions();
    //SetGraphicsOptions();
  }

//---------------------------------------------------------------------------

void CGrfTagsPage::OnEnChangeLibrary()
  {
  SetLibraryOptions();
  }

//---------------------------------------------------------------------------

void CGrfTagsPage::OnEnChangeGrpfilter()
  {
  CString S;
  m_Filter.GetWindowText(S);
  m_sGrfFrameFilter=S;
  SetLibraryOptions();
  }

//---------------------------------------------------------------------------
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
#if WITHDRVMAN
  m_DrvOn = FALSE;
  m_DrvLclTagSrvrOK= TRUE;
  m_DrvName = _T("");
  m_DrvReadAll = FALSE;
#endif
  m_OPCOn = FALSE;
  m_OPCServerNo = FALSE;
  m_OPCResetReg = FALSE;
  m_DDEOn = FALSE;
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
#if WITHDRVMAN
  DDX_Check(pDX, IDC_PRJDRVON, m_DrvOn);
  DDX_Check(pDX, IDC_PRJDRVLOCALTAGSRVR, m_DrvLclTagSrvrOK);
  DDX_Text(pDX, IDD_PRJDRVNAME, m_DrvName);
  DDX_Check(pDX, IDC_PRJDRVREADALL, m_DrvReadAll);
#endif
  DDX_Check(pDX, IDC_PRJOPCON, m_OPCOn);
  DDX_Check(pDX, IDC_PRJOPCSECONDSERVER, m_OPCServerNo);
  DDX_Check(pDX, IDC_PRJOPCRESETREG, m_OPCResetReg);
  DDX_Check(pDX, IDC_PRJDDEON, m_DDEOn);
  DDX_Check(pDX, IDC_PRJIOMON, m_IOMOn);
  DDX_Text(pDX, IDD_PRJIOMNAME, m_IOMName);
  DDX_Text(pDX, IDD_PRJIOMNODE, m_IOMNode);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCommsPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CCommsPage)
#if WITHDRVMAN
  ON_BN_CLICKED(IDC_PRJDRVON, OnDriverOn)
  ON_EN_CHANGE(IDD_PRJDRVNAME, OnDriverChanged)
  ON_BN_CLICKED(IDC_PRJRELOADDRV, OnDriverReload)
  ON_BN_CLICKED(IDC_DRVBROWSE, OnDriverBrowse)
#endif
  ON_BN_CLICKED(IDC_PRJOPCRELOAD, OnOPCReload)
  ON_BN_CLICKED(IDC_PRJOPCON, OnOpcChanged)
  ON_BN_CLICKED(IDC_PRJDDEON, OnDdeChanged)
  //  ON_BN_CLICKED(IDC_ARCBROWSE, OnArchiveBrowse)
  //  ON_BN_CLICKED(IDC_PRJARCON, OnArchiveOn)
  //  ON_BN_CLICKED(IDC_PRJRELOADARC, OnArchiveReload)
  //  ON_EN_CHANGE(IDD_PRJARCNAME, OnArchiveChanged)
  //  ON_BN_CLICKED(IDC_PRJARCOPENONRUN, OnPrjarcopenonrun)
  ON_BN_CLICKED(IDC_IOMBROWSE, OnIOMBrowse)
  ON_BN_CLICKED(IDC_PRJIOMON, OnPrjIOMOn)
  ON_EN_CHANGE(IDD_PRJIOMNAME, OnIOMChanged)
  ON_BN_CLICKED(IDC_COMMSAPPLY, OnCommsApply)
#if WITHDRVMAN
  ON_BN_CLICKED(IDC_PRJDRVLOCALTAGSRVR, OnDriverOn)
  ON_BN_CLICKED(IDC_PRJDRVREADALL, OnDriverChanged)
#endif
  ON_BN_CLICKED(IDC_PRJOPCSECONDSERVER, OnOpcChanged)
  ON_BN_CLICKED(IDC_PRJOPCRESETREG, OnOpcChanged)
  ON_EN_CHANGE(IDD_PRJIOMNODE, OnIOMChanged)
  //}}AFX_MSG_MAP
#if WITHDRVMAN
  ON_UPDATE_COMMAND_UI(IDC_PRJRELOADDRV, OnUpdateReLoadDrv)
  ON_UPDATE_COMMAND_UI(IDC_PRJDRVREADALL, OnUpdateDriverOn)
  ON_UPDATE_COMMAND_UI(IDD_PRJDRVNAME, OnUpdateDriverOn)
  ON_UPDATE_COMMAND_UI(IDC_DRVBROWSE, OnUpdateDriverOn)
#endif
  //ON_UPDATE_COMMAND_UI(IDC_PRJRELOADARC, OnUpdateReLoadArc)
  //ON_UPDATE_COMMAND_UI(IDD_PRJARCNAME, OnUpdateArchiveOn)
  //ON_UPDATE_COMMAND_UI(IDC_ARCBROWSE, OnUpdateArchiveOn)
  ON_UPDATE_COMMAND_UI(IDD_PRJIOMNAME, OnUpdateIOMOn)
  ON_UPDATE_COMMAND_UI(IDC_IOMBROWSE, OnUpdateIOMOn)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CCommsPage::OnInitDialog()
  {
  Strng Fn;

#if WITHDRVMAN
  m_DrvName    = gs_pPrj->m_sDrvManagerName();
  m_DrvOn      = gs_pPrj->m_bDrvOn;
  m_DrvLclTagSrvrOK= gs_pPrj->bDrvLclTagSrvrOK;
  m_DrvReadAll = gs_pPrj->bDrvReadAll;
  if (SymbolicPaths())
    Fn.FnContract(gs_pPrj->sDrvManagerName());
  else
    Fn.FnExpand(gs_pPrj->sDrvManagerName());
  m_DrvName=Fn();
#endif

  m_OPCOn           = m_pSet->m_bOPCOn;
  m_OPCServerNo     = (m_pSet->m_iOPCServerNo!=0);
  m_OPCResetReg     = m_pSet->m_bOPCResetReg;

  m_DDEOn           = m_pSet->m_bDDEOn;

  m_IOMName         = m_pSet->m_sIOMarshalName();
  m_IOMNode         = m_pSet->m_sIOMarshalNode();
  m_IOMOn           = m_pSet->m_bIOMOn;
  if (SymbolicPaths())
    Fn.FnContract(m_pSet->m_sIOMarshalName());
  else
    Fn.FnExpand(m_pSet->m_sIOMarshalName());
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

    Strng Fn;
#if WITHDRVMAN
    Fn=m_DrvName;
    Fn.FnContract();
    m_DrvName=Fn();
#endif

    //Fn=m_ArcName;
    //Fn.FnContract();
    //m_ArcName=Fn();

    Fn=m_IOMName;
    Fn.FnContract();
    m_IOMName=Fn();

#if WITHDRVMAN
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
#else
    if (bIOMChanged || bOPCChanged || bDDEChanged)
      {
#endif

      if (bIOMChanged)
        {
        if (m_pPrj)
          {
          m_pPrj->bIOMChanged = true;
          m_pPrj->CloseIOMarshal();
          }
        m_pSet->m_sIOMarshalName.FnContract((LPSTR)(LPCSTR)m_IOMName);
        m_pSet->m_sIOMarshalNode=(LPSTR)(LPCSTR)m_IOMNode;
        m_pSet->m_bIOMOn          = m_IOMOn;
        //m_pSet->bArcOpenDBOnRun = m_ArcOpenDBOnRun;
        if (m_pPrj)
          {
          m_pPrj->OpenIOMarshal();
          CTagVwDoc::RebuildAll();
          }
        }

      if (bOPCChanged)
        {
        if (m_pPrj)
          DisableMainOPCSrvr();
        //m_pSet->CloseOPCManager();
        m_pSet->m_bOPCOn = m_OPCOn;
        m_pSet->m_iOPCServerNo = m_OPCServerNo;
        m_pSet->m_bOPCResetReg = m_OPCResetReg;
        //m_pSet->OpenOPCManager();
        if (m_pPrj)
          EnableMainOPCSrvr(m_pSet->m_bOPCOn!=0);
        }

      if (bDDEChanged)
        {
        if (m_pPrj)
          m_pPrj->CloseDDEManager();
        m_pSet->m_bDDEOn = m_DDEOn;
        if (m_pPrj)
          m_pPrj->OpenDDEManager();
        }
      }
    }
  }

//---------------------------------------------------------------------------

#if WITHDRVMAN
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
#endif

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
      gs_pPrj->m_sIOMarshalName = m_IOMName;
      gs_pPrj->m_sIOMarshalNode = m_IOMNode;
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

void CCommsPage::OnCommsApply()
  {
  if (bDrvChanged || /*bArcChanged ||*/ bIOMChanged || bOPCChanged || bDDEChanged)
    OnOK();
  bDrvChanged=0;
  //bArcChanged=0;
  bIOMChanged=0;
  bOPCChanged=0;
  bDDEChanged=0;
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CArchivePage, CCustomPropertyPage)

CArchivePage::CArchivePage() : CCustomPropertyPage(CArchivePage::IDD)
  {
  bArcChanged = 0;
  m_ArcName = _T("");
  m_ArcOn = false;
  m_hCfgProcess=NULL;        //process handle for the editor
  m_dwCfgProcessId=0;        //process ID for the editor

  }

//---------------------------------------------------------------------------

CArchivePage::~CArchivePage()
  {
  }

//---------------------------------------------------------------------------

void CArchivePage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CArchivePage)
  DDX_Check(pDX, IDC_PRJARCON, m_ArcOn);
  DDX_Text(pDX, IDD_PRJARCNAME, m_ArcName);
  DDX_Control(pDX, IDC_ARCML_TIME, m_MLTime);
  DDX_Control(pDX, IDC_ARCML_TIMEDATE, m_MLTimeDate);
  DDX_Control(pDX, IDC_ARCML_TIMETIME, m_MLTimeTime);
  DDX_Control(pDX, IDC_ARCML_ELAPSEDHMS, m_MLElapsedHMS);
  DDX_Control(pDX, IDC_ARCML_ELAPSEDDBL, m_MLElapsedDbl);
  DDX_Control(pDX, IDC_ARCML_COMMAND, m_MLCommand);
  DDX_Control(pDX, IDC_ARCML_TYPE, m_MLType);
  DDX_Control(pDX, IDC_ARCML_TAG, m_MLTag);
  DDX_Control(pDX, IDC_ARCML_SOURCE, m_MLSource);
  DDX_Control(pDX, IDC_ARCML_IDNO, m_MLIDNo);
  DDX_Control(pDX, IDC_ARCML_ITERNO, m_MLIterNo);
  DDX_Control(pDX, IDC_ARCML_CALLNO, m_MLCallNo);
  DDX_Control(pDX, IDC_ARCML_SEQNO, m_MLSeqNo);
  DDX_Control(pDX, IDC_ARCML_MESSAGE, m_MLMessage);
  DDX_Control(pDX, IDC_ARCML_MSGLOGON, m_MessageLogOn);
  DDX_Control(pDX, IDC_ARCML_EVTLOGON, m_EventLogOn);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CArchivePage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CArchivePage)
  ON_BN_CLICKED(IDC_ARCBROWSE, OnArchiveBrowse)
  ON_BN_CLICKED(IDC_PRJARCON, OnArchiveOn)
  ON_BN_CLICKED(IDC_PRJRELOADARC, OnArchiveReload)
  ON_EN_CHANGE(IDD_PRJARCNAME, OnArchiveChanged)
  ON_BN_CLICKED(IDC_PRJARCOPENONRUN, OnPrjarcopenonrun)
  ON_BN_CLICKED(IDC_ARCHIVEAPPLY, OnArchiveApply)
  ON_UPDATE_COMMAND_UI(IDC_PRJRELOADARC, OnUpdateReLoadArc)
  ON_UPDATE_COMMAND_UI(IDD_PRJARCNAME, OnUpdateArchiveOn)
  ON_UPDATE_COMMAND_UI(IDC_ARCBROWSE, OnUpdateArchiveOn)

  ON_BN_CLICKED(IDC_ARCML_TIME, &CArchivePage::OnBnClickedArcMLTime)
  ON_BN_CLICKED(IDC_ARCML_TIMEDATE, &CArchivePage::OnBnClickedArcMLTimeDate)
  ON_BN_CLICKED(IDC_ARCML_TIMETIME, &CArchivePage::OnBnClickedArcMLTimeTime)
  ON_BN_CLICKED(IDC_ARCML_ELAPSEDHMS, &CArchivePage::OnBnClickedArcMLElapsedHMS)
  //ON_BN_CLICKED(IDC_ARCML_ELAPSEDDBL, &CArchivePage::OnBnClickedArcMLElapsedDbl)
  ON_BN_CLICKED(IDC_ARCML_COMMAND, &CArchivePage::OnBnClickedArcMLCommand)
  ON_BN_CLICKED(IDC_ARCML_TYPE, &CArchivePage::OnBnClickedArcMLType)
  ON_BN_CLICKED(IDC_ARCML_TAG, &CArchivePage::OnBnClickedArcMLTag)
  ON_BN_CLICKED(IDC_ARCML_SOURCE, &CArchivePage::OnBnClickedArcMLSource)
  ON_BN_CLICKED(IDC_ARCML_IDNO, &CArchivePage::OnBnClickedArcMLIDNo)
  ON_BN_CLICKED(IDC_ARCML_ITERNO, &CArchivePage::OnBnClickedArcMLIterNo)
  ON_BN_CLICKED(IDC_ARCML_CALLNO, &CArchivePage::OnBnClickedArcMLCallNo)
  ON_BN_CLICKED(IDC_ARCML_SEQNO, &CArchivePage::OnBnClickedArcMLSeqNo)
  ON_BN_CLICKED(IDC_ARCML_MESSAGE, &CArchivePage::OnBnClickedArcMLMessage)

  ON_CBN_SELCHANGE(IDC_ARCML_ELAPSEDDBL, &CArchivePage::OnCbnSelchangeArcmlElapseddbl)
  ON_BN_CLICKED(IDC_PRJEDITARC, &CArchivePage::OnBnClickedPrjeditarc)
  ON_BN_CLICKED(IDC_ARCML_MSGLOGON, &CArchivePage::OnBnClickedArcmlMsglogon)
  ON_BN_CLICKED(IDC_ARCML_EVTLOGON, &CArchivePage::OnBnClickedArcmlEvtlogon)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CArchivePage::OnInitDialog()
  {
  Strng Fn;


  m_ArcName        = m_pSet->m_sArcManagerName();
  m_ArcOn          = m_pSet->m_bArcOn;
  if (SymbolicPaths())
    Fn.FnContract(m_pSet->m_sArcManagerName());
  else
    Fn.FnExpand(m_pSet->m_sArcManagerName());
  m_ArcName=Fn();
  
  CCustomPropertyPage::OnInitDialog();

  m_MLElapsedDbl.AddString("Off");
  m_MLElapsedDbl.AddString("Days");
  m_MLElapsedDbl.AddString("Hours");
  m_MLElapsedDbl.AddString("Minutes");
  m_MLElapsedDbl.AddString("Seconds");

  GetOptions(Fn());

  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CArchivePage::GetOptions(LPCTSTR FileName)
  {
  DWORD Opts = CArchiveManager::ReadOptions(FileName);

  m_MLTime       .SetCheck((Opts & LogItem_Time      )  !=0 ? 1:0);
  m_MLTimeDate   .SetCheck((Opts & LogItem_TimeDate  )  !=0 ? 1:0);
  m_MLTimeTime   .SetCheck((Opts & LogItem_TimeTime  )  !=0 ? 1:0);

  m_MLElapsedHMS .SetCheck((Opts & LogItem_ElapsedHMS)  !=0 ? 1:0);
  //m_MLElapsedDbl .SetCheck((Opts & LogItem_ElapsedDbl)  !=0 ? 1:0);
  if (Opts & LogItem_ElapsedDays)
    m_MLElapsedDbl.SetCurSel(1);
  else if (Opts & LogItem_ElapsedHrs)
    m_MLElapsedDbl.SetCurSel(2);
  else if (Opts & LogItem_ElapsedMins)
    m_MLElapsedDbl.SetCurSel(3);
  else if (Opts & LogItem_ElapsedSecs)
    m_MLElapsedDbl.SetCurSel(4);
  else
    m_MLElapsedDbl.SetCurSel(0);
  m_MLCommand    .SetCheck((Opts & LogItem_Command       )  !=0 ? 1:0);
  m_MLType       .SetCheck((Opts & LogItem_Type          )  !=0 ? 1:0);
  m_MLTag        .SetCheck((Opts & LogItem_Tag           )  !=0 ? 1:0);
  m_MLSource     .SetCheck((Opts & LogItem_Source        )  !=0 ? 1:0);
  m_MLIDNo       .SetCheck((Opts & LogItem_IDNo          )  !=0 ? 1:0);
  m_MLIterNo     .SetCheck((Opts & LogItem_IterNo        )  !=0 ? 1:0);
  m_MLCallNo     .SetCheck((Opts & LogItem_CallNo        )  !=0 ? 1:0);
  m_MLSeqNo      .SetCheck((Opts & LogItem_SeqNo         )  !=0 ? 1:0);
  m_MLMessage    .SetCheck((Opts & LogItem_Message       )  !=0 ? 1:0);
  m_MessageLogOn .SetCheck((Opts & LogItem_MessageLogOn  )  !=0 ? 1:0);
  m_EventLogOn   .SetCheck((Opts & LogItem_EventLogOn    )  !=0 ? 1:0);
  };

//---------------------------------------------------------------------------

void CArchivePage::SetOptions(LPCTSTR FileName)
  {
  DWORD Opts=0;

  if (m_MLTime       .GetCheck()!=0) Opts |= LogItem_Time      ;
  if (m_MLTimeDate   .GetCheck()!=0) Opts |= LogItem_TimeDate  ;
  if (m_MLTimeTime   .GetCheck()!=0) Opts |= LogItem_TimeTime  ;

  if (m_MLElapsedHMS .GetCheck()!=0) Opts |= LogItem_ElapsedHMS;

  if (m_MLElapsedDbl.GetCurSel()==1) Opts |= LogItem_ElapsedDays;
  if (m_MLElapsedDbl.GetCurSel()==2) Opts |= LogItem_ElapsedHrs;
  if (m_MLElapsedDbl.GetCurSel()==3) Opts |= LogItem_ElapsedMins;
  if (m_MLElapsedDbl.GetCurSel()==4) Opts |= LogItem_ElapsedSecs;

  if (m_MLCommand    .GetCheck()!=0) Opts |= LogItem_Command   ;
  if (m_MLType       .GetCheck()!=0) Opts |= LogItem_Type      ;
  if (m_MLTag        .GetCheck()!=0) Opts |= LogItem_Tag       ;
  if (m_MLSource     .GetCheck()!=0) Opts |= LogItem_Source    ;
  if (m_MLIDNo       .GetCheck()!=0) Opts |= LogItem_IDNo      ;
  if (m_MLIterNo     .GetCheck()!=0) Opts |= LogItem_IterNo    ;
  if (m_MLCallNo     .GetCheck()!=0) Opts |= LogItem_CallNo    ;
  if (m_MLSeqNo      .GetCheck()!=0) Opts |= LogItem_SeqNo     ;
  if (m_MLMessage    .GetCheck()!=0) Opts |= LogItem_Message   ;

  if (m_MessageLogOn .GetCheck()!=0) Opts |= LogItem_MessageLogOn;
  if (m_EventLogOn   .GetCheck()!=0) Opts |= LogItem_EventLogOn  ;

  CArchiveManager::WriteOptions(FileName, Opts);
  }

//---------------------------------------------------------------------------

void CArchivePage::OnOK()
  {
  if (bDidInit)
    {
    UpdateData(true);

    Strng Fn;

    Fn=m_ArcName;
    Fn.FnContract();
    m_ArcName=Fn();

    if (bArcChanged)
      {
      if (m_pPrj)
        {
        m_pPrj->bArcChanged = true;
        m_pPrj->CloseArcManager();
        }
      m_pSet->m_sArcManagerName.FnContract((char*)(const char*)m_ArcName);
      m_pSet->m_bArcOn=m_ArcOn;

      SetOptions(m_pSet->m_sArcManagerName());
      if (m_pPrj)
        {
        m_pPrj->OpenArcManager(false);
        m_pPrj->ConnectArcManager();
        CTagVwDoc::RebuildAll();
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CArchivePage::OnArchiveBrowse()
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
  FPath.FnExpand();
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
      gs_pPrj->m_sArcManagerName = m_ArcName;
      bArcChanged = 0;
      UpdateDialogControls(this, FALSE);
      }
    }
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CArchivePage::OnArchiveOn()
  {
  UpdateData(true);
  bArcChanged = 1;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CArchivePage::OnBnClickedPrjeditarc()
  {
  DWORD ExitCode = 0;
  if (m_hCfgProcess)
    GetExitCodeProcess(m_hCfgProcess, &ExitCode);
  if (ExitCode==STILL_ACTIVE)
    {
    ActivateApp(m_dwCfgProcessId);
    }
  else
    {
    Strng Fn;
    Fn.FnExpand(gs_pPrj->m_sArcManagerName());
    m_hCfgProcess = NULL;
    Strng NPad=TxtEditCmdString(Fn());
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.wShowWindow = SW_SHOWDEFAULT;
    PROCESS_INFORMATION pi;
    if (CreateProcess(NULL, NPad(), NULL, NULL, FALSE, 0, NULL, PrjFiles(), &si, &pi))
      {
      m_hCfgProcess = pi.hProcess;
      m_dwCfgProcessId = pi.dwProcessId;
      }
    }
  }

//---------------------------------------------------------------------------

void CArchivePage::OnArchiveReload()
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
    gs_pPrj->DisConnectArcManager();
    gs_pPrj->CloseArcManager();
    gs_pPrj->m_sArcManagerName.FnContract((char*)(const char*)m_ArcName);
    gs_pPrj->m_bArcOn=m_ArcOn;

    SetOptions(m_pSet->m_sArcManagerName());

    gs_pPrj->OpenArcManager(false);
    gs_pPrj->ConnectArcManager();
    CTagVwDoc::RebuildAll();
    bArcChanged = 0;
    UpdateDialogControls(this, FALSE);
    }
  }

void CArchivePage::OnUpdateReLoadArc(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(true);//m_ArcOn);
  }

//---------------------------------------------------------------------------

void CArchivePage::OnArchiveChanged()
  {
  bArcChanged=1;
  }

//---------------------------------------------------------------------------

void CArchivePage::OnPrjarcopenonrun()
  {
  UpdateData(true);
  bArcChanged = 1;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CArchivePage::OnUpdateArchiveOn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(true);//m_ArcOn);
  }

//---------------------------------------------------------------------------

void CArchivePage::OnArchiveApply()
  {
  if (bArcChanged)
    OnOK();
  bArcChanged=0;
  }

void CArchivePage::OnBnClickedArcMLTime()         { bArcChanged=1; }
void CArchivePage::OnBnClickedArcMLTimeDate()     { bArcChanged=1; }
void CArchivePage::OnBnClickedArcMLTimeTime()     { bArcChanged=1; }
void CArchivePage::OnBnClickedArcMLElapsedHMS()   { bArcChanged=1; }
void CArchivePage::OnCbnSelchangeArcmlElapseddbl(){ bArcChanged=1; }
void CArchivePage::OnBnClickedArcMLCommand()      { bArcChanged=1; }
void CArchivePage::OnBnClickedArcMLType()         { bArcChanged=1; }
void CArchivePage::OnBnClickedArcMLTag()          { bArcChanged=1; }
void CArchivePage::OnBnClickedArcMLSource()       { bArcChanged=1; }
void CArchivePage::OnBnClickedArcMLIDNo()         { bArcChanged=1; }
void CArchivePage::OnBnClickedArcMLIterNo()       { bArcChanged=1; }
void CArchivePage::OnBnClickedArcMLCallNo()       { bArcChanged=1; }
void CArchivePage::OnBnClickedArcMLSeqNo()        { bArcChanged=1; }
void CArchivePage::OnBnClickedArcMLMessage()      { bArcChanged=1; }
void CArchivePage::OnBnClickedArcmlMsglogon()     { bArcChanged=1; }
void CArchivePage::OnBnClickedArcmlEvtlogon()     { bArcChanged=1; }

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
#if WithNumericTags
  m_MasterNumTagsOK = !FALSE;
  m_SlaveNumTagsOK = !FALSE;
#endif
  m_MasterStNumTagsOK = !FALSE;
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
#if WithNumericTags
  DDX_Check(pDX, IDC_MSTRNUMTAGSOK, m_MasterNumTagsOK);
  DDX_Check(pDX, IDC_SLVNUMTAGSOK, m_SlaveNumTagsOK);
#endif
  DDX_Check(pDX, IDC_MSTRSTNUMTAGSOK, m_MasterStNumTagsOK);
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
#if WithNumericTags
#else
  GetDlgItem(IDC_MSTRNUMTAGSOK)->EnableWindow(false);
#endif
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

#if WithNumericTags
  m_MasterNumTagsOK = !pMPI->bMasterNumTagsBAD;
#endif
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
#if WithNumericTags
    m_SlaveNumTagsOK = !(SlavePF.RdLong("General", "NumericTagsBad", TaggedObject::NumericTagsBad) != 0);
#endif
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
#if WithNumericTags
    if (m_SlaveNumTagsOK && !m_MasterNumTagsOK)
      OK=FALSE;
#endif
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
#if WithNumericTags
    int TagsChngd = gs_Exec.CheckAllTags(!m_MasterNumTagsOK, !m_MasterStNumTagsOK);
#else
    int TagsChngd = gs_Exec.CheckAllTags(!m_MasterStNumTagsOK);
#endif
    if (TagsChngd<0)
      {
      TagsOK=false;
      pMPI->sError="Numeric tags need to be changed";
      //TaggedObject::NumericTagsBad = 0;
      //TaggedObject::NumericStartingTagsBad = 0;
      }
    else
      {
#if WithNumericTags
      pMPI->bMasterNumTagsBAD   = !m_MasterNumTagsOK;
#endif
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
#if WithNumericTags
    pMPI->bMasterNumTagsBAD   = !m_MasterNumTagsOK;
#endif
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
#if WithNumericTags
  m_MasterNumTagsOK;
#endif
  m_MasterStNumTagsOK;
#if WithNumericTags
  m_MasterNumTagsOK = m_MasterNumTagsOK && m_MasterStNumTagsOK;
#endif
  UpdateSlaveNumTagsOK();
  UpdateData(FALSE);
  }

//--------------------------------------------------------------------------

void CMergeDlg::OnMstrnumtags()
  {
  UpdateData(TRUE);
  bTagNamingChanged = 1;
#if WithNumericTags
  m_MasterStNumTagsOK = m_MasterStNumTagsOK || m_MasterNumTagsOK;
#endif
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
              if (_stricmp(pMPI->PostNewRefFiles[i](), "Unknown")==0)
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
                    if (_stricmp(s1(), s2())==0)
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
                    if (_stricmp(s1(), s2())==0)
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
              if (_stricmp(pMPI->PostNewRefFiles[i](), "Exists")==0)
                m_List.AddString(pMPI->PostChangeRefFiles[i]());
            m_List.AddString("");
            }
          if (pMPI->bIgnoreProbRefFiles && pMPI->iRefFilesUnknownCnt>0)
            {
            m_List.AddString("Warning: Referenced files from imported project that will NOT be copied to current project:");
            for (i=0; i<pMPI->PostChangeRefFiles.GetSize(); i++)
              if (_stricmp(pMPI->PostNewRefFiles[i](), "Unknown")==0)
                m_List.AddString(pMPI->PostChangeRefFiles[i]());
            m_List.AddString("");
            }
          if (pMPI->iRefFilesUnknownCnt+pMPI->iRefFilesExistsCnt<pMPI->PostChangeRefFiles.GetSize())
            m_List.AddString("Files to be copied into current project:");
          }
        for (i=0; i<pMPI->PostChangeRefFiles.GetSize(); i++)
          if (_stricmp(pMPI->PostNewRefFiles[i](), "Unknown")!=0 && _stricmp(pMPI->PostNewRefFiles[i](), "Exists")!=0)
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

//--------------------------------------------------------------------------
