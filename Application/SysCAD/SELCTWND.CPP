//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"

#define  __SELCTWND_CPP

#include "sc_defs.h"
#include "resource.h"
#include "selctwnd.h"
#include "executiv.h"
#include "sfe_clnt.h"
#include "project.h"
#include "msgwnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================

//CTagTrees TagTrees;

//===========================================================================

CTagTreeItem::CTagTreeItem(char* pTag, char* pValue, DWORD Flags, int SeqNo)
  {
  sTag = pTag;
  sValue = pValue;
  dwFlags = Flags;
  iSeqNo = SeqNo;
  pNxt = NULL;
  pChild = NULL;
  pParent = NULL;
  }

//---------------------------------------------------------------------------

CTagTreeItem::CTagTreeItem(char* pTag, char* pValue, DWORD Flags, int SeqNo, CTagTreeItem* Parent)
  {
  sTag = pTag;
  sValue = pValue;
  dwFlags = Flags;
  iSeqNo = SeqNo;
  pNxt = NULL;
  pChild = NULL;
  pParent = Parent;
  }

//---------------------------------------------------------------------------

CTagTreeItem::CTagTreeItem(char* pTag, char* pValue, DWORD Flags, int SeqNo, CTagTreeItem* Parent, CTagTreeItem* Nxt)
  {
  sTag = pTag;
  sValue = pValue;
  dwFlags = Flags;
  iSeqNo = SeqNo;
  pNxt = Nxt;
  pChild = NULL;
  pParent = Parent;
  }

//---------------------------------------------------------------------------

CTagTreeItem::~CTagTreeItem()
  {
  if (pChild)
    delete pChild;
  if (pNxt)
    delete pNxt;
  }

//---------------------------------------------------------------------------

CTagTreeItem* CTagTreeItem::InsertChildAtEnd(char* pTag, char* pValue, DWORD Flags, int SeqNo)
  {
  CTagTreeItem* p = new CTagTreeItem(pTag, pValue, Flags, SeqNo, this);
  if (pChild)
    {
    CTagTreeItem* pLastChild = pChild;
    while (pLastChild->Next())
      pLastChild = pLastChild->Next();
    pLastChild->pNxt = p;
    }
  else
    {
    pChild = p;
    }
  return p;
  }

//---------------------------------------------------------------------------

CTagTreeItem* CTagTreeItem::InsertChildAtBegining(char* pTag, char* pValue, DWORD Flags, int SeqNo)
  {
  pChild = new CTagTreeItem(pTag, pValue, Flags, SeqNo, this, pChild);
  return pChild;
  }

//---------------------------------------------------------------------------

CTagTreeItem* CTagTreeItem::FindChildTag(char* pTag)
  {
  CTagTreeItem* p = pChild;
  while (p)
    {
    if (stricmp(p->sTag(), pTag)==0)
      return p;
    p = p->pNxt;
    }
  return NULL;
  }

//---------------------------------------------------------------------------

void CTagTreeItem::GetFullTag(Strng& FullTag)
  {
  if (pParent)
    {
    pParent->GetFullTag(FullTag);
    FullTag.Append(".%s", sTag());
    }
  else
    FullTag = sTag();
  }

//===========================================================================

