// FindTagsDlg.cpp : implementation file
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
                                                        
extern "C" 
  {
#include "grldefs.h"
  }

#include "afxwin.h"
#include "FindTagsDlg.h"
#include ".\findtagsdlg.h"

//=========================================================================
//
//
//
//=========================================================================

CFindTagsDlg * CFindTagsDlg::gs_pDlg=NULL;
static void CALLBACK DoTimer(HWND hWnd, UINT nMsg, UINT_PTR idEvent, DWORD dwTime ) 
  {
  if (CFindTagsDlg::gs_pDlg)
    {
    ASSERT_ALWAYS(CFindTagsDlg::gs_pDlg && CFindTagsDlg::gs_pDlg->m_hWnd==hWnd, "CFindTagsDlg - Bad DoTimer", __FILE__, __LINE__);
    CFindTagsDlg::gs_pDlg->ProcessTimer(idEvent);
    }
  }


//--------------------------------------------------------------------------

CString TrimIt(LPCTSTR Tag, LPCTSTR Separ)
  {
  CString X(Tag);
  int i=X.Find(Separ);
  if (i>=0)
    X = X.Left(i);
  return X;
  }

//=========================================================================
//
//
//
//=========================================================================

const byte TrkLeft      = 0x01;
const byte TrkTop       = 0x02;
const byte TrkRight     = 0x04;
const byte TrkBottom    = 0x08;
const byte TrkSep0      = 0x10;
const byte TrkSep1      = 0x20;
const byte TrkSep2      = 0x40;

static struct { UINT m_ID; byte m_TrkTL; byte m_TrkBR; } const CtrlIDs[]=//CtrlIDCount]= 
  {
    { IDC_TAGFILTER         , TrkLeft   |TrkBottom , TrkRight   |TrkBottom  },
    { IDC_FILTOPTIONS       , TrkRight  |TrkBottom , TrkRight   |TrkBottom  },
    { IDC_TAGFILTERRULE     , TrkLeft   |TrkBottom , TrkRight   |TrkBottom  },
    { ID_GOTO               , TrkRight  |TrkBottom , TrkRight   |TrkBottom  },
    { ID_GOACCESS           , TrkRight  |TrkBottom , TrkRight   |TrkBottom  },
    { IDCANCEL              , TrkRight  |TrkBottom , TrkRight   |TrkBottom  },
    { IDC_TAGTREE           , TrkLeft   |TrkTop    , TrkRight   |TrkBottom  },
    { IDC_STRNM4            , TrkLeft   |TrkBottom , TrkLeft    |TrkBottom  },
    { IDC_STATICFILTER      , TrkLeft   |TrkBottom , TrkRight   |TrkBottom  },
    { IDC_STRNM3            , TrkLeft   |TrkBottom , TrkLeft    |TrkBottom  },
  };

const int CtrlIDCount = sizeof(CtrlIDs)/sizeof(CtrlIDs[0]);

static LPCTSTR Section="FindTags";

//=========================================================================
//
//
//
//=========================================================================

CFTItem::CFTItem(CFindTagsDlg * Dlg, LPCTSTR Tag, CFTClass * ClassId) : m_Dlg(*Dlg) 
  {
  m_sTag=Tag;
  m_sTagLwr=Tag;
  m_sTagLwr.MakeLower();
  m_pClassId=ClassId;
  m_Selected=false;
  m_Marked=false;
  m_Loaded=false;
  m_nRefs=0;
  m_hTree=NULL;
  };

LPCTSTR CFTItem::Display()
  {
  if (m_Loaded)
    {
    m_sDisplay.Format("%s (%i)", m_sTag, m_nRefs);
    }
  else
    m_sDisplay=m_sTag;
  return m_sDisplay;
  };

//=========================================================================
//
//
//
//=========================================================================

IMPLEMENT_DYNAMIC(CFindTagsDlg, CDialog)
CFindTagsDlg::CFindTagsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFindTagsDlg::IDD, pParent)
  {
  m_Inited      = false;
  m_bCaseSens   = false;
  m_pFilterRE   = NULL;
  m_GoTo        = false;
  m_DoAccess    = false;
  m_LastItem    = NULL;
  m_nTimer      = 0;
  m_TrendLine   =  -1;
  }

