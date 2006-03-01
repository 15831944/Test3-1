//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#include "scexec.h"
#include "resource.h"
#include "slvcfg.h"
#include "scd_wm.h"
#include "executiv.h"
#include "odesolve.h"
#include "grfdoc.h"
#include "tagvdoc.h"
#include "project.h"
#include "syscad.h"
#include "afxwin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

enum AStates { ASInactive, ASPartial, ASActive };
inline SFEFlwLib * FE() { return gs_pPrj->FlwLib(); };

#define SetWndDef(h) ::SetWindowLong(h, GWL_STYLE, ::GetWindowLong(h, GWL_STYLE) | BS_DEFPUSHBUTTON);

//===========================================================================
//
//
//
//===========================================================================

class CActivePage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CActivePage)
  public:
    CActivePage();
    ~CActivePage();

    CImageList m_ImgList;
    ActiveTagInfoMap TagMap;

    void UpdateInfo();

    static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

    //{{AFX_DATA(CActivePage)
    enum { IDD = IDD_SOLVEACTIVE };
    CListCtrl m_ActPageList;
    //}}AFX_DATA
    int m_Method;

    //{{AFX_VIRTUAL(CActivePage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CActivePage)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnPgactivate();
    afx_msg void OnPgdeactivate();
    afx_msg void OnClickActpages(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    afx_msg void OnGetDispInfo (NMHDR* pnmhdr, LRESULT* pResult);
    afx_msg void OnUpdateBtns(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CSolveCfgPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CSolveCfgPage)
  public:
    CSolveCfgPage();
    ~CSolveCfgPage();

    //{{AFX_DATA(CSolveCfgPage)
    enum { IDD = IDD_SOLVECFG };
    double	m_EPS_Rel;
    double	m_EPS_Abs;
    int		m_MaxIters;
    double	m_Damping;
    BOOL	m_fMassSmooth;
    int		m_RqdConvergedIters;
    //}}AFX_DATA
    int m_Method;

    //{{AFX_VIRTUAL(CSolveCfgPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CSolveCfgPage)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnChangeData();
    afx_msg void OnEditchangeSlvMeth();
    afx_msg void OnSelchangeSlvMeth();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CSolveOptnPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CSolveOptnPage)
  public:
    CSolveOptnPage();
    ~CSolveOptnPage();

    //{{AFX_DATA(CSolveOptnPage)
    enum { IDD = IDD_SOLVEOPTN };
    BOOL	m_Initialise;
    BOOL	m_StatePinned;
    BOOL	m_IdleWhenDone;
    BOOL	m_ResetHist;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CSolveOptnPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CSolveOptnPage)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnChangeData();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CRunCfgPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CRunCfgPage)
  public:
    CRunCfgPage();
    ~CRunCfgPage();

    //{{AFX_DATA(CRunCfgPage)
    enum { IDD = IDD_RUNCFGMAIN };
    CString m_ForTime;
    CString m_UntilTime;
    long m_ForIters;
    CString	m_CurrentTimeHMS;
    BOOL m_SyncWithClock;
    BOOL m_RealTime;
    double m_RealTimeMult;
    //}}AFX_DATA

    void FixTimes(byte FixWhat=0xFF);
    RunTerms  iRunTerm;
    double    dTimeUntilStop;
    double    m_CurrentTimeAtInit;
    bool      m_CurrentTimeChgd;
    BOOL      m_PrevSyncWithClock;

    //{{AFX_VIRTUAL(CRunCfgPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CRunCfgPage)
    afx_msg void OnRunfor();
    afx_msg void OnRunforever();
    afx_msg void OnRunsteady();
    afx_msg void OnRununtil();
    afx_msg void OnChangeFortime();
    afx_msg void OnChangeUntiltime();
    afx_msg void OnKillfocusFortime();
    afx_msg void OnKillfocusUntiltime();
    virtual void OnOK();
    afx_msg void OnSetfocusFortime();
    afx_msg void OnSetfocusUntiltime();
    virtual void OnCancel();
    virtual BOOL OnInitDialog();
    afx_msg void OnRunforiters();
    afx_msg void OnKillfocusForiters();
    afx_msg void OnSetfocusForiters();
    afx_msg void OnChangeForiters();
    afx_msg void OnSyncWithClock();
    afx_msg void OnRealtime();
    afx_msg void OnChangeRealtimeMult();
    afx_msg void OnKillfocusCurrenttime();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnEnChangeCurrenttime();
  };

//===========================================================================

class CRunOptionPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CRunOptionPage)
  public:
    CRunOptionPage();
    ~CRunOptionPage();

    //{{AFX_DATA(CRunOptionPage)
    enum { IDD = IDD_RUNOPTN };
    BOOL m_MomentumOn;
    BOOL m_ResetHist;
    BOOL m_ResetTime;
    CString	m_ResetTimeOnStart;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CRunOptionPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CRunOptionPage)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnRunResetTime();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  public:
    BOOL m_bEmptySpills;
    BOOL m_bEmptyAll;
    BOOL m_bZeroFlows;
    BOOL m_bInitialiseAll;
    BOOL m_bResetStats;
    BOOL m_bOptnsOnce;
  };

//===========================================================================

class CRunMethodPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CRunMethodPage)
  public:
    CRunMethodPage();
    ~CRunMethodPage();

    int m_iMethod;
    int m_iStep;

    //{{AFX_DATA(CRunMethodPage)
    enum { IDD = IDD_RUN_METHOD };
    double m_Abs_Eps;
    double m_Rel_Eps;
    double m_Step_Cur;
    double m_Step_Max;
    double m_Step_Min;
    double m_MaxStepTime;
    long m_StepCount;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CRunMethodPage)
    public:
    virtual void OnOK();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
  protected:
    //{{AFX_MSG(CRunMethodPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnUpdateOdeAbsEps();
    afx_msg void OnUpdateOdeRelEps();
    afx_msg void OnUpdateOdeStepCur();
    afx_msg void OnUpdateOdeStepMax();
    afx_msg void OnUpdateOdeStepMin();
    afx_msg void OnChangeOdeStepCur();
    afx_msg void OnChangeMaxStepTime();
    afx_msg void OnKillfocusMaxStepTime();
    afx_msg void OnChangeStepCount();
    afx_msg void OnSelchangeOdeMeth();
    afx_msg void OnSelchangeOdeStep();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CRunNetworkPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CRunNetworkPage)
  public:
    CRunNetworkPage();
    ~CRunNetworkPage();

    //{{AFX_DATA(CRunNetworkPage)
    enum { IDD = IDD_RUN_NETWORK };
    double	m_LnkAbs;
    double	m_LnkRel;
    double	m_NetDerate;
    CString	m_SpillArea;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CRunNetworkPage)
    public:
    virtual void OnOK();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CRunNetworkPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnChangeRunLnkabs();
    afx_msg void OnChangeRunLnkrel();
    afx_msg void OnChangeRunNetderate();
    afx_msg void OnChangeRunspillarea();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CRunTearPage : public CCustomPropertyPage
  {
  DECLARE_DYNCREATE(CRunTearPage)
  public:
    CRunTearPage();
    ~CRunTearPage();

    //{{AFX_DATA(CRunTearPage)
    enum { IDD = IDD_RUNTEAR };
    double m_EPS_Rel;
    double m_EPS_Abs;
    int    m_MaxIters;
    double m_Damping;
    //}}AFX_DATA
    int m_Method;

    //{{AFX_VIRTUAL(CRunTearPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CRunTearPage)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnChangeSlvAbs();
    afx_msg void OnChangeSlvDamping();
    afx_msg void OnChangeSlvMaxiters();
    afx_msg void OnEditchangeSlvMeth();
    afx_msg void OnChangeSlvRel();
    afx_msg void OnSelchangeSlvMeth();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================
//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_DYNCREATE(CActivePage, CCustomPropertyPage)

CActivePage::CActivePage() : CCustomPropertyPage(CActivePage::IDD)
  {
  //{{AFX_DATA_INIT(CActivePage)
  //}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

CActivePage::~CActivePage()
  {
  if (m_ActPageList.GetSafeHwnd())
    for (int i=0; i<m_ActPageList.GetItemCount(); i++)
      {
      ActiveTagInfo *pAT=(ActiveTagInfo *)m_ActPageList.GetItemData(i);
      delete pAT;
      }
  }

//--------------------------------------------------------------------------

int CALLBACK CActivePage::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
  {
  ActiveTagInfo *pAT1=(ActiveTagInfo *)lParam1;
  ActiveTagInfo *pAT2=(ActiveTagInfo *)lParam2;

  if (pAT1->fIsDLL)
    return -1;
  if (pAT2->fIsDLL)
    return 1;
  if (pAT1->NTags==0 && pAT2->NTags>0)
    return 1;
  if (pAT1->NTags>0 && pAT2->NTags==0)
    return -1;
  return stricmp(pAT1->Name(), pAT2->Name());
  };
  
//--------------------------------------------------------------------------

inline int CountActive(Strng_List & TagList) 
  {
  int N=0;
  for (pStrng p=TagList.First(); p; p=p->Next())
    if (p->Index())
      N++;
  return N;
  };

//--------------------------------------------------------------------------

inline int SetActive(Strng_List & TagList, flag Active) 
  {
  int N=0;
  for (pStrng p=TagList.First(); p; p=p->Next())
    {
    p->SetIndex(Active);
    N++;
    }
  return N;
  };

//--------------------------------------------------------------------------

inline int SetActive(Strng_List & TagList, ActiveTagInfoMap & TagMap, flag Active) 
  {
  int N=0;
  for (pStrng p=TagList.First(); p; p=p->Next())
    {
    pStrng p0;
    if (TagMap.Lookup(p->Str(), p0))
      p0->SetIndex(Active);
    N++;
    }
  return N;
  };

//--------------------------------------------------------------------------

int ActiveIcon(ActiveTagInfo *pAT)
  {
  if (pAT->NTags==0 && pAT->NActive==0)
    return 0;
  
  int Icon=2;
  if (pAT->NTagsI==pAT->NActiveI)
    Icon=3;
  if (pAT->NTagsI>0 && pAT->NActiveI==0)
    Icon=1;
  if (pAT->NTagsI==pAT->NTags && pAT->NActiveI==pAT->NActive)
    return Icon;

  if (pAT->NActive>pAT->NActiveI)
    return Icon+6;
  return Icon+3;
  }

//--------------------------------------------------------------------------

void CActivePage::UpdateInfo()
  {
  for (int i=0; i<m_ActPageList.GetItemCount(); i++)
    {
    ActiveTagInfo *pAT=(ActiveTagInfo *)m_ActPageList.GetItemData(i);
    if (i>0)
      {
      // Transfer Active States to Page lists
      for (pStrng p=pAT->TagList.First(); p; p=p->Next())
        {
        pStrng p0;
        if (TagMap.Lookup(p->Str(), p0))
          p->SetIndex(p0->Index());
        }
      }

    pAT->NActive = CountActive(pAT->TagList);
    Strng S;
    S.Set("%i", pAT->NTags);
    m_ActPageList.SetItemText(i, 1, S());
    S.Set(pAT->NActive ? "%i" : "", pAT->NActive);
    m_ActPageList.SetItemText(i, 2, S());
    S.Set(pAT->NMissing ? "%i" : "", pAT->NMissing);
    m_ActPageList.SetItemText(i, 3, pAT->Missing() ? pAT->Missing() : "");
    }
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

BOOL CActivePage::OnInitDialog() 
  {
  CWaitMsgCursor Wait("Building");
  CCustomPropertyPage::OnInitDialog();
  m_ActPageList.InsertColumn(0, "Drawing", LVCFMT_LEFT, 100);
  m_ActPageList.InsertColumn(1, "Tags",    LVCFMT_RIGHT, 50);
  m_ActPageList.InsertColumn(2, "Active",  LVCFMT_RIGHT, 50);
  m_ActPageList.InsertColumn(3, "Missing", LVCFMT_LEFT, 0);
  
  CBitmap BM,BMM;
  BM.LoadBitmap(IDB_ACTPAGEIMGS);
  BOOL b=m_ImgList.Create(10, 11, FALSE, 9, 10);
  m_ImgList.Add(&BM, (CBitmap*)NULL);
  
  m_ActPageList.SetImageList(&m_ImgList , LVSIL_SMALL);

  ActiveTagInfo *pAT0=new ActiveTagInfo;
  pAT0->GetTags();
  //pAT0->TagList.Append("*");
  //pAT0->NTags   = FE()->FE_TagOperation(False, pAT0->TagList);
  pAT0->NFound  = pAT0->NTags;
  pAT0->NActive = CountActive(pAT0->TagList);
  pAT0->Name    = pAT0->TagList.Length()>0 ? pAT0->TagList.First()->Str() : "";
  pAT0->fIsDLL  = True;
  pAT0->NTagsI   = pAT0->NTags;
  pAT0->NActiveI = pAT0->NActive; 

  int ItemNo=m_ActPageList.InsertItem(0, pAT0->Name(), I_IMAGECALLBACK);//ActiveIcon(pAT0));
  m_ActPageList.SetItemData(ItemNo, (UINT)pAT0);
  
  pAT0->RebuildTagMap(TagMap);
  pAT0->SetActive(False); // SetIndex to Zero

  POSITION Pos = ScdApp()->GraphTemplate().GetFirstDocPosition();
  while (Pos)
    {
    CGrfDoc* pGDoc = (CGrfDoc*)(ScdApp()->GraphTemplate().GetNextDoc(Pos));
    CString S(pGDoc->GetTitle()), S1(pGDoc->GetTitle());
    S1.MakeUpper();
    int p=S1.Find(".SCG");
    if (p>=0)
      S.GetBufferSetLength(p);

    ItemNo++;
    ActiveTagInfo *pAT=new ActiveTagInfo;
    pAT->NTags=pGDoc->GetTagList(pAT->TagList);
    pAT->Name=(const char *)S;
    pAT->NFound=FE()->FE_TagOperation(FETOp_GetActive, pAT->TagList);
    pAT->NActive = CountActive(pAT->TagList);
    pAT->fIsDLL   = False;
    pAT->NTagsI   = pAT->NTags;
    pAT->NActiveI = pAT->NActive; 

    int i=m_ActPageList.InsertItem(ItemNo, pAT->Name(), I_IMAGECALLBACK);

    ASSERT(i==ItemNo);
    m_ActPageList.SetItemData(ItemNo, (UINT)pAT);

    for (pStrng pT=pAT->TagList.First(); pT; pT=pT->Next())
      {
      pStrng p0;
      if (TagMap.Lookup(pT->Str(), p0))
        p0->SetIndex(1); // Found
      else
        {
        pAT->NMissing++;
        if (pAT->Missing.Length()==0)
          pAT->Missing+=pT->Str();
        else
          {
          pAT->Missing+=",";
          pAT->Missing+=pT->Str();
          int W=m_ActPageList.GetColumnWidth(3);
          int Ws=m_ActPageList.GetStringWidth(pAT->Missing());
          m_ActPageList.SetColumnWidth(3, Max(W, Ws));
          }
        }
      }
    }

  m_ActPageList.SortItems(CompareFunc, (DWORD)this);

  if (gs_pPrj->AllGrfLoaded())
    {
    pAT0->NMissing=0;
    pStrng pT=pAT0->TagList.First(); // Skip the first one - FlwSolve
    for (pT=pT->Next(); pT; pT=pT->Next())
      if (pT->Index()==0)
        {
        pAT0->NMissing++;
        if (pAT0->Missing.Length()==0)
          pAT0->Missing+=pT->Str();
        else
          {
          pAT0->Missing+=",";
          pAT0->Missing+=pT->Str();
          int W=m_ActPageList.GetColumnWidth(3);
          int Ws=m_ActPageList.GetStringWidth(pAT0->Missing());
          m_ActPageList.SetColumnWidth(3, Max(W, Ws));
          }
        }
    }

  // Reload Active status
  FE()->FE_TagOperation(FETOp_GetActive, pAT0->TagList);
  
  UpdateInfo();

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//--------------------------------------------------------------------------

void CActivePage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CActivePage)
  DDX_Control(pDX, IDC_ACTPAGES, m_ActPageList);
  //}}AFX_DATA_MAP
  
  if (!pDX->m_bSaveAndValidate)
    {
    UpdateInfo();
    }
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CActivePage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CActivePage)
  ON_WM_DESTROY()
  ON_BN_CLICKED(ID_PGACTIVATE, OnPgactivate)
  ON_BN_CLICKED(ID_PGDEACTIVATE, OnPgdeactivate)
  ON_NOTIFY(NM_CLICK, IDC_ACTPAGES, OnClickActpages)
  //}}AFX_MSG_MAP
  ON_NOTIFY(LVN_GETDISPINFO, IDC_ACTPAGES, OnGetDispInfo)
  ON_UPDATE_COMMAND_UI(ID_PGACTIVATE, OnUpdateBtns)
  ON_UPDATE_COMMAND_UI(ID_PGDEACTIVATE, OnUpdateBtns)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

void CActivePage::OnUpdateBtns(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_ActPageList.GetSelectedCount()>0);
  }

//--------------------------------------------------------------------------

void CActivePage::OnGetDispInfo(NMHDR* pnmhdr, LRESULT* pResult)
  {
  LV_DISPINFO* pdi = (LV_DISPINFO *) pnmhdr;

  // Fill in the LVITEM structure with the image info.
  // When an item is selected, the image is set to the first
  // image (the new bitmap on the toolbar).
  // When it is not selected, the image index is equal to the
  // item number (that is, 0=new, 1=open, 2=save, and so on.)
  ActiveTagInfo *pAT=(ActiveTagInfo *)m_ActPageList.GetItemData(pdi->item.iItem);
  pdi->item.iImage = ActiveIcon(pAT);//pdi->item.iItem;
  }
 
//--------------------------------------------------------------------------

void CActivePage::OnOK() 
  {
  if (Inited())
    {
    // Set Active status
    ActiveTagInfo *pAT0=(ActiveTagInfo *)m_ActPageList.GetItemData(0);
    FE()->FE_TagOperation(FETOp_SetActive, pAT0->TagList);
    SetModified(FALSE);
    }
  }

//--------------------------------------------------------------------------

void CActivePage::OnDestroy() 
  {
  CCustomPropertyPage::OnDestroy();

  // TODO: Add your message handler code here
  }

//--------------------------------------------------------------------------

void CActivePage::OnPgactivate() 
  {
  ActiveTagInfo *pAT0=(ActiveTagInfo *)m_ActPageList.GetItemData(0);
  int i=m_ActPageList.GetNextItem(-1, LVNI_SELECTED);
  while (i>=0)
    {
    ActiveTagInfo *pAT=(ActiveTagInfo *)m_ActPageList.GetItemData(i);
    m_ActPageList.SetItemState(i, 0, LVIS_SELECTED);
    if (i==0)
      SetActive(pAT->TagList, True);
    else
      SetActive(pAT->TagList, TagMap, True);
    CDocument* pDoc = CWindowLists::GetGrfWndByName(pAT->Name());
    if (pDoc)
      ((CGrfDoc*)pDoc)->bModelsActive=1;
    i=m_ActPageList.GetNextItem(i, LVNI_SELECTED);
    }
  UpdateData(FALSE);
  m_ActPageList.RedrawItems(0, m_ActPageList.GetItemCount()-1);
  SetModified(TRUE);
  }

//--------------------------------------------------------------------------

void CActivePage::OnPgdeactivate() 
  {
  ActiveTagInfo *pAT0=(ActiveTagInfo *)m_ActPageList.GetItemData(0);
  int i=m_ActPageList.GetNextItem(-1, LVNI_SELECTED);
  while (i>=0)
    {
    ActiveTagInfo *pAT=(ActiveTagInfo *)m_ActPageList.GetItemData(i);
    m_ActPageList.SetItemState(i, 0, LVIS_SELECTED);
    if (i==0)
      SetActive(pAT->TagList, False);
    else
      SetActive(pAT->TagList, TagMap, False);
    CDocument* pDoc = CWindowLists::GetGrfWndByName(pAT->Name());
    if (pDoc)
      ((CGrfDoc*)pDoc)->bModelsActive=0;
    i=m_ActPageList.GetNextItem(i, LVNI_SELECTED);
    }
  UpdateData(FALSE);
  m_ActPageList.RedrawItems(0, m_ActPageList.GetItemCount()-1);
  SetModified(TRUE);
  }

void CActivePage::OnClickActpages(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  UpdateDialogControls(this, FALSE);

  *pResult = 0;
  }

//===========================================================================
//
//
//
//===========================================================================

bool SetGrfPageActive(int index, bool Active)
  {
  bool Changed = false;
  if (index<CWindowLists::GetGrfWndCount())
    {
    CGrfDoc* pGDoc = (CGrfDoc*)CWindowLists::GetGrfWndByIndex(index);
    if (pGDoc->bModelsActive != Active)
      {
      pGDoc->bModelsActive = Active;
      Changed = true;
      Strng_List TagList;
      pGDoc->GetTagList(TagList);
      for (pStrng p=TagList.First(); p; p=p->Next())
        {
        p->SetIndex(Active);
        }
      FE()->FE_TagOperation(FETOp_SetActive, TagList);
      }
    }
  return Changed;
  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_DYNCREATE(CSolveCfgPage, CPropertyPage)

CSolveCfgPage::CSolveCfgPage() : CCustomPropertyPage(CSolveCfgPage::IDD)
  {
  //{{AFX_DATA_INIT(CSolveCfgPage)
  m_EPS_Rel = 1;
  m_EPS_Abs = 1;
  m_MaxIters = 1;
  m_Damping = 10.0;
	m_fMassSmooth = FALSE;
	m_RqdConvergedIters = 3;
	//}}AFX_DATA_INIT
  m_Method=TCM_AdaptSubs;
  //bDidInit = False;

  //	m_Iters = 0;
//  m_State = _T("");
  }

//--------------------------------------------------------------------------

CSolveCfgPage::~CSolveCfgPage()
  {
  //bDidInit = False;
  }

//--------------------------------------------------------------------------

BOOL CSolveCfgPage::OnInitDialog() 
  {
  m_EPS_Rel = PBEqnCB().Cfg.dEps_Rel;
  m_EPS_Abs = PBEqnCB().Cfg.dEps_Abs;
  m_MaxIters = PBEqnCB().Cfg.iMaxIters;
  m_RqdConvergedIters = PBEqnCB().Cfg.iRqdCnvrgdIters;
  m_Damping = PBEqnCB().Cfg.dDamping*100.0;
  m_Method = PBEqnCB().Cfg.iConvergeMeth;
  #if ALLOWMASSSMOOTH
  m_fMassSmooth = PBEqnCB().Cfg.fMassSmooth;
  #else
  m_fMassSmooth = false;
	GetDlgItem(IDC_MASS_SMOOTH)->EnableWindow(false);
  #endif
  //InitDialog();//bDidInit = True;
  CCustomPropertyPage::OnInitDialog();

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//--------------------------------------------------------------------------

void CSolveCfgPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSolveCfgPage)
  DDX_Text(pDX, IDC_SLV_REL, m_EPS_Rel);
  DDV_MinMaxDouble(pDX, m_EPS_Rel, 1.e-010, 0.1);
  DDX_Text(pDX, IDC_SLV_ABS, m_EPS_Abs);
  DDV_MinMaxDouble(pDX, m_EPS_Abs, 1.e-010, 1.);
  DDX_Text(pDX, IDC_SLV_MAXITERS, m_MaxIters);
  DDV_MinMaxInt(pDX, m_MaxIters, 1, 20000);
  DDX_Text(pDX, IDC_SLV_DAMPING, m_Damping);
  DDV_MinMaxDouble(pDX, m_Damping, 0., 100.);
	DDX_Check(pDX, IDC_MASS_SMOOTH, m_fMassSmooth);
	DDX_Text(pDX, IDC_SLV_RQDCONVERGEDITERS, m_RqdConvergedIters);
	DDV_MinMaxInt(pDX, m_RqdConvergedIters, 1, 999);
	//}}AFX_DATA_MAP
  
  CComboBox * pMethLst=(CComboBox *)GetDlgItem(IDC_SLV_METH);
  if (pDX->m_bSaveAndValidate)
    {
    m_Method=Range(TCM_DirectSubs, (byte)(pMethLst->GetCurSel()+1), TCM_Wegstein);
    }
  else
    {
    pMethLst->ResetContent();
    pMethLst->InsertString(TCM_DirectSubs-1, "Direct Substitution");
    pMethLst->InsertString(TCM_AdaptSubs-1, "Adaptive Direct Subs");
    pMethLst->InsertString(TCM_Wegstein-1, "Wegstein Acceleration");
    pMethLst->SetCurSel(m_Method-1);
    CWnd * pDampText=GetDlgItem(IDC_SLV_DAMPINGTXT);
    CWnd * pDamp=GetDlgItem(IDC_SLV_DAMPING);
    switch (m_Method)
      {
      case TCM_DirectSubs: pDamp->EnableWindow(TRUE); pDampText->SetWindowText("Damping (%)"); break;
      case TCM_AdaptSubs: pDamp->EnableWindow(TRUE); pDampText->SetWindowText("Minimum Damping (%)"); break;
      case TCM_Wegstein: pDamp->EnableWindow(FALSE); pDampText->SetWindowText(""); break;
      }
    //pDampText->RedrawWindow();
    }

  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSolveCfgPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CSolveCfgPage)
  ON_WM_DESTROY()
  ON_EN_CHANGE(IDC_SLV_REL, OnChangeData)
  ON_EN_CHANGE(IDC_SLV_ABS, OnChangeData)
  ON_EN_CHANGE(IDC_SLV_DAMPING, OnChangeData)
  ON_EN_CHANGE(IDC_SLV_MAXITERS, OnChangeData)
  ON_EN_CHANGE(IDC_SLV_RQDCONVERGEDITERS, OnChangeData)
	ON_BN_CLICKED(IDC_MASS_SMOOTH, OnChangeData)
  ON_CBN_EDITCHANGE(IDC_SLV_METH, OnEditchangeSlvMeth)
  ON_CBN_SELCHANGE(IDC_SLV_METH, OnSelchangeSlvMeth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

void CSolveCfgPage::OnOK() 
  {
  if (Inited())
    {
    //gs_pPrj->lStatsRefreshCnt = m_RefreshCnt;
    CComboBox * pMethLst = (CComboBox *)GetDlgItem(IDC_SLV_METH);
    #if ALLOWMASSSMOOTH
    PBEqnCB().Cfg.fMassSmooth = m_fMassSmooth;
    #else
    PBEqnCB().Cfg.fMassSmooth = false;
    #endif
    PBEqnCB().Cfg.iConvergeMeth = m_Method;
    PBEqnCB().Cfg.dEps_Rel = m_EPS_Rel ;
    PBEqnCB().Cfg.dEps_Abs = m_EPS_Abs ;
    PBEqnCB().Cfg.dDamping = m_Damping/100.0;
    PBEqnCB().Cfg.iMaxIters = m_MaxIters;
    PBEqnCB().Cfg.iRqdCnvrgdIters = m_RqdConvergedIters;
    SetModified(FALSE);
//    pSlvTool->bChanged=1;
    }
  }

//--------------------------------------------------------------------------

void CSolveCfgPage::OnDestroy() 
  {
  CCustomPropertyPage::OnDestroy();
  }

//--------------------------------------------------------------------------

void CSolveCfgPage::OnChangeData() 
  {
  SetModified(TRUE);
  }

//--------------------------------------------------------------------------

void CSolveCfgPage::OnEditchangeSlvMeth() 
  {
  SetModified(TRUE);
  UpdateData(TRUE);
  UpdateData(FALSE);
  }

void CSolveCfgPage::OnSelchangeSlvMeth() 
  {
  SetModified(TRUE);
  UpdateData(TRUE);
  UpdateData(FALSE);
  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_DYNCREATE(CSolveOptnPage, CPropertyPage)

CSolveOptnPage::CSolveOptnPage() : CCustomPropertyPage(CSolveOptnPage::IDD)
  {
	//{{AFX_DATA_INIT(CSolveOptnPage)
	m_Initialise=0;
	m_StatePinned=gs_pPrj->Solver.fStatePinned;
	m_IdleWhenDone=PBEqnCB().Cfg.fIdleWhenDone;
	m_ResetHist = gs_Exec.HstRestartOnStart();
	//}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

CSolveOptnPage::~CSolveOptnPage()
  {
  }

//--------------------------------------------------------------------------

BOOL CSolveOptnPage::OnInitDialog() 
  {
//  m_OneClick = pSlvTool->bOneClick;
  CCustomPropertyPage::OnInitDialog();
  
  return TRUE;
  }

//--------------------------------------------------------------------------

void CSolveOptnPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSolveOptnPage)
  DDX_Check(pDX, IDC_SLV_INITIALISE, m_Initialise);
  DDX_Check(pDX, IDC_SLV_PINRQD,     m_StatePinned);
	DDX_Check(pDX, IDC_SLV_IDLEWHENDONE, m_IdleWhenDone);
	DDX_Check(pDX, IDC_SLV_RESETHIST, m_ResetHist);
	//}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSolveOptnPage, CCustomPropertyPage)
	//{{AFX_MSG_MAP(CSolveOptnPage)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SLV_INITIALISE, OnChangeData)
	ON_BN_CLICKED(IDC_SLV_PINRQD, OnChangeData)
	ON_BN_CLICKED(IDC_SLV_IDLEWHENDONE, OnChangeData)
	ON_BN_CLICKED(IDC_SLV_RESETHIST, OnChangeData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

void CSolveOptnPage::OnOK() 
  {
  if (Inited())
    {
    bool ScrollTrendsToEnd = false;
    if (m_Initialise)
      {
      Strng_List RqdTags;
      gs_Exec.SetModelState(MSA_PBInit, RqdTags);
      ScrollTrendsToEnd = true;
      }
    if (ScrollTrendsToEnd)
      CTagVwDoc::AdjustTimebaseToEndAll();

  	gs_Exec.SetHstRestartOnStart(m_ResetHist!=0);
    //gs_pPrj->Solver.fStateWnd=m_StateWnd;
    gs_pPrj->Solver.fStatePinned=m_StatePinned;
    PBEqnCB().Cfg.fIdleWhenDone=m_IdleWhenDone;
    SetModified(FALSE);
    }
  }

//--------------------------------------------------------------------------

void CSolveOptnPage::OnDestroy() 
  {
	CCustomPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
  }

//--------------------------------------------------------------------------

void CSolveOptnPage::OnChangeData() 
  {
  SetModified(TRUE);
  }

//--------------------------------------------------------------------------
//===========================================================================
//
//
//
//===========================================================================
enum RunTimeTerms { FixFor=1, FixUntil=2, FixUpdate=4 };

IMPLEMENT_DYNCREATE(CRunCfgPage, CCustomPropertyPage)

CRunCfgPage::CRunCfgPage() : CCustomPropertyPage(CRunCfgPage::IDD)
  {
  iRunTerm=gs_Exec.RunTermination();
  dTimeUntilStop=gs_Exec.TimeUntilStop();
  m_CurrentTimeAtInit = gs_Exec.Time();
  m_PrevSyncWithClock = gs_Exec.SyncWithClock();
  m_CurrentTimeChgd=false;
  char buff[256];
  SecstoHMSDate(gs_Exec.Time(), buff, true);
  //{{AFX_DATA_INIT(CRunCfgPage)
  m_ForTime = _T("");
  m_UntilTime = _T("");
  m_ForIters = gs_Exec.MaxRunSteps();
  m_SyncWithClock = gs_Exec.SyncWithClock();
	m_RealTime = (gs_Exec.RealTime()!=0);
	m_RealTimeMult = gs_Exec.RealTimeMult();
	m_CurrentTimeHMS = buff;
	//}}AFX_DATA_INIT

  FixTimes(FixFor|FixUntil);
  }

//--------------------------------------------------------------------------

CRunCfgPage::~CRunCfgPage()
  {
  }

//--------------------------------------------------------------------------

BOOL CRunCfgPage::OnInitDialog() 
  {
  CCustomPropertyPage::OnInitDialog();

  if (m_SyncWithClock)
    {
  	m_RealTime = true;
	  m_RealTimeMult = 1.0;
    }
  GetDlgItem(IDC_REALTIME)->EnableWindow(!m_SyncWithClock);
  GetDlgItem(IDC_REALTIMEMULT)->EnableWindow(!m_SyncWithClock && m_RealTime);
  GetDlgItem(IDC_CURRENTTIME)->EnableWindow(!m_SyncWithClock);

  char buff[256];
  SecstoHMSDate(gs_Exec.Time(), buff, false);
  SetDlgItemText(IDC_DATETIME_TXT, buff);

  switch (iRunTerm)
    {
    case RTRM_For:         GetDlgItem(IDC_FORTIME)->SetFocus();    return TRUE;
    case RTRM_Until:       GetDlgItem(IDC_UNTILTIME)->SetFocus();  return TRUE;
    case RTRM_ForIters:    GetDlgItem(IDC_FORITERS)->SetFocus();   return TRUE;
    default: return TRUE;
    }
  return TRUE;
  }

//--------------------------------------------------------------------------

void CRunCfgPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRunCfgPage)
  DDX_Text(pDX, IDC_FORTIME, m_ForTime);
  DDX_Text(pDX, IDC_UNTILTIME, m_UntilTime);
  DDX_Text(pDX, IDC_FORITERS, m_ForIters);
  DDV_MinMaxLong(pDX, m_ForIters, 1, 2000000000);
	DDX_Check(pDX, IDC_SYNCHRONISECLOCK, m_SyncWithClock);
	DDX_Check(pDX, IDC_REALTIME, m_RealTime);
	DDX_Text(pDX, IDC_REALTIMEMULT, m_RealTimeMult);
	DDX_Text(pDX, IDC_CURRENTTIME, m_CurrentTimeHMS);
	//}}AFX_DATA_MAP
  
  if (!pDX->m_bSaveAndValidate)
    {
    ((CButton*)GetDlgItem(IDC_RUNFOREVER))->SetCheck(iRunTerm==RTRM_None);
    ((CButton*)GetDlgItem(IDC_RUNSTEADY))->SetCheck(iRunTerm==RTRM_SteadyState);
    ((CButton*)GetDlgItem(IDC_RUNFOR))->SetCheck(iRunTerm==RTRM_For);
    ((CButton*)GetDlgItem(IDC_RUNUNTIL))->SetCheck(iRunTerm==RTRM_Until);
    ((CButton*)GetDlgItem(IDC_RUNFORITERS))->SetCheck(iRunTerm==RTRM_ForIters);

    ((CEdit*)GetDlgItem(IDC_FORTIME))->EnableWindow(iRunTerm==RTRM_For);
    ((CEdit*)GetDlgItem(IDC_UNTILTIME))->EnableWindow(iRunTerm==RTRM_Until);
    ((CEdit*)GetDlgItem(IDC_FORITERS))->EnableWindow(iRunTerm==RTRM_ForIters);
    }
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CRunCfgPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CRunCfgPage)
  ON_BN_CLICKED(IDC_RUNFOR, OnRunfor)
  ON_BN_CLICKED(IDC_RUNFOREVER, OnRunforever)
  ON_BN_CLICKED(IDC_RUNSTEADY, OnRunsteady)
  ON_BN_CLICKED(IDC_RUNUNTIL, OnRununtil)
  ON_EN_CHANGE(IDC_FORTIME, OnChangeFortime)
  ON_EN_CHANGE(IDC_UNTILTIME, OnChangeUntiltime)
  ON_EN_KILLFOCUS(IDC_FORTIME, OnKillfocusFortime)
  ON_EN_KILLFOCUS(IDC_UNTILTIME, OnKillfocusUntiltime)
  ON_EN_SETFOCUS(IDC_FORTIME, OnSetfocusFortime)
  ON_EN_SETFOCUS(IDC_UNTILTIME, OnSetfocusUntiltime)
  ON_BN_CLICKED(IDC_RUNFORITERS, OnRunforiters)
  ON_EN_KILLFOCUS(IDC_FORITERS, OnKillfocusForiters)
  ON_EN_SETFOCUS(IDC_FORITERS, OnSetfocusForiters)
	ON_EN_CHANGE(IDC_FORITERS, OnChangeForiters)
	ON_BN_CLICKED(IDC_SYNCHRONISECLOCK, OnSyncWithClock)
	ON_BN_CLICKED(IDC_REALTIME, OnRealtime)
	ON_EN_CHANGE(IDC_REALTIMEMULT, OnChangeRealtimeMult)
  ON_EN_KILLFOCUS(IDC_CURRENTTIME, OnKillfocusCurrenttime)
	//}}AFX_MSG_MAP
  ON_EN_CHANGE(IDC_CURRENTTIME, OnEnChangeCurrenttime)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

void CRunCfgPage::OnOK() 
  {
  if (Inited())
    {
    gs_Exec.SetRunTermination(iRunTerm);
    gs_Exec.SetTimeUntilStop(dTimeUntilStop);
    gs_Exec.SetMaxRunSteps(m_ForIters);
    gs_Exec.SetSyncWithClock(m_SyncWithClock);
    if (m_SyncWithClock)
      {
    	m_RealTime = true;
	    m_RealTimeMult = 1.0;
      }
    gs_Exec.SetRealTime(m_RealTime);
    gs_Exec.SetRealTimeMult(Range(0.0001, m_RealTimeMult, 100000.0));

    double RqdTime;
    bool RestartHistorian = false;
    bool SetTime = false;
    bool ScrollTrendsToEnd = false;

    if (!m_SyncWithClock)
      {
      flag OK1 = HMSDatetoSecs(m_CurrentTimeHMS.GetBuffer(0), RqdTime);
      if (RqdTime<m_CurrentTimeAtInit && RqdTime>=m_CurrentTimeAtInit-0.002) // rounding errors in (2 msecs)
        RqdTime=m_CurrentTimeAtInit;
      if (OK1)
        {
        if (m_CurrentTimeChgd)
          {
          if (RqdTime<m_CurrentTimeAtInit)
            {
            switch (AfxMessageBox("Moving time Backwards!\n\nRestart Historian?", MB_YESNOCANCEL))
              {
              case IDYES: RestartHistorian = true; break;
              case IDNO: break;
              case IDCANCEL: RqdTime=m_CurrentTimeAtInit; break;
              }
            }
          if (RqdTime!=m_CurrentTimeAtInit)
            SetTime = true;
          m_CurrentTimeChgd = false;
          }
        }
      else
        AfxMessageBox("Invalid CurrentTime");
      }
    else
      {
      if (m_SyncWithClock!=m_PrevSyncWithClock)
        {
        m_PrevSyncWithClock = m_SyncWithClock!=0;
        RestartHistorian = true;
        }
      m_CurrentTimeChgd = false;
      }

    if (RestartHistorian)
      {
      gs_pPrj->CloseHistorian();
      gs_pPrj->RestartHistorian();
      gs_pPrj->OpenHistorian();
      ScrollTrendsToEnd = true;
      }
    if (SetTime)
      {
      gs_Exec.SetTime(RqdTime);
      ScrollTrendsToEnd = true;
      }
    if (ScrollTrendsToEnd)
      CTagVwDoc::AdjustTimebaseToEndAll();
    SetModified(FALSE);
    }
  }

//--------------------------------------------------------------------------

void CRunCfgPage::FixTimes(byte FixWhat)
  {
  switch (iRunTerm)
    {
    case RTRM_None:
      m_ForTime = _T("");
      m_UntilTime = _T("Stopped");
      break;
    case RTRM_SteadyState:
      m_ForTime = _T("");
      m_UntilTime = _T("Until Steady");
      break;
    case RTRM_ForIters:
      m_ForTime = _T("For N Steps");//Iterations");
      m_UntilTime = _T("");
      break;
    case RTRM_For:
    case RTRM_Until:
      {
      char Buff[1024];
      if (FixWhat & FixFor)
        m_ForTime = SecstoHMSCnv(dTimeUntilStop+0.005, Buff);
      if (FixWhat & FixUntil)
        m_UntilTime=SecstoHMSCnv(gs_Exec.Time()+dTimeUntilStop+0.005, Buff);
      break;
      }
    }

  if (FixWhat & FixUpdate)
    UpdateData(FALSE);
  SetModified(TRUE);
  }

//--------------------------------------------------------------------------

void CRunCfgPage::OnRunfor()
  {
  UpdateData(TRUE);
  if (iRunTerm==RTRM_For || iRunTerm==RTRM_Until)
    if (HMStoSecs(m_ForTime.GetBuffer(0), dTimeUntilStop))
      dTimeUntilStop=Max(0.01, dTimeUntilStop);
  iRunTerm=RTRM_For;
  FixTimes(FixFor|FixUntil|FixUpdate);
  SetModified(TRUE);
  }

void CRunCfgPage::OnRunforiters() 
  {
  iRunTerm=RTRM_ForIters;
  FixTimes(FixFor|FixUntil|FixUpdate);
  SetModified(TRUE);
  }

void CRunCfgPage::OnRunforever()
  {
  iRunTerm=RTRM_None;
  FixTimes(FixFor|FixUntil|FixUpdate);
  SetModified(TRUE);
  }

void CRunCfgPage::OnRunsteady()
  {
  iRunTerm=RTRM_SteadyState;
  FixTimes(FixFor|FixUntil|FixUpdate);
  SetModified(TRUE);
  }

void CRunCfgPage::OnRununtil()
  {
  UpdateData(TRUE);
  double UTime;
  if (iRunTerm==RTRM_For || iRunTerm==RTRM_Until)
    if (HMStoSecs(m_UntilTime.GetBuffer(0), UTime))
      dTimeUntilStop=Max(0.01, UTime-gs_Exec.Time());
  iRunTerm=RTRM_Until;
  FixTimes(FixFor|FixUntil|FixUpdate);
  SetModified(TRUE);
  }

void CRunCfgPage::OnSetfocusFortime() 
  {
  iRunTerm=RTRM_For;
  FixTimes(FixFor|FixUntil);//|FixUpdate);
//  if (Inited())
//    FixTimes(FixUpdate);
  SetModified(TRUE);
  }

void CRunCfgPage::OnSetfocusUntiltime() 
  {
  iRunTerm=RTRM_Until;
  FixTimes(FixFor|FixUntil);//|FixUpdate);
//  if (Inited())
//    FixTimes(FixUpdate);
  SetModified(TRUE);
  }

void CRunCfgPage::OnSetfocusForiters() 
  {
  iRunTerm=RTRM_ForIters;
  FixTimes(FixFor|FixUntil);//|FixUpdate);
//  if (Inited())
//    FixTimes(FixUpdate);
  SetModified(TRUE);
  }

void CRunCfgPage::OnKillfocusForiters() 
  {
  UpdateData(TRUE);
  }

void CRunCfgPage::OnKillfocusFortime()
  {
  UpdateData(TRUE);
  if (HMStoSecs(m_ForTime.GetBuffer(0), dTimeUntilStop))
    dTimeUntilStop=Max(0.01, dTimeUntilStop);
  FixTimes(FixFor|FixUntil|FixUpdate);
  }

void CRunCfgPage::OnKillfocusUntiltime()
  {
  UpdateData(TRUE);
  double UTime;
  if (HMStoSecs(m_UntilTime.GetBuffer(0), UTime))
    dTimeUntilStop=Max(0.01, UTime-gs_Exec.Time());
  FixTimes(FixFor|FixUntil|FixUpdate);
  }

void CRunCfgPage::OnChangeFortime()
  {
  UpdateData(TRUE);
  iRunTerm=RTRM_For;
  if (HMStoSecs(m_ForTime.GetBuffer(0), dTimeUntilStop))
    dTimeUntilStop=Max(0.01, dTimeUntilStop);
  FixTimes(FixUntil|FixUpdate);
  SetModified(TRUE);
  }

void CRunCfgPage::OnChangeUntiltime()
  {
  UpdateData(TRUE);
  iRunTerm=RTRM_Until;
  double UTime;
  if (HMStoSecs(m_UntilTime.GetBuffer(0), UTime))
    dTimeUntilStop=Max(0.01, UTime-gs_Exec.Time());
  FixTimes(FixFor|FixUpdate);
  SetModified(TRUE);
  }

void CRunCfgPage::OnChangeForiters() 
  {
  SetModified(TRUE);
  }

void CRunCfgPage::OnSyncWithClock() 
  {
  UpdateData(TRUE);
  if (m_SyncWithClock)
    {
  	m_RealTime = true;
	  m_RealTimeMult = 1.0;
    }
  GetDlgItem(IDC_REALTIME)->EnableWindow(!m_SyncWithClock);
  GetDlgItem(IDC_REALTIMEMULT)->EnableWindow(!m_SyncWithClock && m_RealTime);
  GetDlgItem(IDC_CURRENTTIME)->EnableWindow(!m_SyncWithClock);
  UpdateData(FALSE);
  SetModified(TRUE);
  }

void CRunCfgPage::OnRealtime() 
  {
  UpdateData(TRUE);
  GetDlgItem(IDC_REALTIMEMULT)->EnableWindow(m_RealTime);
  SetModified(TRUE);
  }

void CRunCfgPage::OnChangeRealtimeMult() 
  {
  SetModified(TRUE);
  }

void CRunCfgPage::OnKillfocusCurrenttime() 
  {
  UpdateData(TRUE);
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
      UpdateData(FALSE);
      }
    }
  }