void CTagTree::Build(char* pRootTag)
  {
  ASSERT(gs_pPrj);
  DeleteAllItems();
  if (pRootTag==NULL || strlen(pRootTag)==0)
    return;
  int SeqNo = 0;
  CXM_ObjectTag ObjTag(pRootTag, TABOpt_AllInfoOnce|TABOpt_ForView);
  CXM_ObjectData ObjData;
  CXM_Route Route;
  if (gs_pPrj->XReadTaggedItem(ObjTag, ObjData, Route))
    {
    CPkDataIter Iter;
    CPkDataItem * pItem = ObjData.FirstItem(Iter);
    //Strng Class;
    //Class = pItem->Class();
    //char Buff[1024];
    Strng TheTag,Value;
    Strng_List sStrList;
    int TagLevel = 0;
    CTagTreeItem* p = NULL;
    while (pItem)
      {
      const byte Type = pItem->Type();
      //sprintf(Buff, "%20s : %20s (%s)", pItem->SymOrTag(), pItem->Value() ? pItem->Value()->GetString() : "xxx", tt_TypeString(Type));
      //dbgpln(Buff);
      if (!pItem->Empty() && pItem->Contains(PDI_Tag) && pItem->Contains(PDI_Value) && Type!=tt_Blob)
        {
        if (IsData(Type))
          {
          if (pItem->Value())
            {
            if (IsIntData(Type) && pItem->Contains(PDI_StrList))
              {
              pItem->GetStrList(sStrList);
              const long strIndex = pItem->Value()->GetLong();
              pStrng pS = (sStrList.Length()>0) ? sStrList.AtIndexVal(strIndex) : NULL;
              if (pS)
                Value = pS->Str();
              else
                Value.Set("%i", strIndex);
              }
            else
              Value = pItem->Value()->GetString();
            p->InsertChildAtEnd(pItem->SymOrTag(), Value(), (DWORD)pItem->Flags(), SeqNo++);
            }
          else
            p->InsertChildAtEnd(pItem->SymOrTag(), "", (DWORD)pItem->Flags(), SeqNo++);
          }
        //else if (Type==tt_Struct || Type==tt_Object)
        else if (IsStructStart(Type))
          {
          if (p==NULL)
            {
            pRoot = new CTagTreeItem(pItem->SymOrTag(), "", (DWORD)pItem->Flags(), SeqNo++);
            p = pRoot;
            }
          else
            p = p->InsertChildAtEnd(pItem->SymOrTag(), "", (DWORD)pItem->Flags(), SeqNo++);
          TagLevel++;
          }
        //else if (Type==tt_Struct_E || Type==tt_Object_E)
        else if (IsStructEnd(Type))
          {
          TagLevel--;
          p = p->Parent();
          }
        }
      pItem = ObjData.NextItem(Iter);
      }
    }
  }

//---------------------------------------------------------------------------

void CTagTree::DeleteAllItems()
  {
  if (pRoot)
    {
    delete pRoot;
    pRoot = NULL;
    }
  }

//---------------------------------------------------------------------------

flag CTagTreeItem::DoPopulateTree(HTREEITEM hParent, CTreeCtrl& Tree, TV_INSERTSTRUCT& Ins, DWORD dwMask, DWORD dwExcludeMask)
  {
  CTagTreeItem* p = this;
  flag DidInsert = 0;
  while (p)
    {
    if ( p->Child() || ((p->Flags() & dwMask) && !(p->Flags() & dwExcludeMask)) )
      {
      Strng Txt;
      Ins.hParent = hParent;
      if (p->ValueLen()>0)
        Txt.Set("%s (%s)", p->Tag(), p->Value());
      else
        Txt = p->Tag();
      Ins.item.pszText = Txt();
      HTREEITEM h = Tree.InsertItem(&Ins);
      //DidInsert |= ((dwFlags & dwMask)!=0);
      if (p->Child())
        {
        if (p->Child()->DoPopulateTree(h, Tree, Ins, dwMask, dwExcludeMask))
          DidInsert = 1;
        else
          Tree.DeleteItem(h);
        }
      else
        DidInsert = 1;
      }
    p = p->Next();
    }
  return DidInsert;
  }

//---------------------------------------------------------------------------

void CTagTree::PopulateTree(CTreeCtrl& Tree, TV_INSERTSTRUCT& Ins, DWORD dwMask, DWORD dwExcludeMask)
  {
  if (pRoot && pRoot->Child())
    pRoot->Child()->DoPopulateTree(TVI_ROOT, Tree, Ins, dwMask, dwExcludeMask);
  }

//===========================================================================

CTagTrees::CTagTrees()
  {
  }

//---------------------------------------------------------------------------

CTagTrees::~CTagTrees()
  {
  for (int i=0; i<List.GetSize(); i++)
    delete List[i];
  }

//---------------------------------------------------------------------------

int CTagTrees::FindModelName(char* pModelName)
  {
  if (pModelName)
    for (int i=0; i<List.GetSize(); i++)
      if (stricmp(pModelName, (const char*)(List[i]->sModelName))==0)
        return i;
  return -1;
  }

//---------------------------------------------------------------------------

int CTagTrees::FindPartModelName(char* pModelName, int IndexStart)
  {
  if (pModelName)
    for (int i=Max(IndexStart+1, 0); i<List.GetSize(); i++)
      if (strnicmp(pModelName, (const char*)(List[i]->sModelName), List[i]->sModelName.GetLength())==0)
        return i;
  return -1;
  }

//---------------------------------------------------------------------------

