//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "syscad.h"
#include "project.h"
#include "mainfrm.h"
#include "..\schist\hstmain.h"
#include "accnode.h"
#include "tagvdoc.h"
#include "tagvdlgs.h"
#include "grfdoc.h"
#include "toolbars.h"
#include "helpinfo.h"
#include "gendlgs.h"
#include "cmd_mngr.h"
#include "scd_wm.h"
#include "dbgmngr.h"
#include "msgwnd.h"
#include "ordwnd.h"
#include "statswnd.h"
#include "wndslct.h"
#include "explorescd.h"
#include "scdver.h"
#include "scdcmdif.h"
#include "flwnode.h"
#include "dbhelper.h"
//#include "optoff.h"

extern "C" 
  {
  #include "grldefs.h"
  }

#include "afxwin.h"
#include ".\gendlgs.h"
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

#ifdef _RELEASE
BOOL bDebugOptionsVisible = FALSE;
#else
BOOL bDebugOptionsVisible = TRUE;
#endif

//===========================================================================

class CToolbarPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CToolbarPage)
  public:
    CToolbarPage();

    flag bChanged:1;
    int iTBsUsed;
    BOOL m_TBs[IDC_TB7-IDC_TB0+1];
    CString m_Titles[IDC_TB7-IDC_TB0+1];
    //{{AFX_DATA(CToolbarPage)
    enum { IDD = IDD_GENTOOLBARS };
    BOOL m_bStatusBar;
    BOOL m_FlyBys;
    BOOL m_ToolTips;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CToolbarPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CToolbarPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnChanged();
    //}}AFX_MSG
    afx_msg void OnEditBnClicked(UINT nID);
    afx_msg void OnChanged(UINT nID);
    afx_msg void OnDefaultBnClicked(UINT nID);
    virtual void OnOK();
    virtual void OnCancel();
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CGlobalPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CGlobalPage)
  public:
    CGlobalPage();
    ~CGlobalPage();
    //{{AFX_DATA(CGlobalPage)
    enum { IDD = IDD_GENGLOBAL };
    BOOL m_EvalOrdWnd;
    BOOL m_EvalStatsWnd;
    BOOL m_DlgTooltips;
    BOOL m_LoadPrevPrj;
    CString m_Editor;
    int m_UtilsDBFmt;
    BOOL m_UseScdExplorer;
    int m_PrjsDBFmt;
    BOOL m_CascadeOnLoad;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CGlobalPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    int PrevUse97;
    //{{AFX_MSG(CGlobalPage)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
  public:
    DWORD m_ServerWait;
    CComboBox m_cbUtilDB;
    CComboBox m_cbPrjDB;
  public:
    afx_msg void OnBnClickedBrowseEditor();
  };

//===========================================================================

class CTrendsPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CTrendsPage)
  public:
    CTrendsPage();
    ~CTrendsPage();

    //{{AFX_DATA(CTrendsPage)
    enum { IDD = IDD_GENTRENDS };
    BOOL m_DefRecordIt;
    BOOL m_Global;
    BOOL	m_Flash;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CTrendsPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CTrendsPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnBrowse();
    afx_msg void OnRebuild();
    //}}AFX_MSG
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class COtherPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(COtherPage)
  public:
    COtherPage();

    flag     bGrfChanged:1,
             bAllGrfLoaded:1;

    //{{AFX_DATA(COtherPage)
    enum { IDD = IDD_GENOTHER };
    BOOL m_GrfDeferLoad;
    BOOL m_GrfAutoLoad;
    BOOL m_GrfUpdateAnnot;
    BOOL m_GrfAnimationOn;
    //BOOL m_GrfShowStatus;
    BOOL m_CopyDBFromOld;
    BOOL m_RemovePrevRev;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(COtherPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(COtherPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnLoadGrf();
    afx_msg void OnGrfDeferLoad();
    afx_msg void OnGrfChanged();
    //}}AFX_MSG
    afx_msg void OnUpdateLoadGrf(CCmdUI* pCmdUi);
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
  public:
    CComboBox m_GrfBehaviourCB;
  public:
    CComboBox m_TrndBehaviourCB;
  public:
    afx_msg void OnCbnSelchangeGrfbehaviour();
  public:
    afx_msg void OnCbnSelchangeTrndbehaviour();
  };

//===========================================================================

class CMousePage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CMousePage)
  public:
    CMousePage();
        
    //{{AFX_DATA(CMousePage)
    enum { IDD = IDD_GENMOUSE };
    CComboBox	m_RightCtSh;
    CComboBox	m_LeftCtSh;
    CComboBox	m_Left;
    CComboBox	m_LeftDbl;
    CComboBox	m_LeftCtrl;
    CComboBox	m_LeftShft;
    CComboBox	m_Right;
    CComboBox	m_RightDbl;
    CComboBox	m_RightCtrl;
    CComboBox	m_RightShft;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CMousePage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CMousePage)
    virtual BOOL OnInitDialog();
    afx_msg void OnDefault1();
    afx_msg void OnDefault2();
    //}}AFX_MSG
    afx_msg void OnUpdateLoadGrf(CCmdUI* pCmdUi);
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CStatusColourButton : public CButton
  {
  virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
    {
    int ID=lpDrawItemStruct->CtlID;
    int Colour=0;
    switch (ID)
      {
      case IDC_GRCOLHIGHLITE: 
        Colour=GR_HIGHLIGHT; 
        break;
      case IDC_GRCOLOTHER: 
        Colour=GR_OTHERGRAPHICS; 
        break;
      default:
        Colour=(ID-IDC_GRCOLSTATUS0)+GR_FIRSTSTATUSCOLOR;
        break;
      }
    
    COLORREF RGB=grl_get_win_color(Colour);
    HBRUSH hBrush=CreateSolidBrush(RGB);
    ::FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, hBrush);
//    int bm=::SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT);
//    int ii=(ID %5);
//    ::DrawText(lpDrawItemStruct->hDC, Strs[ii], strlen(Strs[ii]), &lpDrawItemStruct->rcItem, DT_SINGLELINE |DT_CENTER);
//    ::SetBkMode(lpDrawItemStruct->hDC, bm);
    DeleteObject(hBrush);
    }
  };

//===========================================================================

const int MaxMdlStatusTypes=24;

class CColourPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CColourPage)
  public:
    CColourPage();
    virtual ~CColourPage();

    //{{AFX_DATA(CColourPage)
	  enum { IDD = IDD_GENCOLORS };
	  CComboBox	m_PenStyle[MaxMdlStatusTypes];
    CStatusColourButton	m_ButtonOther;
    CStatusColourButton m_ButtonHL;
    CStatusColourButton	m_Button[MaxMdlStatusTypes];
	  BOOL	m_StatusOn;
    double m_dVapTest;
    double m_dSolTest;
  	//}}AFX_DATA
    //{{AFX_VIRTUAL(CColourPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CColourPage)
    virtual BOOL OnInitDialog();
	  afx_msg void OnGrcolhighlite();
	  afx_msg void OnGrcolother();
	  //}}AFX_MSG
	  afx_msg void OnGrcolStatus(UINT nID);
//    afx_msg void OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct );

    virtual void OnOK();
    DECLARE_MESSAGE_MAP()

  public:
    BOOL m_ShowEditStatus;
  };

//===========================================================================

class CStatsPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CStatsPage)
  public:
    CStatsPage();
    ~CStatsPage();

    //{{AFX_DATA(CStatsPage)
    enum { IDD = IDD_GENSTATS };
    CString m_StatsFilename;
    DWORD m_StatsCnt;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CStatsPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CStatsPage)
    afx_msg void OnChangeRefreshcnt();
    virtual BOOL OnInitDialog();
    afx_msg void OnHstfetch();
    afx_msg void OnHstreset();
    //}}AFX_MSG
    afx_msg LRESULT OnUpdateHstStats(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUpdateBtn(CCmdUI* pCmdUi);
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================
#if WITHSCRCYCLES
class CScreenSavePage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CScreenSavePage)
  public:
    CScreenSavePage();
    ~CScreenSavePage();

    int AllocLen;
    CScreenSaverInfo* Cycles;
    int CurSelNo;
    CStringArray Windows;

    //{{AFX_DATA(CScreenSavePage)
    enum { IDD = IDD_GENSCRSAVEDLG };
    CButton m_wndUpButton;
    CButton m_wndDownButton;
    CCustomListBox m_WindowList;
    CComboBox m_CycleList;
    int m_ScrCycles;
    BOOL m_SyscadScrSave;
    BOOL m_SystemScrSave;
    CString m_Description;
    int m_WaitTime;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CScreenSavePage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CScreenSavePage)
    virtual BOOL OnInitDialog();
    afx_msg void OnScrSaveCheck();
    afx_msg void OnSelchangeCylelist();
    afx_msg void OnChangeScrcycles();
    afx_msg void OnSelchangeWindowlist();
    afx_msg void OnDblclkWindowlist();
    //}}AFX_MSG
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
  };
#endif
//===========================================================================

class CDebugPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CDebugPage)
  public:
    CDebugPage();
    ~CDebugPage();

    flag bChanged;
    Strng DbgFn;
    int m_currGenSexnIndex;
    CString m_currSubSexnName;

    //{{AFX_DATA(CDebugPage)
    enum { IDD = IDD_GENDBGDLG };
    CEdit m_dbgValueCntrlVar;
    CComboBox m_genSexnCntrlVar;
    CListBox m_subSexnCntrlVar;
    BOOL m_bDbgOn;
    long m_dbgValueValVar;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CDebugPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CDebugPage)
    afx_msg void OnDbgdelete();
    virtual BOOL OnInitDialog();
    afx_msg void OnSelChangeGenSexns();
    afx_msg void OnSelChangeSubSexns();
    afx_msg void OnDbgOn();
    afx_msg void OnUpdateValue();
    afx_msg void OnKillfocusValue();
    afx_msg void OnSetfocusValue();
    afx_msg void OnResetVal();
    afx_msg void OnDblclkSubsexns();
    afx_msg void OnKillfocusGensexns();
    //}}AFX_MSG
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
  public :
    //helper functions
    void GetSubSexnName( CString &ListBoxEntry );
  };

//===========================================================================

class CSnapShotPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CSnapShotPage)
  public:
    CSnapShotPage();
    ~CSnapShotPage();
    //{{AFX_DATA(CSnapShotPage)
    enum { IDD = IDD_GENSNAPSHOTS };
    int m_Prompt;
    CString m_FixedName;
    CString m_Prefix;
    UINT m_Number;
    BOOL m_DoZip;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CSnapShotPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CSnapShotPage)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

IMPLEMENT_DYNAMIC(CGenSheet, CPropertySheet)

CGenSheet::CGenSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
  : CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
  {
  ASSERT(FALSE); //DO NOT EXPECT TO GET HERE !!!
  }

//---------------------------------------------------------------------------

CGenSheet::CGenSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
  : CPropertySheet(pszCaption, pParentWnd, iSelectPage)
  {
  if (gs_pPrj && gs_pPrj->pPrjDoc)
    {
    pToolBarPage    = new CToolbarPage;
    pTrendsPage     = new CTrendsPage;
    pOtherPage      = new COtherPage;
    pMousePage      = new CMousePage;
    pColourPage     = new CColourPage;
    pStatsPage      = new CStatsPage;
    pSnapShotPage   = new CSnapShotPage;
    #if WITHSCRCYCLES
    pScreenSavePage = new CScreenSavePage;
    #endif
    }
  else
    {
    pToolBarPage    = NULL;
    pTrendsPage     = NULL;
    pOtherPage      = NULL;
    pMousePage      = NULL;
    pColourPage     = NULL;
    pStatsPage      = NULL;
    pSnapShotPage   = NULL;
    #if WITHSCRCYCLES
    pScreenSavePage = NULL;
    #endif
    }
  pGlobalPage     = new CGlobalPage;
  if (bDebugOptionsVisible || dbgtestfileopen())
    pDebugPage = new CDebugPage;
  else
    pDebugPage = NULL;

  if (pGlobalPage)
    AddPage(pGlobalPage);
  if (pOtherPage)
    AddPage(pOtherPage);
  if (pMousePage)
    AddPage(pMousePage);
  if (pToolBarPage)
    AddPage(pToolBarPage);
  if (pTrendsPage)
    AddPage(pTrendsPage);
  if (pColourPage)
    AddPage(pColourPage);
  if (pStatsPage)
    AddPage(pStatsPage);
  if (pSnapShotPage)
    AddPage(pSnapShotPage);
  #if WITHSCRCYCLES
  if (pScreenSavePage)
    AddPage(pScreenSavePage);
  #endif
  if (pDebugPage)
    AddPage(pDebugPage);
  //EnableStackedTabs(FALSE);
  //m_psh.dwFlags |= PSH_MULTILINETABS;
  EnableStackedTabs(FALSE);
  if (!Create(pParentWnd, WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | WS_VISIBLE, WS_EX_DLGMODALFRAME/* | WS_EX_SMCAPTION*/))
    {
    TRACE("Failed to create General PropertySheet\n");
    }
  }

//---------------------------------------------------------------------------

CGenSheet::~CGenSheet()
  {
  MainWnd()->m_pGenSheet = NULL; //set global pointer to property sheet to null
  delete pToolBarPage;
  delete pGlobalPage;
  delete pTrendsPage;
  delete pOtherPage;
  delete pMousePage;
  delete pColourPage;
  delete pStatsPage;
  delete pSnapShotPage;
#if WITHSCRCYCLES
  delete pScreenSavePage;
#endif
  delete pDebugPage;
  //dialog just closed, ensure main SysCAD window becomes active ...
  MainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_UPDATE, 0);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CGenSheet, CPropertySheet)
  //{{AFX_MSG_MAP(CGenSheet)
  ON_WM_NCDESTROY()
  ON_WM_CLOSE()
  ON_COMMAND(IDOK,OnOK)
  ON_COMMAND(IDCANCEL,OnCancel)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CGenSheet::OnInitDialog() 
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

void CGenSheet::StoreCurPageNo()
  {
  CPropertyPage* pPage = GetActivePage();
  if (gs_pPrj && gs_pPrj->pPrjDoc && pPage)
    {
    for (int i=0; i<GetPageCount(); i++)
      if (GetPage(i)==pPage)
        {
        CProfINIFile PF(PrjIniFile());
        PF.WrInt("General", "GenOptionsPageNo", i);
        }
    }
  }

//---------------------------------------------------------------------------
// overridden to ensure that a modeless property sheet behaves as a modal
void CGenSheet::OnOK() 
  {
  ASSERT_VALID(this);
  StoreCurPageNo();
  for (int i=0; i<GetPageCount(); i++)
    GetPage(i)->OnOK();
  if (GetActivePage()->OnKillActive())
    {
    //GetActivePage()->OnOK();
    EndDialog(IDOK);
    }
  }

//---------------------------------------------------------------------------
// overridden to ensure that a modeless property sheet behaves as a modal
void CGenSheet::OnCancel() 
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
void CGenSheet::OnClose()
  {
  ASSERT_VALID(this);
  OnCancel();
  }

//---------------------------------------------------------------------------

flag CGenSheet::IsStatsPage()
  { 
  return (GetActivePage() && GetActivePage()->IsKindOf(RUNTIME_CLASS(CStatsPage)));
  }

//---------------------------------------------------------------------------

void CGenSheet::OnNcDestroy() 
  {
  CPropertySheet::OnNcDestroy();
  delete this;
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CToolbarPage, CCustomPropertyPage)

CToolbarPage::CToolbarPage() : CCustomPropertyPage(CToolbarPage::IDD)
  {
  bChanged = 0;
  //{{AFX_DATA_INIT(CToolbarPage)
  m_bStatusBar = FALSE;
  m_FlyBys = FALSE;
  m_ToolTips = FALSE;
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CToolbarPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CToolbarPage)
  DDX_Check(pDX, IDC_TB_STATUSBAR, m_bStatusBar);
  DDX_Check(pDX, IDC_TBFLYBYS, m_FlyBys);
  DDX_Check(pDX, IDC_TBTOOLTIPS, m_ToolTips);
  //}}AFX_DATA_MAP
  for (int i=0; i<iTBsUsed; i++)
    {
    DDX_Check(pDX, IDC_TB0+i, m_TBs[i]);
    DDX_Text(pDX, IDC_TBTITLE0+i, m_Titles[i]);
    }
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CToolbarPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CToolbarPage)
  ON_BN_CLICKED(IDC_TBFLYBYS, OnChanged)
  ON_BN_CLICKED(IDC_TBTOOLTIPS, OnChanged)
  ON_BN_CLICKED(IDC_TB_STATUSBAR, OnChanged)
  //}}AFX_MSG_MAP
  ON_CONTROL_RANGE(BN_CLICKED, IDC_TB0, IDC_TB7, OnChanged)
  ON_CONTROL_RANGE(EN_CHANGE, IDC_TBTITLE0, IDC_TBTITLE7, OnChanged)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_TBEDIT0, IDC_TBEDIT7, OnEditBnClicked)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_TBDEF0, IDC_TBDEF7, OnDefaultBnClicked)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CToolbarPage::OnInitDialog() 
  {
  CMainFrame* w = MainWnd();
  m_bStatusBar = ((w->m_wndStatusBar.GetStyle() & WS_VISIBLE) != 0);
  VERIFY(IDC_TB7-IDC_TB0+1 == MaxDynToolBars);
  iTBsUsed = 0;
  for (int i=0; i<MaxDynToolBars; i++)
    {
    CDynToolBar* pTB = (*pMainTBMngr)[i];
    if (pTB)
      {
      m_Titles[i] = pTB->GetTitle();
      //GetDlgItem(IDC_TBTITLE0+i)->SetWindowText(pTB->GetTitle());
      if (!pTB->Configurable())
        GetDlgItem(IDC_TBEDIT0+i)->EnableWindow(FALSE);
      m_TBs[i] = pTB->IsWindowVisible();//((pTB->GetStyle() & WS_VISIBLE) != 0);
      iTBsUsed++;
      }
    else
      {
      GetDlgItem(IDC_TB0+i)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_TBTITLE0+i)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_TBEDIT0+i)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_TBDEF0+i)->ShowWindow(SW_HIDE);
      }
    }

  m_ToolTips = gs_pPrj->m_bToolTips;
  m_FlyBys = gs_pPrj->m_bFlyBys;

  CCustomPropertyPage::OnInitDialog();

  return TRUE;
  }

//---------------------------------------------------------------------------

void CToolbarPage::OnOK() 
  {
  if (bDidInit)
    {
    if (bChanged)
      {
      UpdateData(True);
      CMainFrame* w = MainWnd();
      w->ShowControlBar(&(w->m_wndStatusBar), m_bStatusBar, FALSE);
      for (int i=0; i<iTBsUsed; i++)
        {
        CDynToolBar* pTB = (*pMainTBMngr)[i];
        if (strcmp(pTB->GetTitle(), (const char*)m_Titles[i])!=0)
          pTB->SetTitle((char*)(const char*)m_Titles[i]);
        w->ShowControlBar(pTB, m_TBs[i], FALSE);
        }

      gs_pPrj->m_bToolTips = m_ToolTips;
      gs_pPrj->m_bFlyBys = m_FlyBys;

      w->UpdateToolBars();
      }
    }
  }

//---------------------------------------------------------------------------

void CToolbarPage::OnCancel() 
  {
  if (bDidInit && bChanged)
    {
    MainWnd()->RecalcLayout();
    }
  }

//---------------------------------------------------------------------------

void CToolbarPage::OnEditBnClicked(UINT nID)
  {
  CDynToolBar* pTB = (*pMainTBMngr)[nID-IDC_TBEDIT0];
  if (pTB->Configurable())
    {
    pTB->Customize();
    bChanged = 1;
    //MainWnd()->m_pGenSheet->SetActiveWindow();
    MainWnd()->m_pGenSheet->BringWindowToTop();
    }
  }

