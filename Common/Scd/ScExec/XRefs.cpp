//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __XREFS_CPP
#define WITHCNVINDEXOPS 1
#include "pgm_e.h"
#include "xrefs.h"
#include "ndtree.h"
#include "dbgmngr.h"
//#include "optoff.h"

#define dbgXRefs     (WITHDEBUG)

#if dbgXRefs
static CDbgMngr dbgConstructTagRef  ("XRefs",  "ConstructTagRef");
static CDbgMngr dbgConstructXRef    ("XRefs",  "ConstructXRef");
static CDbgMngr dbgConstructXRef1   ("XRefs",  "ConstructXRef1");
static CDbgMngr dbgFind             ("XRefs",  "Find");
static CDbgMngr dbgAdd              ("XRefs",  "Add");
static CDbgMngr dbgRemove           ("XRefs",  "Remove");
static CDbgMngr dbgAdd2Me           ("XRefs",  "Add2Me");
static CDbgMngr dbgRemove2Me        ("XRefs",  "Remove2Me");
static CDbgMngr dbgSet              ("XRefs",  "Set");
static CDbgMngr dbgGet              ("XRefs",  "Get");
static CDbgMngr dbgReBuild          ("XRefs",  "ReBuild");
static CDbgMngr dbgTagRefListMgmt   ("XRefs",  "TagRefListMgmt");
static CDbgMngr dbgXRefListMgmt     ("XRefs",  "XRefListMgmt");
//static CDbgMngr dbgReMapList        ("XRefs",  "ReMapList");
static CDbgMngr dbgLocking          ("XRefs",  "Locking");
#define PUSHIN(x)  dbgindent(2,x);
#define POPIN(x)   dbgindent(-2,x);
#else
#define PUSHIN(x)  
#define POPIN(x)   
#endif

//===========================================================================
//
//
//
//===========================================================================

CXRefValue::CXRefValue()
  {
  m_dValue      = dNAN; 
  m_bChanged    = false;
  m_bTouched    = false;
  m_bWasTouched = false;
#if WithIOChgMonitor     
  m_dValMem     = 0;
#endif
  m_lForceCount = 0;
  };

//===========================================================================
//
//
//
//===========================================================================

CGlblXRefMngr * CTagRef::gs_TagRefGlblFn=NULL;
long CTagRef::m_nAllocNo=0;

IMPLEMENT_SPARES(CTagRef, 2000);

CTagRef::CTagRef()
  {
  m_nMyAllocNo=m_nAllocNo++;
  #if dbgXRefs
  if (dbgConstructTagRef())
    dbgpln("CTagRef()           %5i %08x", m_nMyAllocNo, this);
  #endif

  m_pOwner        = NULL;
  m_bInUse        = true;
  m_bIsLcl        = false;

  m_nSets         = 0;
  m_nSetsTot      = 0;
  m_nResets       = 0;
  m_nGets         = 0;
  m_nGetsTot      = 0;
  m_bMustSet      = false;
  m_bMustGet      = false;
  m_bIsFnTerm     = false;
  m_bIsNdActive   = true;
  m_iIOId         = -1;
  m_iIOInx        = -1;

  m_iType         = 0;
  m_iCnvIndex     = 0;
  m_posRefList    = NULL;
  m_posRefMap     = NULL;
  }

//--------------------------------------------------------------------------

CTagRef::~CTagRef()
  {
  #if dbgXRefs
  if (dbgConstructTagRef())
    dbgpln("~CTagRef()          %5i %08x", m_nMyAllocNo, this);
  #endif
  PUSHIN("CTagRef::~CTagRef");
  CTagRef::RemoveMe();
  POPIN("CTagRef::~CTagRef");
  }

//--------------------------------------------------------------------------

void CTagRef::RemoveMe()
  {
  #if dbgXRefs
  if (dbgConstructTagRef())
    dbgpln("CTagRef:RemoveMe    %5i %08x", m_nMyAllocNo, this);
  #endif
  PUSHIN("CTagRef::RemoveMe");
  gs_TagRefGlblFn->RemoveMe(this);
  if (m_pOwner)
    {
    long n=m_pOwner->m_IOTagRefs.GetSize();
    for (int i=0; i<n; i++)
      if (m_pOwner->m_IOTagRefs[i]==this)
        m_pOwner->m_IOTagRefs[i]=NULL;

    for (int j=0; j<m_pOwner->m_TagRefsOwnedByMe.GetSize(); j++)
      {
      if (m_pOwner->m_TagRefsOwnedByMe[j]==this)
        {
        m_pOwner->m_TagRefsOwnedByMe.RemoveAt(j);
        goto Done;
        }
      }
Done:;
    }
  POPIN("CTagRef::RemoveMe");
  }

//--------------------------------------------------------------------------

bool CTagRef::SameReference(CTagRef* Other)
  {              
  if (m_pOwner    != Other->m_pOwner    ) return false;
  if (m_iIOId     != Other->m_iIOId     ) return false;
  if (m_iIOInx    != Other->m_iIOInx    ) return false;
  return true;
  };

//===========================================================================
//
//
//
//===========================================================================

long CXRefItem::m_nAllocNo=0;

IMPLEMENT_SPARES(CXRefItem, 2000);

CXRefItem::CXRefItem(bool LockIt)
  {
  m_nMyAllocNo=m_nAllocNo++;
  #if dbgXRefs
  if (dbgConstructXRef())
    dbgpln("CXRefItem()         %5iXR %08x %s", m_nMyAllocNo, this, LockIt?"LOCK":"");
  #endif

  m_pOther          = NULL;
  m_pSrchRoot       = NULL;
  m_iIOId           = -1;
  m_bInUse          = true;
  m_bLclRqd         = true;//false;
  m_bIsTear         = false;
  m_bIsEvalSeqStart = false;
  m_bDynTag         = false;
  m_eReach          = XR_Null;

  m_Lcl.m_bIsLcl    = true;      

  //m_lForceCount     = 0;

//  m_pValue          = pValue;

  //m_dValue          = 0;
  //m_bChanged        = false;
  //m_bTouched        = false;
  //m_bWasTouched     = false;

  m_pMngr           = NULL;
  m_pStatus         = NULL;

//#if WithIOChgMonitor
//  m_dValMem  = 0;
//#endif
  m_posXRefList     = NULL;
  InitializeCriticalSectionAndSpinCount(&m_LockSect, 4000);
  if (LockIt)
    Lock();
  };

//--------------------------------------------------------------------------

CXRefItem::CXRefItem(bool LockIt, bool MustSet, bool MustGet)
  {
  m_nMyAllocNo=m_nAllocNo++;
  #if dbgXRefs
  if (dbgConstructXRef())
    dbgpln("CXRefItem()         %5iXR %08x %s", m_nMyAllocNo, this, LockIt?"LOCK":"");
  #endif

  m_pOther          = NULL;
  m_pSrchRoot       = NULL;
  m_iIOId           = -1;
  m_bInUse          = true;
  m_bLclRqd         = true;//false;
  m_bIsTear         = false;
  m_bIsEvalSeqStart = false;
  m_bDynTag         = false;
  m_eReach          = XR_Null;


  m_Lcl.m_bIsLcl    = true;      

  //m_lForceCount     = 0;

  //m_pValue          = pValue;

  //m_dValue          = 0;
  //m_bChanged        = false;
  //m_bTouched        = false;
  //m_bWasTouched     = false;

  m_pMngr           = NULL;
  m_pStatus         = NULL;

  m_bMustSet        = MustSet;
  m_bMustGet        = MustGet;

//#if WithIOChgMonitor
//  m_dValMem  = 0;
//#endif
  m_posXRefList     = NULL;
  InitializeCriticalSectionAndSpinCount(&m_LockSect, 4000);
  if (LockIt)
    Lock();
  };

//--------------------------------------------------------------------------

CXRefItem::~CXRefItem()
  {
  #if dbgXRefs
  if (dbgConstructXRef())
    dbgpln("~CXRefItem()        %5iXR %08x s:%08x d:%08x ", m_nMyAllocNo, this, SrcNd(), DstNd());
  #endif
  PUSHIN("CXRefItem::~CXRefItem");
  CXRefItem::RemoveMe();
  DeleteCriticalSection(&m_LockSect);
  POPIN("CXRefItem::~CXRefItem");
  }

//--------------------------------------------------------------------------
#ifndef _RELEASE
void CXRefItem::Lock()
  {
  #if dbgXRefs
  if (dbgLocking())
    dbgpln("  LockXRef     %5iXR [%4i:%3i.%3i.%4x]{%s/%s}%s", 
        m_nMyAllocNo, 
        GetCurrentThreadId(), 
        m_LockSect.LockCount, 
        m_LockSect.RecursionCount, 
        m_LockSect.OwningThread, 
        m_pOwner?m_pOwner->FullObjTag():"---",
        m_pMngr?m_pMngr->GetOwnerTag():"---",
        m_TAB.sTag());
  #endif

  EnterCriticalSection(&m_LockSect); 
  
  #if dbgXRefs
  if (dbgLocking())
    dbgpln("  Done         %5iXR [%4i:%3i.%3i.%4x]", 
      m_nMyAllocNo, 
      GetCurrentThreadId(), 
      m_LockSect.LockCount, 
      m_LockSect.RecursionCount, 
      m_LockSect.OwningThread);
  #endif
  };
//--------------------------------------------------------------------------

void CXRefItem::UnLock()
  {
  #if dbgXRefs
  if (dbgLocking())
    dbgpln("  UnLockXRef   %5iXR [%4i:%3i.%3i.%4x]{%s/%s}%s", 
      m_nMyAllocNo, 
      GetCurrentThreadId(), 
      m_LockSect.LockCount, 
      m_LockSect.RecursionCount, 
      m_LockSect.OwningThread, 
      m_pOwner?m_pOwner->FullObjTag():"---",
      m_pMngr?m_pMngr->GetOwnerTag():"---",
      m_TAB.sTag());
  #endif    

  if (m_LockSect.LockCount<0)    {
    int cccc=0;
    }
  LeaveCriticalSection(&m_LockSect); 

  #if dbgXRefs
  if (dbgLocking())
    dbgpln("  Done         %5iXR [%4i:%3i.%3i.%4x]", 
      m_nMyAllocNo, 
      GetCurrentThreadId(), 
      m_LockSect.LockCount, 
      m_LockSect.RecursionCount, 
      m_LockSect.OwningThread);
  #endif
  }
