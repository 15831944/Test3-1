//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "scdver.h"
#include "sc_defs.h"
#define  __TAGOBJ_CPP
#include "datacnvs.h"
#define WITHSCDINTERFACES
#include "tagobj.h"
#include "errorlog.h"
#include "dbgmngr.h"
#include "licbase.h"

//#include "optoff.h"

#define dbgPutData             0
#define dbgStructureChanged    0
#define dbgConstructObj        0
#define dbgTagObjClass         0
#define dbgSearchTag           0

#define dbgTagObjAttach        0

#define dbgAllowedModes        0
#define dbgSetModes            0
#define dbgGetModes            0

#define dbgTagObj 1
#if dbgTagObj
static CDbgMngr dbgViewSysDefn("FlwNode",  "ViewSysDefn");
static CDbgMngr dbgFldMonitorAdd("TagObj",   "FldMonitor.Add");
static CDbgMngr dbgFldMonitorChg("TagObj",   "FldMonitor.Chg");
#endif

//========================================================================
//
//
//
//========================================================================

// for GUID's
#ifndef __RPC_H__
  #include <rpc.h>
#endif
#ifndef __RPCDCE_H__
  #include <rpcdce.h>
#endif

#pragma comment (lib, "rpcrt4.lib")


//========================================================================
//
//
//
//========================================================================

enum eO2NTypes
  {
  eO2N_End,// End Of List
  eO2N_Direct, // Direct Replacement
  eO2N_Class,  // Replace With Class
  };

class COld2NewItem
  {
  public:
    int     m_Type;
    LPCTSTR m_OldTag;
    LPCTSTR m_NewTag;
    LPCTSTR m_Comment;
    int     m_nLogged;
  };

static COld2NewItem gs_Old2New[] =
  {
    {eO2N_Direct, "$Dyn.DynStats.RqdTime",            "$Dyn.Scenario.DurationRqd",              "$Dyn.Scenario.Type should be set to Duration"},
    {eO2N_Direct, "$Dyn.DynStats.RqdTm",              "$Dyn.Scenario.DurationRqd",              "$Dyn.Scenario.Type should be set to Duration"},
    {eO2N_Direct, "$Dyn.DateTime.Time",               "$Dyn.Time",                              ""},
    {eO2N_Direct, "$Dyn.DateTime.TimeDesc",           "$Dyn.Time.Fmt",                          ""},
    {eO2N_Direct, "$Dyn.DateTime.TimeFullDesc",       "$Dyn.Time.Fmt",                          ""},
    {eO2N_Direct, "$Dyn.DateTime.Day",                "$Dyn.DateRec.Day",                       ""},
    {eO2N_Direct, "$Dyn.DateTime.Month",              "$Dyn.DateRec.Month",                     ""},
    {eO2N_Direct, "$Dyn.DateTime.Year",               "$Dyn.DateRec.Year",                      ""},
    {eO2N_Direct, "$Dyn.ResetTime",                   "$Dyn.Scenario.Start.SetTime",            ""},
    {eO2N_Direct, "$Dyn.TimeAtStart",                 "$Dyn.Scenario.Start.StartAt.Fmt",        ""},
    {eO2N_Direct, "$Dyn.RestartHistorian",            "$Dyn.Scenario.Start.RestartHistorian",   ""},
    {eO2N_Direct, "$Dyn.Try_Start",                   "$Dyn.Cmd.Start",                         ""},
    {eO2N_Direct, "$Dyn.Try_Idle",                    "$Dyn.Cmd.Idle",                          ""},
    {eO2N_Direct, "$Dyn.Try_Stop",                    "$Dyn.Cmd.Stop",                          ""},
 
    {eO2N_Class,  "Pipe-1.Reactions",                 ".EB.Reactions",                          ""},
    {eO2N_Class,  "Pipe-1.EnvironHX",                 ".EB.EnvironHX",                          ""},
    {eO2N_Class,  "Pipe-1.VLEquilibrium",             ".EB.VLEquilibrium",                      ""},
    {eO2N_Class,  "Pipe-1.Evaporator",                ".EB.Evaporator",                         ""},

    {eO2N_Class,  "Pipe-1.Qm_RqdCapacity",            ".Flw.Qm.RqdCapacity",                    ""},
    {eO2N_Class,  "Pipe-1.Qm_Capacity",               ".Flw.Qm.Capacity",                       ""},
    {eO2N_Class,  "Pipe-1.Qm_MinCapacity",            ".Flw.Qm.MinCapacity",                    ""},
    {eO2N_Class,  "Pipe-1.Qm_MinCap",                 ".Flw.Qm.MinCap",                         ""},
    {eO2N_Class,  "Pipe-1.Qm_MaxCapacity",            ".Flw.Qm.MaxCapacity",                    ""},
    {eO2N_Class,  "Pipe-1.Qm_MaxCap",                 ".Flw.Qm.MaxCap",                         ""},
    {eO2N_Class,  "Pipe-1.Qm_Rqd",                    ".Eqn.Qm.Rqd",                            ""},

    {eO2N_Class,  "NdOpts.ExitRule",                   ".Exit.Rule",                             ""},

    {eO2N_Class,  "SpConduit.SatT@P",                 ".Saturation.SatT@P",                     ""},
    {eO2N_Class,  "SpConduit.SatP@T",                 ".Saturation.SatP@T",                     ""},
    {eO2N_Class,  "SpConduit.BPE",                    ".Saturation.BPE",                        ""},

    {eO2N_End},
  };

static CMap<LPCTSTR, LPCTSTR, COld2NewItem*, COld2NewItem*> gs_Old2NewMapDirect;
static CMap<LPCTSTR, LPCTSTR, COld2NewItem*, COld2NewItem*> gs_Old2NewMapClass;

COld2NewItem * FindAndFixOld2New(TaggedObject * pSrchRoot, LPCTSTR pOldTag, Strng & NewTag)
  {
  Strng OldTag(pOldTag);
  const int NLogs=10;
  if (gs_Old2NewMapDirect.GetCount()==0)
    {
    gs_Old2NewMapDirect.InitHashTable(FindNextPrimeNumber(sizeof(gs_Old2New)/sizeof(gs_Old2New[0])));
    gs_Old2NewMapClass.InitHashTable(FindNextPrimeNumber(sizeof(gs_Old2New)/sizeof(gs_Old2New[0])));
    for (int i=0; gs_Old2New[i].m_Type!=eO2N_End; i++)
      {
      switch (gs_Old2New[i].m_Type)
        {
        case eO2N_Direct:
          gs_Old2NewMapDirect.SetAt(gs_Old2New[i].m_OldTag, &gs_Old2New[i]);
          break;
        case eO2N_Class:
          gs_Old2NewMapClass.SetAt(gs_Old2New[i].m_OldTag, &gs_Old2New[i]);
          break;
        }
      }
    }

  COld2NewItem *pRet=NULL;
  if (gs_Old2NewMapDirect.Lookup(OldTag(), pRet))
    {
    NewTag=pRet->m_NewTag;
    if (pRet->m_nLogged<NLogs)
      {
      LogWarning("Executive", 0,
        pRet->m_Comment ? "Changed '%s' to '%s' : %s":"Changed '%s' to '%s'",
        OldTag(), NewTag(), pRet->m_Comment);
      pRet->m_nLogged++;
      }
    return pRet;
    }

  int MinTagLen=0;
  int ObjTagLen=0;
  TaggedObject *p=pSrchRoot->FindObjTag(OldTag(), true, ObjTagLen, MinTagLen);
  if (p)
    {
    TaggedObject *pAtt=p->pAttachments;
    while (pAtt)
      {
      Strng ObjTag(p->Tag());
      ObjTag+='.';
      ObjTag+=pAtt->Tag();
      if (ObjTag.GetLength()<OldTag.GetLength() &&
          OldTag[ObjTag.GetLength()]=='.' &&
          OldTag.XStrNICmp(ObjTag, ObjTag.GetLength())==0)
        {
        Strng TestTag(pAtt->ClassId());
        TestTag+=OldTag.Right(OldTag.GetLength()-ObjTag.GetLength());
        if (gs_Old2NewMapClass.Lookup(TestTag(), pRet))
          {
          NewTag=ObjTag;
          NewTag+=pRet->m_NewTag;
          if (pRet->m_nLogged<NLogs)
            {
            LogWarning("Executive", 0,
              pRet->m_Comment ? "Changed '%s' to '%s' : %s":"Changed '%s' to '%s'",
              OldTag(), NewTag(), pRet->m_Comment);
            pRet->m_nLogged++;
            }
          return pRet;
          }
        }
      pAtt = pAtt->pNxtAttachment;
      }
    }

  return pRet;
  };

//========================================================================
//
//
//
//========================================================================

CGlobalInterfaceTable g_GIT;

//========================================================================
//
//
//
//========================================================================

const LPSTR OtherReservedTags [] =
  {
  PlantModelTag
  };
const int OtherReservedTagsCnt = sizeof(OtherReservedTags)/sizeof(OtherReservedTags[0]);

const LPSTR DollarReservedTags [] =
  {
  "$PB",
  "$Dyn",
  "$SDB",
  "$Tear",
  "$Chg",
  "$Ctrl",
  "$Pwr",
  "$IO",
  "$Tol",
  "$Order"
  };
const int DollarReservedTagsCnt = sizeof(DollarReservedTags)/sizeof(DollarReservedTags[0]);


flag TagObjClass::Selectable()
  {
  return m_bSelectable;
  };

//========================================================================

TagObjClass* TagObjClass::pFirstClass=NULL;
short TagObjClass::iNIds=0;

Strng TagObjClass::sm_sTagFormat("%ID%_%3i");

#if WITHEQUIPSPECS        
bool TagObjClass::m_bEquipSpecs=true;
#endif

LONG TagObjClassLock=-1;

// -------------------------------------------------------------------------

char* TagObjClass::SubClassId() { return m_sSubClassId(); };
char* TagObjClass::BaseClassId() { return m_sBaseClassId(); };

// -------------------------------------------------------------------------

TagObjClass::TagObjClass(LPSTR pClassName_, LPSTR pGroup_, LPSTR pClassId_, LPSTR pSubClassId_, LPSTR pVersion_, LPSTR pDrwGroup_, LPSTR pTagInitialID_, dword dwCat_, LPSTR pShortDesc_, LPSTR pDesc_, DWORD dwSelectMask_) 
#if WITHEQUIPSPECS
: m_CustomData(this),
m_SelectData(this)
#endif
  {
  dbgpln("%-25s %-25s %-25s %-25s %-25s %-25s ", pClassName_, pGroup_, pClassId_, pSubClassId_, pShortDesc_, pDesc_);

  ASSERT(pClassId_!=NULL);
  // Must Have A Name
  if (strlen(pClassId_)==0)
    DoBreak();
  // Must NOT Have A '-' : reserved for Version Extension
  if (strchr(pClassId_, '-'))
    DoBreak();
  //if (SubClassId)
  //  sSubClassId=SubClassId;

  m_pTypeId = NULL;
  m_pGlobalFnBuildDataDefn = NULL;
  m_pGlobalFnDataXchg      = NULL;
  m_pGlobalFnData1         = NULL;
  m_pGlobalFnData2         = NULL;

  TagObjClass* p;
  sClassName=pClassName_;

  char * pClassTabChr= strchr(pClassId_, '\t');
  if (pClassTabChr)
    {
    sTableId=(pClassTabChr+1);
    sClassIdNV=pClassId_;
    sClassIdNV.SetLength(pClassTabChr-pClassId_);
    }
  else
    {
    sTableId=pClassId_;
    sClassIdNV=pClassId_;
    }

  sClassId=sClassIdNV;
  if (strchr(sClassId(), '*'))
    {
    int l=strchr(sClassId(), '*')-sClassId();
    m_sBaseClassId=sClassId.Left(l);
    m_sSubClassId=sClassId.Right(sClassId.Length()-l-1);
    }
  else
    {
    m_sBaseClassId=sClassId;
    m_sSubClassId=pSubClassId_?pSubClassId_:"";
    }

  iClassIdLenNoVersion=strlen(pClassId_);
  if (pVersion_ && (strlen(pVersion_)>0))
    {
    sClassId+="-";//(";
    sClassId+=pVersion_;
    //sClassId+=")";
    }

  sShortDesc=pShortDesc_;
  sDesc=pDesc_;
  pNxtClass=NULL;
  dwCategory=dwCat_;

  dwObjDataVerNo=0xffffffff;

  //dbgpln("TagObjClass: %08x [%3i]%-15s - %-15s %-15s", this, iId, sClassName(), sClassId(), sGroup());

  StartupLock.Lock();

  pNxtClass=NULL;
  if (pFirstClass==NULL)
    pFirstClass=this;
  else
    {
    p=pFirstClass;
    while (p->pNxtClass)
      p=p->pNxtClass;
    p->pNxtClass=this;
    }
  iId=iNIds++;

  sGroup=pGroup_;
  p=pFirstClass;
  while (p && _stricmp(p->sGroup(), sGroup())!=0)
    p=p->pNxtClass;
  pFirstClassInGrp=p;
  pNxtClassInGrp=NULL;
  if (pFirstClassInGrp!=this)
    {
    while (p->pNxtClassInGrp)
      p=p->pNxtClassInGrp;
    p->pNxtClassInGrp=this;
    }

  StartupLock.Unlock();

  iGrpId=-1;
  for (int i=0; iGrpId<0 ; i++)
    {
    p=pFirstClassInGrp;
    while (p && p->iGrpId!=i)
      p=p->pNxtClassInGrp;
    if (p==NULL)
      iGrpId=i;
    }

  sDrwGroup=pDrwGroup_;
  m_sTagInitialID=pTagInitialID_;
  m_sTagFormat="";
  m_bLicenseOK=1;
  m_bLicErrNoted=0;
  m_bCreateIfNotLicensed=true;// CNM false;
  m_bSelectable=(dwSelectMask_!=0);
  dwSelectMask=dwSelectMask_;
  fDuplicateExists=False;

  sMdlLibName="Unknown";
  m_pSubConstruct=NULL;

  m_bMustDelete=false;
  m_bIsCOMClass=false;
  m_sProgID="";

  m_nInstances = 0;
  m_nInstancesMax = 0;

  m_AssocGrfShowInited=false;

  #if dbgConstructObj
  dbgpln("ConstructClass:%08x %2i %s", this, iId, pShortDesc);
  #endif
  };

// -------------------------------------------------------------------------

TagObjClass::~TagObjClass()
  {
//  delete m_pPropertyMap;

  #if dbgConstructObj
  dbgpln("DestroyClass:%08x %2i %s", this, iId, pShortDesc);
  #endif
  if (iId>=0)
    {
    StartupLock.Lock();
    if (pFirstClass==this)
      {
      pFirstClass=pNxtClass;
      //dbgpln("~TagObjClass: %08x [%3i]%-15s - %-15s %-15s %08x  %08x",
      //  this, iId, sClassName(), sClassId(), sGroup(), this, pFirstClass);
      }
    else
      {
      TagObjClass* p=pFirstClass;
      while (p && (p->pNxtClass!=this))
        p=p->pNxtClass;
      ASSERT(p);
      p->pNxtClass=pNxtClass;
      //dbgpln("~TagObjClass: %08x [%3i]%-15s - %-15s %-15s %08x  %08x  %08x",
      //  this, iId, sClassName(), sClassId(), sGroup(), p, p->pNxtClass,pNxtClass);
      }
    pNxtClass=NULL;

    // Must Also Remove from Grp List
    if (pFirstClassInGrp==this)
      {
      pFirstClassInGrp=pNxtClassInGrp;
      // Move Others in group to point at next;
      TagObjClass* p=pFirstClassInGrp;
      while (p)
        {
        p->pFirstClassInGrp=pFirstClassInGrp;
        p=p->pNxtClassInGrp;
        }
      // this one is out of here
      pFirstClassInGrp=NULL;
      }
    else
      {
      TagObjClass* p=pFirstClassInGrp;
      while (p && (p->pNxtClassInGrp!=this))
        p=p->pNxtClassInGrp;
      ASSERT(p);
      p->pNxtClassInGrp=pNxtClassInGrp;
      }


    StartupLock.Unlock();
    }
  iId=-1;

  if (m_bMustDelete)
    delete this;
  };

//---------------------------------------------------------------------------

TagObjClass *  TagObjClass::AddCOMClass(char* pProgID, char* pClassName_, char* pGroup_, char* pClassId_, char* pVersion_, char* pDrwGroup_, char* pTagInitialID_, dword dwCat_, char* pShortDesc_, char* pDesc_, DWORD dwSelectMask_)
  {
  TagObjClass * p= new TagObjClass(pClassName_, NULL, pGroup_, pClassId_, pVersion_, pDrwGroup_, pTagInitialID_, dwCat_, pShortDesc_, pDesc_, dwSelectMask_);

  p->sMdlLibName="A User Lib";

  p->m_bIsCOMClass=true;
  p->m_sProgID=pProgID;

  return p;
  };

// -------------------------------------------------------------------------

void TagObjClass::DumpAll()
  {
  //#if dbgTagObjClass
  StartupLock.Lock();
  dbgpln("===================== ======================== = ================ ===== ================ ===========");
  dbgpln("TagObjClass @         [ Id ]%-18s - %-16s [GId] %-16s (flags) DLL", "ClassName", "ClassID", "Group");
  dbgpln("===================== ======================== = ================ ===== ================ ===========");
  TagObjClass* p=pFirstClass;
  while (p)
    {
    //  Temporary until unlinking problem sorted out
    if (IsBadReadPtr(p, sizeof(*p)))
      {
      dbgpln("TagObjClass BAD ");
      break;
      }
    //CNM Removed now for NTFS & windows 95
    //ASSERT(strlen(p->ClassId())<=8); // nbnb for FileNames for DataBases
    dbgpln("TagObjClass @%08x [%4i]%-18s - %-16s [%3i] %-16s (%s%s%s%s) %-10s %s", p,
      p->iId, p->sClassName(), p->sClassId(), p->iGrpId, p->sGroup(),
      p->Selectable() ? "s" : "-", p->m_bLicenseOK? "l" : "-",
      p->CatIsDynamic() ? "d" : "-", p->CatIsProBal() ? "p" : "-",
      p->sMdlLibName(), p->m_sTagInitialID());
    p=p->pNxtClass;
    }
  dbgpln("===================== ======================== = ================ ===== ================ ===========");
  StartupLock.Unlock();
  //#endif
  }

// -------------------------------------------------------------------------

TagObjClass* TagObjClass::FindClassName(LPSTR ClassName_)
  {
  StartupLock.Lock();
  TagObjClass* p=pFirstClass;
  while (p && strcmpi(p->sClassName(), ClassName_)!=0)
    p=p->pNxtClass;
  StartupLock.Unlock();
  return p;
  };

// -------------------------------------------------------------------------

TagObjClass* TagObjClass::FindClassId(LPSTR ClassId_, flag fWithVersion/*=True*/, LPCTSTR SubClass/*=NULL*/)
  {
  StartupLock.Lock();
  TagObjClass* p=pFirstClass;
  if (fWithVersion)
    {
    while (p)
      {
      if (strcmpi(p->sClassId(), ClassId_)==0)
        {
        if (SubClass)
          {
          if (p->SubClassId() && _stricmp(SubClass, p->SubClassId())==0)
            goto Done;
          }
        else
          goto Done;
        }
      p=p->pNxtClass;
      }
    }
  else
    {
    while (p)
      {
      if (((int)strlen(ClassId_)==p->ClassIdLenNoVersion()) &&
           (_strnicmp(p->sClassId(), ClassId_, p->ClassIdLenNoVersion())==0))
        {
        if (SubClass)
          {
          if (p->SubClassId() && _stricmp(SubClass, p->SubClassId())==0)
            goto Done;
          }
        else
          goto Done;
        }
      p=p->pNxtClass;
      }
    }
Done:
  StartupLock.Unlock();
  return p;
  };

// -------------------------------------------------------------------------

TagObjClass* TagObjClass::FindGroupId(LPSTR GroupId_)
  {
  StartupLock.Lock();
  TagObjClass* p=pFirstClass;
  while (p && strcmpi(p->sGroup(), GroupId_)!=0)
    p=p->pNxtClass;
  StartupLock.Unlock();
  return p;
  };

// -------------------------------------------------------------------------

TagObjClass* TagObjClass::FindClassIndexed(LPSTR GroupId_, int iIndex)
  {
  iIndex++;
  StartupLock.Lock();
  TagObjClass* p=pFirstClass;
  while (p)
    {
//dbgpln("%s %2i, %s %s", GroupId_, p->Selectable(), p->ClassId(), p->SubClassId());

    if (!p->Selectable() && _stricmp(GroupId_, "Hidden")==0)
      iIndex--;
    else if (p->Selectable() && strcmpi(p->sGroup(), GroupId_)==0)
      iIndex--;
    if (iIndex==0)
      break;
    p=p->pNxtClass;
    }
  StartupLock.Unlock();
  return p;
  };

// -------------------------------------------------------------------------

TagObjClass* TagObjClass::FindGrpClassId(LPSTR ClassId_)
  {
  TagObjClass* p=pFirstClassInGrp;
  while (p && strcmpi(p->sClassId(), ClassId_)!=0)
    p=p->pNxtClassInGrp;
  return p;
  };

// --------------------------------------------------------------------------

void TagObjClass::GetShortDesc(Strng &S)
  {
  S=ShortDesc();
  S.SetLength(Min(20, S.Length()));
  S.Trim();
  }

// -------------------------------------------------------------------------

TagObjClass* TagObjClass::FindGrpShortDesc(LPSTR ShortDesc_)
  {
  Strng S;
  TagObjClass* p=pFirstClassInGrp;
  while (p)
    {
    p->GetShortDesc(S);
    if (strcmpi(S(), ShortDesc_)==0)
      break;
    p=p->pNxtClassInGrp;
    }
  return p;
  };

// -------------------------------------------------------------------------

TagObjClass* TagObjClass::FindDuplicateClassId(TagObjClass* pStartAt)
  {
  TagObjClass* p=pStartAt ? pStartAt : pFirstClass;
  for (p=pFirstClass; p; p=p->pNxtClass)
    if ((p!=this) && (_stricmp(p->sClassId(), sClassId())==0))
      {
      //if (p->sSubClassId() && sSubClassId())
      //  {
      //  if (p->sSubClassId.XStrICmp(sSubClassId)!=0)
      //    continue;
      //  }
      return p;
      }
  return NULL;
  }

//---------------------------------------------------------------------------

TaggedObject *TagObjClass::Construct(LPSTR pGroup_, LPSTR pClassId_, LPSTR pSubClassId, LPSTR pRqdTag, TaggedObject *pAttach, TagObjAttachment eAttach)
  {
  TagObjClass* pC=FindClassId(pClassId_); //ClassId's MUST be unique system wide
  TaggedObject *pO=NULL;

  if (pC)
    {
    if (!pC->LicenseOK())
      {
      gs_License.BumpIllegalModelCount(1, pC->ShortDesc(), !pC->LicErrNoted() ? eLic_Msg:eLic_None);
      pC->SetLicErrNoted(true);
      }

    if (pC->LicenseOK() || pC->m_bCreateIfNotLicensed)
      {
      if (strlen(pGroup_)>0 && _stricmp(pGroup_, pC->sGroup())!=0)
        {
        dbgpln("Bad Class %s (groups don't match): %s <> %s", pClassId_, pGroup_, pC->sGroup());
        return NULL;
        }
      pO=pC->Construct(pSubClassId, pRqdTag, pAttach, eAttach);
      }
    }
  #if dbgConstructObj
  dbgpln("Construct:%s %s", pClassId_, pRqdTag);
  #endif
  return pO;
  };


//---------------------------------------------------------------------------

TaggedObject *TagObjClass::ConstructGrp(LPSTR pClassId_, LPSTR pSubClassId, LPSTR pRqdTag, TaggedObject *pAttach, TagObjAttachment eAttach)
  {
  TagObjClass* pC=FindGrpClassId(pClassId_);
  TaggedObject *pO=NULL;

  //if (pSubClassId==NULL)
  //  pSubClassId=sSubClassId();

  if (pC)
    {
    if(!pC->LicenseOK())
      {
      gs_License.BumpIllegalModelCount(1, pC->ShortDesc(), !pC->LicErrNoted() ? eLic_Msg:eLic_None);
      pC->SetLicErrNoted(true);
      }

    if(pC->LicenseOK() || pC->m_bCreateIfNotLicensed)
      {
      if (pC->m_bIsCOMClass)
        {
        pO=CCOMObjManagerRoot::gs_Manager->Construct(pC/*->m_sProgID()*/, pRqdTag, pAttach, eAttach);
        }
      else
        pO=pC->Construct(pSubClassId, pRqdTag, pAttach, eAttach);
      }
    }
  #if dbgConstructObj
  dbgpln("Construct:%s %s", pClassId_, pRqdTag);
  #endif
  return pO;
  };

//---------------------------------------------------------------------------

TaggedObject *TagObjClass::ConstructGrpDesc(LPSTR pShortDesc_, LPSTR pSubClassId, LPSTR pRqdTag, TaggedObject *pAttach, TagObjAttachment eAttach)
  {
  TagObjClass* pC=FindGrpShortDesc(pShortDesc_);
  TaggedObject *pO=NULL;

  if (pC)
    {
    if(!pC->LicenseOK())
      {
      gs_License.BumpIllegalModelCount(1, pC->ShortDesc(), !pC->LicErrNoted() ? eLic_Msg:eLic_None);
      pC->SetLicErrNoted(true);
      }

    if(pC->LicenseOK() || pC->m_bCreateIfNotLicensed)
      {
      pO=pC->Construct(pSubClassId, pRqdTag, pAttach, eAttach);
      }
    }

  #if dbgConstructObj
  dbgpln("Construct:%s %s", pShortDesc_, pRqdTag);
  #endif
  return pO;
  };

//---------------------------------------------------------------------------

