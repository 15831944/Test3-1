//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __NDTREE_CPP
//#include "datacnvs.h"
#include "NdTree.h"

#define dbgInitObj       0
#define xdbgInitObj      0

#define dbgSetValues     0
#define dbgGetValues     0
#define dbgGetSetValue   0
#define dbgConnect       0
#define dbgDisConnect    0
#define dbgConstruction  0

// ===========================================================================

//word CTNode::NoofNdStats=0;
//NodeStats CTNode::NdStats;

static flag DoLookUp=1;

//===========================================================================

int StrPosNo(pchar s1, pchar s2)
  {
  pchar p = strstr(s1,s2);
  if (p)
    return(p-s1);
  else
    return(-1);
  }

//===========================================================================

CExecObj *CTNode::sm_pExecObj=NULL;

CTNode::CTNode(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, TagIn, pAttach, eAttach),
  m_Electrics(this)//,
  //CNodeXRefMngr(this)
  {
  m_dwType = 0;
  MyParent = NULL;
  Nxt    = NULL;
  MyChildren = NULL;
  LastChild = NULL;
  NdInx  = -2;
  Internal = 0;
  AllDataHere=0;
  pChildMap=NULL;

  //NdStats[IdNo()].NoOf++;
  #if dbgConstruction
  //dbgpln("CTNode::%s %08x [%i] %s ",TagType, this,NdStats[NdId].NoOf,FullTag()/*, coreleft()*/);
  dbgpln("CTNode:: %08x %s", this, sTag());
  #endif
  Done = 0;
  Busy = 0;
  Break = 0;
  Done2 = 0;
  Reverse = 0;
  Copy = 0;

  //m_nXRefs2Me=0;
  };

//--------------------------------------------------------------------------

CTNode::~CTNode()
  {
  #if dbgConstruction
  dbgpln("~CTNode:: %08x %s", this, sTag());
  #endif

  if (MyParent)
    MyParent->Remove_Child(this);

  while (MyChildren)
    {
    CTNode* p= MyChildren;
    MyChildren=MyChildren->Nxt;
    p->Destroy();
    }

  //NdStats[IdNo()].NoOf--;

  delete pChildMap;
  };

//--------------------------------------------------------------------------
  
void CTNode::AttachClassInfo(DWORD Type_)
  {
  m_dwType=Type_;
  }

//--------------------------------------------------------------------------

void CTNode::BuildDataDefn(DataDefnBlk & DDB)
  {
  };

//--------------------------------------------------------------------------

pchar CTNode::Tag(pchar ReqdTag)
  {
  if (DoLookUp)
    {
    CNdTagMap *pMap=MyParent ? MyParent->pChildMap : NULL;
    if (ReqdTag && pMap)
      {
      pMap->RemoveKey(TaggedObject::Tag());
      pchar p=TaggedObject::Tag(ReqdTag);
      pMap->SetAt(TaggedObject::Tag(), this);
      return p;
      }
    else
      return TaggedObject::Tag(ReqdTag);
    }
  else
    return TaggedObject::Tag(ReqdTag);
  }

//--------------------------------------------------------------------------