void CRunCfgPage::OnEnChangeCurrenttime()
  {
  m_CurrentTimeChgd=true;
  }

void CRunCfgPage::OnCancel() 
  {
  // TODO: Add extra cleanup here
  //CRunCfgPage::OnOK(); // Catch Escape Button

  CDialog::OnCancel();
  }

//===========================================================================
//
//
//
//===========================================================================
// CRunMethodPage property page
const double MaxIterStep = 86400.0; //24*60*60

IMPLEMENT_DYNCREATE(CRunMethodPage, CCustomPropertyPage)

CRunMethodPage::CRunMethodPage() : CCustomPropertyPage(CRunMethodPage::IDD)
  {
  //{{AFX_DATA_INIT(CRunMethodPage)
  m_Abs_Eps = FE()->FE_ODE_GetMagEPSAbs();
  m_Rel_Eps = FE()->FE_ODE_GetMagEPSRel();
  m_Step_Cur = FE()->FE_ODE_GetTimeInc();
  m_Step_Max = FE()->FE_ODE_GetTimeIncMxRqd();
  m_Step_Min = FE()->FE_ODE_GetTimeIncMnRqd();
  m_MaxStepTime = gs_Exec.StepSizeMax();
  m_StepCount = long(m_MaxStepTime/m_Step_Cur);
  //}}AFX_DATA_INIT
  m_iMethod=FE()->FE_ODE_Method();
  m_iStep=FE()->FE_ODE_StepSizeControl();
  }