void CTagTrees::Rebuild(int Request)
  {
  if (gs_pTheSFELib)
    {
    CWaitCursor Wait;
    ASSERT(gs_pTheSFELib);// && pExec);
    ASSERT((gs_pPrj==NULL) || (gs_pPrj->FlwLib()==gs_pTheSFELib));
    for (int i=0; i<List.GetSize(); i++)
      List[i]->bFlagged = 0;
    int ModelTypeCnt = 0;
    RequestModelInfoRec MInfo;
    for (int j=0; j<3; j++)
      {
      char* pGroup = (j==0 ? FlwUnitGrp : j==1 ? FlwLinkGrp : "Hidden");
      int nModels = 0;
      while (gs_pTheSFELib->RequestModelInfoByGroupIndex(pGroup, nModels, MInfo))
        {
//_asm int 3;
        int index = FindModelName(MInfo.Class());
        if (index<0)
          {
          CModelTypeList* pNew = new CModelTypeList(MInfo.Class());
          index = List.Add(pNew);
          }
        CModelTypeList* pTagList = List[index];
        pTagList->bFlagged = 1;
        pTagList->bUnit = (j==0);
        ModelTypeCnt++;
        int Cnt = 0;
        RequestTagInfoRec Rqst(Request);//RQ_Tags);
        Rqst.bFirst = 1;
        Rqst.pGroup = MInfo.Class();
        ReplyTagInfoRec Info;
        while (gs_Exec.RequestTagInfo(Rqst, Info))
          {
          if (Cnt<pTagList->GetSize())
            (*pTagList)[Cnt] = Info.sData();
          else
            pTagList->Add(Info.sData());
          Cnt++;
          }
        if (pTagList && Cnt<pTagList->GetSize())
          pTagList->SetSize(Cnt);
        nModels++;
        }
      }
    for (i=List.GetSize()-1; i>=0; i--)
      if (!List[i]->bFlagged)
        {
        delete List[i];
        List.RemoveAt(i);
        }
    if (ModelTypeCnt<List.GetSize())
      List.SetSize(ModelTypeCnt);
    }
  else
    {
    List.SetSize(0);
    }
  }

//===========================================================================

CTagTrees CSelectTagDlg::TagTrees;