//---------------------------------------------------------------------------

void CToolbarPage::OnDefaultBnClicked(UINT nID)
  {
  UpdateData(TRUE);
  MainWnd()->DefaultToolBar(nID-IDC_TBDEF0, m_TBs[nID-IDC_TBDEF0]);
  m_Titles[nID-IDC_TBDEF0] = (*pMainTBMngr)[nID-IDC_TBDEF0]->GetTitle();
  UpdateData(FALSE);
  bChanged = 1;
  }

//---------------------------------------------------------------------------

void CToolbarPage::OnChanged(UINT nID)
  {
  bChanged = 1;
  }

//---------------------------------------------------------------------------

void CToolbarPage::OnChanged() 
  {
  bChanged = 1;
  }

//---------------------------------------------------------------------------
//===========================================================================

IMPLEMENT_DYNCREATE(CGlobalPage, CCustomPropertyPage)

CGlobalPage::CGlobalPage() : CCustomPropertyPage(CGlobalPage::IDD)
, m_ServerWait(10000)
  {
  //{{AFX_DATA_INIT(CGlobalPage)
  m_EvalOrdWnd = FALSE;
  m_EvalStatsWnd = FALSE;
  m_DlgTooltips = FALSE;
  m_LoadPrevPrj = FALSE;
  m_Editor = "";
	m_UtilsDBFmt = DBConnect_JET2000;
  m_PrjsDBFmt = DBConnect_JET2000;
  m_UseScdExplorer = TRUE;
  m_CascadeOnLoad = FALSE;
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CGlobalPage::~CGlobalPage()
  {
  }

//---------------------------------------------------------------------------

void CGlobalPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CGlobalPage)
  DDX_Check(pDX, IDC_EVALORDWND, m_EvalOrdWnd);
  DDX_Check(pDX, IDC_DLGTOOLTIPS, m_DlgTooltips);
  DDX_Check(pDX, IDC_LOADPREVPRJ, m_LoadPrevPrj);
  DDX_Text(pDX, IDC_EDITOR, m_Editor);
  DDX_Check(pDX, IDC_USESCDEXPLORER, m_UseScdExplorer);
  DDX_Check(pDX, IDC_CASCADEWINDOWSONLOAD, m_CascadeOnLoad);
  //}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_SERVERWAIT, m_ServerWait);
  DDV_MinMaxUInt(pDX, m_ServerWait, 1, 3600);
  DDX_Control(pDX, IDC_UTILDBCB, m_cbUtilDB);
  DDX_Control(pDX, IDC_PRJDBCB, m_cbPrjDB);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CGlobalPage, CCustomPropertyPage)
	//{{AFX_MSG_MAP(CGlobalPage)
	//}}AFX_MSG_MAP
  //ON_UPDATE_COMMAND_UI(IDC_DB2000, OnUpdateDefMDB)
  ON_BN_CLICKED(IDC_BROWSE_EDITOR, &CGlobalPage::OnBnClickedBrowseEditor)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CGlobalPage::OnInitDialog() 
  {
	m_EvalOrdWnd       = (ScdPFUser.RdInt("General", "ShowEvalOrderWindow", 0)!=0);
  m_UseScdExplorer   = (ScdPFUser.RdInt("General", "UseScdExplorer", 1)!=0);
  m_DlgTooltips      = (ScdPFUser.RdInt("General", "ShowDialogTooltips", 1)!=0);
  m_LoadPrevPrj      = (ScdPFUser.RdInt("General", "PromptPrevPrj", 0)!=0);
  m_CascadeOnLoad    = (ScdPFUser.RdInt("General", "CascadeOnLoad", 0)!=0);
  m_Editor           = ScdPFUser.RdStr("General", "TextEditor", "notepad.exe");
	m_UtilsDBFmt       = ScdPFUser.RdInt("General", "UtilsDBFormat", m_UtilsDBFmt);
	m_PrjsDBFmt        = ScdPFUser.RdInt("General", "PrjsDBFormat", m_PrjsDBFmt);
  m_ServerWait       = ScdPFUser.RdInt("General", "ServerStartUpWait", 10000)/1000;
  PrevUse97 = m_UtilsDBFmt;
  char Buff[_MAX_PATH];
  GetEnvironmentVariable("WINDIR", Buff, sizeof(Buff));
  strcat(Buff, "\\");
  strcat(Buff, ScdApp()->m_pszProfileName);
  SetDlgItemText(IDC_INI_TXT, Buff);

  CCustomPropertyPage::OnInitDialog();
  UpdateDialogControls(this, FALSE);
  for (int i=0; i<DBConnectCount(); i++)
    {
    m_cbUtilDB.AddString(DBConnectName(i));
    m_cbPrjDB.AddString(DBConnectName(i));
    }
  m_cbUtilDB.SetCurSel(m_UtilsDBFmt);
  m_cbPrjDB.SetCurSel(m_PrjsDBFmt);
  return TRUE;  // return TRUE unless you set the focus to a control
  }

//---------------------------------------------------------------------------

void CGlobalPage::OnBnClickedBrowseEditor()
  {
  CSCDFileDialog Dlg(true, NULL, "*.exe", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, "Test Editor (*.exe)|*.exe||");

  Strng Fn(m_Editor);
  Fn.FnDrivePath();

  Dlg.m_ofn.lpstrInitialDir = (Fn.Len()>0 ? Fn() : ProgFiles());
  Dlg.m_ofn.lpstrTitle = "Text Editor";
  HWND H=::GetActiveWindow();
  if (Dlg.DoModal()==IDOK)
    {
    m_Editor=Dlg.GetPathName();
    //m_Editor+=Dlg.GetFileName();
    UpdateData(FALSE);
    }
  ::SetActiveWindow(H);
  }

//---------------------------------------------------------------------------

void CGlobalPage::OnOK() 
  {
  if (bDidInit)
    {
    UpdateData(True);
    m_UtilsDBFmt=m_cbUtilDB.GetCurSel();
    m_PrjsDBFmt=m_cbPrjDB.GetCurSel();

    int PrevUseScdExplorer = ScdPFUser.RdInt("General", "UseScdExplorer", 1);
    if (m_Editor.GetLength()==0)
      m_Editor = "notepad.exe";
    ScdPFUser.WrInt("General", "ShowEvalOrderWindow", m_EvalOrdWnd);
    ScdPFUser.WrInt("General", "ShowEvalStatsWindow", m_EvalStatsWnd);
    ScdPFUser.WrInt("General", "UseScdExplorer", m_UseScdExplorer);
    ScdPFUser.WrInt("General", "PromptPrevPrj", m_LoadPrevPrj);
    ScdPFUser.WrInt("General", "CascadeOnLoad", m_CascadeOnLoad);
    ScdPFUser.WrInt("General", "ShowDialogTooltips", m_DlgTooltips);
    ScdPFUser.WrInt("General", "UtilsDBFormat", m_UtilsDBFmt);
    ScdPFUser.WrInt("General", "PrjsDBFormat", m_PrjsDBFmt);
    ScdPFUser.WrInt("General", "ServerStartUpWait", m_ServerWait*1000);
    ScdPFUser.WrStr("General", "TextEditor", (const char*)m_Editor);
    SetTxtEditor((char*)(const char*)m_Editor);
    HelpMngr.UpdateOptions();

    if (COrdWindow::GetFrame()==NULL && m_EvalOrdWnd)
      COrdWindow::Register();

    if (CStatsWindow::GetFrame()==NULL && m_EvalStatsWnd)
      CStatsWindow::Register();

    if (PrevUseScdExplorer!=m_UseScdExplorer)
      {
      CExploreScd::SetUseScdExplorer(m_UseScdExplorer);
      if (m_UseScdExplorer==0)
        CExploreScd::CloseIt();
      }

    if (PrevUse97!=m_UtilsDBFmt)
      AfxMessageBox("Change to database driver selected will only take affect when SysCAD is restarted.");
    }
  }

//---------------------------------------------------------------------------
//===========================================================================

IMPLEMENT_DYNCREATE(CTrendsPage, CCustomPropertyPage)

CTrendsPage::CTrendsPage() : CCustomPropertyPage(CTrendsPage::IDD)
  {
  //{{AFX_DATA_INIT(CTrendsPage)
  m_DefRecordIt = FALSE;
  m_Global = FALSE;
	m_Flash = FALSE;
	//}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

CTrendsPage::~CTrendsPage()
  {
  }

//--------------------------------------------------------------------------

void CTrendsPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTrendsPage)
  DDX_Check(pDX, IDC_PRJDEFRECORDIT, m_DefRecordIt);
  DDX_Check(pDX, IDC_GLOBAL, m_Global);
	DDX_Check(pDX, IDC_FLASHTREND, m_Flash);
	//}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTrendsPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CTrendsPage)
  ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
  ON_BN_CLICKED(IDC_REBUILD, OnRebuild)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CTrendsPage::OnInitDialog() 
  {
  m_DefRecordIt = gs_pPrj->m_bDefaultRecordIt;
  m_Global = CTagVwDoc::TimeBaseGlobal();
  m_Flash = gs_pPrj->m_bFlashTrend;

  CCustomPropertyPage::OnInitDialog();
  return TRUE;  // return TRUE unless you set the focus to a control
  }

//--------------------------------------------------------------------------

void CTrendsPage::OnBrowse() 
  {
  CTrndDlg Dlg("", this);
  Dlg.DoModal();
  }

//--------------------------------------------------------------------------

void CTrendsPage::OnRebuild() 
  {
  CTagVwDoc::RebuildAll();
  }

//--------------------------------------------------------------------------

void CTrendsPage::OnOK() 
  {
  if (bDidInit)
    {
    UpdateData(True);
    gs_pPrj->m_bDefaultRecordIt = m_DefRecordIt;
    if (CTagVwDoc::TimeBaseGlobal()!=m_Global)
      CTagVwDoc::DoSetTimeGlobal(m_Global);
    gs_pPrj->m_bFlashTrend=m_Flash;
    }
  }

//--------------------------------------------------------------------------
//===========================================================================

IMPLEMENT_DYNCREATE(COtherPage, CCustomPropertyPage)