#endif
//--------------------------------------------------------------------------

void CXRefItem::RemoveMe()
  {
  #if dbgXRefs
  if (dbgConstructXRef1())
    dbgpln("CXRefItem:RemoveMe  %5iXR %08x s:%08x d:%08x ", m_nMyAllocNo, this, SrcNd(), DstNd());
  #endif
  PUSHIN("CXRefItem::RemoveMe");
  if (SrcNd())
    SrcNd()->RemoveXRef2Me(this);
  if (DstNd())
    DstNd()->RemoveXRef2Me(this);
  gs_TagRefGlblFn->RemoveMe(this);

  if (m_pMngr)
    {
    for (int i=0; i<3; i++)
      {
      for (int j=0; j<m_pMngr->m_XRefsOwnedByMe[i].GetSize(); j++)
        {
        if (m_pMngr->m_XRefsOwnedByMe[i][j]==this)
          {
          m_pMngr->m_XRefsOwnedByMe[i].RemoveAt(j);
          //for (int k=j; k<m_pMngr->m_XRefsOwnedByMe[i].GetSize()-1; k++)
          //  m_pMngr->m_XRefsOwnedByMe[i][k]=m_pMngr->m_XRefsOwnedByMe[i][k+1];
          //m_pMngr->m_XRefsOwnedByMe[i].SetSize(m_pMngr->m_XRefsOwnedByMe[i].GetSize()-1);
          goto Done;
          }
        }
      }
Done:;
    }
  
  POPIN("CXRefItem::RemoveMe");
  m_pOwner=NULL;
  m_pOther=NULL;
  }

//--------------------------------------------------------------------------

void CXRefItem::CopyToLcl()
  {
  m_Lcl.m_sRefTag   = m_sRmtTag;
  m_Lcl.m_sRefID    = m_sRmtID;
  m_Lcl.m_sRmtTag   = m_sRefTag;
  m_Lcl.m_sRmtID    = m_sRefID;
  m_Lcl.m_bMustGet  = m_bMustSet;
  m_Lcl.m_bMustSet  = m_bMustGet;
  m_Lcl.m_iCnvIndex = m_iCnvIndex;
  m_Lcl.m_sCnv      = m_sCnv;
  m_Lcl.m_iType     = m_iType;
  m_Lcl.m_pOwner    = m_pOwner;
  };

//--------------------------------------------------------------------------

bool CXRefItem::SameReference(CXRefItem * Other)
  {              
  if (m_sRmtTag   != Other->m_sRmtTag   ) return false;
  if (m_sRmtID    != Other->m_sRmtID    ) return false;
  if (m_sRefTag   != Other->m_sRefTag   ) return false;
  if (m_sRefID    != Other->m_sRefID    ) return false;
  if (m_bMustSet  != Other->m_bMustSet  ) return false;
  if (m_bMustGet  != Other->m_bMustGet  ) return false;
  if (m_bIsFnTerm != Other->m_bIsFnTerm ) return false;
  if (m_iCnvIndex != Other->m_iCnvIndex ) return false;
  if (m_sCnv      != Other->m_sCnv      ) return false;
  if (m_iType     != Other->m_iType     ) return false;
  if (m_pOwner    != Other->m_pOwner    ) return false;
  if (m_iIOId     != Other->m_iIOId     ) return false;
  if (m_iIOInx    != Other->m_iIOInx    ) return false;
  return true;
  };

//--------------------------------------------------------------------------

int CXRefItem::ResolveNearXRef(pTaggedObject pSrchRoot, char* pTag, CTNode* Owner)
  {
//CStopWatch SW;
//SW.Start();
  //m_bNdPtrsValid=false;
  m_pOwner = Owner;
  m_pSrchRoot = pSrchRoot;
  m_TAB.Init(pSrchRoot, pTag, TABOpt_AllInfoOnce/*TABOpt_Parms*/, TU_IndividuallyUnique);
  int Ret=FXR_NotFound;
  if (m_TAB.LoadAddress())
    {
    TaggedObject *p=m_TAB.pAccObj;
    m_pOther=NULL;
    while (!m_pOther && p)
      m_pOther=dynamic_cast<CTNode*>(p);

    m_iType = m_TAB.Type();
    m_iCnvIndex = m_TAB.CnvIndex();
    //m_bNdPtrsValid=true;
    m_eReach=XR_Near;

    Ret=FXR_Found;
    if (!IsNumData(m_iType) && !IsStrng(m_iType))
      Ret|=FXR_BadValue;
    if (m_bMustSet)
      {
      if ((m_TAB.Flags() & DDEF_PARAM)==0)
        Ret|=FXR_NotParam;
      if (m_TAB.Flags() & DDEF_PARAMSTOPPED)
        Ret|=FXR_ParamStopped;
      }
    if (m_TAB.IsNumData())
      m_Value.m_dValue=m_TAB.GetDouble();
    else if (m_TAB.IsStrng())
      m_Value.m_sValue=m_TAB.GetString();
    }
  
  if (m_bDynTag)
    Ret|= FXR_DynTag;
 
//double xx=SW.LapTime();
//dbgpln("  %16.2f : %s", xx*1e6, m_sRefTag());
#if dbgXRefs
  if (dbgFind(m_sRefTag()))
    {
    dbgp("FndNearXRefValue %08x : %s         ", this, (Ret&FXR_Found)?"Found":(Ret&FXR_DynTag)?"DynTg":"     ");
    dbgpln("                               =  %16.5f %-20s -> %s", m_Value.m_dValue, m_sRefTag(), m_sRmtTag());
    }
#endif
  return Ret;
  }

//--------------------------------------------------------------------------

int CXRefItem::ResolveFarXRef(CExecObj *pExecObj, char* pTag, CTNode* Owner)
  {
  //m_bNdPtrsValid=false;
  m_pOwner = Owner;
  m_pOther = NULL;
  m_pSrchRoot = NULL;
  CXM_ObjectTag  ObjTag(pTag, TABOpt_AllInfoOnce);//TABOpt_Parms);
  CXM_ObjectData ObjData(0,0);//,NULL);
  CXM_Route      ObjRoute;
  int Ret=FXR_NotFound;

  if (pExecObj->XReadTaggedItem(ObjTag, ObjData, ObjRoute))
    {
    //bNearRef = 0;
    CPkDataItem* pPItem = ObjData.FirstItem();
    m_iType = pPItem->Type();
    m_iCnvIndex = pPItem->CnvIndex();
    //m_bNdPtrsValid=true;
    m_eReach=XR_Far;

    Ret=FXR_Found;
    if (!IsNumData(m_iType) && !IsStrng(m_iType))
      Ret|=FXR_BadValue;
    if ((pPItem->Flags() & DDEF_PARAM)==0)
      Ret|=FXR_NotParam;
    if (pPItem->Flags() & DDEF_PARAMSTOPPED)
      Ret|=FXR_ParamStopped;
    }
  if (m_bDynTag)
    Ret |= FXR_DynTag;

  #if dbgXRefs
  if (dbgFind(m_sRefTag()))
    {
    dbgp("FndFarXRefValue  %08x : %s         ", this, (Ret&FXR_Found)?"Found":(Ret&FXR_DynTag)?"DynTg":"     ");
    dbgpln("                               =  %16.5f %-20s -> %s", m_Value.m_dValue, m_sRefTag(), m_sRmtTag());
    }
  #endif
  return Ret;
  }

//--------------------------------------------------------------------------
/*
int CTagRef::ResolveIORef(int IOId, CTNode* Owner)
  {
  //m_bNdPtrsValid=false;
  //m_pOther = NULL;
  //m_pSrchRoot = NULL;

  Strng ID, Tg;
  bool IsIn, IsOut;
  int Inx=Owner->GetCIONo(IOId, ID, Tg, IsIn, IsOut);
  if (Inx>=0)
    {
    m_pOwner = Owner;
    m_iIOId=IOId;
    m_iIOInx=Inx;
    m_bMustGet=IsOut;
    m_bMustSet=IsIn;
    m_sRmtID=ID;
    m_sRmtTag=Tg;
    m_iType = tt_Double;
    m_iCnvIndex = 0;
    //m_bNdPtrsValid=true;
    //m_eReach=XR_IO;
    return FXR_Found;
    }
  return FXR_NotFound;
  }
*/
//--------------------------------------------------------------------------

bool CXRefItem::Validate()
  {
  bool OK=true;
  //#if dbgXRefs
  //if (dbgReBuild())
  //  {
  //  dbgpln("CXRefItem::Validate %s %s", OK?"OK ":"Bad", RefTag());
  //  };
  //#endif
  return OK;
  };

//--------------------------------------------------------------------------

bool CXRefItem::GetNearXRefValue()
  {
  if (m_bMustGet && m_TAB.LoadAddress())
    {
    if (m_TAB.pAccObj->GetActive())
      {
      SetNdActive(true);
      m_Lcl.SetNdActive(true);
      }
    else
      {
      SetNdActive(false);
      m_Lcl.SetNdActive(false);
      }
    if (m_TAB.IsNumData())
      {
      m_Value.m_dValue=m_TAB.GetDouble();
      IncGets();
      m_Lcl.IncSets();
      }
    else if (m_TAB.IsStrng())
      {
      m_Value.m_sValue=m_TAB.GetString();
      IncGets();
      m_Lcl.IncSets();
      }
    #if dbgXRefs
    if (dbgGet(RefTag()))
      dbgpln("Xfer Near Get    %08x :                {%2i   /%4i}{%2i:%2i/%4i%s} <-- %16.5f %-20s -> %s", 
             this, m_nGets, m_nGetsTot, m_Lcl.m_nSets, m_Lcl.m_nResets, m_Lcl.m_nSetsTot, m_Lcl.m_bIsFnTerm?"F":" ", 
             m_Value.m_dValue, m_sRefTag(), m_sRmtTag());
    #endif
    return true;
    }
  return false;
  }

//--------------------------------------------------------------------------
bool LogXRefResets = true; //TODO: this should be a user option!!!!?