CFindTagsDlg::~CFindTagsDlg()
  {
  ClearTags();
  }

void CFindTagsDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_TAGTREE, m_TagTree);
  DDX_Control(pDX, IDC_TAGFILTERRULE, m_TagFilterRule);
  DDX_Control(pDX, IDC_TAGFILTER, m_TagFilter);
  }


BEGIN_MESSAGE_MAP(CFindTagsDlg, CDialog)
  ON_NOTIFY(TVN_SELCHANGED, IDC_TAGTREE, OnTvnSelchangedTagtree)
  ON_CBN_SELCHANGE(IDC_TAGFILTERRULE, OnCbnSelchangeTagfilterrule)
  ON_EN_CHANGE(IDC_TAGFILTER, OnEnChangeTagfilter)
  ON_BN_CLICKED(ID_GOTO, &CFindTagsDlg::OnBnClickedGoto)
  ON_BN_CLICKED(ID_GOACCESS, &CFindTagsDlg::OnBnClickedGoaccess)
  ON_NOTIFY(NM_DBLCLK, IDC_TAGTREE, &CFindTagsDlg::OnNMDblclkTagtree)
  ON_WM_GETMINMAXINFO()
  ON_WM_SIZE()
  ON_NOTIFY(NM_CLICK, IDC_TAGTREE, &CFindTagsDlg::OnNMClickTagtree)
  ON_NOTIFY(TVN_SELCHANGING, IDC_TAGTREE, &CFindTagsDlg::OnTvnSelchangingTagtree)
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDOWN()
  ON_NOTIFY(NM_RCLICK, IDC_TAGTREE, &CFindTagsDlg::OnNMRclickTagtree)
END_MESSAGE_MAP()


// CFindTagsDlg message handlers

BOOL CFindTagsDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  CProfINIFile PF(PrjIniFile());
  CString S;

  GetWindowRect(&m_WindowRctInit);
  GetClientRect(&m_ClientRctInit);
  m_CtrlRcts.SetSize(CtrlIDCount);
  for (int i=0; i<CtrlIDCount; i++)
    {
    GetDlgItem(CtrlIDs[i].m_ID)->GetWindowRect(&m_CtrlRcts[i]);
    ScreenToClient(&m_CtrlRcts[i]);
    }

  m_TagFilterRule.AddString(eFilterRuleNames[eFROff]);
  m_TagFilterRule.AddString(eFilterRuleNames[eFRContains]);
  m_TagFilterRule.AddString(eFilterRuleNames[eFRWildCard]);
  m_TagFilterRule.AddString(eFilterRuleNames[eFRRegExp]);

  S=PF.RdStr("FindTags", "TagFilterRule", eFilterRuleNames[eFRContains]);
  if (m_TagFilterRule.SelectString(-1, S)<0)
    m_TagFilterRule.SelectString(-1, eFilterRuleNames[eFRContains]);

  m_TagFilter.SetWindowText(PF.RdStr("FindTags", "TagFilter", ""));

  BuildTags();

  SetFilter();

  LoadTagTree();

  m_Inited=true;

  CenterWindow();

  ShowWindow(SW_SHOW);
  UpdateDialogControls(this, FALSE);

  RestorePos();
  return TRUE;
  }

//--------------------------------------------------------------------------

void CFindTagsDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
  {
  CDialog::OnGetMinMaxInfo(lpMMI);

  if (!m_Inited)
    return;

  lpMMI->ptMinTrackSize.x = m_WindowRctInit.Width();
  lpMMI->ptMinTrackSize.y = m_WindowRctInit.Height();
  }

//--------------------------------------------------------------------------

void CFindTagsDlg::OnSize(UINT nType, int cx, int cy) 
  {
  CDialog::OnSize(nType, cx, cy);
  if (m_Inited)
    {
    PositionControls(cx, cy);
    InvalidateRect(NULL);
    SavePos();
    }
  }

//--------------------------------------------------------------------------