COtherPage::COtherPage() : CCustomPropertyPage(COtherPage::IDD)
  {
  bGrfChanged = 0;
  bAllGrfLoaded = 0;
  //{{AFX_DATA_INIT(COtherPage)
  m_GrfDeferLoad = FALSE;
  m_GrfAutoLoad = FALSE;
  m_GrfUpdateAnnot = FALSE;
  m_GrfAnimationOn = FALSE;
  //m_GrfShowStatus = TRUE;
	m_CopyDBFromOld = FALSE;
	m_RemovePrevRev = FALSE;
  //}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

void COtherPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(COtherPage)
  DDX_Check(pDX, IDC_PRJGRFDEFERLOAD, m_GrfDeferLoad);
  DDX_Check(pDX, IDC_PRJGRFAUTOLOAD, m_GrfAutoLoad);
  DDX_Check(pDX, IDC_UPDATEANNOT, m_GrfUpdateAnnot);
  DDX_Check(pDX, IDC_ANIMATIONON, m_GrfAnimationOn);
  //DDX_Check(pDX, IDC_ANIMATIONSTATUS, m_GrfShowStatus);
  DDX_Check(pDX, IDC_DBCOPYFROMOLD, m_CopyDBFromOld);
  DDX_Check(pDX, IDC_DBREMOVEPREVREVS, m_RemovePrevRev);
  //DDX_Check(pDX, IDC_KEEPGRFSIZED, m_KeepGrfSized);
  //DDX_Check(pDX, IDC_KEEPTRNDSIZED, m_KeepTrndSized);
  //}}AFX_DATA_MAP
  GetDlgItem(IDC_PRJGRFAUTOLOAD)->EnableWindow(m_GrfDeferLoad);
  DDX_Control(pDX, IDC_GRFBEHAVIOUR, m_GrfBehaviourCB);
  DDX_Control(pDX, IDC_TRNDBEHAVIOUR, m_TrndBehaviourCB);
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COtherPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(COtherPage)
  ON_BN_CLICKED(IDC_PRJLOADGRF, OnLoadGrf)
  ON_BN_CLICKED(IDC_PRJGRFDEFERLOAD, OnGrfDeferLoad)
  ON_BN_CLICKED(IDC_PRJGRFAUTOLOAD, OnGrfChanged)
  ON_BN_CLICKED(IDC_UPDATEANNOT, OnGrfChanged)
  ON_BN_CLICKED(IDC_ANIMATIONON, OnGrfChanged)
  //ON_BN_CLICKED(IDC_ANIMATIONSTATUS, OnGrfChanged)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDC_PRJLOADGRF, OnUpdateLoadGrf)
  //ON_BN_CLICKED(IDC_KEEPGRFSIZED, &COtherPage::OnBnClickedKeepgrfsized)
  //ON_BN_CLICKED(IDC_KEEPTRNDSIZED, &COtherPage::OnBnClickedKeeptrndsized)
  ON_CBN_SELCHANGE(IDC_GRFBEHAVIOUR, &COtherPage::OnCbnSelchangeGrfbehaviour)
  ON_CBN_SELCHANGE(IDC_TRNDBEHAVIOUR, &COtherPage::OnCbnSelchangeTrndbehaviour)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

static struct { LPCTSTR m_Str; int m_Val; } gs_WBCBValues[] =
  {
    {"Separate",              WB_None         },
    {"Size Together",         WB_SizeTogether },
    {"Move Together",         WB_MoveTogether },
    {"Move & Size Together",  WB_Together     },
    {"Treat As One",          WB_Coincident   },
    {0},
  };

BOOL COtherPage::OnInitDialog() 
  {
  m_GrfDeferLoad   = gs_pPrj->m_bGrfDeferLoad;
  m_GrfAutoLoad    = gs_pPrj->m_bGrfAutoLoad;
  m_GrfUpdateAnnot = gs_pPrj->m_bGrfUpdateAnnot;
  m_GrfAnimationOn = gs_pPrj->m_bGrfAnimationOn;
  //m_GrfShowStatus  = gs_pPrj->ShowStatus();
  bAllGrfLoaded    = gs_pPrj->AllGrfLoaded();
  m_RemovePrevRev  = gs_pPrj->m_bRemoveRevs;
  m_CopyDBFromOld  = false;//ALWAYS default to false!!! gs_pPrj->m_bCopyDBFiles;

  CCustomPropertyPage::OnInitDialog();

  int ig=0;
  int it=0;
  for (int i=0; gs_WBCBValues[i].m_Str; i++)
    {
    m_GrfBehaviourCB.AddString(gs_WBCBValues[i].m_Str);
    m_TrndBehaviourCB.AddString(gs_WBCBValues[i].m_Str);
    if (gs_WBCBValues[i].m_Val==gs_pPrj->m_GrfBehaviour)
      ig=i;
    if (gs_WBCBValues[i].m_Val==gs_pPrj->m_TrndBehaviour)
      it=i;
    }
  m_GrfBehaviourCB.SetCurSel(ig);
  m_TrndBehaviourCB.SetCurSel(it);

  UpdateDialogControls(this, FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
  }

//--------------------------------------------------------------------------

void COtherPage::OnOK() 
  {
  if (bDidInit)
    {
    UpdateData(True);
    if (bGrfChanged)
      {
      gs_pPrj->m_bGrfDeferLoad = m_GrfDeferLoad;
      gs_pPrj->m_bGrfAutoLoad = m_GrfAutoLoad;
      gs_pPrj->m_bGrfUpdateAnnot = m_GrfUpdateAnnot;
      gs_pPrj->m_bGrfAnimationOn = m_GrfAnimationOn;
      //gs_pPrj->SetShowStatus(m_GrfShowStatus!=0);
      gs_pPrj->m_GrfBehaviour   = gs_WBCBValues[m_GrfBehaviourCB.GetCurSel()].m_Val; 
      gs_pPrj->m_TrndBehaviour  = gs_WBCBValues[m_TrndBehaviourCB.GetCurSel()].m_Val;
      }
    gs_pPrj->m_bRemoveRevs  = m_RemovePrevRev; 
    gs_pPrj->m_bCopyDBFiles = m_CopyDBFromOld; 
    }
  }

//---------------------------------------------------------------------------

void COtherPage::OnGrfDeferLoad() 
  {
  bGrfChanged = 1;
  UpdateData(True);
  }

//---------------------------------------------------------------------------

void COtherPage::OnGrfChanged() 
  {
  bGrfChanged = 1;
  }

//---------------------------------------------------------------------------

void COtherPage::OnCbnSelchangeGrfbehaviour()
  {
  bGrfChanged = 1;
  }

void COtherPage::OnCbnSelchangeTrndbehaviour()
  {
  bGrfChanged = 1;
  }

//---------------------------------------------------------------------------

void COtherPage::OnLoadGrf() 
  {
  if (!bAllGrfLoaded)
    {
    CWaitMsgCursor Wait("Loading drawings");
    for (int i=iGraphTemplate; i<=iVisioTemplate; i++)
      if (ScdApp()->TemplateExists(i))
        {
        POSITION Pos = ScdApp()->Template(i).GetFirstDocPosition();
        while (Pos)
          {
          CGrfDoc* pGrfDoc = (CGrfDoc*)(ScdApp()->Template(i).GetNextDoc(Pos));
          if (pGrfDoc->GCB.bGrfLoadDefered)
            {
            pGrfDoc->GCB.Load(pGrfDoc->GCB.DrawingFile.GetBuffer(0), False);
            pGrfDoc->GCB.bGrfLoadDefered = 0;
            }
          }
        }
    bAllGrfLoaded = 1;
    UpdateDialogControls(this, FALSE);
    }
  }

//---------------------------------------------------------------------------

void COtherPage::OnUpdateLoadGrf(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(!bAllGrfLoaded);
  }

//--------------------------------------------------------------------------
//===========================================================================

IMPLEMENT_DYNCREATE(CMousePage, CCustomPropertyPage)

CMousePage::CMousePage() : CCustomPropertyPage(CMousePage::IDD)
  {
  //{{AFX_DATA_INIT(CMousePage)
	//}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

void CMousePage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMousePage)
	DDX_Control(pDX, IDC_RIGHTBTNCTSH, m_RightCtSh);
	DDX_Control(pDX, IDC_LEFTBTNCTSH, m_LeftCtSh);
	DDX_Control(pDX, IDC_LEFTBTNDOWN, m_Left);
	DDX_Control(pDX, IDC_LEFTBTNDBL, m_LeftDbl);
	DDX_Control(pDX, IDC_LEFTBTNCTRL, m_LeftCtrl);
	DDX_Control(pDX, IDC_LEFTBTNSHIFT, m_LeftShft);
	DDX_Control(pDX, IDC_RIGHTBTNDOWN, m_Right);
	DDX_Control(pDX, IDC_RIGHTBTNDBL, m_RightDbl);
	DDX_Control(pDX, IDC_RIGHTBTNCTRL, m_RightCtrl);
	DDX_Control(pDX, IDC_RIGHTBTNSHIFT, m_RightShft);
	//}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CMousePage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CMousePage)
	ON_BN_CLICKED(IDC_DEFAULT1, OnDefault1)
	ON_BN_CLICKED(IDC_DEFAULT2, OnDefault2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CMousePage::OnInitDialog() 
  {
  CCustomPropertyPage::OnInitDialog();

  static LPCTSTR Strs[]=
    {
    "",
    "Access Closest Unit",
    "Access Menu",
    "Unit Quick View",
    "Unit Quick View & Pin",
    "Unit Quick View & Merge",
    "Model Actions Menu",
    "Show Pressure Network",
    "Show Flow Network",
    "Clear Marks",
    "Toggle Principal State",
    "Access Closest Unit (2nd window)",
    "Closest Cable Wiring Info",
    "Find Connected Feeder",
    NULL
    };
  
  for (int i=0; Strs[i]; i++)
    {
    m_Left.AddString(Strs[i]);      
    m_LeftDbl.AddString(Strs[i]);   
    m_LeftCtrl.AddString(Strs[i]);  
    m_LeftShft.AddString(Strs[i]);  
    m_LeftCtSh.AddString(Strs[i]);  
    m_Right.AddString(Strs[i]);     
    m_RightDbl.AddString(Strs[i]);  
    m_RightCtrl.AddString(Strs[i]); 
    m_RightShft.AddString(Strs[i]); 
    m_RightCtSh.AddString(Strs[i]); 
    }

  UpdateDialogControls(this, FALSE);
                                 
  m_Left.SetCurSel(GrfCmdBlk::m_Left);     
  m_LeftDbl.SetCurSel(GrfCmdBlk::m_LeftDbl);  
  m_LeftCtrl.SetCurSel(GrfCmdBlk::m_LeftCtrl); 
  m_LeftShft.SetCurSel(GrfCmdBlk::m_LeftShft); 
  m_LeftCtSh.SetCurSel(GrfCmdBlk::m_LeftCtSh); 
  m_Right.SetCurSel(GrfCmdBlk::m_Right);    
  m_RightDbl.SetCurSel(GrfCmdBlk::m_RightDbl); 
  m_RightCtrl.SetCurSel(GrfCmdBlk::m_RightCtrl);
  m_RightShft.SetCurSel(GrfCmdBlk::m_RightShft);
  m_RightCtSh.SetCurSel(GrfCmdBlk::m_RightCtSh);

  return TRUE;  // return TRUE unless you set the focus to a control
  }

//--------------------------------------------------------------------------

void CMousePage::OnOK() 
  {
  if (bDidInit)
    {
    UpdateData(True);
    GrfCmdBlk::m_Left       = (MouseBtnActs)m_Left.GetCurSel();     
    GrfCmdBlk::m_LeftDbl    = (MouseBtnActs)m_LeftDbl.GetCurSel();  
    GrfCmdBlk::m_LeftCtrl   = (MouseBtnActs)m_LeftCtrl.GetCurSel(); 
    GrfCmdBlk::m_LeftShft   = (MouseBtnActs)m_LeftShft.GetCurSel(); 
    GrfCmdBlk::m_LeftCtSh   = (MouseBtnActs)m_LeftCtSh.GetCurSel(); 
    GrfCmdBlk::m_Right      = (MouseBtnActs)m_Right.GetCurSel();    
    GrfCmdBlk::m_RightDbl   = (MouseBtnActs)m_RightDbl.GetCurSel(); 
    GrfCmdBlk::m_RightCtrl  = (MouseBtnActs)m_RightCtrl.GetCurSel();
    GrfCmdBlk::m_RightShft  = (MouseBtnActs)m_RightShft.GetCurSel();
    GrfCmdBlk::m_RightCtSh  = (MouseBtnActs)m_RightCtSh.GetCurSel();

    ScdPFUser.WrInt("Mouse", "Left",      GrfCmdBlk::m_Left);
    ScdPFUser.WrInt("Mouse", "LeftDbl",   GrfCmdBlk::m_LeftDbl);
    ScdPFUser.WrInt("Mouse", "LeftCtrl",  GrfCmdBlk::m_LeftCtrl);
    ScdPFUser.WrInt("Mouse", "LeftShft",  GrfCmdBlk::m_LeftShft);
    ScdPFUser.WrInt("Mouse", "LeftCtSh",  GrfCmdBlk::m_LeftCtSh);
    ScdPFUser.WrInt("Mouse", "Right",     GrfCmdBlk::m_Right);
    ScdPFUser.WrInt("Mouse", "RightDbl",  GrfCmdBlk::m_RightDbl);
    ScdPFUser.WrInt("Mouse", "RightCtrl", GrfCmdBlk::m_RightCtrl);
    ScdPFUser.WrInt("Mouse", "RightShft", GrfCmdBlk::m_RightShft);
    ScdPFUser.WrInt("Mouse", "RightCtSh", GrfCmdBlk::m_RightCtSh);
    }
  }


void CMousePage::OnDefault1() 
  {//make the same changes in grfcmds.cpp!
  m_Left.SetCurSel      (MBA_QuickView);
  m_LeftDbl.SetCurSel   (MBA_GotoConnected);//MBA_AccessClosest);
  m_LeftCtrl.SetCurSel  (MBA_QuickViewPin);
  m_LeftShft.SetCurSel  (MBA_MdlActionsMenu);
  m_LeftCtSh.SetCurSel  (MBA_QuickViewMerge);
  m_Right.SetCurSel     (MBA_AccessClosest);
  m_RightDbl.SetCurSel  (MBA_None);
  m_RightCtrl.SetCurSel (MBA_WiringClosest);//MBA_ShowPressNets);
  m_RightShft.SetCurSel (MBA_AltAccessClosest);//MBA_ShowFlowNets);
  m_RightCtSh.SetCurSel (MBA_ClearMarks);
  UpdateData(false);
  }

void CMousePage::OnDefault2() 
  {
  OnDefault1();
  m_LeftShft.SetCurSel  (MBA_AltAccessClosest);
  m_Right.SetCurSel     (MBA_MdlActionsMenu);
  UpdateData(false);
  }

//--------------------------------------------------------------------------
//===========================================================================

IMPLEMENT_DYNCREATE(CColourPage, CCustomPropertyPage)

CColourPage::CColourPage() : CCustomPropertyPage(CColourPage::IDD)
  {
  //{{AFX_DATA_INIT(CColourPage)
	m_StatusOn = gs_pPrj->ShowStatus();
  m_ShowEditStatus = gs_pPrj->ShowEditStatus();
  m_dVapTest = gs_StatusVapTestFrac*100.0;
  m_dSolTest = gs_StatusSolTestFrac*100.0;
	//}}AFX_DATA_INIT
  }       

//--------------------------------------------------------------------------

CColourPage::~CColourPage()
  {
  XUpdateDisplays(UDO_ForceUpd|UDO_ColoursChgd);
  }

//--------------------------------------------------------------------------

void CColourPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CColourPage)
  DDX_Control(pDX, IDC_GRCOLOTHER, m_ButtonOther);
  DDX_Control(pDX, IDC_GRCOLHIGHLITE, m_ButtonHL);
  DDX_Check(pDX, IDC_DISPLAYSTATUSCOLOURS, m_StatusOn);
  DDX_Text(pDX, IDC_DISPLAYLINKVAPTEST, m_dVapTest);
  DDX_Text(pDX, IDC_DISPLAYLINKSOLTEST, m_dSolTest);
  //}}AFX_DATA_MAP
  for (int i=0; i<MaxMdlStatusTypes; i++)
    {
    DDX_Control(pDX, IDC_MSON0+i,  m_PenStyle[i]);
    DDX_Control(pDX, IDC_GRCOLSTATUS0+i, m_Button[i]);
    }
  DDX_Check(pDX, IDC_DISPLAYEDITCOLOURS, m_ShowEditStatus);
  m_dVapTest = Range(0.0, m_dVapTest, 100.0);
  m_dSolTest = Range(0.0, m_dSolTest, 100.0);
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CColourPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CColourPage)
	ON_BN_CLICKED(IDC_GRCOLHIGHLITE, OnGrcolhighlite)
	ON_BN_CLICKED(IDC_GRCOLOTHER, OnGrcolother)
	//}}AFX_MSG_MAP
  ON_CONTROL_RANGE(BN_CLICKED, IDC_GRCOLSTATUS0, IDC_GRCOLSTATUS0+MaxMdlStatusTypes, OnGrcolStatus)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CColourPage::OnInitDialog() 
  {
  CCustomPropertyPage::OnInitDialog();

  bool SolidOnly[MaxMdlStatusTypes]=
    {
    false, false, false, false, false, // 0..4
    false, false, false, false, true,  // 5..9
    true,  true,  false, false, false, // 10..14
    false, true,  true,  false, false, // 15..19
    false, false, true,  true,         // 20..23
    };
  for (int i=0; i<MaxMdlStatusTypes; i++)
    {
    m_PenStyle[i].ResetContent();
    m_PenStyle[i].AddString("Off");
    m_PenStyle[i].AddString("Solid");
    if (SolidOnly[i])
      continue;
    m_PenStyle[i].AddString("L-Dash");
    m_PenStyle[i].AddString("S-Dash");
    m_PenStyle[i].AddString("Dots");
    }

  m_PenStyle[16].SetCurSel(GrfHelper.iShowMdlStatusInActive);
  m_PenStyle[ 0].SetCurSel(GrfHelper.iShowMdlStatusError);
  m_PenStyle[ 1].SetCurSel(GrfHelper.iShowMdlStatusWarning);
  m_PenStyle[ 2].SetCurSel(GrfHelper.iShowMdlStatusTear);
  m_PenStyle[15].SetCurSel(GrfHelper.iShowMdlStatusMacMdl);
  m_PenStyle[ 3].SetCurSel(GrfHelper.iShowMdlStatusOff);
  m_PenStyle[ 4].SetCurSel(GrfHelper.iShowMdlStatusOn);
  m_PenStyle[ 5].SetCurSel(GrfHelper.iShowMdlStatusIsSrc);
  m_PenStyle[ 6].SetCurSel(GrfHelper.iShowMdlStatusIsSnk);
  m_PenStyle[ 7].SetCurSel(GrfHelper.iShowMdlStatusNettQmP);
  m_PenStyle[ 8].SetCurSel(GrfHelper.iShowMdlStatusNettQmN);
  m_PenStyle[ 9].SetCurSel(GrfHelper.iShowMdlStatusUFlw);
  m_PenStyle[10].SetCurSel(GrfHelper.iShowMdlStatusUNoFlw);
  m_PenStyle[11].SetCurSel(GrfHelper.iShowMdlStatusLFlwP);
  m_PenStyle[17].SetCurSel(GrfHelper.iShowMdlStatusLFlwN);
  m_PenStyle[12].SetCurSel(GrfHelper.iShowMdlStatusLNoFlw);
  m_PenStyle[13].SetCurSel(GrfHelper.iShowMdlStatusClear);
  m_PenStyle[14].SetCurSel(GrfHelper.iShowMdlStatusUnKnown);
  m_PenStyle[18].SetCurSel(GrfHelper.iShowMdlStatusXferLnk);
  m_PenStyle[19].SetCurSel(GrfHelper.iShowMdlStatusSimpleLnk);
  m_PenStyle[20].SetCurSel(GrfHelper.iShowMdlStatusLinearLnk);
  m_PenStyle[21].SetCurSel(GrfHelper.iShowMdlStatusFullLnk);
  m_PenStyle[22].SetCurSel(GrfHelper.iShowMdlStatusLFlwVap);
  m_PenStyle[23].SetCurSel(GrfHelper.iShowMdlStatusLFlwSol);

  if (DefNetProbalMode())
    GetDlgItem(IDC_DISPLAYEDITCOLOURS)->EnableWindow(false);
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//--------------------------------------------------------------------------

void CColourPage::OnOK() 
  {
  if (bDidInit)
    {
    UpdateData(True);
    gs_pPrj->SetShowStatus(m_StatusOn!=0);
    gs_pPrj->SetShowEditStatus(m_ShowEditStatus!=0);

    GrfHelper.iShowMdlStatusInActive  = m_PenStyle[ 16].GetCurSel();
    GrfHelper.iShowMdlStatusError     = m_PenStyle[  0].GetCurSel();
    GrfHelper.iShowMdlStatusWarning   = m_PenStyle[  1].GetCurSel();
    GrfHelper.iShowMdlStatusTear      = m_PenStyle[  2].GetCurSel();
    GrfHelper.iShowMdlStatusMacMdl    = m_PenStyle[ 15].GetCurSel();
    GrfHelper.iShowMdlStatusOff       = m_PenStyle[  3].GetCurSel();
    GrfHelper.iShowMdlStatusOn        = m_PenStyle[  4].GetCurSel();
    GrfHelper.iShowMdlStatusIsSrc     = m_PenStyle[  5].GetCurSel();
    GrfHelper.iShowMdlStatusIsSnk     = m_PenStyle[  6].GetCurSel();
    GrfHelper.iShowMdlStatusNettQmP   = m_PenStyle[  7].GetCurSel();
    GrfHelper.iShowMdlStatusNettQmN   = m_PenStyle[  8].GetCurSel();
    GrfHelper.iShowMdlStatusUFlw      = m_PenStyle[  9].GetCurSel();
    GrfHelper.iShowMdlStatusUNoFlw    = m_PenStyle[ 10].GetCurSel();
    GrfHelper.iShowMdlStatusLFlwP     = m_PenStyle[ 11].GetCurSel();
    GrfHelper.iShowMdlStatusLFlwN     = m_PenStyle[ 17].GetCurSel();
    GrfHelper.iShowMdlStatusLNoFlw    = m_PenStyle[ 12].GetCurSel();
    GrfHelper.iShowMdlStatusClear     = m_PenStyle[ 13].GetCurSel();
    GrfHelper.iShowMdlStatusUnKnown   = m_PenStyle[ 14].GetCurSel();
    GrfHelper.iShowMdlStatusXferLnk   = m_PenStyle[ 18].GetCurSel();
    GrfHelper.iShowMdlStatusSimpleLnk = m_PenStyle[ 19].GetCurSel();
    GrfHelper.iShowMdlStatusLinearLnk = m_PenStyle[ 20].GetCurSel();
    GrfHelper.iShowMdlStatusFullLnk   = m_PenStyle[ 21].GetCurSel();
    GrfHelper.iShowMdlStatusLFlwVap   = m_PenStyle[ 22].GetCurSel();
    GrfHelper.iShowMdlStatusLFlwSol   = m_PenStyle[ 23].GetCurSel();
    //ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_ACTIVATECMDS, 0); //after stopping ensure active/top window has commands enabled

    gs_StatusVapTestFrac = m_dVapTest/100.0;
    gs_StatusSolTestFrac = m_dSolTest/100.0;
    }
  }

//---------------------------------------------------------------------------

//void CColourPage::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct )
//  {
//if (MsgWndBusy())
//    {
//    pCmdUi->Enable(false);
//    return;
//    }
  //  //pCmdUi->Enable(!bAllGrfLoaded);
//  int xxx=0;
//  }

void CColourPage::OnGrcolStatus(UINT nID) 
  {
  const int N=nID - IDC_GRCOLSTATUS0;
  CColorDialog ColDlg(grl_get_win_color(GR_FIRSTSTATUSCOLOR+N), CC_RGBINIT | CC_SHOWHELP, this);
  ColDlg.m_cc.lpCustColors = gs_CustomColours;
  if (ColDlg.DoModal()==IDOK)
    {
    grl_set_win_color(GR_FIRSTSTATUSCOLOR+N, ColDlg.GetColor());
    GetDlgItem(IDC_GRCOLSTATUS0+N)->Invalidate();
    }
  }

//---------------------------------------------------------------------------

void CColourPage::OnGrcolhighlite() 
  {
	CColorDialog ColDlg(grl_get_win_color(GR_HIGHLIGHT), CC_RGBINIT | CC_SHOWHELP, this);
  ColDlg.m_cc.lpCustColors = gs_CustomColours;
  if (ColDlg.DoModal()==IDOK)
    {
    grl_set_win_color(GR_HIGHLIGHT, ColDlg.GetColor());
    GetDlgItem(IDC_GRCOLHIGHLITE)->Invalidate();
    }
  }

//--------------------------------------------------------------------------

void CColourPage::OnGrcolother() 
  {
  CColorDialog ColDlg(grl_get_win_color(GR_OTHERGRAPHICS), CC_RGBINIT | CC_SHOWHELP, this);
  ColDlg.m_cc.lpCustColors = gs_CustomColours;
  if (ColDlg.DoModal()==IDOK)
    {
    grl_set_win_color(GR_OTHERGRAPHICS, ColDlg.GetColor());
    GetDlgItem(IDC_GRCOLOTHER)->Invalidate();
    }
  }

//--------------------------------------------------------------------------
//===========================================================================

IMPLEMENT_DYNCREATE(CStatsPage, CCustomPropertyPage)

CStatsPage::CStatsPage() : CCustomPropertyPage(CStatsPage::IDD)
  {
  //{{AFX_DATA_INIT(CStatsPage)
  m_StatsFilename = _T("");
  m_StatsCnt = 0;
  //}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

CStatsPage::~CStatsPage()
  {
  }

//--------------------------------------------------------------------------

BOOL CStatsPage::OnInitDialog() 
  {
  CProfINIFile PF(PrjIniFile());
  m_StatsFilename = PF.RdStr("General", "HistoryStatsFilename", "c:\\hst_stat.csv");
  CCustomPropertyPage::OnInitDialog();
  UpdateDialogControls(this, FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
  }

//--------------------------------------------------------------------------

void CStatsPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CStatsPage)
  DDX_Text(pDX, IDC_HSTFILENAME, m_StatsFilename);
  DDX_Text(pDX, IDC_HSTITERCNT, m_StatsCnt);
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CStatsPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CStatsPage)
  ON_EN_CHANGE(IDC_REFRESHCNT, OnChangeRefreshcnt)
  ON_BN_CLICKED(IDC_HSTFETCH, OnHstfetch)
  ON_BN_CLICKED(IDC_HSTRESET, OnHstreset)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDC_HSTFETCH, OnUpdateBtn)
  ON_UPDATE_COMMAND_UI(IDC_HSTRESET, OnUpdateBtn)
  ON_MESSAGE(WMU_HSTUPDATESTATS, OnUpdateHstStats)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

LRESULT CStatsPage::OnUpdateHstStats(WPARAM wParam, LPARAM lParam)
  {
  UpdateData(TRUE);
  m_StatsCnt = lParam;
  UpdateData(FALSE);
  //RedrawWindow();
  return True;
  }

//--------------------------------------------------------------------------

void CStatsPage::OnChangeRefreshcnt() 
  {
  }

//--------------------------------------------------------------------------

void CStatsPage::OnOK() 
  {
  if (bDidInit)
    {
    UpdateData(TRUE);
    CProfINIFile PF(PrjIniFile());
    PF.WrStr("General", "HistoryStatsFilename", (char*)(const char*)m_StatsFilename);
    }
  }

//--------------------------------------------------------------------------

void CStatsPage::OnHstfetch() 
  {
  gs_HstMngr.DoFetchStats((char*)(const char*)m_StatsFilename);
  }

//--------------------------------------------------------------------------

void CStatsPage::OnHstreset() 
  {
  gs_HstMngr.DoResetStats();
  }

//---------------------------------------------------------------------------

void CStatsPage::OnUpdateBtn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(gs_pPrj && gs_HstMngr.IsOn());
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CSnapShotPage, CCustomPropertyPage)