bool CXRefItem::SetNearXRefValue()
  {
  if (m_bMustSet && (m_pStatus==NULL || m_pStatus->IsXRefActive()))
    {
    DDEF_Flags Flgs=m_TAB.Flags();
    bool IsOneShotBtn = ((Flgs&(DDEF_BUTTON|DDEF_CHECKBOX))==DDEF_BUTTON);

    #if dbgXRefs
    if (dbgSet(RefTag()))
      {
      dbgp("Xfer Near Set %s%s %08x : %s%s Frc:%2i ",
            (Flgs&DDEF_BUTTON)?"B":" ",(Flgs&DDEF_CHECKBOX)?"C":" ", this, 
            m_Value.m_bTouched?"Tch":"   ",m_Value.m_bChanged?"Chg":"   ", ForceCount());
      }
    #endif
    PUSHIN("CXRefItem::SetNearXRefValue");

    bool Touched=m_Value.m_bTouched;
    bool WasTouched=m_Value.m_bWasTouched;
    m_Value.m_bWasTouched=m_Value.m_bTouched;
    m_Value.m_bTouched=false;

    //if ((Touched && !WasTouched) || HasChanged() || (m_Value.m_lForceCount!=0))
    if ((Touched && HasChanged()) || (m_Value.m_lForceCount!=0))
      {
      if (m_Value.m_lForceCount>0)
        m_Value.m_lForceCount--;
      if (m_TAB.LoadAddress() && (m_TAB.IsNumData() || m_TAB.IsStrng()))
        {
        ClearChanged();
        if (m_TAB.IsNumData())
          m_TAB.PutDouble(m_Value.m_dValue);
        else
          m_TAB.PutString(m_Value.m_sValue());
        IncSets();
        m_Lcl.IncGets();
        if (m_TAB.pAccObj->GetActive())
          {
          SetNdActive(true);
          m_Lcl.SetNdActive(true);
          }
        else
          {
          SetNdActive(false);
          m_Lcl.SetNdActive(false);
          }
        
        #if dbgXRefs
        if (dbgSet(RefTag()))
          dbgpln(" {%2i:%2i/%4i}{%2i   /%4i } ==> %16.5f %-20s -> %s", 
                 m_nSets, m_nResets, m_nSetsTot, m_Lcl.m_nGets, m_Lcl.m_nGetsTot, 
                 m_Value.m_dValue, m_sRefTag(), m_sRmtTag());
        #endif
        POPIN("CXRefItem::SetNearXRefValue");
        if (IsOneShotBtn)
          { 
          // reset the value - the remote value will also return to zero
          m_Value.m_dValue=0;
          }
        return true;
        }
      }
    else 
      {
      //if (Touched && !IsOneShotBtn) // OneShot
      if (Touched) // No Change but force current value
        {
        if (m_TAB.LoadAddress() && m_TAB.IsNumData())
          {
          double d1=m_Value.m_dValue;
          double d2=IsOneShotBtn ? 0 : m_TAB.GetDouble();
          if (!Finite(d1) && !Finite(d2))
            goto TheSame;
          bool BothFinite=(Finite(d1) && Finite(d2));
          if (BothFinite)
            {
            if (InRange((double)-FLT_MAX, d1, (double)FLT_MAX) && InRange((double)-FLT_MAX, d2, (double)FLT_MAX))
              {
              float f1=(float)d1; // the typecast allows for dest vars to be floats
              float f2=(float)d2;
              if (f1==f2)  //Surely this should have a tolerance!!! 
                goto TheSame;
              }
            else
              {
              if (d1==d2)  //Surely this should have a tolerance!!! 
                goto TheSame;
              }
            }

          ClearChanged();
          m_TAB.PutDouble(d1);
          IncResets();
          m_Lcl.IncGets();
          #if dbgXRefs
          if (dbgSet(RefTag()))
            dbgpln(" {%2i:%2i/%4i}{%2i   /%4i } >>> %16.5f %-20s -> %s", 
                  m_nSets, m_nResets, m_nSetsTot, m_Lcl.m_nGets, m_Lcl.m_nGetsTot, 
                  d1, m_sRefTag(), m_sRmtTag());
          #endif
          POPIN("CXRefItem::SetNearXRefValue");
          if (!IsOneShotBtn && LogXRefResets && fabs(d1-d2)>1.0e-6*Max(fabs(d1), fabs(d2)))
            {
            if (m_TAB.CnvIndex()>0)
              {
              d1=Cnvs[m_TAB.CnvIndex()]->Normal(d1, m_TAB.sCnvTxt());
              d2=Cnvs[m_TAB.CnvIndex()]->Normal(d2, m_TAB.sCnvTxt());
              }
            LogNote(m_TAB.sTag(), 0, "Value reset to %12.6f %s [was %12.6f]", d1, m_TAB.sCnvTxt()?m_TAB.sCnvTxt():"", d2);
            }
          if (IsOneShotBtn)
            { 
            // reset the value - the remote value will also return to zero
            m_Value.m_dValue=0;
            }
          return true;
TheSame:;
          //return true;
          }
        }
      }

    #if dbgXRefs
    if (dbgSet(RefTag()))
      dbgpln(" {%2i:%2i/%4i}{%2i   /%4i }  =  %16.5f %-20s -> %s", 
             m_nSets, m_nResets, m_nSetsTot, m_Lcl.m_nGets, m_Lcl.m_nGetsTot, 
             m_Value.m_dValue, "", m_sRefTag(), m_sRmtTag());
    #endif
    if (IsOneShotBtn)
      { 
      // reset the value - the remote value will also return to zero
      m_Value.m_dValue=0;
      }
    POPIN("CXRefItem::SetNearXRefValue");
    }
  return false;
  }

//--------------------------------------------------------------------------

CTNode * CXRefItem::SrcNd() const
  {
  //if (!m_bNdPtrsValid)
  //  return NULL;
  if (m_bMustSet)
    return m_pOwner;
  else
    return m_pOther;
  };

//--------------------------------------------------------------------------

CTNode * CXRefItem::DstNd() const
  {
  //if (!m_bNdPtrsValid)
  //  return NULL;
  if (!m_bMustSet)
    return  m_pOwner;
  else
    return m_pOther;
  };

//--------------------------------------------------------------------------

void CTagRef::IncSets()
  {
  m_nSets=Min(m_nSets+1, 254);
  m_nSetsTot=Min(m_nSetsTot+1, 254);
  };
void CTagRef::IncResets()
  {
  m_nResets=Min(m_nResets+1, 254);
  m_nSetsTot=Min(m_nSetsTot+1, 254);
  };
void CTagRef::IncGets()
  {
  m_nGets=Min(m_nGets+1, 254);
  m_nGetsTot=Min(m_nGetsTot+1, 254);
  };
void CTagRef::ClearCnts()
  {
  //dbgpln("CTagRef::Clear %3i %3i %s", m_nSets, m_nGets, RefTag());
  m_nSets=0;
  m_nResets=0;
  m_nGets=0;
  };
void CTagRef::ClearTotal()
  {
  ClearCnts();
  m_nSetsTot=0;
  m_nGetsTot=0;
  };

//--------------------------------------------------------------------------

LPCTSTR CXRefItem::SFGNodeTag(TaggedObject * pRefTo)
  {
  if (/*&m_bNdPtrsValid &&*/ m_pOwner)
    {
    if (pRefTo!=NULL)
      {
      if (pRefTo==m_pOwner)
        m_sSFGTag=m_TAB.sTag();
      else
        {
        if (m_bMustSet)
          m_sSFGTag.Set("%s->%s",m_pOwner->FullObjTag(), m_TAB.sTag());
        else
          m_sSFGTag.Set("%s->%s", m_TAB.sTag(),m_pOwner->FullObjTag());
        }
      }
    else
      {
      if (m_bMustSet)
        m_sSFGTag.Set("%s//%s",m_pOwner->FullObjTag(), m_TAB.sTag());
      else
        m_sSFGTag.Set("%s//%s", m_TAB.sTag(),m_pOwner->FullObjTag());
      }
    }
  else
    m_sSFGTag="? No Owner";

  return m_sSFGTag();
  };

//--------------------------------------------------------------------------

double CXRefItem::GetXRefValue(bool DoCnts)
  { 
  if (DoCnts)
    {
    IncGets();
    m_Lcl.IncSets();
    }
  #if dbgXRefs
  if (dbgSet())//RefTag()))
    {
    if (DoCnts)
      dbgpln("  GetXRefValue   %08x :                {%2i   /%4i}              <-- %16.5f %s -> %s", 
             this, m_nGets, m_nGetsTot, m_Value.m_dValue, m_pOwner?m_pOwner->GetOwnerTag():"", m_TAB.sTag());
    else                                      
      dbgpln("  GetXRefValue   %08x :                                          <-- %16.5f %s -> %s",
             this, m_Value.m_dValue, m_pOwner?m_pOwner->GetOwnerTag():"", m_TAB.sTag());
    }
  #endif

  return m_Value.m_dValue;
  }

//--------------------------------------------------------------------------

void CXRefItem::SetXRefValue(double V, bool DoCnts)
  { 
  if (DoCnts)
    {
    IncSets();
    m_Lcl.IncGets();
    }

  //if (!m_bTouched)
  //  m_bChanged=true; 
  m_Value.m_bTouched=true; 

  bool Valid1=Valid(m_Value.m_dValue);
  bool Valid2=Valid(V);
  if ((Valid1 && Valid2 && (m_Value.m_dValue!=V)) || (Valid1!=Valid2)) 
    {
    //IncChgs(); 
    m_Value.m_bChanged=true; 
    m_Value.m_dValue=V; 
    }; 
  #if dbgXRefs
  if (dbgSet())//RefTag()))
    {
    if (DoCnts)
      dbgpln("  SetXRefValue   %08x : %s%s         {%2i:%2i/%4i}              ==> %16.5f %s -> %s", 
              this, m_Value.m_bTouched?"Tch":"   ", m_Value.m_bChanged?"Chg":"   ",m_nSets, m_nResets, m_nSetsTot, m_Value.m_dValue, 
              m_pOwner?m_pOwner->GetOwnerTag():"", m_TAB.sTag());
    else
      dbgpln("  SetXRefValue   %08x : %s%s                                   ==> %16.5f %s -> %s", 
              this, m_Value.m_bTouched?"Tch":"   ", m_Value.m_bChanged?"Chg":"   ",m_Value.m_dValue, 
              m_pOwner?m_pOwner->GetOwnerTag():"", m_TAB.sTag());
    }
  #endif
  }

//--------------------------------------------------------------------------

