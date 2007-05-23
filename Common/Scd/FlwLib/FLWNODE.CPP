//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992


#include "stdafx.h"
#define WITH_GFB
#include "sc_defs.h"
#include "sp_cont.h"
#include "sp_queue.h"
#define  __FLWNODE_CPP
#include "flwnode.h"
#include "dbgmngr.h"
#include "sfe_srvr.h"
#include "sp_react.h"

#include "flwnets.h"
#include "flwsolve.h"
#include "licbase.h"
#include "ndxrefs.h"

//#include "optoff.h"

#define dbgKwikConnect   0
#define dbgFlwNodeMS     WITHDEBUG
#define dbgFlowInfo      (WITHDEBUG && 0)

#if dbgFlwNode 
static CDbgMngr dbgAccCalcs         ("FlwNode",  "AccCalcs");
static CDbgMngr dbgBuildJoinList    ("FlwNode",  "BuildJoinList");
static CDbgMngr dbgConstruction     ("FlwNode",  "Construction");
static CDbgMngr dbgGlobalConnects   ("FlwNode",  "GlobalConnects");
static CDbgMngr dbgJoinsBld         ("FlwNode",  "JoinsBld");
static CDbgMngr dbgPropagateFlwModes("FlwNode",  "PropagateFlwModes");
static CDbgMngr dbgQRatio           ("FlwNode",  "QRatio");
static CDbgMngr dbgSensitivity      ("FlwNode",  "Sensitivity");
static CDbgMngr dbgTearOutputs      ("FlwNode",  "TearOutputs");
static CDbgMngr dbgTraceChanges     ("FlwNode",  "TraceChanges");

#if WithIOChgMonitor
static CDbgMngr dbgDumpChanges      ("FlwNode",  "DumpChanges");
static CDbgMngr dbgDumpChangesIn    ("FlwNode",  "DumpChangesIn");
static CDbgMngr dbgDumpChangeTol    ("FlwNode",  "DumpChangeTol");
#endif

static CDbgMngr dbgEvalJoinPress    ("ProBal",   "EvalJoinPress");
       CDbgMngr dbgTopologyChg      ("FlwNets",  "TopologyChg");
static CDbgMngr dbgChangeStats      ("FlwNets",  "ChangeStats");

//static CDbgMngr dbgExtendJoin      ("FlwNets",  "ExtendJoin");
#define DBGTOPOCHG(a) if (dbgTopologyChg()) dbgpln("Topo Change : %s",a);
#else
#define DBGTOPOCHG(a)
#endif

#if dbgFlwNodeMS
static CDbgMngr dbgMSSkipMerge    ("ProBal", "MS-SkipMerge");
static CDbgMngr dbgMSSkipSpcReduce("ProBal", "MS-SkipSpcReduction");
static CDbgMngr dbgMSMerge        ("ProBal", "MS-Merge");
static CDbgMngr dbgMSSetIndices   ("ProBal", "MS-SetIndices");
static CDbgMngr dbgMSDumpGamma    ("ProBal", "MS-DumpGamma");
static CDbgMngr dbgMSDumpResults  ("ProBal", "MS-DumpResults");
#endif

IMPLEMENT_MEMSTATS(QmRatioRec)
IMPLEMENT_MEMSTATS(GQmRatioRec)
IMPLEMENT_MEMSTATS(FlwBlk)
IMPLEMENT_MEMSTATS(CFlange)
IMPLEMENT_MEMSTATS(CMacroMdl)
IMPLEMENT_MEMSTATS(FlwNode)

//===========================================================================

//CStopWatch MallocSW;

//===========================================================================
//
//
//
//===========================================================================
double V2LLimit = 1.0e-12;

char *cJoinType[JTyp_NoOf+1] =
  {
  "???   ",
  "lon   ",
  "Edge  ",
  "Link  ",
  "Tie   ",
  "ImpNd ",
  "AccLnk",
  "Open  ",
  "fxLnk ",
  "fxTie ",
  "XfTie ",
  "UnDef ",
  "NoOf  "};

//===========================================================================
//
//
//
//===========================================================================

CIORec::CIORec()
  {
  m_iIODescSelf=0;
  m_iIODescRmt=0;
  m_iIONoRmt=0;
  Rmt=NULL;
  pFlng=NULL;
  //m_dwClass=0;
  m_pTerm=NULL;
  //m_pCData=NULL;
  //m_pEData=NULL;
  //m_pAData=NULL;

  iJoinId=IOPt_UnKnown; // InValid Value - will force the initial change
  iJoinTp=IOJn_UnKnown; // InValid Value - will force the initial change
  iJoinConnNo=-1;
  iClusterId=-1;
  fIsSetXfer=0;
  fIsBuffer=0;
  fIsComplete=0;
  fSameHgt=0;
  fDamped=0;
  fV2LReqd=0;
  fConnToRmt=0;
  fAutoConnect=0;
  fEvalSeqIn=0;
  fEvalSeqOut=0;
  fEvalSeqWasIn=0;
  fEvalSeqWasOut=0;
  m_eEvalSeqStart=ESS_Denied;

  m_pChgs=NULL;
  pFBs=NULL;
  pPBs=NULL;


  iHgtOrd=0;

  dFracHgt=0;
  dApertureAngle=0;
  dAperture=0;
  dEntrainRateA=0;
  dEntrainRateB=0;
  dEstV2LOnEntry=0;
  dEstL2VOnEntry=0;

  iExitRule=0;
  m_pImg=NULL;

  dV2LQmVFact=0;
  dV2LQmLFact=0;
  dRhoHSensV=0;
  dRhoHSensL=0;
  dPSensV=0;
  dPSensL=0;

  iJoinTp=0;
  iJoinId=0;
  iJoinConnNo=0;

  };

CIORec::~CIORec()
  {
  delete m_pImg;
  m_pImg=NULL;
  }

//--------------------------------------------------------------------------

CIORec & CIORec::operator=(CIORec &A)
  {
  m_iIODescSelf    = A.m_iIODescSelf;
  m_iIODescRmt     = A.m_iIODescRmt;
  m_iIONoRmt       = A.m_iIONoRmt;
  m_eEvalSeqStart  = A.m_eEvalSeqStart;
  m_pChgs          = A.m_pChgs;

  m_pTerm          = A.m_pTerm;

  Rmt              = A.Rmt;
  pFlng            = A.pFlng;
  pFBs             = A.pFBs;
  pPBs             = A.pPBs;

  iHgtOrd          = A.iHgtOrd;
  fSameHgt         = A.fSameHgt;
  fIsBuffer        = A.fIsBuffer;
  fIsSetXfer       = A.fIsSetXfer;
  fIsComplete      = A.fIsComplete;
  fDamped          = A.fDamped;
  fV2LReqd         = A.fV2LReqd;
  fConnToRmt       = A.fConnToRmt;
  fAutoConnect     = A.fAutoConnect;

  dFracHgt         = A.dFracHgt;
  dApertureAngle    = A.dApertureAngle;
  dAperture         = A.dAperture;
  dEntrainRateA     = A.dEntrainRateA;
  dEntrainRateB     = A.dEntrainRateB;
  dEstV2LOnEntry    = A.dEstV2LOnEntry;
  dEstL2VOnEntry    = A.dEstL2VOnEntry;


  dV2LQmVFact      = A.dV2LQmVFact;
  dV2LQmLFact      = A.dV2LQmLFact;
  dRhoHSensV       = A.dRhoHSensV;
  dRhoHSensL       = A.dRhoHSensL;
  dPSensV          = A.dPSensV;
  dPSensL          = A.dPSensL;

  iJoinTp          = A.iJoinTp;
  iJoinId          = A.iJoinId;
  iJoinConnNo      = A.iJoinConnNo;

  m_Srg            = A.m_Srg;

#if WithIOChgMonitor
  DataMem          = A.DataMem;
#endif

  // Transfer the Image if it exists
  iExitRule        = A.iExitRule;
  m_pImg           = A.m_pImg;
  A.m_pImg         = NULL;

  return *this;
  }

//--------------------------------------------------------------------------

void CIORec::SetNFBs(int n)
  {
  (*pFBs).SetSize(n);
  (*pPBs).SetSize(n);
  (*pFBs)[0].m_pNdPB=&(*pPBs)[0];
  (*pFBs)[0].m_pFlPB=&pFlng->PB;
  for (int i=1; i<n; i++)
    {
    (*pFBs)[i].m_pNdPB=&(*pPBs)[i];
    (*pFBs)[i].m_pFlPB=&(*pPBs)[i-1];
    }
  };

//============================================================================
//
//
//
//============================================================================

CFlwEqnGrp PipeGroup("PipeFlow", "FE_Darcy");
CFlwEqnGrp PipeFitGroup("PipeFitting", "PF_Line");
CFlwEqnGrp PipeEntryGroup("PipeEntryExit", "PE_KFact");
CFlwEqnGrp TeeEntryGroup("TeeEntry", "TE_KFact");
CFlwEqnGrp PumpGroup("Pump", "PC_FixHd");
CFlwEqnGrp ValveGroup("Valve", "VE_Crane");
CFlwEqnGrp ChkValveGroup("ChkValve", "VC_Crane");
CFlwEqnGrp OrificeGroup("Orifice", "OE_QvOp");
CFlwEqnGrp GasPumpGroup("GasPump", "CC_FixHd");
CFlwEqnGrp SizeChangeGroup("SizeChange", "PF_RedCv");//SC_Crane");
CFlwEqnGrp Leak2AreaGroup("Leak2Area", "LE_EquivDiam");
CFlwEqnGrp Vlv4PortGroup("Vlv4Port", "V4E_PDrop");

//==========================================================================
//
//
//
//==========================================================================

#define TWOIOSTD \
    {"Input",    "In",     IOId_2IOIn,     LIO_In0,  nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},  \
    {"Input",    "I",      IOId_2IOIn,     LIO_In,   nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},  \
    {"",         "Src",    IOId_2IOIn,     LIO_In,   nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},  \
    {"",         "S",      IOId_2IOIn,     LIO_In,   nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},  \
    {"Output",   "Out",    IOId_2IOOut,    LIO_Out0, nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},  \
    {"Output",   "O",      IOId_2IOOut,    LIO_Out,  nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},  \
    {"",         "Dst",    IOId_2IOOut,    LIO_Out,  nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},  \
    {"",         "D",      IOId_2IOOut,    LIO_Out,  nc_MLnk, 1,1, IOOptsHide|IOGRP(1)}

IOAreaRec TwoIOAreaList[] =
  {
    TWOIOSTD,
    {NULL}
  };
IOAreaRec TwoIOAreaListWithS[] =
  {
    TWOIOSTD,
    SPILL2AREA,
    //VENT2AREA,
    {NULL}
  };
IOAreaRec TwoIOAreaListWithL[] =
  {
    TWOIOSTD,
    LEAKS2AREA("LeaksI", IOId_2IOLeakI),
    LEAKS2AREA("LeaksO", IOId_2IOLeakO),
    {NULL}
  };
IOAreaRec TwoIOAreaListWithSL[] =
  {
    TWOIOSTD,
    SPILL2AREA,
    //VENT2AREA,
    LEAKS2AREA("LeaksI", IOId_2IOLeakI),
    LEAKS2AREA("LeaksO", IOId_2IOLeakO),
    {NULL}
  };
IOAreaRec TwoIOAreaListWithSV[] =
  {
    TWOIOSTD,
    SPILL2AREA,
    VENT2AREA,
    //LEAKS2AREA("LeaksI", IOId_2IOLeakI),
    //LEAKS2AREA("LeaksO", IOId_2IOLeakO),
    {NULL}
  };
IOAreaRec TwoIOAreaListWithBS[] =
  {
    TWOIOSTD,
    SPILL2AREA,
    BLEED2AREA(),
    {NULL}
  };
IOAreaRec TwoIOAreaListWithMBS[] =
  {
    TWOIOSTD,
    SPILL2AREA,
    MAKEUP2AREA(),
    BLEED2AREA(),
    {NULL}
  };

//===========================================================================
//
//
//
//===========================================================================

#define LOOP_THRU_QUALITYDEFS(pQC, pSQC)                           \
  for (pQC=TagObjClass::FindGroupId(SpQuality::GroupName);         \
       pSQC=dynamic_cast<SQTagObjClass *>(pQC), pQC!=NULL;         \
       pQC=pQC->NextClassInGrp())

#define LOOP_THRU_QUALITIES(iQ, pQC, pSQC)                         \
  for (iQ=0, pQC=TagObjClass::FindGroupId(SpQuality::GroupName);   \
       pSQC=dynamic_cast<SQTagObjClass *>(pQC), pQC!=NULL;         \
       iQ++, pQC=pQC->NextClassInGrp())

//===========================================================================
//
//
//
//===========================================================================

LPSTR FlwNode::Tag() 
  { 
  return CTNode::Tag(); 
  };

LPSTR FlwNode::SetTag(LPSTR ReqdTag, bool AllowEmptyTag) 
  { 
  CTNode::SetTag(ReqdTag, AllowEmptyTag); 
  //rename all connections
  // for all connects 
  //   connect[i]->FixTag()
  return CTNode::Tag(); 
  };

//--------------------------------------------------------------------------

#define DBGCONNECTS    0
#define DBGCONNECTMAP  0
#define DBGCONNECTLIST 0
#define DBGCONNECTADD  0

void FlwNode::AddToConnectList(CConnect * pConnect)
  {
  if (pConnect->m_ListPosn==NULL)
    {
    pConnect->m_ListPosn=sm_ConnectList.AddTail(pConnect);

#if DBGCONNECTS
    dbgp("ConnectList::Add   [%5i] @0x%08x %-20s ", sm_ConnectList.GetCount(), pConnect, pConnect->Tag());
#endif
    }
  else
    {
#if DBGCONNECTS
    dbgp("ConnectList::Exists[%5i] @0x%08x %-20s ", sm_ConnectList.GetCount(), pConnect, pConnect->Tag());
#endif
    }

#if DBGCONNECTS
  if (DBGCONNECTLIST)// || (FlwNode::sm_ConnectList.GetCount()<10))
    {
    dbgp(":");
    POSITION Pos=FlwNode::sm_ConnectList.GetHeadPosition();
    while (Pos)
      {
      CConnect *p=FlwNode::sm_ConnectList.GetNext(Pos);
#if DBGCONNECTADD  
      dbgp(", @0x%08x %s", p, p->Tag());
#else
      dbgp(", %s", p->Tag());
#endif
      }
    }
  dbgpln("");
#endif
  }

void FlwNode::RemoveFromConnectList(CConnect * pConnect)
  {
  if (pConnect->m_ListPosn!=NULL)
    {
    sm_ConnectList.RemoveAt(pConnect->m_ListPosn);
    pConnect->m_ListPosn=NULL;
#if DBGCONNECTS
    dbgp("ConnectList::Rem   [%5i] @0x%08x %-20s ", sm_ConnectList.GetCount(), pConnect, pConnect->Tag());
#endif
    }
  else
    {
#if DBGCONNECTS
    dbgp("ConnectList::Missng[%5i] @0x%08x %-20s ", sm_ConnectList.GetCount(), pConnect, pConnect->Tag());
#endif
    }

#if DBGCONNECTS
  if (DBGCONNECTLIST)// || (FlwNode::sm_ConnectList.GetCount()<10))
    {
    dbgp(":");
    POSITION Pos=FlwNode::sm_ConnectList.GetHeadPosition();
    while (Pos)
      {
      CConnect *p=FlwNode::sm_ConnectList.GetNext(Pos);
#if DBGCONNECTADD  
      dbgp(", @0x%08x %s", p, p->Tag());
#else
      dbgp(", %s", p->Tag());
#endif
      }
    }
  dbgpln("");
#endif
  }; 

void FlwNode::AddToConnectMap(LPSTR ConnTag, CConnect * pConnect)
  {
  if (ConnTag && strlen(ConnTag)>0 && ConnTag[0]!='?')
    {
#if DBGCONNECTMAP
    if (sm_ConnectMap.GetHashTableSize()<1000)
      {
      dbgpln("ConnectMap ::INITHASHTABLE");
      //sm_Connects.SetSize(0, 256);
      sm_ConnectMap.InitHashTable(5001);
      }
#endif

    sm_ConnectMap.SetAt(ConnTag, pConnect);

#if DBGCONNECTMAP
    CConnectMap::CPair *pX=sm_ConnectMap.PLookup(ConnTag);
    dbgp("ConnectMap ::Add   [%5i] @0x%08x %-20s ", sm_ConnectMap.GetCount()-1, pX, ConnTag);
    //  if (DBGCONNECTLIST || (FlwNode::sm_ConnectMap.GetCount()<10))
    //    {
    ////    dbgp(":");
    ////    CConnectMap::CPair *pX=FlwNode::sm_ConnectMap.PGetFirstAssoc();
    ////    while (pX)
    ////      {
    ////#if DBGCONNECTADD  
    ////      dbgp(", @0x%08x %s", pX, pX->key);
    ////#else
    ////      dbgp(", %s", pX->key);
    ////#endif
    ////      pX=FlwNode::sm_ConnectMap.PGetNextAssoc(pX);
    ////      }
    //    }
    dbgpln("");
#endif
    }
  }; 

void FlwNode::RemoveFromConnectMap(LPSTR ConnTag)
  {
  if (ConnTag && strlen(ConnTag)>0 && ConnTag[0]!='?')
    {
#if DBGCONNECTMAP
    CConnectMap::CPair *pX=sm_ConnectMap.PLookup(ConnTag);
    dbgp("ConnectMap ::Rem   [%5i] @0x%08x %-20s ", sm_ConnectMap.GetCount()-1, pX, ConnTag);
#endif

    sm_ConnectMap.RemoveKey(ConnTag);

#if DBGCONNECTMAP
    //    if (DBGCONNECTLIST || (FlwNode::sm_ConnectMap.GetCount()<10))
    //      {
    //      dbgp(":");
    //      CConnectMap::CPair *pX=FlwNode::sm_ConnectMap.PGetFirstAssoc();
    //      while (pX)
    //        {
    //#if DBGCONNECTADD  
    //        dbgp(", @0x%08x %s", pX, pX->key);
    //#else
    //        dbgp(", %s", pX->key);
    //#endif
    //        pX=FlwNode::sm_ConnectMap.PGetNextAssoc(pX);
    //        }
    //      }
    dbgpln("");
#endif
    }
  }; 

//--------------------------------------------------------------------------

CConnect * FlwNode::FindConnect(pchar pSrchTag, flag SrchAll, int &ObjTagLen, int MinTagLen)
  {
  if (pSrchTag==NULL)
    return NULL;

  ObjTagLen=0;
  int P1=0,L=0;
  CConnect * pRet=NULL;
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
    CConnect * p;
    if (sm_ConnectMap.Lookup(SrchTag, p))
      {
      // Move down a level and search it
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
  };

//===========================================================================
//
//
//
//===========================================================================

CConnectCodeLock ConnectCodeLock;
IMPLEMENT_MEMSTATS(CConnect);

CConnect::CConnect(dword ID, pTagObjClass pClass_, char * TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
TaggedObject(pClass_, TagIn, pAttach, eAttach)
  {
  InitializeCriticalSection(&m_LockSect);

  m_dwID=ID;
  m_fIsEvalSeqStart=false;

  m_pNdSrc=NULL;
  m_pNdDst=NULL;
  m_iIOSrc=0;
  m_iIODst=0;

  m_bTagFixed=false;
  m_ListPosn=NULL;
  FlwNode::AddToConnectList(this);
  LPSTR pTag=Tag();
  if (pTag && strlen(pTag)>0)
    FlwNode::AddToConnectMap(Tag(), this); 
  };

// --------------------------------------------------------------------------

CConnect::~CConnect()
  {
  FlwNode::RemoveFromConnectMap(Tag()); 
  FlwNode::RemoveFromConnectList(this);
  ClrConnection(); 
  DeleteCriticalSection(&m_LockSect);
  };
// --------------------------------------------------------------------------

void  CConnect::ClrConnection() 
  { 
  m_pNdSrc=m_pNdDst=NULL; 
  if (!m_bTagFixed)
    {
    if (Tag())
      FlwNode::RemoveFromConnectMap(Tag()); 
    SetTag("", true);
    }
  };

// --------------------------------------------------------------------------

void  CConnect::SetConnection(const IOInfo & IOSrc, const IOInfo & IODst)
  {
  m_pNdSrc=IOSrc.p; 
  m_iIOSrc=IOSrc.IOIndex; 
  m_iIdSrc=IOSrc.Id; 
  m_pNdDst=IODst.p; 
  m_iIODst=IODst.IOIndex; 
  m_iIdDst=IODst.Id; 
  FixTag();
  }

// --------------------------------------------------------------------------

void CConnect::FixTag(LPCSTR ReqdTag, bool TagFixed)
  {
  Strng NewTag(Tag());
  
  if (ReqdTag && strlen(ReqdTag)>0)
    {
    NewTag=ReqdTag;
    m_bTagFixed=TagFixed;
    }

  if (!m_bTagFixed)
    {
    NewTag=m_sRefIOTg();
    }

  if (NewTag.XStrICmp(Tag())!=0)
    {
    if (Tag())
      FlwNode::RemoveFromConnectMap(Tag()); 
    CConnect::SetTag(NewTag());
    if (Tag())
      FlwNode::AddToConnectMap(CConnect::Tag(), this); 
    }
  
  };

// --------------------------------------------------------------------------

static inline bool TestEqual(dword Type, dword Mask)   {   return ((Type & Mask) == Mask); }
static inline long ExtractIndexFromTag(LPSTR Tag)
  {
  int iLen=strlen(Tag);
  LPCSTR pOpen=strchr(Tag, '<');
  if (pOpen)
    {
    LPCSTR pClose=pOpen+1;
    while (((pClose-Tag)<iLen) && (*pClose!='>') && isdigit(*pClose))
      pClose++;
    if (*pClose=='>')
      return SafeAtoL((pOpen+1), 0);
    }
  return 0;
  }

// --------------------------------------------------------------------------

#define dbgInitConnId 0

void CConnect::InitConnId(FlwNode * pNd, LPSTR IOName, FlwNode * pRmtNd, LPSTR IONameRmt, LPSTR DIOName)
  {
  FlwNode * p=NULL;
  if (pRmtNd)
    {
    dword LclType = pNd->m_dwType;
    dword RmtType = pRmtNd->m_dwType;
#if dbgInitConnId
    dbgp("InitConnId: 0x%08x %s 0x%08x > %s 0x%08x", this, pNd->Tag(), LclType, pRmtNd->Tag(), RmtType);
    if ((stricmp(pNd->Tag(), "P_1")==0) && (stricmp(pRmtNd->Tag(), "PlantARea")==0))
      { int xxx=0; };
#endif
    if (TestEqual(LclType, nc_Process) || TestEqual(LclType, nc_MXfer))
      p=pNd;
    else if (TestEqual(RmtType, nc_Process) || TestEqual(RmtType, nc_MXfer))
      p=pRmtNd;
    else if (TestEqual(LclType, nc_MSrcSnk) && !TestEqual(LclType, nc_Audit))
      p=pNd;
    else if (TestEqual(RmtType, nc_MSrcSnk) && !TestEqual(RmtType, nc_Audit))
      p=pRmtNd;
    else if (TestEqual(LclType, nc_MBst))
      p=pNd;
    else if (TestEqual(RmtType, nc_MBst))
      p=pRmtNd;
    else if (TestEqual(LclType, nc_MLnk))
      p=pNd;
    else if (TestEqual(RmtType, nc_MLnk))
      p=pRmtNd;    
    else if (TestEqual(LclType, nc_MSrcSnk|nc_MkupBled))
      p=pNd;
    else if (TestEqual(RmtType, nc_MSrcSnk|nc_MkupBled))
      p=pRmtNd;
    else if (TestEqual(LclType, nc_MSrcSnk|nc_Audit))
      p=pRmtNd; // Other End
    else if (TestEqual(RmtType, nc_MSrcSnk|nc_Audit))
      p=pNd;    // Other End
    else
      p=pNd;
    }
  else
    {
#if dbgInitConnId
    dbgp("InitConnId: 0x%08x %s", this, pNd->Tag());
#endif
    p=pNd;
    }  

#if dbgInitConnId
  dbgp("  [%s] ", p->Tag());
#endif

  long iNext=0;
  // Must use m_IOs - NoFlwIOs does not return the correct values until all connections are made
  for (int i=0; i<p->m_IOs.GetCount(); i++)
    {
    CConnect *pC=p->IOConnect(i);
    if (pC)
      {
      LPSTR Tag=p->IOConnect(i)->m_sRefIOTg();
      if (Tag)
        iNext=Max(iNext, ExtractIndexFromTag(Tag));
      }
    }
  for (int i=0; i<p->m_AttachedDirectIOs.GetCount(); i++)
    {
    CDirectFlwIO *pDIO=p->m_AttachedDirectIOs[i];
    LPSTR pTag=NULL;
    if (pDIO->HasConduit)
      pTag=pDIO->Flange.FullObjTag();
    else
      pTag=pDIO->m_sConnTagMem();
    if (pTag)
      iNext=Max(iNext, ExtractIndexFromTag(pTag));
    }
  iNext++;

  if (DIOName)
    m_sRefIOTg.Set("%s<%i>%s",p->FullObjTag(), iNext, DIOName);
  else
    {
    m_sRefIOTg.Set("%s<%i>%s",p->FullObjTag(), iNext, p==pNd ? IOName : IONameRmt);
    }
#if dbgInitConnId
  dbgpln("    = %s", m_sRefIOTg());
#endif
  };

// --------------------------------------------------------------------------

void CConnect::AdjustConnection(FlwNode* This, int By)
  {
  if (m_pNdSrc==This)
    m_iIOSrc=m_iIOSrc+By;
  else
    m_iIODst=m_iIODst+By; 
  };

// --------------------------------------------------------------------------

void CConnect::BuildDataDefn(DataDefnBlk & DDB)
  {
  //TaggedObject::BuildDataDefn(DDB);
  };
flag CConnect::DataXchg(DataChangeBlk & DCB)
  {
  return TaggedObject::DataXchg(DCB);
  };
flag CConnect::ValidateData(ValidateDataBlk & VDB)
  {
  return TaggedObject::ValidateData(VDB);
  };

//===========================================================================
//
//
//
//===========================================================================

CTerminal::CTerminal(TagObjClass * pClass_, char * TagIn, TaggedObject * pAttach, TagObjAttachment eAttach) : \
CConnect(Connect_Terminal, pClass_, TagIn, pAttach, TOA_Free)
  {
  m_Attachments=0;
  m_fInTerminalList=false;
  m_fIsTear=false;
  };

// --------------------------------------------------------------------------

CTerminal::~CTerminal()
  {
  };

//===========================================================================
//
//
//
//===========================================================================

CCtrlData::CCtrlData()
  {
  m_dValue=0;
  };

//===========================================================================
//
//
//
//===========================================================================

#if KeepOldTearVarBlkEdit
IMPLEMENT_TAGOBJEDTDERIVED(FlangeTearVarBlk, "FlangeTVB", "TbFlange", "", "TBFG", TOC_SYSTEM,
                   TearVarBlkEdt,
                   "FlangeTearVarBlk", "Flange Tear Variable Calculation Blk",
                   TearVarBlkClassDef);
#else
IMPLEMENT_TAGOBJDERIVED(FlangeTearVarBlk, "FlangeTVB", "TbFlange", "", "TBFG", TOC_SYSTEM,
                   "FlangeTearVarBlk", "Flange Tear Variable Calculation Blk",
                   TearVarBlkClassDef);
#endif

FlangeTearVarBlk::FlangeTearVarBlk(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TearVarBlk(pClass_, Tag_, pAttach, eAttach)
    {
    DoConstruct();
    };

// --------------------------------------------------------------------------

FlangeTearVarBlk::FlangeTearVarBlk(pchar Tag_, pTaggedObject pAttach) :
  TearVarBlk(&FlangeTearVarBlkClass, Tag_, pAttach, TOA_Free)
    {
    DoConstruct();
    };

// --------------------------------------------------------------------------

FlangeTearVarBlk::~FlangeTearVarBlk()
  {
  };

// --------------------------------------------------------------------------

void FlangeTearVarBlk::DoConstruct()
  {
  PCnv.Set(DC_P, "kPa");
  TCnv.Set(DC_T, "C");
  QCnv.Set(DC_Qm, "kg/s");
  PFmt.Set("", 0, 2, 'f');
  TFmt.Set("", 0, 2, 'f');
  QFmt.Set("", 0, 2, 'f');
  iTearTagTyp=TTT_Tear;
  sGroupTag="Flange";
  int NQ=0;

  TagObjClass *pQC;
  SQTagObjClass * pSQC;
  LOOP_THRU_QUALITYDEFS(pQC, pSQC)
    if (pSQC)
      NQ+=pSQC->NTearVariables();

  //CHECK Is all the tear code correct for attributes
  SetNVariables(SVSpcCount()+SVAttCount()+2+NQ, TVU_Inputs|TVU_Outputs|TVU_Meas|TVU_AdvanceVars|TVU_TestVars);//, TVT_Tear);
  int n=0;
  TV[n].Tag.Set("Temp");
  TV[n].Sym.Set("T");
  TV[n].AdvCnv=&TCnv;
  TV[n].AdvFmt=&TFmt;
  TV[n].MeasCnv=&TCnv;
  TV[n].MeasFmt=&TFmt;
  n++;
  TV[n].Tag.Set("Press");
  TV[n].Sym.Set("P");
  TV[n].AdvCnv=&PCnv;
  TV[n].AdvFmt=&PFmt;
  TV[n].MeasCnv=&PCnv;
  TV[n].MeasFmt=&PFmt;
  n++;
  for (int s=0; s<SVSpcCount(); s++)
    {
    TV[n].Tag=SVImg(s).Tag();
    TV[n].Sym=SVImg(s).SymOrTag();
    TV[n].AdvCnv=&QCnv;
    TV[n].AdvFmt=&QFmt;
    TV[n].MeasCnv=&QCnv;
    TV[n].MeasFmt=&QFmt;
    n++;
    }
  for (  ; s<SVValueCount(); s++)
    {
    TV[n].Tag=SVImg(s).Tag();
    TV[n].Sym=SVImg(s).SymOrTag();
    //TODO Fix this Attribute Cnvs
    TV[n].AdvCnv=&QCnv;
    TV[n].AdvFmt=&QFmt;
    TV[n].MeasCnv=&QCnv;
    TV[n].MeasFmt=&QFmt;
    n++;
    }

  LOOP_THRU_QUALITYDEFS(pQC, pSQC)
    if (/*pQC->Selectable() &&*/ pSQC)
      n+=pSQC->DefineTearVariables(TV, n);

  };

// --------------------------------------------------------------------------

void FlangeTearVarBlk::OnEditConstruct()
  {
  ObjectAttribute *pAttr=ObjAttributes.FindObject("FlangeTearEdt");
  if (pAttr)
    {
    pAttr->FieldFmtCnvs("Temp",  TFmt, TCnv);
    pAttr->FieldFmtCnvs("Press", PFmt, PCnv);
    pAttr->FieldFmtCnvs("Qm",    QFmt, QCnv);
    }
  };

// --------------------------------------------------------------------------

void FlangeTearVarBlk::OnEditDestroy()
  {
  ObjectAttribute *pAttr=ObjAttributes.FindObject("FlangeTearEdt");
  if (pAttr)
    {
    pAttr->SetFieldFmt("Press", PFmt);
    pAttr->SetFieldCnvs("Press", PCnv);
    pAttr->SetFieldFmt("Temp", TFmt);
    pAttr->SetFieldCnvs("Temp", TCnv);
    pAttr->SetFieldFmt("Qm", QFmt);
    pAttr->SetFieldCnvs("Qm", QCnv);
    }
  };

//===========================================================================
//
//
//
//===========================================================================

byte CFlange::iMkUpOKCurr=0;
byte CFlange::iMkUpOKNext=0;

IMPLEMENT_TAGOBJ(CFlange, "Flange", "Flange", "", "DirectIO", "Flng", TOC_SYSTEM, "FlangeGroup", "")

CFlange::CFlange(pTagObjClass pClass_, char * TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) : \
TearPosBlk(TT_NoTear),
CConnect(Connect_Flange, pClass_, TagIn, pAttach, eAttach)
  {
  //InitializeCriticalSection(&ConnectLockSect);
  m_Attachments=0;
  m_bMustDelete=false;//MustDelete;

  m_Datum=0.0;
  m_DeltaZ=0.0;
  //fFlngSetUp=false;
  fIsBufferPos=false;
  fIsBuffered=false;
  fInFlangeList=false;
  //fIsEvalSeqStart=false;

  pQIO = new SpConduit("Qio", NULL, TOA_Free);
  pQIn = NULL;
  pQMem= NULL;

  pPMU = NULL;
  bPMUOpts=0;

  dMkUpRqd=dNAN;
  dMkUpRqdLcl=dNAN;
  dMkUpAvl=dNAN;
  iMkUpRqdOK=0;
  iMkUpRqdLclOK=0;
  iMkUpAvlOK=0;

//  dV2LFrac=dNAN;

  m_pTearImg=NULL;

#if DOPB_FILT_RESULTS
  for (int i=0; i<MaxMSRqd; i++)
    pMSRqd[i]=NULL;
  nMSRqd=0;
#endif
  };

// --------------------------------------------------------------------------

CFlange::~CFlange()
  {
  delete pQIO;
  delete pQIn;
  delete pQMem;
  delete pPMU;

  delete m_pTearImg;

#if DOPB_FILT_RESULTS
  for (int i=0; i<MaxMSRqd; i++)
    delete pMSRqd[i];
#endif

  //DeleteCriticalSection(&ConnectLockSect);
  };

// --------------------------------------------------------------------------

LPSTR CFlange::Tag()
  {
  return CConnect::Tag();
  };

LPSTR CFlange::SetTag(LPSTR ReqdTag, bool AllowEmptyTag)
  {
  if (ReqdTag)
    FixTag(ReqdTag, true);

  return CConnect::SetTag(ReqdTag, AllowEmptyTag);
  };

// --------------------------------------------------------------------------

void CFlange::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  //DDB.BeginStruct(this);

  DDB.Text("");
  DDB.String  ("SrcTag",            "",       DC_,      "",   xidFlngSrcTag,   this, isResult|isTag);
  DDB.String  ("DstTag",            "",       DC_,      "",   xidFlngDstTag,   this, isResult|isTag);
  DDB.Text("");

  DDB.Object(ConduitIO(), this, "Qo"); //Name this 'Qo' instead of ('Q') so that it is same as pipe.Qo (Useful for mixed report of streams and flange models)
  //DDB.EndStruct();
  DDB.EndStruct();
  };

// --------------------------------------------------------------------------

flag CFlange::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {    
    case xidFlngSrcTag: 
      DCB.pC=m_pNdSrc ? m_pNdSrc->FullObjTag():""; 
      return 1;
    case xidFlngDstTag: 
      DCB.pC=m_pNdDst ? m_pNdDst->FullObjTag():""; 
      return 1;
    }

  if (ConduitIO()->DataXchg(DCB))
    return 1;
  return 0;
  };

// --------------------------------------------------------------------------

flag CFlange::ValidateData(ValidateDataBlk & VDB)
  {
  if (!ConduitIO()->ValidateData(VDB))
    return false;
  return true;
  };

// --------------------------------------------------------------------------

void CFlange::SetTearType(byte Type, FlwNode * pNd)
  {
  TearPosBlk::SetTearType(Type);
  ASSERT(m_pNdSrc);
  ASSERT(m_pNdDst);

  if (!fIsBuffered && IsTear() && pQIn==NULL)
    {
    pQIn = new SpConduit("Qx", pNd, TOA_Embedded);
    pQIn->QCopy(*pQIO);
    TaggedObject::StructureChanged(pQIn);
    TaggedObject::StructureChanged(m_pNdSrc);
    TaggedObject::StructureChanged(m_pNdDst);
    }
  else if (!IsTear() && pQIn!=NULL)
    {
    delete pQIn;
    pQIn=NULL;
    TaggedObject::StructureChanged(m_pNdSrc);
    TaggedObject::StructureChanged(m_pNdDst);
    };
  };

// --------------------------------------------------------------------------

void CFlange::SetTearImage(FlwNode * pNd)
  {
  if (m_pTearImg==NULL)
    {
    m_pTearImg=new SpImage("TImg", pNd, TOA_Embedded);
    pNd->StructureChanged(pNd);
    m_pTearImg->SetMassInUseOK(true); // Allow next line to work
    m_pTearImg->SetMassInUse(false);
    m_pTearImg->SetMassInUseOK(false);
    };
  };

// --------------------------------------------------------------------------

void CFlange::ClrTearImage(FlwNode * pNd)
  {
  if (m_pTearImg)
    {
    delete m_pTearImg;
    m_pTearImg=NULL;
    pNd->StructureChanged(pNd);
    };
  };

// --------------------------------------------------------------------------

TearVarBlk* CFlange::CreateVarBlk(char * Tag, pTaggedObject pAttach)
  {
  return new FlangeTearVarBlk(Tag, pAttach);
  };

// --------------------------------------------------------------------------

byte CFlange::TearVarType()
  {
  return (!fIsBuffered && IsTear()) ? TVT_Tear|TVT_Test : TVT_None;
  };
//flag CFlange::TearAdvanceReqd()
//  {
//  return !fIsBuffered && IsTear();
//  };
//
//// --------------------------------------------------------------------------
//
//flag CFlange::TearCnvTestReqd()
//  {
//  return !fIsBuffered && IsTear();
//  };

// --------------------------------------------------------------------------

void CFlange::SetTearInitEstUsage(byte iEu)
  {
  TearPosBlk::SetTearInitEstUsage(iEu);
  if (TearImage())
    TearImage()->SetMassInUseOK(TearInitEstUsage()==TIEU_PartEst);
  };

// --------------------------------------------------------------------------

void CFlange::TearInitialiseOutputs(double EstimatePortion)
  {
  if (fIsBuffered)
    return;

  #if dbgFlwNode
  SpVector dbgm;
  if (dbgTearOutputs())
    dbgpln("================================================ TearInitialiseOutputs %10.3f", EstimatePortion);
  #endif

  // See Method Conduit for Direction Info
  if (TearImageExists() && (TearType()>=TT_ManualTear) && EstimatePortion>=0.0)
    {
    m_pVarBlk->SetHoldDamping(true);
    SpModel * QIOMdl=pQIO->Model();
    SpModel * QInMdl=pQIn->Model();
    SpModel * ImgMdl=TearImage()->Model();
    CSysVector & QIOM =QIOMdl->MArray();
    CSysVector & QInM =QInMdl->MArray();
    CSysVector & ImgM =ImgMdl->MArray();
    CSpUseArray  & MassInUse = TearImage()->Model()->m_MassInUse;
    bool MassInUseOK = TearImage()->Model()->m_bMassInUseOK;
    #if dbgFlwNode
    dbgm=QIOM;
    #endif
    byte Use=TearInitEstUsage();

    EstimatePortion=Min(EstimatePortion, 1.0);

    double TRqd=TearImage()->TRqd();
    double TIn=pQIn->Temp();
    double T2Set=TIn;
    if (Valid(TRqd))
      T2Set=EstimatePortion*TRqd+(1.0-EstimatePortion)*TIn;
    else
      T2Set=TIn;

    double PRqd=TearImage()->PRqd();
    double PIn=pQIn->Press();
    double P2Set=PIn;
    if (Valid(PRqd))
      P2Set=EstimatePortion*PRqd+(1.0-EstimatePortion)*PIn;
    else
      P2Set=PIn;

    #if dbgFlwNode
    if (dbgTearOutputs())
      {
      dbgpln("%15s %15s %15s >> %15s %s", "In", "Img", "IO", "Out", "");
      dbgpln("%15.4f %15.4f %15.4f >> %15.4f %s", K2C(TIn), K2C(TRqd), K2C(QIOMdl->Temp()), K2C(T2Set), "Temp");
      dbgpln("%15.4f %15.4f %15.4f >> %15.4f %s", PIn-AtmosPress(), PRqd-AtmosPress(), QIOMdl->Press()-AtmosPress(), P2Set-AtmosPress(), "Press");
      }
    #endif

    bool SkipSomeMasses=false;
    if (Use==TIEU_PartEst && MassInUseOK && MassInUse.GetSize()>0)
      {
      SpecieIter I(som_ALL);
      for (int s=-1; I.Loop(s) && ! SkipSomeMasses; )
        SkipSomeMasses=!MassInUse[s];
      }

    switch (ImgMdl->GetView())
      {
      //case SVV_AsRawMassFlow:
      //  if (SkipSomeMasses)
      //    {
      //    SpecieIter I(som_ALL);
      //    for (int s=-1; I.Loop(s); )
      //      {
      //      double dbgm=QIOM.VMass[s];
      //      if (MassInUse[s])
      //        {
      //        double Qm2SetImg=EstimatePortion*ImgM.VMass[s];
      //        double Qm2SetIn=(1.0-EstimatePortion)*QInM.VMass[s];
      //        double Frac2SetImg=Qm2SetImg/GTZ(ImgM.VMass[s]+QInM.VMass[s]);
      //        double Frac2SetIn=Qm2SetIn/GTZ(ImgM.VMass[s]+QInM.VMass[s]);
      //        QIOM.SetVMass(s, QInM, Qm2SetImg);
      //        QIOM.AddVMass(s, ImgM, Qm2SetIn);
      //        }
      //      else
      //        QIOM.SetVMass(s, QInM, QInM.VMass[s]);

      //      dbgpln("%15.4f %15.4f %15.4f >> %15.4f %s", QInM.VMass[s], ImgM.VMass[s], dbgm, QIOM.VMass[s], SVImg(s).Tag());
      //      }
      //    break;
      //    }
      //  // else fall through
      case SVV_AsMassFlow:
      case SVV_AsMoleFlow:
      case SVV_AsVolFlow:
      case SVV_AsNVolFlow:
        {
        double MassScale=ImgMdl->MassScale();
        if (SkipSomeMasses)
          {
          SpecieIter I(som_ALL);
          for (int s=-1; I.Loop(s); )
            {
            if (MassInUse[s])
              {
              double ImgMass=ImgM.VMass[s]*MassScale;
              double Qm2SetImg=EstimatePortion*ImgMass;
              double Qm2SetIn=(1.0-EstimatePortion)*QInM.VMass[s];
              QIOM.SetVMass(s, QInM, Qm2SetImg);
              QIOM.AddVMass(s, ImgM, Qm2SetIn);
              }
            else
              QIOM.SetVMass(s, QInM, QInM.VMass[s]);
            }
          }
        else
          {
          double QmRqd=TearImage()->QmRqd();
          double QmIn=pQIn->QMass();
          double Qm2SetIn=QmIn;
          double Qm2SetImg=0;
          if (Valid(QmRqd))
            {
            Qm2SetImg=EstimatePortion*QmRqd;
            Qm2SetIn=(1.0-EstimatePortion)*QmIn;
            }
          else
            {
            Qm2SetImg=0;
            Qm2SetIn=QmIn;
            }

          double Frac2SetImg=Qm2SetImg/GTZ(Qm2SetImg+Qm2SetIn);
          double Frac2SetIn=Qm2SetIn/GTZ(Qm2SetImg+Qm2SetIn);

          SpecieIter I(som_ALL);
          for (int s=-1; I.Loop(s); )
            QIOM.SetVMass(s, QInM, Frac2SetIn*QInM.VMass[s]+Frac2SetImg*ImgM.VMass[s]*MassScale);
          }
        break;
        }
      case SVV_AsMassFrac:
      case SVV_AsMoleFrac:
      case SVV_AsVolFrac:
      case SVV_AsNVolFrac:
      case SVV_AsRawFrac:
        {
        double MassScale=ImgMdl->MassScale();
        if (SkipSomeMasses)
          {
          SpecieIter I(som_ALL);
          for (int s=-1; I.Loop(s); )
            {
            if (MassInUse[s])
              {
              double ImgMass=ImgM.VMass[s]*MassScale;
              double Qm2SetImg=EstimatePortion*ImgMass;
              double Qm2SetIn=(1.0-EstimatePortion)*QInM.VMass[s];
              double Frac2SetImg=Qm2SetImg/GTZ(ImgMass+QInM.VMass[s]);
              double Frac2SetIn=Qm2SetIn/GTZ(ImgMass+QInM.VMass[s]);
              QIOM.SetVMass(s, QInM, Qm2SetImg);
              QIOM.AddVMass(s, ImgM, Qm2SetIn);
              }
            else
              QIOM.SetVMass(s, QInM, QInM.VMass[s]);
            }
          }
        else
          {
          double QmRqd=TearImage()->QmRqd();
          double QmIn=pQIn->QMass();
          double Qm2SetIn=QmIn;
          double Qm2SetImg=0;
          if (Valid(QmRqd))
            {
            Qm2SetImg=EstimatePortion*QmRqd;
            Qm2SetIn=(1.0-EstimatePortion)*QmIn;
            }
          else
            {
            Qm2SetImg=0;
            Qm2SetIn=QmIn;
            }

          double Frac2SetImg=Qm2SetImg/GTZ(Qm2SetImg+Qm2SetIn);
          double Frac2SetIn=Qm2SetIn/GTZ(Qm2SetImg+Qm2SetIn);

          SpecieIter I(som_ALL);
          for (int s=-1; I.Loop(s); )
            QIOM.SetVMass(s, QInM, Frac2SetIn*QInM.VMass[s]+Qm2SetImg*ImgM.VMass[s]);
          }
        break;
        }
      }
    #if dbgFlwNode
    if (dbgTearOutputs())
      {
      SpecieIter I(som_ALL);
      for (int s=-1; I.Loop(s); )
        {
        if (dbgm[s]>0 || QIOM.VMass[s]>0)
          dbgpln("%15.4f %15.4f %15.4f >> %15.4f %s", QInM.VMass[s], ImgM.VMass[s], dbgm[s], QIOM.VMass[s], SVImg(s).Tag());
        }
      }
    #endif

    QIOMdl->ClrStatesOK();
    QIOMdl->SetTempPress(Max(1.0, T2Set), Max(0.1, P2Set));
    QIOMdl->SetQualities(ImgMdl);
    QIOMdl->EquilibrateAll();

    TESTMASSNOTNEGC(QIOMdl);

    m_pNdDst->Set_IOP_Self(m_iIODst, Max(0.1, P2Set));
    }
  else
    {
    pQIO->QZero();
    pQIO->SetTempPress(StdT, StdP);
    m_pNdDst->Set_IOP_Self(m_iIODst, Std_P);
    #if dbgFlwNode
    if (dbgTearOutputs())
      {
      dbgpln(" Temp   %10.4f", K2C(pQIO->Temp()));
      dbgpln(" Press  %10.4f", pQIO->Press()-AtmosPress());
      dbgpln(" Mass   Zero");
      }
    #endif
    }
  }

// --------------------------------------------------------------------------

void CFlange::TearInputs2Outputs()
  {
  if (fIsBuffered)
    return;
  // See Method Conduit for Direction Info
  pQIO->QCopy(*pQIn);
  if (gs_Exec.DoingPB())
    m_pNdDst->Set_IOP_Self(m_iIODst, m_pNdSrc->IOP_Flng(m_iIOSrc));
  }

// --------------------------------------------------------------------------

char * CFlange::TearGetTag(Strng & RootTag)
  {
  RootTag.Set("%s#", /*FullTag*/Tag());
  return RootTag();
  };

// --------------------------------------------------------------------------

void CFlange::TearGetInputs(TearVarArray & TV)
  {
  if (fIsBuffered)
    return;

  int n=0;
  TV[n++].Y[0]=pQIn->Temp();
  TV[n++].Y[0]=m_pNdSrc->IOP_Flng/*Self*/(m_iIOSrc);
  for (int s=0; s<SVSpcCount(); s++)
    TV[n++].Y[0]=pQIn->VMass[s];
  for (int s=0; s<SVAttCount(); s++)
    TV[n++].Y[0]=pQIn->VAttr[s];


  int iQ;
  TagObjClass *pQC;
  SQTagObjClass * pSQC;
  LOOP_THRU_QUALITIES(iQ, pQC, pSQC)
    if (/*pQC->Selectable() &&*/ pSQC)
      {
      SpQuality *pQ=pQIn->FindQuality(iQ);
      int N=pSQC->NTearVariables();
      if (pQ)
        {
        pQ->TearGetInputs(TV, n);
        n+=N;
        }
      else
        {
        for (int i=0; i<N; i++)
          TV[n+i].Y[0]=0.0;
        n+=N;
        }
      }

  };

// --------------------------------------------------------------------------

void CFlange::TearGetOutputs(TearVarArray & TV)
  {
  if (fIsBuffered)
    return;

  int n=0;
  TV[n++].X[0]=pQIO->Temp();
  TV[n++].X[0]=m_pNdDst->IOP_Self(m_iIODst);
  for (int s=0; s<SVSpcCount(); s++)
    TV[n++].X[0]=pQIO->VMass[s];
  for (int s=0; s<SVAttCount(); s++)
    TV[n++].X[0]=pQIO->VAttr[s];

  int iQ;
  TagObjClass *pQC;
  SQTagObjClass * pSQC;
  LOOP_THRU_QUALITIES(iQ, pQC, pSQC)
    if (/*pQC->Selectable() &&*/ pSQC)
      {
      SpQuality *pQ=pQIO->FindQuality(iQ);
      int N=pSQC->NTearVariables();
      if (pQ)
        {
        pQ->TearGetOutputs(TV, n);
        n+=N;
        }
      else
        {
        for (int i=0; i<N; i++)
          TV[n+i].X[0]=0.0;
        n+=N;
        }
      }
  };

// --------------------------------------------------------------------------

void CFlange::TearSetOutputs(TearVarArray & TV)
  {
  if (fIsBuffered)
    return;

  // Must not Transfer Qualities this way
  //  pQIO->QCopy(*pQIn);

  pQIO->ChangeModel(pQIn);

  int n=0;
  double T=TV[n++].X[0];
  double P=TV[n++].X[0];
  //pQIO->SetTemp(T);
  for (int s=0; s<SVSpcCount(); s++)
    pQIO->SetVValue(s, Max(0.0, TV[n++].X[0]));
  for (int s=0; s<SVAttCount(); s++)
    pQIO->SetVAttr(s, TV[n++].X[0]);
  pQIO->Model()->ClrMStatesOK();
  pQIO->SetTempPress(T, P);
  m_pNdDst->Set_IOP_Self(m_iIODst, P);

  int iQ;
  TagObjClass *pQC;
  SQTagObjClass * pSQC;
  LOOP_THRU_QUALITIES(iQ, pQC, pSQC)
    {
    SpQuality *pQI=pQIn->FindQuality(iQ);
    if (pSQC && pQI)
      {
      SpQuality *pQO=pQIO->FindQuality(iQ);
      int N=pSQC->NTearVariables();
      if (N>0 && pQO==NULL) // CFlange Outlet does not have qualities
        pQIO->CopyQualities(pQIn->Model(), iQ, iQ);
      else // if (pQI)
        {
        SpQuality *pQO=pQIO->CreateQuality(iQ);
        pQO->TearSetOutputs(TV, n);
        n+=N;
        }
      //else
      //  {
      //  n+=N;
      //  }
/*
      int N=pSQC->NTearVariables();
      if (N>0)
        pQIO->CopyQualities(pQIn->Model(), iQ, iQ);
      else if (pQI)
        {
        SpQuality *pQO=pQIO->CreateQuality(iQ);
        pQO->TearSetOutputs(TV, n);
        n+=N;
        }
      else
        {
        n+=N;
        }
*/
      }
    else
      pQIO->CopyQualities(pQIn->Model(), iQ, iQ);
    }
  };

//===========================================================================

SpVector * CFlange::CreatePMUVector()
  {
  if (pPMU==NULL)
    pPMU=new SpVector;
  return pPMU;
  }

void CFlange::DeletePMUVector()
  {
  delete pPMU;
  pPMU=NULL;
  };

//===========================================================================

void CFlange::ResetMakeUpCntGlbl()
  {
  iMkUpOKCurr=0;
  iMkUpOKNext=0;
  };

void CFlange::BumpMakeUpCntGlbl()
  {
  iMkUpOKCurr=iMkUpOKNext;
  // Must have a value NE Zero for all iters but the first
  if (iMkUpOKNext==250)
    iMkUpOKNext=1;
  else
    iMkUpOKNext++;
  };

void CFlange::BumpMakeUpCnt()
  {
  if (iMkUpOKCurr==0) // Dont Clear on First Iteration
    return;

  if (iMkUpRqdOK != iMkUpOKCurr)
    {
    dMkUpRqd=dNAN;
    iMkUpRqdOK=0;
    }
  if (iMkUpRqdLclOK != iMkUpOKCurr)
    {
    dMkUpRqdLcl=dNAN;
    iMkUpRqdLclOK=0;
    }
  if (iMkUpAvlOK != iMkUpOKCurr)
    {
    dMkUpAvl=dNAN;
    iMkUpAvlOK=0;
    }
  };

#ifndef _RELEASE
//const char* TstTag = "A723S501//A723H01";
double CFlange::MakeUpReqd()
  {
  /*if (_stricmp(FullTag(), TstTag)==0 || _stricmp(FullTag(), "A723H01//A723PRC05")==0)
    {
    Strng s(FullTag());
    int xx=0; }*/
  return Valid(dMkUpRqdLcl) ? dMkUpRqdLcl : dMkUpRqd;
  };

void CFlange::SetMakeUpReqd(double A)
  {
  iMkUpRqdOK=iMkUpOKNext;
  /*if (_stricmp(FullTag(), TstTag)==0 || _stricmp(FullTag(), "A723H01//A723PRC05")==0)
    {
    Strng s(FullTag());
    int xx=0; }*/
  dMkUpRqd=A;
  };

void CFlange::ClrMakeUpReqd()
  {
  iMkUpRqdOK=0;
  /*if (_stricmp(FullTag(), TstTag)==0 || _stricmp(FullTag(), "A723H01//A723PRC05")==0)
    {
    Strng s(FullTag());
    int xx=0; }*/
  dMkUpRqd=dNAN;
  };
#endif

dword  CFlange::ModelStatus()
  {
  dword Status=0;

  if (FlwNode::GetStatusAsStopped())
    {
    Status = FNS_LNoFlw;
    }
  else
    {
    SpConduit * pCd=Conduit(true);
    if (pCd->QMass()>UsableMass)
      {
      Status |= FNS_LFlwP;
      if (pCd->MassFrac(som_Gas)>FlwNode::m_dStatusVapTestFrac)
        Status |= FNS_LFlwVap;
      else if (pCd->MassFrac(som_Sol)>FlwNode::m_dStatusSolTestFrac)
        Status |= FNS_LFlwSol;
      }
    else
      Status |= FNS_LNoFlw;
    }

  return Status;
  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_TAGOBJ(CCtrlTermX, "CtrlTermX", "CtrlTermX", "", "CtrlIO", "T", TOC_SYSTEM, "CtrlTermGroup", "")

CCtrlTermX::CCtrlTermX(pTagObjClass pClass_, char * TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) : \
CTerminal(pClass_, TagIn, pAttach, eAttach)
  {
  //InitializeCriticalSection(&ConnectLockSect);
  m_Attachments=0;

  };

// --------------------------------------------------------------------------

CCtrlTermX::~CCtrlTermX()
  {
  };

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_TAGOBJ(CElecTermX, "ElecTermX", "ElecTermX", "", "ElecIO", "T", TOC_SYSTEM, "ElecTermGroup", "")

CElecTermX::CElecTermX(pTagObjClass pClass_, char * TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) : \
CTerminal(pClass_, TagIn, pAttach, eAttach)
  {
  //InitializeCriticalSection(&ConnectLockSect);
  m_Attachments=0;

  };

// --------------------------------------------------------------------------

CElecTermX::~CElecTermX()
  {
  };

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_TAGOBJ(CAirTermX, "AirTermX", "AirTermX", "", "AirIO", "T", TOC_SYSTEM, "AirTermGroup", "")

CAirTermX::CAirTermX(pTagObjClass pClass_, char * TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) : \
CTerminal(pClass_, TagIn, pAttach, eAttach)
  {
  //InitializeCriticalSection(&ConnectLockSect);
  m_Attachments=0;

  };

// --------------------------------------------------------------------------

CAirTermX::~CAirTermX()
  {
  };

//===========================================================================
//
//
//
//===========================================================================

double  CJoinRec::dProdFracTol=1.0e-6;
long CJoinRec::sm_iNo=0;

CJoinRec::CJoinRec()
  {
  m_iNo=sm_iNo++;
//  fOvrSpecd=0;
//  fWasOvrSpecd=0;
  pQmAux=NULL;

  Clear();
  }

// --------------------------------------------------------------------------

CJoinRec::~CJoinRec()
  {
  if (pQmAux)
    delete pQmAux;
  };

// --------------------------------------------------------------------------

void CJoinRec::Clear()
  {
  pNd               = NULL;
  m_nConns            = 0;
  nProdFracs        = 0;
  fFastConvergeOK   = true;
  fMatrixChanged    = true;
  fProdFracsChgd    = true;
  fHasProdFracs     = false;

  TieInx.Clear();

  dSigmaQmV         = 0.0;
  dSigmaQmL         = 0.0;
  dQmVAcc           = 0.0;
  dQmLAcc           = 0.0;
  dQmV2L            = 0.0;
  dQmVSink          = 0.0;
  dQmLSink          = 0.0;
  Typ               = JTyp_Unknown;
  pGFb              = NULL;
  iJoinArrayInx     = -1;
  m_iJoinId         = -1;

  fLonely           = false;
  fUnDefined        = false;
  fIsolatedNet      = false;
  fIsolatedNode     = false;
  fInFixedNet       = false;
  fIsFixedXfer      = false;
  fIsFixedExtern    = false;
  fInXferNet        = false;
  fHasQRatio        = false;
  fHasXBstHere      = false;
//  fIsPSetEdge       = false;
//  fIsPSetEdge1      = false;
//  fIsPSetEdgeN      = false;
  fNetWrk           = false;
  iFxdQOrder        = -1;
  iFxdPOrder        = -1;
  iFxdPSrcCn        = -1;
  fIsOpen           = false;
  fIsTie            = false;
  fIs2IO            = false;
  fIsImpNd          = false;
  fIsAccLnk         = false;
  fIsPumpNd         = false;
  fIsMkup           = false;
  fIsAudit          = false;
  fIsDirect         = false;
  //fHasVolume        = 0;
  fHasQmAux         = false;
  Dirn              = 0;

  Prv_Sigma_R       = 1.0;

  fSelfLoop         = false;
  fUDFShut          = false;
  fDamped           = false;

//  iSolnTyp                      = UnknownSoln;
  if (pQmAux)
    pQmAux->Clear();

  fVolPrsFX         = false;
  fVolIsSmall       = true;
  //m_fVFlowAllowed   = false;
  //m_fLFlowAllowed   = false;
  fJnV2LReqd        = false;
  fIOV2LReqd        = false;
  m_iFlwPh_Act        = FPh_Both;
  m_iFlwPh_Rqd        = FPh_Both;

#if WithOEP
  if (OEPOptions())
    {
    // fHasVolume      =  True;
    fVFlowAllowed       = true;
    fLFlowAllowed       = false;
//    fJnV2LReqd      = false;
//    fIOV2LReqd      = false;
    }
#endif
  dV2LQmReqd        = 0.0;
//  dV2LQmVFact       = 0.0;
//  dV2LQmLFact       = 0.0;
  dPressEquil       = dNAN;

  m_NetNo           = -1;
  m_SubNetNo        = -1;
  sNetInfo          = "";
  }

// --------------------------------------------------------------------------

//char * CJoinRec::TIndicesStr(char * Buff)
//  {
//  char P[33], V[50], Va[33], L[50], La[33], X[33], Iso[33], Iso1[33];
//
//  sprintf(V, "%s,%s,%s", "", (TQVAccInx() >=0 ? ltoa(TQVAccInx(), Va, 10) : ""), "");
//  sprintf(L, "%s,%s,%s", "", (TQLAccInx() >=0 ? ltoa(TQLAccInx(), La, 10) : ""), "");
//  sprintf(Iso, "%s", (TQIsoInx() >=0 ? ltoa(TQIsoInx(), Iso1, 10) : "."));
//
//  sprintf(Buff, "%3.3s %11.11s %11.11s %3s %3s",
//    (PInx() >=0 ? ltoa(PInx(), P, 10) : "."),V,L,(TQV2LInx() >=0 ? ltoa(TQV2LInx(), X, 10) : "."),Iso);
//  return Buff;
//  }
//
//// --------------------------------------------------------------------------
//
//char * CJoinRec::LIndicesStr(char * Buff)
//  {
////  char P[33], V[50], Vi[33], Va[33], Vo[33], L[50], Li[33], La[33], Lo[33], Iso[33], Iso1[33];
////  sprintf(V, "%s,%s,%s",
////    (LQVInx() >=0 ? ltoa(LQVInx(), Vi, 10) : ""),
////    (LQVAccInx() >=0 ? ltoa(LQVAccInx(), Va, 10) : ""),
////    (LQVOutInx() >=0 ? ltoa(LQVOutInx(), Vo, 10) : ""));
////  sprintf(L, "%s,%s,%s",
////    (LQLInx() >=0 ? ltoa(LQLInx(), Li, 10) : ""),
////    (LQLAccInx() >=0 ? ltoa(LQLAccInx(), La, 10) : ""),
////    (LQLOutInx() >=0 ? ltoa(LQLOutInx(), Lo, 10) : ""));
////  sprintf(Iso, "%s", (TQIsoInx() >=0 ? ltoa(TQIsoInx(), Iso1, 10) : "."));
////
////  sprintf(Buff, "%3.3s %11.11s %11.11s %3s %3s",
////    (PInx() >=0 ? ltoa(PInx(), P, 10) : "."),V,L,"",Iso);
////  return Buff;
//  return "";
//  }

// --------------------------------------------------------------------------

//char * CJoinRec::IndexStr(int I, char * Buff)
//  {
//  char B[33];
//  sprintf(Buff, "%s", (I >=0 ? ltoa(I, B, 10) : "."));
//  return Buff;
//  };
//
// --------------------------------------------------------------------------

void CJoinRec::Prepare()
  {
  Typ             = JTyp_Unknown;
  Typ             = JTyp_Unknown;
  NdInx           = -3; // Never Touched

  TieInx.Clear();
//  iPInx           = UnUsedIndex;
//  iPSrcInx        = UnUsedIndex;
//  iPDstInx        = UnUsedIndex;
//  iQVInInx        = UnUsedIndex;
//  iQLInInx        = UnUsedIndex;
//  iQVOutInx       = UnUsedIndex;
//  iQVAccInx       = UnUsedIndex;
//  iQLOutInx       = UnUsedIndex;
//  iQLAccInx       = UnUsedIndex;
//  iQV2LInx        = UnUsedIndex;
//  iQIsoInx        = UnUsedIndex;

  m_pSubNet       = NULL;
  // CNM 02/12/95 fFixed                                 = 0;
  fInFixedNet     = false;
  fHasXBstHere    = false;
  fIsFixedExtern  = false;
  fIsFixedXfer    = false;
  fInXferNet      = false;

//  fIsPSetEdge     = false;
//  fIsPSetEdge1    = false;
//  fIsPSetEdgeN    = false;
  fNetWrk         = false;
  fIsolatedNet    = false;
  fIsolatedNode   = false;
  fIsoNetRef      = false;
  fLonely         = false;
  fUnDefined      = false;
  Dirn            = 0;
  fHasQRatio      = false;
  iFxdQOrder      = -1;
  iFxdPOrder      = -1;
  iFxdPSrcCn      = -1;
  for (int i = 0; i < m_nConns; i++)
    {
    for (int fe=0; fe < NIOFBs(i); fe++)
      {
      IOFB(i, fe)->UDF.fInList=false;
      IOFB(i, fe)->m_fInXferNet=false;
      }
    }

  fHasQmAux       = (pQmAux!=NULL);
  fFastConvergeOK = true;
  fMatrixChanged  = true;
  fProdFracsChgd  = true;

  SetTieLnkFlags();
  }

// --------------------------------------------------------------------------

void CJoinRec::SetTieLnkFlags()
  {
  if (fIsOpen)
    {
    fIsImpNd  = false;
    fIsAccLnk = false;
    fIsTie    = false;
    fIs2IO    = false;
    }
  else
    {
    fIsImpNd  = (fDamped && 1);//IsImpSolution());
    fIsAccLnk = (fVolPrsFX && 0);//IsImpSolution());
    fIsTie    = ((m_nConns>2) || ((m_nConns==2) && (fIsImpNd || fIsAccLnk || fIsPumpNd)));
    fIs2IO    = ((m_nConns==2) && !fIsTie);
    }
  }

//===========================================================================

CLinkRec::CLinkRec()
  {
  m_iJoinId0=-1;
  m_iJoinId1=-1;
  //m_pFEP=NULL;
  m_pNd=NULL;
  m_pGFb=NULL;
  m_FBs.SetSize(1);
  m_PBs.SetSize(2);
  FlwBlk &FB=m_FBs[0];
  FB.m_pFlPB=&m_PBs[0];
  FB.m_pNdPB=&m_PBs[1];

  }

//--------------------------------------------------------------------------

CLinkRec::~CLinkRec()
  {
  }

//--------------------------------------------------------------------------

#define dbgSetQm 0

//===========================================================================
#if DoPressTestAndRange
const double MaxTestPress = 1.0e15;
#ifndef _RELEASE
static flag DoPressCheck = 1;
#endif
double PressTest_(double P)
  {
  #ifndef _RELEASE
  ASSERT_ALWAYS(!DoPressCheck || !Valid(P) || P<MaxTestPress, "Excessive pressure!!!", __FILE__, __LINE__);
  #endif
  return (Valid(P ) ? Min(MaxTestPress, P) : P);
  };
#endif
//===========================================================================
//
//
//
//===========================================================================

CFlwRegBlk::CFlwRegBlk(LPCTSTR CtrlName,
                       byte ShowWhat,
                       byte iWhatOk,
                       byte iWhat,
                       float dTau,
                       float dPiReqd,
                       float dPoReqd,
                       float dQmReqd,
                       float dCtrlMin,
                       float dCtrlMax
                       )
  {
  m_sCtrlMinTg  = CtrlName;
  m_sCtrlMaxTg  = CtrlName;
  m_sCtrlCurTg  = CtrlName;
  m_sCtrlMinTg  += "Min";
  m_sCtrlMaxTg  += "Max";
  m_sCtrlCurTg  += "Cur";
  m_iShowWhat   = ShowWhat;
  m_iWhat       = iWhat;
  m_iWhatOk     = iWhatOk;
  m_bHoldCvg    = false;
  m_dTau        = dTau;
  m_dPiReqd     = dPiReqd;
  m_dPiMeas     = (float)Std_P;
  m_dPoReqd     = dPoReqd;
  m_dPoMeas     = (float)Std_P;
  m_dQmReqd     = dQmReqd;
  m_dQmMeas     = 0.0f;
  m_dQvReqd     = 0;//dQmReqd;
  m_dQvMeas     = 0.0f;
  m_dCtrlMin    = Max(0.0001f, dCtrlMin);
  m_dCtrlMax    = Min(1.0f, dCtrlMax);
  m_dCtrlCur    = 1;
  m_pFB         = NULL;
  };

void CFlwRegBlk::BuildDataDefn(DataDefnBlk &DDB, FlwNode * pNd, bool ModeOK)
  {
  if (ModeOK || DDB.ForFileSnpScn()) //m_Data d->FlowMode()==LFM_Linear || pNd->FlowMode()==LFM_Full)
    {
    DDB.BeginStruct(pNd, "Regulator", NULL, DDB_NoPage);
    DDBValueLstMem DDBV;
    DDBV.Empty();
    DDBV.Add(FBReg_Off, "Off");
    if (m_iWhatOk & FBReg_Pi)
      {
      DDBV.Add(FBReg_Pi , "Pi", MDD_Hidden);
      DDBV.Add(FBReg_Pi , "InletPress");
      }
    if (m_iWhatOk & FBReg_Po)
      {
      DDBV.Add(FBReg_Po , "Po", MDD_Hidden);
      DDBV.Add(FBReg_Po , "OutletPress");
      }
    if (m_iWhatOk & FBReg_Qm)
      {
      DDBV.Add(FBReg_Qm , "Qm", MDD_Hidden);
      DDBV.Add(FBReg_Qm , "MassFlow");
      }
    if (m_iWhatOk & FBReg_Qv)
      {
      DDBV.Add(FBReg_Qv , "Qv", MDD_Hidden);
      DDBV.Add(FBReg_Qv , "VolumeFlow");
      }
    if (m_iWhatOk & FBReg_QvStep)
      DDBV.Add(FBReg_QvStep , "VolumeFlow(Step)");

    DDB.Visibility(NM_Dynamic|SM_All|HM_All, (m_iShowWhat&FBRShow_What)!=0);
    DDB.Byte("Mode", "", DC_, "", &m_iWhat, pNd, isParm|SetOnChange, DDBV());
    DDB.Float ("TimeConst",    "Tau",     DC_Time, "s",  &m_dTau,   pNd, isParm);

    DDB.Visibility(NM_Dynamic|SM_All|HM_All, (m_iWhat & FBReg_Pi)!=0 && (m_iShowWhat&FBRShow_Reqd)!=0);
    DDB.Float ("PressInReqd",  "PInRqd",  DC_P,  "kPa",  &m_dPiReqd,pNd, isParm);
    DDB.Float ("PressIn",      "PIn",     DC_P,  "kPa",  &m_dPiMeas,pNd, 0);
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, (m_iWhat & FBReg_Po)!=0 && (m_iShowWhat&FBRShow_Reqd)!=0);
    DDB.Float ("PressOutReqd", "POutRqd", DC_P,  "kPa",  &m_dPoReqd,pNd, isParm);
    DDB.Float ("PressOut",     "POut",    DC_P,  "kPa",  &m_dPoMeas,pNd, 0);
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, (m_iWhat & FBReg_Qm)!=0 && (m_iShowWhat&FBRShow_Reqd)!=0);
    DDB.Float ("MassFlowReqd", "QmRqd",   DC_Qm, "kg/s", &m_dQmReqd,pNd, isParm);
    DDB.Float ("MassFlow",     "Qm",      DC_Qm, "kg/s", &m_dQmMeas,pNd, 0);
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, (m_iWhat & (FBReg_Qv|FBReg_QvStep))!=0 && (m_iShowWhat&FBRShow_Reqd)!=0);
    DDB.Float ("VolumeFlowReqd", "QvRqd",   DC_Qv, "L/s", &m_dQvReqd,pNd, isParm);
    DDB.Float ("VolumeFlow",     "Qv",      DC_Qv, "L/s", &m_dQvMeas,pNd, 0);

    DDB.Double("", "MeasRho",    DC_Rho, "kg/m^3", xidFRBMeasRho,     pNd, 0);
    DDB.Double("", "MeasRhoS",   DC_Rho, "kg/m^3", xidFRBMeasRhoStep, pNd, 0);

    DDB.Visibility(NM_Dynamic|SM_All|HM_All, (m_iShowWhat&FBRShow_Min)!=0);
    bool SwapMinMax=PrjFileVerNo()<82;
    DDB.Float (SwapMinMax ? m_sCtrlMaxTg():m_sCtrlMinTg(), "",    DC_Frac,    "%",     &m_dCtrlMin,pNd, isParm);
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, (m_iShowWhat&FBRShow_Max)!=0);
    DDB.Float (SwapMinMax ? m_sCtrlMinTg():m_sCtrlMaxTg(), "",    DC_Frac,    "%",     &m_dCtrlMax,pNd, isParm);
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, (m_iShowWhat&FBRShow_Cur)!=0);
    DDB.Float (m_sCtrlCurTg(), "",    DC_Frac,    "%",     &m_dCtrlCur,pNd, 0);

    DDB.Visibility();
    DDB.EndStruct();
    }
  };
flag CFlwRegBlk::DataXchg(DataChangeBlk & DCB, FlwNode * pNd)
  {
  switch (DCB.lHandle)
    {
    case xidFRBMeasRho:
      DCB.D = m_pFB ? m_pFB->m_MeasRho:dNAN;
      return 1;
    case xidFRBMeasRhoStep:
      DCB.D = m_pFB ? m_pFB->m_MeasRhoStep:dNAN;
      return 1;
    }
  return false;
  };
flag CFlwRegBlk::ValidateData(ValidateDataBlk & VDB, FlwNode * pNd)
  {
  m_dTau        = Range(0.0001f, m_dTau,        1000.0f);
  m_dCtrlMin    = Range(0.0001f, m_dCtrlMin,    1.0f);
  m_dCtrlMax    = Range(m_dCtrlMin, m_dCtrlMax, 1.0f);
  return true;
  };

//===========================================================================
//
//
//
//===========================================================================

CClosureInfo::CClosureInfo()
  {
  Clear();
  //m_DoFlows = false;
  };
CClosureInfo::CClosureInfo(const CClosureInfo & X)
  {
  *this=X;
  };

void CClosureInfo::ZeroFlows()
  {
  m_totHfIn             = 0;
  m_totHfOut            = 0;
  m_totHsIn             = 0;
  m_totHsOut            = 0;
  m_MassFlwIn           = 0;
  m_MassFlwOut          = 0;

  m_HfGain              = 0;
  m_HsGain              = 0;
  m_HfLoss              = 0;
  m_HsLoss              = 0;
  m_MassGain            = 0;
  m_MassLoss            = 0;
  m_PowerIn             = 0;

  m_RCTMassGain         = 0;
  m_RCTHfGain           = 0;
  m_RCTHsGain           = 0;
  m_RCTPowerIn          = 0;
  m_RCTHOR_Std0         = 0;
  m_RCTHOR_Mdl0         = 0;
  m_RCTHOR_Used0        = 0;
  m_RCTHOR_Diff0        = 0;
  m_HfGainAtZero        = 0;
  m_EHXPowerIn          = 0;
  m_FeedCpT             = 0;
  m_ProdCpT             = 0;
  }

void CClosureInfo::ZeroContent()
  {
  m_ContentMass         = 0;
  m_ContentHeat         = 0;

  //m_InitialContent      = 0;
  //m_InitialHeat         = 0;
  //m_NettMass            = 0;
  //m_NettHeat            = 0;
  };

void CClosureInfo::Clear()
  {
  m_JoinId              = -1;
  m_pRefMdl             = NULL;
  ZeroFlows();
  ZeroContent();
  };


CClosureInfo & CClosureInfo::operator=(const CClosureInfo  & X)
  {
  //m_IsValid             = X.m_IsValid;
  //m_DoFlows             = X.m_DoFlows;
  m_JoinId              = X.m_JoinId;
  m_pRefMdl             = X.m_pRefMdl;

  m_totHfIn             = X.m_totHfIn;
  m_totHfOut            = X.m_totHfOut;
  m_totHsIn             = X.m_totHsIn;
  m_totHsOut            = X.m_totHsOut;
  m_MassFlwIn           = X.m_MassFlwIn;
  m_MassFlwOut          = X.m_MassFlwOut;

  m_HfGain              = X.m_HfGain;
  m_HsGain              = X.m_HsGain;
  m_HfLoss              = X.m_HfLoss;
  m_HsLoss              = X.m_HsLoss;
  m_MassGain            = X.m_MassGain;
  m_MassLoss            = X.m_MassLoss;
  m_PowerIn             = X.m_PowerIn;

  m_RCTMassGain         = X.m_RCTMassGain;
  m_RCTHfGain           = X.m_RCTHfGain;
  m_RCTHsGain           = X.m_RCTHsGain;
  m_RCTPowerIn          = X.m_RCTPowerIn;
  m_RCTHOR_Std0         = X.m_RCTHOR_Std0;
  m_RCTHOR_Mdl0         = X.m_RCTHOR_Mdl0;
  m_RCTHOR_Used0        = X.m_RCTHOR_Used0;
  m_RCTHOR_Diff0        = X.m_RCTHOR_Diff0;
  m_HfGainAtZero        = X.m_HfGainAtZero;
  m_EHXPowerIn          = X.m_EHXPowerIn;

  m_ContentMass         = X.m_ContentMass;
  m_ContentHeat         = X.m_ContentHeat;

  m_InitialContent      = X.m_InitialContent;
  m_InitialHeat         = X.m_InitialHeat;
  m_NettMass            = X.m_NettMass;
  m_NettHeat            = X.m_NettHeat;
  return *this;
  };

CClosureInfo & CClosureInfo::operator+=(const CClosureInfo  & X)
  {
  m_JoinId              = -1;
  //m_pRefMdl             += X.m_pRefMdl;

  m_totHfIn             += X.m_totHfIn;
  m_totHfOut            += X.m_totHfOut;
  m_totHsIn             += X.m_totHsIn;
  m_totHsOut            += X.m_totHsOut;
  m_MassFlwIn           += X.m_MassFlwIn;
  m_MassFlwOut          += X.m_MassFlwOut;

  m_HfGain              += X.m_HfGain;
  m_HsGain              += X.m_HsGain;
  m_HfLoss              += X.m_HfLoss;
  m_HsLoss              += X.m_HsLoss;
  m_MassGain            += X.m_MassGain;
  m_MassLoss            += X.m_MassLoss;
  m_PowerIn             += X.m_PowerIn;

  m_RCTMassGain         += X.m_RCTMassGain;
  m_RCTHfGain           += X.m_RCTHfGain;
  m_RCTHsGain           += X.m_RCTHsGain;
  m_RCTPowerIn          += X.m_RCTPowerIn;
  m_RCTHOR_Std0         += X.m_RCTHOR_Std0;
  m_RCTHOR_Mdl0         += X.m_RCTHOR_Mdl0;
  m_RCTHOR_Used0        += X.m_RCTHOR_Used0;
  m_RCTHOR_Diff0        += X.m_RCTHOR_Diff0;
  m_HfGainAtZero        += X.m_HfGainAtZero;
  m_EHXPowerIn          += X.m_EHXPowerIn;

  m_ContentMass         += X.m_ContentMass;
  m_ContentHeat         += X.m_ContentHeat;

  //m_InitialContent      += X.m_InitialContent;
  //m_InitialHeat         += X.m_InitialHeat;
  //m_NettMass            += X.m_NettMass;
  //m_NettHeat            += X.m_NettHeat;
  return *this;
  };

//void CClosureInfo::AddMassFlowIn(double M)
//  {
//  if (M>=0)
//    m_MassGain += M;
//  else
//    m_MassLoss -= M;
//  }
//void CClosureInfo::AddHeatFlowIn(double H)
//  {
//  if (H>=0)
//    m_HeatGain += H;
//  else
//    m_HeatLoss -= H;
//  }

//===========================================================================

void CClosureHfGain::SetHfInAtZero(SpModel & Mdl)
  {
  m_HfGainAtZero=Mdl.totHf(som_ALL, C2K(0), StdP);
  };
void CClosureHfGain::AddHfInAtZero(SpModel & Mdl)
  {
  m_HfGainAtZero+=Mdl.totHf(som_ALL, C2K(0), StdP);
  };
void CClosureHfGain::AddHfOutAtZero(SpModel & Mdl)
  {
  m_HfGainAtZero-=Mdl.totHf(som_ALL, C2K(0), StdP);
  };


//===========================================================================

CClosureInfoArray::CClosureInfoArray()
  {
  m_DataValid=false;
  m_DataAvail=false;
  m_DoFlows=false;
  };

void CClosureInfoArray::Setup(bool DoFlows, int NJoins)
  {
  m_DoFlows = DoFlows;
  SetSize(NJoins);
  for (int j=0; j<GetSize(); j++)
    GetAt(j).m_JoinId=j;
  };

void CClosureInfoArray::ZeroFlows()
  {
  for (int j=0; j<GetSize(); j++)
    {
    GetAt(j).m_JoinId=j;
    GetAt(j).ZeroFlows();
    };
  };

void CClosureInfoArray::ZeroContent()
  {
  for (int j=0; j<GetSize(); j++)
    {
    GetAt(j).m_JoinId=j;
    GetAt(j).ZeroContent();
    };
  };
void CClosureInfoArray::Clear()
  {
  for (int j=0; j<GetSize(); j++)
    {
    GetAt(j).m_JoinId=j;
    GetAt(j).Clear();
    };
  };

//===========================================================================
//
//
//
//===========================================================================

LPTSTR IOAreaRec::DefaultIOName(int iOrigFileVerNo)
  { 
  if (m_pAltNames)
    {
    for (int i=0; m_pAltNames[i].m_pName; i++) 
      {
      if (m_pAltNames[i].m_DefB4Ver>0 && m_pAltNames[i].m_DefB4Ver<iOrigFileVerNo)
        return m_pAltNames[i].m_pName;
      }
    }
  return m_pName;
  };


void IOAreaRec::Duplicate(IOAreaRec & I)
  {
  *this=I;

  SetIODesc(strdup(I.IODesc()));
  SetIOName(strdup(I.IOName()));
  m_pDefaultTS=strdup(I.m_pDefaultTS);

  if (I.IONameCount()>1)
    {
    m_pAltNames=new CAltIOName[I.IONameCount()];
    for (int n=1; n<I.IONameCount(); n++)
      {
      m_pAltNames[n-1].m_pName=strdup(I.m_pAltNames[n-1].m_pName);
      m_pAltNames[n-1].m_DefB4Ver=I.m_pAltNames[n-1].m_DefB4Ver;
      }
    m_pAltNames[n-1].m_pName=0;
    }
  else
    m_pAltNames=NULL;
  }

void IOAreaRec::PreDelete()
  {
  free(IODesc());
  free(IOName());
  free(m_pDefaultTS);
  for (int n=1; n<IONameCount(); n++)
    free(m_pAltNames[n-1].m_pName);
  delete [] m_pAltNames;
  }

//===========================================================================
//
//
//
//===========================================================================

flag FlwNode::fInConfigureJoins=0;
flag FlwNode::fCollectChgStats=0;
dword FlwNode::m_dwMdlStatusOptions=0;
double FlwNode::m_dStatusVapTestFrac=0.99;
double FlwNode::m_dStatusSolTestFrac=0.9;

long gs_ClosureOption   = CloseTest_Off;
long gs_SpillOption     = SpillReports_Cnds;
long gs_AuditOption     = Audit_Off;
bool gs_FSIdleRequested = false;

CConnectMap FlwNode::sm_ConnectMap;
CConnectList FlwNode::sm_ConnectList;

// --------------------------------------------------------------------------

FlwNode::FlwNode(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CTNode(pClass_, TagIn, pAttach, eAttach),
  m_ExtAudit(this)
  {
  #if dbgFlwNode
  if (dbgConstruction())
    dbgpln("FlwNode()");
  #endif

  if (stricmp(Class()->Group(), FlwUnitGrp)==0)
    gs_License.BumpNodeCount(+1, TagIn);

  if (!Class()->LicenseOK())
    {
    gs_License.BumpIllegalNodeCount(+1, Class()->ClassName(), !Class()->LicErrNoted() ? eLic_Msg:eLic_None);
    Class()->SetLicErrNoted(true);
    }

  m_AllowedModes=0;

  m_pIOAreas=NULL;
  m_pIOAreasOwned=NULL;

  m_nFIOs         = 0;
  m_nProcLnkIOs   = 0;
  m_nProcessIOs   = 0;

  m_nProcDirectIOs = 0;
  m_nMkupDirectIOs = 0;

  m_iCIO1         = 0;
  m_nCIOs         = 0;
  m_iEIO1         = 0;
  m_nEIOs         = 0;
  m_iAIO1         = 0;
  m_nAIOs         = 0;
  m_nIOs          = 0;
  //m_nXRefs      = 0;
  m_nJoins        = 0;
  m_nProcessJoins = 0;
  m_nMkupJoins    = 0;
  m_nAuditJoins   = 0;
  m_nJoins        = 0;
  m_nRqdJoins     = 0;
  m_nLinks        = 0;
  m_nRqdLinks     = 0;

  //m_nFlwIOsIn     = 0;
  //m_nFlwIOsOut    = 0;
  //m_nFlwIOsInReqd = 0;
  //m_nFlwIOsInDone = 0;

  m_nCtrlIOsIn    = 0;
  m_nCtrlIOsOut   = 0;
  m_nSeqCtrlIOsIn = 0;
  m_nSeqCtrlIOsOut = 0;
  m_nXRefsIn      = 0;
  m_nXRefsOut     = 0;
  m_nSeqXRefsIn   = 0;
  m_nSeqXRefsOut  = 0;
  m_nCtrlsInReqd  = 0;
  m_nCtrlsInDone  = 0;

  m_nElecIOsIn    = 0;
  m_nElecIOsOut   = 0;
  m_nCAirIOsIn    = 0;
  m_nCAirIOsOut   = 0;
  m_nPwrIOsInReqd = 0;
  m_nPwrIOsInDone = 0;

  m_InPwrCtrlSeq=false;

  //m_lPrevXRefUpdates=XR_InitValue;

  pNetTopoChg=NULL;
  pSubNetTopoChg=NULL;

  m_Origin.m_X = 0.0;
  m_Origin.m_Y = 0.0;
  m_Origin.m_Z = 0.0;

  m_Datum      = 0.0;
  m_DatumRqd   = dNAN;
  m_ZState     = Z_Unknown;

  Done2        = 0;
  Reverse      = 0;

  fSolutionBusy=0;  // Solution is Busy
  fHasFiles=0;
  Joins.SetSize(0);

  m_AuditOption      = Audit_Ignore;
  m_ForceAuditOption = Audit_Ignore;

  #if NDSTOPWATCHES
  pSWs=NULL;
  #endif

  fActiveInMacroMdl=false;
  m_fValidateDataComplete=false;
  m_fGlobalConnectBusy=false;
  m_fRevalidate=false;

  WrkConduits.InitHashTable(7);

  //m_iProcOrder=iNotInEvalOrder;
  m_iCtrlOrder=iNotInEvalOrder ;
  iAllIndex=-1;
  fSolutionForRoot=false;
  fIntegralDone=false;
  fEvalPressSensReqd=false;
  fElecConnsDone=false;

  fMarked=false;
  fNegPress=false;

  m_iEvalPosn=128;

  //m_Closure.m_NettHeat=0;
  //m_Closure.m_NettMass=0;
  //m_Closure.m_InitialHeat=0;
  //m_Closure.m_InitialContent=0;//.Zero();

  AttachClassInfo(nc_Process, NULL);

  //m_DDBRunModes.Add(SM_DynBoth Xfer);
  m_AreaNdListPos=NULL;

#if WITHNODEPROCS
  m_ProcsAllowed   = false;
  m_ProcsReqd = false;
  m_pProcs      = NULL;
#endif
#if WITHNODETAGIO
  m_pTagIO      = NULL;
  //m_EstTagCount = -1;
#endif

  InitializeCriticalSectionAndSpinCount(&m_LockSect, 4000);
  };

// --------------------------------------------------------------------------

FlwNode::~FlwNode()
  {
  DeleteCriticalSection(&m_LockSect);

  Joins.SetSize(0);
  if (NoIOs())
    LogError(FullObjTag(), 0, "Internal error - Deleting Node with current Connections");

  #if NDSTOPWATCHES
  delete pSWs;
  #endif

  POSITION Pos=WrkConduits.GetStartPosition();
  while (Pos)
    {
    CString a;
    SpConduit *p;
    WrkConduits.GetNextAssoc(Pos, a, p);
    delete p;
    }
  WrkConduits.RemoveAll();

  #if dbgFlwNode
  if (dbgConstruction())
    dbgpln("~FlwNode()");
  #endif

  if (m_pIOAreasOwned)
    {
    for (int i=0; m_pIOAreasOwned[i].IODesc()!=NULL; i++)
      m_pIOAreasOwned[i].PreDelete();
    delete [] m_pIOAreasOwned;
    }

  if (m_pArea.Connected())
    {
    CString S;
    S.Format("Node %s still connected to Area", FullObjTag());
    ASSERT_ALWAYS(FALSE, (LPTSTR)(LPCTSTR)S, __FILE__, __LINE__);
    }

  delete m_pTagIO;

  if (stricmp(Class()->Group(), FlwUnitGrp)==0)
    gs_License.BumpNodeCount(-1, FullObjTag());

  if (!Class()->LicenseOK())
    gs_License.BumpIllegalNodeCount(-1, Class()->ShortDesc(), eLic_None);

  };

//--------------------------------------------------------------------------

void FlwNode::AttachClassInfo(DWORD Class_, IOAreaRec * IOAreas_, CFlwEqnGrp * DefFlwEqnGrp)
  {
  m_dwType=Class_;
  AttachIOAreas(IOAreas_, DefFlwEqnGrp);
  };

//--------------------------------------------------------------------------

void FlwNode::AttachClassInfo(DWORD Class_, IOAreaRec * IOAreas_, CFlwEqnGrp * DefFlwEqnGrp0, CFlwEqnGrp * DefFlwEqnGrp1)
  {
  m_dwType=Class_;
  AttachIOAreas(IOAreas_, DefFlwEqnGrp0, DefFlwEqnGrp1);
  };

//--------------------------------------------------------------------------

bool FlwNode::FixElectricalIO(IOAreaRec * FixedIOPts, CETermStripDefnArray & NewTerms)
  {
  int Id=ElecIOId(0);
  long N=0;

  CArray <IOAreaRec, IOAreaRec&> TmpIOAreas;
  TmpIOAreas.SetSize(0, 16);

  // all non elnks are required
  for (int i=0; IOArea(i).IODesc(); i++)
    if ((IOArea(i).m_dwType&nc_ELnk)==0)
      {
      TmpIOAreas.SetAtGrow(N++, IOArea(i));
      Id=Max(Id, IOArea(i).m_Id);
      }

  if (FixedIOPts)
    for ( ;FixedIOPts->IODesc(); FixedIOPts++)
      {
      TmpIOAreas.SetAtGrow(N++, *FixedIOPts);
      Id=Max(Id, FixedIOPts->m_Id);
      }

  for (i=0; i<NewTerms.GetSize(); i++)
    {
    CETermStripDefn &NT=NewTerms[i];
    IOAreaRec IOPt = {"", (LPTSTR)(LPCTSTR)NT.m_sName, Id++, NT.m_bIn?LIO_In:LIO_Out, nc_ELnk, NT.m_bReqd?1:0, 1};
    //IOWires.SetSize(N+1);
    //for (int w=0; w<NC.m_Wires.GetSize(); w++)
    //  IOWires[N].SetAtGrow(w, (LPCTSTR)NC.m_Wires[w]);
    //IOWires[N].SetAtGrow(w, (LPCTSTR)NULL);

//    IOPt.m_pTerms=new CETermStripDefn;//&IOWires[N][0];
//    (*IOPt.m_pTerms)=NT;

    IOPt.m_pDefaultTS=(LPTSTR)(LPCTSTR)NT.m_sName;//pTerms=new CETermStripDefn;//&IOWires[N][0];

    TmpIOAreas.SetAtGrow(N++, IOPt);
    }

  IOAreaRec UnassignedIOPt = {"",    "Electrical",  Id++, LIO_InOut, nc_ELnk, 0,50};
  TmpIOAreas.SetAtGrow(N++, UnassignedIOPt);

  IOAreaRec NullIOPt = {0};
  TmpIOAreas.SetAtGrow(N++, NullIOPt);

  AttachIOAreas(&TmpIOAreas[0], NULL, true);

  //for (i=0; i<TmpIOAreas.GetSize(); i++)
  //  delete TmpIOAreas[i].m_pTerms;

  StructureChanged(this);
  return true;
  };

//--------------------------------------------------------------------------

void FlwNode::SwapIOAreas(IOAreaRec * IOAreas_, bool Copy)
  {
  const int dbgSwapIOAreas=0;
  int NXtra=0;
  bool InUse[MaxIOList+1];
  bool Retained[MaxIOList+1];
  long NewDescPos[MaxIOList+1];
  long NNew=0;

  for (int i=0; IOAreas_ && IOAreas_[i].IODesc()!=NULL; i++)
    {
    //if (IOAreas_[i].m_Id>=27 && IOAreas_[i].m_Id<64)
    //  {
    //  LogError(ClassId(), LF_Exclamation, "Possible BadIOId - Bld 116 Changes");
    //  dbgpln("%3i %5i %-20s, %s", i, IOAreas_[i].m_Id, IOAreas_[i].IOName(), ClassId());
    //  }
    NNew++;
    }
  if (dbgSwapIOAreas)
    {
    dbgpln("SwapIO ----------- %s", FullObjTag());
    for (int i=0; i<m_IOs.GetCount(); i++)
      dbgpln("IO %3i) %6i %6i %s", i, IODescNo_Self(i), IODescNo_Rmt(i), IODesc_Self(i)->IOName());
    }

  if (IOAreas)
    {
    for (int i=0; IOArea(i).IODesc()!=NULL; i++)
      {
      InUse[i]=false;
      Retained[i]=false;

      if (dbgSwapIOAreas)
        dbgpln("SwapIO Old %6i %s", IOArea(i).m_Id, IOArea(i).IOName());
      }
    for (i=0; i<m_nIOs; i++)
      NewDescPos[i]=-1;

    for (i=0; i<m_nIOs; i++)
      InUse[IODescNo_Self(i)]=true;
    for (i=0; IOArea(i).IODesc()!=NULL; i++)
      {
      for (int j=0; IOAreas_[j].IODesc()!=NULL; j++)
        if (_stricmp(IOArea(i).IOName(), IOAreas_[j].IOName())==0)
          {
          NewDescPos[i]=j;
          Retained[i]=true;
          break;
          }
      }
    for (i=0; IOArea(i).IODesc()!=NULL; i++)
      if (InUse[i] & !Retained[i])
        {
        NewDescPos[i]=NNew+(NXtra++);
        LogError(FullObjTag(), 0, "IOPoint %s not removed because it is in use", IOArea(i).IOName());
        }
    }

  IOAreaRec * p2Delete=m_pIOAreasOwned;
  if ((Copy || NXtra>0) && IOAreas_)
    {
    IOAreaRec * pNew=new IOAreaRec[NNew+1+NXtra];
    int LastId=0;
    for (i=0; IOAreas_[i].IODesc()!=NULL; i++)
      {
      if (dbgSwapIOAreas)
        dbgpln("SwapIO New %6i %s", IOAreas_[i].m_Id, IOAreas_[i].IOName());

      pNew[i].Duplicate(IOAreas_[i]);
      
      LastId=Max(LastId, pNew[i].m_Id);
      }
    for (int j=0; IOArea(j).IODesc()!=NULL; j++)
      if (InUse[j] & !Retained[j])
        {
        pNew[i].Duplicate(IOArea(j));
        pNew[i].m_Id=++LastId; // reassign Id - the value should not matter as it is not needed by model
        i++;
        }
    memset(&pNew[i], 0, sizeof(pNew[i]));
    m_pIOAreasOwned=pNew;

    for (i=0; i<m_nIOs; i++)
      {
      int DescNo=IODescNo_Self(i);
      ASSERT(NewDescPos[DescNo]>=0);

      if (dbgSwapIOAreas)
        dbgpln("IODesc %2i moved to %2i for %s", m_IOs[i].m_iIODescSelf, NewDescPos[DescNo], FullObjTag());
      //LogNote(FullObjTag(), 0, "IODesc %i moved to %i", m_IOs[i].m_iIODescSelf, NewDescPos[DescNo]);

      m_IOs[i].m_iIODescSelf=NewDescPos[DescNo];

      int &RmtIODescNo=Nd_Rmt(i)->m_IOs[IOIONo_Rmt(i)].m_iIODescRmt;
      RmtIODescNo=NewDescPos[RmtIODescNo];
      }
    }
  else
    {
    m_pIOAreas=IOAreas_;
    m_pIOAreasOwned=NULL;
    }

  if (dbgSwapIOAreas && IOAreas)
    {
    for (int i=0; IOArea(i).IODesc()!=NULL; i++)
      dbgpln("SwapIO === %6i %s", IOArea(i).m_Id, IOArea(i).IOName());
    }

  if (dbgSwapIOAreas)
    {
    for (int i=0; i<m_IOs.GetCount(); i++)
      dbgpln("IO %3i) %6i %6i %s", i, IODescNo_Self(i), IODescNo_Rmt(i), IODesc_Self(i)->IOName());
    dbgpln("---------------------");
    }

  if (p2Delete)
    {
    for (int i=0; p2Delete[i].IODesc()!=NULL; i++)
      p2Delete[i].PreDelete();
    delete [] p2Delete;
    p2Delete=NULL;
    }
  }

//--------------------------------------------------------------------------

void FlwNode::CheckIOAreas(bool OneGroup, CFlwEqnGrp * DefFlwEqnGrp0, CFlwEqnGrp * DefFlwEqnGrp1)
  {
  if (0)
    dbgpln("------- CheckIOAreas :%s", ClassId());

  IOAreaRec *p=IOAreas;
  IOAreaRec *pPrev=NULL;
  int i=0;
  while (p->IODesc()!=NULL)
    {
    ASSERT_ALWAYS(p->m_Id<MaxIOId, "IOId >= MaxIOId", __FILE__, __LINE__);
    if (p->m_dwType&nc_MLnk)
      ASSERT_ALWAYS(IsFlwIOId(p->m_Id), "Condition Not Met FlwIOId0 <= FlwIOId < FlwIOIdN", __FILE__, __LINE__);
    if (p->m_dwType&nc_CLnk)
      ASSERT_ALWAYS(IsCtrlIOId(p->m_Id), "Condition Not Met CtrlIOId0 <= CtrlIOId < CtrlIOIdN", __FILE__, __LINE__);
    if (p->m_dwType&nc_ELnk)
      ASSERT_ALWAYS(IsElecIOId(p->m_Id), "Condition Not Met ElecIOId0 <= ElecIOId < ElecIOIdN", __FILE__, __LINE__);
    if (p->m_dwType&nc_ALnk)
      ASSERT_ALWAYS(IsAirIOId(p->m_Id), "Condition Not Met AirIOId0 <= AirIOId < AirIOIdN", __FILE__, __LINE__);

    if (pPrev)
      {
      ASSERT_ALWAYS((p->m_Id>=pPrev->m_Id), "Ids Must Increase", __FILE__, __LINE__);

      if (p->m_dwType&nc_MLnk)
        {ASSERT_ALWAYS(((pPrev->m_dwType&(nc_CLnk|nc_ELnk|nc_ALnk))==0), "MLnks must Preceed C/E/ALnks", __FILE__, __LINE__);}
      else if (p->m_dwType&nc_CLnk)
        {ASSERT_ALWAYS(((pPrev->m_dwType&(nc_ELnk|nc_ALnk))==0), "CLnks must Preceed E/ALnks", __FILE__, __LINE__);}
      else if (p->m_dwType&nc_ELnk)
        {ASSERT_ALWAYS(((pPrev->m_dwType&(nc_ALnk))==0), "CLnks must Preceed ALnks", __FILE__, __LINE__);}
      else if (p->m_dwType&nc_ALnk)
        {}
      }

    ASSERT_ALWAYS(p->m_nIORqd<=p->m_nIOMax, "m_nIORqd > m_nIOMax", __FILE__, __LINE__);
    // No blanks in Names
    for (int n=0; p->IOName(n); n++)
      {
      char *pb;
      while ((pb=strchr(p->IOName(n), ' '))!=NULL)
        *pb='_';
      if (0)
        dbgpln("  %2i %2i %s", i, n, p->IOName(n));
      }
    p->m_nNameCount= n;

    p->m_fChecked = 1;
    p->m_fValid   = 1;
    
    if (p->m_pFlwGroup==NULL)
      {
      if (OneGroup)
        {
        if (p->m_pFlwGroup==NULL)
          p->m_pFlwGroup=DefFlwEqnGrp0 ? DefFlwEqnGrp0 : &NullFlwGroup;
        }
      else
        {
        switch (i)
          {
          case 0  : p->m_pFlwGroup=DefFlwEqnGrp0 ? DefFlwEqnGrp0 : &NullFlwGroup; break;
          case 1  : p->m_pFlwGroup=DefFlwEqnGrp1 ? DefFlwEqnGrp1 : &NullFlwGroup; break;
          default : p->m_pFlwGroup=&NullFlwGroup; break;
          }
        }
      }
    pPrev=p;
    p++;
    i++;
    };
  p->m_fValid=0;
  }

//--------------------------------------------------------------------------

void FlwNode::AttachIOAreas(IOAreaRec * IOAreas_, CFlwEqnGrp * DefFlwEqnGrp, bool Copy)
  {
  SwapIOAreas(IOAreas_, Copy);

  if (IOAreas && !IOAreas->m_fChecked)
    CheckIOAreas(true, DefFlwEqnGrp, NULL);

#if KWIKCONNECT
  if (IOAreas)
    {
    for (int i=0; IOAreas[i].IODesc(); i++)
      {}
    m_IOAreaLcl.SetSize(i);
    }
  else
    m_IOAreaLcl.SetSize(0);
#endif
  };

//--------------------------------------------------------------------------

void FlwNode::AttachIOAreas(IOAreaRec * IOAreas_, CFlwEqnGrp * DefFlwEqnGrp0, CFlwEqnGrp * DefFlwEqnGrp1, bool Copy)
  {
  SwapIOAreas(IOAreas_, Copy);

  if (IOAreas && !IOAreas->m_fChecked)
    CheckIOAreas(false, DefFlwEqnGrp0, DefFlwEqnGrp1);

#if KWIKCONNECT
  if (IOAreas)
    {
    for (int i=0; IOAreas[i].IODesc(); i++)
      {}
    m_IOAreaLcl.SetSize(i);
    }
  else
    m_IOAreaLcl.SetSize(0);
#endif
  };

//--------------------------------------------------------------------------

CFlange * FlwNode::GetFlange(int IoId)
  {
  for (int i=0; i<m_AttachedDirectIOs.GetCount(); i++)
    {
    CDirectFlwIO & DIO = *m_AttachedDirectIOs[i];
    if (DIO.LclDIOId() == IoId)
      {
      //dbgpln("FlwNode::GetFlange %5i %5i 0x%08x %s", IoId, DIO.m_iMyConnectedIO, &DIO.Flange, FullObjTag());
      return &DIO.Flange;
      }
    }
  return NULL;
  };

//--------------------------------------------------------------------------

void FlwNode::BuildDataDefnAreaIOs(DataDefnBlk & DDB)
  {
  flag Enabled=false;
  for (int i=0; !Enabled && i<m_AttachedDirectIOs.GetCount(); i++)
    Enabled=m_AttachedDirectIOs[i]->Enabled;

  if ((DDB.ForFileSnpScn() || NetDynamicMethod()) && Enabled)
    {
    DDB.Visibility();
    DDB.Page("DirectIO", DDB_RqdPage);
    //if (PrjFileVerNo()<107)
    //  {
    //  for (int i=0; i<m_AttachedDirectIOs.GetCount(); i++)
    //    m_AttachedDirectIOs[i]->BuildDataDefn(DDB, tt_Object, "DIO", DDB_NoPage, 99500+i, DFIO_ShowQm);
    //  }
    //else
      {
      if (DDB.BeginArray(this, "DIO", PrjFileVerNo()<107 ? "DirectedIO":"DirectIO", Joins.GetSize(), 0, DDB_NoPage))
        {
        for (int i=0; i<m_AttachedDirectIOs.GetCount(); i++)
          m_AttachedDirectIOs[i]->BuildDataDefn(DDB, tt_Element, NULL, DDB_NoPage, 99500+i, DFIO_ShowQm|DFIO_ShowSrcTag|DFIO_ShowDstTag);
        }
      }
    DDB.EndArray();
    }
  };

//--------------------------------------------------------------------------

void FlwNode::BuildDataDefnAreaIOs_OnOff(DataDefnBlk & DDB)
  {
  flag Enabled=false;
  for (int i=0; !Enabled && i<m_AttachedDirectIOs.GetCount(); i++)
    Enabled=m_AttachedDirectIOs[i]->Enabled;

  if (NetDynamicMethod() && Enabled)
    {
    DDB.Visibility();
    for (int i=0; i<m_AttachedDirectIOs.GetCount(); i++)
      m_AttachedDirectIOs[i]->Add_OnOff(DDB, 0,99500+i);
      //DDB.Page("DirectIO", DDB_RqdPage);
    //for (int i=0; i<m_AttachedDirectIOs.GetCount(); i++)
    //  m_AttachedDirectIOs[i]->BuildDataDefn(DDB, "DIO", NULL, DDB_NoPage, 99500+i, DFIO_ShowQm);
    }
  };

//--------------------------------------------------------------------------

void FlwNode::BuildCommonEndDefn(DataDefnBlk & DDB)
  {
#if WITHEQUIPSPECS
  if (CustomData())
    CustomData()->BuildDataDefnData(DDB, this);
#endif

//?
//     CTOCustomData::CDataSet &DataSet = *CustomDataSelected();
//    DDB.Page("CD", DDB_RqdPage);
//    Strng Tbl(Class()->ClassIdNoVersion());
//    Tbl+="_CD";
//    if (DDB.BeginObject(this, "CD", Tbl()))
//      {
//      for (int f=0; f<DataSet.FieldCount(); f++)
//        DataSet.Field(f).BuildDataDefn(DDB, this);
//      }
//    DDB.EndObject();
//    }
  
  BuildDataDefnAreaIOs(DDB);

  CTNode::BuildCommonEndDefn(DDB);
#if WITHNODEPROCS
  if (m_ProcsAllowed && m_ProcsReqd)
    m_pProcs->BuildDataDefn(DDB);
#endif
#if WITHNODETAGIO
  if (m_pTagIO)
    m_pTagIO->BuildDataDefn(DDB);
#endif
  }

//--------------------------------------------------------------------------

flag FlwNode::DataXchgDefnAreaIOs(DataChangeBlk & DCB)
  {
  for (int i=0; i<m_AttachedDirectIOs.GetCount(); i++)
    {
    if (DCB.dwUserInfo==99500+i)
      return m_AttachedDirectIOs[i]->DataXchg(DCB);
    }
  return 0;
  };

//--------------------------------------------------------------------------

flag FlwNode::ValidateDataAreaIOs(ValidateDataBlk & VDB)
  {
  bool OK =true;
  for (int i=0; i<m_AttachedDirectIOs.GetCount(); i++)
    if (!m_AttachedDirectIOs[i]->ValidateData(VDB))
      OK=false;
  return OK;  
  };

//--------------------------------------------------------------------------

void FlwNode::AddPwrUser(CPwrUser*pPwr)
  {
  for (int i=0; i<m_PwrUsers.GetSize(); i++)
    if (m_PwrUsers[i]==pPwr)
      return;
  m_PwrUsers.Add(pPwr);;
  };

//--------------------------------------------------------------------------

void FlwNode::RemovePwrUser(CPwrUser*pPwr)
  {
  for (int i=0; i<m_PwrUsers.GetSize(); i++)
    if (m_PwrUsers[i]==pPwr)
      {
      m_PwrUsers.RemoveAt(i);
      return;
      }
  };

//--------------------------------------------------------------------------

void FlwNode::MakeAreaTag()
  {
  m_sAreaTag.LRTrim();

  if (m_sAreaTag.GetLength()>0)
    m_pArea.m_sTag=m_sAreaTag;
  else
    m_pArea.m_sTag=gs_DirectFlwIOTagDefault;
  }

//--------------------------------------------------------------------------

void FlwNode::SetAreaTag(LPCTSTR Tag)
  {
  if (Tag)
    {
    m_sAreaTag=Tag;
    m_sAreaTag.LRTrim();
    if (m_sAreaTag.GetLength()>0 && m_sAreaTag[0]=='<' && m_sAreaTag[m_sAreaTag.GetLength()-1]=='>')
      m_sAreaTag="";
    }
  }

LPCTSTR FlwNode::GetAreaTagDisp()
  {
  if (m_sAreaTag.GetLength()>0)
    m_sAreaDisp=m_sAreaTag();
  else
    m_sAreaDisp.Set("<%s>", gs_DirectFlwIOTagDefault());
  return m_sAreaDisp();
  }

//--------------------------------------------------------------------------

void FlwNode::DoGlobalDisconnect()
  {
  FlwNode * p=Parent();
  while (p)
    {
    CFlwSolver *pFS=dynamic_cast<CFlwSolver*>(p);
    if (pFS)
      {
      pFS->UnDoGlobalLinks();
      break;
      }
    p=p->Parent();
    }
  };

//--------------------------------------------------------------------------

void FlwNode::DoGlobalConnect()
  {
  FlwNode * p=Parent();
  while (p)
    {
    CFlwSolver *pFS=dynamic_cast<CFlwSolver*>(p);
    if (pFS)
      {
      //pFS->DoGlobalLinks();
      break;
      }
    p=p->Parent();
    }
  };

//--------------------------------------------------------------------------

void FlwNode::GlobalConnect()
  {
  flag AreaOK=true;
  flag AuditOK=true;
  int BadDirects=0;
  int BadPower=0;
  if (!GetActive())
    goto Done;

  for (int io=0; io<NoProcLnkIOs(); io++)
    IOFlange(io)->FixTag();

  MakeAreaTag();
  if (m_pArea.Connect(this, -1, "Area"))
    {
    if (m_pArea->IsValidAuditNode())
      {
      //dbgpln("Add Nd %#010x %s %s", this, typeid(this).name(), FullObjTag());
      m_AreaNdListPos=m_pArea->m_NdsInArea.AddTail(this);
      }
    else
      {
      m_pArea.DisConnect();
      //LogError(FullTag(), 0, "%s Not an Audit Node", m_pArea.m_sTag());
      AuditOK=false;
      }
    }
  else
    {
    //LogError(FullTag(), 0, "%s NOT Found", m_pArea.m_sTag());
    AreaOK=false;
    }

  int iEnabled=0;
  m_nMkupDirectIOs=0;
  m_nProcDirectIOs=0;
  for (int d=0; d<m_AttachedDirectIOs.GetSize(); d++)
    {
    CDirectFlwIO *pDIO=m_AttachedDirectIOs[d];
    if (pDIO->GlobalConnect())
      {
      if (pDIO->Enabled)
        {
        m_EnabledDirectIOs.SetAtGrow(iEnabled++, pDIO);
        switch (pDIO->m_eType)
          {
          case eDIO_Spill:
          case eDIO_Vent:
          case eDIO_Leak:
            m_nProcDirectIOs++;
            break;
          case eDIO_Makeup:
          case eDIO_Bleed:
            m_nMkupDirectIOs++;
            break;
          //case eDIO_Src:
          //case eDIO_Snk:
          //case eDIO_Accum:
          //case eDIO_Deplete:
          default:
            break;
          }
        }
      }
    else
      BadDirects++;
    }
  m_EnabledDirectIOs.SetSize(iEnabled);

  for (int d=0; d<m_EnabledDirectIOs.GetSize(); d++)
    m_EnabledDirectIOs[d]->GlobalConnectFinal();


  //POSITION Pos=m_DirectIOs.GetHeadPosition();
  //while (Pos)
  //  if (!m_DirectIOs.GetNext(Pos)->GlobalConnect())
  //    OK=false;
  //Pos=m_DirectIOs.GetHeadPosition();
  //while (Pos)
  //  m_DirectIOs.GetNext(Pos)->GlobalConnectFinal();

  for (int i=0; i<m_PwrUsers.GetSize(); i++)
    if (!m_PwrUsers[i]->GlobalConnect(this, NULL))
      BadPower++;

Done:
  if (AuditOK)
    ClrCI(30);
  else
    SetCI(30, "W\t%s NOT an Audit Node", m_pArea.m_sTag());

  if (AreaOK)
    ClrCI(31);
  else
    SetCI(31, "W\tArea %s NOT Found", m_pArea.m_sTag());

  if (BadDirects==0)
    ClrCI(32);
  else
    SetCI(32, "W\t%i Bad Direct connection(s)", BadDirects);

  if (BadPower==0)
    ClrCI(33);
  else
    SetCI(33, "W\t%i Bad Power connection(s)", BadPower);


  };

//--------------------------------------------------------------------------

void FlwNode::PostGlobalConnect()
  {
  flag OK=true;
  if (!GetActive())
    return ;

  //dbgpln("PostGlobalConnect %s", FullObjTag());

  if (OK)
    {
    for (int i=0; i<NoFlwIOs(); i++)
      {
      //FlwNode * pRmtNd=Nd_Rmt(i);
      byte S=IODesc_Self(i)->m_Dirn;
      byte D=IODesc_Rmt(i)->m_Dirn;
      bool SIn  = (S & (LIO_In | LIO_Out)) == LIO_In;
      bool SOut = (S & (LIO_In | LIO_Out)) == LIO_Out;
      bool DIn  = (D & (LIO_In | LIO_Out)) == LIO_In;
      bool DOut = (D & (LIO_In | LIO_Out)) == LIO_Out;
      int Dirn=0;
      if (SIn || DOut)
        Dirn=1;
      else if (SOut || DIn)
        Dirn=-1;
      else
        ASSERT_ALWAYS(FALSE, "Connect Direction undefined", __FILE__, __LINE__);

      for (int j=0; j<NIOFBs(i); j++)
        {
        IOFB(i, j)->m_iDirnRel2Connect = Dirn;
        //dbgpln("  IO  %2i fb[%4i] = %+i %s", i, IOFB(i, j)->m_iNo, Dirn, Nd_Rmt(i)->FullObjTag());
        }

      //if (Nd_Rmt(i))
      //  Nd_Rmt(i)->SetHasModeNear(FlowMode());
      }

    for (int i=0; i<NoLinks(); i++)
      {
      CLinkRec &L=*Link(i);
      LinkFB(i, 0)->m_iDirnRel2Connect=1;
      //dbgpln("  Lnk %2i fb[%4i] = %+i ", i, LinkFB(i, 0)->m_iNo, 1);
      }
    }
  };

//--------------------------------------------------------------------------

void FlwNode::GlobalDisConnect()
  {
  for (int i=0; i<m_PwrUsers.GetSize(); i++)
    m_PwrUsers[i]->GlobalDisConnect();

  for (int d=0; d<m_EnabledDirectIOs.GetSize(); d++)
    m_EnabledDirectIOs[d]->GlobalDisConnect();
  for (int d=0; d<m_EnabledDirectIOs.GetSize(); d++)
    m_EnabledDirectIOs[d]->GlobalConnectFinal();
  //POSITION Pos=m_DirectIOs.GetHeadPosition();
  //while (Pos)
  //  m_DirectIOs.GetNext(Pos)->GlobalDisConnect();
  //Pos=m_DirectIOs.GetHeadPosition();
  //while (Pos)
  //  m_DirectIOs.GetNext(Pos)->GlobalConnectFinal();

  if (m_pArea.Connected())
    {
    //POSITION Pos=m_pArea->m_NdsInArea.GetCount()>0 ? m_pArea->m_NdsInArea.Find(this) : NULL;
    ASSERT_ALWAYS(m_AreaNdListPos!=0, "Node must be in Area", __FILE__, __LINE__);
    //dbgpln("Rem Nd %#010x %s", this, typeid(this).name());
    m_pArea->m_NdsInArea.RemoveAt(m_AreaNdListPos);
    m_pArea.DisConnect();
    m_AreaNdListPos=NULL;
    }
  };

//--------------------------------------------------------------------------

void FlwNode::PreGlobalDisConnect()
  {
  }

//--------------------------------------------------------------------------

static char * Active_Str        = "Active";
static char * ActiveHold_Str    = "Active-Hold";
static char * Inactive_Str      = "Inactive";
static char * InactiveHold_Str  = "Inactive-Hold";
static char * PB_Str            = "ProBal";
static char * SS_Str            = "SteadyState";
static char * DYN_Str           = "Dynamic";
static char * PBSS_Str          = "ProBal_SS";

static DDBValueLst SolutionStrs[]={
  {-1, PB_Str },
  {-1, DYN_Str},
  {-1, Inactive_Str},
  {0}};

static DDBValueLst SolutionStrsR[]={
  {-1, PB_Str },
  {-1, DYN_Str},
  {-1, Inactive_Str},
  {0}};

static DDBValueLst SolutionStrsN[]={
  {-1, Inactive_Str},
  {-1, InactiveHold_Str},
  {-1, Active_Str},
  {-1, ActiveHold_Str},
  {0}};

void FlwNode::BuildCommonStartDefn(DataDefnBlk & DDB)
  {
  CTNode::BuildCommonStartDefn(DDB);

  //DDB.Text(" ");
  LPTSTR AreaTag=PrjFileVerNo()<96 ? "Location":"PlantArea";
  DDB.String(AreaTag,       "", DC_,     "",   xidFlwNdRqdArea,  this, isParmStopped|InitHidden);

  DDB.Double ("Origin.X",   "", DC_L,    "m",  &m_Origin.m_X,    this, isParmStopped|InitHidden);
  DDB.Double ("Origin.Y",   "", DC_L,    "m",  &m_Origin.m_Y,    this, isParmStopped|InitHidden);
  DDB.Double ("Origin.Z",   "", DC_L,    "m",  &m_Origin.m_Z,    this, isParmStopped|InitHidden);
  DDB.Text(" ");

  //GetGlblModeValueLst(DDBR, GetPermissableRunModes(SM_All));
  //if (m_DDBSolveModes.Length()>0)
  //  {
  //  DDBValueLstMem DDBR;
  //  for (int i=0; i<m_DDBSolveModes.Length(); i++)
  //    {
  //    DDBValueLst &V=*m_DDBSolveModes.Item(i);
  //    if (V.m_lVal & GetPermissableRunModes(SM_All))//GlblSolveMode())
  //      DDBR.Add(V.m_lVal & SM_All, V.m_pStr, V.m_dwFlags);
  //    }
  //  DDBR.Add(0, "None", MDD_Hidden);
  //  DDB.Long("SolveMethod", "", DC_, "", xidSolveMode,   this, isParmStopped|SetOnChange, &DDBR);
  //  }

  if (m_AllowedModes & NM_All)
    {
    //DDBValueLstMem DDBR;
    //if (NM_Probal & GetPermissableRunModes(NM_All))
    //  DDBR.Add(NM_Probal, "ProBal");
    //if (NM_Dynamic & GetPermissableRunModes(NM_All))
    //  DDBR.Add(NM_Dynamic, "Dynamic");
    //if (m_AllowedModes & SM_Inline & GetPermissableRunModes(SM_All))
    //  DDBR.Add(SM_Inline, "Inline");
    //if (m_AllowedModes & SM_Buffered & GetPermissableRunModes(SM_All))
    //  DDBR.Add(SM_Buffered, "Buffered");
    //DDBR.Add(0, "None", MDD_Hidden);
    DDBValueLstMem DDBModes;
    GetGlblModeValueLst(DDBModes, GetPermissableModes(m_AllowedModes&NM_All, this), DefNetMode(), false);
    // This could opened up 
    DDB.String("NetMethod", "", DC_, "", xidNetMethod,   this, (/*DDBR.Length()>2?isParmStopped:*/0)|SetOnChange);
    }

  if ((m_AllowedModes & SM_All) && NetMethod()==NM_Dynamic)
    {
    DDBValueLstMem DDBModes;
    GetGlblModeValueLst(DDBModes, GetPermissableModes(m_AllowedModes&SM_All, this), m_bNodeSolveMode ? DefNodeSolveMode(): DefLinkSolveMode(), false);
    DDB.String("Solution", "", DC_, "", xidSolveMethod,   this, (DDBModes.Length()>2?isParmStopped:0)|SetOnChange, &DDBModes);
    }

  if (m_AllowedModes & HM_All)
    {
    DDBValueLstMem DDBModes;
    GetGlblModeValueLst(DDBModes, GetPermissableModes(m_AllowedModes&HM_All, this), DefHeatMode(), false);
    DDB.String("HeatCalc", "", DC_, "", xidHeatMethod,   this, (DDBModes.Length()>2?isParmStopped:0)|SetOnChange, &DDBModes);
    }

  if ((m_AllowedModes & LFM_All) && NetMethod()==NM_Dynamic)
    {
    if (PrjFileVerNo()>=100)
      {
      DDBValueLstMem DDBModes;
      GetGlblModeValueLst(DDBModes, GetPermissableModes(m_AllowedModes&LFM_All, this), DefFlowMode(), false);
      DDB.String("FlowCalc",    "", DC_, "", xidFlowMode,   this, (DDBModes.Length()>2?isParmStopped:0)|SetOnChange, &DDBModes);
      DDB.String("FlowCalcRqd", "", DC_, "", xidFlowModeRqd,   this, (DDBModes.Length()>2?isParmStopped:0)|SetOnChange, &DDBModes);
      //if (FlowModeObj())
      DDB.String("FlowCalcSet", "", DC_, "", xidFlowModeSet, this, isTag);
      }
    else
      {
      DDBValueLstMem DDBModes;
      GetGlblModeValueLst(DDBModes, GetPermissableModes(m_AllowedModes&LFM_All, this), DefFlowMode(), false);
      if (PrjFileVerNo()<94)
        DDB.String("UserMethod", "", DC_, "", xidFlowMode, this, isParmStopped, &DDBModes);
      else
        DDB.String("FlowMode",   "", DC_, "", xidFlowMode, this, isParmStopped, &DDBModes);
      }
    }

#if WITHNODEPROCS
  if (m_ProcsAllowed)
    {
    DDB.CheckBoxBtn("Procedures", "", DC_, "", xidProcsEnable,  this, isParmStopped|SetOnChange);
    //DDB.Byte("Procedures", "", DC_, "", xidProcsEnable,  this, isParmStopped|SetOnChange, DDBOnOff);
    }
#endif

  if (!fSolutionForRoot)
    {
    DDBValueLstMem DDBModes;
    GetAuditOptionStrLst(DDBModes, true);
    DDB.String("AuditLevel",      "", DC_, "", xidAuditReqd,   this, (DDBModes.Length()>1?isParmStopped:0)|SetOnChange|InitHidden, &DDBModes);
    }

#if WITHSPECSHEETS
  DDB.String("SpecSheet",  "", DC_, "", xidSpecSheet,     this, isParm);
#endif


  DDBValueLst * pVL;
  if (PrjFileVerNo()<74)
    {
    if (PrjFileVerNo()<43)
      pVL=SolutionStrs;
    else if (fSolutionForRoot)
      pVL=SolutionStrsR;
    else
      pVL=SolutionStrsN;

    DDB.String      ("Solution",     "", DC_, "", xidSysCADSolutionOld, this, 0/*InitHidden*/, pVL);
    }

  if (!fSolutionForRoot)
    {
    DDB.CheckBoxBtn ("Active", "", DC_, "", xidActiveMode, this, isParmStopped);
    DDB.CheckBoxBtn ("Hold",   "", DC_, "", xidHoldMode,   this, isParm);
    }

  DDB.CheckBoxBtn ("Marked", "", DC_, "", xidMarked,  this, isParm|noFileAtAll|InitHidden);
  if (!fSolutionForRoot)
    {
    DDB.String      ("EvalOrder",    "", DC_, "", xidEvalOrder,  this, InitHidden);
    }


  DDB.String      ("Condition",      "", DC_, "", xidNdCondition,       this, MultiLineStr|noFile|noSnap);
  DDB.Long        ("ConditionCount", "", DC_, "", xidNdConditionCount,  this, noFile|noSnap);

  //DDB.CheckBoxBtn ("ShowPressNet", "", DC_, "", xidShwPrsNet,  this, isParm|noFileAtAll|InitHidden);
  //DDB.CheckBoxBtn ("ShowFlowNet",  "", DC_, "", xidShwFlwNet,  this, isParm|noFileAtAll|InitHidden);
  if (DDB.DoingGetData())
    {
    int n=CountAllCIs(1000);
    if (n>1)
      {
      Strng S;
      S.Set("%i", n);
      DDB.TagComment(S());
      }
    }
  if (!fSolutionForRoot)
    {
    DDB.Bool("SetState",           "", DC_, "", xidSetState,   this, isParm|noFileAtAll|InitHidden, SetStateStrs);
    /*kga I don't believe  we need this in all models...
    DDB.Button("Cmd.ZeroFlows",         "", DC_, "", xidSetStateZeroFlows,        this, isParm|noFileAtAll|InitHidden);
    DDB.Button("Cmd.Empty",             "", DC_, "", xidSetStateEmpty,            this, isParm|noFileAtAll|InitHidden);
    DDB.Button("Cmd.Preset",            "", DC_, "", xidSetStatePreSet,           this, isParm|noFileAtAll|InitHidden);
    DDB.Button("Cmd.SteadyState",       "", DC_, "", xidSetStateSteadyState,      this, isParm|noFileAtAll|InitHidden);
    DDB.Button("Cmd.PBInit",            "", DC_, "", xidSetStatePBInit,           this, isParm|noFileAtAll|InitHidden);
    DDB.Button("Cmd.DynStatsRunInit",   "", DC_, "", xidSetStateDynStatsRunInit,  this, isParm|noFileAtAll|InitHidden);
    DDB.Button("Cmd.EmptySpillTargets", "", DC_, "", xidSetStateEmptySpillTargets,this, isParm|noFileAtAll|InitHidden);
    */
    }    

#if WITHEQUIPSPECS
  if (SelectData())
    SelectData()->BuildDataDefnHead(DDB, this);

  if (CustomData())
    CustomData()->BuildDataDefnHead(DDB, this);
#endif
  }

//--------------------------------------------------------------------------

void FlwNode::BuildSystemDefn_FB(bool IsLink, LPTSTR Tg, int i, int fe, DWORD UI, FlwBlk & FB, DataDefnBlk & DDB)
  {
  if (DDB.BeginElement(this, Tg,NULL,UI, DDB_OptPage,MarkerClosed))
    {
    DDB.Double ("Qm",             "", DC_Qm,    "kg/s",       &FB.m_Qm,               this, 0);
    DDB.Double ("QmEst",          "", DC_Qm,    "kg/s",       &FB.m_QmEst,            this, 0);
    DDB.Double ("QmEstGood",      "", DC_Qm,    "kg/s",       &FB.m_QmEstGood,        this, 0|InitHidden);
    DDB.Double ("QmAvail",        "", DC_Qm,    "kg/s",       &FB.m_QmAvail,          this, 0|InitHidden);
    DDB.Double ("QmSpace",        "", DC_Qm,    "kg/s",       &FB.m_QmSpace,          this, 0|InitHidden);
    DDB.Double ("QmPrvPB",        "", DC_Qm,    "kg/s",       &FB.m_QmPrvPB,          this, 0|InitHidden);
    //DDB.Double ("QmMemPB",        "", DC_Qm,    "kg/s",       &FB.QmMemPB,          this, 0|InitHidden);
    DDB.Double ("mQm",            "", DC_Qm,    "kg/s",       &FB.m_QmMeas,           this, 0|InitHidden);
    DDB.Double ("mQmLo",          "", DC_Qm,    "kg/s",       &FB.m_MeasLoLimit,    this, 0|InitHidden);
    DDB.Byte   ("PropSelRqd",     "", DC_,      "",           &FB.m_PropSel.m_iReqd,  this, 0);
    DDB.Byte   ("PropSelUse",     "", DC_,      "",           &FB.m_PropSel.m_iInUse, this, 0);
    //DDB.Double ("VFrac",          "", DC_Frac,  "%",          &FB.FillC.dVFrac,       this, 0);
    //DDB.Double ("LFrac",          "", DC_Frac,  "%",          &FB.FillC.dLFrac,       this, 0);
    DDB.Double ("PNode",          "", DC_P,     "kPa",        FB.NodePB().PAdd(),         this, 0);
    DDB.Double ("MnPNode",        "", DC_P,     "kPa",        FB.NodePB().PMnAdd(),       this, 0);
    //DDB.Float  ("PMaxNode",       "", DC_P,     "kPa",        &FB.NodePB().PMax,      this, 0|NAN_OK);
    DDB.Double ("PFlng",          "", DC_P,     "kPa",        FB.FlngPB().PAdd(),         this, 0);
    DDB.Double ("MnPFlng",        "", DC_P,     "kPa",        FB.FlngPB().PMnAdd(),       this, 0);

    //TODO Previous Versions
    if (DDB.BeginStruct(this, "DP", NULL, DDB_NoPage, -1, MarkerClosed))
      {
      DDB.Double ("dPb",            "", DC_DP,    "kPa",        &FB.m_dPb,            this, 0);
      DDB.Double ("dPbX",           "", DC_DP,    "kPa",        &FB.m_dPbX,           this, 0);
      DDB.Double ("dPbdQ",          "", DC_DP,    "kPa",        &FB.m_dPbdQ,          this, 0|InitHidden);
      DDB.Double ("dPq",            "", DC_DP,    "kPa",        &FB.m_dPq,            this, 0);
      DDB.Double ("dPqdQ",          "", DC_DP,    "kPa",        &FB.m_dPqdQ,          this, 0|InitHidden);
      DDB.Double ("dPa",            "", DC_DP,    "kPa",        &FB.m_dPa,            this, 0);
      DDB.Double ("dPadQ",          "", DC_DP,    "kPa",        &FB.m_dPadQ,          this, 0|InitHidden);
      DDB.Double ("dPz",            "", DC_DP,    "kPa",        &FB.m_dPz,            this, 0);
      DDB.Double ("dPzI",           "", DC_DP,    "kPa",        &FB.m_dPzI,           this, 0);
      DDB.Double ("dPzdQ",          "", DC_DP,    "kPa",        &FB.m_dPzdQ,          this, 0|InitHidden);
      DDB.Double ("dPzX",           "", DC_DP,    "kPa",        &FB.m_dPzX,           this, 0);
      DDB.Double ("dPzXdQ",         "", DC_DP,    "kPa",        &FB.m_dPzXdQ,         this, 0|InitHidden);
      DDB.Double ("dPmB",           "", DC_DP,    "kPa",        &FB.m_dPmB,           this, 0);
      DDB.Double ("dPmQ",           "", DC_DP,    "kPa",        &FB.m_dPmQ,           this, 0);
      DDB.Double ("dPmdQ",          "", DC_DP,    "kPa",        &FB.m_dPmdQ,          this, 0|InitHidden);
      DDB.Double ("ResT",           "", DC_,      "",           &FB.m_ResT,           this, 0);
      DDB.Double ("ResFM",          "", DC_,      "",           &FB.m_SimpleRes,          this, 0);
//                DDB.Double ("ResQ",           "", DC_,      "",           &FB.ResQ,             this, 0);
      }
    DDB.EndStruct();

    DDB.Double ("MomentScl",      "", DC_Frac,  "%",          &FB.m_MomentScl,        this, 0|InitHidden);
    DDB.Double ("xVel",           "", DC_Ldt,   "m/s",        &FB.m_Vel,              this, 0|NAN_OK);
    DDB.Double ("VelSonic",       "", DC_Ldt,   "m/s",        &FB.m_VelSonic,         this, 0|NAN_OK);
    //DDB.Bool   ("VFlowAllowed",   "", DC_,      "",           &FB.xm_fVFlowAllowed,    this, 0|InitHidden);
    //DDB.Bool   ("LFlowAllowed",   "", DC_,      "",           &FB.xm_fLFlowAllowed,    this, 0|InitHidden);
    DDB.Double ("VThrottle",      "", DC_Frac,  "%",          &FB.dVThrottle,         this, 0|InitHidden);
    DDB.Double ("LThrottle",      "", DC_Frac,  "%",          &FB.dLThrottle,         this, 0|InitHidden);

//              DDB.Double ("OvrTau",         "", DC_Time, "s",           &FB.xm_dOverideTau,     this,  isParm|NAN_OK);
    DDB.Double ("OvrRhoL",        "", DC_Rho,   "kg/m^3",     &FB.m_dOverideRhoL,     this,  isParm|InitHidden|NAN_OK);
    DDB.Double ("OvrRhoV",        "", DC_Rho,   "kg/m^3",     &FB.m_dOverideRhoV,     this,  isParm|InitHidden|NAN_OK);
    DDB.Double ("OvrTemp",        "", DC_T,     "C",          &FB.m_dOverideTemp,     this,  isParm|InitHidden|NAN_OK);
    DDB.Double ("OvrVisc",        "", DC_Visc,  "cP",         &FB.m_dOverideVisc,     this,  isParm|InitHidden|NAN_OK);

//TODO Previous Versions
    for (int ip=0; ip<3; ip++)
      {
      if (ip==1 && PrjFileVerNo()<61 ||
          ip==2 && ((fe!=0) ||IsLink))
        continue;

      LPTSTR Tags0[] = {"FP", "", "NP"};
      LPTSTR Tags1[] = {"FwdP", "RevP", "NetP"};
      if (DDB.BeginStruct(this, PrjFileVerNo()<61 ? Tags0[ip]:Tags1[ip], NULL, DDB_NoPage, -1, MarkerClosed))
        {
        CSpPropInfo *Props=NULL;
        if (IsLink)
          {
          switch (ip)
            {
            case 0: Props=LinkFwdFlwProps(i,fe); break;
            case 1: Props=LinkRevFlwProps(i,fe); break;
            case 2: Props=NULL; break;
            }
          }
        else
          {
          switch (ip)
            {
            case 0: Props=IOFwdFlwProps(i,fe); break;
            case 1: Props=IORevFlwProps(i,fe); break;
            case 2: Props=IONetProps(i); break;
            }
          }
        if (PrjFileVerNo()>=94) // allow projects to reinitialise
          {
          DDB.Bool   ("Valid",          "", DC_,      "",           &Props->m_bValid,      this, 0/*isParm*/);
          DDB.Double ("LiqVol",         "", DC_Frac,  "%",          &Props->m_dLVolFrac,   this, 0/*isParm*/);
          DDB.Double ("VapVol",         "", DC_Frac,  "%",          &Props->m_dVVolFrac,   this, 0/*isParm*/);
          if (PrjFileVerNo()<56)
            {
            double VFrac=0;
            DDB.Double ("VapVolBal",    "", DC_Frac,  "%",          &VFrac,this, 0/*isParm*/|noFileAtAll);
            if (DDB.DoingPutData())
              {
              Props->m_dLVolFracBal=-VFrac;
              Props->m_dLVolFrac=1-Props->m_dVVolFrac;
              }
            }
          else
            DDB.Double ("LiqVolBal",    "", DC_Frac,  "%",          &Props->m_dLVolFracBal,this, 0/*isParm*/|noFileAtAll);
          }
        DDB.Double ("RhoL",           "", DC_Rho,   "kg/m^3",     &Props->m_dRhoL,       this, 0/*isParm*/);
        DDB.Double ("RhoV",           "", DC_Rho,   "kg/m^3",     &Props->m_dRhoV,       this, 0/*isParm*/);
        DDB.Double ("Temp",           "", DC_T,     "C",          &Props->m_dTemp,       this, 0/*isParm*/);
        DDB.Double ("Press",          "", DC_P,     "kPa",        &Props->m_dPress,      this, 0/*isParm*/);
        DDB.Double ("Visc",           "", DC_Visc,  "cP",         &Props->m_dVisc,       this, 0/*isParm*/);
        }
      DDB.EndStruct();
      }

//              DDB.Double ("Fill",           "", DC_Frac,  "%",          &FB.FillC.dFill, this, isParm);
    DDB.Double ("SlipRatio",      "", DC_,      "",           &FB.dSlip,       this, isParm|NAN_OK|InitHidden);
    DDB.Double ("VoidFraction",   "", DC_Frac,  "%",          &FB.dVoidFrac,   this, isParm|NAN_OK|InitHidden);
    DDB.Double ("OnePhPart",      "", DC_Frac,  "%",          &FB.dOnePhPart,  this, isParm|NAN_OK|InitHidden);
    DDB.Double ("TempOut",        "", DC_T,     "C",          &FB.dTempOut,    this, isParm|NAN_OK|InitHidden);

//              DDB.Double("MeanTemp",        "", DC_T,     "C",          xidFBMeanTemp,            this, 0);
//              DDB.Double("MeanPress",       "", DC_P,     "kPa",        xidFBMeanPress,           this, 0);
//              DDB.Double("MeanRho",         "", DC_Rho,   "kg/m^3",     xidFBMeanRho,             this, 0);
    DDB.Double("Rise",            "", DC_L,     "m",          xidFBRise,                this, 0);
    DDB.Double("RqdLen",          "", DC_L,     "m",          xidFBRqdLen,              this, 0);
    DDB.Double("ActLen",          "", DC_L,     "m",          xidFBActLen,              this, 0);
    DDB.Double("FitLen",          "", DC_L,     "m",          xidFBFitLen,              this, 0);
    }
  }

void FlwNode::BuildSystemDefn(DataDefnBlk & DDB)
  {
  CTNode::BuildSystemDefn(DDB);
  DDB.Long("NdIOs", "", DC_, "", xidNdNIOs, this, isParmConstruct|DDEF_NOCOMPARE);
  DDB.Long("NdJoins", "", DC_, "", xidNdNJoins, this, isParmConstruct|DDEF_NOCOMPARE);
  if (Joins.GetSize()>0)
    {
    flag DoBlk;
    if (PrjFileVerNo()<22)
      DoBlk=DDB.BeginArray(this, "Jn", "NdJoins", Joins.GetSize(), 0, DDB_NoPage);
    else
      DoBlk=DDB.BeginArray(this, "Join", "NdJoins", Joins.GetSize(), 0, DDB_NoPage);
    if (DoBlk)
      for (int j=0; j<Joins.GetSize(); j++)
        if (DDB.BeginElement(this, j, NULL, j, DDB_OptPage, MarkerClosed))
          {
          DDB.String("NetInfo",     "",   DC_,    "",       xidJoinNetInfo,          this, isParmConstruct);
          DDB.Double("P",           "",   DC_P,   "kPa",    Joins[j].m_PB.PAdd(),        this, isParmConstruct);
          DDB.Double("PGood",       "",   DC_P,   "kPa",    Joins[j].m_PB.PGoodAdd(),    this, isParmConstruct|InitHidden);
          DDB.Double("PEst",        "",   DC_P,   "kPa",    Joins[j].m_PB.PEstAdd(),     this, isParmConstruct|InitHidden);
          DDB.Double("QmVAcc",      "",   DC_Qm,  "kg/s",   &Joins[j].dQmVAcc,       this, isParmConstruct);
          DDB.Double("QmLAcc",      "",   DC_Qm,  "kg/s",   &Joins[j].dQmLAcc,       this, isParmConstruct);
          DDB.Double("QmV2L",       "",   DC_Qm,  "kg/s",   &Joins[j].dQmV2L,        this, isParmConstruct);
          DDB.Double("QmVSink",     "",   DC_Qm,  "kg/s",   &Joins[j].dQmVSink,      this, isParmConstruct|InitHidden);
          DDB.Double("QmLSink",     "",   DC_Qm,  "kg/s",   &Joins[j].dQmLSink,      this, isParmConstruct|InitHidden);
          DDB.Double("V2LQmReqd",   "",   DC_Qm,  "kg/s",   &Joins[j].dV2LQmReqd,    this, isParmConstruct|InitHidden);
          DDB.Double("PressEquil",  "",   DC_Qm,  "kg/s",   &Joins[j].dPressEquil,   this, isParmConstruct|NAN_OK|InitHidden);
          }
    DDB.EndArray();
    }

  if (NoFlwIOs()>0)
    {
    flag DoBlk;
    if (PrjFileVerNo()<22)
      DoBlk=DDB.BeginArray(this, "FB", "NdFBS", NoFlwIOs()+1, 0, DDB_OptPage);
    else
      DoBlk=DDB.BeginArray(this, "Flng", "NdFlngs", NoFlwIOs()+1, 0, DDB_OptPage);
    if (DoBlk)
      for (int i=0; i<NoFlwIOs(); i++)
        //for (int fe=0; fe<NIOFBs(i); fe++)
        if (Nd_Rmt(i))
          {
          Strng Tg;
          MAKEIOTG4DDB(Tg, i, 0);

          DWORD UI=UIIOFlag+i;
          if (DDB.BeginElement(this, Tg(),NULL,UI, DDB_OptPage,MarkerClosed))
            {
            DDB.Double("P",           "",   DC_P,   "kPa",    IOPB_Self(i).PAdd(),           this, isParmConstruct);
            DDB.Double("PGood",       "",   DC_P,   "kPa",    IOPB_Self(i).PGoodAdd(),       this, isParmConstruct|InitHidden);
            DDB.Double("PEst",        "",   DC_P,   "kPa",    IOPB_Self(i).PEstAdd(),        this, isParmConstruct|InitHidden);

            DDB.Double("PSensV",      "",   DC_,    "",       &m_IOs[i].dPSensV,           this, isParmConstruct|InitHidden);
            DDB.Double("PSensL",      "",   DC_,    "",       &m_IOs[i].dPSensL,           this, isParmConstruct|InitHidden);

            DDB.Double("RhoHSensV",   "",   DC_,    "",       &m_IOs[i].dRhoHSensV,        this, isParmConstruct|InitHidden);
            DDB.Double("RhoHSensL",   "",   DC_,    "",       &m_IOs[i].dRhoHSensL,        this, isParmConstruct|InitHidden);

            DDB.Double("V2LQmVFact",  "",   DC_,    "",       &m_IOs[i].dV2LQmVFact,       this, isParmConstruct|InitHidden);
            DDB.Double("V2LQmLFact",  "",   DC_,    "",       &m_IOs[i].dV2LQmLFact,       this, isParmConstruct|InitHidden);

            DDB.String("SeqStartLcl", "",   DC_,    "",       xidIOSeqStartOK,           this, 0);
            DDB.String("SeqStartRmt", "",   DC_,    "",       xidIORmtSeqStartOK,        this, 0);
            DDB.String("SeqStart",    "",   DC_,    "",       xidIOSeqStart,             this, 0);

            DDB.Double("MkUpRqd",    "",   DC_Qm,  "kg/s",   &m_IOs[i].pFlng->dMkUpRqd,   this, isParmConstruct|InitHidden|NAN_OK);
            DDB.Double("MkUpRqdLcl", "",   DC_Qm,  "kg/s",   &m_IOs[i].pFlng->dMkUpRqdLcl,this, isParmConstruct|InitHidden|NAN_OK);
            DDB.Double("MkUpAvl",    "",   DC_Qm,  "kg/s",   &m_IOs[i].pFlng->dMkUpAvl,   this, isParmConstruct|InitHidden|NAN_OK);
            }
          }
    DDB.EndArray();
    }

  if (PrjFileVerNo()>=22)
    {
    if (NoFlwIOs()+NoLinks()>0)
      {
      if (DDB.BeginArray(this, "FB", "NdFBS", NoFlwIOs()+NoLinks(), 0, DDB_OptPage))
        {
        for (int i=0; i<NoFlwIOs(); i++)
          {
          if (Nd_Rmt(i))
            {
            for (int fe=0; fe<NIOFBs(i); fe++)
              {
              Strng Tg;
              DWORD UI=UIIOFBFlag+i+(fe<<16);
              MAKEIOTG4DDB(Tg, i, fe);
              FlwBlk &FB=*IOFB(i,fe);
              BuildSystemDefn_FB(false, Tg(), i, fe, UI, FB, DDB);
              }
            }
          }
        for (int i=0; i<NoLinks(); i++)
          {
          for (int fe=0; fe<Link(i)->m_FBs.GetCount(); fe++)
            {
            Strng Tg;
            int ii=i+NoFlwIOs();
            DWORD UI=UIIOFBFlag+ii+(fe<<16);
            MAKELNKTG4DDB(Tg, i, fe);
            FlwBlk &FB=Link(i)->FB;
            BuildSystemDefn_FB(true, Tg(), i, fe, UI, FB, DDB);
            }
          }
        }
      DDB.EndArray();
      }

    if (GetSystemDefnFlwEqn(0, NULL))
      {
      Strng Tg;
      if (NoFlwIOs()>0)
        {
        if (DDB.BeginArray(this, "NdEqn", "NdFBEqns", NoFlwIOs()+1, 0, DDB_OptPage))
          {
          for (int i=0; ; i++)
            {
            CFlwEqnPtr * pFEP;
            if ((pFEP=GetSystemDefnFlwEqn(i, &Tg))!=NULL)
              {
              DWORD UI=UINdFBFlag+i;
              if (DDB.BeginElement(this, Tg(),NULL,UI, DDB_OptPage,MarkerClosed))
                pFEP->BuildDataDefn(FBDDOpt_WithEqn|FBDDOpt_HideEqnStruct|FBDDOpt_WithQmSpec|FBDDOpt_WithDPSpec, DDB, this, Tg());
              }
            else
              break;
            }
          }
        DDB.EndArray();
        }
      }
    if (NoFlwIOs()+NoLinks()>0)
      {
      if (DDB.BeginArray(this, "FBEqn", "NdFBEqns", NoFlwIOs()+NoLinks()+1, 0, DDB_OptPage))
        {
        for (int i=0; i<NoFlwIOs(); i++)
          {
          if (Nd_Rmt(i))
            {
            for (int fe=0; fe<NIOFBs(i); fe++)
              {
              if (!IOFB(i,fe)->HasParent())
                {
                Strng Tg;
                MAKEIOTG4DDB(Tg, i, fe);

                DWORD UI=UIIOFBFlag+i+(fe<<16);
                if (DDB.BeginElement(this, Tg(),NULL,UI, DDB_OptPage,MarkerClosed))
                  {
                  IOFB(i,fe)->BuildDataDefn(FBDDOpt_WithEqn|FBDDOpt_HideEqnStruct|FBDDOpt_WithQmSpec|FBDDOpt_WithDPSpec, DDB, this, Nd_Rmt(i)->Tag(), UI, DDB_NoPage);
                  }
                }
              }
            }
          }
        for (int i=0; i<NoLinks(); i++)
          {
          for (int fe=0; fe<Link(i)->m_FBs.GetCount(); fe++)
            {
            Strng Tg;
            int ii=i+NoFlwIOs();
            DWORD UI=UIIOFBFlag+ii+(fe<<16);
            MAKELNKTG4DDB(Tg, i, fe);
            if (DDB.BeginElement(this, Tg(),NULL,UI, DDB_OptPage,MarkerClosed))
              {
              //FlwBlk &FB=Link(i)->FB;
              Link(i)->FB.BuildDataDefn(FBDDOpt_WithEqn|FBDDOpt_HideEqnStruct|FBDDOpt_WithQmSpec|FBDDOpt_WithDPSpec, DDB, this, Tg(), UI, DDB_NoPage);
              }
            }
          }
        }
      DDB.EndArray();
      }
    }

  if (PrjFileVerNo()>=64)
    {
    if (NoCIOs()>0)
      {
      flag DoBlk;
      DoBlk=DDB.BeginArray(this, "Term", "NdCTerms", NoCIOs(), 0, DDB_OptPage);
      if (DoBlk)
        for (int i=CIO1(); i<CION(); i++)
          if (Nd_Rmt(i))
            {
            Strng Tg;
            MAKEIOTG4DDB(Tg, i, 0);

            DWORD UI=UIIOFlag+i;
            if (DDB.BeginElement(this, Tg(),NULL,UI, DDB_OptPage,MarkerClosed))
              {
              DDB.Double("Value",     "",   DC_,    "",       &m_IOs[i].CtrlData().m_dValue,  this, isParmConstruct);
              }
            }
      DDB.EndArray();
      }
    if (NoEIOs()>0)
      {
//      flag DoBlk;
//      DoBlk=DDB.BeginArray(this, "Term", "NdETerms", NoEIOs(), 0, DDB_OptPage);
//      if (DoBlk)
//        for (int i=EIO1(); i<EION(); i++)
//          if (Nd_Rmt(i))
//            {
//            Strng Tg;
//            Tg=Nd_Rmt(i)->Tag();
//            DWORD UI=UIIOFlag+i;
//            if (DDB.BeginElement(this, Tg(),NULL,UI, DDB_OptPage,MarkerClosed))
//              {
////              CElecConduit & D=m_IOs[i].ElecConduit();
////ELEC               DDB.Long  ("Phases",    "",   DC_,      "",   &D.m_nPhases,            this, isParmConstruct);
////ELEC               DDB.Double("NomVolts",  "",   DC_V,     "V",  &D.m_dNomVolts,          this, isParmConstruct|InitHidden);
////ELEC               //DDB.Double("NomAmps",   "",   DC_I,     "A",  &D.m_dNomAmps,           this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("V1",        "",   DC_V,     "V",  &D.m_Phase[0].m_dV,      this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("V2",        "",   DC_V,     "V",  &D.m_Phase[1].m_dV,      this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("V3",        "",   DC_V,     "V",  &D.m_Phase[2].m_dV,      this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("Vn",        "",   DC_V,     "V",  &D.m_Neutral.m_dV,       this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("Ve",        "",   DC_V,     "V",  &D.m_Earth.m_dV,         this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("I1",        "",   DC_I,     "A",  &D.m_Phase[0].m_dI,      this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("I2",        "",   DC_I,     "A",  &D.m_Phase[1].m_dI,      this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("I3",        "",   DC_I,     "A",  &D.m_Phase[2].m_dI,      this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("In",        "",   DC_I,     "A",  &D.m_Neutral.m_dI,       this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("Ie",        "",   DC_I,     "A",  &D.m_Earth.m_dI,         this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("Phi1",      "",   DC_CosPhi,"",   &D.m_Phase[0].m_dPhi, this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("Phi2",      "",   DC_CosPhi,"",   &D.m_Phase[1].m_dPhi, this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("Phi3",      "",   DC_CosPhi,"",   &D.m_Phase[2].m_dPhi, this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("Phin",      "",   DC_CosPhi,"",   &D.m_Neutral.m_dPhi,  this, isParmConstruct|InitHidden);
////ELEC               DDB.Double("Phie",      "",   DC_CosPhi,"",   &D.m_Earth.m_dPhi,    this, isParmConstruct|InitHidden);
//              }
//            }
//      DDB.EndArray();
      }
    }
  else if (PrjFileVerNo()>=60)
    {
    if (NoCIOs()>0)
      {
      flag DoBlk;
      DoBlk=DDB.BeginArray(this, "Term", "NdTerms", NoCIOs(), 0, DDB_OptPage);
      if (DoBlk)
        for (int i=CIO1(); i<CION(); i++)
          if (Nd_Rmt(i))
            {
            Strng Tg;
            MAKEIOTG4DDB(Tg, i, 0);
            DWORD UI=UIIOFlag+i;
            if (DDB.BeginElement(this, Tg(),NULL,UI, DDB_OptPage,MarkerClosed))
              {
              DDB.Double("Value",     "",   DC_,    "",       &m_IOs[i].CtrlData().m_dValue,  this, isParmConstruct);
              }
            }
      DDB.EndArray();
      }
    }
  }

//--------------------------------------------------------------------------

void FlwNode::BuildDataDefnElevation(DataDefnBlk & DDB)
  {
//  if (1)//HasFullFlow() || DDB.ForFileSnpScn())
  if (HasModeNear(-1, LFM_Full) || DDB.ForNDB() || DDB.ForFileSnpScn())
    {
    DDEF_Flags Vis=DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    DDB.Text    (" ");
    DDB.Text    ("Datum");
    DDB.Double  ("Elevation",       "",     DC_L,     "m",     xidElevation,    this, isResult|0   |SM_DynBoth);
    DDB.Double  ("Datum",           "Z",    DC_L,     "m",     &m_Datum,        this, isResult|0   |SM_DynBoth);
    DDB.Double  ("Datum_Rqd",       "Z_Rqd",DC_L,     "m",     &m_DatumRqd,     this, isParmStopped|SM_DynBoth|NAN_OK, DDBNAN_Floating);
    DDB.SetVisibility(Vis);
    }
  };

//--------------------------------------------------------------------------

LPTSTR ESSStr[] = {"Denied", "Request", "Required"};
static char IOTagTmp[256];

flag FlwNode::DataXchg(DataChangeBlk & DCB)
  {
  if (DCB.dwUserInfo & UIIOFlag)
    {
    DWORD UI=DCB.dwUserInfo&(~UIIOFlag);
    DCB.dwUserInfo=0;
    switch (DCB.lHandle)
      {
      case xidIOSeqStartOK:
        DCB.pC=ESSStr[IOEvalSeqStart_Self(UI&0xff)];
        return 1;
      case xidIORmtSeqStartOK:
        DCB.pC=ESSStr[IOEvalSeqStart_Rmt(UI&0xff)];
        return 1;
      case xidIOSeqStart:
//        DCB.pC=IO_EvalSeqStartOK(UI&0xff) ? "Start":"";
        DCB.pC=IOFlange(UI&0xff)->IsEvalSeqStart() ? "Yes":"";
        return 1;
      }
    DCB.dwUserInfo=UI;
    }
  if (DCB.dwUserInfo & UIIOFBFlag)
    {
    DWORD UI=DCB.dwUserInfo&(~UIIOFBFlag);
    DCB.dwUserInfo=0;
    int i=UI&0xffff;
    if (i<NoFlwIOs())
      {
      if (IOFB(i, UI>>16)->DataXchg(DCB))
        return 1;
      }
    else
      {
      if (LinkFB(i-NoFlwIOs(), UI>>16)->DataXchg(DCB))
        return 1;
      }
    DCB.dwUserInfo=UI;
    }
  if (DCB.dwUserInfo & UINdFBFlag)   
    {
    DWORD UI=DCB.dwUserInfo&(~UINdFBFlag);
    DCB.dwUserInfo=0;
    CFlwEqnPtr *p=GetSystemDefnFlwEqn(UI&0xffff, NULL);
    if (!p)
      {
      LogError(FullObjTag(), 0, "GetSystemDefnFlwEqn returned NULL");
      }
    else if (p->DataXchg(DCB))
      return 1;
    DCB.dwUserInfo=UI;
    }

#if WITHEQUIPSPECS
  if (SelectData() && SelectData()->DataXchg(DCB, this))
    return 1;

  if (CustomData() && CustomData()->DataXchg(DCB, this))
    return 1;
#endif

  if (DataXchgDefnAreaIOs(DCB))
    return 1;

  if (CTNode::DataXchg(DCB))
    return 1;

#if WITHNODEPROCS
  if (m_ProcsAllowed && m_ProcsReqd && m_pProcs->DataXchg(DCB))
    return 1;
#endif
#if WITHNODETAGIO
  if (m_pTagIO && m_pTagIO->DataXchg(DCB))
    return 1;
#endif

  switch (DCB.lHandle)
    {
    case xidElevation:
      DCB.D=m_Datum+BaseElevation();
      return 1;
    case xidFlwNdRqdArea:
      if (DCB.rpC)
        SetAreaTag(DCB.rpC);
      DCB.pC=LPTSTR(GetAreaTagDisp());
      return 1;
#if WITHSPECSHEETS
    case xidSpecSheet:
      {
      if (DCB.rpC)
        sSpecSheet=DCB.rpC;
      flag BadChars=0;
      for (int i=0; i<sSpecSheet.Length(); i++)
        if (!isalnum(sSpecSheet[i]) && !ispunct(sSpecSheet[i]))
          BadChars=1;
      if (sSpecSheet.Length()==0 || BadChars)
        sSpecSheet="Standard";
      if (sSpecSheet.Length()>0)
        DCB.pC=sSpecSheet();
      else
        DCB.pC="";
      }
      return 1;
#endif
    case xidNetMethod:
      {
      if (DCB.rpC)
        SetNetMethod((LPCTSTR)DCB.rpC);
      DCB.pC=NetMethodStr();
      return 1;
      }
    case xidSolveMethod:
      {
      if (DCB.rpC)
        SetSolveMethod((LPCTSTR)DCB.rpC);
      DCB.pC=SolveMethodStr();
      return 1;
      }
    case xidHeatMethod:
      {
       if (DCB.rpC)
         SetHeatMethod((LPCTSTR)DCB.rpC);
      DCB.pC=HeatMethodStr();
      return 1;
      }
    case xidFlowMode:
      {
      if (DCB.rpC)
        SetFlowModeLcl((LPCTSTR)DCB.rpC);
      DCB.pC=FlowModeStr();
      return 1;
      }
    case xidFlowModeRqd:
      {
      if (DCB.rpC)
        SetFlowModeRqd((LPCTSTR)DCB.rpC, LFM_Propagate, this);
      DCB.pC=FlowModeRqdStr();
      return 1;
      }
    case xidFlowModeSet:
      {
      DCB.pC = (m_FlowModeObj==this) ? "Self" : (m_FlowModeObj!=NULL) ? m_FlowModeObj->FullObjTag() : "";
      return 1;
      }
    case xidActiveMode:
      {
      if (DCB.rB)
        SetActive(*DCB.rB);
      DCB.B=GetActive();
      return 1;
      }
    case xidHoldMode:
      {
      if (DCB.rB)
        SetActiveHold(*DCB.rB);
      DCB.B=GetActiveHold();
      return 1;
      }
    case xidAuditReqd:
      {
      if (DCB.rpC)
        SetAuditOptionStr(DCB.rpC);
      DCB.pC=(LPSTR)GetAuditOptionStr();
      return 1;
      }

#if WITHNODEPROCS
    case xidProcsEnable:
      {
      if (DCB.rB)
        SetProcsReqd(*DCB.rB!=0);
      DCB.B=GetProcsReqd()?1:0;
      return 1;
      }
#endif

    case xidSolveState:
      {
      if (DCB.rpC)
        {
        SetActive(_stricmp(DCB.rpC, Inactive_Str)!=0 && _stricmp(DCB.rpC, InactiveHold_Str)!=0);
        SetActiveHold(_stricmp(DCB.rpC, ActiveHold_Str)==0 || _stricmp(DCB.rpC, InactiveHold_Str)==0);
        StructureChanged(NULL);
        }

      if (!GetActive())
        DCB.pC=GetActiveHold() ? InactiveHold_Str:Inactive_Str;
      else
        DCB.pC=GetActiveHold() ? ActiveHold_Str:Active_Str;
      return 1;
      }
    case xidSysCADSolutionOld:
      {
      if (PrjFileVerNo()<43 || fSolutionForRoot)
        {
        if (DCB.rpC)
          {
          SetActive(_stricmp(DCB.rpC, Inactive_Str)!=0);
          StructureChanged(NULL);
          }
        }
      else  // NormalNode
        {
        if (DCB.rpC)
          {
          SetActive(_stricmp(DCB.rpC, Inactive_Str)!=0 && _stricmp(DCB.rpC, InactiveHold_Str)!=0);
          SetActiveHold(_stricmp(DCB.rpC, ActiveHold_Str)==0 || _stricmp(DCB.rpC, InactiveHold_Str)==0);
          StructureChanged(NULL);
          }
        DCB.pC="";
        }
      return 1;
      }
    case xidEvalOrder:
      {
      DCB.pC=m_sEvalOrder();
      return 1;
      }
    case xidNdCondition:
      {
      sCndStrng="";
      Strng_List L;
      GetAllCIs(L);
      int i=0;
      for (pStrng p= L.First();p; p=L.Next())
        {
        if (i++>0)
          sCndStrng+="\n";
        char *ps=p->Str();
        char *pt=strchr(ps, '\t');
        sCndStrng+=pt ? (pt+1) : ps;
        }
      if (i>0)
        DCB.dwRetFlags=DDEF_INERROR;
      else
        sCndStrng="OK";

      DCB.pC=sCndStrng();
      return 1;
      }
    case xidNdConditionCount:
      DCB.L=CountAllCIs(1000);
      return 1;
    case xidMarked:
      if (DCB.rB)
        {
        fMarked=*DCB.rB;
        }
      DCB.B=fMarked;
      return 1;
    case xidSetState:
      if (DCB.rB)
        SetState((eScdMdlStateActs)*DCB.rB);
      DCB.B=0;
      return 1;

    case xidSetStateZeroFlows:
      if (DCB.rB && (*DCB.rB!=0))
        SetState(MSA_ZeroFlows);
      DCB.B=0;
      return 1;
    case xidSetStateEmpty:
      if (DCB.rB && (*DCB.rB!=0))
        SetState(MSA_Empty);
      DCB.B=0;
      return 1;
    case xidSetStatePreSet:
      if (DCB.rB && (*DCB.rB!=0))
        SetState(MSA_PreSet);
      DCB.B=0;
      return 1;
    case xidSetStateSteadyState:
      if (DCB.rB && (*DCB.rB!=0))
        SetState(MSA_SteadyState);
      DCB.B=0;
      return 1;
    case xidSetStatePBInit:
      if (DCB.rB && (*DCB.rB!=0))
        SetState(MSA_PBInit);
      DCB.B=0;
      return 1;
    case xidSetStateDynStatsRunInit:
      if (DCB.rB && (*DCB.rB!=0))
        SetState(MSA_DynStatsRunInit);
      DCB.B=0;
      return 1;
    case xidSetStateEmptySpillTargets:
      if (DCB.rB && (*DCB.rB!=0))
        SetState(MSA_EmptySpillTargets);
      DCB.B=0;
      return 1;

    case xidNdNJoins:
      if (DCB.rL && ((DCB.dwOpts&TABOpt_ForFile)!=0))
        Joins.SetSize(*DCB.rL);
      DCB.L=Joins.GetSize();
      return 1;
    case xidNdNIOs:
      DCB.L=NoFlwIOs();
      return 1;
    case xidSrcIOTag:
      if (NoIOs() >= 1)
        strcpy(IOTagTmp, Nd_Rmt(0)->FullIOTag(OId_NodeIO, IODescNo_Rmt(0), TU_IndividuallyUnique, False, true));
      else
        strcpy(IOTagTmp, "");
      DCB.pC=IOTagTmp;
      return true;
    case xidDstIOTag:
      if (NoIOs() >= 2)
        strcpy(IOTagTmp, Nd_Rmt(1)->FullIOTag(OId_NodeIO, IODescNo_Rmt(1), TU_IndividuallyUnique, False, true));
      else
        strcpy(IOTagTmp, "");
      DCB.pC=IOTagTmp;
      return true;
    case xidFBMeanTemp:
      DCB.D=dNAN;//IOFB(DCB.dwUserInfo&0xff, DCB.dwUserInfo>>8)->MeanTemp(pProps);
      return 1;
    case xidFBMeanPress:
      DCB.D=dNAN;//IOFB(DCB.dwUserInfo&0xff, DCB.dwUserInfo>>8)->MeanPress();
      return 1;
    case xidFBMeanRho:
      DCB.D=dNAN;//IOFB(DCB.dwUserInfo&0xff, DCB.dwUserInfo>>8)->MeanRho(pProps);
      return 1;
    case xidFBRise:
      {
      int i=DCB.dwUserInfo&0xffff;
      int fe=DCB.dwUserInfo>>16;
      if (i<NoFlwIOs())
        {
        if (DCB.rD)
          IOFB(i, fe)->SetRise(*DCB.rD);
        DCB.D=IOFB(i, fe)->Rise();
        }
      else
        {
        i-=NoFlwIOs();
        if (DCB.rD)
          LinkFB(i, fe)->SetRise(*DCB.rD);
        DCB.D=LinkFB(i, fe)->Rise();
        }
      return 1;
      }
    case xidFBRqdLen:
      {
      int i=DCB.dwUserInfo&0xffff;
      int fe=DCB.dwUserInfo>>16;
      if (i<NoFlwIOs())
        {
        if (DCB.rD)
          IOFB(i, fe)->SetRqdLength(*DCB.rD);
        DCB.D=IOFB(i, fe)->RqdLength();
        }
      else
        {
        i-=NoFlwIOs();
        if (DCB.rD)
          LinkFB(i, fe)->SetRqdLength(*DCB.rD);
        DCB.D=LinkFB(i, fe)->RqdLength();
        }
      return 1;
      }
    case xidFBActLen:
      {
      int i=DCB.dwUserInfo&0xffff;
      int fe=DCB.dwUserInfo>>16;
      if (i<NoFlwIOs())
        {
        if (DCB.rD)
          IOFB(i, fe)->SetActLength(*DCB.rD);
        DCB.D=IOFB(i, fe)->ActLength();
        }
      else
        {
        i-=NoFlwIOs();
        if (DCB.rD)
          LinkFB(i, fe)->SetActLength(*DCB.rD);
        DCB.D=LinkFB(i, fe)->ActLength();
        }
      return 1;
      }
    case xidFBFitLen:
      {
      int i=DCB.dwUserInfo&0xffff;
      int fe=DCB.dwUserInfo>>16;
      if (i<NoFlwIOs())
        {
        if (DCB.rD)
          IOFB(i, fe)->SetFitLength(*DCB.rD);
        DCB.D=IOFB(i, fe)->FitLength();
        }
      else
        {
        i-=NoFlwIOs();
        if (DCB.rD)
          LinkFB(i, fe)->SetFitLength(*DCB.rD);
        DCB.D=LinkFB(i, fe)->FitLength();
        }
      return 1;
      }
    case xidJoinNetInfo:
      if (DCB.rpC)
        Joins[DCB.dwUserInfo].sNetInfo=DCB.rpC;
      DCB.pC=Joins[DCB.dwUserInfo].sNetInfo();
      return 1;
    }
  return 0;
  }

//--------------------------------------------------------------------------

long FlwNode::AuditOption() 
  { 
  //return m_ForceAuditOption ? m_ForceAuditOption : m_AuditOption ? m_AuditOption :gs_AuditOption; 
  return Max(m_ForceAuditOption, Max(m_AuditOption, gs_AuditOption)); 
  }

LPCSTR FlwNode::GetAuditOptionStr()
  {
  long Opt=Max(m_ForceAuditOption , m_AuditOption);
  switch (Opt)
    {
    case Audit_Off:          return "Off";
    case Audit_Flows:        return "Flows";
    case Audit_Totals:       return "Totals";
    case Audit_Balance:      return "Balance";
    case Audit_Ignore:
      {
      switch (gs_AuditOption)
        {
        case Audit_Ignore:       return "<Off>";
        case Audit_Off:          return "<Off>";
        case Audit_Flows:        return "<Flows>";
        case Audit_Totals:       return "<Totals>";
        case Audit_Balance:      return "<Balance>";
        }
      }
    }
  return "??";
  }
long FlwNode::SetAuditOptionStr(LPCSTR Str) 
  {
  if        (stricmp(Str, "Off")==0)     m_AuditOption = Audit_Off;
  else if   (stricmp(Str, "Flows")==0)   m_AuditOption = Audit_Flows;
  else if   (stricmp(Str, "Totals")==0)  m_AuditOption = Audit_Totals;
  else if   (stricmp(Str, "Balance")==0) m_AuditOption = Audit_Balance;
  else 
    {
    m_AuditOption = Audit_Ignore;
    if        (stricmp(Str, "<Off>")==0)     gs_AuditOption = Audit_Off;
    else if   (stricmp(Str, "<Flows>")==0)   gs_AuditOption = Audit_Flows;
    else if   (stricmp(Str, "<Totals>")==0)  gs_AuditOption = Audit_Totals;
    else if   (stricmp(Str, "<Balance>")==0) gs_AuditOption = Audit_Balance;
    }                 

  return AuditOption(); 
  }

void FlwNode::GetAuditOptionStrLst(DDBValueLstMem & Lst, bool ForNode)
  {
  Lst.Empty();

  if (ForNode)
    {
    if       (gs_AuditOption == Audit_Off)     Lst.Add("<Off>");     
    else if  (gs_AuditOption == Audit_Flows)   Lst.Add("<Flows>");
    else if  (gs_AuditOption == Audit_Totals)  Lst.Add("<Totals>");  
    else if  (gs_AuditOption == Audit_Balance) Lst.Add("<Balance>");
    }

  if (Lst.Length()>0)
    Lst.Add("---------");

  if (m_ForceAuditOption <= Audit_Off)                  Lst.Add("Off");     
  if (m_ForceAuditOption <= Audit_Flows)                Lst.Add("Flows");
  if (m_ForceAuditOption <= Audit_Totals)               Lst.Add("Totals");  
  if (m_ForceAuditOption <= Audit_Balance && !ForNode)  Lst.Add("Balance");

  //if (m_ForceAuditOption>Audit_Ignore)
  //  {
  //  if       (m_ForceAuditOption <= Audit_Off)     Lst.Add("Off");     
  //  else if  (m_ForceAuditOption <= Audit_Flows)   Lst.Add("Flows");
  //  else if  (m_ForceAuditOption <= Audit_Totals)  Lst.Add("Totals");  
  //  else if  (m_ForceAuditOption <= Audit_Balance) Lst.Add("Balance");
  //  }
  //else
  //  {
  //  if (ForNode)
  //    {
  //    if       (gs_AuditOption == Audit_Off)     Lst.Add("<Off>");     
  //    else if  (gs_AuditOption == Audit_Flows)   Lst.Add("<Flows>");
  //    else if  (gs_AuditOption == Audit_Totals)  Lst.Add("<Totals>");  
  //    else if  (gs_AuditOption == Audit_Balance) Lst.Add("<Balance>");
  //    }
  //  Lst.Add("Off");     
  //  Lst.Add("Flows");
  //  Lst.Add("Totals");  
  //  Lst.Add("Balance");
  //  }
  };

//--------------------------------------------------------------------------

#if WITHNODEPROCS

void FlwNode::SetProcsAllowed(bool Allowed)
  {
  m_ProcsAllowed = Allowed;
  }

void FlwNode::SetProcsReqd(bool Reqd)
  {
  m_ProcsReqd=Reqd;
  if (!m_ProcsAllowed || !m_ProcsReqd)
    {
    delete m_pProcs;
    m_pProcs=NULL;
    }
  else if (m_ProcsAllowed && m_pProcs==NULL)
    {
    m_pProcs = new CNodeProcedures(this);
    }
  };

#endif

//--------------------------------------------------------------------------

#if WITHNODETAGIO

void FlwNode::OpenTagIO()
  {              
  if (gs_Exec.Stopped())
    {
    if (m_pTagIO==NULL)
      m_pTagIO = new CNodeTagIOList(this);
    m_TagIOActive=true;
    }
  };

void FlwNode::CloseTagIO()
  {              
  if (gs_Exec.Stopped())
    m_TagIOActive=false;
  };

bool FlwNode::SetTagIOActive(bool On)
  {
  if (gs_Exec.Stopped())
    {
    m_TagIOActive=On; 
    return true;
    };
  return false;
  }

#endif

//--------------------------------------------------------------------------

flag FlwNode::ValidateData(ValidateDataBlk & VDB)
  {
  m_fValidateDataComplete=True;
  flag OK=ValidateDataAreaIOs(VDB);
  
  for (int i=0; i<NoFlwIOs(); i++)
    {
    if (Nd_Rmt(i))
      {
      for (int fe=0; fe<NIOFBs(i); fe++)
        if (!IOFB(i,fe)->ValidateData(VDB))
          OK=False;
      }
    else
      {
      PreDisConnect(i);
      Detach(i);
      LogWarning(FullObjTag(), 0, "IO Adjusted for missing Connection");
      i--;
      }
    }

#if WITHNODEPROCS
  if (m_ProcsAllowed && m_ProcsReqd && !m_pProcs->ValidateData(VDB))
    OK=false;
#endif
#if WITHNODETAGIO
  if (m_TagIOActive && !m_pTagIO->ValidateData(VDB))
    OK=false;
#endif

  return OK;
  }

//--------------------------------------------------------------------------

pchar FlwNode::TypeString(DWORD Type, Strng & TypeStr)
  {
  if (Type & nc_Process    ) TypeStr+="Process,";
  if (Type & nc_MSrcSnk    ) TypeStr+="SrcSnk,";
  if (Type & nc_MXfer      ) TypeStr+="Transfer,";
  if (Type & nc_MLnk       ) TypeStr+="MLnk,";
  if (Type & nc_MBst       ) TypeStr+="MBst,";
  if (Type & nc_Parent     ) TypeStr+="Parent,";
  if (Type & nc_Control    ) TypeStr+="Control,";
  if (Type & nc_CLnk       ) TypeStr+="CtrlLnk,";
  if (Type & nc_Elec       ) TypeStr+="Electics,";
  if (Type & nc_ELnk       ) TypeStr+="ElecLine,";
  if (Type & nc_ALnk       ) TypeStr+="AirPipe,";
  if (Type & nc_FlwSolver  ) TypeStr+="CFlwSolver,";
  if (Type & nc_StandAlone ) TypeStr+="StandAlone,";

  if (TypeStr.Len()>0)
    TypeStr[TypeStr.Len()-1] = 0;

  return TypeStr();
  }

//--------------------------------------------------------------------------

pchar FlwNode::TypeString(Strng & TypeStr, flag Clr)
  {
  if (Clr)
    TypeStr="";
  return FlwNode::TypeString(m_dwType, TypeStr);
  }

// --------------------------------------------------------------------------

FlwNode * FlwNode::InsertRoot(pchar TagIn)
  {
  FlwNode * p;
  if (TagIn && (strlen(TagIn) > 0) && (p = (FlwNode *)FindObj(TagIn)) != NULL)
    return p;
  else
    {
    p = this->FamilyHead();
    return p;
    }
  }

//--------------------------------------------------------------------------

FlwNode * FlwNode::InsertNode(FlwNode * Root, LPTSTR ClassIn, LPTSTR SubClassIn, LPTSTR PrimaryCfgIn, LPTSTR TagIn, pTaggedObject pAttach, TagObjAttachment eAttach)
  {
  FlwNode *     rt = Root;
  FlwNode *     RetNd=NULL;
  SCDPartTag    FirstPart;
  pchar         Remainder;

  SplitTag(TagIn, FirstPart, Remainder);

  RetNd = (FlwNode *)rt->FindObj(FirstPart);
  while (Remainder && RetNd)
    {
    rt = RetNd;
    SplitTag(Remainder, FirstPart, Remainder);
    RetNd = (FlwNode *)rt->FindObj(FirstPart);
    }

  while (Remainder && (RetNd==NULL))
    {
    RetNd = (FlwNode *)CreateFlwNode("Area", NULL, FirstPart, pAttach, eAttach);
    if (RetNd)
      {
      rt->Insert(RetNd, 0);
      if (PrimaryCfgIn)
        RetNd->PutPrimaryCfg(PrimaryCfgIn);
      }
    else
      break;
    rt = RetNd;
    SplitTag(Remainder, FirstPart, Remainder);
    }

  if (RetNd==NULL)
    {
    RetNd = (FlwNode *)CreateFlwNode(ClassIn, SubClassIn, FirstPart, pAttach, eAttach);
    if (RetNd)
      {
      rt->Insert(RetNd, 0);
      if (PrimaryCfgIn)
        RetNd->PutPrimaryCfg(PrimaryCfgIn);
      }
    }
  return RetNd;
  };

//--------------------------------------------------------------------------

flag FlwNode::GetObj_All(FlwNode * &p)
  {
  if (p == NULL)
    p = this;
  else if (p == this)
    p = (FlwNode *)this->MyChildren;
  else if (p->MyChildren)
    p = (FlwNode *)p->MyChildren;
  else  if (p->Nxt)
    p = (FlwNode *)p->Nxt;
  else
    {
    while ((p->MyParent != this) && (p->MyParent->Nxt == NULL))
      p = (FlwNode *)p->MyParent;
    if (p->MyParent != this)
      p = (FlwNode *)p->MyParent->Nxt;
    else
      p = NULL;
    };

  return(p!=NULL);
  };

//--------------------------------------------------------------------------

flag FlwNode::GetObj_Tree(FlwNode * &p)
  {
  if (p == NULL)
    p = this;
  else if (p == this)
    p = (FlwNode *)this->MyChildren;
  else if (p->MyChildren)
    p = (FlwNode *)p->MyChildren;
  else  if (p->Nxt)
    p = (FlwNode *)p->Nxt;
  else
    p = NULL;

  return(p!=NULL);
  };

//--------------------------------------------------------------------------

pchar FlwNode::FullIOTag(int OId, int Inx, TagUniqueness TU, flag InternalNodes, flag SubsDescriptor)
  {
  const int TagBuffLen = 128;
  static char FullTagTmp[4][TagBuffLen];
  static char FullTagInx = 0;
  pchar result = FullTagTmp[FullTagInx];
  FlwNode * p[MaxTagDepth];
  int i = 0;

  FullTagInx = (++FullTagInx)%4;
  p[0] = this;
  while (p[i]->MyParent && (p[i]->Internal || TU==TU_UniqueInFamily))
    {
    ASSERT(i+1<MaxTagDepth);
    p[i+1] = (FlwNode *)p[i]->MyParent;
    i++;
    };
  while (i && (strlen(p[i]->Tag()) == 0))
    i--;
  strcpy(result, "");
  strcat(result,p[i]->Tag());
  while (i > 0)
    {
    i--;
    strcat(result,TagSeparator);
    strcat(result,p[i]->Tag());
    };
  ASSERT(strlen(result)<TagBuffLen);

  switch (OId)
    {
    case OId_NodeIO:
        {
        strcat(result,TagSeparator);
        if (SubsDescriptor && IOAreas[Inx].IODesc() && strlen(IOAreas[Inx].IODesc())>0)
          strcat(result,IOAreas[Inx].IODesc());
        else
          strcat(result,IOAreas[Inx].IOName() ? IOAreas[Inx].IOName() : "");

        if(!InternalNodes)
          {
          flag Done=0;
          while (p[i]->Internal && !Done)
            {
            i++;
            for (int j=0; ; j++)
              {
              IOAreaRec * pIO=&(p[i]->IOAreas[j]);
              if (pIO->IOName()==NULL || pIO->m_pMapTo==NULL || pIO->IOName()[0]==0 || pIO->m_pMapTo[0]==0)
                {
                Done=1;
                break;
                }
              int lr=strlen(result);
              int lm=pIO->m_pMapTo ? strlen(pIO->m_pMapTo) : 0;
              if (lm>0 && _stricmp(&result[lr-lm], pIO->m_pMapTo)==0)
                {
                strcpy(&result[lr-lm], pIO->IOName());
                break;
                }
              }
            }
          }

        break;
        };
    default:;
    };

  ASSERT(strlen(result)<TagBuffLen);
  return result;
  };

//--------------------------------------------------------------------------

void FlwNode::SetIO_Open(int i, int iJoinId, flag Damped, eEvalSeqStart ESS, byte FlwPhasesAllowed)
  {
  if (FlwPhasesAllowed==FPh_Default)
    FlwPhasesAllowed=FPh_Both;

  if (i>=NoFlwIOs())
    return;
  CIORec &S=IO_Self(i);
  ASSERT(fInConfigureJoins);

//  if (!IsImpSolution())
//    Damped=False;

  if (S.iJoinId!=iJoinId ||
      S.iJoinTp!=IOJn_Open ||
      S.fDamped!=Damped ||
      S.m_eEvalSeqStart!=ESS ||
      Joins[iJoinId].m_iFlwPh_Rqd!=FlwPhasesAllowed)
    {
    for (int fe=0; fe<NIOFBs(i); fe++)
      {
      IOFB(i,fe)->m_bClosed=false;
      IOFB(i,fe)->SetQmFree();
      IOFB(i,fe)->m_iFlwPh_Rqd=FlwPhasesAllowed;
      //IOFB(i,fe)->m_iFlwPh_Act=FPh_UnKnown;
      }
    SetTopologyChanged();
    DBGTOPOCHG("IO_Open")
    }

  S.iJoinTp=IOJn_Open;
  S.iJoinId=iJoinId;
  S.fDamped=Damped;
  S.m_eEvalSeqStart=ESS;

  m_nJoins=Max(m_nJoins, iJoinId+1);
  Joins.SetSize(Max(Joins.GetSize(), iJoinId+1));
  Joins[iJoinId].fIsOpen=!Damped;
  Joins[iJoinId].m_iFlwPh_Rqd = FlwPhasesAllowed;
  Joins[iJoinId].fIsMkup = false;
  Joins[iJoinId].fIsAudit = false;
  Joins[iJoinId].fIsDirect = false;
  //Joins[iJoinId].m_iFlwPh_Act = FPh_UnKnown;
  };

//--------------------------------------------------------------------------

void FlwNode::SetIO_Direct(int i, int iJoinId)
  {
  flag Damped=false;
  eEvalSeqStart ESS=ESS_Allowed;
  byte FlwPhasesAllowed=FPh_Default;
  if (FlwPhasesAllowed==FPh_Default)
    FlwPhasesAllowed=FPh_Both;

  if (i>=NoFlwIOs())
    return;
  CIORec &S=IO_Self(i);
  ASSERT(fInConfigureJoins);

  if (S.iJoinId!=iJoinId ||
      S.iJoinTp!=IOJn_Direct ||
      S.fDamped!=Damped ||
      S.m_eEvalSeqStart!=ESS ||
      Joins[iJoinId].m_iFlwPh_Rqd!=FlwPhasesAllowed)
    {
    for (int fe=0; fe<NIOFBs(i); fe++)
      {
      IOFB(i,fe)->m_bClosed=false;
      IOFB(i,fe)->SetQmFree();
      IOFB(i,fe)->m_iFlwPh_Rqd=FlwPhasesAllowed;
      //IOFB(i,fe)->m_iFlwPh_Act=FPh_UnKnown;
      }
    SetTopologyChanged();
    DBGTOPOCHG("IO_Open")
    }

  S.iJoinTp=IOJn_Direct;
  S.iJoinId=iJoinId;
  S.fDamped=Damped;
  S.m_eEvalSeqStart=ESS;

  m_nJoins=Max(m_nJoins, iJoinId+1);
  Joins.SetSize(Max(Joins.GetSize(), iJoinId+1));
  Joins[iJoinId].fIsOpen=!Damped;
  Joins[iJoinId].m_iFlwPh_Rqd = FlwPhasesAllowed;

  int Id=IOId_Self(i);
  Joins[iJoinId].fIsMkup = IsMakeupIOId(Id);
  Joins[iJoinId].fIsAudit = false;//IsAuditIOId(Id);
  Joins[iJoinId].fIsDirect = true;

  if (0)
    dbgpln("FlwNode::SetIO_Direct %3i %3i %i %s", i, iJoinId, IsMakeupIOId(Id), Tag());
  };

//--------------------------------------------------------------------------

void FlwNode::SetIO_Closed(int i, int iJoinId)
  {
  if (i>=NoFlwIOs())
    return;
  CIORec &S=IO_Self(i);
  ASSERT(fInConfigureJoins);
  if (S.iJoinId!=iJoinId ||
      S.iJoinTp!=IOJn_Closed ||
      S.fDamped ||
      S.m_eEvalSeqStart!=ESS_Denied ||
      Joins[iJoinId].m_iFlwPh_Rqd!=FPh_None)
    {
    for (int fe=0; fe<NIOFBs(i)-1; fe++)
      {
      IOFB(i,fe)->m_bClosed=false;
      IOFB(i,fe)->SetQmFree();
      IOFB(i,fe)->m_iFlwPh_Rqd=FPh_None;
      //IOFB(i,fe)->m_iFlwPh_Act=FPh_UnKnown;
      }
    IOFB(i,NIOFBs(i)-1)->m_bClosed=true;
    IOFB(i,NIOFBs(i)-1)->SetQmReqd(0.0);
    IOFB(i,NIOFBs(i)-1)->m_iFlwPh_Rqd=FPh_Both;
    //IOFB(i,NIOFBs(i)-1)->m_iFlwPh_Act=FPh_UnKnown;
    SetTopologyChanged();
    DBGTOPOCHG("IO_Closed")
    }

  S.iJoinTp=IOJn_Closed;
  S.iJoinId=iJoinId;
  S.fDamped=false;
  S.m_eEvalSeqStart=ESS_Denied;

  m_nJoins=Max(m_nJoins, iJoinId+1);
  Joins.SetSize(Max(Joins.GetSize(), iJoinId+1));
  Joins[iJoinId].fIsOpen=False;
  Joins[iJoinId].m_iFlwPh_Rqd=FPh_None;
  Joins[iJoinId].fIsMkup = false;
  Joins[iJoinId].fIsAudit = false;
  Joins[iJoinId].fIsDirect = false;
  //Joins[iJoinId].m_iFlwPh_Act=FPh_UnKnown;
  };

//--------------------------------------------------------------------------

void FlwNode::SetIO_Join(int i, int iJoinId, eEvalSeqStart ESS, byte FlwPhasesAllowed)
  {
  if (FlwPhasesAllowed==FPh_Default)
    FlwPhasesAllowed=FPh_Both;

  if (i>=NoFlwIOs())
    return;
  CIORec &S=IO_Self(i);
  ASSERT(fInConfigureJoins);

  if (S.iJoinId!=iJoinId ||
      S.iJoinTp!=IOJn_Joined ||
      S.fDamped ||
      S.m_eEvalSeqStart!=ESS ||
      Joins[iJoinId].m_iFlwPh_Rqd!=FlwPhasesAllowed)
    {
    for (int fe=0; fe<NIOFBs(i); fe++)
      {
      IOFB(i,fe)->m_bClosed=false;
      IOFB(i,fe)->SetQmFree();
      IOFB(i,fe)->m_iFlwPh_Rqd=FlwPhasesAllowed;
      //IOFB(i,fe)->m_iFlwPh_Act=FPh_UnKnown;
      }
    SetTopologyChanged();
    DBGTOPOCHG("IO_Join")
    }

  S.iJoinTp=IOJn_Joined;
  S.iJoinId=iJoinId;
  S.fDamped=false;
  S.m_eEvalSeqStart=ESS;

  m_nJoins=Max(m_nJoins, iJoinId+1);
  Joins.SetSize(Max(Joins.GetSize(), iJoinId+1));

  Joins[iJoinId].fIsOpen=false;
  Joins[iJoinId].m_iFlwPh_Rqd = FlwPhasesAllowed;
  Joins[iJoinId].fIsMkup = false;
  Joins[iJoinId].fIsAudit = false;
  Joins[iJoinId].fIsDirect = false;
  //Joins[iJoinId].m_iFlwPh_Act = FPh_UnKnown;
  };

//--------------------------------------------------------------------------

void FlwNode::Init_NoJoins(int n)
  {
  m_nJoins=n;
  Joins.SetSize(Max(Joins.GetSize(), n));
  };

//--------------------------------------------------------------------------

void FlwNode::Set_NoJoins(int n)
  {
  Joins.SetSize(Max(Joins.GetSize(), n));
  m_nRqdJoins=n;
  };

//--------------------------------------------------------------------------

void FlwNode::Init_NLinks(int n)
  {
  m_Links.SetSize(Max(m_Links.GetSize(), n));
  };

//--------------------------------------------------------------------------

void FlwNode::Set_NLinks(int n)
  {
  m_Links.SetSize(Max(m_Links.GetSize(), n));
  m_nRqdLinks=n;
  };

//--------------------------------------------------------------------------

void FlwNode::LinkJoins(int iLinkId, int iJoinId0, int iJoinId1, CFlwEqnPtr * FEP)//, byte FlwPhasesAllowed)
  {
  m_nLinks=Max(m_nLinks, iLinkId+1);
  m_Links.SetSize(Max(m_nLinks, m_nRqdLinks));
  ASSERT(iLinkId>=0);
  ASSERT(fInConfigureJoins);

  CLinkRec &L=m_Links[iLinkId];
  if (L.m_iJoinId0!=iJoinId0 || L.m_iJoinId1!=iJoinId1)
    {
    SetTopologyChanged();
    DBGTOPOCHG("LinkJoins")
    }

  L.m_iJoinId0=iJoinId0;
  L.m_iJoinId1=iJoinId1;
  L.m_pNd=this;
  L.m_FBs[0].SetParentFlwEqn(FEP);

  m_nJoins=Max(m_nJoins, iJoinId0+1);
  m_nJoins=Max(m_nJoins, iJoinId1+1);
  Joins.SetSize(Max(Joins.GetSize(), iJoinId0+1));
  Joins.SetSize(Max(Joins.GetSize(), iJoinId1+1));

  };

//--------------------------------------------------------------------------

double FlwNode::IODatum_Term(int i)
  {
  if (NetProbalMethod())
    return 0.0;

  double Z=IOFlange(i)->m_Datum;
  for (int j=0; j<NIOFBs(i); j++)
    Z+=IOFB(i,j)->Rise();
  return Z;
  };

//--------------------------------------------------------------------------

double FlwNode::IOAbsFracHgt_Term(int i, double ShapeHeight, double ShapeDatumPt)
  {
  double Hgt=Max(1.0e-3, ShapeHeight);
  double F=IOAbsFracHgt_Flng(i, ShapeHeight, ShapeDatumPt);
  for (int j=0; j<NIOFBs(i); j++)
    F+=IOFB(i,j)->Rise()/Hgt;
  return F;
  };

//--------------------------------------------------------------------------

double FlwNode::IOAbsFracHgt_Flng(int i, double ShapeHeight, double ShapeDatumPt)
  {
  double Hgt=Max(1.0e-3, ShapeHeight);
  double Dtm=ShapeDatumPt;
  double F=IORelFracHgt_Flng(i);
  if (F<0.0)
    return Dtm/Hgt*(1.0+F);
  else
    return (Dtm+(Hgt-Dtm)*F)/Hgt;
  };

//--------------------------------------------------------------------------

static void CheckQmAuxExists(JoinArray &Joins, int iJoinId, FlwNode * pNd)
  {
  if (iJoinId<Joins.GetSize()-1)
    {
    pNd->SetTopologyChanged();
    DBGTOPOCHG("CheckQmAuxExists")
    pNd->m_nJoins=Max(pNd->m_nJoins, iJoinId+1);
    Joins.SetSize(Max(Joins.GetSize(), iJoinId+1));
    }
  CJoinRec &J=Joins[iJoinId];

  if (!J.pQmAux)
    J.pQmAux= new JoinQmAux(pNd);
  }

//--------------------------------------------------------------------------

void FlwNode::SetJoinQm_PressRes(int iJoinId)
  {
  ASSERT(0); //Not yet implemented

  ASSERT(fInConfigureJoins);
  CheckQmAuxExists(Joins, iJoinId, this);
  JoinQmAux &QA=*Joins[iJoinId].pQmAux;
  QA.iNextCfg|=JQA_Press;
  };

//--------------------------------------------------------------------------

void FlwNode::SetJoinQm_FixedQm (int iJoinId)
  {
  ASSERT(0); //Not yet implemented

  ASSERT(fInConfigureJoins);
  CheckQmAuxExists(Joins, iJoinId, this);
  JoinQmAux &QA=*Joins[iJoinId].pQmAux;
  QA.iNextCfg|=JQA_RqdQm;
  };

//--------------------------------------------------------------------------

void FlwNode::SetJoinQm_QmRatio (int iJoinId, int iIONo)
  {
  ASSERT(fInConfigureJoins);
  CheckQmAuxExists(Joins, iJoinId, this);
  JoinQmAux &QA=*Joins[iJoinId].pQmAux;

  if (QA.iNextCfg==JQA_None)
    QA.nQmRatio=0;

  QA.iNextCfg|=JQA_QmRatio;

  if (QA.QmRatio.GetSize()<=QA.nQmRatio)
    QA.QmRatio.SetSize(QA.nQmRatio+1);
  QA.QmRatio[QA.nQmRatio].IONo=iIONo;
  QA.nQmRatio++;
  };

//--------------------------------------------------------------------------

void FlwNode::SetJoin_HasVolume(int JoinId, flag VolPrsFX, flag VolIsSmall)
  {
  m_nJoins=Max(m_nJoins, JoinId+1);
  Joins.SetSize(Max(Joins.GetSize(), JoinId+1));

  if (Joins[JoinId].fVolPrsFX!=VolPrsFX || Joins[JoinId].fVolIsSmall!=VolIsSmall)
    {
    SetTopologyChanged();
    DBGTOPOCHG("Join_HasVolume")
    }

  Joins[JoinId].fVolPrsFX=VolPrsFX;
  Joins[JoinId].fVolIsSmall=VolIsSmall;
  };

//--------------------------------------------------------------------------

void FlwNode::SetJoin_IORatio(int iJoinId, int iIONo, flag Adjustable)
  {
  ASSERT(fInConfigureJoins);
  CheckQmAuxExists(Joins, iJoinId, this);
  JoinQmAux &QA=*Joins[iJoinId].pQmAux;

  if (QA.iNextCfg==JQA_None)
    QA.nIORatio=0;

  QA.iNextCfg|=JQA_IORatio;

  if (QA.IORatio.GetSize()<=QA.nIORatio)
    QA.IORatio.SetSize(QA.nIORatio+1);
  QA.IORatio[QA.nIORatio].IONo=iIONo;
  QA.IORatio[QA.nIORatio].bAdjustable=Adjustable;
  QA.nIORatio++;
  };

//--------------------------------------------------------------------------

void FlwNode::SetJoinQm_Joined(int iJoinId, int OtherJoin)
  {
  ASSERT(0); //Not yet implemented

  ASSERT(fInConfigureJoins);
  //??
  };

//--------------------------------------------------------------------------

void FlwNode::SetJoin_PumpNd(int JoinId, flag On)
  {
  ASSERT(fInConfigureJoins);
  Joins[JoinId].fIsPumpNd=On;
  }

//--------------------------------------------------------------------------

void FlwNode::Set_IO_V2LQmFact(int i, double FV, double FL)
  {
  if (i>=NoFlwIOs())
    return;
  CIORec &S=IO_Self(i);

  flag Reqd=fabs(FV)>V2LLimit || fabs(FL)>V2LLimit;

//  m_IOs[i].dV2LFactor=F;

  m_IOs[i].dV2LQmVFact=FV;
  m_IOs[i].dV2LQmLFact=FL;
  if (Reqd!=S.fV2LReqd)
    {
    m_IOs[i].fV2LReqd=Reqd;
    SetTopologyChanged();
    DBGTOPOCHG("V2LQmFact")
    }

  };

//--------------------------------------------------------------------------

void FlwNode::SetJoinV2LFlow(int j, double Flow)//, double VFact, double LFact)
  {
  if (j>=Joins.GetSize())
    return;

  CJoinRec &J=Joins[j];

  flag Reqd=fabs(Flow)>V2LLimit;// || fabs(VFact)>V2LLimit || fabs(LFact)>V2LLimit;

  J.dV2LQmReqd=Flow;
//  J.dV2LQmVFact=VFact;
//  J.dV2LQmLFact=LFact;

  if (Reqd!=J.fJnV2LReqd)
    {
    J.fJnV2LReqd=Reqd;
    SetTopologyChanged();
    DBGTOPOCHG("V2LFlow")
    }

  };

//--------------------------------------------------------------------------

void FlwNode::SetJoinPressEquil(int j, double P)
  {
  if (j>=Joins.GetSize())
    return;

  CJoinRec &J=Joins[j];

  flag Reqd=Valid(P);

  J.dPressEquil=P;
  if (Reqd!=J.fJnV2LReqd)
    {
    J.fJnV2LReqd=Reqd;
    SetTopologyChanged();
    DBGTOPOCHG("PressEquil")
    }

  };

//--------------------------------------------------------------------------

void FlwNode::PreConfigureJoins()
  {
  m_nJoins        = 0;
  m_nProcessJoins = 0;
  m_nMkupJoins    = 0;
  m_nAuditJoins   = 0;
  m_nLinks        = 0;
  }

//--------------------------------------------------------------------------

//class CFailedAssertException : public CSimpleException
//  {
//  DECLARE_DYNAMIC(CFailedAssertException)
//  public:
//    CFailedAssertException();
//
//    // Implementation
//  public:
//    CFailedAssertException(BOOL bAutoDelete) {};
//    CFailedAssertException(BOOL bAutoDelete, LPCTSTR Message) : CSimpleException(bAutoDelete)
//      { 
//      m_bInitialized=true;
//     	m_bLoaded=true;
//	    strcpy_s(m_szMessage, _countof(m_szMessage), Message);
//      };
//    virtual ~CFailedAssertException() {};
//  };
//IMPLEMENT_DYNAMIC(CFailedAssertException, CSimpleException)

void FlwNode::PostConfigureJoins()
  {
  //m_nJoins=Joins.GetSize();
  Joins.SetSize(Max(m_nJoins, m_nRqdJoins));
  ASSERT(fInConfigureJoins);

  if (NoFlwIOs()>0 && (Joins.GetSize()==0) && ((m_dwType&(nc_Control|nc_CLnk|nc_Elec|nc_ELnk|nc_ALnk))==0))
    {
    LogError(FullObjTag(), 0, "NoFlwIOs()>0 & NJoins==0 [%s]", ClassId());
    }
  m_nProcessJoins = 0;
  m_nMkupJoins    = 0;
  m_nAuditJoins   = 0;
  for (int j=0; j<Joins.GetSize(); j++)
    {
    CJoinRec &J=Joins[j];
    if (J.fIsMkup)
      {
      m_nMkupJoins++;
      ASSERT_ALWAYS(m_nAuditJoins==0, "m_nAuditJoins!=0", __FILE__, __LINE__);
      }
    else if (J.fIsAudit)
      {
      m_nAuditJoins++;
      }
//    else if (J.fIsAudit)
//    const int IOId_Spill2Area     = SpillIOId(500);
//const int IOId_Vent2Area      = VentIOId(500);
//const int IOId_LeakI2Area     = LeakIOId(500);
//const int IOId_LeakO2Area     = LeakIOId(501);
//const int IOId_Makeup2Area    = MakeupIOId(500);
//const int IOId_Bleed2Area     = BleedIOId(500);
//
    else
      {
      m_nProcessJoins++;
      ASSERT_ALWAYS(m_nMkupJoins==0 && m_nAuditJoins==0, "m_nMkupJoins!=0 || m_nAuditJoins!=0", __FILE__, __LINE__);
      }

    if (J.pQmAux)
      {
      JoinQmAux &QA=*J.pQmAux;
      if (pNetTopoChg)
        {
        if (QA.iNextCfg!=QA.iCfg)
          {
          SetTopologyChanged();
          DBGTOPOCHG("QA.iCfg")
          }
        if (QA.nQmRatio!=QA.QmRatio.GetSize())
          {
          SetTopologyChanged();
          DBGTOPOCHG("QA.nQmRatio")
          }
        if (QA.nIORatio!=QA.IORatio.GetSize())
          {
          SetTopologyChanged();
          DBGTOPOCHG("QA.nIORatio")
          }
        }

      QA.iCfg=QA.iNextCfg;
      QA.iNextCfg=JQA_None;
      QA.QmRatio.SetSize(QA.nQmRatio);
      QA.IORatio.SetSize(QA.nIORatio);
      }
    }

  //m_nProcessJoins = m_nJoins; only a temporary test

  if (0)
    dbgpln("FlwNode::PostConfigureJoins m_nJoins %3i %3i %3i %3i %s", m_nJoins, m_nProcessJoins, m_nMkupJoins, m_nAuditJoins, Tag());
  };

//--------------------------------------------------------------------------

void FlwNode::InitFlowInfo()
  {
  #if dbgFlowInfo
    dbgpln("InitFlow : %2i %s", NoFlwIOs(), FullObjTag());
  #endif
  for (int i=0; i<NoFlwIOs(); i++)
    for (int fe=0; fe<NIOFBs(i); fe++)
      IOFB(i,fe)->InitFlowInfo(IOConduit(i));
  for (int i=0; i<m_Links.GetSize(); i++)
    m_Links[i].m_FBs[0].InitFlowInfo(&m_Links[i].m_Cd);
  }

//--------------------------------------------------------------------------

void FlwNode::StepFlowInfo()
  {
  #if dbgFlowInfo
    dbgpln("StepFlow : %2i %s", NoFlwIOs(), FullObjTag());
  #endif
  for (int i=0; i<NoFlwIOs(); i++)
    for (int fe=0; fe<NIOFBs(i); fe++)
      IOFB(i,fe)->StepFlowInfo();
  for (int i=0; i<m_Links.GetSize(); i++)
    m_Links[i].m_FBs[0].StepFlowInfo();
  }

//--------------------------------------------------------------------------

CSpPropInfo *FlwNode::IOFlwProps(int i, int fe)
  {
  return IOFB(i,fe)->Properties(NULL);
  };

//--------------------------------------------------------------------------

void FlwNode::EvalFlowInfoStart()
  {
  #if dbgFlowInfo
    dbgpln("EvalFlowS : %2i %s", NoFlwIOs(), FullObjTag());
  #endif
  for (int i=0; i<NoFlwIOs(); i++)
    {
   // if (IO_In(i))
   //   {
   //   // Copy FlwProps to NetProps
   //   // only for if flow
   //   CSpPropInfo &NP=*IONetProps(i);
   //   CSpPropInfo &FP=*IOFlwProps(i,0);
   //   NP.dNRhoV        = FP.dNRhoV;
   //   NP.dRhoL         = FP.dRhoL;
   //   NP.dTemp         = FP.dTemp;
   //   NP.dVisc         = FP.dVisc;
   //   NP.dVVolFrac     = FP.dVVolFrac;
   //
   //   NP.dVVolFracBal  = 0.0;
   //   }
   //
    for (int fe=0; fe<NIOFBs(i); fe++)
      {
      if (IOFB(i,fe)->EvalFlowInfoStart())
        {
        SetTopologyChanged();
        DBGTOPOCHG("EvalFlowInfoStart")
        }
      }
    }
  for (int i=0; i<m_Links.GetSize(); i++)
    {
    if (m_Links[i].m_FBs[0].EvalFlowInfoStart())
      {
      SetTopologyChanged();
      DBGTOPOCHG("EvalFlowInfoStart")
      }
    }
  };

//--------------------------------------------------------------------------

void FlwNode::EvalFlowInfoEnd()
  {
  #if dbgFlowInfo
    dbgpln("EvalFlowE : %2i %s", NoFlwIOs(), FullObjTag());
  #endif
  for (int i=0; i<NoFlwIOs(); i++)
    for (int fe=0; fe<NIOFBs(i); fe++)
      IOFB(i,fe)->EvalFlowInfoEnd();
  for (int i=0; i<m_Links.GetSize(); i++)
    m_Links[i].m_FBs[0].EvalFlowInfoEnd();
  };

//--------------------------------------------------------------------------

flag FlwNode::TestFlowInfo()
  {
  int OK=true;
  for (int i=0; i<NoFlwIOs(); i++)
    for (int fe=0; fe<NIOFBs(i); fe++)
      if (!IOFB(i,fe)->TestFlowInfo())
        OK=false;
  for (int i=0; i<m_Links.GetSize(); i++)
    if (!m_Links[i].m_FBs[0].TestFlowInfo())
      OK=false;
  return OK!=0;
  };

//--------------------------------------------------------------------------

int FlwNode::IOModifiedIn()
  {
  if (NoFlwIOs()!=2)
    return -1;
  bool Mod0=IOConduit(0)->Modified();
  bool Mod1=IOConduit(1)->Modified();
  if ((Mod0 && Mod1) || (!Mod0 && !Mod1))
    return -2;
  return Mod0?0:1;
  }

//--------------------------------------------------------------------------

void FlwNode::ODEOperate(CODEDataBlock & ODB)
  {
  switch (ODB.m_Cmd)
    {
    case eStateAdvance:
    case eStateConverge:
    case eStateLoad:
    case eStateDiskLoad:
      {
      ConvergeStates(CConvergeStateBlk(ODB));
      break;
      }
    case eStateFixDV:
    case eStateTest:
    case eStateSave:
    case eStateDiskSave:
      {
      break;
      }
    }
  };


//--------------------------------------------------------------------------
/*
void FlwNode::CompletePressureSens()
  {
  for (int j=0; j<Joins.GetSize(); j++)
    {
    double TQV=0.0;
    double TQL=0.0;
    double TQVSens=0.0;
    double TQLSens=0.0;
    //for (int i=0; i<NoFlwIOs(); i++)
    for (int c=0; c<Joins[j].NConns(); c++)
      {
      int i=Joins[j].IONo(c);
      SpConduit &Cd=*IOConduit(i);
      double QMassV=Cd.QMass(som_Gas);
      double QMassL=Cd.QMass(som_SL);

      TQV+=QMassV;
      TQL+=QMassL;
      TQVSens+=IOP_SensV(i)*QMassV;
      TQLSens+=IOP_SensL(i)*QMassL;
      }
    Set_JoinP_SensV(j, fabs(TQVSens)>1.0e-100 ? TQVSens/GTZ(TQV) : 0.0);
    Set_JoinP_SensL(j, fabs(TQLSens)>1.0e-100 ? TQLSens/GTZ(TQL) : 0.0);
    }
  };
*/
//--------------------------------------------------------------------------

void FlwNode::BuildJoinList()
  {
  if (0)
    {
    static int XXX=0;
    if (!XXX)
      {
      dbgpln("NIOs %7s %7s %7s %7s %7s %7s %7s %7s %7s %7s %7s %7s %7s",
              "IOs","FlwsIOs","ProcIOs","PLnkIOs","PDirIOs",
              "AttDIOs","EnaDIOs","AdtDIOs",
              "fProc","fMkup","lMkup","fAudt","lAudt");
      XXX=1;
      }

    dbgpln("NIOs %7i %7i %7i %7i %7i %7i %7i %7i %7i %7i %7i %7i %7i %s",
            NoIOs(),NoFlwIOs(),NoProcessIOs(),NoProcLnkIOs(),NoProcDirectIOs(),
            NoAttachedDirectIOs(),NoEnabledDirectIOs(),NoAuditDirectIOs(),
            FirstProcDirect(),FirstMkupDirect(),LastMkupDirect(),FirstAuditDirect(),LastAuditDirect(),
            Tag());
    }

  for (int i=0; i<NoFlwIOs(); i++)
    {
    m_IOs[i].fConnToRmt=(m_IOs[i].iJoinTp&(IOJn_Joined|IOJn_Closed|IOJn_Open))!=0;
    for (int fe=0; fe<NIOFBs(i); fe++)
      {
      IOFB(i,fe)->m_bAttached2GFB=false;
      IOFB(i,fe)->m_pGFb=NULL;
      IOFB(i,fe)->m_bPlaced=false;
      IOFB(i,fe)->m_iFxdPOrder=-1;
      }
    for (int pb=0; pb<NIOPBs(i); pb++)
      IOPB(i,pb).m_iFxdPOrder=-1;
    }

  for (int j=0; j<m_nLinks; j++)
    {
    CLinkRec &L=m_Links[j];
    L.m_pNd=this;
    L.m_pGFb=NULL;
    L.m_iJoinConnNo0=-1;
    L.m_iJoinConnNo1=-1;
    L.m_iSgn=0;
    for (int f=0; f<L.m_FBs.GetSize(); f++)
      {
      L.m_FBs[f].m_bAttached2GFB=false;
      L.m_FBs[f].m_pGFb=NULL;
      L.m_FBs[f].m_bPlaced=false;
      }
    }

  Joins.SetSize(Max(m_nRqdJoins, m_nJoins));
  m_nJoins=Joins.GetSize();

  //m_nProcessJoins=0;
  for (int j=0; j<m_nJoins; j++)
    {
    CJoinRec &J=Joins[j];
    J.iJoinArrayInx=j;
    J.pNd=this;
    J.pGFb=NULL;
    J.m_iJoinId=j;
    J.m_nConns=0;
    J.fDamped=False;
    J.iSeqNo=0;
    J.iQmToSet=0;
    J.fBusy=False;
    J.fHasDbgBrk=false;
    J.fJnV2LReqd=fabs(J.dV2LQmReqd)>V2LLimit;
    J.fIOV2LReqd=false;
    }

  fEvalPressSensReqd=false;
  for (i=0; i<NoFlwIOs(); i++)
    {
    if (IO_Join_Typ(i)&IOJn_Open)
      {
      if (m_IOs[i].fDamped)
        {
        fEvalPressSensReqd=true;
        Joins[IO_Join_Id(i)].fDamped=true;
        }
      Joins[IO_Join_Id(i)].fIOV2LReqd|=m_IOs[i].fV2LReqd;
      }
    }
  //  }

  for (i=0; i<NoFlwIOs(); i++)
    {
    CIORec &R=IO_Self(i);
    if (m_IOs[i].fConnToRmt)
      {
      CJoinRec &J=Joins[IO_Join_Id(i)];
      R.iJoinConnNo=J.m_nConns++;
     }
    else
      R.iJoinConnNo=MaxIOList+1;
    }

  for (i=0; i<m_nLinks; i++)
    {
    CLinkRec &L=m_Links[i];
    CJoinRec &J0=Joins[L.m_iJoinId0];
    L.m_iJoinConnNo0=J0.m_nConns++;
    CJoinRec &J1=Joins[L.m_iJoinId1];
    L.m_iJoinConnNo1=J1.m_nConns++;
    }

  int JoinConnCnt[MaxIOList];
  for (j=0; j<NoJoins(); j++)
    {
    JoinConnCnt[j]=0;
    CJoinRec &J=Joins[j];
    J.fHasQmAux = (J.pQmAux!=NULL);
    J.fMatrixChanged=1;
    J.fProdFracsChgd=1;
    J.m_Conns.SetSize(J.m_nConns);
    for (int c=0; c<J.m_nConns; c++)
      {
      J.m_Conns[c].m_iRmtJoin=-1;
      J.m_Conns[c].m_iRmtConn=-1;
      }
    }

  for (i=0; i<NoFlwIOs(); i++)
    {
    if (m_IOs[i].fConnToRmt)
      {
      int j=IO_Join_Id(i);
      int n=JoinConnCnt[j]++;
      CJConnectRec &JC=Joins[j].m_Conns[n];
      JC.m_pFlng=IOFlange(i);
      JC.m_pFBs=IOFBs(i);
      JC.m_pPBs=&IOPBs(i);
      JC.m_pRmtNd=Nd_Rmt(i);
      JC.m_iRmtJoin=-1; // Still to fill in (done in ConnectJoinList)
      JC.m_iIONo=i;
      JC.m_iLnkNo=-1;
      }
    }
  for (i=0; i<m_nLinks; i++)
    {
    CLinkRec &L=m_Links[i];
    for (int ii=0; ii<2; ii++)
      {
      int j=(ii==0 ? L.m_iJoinId0 : L.m_iJoinId1);
      int n=JoinConnCnt[j]++;
      CJConnectRec &JC=Joins[j].m_Conns[n];
      JC.m_pFlng=NULL;//IOFlange(i);
      JC.m_pFBs=&L.m_FBs;//IOFBs(i);
      JC.m_pPBs=&L.m_PBs;//&IOPBs(i);
      JC.m_pRmtNd=this;//Nd_Rmt(i);
      JC.m_iRmtJoin=-1; // Still to fill in (done in ConnectJoinList)
      JC.m_iIONo=-1;
      JC.m_iLnkNo=i;
      }
    }

  for (j=0; j<NoJoins(); j++)
    {
    CJoinRec &J=Joins[j];
    for (int pf=0; pf<J.ProdFracs.GetSize(); pf++)
      {
      J.ProdFracs[pf].dProdFrac=dNAN;
      J.ProdFracs[pf].fRestrict=0;
      }

    if (J.pQmAux)
      {
      JoinQmAux &QA=*J.pQmAux;
      for (int r=0; r<QA.nQmRatio; r++)
        {
        QA.QmRatio[r].ConnNo=MaxIOList;
        for (int c=0; c<J.m_nConns; c++)
          if (J.m_Conns[c].m_iIONo==QA.QmRatio[r].IONo)
            {
            QA.QmRatio[r].ConnNo=c;
            break;
            }
        if (QA.QmRatio[r].ConnNo==MaxIOList)
          {
          LogError(J.Nd()->FullObjTag(), 0, "Bad Join QmRatio : IO Not found");
          QA.iCfg = QA.iCfg & ~JQA_QmRatio;
          }
        }


      // Each new Adjustable Link is followed by the links that it refs
      int nAdj=0, BadAdj=0;
      int nFree=1, BadFree=0;
      for (r=0; r<QA.nIORatio; r++)
        {
        QA.IORatio[r].ConnNo=MaxIOList;
        for (int c=0; c<J.m_nConns; c++)
          {
          if (J.m_Conns[c].m_iIONo==QA.IORatio[r].IONo)
            {
            QA.IORatio[r].ConnNo=c;
            break;
            }
          }
        if (QA.IORatio[r].bAdjustable)
          {
          nAdj=1;
          if (nFree==0)
            BadFree=1;
          nFree=0;
          }
        else
          {
          nFree++;
          if (nAdj!=1)
            BadAdj=1;
          }
        if (QA.IORatio[r].ConnNo==MaxIOList)
          {
          LogError(J.Nd()->FullObjTag(), 0, "Bad Join IORatio : IO Not found");
          QA.iCfg = QA.iCfg & ~JQA_IORatio;
          }
        }
      if (BadFree)
        {
        LogError(J.Nd()->FullObjTag(), 0, "Bad Join IORatio : Bad Free Links ");
        QA.iCfg = QA.iCfg & ~JQA_IORatio;
        }
      if (BadAdj)
        {
        LogError(J.Nd()->FullObjTag(), 0, "Bad Join IORatio : Bad Adjustable Links ");
        QA.iCfg = QA.iCfg & ~JQA_IORatio;
        }
      }
    }

#if dbgFlwNode
  if (dbgBuildJoinList())
    dbgpln("FlwNode::BuildJoinList      m_nJoins %i %s", m_nJoins, Tag());
#endif
  }

//--------------------------------------------------------------------------

void FlwNode::ConnectJoinList()
  {
  for (int i=0; i<NoFlwIOs(); i++)
    {
    if (m_IOs[i].fConnToRmt)
      {
      int js=IO_Self(i).iJoinId;
      int cs=IO_Self(i).iJoinConnNo;
      //int tr=IO_Rmt(i).iJoinTp;
      int jr=IO_Rmt(i).iJoinId;
      int cr=IO_Rmt(i).iJoinConnNo;

      if (IO_Rmt(i).fConnToRmt)
        {
        CJConnectRec &JCs=Joins[js].m_Conns[cs];
        JCs.m_iRmtJoin=jr;
        JCs.m_iRmtConn=cr;
        }
      }
    }

  for (i=0; i<m_nLinks; i++)
    {
    CLinkRec & L=m_Links[i];
    int js=L.m_iJoinId0;
    int cs=L.m_iJoinConnNo0;
    int jr=L.m_iJoinId1;
    int cr=L.m_iJoinConnNo1;

    CJConnectRec &JCs=Joins[js].m_Conns[cs];
    JCs.m_iRmtJoin=jr;
    JCs.m_iRmtConn=cr;

    CJConnectRec &JCr=Joins[jr].m_Conns[cr];
    JCr.m_iRmtJoin=js;
    JCr.m_iRmtConn=cs;
    }

  #if dbgFlwNode
  if (dbgJoinsBld())
    {
    dbgpln("ConnectJoinList:%s [%s]", FullObjTag(), ClassId());
    for (i=0; i<NoFlwIOs(); i++)
      {
      Strng S;
      if (IO_Closed_Self(i))
        S="Closed_Self";
      else if (IO_Closed_Rmt(i))
        S="Closed_Self_Rmt";
      else if (IO_Direct_Self(i))
        S="Direct_Self";
      else if (IO_Direct_Rmt(i))
        S="Direct_Rmt ";
      else if (IO_Open_Self(i))
        S="Open_Self  ";
      else if (IO_Open_Rmt(i))
        S="Open_Rmt   ";
      else
        S.Set("Jn[%2i]     ", IO_Join_Id(i));
      dbgpln("IO   %3i) [%5i %5i %5i %5i] %-20s -> %s",i, 
        IO_Self(i).iJoinId,
        IO_Self(i).iJoinConnNo,
        IO_Rmt(i).iJoinId,
        IO_Rmt(i).iJoinConnNo,
        S(), Nd_Rmt(i)->FullObjTag());
      }
    for (i=0; i<m_nLinks; i++)
      {
      CLinkRec & L=m_Links[i];
      dbgpln("Link %3i) Jn[%2i]  -> Jn[%2i]",i, L.m_iJoinId0, L.m_iJoinId1);
      }
    }
  #endif
  }

//--------------------------------------------------------------------------

void FlwNode::PostConnectJoinList()
  {
  //dbgpln("PostConnectJoinList    %s", FullObjTag());
  for (int j=0; j<NoJoins(); j++)
    {
    CJoinRec &J=Joins[j];
    J.SetTieLnkFlags();
    J.SetQmAccum(0,0.0);
    J.SetQmAccum(1,0.0);
    
    //dbgpln("%3i) %s %s", j, J.fIs2IO?"Is2IO":"     ", J.fIsTie?"IsTie":"     ");
    }
  }

//--------------------------------------------------------------------------

void FlwNode::CleanupConnections()
  {
  for (int i=0; i<NoFlwIOs(); )
    {
    if (!Nd_Rmt(i))
      {
      PreDisConnect(i);
      Detach(i);
      LogWarning(FullObjTag(), 0, "IO Adjusted for missing Connection");
      //dbgpln("Detach Bad IO %3i %s", i, FullObjTag());
      //Detach(i);
      }
    else
      i++;
    }
  };

//--------------------------------------------------------------------------

flag FlwNode::ConnectionsOK()
  {
  if (IOAreas==NULL)
    return True;
  flag BadConn=0;
  int PrevId=-1;
  IOAreaRec * pA=IOAreas;
  while (pA->m_fValid)
    {
    if (pA->m_Id!=PrevId && pA->m_pMapTo==NULL) // ie not Mapped to other IO
      {
      int n=NIOsWithId_Self(pA->m_Id);
      int nm=pA->m_nIORqd;
      int nx=pA->m_nIOMax;
      dword Grp=(pA->m_dwFlags & dwIOGRP);
      flag DoTest=0;
      if (Grp>0)
        {
        IOAreaRec * pA1=IOAreas;
        while (!DoTest && pA1->m_fValid)
          {
          if ((Grp==(pA1->m_dwFlags & dwIOGRP)) && (NIOsWithId_Self(pA1->m_Id)>0))
            DoTest=1;
          pA1++;
          }
        }
      else
        DoTest=1;
      if (DoTest)
        if (n < nm)
          {
          LogError(FullObjTag(), 0, "Too few Connections to %s", pA->IOName());
          BadConn=1;
          }
        else if (n > nx)
          {
          // NBNB this path should never be executed because of checks at connect time.
          LogError(FullObjTag(), 0, "Too Many Connections to %s", pA->IOName());
          BadConn=1;
          }
      }
    PrevId=pA->m_Id;
    pA++;
    }
  return !BadConn;
  }

//--------------------------------------------------------------------------

#if KWIKCONNECT

int FlwNode::NIOsWithId_Self(int  RqdId)
  {
  for (int i=0; IOAreas[i].IOName(); i++)
    {
    if (IOAreas[i].m_Id==RqdId)
      return m_IOAreaLcl[i].m_nConnected;
    }
  return 0;
  };

//--------------------------------------------------------------------------

int FlwNode::IOWithId_Self(int  RqdId)
  {
  int iIO=0;
  for (int i=0; IOAreas[i].IOName(); i++)
    {
    if (IOAreas[i].m_Id==RqdId)
      {
      if (m_IOAreaLcl[i].m_nConnected==0)
        iIO=-1;
      goto Found;
      }
    iIO+=m_IOAreaLcl[i].m_nConnected;
    }
  // Not Found - not in list
  iIO=-1;

Found:
  if (0)//i!=iIO)
    {
    dbgpln("=======================");
    //dbgpln("  %5i %5i", IOId_Self(i), RqdId);
    for (int x=0; IOAreas[x].IOName(); x++)
      dbgpln("IOIdL : %4i: %4i %4i", x, IOAreas[x].m_Id, m_IOAreaLcl[x].m_nConnected);

    for (int x=0; x<NoFlwIOs(); x++)
      dbgpln("IOId  : %4i: %i", x, IOId_Self(x));

    //__debugbreak();
    dbgpln("= %i [%i]", iIO, RqdId);
    }
  //if (1) // only for test
  //  {
  //  for (int i = 0; i < NoFlwIOs(); i++)
  //    {
  //    if (IOId_Self(i)==RqdId)
  //      {
  //      break;
  //      }
  //    }
  //  }
  return iIO;
  };

//--------------------------------------------------------------------------

//int FlwNode::IOWithId_Rmt(int  RqdId)
//  {
//  for (int i = 0; i < NoFlwIOs(); i++)
//    {
//    if (IOId_Rmt(i)==RqdId)
//      return i;
//    }
//  return -1;
//  };

#endif

//--------------------------------------------------------------------------

flag FlwNode::MoveTearFlange(int iTearIO, FlwNode * &pNewTearNd, int &iNewTearIO)
  {
  pNewTearNd = NULL;
  if (fDoDbgBrk)
    { int xxx=0;};
  if (iTearIO>=2)
    return False;
  if (NetDynamicMethod())
    {
    if (!IsLnk() || Internal)
      return False;
    }
  else
    {
    if (Internal)
      return False;
    if (NoProcLnkIOs()!=2 || !IO_Out(iTearIO) || !IO_In(OtherEnd(iTearIO)))
      return False;
    }
  if (IO_Out(iTearIO))
    {
    // Move to Beginning
    pNewTearNd = this;
    iNewTearIO = OtherEnd(iTearIO);

    FlwNode * pRmtNd=pNewTearNd->Nd_Rmt(iNewTearIO);
    int iRmtIO=pNewTearNd->IOIONo_Rmt(iNewTearIO);
    FlwNode * pRmtTearNd;
    int iRmtTearIO;
    if (pRmtNd->MoveTearFlange(iRmtIO, pRmtTearNd, iRmtTearIO))
      {
      pNewTearNd=pRmtTearNd;
      iNewTearIO=iRmtTearIO;
      }
    return True;
    }
  return False;
  }

//--------------------------------------------------------------------------

int FindIOAreaProc(pchar TagIn, IOAreaRec * al)
  {
  for (int i=0; al[i].IOName() && al[i].IOName()[0]; i++)
    {
    for (int n=0; n<al[i].IONameCount(); n++)
      {
      if (stricmp(TagIn, al[i].IOName(n))==0)
        return i;
      }
    }
  return -1;
  };

//--------------------------------------------------------------------------

flag FlwNode::FindIOTag(pchar TagIn, int OId_Rqd, IOInfo &Info)
  {
  SCDPartTag    FirstPart;
  pchar         Remainder;
  int           i;
  char          Found;

  SplitTag(TagIn, FirstPart, Remainder);
  Found = 0;
  if (IOAreas)// && (1 || !Remainder))
    {
    i = FindIOAreaProc(FirstPart, IOAreas);
    if (i<0 && Remainder) // try for the whole thing
      i = FindIOAreaProc(TagIn, IOAreas);
    if (i>=0)
      {
      if (IOAreas[i].m_pMapTo != NULL && IOAreas[i].m_pMapTo[0] != NULL)
        {
        SCDFullTag    Tx;
        strcpy(Tx,IOAreas[i].m_pMapTo);
        SplitTag(Tx, FirstPart,Remainder);
        }
      else
        {
        Info.p       = this;
        Info.OId     = OId_NodeIO;
        Info.AreaInx = i;
        Info.Id    = IOAreas[i].m_Id;
        Found = ((OId_Rqd==0) || (OId_Rqd==OId_NodeIO));
        };
      }
    };
  if (!Found)
    {
    Info.p = (FlwNode *)FindChildTag(FirstPart);
    if (Info.p)
      if (Remainder)
        Found = Info.p->FindIOTag(Remainder, OId_Rqd, Info);
      else
        {
        Info.OId     = OId_Node;
        Info.AreaInx = 0;
        Info.Id      = -1;
        Found = ((OId_Rqd==0) || (OId_Rqd==OId_Node));
        };
    };
  if (!Found)
    {
    int xxx=0;
    }
  return(Found);
  };

//--------------------------------------------------------------------------

int FlwNode::Attach(int LIONo, FlwNode * R, int RIONo,
                    CFlange* pF, CTerminal * pT,
                    //CCtrlData * pC, CElecConduit * pE, CAirConduit * pA,
                    int RqdIONo, int FlngDirn)
  {
  int     AreaId, ion;

  AreaId = IOAreas[LIONo].m_Id;

  if (RqdIONo>=0)
    {
    if (RqdIONo>=NoIOs())
      {
      m_IOs.SetMySize(RqdIONo+1);//m_IOs.SetSize(RqdIONo+1);
      ion=RqdIONo;
      }
    else
      ion=RqdIONo;
    }
  else
    {
    for (ion=0; ion<NoIOs() ; ion++)
      {
      if (IOAreas[m_IOs[ion].m_iIODescSelf].m_Id > AreaId)
        break;
      }
    m_IOs.InsertAt(ion, new CIORec);
    }

  CIORec &io=m_IOs[ion];
  dword flags=IOAreas[LIONo].m_dwFlags;

  flag IsProc=(IOAreas[LIONo].m_dwType & nc_MLnk)!=0;
  flag IsCtrl=(IOAreas[LIONo].m_dwType & nc_CLnk)!=0;
  flag IsElec=(IOAreas[LIONo].m_dwType & nc_ELnk)!=0;
  flag IsAir=(IOAreas[LIONo].m_dwType & nc_ALnk)!=0;

  io.Rmt          = R;
  io.m_iIONoRmt   = 0;
  io.fIsComplete  = False;
  io.m_iIODescRmt = RIONo;
  io.m_iIODescSelf= LIONo;
  io.fAutoConnect = false;
  io.pFlng        = pF;
  io.m_pTerm      = pT;
  io.pFBs         = new FlwBlkArray;
  io.pPBs         = new CPressBlkArray;
  io.m_pChgs      = new CTraceArray;
  if (pF)
    {
    io.SetNFBs(1);
    CFlwEqnGrp * pFG = IOAreas[LIONo].m_pFlwGroup;
    if (pFG)
      io.FBs()[0].AssignFlwEqnGroup(*pFG, pFG->Default(), this);

    io.dFracHgt       = IOAreas[LIONo].m_dFracHgt;
    io.dAperture      = 0.0;
    io.dApertureAngle = (flags & IOApertureHoriz) ? 0.0 : Degs2Rads(90.0);
    io.dEntrainRateA  = 0.0;
    io.dEntrainRateB  = 0.0;
    io.dEstV2LOnEntry = 0.0,
    io.dEstL2VOnEntry = 0.0;

    io.iExitRule   = 0;
    io.m_pImg      = NULL;
    if (flags & IOPassVapours)
      io.iExitRule |= IOER_Vap;
    if (flags & IOPassLiquids)
      io.iExitRule |= IOER_Liq;
    if (flags & IOPassSolids)
      io.iExitRule |= IOER_Sol;

    if (io.iExitRule==0)
      io.iExitRule  = IOER_Lvl;

    io.dV2LQmVFact  = 0.0;
    io.dV2LQmLFact  = 0.0;

    io.dRhoHSensV   = 0.0;
    io.dRhoHSensL   = 0.0;

    io.dPSensV      = 0.0;
    io.dPSensL      = 0.0;

    io.iHgtOrd      = -1;
    io.fSameHgt     = 0;
    //io.PB.Init();
    io.fIsSetXfer   = IOISSETXFER(IOAreas[LIONo].m_dwFlags);
    io.fIsBuffer    = IOISBUFFER(IOAreas[LIONo].m_dwFlags);
    if (io.fIsBuffer)
      {
      pF->fIsBufferPos=true;
      pF->SetTearPriority(TP_First);
      }
    }
  if (pF)
    pF->m_Attachments++;

  if (pT)
    pT->m_Attachments++;

#if KWIKCONNECT
  m_IOAreaLcl[LIONo].m_nConnected++;
#endif

  if (RqdIONo<0)
    {
    for (int l=ion ;l<NoIOs() ;l++)
      {
      //m_IOs[l+1] = OldIOs[l];
      m_IOs[l+1].Rmt->m_IOs[m_IOs[l+1].m_iIONoRmt].m_iIONoRmt++;
      if (m_IOs[l+1].pFlng)
        IOFlange(l+1)->AdjustConnection(this, +1);
      };
    }

  //dbgpln("%2i %08x %08x %08x %s", ion, m_IOs[ion].pFBs, m_IOs[ion].pPBs, m_IOs[ion].m_pChgs, sTag());

  //delete[] OldIOs;

  m_nIOs=m_IOs.GetSize();
  if (IsCtrl)
    m_nCIOs++;
  else if (IsElec)
    m_nEIOs++;
  else if (IsAir)
    m_nAIOs++;
  else
    {
    m_nFIOs++;
    if (IsProcessIOId(AreaId))
      {
      m_nProcessIOs++;
      if (IsMLnkIOId(AreaId))
        m_nProcLnkIOs++;
      //else
      //  m_nProcDirectIOs++;
      }
    //if (IsMakeupIOId(AreaId) || IsBleedIOId(AreaId))
    //  m_nMkupIOs++;
    }

  m_iCIO1=m_nFIOs;
  m_iEIO1=m_iCIO1+m_nCIOs;
  m_iAIO1=m_iEIO1+m_nEIOs;

  StructureChanged(this);

  return(ion);
  };

//--------------------------------------------------------------------------

int FlwNode::Detach(int IONo)
  {
  int    l;
  //pIORec OldIOs;

  //dbgpln("%2i %08x %08x %08x %s", IONo, m_IOs[IONo].pFBs, m_IOs[IONo].pPBs, m_IOs[IONo].m_pChgs, sTag());
  CIORec &io=m_IOs[IONo];
  int AreaId = IOAreas[io.m_iIODescSelf].m_Id;

#if KWIKCONNECT
  m_IOAreaLcl[io.m_iIODescSelf].m_nConnected--;
#endif

  delete io.m_pChgs;
  delete io.pPBs;
  delete io.pFBs;

  CFlange* pF=m_IOs[IONo].pFlng;
  CTerminal* pT=m_IOs[IONo].m_pTerm;
  if (pF && (--pF->m_Attachments)==0 && pF->m_bMustDelete)
    delete pF;
  if (pT && (--pT->m_Attachments)==0)
    delete pT;

  flag IsProc=(IODesc_Self(IONo)->m_dwType & nc_MLnk)!=0;
  flag IsCtrl=(IODesc_Self(IONo)->m_dwType & nc_CLnk)!=0;
  flag IsElec=(IODesc_Self(IONo)->m_dwType & nc_ELnk)!=0;
  flag IsAir =(IODesc_Self(IONo)->m_dwType & nc_ALnk)!=0;

  if (0)
    {
    dbgpln("* %s", FullObjTag());
    for (int i=0; i<m_IOs.GetCount(); i++)
      {
      //dbgp(" %3i", m_IOs[i].m_iIONoRmt);
      dbgpln("  %s %2i %3i %s",
        i==IONo?"*":" ",
        m_IOs[i].m_iIONoRmt,
        m_IOs[i].m_iIONoRmt<m_IOs[i].Rmt->m_IOs.GetCount()?m_IOs[i].Rmt->m_IOs[m_IOs[i].m_iIONoRmt].m_iIONoRmt:-1,
        m_IOs[i].Rmt->FullObjTag()
        );
      }
    }

  //OldIOs = m_IOs;
  if (m_nIOs > 1)
    {
    delete m_IOs.ElementAt(IONo);
    m_IOs.RemoveAt(IONo);
    m_nIOs=m_IOs.GetSize();
    for (l = IONo ; l < m_nIOs; l++)
      {
      if (m_IOs[l].Rmt)
        m_IOs[l].Rmt->m_IOs[m_IOs[l].m_iIONoRmt].m_iIONoRmt--;
      if (m_IOs[l].pFlng)
        IOFlange(l)->AdjustConnection(this, -1);
      };
    }
  else
    {
    m_IOs.SetMySize(0); //m_IOs.SetSize(0);
    m_nIOs=m_IOs.GetSize();
    }

  if (IsCtrl)
    m_nCIOs--;
  else if (IsElec)
    m_nEIOs--;
  else if (IsAir)
    m_nAIOs--;
  else
    {
    m_nFIOs--;
    if (IsProcessIOId(AreaId))
      {
      m_nProcessIOs--;
      if (IsMLnkIOId(AreaId))
        m_nProcLnkIOs--;
      //else
      //  m_nProcDirectIOs--;
      }
    //if (IsMakeupIOId(AreaId) || IsBleedIOId(AreaId))
    //  m_nMkupIOs--;
    }

  if (0)
    {
    dbgpln("* %s", FullObjTag());
    for (int i=0; i<m_IOs.GetCount(); i++)
      {
      //dbgp(" %3i", m_IOs[i].m_iIONoRmt);
      dbgpln("  %s %2i %3i %s",
        i==IONo?"*":" ",
        m_IOs[i].m_iIONoRmt,
        m_IOs[i].m_iIONoRmt<m_IOs[i].Rmt->m_IOs.GetCount()?m_IOs[i].Rmt->m_IOs[m_IOs[i].m_iIONoRmt].m_iIONoRmt:-1,
        m_IOs[i].Rmt->FullObjTag()
        );
      }
    }

  m_iCIO1=m_nFIOs;
  m_iEIO1=m_iCIO1+m_nCIOs;
  m_iAIO1=m_iEIO1+m_nEIOs;

  StructureChanged(this);

  return(1);
  };

//--------------------------------------------------------------------------

int FlwNode::ConnectTag(FlwNode * Root, pchar Tag1, pchar Tag2, int RqdIONo1, int RqdIONo2, char * ConnectTag)
  {
  int          Ret=0;
  IOInfo       Info[2];

  if (Root==NULL)
    Root=this;

  if (Root->FindIOTag(Tag1, OId_NodeIO, Info[0]) & Root->FindIOTag(Tag2, OId_NodeIO, Info[1]))
    {
    // check same type connection
    dword dwClass0=Info[0].p->IODesc_SelfId(Info[0].Id)->m_dwType;
    dword dwClass1=Info[1].p->IODesc_SelfId(Info[1].Id)->m_dwType;

    if (dwClass0!=dwClass1)
      {
      LogError(FullObjTag(), 0, "Incompatible Connections");
      Ret=NErr_Incompatibleconnections;
      }


    // check not already connected
    for (int j=0; j<2; j++)
      {
      FlwNode *p=Info[j].p;
      int n=0;
      for (int i=0; i < p->m_nIOs; i++)
        if (p->m_IOs[i].Rmt && p->IOId_Self(i)==Info[j].Id)
          n++;

      if(n>=p->IODesc_SelfId(Info[j].Id)->m_nIOMax)
        {
        LogError(p->FullObjTag(), 0, "Too many Flanges to %s.%s", p->FullObjTag(), p->IODesc_SelfId(Info[j].Id)->IOName());
        Ret=NErr_Toomanyconnections;
        }
      }

    flag ConnExists=0;
    for (int i=0; i<Info[0].p->m_nIOs; i++)
      if ((Info[0].p->Nd_Rmt(i)==Info[1].p) &&
          (Info[0].p->IOId_Rmt(i)==Info[1].Id))
        ConnExists=1;

    if (!ConnExists && Ret==0)
      {
      CFlange *pF = NULL;
      bool DoInitConn=false;
      if (dwClass0&nc_MLnk)
        {
        for (int i=0; !pF && i<2; i++)
          pF=Info[i].p->GetFlange(Info[i].Id);
        if (!pF)
          {
          pF                = (CFlange*)CFlangeClass.Construct("Flange" ,"" ,NULL , TOA_Free);// new CFlange(true);
          pF->m_bMustDelete = true;
          DoInitConn=true;
          }
        else
          {
          dbgpln("Existing Flange: %5i -> %5i", Info[0].Id, Info[1].Id);
          }
        }
      CTerminal *pT = NULL;
      if (dwClass0&nc_CLnk)
        pT=(CCtrlTermX*)CCtrlTermXClass.Construct("CtrlTermX" ,"TermTag" ,NULL , TOA_Free);// 
      else if (dwClass0&nc_ELnk)
        {
//ELEC         CETermStripDefn *pTerm0=Info[0].p->IODesc_SelfId(Info[0].Id)->m_pTerms;
//ELEC         CETermStripDefn *pTerm1=Info[1].p->IODesc_SelfId(Info[1].Id)->m_pTerms;
//ELEC         if (pTerm0 && pTerm1)
//ELEC           {
//ELEC           LogError(FullObjTag(), 0, "Too many Wire Definitions %s.%s", Info[0].p->FullObjTag(), IODesc_SelfId(Info[0].Id)->IOName());
//ELEC           Ret=NErr_Incompatibleconnections;
//ELEC           }
//ELEC         else if (pTerm0)
//ELEC           pT=new CElecTermX(*pTerm0);
//ELEC         else if (pTerm1)
//ELEC           pT=new CElecTermX(*pTerm1);
//ELEC         else
        pT=(CElecTermX*)CElecTermXClass.Construct("ETerminalX" ,"ETermTag" ,NULL , TOA_Free);// 
        }
      else if (dwClass0&nc_ALnk)
        pT=(CAirTermX*)CAirTermXClass.Construct("AirTermX" ,"ATermTag" ,NULL , TOA_Free);// 
      
      Ret = Connect(Info[0], Info[1], pF, pT, RqdIONo1, RqdIONo2);
      if (pF && DoInitConn)
        {
        pF->InitConnId(Info[0].p, Info[0].p->IODesc_Self(Info[0].IOIndex)->IOName(0), 
                       Info[1].p, Info[1].p->IODesc_Self(Info[1].IOIndex)->IOName(0), NULL);
        //pF->m_pRefNd      = (Info[0].p->m_dwType & nc_MLnk) ? Info[1].p : Info[0].p;
        if (ConnectTag)
          pF->FixTag(ConnectTag, true);
        else
          pF->FixTag();
        }
      }
    else if (Ret==0)
      {
      LogError(Tag1, 0, "Already Connected --> %s", Tag2);
      Ret = NErr_AlreadyConnected;
      }
    }
  else
    {
    LogError(Tag1, /*LF_Exclamation*/0, "NOT Connected --> %s", Tag2); // this dialog biox creates havoc wioth templates if they fail
    Ret = NErr_NotFound;
    }
  #if dbgConnect
  dbgnln;
  dbgpln("Connect     : %s -> %s [%i]",Tag1,Tag2,Ret);
  #endif
  return Ret;
  }

//--------------------------------------------------------------------------

int FlwNode::ConnectIOTag(FlwNode * s, pchar IOTag1, FlwNode * d, pchar IOTag2, IOInfoBlk *pInfo, char * ConnectTag)
  {
  if (0)
    dbgpln("ConnectIOTag %25s.%-15s %25s.%-15s @ %s", s->FullObjTag(), IOTag1, d->FullObjTag(), IOTag2, FullObjTag());

  CStopWatch SW;
  SW.Start();
  int          Ret=0;
  IOInfoBlk    InfoB;
//  IOInfoBlk  & Info = (pInfo ? *((IOInfoBlk*)pInfo) : InfoB);
  if (pInfo==NULL)
    pInfo = &InfoB;
  IOInfoBlk &Info=*pInfo;

  if (s->FindIOTag(IOTag1, OId_NodeIO, Info[0]) & d->FindIOTag(IOTag2, OId_NodeIO, Info[1]))
    {

    //dbgpln("%25s.%-15s AreaInx:%6i Id:%6i IOIndex:%6i OId:%6i %s", s->FullObjTag(), IOTag1, Info[0].AreaInx, Info[0].Id, Info[0].IOIndex, Info[0].OId, Info[0].p->FullObjTag());
    //dbgpln("%25s.%-15s AreaInx:%6i Id:%6i IOIndex:%6i OId:%6i %s", d->FullObjTag(), IOTag2, Info[1].AreaInx, Info[1].Id, Info[1].IOIndex, Info[1].OId, Info[1].p->FullObjTag());

#if dbgKwikConnect
    dbgpln(" .a %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
#endif
    // check same type connection
    dword dwClass0=Info[0].p->IODesc_SelfId(Info[0].Id)->m_dwType;
    dword dwClass1=Info[1].p->IODesc_SelfId(Info[1].Id)->m_dwType;
    if (dwClass0!=dwClass1)
      {
      LogError(FullObjTag(), 0, "Incompatible Connections");
      Ret=NErr_Incompatibleconnections;
      }

    for (int j=0; j<2; j++)
      {
      int n=Info[j].p->NIOsWithId_Self(Info[j].Id);

      if(n>=Info[j].p->IODesc_SelfId(Info[j].Id)->m_nIOMax)
        {
        LogError(Info[0].p->FullObjTag(), 0, "Too many Flanges to %s", Info[j].p->IODesc_SelfId(Info[j].Id)->IOName());
        Ret=NErr_Toomanyconnections;
        }
      }

    flag ConnExists=0;
    //for (int i=0; i<Info[0].p->m_nIOs; i++)
    //  if ((Info[0].p->Nd_Rmt(i)==Info[1].p) &&
    //      (Info[0].p->IOId_Self(i)==Info[0].Id) &&
    //      (Info[0].p->IOId_Rmt(i)==Info[1].Id))
    //    ConnExists=1;
#if dbgKwikConnect
    dbgpln(" .c %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
#endif

    if (!ConnExists && Ret==0)
      {
      CFlange *pF = NULL;
      bool DoInitConn=false;
      if (dwClass0&nc_MLnk)
        {
        for (int i=0; !pF && i<2; i++)
          pF=Info[i].p->GetFlange(Info[i].Id);
        if (!pF)
          {
          //pF=new CFlange(true);
          pF= (CFlange*)CFlangeClass.Construct("Flange" ,"" ,NULL , TOA_Free);// new CFlange(true);
          pF->m_bMustDelete=true;
          DoInitConn=true;
          }
        }
      CTerminal *pT = NULL;
      if (dwClass0&nc_CLnk)
        pT=(CCtrlTermX*)CElecTermXClass.Construct("CtrlTermX" ,"TermTag" ,NULL , TOA_Free);// 
      else if (dwClass0&nc_ELnk)
        {
//ELEC         CETermStripDefn* pTerm0=Info[0].p->IODesc_SelfId(Info[0].Id)->m_pTerms;
//ELEC         CETermStripDefn* pTerm1=Info[1].p->IODesc_SelfId(Info[1].Id)->m_pTerms;
//ELEC         if (pTerm0 && pTerm1)
//ELEC           {
//ELEC           LogError(FullObjTag(), 0, "Too many Wire Definitions %s.%s", Info[0].p->FullObjTag(), IODesc_SelfId(Info[0].Id)->IOName());
//ELEC           Ret=NErr_Incompatibleconnections;
//ELEC           }
//ELEC         else if (pTerm0)
//ELEC           pT=new CElecTermX(*pTerm0);
//ELEC         else if (pTerm1)
//ELEC           pT=new CElecTermX(*pTerm1);
//ELEC         else
        pT=(CElecTermX*)CElecTermXClass.Construct("ETerminalX" ,"ETermTag" ,NULL , TOA_Free);// 
        }
      else if (dwClass0&nc_ALnk)
        pT=(CAirTermX*)CAirTermXClass.Construct("AirTermX" ,"ATermTag" ,NULL , TOA_Free);// 
#if dbgKwikConnect
      dbgpln(" .d %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
#endif
      Ret=Connect(Info[0], Info[1], pF, pT);

      if (pF && DoInitConn)
        {
        pF->InitConnId(Info[0].p, Info[0].p->IODesc_SelfId(Info[0].Id)->IOName(0), 
          Info[1].p, Info[1].p->IODesc_SelfId(Info[1].Id)->IOName(0), NULL);
        //pF->m_pRefNd      = (Info[0].p->m_dwType & nc_MLnk) ? Info[1].p : Info[0].p;
        if (ConnectTag)
          pF->FixTag(ConnectTag, true);
        else
          pF->FixTag();
        }

#if dbgKwikConnect
      dbgpln(" .e %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
#endif
      }
    else
      {
      LogError(s->FullTag(), 0, "Already Connected %s.%s --> %s.%s", s->FullTag(), IOTag1, d->FullTag(), IOTag2);
      Ret=NErr_AlreadyConnected;
      }
    }
  else
    {
    LogError(s->FullTag(), 0, "NOT Connected %s.%s --> %s.%s", s->FullTag(), IOTag1, d->FullTag(), IOTag2);
    Ret = NErr_NotFound;
    }
  #if dbgConnect
  dbgnln;
  dbgpln("Connect     : %s.%s -> %s.%s [%i]",s->Tag,IOTag1,d->Tag,IOTag2,Ret);
  #endif
  return Ret;
  };

//--------------------------------------------------------------------------

int FlwNode::Connect(IOInfo &Info1, IOInfo &Info2, CFlange* pF, CTerminal * pT, int RqdIONo1, int RqdIONo2)
  {
  CStopWatch SW;
  SW.Start();

  if (Info1.p==Info2.p)
    {
    LogError(Info1.p->FullObjTag(), 0, "Self Connect - Not Allowed");
    return NErr_SelfConnect;
    }


  Info1.IOIndex = Info1.p->Attach(Info1.AreaInx, Info2.p, Info2.AreaInx, pF, pT, RqdIONo1, 1);
#if dbgKwikConnect
  dbgpln(" .X %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
#endif
  Info2.IOIndex = Info2.p->Attach(Info2.AreaInx, Info1.p, Info1.AreaInx, pF, pT, RqdIONo2, -1);
#if dbgKwikConnect
  dbgpln(" .Y %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
#endif
  Info1.p->m_IOs[Info1.IOIndex].m_iIONoRmt = Info2.IOIndex;
  Info2.p->m_IOs[Info2.IOIndex].m_iIONoRmt = Info1.IOIndex;

  IOAreaRec & Area1 = Info1.p->IOAreas[Info1.AreaInx];
  IOAreaRec & Area2 = Info2.p->IOAreas[Info2.AreaInx];

  bool Do1to2=true;
  if (((Area1.m_Dirn & LIO_InOut) == LIO_InOut) && ((Area2.m_Dirn & LIO_InOut) == LIO_InOut))
    {
    // old
    Do1to2=(Info1.IOIndex>Info2.IOIndex);
    }
  else
    {
    if (Area2.m_Dirn & LIO_In)
      Do1to2=true;
    else if (Area1.m_Dirn & LIO_In)
      Do1to2=false;
    else if (Area2.m_Dirn & LIO_Out)
      Do1to2=false;
    else if (Area1.m_Dirn & LIO_Out)
      Do1to2=true;
    else if (Area2.m_Dirn & LIO_In)
      Do1to2=true;
    else
      Do1to2=false;
    }

#if dbgKwikConnect
  dbgpln(" .M %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
#endif

  if (pF)
    {
    if (Do1to2)
      pF->SetConnection(Info1, Info2);
    else
      pF->SetConnection(Info2, Info1);
    }
#if dbgKwikConnect
  dbgpln(" .N %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
#endif

  if (pT)
    {
    if (Do1to2)
      pT->SetConnection(Info1, Info2);
    else
      pT->SetConnection(Info2, Info1);
    }

#if dbgKwikConnect
  dbgpln(" .O %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
#endif

  //  if (Info1.IOIndex>Info2.IOIndex)
//    pC->SetConnection(Info1.p, Info1.IOIndex, Info2.p, Info2.IOIndex);
//  else
//    pC->SetConnection(Info2.p, Info2.IOIndex, Info1.p, Info1.IOIndex);

  for (int Pass=0; Pass<2; Pass++)
    {
    FlwNode *p=Pass==0 ? Info1.p : Info2.p;
    for (int i=0 ; i<p->m_nIOs; i++)
      {
      CIORec & io=p->m_IOs[i];
      if (io.Rmt)
        {
        if (!io.fIsComplete)
          {
          p->PostConnect(i);
          p->ConnectionsChanged();
          io.fIsComplete=True;
          }
        }
      else
        break;
      }
    }

  StructureChanged(NULL);
#if dbgKwikConnect
  dbgpln(" .Z %5s %5s %10.1fus %-20s %s > %s",
    "", "", SW.Lap()*1e6, "", Info1.p->FullObjTag(), Info2.p->FullObjTag());
#endif

  #ifdef xdbgConnect
  dbgnln;
  dbgpln("Connect     : %s -> %s using %s [%i]",Tag1,Tag2,ConnTag,Ret);
  #endif
  return(0);
  };

//--------------------------------------------------------------------------

CFlange* FlwNode::DisConnect(int IONo)
  {
  CFlange* pF = IOFlange(IONo);
  if (pF)
    pF->ClrConnection();

  CTerminal *pT=Ctrl_Terminal(IONo);
  if (pT)
    pT->ClrConnection();

  FlwNode* Rmt = Nd_Rmt(IONo);
  if (Rmt)
    {
    int RmtIONo=IOIONo_Rmt(IONo);
    Rmt->PreDisConnect(RmtIONo);
    PreDisConnect(IONo);
    Rmt->Detach(RmtIONo);
    Detach(IONo);

    Rmt->ConnectionsChanged();
    ConnectionsChanged();
    }
  else
    {
    PreDisConnect(IONo);
    Detach(IONo);
    ConnectionsChanged();
    }

  return pF;
  };

//--------------------------------------------------------------------------

int FlwNode::DisConnectTag(FlwNode * Root, pchar Tag1, pchar Tag2)
  {
  int          Ret=0;
  IOInfo       Info[2];

  if (Root==NULL)
    Root=this;

  if (Root->FindIOTag(Tag1, OId_NodeIO, Info[0]) & Root->FindIOTag(Tag2, OId_NodeIO, Info[1]))
    {
    flag ConnExists=0;
    for (int iNo=0; iNo<Info[0].p->m_nIOs; iNo++)
      if ((Info[0].p->Nd_Rmt(iNo)==Info[1].p) &&
          (Info[0].p->IOId_Rmt(iNo)==Info[1].Id))
        {
        ConnExists=1;
        break;
        }

    if (ConnExists)
      {
      Info[0].p->DisConnect(iNo);
      }
    else
      {
      LogError(Tag1, 0, "Not Already Connected: %s > %s", Tag1, Tag2);
      Ret = NErr_NotFound;
      }
    }
  else
    {
    LogError(Tag1, 0, "Connection Not Found: %s > %s", Tag1, Tag2);
    Ret = NErr_NotFound;
    }
  return Ret;
  }

//--------------------------------------------------------------------------

void FlwNode::DisConnectAll()
  {
  while (NoIOs())
    DisConnect(0);
  }

//--------------------------------------------------------------------------

bool FlwNode::PostConnectDirect(int IONo)
  {
  int IOId=IOId_Self(IONo);
  for (int d=0; d<m_AttachedDirectIOs.GetSize(); d++)
    {
    CDirectFlwIO &D=*m_AttachedDirectIOs[d];
    if (D.m_iLclDIOId==IOId)
      {
      D.PostConnect(IONo);
      FlwNode::PostConnect(IONo);
      return true;
      }
    }
  return false;
  }

//--------------------------------------------------------------------------

void FlwNode::PostConnect(int IONo)
  {
  for (int fe=0; fe<NIOFBs(IONo); fe++)
    IOFB(IONo,fe)->SetCd(IOConduit(IONo));
  };

//--------------------------------------------------------------------------

bool FlwNode::PreDisConnectDirect(int IONo)
  {
  int IOId=IOId_Self(IONo);
  for (int d=0; d<m_AttachedDirectIOs.GetSize(); d++)
    {
    CDirectFlwIO &D=*m_AttachedDirectIOs[d];
    if (D.m_iLclDIOId==IOId)
      {
      D.PreDisConnect(IONo);
      FlwNode::PreDisConnect(IONo);
      return true;
      }
    }
  return false;
  };

//--------------------------------------------------------------------------

void FlwNode::PreDisConnect(int IONo)
  {
  };

//--------------------------------------------------------------------------

void FlwNode::ConnectionsChanged()
  {
  };

//--------------------------------------------------------------------------

CPropagateNetInfoCtrl::CPropagateNetInfoCtrl(eScdPropagateNetTasks Task, long Info)
  {
  m_Task        = Task;
  m_Pass        = 0;
  m_Info        = Info;
  m_ReqdMethod  = 0;//LFM_Full;
  m_pObj        = NULL;
#if dbgFlwNode
  if (dbgPropagateFlwModes())
    {
    dbgpln("PropagateNetInfo");
    dbgindent(2);
    }
#endif

  }
CPropagateNetInfoCtrl::~CPropagateNetInfoCtrl()
  {
#if dbgFlwNode
  if (dbgPropagateFlwModes())
    dbgindent(-2);
#endif
  }

bool FlwNode::PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo)
  {
  return FlwNode::DoPropagateNetInfo(Ctrl, IONo, false);
  }

bool FlwNode::DoPropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo, bool HasMethod)
  {
  if (m_FlowModePass>Ctrl.m_Pass)
    return false;

  m_FlowModePass++;

  if (HasMethod)
    {
    switch (Ctrl.m_Task)
      {
      case eScdGetFlowMode :
        {
        if (m_FlowModeRqd>0)// && (m_FlowModeRqd&~LFM_Propagate)<(Ctrl.m_ReqdMethod&~LFM_Propagate))
          {
          long ModeReqd     = m_FlowModeRqd&LFM_All;
          bool DoPropagate  = (m_FlowModeRqd&LFM_Propagate)!=0;
          if (ModeReqd==LFM_Xfer)
            DoPropagate=true;

          if (DoPropagate && (Ctrl.m_ReqdMethod==0 || ModeReqd<Ctrl.m_ReqdMethod))
            {
            Ctrl.m_ReqdMethod=/*ModeReqd;//*/m_FlowModeRqd&LFM_All;
            //if ((m_FlowModeRqd&~LFM_Propagate)==LFM_Xfer)
            Ctrl.m_pObj=this;
            }
          }
#if dbgFlwNode
        if (dbgPropagateFlwModes())
          {
          dbgpln("   %i Reqd:%s   Ctrl:%s   this:%-11s  %s",
            m_FlowModePass, dbgFlowModeStr(m_FlowModeRqd), dbgFlowModeStr(Ctrl.m_ReqdMethod),
            Ctrl.m_pObj==this?"--->":"", FullObjTag());
          }
#endif
        break;
        }
      case eScdSetFlowMode :
        if (Ctrl.m_ReqdMethod>0) // (Ctrl.m_ReqdMethod&LFM_Propagate)!=0)
          {
          long ModeReqd   = Ctrl.m_ReqdMethod&LFM_All;
          bool DoPropagate  = true;//(Ctrl.m_ReqdMethod&LFM_Propagate)!=0;
          long FM=FlowMode();
          if (FM!=ModeReqd)    
            {
            switch (ModeReqd)
              {
              case LFM_Xfer:
                FM=LFM_Xfer;
                break;
              default: // >= LFM_Simple
                if (FM==0 || FM==LFM_Xfer || DoPropagate)
                  FM=ModeReqd;
                break;
              }
            }

          TaggedObject * pObj=1/*(FM==LFM_Xfer)*/ ? Ctrl.m_pObj:NULL;
          if (Ctrl.m_pObj==this)
            {
#if dbgFlwNode
            if (dbgPropagateFlwModes())
              {
              dbgpln("** %i Reqd:%s   Ctrl:%s   Mode:%s   %-20s @ %s",
                m_FlowModePass, dbgFlowModeStr(m_FlowModeRqd), dbgFlowModeStr(Ctrl.m_ReqdMethod), dbgFlowModeStr(FlowMode()), FullObjTag(),
                pObj?pObj->FullObjTag():"???");
              }
#endif
            }
          else if ((m_FlowModeRqd&LFM_Propagate)==0)   
            {
            SetFlowModeRqd(FM, LFM_Propagated, pObj);
            //if (DoPropagate)
            //  m_FlowModeRqd = FM;
            //else
            //  m_FlowModeRqd &= ~LFM_Propagate;
#if dbgFlwNode
            if (dbgPropagateFlwModes())
              {
              dbgpln("   %i Reqd:%s   Ctrl:%s   Mode:%s   %-20s @ %s",
                m_FlowModePass, dbgFlowModeStr(m_FlowModeRqd), dbgFlowModeStr(Ctrl.m_ReqdMethod), dbgFlowModeStr(FlowMode()), FullObjTag(),
                pObj?pObj->FullObjTag():"???");
              }
#endif
            }
          else
            {
            return false;
            }
          }
        else if (m_FlowModeRqd>0)
          {
//          //SetFlowModeRqd(m_FlowModeRqd, NULL);
//          m_FlowModeRqd &= LFM_All;
//#if dbgFlwNode
//          if (dbgPropagateFlwModes())
//            {
//            dbgpln("%i Reqd:%s                   Mode:%s B   %s",
//              m_FlowModePass, dbgFlowModeStr(m_FlowModeRqd), dbgFlowModeStr(FlowMode()), FullObjTag());
//            }
//#endif
          }
        else
          {
//          SetFlowModeRqd(FlowMode(), NULL);
//          m_FlowModeRqd &= ~LFM_Propagate;
//#if dbgFlwNode
//          if (dbgPropagateFlwModes())
//            {
//            dbgpln("%i Reqd:%s                   Mode:%s C   %s",
//              m_FlowModePass, dbgFlowModeStr(m_FlowModeRqd), dbgFlowModeStr(FlowMode()), FullObjTag());
//            }
//#endif
          }
        break;
      }
    }

  return true;
  };

//--------------------------------------------------------------------------

void FlwNode::OnElecComponentChange(eElecChanges EC, CECompData *pC)
  {
  //gs_pFlwLib->FE_ElecComponentsChanged();
  };

//--------------------------------------------------------------------------

void FlwNode::LockCtrls()
  {
  //dbgfileflush(true);

  ConnectCodeLock.Lock(); // Prevent possible deadlock;
  for (int i=Ctrl1();i<CtrlN(); i++)
    {
    //long xxx=dbglockcnt++;
    //dbgpln("  LockTerm     %#10x %4i %-30s %s", m_IOs[i].m_pTerm, i, Tag(), m_IOs[i].m_pTerm->FullTag());
    m_IOs[i].m_pTerm->Lock();
    //dbgpln("  Done         %#10x", m_IOs[i].m_pTerm);
    }
  for (i=0;i<NoXRefs2Me(); i++)
    {
    m_XRefs2Me[i]->Lock();
    }
  BumpNodeXRefsLocked(1);
  ConnectCodeLock.UnLock();
  //dbgfileflush(false);
  };
void FlwNode::UnLockCtrls()
  {
  //dbgfileflush(true);
  //ConnectCodeLock.Lock(); // Prevent possible deadlock;
  BumpNodeXRefsLocked(-1);
  for (int i=Ctrl1();i<CtrlN(); i++)
    {
    //dbgpln("  UnLockTerm   %#10x %4i %-30s %s",
    //  m_IOs[i].m_pTerm, i, Tag(), m_IOs[i].m_pTerm->FullTag());
    m_IOs[i].m_pTerm->UnLock();
    //dbgpln("  Done         %#10x", m_IOs[i].m_pTerm);
    }
  for (i=0;i<NoXRefs2Me(); i++)
    {
    //if (m_XRefs2Me[i]->InTOSequence())
    m_XRefs2Me[i]->UnLock();
    }
  //ConnectCodeLock.UnLock();
  //dbgfileflush(false);
  };

//--------------------------------------------------------------------------

pchar StrCopySym(pchar Dst, pchar p, int l)
  {
  pchar e;

  while (* p == ' ') p++;
  e = strstr(p, " ");
  if (e == NULL)
    e = strstr(p, "]");
  if (e == NULL)
    e = p + strlen(p);
  l = Min(int (p - e), l);
  strncpy(Dst, p, l);
  Dst[l] = NULL;
  Dst += l;
  * Dst = NULL;
  while (* e == ' ') e++;
  return (e);
  };

//--------------------------------------------------------------------------

void FlwNode::Set_ProdFrac(int JoinNo, int i, double Frac)
  {
  if (Valid(Frac))
    {
    int js=IO_Self(i).iJoinId;
    int cs=IO_Self(i).iJoinConnNo;
    CJoinRec &J = Joins[js];

    if ((NetNo==Nd_Rmt(i)->NetNo) &&
        (J.m_pSubNet==J.Join_Rmt(cs)->m_pSubNet))
      {
      int pf=J.nProdFracs;
      if (pf>=J.ProdFracs.GetSize())
        {
        J.ProdFracs.SetSize(pf+1);
        J.fMatrixChanged=1;
        }
      else
        {
        JProdFracRec & PF=J.ProdFracs[pf];
        if (PF.iIONo!=i || PF.iConnNo!=cs)
          J.fMatrixChanged=1;
        else if (Valid(PF.dProdFrac)!=Valid(Frac))
          J.fMatrixChanged=1;
        else if (fabs(PF.dProdFrac-Frac)>J.dProdFracTol)
          J.fProdFracsChgd=1;
        }

      JProdFracRec & PF=J.ProdFracs[pf];
      PF.iIONo=i;
      PF.iConnNo=cs;
      //PF.dProdFrac=Frac;
      if (Valid(PF.dProdFrac))
        PF.dProdFrac=PF.dProdFrac*0.2+Frac*0.8; // a bit of filtering
      else
        PF.dProdFrac=Frac;
      if (J.fMatrixChanged)
        PF.fRestrict=0;
      J.nProdFracs++;
      }
    }
  };

//--------------------------------------------------------------------------

void FlwNode::SetQRatio(int iSolnIndex, FlwBlk *pSlaveB, double Ratio, FlwBlk *pMasterB)
  {
  DoBreak();
//TODO Code to go back
#ifdef NOTDEFINED
  if (fabs(Ratio)< 1.0e-3)
    Ratio=Sign(Ratio)*1.0e-3;
  else if (fabs(Ratio)> 1.0e3)
    Ratio=Sign(Ratio)*1.0e3;
  if (pSlaveB==NULL || pMasterB==NULL)
    {
    pMasterB=NULL;
    LogError(FullObjTag(), 0, "QRatio : IO Does Not Exist");
    }
  else if (pSlaveB->iNet[iSolnIndex]!=pMasterB->iNet[iSolnIndex] || pSlaveB->iSubNet[iSolnIndex]!=pMasterB->iSubNet[iSolnIndex])  // different subnets become 1 becomes fixed flw (to be done!)
    {
    pMasterB=NULL;
    LogError(FullObjTag(), 0, "QRatio : Case not catered for YET!");
    }

  if (!Valid(Ratio) || pMasterB==NULL)
    {
    pSlaveB->QmRatio.MFb = NULL;
    pSlaveB->QmRatio.Mult = dNAN;
    }
  else
    {
    if (pSlaveB->QmRatio.MFb!=pMasterB)
      {
      pSlaveB->SetTopologyChanged();
      DBGTOPOCHG("SlaveQmRatio")
      }
//      *(pSlaveB->/*pGFb->pNet->*/pTopologyChg)=1;
    pSlaveB->QmRatio.MFb=pMasterB;
    pSlaveB->QmRatio.Mult = Ratio;
    #if dbgFlwNode
    if (dbgQRatio() || dbgTopologyChg())
      dbgpln("Set QRatio %g",Ratio);
    #endif
    }
#endif
  }

//--------------------------------------------------------------------------

static LPCTSTR dbgNodeFlwTask[] =
  {
    "PBInit",
    "PBQueryRemove",
    "PBRemove",
    "PBStep",
    "PBTerm",
    "PBQueryReInit",
    "PBReInit",
    "DynInit",
    "DynTerm",
  };

long FlwNode::NodeFlwTask(NodeFlwTasks Task)
  {
  //int i;
  //dbgpln("Task %s %s", dbgNodeFlwTask[Task], m_sTag());
  switch (Task)
    {
    case NFT_PBQueryReInit:
      if (IsLnk() && GetQmEst2IO(NULL)!=LnkFlowDefined())
        return 1;
      return 0;
    case NFT_PBInit:
      if (IsLnk())
        {
        SetQm2IO(0.0, SQ2IO_NonLink);
        }
      //dbgpln("PBInit   %s %s", IsLnk()?"IsLnk":"", m_sTag());
    case NFT_PBReInit:
      if (fDoDbgBrk)
        { int xxx=0;}
      if (IsLnk())
        {
        SetQmEst2IO(LnkFlowDefined(), SQ2IO_NonLink);
        //SetQm2IO(IOFB(0,0)->GetQmMemPB(), SQ2IO_NonLink);
        }
      else if (m_dwType & nc_Audit)
        {
        }

      for (int d=0; d<NoEnabledDirectIOs(); d++) 
        {
        CDirectFlwIO &D=*m_EnabledDirectIOs[d];
        if (D.HasConduit && D.m_iMyConnectedIO>=0)
          {
          int IOId=D.LclDIOId();
          switch (IOId)
            {
            case IOId_Spill2Area:
            case IOId_Vent2Area:
              SetIOQmEst_Out(D.m_iMyConnectedIO, PBQmEst);
              break;
            case IOId_LeakI2Area:
            case IOId_LeakO2Area:
              break;
            default:
              if (IOId>=IOId_Makeup2Area && IOId<IOId_Makeup2Area+MaxNdMakeups)
                SetIOQmEst_In(D.m_iMyConnectedIO, PBQmEst);
              else if (IOId>=IOId_Bleed2Area && IOId<IOId_Bleed2Area+MaxNdBleeds)
                SetIOQmEst_Out(D.m_iMyConnectedIO, PBQmEst);
              break;
            }
          }
        }

      //dbgpln("PBReInit %s %s", IsLnk()?"IsLnk":"", m_sTag());
      return 0;
    case NFT_PBQueryRemove:
      if (m_dwType & (nc_Process|nc_MSrcSnk))
        {
        long nIn=0;
        for (int i=0; i<NoFlwIOs(); i++)
          {
          //dbgpln("  %3i %20s %20s", i, DbgFltString(IOQmEst_In(i)), DbgFltString(IOQm_In(i)));
          if (IO_In(i))
            nIn++;
          }
        long RemOK=(nIn>0) ? 0 : 1;
        //dbgpln("PBQueryRemove Proc|SrcSnk  %3s i:%3i       %s", RemOK?"YES":"", nIn, m_sTag());
        return RemOK && ((m_dwType&nc_Audit)==0);
        }
      else if (m_dwType & (nc_MLnk|nc_MBst|nc_MXfer))
        {
        long nIn=0;
        long nOut=0;
        for (int i=0; i<NoFlwIOs(); i++)
          {
          //dbgpln("  %3i %20s %20s", i, DbgFltString(IOQmEst_In(i)), DbgFltString(IOQm_In(i)));
          if (IO_In(i))
            nIn++;
          else if (IO_Out(i))
            nOut++;
          }
        long RemOK=(nIn==0 || nOut==0) ? 1 : 0;
        //dbgpln("PBQueryRemove Lnk|Bst|Xfer %3s i:%3i o:%3i %s", RemOK?"YES":"", nIn, nOut, m_sTag());
        return RemOK && ((m_dwType&nc_Audit)==0);
        }
      else
        {
        //dbgpln("PBQueryRemove Other        %3s i:%3i o:%3i %s", "", 0, 0, m_sTag());
        return 0;
        }
      break;
    case NFT_PBRemove:
      SetState(MSA_Empty);
      if (IsLnk())
        {
        SetQmEst2IO(0.0);
        SetQm2IO(0.0);
        }
      else
        {
        for (int i=0; i<NoFlwIOs(); i++)
          {
          Nd_Rmt(i)->SetQmEst2IO(0.0);
          }
        }
      return 0;
    case NFT_PBStep:
      for (int i=0; i<NoFlwIOs(); i++)
        for (int fe=0; fe<NIOFBs(i); fe++)
          IOFB(i,fe)->SetQmPrvPB(IOFB(i,fe)->GetQm());

      if (IsLnk())
        {
        SetQmEst2IO(LnkFlowDefined());
        SetQm2IO(LnkFlowDefined());
        }

      return 0;

    case NFT_PBTerm:
      //for (i=0; i<NoFlwIOs(); i++)
      //  for (int fe=0; fe<NIOFBs(i); fe++)
      //    IOFB(i,fe)->SetQmMemPB(IOFB(i,fe)->GetQm());
      return 0;

    case NFT_DynInit:
      return 0;
    case NFT_DynTerm:
      return 0;
    }
  return 0;
  };

//--------------------------------------------------------------------------

FlwNode * FlwNode::CreateFlwNode(pchar Class, pchar SubClass, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach)
  {
  pTagObjClass pC=TagObjClass::FindClassId(Class);
  if (!pC)
    {
    //Find "A Version"
    pC=TagObjClass::FindClassId(Class, False);
    if (pC)
      LogNote(TagIn, 0, "Version Changed %s to %s", Class, pC->ClassId());
    }

  if (pC)
    {
    FlwNode * w = (FlwNode *)pC->Construct(SubClass, TagIn, pAttach, eAttach);

    return (w);
    }
  else
    return NULL;
  };

//--------------------------------------------------------------------------

pchar FlwNode::FindFlwNodeIdIndexed(pchar pGroup, int iIndex)
  {
  pTagObjClass p = TagObjClass::FindClassIndexed(pGroup, iIndex);
  return p ? p->ClassId() : NULL;
  };

//--------------------------------------------------------------------------

pchar FlwNode::FindFlwNodeDescIndexed(pchar pGroup, int iIndex)
  {
  pTagObjClass p = TagObjClass::FindClassIndexed(pGroup, iIndex);
  return p ? p->Desc() : NULL;
  };

//--------------------------------------------------------------------------

pchar FlwNode::FindFlwNodeShortDescIndexed(pchar pGroup, int iIndex)
  {
  pTagObjClass p = TagObjClass::FindClassIndexed(pGroup, iIndex);
  return p ? p->ShortDesc() : NULL;
  };

//--------------------------------------------------------------------------

void FlwNode::SetDatumsDone()
  {
  double Z;
  for (int i=0; i<NoProcLnkIOs(); i++)
    if (IOFlange(i))
      {
      if (Nd_Rmt(i) && (IOPipeEntry_Rmt(i) || IOPipeJoin_Rmt(i)))
        {
        Z=IODatum_Flng(i);
        for (int j=0; j<NIOFBs_Rmt(i); j++)
          {
          IOFB_Rmt(i, j)->SetDatumFlng(Z);
          Z+=IOFB_Rmt(i, j)->Rise();
          IOFB_Rmt(i, j)->SetDatumNode(Z);
          }
        }
      Z=IODatum_Flng(i);
      for (int j=0; j<NIOFBs(i); j++)
        {
        IOFB(i, j)->SetDatumFlng(Z);
        Z+=IOFB(i, j)->Rise();
        IOFB(i, j)->SetDatumNode(Z);
        }
      }
  }

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

double CRqdPressCtrl::m_dLowQmFracNC=0.001f;

void CRqdPressCtrl::BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pObj, flag ForProbal, long PBAllowed, long DynAllowed)
  {
  long Allowed = ForProbal ? PBAllowed : DynAllowed;
  if (Allowed)
    {

    DDBValueLst DDBPBP[]={
      {PC_Null     , "Null",                                                       MDD_Hidden},
      {PC_Atmos    , "Atmospheric",  (PC_Atmos & Allowed)                  ? 0 : MDD_Hidden},
      {PC_MinFd    , "LowestFeed",   (PC_MinFd & Allowed)                  ? 0 : MDD_Hidden},
      {PC_MaxFd    , "HighestFeed",  (PC_MaxFd & Allowed)                  ? 0 : MDD_Hidden},
      {PC_Reqd     , "Required",     /*(PC_Reqd & Allowed)                   ? 0 :*/ MDD_Hidden},
      {PC_Reqd     , "RequiredP",    (PC_Reqd & Allowed)                   ? 0 : MDD_Hidden},
      {PC_SatP     , "Saturated",    (PC_SatP & Allowed)    && m_fMdlAvail ? 0 : MDD_Hidden},
      {PC_ReqdSatT , "RequiredT",    (PC_ReqdSatT &Allowed) && m_fMdlAvail ? 0 : MDD_Hidden},
      //{PC_BubblePt , "BubblePt",      ((PC_SatP &Allowed) && m_fMdlAvail) ? 0 : MDD_Hidden},
      //{PC_DewPt    , "DewPt",         ((PC_SatP &Allowed) && m_fMdlAvail) ? 0 : MDD_Hidden},
      //{PC_SubCooled, "SubCooled_Liq", ((PC_SatP &Allowed) && m_fMdlAvail) ? 0 : MDD_Hidden},
      //{PC_SuperHt  , "SuperHeat_Vap", ((PC_SatP &Allowed) && m_fMdlAvail) ? 0 : MDD_Hidden},
      {0}};

    //const DDEF_Flags flgs = SM_Direct|HM_All;
    const DDEF_Flags flgs = SM_Direct|SM_Inline|HM_All;

    LPTSTR Tag=ForProbal ? "PBPress":"OperatingP";
    if(DDB.BeginStruct(pObj, Tag))
      {
      DDB.Visibility(flgs);
      DDB.Long ("Method",    "", DC_,  "",    &m_iMethod, pObj, isParm|SetOnChange, DDBPBP);

      DDB.Visibility(flgs, (m_iMethod & (PC_MinFd | PC_MaxFd))!=0);
      DDB.CheckBoxBtn("IgnoreLowQm",   "", DC_,  "",    &m_fIgnoreLoQm, pObj, isParm|SetOnChange, DDBOnOff);
      DDB.Visibility(flgs, (m_iMethod & (PC_MinFd | PC_MaxFd))!=0 && m_fIgnoreLoQm);
      DDB.Double("LowQmFrac", "", DC_Frac, "%",&m_dLowQmFrac, pObj, isParm|SetOnChange); 

      DDB.Visibility(flgs, m_iMethod==PC_Reqd);
      DDB.Double("Reqd",      "", DC_P, "kPa", &m_dPRqd,   pObj, m_iMethod==PC_Reqd?isParm:0);
      DDB.Visibility(flgs, m_iMethod==PC_ReqdSatT);
      DDB.Double("ReqdT",     "", DC_T, "C",   &m_dTRqd,   pObj, isParm);
      DDB.Visibility(flgs);
      DDB.Double("Result",    "", DC_P, "kPa", &m_dP2Set,  pObj, isResult);
      DDB.Text(" ");
      DDB.Visibility();
      }
    DDB.EndStruct();
    }
  }

//--------------------------------------------------------------------------

void CRqdPressCtrl::CalculateResult()
  {
  if (m_fMdlAvail!=(m_pMdl!=NULL))
    DoBreak(); // these must match

  m_fDoInputs=false;
  switch (m_iMethod)
    {
    case PC_Atmos:
      m_dP2Set=AtmosPress();
      m_fDoInputs=true;
      break;
    case PC_Reqd:
      m_dP2Set=m_dPRqd;
      m_fDoInputs=true;
      break;
    case PC_SatP:
      {
      SpModel &Mdl=*m_pMdl;//->pModel;
      if (Mdl.Mass()>SmallPosFlow)
        m_dP2Set=Mdl.SaturationP(Mdl.Temp());
      //else
        //m_dP2Set=Mdl.SaturationP(Mdl.Temp());
      m_fDoInputs=true;
      }
      break;
    case PC_ReqdSatT:
      {
      SpModel &Mdl=*m_pMdl;//->pModel;
      if (Mdl.SaturationMethod()==SMFM_PartialP)
        {
        m_dP2Set=Mdl.SaturationP(m_dTRqd);
        }
      else
        {
        m_dP2Set=Mdl.SaturationTotalP(m_dTRqd);
        };
      m_fDoInputs=true;
      }
    case PC_BubblePt:
      //m_fP2Set=;
      m_fDoInputs=true;
      break;
    case PC_DewPt:
      //m_fP2Set=;
      m_fDoInputs=true;
      break;
    case PC_SubCooled:
      //m_fP2Set=;
      m_fDoInputs=true;
      break;
    case PC_SuperHt:
      //m_fP2Set=;
      m_fDoInputs=true;
      break;
    }
  }

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

flag FlwNode::HasModeNear(int iJoinNo, long Mode)    
  { 
  if (FlowMode() & Mode)
    return true;
  if (iJoinNo<0)
    {
    for (int i=0; i<NoProcessIOs(); i++)
      {
      if (Nd_Rmt(i)->FlowMode() & Mode)
        return true;
      }
    }
  else
    {
    for (int i=0; i<NoJoins(); i++)
      {
      if (i==iJoinNo)
        {
        CJoinRec & J=Joins[i];
        for (int c=0; c<J.NConns(); c++)
          {
          if (J.Nd_Rmt(c)->FlowMode() & Mode)
            return true;
          }
        }
      }
    }
  return false; 
  };

//--------------------------------------------------------------------------

double FlwNode::MeasureJoinPressure(int iJoinNo, CRqdPressCtrl *pPC/*=NULL*/)
  {
  CJoinRec & J=Joins[iJoinNo];

  double Pm=Std_P; // Pressure = Std_P if no inputs

  double QmIn=0.0;
  for (int c=0; c<J.NConns(); c++)
    {
    int i = J.IONo(c);
    if (i<NoFlwIOs() && IO_In(i))
      QmIn+=IOConduit(i)->QMass();
    }

  const double LoQm=QmIn*(pPC ? (pPC->IgnoreLoQm() ? pPC->LowQmFrac():0.0):pPC->LowQmFracNC());

  int n=0;
  for (c=0; c<J.NConns(); c++)
    {
    int i = J.IONo(c);
    if (i<NoFlwIOs() && IO_In(i))
      {
      CFlange* pF=IOFlange(i);
      double P=IOP_Flng(i);
      Set_IOP_Self(i, P);

      if (IOConduit(i)->QMass()>=LoQm)
        {
        if (n++)
          {
          if (pPC)
            {
            switch (pPC->Method())
              {
              case PC_MinFd: Pm=Min(Pm, P); break;
              case PC_MaxFd: Pm=Max(Pm, P); break;
              }
            }
          else
            Pm=Min(Pm, P);
          }
        else
          Pm=P;
        }
      }
    }
  if (pPC)
    {
    pPC->SetPMeas(Pm);
    pPC->SetP2Set(Pm, true, true);
    }
  return Pm;
  }

//--------------------------------------------------------------------------

void FlwNode::SetJoinPressure(int iJoinNo, CRqdPressCtrl *pPC/*=NULL*/)
  {
  SetJoinPressure(iJoinNo, pPC->P2Set(), pPC->DoInputs(), pPC->DoOutputs());
  }

//--------------------------------------------------------------------------

void FlwNode::SetJoinPressure(int iJoinNo, double P, bool DoInputs, bool DoOutputs)
  {
  CJoinRec & J=Joins[iJoinNo];
  for (int c=0; c<J.NConns(); c++)
    {
    int i = J.IONo(c);
    if (i<NoFlwIOs() && (DoInputs && IO_In(i) || DoOutputs && IO_Out(i)))
      {
      Set_IOP_Self(i, P);
      Set_IOP_Flng(i, P);
      Set_IOP_Est_Self(i, P);
      Set_IOP_Est_Flng(i, P);
      }
    }
  Joins[iJoinNo].m_PB.P=P;
  Joins[iJoinNo].m_PB.PEst=P;
  }

//--------------------------------------------------------------------------

void FlwNode::EvalJoinPressure(int iJoin, CRqdPressCtrl *pPC, SpModel/*Owner*/ * pMdl)
  {
  if (iJoin<NoJoins())
    {
    if (pPC)
      {
      pPC->SetMdl(pMdl);
      MeasureJoinPressure(iJoin, pPC);
      pPC->CalculateResult();
      SetJoinPressure(iJoin, pPC);
      }
    else
      {
      double P=MeasureJoinPressure(iJoin);
      SetJoinPressure(iJoin, P);
      }
    }
  }

//--------------------------------------------------------------------------

void FlwNode::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NoJoins(); j++)
        {
        double Pm=MeasureJoinPressure(j);
        SetJoinPressure(j, Pm);
        //MeasureJoinPressure(j, pPC);
        //SetJoinPressure(j, pPC);
        //#if dbgFlwNode
        //if (dbgEvalJoinPress(sTag()))
        //  dbgpln("EJP: %12.2f  %s[%i]", Pm, sTag(), j );
        //#endif
        }
      }
      break;
    default:;
      {
      for (int j=0; j<NoJoins(); j++)
        for (int c=0; c<Joins[j].NConns(); c++)
          {
          if (Joins[j].IsIO(c))
            {
            //Joins[j].PB.P=IOP_Self(Joins[j].IONo(0));
            Set_IOP_Self(Joins[j].IONo(c), Joins[j].m_PB.P);
            Set_IOP_Est_Self(Joins[j].IONo(c), Joins[j].m_PB.PEst);
            }
          }
      break;
      }
    }
  };

//--------------------------------------------------------------------------

void FlwNode::SetState(eScdMdlStateActs RqdState)
  {
  for (int i=0; i<NoFlwIOs(); i++)
    {
    IOSurgeInfo(i)->Reset();
    IONetProps(i)->Reset();
    for (int j=0; j<NIOFBs(i); j++)
      {
      IOFwdFlwProps(i,j)->Reset();
      IORevFlwProps(i,j)->Reset();
      }
      //      //IOFB(i,j)->ResetData(false);
//      //IOFB(i,j)->SetFill(0);
//      IOFB(i,j)->SetVLFracs(NULL, 0,0);
    IOConduit(i)->SetState(RqdState);
    if (IOConduitIO(i))
      IOConduitIO(i)->SetState(RqdState);
    if (IOConduitIn(i))
      IOConduitIn(i)->SetState(RqdState);
    }
  for (int i=0; i<NoAreaSums(); i++)
    {
    switch (RqdState)
      {
      case MSA_Empty:
      case MSA_PreSet:
      case MSA_ZeroFlows:
      case MSA_EmptySpillTargets:
      //case MSA_SteadyState:
        m_ExtAudit.ZeroFlows();
        AreaSum(i)->ZeroFlows();
      case MSA_DynStatsRunInit:
        m_ExtAudit.ZeroTotals();
        AreaSum(i)->ZeroTotals();
        break;
      }
    }
  }

//--------------------------------------------------------------------------

#define dbgAssocGrfs 0

flag FlwNode::EO_RequestModelAssocGraphics(CMdlAssocGraphicList & Grfs)
  {
  if (NoRealDirectIOs()>0)
    {
    CMdlAssocGraphic G;
    G.m_sNdTag    = FullObjTag();
    G.m_bIsLnk    = IsLnk()!=0;
    Grfs.AddTail(G);
    CMdlAssocGraphic &GT=Grfs.GetTail();

    for (int iDir=0; iDir<NoRealDirectIOs(); iDir++) 
      {
      CDirectFlwIO &D=*m_EnabledDirectIOs[iDir];
#if dbgAssocGrfs 
      dbgp("%2i %2i %s", iDir, D.m_eType, D.Connected?"Conn":"    ");
#endif
      if (D.Connected)
        {
        CConnect *pC  = dynamic_cast<CConnect*>(&D.Flange);

        CMdlAssocGraphic::CItem I;
        switch (D.Type)
          {
          case eDIO_Makeup: I.m_eAGType = eAG_Makeup;  break;
          case eDIO_Bleed:  I.m_eAGType = eAG_Bleed;   break;
          case eDIO_Spill:  I.m_eAGType = eAG_Spill;   break;
          case eDIO_Vent:   I.m_eAGType = eAG_Vent;    break;
          case eDIO_Leak:   I.m_eAGType = eAG_Leak;    break;
          default:
            ASSERT_ALWAYS(false,"Uncatered for DIO Type", __FILE__, __LINE__);
          }
        I.m_sTag = pC->Tag();
        I.m_bShowIt = AssocGraphicOn(I.m_eAGType);

        GT.m_AssocGrfs.Add(I);
#if dbgAssocGrfs 
        dbgp(" %s", pC->FullTag());
#endif
        }
#if dbgAssocGrfs 
      dbgpln("");
#endif
      }

    // etc for RB etc
    }
  return true;
  };

//--------------------------------------------------------------------------

double FlwNode::ContainerMeanPress(IOP_RhoH_Info & Info, double POffset)
  {
  SpContainer & Cn=Info.Cn;
  Info.Hgt  = Cn.ShapeHeight();
  Info.Lvl  = Cn.Level();
  Info.ZTop = m_Datum+Info.Hgt*Info.Lvl;
  if (Cn.Closed())
    {
    if (Valid(Cn.TopPressRqd()))//FlowMode())
      Info.Pm   = Cn.TopPressRqd()+POffset;
    else
      Info.Pm   = Cn.Press()+POffset;
    }
  else
    {
    Cn.SetPress(AtmosPress(Info.ZTop));
    Info.Pm = Cn.Press();
    }
  Info.RhoV = (Cn.Closed() ? Cn.Rho(som_Gas) : AtmosDensity(Info.ZTop));
  Info.RhoSL = Cn.SLEVDensity();
  return Info.Pm;
  };

//--------------------------------------------------------------------------

void FlwNode::Set_IOP_RhoH_Self(int i, double P, IOP_RhoH_Info & Info)
  {
  double SL=GEZ(Info.Lvl - IOAbsFracHgt_Term(i, Info.Cn.ShapeHeight(), Info.Cn.ShapeDatumPt())) * Info.RhoSL;
//CHECK to go back - confuses RHoH Sensitivity
  double V=0.0; // Min(1.0-Info.Lvl, 1.0-Info.IOFracHgt) * Info.RhoV;
  double RhoH=0;
  FlwBlk &FB=*IOFB(i,NIOFBs(i)-1);
  if (FB.FlowMode()==LFM_Linear || FB.FlowMode()==LFM_Full || Info.Cn.UseRhoH())
    RhoH=Gc_Kpa * (SL+V) * Info.Hgt;

  IOPB_Self(i).PEst=P;
  IOPB_Self(i).P=P;
  FB.SetDPzX(-RhoH, 0.0);
  };

//--------------------------------------------------------------------------

void FlwNode::Set_IOP_RhoH_Self(int i, double P, double SLDensity, double Head)
  {
  double RhoH=0;
  FlwBlk &FB=*IOFB(i,NIOFBs(i)-1);
  if (FB.FlowMode()==LFM_Linear || FB.FlowMode()==LFM_Full)
    RhoH=Gc_Kpa * SLDensity * Head;
  IOPB_Self(i).PEst=P;
  IOPB_Self(i).P=P;
  IOFB(i,NIOFBs(i)-1)->SetDPzX(-RhoH, 0.0);
  };

//--------------------------------------------------------------------------

double FlwNode::IOP_RhoH_Self(int i)
  {
  return -IOFB(i,NIOFBs(i)-1)->DPzX();
  };

//--------------------------------------------------------------------------

double FlwNode::IOP_RhoH_Rmt(int i)
  {
  return -IOFB_Rmt(i,NIOFBs_Rmt(i)-1)->DPzX();
  };

//--------------------------------------------------------------------------

void FlwNode::Clr_RhoH_Self(int i)
  {
  IOFB(i,NIOFBs(i)-1)->SetDPzX(0.0, 0.0);
  };

////--------------------------------------------------------------------------
//
//bool FlwNode::IOFB_GetRmtArea(int c, double &Area)
//  {
//  Area=0.0;
//  CFBPhysData * p=IOFB_GetRmtPhysData(c);
//  if (p==NULL)
//    return false;
//  Area=p->Area();
//  return true;
//  };
//
////--------------------------------------------------------------------------
//
//CFBPhysData * FlwNode::IOFB_GetRmtPhysData(int c)
//  {
//  IOFBFlng_Rmt(c)->Area();
//  pFB->PhD()->Area();
//
//
//  CFBPhysData * p=NILL;
//  return p;
//  };
//
//--------------------------------------------------------------------------

CPressBlk & FlwNode::JoinPBlk(int iJoinId)
  {
  if (Joins.GetUpperBound()<iJoinId)
    {
    m_nJoins=Max(m_nJoins, iJoinId+1);
    Joins.SetSize(iJoinId+1);
    }
  return Joins[iJoinId].m_PB;
  };

//---------------------------------------------------------------------------

ContStartInfo::ContStartInfo(SpContainer & C) :
  MassA(C.MArray())
  {
  totH0=C.totHf();
  Pres=C.Press();
  Mass=C.Mass();
  MassV=C.Mass(som_Gas);
  MassL=C.Mass(som_SL);
  };

//---------------------------------------------------------------------------

void FlwNode::RestoreContents(SpContainer & C, ContStartInfo & Start)
  {
  C.MArray().Set(Start.MassA, som_ALL, 1.0);
  C.Set_totHf(Start.totH0);
  };

//---------------------------------------------------------------------------

void FlwNode::ZeroVLSensitivities(int JoinId)
  {
  if (JoinId<Joins.GetSize())
    {
    CJoinRec & J=Joins[JoinId];
    for (int c=0; c<J.NConns(); c++)
      {
      int i=J.IONo(c);
  //    SpConduit &Cd=*IOConduit(i);
      Set_IO_RhoHSensVL(i, 0.0,0.0);
      Set_IO_PSensVL(i, 0.0, 0.0);
      Set_IO_V2LQmFact(i, 0.0, 0.0);
      }
    }
  };

//---------------------------------------------------------------------------

void FlwNode::SetVLSensitivities(int JoinId, SpContainer & C, ContStartInfo & Start, bool DoConvergeStates)
  {
  // Initial Conditions
  //SpMArray &ContentsMA=C.MArray();
  //  SpMArray Start.MassA(CMA);
//  double totH0=C.totEnthalpy();
//  double dtotH0=C.dtotEnthalpy();
//  double Start.Pres=C.Press();
//  double MassStart=C.Mass();
//  double Start.MassV=C.Mass(som_Gas);
//  double MassStartL=C.Mass(som_SL);


  //ContStartInfo Start(C);
  // Vapour Sensitivity
  //double QMassV=1.0;
  //double DeltaMass=Range(1.0e-6, 0.1*Max(1.0, Start.MassV)/NZ(fabs(QMassV)), 1.0);
  double DeltaMass=Max(1.0e-10, 0.001*Start.MassV);///NZ(fabs(QMassV)), 1.0);
  C.AddM(C, som_Gas, DeltaMass);
  double AvgPSensV=(C.Press()-Start.Pres)/DeltaMass;
  RestoreContents(C, Start);

  // Liquid Sensitivity
  //double QMassL=1.0;
  //DeltaMass=Range(1.0e-6, 0.1*Max(1.0, Start.MassL)/NZ(fabs(QMassL)), 1.0);
  DeltaMass=Max(1.0e-10, 0.001*Start.MassL);
  C.AddM(C, som_SL, DeltaMass);
  double AvgPSensL=(C.Press()-Start.Pres)/DeltaMass;
  RestoreContents(C, Start);
  // Set them
//  Set_JoinP_SensVL(0, PSensV, PSensL);
//  dbgpln("Sens V/L  V:%14.6g L:%14.6g %s", PSensV, PSensL, FullObjTag());


  #if dbgFlwNode
  if (dbgSensitivity())
    dbgpln("Sens %s", FullObjTag());
  #endif
  ContStartInfo Equil(C);

  double Mv0=0.0;
  double Mv1=0.0;
  double /*DeltaMass,*/ P0, P1;
  CJoinRec & J=Joins[JoinId];
  for (int c=0; c<J.NConns(); c++)
    {
    int i=J.IONo(c);
    SpConduit &Cd=*IOConduit(i);

    // Transfer to retain equil due to movement of vapour
    RestoreContents(C, Equil);
    P0=C.Press();
    DeltaMass=Max(1.0e-6, 0.01*Equil.MassV);
    C.AddM(Cd, som_Gas, DeltaMass);

    if (DoConvergeStates)
      {
      Mv0=C.Mass(som_Gas);
      CConvergeStateBlk CSB;
      ConvergeStates(CSB);
      Mv1=C.Mass(som_Gas);
      }
    double V2LQVFrac=(Mv0-Mv1)/DeltaMass;
    //double xxP1=C.Press();
    P1=C.Press();
    double PSensV=(fabs(P1-P0)>1.0e-6) ? (P1-P0)/DeltaMass : AvgPSensV;

    // Transfer to retain equil due to movement of liquor
    RestoreContents(C, Equil);
    P0=C.Press();
    DeltaMass=Max(1.0e-6, 0.01*Equil.MassL);
    C.AddM(Cd, som_SL, DeltaMass);

    if (DoConvergeStates)
      {
      Mv0=C.Mass(som_Gas);
      CConvergeStateBlk CSB;
      ConvergeStates(CSB);
      Mv1=C.Mass(som_Gas);
      }
    double V2LQLFrac=(Mv0-Mv1)/DeltaMass;

    P1=C.Press();
//    double PSensL=(P1-P0)/DeltaMass;
    double PSensL=(fabs(P1-P0)>1.0e-6) ? (P1-P0)/DeltaMass : AvgPSensL;

    Set_IO_PSensVL(i, PSensV, PSensL);
    Set_IO_V2LQmFact(i, V2LQVFrac, V2LQLFrac);

    #if dbgFlwNode
    if (dbgSensitivity())
      dbgpln("     P/V:%14.6g P/L:%14.6g V2LV:%14.6g V2LL:%14.6g %s", PSensV, PSensL, V2LQVFrac, V2LQLFrac, Nd_Rmt(i)->FullObjTag());
    #endif
    }
  RestoreContents(C, Equil);
  }

//---------------------------------------------------------------------------

void FlwNode::SetStaticHeadSensitivities(int JoinId, SpContainer & C, ContStartInfo & Start)
  {
  double RhoL=C.Rho(som_SL);
  double Levl=C.Level();
  double Area=C.ShapeVolume()/Max(0.1, C.ShapeHeight());
  double dZL=1.0/(RhoL*Area); // Change in hgt of liquid per kg of liquid

  double dRhoHL=dZL*9.81*RhoL/1000.0;

  CJoinRec & J=Joins[JoinId];
  for (int c=0; c<J.NConns(); c++)
    {
    int i=J.IONo(c);
    if (IOAbsFracHgt_Term(i, C.ShapeHeight(), C.ShapeDatumPt())<Levl)
      Set_IO_RhoHSensVL(i, dRhoHL, dRhoHL);
    else
      Set_IO_RhoHSensVL(i, 0.0,0.0);
    }
  #if dbgFlwNode
  if (dbgSensitivity())
    dbgpln("     RhoH V:%14.6g L:%14.6g", dRhoHL, dRhoHL);
  #endif
  }

//---------------------------------------------------------------------------

void FlwNode::SetEquilibratingV2L(int JoinId, SpContainer & C, ContStartInfo & Start)
  {
  double Tau=ICGetTimeInc()*2;
  // Transfer due to movement toward equil
  double MV0,MV1,Duty=0.0;//, DeltaMass;
  MV0=C.Mass(som_Gas);
  CConvergeStateBlk CSB;
  ConvergeStates(CSB);
  MV1=C.Mass(som_Gas);
  double V2LQ=(MV0-MV1)/Tau;
  SetJoinV2LFlow(JoinId, V2LQ);
  #if dbgFlwNode
  if (dbgSensitivity())
    dbgpln("     V2L    %14.6g ", V2LQ);
  #endif
  }

//--------------------------------------------------------------------------

flag FlwNode::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tBad Link";                               return 1;
    case  2: pS="W\tLow (Negative) Pressure";                return 1;
    case  3: pS="W\tFlows Not Converged";                    return 1;
    case  4: pS="W\tImpedance Range Modified";               return 1;
    case  5: pS="W\tLine Pressure Not Converged";            return 1;
    case  6: pS="W\tLine Flow Not Converged";                return 1;
    case  7: pS="W\tLine Flow Not Converged (Not Started)";  return 1;
    case  8: pS="W\tLine Flow Not Converged (No Straddle)";  return 1;
    case  9: pS="W\tMass & Energy Balance not achieved";     return 1;
    case 10: pS="W\tFlows OverSpecified";                    return 1;
    case 11: pS="W\tFlows OverSpecified - No Balance";       return 1;
    case 12: pS="W\tZ OverSpecified";                        return 1;
    case 13: pS="W\tFwd & Rev Eval Specified";               return 1;
    case 14: pS="W\tLink preventing network convergence";    return 1;
    case 15: pS="W\tLarge Flow - Set to Zero";               return 1;
    case 16: pS="E\tBad PreStartCheck";                      return 1;
    case 17: pS="E\tInvalid Data";                           return 1;
    case 18: pS="W\tRegulator Out of range";                 return 1;
    case 19: pS="W\tEvalMsAGData not Balanced";              return 1;
    case 20: pS="E\t";                                       return 1;
    case 21: pS="W\tVol Fraction not found";                 return 1;
    case 22: pS="W\tVol Fraction not found (Not Started)";   return 1;
    case 23: pS="E\tEvalSurgeProducts not Called";           return 1;
    case 24: pS="E\tEvalProducts not Called";                return 1;
    case 25: pS="N\tHold Active";                            return 1;
    case 26: pS="E\tBad external tag references";            return 1;
    case 27: pS="W\tEngineering units invalid";              return 1;
    case 28: pS="W\tDynTags missing ";                       return 1;
    case 29: pS="W\tSpecie Model Operating Out of Range";    return 1;
    case 30: pS="W\tNotAuditNode";                           return 1;
    case 31: pS="W\tAreaNotFOund";                           return 1;
    case 32: pS="W\tNBadDirects";                            return 1;
    case 33: pS="W\tNBadPwrCOnnects";                        return 1;
    case 34: pS="W\tNBadCluster";                            return 1;
    case 35: pS="W\tProcedure reload required";              return 1;
    default:
      return CTNode::CIStrng(No, pS);
    }
  };

//--------------------------------------------------------------------------

void FlwNode::OnAppActivate(BOOL bActive)
  {
#if WITHNODEPROCS
  if (ProcsOn(NPM_Any))
    m_pProcs->OnAppActivate(bActive);
#endif
  };

//--------------------------------------------------------------------------

int FlwNode::FilesUsed(CFilesUsedArray & Files)
  {
  //Expect model with reactions etc to override this virtual method eg:
  //ASSERT_ALWAYS(fHasFiles==0, "HasFiles!=0", __FILE__, __LINE__);
#if WITHNODEPROCS
  if (ProcsOn(NPM_Any))
    return m_pProcs->FilesUsed(Files);
#endif
  return 0;
  };

//--------------------------------------------------------------------------

int FlwNode::ChangeTag(char * pOldTag, char * pNewTag)
  {
#if WITHNODEPROCS
  if (ProcsOn(NPM_Any))
    return m_pProcs->ChangeTag(pOldTag, pNewTag);
#endif
  return EOCT_DONE;
  };

//--------------------------------------------------------------------------

int FlwNode::DeleteTag(char * pDelTag)
  {
#if WITHNODEPROCS
  if (ProcsOn(NPM_Any))
    return m_pProcs->DeleteTag(pDelTag);
#endif
  return EODT_DONE; 
  };

//--------------------------------------------------------------------------

int FlwNode::GetConnectionTags(Strng_List & TagLst)
  {
  for (int i=0; i<NoProcLnkIOs(); i++)
    {
    Strng *p=new Strng(Nd_Rmt(i)->FullObjTag());
    p->SetIndex(0);
    TagLst.Append(p);
    }
  return i;
  };

//---------------------------------------------------------------------------

void FlwNode::InitChangeTrace()
  {
  #if dbgFlwNode
  if (dbgTraceChanges())
    dbgpln("Trc Init   : %s", Tag());
  #endif
  for (int io=0; io<NoFlwIOs(); io++)
    {
    #if dbgFlwNode
    if (dbgTraceChanges())
      dbgpln("                             Flw  : %s", Nd_Rmt(io)->Tag());
    #endif
    CTraceArray &CA=IOChanges(io);
    CA.SetSize(5);
    CA[0].Init(2, "Qm", "kg/s", FullIOTag(OId_NodeIO, IODescNo_Self(io), TU_IndividuallyUnique, false), Nd_Rmt(io)->FullObjTag());
    CA[1].Init(3, "T",  "C",    FullIOTag(OId_NodeIO, IODescNo_Self(io), TU_IndividuallyUnique, false), Nd_Rmt(io)->FullObjTag());
    CA[2].Init(4, "P",  "kPa",  FullIOTag(OId_NodeIO, IODescNo_Self(io), TU_IndividuallyUnique, false), Nd_Rmt(io)->FullObjTag());
    CA[3].Init(5, "Sf",  "%",   FullIOTag(OId_NodeIO, IODescNo_Self(io), TU_IndividuallyUnique, false), Nd_Rmt(io)->FullObjTag());
    CA[4].Init(6, "Lf",  "%",   FullIOTag(OId_NodeIO, IODescNo_Self(io), TU_IndividuallyUnique, false), Nd_Rmt(io)->FullObjTag());
    }
  for (io=CIO1(); io<CION(); io++)
    {
    #if dbgFlwNode
    if (dbgTraceChanges())
      dbgpln("                             Ctrl : %s", Nd_Rmt(io)->Tag());
    #endif
    CTraceArray &CA=IOChanges(io);
    CA.SetSize(1);
    CA[0].Init(1, "Mv", "", FullIOTag(OId_NodeIO, IODescNo_Self(io), TU_IndividuallyUnique, false), Nd_Rmt(io)->FullObjTag());
    }
  for (io=EIO1(); io<EION(); io++)
    {
    #if dbgFlwNode
    if (dbgTraceChanges())
      dbgpln("                             Elec : %s", Nd_Rmt(io)->Tag());
    #endif
    CTraceArray &CA=IOChanges(io);
//ELEC     int nPh=m_IOs[io].ElecConduit().m_nPhases;
//ELEC     CA.SetSize(2*nPh);
//ELEC     static LPCSTR VTag[]={"V1", "V2", "V3"};
//ELEC     static LPCSTR ITag[]={"I1", "I2", "I3"};
//ELEC     for (int i=0; i<nPh; i++)
//ELEC       {
//ELEC       CA[i*2].Init(1, VTag[i], "", FullIOTag(OId_NodeIO, IODescNo_Self(io), TU_IndividuallyUnique, false), Nd_Rmt(io)->FullObjTag());
//ELEC       CA[i*2+1].Init(1, ITag[i], "", FullIOTag(OId_NodeIO, IODescNo_Self(io), TU_IndividuallyUnique, false), Nd_Rmt(io)->FullObjTag());
//ELEC       }
    }

  for (io=0; io<NoXRefs2Me(); io++)
    {
    #if dbgFlwNode
    if (dbgTraceChanges())
      dbgpln("                             XRef : %s", CXRef(io)->SFGNodeTag());
    #endif
    CTraceItem &C=CXRef(io)->m_Value.m_Chg;
    char * pCnv=CXRef(io)->m_iCnvIndex>0 ? Cnvs[CXRef(io)->m_iCnvIndex]->Txt() :"";
    C.Init(1, "Mv", pCnv?pCnv:"", CXRef(io)->SFGNodeTag(), CXRef(io)->DstNd()->FullObjTag());
    }
  };

//---------------------------------------------------------------------------

void FlwNode::TraceChanges()
  {
  #if dbgFlwNode
  if (dbgTraceChanges())
    dbgpln("Trc Changes: %s", Tag());
  #endif

  for (int io=0; io<NoFlwIOs(); io++)
    {
    #if dbgFlwNode
    if (dbgTraceChanges())
      dbgpln("                             Flw  : %s", Nd_Rmt(io)->Tag());
    #endif
    CTraceArray &CA=IOChanges(io);
    if (CA.GetSize()>=5)
      {
      CA[0].Set(IOConduit(io)->QMass());
      CA[1].Set(K2C(IOConduit(io)->Temp()));
      CA[2].Set(IOConduit(io)->Press());
      CA[3].Set(IOConduit(io)->MassFrac(som_Sol));
      CA[4].Set(IOConduit(io)->MassFrac(som_Liq));
      }
    }
  for (io=CIO1(); io<CION(); io++)
    {
    #if dbgFlwNode
    if (dbgTraceChanges())
      dbgpln("                             Ctrl : %10.2f %s", m_IOs[io].CtrlData().m_dValue, Nd_Rmt(io)->Tag());
    #endif
    CTraceArray &CA=IOChanges(io);
    if (CA.GetSize()>=1)
      CA[0].Set(m_IOs[io].CtrlData().m_dValue);
    }
  for (io=EIO1(); io<EION(); io++)
    {
    #if dbgFlwNode
    if (dbgTraceChanges())
      dbgpln("                             Elec : %s", Nd_Rmt(io)->Tag());
    #endif
    CTraceArray &CA=IOChanges(io);
    //ELEC     int nPh=m_IOs[io].ElecConduit().m_nPhases;
    //ELEC     if (CA.GetSize()>=nPh*2)
    //ELEC       {
    //ELEC       for (int i=0; i<nPh; i++)
    //ELEC         {
    //ELEC         CElecConduit &D=m_IOs[io].ElecConduit();
    //ELEC         CA[i*2].Set(D.m_Phase[i].m_dV);
    //ELEC         CA[i*2+1].Set(D.m_Phase[i].m_dI);
    //ELEC         }
    //ELEC       }
    }
  for (io=0; io<NoXRefs2Me(); io++)
    {
    if (CXRef_Out(io))
      {
      #if dbgFlwNode
      if (dbgTraceChanges())
        dbgpln("                             XRef : %10.2f %s", CXRef(io)->m_Value.m_Chg.Val(0), CXRef(io)->SFGNodeTag());
      #endif
      CXRef(io)->m_Value.m_Chg.Set(CXRef(io)->TagValue().DoubleSI);
      }
    }
  };

//---------------------------------------------------------------------------

CTraceArray * FlwNode::GetChangeTrace(long i, dword Flags)
  {
//  int i=0;
//  //ChgStats.SetSize(0, 3);
//  for (int io=0; io<NoFlwIOs(); io++)
//    {
//    if (IOQmEst_Out(io)>0.0)
//      {
//      SpConduit &Fo=*IOConduit(io);
//      ChgStats.SetSize(Max(i+3, ChgStats.GetSize()), 3);
//      Strng DstTag=Nd_Rmt(io)->FullObjTag();
//      ChgStats[i++].Set(FullObjTag(), DstTag(), "Qm", K_2_C(Fo.QMass()), dNAN);
//      ChgStats[i++].Set(FullObjTag(), DstTag(), "T", K_2_C(Fo.Temp()), dNAN);
//      ChgStats[i++].Set(FullObjTag(), DstTag(), "P", K_2_C(Fo.Press()), dNAN);
//      /*
//      for (int s=0; s<SDB.No(); s++)
//        {
//        Tag.Set("%s.Qi.%s", FullObjTag(), SDB[s].SymOrTag());
//        ChgStats[i++].Set(Tag(), Fo.Qm(s), pImg ? pImg->M(s) : dNAN);
//        }
//      */
//      }
//    }
//  ChgStats.SetSize(i, 3);
//  return &ChgStats;
  return NULL;
  };

//--------------------------------------------------------------------------

void FlwNode::EvalAudit(int Pass)
  {
  if (AuditOption()>Audit_Off)
    {
    bool dbgOn=false;
    if (dbgOn)
      dbgpln("EvalAudit [%i] %s", Pass, FullObjTag());
    switch (Pass)
      {
      case 0:
        {
        m_ExtAudit.Integrate(ICGetTimeInc());
        for (int i=0; i<NoProcessIOs(); i++)
          {
          // Only do the Ins
          // The model at the other end will do this if it is 'out'
          if (IOQm_In(i)>=0.0)
            {
            CAreaSum &Sum=IOFlange(i)->Sum;
            Sum.Set(*IOConduit(i));
            if (AuditOption()>=Audit_Totals)
              Sum.Integrate(ICGetTimeInc());
            if (dbgOn)
              dbgpln("%2i)    f %20s %20s %s",i,DbgFltString(Sum.FlowMass),DbgFltString(Sum.TotalMass), Nd_Rmt(i)->FullObjTag());
            }
          }

        if (NoEnabledDirectIOs()>0)
          {
          // Integrate All DirectIOs
          for (int d=FirstMkupDirect(); d<=LastAuditDirect(); d++)
            {
            CDirectFlwIO &D=*m_EnabledDirectIOs[d];
            if (AuditOption()>=Audit_Totals)
              D.Sum.Integrate(ICGetTimeInc());
            if (dbgOn)
              dbgpln("%2i)[%2i]d %20s %20s %s",i++,d,DbgFltString(D.Sum.FlowMass),DbgFltString(D.Sum.TotalMass), D.Tag);
            }
          }
        break;
        }
      case 1:
        break;
      }
    }
  }

//--------------------------------------------------------------------------

void FlwNode::GetBalanceReport(CNodeAuditInfo & Audit)
  {
  Audit.m_Report="FlwNode::GetBalanceReport\r\n "
                 "=========================\r\n"
                 "\r\n"
                 "Etc\r\n"
                 ;

  };

//--------------------------------------------------------------------------

#if WithIOChgMonitor
#define WithSpecies 0
void FlwNode::MonitorPreEvalProducts()
  {
  #if dbgFlwNode
  if (dbgDumpChanges())
    {

    for (int i=0; i<NoXRefs2Me(); i++)
      {
      CXRefItem *pRef=CXRef(i);
      pRef->m_Value.m_TagValueMem=pRef->TagValue();
      }

    for (i=0; i<NoFlwIOs(); i++)
      {
#if WithSpecies
      int M=6+SDB.Count();
#else
      int M=6;
#endif
      int N=2*M;
      m_IOs[i].DataMem.SetSize(N);
      CDArray &DM=m_IOs[i].DataMem;
      ASSERT(DM.GetSize()>0);
      for (int j=0; j<2; j++)
        {
        SpConduit *C=(j==0)?IOConduitIn(i):IOConduitIO(i);
        if (C==NULL)
          continue;
        DM[0+j*M]=C->QMass();
        DM[1+j*M]=C->Temp();
        DM[2+j*M]=C->Press();
        DM[3+j*M]=C->totHf();
        DM[4+j*M]=C->MassFrac(som_Sol);
        DM[5+j*M]=C->MassFrac(som_Liq);
#if WithSpecies
        for (int ss=0; ss<SDB.Count(); ss++)
          DM[6+ss+j*M]=C->VMass[ss];
#endif
        }
      }
    }
  #endif
  }

//--------------------------------------------------------------------------

void FlwNode::MonitorPostEvalProducts()
  {
  #if dbgFlwNode
  if (dbgDumpChanges()) 
    {
    dbglock();
    int nSpc=120;
    dbgpln("%*s %s [%s]", nSpc, "                        -------------------------------", FullObjTag(), ClassId());
    //int N=6;//dbgDumpChangesIn() 8 : 4;
#if WithSpecies
    int M=6+SDB.Count();
#else
    int M=6;
#endif
    CDArray MaxVal;
    MaxVal.SetSize(M);
    for (int i=0; i<NoFlwIOs(); i++)
      {
      if (!IOQmEst_In(i) || dbgDumpChangesIn())
        {
        CDArray &DM=m_IOs[i].DataMem;
        ASSERT(DM.GetSize()>0);

        int j1=IOQmEst_In(i)?0:1;
        for (int j=j1; j<2; j++)
        //int j=1;
          {
          SpConduit * C=(j==0)?IOConduitIn(i):IOConduitIO(i);
          if (C==NULL)
            continue;

          MaxVal[0]=Max(MaxVal[0], Max(DM[0+j*M], C->QMass()));
          MaxVal[1]=Max(MaxVal[1], Max(DM[1+j*M], C->Temp()));
          MaxVal[2]=Max(MaxVal[2], Max(DM[2+j*M], C->Press()));
          //MaxVal[3]=Max(MaxVal[3], Max(DM[3+j*M], C->totEnthalpy()));
          MaxVal[4]=Max(MaxVal[4], Max(DM[4+j*M], C->MassFrac(som_Sol)));
          MaxVal[5]=Max(MaxVal[5], Max(DM[5+j*M], C->MassFrac(som_Liq)));
#if WithSpecies
          for (int ss=0; ss<SDB.Count(); ss++)
            MaxVal[6+ss]=Max(MaxVal[6+ss], Max(DM[6+ss+j*M], C->VMass[ss]));
#endif
          }
        }
      }

    for (i=0; i<NoFlwIOs(); i++)
      {
      if (!IOQmEst_In(i) || dbgDumpChangesIn())
        {
        CDArray &DM=m_IOs[i].DataMem;

        int j1=IOQmEst_In(i)?0:1;
        for (int j=j1; j<2; j++)
        //int j=1;
          {
          SpConduit * C=(j==0)?IOConduitIn(i):IOConduitIO(i);
          if (C==NULL)
            continue;

          #define DBGIT(A,B,C,D,ShowIt,T)                                                                         \
          if (ShowIt && fabs((B-A)/GTZ(Max(fabs(A), fabs(B))))>dbgDumpChangeTol()*1.0e-6)                         \
            dbgpln("EP %18.10f > %18.10f %+15.6f %+25.16f *e-6 %+25.16f *e-6 %s(%-4.4s): %2s%s %s->%s",           \
            A, B, (B-A), ShowIt?Range(-9999999., 9999999*(B-A)/GTZ(Max(fabs(A), fabs(B))), 9999999.):0.0,         \
                         ShowIt?Range(-9999999., 9999999*(B-A)/GTZ(Max(Max(fabs(A), fabs(B)), C)), 9999999.):0.0, \
                         IOQmEst_In(i)>0.0 ? (j==0?"TrIn":"In  "):"Out ",                                         \
                         IODesc_Self(i)->IOName(), D, T,                                                             \
                         FullObjTag(), Nd_Rmt(i)->FullObjTag());

          char * Tmp=(j==0)?"*":" ";
          bool ShowIt = true || C->QMass()>1.0e-6;
          DBGIT(DM[0+j*M], C->QMass(),            MaxVal[0], "Qm", ShowIt, Tmp);
          DBGIT(DM[1+j*M], C->Temp(),             MaxVal[1], "T",  ShowIt, Tmp);
          DBGIT(DM[2+j*M], C->Press(),            MaxVal[2], "P",  ShowIt, Tmp);
          //DBGIT(DM[3+j*M], C->totEnthalpy(),      MaxVal[3], "H",  DoIt, Tmp);
          DBGIT(DM[4+j*M], C->MassFrac(som_Sol),  MaxVal[4], "Sf", ShowIt, Tmp);
          DBGIT(DM[5+j*M], C->MassFrac(som_Liq),  MaxVal[5], "Lf", ShowIt, Tmp);
#if WithSpecies
          for (int ss=0; ss<SDB.Count(); ss++)
            {
            DBGIT(DM[6+ss+j*M], C->VMass[ss],  MaxVal[6+ss], SDB[ss].SymOrTag(), ShowIt, Tmp);
            }
#endif
          #undef DBGIT
          }

        }
      }
    //dbgpln("");

#if STILLTODO
    _asm int 3;
    #define DBGIT(A,B,IO,T)  \
      {   \
      if (fabs((B-A)/GTZ(Max(fabs(A), fabs(B))))>dbgDumpChangeTol()*1.0e-6) \
        dbgpln("XR %18.10f > %18.10f %+15.6f %+25.16f *e-6 %31s%-3s %s", \
          A, B, (B-A), Range(-9999999., 9999999*(B-A)/Max(1e-12, Max(fabs(A), fabs(B))), 9999999.), "", IO, T); \
        }
    #define DBGITX(A,B,IO,T)  \
      dbgpln("XR*%18s > %18.10f %+15.6f %+25.16f *e-6 %31s%-3s %s", \
      "NAN", B, B, 9999999*1.0, "", IO, T)

    for (i=0; i<NoXRefs2Me(); i++)
      {
      if (CXRef_DstNd(i)==this)
        {
        CXRefItem *pRef=CXRef(i);
        if (Valid(pRef->m_Value.m_dValMem))
          DBGIT(pRef->m_Value.m_TagValueMem.AsString(), pRef->TheValue(),  "In        :", pRef->SFGNodeTag(this))
        else
          DBGITX(pRef->m_Value.m_dValMem, pRef->TheValue(), "In        :", pRef->SFGNodeTag(this));
        }
      }

    for (i=0; i<NoXRefs2Me(); i++)
      {
      if (CXRef_SrcNd(i)==this)
        {
        CXRefItem *pRef=CXRef(i);
        if (Valid(pRef->m_Value.m_dValMem))
          DBGIT(pRef->m_Value.m_dValMem, pRef->TheValue(),  "Out       :", pRef->SFGNodeTag(this))
        else
          DBGITX(pRef->m_Value.m_dValMem, pRef->TheValue(), "Out       :", pRef->SFGNodeTag(this));
        }
      }

    #undef DBGIT
    #undef DBGITX
#endif

    dbgunlock();
    }
  #endif
  }

//--------------------------------------------------------------------------

void FlwNode::MonitorPostTestTears()
  {
  #if dbgFlwNode
  if (dbgDumpChanges())
    {
    for (int i=0; i<NoFlwIOs(); i++)
      {
      if (!IOQmEst_In(i) || dbgDumpChangesIn())
        {
        if (IOConduitIn(i))
          {
          for (int j=0; j<2; j++)
            {
            SpConduit * C=(j==0)?IOConduitIn(i):IOConduitIO(i);
            if (C==NULL)
              continue;
            goto DoIt;
            }
          }
        }
      }
    goto Done;

DoIt:
    dbglock();
    int nSpc=99;
    dbgpln("%*s %s", nSpc, "", FullObjTag());
    //int N=6;//dbgDumpChangesIn() 8 : 4;
    for (int i=0; i<NoFlwIOs(); i++)
      {
      int M=6;
      //int N=2*M;
      if (!IOQmEst_In(i) || dbgDumpChangesIn())
        {
        CDArray &DM=m_IOs[i].DataMem;

        dbgpln("%*s %s : %s : %s",
           nSpc, "", IOQmEst_In(i)>0.0 ? "In":"Out",
           IODesc_Self(i)->IOName(),
           Nd_Rmt(i)->FullObjTag());

        if (IOConduitIn(i))
          {
          for (int j=0; j<2; j++)
            {
            SpConduit * C=(j==0)?IOConduitIn(i):IOConduitIO(i);
            if (C==NULL)
              continue;

            #define DBGIT(A,B,D,ShowIt,T)  \
              if (fabs((B-A)/GTZ(Max(fabs(A), fabs(B))))>dbgDumpChanges()*1.0e-6) \
                 dbgpln("TT %18.10f > %18.10f %+15.6f %+25.16f *e-6 %+25.16f *e-6     %s %s", \
              A, B, (B-A), ShowIt?9999999*(B-A)/GTZ(Max(fabs(A), fabs(B))):0.0, ShowIt?9999999*(B-A)/GTZ(Max(fabs(A), fabs(B))):0.0, D, T)

            char * Tmp=(j==0)?"*":"";
            DBGIT(DM[0+j*M], C->QMass(),            "Qm", true, Tmp);
            DBGIT(DM[1+j*M], C->Temp(),             "T",  true, Tmp);
            DBGIT(DM[2+j*M], C->Press(),            "P",  true, Tmp);
            //DBGIT(DM[3+j*M], C->totEnthalpy(),      "H",  Tmp);
            DBGIT(DM[4+j*M], C->MassFrac(som_Sol),  "Sf", true, Tmp);
            DBGIT(DM[5+j*M], C->MassFrac(som_Liq),  "Lf", true, Tmp);
            #undef DBGIT
            }
          }
        }
      }
    //dbgpln("");
    dbgunlock();
      
Done:;
    }
  #endif
  }
#endif
//--------------------------------------------------------------------------

void FlwNode::ResetMakeUpCnt()
  {
  for (int i=0; i<NoFlwIOs(); i++)
    IOFlange(i)->ResetMakeUpCnt();
  };

//--------------------------------------------------------------------------

void FlwNode::BumpMakeUpCnt()
  {
  for (int i=0; i<NoFlwIOs(); i++)
    IOFlange(i)->BumpMakeUpCnt();
  };

//--------------------------------------------------------------------------

void FlwNode::SteadyStateInit(eScdSSTests ssTest)
  {
  for (int i=0; i<NoFlwIOs(); i++)
    {
    CPressBlk & PB=IOPB_Flng(i);
    PB.PStdyState = PB.P;
    FlwBlk & FB=*IOFB(i, 0);
    FB.m_QmStdyState = FB.m_Qm;
    }
  };

bool FlwNode::SteadyStateTest(eScdSSTests ssTest, CToleranceBlock & PTol, CToleranceBlock & QmTol,
                   double & MaxError, Strng & MaxTag)
  {
  double MaxErr=0;
  bool OK=true;
  for (int i=0; OK && i<NoFlwIOs(); i++)
    {
    FlwBlk & FB=*IOFB(i, 0);
    if (!QmTol.ConvergedVV(FB.m_QmStdyState, FB.m_Qm))
      {
      //dbgpln("SST Qm %12.4f %12.4f %12.4f %s", FB.m_QmStdyState, FB.m_Qm, QmTol.GetNormalError(), FullTag());
      OK=false;
      }
    MaxErr=Max(MaxErr, QmTol.GetNormalError());

    if (fabs(FB.m_Qm)>SmallPosFlow)
      {
      CPressBlk & PB=IOPB_Flng(i);
      if (!PTol.ConvergedVV(PB.PStdyState, PB.P))
        {
        //dbgpln("SST P  %12.4f %12.4f %12.4f %s", PB.PStdyState, PB.P, PTol.GetNormalError(), FullTag());
        OK=false;
        }
      MaxErr=Max(MaxErr, PTol.GetNormalError());
      }
    }
  
  if (MaxErr>MaxError)
    {
    MaxError=MaxErr;
    MaxTag=FullObjTag();
    }
  return OK;
  };

//--------------------------------------------------------------------------

void FlwNode::ClosureInfo()
  {
  };

//--------------------------------------------------------------------------

void FlwNode::GetClosureInfo(bool DoFlows, bool DoClear)
  {
  m_Closure.Setup(DoFlows, NoJoins());
  if (DoClear)
    m_Closure.ZeroContent();

  if (DoFlows)
    {
    m_Closure.ZeroFlows();
    for (int i=0; i<NoFlwIOs(); i++)
      {
      int j=IO_Join_Id(i);
      CClosureInfo &CI=m_Closure[j];
      if (IOQm_In(i)>0.0)
        {
        CI.m_MassFlwIn+=IOConduit(i)->QMass();
        CI.m_totHfIn+=IOConduit(i)->totHf();
        CI.m_totHsIn+=IOConduit(i)->totHs();
        CI.m_FeedCpT+=IOConduit(i)->totCp();
        }
      else
        {
        CI.m_MassFlwOut+=IOConduit(i)->QMass();
        CI.m_totHfOut+=IOConduit(i)->totHf();
        CI.m_totHsOut+=IOConduit(i)->totHs();
        CI.m_ProdCpT+=IOConduit(i)->totCp();
        }
      }
    for (int j=0; j<m_Closure.GetCount(); j++)
      {
      CClosureInfo &CI=m_Closure[j];
      CI.m_FeedCpT/=GTZ(CI.m_MassFlwIn);
      CI.m_ProdCpT/=GTZ(CI.m_MassFlwOut);
      }
    }

  ClosureInfo(); // call it
  };

//--------------------------------------------------------------------------

double FlwNode::GetBalanceValue(eScdBalanceValues BV, int Join)
  {
  if (!m_Closure.DataAvail())
    return dNAN;

  if (!m_Closure.DataValid())
    {
    GetClosureInfo(true, true);
    m_Closure.SetDataValid(true);
    }

  switch (BV)
    {
    case eBV_JoinMassFlwIn        : return m_Closure[Join].m_MassFlwIn;
    case eBV_JoinMassGain         : return m_Closure[Join].m_MassGain-m_Closure[Join].m_MassLoss;
    case eBV_JoinMassFlwOut       : return m_Closure[Join].m_MassFlwOut;
    case eBV_JoinFeedCpT          : return m_Closure[Join].m_FeedCpT;
    case eBV_JoinProdCpT          : return m_Closure[Join].m_ProdCpT;

    case eBV_JoinHfIn             : return m_Closure[Join].m_totHfIn;
    case eBV_JoinHfGain           : return m_Closure[Join].m_HfGain-m_Closure[Join].m_HfLoss;
    case eBV_JoinHsGain           : return m_Closure[Join].m_HsGain-m_Closure[Join].m_HsLoss;
    case eBV_JoinHfOut            : return m_Closure[Join].m_totHfOut;
    case eBV_JoinHfChg            : return m_Closure[Join].m_totHfOut-m_Closure[Join].m_totHfIn;
    case eBV_JoinHsIn             : return m_Closure[Join].m_totHsIn;
    case eBV_JoinHsOut            : return m_Closure[Join].m_totHsOut;
    case eBV_JoinHsChg            : return m_Closure[Join].m_totHsOut-m_Closure[Join].m_totHsIn;
    case eBV_JoinPowerIn          : return m_Closure[Join].m_PowerIn;

    case eBV_RCTMassGain          : return m_Closure[Join].m_RCTMassGain;
    case eBV_RCTHfGain            : return m_Closure[Join].m_RCTHfGain;
    case eBV_RCTHsGain            : return m_Closure[Join].m_RCTHsGain;
    case eBV_RCTPowerIn           : return m_Closure[Join].m_RCTPowerIn;
    case eBV_RCTHOR_Std0          : return m_Closure[Join].m_RCTHOR_Std0;
    case eBV_RCTHOR_Mdl0          : return m_Closure[Join].m_RCTHOR_Mdl0;
    case eBV_RCTHOR_Used0         : return m_Closure[Join].m_RCTHOR_Used0;
    case eBV_RCTHOR_Diff0         : return m_Closure[Join].m_RCTHOR_Diff0;

    case eBV_VLEHsGain            : return m_Closure[Join].m_HfGainAtZero;
    case eBV_EHXPowerIn           : return m_Closure[Join].m_EHXPowerIn;
    }

  return dNAN;
  };

//--------------------------------------------------------------------------

void FlwNode::DumpStates()
  {
  }

//--------------------------------------------------------------------------

void FlwNode::DumpDerivs()
  {
  }

//--------------------------------------------------------------------------

void FlwNode::SetQmEst2IO(double Rqd, eSQ2IO DoWhat)
  {
  // Assumes that FBlk exists
  if (NoFlwIOs()>=2)
    {
    bool DoRmt0=false;
    bool DoRmt1=false;
    switch (DoWhat)
      {
      case SQ2IO_All:
        DoRmt0=true;
        DoRmt1=true;
        break;
      case SQ2IO_NonLink:
        DoRmt0=!Nd_Rmt(0)->IsLnk();
        DoRmt1=!Nd_Rmt(1)->IsLnk();
        break;
      }
    SetIOQmEst_In(0, Rqd, DoRmt0);
    SetIOQmEst_Out(1, Rqd, DoRmt1);
    }
  }

//--------------------------------------------------------------------------

void FlwNode::SetQm2IO(double Rqd, eSQ2IO DoWhat)
  {
  //if (Rqd && NoFlwIOs()>=2) CNM Why was Rqd here - presence is not checked but value
  if (NoFlwIOs()>=2)
    {
    bool DoRmt0=false;
    bool DoRmt1=false;
    switch (DoWhat)
      {
      case SQ2IO_All:
        DoRmt0=true;
        DoRmt1=true;
        break;
      case SQ2IO_NonLink:
        DoRmt0=!Nd_Rmt(0)->IsLnk();
        DoRmt1=!Nd_Rmt(1)->IsLnk();
        break;
      }
    SetIOQm_In(0, Rqd, DoRmt0);
    SetIOQm_Out(1, Rqd, DoRmt1);
    }
  }

//--------------------------------------------------------------------------

FlwNode * FlwNode::GetQmEstSrc2IO()
  {
  if (NoFlwIOs()>0)
    return IOFB(0,0)->GetQmEstSrc();
  return NULL;
  }

//--------------------------------------------------------------------------

LPCTSTR FlwNode::GetQmEstType2IO()
  {
  if (NoFlwIOs()>0)
    {
    FlwBlk & FB=*IOFB(0,0);
    if (FB.m_pGFb==NULL)              return "";
    if (FB.m_pGFb->m_fFixedExternal)  return "Set Remote";
    if (FB.m_pGFb->m_fFixedXfer)      return "Transfer";
    if (FB.m_pGFb->m_fUnDefined)      return "No Set(Zero)";
    if (FB.m_pGFb->m_fFixed)          return "Set";
    //if (FB.m_bf Attached2GFB_fX.m_ ) return "Request";
    //if (FB.m_fFixed) return "Request";
    return "Free";
    }
  return "??";
  }

//--------------------------------------------------------------------------

double FlwNode::GetQmEst2IO(double *Rqd)
  {
  if (Rqd)
    SetQmEst2IO(*Rqd);
  if (NoFlwIOs()>0)
    return IOFB(0,0)->GetQmEst();
  return dNAN;
  }

//--------------------------------------------------------------------------

double FlwNode::GetQm2IO(double *Rqd)
  {
  if (Rqd)
    SetQm2IO(*Rqd);
  if (NoFlwIOs() == 1)
    return IOConduit(0)->QMass(som_ALL);
  else if (NoFlwIOs() >= 2)
    return IOFB(0,0)->GetQm();
  return dNAN;
  }

//--------------------------------------------------------------------------

flag FlwNode::OnLoad(pchar pConfigName)
  {
  int IOsComplete=True;
  int ChkId=0;
  for (int i=0; i<NoIOs(); i++)
    {
    //check id order ?
    if (IOId_Self(i)<ChkId)
      LogError(FullObjTag(), 0, "Connections Order is Bad");
    ChkId=IOId_Self(i);

    if (!m_IOs[i].fIsComplete)
      IOsComplete=False;
    }

  if (!IOsComplete)
    {
    LogError(FullObjTag(), 0, "Incomplete Connections");
    }

  return True;
  };

//--------------------------------------------------------------------------

flag FlwNode::OnSave(pchar pConfigName)
  {
  return True;
  };

//--------------------------------------------------------------------------

flag FlwNode::PreStartCheck()
  {
  return CNodeXRefMngr::UpdateXRefLists(true)>=0;
  };

//==========================================================================
#if DOPBSMOOTHING

static int DoMergeAGDatas=true;
static int ReduceSpCount=true;

void CMSIO::SetSrc(FlwNode *pNd, int IONo)
  {
  m_pNd=pNd;
  m_iIONo=IONo;
  };
void CMSIO::SetDst(FlwNode *pNd, int IONo, CMsAGData * AGData)
  {
  m_pNd=pNd;
  m_iIONo=IONo;
  if (MSIndex())
    MSIndex()->SetSrc(AGData);
  };

//--------------------------------------------------------------------------

void CMSIO::CheckIndex(CMSIndexInit &Index)
  {
  MSIndex()->CheckIndex(Index);
  };

//==========================================================================

CMSIndex::CMSIndex()
  {
  Clear();
  }

//--------------------------------------------------------------------------

void CMSIndex::Clear()
  {
  m_fMustExist=false;
  m_fMerged=false;
  m_iIndex=-1;
  m_iSpIndex.SetSize(SDB.No());
  for (int s=0; s<m_iSpIndex.GetSize(); s++)
    m_iSpIndex[s]=-2;
  m_pSrcAGData=NULL;
  }

//--------------------------------------------------------------------------

void CMSIndex::CheckIndex(CMSIndexInit &IndexInit)
  {
  #if dbgFlwNodeMS
  flag dbg=dbgMSSetIndices();
  #endif
  if (!m_fMerged)
    {
    if (Index()<0 && !m_fMerged)
      Index()=IndexInit.m_iLnkIndex++;
    for (int s=0; s<SpIndex().GetSize(); s++)
      {
      #if dbgFlwNodeMS
      if (dbg) dbgp("%25.25s  %5i", SDB[s].SymOrTag(),SpIndex()[s]);
      #endif
      if (SpIndex()[s]==0)
        {
        SpIndex()[s]=IndexInit.m_iSpIndex++;
        #if dbgFlwNodeMS
        if (dbg) dbgp(" >> %5i", SpIndex()[s]);
        #endif
        }
      #if dbgFlwNodeMS
      if (dbg) dbgpln("");
      #endif
      }
    }
  else
    {
    #if dbgFlwNodeMS
    if (dbg) dbgpln("%25.25s","** Merged **");
    #endif
    }
  };

//==========================================================================

void CMSJoin::EstimateData(CMSVector & MTotI, CMSVector & MTotO)
  {
  // Assume No Extra Prop In or Out
  // Assume Const In or Out is Correct
  double TotI=MTotI.Sum();
  double TotO=MTotO.Sum();

  SpMArray MassLoss=0;
  SpMArray MassGain=0;

  for (int s=0; s<SDB.No(); s++)
    {
    double D=MTotI[s]-MTotO[s];//+MassInP[s]-MassOutP[s];
    if (m_ConstBG.IsValid())
      D+=m_ConstBG[s];
    if (m_ConstAG.IsValid())
      D-=m_ConstAG[s];
    if (D>0)
      MassLoss[s]=D;
    else if (D<0)
      MassGain[s]=-D;
    }
  double TMassLoss=MassLoss.Sum();
  double TMassGain=MassGain.Sum();

  flag Inited=false;
  for (s=0; s<SDB.No(); s++)
    {
    if (MassGain[s]>1.0e-12)// && nMSrcIOs)
      {
      if (!Inited)
        {
        m_Gamma.Init(true);
        Inited=true;
        }
      double Gain=MassGain[s]/GTZ(TMassGain);
      for (int sg=0; sg<SDB.No(); sg++)
        {
        double Denom=MTotI[sg];
        if (m_ConstBG.IsValid())
          Denom+=m_ConstBG[sg];//+MassInP[s]);
        double Frac=MassLoss[sg]/GTZ(Denom);//+MassInP[s]);
        m_Gamma[s][sg]=Frac*Gain;
        }
      m_Gamma[s][s]+=1.0;
      }
    else if (MassLoss[s]>1.0e-12)// && nMSrcIOs)
      {
      if (!Inited)
        {
        m_Gamma.Init(true);
        Inited=true;
        }
      double Denom=MTotI[s];
      if (m_ConstBG.IsValid())
          Denom+=m_ConstBG[s];//+MassInP[s]);
      double Frac=MassLoss[s]/GTZ(Denom);//+MassInP[s]);
      m_Gamma[s][s]=1.0-Frac;
      }
    }

  if (!Inited)
    m_Gamma.Clear();
  else
    m_Gamma.ChkMeaningFull();
  };

//==========================================================================

CMsAGData::CMsAGData()
  {
  m_pMrgAGData=NULL;
  Clear();
  }

//--------------------------------------------------------------------------

CMsAGData::~CMsAGData()
  {
  Clear();
  }

//--------------------------------------------------------------------------

void CMsAGData::Clear()
  {
  m_Op=CMSOp_None;
  m_fProdSpRefd=true;
  m_fMerged=false;
  m_fSwitched=false;
  m_fMergeSrc=false;
  m_fMergeDst=false;
  m_fMergeDstIfUnity=false;
  m_fUnity=false;

  delete m_pMrgAGData;
  m_pMrgAGData=NULL;
  }

//--------------------------------------------------------------------------
#if DOPB_FILT_ALPHAS
#define M_EST m_Est[0]
#define M_AVG m_Avg
#else
#define M_EST m_Avg
#define M_AVG m_Avg
#endif

//--------------------------------------------------------------------------

void CMsAGData::Copy(CMsAGData * pSrc)
  {
  m_Op=pSrc->m_Op;
  m_iJoinNo=-1;
  m_pNd=NULL;

  #if DOPB_FILT_ALPHAS
  m_Est.SetSize(1);
  #endif
  M_EST.m_SpReqd.Init();
  M_EST.m_MConst.Init();
  M_EST.m_MAlpha.Init();
  M_EST.m_MGamma.Init(true);

  m_fUnity = pSrc->m_fUnity;
//  m_DstIO = pSrc->m_DstIO;
  if (pSrc->M_EST.m_SpReqd.IsValid())
    M_EST.m_SpReqd=pSrc->M_EST.m_SpReqd;
  if (pSrc->M_EST.m_MConst.IsValid())
    M_EST.m_MConst=pSrc->M_EST.m_MConst;
  if (pSrc->M_EST.m_MAlpha.IsValid())
    M_EST.m_MAlpha=pSrc->M_EST.m_MAlpha;
  if (pSrc->M_EST.m_MGamma.IsValid())
    M_EST.m_MGamma=pSrc->M_EST.m_MGamma;
  }

//--------------------------------------------------------------------------

void CMsAGData::DoMerge(CMsAGData * pSrc, flag DstUnity)
  {

  if (m_Op!=CMSOp_SrcMult)
    DoBreak();
  if (m_SrcIOs.GetSize()>1)
    DoBreak();

  if (DstUnity)
    m_Op=pSrc->m_Op;
  m_pNd=pSrc->m_pNd;
  m_fUnity=m_fUnity && pSrc->m_fUnity;

  //m_SrcIOs.Copy(pSrc->m_SrcIOs);
  // Check that these do not exist
  if (M_EST.m_MBeta.IsValid() || M_EST.m_MPhi.IsValid())
    {
    DoBreak();
    int xxx=0;
    }

  if (pSrc->M_EST.m_SpReqd.IsValid())
    M_EST.m_SpReqd|=pSrc->M_EST.m_SpReqd;

  if (!pSrc->m_fUnity)
    {
    if (M_EST.m_MGamma.IsValid())
      {
      CMSVector V;
      V.Mult(pSrc->M_EST.m_MGamma, M_EST.m_MConst);
      M_EST.m_MConst=V;
      M_EST.m_MConst.Add(pSrc->M_EST.m_MConst);

      if (pSrc->M_EST.m_MGamma.IsValid())
        {
        // Both Exist
        CMSMatrix T;
        T.Mult(pSrc->M_EST.m_MGamma, M_EST.m_MGamma);
        M_EST.m_MGamma=T;
        }
      else
        {
        // No Gamma Here
        }
      }
    else if (pSrc->M_EST.m_MGamma.IsValid())
      {
      // First Gamma
      M_EST.m_MGamma=pSrc->M_EST.m_MGamma;
      M_EST.m_MConst.Add(pSrc->M_EST.m_MConst);
      }
    if (pSrc->M_EST.m_MBeta.IsValid())
      M_EST.m_MBeta=pSrc->M_EST.m_MBeta;
    if (pSrc->M_EST.m_MPhi.IsValid())
      M_EST.m_MPhi=pSrc->M_EST.m_MPhi;
//    m_MOutIOs.Copy(pSrc->m_MOutIOs);
    }
  }

//--------------------------------------------------------------------------

void CMsAGData::DoMSDumpGamma(CMsAGData * pOther)
  {
  CMSMatrix * Gamma[2];
  Gamma[1]= &M_EST.m_MGamma;
  Gamma[0]= pOther ? &pOther->M_EST.m_MGamma : NULL;

  int NColsMax[2]={0,0};
  for (int g=(pOther ? 0 : 1); g<2; g++)
    {
    for (int i=0; i<SDB.No(); i++)
      {
      int NColsM=0;
      if (Gamma[g]->IsValid())
        for (int j=0; j<SDB.No(); j++)
          if ((*Gamma[g])[i][j]!=0.0)
            NColsM++;
      NColsMax[g]=Max(NColsMax[g], NColsM);
      }
    }

  //dbgpln("Data for : ---------------------------------- %s[%i]", FullObjTag(), j);
  if (Gamma[0]->IsValid() || (Gamma[1] && Gamma[1]->IsValid()))
    {
    for (int i=0; i<SDB.No(); i++)
      {
      dbgp(pOther->M_EST.m_SpReqd[i] ? "*":" ");
      //for (int g=0; g<(pOther ? 2 : 1); g++)
      for (int g=(pOther ? 0 : 1); g<2; g++)
        {
        if ((*Gamma[g]).IsValid())
          {
          for (int j=0; j<SDB.No(); j++)
            {
            if ((*Gamma[g])[i][j]>0.0)
              dbgp("+");
            else if ((*Gamma[g])[i][j]<0.0)
              dbgp("-");
            else if (j%2==1)
              dbgp(".");
            else
              dbgp(" ");
            }
          dbgp(":");
          int n=0;
          for (j=0; j<SDB.No(); j++)
            if ((*Gamma[g])[i][j]!=0.0)
              {
              dbgp(" %8.2f", (*Gamma[g])[i][j]);
              n++;
              }
          for ( ; n<NColsMax[g]; n++)
            dbgp(" %8s", "");
          }
        dbgp(" |");
        }
      dbgpln(" %s", SDB[i].SymOrTag());
      }
    }
  }

//--------------------------------------------------------------------------

flag CMsAGData::Merge(CMsAGData * pDstAGData, CMsAGData * & pMrgdAG)
  {
  m_fMergeDstIfUnity=false;

  flag MrgDst=false, DstUnity=false;
  if (m_fMergeDst)
    MrgDst=true;
  else if (m_fMergeDstIfUnity)
    {
    if (pMrgdAG)
      MrgDst=pMrgdAG->m_fUnity;
    else if (pDstAGData)
      MrgDst=pDstAGData->m_fUnity;
    DstUnity=true;
    }

  if (!m_fMerged && MrgDst)
    {
    flag SrcMrgd=false;
    if (m_fMergeSrc)
      {
      CMSIndex & MSI=*m_SrcIOs[0].MSIndex();
      if (MSI.SrcAGData() && !MSI.MustExist())
        SrcMrgd=MSI.SrcAGData()->Merge(this, pMrgdAG);
      }

    // Do Arithmetic
    if (SrcMrgd)
      {
      ASSERT_ALWAYS(pMrgdAG!=NULL, "No Merged Data", __FILE__, __LINE__);
      #if dbgFlwNodeMS
      if (dbgMSMerge())
        dbgpln("       %s", m_pNd->FullObjTag());
      #endif

      pMrgdAG->DoMerge(this, DstUnity);
      pMrgdAG->m_DstIO = m_DstIO;
      pMrgdAG->m_MOutIOs.Copy(m_MOutIOs);
      m_fMerged=true;

      #if dbgFlwNodeMS
      if (dbgMSDumpGamma())
        DoMSDumpGamma(pMrgdAG);
      #endif
      }
    else
      {
      ASSERT_ALWAYS(pMrgdAG==NULL, "Merged Data Exists", __FILE__, __LINE__);
      #if dbgFlwNodeMS
      if (dbgMSMerge())
        dbgpln("Merge  %s", m_pNd->FullObjTag());
//        dbgpln("Merge  %s", pDstAGData->m_pNd->FullObjTag());
      #endif

      pMrgdAG=new CMsAGData;
      pMrgdAG->Copy(this);
      pMrgdAG->m_SrcIOs.Copy(m_SrcIOs);
      pMrgdAG->m_DstIO = m_DstIO;
      pMrgdAG->m_MOutIOs.Copy(m_MOutIOs);
      m_fMerged=true;

      m_pMrgAGData=pMrgdAG;

      #if dbgFlwNodeMS
      if (dbgMSDumpGamma())
        DoMSDumpGamma(pMrgdAG);
      #endif
      }
    if (pDstAGData)
      m_DstIO.MSIndex()->m_fMerged=true;

    #if dbgFlwNodeMS
    if (dbgMSMerge() && pDstAGData==NULL)
      dbgpln("       -------------");
    #endif

    return true;
    }

  return false;
  }

//--------------------------------------------------------------------------

void CMsAGData::MarkMsIndices()
  {
  if (m_pMrgAGData)
    m_pMrgAGData->MarkMsIndices();

  flag InxMerged=this->m_DstIO.MSIndex()->m_fMerged;

  for (int s=0; s<SDB.No(); s++)
    {
    // Force All Specie
    if (!ReduceSpCount)
      goto SpecieRqd;
    if (M_EST.m_SpReqd.IsValid() && M_EST.m_SpReqd[s])
      goto SpecieRqd;
    #if dbgFlwNodeMS
    if (dbgMSSetIndices())
      dbgpln("%s.%-25s -", m_pNd->Tag(), SDB[s].SymOrTag());
    #endif
    continue;
SpecieRqd:
    m_DstIO.SetSpIndexRqd(s, InxMerged);//m_fMerged);
    #if dbgFlwNodeMS
    if (dbgMSSetIndices())
      dbgpln("%s.%-25s %3i %s", m_pNd->Tag(), SDB[s].SymOrTag(), m_DstIO.SpIndex(s), m_DstIO.SpIndex(s)<0?"Mgd":"Reqd");
    #endif
    }
  };

//--------------------------------------------------------------------------

void CMsAGData::Dump(char * Tag, int Jn)
  {
  Strng S1,S2;
  dbgp("%s", Tag);
  dbgp(Jn>=0 ? "[%i]":"   ", Jn);
  if (m_fUnity)
    dbgp(" UNITY ");
  if (Jn<0)
    dbgp(" ** MERGE **");
  dbgpln("");

  int MxSpInx=1;
  for (int ss=0; ss<SDB.No(); ss++)
    {
    MxSpInx=Max(MxSpInx, m_DstIO.SpIndex(ss));
    for (int j=0; j<m_SrcIOs.GetSize(); j++)
      MxSpInx=Max(MxSpInx, m_SrcIOs[j].SpIndex(ss));
    }
  int SpDigs=MxSpInx<100 ? 2 : MxSpInx<1000 ? 3 : MxSpInx<10000 ? 4 : 5;

  for (int j=0; j<m_SrcIOs.GetSize(); j++)
    {
    if (j>0) S1+=",";
    S1+=m_SrcIOs[j].Nd_Rmt()->Tag();
    S1+="..";
    S1+=m_SrcIOs[j].Nd()->Tag();
    }

  j=m_SrcIOs.GetSize();
  dbgp("    %*s ",35+j*SpDigs+SpDigs-1, S1());
  dbgp(" >> %s", m_DstIO.Nd()->Tag());
  dbgp("..%s", m_DstIO.Nd_Rmt()->Tag());
  dbgpln("");
  for (int s=0; s<SDB.No(); s++)
    {
    // Removes UnUsed species
    if (m_DstIO.SpIndex(s)<-1)
      continue;

    dbgp((Jn<0 || !m_fMerged) ? "#":" ");
    dbgp("   ");
    switch (m_Op)
      {
      case CMSOp_SrcMult: dbgp("       "); break;
      case CMSOp_IODiff : dbgp("IODiff "); break;
      case CMSOp_RmtMult: dbgp("RmtMult"); break;
      default: dbgp("???????"); break;
      }
    dbgp("  %25.25s", SDB[s].SymOrTag());
    if (m_SrcIOs.GetSize())
      {
      dbgp(" [");
      for (int j=0; j<m_SrcIOs.GetSize(); j++)
        {
        if (j>0) dbgp(",");
        if (m_SrcIOs[j].SpIndex(s)>0)
          dbgp("%*i", SpDigs, m_SrcIOs[j].SpIndex(s));
        else
          dbgp("%*s", SpDigs, "");
        }
      dbgp("]");
      }
    dbgp(" >> ");
    if(m_DstIO.SpIndex(s)>0)
      dbgp("[%*i]", SpDigs, m_DstIO.SpIndex(s));
    else
      dbgp("[%*s]", SpDigs, "");
    if (m_SrcIOs.GetSize() && M_EST.m_MAlpha.IsValid())
      S1.Set("%+11.4g",M_EST.m_MAlpha[s]);
    else if (m_SrcIOs.GetSize())
      S1="*";
    else
      S1=".";

    dbgp(" %10s %+11.4g ", S1(), M_EST.m_MConst.IsValid() ? M_EST.m_MConst[s] : 0.0);

    int Done1=false;
    Done1=false;
    if (M_EST.m_MGamma.IsValid())
      {
      for (int ss=0; ss<SDB.No(); ss++)
        if (M_EST.m_MGamma[s][ss]!=0)
          {
          if (!Done1)
            {
            Done1=true;
            dbgp("   ");
            }
          else
            dbgp(" ");
          dbgp(" %+.4g* %s", M_EST.m_MGamma[s][ss], SDB[ss].SymOrTag());
          //dbgp("[");
          //for (int j=0; j<m_SrcIOs.GetSize(); j++)
          //  dbgp("%s%i", j>0 ? ",":"", m_SrcIOs[j].SpIndex(ss));
          //dbgp("]");
          }
      }
    Done1=false;
    if (M_EST.m_MBeta.GetCols())
      {
      for (int ss=0; ss<SDB.No(); ss++)
        if (M_EST.m_MBeta[s][ss]!=0)
          {
          if (!Done1)
            {
            Done1=true;
            dbgp("   /");
            }
          else
            dbgp(" ");
          dbgp(" %+.4g* %s", M_EST.m_MBeta[s][ss], SDB[ss].SymOrTag());
          //dbgp("[");
          //for (int j=0; j<m_MOutIOs.GetSize(); j++)
          //  dbgp("%s%i", j>0 ? ",":"", m_MOutIOs[j].SpIndex(ss));
          //dbgp("]/");
          }
      if (Done1)
        dbgp("/");
      }
    if (m_Op==CMSOp_RmtMult)
      {
      dbgp(" %+.4g*", M_EST.m_RmtScl);
      if (M_EST.m_MPhi.IsValid())
        {
        dbgp(" \\");
        if (M_EST.m_MPhi[s]!=0)
          dbgp(" %+.4g* %s", M_EST.m_MPhi[s], SDB[s].SymOrTag());
        dbgp(" ");
        for (int j=0; j<m_RmtIOs.GetSize(); j++)
          dbgp(" %s", m_RmtIOs[j].Nd_Rmt()->Tag());
        dbgp("\\");
        }
      }

    dbgpln("");
    }
  };

//--------------------------------------------------------------------------

inline void CalcAvg(double &a, double e, double Filt)
  {
  if (a!=0.0 && e!=0)
    {
    a=(Filt)*a+(1.0-Filt)*e;
    }
  else
    {
    a=e;
    }
  }

//--------------------------------------------------------------------------

void CMsAGData::SmoothEsts()
  {
  static double Filt=0.0;
  if (!m_fMerged && !m_fSwitched)
    {
    m_Avg.m_MAlpha.SetSize(SDB.No());
    for (int s=0; s<SDB.No(); s++)
      CalcAvg(m_Avg.m_MAlpha[s], M_EST.m_MAlpha[s], Filt);

    if (M_EST.m_MGamma.IsValid())
      {
      m_Avg.m_MGamma.SetSize(SDB.No(), SDB.No());
      for (int r=0; r<M_EST.m_MGamma.GetRows(); r++)
        for (int c=0; c<M_EST.m_MGamma.GetCols(); c++)
          CalcAvg(m_Avg.m_MGamma[r][c], M_EST.m_MGamma[r][c], Filt);
      }

    m_Avg.m_MConst.SetSize(SDB.No());
    for (s=0; s<SDB.No(); s++)
      CalcAvg(m_Avg.m_MConst[s], M_EST.m_MConst[s], Filt);

    CalcAvg(m_Avg.m_RmtScl, M_EST.m_RmtScl, Filt);

    if (M_EST.m_MPhi.IsValid())
      {
      m_Avg.m_MPhi.SetSize(SDB.No());
      for (int s=0; s<SDB.No(); s++)
        CalcAvg(m_Avg.m_MPhi[s], M_EST.m_MPhi[s], Filt);
      }

    if (M_EST.m_MBeta.IsValid()>0)
      {
      m_Avg.m_MBeta.SetSize(SDB.No(), SDB.No());
      for (int r=0; r<M_EST.m_MBeta.GetRows(); r++)
        for (int c=0; c<M_EST.m_MBeta.GetCols(); c++)
          CalcAvg(m_Avg.m_MBeta[r][c], M_EST.m_MBeta[r][c], Filt);
      }
    }
  };

//--------------------------------------------------------------------------

void CMsAGData::AddMsEqns(SparseSolver & SS, int & Eqn)
  {
  if (m_pMrgAGData && !m_fMerged)
    DoBreak();

  if (m_pMrgAGData)
    m_pMrgAGData->AddMsEqns(SS, Eqn);

  if (!m_fMerged && !m_fSwitched)
    {
    #if DOPB_FILT_ALPHAS
    SmoothEsts();
    #endif

    for (int s=0; s<SDB.No(); s++)
      {
      if (m_DstIO.SpIndex(s)>0)
        {
        //dbgpln("AddMsEqns %3i %3i %s.%s ", Eqn, m_DstIO.SpIndex(s), m_pNd->Tag(), SDB[s].SymOrTag());
        switch (m_Op)
          {
          case CMSOp_SrcMult:
            {
            SS.SetValue(Eqn, m_DstIO.SpIndex(s), 1);
            //if (M_AVG.m_MAlpha[s]>0.0)
            //  for (int j=0; j<m_SrcIOs.GetSize(); j++)
            //    if (m_SrcIOs[j].SpIndex(s)>0)
            //      SS.SetValue(Eqn, m_SrcIOs[j].SpIndex(s), -M_AVG.m_MAlpha[s]);
            if (M_AVG.m_MGamma.IsValid())
              for (int ss=0; ss<SDB.No(); ss++)
                if (M_AVG.m_MGamma[s][ss]!=0.0)
                  for (int j=0; j<m_SrcIOs.GetSize(); j++)
                    if (m_SrcIOs[j].SpIndex(ss)>0)
                      SS.SetValue(Eqn, m_SrcIOs[j].SpIndex(ss), -M_AVG.m_MGamma[s][ss]);
            SS.SetVector(Eqn, M_AVG.m_MConst[s]);
            Eqn++;
            break;
            }
          case CMSOp_RmtMult:
            {
            SS.SetValue(Eqn, m_DstIO.SpIndex(s), 1);
            //SS.SetVector(Eqn, -M_AVG.m_MConst[s]);
            if (M_AVG.m_MPhi.IsValid())
              if (M_AVG.m_RmtScl*M_AVG.m_MPhi[s]!=0.0)
                for (int ss=0; ss<SDB.No(); ss++)
                  for (int j=0; j<m_RmtIOs.GetSize(); j++)
                    if (m_RmtIOs[j].SpIndex(ss)>0)
                      SS.SetValue(Eqn, m_RmtIOs[j].SpIndex(ss), -M_AVG.m_RmtScl*M_AVG.m_MPhi[s]);
            SS.SetVector(Eqn, M_AVG.m_MConst.IsValid() ? -M_AVG.m_MConst[s] : 0.0);
            Eqn++;
            break;
            }
          case CMSOp_IODiff:
            {
            SS.SetValue(Eqn, m_DstIO.SpIndex(s), 1);
            if (M_AVG.m_MGamma.IsValid())
              for (int ss=0; ss<SDB.No(); ss++)
                if (M_AVG.m_MGamma[s][ss]!=0.0)
                  for (int j=0; j<m_SrcIOs.GetSize(); j++)
                    if (m_SrcIOs[j].SpIndex(ss)>0)
                      SS.SetValue(Eqn, m_SrcIOs[j].SpIndex(ss), -M_AVG.m_MGamma[s][ss]);
//            if (M_AVG.m_MAlpha.IsValid() && M_AVG.m_MAlpha[s]!=0.0)
//              for (int j=0; j<m_SrcIOs.GetSize(); j++)
//                if (m_SrcIOs[j].SpIndex(s)>0)
//                  SS.SetValue(Eqn, m_SrcIOs[j].SpIndex(s), -M_AVG.m_MAlpha[s]);
            if (M_AVG.m_MBeta.IsValid())
              for (int ss=0; ss<SDB.No(); ss++)
                if (M_AVG.m_MBeta[s][ss]!=0.0)
                  for (int j=0; j<m_MOutIOs.GetSize(); j++)
                    if (m_MOutIOs[j].SpIndex(ss)>0)
                      SS.SetValue(Eqn, m_MOutIOs[j].SpIndex(ss), +M_AVG.m_MBeta[s][ss]);
            SS.SetVector(Eqn, M_AVG.m_MConst.IsValid() ? -M_AVG.m_MConst[s] : 0.0);
            Eqn++;
            break;
            }
          }
        }
      }
    }
  };

//==========================================================================

void FlwNode::ClearMsIndices()
  {
  #if dbgFlwNodeMS
  DoMergeAGDatas=!dbgMSSkipMerge();
  ReduceSpCount=!dbgMSSkipSpcReduce();
  #endif
  for (int i=0; i<NoIOs(); i++)
    IOMSIndex(i)->Clear();
  for (i=0; i<MsAGDatas.GetSize(); i++)
    MsAGDatas[i].Clear();
  }

//--------------------------------------------------------------------------

inline flag UnityTest(double x, double y) { return fabs(x-y)<1.0e-12; };

void FlwNode::EvalMsAGDatas()
  {
  if (fDoDbgBrk)
    { int xxx=0;};

  int nSrcIOs=0;
  int nDstIOs=0;
  for (int i=0; i<NoIOs(); i++)
    {
    if (IO_In(i))
      nSrcIOs++;
    else if (IO_Out(i))
      nDstIOs++;
    }

  MsAGDatas.SetSize(nDstIOs);
  MsJoins.SetSize(Joins.GetSize());

  flag fBalOK=true;

  int iDstIO=0;
  for (int j=0; j<Joins.GetSize(); j++)
    {
    int iDstIO1=iDstIO;
    CJoinRec &J=Joins[j];
    CMSVector MTotI;
    CMSVector MTotO;
    MTotI.Init();
    MTotO.Init();
    CMSIO MSrcIOs[MaxIOList];
    int nMSrcIOs=0;
    int nMDstIOs=0;
    for (int c=0; c<J.NConns(); c++)
      {
      int i=J.IONo(c);
      if (IO_In(i))
        {
        MSrcIOs[nMSrcIOs++].SetSrc(this, i);
        MTotI.Add(IOConduit(i)->MArray(), 1);
        }
      else if (IO_Out(i))
        {
        nMDstIOs++;
        MTotO.Add(IOConduit(i)->MArray(), 1);
        }
      }

    double TotI=MTotI.Sum();
    double TotO=MTotO.Sum();

    //SpMArray MassLoss=0;
    //SpMArray MassGain=0;
    CMSVector & JnConstBG=MsJoins[j].m_ConstBG;
    CMSVector & JnConstAG=MsJoins[j].m_ConstAG;
    CMSMatrix & JnPropBG=MsJoins[j].m_PropBG;
    CMSVector & JnPropAG=MsJoins[j].m_PropAG;
    CMSMatrix & JnGamma=MsJoins[j].m_Gamma;

    if (!MsJoins[j].m_fDataIsValid)
      MsJoins[j].EstimateData(MTotI, MTotO);

    #if dbgFlwNodeMS
    if (dbgMSDumpGamma())
      {
      int NGColsMax=0;
      int NPColsMax=0;
      for (int i=0; i<SDB.No(); i++)
        {
        int NGCols=0;
        if (JnGamma.IsValid())
          for (int j=0; j<SDB.No(); j++)
            if (JnGamma[i][j]!=0.0)
              NGCols++;
        NGColsMax=Max(NGColsMax, NGCols);
        //int NPCols=0;
        //if (JnConstBG.IsValid())
        //  for (int j=0; j<SDB.No(); j++)
        //    if (JnConstBG[i][j]!=0.0)
        //      NPCols++;
        //NPColsMax=Max(NPColsMax, NPCols);
        }

      dbgpln("Data for : ---------------------------------- %s[%i]", FullObjTag(), j);
      if (JnConstBG.IsValid() || JnGamma.IsValid())// || JnConstBG.IsValid())
        {
        if (JnConstBG.IsValid())
          dbgp("%8s","ConstBG");
        if (JnGamma.IsValid())
          {
          dbgp("%8s","Gamma");
          //for (int j=1; j<SDB.No(); j++)
          //  dbgp("%8s", "");
          }
        dbgpln("");

        for (i=0; i<SDB.No(); i++)
          {
          if (JnConstBG.IsValid())
            dbgp("%8.2f", JnConstBG[i]);
          dbgp(" |");
          if (JnGamma.IsValid())
            {
            for (int j=0; j<SDB.No(); j++)
              {
              if (JnGamma[i][j]>0.0)
                dbgp("+");
              else if (JnGamma[i][j]<0.0)
                dbgp("-");
              else if (j%2==1)
                dbgp(".");
              else
                dbgp(" ");
              }
            dbgp(":");
            int n=0;
            for (j=0; j<SDB.No(); j++)
              if (JnGamma[i][j]!=0.0)
                {
                dbgp(" %8.2f", JnGamma[i][j]);
                n++;
                }
            for ( ; n<NGColsMax; n++)
              dbgp(" %8s", "");
            }
          dbgp(" |");
          //if (JnPropBG.IsValid())
          //  {
          //  for (int j=0; j<SDB.No(); j++)
          //    {
          //    if (JnPropBG[i][j]>0.0)
          //      dbgp("+");
          //    else if (JnPropBG[i][j]<0.0)
          //      dbgp("-");
          //    else if (j%2==1)
          //      dbgp(".");
          //    else
          //      dbgp(" ");
          //    }
          //  dbgp(":");
          //  int n=0;
          //  for (j=0; j<SDB.No(); j++)
          //    if (JnPropBG[i][j]!=0.0)
          //      {
          //      dbgp(" %8.2f", JnPropBG[i][j]);
          //      n++;
          //      }
          //  for ( ; n<NPColsMax; n++)
          //    dbgp(" %8s", "");
          //  }
          ////if (JnGamma.IsValid())
          ////  for (int j=0; j<SDB.No(); j++)
          ////    dbgp(" %8.2f", JnGamma[i][j]);
          //
          //dbgp(" |");
          dbgpln(" %s", SDB[i].SymOrTag());
          }
        }
      }
    #endif

    bool MergeSrc=(nMSrcIOs==1 && nMDstIOs==1);
    bool MergeDst=(nMSrcIOs==1 && nMDstIOs==1);

    //SpMArray AMTot;
    flag Exists[MaxSpecies];
    for (int s=0; s<SDB.No(); s++)
      Exists[s]=false;

    for (c=0; c<J.NConns(); c++)
      {
      int i=J.IONo(c);
      if (IO_In(i))
        { }//IOMSIndex(i)->m_fMustExist=!MergeSrc;
      else if (IO_Out(i))
        { // Add Dst
        int iOut=i;
        //IOMSIndex(iOut)->m_fMustExist=!MergeDst;
        CMsAGData & MSA=MsAGDatas[iDstIO++];
        #if DOPB_FILT_ALPHAS
        MSA.m_Est.SetSize(1);
        #endif

        MSA.m_Op=CMSOp_SrcMult;
        MSA.m_iJoinNo=j;
        MSA.m_pNd=this;
        MSA.m_fMergeSrc=MergeSrc;
        MSA.m_fMergeDst=MergeDst;
        MSA.m_fUnity=true;

        MSA.m_DstIO.SetDst(this, iOut, &MSA);
        MSA.m_SrcIOs.SetSize(nMSrcIOs);
        for (int m=0; m<MSA.m_SrcIOs.GetSize(); m++)
          MSA.m_SrcIOs[m]=MSrcIOs[m];

        CMSFlags  & IoSpRqd=MSA.M_EST.m_SpReqd;
        CMSVector & IoConst=MSA.M_EST.m_MConst;
        CMSVector & IoAlpha=MSA.M_EST.m_MAlpha;
        CMSMatrix & IoGamma=MSA.M_EST.m_MGamma;

        //IoGamma.Init(false);//SetAll(0.0);

        IoSpRqd.Init();
        IoConst.Init();
        if (JnConstBG.IsValid())
          {
          if (JnGamma.IsValid())
            IoConst.Mult(JnGamma, JnConstBG);
          else
            IoConst=JnConstBG;
          MSA.m_fUnity=false;
          }
        if (JnConstAG.IsValid())
          IoConst.Sub(JnConstAG);

        if (JnGamma.IsValid())
          {
          //IoGamma.MultRows(JnPropBG, JnGamma);
          if (JnPropBG.IsValid())
            {
            IoGamma.Mult(JnGamma, JnPropBG);
            IoGamma.Add(JnGamma);
            }
          else
            IoGamma=JnGamma;
          MSA.m_fUnity=false;
          }
        else
          IoGamma.Init(true);

        CMSVector MTotX;
        MTotX.Mult(IoGamma, MTotI);
        MTotX.Add(IoConst);

        IoAlpha.Init();
        for (s=0; s<SDB.No(); s++)
          {
          //double OutSplit=(IOConduit(iOut)->Qm(s))/GTZ(MTotO[s]+MassOutP[s]);
          double OutSplit=(IOConduit(iOut)->Qm(s))/GTZ(MTotX[s]);
          if (MTotX[s]>1.0e-12)
            {
            Exists[s]=true;
            IoAlpha[s]=ChkMeaningFull(OutSplit);
            if (!UnityTest(IoAlpha[s], 1.0))
              MSA.m_fUnity=false;
            }
          else
            IoAlpha[s]=1.0/nMDstIOs;//.0;
//            IoAlpha[s]=ReduceSpCount ? -1.0 : 0.0;

          double Out=MTotO[s];
          if (JnPropAG.IsValid())
            Out+=JnPropAG[s];
          if (fabs(MTotX[s]-Out)> 1.0e-6*0.5*(MTotX[s]+Out))
            fBalOK=false;
          if ((MTotI[s]!=0.0) || (MTotO[s]!=0.0) || (IoConst[s]!=0.0))
            IoSpRqd[s]=true;
          }

        IoConst.Mult(IoAlpha);
        IoGamma.MultRows(IoAlpha);

        IoConst.ChkMeaningFull();
        IoGamma.ChkMeaningFull();

        //AMTot+=MSA.M_EST.m_MAlpha;
        }
      }

//    int iDst=iDstIO1;
//    if (!ReduceSpCount)
//      for (c=0; c<J.NConns(); c++)
//        {
//        if (IO_Out(J.IONo(c)))
//          {
//          CMSAlpha & MSA=MSAlphas[iDst++];
//          for (s=0; s<SDB.No(); s++)
//            //if (MSA.m_MAlpha[s]>=0.0)
//            if (!Exists[s])
//              MSA.M_EST.m_MAlpha[s]+=(1.0-AMTot[s])/nMDstIOs;
//          }
//        }
    }

  SetCI(19, !fBalOK);
  flag InLineAlpha=(nDstIOs==1 && nDstIOs==1 && Joins.GetSize()==1);
  }

//--------------------------------------------------------------------------

void FlwNode::MergeMsAGDatas()
  {
  if (!DoMergeAGDatas)
    return;

  for (int i=0; i<MsAGDatas.GetSize(); i++)
    {
    CMsAGData * pMrgdAG=NULL;
    MsAGDatas[i].Merge(NULL, pMrgdAG);
    }
  };

//--------------------------------------------------------------------------

void FlwNode::MarkMsIndices()
  {
  for (int i=0; i<MsAGDatas.GetSize(); i++)
    MsAGDatas[i].MarkMsIndices();
  };

//--------------------------------------------------------------------------

void FlwNode::SetMsIndices(CMSIndexInit &Index)
  {
  for (int a=0; a<MsAGDatas.GetSize(); a++)
    {
    #if dbgFlwNodeMS
    if (dbgMSSetIndices())
      dbgpln("SetMsIndices %s[%i]", FullObjTag(),a);
    #endif
    CMsAGData & MSA=MsAGDatas[a];
    MSA.m_DstIO.CheckIndex(Index);
    }
  };

//--------------------------------------------------------------------------

void FlwNode::DumpMsAGDatas()
  {
  int HeadDone=false;
  for (int jn=0; jn<Joins.GetSize(); jn++)
    for (int i=0;i<MsAGDatas.GetSize(); i++)
      if (MsAGDatas[i].m_iJoinNo==jn)
        {
        CMsAGData & MSA=MsAGDatas[i];
        if (!HeadDone)
          dbgpln("---------------------------------------------------------------------------------");
        HeadDone=true;
        if (MSA.m_pMrgAGData)
          MSA.m_pMrgAGData->Dump(Tag(), -1);
        MSA.Dump(Tag(), jn);
        }
  };

//--------------------------------------------------------------------------

void FlwNode::AddMsEqns(SparseSolver & SS, int & Eqn)
  {
  for (int i=0; i<MsAGDatas.GetSize(); i++)
    MsAGDatas[i].AddMsEqns(SS, Eqn);
  };

//--------------------------------------------------------------------------

void FlwNode::AdjustMsFlows(SparseSolver & SS)
  {
  static double Filt=0.5;
  for (int io=0; io<NoIOs(); io++)
    if (IO_Out(io))
      {

      // if no Tear return
      //if (!IOConduitIn(0))
      //  return;

      CMSIndex & MSI=*IOMSIndex(io);
      if (MSI.Index()>=0)
        {
#if DOPB_FILT_RESULTS
        CFlange * pF=IOFlange(io);
        int n=pF->nMSRqd;
        if (n==0)
          {
          for (int i=0; i<MaxMSRqd; i++)
            *IOMSRqd(io, i)=IOConduit(io)->MArray();
          }
        else
          {
          for (int i=MaxMSRqd-1; i>0; i--)
            *IOMSRqd(io, i)=*IOMSRqd(io, i-1);
          }
        pF->SetNMSRqd(Min(MaxMSRqd, pF->NMSRqd()+1));
#endif

        SpMArray ReqdM;
        //SpVector &MRqd0=*IOMSRqd(io, 0);
        for (int s=0; s<SDB.No(); s++)
          {
          double M;
          if (MSI.SpIndex()[s]>0)
            M=SS.Solution(MSI.SpIndex()[s]);
          else
            M=0.0;
          if (M<0.0)
            {
            M=0.0;
            // Message ???
            }
#if DOPB_FILT_RESULTS
          (*IOMSRqd(io, 0))[s]=M;
          if (0)
            {
            M=0;
            for (int i=0; i<MaxMSRqd; i++)
              M+=(*IOMSRqd(io, i))[s];
            M/=MaxMSRqd;
            }
#endif
          ReqdM[s]=M;
          }

        rSpConduit Fo=*IOConduit(io);

        double xxx=Fo.QMass();
        double yyy=0;
        double T, P;
        #if dbgFlwNodeMS
        if (dbgMSDumpResults())
          dbgpln("%s >> %s",FullObjTag(), Nd_Rmt(io)->FullObjTag());
        #endif
        for (s=0; s<SDB.No(); s++)
          {
          #if dbgFlwNodeMS
          if (dbgMSDumpResults())
            if (/*i==0 &&*/ (IOConduit(io)->Qm(s)>0.0 || ReqdM[s]>0.0))
              {
              double e1=(ReqdM[s]-IOConduit(io)->Qm(s))/
                          Max(fabs(ReqdM[s]), fabs(IOConduit(io)->Qm(s)));
              dbgpln("  [%4i]  %16.8f >> %16.8f :%+16.8f   %10.6f%%  %10.6f%%  %s",
                    MSI.SpIndex()[s],
                    IOConduit(io)->Qm(s), ReqdM[s],
                    ReqdM[s]-IOConduit(io)->Qm(s),
                    100.0*e1, 100.0*e1*ReqdM[s]/GTZ(ReqdM.Sum()),
                    SDB[s].SymOrTag());
              }
          #endif
          }

        if (IOConduitIO(io))
          {
          T=IOConduitIO(io)->Temp();
          P=IOConduitIO(io)->Press();
          for (int s=0; s<SDB.No(); s++)
            IOConduitIO(io)->SetQm(s, ReqdM[s]);
          IOConduitIO(io)->SetTempPress(T, P);
          #if DOPBHSMOOTHING
          IOConduitIO(io)->Set_totCpEnergy(ReqdH);
          #endif
          }
        if (IOConduitIn(io))
          {
          T=IOConduitIn(io)->Temp();
          P=IOConduitIn(io)->Press();
          for (int s=0; s<SDB.No(); s++)
            IOConduitIn(io)->SetQm(s, ReqdM[s]);
          IOConduitIn(io)->SetTempPress(T, P);
          #if DOPBHSMOOTHING
          IOConduitIn(io)->Set_totCpEnergy(ReqdH);
          #endif
          }
        }
      }
  };

//--------------------------------------------------------------------------

void FlwNode::UpdateMsSoln(SparseSolver & SSPB)
  {
  };

#endif

//--------------------------------------------------------------------------

dword FlwNode::ModelStatus()
  {
  dword Ret=FNS_OK;

  if (CountAllCIs(1, LogFlag_Error)>0)
    Ret=FNS_Error;
  else if (CountAllCIs(1, LogFlag_Warning)>0)
    Ret=FNS_Warning;

  if (fMarked)
    Ret|=FNS_Marked;
  if (!GetActive())
    Ret|=FNS_InActive;
  if (GetActiveHold())
    Ret|=FNS_Hold;
  if (fActiveInMacroMdl)
    Ret|=FNS_MacMdl;
  return Ret;
  };

//--------------------------------------------------------------------------

SpConduit * FlwNode::GetWrkConduit(char *Tag, char *IdStr)
  {
  SpConduit * p;
  if (!WrkConduits.Lookup(IdStr, p))
    {
    p=new SpConduit(Tag);
    WrkConduits.SetAt(IdStr,p);
    }
  p->QZero();
  return p;
  };

//--------------------------------------------------------------------------

SpContainer * FlwNode::GetWrkContainer(char *Tag, char *IdStr)
  {
  SpContainer * p;
  if (!WrkContainers.Lookup(IdStr, p))
    {
    p=new SpContainer(Tag);
    WrkContainers.SetAt(IdStr,p);
    }
  p->ZeroMass();
  p->ZeroDeriv();
  return p;
  };

//--------------------------------------------------------------------------

int FlwNode::GetNodeWiring(CNodeWiring &NodeWiring)
  {
  UpdateElectrics();

  NodeWiring.BuildWiring(FullObjTag(), m_Electrics);

  for (int e=EIO1(); e<EION(); e++)
    NodeWiring.AddCable(Nd_Rmt(e)->FullObjTag());

  return 0;
  }

//--------------------------------------------------------------------------

int FlwNode::UpdateElectrics()
  {
  if (1)
    {
    CNodeElectricsArray TSCA;
    TSCA.InitialiseCollection();
    CollectElectrics(TSCA);
    ConnectElectrics();
    }
  return 0;
  }

//==========================================================================
//
//CNodeXRefMngr::CNodeXRefMngr(CTNode* Nd) : m_pNd(Nd)  {};
//CNodeXRefMngr::~CNodeXRefMngr()                     {};

//---------------------------------------------------------------------------

LPSTR FlwNode::GetOwnerTag()    { return FullObjTag(); };
bool  FlwNode::GetOwnerDbgBrk() { return DoDbgBrk()!=0; };

//---------------------------------------------------------------------------

int FlwNode::ResolveNearXRef(CXRefItem * XRef, flag MustBeParm)
  {
  return XRef->ResolveNearXRef(FamilyHead(), XRef->RefTag(), this);
  }

//---------------------------------------------------------------------------

int FlwNode::ResolveFarXRef(CXRefItem * XRef)
  {
  return XRef->ResolveFarXRef(ExecObj(), XRef->RefTag(), this);
  }

//---------------------------------------------------------------------------

int FlwNode::ResolveIORef(int IOId, CTagRef * & pRef)
  {

  Strng ID, Tg;
  bool IsIn, IsOut;
  int Inx=GetCIONo(IOId, ID, Tg, IsIn, IsOut);
  if (Inx>=0)
    {
    pRef = new CTagRef;
    pRef->m_pOwner = this;//Owner;
    pRef->m_iIOId=IOId;
    pRef->m_iIOInx=Inx;
    pRef->m_bMustGet=IsOut;
    pRef->m_bMustSet=IsIn;
    pRef->m_sRmtID=ID;
    pRef->m_sRmtTag=Tg;
    pRef->m_iType = tt_Double;
    pRef->m_iCnvIndex = 0;
    //m_bNdPtrsValid=true;
    //m_eReach=XR_IO;
    return FXR_Found;
    }
  return FXR_NotFound;
  }

//--------------------------------------------------------------------------

bool FlwNode::TestXRefListActive()
  {
#if (WITHNODEPROCS && WITHNODETAGIO)
  return SetXRefListActive(!GetActiveHold() && (ProcsOn(NPM_Any) && m_pProcs->On() || TagIOOn()));
#elif (WITHNODEPROCS)
  return SetXRefListActive(!GetActiveHold() && (ProcsOn(NPM_Any) && m_pProcs->On())); 
#elif (WITHNODETAGIO)
  return SetXRefListActive(!GetActiveHold() && TagIOActive());
#else
  return false;
#endif
  }

//--------------------------------------------------------------------------

int FlwNode::UpdateXRefLists(CXRefBuildResults & Results) 
  {
#if WITHNODEPROCS
  if (ProcsOn(NPM_Any))
    m_pProcs->m_ProcMngr.UpdateXRefLists(Results);
#endif
#if WITHNODETAGIO
  if (TagIOActive())
    m_pTagIO->UpdateXRefLists(Results);
#endif
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void FlwNode::UnlinkAllXRefs()
  {
#if WITHNODEPROCS
  if (ProcsOn(NPM_Any))
    m_pProcs->m_ProcMngr.UnlinkAllXRefs();
#endif
#if WITHNODETAGIO
  if (TagIOActive())
    {
    m_pTagIO->UnlinkAllXRefs();
    }
#endif
  //PgmMngr.UnlinkAllXRefs();
  CNodeXRefMngr::UnlinkAllXRefs();
  };

//--------------------------------------------------------------------------

#if (WITHNODEPROCS || WITHNODETAGIO)

void FlwNode::BeforeNode(long ProcIDMask)
  {
#if WITHNODEPROCS
  if (ProcIDMask && ProcsOn(ProcIDMask))
    m_pProcs->DoEvalProc(ProcIDMask, true, true);

#endif
 
#if WITHNODETAGIO
  if (TagIOActive())
    m_pTagIO->GetAllValues(true);
#endif
 }

#endif

//--------------------------------------------------------------------------

#if (WITHNODEPROCS || WITHNODETAGIO)

void FlwNode::AfterNode(long ProcIDMask)
  {
#if WITHNODETAGIO
  if (TagIOActive())
    m_pTagIO->SetAllValues(true);
#endif

#if WITHNODEPROCS
  if (ProcIDMask && ProcsOn(ProcIDMask))
    m_pProcs->DoEvalProc(ProcIDMask, true, true);
#endif
 
 }

#endif

//==========================================================================
//
//
//
//==========================================================================

void CMacroMdlBase::Activate(CMacroMdl* pMMNd, FlwNode * pNd)
  {
  m_fActive=1;
  m_pFNd=pNd;
  m_pMNd=pMMNd;
  m_pFNd->StructureChanged(m_pFNd);
  };

//--------------------------------------------------------------------------

void CMacroMdlBase::Deactivate()
  {
  m_fActive=0;
  m_pFNd->StructureChanged(m_pFNd);
  };

//==========================================================================
//
//
//
//==========================================================================

//IMPLEMENT_TAGOBJ(Obj, Grp, ClassId, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc)
IMPLEMENT_TAGOBJ(CMacroMdl, "CMacroMdl", "CMacroMdl", "", "", "FG", TOC_SYSTEM, "FlowNodeGrp", "Long Descriptor")

CMacroMdl::CMacroMdl(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, TagIn, pAttach, eAttach)
  {
  };

//--------------------------------------------------------------------------

CMacroMdl::~CMacroMdl()
  {
  for (int i=0; i<Nds.GetSize(); i++)
    Nds[i].m_pFNd->MacroMdlRemoveNd(Nds[i].m_iIONo);
  };

//--------------------------------------------------------------------------

void CMacroMdl::AddNd(FlwNode * pNd, int iIONo, CMacroMdlIO * pMMIO)
  {
  for (int i=0; i<Nds.GetSize();i++)
    if (Nds[i].m_pFNd==pNd)
      return;
  CMacroMdlNd F = {pNd, iIONo, pMMIO, pMMIO->m_pNdVarTypeId};
  Nds.Add(F);
  pNd->MacroMdlAddNd(iIONo);

  if (pMMIO->m_pNdVarTypeId)
    if (MMList.Length()>0)
      {
      MMList+="\n";
      MMList+=pNd->FullObjTag();
      }
    else
      MMList=pNd->FullObjTag();
  };

//--------------------------------------------------------------------------

CMacroMdlIO * CMacroMdl::ValidNd(flag fIsModel, FlwNode * pNd, int iIONo, CMacroMdlIOArray &MMIOs)
  {
  for (int i=0; i<MMIOs.GetSize(); i++)
    if (MMIOs[i].m_fIsModel==fIsModel)
      {
      if (pNd->Class()->TypeId()==MMIOs[i].m_pNdTypeId)
      //if (&typeid(*pNd)==MMIOs[i].m_pNdTypeId)
      //if (pNd->Class()==MMIOs[i].xm_pClass)
        if (iIONo<0)
          return &MMIOs[i];
        else if (pNd->IOId_Self(iIONo)==MMIOs[i].m_ioId)
          {
          if (pNd->MacroMdlValidNd(iIONo))
            return &MMIOs[i];
          else
            return NULL;
          }
      }
    return NULL;
  }

//--------------------------------------------------------------------------

void CMacroMdl::RegisterNode(CMacroMdlIOArray &MMIOs, const type_info * pNdType, int ioId, flag fIsModel, const type_info * pNdVarTypeId)
  {
  for (int i=0; i<MMIOs.GetSize(); i++)
    if (MMIOs[i].m_pNdTypeId)
      if (pNdType==MMIOs[i].m_pNdTypeId && ioId==MMIOs[i].m_ioId && fIsModel==MMIOs[i].m_fIsModel)
        return;
  CMacroMdlIO MIO={pNdType, ioId, fIsModel, pNdVarTypeId};
//  CMacroMdlIO MIO={pClass, pClass->ClassId(), ioId, fIsModel, pNdVarTypeId};
  MMIOs.Add(MIO);
  };

//--------------------------------------------------------------------------

//void CMacroMdl::RegisterNode(CMacroMdlIOArray &MMIOs, pTagObjClass pClass, int ioId, flag fIsModel, const type_info * pNdVarTypeId)
//  {
//  for (int i=0; i<MMIOs.GetSize(); i++)
//    if (MMIOs[i].xm_pClass)
//      if (pClass==MMIOs[i].xm_pClass && ioId==MMIOs[i].m_ioId && fIsModel==MMIOs[i].m_fIsModel)
//        return;
//  CMacroMdlIO MIO={pClass, pClass->ClassId(), ioId, fIsModel, pNdVarTypeId};
//  MMIOs.Add(MIO);
//  };
//
////--------------------------------------------------------------------------
//
//flag CMacroMdl::RegisterNode(CMacroMdlIOArray &MMIOs, char * pClassId, int ioId, flag fIsModel, const type_info * pNdVarTypeId)
//  {
//
//  for (int i=0; i<MMIOs.GetSize(); i++)
//    if ((_stricmp(pClassId, MMIOs[i].m_pClassId)==0) && ioId==MMIOs[i].m_ioId && fIsModel==MMIOs[i].m_fIsModel)
//      return True;
//
//  TagObjClass *pC=(TagObjClass*)TagObjClass::FindGroupId(FlwUnitGrp);
//  while (pC)
//    {
//    if (_stricmp(pClassId, pC->ClassId())==0)
//      break;
//    pC=pC->NextClassInGrp();
//    }
//
//  if (!pC)
//    {
//    pC=(TagObjClass*)TagObjClass::FindGroupId(FlwLinkGrp);
//    while (pC)
//      {
//      if (_stricmp(pClassId, pC->ClassId())==0)
//        break;
//      pC=pC->NextClassInGrp();
//      }
//    }
//
//  if (pC)
//    {
//    CMacroMdlIO MIO={pC, pC->ClassId(), ioId, fIsModel, pNdVarTypeId};
//    MMIOs.Add(MIO);
//    return True;
//    }
//  return False;
//  };

//==========================================================================
//
//
//
//==========================================================================

NodeDrawDef::NodeDrawDef()
  {
  pNdDrw=NULL;
  Len=NULL;
  };

//--------------------------------------------------------------------------

NodeDrawDef::~NodeDrawDef()
  {
  }

//--------------------------------------------------------------------------

NodeGrfInfo* NodeDrawDef::FindGrfInfo()
  {
  pNdDrw=new NodeGrfInfo;
  Len=0;

  NodeDrawEntry* p=NodeDrawEntry::pFirst;
  while (p)
    {
    AppendDrawing(p->ClassName(), p->DrwGroup(), p->Drawing());
    p=p->Next();
    }

  if (pNdDrw)
    pNdDrw->PutValue(DD_EndList);
  return pNdDrw;
  }

//--------------------------------------------------------------------------

long NodeDrawDef::AppendDrawing(char* ClassName, char* DrwGroup, double * Drawing)
  {
  if (Drawing!=NULL)
    {
    Strng FullGrp(DrwGroup);
    FullGrp+=":";
    FullGrp+=ClassName;

    // dbgpln("NodeDrawDef::AppendDrawing %s", FullGrp());

    pNdDrw->PutValue(DD_Name);
    pNdDrw->PutPChar(FullGrp());
    double * s=Drawing;
    while (*s!=DD_End)
      switch ((long)*s)
        {
        case DD_In:
        case DD_Out:
        case DD_Name:
          pNdDrw->PutValue(*s);
          pNdDrw->PutPChar((char*)(long)*(s+1));
          s+=2;
          break;
        default :
          pNdDrw->PutValue(*s);
          s++;
          break;
        }
    pNdDrw->PutValue(DD_End);
    }

  return Len;
  }

//==========================================================================

NodeDrawEntry* NodeDrawEntry::pFirst=NULL;

NodeDrawEntry::NodeDrawEntry(char* pClassName, char* pVersion, char * pDrwGroup, double * pDrawing)
  {
  pNext=pFirst;
  pFirst=this;
  m_ClassName=pClassName;
  if (pVersion && (strlen(pVersion)>0))
    {
    m_ClassName+="-";
    m_ClassName+=pVersion;
    }
  m_DrwGroup=pDrwGroup;
  m_pDrawing=pDrawing;
  //dbgpln("NodeDrawEntry::Create:%s", m_Name());
  }

//--------------------------------------------------------------------------

NodeDrawEntry::~NodeDrawEntry()
  {
  if (pFirst==this)
    pFirst=pNext;
  else
    {
    NodeDrawEntry* p=NodeDrawEntry::pFirst;
    while (p && p->pNext!=this)
      p=p->Next();
    if (p && p->pNext==this)
      p->pNext=pNext;
    }
  //dbgpln("NodeDrawEntry::Delete:%s", m_Name());
  }

//==========================================================================
//
//
//
//==========================================================================

CAreaSum::eSumAction CAreaSum::sm_eAction=CAreaSum::SumMass;

CAreaSum::CAreaSum()
  {
  m_FlowHf        = 0;
  m_FlowMass      = 0;
  m_pFlowArray    = NULL;
  m_TotalMass     = 0;
  m_TotalMassInc  = 0;
  m_TotalHf       = 0;
  m_TotalHfInc    = 0;
  m_pTotalArray   = NULL;
  };

CAreaSum::~CAreaSum()
  {
  delete m_pFlowArray;
  delete m_pTotalArray;
  };

//--------------------------------------------------------------------------

void CAreaSum::ZeroFlows()
  {
  m_FlowMass = 0;
  m_FlowHf = 0;
  if (DoArray)
    FlowArray.Zero();
  };

void CAreaSum::ZeroTotals()
  {
  m_TotalMass = 0;
  m_TotalMassInc = 0;
  m_TotalHf = 0;
  m_TotalHfInc = 0;
  if (DoArray)
    TotalArray.Zero();
  };

void CAreaSum::ZeroTotalIncs()
  {
  m_TotalMassInc = 0;
  m_TotalHfInc = 0;
  };

//--------------------------------------------------------------------------

void CAreaSum::Set(CAreaSum & C)
  {
  m_FlowMass = C.FlowMass;
  m_FlowHf = C.FlowHf;
  if (DoArray)
    FlowArray.Set(C.FlowArray);

  m_TotalMass     = C.TotalMass;
  m_TotalMassInc  = C.TotalMassInc;
  m_TotalHf       = C.TotalHf;
  m_TotalHfInc    = C.TotalHfInc;
  if (DoArray)
    TotalArray.Set(C.TotalArray);
  };

void CAreaSum::Set(SpModel & C)
  {
  m_FlowMass = C.Mass();
  m_FlowHf = C.totHf();
  if (DoArray)
    FlowArray.Set(C.MArray());
  };

void CAreaSum::Set(SpQueue & Q)
  {
  m_FlowMass=0;
  m_FlowHf=0;
  if (DoArray)
    FlowArray.Zero();
  Q.AddTotals(m_FlowMass, m_FlowHf, DoArray ? &FlowArray : NULL);
  };

void CAreaSum::Set(CSysVector & V, double totHz)
  {
  m_FlowMass = V.Mass();
  m_FlowHf = FlowHf;
  if (DoArray)
    FlowArray.Set(V);
  };

//--------------------------------------------------------------------------

void CAreaSum::Add(CAreaSum & C)
  {
  m_FlowMass += C.FlowMass;
  m_FlowHf += C.FlowHf;
  if (DoArray)
    FlowArray.Add(C.FlowArray, som_ALL);

  m_TotalMass += C.TotalMassInc;
  m_TotalHf += C.TotalHfInc;
  if (DoArray)
    TotalArray.Add(C.TotalArray, som_ALL);
  };

void CAreaSum::Add(SpModel & C)
  {
  m_FlowMass += C.Mass();
  m_FlowHf += C.totHf();
  if (DoArray)
    FlowArray.Add(C.MArray(), som_ALL);
  };

void CAreaSum::Add(SpQueue & Q)
  {
  Q.AddTotals(m_FlowMass, m_FlowHf, DoArray ? &FlowArray : NULL);
  };

void CAreaSum::Add(CSysVector & V, double FlowHf)
  {
  m_FlowMass += V.Mass();
  m_FlowHf += FlowHf;
  if (DoArray)
    FlowArray.Add(V, som_ALL);
  };

//--------------------------------------------------------------------------

void CAreaSum::Integrate(double TimeInc)
  {
  m_TotalMassInc=m_FlowMass*TimeInc;
  m_TotalMass+=m_TotalMassInc;
  m_TotalHfInc=m_FlowHf*TimeInc;
  m_TotalHf+=m_TotalHfInc;
  if (DoArray)
    {
    INCOMPLETECODE(__FILE__, __LINE__)
    }
  };

//==========================================================================
//
//
//
//==========================================================================

CDirectAccCalc::CDirectAccCalc(FlwNode * pNd, CDirectFlwIO & In, CDirectFlwIO & Out) :
  m_Nd(*pNd),
  m_In(In),
  m_Out(Out)
  {
  m_LastInstance=-1;
  };

CDirectAccCalc::~CDirectAccCalc()
  {
  };

void CDirectAccCalc::Start(int Instance, SpContainer & Cn)
  {
  if (Instance==0)
    {
    m_In.Sum.ZeroFlows();
    m_Out.Sum.ZeroFlows();
    }
  m_LastInstance=Instance;
  m_In.Sum.Add(Cn);
  };

void CDirectAccCalc::Start(int Instance, SpQueue & Q)
  {
  if (Instance==0)
    {
    m_In.Sum.ZeroFlows();
    m_Out.Sum.ZeroFlows();
    }
  m_LastInstance=Instance;
  m_In.Sum.Add(Q);
  };

void CDirectAccCalc::Start(int Instance, CSysVector & V, double totHz)
  {
  if (Instance==0)
    {
    m_In.Sum.ZeroFlows();
    m_Out.Sum.ZeroFlows();
    }
  m_LastInstance=Instance;
  m_In.Sum.Add(V, totHz);

  #if dbgFlwNode
  if (dbgAccCalcs())
    {
    dbgpln("Start    %3i %20s %20s %20s %20s %s", Instance,
      DbgFltString(m_In.Sum.FlowMass,7,6),
      DbgFltString(V.Mass(),7,6),
      "","",
      m_Nd.FullObjTag());
    }
  #endif
  };

void CDirectAccCalc::Complete(int Instance, SpContainer & Cn)
  {
  m_Out.Sum.Add(Cn);
  #if dbgFlwNode
  if (dbgAccCalcs())
    {
    dbgpln("Complete %3i %20s %20s %20s %20s %s", Instance,
      DbgFltString(m_Out.Sum.FlowMass,7,10),
      DbgFltString(Cn.Mass(),7,10),
      "","",
      m_Nd.FullObjTag());
    }
  #endif
  DoComplete(Instance);
  };

void CDirectAccCalc::Complete(int Instance, SpQueue & Q)
  {
  m_Out.Sum.Add(Q);
  #if dbgFlwNode
  if (dbgAccCalcs())
    {
    dbgpln("Complete %3i %20s %20s %20s %20s %s", Instance,
      DbgFltString(m_Out.Sum.FlowMass,7,6),
      DbgFltString(Q.TotalMass(),7,6),
      "","",
      m_Nd.FullObjTag());
    }
  #endif
  DoComplete(Instance);
  };

void CDirectAccCalc::Complete(int Instance, CSysVector & V, double totHz)
  {
  m_Out.Sum.Add(V, totHz);
  #if dbgFlwNode
  if (dbgAccCalcs())
    {
    dbgpln("Complete %3i %20s %20s %20s %20s %s", Instance,
      DbgFltString(m_Out.Sum.FlowMass,7,6),
      DbgFltString(V.Mass(),7,6),
      "","",
      m_Nd.FullObjTag());
    }
  #endif
  DoComplete(Instance);
  };

void CDirectAccCalc::DoComplete(int Instance)
  {
  if (Instance<m_LastInstance)
    return;

  if (CAreaSum::sm_eAction==CAreaSum::SumArray)
    {
    INCOMPLETECODE(__FILE__, __LINE__);
    }
  else
    {
    double TimeInc=GTZ(ICGetTimeInc());
    double dM=(m_Out.Sum.FlowMass-m_In.Sum.FlowMass)/TimeInc;
    if (dM>=0)
      {
      m_In.Sum.FlowMass=dM;
      m_Out.Sum.FlowMass=0;
      }
    else
      {
      m_In.Sum.FlowMass=0;
      m_Out.Sum.FlowMass=-dM;
      }

    double dH=(m_Out.Sum.FlowHf-m_In.Sum.FlowHf)/TimeInc;
    if (dH>=0)
      {
      m_In.Sum.FlowHf=dH;
      m_Out.Sum.FlowHf=0;
      }
    else
      {
      m_In.Sum.FlowHf=0;
      m_Out.Sum.FlowHf=-dH;
      }
    }

  #if dbgFlwNode
  if (dbgAccCalcs())
    {
    dbgpln("AccCalc::Complete === %20s %20s %20s %20s %s",
      DbgFltString(m_In.Sum.FlowMass,7,6),
      DbgFltString(m_Out.Sum.FlowMass,7,6),
      DbgFltString(m_In.Sum.FlowHf),
      DbgFltString(m_Out.Sum.FlowHf),
      m_Nd.FullObjTag());
    }
  #endif
  };

//==========================================================================
//
//
//
//===========================================================================

CExternAuditCalc::CExternAuditCalc(FlwNode * pNd) :
  m_Nd(*pNd)
  {
  m_LastInstance=-1;
  ZeroFlows();
  //m_Mass=0;
  //m_Heat=0;
  };

CExternAuditCalc::~CExternAuditCalc()
  {
  };

void CExternAuditCalc::Complete(int Instance, CReactionBase * pRB, double ExtMassIn, double ExtPwrIn)
  {
  if (Instance==0)
    {
    ZeroFlows();
    //m_Mass=0;
    //m_Heat=0;
    }

  if (Instance<m_LastInstance)
    return;

  m_LastInstance=Instance;

  double TimeInc=GTZ(ICGetTimeInc());
  if (pRB && pRB->OnAndOK())
    {
    m_FlowMass+=pRB->MsSumSrc()->Mass()/TimeInc;
    m_FlowMass-=pRB->MsSumSnk()->Mass()/TimeInc;

    m_FlowHf+=pRB->HsSumPwr()/TimeInc;
    m_FlowHf+=pRB->HsSumHX()/TimeInc;
    }

  m_FlowMass += ExtMassIn;
  m_FlowHf   += ExtPwrIn;

  #if dbgFlwNode
  if (dbgAccCalcs())
    {
    dbgpln("AdtCalc::Complete === %20s %20s %20s %20s %s",
      DbgFltString(FlowMass,7,6),
      DbgFltString(FlowMass,7,6),
      DbgFltString(FlowHf),
      DbgFltString(FlowHf),
      m_Nd.FullObjTag());
    }
  #endif
  };

//==========================================================================
//
//
//
//===========================================================================

Strng gs_DirectFlwIOTagDefault(PlantAreaTag);
//Strng_List gs_DirectFlwIOTagsMissing;

//===========================================================================

eDIOConnectSelects CDirectFlwIO::sm_ConnectVentSpills  = eDIOSlct_All;
eDIOConnectSelects CDirectFlwIO::sm_ConnectLeaks       = eDIOSlct_All;
eDIOConnectSelects CDirectFlwIO::sm_ConnectAudit       = eDIOSlct_All;

CDirectFlwIO::CDirectFlwIO(eDirectIO eType, FlwNode* Nd, flag ShowUsr, flag SysEnable, LPCTSTR Tag, int LclDIOId, int RmtDIOId, LPCTSTR RmtTypeName, LPCTSTR RmtTagDef) :
  m_FEP(NULL, FBDDOpt_WithAll, 0.0)
  {
  m_sRmtTypeName=RmtTypeName?RmtTypeName:"Area";
  m_sRmtTagDef=RmtTagDef;
  m_NdSignImp=0;
  m_RmtListPos=NULL;
  m_EvalFlowEquationsReturn=false;
  Initialise(eType, Nd);
  InitialiseIO(ShowUsr, SysEnable, Tag, LclDIOId, RmtDIOId, -1);
  };

CDirectFlwIO::CDirectFlwIO(eDirectIO eType, FlwNode* Nd, flag ShowUsr, flag SysEnable, LPCTSTR Tag, LPCTSTR RmtTypeName, LPCTSTR RmtTagDef) :
  m_FEP(NULL, FBDDOpt_WithAll, 0.0)
  {
  m_sRmtTypeName=RmtTypeName?RmtTypeName:"Area";
  m_sRmtTagDef=RmtTagDef;
  m_NdSignImp=0;
  m_RmtListPos=NULL;
  m_EvalFlowEquationsReturn=false;
  Initialise(eType, Nd);

  InitialiseIO(ShowUsr, SysEnable, Tag, -1, -1, -1);
  //InitialiseFlags(ShowUsr, SysEnable, Tag);
  };

//--------------------------------------------------------------------------

CDirectFlwIO::~CDirectFlwIO()
  {
  delete m_pFlng;
  if (m_bDeleteSum)
    m_pSum;
  ManageNdDirectIOs(false);
  };

//--------------------------------------------------------------------------

static LPCTSTR eTypeStr(eDirectIO eType)
  {
  switch (eType)
    {
    case eDIO_Spill:       return "Spill";
    case eDIO_Vent:        return "Vent";
    case eDIO_Src:         return "Source";
    case eDIO_Makeup:      return "Makeup";
    case eDIO_Deplete:     return "Deplete";
    case eDIO_Snk:         return "Sink";
    case eDIO_Bleed:       return "Bleed";
    case eDIO_Accum:       return "Accum";
    default:               return "???";
    }
  }

//--------------------------------------------------------------------------

void CDirectFlwIO::Initialise(eDirectIO eType, FlwNode* Nd)
  {
  m_pNd         = Nd;
  m_sTag        = "";

  m_eType       = eType;
  m_bIsXfer     = (eType != eDIO_Leak);
  m_bSysEnable  = m_bIsXfer;

  switch (m_eType)
    {
    case eDIO_Spill:
    case eDIO_Vent:
      m_NdSignImp=-1;
      break;
    case eDIO_Src:
    case eDIO_Makeup:
    case eDIO_Deplete:
      m_NdSignImp=+1;
      break;
    case eDIO_Snk:
    case eDIO_Bleed:
    case eDIO_Accum:
      m_NdSignImp=-1;
      break;
    default:
      break;
    }

  m_bUsrEnable      = false;
  m_bMLnkConnd      = false;
  m_bShowUsr        = false;

  m_sRmtTag         = "";
  m_bConnected      = false;
  m_iMyConnectedIO  = -1;
  m_Opening         = 1.0;
  m_iLclDIOId       = -1;
  m_iRmtDIOId       = -1;
  m_iLclIOId        = -1;

  m_pFlng           = NULL;
  m_pSum            = NULL;
  m_bDeleteSum      = false;

  if (Enabled)
    ManageNdDirectIOs(true);
  };

//--------------------------------------------------------------------------

#define dbgDIOList 0

void CDirectFlwIO::ManageNdDirectIOs(bool AddIt)
  {
  if (m_pNd)
    {
    bool Found=false;
    for (int dPos=0; dPos<m_pNd->m_AttachedDirectIOs.GetCount(); dPos++)
      {
      if (m_pNd->m_AttachedDirectIOs[dPos]==this)
        {
        Found=true;
        break;
        }
      }
    if (AddIt)
      {
      if (!Found)
        {
        bool Done=false;
        for (int d=0; !Done && d<m_pNd->m_AttachedDirectIOs.GetCount(); d++)
          {
          CDirectFlwIO *pD=m_pNd->m_AttachedDirectIOs[d];
          if (pD->m_eType>m_eType)
            {
            m_pNd->m_AttachedDirectIOs.InsertAt(d, this);
            Done=true;
            if (dbgDIOList)
              dbgpln("DIO Insert %-10s %08x @   %3i", eTypeStr(m_eType), this, d);
            }
          }
        if (!Done)
          {
          m_pNd->m_AttachedDirectIOs.Add(this);
          if (dbgDIOList)
            dbgpln("DIO Insert %-10s %08x End %3i", eTypeStr(m_eType), this, d);
          }
        }
      else
        {
        if (dbgDIOList)
          dbgpln("DIO Insert %-10s %08x Found", eTypeStr(m_eType), this);
        }
      }
    else
      {
      if (Found)
        {
        m_pNd->m_AttachedDirectIOs.RemoveAt(dPos);
        if (dbgDIOList)
          dbgpln("DIO Remove %-10s %08x ", eTypeStr(m_eType), this);
        }
      else
        {
        if (dbgDIOList)
          dbgpln("DIO Remove %-10s %08x NOT FOUND", eTypeStr(m_eType), this);
        }
      }
    }
  };

//--------------------------------------------------------------------------

void CDirectFlwIO::SetLclIOId(int LclIOId)
  {
  m_iLclIOId = LclIOId;
  };

void CDirectFlwIO::InitialiseIO(flag ShowUsr, flag SysEnable, LPCTSTR Tag, int LclDIOId, int RmtDIOId, int LclIOId)
  {
  m_sTag          = Tag;
  m_bSysEnable    = SysEnable;
  m_bShowUsr      = ShowUsr;

  ASSERT_ALWAYS(!m_bConnected, "CDirectFlwIO Still Connected", __FILE__, __LINE__);
  m_iMyConnectedIO  = -1;
  m_Opening         = 1.0;
  m_iLclDIOId       = LclDIOId;
  m_iRmtDIOId       = RmtDIOId;
  m_iLclIOId        = LclIOId;

  //Strng FlngTag;
  //switch (m_eType)
  //  {
  //  case eDIO_Spill:  FlngTag.Set("Spl%i", LclDIOId-IOId_Spill2Area+1); break;
  //  case eDIO_Vent:   FlngTag.Set("Vnt%i", LclDIOId-IOId_Vent2Area+1);  break;
  //  case eDIO_Makeup: FlngTag.Set("MU%i", LclDIOId-IOId_Makeup2Area+1); break;
  //  case eDIO_Bleed:  FlngTag.Set("BL%i", LclDIOId-IOId_Bleed2Area+1);  break;
  //  case eDIO_Leak:   FlngTag.Set("Lk%i", LclDIOId-IOId_LeakI2Area+1);  break;
  //  default :         FlngTag.Set("XX");                                break;
  //  }

  if (HasConduit && (m_pFlng == NULL))
    {
    m_pFlng                 = (CFlange*)CFlangeClass.Construct("Flange" ,""/*()*/ ,NULL , TOA_Free);
    m_pFlng->m_bMustDelete  = false;
    m_pFlng->InitConnId(m_pNd, NULL, NULL, NULL, m_sTag());
    //m_pFlng->m_pRefNd       = m_pNd;
    //m_pFlng->m_sRefIOTg     = FlngTag;
    m_pFlng->FixTag();

    m_pSum                  = &m_pFlng->m_Sum;
    m_bDeleteSum            = false;
    }
  else
    {
    m_pFlng         = NULL;
    m_pSum          = new CAreaSum;
    m_bDeleteSum    = true;
    }

  if (Enabled)
    ManageNdDirectIOs(true);
  };

//--------------------------------------------------------------------------

void CDirectFlwIO::Add_OnOff(DataDefnBlk &DDB, dword Flags, dword UserInfo)
  {
  if (m_bShowUsr)
    {
    Strng Tag(m_sTag);
    Tag+="s";
    DDB.PushUserInfo(UserInfo);
    DDB.CheckBoxBtn(Tag(), "",  DC_, "", xidDrctEnable, m_pNd,  isParmStopped, DDBOnOff);
    DDB.PopUserInfo();
    }
  }

//--------------------------------------------------------------------------

void CDirectFlwIO::BuildDataDefn(DataDefnBlk & DDB, int Type/*, LPCTSTR TagPrefix*/, LPSTR Tag, DDBPages PgOpt, DWORD UserInfo, DWORD Options)
  {
  if (!DDB.ForFileSnpScn() && !Enabled)
    return;

  flag DoIt=false;
  switch (Type)
    {
    case tt_Struct:
      DoIt=DDB.BeginStruct(m_pNd, Tag?Tag:m_sTag(), NULL, PgOpt, UserInfo);
      break;
    case tt_Element:
      DoIt=DDB.BeginElement(m_pNd, Tag?Tag:m_sTag(), NULL, UserInfo, PgOpt);
      break;
    case tt_Object:
      DoIt=DDB.BeginObject(m_pNd, Tag ? Tag:m_sTag(), "DirectedIO", NULL, PgOpt, UserInfo);
      break;
    default:
      _asm int 3;
      break;
    }

  if (DoIt)
    {
    DDB.String  (m_sRmtTypeName(),     "",       DC_,      "",       xidRmtTag,       m_pNd, isParmStopped|isTag);

    static DDBValueLst DDBConnected [] =
      {
        {TOD_OK          , "OK"             },
        {TOD_NoTag       , "No Tag"         },
        {TOD_BadTag      , "Bad Tag"        },
        {TOD_Inactive    , "Inactive Node"  },
        {TOD_BadConn     , "Missing IO"     },
        {TOD_Disconected , "Disconnected"   },
        {0}
      };

    DDB.String  ("Flange",             "",       DC_,      "",   xidDIOFlangeTag,    m_pNd, (DDB.ForFileSnpScn()?isParm:0/*isResult*/)|isTag/*|hasNoGrf*/);
    if (!DDB.ForFileSnpScn())
      {
      if (Options&DFIO_ShowSrcTag)
        DDB.String  ("SrcTag",            "",       DC_,      "",   xidDIOFlngSrcTag,   m_pNd, isResult|isTag);
      if (Options&DFIO_ShowDstTag)
        DDB.String  ("DstTag",            "",       DC_,      "",   xidDIOFlngDstTag,   m_pNd, isResult|isTag);
      DDB.Byte    ("Connect",            "",       DC_,      "",   xidConnectState,    m_pNd, 0, DDBConnected);
      }
    if (!m_bIsXfer)
      DDB.Double  ("Opening",   "",       DC_Frac,  "%",      &m_Opening,        m_pNd, isParm);

    if (Options&DFIO_ShowQm)
      DDB.Double  ("MassFlowOut", "QmOut",  DC_Qm,    "kg/s",  xidDrctQm,         m_pNd, isResult);

    DDB.Text("");
    if (!m_bIsXfer)
      m_FEP.BuildDataDefn(FBDDOpt_WithAll, DDB, m_pNd, NULL, UserInfo);
    }

  switch (Type)
    {
    case tt_Struct:
      DDB.EndStruct();
      break;
    case tt_Element:
      break;
    case tt_Object:
      DDB.EndObject();
      break;
    }
  };

//--------------------------------------------------------------------------

void CDirectFlwIO::SetTarget(LPCTSTR T)
  {
  if (T[0]!='<')
    m_sRmtTag=T;
  else
    m_sRmtTag="";
  m_sRmtTag.LRTrim();
  };

LPCTSTR CDirectFlwIO::Target()
  {
  return (LPCTSTR)m_sRmtTag();
  };
LPCTSTR CDirectFlwIO::TargetStr(flag ForFiling)
  {
  if (m_sRmtTag.GetLength()>0)
    m_sRmtTmp=m_sRmtTag;
  else if (m_sRmtTagDef.GetLength()>0)
    m_sRmtTmp=m_sRmtTagDef;
  else if (ForFiling)//DCB.ForFileSnpScn())
    m_sRmtTmp="";
  else if (m_pNd->m_sAreaTag.GetLength()>0)
    m_sRmtTmp.Set("<%s>", m_pNd->m_sAreaTag());
  else
    m_sRmtTmp.Set("<%s>", gs_DirectFlwIOTagDefault());
  return (LPCTSTR)m_sRmtTmp();
  };

flag CDirectFlwIO::DataXchg(DataChangeBlk & DCB)
  {
  if (/*DCB.dwUserInfo==1 &&*/ m_FEP.DataXchg(DCB))
    return 1;
  switch (DCB.lHandle)
    {
    case xidDrctEnable:
      {
      if (DCB.rB)
        {
        UsrEnable=*DCB.rB;
        }
      DCB.B=m_bUsrEnable;
      return 1;
      }

    case xidRmtTag:
      if (DCB.rpC)
        SetTarget(DCB.rpC);
      DCB.pC=(LPTSTR)TargetStr(DCB.ForFileSnpScn()!=0);
      return 1;

    case xidDIOFlangeTag:
      if (DCB.rpC && strlen(DCB.rpC)>0)
        {
        if (m_pFlng)
          m_pFlng->SetTag(DCB.rpC);
        else
          m_sConnTagMem = DCB.rpC;
        }

      DCB.pC=m_pFlng ? m_pFlng->Tag():m_sConnTagMem();
      return 1;

    case xidDIOFlngSrcTag:
      DCB.pC=m_pFlng && m_pFlng->m_pNdSrc? m_pFlng->m_pNdSrc->Tag():"";
      return 1;
    case xidDIOFlngDstTag:
      DCB.pC=m_pFlng && m_pFlng->m_pNdDst? m_pFlng->m_pNdDst->Tag():"";
      return 1;
    
    case xidRmtTagConnd:
      DCB.pC=m_pRmt.m_sTag();
      return 1;
    case xidDrctQm:
      DCB.D=HasConduit ? Cd.QMass() : Sum.FlowMass;//dNAN;//(m_bConnected ? m_iMyConnectedIO>=0 ? -m_pNd->IOSign(m_iMyConnectedIO)*m_pNd->IOConduit(m_iMyConnectedIO)->QMass() : Cd.QMass() : 0);
      return 1;
    case xidConnectState:
      DCB.B=m_pRmt.ConnectState();
      return 1;
    //case xidDrctQv:
    //  DCB.D=Sum.totMass;//dNAN;//(m_bConnected ? m_iMyConnectedIO>=0 ? -m_pNd->IOSign(m_iMyConnectedIO)*m_pNd->IOConduit(m_iMyConnectedIO)->QVolume() : Cd.QVolume() : 0);
    //  return 1;
    //case xidDrctNQv:
    //  DCB.D=Sum.totMass;//dNAN;//(m_bConnected ? m_iMyConnectedIO>=0 ? -m_pNd->IOSign(m_iMyConnectedIO)*m_pNd->IOConduit(m_iMyConnectedIO)->QNVolume() : Cd.QNVolume() : 0);
    //  return 1;
    }
  return false;
  };

//--------------------------------------------------------------------------

flag CDirectFlwIO::ValidateData(ValidateDataBlk & VDB)
  {
  MakeRmtTag();
  bool OK=true;
  return OK;
  }

//--------------------------------------------------------------------------

flag CDirectFlwIO::getConnectError()
  {
  if (!Enabled)
    return false;
  if (m_bConnected)
    return false;
  if (m_iMyConnectedIO>=0)
    return false;

  return true;
  };

//--------------------------------------------------------------------------

void CDirectFlwIO::MakeRmtTag()
  {
  m_sRmtTag.LRTrim();
  if (m_pNd)
    m_pNd->m_sAreaTag.LRTrim();

  if (m_sRmtTag.GetLength()>0)
    m_pRmt.m_sTag=m_sRmtTag;
  else if (m_sRmtTagDef.GetLength()>0)
    m_pRmt.m_sTag=m_sRmtTagDef;
  else if (m_pNd && m_pNd->m_sAreaTag.GetLength()>0)
    m_pRmt.m_sTag=m_pNd->m_sAreaTag;
  else
    m_pRmt.m_sTag=gs_DirectFlwIOTagDefault;
  }

//--------------------------------------------------------------------------

static bool gs_DisableDirectFlwIO=false;

flag CDirectFlwIO::GlobalConnect()
  {
  CStopWatch SW;
  SW.Start();

  MakeRmtTag();

  bool Disabled=true;

  switch (m_eType)
    {
    case eDIO_Spill:
    case eDIO_Vent:
      if (sm_ConnectVentSpills!=eDIOSlct_None)
        Disabled=false;
      break;
    case eDIO_Leak:
      if (sm_ConnectLeaks!=eDIOSlct_None)
        Disabled=false;
      break;
    default:
      if (sm_ConnectAudit!=eDIOSlct_None)
        Disabled=false;
      break;
    };

  m_bMLnkConnd=false;
  if (m_iLclIOId>=0 && (m_pNd->IOWithId_Self(m_iLclIOId)>0))
    m_bMLnkConnd=true; // Will Disable

  #if dbgFlwNode
  if (dbgGlobalConnects())
    dbgpln("GlobalConnect %-7s %5i %5i %5i %5s %5s %5s %-15s %35s.%s > %s",
            eTypeStr(m_eType), m_iLclDIOId, m_iRmtDIOId, m_iLclIOId,
            HasConduit?"HasCd":"", Enabled?"ENAB":"", Disabled?"DISAB":"",
            m_pNd->ClassId(), m_pNd->FullObjTag(), m_sTag(), m_pRmt.m_sTag());
  #endif

  if (gs_DisableDirectFlwIO || !Enabled || Disabled)
    return true;


  #if dbgKwikConnect
  dbgpln(" .A %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
  #endif

  m_bConnected = m_pRmt.Connect(m_pNd, m_iRmtDIOId, m_sRmtTypeName());
  if (!m_bConnected && m_pRmt.m_sTag.GetLength()>0)
    {
    //LogError(m_pNd->FullObjTag(), 0, "Envelope %s not found", m_sAreaTag());
    return false;
    }

  if (!m_bConnected)
    return false;

  #if dbgKwikConnect
  dbgpln(" .B %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
  #endif

  CList<CDirectFlwIO*, CDirectFlwIO*> &Connd=m_pRmt->m_DirectIOsConnected;
  if (Connd.GetCount()==0 || Connd.Find(this)==NULL)
    {
    m_RmtListPos=Connd.AddTail(this);
    //dbgpln("+++ %i", Connd.GetCount());
    }

  if (m_iLclDIOId>=0)
    {
    IOAreaRec * NdIOLcl=m_pNd->IOAreas;
    for (int iO=0; (NdIOLcl[iO].IODesc() && NdIOLcl[iO].m_Id != m_iLclDIOId); iO++)
      {};
    // if 'NdIOLcl[iO].pDesc' is NULL then this is not defined in the IOList and hence is not needed

    #if dbgKwikConnect
    dbgpln(" .C %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
    #endif
    if (NdIOLcl[iO].IODesc()==NULL)
      {
      //#ifndef _RELEASE
      //dbgpln("Direct Connect Failed LclDIOId %i not found in %s", m_iLclDIOId, m_pNd->FullObjTag());
      //#endif
      LogError(m_pNd->FullObjTag(), 0, "Direct Connect Failed LclDIOId %i not found", m_iLclDIOId);
      return true;
      }

    IOAreaRec * NdIORmt=m_pRmt->IOAreas;
    for (int iI=0; (NdIORmt[iI].IODesc() &&NdIORmt[iI].m_Id != m_iRmtDIOId); iI++)
      {};
    #if dbgKwikConnect
    dbgpln(" .D %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
    #endif

    if (m_pRmt->NoFlwIOs() >= MaxIOList)
      {
      LogError(m_pNd->FullObjTag(), 0, "%s has Too many Connections directed to it ", m_pRmt->Tag());
      return false;
      }

    if (!m_pNd->GetActive())
      {
      LogNote(m_pNd->FullObjTag(), 0, "Not Active");
      return false;
      }

    if (!m_pRmt->GetActive())
      {
      LogNote(m_pNd->FullObjTag(), 0, "%s Not Active", m_pRmt->Tag());
      return false;
      }

    //if (NdIOLcl[iO].pDesc==NULL)
    //  {
    //  LogWarning(m_pNd->FullObjTag(), 0, "Direct Connect Failed LclDIOId %i not found", m_iLclDIOId);
    //  return false;
    //  }

    if (NdIORmt[iI].IODesc()==NULL)
      {
      LogWarning(m_pNd->FullObjTag(), 0, "Direct Connect Failed RmtDIOId %i not found", m_iRmtDIOId);
      return false;
      }


    ASSERT(NdIOLcl[iO].IODesc()!=NULL);
    ASSERT(NdIORmt[iI].IODesc()!=NULL);

    #if dbgKwikConnect
    dbgpln(" .E %5s %5s %10.1fus", "", "", SW.Lap()*1e6);
    #endif
    IOInfoBlk Info;
    if (m_pNd->ConnectIOTag(m_pNd, NdIOLcl[iO].IOName(), m_pRmt, NdIORmt[iI].IOName(), &Info, m_sConnTagMem())==0)
      {
      if (m_sConnTagMem.Len()==0)
        {
        m_pFlng->InitConnId(m_pNd, NdIOLcl[iO].IOName(), 
                            m_pRmt, NdIORmt[iI].IOName(), NULL);
        }
      else
        {
        m_sConnTagMem="";
        }
      m_iMyConnectedIO = Info[0].IOIndex;
      int iEnvConnectedIO = Info[1].IOIndex;
      m_pNd->SetIOAutoConnect(m_iMyConnectedIO, true);
      m_pRmt->SetIOAutoConnect(iEnvConnectedIO, true);

      #if dbgFlwNode
      if (dbgGlobalConnects())
        dbgpln(".. %s[%i] %s[%i] ", m_pNd->FullObjTag(), m_iMyConnectedIO, m_pNd->Nd_Rmt(m_iMyConnectedIO)->FullObjTag(), iEnvConnectedIO);
      #endif
      #if dbgKwikConnect
      dbgpln(" .F %5i %5i %10.1fus %-20s %s > %s", m_iMyConnectedIO, iEnvConnectedIO, SW.Lap()*1e6, "", m_pNd->FullObjTag(), m_pNd->Nd_Rmt(m_iMyConnectedIO)->FullObjTag());
      #endif

      return true;
      }
    m_iMyConnectedIO = -1;
    LogWarning(m_pNd->FullObjTag(), 0, "Direct Connect Failed");
    return false;
    }
  return true;
  };

// -------------------------------------------------------------------------

flag CDirectFlwIO::GlobalDisConnect()
  {
  #if dbgFlwNode
  if (dbgGlobalConnects())
    dbgpln("GlobalDisConnect %4i %35s.%s > %s", m_iLclDIOId, m_pNd->FullObjTag(), m_sTag(), m_pRmt.m_sTag());
  #endif

  if (m_iLclDIOId>=0)
    {
    if (m_iMyConnectedIO>=0)
      {
      int io=m_pNd->IOWithId_Self(m_iLclDIOId);
      m_sConnTagMem = m_pNd->IOConnect(io)->Tag();
      m_pNd->DisConnect(io);
      m_iMyConnectedIO=-1;
      }
    }

  if (m_pRmt.Connected())
    {

    //CMap<CDirectFlwIO*, CDirectFlwIO*, CDirectFlwIO*, CDirectFlwIO*> &Connd = m_pArea->m_DirectIOsConnected;
    //Connd.RemoveKey(this);
    ////dbgpln("+++ %i", Connd.GetCount());


    //CList<CDirectFlwIO*, CDirectFlwIO*> &Connd=m_pArea->m_DirectIOsConnected;
    //POSITION Pos=Connd.GetCount()>0 ? Connd.Find(this) : NULL;
    if (m_RmtListPos)
      {
      m_pRmt->m_DirectIOsConnected.RemoveAt(m_RmtListPos);
      m_RmtListPos=NULL;
      //dbgpln("--- %i", Connd.GetCount());
      }
    m_pRmt.DisConnect();
    }
  m_bConnected=false;
  return true;
  }

// -------------------------------------------------------------------------

void CDirectFlwIO::GlobalConnectFinal()
  {
  if (strcmp(m_pNd->Tag(), "TNK_1")==0 && m_eType == eDIO_Vent)
    { int xxx=0;; }

  //ConnectFlange();

  if (m_iMyConnectedIO>=0)
    {
    m_iMyConnectedIO=m_pNd->IOWithId_Self(m_iLclDIOId);
    ASSERT(m_iMyConnectedIO>=0);
    }

  //dbgpln("Final: %-20s %5i %5i %-20s %-20s %-20s ", 
  //  m_pNd->Tag(), m_iLclDIOId, m_iMyConnectedIO, 
  //  m_sTag(), m_pFlng?m_pFlng->FullTag():"-", m_pFlng?m_pFlng->FullObjTag():"-");
  };

// -------------------------------------------------------------------------

void CDirectFlwIO::PostConnect(int IONo)
  {
  //m_pNd->PostConnect(IONo);

  if (m_pNd->IOId_Self(IONo)!=m_iLclDIOId)
    return;

  m_pNd->IOConduit(IONo)->AttachMeToObj(m_pNd, TOA_Embedded);
  if (!m_bIsXfer)
    m_pNd->IOFB(IONo,0)->SetParentFlwEqn(&m_FEP);
//  m_pNd->IOFB(IONo,0)->AssignFlwEqnGroup(Leak2AreaGroup, m_FEP.Eqn()->ClassId()/*Leak2AreaGroup.Default()*/, m_pNd);
  m_pNd->IOFB(IONo,0)->SetIsPipe(true);
  m_pNd->IOFB(IONo,0)->SetFBScales(1.0,1.0);

  m_pNd->IOFB(IONo,0)->SetFlowMode(FBMode_Xfer);
  };

// -------------------------------------------------------------------------

void CDirectFlwIO::PreDisConnect(int IONo)
  {
  };

// -------------------------------------------------------------------------

flag CDirectFlwIO::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  if ((IONo>=0) && m_pNd->IOId_Self(IONo)==m_iLclDIOId)
    {
    m_EvalFlowEquationsReturn = m_pNd->IOFB(IONo, FE)->EvaluateFlwEqn(Task, pProps, true, false, m_Opening, &m_pNd->IOFBFlng_Rmt(IONo)->PhD(), NULL);
    return true;
    }
  return false;
  }

// -------------------------------------------------------------------------

void CDirectFlwIO::EvalDiscrete()
  {
  if (m_bConnected && m_iMyConnectedIO>=0)
    {
    }
  else
    {
    };
  }

// -------------------------------------------------------------------------

void CDirectFlwIO::EvalProducts(SpConduit & Cd, bool ScaleCd)
  {
  if (m_iMyConnectedIO>=0)
    {
    if (m_pNd->IOQm_Out(m_iMyConnectedIO)>0.0)
      {
      double QmIn=Cd.QMass();
      double QmLeak=m_pNd->IOQm_Out(m_iMyConnectedIO);
      double FracLeak=QmLeak/GTZ(QmIn);
      m_pNd->IOConduit(m_iMyConnectedIO)->QSetF(Cd, som_ALL, FracLeak);
      if (ScaleCd)
        Cd.QScaleMass(som_ALL, Range(0.0, 1.0-FracLeak, 1.0));
      }
    else
      Cd.QAddF(*m_pNd->IOConduit(m_iMyConnectedIO), som_ALL, 1.0);
    }
  };

// -------------------------------------------------------------------------

void  CDirectFlwIO::Set(CSysVector & V, double Hz)
  {
  if (HasConduit)
    {
    __debugbreak();// should not come here - use copy
    }
  else
    {
    Sum.Set(V, Hz);
    }
  }

// -------------------------------------------------------------------------

void CDirectFlwIO::SetF(SpConduit & CdIn, PhMask PhaseM, double Fraction)
  {
  if (HasConduit)
    {
    Cd.QSetF(CdIn, som_ALL, Fraction, CdIn.Press());
    Sum.Set(Cd);
    }
  else
    Sum.Set(CdIn);
  };

// -------------------------------------------------------------------------

void CDirectFlwIO::SetM(SpConduit & CdIn, PhMask PhaseM, double Qm)
  {
  if (HasConduit)
    {
    Cd.QSetM(CdIn, som_ALL, Qm, CdIn.Press());
    Sum.Set(Cd);
    }
  else
    Sum.Set(CdIn);
  };

// -------------------------------------------------------------------------

void  CDirectFlwIO::Add(CSysVector & V, double Hz)
  {
  if (HasConduit)
    __debugbreak();// should not come here - use copy
  else
    Sum.Add(V, Hz);
  }

// -------------------------------------------------------------------------

void CDirectFlwIO::AddF(SpConduit & CdIn, PhMask PhaseM, double Fraction)
  {
  if (HasConduit)
    {
    Cd.QAddF(CdIn, som_ALL, Fraction);
    Sum.Set(Cd);
    }
  else
    Sum.Set(CdIn);
  };

// -------------------------------------------------------------------------

void CDirectFlwIO::AddM(SpConduit & CdIn, PhMask PhaseM, double Qm)
  {
  if (HasConduit)
    {
    Cd.QAddM(CdIn, som_ALL, Qm);
    Sum.Set(Cd);
    }
  else
    Sum.Set(CdIn);
  };

//==========================================================================

IMPLEMENT_SPARES(CJConnectRec, 10000)
IMPLEMENT_SPARES(JProdFracRec, 10000)
IMPLEMENT_SPARES(JoinQmAux, 10000)
IMPLEMENT_SPARES(CJoinRec, 10000)
IMPLEMENT_SPARES(CLinkRec, 10000)

//==========================================================================
//
//
//
//==========================================================================