CSnapShotPage::CSnapShotPage() : CCustomPropertyPage(CSnapShotPage::IDD)
  {
  //{{AFX_DATA_INIT(CSnapShotPage)
  m_Prompt = -1;
  m_FixedName = _T("");
  m_Prefix = _T("");
  m_Number = 0;
	m_DoZip = FALSE;
	//}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

CSnapShotPage::~CSnapShotPage()
  {
  }

//--------------------------------------------------------------------------

void CSnapShotPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSnapShotPage)
  DDX_Radio(pDX, IDC_PROMPT, m_Prompt);
  DDX_Text(pDX, IDC_FIXEDNAME, m_FixedName);
  DDX_Text(pDX, IDC_PREFIX, m_Prefix);
  DDX_Text(pDX, IDC_NUMBER, m_Number);
  DDV_MinMaxUInt(pDX, m_Number, 0, 999);
	DDX_Check(pDX, IDC_ZIPSS, m_DoZip);
	//}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSnapShotPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CSnapShotPage)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CSnapShotPage::OnInitDialog() 
  {
  m_Prompt = gs_pPrj->m_eSSAction;
  m_FixedName = gs_pPrj->m_sSSName();
  m_Prefix = gs_pPrj->m_sSSPrefix();
  m_Number = gs_pPrj->m_iSSNumber;
  m_DoZip = gs_pPrj->m_bSSDoZip;
  CCustomPropertyPage::OnInitDialog();
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//--------------------------------------------------------------------------

void CSnapShotPage::OnOK() 
  {
  if (bDidInit)
    {
    UpdateData(True);
    gs_pPrj->m_eSSAction = (SnapShotActions)m_Prompt;
    gs_pPrj->m_sSSName = (const char*)m_FixedName;
    gs_pPrj->m_sSSPrefix = (const char*)m_Prefix;
    gs_pPrj->m_iSSNumber = m_Number;
    gs_pPrj->m_bSSDoZip = m_DoZip;
    }
  }

//===========================================================================
#if WITHSCRCYCLES
IMPLEMENT_DYNCREATE(CScreenSavePage, CCustomPropertyPage)

CScreenSavePage::CScreenSavePage() : CCustomPropertyPage(CScreenSavePage::IDD)
  {
  //{{AFX_DATA_INIT(CScreenSavePage)
  m_ScrCycles = 0;
  m_SyscadScrSave = FALSE;
  m_SystemScrSave = FALSE;
  m_Description = _T("");
  m_WaitTime = 0;
  //}}AFX_DATA_INIT
  Cycles = NULL;
  }

//---------------------------------------------------------------------------

CScreenSavePage::~CScreenSavePage()
  {
  if (Cycles)
    delete []Cycles;
  }

//---------------------------------------------------------------------------

void CScreenSavePage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CScreenSavePage)
  DDX_Control(pDX, IDC_WINDOWLIST, m_WindowList);
  DDX_Control(pDX, IDC_CYLELIST, m_CycleList);
  DDX_Text(pDX, IDC_SCRCYCLES, m_ScrCycles);
  DDX_Check(pDX, IDC_SYSCADSCRSAVE, m_SyscadScrSave);
  DDX_Check(pDX, IDC_SYSTEMSCRSAVE, m_SystemScrSave);
  DDX_Text(pDX, IDC_DESCRIPTION, m_Description);
  DDX_Text(pDX, IDC_WAITTIME, m_WaitTime);
  //}}AFX_DATA_MAP
  GetDlgItem(IDC_SYSCADSCRSAVE)->EnableWindow(m_SystemScrSave);
  GetDlgItem(IDC_SCRCYCLES)->EnableWindow(m_SyscadScrSave);
  GetDlgItem(IDC_DESCRIPTION)->EnableWindow(m_SyscadScrSave);
  GetDlgItem(IDC_WAITTIME)->EnableWindow(m_SyscadScrSave);
  GetDlgItem(IDC_WINDOWLIST)->EnableWindow(m_SyscadScrSave);
  GetDlgItem(IDC_CYLELIST)->EnableWindow(m_SyscadScrSave);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CScreenSavePage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CScreenSavePage)
  ON_BN_CLICKED(IDC_SYSTEMSCRSAVE, OnScrSaveCheck)
  ON_BN_CLICKED(IDC_SYSCADSCRSAVE, OnScrSaveCheck)
  ON_CBN_SELCHANGE(IDC_CYLELIST, OnSelchangeCylelist)
  ON_EN_CHANGE(IDC_SCRCYCLES, OnChangeScrcycles)
  ON_LBN_SELCHANGE(IDC_WINDOWLIST, OnSelchangeWindowlist)
  ON_LBN_DBLCLK(IDC_WINDOWLIST, OnDblclkWindowlist)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CScreenSavePage::OnInitDialog() 
  {
  m_SystemScrSave = !gs_pPrj->bSystemScreenSave;
  m_SyscadScrSave = gs_pPrj->bSyscadScreenSave;
  m_ScrCycles = gs_pPrj->iScrCycles;
  AllocLen = Max(1, m_ScrCycles);
  ASSERT(Cycles==NULL);
  Cycles = new CScreenSaverInfo[m_ScrCycles];
  for (int i=0; i<m_ScrCycles; i++)
    Cycles[i] = gs_pPrj->Cycles[i];
  m_ScrCycles = AllocLen;
  CurSelNo = -1;
  CCustomPropertyPage::OnInitDialog();

  Windows.RemoveAll();
  VERIFY(m_WindowList.SubclassMoveUpDownControls(this, IDC_WINDOWLIST, IDC_WNDUP, IDC_WNDDWN));
  CWindowLists WL;
  const int Count = WL.BuildSingleList();
  for (i=0; i<Count; i++)
    {
  #if WITHSFEWND 
    bool Fnd=(WL.Wnds[i].pWnd==WL.pMainWnd || WL.Wnds[i].pWnd==WL.pSFEWnd);
  #else
    bool Fnd=(WL.Wnds[i].pWnd==WL.pMainWnd);
  #endif
    for (int a=0; a<NAccessWnds; a++)
      if (WL.Wnds[i].pWnd==WL.pAccessWnd[a])
        Fnd=true;
    if (!Fnd)
      Windows.Add((const char*)(WL.Wnds[i].sName));
    }
  OnChangeScrcycles();
  return TRUE;  // return TRUE unless you set the focus to a control
  }

//---------------------------------------------------------------------------

void CScreenSavePage::OnOK() 
  {
  if (bDidInit)
    {
    UpdateData(True);
    gs_pPrj->bSystemScreenSave = !m_SystemScrSave;
    gs_pPrj->bSyscadScreenSave = m_SyscadScrSave;
    gs_pPrj->iScrCycles = m_ScrCycles;
    int i = m_CycleList.GetCurSel();
    if (i>=0 && CurSelNo>=0 && CurSelNo<m_ScrCycles)
      {
      Cycles[CurSelNo].iWaitTime = m_WaitTime;
      Cycles[CurSelNo].sDescription = m_Description;
      Cycles[CurSelNo].Windows.RemoveAll();
      CString Txt;
      for (int j=0; j<m_WindowList.GetCount(); j++)
        {
        m_WindowList.GetText(j, Txt);
        if (Txt.Left(3)==" on")
          {
          Txt = Txt.Right(Txt.GetLength() - 4);
          Cycles[CurSelNo].Windows.SetSize(Cycles[CurSelNo].Windows.GetSize()+1);
          Cycles[CurSelNo].Windows.SetAt(Cycles[CurSelNo].Windows.GetSize()-1, Txt);
          }
        }  
      }
    delete [](gs_pPrj->Cycles);
    gs_pPrj->Cycles = Cycles;
    Cycles = NULL;
    OnInitDialog();
    }
  }

//---------------------------------------------------------------------------

void CScreenSavePage::OnScrSaveCheck() 
  {
  UpdateData(True);
  UpdateData(False);
  Invalidate();
  }

//---------------------------------------------------------------------------

void CScreenSavePage::OnChangeScrcycles() 
  {//changed the number of cycle screens...
  UpdateData(True);
  m_ScrCycles = Max(1, m_ScrCycles);
  char Buff[256];
  m_CycleList.ResetContent();
  for (int i=0; i<m_ScrCycles; i++)
    {
    sprintf(Buff, "Cycle %i", i+1);
    m_CycleList.AddString(Buff);
    }
  CScreenSaverInfo* NewCycles = new CScreenSaverInfo[m_ScrCycles];
  for (i=0; i<AllocLen && i<m_ScrCycles; i++)
    NewCycles[i] = Cycles[i];
  //for (i=AllocLen; i<m_ScrCycles; i++)
  //  NewCycles[i].iWaitTime = 10;
  delete []Cycles;
  Cycles = NewCycles;
  AllocLen = m_ScrCycles;
  m_CycleList.SetCurSel(0);
  UpdateData(False);
  OnSelchangeCylelist();
  }

//---------------------------------------------------------------------------

void CScreenSavePage::OnSelchangeCylelist() 
  {
  UpdateData(True);
  int i = m_CycleList.GetCurSel();
  if (i<0)
    m_CycleList.SetCurSel(0);
  if (CurSelNo>=0 && CurSelNo<m_ScrCycles)
    {
    Cycles[CurSelNo].iWaitTime = m_WaitTime;
    Cycles[CurSelNo].sDescription = m_Description;
    Cycles[CurSelNo].Windows.RemoveAll();
    CString Txt;
    for (int j=0; j<m_WindowList.GetCount(); j++)
      {
      m_WindowList.GetText(j, Txt);
      if (Txt.Left(3)==" on")
        {
        Txt = Txt.Right(Txt.GetLength() - 4);
        Cycles[CurSelNo].Windows.SetSize(Cycles[CurSelNo].Windows.GetSize()+1);
        Cycles[CurSelNo].Windows.SetAt(Cycles[CurSelNo].Windows.GetSize()-1, Txt);
        }
      }  
    }
  m_WaitTime = Cycles[i].iWaitTime;
  m_Description = Cycles[i].sDescription;
  char Buff[256];
  m_WindowList.ResetContent();
/*cycle through the windows already selecetd in the cycles window and add these, 
in order, to the Cycling list, so that these windows are then cycled in the 
correct order*/ 
  for (int k=0; k < Cycles[i].Windows.GetSize(); k++)
    {
    sprintf(Buff, " on %s", (const char*)(Cycles[i].Windows.GetAt(k)));
    m_WindowList.AddString(Buff);
    }

  for (int j=0; j<Windows.GetSize(); j++)
    {
    if ( Cycles[i].FindWindow(Windows[j]) < 0 )
      {// if window is Null then turn off
      sprintf(Buff, "off %s", (const char*)(Windows.GetAt(j)));
      m_WindowList.AddString(Buff);
      }
    }
  m_WindowList.SetCurSel(0);
  //m_WindowList.Invalidate();
  CurSelNo = i;

  UpdateData(False);
  }

//---------------------------------------------------------------------------

void CScreenSavePage::OnSelchangeWindowlist() 
  {
  }

//---------------------------------------------------------------------------

void CScreenSavePage::OnDblclkWindowlist() 
  {
  int i = m_WindowList.GetCurSel();
  if (i>=0)
    {
    CString Txt;
    CString Left;
    m_WindowList.GetText(i,	Txt);
    m_WindowList.DeleteString(i);
    char Buff[256];
    Left = Txt.Left(3);
    Txt = Txt.Right(Txt.GetLength()-4);
    if (Left==" on")
      sprintf(Buff, "off %s", (const char*)Txt);
    else
      sprintf(Buff, " on %s", (const char*)Txt);
    if (m_WindowList.GetCount()==i)
      m_WindowList.InsertString(-1, Buff);
    else
      m_WindowList.InsertString(i, Buff);
    m_WindowList.SetCurSel(i);
    }
  }
#endif //end WITHSCRCYCLES
//===========================================================================

IMPLEMENT_DYNCREATE(CDebugPage, CCustomPropertyPage)