char* CXRefItem::GetXRefStrValue(bool DoCnts)
  { 
  if (DoCnts)
    {
    IncGets();
    m_Lcl.IncSets();
    }
  #if dbgXRefs
  if (dbgSet())//RefTag()))
    {
    if (DoCnts)
      dbgpln("  GetXRefStrValue   %08x :                {%2i   /%4i}              <-- %16s %s -> %s", 
             this, m_nGets, m_nGetsTot, m_Value.m_sValue(), m_pOwner?m_pOwner->GetOwnerTag():"", m_TAB.sTag());
    else                                      
      dbgpln("  GetXRefStrValue   %08x :                                          <-- %16s %s -> %s",
             this, m_Value.m_sValue(), m_pOwner?m_pOwner->GetOwnerTag():"", m_TAB.sTag());
    }
  #endif

  return m_Value.m_sValue();
  }

//--------------------------------------------------------------------------

void CXRefItem::SetXRefStrValue(char* p, bool DoCnts)
  { 
  if (DoCnts)
    {
    IncSets();
    m_Lcl.IncGets();
    }

  //if (!m_bTouched)
  //  m_bChanged=true; 
  m_Value.m_bTouched=true; 

  const int len1 = m_Value.m_sValue.Len();
  const int len2 = p ? strlen(p) : 0;
  if (len1!=len2 || (len1>0 && _stricmp(p, m_Value.m_sValue())!=0)) 
    {
    //IncChgs(); 
    m_Value.m_bChanged=true; 
    m_Value.m_sValue=p ? p : ""; 
    }
  #if dbgXRefs
  if (dbgSet())//RefTag()))
    {
    if (DoCnts)
      dbgpln("  SetXRefStrValue   %08x : %s%s         {%2i:%2i/%4i}              ==> %16s %s -> %s", 
              this, m_Value.m_bTouched?"Tch":"   ", m_Value.m_bChanged?"Chg":"   ",m_nSets, m_nResets, m_nSetsTot, m_Value.m_sValue(), 
              m_pOwner?m_pOwner->GetOwnerTag():"", m_TAB.sTag());
    else
      dbgpln("  SetXRefStrValue   %08x : %s%s                                   ==> %16s %s -> %s", 
              this, m_Value.m_bTouched?"Tch":"   ", m_Value.m_bChanged?"Chg":"   ",m_Value.m_sValue(), 
              m_pOwner?m_pOwner->GetOwnerTag():"", m_TAB.sTag());
    }
  #endif
  }

//==========================================================================
//
//
//
//==========================================================================

int TryTestTag(TaggedObject* pSrchRoot, char* pTag, bool TestParamStopped/*=true*/)
  {
  Strng WrkTag,WrkCnv;
  TaggedObject::SplitTagCnv(pTag, WrkTag, WrkCnv);
  TagAccessBlk TAB;
  TAB.Init(pSrchRoot, WrkTag(), TABOpt_AllInfoOnce/*TABOpt_Parms*/, TU_IndividuallyUnique);
  int Ret=FXR_NotFound;
  if (TAB.LoadAddress())
    {
    CCnvIndex iCnvIndex = TAB.CnvIndex();

    Ret=FXR_Found;
    if (!IsNumData(TAB.Type()) && !IsStrng(TAB.Type()))
      Ret|=FXR_BadValue;
    if ((TAB.Flags() & DDEF_PARAM)==0)
      Ret|=FXR_NotParam;
    if (TestParamStopped && (TAB.Flags() & DDEF_PARAMSTOPPED))
      Ret|=FXR_ParamStopped;
    }
  return Ret;
  }

int TryWriteTag(TaggedObject* pSrchRoot, char* pTag, double Value, bool TestParamStopped/*=true*/)
  {
  //CStopWatch SW(true);
  Strng WrkTag,WrkCnv;
  TaggedObject::SplitTagCnv(pTag, WrkTag, WrkCnv);
  TagAccessBlk TAB;
  TAB.Init(pSrchRoot, WrkTag(), TABOpt_AllInfoOnce/*TABOpt_Parms*/, TU_IndividuallyUnique);
  int Ret=FXR_NotFound;
  if (TAB.LoadAddress())
    {
    CCnvIndex iCnvIndex = TAB.CnvIndex();

    Ret=FXR_Found;
    if (!IsNumData(TAB.Type()) && !IsStrng(TAB.Type()))
      Ret|=FXR_BadValue;
    if ((TAB.Flags() & DDEF_PARAM)==0)
      Ret|=FXR_NotParam;
    if (TestParamStopped && (TAB.Flags() & DDEF_PARAMSTOPPED))
      Ret|=FXR_ParamStopped;
    if (Ret==FXR_Found)
      {
      if (iCnvIndex && WrkCnv.Len()>0)
        {
        TAB.PutDouble(Cnvs[iCnvIndex]->Normal(Value, WrkCnv()));
        }
      else
        {
        TAB.PutDouble(Value);
        }
      }
    }

  //dbgpln("TryWriteTag D %10.2f %s ", 1e6*SW.Secs(), pTag);
  return Ret;
  }

int TryWriteTag(TaggedObject* pSrchRoot, char* pTag, char* pValue, bool TestParamStopped/*=true*/)
  {
  //CStopWatch SW(true);
  Strng WrkTag,WrkCnv;
  TaggedObject::SplitTagCnv(pTag, WrkTag, WrkCnv);
  TagAccessBlk TAB;
  TAB.Init(pSrchRoot, WrkTag(), TABOpt_AllInfoOnce/*TABOpt_Parms*/, TU_IndividuallyUnique);
  int Ret=FXR_NotFound;
  if (TAB.LoadAddress())
    {
    CCnvIndex iCnvIndex = TAB.CnvIndex();

    Ret=FXR_Found;
    if (!IsNumData(TAB.Type()) && !IsStrng(TAB.Type()))
      Ret|=FXR_BadValue;
    if ((TAB.Flags() & DDEF_PARAM)==0)
      Ret|=FXR_NotParam;
    if (TestParamStopped && (TAB.Flags() & DDEF_PARAMSTOPPED))
      Ret|=FXR_ParamStopped;
    if (Ret==FXR_Found)
      {
      if (IsNumData(TAB.Type()))
        {
        double dValue = 0.0;
        if (pValue && strlen(pValue)>0)
          {
          dValue = (pValue[0]=='*' ? dNAN : atof(pValue));
          }
        if (iCnvIndex && WrkCnv.Len()>0)
          {
          TAB.PutDouble(Cnvs[iCnvIndex]->Normal(dValue, WrkCnv()));
          }
        else
          {
          TAB.PutDouble(dValue);
          }
        }
      else
        {
        TAB.PutString(pValue ? pValue : "");
        }
      }
    }

  //dbgpln("TryWriteTag S %10.2f %s ", 1e6*SW.Secs(), pTag);
  return Ret;
  }

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_SPARES(CTagRefsMapItem, 1000);

CTagRefStatusWord CTagRefsMapItem::GetStatusWord()
  {
  //int dbg=0;
  //if (_stricmp(m_sTag, "MW_SR4_SEPARATION.GM.IOs.[M30P133].Splt.Other(Ru)")==0)// ||
  ////    _stricmp(m_sTag, "P105b.Stats.Reset_Stats")==0)
  //  { dbg=1; }

  CTagRefStatusWord W=FFM_Off;
  POSITION Pos=m_pTagRefs.GetHeadPosition();
  while (Pos)
    { 
    CTagRef * pRef=m_pTagRefs.GetNext(Pos);
    if (pRef->m_bMustSet)
      {
      if (W & FFM_HasSet)
        W|=FFM_HasMultiSets;
      W|=FFM_HasSet;
      }
    if (pRef->SetCnt()>0)
      {
      if ((W & FFM_HasSetCnt) && !pRef->m_bIsFnTerm)
        W|=FFM_HasMultiSetCnts;
      W|=FFM_HasSetCnt;
      }
    if (pRef->ResetCnt()>0)
      W|=FFM_HasReset;
    if (pRef->m_bMustGet)
      W|=FFM_HasGet;
    if (pRef->GetCnt()>0)
      W|=FFM_HasGetCnt;
    if (!pRef->IsNdActive())
      W|=FFM_HasInactive;
    }

  if (W & FFM_HasMultiSets)
    W&=~FFM_HasReset;

  //if (dbg)
  //  dbgpln("SW: %04x %s %s", W, m_sTag, m_sLwrTag);

  return W;
  };

//==========================================================================
//
//
//
//==========================================================================

CNodeXRefMngr::CNodeXRefMngr()
  {
  m_bXRefListActive=false;
  m_bXRefsValid=false;
  m_bUpdateXRefLists=false;
  m_bXRefListsChanged=false;

  m_bFnMngrInUse=false;
  m_pFnMngr=NULL;
  m_nXRefs2Me=0;
  m_NodeXRefsLockCount=0;
  }

//--------------------------------------------------------------------------

CNodeXRefMngr::~CNodeXRefMngr()
  {
  delete m_pFnMngr;
  }

//--------------------------------------------------------------------------

CFnMngr & CNodeXRefMngr::FnMngr()
  {
  if (!m_pFnMngr)
    {
    m_pFnMngr=new CFnMngr    ;
    ////TODO What about this
    m_pFnMngr->Reset(this);
    }
  return *m_pFnMngr;
  };

//--------------------------------------------------------------------------

void CNodeXRefMngr::FnMngrClear()
  { 
  m_bFnMngrInUse=false; 
  if (FnMngrPresent()) 
    FnMngr().Reset(this);
  }; 

//--------------------------------------------------------------------------

void CNodeXRefMngr::FnMngrRequired()
  { 
  m_bFnMngrInUse=true; 
  FnMngr(); // force Presence if neccessary
  }; 

//--------------------------------------------------------------------------

void CNodeXRefMngr::FnMngrTryUpdateXRefLists(CXRefBuildResults & Results)
  {
  if (m_bFnMngrInUse) 
    {
    #if dbgXRefs
    if (dbgReBuild())// && m_bXRefsValid)
      dbgpln("FnMngrTryUpdateXRefLists %s", GetOwnerTag());
    #endif
    PUSHIN("CNodeXRefMngr::FnMngrTryUpdateXRefLists");
    FnMngr().UpdateXRefLists(Results);
    POPIN("CNodeXRefMngr::FnMngrTryUpdateXRefLists");
    }
  };

//--------------------------------------------------------------------------