CSelectTagDlg::CSelectTagDlg(char* pTag, byte SelnAllowed/*=SelFull*/, flag AllowTagEdit/*=1*/, CWnd* pParent/*=NULL*/)
  : CDialog(CSelectTagDlg::IDD, pParent)
  {
  ASSERT(SelnAllowed>=SelAny && SelnAllowed<=SelRoot);
  eSelType = (TagSelectAllowed)SelnAllowed;
  bAllowTagEdit = AllowTagEdit;
  iLevel = 0;
  iMdlIndex = -1;
  iLastTagLen = 0;
  bDoneRebuild = 0;
  bBackward = 0;
  bEnableOK = 0;
  pTI = NULL;
  Strng WrkTag;
  if (pTag)
    TaggedObject::SplitTagCnv(pTag, WrkTag, sCnvTxt);
  //{{AFX_DATA_INIT(CSelectTagDlg)
  sTag = _T(WrkTag());
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CSelectTagDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSelectTagDlg)
  DDX_Control(pDX, IDC_SELN, Seln);
  DDX_Text(pDX, IDC_TAG, sTag);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSelectTagDlg, CDialog)
  //{{AFX_MSG_MAP(CSelectTagDlg)
  ON_BN_CLICKED(IDC_BACK, OnBack)
  ON_CBN_SELCHANGE(IDC_SELN, OnSelchangeSeln)
  ON_BN_CLICKED(IDC_FWD, OnFwd)
  ON_EN_CHANGE(IDC_TAG, OnChangeTag)
  ON_BN_CLICKED(IDC_ROOT, OnRoot)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDC_FWD, OnUpdateFwd)
  ON_UPDATE_COMMAND_UI(IDC_BACK, OnUpdateBck)
  ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOK)
  ON_UPDATE_COMMAND_UI(IDC_TAG, OnUpdateTagEdit)
  ON_UPDATE_COMMAND_UI(IDC_SELN, OnUpdateSelnCombo)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CSelectTagDlg::CalcLevel(flag UseLast)
  {
  ASSERT(gs_pPrj);
  UpdateData(TRUE);
  Strng s((const char*)sTag);
  s.Trim(" \t");
  iLastTagLen = 0;
  sTag = "";
  sRootTag = "";
  iLevel = 0;
  pTI = NULL;
  if (s.Len()>0)
    {
    Strng PrevTag;
    sRootTag = s();
    int DotPos = sRootTag.Find('.');
    if (DotPos>=0)
      sRootTag = sRootTag.Left(DotPos);
    sTag = sRootTag;
    iLastTagLen = sTag.GetLength();
    CXM_ObjectTag ObjTag((char*)(const char*)sRootTag, TABOpt_Parms);//TABOpt_Exists);
    CXM_ObjectData ObjData;
    CXM_Route Route;
    if (gs_pPrj->XReadTaggedItem(ObjTag, ObjData, Route))
      {
      if (!bDoneRebuild)
        {
        TagTrees.Rebuild(RQ_Tags);
        bDoneRebuild = 1;
        }
      CPkDataItem * pItem = ObjData.FirstItem();
      Strng Class;
      Class = pItem->Class();
      iMdlIndex = TagTrees.FindModelName(Class());
      if (iMdlIndex<0)
        {
        iLevel = 0;
        sTag = "";
        sRootTag = "";
        }
      else
        {
        if (eSelType==SelRoot)
          {
          iLevel = 1;
          sTag = sRootTag;
          }
        else
          {
          iLevel = 2;
          sTag = sRootTag;
          UpdateData(FALSE);
          CalcTagTree();

          flag First = 1;
          flag Done = 0;
          DotPos = -1;
          while (!Done)
            {
            DotPos = s.FindFrom(DotPos+1, '.');
            if (DotPos>=0)
              {
              Strng sss;
              sss = s.Left(DotPos);
              sTag = sss();
              }
            else
              sTag = s();
            if (!First)
              {
              Strng ss((const char*)sTag);
              int ADotPos = ss.ReverseFind('.');
              if (ADotPos>=0)
                ss = ss.Mid(ADotPos+1, 4096);
              CTagTreeItem* p = ss() ? pTI->FindChildTag(ss()) : NULL;
              if (p)
                {
                pTI = p;
                iLevel++;
                }
              else
                {
                ADotPos = sTag.ReverseFind('.');
                if (UseLast)
                  PrevTag = (const char*)sTag;
                if (ADotPos>=0)
                  sTag = sTag.Left(ADotPos);
                Done = 1;
                }
              }
            if (!Done)
              {
              if (DotPos<0)
                Done = 1;
              }
            First = 0;
            }
          }
        }
      }
    else
      {
      sTag = "";
      if (UseLast)
        PrevTag = (const char*)sRootTag;
      sRootTag = "";
      iLevel = 0;
      }
    iLastTagLen = sTag.GetLength();
    if (PrevTag.Len()>0)
      sTag = PrevTag();
    UpdateData(FALSE);
    }
  }

//---------------------------------------------------------------------------

void CSelectTagDlg::CalcSelnList()
  {
  Seln.ResetContent();
  Strng Desc;
  switch (iLevel)
    {
    case 0:
      {
      if (!bDoneRebuild)
        {
        TagTrees.Rebuild(RQ_Tags);
        bDoneRebuild = 1;
        }
      CModelTypeListArray& List = TagTrees.GetList();
      for (int j=0; j<List.GetSize(); j++)
        {
        //CModelTypeList* pTagList = List[j];
        if (List[j]->GetSize()>0)
          Seln.AddString(List[j]->ModelName());
        }
      if (Seln.GetCount()>0)
        Desc += "Select required model type";
      break;
      }
    case 1:
      if (iMdlIndex>=0)
        {
        CModelTypeListArray& List = TagTrees.GetList();
        CModelTypeList* pTagList = List[iMdlIndex];
        for (int j=0; j<pTagList->GetSize(); j++)
          Seln.AddString(pTagList->GetTagAt(j));
        Desc.Set("%s", pTagList->ModelName());
        if (Seln.GetCount()>0)
          Desc.Append("  Select %srequired model tag", bAllowTagEdit ? "or enter " : "");
        }
      else
        {
        iLevel--;
        CalcSelnList();
        return;
        }
      break;
    case 2:
      {
      if (iMdlIndex>=0)
        {
        CalcTagTree();
        CModelTypeListArray& List = TagTrees.GetList();
        Desc.Set("%s : %s", List[iMdlIndex]->ModelName(), (const char*)sRootTag);
        if (Seln.GetCount()>0)
          {
          if (sTag.GetLength()>0)
            {
            int jj = Seln.FindStringExact(-1, (const char*)sTag);
            if (jj>=0)
              Seln.SetCurSel(jj);
            }
          Desc.Append("  Select %stag", bAllowTagEdit ? "or enter " : "");
          }
        }
      else
        {
        iLevel--;
        CalcSelnList();
        return;
        }
      break;
      }
    default:
      {
      if (pTI && iMdlIndex>=0)
        {
        CalcSubSeln();
        CModelTypeListArray& List = TagTrees.GetList();
        Desc.Set("%s : %s", List[iMdlIndex]->ModelName(), (const char*)sRootTag);
        if (Seln.GetCount()>0)
          Desc += "  Select or enter tag";
        }
      else
        {
        iLevel--;
        CalcSelnList();
        return;
        }
      break;
      }
    }
  UpdateData(TRUE);
  /*if (Seln.GetCount()==1)
    {
    Seln.SetCurSel(0);
    if (bBackward)
      {
      if (iLevel>0)
        iLevel--;
      CalcSelnList();
      }
    else
      OnSelchangeSeln();
    return;
    }*/
  if (Seln.GetCount()>0 && Seln.GetCurSel()<0)
    Seln.SetCurSel(0);
  SetDlgItemText(IDC_TXT_DESC, Desc());
  SetDlgItemText(IDC_TXT_COMPLETE, (Seln.GetCount()==0 || (eSelType==SelRoot && iLevel==1 && iLastTagLen>0)) ? "COMPLETE" : "");
  bBackward = 0;
  }