CRunMethodPage::~CRunMethodPage()
  {
  }

//--------------------------------------------------------------------------

void CRunMethodPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRunMethodPage)
  DDX_Text(pDX, IDC_ODE_ABS_EPS, m_Abs_Eps);
  DDX_Text(pDX, IDC_ODE_REL_EPS, m_Rel_Eps);
  DDX_Text(pDX, IDC_ODE_STEP_CUR, m_Step_Cur);
  DDV_MinMaxDouble(pDX, m_Step_Cur, 0.00001, MaxIterStep);
  DDX_Text(pDX, IDC_ODE_STEP_MAX, m_Step_Max);
  DDV_MinMaxDouble(pDX, m_Step_Max, 0.00001, MaxIterStep);
  DDX_Text(pDX, IDC_ODE_STEP_MIN, m_Step_Min);
  DDV_MinMaxDouble(pDX, m_Step_Min, 0.00001, MaxIterStep);
  DDX_Text(pDX, IDC_STEPTIME, m_MaxStepTime);
  DDV_MinMaxDouble(pDX, m_MaxStepTime, 0.00001, MaxIterStep);
  DDX_Text(pDX, IDC_STEPCOUNT, m_StepCount);
  //}}AFX_DATA_MAP

  CComboBox * pMethLst=(CComboBox *)GetDlgItem(IDC_ODE_METH);
  CComboBox * pStepLst=(CComboBox *)GetDlgItem(IDC_ODE_STEP);

  if (pDX->m_bSaveAndValidate)
    {
    m_iMethod=pMethLst->GetCurSel();
    m_iStep=pStepLst->GetCurSel();
    }
  else
    {
    pMethLst->ResetContent();
    pMethLst->InsertString(ODE_Euler, "Euler");
    pMethLst->InsertString(ODE_RK2,   "Runge-Kutta 2");
    pMethLst->InsertString(ODE_RK4,   "Runge-Kutta 4");
    pMethLst->SetCurSel(m_iMethod);
    pStepLst->ResetContent();
    pStepLst->InsertString(0, "Fixed");
    pStepLst->InsertString(1, "Variable");
    pStepLst->SetCurSel(m_iStep);
    }
  GetDlgItem(IDC_ODE_ABS_EPS)->EnableWindow(m_iMethod!=ODE_Euler);
  GetDlgItem(IDC_ODE_REL_EPS)->EnableWindow(m_iMethod!=ODE_Euler);
  GetDlgItem(IDC_ODE_STEP_MIN)->EnableWindow(m_iStep!=0);
  GetDlgItem(IDC_ODE_STEP_MAX)->EnableWindow(m_iStep!=0);
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CRunMethodPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CRunMethodPage)
	ON_EN_CHANGE(IDC_ODE_ABS_EPS, OnUpdateOdeAbsEps)
	ON_EN_UPDATE(IDC_ODE_REL_EPS, OnUpdateOdeRelEps)
	ON_EN_UPDATE(IDC_ODE_STEP_CUR, OnUpdateOdeStepCur)
	ON_EN_UPDATE(IDC_ODE_STEP_MAX, OnUpdateOdeStepMax)
	ON_EN_UPDATE(IDC_ODE_STEP_MIN, OnUpdateOdeStepMin)
	ON_EN_CHANGE(IDC_ODE_STEP_CUR, OnChangeOdeStepCur)
	ON_EN_CHANGE(IDC_STEPTIME, OnChangeMaxStepTime)
	ON_EN_KILLFOCUS(IDC_STEPTIME, OnKillfocusMaxStepTime)
	ON_EN_CHANGE(IDC_STEPCOUNT, OnChangeStepCount)
	ON_CBN_SELCHANGE(IDC_ODE_METH, OnSelchangeOdeMeth)
	ON_CBN_SELCHANGE(IDC_ODE_STEP, OnSelchangeOdeStep)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CRunMethodPage::OnInitDialog() 
  {
  CCustomPropertyPage::OnInitDialog();

  return TRUE;
  }

//--------------------------------------------------------------------------

void CRunMethodPage::OnOK() 
  {
  if (Inited())
    {
    m_Step_Cur = Range(0.00001, m_Step_Cur, MaxIterStep);
    if (m_iStep==0 && m_Step_Max>m_Step_Cur)
      m_Step_Max = m_Step_Cur;
    if (m_Step_Min>m_Step_Max)
      m_Step_Min = m_Step_Max;
    if (m_iStep==0)
      {
      if (m_MaxStepTime<m_Step_Cur)
        m_MaxStepTime = m_Step_Cur;
      m_MaxStepTime = floor(m_MaxStepTime/m_Step_Cur*1.001)*m_Step_Cur;
      }
    else
      {
      if (m_MaxStepTime<m_Step_Min)
        m_MaxStepTime = m_Step_Min;
      }
    FE()->FE_ODE_SetMagEPSAbs(m_Abs_Eps);
    FE()->FE_ODE_SetMagEPSRel(m_Rel_Eps);
    FE()->FE_ODE_SetTimeInc(m_Step_Cur);
    FE()->FE_ODE_SetTimeIncMxRqd(m_Step_Max);
    FE()->FE_ODE_SetTimeIncMnRqd(m_Step_Min);
    if (m_iMethod!=CB_ERR)
      FE()->FE_ODE_SetMethod(m_iMethod);
    if (m_iStep!=CB_ERR)
      FE()->FE_ODE_SetStepSizeControl(m_iStep);

    gs_Exec.SetStepSizeMax(Range(0.00001, m_MaxStepTime, MaxIterStep));
    }

  CCustomPropertyPage::OnOK();
  }

