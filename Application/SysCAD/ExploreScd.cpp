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

extern "C" 
  {
#include "grldefs.h"
  }

#include "afxwin.h"
#include "ExploreScd.h"
#include "wndslct.h"
//#include "optoff.h"

#define dbgAdd            0
#define dbgTime           0
#define dbgDumpAll        0
#define dbgHoldLockUpdate 0

// CExploreScd dialog

const int Img_GrfActive   = 0;
const int Img_Trnd        = 1;
const int Img_Other       = 2;
const int Img_Node        = 3;
const int Img_Class       = 4;
const int Img_LostGrf     = 5;
const int Img_LostMdl     = 6;
const int Img_GrfInActive = 7;

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

CXTTag::CXTTag(CExploreScd * Dlg, LPCTSTR Tag, CXTClass * Class) : m_Dlg(*Dlg) , CXTTreeInfo(TrID_Node, &m_sTag)
  {
  m_sTag=Tag;
  m_sTagLwr=Tag;
  m_sTagLwr.MakeLower();
  m_pClass=Class;
  m_Selected=false;
  m_Marked=false;
  m_InUse=true;
  m_ModelOK=true;
  m_hTreeItem=NULL;
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

CXTClass::CXTClass(CExploreScd * Dlg, LPCTSTR ClassId) : m_Dlg(*Dlg),
CXTTreeInfo(TrID_Class, &m_sClassId)
  {
  m_sClassId=ClassId;
  m_Selected=true;
  m_hClassItem=NULL;
  m_nCount=0;
  m_nSelected=0;
  m_Tags.SetSize(0, 128);
  };

CXTClass::~CXTClass() 
  {
  for (int i=0; i<m_Tags.GetCount(); i++)
    delete m_Tags[i];
  m_Tags.SetSize(0, 128);
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

//---------------------------------------------------------------------------

CExploreScd * CExploreScd::sm_pTheWnd=NULL;
BOOL CExploreScd::sm_bDoRefresh=false;
BOOL CExploreScd::sm_bDoRefreshAll=false;
BOOL CExploreScd::sm_bUseScdExplorer=true;
BOOL CExploreScd::sm_bInited=false;

IMPLEMENT_DYNAMIC(CExploreScd, CDialog)
CExploreScd::CExploreScd(CWnd* pParent /*=NULL*/)
: CDialog(CExploreScd::IDD, pParent),
m_GraphicTitle(TrID_GraphicHdr, "Graphic"),
m_ClassTitle(TrID_ClassHdr, "Class"),
m_TrendTitle(TrID_TrendHdr, "Trend"),
m_OtherTitle(TrID_OtherHdr, "Other"),
m_NodeTitle(TrID_NodeHdr,    "Node")
  {
  m_FilterString="Boil";
  m_FilterRule=eFRContains;

  m_hGraphItem=NULL;
  m_hTrendItem=NULL;
  m_hOtherItem=NULL;
  m_hClassItem=NULL;
  m_hNodesItem=NULL;

  m_nSelected=0;
  m_WindowCount=0;
  m_GraphicCount=0;
  m_TrendCount=0;

  m_ChangeBusy=0;

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
  m_hOtherItem = InsertItem(LPSTR_TEXTCALLBACK, Img_Other,      GetTreeInfo(m_OtherTitle), TVI_ROOT);
  m_hClassItem = InsertItem(LPSTR_TEXTCALLBACK, Img_Class,      GetTreeInfo(m_ClassTitle), TVI_ROOT);
  m_hNodesItem = InsertItem(LPSTR_TEXTCALLBACK, Img_Node,       GetTreeInfo(m_NodeTitle), TVI_ROOT);

  m_bGraphExpanded=PF.RdInt(Section, "Graphics.Open", 1);
  m_bTrendExpanded=PF.RdInt(Section, "Trends.Open", 0);
  m_bOtherExpanded=PF.RdInt(Section, "Other.Open", 0);
  m_bClassExpanded=PF.RdInt(Section, "Classes.Open", 0);
  m_bNodesExpanded=PF.RdInt(Section, "Nodes.Open", 0);

  m_hWndItems[0]=m_hOtherItem;
  m_hWndItems[1]=m_hGraphItem;
  m_hWndItems[2]=m_hTrendItem;

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

    NTags = gs_pPrj->FlwLib()->FE_TagOperation(FETOp_GetActive, RawTags);
    NTags=Max(1000, NTags);

    m_TagMap.InitHashTable(FindNextPrimeNumber((UINT)(NTags*1.18)));
    m_HTagMap.InitHashTable(FindNextPrimeNumber((UINT)(NTags*1.18)));
    m_ClassMap.InitHashTable(FindNextPrimeNumber((UINT)(100)));
    m_PageMap.InitHashTable(FindNextPrimeNumber((UINT)(200)));
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
    //sm_pTheWnd->DestroyWindow();
    }
  //delete sm_pTheWnd;
  //sm_pTheWnd=NULL;
  //sm_bInited=false;
  }

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
  switch (pNM->hdr.code)
    {
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

      // ASSERT_ALWAYS(ChangesOK, "Unexpected New Page");

      CXTPage *pPage=new CXTPage(this, PgType2TrIDs[WL.Wnds[i].iType], PgId(), PgName());
      pPage->m_iType=WL.Wnds[i].iType;
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
              CXTTag *pTag;
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
                  dbgpln("Add Tag2Page %-25s << %s", pPage->m_sPageId, pTag->m_sTag);
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
                  dbgpln("Add Page2Tag %-25s << %s", pTag->m_sTag, pPage->m_sPageId);
#endif
                  }
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
                dbgpln("Tag  NoMdl%-25s %s", m_Classes[iClass]->m_sClassId, pTag->m_sTag);
#endif
                m_Tags.Add(pTag);
                m_TagMap.SetAt(pTag->m_sTag, pTag);

                }
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
  if (m_Classes.GetCount())
    {
    HpSort(m_Classes.GetCount(), (void**)&m_Classes[0], HpClassTest);
    for (int i=0; i<m_Classes.GetCount(); i++)
      {
      CXTClass &Cl=*m_Classes[i];
      if (Cl.m_Tags.GetCount())
        HpSort(Cl.m_Tags.GetCount(), (void**)&Cl.m_Tags[0], HpTagTest);
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

  ClearTags();

#if dbgTime
  dbgpln("A:%10.3f", SW.Lap()*1e3);
#endif

  GetRawTags();
#if dbgTime
  dbgpln("B:%10.3f", SW.Lap()*1e3);
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

  CXTTag *pPrev=NULL;
  for (int t=0; t<m_Tags.GetCount(); t++)
    {
    CXTTag *pTag=m_Tags[t];
    if (pTag->m_Selected)
      {
      AddTagToTree(pTag, pPrev);
      pPrev=pTag;
      }
    }

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

  for (int i=0; i<m_Pages.GetCount(); i++)
    m_Pages[i]->m_InUse=false;

#if dbgTime
  dbgpln("A:%10.3f", SW.Lap()*1e3);
#endif

  GetRawTags();
#if dbgTime
  dbgpln("B:%10.3f", SW.Lap()*1e3);
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

  CXTTag *pPrev=NULL;
  for (int t=0; t<m_Tags.GetCount(); t++)
    {
    CXTTag *pTag=m_Tags[t];
    if (pTag->m_InUse && pTag->m_Selected)
      {
      AddTagToTree(pTag, pPrev);
      pPrev=pTag;
      }
    else
      {
      RemoveTagFromTree(pTag);
      }
    }

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

void CExploreScd::LoadTagTree()
  {
  if (!sm_bInited)
    return;
#if dbgAdd
  dbgpln("LoadTagTree====================================");
#endif

  CXTTag *pPrev=NULL;
  for (int i=0; i<m_Tags.GetCount(); i++)
    {
    CXTTag * pTag=m_Tags[i];
    if (pTag->m_Selected)
      {
      AddTagToTree(pTag, pPrev);
      pPrev=pTag;
      }
    else
      RemoveTagFromTree(pTag);
    }

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
      }
    }
#if dbgAdd
  dbgpln("Done       ====================================");
#endif
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

    ASSERT_ALWAYS(!pTag->m_hClassItem, "Tag Class ??")

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
    SetTagFilter(*m_Tags[i], i==0);

  UpdateSelectDisplay();
  }

