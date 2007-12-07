// ExploreScd.cpp : implementation file
//

#include "stdafx.h"
#include "sc_defs.h"
#include "syscad.h"
#include "project.h"
#include "mainfrm.h"
#include "grfdoc.h"
#include "scd_wm.h"
#include "dbgmngr.h"
#include "scdver.h"
#include "nrecipe.h"
#include "tagvdoc.h"
#include "fixedit.h"
#include "tagvtrnd.h"
#include "tagvtext.h"
#include "tagvsplt.h"
#include "tagvdlgs.h"
#include "renamepagedlg.h"
#include "slvcfg.h"
#include "bulktagchange.h"
#include "chngtag.h"

extern "C" 
  {
#include "grldefs.h"
  }

#include "afxwin.h"
#include "ExploreScd.h"
#include "wndslct.h"
//#include "optoff.h"

#define dbgAdd            01
#define dbgTime           0
#define dbgDumpAll        0
#define dbgHoldLockUpdate 0

// CExploreScd dialog

const int Img_GrfActive       = 0;
const int Img_Trnd            = 1;
const int Img_Other           = 2;
const int Img_Node            = 3;
const int Img_Class           = 4;
const int Img_LostGrf         = 5;
const int Img_LostMdl         = 6;
const int Img_GrfInActive     = 7;
const int Img_FlowNU          = 8;   // Never Used
const int Img_FlowProcNU      = 9;
const int Img_FlowMakeupNU    = 10;
const int Img_FlowBleedNU     = 11;
const int Img_FlowSpillNU     = 12;
const int Img_FlowVentNU      = 13;
const int Img_FlowLeakNU      = 14;
const int Img_FlowOtherNU     = 15;
const int Img_FlowWU          = 16;  // Was Used
const int Img_FlowProcWU      = 17;
const int Img_FlowMakeupWU    = 18;
const int Img_FlowBleedWU     = 19;
const int Img_FlowSpillWU     = 20;
const int Img_FlowVentWU      = 21;
const int Img_FlowLeakWU      = 22;
const int Img_FlowOtherWU     = 23;
const int Img_FlowIU          = 24;  // In Use
const int Img_FlowProcIU      = 25;
const int Img_FlowMakeupIU    = 26;
const int Img_FlowBleedIU     = 27;
const int Img_FlowSpillIU     = 28;
const int Img_FlowVentIU      = 29;
const int Img_FlowLeakIU      = 30;
const int Img_FlowOtherIU     = 31;
const int Img_Tear            = 32;

const int PgType2TrIDs[3]={TrID_Other, TrID_Graphic, TrID_Trend};
const int PgType2ImgIDsActive[3]={Img_Other, Img_GrfActive, Img_Trnd};
const int PgType2ImgIDsInactive[3]={Img_Other, Img_GrfInActive, Img_Trnd};

//---------------------------------------------------------------------------

const byte TrkLeft      = 0x01;
const byte TrkTop       = 0x02;
const byte TrkRight     = 0x04;
const byte TrkBottom    = 0x08;
const byte TrkSep0      = 0x10;
const byte TrkSep1      = 0x20;
const byte TrkSep2      = 0x40;

static struct { UINT m_ID; byte m_TrkTL; byte m_TrkBR; } const CtrlIDs[]=//CtrlIDCount]= 
  {
    { IDC_TREE              , TrkLeft   |TrkTop    , TrkRight   |TrkBottom  },
    { IDC_STRNM3            , TrkLeft   |TrkBottom , TrkLeft    |TrkBottom  },
    { IDC_TAGFILTERRULE     , TrkLeft   |TrkBottom , TrkRight   |TrkBottom  },
    { IDC_TAGFILTER         , TrkLeft   |TrkBottom , TrkRight   |TrkBottom  },
  };

const int CtrlIDCount = sizeof(CtrlIDs)/sizeof(CtrlIDs[0]);

//---------------------------------------------------------------------------

static LPCTSTR Section="Explorer";

//---------------------------------------------------------------------------

CXTCommon::CXTCommon(CExploreScd * Dlg, int Id, LPCSTR Tag) : m_Dlg(*Dlg) , CXTTreeInfo(Id, &m_sTag)
  {
  m_sTag=Tag;
  m_sTagLwr=Tag;
  m_sTagLwr.MakeLower();
  //m_pClass=Class;
  m_Selected=false;
  m_Marked=false;
  //m_InUse=true;
  //m_ModelOK=true;
  m_hTreeItem=NULL;
  //m_hClassItem=NULL;
  };

CXTCommon::~CXTCommon()
  {
  }

//---------------------------------------------------------------------------

CXTTag::CXTTag(CExploreScd * Dlg, LPCTSTR Tag, CXTClass * Class) : /*m_Dlg(*Dlg), */  CXTCommon(Dlg, TrID_Node, Tag)
  {
  //m_sTag=Tag;
  //m_sTagLwr=Tag;
  //m_sTagLwr.MakeLower();
  m_pClass=Class;
  //m_Selected=false;
  //m_Marked=false;
  m_InUse=true;
  m_ModelOK=true;
//  m_hTreeItem=NULL;
  m_hClassItem=NULL;
  m_iAccPage=-1;
  };

CXTTag::~CXTTag() 
  {
  for (int i=0; i<m_Pages.GetCount(); i++)
    delete m_Pages[i];
  m_Pages.SetSize(0, 128);
  };

int CXTTag::Icon() { return m_ModelOK ? (m_Pages.GetCount()>0?Img_Node:Img_LostGrf):Img_LostMdl; }

//---------------------------------------------------------------------------

CXTFlow::CXTFlow(CExploreScd * Dlg, LPCTSTR Tag, int FlowType, int UseStatus) : /*m_Dlg(*Dlg) ,*/ CXTCommon(Dlg, TrID_Flow, Tag)
  {
  //m_sTag=Tag;
  //m_sTagLwr=Tag;
  //m_sTagLwr.MakeLower();
  m_iFlowType=FlowType;
  m_iUseStatus=UseStatus;
  //m_pClass=Class;
  //m_Selected=false;
  //m_Marked=false;
  m_InUse=true;
  m_ModelOK=true;
  //m_hTreeItem=NULL;
  //m_hClassItem=NULL;
  //m_iAccPage=-1;
  };

CXTFlow::~CXTFlow() 
  {
  //for (int i=0; i<m_Pages.GetCount(); i++)
  //  delete m_Pages[i];
  //m_Pages.SetSize(0, 128);
  };

int CXTFlow::Icon()    
  { 
  if (m_iUseStatus&CUseMsk_InUse)
    return Img_FlowIU+m_iFlowType; 
  if (m_iUseStatus&CUseMsk_WasUsed)
    return Img_FlowWU+m_iFlowType; 
  return Img_FlowNU+m_iFlowType; 
  }

bool CXTFlow::ShowIt() 
  { return m_Selected && ((m_iUseStatus&m_Dlg.m_iShowWhatFlows) || ((m_iUseStatus==0) && (m_Dlg.m_iShowWhatFlows&CUseMsk_Exists))); }

HTREEITEM CXTFlow::ReqdParentItem()
  {
  switch (m_iFlowType)
    {
    case FlwType_Makeup:
    case FlwType_Bleed:
      return m_Dlg.m_bBranchMU ? m_Dlg.m_hMUFlowsItem : m_Dlg.m_hFlowsItem;
    case FlwType_Spill:
    case FlwType_Vent:
    case FlwType_Leak:
      return m_Dlg.m_bBranchSP ? m_Dlg.m_hSPFlowsItem : m_Dlg.m_hFlowsItem;
    default:
      return m_Dlg.m_hFlowsItem;
    }
  }

//---------------------------------------------------------------------------

CXTTear::CXTTear(CExploreScd * Dlg, LPCTSTR Tag/*, int TearType, int UseStatus*/) : /*m_Dlg(*Dlg) ,*/ CXTCommon(Dlg, TrID_Tear, Tag)
  {
  //m_sTag=Tag;
  //m_sTagLwr=Tag;
  //m_sTagLwr.MakeLower();
  //m_iTearType=TearType;
  //m_iUseStatus=UseStatus;
  //m_pClass=Class;
  //m_Selected=false;
  //m_Marked=false;
  m_InUse=true;
  m_ModelOK=true;
  //m_hTreeItem=NULL;
  //m_hClassItem=NULL;
  //m_iAccPage=-1;
  };

CXTTear::~CXTTear() 
  {
  //for (int i=0; i<m_Pages.GetCount(); i++)
  //  delete m_Pages[i];
  //m_Pages.SetSize(0, 128);
  };

int CXTTear::Icon()    
  { 
  //if (m_iUseStatus&CUseMsk_InUse)
  //  return Img_TearIU+m_iTearType; 
  //if (m_iUseStatus&CUseMsk_WasUsed)
  //  return Img_TearWU+m_iTearType; 
  return Img_Tear;//NU+m_iTearType; 
  }

bool CXTTear::ShowIt() 
  { return m_Selected && true; };//((m_iUseStatus&m_Dlg.m_iShowWhatTears) || ((m_iUseStatus==0) && (m_Dlg.m_iShowWhatTears&CUseMsk_Exists))); }

HTREEITEM CXTTear::ReqdParentItem()
  {
  //switch (m_iTearType)
  //  {
  //  case FlwType_Makeup:
  //  case FlwType_Bleed:
  //    return m_Dlg.m_bBranchMU ? m_Dlg.m_hMUTearsItem : m_Dlg.m_hTearsItem;
  //  case FlwType_Spill:
  //  case FlwType_Vent:
  //  case FlwType_Leak:
  //    return m_Dlg.m_bBranchSP ? m_Dlg.m_hSPTearsItem : m_Dlg.m_hTearsItem;
  //  default:
  //    return m_Dlg.m_hTearsItem;
  //  }
  return m_Dlg.m_hTearsItem;
  }

//---------------------------------------------------------------------------

CXTClass::CXTClass(CExploreScd * Dlg, LPCTSTR ClassId) : m_Dlg(*Dlg),
CXTTreeInfo(TrID_Class, &m_sClassId)
  {
  m_sClassId=ClassId;
  m_Selected=true;
  m_hClassItem=NULL;
  m_nCount=0;
  m_nSelected=0;
  xm_Tags.SetSize(0, 128);
  };

CXTClass::~CXTClass() 
  {
  for (int i=0; i<xm_Tags.GetCount(); i++)
    delete xm_Tags[i];
  xm_Tags.SetSize(0, 128);
  };

//---------------------------------------------------------------------------

CXTPage::CXTPage(CExploreScd * Dlg, int IdNo, LPCTSTR PageId, LPCTSTR PageName) : m_Dlg(*Dlg), 
CXTTreeInfo(IdNo, &m_sPageId)
  {
  m_sPageId=PageId;
  m_sPageName=PageName;
  m_Selected=true;
  m_InUse=true;
  m_hPage=NULL;
  m_iType=-1;
  m_pGrfDoc=NULL;
  m_TagHs.SetSize(0, 128);
  m_TagHMap.InitHashTable(FindNextPrimeNumber((UINT)(256)));
  };

CXTPage::~CXTPage() 
  {
  for (int i=0; i<m_TagHs.GetCount(); i++)
    delete m_TagHs[i];
  m_TagHs.SetSize(0, 128);
  };

//===========================================================================

CExpTreeCtrl::~CExpTreeCtrl()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CExpTreeCtrl, CTreeCtrl)
  //{{AFX_MSG_MAP(CExpTreeCtrl)
  ON_WM_RBUTTONDOWN()
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_LBUTTONUP()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CExpTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
  {
  CTreeCtrl::OnRButtonDown(nFlags, point);
  }

//---------------------------------------------------------------------------

void CExpTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
  {
  //dbgpln("OnLButtonDown>>");
  CTreeCtrl::OnLButtonDown(nFlags, point);
  //dbgpln("<<OnLButtonDown");
  }

//---------------------------------------------------------------------------

void CExpTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
  {
  CTreeCtrl::OnRButtonUp(nFlags, point);
  }

//---------------------------------------------------------------------------

void CExpTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
  {
  //dbgpln("OnLButtonUp>>");
  CTreeCtrl::OnLButtonUp(nFlags, point);
  //dbgpln("<<OnLButtonUp");
  }

//===========================================================================

CExploreScd * CExploreScd::sm_pTheWnd=NULL;
BOOL CExploreScd::sm_bDoRefresh=false;
BOOL CExploreScd::sm_bDoRefreshAll=false;
BOOL CExploreScd::sm_bUseScdExplorer=true;
BOOL CExploreScd::sm_bInited=false;

IMPLEMENT_DYNAMIC(CExploreScd, CDialog)
CExploreScd::CExploreScd(CWnd* pParent /*=NULL*/)
: CDialog(CExploreScd::IDD, pParent),
m_GraphicTitle(TrID_GraphicHdr, "Graphic"),
m_TrendTitle(TrID_TrendHdr, "Trend"),
m_ClassTitle(TrID_ClassHdr, "Class"),
m_NodeTitle(TrID_NodeHdr,   "Node"),
m_FlowTitle(TrID_FlowHdr,  "Flow"),
m_MUFlowTitle(TrID_MUFlowHdr,  "Makeups & Bleeds"),
m_SPFlowTitle(TrID_SPFlowHdr,  "Spills, Vents & Leaks"),
m_TearTitle(TrID_TearHdr, "Tears"),
m_OtherTitle(TrID_OtherHdr, "...")
  {
  m_FilterString    = "Boil";
  m_FilterRule      = eFRContains;

  m_hGraphItem      = NULL;
  m_hTrendItem      = NULL;
  m_hOtherItem      = NULL;
  m_hClassItem      = NULL;
  m_hNodesItem      = NULL;
  m_hFlowsItem      = NULL;
  m_hMUFlowsItem    = NULL;
  m_hSPFlowsItem    = NULL;
  m_hTearsItem      = NULL;

  m_nTagsSelected   = 0;
  m_nFlowsSelected  = 0;
  m_nTearsSelected  = 0;
  m_WindowCount     = 0;
  m_GraphicCount    = 0;
  m_TrendCount      = 0;

  m_ChangeBusy      = 0;

  m_hPrevSel        = NULL;

  m_iShowWhatFlows  = CUseMsk_InUse | CUseMsk_WasUsed;// | CUseMsk_Exists;
  m_bBranchMU       = false;
  m_bBranchSP       = true;

  Create(CExploreScd::IDD, pParent);
  }

CExploreScd::~CExploreScd()
  {
  }

//---------------------------------------------------------------------------

void CExploreScd::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_TREE, m_Tree);
  DDX_Control(pDX, IDC_TAGFILTERRULE, m_TagFilterRule);
  DDX_Control(pDX, IDC_TAGFILTER, m_TagFilter);
  }


BEGIN_MESSAGE_MAP(CExploreScd, CDialog)
  ON_WM_SIZE()
  ON_WM_MOVE()
  //ON_WM_ACTIVATE()

  ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnTvnSelchangedTree)
  ON_NOTIFY(NM_CLICK, IDC_TREE, OnNMClickTree)
  ON_NOTIFY(NM_RCLICK, IDC_TREE, OnNMRclickTree)
  ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnNMDblclkTree)
  ON_EN_CHANGE(IDC_TAGFILTER, OnEnChangeTagfilter)
  ON_CBN_SELCHANGE(IDC_TAGFILTERRULE, OnCbnSelchangeTagfilterrule)
  ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE, OnTvnItemexpandedTree)
END_MESSAGE_MAP()

inline DWORD_PTR GetTreeInfo(CXTTreeInfo * Info) { return (DWORD_PTR)Info; };
inline DWORD_PTR GetTreeInfo(CXTTreeInfo & Info) { return (DWORD_PTR)&Info; };

inline HTREEITEM CExploreScd::InsertItem( LPCTSTR lpszItem, int nImage, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter)
  {
  TVINSERTSTRUCT In;
  In.hParent=hParent;
  In.hInsertAfter=hInsertAfter;
  In.item.mask=TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;
  In.item.iImage=nImage;
  In.item.iSelectedImage=nImage;
  In.item.lParam=lParam;
  In.item.pszText=(LPSTR)lpszItem;

  return m_Tree.InsertItem(&In);
  }