TaggedObject *TagObjClass::Construct(LPSTR pSubClassId, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  {
  if (pSubClassId && strlen(pSubClassId))
    {
    INCOMPLETECODE(__FILE__, __LINE__)(__FILE__, __LINE__);
    }
  if (m_bIsCOMClass)
    return CCOMObjManagerRoot::gs_Manager->Construct(this, pTag, pAttach, eAttach);

  return NULL;
  };

//---------------------------------------------------------------------------

flag TagObjClass::EditorExists(LPSTR pGroup_, LPSTR pClassId_, LPSTR pRqdTag)
  {
  TagObjClass* pC=FindClassId(pClassId_);
  if (pC)
    return pC->EditorExists();
  return 0;
  };

//---------------------------------------------------------------------------

void TagObjClass::Clear()
  {
  };

//---------------------------------------------------------------------------

void TagObjClass::ClearAll()
  {
  StartupLock.Lock();
  TagObjClass* pC=pFirstClass;
  while (pC)
    {
    //  Temporary until unlinking problem sorted out
    if (IsBadReadPtr(pC, sizeof(*pC)))
      break;
    pC->Clear();
    pC=pC->pNxtClass;
    }
  StartupLock.Unlock();
  };

//---------------------------------------------------------------------------

void TagObjClass::GetClassValueLst(char * GroupName, DDBValueLstMem & DDB0, char * Value0, DWORD SelectMask)
  {
  DDB0.Empty();
  if (Value0)
    DDB0.Add(Value0);
  for (long i=0; ;i++)
    {
    TagObjClass* pC=FindClassIndexed(GroupName, i);
    if (pC)
      {
      if (pC->dwSelectMask & SelectMask && pC->LicenseOK())
        DDB0.Add(pC->ClassId());
      }
    else
      break;
    }
  }

//---------------------------------------------------------------------------

void TagObjClass::GetSDescValueLst(char * GroupName, DDBValueLstMem & DDB0, char * Value0, DWORD SelectMask)
  {
  Strng S;
  DDB0.Empty();
  if (Value0)
    DDB0.Add(Value0);
  for (long i=0; ;i++)
    {
    TagObjClass* pC=FindClassIndexed(GroupName, i);
    if (pC)
      {
      if (pC->dwSelectMask & SelectMask && pC->LicenseOK())
        {
        pC->GetShortDesc(S);
        if (!Value0 || S.XStrICmp(Value0)!=0)
          DDB0.Add(S());
        }
      }
    else
      break;
    }
  }

//---------------------------------------------------------------------------

#if WITHCLASSOPTIONS

const char* DefaultSCO =
";--- SysCAD Class Options file ---\n"
"\n"
"[Options]\n"
";Use : %ID%, %PAGE%, %AREA%, %SRC%, %DST% ........\n"
"TagInitialID       =\n"
";todo TagFormat          =\n"
"\n"
"DefaultProbalValues   = Defaults0 \n"
"DefaultDynamicValues  = Defaults1 \n"
"\n"
";CustomDataTables      = CustomDataTags0,CustomDataTags2  ;allow multiple sets of user tags\n"
";SelectionTable        = Selection0                       ;do we want to allow multiple selections\n"
";ReportTables          = Report0,Report1                  ;allow multiple selections\n"
"\n"
";=========================================================================\n"
"\n"
"[Defaults1]\n"
";values set on model create\n"
"Description = Defaults for Dynamic\n"
";Field0 = Eqn.Name,     Darcy DP,     ,\n"
";Field1 = Eqn.Rqd_Len,  100,            m,\n"
"\n"
"[Defaults0]\n"
";values set on model create\n"
"Description = Defaults for ProBal\n"
";Field0 = ,     ,     ,\n"
"\n"
"\n"
";;=========================================================================\n"
";\n"
";[CustomDataTags0]\n"
";;custom data (this would go on a separate tab page \"CD\")\n"
";;       Symbol,   Tag,            Type,   DefaultValue, Options,      Conversion\n"
";Field0, MaxDesQm, MaxDesignFlow,  double, 10.0,         ParamStopped, Qm(t/h)\n"
";Field1, MinDesQm, MinDesignFlow,  double, 0.0 ,         ParamStopped, Qm(t/h)\n"
";\n"
";[CustomDataTags1]\n"
";;       Symbol,   Tag,            Type,   DefaultValue, Options,      Conversion\n"
";Field0, ExtraDesc, ,  Str, "",         ,\n"
"\n"
";=========================================================================\n"
"\n"
";[Selection0]\n"
";;        PartialTag,   InsertDefault,  Cnv\n"
";SelectName = PipeSpecification\n"
";\n"
";Field0 = Eqn.Name,     Darcy DP,     ,\n"
";Field1 = Eqn.Rqd_Len,  100,            m,\n"
";Field2 = Eqn.Diam,     10,             cm,\n"
";Field3 = CD.MaxDesQm,  15,             t/h,\n"
";\n"
";;         Select,          Field0,     Field1,  Field2,  Field3\n"
";Select0=  Short Pipe,   Darcy DP,  10,      0.2,  8\n"
";Select1=  Medium Pipe,  Darcy DP,  100,     0.2,  12  \n"
";Select2=  Long Pipe,    Darcy DP,  1000,    0.4,  16  \n"
";\n"
";[Selection1]\n"
";SelectName = ?\n"
";Select= ???\n"
";;       PartTag,      InsertDefault,\n"
";Field0= ? \n"
";Field1= ? \n"
";Field2= ?\n"
";\n"
";Select0=  \n"
";Select1=  \n"
";Select2=  \n"
"\n"
";=========================================================================\n"
"\n"
";[DBSelection0]\n"
";SelectName = PipeSpecification\n"
";DBSource = .\\CfgFiles\\Catalogs\\Pipes.mdb\n"
";Table  =  Pipes\n"
";SQLSelect = Some SQL select statement\n"
";;       FieldName,    Type?,    Cnv,\n"
";Select = PartNumber,  STR,      ,\n"
";Field0 = CD.PartNumber, STR,    ,\n"
";Field1 = PipeLength,  R8,       m,\n"
";Field2 = Diameter,    R8,       mm,\n"
"\n"
";=========================================================================\n"
"\n"
";[Report0]\n"
";ReportName=Simple\n"
";;       Tag,         PartTag,      Cnvs,\n"
";Field0= Mass flow,   Qm,     , t/h\n"
";\n"
";[Report1]\n"
";ReportName=Complex\n"
";;       Tag,         PartTag,      Cnvs,\n"
";Field0= Mass flow,   Qm,     , t/h\n"
";Field1= FlwEquation, Eqn.Name,     , \n"
";Field2= Length,      Eqn.Rqd_Len,  m, \n"
";Field3= Diameter,    Eqn.Diam,     mm, \n"
"\n"
";=========================================================================\n"
"\n";
const int DefaultSCOLen = strlen(DefaultSCO);

flag TagObjClass::CreateClassOptions(LPCSTR File)
  {
  FILE* f = fopen(File, "wt");
  if (f)
    {
    fwrite(DefaultSCO, sizeof(char), DefaultSCOLen, f);
    fclose(f);

    CProfINIFile SCO(File);
    SCO.WrStr("Options", "TagInitialID",      m_sTagInitialID()?m_sTagInitialID():"");
    SCO.WrStr("Options", "TagFormat",         m_sTagFormat()?m_sTagFormat():"");
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

flag TagObjClass::ReadClassOptions(LPCSTR Path)
  {
  Strng Fn(Path);
  Strng E;
  
  CProfINIFile Cfg(CfgFile());
  
  if (Cfg.RdInt("ClassOptions", sClassIdNV(), 0)==0)
    return false;

  if (!FnCreatePath((LPSTR)Path, E))
    return false;

  Fn.FnCheckEndBSlash();
  Fn+=sClassIdNV;
  Fn.FnCheckExtension(".sco");
  dbgpln("LoadClassOptions: %-25s %s", sClassIdNV(), Fn);
  if (!FileExists(Fn()))
    {
    //CProfINIFile Cfg(CfgFile());
    //for (int i=0; ; i++)
    //  {
    //  Field
    //  }

    CreateClassOptions(Fn());
    }

  if (FileExists(Fn()))
    {
    CProfINIFile SCO(Fn());
    m_sTagInitialID = SCO.RdStr("Options", "TagInitialID", m_sTagInitialID()?m_sTagInitialID():"");
    m_sTagFormat    = SCO.RdStr("Options", "TagFormat", m_sTagFormat()?m_sTagFormat():"");
    m_sTagInitialID.LRTrim();
    m_sTagFormat.LRTrim();
    
#if WITHEQUIPSPECS
    m_CustomData.Load(SCO);
    m_SelectData.Load(SCO);
#endif
    return true;
    }
  return false;
  };
#endif

//===========================================================================
//
//
//
//===========================================================================

#if WITHEQUIPSPECS

CTOCustomData::CDataSet::CField::CField(TagObjClass * pClass, int Index) : \
m_pClass(pClass), m_iIndex(Index)
  {
  m_iType     = tt_NULL;
  m_iOptions  = 0;
  m_iCnv      = 0;
  //  int           m_iCnv;
  };
CTOCustomData::CDataSet::CField::~CField()
  {
  };

static struct {LPCSTR m_Str; DDEF_Flags m_Mask; } gs_OptionStrs[]=
  {
    {"NOCOMPARE",         DDEF_NOCOMPARE},
    {"NAN_OK",            DDEF_NAN_OK},
    {"NONDB",             DDEF_NONDB},
    {"NOFILE",            DDEF_NOFILE},
    {"NOVIEW",            DDEF_NOVIEW},
    {"NOSNAPSHOT",        DDEF_NOSNAPSHOT},
    {"NOSCENARIO",        DDEF_NOSCENARIO},
    {"CHECKBOX",          DDEF_CHECKBOX},
    {"PARAMSTOPPED",      DDEF_PARAMSTOPPED},
    {"PARAM",             DDEF_PARAM},
    {"RESULT",            DDEF_RESULT},
    {"SETONCHANGE",       DDEF_SETONCHANGE},
    {"HIDDEN",            DDEF_HIDDEN},
    {"HIDEIFZERO",        DDEF_HIDEIFZERO},
    {"PARAMCONSTRUCT",    DDEF_PARAMCONSTRUCT},
    {"TAGPARM",           DDEF_TAGPARM},
    {"KEEPHISTORY",       DDEF_KEEPHISTORY},
    {},
  };

bool CTOCustomData::CDataSet::CField::Load(CProfINIFile & SCO, LPCSTR FieldDefn)
  {
  Strng FieldDefnMem(FieldDefn);
  CSVColArray C;
  int iQuote=0;
  int NToks=ParseCSVTokens((LPSTR)FieldDefn,C, iQuote);
  if (NToks>=5)
    {
    m_sSym        = C[0];
    m_sTag        = C[1];
    m_iType       = tt_StringType(C[2]);
    if (m_iType==tt_NULL)
      LogError(m_pClass->ClassId(), 0, "CustomData Bad Type String:%s", C[2]);
    m_sDefaultValue = C[3];
    
    m_iOptions    = 0;

    CSVColArray CO;
    int iCOQuote=0;
    int NOpts=ParseCSVTokens(C[4], CO, iCOQuote, "|");
    if (NOpts>0)
      {
      for (int o=0; o<NOpts; o++)
        {
        for (int i=0; gs_OptionStrs[i].m_Str; i++)
          {
          if (stricmp(CO[o], gs_OptionStrs[i].m_Str)==0)
            {
            m_iOptions |= gs_OptionStrs[i].m_Mask;
            break;
            }
          }
        }
      }

    if (NToks>=5 && strlen(C[5])>0)
      {
      CSVColArray CC;
      int iCQuote=0;
      int NCnvs=ParseCSVFunction(C[5], CC, iCQuote);
      if (NCnvs==2)
        {
        m_iCnv = gs_CnvsMngr.FindCnv(CC[0]);
        if (m_iCnv<0)
          LogError(m_pClass->ClassId(), 0, "CustomData Bad Cnv Family:%s", CC[0]);
        if (Cnvs[m_iCnv]->Find(CC[1]))
          m_sCnvText=CC[1];
        else
          LogError(m_pClass->ClassId(), 0, "CustomData Bad Cnv:%s", CC[1]);
        }
      else
        LogError(m_pClass->ClassId(), 0, "CustomData Bad Cnv String:%s", C[5]);
      }
    else
      {
      m_iCnv        = 0;
      m_sCnvText    = "";
      }
    return true;
    }
  LogError(m_pClass->ClassId(), 0, "CustomData Bad Field Defn : %s", FieldDefnMem());
  return false;
  };

inline LPSTR NotNull(Strng &X) { return X()?X():""; };
void CTOCustomData::CDataSet::CField::BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pOwner)
  {
  switch (m_iType)
    {
    case tt_Double:
      DDB.Double(NotNull(m_sTag), NotNull(m_sSym), m_iCnv, NotNull(m_sCnvText), (double*)pOwner->m_CustomData[m_iIndex],   pOwner, m_iOptions);
      break;
    case tt_Strng:
      DDB.String(NotNull(m_sTag), NotNull(m_sSym), m_iCnv, NotNull(m_sCnvText), (Strng*)pOwner->m_CustomData[m_iIndex],   pOwner, m_iOptions);
      break;
    default:
      _asm int 3;
    }
  };

//===========================================================================

static Strng EnsureTagOK(LPSTR DspName)
  {
  Strng X(DspName);
  for (int i=0; i<X.GetLength(); i++)
    {
    if (isalnum(X[i])==0)   
      X[i]='_';
    }
  return X;
  };

CTOCustomData::CDataSet::CDataSet(TagObjClass * pClass) : \
m_pClass(pClass)  
  { 
  };

CTOCustomData::CDataSet::~CDataSet()
  { 
  Clear(); 
  };

bool CTOCustomData::CDataSet::Load(CProfINIFile & SCO, LPCSTR DataSetName)
  {
  m_sName=DataSetName;

  Strng Section("Custom.");
  Section+=DataSetName;

  m_sDspName     = SCO.RdStr(Section(), "Name", DataSetName);
  m_sDspNameTag  = EnsureTagOK(m_sDspName());

  Strng Entry;
  for (int f=0; ; f++)
    {
    Entry.Set("Field.%i", f);
    Strng Defn=SCO.RdStr(Section(), Entry(), "");
    if (Defn.GetLength()>0)
      {
      m_Fields.Add(new CField(m_pClass, f));
      m_Fields[f]->Load(SCO, Defn());
      }
    else
      break;
    }
  if (f==0)
    {
    LogError(m_pClass->ClassId(), 0, "CustomData DataSet is Empty : %s", m_sName());
    return false;
    }
  return true;
  }

void CTOCustomData::CDataSet::Clear()
  {
  for (int i=0; i<m_Fields.GetCount(); i++)
    delete m_Fields[i];
  };

//===========================================================================

CTOCustomData::CTOCustomData(TagObjClass * pClass) : \
m_pClass(pClass)
  {
  };
CTOCustomData::~CTOCustomData()
  {
  Clear();
  };

bool CTOCustomData::Load(CProfINIFile & SCO)
  {
  Clear();
  Strng Sets=SCO.RdStr("Options", "CustomDataTables");
  if (Sets.GetLength()==0)
    return true;

  m_DDBDataSets.Add(0, " ");
  CSVColArray C;
  int iQuote=0;
  int NSets=ParseCSVTokens(Sets(),C, iQuote);
  if (NSets>0)
    {
    bool OK=true;
    for (int s=0; s<NSets; s++)
      {
      CDataSet *p = new CDataSet(m_pClass);
      if (p->Load(SCO, C[s]))
        {
        m_DataSets.Add(p);
        m_DDBDataSets.Add(s+1, p->m_sDspName());
        }
      else
        {
        delete p;
        };
      }
    DbgDump();
    return true;
    }
  DbgDump();
  return false;
  }

void CTOCustomData::Clear()
  {
  for (int i=0; i<m_DataSets.GetCount(); i++)
    delete m_DataSets[i];
  m_DDBDataSets.Empty();
  };

void CTOCustomData::BuildDataDefnHead(DataDefnBlk & DDB, TaggedObject * pOwner)
  {
  DDB.Long      ("CustomData",        "", DC_, "", xidTOCustomDataSelect,  pOwner, isParm|SetOnChange, DDBSetNames());
  //CTOCustomData::CDataSet &DataSet = *pOwner->CustomDataSelected();
  //DDB.Page("CD", DDB_RqdPage);
  //Strng Tbl(m_pClass->ClassIdNoVersion());
  //Tbl+="_CD";
  //if (DDB.BeginObject(pOwner, "CD", Tbl()))
  //  {
  //  for (int f=0; f<DataSet.FieldCount(); f++)
  //    DataSet.Field(f).BuildDataDefn(DDB, pOwner);
  //  }
  //DDB.EndObject();
  };

void CTOCustomData::BuildDataDefnData(DataDefnBlk & DDB, TaggedObject * pOwner)
  {
  if (pOwner->CustomDataSelected())
    {
    CTOCustomData::CDataSet &DataSet = *pOwner->CustomDataSelected();
    DDB.Page("CD", DDB_RqdPage);
    Strng Tbl(m_pClass->ClassIdNoVersion());
    Tbl+="_CD";
    if (DDB.BeginObject(pOwner, "CD", Tbl()))
      {
      for (int f=0; f<DataSet.FieldCount(); f++)
        DataSet.Field(f).BuildDataDefn(DDB, pOwner);
      }
    DDB.EndObject();
    }
  };

flag CTOCustomData::DataXchg(DataChangeBlk &DCB, TaggedObject * pOwner)
  {
  switch (DCB.lHandle)
    {
    case xidTOCustomDataSelect:
      if (DCB.rL)
        pOwner->SetCustomDataIndex(*DCB.rL);
      DCB.L=pOwner->GetCustomDataIndex();
      return 1;
    default:;
    }
  
  return 0;
  };

void CTOCustomData::DbgDump()
  {
  dbgpln("======================================================================== CustomData:%s", m_pClass->ClassId());
  for (int i=0; i<m_DataSets.GetCount(); i++)
    {
    CDataSet & DS=*m_DataSets[i];
    dbgpln("----------------------- %s %s", DS.m_sName(), DS.m_sDspName());
    for (int f=0; f<DS.m_Fields.GetCount(); f++)
      {
      CDataSet::CField &F=*DS.m_Fields[f];
      Strng Cnv;
      Cnv.Set(F.m_iCnv>0?"%s(%s)":" ", Cnvs[F.m_iCnv]->Fam(), F.m_sCnvText()?F.m_sCnvText():"");
      dbgpln("Fld:%3i) %-15s %-15s %-10s %s", f, F.m_sTag()?F.m_sTag():"", F.m_sSym()?F.m_sSym():"", tt_TypeString(F.m_iType), Cnv());
      }
    }
  };

//========================================================================
//
//
//
//========================================================================

CTOSelectData::CDataSet::CField::CField(TagObjClass * pClass, int Index) : \
m_pClass(pClass), m_iIndex(Index)
  {
  m_iType    = -1;
  m_sCnvText = "";
  };
CTOSelectData::CDataSet::CField::~CField()
  {
  };

bool CTOSelectData::CDataSet::CField::Load(CProfINIFile & SCO, LPCSTR FieldDefn)
  {
  Strng FieldDefnMem(FieldDefn);
  CSVColArray C;
  int iQuote=0;
  int NToks=ParseCSVTokens((LPSTR)FieldDefn,C, iQuote);
  if (NToks>=4)
    {
    m_sName       = C[0];
    if (strlen(C[1])>0)
      {
      m_iType = tt_StringType(C[1]);
      if (m_iType==tt_NULL)
        LogError(m_pClass->ClassId(), 0, "SelectData Bad Type String:%s", C[2]);
      }
    else
      m_iType = tt_NULL;

    m_sDstPartTag   = (NToks>=3 ? C[2]:"");
    m_sCnvText      = (NToks>=4 ? C[3]:"");
    m_sDefaultValue = (NToks>=5 ? C[4]:"");

    m_iCnv          = 0; // still to be discovered
    return true;
    }

  LogError(m_pClass->ClassId(), 0, "SelectData Bad Field Defn : %s", FieldDefnMem());
  return false;
  };

//===========================================================================

CTOSelectData::CDataSet::CRecord::CRecord(TagObjClass * pClass, int Index) :\
m_pClass(pClass), m_iIndex(Index)
  {
  };

CTOSelectData::CDataSet::CRecord::~CRecord()
  {
  };

bool CTOSelectData::CDataSet::CRecord::Load(CProfINIFile & SCO, LPCSTR FieldDefn)
  {
  CSVColArray C;
  int iQuote=0;
  int NToks=ParseCSVTokens((LPSTR)FieldDefn,C, iQuote);
  for (int f=0; f<NToks; f++)
    m_Values.Add(Strng(C[f]));

  return false;
  };

//===========================================================================

CTOSelectData::CDataSet::CDataSet(TagObjClass * pClass, CTOSelectData *pSelect) : \
m_pClass(pClass), m_pSelect(pSelect)
  {
  m_pChildDS    = NULL;
  m_iDisplayFld = -1;
  m_iOrderByFld = -1;
  m_iChildTblFld = -1;
  m_iSelectedRecord = -1;
  };

CTOSelectData::CDataSet::~CDataSet()
  { 
  Clear(); 
  };

bool CTOSelectData::CDataSet::Load(CProfINIFile & SCO, LPCSTR DataSetName)
  {
  m_sName=DataSetName;

  Strng Section("Select.");
  Section+=DataSetName;

  m_sDspName        = SCO.RdStr(Section(), "Name",       DataSetName );
  m_sDspNameTag     = EnsureTagOK(m_sDspName());

  Strng Dsp = SCO.RdStr(Section(), "Display",    "");

  CSVColArray CO;
  int iCOQuote=0;
  int NOpts=ParseCSVTokens(Dsp(), CO, iCOQuote);
  m_sDisplayFld     = NOpts>0 ? CO[0]:"";
  m_sDisplayCnvTxt  = NOpts>1 ? CO[1]:"";
  m_sOrderByFld     = SCO.RdStr(Section(), "OrderBy",    "");

  m_iDisplayFld     = -1;
  m_iOrderByFld     = -1;
  m_iChildTblFld    = -1;
 
  Strng Entry;
  for (int f=0; ; f++)
    {
    Entry.Set("Field.%i", f);
    Strng Defn=SCO.RdStr(Section(), Entry(), "");
    if (Defn.GetLength()>0)
      {
      m_Fields.Add(new CField(m_pClass, f));
      m_Fields[f]->Load(SCO, Defn());
      if (m_Fields[f]->m_sName.XStrICmp(m_sDisplayFld)==0)
        m_iDisplayFld = f;
      if (m_Fields[f]->m_sName.XStrICmp(m_sOrderByFld)==0)
        m_iOrderByFld = f;

      }
    else
      break;
    }
  if (f==0)
    {
    LogError(m_pClass->ClassId(), 0, "SelectData No Fields : %s", m_sName());
    return false;
    }

  if (m_iDisplayFld<0)
    m_iDisplayFld = 0;
  if (m_iOrderByFld<0)
    m_iOrderByFld = 0;

  m_DDBRecords.Empty();
  m_DDBRecords.Add(0, " ");
  for (int r=0; ; r++)
    {
    Entry.Set("Record.%i", r);
    Strng Defn=SCO.RdStr(Section(), Entry(), "");
    if (Defn.GetLength()>0)
      {
      m_Records.Add(new CRecord(m_pClass, r));
      m_Records[r]->Load(SCO, Defn());
      m_DDBRecords.Add(r+1, m_Records[r]->m_Values[m_iDisplayFld]()); // Sort Later
      }
    else
      break;
    }

  // Load Dependant Tables 
  for (int f=0; f<m_Fields.GetCount(); f++)
    {
    if (m_Fields[f]->m_iType==tt_Table)
      {
      m_iChildTblFld= f;
      for (int r=0; r<m_Records.GetCount(); r++)
        {
        if (m_Records[r]->m_Values[f].GetLength()>0)
          m_pSelect->AddDataSet(SCO, m_Records[r]->m_Values[f]());
        }
      }
    }

  return true;
  }

void CTOSelectData::CDataSet::Clear()
  {
  for (int i=0; i<m_Fields.GetCount(); i++)
    delete m_Fields[i];
  for (int i=0; i<m_Records.GetCount(); i++)
    delete m_Records[i];
  m_DDBRecords.Empty();
  m_iSelectedRecord=-1;
  };

void CTOSelectData::CDataSet::BuildDataDefnHead(DataDefnBlk & DDB, TaggedObject * pOwner, int Level)
  {
  DDB.Long(m_sDspNameTag(),      "", DC_, "", xidTOSelectString0+Level,  pOwner, isParm|SetOnChange, &m_DDBRecords);
  if (m_pChildDS)
    m_pChildDS->BuildDataDefnHead(DDB, pOwner, Level+1);
  }

flag CTOSelectData::CDataSet::DataXchg(DataChangeBlk & DCB, TaggedObject * pOwner, int Level)
  {
  if (DCB.lHandle==(xidTOSelectString0+Level))
    {
    if (DCB.rL)
      {
      ChangeSelection(DCB.ForFileSnpScn()!=0, (*DCB.rL)-1, pOwner);
      }
    DCB.L=m_iSelectedRecord+1;
    return 1;
    }

  if (m_pChildDS && m_pChildDS->DataXchg(DCB, pOwner, Level+1))
    return 1;

  return 0;
  }

flag CTOSelectData::CDataSet::ChangeSelection(bool ForFiling, int SelectedRecord, TaggedObject * pOwner)
  {
  m_iSelectedRecord=SelectedRecord;
  dbgpln("Change Selection ---- %s", m_sDspName());
  dbgindent(2);
  if (m_iSelectedRecord>=0)
    {
    CRecord &R = *m_Records[m_iSelectedRecord];
    bool ChildDone=false;
    for (int f=0; f<m_Fields.GetCount(); f++)
      {
      CField & F=*m_Fields[f];
      if (F.m_iType==tt_Table)
        {
        if (!ChildDone)
          {
          Strng ChildTableName  = R.m_Values[f];
          m_pChildDS            = m_pSelect->FindDataSet(ChildTableName());
          ChildDone             = m_pChildDS!=NULL;
          dbgpln("Select    %-20s %s", F.m_sName(), ChildTableName());
          }
        }
      else if (!ForFiling && F.m_sDstPartTag.GetLength()>0)
        {
        Strng FullTag;
        FullTag.Set("%s.%s", pOwner->FullObjTag(), F.m_sDstPartTag());
        dbgpln("Set       %-20s %-30s = %s", F.m_sName(), FullTag(), R.m_Values[f]());
        //        pOwner->PutDefinedData()

        CPkDataList GetList;
        pOwner->GetDefinedData(FullTag(), GetList, NSHM_All|TABOpt_ForView|TABOpt_AllInfo, NULL);
        if (GetList.GetCount()>0)
          {
          CPkDataItem &I=*GetList.FirstItem();
          F.m_iType     = I.Type();
          F.m_iCnv      = I.CnvIndex();
          //F.m_sCnvText  = I.CnvTxt();

          CPkDataList PutList;
          CPkDataIter Iter;
          PkDataUnion PData;
          if (IsFloatData(F.m_iType))
            {
            PData.SetTypeDouble(tt_Double, Cnvs[F.m_iCnv]->Normal(SafeAtoF(R.m_Values[f]()), F.m_sCnvText()));  
            }
          else
            PData.SetTypeString(tt_Generic, R.m_Values[f]());  

          PutList.SetDataValue(FullTag(), 0, PData);
          pOwner->PutDefinedData(FullTag(), PutList, Iter, TABOpt_ForView|TABOPT_ValueStatus, NULL);
          //pOwner->SelectData()->SetTagValueStatus(FullTag(), NULL, VS_Set);
          }
        }
      }
    }
  else
    {
    // Leave all Fields or Reset to Defaults
    m_pChildDS=NULL;
    }
  dbgindent(-2);
  return true;
  }

//===========================================================================

CTOSelectData::CTOSelectData(TagObjClass * pClass) : \
m_pClass(pClass)
  {
  m_pRootDS = NULL;
  //m_TagStatusMap.InitHashTable(FindNextPrimeNumber(500));
  };

bool CTOSelectData::Load(CProfINIFile & SCO)
  {
  Clear();
  Strng Table;
  Table=SCO.RdStr("Options", "SelectDataTable");
  if (Table.GetLength()==0)
    return true;

  m_pRootDS = AddDataSet(SCO, Table());
  DbgDump();
  return m_pRootDS!=NULL;
  };

CTOSelectData::CDataSet * CTOSelectData::AddDataSet(CProfINIFile & SCO, LPSTR Table)
  {
  CDataSet *p=FindDataSet(Table);
  if (p)
    return p;

  p = new CDataSet(m_pClass, this);
  if (p->Load(SCO, Table))
    {
    m_DataSets.Add(p);
    return p;
    }
  delete p;
  return NULL;
  };

CTOSelectData::CDataSet * CTOSelectData::FindDataSet(LPSTR Table)
  {
  for (int d=0; d<m_DataSets.GetCount(); d++)
    {
    if (m_DataSets[d]->m_sName.XStrICmp(Table)==0)
      return m_DataSets[d];
    }
  return NULL;
  };

void CTOSelectData::Clear()
  {
  for (int i=0; i<m_DataSets.GetCount(); i++)
    delete m_DataSets[i];

  POSITION Pos=m_TagStatusList.GetHeadPosition();
  while (Pos)
    {
    delete m_TagStatusList.GetNext(Pos);
    }
  };

void CTOSelectData::BuildDataDefnHead(DataDefnBlk & DDB, TaggedObject * pOwner)
  {
  if (m_pRootDS)
    {
    m_pRootDS->BuildDataDefnHead(DDB, pOwner, 0);
    //DDB.String      ("Manufacturer",      "", DC_, "", xidTOSelectString0,  pOwner, isParm);
  //DDB.String      ("Chris_Pipes",       "", DC_, "", xidTOSelectString1,  pOwner, isParm);
  //CTOCustomData::CDataSet &DataSet = *pOwner->CustomDataSelected();
  //DDB.Page("CD", DDB_RqdPage);
  //Strng Tbl(m_pClass->ClassIdNoVersion());
  //Tbl+="_CD";
  //if (DDB.BeginObject(pOwner, "CD", Tbl()))
  //  {
  //  for (int f=0; f<DataSet.FieldCount(); f++)
  //    DataSet.Field(f).BuildDataDefn(DDB, pOwner);
  //  }
  //DDB.EndObject();
    }
  };

flag CTOSelectData::DataXchg(DataChangeBlk &DCB, TaggedObject * pOwner)
  {
  if (m_pRootDS->DataXchg(DCB, pOwner, 0))
    return 1;
  
  return 0;
  };

void CTOSelectData::SetTagValueStatus(LPSTR Tag1, LPSTR Tag2, byte TagStatus)
  {
  CTagStatus *p;
  if (m_TagStatusMap.Lookup(Tag1, p))
    {
    p->m_Status=TagStatus;
    return;
    }
  if (Tag2 && m_TagStatusMap.Lookup(Tag2, p))
    {
    p->m_Status=TagStatus;
    return;
    }

  p = new CTagStatus;
  p->m_Tag1   = Tag1;
  if (Tag2)
    p->m_Tag2   = Tag2;
  p->m_Status = TagStatus;

  if ((INT_PTR)m_TagStatusMap.GetHashTableSize()<m_TagStatusList.GetCount())
    {
    //Rebuild
    m_TagStatusMap.RemoveAll();
    m_TagStatusMap.InitHashTable(FindNextPrimeNumber(m_TagStatusList.GetCount()+500));

    POSITION Pos=m_TagStatusList.GetHeadPosition();
    while (Pos)
      {
      CTagStatus *x = m_TagStatusList.GetNext(Pos);
      m_TagStatusMap.SetAt(x->m_Tag1(), x);
      if (x->m_Tag2())
        m_TagStatusMap.SetAt(x->m_Tag2(), x);
      }
    }
  m_TagStatusList.AddTail(p);
  m_TagStatusMap.SetAt(p->m_Tag1(), p);
  if (p->m_Tag2())
    m_TagStatusMap.SetAt(p->m_Tag2(), p);

  //m_TagStatusMap.InitHashTable(FindNextPrimeNumber(500));
  };
byte CTOSelectData::GetTagValueStatus(LPSTR FullTag)
  {
  CTagStatus *p;
  if (m_TagStatusMap.Lookup(FullTag, p))
    {
    return p->m_Status;
    }
  return VS_None;
  };

flag CTOSelectData::GetFirstClassVariableStatusEntry(POSITION & Pos)
  {
  Pos=m_TagStatusList.GetHeadPosition();
  return (Pos!=NULL);
  }

flag CTOSelectData::GetNextClassVariableStatusEntry(POSITION & Pos, Strng &Tag1, Strng &Tag2, CTagValueStatus & Status)
  {
  if (Pos==NULL)
    return false;
  CTagStatus *p=m_TagStatusList.GetNext(Pos);
  if (p)
    {
    Tag1    = p->m_Tag1;
    Tag2    = p->m_Tag2;
    Status  = p->m_Status;
    return true;
    }
  return false;
  };

void CTOSelectData::DbgDump()
  {
  dbgpln("======================================================================== SelectData:%s", m_pClass->ClassId());
  for (int i=0; i<m_DataSets.GetCount(); i++)
    {
    CDataSet & DS=*m_DataSets[i];
    dbgpln("----------------------- %s %s", DS.m_sName(), DS.m_sDspName());
    for (int f=0; f<DS.m_Fields.GetCount(); f++)
      {
      CDataSet::CField &F=*DS.m_Fields[f];
      Strng Cnv;
      Cnv.Set(F.m_iCnv>0?"%s(%s)":" ", Cnvs[F.m_iCnv]->Fam(), F.m_sCnvText()?F.m_sCnvText():"");
      dbgpln("Fld:%3i) %-15s %-10s %s %-25s %s", f, F.m_sName(), tt_TypeString(F.m_iType), Cnv(), F.m_sDstPartTag()?F.m_sDstPartTag():"", F.m_sDefaultValue()?F.m_sDefaultValue():"");
      }
    for (int r=0; r<DS.m_Records.GetCount(); r++)
      {
      CDataSet::CRecord &R=*DS.m_Records[r];
      dbgp("Rec%3i) ", r);
      for (int f=0; f<R.m_Values.GetCount(); f++)
        dbgp("%15s,",R.m_Values[f]()?R.m_Values[f]():"");
      dbgpln("");
      }
    }
  };

#endif

//===========================================================================
//
//
//
//===========================================================================

INLINE DDBShortcut & DDBShortcut::operator=(const DDBShortcut & X)
  {
  *((DDBDataInfo*)this)=X;
  //DDBDataInfo XXX=X;
  fOK = X.fOK;
  DataVer = X.DataVer;
  sFullItemTag = X.sFullItemTag;
  sTag = X.sTag;
  sSym = X.sSym;
  sCnvTxt = X.sCnvTxt;
  sCnvFam = X.sCnvFam;
  ValueLstMem = X.ValueLstMem;
  return *this;
  }

INLINE void DDBShortcut::Set(DDBDataInfo * pDI, LPSTR FullItemTag, dword CurDataVer)
  {
  *(DDBDataInfo*)this=*pDI;
  fOK=True;
  DataVer.dwDataVerObj=CurDataVer;
  DataVer.dwDataVerGlbl=TaggedObject::dwGlblDataVerNo;
  sFullItemTag=FullItemTag;
  m_bCurrentComplete=true;
  m_sCurrentTag=FullItemTag;
   // Overcome the possible volatility of these strings;

  if (DataDefnBlk::debugging&0x4)
    dbgpln("Set ShortCut %08x %08x      %6i:%6i %s", this, pThis, CurDataVer, TaggedObject::dwGlblDataVerNo, FullItemTag);

  sTag    = pTag;
  sSym    = pSym;
  sCnvTxt = pCnvTxt;
//  sCnvFam = pCnvFam;
  if (pValueLstMem)
    {
    ValueLstMem=*pValueLstMem;
    pValueLstMem=&ValueLstMem;
    pValueLst=ValueLstMem.List();
    }
  else if (pValueLst)
    {
    ValueLstMem=*pValueLst;
    //pValueLstMem=&ValueLstMem;
    pValueLst=ValueLstMem.List();
    }

  pTag    = sTag.Buffer();
  pSym    = sSym.Buffer();
  pCnvTxt = sCnvTxt.Buffer();
  pCnvFam = sCnvFam.Buffer();
  };

INLINE flag DDBShortcut::TestShortcut(char *FullItemTag, dword ObjectVersion)
  {
  if (fOK &&
      (DataVer.dwDataVerObj==ObjectVersion) &&
      (DataVer.dwDataVerGlbl==TaggedObject::dwGlblDataVerNo)
      )
    {
    if (sFullItemTag.XStrICmp(FullItemTag)==0)
      {
      if (DataDefnBlk::debugging&0x4)
        dbgpln("Shortcut FOUND  %s", FullItemTag);
      return True;
      }
    }
  if (DataDefnBlk::debugging)
    {
    dbgpln("Shortcut BAD %08x %08x %s  %6i:%6i  %s", this, pThis, fOK ? "OK ":"Bad",DataVer.dwDataVerObj,DataVer.dwDataVerGlbl, sFullItemTag());
    dbgpln("             %8s %8s %s  %6i:%6i  %s", "", "", "   ", ObjectVersion, TaggedObject::dwGlblDataVerNo,  FullItemTag);
    dbgpln("             %8s %8s %s  %6i:", "", "", "   ", m_pRootObj/*pThis*/->CurrentDataVersion());
    }
  return False;
  }

//===========================================================================
//
//
//
//===========================================================================

#if DoDDBChecks
#ifdef _RELEASE
int gs_DevelopementChecks=0;
#else
int gs_DevelopementChecks=1;
#endif

void DevelopementChecksOn()
  {
  gs_DevelopementChecks++;
  };
void DevelopementChecksOff()
  {
  gs_DevelopementChecks--;
  };

int bStartChecking        = 1;
int bDoneCnvWarningOnce   = 0;
int bContinueChecking     = bStartChecking;
int bDoDuplicateWarnings  = 0;

long DataDefnBlk::lOpenCnt=0;
DDBCheckClassArray    DataDefnBlk::ClassList;
DDBCheckClassMap      DataDefnBlk::ClassMap;
//DDBCheckDuplicateMap  DataDefnBlk::DuplicateMap;

DDBCheckClass * DataDefnBlk::FindClass(char * ClassName)
  {
  DDBCheckClass *CC;
  char CN[1024];
  strcpy(CN, ClassName);
  char * pDash=strchr(CN, '-');
  if (pDash)
    *pDash=0;
  if (!ClassMap.Lookup(CN, CC))
    {
    CC= new DDBCheckClass;
    CC->m_ClassName=CN;
    CC->m_Map.InitHashTable(FindNextPrimeNumber(DDBCheckListSize));
    CC->m_Vars.SetSize(0,DDBCheckListSize);
    ClassList.Add(CC);
    ClassMap.SetAt(CC->m_ClassName(), CC);
    }
  return CC;
  }

#pragma optimize("", off)

void DataDefnBlk::DoChecks(DWORD RetAddress, dword UserInfo)
  {
  if (!bContinueChecking || !RetAddress)
    return;

  DDBCheckClass &CheckClass=*Chk[lCallLevel].pClass;
  DDBCheckMap &CheckMap=CheckClass.m_Map;
  DDBCheckArray &CheckVars=CheckClass.m_Vars;
  Strng T, S;
  DDBCheckItem *CRT, *CRS;
  flag AltName=false;

  S.Concat(CheckClass.m_ClassName(), ":", pSym && pSym[0] ? pSym : pTag);
//  if (S.XStrICmp("SpCondui:VF")==0)
//    DoBreak();
//  dbgpln("  -- %s %08x %8i", S(), RetAddress, UserInfo);
//  dbgpln("  -- %5i %s %08x %8i", Chk[lCallLevel].lOpenCnts, S(), RetAddress, UserInfo);

  flag DoUniqueTest=(PrjFileVerNo()==NewPrjFileVerNo());
  static char * Known[] = { NULL, "SpConduit:VF", "SpContainer:VF", "SpImage:VF", NULL};
  for (int ik=0; DoUniqueTest && Known[ik]; ik++)
    if (S.XStrICmp(Known[ik])==0)
      DoUniqueTest=false;

  if (DoUniqueTest && (iFlags&DupHandlesOk)==0)
    {
    flag GotSym=false;
    if (pSym && strlen(pSym))
      {
      GotSym=true;
      S.Concat(Chk[lCallLevel].pTagStart, strlen(Chk[lCallLevel].pTagStart)?".":"", pSym);
      if (!CheckMap.Lookup(S(), CRS))
        {
        CRS = new DDBCheckItem;
        CRS->m_Tag=S();
        CRS->m_Add=RetAddress;
        CRS->m_FileVer=PrjFileVerNo();
        CRS->m_AltName=AltName;
        CRS->m_Reported=false;
        CRS->m_iFlags=iFlags;
        CRS->m_UserInfo=UserInfo;
        AltName=true;
        CheckMap.SetAt(CRS->m_Tag(), CRS);
        CheckVars.Add(CRS);
        //dbgpln("  %08x %2i '%s:%s'", RetAddress, PrjFileVerNo(), CheckClass.m_ClassName(), S());
        }
      else if (CRS->m_lOpenCnt==Chk[lCallLevel].lOpenCnts)// ||
  //             CRS->m_UserInfo!=UserInfo ||
  //             CRS->m_Add!=RetAddress)// && CRS->m_FileVer==PrjFileVerNo())
        {
        if (CRS->m_FileVer<PrjFileVerNo())
          {
          CRS->m_Add=RetAddress;
          CRS->m_FileVer=PrjFileVerNo();
          }
        else
          {
          if (!CRS->m_Reported || bDoDuplicateWarnings)
            {
            CRS->m_Reported=true;
            LogError("TAGOBJ", 0/*LF_DoAfxMsgBox|LF_Exclamation*/, "Duplicate Symbol: %s:%s", CheckClass.m_ClassName(), S());
            }
          //  }
          }
        }
      CRS->m_lOpenCnt=Chk[lCallLevel].lOpenCnts;
      }

    if (pTag && strlen(pTag) && (!GotSym || _stricmp(pTag, pSym)!=0))
      {
      T.Concat(Chk[lCallLevel].pTagStart, strlen(Chk[lCallLevel].pTagStart)?".":"", pTag);
      if (!CheckMap.Lookup(T(), CRT))
        {
        CRT = new DDBCheckItem;
        CRT->m_Tag=T();
        CRT->m_Add=RetAddress;
        CRT->m_FileVer=PrjFileVerNo();
        CRT->m_AltName=AltName;
        CRT->m_Reported=false;
        CRT->m_iFlags=iFlags;
        CRT->m_UserInfo=UserInfo;
        AltName=true;
        CheckMap.SetAt(CRT->m_Tag(), CRT);
        CheckVars.Add(CRT);
        //dbgpln("  %08x %2i '%s:%s'", RetAddress, PrjFileVerNo(), CheckClass.m_ClassName(), T());
        }
      else if (CRT->m_lOpenCnt==Chk[lCallLevel].lOpenCnts)// ||
               //CRT->m_UserInfo!=UserInfo ||
               //CRT->m_Add!=RetAddress)// && CRT->m_FileVer==PrjFileVerNo())
        {
        if (CRT->m_FileVer<PrjFileVerNo())
          {
          CRT->m_Add=RetAddress;
          CRT->m_FileVer=PrjFileVerNo();
          }
        else
          {
          if (!CRT->m_Reported || bDoDuplicateWarnings)
            {
            CRT->m_Reported=true;
            LogError("TAGOBJ", 0/*LF_DoAfxMsgBox|LF_Exclamation*/, "Duplicate Symbol: %s:%s", CheckClass.m_ClassName(), T());
            }
          }
        }
      CRT->m_lOpenCnt=Chk[lCallLevel].lOpenCnts;
      }
    }

  //check that reserved tags aren't used for filing...
  static char * ReservedTags[] = { "Tag", "RevNo", NULL};
  static int    ReservedLvls[] = { 1, 0 };
  if ((iFlags & DDEF_NOFILE)==0)
    {
    for (int ir=0; ReservedTags[ir]; ir++)
      if (lCallLevel>ReservedLvls[ir])
        {
        bool Er=(pSym && _stricmp(pSym, ReservedTags[ir])==0);
        if (!Er && (pSym==NULL || strlen(pSym)==0))
          Er=(pTag && _stricmp(pTag, ReservedTags[ir])==0);
        if (Er)
          {
          char Buff[1024];
          sprintf(Buff, "Cannot use reserved tag or symbol:\n\n  %s:%s%s%s",
            CheckClass.m_ClassName(), Chk[lCallLevel].pTagStart,
            strlen(Chk[lCallLevel].pTagStart)?".":"", (pSym && strlen(pSym)>0) ? pSym : pTag);
          TRACE("%s\n", Buff);
          dbgpln(Buff);
          LogError("TAGOBJ", 0/*LF_DoAfxMsgBox|LF_Exclamation*/, Buff);
          }
        }
    }

  //check tag or sym specefied...
  if ((pTag==NULL || strlen(pTag)==0) && (pSym==NULL || strlen(pSym)==0))
    {
    char Buff[1024];
    sprintf(Buff, "A tag or symbol must be specified!  (tag in %s)", CheckClass.m_ClassName());
    TRACE("%s\n", Buff);
    LogError("TAGOBJ", 0/*LF_DoAfxMsgBox|LF_Exclamation*/, Buff);
    }

  //check tag length...
  if (pTag && strlen(pTag)>=SCDPartTagLen)
    {
    char Buff[1024];
    sprintf(Buff, "Tag name too long:\n\n  %s:%s%s%s",
      CheckClass.m_ClassName(), Chk[lCallLevel].pTagStart,
      strlen(Chk[lCallLevel].pTagStart)?".":"",pTag);
    TRACE("%s\n", Buff);
    dbgpln(Buff);
    LogError("TAGOBJ", 0/*LF_DoAfxMsgBox|LF_Exclamation*/, Buff);
    }
  //check Sym length...
  if (pSym && strlen(pSym)>=SCDPartTagLen)
    {
    char Buff[1024];
    sprintf(Buff, "Symbol name too long:\n\n  %s:%s%s%s",
      CheckClass.m_ClassName(), Chk[lCallLevel].pTagStart,
      strlen(Chk[lCallLevel].pTagStart)?".":"", pSym);
    TRACE("%s\n", Buff);
    dbgpln(Buff);
    LogError("TAGOBJ", 0/*LF_DoAfxMsgBox|LF_Exclamation*/, Buff);//ASSERT_ALWAYS(FALSE, Buff);
    }
  //check Sym...
  //check tag...
  if ((iFlags & DDEF_BUTTON)==0 || (iFlags & DDEF_CHECKBOX)!=0)
    {
    if (pTag && strlen(pTag)>0 && !TaggedObject::TestValidFieldName(pTag))
      {
      char Buff[1024];
      sprintf(Buff, "Field Tag name may be invalid:\n\n  %s:%s%s%s",
        CheckClass.m_ClassName(), Chk[lCallLevel].pTagStart,
        strlen(Chk[lCallLevel].pTagStart)?".":"", pTag);
      TRACE("%s\n", Buff);
      dbgpln(Buff);
      LogError("TAGOBJ", 0/*LF_DoAfxMsgBox|LF_Exclamation*/, Buff);//ASSERT_ALWAYS(FALSE, Buff);
      }
    if (pSym && strlen(pSym)>0 && !TaggedObject::TestValidFieldName(pSym))
      {
      char Buff[1024];
      sprintf(Buff, "Field Symbol name may be invalid:\n\n  %s:%s%s%s",
        CheckClass.m_ClassName(), Chk[lCallLevel].pTagStart,
        strlen(Chk[lCallLevel].pTagStart)?".":"", pSym);
      TRACE("%s\n", Buff);
      dbgpln(Buff);
      LogError("TAGOBJ", 0/*LF_DoAfxMsgBox|LF_Exclamation*/, Buff);//ASSERT_ALWAYS(FALSE, Buff);
      }
    }
  //check cnv...
//    if (iCnv!=0 && pCnvTxt && strlen(pCnvTxt)>0 && gs_CnvsMngr.FindSecCnv(iCnv, pCnvTxt)==NULL)
//      {
//      char Buff[1024];
//      sprintf(Buff, "Warning: Invalid CnvText '%s' for Cnv '%s'  %s:%s.%s", pCnvTxt, Cnvs[iCnv]->Fam(), pTag, pChkTagCls[lCallLevel]->ClassIdNoVersion(), Chk[lCallLevel].pTagStart);
//      TRACE("%s\n", Buff);
//      dbgpln(Buff);
//      if (!bDoneCnvWarningOnce)
//        {
//        ASSERT_ALWAYS(FALSE, Buff);
//        bDoneCnvWarningOnce = 1;
//        }
//      }
  };

#pragma optimize("", on)

void DataDefnBlk::ClearAllChecks()
  {
  StartupLock.Lock();

  ClassMap.RemoveAll();
  for (int i=0; i<ClassList.GetSize(); i++)
    {
    DDBCheckClass &C=*ClassList[i];
    C.m_Map.RemoveAll();
    for (int j=0; j<C.m_Vars.GetSize(); j++)
      delete C.m_Vars[j];
    C.m_Map.InitHashTable(FindNextPrimeNumber(DDBCheckListSize));
    C.m_Vars.SetSize(0,DDBCheckListSize);
    delete ClassList[i];
    }
  ClassList.SetSize(0, DDBCheckListSize);
  ClassMap.InitHashTable(FindNextPrimeNumber(DDBCheckListSize));
//  DuplicateMap.RemoveAll();

  StartupLock.Unlock();
  };

void DataDefnBlk::DumpAllChecks()
  {
  StartupLock.Lock();
  dbgpln("=====================================================================");
  CArray <int, int> Ind;
  Ind.SetSize(ClassList.GetSize());
  for (int i=0; i<ClassList.GetSize(); i++)
    {
    dbgp("%s\t", ClassList[i]->m_ClassName());
    Ind[i]=0;
    }
  dbgpln("");

  flag Done=false;
  for (i=0; !Done ; i++)
    {
    Done=true;
    for (int j=0; j<ClassList.GetSize(); j++)
      {
      DDBCheckClass &C=*ClassList[j];

      int &k=Ind[j];
      while ((k<C.m_Vars.GetSize()) &&
             (C.m_Vars[k]->m_AltName ||(C.m_Vars[k]->m_iFlags&DDEF_NOFILE)))
        k++;

      if (k<C.m_Vars.GetSize())
        {
        dbgp("%s\t", C.m_Vars[k]->m_Tag());
        Done=false;
        }
      else
        dbgp("\t");
      k++;
      }
    dbgpln("");
    }
  dbgpln("=====================================================================");
  StartupLock.Unlock();
  };
#endif

//===========================================================================
//
//
//
//===========================================================================

FilingControlBlock::FilingControlBlock(char * Root, char * Label, TOFilingTypes Type)
  {
  sRoot=Root;
  sLabel=Label;
  eType=Type;
  sXName="";
  sObjTag="";
  bCreateObj=0;
  bSaveAs=0;
  bFarCopy=0;
  xm_sOldPrjFiles="";
  hFile=NULL;
  dwStartPos=dwCurrentPos=dwEndPos=0;
  m_PrjFileVerNo=::PrjFileVerNo();
  }

//---------------------------------------------------------------------------

void FilingControlBlock::InitialiseForRead(HANDLE File, DWORD StartPos, DWORD EndPos)
  {
  hFile=File;
  dwStartPos=dwCurrentPos=StartPos;
  dwEndPos=EndPos;
  };

//---------------------------------------------------------------------------

void FilingControlBlock::InitialiseForWrite(HANDLE File, DWORD StartPos)
  {
  hFile=File;
  dwStartPos=dwCurrentPos=dwEndPos=StartPos;
  };

//---------------------------------------------------------------------------

void FilingControlBlock::SetSaveAs(char* pOldPrjFiles)
  {
  bSaveAs = 1;
  xm_sOldPrjFiles = pOldPrjFiles;
//  m_sPrevPrjFiles = m_sPrevPrjFiles.Left(m_sPrevPrjFiles.Length() - 4);
//  m_sPrevPrjFiles += '\\';
  bFarCopy = 1;
//DoBreak();
//#pragma chFIXIT(??????)
///*
//  if (_strnicmp(PrjPrevDirectory(), m_sPrevPrjFiles(), strlen(PrjPrevDirectory()))==0)
//    bFarCopy = 0;
//*/
  }

//---------------------------------------------------------------------------

BOOL FilingControlBlock::ReadFile(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead)
  {
  nNumberOfBytesToRead=Min(nNumberOfBytesToRead, dwEndPos-dwCurrentPos);
  flag OK=::ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, NULL);
  dwCurrentPos+=*lpNumberOfBytesRead;
  return OK;
  };

//---------------------------------------------------------------------------

BOOL FilingControlBlock::WriteFile(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten)
  {
  flag OK=::WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, NULL);
  dwCurrentPos+=*lpNumberOfBytesWritten;
  dwEndPos=Max(dwEndPos, dwCurrentPos);
  return OK;
  };

