// BulkTagChange.cpp : implementation file
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
#include "explorescd.h"

extern "C" 
  {
#include "grldefs.h"
  }

#include "afxwin.h"
#include "BulkTagChange.h"

// ==========================================================================

const int ColNumberMark     = 0;
const int ColNumberClass    = 1;
const int ColNumberCurrent  = 2;
const int ColNumberReplace  = 3;
const int ColNumberStatus   = 4;
const int ColNumberPage     = 5;
const int ColCount          = 6;
static LPCTSTR ColHeadName[ColCount]  = {"Mark", "Class", "Current", "Replacement", "Status", "Page"};//, "Pages"};
static LPCTSTR ColHeadTxt[ColCount]   = {   "", "Class", "Current", "Replacement", "Status", "Page"};//, "Pages"};
static const int ColWide[ColCount]    = {   10,    80,       120,           120,      60,   -100};
//const int CtrlIDCount = 42;
const byte TrkLeft      = 0x01;
const byte TrkTop       = 0x02;
const byte TrkRight     = 0x04;
const byte TrkBottom    = 0x08;
const byte TrkSep0      = 0x10;
const byte TrkSep1      = 0x20;
const byte TrkSep2      = 0x40;

static struct { UINT m_ID; byte m_TrkTL; byte m_TrkBR; } const CtrlIDs[]=//CtrlIDCount]= 
  {
    { IDC_TAGLIST                    , TrkLeft   |TrkTop    , TrkSep0   |TrkBottom  },
    { IDC_SHOWALLTAGS                , TrkLeft   |TrkBottom , TrkLeft   |TrkBottom  },
    { IDC_MARKEDONLY                 , TrkLeft   |TrkBottom , TrkLeft   |TrkBottom  },
    { IDC_TAGCHECKS                  , TrkSep0   |TrkBottom , TrkSep0   |TrkBottom  },
    { IDC_TAGCOUNT                   , TrkSep0   |TrkBottom , TrkSep0   |TrkBottom  },

    { IDC_CLASSIDS                   , TrkSep0   |TrkTop    , TrkSep1   |TrkBottom  },
    { IDC_CLASSCHECKS                , TrkSep1   |TrkBottom , TrkSep1   |TrkBottom  },
    { IDC_CLASSCOUNT                 , TrkSep1   |TrkBottom , TrkSep1   |TrkBottom  },

    { IDC_PAGES                      , TrkSep1   |TrkTop    , TrkRight  |TrkBottom  },
    { IDC_PAGECHECKS                 , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_PAGECOUNT                  , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },

    { IDC_TAGFILTERRULE              , TrkLeft   |TrkBottom , TrkSep2   |TrkBottom  },
    { IDC_TAGFILTER                  , TrkLeft   |TrkBottom , TrkSep2   |TrkBottom  },
    { IDC_TAGFIND                    , TrkLeft   |TrkBottom , TrkSep2   |TrkBottom  },
    { IDC_TAGREPLACE                 , TrkLeft   |TrkBottom , TrkSep2   |TrkBottom  },
    { IDC_FILTOPTIONS                , TrkSep2   |TrkBottom , TrkSep2   |TrkBottom  },
    { IDC_FINDOPTIONS                , TrkSep2   |TrkBottom , TrkSep2   |TrkBottom  },
    { IDC_REPLACEOPTIONS             , TrkSep2   |TrkBottom , TrkSep2   |TrkBottom  },
    { IDC_DOREPLACE                  , TrkSep2   |TrkBottom , TrkSep2   |TrkBottom  },
    { IDC_UNDOREPLACE                , TrkSep2   |TrkBottom , TrkSep2   |TrkBottom  },
    { IDC_STATICREPLACEMENT          , TrkLeft   |TrkBottom , TrkSep2   |TrkBottom  },
    { IDC_STRNM1                     , TrkLeft   |TrkBottom , TrkLeft   |TrkBottom  },
    { IDC_STRNM2                     , TrkLeft   |TrkBottom , TrkLeft   |TrkBottom  },
    { IDC_STRNM3                     , TrkLeft   |TrkBottom , TrkLeft   |TrkBottom  },
    { IDC_STRNM4                     , TrkLeft   |TrkBottom , TrkLeft   |TrkBottom  },
    { IDC_STATICFILTER               , TrkLeft   |TrkBottom , TrkSep2   |TrkBottom  },

    { IDC_STATICCLIP                 , TrkSep2   |TrkBottom , TrkSep2   |TrkBottom  },
    //{ IDC_COPYCLIP                   , TrkSep2   |TrkBottom , TrkSep2   |TrkBottom  },
    { IDC_EXPORT                     , TrkSep2   |TrkBottom , TrkSep2   |TrkBottom  },
    { IDC_IMPORT                     , TrkSep2   |TrkBottom , TrkSep2   |TrkBottom  },

    { IDC_STATICAPPLICATION          , TrkSep2   |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_APPLYPROGRESS              , TrkSep2   |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_APPLYLOG                   , TrkSep2   |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_APPLY                      , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },

    { IDC_STATICCHANGES              , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_STATICCHANGES1             , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_STATICCHANGES2             , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_STATICCHANGES3             , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_STATICCHANGES4             , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_STATICCHANGES5             , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_CHANGECOUNT                , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_ADJUSTCOUNT                , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_DUPLICATECOUNT             , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_IMPORTCOUNT                , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },
    { IDC_APPLYCOUNT                 , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },
    { IDOK                           , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },
    { IDCANCEL                       , TrkRight  |TrkBottom , TrkRight  |TrkBottom  },

    { IDC_SHOWCLASSES                , TrkLeft   |TrkBottom , TrkLeft   |TrkBottom  },
    { IDC_STATICSHOW                 , TrkLeft   |TrkBottom , TrkLeft   |TrkBottom  },
    { IDC_SHOWPAGES                  , TrkLeft   |TrkBottom , TrkLeft   |TrkBottom  },
  };

const int CtrlIDCount = sizeof(CtrlIDs)/sizeof(CtrlIDs[0]);

// ==========================================================================

CBTCItem::CBTCItem(CBulkTagChange * Dlg, LPCTSTR OldTag, CBTCClass * ClassId) : m_Dlg(*Dlg)
  {
  m_sCurrent=OldTag;
  m_pClassId=ClassId;
  m_Checked=true;
  m_Selected=true;
  m_Marked=false;
  m_DisplayIndex=-1;
  m_ApplyCnt=0;
  Start();
  }    


void CBTCItem::Start()
  {
  m_sReplacement=m_sCurrent;
  m_sCurrentLwr=m_sCurrent;
  m_sCurrentLwr.MakeLower();
  m_sStatus="";
  m_Changed=false;
  m_AdjustCnt=0;

  while (m_RepSeq.GetCount()>0)
    m_RepSeq.RemoveTail();
  }    

void CBTCItem::TagChanged()
  {
  m_sCurrent=m_sReplacement;
  m_sCurrentLwr=m_sCurrent;
  m_sCurrentLwr.MakeLower();
  m_sStatus="";
  m_Changed=false;
  m_AdjustCnt=0;

  while (m_RepSeq.GetCount()>0)
    m_RepSeq.RemoveTail();
  }    

void CBTCItem::Replace(LPCTSTR Replacement)
  {
  m_ReplaceUndoCnt=m_Dlg.m_ReplaceUndoCnt;

  dbgpln("Replace %3i %3i %30s %-30s", m_Dlg.m_ReplaceUndoCnt, m_ReplaceUndoCnt, m_sCurrent, m_sReplacement);

  m_RepSeq.AddTail(CBTCRepSeq(*this));

  m_AdjustCnt=0;
  Strng sRep(Replacement);
  if (TaggedObject::ValidateTagChanged(sRep))
    m_AdjustCnt++;
  Replacement=(LPCTSTR)sRep();

  m_Dlg.MonitorRemove(this, SetReqd());

  m_sReplacement=Replacement;

  m_Dlg.MonitorAdd(this, SetReqd());

  m_Changed = m_sCurrent.Compare(m_sReplacement)!=0;
  }

LPCTSTR CBTCItem::StatusString()
  {
  m_sStatus="";
  if (m_Changed)
    {
    if (m_AdjustCnt==1)
      m_sStatus.Format("Adjusted to '%s'", m_sReplacement);
    else if (m_AdjustCnt>1)
      m_sStatus.Format("Adjusted(%i) to '%s'", m_AdjustCnt, m_sReplacement);
    else if (SetReqd())
      m_sStatus="<--";
    }
  if (m_nDuplicateCount)
    {
    CString S;
    S.Format("%i Duplicate%s", m_nDuplicateCount, m_nDuplicateCount>1?"s":"");
    m_sStatus=S;
    }
  return m_sStatus;
  }

void CBTCItem::Undo()
  {
  while (m_RepSeq.GetCount()>0)
    {
    CBTCRepSeq & RS=m_RepSeq.GetTail();
    if (RS.m_ReplaceUndoCnt<m_Dlg.m_ReplaceUndoCnt)
      break;

    m_Dlg.MonitorRemove(this, SetReqd());
    *dynamic_cast<CBTCRepSeq*>(this)=RS;
    m_Dlg.MonitorAdd(this, SetReqd());

    dbgpln("Undo    %3i %3i %30s %-30s", m_Dlg.m_ReplaceUndoCnt, m_ReplaceUndoCnt, m_sCurrent, m_sReplacement);
    m_RepSeq.RemoveTail();
    }

  m_Changed=m_RepSeq.GetCount()>0;
  }    

LPCTSTR CBTCItem::GetString (int j)
  {
  switch (j)
    {
    case ColNumberMark   :   return m_Marked?"*":" ";
    case ColNumberClass  :   return m_pClassId->m_sClassId;
    case ColNumberCurrent:   return m_sCurrent;
    case ColNumberReplace:   return SetReqd() ? m_sReplacement : "";
    case ColNumberStatus :   return StatusString();
    case ColNumberPage   :   return m_sPages;
    default:  return "";
    }

  return "???";
  }
void CBTCItem::PutString (int j, LPCTSTR Str)
  {
  switch (j)
    {
    case ColNumberReplace:   m_sReplacement=Str;       break;
    case ColNumberStatus :   m_sStatus=Str;    break;
    default:                    break;
    }
  }