void CNodeXRefMngr::FnMngrTryUnlinkAllXRefs()
  {
  if (FnMngrPresent()) 
    {
    #if dbgXRefs
    if (dbgReBuild())// && m_bXRefsValid)
      dbgpln("FnMngrTryUnlinkAllXRefs %s", GetOwnerTag());
    #endif
    PUSHIN("CNodeXRefMngr::FnMngrTryUnlinkAllXRefs");
    FnMngr().UnlinkAllXRefs();
    POPIN("CNodeXRefMngr::FnMngrTryUnlinkAllXRefs");
    }
  }

//--------------------------------------------------------------------------

void CNodeXRefMngr::RequestUpdateXRefLists()
  {
  //#if dbgXRefs
  //if (dbgReBuild() && !m_bUpdateXRefLists)
  //  dbgpln("RequestUpdateXRefLists %s", GetOwnerTag());
  //#endif

  m_bUpdateXRefLists=true;
  };

//--------------------------------------------------------------------------

bool CNodeXRefMngr::SetXRefListActive(bool Active)                      
  {
#if dbgXRefs
  if (dbgReBuild())
    {
    if (Active != m_bXRefListActive) 
      dbgpln("SetXRefListActive : %s %s", Active?"ACTIVE":"      ", GetOwnerTag());
    }
#endif
  m_bXRefListActive = Active; 
  return m_bXRefListActive; 
  }
bool CNodeXRefMngr::XRefListActive()                                    
  { 
  //dbgpln("   XRefListActive : %s %s", m_bXRefListActive?"ACTIVE":"      ", GetOwnerTag());
  return m_bXRefListActive; 
  }
//--------------------------------------------------------------------------

long CNodeXRefMngr::UpdateXRefLists(bool SetXRefListsChangedFlag)
  {
  //CStopWatch SW;
  //SW.Start();
  #if dbgXRefs
  if (dbgReBuild())
    dbgpln("CNodeXRefMngr::UpdateXRefLists: %s ", GetOwnerTag());
  #endif
  PUSHIN("CNodeXRefMngr::UpdateXRefLists");

  UnlinkAllXRefs(); // disconnects
  //dbgpln("%16.2f A", SW.LapTime()*1e6);

  long PrevBadDynTagsCurrent = m_BuildResults.m_nBadDynTagsCurrent;
  long PrevBadDynTagsAlways  = m_BuildResults.m_nBadDynTagsAlways;
  m_BuildResults.Clear();

  m_bUpdateXRefLists=false;
  if (TestXRefListActive())
    {
    UpdateXRefLists(m_BuildResults);  // This is a virtual call to Model
    //dbgpln("%16.2f C", SW.LapTime()*1e6);

    if (m_BuildResults.m_nBadFuncs+m_BuildResults.m_nBadXRefs+m_BuildResults.m_nMissing>0)
      {
      SetXRefCI(0, true);
      POPIN("CNodeXRefMngr::UpdateXRefLists");
      return XR_BadTags;
      }
    else
      SetXRefCI(0, false);
    if (m_BuildResults.m_nBadCnvs>0)
      {
      SetXRefCI(1, true);
      POPIN("CNodeXRefMngr::UpdateXRefLists");
      return XR_BadTags;
      }
    else
      SetXRefCI(1, false);
    if (m_BuildResults.m_nBadDynTagsCurrent>0 || m_BuildResults.m_nBadDynTagsAlways>0)
      {
      if ((m_BuildResults.m_nBadDynTagsCurrent != PrevBadDynTagsCurrent) || 
          (m_BuildResults.m_nBadDynTagsAlways != PrevBadDynTagsAlways))
        {
        SetXRefCI(2, false); // Clear previous
        Strng S;
        if (m_BuildResults.m_nBadDynTagsCurrent && m_BuildResults.m_nBadDynTagsAlways)
          S.Set("E\tDynamic Tags Missing %i, Never found %i", m_BuildResults.m_nBadDynTagsCurrent, m_BuildResults.m_nBadDynTagsAlways);
        else if (m_BuildResults.m_nBadDynTagsAlways)
          S.Set("E\tDynamic Tags Never found %i", m_BuildResults.m_nBadDynTagsAlways);
        else
          S.Set("Dynamic Tags Missing %i", m_BuildResults.m_nBadDynTagsCurrent);
        SetXRefCI(2, true, S());
        }
      POPIN("CNodeXRefMngr::UpdateXRefLists");
      //return XR_BadTags; // allow to continue
      }
    else
      SetXRefCI(2, false);
    }
  //m_BuildResults=Results;

  m_bXRefListsChanged=SetXRefListsChangedFlag;
  m_bXRefsValid=true;
  //dbgpln("%16.2f D", SW.LapTime()*1e6);

  for (int i=0; i<XR_ReachCnt; i++)
    {
    for (int j=0; j<m_XRefsOwnedByMe[i].GetSize();  )
      {
      if (!m_XRefsOwnedByMe[i][j]->m_bInUse)
        delete m_XRefsOwnedByMe[i][j]; // this will remove the element from this array
      else
        j++;
      }
    }
  for (int i=0; i<m_TagRefsOwnedByMe.GetSize(); )
    {
    if (!m_TagRefsOwnedByMe[i]->m_bInUse)
      delete m_TagRefsOwnedByMe[i]; // this will remove the element from this array
    else
      i++;
    }

  //dbgpln("%16.2f E", SW.LapTime()*1e6);
  POPIN("CNodeXRefMngr::UpdateXRefLists");
  return XR_OK;
  }

//--------------------------------------------------------------------------

void CNodeXRefMngr::UnlinkAllXRefs()
  {
  // Mark As Unused
  for (int i=0; i<XR_ReachCnt; i++)
    {
    for (int j=0; j<m_XRefsOwnedByMe[i].GetSize(); j++)
      m_XRefsOwnedByMe[i][j]->m_bInUse=false;
    }
  for (int i=0; i<m_TagRefsOwnedByMe.GetSize(); i++)
    m_TagRefsOwnedByMe[i]->m_bInUse=false;

  m_bXRefsValid=false;
  m_bXRefListsChanged=true;
  };

//--------------------------------------------------------------------------

void CNodeXRefMngr::RemoveAllXRefs()
  {
  for (int i=0; i<XR_ReachCnt; i++)
    {
    while (m_XRefsOwnedByMe[i].GetSize() )
      {
      CXRefItem *p=m_XRefsOwnedByMe[i][0];
      p->RemoveMe();
      delete p; // this will remove the element from this array
      }
    }
  while ( m_TagRefsOwnedByMe.GetSize() )
    {
    CTagRef *p=m_TagRefsOwnedByMe[0];
    p->RemoveMe();
    delete p; // this will remove the element from this array
    }
  }

//--------------------------------------------------------------------------

CXRefItem * CNodeXRefMngr::AddNearXRef(CXRefItem * pXRef)
  {
  int n=m_XRefsOwnedByMe[XR_Near].GetSize();
  for (int i=0; i<n; i++)
    {
    CXRefItem *pY=m_XRefsOwnedByMe[XR_Near][i];
    if (pY->SameReference(pXRef))
      {
      // delete new one 
      delete pXRef; 
      
      // return Original one
      #if dbgXRefs
      if (dbgAdd(pY->RefTag()))
        dbgpln("AddNearXRef ORIGINAL   [%4i] %5iXR %s -> %s", m_XRefsOwnedByMe[XR_Near].GetSize(), pY->m_nMyAllocNo, GetOwnerTag(), pY->RefTag());
      #endif

      pY->m_bInUse=true;
      return pY; 
      }
    }

  #if dbgXRefs
  if (dbgAdd(pXRef->RefTag()))
    dbgpln("AddNearXRef NEW        [%4i] %5iXR %s -> %s", m_XRefsOwnedByMe[XR_Near].GetSize(), pXRef->m_nMyAllocNo, GetOwnerTag(), pXRef->RefTag());
  #endif
  PUSHIN("CNodeXRefMngr::AddNearXRef");

  CXRefItem::gs_TagRefGlblFn->AddMe(pXRef);
  if (pXRef->m_bLclRqd)
    CXRefItem::gs_TagRefGlblFn->AddMe(&pXRef->m_Lcl);

  m_XRefsOwnedByMe[XR_Near].Add(pXRef);
  pXRef->m_pMngr = this;

  CTNode *pSrc=dynamic_cast<CTNode*>(pXRef->SrcNd());
  CTNode *pDst=dynamic_cast<CTNode*>(pXRef->DstNd());
  if (pSrc && pDst)
    {
    pSrc->AddXRef2Me(pXRef, true);
    pDst->AddXRef2Me(pXRef, false);
    }
  POPIN("CNodeXRefMngr::AddNearXRef");
  return pXRef;
  }

//---------------------------------------------------------------------------

CXRefItem * CNodeXRefMngr::AddFarXRef(CXRefItem * pXRef)
  {
  DoBreak();

  #if dbgXRefs
  if (dbgAdd(pXRef->RefTag()))
    dbgpln("AddFarXRef            [%4i] %5iXR %s -> %s", m_XRefsOwnedByMe[XR_Far].GetSize(), pXRef->m_nMyAllocNo, GetOwnerTag(), pXRef->RefTag());
  #endif

  PUSHIN("CNodeXRefMngr::AddFarXRef");
  CXRefItem::gs_TagRefGlblFn->AddMe(pXRef);
  m_XRefsOwnedByMe[XR_Far].Add(pXRef);
  POPIN("CNodeXRefMngr::AddFarXRef");
  return pXRef;
  }

//---------------------------------------------------------------------------

CTagRef * CNodeXRefMngr::AddIORef(CTagRef * pTgRef)
  {
  int n=m_TagRefsOwnedByMe.GetSize();
  for (int i=0; i<n; i++)
    {
    CTagRef *pY=m_TagRefsOwnedByMe[i];
    if (pY->SameReference(pTgRef))
      {
      // delete new one 
      delete pTgRef; 
      
      // return Original one
      #if dbgXRefs
      if (dbgAdd(pY->RefTag()))
        dbgpln("AddNearXRef ORIGINAL   [%4i] %5iXR %s -> %s", m_TagRefsOwnedByMe.GetSize(), pY->m_nMyAllocNo, GetOwnerTag(), pY->RefTag());
      #endif

      pY->m_bInUse=true;
      return pY; 
      }
    }

#if dbgXRefs
  if (dbgAdd(pTgRef->RefTag()))
    dbgpln("AddIOXRef            [%4i] %5iXR %s[%s] -> %s ", m_TagRefsOwnedByMe.GetSize(), pTgRef->m_nMyAllocNo, GetOwnerTag(), pTgRef->m_sRmtTag(), pTgRef->RefTag());
  #endif
  PUSHIN("CNodeXRefMngr::AddIORef");

  ASSERT(pTgRef);
  ASSERT(pTgRef->m_iIOInx>=0);

  CXRefItem::gs_TagRefGlblFn->AddMe(pTgRef);
  m_TagRefsOwnedByMe.Add(pTgRef);

  m_IOTagRefs.SetAtGrow(pTgRef->m_iIOInx, pTgRef);
  POPIN("CNodeXRefMngr::AddIORef");
  return pTgRef;//m_IOTagRefs.GetSize()-1;
  }