//---------------------------------------------------------------------------

DWORD FilingControlBlock::SetFilePointer(LONG lDistanceToMove, DWORD dwMoveMethod)
  {
  switch (dwMoveMethod)
    {
    case FILE_BEGIN:
      dwCurrentPos=::SetFilePointer(hFile, dwStartPos+lDistanceToMove, NULL, dwMoveMethod);
      break;
    case FILE_CURRENT:
      dwCurrentPos=::SetFilePointer(hFile, lDistanceToMove, NULL, dwMoveMethod);
      break;
    case FILE_END: ASSERT(0);
    }
  if (dwCurrentPos<dwStartPos)
    dwCurrentPos=::SetFilePointer(hFile, dwStartPos, NULL, FILE_BEGIN);
  else if (dwCurrentPos>dwEndPos)
    dwCurrentPos=::SetFilePointer(hFile, dwEndPos, NULL, FILE_BEGIN);
  return dwCurrentPos-dwStartPos;
  }

//---------------------------------------------------------------------------

BOOL FilingControlBlock::Eof(){ return dwCurrentPos>=dwEndPos;};

//===========================================================================
//
//
//
//===========================================================================

int     TaggedObject::m_DisplayTagsOnly=0;
int     TaggedObject::m_HideZeros=0;
#if WithNumericTags
int     TaggedObject::NumericTagsBad=1;
#endif
int     TaggedObject::NumericStartingTagsBad=1;
char    TaggedObject::NonNumericTagChr='#';
int     TaggedObject::bHoldValidateData=0;
CTaggedObjectList TaggedObject::m_List2Validate;
Strng   TaggedObject::sBadFieldNameChars("");
Strng   TaggedObject::sRepFieldNameChars(" ");

dword   TaggedObject::dwGlblDataVerNo=0;

long    TaggedObject::gs_DefNetMode=0;
long    TaggedObject::gs_DefNodeMode=0;
long    TaggedObject::gs_DefLinkMode=0;
long    TaggedObject::gs_DefHeatMode=0;
long    TaggedObject::gs_DefFlowMode=0;

long    TaggedObject::gs_MaxLinkMode=0;
long    TaggedObject::gs_MaxNodeMode=0;
long    TaggedObject::gs_MaxHeatMode=0;
long    TaggedObject::gs_MaxFlowMode=0;

long    TaggedObject::gs_AllowedLinkMode=0;
long    TaggedObject::gs_AllowedNodeMode=0;
long    TaggedObject::gs_AllowedHeatMode=0;
long    TaggedObject::gs_AllowedFlowMode=0;

long    TaggedObject::gs_AllowedNodeModes=NM_All|LFM_All; // Just allow these licensing will fix
long    TaggedObject::gs_AllowedLinkModes=NM_All|LFM_All; // Just allow these licensing will fix

bool    TaggedObject::gs_Revert2DefaultRqd=false;

flag    TaggedObject::fRunning=0;
flag    TaggedObject::fBuildingAccess=0;
flag    TaggedObject::bGlblResultsValid=false;
flag    TaggedObject::bGlblResultsAlwaysValid=false;
flag    TaggedObject::bGlblResultsValidChg=false;
flag    TaggedObject::bGlblResultsAlwaysValidChg=false;

GlblTopologyChg TaggedObject::GlblTopoChg = {NULL, 0};

//===========================================================================
//
//
//
//===========================================================================

//===========================================================================
//
//
//
//===========================================================================

TaggedObject::TaggedObject(TagObjClass* pClass_, LPSTR Tag_, TaggedObject *pAttach, TagObjAttachment eAttach)
#if WITHTAGMONITOR
: m_Monitor(this)
#endif
  {
  #if dbgTagObjAttach
  dbgpln("Construct %8x %s %s",this,pClass_->ClassId(), Tag_);
  #endif

  m_bNodeSolveMode=true;
  m_AllowedModes=0;
  m_FallBackModes=0;
  m_lRunMds=0;
  m_pRunMds=&m_lRunMds;

  m_FlowModeRqd=0;
  m_FlowModePass=0;
  m_FlowModeObj=NULL;

  m_pClass=pClass_;
  VERIFY(m_pClass!=NULL);
  m_pClass->IncInstances();
  m_sTag=Tag_;
  m_sPrefix="";
  bObjModified=false;
  bUnValidatedData=false;
  bDoSystemDefn=true;
  bSkipSnapLoad=false;

  dwObjDataVerNo=++m_pClass->dwObjDataVerNo;

  pAttachedTo=NULL;
  eAttachment=TOA_Unknown;
  pAttachments=NULL;
  pNxtAttachment=NULL;

  AttachMeToObj(pAttach, eAttach);

  m_iCustomData=-1;

  m_fActive=true;
  m_fActiveHold=false;
  m_fActiveHoldIfSS=false;
  fActiveHold_Auto=false;
  fActiveHoldOK_PB=true;
  fActiveHoldOK_Dyn=false;

  m_bFileIOBusy=false;

  fDoDbgBrk=False; // For debugging purposes

  m_pgpTO=NULL;

  m_lEqLink=-1;

  //// Generate DefaultGuid
  //UCHAR*psEqGUID=NULL;
  //UUID AGuid;
  //ASSERT_ALWAYS(UuidCreate(&AGuid)==RPC_S_OK, "Guid Not Created", __FILE__, __LINE__);
  //ASSERT_ALWAYS(UuidToString(&AGuid, &psEqGUID)==RPC_S_OK, "Guid Not Converted", __FILE__, __LINE__);
  //m_sEqGUID.Set("{%s}", psEqGUID);
  //RpcStringFree(&psEqGUID);

  m_sGuid = CreateGuidStr();

  m_lReportKey=0;
  }

//---------------------------------------------------------------------------

TaggedObject::~TaggedObject()
  {
  //if (m_psEqGUID)
  //  RpcStringFree(&m_psEqGUID);

  m_pClass->DecInstances();
  DetachMeFromOwner();
  while (pAttachments)
    pAttachments->DetachMeFromOwner();

  POSITION VPos=m_List2Validate.Find(this);
  if (VPos)
    m_List2Validate.RemoveAt(VPos);

  #if dbgTagObjAttach
  dbgpln("Destroy   %8x %s %s",this,m_pClass->ClassId(), sTag());
  #endif
  }

// -------------------------------------------------------------------------

void SetDefNetMode(long Mode)         
  { 
  TaggedObject::gs_DefNetMode  = (Mode & NM_All);  
  if ((TaggedObject::gs_DefNetMode & NM_All) == NM_All)
    __debugbreak();
  if (dbgSetModes) 
    dbgpln(">>~~SetDefNetMode  %08x", TaggedObject::gs_DefNetMode); 
  };
                                                                                                       
void SetDefNodeMode(long Mode)        
  { 
  TaggedObject::gs_DefNodeMode = (Mode & SM_All);  
  if ((TaggedObject::gs_DefNodeMode & SM_All) == SM_All)
    __debugbreak();
  if (dbgSetModes)  
    dbgpln(">>~~SetDefNodeMode %08x", TaggedObject::gs_DefNodeMode); 
  };
void SetDefLinkMode(long Mode)        
  { 
  TaggedObject::gs_DefLinkMode = (Mode & SM_All);  
  if ((TaggedObject::gs_DefLinkMode & SM_All) == SM_All)
    __debugbreak();
  if (dbgSetModes)  
    dbgpln(">>~~SetDefLinkMode %08x", TaggedObject::gs_DefLinkMode); 
  };
void SetDefHeatMode(long Mode)        
  { 
  TaggedObject::gs_DefHeatMode = (Mode & HM_All);  
  if ((TaggedObject::gs_DefHeatMode & HM_All) == HM_All)
    __debugbreak();
  if (dbgSetModes) 
    dbgpln(">>~~SetDefHeatMode %08x", TaggedObject::gs_DefHeatMode); 
  };
void SetDefFlowMode(long Mode)        
  { 
  TaggedObject::gs_DefFlowMode = (Mode & LFM_All); 
  if ((TaggedObject::gs_DefFlowMode & LFM_All) == LFM_All)
    __debugbreak();
  if (dbgSetModes)  
    dbgpln(">>~~SetDefFlowMode %08x", TaggedObject::gs_DefFlowMode);  
  };

static long Max2Mask(long MaxMode, long Mask)
  {
  long Ret=0; 
  while (MaxMode & Mask) 
    { 
    Ret |= MaxMode & Mask; 
    MaxMode = MaxMode >> 1;
    }
  return Ret;
  }

static long Modes2Max(long Modes, long Mask)
  {
  long Ret=1; 
  Modes &= Mask;

  for (;;) 
    { 
    Modes = Modes >> 1;
    if (!Modes)
      return Ret;
    Ret = Ret << 1;
    }
  return 0;
  }

long MaxModes2Mask(long MaxMode)
  {
  return Max2Mask(MaxMode, NM_All) | Max2Mask(MaxMode, SM_All) | Max2Mask(MaxMode, HM_All) | Max2Mask(MaxMode, LFM_All); 
  }

void SetMaxNodeMode(long Mode)
  { 
  Mode |= SM_Direct|SM_Inline; // AT Least Inline is Required

  long Allowed=Max2Mask(Mode, SM_All);

  TaggedObject::gs_MaxNodeMode      = Mode&SM_All; 
  TaggedObject::gs_AllowedNodeMode  = Allowed; 
  TaggedObject::gs_AllowedNodeModes = (TaggedObject::gs_AllowedNodeModes & ~SM_All) | Allowed; 
  if (dbgSetModes) 
    dbgpln(">>~~SetMaxNodeMode %08x %08x %08x", TaggedObject::gs_MaxNodeMode, TaggedObject::gs_AllowedNodeMode, TaggedObject::gs_AllowedNodeModes);
  };

void SetMaxLinkMode(long Mode)
  {
  Mode |= SM_Direct; // AT Least Direct is Required

  long Allowed=Max2Mask(Mode, SM_All);

  TaggedObject::gs_MaxLinkMode      = Mode&SM_All; 
  TaggedObject::gs_AllowedLinkMode  = Allowed; 
  TaggedObject::gs_AllowedLinkModes = (TaggedObject::gs_AllowedLinkModes & ~SM_All) | Allowed; 
  if (dbgSetModes) 
    dbgpln(">>~~SetMaxLinkMode %08x %08x %08x", TaggedObject::gs_MaxLinkMode, TaggedObject::gs_AllowedLinkMode, TaggedObject::gs_AllowedLinkModes);
  };
void SetMaxHeatMode(long Mode)
  {
  long Allowed=Max2Mask(Mode, HM_All);

  TaggedObject::gs_MaxHeatMode      = Mode&HM_All;
  TaggedObject::gs_AllowedHeatMode  = Allowed; 
  TaggedObject::gs_AllowedNodeModes = (TaggedObject::gs_AllowedNodeModes & ~HM_All) | Allowed; 
  TaggedObject::gs_AllowedLinkModes = (TaggedObject::gs_AllowedLinkModes & ~HM_All) | Allowed; 
  if (dbgSetModes) 
    dbgpln(">>~~SetMaxHeatMode %08x %08x", TaggedObject::gs_MaxHeatMode, TaggedObject::gs_AllowedHeatMode);
  };
void SetMaxFlowMode(long Mode)
  {
  long Allowed=Max2Mask(Mode, LFM_All);

  TaggedObject::gs_MaxFlowMode      = Mode&LFM_All;
  TaggedObject::gs_AllowedFlowMode  = Allowed; 
  TaggedObject::gs_AllowedNodeModes = (TaggedObject::gs_AllowedNodeModes & ~LFM_All) | Allowed; 
  TaggedObject::gs_AllowedLinkModes = (TaggedObject::gs_AllowedLinkModes & ~LFM_All) | Allowed; 
  if (dbgSetModes) 
    dbgpln(">>~~SetMaxFlowMode %08x %08x", TaggedObject::gs_MaxFlowMode, TaggedObject::gs_AllowedFlowMode);
  };

long AllowedNodeModes() { return TaggedObject::gs_AllowedNodeModes & SM_All; };
long AllowedLinkModes() { return TaggedObject::gs_AllowedLinkModes & SM_All; };
//long AllowedHeatModes() { return TaggedObject::gs_AllowedHeatMode & HM_All;  };
//long AllowedFlowModes() { return TaggedObject::gs_AllowedFlowMode & LFM_All; };

long MaxNodeMode() { return TaggedObject::gs_MaxNodeMode & SM_All; };
long MaxLinkMode() { return TaggedObject::gs_MaxLinkMode & SM_All; };
long MaxHeatMode() { return TaggedObject::gs_MaxHeatMode & HM_All;  };
long MaxFlowMode() { return TaggedObject::gs_MaxFlowMode & LFM_All; };

void TaggedObject::SetNetMethod(long Mode)
  {
  *m_pRunMds = (*m_pRunMds & ~NM_All) | Mode; 
  if (dbgSetModes) 
    dbgpln(">>~~NetMode     %08x", (*m_pRunMds & ~NM_All));
  };
void TaggedObject::SetNetMethod(LPCTSTR Mode)
  {
  SetNetMethod(String2GlblMode(Mode, NM_All)); };
void TaggedObject::SetSolveMethod(long Mode)
  {
  *m_pRunMds = (*m_pRunMds & ~SM_All) | (SM_All & Mode & m_AllowedModes);
  if (dbgSetModes) 
    dbgpln(">>~~SolveMethod %08x", (*m_pRunMds & ~SM_All));
  };
void TaggedObject::SetSolveMethod(LPCTSTR Mode)
  {
  SetSolveMethod(String2GlblMode(Mode, SM_All));
  };                     
void TaggedObject::SetHeatMethod(long Mode)
  {
  *m_pRunMds = (*m_pRunMds & ~HM_All) | (HM_All & Mode & m_AllowedModes);
  if (dbgSetModes) 
    dbgpln(">>~~HeatMethod  %08x", (*m_pRunMds & ~HM_All));
  };
void TaggedObject::SetHeatMethod(LPCTSTR Mode)
  {
  SetHeatMethod(String2GlblMode(Mode, HM_All));
  };

void TaggedObject::SetFlowModeLcl(long Mode)
  {         
  if (Mode&LFM_Propagate)
    {
    _asm int 3;
    //SetxFlowModeReqd(Mode);
    }
  else
    {
    *m_pRunMds = (*m_pRunMds & ~LFM_All) | (LFM_All & Mode & m_AllowedModes); // Will goto Default if not allowed
    if(dbgSetModes)
      dbgpln(">>~~FlowMethod %08x", (*m_pRunMds & ~LFM_All)); 
    }
  };
long TaggedObject::AllModes(long MdMsk)
  {
  return (NetMethod()|SolveMethod()|HeatMethod()) & MdMsk;
  };

long TaggedObject::NetMethod() // this needs to be worked on to allow probal and dyn flwsheets to be mixed.
  {
  long X;
  if (*m_pRunMds & NM_All)
    X=*m_pRunMds & NM_All;
  else
    X=DefNetMode();
  //if ((X & m_AllowedModes)==0 && (m_AllowedModes!=0))
  //  __debugbreak();
  //X = m_FallBackModes & NM_All;
  if (dbgGetModes) 
    dbgpln("NetMethod   = %-20s %-20s %s", GlblMode2LPTSTR(X, NM_All, eGM_Name), GlblMode2LPTSTR(X&m_AllowedModes, NM_All, eGM_Name), Tag());
  return X;
  };

LPTSTR TaggedObject::NetMethodStr()
  {
  if (*m_pRunMds & NM_All)
    return GlblMode2LPTSTR(*m_pRunMds, NM_All, eGM_Name);
  return GlblMode2LPTSTR(DefNetMode(), NM_All, eGM_Default);
  };

long TaggedObject::SolveMethod()
  {
  long X;
  if (NetMethod()==NM_Probal)
    {
    X=SM_Direct;
    }
  else
    {
    if (*m_pRunMds & SM_All)
      X=*m_pRunMds & SM_All;
    else if (m_bNodeSolveMode)
      X=DefNodeSolveMode();
    else
      X=DefLinkSolveMode();
    if ((X & m_AllowedModes)==0 && (m_AllowedModes &SM_All)!=0)
      X = m_FallBackModes & SM_All;
    }

  if (dbgGetModes) 
    dbgpln("SolveMethod = %-20s %-20s %s", GlblMode2LPTSTR(X, SM_All, eGM_Name), GlblMode2LPTSTR(X&m_AllowedModes, SM_All, eGM_Name), Tag());
  return X;

  };

LPTSTR TaggedObject::SolveMethodStr()
  {
  if (NetMethod()==NM_Probal)
    return GlblMode2LPTSTR(SM_Direct, SM_All, eGM_Name);
  if (*m_pRunMds & SM_All)
    return GlblMode2LPTSTR(*m_pRunMds, SM_All, eGM_Name);
  long X;
  if (m_bNodeSolveMode)
    X=DefNodeSolveMode();
  else
    X=DefLinkSolveMode();
  if ((X & m_AllowedModes)==0 && (m_AllowedModes &SM_All)!=0)
    {
    X = m_FallBackModes & SM_All;
    return GlblMode2LPTSTR(X, SM_All, eGM_FallBack);
    }
  return GlblMode2LPTSTR(X, SM_All, eGM_Default);
  };

long TaggedObject::HeatMethod()
  {
  long X;
  if (*m_pRunMds & HM_All)
    X=*m_pRunMds & HM_All;
  else
    X=DefHeatMode();
  if ((X & m_AllowedModes)==0 && (m_AllowedModes&HM_All)!=0)
    X = m_FallBackModes & HM_All;

  if (dbgGetModes) 
    dbgpln("HeatMethod  = %-20s %-20s %s", GlblMode2LPTSTR(X, SM_All, eGM_Name), GlblMode2LPTSTR(X&m_AllowedModes, SM_All, eGM_Name), Tag());
  return X;
  };

LPTSTR TaggedObject::HeatMethodStr()
  {
  if (*m_pRunMds & HM_All)
    return GlblMode2LPTSTR(*m_pRunMds, HM_All, eGM_Name);
  long X=DefHeatMode();
  if ((X & m_AllowedModes)==0 && (m_AllowedModes&HM_All)!=0)
    {
    X = m_FallBackModes & HM_All;
    return GlblMode2LPTSTR(X, HM_All, eGM_FallBack);
    }
  return GlblMode2LPTSTR(X, HM_All, eGM_Default);
  };

void TaggedObject::SetFlowModeLcl(LPCTSTR Mode)
  {
  SetFlowModeLcl(String2GlblMode(Mode, LFM_All)); 
  };

//void TaggedObject::SetxFlowModeReqd(long FlowMode)
//  {
//  m_FlowModeRqd=FlowMode;
//  }

void TaggedObject::SetFlowModeRqd(long FlowMode, long FlowFlags, TaggedObject * pRuleObj)
  {
  m_FlowModeRqd = (FlowMode ? (FlowMode & LFM_All) | FlowFlags : 0);
  //*m_pRunMds = (*m_pRunMds & ~LFM_All) | (FlowMode & LFM_All & ~LFM_Propagate);
  m_FlowModeObj=pRuleObj;
  StructureChanged(this);
  if (dbgSetModes) 
    dbgpln("SetFlowModeRqd %08x", (m_FlowModeRqd & LFM_AllPg)); 
  };

void TaggedObject::SetFlowModeRqd(LPCTSTR FlowMode, long FlowFlags, TaggedObject * pRuleObj)
  {
  SetFlowModeRqd(String2GlblMode(FlowMode, LFM_All), FlowFlags, pRuleObj); 
  };


long TaggedObject::FlowModeRqd()
  {
  return m_FlowModeRqd;
  };

LPTSTR TaggedObject::FlowModeRqdStr()
  {
  if (m_FlowModeRqd)
    return GlblMode2LPTSTR(m_FlowModeRqd, LFM_All, 
      (m_FlowModeRqd & LFM_Propagate) ? eGM_Propagate : (m_FlowModeRqd & LFM_Propagated) ? eGM_Propagated:eGM_Name);
  return "";
  };

long TaggedObject::FlowMode()               
  { 
  //return m_lRunMds & LFM_All; 
  if (*m_pRunMds & LFM_All)
    return (*m_pRunMds & LFM_All);
  if (m_FlowModeRqd & LFM_All)
    return (m_FlowModeRqd & LFM_All);
  return DefFlowMode();
  };

long TaggedObject::FlowModePg()               
  { 
  //return m_lRunMds & LFM_All; 
  if (*m_pRunMds & LFM_AllPg)
    return (*m_pRunMds & LFM_AllPg);
  if (m_FlowModeRqd & LFM_AllPg)
    return (m_FlowModeRqd & LFM_AllPg);
  return DefFlowMode();
  };

void TaggedObject::Revert2Default()
  {
  if (gs_Revert2DefaultRqd)
    {
    if ((*m_pRunMds & LFM_All) == DefFlowMode())
      *m_pRunMds &= ~LFM_All;
    }
  };

LPTSTR TaggedObject::FlowModeStr()             
  { 
  if (gs_Revert2DefaultRqd)
    {
    if ((*m_pRunMds & LFM_All) == DefFlowMode())
      return GlblMode2LPTSTR(DefFlowMode(), LFM_All, eGM_Default);
    }
  if (*m_pRunMds & LFM_All)
    return GlblMode2LPTSTR(*m_pRunMds, LFM_All, eGM_Name);
  if (m_FlowModeRqd & LFM_All)
    return GlblMode2LPTSTR(m_FlowModeRqd, LFM_All, eGM_Remote/*eGM_Propagate*/);
  return GlblMode2LPTSTR(DefFlowMode(), LFM_All, eGM_Default);
  };

TaggedObject *  TaggedObject::FlowModeObj()    { return m_FlowModeObj; };

struct tOldGlblMdName { long Md; LPTSTR Nm[3]; bool Hidden; };
static const tOldGlblMdName OldGlblModeNames[] =
  { //              String               Default
    {SM_Direct,    {"ProBal"           , "<ProBal>"           , "" }, false},
    {SM_Inline,    {"Dynamic-Transfer" , "<Dynamic-Transfer>" , "" }, true},
    {SM_Inline,    {"Dynamic-Flow"     , "<Dynamic-Flow>"     , "" }, false},
    {SM_Buffered,  {"Dynamic-Full"     , "<Dynamic-Full>"     , "" }, false},
    {HM_None,      {"None"             , "<None>"             , "" }, false},
    {HM_Reduced,   {"Simple"           , "<Simple>"           , "" }, false},
    {HM_Full,      {"Rigorous"         , "<Rigorous>"         , "" }, false},
    {0}
  };


struct tGlblMdName { long Md; LPTSTR m_Nm[6]; long MdPropagate, MdPropagated; };
static tGlblMdName ls_GlblModeNames[] =
  {
  //                eGM_Name,      eGM_Default,    eGM_Remote,      eGM_Propagate,   eGM_Propagated, eGM_FallBack };
    {NM_Probal,    {"ProBal"     , "<ProBal>"    , "?1?"          , "?1?"          , "?2?"         , "ProBal*"    }},
    {NM_Dynamic,   {"Dynamic"    , "<Dynamic>"   , "?1?"          , "?1?"          , "?2?"         , "Dynamic*"   }},
    {SM_Direct,    {"Direct"     , "<Direct>"    , "?1?"          , "?1?"          , "?2?"         , "Direct*"    }},
    {SM_Inline,    {"Inline"     , "<Inline>"    , "?1?"          , "?1?"          , "?2?"         , "Inline*"    }},
    {SM_Buffered,  {"Buffered"   , "<Buffered>"  , "?1?"          , "?1?"          , "?2?"         , "Buffered*"  }},
    {HM_None,      {"None"       , "<None>"      , "?1?"          , "?1?"          , "?2?"         , "None*"      }},
    {HM_Reduced,   {"Reduced"    , "<Reduced>"   , "?1?"          , "?1?"          , "?2?"         , "Reduced*"   }},
    {HM_Full,      {"Rigorous"   , "<Rigorous>"  , "?1?"          , "?1?"          , "?2?"         , "Rigorous*"  }},
    {LFM_Xfer,     {"Transfer"   , "<Transfer>"  , "[Transfer]"   , "Transfer-->"  , "-->Transfer" , "Transfer*"  }},
    {LFM_Simple,   {"Simple"     , "<Simple>"    , "[Simple]"     , "Simple-->"    , "-->Simple"   , "Simple*"    }},
    {LFM_Linear,   {"Linear"     , "<Linear>"    , "[Linear]"     , "Linear-->"    , "-->Linear"   , "Linear*"    }},
    {LFM_Full,     {"Full"       , "<Full>"      , "[Full]"       , "Full-->"      , "-->Full"     , "Full*"      }},
    {0}
  };

static class CGlblMode2NameMap : public CMap<long, long, const tGlblMdName*, const tGlblMdName*> 
  {
  public:
    CGlblMode2NameMap()
      {
      InitHashTable(37);
      for (int i=0; ls_GlblModeNames[i].Md; i++)
        {
        tGlblMdName & X=ls_GlblModeNames[i];
        X.MdPropagate=X.Md|LFM_Propagate;
        X.MdPropagated=X.Md|LFM_Propagated;
        SetAt(X.Md, &X);
        SetAt(X.MdPropagate, &X);
        SetAt(X.MdPropagated, &X);
        }
      }

  } ls_GlblMode2NameMap;

static class CGlblNamesModeMap : public CMap<LPTSTR, LPTSTR, long, long> 
  {
  public:
    CGlblNamesModeMap()
      {
      InitHashTable(37);
      for (int i=0; ls_GlblModeNames[i].Md; i++)
        {
        const tGlblMdName & X=ls_GlblModeNames[i];
        SetAt(X.m_Nm[eGM_Name], X.Md);
        SetAt(X.m_Nm[eGM_Propagate], X.MdPropagate);
        SetAt(X.m_Nm[eGM_Propagated], X.MdPropagated);
        }
      }

  } ls_GlblNamesModeMap;

Strng GlblMode2String(long Md, long MdMsk)
  {
  Strng MdStr;
  for (int i=0; ls_GlblModeNames[i].Md; i++)
    {
    //ASSERT_ALWAYS(ls_GlblModeNames[i].Md==m, "Bad Mode Order", __FILE__, __LINE__);
    if (ls_GlblModeNames[i].Md & Md & MdMsk)
      {
      if ((MdStr.Length()>0))// && !ls_GlblModeNames[i].Hidden)
        MdStr+="-";
      MdStr+=ls_GlblModeNames[i].m_Nm[eGM_Name];
      }
    }
  return MdStr;
  };

LPTSTR GlblMode2LPTSTR(long Md, long MdMsk, eGlblModeWhich Which)
  {
  const tGlblMdName* p;
  if (ls_GlblMode2NameMap.Lookup(Md & MdMsk, p))
    return p->m_Nm[Which];

  //for (int i=0; ls_GlblModeNames[i].Md; i++)
  //  {
  //  if (ls_GlblModeNames[i].Md & Md & MdMsk)
  //    return ls_GlblModeNames[i].Nm[Which];//:ls_GlblModeNames[i].Nm;
  //  }
  return "";
  };

long  String2GlblMode(LPCTSTR MdStr, long MdMsk)
  {
  //const tGlblMdName* p;
  long Md;
  if (ls_GlblNamesModeMap.Lookup((LPTSTR)MdStr, Md))//p))
    return Md;
    //return p->Md;

  //for (int i=0; ls_GlblModeNames[i].Md; i++)
  //  {
  //  if ((MdMsk&ls_GlblModeNames[i].Md)==0)
  //    continue;
  //  //ASSERT_ALWAYS(ls_GlblModeNames[i].Md==m, "Bad Mode Order", __FILE__, __LINE__);
  //  //Strng S(ls_GlblModeNames[i].Nm[0]);
  //  //S.Lower();
  //  //if (strstr(MS(), S()))
  //  if (stricmp(MdStr, ls_GlblModeNames[i].Nm[0])==0)
  //    return ls_GlblModeNames[i].Md;
  //  if (stricmp(MdStr, ls_GlblModeNames[i].Nm[1])==0) // Default
  //    return 0;
  //  }
  return 0;
  };


void GetGlblModeValueLst(DDBValueLstMem & Lst, long MdMask, long DefMd, bool PropMds)
  {
  Lst.Empty();
  if (DefMd)
    {
    for (int i=0; ls_GlblModeNames[i].Md; i++)
      {
      if ((ls_GlblModeNames[i].Md & DefMd))// && !ls_GlblModeNames[i].Hidden)
        {
        Lst.Add(0, ls_GlblModeNames[i].m_Nm[eGM_Default], MDD_Default);
        Lst.Add(0, "------------");
        break;
        }
      }
    }

  for (int i=0; ls_GlblModeNames[i].Md; i++)
    {
    if ((ls_GlblModeNames[i].Md & MdMask))// && !ls_GlblModeNames[i].Hidden)
      Lst.Add(ls_GlblModeNames[i].Md, ls_GlblModeNames[i].m_Nm[eGM_Name]);
    }

  if (PropMds)
    {
    Lst.Add(0, "------------");
    for (int i=0; ls_GlblModeNames[i].Md; i++)
      {
      if ((ls_GlblModeNames[i].Md & MdMask))// && !ls_GlblModeNames[i].Hidden)
        Lst.Add(ls_GlblModeNames[i].Md|LFM_Propagate, ls_GlblModeNames[i].m_Nm[eGM_Remote], MDD_NoSelect);
      }
    }
  };

//--------------------------------------------------------------------------

void TaggedObject::SetAllowedModes(bool NodeSolve, long AllowedModes)//LPCTSTR PBModeNm, LPCTSTR XferModeNm, LPCTSTR FullModeNm)
  {
  m_bNodeSolveMode=NodeSolve;
  m_AllowedModes=AllowedModes;

  long LicModes=0;
  if (gs_License.ProBalOK())
    LicModes|=NM_Probal ;
  if (gs_License.DynamicOK())
    LicModes|=NM_Dynamic;
  if (gs_License.DemoMode())
    LicModes|=NM_Probal|NM_Dynamic;

  LicModes |= SM_All;
  LicModes |= HM_All;

  if (gs_License.ProBalOK())
    LicModes|=LFM_Xfer;
  if (gs_License.DynamicOK())
    LicModes|=LFM_Xfer|LFM_Simple;
  if (gs_License.AllowDynamicFull())
    LicModes|=LFM_Linear|LFM_Full;
  if (gs_License.DemoMode())
    LicModes|=LFM_Xfer|LFM_Simple|LFM_Linear|LFM_Full;

  m_AllowedModes &= LicModes; 
  if (m_bNodeSolveMode)
    m_AllowedModes &= gs_AllowedNodeModes;
  else
    m_AllowedModes &= gs_AllowedLinkModes;

  m_FallBackModes=0;
  m_FallBackModes|=Modes2Max(AllowedModes&LicModes, SM_All);
  m_FallBackModes|=Modes2Max(AllowedModes&LicModes, HM_All);

  if (/*((m_AllowedModes & NM_All)==0) ||*/
      ((m_AllowedModes & SM_All)==0) ||
      ((m_AllowedModes & HM_All)==0)/* ||
      ((m_AllowedModes & LFM_All)==0)*/)
    {
    __debugbreak();
    }


  if (dbgAllowedModes)
    dbgpln("AllowedMds  %08x %-20s %s", m_AllowedModes, ClassId(), Tag());
  };