BOOL CBTCItem::PageChecked()
  {
  for (int i=0; i<m_Pages.GetCount(); i++)
    if (m_Pages[i]->m_Checked)
      return true;
  return false;
  }

BOOL CBTCItem::SetReqd(bool IgnoreCheck)
  {
  return ((m_Checked || IgnoreCheck) &&
    m_pClassId->m_Checked && 
    (!m_Dlg.m_MarkedOnly || m_Marked) && 
    m_Selected &&
    PageChecked()); 
  };

void CBTCItem::SaveSetReqd()
  {
  m_SetWasReqd=SetReqd();
  }

void CBTCItem::TestSetReqd()
  {
  BOOL SR=SetReqd();
  if (SR!=m_SetWasReqd)
    {
    m_Dlg.MonitorRemove(this, m_SetWasReqd);
    m_Dlg.MonitorAdd(this, SR);
    }
  }

void CBTCItem::ChangeSetReqd(eDoItTerms Term, BOOL On)
  {       
  BOOL IsNow;
  switch (Term)
    {
    case eSRCheck:    IsNow = m_Checked;  break; 
    case eSRMarked:   IsNow = m_Marked;   break; 
    case eSRSelected: IsNow = m_Selected; break; 
    default:_asm int 3;
    }

  if (On!=IsNow)
    {
    m_Dlg.MonitorRemove(this, SetReqd());

    switch (Term)
      {
      case eSRCheck:    m_Checked  = On; break; 
      case eSRMarked:   m_Marked   = On; break; 
      case eSRSelected: m_Selected = On; break; 
      default:_asm int 3;
      }

    m_Dlg.MonitorAdd(this, SetReqd());
    }
  };

BOOL CBTCItem::ShowIt()
  {
  return m_Dlg.m_ShowAllTags || SetReqd();
  };

// ==========================================================================

CBTCClass::CBTCClass(CBulkTagChange * Dlg, LPCTSTR ClassId) : m_Dlg(*Dlg)
  {
  m_sClassId=ClassId;
  m_Checked=true;
  }
// ==========================================================================

CBTCPage::CBTCPage(CBulkTagChange * Dlg, LPCTSTR PageId) : m_Dlg(*Dlg)
  {
  m_sPageId=PageId;
  m_Checked=true;
  }

// ==========================================================================
// CBulkTagChange dialog

IMPLEMENT_DYNAMIC(CBulkTagChange, CDialog)
CBulkTagChange::CBulkTagChange(CStringList * pMarkedTags, CWnd* pParent /*=NULL*/) : \
CDialog(CBulkTagChange::IDD, pParent),
m_InPlaceEdit(this),
m_TagList(this)
  {
  m_SetWhat=-1;
  m_Inited=false;
  m_Loading=false;
  m_MarkedOnly=false;
  m_pMarkedTags=pMarkedTags;
  if (m_pMarkedTags)
    {
    m_MarkedTagMap.InitHashTable(FindNextPrimeNumber((UINT)(m_pMarkedTags->GetCount()*1.18)));

    POSITION Pos=m_pMarkedTags->GetHeadPosition();
    while (Pos)
      {
      CString & S=m_pMarkedTags->GetNext(Pos);
      m_MarkedTagMap.SetAt(S, Pos);
      }
    }

  m_ReplaceUndoCnt=0;
  m_bCaseSens=false;
  m_pFilterRE=NULL;
  }

CBulkTagChange::~CBulkTagChange()
  { 
  delete m_pFilterRE;
  ClearTags();
  }

void CBulkTagChange::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_ADJUSTCOUNT, m_AdjustCount);
  DDX_Control(pDX, IDC_APPLY, m_ApplyBtn);
  DDX_Control(pDX, IDC_APPLYCOUNT, m_ApplyCount);
  DDX_Control(pDX, IDC_APPLYLOG, m_ApplyLog);
  DDX_Control(pDX, IDC_APPLYPROGRESS, m_ApplyProgress);
  DDX_Control(pDX, IDC_CHANGECOUNT, m_ChangeCount);
  DDX_Control(pDX, IDC_CLASSCHECKS, m_ClassChecks);
  DDX_Control(pDX, IDC_CLASSCOUNT, m_ClassCount);
  DDX_Control(pDX, IDC_CLASSIDS, m_ClassList);
  DDX_Control(pDX, IDC_DOREPLACE, m_ReplaceBtn);
  DDX_Control(pDX, IDC_DUPLICATECOUNT, m_DuplicateCount);
  DDX_Control(pDX, IDC_MARKEDONLY, m_MarkedOnlyBtn);
  DDX_Control(pDX, IDC_PAGECHECKS, m_PageChecks);
  DDX_Control(pDX, IDC_PAGECOUNT, m_PageCount);
  DDX_Control(pDX, IDC_PAGES, m_PageList);
  DDX_Control(pDX, IDC_SHOWALLTAGS, m_ShowAllBtn);
  DDX_Control(pDX, IDC_SHOWCLASSES, m_ShowClassesBtn);
  DDX_Control(pDX, IDC_SHOWPAGES, m_ShowPagesBtn);
  DDX_Control(pDX, IDC_TAGCHECKS, m_TagChecks);
  DDX_Control(pDX, IDC_TAGCOUNT, m_TagCount);
  DDX_Control(pDX, IDC_TAGFILTER, m_TagFilter);
  DDX_Control(pDX, IDC_TAGFILTERRULE, m_TagFilterRule);
  DDX_Control(pDX, IDC_TAGFIND, m_Find);
  DDX_Control(pDX, IDC_TAGLIST, m_TagList);
  DDX_Control(pDX, IDC_TAGREPLACE, m_Replace);
  DDX_Control(pDX, IDC_UNDOREPLACE, m_UndoReplaceBtn);
  DDX_Control(pDX, IDOK, m_DoneBtn);
  DDX_Control(pDX, IDC_STATICAPPLICATION, m_StaticApply);
  DDX_Control(pDX, IDC_STATICCLIP, m_StaticClip);
  DDX_Control(pDX, IDC_IMPORTCOUNT, m_ImportCount);
  }

BEGIN_MESSAGE_MAP(CBulkTagChange, CDialog)
  ON_BN_CLICKED(IDC_APPLY, OnBnClickedApply)
  ON_BN_CLICKED(IDC_CLASSCHECKS, OnBnClickedClasschecks)
  ON_BN_CLICKED(IDC_DOREPLACE, OnBnClickedDoreplace)
  ON_BN_CLICKED(IDC_MARKEDONLY, OnBnClickedMarkedonly)
  ON_BN_CLICKED(IDC_PAGECHECKS, OnBnClickedPagechecks)
  ON_BN_CLICKED(IDC_SHOWALLTAGS, OnBnClickedShowalltags)
  ON_BN_CLICKED(IDC_SHOWCLASSES, OnBnClickedShowclasses)
  ON_BN_CLICKED(IDC_SHOWPAGES, OnBnClickedShowpages)
  ON_BN_CLICKED(IDC_TAGCHECKS, OnBnClickedTagchecks)
  ON_BN_CLICKED(IDC_UNDOREPLACE, OnBnClickedUndoreplace)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_CBN_SELCHANGE(IDC_TAGFILTERRULE, OnCbnSelchangeTagfilterrule)
  ON_EN_CHANGE(IDC_TAGFILTER, OnEnChangeTagfilter)
  ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickTaglist)
  ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_TAGLIST, OnLvnBeginlabeleditTaglist)
  ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TAGLIST, OnLvnEndlabeleditTaglist)
  ON_NOTIFY(LVN_ITEMACTIVATE, IDC_TAGLIST, OnLvnItemActivateTaglist)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_CLASSIDS, OnLvnItemchangedClassids)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_PAGES, OnLvnItemchangedPages)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_TAGLIST, OnLvnItemchangedTaglist)
  ON_WM_GETMINMAXINFO()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_SIZE()
  ON_BN_CLICKED(IDC_FINDOPTIONS, OnBnClickedFindoptions)
  ON_BN_CLICKED(IDC_REPLACEOPTIONS, OnBnClickedReplaceoptions)
  ON_BN_CLICKED(IDC_FILTOPTIONS, OnBnClickedFiltoptions)
  ON_BN_CLICKED(IDC_EXPORT, OnBnClickedExport)
  ON_BN_CLICKED(IDC_IMPORT, OnBnClickedImport)
END_MESSAGE_MAP()


// CBulkTagChange message handlers
// From MSDN
BOOL CBulkTagChange::PreTranslateMessage(MSG *pMsg)
  {
  if(pMsg->message==WM_KEYDOWN && (pMsg->wParam==VK_RETURN || pMsg->wParam==VK_ESCAPE))
    {
    static const TCHAR szEdit[] = _T("edit");
    TCHAR szCompare[sizeof(szEdit)+1];
    HWND hFocus = ::GetFocus();

    // Check for "edit" controls which want the RETURN key.
    // If it doesn't want the return key then let it be
    // translated by just calling
    // CPropertyPage::PreTranslateMessage
    ::GetClassName(hFocus, szCompare, sizeof(szCompare));
    if (lstrcmpi(szCompare, szEdit) == 0)
      {
      if(::GetWindowLong(hFocus, GWL_STYLE) & ES_WANTRETURN)
        return FALSE;    // Don't translate...just send on
      // to control
      else
        return CDialog::PreTranslateMessage(pMsg);
      }

    // Check for other controls which want the return key
    if (::SendMessage(hFocus,WM_GETDLGCODE,0,0) &
      (DLGC_WANTALLKEYS | DLGC_WANTMESSAGE))
      return FALSE;        // Don't translate it
    };

  return CDialog::PreTranslateMessage(pMsg);
  } 

//--------------------------------------------------------------------------