//---------------------------------------------------------------------------

void CSelectTagDlg::CalcSubSeln()
  {
  Seln.ResetContent();
  if (pTI)
    {
    CTagTreeItem* p = pTI->Child();
    char Buff[1024];
    while (p)
      {
      if (p->Child())
        {
        sprintf(Buff, "%s ...", p->Tag());
        Seln.AddString(Buff);
        }
      else
        Seln.AddString(p->Tag());
      p = p->Next();
      }
    }
  }

//---------------------------------------------------------------------------

void CSelectTagDlg::CalcTagTree()
  {
  Seln.ResetContent();
  UpdateData(TRUE);
  Strng ss;
  if (iLastTagLen>0)
    ss = (const char*)sTag.Left(iLastTagLen);
  else
    ss = (const char*)sTag;
  ss.LRTrim(" \t");
  if (ss.Len()>0)
    {
    T.Build(ss());
    pTI = T.Root();
    CalcSubSeln();
    }
  /*flag Changed = 0;
  int len = sTag.GetLength();
  if (len>0)
    {
    Strng s((const char*)sTag);
    s.LRTrim(" \t");
    if (len!=s.Len())
      {
      Changed = 1;
      sTag = s();
      len = s.Len();
      }
    if (len>0)
      {
      if (s[len-1]=='.')
        s = s.Left(len-1);
      if (s.Len()==0)
        {
        sTag = "";
        Changed = 1;
        }
      else
        {
        T.Build(s());
        pTI = T.Root();
        CalcSubSeln();
        }
      }
    }
  if (Changed)
    UpdateData(FALSE);*/
  }

//---------------------------------------------------------------------------