BOOL CExploreScd::OnInitDialog() 
  {
  CDialog::OnInitDialog();

  CProfINIFile PF(PrjIniFile());
  CString S;


  CBitmap BM;
  BM.LoadBitmap(IDB_EXPLOREIMGS);
  m_TreeImgList.Create(12, 12, FALSE, 0, 8);
  m_TreeImgList.Add(&BM, (CBitmap*)NULL);
  m_Tree.SetImageList(&m_TreeImgList, TVSIL_NORMAL);

  m_bCaseSens=false;
  m_pFilterRE=NULL;

  m_TagFilterRule.AddString(eFilterRuleNames[eFROff]);
  m_TagFilterRule.AddString(eFilterRuleNames[eFRContains]);
  m_TagFilterRule.AddString(eFilterRuleNames[eFRWildCard]);
  m_TagFilterRule.AddString(eFilterRuleNames[eFRRegExp]);

  S=PF.RdStr(Section, "TagFilterRule", eFilterRuleNames[eFRContains]);
  if (m_TagFilterRule.SelectString(-1, S)<0)
    m_TagFilterRule.FindString(-1, eFilterRuleNames[eFRContains]);

  m_TagFilter.SetWindowText(PF.RdStr(Section, "TagFilter", ""));

  FixFilterStuff();

  GetClientRect(&m_ClientRctInit);
  m_CtrlRcts.SetSize(CtrlIDCount);
  for (int i=0; i<CtrlIDCount; i++)
    {
    GetDlgItem(CtrlIDs[i].m_ID)->GetWindowRect(&m_CtrlRcts[i]);
    ScreenToClient(&m_CtrlRcts[i]);
    }

  InitTags();

  m_hGraphItem = InsertItem(LPSTR_TEXTCALLBACK, Img_GrfActive,  GetTreeInfo(m_GraphicTitle), TVI_ROOT);
  m_hTrendItem = InsertItem(LPSTR_TEXTCALLBACK, Img_Trnd,       GetTreeInfo(m_TrendTitle), TVI_ROOT);
  m_hClassItem = InsertItem(LPSTR_TEXTCALLBACK, Img_Class,      GetTreeInfo(m_ClassTitle), TVI_ROOT);
  m_hNodesItem = InsertItem(LPSTR_TEXTCALLBACK, Img_Node,       GetTreeInfo(m_NodeTitle), TVI_ROOT);
  m_hFlowsItem = InsertItem(LPSTR_TEXTCALLBACK, Img_FlowProcIU, GetTreeInfo(m_FlowTitle), TVI_ROOT);
  m_hTearsItem = InsertItem(LPSTR_TEXTCALLBACK, Img_Tear,       GetTreeInfo(m_TearTitle), TVI_ROOT);
  m_hOtherItem = InsertItem(LPSTR_TEXTCALLBACK, Img_Other,      GetTreeInfo(m_OtherTitle), TVI_ROOT);
  m_hMUFlowsItem = NULL;
  m_hSPFlowsItem = NULL;

  m_bGraphExpanded = PF.RdInt(Section, "Graphics.Open", 1);
  m_bTrendExpanded = PF.RdInt(Section, "Trends.Open", 0);
  m_bOtherExpanded = PF.RdInt(Section, "Other.Open", 0);
  m_bClassExpanded = PF.RdInt(Section, "Classes.Open", 0);
  m_bNodesExpanded = PF.RdInt(Section, "Nodes.Open", 0);
  m_bFlowsExpanded = PF.RdInt(Section, "Flows.Open", 0);
  m_bTearsExpanded = PF.RdInt(Section, "Tears.Open", 0);
  m_bMUFlowsExpanded = PF.RdInt(Section, "Flows.Open.MU", 0);
  m_bSPFlowsExpanded = PF.RdInt(Section, "Flows.Open.SP", 0);

  m_hWndItems[0] = m_hOtherItem;
  m_hWndItems[1] = m_hGraphItem;
  m_hWndItems[2] = m_hTrendItem;

  m_iShowWhatFlows  = PF.RdInt(Section, "Show.What", m_iShowWhatFlows);
  m_bBranchMU       = PF.RdInt(Section, "Branch.Makeups", m_bBranchMU?1:0)!=0;
  m_bBranchSP       = PF.RdInt(Section, "Branch.Spills", m_bBranchSP?1:0)!=0;

  sm_bInited=true;

  ShowWindow(SW_SHOW);
  UpdateDialogControls(this, FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------

void CExploreScd::PositionControls(int cx, int cy)
  {
  if (!sm_bInited)
    return;

  int CW=m_ClientRctInit.Width();
  int CH=m_ClientRctInit.Height();

  for (int i=0; i<CtrlIDCount; i++)
    {
    CWnd &Wnd = *GetDlgItem(CtrlIDs[i].m_ID);
    int X1=m_CtrlRcts[i].left;
    int Y1=m_CtrlRcts[i].top;
    int X2=X1+m_CtrlRcts[i].Width();
    int Y2=Y1+m_CtrlRcts[i].Height();
    if (CtrlIDs[i].m_TrkTL & TrkLeft)
      {}

    if (CtrlIDs[i].m_TrkTL & TrkRight)
      X1+=cx-CW;
    if (CtrlIDs[i].m_TrkBR & TrkLeft)
      {}

    if (CtrlIDs[i].m_TrkBR & TrkRight)
      X2+=cx-CW;

    if (CtrlIDs[i].m_TrkTL & TrkTop)
      {}
    if (CtrlIDs[i].m_TrkTL & TrkBottom)
      Y1+=cy-CH;
    if (CtrlIDs[i].m_TrkBR & TrkTop)
      {}
    if (CtrlIDs[i].m_TrkBR & TrkBottom)
      Y2+=cy-CH;

    Wnd.SetWindowPos(NULL, X1, Y1, X2-X1, Y2-Y1, SWP_NOCOPYBITS|SWP_NOACTIVATE|SWP_NOZORDER);
    }
  };

//---------------------------------------------------------------------------

static int HpTagTest(void * p, void * q)
  {
  CXTTag * r1=(CXTTag*)p;
  CXTTag * r2=(CXTTag*)q;
  int Cmp=_stricmp(r1->m_sTag, r2->m_sTag);
  return Cmp<0;
  }

static int HpFlowTest(void * p, void * q)
  {
  CXTFlow * r1=(CXTFlow*)p;
  CXTFlow * r2=(CXTFlow*)q;
  int Cmp=_stricmp(r1->m_sTag, r2->m_sTag);
  return Cmp<0;
  }

static int HpTagHTest(void * p, void * q)
  {
  CXTTagHPair * r1=(CXTTagHPair*)p;
  CXTTagHPair * r2=(CXTTagHPair*)q;
  int Cmp=_stricmp(r1->m_pTag->m_sTag, r2->m_pTag->m_sTag);
  return Cmp<0;
  }

static int HpClassTest(void * p, void * q)
  {
  CXTClass * r1=(CXTClass*)p;
  CXTClass * r2=(CXTClass*)q;
  int Cmp=_stricmp(r1->m_sClassId, r2->m_sClassId);
  return Cmp<0;
  }

static int HpPageTest(void * p, void * q)
  {
  CXTPage * r1=(CXTPage*)p;
  CXTPage * r2=(CXTPage*)q;
  int Cmp=_stricmp(r1->m_sPageId, r2->m_sPageId);
  return Cmp<0;
  }

static int HpPageHTest(void * p, void * q)
  {
  CXTPageHPair * r1=(CXTPageHPair*)p;
  CXTPageHPair * r2=(CXTPageHPair*)q;
  int Cmp=_stricmp(r1->m_pPage->m_sPageId, r2->m_pPage->m_sPageId);
  return Cmp<0;
  }

//---------------------------------------------------------------------------

void CExploreScd::InitTags()
  {
  ClearTags();

  SFEFlwLib* pFlwLib = gs_pPrj->FlwLib();
  if (pFlwLib)
    {
    Strng_List RawTags;
    int NTags;

    //NTags = gs_pPrj->FlwLib()->FE_TagOperation(FETOp_GetActive, RawTags);
    //NTags=Max(1000, NTags);
    NTags=10000;

    m_TagMap.InitHashTable(FindNextPrimeNumber((UINT)(NTags*1.18)));
    m_HTagMap.InitHashTable(FindNextPrimeNumber((UINT)(NTags*1.18)));
    m_ClassMap.InitHashTable(FindNextPrimeNumber((UINT)(100)));
    m_PageMap.InitHashTable(FindNextPrimeNumber((UINT)(200)));
    m_FlowMap.InitHashTable(FindNextPrimeNumber((UINT)(NTags*1.18)));
    }
  };

//--------------------------------------------------------------------------

void CExploreScd::ClearTags()
  {
  m_TagMap.RemoveAll();
  m_HTagMap.RemoveAll();
  m_ClassMap.RemoveAll();
  m_PageMap.RemoveAll();
  //m_Monitor.RemoveAll();

  for (int i=0; i<m_Tags.GetCount(); i++)
    delete m_Tags[i];
  m_Tags.SetSize(0,256);

  for (int i=0; i<m_Pages.GetCount(); i++)
    delete m_Pages[i];
  m_Pages.SetSize(0,32);

  for (int i=0; i<m_Classes.GetCount(); i++)
    delete m_Classes[i];
  m_Classes.SetSize(0,32);
  };
//---------------------------------------------------------------------------

void CExploreScd::SavePos()
  {
  CProfINIFile PF(PrjIniFile());

  CRect Rct;
  GetWindowRect(&Rct);

  PF.WrInt(Section, "XPos", Rct.left);
  PF.WrInt(Section, "YPos", Rct.top);
  PF.WrInt(Section, "Width", Rct.Width());
  PF.WrInt(Section, "Height", Rct.Height());

  CRect SRct;
  ScdMainWnd()->GetClientRect(&SRct);
  ScdMainWnd()->ClientToScreen(&SRct);

  m_TopLeft = Rct.TopLeft();
  m_TopLeft -= SRct.TopLeft();
  //dbgpln(">>>>>> Save Pos         %5i %5i  %5i %5i", Rct.left, Rct.top, m_TopLeft.x, m_TopLeft.y); 
  }

//--------------------------------------------------------------------------

void CExploreScd::RestorePos()
  {
  CProfINIFile PF(PrjIniFile());

  CRect Rct;
  GetClientRect(&Rct);
  GetParent()->ClientToScreen(&Rct);
  int xPos = PF.RdInt(Section, "XPos", -10000);//Rct.left);
  int yPos = PF.RdInt(Section, "YPos", Rct.top);
  int CW = PF.RdInt(Section, "Width", Rct.Width());
  int CH = PF.RdInt(Section, "Height", Rct.Height());

  if (xPos<=-10000)
    {
    CRect Rct;
    GetParent()->GetClientRect(&Rct);
    GetParent()->ClientToScreen(&Rct);
    xPos=Rct.left;
    yPos=Rct.top+50;

    CRect IRect;
    CWnd * p=ScdMainWnd();
    if (dynamic_cast<CMainFrame*>(p)->GetInitRect(0, IRect))
      {
      xPos=IRect.left;
      yPos=IRect.top;
      CW=IRect.Width();
      CH=IRect.Height();
      }
    }
  SetWindowPos(NULL, 0,0, 1,1 , SWP_NOREPOSITION | SWP_NOZORDER | SWP_SHOWWINDOW); // set size small to force an OnSize
  SetVisibleWindowPos(this, xPos, yPos, CW, CH, true);
  }

//--------------------------------------------------------------------------

void CExploreScd::MainWndMovedTo(int x, int y)
  {
  //sm_bDoRefresh=false;
  if (sm_pTheWnd && x>=-30000 && y>=-30000)
    {
    int xPos = x+sm_pTheWnd->m_TopLeft.x;
    int yPos = y+sm_pTheWnd->m_TopLeft.y;
    CRect Rct;
    sm_pTheWnd->GetWindowRect(&Rct);
    //dbgpln(">>>>>> MainWndMovedTo   %5i %5i  %5i %5i", Rct.left, Rct.top, sm_pTheWnd->m_TopLeft.x, sm_pTheWnd->m_TopLeft.y); 
    SetVisibleWindowPos(sm_pTheWnd, xPos, yPos, Rct.Width(), Rct.Height(), true);
    }
  };

//--------------------------------------------------------------------------

void CExploreScd::SaveProfile()
  {
  CProfINIFile PF(PrjIniFile());
  CString S;

  SavePos();
  m_TagFilterRule.GetWindowText(S);
  PF.WrStr(Section, "TagFilterRule", S);
  m_TagFilter.GetWindowText(S);
  PF.WrStr(Section, "TagFilter", S);
  };

//--------------------------------------------------------------------------

void CExploreScd::SaveExpanded()
  {
  CProfINIFile PF(PrjIniFile());
  if (m_hGraphItem)
    {
    PF.WrInt(Section, "Graphics.Open", (m_Tree.GetItemState(m_hGraphItem, TVIS_EXPANDED)&TVIS_EXPANDED)!=0?1:0);
    PF.WrInt(Section, "Trends.Open",   (m_Tree.GetItemState(m_hTrendItem, TVIS_EXPANDED)&TVIS_EXPANDED)!=0?1:0);
    PF.WrInt(Section, "Other.Open",    (m_Tree.GetItemState(m_hOtherItem, TVIS_EXPANDED)&TVIS_EXPANDED)!=0?1:0);
    PF.WrInt(Section, "Classes.Open",  (m_Tree.GetItemState(m_hClassItem, TVIS_EXPANDED)&TVIS_EXPANDED)!=0?1:0);
    PF.WrInt(Section, "Nodes.Open",    (m_Tree.GetItemState(m_hNodesItem, TVIS_EXPANDED)&TVIS_EXPANDED)!=0?1:0);
    PF.WrInt(Section, "Flows.Open",    (m_Tree.GetItemState(m_hFlowsItem, TVIS_EXPANDED)&TVIS_EXPANDED)!=0?1:0);
    PF.WrInt(Section, "Tears.Open",    (m_Tree.GetItemState(m_hTearsItem, TVIS_EXPANDED)&TVIS_EXPANDED)!=0?1:0);
    PF.WrInt(Section, "MUFlows.Open",  m_hMUFlowsItem && (m_Tree.GetItemState(m_hMUFlowsItem, TVIS_EXPANDED)&TVIS_EXPANDED)!=0?1:0);
    PF.WrInt(Section, "SPFlows.Open",  m_hSPFlowsItem && (m_Tree.GetItemState(m_hSPFlowsItem, TVIS_EXPANDED)&TVIS_EXPANDED)!=0?1:0);

    PF.WrInt(Section, "Show.What", m_iShowWhatFlows);
    PF.WrInt(Section, "Branch.Makeups", m_bBranchMU?1:0);
    PF.WrInt(Section, "Branch.Spills", m_bBranchSP?1:0);
    }
  }

//--------------------------------------------------------------------------

void CExploreScd::OpenIt()
  {
  sm_bDoRefresh=false;
  if (sm_pTheWnd==NULL)
    sm_pTheWnd=new CExploreScd(AfxGetMainWnd());
  else
    sm_pTheWnd->ShowWindow(SW_NORMAL);
  RefreshIt(true);

  CProfINIFile PF(PrjIniFile());
  PF.WrInt(Section, "Open", 1);

  sm_pTheWnd->RestorePos();  
  }

//--------------------------------------------------------------------------

void CExploreScd::CloseIt()
  {
  if (sm_pTheWnd && PrjIniFile())
    {
    sm_pTheWnd->SaveExpanded();
    sm_pTheWnd->SaveProfile();

    sm_pTheWnd->ShowWindow(SW_HIDE);

    CProfINIFile PF(PrjIniFile());
    PF.WrInt(Section, "Open", 0);
    }
  }

//--------------------------------------------------------------------------

bool CExploreScd::TestOpen()
  {
  CProfINIFile PF(PrjIniFile());
  return PF.RdInt(Section, "Open", 0)!=0;
  };

//--------------------------------------------------------------------------

void CExploreScd::RefreshIt(BOOL Complete)
  {
  sm_bDoRefresh=true;
  if (Complete)
    sm_bDoRefreshAll=true;
  }

void CExploreScd::ChkRefreshIt()
  {
  if (sm_bInited && sm_bDoRefresh && sm_pTheWnd && sm_pTheWnd->m_hWnd)
    {
    //if (sm_bDoRefreshAll)
    //  sm_pTheWnd->BuildTags();
    //else
    sm_pTheWnd->ReBuildTags();

    sm_bDoRefresh=false;
    sm_bDoRefreshAll=false;
    }
  }

BOOL CExploreScd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
  {
  HD_NOTIFY* pNM = (HD_NOTIFY*)lParam;
  //dbgpln("CExploreScd::OnNotify %6i", pNM->hdr.code);
  switch (pNM->hdr.code)
    {
    case NM_KILLFOCUS:
      {
      int sss=0;
      break;
      }
    case NM_LDOWN:
      {
      int sss=0;
      break;
      }
    case TVN_GETDISPINFO:
      {
      NMTVDISPINFO * pnmv = (NMTVDISPINFO*)lParam;
      if (pnmv->item.lParam!=0)//>(DWORD)20)
        pnmv->item.pszText=reinterpret_cast<CXTTreeInfo*>((void*)pnmv->item.lParam)->Text();//"XXXXXXX";
      else
        pnmv->item.pszText="XXXXXXX";
      //return TRUE;
      break;
      }
    case HDN_ITEMCLICK:
      {
      LPNMHEADER phdn = (LPNMHEADER) lParam;
      int i=phdn->iItem;

      break;
      }
    default:
      {
      int xx=0;
      break;
      }
    }

  return CDialog::OnNotify(wParam, lParam, pResult);
  }

//BOOL CExploreScd::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
//  {
//
//  BOOL CExploreScd::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
//  }

//---------------------------------------------------------------------------

void CExploreScd::GetRawTags()
  {
  SFEFlwLib* pFlwLib = gs_pPrj->FlwLib();
  if (pFlwLib)
    {
    Strng_List RawTags;
    int NTags;

    RawTags.Append("*");
    NTags = gs_pPrj->FlwLib()->FE_TagOperation(FETOp_GetActive, RawTags);


    for (Strng *pRawTag=RawTags.First(); pRawTag; pRawTag=RawTags.Next())
      {
      if (pRawTag->XStrICmp(PlantModelTag)==0 || pRawTag->XStrICmp(PlantAreaTag)==0)
        continue;

      CXTTag *pTag;
      if (m_TagMap.Lookup(pRawTag->Str(), pTag))
        {
        pTag->m_InUse=true;
        pTag->m_ModelOK=true;
#if dbgAdd
        dbgpln("Tag  InUse %s", pTag->m_sTag);
#endif
        }
      else
        {

        Strng ClassId;
        flag b = gs_pPrj->RequestModelClassId(pRawTag->Str(), ClassId);
        if (!b)
          ClassId="";

        for (int iClass=0; iClass<m_Classes.GetCount(); iClass++)
          if (m_Classes[iClass]->m_sClassId.CompareNoCase(ClassId())==0)
            break;
        if (iClass==m_Classes.GetCount())
          {
          CXTClass *pClass=new CXTClass(this, ClassId());
          m_Classes.Add(pClass);
          m_ClassMap.SetAt(pClass->m_sClassId, pClass);
          pClass->m_hClassItem=InsertItem(LPSTR_TEXTCALLBACK, Img_Class, GetTreeInfo(pClass), m_hClassItem, TVI_SORT); 
          }

        pTag=new CXTTag(this, pRawTag->Str(), m_Classes[iClass]);
        pTag->m_ModelOK=true;

#if dbgAdd   
        dbgpln("Tag  New   %-25s %s", m_Classes[iClass]->m_sClassId, pTag->m_sTag);
#endif
        m_Tags.Add(pTag);
        m_TagMap.SetAt(pTag->m_sTag, pTag);
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CExploreScd::GetRawFlows()
  {
  SFEFlwLib* pFlwLib = gs_pPrj->FlwLib();
  if (pFlwLib)
    {
    Strng_List RawFlows;
    int NFlows;

    RawFlows.Append("*");
    NFlows = gs_pPrj->FlwLib()->FE_TagOperation(FETOp_GetActiveConns, RawFlows);


    for (Strng *pRawFlow=RawFlows.First(); pRawFlow; pRawFlow=RawFlows.Next())
      {
      //if (pRawFlow->XStrICmp(PlantModelTag)==0 || pRawTag->XStrICmp(PlantAreaTag)==0)
      //  continue;

      CXTFlow *pFlow;
      if (m_FlowMap.Lookup(pRawFlow->Str(), pFlow))
        {
        pFlow->m_InUse=true;
        pFlow->m_ModelOK=true;
        pFlow->m_iFlowType=UnPackConnectFlowType(pRawFlow->Index());
        pFlow->m_iUseStatus=UnPackConnectUseStatus(pRawFlow->Index());
#if dbgAdd
        dbgpln("Flow  InUse %4i %4i %s", pFlow->m_iFlowType, pFlow->m_iUseStatus, pFlow->m_sTag);
#endif
        }
      else
        {

        //Strng ClassId;
        //flag b = gs_pPrj->RequestModelClassId(pRawFlow->Str(), ClassId);
        //if (!b)
        //  ClassId="";

        //for (int iClass=0; iClass<m_Classes.GetCount(); iClass++)
        //  if (m_Classes[iClass]->m_sClassId.CompareNoCase(ClassId())==0)
        //    break;
        //if (iClass==m_Classes.GetCount())
        //  {
        //  CXTClass *pClass=new CXTClass(this, ClassId());
        //  m_Classes.Add(pClass);
        //  m_ClassMap.SetAt(pClass->m_sClassId, pClass);
        //  pClass->m_hClassItem=InsertItem(LPSTR_TEXTCALLBACK, Img_Class, GetTreeInfo(pClass), m_hClassItem, TVI_SORT); 
        //  }

        pFlow=new CXTFlow(this, pRawFlow->Str(), UnPackConnectFlowType(pRawFlow->Index()), UnPackConnectUseStatus(pRawFlow->Index()));//, m_Classes[iClass]);
        //pFlow=new CXTFlow(this, pRawFlow->Str(), 1, 09);//, m_Classes[iClass]);
        pFlow->m_ModelOK=true;

#if dbgAdd   
        dbgpln("Flow  New   %4i %4i %s", pFlow->m_iFlowType, pFlow->m_iUseStatus, pFlow->m_sTag);
#endif
        m_Flows.Add(pFlow);
        m_FlowMap.SetAt(pFlow->m_sTag, pFlow);
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CExploreScd::GetRawTears()
  {
  SFEFlwLib* pFlwLib = gs_pPrj->FlwLib();
  if (pFlwLib)
    {
    Strng_List RawTears;
    int NTears;

    RawTears.Append("*");
    NTears = gs_pPrj->FlwLib()->FE_TagOperation(FETOp_GetTears, RawTears);


    for (Strng *pRawTear=RawTears.First(); pRawTear; pRawTear=RawTears.Next())
      {
      int xxx=0;
      //if (pRawTear->XStrICmp(PlantModelTag)==0 || pRawTag->XStrICmp(PlantAreaTag)==0)
      //  continue;

      CXTTear *pTear;
      if (m_TearMap.Lookup(pRawTear->Str(), pTear))
        {
        pTear->m_InUse=true;
        pTear->m_ModelOK=true;
        //pTear->m_iTearType=UnPackConnectTearType(pRawTear->Index());
        //pTear->m_iUseStatus=UnPackConnectUseStatus(pRawTear->Index());
#if dbgAdd
        //dbgpln("Tear  InUse %4i %4i %s", pTear->m_iTearType, pTear->m_iUseStatus, pTear->m_sTag);
        dbgpln("Tear  %s", pTear->m_sTag);
#endif
        }
      else
        {

        //Strng ClassId;
        //flag b = gs_pPrj->RequestModelClassId(pRawTear->Str(), ClassId);
        //if (!b)
        //  ClassId="";

        //for (int iClass=0; iClass<m_Classes.GetCount(); iClass++)
        //  if (m_Classes[iClass]->m_sClassId.CompareNoCase(ClassId())==0)
        //    break;
        //if (iClass==m_Classes.GetCount())
        //  {
        //  CXTClass *pClass=new CXTClass(this, ClassId());
        //  m_Classes.Add(pClass);
        //  m_ClassMap.SetAt(pClass->m_sClassId, pClass);
        //  pClass->m_hClassItem=InsertItem(LPSTR_TEXTCALLBACK, Img_Class, GetTreeInfo(pClass), m_hClassItem, TVI_SORT); 
        //  }

        pTear=new CXTTear(this, pRawTear->Str());//, UnPackConnectTearType(pRawTear->Index()), UnPackConnectUseStatus(pRawTear->Index()));//, m_Classes[iClass]);
        //pTear=new CXTTear(this, pRawTear->Str(), 1, 09);//, m_Classes[iClass]);
        pTear->m_ModelOK=true;

#if dbgAdd   
        //dbgpln("Tear  New   %4i %4i %s", pTear->m_iTearType, pTear->m_iUseStatus, pTear->m_sTag);
        dbgpln("Tear  New   %s", pTear->m_sTag);
#endif
        m_Tears.Add(pTear);
        m_TearMap.SetAt(pTear->m_sTag, pTear);
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CExploreScd::GetRawPages(bool ChangesOK)
  {
  CWindowLists WL;

  m_WindowCount = WL.BuildSingleList();
  m_GraphicCount = WL.GetGrfWndCount();
  m_TrendCount = WL.GetTrendWndCount();

  LPCTSTR ExcludeWnds[] = { "Command",  "Order of Evaluation", "SysCAD ", NULL};
  LPCTSTR TrimWnds[]    = { "Messages:", NULL};
  LPCTSTR PageExts[] = { ".scg",  ".trn", ".spj", NULL};

  for (int i=0; i<m_WindowCount; i++)
    {
    Strng PgName=WL.Wnds[i].m_sName;
    Strng PgId=WL.Wnds[i].m_sName;

    for (int j=0; TrimWnds[j]; j++)
      {
      if (_strnicmp(PgName(), TrimWnds[j], strlen(TrimWnds[j]))==0)
        {
        PgId=TrimWnds[j];
        break;
        }
      }

    Strng PgExt(PgName);
    PgExt.FnExt();

    if (PgExt.GetLength()>0)
      {
      for (int j=0; PageExts[j]; j++)
        {
        if (_stricmp(PgExt(), PageExts[j])==0)
          {
          PgId.FnName();
          break;
          }
        }
      }

    CXTPage *pPg;
    if (m_PageMap.Lookup(PgId(), pPg))
      {
      pPg->m_InUse=true;
#if dbgAdd   
      dbgpln("Page InUse %s", pPg->m_sPageId);
#endif
      }
    else
      {
      for (int j=0; ExcludeWnds[j]; j++)
        {
        if (_strnicmp(WL.Wnds[i].m_sName, ExcludeWnds[j], strlen(ExcludeWnds[j]))==0)
          goto NextWindow;
        }

      // ASSERT_ALWAYS(ChangesOK, "Unexpected New Page", __FILE__, __LINE__);

      CXTPage *pPage=new CXTPage(this, PgType2TrIDs[WL.Wnds[i].iType], PgId(), PgName());
      pPage->m_iType=WL.Wnds[i].iType;
      
      WINDOWPLACEMENT WP;
      WP.length=sizeof(WP);
      WL.Wnds[i].pWnd->GetWindowPlacement(&WP);

      switch (WP.showCmd)
        {
        case SW_MINIMIZE:
        case SW_HIDE:
          {
          int xxx=0;
          }
        }

      pPage->m_InUse=true;

#if dbgAdd   
      dbgpln("Page New   %s", pPage->m_sPageId);
#endif
      m_Pages.Add(pPage);
      m_PageMap.SetAt(pPage->m_sPageId, pPage);
      }

NextWindow:;
    }

  }

//---------------------------------------------------------------------------

void CExploreScd::FindTagPages()
  {
  if (gs_pPrj->AllGrfLoaded())
    {
    for (int iTmpl=iGraphTemplate; iTmpl<=iVisioTemplate; iTmpl++)
      {
      if (ScdApp()->TemplateExists(iTmpl))
        {
        POSITION Pos = ScdApp()->Template(iTmpl).GetFirstDocPosition();
        while (Pos)
          {
          CGrfDoc* pGDoc = (CGrfDoc*)(ScdApp()->Template(iTmpl).GetNextDoc(Pos));
          CString GrfTitle((LPCTSTR)(pGDoc->GetTitle()));
          while (GrfTitle.GetLength()>4 && GrfTitle.Right(4).CompareNoCase(".scg")==0)
            GrfTitle.Delete(GrfTitle.GetLength()-4,4);

          CXTPage *pPage;
          if (m_PageMap.Lookup(GrfTitle, pPage))
            {
            pPage->m_NoModelList.Clear();
            pPage->m_pGrfDoc=pGDoc;

            int NGrfTags = pGDoc->GetTagList(pPage->m_GrfTagList);
            for (Strng * pTagLst=pPage->m_GrfTagList.First(); pTagLst; pTagLst=pPage->m_GrfTagList.Next())
              {
#if dbgAdd   
              dbgp("Find %s", pTagLst->Str());
#endif
              CXTTag *pTag;
              CXTFlow *pFlow;
              CXTTear *pTear;
              if (m_TagMap.Lookup(pTagLst->Str(), pTag))
                { 
                // Tag Exists
                // Add to Page if reqd
                CXTTagHPair *pTH;
                if (!pPage->m_TagHMap.Lookup(pTag, pTH))
                  {
                  pTH=new CXTTagHPair(pTag);
                  pPage->m_TagHs.Add(pTH);
                  pPage->m_TagHMap.SetAt(pTag, pTH);
#if dbgAdd   
                  dbgp("Add Tag2Page %-25s << %s", pPage->m_sPageId, pTag->m_sTag);
#endif
                  }

                // Add to Tag if reqd
                CXTPageHPair *pPH;
                if (!pTag->m_PHMap.Lookup(pPage, pPH))
                  {
                  pPH=new CXTPageHPair(pPage);
                  pTag->m_Pages.Add(pPH);
                  pTag->m_PHMap.SetAt(pPage, pPH);
#if dbgAdd   
                  dbgp("Add Page2Tag %-25s << %s", pTag->m_sTag, pPage->m_sPageId);
#endif
                  }
                }
              else if (m_FlowMap.Lookup(pTagLst->Str(), pFlow))
                {
                int xxx=0;
                }
              else if (m_TearMap.Lookup(pTagLst->Str(), pTear))
                {
                int xxx=0;
                }
              else
                { 
                Strng ClassId("Missing");

                for (int iClass=0; iClass<m_Classes.GetCount(); iClass++)
                  if (m_Classes[iClass]->m_sClassId.CompareNoCase(ClassId())==0)
                    break;
                if (iClass==m_Classes.GetCount())
                  {
                  CXTClass *pClass=new CXTClass(this, ClassId());
                  m_Classes.Add(pClass);
                  m_ClassMap.SetAt(pClass->m_sClassId, pClass);
                  pClass->m_hClassItem=InsertItem(LPSTR_TEXTCALLBACK, Img_Class, GetTreeInfo(pClass), m_hClassItem, TVI_SORT); 
                  }

                CXTTag *pTag=new CXTTag(this, pTagLst->Str(), m_Classes[iClass]);
                pTag->m_ModelOK=false;

#if dbgAdd   
                dbgp("Tag  NoMdl%-25s %s", m_Classes[iClass]->m_sClassId, pTag->m_sTag);
#endif
                m_Tags.Add(pTag);
                m_TagMap.SetAt(pTag->m_sTag, pTag);

                }
#if dbgAdd   
              dbgpln("");
#endif
              }
            }
          }
        }
      }

    }
  }

//---------------------------------------------------------------------------

void CExploreScd::RemoveUnusedItems()
  {
  for (int t=0; t<m_Tags.GetCount(); t++)
    {
    CXTTag *pTag=m_Tags[t];
    if (!pTag->m_InUse)
      {
      RemoveTagFromTree(pTag);
      for (int p=0; p<pTag->m_Pages.GetCount(); p++)
        {
        // Remove Page under Tag
        CXTPageHPair *pPH=pTag->m_Pages[p];

        // Remove Tag entries under the Page where the tag is shown
        CXTPage *pPg=pPH->m_pPage;
        for (int i=0; i<pPg->m_TagHs.GetCount(); i++)
          {
          CXTTagHPair *pTH=pPg->m_TagHs[i];
          if (pTH->m_pTag==pTag)
            {
            HTREEITEM h=pTH->m_hTag;
            if (h)
              m_Tree.DeleteItem(h);
            pPg->m_TagHMap.RemoveKey(pTag);
            pPg->m_TagHs.RemoveAt(i);
            delete pTH;
            break;
            }
          }
        }

#if dbgAdd   
      dbgpln("Tag   Remov %s", pTag->m_sTag);
#endif

      m_TagMap.RemoveKey(pTag->m_sTag);
      m_Tags.RemoveAt(t--);
      delete pTag;
      }
    }

  for (int t=0; t<m_Flows.GetCount(); t++)
    {
    CXTFlow *pFlow=m_Flows[t];
    if (!pFlow->m_InUse)
      {
      RemoveFlowFromTree(pFlow);
      //for (int p=0; p<pTag->m_Pages.GetCount(); p++)
      //  {
      //  // Remove Page under Tag
      //  CXTPageHPair *pPH=pTag->m_Pages[p];

      //  // Remove Tag entries under the Page where the tag is shown
      //  CXTPage *pPg=pPH->m_pPage;
      //  for (int i=0; i<pPg->m_TagHs.GetCount(); i++)
      //    {
      //    CXTTagHPair *pTH=pPg->m_TagHs[i];
      //    if (pTH->m_pTag==pTag)
      //      {
      //      HTREEITEM h=pTH->m_hTag;
      //      if (h)
      //        m_Tree.DeleteItem(h);
      //      pPg->m_TagHMap.RemoveKey(pTag);
      //      pPg->m_TagHs.RemoveAt(i);
      //      delete pTH;
      //      break;
      //      }
      //    }
      //  }

#if dbgAdd   
      dbgpln("Flow  Remov %s", pFlow->m_sTag);
#endif

      m_FlowMap.RemoveKey(pFlow->m_sTag);
      m_Flows.RemoveAt(t--);
      delete pFlow;
      }
    }

  for (int t=0; t<m_Tears.GetCount(); t++)
    {
    CXTTear *pTear=m_Tears[t];
    if (!pTear->m_InUse)
      {
      RemoveTearFromTree(pTear);
      //for (int p=0; p<pTag->m_Pages.GetCount(); p++)
      //  {
      //  // Remove Page under Tag
      //  CXTPageHPair *pPH=pTag->m_Pages[p];

      //  // Remove Tag entries under the Page where the tag is shown
      //  CXTPage *pPg=pPH->m_pPage;
      //  for (int i=0; i<pPg->m_TagHs.GetCount(); i++)
      //    {
      //    CXTTagHPair *pTH=pPg->m_TagHs[i];
      //    if (pTH->m_pTag==pTag)
      //      {
      //      HTREEITEM h=pTH->m_hTag;
      //      if (h)
      //        m_Tree.DeleteItem(h);
      //      pPg->m_TagHMap.RemoveKey(pTag);
      //      pPg->m_TagHs.RemoveAt(i);
      //      delete pTH;
      //      break;
      //      }
      //    }
      //  }

#if dbgAdd   
      dbgpln("Tear  Remov %s", pTear->m_sTag);
#endif

      m_TearMap.RemoveKey(pTear->m_sTag);
      m_Tears.RemoveAt(t--);
      delete pTear;
      }
    }

  for (int p=0; p<m_Pages.GetCount(); p++)
    {
    CXTPage *pPage=m_Pages[p];
    if (!pPage->m_InUse)
      {
      RemovePageFromTree(pPage);
      for (int t=0; t<pPage->m_TagHs.GetCount(); t++)
        {
        // Remove Tag under Page
        CXTTagHPair *pTH=pPage->m_TagHs[t];

        // Remove Page entries under the Tag where the Page is shown
        CXTTag *pTg=pTH->m_pTag;
        for (int i=0; i<pTg->m_Pages.GetCount(); i++)
          {
          CXTPageHPair *pPH=pTg->m_Pages[i];
          if (pPH->m_pPage==pPage)
            {
            HTREEITEM h=pPH->m_hPage;
            if (h)
              m_Tree.DeleteItem(h);
            //pTg->m_Pages.RemoveKey(i);
            pTg->m_Pages.RemoveAt(i);
            delete pPH;
            break;
            }
          }
        }

#if dbgAdd   
      dbgpln("Page  Remov %s", pPage->m_sPageId);
#endif


      m_PageMap.RemoveKey(pPage->m_sPageId);
      m_Pages.RemoveAt(p--);
      delete pPage;
      }
    }

  };

//---------------------------------------------------------------------------

void CExploreScd::SortAll()
  {
  if (m_Tags.GetCount())
    {
    HpSort(m_Tags.GetCount(), (void**)&m_Tags[0], HpTagTest);
    for (int i=0; i<m_Tags.GetCount(); i++)
      {
      CXTTag &Tg=*m_Tags[i];
      if (Tg.m_Pages.GetCount())
        HpSort(Tg.m_Pages.GetCount(), (void**)&Tg.m_Pages[0], HpPageHTest);
      }
    }

  if (m_Flows.GetCount())
    {
    HpSort(m_Flows.GetCount(), (void**)&m_Flows[0], HpFlowTest);
    //for (int i=0; i<m_Flows.GetCount(); i++)
    //  {
    //  CXTTag &Tg=*m_Flows[i];
    //  if (Tg.m_Pages.GetCount())
    //    HpSort(Tg.m_Pages.GetCount(), (void**)&Tg.m_Pages[0], HpPageHTest);
    //  }
    }

  if (m_Classes.GetCount())
    {
    HpSort(m_Classes.GetCount(), (void**)&m_Classes[0], HpClassTest);
    for (int i=0; i<m_Classes.GetCount(); i++)
      {
      CXTClass &Cl=*m_Classes[i];
      if (Cl.xm_Tags.GetCount())
        HpSort(Cl.xm_Tags.GetCount(), (void**)&Cl.xm_Tags[0], HpTagTest);
      }
    }

  if (m_Pages.GetCount())
    {
    HpSort(m_Pages.GetCount(), (void**)&m_Pages[0], HpPageTest);
    for (int i=0; i<m_Pages.GetCount(); i++)
      {
      CXTPage &Pg=*m_Pages[i];
      if (Pg.m_TagHs.GetCount())
        HpSort(Pg.m_TagHs.GetCount(), (void**)&Pg.m_TagHs[0], HpTagHTest);
      }
    }
  }

//---------------------------------------------------------------------------

void CExploreScd::FinaliseHeaders()
  {
  m_Tree.Expand(m_hGraphItem, m_bGraphExpanded?TVE_EXPAND:TVE_COLLAPSE);
  m_Tree.Expand(m_hTrendItem, m_bTrendExpanded?TVE_EXPAND:TVE_COLLAPSE);
  m_Tree.Expand(m_hOtherItem, m_bOtherExpanded?TVE_EXPAND:TVE_COLLAPSE);
  m_Tree.Expand(m_hClassItem, m_bClassExpanded?TVE_EXPAND:TVE_COLLAPSE);
  m_Tree.Expand(m_hNodesItem, m_bNodesExpanded?TVE_EXPAND:TVE_COLLAPSE);
  m_Tree.Expand(m_hFlowsItem, m_bFlowsExpanded?TVE_EXPAND:TVE_COLLAPSE);
  m_Tree.Expand(m_hTearsItem, m_bTearsExpanded?TVE_EXPAND:TVE_COLLAPSE);
  if (m_hMUFlowsItem)
    m_Tree.Expand(m_hMUFlowsItem, m_bSPFlowsExpanded?TVE_EXPAND:TVE_COLLAPSE);
  if (m_hMUFlowsItem)
    m_Tree.Expand(m_hMUFlowsItem, m_bMUFlowsExpanded?TVE_EXPAND:TVE_COLLAPSE);
  }

//---------------------------------------------------------------------------

// called on Project load/close & page open/close
void CExploreScd::BuildTags()
  {
  CStopWatch SW;
  SW.Start();
#if dbgAdd
  dbgpln("BuildTags  ====================================");
#endif

  CWaitMsgCursor WaitMsg("Updating Explorer");

#if (!dbgHoldLockUpdate)
  LockWindowUpdate();
#endif

  m_Tree.Expand(m_hGraphItem, TVE_COLLAPSE|TVE_COLLAPSERESET);
  m_Tree.Expand(m_hTrendItem, TVE_COLLAPSE|TVE_COLLAPSERESET);
  m_Tree.Expand(m_hOtherItem, TVE_COLLAPSE|TVE_COLLAPSERESET);
  m_Tree.Expand(m_hClassItem, TVE_COLLAPSE|TVE_COLLAPSERESET);
  m_Tree.Expand(m_hNodesItem, TVE_COLLAPSE|TVE_COLLAPSERESET);
  m_Tree.Expand(m_hFlowsItem, TVE_COLLAPSE|TVE_COLLAPSERESET);
  m_Tree.Expand(m_hTearsItem, TVE_COLLAPSE|TVE_COLLAPSERESET);

  ClearTags();

#if dbgTime
  dbgpln("A:%10.3f", SW.Lap()*1e3);
#endif

  GetRawTags();
#if dbgTime
  dbgpln("B:%10.3f", SW.Lap()*1e3);
#endif

  GetRawFlows();
#if dbgTime
  dbgpln("B1:%10.3f", SW.Lap()*1e3);
#endif

  GetRawTears();
#if dbgTime
  dbgpln("B2:%10.3f", SW.Lap()*1e3);
#endif

  GetRawPages(true);
#if dbgTime
  dbgpln("D:%10.3f", SW.Lap()*1e3);
#endif

  FindTagPages();
#if dbgTime
  dbgpln("E:%10.3f", SW.Lap()*1e3);
#endif

  SortAll();
#if dbgTime
  dbgpln("F:%10.3f", SW.Lap()*1e3);
#endif

  SetFilter();
#if dbgTime
  dbgpln("G:%10.3f", SW.Lap()*1e3);          
#endif

  for (int i=0; i<m_Pages.GetCount(); i++)
    AddPageToTree(m_Pages[i]);

  CXTTag *pPrevTag=NULL;
  for (int t=0; t<m_Tags.GetCount(); t++)
    {
    CXTTag *pTag=m_Tags[t];
    if (pTag->m_Selected)
      {
      AddTagToTree(pTag, pPrevTag);
      pPrevTag=pTag;
      }
    }

  UpdateFlowBranches(true);

  CXTFlow *pPrevFlow=NULL;
  for (int t=0; t<m_Flows.GetCount(); t++)
    {
    CXTFlow *pFlow=m_Flows[t];
    if (pFlow->ShowIt())
      {
      AddFlowToTree(pFlow, pPrevFlow);
      pPrevFlow=pFlow;
      }
    }

  UpdateFlowBranches(false);

#if dbgTime
  dbgpln("H:%10.3f", SW.Lap()*1e3);
#endif

#if dbgDumpAll
  DumpAll("Build");
#endif

  UpdateSelectDisplay();
  FinaliseHeaders();

#if dbgTime
  dbgpln("J:%10.3f", SW.Lap()*1e3);
#endif

  UpdateData(FALSE);
  UpdateDialogControls(this, FALSE);

  Invalidate();
#if (!dbgHoldLockUpdate)
  UnlockWindowUpdate();
#endif

#if dbgAdd
  dbgpln("Done       ====================================");
#endif
  };

//---------------------------------------------------------------------------

// Only called when tags are added/removed.
void CExploreScd::ReBuildTags()  
  {
  CStopWatch SW;
  SW.Start();
#if dbgAdd
  dbgpln("RebuildTags====================================");
#endif

  CWaitMsgCursor WaitMsg("Updating Explorer");

#if (!dbgHoldLockUpdate)
  LockWindowUpdate();
#endif

  for (int i=0; i<m_Tags.GetCount(); i++)
    m_Tags[i]->m_InUse=false;

  for (int i=0; i<m_Flows.GetCount(); i++)
    m_Flows[i]->m_InUse=false;
                                                
  for (int i=0; i<m_Pages.GetCount(); i++)
    m_Pages[i]->m_InUse=false;

#if dbgTime
  dbgpln("A:%10.3f", SW.Lap()*1e3);
#endif

  GetRawTags();
#if dbgTime
  dbgpln("B:%10.3f", SW.Lap()*1e3);
#endif

  GetRawFlows();
#if dbgTime
  dbgpln("B1:%10.3f", SW.Lap()*1e3);
#endif

  GetRawTears();
#if dbgTime
  dbgpln("B2:%10.3f", SW.Lap()*1e3);
#endif

  GetRawPages(true);//false);
#if dbgTime
  dbgpln("D:%10.3f", SW.Lap()*1e3);
#endif

  RemoveUnusedItems();
#if dbgTime
  dbgpln("d:%10.3f", SW.Lap()*1e3);
#endif

  FindTagPages();
#if dbgTime
  dbgpln("E:%10.3f", SW.Lap()*1e3);
#endif

  SortAll();
#if dbgTime
  dbgpln("F:%10.3f", SW.Lap()*1e3);
#endif

  SetFilter();
#if dbgTime
  dbgpln("G:%10.3f", SW.Lap()*1e3);          
#endif

  for (int i=0; i<m_Pages.GetCount(); i++)
    AddPageToTree(m_Pages[i]);

  CXTTag *pPrevTag=NULL;
  for (int t=0; t<m_Tags.GetCount(); t++)
    {
    CXTTag *pTag=m_Tags[t];
    if (pTag->m_InUse && pTag->m_Selected)
      {
      AddTagToTree(pTag, pPrevTag);
      pPrevTag=pTag;
      }
    else
      {
      RemoveTagFromTree(pTag);
      }
    }

  UpdateFlowBranches(true);
  
  CXTFlow *pPrevFlow=NULL;
  for (int t=0; t<m_Flows.GetCount(); t++)
    {
    CXTFlow *pFlow=m_Flows[t];
    if (pFlow->m_InUse && pFlow->ShowIt())
      {
      AddFlowToTree(pFlow, pPrevFlow);
      pPrevFlow=pFlow;
      }
    else
      {
      RemoveFlowFromTree(pFlow);
      }
    }

  UpdateFlowBranches(false);

  CXTTear *pPrevTear=NULL;
  for (int t=0; t<m_Tears.GetCount(); t++)
    {
    CXTTear *pTear=m_Tears[t];
    if (pTear->m_InUse && pTear->ShowIt())
      {
      AddTearToTree(pTear, pPrevTear);
      pPrevTear=pTear;
      }
    else
      {
      RemoveTearFromTree(pTear);
      }
    }

  //UpdateTearBranches(false);


#if dbgTime
  dbgpln("H:%10.3f", SW.Lap()*1e3);
#endif

#if dbgDumpAll
  DumpAll("ReBuild");
#endif

  UpdateSelectDisplay();
  FinaliseHeaders();

#if dbgTime
  dbgpln("J:%10.3f", SW.Lap()*1e3);
#endif

  UpdateData(FALSE);
  UpdateDialogControls(this, FALSE);

  Invalidate();
#if (!dbgHoldLockUpdate)
  UnlockWindowUpdate();
#endif

#if dbgAdd
  dbgpln("Done       ====================================");
#endif

  };

//--------------------------------------------------------------------------

bool CExploreScd::LoadTagTree(bool DoKbdTest)
  {
  if (!sm_bInited)
    return true;
#if dbgAdd
  dbgpln("LoadTagTree====================================");
#endif

  DoKbdTest=false;

  CXTTag *pPrevTag=NULL;
  for (int i=0; i<m_Tags.GetCount(); i++)
    {
    CXTTag * pTag=m_Tags[i];
    if (pTag->m_Selected)
      {
      AddTagToTree(pTag, pPrevTag);
      pPrevTag=pTag;
      }
    else
      RemoveTagFromTree(pTag);

    if (DoKbdTest)
      {
      dbgpln("Sleep %s %s", pTag->m_Selected?"SEL":"   ", pTag->m_sTag);

      Sleep(50); // while testing
      MSG msg;
      if (PeekMessage(&msg, GetSafeHwnd(), WM_KEYFIRST, WM_KEYLAST, PM_NOREMOVE))
        {
        dbgpln("Peek1 %3i", msg.message);

        switch (msg.message)
          {
          //    case WM_CHAR:
          case WM_KEYUP:
          //  return false;
          //case WM_KEYDOWN:
            return true;
            //    case WM_KEYDOWN:
            //      break;
            //    case WM_KEYUP:
            //      return false;
          };
        }
      }
    }

  //--------------------------

  UpdateFlowBranches(true);
  CXTFlow *pPrevFlow=NULL;
  for (int i=0; i<m_Flows.GetCount(); i++)
    {
    CXTFlow * pFlow=m_Flows[i];
    if (pFlow->ShowIt())
      {
      AddFlowToTree(pFlow, pPrevFlow);
      pPrevFlow=pFlow;
      }
    else
      RemoveFlowFromTree(pFlow);

    if (DoKbdTest)
      {
      dbgpln("Sleep %s %s", pFlow->m_Selected?"SEL":"   ", pFlow->m_sTag);

      Sleep(50); // while testing
      MSG msg;
      if (PeekMessage(&msg, GetSafeHwnd(), WM_KEYFIRST, WM_KEYLAST, PM_NOREMOVE))
        {
        dbgpln("Peek1 %3i", msg.message);

        switch (msg.message)
          {
          //    case WM_CHAR:
          case WM_KEYUP:
          //  return false;
          //case WM_KEYDOWN:
            return true;
            //    case WM_KEYDOWN:
            //      break;
            //    case WM_KEYUP:
            //      return false;
          };
        }
      }
    }

  UpdateFlowBranches(false);

  //--------------------------

  CXTTear *pPrevTear=NULL;
  for (int i=0; i<m_Tears.GetCount(); i++)
    {
    CXTTear * pTear=m_Tears[i];
    if (pTear->m_Selected)
      {
      AddTearToTree(pTear, pPrevTear);
      pPrevTear=pTear;
      }
    else
      RemoveTearFromTree(pTear);
    }

  //--------------------------

  for (int p=0; p<m_Pages.GetCount(); p++)
    {
    CXTPage &Pg=*m_Pages[p];
    HTREEITEM hPrev=0;
    for (int t=0; t<Pg.m_TagHs.GetCount(); t++)
      {
      CXTTagHPair &P=*Pg.m_TagHs[t];
      if (P.m_pTag->m_Selected)
        {
        if (!P.m_hTag)
          P.m_hTag=InsertItem(LPSTR_TEXTCALLBACK, P.m_pTag->Icon(), GetTreeInfo(P.m_pTag), Pg.m_hPage, hPrev ? hPrev : TVI_FIRST);
        m_Tree.SetItemImage(P.m_hTag, P.m_pTag->Icon(), P.m_pTag->Icon());
        hPrev=P.m_hTag;
        }
      else
        {
        if (P.m_hTag)
          {
          m_Tree.DeleteItem(P.m_hTag);
          P.m_hTag=0;
          }
        }
      if (DoKbdTest)
        {
        dbgpln("Sleep %s %s", P.m_pTag->m_Selected?"SEL":"   ", P.m_pTag->m_sTag);
        Sleep(50); // while testing
        MSG msg;
        if (PeekMessage(&msg, GetSafeHwnd(), WM_KEYFIRST, WM_KEYLAST, PM_NOREMOVE))
          {
          dbgpln("Peek2 %3i", msg.message);
          switch (msg.message)
            {
            case WM_KEYUP:
            //  return false;
            //case WM_KEYDOWN:
              return true;
          //  case WM_CHAR:
          //    return false;
            };
          }
        }
      }
    }
#if dbgAdd
  dbgpln("Done       ====================================");
#endif
  return true;
  };

//--------------------------------------------------------------------------

void CExploreScd::AddTagToTree(CXTTag *pTag, CXTTag * pPrev)
  {
#if dbgAdd   
  dbgp("Add  Tag  Tree  %-25s", pTag->m_sTag);
#endif
  if (!pTag->m_hTreeItem)
    {
    pTag->m_hTreeItem = InsertItem(LPSTR_TEXTCALLBACK, 
      pTag->Icon(), 
      GetTreeInfo(pTag), m_hNodesItem, 
      pPrev ? pPrev->m_hTreeItem : TVI_FIRST);

#if dbgAdd   
    dbgp(" [Add Tree %#08x]",pTag->m_hTreeItem);
#endif
    m_HTagMap.SetAt(pTag->m_hTreeItem, pTag);
    HTREEITEM hPrev=NULL;
    for (int i=0; i<pTag->m_Pages.GetCount(); i++)
      {
      CXTPageHPair &P=*pTag->m_Pages[i];
      if (P.m_pPage->m_Selected)
        {
        P.m_hPage=InsertItem(LPSTR_TEXTCALLBACK, P.m_pPage->m_pGrfDoc->bModelsActive ? Img_GrfActive:Img_GrfInActive, GetTreeInfo(P.m_pPage), pTag->m_hTreeItem, hPrev?hPrev:TVI_FIRST);
        hPrev=P.m_hPage;
        }
      }

    ASSERT_ALWAYS(!pTag->m_hClassItem, "Tag Class ??", __FILE__, __LINE__);

#if dbgAdd   
      dbgp(" [Class %-20s]", pTag->m_pClass->m_sClassId);
#endif
    pTag->m_hClassItem = InsertItem(LPSTR_TEXTCALLBACK, pTag->Icon(), GetTreeInfo(pTag), pTag->m_pClass->m_hClassItem, TVI_LAST);


    for (int p=0; p<pTag->m_Pages.GetCount(); p++)
      {
      CXTPage *pPage=pTag->m_Pages[p]->m_pPage;

      HTREEITEM hPrev=NULL;
      for (int t=0; t<pPage->m_TagHs.GetCount(); t++)
        {
        CXTTagHPair *pTH=pPage->m_TagHs[t];

        if (!pTH->m_hTag)
          pTH->m_hTag= InsertItem(LPSTR_TEXTCALLBACK, pTH->m_pTag->Icon(), GetTreeInfo(pTH->m_pTag), pPage->m_hPage, hPrev?hPrev:TVI_FIRST);
        m_Tree.SetItemImage(pTH->m_hTag, pTH->m_pTag->Icon(), pTH->m_pTag->Icon());
        hPrev=pTH->m_hTag;
        }
      }
    }
  m_Tree.SetItemImage(pTag->m_hTreeItem, pTag->Icon(), pTag->Icon());
#if dbgAdd   
  dbgpln("");
#endif
  }

//--------------------------------------------------------------------------

void CExploreScd::RemoveTagFromTree(CXTTag * pTag)
  {
  if (pTag->m_hTreeItem)
    {
#if dbgAdd   
    dbgpln("Rem  Tag  Tree %#08x %s", pTag->m_hTreeItem, pTag->m_sTag);
#endif
    m_Tree.DeleteItem(pTag->m_hTreeItem);
    m_HTagMap.RemoveKey(pTag->m_hTreeItem);
    pTag->m_hTreeItem=NULL;
    }
  if (pTag->m_hClassItem)
    {
#if dbgAdd   
    dbgpln("Rem  Tag  Class %s", pTag->m_sTag);
#endif
    m_Tree.DeleteItem(pTag->m_hClassItem);
    pTag->m_hClassItem=NULL;
    }

  for (int p=0; p<pTag->m_Pages.GetCount(); p++)
    {
    CXTPage *pPage=pTag->m_Pages[p]->m_pPage;
    for (int t=0; t<pPage->m_TagHs.GetCount(); t++)
      {
      CXTTagHPair *pTH=pPage->m_TagHs[t];
      if (pTH->m_pTag==pTag && pTH->m_hTag)
        {
        m_Tree.DeleteItem(pTH->m_hTag);
        pTH->m_hTag=NULL;
        }
      }
    }
  }

//--------------------------------------------------------------------------

void CExploreScd::UpdateFlowBranches(bool AtStart)
  {
  if (AtStart)
    {
    if (m_bBranchMU && m_hMUFlowsItem==NULL)
      {
      m_hMUFlowsItem = InsertItem(LPSTR_TEXTCALLBACK, Img_FlowMakeupIU, GetTreeInfo(m_MUFlowTitle), m_hFlowsItem, TVI_FIRST);
      }
    if (m_bBranchSP && m_hSPFlowsItem==NULL)
      {
      m_hSPFlowsItem = InsertItem(LPSTR_TEXTCALLBACK, Img_FlowSpillIU, GetTreeInfo(m_SPFlowTitle), m_hFlowsItem, m_hMUFlowsItem ? m_hMUFlowsItem : TVI_FIRST);
      }
    }
  else
    {
    if (!m_bBranchMU && m_hMUFlowsItem!=NULL)
      {
      m_Tree.DeleteItem(m_hMUFlowsItem);
      m_hMUFlowsItem = NULL;
      }
    if (!m_bBranchSP && m_hSPFlowsItem!=NULL)
      {
      m_Tree.DeleteItem(m_hSPFlowsItem);
      m_hSPFlowsItem = NULL;
      }
    };
  };

//--------------------------------------------------------------------------

void CExploreScd::AddFlowToTree(CXTFlow *pFlow, CXTFlow * pPrev)
  {
#if dbgAdd   
  dbgp("Add  Flow  Tree  %-25s", pFlow->m_sTag);
#endif

  if (pFlow->m_hTreeItem && pFlow->m_hTreeItem!=pFlow->ReqdParentItem())
    RemoveFlowFromTree(pFlow);

  if (!pFlow->m_hTreeItem)
    {

    pFlow->m_hTreeItem = InsertItem(LPSTR_TEXTCALLBACK, 
      pFlow->Icon(), 
      GetTreeInfo(pFlow), 
      pFlow->ReqdParentItem(), 
      pPrev ? pPrev->m_hTreeItem : TVI_LAST);

#if dbgAdd   
    dbgp(" [Add Tree %#08x]",pFlow->m_hTreeItem);
#endif
//    m_HFlowMap.SetAt(pFlow->m_hTreeItem, pFlow);
//    HTREEITEM hPrev=NULL;
//    for (int i=0; i<pFlow->m_Pages.GetCount(); i++)
//      {
//      CXTPageHPair &P=*pFlow->m_Pages[i];
//      if (P.m_pPage->m_Selected)
//        {
//        P.m_hPage=InsertItem(LPSTR_TEXTCALLBACK, P.m_pPage->m_pGrfDoc->bModelsActive ? Img_GrfActive:Img_GrfInActive, GetTreeInfo(P.m_pPage), pFlow->m_hTreeItem, hPrev?hPrev:TVI_FIRST);
//        hPrev=P.m_hPage;
//        }
//      }
//
//    ASSERT_ALWAYS(!pFlow->m_hClassItem, "Flow Class ??", __FILE__, __LINE__);
//
//#if dbgAdd   
//      dbgp(" [Class %-20s]", pFlow->m_pClass->m_sClassId);
//#endif
//    pFlow->m_hClassItem = InsertItem(LPSTR_TEXTCALLBACK, pFlow->Icon(), GetTreeInfo(pFlow), pFlow->m_pClass->m_hClassItem, TVI_LAST);


    //for (int p=0; p<pFlow->m_Pages.GetCount(); p++)
    //  {
    //  CXTPage *pPage=pFlow->m_Pages[p]->m_pPage;

    //  HTREEITEM hPrev=NULL;
    //  for (int t=0; t<pPage->m_FlowHs.GetCount(); t++)
    //    {
    //    CXTFlowHPair *pTH=pPage->m_FlowHs[t];

    //    if (!pTH->m_hFlow)
    //      pTH->m_hFlow= InsertItem(LPSTR_TEXTCALLBACK, pTH->m_pFlow->Icon(), GetTreeInfo(pTH->m_pFlow), pPage->m_hPage, hPrev?hPrev:TVI_FIRST);
    //    m_Tree.SetItemImage(pTH->m_hFlow, pTH->m_pFlow->Icon(), pTH->m_pFlow->Icon());
    //    hPrev=pTH->m_hFlow;
    //    }
    //  }
    }
  m_Tree.SetItemImage(pFlow->m_hTreeItem, pFlow->Icon(), pFlow->Icon());
#if dbgAdd   
  dbgpln("");
#endif
  }

//--------------------------------------------------------------------------

void CExploreScd::RemoveFlowFromTree(CXTFlow * pFlow)
  {
  if (pFlow->m_hTreeItem)
    {
#if dbgAdd   
    dbgpln("Rem  Flow  Tree %#08x %s", pFlow->m_hTreeItem, pFlow->m_sTag);
#endif
    m_Tree.DeleteItem(pFlow->m_hTreeItem);
    //m_HFlowMap.RemoveKey(pFlow->m_hTreeItem);
    pFlow->m_hTreeItem=NULL;
    }
//  if (pFlow->m_hClassItem)
//    {
//#if dbgAdd   
//    dbgpln("Rem  Flow  Class %s", pFlow->m_sFlow);
//#endif
//    m_Tree.DeleteItem(pFlow->m_hClassItem);
//    pFlow->m_hClassItem=NULL;
//    }

  //for (int p=0; p<pFlow->m_Pages.GetCount(); p++)
  //  {
  //  CXTPage *pPage=pFlow->m_Pages[p]->m_pPage;
  //  for (int t=0; t<pPage->m_FlowHs.GetCount(); t++)
  //    {
  //    CXTFlowHPair *pTH=pPage->m_FlowHs[t];
  //    if (pTH->m_pFlow==pFlow && pTH->m_hFlow)
  //      {
  //      m_Tree.DeleteItem(pTH->m_hFlow);
  //      pTH->m_hFlow=NULL;
  //      }
  //    }
  //  }
  }

//--------------------------------------------------------------------------

void CExploreScd::AddTearToTree(CXTTear *pTear, CXTTear * pPrev)
  {
#if dbgAdd   
  dbgp("Add  Tear  Tree  %-25s", pTear->m_sTag);
#endif

  if (pTear->m_hTreeItem && pTear->m_hTreeItem!=pTear->ReqdParentItem())
    RemoveTearFromTree(pTear);

  if (!pTear->m_hTreeItem)
    {

    pTear->m_hTreeItem = InsertItem(LPSTR_TEXTCALLBACK, 
      pTear->Icon(), 
      GetTreeInfo(pTear), 
      pTear->ReqdParentItem(), 
      pPrev ? pPrev->m_hTreeItem : TVI_LAST);

#if dbgAdd   
    dbgp(" [Add Tree %#08x]",pTear->m_hTreeItem);
#endif
    }
  m_Tree.SetItemImage(pTear->m_hTreeItem, pTear->Icon(), pTear->Icon());
#if dbgAdd   
  dbgpln("");
#endif
  }

//--------------------------------------------------------------------------

void CExploreScd::RemoveTearFromTree(CXTTear * pTear)
  {
  if (pTear->m_hTreeItem)
    {
#if dbgAdd   
    dbgpln("Rem  Tear  Tree %#08x %s", pTear->m_hTreeItem, pTear->m_sTag);
#endif
    m_Tree.DeleteItem(pTear->m_hTreeItem);
    //m_HTearMap.RemoveKey(pTear->m_hTreeItem);
    pTear->m_hTreeItem=NULL;
    }
  }

//--------------------------------------------------------------------------

void CExploreScd::AddPageToTree(CXTPage * pPage)
  {
  if (!pPage->m_hPage)
    pPage->m_hPage=InsertItem(LPSTR_TEXTCALLBACK, 
                   pPage->m_iType==1 && !pPage->m_pGrfDoc->bModelsActive ? PgType2ImgIDsInactive[pPage->m_iType]:PgType2ImgIDsActive[pPage->m_iType], 
                   GetTreeInfo(pPage), m_hWndItems[pPage->m_iType], TVI_LAST); 
  }

void CExploreScd::RemovePageFromTree(CXTPage * pPage)
  {
  if (pPage->m_hPage)
    m_Tree.DeleteItem(pPage->m_hPage);
  pPage->m_hPage=NULL;
  }

//--------------------------------------------------------------------------

void CExploreScd::DumpAll(LPCTSTR Where)
  {
  dbgpln("Dump======= %-8s ===============================", Where);

  dbgpln("TagCount    %5i", m_Tags.GetCount());
  dbgpln("TagMapCount %5i", m_TagMap.GetCount());
  for (int t=0; t<m_Tags.GetCount(); t++)
    {
    CXTTag & Tg=*m_Tags[t];
    dbgpln("Tag  %-25s %-10s %-10s %-10s", Tg.m_sTag, 
      Tg.m_hTreeItem?"InTree":"", 
      Tg.m_hClassItem?"InClass":"", 
      Tg.m_ModelOK?"OK":""); 
    for (int p=0; p<Tg.m_Pages.GetCount(); p++)
      dbgpln("     %-25s %-10s", Tg.m_Pages[p]->m_pPage->m_sPageId, Tg.m_Pages[p]->m_hPage?"InPage":""); 
    }

  dbgpln("PageCount    %5i", m_Pages.GetCount());
  dbgpln("PageMapCount %5i", m_PageMap.GetCount());
  for (int p=0; p<m_Pages.GetCount(); p++)
    {
    CXTPage & Pg=*m_Pages[p];
    dbgpln("Page %-25s %-10s", Pg.m_sPageId, Pg.m_hPage?"InTree":""); 
    for (int t=0; t<Pg.m_TagHs.GetCount(); t++)
      dbgpln("     %-25s %-10s", Pg.m_TagHs[t]->m_pTag->m_sTag, Pg.m_TagHs[t]->m_hTag?"InTag":""); 
    for (Strng *pS=Pg.m_NoModelList.First(); pS; pS=pS->Next())
      dbgpln("     %-25s --> NO MODEL", pS->Str()); 
    }

  dbgpln("====================================================");
  };

//--------------------------------------------------------------------------

void CExploreScd::SetFilter()
  {
  if (!sm_bInited)
    return;

  for (int i=0; i<m_Tags.GetCount(); i++)
    SetFilter(*m_Tags[i], i==0);

  for (int i=0; i<m_Flows.GetCount(); i++)
    SetFilter(*m_Flows[i], i==0);

  for (int i=0; i<m_Tears.GetCount(); i++)
    SetFilter(*m_Tears[i], i==0);

  UpdateSelectDisplay();
  }

//-------------------------------------------------------------------------

void CExploreScd::SetFilter(CXTCommon & Tg, BOOL DoSetup)
  {
  switch (m_FilterRule)
    {
    case eFROff:
      {
      Tg.m_Selected=true;
      break;
      }
    case eFRContains:
      {
      BOOL Selected;
      if (m_bCaseSens)
        Selected=strstr(Tg.m_sTag, m_FilterString)!=NULL;
      else
        Selected=strstr(Tg.m_sTagLwr, m_FilterString)!=NULL;
      Tg.m_Selected=Selected;
      //}
      break;
      }
    case eFRWildCard:
    case eFRRegExp:
      {
      if (DoSetup)
        {
        if (!m_pFilterRE)
          m_pFilterRE=new IRegExpPtr("VBScript.RegExp");
        (*m_pFilterRE)->Global     = _variant_t(true);//' Set do all occurs
        (*m_pFilterRE)->IgnoreCase = _variant_t(!m_bCaseSens);//' Set case insensitivity.
        switch (m_FilterRule)
          {
          case eFRWildCard:
            {
            CString S("^");
            for (int i=0; i<m_FilterString.GetLength(); i++)
              {
              if (m_FilterString[i]=='*')
                S+=".*";
              else if (m_FilterString[i]=='?')
                S+=".";
              else if (m_FilterString[i]!='^' && m_FilterString[i]!='$')
                S+=m_FilterString[i];
              }
            S+="$";
            (*m_pFilterRE)->Pattern = _bstr_t(S);
            break;
            }
          case eFRRegExp:
            {
            CString S("^");
            for (int i=0; i<m_FilterString.GetLength(); i++)
              {
              if (m_FilterString[i]!='^' && m_FilterString[i]!='$')
                S+=m_FilterString[i];
              }
            S+="$";
            (*m_pFilterRE)->Pattern = _bstr_t(S);
            break;
            }
          default:
            (*m_pFilterRE)->Pattern = _bstr_t(m_FilterString);
            break;
          }
        }

      try
        {
        Tg.m_Selected=((*m_pFilterRE)->Test(_bstr_t(Tg.m_sTag))==VARIANT_TRUE);
        }
      catch(...)
        {
        LogError("TagChangeFilter", 0, "Failure in Regular expressions");
        Tg.m_Selected=true;
        }
      //  }
      break;
      }
    }
  }

////-------------------------------------------------------------------------
//
//void CExploreScd::SetFlowFilter(CXTFlow & Tg, BOOL DoSetup)
//  {
//  switch (m_FilterRule)
//    {
//    case eFROff:
//      {
//      Tg.m_Selected=true;
//      break;
//      }
//    case eFRContains:
//      {
//      BOOL Selected;
//      if (m_bCaseSens)
//        Selected=strstr(Tg.m_sTag, m_FilterString)!=NULL;
//      else
//        Selected=strstr(Tg.m_sTagLwr, m_FilterString)!=NULL;
//      Tg.m_Selected=Selected;
//      //}
//      break;
//      }
//    case eFRWildCard:
//    case eFRRegExp:
//      {
//      if (DoSetup)
//        {
//        if (!m_pFilterRE)
//          m_pFilterRE=new IRegExpPtr("VBScript.RegExp");
//        (*m_pFilterRE)->Global     = _variant_t(true);//' Set do all occurs
//        (*m_pFilterRE)->IgnoreCase = _variant_t(!m_bCaseSens);//' Set case insensitivity.
//        switch (m_FilterRule)
//          {
//          case eFRWildCard:
//            {
//            CString S("^");
//            for (int i=0; i<m_FilterString.GetLength(); i++)
//              {
//              if (m_FilterString[i]=='*')
//                S+=".*";
//              else if (m_FilterString[i]=='?')
//                S+=".";
//              else if (m_FilterString[i]!='^' && m_FilterString[i]!='$')
//                S+=m_FilterString[i];
//              }
//            S+="$";
//            (*m_pFilterRE)->Pattern = _bstr_t(S);
//            break;
//            }
//          case eFRRegExp:
//            {
//            CString S("^");
//            for (int i=0; i<m_FilterString.GetLength(); i++)
//              {
//              if (m_FilterString[i]!='^' && m_FilterString[i]!='$')
//                S+=m_FilterString[i];
//              }
//            S+="$";
//            (*m_pFilterRE)->Pattern = _bstr_t(S);
//            break;
//            }
//          default:
//            (*m_pFilterRE)->Pattern = _bstr_t(m_FilterString);
//            break;
//          }
//        }
//
//      try
//        {
//        Tg.m_Selected=((*m_pFilterRE)->Test(_bstr_t(Tg.m_sTag))==VARIANT_TRUE);
//        }
//      catch(...)
//        {
//        LogError("TagChangeFilter", 0, "Failure in Regular expressions");
//        Tg.m_Selected=true;
//        }
//      //  }
//      break;
//      }
//    }
//  }

//-------------------------------------------------------------------------

void CExploreScd::UpdateSelectDisplay()
  {
  for (int i=0; i<m_Classes.GetCount(); i++)
    {
    CXTClass & Cl=*m_Classes[i];
    Cl.m_nCount=0;
    Cl.m_nSelected=0;
    }

  m_nTagsSelected=0;
  for (int i=0; i<m_Tags.GetCount(); i++)
    {
    CXTTag &Tg=*m_Tags[i];
    if (Tg.m_Selected)
      {
      m_nTagsSelected++;
      Tg.m_pClass->m_nSelected++;
      }
    Tg.m_pClass->m_nCount++;
    }

  m_nFlowsSelected=0;
  for (int i=0; i<m_Flows.GetCount(); i++)
    {
    CXTFlow &Fl=*m_Flows[i];
    if (Fl.ShowIt())
      {
      //dbgpln("-- Show %s", Fl.m_sTag);
      m_nFlowsSelected++;
      //Tg.m_pClass->m_nSelected++;
      }
    else
      {
      //dbgpln("--      %s", Fl.m_sTag);
      }
    //Tg.m_pClass->m_nCount++;
    }

  m_nTearsSelected=0;
  for (int i=0; i<m_Tears.GetCount(); i++)
    {
    CXTTear &Fl=*m_Tears[i];
    if (Fl.ShowIt())
      {
      //dbgpln("-- Show %s", Fl.m_sTag);
      m_nTearsSelected++;
      //Tg.m_pClass->m_nSelected++;
      }
    else
      {
      //dbgpln("--      %s", Fl.m_sTag);
      }
    //Tg.m_pClass->m_nCount++;
    }


  m_GraphicTitle.m_pStr->Format("Graphics (%i)", m_GraphicCount);
  m_TrendTitle.m_pStr->Format("Trends (%i)", m_TrendCount);
  m_ClassTitle.m_pStr->Format("Classes (%i)", m_Classes.GetCount());

  if (m_nTagsSelected==m_Tags.GetCount())
    m_NodeTitle.m_pStr->Format("Nodes (%i)", m_Tags.GetCount());
  else
    m_NodeTitle.m_pStr->Format("Nodes (%i of %i)", m_nTagsSelected, m_Tags.GetCount());

  if (m_nFlowsSelected==m_Flows.GetCount())
    m_FlowTitle.m_pStr->Format("Flows (%i)", m_Flows.GetCount());
  else
    m_FlowTitle.m_pStr->Format("Flows (%i of %i)", m_nFlowsSelected, m_Flows.GetCount());

  if (m_nTearsSelected==m_Tears.GetCount())
    m_TearTitle.m_pStr->Format("Tears (%i)", m_Tears.GetCount());
  else
    m_TearTitle.m_pStr->Format("Tears (%i of %i)", m_nTearsSelected, m_Tears.GetCount());

  }

//--------------------------------------------------------------------------

void CExploreScd::OnOK() 
  {
  m_Tree.SetFocus();
  }

void CExploreScd::OnCancel() 
  {
  CloseIt();
  }

//--------------------------------------------------------------------------

void CExploreScd::OnSize(UINT nType, int cx, int cy) 
  {
  CDialog::OnSize(nType, cx, cy);
  if (sm_bInited)
    {
    PositionControls(cx, cy);
    InvalidateRect(NULL);
    SavePos();
    }
  }

//--------------------------------------------------------------------------

void CExploreScd::OnMove(int x, int y)
  {
  CDialog::OnMove(x, y);
  if (sm_bInited)
    {
    SavePos();
    }
  //CPoint 
  //m_TopLeft;

  };

//--------------------------------------------------------------------------

static void CalcTileSizes(int Cnt, int WidthAvail, int HeightAvail, double WndRatio, int & Width, int & Height)
  {
  if (Cnt>0)
    {
    Height = (int)(WndRatio*WidthAvail);
    double Ratio = (double)HeightAvail/Height;
    double cols = sqrt((double)Cnt/Ratio);
    int Cols = Max(1, (int)(cols+0.5));
    int Rows = (int)((double)Cnt/Cols+0.5);
    Width = WidthAvail/Cols;
    Height = (int)(WndRatio*Width);
    if (Rows*Height>HeightAvail+1)
      {
      Cols++;
      Width = WidthAvail/Cols;
      Height = (int)(WndRatio*Width);
      }
    }
  }

void CExploreScd::ArrangeWindows(int RqdLayoutStyle/*=-1*/) 
  {
  WINDOWPLACEMENT wp;
  wp.length = sizeof(wp);

  CWndArrangeDlg Dlg;
  if (RqdLayoutStyle<0)
    if (Dlg.DoModal()!=IDOK)
      return;

  CWaitCursor Wait;
  const flag LockUpdate = 0;
  CWindowLists WL;
  const int Count = WL.BuildZOrder(true);
  int MainWndIndex = -1;
  for (int i=0; MainWndIndex<0 && i<Count; i++)
    if (WL.Wnds[i].pWnd==WL.pMainWnd)
      MainWndIndex=i;
  CWnd* pCurTopWnd = CWindowLists::GetCurrentTopWindow();
  if (Dlg.m_MaximiseMain)
    WL.pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
  else
    WL.pMainWnd->ShowWindow(SW_SHOWNOACTIVATE);
  if (LockUpdate)
    WL.pMainWnd->LockWindowUpdate();
  CWnd* pMDIWnd = WL.GetMDIClientWnd();
  CRect R;
  pMDIWnd->GetWindowRect(R);
  const int MainWidth = R.Width();
  const int MainHeight = R.Height();
  const double WndRatio = (double)MainHeight/MainWidth;
  const int AccWidth = Max(460, (int)(MainWidth/3));
  const int cycaption = GetSystemMetrics(SM_CYCAPTION);

  const int TrnWndCnt = WL.GetTrendWndCount();
  const int TrnDelta = cycaption-1;
  int TrnTop = -1;
  int TrnLeft = -1;
  int TrnWidth;
  int TrnHeight;
  int TrnOrd=Count+1;
  switch (Dlg.m_TrnStyle)
    {
    case 0: break;
    case 1:
      TrnTop = cycaption;
      TrnWidth = Max(380, (int)(MainWidth*0.4));
      TrnHeight = (int)(MainHeight*0.7);
      break;
    case 2:
      TrnLeft = MainWidth/3;
      TrnWidth = MainWidth/2;
      TrnHeight = (int)(MainHeight*0.7);
      break;
    case 3:
      if (TrnWndCnt>0)
        CalcTileSizes(TrnWndCnt, MainWidth, MainHeight, WndRatio, TrnWidth, TrnHeight);
      break;
    case 4:
      {
      for (i=0; i<Count; i++)
        {
        if (i!=MainWndIndex && WL.Wnds[i].iType==2)
          {
          if (WL.Wnds[i].iZOrd<TrnOrd)
            {
            wp.length = sizeof(wp);
            WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
            TrnLeft   = wp.rcNormalPosition.left;
            TrnWidth  = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
            TrnTop    = wp.rcNormalPosition.top;
            TrnHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
            TrnOrd=WL.Wnds[i].iZOrd;
            }
          }
        }
      }
      break;
    }

  const int GrfWndCnt = WL.GetGrfWndCount();
  const int GrfDelta = cycaption-1;
  const int GrfWidthSpace = MainWidth-AccWidth+2;
  int GrfTop = -1;
  int GrfLeft = -1;
  int GrfWidth;
  int GrfHeight;
  int GrfOrd=Count+1;
  switch (Dlg.m_GrfStyle)
    {
    case 0: break;
    case 1:
      GrfWidth = Range((int)(MainWidth*0.4), (int)((MainWidth-AccWidth)-((GrfWndCnt-1)*GrfDelta)), GrfWidthSpace);
      GrfHeight = (int)(MainHeight*GrfWidth/MainWidth);
      break;
    case 2:
      if (GrfWndCnt>0)
        CalcTileSizes(GrfWndCnt, GrfWidthSpace, MainHeight, WndRatio, GrfWidth, GrfHeight);
      break;
    case 3:
      {
      for (i=0; i<Count; i++)
        {
        if (i!=MainWndIndex && WL.Wnds[i].iType==1)
          {
          if (WL.Wnds[i].iZOrd<GrfOrd)
            {
            wp.length = sizeof(wp);
            WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
            GrfLeft   = wp.rcNormalPosition.left;
            GrfWidth  = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
            GrfTop    = wp.rcNormalPosition.top;
            GrfHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
            GrfOrd=WL.Wnds[i].iZOrd;
            }
          }
        }
      }
      break;
    }

  for (i=0; i<Count; i++)
    if (i!=MainWndIndex)
      {
      if (WL.Wnds[i].iType==1)
        {
        if (Dlg.m_GrfStyle==0)
          {
          //CNM ??? WL.Wnds[i].pWnd->ShowWindow(SW_SHOWNOACTIVATE);
          }
        else
          {
          WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
          wp.length = sizeof(wp);
          wp.showCmd = SW_SHOWNOACTIVATE;
          wp.rcNormalPosition.left = GrfLeft;
          wp.rcNormalPosition.right = wp.rcNormalPosition.left+GrfWidth;
          wp.rcNormalPosition.top = GrfTop;
          wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+GrfHeight;
          WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
          if (Dlg.m_GrfStyle==3)
            {
            }
          else if (Dlg.m_GrfStyle==1)
            {
            GrfTop += GrfDelta;
            GrfLeft += GrfDelta;
            if (GrfTop>MainHeight-GrfHeight)
              {
              GrfTop = (int)(GrfDelta*2.5);
              GrfLeft = -1;
              }
            }
          else
            {
            GrfLeft += GrfWidth;
            if (GrfLeft>GrfWidthSpace-10)
              {
              GrfTop += GrfHeight;
              GrfLeft = -1;
              }
            }
          }
        }
      else if (WL.Wnds[i].iType==2)
        {
        if (Dlg.m_TrnStyle==0)
          {
          //CNM ??? WL.Wnds[i].pWnd->ShowWindow(SW_SHOWNOACTIVATE);
          }
        else
          {
          WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
          wp.length = sizeof(wp);
          wp.showCmd = SW_SHOWNOACTIVATE;
          wp.rcNormalPosition.left = TrnLeft;
          wp.rcNormalPosition.right = wp.rcNormalPosition.left+TrnWidth;
          wp.rcNormalPosition.top = TrnTop;
          wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+TrnHeight;
          WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
          if (Dlg.m_TrnStyle==4)
            {
            }
          else if (Dlg.m_TrnStyle==1)
            {
            if (TrnWndCnt>1)
              TrnLeft += ((MainWidth-TrnWidth)/(TrnWndCnt-1));
            }
          else if (Dlg.m_TrnStyle==2)
            {
            TrnTop += TrnDelta;
            TrnLeft += TrnDelta;
            if (TrnLeft>MainWidth-TrnWidth || TrnTop>MainHeight-TrnHeight)
              {
              TrnTop = -1;
              TrnLeft = -1;
              }
            }
          else
            {
            TrnLeft += TrnWidth;
            if (TrnLeft>MainWidth-10)
              {
              TrnTop += TrnHeight;
              TrnLeft = -1;
              }
            }
          }
        }
      else
        {//other windows...
        ASSERT(NAccessWnds==2);
        if (Dlg.m_OthStyle==0)
          {
          //CNM ??? WL.Wnds[i].pWnd->ShowWindow(SW_SHOWNOACTIVATE);
          }
        else if (WL.Wnds[i].pWnd==WL.pAccessWnd[0])
          {
          WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
          wp.length = sizeof(wp);
          wp.showCmd = SW_SHOWNOACTIVATE;
          wp.rcNormalPosition.left = MainWidth-AccWidth;
          wp.rcNormalPosition.right = MainWidth;
          wp.rcNormalPosition.top = -2;
          wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+MainHeight;
          WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
          }
        else if (WL.Wnds[i].pWnd==WL.pAccessWnd[1])
          {
          WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
          wp.length = sizeof(wp);
          wp.showCmd = SW_SHOWNOACTIVATE;
          wp.rcNormalPosition.left = MainWidth-AccWidth;
          wp.rcNormalPosition.right = MainWidth;
          wp.rcNormalPosition.top = -2;
          wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+MainHeight;
          WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
          }
        else if (WL.Wnds[i].pWnd==WL.pPrjWnd)
          {
          WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
          wp.length = sizeof(wp);
          wp.showCmd = SW_SHOWNOACTIVATE;
          const int PrjWidth = Min(MainWidth-2,930);
          wp.rcNormalPosition.left = (MainWidth-PrjWidth-2)/2;
          wp.rcNormalPosition.right = wp.rcNormalPosition.left+PrjWidth;
          wp.rcNormalPosition.top = cycaption;
          wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + (int)(MainHeight*0.8);
          WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
          }
        else if (WL.Wnds[i].pWnd==WL.pMsgWnd)
          {
          WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
          wp.length = sizeof(wp);
          wp.showCmd = SW_SHOWNOACTIVATE;
          wp.rcNormalPosition.left = 0;
          wp.rcNormalPosition.right = Max(400L, long(wp.rcNormalPosition.left+MainWidth-AccWidth));
          wp.rcNormalPosition.top = (int)(MainHeight/2)-cycaption;
          wp.rcNormalPosition.bottom = MainHeight-cycaption;
          WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
          //todo: centre message window splitter
          }
        else
          WL.Wnds[i].pWnd->ShowWindow(SW_SHOWNOACTIVATE);
        }
      }

    //minimise windows in required sequence...
    int Pass[3] = {1,2,0};
    if (!Dlg.m_GrfMinimise)
      Pass[0]=-1;
    if (!Dlg.m_TrnMinimise)
      Pass[1]=-1;
    if (!Dlg.m_OthMinimise)
      Pass[2]=-1;
    for (int j=0; j<3; j++)
      if (Pass[j]!=-1)
        {
        for (i=0; i<Count; i++)
          {
          if (WL.Wnds[i].iType==Pass[j] && i!=MainWndIndex)
            WL.Wnds[i].pWnd->ShowWindow(WL.Wnds[i].pWnd==pCurTopWnd ? SW_SHOWMINIMIZED : SW_SHOWMINNOACTIVE);
          }
        }

      if (LockUpdate)
        WL.pMainWnd->UnlockWindowUpdate();
      if (pCurTopWnd)
        pCurTopWnd->ShowWindow(pCurTopWnd->IsIconic() ? SW_SHOWMINIMIZED : SW_SHOWNORMAL);
      /*
      CWndArrangeDlg Dlg;
      if (RqdLayoutStyle<0)
      if (Dlg.DoModal()!=IDOK)
      return;

      CWaitCursor Wait;
      const flag LockUpdate = 0;
      CWindowLists WL;
      const int Count = WL.BuildSingleList(true);

      int MainWndIndex = -1;
      for (int i=0; MainWndIndex<0 && i<Count; i++)
      if (WL.Wnds[i].pWnd==WL.pMainWnd)
      MainWndIndex=i;
      CWnd* pCurTopWnd = CWindowLists::GetCurrentTopWindow();
      if (Dlg.m_MaximiseMain)
      WL.pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
      else
      WL.pMainWnd->ShowWindow(SW_SHOWNOACTIVATE);
      if (LockUpdate)
      WL.pMainWnd->LockWindowUpdate();
      CWnd* pMDIWnd = WL.GetMDIClientWnd();
      CRect R;
      pMDIWnd->GetWindowRect(R);
      const int MainWidth = R.Width();
      const int MainHeight = R.Height();
      const double WndRatio = (double)MainHeight/MainWidth;
      const int AccWidth = Max(460, (int)(MainWidth/3));
      const int cycaption = GetSystemMetrics(SM_CYCAPTION);

      const int TrnWndCnt = WL.GetTrendWndCount();
      const int TrnDelta = cycaption-1;
      int TrnTop = -1;
      int TrnLeft = -1;
      int TrnWidth;
      int TrnHeight;
      switch (Dlg.m_TrnStyle)
      {
      case 0: break;
      case 1:
      TrnTop = cycaption;
      TrnWidth = Max(380, (int)(MainWidth*0.4));
      TrnHeight = (int)(MainHeight*0.7);
      break;
      case 2:
      TrnLeft = MainWidth/3;
      TrnWidth = MainWidth/2;
      TrnHeight = (int)(MainHeight*0.7);
      break;
      case 3:
      if (TrnWndCnt>0)
      CalcTileSizes(TrnWndCnt, MainWidth, MainHeight, WndRatio, TrnWidth, TrnHeight);
      break;
      }

      const int GrfWndCnt = WL.GetGrfWndCount();
      const int GrfDelta = cycaption-1;
      const int GrfWidthSpace = MainWidth-AccWidth+2;
      int GrfTop = -1;
      int GrfLeft = -1;
      int GrfWidth;
      int GrfHeight;
      switch (Dlg.m_GrfStyle)
      {
      case 0: break;
      case 1:
      GrfWidth = Range((int)(MainWidth*0.4), (int)((MainWidth-AccWidth)-((GrfWndCnt-1)*GrfDelta)), GrfWidthSpace);
      GrfHeight = (int)(MainHeight*GrfWidth/MainWidth);
      break;
      case 2:
      if (GrfWndCnt>0)
      CalcTileSizes(GrfWndCnt, GrfWidthSpace, MainHeight, WndRatio, GrfWidth, GrfHeight);
      break;
      }

      WINDOWPLACEMENT wp;
      for (i=0; i<Count; i++)
      {
      if (i!=MainWndIndex)
      {
      if (WL.Wnds[i].iType==1)
      {
      if (Dlg.m_GrfStyle==0)
      {
      WL.Wnds[i].pWnd->ShowWindow(SW_SHOWNOACTIVATE);
      }
      else
      {
      WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
      wp.length = sizeof(wp);
      wp.showCmd = SW_SHOWNOACTIVATE;
      wp.rcNormalPosition.left = GrfLeft;
      wp.rcNormalPosition.right = wp.rcNormalPosition.left+GrfWidth;
      wp.rcNormalPosition.top = GrfTop;
      wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+GrfHeight;
      WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
      if (Dlg.m_GrfStyle==1)
      {
      GrfTop += GrfDelta;
      GrfLeft += GrfDelta;
      if (GrfTop>MainHeight-GrfHeight)
      {
      GrfTop = (int)(GrfDelta*2.5);
      GrfLeft = -1;
      }
      }
      else
      {
      GrfLeft += GrfWidth;
      if (GrfLeft>GrfWidthSpace-10)
      {
      GrfTop += GrfHeight;
      GrfLeft = -1;
      }
      }
      }
      }
      else if (WL.Wnds[i].iType==2)
      {
      if (Dlg.m_TrnStyle==0)
      {
      WL.Wnds[i].pWnd->ShowWindow(SW_SHOWNOACTIVATE);
      }
      else
      {
      WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
      wp.length = sizeof(wp);
      wp.showCmd = SW_SHOWNOACTIVATE;
      wp.rcNormalPosition.left = TrnLeft;
      wp.rcNormalPosition.right = wp.rcNormalPosition.left+TrnWidth;
      wp.rcNormalPosition.top = TrnTop;
      wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+TrnHeight;
      WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
      if (Dlg.m_TrnStyle==1)
      {
      if (TrnWndCnt>1)
      TrnLeft += ((MainWidth-TrnWidth)/(TrnWndCnt-1));
      }
      else if (Dlg.m_TrnStyle==2)
      {
      TrnTop += TrnDelta;
      TrnLeft += TrnDelta;
      if (TrnLeft>MainWidth-TrnWidth || TrnTop>MainHeight-TrnHeight)
      {
      TrnTop = -1;
      TrnLeft = -1;
      }
      }
      else
      {
      TrnLeft += TrnWidth;
      if (TrnLeft>MainWidth-10)
      {
      TrnTop += TrnHeight;
      TrnLeft = -1;
      }
      }
      }
      }
      else
      {//other windows...
      ASSERT(NAccessWnds==2);
      if (Dlg.m_OthStyle==0)
      {
      WL.Wnds[i].pWnd->ShowWindow(SW_SHOWNOACTIVATE);
      }
      else if (WL.Wnds[i].pWnd==WL.pAccessWnd[0])
      {
      WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
      wp.length = sizeof(wp);
      wp.showCmd = SW_SHOWNOACTIVATE;
      wp.rcNormalPosition.left = MainWidth-AccWidth;
      wp.rcNormalPosition.right = MainWidth;
      wp.rcNormalPosition.top = -2;
      wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+MainHeight;
      WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
      }
      else if (WL.Wnds[i].pWnd==WL.pAccessWnd[1])
      {
      WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
      wp.length = sizeof(wp);
      wp.showCmd = SW_SHOWNOACTIVATE;
      wp.rcNormalPosition.left = MainWidth-AccWidth;
      wp.rcNormalPosition.right = MainWidth;
      wp.rcNormalPosition.top = -2;
      wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+MainHeight;
      WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
      }
      else if (WL.Wnds[i].pWnd==WL.pPrjWnd)
      {
      WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
      wp.length = sizeof(wp);
      wp.showCmd = SW_SHOWNOACTIVATE;
      const int PrjWidth = Min(MainWidth-2,930);
      wp.rcNormalPosition.left = (MainWidth-PrjWidth-2)/2;
      wp.rcNormalPosition.right = wp.rcNormalPosition.left+PrjWidth;
      wp.rcNormalPosition.top = cycaption;
      wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + (int)(MainHeight*0.8);
      WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
      }
      else if (WL.Wnds[i].pWnd==WL.pMsgWnd)
      {
      WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
      wp.length = sizeof(wp);
      wp.showCmd = SW_SHOWNOACTIVATE;
      wp.rcNormalPosition.left = 0;
      wp.rcNormalPosition.right = Max(400L, long(wp.rcNormalPosition.left+MainWidth-AccWidth));
      wp.rcNormalPosition.top = (int)(MainHeight/2)-cycaption;
      wp.rcNormalPosition.bottom = MainHeight-cycaption;
      WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
      //todo: centre message window splitter
      }
      else
      WL.Wnds[i].pWnd->ShowWindow(SW_SHOWNOACTIVATE);
      }
      }
      }
      //minimise windows in required sequence...
      int Pass[3] = {1,2,0};
      if (!Dlg.m_GrfMinimise)
      Pass[0]=-1;
      if (!Dlg.m_TrnMinimise)
      Pass[1]=-1;
      if (!Dlg.m_OthMinimise)
      Pass[2]=-1;
      for (int j=0; j<3; j++)
      {
      if (Pass[j]!=-1)
      {
      for (i=0; i<Count; i++)
      {
      if (WL.Wnds[i].iType==Pass[j] && i!=MainWndIndex)
      WL.Wnds[i].pWnd->ShowWindow(WL.Wnds[i].pWnd==pCurTopWnd ? SW_SHOWMINIMIZED : SW_SHOWMINNOACTIVE);
      }
      }
      }

      if (LockUpdate)
      WL.pMainWnd->UnlockWindowUpdate();
      if (pCurTopWnd)
      pCurTopWnd->ShowWindow(pCurTopWnd->IsIconic() ? SW_SHOWMINIMIZED : SW_SHOWNORMAL);
      */
  }

//--------------------------------------------------------------------------

void CExploreScd::ActivateWnd(LPCTSTR Txt, CWnd *pToActivate)
  {
  if (pToActivate==NULL)
    {
    CWindowLists WL;
    if (WL.BuildSingleList()<0)
      return;

    const int Index = WL.Find(255, (char*)(const char*)Txt);
    if (Index<0) 
      return;

    ASSERT(Index>=0);
    pToActivate=WL.Wnds[Index].pWnd;
    }
  if (pToActivate)
    {
    if (pToActivate->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
      {
      if (pToActivate->GetFocus()!=pToActivate)
        ((CMDIChildWnd*)pToActivate)->MDIActivate(); //bring the window to the front
      if (pToActivate->IsIconic())
        //pToActivate->ShowWindow(SW_RESTORE);
        pToActivate->ShowWindow(SW_SHOWNOACTIVATE);
      else
        {
        pToActivate->SetFocus();
        }
      }
    else
      ASSERT(FALSE); //What window is this ???
    }
  }

//--------------------------------------------------------------------------

void CExploreScd::DeleteGraphicPage(CXTPage * pPage)
  {
  if (pPage->m_pGrfDoc->DeleteTags(true)>=0)
    pPage->m_pGrfDoc->CloseDocument();
  }

//--------------------------------------------------------------------------

void CExploreScd::RenameGraphicPage(CXTPage * pPage)
  {
  CRenamePageDlg Dlg(pPage->m_sPageId);
  if (Dlg.DoModal()==IDOK)
    {
    m_PageMap.RemoveKey(pPage->m_sPageId);
    pPage->m_sPageId=Dlg.m_NewName;
    Strng Ext;
    Ext.FnExt(pPage->m_sPageName.GetBuffer());

    Strng Path;
    Path.FnDrivePath((LPTSTR)(LPCTSTR)pPage->m_pGrfDoc->GetPathName());
    Path+=pPage->m_sPageId;
    Path+=Ext();
    pPage->m_pGrfDoc->SetPathName(Path());

    m_PageMap.SetAt(pPage->m_sPageId, pPage);
    RefreshIt(false);
    }
  }

//--------------------------------------------------------------------------

void CExploreScd::DeleteTrendPage(CXTPage * pPage)
  {
  //if (pPage->m_pGrfDoc->DeleteTags(true)>=0)
  //  pPage->m_pGrfDoc->CloseDocument();
  }

//--------------------------------------------------------------------------

void CExploreScd::RenameTrendPage(CXTPage * pPage)
  {
  //CRenamePageDlg Dlg(pPage->m_sPageId);
  //if (Dlg.DoModal()==IDOK)
  //  {
  //  m_PageMap.RemoveKey(pPage->m_sPageId);
  //  pPage->m_sPageId=Dlg.m_NewName;
  //  Strng Ext;
  //  Ext.FnExt(pPage->m_sPageName.GetBuffer());

  //  Strng Path;
  //  Path.FnDrivePath((LPTSTR)(LPCTSTR)pPage->m_pGrfDoc->GetPathName());
  //  Path+=pPage->m_sPageId;
  //  Path+=Ext();
  //  pPage->m_pGrfDoc->SetPathName(Path());

  //  m_PageMap.SetAt(pPage->m_sPageId, pPage);
  //  RefreshIt(false);
  //  }
  }

//--------------------------------------------------------------------------


void CExploreScd::CopyTagList2Clipboard(HTREEITEM hRootItem, int DoLevelCount, LPCTSTR Header, CString & LineHeader, CString &Buffer, bool TopLevel)
  {

  dbgpln("CCC %3i %s", DoLevelCount, Header);
  dbgpln("        %s", LineHeader);
  dbgpln("        %s", Buffer);

  if (TopLevel)
    Buffer=Header;

  HTREEITEM h=m_Tree.GetNextItem(hRootItem, TVGN_CHILD);
  while (h)
    {
    CString S=m_Tree.GetItemText(h);
    CString Line;
    if (LineHeader.GetLength()>0)
      Line.Format("%s\t%s", LineHeader, S);
    else
      Line=S;

    if (DoLevelCount==0)
      {
      if (Buffer.GetLength()>0)
        Buffer+="\n";
      Buffer+=Line;
      }

    CopyTagList2Clipboard(h, DoLevelCount-1, Header, Line, Buffer, false);

    h=m_Tree.GetNextItem(h, TVGN_NEXT);
    }

  if (TopLevel)
    CopyTextToClipboard(this, (LPTSTR)(LPCTSTR)Buffer);
  };

//--------------------------------------------------------------------------

void CExploreScd::OnDoClicks(NMHDR *pNMHDR, LRESULT *pResult, int Where)
  {
  LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

  //dbgpln("OnDoClicks  %3i %5i", m_ChangeBusy, pNMTreeView->action);

  HTREEITEM hSel = m_Tree.GetSelectedItem();
  if (m_ChangeBusy==0 && hSel && m_Tree.GetItemData(hSel)!=0 && 
    (pNMTreeView->action==TVC_BYMOUSE || 
     pNMTreeView->action==TVC_BYKEYBOARD ||
     hSel==m_hPrevSel))
    {
    int Id=reinterpret_cast<CXTTreeInfo*>((void*)m_Tree.GetItemData(hSel))->m_Id;
    switch (Id)
      {
      case TrID_Graphic:
        {
        CString Txt = m_Tree.GetItemText(hSel);
        Txt+=".scg";
        ActivateWnd(Txt);
        break;
        }
      case TrID_Trend:
        {
        CString Txt = m_Tree.GetItemText(hSel);
        Txt+=".trn";
        ActivateWnd(Txt);
        break;
        }
      case TrID_Other:
        {
        CString Txt = m_Tree.GetItemText(hSel);
        ActivateWnd(Txt);
        break;
        }
      case TrID_Node: // NOdes
        {
        CString Txt = m_Tree.GetItemText(hSel);
        CXTTag *pTag;
        if (m_TagMap.Lookup(Txt, pTag))
          {
          if (pTag->m_Pages.GetCount())
            {
            pTag->m_iAccPage++;
            if (pTag->m_iAccPage>=pTag->m_Pages.GetCount())
              pTag->m_iAccPage=0;

            //CString S;
            //S.Format("%s\t%s", Txt, pTag->m_Pages[pTag->m_iAccPage]->m_pPage->m_sPageName);
            bool GMC=gs_pPrj->m_bGrfMoveCursor!=0;
            gs_pPrj->m_bGrfMoveCursor=false;
            gs_pPrj->FindTag(Txt, pTag->m_Pages[pTag->m_iAccPage]->m_pPage->m_sPageName, NULL, -1, FTO_HighliteSlow);
            gs_pPrj->m_bGrfMoveCursor=GMC;
            }
          }
        //gs_AccessWnds.AccessNode(-1, Txt); 
        break;
        }
      }
    }
  m_hPrevSel=hSel;
  ShowWindow(SW_RESTORE);//SW_SHOWNORMAL);
  m_Tree.SetFocus();
  *pResult = 0;
  }

void CExploreScd::CollectsBulkTags(HTREEITEM hTagOwner, int Level, CStringList &MarkTags)
  {
  HTREEITEM h=m_Tree.GetNextItem(hTagOwner, TVGN_CHILD);
  while (h)
    {
    CString S=m_Tree.GetItemText(h);
    if (Level==0)
      MarkTags.AddTail(S);
    else
      CollectsBulkTags(h, Level-1, MarkTags);
    h=m_Tree.GetNextItem(h, TVGN_NEXT);
    }
  }

void CExploreScd::DoBulkTagChange(HTREEITEM hTagCheckTags, Strng_List * pAllTags, CXTTagArray *pAllXTags, int Level)
  {
  CStringList MarkTags;
  CStringList CheckTags;
  CollectsBulkTags(hTagCheckTags, Level, CheckTags);
  if (pAllXTags)
    {
    for (int i=0; i<pAllXTags->GetCount(); i++)
      MarkTags.AddTail((*pAllXTags)[i]->m_sTag);
    }
  else
    {
    for (Strng *p=pAllTags ? pAllTags->First():NULL; p; p=p->Next())
      MarkTags.AddTail(p->Str());
    }

  CBulkTagChange Dlg(&MarkTags, &CheckTags, true, this);
  if (Dlg.DoModal())
    {
    }
  }

void CExploreScd::OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  dbgpln("OnNMClickTree>>");
  OnDoClicks(pNMHDR, pResult, 0);
  dbgpln("<<OnNMClickTree");
  }

void CExploreScd::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  dbgpln("OnTvnSelchangedTree>>");
  OnDoClicks(pNMHDR, pResult, ODC_Select);
  dbgpln("<<OnTvnSelchangedTree");
  }

void CExploreScd::OnNMRclickTree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  TV_HITTESTINFO HTI;
  DWORD dw=::GetMessagePos();
  POINTS pts=MAKEPOINTS(dw);
  HTI.pt.x = pts.x;
  HTI.pt.y = pts.y;
  m_Tree.ScreenToClient(&HTI.pt);
  HTREEITEM hSel=NULL;

  bool OnLine = m_Tree.HitTest(&HTI) && (HTI.flags & TVHT_ONITEMRIGHT|TVHT_TOLEFT|TVHT_TORIGHT);
  bool OnItem = m_Tree.HitTest(&HTI) && (HTI.flags & TVHT_ONITEM/*|TVHT_ONITEMRIGHT|TVHT_ONITEMLABEL*/);

  hSel=HTI.hItem;

  if (hSel)
    {
    m_Tree.SelectItem(hSel);
    LRESULT LclResult;
    OnDoClicks(pNMHDR, &LclResult, 0);
    
    int Id=reinterpret_cast<CXTTreeInfo*>((void*)m_Tree.GetItemData(hSel))->m_Id;
    if (!OnItem)
      {
      switch (Id)
        {
        case TrID_Graphic:  Id = TrID_GraphicHdr; break;
        case TrID_Trend: Id = TrID_TrendHdr;      break;
        case TrID_Class: Id = TrID_ClassHdr;      break;
        case TrID_Node: Id = TrID_NodeHdr;        break;
        case TrID_Flow: Id = TrID_FlowHdr;        break;
        case TrID_MUFlow: Id = TrID_MUFlowHdr;    break;
        case TrID_SPFlow: Id = TrID_SPFlowHdr;    break;
        case TrID_Tear: Id = TrID_TearHdr;        break;
        }
      }
  //m_hGraphItem = InsertItem(LPSTR_TEXTCALLBACK, Img_GrfActive,  GetTreeInfo(m_GraphicTitle), TVI_ROOT);
  //m_hTrendItem = InsertItem(LPSTR_TEXTCALLBACK, Img_Trnd,       GetTreeInfo(m_TrendTitle), TVI_ROOT);
  //m_hOtherItem = InsertItem(LPSTR_TEXTCALLBACK, Img_Other,      GetTreeInfo(m_OtherTitle), TVI_ROOT);
  //m_hClassItem = InsertItem(LPSTR_TEXTCALLBACK, Img_Class,      GetTreeInfo(m_ClassTitle), TVI_ROOT);
  //m_hNodesItem = InsertItem(LPSTR_TEXTCALLBACK, Img_Node,       GetTreeInfo(m_NodeTitle), TVI_ROOT);

    switch (Id)
      {
      case TrID_GraphicHdr:
        {
        CMenu Menu;
        Menu.CreatePopupMenu();
        Menu.AppendMenu(MF_STRING|MF_GRAYED, 99, "Graphics");
        Menu.AppendMenu(MF_SEPARATOR, 99);
        Menu.AppendMenu(MF_STRING, 105, "Bulk Tag Change");
        Menu.AppendMenu(MF_SEPARATOR, 99);
        //#550 - Menu.AppendMenu(MF_STRING, 101, "New");
        //#550 - Menu.AppendMenu(MF_SEPARATOR, 99);
        //#550 - Menu.AppendMenu(MF_STRING | (gs_pPrj->m_GrfBehaviour == WB_Coincident ? MF_CHECKED:0), 108, "Treat as One");
        //#550 - Menu.AppendMenu(MF_SEPARATOR, 99);
        Menu.AppendMenu(MF_STRING, 130, "Copy Graphic List to Clipboard");
        Menu.AppendMenu(MF_STRING, 131, "Copy Graphic, Tag List to Clipboard");

        CPoint curPoint;
        GetCursorPos(&curPoint);

        int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
        Menu.DestroyMenu();                                           
        switch (RetCd)
          {
          case 101: 
            {
            CAutoIncDec AID(gs_FileNewFlag);
            ScdApp()->GraphTemplate().OpenDocumentFile(NULL);
            break;
            }
          case 105:
            DoBulkTagChange(m_hClassItem, NULL, &m_Tags, 1);
            break;
          case 108:
            if (gs_pPrj->m_GrfBehaviour == WB_Coincident)
              gs_pPrj->m_GrfBehaviour = WB_None;
            else if (gs_pPrj->m_GrfBehaviour == WB_None)
              gs_pPrj->m_GrfBehaviour = WB_Coincident;
            CGrfFrameWnd::DoBehaviourChange();
            break;
          case 130:
            CopyTagList2Clipboard(m_hGraphItem, 0, "Graphic", CString(""), CString(""));
            break;
          case 131:
            CopyTagList2Clipboard(m_hGraphItem, 1, "Graphic\tTag", CString(""), CString(""));
            break;
          }
        break;
        }
      case TrID_TrendHdr:
        {
        CMenu Menu;
        Menu.CreatePopupMenu();
        Menu.AppendMenu(MF_STRING|MF_GRAYED, 99, "Trends");
        Menu.AppendMenu(MF_SEPARATOR, 99);
        //#550 - Menu.AppendMenu(MF_STRING, 101, "New");
        //#550 - Menu.AppendMenu(MF_SEPARATOR, 99);
        //#550 - Menu.AppendMenu(MF_STRING | (gs_pPrj->m_TrndBehaviour == WB_Coincident ? MF_CHECKED:MF_UNCHECKED), 108, "Treat as One");
        //#550 - Menu.AppendMenu(MF_SEPARATOR, 99);
        Menu.AppendMenu(MF_STRING, 130, "Copy Trend List to Clipboard");

        CPoint curPoint;
        GetCursorPos(&curPoint);

        int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
        Menu.DestroyMenu();                                           
        switch (RetCd)
          {
          case 101: 
            {
            CAutoIncDec AID(gs_FileNewFlag);
            ScdApp()->TrendTemplate().OpenDocumentFile(NULL);
            break;
            }
          case 108:
            if (gs_pPrj->m_TrndBehaviour == WB_Coincident)
              gs_pPrj->m_TrndBehaviour = WB_None;
            else if (gs_pPrj->m_TrndBehaviour == WB_None)
              gs_pPrj->m_TrndBehaviour = WB_Coincident;
            CTagVwSplit::DoBehaviourChange();
            break;
          case 130:
            CopyTagList2Clipboard(m_hTrendItem, 0, "Trend", CString(""), CString(""));
            break;
          }
        break;
        }
      case TrID_ClassHdr:
        {
        CMenu Menu;
        Menu.CreatePopupMenu();
        Menu.AppendMenu(MF_STRING|MF_GRAYED, 99, "Classes");
        Menu.AppendMenu(MF_SEPARATOR, 99);
        Menu.AppendMenu(MF_STRING, 105, "Bulk Tag Change");
        Menu.AppendMenu(MF_SEPARATOR, 99);
        Menu.AppendMenu(MF_STRING, 130, "Copy Class List to Clipboard");
        Menu.AppendMenu(MF_STRING, 131, "Copy Class, Tag List to Clipboard");

        CPoint curPoint;
        GetCursorPos(&curPoint);

        int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
        Menu.DestroyMenu();                                           
        switch (RetCd)
          {
          case 105:
            DoBulkTagChange(m_hClassItem, NULL, &m_Tags, 1);
            break;
          case 130:
            CopyTagList2Clipboard(m_hClassItem, 0, "Class", CString(""), CString(""));
            break;
          case 131:
            CopyTagList2Clipboard(m_hClassItem, 1, "Class\tTag", CString(""), CString(""));
            break;
          }
        break;
        }
      case TrID_NodeHdr:
        {
        CMenu Menu;
        Menu.CreatePopupMenu();
        Menu.AppendMenu(MF_STRING|MF_GRAYED, 99, "Nodes");
        Menu.AppendMenu(MF_SEPARATOR, 99);
        Menu.AppendMenu(MF_STRING, 105, "Bulk Tag Change");
        Menu.AppendMenu(MF_SEPARATOR, 99);
        Menu.AppendMenu(MF_STRING, 130, "Copy Tag List to Clipboard");

        CPoint curPoint;
        GetCursorPos(&curPoint);

        int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
        Menu.DestroyMenu();                                           
        switch (RetCd)
          {
          case 105:
            DoBulkTagChange(m_hNodesItem, NULL, &m_Tags, 0);//m_hNodesItem);
            break;
          case 130:
            CopyTagList2Clipboard(m_hNodesItem, 0, "Tag", CString(""), CString(""));
            break;
          }
        break;
        }
      case TrID_FlowHdr:
        {
        CMenu Menu;
        Menu.CreatePopupMenu();
        Menu.AppendMenu(MF_STRING|MF_GRAYED, 99, "Flows");
        Menu.AppendMenu(MF_SEPARATOR, 99);
        Menu.AppendMenu(MF_STRING, 130, "Copy Tag List to Clipboard");
        Menu.AppendMenu(MF_SEPARATOR, 99);
        Menu.AppendMenu(MF_STRING|(m_iShowWhatFlows & CUseMsk_InUse   ? MF_CHECKED:0), 110,  "Show In Use");
        Menu.AppendMenu(MF_STRING|(m_iShowWhatFlows & CUseMsk_WasUsed ? MF_CHECKED:0), 111,  "Show Ever Used");
        Menu.AppendMenu(MF_STRING|(m_iShowWhatFlows & CUseMsk_Exists  ? MF_CHECKED:0), 112,  "Show All");
        Menu.AppendMenu(MF_SEPARATOR, 99);
        Menu.AppendMenu(MF_STRING|(m_bBranchMU ? MF_CHECKED:0),                        120,  "Branch Makeups...");
        Menu.AppendMenu(MF_STRING|(m_bBranchSP ? MF_CHECKED:0),                        121,  "Branch Spills...");

        CPoint curPoint;
        GetCursorPos(&curPoint);

        int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
        Menu.DestroyMenu();                                           
        switch (RetCd)
          {
          case 105:
            break;
          case 110:
            if (m_iShowWhatFlows & CUseMsk_InUse)
              m_iShowWhatFlows  &= ~CUseMsk_InUse;
            else
              m_iShowWhatFlows  |= CUseMsk_InUse;
            CExploreScd::RefreshIt(true); 
            CExploreScd::ChkRefreshIt(); 
            break;
          case 111:
            if (m_iShowWhatFlows & CUseMsk_WasUsed)
              m_iShowWhatFlows  &= ~CUseMsk_WasUsed;
            else
              m_iShowWhatFlows  |= CUseMsk_WasUsed;
            CExploreScd::RefreshIt(true); 
            CExploreScd::ChkRefreshIt(); 
            break;
          case 112:
            if (m_iShowWhatFlows & CUseMsk_Exists)
              m_iShowWhatFlows  &= ~CUseMsk_Exists;
            else
              m_iShowWhatFlows  |= CUseMsk_Exists;
            CExploreScd::RefreshIt(true); 
            CExploreScd::ChkRefreshIt(); 
            break;
          case 120:
            m_bBranchMU=!m_bBranchMU;
            CExploreScd::RefreshIt(true); 
            CExploreScd::ChkRefreshIt(); 
            break;
          case 121:
            m_bBranchSP=!m_bBranchSP;
            CExploreScd::RefreshIt(true); 
            CExploreScd::ChkRefreshIt(); 
            break;
          //case 130:
          //  CopyTagList2Clipboard(m_hNodesItem, 0, "Tag", CString(""), CString(""));
          //  break;
          }
        break;
        }
      case TrID_TearHdr:
        {
        CMenu Menu;
        Menu.CreatePopupMenu();
        Menu.AppendMenu(MF_STRING|MF_GRAYED, 99, "Tears");
        Menu.AppendMenu(MF_SEPARATOR, 99);
        //Menu.AppendMenu(MF_STRING, 105, "Bulk Tag Change");
        Menu.AppendMenu(MF_STRING, 131, "Summary");
        Menu.AppendMenu(MF_SEPARATOR, 99);
        Menu.AppendMenu(MF_STRING, 130, "Copy Tag List to Clipboard");

        CPoint curPoint;
        GetCursorPos(&curPoint);

        int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
        Menu.DestroyMenu();                                           
        switch (RetCd)
          {
          case 105:
            DoBulkTagChange(m_hTearsItem, NULL, &m_Tags, 0);//m_hNodesItem);
            break;
          case 130:
            CopyTagList2Clipboard(m_hTearsItem, 0, "Tag", CString(""), CString(""));
            break;
          case 131:
            gs_AccessWnds.AccessNode(-1, "$Tear"); 
            break;
          }
        break;
        }
      case TrID_Graphic:
        {
        CString Txt = m_Tree.GetItemText(hSel);
        CXTPage *pPage;
        if (m_PageMap.Lookup(Txt, pPage))
          {
          POSITION pos = pPage->m_pGrfDoc->GetFirstViewPosition();
          CView* pFirstView = pPage->m_pGrfDoc->GetNextView( pos );
          CGrfWnd* pGWnd = (CGrfWnd*)pFirstView;
          ActivateWnd(NULL, pGWnd->GetParent());

          CMenu Menu;
          Menu.CreatePopupMenu();
          CString S;
          S.Format("Page '%s'", Txt); 
          Menu.AppendMenu(MF_STRING|MF_GRAYED, 100, S);
          Menu.AppendMenu(MF_SEPARATOR, 99);
          //Menu.AppendMenu(MF_STRING, 101, "New");
          Menu.AppendMenu(MF_STRING|(pPage->m_pGrfDoc->bModelsActive ? MF_CHECKED:MF_UNCHECKED), 104, "Active");
          Menu.AppendMenu(MF_STRING, 102, "Rename");
          Menu.AppendMenu(MF_STRING, 105, "Bulk Tag Change");

          //Menu.AppendMenu(MF_STRING, 103, "Delete");
          //Menu.AppendMenu(MF_SEPARATOR, 99);
          //Menu.AppendMenu(MF_STRING | (gs_pPrj->m_TrndBehaviour == WB_Coincident ? MF_CHECKED:MF_UNCHECKED), 108, "Treat as One");
          //Menu.AppendMenu(MF_SEPARATOR, 99);

          Menu.AppendMenu(MF_SEPARATOR, 99);
          //Menu.AppendMenu(MF_STRING, 130, "Copy Graphics List to Clipboard");
          Menu.AppendMenu(MF_STRING, 122, "Copy Bitmap to Clipboard");
          Menu.AppendMenu(MF_STRING, 131, "Copy Tag List to Clipboard");

          CPoint curPoint;
          GetCursorPos(&curPoint);

          int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
          Menu.DestroyMenu();                                           
          switch (RetCd)
            {
            case 101: 
              {
              CAutoIncDec AID(gs_FileNewFlag);
              ScdApp()->GraphTemplate().OpenDocumentFile(NULL);
              break;
              }
            case 102: 
              RenameGraphicPage(pPage);
              break;
            case 103:
              DeleteGraphicPage(pPage);
              break;
            case 104:
              {
              SetGrfPageActive(pPage->m_pGrfDoc, !pPage->m_pGrfDoc->bModelsActive);
              int iImg=pPage->m_pGrfDoc->bModelsActive ? Img_GrfActive:Img_GrfInActive;
              m_Tree.SetItemImage(pPage->m_hPage, iImg, iImg);
              break;
              }
            case 105:
              DoBulkTagChange(pPage->m_hPage, &pPage->m_GrfTagList, NULL, 0);
              break;
            case 108:
              if (gs_pPrj->m_GrfBehaviour == WB_Coincident)
                gs_pPrj->m_GrfBehaviour = WB_None;
              else if (gs_pPrj->m_GrfBehaviour == WB_None)
                gs_pPrj->m_GrfBehaviour = WB_Coincident;
              CGrfFrameWnd::DoBehaviourChange();
              break;
            case 121:
            case 122:
              {

              //POSITION pos = pPage->m_pGrfDoc->GetFirstViewPosition();
              //CView* pFirstView = pPage->m_pGrfDoc->GetNextView( pos );
              //CGrfWnd* pGWnd = (CGrfWnd*)pFirstView;
              //ActivateWnd(NULL, pGWnd->GetParent());
              pGWnd->CopyBMPtoClipBoard(RetCd-121);
              break;
              }
            case 131:
              CopyTagList2Clipboard(hSel, 0, "Tag", CString(""), CString(""));
              break;
            }
          }
        else
          ASSERT_ALWAYS(FALSE, "Bad Graphic Page Lookup", __FILE__, __LINE__);
        break;
        }
      case TrID_Trend:
        {
        CString Txt = m_Tree.GetItemText(hSel);
        CXTPage *pPage;
        if (m_PageMap.Lookup(Txt, pPage))
          {
          //ActivateWnd(NULL, pGWnd->GetParent());


          CMenu Menu;
          Menu.CreatePopupMenu();

          CString S;
          S.Format("Trend '%s'", Txt); 
          Menu.AppendMenu(MF_STRING|MF_GRAYED, 100, S);
          Menu.AppendMenu(MF_SEPARATOR, 99);
          //Menu.AppendMenu(MF_STRING, 101, "New");
          //Menu.AppendMenu(MF_STRING, 102, "Rename");
          //Menu.AppendMenu(MF_STRING, 103, "Delete");
          //Menu.AppendMenu(MF_SEPARATOR, 99);
          //Menu.AppendMenu(MF_STRING | (gs_pPrj->m_TrndBehaviour == WB_Coincident ? MF_CHECKED:MF_UNCHECKED), 108, "Treat as One");

          CPoint curPoint;
          GetCursorPos(&curPoint);

          int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
          Menu.DestroyMenu();                                           
          switch (RetCd)
            {
            case 101: 
              {
              CAutoIncDec AID(gs_FileNewFlag);
              ScdApp()->TrendTemplate().OpenDocumentFile(NULL);
              break;
              }
            case 102: 
              RenameTrendPage(pPage);
              break;
            case 103:
              DeleteTrendPage(pPage);
              break;
            case 108:
              if (gs_pPrj->m_TrndBehaviour == WB_Coincident)
                gs_pPrj->m_TrndBehaviour = WB_None;
              else if (gs_pPrj->m_TrndBehaviour == WB_None)
                gs_pPrj->m_TrndBehaviour = WB_Coincident;
              CTagVwSplit::DoBehaviourChange();
              break;
            }
          }
        else
          ASSERT_ALWAYS(FALSE, "Bad Trend Page Lookup", __FILE__, __LINE__);
        break;
        }
      case TrID_Other:
        {
        break;
        }
      case TrID_Class: // NOdes
        {
        CString Txt = m_Tree.GetItemText(hSel);
        CXTClass *pClass;
        if (m_ClassMap.Lookup(Txt, pClass))
          {
          CString S;
          CMenu Menu;
          Menu.CreatePopupMenu();
          S.Format("Class %s", Txt); 
          Menu.AppendMenu(MF_STRING|MF_GRAYED, 100, S);
          Menu.AppendMenu(MF_SEPARATOR, 99);
          Menu.AppendMenu(MF_STRING, 105, "Bulk Tag Change");
          Menu.AppendMenu(MF_SEPARATOR, 99);
          Menu.AppendMenu(MF_STRING, 130, "Copy Tag List to Clipboard");

          CPoint curPoint;
          GetCursorPos(&curPoint);

          int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
          Menu.DestroyMenu();                                           
          switch (RetCd)
            {
            case 105:                
              DoBulkTagChange(hSel, &pClass->xm_TagList, NULL, 0);
              break;
            case 130:
              CopyTagList2Clipboard(hSel, 0, "Tag", CString(""), CString(""));
              break;
            }
          }
        else
          ASSERT_ALWAYS(FALSE, "Bad Class Lookup", __FILE__, __LINE__);
        break;
        }
      case TrID_Node:
        {
        CString Txt = m_Tree.GetItemText(hSel);
        CXTTag *pTag;
        if (m_TagMap.Lookup(Txt, pTag))
          {
          CString S;
          CMenu Menu;
          Menu.CreatePopupMenu();
          S.Format("Node %s", Txt); 
          Menu.AppendMenu(MF_STRING|MF_GRAYED, 100, S);
          Menu.AppendMenu(MF_SEPARATOR, 99);
          Menu.AppendMenu(MF_STRING, 102, "Access");
          Menu.AppendMenu(MF_STRING, 104, "Change Tag ...");
          Menu.AppendMenu(MF_SEPARATOR, 99);
  
          for (int i=0; i<pTag->m_Pages.GetCount(); i++)
            {
            S.Format("Goto on %s", pTag->m_Pages[i]->m_pPage->m_sPageId);
            Menu.AppendMenu(MF_STRING, 110+i, S);
            }
          Menu.AppendMenu(MF_STRING|MF_GRAYED, 103, "XRefs");

          CPoint curPoint;
          GetCursorPos(&curPoint);

          int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
          Menu.DestroyMenu();                                           
          switch (RetCd)
            {
            case 102: 
              gs_AccessWnds.AccessNode(-1, Txt); 
              break;
            case 103: 
              break;
            case 104:
              {
              CChangeTag ChgTag((LPSTR)(LPCSTR)Txt, true);
              ChgTag.DoModal();
              break;
              }
            default:
              {
              if (RetCd>=110 && RetCd<110+pTag->m_Pages.GetCount())
                {
                //S.Format("%s\t%s", Txt, pTag->m_Pages[RetCd-110]->m_pPage->m_sPageName);
                S=Txt;
                gs_pPrj->FindTag(S, pTag->m_Pages[RetCd-110]->m_pPage->m_sPageName, NULL, -1, FTO_MoveCursor|FTO_HighliteSlow);
                }
              break;
              }
            //case 130:
            //  CopyTagList2Clipboard(pTag->m_hNodeItem, 0, "Tag", CString(""), CString(""));
            //  break;
            }
          }
        else
          ASSERT_ALWAYS(FALSE, "Bad Tag Lookup", __FILE__, __LINE__);
        break;
        }
      case TrID_Flow:
        {
        CString Txt = m_Tree.GetItemText(hSel);
        CXTFlow *pFlow;
        if (m_FlowMap.Lookup(Txt, pFlow))
          {
          CString S;
          CMenu Menu;
          Menu.CreatePopupMenu();
          S.Format("Flow %s", Txt); 
          Menu.AppendMenu(MF_STRING|MF_GRAYED, 100, S);
          Menu.AppendMenu(MF_SEPARATOR, 99);
          Menu.AppendMenu(MF_STRING, 102, "Access");
          Menu.AppendMenu(MF_STRING, 104, "Change Tag ...");
          //Menu.AppendMenu(MF_SEPARATOR, 99);
  
  
          //for (int i=0; i<pTag->m_Pages.GetCount(); i++)
          //  {
          //  S.Format("Goto on %s", pTag->m_Pages[i]->m_pPage->m_sPageId);
          //  Menu.AppendMenu(MF_STRING, 110+i, S);
          //  }
          //Menu.AppendMenu(MF_STRING|MF_GRAYED, 103, "XRefs");

          CPoint curPoint;
          GetCursorPos(&curPoint);

          int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
          Menu.DestroyMenu();                                           
          switch (RetCd)
            {
            case 102: gs_AccessWnds.AccessNode(-1, Txt); break;
            case 104:
              {
              CChangeTag ChgTag((LPSTR)(LPCSTR)Txt, true);
              ChgTag.DoModal();
              break;
              }
            //case 103: ; break;
            //default:
            //  {
            //  if (RetCd>=110 && RetCd<110+pTag->m_Pages.GetCount())
            //    {
            //    //S.Format("%s\t%s", Txt, pTag->m_Pages[RetCd-110]->m_pPage->m_sPageName);
            //    S=Txt;
            //    gs_pPrj->FindTag(S, pTag->m_Pages[RetCd-110]->m_pPage->m_sPageName, NULL, -1, FTO_MoveCursor|FTO_HighliteSlow);
            //    }
            //  break;
            //  }
            //case 130:
            //  CopyTagList2Clipboard(pTag->m_hNodeItem, 0, "Tag", CString(""), CString(""));
            //  break;
            }
          }
        else
          ASSERT_ALWAYS(FALSE, "Bad Flow Lookup", __FILE__, __LINE__);
        break;
        }
      case TrID_Tear:
        {
        CString Txt = m_Tree.GetItemText(hSel);
        CXTTear *pTear;
        if (m_TearMap.Lookup(Txt, pTear))
          {
          CString S;
          CMenu Menu;
          Menu.CreatePopupMenu();
          S.Format("Tear %s", Txt); 
          Menu.AppendMenu(MF_STRING|MF_GRAYED, 100, S);
          Menu.AppendMenu(MF_SEPARATOR, 99);
          Menu.AppendMenu(MF_STRING, 131, "Summary");
          Menu.AppendMenu(MF_SEPARATOR, 99);
          Menu.AppendMenu(MF_STRING, 102, "Access");
          //Menu.AppendMenu(MF_STRING, 104, "Change Tag ...");

          //for (int i=0; i<pTear->m_Pages.GetCount(); i++)
          //  {
          //  S.Format("Goto on %s", pTag->m_Pages[i]->m_pPage->m_sPageId);
          //  Menu.AppendMenu(MF_STRING, 110+i, S);
          //  }

          CPoint curPoint;
          GetCursorPos(&curPoint);

          int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
          Menu.DestroyMenu();                                           
          switch (RetCd)
            {
            case 102: gs_AccessWnds.AccessNode(-1, Txt); break;
            case 104:
              {
              //CChangeTag ChgTag((LPSTR)(LPCSTR)Txt, true);
              //ChgTag.DoModal();
              break;
              }
            case 131:
              gs_AccessWnds.AccessNode(-1, "$Tear"); 
              break;
            }
          }
        else
          ASSERT_ALWAYS(FALSE, "Bad Tear Lookup", __FILE__, __LINE__);
        break;
        }
      }
    }
  ShowWindow(SW_RESTORE);//SW_SHOWNORMAL);
  *pResult = 0;
  }

void CExploreScd::OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  HTREEITEM hSel = m_Tree.GetSelectedItem();
  if (hSel)
    {
    int Id=reinterpret_cast<CXTTreeInfo*>((void*)m_Tree.GetItemData(hSel))->m_Id;
    switch (Id)
      {
      case TrID_Graphic:
      case TrID_Trend:
      case TrID_Other:
        {
        CString Txt = m_Tree.GetItemText(hSel);
        ActivateWnd(Txt);
        break;
        }
      case TrID_Node:
        //{
        //CString Txt = m_Tree.GetItemText(hSel);
        //gs_AccessWnds.AccessNode(-1, Txt); 
        //break;
        //}
      case TrID_Flow: 
        //{
        //CString Txt = m_Tree.GetItemText(hSel);
        //gs_AccessWnds.AccessNode(-1, Txt); 
        //break;
        //}
      case TrID_Tear: 
        {
        CString Txt = m_Tree.GetItemText(hSel);
        gs_AccessWnds.AccessNode(-1, Txt); 
        break;
        }
      }
    }
  ShowWindow(SW_RESTORE);//SW_SHOWNORMAL);
  *pResult = 0;
  }

void CExploreScd::FixFilterStuff()
  {
  m_TagFilter.GetWindowText(m_FilterString);
  m_FilterString = m_FilterString.Trim();
  if (!m_bCaseSens)
    m_FilterString.MakeLower();

  if (m_FilterString.GetLength()>0)
    m_FilterRule=(eFilterRule)m_TagFilterRule.GetCurSel();
  else
    m_FilterRule=eFROff;
  }

void CExploreScd::OnEnChangeTagfilter()
  {
  m_ChangeBusy++;
  CWaitMsgCursor WaitMsg("Changing Selection");
  m_Tree.LockWindowUpdate();
  for (;;)
    {
    FixFilterStuff();
    SetFilter();
    if (LoadTagTree(true))
      break;
    }
#if dbgDumpAll
  DumpAll("Filter");
#endif
  m_Tree.UnlockWindowUpdate();
  m_Tree.RedrawWindow();
  m_ChangeBusy--;
  }

void CExploreScd::OnCbnSelchangeTagfilterrule()
  {
  m_ChangeBusy++;
  CWaitMsgCursor WaitMsg("Changing Selection");
  m_Tree.LockWindowUpdate();
  for (;;)
    {
    FixFilterStuff();
    SetFilter();
    if (LoadTagTree(true))
      break;
    }
#if dbgDumpAll
  DumpAll("Rule");
#endif
  m_Tree.UnlockWindowUpdate();
  m_Tree.RedrawWindow();
  m_ChangeBusy--;
  }


void CExploreScd::OnTvnItemexpandedTree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
  HTREEITEM Item=pNMTreeView->itemNew.hItem;
  BOOL Exp=(m_Tree.GetItemState(pNMTreeView->itemNew.hItem, TVIS_EXPANDED )&TVIS_EXPANDED)!=0;

  if (Item==m_hGraphItem)
    m_bGraphExpanded=Exp;
  if (Item==m_hTrendItem) 
    m_bTrendExpanded=Exp;
  if (Item==m_hOtherItem) 
    m_bOtherExpanded=Exp;
  if (Item==m_hClassItem) 
    m_bClassExpanded=Exp;
  if (Item==m_hNodesItem) 
    m_bNodesExpanded=Exp;
  if (Item==m_hFlowsItem) 
    m_bFlowsExpanded=Exp;
  if (Item==m_hTearsItem) 
    m_bTearsExpanded=Exp;
  if (Item==m_hMUFlowsItem) 
    m_bMUFlowsExpanded=Exp;
  if (Item==m_hSPFlowsItem) 
    m_bSPFlowsExpanded=Exp;

  *pResult = 0;
  }