// -------------------------------------------------------------------------

void TaggedObject::ValidateModes()
  {
  // Can ths be improved ?????

  };

// -------------------------------------------------------------------------

long GetPermissableModes(long MdMask, TaggedObject * pObject)
  {
  long Ret = (pObject?pObject->m_AllowedModes & MdMask : MdMask);  

  if (MdMask & LFM_All) // Any Flow Mode
    {
    long FlwModes;
    if (pObject)
      {
      if (pObject->NetProbalMethod())
        FlwModes=LFM_Xfer;
      else
        FlwModes=LFM_All;
      }
    else
      FlwModes=LFM_All;
    Ret=(Ret & ~LFM_All) | (Ret & LFM_All & FlwModes);
    }
  if (pObject)
    return Ret & MdMask & pObject->m_AllowedModes;
  return Ret & MdMask;
  };

// -------------------------------------------------------------------------

flag TaggedObject::GetActive()             { return m_fActive; };
void TaggedObject::SetActive(flag Active)  { m_fActive=Active; };

flag TaggedObject::GetActiveHold()
  { return (m_fActiveHold || fActiveHold_Auto || (m_fActiveHoldIfSS && gs_License.OnlySteadyState())) && GetActiveHoldOK();};
flag TaggedObject::GetActiveHoldIsAuto()   { return GetActiveHold() && !m_fActiveHold; };
flag TaggedObject::GetActiveHoldOK()       { return NetDynamicMethod() ? fActiveHoldOK_Dyn:fActiveHoldOK_PB;};
void TaggedObject::SetActiveHold(flag Hold) { m_fActiveHold = Hold && GetActiveHoldOK(); };
void TaggedObject::SetActiveHoldAuto(flag Hold) { fActiveHold_Auto = Hold && GetActiveHoldOK(); };

// -------------------------------------------------------------------------

void TaggedObject::MarkAsCOMOwned(void *gpTO)
  {
  m_pgpTO=static_cast<GIScdTaggedObject*>(gpTO);
  };
void TaggedObject::DestroyCOMOwned()
  {
  m_pgpTO->Unglobalize();
  };

// -------------------------------------------------------------------------

void TaggedObject::AttachMeToObj(TaggedObject *pObj, TagObjAttachment eAttach)
  {
  if (pAttachedTo)
    DetachMeFromOwner();

  StructureChanged(this);
  pAttachedTo=pObj;
  eAttachment=eAttach;
  if (pObj)
    {
    pNxtAttachment=pObj->pAttachments;
    pObj->pAttachments=this;
    }
  else
    pNxtAttachment=NULL;

  // m_pSolve Always points to its own m_lRunMds or the root object's m_lRunMds
  TaggedObject *p=this;
  while (p->pAttachedTo)
    p=p->pAttachedTo;
  m_pRunMds=&p->m_lRunMds;

  SetFullObjTag();
  #if dbgTagObjAttach
  dbgpln("  Attach  %8x %8x %s %s",this, pObj,m_pClass->ClassId(), sTag());
  #endif
  };

// -------------------------------------------------------------------------

void TaggedObject::DetachMeFromOwner()
  {
  #if dbgTagObjAttach
  dbgpln("  Detach  %8x %8x %s %s",this,pAttachedTo,m_pClass->ClassId(), sTag());
  #endif
  StructureChanged(this);
  if (pAttachedTo)
    {
    if (pAttachedTo->pAttachments==this)
      pAttachedTo->pAttachments=pNxtAttachment;
    else
      {
      TaggedObject *p=pAttachedTo->pAttachments;
      while (p && (p->pNxtAttachment!=this))
        p=p->pNxtAttachment;
      ASSERT_ALWAYS(p!=NULL, "Bad Ptr DetachMeFromOwner", __FILE__, __LINE__);
      p->pNxtAttachment=pNxtAttachment;
      }
    }
  pAttachedTo=NULL;
  pNxtAttachment=NULL;
  m_pRunMds=&m_lRunMds;
  SetFullObjTag();
  };

// -------------------------------------------------------------------------

LPSTR TaggedObject::Tag()
  {
  return m_sTag.Length()==0 ? "" : m_sTag();
  }

// -------------------------------------------------------------------------

LPSTR TaggedObject::SetTag(LPSTR ReqdTag, bool AllowEmptyTag)
  {
  ASSERT(pAttachedTo!=NULL || eAttachment>TOA_Unknown);
  if (ReqdTag || AllowEmptyTag)
    {
    if (ReqdTag==NULL)
      ReqdTag="";

    char buff[512];
    strcpy(buff, ReqdTag);
    LPSTR p = strchr(buff, ' ');
    if (p)
      *p=0;
    if (m_sTag.XStrCmp(buff)!=0)
      {
      m_sTag=buff;
      StructureChanged(this);
      }
    SetFullObjTag();
    }
  return m_sTag.Length()==0 ? "" : m_sTag();
  }

// -------------------------------------------------------------------------

LPSTR TaggedObject::Prefix(LPSTR ReqdPrefix)
  {
  ASSERT(pAttachedTo!=NULL || eAttachment>TOA_Unknown);
  if (ReqdPrefix)
    {
    char buff[512];
    strcpy(buff, ReqdPrefix);
    LPSTR p = strchr(buff, ' ');
    if (p)
      *p=0;
    if (m_sPrefix.XStrCmp(buff)!=0)
      {
      m_sPrefix=buff;
//      wObjDataVerNo++;
      StructureChanged(this);
      }
    SetFullObjTag();
    }
  return m_sPrefix.Length()==0 ? "" : m_sPrefix();
  }

// -------------------------------------------------------------------------

Strng TaggedObject::PrefixTag()
  {
  Strng S;
  if (m_sPrefix.GetLength()>0)
    {
    S=m_sPrefix;
    S+=".";
    }
  S+=Tag();
  return S;
  }
//---------------------------------------------------------------------------

int TaggedObject::TagCmp(LPSTR pTag, LPSTR pSrchTag, int MinTagLen)
  {
  if (pSrchTag==NULL || pTag==NULL)
    return 0;

  const int lt=strlen(pTag);
  const int le=_strnicmp(pTag, pSrchTag, lt); // will fail if ls<lt
  if (le==0) // will fail if ls<lt
    {
    const int ls=strlen(pSrchTag);
    if (ls==lt)
      return lt;
    else if (lt>=MinTagLen && pSrchTag[lt]=='.') // ls > lt
      return lt+1;
    //else if (pSrchTag[lt]=='[') // ls > lt
    //  return lt;
    }
  return 0;
  }

//---------------------------------------------------------------------------

int TaggedObject::TagCmpFn(LPSTR pTag, LPSTR pSrchTag, int MinTagLen)
  {
  if (pSrchTag==NULL || pTag==NULL)
    return 0;

  const int lt=strlen(pTag);
  const int le=_strnicmp(pTag, pSrchTag, lt); // will fail if ls<lt
  if (le==0) // will fail if ls<lt
    {
    const int ls=strlen(pSrchTag);
//    if (ls==lt)
//      return lt;
    if (lt>=MinTagLen && pSrchTag[lt]=='(') // ls > lt
      return lt;
    //else if (pSrchTag[lt]=='[') // ls > lt
    //  return lt;
    }
  return 0;
  }

//---------------------------------------------------------------------------

TaggedObject *TaggedObject::FindObjTag(LPSTR pSrchTag, flag SrchAll, int &ObjTagLen, int MinTagLen)
  {
  return NULL;
  }

//----------------------------------------------------------------------------

//static CCriticalSection FTOSect;
//Strng TaggedObject::FullObjTagTmp[MaxFullObjTagTmp];
//int   TaggedObject::FullObjTagTmpNo=0;

//LPSTR TaggedObject::FullObjTag(byte What, flag SelfCall)
//  {
//  CSingleLock Lk(&FTOSect, TRUE);
//  ASSERT(pAttachedTo!=NULL || eAttachment>TOA_Unknown);
//  if (!SelfCall)
//    {
//    FullObjTagTmpNo=(FullObjTagTmpNo+1) % MaxFullObjTagTmp;
//    FullObjTagTmp[FullObjTagTmpNo]="";
//    }
//  if (pAttachedTo)
//    pAttachedTo->FullObjTag(What, True);
//  if ((What & eAttachment) && ((eAttachment & TOA_Hidden)==0))
//    {
//    if(FullObjTagTmp[FullObjTagTmpNo].Length()>0)
//      FullObjTagTmp[FullObjTagTmpNo]+=".";
//    FullObjTagTmp[FullObjTagTmpNo]+=Tag();
//    }
//  return FullObjTagTmp[FullObjTagTmpNo]();
//  }

void TaggedObject::SetFullObjTag()
  {
  if (pAttachedTo)
    pAttachedTo->FullObjTag(m_sFullObjTag, TOA_All);
  else
    m_sFullObjTag= "";
  if ((TOA_All & eAttachment) && ((eAttachment & TOA_Hidden)==0))
    {
    if(m_sFullObjTag.Length()>0)
      m_sFullObjTag+=".";
    if(m_sPrefix.Length()>0)
      {
      m_sFullObjTag+=m_sPrefix;
      m_sFullObjTag+=".";
      }
    m_sFullObjTag+=Tag();
    }

  TaggedObject *p=pAttachments;
  while (p)
    {
    p->SetFullObjTag();
    p=p->pNxtAttachment;
    }
  }

char * TaggedObject::FullObjTag()
  {
  return m_sFullObjTag();
  }

char * TaggedObject::FullObjTag(Strng &S,  byte What)
  {
  if (pAttachedTo)
    pAttachedTo->FullObjTag(S, TOA_All);
  else
    S= "";
  if ((What & eAttachment) && ((eAttachment & TOA_Hidden)==0))
    {
    if(S.Length()>0)
      S+=".";
    S+=Tag();
    }
  return S();
  }

//---------------------------------------------------------------------------

CString TaggedObject::CreateGuidStr()
  {
  CString Guid;
  // Generate DefaultGuid
  UCHAR*psEqGUID=NULL;
  UUID AGuid;
  ASSERT_ALWAYS(UuidCreate(&AGuid)==RPC_S_OK, "Guid Not Created", __FILE__, __LINE__);
  ASSERT_ALWAYS(UuidToString(&AGuid, &psEqGUID)==RPC_S_OK, "Guid Not Converted", __FILE__, __LINE__);
#if (SYSCAD10)  
  Guid=psEqGUID;     /* Why does this not work */
#else                
  Guid.Format("{%s}", psEqGUID);
#endif

  RpcStringFree(&psEqGUID);
  
  return Guid;
  }

//---------------------------------------------------------------------------

LPSTR TaggedObject::SetGuid(LPSTR strGuid)
  {
  if (strGuid==NULL || strlen(strGuid)==0)
    return m_sGuid();

  UUID LclGuid;
  if (strGuid[0]=='{')
    {
    char Buff[128];
    strcpy(Buff, &strGuid[1]);
    Buff[strlen(Buff)-1]=0;
    if (UuidFromString((UCHAR*)&Buff, &LclGuid)==RPC_S_OK)
      {
#if (SYSCAD10)   
      m_sGuid=Buff;
#else
      m_sGuid=strGuid;
#endif
      return m_sGuid();
      }
    }
  else if (UuidFromString((UCHAR*)strGuid, &LclGuid)==RPC_S_OK)
    {
#if (SYSCAD10)  
    m_sGuid.Set("%s", strGuid);
#else
    m_sGuid.Set("{%s}", strGuid);
#endif
    return m_sGuid();
    }

  LogWarning(Tag(), 0, "Bad GUID:%s", strGuid);
  return m_sGuid();
  };

//---------------------------------------------------------------------------

//LPSTR TaggedObject::Guid()
//  {
//  return m_sGuid();
//  };

//---------------------------------------------------------------------------

void TaggedObject::BuildCommonStartDefn(DataDefnBlk &DDB)
  {
  DDB.String      ("Tag",         "", DC_,   "", xidTag,                this, 0|DDEF_WRITEPROTECT);
  DDB.String      ("Parent",      "", DC_,   "", xidParent,             this, 0|DDEF_WRITEPROTECT|InitHidden);
  DDB.String      ("Type",        "", DC_,   "", &m_pClass->sClassName, this, 0|DDEF_WRITEPROTECT);
  DDB.String      ("ClassId",     "", DC_,   "", &m_pClass->sClassId,   this, 0|DDEF_WRITEPROTECT|InitHidden);
  DDB.String      ("PrimaryCfg",  "", DC_,   "", xidPrimaryCfg,         this, 0|InitHidden);
  DDB.String      ("EqpDesc",     "", DC_,   "", xidEqDesc,             this, isParm);
  DDB.String      ("EqpMemo",     "", DC_,   "", xidEqMemo,             this, isParm|InitHidden);
  DDB.String      ("EqpIdStr",    "", DC_,   "", xidEqIdStr,            this, isParm|InitHidden);
  DDB.String      ("EqpLocation", "", DC_,   "", xidEqLocation,         this, isParm|InitHidden);
  DDB.String      ("EqpGUID",     "", DC_,   "", xidEqGUID,             this, isParmConstruct|InitHidden);
  DDB.Long        ("EqpLink",     "", DC_,   "", xidEqLink,             this, isParmConstruct|InitHidden);
  DDB.Long        ("ReportKey",   "", DC_,   "", xidEqRepKey,           this, isParm|InitHidden);
  if (dbgtestfileopen())
    DDB.CheckBoxBtn ("DbgBrk",    "", DC_,   "", &fDoDbgBrk,            this, isParm);
  DDB.CheckBoxBtn ("SkipSnapLoad","", DC_,   "", &bSkipSnapLoad,        this, isParm|InitHidden|noFileAtAll);
  //if (DDB.ForFiling())
  //  {
  //  DDB.DWord     ("AssocGrfShow", "", DC_,   "", &m_AssocGrfShow,  this, isParm|SetOnChange);
  //  DDB.DWord     ("AssocGrfHide", "", DC_,   "", &m_AssocGrfHide,  this, isParm|SetOnChange);
  //  }
  }

//---------------------------------------------------------------------------

flag TaggedObject::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidTag       : 
      if (DCB.rpC)
        SetTag(DCB.rpC); 
      DCB.pC=Tag(); 
      return 1;
    case xidParent    : DCB.pC=TagOfParent(); return 1;
    case xidEqDesc    : if (DCB.rpC) m_sEqDesc=DCB.rpC; DCB.pC=m_sEqDesc(); return 1;
    case xidEqMemo    : if (DCB.rpC) m_sEqMemo=DCB.rpC; DCB.pC=m_sEqMemo(); return 1;
    case xidEqIdStr   : if (DCB.rpC) m_sEqIdStr=DCB.rpC; DCB.pC=m_sEqIdStr(); return 1;
    case xidEqLocation: if (DCB.rpC) m_sEqLocation=DCB.rpC; DCB.pC=m_sEqLocation(); return 1;
    case xidEqLink    : if (DCB.rL)  m_lEqLink=*DCB.rL; DCB.L=m_lEqLink; return 1;
    case xidEqRepKey  : if (DCB.rL)  m_lReportKey=*DCB.rL; DCB.L=m_lReportKey; return 1;
    case xidEqGUID    : if (DCB.rpC) SetGuid(DCB.rpC); DCB.pC=(LPSTR)Guid(); return 1;
    case xidPrimaryCfg: if (DCB.rpC) PutPrimaryCfg(DCB.rpC); DCB.pC=(LPSTR)GetPrimaryCfg(); return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag TaggedObject::ValidateData(ValidateDataBlk & VDB)
  {
  ValidateModes();
  // return True if no data error
  return True;
  };

//---------------------------------------------------------------------------

void TaggedObject::StructureChanged(TaggedObject *pObj)
  {
  //#if dbgStructureChanged
  //dbgpln("StructureChanged %s", pObj ? pObj->FullObjTag() : "NULL");
  //#endif
  if (pObj)
    {
    #if dbgStructureChanged
    int xx=0;
    #endif
    while (pObj)
      {
      #if dbgStructureChanged
      dbgpln("%s %8u %s", xx++==0 ? "StructureChanged":"             ...", pObj->dwObjDataVerNo, pObj->FullObjTag());
      #endif
      pObj->dwObjDataVerNo=++pObj->m_pClass->dwObjDataVerNo;
      pObj=pObj->pAttachedTo;
      }
    }
  else
    {
    #if dbgStructureChanged
    dbgpln("StructureChanged %8u NULL", dwGlblDataVerNo);
    #endif
    dwGlblDataVerNo++;
    }
  };

//---------------------------------------------------------------------------

void TaggedObject::SetGlblTopologyChg(GlblTopologyChgFun *ChgFun, dword ChgData)
  {
  GlblTopoChg.Fun=ChgFun;
  GlblTopoChg.Data=ChgData;
  };

//---------------------------------------------------------------------------

void TaggedObject::MyTagsHaveChanged(bool FirstOne)
  {
  if (pAttachedTo)
    pAttachedTo->MyTagsHaveChanged(false);

  if (FirstOne)
    StructureChanged(this);
  }

//---------------------------------------------------------------------------

flag TaggedObject::FndDefinedData(char * FullItemTag, TABOptions Opts, pDDBShortcut pShortcut)
  {
  DataDefnBlk DDB(NULL);//, NULL);

  DDB.Open(this, FullItemTag, DDBTask_FndData, Opts, pShortcut);

  DDB.BeginBlock(tt_Object, this, Tag(), ClassId(), SubClassId(), GetPrimaryCfg(), TagOfParent(), NULL, 0, 0, 0, 0x0);
  DDB.Page(Tag(),DDB_RqdPage);
  BuildCommonStartDefn(DDB);
  DDB.SuppressNextBlock();
  BuildDataDefn(DDB);
  BuildCommonEndDefn(DDB);

  if (bDoSystemDefn && (!DDB.ForView() ||
#if dbgTagObj
    dbgViewSysDefn()
#else
    false
#endif
    /*dbgtestfileopen()*/))
    {
    if (PrjFileVerNo()<22)
      DDB.BeginStruct(this, "Sys", NULL, DDB_RqdPage, -1, MarkerClosed);
    else
      DDB.Page("Sys", DDB_RqdPage);
    BuildSystemDefn(DDB);
    if (PrjFileVerNo()<22)
      DDB.EndStruct();
    }
  DDB.EndBlock();

  DDB.Close();

  return pShortcut ? pShortcut->fOK : True;
  }

//---------------------------------------------------------------------------

flag TaggedObject::GetDefinedData(char * FullItemTag, CPkDataList &DataObj, TABOptions Opts, /*CPkDataItem* &pPItem,*/ pDDBShortcut pShortcut)
  {
  TaggedObject *p=pAttachments;
  while (p)
    {
    p->PrepareDefinedData();
    p=p->pNxtAttachment;
    }

  PrepareDefinedData();

  if (pShortcut && !(Opts& TABOpt_AllInfoOnce))
    if (IsData(pShortcut->iType))
      if (pShortcut->TestShortcut(FullItemTag, CurrentDataVersion()))
        {
        //pPItem=DataObj.FirstItem();
        pShortcut->GetDefinedData(Opts, DataObj);//.GetInfo(), pPItem);
        //DataObj.AdvanceWrite(/*pPItem*/);
        return True;
        }

  DataDefnBlk DDB(&DataObj);//, &pPItem);

  DDB.Open(this, FullItemTag, DDBTask_GetData, Opts, pShortcut);

  DDB.BeginBlock(tt_Object, this, Tag(), ClassId(), SubClassId(), GetPrimaryCfg(), TagOfParent(), NULL, 0, 0, 0, 0x0);
  DDB.Page(Tag(),DDB_RqdPage);
  BuildCommonStartDefn(DDB);
  DDB.SuppressNextBlock();
  BuildDataDefn(DDB);
  BuildCommonEndDefn(DDB);
  if (bDoSystemDefn && (!DDB.ForView() ||
#if dbgTagObj
    dbgViewSysDefn()
#else
    false
#endif
    /*dbgtestfileopen()*/))
    {
    if (PrjFileVerNo()<22)
      DDB.BeginStruct(this, "Sys", NULL, DDB_RqdPage, -1, MarkerClosed);
    else
      DDB.Page("Sys", DDB_RqdPage);
    BuildSystemDefn(DDB);
    if (PrjFileVerNo()<22)
      DDB.EndStruct();
    }
  DDB.EndBlock();

  DDB.Close();

  return True;
  }

//--------------------------------------------------------------------------

flag TaggedObject::PutDefinedData(char * FullItemTag, CPkDataList &DataObj, CPkDataIter &Iter, TABOptions Opts, /*CPkDataItem* &pPItem,*/ pDDBShortcut pShortcut)
  {
  //if (pShortcut && ((pShortcut->iFlags & NOPutShortcut)==0))
  //  if (IsData(pShortcut->iType))
  //    if (pShortcut->TestShortcut(FullItemTag, CurrentDataVersion()))
  //      {
  //      pPItem=DataObj.FirstItem();
  //      pShortcut->PutDefinedData(/*DataObj,*/ Opts, pPItem);
  //      return True;
  //      }
  DataDefnBlk DDB(&DataObj);//, &pPItem);
  DDB.Open(this, FullItemTag, DDBTask_PutData, Opts, pShortcut);
  if (DDB.Analyse(Iter))
    {
    m_bFileIOBusy=(Opts & (TABOpt_ForFile|TABOpt_ForScenario|TABOpt_ForSnapShot))!=0;
    
    DDB.BeginBlock(tt_Object, this, Tag(), ClassId(), SubClassId(), GetPrimaryCfg(), TagOfParent(), NULL, 0, 0, 0, 0x0);
    DDB.Page(Tag(),DDB_RqdPage);
    BuildCommonStartDefn(DDB);
    DDB.SuppressNextBlock();
    BuildDataDefn(DDB);
    BuildCommonEndDefn(DDB);
    if (bDoSystemDefn && (!DDB.ForView() ||
#if dbgTagObj
      dbgViewSysDefn()
#else
      false
#endif
    /*dbgtestfileopen()*/))
      {
      if (PrjFileVerNo()<22)
        DDB.BeginStruct(this, "Sys", NULL, DDB_RqdPage, -1, MarkerClosed);
      else
        DDB.Page("Sys", DDB_RqdPage);
      BuildSystemDefn(DDB);
      if (PrjFileVerNo()<22)
        DDB.EndStruct();
      }
    DDB.EndBlock();

    DDB.Close();

//#if WITHEQUIPSPECS
//    if (/*(TABOPT_SelectStatus&Opts) &&*/ SelectData())
//      {
//      if (DataObj.GetCount()==1)
//        {
//        CPkDataItem *p=DataObj.FirstItem();
//        if (stricmp(p->TagStr(), FullItemTag)==0)
//          {
//          if (TABOPT_ValueStatusSet&Opts)
//            SelectData()->SetTagStatus(FullItemTag, CTOSelectData::VS_Set);
//          else if (SelectData()->GetTagStatus(FullItemTag)==CTOSelectData::VS_Set)
//            SelectData()->SetTagStatus(FullItemTag, CTOSelectData::VS_Reset);
//          }
//        }
//      }
//#endif
    
    m_bFileIOBusy=false;
    return True;
    }
  DDB.Close();

  return False;
  };

//---------------------------------------------------------------------------

double TaggedObject::ValidateRange(ValidateDataBlk & VDB, LPCTSTR What, double MinV, double V, double MaxV, flag *pOK)
  {
  if (!Valid(V))
    return V;

  double VR;
  if (Valid(MinV) && Valid(MaxV))
    VR=Range(MinV, V, MaxV);
  else if (Valid(MinV))
    VR=Max(MinV, V);
  else if (Valid(MaxV))
    VR=Min(MaxV, V);
  else
    VR=V;

  if (VR!=V)
    {
    if (pOK)
      {
      if (VDB.fValidateDataMessagesOn)
        {
        if (Valid(MinV) && Valid(MaxV))
          LogWarning(FullObjTag(), 0, "%s out of Range %5g > %5g", What, MinV, MaxV);
        else if (Valid(MinV))
          LogWarning(FullObjTag(), 0, "%s Less than %5g", What, MinV);
        else if (Valid(MaxV))
          LogWarning(FullObjTag(), 0, "%s Greater that %5g", What, MaxV);
        }
      *pOK=false;
      }
    else
      {
      //if (fValidateDataMessagesOn)
      LogNote(FullObjTag(), 0, "%s Changed %5g > %5g", What, V, VR);
      }
    }
  return VR;
  };

//---------------------------------------------------------------------------

flag TaggedObject::ValidateDefinedData(ValidateDataBlk & VDB)
  {
  if (bHoldValidateData)
    {
    if (m_List2Validate.Find(this)==NULL)
      m_List2Validate.AddTail(this);
    return 1;
    }

//  if (DoSetValidateDataRqd)
//    SetValidateDataRqd(true);
  if (fDoDbgBrk)
    { int xxx=0; }

  flag b=1;
  TaggedObject *p=pAttachments;
  while (p)
    {
    if (p->ValidateDefinedData(VDB))
      p->bUnValidatedData=False;
    else
      b=0;
    p=p->pNxtAttachment;
    }
  if (ValidateData(VDB))
    bUnValidatedData=False;
  else
    b=0;
  //fValidateDataRqd=false;
  return b;
  }

//---------------------------------------------------------------------------

int TaggedObject::SetHoldValidateData(flag On, flag MessagesOn)
  {
  flag WasOn=bHoldValidateData;
  if (On)
    {
    m_List2Validate.RemoveAll(); // In case
    bHoldValidateData = true;
    }
  else
    {
    bHoldValidateData = false; // In case
    while (!m_List2Validate.IsEmpty())
      {
      TaggedObject * p=m_List2Validate.RemoveHead();
      ValidateDataBlk VDB(MessagesOn);
      p->ValidateDefinedData(VDB);
      }
    }
  return WasOn;
  }

//---------------------------------------------------------------------------

void TaggedObject::ClearDDBLists()
  {
  }

//---------------------------------------------------------------------------

flag TaggedObject::DoDbgBrk()
  {
  if (fDoDbgBrk)
    return true;
  TaggedObject *p=pAttachedTo;
  while (p)
    {
    if (p->fDoDbgBrk)
      return true;
    else
      p=p->pAttachedTo;
    }
  return false;
  };

//---------------------------------------------------------------------------

void TaggedObject::SplitTagCnv(char *pTag, Strng & Tag, Strng& Cnv)
  {
  char Tmp[2048], *p=Tmp;
  if (pTag==NULL || strlen(pTag)==0)
    {
    Tag="";
    Cnv="";
    return;
    }
  strcpy(Tmp, pTag);
//dbgp("Split '%s'", pTag);
  Tag=StrToken(p);
  if (p && *p=='(')
    {
    Cnv=StrToken(p);
    int nParen=0;
    int iCS=-1, iCE=-1;
    for (int iChar=0; iChar<Cnv.Length() && iCE<0; iChar++)
      {
      switch (Cnv[iChar])
        {
        case '(': if (nParen==0 && iCS<0) iCS=iChar; nParen++; break;
        case ')': nParen--; if (nParen==0 && iCE<0) iCE=iChar; break;
        }
      }
    if (iCE>=0)
      Cnv=Cnv.Mid(iCS+1, iCE-iCS-1);
    }
  else
    Cnv=StrToken(p);
  }

//---------------------------------------------------------------------------

flag TaggedObject::CheckNonNumericTagChr(char c)
  {
  return ((c>32 && c<48 && c!='.' && c!=',' && c!='\'') ||
          (c>57 && c<127 && c!='='));
  }

//---------------------------------------------------------------------------

void TaggedObject::SetBadFieldNameChars(char* pBad, char * pRep)
  {
  if (pBad)
    {
    sBadFieldNameChars=pBad;
    sRepFieldNameChars=pRep;
    }
  else
    {
    sBadFieldNameChars="";
    sRepFieldNameChars=" ";
    }
  };

int TaggedObject::TestValidFieldName(char* pTag)
  {
  if (strpbrk(pTag, sBadFieldNameChars() ? sBadFieldNameChars() : ""))
    return 0;
  //if (strpbrk(pTag, sRepFieldNameChars()))
  //  return 0;
  return 1;
  };