void CFindTagsDlg::PositionControls(int cx, int cy)
  {
  if (!m_Inited)
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

void CFindTagsDlg::SavePos()
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

void CFindTagsDlg::RestorePos()
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

BOOL CFindTagsDlg::XXOnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
  {
  HD_NOTIFY* pNM = (HD_NOTIFY*)lParam;
  switch (pNM->hdr.code)
    {
    case LVN_GETDISPINFO:
      {
      NMLVDISPINFO * pnmv = (NMLVDISPINFO*)lParam;
      if (pNM->hdr.idFrom==IDC_TAGLIST && pnmv->item.lParam<m_Tags.GetSize())
        {
        CFTItem &TI=*(m_Tags[pnmv->item.lParam]);
        //switch (pnmv->item.iSubItem)
        //  {
        //  case ColNumberMark:
        //  case ColNumberClass:
        //  case ColNumberCurrent:
        //  case ColNumberReplace:
        //  case ColNumberStatus:
        //  case ColNumberPage:
        //    pnmv->item.pszText=(LPSTR)(LPCTSTR)TI.GetString(pnmv->item.iSubItem);
        //    break;
        //    //case 4:
        //    //  pnmv->item.pszText=TRI.sSetCnt();
        //    //  break;
        //  }
        }
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

//--------------------------------------------------------------------------

void CFindTagsDlg::OnOK() 
  {
  UpdateData(TRUE);

  SaveProfile();
  SavePos();

  CDialog::OnOK();
  }

//--------------------------------------------------------------------------

void CFindTagsDlg::OnCancel()
  {
  SaveProfile();
  SavePos();

  CDialog::OnCancel();
  }

//--------------------------------------------------------------------------

void CFindTagsDlg::OnBnClickedGoto()
  {
  UpdateData(TRUE);
  SaveProfile();
  m_GoTo = true;
  //if (m_TagName.GetLength()==0)
  //  m_TagFilter.GetWindowText(m_TagName);
  //m_TrndName;
  if (m_TagName.GetLength()>0)
    OnOK();
  }

//--------------------------------------------------------------------------

void CFindTagsDlg::OnBnClickedGoaccess()
  {
  UpdateData(TRUE);
  SaveProfile();
  m_DoAccess = true;
  //if (m_TagName.GetLength()==0)
  //  m_TagFilter.GetWindowText(m_TagName);
  if (m_TagName.GetLength()>0)
    OnOK();
  }

//--------------------------------------------------------------------------

void CFindTagsDlg::OnNMDblclkTagtree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  SaveProfile();
  m_GoTo=true;
  m_DoAccess=false;

  OnOK();

  *pResult = 0;
  }

//--------------------------------------------------------------------------

static int HpTagTest(void * p, void * q)
  {
  CFTItem * r1=(CFTItem*)p;
  CFTItem * r2=(CFTItem*)q;
  int Cmp=_stricmp(r1->m_sTag, r2->m_sTag);
  //CArray<int,int> &SO = r1->m_Dlg.m_SortOrder;
  //int Cmp=0;
  //for (int i=0; i<ColCount; i++)
  //  {
  //  int iCol=abs(SO[i]);
  //  if (iCol>=ColCount)
  //    break;
  //  int iDir=SO[i]>0?1:-1;
  //  switch (iCol)
  //    {
  //    case ColNumberClass   : Cmp=_stricmp(r1->m_pClassId->m_sClassId, r2->m_pClassId->m_sClassId); break;
  //    case ColNumberCurrent : Cmp=_stricmp(r1->m_sTag, r2->m_sCurrent); break;
  //    //case ColNumberReplace : Cmp=_stricmp(r1->GetString(ColNumberReplace), r2->GetString(ColNumberReplace)); break;
  //    //case ColNumberStatus  : Cmp=_stricmp(r1->m_sStatus, r2->m_sStatus); break;
  //    }
  //  Cmp*=iDir;
  //  if (Cmp)
  //    break;
  //  }
  return Cmp<0;
  }

static int HpClassTest(void * p, void * q)
  {
  CFTClass * r1=(CFTClass*)p;
  CFTClass * r2=(CFTClass*)q;
  int Cmp=_stricmp(r1->m_sClassId, r2->m_sClassId);
  return Cmp<0;
  }

static int HpPageTest(void * p, void * q)
  {
  CFTPage * r1=(CFTPage*)p;
  CFTPage * r2=(CFTPage*)q;
  int Cmp=_stricmp(r1->m_sPageId, r2->m_sPageId);
  return Cmp<0;
  }

//--------------------------------------------------------------------------

void CFindTagsDlg::BuildTags()
  {
  ClearTags();

  SFEFlwLib* pFlwLib = gs_pPrj->FlwLib();
  if (pFlwLib)
    {
    Strng_List RawTags;
    int NTags;

    CWaitMsgCursor WaitMsg("Building Tag List");

    RawTags.Append("*");
    NTags = gs_pPrj->FlwLib()->FE_TagOperation(FETOp_GetActive, RawTags);

    m_TagMap.InitHashTable(FindNextPrimeNumber((UINT)(NTags*1.18)));
    m_TagHMap.InitHashTable(FindNextPrimeNumber((UINT)(NTags*1.18)));
    m_ClassMap.InitHashTable(FindNextPrimeNumber((UINT)(100)));
    m_PageMap.InitHashTable(FindNextPrimeNumber((UINT)(100)));
    //m_Monitor.InitHashTable(FindNextPrimeNumber((UINT)(NTags*1.18)));

    for (Strng *pTag=RawTags.First(); pTag; pTag=RawTags.Next())
      {
      if (pTag->XStrICmp(PlantModelTag)==0 || pTag->XStrICmp(PlantAreaTag)==0)
        continue;

      Strng ClassId;
      flag b = gs_pPrj->RequestModelClassId(pTag->Str(), ClassId);
      if (!b)
        ClassId="";

      for (int iClass=0; iClass<m_Classes.GetCount(); iClass++)
        if (m_Classes[iClass]->m_sClassId.CompareNoCase(ClassId())==0)
          break;
      if (iClass==m_Classes.GetCount())
        {
        CFTClass *pClass=new CFTClass(this, ClassId());
        m_Classes.Add(pClass);
        m_ClassMap.SetAt(pClass->m_sClassId, pClass);
        }

      CFTItem *pItem=new CFTItem(this, pTag->Str(), m_Classes[iClass]);

      //if (m_pMarkedTags)
      //  {
      //  POSITION Pos;
      //  pItem->m_Marked=m_MarkedTagMap.Lookup(pTag->Str(), Pos);
      //  }

      m_Tags.Add(pItem);
      m_TagMap.SetAt(pItem->m_sTag, pItem);
      //MonitorAdd(pItem, pItem->SetReqd());
      }
    }
  // This info is no longer needed - and becomes incorrect after an apply.
  //m_MarkedTagMap.RemoveAll();


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

          CFTPage *pPage=new CFTPage(this, GrfTitle);
          m_Pages.Add(pPage);
          m_PageMap.SetAt(pPage->m_sPageId, pPage);

          Strng_List GrfTagList;
          int NGrfTags = pGDoc->GetTagList(GrfTagList);
          for (Strng * pT=GrfTagList.First(); pT; pT=GrfTagList.Next())
            {

            CFTItem *pItem;
            if (m_TagMap.Lookup(pT->Str(), pItem))
              {
              pItem->m_Pages.Add(pPage);
              //if (pItem->m_sPages.GetLength()>0)
              //  pItem->m_sPages+=",";
              //pItem->m_sPages+=pPage->m_sPageId;

              }
            }
          }
        }
      }
    }

  HpSort(m_Tags.GetCount(), (void**)&m_Tags[0], HpTagTest);
  HpSort(m_Classes.GetCount(), (void**)&m_Classes[0], HpClassTest);
  HpSort(m_Pages.GetCount(), (void**)&m_Pages[0], HpPageTest);

  };