//--------------------------------------------------------------------------

BOOL CNodeXRefMngr::GetValidCnvTxt(CTgFnIoVar & Var, Strng & CnvTxt)
  {
  if (Var.m_eWhat==eIsTag && Var.bCnvOK && Var.m_pXRef)
    {
    CnvTxt = Var.m_pXRef->m_sCnv();
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

void CNodeXRefMngr::GetNearXRefValues()
  {
  //  CStopWatch SW;
  //  SW.Start();
  ASSERT_ALWAYS(XRefListActive(), "CNodeXRefMngr::GetNearXRefValues XRefList Inactive", __FILE__, __LINE__);

  for (int i=0; i<m_XRefsOwnedByMe[XR_Near].GetSize(); i++)
    {
    m_XRefsOwnedByMe[XR_Near][i]->GetNearXRefValue();
    //dbgpln("     XR    %10.2f %s",SW.Lap()*1e6, m_XRefsOwnedByMe[XR_Near][i]->RefTag()); 
    }
  };

//---------------------------------------------------------------------------

void CNodeXRefMngr::SetNearXRefValues()
  {
  ASSERT_ALWAYS(XRefListActive(), "CNodeXRefMngr::SetNearXRefValues XRefList Inactive", __FILE__, __LINE__);

  for (int i=0; i<m_XRefsOwnedByMe[XR_Near].GetSize(); i++)
    m_XRefsOwnedByMe[XR_Near][i]->SetNearXRefValue();
  };

//---------------------------------------------------------------------------

void CNodeXRefMngr::AddXRef2Me(CXRefItem *pRef, bool IsSrc)
  {
  //dbgp("AddXRefPtr : Nd:%08x Rf:%08x" , this, pRef);
  for (int i=0; i<m_nXRefs2Me; i++)
    if (m_XRefs2Me[i]==pRef)
      {
      #if dbgXRefs            
      if (dbgAdd2Me(pRef->RefTag()))
        dbgpln("AddXRef2Me EXISTS           %5iXR %-20s %s", pRef->m_nMyAllocNo, GetOwnerTag(), pRef->SFGNodeTag());
      #endif
      return;
      }

  #if dbgXRefs
  if (dbgAdd2Me(pRef->RefTag()))
    dbgpln("AddXRef2Me NEW              %5iXR %-20s %s", pRef->m_nMyAllocNo, GetOwnerTag(), pRef->SFGNodeTag());
  #endif
  m_XRefs2Me.SetSize(Max((long)m_nXRefs2Me+1L, (long)m_XRefs2Me.GetSize()));
  m_XRefs2Me[m_nXRefs2Me]=pRef;
  m_nXRefs2Me++;
  };

//--------------------------------------------------------------------------

void CNodeXRefMngr::RemoveXRef2Me(CXRefItem *pRef)
  {
  for (int i=0; i<m_nXRefs2Me; i++)
    if (m_XRefs2Me[i]==pRef)
      {
      #if dbgXRefs
      if (dbgRemove2Me(pRef->RefTag()))
        dbgpln("RemoveXRef2Me EXISTS        %5iXR %-20s %s", pRef->m_nMyAllocNo, GetOwnerTag(), pRef->SFGNodeTag());
      #endif
      for (  ; i<m_nXRefs2Me-1; i++)
        m_XRefs2Me[i]=m_XRefs2Me[i+1];
      m_nXRefs2Me--;
      return;
      }
  #if dbgXRefs
  if (dbgRemove2Me(pRef->RefTag()))
    dbgpln("RemoveXRef2Me NOTFND        %5iXR %-20s %s", pRef->m_nMyAllocNo, GetOwnerTag(), pRef->SFGNodeTag());
  #endif
  };

//--------------------------------------------------------------------------

flag CNodeXRefMngr::QuerySubsReqd(CXM_DataRequest &Rqst, long &MdlIndex)
  {
  MdlIndex=(MdlIndex<0) ? 0 : MdlIndex+1;
  if (MdlIndex < m_XRefsOwnedByMe[XR_Far].GetSize())
    {
    CXRefItem * pX=m_XRefsOwnedByMe[XR_Far][MdlIndex];
    if (pX->m_pStatus==NULL || pX->m_pStatus->IsXRefActive())
      {
      Rqst.Set(-1, pX->RefTag(), TABOpt_AllInfoOnce, (pX->m_bMustSet ? XIO_InOut : XIO_In));
      return True;
      }
    }
  return False;
  }

//--------------------------------------------------------------------------

flag CNodeXRefMngr::QuerySubsAvail(CXM_DataRequest &Rqst, long &MdlIndex)
  {
  VERIFY(0);
  return False;
  }

//--------------------------------------------------------------------------

flag CNodeXRefMngr::ReadSubsData(CXM_ObjectData &ObjData, long &MdlIndex)
  {//have any far external tags changed that need to be sent
  MdlIndex=(MdlIndex<0) ? 0 : MdlIndex+1;
  if (MdlIndex < m_XRefsOwnedByMe[XR_Far].GetSize())
    {
    CXRefItem * pX=m_XRefsOwnedByMe[XR_Far][MdlIndex];
    if ((pX->m_pStatus==NULL || pX->m_pStatus->IsXRefActive()) && pX->HasChanged())
      {
      PkDataUnion PData;
      PData.SetTypeDouble(pX->m_iType, pX->GetXRefValue(true));
      ObjData.List.Clear();
      //CPkDataItem * pPItem = ObjData.FirstItem();
      ObjData.List.SetDataValue(/*pPItem,*/ NULL, PData);
      //ObjData.SetSize();
      return True;
      }
    }
  return False;
  }

//--------------------------------------------------------------------------

flag CNodeXRefMngr::WriteSubsData(CXM_ObjectData &ObjData, long &MdlIndex)
  {//get current values of all far external tags
  MdlIndex=(MdlIndex<0) ? 0 : MdlIndex+1;
  if (MdlIndex < m_XRefsOwnedByMe[XR_Far].GetSize())
    {
    CPkDataItem * pItem = ObjData.FirstItem();
    pPkDataUnion pPData = pItem->Value();
    m_XRefsOwnedByMe[XR_Far][MdlIndex]->SetXRefValue(pPData->GetDouble(), true);
    return True;
    }
  return False;
  }

//==========================================================================
//
//
//
//==========================================================================

CTgFnIoVar::CTgFnIoVar(BXOptions Options)
  {
  m_bAllowFunct = (Options&BXO_Funct)!=0;
  m_bAllowBlank = (Options&BXO_Blank)!=0;
  m_iIOId=-1;
  m_iIOInx=-1;
  m_pXRef   = NULL;
  m_pTagRef = NULL;
  Clear();
  }

CTgFnIoVar::~CTgFnIoVar()
  {
  Clear();
  }

CTgFnIoVar & CTgFnIoVar::operator=(CTgFnIoVar & X)
  {
  sVar            = X.sVar;
  iFunctNo        = X.iFunctNo;
  m_pXRef         = X.m_pXRef;
  m_pTagRef       = X.m_pTagRef;
  m_iIOId         = X.m_iIOId;
  m_iIOInx        = X.m_iIOInx;
  pFunctVar       = X.pFunctVar;
  m_bAllowFunct   = X.m_bAllowFunct;
  m_bAllowBlank   = X.m_bAllowBlank;
  m_eWhat         = X.m_eWhat;
  //bIsFunct        = X.bIsFunct;
  //bIsTag          = X.bIsTag;
  //bIsBlank        = X.bIsBlank;
  //bIsNear         = X.bIsNear;
  m_eReach        = X.m_eReach; 
  bCnvOK          = X.bCnvOK;
  //m_bIsIO       = X.m_bIsIO;
  m_pXRM          = X.m_pXRM;
  return *this;
  }

//--------------------------------------------------------------------------

//void CTgFnIoVar::Init(BOOL AllowFunct, BOOL AllowBlank, int ioId)
//  {
//  m_bAllowFunct = AllowFunct;
//  bAllowBlank = AllowBlank;
//  m_ioId = ioId;
//  }

//--------------------------------------------------------------------------

void CTgFnIoVar::Clear()
  {
  sVar = "";
  
  //delete m_pXRef;
  m_pXRef = NULL;
  
  //delete m_pTagRef;
  m_pTagRef = NULL;

  iFunctNo = -1;
  pFunctVar = NULL;
  m_eWhat=eIsBlank;
  //bIsFunct = 0;
  //bIsTag = 0;
  //bIsBlank = 1;
  m_eReach=XR_Null;//bIsNear = 0;
  bCnvOK = 0;
  //m_bIsIO = 0;
  m_pXRM = NULL;
  }

//--------------------------------------------------------------------------

int CTgFnIoVar::SetVar(char* Tag, flag IgnoreCnv/*=false*/, char* pForceTag/*=NULL*/, Strng *pForTagOnly/*=NULL*/)
  {
  int RetCode = 0;
  Strng NewTg(Tag);
  if (pForceTag)
    {
    if (NewTg.Find('.')>=0)
      {
      NewTg = NewTg.Left(NewTg.Find('.')+1);
      NewTg += pForceTag;
      }
    else if (NewTg.Len()>0)
      {
      NewTg += '.';
      NewTg += pForceTag;
      }
    }

  const flag EmptyNewTag = NewTg.IsEmpty();
  const flag EmptyVar = sVar.IsEmpty();
  if (!EmptyNewTag || !EmptyVar)
    {
    if ( (EmptyVar && !EmptyNewTag) ||
        (!EmptyVar && EmptyNewTag) ||
        (strcmp(sVar(), NewTg())!=0) )
      {
      RetCode = 1;//changed...
      sVar = NewTg();
      sVar.Trim();
      if (sVar())
        {
        if (m_bAllowFunct && sVar.IsNotEmpty() && sVar[0]=='=')
          {

          }
        else
          {
          Strng WrkTag,WrkCnv;
          TaggedObject::SplitTagCnv(sVar(), WrkTag, WrkCnv);
          if (IgnoreCnv)
            sVar = WrkTag();
          else if (WrkCnv.IsNotEmpty())
            sVar.Set("%s (%s)", WrkTag(), WrkCnv());
          }
        }
      }
    }
  if (!(sVar() && sVar.IsNotEmpty()))
    m_eWhat=eIsBlank;

  if (pForTagOnly)
    {
    Strng sCnv;
    TaggedObject::SplitTagCnv(sVar(), *pForTagOnly, sCnv);
    }
  return RetCode;
  }

//--------------------------------------------------------------------------

int CTgFnIoVar::DoDataXchg(DataChangeBlk& DCB, flag IgnoreCnv/*=false*/, char* pForceTag/*=NULL*/)
  {
  int RetCode = 0;
  if (DCB.rpC)
    RetCode = SetVar(DCB.rpC, IgnoreCnv, pForceTag);
  DCB.pC = sVar();
  return RetCode;
  }

//--------------------------------------------------------------------------

BXReturn CTgFnIoVar::UpdateXRef(CXRefStatus * pStatus, bool MustSet, bool MustGet, int& FunctNo, CNodeXRefMngr * pXRM, int IOId, LPCSTR LclTag, LPCSTR LclID, LPCSTR Desc, CXRefBuildResults &Results)
  {
  m_iIOId       = IOId;
  m_iIOInx      = -1;

  BXReturn RetCode = BXR_OK;
  m_pXRef = NULL;
  m_pTagRef = NULL;
  iFunctNo = -1;
  m_eReach=XR_Null;
  m_eWhat=eIsBlank;
  bCnvOK = 1;
  m_pXRM = pXRM;
  pFunctVar = NULL;

  #if dbgXRefs
  if (dbgReBuild())
    dbgpln("CTgFnIoVar::UpdateXRef %08x IO:%3i Tg:'%s'", this, m_iIOId, sVar() ? sVar() : "");
  #endif
  PUSHIN("CTgFnIoVar::UpdateXRef");

  CTagRef * pTgRf=NULL;
  if (m_iIOId>=0 && (pXRM->ResolveIORef(IOId, pTgRf) & FXR_Found))//pXRM->GetCIONo(m_iIOId)>=0)
    {
    pTgRf->m_sRefTag  = LclTag;
    pTgRf->m_sRefID   = LclID;
    m_eWhat=eIsCtrlIO;
    m_eReach   = XR_IO;
    m_pTagRef=pXRM->AddIORef(pTgRf);
    }
  else
    {
    m_pTagRef=NULL;
  
    if (sVar() && sVar.IsNotEmpty())
      {
      if (m_bAllowFunct && sVar[0]=='=')
        {
        m_eWhat=eIsFunct;
        m_eReach = XR_Near; 
        pXRM->FnMngrRequired();
        char FunctName[16];
        iFunctNo = FunctNo++;
        sprintf(FunctName, "F%d", iFunctNo);
        flag b = pXRM->m_pFnMngr->AddFunct(FunctName, sVar(), (LPSTR)LclTag);
        if (b)
          {
          pFunctVar = pXRM->m_pFnMngr->GetFunctVar(FunctName);
          }
        else
          {
          RetCode = BXR_FunctErr; //error in fuction
          }
        }
      else
        {
        CXRefItem *pXRef=new CXRefItem(pXRM->NodeXRefsLocked(), MustSet, MustGet);
        pXRef->m_iCnvIndex  = 0;
        pXRef->m_iIOId      = IOId;
        pXRef->m_pStatus    = pStatus;

        TaggedObject::SplitTagCnv(sVar(), pXRef->m_sRefTag, pXRef->m_sCnv);
        if (pXRef->m_sRefTag.IsNotEmpty())
          {
          m_eWhat=eIsTag; 
          
          pXRef->m_sRefID=pXRef->m_sRefTag;
          pXRef->m_sRmtTag=LclTag;
          pXRef->m_sRmtID=LclID;

          pXRef->CopyToLcl();
          pXRef->m_bLclRqd=true;

          int errCode = pXRM->ResolveNearXRef(pXRef, FALSE);
          if ((errCode & FXR_NotFound) ==0)
            {
            if (errCode & FXR_BadValue)
              RetCode = BXR_TagErr;
            else
              {
              //if (MustBeParm && ((errCode & FXR_NotParam) || (errCode & FXR_ParamStopped)))
              //  {
              //  RetCode = BXR_TagErr;
              //  break;
              //  }
              //near tags are in the flowsheet (graphics model engine)
              //bIsNear = 1;
              m_eReach=XR_Near;
              pXRef->m_eReach=XR_Near;
              if ((pXRef->m_iCnvIndex)>0 && pXRef->m_sCnv.IsEmpty())
                {
                pXRef->m_sCnv = (Cnvs[pXRef->m_iCnvIndex]->SICnv())->Txt(); //default to si units
                sVar.Set("%s (%s)", pXRef->RefTag(), pXRef->m_sCnv());
                }
              else
                bCnvOK = (((pXRef->m_iCnvIndex==0) && pXRef->m_sCnv.IsEmpty()) || 
                          (Cnvs[pXRef->m_iCnvIndex]->Find(pXRef->m_sCnv())!=NULL));
              m_pXRef=pXRM->AddNearXRef(pXRef);
              pXRef=NULL;
              }
            }
          else 
            {
            errCode=pXRM->ResolveFarXRef(pXRef);
            if ((errCode & FXR_NotFound) || (errCode & FXR_BadValue))
              RetCode = BXR_TagErr;
            else
              {
              //far tags are external to the flowsheet (example driver tags)
              m_eReach = XR_Far;
              pXRef->m_eReach = XR_Far;
              if ((pXRef->m_iCnvIndex)>0 && pXRef->m_sCnv.IsEmpty())
                {
                pXRef->m_sCnv = (Cnvs[pXRef->m_iCnvIndex]->SICnv())->Txt();
                sVar.Set("%s (%s)", pXRef->RefTag(), pXRef->m_sCnv());
                }
              else
                bCnvOK = (((pXRef->m_iCnvIndex==0) && pXRef->m_sCnv.IsEmpty()) || 
                          (Cnvs[pXRef->m_iCnvIndex]->Find(pXRef->m_sCnv())!=NULL));
              m_pXRef=pXRM->AddFarXRef(pXRef);
              pXRef=NULL;
              }
            }
          }
        else
          m_eWhat=eIsBlank;
        delete pXRef; // for those cases where a there is a problem or a blank 
        }
      }
    else
      {
      m_eWhat=eIsBlank;
      }
    }


  if ((m_eWhat==eIsBlank) && !m_bAllowBlank)
    RetCode = BXR_Blank; //nothing specified
    
  if (RetCode!=BXR_OK)
    {
    LPSTR Tag=pXRM->GetOwnerTag();
    switch (RetCode)
      {
      case BXR_FunctErr:
        {
        Strng s;
        s.Set("%s: %s : %s", Desc, pXRM->m_pFnMngr->m_StateLine[0](), pXRM->m_pFnMngr->m_StateLine[1]());
        LogWarning(Tag, 0, s());
        LogNote(Tag, 0, "%s", pXRM->m_pFnMngr->m_StateLine[2]());
        LogNote(Tag, 0, "%s", pXRM->m_pFnMngr->m_StateLine[3]());
        Results.m_nBadFuncs++;
        break;
        }
      case BXR_TagErr:
        {
        LogWarning(Tag, 0, "%s: '%s' not found or not allowed",  Desc, sVar());
        Results.m_nMissing++;
        break;
        }
      case BXR_Blank:
        {
//        LogWarning(Tag, 0, "%s: '%s' not specified", Desc, TagSuffix);
//        Results.m_nMissing++;
        break;
        }
      case BXR_NotRequired:
        break;
      default:
        DoBreak();
      }
    }

  if ((RetCode==BXR_OK) && (m_eWhat==eIsTag && !bCnvOK))
    {
    RetCode=BXR_CnvErr;
    Results.m_nBadCnvs++;
    LogWarning(pXRM->GetOwnerTag(), 0, "%s: Invalid conversion %s", Desc, sVar());
    }

  POPIN("CTgFnIoVar::UpdateXRef");
  return RetCode;
  };

//--------------------------------------------------------------------------

bool CTgFnIoVar::UnlinkXRefs()
  {
  if (m_pTagRef)
    {
    #if dbgXRefs
    if (dbgRemove(m_pTagRef->RefTag()))
      dbgpln("CTgFnIoVar::RemoveTagRef %08x %08x '%s' %s->%s", this, m_pTagRef, sVar(), m_pTagRef->m_sRefTag(), m_pTagRef->m_sRmtTag());
    #endif
    PUSHIN("CTgFnIoVar::UnlinkXRefs");
// NOT Owned by ME    delete m_pTagRef;
    m_pTagRef=NULL;
    POPIN("CTgFnIoVar::UnlinkXRefs");
    }

  if (m_pXRef)
    {
    #if dbgXRefs
    if (dbgRemove(m_pXRef->RefTag()))
      dbgpln("CTgFnIoVar::RemoveXRef   %08x %08x '%s' %s->%s", this, m_pXRef, sVar(), m_pXRef->m_sRefTag(), m_pXRef->m_sRmtTag());
    #endif
    PUSHIN("CTgFnIoVar::UnlinkXRefs");
// NOT Owned by ME    delete m_pXRef;
    m_pXRef=NULL;
    POPIN("CTgFnIoVar::UnlinkXRefs");
    }
  return true;
  };

//--------------------------------------------------------------------------

double CTgFnIoVar::GetCtrlValue()
  {
  return (m_eWhat==eIsCtrlIO) ? m_pXRM->GetCIOValue(m_iIOId) : dNAN;
  };

//--------------------------------------------------------------------------

void CTgFnIoVar::SetCtrlValue(double D)
  {
  if (m_eWhat==eIsCtrlIO)
    m_pXRM->SetCIOValue(m_iIOId, D);
  };

//--------------------------------------------------------------------------

bool CTgFnIoVar::GetValue(double &D, bool AsSIValue)
  {
  switch (m_eWhat)
    {
    case eIsCtrlIO:
      D=GetCtrlValue();
//      TagRef().IncGets();
      return true;
    case eIsFunct:
      D=pFunctVar->getD();
      return true;
    case eIsTag:
      if (AsSIValue)
        D=XRef().GetXRefValue();  
      else
        D=XRef().GetHumanXRefValue();  
      return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

bool CTgFnIoVar::PutValue(double D, bool AsSIValue)
  {
  switch (m_eWhat)
    {
    case eIsCtrlIO:   
      SetCtrlValue(D);
//      TagRef().IncSets();
      return true;
    case eIsFunct:
      //D=pFunctVar->getD();
      INCOMPLETECODE(__FILE__, __LINE__); //?????????????
      return true;
    case eIsTag:
      if (m_pXRef)
        {
        CXRefItem& Output = *m_pXRef;
        //D=XRef().GetHumanRefValue();  
        if (AsSIValue)
          Output.SetXRefValue(D, false);
        else
          Output.SetXRefValue(Cnvs[Output.m_iCnvIndex]->Normal(D, Output.m_sCnv()), false);
        }
      return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

BOOL CTgFnIoVar::ChangeTag(Strng & SrcTag, char* pOldTag, char* pNewTag)
  {
  if (SrcTag() && SrcTag.IsNotEmpty() && SrcTag[0]!='=')
    {
    const int len = strlen(pOldTag);
    const int TLen = SrcTag.Len();
    if (TLen>=len)
      {
      if (_strnicmp(pOldTag, SrcTag(), len)==0 && (TLen==len || SrcTag[len]=='.' || SrcTag[len]==' '))
        {
        Strng s;
        s = pNewTag;
        s += SrcTag.Mid(len, 256);
        SrcTag = s();
        return TRUE;
        }
      else
        {
        int OpenPos = SrcTag.Find('[');
        if (OpenPos>0)
          {
          int ClosePos = SrcTag.Find(']');
          if (ClosePos-OpenPos-1==len && _strnicmp(pOldTag, &(SrcTag[OpenPos+1]), len)==0)
            {
            Strng s = SrcTag.Left(OpenPos+1);
            s += pNewTag;
            s += SrcTag.Mid(ClosePos, 256);
            SrcTag = s();
            return TRUE;
            }
          }
        }
      }
    }
  return FALSE;
  }

//--------------------------------------------------------------------------

inline BOOL CTgFnIoVar::DoChangeTag(pchar pOldTag, pchar pNewTag) { return ChangeTag(sVar, pOldTag, pNewTag); };
inline BOOL CTgFnIoVar::ContainsTag(pchar pTag) { return ContainsTag(sVar, pTag); };

//--------------------------------------------------------------------------

BOOL CTgFnIoVar::ContainsTag(Strng & SrcTag, char* pTag)
  {
  const int len = strlen(pTag);
  if (SrcTag() && SrcTag.IsNotEmpty() && SrcTag[0]!='=')
    {
    const int TLen = SrcTag.Len();
    if (TLen>=len)
      {
      if (_strnicmp(pTag, SrcTag(), len)==0 && (TLen==len || SrcTag[len]=='.' || SrcTag[len]==' '))
        return TRUE;
      else
        {
        int OpenPos = SrcTag.Find('[');
        if (OpenPos>0)
          {
          int ClosePos = SrcTag.Find(']');
          if (ClosePos-OpenPos-1==len && _strnicmp(pTag, &(SrcTag[OpenPos+1]), len)==0)
            return TRUE;
          }
        }
      }
    }
  return FALSE;
  }

//==========================================================================
//
//
//
//==========================================================================

CGlblXRefMngr::CGlblXRefMngr()
  {
  m_TagRefsMap.InitHashTable(4096+1);
  }

CGlblXRefMngr::~CGlblXRefMngr()
  {
  }

void CGlblXRefMngr::AddMe(CXRefItem * pMe)
  {
  CSingleLock Lock(&m_LockSect, true);

  if (pMe->m_posXRefList==NULL)
    {
    pMe->m_posXRefList=m_XRefsList.AddTail(pMe); 
    #if dbgXRefs
    if (dbgXRefListMgmt())
      dbgpln("CGlblXRefMngr::AddMe(XRef): %i %5iXR %s", m_XRefsList.GetSize(), pMe->m_nMyAllocNo, pMe->RefTag());
    #endif
    AddMe((CTagRef*)pMe);
    }
  else
    {
    #if dbgXRefs
    if (dbgXRefListMgmt())
      dbgpln("CGlblXRefMngr::AddMe(XRef): %i %5iXR %s <<< ALREADY IN LIST", m_XRefsList.GetSize(), pMe->m_nMyAllocNo, pMe->RefTag());
    #endif
    }
  };

//--------------------------------------------------------------------------

void CGlblXRefMngr::AddMe(CTagRef * pMe)
  {
  CSingleLock Lock(&m_LockSect, true);

  //pMe->m_sTagLwr=pMe->m_sTag;
  //pMe->m_sTagLwr.Lower();

  if (pMe->m_posRefList==NULL)
    {

    CString S(pMe->m_sRefTag());
    S.MakeLower();
    S.Trim();
    if (S.GetLength()==0)
      {
      int xxx=0; 
      }

    CTagRefsMapItem *pMapItem=NULL, *pLclMapItem=NULL;
    if (!m_TagRefsMap.Lookup(S, pMapItem))
      {
      pMapItem=new CTagRefsMapItem;
      pMapItem->m_sTag=pMe->RefTag();
      pMapItem->m_sLwrTag=S;
      m_TagRefsMap.SetAt(pMapItem->m_sLwrTag, pMapItem);
      }
    
    if (pMe->m_bIsFnTerm) // GetStatusWord requires that Function Terms are first
      pMe->m_posRefMap=pMapItem->m_pTagRefs.AddHead(pMe);
    else
      pMe->m_posRefMap=pMapItem->m_pTagRefs.AddTail(pMe);
    
    pMe->m_posRefList=m_TagRefsList.AddTail(pMe);

    #if dbgXRefs
    if (dbgTagRefListMgmt())
      dbgpln("CGlblXRefMngr::AddMe(TagRef): %i %i %08x %08x %08x [%2i] %s", 
              m_TagRefsList.GetSize(), 
              pMapItem->m_pTagRefs.GetSize(), 
              pMe, pMe->m_posRefList, pMe->m_posRefMap, pMapItem->m_pTagRefs.GetSize(), pMe->RefTag());
    #endif
    }
  else
    {
    //_asm int 3;
    #if dbgXRefs
    if (dbgTagRefListMgmt())
      dbgpln("CGlblXRefMngr::AddMe(TagRef): ALREADY DONE %i %08x %s", 
              m_TagRefsList.GetSize(), 
              pMe, pMe->RefTag());
    #endif
    }
  };

//--------------------------------------------------------------------------

void CGlblXRefMngr::RemoveMe(CXRefItem * pMe)
  {
  CSingleLock Lock(&m_LockSect, true);

  #if dbgXRefs
  if (dbgXRefListMgmt())
    dbgp("CGlblXRefMngr::RemoveMe(XRef)  : %08x %5iXR %s", pMe, pMe->m_nMyAllocNo, pMe->RefTag());
  #endif
  PUSHIN("CGlblXRefMngr::RemoveMe");

  if (pMe->m_posXRefList)
    {
    m_XRefsList.RemoveAt(pMe->m_posXRefList);
    pMe->m_posXRefList=NULL;
    #if dbgXRefs
    if (dbgXRefListMgmt())
      dbgp(" LIST <<<<<<<<<<<<<<<");
    #endif
    };
  #if dbgXRefs
  if (dbgXRefListMgmt())
    dbgpln("");
  #endif

  RemoveMe((CTagRef*)pMe);
  RemoveMe(&pMe->m_Lcl);
  POPIN("CGlblXRefMngr::RemoveMe");
  };

//--------------------------------------------------------------------------

void CGlblXRefMngr::RemoveMe(CTagRef * pMe)
  {
  CSingleLock Lock(&m_LockSect, true);

  #if dbgXRefs
  if (dbgTagRefListMgmt())
    dbgp("CGlblXRefMngr::RemoveMe(TagRef): %08x %08x %08x %s", pMe, pMe->m_posRefList, pMe->m_posRefMap, pMe->RefTag());
  #endif
  PUSHIN("CGlblXRefMngr::RemoveMe");

  if (pMe->m_posRefList)
    {
    m_TagRefsList.RemoveAt(pMe->m_posRefList);
    pMe->m_posRefList=NULL;
    #if dbgXRefs
    if (dbgTagRefListMgmt())
      dbgp(" REM-LISTITEM (%i)", m_TagRefsList.GetSize());
    #endif
    }

  if (pMe->m_posRefMap)
    {
    CString S(pMe->RefTag());
    S.MakeLower();

    CTagRefsMapItem* pMapItem;
    if (m_TagRefsMap.Lookup(S, pMapItem))
      {
      pMapItem->m_pTagRefs.RemoveAt(pMe->m_posRefMap);
      #if dbgXRefs
      if (dbgTagRefListMgmt())
        dbgp(" REM-MAPITEM (%i)", pMapItem->m_pTagRefs.GetSize());
      #endif
      if (pMapItem->m_pTagRefs.GetSize()==0)// && pMapItem->m_pLclXRefs.GetSize()==0)
        {
        #if dbgXRefs
        if (dbgTagRefListMgmt())
          dbgp(" REM-MAP");
        #endif
        m_TagRefsMap.RemoveKey(S);
        delete pMapItem;
        }
      }
    else
      {
      int xxx=0;
      #if dbgXRefs
      if (dbgTagRefListMgmt())
        dbgp(" MAP MISSING");
      #endif
      }
    pMe->m_posRefMap=NULL;
    }

  #if dbgXRefs
  if (dbgTagRefListMgmt())
    dbgpln("");
  #endif
  POPIN("CGlblXRefMngr::RemoveMe");
  };

//--------------------------------------------------------------------------

void CGlblXRefMngr::ClearRefCnts()
  {
  CSingleLock Lock(&m_LockSect, true);

  m_XRefsList.GetHeadPosition();
  POSITION Pos=m_TagRefsList.GetHeadPosition();
  for (long i=0; Pos; i++)
    {
    CTagRef *pItem=m_TagRefsList.GetNext(Pos);
    pItem->ClearCnts();
    }
  };

//==========================================================================
//
//
//
//==========================================================================

#pragma chNOTE("XRef Set counting will fail if the Sym is used for one while Tag is used for another")

//==========================================================================
//
//
//
//==========================================================================
