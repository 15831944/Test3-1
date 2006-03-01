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

extern "C" 
  {
#include "grldefs.h"
  }

#include "afxwin.h"
#include "FindTagsDlg.h"
#include ".\findtagsdlg.h"


// CFindTagsDlg dialog

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

IMPLEMENT_DYNAMIC(CFindTagsDlg, CDialog)
CFindTagsDlg::CFindTagsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFindTagsDlg::IDD, pParent)
  {
  m_Inited=false;
  m_bCaseSens=false;
  m_pFilterRE=NULL;
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
END_MESSAGE_MAP()


// CFindTagsDlg message handlers

BOOL CFindTagsDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  CProfINIFile PF(PrjIniFile());
  CString S;

  m_TagFilterRule.AddString(eFilterRuleNames[eFROff]);
  m_TagFilterRule.AddString(eFilterRuleNames[eFRContains]);
  m_TagFilterRule.AddString(eFilterRuleNames[eFRWildCard]);
  m_TagFilterRule.AddString(eFilterRuleNames[eFRRegExp]);

  S=PF.RdStr("FindTags", "TagFilterRule", eFilterRuleNames[eFRContains]);
  if (m_TagFilterRule.SelectString(-1, S)<0)
    m_TagFilterRule.SelectString(-1, eFilterRuleNames[eFRContains]);

  m_TagFilter.SetWindowText(PF.RdStr("FindTags", "TagFilter", ""));

  //m_ShowAllBtn.SetCheck(PF.RdLong("FindTags", "ShowAll", 0));
  //if (m_pMarkedTags)
  //  m_MarkedOnlyBtn.SetCheck(PF.RdLong("FindTags", "MarkedOnly", 1));
  //else
  //  {
  //  m_MarkedOnlyBtn.SetCheck(0);
  //  m_MarkedOnlyBtn.EnableWindow(0);
  //  }

  //m_Find.SetWindowText(PF.RdStr("FindTags", "FindString", "^"));
  //m_Replace.SetWindowText(PF.RdStr("FindTags", "ReplaceString", ""));
  //m_bCaseSens=PF.RdInt("FindTags", "CaseSensitive", 0)!=0;
  //m_bCaseSens=false; // force it

  //m_ShowAllTags=m_ShowAllBtn.GetCheck()!=0;
  //m_MarkedOnly=m_MarkedOnlyBtn.GetCheck()!=0;

  //int ColLen[ColCount];
  //m_TagList.SetExtendedStyle(m_TagList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);

  //for (int i=0; i<ColCount; i++)
  //  {
  //  CString S;
  //  S.Format("ColWidth.%s", ColHeadName[i]);
  //  LPCTSTR Nm=(i==ColNumberMark) ? (m_pMarkedTags?"    ":"  "): ColHeadName[i];
  //  int hw=m_TagList.GetStringWidth(Nm)+15;
  //  int cw=PF.RdInt("FindTags", S, ColWide[i]);
  //  switch (i)
  //    {
  //    case ColNumberMark:   cw=hw;                              break;  
  //    case ColNumberClass:  m_ClassColWidth=cw; cw=Max(0, cw);  break;  
  //    case ColNumberPage:   m_PageColWidth=cw; cw=Max(0, cw);   break;  
  //    default:              cw=Max(cw, hw);                     break;  
  //    }

  //  m_TagList.InsertColumn(i, ColHeadTxt[i], LVCFMT_LEFT, cw);
  //  m_TagList.SetColumnWidth(i, cw);
  //  }

  //int SBWide=GetSystemMetrics(SM_CXVSCROLL);
  //CRect Rct;
  //m_ClassList.GetClientRect(&Rct);
  //m_ClassList.InsertColumn(0, "ClassIds", LVCFMT_LEFT, Rct.Width()-SBWide);
  //m_ClassList.SetExtendedStyle(m_ClassList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

  //m_PageList.GetClientRect(&Rct);
  //m_PageList.InsertColumn(0, "Pages", LVCFMT_LEFT, Rct.Width()-SBWide);
  //m_PageList.SetExtendedStyle(m_PageList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

  //int TagColWide=120;
  //m_ApplyLog.GetClientRect(&Rct);
  //m_ApplyLog.InsertColumn(0, "Note", LVCFMT_LEFT, Rct.Width()-SBWide);
  //m_ApplyLog.SetExtendedStyle(m_ApplyLog.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
  //m_ApplyProgress.SetRange(0, 100);
  //m_ApplyProgress.SetPos(0);

  //m_ApplyBtn.EnableWindow(0);

  //m_ChangeCount.SetWindowText("0");
  //m_AdjustCount.SetWindowText("0");
  //m_DuplicateCount.SetWindowText("0");
  //m_ImportCount.SetWindowText("0");
  //m_ApplyCount.SetWindowText("0");

  //GetClientRect(&m_ClientRctInit);
  //m_CtrlRcts.SetSize(CtrlIDCount);
  //for (int i=0; i<CtrlIDCount; i++)
  //  {
  //  GetDlgItem(CtrlIDs[i].m_ID)->GetWindowRect(&m_CtrlRcts[i]);
  //  ScreenToClient(&m_CtrlRcts[i]);
  //  }
  //m_SepInit[0]=(int)(0.5*(m_CtrlRcts[CtrlIDIndex(IDC_TAGLIST)].right+m_CtrlRcts[CtrlIDIndex(IDC_CLASSIDS)].left));
  //m_SepInit[1]=(int)(0.5*(m_CtrlRcts[CtrlIDIndex(IDC_CLASSIDS)].right+m_CtrlRcts[CtrlIDIndex(IDC_PAGES)].left));
  //m_SepInit[2]=(int)(0.5*(m_CtrlRcts[CtrlIDIndex(IDC_STATICCLIP)].right+m_CtrlRcts[CtrlIDIndex(IDC_STATICAPPLICATION)].left));

  //for (int i=0; i<3; i++)
  //  m_SepPos[i]=(1000*m_SepInit[i])/m_ClientRctInit.Width();

  //GetClientRect(&Rct);
  ////ClientToScreen(&Rct);
  //m_SepPos[0]= PF.RdInt("FindTags", "Separator1", m_SepPos[0]);
  //m_SepPos[1]= PF.RdInt("FindTags", "Separator2", m_SepPos[1]);
  //m_SepPos[2]= PF.RdInt("FindTags", "Separator3", m_SepPos[2]);
  //int xPos = PF.RdInt("FindTags", "XPos", -10000);//Rct.left);
  //int yPos = PF.RdInt("FindTags", "YPos", Rct.top);
  //int CW = PF.RdInt("FindTags", "Width", Rct.Width());
  //int CH = PF.RdInt("FindTags", "Height", Rct.Height());

  //m_InPlaceEditItem=-1;

  //i=0;
  //m_SortOrder.SetSize(ColCount+1);
  //m_SortOrder[i++]=2; // Tag Ascend
  //m_SortOrder[i++]=1; // Class Ascend
  //for ( ; i<m_SortOrder.GetSize(); i++)
  //  m_SortOrder[i]=ColCount; // Rest off

  BuildTags();

  //for (int i=0; i<m_Classes.GetCount(); i++)
  //  {                                                   
  //  CFTClass &Cl=*m_Classes[i];
  //  CString S;
  //  S.Format("Class.%s", Cl.m_sClassId);
  //  Cl.m_Checked = PF.RdInt("FindTags", S, Cl.m_Checked?1:0)!=0;
  //  }

  //for (int i=0; i<m_Pages.GetCount(); i++)
  //  {
  //  CFTPage &Pg=*m_Pages[i];
  //  CString S;
  //  S.Format("Page.%s", Pg.m_sPageId);
  //  Pg.m_Checked = PF.RdInt("FindTags", S, Pg.m_Checked?1:0)!=0;
  //  }


  SetFilter();

  LoadTagTree();
  //LoadPageList();
  //LoadClassList();

  //ShowSelectedColumns();
  //DoChecks();

  m_Inited=true;

  //if (xPos>-10000)
  //  {
  //  SetWindowPos(NULL, 0,0, 1,1 , SWP_NOREPOSITION | SWP_NOZORDER | SWP_SHOWWINDOW); // set size small to force an OnSize
  //  SetVisibleWindowPos(this, xPos, yPos, CW, CH);
  //  //PositionControls(CW, CH);
  //  }
  //else
  CenterWindow();

  ShowWindow(SW_SHOW);
  UpdateDialogControls(this, FALSE);

  return TRUE;
  }


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

  CDialog::OnOK();
  }