BOOL CSelectTagDlg::OnInitDialog()
  {
  ASSERT(gs_pPrj);
  CDialog::OnInitDialog();
  CalcLevel(bAllowTagEdit);
  CalcSelnList();
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CSelectTagDlg::OnBack() 
  {
  bEnableOK = 0;
  UpdateData(TRUE);
  if (iLevel<1)
    iLevel = 0;
  else
    iLevel--;
  bBackward = 1;
  switch (iLevel)
    {
    case 0:
      sTag = "";
      iLastTagLen = 0;
      break;
    case 1:
      sTag = "";
      iLastTagLen = 0;
      break;
    case 2:
      sTag = sRootTag;
      iLastTagLen = sTag.GetLength();
      break;
    default:
      if (pTI)
        pTI = pTI->Parent();
      sTag = sTag.Left(iLastTagLen);
      int DotPos = sTag.ReverseFind('.');
      if (DotPos>=0)
        sTag = sTag.Left(DotPos);
      iLastTagLen = sTag.GetLength();
      break;
    }
  UpdateData(FALSE);
  CalcSelnList();
  UpdateDialogControls(this, FALSE);
  }

void CSelectTagDlg::OnUpdateBck(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(iLevel>0);
  }

//---------------------------------------------------------------------------

void CSelectTagDlg::OnFwd()
  {
  bEnableOK = 0;
  OnSelchangeSeln();
  UpdateDialogControls(this, FALSE);
  }

void CSelectTagDlg::OnUpdateFwd(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(Seln.GetCount()>0);
  }

//---------------------------------------------------------------------------

void CSelectTagDlg::OnRoot() 
  {
  iLevel = 0;
  iMdlIndex = -1;
  iLastTagLen = 0;
  bBackward = 0;
  bEnableOK = 0;
  pTI = NULL;
  sTag = "";
  sRootTag = "";
  UpdateData(FALSE);
  CalcSelnList();
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CSelectTagDlg::OnSelchangeSeln()
  {
  bEnableOK = 0;
  UpdateData(TRUE);
  switch (iLevel)
    {
    case 0:
      {
      CString Txt;
      int xxx=Seln.GetCurSel();
      Seln.GetLBText(xxx, Txt);
//      Seln.GetWindowText(Txt);
//      Seln.GetWindowText(Txt);
      if (Txt.GetLength()>0)
        {
        iMdlIndex = TagTrees.FindModelName((char*)(const char*)Txt);
        if (iMdlIndex>=0)
          {
          sTag = "";
          iLastTagLen = 0;
          UpdateData(FALSE);
          iLevel = 1;
          CalcSelnList();
          }
        }
      break;
      }
    case 1:
      {
      int xxx=Seln.GetCurSel();
      Seln.GetLBText(xxx, sRootTag);
//      Seln.GetWindowText(sRootTag);
      if (sRootTag.GetLength()>0)
        {
        sTag = sRootTag;
        iLastTagLen = sTag.GetLength();
        UpdateData(FALSE);
        if (eSelType!=SelRoot)
          iLevel = 2;
        CalcSelnList();
        }
      break;
      }
    default:
      {
      CString Txt;
      int xxx=Seln.GetCurSel();
      Seln.GetLBText(xxx, Txt);
//      Seln.GetWindowText(Txt);
      const int len = Txt.GetLength();
      if (len>0)
        {
        if (len>3 && Txt.Find(" ...")==len-4)
          Txt = Txt.Left(len-4);
        CTagTreeItem* p = pTI->FindChildTag((char*)(const char*)Txt);
        if (p)
          {
          sTag = sTag.Left(iLastTagLen);
          sTag += ".";
          sTag += Txt;
          iLastTagLen = sTag.GetLength();
          UpdateData(FALSE);
          iLevel++;
          pTI = p;
          CalcSelnList();
          }
        }
      break;
      }
    }
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CSelectTagDlg::OnUpdateSelnCombo(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(Seln.GetCount()>0);
  }

//---------------------------------------------------------------------------

void CSelectTagDlg::OnChangeTag() 
  {
  flag DoUpdCtrls = bEnableOK;
  bEnableOK = 0;
  //ASSERT(bAllowEdit);
  UpdateData(TRUE);
  Strng s((const char*)sTag);
  s.Trim(" \t");
  if (sTag.GetLength()!=s.Len())
    {
    sTag = s();
    UpdateData(FALSE);
    }
  Strng FullTag;
  if (pTI)
    pTI->GetFullTag(FullTag);
  if ( s.Len()<iLastTagLen || 
       FullTag.Len()<iLastTagLen || 
       strnicmp(s(), FullTag(), iLastTagLen)!=0 ||
       (s.Len()==iLastTagLen+1 && s[s.Len()-1]!='.') ||
       (s.Len()>iLastTagLen+1 && s[s.Len()-1]=='.') )
    {
    CalcLevel(True);
    CalcSelnList();
    DoUpdCtrls = 1;
    //TODO ensure cursor (caret) doesn't move... ???
    }
  else
    {
    Strng ss;
    if (iLastTagLen==0)
      ss = s;
    else
      ss = s.Mid(iLastTagLen+1, 4096);
    int i = Seln.FindStringExact(-1, ss());
    if (i>=0)
      {
      bEnableOK = 1;
      Seln.SetCurSel(i);
      DoUpdCtrls = 1;
      }
    else
      {
      i = Seln.FindString(-1, ss());
      if (i>=0)
        Seln.SetCurSel(i);
      }
    }
  if (DoUpdCtrls)
    UpdateDialogControls(this, FALSE);
  }

void CSelectTagDlg::OnUpdateTagEdit(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(bAllowTagEdit && iLevel>0);
  }

//---------------------------------------------------------------------------

void CSelectTagDlg::OnUpdateOK(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(eSelType==SelAny || Seln.GetCount()==0 || bEnableOK || (eSelType==SelRoot && iLevel==1 && iLastTagLen>0));
  }

//---------------------------------------------------------------------------