flag TaggedObject::TestIfReservedTag(char* pTag)
  {
  if (pTag && pTag[0])
    {
    if (pTag[0]=='$')
      {
      for (int i=0; i<DollarReservedTagsCnt; i++)
        if (_stricmp(pTag, DollarReservedTags[i])==0)
          return 1;
      }
    else
      {
      for (int i=0; i<OtherReservedTagsCnt; i++)
        if (_stricmp(pTag, OtherReservedTags[i])==0)
          return 1;
      }
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag TaggedObject::CIStrng(int No, LPSTR & pS)
  {
  // NB check CBCount is large enough.
  int i=No-CBContext();
  switch (i)
    {
    case 1: pS="W\tRange Errors Exist";                  return 1;
    default:
      if (i>=FirstRngChkCI && i<FirstRngChkCI+MaxRngChksPerNode)
        {
        pS="?";
        return 1;
        }
    
      return ConditionBlk::CIStrng(No, pS);
    }
  };

//---------------------------------------------------------------------------

flag TaggedObject::GetAllCIs(Strng_List & L, int MaxNo)
  {
  ConditionBlk::GetAllCIs(L, MaxNo);
  TaggedObject *p=pAttachments;
  while (p)
    {
    p->GetAllCIs(L, MaxNo-L.Length());
    p=p->pNxtAttachment;
    }
  return 1;
  };

//---------------------------------------------------------------------------

void TaggedObject::ClrAllCIs()
  {
  ConditionBlk::ClrAllCIs();
  TaggedObject *p=pAttachments;
  while (p)
    {
    p->ClrAllCIs();
    p=p->pNxtAttachment;
    }
  };

//---------------------------------------------------------------------------

int TaggedObject::CountAllCIs(int MaxNo, dword RqdFlags)
  {
  int n=ConditionBlk::CountAllCIs(MaxNo, RqdFlags);
  TaggedObject *p=pAttachments;
  while (p && (n<MaxNo))
    {
    n+=p->CountAllCIs(MaxNo, RqdFlags);
    p=p->pNxtAttachment;
    }
  return n;
  };

//---------------------------------------------------------------------------

#if WITHEQUIPSPECS

void TaggedObject::SetCustomDataIndex(int i)
  {
  CTOCustomData::CDataSet * pDataSet;
  pDataSet = CustomDataSelected();

  if (pDataSet)
    {
    for (int f=0; f<pDataSet->FieldCount(); f++)
      {
      CTOCustomData::CDataSet::CField & Fld=pDataSet->Field(f);
      switch (Fld.m_iType)
        {
        case tt_Double:
          delete (double*)m_CustomData[f];
          break;
        case tt_Strng:
          delete (Strng*)m_CustomData[f];
          break;
        default:
          _asm int 3;
        }
      }
    }

  m_iCustomData=i-1; 
  pDataSet = CustomDataSelected();
  if (pDataSet)
    {
    m_CustomData.SetSize(pDataSet->FieldCount());

    for (int f=0; f<pDataSet->FieldCount(); f++)
      {
      CTOCustomData::CDataSet::CField & Fld=pDataSet->Field(f);
      switch (Fld.m_iType)
        {
        case tt_Double:
          m_CustomData[f] = (void*)(new double(Cnvs[Fld.m_iCnv]->Normal(SafeAtoF(Fld.m_sDefaultValue), Fld.m_sCnvText())));
          break;
        case tt_Strng:
          m_CustomData[f] = (void*)(new Strng(Fld.m_sDefaultValue));
          break;
        default:
          _asm int 3;
        }
      }
    }
  else
    m_CustomData.SetSize(0);
  };

int TaggedObject::GetCustomDataIndex()
  {
  return m_iCustomData+1; 
  };

CTOCustomData::CDataSet* TaggedObject::CustomDataSelected()
  {
  return m_iCustomData>=0 && m_pClass->m_CustomData.DataSetCount()>0 ? &m_pClass->m_CustomData.DataSet(m_iCustomData):NULL; 
  };

#endif

//==========================================================================
//
//
//
//==========================================================================

bool TaggedObject::AssocGrfOn(eAssocGrfTypes AGType, byte UseStatus)
  {
  eAssocGrfShows Shw=AssocGrfShow()[AGType];

  if (Shw==eAGS_AsPerClass)
    Shw=AssocGrfShowClass()[AGType];


  bool Ret=false;

  switch (Shw)
    {
    case eAGS_Hide:
      Ret=false;
      break;
    case eAGS_Show:
      Ret=true;
      break;
    case eAGS_ShowInUse:
      Ret=(UseStatus&CUseMsk_InUse)!=0;
      break;
    case eAGS_ShowEverUsed:
      Ret=(UseStatus&(CUseMsk_InUse|CUseMsk_WasUsed))!=0;
      break;
    }      

  //dbgpln("AssocGrfOn %2i %2i %04x %3s %s", AGType, Shw, UseStatus, Ret?"ON":"", Tag());
  return Ret;
  };

//========================================================================
//
//
//
//========================================================================

DDEF_Flags DDBDataInfo::GetValue(TABOptions Opts, PkDataUnion &DataValue)
  {
  DDEF_Flags RetFlags=0;
  switch (iType)
    {
    // NB Arranged in estimated order of frequency
    case tt_Double: RetFlags=GetDouble(Opts, DataValue); break;
    case tt_Bool  : RetFlags=GetBool  (Opts, DataValue); break;
    case tt_Byte  : RetFlags=GetByte  (Opts, DataValue); break;
    case tt_Char  : RetFlags=GetChar  (Opts, DataValue); break;
    case tt_Word  : RetFlags=GetWord  (Opts, DataValue); break;
    case tt_DWord : RetFlags=GetDWord (Opts, DataValue); break;
    case tt_Int   : RetFlags=GetInt   (Opts, DataValue); break;
    case tt_Short : RetFlags=GetShort (Opts, DataValue); break;
    case tt_Long  : RetFlags=GetLong  (Opts, DataValue); break;
    case tt_Float : RetFlags=GetFloat (Opts, DataValue); break;
    case tt_Strng : RetFlags=GetString(Opts, DataValue); break;
    default: DoBreak(); break;
    }
  return RetFlags;
  };

//---------------------------------------------------------------------------

void DDBDataInfo::PutValue(TABOptions Opts, PkDataUnion &Data)
  {
  switch (iType)
    {
    case tt_Double : PutDouble(Opts, Data);        break;
    case tt_Bool   : PutBool  (Opts, Data);        break;//, NULL);  break;
    case tt_Byte   : PutByte  (Opts, Data);        break;//, NULL);  break;
    case tt_Char   : PutChar  (Opts, Data);        break;//, NULL);  break;
    case tt_Word   : PutWord  (Opts, Data);        break;//, NULL);  break;
    case tt_DWord  : PutDWord (Opts, Data);        break;//, NULL);  break;
    case tt_Int    : PutInt   (Opts, Data);        break;//, NULL);  break;
    case tt_Short  : PutShort (Opts, Data);        break;//, NULL);  break;
    case tt_Long   : PutLong  (Opts, Data);        break;//, NULL);  break;
    case tt_Float  : PutFloat (Opts, Data);        break;
    case tt_Strng  : PutString(Opts, Data);        break;//, NULL);  break;
    default: DoBreak(); break;
    }
  };

//---------------------------------------------------------------------------

PkDataUnion * DDBDataInfo::FixGenericData(PkDataUnion &LclDataValue, PkDataUnion &DataValue, byte iType, CCnvIndex iCnv, DDBValueLst* pValueList)
  {
  if (DataValue.iType==tt_Generic)
    {
    flag CnvIsBad=0;
    flag Done=0;
    if (pValueList!=NULL && (IsIntData(iType) || IsUnSgnData(iType)))
      {
      bool GotDefVal=false;
      int iDefVal;
      char*pStr=DataValue.GetString();
      for ( ; pValueList->m_pStr; pValueList++)
        {
        if (pValueList->m_dwFlags & MDD_NoSelect)
          {
          }
        else if (_stricmp(pStr, pValueList->m_pStr)==0)
          {
          LclDataValue.SetTypeLong(iType, pValueList->m_lVal);
          Done=1;
          break;
          }
        else if (pValueList->m_dwFlags & MDD_Default)
          {
          GotDefVal=true;
          iDefVal=pValueList->m_lVal;
          }
        }

      if (!Done)
        {
        Strng S(pStr);
        S.LRTrim();
        if (S.XStrSpn("+-0123456789")>=S.Length())
          {
          LclDataValue.SetTypeLong(iType, SafeAtoL(S()));
          Done=1;
          if (iCnv>DC_)
            CnvIsBad=1;
          }
        }

      if (!Done && GotDefVal)
        {
        LclDataValue.SetTypeLong(iType, iDefVal);
        Done=1;
        if (iCnv>DC_)
          CnvIsBad=1;
        }
      }

    if (!Done && (IsIntData(iType) || IsUnSgnData(iType)))
      {
      Strng Val(DataValue.GetString());
      Val.LRTrim();
      if (Val.XStrSpn("+-0123456789")>=Val.Length())
        {
        LclDataValue.SetTypeLong(iType, SafeAtoL(Val()));
        Done=1;
        if (iCnv>DC_)
          CnvIsBad=1;
        }
      }

    if (!Done && IsFloatData(iType))
      {
      Strng Val, CnvTxt;
      TaggedObject::SplitTagCnv(DataValue.GetString(), Val, CnvTxt);
      if (Val.XStrSpn("+-0123456789.eE")>=Val.Length())
        {
        LclDataValue.SetTypeDouble(iType, SafeAtoF(Val()), iCnv, CnvTxt());
        Done=1;
        }
      }

    if (!Done)
      {
      LclDataValue.SetTypeString(iType, DataValue.GetString());
      if (CnvIsBad)
        {
        //LogError("TagObj", 0, "Bad Cnv String in %");
        };
      }
    return &LclDataValue;
    }

  if (pValueList!=NULL && (IsIntData(DataValue.iType) || IsUnSgnData(DataValue.iType)))
    {
    // Find closest value in list
    flag Done=0;
    if (pValueList!=NULL && (IsIntData(iType) || IsUnSgnData(iType)))
      {
      long Diff=LONG_MAX;
      long L=DataValue.GetLong();
      long LRqd=L;
      for ( ; pValueList->m_pStr; pValueList++)
        {
        long thisDiff=abs(pValueList->m_lVal-L);
        if (thisDiff<Diff)
          {
          Diff=thisDiff;
          LRqd=pValueList->m_lVal;
          }
        }
      if (L!=LRqd)
        {
        LclDataValue.SetTypeLong(iType, LRqd);
        return &LclDataValue;
        }
      }
    }

  if (DataValue.iType!=iType && IsNumData(DataValue.iType) && IsNumData(iType))
    {
    double Val=DataValue.GetDouble();
    DataValue.SetTypeDouble(iType, Val);
    }

  return &DataValue;
  }

//---------------------------------------------------------------------------

#if WITHTAGMONITOR
bool CFieldMonitor::m_On=false;
DWORD CFieldMonitor::m_dwCount=0;
DWORD CFieldMonitor::gs_dwChgSeqNo=0;
#endif

void DDBDataInfo::MonitorParameter(TABOptions Opts, PkDataUnion *pNewValue)
  {
#if WITHTAGMONITOR
  if (!CFieldMonitor::m_On)
    return;

  TaggedObject *pMonObj=m_pRootObj;

  long CurTagLen=m_sCurrentTag.Length();
  if (!m_bCurrentComplete)
    {
    Strng TgSym=pSym;
    if (pSym[0]==0)
      TgSym=pTag;
    m_sCurrentTag+=TgSym;
    }

  Strng NdTag(m_pRootObj->Tag());
  Strng FldTag=&m_sCurrentTag[NdTag.GetLength()+1];

  CFieldMonitorTO &TOMon=pMonObj->m_Monitor;
  CFieldMonitorTOCls &ClsMon=pMonObj->Class()->m_Monitor;

  CFieldMonitor *pFM;
  if (!ClsMon.m_Map.Lookup(FldTag(), pFM))
    {
    PkDataUnion CurrentValue;
    GetValue(Opts, CurrentValue);

    if (pNewValue->EqualNANOK(CurrentValue))
      goto SkipIt;

    CFieldMonitor FM;
    FM.m_sFldTag=FldTag();
    FM.m_iIndex=ClsMon.m_List.GetCount();
    if (iCnv>0 && strlen(Cnvs[iCnv]->SICnv()->Txt())>0)
      FM.m_sFldCnv=Cnvs[iCnv]->SICnv()->Txt();

    #if dbgTagObj
    if (dbgFldMonitorAdd())
      {
      Strng Tg(m_pRootObj->FullObjTag());
      Tg+=".";
      Tg+=FM.m_sFldTag;
      if (FM.m_sFldCnv())
        dbgpln("FldMon ADD %6u: %s (%s) %s[%i] ", CFieldMonitor::m_dwCount, Tg(), FM.m_sFldCnv(), pMonObj->ClassId(), FM.m_iIndex);
      else
        dbgpln("FldMon ADD %6u: %s %s[%i] ", CFieldMonitor::m_dwCount, Tg(), pMonObj->ClassId(), FM.m_iIndex);
      }
    #endif

    CFieldMonitor::m_dwCount++;
    POSITION Pos=ClsMon.m_List.GetHeadPosition();
    //if (Pos)
    //  {
    //  while (Pos)
    //    {
    //    CFieldMonitor * p=&ClsMon.m_List.GetNext(Pos);
    //    if (p->m_sFldTag.XStrICmp(FldTag())>=0)
    //      break;
    //    }
    //  if (Pos)
    //    {
    //    Pos=ClsMon.m_List.InsertBefore(Pos, FM);
    //    pFM=&ClsMon.m_List.GetAt(Pos);
    //    }
    //  else
    //    {
    //    ClsMon.m_List.AddTail(FM);
    //    pFM=&ClsMon.m_List.GetTail();
    //    }
    //  }
    //else
      {
      ClsMon.m_List.AddTail(FM);
      pFM=&ClsMon.m_List.GetTail();
      }
    ClsMon.m_Map.SetAt(pFM->m_sFldTag(), pFM);
    pFM->m_InitialValue.Set(CurrentValue);
    }
  CArray<DataUnion*,DataUnion*> &CV=TOMon.m_CurrentValue;
  CV.SetSize(Max((int)pFM->m_iIndex+1, CV.GetSize()));
  if (CV[pFM->m_iIndex] == NULL)
    CV[pFM->m_iIndex]=new DataUnion;
  CV[pFM->m_iIndex]->Set(*pNewValue);

  pFM->m_dwChgSeqNo=CFieldMonitor::gs_dwChgSeqNo++;

  #if dbgTagObj
  if (dbgFldMonitorChg())
    {
    if (!CV[pFM->m_iIndex]->EqualNANOK(pFM->m_InitialValue))
      {
      Strng Tg(m_pRootObj->FullObjTag());
      Tg+=".";
      Tg+=pFM->m_sFldTag;
      dbgpln("FldMon CHG %6u: %-50s = %20s (%s)", pFM->m_dwChgSeqNo, Tg(), TOMon.m_CurrentValue[pFM->m_iIndex]->GetString(), pFM->m_InitialValue.GetString());
      if (m_sCurrentTag.XStrCmp(Tg)!=0)
        dbgpln("???????????????????????");
      }
    }
  #endif

SkipIt:
  if (!m_bCurrentComplete)
    m_sCurrentTag.SetLength(CurTagLen);

#endif
  };

//===========================================================================
//
//
//
//===========================================================================

void DoBadDataExError(TaggedObject * pThis, dword lHandle, dword lHandle2, dword dwUserInfo)
  {
  LogError(pThis->FullObjTag(), 0, "Bad Data Exchange [%i %#x/%i] {%#x/%i}", lHandle, lHandle2, lHandle2, dwUserInfo, dwUserInfo);
  }

DDEF_Flags DDBDataInfo::GetBool(TABOptions Opts, PkDataUnion &DataValue)
  {
  //PkDataUnion DataValue;
  DataValue.iType=iType;
  DDEF_Flags XtraFlags=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.dwRetFlags=0;
    DCB.Init(Opts, DataValue.iType, lHandle, lHandle2, NULL, dwUserInfo);
    DCB.B=0;
    if (!pThis->DataXchg(DCB)) // NB is your TabgObj::DataXchg Correct ?
      DoBadDataExError(pThis, lHandle, lHandle2, dwUserInfo);
    DataValue.Byte=DCB.B;
    XtraFlags=DCB.dwRetFlags;
    }
  else
    {
    DataValue.Byte=*(flag*)pData;
    }

if (DataDefnBlk::debugging&0x1)
  dbgpln("--------------------------GetBool %s = %i", pSym[0]?pSym:pTag, DataValue.Byte);

  return XtraFlags;
  }

//---------------------------------------------------------------------------

void DDBDataInfo::PutBool(TABOptions Opts, PkDataUnion &DataValue)//, DDBValueLst *pValueList)
  {
  if (PutDataBlocked(Opts, iFlags))
    return;

  TaggedObject::CallGlblTopologyChg(this);

  PkDataUnion LclDataValue;
  PkDataUnion *pDataValue=FixGenericData(LclDataValue, DataValue, iType, iCnv, pValueLst);

  if ((iFlags&DDEF_PARAM)!=0)
    MonitorParameter(Opts, pDataValue);

  flag Chgd=0;
  ASSERT(pDataValue->iType==iType);
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.B=pDataValue->Byte;
    DCB.Init(Opts, iType, lHandle, lHandle2, &DCB.B, dwUserInfo);
    pThis->DataXchg(DCB);
    Chgd=(DCB.B!=pDataValue->Byte);
    Chgd=true; // Above does not work ...
    }
  else
    {
    Chgd=(*(flag*)pData!=pDataValue->Byte);
    *(flag*)pData=pDataValue->Byte;
    }
  if (Chgd)
    {
    pThis->SetGlblResultsValid(false);
    if ((iFlags & DDEF_AFFECTSSTRUCT)!=0)
      pThis->StructureChanged(pThis);
    }
  pThis->bObjModified=True;
  pThis->bUnValidatedData=True;

if (DataDefnBlk::debugging&0x2)
  dbgpln("--------------------------PutBool %s = %i", pSym[0]?pSym:pTag, pDataValue->Byte);

  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

DDEF_Flags DDBDataInfo::GetChar(TABOptions Opts, PkDataUnion &DataValue)
  {
  //PkDataUnion DataValue;
  DataValue.iType=iType;
  DDEF_Flags XtraFlags=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.dwRetFlags=0;
    DCB.Init(Opts, DataValue.iType, lHandle, lHandle2, NULL, dwUserInfo);
    DCB.C=0;
    if (!pThis->DataXchg(DCB)) // NB is your TabgObj::DataXchg Correct ?
      DoBadDataExError(pThis, lHandle, lHandle2, dwUserInfo);
    DataValue.Char=DCB.C;
    XtraFlags=DCB.dwRetFlags;
    }
  else
    {
    DataValue.Char=*(char*)pData;
    }

if (DataDefnBlk::debugging&0x1)
  dbgpln("--------------------------GetChar %s = %i", pSym[0]?pSym:pTag, DataValue.Char);

  return XtraFlags;
  }

//---------------------------------------------------------------------------

void DDBDataInfo::PutChar(TABOptions Opts, PkDataUnion &DataValue)//, DDBValueLst *pValueList)
  {
  if (PutDataBlocked(Opts, iFlags))
    return;

  TaggedObject::CallGlblTopologyChg(this);

  PkDataUnion LclDataValue;
  PkDataUnion *pDataValue=FixGenericData(LclDataValue, DataValue, iType, iCnv, pValueLst);

  if ((iFlags&DDEF_PARAM)!=0)
    MonitorParameter(Opts, pDataValue);

  ASSERT(pDataValue->iType==iType);
  flag Chgd=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.C=pDataValue->Char;
    DCB.Init(Opts, iType, lHandle, lHandle2, &DCB.C, dwUserInfo);
    pThis->DataXchg(DCB);
    Chgd=(DCB.C!=pDataValue->Char);
    Chgd=true; // Above does not work ...
    }
  else
    {
    Chgd=(*(char*)pData!=pDataValue->Char);
    *(char*)pData=pDataValue->Char;
    }
  if (Chgd)
    {
    pThis->SetGlblResultsValid(false);
    if ((iFlags & DDEF_AFFECTSSTRUCT)!=0)
      pThis->StructureChanged(pThis);
    }
  pThis->bObjModified=True;
  pThis->bUnValidatedData=True;

if (DataDefnBlk::debugging&0x2)
  dbgpln("--------------------------PutChar %s = %i", pSym[0]?pSym:pTag, pDataValue->Char);

  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

DDEF_Flags DDBDataInfo::GetByte(TABOptions Opts, PkDataUnion &DataValue)
  {
  //PkDataUnion DataValue;
  DataValue.iType=iType;
  DDEF_Flags XtraFlags=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.dwRetFlags=0;
    DCB.Init(Opts, DataValue.iType, lHandle, lHandle2, NULL, dwUserInfo);
    DCB.B=0;
    if (!pThis->DataXchg(DCB)) // NB is your TabgObj::DataXchg Correct ?
      DoBadDataExError(pThis, lHandle, lHandle2, dwUserInfo);
    DataValue.Byte=DCB.B;
    XtraFlags=DCB.dwRetFlags;
    }
  else
    {
    DataValue.Byte=*(byte*)pData;
    }

if (DataDefnBlk::debugging&0x1)
  dbgpln("--------------------------GetByte %s = %i", pSym[0]?pSym:pTag, DataValue.Byte);

  return XtraFlags;
  }

//---------------------------------------------------------------------------

void DDBDataInfo::PutByte(TABOptions Opts, PkDataUnion &DataValue)//, DDBValueLst *pValueList)
  {
  if (PutDataBlocked(Opts, iFlags))
    return;

  TaggedObject::CallGlblTopologyChg(this);

  PkDataUnion LclDataValue;
  PkDataUnion *pDataValue=FixGenericData(LclDataValue, DataValue, iType, iCnv, pValueLst);

  if ((iFlags&DDEF_PARAM)!=0)
    MonitorParameter(Opts, pDataValue);

  ASSERT(pDataValue->iType==iType);
  flag Chgd=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.B=pDataValue->Byte;
    DCB.Init(Opts, iType, lHandle, lHandle2, &DCB.B, dwUserInfo);
    pThis->DataXchg(DCB);
    Chgd=(DCB.B!=pDataValue->Byte);
    Chgd=true; // Above does not work ...
    }
  else
    {
    Chgd=(*(byte*)pData!=pDataValue->Byte);
    *(byte*)pData=pDataValue->Byte;
    }
  if (Chgd)
    {
    pThis->SetGlblResultsValid(false);
    if ((iFlags & DDEF_AFFECTSSTRUCT)!=0)
      pThis->StructureChanged(pThis);
    }
  pThis->bObjModified=True;
  pThis->bUnValidatedData=True;

if (DataDefnBlk::debugging&0x2)
  dbgpln("--------------------------PutByte %s = %i", pSym[0]?pSym:pTag, pDataValue->Byte);

  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

DDEF_Flags DDBDataInfo::GetWord(TABOptions Opts, PkDataUnion &DataValue)
  {
  //PkDataUnion DataValue;
  DataValue.iType=iType;
  DDEF_Flags XtraFlags=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.dwRetFlags=0;
    DCB.Init(Opts, DataValue.iType, lHandle, lHandle2, NULL, dwUserInfo);
    DCB.W=0;
    if (!pThis->DataXchg(DCB)) // NB is your TabgObj::DataXchg Correct ?
      DoBadDataExError(pThis, lHandle, lHandle2, dwUserInfo);
    DataValue.Word=DCB.W;
    XtraFlags=DCB.dwRetFlags;
    }
  else
    {
    DataValue.Word=*(word*)pData;
    }

if (DataDefnBlk::debugging&0x1)
  dbgpln("--------------------------GetWord %s = %i", pSym[0]?pSym:pTag, DataValue.Word);

  return XtraFlags;
  }

//---------------------------------------------------------------------------

void DDBDataInfo::PutWord(TABOptions Opts, PkDataUnion &DataValue)//, DDBValueLst *pValueList)
  {
  if (PutDataBlocked(Opts, iFlags))
    return;

  TaggedObject::CallGlblTopologyChg(this);

  PkDataUnion LclDataValue;
  PkDataUnion *pDataValue=FixGenericData(LclDataValue, DataValue, iType, iCnv, pValueLst);

  if ((iFlags&DDEF_PARAM)!=0)
    MonitorParameter(Opts, pDataValue);

  ASSERT(pDataValue->iType==iType);
  flag Chgd=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.W=pDataValue->Word;
    DCB.Init(Opts, iType, lHandle, lHandle2, &DCB.W, dwUserInfo);
    pThis->DataXchg(DCB);
    Chgd=(DCB.W!=pDataValue->Word);
    Chgd=true; // Above does not work ...
    }
  else
    {
    Chgd=(*(word*)pData!=pDataValue->Word);
    *(word*)pData=pDataValue->Word;
    }
  if (Chgd)
    {
    pThis->SetGlblResultsValid(false);
    if ((iFlags & DDEF_AFFECTSSTRUCT)!=0)
      pThis->StructureChanged(pThis);
    }
  pThis->bObjModified=True;
  pThis->bUnValidatedData=True;

if (DataDefnBlk::debugging&0x2)
  dbgpln("--------------------------PutWord %s = %i", pSym[0]?pSym:pTag, pDataValue->Word);

  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

DDEF_Flags DDBDataInfo::GetDWord(TABOptions Opts, PkDataUnion &DataValue)
  {
  //PkDataUnion DataValue;
  DataValue.iType=iType;
  DDEF_Flags XtraFlags=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.dwRetFlags=0;
    DCB.Init(Opts, DataValue.iType, lHandle, lHandle2, NULL, dwUserInfo);
    DCB.DW=0;
    if (!pThis->DataXchg(DCB)) // NB is your TabgObj::DataXchg Correct ?
      DoBadDataExError(pThis, lHandle, lHandle2, dwUserInfo);
    DataValue.DWord=DCB.DW;
    XtraFlags=DCB.dwRetFlags;
    }
  else
    {
    DataValue.DWord=*(dword*)pData;
    }

if (DataDefnBlk::debugging&0x1)
  dbgpln("--------------------------GetDWord %s = %i", pSym[0]?pSym:pTag, DataValue.DWord);

  return XtraFlags;
  }

//---------------------------------------------------------------------------

void DDBDataInfo::PutDWord(TABOptions Opts, PkDataUnion &DataValue)//, DDBValueLst *pValueList)
  {
  if (PutDataBlocked(Opts, iFlags))
    return;

  TaggedObject::CallGlblTopologyChg(this);

  PkDataUnion LclDataValue;
  PkDataUnion *pDataValue=FixGenericData(LclDataValue, DataValue, iType, iCnv, pValueLst);

  if ((iFlags&DDEF_PARAM)!=0)
    MonitorParameter(Opts, pDataValue);

  ASSERT(pDataValue->iType==iType);
  flag Chgd=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.DW=pDataValue->DWord;
    DCB.Init(Opts, iType, lHandle, lHandle2, &DCB.DW, dwUserInfo);
    pThis->DataXchg(DCB);
    Chgd=(DCB.DW!=pDataValue->DWord);
    Chgd=true; // Above does not work ...
    }
  else
    {
    Chgd=(*(dword*)pData!=pDataValue->DWord);
    *(dword*)pData=pDataValue->DWord;
    }
  if (Chgd)
    {
    pThis->SetGlblResultsValid(false);
    if ((iFlags & DDEF_AFFECTSSTRUCT)!=0)
      pThis->StructureChanged(pThis);
    }
  pThis->bObjModified=True;
  pThis->bUnValidatedData=True;

if (DataDefnBlk::debugging&0x2)
  dbgpln("--------------------------PutDWord %s = %i", pSym[0]?pSym:pTag, pDataValue->DWord);

  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

DDEF_Flags DDBDataInfo::GetInt(TABOptions Opts, PkDataUnion &DataValue)
  {
  //PkDataUnion DataValue;
  DataValue.iType=iType;
  DDEF_Flags XtraFlags=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.dwRetFlags=0;
    DCB.Init(Opts, DataValue.iType, lHandle, lHandle2, NULL, dwUserInfo);
    DCB.L=0;
    if (!pThis->DataXchg(DCB)) // NB is your TabgObj::DataXchg Correct ?
      DoBadDataExError(pThis, lHandle, lHandle2, dwUserInfo);
    DataValue.Int=DCB.L;
    XtraFlags=DCB.dwRetFlags;
    }
  else
    {
    DataValue.Int=*(int*)pData;
    }

if (DataDefnBlk::debugging&0x1)
  dbgpln("--------------------------GetInt %s = %i", pSym[0]?pSym:pTag, DataValue.Int);

  return XtraFlags;
  }

//---------------------------------------------------------------------------

void DDBDataInfo::PutInt(TABOptions Opts, PkDataUnion &DataValue)//, DDBValueLst *pValueList)
  {
  if (PutDataBlocked(Opts, iFlags))
    return;

  TaggedObject::CallGlblTopologyChg(this);

  PkDataUnion LclDataValue;
  PkDataUnion *pDataValue=FixGenericData(LclDataValue, DataValue, iType, iCnv, pValueLst);

  if ((iFlags&DDEF_PARAM)!=0)
    MonitorParameter(Opts, pDataValue);

  ASSERT(pDataValue->iType==iType);
  flag Chgd=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.L=pDataValue->Byte;
    DCB.Init(Opts, iType, lHandle, lHandle2, &DCB.L, dwUserInfo);
    pThis->DataXchg(DCB);
    Chgd=(DCB.L!=pDataValue->Byte);
    Chgd=true; // Above does not work ...
    }
  else
    {
    Chgd=(*(int*)pData!=pDataValue->Int);
    *(int*)pData=pDataValue->Int;
    }
  if (Chgd)
    {
    pThis->SetGlblResultsValid(false);
    if ((iFlags & DDEF_AFFECTSSTRUCT)!=0)
      pThis->StructureChanged(pThis);
    }
  pThis->bObjModified=True;
  pThis->bUnValidatedData=True;

if (DataDefnBlk::debugging&0x2)
  dbgpln("--------------------------PutInt %s = %i", pSym[0]?pSym:pTag, pDataValue->Int);

  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

DDEF_Flags DDBDataInfo::GetShort(TABOptions Opts, PkDataUnion &DataValue)
  {
  //PkDataUnion DataValue;
  DataValue.iType=iType;
  DDEF_Flags XtraFlags=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.dwRetFlags=0;
    DCB.Init(Opts, DataValue.iType, lHandle, lHandle2, NULL, dwUserInfo);
    DCB.S=0;
    if (!pThis->DataXchg(DCB)) // NB is your TabgObj::DataXchg Correct ?
      DoBadDataExError(pThis, lHandle, lHandle2, dwUserInfo);
    DataValue.Short=DCB.S;
    XtraFlags=DCB.dwRetFlags;
    }
  else
    {
    DataValue.Short=*(short*)pData;
    }

if (DataDefnBlk::debugging&0x1)
  dbgpln("--------------------------GetShort %s = %i", pSym[0]?pSym:pTag, DataValue.Short);

  return XtraFlags;
  }

//---------------------------------------------------------------------------

void DDBDataInfo::PutShort(TABOptions Opts, PkDataUnion &DataValue)//, DDBValueLst *pValueList)
  {
  if (PutDataBlocked(Opts, iFlags))
    return;

  TaggedObject::CallGlblTopologyChg(this);

  PkDataUnion LclDataValue;
  PkDataUnion *pDataValue=FixGenericData(LclDataValue, DataValue, iType, iCnv, pValueLst);

  if ((iFlags&DDEF_PARAM)!=0)
    MonitorParameter(Opts, pDataValue);

  ASSERT(pDataValue->iType==iType);
  flag Chgd=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.S=pDataValue->Short;
    DCB.Init(Opts, iType, lHandle, lHandle2, &DCB.S, dwUserInfo);
    pThis->DataXchg(DCB);
    Chgd=(DCB.S!=pDataValue->Short);
    Chgd=true; // Above does not work ...
    }
  else
    {
    Chgd=(*(short*)pData!=pDataValue->Short);
    *(short*)pData=pDataValue->Short;
    }
  if (Chgd)
    {
    pThis->SetGlblResultsValid(false);
    if ((iFlags & DDEF_AFFECTSSTRUCT)!=0)
      pThis->StructureChanged(pThis);
    }
  pThis->bObjModified=True;
  pThis->bUnValidatedData=True;

if (DataDefnBlk::debugging&0x2)
  dbgpln("--------------------------PutShort %s = %i", pSym[0]?pSym:pTag, pDataValue->Short);

  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

DDEF_Flags DDBDataInfo::GetLong(TABOptions Opts, PkDataUnion &DataValue)
  {
  //PkDataUnion DataValue;
  DataValue.iType=iType;
  DDEF_Flags XtraFlags=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.dwRetFlags=0;
    DCB.Init(Opts, DataValue.iType, lHandle, lHandle2, NULL, dwUserInfo);
    DCB.L=0;
    if (!pThis->DataXchg(DCB)) // NB is your TabgObj::DataXchg Correct ?
      DoBadDataExError(pThis, lHandle, lHandle2, dwUserInfo);
    DataValue.Long=DCB.L;
    XtraFlags=DCB.dwRetFlags;
    }
  else
    {
    DataValue.Long=*(long*)pData;
    }

if (DataDefnBlk::debugging&0x1)
  dbgpln("--------------------------GetLong %s = %i", pSym[0]?pSym:pTag, DataValue.Byte);

  return XtraFlags;
  }

//---------------------------------------------------------------------------

void DDBDataInfo::PutLong(TABOptions Opts, PkDataUnion &DataValue)//, DDBValueLst *pValueList)
  {
  if (PutDataBlocked(Opts, iFlags))
    return;

  TaggedObject::CallGlblTopologyChg(this);

  PkDataUnion LclDataValue;
  PkDataUnion *pDataValue=FixGenericData(LclDataValue, DataValue, iType, iCnv, pValueLst);

  if ((iFlags&DDEF_PARAM)!=0)
    MonitorParameter(Opts, pDataValue);

  ASSERT(pDataValue->iType==iType);
  flag Chgd=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.L=pDataValue->Long;
    DCB.Init(Opts, iType, lHandle, lHandle2, &DCB.L, dwUserInfo);
    pThis->DataXchg(DCB);
    Chgd=(DCB.L!=pDataValue->Long);
    Chgd=true; // Above does not work ...
    }
  else
    {
    Chgd=(*(long*)pData!=pDataValue->Long);
    *(long*)pData=pDataValue->Long;
    }
  if (Chgd)
    {
    pThis->SetGlblResultsValid(false);
    if ((iFlags & DDEF_AFFECTSSTRUCT)!=0)
      pThis->StructureChanged(pThis);
    }
  pThis->bObjModified=True;
  pThis->bUnValidatedData=True;

if (DataDefnBlk::debugging&0x2)
  dbgpln("--------------------------PutLong %s = %i", pSym[0]?pSym:pTag, pDataValue->Long);

  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

DDEF_Flags DDBDataInfo::GetFloat(TABOptions Opts, PkDataUnion &DataValue)
  {
  //PkDataUnion DataValue;
  DataValue.iType=tt_Float;
  DDEF_Flags XtraFlags=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.dwRetFlags=0;
    DCB.Init(Opts, DataValue.iType, lHandle, lHandle2, NULL, dwUserInfo);
    DCB.F=fNAN;
    if (!pThis->DataXchg(DCB)) // NB is your TabgObj::DataXchg Correct ?
      DoBadDataExError(pThis, lHandle, lHandle2, dwUserInfo);
    DataValue.Float=DCB.F;
    XtraFlags=DCB.dwRetFlags;
    }
  else
    DataValue.Float=*(float*)pData;

if (DataDefnBlk::debugging&0x1)
  dbgpln("--------------------------GetFloat %s = %g", pSym[0]?pSym:pTag, DataValue.Float);

  return XtraFlags;
  }

//---------------------------------------------------------------------------

void DDBDataInfo::PutFloat(TABOptions Opts, PkDataUnion &DataValue)
  {
  if (PutDataBlocked(Opts, iFlags))
    return;

  TaggedObject::CallGlblTopologyChg(this);

  PkDataUnion LclDataValue;
  PkDataUnion *pDataValue=FixGenericData(LclDataValue, DataValue, tt_Float, iCnv, NULL);

  if ((iFlags&DDEF_PARAM)!=0)
    MonitorParameter(Opts, pDataValue);

  ASSERT(pDataValue->iType==iType);
  flag Chgd=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.m_bWasNAN=((iFlags & DDEF_NAN_OK)==0) && !Valid(pDataValue->Float);
    DCB.F=(((iFlags & DDEF_NAN_OK)==0) && !Valid(pDataValue->Float)) ? 0.0F : pDataValue->Float;
    DCB.Init(Opts, iType, lHandle, lHandle2, &DCB.F, dwUserInfo);
    pThis->DataXchg(DCB);
    flag V0=Valid(DCB.F);
    flag V1=Valid(pDataValue->Float);
    if (V0 && V1)
      Chgd=(DCB.F!=pDataValue->Float);
    else
      Chgd=(V0 != V1);
    Chgd=true; // Above does not work ...
    }
  else
    {
    flag V0=Valid(*(float*)pData);
    flag V1=Valid(pDataValue->Float);
    if (V0 && V1)
      Chgd=(*(float*)pData!=pDataValue->Float);
    else
      Chgd=(V0 != V1);
    *(float*)pData=(((iFlags & DDEF_NAN_OK)==0) && !V1) ? 0.0F : pDataValue->Float;
    }
  if (Chgd)
    {
    pThis->SetGlblResultsValid(false);
    if ((iFlags & DDEF_AFFECTSSTRUCT)!=0)
      pThis->StructureChanged(pThis);
    }
  pThis->bObjModified=True;
  pThis->bUnValidatedData=True;

if (DataDefnBlk::debugging&0x2)
  dbgpln("--------------------------PutFloat %s = %g", pSym[0]?pSym:pTag, pDataValue->Float);

  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

DDEF_Flags DDBDataInfo::GetDouble(TABOptions Opts, PkDataUnion &DataValue)
  {
  //PkDataUnion DataValue;
  DataValue.iType=tt_Double;
  DDEF_Flags XtraFlags=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.dwRetFlags=0;
    DCB.Init(Opts, DataValue.iType, lHandle, lHandle2, NULL, dwUserInfo);
    DCB.D=dNAN;
    if (!pThis->DataXchg(DCB)) // NB is your TabgObj::DataXchg Correct ?
      DoBadDataExError(pThis, lHandle, lHandle2, dwUserInfo);
    DataValue.Double=DCB.D;
    XtraFlags=DCB.dwRetFlags;
    }
  else
    DataValue.Double=*(double*)pData;

if (DataDefnBlk::debugging&0x1)
  dbgpln("--------------------------GetDouble %s = %g", pSym[0]?pSym:pTag, DataValue.Double);

  return XtraFlags;
  }

//---------------------------------------------------------------------------

void DDBDataInfo::PutDouble(TABOptions Opts, PkDataUnion &DataValue)
  {
  if (PutDataBlocked(Opts, iFlags))
    {
    if (DataDefnBlk::debugging)
      dbgpln("--------------------------BlkDouble %s", pSym[0]?pSym:pTag);
    return;
    }

  TaggedObject::CallGlblTopologyChg(this);

  PkDataUnion LclDataValue;
  PkDataUnion *pDataValue=FixGenericData(LclDataValue, DataValue, tt_Double, iCnv, NULL);

  if ((iFlags&DDEF_PARAM)!=0)
    MonitorParameter(Opts, pDataValue);

  ASSERT(pDataValue->iType==iType);
  flag Chgd=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    if (((iFlags & DDEF_NAN_OK)==0) && !Valid(pDataValue->Double))
      {
      DCB.m_bWasNAN=true;
      DCB.m_dNANValue=pDataValue->Double;
      }
    else
      DCB.m_bWasNAN=false;
    DCB.D=(((iFlags & DDEF_NAN_OK)==0) && !Valid(pDataValue->Double)) ? 0.0 : pDataValue->Double;
    DCB.Init(Opts, iType, lHandle, lHandle2, &DCB.D, dwUserInfo);
    pThis->DataXchg(DCB);
    flag V0=Valid(DCB.D);
    flag V1=Valid(pDataValue->Double);
    if (V0 && V1)
      Chgd=(DCB.D!=pDataValue->Double);
    else
      Chgd=(V0 != V1);
    Chgd=true; // Above does not work ...
    }
  else
    {
    flag V0=Valid(*(double*)pData);
    flag V1=Valid(pDataValue->Double);
    if (V0 && V1)
      Chgd=(*(double*)pData!=pDataValue->Double);
    else
      Chgd=(V0 != V1);
    *(double*)pData=(((iFlags & DDEF_NAN_OK)==0) && !V1) ? 0.0 : pDataValue->Double;
    }

  if (Chgd)
    {
    pThis->SetGlblResultsValid(false);
    if ((iFlags & DDEF_AFFECTSSTRUCT)!=0)
      pThis->StructureChanged(pThis);
    }
  pThis->bObjModified=True;
  pThis->bUnValidatedData=True;

if (DataDefnBlk::debugging&0x2)
  dbgpln("--------------------------PutDouble %s = %g", pSym[0]?pSym:pTag, pDataValue->Double);

  }

//---------------------------------------------------------------------------

DDEF_Flags DDBDataInfo::GetString(TABOptions Opts, PkDataUnion &DataValue)
  {
  //PkDataUnion DataValue;
  DataValue.iType=iType;
  DDEF_Flags XtraFlags=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.dwRetFlags=0;
    DCB.Init(Opts, DataValue.iType, lHandle, lHandle2, NULL, dwUserInfo);
    DCB.pC="";
    if (!pThis->DataXchg(DCB)) // NB is your TabgObj::DataXchg Correct ?
      DoBadDataExError(pThis, lHandle, lHandle2, dwUserInfo);
    DataValue.StringSet(DCB.pC ? DCB.pC : "");
    XtraFlags=DCB.dwRetFlags;
    }
  else
    {
    Strng* ps=(Strng*)pData;
    char* p=ps ? ps->Str() : NULL;
    DataValue.StringSet(p ? p : "");
    //char * p=((Strng*)pData)->Str();
    //DataValue.StringSet(p ? p : "");
    }

if (DataDefnBlk::debugging&0x1)
  dbgpln("--------------------------GetString %s = %s", pSym[0]?pSym:pTag, DataValue.String());

  return XtraFlags;
  }

//---------------------------------------------------------------------------

void DDBDataInfo::PutString(TABOptions Opts, PkDataUnion &DataValue)//, DDBValueLst *pValueList)
  {
  if (PutDataBlocked(Opts, iFlags))
    return;

  TaggedObject::CallGlblTopologyChg(this);

  PkDataUnion LclDataValue;
  PkDataUnion *pDataValue=FixGenericData(LclDataValue, DataValue, iType, iCnv, pValueLst);

  if ((iFlags&DDEF_PARAM)!=0)
    MonitorParameter(Opts, pDataValue);

  ASSERT(pDataValue->iType==iType);
  flag Chgd=0;
  if (lHandle)
    {
    DataChangeBlk DCB(this);
    DCB.pC=pDataValue->String();
    DCB.Init(Opts, iType, lHandle, lHandle2, DCB.pC, dwUserInfo);
    pThis->DataXchg(DCB);
    if (DCB.pC==NULL)
      Chgd=(strlen(pDataValue->String())>0);
    else
      Chgd=(strcmp(DCB.pC, pDataValue->String())!=0);
    Chgd=true; // Above does not work ...
    }
  else
    {
    Chgd=(((Strng*)pData)->XStrCmp(pDataValue->String())!=0);
    *(Strng*)pData=pDataValue->String();
    }
  if (Chgd)
    {
    pThis->SetGlblResultsValid(false);
    if ((iFlags & DDEF_AFFECTSSTRUCT)!=0)
      pThis->StructureChanged(pThis);
    }
  pThis->bObjModified=True;
  pThis->bUnValidatedData=True;

if (DataDefnBlk::debugging&0x2)
  dbgpln("--------------------------PutString %s = %s", pSym[0]?pSym:pTag, pDataValue->String());

  return;
  }

//---------------------------------------------------------------------------

DDEF_Flags DDBDataInfo::GetBlob(TABOptions Opts, PkDataUnion &DataValue)
  {
  DataValue.iType=iType;
  DDEF_Flags XtraFlags=0;
  if (lHandle)
    {
    char Buff[DataItemBuffSize]; // Temporary Space
    DataChangeBlk DCB(this);
    DCB.dwRetFlags=0;
    DCB.Init(Opts, DataValue.iType, lHandle, lHandle2, NULL, dwUserInfo);
    DCB.pBlob=Buff;

    if (!pThis->DataXchg(DCB)) // NB is your TabgObj::DataXchg Correct ?
      DoBadDataExError(pThis, lHandle, lHandle2, dwUserInfo);

    ASSERT_ALWAYS(DCB.wBlobLen<DataItemBuffSize, "Bad Blob Size", __FILE__, __LINE__);

    DataValue.BlobSet(DCB.pBlob, DCB.wBlobLen);
    XtraFlags=DCB.dwRetFlags;
    }
  else
    {
    //char * p=((Strng*)pData)->Str();
    ASSERT_ALWAYS(wBlobSize<DataItemBuffSize, "Bad Blob Size", __FILE__, __LINE__);
    DataValue.BlobSet(pData, wBlobSize);
    }

if (DataDefnBlk::debugging&0x1)
  dbgpln("--------------------------GetBlob %s ", pTag);

  return XtraFlags;
  }