//--------------------------------------------------------------------------

void CExploreScd::SetTagFilter(CXTTag & Tg, BOOL DoSetup)
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

void CExploreScd::UpdateSelectDisplay()
  {
  m_nSelected=0;
  for (int i=0; i<m_Classes.GetCount(); i++)
    {
    CXTClass & Cl=*m_Classes[i];
    Cl.m_nCount=0;
    Cl.m_nSelected=0;
    }
  for (int i=0; i<m_Tags.GetCount(); i++)
    {
    CXTTag &Tg=*m_Tags[i];
    if (Tg.m_Selected)
      {
      m_nSelected++;
      Tg.m_pClass->m_nSelected++;
      }
    Tg.m_pClass->m_nCount++;
    }

  m_GraphicTitle.m_pStr->Format("Graphics (%i)", m_GraphicCount);
  m_TrendTitle.m_pStr->Format("Trends (%i)", m_TrendCount);
  m_ClassTitle.m_pStr->Format("Classes (%i)", m_Classes.GetCount());
  if (m_nSelected==m_Tags.GetCount())
    m_NodeTitle.m_pStr->Format("Nodes (%i)", m_Tags.GetCount());
  else
    m_NodeTitle.m_pStr->Format("Nodes (%i of %i)", m_nSelected, m_Tags.GetCount());

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

void CExploreScd::ActivateWndByName(LPCTSTR Txt)
  {
  CWindowLists WL;
  if (WL.BuildSingleList()<0)
    return;

  const int Index = WL.Find(255, (char*)(const char*)Txt);
  if (Index<0) 
    return;

  ASSERT(Index>=0);
  CWnd *pToActivate=WL.Wnds[Index].pWnd;
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

void CExploreScd::DeletePage(CXTPage * pPage)
  {
  if (pPage->m_pGrfDoc->DeleteTags(true)>=0)
    pPage->m_pGrfDoc->CloseDocument();
  }

//--------------------------------------------------------------------------

void CExploreScd::RenamePage(CXTPage * pPage)
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


void CExploreScd::CopyTagList2Clipboard(HTREEITEM hRootItem, int DoLevelCount, LPCTSTR Header, CString & LineHeader, CString &Buffer, bool TopLevel)
  {
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

  HTREEITEM hSel = m_Tree.GetSelectedItem();
  if (m_ChangeBusy==0 &&  hSel && m_Tree.GetItemData(hSel)!=0 /* && pNMTreeView->action==TVC_BYMOUSE*/ )
    {
    int Id=reinterpret_cast<CXTTreeInfo*>((void*)m_Tree.GetItemData(hSel))->m_Id;
    switch (Id)
      {
      case TrID_Graphic:
        {
        CString Txt = m_Tree.GetItemText(hSel);
        Txt+=".scg";
        ActivateWndByName(Txt);
        break;
        }
      case TrID_Trend:
        {
        CString Txt = m_Tree.GetItemText(hSel);
        Txt+=".trn";
        ActivateWndByName(Txt);
        break;
        }
      case TrID_Other:
        {
        CString Txt = m_Tree.GetItemText(hSel);
        ActivateWndByName(Txt);
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

            CString S;
            S.Format("%s\t%s", Txt, pTag->m_Pages[pTag->m_iAccPage]->m_pPage->m_sPageName);
            bool GMC=gs_pPrj->m_bGrfMoveCursor!=0;
            gs_pPrj->m_bGrfMoveCursor=false;
            gs_pPrj->FindTag(S);
            gs_pPrj->m_bGrfMoveCursor=GMC;
            }
          }
        //gs_AccessWnds.AccessNode(-1, Txt); 
        break;
        }
      }
    }
  ShowWindow(SW_RESTORE);//SW_SHOWNORMAL);
  m_Tree.SetFocus();
  *pResult = 0;
  }