static int HpTagTest(void * p, void * q)
  {
  CBTCItem * r1=(CBTCItem*)p;
  CBTCItem * r2=(CBTCItem*)q;
  CArray<int,int> &SO = r1->m_Dlg.m_SortOrder;
  int Cmp=0;
  for (int i=0; i<ColCount; i++)
    {
    int iCol=abs(SO[i]);
    if (iCol>=ColCount)
      break;
    int iDir=SO[i]>0?1:-1;
    switch (iCol)
      {
      case ColNumberClass   : Cmp=_stricmp(r1->m_pClassId->m_sClassId, r2->m_pClassId->m_sClassId); break;
      case ColNumberCurrent : Cmp=_stricmp(r1->m_sCurrent, r2->m_sCurrent); break;
      case ColNumberReplace : Cmp=_stricmp(r1->GetString(ColNumberReplace), r2->GetString(ColNumberReplace)); break;
      case ColNumberStatus  : Cmp=_stricmp(r1->m_sStatus, r2->m_sStatus); break;
      }
    Cmp*=iDir;
    if (Cmp)
      break;
    }
  return Cmp<0;
  }

static int HpClassTest(void * p, void * q)
  {
  CBTCClass * r1=(CBTCClass*)p;
  CBTCClass * r2=(CBTCClass*)q;
  int Cmp=_stricmp(r1->m_sClassId, r2->m_sClassId);
  return Cmp<0;
  }

static int HpPageTest(void * p, void * q)
  {
  CBTCPage * r1=(CBTCPage*)p;
  CBTCPage * r2=(CBTCPage*)q;
  int Cmp=_stricmp(r1->m_sPageId, r2->m_sPageId);
  return Cmp<0;
  }


void CBulkTagChange::BuildTags()
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
    m_ClassMap.InitHashTable(FindNextPrimeNumber((UINT)(100)));
    m_PageMap.InitHashTable(FindNextPrimeNumber((UINT)(100)));
    m_Monitor.InitHashTable(FindNextPrimeNumber((UINT)(NTags*1.18)));

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
        CBTCClass *pClass=new CBTCClass(this, ClassId());
        m_Classes.Add(pClass);
        m_ClassMap.SetAt(pClass->m_sClassId, pClass);
        }

      CBTCItem *pItem=new CBTCItem(this, pTag->Str(), m_Classes[iClass]);

      if (m_pMarkedTags)
        {
        POSITION Pos;
        pItem->m_Marked=m_MarkedTagMap.Lookup(pTag->Str(), Pos);
        }

      m_Tags.Add(pItem);
      m_TagMap.SetAt(pItem->m_sCurrent, pItem);
      MonitorAdd(pItem, pItem->SetReqd());
      }
    }
  // This info is no longer needed - and becomes incorrect after an apply.
  m_MarkedTagMap.RemoveAll();


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

          CBTCPage *pPage=new CBTCPage(this, GrfTitle);
          m_Pages.Add(pPage);
          m_PageMap.SetAt(pPage->m_sPageId, pPage);

          Strng_List GrfTagList;
          int NGrfTags = pGDoc->GetTagList(GrfTagList);
          for (Strng * pT=GrfTagList.First(); pT; pT=GrfTagList.Next())
            {

            CBTCItem *pItem;
            if (m_TagMap.Lookup(pT->Str(), pItem))
              {
              pItem->m_Pages.Add(pPage);
              if (pItem->m_sPages.GetLength()>0)
                pItem->m_sPages+=",";
              pItem->m_sPages+=pPage->m_sPageId;

              }
            }
          }
        }
      }
    }

  HpSort(m_Tags.GetCount(), (void**)&m_Tags[0], HpTagTest);
  HpSort(m_Classes.GetCount(), (void**)&m_Classes[0], HpClassTest);
  HpSort(m_Pages.GetCount(), (void**)&m_Pages[0], HpPageTest);
  }

//--------------------------------------------------------------------------

void CBulkTagChange::RebuildTags()
  {
  //ClearTags();

  m_TagMap.RemoveAll();
  m_Monitor.RemoveAll();

  for (int i=0; i<m_Tags.GetCount(); i++)
    {
    CBTCItem * pItem=m_Tags[i];

    m_TagMap.SetAt(pItem->m_sCurrent, pItem);
    MonitorAdd(pItem, pItem->SetReqd());
    }


  HpSort(m_Tags.GetCount(), (void**)&m_Tags[0], HpTagTest);
  HpSort(m_Classes.GetCount(), (void**)&m_Classes[0], HpClassTest);
  HpSort(m_Pages.GetCount(), (void**)&m_Pages[0], HpPageTest);
  }

//--------------------------------------------------------------------------

int CBulkTagChange::MonitorAdd(CBTCItem * pItem, BOOL SetRqd)
  {
  LPCTSTR pTag;
  if (SetRqd)//pItem->SetReqd())
    pTag=pItem->m_sReplacement;
  else
    pTag=pItem->m_sCurrent;

  CBTCMonitor * p;
  if (m_Monitor.Lookup(pTag, p))
    {
    p->m_Count++;
    p->m_Items.AddTail(pItem);
    }
  else
    {
    p = new CBTCMonitor;
    p->m_sTag=pTag;//pItem->m_sReplacement;
    p->m_Count=1;
    p->m_Items.AddTail(pItem);
    m_Monitor.SetAt(p->m_sTag, p);
    }

  UpdateDuplicateCounts(p);
  return p->m_Count;
  };

int CBulkTagChange::MonitorRemove(CBTCItem * pItem, BOOL SetRqd)
  {
  LPCTSTR pTag;
  if (SetRqd)//pItem->SetReqd())
    pTag=pItem->m_sReplacement;
  else
    pTag=pItem->m_sCurrent;

  int Ret=-1;
  CBTCMonitor * p;
  if (m_Monitor.Lookup(pTag, p))
    {
    p->m_Count--;
    if (p->m_Count==0)
      {
      m_Monitor.RemoveKey(pTag);
      UpdateDuplicateCounts(p);
      delete p;
      Ret=0;
      }
    else
      {
      POSITION Pos=p->m_Items.Find(pItem);
      if (Pos)
        p->m_Items.RemoveAt(Pos);
      else
        ASSERT_ALWAYS(FALSE, "CBTCMonitor:Item not in list", __FILE__, __LINE__);
      Ret=p->m_Count;
      UpdateDuplicateCounts(p);
      }
    }
  else
    ASSERT_ALWAYS(FALSE, "CBTCMonitor:Item not found", __FILE__, __LINE__);
  return Ret;
  };

//--------------------------------------------------------------------------

void CBulkTagChange::UpdateDuplicateCounts(CBTCMonitor * p)
  {
  POSITION Pos=p->m_Items.GetHeadPosition();
  while (Pos)
    {
    CBTCItem * pDup=p->m_Items.GetNext(Pos);
    pDup->m_nDuplicateCount=p->m_Count-1;
    if (pDup->m_DisplayIndex>=0)
      m_TagList.Update(pDup->m_DisplayIndex);
    }
  }

//--------------------------------------------------------------------------

void CBulkTagChange::SetFilter()
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
          CBTCItem & Tg=*m_Tags[i];
          BOOL Selected;
          if (m_bCaseSens)
            Selected=strstr(Tg.m_sCurrent, FilterString)!=NULL;
          else
            Selected=strstr(Tg.m_sCurrentLwr, FilterString)!=NULL;
          Tg.ChangeSetReqd(CBTCItem::eSRSelected, Selected);
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
          CBTCItem & Tg=*m_Tags[i];
          try
            {
            Tg.ChangeSetReqd(CBTCItem::eSRSelected, (*m_pFilterRE)->Test(_bstr_t(Tg.m_sCurrent))==VARIANT_TRUE);
            }
          catch(...)
            {
            //ReportError("InFilter", 0, "Failure in Regular expressions");
            LogError("TagChangeFilter", 0, "Failure in Regular expressions");
            Tg.ChangeSetReqd(CBTCItem::eSRSelected, true);
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
      CBTCItem & Tg=*m_Tags[i];
      Tg.ChangeSetReqd(CBTCItem::eSRSelected, true);
      }
    };
  }

//--------------------------------------------------------------------------