//---------------------------------------------------------------------------

void DDBDataInfo::PutBlob(TABOptions Opts, PkDataUnion &DataValue)
  {
  if (PutDataBlocked(Opts, iFlags))
    return;

  TaggedObject::CallGlblTopologyChg(this);

  ASSERT(DataValue.iType==iType);
  flag Chgd=1;
  if (lHandle)
    {
//  DataChangeBlk DCB;
//  DCB.pC=pDataValue->String();
//  DCB.Init(Opts, iType, lHandle, DCB.pC, dwUserInfo);
//  pThis->DataXchg(DCB);
    }
  else
    {
//  Chgd=1;
//  *(Strng*)pData=pDataValue->String();
    }
  if (Chgd)
    {
    pThis->SetGlblResultsValid(false);
    if ((iFlags & DDEF_AFFECTSSTRUCT)!=0)
      pThis->StructureChanged(pThis);
    }
  pThis->bObjModified=True;
  pThis->bUnValidatedData=True;

if (DataDefnBlk::debugging&0x2)
  dbgpln("--------------------------PutBlob %s ", pTag);

  return;
  }

//========================================================================
//
//
//
//========================================================================

#if XHEAP_SPARES
DDBPkListArraySpares DDBPkListArray::Spares;
DDBPkListMapSpares DDBPkListMap::Spares;
DDBTagMapSpares DDBTagMap::Spares;
#endif

CTagRefStatusFnHook * DDBDataInfo::gs_TagRefGlblFn=NULL;
flag DataDefnBlk::debugging=0;

DataDefnBlk::DataDefnBlk(CPkDataList * List)//, CPkDataItem * * PItem)
  {
  m_pList = List;
  m_pItem = List ? &List->Item:NULL;

  m_pFullItemTag=NULL;
  pItemList=NULL;
  m_nParms=0;
  }

//------------------------------------------------------------------------

DataDefnBlk::~DataDefnBlk()
  {
  ASSERT(m_pFullItemTag==NULL);
  if (pItemList)
    pItemList->Free();
  pItemList=NULL;
  }

//------------------------------------------------------------------------

flag DataDefnBlk::GetReqdItem()
  {
//  if (!Nest[lCallLevel].fBlockReqd)
//    return False;
  if (!Nest[lCallLevel].fSearching && !Nest[lCallLevel].fWantAll)
    {
    #if dbgSearchTag
    dbgp("%*s%*s %s ABS", 1+lCallLevel," ", 10-lCallLevel," "," --");
    for (int i=0; i<=lCallLevel; i++)
      dbgp(" %i%i%i", Nest[i].fWantAll, Nest[i].fBlockReqd, Nest[i].fSearching);
    dbgpln(" %s %s", pTag, pSym);
    #endif
    return False;
    }
  flag WantItem=False;
  int Pos=0;
  if (pSym[0])
    {
    Pos=TaggedObject::TagCmp(pSym, Nest[lCallLevel].pSrchTag);
    if (Pos > 0) // Part of the Sym is the same
      {
      // is this the end of the tag
      if (strlen(Nest[lCallLevel].pSrchTag+Pos)==0)
        {
        Nest[lCallLevel].pSrchTag+=Pos;
        WantItem=true;
        }
      }
    }
  if (Pos==0 && pTag[0])
    {
    Pos=TaggedObject::TagCmp(pTag, Nest[lCallLevel].pSrchTag);
    if (Pos > 0) // Part of the Tag is the same
      {
      // is this the end of the tag
      if (strlen(Nest[lCallLevel].pSrchTag+Pos)==0)
        {
        Nest[lCallLevel].pSrchTag+=Pos;
        WantItem=true;
        }
      }
    }
  if (Pos==0 && Nest[lCallLevel].fWantAll)
    WantItem=True;
  #if dbgSearchTag
  dbgp("%*s%*s %s ABS", 1+lCallLevel," ", 10-lCallLevel," ",WantItem && GetReqdFlags()?"Rqd":"   ");
  for (int i=0; i<=lCallLevel; i++)
    dbgp(" %i%i%i", Nest[i].fWantAll, Nest[i].fBlockReqd, Nest[i].fSearching);
  dbgpln(" %s %s", pTag, pSym);
  #endif
  return WantItem && GetReqdFlags();
  }

//------------------------------------------------------------------------

flag DataDefnBlk::GetReqdItemFn()
  {
  if (!Nest[lCallLevel].fSearching && !Nest[lCallLevel].fWantAll)
    {
    #if dbgSearchTag
    dbgp("%*s%*s %s ABS", 1+lCallLevel," ", 10-lCallLevel," "," --");
    for (int i=0; i<=lCallLevel; i++)
      dbgp(" %i%i%i", Nest[i].fWantAll, Nest[i].fBlockReqd, Nest[i].fSearching);
    dbgpln(" %s %s", pTag, pSym);
    #endif
    return False;
    }
  flag WantItem=False;
  int Pos=0;
  if (pSym[0])
    {
    Pos=TaggedObject::TagCmpFn(pSym, Nest[lCallLevel].pSrchTag);
    if (Pos > 0) // Part of the Sym is the same
      {
      Nest[lCallLevel].pSrchTag+=Pos;
      WantItem=True;//(strlen(Nest[lCallLevel].pSrchTag)==0);
      }
    }
  if (Pos==0 && pTag[0])
    {
    Pos=TaggedObject::TagCmpFn(pTag, Nest[lCallLevel].pSrchTag);
    if (Pos > 0) // Part of the Tag is the same
      {
      Nest[lCallLevel].pSrchTag+=Pos;
      WantItem=True;//(strlen(Nest[lCallLevel].pSrchTag)==0);
      }
    }
  if (Pos==0 && Nest[lCallLevel].fWantAll)
    WantItem=True;
  return WantItem && GetReqdFlags();
  }

//---------------------------------------------------------------------------

flag DataDefnBlk::GetReqdLayout()
  {
  flag WantItem;
  if (Nest[lCallLevel].fWantAll)
    WantItem=True;
  else
    WantItem=False;
  return WantItem && GetReqdFlags();
  }

//---------------------------------------------------------------------------

flag DataDefnBlk::GetReqdBlock(flag & SearchingTag, LPSTR &pSrchTag)
  {
  if (!Nest[lCallLevel].fSearching && !Nest[lCallLevel].fWantAll)
    {
    #if dbgSearchTag
    dbgp("%*s%*s %s ABS", 1+lCallLevel," ", 10-lCallLevel," "," --");
    for (int i=0; i<=lCallLevel; i++)
      dbgp(" %i%i%i", Nest[i].fWantAll, Nest[i].fBlockReqd, Nest[i].fSearching);
    dbgpln(" %s %s", pTag, pSym);
    #endif
    return False;
    }
  SearchingTag=False;
  flag WantItem=False;
  int Pos=0;
  if (pSym[0])
    {
    Pos=TaggedObject::TagCmp(pSym, pSrchTag);
    if (Pos > 0) // Part of the Sym is the same
      {
      pSrchTag+=Pos;
      WantItem=(strlen(pSrchTag)==0);
      SearchingTag=1;
      }
    }
  if (Pos==0 && pTag[0])
    {
    Pos=TaggedObject::TagCmp(pTag, pSrchTag);
    if (Pos > 0) // Part of the Tag is the same
      {
      pSrchTag+=Pos;
      WantItem=(strlen(pSrchTag)==0);
      SearchingTag=1;
      }
    }
  if (Pos==0 && Nest[lCallLevel].fWantAll)
    {
    WantItem=True;
    SearchingTag=1;
    }
  return WantItem && GetReqdFlags();
  }

//---------------------------------------------------------------------------

flag DataDefnBlk::PutReqdItem()
  {
  flag WantItem=False;
  int Pos=0;
  if (pSym[0])
    {
    Pos=TaggedObject::TagCmp(pSym, Nest[lCallLevel].pSrchTag);
    if (Pos > 0) // Part of the Sym is the same
      {
      // is this the end of the tag
      if (strlen(Nest[lCallLevel].pSrchTag+Pos)==0)
        {
        Nest[lCallLevel].pSrchTag+=Pos;
        WantItem=true;
        }
      //Nest[lCallLevel].pSrchTag+=Pos;
      //WantItem=(strlen(Nest[lCallLevel].pSrchTag)==0);
      }
    }
  if (Pos==0 && pTag[0])
    {
    Pos=TaggedObject::TagCmp(pTag, Nest[lCallLevel].pSrchTag);
    if (Pos > 0) // Part of the Tag is the same
      {
      // is this the end of the tag
      if (strlen(Nest[lCallLevel].pSrchTag+Pos)==0)
        {
        Nest[lCallLevel].pSrchTag+=Pos;
        WantItem=true;
        }
      //Nest[lCallLevel].pSrchTag+=Pos;
      //WantItem=(strlen(Nest[lCallLevel].pSrchTag)==0);
      }
    }
  if (Pos==0 && Nest[lCallLevel].fWantAll)
    WantItem=True;

  return WantItem;
  }

//------------------------------------------------------------------------

flag DataDefnBlk::PutReqdBlock(flag & SearchingTag, LPSTR &pSrchTag)
  {
  SearchingTag=False;
  flag WantItem=False;
  int Pos=0;
  if (pSym[0])
    {
    Pos=TaggedObject::TagCmp(pSym, pSrchTag);
    if (Pos > 0) // Part of the Sym is the same
      {
      pSrchTag+=Pos;
      WantItem=(strlen(pSrchTag)==0);
      SearchingTag=1;
      }
    }
  if (Pos==0 && pTag[0])
    {
    Pos=TaggedObject::TagCmp(pTag, pSrchTag);
    if (Pos > 0) // Part of the Tag is the same
      {
      pSrchTag+=Pos;
      WantItem=(strlen(pSrchTag)==0);
      SearchingTag=1;
      }
    }
  if (Pos==0 && Nest[lCallLevel].fWantAll)
    {
    WantItem=True;
    SearchingTag=1;
    }

  return WantItem;
  }

//------------------------------------------------------------------------

void DataDefnBlk::Open(TaggedObject *pRootObj_, char * FullItemTag, DDBTasks Task, TABOptions Opts, pDDBShortcut Shortcut)
  {
  m_pFullItemTag=FullItemTag;
  eTask=Task;
  dwOptions=Opts;
  fAllInfo = (TABOpt_AllInfo & dwOptions)!=0;
  fValCnvs = ((TABOpt_ValCnvs|TABOpt_AllInfo) & dwOptions)!=0;
  fStrList = ((TABOpt_StrList|TABOpt_AllInfo) & dwOptions)!=0;
  fAllDone=False;
  pShortcut=Shortcut;

  m_pRootObj=pRootObj_;

  lCallLevel=0;
  Nest[lCallLevel].iTyp=0;
  Nest[lCallLevel].fBlockReqd=False;
  Nest[lCallLevel].fItemReqd=False;
  Nest[lCallLevel].fSearching=True;
  Nest[lCallLevel].fWantAll=False;
  Nest[lCallLevel].pDataObj=pRootObj_;
  Nest[lCallLevel].dwDefaultFlags=0;
  Nest[lCallLevel].dwUserInfo=0;
  Nest[lCallLevel].pThis=NULL;
  Nest[lCallLevel].pTag="";
  Nest[lCallLevel].pSrchTag=FullItemTag;
  //Nest[lCallLevel].m_CurrentTagEnd=m_CurrentTag;
  Nest[lCallLevel].m_CurrentTagLen=0;

  //strcpy(m_CurrentTag, "");
  m_bCurrentComplete=false;
  m_sCurrentTag="";

  iBlkDepth=0;
  bBlkSuppressed[iBlkDepth]=0;

  iADepth=0;
  AInfo[iADepth].pClass=NULL;
  AInfo[iADepth].nElem=0;
  AInfo[iADepth].iIndex=0;
  AInfo[iADepth].m_ElementClosed=0;

  fWriting=False;
  pItemList=NULL;
  lMsgItemLevel=0;

  switch (eTask)
    {
    case DDBTask_FndData:
      fSetShortcut=pShortcut!=NULL;
      break;
    case DDBTask_GetData:
      //m_pList->Clear();
      //if (pPItem==NULL)
      //  pPItem=m_pList->FirstItem();
      // set to get a short cut to the first
      fSetShortcut=pShortcut!=NULL;
      break;
    case DDBTask_PutData:
      //if (pPItem==NULL)
      //  pPItem=m_pList->FirstItem();
      fSetShortcut=False;
      break;
    };

  Visibility();

#if DoDDBChecks
  if (gs_DevelopementChecks)
    {
    Chk[lCallLevel].lOpenCnts=++lOpenCnt;
    Chk[lCallLevel].pClass=FindClass(pRootObj_->Class()->ClassIdNoVersion());
    strcpy(cChkTag, pRootObj_->ClassId());//FullItemTag);
    Chk[lCallLevel].iTagEnd=strlen(cChkTag);
    Chk[lCallLevel].pTagStart=&cChkTag[Chk[lCallLevel].iTagEnd];
    ASSERT_ALWAYS(Chk[lCallLevel].pTagStart!=NULL, "Chk[lCallLevel].pTagStart", __FILE__, __LINE__);
    }
#endif

  }

//------------------------------------------------------------------------

void DataDefnBlk::Close()
  {
  ASSERT(lCallLevel==0);
  switch (eTask)
    {
    case DDBTask_FndData:
      break;
    case DDBTask_GetData:
      if (fWriting)
        m_pList->AdvanceWrite();//pPItem);
//_asm int 3;
      //m_pItem->Clear();
      //m_pList->AdvanceWrite(/*pPItem*/);
      //if (ppPItem) // Update remote Pointer
      //  *ppPItem=pPItem;
      break;
    case DDBTask_PutData:
//_asm int 3;
      //if (ppPItem) // Update remote Pointer
      //  *ppPItem=pPItem;
      break;
    };
  if (pItemList)
    pItemList->Free();

  pItemList=NULL;
  //pItemMap=NULL;
  m_pFullItemTag=NULL;
#if DoDDBChecks
  if (gs_DevelopementChecks)
    {
    lOpenCnt++;
    cChkTag[Chk[lCallLevel].iTagEnd]=0;
    }
#endif
  }

//------------------------------------------------------------------------

flag DataDefnBlk::Analyse(CPkDataIter &Iter)
  {
  ASSERT(lCallLevel==0);
  ASSERT(eTask==DDBTask_PutData);

  pItemList=DDBPkListArray::Get();
  lNMsgItems=0;
  //for (pPItem=m_pList->FirstItem(); pPItem; m_pList->AdvanceRead(pPItem))
  int BlkLvl=0;
//_asm int 3;
  //CPkDataIter Iter;
  CPkDataItem *pPItem=m_pList->CurrentItem(Iter);
  for (flag Done=0; !m_pList->EndOfList(Iter)/*pPItem*/ && !Done ; pPItem=m_pList->NextItem(Iter)) //AdvanceReadItem(/*pPItem*/))
    {
    char iType=pPItem->Type();
    if (lNMsgItems<=pItemList->GetSize())
      pItemList->SetSize(lNMsgItems+1, 1024);

    DDBPkListItem &Item=pItemList->ElementAt(lNMsgItems++);
    Item.Clear();
    Item.pItem      = pPItem;
    Item.iType      = iType;
    Item.pTag       = pPItem->Contains(PDI_Tag) ? pPItem->SymOrTag() : "";
    Item.iFlags     = pPItem->Contains(PDI_Flags) ? pPItem->Flags() : 0;
    Item.uHash      = HashValue(Item.pTag)%DDBHashMax;

    if (IsStructStart(iType))
      BlkLvl++;
    else if (IsStructEnd(iType))
      {
      BlkLvl--;
      Done=(BlkLvl<=0);
      }
    }

  if (lNMsgItems>1)
    {
    long iItemNo=0;
    long iNesting=0;
    AnalyseItem(NULL, iItemNo, iNesting);
    fSetShortcut=False;
    }
  else
    {
    fSetShortcut=(pShortcut!=NULL);//True;
    }
  lMsgItemLevel=0;
  pParentItem[lMsgItemLevel]=NULL;

  if (debugging&0x8)
    dbgItem(pItemList->ElementAt(0));

  return True;
  }

//--------------------------------------------------------------------------

void DataDefnBlk::AnalyseItem(DDBPkListItem *Parent, long & iItemNo, long &MsgNesting)
  {
  while (iItemNo<lNMsgItems)
    {
    DDBPkListItem &Item=pItemList->ElementAt(iItemNo);
    if (IsStructStart(Item.iType))
      {
      DDBPkListItem *pItemOcc1=NULL;

      if (Parent)
        {
        // Look for a previous occurence of the struct
        pDDBPkListItem p=(*Parent->pMap)[Item.uHash];
        while (p)
          if ((p->iType==Item.iType) && (_stricmp(p->pTag, Item.pTag)==0))
            break;
          else
            p=p->pNextInMap;
        if (p)
          pItemOcc1=p;
        else
          AddToParentMap(*Parent, Item);
        }
      iItemNo++;
      MsgNesting++;
      if (pItemOcc1==NULL)
        {
        if (Item.pMap==NULL)
          Item.pMap=DDBPkListMap::Get();
        Item.pMap->SetSize(DDBHashMax);
        for (int i=0; i<DDBHashMax; i++)
          (*Item.pMap)[i]=NULL;
        AnalyseItem(&Item, iItemNo, MsgNesting);
        }
      else
        AnalyseItem(pItemOcc1, iItemNo, MsgNesting);
      }
    else if (IsStructEnd(Item.iType))
      {
      MsgNesting--;
      iItemNo++;
      break;
      }
    else
      {
      if (Parent)
        AddToParentMap(*Parent, Item);
      iItemNo++;
      }
    }
  };

//--------------------------------------------------------------------------

void DataDefnBlk::AddToParentMap(DDBPkListItem &Parent, DDBPkListItem &Item)
  {
  pDDBPkListItem & HashItem=(*Parent.pMap)[Item.uHash];

  if (HashItem)
    {
    Item.pNextInMap=HashItem;
    HashItem=&Item;
    }
  else
    HashItem=&Item;
  }

//--------------------------------------------------------------------------

DDBPkListItem * DataDefnBlk::FindChildItem(int MsgItemLevel, char *pTag, char * pSym)
  {
  if (MsgItemLevel==0)
    return &pItemList->ElementAt(0);
  DDBPkListItem * pParent=pParentItem[MsgItemLevel];
  int BadType=-1;

  DDBPkListItem *pChild=NULL;
  // First look for Sym - Then Tag                                                              Problems  
  if (pSym && strlen(pSym)>0)
    {
    UINT uHash = HashValue(pSym)%DDBHashMax;
    pChild=(*pParent->pMap)[uHash];
    while (pChild)
      {
      if ((iFlags&DDEF_TAGCASESENS) ?  strcmp(pSym, pChild->pTag)==0 : _stricmp(pSym, pChild->pTag)==0)
        {
        if ((pChild->iType==iType) || ((pChild->iType==tt_Generic) && (IsData(iType))))
          return pChild;
        else if (pChild->iType>=tt_Bool && pChild->iType<=tt_Double && iType>=tt_Bool && iType<=tt_Double )
          goto TypeCast;
        else
          {
          BadType=pChild->iType;
          break;
          }
        }
      pChild=pChild->pNextInMap;
      }
    }
  if (pTag && strlen(pTag)>0)
    {
    UINT uHash = HashValue(pTag)%DDBHashMax;
    pChild=(*pParent->pMap)[uHash];
    while (pChild)
      {
      if ((iFlags&DDEF_TAGCASESENS) ?  strcmp(pTag, pChild->pTag)==0 : _stricmp(pTag, pChild->pTag)==0)
      //if (_stricmp(pTag, pChild->pTag)==0)
        {
        if ((pChild->iType==iType) || ((pChild->iType==tt_Generic) && (IsData(iType))))
          return pChild;
        else if (pChild->iType>=tt_Bool && pChild->iType<=tt_Double && iType>=tt_Bool && iType<=tt_Double )
          goto TypeCast;
        else
          {
          BadType=pChild->iType;
          break;
          }
        }
      //  ((pChild->iType==iType) ||
      //   ((pChild->iType==tt_Generic) && (IsData(iType)))))
      //return pChild;
      pChild=pChild->pNextInMap;
      }
    }
  if (BadType>=0)
    {
    Strng S;
    if (pSym && strlen(pSym)>0 && pTag && strlen(pTag)>0)
      S.Set("%s/%s", pSym,pTag);
    else if (pTag && strlen(pTag)>0)
      S=pTag;
    else if (pSym && strlen(pSym)>0)
      S=pSym;
    else
      S="???";
    LogWarning(pThis->FullObjTag(), 0, "Project save-version (or save-as) likely required because of database change (Type Mismatch for '%s': %s!=%s)", S(), tt_TypeString(BadType), tt_TypeString(iType));
    }
  return NULL;

TypeCast:
  Strng S;
  if (pSym && strlen(pSym)>0 && pTag && strlen(pTag)>0)
    S.Set("%s/%s", pSym,pTag);
  else if (pTag && strlen(pTag)>0)
    S=pTag;
  else if (pSym && strlen(pSym)>0)
    S=pSym;
  else
    S="???";
  LogWarning(pThis->FullObjTag(), 0, "Project save-version (or save-as) likely required because of database change (Type Cast for '%s': %s to %s)", S(), tt_TypeString(pChild->iType), tt_TypeString(iType));
  return pChild;
  }

//--------------------------------------------------------------------------

void DataDefnBlk::dbgItem(DDBPkListItem &Item)
  {
  dbgpln("%-8.8s %5i %s", tt_TypeString(Item.iType), Item.uHash, Item.pTag);
  if (Item.pMap)
    {
    dbgindent(+2);
    for (int i=0; i<DDBHashMax; i++)
      {
      DDBPkListItem *pChild=(*Item.pMap)[i];
      while (pChild)
        {
        dbgItem(*pChild);
        pChild=pChild->pNextInMap;
        }
      }
    dbgindent(-2);
    }
  }

//--------------------------------------------------------------------------

flag DataDefnBlk::BeginBlock(byte Type, TaggedObject *Obj,
                             LPSTR Tag, LPSTR ClassName, LPSTR SubClassName, LPSTR pSubClassName,
                             LPSTR TagOfParent, LPSTR TagComment, dword UserInfo, long NElems, DDEF_Flags BlkFlags, dword RetAddress)
  {
  //CHKSTUFF0()
  VERIFY(iBlkDepth<sizeof(bBlkSuppressed));
  bBlkSuppressed[iBlkDepth+1]=0;
  if (bBlkSuppressed[iBlkDepth++])
    return True;

  if (fAllDone)
    return False;

  LPTSTR pPrimaryCfg=Obj?(LPTSTR)Obj->GetPrimaryCfg():"";
  iType=Type;
  pTag=Tag;
  pSym="";//NULL;
  Nest[lCallLevel].iTyp=Type;
  Nest[lCallLevel].pTag=Tag;
  iFlags=Nest[lCallLevel].dwDefaultFlags;
  //flag SearchingTag=True;
  flag WantAll=Nest[lCallLevel].fWantAll;

#if DoDDBChecks
  if (gs_DevelopementChecks)
    {
    //Chk[lCallLevel].lOpenCnts=++lOpenCnt;
    DoChecks(RetAddress, UserInfo);
    }
#endif

  // In anticipation
  LPSTR &pSrchTag =Nest[lCallLevel+1].pSrchTag;
  flag &fSearching=Nest[lCallLevel+1].fSearching;

  pSrchTag=Nest[lCallLevel].pSrchTag;
  fSearching=True;

  pValueLstMem=NULL;
  pValueLst=NULL;

  if (lCallLevel>0)
    m_sCurrentTag.SetLength(Nest[lCallLevel-1].m_CurrentTagLen);
  m_sCurrentTag+=Nest[lCallLevel].pTag;
  m_sCurrentTag+=".";
  Nest[lCallLevel].m_CurrentTagLen=m_sCurrentTag.Length();

  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (pTag==NULL) pTag="";
      Nest[lCallLevel].fBlockReqd = GetReqdBlock(fSearching, pSrchTag);
      WantAll=fSearching && (strlen(pSrchTag)==0);

      if (Nest[lCallLevel].fBlockReqd)
        {
        pSym = ClassName ? ClassName : "$blk";
        char * pSubClassName = SubClassName ? SubClassName : "";
        if (fWriting)
          m_pList->AdvanceWrite();//pPItem);
        fWriting=True;
        if ((dwOptions & TABOpt_ForSnapShot)!=0)
          m_pItem->AppendStructureS(/*m_pList->GetInfo(),*/ iType, pTag, pSym, pSubClassName, pPrimaryCfg, BlkFlags, NULL, NULL, NULL, NElems/*iSize*/);
        else
          m_pItem->AppendStructureS(/*m_pList->GetInfo(),*/ iType, pTag, pSym, pSubClassName, pPrimaryCfg, BlkFlags, TagOfParent, NULL, TagComment, NElems/*pDesc, pTagComment, iSize*/);
        }

      //if (1 && (dwOptions&TABOpt_XRefStatus))
      //  {
      //  if (lCallLevel>0)
      //    *Nest[lCallLevel-1].m_CurrentTagEnd=0;
      //  strcat(m_CurrentTag, Nest[lCallLevel].pTag);
      //  strcat(m_CurrentTag, ".");
      //  Nest[lCallLevel].m_CurrentTagEnd=&m_CurrentTag[strlen(m_CurrentTag)];
      //  };

      break;
      }
    case DDBTask_FndData:
      {
      if (pTag==NULL) pTag="";
      pSym="";
      pCnvTxt="";
      pCnvFam=NULL;
      Nest[lCallLevel].fBlockReqd = GetReqdBlock(fSearching, pSrchTag);
      WantAll=fSearching && (strlen(pSrchTag)==0);

      if (Nest[lCallLevel].fBlockReqd)
        {
        if (fSetShortcut) // Set Shortcut to First Item
          pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
        fSetShortcut=False;
        }
      break;
      }
    case DDBTask_PutData:
      {
      if (pTag==NULL) pTag="";
      Nest[lCallLevel].fBlockReqd = PutReqdBlock(fSearching, pSrchTag);
      WantAll=fSearching && (strlen(pSrchTag)==0);
//
      if (Nest[lCallLevel].fBlockReqd)
        {
        pParentItem[lMsgItemLevel+1]=FindChildItem(lMsgItemLevel, pTag, pSym);
        if (pParentItem[lMsgItemLevel+1]!=NULL)
          {
          lMsgItemLevel++;
          if (debugging&0x4)
            dbgpln("Enter Blk: %s", pTag);
          }
        else
          {
          Nest[lCallLevel].fBlockReqd=False;
          if (debugging)
            dbgpln("-------------------- Not in Msg : %s", pSym[0]?pSym:pTag);
          }
        }
      break;
      }
    };

  lCallLevel++;
  Nest[lCallLevel].dwDefaultFlags   = (lCallLevel==0 ? 0 : Nest[lCallLevel-1].dwDefaultFlags);
  Nest[lCallLevel].dwUserInfo       = (UserInfo==-1) ? Nest[lCallLevel-1].dwUserInfo: UserInfo;
  Nest[lCallLevel].pThis            = (UserInfo==-1) ? Nest[lCallLevel-1].pThis: Obj;
  Nest[lCallLevel].pDataObj         = Obj;
  Nest[lCallLevel].fWantAll         = WantAll && Nest[lCallLevel-1].fBlockReqd;
  Nest[lCallLevel].fBlockReqd       = Nest[lCallLevel-1].fBlockReqd;
  Nest[lCallLevel].pTag             = Nest[lCallLevel-1].pTag;
//  Nest[lCallLevel].fSearching       = Nest[lCallLevel-1].fSearching;
  Nest[lCallLevel].fItemReqd        = False;
  //Nest[lCallLevel].m_CurrentTagEnd = Nest[lCallLevel-1].m_CurrentTagEnd;
  Nest[lCallLevel].m_CurrentTagLen  = Nest[lCallLevel-1].m_CurrentTagLen;

  dwUserInfo = Nest[lCallLevel].dwUserInfo;
  pThis      = Nest[lCallLevel].pThis;

#if DoDDBChecks
  if (gs_DevelopementChecks)
    {
    Chk[lCallLevel].lOpenCnts=++lOpenCnt;
    strcat(cChkTag, ".");
    strcat(cChkTag, pTag);
    Chk[lCallLevel].iTagEnd=strlen(cChkTag);

    if (Type==tt_Object || Type==tt_Element)
      {
      Chk[lCallLevel].pClass=FindClass(ClassName ? ClassName : Obj->Class()->ClassName());
      Chk[lCallLevel].pTagStart=&cChkTag[Chk[lCallLevel].iTagEnd];
      ASSERT_ALWAYS(Chk[lCallLevel].pTagStart!=NULL, "Chk[lCallLevel].pTagStart", __FILE__, __LINE__);
      }
    else
      {
      Chk[lCallLevel].pClass=Chk[lCallLevel-1].pClass;
      Chk[lCallLevel].pTagStart=Chk[lCallLevel-1].pTagStart;
      ASSERT_ALWAYS(Chk[lCallLevel].pTagStart!=NULL, "Chk[lCallLevel].pTagStart", __FILE__, __LINE__);
      }
    //dbgpln("> '%s:%s'  %i", ClassName, Chk[lCallLevel].pTagStart, Chk[lCallLevel].lOpenCnts);
    }
#endif

  #if dbgSearchTag
  if (lCallLevel==1)
    dbgpln("%s ----------------------------------------------", m_pFullItemTag);
  dbgp("%*s%*s %s ABS", 1+lCallLevel,">", 10-lCallLevel," ",fSearching?"Rqd":"   ");
  for (int i=0; i<=lCallLevel; i++)
   dbgp(" %i%i%i", Nest[i].fWantAll, Nest[i].fBlockReqd, Nest[i].fSearching);
  dbgpln(" %s[%s]", pTag, ClassName);
  #endif

  return /*true ||*/ fSearching;
  }

//--------------------------------------------------------------------------

void DataDefnBlk::EndBlock(DDEF_Flags Flags)
  {
  if (bBlkSuppressed[--iBlkDepth])
    {
    bBlkSuppressed[iBlkDepth]=0;
    return;
    }

//dbgpln("%*s%*s", 1+lCallLevel,"<", 15-lCallLevel," ");

  lCallLevel--;
  dwUserInfo=Nest[lCallLevel].dwUserInfo;
  pThis     =Nest[lCallLevel].pThis;
  if (lCallLevel>0)
    m_sCurrentTag.SetLength(Nest[lCallLevel-1].m_CurrentTagLen);

  switch (eTask)
    {
    case DDBTask_GetData:
      if (Nest[lCallLevel].fBlockReqd)
        {
        if (fWriting)
          m_pList->AdvanceWrite();//pPItem);
        fWriting=True;
        if ((dwOptions & TABOpt_ForSnapShot)!=0)
          m_pItem->AppendStructureE(/*m_pList->GetInfo(),*/ StructEndType(Nest[lCallLevel].iTyp), NULL, Flags);
        else
          m_pItem->AppendStructureE(/*m_pList->GetInfo(),*/ StructEndType(Nest[lCallLevel].iTyp), Nest[lCallLevel].pTag, Flags);
        }
      break;
    case DDBTask_FndData:
      if (Nest[lCallLevel].fBlockReqd)
        {
        }
      break;
    case DDBTask_PutData:
      if (Nest[lCallLevel].fBlockReqd)
        {
        if (debugging&0x4)
          dbgpln("Leave Blk");
        lMsgItemLevel--;
        //m_pList->AdvanceRead(pPItem);
        }
      break;
    };

#if DoDDBChecks
  if (gs_DevelopementChecks)
    {
    cChkTag[Chk[lCallLevel].iTagEnd]=0;
    //dbgpln(" %s  %i", Chk[lCallLevel].pTagStart, Chk[lCallLevel].lOpenCnts);
    }
#endif
  }

//---------------------------------------------------------------------------

flag DataDefnBlk::BeginStruct(TaggedObject *Obj, LPSTR Tag, LPSTR TagComment, DDBPages PageIs, dword UserInfo, DDEF_Flags BlkFlags)
  {
  CHKSTUFF0();
  LPSTR pS= Tag ? Tag : Obj->Tag();
  flag SearchingTag=BeginBlock(tt_Struct, Obj, pS, NULL, NULL, NULL, NULL, TagComment, UserInfo, 0, BlkFlags, asmRetAddress);
  if (!bBlkSuppressed[iBlkDepth-1] && Nest[lCallLevel-1].fBlockReqd)
    Page(pS, PageIs);
  return SearchingTag;
  };

//--------------------------------------------------------------------------

void DataDefnBlk::EndStruct()
  {
  EndBlock();
  }

//---------------------------------------------------------------------------

flag DataDefnBlk::BeginObject(TaggedObject *Obj, LPSTR Tag, LPSTR ObjectId, LPSTR TagComment, DDBPages PageIs, dword UserInfo, DDEF_Flags BlkFlags)
  {
  CHKSTUFF0();
  LPSTR pS= Tag ? Tag : Obj->Tag();
  flag SearchingTag=BeginBlock(tt_Object, Obj, pS, ObjectId, NULL, NULL, NULL, TagComment, UserInfo, 0, BlkFlags, asmRetAddress);
  if (!bBlkSuppressed[iBlkDepth-1] && Nest[lCallLevel-1].fBlockReqd)
    Page(pS, PageIs);
  return SearchingTag;
  };

//--------------------------------------------------------------------------

void DataDefnBlk::EndObject()
  {
  EndBlock();
  }

//--------------------------------------------------------------------------

void DataDefnBlk::PushUserInfo(dword UserInfo, TaggedObject *This)
  {
  lCallLevel++;
  if (lCallLevel<1)
    DoBreak();
  Nest[lCallLevel] = Nest[lCallLevel-1];
  Nest[lCallLevel].dwUserInfo = UserInfo;
  if (This)
    Nest[lCallLevel].pThis = This;

  dwUserInfo = Nest[lCallLevel].dwUserInfo;
  pThis      = Nest[lCallLevel].pThis;

#if DoDDBChecks
  if (gs_DevelopementChecks)
    {
    Chk[lCallLevel]=Chk[lCallLevel-1];
    }
#endif
  }

//--------------------------------------------------------------------------

void DataDefnBlk::PopUserInfo()
  {
  lCallLevel--;
  dwUserInfo=Nest[lCallLevel].dwUserInfo;
  pThis     =Nest[lCallLevel].pThis;
  }

//------------------------------------------------------------------------

void DataDefnBlk::Object(TaggedObject *Obj, TaggedObject *This, LPSTR Tag, LPSTR pTagComment, DDBPages PageIs, dword UserInfo, DDEF_Flags BlkFlags)
  {
  CHKSTUFF0();
  LPSTR pS= Tag ? Tag : Obj->Tag();
  flag SearchingTag=BeginBlock(tt_Object, Obj, pS,
                               Obj->m_pClass->sClassId(), Obj->SubClassId(), Obj->GetPrimaryCfg(),
                               NULL, pTagComment, UserInfo, 0, BlkFlags, asmRetAddress);
  if (SearchingTag || (!bBlkSuppressed[iBlkDepth-1] && Nest[lCallLevel-1].fBlockReqd))
    {
    Page(pS, PageIs);
    //if (pTagComment)
    //  TagComment(pTagComment);
    SuppressNextBlock();
    Obj->BuildDataDefn(*this);
    }
  EndBlock();
  };

//---------------------------------------------------------------------------