//--------------------------------------------------------------------------

void CFindTagsDlg::OnCancel()
  {
  SaveProfile();

  CDialog::OnCancel();
  }


//--------------------------------------------------------------------------

static int HpTagTest(void * p, void * q)
  {
  CFTItem * r1=(CFTItem*)p;
  CFTItem * r2=(CFTItem*)q;
  int Cmp=stricmp(r1->m_sTag, r2->m_sTag);
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
  //    case ColNumberClass   : Cmp=stricmp(r1->m_pClassId->m_sClassId, r2->m_pClassId->m_sClassId); break;
  //    case ColNumberCurrent : Cmp=stricmp(r1->m_sTag, r2->m_sCurrent); break;
  //    //case ColNumberReplace : Cmp=stricmp(r1->GetString(ColNumberReplace), r2->GetString(ColNumberReplace)); break;
  //    //case ColNumberStatus  : Cmp=stricmp(r1->m_sStatus, r2->m_sStatus); break;
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
  int Cmp=stricmp(r1->m_sClassId, r2->m_sClassId);
  return Cmp<0;
  }

static int HpPageTest(void * p, void * q)
  {
  CFTPage * r1=(CFTPage*)p;
  CFTPage * r2=(CFTPage*)q;
  int Cmp=stricmp(r1->m_sPageId, r2->m_sPageId);
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
  };