TaggedObject * CTNode::FindObjTag(pchar pSrchTag, flag SrchAll, int &ObjTagLen, int MinTagLen)
  {
  if (pSrchTag==NULL)
    return NULL;
  if (DoLookUp)
    {
    ObjTagLen=0;
    int P1=0,L=0;
    CTNode* pRet=NULL;
    CTNode* SrchRoot=FamilyHead();
    if ((L=TaggedObject::TagCmp(SrchRoot->Tag(), &pSrchTag[P1], MinTagLen))>0)
      {
      ObjTagLen=L;
      P1=L;
      pRet=SrchRoot;
      }
  
    CTNode* p=NULL;
    flag Looking=1;

    // Look for tag while expanding the 
    // length of the SrchTag until found or not
    int NDots=0;
    //flag MoveDown=0;
    while (Looking)// && !MoveDown)
      {
      SCDFullTag SrchTag;
      strcpy(SrchTag, &pSrchTag[P1]);
      char * pDot=strchr(&SrchTag[MinTagLen], '.');
      for (int d=0; pDot && (d<NDots) ; d++)
        pDot=strchr(pDot+1, '.');
      if (pDot)
        *pDot=0; // Truncate @ Dot
      CTNode* p;
      if (SrchRoot->pChildMap && SrchRoot->pChildMap->Lookup(SrchTag, p))
        {
        // Move down a level and search it
        SrchRoot=p;  
        //ObjTagLen=P1;
        ObjTagLen=P1+strlen(SrchTag);
        P1+=strlen(SrchTag);
        if (pSrchTag[P1]=='.')
          P1++;
        pRet=p;
        //MoveDown=1;
        Looking=(strlen(&pSrchTag[P1])>0);
        NDots=0;
        MinTagLen=0;
        }
      else if (pDot) // More Tag to search
        NDots++;
      else    // End Of Tag & Not Found
        Looking=0;
      }

    //ObjTagLen=P1;
    return pRet;
    }
  else
    {
    ObjTagLen=0;
    int P1=0,L=0;
    CTNode* pRet=NULL;
    CTNode* h=FamilyHead();
    if ((L=TaggedObject::TagCmp(h->Tag(), &pSrchTag[P1], MinTagLen))>0)
      {
      P1=L;
      pRet=h;
      }
  
    CTNode* p=NULL, *p1=h;
    flag GotOne;
    do
      {
      if (SrchAll)
        GotOne=p1->GetObj_All(p);
      else
        GotOne=p1->GetObj(p);
      if (GotOne)  
        if ((L=TaggedObject::TagCmp(p->Tag(), &pSrchTag[P1], MinTagLen))>0)
          {
          ObjTagLen=P1;
          P1+=L;
          pRet=p;
          p1=p;
          p=NULL;
          }
      }
    while (GotOne);

    ObjTagLen=P1;
    return pRet;
    }
  };

//--------------------------------------------------------------------------

pchar CTNode::FullTag()
  {
  const int TagBuffLen = 128;
  static char FullTagTmp[4][TagBuffLen];
  static char FullTagInx = 0;
  pchar result = FullTagTmp[FullTagInx];
  CTNode* p[MaxTagDepth];
  int i = 0;

  FullTagInx = (++FullTagInx)%4;
  p[0] = this;
  while (p[i]->MyParent)
    {
    ASSERT(i+1<MaxTagDepth);
    p[i+1] = p[i]->MyParent;
    i++;
    }
  /*
  while (i && StartAt)
    {
    i--;
    StartAt--;
    };
  */
  while (i && (strlen(p[i]->Tag()) == 0))
    i--;
  strcpy(result, "");
  strcat(result,p[i]->Tag());
  while (i > 0)
    {
    i--;
    strcat(result,TagSeparator);
    strcat(result,p[i]->Tag());
    }
  /*
  switch (OId)
    {
    case OId_NodeIO:
        {
        strcat(result,TagSeparator);
        strcat(result,IOAreas[Inx].Name);
        break;
        };
    default:;
    };
  */
  ASSERT(strlen(result)<TagBuffLen);
  return result;
  };

////--------------------------------------------------------------------------
//
//pchar CTNode::TypeString(Strng& TypeStr, flag Clr)
//  {
//  if (Clr)
//    TypeStr = "";
//  if (TypeStr.Len()>0)
//    TypeStr[TypeStr.Len()-1] = 0;
//  return TypeStr();
//  }
//
//--------------------------------------------------------------------------

void CTNode::Remove_MyChildren()
  {
  while (MyChildren)
    delete MyChildren;
  LastChild = NULL;
  };

//--------------------------------------------------------------------------