void CBulkTagChange::ClearTags()
  {
  m_TagMap.RemoveAll();
  m_ClassMap.RemoveAll();
  m_PageMap.RemoveAll();
  m_Monitor.RemoveAll();

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

void CBulkTagChange::LoadTagList()
  {
  m_Loading=true;
  LockWindowUpdate();

  int Index=0;
  for (int i=0; i<m_Tags.GetCount(); i++)
    {
    CBTCItem &Tg=*m_Tags[i];
    if (Tg.ShowIt())
      {
      int OldData;
      if (Index>=m_TagList.GetItemCount())
        {
        int IndexChk=m_TagList.InsertItem(Index, LPSTR_TEXTCALLBACK);
        ASSERT_ALWAYS(IndexChk==Index, "Bad TagList Insert", __FILE__, __LINE__);
        m_TagList.SetItem(Index, 1, LVIF_TEXT, LPSTR_TEXTCALLBACK,0,0,0,0);
        m_TagList.SetItem(Index, 2, LVIF_TEXT, LPSTR_TEXTCALLBACK,0,0,0,0);
        m_TagList.SetItem(Index, 3, LVIF_TEXT, LPSTR_TEXTCALLBACK,0,0,0,0);
        OldData=i;
        }
      else
        {
        OldData=-1;
        }
      m_TagList.SetCheck(Index, Tg.m_Checked);// && Tg.SetReqd(true));
      m_TagList.SetItemData(Index, i);
      Tg.m_DisplayIndex=Index;
      if (OldData!=i)
        m_TagList.Update(Index);

      Index++;
      }
    else
      Tg.m_DisplayIndex=-1;
    }
  while (m_TagList.GetItemCount()>Index)
    m_TagList.DeleteItem(Index);

  UnlockWindowUpdate();
  m_Loading=false;


  SetTagCheckCtrls();
  }

void CBulkTagChange::LoadPageList()
  {
  m_Loading=true;
  LockWindowUpdate();

  m_PageList.DeleteAllItems(); 
  for (int i=0; i<m_Pages.GetCount(); i++)
    {
    m_PageList.InsertItem(m_PageList.GetItemCount(), m_Pages[i]->m_sPageId);
    m_PageList.SetCheck(i, m_Pages[i]->m_Checked);
    }
  UnlockWindowUpdate();
  m_Loading=false;

  SetPageCheckCtrls();
  }

void CBulkTagChange::LoadClassList()
  {
  m_Loading=true;
  LockWindowUpdate();

  m_ClassList.DeleteAllItems(); 
  for (int i=0; i<m_Classes.GetCount(); i++)
    {
    m_ClassList.InsertItem(m_ClassList.GetItemCount(), m_Classes[i]->m_sClassId);
    m_ClassList.SetCheck(i, m_Classes[i]->m_Checked);
    }

  UnlockWindowUpdate();
  m_Loading=false;

  SetClassCheckCtrls();
  }

//--------------------------------------------------------------------------

void CBulkTagChange::UpdateChangeCounts()
  {
  m_nChangeCount=0;
  m_nAdjustCount=0;
  m_nDuplicateCount=0;

  int N=0;
  for (int i=0; i<m_Tags.GetCount(); i++)
    {
    CBTCItem &Tg=*m_Tags[i];
    if (Tg.m_Checked)
      N++;
    if (Tg.m_Changed)
      m_nChangeCount++;
    if (Tg.m_AdjustCnt)
      m_nAdjustCount++;
    if (Tg.m_nDuplicateCount)
      m_nDuplicateCount++;
    }

  CString S;
  S.Format("%i", m_nChangeCount);
  m_ChangeCount.SetWindowText(S);
  S.Format("%i", m_nAdjustCount);
  m_AdjustCount.SetWindowText(S);
  S.Format("%i", m_nDuplicateCount);
  m_DuplicateCount.SetWindowText(S);
  S.Format("%i", m_nImportCount);
  m_ImportCount.SetWindowText(S);
  S.Format("%i", m_nApplyCount);
  m_ApplyCount.SetWindowText(S);

  m_ApplyBtn.EnableWindow(m_nChangeCount>0 && m_nDuplicateCount==0 ? 1:0);
  m_ReplaceBtn.EnableWindow(1);
  m_UndoReplaceBtn.EnableWindow(m_ReplaceUndoCnt>0);
  }

void CBulkTagChange::SetTagCheckCtrls()
  {
  int N=0;
  for (int i=0; i<m_Tags.GetCount(); i++)
    {
    if (m_Tags[i]->m_Checked)
      N++;
    }

  CString S;
  S.Format("%i of %i", N, m_Tags.GetCount());
  m_TagCount.SetWindowText(S);

  if (N==m_Tags.GetCount())
    m_TagChecks.SetCheck(BST_CHECKED);
  else if (N==0)
    m_TagChecks.SetCheck(BST_UNCHECKED);
  else
    m_TagChecks.SetCheck(BST_INDETERMINATE);
  }

void CBulkTagChange::SetPageCheckCtrls()
  {
  int N=0;
  for (int i=0; i<m_Pages.GetCount(); i++)
    {
    if (m_Pages[i]->m_Checked)
      N++;
    }

  CString S;
  S.Format("%i of %i", N, m_Pages.GetCount());
  m_PageCount.SetWindowText(S);

  if (N==m_Pages.GetCount())
    m_PageChecks.SetCheck(BST_CHECKED);
  else if (N==0)
    m_PageChecks.SetCheck(BST_UNCHECKED);
  else
    m_PageChecks.SetCheck(BST_INDETERMINATE);
  }

void CBulkTagChange::SetClassCheckCtrls()
  {
  int N=0;
  for (int i=0; i<m_Classes.GetCount(); i++)
    {
    if (m_Classes[i]->m_Checked)
      N++;
    }

  CString S;
  S.Format("%i of %i", N, m_Classes.GetCount());
  m_ClassCount.SetWindowText(S);

  if (N==m_Classes.GetCount())
    m_ClassChecks.SetCheck(BST_CHECKED);
  else if (N==0)
    m_ClassChecks.SetCheck(BST_UNCHECKED);
  else
    m_ClassChecks.SetCheck(BST_INDETERMINATE);
  }

//--------------------------------------------------------------------------

static int CtrlIDIndex(int ID) { int i=0; while (CtrlIDs[i].m_ID!=ID) i++; return i; } ;

BOOL CBulkTagChange::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  CProfINIFile PF(PrjIniFile());
  CString S;
  //SetDBNameSymb(PF.RdStr("NeutralGroup", "Database", "$CfgFiles\\GroupLibrary.MDB"));
  //m_editDatabase.SetWindowText(m_sDatabaseSymb());
  //m_pInPlaceEdit=NULL;
  m_nChangeCount=0;
  m_nAdjustCount=0;
  m_nApplyCount=0;
  m_nDuplicateCount=0;
  m_nImportCount=0;

  m_TagFilterRule.AddString(eFilterRuleNames[eFROff]);
  m_TagFilterRule.AddString(eFilterRuleNames[eFRContains]);
  m_TagFilterRule.AddString(eFilterRuleNames[eFRWildCard]);
  m_TagFilterRule.AddString(eFilterRuleNames[eFRRegExp]);

  S=PF.RdStr("BulkTagChange", "TagFilterRule", eFilterRuleNames[eFRContains]);
  if (m_TagFilterRule.SelectString(-1, S)<0)
    m_TagFilterRule.SelectString(-1, eFilterRuleNames[eFRContains]);

  m_TagFilter.SetWindowText(PF.RdStr("BulkTagChange", "TagFilter", ""));

  m_ShowAllBtn.SetCheck(PF.RdLong("BulkTagChange", "ShowAll", 0));
  if (m_pMarkedTags)
    m_MarkedOnlyBtn.SetCheck(PF.RdLong("BulkTagChange", "MarkedOnly", 1));
  else
    {
    m_MarkedOnlyBtn.SetCheck(0);
    m_MarkedOnlyBtn.EnableWindow(0);
    }

  m_Find.SetWindowText(PF.RdStr("BulkTagChange", "FindString", "^"));
  m_Replace.SetWindowText(PF.RdStr("BulkTagChange", "ReplaceString", ""));
  m_bCaseSens=PF.RdInt("BulkTagChange", "CaseSensitive", 0)!=0;
  m_bCaseSens=false; // force it

  m_ShowAllTags=m_ShowAllBtn.GetCheck()!=0;
  m_MarkedOnly=m_MarkedOnlyBtn.GetCheck()!=0;

  //int ColLen[ColCount];
  m_TagList.SetExtendedStyle(m_TagList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);

  for (int i=0; i<ColCount; i++)
    {
    CString S;
    S.Format("ColWidth.%s", ColHeadName[i]);
    LPCTSTR Nm=(i==ColNumberMark) ? (m_pMarkedTags?"    ":"  "): ColHeadName[i];
    int hw=m_TagList.GetStringWidth(Nm)+15;
    int cw=PF.RdInt("BulkTagChange", S, ColWide[i]);
    switch (i)
      {
      case ColNumberMark:   cw=hw;                              break;  
      case ColNumberClass:  m_ClassColWidth=cw; cw=Max(0, cw);  break;  
      case ColNumberPage:   m_PageColWidth=cw; cw=Max(0, cw);   break;  
      default:              cw=Max(cw, hw);                     break;  
      }

    m_TagList.InsertColumn(i, ColHeadTxt[i], LVCFMT_LEFT, cw);
    m_TagList.SetColumnWidth(i, cw);
    }

  int SBWide=GetSystemMetrics(SM_CXVSCROLL);
  CRect Rct;
  m_ClassList.GetClientRect(&Rct);
  m_ClassList.InsertColumn(0, "ClassIds", LVCFMT_LEFT, Rct.Width()-SBWide);
  m_ClassList.SetExtendedStyle(m_ClassList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

  m_PageList.GetClientRect(&Rct);
  m_PageList.InsertColumn(0, "Pages", LVCFMT_LEFT, Rct.Width()-SBWide);
  m_PageList.SetExtendedStyle(m_PageList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

  int TagColWide=120;
  m_ApplyLog.GetClientRect(&Rct);
  m_ApplyLog.InsertColumn(0, "Note", LVCFMT_LEFT, Rct.Width()-SBWide);
  m_ApplyLog.SetExtendedStyle(m_ApplyLog.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
  m_ApplyProgress.SetRange(0, 100);
  m_ApplyProgress.SetPos(0);

  m_ApplyBtn.EnableWindow(0);

  m_ChangeCount.SetWindowText("0");
  m_AdjustCount.SetWindowText("0");
  m_DuplicateCount.SetWindowText("0");
  m_ImportCount.SetWindowText("0");
  m_ApplyCount.SetWindowText("0");

  GetClientRect(&m_ClientRctInit);
  m_CtrlRcts.SetSize(CtrlIDCount);
  for (int i=0; i<CtrlIDCount; i++)
    {
    GetDlgItem(CtrlIDs[i].m_ID)->GetWindowRect(&m_CtrlRcts[i]);
    ScreenToClient(&m_CtrlRcts[i]);
    }
  m_SepInit[0]=(int)(0.5*(m_CtrlRcts[CtrlIDIndex(IDC_TAGLIST)].right+m_CtrlRcts[CtrlIDIndex(IDC_CLASSIDS)].left));
  m_SepInit[1]=(int)(0.5*(m_CtrlRcts[CtrlIDIndex(IDC_CLASSIDS)].right+m_CtrlRcts[CtrlIDIndex(IDC_PAGES)].left));
  m_SepInit[2]=(int)(0.5*(m_CtrlRcts[CtrlIDIndex(IDC_STATICCLIP)].right+m_CtrlRcts[CtrlIDIndex(IDC_STATICAPPLICATION)].left));

  for (int i=0; i<3; i++)
    m_SepPos[i]=(1000*m_SepInit[i])/m_ClientRctInit.Width();

  m_InPlaceEditItem=-1;

  i=0;
  m_SortOrder.SetSize(ColCount+1);
  m_SortOrder[i++]=2; // Tag Ascend
  m_SortOrder[i++]=1; // Class Ascend
  for ( ; i<m_SortOrder.GetSize(); i++)
    m_SortOrder[i]=ColCount; // Rest off

  BuildTags();

  for (int i=0; i<m_Classes.GetCount(); i++)
    {                                                   
    CBTCClass &Cl=*m_Classes[i];
    CString S;
    S.Format("Class.%s", Cl.m_sClassId);
    Cl.m_Checked = PF.RdInt("BulkTagChange", S, Cl.m_Checked?1:0)!=0;
    }

  for (int i=0; i<m_Pages.GetCount(); i++)
    {
    CBTCPage &Pg=*m_Pages[i];
    CString S;
    S.Format("Page.%s", Pg.m_sPageId);
    Pg.m_Checked = PF.RdInt("BulkTagChange", S, Pg.m_Checked?1:0)!=0;
    }


  SetFilter();

  LoadTagList();
  LoadPageList();
  LoadClassList();

  ShowSelectedColumns();
  //DoChecks();

  m_Inited=true;

  GetClientRect(&Rct);
  ClientToScreen(&Rct);
  //ClientToScreen(&Rct);
  m_SepPos[0]= PF.RdInt("BulkTagChange", "Separator1", m_SepPos[0]);
  m_SepPos[1]= PF.RdInt("BulkTagChange", "Separator2", m_SepPos[1]);
  m_SepPos[2]= PF.RdInt("BulkTagChange", "Separator3", m_SepPos[2]);
  int xPos = PF.RdInt("BulkTagChange", "XPos", -10000);//Rct.left);
  int yPos = PF.RdInt("BulkTagChange", "YPos", Rct.top);
  int CW = PF.RdInt("BulkTagChange", "Width", Rct.Width());
  int CH = PF.RdInt("BulkTagChange", "Height", Rct.Height());

  if (xPos>-10000)
    {
    SetWindowPos(NULL, 0,0, 1,1 , SWP_NOREPOSITION | SWP_NOZORDER | SWP_SHOWWINDOW); // set size small to force an OnSize
    SetVisibleWindowPos(this, xPos, yPos, CW, CH, true);
    //PositionControls(CW, CH);
    }
  else
    CenterWindow();

  ShowWindow(SW_SHOW);
  UpdateDialogControls(this, FALSE);

  return TRUE;
  }


BOOL CBulkTagChange::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
  {
  HD_NOTIFY* pNM = (HD_NOTIFY*)lParam;
  switch (pNM->hdr.code)
    {
    case LVN_GETDISPINFO:
      {
      NMLVDISPINFO * pnmv = (NMLVDISPINFO*)lParam;
      if (pNM->hdr.idFrom==IDC_TAGLIST && pnmv->item.lParam<m_Tags.GetSize())
        {
        CBTCItem &TI=*(m_Tags[pnmv->item.lParam]);
        switch (pnmv->item.iSubItem)
          {
          case ColNumberMark:
          case ColNumberClass:
          case ColNumberCurrent:
          case ColNumberReplace:
          case ColNumberStatus:
          case ColNumberPage:
            pnmv->item.pszText=(LPSTR)(LPCTSTR)TI.GetString(pnmv->item.iSubItem);
            break;
            //case 4:
            //  pnmv->item.pszText=TRI.sSetCnt();
            //  break;
          }
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

void CBulkTagChange::SaveAllSetReqd()
  {
  for (int i=0; i<m_Tags.GetCount(); i++)
    m_Tags[i]->SaveSetReqd();
  }

void CBulkTagChange::TestAllSetReqd()
  {
  for (int i=0; i<m_Tags.GetCount(); i++)
    m_Tags[i]->TestSetReqd();
  }
//--------------------------------------------------------------------------

void CBulkTagChange::ShowSelectedColumns()
  {
  m_TagList.SetColumnWidth(ColNumberClass, Max(0, m_ClassColWidth));
  m_TagList.SetColumnWidth(ColNumberPage, Max(0, m_PageColWidth));
  m_ShowClassesBtn.SetCheck(m_ClassColWidth>=0 ? 1:0);
  m_ShowPagesBtn.SetCheck(m_PageColWidth>=0 ? 1:0);
  };

//--------------------------------------------------------------------------

void CBulkTagChange::OnOK() 
  {
  UpdateData(TRUE);

  if (m_nChangeCount>0)
    {
    AddApplyLog("%s", "Outstanding Changes");
    return;
    }
  if (m_InPlaceEditItem>=0)  // stop falling out if edit busy
    return;

  SaveProfile();

  CDialog::OnOK();
  }

//--------------------------------------------------------------------------

void CBulkTagChange::OnCancel()
  {
  SaveProfile();

  CDialog::OnCancel();
  }

//--------------------------------------------------------------------------

void CBulkTagChange::SaveProfile()
  {
  CProfINIFile PF(PrjIniFile());
  CString S;

  CRect Rct;
  GetWindowRect(&Rct);

  PF.WrInt("BulkTagChange", "Separator1", m_SepPos[0]);
  PF.WrInt("BulkTagChange", "Separator2", m_SepPos[1]);
  PF.WrInt("BulkTagChange", "Separator3", m_SepPos[2]);
  PF.WrInt("BulkTagChange", "XPos", Rct.left);
  PF.WrInt("BulkTagChange", "YPos", Rct.top);
  PF.WrInt("BulkTagChange", "Width", Rct.Width());
  PF.WrInt("BulkTagChange", "Height", Rct.Height());

  for (int i=0; i<ColCount; i++)
    {
    CString S;
    S.Format("ColWidth.%s", ColHeadName[i]);

    int w=m_TagList.GetColumnWidth(i);
    switch (i)
      {
      case ColNumberClass:  w = w>0 ? w : m_ClassColWidth; break;  
      case ColNumberPage:   w = w>0 ? w : m_PageColWidth;  break;  
        //default:              w;  break;
      }

    PF.WrInt("BulkTagChange", S, w);
    }

  m_TagFilterRule.GetWindowText(S);
  PF.WrStr("BulkTagChange", "TagFilterRule", S);
  m_TagFilter.GetWindowText(S);
  PF.WrStr("BulkTagChange", "TagFilter", S);

  //PF.WrStr("NeutralGroup", "Database", m_sDatabaseSymb);
  PF.WrLong("BulkTagChange", "ShowAll", m_ShowAllBtn.GetCheck());
  if (m_pMarkedTags)
    PF.WrLong("BulkTagChange", "MarkedOnly", m_MarkedOnlyBtn.GetCheck());


  CString FindStr, ReplaceStr;
  m_Find.GetWindowText(FindStr);
  m_Replace.GetWindowText(ReplaceStr);
  PF.WrStr("BulkTagChange", "FindString", FindStr);
  PF.WrStr("BulkTagChange", "ReplaceString", ReplaceStr);
  PF.WrInt("BulkTagChange", "CaseSensitive", m_bCaseSens?1:0);

  for (int i=0; i<m_Classes.GetCount(); i++)
    {
    CBTCClass &Cl=*m_Classes[i];
    CString S;
    S.Format("Class.%s", Cl.m_sClassId);
    PF.WrInt("BulkTagChange", S, Cl.m_Checked?1:0);
    }

  for (int i=0; i<m_Pages.GetCount(); i++)
    {
    CBTCPage &Pg=*m_Pages[i];
    CString S;
    S.Format("Page.%s", Pg.m_sPageId);
    PF.WrInt("BulkTagChange", S, Pg.m_Checked?1:0);
    }
  }

//--------------------------------------------------------------------------

void CBulkTagChange::OnBnClickedOk()
  {
  OnOK();
  }

//--------------------------------------------------------------------------

void CBulkTagChange::OnBnClickedFiltoptions()
  {
  eFilterRule Rule=(eFilterRule)m_TagFilterRule.GetCurSel();
  if (Rule>eFRContains)
    {
    CString FndTxt;
    m_TagFilter.GetWindowText(FndTxt);
    REMenuOptions(this, FndTxt, Rule, true);
    m_TagFilter.SetWindowText(FndTxt);
    }
  }
//--------------------------------------------------------------------------

void CBulkTagChange::OnBnClickedFindoptions()
  {
  CString FndTxt;
  m_Find.GetWindowText(FndTxt);
  REMenuOptions(this, FndTxt, eFRRegExp, true);
  m_Find.SetWindowText(FndTxt);
  }

//--------------------------------------------------------------------------

void CBulkTagChange::OnBnClickedReplaceoptions()
  {
  CString RepTxt;
  m_Replace.GetWindowText(RepTxt);
  REMenuOptions(this, RepTxt, eFRRegExp, false);
  m_Replace.SetWindowText(RepTxt);
  }

//--------------------------------------------------------------------------

void CBulkTagChange::OnBnClickedDoreplace()
  {
  m_ApplyProgress.SetPos(0);

  m_TagList.LockWindowUpdate();

  IRegExpPtr *pRE= new IRegExpPtr("VBScript.RegExp");
  CString FndTxt;
  CString RepTxt;
  m_Find.GetWindowText(FndTxt);
  m_Replace.GetWindowText(RepTxt);

  _bstr_t bFndTxt(FndTxt);
  _bstr_t bRepTxt(RepTxt);

  // A Test that a replace wont throw an exception
  try
    {
    _bstr_t NewTag("SomeTag");

    (*pRE)->Pattern = bFndTxt;
    NewTag=(*pRE)->Replace(NewTag, bRepTxt);
    }
  catch(_com_error)// & e)
    {
    AfxMessageBox("Invalid Find and/or Replacement Specification", MB_ICONEXCLAMATION);
    goto Done;
    }

  dbgpln("==========================");

  for (int i=0; i<m_Tags.GetCount(); i++)
    {
    CBTCItem &Tg=*m_Tags[i];
    if (Tg.SetReqd())
      {
      _bstr_t NewTag(Tg.m_sReplacement);

      (*pRE)->Pattern = bFndTxt;
      NewTag=(*pRE)->Replace(NewTag, bRepTxt);

      Tg.Replace((LPCTSTR)NewTag);

      if (Tg.m_DisplayIndex>=0)
        m_TagList.Update(Tg.m_DisplayIndex);
      }                                                    
    }
  m_ReplaceUndoCnt++;

  DoChecks();

Done:
  delete pRE;
  m_TagList.UnlockWindowUpdate();

  UpdateChangeCounts();
  }

void CBulkTagChange::OnBnClickedUndoreplace()
  {
  m_TagList.LockWindowUpdate();
  dbgpln("==========================");
  m_ReplaceUndoCnt--;
  for (int i=0; i<m_Tags.GetCount(); i++)
    {
    CBTCItem &Tg=*m_Tags[i];
    //if (Tg.SetReqd())
      {
      Tg.Undo();
      if (Tg.m_DisplayIndex>=0)
        m_TagList.Update(Tg.m_DisplayIndex);
      }
    }
  m_TagList.UnlockWindowUpdate();
  UpdateChangeCounts();
  }

//--------------------------------------------------------------------------

bool CBulkTagChange::DoChecks()
  {
  bool OK=true;

  return OK;
  }

//--------------------------------------------------------------------------

void CBulkTagChange::OnBnClickedShowalltags()
  {
  m_ShowAllTags=m_ShowAllBtn.GetCheck();
  LoadTagList();
  }

void CBulkTagChange::OnBnClickedMarkedonly()
  {
  m_MarkedOnly=m_MarkedOnlyBtn.GetCheck();
  LoadTagList();
  }


void CBulkTagChange::OnLvnItemchangedTaglist(NMHDR *pNMHDR, LRESULT *pResult)
  {
  if (!m_Loading)
    {
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    if (pNMLV->uChanged & LVIF_STATE)
      {
      int i=pNMLV->iItem;

      CBTCItem *pItem=m_Tags[TagIndex(i)];

      BOOL Chk=pItem->m_Checked;

      pItem->ChangeSetReqd(CBTCItem::eSRCheck, m_TagList.GetCheck(i));

      if (Chk!=pItem->m_Checked)
        {
        if (m_ShowAllTags)
          m_TagList.Update(i);
        else
          LoadTagList();
        }
      }
    }

  *pResult = 0;
  }

void CBulkTagChange::OnLvnItemchangedClassids(NMHDR *pNMHDR, LRESULT *pResult)
  {
  if (!m_Loading)
    {
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    if (pNMLV->uChanged & LVIF_STATE)
      {
      SaveAllSetReqd();
      int i=pNMLV->iItem;
      CBTCClass & Itm=*m_Classes[i];
      BOOL Chk=Itm.m_Checked;
      Itm.m_Checked=m_ClassList.GetCheck(i);
      SetClassCheckCtrls();
      TestAllSetReqd();
      if (Chk!=Itm.m_Checked)
        LoadTagList();
      }
    }
  *pResult = 0;
  }

void CBulkTagChange::OnLvnItemchangedPages(NMHDR *pNMHDR, LRESULT *pResult)
  {
  if (!m_Loading)
    {
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    if (pNMLV->uChanged & LVIF_STATE)
      {
      SaveAllSetReqd();
      int i=pNMLV->iItem;
      CBTCPage & Itm=*m_Pages[i];
      BOOL Chk=Itm.m_Checked;
      Itm.m_Checked=m_PageList.GetCheck(i);
      SetPageCheckCtrls();
      TestAllSetReqd();
      if (Chk!=Itm.m_Checked)
        LoadTagList();
      }
    }
  *pResult = 0;
  }    

void CBulkTagChange::OnBnClickedTagchecks()
  {
  int Chk=m_TagChecks.GetCheck();
  switch (Chk)
    {
    case BST_CHECKED:
      {
      for (int i=0; i<m_Tags.GetCount(); i++)
        m_Tags[i]->m_Checked=false;
      break;
      }
    case BST_UNCHECKED:
      {
      for (int i=0; i<m_Tags.GetCount(); i++)
        m_Tags[i]->m_Checked=true;
      break;
      }
    case BST_INDETERMINATE: 
      {
      for (int i=0; i<m_Tags.GetCount(); i++)
        m_Tags[i]->m_Checked=false;
      break;
      }
    }
  LoadTagList();
  }

void CBulkTagChange::OnBnClickedClasschecks()
  {
  int Chk=m_ClassChecks.GetCheck();
  switch (Chk)
    {
    case BST_CHECKED:
      {
      for (int i=0; i<m_Classes.GetCount(); i++)
        m_Classes[i]->m_Checked=false;
      break;
      }
    case BST_UNCHECKED:
    case BST_INDETERMINATE: 
      {
      for (int i=0; i<m_Classes.GetCount(); i++)
        m_Classes[i]->m_Checked=true;
      break;
      }
    }
  LoadClassList();
  LoadTagList();
  }

void CBulkTagChange::OnBnClickedPagechecks()
  {
  int Chk=m_PageChecks.GetCheck();
  switch (Chk)
    {
    case BST_CHECKED:
      {
      for (int i=0; i<m_Pages.GetCount(); i++)
        m_Pages[i]->m_Checked=false;
      break;
      }
    case BST_UNCHECKED:
    case BST_INDETERMINATE: 
      {
      for (int i=0; i<m_Pages.GetCount(); i++)
        m_Pages[i]->m_Checked=true;
      break;
      }
    }
  LoadPageList();
  LoadTagList();
  }

void CBulkTagChange::OnEnChangeTagfilter()
  {
  SetFilter();
  LoadTagList();
  }

void CBulkTagChange::OnCbnSelchangeTagfilterrule()
  {
  SetFilter();
  LoadTagList();
  }

//--------------------------------------------------------------------------

void CBulkTagChange::OnBnClickedApply()
  {
  gs_AccessWnds.CloseAccessData(-1, False, False, True);
  gs_AccessWnds.CloseWnd(-1);

  int NChanges=0;
  int NClashes=0;
  int Done=0;

  for (int i=0; i<m_Tags.GetCount(); i++)
    {
    CBTCItem &Tg=*m_Tags[i];
    Tg.m_UseInter=false;
    Tg.m_SetReqd=Tg.SetReqd() && Tg.m_Changed;
    if (Tg.m_SetReqd)
      NChanges++;
    }
  if (NChanges>0)
    {
    // Check for clashes
    for (int i=0; i<m_Tags.GetCount(); i++)
      {
      CBTCItem &Tg=*m_Tags[i];
      CBTCItem * pOtherTg;
      if (Tg.m_SetReqd && m_TagMap.Lookup(Tg.m_sReplacement, pOtherTg))
        { // tag currently exists (Other unit?)
        NClashes++;
        ASSERT_ALWAYS(pOtherTg->m_SetReqd, "Duplicate tag not marked for change", __FILE__, __LINE__);

        // Keep appending "Q" to Intermediate tag until it exists in
        // neither 'Old' list or 'New' List
        Tg.m_sIntermediate=Tg.m_sReplacement;
        while (1)
          {
          Tg.m_sIntermediate+="Q";
          Tg.m_UseInter=true;

          CBTCItem  *p1;
          CBTCMonitor *p2;
          if (!m_TagMap.Lookup(Tg.m_sIntermediate, p1) && 
            !m_Monitor.Lookup(Tg.m_sIntermediate, p2))
            break;
          }
        AddApplyLog("Intermediate Tag %s -> %s -> %s", Tg.m_sCurrent, Tg.m_sIntermediate, Tg.m_sReplacement);
        }
      }


    if (NClashes>0)
      {
      Done=0;
      AddApplyLog("Preliminary Pass: %i Tags to change", NClashes);
      m_ApplyProgress.SetPos(0);
      m_ApplyProgress.SetRange(0, NClashes);
      for (int i=0; i<m_Tags.GetCount(); i++)
        {
        CBTCItem &Tg=*m_Tags[i];
        if (Tg.m_UseInter && (SetATag(Tg.m_sCurrent, Tg.m_sIntermediate)==EOCT_DONE))
          m_ApplyProgress.SetPos(++Done);
        }
      }

    Done=0;
    m_ApplyProgress.SetPos(0);
    m_ApplyProgress.SetRange(0, NChanges);
    AddApplyLog("Primary Pass: %i Tags to change", NChanges);

    for (int i=0; i<m_Tags.GetCount(); i++)
      {
      CBTCItem &Tg=*m_Tags[i];
      if (Tg.m_SetReqd && (SetATag(Tg.m_UseInter ? Tg.m_sIntermediate : Tg.m_sCurrent, Tg.m_sReplacement)==EOCT_DONE))
        {
        Tg.m_SetReqd=false;
        m_ApplyProgress.SetPos(++Done);
        if (Tg.m_DisplayIndex>=0)
          m_TagList.Update(Tg.m_DisplayIndex);
        Tg.TagChanged();
        m_nApplyCount++;
        }
      }

    AddApplyLog("Done");
    }


  RebuildTags();

  UpdateChangeCounts();
  LoadTagList();
  
  CExploreScd::RefreshIt(true); 
  CExploreScd::ChkRefreshIt(); 
  }

int CBulkTagChange::SetATag(LPCTSTR OldTag, LPCTSTR NewTag)
  {
  //BOOL ok = TRUE;
  //if (TaggedObject::ValidateTagChanged(NewTag))
  //  {
  //  Strng s;
  //  s.Set("New tag '%s' is illegal and has been altered to '%s'\nContinue?", (LPCTSTR)Tg.m_sCurrent, NewTag);
  //  //ok = (AfxMessageBox(s(), MB_YESNO|MB_ICONQUESTION)==IDYES);
  //  //if (ok)
  //  LogNote("ChangeTag", 0, "New tag '%s' altered to '%s'", (LPCTSTR)m_NewTag, NewTag);
  //  }
  //if (ok && !gs_pPrj->AllGrfLoaded())
  //  {
  //  Strng s;
  //  s.Set("All the graphics pages have not been loaded!\nTag '%s' may not be changed in the graphics window.\nContinue?", OldTag);
  //  ok = (AfxMessageBox(s(), MB_YESNO|MB_ICONEXCLAMATION|MB_DEFBUTTON2)==IDYES);
  //  }
  //if (ok)
  //  {
  //pStatusBar->SetMsg("Changing tag %s to %s", OldTag, NewTag);
  int Err = gs_Exec.ChangeTag((LPTSTR)OldTag, (LPTSTR)NewTag);
  Strng Txt;
  switch (Err)
    {
    case EOCT_NOTFOUND:
      Txt.Set("Tag %s not found", OldTag);
      break;
    case EOCT_DUPLICATE:
      Txt.Set("Duplicate Tag %s", NewTag);
      break;
    case EOCT_FAILED:
      Txt.Set("FAILURE %s", OldTag);
      break;
    case EOCT_NULLTAG:
      Txt.Set("Valid (not blank) tags must be specified");
      break;
    case EOCT_INVALIDTAG:
      Txt.Set("Valid (all characters must be legal) tags must be specified");
      break;
    case EOCT_BADTAGLEN:
      Txt.Set("New tag length is too long");
      break;
    case EOCT_NOTDIFFER:
      Txt.Set("Tags should differ");
      break;
    case EOCT_NOTALLOWED:
      Txt.Set("Tag %s not allowed to be changed", OldTag);
      break;
    case EOCT_DRVNOTALLOWED:
      Txt.Set("Tag %s is referenced by a driver and cannot be changed while solving", OldTag);
      break;
    case EOCT_ARCNOTALLOWED:
      Txt.Set("Tag %s is referenced by the acrhiver and cannot be changed while solving", OldTag);
      break;
    case EOCT_DONE:
      //Txt.Set("Tag %s EOCT_Done", OldTag);
      //gs_AccessWnds.CloseAccessData(-1, False, False, True);
      //gs_AccessWnds.CloseWnd(-1);
      //for (int a=0; a<NAccessWnds; a++)
      //  {
      //  if (!fCallAccess && gs_AccessWnds.CurTag(a) && strlen(gs_AccessWnds.CurTag(a))>0 && _stricmp(gs_AccessWnds.CurTag(a), OldTag)==0)
      //    {
      //    fCallAccess = 1;
      //    break;
      //    }
      //  }
      //if (fCallAccess)
      //  {
      //  char* pTxt = new char[NewTag.Len()+1];
      //  strcpy(pTxt, NewTag);
      //  ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAG_ACCESS, (LPARAM)pTxt);
      //  }

      break;
    default:
      Txt.Set("Tag %s ?????????????", OldTag);
    }
  if (Txt.Len()>0)
    AddApplyLog("%s", Txt());

  return Err;
  }

//--------------------------------------------------------------------------

void CBulkTagChange::OnBnClickedShowclasses()
  {
  m_ClassColWidth=-m_ClassColWidth;
  ShowSelectedColumns();
  }

void CBulkTagChange::OnBnClickedShowpages()
  {                                    
  m_PageColWidth=-m_PageColWidth;
  ShowSelectedColumns();
  }

//--------------------------------------------------------------------------

void CBulkTagChange::OnHdnItemclickTaglist(NMHDR *pNMHDR, LRESULT *pResult)
  {
  LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
  int ColIndex=phdr->iItem;
  switch (ColIndex)
    {
    case ColNumberClass:
    case ColNumberCurrent:
    case ColNumberReplace:
    case ColNumberStatus:
      {
      for (int i=0; i<m_SortOrder.GetSize() && m_SortOrder[i]<ColCount; i++)
        if (abs(m_SortOrder[i])==ColIndex)
          break;

      int New=m_SortOrder[i];
      if (New>=ColCount)
        New=ColIndex; // Ascend

      if (i==0)
        New=-New;
      for ( ; i>0; i--)
        m_SortOrder[i]=m_SortOrder[i-1];

      m_SortOrder[0]=New;

      HpSort(m_Tags.GetCount(), (void**)&m_Tags[0], HpTagTest);

      LoadTagList();
      break;
      }    
    }
  *pResult = 0;
  }

//--------------------------------------------------------------------------

void CBulkTagChange::OnLvnBeginlabeleditTaglist(NMHDR *pNMHDR, LRESULT *pResult)
  {
  NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
  CEdit * pEdit=m_TagList.GetEditControl();
  int Item=pDispInfo->item.iItem;
  CRect Rct;
  CString Txt;
  m_TagList.GetItemRect(Item, &Rct, LVIR_BOUNDS);
  m_TagList.GetSubItemRect(Item, ColNumberReplace, LVIR_BOUNDS, Rct);
  Txt=m_TagList.GetItemText(Item, ColNumberReplace);


  pEdit->MoveWindow(Rct.left, Rct.top, Rct.Width(), Rct.Height());
  pEdit->SetWindowText(Txt);

  *pResult = 0;
  }

void CBulkTagChange::OnLvnEndlabeleditTaglist(NMHDR *pNMHDR, LRESULT *pResult)
  {
  NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
  CEdit * pEdit=m_TagList.GetEditControl();
  int Item=pDispInfo->item.iItem;
  CString Txt;
  pEdit->GetWindowText(Txt);

  int Index=m_TagList.GetItemData(Item);
  CBTCItem &Tg=*m_Tags[Index];
  Tg.Replace(Txt);
  if (Tg.m_DisplayIndex>=0)
    m_TagList.Update(Tg.m_DisplayIndex);

  *pResult = 0;
  }

void CBulkTagChange::OnLvnItemActivateTaglist(NMHDR *pNMHDR, LRESULT *pResult)
  {
  LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

  StartInplaceEdit(pNMIA->iItem, 0, 0, &pNMIA->ptAction);

  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CBulkTagChange::StartInplaceEdit(int Item, UINT  nChar, UINT nFlags, POINT *Pt)
  {
  m_InPlaceEditItem=Item;

  int TitleHgt=GetSystemMetrics(SM_CYCAPTION);

  CRect Rct;
  m_TagList.GetItemRect(m_InPlaceEditItem, &Rct, LVIR_LABEL);
  m_TagList.GetSubItemRect(m_InPlaceEditItem, ColNumberReplace, LVIR_LABEL, Rct);

  UpdateChangeCounts();

  CRect RctEdit(Rct.left, 
    Rct.top-1, 
    Rct.left+Rct.Width()+1, 
    Rct.top-1+Rct.Height()+1);

  m_InPlaceEdit.Create(ES_LEFT | ES_WANTRETURN |ES_AUTOHSCROLL | 
    WS_CHILD | WS_VISIBLE | WS_BORDER, 
    RctEdit, &m_TagList, IDC_INPLACEEDIT);
  m_InPlaceEdit.SubclassDlgItem(IDC_INPLACEEDIT, this);
  m_InPlaceEdit.SetFont(m_TagList.GetFont());

  CString Txt;
  if (nChar>0)
    {
    Txt.Format("%c", nChar);
    m_InPlaceEdit.SetWindowText(Txt);
    m_InPlaceEdit.SetSel(1,1,0);
    }
  else
    {
    Txt=m_TagList.GetItemText(m_InPlaceEditItem, ColNumberReplace);
    m_InPlaceEdit.SetWindowText(Txt);
    if (Pt)
      {
      m_TagList.ClientToScreen(Pt);
      int l=Txt.GetLength();
      CPoint TxtPt;
      for (int i=0; i<=l; i++)
        {
        TxtPt=m_InPlaceEdit.PosFromChar(i);
        m_InPlaceEdit.ClientToScreen(&TxtPt);
        if (TxtPt.x>=Pt->x)
          break;
        }
      m_InPlaceEdit.SetSel(i,i,0);
      }
    }

  m_InPlaceEdit.SetFocus();
  m_InPlaceEdit.m_IsValid=true;
  }

//---------------------------------------------------------------------------

void CBulkTagChange::FinishInplaceEdit()
  {
  int Index=m_TagList.GetItemData(m_InPlaceEditItem);
  CBTCItem &Tg=*m_Tags[Index];

  if (m_InPlaceEdit.m_IsValid)
    {
    CString Txt;

    m_InPlaceEdit.GetWindowText(Txt);

    Tg.Replace(Txt);
    m_ReplaceUndoCnt++;
    }

  m_InPlaceEditItem=-1;
  if (Tg.m_DisplayIndex>=0)
    m_TagList.Update(Tg.m_DisplayIndex);

  UpdateChangeCounts();
  }

//---------------------------------------------------------------------------

void CBulkTagChange::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
  {
  CDialog::OnGetMinMaxInfo(lpMMI);

  if (!m_Inited)
    return;

  lpMMI->ptMinTrackSize.x = (3*m_ClientRctInit.Width())/4;
  lpMMI->ptMinTrackSize.y = (3*m_ClientRctInit.Height())/4;
  }

void CBulkTagChange::OnSize(UINT nType, int cx, int cy)
  {
  CDialog::OnSize(nType, cx, cy);
  if (!m_Inited)
    return;

  switch (nType)
    {
    case SIZE_MAXIMIZED:
    case SIZE_RESTORED:
      PositionControls(cx, cy);
      InvalidateRect(NULL);
      break;
    }
  };

void CBulkTagChange::OnMouseMove(UINT nFlags, CPoint point)
  {
  CDialog::OnMouseMove(nFlags, point);

  CRect TagRct, ClassRct, PageRct, ClntRct, ApplyRct, ClipRct;
  m_TagList.GetWindowRect(&TagRct);
  ScreenToClient(&TagRct);
  m_StaticApply.GetWindowRect(&ApplyRct);
  ScreenToClient(&ApplyRct);

  GetClientRect(&ClntRct);

  HCURSOR h=NULL;
  int SetWhat=m_SetWhat;

  if (point.y>TagRct.top && point.y<TagRct.bottom)
    {
    m_ClassList.GetWindowRect(&ClassRct);
    m_PageList.GetWindowRect(&PageRct);
    ScreenToClient(&ClassRct);
    ScreenToClient(&PageRct);
    if (point.x>TagRct.right && point.x<ClassRct.left)
      {
      if (SetWhat<0)
        SetWhat=0;
      }
    else if (point.x>ClassRct.right && point.x<PageRct.left)
      {
      if (SetWhat<0)
        SetWhat=1;
      }
    }
  else if (point.y>ApplyRct.top && point.y<ApplyRct.bottom)
    {
    m_StaticClip.GetWindowRect(&ClipRct);
    ScreenToClient(&ClipRct);
    if (point.x>ClipRct.right && point.x<ApplyRct.left)
      {
      if (SetWhat<0)
        SetWhat=2;
      }
    }

  SetCursor(h);
  if (SetWhat>=0)
    {
    if (nFlags & MK_LBUTTON)
      {
      m_SetWhat=SetWhat;
      m_SepPos[SetWhat]=(1000*point.x)/ClntRct.Width();
      PositionControls(ClntRct.Width(), ClntRct.Height());
      }
    h=ScdApp()->LoadCursor(MAKEINTRESOURCE(IDC_ADJUSTEW));
    }
  else
    h=ScdApp()->LoadStandardCursor(IDC_ARROW);
  SetCursor(h);
  };

//---------------------------------------------------------------------------

void CBulkTagChange::OnLButtonDown(UINT nFlags, CPoint point) 
  {
  CDialog::OnLButtonDown(nFlags, point);
  SetCapture();
  }

void CBulkTagChange::OnLButtonUp(UINT nFlags, CPoint point) 
  {
  ReleaseCapture();
  m_SetWhat=-1;
  CDialog::OnLButtonUp(nFlags, point);
  }

//---------------------------------------------------------------------------

void CBulkTagChange::PositionControls(int cx, int cy)
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
    if (CtrlIDs[i].m_TrkTL & TrkSep0)
      X1=(m_SepPos[0]*cx)/1000+(X1-m_SepInit[0]);
    if (CtrlIDs[i].m_TrkTL & TrkSep1)
      X1=(m_SepPos[1]*cx)/1000+(X1-m_SepInit[1]);
    if (CtrlIDs[i].m_TrkTL & TrkSep2)
      X1=(m_SepPos[2]*cx)/1000+(X1-m_SepInit[2]);
    if (CtrlIDs[i].m_TrkBR & TrkLeft)
      {}

    if (CtrlIDs[i].m_TrkBR & TrkRight)
      X2+=cx-CW;
    if (CtrlIDs[i].m_TrkBR & TrkSep0)
      X2=(m_SepPos[0]*cx)/1000+(X2-m_SepInit[0]);
    if (CtrlIDs[i].m_TrkBR & TrkSep1)
      X2=(m_SepPos[1]*cx)/1000+(X2-m_SepInit[1]);
    if (CtrlIDs[i].m_TrkBR & TrkSep2)
      X2=(m_SepPos[2]*cx)/1000+(X2-m_SepInit[2]);

    if (CtrlIDs[i].m_TrkTL & TrkTop)
      {}
    if (CtrlIDs[i].m_TrkTL & TrkBottom)
      Y1+=cy-CH;
    if (CtrlIDs[i].m_TrkBR & TrkTop)
      {}
    if (CtrlIDs[i].m_TrkBR & TrkBottom)
      Y2+=cy-CH;

    //Y+=cy-CH;

    Wnd.SetWindowPos(NULL, X1, Y1, X2-X1, Y2-Y1, SWP_NOCOPYBITS|SWP_NOACTIVATE|SWP_NOZORDER);
    //Wnd.Invalidate(false);
    }
  };

//---------------------------------------------------------------------------

int CBulkTagChange::AddApplyLog(char * fmt, ...)
  {
  char buff[4096];
  va_list argptr;
  va_start(argptr,fmt);
  vsprintf(buff, fmt, argptr);
  va_end(argptr);
  int i=m_ApplyLog.InsertItem(m_ApplyLog.GetItemCount(), buff);
  //m_ApplyLog.Update(i);
  m_ApplyLog.EnsureVisible(i, false);
  return i;
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CBTCEdit, CEdit)
  ON_WM_KEYDOWN()
  ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CBTCEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  switch (nChar)
    {
    case VK_ESCAPE:
      m_IsValid=false;
    case VK_UP:
    case VK_DOWN:
    case VK_RETURN:
      m_Dlg.SetFocus();
      break;
    default:
      CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
      break;
    }
  }

//---------------------------------------------------------------------------

void CBTCEdit::OnKillFocus( CWnd* pNewWnd )
  {
  m_Dlg.FinishInplaceEdit();
  CEdit::OnKillFocus( pNewWnd );
  DestroyWindow();
  };

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CBTCListCtrl, CListCtrl)
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CBTCListCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  if ((nChar>='0' && nChar<='9' ||
    nChar>='a' && nChar<='z' ||
    nChar>='A' && nChar<='Z' ||
    nChar=='_') && (nFlags & MK_CONTROL)==0)
    {
    int i=m_Dlg.m_TagList.GetSelectionMark();
    if (i>=0)
      m_Dlg.StartInplaceEdit(i, nChar, nFlags, NULL);
    }
  else
    CListCtrl::OnChar(nChar, nRepCnt, nFlags);
  }

//---------------------------------------------------------------------------

void CBTCListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  if (nChar==VK_RETURN && (nFlags & MK_CONTROL & MK_SHIFT)==MK_CONTROL)
    {
    int i=m_Dlg.m_TagList.GetSelectionMark();
    if (i>=0)
      m_Dlg.StartInplaceEdit(i, 0, 0, NULL);
    }
  else if ((nChar=='a' || nChar=='A') && (nFlags & MK_CONTROL)==MK_CONTROL)
    m_Dlg.SelectAll();
  else if ((nChar=='c' || nChar=='C') && (nFlags & MK_CONTROL)==MK_CONTROL)
    m_Dlg.ExportSelected();
  else if ((nChar=='x' || nChar=='X') && (nFlags & MK_CONTROL)==MK_CONTROL)
    m_Dlg.ExportSelected();
  else if ((nChar=='v' || nChar=='V') && (nFlags & MK_CONTROL)==MK_CONTROL)
    m_Dlg.ImportSelected();
  else
    CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
  }

void CBTCListCtrl::OnRButtonDown(UINT nFlags, CPoint Point)
  {
  CMenu Menu;
  Menu.CreatePopupMenu();
  Menu.AppendMenu(MF_STRING, 100, "Select All\tCtrl+A");
  Menu.AppendMenu(MF_STRING, 101, "Export(Copy)\tCtrl+C");
  Menu.AppendMenu(MF_STRING, 102, "Import(Paste)\tCtrl+V");

  CPoint curPoint;
  GetCursorPos(&curPoint);

  int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
  Menu.DestroyMenu();                                           
  switch (RetCd)
    {
    case 100: m_Dlg.SelectAll(); break;
    case 101: m_Dlg.ExportSelected(); break;
    case 102: m_Dlg.ImportSelected(); break;
    default:
      CListCtrl::OnRButtonDown(nFlags, Point);
    }
  };

//---------------------------------------------------------------------------

int GetLineOfFlds(char ** pStart, CSVector &FldBuff)
  {
  char * p=*pStart;
  long NFlds=0;
  for (int i=0; i<FldBuff.GetSize(); i++)
    FldBuff[i]="";
  for (;;)
    {
    int i=NFlds;
    FldBuff.SetSize(Max(FldBuff.GetSize(), NFlds+1));
    char *p1=strpbrk(p, "\t\n\r");
    if (p1)
      {
      NFlds++;
      FldBuff[i].Set("%*.*s", p1-p, p1-p, p);
      p=p1+1;
      if (*p1=='\r' && *p=='\n') // CR-LF
        {
        p++;
        break;
        }
      else if (*p1=='\n' || *p1=='\r') // newline
        break;
      }
    else if (strlen(p)>0)
      {
      NFlds++;
      FldBuff[i]=p;
      p=p+strlen(p);
      break;
      }
    else
      break;
    }
  *pStart=p;
  return NFlds;
  }

//---------------------------------------------------------------------------

void CBulkTagChange::OnBnClickedExport()
  {
  ExportSelected();
  }

void CBulkTagChange::OnBnClickedImport()
  {
  ImportSelected();
  }

//---------------------------------------------------------------------------

void CBulkTagChange::SelectAll()
  {
  for (int i=0; i<m_TagList.GetItemCount(); i++)
    m_TagList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
  }

//---------------------------------------------------------------------------

const LPCTSTR CurrentTagName="CurrentTag";
const LPCTSTR ReplacementTagName="ReplacementTag";

void CBulkTagChange::ExportSelected()
  {
  CString Buff, S;
  Buff.Format("%s\t%s\t%s\t%s\n", "ClassId", CurrentTagName, ReplacementTagName, "Pages");
  BOOL All=m_TagList.GetSelectedCount()==0;
  for (int i=0; i<m_TagList.GetItemCount(); i++)
    {
    if (All || m_TagList.GetItemState(i, LVIS_SELECTED))
      {
      CBTCItem &Tg=*m_Tags[TagIndex(i)];
      S.Format("%s\t%s\t%s\t%s\n", Tg.m_pClassId->m_sClassId, Tg.m_sCurrent, Tg.m_sReplacement, Tg.m_sPages);
      Buff+=S;
      }
    }
  CopyTextToClipboard(this, Buff);
  }

//---------------------------------------------------------------------------

void CBulkTagChange::ImportSelected()
  {
  if (OpenClipboard())
    {
    HANDLE hData=GetClipboardData(CF_TEXT);
    if (hData!=NULL)
      {
      Strng DataBuff;
      char *p= (char *)GlobalLock(hData);

      int iCurrent=-1;
      int iReplacement=-1;
      CSVector FldBuff;
      int NFlds=GetLineOfFlds(&p, FldBuff);
      for (int i=0; i<NFlds; i++)
        {
        if (FldBuff[i].XStrICmp(CurrentTagName)==0)
          iCurrent=i;
        if (FldBuff[i].XStrICmp(ReplacementTagName)==0)
          iReplacement=i;
        }

      if (iCurrent>=0 && iReplacement>=0)
        {
        int iRight=Max(iCurrent, iReplacement);

        int NTagsImported=0;
        while (NFlds=GetLineOfFlds(&p, FldBuff))
          {
          Strng & CurrentTag=FldBuff[iCurrent];
          Strng & ReplacementTag=FldBuff[iReplacement];
          if (NFlds>iRight && CurrentTag.Len()>0 && ReplacementTag.Len()>0)
            {
            CBTCItem * pItem;
            if (m_TagMap.Lookup(CurrentTag(), pItem))
              {
              pItem->Replace(ReplacementTag());
              if (pItem->m_Changed)
                m_nImportCount++;
              NTagsImported++;
              dbgpln(">>> %-30s %-30s", CurrentTag(), ReplacementTag());
              }
            else
              AddApplyLog("Import Tag %s not found", CurrentTag());
            }
          else
            break;
          }
        m_ReplaceUndoCnt++;

        AddApplyLog("%i Tags imported", NTagsImported);
        }

      GlobalUnlock(hData);
      }
    CloseClipboard();
    }
  UpdateChangeCounts();
  }

//---------------------------------------------------------------------------