//--------------------------------------------------------------------------

void CFindTagsDlg::LoadTagItem(CFTItem & Tg, bool UpdateTag)
  {
  CString S;

  if (!Tg.m_Loaded)
    {
    Tg.m_nRefs=Tg.m_Pages.GetCount();
    // Get 'Other stuff from Project'

    Tg.m_nRefs+=3; // Trends
    Tg.m_nRefs+=3; // XRefs

    Tg.m_Loaded=true;
    }

  if (!m_TagTree.GetChildItem(Tg.m_hTree))
    {
    S.Format("Class: %s",Tg.m_pClassId->m_sClassId);
    HTREEITEM h=m_TagTree.InsertItem(S, Tg.m_hTree, TVI_LAST);
    m_TagTree.EnsureVisible(h);

    for (int i=0; i<Tg.m_Pages.GetCount(); i++)
      {
      S.Format("Graphic: %s",m_Pages[i]->m_sPageId);
      h=m_TagTree.InsertItem(S, Tg.m_hTree, TVI_LAST);
      m_TagTree.EnsureVisible(h);
      }

    h=m_TagTree.InsertItem("Trend: Flows", Tg.m_hTree, TVI_LAST); m_TagTree.EnsureVisible(h);
    h=m_TagTree.InsertItem("Trend: Levels", Tg.m_hTree, TVI_LAST); m_TagTree.EnsureVisible(h);
    h=m_TagTree.InsertItem("Trend: Temperatures", Tg.m_hTree, TVI_LAST); m_TagTree.EnsureVisible(h);

    HTREEITEM hx;
    hx=m_TagTree.InsertItem("XRefs", Tg.m_hTree, TVI_LAST); m_TagTree.EnsureVisible(h);


    CXRefInfoArray Refs;
    gs_pTheSFELib->RequestTagRefInfo(Tg.m_sTag, Refs);
    if (Refs.GetSize())
      {
      //Menu.AppendMenu(MF_SEPARATOR);
      //bool HasConns=0;
      //CMenu CnMenu;
      //CnMenu.CreatePopupMenu();
      //if (1)
      //  {
      //  bool FirstOne=true;
      //  int iTag=1;
      //  for (int iPass=0; iPass<2; iPass++)
      //    {
      //    for (int ii=0; ii<Refs.GetSize(); ii++)
      //      {
      //      CTagRefInfoItem  *Ref = Refs[ii];
      //      CString s, sID;
      //      if (Ref->m_fIsSetRef && iPass==0)
      //        {
      //        sLastSelected[iTag] = Ref->m_sRmtTag();
      //        s.Format(Ref->m_iSetCnt ? "%s -->(Set)":"%s -->()", Ref->m_sRmtID());
      //        }
      //      if (Ref->m_fIsGetRef && iPass==1)
      }

    LPCTSTR Vars[]={"Level", "ReqdPos", "P", NULL};
    LPCTSTR XRef[]={"GC_1", "PID_1", "V_1", NULL};
    for (LPCTSTR *p=Vars, *x=XRef; *p; p++, x++)
      {
      S.Format("%s [%s]", *x, *p);
      h=m_TagTree.InsertItem(S, hx, TVI_LAST); 
      m_TagTree.EnsureVisible(h);
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

void CFindTagsDlg::OnTvnSelchangedTagtree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
  // TODO: Add your control notification handler code here
  CFTItem *p;
  if (pNMTreeView->action==1 && m_TagHMap.Lookup(pNMTreeView->itemNew.hItem, p))
    {
    LoadTagItem(*p, true);
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