void CTNode::Remove_Child(CTNode* pRem)
  {
  if (MyChildren)
    {
    if (MyChildren==pRem)
      {
      if (LastChild==MyChildren)
        LastChild=pRem->Nxt;
      MyChildren=pRem->Nxt;
      }
    else
      {
      CTNode* p=MyChildren;
      while (p && p->Nxt!=pRem)
        p=p->Nxt;
      if (p)  
        {
        // and now p->Nxt==pRem
        if (LastChild==pRem)
          LastChild=p;
        p->Nxt=pRem->Nxt;
        }
      }
    }

  if (DoLookUp)
    pChildMap->RemoveKey(pRem->Tag());
  
  }

//--------------------------------------------------------------------------

void CTNode::Add_Child(CTNode* pAdd)
  {
  pAdd->MyParent = this;
  if (MyChildren == NULL)
    {
    MyChildren  = pAdd;
    LastChild = pAdd;
    }
  else
    {
    //CTNode* pe = MyChildren;
    //while (pe->Nxt) 
    //  pe = pe->Nxt;
    //pe->Nxt = pAdd;
    LastChild->Nxt = pAdd;
    LastChild = pAdd;
    };
  if (DoLookUp)
    {
    if (pChildMap==NULL)
      {
      // to comeout
      //int xxx=FindNextPrimeNumber(1000);
      //DoBreak(); 

      pChildMap = new CNdTagMap(100);
      pChildMap->InitHashTable(FindNextPrimeNumber(1000));
      }
  
    CTNode* pX;
    ASSERT_ALWAYS(!pChildMap->Lookup(pAdd->Tag(), pX),"Tag already in ChildMap");
    pChildMap->SetAt(pAdd->Tag(), pAdd);
    }
  }

//--------------------------------------------------------------------------

char CTNode::GetObj_All(CTNode* &p)
  {
  if (p == NULL)
    p = this;
  else if (p == this)
    p = this->MyChildren;
  else if (p->MyChildren)
    p = p->MyChildren;
  else  if (p->Nxt)
    p = p->Nxt;
  else
    {
    while ((p->MyParent != this) && (p->MyParent->Nxt == NULL))
      p = p->MyParent;
    if (p->MyParent != this)
      p = p->MyParent->Nxt;
    else
      p = NULL;
    };

  return(p!=NULL);
  };

//--------------------------------------------------------------------------

CTNode* CTNode::Insert(CTNode* p, char IsInternal)
  {
  if (p==NULL)
    return NULL;

  //CTNode* pe;

  #if dbgInitObj
  dbgpln("Insert : %s%s%s",Tag,TagSeparator,p->Tag);
  #endif

  Add_Child(p);
  /*
  p->MyParent = this;
  if (MyChildren == NULL)
    MyChildren  = p;
  else
    {
    pe = MyChildren;
    while (pe->Nxt) pe = pe->Nxt;
    pe->Nxt = p;
    };
  */
  p->Internal = IsInternal;

  #if xdbgInitObj
  dbgpln("Insert : %s%s%s",Tag,TagSeparator,p->Tag);
  #endif

  return(p);
  };

//--------------------------------------------------------------------------

pchar CTNode::InternalNodeTag()
  {
  const int TagBuffLen = 128;
  static char FullTagTmp[4][TagBuffLen];
  static char FullTagInx = 0;
  pchar result = FullTagTmp[FullTagInx];
  CTNode* p[MaxTagDepth],*q;

  p[0]=NULL;
  q=this;
  int i = 0;
  while (q && q->Internal)
    {
    ASSERT(i<MaxTagDepth);
    p[i++] = q;
    q = q->MyParent;
    }
  i--;
  while (i>=0 && (strlen(p[i]->Tag()) == 0)) i--;
  strcpy(result, "");
  if (i>=0)
    {
    strcat(result,p[i--]->Tag());
    while (i >= 0)
      {
      strcat(result,TagSeparator);
      strcat(result,p[i--]->Tag());
      }
    }
  ASSERT(strlen(result)<TagBuffLen);
  return result;
  };

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void CTNode::ShowStruct(char LclCall)
  {
  CTNode* p;
  if (!LclCall)
    dbgpln("==============================================================");

  dbgpln("FlwSolverTag : %s",FullTag());

  p = NULL;
  while (GetObj(p))
    p->ShowStruct(1);

  if (!LclCall)
    dbgpln("==============================================================");
  };