CDebugPage::CDebugPage() : CCustomPropertyPage(CDebugPage::IDD)
  {
  bChanged = 0; //reset flag which determines if dlg box has been specifically opened
  //{{AFX_DATA_INIT(CDebugPage)
  m_bDbgOn = FALSE;
  m_dbgValueValVar = 0;
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CDebugPage::~CDebugPage()
  {
  }

//---------------------------------------------------------------------------

void CDebugPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDebugPage)
  DDX_Control(pDX, IDC_DBGVALUE, m_dbgValueCntrlVar);
  DDX_Control(pDX, IDC_DBGENSEXNS, m_genSexnCntrlVar);
  DDX_Control(pDX, IDC_DBGSUBSEXNS, m_subSexnCntrlVar);
  DDX_Check(pDX, IDC_DBGON, m_bDbgOn);
  DDX_Text(pDX, IDC_DBGVALUE, m_dbgValueValVar);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDebugPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CDebugPage)
  ON_BN_CLICKED(IDC_DBGDELETE, OnDbgdelete)
  ON_CBN_SELCHANGE(IDC_DBGENSEXNS, OnSelChangeGenSexns)
  ON_LBN_SELCHANGE(IDC_DBGSUBSEXNS, OnSelChangeSubSexns)
  ON_BN_CLICKED(IDC_DBGON, OnDbgOn)
  ON_EN_UPDATE(IDC_DBGVALUE, OnUpdateValue)
  ON_EN_KILLFOCUS(IDC_DBGVALUE, OnKillfocusValue)
  ON_EN_SETFOCUS(IDC_DBGVALUE, OnSetfocusValue)
  ON_BN_CLICKED(IDC_RESET_VAL, OnResetVal)
  ON_LBN_DBLCLK(IDC_DBGSUBSEXNS, OnDblclkSubsexns)
  ON_CBN_KILLFOCUS(IDC_DBGENSEXNS, OnKillfocusGensexns)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CDebugPage::OnInitDialog() 
  {//1.) determine path to SCD_Dbg.Txt file
   //2.) Cycle through the CDbgMngrs 
   //       a.)  Fill the "General Section Combo Box" with [SECTION] names
   //       b.)  Read all values from the existing ? SysCAD.DMF file
   // Display "SubSection List Box" with appropriate names from GenSexn selection
   // Display "SubSection Value Edit Box" with appropriate value from selected SubSexn
    
  bChanged = 0;
  DbgFn = DBGFILESPATH;
  //if (DbgFn.Len()>0 && DbgFn[DbgFn.Len()-1] != '\\')
  //  DbgFn += "\\";
#if BLDDEPENDENTFILES
  DbgFn.Set("%sScd_Dbg.%i.Txt", DbgFn(), SCD_BUILD_NUM);
#else
  DbgFn += "Scd_Dbg.Txt";
#endif
  m_bDbgOn = dbgtestfileopen();

  CCustomPropertyPage::OnInitDialog();
  #if defined(_DEBUG)
  SetDlgItemText(IDC_TXTVERSION, "Version:DEBUG");
  #elif defined(_RELEASE)
  SetDlgItemText(IDC_TXTVERSION, "Version:RELEASE");
  #else
  SetDlgItemText(IDC_TXTVERSION, "Version:RELEASE_DBG");
  #endif
  Strng s;
  #if HEAP_SPARES
  s += "Uses SPARES    ";
  #endif
  SetDlgItemText(IDC_TXTOTHER, s());
 
  m_genSexnCntrlVar.ResetContent();
#if DBGMANAGER
  if (CGenSexn::pFirstGen)// if a list exists 
    {
    pCGenSexn getGenSexnNames = CGenSexn::pFirstGen;
    while (getGenSexnNames) // cycle through list until getGenSexnNames is NULL
      {
      pCSubSexn getSubSexns = getGenSexnNames->pSubSexns;
      pCGenSexn parentGenSexn = getGenSexnNames;
      // cycle through related sub sexn names and read the values from the file
      if (getSubSexns)// if a list of sub sexns exists 
        {
        while (getSubSexns)
          {// cycle through list of sub sexns assoc with curr gen sexn
          parentGenSexn->ReadFromFile(getSubSexns);// read values from the file
          getSubSexns = getSubSexns->pNxt;
          }
        }
      m_genSexnCntrlVar.AddString( getGenSexnNames->genStrng() );
      getGenSexnNames = getGenSexnNames->pNxt;
      }
    }
#endif
  int Sel = 0;
  if (gs_pPrj && gs_pPrj->pPrjDoc)
    {
    CProfINIFile PF(PrjIniFile());
    Sel = Range(0, PF.RdInt("General", "PrevDebugSelection", 0), m_genSexnCntrlVar.GetCount());
    }
  m_genSexnCntrlVar.SetCurSel(Sel);
  OnSelChangeGenSexns(); // fill subsection list box on startup
  //OnSelChangeSubSexns(); // add value of selected sub section on start up
  UpdateData(FALSE);

  m_subSexnCntrlVar.SetCurSel(-1);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------
/*#F:This is a helper function, used to retrieve the currentrly selected SubSexn name*/
void CDebugPage::GetSubSexnName(CString &ListBoxEntry)
  {
  int index = m_subSexnCntrlVar.GetCurSel(); 
  //zero-based index of current selection
  m_subSexnCntrlVar.GetText(index, ListBoxEntry);
  //retrieve the List Box entry at the index value
  ListBoxEntry = ListBoxEntry.Mid(ListBoxEntry.Find(' ') + 1);
  // CString containing the remainder of the string after the space
  }

//---------------------------------------------------------------------------
/*#f: This fills the SubSexn list box with the sub sexns corresponding to the selected Gen sexn */
void CDebugPage::OnSelChangeGenSexns() 
  {
  m_subSexnCntrlVar.ResetContent();
  //set the value in Value wnd to "      " to avoid confusion of values
  m_dbgValueCntrlVar.SetWindowText("     ");

  int i = m_genSexnCntrlVar.GetCurSel();
#if DBGMANAGER
  if (i>=0) //-1 if nothing
    {
    pCGenSexn pgen = CGenSexn::GetByIndex(i);
    if (pgen)
      {
      pCSubSexn psub = pgen->pSubSexns;
      while (psub)
        { //display the folowing entry "Value SubSexnName" in listbox
        Strng subSexnValue;
        subSexnValue.Set("%ld", psub->fValue);
        Strng subSexnListBoxEntry;
        subSexnListBoxEntry = subSexnValue();
        subSexnListBoxEntry += " ";
        subSexnListBoxEntry += psub->subStrng();
        m_subSexnCntrlVar.AddString(subSexnListBoxEntry());
        psub = psub->pNxt;
        }
      m_subSexnCntrlVar.SetCurSel(0);
      }
    }
#endif
  OnSelChangeSubSexns();// goto value of selected sub section
  }

//--------------------------------------------------------------------------
/*#f:This saves the current index for retrieval by DblclkSubSexn*/
void CDebugPage::OnKillfocusGensexns() 
  {
  m_currGenSexnIndex = m_genSexnCntrlVar.GetCurSel(); 
  }

//--------------------------------------------------------------------------
/*#f: This places the value corresponding to the selected sub sexn in the Sub Sexn Value edit box */
void CDebugPage::OnSelChangeSubSexns() 
  {// place reqd dbgvalue into edit box
  // Value is about to be altered - clear the current value in edit box
  m_dbgValueCntrlVar.Clear();
  m_dbgValueCntrlVar.EnableWindow(1);// enable window
#if DBGMANAGER
  int index = m_genSexnCntrlVar.GetCurSel();
  //the current index value, is used to get the currently selected CGenSexn
  pCGenSexn genSexnNow = CGenSexn::GetByIndex(index);
  pCSubSexn getSubSexns = genSexnNow->pSubSexns;
  if (getSubSexns)// if a list of sub sexns exists 
    {
    CString nameInBoxNow;
    GetSubSexnName(nameInBoxNow);
    flag matchedSel = 0;

    while (getSubSexns)
      {// cycle through list 
      if (getSubSexns->subStrng.XStrICmp(nameInBoxNow) == 0)
        {//the current selection matches the name in the list box - EXIT
        matchedSel = 1; 
        break;
        }
      getSubSexns = getSubSexns->pNxt;
      }
    if (matchedSel)
      {// get values from reqd CSubSexn
      // convert fValue to null terminated string
      Strng newValue;
      newValue.Set("%ld", getSubSexns->fValue);
      // change the ENTIRE value in the edit control window
      m_dbgValueCntrlVar.SetWindowText(newValue());
      // change focus to dbg Value Edit Control
      m_dbgValueCntrlVar.SetFocus();
      //m_dbgValueCntrlVar.ReplaceSel( newValue() );
      }
      // else do nothing - it should never happen!!
    }
#endif
  }

//--------------------------------------------------------------------------
/*#f: If the value associated with the sub section is zero, 
then its value becomes one, else if the value is non-zero, 
then it becomes one*/
void CDebugPage::OnDblclkSubsexns() 
  {
  bChanged = 1; 
  // retrieve the most currently selected index of Gen sexn
  m_currGenSexnIndex = m_genSexnCntrlVar.GetCurSel(); 
#if DBGMANAGER
  pCGenSexn genSexnNow = CGenSexn::GetByIndex(m_currGenSexnIndex);
  pCSubSexn subSexnNow = genSexnNow->pSubSexns;
  // allows, unrelated, direct access to the subSexns in the list without actually affecting the list
  int originalValue = 0;
  if (subSexnNow)// if a list of sub sexns exists 
    {                                          
    flag matchedSel = 0;
    while (subSexnNow)
      {// cycle through list using last selected subsexn name
      if (subSexnNow->subStrng.XStrICmp(m_currSubSexnName) == 0)
        {//the current selection matches the name in the list box - EXIT
        matchedSel = 1; 
        break;
        }
      subSexnNow = subSexnNow->pNxt;
      }
    if ( matchedSel ) 
      {//the current selection has been found - toggle fValue accordingly
      originalValue = subSexnNow->fValue;
      if (subSexnNow->fValue != 0)
        subSexnNow->fValue = 0;
      else
        subSexnNow->fValue = 1;
      // find string in sub sexn list box
      Strng newValue;
      newValue.Set("%ld", originalValue);
      Strng originalSubSexnListBoxEntry;
      originalSubSexnListBoxEntry = newValue();
      originalSubSexnListBoxEntry += " ";
      originalSubSexnListBoxEntry += subSexnNow->subStrng();
  
      int index = m_subSexnCntrlVar.FindStringExact(-1, originalSubSexnListBoxEntry());
      if (index != LB_ERR)
        {
        //update the value in the Value edit box
        Strng newValue;
        newValue.Set("%ld",subSexnNow->fValue);
        m_dbgValueCntrlVar.SetWindowText(newValue());

        // Update the Sub sexn list box entry & keep focus in list box
        m_subSexnCntrlVar.DeleteString(index);
        Strng newSubSexnListBoxEntry;
        newSubSexnListBoxEntry = newValue();
        newSubSexnListBoxEntry += " ";
        newSubSexnListBoxEntry += subSexnNow->subStrng();
        m_subSexnCntrlVar.InsertString(index, newSubSexnListBoxEntry());
        m_subSexnCntrlVar.SetFocus();
        m_subSexnCntrlVar.SetCurSel(index);
        }
      }
    }
#endif
  }
  
//--------------------------------------------------------------------------
/*#f: If nothing is selected in the Subsexn list box window then disable
 the Value edit control window. also determine the currently selected SubSexn name
 while it is still known ( this cannot be done effectively in KillFocus since if 
 one presses Enter then the selection is removed before it can be used to establish
 the name*/
void CDebugPage::OnSetfocusValue() 
  {
  if (m_subSexnCntrlVar.GetCurSel() == LB_ERR)// no item selected
    m_dbgValueCntrlVar.EnableWindow(0);// disable window
  else
    {
    m_dbgValueCntrlVar.EnableWindow(1);// enable window
    GetSubSexnName(m_currSubSexnName);
    }
  }

//--------------------------------------------------------------------------
/*#f: This clears the sub section value edit box bvefore a new value is inserted */	
void CDebugPage::OnUpdateValue() 
  {
  // Value is about to be altered - clear the current value
  m_dbgValueCntrlVar.Clear();
  }

//--------------------------------------------------------------------------
/*#f: This saves the new value in the respective sub sexn and updates the respective line in tyhe sub section list box */
void CDebugPage::OnKillfocusValue()
  {// save the value in the respective CSubSexns fValue
#if DBGMANAGER
  // locate current Gen sexn and subsexn
  int index = m_genSexnCntrlVar.GetCurSel();
  if (index>=0 && index<m_subSexnCntrlVar.GetCount())
    { // do - only if something is currently selected in the list box
    //the current index value, is used to get the currently selected CGenSexn
    pCGenSexn genSexnNow = CGenSexn::GetByIndex(index);
    pCSubSexn subSexnNow = genSexnNow->pSubSexns;
    // allows, unrelated, direct access to the subSexns in the list without actually affecting the list
    // this is NOT the case when using  "genSexnNow->pSubSexns" as this causes the 
    //"first" CSubSexn in the list to pointed to the nxt and nxt CSubsen as it cycled through
    //the list Thus "losing" the CSubSexns from the list 

    if (subSexnNow)// if a list of sub sexns exists 
      {                                          
      int indexsub = m_subSexnCntrlVar.GetCurSel();
      flag matchedSel = 0;
      while (subSexnNow && indexsub>=0)
        {// cycle through list 
        if (subSexnNow->subStrng.XStrICmp(m_currSubSexnName) == 0)
          {//the current selection matches the name in the list box - EXIT
          matchedSel = 1; 
          break;
          }
        subSexnNow = subSexnNow->pNxt;
        }
      if (matchedSel) 
        {// the current selections have been found - update fValue
        char buff[256];
        int end = m_dbgValueCntrlVar.GetWindowText(buff, 256);
        buff[end] = '\0';// null terminate only the characters that are copied
        long newValue = atol(buff);// returns 0 if this cannot be done
        bChanged = bChanged || ((long)subSexnNow->fValue != newValue);
        subSexnNow->fValue = newValue;

        // now update the respective string in the Sub sexn list box
        int index =  m_subSexnCntrlVar.GetCurSel();
        m_subSexnCntrlVar.DeleteString(index);
    
        Strng newSubSexnValue;
        newSubSexnValue.Set("%ld", newValue);
        Strng newSubSexnListBoxEntry;
        newSubSexnListBoxEntry = newSubSexnValue();
        newSubSexnListBoxEntry += " ";
        newSubSexnListBoxEntry += subSexnNow->subStrng();
    
        m_subSexnCntrlVar.InsertString(index, newSubSexnListBoxEntry());
        }
      }
    }
#endif
  }

//--------------------------------------------------------------------------
/*#F: This cycles through all the sub sections and set all their fValues to 0
and then writes them to the SysCAD.dmf file. Afterwards the Dialog controls 
are suitably updated*/
void CDebugPage::OnResetVal() 
  {
  bChanged = 1; 
#if DBGMANAGER
  if (CGenSexn::pFirstGen)// if a list exists 
    {
    pCGenSexn getGenSexnNames = CGenSexn::pFirstGen;
    while (getGenSexnNames) // cycle through list until getGenSexnNames is NULL
      {
      pCGenSexn parentGenSexn = getGenSexnNames;
      //used to be able to retrieve the current GenSexnName without affecting getGenSexnNames
      pCSubSexn getSubSexns = getGenSexnNames->pSubSexns;
      //used to cycle thro the CSubSexn names without directly manipulating 
      //the list & thus "losing" list elements
 
      // cycle through related sub sexn names, set fValue to 0 & write values to file
      if (getSubSexns)// if a list of sub sexns exists 
        {
        while (getSubSexns)
          {// cycle through list of sub sexns assoc with curr gen sexn
          getSubSexns->fValue = 0;
          parentGenSexn->WriteToFile(getSubSexns);// write values to file
          getSubSexns = getSubSexns->pNxt;
          }
        }
      getGenSexnNames = getGenSexnNames->pNxt;
      }
    }
#endif
  OnSelChangeGenSexns();//fill subsection list box with valid strings
  OnSelChangeSubSexns();// add value of selected sub section to Value edit box
  }

//--------------------------------------------------------------------------
/*#f: This toggles the value of the DbgOn value when ever the button is clicked */
void CDebugPage::OnDbgOn() 
  { // button has been clicked, thus flag value changed to opposite value
  bChanged = 1; 
  m_bDbgOn = !m_bDbgOn;
  }

//---------------------------------------------------------------------------
/*F:This deletes the debug file, SysCAD.DBG*/
void CDebugPage::OnDbgdelete() 
  {
  bChanged = 1; 
  dbgfiletrunc();
  }

//---------------------------------------------------------------------------
/*#F:If the dialog box was initialized, Then #n
a.) Save the current Value in the currently selected Sub Sexn.#n      
b.) Save all the GenSexns and SubSexns and respective values to SysCAD.DMF file.#n      
c.) If debugging facility on then open the debug file, otherwise close it.*/

void CDebugPage::OnOK() 
  {                  
  if (bDidInit)
    {
    if (bChanged)
      {
      OnKillfocusValue(); //save value to sub section
      UpdateData(True);
      // Cycle through list and save values to DMF file 
      // - either create new file or overwrite old file
#if DBGMANAGER
      if(CGenSexn::pFirstGen)// if a list exists 
        {
        pCGenSexn getGenSexnNames = CGenSexn::pFirstGen;
        while (getGenSexnNames) // cycle through list until getGenSexnNames is NULL
          {
          pCGenSexn parentGenSexn = getGenSexnNames;
          //used to be able to retrieve the current GenSexnName without affecting getGenSexnNames
          pCSubSexn getSubSexns = getGenSexnNames->pSubSexns;
          //used to cycle thro the CSubSexn names without directly manipulating 
          //the list & thus "losing" list elements
   
          // cycle through related sub sexn names and read the values from the file
          if (getSubSexns)// if a list of sub sexns exists 
            {
            while (getSubSexns)
              {// cycle through list of sub sexns assoc with curr gen sexn
              parentGenSexn->WriteToFile(getSubSexns);// write values to file
              getSubSexns = getSubSexns->pNxt;
              }
            }
          getGenSexnNames = getGenSexnNames->pNxt;
          }
        }
#endif
      if (m_bDbgOn)
        {
        if (!dbgtestfileopen())
          dbgfileopen(DbgFn());
        }
      else
        {
        if (dbgtestfileopen())
          dbgfileclose();
        }
      }
    if (gs_pPrj && gs_pPrj->pPrjDoc)
      {
      CProfINIFile PF(PrjIniFile());
      PF.WrInt("General", "PrevDebugSelection", m_genSexnCntrlVar.GetCurSel());
      }
    CDbgMngr::LoadAll();
    }
  }

//--------------------------------------------------------------------------
//===========================================================================

class CReportOptionsDlg : public CDialog
  {
  public:
    CReportOptionsDlg(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CReportOptionsDlg)
    enum { IDD = IDD_REPORTOPTIONS };
    BOOL m_ExcelUseCellName;
    BOOL m_ExcelLock;
    BOOL m_ExcelMakeActive;
    BOOL m_ExcelUpdateLinks;
    BOOL m_ExcelSysCADActive;
    BOOL m_ExcelSaveOnComplete;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CReportOptionsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CReportOptionsDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

CReportOptionsDlg::CReportOptionsDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CReportOptionsDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CReportOptionsDlg)
  m_ExcelUseCellName = FALSE;
	m_ExcelLock = FALSE;
	m_ExcelMakeActive = FALSE;
  m_ExcelUpdateLinks = FALSE;
	m_ExcelSysCADActive = FALSE;
	m_ExcelSaveOnComplete = FALSE;
	//}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

void CReportOptionsDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CReportOptionsDlg)
  DDX_Check(pDX, IDC_EXCELCELLNAME, m_ExcelUseCellName);
	DDX_Check(pDX, IDC_EXCELLOCK, m_ExcelLock);
	DDX_Check(pDX, IDC_EXCELMAKEACTIVE, m_ExcelMakeActive);
	DDX_Check(pDX, IDC_EXCELUPDATELINKS, m_ExcelUpdateLinks);
	DDX_Check(pDX, IDC_EXCELSYSCADACTIVE, m_ExcelSysCADActive);
	DDX_Check(pDX, IDC_EXCELSAVEONCOMPLETE, m_ExcelSaveOnComplete);
	//}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CReportOptionsDlg, CDialog)
  //{{AFX_MSG_MAP(CReportOptionsDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------
//===========================================================================

const char* RepKey_GetValues = "Get Tag Values";
const char* RepKey_SetValues = "Set Tag Values";
const char* RepKey_Trend = "Trend";
const char* RepKey_Macro = "Macro";
const char* RepKey_GenInfo = "General Information";

class CUpdateReportDlg : public CDialog
  {
  public:
    CUpdateReportDlg(int RepList, CWnd* pParent = NULL);
    //{{AFX_DATA(CUpdateReportDlg)
    enum { IDD = IDD_REPORTUPDATEDLG };
    CComboBox m_Type;
    CString m_Filename;
    CString m_Name;
    //}}AFX_DATA
    CString sType;
    //{{AFX_VIRTUAL(CUpdateReportDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    int iRepList;
    //{{AFX_MSG(CUpdateReportDlg)
    afx_msg void OnBrowse();
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//--------------------------------------------------------------------------

CUpdateReportDlg::CUpdateReportDlg(int RepList, CWnd* pParent /*=NULL*/)
  : CDialog(CUpdateReportDlg::IDD, pParent)
  {
  iRepList = RepList;
  //{{AFX_DATA_INIT(CUpdateReportDlg)
  m_Filename = _T("");
  m_Name = _T("");
  //}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

void CUpdateReportDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CUpdateReportDlg)
  DDX_Control(pDX, IDC_TYPE, m_Type);
  DDX_Text(pDX, IDC_FILENAME, m_Filename);
  DDX_Text(pDX, IDC_NAME, m_Name);
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CUpdateReportDlg, CDialog)
  //{{AFX_MSG_MAP(CUpdateReportDlg)
  ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CUpdateReportDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  switch (iRepList)
    {
    case 0:
      m_Type.AddString(RepKey_GetValues);
      m_Type.AddString(RepKey_Trend);
      m_Type.AddString(RepKey_GenInfo);
      if (m_Type.SelectString(-1, sType)==CB_ERR)
        m_Type.SelectString(-1, RepKey_GetValues);
      SetWindowText("Report");
      break;
    case 1:
      m_Type.AddString(RepKey_SetValues);
      if (m_Type.SelectString(-1, sType)==CB_ERR)
        m_Type.SelectString(-1, RepKey_SetValues);
      SetWindowText("SetTags");
      break;
    case 2:
      m_Type.AddString(RepKey_Macro);
      if (m_Type.SelectString(-1, sType)==CB_ERR)
        m_Type.SelectString(-1, RepKey_Macro);
      SetWindowText("Macro");
      break;
    }
  return TRUE;
  }

//--------------------------------------------------------------------------

void CUpdateReportDlg::OnBrowse() 
  {
  UpdateData(TRUE);
  const char* pFName = NULL;
  CString FName;
  CString FPath = PrjFiles();
  if (m_Filename.GetLength()>0)
    {
    char Drv[_MAX_DRIVE];
    char Dir[_MAX_DIR];
    char Name[_MAX_FNAME];
    char Ext[_MAX_EXT];
    _splitpath((const char*)m_Filename, Drv, Dir, Name, Ext);
    FPath = Drv;
    FPath += Dir;
    FName = Name;
    FName += Ext;
    pFName = (const char*)FName;
    }
  CSCDFileDialog Dlg(FALSE, NULL, pFName, OFN_NOCHANGEDIR | OFN_HIDEREADONLY, "Excel Worksheet (*.xls)|*.xls|All files (*.*)|*.*||", this);
  Dlg.m_ofn.lpstrInitialDir = (const char*)FPath;
  Dlg.m_ofn.lpstrTitle = "Browse";
  if (Dlg.DoModal()==IDOK)
    m_Filename = Dlg.GetPathName();

  /*if (SymbolicPaths())
    {
    Strng Tmp((char*)(const char*)m_Filename); 
    Tmp.FnContract();
    m_Filename=Tmp();  
    }*/
  
  UpdateData(FALSE);
  }

//--------------------------------------------------------------------------

void CUpdateReportDlg::OnOK() 
  {
  UpdateData(TRUE);
  m_Type.GetWindowText(sType);
  CDialog::OnOK();
  }

//===========================================================================

CReportsDlg::CReportsDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CReportsDlg::IDD, pParent)
  {
  iPrevTab = -1;
  pList = NULL;
  //{{AFX_DATA_INIT(CReportsDlg)
  //}}AFX_DATA_INIT
  Create(CReportsDlg::IDD, pParent); //create modeless dialog
  }

//--------------------------------------------------------------------------

CReportsDlg::~CReportsDlg()
  {
  MainWnd()->m_pReportsDlg = NULL; //set global pointer to null
  }

//--------------------------------------------------------------------------

void CReportsDlg::BuildList(CRPFLIArray& List, int& RepSelected, int& SetSelected, int& MacroSelected)
  {//NOTE: Remember to delete all the individual items in list!!!

  //load report list and selections from ini file...
  RepSelected = -1;
  SetSelected = -1;
  MacroSelected = -1;
  CProfINIFile PF(PrjIniFile());
  for (int i=0; ; i++)
    {
    Strng RSect, TSect, FSect, SSect;//, s;
    RSect.Set("Report_Name%i", i);
    TSect.Set("Report_Type%i", i);
    FSect.Set("Report_FileName%i", i);
    SSect.Set("Report_Selected%i", i);
    Strng RepName = PF.RdStr("Reports", RSect(), "");
    if (RepName.Length()>0)
      {
      CReportsProfileLineInfo* pRPLI = new CReportsProfileLineInfo;
      pRPLI->Name = RepName;
      pRPLI->Type = PF.RdStr("Reports", TSect(), (char*)RepKey_GetValues);
      if (_stricmp(pRPLI->Type(), "Tag Values")==0)
        pRPLI->Type = RepKey_GetValues;
      Strng RepFileName = PF.RdStr("Reports", FSect(), "x.xls");
      if (PF.RdLong("Reports", SSect(), 0) != 0)
        {
        if (_stricmp(pRPLI->Type(), RepKey_SetValues)==0)
          SetSelected = i;
        else if (_stricmp(pRPLI->Type(), RepKey_Macro)==0)
          MacroSelected = i;
        else
          RepSelected = i;
        }
      pRPLI->Path.FnDrivePath(RepFileName());
      pRPLI->FileOnly.FnNameExt(RepFileName());
      if (pRPLI->Path()==NULL)
        {
        Strng Fn;
        Fn.FnSearchExpand(RepFileName(), AF_All);
        }
      pRPLI->FullFilename.Set("%s%s", pRPLI->Path(), pRPLI->FileOnly());
      pRPLI->FullFilename.FnContract();
      pRPLI->Path.FnDrivePath(pRPLI->FullFilename());
      if (SymbolicPaths())
        pRPLI->Path.FnContract();
      else
        pRPLI->Path.FnExpand();
      List.Add(pRPLI);
      }
    else
      break;
    }
  }

//--------------------------------------------------------------------------

void CReportsDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CReportsDlg)
  DDX_Control(pDX, IDC_REPORTSTAB, m_Tab);
  DDX_Control(pDX, IDC_FILELIST, m_RepList);
  DDX_Control(pDX, IDC_SETLIST, m_SetList);
  DDX_Control(pDX, IDC_MACROLIST, m_MacroList);
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CReportsDlg, CDialog)
  //{{AFX_MSG_MAP(CReportsDlg)
  ON_BN_CLICKED(IDC_ADD, OnAdd)
  ON_BN_CLICKED(IDC_REMOVE, OnRemove)
  ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
  ON_BN_CLICKED(IDC_EDIT, OnEdit)
  ON_BN_CLICKED(IDC_GENERATE, OnGenerate)
  ON_NOTIFY(TCN_SELCHANGE, IDC_REPORTSTAB, OnSelchangeReportstab)
  ON_BN_CLICKED(IDC_GENERATEALL, OnGenerateall)
  ON_LBN_SELCHANGE(IDC_FILELIST, OnSelchangeList)
  ON_LBN_DBLCLK(IDC_FILELIST, OnDblclkList)
	ON_BN_CLICKED(IDC_EDITINEXCEL, OnEditExcel)
	//}}AFX_MSG_MAP
  ON_LBN_DBLCLK(IDC_SETLIST, OnDblclkList)
  ON_LBN_SELCHANGE(IDC_SETLIST, OnSelchangeList)
  ON_UPDATE_COMMAND_UI(IDC_REMOVE, OnUpdateSelection)
  ON_UPDATE_COMMAND_UI(IDC_EDIT, OnUpdateSelection)
  ON_UPDATE_COMMAND_UI(IDC_GENERATE, OnUpdateSelection)
  ON_UPDATE_COMMAND_UI(IDC_GENERATEALL, OnUpdateGenerateAll)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CReportsDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  TC_ITEM Item;
  Item.mask = TCIF_TEXT;
  Item.pszText = "  Reports  ";
  m_Tab.InsertItem(0, &Item);
  Item.pszText = "  Set Tags  ";
  m_Tab.InsertItem(1, &Item);
  Item.pszText = "  Macro  ";
  m_Tab.InsertItem(2, &Item);
  for (int j=0; j<3; j++)
    {
    pList = ((j==0) ? &m_RepList : ((j==1) ? &m_SetList : &m_MacroList));
    RECT Rect;
    pList->GetWindowRect(&Rect);
    const int Width = Rect.right - Rect.left;
    pList->SetHeaderCount(4);
    pList->SetHeaderItem(0, "Filename", (int)(Width*0.28));
    pList->SetHeaderItem(1, "Name", (int)(Width*0.13));
    pList->SetHeaderItem(2, "Type", (int)(Width*0.16));
    pList->SetHeaderItem(3, "Folder", (int)(Width*0.43));
    pList->MakeHeader(34755+j);
    pList->ResetContent();
    }
  Strng s;
  CRPFLIArray List;
  int RepSelected, SetSelected, MacroSelected;
  BuildList(List, RepSelected, SetSelected, MacroSelected);
  for (int i=0; i<List.GetSize(); i++)
    {
    CReportsProfileLineInfo* pRPLI = List[i];
    s.Set("%s\t%s\t%s\t%s", pRPLI->FileOnly(), pRPLI->Name(), pRPLI->Type(), pRPLI->Path());
    if (_stricmp(pRPLI->Type(), RepKey_SetValues)==0)
      {
      j = m_SetList.AddString(s());
      if (i==SetSelected)
        m_SetList.SetCurSel(j);
      }
    else if (_stricmp(pRPLI->Type(), RepKey_Macro)==0)
      {
      j = m_MacroList.AddString(s());
      if (i==MacroSelected)
        m_MacroList.SetCurSel(j);
      }
    else
      {
      j = m_RepList.AddString(s());
      if (i==RepSelected)
        m_RepList.SetCurSel(j);
      }
    delete pRPLI;
    }
  CProfINIFile PF(PrjIniFile());
  int xPos = PF.RdInt("Reports", "XPos", 10);
  int yPos = PF.RdInt("Reports", "YPos", 10);
  int CurTab =  PF.RdInt("Reports", "TabPage", 0);

  m_Tab.SetCurSel(CurTab);
  ChangeTab();

  UpdateData(FALSE);
  SetVisibleWindowPos(this, xPos, yPos);
  UpdateDialogControls(this, FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//--------------------------------------------------------------------------

void CReportsDlg::ChangeTab()
  {
  int CurTab = m_Tab.GetCurSel();
  if (CurTab!=iPrevTab)
    {
    CWnd* w = GetDlgItem(IDC_FILELIST);
    w->EnableWindow(CurTab==0);
    w->ShowWindow(CurTab==0 ? SW_SHOW : SW_HIDE);
    w = GetDlgItem(IDC_SETLIST);
    w->EnableWindow(CurTab==1);
    w->ShowWindow(CurTab==1 ? SW_SHOW : SW_HIDE);
    w = GetDlgItem(IDC_MACROLIST);
    w->EnableWindow(CurTab==2);
    w->ShowWindow(CurTab==2 ? SW_SHOW : SW_HIDE);
    GetDlgItem(IDC_GENERATE)->SetWindowText(CurTab==0 ? "&Generate" : (CurTab==1 ? "&Process" : "&Execute"));
    w = GetDlgItem(IDC_GENERATEALL);
    w->SetWindowText(CurTab==0 ? "Generate A&ll" : "Process A&ll");
    w->EnableWindow(CurTab!=2);
    w->ShowWindow(CurTab!=2 ? SW_SHOW : SW_HIDE);
    pList = ((CurTab==0) ? &m_RepList : ((CurTab==1) ? &m_SetList : &m_MacroList));
    iPrevTab = CurTab;
    UpdateDialogControls(this, FALSE);
    }
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnOK() 
  {
  //save report list and selections to ini file...
  CProfINIFile PF(PrjIniFile());
  Strng RSect;
  int Cnt = 0;
  for (int j=0; j<3; j++)
    {
    pList = ((j==0) ? &m_RepList : ((j==1) ? &m_SetList : &m_MacroList));
    for (int i=0; i<pList->GetCount(); i++)
      {
      Strng TSect, FSect, SSect;
      RSect.Set("Report_Name%i", Cnt);
      TSect.Set("Report_Type%i", Cnt);
      FSect.Set("Report_FileName%i", Cnt);
      SSect.Set("Report_Selected%i", Cnt);
      CString RepName, RepFilename, RepType;
      ParseLine(i, RepName, RepFilename, RepType);
      Strng RepFileName((const char*)RepFilename);
      RepFileName.FnContract();

      PF.WrStr("Reports", RSect(), (char*)(const char*)RepName);
      PF.WrStr("Reports", TSect(), (char*)(const char*)RepType);
      PF.WrStr("Reports", FSect(), RepFileName());
      PF.WrLong("Reports", SSect(), (pList->GetCurSel()==i));
      Cnt++;
      }
    }
  RSect.Set("Report_Name%i", Cnt);
  PF.WrStr("Reports", RSect(), "");

  RECT Rect;
  GetWindowRect(&Rect);
  PF.WrInt("Reports", "XPos", Rect.left);
  PF.WrInt("Reports", "YPos", Rect.top);
  PF.WrInt("Reports", "TabPage", m_Tab.GetCurSel());

  CDialog::OnOK();
  DestroyWindow();
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnCancel()
  {
  OnOK();
  }

//--------------------------------------------------------------------------

void CReportsDlg::PostNcDestroy() 
  {
  CDialog::PostNcDestroy();
  delete this;
  }

//--------------------------------------------------------------------------

void CReportsDlg::ParseLine(int index, CString& RepName, CString& RepFilename, CString& RepType)
  {
  char Buff[1024];
  pList->GetText(index, Buff);
  CSVColArray C;
  ParseTabTokens(Buff, C);
  RepName = C[1];
  RepType = C[2];
  RepFilename = C[3];
  RepFilename += C[0];
  Strng s = RepFilename;
  s.FnExpand();
  RepFilename = s();
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnAdd() 
  {
  Strng s;
  s.Set("\t*\t%s\t", pList==&m_RepList ? RepKey_GetValues : (pList==&m_SetList ? RepKey_SetValues : RepKey_Macro));
  int i = pList->AddString(s());
  pList->SetCurSel(i);
  OnEdit();
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnRemove() 
  {
  int i = pList->GetCurSel();
  if (i!=LB_ERR)
    pList->DeleteString(i);
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnOptions()
  {
  CReportOptionsDlg Dlg(this);
  Dlg.m_ExcelLock = gs_pPrj->m_bRptExcelLock;
  Dlg.m_ExcelUseCellName = gs_pPrj->m_bRptExcelCellName;
  Dlg.m_ExcelMakeActive = gs_pPrj->m_bRptExcelMakeActive;
  Dlg.m_ExcelSysCADActive = gs_pPrj->m_bRptExcelSysCADActive;
  Dlg.m_ExcelUpdateLinks = gs_pPrj->m_bRptExcelUpdateLinks;
  Dlg.m_ExcelSaveOnComplete = gs_pPrj->m_bRptExcelSaveOnComplete;
  if (Dlg.DoModal()==IDOK)
    {
    gs_pPrj->m_bRptExcelLock = Dlg.m_ExcelLock;
    gs_pPrj->m_bRptExcelCellName = Dlg.m_ExcelUseCellName;
    gs_pPrj->m_bRptExcelMakeActive = Dlg.m_ExcelMakeActive;
    gs_pPrj->m_bRptExcelSysCADActive = Dlg.m_ExcelSysCADActive;
    gs_pPrj->m_bRptExcelUpdateLinks = Dlg.m_ExcelUpdateLinks;
    gs_pPrj->m_bRptExcelSaveOnComplete = Dlg.m_ExcelSaveOnComplete;
    }
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnEdit() 
  {
  int i = pList->GetCurSel();
  if (i!=LB_ERR)
    {
    CUpdateReportDlg Dlg(pList==&m_RepList ? 0 : (pList==&m_SetList ? 1 : 2), this);
    CString RepFilename;
    ParseLine(i, Dlg.m_Name, Dlg.m_Filename, Dlg.sType);
    RepFilename = Dlg.m_Filename;
    if (Dlg.DoModal()==IDOK && Dlg.m_Filename.GetLength()>0)
      {
      if (Dlg.m_Name.GetLength()==0)
        Dlg.m_Name = "*";

      char Drv[_MAX_DRIVE];
      char Dir[_MAX_DIR];
      char Name[_MAX_FNAME];
      char Ext[_MAX_EXT];
      _splitpath((const char*)Dlg.m_Filename, Drv, Dir, Name, Ext);
      Strng s,Path;
      Path.Set("%s%s", Drv, Dir);
      if (Path.Len()==0)
        Path = PrjFiles();
      if (SymbolicPaths())
        Path.FnContract();
      else
        Path.FnExpand();
      s.Set("%s%s\t%s\t%s\t%s", Name, Ext, (const char*)Dlg.m_Name, (const char*)Dlg.sType, Path());
      pList->DeleteString(i);
      i = pList->AddString(s());
      pList->SetCurSel(i);
      }
    else if (RepFilename.GetLength()==0)
      pList->DeleteString(i);
    }
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

void CReportsDlg::GenerateOne(int index)
  {
  CString RepName, RepFilename, RepType;
  ParseLine(index, RepName, RepFilename, RepType);
  short Opt = SUB_REPORT_GETVALUES;
  if (_stricmp((const char*)RepType, RepKey_GetValues)==0)
    Opt = SUB_REPORT_GETVALUES;
  else if (_stricmp((const char*)RepType, RepKey_Trend)==0)
    Opt = SUB_REPORT_TREND;
  else if (_stricmp((const char*)RepType, RepKey_SetValues)==0)
    Opt = SUB_REPORT_SETVALUES;
  else if (_stricmp((const char*)RepType, RepKey_Macro)==0)
    Opt = SUB_REPORT_MACRO;
  else if (_stricmp((const char*)RepType, RepKey_GenInfo)==0)
    Opt = SUB_REPORT_GENERALINFO;
  
  CXM_OleExcelReport* pXB = new CXM_OleExcelReport(NULL, (char*)(const char*)RepFilename, (char*)(const char*)RepName, Opt);
  MainWnd()->PostMessage(WMU_CMD, SUB_CMD_GENERATEOLEREPORT, (LPARAM)pXB);
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnGenerate() 
  {
  int i = pList->GetCurSel();
  if (i!=LB_ERR)
    GenerateOne(i);
  //OnOK();
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnGenerateall() 
  {
  for (int i=0; i<pList->GetCount(); i++)
    GenerateOne(i);
  //OnOK();
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnEditExcel() 
  {
  int i = pList->GetCurSel();
  if (i!=LB_ERR)
    {
    CString RepName, RepFilename, RepType;
    ParseLine(i, RepName, RepFilename, RepType);
    CXM_OleExcelReport* pXB = new CXM_OleExcelReport(NULL, (char*)(const char*)RepFilename, (char*)(const char*)RepName, SUB_REPORT_OPEN);
    MainWnd()->PostMessage(WMU_CMD, SUB_CMD_GENERATEOLEREPORT, (LPARAM)pXB);
    }
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnUpdateSelection(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(pList->GetCurSel()!=LB_ERR);
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnUpdateGenerateAll(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(pList->GetCount());
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnDblclkList() 
  {
  OnEdit();
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnSelchangeList() 
  {
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

void CReportsDlg::OnSelchangeReportstab(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  ChangeTab();
  *pResult = 0;
  }

//===========================================================================

class CFilenameDlg : public CDialog
  {
  public:
    CFilenameDlg(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CFilenameDlg)
    enum { IDD = IDD_FILENAMEDLG };
    CString m_Filename;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CFilenameDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CFilenameDlg)
    afx_msg void OnBrowse();
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//--------------------------------------------------------------------------

CFilenameDlg::CFilenameDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CFilenameDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CFilenameDlg)
  m_Filename = _T("");
  //}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

void CFilenameDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CFilenameDlg)
  DDX_Text(pDX, IDC_FILENAME, m_Filename);
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CFilenameDlg, CDialog)
  //{{AFX_MSG_MAP(CFilenameDlg)
  ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

void CFilenameDlg::OnBrowse() 
  {
  UpdateData(TRUE);
  const char* pFName = NULL;
  CString FName;
  CString FPath = PrjFiles();
  if (m_Filename.GetLength()>0)
    {
    char Drv[_MAX_DRIVE];
    char Dir[_MAX_DIR];
    char Name[_MAX_FNAME];
    char Ext[_MAX_EXT];
    _splitpath((const char*)m_Filename, Drv, Dir, Name, Ext);
    FPath = Drv;
    FPath += Dir;
    FName = Name;
    FName += Ext;
    pFName = (const char*)FName;
    }
  CSCDFileDialog Dlg(FALSE, NULL, pFName, OFN_NOCHANGEDIR | OFN_HIDEREADONLY, "SysCAD Command Scripts (*.ssc)|*.ssc|All files (*.*)|*.*||", this);
  Dlg.m_ofn.lpstrInitialDir = (const char*)FPath;
  Dlg.m_ofn.lpstrTitle = "Browse";
  Dlg.m_ofn.lpstrFilter = "SysCAD Command Scripts (*.ssc)\0*.ssc\0All files (*.*)\0*.*\0\0";
  if (Dlg.DoModal()==IDOK)
    m_Filename = Dlg.GetPathName();

  if (m_Filename.GetLength()>0)
    {
    char Ext[_MAX_EXT];
    _splitpath((const char*)m_Filename, NULL, NULL, NULL, Ext);
    if (strlen(Ext)==0)
      m_Filename += ".ssc";
    }

  Strng Tmp((char*)(const char*)m_Filename);  
  Tmp.FnContract();
  m_Filename=Tmp();  
  
  UpdateData(FALSE);
  }

//--------------------------------------------------------------------------

void CFilenameDlg::OnOK() 
  {
  UpdateData(TRUE);
  if (m_Filename.GetLength()>0)
    {
    char Ext[_MAX_EXT];
    _splitpath((const char*)m_Filename, NULL, NULL, NULL, Ext);
    if (strlen(Ext)==0)
      {
      m_Filename += ".ssc";
      UpdateData(FALSE);
      }
    }
  CDialog::OnOK();
  }

//===========================================================================

CScriptsDlg::CScriptsDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CScriptsDlg::IDD, pParent)
  {
  pTTC = NULL;
  hProcess = NULL;
  //{{AFX_DATA_INIT(CScriptsDlg)
  //}}AFX_DATA_INIT
  Create(CScriptsDlg::IDD, pParent); //create modeless dialog
  }

//--------------------------------------------------------------------------

CScriptsDlg::~CScriptsDlg()
  {
  MainWnd()->m_pCmdScriptsDlg = NULL; //set global pointer to null
  delete pTTC;
  }

//--------------------------------------------------------------------------

int CScriptsDlg::BuildList(CSPFLIArray& List)
  {//NOTE: Remember to delete all the individual items in list!!!

  //load list and selections from ini file...
  int Selected = -1;
  CProfINIFile PF(PrjIniFile());
  for (int i=0; ; i++)
    {
    Strng FSect, SSect;//, s;
    FSect.Set("Script_FileName%i", i);
    SSect.Set("Script_Selected%i", i);
    Strng FileName = PF.RdStr("CmdScripts", FSect(), "");
    if (FileName.Length()>0)
      {
      CScriptsProfileLineInfo* pSPLI = new CScriptsProfileLineInfo;
      if (PF.RdLong("CmdScripts", SSect(), 0) != 0)
        Selected = i;
      pSPLI->Path.FnDrivePath(FileName());
      pSPLI->FileOnly.FnNameExt(FileName());
      if (pSPLI->Path()==NULL)
        {
        Strng Fn;
        Fn.FnSearchExpand(FileName(), AF_All);
        }
      pSPLI->FullFilename.Set("%s%s", pSPLI->Path(), pSPLI->FileOnly());
      pSPLI->FullFilename.FnContract();
      pSPLI->Path.FnDrivePath(pSPLI->FullFilename());
      if (SymbolicPaths())
        pSPLI->Path.FnContract();
      else
        pSPLI->Path.FnExpand();

      List.Add(pSPLI);
      }
    else
      break;
    }
  return Selected;
  }

//--------------------------------------------------------------------------

void CScriptsDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CScriptsDlg)
  DDX_Control(pDX, IDC_FILELIST, m_List);
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CScriptsDlg, CDialog)
  //{{AFX_MSG_MAP(CScriptsDlg)
  ON_BN_CLICKED(IDC_ADD, OnAdd)
  ON_BN_CLICKED(IDC_REMOVE, OnRemove)
  ON_BN_CLICKED(IDC_EDIT, OnEdit)
  ON_LBN_SELCHANGE(IDC_FILELIST, OnSelchangeFilelist)
  ON_BN_CLICKED(IDC_EXECSCRIPT, OnExecScript)
  ON_LBN_DBLCLK(IDC_FILELIST, OnDblclkFilelist)
  ON_BN_CLICKED(IDC_EDITFILE, OnEditfile)
  //}}AFX_MSG_MAP
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
  ON_UPDATE_COMMAND_UI(IDC_REMOVE, OnUpdateSelection)
  ON_UPDATE_COMMAND_UI(IDC_EDIT, OnUpdateSelection)
  ON_UPDATE_COMMAND_UI(IDC_EXECSCRIPT, OnUpdateSelection)
  ON_UPDATE_COMMAND_UI(IDC_EDITFILE, OnUpdateSelection)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CScriptsDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  pTTC = new CCustomToolTipCtrl(this);
  m_List.SetHeaderCount(2);
  RECT Rect;
  m_List.GetWindowRect(&Rect);
  const int Width = Rect.right - Rect.left;
  m_List.SetHeaderItem(0, "Filename", (int)(Width*0.3));
  m_List.SetHeaderItem(1, "Folder", (int)(Width*0.7));
  m_List.MakeHeader(34757);
  m_List.ResetContent();

  Strng s;
  CSPFLIArray List;
  const int Selected = BuildList(List);
  for (int i=0; i<List.GetSize(); i++)
    {
    CScriptsProfileLineInfo* pSPLI = List[i];
    s.Set("%s\t%s", pSPLI->FileOnly(), pSPLI->Path());
    int j = m_List.AddString(s());
    if (i==Selected)
      m_List.SetCurSel(j);
    delete pSPLI;
    }
  
  CProfINIFile PF(PrjIniFile());
  int xPos = PF.RdInt("CmdScripts", "XPos", 10);
  int yPos = PF.RdInt("CmdScripts", "YPos", 10);

  UpdateData(FALSE);
  SetVisibleWindowPos(this, xPos, yPos);
  UpdateDialogControls(this, FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------

BOOL CScriptsDlg::PreTranslateMessage(MSG* pMsg) 
  {
  if (pTTC && HelpMngr.ShowToolTips())
    pTTC->RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CScriptsDlg::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
  if (pTTC && HelpMngr.ShowToolTips())
    return pTTC->OnToolTipNotify(pNMHDR, CScriptsDlg::IDD);
  return FALSE;
  }

//--------------------------------------------------------------------------

void CScriptsDlg::OnOK() 
  {
  //save list and selections to ini file...
  CProfINIFile PF(PrjIniFile());
  for (int i=0; i<=m_List.GetCount(); i++)
    {
    Strng FSect, SSect;
    FSect.Set("Script_FileName%i", i);
    SSect.Set("Script_Selected%i", i);
    if (i<m_List.GetCount())
      {
      CString Filename, s;
      m_List.GetTextInColumn(0, i, s);
      m_List.GetTextInColumn(1, i, Filename);
      Filename += s;
      Strng FileName((const char*)Filename);
      FileName.FnContract();
      
      PF.WrStr("CmdScripts", FSect(), FileName());
      PF.WrLong("CmdScripts", SSect(), (m_List.GetCurSel()==i));
      }
    else
      PF.WrStr("CmdScripts", FSect(), "");
    }
  RECT Rect;
  GetWindowRect(&Rect);
  PF.WrInt("CmdScripts", "XPos", Rect.left);
  PF.WrInt("CmdScripts", "YPos", Rect.top);

  CDialog::OnOK();
  DestroyWindow();
  }

//--------------------------------------------------------------------------

void CScriptsDlg::OnCancel()
  {
  OnOK();
  }

//--------------------------------------------------------------------------

void CScriptsDlg::PostNcDestroy() 
  {
  CDialog::PostNcDestroy();
  delete this;
  }

//--------------------------------------------------------------------------

void CScriptsDlg::OnAdd() 
  {
  int i = m_List.AddString("\t");
  m_List.SetCurSel(i);
  OnEdit();
  }

//--------------------------------------------------------------------------

void CScriptsDlg::OnRemove() 
  {
  int i = m_List.GetCurSel();
  if (i!=LB_ERR)
    m_List.DeleteString(i);
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

void CScriptsDlg::OnEdit() 
  {
  int i = m_List.GetCurSel();
  if (i!=LB_ERR)
    {
    CFilenameDlg Dlg(this);
    Strng ss;
    CString Filename, s;
    m_List.GetTextInColumn(0, i, s);
    m_List.GetTextInColumn(1, i, Filename);
    Filename += s;
    ss = Filename;
    ss.FnExpand();
    Filename = ss();
    Dlg.m_Filename = Filename;
    if (Dlg.DoModal()==IDOK && Dlg.m_Filename.GetLength()>0)
      {
      char Drv[_MAX_DRIVE];
      char Dir[_MAX_DIR];
      char Name[_MAX_FNAME];
      char Ext[_MAX_EXT];
      _splitpath((const char*)Dlg.m_Filename, Drv, Dir, Name, Ext);
      Strng Path;
      Path.Set("%s%s", Drv, Dir);
      if (Path.Length()==0)
        Path = PrjFiles();
      if (SymbolicPaths())
        Path.FnContract();
      else
        Path.FnExpand();
      ss.Set("%s%s\t%s", Name, Ext, Path());
      m_List.DeleteString(i);
      i = m_List.AddString(ss());
      m_List.SetCurSel(i);
      }
    else if (Filename.GetLength()==0)
      m_List.DeleteString(i);
    }
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

void CScriptsDlg::OnExecScript() 
  {
  int i = m_List.GetCurSel();
  if (i!=LB_ERR)
    {
    CString Filename, s;
    m_List.GetTextInColumn(0, i, s);
    m_List.GetTextInColumn(1, i, Filename);
    Filename += s;

    Strng TmpFN((char*)(const char*)Filename);
    TmpFN.FnSearchExpand(NULL, AF_All|AF_BackupFiles);

    //char* pTxt = new char[TmpFN.Len()+1];
    //strcpy(pTxt, TmpFN());
    //ScdMainWnd()->PostMessage(WMU_RUNSCRIPT, (WPARAM)0, (LPARAM)pTxt);
    if (gs_pXCmd->AddCmds(TmpFN())<1)
      {
      OnOK();
      MainWnd()->PostMessage(WMU_CMDNEXT, ComCmd_ExecNextCmd, 0);
      }
    }
  }

//--------------------------------------------------------------------------

void CScriptsDlg::OnEditfile() 
  {
  int i = m_List.GetCurSel();
  if (i!=LB_ERR)
    {
    CString Filename, s;
    m_List.GetTextInColumn(0, i, s);
    m_List.GetTextInColumn(1, i, Filename);
    Filename += s;
    if (Filename.GetLength()>0)
      {
      DWORD ExitCode = 0;
      if (hProcess)
        GetExitCodeProcess(hProcess, &ExitCode);
      if (ExitCode==STILL_ACTIVE)
        {
        ActivateApp(dwProcessId);
        //AfxMessageBox("A script file is allready open!");
        }
      else
        {
        hProcess = NULL;
        Strng NPad;
        Strng TmpFN((char*)(const char*)Filename);
        TmpFN.FnSearchExpand(NULL, AF_All|AF_BackupFiles);

        NPad=TxtEditCmdString(TmpFN());
        STARTUPINFO si;
        memset(&si, 0, sizeof(si));
        si.cb = sizeof(si);
        si.wShowWindow = SW_SHOWDEFAULT;
        PROCESS_INFORMATION pi;
        if (CreateProcess(NULL, NPad(), NULL, NULL, FALSE, 0, NULL, PrjFiles(), &si, &pi))
          {
          hProcess = pi.hProcess;
          dwProcessId = pi.dwProcessId;
          }
        }
      }
    }
  }

//--------------------------------------------------------------------------

void CScriptsDlg::OnUpdateSelection(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_List.GetCurSel()!=LB_ERR);
  }

//--------------------------------------------------------------------------

void CScriptsDlg::OnSelchangeFilelist() 
  {
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

void CScriptsDlg::OnDblclkFilelist() 
  {
  OnEdit();
  }

//===========================================================================

class CCompareOptionsDlg : public CDialog
  {
  public:
    CCompareOptionsDlg(CWnd* pParent = NULL);   // standard constructor
    ~CCompareOptionsDlg();
    //{{AFX_DATA(CCompareOptionsDlg)
    enum { IDD = IDD_COMPAREOPTIONSDLG };
    BOOL m_ShowAllTags;
    BOOL m_ShowStrings;
    BOOL m_ShowAllMissing;
    BOOL m_ShowSortedFile;
    BOOL m_ResetOutFile;
    double m_AbsTolerance;
    double m_RelTolerance;
    long m_MaxCount;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CCompareOptionsDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    CCustomToolTipCtrl* pTTC;
    //{{AFX_MSG(CCompareOptionsDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()
  };

//--------------------------------------------------------------------------

CCompareOptionsDlg::CCompareOptionsDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CCompareOptionsDlg::IDD, pParent)
  {
  pTTC = NULL;
  //{{AFX_DATA_INIT(CCompareOptionsDlg)
  m_ShowAllTags = FALSE;
  m_ShowStrings = FALSE;
  m_ShowAllMissing = FALSE;
  m_ShowSortedFile = FALSE;
  m_ResetOutFile = FALSE;
  m_AbsTolerance = 0.0;
  m_RelTolerance = 0.0;
  m_MaxCount = 0;
  //}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

CCompareOptionsDlg::~CCompareOptionsDlg()
  {
  delete pTTC;
  }

//--------------------------------------------------------------------------

void CCompareOptionsDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCompareOptionsDlg)
  DDX_Check(pDX, IDC_SHOWALLTAGS, m_ShowAllTags);
  DDX_Check(pDX, IDC_SHOWSTRINGS, m_ShowStrings);
  DDX_Check(pDX, IDC_SHOWALLMISSING, m_ShowAllMissing);
  DDX_Check(pDX, IDC_SHOWSORTEDFILE, m_ShowSortedFile);
  DDX_Check(pDX, IDC_RESETOUTFILE, m_ResetOutFile);
  DDX_Text(pDX, IDC_ABSTOLERANCE, m_AbsTolerance);
  DDX_Text(pDX, IDC_RELTOLERANCE, m_RelTolerance);
  DDX_Text(pDX, IDC_MAXCOUNT, m_MaxCount);
  //}}AFX_DATA_MAP
  m_AbsTolerance = Max(0.0, fabs(m_AbsTolerance));
  m_RelTolerance = Max(0.0, fabs(m_RelTolerance));
  if (m_AbsTolerance==0.0 && m_RelTolerance==0.0)
    m_AbsTolerance = 1.0e-6;
  m_MaxCount = Max(10L, m_MaxCount);
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCompareOptionsDlg, CDialog)
  //{{AFX_MSG_MAP(CCompareOptionsDlg)
  //}}AFX_MSG_MAP
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CCompareOptionsDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  pTTC = new CCustomToolTipCtrl(this);
  return TRUE;
  };

//---------------------------------------------------------------------------

BOOL CCompareOptionsDlg::PreTranslateMessage(MSG* pMsg) 
  {
  if (pTTC && HelpMngr.ShowToolTips())
    pTTC->RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CCompareOptionsDlg::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
  if (pTTC && HelpMngr.ShowToolTips())
    return pTTC->OnToolTipNotify(pNMHDR, CCompareOptionsDlg::IDD);
  return FALSE;
  }

//===========================================================================

class CCompareUpdateDlg : public CDialog
  {
  public:
    CCompareUpdateDlg(CWnd* pParent = NULL);   // standard constructor
    ~CCompareUpdateDlg();
	//{{AFX_DATA(CCompareUpdateDlg)
    enum { IDD = IDD_COMPAREUPDATEDLG };
    CString m_Filename1;
    CString m_Filename2;
    CString m_OutFilename;
    int m_Current;
    int m_Window;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CCompareUpdateDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    CCustomToolTipCtrl* pTTC;
    void Browse(CString& Filename, char* pFilter);
    //{{AFX_MSG(CCompareUpdateDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnBrowse();
    afx_msg void OnBrowse2();
    afx_msg void OnBrowse3();
    //}}AFX_MSG
    afx_msg void OnRadio(UINT nID);
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpdateOutputFile(CCmdUI* pCmdUi);
    afx_msg void OnUpdateFile2(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//--------------------------------------------------------------------------

CCompareUpdateDlg::CCompareUpdateDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CCompareUpdateDlg::IDD, pParent)
  {
  pTTC = NULL;
  //{{AFX_DATA_INIT(CCompareUpdateDlg)
  m_Filename1 = _T("");
  m_Filename2 = _T("");
  m_OutFilename = _T("");
  m_Current = -1;
  m_Window = -1;
  //}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

CCompareUpdateDlg::~CCompareUpdateDlg()
  {
  delete pTTC;
  }

//--------------------------------------------------------------------------

void CCompareUpdateDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCompareUpdateDlg)
  DDX_Text(pDX, IDC_FILENAME, m_Filename1);
  DDX_Text(pDX, IDC_FILENAME2, m_Filename2);
  DDX_Text(pDX, IDC_OUTFILENAME, m_OutFilename);
  DDX_Radio(pDX, IDC_FILE2RADIO1, m_Current);
  DDX_Radio(pDX, IDC_OUTRADIO1, m_Window);
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCompareUpdateDlg, CDialog)
  //{{AFX_MSG_MAP(CCompareUpdateDlg)
  ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
  ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
  ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
  //}}AFX_MSG_MAP
  ON_CONTROL_RANGE(BN_CLICKED, IDC_FILE2RADIO1, IDC_OUTRADIO2, OnRadio)
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
  ON_UPDATE_COMMAND_UI(IDC_OUTFILENAME, OnUpdateOutputFile)
  ON_UPDATE_COMMAND_UI(IDC_BROWSE3, OnUpdateOutputFile)
  ON_UPDATE_COMMAND_UI(IDC_FILENAME2, OnUpdateFile2)
  ON_UPDATE_COMMAND_UI(IDC_BROWSE2, OnUpdateFile2)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CCompareUpdateDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  pTTC = new CCustomToolTipCtrl(this);
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CCompareUpdateDlg::PreTranslateMessage(MSG* pMsg) 
  {
  if (pTTC && HelpMngr.ShowToolTips())
    pTTC->RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CCompareUpdateDlg::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
  if (pTTC && HelpMngr.ShowToolTips())
    return pTTC->OnToolTipNotify(pNMHDR, CCompareUpdateDlg::IDD);
  return FALSE;
  }

//--------------------------------------------------------------------------

void CCompareUpdateDlg::Browse(CString& Filename, char* pFilter)
  {
  UpdateData(TRUE);
  const char* pFName = NULL;
  CString FName;
  CString FPath = PrjFiles();
  if (Filename.GetLength()>0)
    {
    char Drv[_MAX_DRIVE];
    char Dir[_MAX_DIR];
    char Name[_MAX_FNAME];
    char Ext[_MAX_EXT];
    _splitpath((const char*)Filename, Drv, Dir, Name, Ext);
    FPath = Drv;
    FPath += Dir;
    FName = Name;
    FName += Ext;
    pFName = (const char*)FName;
    }

  CSCDFileDialog Dlg(FALSE, NULL, pFName, OFN_NOCHANGEDIR | OFN_HIDEREADONLY, pFilter, this);
  Dlg.m_ofn.lpstrInitialDir = (const char*)FPath;
  Dlg.m_ofn.lpstrTitle = "Browse";
  if (Dlg.DoModal()==IDOK)
    Filename = Dlg.GetPathName();

  UpdateData(FALSE);
  }

//--------------------------------------------------------------------------

void CCompareUpdateDlg::OnBrowse() 
  {
  Browse(m_Filename1, "Scenarios (*.scn)|*.scn|SnapShots (*.snp)|*.snp|All files (*.*)|*.*||");
  }

//--------------------------------------------------------------------------

void CCompareUpdateDlg::OnBrowse2() 
  {
  Browse(m_Filename2, "Scenarios (*.scn)|*.scn|SnapShots (*.snp)|*.snp|All files (*.*)|*.*||");
  }

//--------------------------------------------------------------------------

void CCompareUpdateDlg::OnBrowse3() 
  {
  //Browse(m_OutFilename, "Output text file (*.csv)\0*.csv\0All files (*.*)\0*.*\0\0");
  Browse(m_OutFilename, "Output text file (*.txt;*.csv)|*.txt;*.csv|All files (*.*)|*.*||");
  }

//--------------------------------------------------------------------------

void CCompareUpdateDlg::OnRadio(UINT nID) 
  {
  UpdateData(TRUE);
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

void CCompareUpdateDlg::OnUpdateOutputFile(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_Window==1);
  }

//--------------------------------------------------------------------------

void CCompareUpdateDlg::OnUpdateFile2(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_Current==1);
  }

//===========================================================================

char* GetNameExt(char* Filename)
  {
  if (Filename && strlen(Filename)>0)
    {
    char Drv[_MAX_DRIVE];
    char Dir[_MAX_DIR];
    char Name[_MAX_FNAME];
    char Ext[_MAX_EXT];
    _splitpath(Filename, Drv, Dir, Name, Ext);
    static char NameExt[_MAX_FNAME + _MAX_EXT + 2];
    sprintf(NameExt, "%s%s", Name, Ext);
    return NameExt;
    }
  return "";
  }

char* CCompareLine::MakeLine(Strng& Line)
  {
  Line.Set("%s\t", GetNameExt(sFilename1()));
  Line += (bCurrent ? "Current" : GetNameExt(sFilename2()));
  Line += '\t';
  Line += (bWindow ? "Window" : GetNameExt(sOutFilename()));
  return Line();
  }

//===========================================================================

CCompareResultsDlg::CCompareResultsDlg(CCompareInfo* Info, CWnd* pParent)
  : CDialog(CCompareResultsDlg::IDD, pParent)
  {
  pInfo = Info;
  //{{AFX_DATA_INIT(CCompareResultsDlg)
  //}}AFX_DATA_INIT
  RetVal = gs_Exec.CompareScenarios(*pInfo);
  if (RetVal>=0)
    Create(CCompareResultsDlg::IDD, pParent); //create modeless dialog
  }

//--------------------------------------------------------------------------

CCompareResultsDlg::~CCompareResultsDlg()
  {
  MainWnd()->m_pCompareResDlg = NULL; //set global pointer to null
  }

//--------------------------------------------------------------------------

void CCompareResultsDlg::PostNcDestroy() 
  {
  CDialog::PostNcDestroy();
  delete this;
  }

//--------------------------------------------------------------------------

void CCompareResultsDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCompareResultsDlg)
  DDX_Control(pDX, IDC_FILELIST, m_List);
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCompareResultsDlg, CDialog)
  //{{AFX_MSG_MAP(CCompareResultsDlg)
  ON_NOTIFY(NM_DBLCLK, IDC_FILELIST, OnDblclkList)
  ON_NOTIFY(NM_RCLICK, IDC_FILELIST, OnRclickList)
  //}}AFX_MSG_MAP
  ON_COMMAND_RANGE(IDM_PRJ_ACCESS, IDM_PRJ_FIND, OnMenu)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------
#define ReverseColumns 1
#define AdjustWidths   0

BOOL CCompareResultsDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();

  Strng S1("Current");
  if (pInfo->sFilename1.Length()>0)
    S1=pInfo->sFilename1.FnNameExt(pInfo->sFilename1());
  Strng S2("Current");
  if (pInfo->sFilename2.Length()>0 && !pInfo->bCurrent)
    S2=pInfo->sFilename2.FnNameExt(pInfo->sFilename2());

#if AdjustWidths   
  int ReqdColWidth[4]={20,20,20,20};
#endif
#if ReverseColumns
  m_List.InsertColumn(0, "Tag", LVCFMT_LEFT, 226);
  m_List.InsertColumn(1, "Error(%)", LVCFMT_RIGHT, 70);
#else
  m_List.InsertColumn(0, "Error(%)", LVCFMT_RIGHT, 70);
  m_List.InsertColumn(1, "Tag", LVCFMT_LEFT, 226);
#endif
  m_List.InsertColumn(2, S1(), LVCFMT_RIGHT, 125);
  m_List.InsertColumn(3, S2(), LVCFMT_RIGHT, 125);
  m_List.DeleteAllItems();
  for (int i=0; i<pInfo->List.GetSize(); i++)
    {
    LVITEM Item;
    Item.mask = LVIF_TEXT;
    Item.state = 0;
    Item.stateMask = 0;
    Item.cchTextMax = 0;
    Item.iImage = 0;
    int k = 0;
    
#if ReverseColumns 
    int j = m_List.InsertItem(i, (LPCTSTR)pInfo->List[i].sTag());
#if AdjustWidths   
    ReqdColWidth[k]=Max(ReqdColWidth[k],m_List.GetStringWidth((LPCTSTR)pInfo->List[i].sTag()));
#endif
#else
    char Buff[64];
    if (pInfo->List[i].sErrStr.Length()>0)
      strcpy(Buff, pInfo->List[i].sErrStr());
    else
      sprintf(Buff, "%8.4f", pInfo->List[i].dErr*100);
    int j = m_List.InsertItem(i, (LPCTSTR)Buff);
#if AdjustWidths   
    ReqdColWidth[k]=Max(ReqdColWidth[k],m_List.GetStringWidth(Buff));
#endif
#endif
    k++;

    Item.iItem = j;
    Item.iSubItem = k;
#if ReverseColumns 
    char Buff[64];
    if (pInfo->List[i].sErrStr.Length()>0)
      strcpy(Buff, pInfo->List[i].sErrStr());
    else
      sprintf(Buff, "%8.4f", pInfo->List[i].dErr*100);
    Item.pszText = Buff;
//    ReqdColWidth[k]=Max(ReqdColWidth[k],m_List.GetStringWidth(Item.pszText));
#else
    Item.pszText = pInfo->List[i].sTag();
#endif
    m_List.SetItem(&Item);
#if AdjustWidths   
    ReqdColWidth[k]=Max(ReqdColWidth[k],m_List.GetStringWidth(Item.pszText));
#endif
    k++;

    Item.iSubItem = k;
    Item.pszText = pInfo->List[i].sValue1();
    m_List.SetItem(&Item);
#if AdjustWidths   
    ReqdColWidth[k]=Max(ReqdColWidth[k],m_List.GetStringWidth(Item.pszText));
#endif
    k++;
    
    Item.iSubItem = k;
    Item.pszText = pInfo->List[i].sValue2();
    m_List.SetItem(&Item);
#if AdjustWidths   
    ReqdColWidth[k]=Max(ReqdColWidth[k],m_List.GetStringWidth(Item.pszText));
#endif
    k++;
    }

#if AdjustWidths   
  for (int k=0; k<4; k++)
    m_List.SetColumnWidth(k, ReqdColWidth[k]+10);
#endif  
  char Buff[64];
  sprintf(Buff, "%d", pInfo->iNoObjsCompared);
  SetDlgItemText(IDC_NOOBJSCOMPARED, Buff);
  sprintf(Buff, "%d", pInfo->iNoObjsNotFound);
  SetDlgItemText(IDC_NOOBJSNOTFOUND, Buff);
  sprintf(Buff, "%d", pInfo->iNoTagsDifferent);
  SetDlgItemText(IDC_NOTAGSDIFFERENT, Buff);
  sprintf(Buff, "%d", pInfo->iNoTagsNotMatched);
  SetDlgItemText(IDC_NOTAGSNOTMATCHED, Buff);
  CProfINIFile PF(PrjIniFile());
  int xPos = PF.RdInt("CompareScenarios", "XResPos", 10);
  int yPos = PF.RdInt("CompareScenarios", "YResPos", 10);
  //UpdateData(FALSE);
  //UpdateDialogControls(this, FALSE);
  SetVisibleWindowPos(this, xPos, yPos);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CCompareResultsDlg::OnOK() 
  {
  RECT Rect;
  GetWindowRect(&Rect);
  CProfINIFile PF(PrjIniFile());
  PF.WrInt("CompareScenarios", "XResPos", Rect.left);
  PF.WrInt("CompareScenarios", "YResPos", Rect.top);
  CDialog::OnOK();
  }

//---------------------------------------------------------------------------

int CCompareResultsDlg::AccessableTag(CString& sTag)
  {
  sTag = "";
  LV_HITTESTINFO HTI;
  HTI.pt.y = m_List.PrevDownPoint.y;
  HTI.pt.x = 10;
  iLastIndex = m_List.HitTest(&HTI);
  if (gs_pPrj && iLastIndex>=0 && (HTI.flags & LVHT_ONITEM))
    {
#if ReverseColumns 
    sTag = m_List.GetItemText(iLastIndex, 0);
#else
    sTag = m_List.GetItemText(iLastIndex, 1);
#endif
    if (sTag.GetLength()>0)
      {
      const int i = sTag.Find('.');
      if (i>=0)
        sTag = sTag.Left(i);
      CXM_ObjectTag ObjTag((char*)(const char*)sTag, TABOpt_Exists);
      CXM_ObjectData ObjData;
      CXM_Route Route;
      if (gs_pPrj->XReadTaggedItem(ObjTag, ObjData, Route))
        return iLastIndex;
      }
    }
  return -1;
  }

//--------------------------------------------------------------------------

void CCompareResultsDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  if (AccessableTag(sLastTag)>=0)
    gs_AccessWnds.AccessNode(-1, (char*)(const char*)sLastTag);
  *pResult = 0;
  }

//--------------------------------------------------------------------------

void CCompareResultsDlg::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  if (AccessableTag(sLastTag)>=0)
    {
    CMenu Menu;
    Menu.CreatePopupMenu();
    Strng s;
    s.Set("&Access %s...", (const char*)sLastTag);
    Menu.AppendMenu(MF_STRING, IDM_PRJ_ACCESS, s());
    Menu.AppendMenu(MF_STRING, IDM_PRJ_FIND, "&Find...");
    //Menu.AppendMenu(MF_STRING, IDM_PRJ_CHANGETAG, "&Change Tag...");
    //Menu.AppendMenu(MF_SEPARATOR);
    CRect Rect;
    m_List.GetWindowRect(&Rect);
    Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, Rect.left+m_List.PrevDownPoint.x, Rect.top+m_List.PrevDownPoint.y, this);
    Menu.DestroyMenu();
    }
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CCompareResultsDlg::OnMenu(UINT id)
  {
  switch (id)
    {
    case IDM_PRJ_ACCESS:
      gs_AccessWnds.AccessNode(-1, (char*)(const char*)sLastTag);
      break;
    case IDM_PRJ_FIND:
      gs_pPrj->FindTag((char*)(const char*)sLastTag, NULL, NULL, -1, FTO_MoveCursor|FTO_HighliteSlow);
      break;
    }
  }

//===========================================================================

CCompareDlg::CCompareDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CCompareDlg::IDD, pParent)
  {
  pTTC = NULL;
  //{{AFX_DATA_INIT(CCompareDlg)
  m_ResetOutFile = FALSE;
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CCompareDlg::~CCompareDlg()
  {
  delete pTTC;
  }

//--------------------------------------------------------------------------

void CCompareDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCompareDlg)
  DDX_Control(pDX, IDC_FILELIST, m_List);
  DDX_Check(pDX, IDC_RESETOUTFILE, m_ResetOutFile);
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCompareDlg, CDialog)
  //{{AFX_MSG_MAP(CCompareDlg)
  ON_BN_CLICKED(IDC_ADD, OnAdd)
  ON_BN_CLICKED(IDC_REMOVE, OnRemove)
  ON_BN_CLICKED(IDC_EDIT, OnEdit)
  ON_LBN_SELCHANGE(IDC_FILELIST, OnSelchangeFilelist)
  ON_BN_CLICKED(IDC_COMPARE, OnCompare)
  ON_LBN_DBLCLK(IDC_FILELIST, OnDblclkFilelist)
  ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
  //}}AFX_MSG_MAP
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
  ON_UPDATE_COMMAND_UI(IDC_REMOVE, OnUpdateSelection)
  ON_UPDATE_COMMAND_UI(IDC_EDIT, OnUpdateSelection)
  ON_UPDATE_COMMAND_UI(IDC_COMPARE, OnUpdateSelection)
  ON_UPDATE_COMMAND_UI(IDC_RESETOUTFILE, OnUpdateResetOutFile)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

const char* PFCompareSections[5] = { "Compare1_FileName", "Compare2_FileName", "Output_Filename", "Compare_Current", "Compare_Window" };

BOOL CCompareDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();

  pTTC = new CCustomToolTipCtrl(this);
  
  m_List.SetHeaderCount(3);
  RECT Rect;
  m_List.GetWindowRect(&Rect);
  const int Width = Rect.right - Rect.left;
  m_List.SetHeaderItem(0, "File 1", (int)(Width*0.34));
  m_List.SetHeaderItem(1, "File 2", (int)(Width*0.34));
  m_List.SetHeaderItem(2, "Output", (int)(Width*0.32));
  m_List.MakeHeader(34759);
  m_List.ResetContent();
  //load list and selections from ini file...
  CProfINIFile PF(PrjIniFile());
  for (int i=0; ; i++)
    {
    Strng Sect;
    Sect.Set("%s%i", PFCompareSections[0], i);
    Strng s = PF.RdStr("CompareScenarios", Sect(), "");
    if (s.Length()>0)
      {
      CCompareLine Line;
      Line.sFilename1 = s;
      for (int j=1; j<5; j++)
        {
        Sect.Set("%s%i", PFCompareSections[j], i);
        switch (j)
          {
          case 1: Line.sFilename2 = PF.RdStr("CompareScenarios", Sect(), ""); break;
          case 2: Line.sOutFilename = PF.RdStr("CompareScenarios", Sect(), ""); break;
          case 3: Line.bCurrent = (PF.RdInt("CompareScenarios", Sect(), 1)!=0); break;
          case 4: Line.bWindow = (PF.RdInt("CompareScenarios", Sect(), 1)!=0); break;
          }
        }
      Lines.Add(Line);
      Sect.Set("Selected%i", i);
      BOOL Selected = (PF.RdLong("CompareScenarios", Sect(), 0) != 0);
      Strng TheLine;
      Line.MakeLine(TheLine);
      j = m_List.AddString(TheLine());
      if (Selected)
        m_List.SetCurSel(j);
      }
    else
      break;
    }
  CompInfo.dRelTolerance = PF.RdDouble("CompareScenarios", "RelTolerance", 0.0001);
  CompInfo.dAbsTolerance = PF.RdDouble("CompareScenarios", "AbsTolerance", 1.0e-6);
  CompInfo.iMaxCount = PF.RdLong("CompareScenarios", "MaxCount", 200);
  CompInfo.bShowAllTags = (PF.RdInt("CompareScenarios", "ShowAllTags", 0)!=0);
  CompInfo.bShowStrings = (PF.RdInt("CompareScenarios", "ShowStrings", 0)!=0);
  CompInfo.bShowSortedFile = (PF.RdInt("CompareScenarios", "ShowSortedFile", 0)!=0);
  CompInfo.bShowAllMissing = (PF.RdInt("CompareScenarios", "ShowAllMissing", 0)!=0);
  CompInfo.bResetOutFile = (PF.RdInt("CompareScenarios", "ResetOutFile", 1)!=0);
  int xPos = PF.RdInt("CompareScenarios", "XPos", 10);
  int yPos = PF.RdInt("CompareScenarios", "YPos", 10);
  m_ResetOutFile = CompInfo.bResetOutFile;

  UpdateData(FALSE);
  SetVisibleWindowPos(this, xPos, yPos);
  UpdateDialogControls(this, FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------

BOOL CCompareDlg::PreTranslateMessage(MSG* pMsg) 
  {
  if (pTTC && HelpMngr.ShowToolTips())
    pTTC->RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CCompareDlg::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
  if (pTTC && HelpMngr.ShowToolTips())        
    return pTTC->OnToolTipNotify(pNMHDR, CCompareDlg::IDD);
  return FALSE;
  }

//---------------------------------------------------------------------------

void CCompareDlg::OnOK() 
  {
  UpdateData(TRUE);
  CompInfo.bResetOutFile = m_ResetOutFile;
  //save list and selections to ini file...
  CProfINIFile PF(PrjIniFile());
  for (int i=0; i<=m_List.GetCount(); i++)
    {
    Strng Sect;
    CString s;
    if (i<m_List.GetCount())
      {
      for (int j=0; j<5; j++)
        {
        Sect.Set("%s%i", PFCompareSections[j], i);
        switch (j)
          {
          case 0: PF.WrStr("CompareScenarios", Sect(), Lines[i].sFilename1()); break;
          case 1: PF.WrStr("CompareScenarios", Sect(), Lines[i].sFilename2()); break;
          case 2: PF.WrStr("CompareScenarios", Sect(), Lines[i].sOutFilename()); break;
          case 3: PF.WrInt("CompareScenarios", Sect(), Lines[i].bCurrent); break;
          case 4: PF.WrInt("CompareScenarios", Sect(), Lines[i].bWindow); break;
          }
        }
      Sect.Set("Selected%i", i);
      PF.WrLong("CompareScenarios", Sect(), (m_List.GetCurSel()==i));
      }
    else
      {
      Sect.Set("%s%i", PFCompareSections[0], i);
      PF.WrStr("CompareScenarios", Sect(), "");
      }
    }
  PF.WrDouble("CompareScenarios", "RelTolerance", CompInfo.dRelTolerance);
  PF.WrDouble("CompareScenarios", "AbsTolerance", CompInfo.dAbsTolerance);
  PF.WrLong("CompareScenarios", "MaxCount", CompInfo.iMaxCount);
  PF.WrInt("CompareScenarios", "ShowAllTags", CompInfo.bShowAllTags);
  PF.WrInt("CompareScenarios", "ShowStrings", CompInfo.bShowStrings);
  PF.WrInt("CompareScenarios", "ShowSortedFile", CompInfo.bShowSortedFile);
  PF.WrInt("CompareScenarios", "ShowAllMissing", CompInfo.bShowAllMissing);
  PF.WrInt("CompareScenarios", "ResetOutFile", CompInfo.bResetOutFile);
  RECT Rect;
  GetWindowRect(&Rect);
  PF.WrInt("CompareScenarios", "XPos", Rect.left);
  PF.WrInt("CompareScenarios", "YPos", Rect.top);

  CDialog::OnOK();
  }

//--------------------------------------------------------------------------

void CCompareDlg::OnCancel()
  {
  //OnOK();
  }

//--------------------------------------------------------------------------

void CCompareDlg::OnAdd() 
  {
  int i = m_List.AddString("\tCurrent\tWindow");
  CCompareLine Line;
  Line.sFilename1 = "";
  Line.sFilename2 = "";
  Line.sOutFilename = "compare.txt";
  Line.bCurrent = 1;
  Line.bWindow = 1;
  Lines.Add(Line);
  m_List.SetCurSel(i);
  OnEdit();
  }

//--------------------------------------------------------------------------

void CCompareDlg::OnRemove() 
  {
  int i = m_List.GetCurSel();
  if (i!=LB_ERR)
    {
    m_List.DeleteString(i);
    Lines.RemoveAt(i);
    }
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

void CheckPath(Strng& s)
  {
  if (s.Length()>0)
    {
    char Drv[_MAX_DRIVE];
    char Dir[_MAX_DIR];
    char Name[_MAX_FNAME];
    char Ext[_MAX_EXT];
    _splitpath(s(), Drv, Dir, Name, Ext);
    Strng Path;
    Path.Set("%s%s", Drv, Dir);
    if (Path.Length()==0)
      Path = PrjFiles();
    s.Set("%s%s%s", Path(), Name, Ext);
    }
  }

//--------------------------------------------------------------------------

void CCompareDlg::OnEdit() 
  {
  int i = m_List.GetCurSel();
  if (i!=LB_ERR)
    {
    CCompareUpdateDlg Dlg(this);
    Dlg.m_Filename1 = Lines[i].sFilename1();
    Dlg.m_Filename2 = Lines[i].sFilename2();
    Dlg.m_OutFilename = Lines[i].sOutFilename();
    Dlg.m_Current = (Lines[i].bCurrent ? 0 : 1);
    Dlg.m_Window = (Lines[i].bWindow ? 0 : 1);
    if (Dlg.DoModal()==IDOK && Dlg.m_Filename1.GetLength()>0)
      {
      Lines[i].sFilename1 = (const char*)Dlg.m_Filename1;
      Lines[i].sFilename2 = (const char*)Dlg.m_Filename2;
      Lines[i].sOutFilename = (const char*)Dlg.m_OutFilename;
      Lines[i].bCurrent = (Dlg.m_Current==0);
      Lines[i].bWindow = (Dlg.m_Window==0);
      CheckPath(Lines[i].sFilename1);
      CheckPath(Lines[i].sFilename2);
      CheckPath(Lines[i].sOutFilename);
      Strng TheLine;
      Lines[i].MakeLine(TheLine);
      m_List.DeleteString(i);
      i = m_List.InsertString(i, TheLine());
      m_List.SetCurSel(i);
      }
    else if (Dlg.m_Filename1.GetLength()==0)
      {
      m_List.DeleteString(i);
      Lines.RemoveAt(i);
      }
    }
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

void CCompareDlg::OnOptions() 
  {
  UpdateData(TRUE);
  CompInfo.bResetOutFile = m_ResetOutFile;
  CCompareOptionsDlg Dlg(this);
  Dlg.m_ShowAllTags = CompInfo.bShowAllTags;
  Dlg.m_ShowStrings = CompInfo.bShowStrings;
  Dlg.m_ShowAllMissing = CompInfo.bShowAllMissing;
  Dlg.m_ShowSortedFile = CompInfo.bShowSortedFile;
  Dlg.m_ResetOutFile = CompInfo.bResetOutFile;
  Dlg.m_AbsTolerance = CompInfo.dAbsTolerance;
  Dlg.m_RelTolerance = CompInfo.dRelTolerance;
  Dlg.m_MaxCount = CompInfo.iMaxCount;
  if (Dlg.DoModal()==IDOK)
    {
    CompInfo.bShowAllTags = Dlg.m_ShowAllTags;
    CompInfo.bShowStrings = Dlg.m_ShowStrings;
    CompInfo.bShowAllMissing = Dlg.m_ShowAllMissing;
    CompInfo.bShowSortedFile = Dlg.m_ShowSortedFile;
    CompInfo.bResetOutFile = Dlg.m_ResetOutFile;
    CompInfo.dAbsTolerance = Dlg.m_AbsTolerance;
    CompInfo.dRelTolerance = Dlg.m_RelTolerance;
    CompInfo.iMaxCount = Dlg.m_MaxCount;
    m_ResetOutFile = CompInfo.bResetOutFile;
    }
  UpdateData(FALSE);
  }

//--------------------------------------------------------------------------

void CCompareDlg::OnCompare() 
  {
  const int i = m_List.GetCurSel();
  if (i!=LB_ERR)
    {
    UpdateData(TRUE);
    GetParent()->SetActiveWindow();
    ShowWindow(SW_HIDE);
    CCompareInfo Info;
    Info.pRoot = PrjFiles();
    Info.sFilename1 = Lines[i].sFilename1;
    Info.sFilename2 = Lines[i].sFilename2;
    Info.sOutFilename = Lines[i].sOutFilename;
    Info.iMaxCount = CompInfo.iMaxCount;
    Info.dRelTolerance = CompInfo.dRelTolerance;
    Info.dAbsTolerance = CompInfo.dAbsTolerance;
    Info.bCurrent = Lines[i].bCurrent;
    Info.bWindow = Lines[i].bWindow;
    Info.bShowStrings = CompInfo.bShowStrings;
    Info.bResetOutFile = CompInfo.bResetOutFile;
    Info.bShowAllTags = CompInfo.bShowAllTags;
    Info.bShowSortedFile = CompInfo.bShowSortedFile;
    Info.bShowAllMissing = CompInfo.bShowAllMissing;

    int RetVal = 0;
    if (Info.bWindow)
      {
      if (MainWnd()->m_pCompareResDlg)
        MainWnd()->m_pCompareResDlg->DestroyWindow();
      MainWnd()->m_pCompareResDlg = new CCompareResultsDlg(&Info, MainWnd());
      RetVal = MainWnd()->m_pCompareResDlg->RetVal;
      if (RetVal<0)
        delete MainWnd()->m_pCompareResDlg;
      //CCompareResultsDlg Dlg(&Info, this);
      //Dlg.DoModal();
      //RetVal = Dlg.RetVal;
      }
    else
      RetVal = gs_Exec.CompareScenarios(Info);
    if (RetVal>=0)
      OnOK();
    else
      ShowWindow(SW_SHOW);
    }
  }

//--------------------------------------------------------------------------

void CCompareDlg::OnUpdateSelection(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_List.GetCurSel()!=LB_ERR);
  }

//--------------------------------------------------------------------------

void CCompareDlg::OnUpdateResetOutFile(CCmdUI* pCmdUi)
  {
  const int i = m_List.GetCurSel();
  pCmdUi->Enable(i!=LB_ERR && !Lines[i].bWindow);
  }

//--------------------------------------------------------------------------

void CCompareDlg::OnSelchangeFilelist() 
  {
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

void CCompareDlg::OnDblclkFilelist() 
  {
  OnEdit();
  }

//===========================================================================