//--------------------------------------------------------------------------

void CRunMethodPage::OnUpdateOdeAbsEps() 
  {
  SetModified(TRUE);
  }

void CRunMethodPage::OnUpdateOdeRelEps() 
  {
  SetModified(TRUE);
  }

void CRunMethodPage::OnUpdateOdeStepCur() 
  {
  SetModified(TRUE);
  }

void CRunMethodPage::OnUpdateOdeStepMax() 
  {
  SetModified(TRUE);
  }

void CRunMethodPage::OnUpdateOdeStepMin() 
  {
  SetModified(TRUE);
  }

void CRunMethodPage::OnChangeOdeStepCur() 
  {
  SetModified(TRUE);
  CString s;
  GetDlgItem(IDC_ODE_STEP_CUR)->GetWindowText(s);
  if (s.GetLength()>0)
    {
    UpdateData(TRUE);
    m_Step_Cur = Range(0.00001, m_Step_Cur, MaxIterStep);
    m_StepCount = long(m_MaxStepTime/m_Step_Cur);
    UpdateData(FALSE);
    }
  }

void CRunMethodPage::OnChangeMaxStepTime() 
  {
  SetModified(TRUE);
  CString s;
  GetDlgItem(IDC_STEPTIME)->GetWindowText(s);
  if (s.GetLength()>0)
    {
    UpdateData(TRUE);
    m_MaxStepTime = Range(0.00001, m_MaxStepTime, MaxIterStep);
    m_StepCount = long(m_MaxStepTime/m_Step_Cur);
    UpdateData(FALSE);
    }
  }
void CRunMethodPage::OnKillfocusMaxStepTime() 
  {
  UpdateData(TRUE);
  m_MaxStepTime = Range(0.00001, m_MaxStepTime, MaxIterStep);
  if (m_iStep==0)
    {
    if (m_MaxStepTime<m_Step_Cur)
      m_MaxStepTime = m_Step_Cur;
    m_MaxStepTime = floor(m_MaxStepTime/m_Step_Cur*1.001)*m_Step_Cur;
    }
  else
    {
    if (m_MaxStepTime<m_Step_Min)
      m_MaxStepTime = m_Step_Min;
    }
  m_StepCount = long(m_MaxStepTime/m_Step_Cur);
  UpdateData(FALSE);
  }

void CRunMethodPage::OnChangeStepCount() 
  {
  SetModified(TRUE);
  CString s;
  GetDlgItem(IDC_STEPCOUNT)->GetWindowText(s);
  if (s.GetLength()>0)
    {
    UpdateData(TRUE);
    m_StepCount = Range(1L, m_StepCount, 1000L);
    m_MaxStepTime = m_Step_Cur*m_StepCount;
    UpdateData(FALSE);
    }
  }

void CRunMethodPage::OnSelchangeOdeMeth() 
  {
  CComboBox * pMethLst=(CComboBox *)GetDlgItem(IDC_ODE_METH);
  CComboBox * pStepLst=(CComboBox *)GetDlgItem(IDC_ODE_STEP);
  const int Sel = pMethLst->GetCurSel();
  if (Sel==ODE_Euler)
    pStepLst->SetCurSel(0); //for Euler force fixed step size
  //pStepLst->EnableWindow(m_iMethod!=ODE_Euler);
  GetDlgItem(IDC_ODE_ABS_EPS)->EnableWindow(Sel!=ODE_Euler);
  GetDlgItem(IDC_ODE_REL_EPS)->EnableWindow(Sel!=ODE_Euler);
  GetDlgItem(IDC_ODE_STEP_MIN)->EnableWindow(pStepLst->GetCurSel()!=0);
  GetDlgItem(IDC_ODE_STEP_MAX)->EnableWindow(pStepLst->GetCurSel()!=0);

  SetModified(TRUE);
  }

void CRunMethodPage::OnSelchangeOdeStep() 
  {
  CComboBox * pMethLst=(CComboBox *)GetDlgItem(IDC_ODE_METH);
  CComboBox * pStepLst=(CComboBox *)GetDlgItem(IDC_ODE_STEP);
  const int Sel = pMethLst->GetCurSel();
  if (Sel==ODE_Euler)
    pStepLst->SetCurSel(0);
  GetDlgItem(IDC_ODE_STEP_MIN)->EnableWindow(pStepLst->GetCurSel()!=0);
  GetDlgItem(IDC_ODE_STEP_MAX)->EnableWindow(pStepLst->GetCurSel()!=0);

  SetModified(TRUE);
  }

//===========================================================================
/////////////////////////////////////////////////////////////////////////////
// CRunNetworkPage property page

IMPLEMENT_DYNCREATE(CRunNetworkPage, CCustomPropertyPage)

CRunNetworkPage::CRunNetworkPage() : CCustomPropertyPage(CRunNetworkPage::IDD)
  {
  //{{AFX_DATA_INIT(CRunNetworkPage)
  m_LnkAbs = FE()->FE_Net_GetLnkEPSAbs();
  m_LnkRel = FE()->FE_Net_GetLnkEPSRel();
  m_NetDerate = FE()->FE_Net_GetNetDeRate();
  m_SpillArea = FE()->FE_Net_GetSpillArea();
  //}}AFX_DATA_INIT
  }

CRunNetworkPage::~CRunNetworkPage()
  {
  }

void CRunNetworkPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRunNetworkPage)
  DDX_Text(pDX, IDC_RUN_LNKABS, m_LnkAbs);
  DDV_MinMaxDouble(pDX, m_LnkAbs, 1.e-010, 1.e-002);
  DDX_Text(pDX, IDC_RUN_LNKREL, m_LnkRel);
  DDV_MinMaxDouble(pDX, m_LnkRel, 1.e-010, 1.e-004);
  DDX_Text(pDX, IDC_RUN_NETDERATE, m_NetDerate);
  DDV_MinMaxDouble(pDX, m_NetDerate, 2., 100.);
  DDX_Text(pDX, IDC_RUNSPILLAREA, m_SpillArea);
  DDV_MaxChars(pDX, m_SpillArea, 100);
  //}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CRunNetworkPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CRunNetworkPage)
	ON_EN_CHANGE(IDC_RUN_LNKABS, OnChangeRunLnkabs)
	ON_EN_CHANGE(IDC_RUN_LNKREL, OnChangeRunLnkrel)
	ON_EN_CHANGE(IDC_RUN_NETDERATE, OnChangeRunNetderate)
	ON_EN_CHANGE(IDC_RUNSPILLAREA, OnChangeRunspillarea)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CRunNetworkPage::OnInitDialog() 
  {
  CCustomPropertyPage::OnInitDialog();
  // TODO: Add extra initialization here
  return TRUE;
  }

void CRunNetworkPage::OnOK() 
  {
  // TODO: Add your specialized code here and/or call the base class
  if (Inited())
    {
    FE()->FE_Net_SetLnkEPSAbs(m_LnkAbs   );
    FE()->FE_Net_SetLnkEPSRel(m_LnkRel   );
    FE()->FE_Net_SetNetDeRate(m_NetDerate);
    FE()->FE_Net_SetSpillArea(m_SpillArea.GetBuffer(0));
    }
  CCustomPropertyPage::OnOK();
  SetModified(FALSE);
  }


void CRunNetworkPage::OnChangeRunLnkabs() 
  {
  SetModified(TRUE);
  }

void CRunNetworkPage::OnChangeRunLnkrel() 
  {
  SetModified(TRUE);
  }

void CRunNetworkPage::OnChangeRunNetderate() 
  {
  SetModified(TRUE);
  }

void CRunNetworkPage::OnChangeRunspillarea() 
  {
  SetModified(TRUE);
  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_DYNCREATE(CRunTearPage, CCustomPropertyPage)

CRunTearPage::CRunTearPage() : CCustomPropertyPage(CRunTearPage::IDD)
  {
  //{{AFX_DATA_INIT(CRunTearPage)
  m_EPS_Rel = 1;
  m_EPS_Abs = 1;
  m_MaxIters = 1;
  m_Damping = 10.0;
  //}}AFX_DATA_INIT
  m_Method=TCM_AdaptSubs;
  //bDidInit = False;

  //	m_Iters = 0;
//  m_State = _T("");
  }

//--------------------------------------------------------------------------

CRunTearPage::~CRunTearPage()
  {
  //bDidInit = False;
  }

//--------------------------------------------------------------------------

BOOL CRunTearPage::OnInitDialog() 
  {
  m_EPS_Rel = DynEqnCB().Cfg.dEps_Rel;
  m_EPS_Abs = DynEqnCB().Cfg.dEps_Abs;
  m_MaxIters = DynEqnCB().Cfg.iMaxIters;
  m_Damping = DynEqnCB().Cfg.dDamping*100.0;
  m_Method  = DynEqnCB().Cfg.iConvergeMeth;
  //InitDialog();//bDidInit = True;
  CCustomPropertyPage::OnInitDialog();

  //CComboBox * pMethLst=(CComboBox *)GetDlgItem(IDC_RUNTEAR_METH);
  //m_EPS_Rel = DynEqnCB().Cfg.dEps_Rel;
  //m_EPS_Abs = DynEqnCB().Cfg.dEps_Abs;
  //m_MaxIters = DynEqnCB().Cfg.nMaxIters;
  //m_Damping = DynEqnCB().Cfg.dDamping;
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//--------------------------------------------------------------------------

void CRunTearPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRunTearPage)
  DDX_Text(pDX, IDC_RUNTEAR_REL, m_EPS_Rel);
  DDV_MinMaxDouble(pDX, m_EPS_Rel, 1.e-010, 0.1);
  DDX_Text(pDX, IDC_RUNTEAR_ABS, m_EPS_Abs);
  DDV_MinMaxDouble(pDX, m_EPS_Abs, 1.e-010, 1.);
  DDX_Text(pDX, IDC_RUNTEAR_MAXITERS, m_MaxIters);
  DDV_MinMaxInt(pDX, m_MaxIters, 3, 20000);
  DDX_Text(pDX, IDC_RUNTEAR_DAMPING, m_Damping);
  DDV_MinMaxDouble(pDX, m_Damping, 0., 100.);
  //}}AFX_DATA_MAP
  
  CComboBox * pMethLst=(CComboBox *)GetDlgItem(IDC_RUNTEAR_METH);
  if (pDX->m_bSaveAndValidate)
    {
    m_Method=Range(TCM_DirectSubs, (byte)(pMethLst->GetCurSel()+1), TCM_Wegstein);
    }
  else
    {
    pMethLst->ResetContent();
    pMethLst->InsertString(TCM_DirectSubs-1, "Direct Substitution");
    pMethLst->InsertString(TCM_AdaptSubs-1, "Adaptive Direct Subs");
    pMethLst->InsertString(TCM_Wegstein-1, "Wegstein Acceleration");
    pMethLst->SetCurSel(m_Method-1);
    CWnd * pDampText=GetDlgItem(IDC_RUNTEAR_DAMPINGTXT);
    CWnd * pDamp=GetDlgItem(IDC_RUNTEAR_DAMPING);
    switch (m_Method)
      {
      case TCM_DirectSubs: pDamp->EnableWindow(TRUE); pDampText->SetWindowText("Damping (%)"); break;
      case TCM_AdaptSubs: pDamp->EnableWindow(TRUE); pDampText->SetWindowText("Minimum Damping (%)"); break;
      case TCM_Wegstein:pDamp->EnableWindow(FALSE); pDampText->SetWindowText(""); break;
      }
    //pDampText->RedrawWindow();
    }

  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CRunTearPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CRunTearPage)
  ON_WM_DESTROY()
  ON_EN_CHANGE(IDC_RUNTEAR_ABS, OnChangeSlvAbs)
  ON_EN_CHANGE(IDC_RUNTEAR_DAMPING, OnChangeSlvDamping)
  ON_EN_CHANGE(IDC_RUNTEAR_MAXITERS, OnChangeSlvMaxiters)
  ON_CBN_EDITCHANGE(IDC_RUNTEAR_METH, OnEditchangeSlvMeth)
  ON_EN_CHANGE(IDC_RUNTEAR_REL, OnChangeSlvRel)
  ON_CBN_SELCHANGE(IDC_RUNTEAR_METH, OnSelchangeSlvMeth)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

void CRunTearPage::OnOK() 
  {
  if (Inited())
    {
    //gs_pPrj->lStatsRefreshCnt = m_RefreshCnt;
    CComboBox * pMethLst=(CComboBox *)GetDlgItem(IDC_RUNTEAR_METH);
    DynEqnCB().Cfg.iConvergeMeth=m_Method;
    DynEqnCB().Cfg.dEps_Rel = m_EPS_Rel ;
    DynEqnCB().Cfg.dEps_Abs = m_EPS_Abs ;
    DynEqnCB().Cfg.dDamping = m_Damping/100.0;
    DynEqnCB().Cfg.iMaxIters = m_MaxIters;
    SetModified(FALSE);
//    pSlvTool->bChanged=1;
    }
  }