//--------------------------------------------------------------------------
/*
CTNode* CTNode::FindObjId(long RqdId, CTNode* ps)
  {
  while (GetObj_All(ps) && (ps->IdNo() != RqdId));
  return ps;
  };
**/
//--------------------------------------------------------------------------

CTNode* CTNode::FindObj(pchar TagIn)
  {
  SCDPartTag FirstPart;
  pchar  Remainder;
  CTNode*  p = NULL;

  if (TagIn)
    {
    SplitTag(TagIn, FirstPart, Remainder);
    p = FindChildTag(FirstPart);
    if (Remainder && p)
      p=p->FindObj(Remainder);
    }

  return p;
  };

//--------------------------------------------------------------------------

CTNode* CTNode::FindChildTag(pchar pTag)
  {
  if (DoLookUp)
    {
    CTNode* p;
    if (pChildMap && pChildMap->Lookup(pTag, p))
      return p;
    return NULL;
    }
  else
    {
    CTNode* p = NULL;
    while (GetObj(p) && (stricmp(p->Tag(), pTag) != 0));
    return p;
    }
  }

//--------------------------------------------------------------------------

void CTNode::SplitTag(pchar TagIn, SCDPartTag &FirstPart, pchar &Remainder)
  {
  pchar p = TagIn+strspn(TagIn, " ");
  pchar rs = strstr(&p[1], TagSeparator);
  if (rs)
    {
    int i = (rs-p);
    strncpy((pchar)&FirstPart[0], p, i);
    FirstPart[i] = 0;
    rs++;
    Remainder = rs+strspn(rs, " ");
    }
  else
    {
    strcpy((pchar)&FirstPart[0], p);
    Remainder = NULL;
    }
  }
           
//--------------------------------------------------------------------------

void CTNode::MyTagsHaveChanged(bool FirstOne)
  {
  RequestUpdateXRefLists();
  ExecObj()->XBuildDataLists();

  TaggedObject::MyTagsHaveChanged(FirstOne);
  }

//--------------------------------------------------------------------------

void CTNode::Print_Node_Stats()
  {
  //dbgpln("================== CTNode Stats ==================");
  //dbgpln("Id  Name                 :  No Size    Total");
  //word LastId=0xffff;
  //for (word Id=0;Id<NoofNdStats;Id++)
  //  {
  //  if ((LastId >= 0) && (LastId < Id-1))
  //    dbgpln("-----");
  //  LastId=Id;
  //  }
  //dbgpln("====================================");
  };

//--------------------------------------------------------------------------

void CTNode::Print_Family(int Lvl)
  {
  if (Lvl==0)
    dbgpln("================== Family ==================");
  dbgindent(2);
  CTNode* p = NULL;
  while (GetObj(p))
    p->Print_Family(Lvl+2);
  dbgindent(-2);
  if (Lvl==0)
    dbgpln("====================================");
  };


//==========================================================================

void CTNode::CollectElectrics(CNodeElectricsArray & TSCA)
  { 
  //dbgpln("ConnectElectrics : %s ---------------------------------------------", FullObjTag());
  m_Electrics.ResetTermStripCount();
  TSCA.Add(this, &m_Electrics);
  };

//--------------------------------------------------------------------------

void CTNode::ConnectElectrics()
  { 
  };

//--------------------------------------------------------------------------

flag CTNode::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  //switch (No-CBContext())
  //  {
  //  //case  1: pS="W\tPGM reload required";       return 1;
  //  case  2: pS="E\tBad external tag references"; return 1;
  //  case  3: pS="W\tEngineering units invalid"; return 1;
  //  default:                                               
      return TaggedObject::CIStrng(No, pS);
  //  }
  }

//==========================================================================
