//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
//#ifdef NEVER
#define  __TAGBROWSE_CPP

#include "sc_defs.h"
#include "resource.h"
#include "tagbrowse.h"
#include "executiv.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================

CTagTrees TagTrees;

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

void CTagTree::Build( /*CScdOPCCallBack*/CExecObj* pPrj, char* pRootTag)
  {

char Buff[160];

//  ASSERT(gs_pPrj);
  DeleteAllItems();
  if (pRootTag==NULL || strlen(pRootTag)==0)
    return;

  int SeqNo = 0;
  CXM_ObjectTag ObjTag(pRootTag, TABOpt_AllInfoOnce/*|TABOpt_ForView*/);
  CXM_ObjectData ObjData;
  CXM_Route Route;
  if ( pPrj->XReadTaggedItem(ObjTag, ObjData, Route))
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
      sprintf(Buff, "%20s : %20s (%s)", pItem->SymOrTag(), pItem->Value() ? pItem->Value()->GetString() : "xxx", tt_TypeString(Type));
      dbgpln(Buff);

      if (!pItem->Empty() && pItem->Contains(PDI_Tag) && pItem->Contains(PDI_Value) && Type!=tt_Blob)
        {

        if (IsStructStart(Type))
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
        else if (IsData(Type))
          {


         if (p==NULL) // Kludge until I work out what is going on
            {
            pRoot = new CTagTreeItem(pItem->SymOrTag(), "", (DWORD)pItem->Flags(), SeqNo++);
            p = pRoot;
            }


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

        } // if

      pItem = ObjData.NextItem(Iter);

      } // while
    } // if
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


    for (int i=0; i<List.GetSize(); i++)
      List[i]->bFlagged = 0;

    CModelTypeList* pNew = new CModelTypeList("SysCAD");
    int index = List.Add(pNew);
    CModelTypeList* pTagList = List[index];
    pTagList->bFlagged = 1;
    pTagList->bUnit = 1;
    int ModelTypeCnt = 1;
    int Cnt = 0;
    RequestTagInfoRec Rqst(Request);//RQ_Tags);
    Rqst.bFirst = 0;
    Rqst.pGroup = NULL;
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



    for (i=List.GetSize()-1; i>=0; i--)
      if (!List[i]->bFlagged)
        {
        delete List[i];
        List.RemoveAt(i);
        }
    if (ModelTypeCnt<List.GetSize())
      List.SetSize(1);

 
/*
  if (gs_pTheSFELib)
    {
    CWaitCursor Wait;
    ASSERT(gs_pTheSFELib && pExec);
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
        int index = FindModelName(MInfo.Name());
        if (index<0)
          {
          CModelTypeList* pNew = new CModelTypeList(MInfo.Name());
          index = List.Add(pNew);
          }
        CModelTypeList* pTagList = List[index];
        pTagList->bFlagged = 1;
        pTagList->bUnit = (j==0);
        ModelTypeCnt++;
        int Cnt = 0;
        RequestTagInfoRec Rqst(Request);//RQ_Tags);
        Rqst.bFirst = 1;
        Rqst.pGroup = MInfo.Name();
        ReplyTagInfoRec Info;
        while (pExec->RequestTagInfo(Rqst, Info))
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
*/

  }

//===========================================================================


//#endif