//--------------------------------------------------------------------------

void CRunTearPage::OnDestroy() 
  {
  CCustomPropertyPage::OnDestroy();
  }

void CRunTearPage::OnChangeSlvAbs() 
  {
  SetModified(TRUE);
  }

void CRunTearPage::OnChangeSlvDamping() 
  {
  SetModified(TRUE);
  }

void CRunTearPage::OnChangeSlvMaxiters() 
  {
  SetModified(TRUE);
  }

void CRunTearPage::OnEditchangeSlvMeth() 
  {
  SetModified(TRUE);
  UpdateData(TRUE);
  UpdateData(FALSE);
  }

void CRunTearPage::OnSelchangeSlvMeth() 
  {
  SetModified(TRUE);
  UpdateData(TRUE);
  UpdateData(FALSE);
  }

void CRunTearPage::OnChangeSlvRel() 
  {
  SetModified(TRUE);
  }

//===========================================================================
//
//
//
//===========================================================================
IMPLEMENT_DYNCREATE(CRunOptionPage, CCustomPropertyPage)

CRunOptionPage::CRunOptionPage() : CCustomPropertyPage(CRunOptionPage::IDD)
, m_bEmptySpills(gs_pPrj->m_bRunOptEmptySpills)
, m_bEmptyAll(gs_pPrj->m_bRunOptEmptyAll)
, m_bZeroFlows(gs_pPrj->m_bRunOptZeroFlows)
, m_bInitialiseAll(gs_pPrj->m_bRunOptInitialiseAll)
, m_bResetStats(gs_pPrj->m_bRunOptResetStats)
, m_bOptnsOnce(gs_pPrj->m_bRunOptOnceOnly)
  {
  char buff[256];
  SecstoHMSDate(gs_Exec.OnStartTime(), buff, true);
  //{{AFX_DATA_INIT(CRunOptionPage)
  m_MomentumOn = FE()->FE_FlwMomentumOn();
	m_ResetHist = gs_Exec.HstRestartOnStart();
	m_ResetTime = gs_Exec.ResetTimeOnStart();
	m_ResetTimeOnStart = buff;
	//}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

CRunOptionPage::~CRunOptionPage()
  {
  }

//--------------------------------------------------------------------------

BOOL CRunOptionPage::OnInitDialog() 
  {
  CCustomPropertyPage::OnInitDialog();

  GetDlgItem(IDC_RUN_RESETTIME)->EnableWindow(m_ResetHist && !gs_Exec.SyncWithClock());
  GetDlgItem(IDC_RESETTIMEONSTART)->EnableWindow(m_ResetHist && m_ResetTime && !gs_Exec.SyncWithClock());

  return TRUE;
  }

//--------------------------------------------------------------------------

void CRunOptionPage::DoDataExchange(CDataExchange* pDX)
  {
  CCustomPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRunOptionPage)
  DDX_Check(pDX, IDC_RUN_FLWMOMENTON, m_MomentumOn);
  DDX_Check(pDX, IDC_RUN_RESETHIST, m_ResetHist);
  DDX_Check(pDX, IDC_RUN_RESETTIME, m_ResetTime);
	DDX_Text(pDX, IDC_RESETTIMEONSTART, m_ResetTimeOnStart);
  //}}AFX_DATA_MAP
  DDX_Check(pDX, IDC_RUN_EMPTYSPILLS, m_bEmptySpills);
  DDX_Check(pDX, IDC_RUN_EMPTYALL, m_bEmptyAll);
  DDX_Check(pDX, IDC_RUN_ZEROFLOWS, m_bZeroFlows);
  DDX_Check(pDX, IDC_RUN_INITIALALL, m_bInitialiseAll);
  DDX_Check(pDX, IDC_RUN_RESETSTATS, m_bResetStats);
  DDX_Check(pDX, IDC_RUN_OPTSONCE, m_bOptnsOnce);
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CRunOptionPage, CCustomPropertyPage)
  //{{AFX_MSG_MAP(CRunOptionPage)
  ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RUN_RESETHIST, OnRunResetTime)
	ON_BN_CLICKED(IDC_RUN_RESETTIME, OnRunResetTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

void CRunOptionPage::OnOK() 
  {
  if (Inited())
    {
    gs_pPrj->m_bRunOptEmptySpills   = m_bEmptySpills;
    gs_pPrj->m_bRunOptEmptyAll      = m_bEmptyAll;     
    gs_pPrj->m_bRunOptZeroFlows     = m_bZeroFlows;    
    gs_pPrj->m_bRunOptInitialiseAll = m_bInitialiseAll;
    gs_pPrj->m_bRunOptResetStats    = m_bResetStats;   
    gs_pPrj->m_bRunOptOnceOnly      = m_bOptnsOnce;    

    FE()->FE_SetFlwMomentumOn(m_MomentumOn);

  	gs_Exec.SetHstRestartOnStart(m_ResetHist!=0);
    if (gs_Exec.SyncWithClock() || !m_ResetHist)
      m_ResetTime = false;
	  gs_Exec.SetResetTimeOnStart(m_ResetTime!=0);
    if (m_ResetTime)
      {
      double RqdTime;
      flag OK = HMSDatetoSecs(m_ResetTimeOnStart.GetBuffer(0), RqdTime);
      if (OK)
        gs_Exec.SetOnStartTime(Max(0.0, RqdTime));
      else
        AfxMessageBox("Invalid time at start");
      }
    
    SetModified(FALSE);
    }
  }

//--------------------------------------------------------------------------

void CRunOptionPage::OnDestroy() 
  {
  CCustomPropertyPage::OnDestroy();
  }

//--------------------------------------------------------------------------

void CRunOptionPage::OnRunResetTime() 
  {
  UpdateData(TRUE);
  if (!m_ResetHist)
    m_ResetTime = false;
  GetDlgItem(IDC_RUN_RESETTIME)->EnableWindow(m_ResetHist && !gs_Exec.SyncWithClock());
  GetDlgItem(IDC_RESETTIMEONSTART)->EnableWindow(m_ResetHist && m_ResetTime && !gs_Exec.SyncWithClock());
  UpdateData(FALSE);
  }

//===========================================================================
//===========================================================================
//
//
//
//===========================================================================

int CSolverCfgSheet::iCurrentPage=0;

IMPLEMENT_DYNAMIC(CSolverCfgSheet, CPropertySheet)

CSolverCfgSheet::CSolverCfgSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
  :CPropertySheet(nIDCaption, pParentWnd, iSelectPage)//,
  {
  ASSERT(FALSE); //DO NOT EXPECT TO GET HERE !!!
  }

//---------------------------------------------------------------------------

CSolverCfgSheet::CSolverCfgSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
  :CPropertySheet(pszCaption, pParentWnd, iSelectPage)//,
  {
  EnableStackedTabs(FALSE);

  if (XDoingPB())
    {
    Pages[0] = new CSolveCfgPage;
    Pages[1] = new CActivePage;
    Pages[2] = new CSolveOptnPage;
    for (int i=3; i<CfgFixedPages; i++)
      Pages[i] = NULL;
    }
  else
    {
    Pages[0] = new CRunCfgPage;
    Pages[1] = new CRunMethodPage;
    Pages[2] = new CRunNetworkPage;
    Pages[3] = new CRunTearPage;
    Pages[4] = new CActivePage;
    Pages[5] = new CRunOptionPage;
    }
  for (int i=0; i<CfgFixedPages; i++)
    if (Pages[i])
      AddPage(Pages[i]);
  //ASSERT(GetPageCount()==CfgFixedPages);
  }

//---------------------------------------------------------------------------

CSolverCfgSheet::~CSolverCfgSheet()
  {
  for (int i=0; i<CfgFixedPages; i++)
    delete Pages[i];
  }

//---------------------------------------------------------------------------

BOOL CSolverCfgSheet::OnInitDialog() 
  {
  CWnd* pOKButton = GetDlgItem(IDOK);
  SetWndDef(pOKButton->m_hWnd);

  //We want buttons on a modeless property sheet, make MFC code work; see dlgprop.cpp...
  BOOL b;
  if (m_bModeless)
    {//we want a modeless dialog...
    PROPSHEETTRICK0()
    BOOL b = CPropertySheet::OnInitDialog();
    PROPSHEETTRICK1()
    }
  else
    b = CPropertySheet::OnInitDialog();

  GetDlgItem(IDHELP)->ShowWindow(SW_HIDE);
  return b;
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSolverCfgSheet, CPropertySheet)
  //{{AFX_MSG_MAP(CSolverCfgSheet)
  ON_WM_NCDESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CSolverCfgSheet::UpdateOptMeth(byte iReqdOptMeth)
  {
  //while (GetPageCount()>CfgFixedPages)
  //  RemovePage(GetPage(CfgFixedPages));
  }

//---------------------------------------------------------------------------

BOOL CSolverCfgSheet::OnCommand(WPARAM wParam, LPARAM lParam) 
  {
  iCurrentPage=GetActiveIndex();
  return CPropertySheet::OnCommand(wParam, lParam);
  }

//===========================================================================
//
//
//
//===========================================================================

//IMPLEMENT_DYNCREATE(CBalReportPage, CPropertyPage)
//
//CBalReportPage::CBalReportPage() : CCustomPropertyPage(CBalReportPage::IDD)
//  {
//	//{{AFX_DATA_INIT(CBalReportPage)
//	m_Errors = TRUE;
//	m_Sinks = FALSE;
//	m_Sources = FALSE;
//	m_Species = FALSE;
//	//}}AFX_DATA_INIT
//  //bDidInit = False;
//  bListChgd=1;
//  InitializeCriticalSection(&ListLock);
//  }
//
////--------------------------------------------------------------------------
//
//CBalReportPage::~CBalReportPage()
//  {
//  //bDidInit = False;
//  DeleteCriticalSection(&ListLock);
//  }
//
////--------------------------------------------------------------------------
//
//BOOL CBalReportPage::OnInitDialog() 
//  {
//  InitDialog();//bDidInit = True;
//	CCustomPropertyPage::OnInitDialog();
//
//  DoBalReport();
//
//  UpdateBalReportList();
//	
//	return TRUE;  // return TRUE unless you set the focus to a control
//	              // EXCEPTION: OCX Property Pages should return FALSE
//  }
//
////--------------------------------------------------------------------------
//
//void CBalReportPage::DoDataExchange(CDataExchange* pDX)
//  {
//	CCustomPropertyPage::DoDataExchange(pDX);
//	//{{AFX_DATA_MAP(CBalReportPage)
//	DDX_Check(pDX, IDC_CHECK_ERRORS, m_Errors);
//	DDX_Check(pDX, IDC_CHECK_SINKS, m_Sinks);
//	DDX_Check(pDX, IDC_CHECK_SOURCES, m_Sources);
//	DDX_Check(pDX, IDC_CHECK_SPECIES, m_Species);
//	//}}AFX_DATA_MAP
//  
//  if (!pDX->m_bSaveAndValidate)
//    UpdateBalReportList();
//
//  }
//
////--------------------------------------------------------------------------
//
//BEGIN_MESSAGE_MAP(CBalReportPage, CCustomPropertyPage)
//	//{{AFX_MSG_MAP(CBalReportPage)
//	ON_WM_DESTROY()
//	ON_BN_CLICKED(IDC_REPORT, OnBalReport)
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()
//
////--------------------------------------------------------------------------
//
//void CBalReportPage::OnOK() 
//  {
//  if (Inited())
//    {
//    //gs_pPrj->lStatsRefreshCnt = m_RefreshCnt;
//    }
//  }
//
////--------------------------------------------------------------------------
//
//void CBalReportPage::OnDestroy() 
//  {
//	CCustomPropertyPage::OnDestroy();
//	
//	// TODO: Add your message handler code here
//	
//  }
//
////--------------------------------------------------------------------------
//
//void CBalReportPage::UpdateBalReportList() 
//  {
//  if (!bListChgd)
//    return;
//  
//  bListChgd=0;
//
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_REPORTLIST);
//  pLst->ResetContent();
//
//  if (!gs_Exec.SolvingPB() || gs_Exec.Waiting())//pSlvTool->bSolutionBusy)
//    {
//    pLst->AddString("Solution Busy");
//    return;
//    }
//  int w[20];
//  int Same[20];
//  char * pFld[20];
//  for (int i=0; i<20; i++)
//    {
//    w[i]=0;
//    pFld[i]=NULL;
//    }
//  
//  //CClientDC DC(this);
//  EnterCriticalSection(&ListLock);
//  for (pStrng pS = m_List.First(); pS; pS = m_List.Next())
//    {
//
//    if (pS->XStrChr('\t'))
//      {
//      char buff[4069];
//      buff[0]=0;
//      char *p0=pS->Str(), *p1;
//      int i=0, l=0;
//      while ((p1=strchr(p0, '\t'))!=NULL)
//        {
//        *p1=0;
//        XStrTrim(p0);
//        w[i]=Max(w[i], (int)strlen(p0));
//        Same[i]=(pFld[i]!=NULL) && (strcmp(pFld[i], p0)==0);
//        pFld[i]=p0;
//        //CSize Sz=DC.GetTextExtent(pFld[i], strlen(pFld[i]));
//        p0=p1+1;
//        i++;
//        }
//      XStrTrim(p0);
//      w[i]=Max(w[i], (int)strlen(p0));
//      pFld[i]=p0;
//      Same[i]=0;
//    
//      l=0;
//      if (!Same[0] || !Same[1])
//        l=sprintf(buff, "%s\t%s\t", pFld[0], pFld[1]);
//      else
//        l=sprintf(buff, "  \t  \t"); 
//      for (int j=2; j<i; j++)
//        l+=sprintf(&buff[l], "%s\t", pFld[j]); 
//      l+=sprintf(&buff[l], "%s", pFld[j]); 
//      //dbgpln(" Add to CList %s",buff);
//      pLst->AddString(buff);
//      }
//    else
//      {
//      //dbgpln(" Add to CList %s",pS->Str());
//      pLst->AddString(pS->Str());
//      }
//    }
//  LeaveCriticalSection(&ListLock);
//
//  for (i=0; i<20; i++)
//    {
//    if (w[i]==0) 
//      break;
//    w[i]=(w[i]+2)*4;
//    if (i>0)
//      w[i]+=w[i-1];
//    }
//  pLst->SetTabStops(i, w);
//  //pLst->SetCurSel(iSelTarget);
//  
//  }
//
////--------------------------------------------------------------------------
//
//void CBalReportPage::OnBalReport() 
//  {
//  UpdateData(TRUE);
//  DoBalReport();
//  pSlvTool->SendMessage(WMU_PB_UPDATEDATA, 0, 0);
//  }
//
////--------------------------------------------------------------------------
//
//void CBalReportPage::DoBalReport() 
//  {
//	// TODO: Add your control notification handler code here
//  //EqnSlvCtrlBlk &CB = pSlvTool->SolverPage.SlvCB; 
//  EqnSlvCtrlBlk &CB = SlvCB(); 
//
////  pCExecObj pObj=NULL;
////  EnterCriticalSection(&ListLock);
////  m_List.Clear();
////  for (int i=0; gs_Exec.GetSolutionObj(i, pObj); i++) 
////    {
////    CB.dReportTol=1.0e-5;
////    CB.iReport=0;
////    if (m_Sinks)   CB.iReport |= ESR_Snks;
////    if (m_Sources) CB.iReport |= ESR_Srcs;   
////    if (m_Errors)  CB.iReport |= ESR_Errors;
////    if (m_Species) CB.iReport |= ESR_Species;
////
////    pStrng pS=new Strng;
////    pStrng pPrev=NULL;
////    while (pObj->EO_GetSolutionError(CB, *pS)>=0)
////      {
////      CB.iReport=0; // Prevent new report begin generated
////      pchar p=pS->XStrChr('\t');
////      if (pPrev && (pS->StrNCmp(pPrev->Str(), pS->Length())!=0))
////        {
////        m_List.Append("------------------------------------------------------------------");
////        pPrev=pS;
////        }
////
////      // (p)
////      //  *p='.';
////      //dbgpln("m_List %s",pS->Str());
////      m_List.Append(pS);
////      pS=new Strng;
////      }
////    delete pS;
////    }
////  bListChgd=1;
////  LeaveCriticalSection(&ListLock);
//
//  //SendMessage(WMU_PB_UPDATEDATA, bProbConverged ? 3 : 2, 0);
//	
//  }
//
////===========================================================================
////
////
////
////===========================================================================
//
//IMPLEMENT_DYNCREATE(CControlsPage, CPropertyPage)
//
//CControlsPage::CControlsPage() : CCustomPropertyPage(CControlsPage::IDD)
//  {
//	//{{AFX_DATA_INIT(CControlsPage)
//	//}}AFX_DATA_INIT
//  iSelTarget=-1;
//  //bDidInit = False;
//  }
//
////--------------------------------------------------------------------------
//
//CControlsPage::~CControlsPage()
//  {
//  //bDidInit = False;
//  }
//
////--------------------------------------------------------------------------
//
//BOOL CControlsPage::OnInitDialog() 
//  {
//  //bDidInit = True;
//  InitDialog();
//
//	CCustomPropertyPage::OnInitDialog();
//
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_TARGETLIST);
//  int j=0;
//  pLst->ResetContent();
//  int Tabs[] = {45*4,47*4};
//  pLst->SetTabStops(2, Tabs);
//
//	UpdateTargetList();
//  UpdateTargetText();
//    
//	return TRUE;  // return TRUE unless you set the focus to a control
//	              // EXCEPTION: OCX Property Pages should return FALSE
//  }
//
////--------------------------------------------------------------------------
//
//void CControlsPage::DoDataExchange(CDataExchange* pDX)
//  {
//	CCustomPropertyPage::DoDataExchange(pDX);
//	//{{AFX_DATA_MAP(CControlsPage)
//	//}}AFX_DATA_MAP
//
//  if (!pDX->m_bSaveAndValidate)
//    {
//    //flag Lckd=LockWindowUpdate();
//    UpdateTargetList();
//    //UpdateTargetText();
//    //if (Lckd)
//    //  UnlockWindowUpdate();
//    }
//  }
//
////--------------------------------------------------------------------------
//
//BEGIN_MESSAGE_MAP(CControlsPage, CCustomPropertyPage)
//	//{{AFX_MSG_MAP(CControlsPage)
//	ON_BN_CLICKED(IDC_TARGETADD, OnTargetadd)
//	ON_BN_CLICKED(IDC_TARGETREMOVE, OnTargetremove)
//	ON_LBN_SELCHANGE(IDC_TARGETLIST, OnSelchangeTargetlist)
//	ON_WM_DESTROY()
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()
//
////--------------------------------------------------------------------------
//
//void CControlsPage::OnOK() 
//  {
//  if (Inited())
//    {
//    //gs_pPrj->lStatsRefreshCnt = m_RefreshCnt;
//    }
//  }
//
////--------------------------------------------------------------------------
//
//void CControlsPage::OnTargetadd() 
//  {
//	// TODO: Add your control notification handler code here
//  //char buff[4096];
//  pSlvTool->bChanged |= 1;
//
//
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_TARGETLIST);
//  CEdit* pEdtExp=(CEdit*)GetDlgItem(IDC_TRGEXPEDIT);
//  CEdit* pEdtVal=(CEdit*)GetDlgItem(IDC_TRGVALEDIT);
//  
//  char Exp[1024], Val[1024];
//  pEdtExp->GetWindowText(Exp, sizeof(Exp));
//  pEdtVal->GetWindowText(Val, sizeof(Val));
//  XStrTrim(Exp);
//  XStrTrim(Val);
//
//  CTargetArray &Trg = pSlvTool->Slv().Targets;
//
//  int sn=Trg.GetSize();
//  for (int s=0; s<sn; s++)
//    if (Trg[s].Exp.sExp.XStrICmp(Exp)==0)
//      break;
//
//  if (strlen(Exp)>0 && strlen(Val)>0)
//    {
//    if (s>=sn)
//      Trg.SetSize(sn+1);
//
//    Trg[s].Exp.sExp=(const char*)Exp;
//    Trg[s].Val.sExp=(const char*)Val;
//    iSelTarget=s;
//    //pLst->AddString("???"); // InCase Last
//    //pLst->SetCurSel(s);
//    }
//  else if (s<sn)
//    {
//    Trg.RemoveAt(s);
//    iSelTarget=-1;
//    //pLst->SetCurSel(-1);
//    }
//	UpdateTargetList();
//  UpdateTargetText();
//  }
//
////--------------------------------------------------------------------------
//
//void CControlsPage::OnTargetremove() 
//  {
//  // TODO: Add your control notification handler code here
//  pSlvTool->bChanged |= 1;
//
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_TARGETLIST);
//  int s=pLst->GetCurSel();
//  if (s!=LB_ERR)
//    {
//    //int sn=pLst->GetCount();
//    //if (sn!=LB_ERR)
//    //  pLst->SetCurSel(Min(s, sn-1));
//    pSlvTool->Slv().Targets.RemoveAt(s);
//    iSelTarget=-1;
//    UpdateTargetList();
//    UpdateTargetText();
//    }
//  }
//
////--------------------------------------------------------------------------
//
//void CControlsPage::OnSelchangeTargetlist() 
//  {
//  // TODO: Add your control notification handler code here
//  
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_TARGETLIST);
//  int n=pLst->GetCurSel();
//  iSelTarget=(n!=LB_ERR) ? n : -1;
//  UpdateTargetText();
//  }
//
////--------------------------------------------------------------------------
//
//void CControlsPage::UpdateTargetList() 
//  {
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_TARGETLIST);
//  CTargetArray &Trg = pSlvTool->Slv().Targets;
//
//  int sn=Trg.GetSize();//pLst->GetCount();
//  //int cs=pLst->GetCurSel();
//  pLst->ResetContent();
//  for (int s=0; s<sn; s++)
//    {
//    char buff[4069];
//    int l=sprintf(buff, "%s", Trg[s].Exp.sExp()); 
//    l+=sprintf(&buff[l], Trg[s].Val.sExp.Length()>0 ? "==%s" : "", Trg[s].Val.sExp()); 
//    l+=sprintf(&buff[l], Trg[s].Exp.sCurVal.Length()>0 ? "\t=%s" : "\t=.", Trg[s].Exp.sCurVal()); 
//    pLst->AddString(buff);
//    }
//  pLst->SetCurSel(iSelTarget);
//  }
//
////--------------------------------------------------------------------------
//
//void CControlsPage::UpdateTargetText() 
//  {
//  //char buff[4096];
//  
//  //CListBox* pLst=(CListBox*)GetDlgItem(IDC_TARGETLIST);
//  CEdit* pEdtExp=(CEdit*)GetDlgItem(IDC_TRGEXPEDIT);
//  CEdit* pEdtVal=(CEdit*)GetDlgItem(IDC_TRGVALEDIT);
//	if (iSelTarget>=0)
//    {
//    //pLst->GetText(iSelTarget, buff);
//    //char*pVal=KillTab(buff); 
//    CTarget &Trg=pSlvTool->Slv().Targets[iSelTarget];
//    pEdtExp->SetWindowText(Trg.Exp.sExp());
//    pEdtVal->SetWindowText(Trg.Val.sExp()); 
//    
//    //pEdtExp->SetWindowText(buff);
//    //pEdtVal->SetWindowText((pVal+1));
//    }
//  else
//    {
//    pEdtExp->SetWindowText("");
//    pEdtVal->SetWindowText("");
//    }
//  }
//
////--------------------------------------------------------------------------
//
//void CControlsPage::OnDestroy() 
//  {
//	CCustomPropertyPage::OnDestroy();
//	
//	// TODO: Add your message handler code here
//	
//  }
//
////===========================================================================
////
////
////
////===========================================================================
//
//IMPLEMENT_DYNCREATE(CTargetsPage, CPropertyPage)
//
//CTargetsPage::CTargetsPage() : CCustomPropertyPage(CTargetsPage::IDD)
//  {
//	//{{AFX_DATA_INIT(CTargetsPage)
//	//}}AFX_DATA_INIT
//  iSelTarget=-1;
//  //bDidInit = False;
//  }
//
////--------------------------------------------------------------------------
//
//CTargetsPage::~CTargetsPage()
//  {
//  //bDidInit = False;
//  }
//
////--------------------------------------------------------------------------
//
//BOOL CTargetsPage::OnInitDialog() 
//  {
//  InitDialog();//bDidInit = True;
//	CCustomPropertyPage::OnInitDialog();
//
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_TARGETLIST);
//  int j=0;
//  pLst->ResetContent();
//  int Tabs[] = {45*4,47*4};
//  pLst->SetTabStops(2, Tabs);
//
//	UpdateTargetList();
//  UpdateTargetText();
//    
//	return TRUE;  // return TRUE unless you set the focus to a control
//	              // EXCEPTION: OCX Property Pages should return FALSE
//  }
//
////--------------------------------------------------------------------------
//
//void CTargetsPage::DoDataExchange(CDataExchange* pDX)
//  {
//	CCustomPropertyPage::DoDataExchange(pDX);
//	//{{AFX_DATA_MAP(CTargetsPage)
//	//}}AFX_DATA_MAP
//
//  if (!pDX->m_bSaveAndValidate)
//    {
//    //flag Lckd=LockWindowUpdate();
//    UpdateTargetList();
//    //UpdateTargetText();
//    //if (Lckd)
//    //  UnlockWindowUpdate();
//    }
//  }
//
////--------------------------------------------------------------------------
//
//BEGIN_MESSAGE_MAP(CTargetsPage, CCustomPropertyPage)
//	//{{AFX_MSG_MAP(CTargetsPage)
//	ON_BN_CLICKED(IDC_TARGETADD, OnTargetadd)
//	ON_BN_CLICKED(IDC_TARGETREMOVE, OnTargetremove)
//	ON_LBN_SELCHANGE(IDC_TARGETLIST, OnSelchangeTargetlist)
//	ON_WM_DESTROY()
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()
//
////--------------------------------------------------------------------------
//
//void CTargetsPage::OnOK() 
//  {
//  if (Inited())
//    {
//    //gs_pPrj->lStatsRefreshCnt = m_RefreshCnt;
//    }
//  }
//
////--------------------------------------------------------------------------
//
//void CTargetsPage::OnTargetadd() 
//  {
//	// TODO: Add your control notification handler code here
//  //char buff[4096];
//
//  pSlvTool->bChanged |= 1;
//
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_TARGETLIST);
//  CEdit* pEdtExp=(CEdit*)GetDlgItem(IDC_TRGEXPEDIT);
//  CEdit* pEdtVal=(CEdit*)GetDlgItem(IDC_TRGVALEDIT);
//  
//  char Exp[1024], Val[1024];
//  pEdtExp->GetWindowText(Exp, sizeof(Exp));
//  pEdtVal->GetWindowText(Val, sizeof(Val));
//  XStrTrim(Exp);
//  XStrTrim(Val);
//
//  CTargetArray &Trg = pSlvTool->Slv().Targets;
//
//  int sn=Trg.GetSize();
//  for (int s=0; s<sn; s++)
//    if (Trg[s].Exp.sExp.XStrICmp(Exp)==0)
//      break;
//
//  if (strlen(Exp)>0 && strlen(Val)>0)
//    {
//    if (s>=sn)
//      Trg.SetSize(sn+1);
//
//    Trg[s].Exp.sExp=(const char*)Exp;
//    Trg[s].Val.sExp=(const char*)Val;
//    iSelTarget=s;
//    //pLst->AddString("???"); // InCase Last
//    //pLst->SetCurSel(s);
//    }
//  else if (s<sn)
//    {
//    Trg.RemoveAt(s);
//    iSelTarget=-1;
//    //pLst->SetCurSel(-1);
//    }
//	UpdateTargetList();
//  UpdateTargetText();
//  }
//
////--------------------------------------------------------------------------
//
//void CTargetsPage::OnTargetremove() 
//  {
//  // TODO: Add your control notification handler code here
//
//  pSlvTool->bChanged |= 1;
//
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_TARGETLIST);
//  int s=pLst->GetCurSel();
//  if (s!=LB_ERR)
//    {
//    //int sn=pLst->GetCount();
//    //if (sn!=LB_ERR)
//    //  pLst->SetCurSel(Min(s, sn-1));
//    pSlvTool->Slv().Targets.RemoveAt(s);
//    iSelTarget=-1;
//    UpdateTargetList();
//    UpdateTargetText();
//    }
//  }
//
////--------------------------------------------------------------------------
//
//void CTargetsPage::OnSelchangeTargetlist() 
//  {
//  // TODO: Add your control notification handler code here
//  
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_TARGETLIST);
//  int n=pLst->GetCurSel();
//  iSelTarget=(n!=LB_ERR) ? n : -1;
//  UpdateTargetText();
//  }
//
////--------------------------------------------------------------------------
//
//void CTargetsPage::UpdateTargetList() 
//  {
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_TARGETLIST);
//  CTargetArray &Trg = pSlvTool->Slv().Targets;
//
//  int sn=Trg.GetSize();//pLst->GetCount();
//  //int cs=pLst->GetCurSel();
//  pLst->ResetContent();
//  for (int s=0; s<sn; s++)
//    {
//    char buff[4069];
//    int l=sprintf(buff, "%s", Trg[s].Exp.sExp()); 
//    l+=sprintf(&buff[l], Trg[s].Val.sExp.Length()>0 ? "==%s" : "", Trg[s].Val.sExp()); 
//    l+=sprintf(&buff[l], Trg[s].Exp.sCurVal.Length()>0 ? "\t=%s" : "\t=.", Trg[s].Exp.sCurVal()); 
//    pLst->AddString(buff);
//    }
//  pLst->SetCurSel(iSelTarget);
//  }
//
////--------------------------------------------------------------------------
//
//void CTargetsPage::UpdateTargetText() 
//  {
//  //char buff[4096];
//  
//  //CListBox* pLst=(CListBox*)GetDlgItem(IDC_TARGETLIST);
//  CEdit* pEdtExp=(CEdit*)GetDlgItem(IDC_TRGEXPEDIT);
//  CEdit* pEdtVal=(CEdit*)GetDlgItem(IDC_TRGVALEDIT);
//	if (iSelTarget>=0)
//    {
//    CTarget &Trg=pSlvTool->Slv().Targets[iSelTarget];
//    pEdtExp->SetWindowText(Trg.Exp.sExp());
//    pEdtVal->SetWindowText(Trg.Val.sExp()); 
//    //pLst->GetText(iSelTarget, buff);
//    //char*pVal=KillTab(buff); 
//    
//    //pEdtExp->SetWindowText(buff);
//    //pEdtVal->SetWindowText((pVal+1));
//    }
//  else
//    {
//    pEdtExp->SetWindowText("");
//    pEdtVal->SetWindowText("");
//    }
//  }
//
////--------------------------------------------------------------------------
//
//void CTargetsPage::OnDestroy() 
//  {
//	CCustomPropertyPage::OnDestroy();
//	
//	// TODO: Add your message handler code here
//	
//  }
//
////===========================================================================
////
////
////
////===========================================================================
//
//IMPLEMENT_DYNCREATE(CParametersPage, CPropertyPage)
//
//CParametersPage::CParametersPage() : CCustomPropertyPage(CParametersPage::IDD)
//  {
//	//{{AFX_DATA_INIT(CParametersPage)
//	//}}AFX_DATA_INIT
//  iSelParam=-1;
//  //bDidInit = False;
//  }
//
////--------------------------------------------------------------------------
//
//CParametersPage::~CParametersPage()
//  {
//  //bDidInit = False;
//  }
//
////--------------------------------------------------------------------------
//
//BOOL CParametersPage::OnInitDialog() 
//  {
//  InitDialog();//bDidInit = True;
//	CCustomPropertyPage::OnInitDialog();
//
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_PARAMLIST);
//  int j=0;
//  pLst->ResetContent();
//  int Tabs[] = {45*4,47*4};
//  pLst->SetTabStops(2, Tabs);
//    
//	UpdateParamList();
//  UpdateParamText();
//	
//	return TRUE;  // return TRUE unless you set the focus to a control
//	              // EXCEPTION: OCX Property Pages should return FALSE
//  }
//
////--------------------------------------------------------------------------
//
//void CParametersPage::DoDataExchange(CDataExchange* pDX)
//  {
//	CCustomPropertyPage::DoDataExchange(pDX);
//	//{{AFX_DATA_MAP(CParametersPage)
//	//}}AFX_DATA_MAP
//  if (!pDX->m_bSaveAndValidate)
//    {
//    //flag Lckd=LockWindowUpdate();
//    UpdateParamList();
//    //UpdateTargetText();
//    //if (Lckd)
//    //  UnlockWindowUpdate();
//    }
//  }
//
////--------------------------------------------------------------------------
//
//BEGIN_MESSAGE_MAP(CParametersPage, CCustomPropertyPage)
//	//{{AFX_MSG_MAP(CParametersPage)
//	ON_LBN_SELCHANGE(IDC_PARAMLIST, OnSelchangeParamlist)
//	ON_BN_CLICKED(IDC_PARAMADD, OnParamadd)
//	ON_BN_CLICKED(IDC_PARAMREMOVE, OnParamremove)
//	ON_WM_DESTROY()
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()
//
////--------------------------------------------------------------------------
//
//void CParametersPage::OnOK() 
//  {
//  if (Inited())
//    {
//    //gs_pPrj->lStatsRefreshCnt = m_RefreshCnt;
//    }
//  }
//
////--------------------------------------------------------------------------
//
//void CParametersPage::OnParamadd() 
//  {
//	// TODO: Add your control notification handler code here
//  //char buff[4096];
//  pSlvTool->bChanged |= 1;
//
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_PARAMLIST);
//  CEdit* pEdtExp=(CEdit*)GetDlgItem(IDC_PARAMEXPEDIT);
//  CEdit* pEdtMin=(CEdit*)GetDlgItem(IDC_PARAMMINEDIT);
//  CEdit* pEdtMax=(CEdit*)GetDlgItem(IDC_PARAMMAXEDIT);
//  
//  char Exp[1024], Min[1024], Max[1024];
//  pEdtExp->GetWindowText(Exp, sizeof(Exp));
//  pEdtMin->GetWindowText(Min, sizeof(Min));
//  pEdtMax->GetWindowText(Max, sizeof(Max));
//  XStrTrim(Exp);
//  XStrTrim(Min);
//  XStrTrim(Max);
//
//  CParamArray &Prm = pSlvTool->Slv().Params;
//
//  int sn=Prm.GetSize();
//  for (int s=0; s<sn; s++)
//    if (Prm[s].Exp.sExp.XStrICmp(Exp)==0)
//      break;
//
//  if (strlen(Exp)>0) // && (strlen(Min)>0 || strlen(Max)>0))
//    {
//    if (s>=sn)
//      Prm.SetSize(sn+1);
//
//    Prm[s].Exp.sExp=(const char*)Exp;
//    Prm[s].Min.sExp=(const char*)Min;
//    Prm[s].Max.sExp=(const char*)Max;
//    iSelParam=s;
//    //pLst->AddString("???"); // InCase Last
//    //pLst->SetCurSel(s);
//    }
//  else if (s<sn)
//    {
//    Prm.RemoveAt(s);
//    iSelParam=-1;
//    //pLst->SetCurSel(-1);
//    }
//	
//	UpdateParamList();
//  UpdateParamText();
//  }
//
////--------------------------------------------------------------------------
//
//void CParametersPage::OnParamremove() 
//  {
//  // TODO: Add your control notification handler code here
//  pSlvTool->bChanged |= 1;
//
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_PARAMLIST);
//  int s=pLst->GetCurSel();
//  if (s!=LB_ERR)
//    {
//    //int sn=pLst->GetCount();
//    //if (sn!=LB_ERR)
//    //  pLst->SetCurSel(Min(s, sn-1));
//    pSlvTool->Slv().Params.RemoveAt(s);
//    iSelParam=-1;
//    UpdateParamList();
//    UpdateParamText();
//    }
//  }
//
////--------------------------------------------------------------------------
//
//void CParametersPage::OnSelchangeParamlist() 
//  {
//  // TODO: Add your control notification handler code here
//  
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_PARAMLIST);
//  int n=pLst->GetCurSel();
//  iSelParam=(n!=LB_ERR) ? n : -1;
//  UpdateParamText();
//  }
//
////--------------------------------------------------------------------------
//
//void CParametersPage::UpdateParamList() 
//  {
//  CListBox* pLst=(CListBox*)GetDlgItem(IDC_PARAMLIST);
//  CParamArray &Prm= pSlvTool->Slv().Params;
//
//  int sn=Prm.GetSize();//pLst->GetCount();
//  //int cs=pLst->GetCurSel();
//  pLst->ResetContent();
//  for (int s=0; s<sn; s++)
//    {
//    CParam &Prm=pSlvTool->Slv().Params[s];
//
//    char buff[4069];
//    int l=sprintf(buff, "%s", Prm.Exp.sExp()); 
//    l+=sprintf(&buff[l], Prm.Min.sExp.Length()>0 ? " {>=%s}" : "", Prm.Min.sExp()); 
//    l+=sprintf(&buff[l], Prm.Max.sExp.Length()>0 ? " {<=%s}" : "", Prm.Max.sExp()); 
//    l+=sprintf(&buff[l], Prm.Exp.sCurVal.Length()>0 ? "\t=%s" : "\t=.", Prm.Exp.sCurVal()); 
//
//    pLst->AddString(buff);
//    }
//  //if (cs!=LB_ERR && cs!=LB_ERRSPACE)
//  pLst->SetCurSel(iSelParam);
//  }
//
////--------------------------------------------------------------------------
//
//void CParametersPage::UpdateParamText() 
//  {
//  //ar buff[4096];
//  
//  //CListBox* pLst=(CListBox*)GetDlgItem(IDC_PARAMLIST);
//  CEdit* pEdtExp=(CEdit*)GetDlgItem(IDC_PARAMEXPEDIT);
//  CEdit* pEdtMin=(CEdit*)GetDlgItem(IDC_PARAMMINEDIT);
//  CEdit* pEdtMax=(CEdit*)GetDlgItem(IDC_PARAMMAXEDIT);
//  //int n=pLst->GetCurSel();
//	if (iSelParam>=0)
//    {
//    
//    CParam & Prm=pSlvTool->Slv().Params[iSelParam]; 
//    pEdtExp->SetWindowText(Prm.Exp.sExp()); 
//    pEdtMin->SetWindowText(Prm.Min.sExp.Length()>0 ? Prm.Min.sExp() : ""); 
//    pEdtMax->SetWindowText(Prm.Max.sExp.Length()>0 ? Prm.Max.sExp() : ""); 
//    }
//  else
//    {
//    pEdtExp->SetWindowText("");
//    pEdtMin->SetWindowText("");
//    pEdtMax->SetWindowText("");
//    }
//
//  }
//
////--------------------------------------------------------------------------
//
//void CParametersPage::OnDestroy() 
//  {
//	CCustomPropertyPage::OnDestroy();
//	
//	// TODO: Add your message handler code here
//	
//  }
//===========================================================================
//
//
//
//===========================================================================