//--------------------------------------------------------------------------

void CFindTagsDlg::ClearTags()
  {
  m_TagMap.RemoveAll();
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

//--------------------------------------------------------------------------

void CFindTagsDlg::SetFilter()
  {
  CString FilterString;
  m_TagFilter.GetWindowText(FilterString);
  FilterString = FilterString.Trim();
  eFilterRule Rule=(eFilterRule)m_TagFilterRule.GetCurSel();

  bool DoFilter=(FilterString.GetLength()>0) && (Rule!=eFROff);
  if (DoFilter)
    {
    if (!m_bCaseSens)
      FilterString.MakeLower();

    switch (Rule)
      {
      case eFRContains:
        {
        for (int i=0; i<m_Tags.GetCount(); i++)
          {
          CFTItem & Tg=*m_Tags[i];
          //BOOL Selected;
          if (m_bCaseSens)
            Tg.m_Selected=strstr(Tg.m_sTag, FilterString)!=NULL;
          else
            Tg.m_Selected=strstr(Tg.m_sTagLwr, FilterString)!=NULL;
          //Tg.ChangeSetReqd(CFTItem::eSRSelected, Selected);
          }
        break;
        }
      case eFRWildCard:
      case eFRRegExp:
        {
        if (!m_pFilterRE)
          m_pFilterRE=new IRegExpPtr("VBScript.RegExp");
        (*m_pFilterRE)->Global     = _variant_t(true);//' Set do all occurs
        (*m_pFilterRE)->IgnoreCase = _variant_t(!m_bCaseSens);//' Set case insensitivity.
        switch (Rule)
          {
          case eFRWildCard:
            {
            CString S("^");
            for (int i=0; i<FilterString.GetLength(); i++)
              {
              if (FilterString[i]=='*')
                S+=".*";
              else if (FilterString[i]=='?')
                S+=".";
              else if (FilterString[i]!='^' && FilterString[i]!='$')
                S+=FilterString[i];
              }
            S+="$";
            (*m_pFilterRE)->Pattern = _bstr_t(S);
            break;
            }
          case eFRRegExp:
            {
            CString S("^");
            for (int i=0; i<FilterString.GetLength(); i++)
              {
              if (FilterString[i]!='^' && FilterString[i]!='$')
                S+=FilterString[i];
              }
            S+="$";
            (*m_pFilterRE)->Pattern = _bstr_t(S);
            break;
            }
          default:
            (*m_pFilterRE)->Pattern = _bstr_t(FilterString);
            break;
          }

        for (int i=0; i<m_Tags.GetCount(); i++)
          {
          CFTItem & Tg=*m_Tags[i];
          try
            {
            Tg.m_Selected=(*m_pFilterRE)->Test(_bstr_t(Tg.m_sTag))==VARIANT_TRUE;
            //Tg.ChangeSetReqd(CFTItem::eSRSelected, (*m_pFilterRE)->Test(_bstr_t(Tg.m_sTag))==VARIANT_TRUE);
            }
          catch(...)
            {
            //ReportError("InFilter", 0, "Failure in Regular expressions");
            LogError("TagChangeFilter", 0, "Failure in Regular expressions");
            Tg.m_Selected=true;
            //Tg.ChangeSetReqd(CFTItem::eSRSelected, true);
            }
          }
        break;
        }
      }
    }
  else
    {
    for (int i=0; i<m_Tags.GetCount(); i++)
      {
      CFTItem & Tg=*m_Tags[i];
      Tg.m_Selected=true;
      //Tg.ChangeSetReqd(CFTItem::eSRSelected, true);
      }
    };
  };

//--------------------------------------------------------------------------

const int TagId       = 0x10000000;
const int GraphPageId = 0x20000000;
const int TrendPageId = 0x30000000;
const int FTIdMask    = 0xf0000000;
const int FTValueMask = 0x0fffffff;

void CFindTagsDlg::LoadTagTree()
  {
  CString S;
  m_TagTree.LockWindowUpdate();
  //m_TagTree.DeleteAllItems();

  for (int i=0; i<m_Tags.GetCount(); i++)
    {
    CFTItem & Tg=*m_Tags[i];
    if (Tg.m_Selected)
      {
      if (!Tg.m_hTree)
        {
        Tg.m_hTree = m_TagTree.InsertItem(Tg.Display(), TVI_ROOT);
        m_TagTree.SetItemData(Tg.m_hTree, TagId);
        m_TagHMap.SetAt(Tg.m_hTree, m_Tags[i]);
        if (Tg.m_Loaded)
          LoadTagItem(Tg);
        }
      }
    else
      {
      if (Tg.m_hTree)
        {
        m_TagTree.DeleteItem(Tg.m_hTree);
        m_TagHMap.RemoveKey(Tg.m_hTree);
        Tg.m_hTree=NULL;
        }
      }
    }

  m_TagTree.UnlockWindowUpdate();

  HTREEITEM hTop=m_TagTree.GetFirstVisibleItem();
  if (hTop)
    {
    m_TagTree.SetItemState(hTop, TVIS_SELECTED, TVIS_SELECTED);
    m_TagName = TrimIt(m_TagTree.GetItemText(hTop), " (");
    }
  };

//--------------------------------------------------------------------------

void CFindTagsDlg::LoadTagItem(CFTItem & Tg, bool UpdateTag)
  {
  CString S;

  if (!Tg.m_Loaded)
    {
    Tg.m_nRefs=Tg.m_Pages.GetCount();

    POSITION Pos=ScdApp()->TrendTemplate().GetFirstDocPosition();
    int iLine=-1;
    while (Pos)
      {
      CTagVwDoc * pTrndDoc = (CTagVwDoc*)(ScdApp()->TrendTemplate().GetNextDoc(Pos));
      if (pTrndDoc->FindTag(Tg.m_sTag, iLine, false))
        Tg.m_nRefs++;
      }

    Tg.m_Loaded=true;
    }

  if (Tg.m_nRefs<=1)
    return;

  if (!m_TagTree.GetChildItem(Tg.m_hTree))
    {
    HTREEITEM h;
    for (int i=0; i<Tg.m_Pages.GetCount(); i++)
      {
      Strng S(Tg.m_Pages[i]->m_sPageId);
      S.FnCheckExtension(".scg");
      h=m_TagTree.InsertItem(S(), Tg.m_hTree, TVI_LAST);
      m_TagTree.SetItemData(h, GraphPageId);
      m_TagTree.EnsureVisible(h);
      }

    POSITION Pos=ScdApp()->TrendTemplate().GetFirstDocPosition();
    while (Pos)
      {
      CTagVwDoc* pTrndDoc = (CTagVwDoc*)(ScdApp()->TrendTemplate().GetNextDoc(Pos));
      int iLine=-1;
      while (pTrndDoc->FindTag(Tg.m_sTag, iLine, false))
        {
        S=pTrndDoc->GetTitle();
        S = S + " : " + pTrndDoc->DS[iLine].sTag();

        h=m_TagTree.InsertItem(S, Tg.m_hTree, TVI_LAST); m_TagTree.EnsureVisible(h);
        m_TagTree.SetItemData(h, TrendPageId+iLine);
        }
      }
    }
  if (UpdateTag)
    m_TagTree.SetItemText(Tg.m_hTree, Tg.Display());
  };

//--------------------------------------------------------------------------

void CFindTagsDlg::SaveProfile()
  {
  CProfINIFile PF(PrjIniFile());
  CString S;

  m_TagFilterRule.GetWindowText(S);
  PF.WrStr("FindTags", "TagFilterRule", S);
  m_TagFilter.GetWindowText(S);
  PF.WrStr("FindTags", "TagFilter", S);
  };

//--------------------------------------------------------------------------

void CFindTagsDlg::DoOnItem(HTREEITEM hItem, bool RClick)
  {
  if (hItem)// && hItem!=m_LastItem)
    {
    m_LastItem=hItem;
    switch((m_TagTree.GetItemData(hItem) & FTIdMask))
      {
      case TagId:
        {
        CFTItem *p=NULL;
        if (m_TagHMap.Lookup(hItem, p))
          LoadTagItem(*p, true);
        m_TagName   = TrimIt(m_TagTree.GetItemText(hItem), " (");
        m_GraphName = "";
        m_TrendName = "";
        m_TrendLine = -1;

        dword Options;
        if (RClick)
          {
          Options=0;

          CMenu Menu;
          Menu.CreatePopupMenu();

          Menu.AppendMenu(MF_STRING, 102, "GoTo");
          Menu.AppendMenu(MF_STRING, 103, "Access");

          CPoint curPoint;
          GetCursorPos(&curPoint);

          int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
          Menu.DestroyMenu();                                           
          switch (RetCd)
            {
            case 102: Options |= FTO_MoveCursor|FTO_HighliteSlow; break;
            case 103: Options |= FTO_DoAccess|FTO_HighliteSlow;   break;
            }
          }
        else
          Options=FTO_HighliteSlow;

        if (Options)
          gs_pPrj->FindTag(m_TagName, NULL, NULL, -1, Options|FTO_HighliteSlow);
        break;
        }
      case GraphPageId:
        {
        HTREEITEM hTag=m_TagTree.GetParentItem(hItem);
        m_TagName   = TrimIt(m_TagTree.GetItemText(hTag), " (");
        m_GraphName = TrimIt(m_TagTree.GetItemText(hItem), " :");
        m_TrendName = "";
        m_TrendLine = -1;
        
        dword Options;
        if (RClick)
          {
          Options=0;

          CMenu Menu;
          Menu.CreatePopupMenu();

          Menu.AppendMenu(MF_STRING, 102, "GoTo");
          Menu.AppendMenu(MF_STRING, 103, "Access");

          CPoint curPoint;
          GetCursorPos(&curPoint);

          int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
          Menu.DestroyMenu();                                           
          switch (RetCd)
            {
            case 102: Options |= FTO_MoveCursor; break;
            case 103: Options |= FTO_DoAccess;   break;
            }
          }
        else
          Options=FTO_HighliteSlow;

        if (Options)
          gs_pPrj->FindTag(m_TagName, m_GraphName, NULL, -1, Options);
        break;
        }
      case TrendPageId:
        {
        HTREEITEM hTag=m_TagTree.GetParentItem(hItem);
        m_TagName   = TrimIt(m_TagTree.GetItemText(hTag), " (");
        m_GraphName = "";
        m_TrendName = TrimIt(m_TagTree.GetItemText(hItem), " :");
        m_TrendLine = m_TagTree.GetItemData(hItem) & FTValueMask;

        dword Options=0;
        if (RClick)
          {

          CMenu Menu;
          Menu.CreatePopupMenu();

          Menu.AppendMenu(MF_STRING, 103, "Access");

          CPoint curPoint;
          GetCursorPos(&curPoint);

          int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
          Menu.DestroyMenu();                                           
          switch (RetCd)
            {
            case 102: Options |= FTO_MoveCursor; break;
            case 103: Options |= FTO_DoAccess;   break;
            }
          }

        if (Options)
          gs_pPrj->FindTag(m_TagName, m_GraphName, NULL, -1, Options|FTO_HighliteSlow);
        else
          gs_pPrj->FindTag(m_TagName, NULL, m_TrendName, m_TrendLine, FTO_HighliteSlow);
        break;
        }
      default:
        m_TagName   = "";
        m_GraphName = "";
        m_TrendName = "";
        m_TrendLine = -1;
        break;
      }
    }
  }

void CFindTagsDlg::OnTvnSelchangedTagtree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

  switch (pNMTreeView->action)
    {
    case 1:
    case 2:
      {
      if (m_nTimer)
        {
        KillTimer(m_nTimer);
        m_nTimer=0;
        }

      DoOnItem(m_TagTree.GetSelectedItem(), false);

      break;
      }
    }

  *pResult = 0;
  }