flag DataDefnBlk::BeginArray(TaggedObject *pObj, LPSTR pTag, LPSTR pClassName, int nElem, long Handle, DDBPages PageIs, DDEF_Flags BlkFlags)
  {
  CHKSTUFF0();
  flag SearchingTag=BeginBlock(tt_Array, pObj, pTag, pClassName, NULL, NULL, NULL, NULL, -1, nElem, BlkFlags, asmRetAddress);
  if (!bBlkSuppressed[iBlkDepth-1] && Nest[lCallLevel-1].fBlockReqd)
    {
    if (PageIs!=DDB_NoPage)
      {
      LPSTR pS= pTag ? pTag : pObj->Tag();
      Page(pS, PageIs);
      }
    if ((eTask==DDBTask_PutData) && (Handle>0))
      {
      pDDBPkListItem pMsgItem=FindChildItem(lMsgItemLevel-1, pTag, pSym);
      if (pMsgItem)
        {
        CPkDataItem &PkData=*(pMsgItem->pItem);
        long RqdSize=PkData.NElements();
        #if dbgPutData
        dbgpln("Array Found SetLength %i", RqdSize);
        #endif
        DataChangeBlk DCB(this);
        DCB.L=RqdSize;
        DCB.Init(dwOptions, iType, Handle, lHandle2, &DCB.L, dwUserInfo);
        if (!pObj->DataXchg(DCB))
          DoBadDataExError(pThis, Handle, lHandle2, dwUserInfo);
        }
      }
    }
  iADepth++;
  VERIFY(iADepth<MAXSTRUCTNESTING);
  AInfo[iADepth].pClass=pClassName;
  AInfo[iADepth].nElem=nElem;
  AInfo[iADepth].iIndex=-1;
  AInfo[iADepth].m_ElementClosed=false;
  return SearchingTag;
  };

//---------------------------------------------------------------------------

flag DataDefnBlk::BeginElement(TaggedObject *pObj, int iElementIndex, LPSTR TagComment, dword UserInfo, DDBPages PageIs, DDEF_Flags BlkFlags)
  {
  CHKSTUFF0();
  if (!AInfo[iADepth].m_ElementClosed && AInfo[iADepth].iIndex>=0)
    EndBlock();
  AInfo[iADepth].iIndex++;
  AInfo[iADepth].m_ElementClosed=false;

  sprintf(cElementTag, "[%0*i]", TOElementTagDigitCnt(ForFileSnpScn()!=0), iElementIndex);
  flag SearchingTag=BeginBlock(tt_Element, pObj, cElementTag, AInfo[iADepth].pClass, NULL, NULL, NULL, TagComment, UserInfo, 0, BlkFlags, asmRetAddress);
  if (!bBlkSuppressed[iBlkDepth-1] && Nest[lCallLevel-1].fBlockReqd)
    {
    if (PageIs!=DDB_NoPage)
      {
      LPSTR pS= cElementTag ? cElementTag : pObj->Tag();
      Page(pS, PageIs);
      }
    }

  return SearchingTag;
  };

//---------------------------------------------------------------------------

flag DataDefnBlk::BeginElement(TaggedObject *pObj, LPCTSTR pElementTag, LPSTR TagComment, dword UserInfo, DDBPages PageIs, DDEF_Flags BlkFlags)
  {
  CHKSTUFF0();
  if (!AInfo[iADepth].m_ElementClosed && AInfo[iADepth].iIndex>=0)
    EndBlock();
  AInfo[iADepth].iIndex++;
  AInfo[iADepth].m_ElementClosed=false;

  sprintf(cElementTag, "[%.*s]", sizeof(cElementTag)-1, pElementTag);
  flag SearchingTag=BeginBlock(tt_Element, pObj, cElementTag, AInfo[iADepth].pClass, NULL, NULL, NULL, TagComment, UserInfo, 0, BlkFlags, asmRetAddress);
  if (!bBlkSuppressed[iBlkDepth-1] && Nest[lCallLevel-1].fBlockReqd)
    {
    if (PageIs!=DDB_NoPage)
      {
      LPCTSTR pS= pElementTag ? pElementTag : pObj->Tag();
      Page((LPTSTR)pS, PageIs);
      }
    }

  return SearchingTag;
  };

//--------------------------------------------------------------------------

void DataDefnBlk::EndElement(DDEF_Flags Flags)
  {
  VERIFY(iADepth>0);
  VERIFY(AInfo[iADepth].iIndex>=0);
  AInfo[iADepth].m_ElementClosed=true;
  EndBlock(Flags);
  }
  
//---------------------------------------------------------------------------

void DataDefnBlk::EndArray(DDEF_Flags Flags)
  {
  VERIFY(iADepth>0);
  if (!AInfo[iADepth].m_ElementClosed && AInfo[iADepth].iIndex>=0)
    EndBlock();

  iADepth--;
  EndBlock(Flags);
  };

//---------------------------------------------------------------------------

flag DataDefnBlk::BeginMatrix(TaggedObject * Obj, LPSTR Tag, LPSTR ClassName, int MaxCols, int MaxRows, int ColWidth, int Gap, DDBPages PageIs, DDEF_Flags BlkFlags)
  {
  flag Ret=BeginArray(Obj, Tag, ClassName, MaxCols*MaxRows, -1, PageIs, BlkFlags/*|DDEF_STARTMATRIX*/);

  AInfo[iADepth].m_MaxCols=MaxCols;
  AInfo[iADepth].m_PrevRow=-1;

  BeginGrid(Tag, MaxCols, MaxRows, ColWidth, Gap);

  return Ret;
  };

//---------------------------------------------------------------------------

flag DataDefnBlk::BeginMatrix(TaggedObject * Obj, LPSTR Tag, LPSTR ClassName, int MaxCols, int MaxRows, int ColWidth, int Gap, LPSTR ColHdr, LPSTR RowHdr, DDBPages PageIs, DDEF_Flags BlkFlags)
  {
  flag Ret=BeginArray(Obj, Tag, ClassName, MaxCols*MaxRows, -1, PageIs, BlkFlags/*|DDEF_STARTMATRIX*/);

  AInfo[iADepth].m_MaxCols=MaxCols;
  AInfo[iADepth].m_PrevRow=-1;

  BeginGrid(Tag, MaxCols, MaxRows, ColWidth, Gap);

  Strng S;
  ColumnHeader(Tag,ColWidth,0,-1);
  for (int i=0; i<MaxCols; i++)
    ColumnHeader(S.Set("%s[%i]", ColHdr?ColHdr:"", i), ColWidth, Gap, 1);
  for (int i=0; i<MaxRows; i++)
    RowHeader(S.Set("%s[%i]", RowHdr?RowHdr:"", i));

  return Ret;
  };

//---------------------------------------------------------------------------

flag DataDefnBlk::BeginMatrixElement(TaggedObject * Obj, int iColIndex, int iRowIndex, LPSTR TagComment, dword UserInfo, DDBPages PageIs, DDEF_Flags Flags)
  {
  if (iRowIndex > AInfo[iADepth].m_PrevRow)
    {
    AInfo[iADepth].m_PrevRow=iRowIndex;
    };

  return BeginElement(Obj, iColIndex+(iRowIndex*AInfo[iADepth].m_MaxCols), TagComment, UserInfo, PageIs, Flags);
  };

//---------------------------------------------------------------------------

//flag DataDefnBlk::BeginMatrixElement(TaggedObject * Obj, LPCTSTR pColTag, LPCTSTR pRowTag, LPSTR TagComment, dword UserInfo, DDBPages PageIs, DDEF_Flags BlkFlags)
//  {
//  };

//---------------------------------------------------------------------------

void DataDefnBlk::EndMatrixElement(DDEF_Flags Flags)
  {
  EndElement(Flags);
  };

//---------------------------------------------------------------------------

void DataDefnBlk::EndMatrix(DDEF_Flags Flags)
  {
  EndGrid("");
  EndArray(Flags /*| DDEF_ENDMATRIX*/);
  };

//---------------------------------------------------------------------------

flag DataDefnBlk::BeginGrid(LPSTR Text, int MaxCols, int MaxRows, int Width, int Gap)
  {
  if (eTask==DDBTask_GetData)
    {
    if (fAllInfo && GetReqdLayout())
      {
      if (fWriting)
        m_pList->AdvanceWrite();
      fWriting=True;

      Strng T;
      T.Set("%s\t%i\t%i\t%i\t%i\t", Text, MaxCols, MaxRows, Width, Gap);
      m_pItem->AppendLayout(tt_Grid, T(), CurrentFlags());
      }
    }

  return true;
  };

//---------------------------------------------------------------------------

void DataDefnBlk::ColumnHeader(LPSTR Text, int Width, int Gap, int Justification)
  {
  if (eTask==DDBTask_GetData)
    {
    if (fAllInfo && GetReqdLayout())
      {
      if (fWriting)
        m_pList->AdvanceWrite();
      fWriting=True;

      Strng T(Text);
      if (Width>0)
        {
        T.LRTrim();
        if (T.GetLength()>Width)
          T.SetLength(Width); // trunc if neccessary
        //if (Justification<0)
          T.RPad(Width);      
        //else if (Justification>0)
        //  T.LPad(Width);      
        //else
        //  {
        //  int Len=T.GetLength();
        //  int RightSpc=(Width-Len)/2;
        //  T.LPad(Len+RightSpc);      
        //  T.RPad(Width);      
        //  }
        }
      T+="\t";
      int L=T.GetLength()+Gap;
      T.RPad(L);      // lengthen if neccessary
      T.SetLength(L); // trunc if neccessary
      T+="\t";
      T+=Justification<0?"<":Justification>0?">":"";
      T+="\t";

      m_pItem->AppendLayout(tt_ColHdr, T(), CurrentFlags());
      }
    }
  };

//---------------------------------------------------------------------------

void DataDefnBlk::RowHeader(LPSTR Text)
  {
  if (eTask==DDBTask_GetData)
    {
    if (fAllInfo && GetReqdLayout())
      {
      if (fWriting)
        m_pList->AdvanceWrite();
      fWriting=True;
      m_pItem->AppendLayout(tt_RowHdr, Text, CurrentFlags());
      }
    }
  };

//---------------------------------------------------------------------------

void DataDefnBlk::RowStart(LPSTR Text)
  {
  if (eTask==DDBTask_GetData)
    {
    if (fAllInfo && GetReqdLayout())
      {
      if (fWriting)
        m_pList->AdvanceWrite();
      fWriting=True;
      m_pItem->AppendLayout(tt_RowStart, Text, CurrentFlags());
      }
    }
  };

//---------------------------------------------------------------------------

void DataDefnBlk::EndGrid(LPSTR Text)
  {
  if (eTask==DDBTask_GetData)
    {
    if (fAllInfo && GetReqdLayout())
      {
      if (fWriting)
        m_pList->AdvanceWrite();
      fWriting=True;
      m_pItem->AppendLayout(tt_Grid_E, Text, CurrentFlags());
      }
    }
  };

//---------------------------------------------------------------------------

//INLINE void DataDefnBlk::ColumnHeader(LPSTR Txt, int Width, int Gap, DDEF_Flags Flags)
//  {
//  iFlags=Nest[lCallLevel].dwDefaultFlags|Flags;
//  switch (eTask)
//    {
//    case DDBTask_GetData:
//      {
//      if (fAllInfo && GetReqdLayout())//iFlags))
//        {
//        if (fWriting)
//          m_pList->AdvanceWrite();//pPItem);
//        fWriting=True;
//        //m_pItem->Clear();
//        m_pItem->AppendLayout(/*pDataObj->GetInfo(),*/ tt_ColHdr, Txt, CurrentFlags());
//        }
//      break;
//      }
//    case DDBTask_PutData:
//      {
//      //if (PutReqdItem(pTag, pSym))
//      //  {
//      //  // ???????????????????????????????????????
//      //          
//      //  pDataObj->AdvanceRead(rpPItem);
//      //  }
//      break;
//      }
//    };
//  };
//
//
//INLINE void DataDefnBlk::RowHeader(LPSTR Txt, DDEF_Flags Flags)
//  {
//  iFlags=Nest[lCallLevel].dwDefaultFlags|Flags;
//  switch (eTask)
//    {
//    case DDBTask_GetData:
//      {
//      if (fAllInfo && GetReqdLayout())//iFlags))
//        {
//        if (fWriting)
//          m_pList->AdvanceWrite();//pPItem);
//        fWriting=True;
//        //m_pItem->Clear();
//        m_pItem->AppendLayout(/*pDataObj->GetInfo(),*/ tt_RowHdr, Txt, CurrentFlags());
//        }
//      break;
//      }
//    case DDBTask_PutData:
//      {
//      //if (PutReqdItem(pTag, pSym))
//      //  {
//      //  // ???????????????????????????????????????
//      //          
//      //  pDataObj->AdvanceRead(rpPItem);
//      //  }
//      break;
//      }
//    };
//  };


//---------------------------------------------------------------------------

DDEF_Flags DataDefnBlk::Visibility(DDEF_Flags ModeFlags, flag ViewVisible, flag FileVisible, flag SnapVisible, flag ScenVisible)
  {
  DDEF_Flags & dwDefaultFlags=Nest[lCallLevel].dwDefaultFlags;
  DDEF_Flags old=dwDefaultFlags;
  dwDefaultFlags &= ~(NSHM_All|DDEF_NONDB|DDEF_NOVIEW|DDEF_NOFILE|DDEF_NOSNAPSHOT|DDEF_NOSCENARIO);
  dwDefaultFlags |= ModeFlags & NSHM_All;
  if (!ViewVisible)
    dwDefaultFlags |= DDEF_NOVIEW;
  if (!FileVisible)
    dwDefaultFlags |= DDEF_NOFILE;
  if (!SnapVisible)
    dwDefaultFlags |= DDEF_NOSNAPSHOT;
  if (!ScenVisible)
    dwDefaultFlags |= DDEF_NOSCENARIO;
  return old;
  }

//---------------------------------------------------------------------------

DDEF_Flags DataDefnBlk::GetVisibility()
  {
  return Nest[lCallLevel].dwDefaultFlags;
  }

//---------------------------------------------------------------------------

DDEF_Flags DataDefnBlk::SetVisibility(DDEF_Flags VisFlags)
  {
  DDEF_Flags Old=Nest[lCallLevel].dwDefaultFlags;
  Nest[lCallLevel].dwDefaultFlags = VisFlags;
  return Old;
  }

//---------------------------------------------------------------------------

DDEF_Flags DataDefnBlk::CurrentFlags()
  {
  iFlags &= ~MODEVISIBLE;
  if (((dwOptions & iFlags & SM_Inline)!=0) ||
      ((dwOptions & iFlags & SM_Buffered)!=0) ||
      ((dwOptions & iFlags & SM_Direct)!=0) ||
      ((iFlags & NSHM_All)==0)) // Default
    iFlags |= MODEVISIBLE;

  return iFlags;
  }

//---------------------------------------------------------------------------
#if WITHEQUIPSPECS

#define TRYAPPENDSTATUS                                                     \
if (dwOptions&(TABOpt_XRefStatus|TABOPT_ValueStatus))                       \
  {                                                                         \
  CTagRefStatus W=0;                                                        \
  CTagValueStatus S=0;                                                      \
  long CurrentTagLen=Nest[lCallLevel-1].m_CurrentTagLen;                    \
                                                                            \
  m_sCurrentTag.SetLength(CurrentTagLen);                                   \
  if (pTag[0])                                                              \
    {                                                                       \
    m_sCurrentTag+=pTag;                                                    \
    if (dwOptions&TABOpt_XRefStatus)                                        \
      W=gs_TagRefGlblFn->GetTagRefStatus(m_sCurrentTag());                  \
    if (dwOptions&TABOPT_ValueStatus && m_pRootObj->SelectData())           \
      S=m_pRootObj->SelectData()->GetTagValueStatus(m_sCurrentTag());       \
    m_sCurrentTag.SetLength(CurrentTagLen);                                 \
    }                                                                       \
  else if (pSym[0])                                                         \
    {                                                                       \
    m_sCurrentTag+=pSym;                                                    \
    if (dwOptions&TABOpt_XRefStatus)                                        \
      W|=gs_TagRefGlblFn->GetTagRefStatus(m_sCurrentTag());                 \
    if (dwOptions&TABOPT_ValueStatus && m_pRootObj->SelectData())           \
      S|=m_pRootObj->SelectData()->GetTagValueStatus(m_sCurrentTag());      \
    m_sCurrentTag.SetLength(CurrentTagLen);                                 \
    }                                                                       \
  m_pItem->AppendItemStatus(W, S);                                          \
  }

#else

#define TRYAPPENDSTATUS                                                     \
if (dwOptions&(TABOpt_XRefStatus|TABOPT_ValueStatus))                       \
  {                                                                         \
  CTagRefStatus W=0;                                                        \
  long CurrentTagLen=Nest[lCallLevel-1].m_CurrentTagLen;                    \
                                                                            \
  m_sCurrentTag.SetLength(CurrentTagLen);                                   \
  if (pTag[0])  /*strlen(pTag)>0)*/                                         \
    {                                                                       \
    m_sCurrentTag+=pTag;                                                    \
    if (dwOptions&TABOpt_XRefStatus)                                        \
      W=gs_TagRefGlblFn->GetTagRefStatus(m_sCurrentTag());                  \
    m_sCurrentTag.SetLength(CurrentTagLen);                                 \
    }                                                                       \
  if (pSym[0])  /*strlen(pSym)>0)*/                                         \
    {                                                                       \
    m_sCurrentTag+=pSym;                                                    \
    if (dwOptions&TABOpt_XRefStatus)                                        \
      W|=gs_TagRefGlblFn->GetTagRefStatus(m_sCurrentTag());                 \
    m_sCurrentTag.SetLength(CurrentTagLen);                                 \
    }                                                                       \
  m_pItem->AppendItemStatus(W);                                             \
  }

#endif

//---------------------------------------------------------------------------

#if WITHEQUIPSPECS

#define TRYSETSTATUS                                                                                        \
if (m_pRootObj->SelectData())                                                                               \
  {                                                                                                         \
  bool DoingSet = (dwOptions&(TABOPT_ValueStatus))!=0;                                                      \
  long CurrentTagLen=Nest[lCallLevel-1].m_CurrentTagLen;                                                    \
  m_sCurrentTag.SetLength(CurrentTagLen);                                                                   \
  if (pTag[0] && pSym[0])                                                                                   \
    {                                                                                                       \
    Strng Tag(m_sCurrentTag), Sym(m_sCurrentTag);                                                           \
    Tag+=pTag;                                                                                              \
    Sym+=pSym;                                                                                              \
    CTagValueStatus S=m_pRootObj->SelectData()->GetTagValueStatus(Tag());                                   \
    if (DoingSet)                                                                                           \
      m_pRootObj->SelectData()->SetTagValueStatus(Tag(), Sym(), CTOSelectData::VS_Set);                     \
    else if (S & CTOSelectData::VS_Set)                                                                     \
      m_pRootObj->SelectData()->SetTagValueStatus(Tag(), Sym(), S | CTOSelectData::VS_Changed);             \
    }                                                                                                       \
  else if (pTag[0])                                                                                         \
    {                                                                                                       \
    m_sCurrentTag+=pTag;                                                                                    \
    CTagValueStatus S=m_pRootObj->SelectData()->GetTagValueStatus(m_sCurrentTag());                         \
    if (DoingSet)                                                                                           \
      m_pRootObj->SelectData()->SetTagValueStatus(m_sCurrentTag(), NULL, CTOSelectData::VS_Set);            \
    else if (S & CTOSelectData::VS_Set)                                                                     \
      m_pRootObj->SelectData()->SetTagValueStatus(m_sCurrentTag(), NULL, S | CTOSelectData::VS_Changed);    \
    m_sCurrentTag.SetLength(CurrentTagLen);                                                                 \
    }                                                                                                       \
  else if (pSym[0])                                                                                         \
    {                                                                                                       \
    m_sCurrentTag+=pSym;                                                                                    \
    CTagValueStatus S=m_pRootObj->SelectData()->GetTagValueStatus(m_sCurrentTag());                         \
    if (DoingSet)                                                                                           \
      m_pRootObj->SelectData()->SetTagValueStatus(m_sCurrentTag(), NULL, CTOSelectData::VS_Set);            \
    else if (S & CTOSelectData::VS_Set)                                                                     \
      m_pRootObj->SelectData()->SetTagValueStatus(m_sCurrentTag(), NULL, S | CTOSelectData::VS_Changed);    \
    m_sCurrentTag.SetLength(CurrentTagLen);                                                                 \
    }                                                                                                       \
  }

#else

#define TRYSETSTATUS                                                                                        \

#endif

//---------------------------------------------------------------------------

void DataDefnBlk::DoBool()
  {
  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (fWriting)
        m_pList->AdvanceWrite(/*pPItem*/);
      fWriting=True;

      PkDataUnion DataValue;
      DDEF_Flags XtraFlags=GetBool(dwOptions, DataValue);
      m_pItem->AppendDataValue(/*m_pList->GetInfo(),*/ dwOptions, pTag, pSym, XtraFlags|CurrentFlags(), DataValue, iCnv, pCnvTxt, pCnvFam);
      TRYAPPENDSTATUS;
      if (fStrList && pValueLst)
        m_pItem->AppendValueLst(/*m_pList->GetInfo(), */pValueLst, True);
      }
    case DDBTask_FndData:
      {
      if (fSetShortcut) // Set Shortcut to First Item
        pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
      fSetShortcut=False;
      }
      break;
    case DDBTask_PutData:
      {
      pDDBPkListItem pMsgItem=FindChildItem(lMsgItemLevel, pTag, pSym);
      if (pMsgItem)
        {
        CPkDataItem &PkData=*(pMsgItem->pItem);
        PutBool(dwOptions, *PkData.Value());//, pValueLst);
        TRYSETSTATUS;
        //if (PkData.Type()!=tt_Generic)
        //  {
        //  flag ValuesExist=PkData.GetStrList(ValueLst);
        //  PutBool(dwOptions, *PkData.Value(), ValuesExist ? &ValueLst : NULL);
        //  }
        //else
        //  {
        //  GD_StrValue=PkData.Value()->GetString();
        //  PutBool(dwOptions, *PkData.Value(), NULL);
        //  }
        if (fSetShortcut)
          pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
        }
      else
        {
        if (debugging)
          dbgpln("-------------------- Not in Msg : %s", pSym[0]?pSym:pTag);
        }
      }
      break;
    };
  };

//---------------------------------------------------------------------------

void DataDefnBlk::DoChar()
  {
  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (fWriting)
        m_pList->AdvanceWrite(/*pPItem*/);
      fWriting=True;

      PkDataUnion DataValue;
      DDEF_Flags XtraFlags=GetChar(dwOptions, DataValue);
      m_pItem->AppendDataValue(/*m_pList->GetInfo(),*/ dwOptions, pTag, pSym, XtraFlags|CurrentFlags(), DataValue, iCnv, pCnvTxt);
      TRYAPPENDSTATUS;
      if (fStrList && pValueLst)
        m_pItem->AppendValueLst(/*m_pList->GetInfo(), */pValueLst, True);
      }
    case DDBTask_FndData:
      {
      if (fSetShortcut) // Set Shortcut to First Item
        pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
      fSetShortcut=False;
      }
      break;
    case DDBTask_PutData:
      {
      pDDBPkListItem pMsgItem=FindChildItem(lMsgItemLevel, pTag, pSym);
      if (pMsgItem)
        {
        CPkDataItem &PkData=*(pMsgItem->pItem);
        PutChar(dwOptions, *PkData.Value());//, pValueLst);
        TRYSETSTATUS;
        //if (PkData.Type()!=tt_Generic)
        //  {
        //  flag ValuesExist=PkData.GetStrList(ValueLst);
        //  PutChar(dwOptions, *PkData.Value(), ValuesExist ? &ValueLst : NULL);
        //  }
        //else
        //  {
        //  GD_StrValue=PkData.Value()->GetString();
        //  PutChar(dwOptions, *PkData.Value(), NULL);
        //  }
        if (fSetShortcut)
          pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
        }
      else
        {
        if (debugging)
          dbgpln("-------------------- Not in Msg : %s", pSym[0]?pSym:pTag);
        }
      }
      break;
    };
  };

//---------------------------------------------------------------------------

//void DataDefnBlk::IndexedStrValue(int iVal, LPSTR pStr)
//  {
//  if (eTask==DDBTask_GetData && Nest[lCallLevel].fReqd)
//    {
//    if (fAllInfo)
//      {
//      if (pStrLstLen==NULL)
//        m_pItem->AppendFirstStrListItem(pStr, iVal, True, pStrLstLen);
//      else
//        m_pItem->AppendNextStrListItem(pStr, iVal, True, pStrLstLen);
//      }
//    }
//  else if (eTask==DDBTask_PutData && Nest[lCallLevel].fReqd)
//    {
//    if (GD_StrValue.Length()>0)
//      {
//      if (GD_StrValue.XStrICmp(pStr)==0)
//        {
//        if (debugging)
//          dbgpln("-------------------------------- IndexedStrValue : %s %i %s", pSym[0]?pSym:pTag, iVal, pStr);
//        GD_StrValue="";
//        PkDataUnion LclDataValue;
//        LclDataValue.SetTypeLong(iType, iVal);
//        switch (iType)
//          {
//          case tt_Bool   : PutBool  (dwOptions, LclDataValue, NULL);  break;
//          case tt_Byte   : PutByte  (dwOptions, LclDataValue, NULL);  break;
//          case tt_Char   : PutChar  (dwOptions, LclDataValue, NULL);  break;
//          case tt_Word   : PutWord  (dwOptions, LclDataValue, NULL);  break;
//          case tt_DWord  : PutDWord (dwOptions, LclDataValue, NULL);  break;
//          case tt_Int    : PutInt   (dwOptions, LclDataValue, NULL);  break;
//          case tt_Short  : PutShort (dwOptions, LclDataValue, NULL);  break;
//          case tt_Long   : PutLong  (dwOptions, LclDataValue, NULL);  break;
//          case tt_Strng  : PutString(dwOptions, LclDataValue, NULL);  break;
//          default: DoBreak(); break;
//          }
//        }
//      }
//    }
//  }
//
//char * DDBStringLst;
//struct DDBInxStrLst { int iVal, char * pStr; };
//


void DataDefnBlk::DoByte()
  {
  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (fWriting)
        m_pList->AdvanceWrite(/*pPItem*/);
      fWriting=True;

      PkDataUnion DataValue;
      DDEF_Flags XtraFlags=GetByte(dwOptions, DataValue);
      m_pItem->AppendDataValue(/*m_pList->GetInfo(),*/ dwOptions, pTag, pSym, XtraFlags|CurrentFlags(), DataValue, iCnv, pCnvTxt);
      TRYAPPENDSTATUS;
      if (fStrList && pValueLst)
        m_pItem->AppendValueLst(/*m_pList->GetInfo(), */pValueLst, True);

      }
    case DDBTask_FndData:
      {
      if (fSetShortcut) // Set Shortcut to First Item
        pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
      fSetShortcut=False;
      }
      break;
    case DDBTask_PutData:
      {
      pDDBPkListItem pMsgItem=FindChildItem(lMsgItemLevel, pTag, pSym);
      if (pMsgItem)
        {
        CPkDataItem &PkData=*(pMsgItem->pItem);
        PutByte(dwOptions, *PkData.Value());//, pValueLst);
        TRYSETSTATUS;
        //if (PkData.Type()!=tt_Generic)
        //  {
        //  //flag ValuesExist=PkData.GetStrList(ValueLst);
        //  PutByte(dwOptions, *PkData.Value(), NULL);//, ValuesExist ? &ValueLst : NULL);
        //  }
        //else
        //  {
          //GD_StrValue=PkData.Value()->GetString();
          //if (isdigit(GD_StrValue[0]) || GD_StrValue[0]=='-' || GD_StrValue[0]=='+')

        //  }
        if (fSetShortcut)
          pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
        }
      else
        {
        if (debugging)
          dbgpln("-------------------- Not in Msg : %s", pSym[0]?pSym:pTag);
        }
      }
      break;
    };
  };

//---------------------------------------------------------------------------

void DataDefnBlk::DoWord()
  {
  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (fWriting)
        m_pList->AdvanceWrite(/*pPItem*/);
      fWriting=True;

      PkDataUnion DataValue;
      DDEF_Flags XtraFlags=GetWord(dwOptions, DataValue);
      m_pItem->AppendDataValue(/*m_pList->GetInfo(),*/ dwOptions, pTag, pSym, XtraFlags|CurrentFlags(), DataValue, iCnv, pCnvTxt);
      TRYAPPENDSTATUS;
      if (fStrList && pValueLst)
        m_pItem->AppendValueLst(/*m_pList->GetInfo(), */pValueLst, True);
      }
    case DDBTask_FndData:
      {
      if (fSetShortcut) // Set Shortcut to First Item
        pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
      fSetShortcut=False;
      }
      break;
    case DDBTask_PutData:
      {
      pDDBPkListItem pMsgItem=FindChildItem(lMsgItemLevel, pTag, pSym);
      if (pMsgItem)
        {
        CPkDataItem &PkData=*(pMsgItem->pItem);
        PutWord(dwOptions, *PkData.Value());//, pValueLst);
        TRYSETSTATUS;
        //if (PkData.Type()!=tt_Generic)
        //  {
        //  flag ValuesExist=PkData.GetStrList(ValueLst);
        //  PutWord(dwOptions, *PkData.Value(), ValuesExist ? &ValueLst : NULL);
        //  }
        //else
        //  {
        //  GD_StrValue=PkData.Value()->GetString();
        //  PutWord(dwOptions, *PkData.Value(), NULL);
        //  }
        if (fSetShortcut)
          pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
        }
      else
        {
        if (debugging)
          dbgpln("-------------------- Not in Msg : %s", pSym[0]?pSym:pTag);
        }
      }
      break;
    };
  };

//---------------------------------------------------------------------------

void DataDefnBlk::DoDWord()
  {
  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (fWriting)
        m_pList->AdvanceWrite(/*pPItem*/);
      fWriting=True;

      PkDataUnion DataValue;
      DDEF_Flags XtraFlags=GetDWord(dwOptions, DataValue);
      m_pItem->AppendDataValue(/*m_pList->GetInfo(),*/ dwOptions, pTag, pSym, XtraFlags|CurrentFlags(), DataValue, iCnv, pCnvTxt);
      TRYAPPENDSTATUS;
      if (fStrList && pValueLst)
        m_pItem->AppendValueLst(/*m_pList->GetInfo(), */pValueLst, True);
      }
    case DDBTask_FndData:
      {
      if (fSetShortcut) // Set Shortcut to First Item
        pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
      fSetShortcut=False;
      }
      break;
    case DDBTask_PutData:
      {
      pDDBPkListItem pMsgItem=FindChildItem(lMsgItemLevel, pTag, pSym);
      if (pMsgItem)
        {
        CPkDataItem &PkData=*(pMsgItem->pItem);
        PutDWord(dwOptions, *PkData.Value());//, pValueLst);
        TRYSETSTATUS;
        //if (PkData.Type()!=tt_Generic)
        //  {
        //  flag ValuesExist=PkData.GetStrList(ValueLst);
        //  PutDWord(dwOptions, *PkData.Value(), ValuesExist ? &ValueLst : NULL);
        //  }
        //else
        //  {
        //  GD_StrValue=PkData.Value()->GetString();
        //  PutDWord(dwOptions, *PkData.Value(), NULL);
        //  }
        if (fSetShortcut)
          pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
        }
      else
        {
        if (debugging)
          dbgpln("-------------------- Not in Msg : %s", pSym[0]?pSym:pTag);
        }
      }
      break;
    };
  };

//---------------------------------------------------------------------------

void DataDefnBlk::DoInt()
  {
  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (fWriting)
        m_pList->AdvanceWrite(/*pPItem*/);
      fWriting=True;

      PkDataUnion DataValue;
      DDEF_Flags XtraFlags=GetInt(dwOptions, DataValue);
      m_pItem->AppendDataValue(/*m_pList->GetInfo(),*/ dwOptions, pTag, pSym, XtraFlags|CurrentFlags(), DataValue, iCnv, pCnvTxt);
      TRYAPPENDSTATUS;
      if (fStrList && pValueLst)
        m_pItem->AppendValueLst(/*m_pList->GetInfo(), */pValueLst, True);
      }
    case DDBTask_FndData:
      {
      if (fSetShortcut) // Set Shortcut to First Item
        pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
      fSetShortcut=False;
      }
      break;
    case DDBTask_PutData:
      {
      pDDBPkListItem pMsgItem=FindChildItem(lMsgItemLevel, pTag, pSym);
      if (pMsgItem)
        {
        CPkDataItem &PkData=*(pMsgItem->pItem);
        PutInt(dwOptions, *PkData.Value());//, pValueLst);
        TRYSETSTATUS;
        //if (PkData.Type()!=tt_Generic)
        //  {
        //  flag ValuesExist=PkData.GetStrList(ValueLst);
        //  PutInt(dwOptions, *PkData.Value(), ValuesExist ? &ValueLst : NULL);
        //  }
        //else
        //  {
        //  GD_StrValue=PkData.Value()->GetString();
        //  PutInt(dwOptions, *PkData.Value(), NULL);
        //  }
        if (fSetShortcut)
          pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
        }
      else
        {
        if (debugging)
          dbgpln("-------------------- Not in Msg : %s", pSym[0]?pSym:pTag);
        }
      }
      break;
    };
  };

//---------------------------------------------------------------------------

void DataDefnBlk::DoShort()
  {
  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (fWriting)
        m_pList->AdvanceWrite(/*pPItem*/);
      fWriting=True;

      PkDataUnion DataValue;
      DDEF_Flags XtraFlags=GetShort(dwOptions, DataValue);
      m_pItem->AppendDataValue(/*m_pList->GetInfo(),*/ dwOptions, pTag, pSym, XtraFlags|CurrentFlags(), DataValue, iCnv, pCnvTxt);
      TRYAPPENDSTATUS;
      if (fStrList && pValueLst)
        m_pItem->AppendValueLst(/*m_pList->GetInfo(), */pValueLst, True);
      }
    case DDBTask_FndData:
      {
      if (fSetShortcut) // Set Shortcut to First Item
        pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
      fSetShortcut=False;
      }
      break;
    case DDBTask_PutData:
      {
      pDDBPkListItem pMsgItem=FindChildItem(lMsgItemLevel, pTag, pSym);
      if (pMsgItem)
        {
        CPkDataItem &PkData=*(pMsgItem->pItem);
        PutShort(dwOptions, *PkData.Value());//, pValueLst);
        TRYSETSTATUS;
        //if (PkData.Type()!=tt_Generic)
        //  {
        //  flag ValuesExist=PkData.GetStrList(ValueLst);
        //  PutShort(dwOptions, *PkData.Value(), ValuesExist ? &ValueLst : NULL);
        //  }
        //else
        //  {
        //  GD_StrValue=PkData.Value()->GetString();
        //  PutShort(dwOptions, *PkData.Value(), NULL);
        //  }
        if (fSetShortcut)
          pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
        }
      else
        {
        if (debugging)
          dbgpln("-------------------- Not in Msg : %s", pSym[0]?pSym:pTag);
        }
      }
      break;
    };
  };

//---------------------------------------------------------------------------

void DataDefnBlk::DoLong()
  {
  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (fWriting)
        m_pList->AdvanceWrite(/*pPItem*/);
      fWriting=True;

      PkDataUnion DataValue;
      DDEF_Flags XtraFlags=GetLong(dwOptions, DataValue);
      m_pItem->AppendDataValue(/*m_pList->GetInfo(),*/ dwOptions, pTag, pSym, XtraFlags|CurrentFlags(), DataValue, iCnv, pCnvTxt);
      TRYAPPENDSTATUS;
      if (fStrList && pValueLst)
        m_pItem->AppendValueLst(/*m_pList->GetInfo(), */pValueLst, True);
      }
    case DDBTask_FndData:
      {
      if (fSetShortcut) // Set Shortcut to First Item
        pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
      fSetShortcut=False;
      }
      break;
    case DDBTask_PutData:
      {
      pDDBPkListItem pMsgItem=FindChildItem(lMsgItemLevel, pTag, pSym);
      if (pMsgItem)
        {
        CPkDataItem &PkData=*(pMsgItem->pItem);
        PutLong(dwOptions, *PkData.Value());//, pValueLst);
        TRYSETSTATUS;
        //if (PkData.Type()!=tt_Generic)
        //  {
        //  flag ValuesExist=PkData.GetStrList(ValueLst);
        //  PutLong(dwOptions, *PkData.Value(), ValuesExist ? &ValueLst : NULL);
        //  }
        //else
        //  {
        //  GD_StrValue=PkData.Value()->GetString();
        //  PutLong(dwOptions, *PkData.Value(), NULL);
        //  }
        if (fSetShortcut)
          pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
        }
      else
        {
        if (debugging)
          dbgpln("-------------------- Not in Msg : %s", pSym[0]?pSym:pTag);
        }
      }
      break;
    };
  };

//---------------------------------------------------------------------------