void CExploreScd::OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  OnDoClicks(pNMHDR, pResult, 0);
  }

void CExploreScd::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  OnDoClicks(pNMHDR, pResult, ODC_Select);
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
        case TrID_Trend: Id = TrID_TrendHdr; break;
        }
      }

    switch (Id)
      {
      case TrID_GraphicHdr:
        {
        CMenu Menu;
        Menu.CreatePopupMenu();

        Menu.AppendMenu(MF_STRING, 101, "New");
        Menu.AppendMenu(MF_SEPARATOR, 100);
        Menu.AppendMenu(MF_STRING | (gs_pPrj->m_GrfBehaviour == WB_Coincident ? MF_CHECKED:0), 108, "Treat as One");
        Menu.AppendMenu(MF_SEPARATOR, 100);
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

        Menu.AppendMenu(MF_STRING, 101, "New");
        Menu.AppendMenu(MF_SEPARATOR, 100);
        Menu.AppendMenu(MF_STRING | (gs_pPrj->m_TrndBehaviour == WB_Coincident ? MF_CHECKED:MF_UNCHECKED), 108, "Treat as One");
        Menu.AppendMenu(MF_SEPARATOR, 100);
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

        Menu.AppendMenu(MF_STRING, 130, "Copy Class List to Clipboard");
        Menu.AppendMenu(MF_STRING, 131, "Copy Class, Tag List to Clipboard");

        CPoint curPoint;
        GetCursorPos(&curPoint);

        int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
        Menu.DestroyMenu();                                           
        switch (RetCd)
          {
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

        Menu.AppendMenu(MF_STRING, 130, "Copy Tag List to Clipboard");

        CPoint curPoint;
        GetCursorPos(&curPoint);

        int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
        Menu.DestroyMenu();                                           
        switch (RetCd)
          {
          case 130:
            CopyTagList2Clipboard(m_hNodesItem, 0, "Tag", CString(""), CString(""));
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
          CMenu Menu;
          Menu.CreatePopupMenu();
          CString S;
          S.Format("Page '%s'", Txt); 
          Menu.AppendMenu(MF_STRING|MF_GRAYED, 100, S);
          Menu.AppendMenu(MF_SEPARATOR, 100);
          //Menu.AppendMenu(MF_STRING, 101, "New");
          Menu.AppendMenu(MF_STRING, 102, "Rename");
          Menu.AppendMenu(MF_STRING, 122, "Copy Bitmap to Clipboard");

          //Menu.AppendMenu(MF_STRING, 103, "Delete");
          //Menu.AppendMenu(MF_SEPARATOR, 100);
          //Menu.AppendMenu(MF_STRING | (gs_pPrj->m_TrndBehaviour == WB_Coincident ? MF_CHECKED:MF_UNCHECKED), 108, "Treat as One");
          Menu.AppendMenu(MF_SEPARATOR, 100);

          Menu.AppendMenu(MF_STRING|(pPage->m_pGrfDoc->bModelsActive ? MF_CHECKED:MF_UNCHECKED), 104, "Active");
          Menu.AppendMenu(MF_SEPARATOR, 100);
          //Menu.AppendMenu(MF_STRING, 130, "Copy Graphics List to Clipboard");
          Menu.AppendMenu(MF_STRING, 131, "Copy Graphic, Tags List to Clipboard");

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
              RenamePage(pPage);
              break;
            case 103:
              DeletePage(pPage);
              break;
            case 104:
              {
              SetGrfPageActive(pPage->m_pGrfDoc, !pPage->m_pGrfDoc->bModelsActive);
              int iImg=pPage->m_pGrfDoc->bModelsActive ? Img_GrfActive:Img_GrfInActive;
              m_Tree.SetItemImage(pPage->m_hPage, iImg, iImg);
              break;
              }
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
              POSITION pos = pPage->m_pGrfDoc->GetFirstViewPosition();
              CView* pFirstView = pPage->m_pGrfDoc->GetNextView( pos );
              CGrfWnd* pGWnd = (CGrfWnd*)pFirstView;
              pGWnd->CopyBMPtoClipBoard(RetCd-121);
              }
            case 131:
              CopyTagList2Clipboard(hSel, 1, "Graphic\tTag", CString(""), CString(""));
              break;
            }
          }
        else
          ASSERT_ALWAYS(FALSE, "Bad Page Lookup");
        break;
        }
      case TrID_Trend:
        {
        CString Txt = m_Tree.GetItemText(hSel);
        CMenu Menu;
        Menu.CreatePopupMenu();

        CString S;
        S.Format("Trend '%s'", Txt); 
        Menu.AppendMenu(MF_STRING|MF_GRAYED, 100, S);
        Menu.AppendMenu(MF_SEPARATOR, 100);
        //Menu.AppendMenu(MF_STRING, 101, "New");
        Menu.AppendMenu(MF_STRING|MF_GRAYED, 102, "Rename");
        //Menu.AppendMenu(MF_STRING|MF_GRAYED, 103, "Delete");
        //Menu.AppendMenu(MF_SEPARATOR, 100);
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
            break;
          case 103:
            break;
          case 108:
            if (gs_pPrj->m_TrndBehaviour == WB_Coincident)
              gs_pPrj->m_TrndBehaviour = WB_None;
            else if (gs_pPrj->m_TrndBehaviour == WB_None)
              gs_pPrj->m_TrndBehaviour = WB_Coincident;
            CTagVwSplit::DoBehaviourChange();
            break;
          }
        break;
        }
      case TrID_Other:
        {
        break;
        }
      case TrID_Class: // NOdes
        {
        CString Txt = m_Tree.GetItemText(hSel);
        CString S;
        CMenu Menu;
        Menu.CreatePopupMenu();
        S.Format("Class %s", Txt); 
        Menu.AppendMenu(MF_SEPARATOR, 100);
        Menu.AppendMenu(MF_STRING, 130, "Copy Tag List to Clipboard");

        CPoint curPoint;
        GetCursorPos(&curPoint);

        int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
        Menu.DestroyMenu();                                           
        switch (RetCd)
          {
          case 130:
            CopyTagList2Clipboard(hSel, 0, "Tag", CString(""), CString(""));
            break;
          }
        break;
        }
      case TrID_Node: // NOdes
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
          Menu.AppendMenu(MF_SEPARATOR, 100);
          Menu.AppendMenu(MF_STRING, 102, "Access");

          for (int i=0; i<pTag->m_Pages.GetCount(); i++)
            {
            S.Format("Goto on %s", pTag->m_Pages[i]->m_pPage->m_sPageId);
            Menu.AppendMenu(MF_STRING, 110+i, S);
            }
          Menu.AppendMenu(MF_STRING|MF_GRAYED, 103, "XRefs");
          Menu.AppendMenu(MF_SEPARATOR, 100);
          Menu.AppendMenu(MF_STRING, 130, "Copy Graphic List to Clipboard");

          CPoint curPoint;
          GetCursorPos(&curPoint);

          int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
          Menu.DestroyMenu();                                           
          switch (RetCd)
            {
            case 102: gs_AccessWnds.AccessNode(-1, Txt); break;
            case 103: ; break;
            default:
              {
              if (RetCd>=110 && RetCd<110+pTag->m_Pages.GetCount())
                {
                S.Format("%s\t%s", Txt, pTag->m_Pages[RetCd-110]->m_pPage->m_sPageName);

                gs_pPrj->FindTag(S);
                }
              break;
              }
            case 130:
              CopyTagList2Clipboard(m_hClassItem, 0, "Graphic", CString(""), CString(""));
              break;
            }
          }
        else
          ASSERT_ALWAYS(FALSE, "Bad Tag Lookup");
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
        ActivateWndByName(Txt);
        break;
        }
      case TrID_Node: // NOdes
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
  FixFilterStuff();
  SetFilter();
  LoadTagTree();
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
  FixFilterStuff();
  SetFilter();
  LoadTagTree();
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

  *pResult = 0;
  }