void CFindTagsDlg::OnCbnSelchangeTagfilterrule()
  {
  SetFilter();
  LoadTagTree();
  }

void CFindTagsDlg::OnEnChangeTagfilter()
  {
  SetFilter();
  LoadTagTree();
  }


void CFindTagsDlg::ProcessTimer(UINT_PTR idEvent)
  {
  switch (idEvent)
    {
    case 1:
      DoOnItem(gs_pDlg->m_LastItem, false);
      m_nTimer = 0;
      gs_pDlg  = 0;
      break;
    case 2:
      break;
    };
  }

void CFindTagsDlg::OnNMClickTagtree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

  gs_pDlg=this;
  m_nTimer = SetTimer(1, 100, DoTimer);

  *pResult = 0;
  }

void CFindTagsDlg::OnTvnSelchangingTagtree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

  if (m_nTimer)
    {
    KillTimer(m_nTimer);
    m_nTimer=0;
    }

  *pResult = 0;
  }

void CFindTagsDlg::OnNMRclickTagtree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

  gs_pDlg=this;
  //m_RClick++;
  //m_nTimer = SetTimer(1, 100, DoTimer);
  DoOnItem(m_TagTree.GetSelectedItem(), true);

  *pResult = 0;
  }

//=========================================================================
//
//
//
//=========================================================================