void DataDefnBlk::DoFloat()
  {
  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (fWriting)
        m_pList->AdvanceWrite(/*pPItem*/);
      fWriting=True;

      PkDataUnion DataValue;
      DDEF_Flags XtraFlags=GetFloat(dwOptions, DataValue);
      m_pItem->AppendDataValue(/*m_pList->GetInfo(),*/ dwOptions, pTag, pSym, XtraFlags|CurrentFlags(), DataValue, iCnv, pCnvTxt);
      TRYAPPENDSTATUS;
      }
    case DDBTask_FndData:
      {
      if (fSetShortcut) // Set Shortcut to First Item
        pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
      fSetShortcut=False;
      }
      break;
    case DDBTask_PutData:
      {
      pDDBPkListItem pMsgItem=FindChildItem(lMsgItemLevel, pTag, pSym);
      if (pMsgItem)
        {
        CPkDataItem &PkData=*(pMsgItem->pItem);
        PutFloat(dwOptions, *PkData.Value());
        TRYSETSTATUS;
        if (fSetShortcut)
          pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
        }
      else
        {
        if (debugging)
          dbgpln("-------------------- Not in Msg : %s", pSym[0]?pSym:pTag);
        }
      }
      break;
    };
  };

//---------------------------------------------------------------------------

void DataDefnBlk::DoDouble()
  {
  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (fWriting)
        m_pList->AdvanceWrite(/*pPItem*/);
      fWriting=True;

      PkDataUnion DataValue;
      DDEF_Flags XtraFlags=GetDouble(dwOptions, DataValue);
      m_pItem->AppendDataValue(/*m_pList->GetInfo(),*/ dwOptions, pTag, pSym, XtraFlags|CurrentFlags(), DataValue, iCnv, pCnvTxt);
      TRYAPPENDSTATUS;
      }
    case DDBTask_FndData:
      {
      if (fSetShortcut)// && (iFlags&0x40000000)==0) // Set Shortcut to First Item
        pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
      fSetShortcut=False;
      }
      break;
    case DDBTask_PutData:
      {
      pDDBPkListItem pMsgItem=FindChildItem(lMsgItemLevel, pTag, pSym);
      if (pMsgItem)
        {
        CPkDataItem &PkData=*(pMsgItem->pItem);
        PutDouble(dwOptions, *PkData.Value());
        TRYSETSTATUS;
        if (fSetShortcut)// && (iFlags&0x40000000)==0)
          pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
        }
      else
        {
        if (debugging)
          dbgpln("-------------------- Not in Msg : %s", pSym[0]?pSym:pTag);
        }
      }
      break;
    };
  };

//------------------------------------------------------------------------

void DataDefnBlk::DoString()
  {
  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (fWriting)
        m_pList->AdvanceWrite(/*pPItem*/);
      fWriting=True;

      PkDataUnion DataValue;
      DDEF_Flags XtraFlags=GetString(dwOptions, DataValue);
      m_pItem->AppendDataValue(/*m_pList->GetInfo(),*/ dwOptions, pTag, pSym, XtraFlags|CurrentFlags(), DataValue, iCnv, pCnvTxt);
      TRYAPPENDSTATUS;
      if (fStrList && pValueLst)
        m_pItem->AppendValueLst(/*m_pList->GetInfo(), */pValueLst, False);
      }
    case DDBTask_FndData:
      {
      if (fSetShortcut) // Set Shortcut to First Item
        pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
      fSetShortcut=False;
      }
      break;
    case DDBTask_PutData:
      {
      pDDBPkListItem pMsgItem=FindChildItem(lMsgItemLevel, pTag, pSym);
      if (pMsgItem)
        {
        CPkDataItem &PkData=*(pMsgItem->pItem);
        //flag ValuesExist=PkData.GetStrList(ValueLst);
        PutString(dwOptions, *PkData.Value());//, pValueLst);//ValuesExist ? &ValueLst : NULL);
        TRYSETSTATUS;
        if (fSetShortcut)
          pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
        }
      else
        {
        if (debugging)
          dbgpln("-------------------- Not in Msg : %s", pSym[0]?pSym:pTag);
        }
      }
      break;
    };

  }

//------------------------------------------------------------------------

void DataDefnBlk::DoBlob()
  {
  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (fWriting)
        m_pList->AdvanceWrite(/*pPItem*/);
      fWriting=True;

      PkDataUnion DataValue;
      DDEF_Flags XtraFlags=GetBlob(dwOptions, DataValue);
      m_pItem->AppendDataValue(/*m_pList->GetInfo(),*/ dwOptions, pTag, pSym, XtraFlags|CurrentFlags(), DataValue, iCnv, pCnvTxt);
      }
    case DDBTask_FndData:
      {
      if (fSetShortcut) // Set Shortcut to First Item
        pShortcut->Set(this, m_pFullItemTag, m_pRootObj->CurrentDataVersion());
      fSetShortcut=False;
      }
      break;
    case DDBTask_PutData:
      {
      pDDBPkListItem pMsgItem=FindChildItem(lMsgItemLevel, pTag, pSym);
      if (pMsgItem)
        {
        int xxx=0;
        }
      else
        {
        if (debugging)
          dbgpln("-------------------- Not in Msg : %s", pSym[0]?pSym:pTag);
        }
      }
      break;
    };

  }

//------------------------------------------------------------------------

void DataDefnBlk::ParseParms(DDBFnParms * ParmDefs)
  {
  char * pParms=Nest[lCallLevel].pSrchTag;
  if (!pParms)
    {
    m_nParms=0;
    return;
    }

  CSVColArray c;
  int Quote;
  char Buff[512];
  strcpy(Buff, pParms);
  m_nParms=ParseCSVFunction(Buff, c, Quote)-1;
  int i=0;
  while (ParmDefs->iType!=tt_NULL)
    {
    switch (ParmDefs->iType)
      {
      case tt_Double:
      case tt_Long:
        m_dParms[i]=SafeAtoF(i<m_nParms ? c[i+1]: "", ParmDefs->dDefault);
        if (ParmDefs->dMax>ParmDefs->dMin)
          m_dParms[i]=::Range(ParmDefs->dMin, m_dParms[i], ParmDefs->dMax);
        break;
      default:
        // Unhandled type
        DoBreak();
      }
    ParmDefs++;
    i++;
    }
  m_nParms=i;  // Update actual Parms
  };

//------------------------------------------------------------------------

void DataDefnBlk::Add_OnOff(LPSTR pTag, pflag pOn, TaggedObject *pOwn, dword Flags)
  {
  Bool("",    pTag,  DC_,    "",     pOn,     pOwn, Flags, DDBOnOff);
  }

//---------------------------------------------------------------------------

void DataDefnBlk::Add_YesNo(LPSTR pTag, pflag pYes, TaggedObject *pOwn, dword Flags)
  {
  Bool("",    pTag,  DC_,    "",     pYes,     pOwn, Flags, DDBYesNo);
  }

//---------------------------------------------------------------------------

void DataDefnBlk::Add_PropertyMap(CPropertyMap & Map, ULONG WhichProps, flag AsParms, long iBaseHandle, CPropertyBase * pPropBase, TaggedObject * pTgObj)
  {
  bool IsStruct   = false;
  bool IsObject   = false;
  bool DoingGlbl  = (WhichProps & MP_GlobalProp)!=0;

  DWORD ReqdVis=0xFFFFFFFF;
  CString SN;
  DataDefnBlk & DDB=*this;


  //dbgpln("====================================================================");
  //CPropertyMap &Map=PropertyMap();
  for (long i=0; i<Map.Count(); i++)
    {
    MPropertyInfo *I=Map[i];

    //dbgp("%3i %-20s %08x %-20s %-20s", i, I->m_sStruct, I->m_dwFlags, I->m_sTag, I->m_sSym);

    if (I && (WhichProps & I->m_dwFlags)!=0 && ((I->m_dwFlags&MP_NoAccess)==0))
      {
      if (SN.CompareNoCase(I->m_sStruct)!=0)
        {
        CString ObjTbl;
        if (DoingGlbl)
          ObjTbl.Format("%s_Glbl", pPropBase->DefinedPropertyMapName());
        else if (I->m_bIsObject)
          ObjTbl.Format("%s_%s", pPropBase->DefinedPropertyMapName(), I->m_sTableSuffix);
        else
          ObjTbl=pPropBase->DefinedPropertyMapName();

        if (IsObject)
          DDB.EndObject();
        if (IsStruct)
          DDB.EndStruct();
        bool DoAsObject;
        if (DoingGlbl)
          DoAsObject  = false;
        else
          DoAsObject  = I->m_bIsObject;
        if (DoAsObject)//I->m_bIsObject || (I->m_dwFlags & MP_GlobalProp)!=0)
          {
          DDB.BeginObject(pTgObj, (LPTSTR)(LPCTSTR)I->m_sStruct, (LPTSTR)(LPCTSTR)ObjTbl);//(LPTSTR)(LPCTSTR)pPropBase->DefinedPropertyMapName());
          IsObject=true;
          IsStruct=false;
          }
        else
          {
          DDB.BeginStruct(pTgObj, (LPTSTR)(LPCTSTR)I->m_sStruct);//, (LPTSTR)(LPCTSTR)ObjTbl);//(LPTSTR)(LPCTSTR)pPropBase->DefinedPropertyMapName());
          IsObject=false;
          IsStruct=true;
          }
        SN=I->m_sStruct;
        }

      //dbgp(" | %3s %3s %-20s", IsObject?"Obj":"", IsStruct?"Str":"", SN);

      DWORD Vis=pPropBase->GetPropertyVisibility(i);
      if (ReqdVis!=Vis)
        {
        ReqdVis=Vis;
        DDB.Visibility(((Vis&ePVis_Probal)?SM_Direct:0)|((Vis&ePVis_DynFlow)?SM_Inline:0)|((Vis&ePVis_DynFull)?SM_Buffered:0),
                       (Vis&ePVis_View)!=0,
                       (Vis&ePVis_File)!=0,
                       (Vis&ePVis_Snapshot)!=0,
                       (Vis&ePVis_Scenario)!=0);
        }

      DDEF_Flags Flags=0;
      if (AsParms)
        Flags|=(I->m_dwFlags&MP_Parameter) ? DDEF_PARAM : ((I->m_dwFlags&MP_ParamStopped)?DDEF_PARAMSTOPPED:DDEF_RESULT);
      else
        Flags|=DDEF_RESULT;
      Flags|=(I->m_dwFlags&MP_InitHidden)?DDEF_HIDDEN:0;
      Flags|=(I->m_dwFlags&MP_NanOK)?DDEF_NAN_OK:0;
      Flags|=(I->m_dwFlags&MP_NoView)?DDEF_NOVIEW:0;
      Flags|=(I->m_dwFlags&MP_NoFiling)?noFileAtAll:0;

      if (I->m_ePgOpt!=ePPg_Null)
        DDB.Page((LPSTR)(LPCSTR)I->m_sText, I->m_ePgOpt==ePPg_Optional?DDB_OptPage:DDB_RqdPage);
      if (I->m_sText.GetLength())
        DDB.Text((LPSTR)(LPCSTR)I->m_sText);

      switch (I->m_iType)
        {
        case ePT_Bool:
          if (I->m_dwFlags&MP_CheckBox)
            {
            DDB.CheckBoxBtn((LPTSTR)(LPCTSTR)I->m_sTag, (LPTSTR)(LPCTSTR)I->m_sSym, I->m_iCnv, (LPSTR)(LPCSTR)I->m_sCnv, iBaseHandle+i, pTgObj, Flags, (LPTSTR)(LPCTSTR)I->m_sDesc);
            }
          else if (I->m_StrValues.GetSize()>0)
            {
            DDBValueLstMem DDB1;
            for (int s=0; s<I->m_StrValues.GetSize(); s++)
              {
              MPropertyInfo::MStringValueS &V=I->m_StrValues[s];
              DDB1.Add(V.m_iValue, (LPSTR)(LPCSTR)V.m_sID, V.m_dwFlags);
              }
            DDB.Bool ((LPTSTR)(LPCTSTR)I->m_sTag, (LPTSTR)(LPCTSTR)I->m_sSym, I->m_iCnv, (LPSTR)(LPCSTR)I->m_sCnv,iBaseHandle+i, pTgObj, Flags, &DDB1, (LPTSTR)(LPCTSTR)I->m_sDesc);
            }
          else
            DDB.Bool ((LPTSTR)(LPCTSTR)I->m_sTag, (LPTSTR)(LPCTSTR)I->m_sSym, I->m_iCnv, (LPSTR)(LPCSTR)I->m_sCnv,iBaseHandle+i, pTgObj, Flags, (LPTSTR)(LPCTSTR)I->m_sDesc);
          break;
        case ePT_Long:
          if (I->m_StrValues.GetSize()>0)
            {
            DDBValueLstMem DDB1;
            for (int s=0; s<I->m_StrValues.GetSize(); s++)
              {
              MPropertyInfo::MStringValueS &V=I->m_StrValues[s];
              DDB1.Add(V.m_iValue, (LPSTR)(LPCSTR)V.m_sID, V.m_dwFlags);
              }
            DDB.Long  ((LPTSTR)(LPCTSTR)I->m_sTag, (LPTSTR)(LPCTSTR)I->m_sSym, I->m_iCnv, (LPSTR)(LPCSTR)I->m_sCnv,iBaseHandle+i, pTgObj, Flags, &DDB1, (LPTSTR)(LPCTSTR)I->m_sDesc);
            }
          else
            DDB.Long  ((LPTSTR)(LPCTSTR)I->m_sTag, (LPTSTR)(LPCTSTR)I->m_sSym, I->m_iCnv, (LPSTR)(LPCSTR)I->m_sCnv,iBaseHandle+i, pTgObj, Flags, (LPTSTR)(LPCTSTR)I->m_sDesc);
          break;
        //case ePT_ULong:   DDB.DWord ((LPTSTR)(LPCTSTR)I->m_sTag, (LPTSTR)(LPCTSTR)I->m_sSym, I->m_iCnv, (LPSTR)(LPCSTR)I->m_sCnv, iBaseHandle+i, pTgObj, Flags); break;
        case ePT_Double:
          DDB.Double((LPTSTR)(LPCTSTR)I->m_sTag, (LPTSTR)(LPCTSTR)I->m_sSym, I->m_iCnv, (LPSTR)(LPCSTR)I->m_sCnv, iBaseHandle+i, pTgObj, Flags, NULL, (LPTSTR)(LPCTSTR)I->m_sDesc);
          break;
        case ePT_String:
          if (I->m_StrValues.GetSize()>0)
            {
            DDBValueLstMem DDB1;
            for (int s=0; s<I->m_StrValues.GetSize(); s++)
              {
              MPropertyInfo::MStringValueS &V=I->m_StrValues[s];
              DDB1.Add(V.m_iValue, (LPSTR)(LPCSTR)V.m_sID, V.m_dwFlags);
              }
            DDB.String((LPTSTR)(LPCTSTR)I->m_sTag, (LPTSTR)(LPCTSTR)I->m_sSym, I->m_iCnv, (LPSTR)(LPCSTR)I->m_sCnv, iBaseHandle+i, pTgObj, Flags, &DDB1, (LPTSTR)(LPCTSTR)I->m_sDesc);
            }
          else
            DDB.String((LPTSTR)(LPCTSTR)I->m_sTag, (LPTSTR)(LPCTSTR)I->m_sSym, I->m_iCnv, (LPSTR)(LPCSTR)I->m_sCnv, iBaseHandle+i, pTgObj, Flags, (DDBValueLstMem*)NULL, (LPTSTR)(LPCTSTR)I->m_sDesc);
          break;
        }
      }
    //dbgpln("");
    }
  if (IsObject)
    DDB.EndObject();
  if (IsStruct)
    DDB.EndStruct();
  };

// ===========================================================================
//
//
//
// ===========================================================================
//
//flag DataItem::ValidateObjectData(TaggedObject *pRootObj)
//  {
//  flag OK=1;
//  pRootObj->FixObjectDefn(this);
//  if (IsStructure(iType))
//    {
//    if (IsObjectStart(iType))
//      OK = OK && m_pList->ValidateData(VDB);
//
//    pDataItem p=pNext;
//    while (p && (p->iLvl > iLvl))
//      {
//      if (IsStructure(p->iType))
//        OK = OK && p->ValidateObjectData(pRootObj);
//      p=p->pSibling;
//      }
//    }
//  return OK;
//  }
//
//============================================================================
//
//
//
//============================================================================

TagAccessBlk::TagAccessBlk()
  {
  //pTag = NULL;
  Ok=0;
  m_pPrevValues=NULL;
  m_PrevXRefStatus=-1;
  //iCnv=0;
  //iCnvOk=0;
  //pCnvTxt=NULL;
  };

//--------------------------------------------------------------------------

TagAccessBlk::TagAccessBlk(TaggedObject * pOwner, TaggedObject *pSrchRoot_, LPSTR Tag_, TABOptions Opts, TagUniqueness TU, flag AllowChangeTest/*=False*/)
  {
  //pTag = NULL;
  m_pPrevValues=NULL;
  m_PrevXRefStatus=-1;
  //Ok=0;
  //iCnv=0;
  //iCnvOk=0;
  //pCnvTxt=NULL;
  Init(pOwner, pSrchRoot_, Tag_, Opts, TU);
  };

//--------------------------------------------------------------------------

TagAccessBlk::~TagAccessBlk()
  {
  /*
  if (pTag)
    delete[] pTag;
  if (pCnvTxt)
    delete[] pCnvTxt;
  */
  if (m_pPrevValues)
    delete []m_pPrevValues;
  };

//--------------------------------------------------------------------------

TagAccessBlk& TagAccessBlk::operator=(const TagAccessBlk&X)
  {
  Ok               = X.Ok;
  bAllowChangeTest = X.bAllowChangeTest;
  bChanged         = X.bChanged;
  eUnique          = X.eUnique;
  m_pTagFound      = X.m_pTagFound;
  sTag             = X.sTag;
  sCnvTxt          = X.sCnvTxt;
  sCnvFam          = X.sCnvFam;
  pSrchRoot        = X.pSrchRoot;
  pAccObj          = X.pAccObj;
  dwOptions        = X.dwOptions;
  m_pPrevValues    = X.m_pPrevValues;
  m_lPrevValues    = X.m_lPrevValues;
  m_PrevXRefStatus = X.m_PrevXRefStatus;
  m_Shortcut       = X.m_Shortcut;
  return *this;
  };

//--------------------------------------------------------------------------

void TagAccessBlk::Init(TaggedObject * pOwner, TaggedObject *pSrchRoot_, LPSTR Tag_, TABOptions Opts, TagUniqueness TU, flag AllowChangeTest/*=False*/)
  {
  TaggedObject::SplitTagCnv(Tag_, sTag, sCnvTxt);
  m_pOwner=pOwner;
  pSrchRoot=pSrchRoot_;
  pAccObj=NULL;
  Ok=0;
  dwOptions=Opts;
  eUnique=TU;
  m_pTagFound=NULL;
  bAllowChangeTest=AllowChangeTest;
  bChanged=1;
  if (m_pPrevValues)
    delete [] m_pPrevValues;
  m_pPrevValues=NULL;
  m_lPrevValues=0;
  m_PrevXRefStatus=-1;
  m_Shortcut.fOK=False;
  };

//--------------------------------------------------------------------------

flag TagAccessBlk::LoadAddress(flag FndShortcut)
  {
  CStopWatch SW(true);
#define MyDBG 0
#if MyDBG
  static int DoDbg=01;
  int iDbg=0;
  DDBShortcutVersion SV;
  DWORD dwCV, dwGV;
  if (DoDbg)
    dbglock();
#endif
  //dbgpln("                         LoadAddress %s", pFullItemTag);

  bool ReMapTried=false;

  if (FndShortcut)
    {
    if (!m_Shortcut.fOK)
      {
#if MyDBG 
      if (DoDbg)                           
        iDbg=1;
#endif
      pAccObj=NULL;
      }
    else if (!m_Shortcut.TestShortcut(m_pTagFound, m_Shortcut.m_pRootObj/*pThis*/->CurrentDataVersion()))
      {
#if MyDBG 
      if (DoDbg)
        {
        iDbg=2;
        SV=m_Shortcut.DataVer;
        dwCV=m_Shortcut.m_pRootObj->CurrentDataVersion();
        dwGV=TaggedObject::dwGlblDataVerNo;
        }
#endif
      pAccObj=NULL;
      }
    }
#if MyDBG 
  else
    {
    if (DoDbg)
      iDbg=3;
    }
#endif

ReTry:
  if (pAccObj==NULL )
    {
    m_Shortcut.fOK=False;
    Ok=0;
    int MinTagLen=0;
    while (!Ok)
      {
      int ObjTagLen=0;
      pAccObj=pSrchRoot->FindObjTag(sTag(), eUnique==TU_IndividuallyUnique, ObjTagLen, MinTagLen);
      if (pAccObj==NULL)
        {
  //CNM       pAccObj=pSrchRoot;
        goto Failed;
        }
      m_pTagFound=sTag();//[ObjTagLen];
      if (FndShortcut)
        {
        long AllModes=pAccObj->AllModes();
        DDEF_Flags Opts=dwOptions|AllModes;
        Opts=Opts &~(noView|noFileAtAll); // Full Visibility;
        Ok=pAccObj->FndDefinedData(m_pTagFound, Opts|dwOptions|AllModes, &m_Shortcut);
        }
      else
        Ok=1;
      MinTagLen=ObjTagLen+1;
      }
    }

Failed:
  if (!Ok && !ReMapTried)
    {
    ReMapTried=true;
    // Try Remap ????
    COld2NewItem *pOld2New = FindAndFixOld2New(pSrchRoot, sTag(), sTag);
    if (pOld2New)
      {
      pAccObj=NULL;
      goto ReTry;
      }
    }
#if MyDBG 
  if (DoDbg)
    {
    switch (iDbg)                           
      {
      case 1: 
        dbgpln("LoadAddress - !OK ReFind                               %12.3f %s", SW.LapTime()*1e6, sTag());
        break;
      case 2:
        dbgpln("LoadAddress - Bad ReFind   %6i?%6i:%6i?%6i %12.3f %s",
          SV.dwDataVerObj, dwCV, SV.dwDataVerGlbl, dwGV, SW.LapTime()*1e6,
          sTag());
        break;
      case 3:
        dbgpln("LoadAddress - Find it                                  %12.3f %s", SW.LapTime()*1e6, sTag());
        break;
      }
    dbgunlock();
    }
#endif
  
  m_LoadAddress.Add(SW);
  return Ok;
  }

//--------------------------------------------------------------------------

flag TagAccessBlk::GetDefinedData(CPkDataList &DataObj, TABOptions Opts)
  {
  flag Ret=false;
  if (LoadAddress(true))//False))
    {
    CStopWatch SW(true);
    //CPkDataItem* pPItem=NULL;
    pAccObj->GetDefinedData(m_pTagFound, DataObj, Opts|dwOptions|pAccObj->AllModes(), /*pPItem,*/ &m_Shortcut);
    if (bAllowChangeTest && DataObj.GetCount()>0)
      {
      bChanged=0;

      ASSERT(DataObj.GetCount()<=1);

      CPkDataItem * pItem = DataObj.FirstItem();
      PkDataUnion * pData = pItem->Value();
      CTagRefStatus SW=pItem->GetTagRefStatus();
      dword Sz=pData->Size();
      if (Sz>m_lPrevValues)
        {
        delete [] m_pPrevValues;
        m_pPrevValues = new char[Sz];
        m_lPrevValues = Sz;
        bChanged=1;
        }
      else
        bChanged=(memcmp(m_pPrevValues, pData, Sz)!=0);
      memcpy(m_pPrevValues, pData, Sz);

      if (SW!=m_PrevXRefStatus)
        bChanged=1;
      m_PrevXRefStatus=SW;

      //  bChanged=(memcmp(pPrevValues, &DataObj, Sz)!=0);
      //memcpy(pPrevValues, &DataObj, Sz);
      }

    GotAllDataFromObject();
    Ret=True;
    m_GetDefined.Add(SW);
    }
  return Ret;
  }

//--------------------------------------------------------------------------

bool s_LogTABGets = false;
bool s_LogTABPuts = false;


int TagAccessBlk::PutDefinedData(CPkDataList &DataObj)
  {
  int Ret=TOData_NotFound;
  if (LoadAddress(true))//False))
    {
    CStopWatch SW(true);
    ValidateDataBlk VDB;
    CPkDataIter Iter;
    pAccObj->PutDefinedData(m_pTagFound, DataObj, Iter, dwOptions, &m_Shortcut);
    Ret=(pAccObj->ValidateDefinedData(VDB) ? TOData_OK : TOData_InvalidData);
    m_PutDefined.Add(SW);

    if (s_LogTABPuts)
      {
      dbgpln("TAB Set %-30s -> %s[%]", m_pOwner ? m_pOwner->Tag():"Unknown", sTag());
      }

    }
  return Ret;
  }

//--------------------------------------------------------------------------

flag TagAccessBlk::ValidateDefinedData()
  {
  flag Ret=False;
  ValidateDataBlk VDB;
  if (LoadAddress(False))
    {
    CStopWatch SW(true);
    Ret=pAccObj->ValidateDefinedData(VDB);
    m_ValidateDefined.Add(SW);
    }
  return Ret;
  }

//---------------------------------------------------------------------------

void TagAccessBlk::GotAllDataFromObject()
  {
  //if ((dwOptions & TABOpt_AllInfoOnce)==TABOpt_AllInfoOnce)
  if (dwOptions & TABOpt_AllInfoOnce)
    dwOptions = dwOptions & ~TABOpt_AllInfoOnce;
  };

//---------------------------------------------------------------------------

void TagAccessBlk::ClearDDBLists()
  {
  if (pAccObj)
    pAccObj->ClearDDBLists();
  }

//---------------------------------------------------------------------------

MTagIOValue TagAccessBlk::GetTagValue(LPSTR pRqdCnvText)
  {
  if (m_Shortcut.fOK)
    {
    PkDataUnion DataValue;
    m_Shortcut.GetValue(dwOptions, DataValue);
    if (!pRqdCnvText)
      pRqdCnvText=sCnvTxt();
    if (::IsIntData(DataValue.Type()))
      return MTagIOValue(DataValue.GetLong());
    if (::IsFloatData(DataValue.Type()))
      return MTagIOValue(DataValue.GetDouble(pRqdCnvText ? m_Shortcut.iCnv : 0, pRqdCnvText));
    if (::IsStrng(DataValue.Type()))
      return MTagIOValue(DataValue.GetString());
    //return DataValue.GetDouble(pRqdCnvText ? m_Shortcut.iCnv : 0, pRqdCnvText);
    }
  return MTagIOValue(0.0);
  };

int TagAccessBlk::PutTagValue(MTagIOValue V, LPSTR pRqdCnvText)
  {
  if (m_Shortcut.fOK)
    {
    if (!pRqdCnvText)
      pRqdCnvText=sCnvTxt();
    PkDataUnion DataValue;
    switch (V.TIOType)
      {
      case MTagType_Long:    DataValue.SetTypeLong(m_Shortcut.iType, V.Long); break;
      case MTagType_Double:  DataValue.SetTypeDouble(m_Shortcut.iType, V.DoubleSI, pRqdCnvText ? m_Shortcut.iCnv : 0, pRqdCnvText); break;
      case MTagType_String:  DataValue.SetTypeString(m_Shortcut.iType, (LPSTR)V.String); break;
      }
    //DataValue.SetTypeDouble(m_Shortcut.iType, D, pRqdCnvText ? m_Shortcut.iCnv : 0, pRqdCnvText);
    
    m_Shortcut.PutValue(dwOptions, DataValue);
    ValidateDataBlk VDB;

    return (pAccObj->ValidateDefinedData(VDB) ? TOData_OK : TOData_InvalidData);
    }
  return TOData_NotFound;
  };

double TagAccessBlk::GetDouble(char * pRqdCnvText)
  {
  if (m_Shortcut.fOK)
    {
    PkDataUnion DataValue;
    m_Shortcut.GetValue(dwOptions, DataValue);
    if (!pRqdCnvText)
      pRqdCnvText=sCnvTxt();
    return DataValue.GetDouble(pRqdCnvText ? m_Shortcut.iCnv : 0, pRqdCnvText);
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

long TagAccessBlk::GetLong()
  {
  if (m_Shortcut.fOK)
    {
    PkDataUnion DataValue;
    m_Shortcut.GetValue(dwOptions, DataValue);
    return DataValue.GetLong();
    }
  return 0L;
  };

//---------------------------------------------------------------------------

char* TagAccessBlk::GetString()
  {
  if (m_Shortcut.fOK)
    {
    PkDataUnion DataValue;
    m_Shortcut.GetValue(dwOptions, DataValue);
    return DataValue.GetString();
    }
  return "";
  }

//---------------------------------------------------------------------------

int TagAccessBlk::PutDouble(double D, char * pRqdCnvText)
  {
  if (m_Shortcut.fOK)
    {
    if (!pRqdCnvText)
      pRqdCnvText=sCnvTxt();
    PkDataUnion DataValue;
    DataValue.SetTypeDouble(m_Shortcut.iType, D, pRqdCnvText ? m_Shortcut.iCnv : 0, pRqdCnvText);
    m_Shortcut.PutValue(dwOptions, DataValue);
    ValidateDataBlk VDB;
    if (s_LogTABPuts)
      {
      dbgpln("TAB Set %-30s -> %s", m_pOwner ? m_pOwner->Tag():"Unknown", sTag());
      }
    return (pAccObj->ValidateDefinedData(VDB) ? TOData_OK : TOData_InvalidData);
    }
  return TOData_NotFound;
  };

//---------------------------------------------------------------------------

int TagAccessBlk::PutLong(long L)
  {
  if (m_Shortcut.fOK)
    {
    PkDataUnion DataValue;
    DataValue.SetTypeLong(m_Shortcut.iType, L);
    m_Shortcut.PutValue(dwOptions, DataValue);
    ValidateDataBlk VDB;
    if (s_LogTABPuts)
      {
      dbgpln("TAB Set %-30s -> %s", m_pOwner ? m_pOwner->Tag():"Unknown", sTag());
      }
    return (pAccObj->ValidateDefinedData(VDB) ? TOData_OK : TOData_InvalidData);
    }
  return TOData_NotFound;
  };

//---------------------------------------------------------------------------

int TagAccessBlk::PutString(char* p)
  {
  if (m_Shortcut.fOK)
    {
    PkDataUnion DataValue;
    DataValue.SetTypeString(m_Shortcut.iType, p);
    m_Shortcut.PutValue(dwOptions, DataValue);
    ValidateDataBlk VDB;
    if (s_LogTABPuts)
      {
      dbgpln("TAB Set %-30s -> %s", m_pOwner ? m_pOwner->Tag():"Unknown", sTag());
      }
    return (pAccObj->ValidateDefinedData(VDB) ? TOData_OK : TOData_InvalidData);
    }
  return TOData_NotFound;
  };

//========================================================================

TagAccessBlk::CTimes   TagAccessBlk::m_LoadAddress;
TagAccessBlk::CTimes   TagAccessBlk::m_GetDefined;
TagAccessBlk::CTimes   TagAccessBlk::m_PutDefined;
TagAccessBlk::CTimes   TagAccessBlk::m_ValidateDefined;

void TagAccessBlk::ResetTimers()
  {
  m_LoadAddress.Reset();
  m_GetDefined.Reset();
  m_PutDefined.Reset();
  m_ValidateDefined.Reset();
  };

void TagAccessBlk::CTimes::Reset()
  {
  m_Calls=0;
  m_Time=0;
  };

void TagAccessBlk::CTimes::Add(CStopWatch & SW)
  {
  static CCriticalSection C;
  C.Lock();
  m_Calls++;
  m_Time+=SW.Secs();
  C.Unlock();
  };

//========================================================================

void DataChangeBlk::DoBrowseBtn(char* ext, char* FileDesc, Strng& Path, Strng& File)
  {
  Strng Ext,s;
  Ext.Set("*.%s", ext ? ext : "*");
  s.Set("%s files (*.%s)|*.%s||", FileDesc, ext ? ext : "*", ext ? ext : "*");
  CSCDFileDialog Dlg(true, NULL, Ext(), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, s());
  Strng Fn;
  Fn = Path.Len() ? Path() : PrjFiles();
  Fn.FnExpand();
  Dlg.m_ofn.lpstrInitialDir = Fn();
  Dlg.m_ofn.lpstrTitle = FileDesc;
  HWND H=::GetActiveWindow();
  if (Dlg.DoModal()==IDOK)
    {
    Path=Dlg.GetPathName();
    Path.FnDrivePath();
    Path.FnContract();
    File=Dlg.GetFileName();
    }
  ::SetActiveWindow(H);
  }

//--------------------------------------------------------------------------

int DataChangeBlk::DoEditBtn(HANDLE& hProcess, DWORD& dwProcessId, char* Path, char* File, char* ext/*=NULL*/, Strng* pFullFilename/*=NULL*/, bool CheckExists/*=false*/, char* SubFolder/*=NULL*/)
  {
  int RetCode;
  if (File && strlen(File)>0)
    {
    DWORD ExitCode = 0;
    if (hProcess)
      GetExitCodeProcess(hProcess, &ExitCode);
    if (ExitCode==STILL_ACTIVE)
      {
      ActivateApp(dwProcessId);
      RetCode = 1; //OK: still active, activate editor
      }
    else
      {
      hProcess = NULL;
      Strng Fn(Path);
      Fn.FnExpand();
      Fn.FnCheckEndBSlash();
      Fn += File;
      if (ext)
        Fn.FnCheckExtension(ext);
      //Fn.FnSearchExpand(NULL, AF_PrjFiles/*AF_All|AF_BackupFiles*/, AF_PrjFiles); //Why would we do this!!!
      Fn.FnExpand();
      Strng Pth(Fn);
      Pth.FnDrivePath();
      if (Pth.Len()==0)
        {//if path isn't specified, assume project folder
        if (SubFolder)
          Pth=SubFolder;
        else
          Pth=PrjFiles();
        Pth+=Fn();
        Fn=Pth();
        }
      if (pFullFilename)
        (*pFullFilename)=Fn;
      bool OK=true;
      if (CheckExists)
        {
        if (!FileExists(Fn()))
          {
          RetCode = -1; //file does not exist
          OK=false;
          }
        }
      if (OK)
        {
        Strng Editor;
        if (ext && stricmp(ext, "rct")==0)
          Editor=RctEditCmdString(Fn());
        else
          Editor=TxtEditCmdString(Fn());
        STARTUPINFO si;
        memset(&si, 0, sizeof(si));
        si.cb = sizeof(si);
        si.wShowWindow = SW_SHOWDEFAULT;
        PROCESS_INFORMATION pi;
        if (CreateProcess(NULL, Editor(), NULL, NULL, FALSE, 0, NULL, PrjFiles(), &si, &pi))
          {
          RetCode = 0; //OK: open editor
          hProcess = pi.hProcess;
          dwProcessId = pi.dwProcessId;
          }
        else
          RetCode = -2; //cannot open editor
        }
      }
    }
  else
    RetCode = -3; //file not specified

  return RetCode;
  }

//========================================================================
//
//
//
//========================================================================

INLINE flag DDBDataInfo::PutDataBlocked(TABOptions Opts, DDEF_Flags Flags)
  {
  flag Parm=(Flags & DDEF_PARAM)!=0;
  if ((Flags & DDEF_PARAMSTOPPED) &&
      (TaggedObject::GetRunning() && !TaggedObject::GetBuildingAccess()))
    Parm=False;
  flag Btn=((Flags & DDEF_BUTTON) && !(Flags & DDEF_CHECKBOX));
  flag DoingAccess=((Opts & (TABOpt_ForFile|TABOpt_ForSnapShot|TABOpt_ForScenario))==0);

  if ((Flags & DDEF_PARAMCONSTRUCT) && ((Opts & (TABOpt_ForFile|TABOpt_ForViewConstruct))==0))
    {
    LogWarning(m_psFullItemTag?m_psFullItemTag->Str():"Unknown", 0, "Value not set - Only allowed during Construct");
    return 1;
    }
  //if ((Flags & DDEF_RESULT) && !(Flags & DDEF_PARAMCONSTRUCT))
  //  return 1; THIS CAUSES PROBLEMS BECAUSE DDEF_RESULT FIELDS DO NOT RECOVER FROM DATABASE!!!!!!!
  if (!Parm && DoingAccess)
    {
    if ((Opts & (TABOpt_ForViewConstruct|TABOpt_ForView))==0)
      LogWarning(m_psFullItemTag?m_psFullItemTag->Str():"Unknown", 0, "Value not set - Access Blocked");
    return 1;
    }
  if (Btn && !DoingAccess)
    return 1;
  if (Flags & DDEF_WRITEPROTECT)
    return 1;
  return 0;
  }

//========================================================================
//
//
//
//========================================================================

CCOMObjManagerRoot * CCOMObjManagerRoot::gs_Manager;


#if !DOINLINE
#include "TagObj.inl"
#endif

