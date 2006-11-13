//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
//#include <math.h>
//#include <ctype.h>
#define  __SFE_BASE_CPP
#include "sc_defs.h"
#include "sfe_base.h"
#include "sp_db.h"
#include "SDBEdit.h"
#include "scdver.h"
#include "scdverlic.h"
#include "tknparse.h"
#include "mdlcfg.h"
#include "dlgbusy.h"
#include "tagdb.h"
#include "ndtree.h"
#include "dbhelper.h"
#include "..\..\utils\devlic\scribble.h"
#include ".\sfe_base.h"
#include "licbase.h"
#include "afxwin.h"
#include "scd_wm.h"
//#include "optoff.h"

// Force search of Version.Lib for Version control Functions
#pragma comment(lib, "version.lib")

extern AFX_EXTENSION_MODULE SCExecDLL;

//===========================================================================
//
//
//
//===========================================================================

CNodeBalanceIO::CNodeBalanceIO ()
  {
  Clear(true);
  }

void CNodeBalanceIO::Clear(bool Full)
  {
  m_iJoinId   = -1;
  m_iSgn      = 0;
  m_Qm        = 0.0;
  m_T         = 0.0;
  m_P         = 0.0;
  m_Rho       = 0.0;
  m_Sf        = 0.0;
  m_Lf        = 0.0;
  m_Vf        = 0.0;
  m_msCp0     = 0.0;
  m_msCpT     = 0.0;
  m_msHsT     = 0.0;
  m_msHzT     = 0.0;
  m_msHfT     = 0.0;
  m_totHsT    = 0.0;
  m_totHzT    = 0.0;
  m_totHfT    = 0.0;
  };

CNodeBalanceIO::CNodeBalanceIO (const CNodeBalanceIO  & X)
  {
  *this=X;
  }

CNodeBalanceIO & CNodeBalanceIO::operator=(const CNodeBalanceIO  & X)
  {
  m_iJoinId     = X.m_iJoinId;
  m_iSgn        = X.m_iSgn;
  m_Tag         = X.m_Tag;
  m_IOPort      = X.m_IOPort;
  m_SpModelName = X.m_SpModelName;
  m_Qm          = X.m_Qm;
  m_T           = X.m_T;
  m_P           = X.m_P;
  m_Rho         = X.m_Rho;
  m_Sf          = X.m_Sf;
  m_Lf          = X.m_Lf;
  m_Vf          = X.m_Vf;
  m_msCp0       = X.m_msCp0;
  m_msCpT       = X.m_msCpT;
  m_msHsT       = X.m_msHsT;
  m_msHzT       = X.m_msHzT;
  m_msHfT       = X.m_msHfT;
  m_totHsT      = X.m_totHsT;
  m_totHzT      = X.m_totHzT;
  m_totHfT      = X.m_totHfT;
  return *this;
  }

void CNodeBalanceIO::DoCalculations()
  {
  };

//---------------------------------------------------------------------------


CNodeBalanceJoin::CNodeBalanceJoin()
  {
  Clear(true);
  }

void CNodeBalanceJoin::Clear(bool Full)
  {
  m_JoinMassFlwIn       = 0.0;
  m_JoinMassGain        = 0.0;
  m_JoinMassFlwOut      = 0.0;
  m_JoinFeedCpT         = 1.0;
  m_JoinProdCpT         = 1.0;
  m_JoinHfIn            = 0.0;
  m_JoinHfGain          = 0.0;
  m_JoinHfOut           = 0.0;
  m_JoinHfChg           = 0.0;
  m_JoinHsIn            = 0.0;
  m_JoinHsGain          = 0.0;
  m_JoinHsOut           = 0.0;
  m_JoinHsChg           = 0.0;
  m_JoinPowerIn         = 0.0;

  m_RCTMassGain         = 0.0;
  m_RCTHfGain           = 0.0;
  m_RCTHsGain           = 0.0;
  m_RCTHOR_Std0         = 0.0;
  m_RCTHOR_Mdl0         = 0.0;
  m_RCTHOR_Used0        = 0.0;
  m_RCTHOR_Diff0        = 0.0;
  m_RCTPowerIn          = 0.0;
  m_VLEHsGain           = 0.0;
  m_EHXPowerIn          = 0.0;

  };

CNodeBalanceJoin::CNodeBalanceJoin(const CNodeBalanceJoin& X)
  {
  *this=X;
  }

CNodeBalanceJoin& CNodeBalanceJoin::operator=(const CNodeBalanceJoin& X)
  {
  m_JoinMassFlwIn       = X.m_JoinMassFlwIn;
  m_JoinMassGain        = X.m_JoinMassGain;
  m_JoinMassFlwOut      = X.m_JoinMassFlwOut;
  m_JoinFeedCpT         = X.m_JoinFeedCpT;
  m_JoinProdCpT         = X.m_JoinProdCpT;
  m_JoinHfIn            = X.m_JoinHfIn;
  m_JoinHfGain          = X.m_JoinHfGain;
  m_JoinHfOut           = X.m_JoinHfOut;
  m_JoinHfChg           = X.m_JoinHfChg;
  m_JoinHsIn            = X.m_JoinHsIn;
  m_JoinHsGain          = X.m_JoinHsGain;
  m_JoinHsOut           = X.m_JoinHsOut;
  m_JoinHsChg           = X.m_JoinHsChg;
  m_JoinPowerIn         = X.m_JoinPowerIn;

  m_RCTHfGain           = X.m_RCTHfGain;
  m_RCTHsGain           = X.m_RCTHsGain;
  m_RCTHOR_Std0         = X.m_RCTHOR_Std0;
  m_RCTHOR_Mdl0         = X.m_RCTHOR_Mdl0;
  m_RCTHOR_Used0        = X.m_RCTHOR_Used0;
  m_RCTHOR_Diff0        = X.m_RCTHOR_Diff0;
  m_RCTPowerIn          = X.m_RCTPowerIn;
  m_VLEHsGain           = X.m_VLEHsGain;
  m_EHXPowerIn          = X.m_EHXPowerIn;

  return *this;
  }

CNodeBalanceJoin& CNodeBalanceJoin::operator+=(const CNodeBalanceJoin& X)
  {
  m_JoinMassFlwIn       += X.m_JoinMassFlwIn;
  m_JoinMassGain        += X.m_JoinMassGain;
  m_JoinMassFlwOut      += X.m_JoinMassFlwOut;
  m_JoinFeedCpT         += X.m_JoinFeedCpT;
  m_JoinProdCpT         += X.m_JoinProdCpT;
  m_JoinHfIn            += X.m_JoinHfIn;
  m_JoinHfGain          += X.m_JoinHfGain;
  m_JoinHfOut           += X.m_JoinHfOut;
  m_JoinHfChg           += X.m_JoinHfChg;
  m_JoinHsIn            += X.m_JoinHsIn;
  m_JoinHsGain          += X.m_JoinHsGain;
  m_JoinHsOut           += X.m_JoinHsOut;
  m_JoinHsChg           += X.m_JoinHsChg;
  m_JoinPowerIn         += X.m_JoinPowerIn;

  m_RCTHfGain           += X.m_RCTHfGain;
  m_RCTHsGain           += X.m_RCTHsGain;
  m_RCTHOR_Std0         += X.m_RCTHOR_Std0;
  m_RCTHOR_Mdl0         += X.m_RCTHOR_Mdl0;
  m_RCTHOR_Used0        += X.m_RCTHOR_Used0;
  m_RCTHOR_Diff0        += X.m_RCTHOR_Diff0;
  m_RCTPowerIn          += X.m_RCTPowerIn;
  m_VLEHsGain           += X.m_VLEHsGain;
  m_EHXPowerIn          += X.m_EHXPowerIn;

  return *this;
  }

void CNodeBalanceJoin::DoCalculations()
  {
  };


void CNodeBalanceJoin::AddIO(const CNodeBalanceIO & X)
  {
  }

//---------------------------------------------------------------------------

CNodeBalanceInfo::CNodeBalanceInfo(LPCTSTR Tag)
  {
  m_NodeTag=Tag;
  }

void CNodeBalanceInfo::SetJoinCount(int N)
  {
  m_Joins.SetSize(Max(1,N));
  }

void CNodeBalanceInfo::DoCalculations(bool CombinedJoins)
  {
  for (int i=0; i<m_Feeds.GetCount(); i++)
    {
    m_Feeds[i].DoCalculations();
    m_Joins[m_Feeds[i].m_iJoinId].AddIO(m_Feeds[i]);
    }
  for (int i=0; i<m_Prods.GetCount(); i++)
    {
    m_Prods[i].DoCalculations();
    m_Joins[m_Prods[i].m_iJoinId].AddIO(m_Prods[i]);
    }

  for (int j=0; j<m_Joins.GetSize(); j++)
    m_Joins[j].DoCalculations();
  };

void CNodeBalanceInfo::Combine(CNodeBalanceInfo & B)
  {
  m_NodeTag=B.m_NodeTag;
  m_Feeds.SetSize(B.m_Feeds.GetSize());
  m_Prods.SetSize(B.m_Prods.GetSize());

  m_Joins.SetSize(1);
  m_Joins[0].Clear(true);

  for (int i=0; i<B.m_Feeds.GetCount(); i++)
    {
    m_Feeds[i]=B.m_Feeds[i];
    m_Feeds[i].m_iJoinId=0;
    }
  for (int i=0; i<B.m_Prods.GetCount(); i++)
    {
    m_Prods[i]=B.m_Prods[i];
    m_Prods[i].m_iJoinId=0;
    }
  for (int i=0; i<B.m_Joins.GetCount(); i++)
    m_Joins[0]+=B.m_Joins[i];
  };

//===========================================================================
//
//
//
//===========================================================================

CNodeAuditInfo::CNodeAuditInfo(LPCTSTR Tag, dword Options)
  {
  m_NodeTag=Tag;
  m_Options=Options;
  };

//===========================================================================
//
//
//
//===========================================================================

LPCTSTR NErr_String(int iErr)
  {
  switch (iErr)
    {

    case NErr_NotFound                : return "NotFound";
    case NErr_NoSpace                 : return "NoSpace";
    case NErr_NoProblem               : return "NoProblem";
    case NErr_NotConverged            : return "NotConverged";
    case NErr_AlreadyConnected        : return "AlreadyConnected";
    case NErr_Toomanyconnections      : return "Toomanyconnections";
    case NErr_Incompatibleconnections : return "Incompatibleconnections";
    case NErr_AlreadyExists           : return "AlreadyExists";
    case NErr_SelfConnect             : return "SelfConnect";
    }
  return "???";
  };

//===========================================================================
//
//
//
//===========================================================================

CFlwTreeItem::CFlwTreeItem()
  {
  pChild=NULL;
  pNext=NULL;
//    CFlwTreeItem * pNext;
//    CFlwTreeItem * pChild;
//    Strng          sText; 
//    DWORD          dwFlags; 
//    int            iImage; 
  }

//--------------------------------------------------------------------------

CFlwTreeItem::CFlwTreeItem(char * Txt, DWORD Flags, int Image)
  {
  pChild=NULL;
  pNext=NULL;
  sText=Txt; 
  dwFlags=Flags; 
  iImage=Image; 
  }

//--------------------------------------------------------------------------

CFlwTreeItem::~CFlwTreeItem()
  {
  delete pChild;
  delete pNext;
  };

//--------------------------------------------------------------------------

CFlwTreeItem * CFlwTreeItem::AddSibling(char * Txt, DWORD Flags, int Image)
  {
  CFlwTreeItem * pNew=new CFlwTreeItem(Txt, Flags, Image);
  CFlwTreeItem * pThis=this;
  while (pThis->pNext)
    pThis=pThis->pNext;
  pThis->pNext=pNew;

//  CFlwTreeItem * pNew=new CFlwTreeItem;
//  if (pNext)
//    {
//    CFlwTreeItem * p=pNext;
//    while (p->pNext)
//      p=p->pNext;
//    p->pNext=pNew;
//    }
//  else
//    pNext=pNew;
  return pNew;
  };

//--------------------------------------------------------------------------

CFlwTreeItem * CFlwTreeItem::AddChild(char * Txt, DWORD Flags, int Image)
  {
  CFlwTreeItem * pNew=new CFlwTreeItem(Txt, Flags, Image);
  if (pChild)
    {
    CFlwTreeItem * p=pChild;
    while (p->pNext)
      p=p->pNext;
    p->pNext=pNew;
    }
  else
    pChild=pNew;
  
  return pNew;
  };

//===========================================================================
//
//
//
//===========================================================================

void CNodeWiring::BuildWiring(LPCTSTR pTag, CNodeElectrics &WI)
  {
  m_NodeTag=pTag;
  m_Cables.SetSize(0, 4);
  m_Connects.SetSize(0, 16);
  m_Terminals.SetSize(0, 16);
  m_TermStrips.SetSize(0, 4);

//void CNodeWiring::AddTermStrip(LPCTSTR Name)

  //  {
//  CTermStrip C(Name);
//  m_TermStrips.Add(C);
//  };
//void CNodeWiring::AddTerminal(long TSNo, LPCTSTR Name)
//  {
//  CTerminal T(Name);
//  T.m_iTermStrip=TSNo;
//  m_Terminals.Add(T);
//  };
//void CNodeWiring::AddConnect(long CNo, LPCTSTR SrcTS, LPCTSTR SrcTerm, LPCTSTR DstTS, LPCTSTR DstTerm)
//  {
//  CConnect C;
//  m_Connects.Add(C);
//  };

  for (int t=0; t<WI.TermStripCount(); t++)
    {
    CETermStrip &TS=*WI.TermStrip(t);
    m_TermStrips.Add(CTermStrip(TS.Name()));
    for (int i=0; i<TS.TerminalCount(); i++)
      m_Terminals.Add(CTerminal(TS[i]->m_sName, t));
    }
  
  for (int c=0; c<WI.ComponentCount(); c++)
    {
    CEComponent &C=*WI.Component(c);
    //WI.m_Connects.m_pData
    //AddConnect(c, C.SrcTSName(), C.SrcTName(), C.DstTSName(), C.DstTName());
    }

  }

void CNodeWiring::AddCable(LPCTSTR Name)
  {
  CCable C(Name);
  m_Cables.Add(C);
  };

int CNodeWiring::FindTerminal(int iTermStrip, LPCTSTR Name)
  {
  for (int i=0; i<m_Terminals.GetCount(); i++)
    if (m_Terminals[i].m_iTermStrip==iTermStrip && 
        m_Terminals[i].m_sName.CompareNoCase(Name)==0)
      return i;
  return -1;
  };
int CNodeWiring::TSTerminals(int iTermStrip, CLArray & Terminals)
  {
  long n=0;
  for (long i=0; i<m_Terminals.GetCount(); i++)
    if (m_Terminals[i].m_iTermStrip==iTermStrip)
      Terminals.SetAtGrow(n++, i);
  if (Terminals.GetSize()>n)
    Terminals.SetSize(n);
  return n;
  };

//---------------------------------------------------------------------------

CString CNodeWiring::TsName(long iTs)
  {
  CString S=m_TermStrips[iTs].m_sName;
  return S;
  };
CString CNodeWiring::TsTrmName(long iTerm)
  {
  CTerminal &T=m_Terminals[iTerm];
  CString S=m_TermStrips[T.m_iTermStrip].m_sName;
  return S;
  };
CString CNodeWiring::ConnectSrcName(long iConnect)
  {
  CConnect &C=m_Connects[iConnect];
  CTerminal &T=m_Terminals[C.m_iSrcTerm];
  CString S=m_TermStrips[T.m_iTermStrip].m_sName;
  S+=":";
  S+=T.m_sName;
  return S;
  };
CString CNodeWiring::ConnectDstName(long iConnect)
  {
  CConnect &C=m_Connects[iConnect];
  CTerminal &T=m_Terminals[C.m_iDstTerm];
  CString S=m_TermStrips[T.m_iTermStrip].m_sName;
  S+=":";
  S+=T.m_sName;
  return S;
  };

//===========================================================================
//
//
//
//===========================================================================

long CTraceItem::m_lIterCount=0;

CTraceItem::CTraceItem()
  {
  m_lId=-1;
  m_nVals=0;
  for (int i=0; i<MaxChngTrcVals; i++)
    {
    m_dVals[i]  = dQuietNAN;
    m_dRqds[i]  = dQuietNAN;
    m_lIter[i]  = -1;
    }
  };

// --------------------------------------------------------------------------

CTraceItem::~CTraceItem()
  {
  };

// --------------------------------------------------------------------------

CTraceItem::CTraceItem(const CTraceItem & Other)
  {
  m_sFullTag=Other.m_sFullTag;
  m_sDstTag=Other.m_sDstTag;
  m_sDescTag=Other.m_sDescTag;
  m_sCnv=Other.m_sCnv;
  m_lId=Other.m_lId;
  for (int i=0; i<MaxChngTrcVals; i++)
    {
    m_dVals[i]=Other.m_dVals[i];
    m_dRqds[i]=Other.m_dRqds[i];
    m_lIter[i]=Other.m_lIter[i];
    }
  m_nVals=Other.m_nVals;
  };

// --------------------------------------------------------------------------

CTraceItem & CTraceItem::operator=(const CTraceItem & Other)
  {
  m_sFullTag=Other.m_sFullTag;
  m_sDstTag=Other.m_sDstTag;
  m_sDescTag=Other.m_sDescTag;
  m_sCnv=Other.m_sCnv;
  m_lId=Other.m_lId;
  for (int i=0; i<MaxChngTrcVals; i++)
    {
    m_dVals[i]=Other.m_dVals[i];
    m_dRqds[i]=Other.m_dRqds[i];
    m_lIter[i]=Other.m_lIter[i];
    }
  m_nVals=Other.m_nVals;
  
  return *this;
  };

// --------------------------------------------------------------------------

void CTraceItem::Init(long Id, LPCTSTR DescTag, LPCTSTR Cnv, LPCTSTR NdTg, LPCTSTR DstNdTg)
  {
  m_lId=Id;
  m_sDescTag=DescTag;
  m_sDstTag=DstNdTg;
  m_sCnv=Cnv;
  m_sFullTag.Set("%s.%s.[%s].%s", NdTg, Prefix(), DstNdTg, DescTag);
  }

// --------------------------------------------------------------------------

void CTraceItem::Set(double Val, double Rqd/*=dNAN*/)
  {
  for (int i=MaxChngTrcVals-1; i>0; i--)
    {
    m_dVals[i]=m_dVals[i-1];
    m_dRqds[i]=m_dRqds[i-1];
    m_lIter[i]=m_lIter[i-1];
    }
  m_dVals[0]=Val;
  m_dRqds[0]=Rqd;
  m_lIter[0]=m_lIterCount;
  if (m_nVals<MaxChngTrcVals)
    m_nVals++;
  }

// --------------------------------------------------------------------------

double CTraceItem::CalcValue(eTraceValue What, int i/*=0*/)
  {
  switch (What)
    {
    case TV_AbsChg: return AbsChg(i);
    case TV_RelChg: return RelChg(i);
    case TV_Value:  return Val(i);
    }
  return 0;
  };

// --------------------------------------------------------------------------

LPCTSTR  CTraceItem::IdStr(long Id)
  {
  static LPCTSTR S[] = {"Chg", "Mv", "Qm", "T",  "P"};
  return (Id>=0 && Id<sizeof(S)/sizeof(S[0])) ? S[Id] : "??";
  };

// --------------------------------------------------------------------------

//void CTraceItem::DoCalculations(double Smoothing)
//  {
//  double E = Valid(dRqd) ? (dVal-dRqd)/(1.0e-6+Max(fabs(dVal), fabs(dRqd))): 0.0;
//  double C = m_nVals>=1 ? (dVal-m_dVals[0])/(1.0e-6+fabs(dVal)) : 0;
//  double O = m_nVals>=2 ? ((dVal-m_dVals[0])-(m_dVals[0]-m_dVals[1]))/(1.0e-6+fabs(dVal)) : 0;
//
//  dRelErr = (1.0-Smoothing)*E+Smoothing*dRelErr;
//  dRelChg = (1.0-Smoothing)*C+Smoothing*dRelChg;
//  dRelOsc = (1.0-Smoothing)*O+Smoothing*dRelOsc;
//
//  #if dbgFlwNode
//  if (dbgChangeStats())
//    {
//    //static double xxx=0.0;
//    //static double yyy=0.0;
//    //dRelErr = ++xxx/100.0;
//    //dRelChg = ++yyy;
//    dbgpln("** %12.6e  %12.6e  %12.6e  %12.6e  %12.6e  %s", dVal, dRqd, dRelErr, dRelChg, dRelOsc, sFullTag());
//    }
//  #endif
//
//  for (int i=MaxChngTrcVals-1; i>0; i--)
//    m_dVals[i]=m_dVals[i-1];
//  m_dVals[0]=dVal;
//  if (m_nVals<MaxChngTrcVals)
//    m_nVals++;
//  }
     

bool   CTraceItem::ErrorDefined(int i) 
  { 
  return Valid(Rqd())!=0; 
  };
double CTraceItem::AbsErr(int i)       
  { 
  return m_dVals[i]-m_dRqds[i]; 
  };
double CTraceItem::AbsChg(int i)
  { 
  return Valid(m_dVals[i]) && Valid(m_dVals[i+1]) ? m_dVals[i]-m_dVals[i+1] : dQuietNAN;
  };
double CTraceItem::RelErr(int i)
  { 
  return Valid(m_dVals[i]) && Valid(m_dRqds[i]) ? (m_dVals[i]-m_dRqds[i])/GTZ(Max(fabs(m_dVals[i]), fabs(m_dRqds[i]))) : dQuietNAN; 
  };
double CTraceItem::RelChg(int i)
  { 
  return Valid(m_dVals[i]) && Valid(m_dVals[i+1]) ? (m_dVals[i]-m_dVals[i+1])/GTZ(Max(fabs(m_dVals[i]), fabs(m_dVals[i+1]))) : dQuietNAN; 
  };

//===========================================================================
//
//
//
//===========================================================================

//class CFlwTree : public CFlwTreeItem
//  {
//  public:
//
CFlwTree::CFlwTree()
  {
  };

//--------------------------------------------------------------------------

CFlwTree::~CFlwTree()
  {
  };

//===========================================================================
//
//
//
//===========================================================================

CSfeSrvrBase * gs_pSfeSrvr=NULL;

CSfeSrvrBase::CSfeSrvrBase()  
  {
  SfeCallBack=NULL; 
  SfeFS=NULL;
  };

//--------------------------------------------------------------------------

CSfeSrvrBase::~CSfeSrvrBase() 
  { 
  ASSERT(SfeFS==NULL);
  };

//--------------------------------------------------------------------------

flag CSfeSrvrBase::RequestModelInfoByGroupIndex(pchar pGroup, int iNo, RequestModelInfoRec &Info)
  {
  Info.Class="";
  Info.Desc="";
  pTagObjClass p = TagObjClass::FindClassIndexed(pGroup, iNo);
  if (p)
    {
    Info.Class=p->ClassId();
    Info.Desc=p->Desc();
    Info.ShortDesc=p->ShortDesc();
    Info.DefTag=p->DefaultTag();
    Info.MdlLibName=p->MdlLibName();
    Info.Group=p->Group();
    Info.DrwGroup=p->DrwGroup();
    Info.Category=p->Category();
    Info.IsSelectable=p->Selectable()!=0;
    return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

flag CSfeSrvrBase::RequestModelInfoByClassId(pchar pClass, RequestModelInfoRec &Info)
  {
  Info.Class="";
  Info.Desc="";
  if (pClass==NULL || pClass[0]==0)
    return false;
  pTagObjClass p = TagObjClass::FindClassId(pClass);
  if (p)
    {
    Info.Class=p->ClassId();
    Info.Desc=p->Desc();
    Info.ShortDesc=p->ShortDesc();
    Info.DefTag=p->DefaultTag();
    Info.MdlLibName=p->MdlLibName();
    Info.Group=p->Group();
    Info.DrwGroup=p->DrwGroup();
    Info.Category=p->Category();
    return true;
    }
  return false;
  }

//--------------------------------------------------------------------------
/*
flag CSfeSrvrBase::RequestModelIOInfo(pchar pTag, int iIndex, RequestModelIOInfoRec & Info)
  {
  int ObjTagLen=0;
  pFlwNode p=dynamic_cast<FlwNode*>(CTNode::FindObjTag(pTag, false, ObjTagLen, 0));
  if (p)
    {
    IOAreaRec * pIO=p->IODesc_SelfIndexed(iIndex);
    if (pIO)
      {
      Info.Name=pIO->pName;
      Info.Desc=pIO->pDesc;
      Info.iId=pIO->Id;
      Info.Input=(pIO->Dirn & LIO_In)!=0;
      Info.Output=(pIO->Dirn & LIO_Out)!=0;
      Info.nIORqd=pIO->nIORqd;
      Info.nIOMax=pIO->nIOMax;
      Info.Dirn=pIO->Dirn;
      Info.iGrp=IOGRP(pIO->m_dwFlags);
      return true;
      }
    }
  return false;
  }
*/
//--------------------------------------------------------------------------
//
//flag CSfeSrvrBase::RequestModelIOConn(pchar pTag, int iNo, RequestModelIOConnRec & Info)
//  {
//  int ObjTagLen=0;
//  pFlwNode p=dynamic_cast<FlwNode*>(CTNode::FindObjTag(pTag, false, ObjTagLen, 0));
//  if (p && iNo<p->NIOs)
//    {
//    Info.iId = p->IOId_Self(iNo);
//    Info.Tag = p->Nd_Rmt(iNo)->FullObjTag();
//    Info.ClassName = p->Nd_Rmt(iNo)->Class()->ClassName();
//    Info.ClassId   = p->Nd_Rmt(iNo)->ClassId();
//    Info.Group     = p->Nd_Rmt(iNo)->Group();
//    Info.iGrp      = IOGRP(p->IODesc_Self(iNo)->m_dwFlags);
//    if (p->NIOs==2)
//      Info.iDirn = (iNo==0) ? 1 : -1;
//    else if (p->Nd_Rmt(iNo)->NIOs==2)
//      Info.iDirn = (p->IOIONo_Rmt(iNo)==0) ? -1 : 1;
//    else
//      Info.iDirn = p->IODesc_Self(iNo)->Dirn & LIO_In ? 1 : -1;
//    return true;
//    }
//  return false;
//  }

//===========================================================================
//
//
//
//===========================================================================

CGrfTagGroups gs_GrfTagGroups;

void CGrfTagGroups::Reset(int Size/*=0*/)
  {
  Groups.SetSize(Size);
  Active.SetSize(Size);
  }

//--------------------------------------------------------------------------

int CGrfTagGroups::Find(char* pGrpName)
  {
  for (int i=0; i<Groups.GetSize(); i++)
    if (_stricmp(pGrpName, Groups[i].Buffer())==0)
      return i;
  return -1;
  }

char* CGrfTagGroups::GetName(int index)
  {
  return Groups[index].Buffer();
  }

bool CGrfTagGroups::IsActive(int index)
  {
  return (Active[index]!=0);
  }

void CGrfTagGroups::Rebuild(bool Force/*=false*/)
  {
  ScdMainWnd()->PostMessage(WMU_GRFTAGGROUPS, SUB_GTG_BUILD, (LPARAM)Force);
  }

void CGrfTagGroups::SetActive(int index, bool Active)
  {
  if (Active)
    ScdMainWnd()->PostMessage(WMU_GRFTAGGROUPS, SUB_GTG_ACTIVATE, (LPARAM)index);
  else
    ScdMainWnd()->PostMessage(WMU_GRFTAGGROUPS, SUB_GTG_DEACTIVATE, (LPARAM)index);
  }

//===========================================================================
//
//
//
//===========================================================================

CPrjSaveAsDlg::CPrjSaveAsDlg(char* pRootDir, BOOL IsNew, CWnd* pParent /*=NULL*/)
  : CDialog(CPrjSaveAsDlg::IDD, pParent)
  {
  sRoot = pRootDir;
  bIsNew = IsNew;
  //{{AFX_DATA_INIT(CPrjSaveAsDlg)
  m_CopyHist = TRUE;
	m_DbFmt = 0;
	m_VerStr = _T("");
	m_RemovePrevRevs = TRUE;
	m_CopyDBFromOld = FALSE;
	//}}AFX_DATA_INIT
  for (int i=0; i<6; i++)
    {
    m_Copy[i] = FALSE;
    m_CopyCnt[i] = 0;
    }
  m_Copy[4] = TRUE;
  m_Copy[5] = TRUE;
  m_DBOn=true;
  m_CopyOn=true;
  m_CopyDBOn=false;
  }

//---------------------------------------------------------------------------

void CPrjSaveAsDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPrjSaveAsDlg)
  DDX_Check(pDX, IDC_COPYHIST, m_CopyHist);
  DDX_Text(pDX, IDC_VERSSTR, m_VerStr);
  DDX_Check(pDX, IDC_REMOVEPREVREVS, m_RemovePrevRevs);
  DDX_Check(pDX, IDC_COPYDBFROMOLD, m_CopyDBFromOld);
  //}}AFX_DATA_MAP
  DDX_Check(pDX, IDC_COPYSNAP, m_Copy[0]);
  DDX_Check(pDX, IDC_COPYSCENARIOS, m_Copy[1]);
  DDX_Check(pDX, IDC_COPYPGM, m_Copy[2]);
  DDX_Check(pDX, IDC_COPYRCT, m_Copy[3]);
  DDX_Check(pDX, IDC_COPYREPORTS, m_Copy[4]);
  DDX_Check(pDX, IDC_COPYSCRIPTS, m_Copy[5]);

  DDX_Control(pDX, IDC_DBFMTCOMBO, m_cbFmts);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CPrjSaveAsDlg, CDialog)
  //{{AFX_MSG_MAP(CPrjSaveAsDlg)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CPrjSaveAsDlg::OnInitDialog() 
  {
  UINT IDs[] = { IDC_COPYSNAP, IDC_COPYSCENARIOS, IDC_COPYPGM, IDC_COPYRCT, IDC_COPYREPORTS, IDC_COPYSCRIPTS };
  for (int i=0; i<6; i++)
    GetDlgItem(IDs[i])->EnableWindow(m_CopyOn && (m_CopyCnt[i]>0));
  
  GetDlgItem(IDC_COPYSTATIC)->EnableWindow(m_CopyOn);
  GetDlgItem(IDC_COPYHIST)->EnableWindow(m_CopyOn);

  SetWindowText(bIsNew ? "New Project Options" : "Project Save Options");
  GetDlgItem(IDC_VERSSTR)->EnableWindow(true/*!bIsNew*/);

  if (bIsNew)
    {
    int HideWnds[] = {
      IDC_COPYSTATIC,
      IDC_DATASTATIC2,
      IDC_REMOVEPREVREVS,
      IDC_COPYDBFROMOLD,
      -1};
    for (int h=0; HideWnds[h]>0; h++)
      GetDlgItem(HideWnds[h])->ShowWindow(FALSE);

    CRect WinRect, wr1, wr2, mwr;
    GetWindowRect(&WinRect);
    GetDlgItem(IDC_DATASTATIC)->GetWindowRect(&wr1);
    GetDlgItem(IDC_COPYSTATIC)->GetWindowRect(&wr2);
    int dy=wr2.bottom-wr1.bottom;

    WinRect.bottom -= dy;
    MoveWindow(&WinRect, TRUE);
    }
  
  UpdateData(FALSE);

//  GetDlgItem(IDC_REMOVEPREVREVS)->ShowWindow(m_DbFmt!=2 ? SW_SHOW : SW_HIDE);
  GetDlgItem(IDC_REMOVEPREVREVS)->EnableWindow(m_DbFmt!=2);
  if (!m_CopyDBOn)
    ((CButton*)GetDlgItem(IDC_COPYDBFROMOLD))->SetCheck(0);
  GetDlgItem(IDC_COPYDBFROMOLD)->EnableWindow(m_DbFmt!=2 && m_CopyDBOn);

  for (long i=0; i<DBConnectCount(); i++)
    m_cbFmts.AddString(DBConnectName(i));
  m_cbFmts.SetCurSel(m_DbFmt);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------

void CPrjSaveAsDlg::OnBnClickedOk()
  {
  UpdateData(true);
  m_DbFmt=m_cbFmts.GetCurSel();
  if (m_VerStr.FindOneOf("\\/:*?\"<>|")<0)
    OnOK();
  else
    AfxMessageBox("Invalid Characters in Comment String (\\/:*\"<>|)", MB_ICONEXCLAMATION|MB_OK);
  }

//===========================================================================

static char *KenwaltDLLs[]=
  {
  //base...
  "flwlib.dll",
  "mdlbase.dll",

  //standard...
  "alumina1.dll", 
  "basic1.dll",
  "commn1.dll",
  "control1.dll",
  "control2.dll",
  "heatxch1.dll",
  "mathand1.dll",
  "power1.dll",
  "separ1.dll",
  "separ2.dll",
  "sizedst1.dll",
  
  //client...
  "alcanmdl.dll",
  "alcanspm.dll",
  "alcoaspmdl.dll",
  "alunorte.dll",
  "qalmdls.dll",
  "qalprcp1.dll",
  "qalspmdl.dll",
  "rtts.dll",

  // temporary
  "sko.dll",
  "ilukaunits.dll",
  "alumina2.dll",
  "heatxch2.dll",
  
  NULL
  };

int DLLVersionOK(char * FullName)
  {
  int ErrCode=1;
  DWORD dwHandle;
  DWORD dwVerSize=::GetFileVersionInfoSize(FullName, &dwHandle);
  if (dwVerSize)
    {
    char * Buff=new char[dwVerSize];
    if (::GetFileVersionInfo(FullName, dwHandle, dwVerSize, Buff))
      {
      VS_FIXEDFILEINFO * pInfo;
      UINT InfoLen;
      if (::VerQueryValue(Buff, "\\", (LPVOID*)&pInfo, &InfoLen))
        {
        if ((HIWORD(pInfo->dwFileVersionMS)==SCD_VERINFO_V0) &&
            (LOWORD(pInfo->dwFileVersionMS)==SCD_VERINFO_V1) &&
            (HIWORD(pInfo->dwFileVersionLS)==SCD_VERINFO_V2))
          ErrCode=0;
        }
      }
    delete[] Buff;
    }

  if (ErrCode==0)
    {
    Strng NameOnly;
    NameOnly.FnNameExt(FullName);
    for (int i=0; KenwaltDLLs[i]; i++)
      {
      if (_stricmp(NameOnly(), KenwaltDLLs[i])==0)
        break;
      }
    if (KenwaltDLLs[i]!=NULL)
      return 0; //no further tests required
    }
  
  if (ErrCode==0 || ErrCode==1)
    {
    //Assume SMDK DLL, check if legal...
    HINSTANCE hInst = MyAfxLoadLibrary(FullName, DllFilesPath());
    if (hInst!=NULL)
      {
      if (GetProcAddress(hInst, "IsSMDKLibDLL")!=NULL)
        ErrCode=0;
      AfxFreeLibrary(hInst);
      }
    else
      ErrCode=3;
    #if !BYPASSLICENSING
    if (ErrCode==0)
      {
      //LogNote("xxx",0,"Check scribble %s", FullName);
      CScribble TestDll;
      CString fn;
      fn = FullName;
      if (!TestDll.ReadScribble(fn))
        ErrCode=2;
      }
    #endif
    }

  return ErrCode;
  }

flag IsDLLOK(char *CfgName, char * DLL, char * DllPath, Strng & FullName, flag CheckVersion, flag CheckScribble, flag Whinge, flag WhingeLoudly = false)
  {
  flag OK=false;
  if (FindDLLOrHlp(DLL, DllPath, FullName))
    {
    if (CheckVersion)
      {
      int ErrCode = DLLVersionOK(FullName());
      OK=(ErrCode==0);
      if (!OK && (1||Whinge))
        {
        if (ErrCode==1)
          LogWarning(CfgName, WhingeLoudly ? LF_Exclamation: 0, "Incorrect Model DLL Version '%s'", FullName());
        else if (ErrCode==3)
          LogWarning(CfgName, WhingeLoudly ? LF_Exclamation: 0, "Model DLL Not Loaded '%s' (Not found or Missing Dependency)", FullName());
        else if (ErrCode==2)
          LogWarning(DLL/*CfgName*/, WhingeLoudly ? LF_Exclamation: 0, "User Model DLL not licensed with SMDK! (%s)", FullName());
        }
      }
/*    else if (CheckScribble)
      {
      OK=true;
      CScribble TestDll;
      CString fn;
      fn = FullName();
      OK=TestDll.ReadScribble(fn) || BYPASSLICENSING;
      LogNote("xxx",0,"Check Scribble %d  %s", OK, FullName());
      if (!OK && (1||Whinge))
        LogWarning(DLL, WhingeLoudly ? LF_Exclamation: 0, "User Model DLL not licensed with SMDK! (%s)", FullName());
      }
    else
      OK=TRUE;*/
    }
  else if (Whinge)
    {
    Strng DLLName;
    DLLName.FnNameExt(FullName());
    LogWarning(CfgName, WhingeLoudly ? LF_Exclamation: 0, "Model DLL '%s' not Found", DLLName());
    }

  return OK;
  }

//===========================================================================
//
//
//
//===========================================================================

class CNewCfgDlg : public CDialog
  {
  public:
	  CNewCfgDlg(char* pCfgFile, CWnd* pParent = NULL);
    //{{AFX_DATA(CNewCfgDlg)
    enum { IDD = IDD_CFG_NEWDLG };
    //}}AFX_DATA
	  //{{AFX_VIRTUAL(CNewCfgDlg)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();
	  //}}AFX_VIRTUAL
    Strng SrcCfgFile;
  protected:
    bool m_bOK;
    bool bNewCfg;
	  //{{AFX_MSG(CNewCfgDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnBrowse();
    afx_msg void OnKillfocusCfg();
    afx_msg void OnChangeName();
    afx_msg void OnChangeFolder();
    //}}AFX_MSG
    afx_msg void OnUpdateOK(CCmdUI* pCmdUi);
	  DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnStnClickedCfgCopyname();
    afx_msg void OnEnChangeCfgName();
    afx_msg void OnBnClickedInfilesfolder();
    afx_msg void OnEnChangeCfgFilesfolder();
    afx_msg void OnBnClickedForceeqlname();
    BOOL m_bPrjEqlName;
    CString m_sCfgName;
    CString m_sCfgFolderName;
    CString m_sRootFolder;
    CString m_PrjSetName;
    CString m_sSummary1;
    CString m_sSummary2;
    CString m_sSummary3;

    CString m_sCfgFile;
    CString m_sCfgFiles;
    CString m_sDescription;

    void ConstructPaths(bool Ld, bool Sv, bool StripHome=true);

    afx_msg void OnEnKillfocusCfgPrjset();
    afx_msg void OnEnKillfocusCfgFilesfolder();
    afx_msg void OnEnChangeCfgPrjset();
  };

//---------------------------------------------------------------------------

CNewCfgDlg::CNewCfgDlg(char* pCfgFile, CWnd* pParent /*=NULL*/)
	: CDialog(CNewCfgDlg::IDD, pParent)
  , m_sCfgName(_T("<ConfigName>"))
  , m_sCfgFolderName(_T(DefCfgFolderName()))
  , m_sSummary1(_T("???????"))
  , m_sSummary2(_T("???????"))
  , m_sSummary3(_T("???????"))
  , m_PrjSetName(_T("<ProjectSet>"))
  , m_sDescription(_T("<Description of Configuration>"))
  , m_bPrjEqlName(false)
  {

  Strng BCfg(ProgFiles());
  BCfg.FnClearEndBSlash();
  //BCfg.FnDrivePath(BCfg());
  //BCfg.FnClearEndBSlash();
  BCfg.FnDrive();
  BCfg.Upper();
  BCfg+="\\SysCAD Projects";
  
  m_sRootFolder=ScdPFUser.RdStr("Folders", "NewCfgDlgRoot", BCfg());//StartupDirectory();

  SrcCfgFile = pCfgFile;
  bNewCfg = (pCfgFile==NULL || strlen(pCfgFile)==0) ? 1 : 0;
  //{{AFX_DATA_INIT(CNewCfgDlg)
	//}}AFX_DATA_INIT
  Strng Test,TestCfg;
  if (bNewCfg)
    {
    TestCfg = "NewCfg";
    Test = StartupDirectory();
    Test += TestCfg;
    int i=1;
    while (FileExists(Test()))
      {
      TestCfg.Set("NewCfg%i", i);
      Test.Set("%sNewCfg%i", StartupDirectory(), i);
      Test = StartupDirectory();
      Test += TestCfg;
      i++;
      }
	  //m_NewName = TestCfg();
    }
  else
    {
    Strng SrcCfgName,SrcCfgPath;
    SrcCfgName.FnName(SrcCfgFile());
    SrcCfgPath.FnDrivePath(SrcCfgFile());
    SrcCfgPath.FnCheckEndBSlash();
    TestCfg = "Copy of ";
    TestCfg += SrcCfgName();
    Test = SrcCfgPath();
    Test += TestCfg;
    Test += ".cfg";
    int i=1;
    while (FileExists(Test()))
      {
      TestCfg.Set("Copy%i of %s", i, SrcCfgName());
      Test = SrcCfgPath();
      Test += TestCfg;
      Test += ".cfg";
      i++;
      }
	  //m_NewName = TestCfg();
    }
  }

//---------------------------------------------------------------------------

void CNewCfgDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CNewCfgDlg)
  //}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_CFG_NAME, m_sCfgName);
  DDX_Text(pDX, IDC_CFG_FILESFOLDER, m_sCfgFolderName);
  DDX_Text(pDX, IDC_CFG_ROOT, m_sRootFolder);
  DDX_Text(pDX, IDC_SUMMARY2, m_sSummary2);
  DDX_Text(pDX, IDC_SUMMARY3, m_sSummary3);
  DDX_Text(pDX, IDC_CFG_PRJSET, m_PrjSetName);
  DDX_Check(pDX, IDC_FORCEEQLNAME, m_bPrjEqlName);
  DDX_Text(pDX, IDC_CFG_DESCRIP, m_sDescription);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CNewCfgDlg, CDialog)
  //{{AFX_MSG_MAP(CNewCfgDlg)
	ON_BN_CLICKED(IDC_CFG_BROWSE, OnBrowse)
	ON_EN_KILLFOCUS(IDC_CFG_ROOT, OnKillfocusCfg)
	ON_EN_KILLFOCUS(IDC_CFG_NAME, OnKillfocusCfg)
	ON_EN_CHANGE(IDC_CFG_ROOT, OnChangeFolder)
	ON_EN_CHANGE(IDC_CFG_NAME, OnChangeName)
  ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOK)
  ON_UPDATE_COMMAND_UI(IDC_CFG_BROWSE, OnUpdateOK)
  ON_STN_CLICKED(IDC_CFG_COPYNAME, OnStnClickedCfgCopyname)
  ON_EN_CHANGE(IDC_CFG_NAME, OnEnChangeCfgName)
  ON_EN_CHANGE(IDC_CFG_FILESFOLDER, OnEnChangeCfgFilesfolder)
  ON_BN_CLICKED(IDC_FORCEEQLNAME, OnBnClickedForceeqlname)
  ON_EN_KILLFOCUS(IDC_CFG_PRJSET, OnEnKillfocusCfgPrjset)
  ON_EN_KILLFOCUS(IDC_CFG_FILESFOLDER, OnEnKillfocusCfgFilesfolder)
  ON_EN_CHANGE(IDC_CFG_PRJSET, OnEnChangeCfgPrjset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CNewCfgDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  Strng s;
  if (bNewCfg)
    s = "Create new configuration";
  else
    s.Set("Create copy of %s", SrcCfgFile());
  SetDlgItemText(IDC_CFG_COPYNAME, s());
  ConstructPaths(false, true);
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CNewCfgDlg::ConstructPaths(bool Ld, bool Sv, bool StripHome)
  {
  if (Ld)
    UpdateData(TRUE);

  m_sCfgName.TrimRight('\\');
  m_sCfgFolderName.TrimRight('\\');
  if (StripHome)
    m_sRootFolder.TrimRight('\\');

  if (m_bPrjEqlName)
    m_PrjSetName=m_sCfgName;

  m_sCfgFile.Format("%s\\%s\\%s\\%s.cfg", m_sRootFolder, m_PrjSetName, m_sCfgFolderName, m_sCfgName);

  m_sCfgFiles.Format("%s\\%s\\%s", m_sRootFolder, m_PrjSetName, m_sCfgFolderName);

  m_sSummary1="?????";
  m_sSummary2=m_sCfgFile;
  m_sSummary3=m_sCfgFiles;

  GetDlgItem(IDC_CFG_PRJSET)->EnableWindow(!m_bPrjEqlName);
    
  m_bOK=m_sCfgName.GetLength()>0 && m_sCfgFolderName.GetLength()>0;

  if (m_sRootFolder.GetLength()==0 || m_sRootFolder.FindOneOf("/*\"<>|")>=0)
    m_bOK=false;
  if (m_PrjSetName.GetLength()==0 || m_PrjSetName.FindOneOf("\\/:*?\"<>|")>=0)
    m_bOK=false;
  if (m_sCfgFolderName.GetLength()==0 || m_sCfgFolderName.FindOneOf("\\/:*?\"<>|")>=0)
    m_bOK=false;
  if (m_sCfgName.GetLength()==0 || m_sCfgName.FindOneOf("\\/:*?\"<>|")>=0)
    m_bOK=false;
  //if (m_sDescription.FindOneOf("\\/:*?\"<>|")>=0)
  //  m_bOK=false;

  if (Sv)
    {
    UpdateData(FALSE);
    UpdateDialogControls(this, FALSE);
    }
  }

//---------------------------------------------------------------------------

void CNewCfgDlg::OnBrowse() 
  {
  UpdateData(TRUE);
  const char* pFName = NULL;
  CString FName;
  Strng Tmp, FPath;

  FPath = ScdPFUser.RdStr("Folders", "CfgSearchPath", m_sRootFolder);//*/StartupDirectory());
  if (m_sRootFolder.GetLength()>0)
    {
    char Drv[_MAX_DRIVE];
    char Dir[_MAX_DIR];
    char Name[_MAX_FNAME];
    char Ext[_MAX_EXT];
    _splitpath((const char*)m_sRootFolder, Drv, Dir, Name, Ext);
    FPath = Drv;
    FPath += Dir;
    FName = Name;
    FName += Ext;
    pFName = (const char*)FName;
    }
  BOOL Ok = TRUE;
  Tmp = FPath();
  Tmp.FnClearEndBSlash();
  if (!FileExists(Tmp()))
    {
    Strng Msg;
    Msg.Set("Create folder:\n%s", Tmp());
    Ok = (AfxMessageBox(Msg(), MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2)==IDYES);
    if (Ok)
      {
      Strng E;
      Ok = FnCreatePath(Tmp(), E);
      if (!Ok)
        LogWarning("SysCAD", 0, "Folder not created :\n%s\n%s", Tmp(), E());
      }
    }
  
  if (Ok)
    {
    USES_CONVERSION;
    char Buff[MAX_PATH];
    LPITEMIDLIST pidlSelected = NULL;
    BROWSEINFO bi = {0};
    LPMALLOC pMalloc = NULL;
    LPITEMIDLIST pidlRoot=NULL;//SHSimpleIDListFromPath(A2CW("D:\\"/*FPath()*/));

    SHGetMalloc(&pMalloc);

    bi.hwndOwner = m_hWnd;
    bi.pidlRoot = pidlRoot;
    bi.pszDisplayName = Buff;
    bi.lpszTitle = "Root Folder";
    bi.ulFlags = BIF_DONTGOBELOWDOMAIN|BIF_EDITBOX|BIF_NEWDIALOGSTYLE;
    bi.lpfn = NULL;
    bi.lParam = 0;

    pidlSelected = SHBrowseForFolder(&bi);

    BOOL PathOK=pidlSelected  && SHGetPathFromIDList(pidlSelected, Buff);

    if(pidlRoot)
      pMalloc->Free(pidlRoot);
    if(pidlSelected)
      pMalloc->Free(pidlSelected);

    pMalloc->Release();


    if (PathOK)
      m_sRootFolder = Buff;
    }
  ConstructPaths(false, true, true);
  }

//---------------------------------------------------------------------------

void CNewCfgDlg::OnKillfocusCfg() 
  {
  ConstructPaths(true, true);
  }

//---------------------------------------------------------------------------

void CNewCfgDlg::OnChangeName() 
  {
  ConstructPaths(true, true);
  }

//---------------------------------------------------------------------------

void CNewCfgDlg::OnChangeFolder() 
  {
  ConstructPaths(true, true, false);
  }

//---------------------------------------------------------------------------

void CNewCfgDlg::OnUpdateOK(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_bOK);
  }

void CNewCfgDlg::OnStnClickedCfgCopyname()
  {
  // TODO: Add your control notification handler code here
  }

void CNewCfgDlg::OnEnChangeCfgName()
  {
  ConstructPaths(true, true);
  }

void CNewCfgDlg::OnBnClickedInfilesfolder()
  {
  ConstructPaths(true, true);
  }

void CNewCfgDlg::OnEnChangeCfgFilesfolder()
  {
  ConstructPaths(true, true);
  }

void CNewCfgDlg::OnBnClickedForceeqlname()
  {
  ConstructPaths(true, true);
  }

void CNewCfgDlg::OnEnKillfocusCfgPrjset()
  {
  ConstructPaths(true, true);
  }

void CNewCfgDlg::OnEnKillfocusCfgFilesfolder()
  {
  ConstructPaths(true, true);
  }

void CNewCfgDlg::OnEnChangeCfgPrjset()
  {
  ConstructPaths(true, true);
  }

void CNewCfgDlg::OnOK()
  {
  UpdateData(true);

  ScdPFUser.WrStr("Folders", "NewCfgDlgRoot", m_sRootFolder);//StartupDirectory();

  CDialog::OnOK();
  };


//===========================================================================
//
//
//
//===========================================================================

CNewPrjDlg::CNewPrjDlg(char * PrjFile, BOOL ForNewPrj, BOOL ForEdit, DWORD LicCategory, CWnd* pParent /*=NULL*/)
  : CDialog(CNewPrjDlg::IDD, pParent)
  {
//ToolTips  pTTC = NULL;
  m_PrjFile = PrjFile;
  m_bForNewPrj = ForNewPrj;
  m_bForEdit = ForEdit;
  m_dwLicCategory = LicCategory;
  //{{AFX_DATA_INIT(CNewPrjDlg)
  m_NewGraph=True;
  m_NewTrend=True;
  m_DllPath = _T("");
  //}}AFX_DATA_INIT
//  pMdlList = 0;

#if WITHVISIODOC
  m_NewGraph=false;
  m_NewTrend=false;
#endif

  m_iModelCfg = -1;
  //m_BrowseFolder = StartupDirectory();
  m_BrowseFolder = BaseCfgFiles();
  if (m_BrowseFolder.FindI(BCfgFolderName())>=0)
    m_BrowseFolder = m_BrowseFolder.Left(m_BrowseFolder.FindI(BCfgFolderName()));

  m_DllPath = ScdPFMachine.RdStr("General", "DllPath", "");
  }

//--------------------------------------------------------------------------

CNewPrjDlg::~CNewPrjDlg()
  {
//ToolTips  delete pTTC;
  }

//---------------------------------------------------------------------------

void CNewPrjDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  // Append new

  //{{AFX_DATA_MAP(CNewPrjDlg)
	DDX_Control(pDX, IDC_MDLLIST2, m_MdlListC);
  DDX_Check(pDX, IDC_NEWGRFWNDS, m_NewGraph);
  DDX_Check(pDX, IDC_NEWTRENDWNDS, m_NewTrend);
	DDX_Text(pDX, IDC_DLLPATH, m_DllPath);
	//}}AFX_DATA_MAP

  Strng TPath;
  for (int i=m_MdlListC.GetItemCount(); i<m_FileArray.GetSize(); i++)
    {
    int Img=(m_ExistsArray[i] ? 0 : 2);
    int Pos=m_MdlListC.InsertItem(i, (const char*)(m_NameAndDescArray[i]), Img);
    TPath.FnDrivePath((char*)(const char*)(m_FileArray[i]));
    m_MdlListC.SetItemText(Pos, 1, TPath());
    m_MdlListC.SetItemData(Pos, i);
    }
  if (pDX->m_bSaveAndValidate)
    {
    ScdPFMachine.WrStr("General", "DllPath", (char*)(const char*)m_DllPath);
    SetDllFilesPath((char*)(const char*)m_DllPath);
    }

  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CNewPrjDlg, CDialog)
  //{{AFX_MSG_MAP(CNewPrjDlg)
	ON_BN_CLICKED(IDC_CFG_BROWSE, OnBrowse)
	ON_NOTIFY(NM_DBLCLK, IDC_MDLLIST2, OnDblclkMdllist2)
	ON_BN_CLICKED(IDC_CFG_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_EDITCFG, OnEditCfg)
	ON_BN_CLICKED(IDC_NEWCFG, OnNewCfg)
	ON_BN_CLICKED(IDC_COPYCFG, OnCopyCfg)
	//}}AFX_MSG_MAP
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
  ON_UPDATE_COMMAND_UI(IDC_CFG_REMOVE, OnUpdateButton)
  ON_UPDATE_COMMAND_UI(IDC_EDITCFG, OnUpdateButton)
  ON_UPDATE_COMMAND_UI(IDC_COPYCFG, OnUpdateButton)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CNewPrjDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
//ToolTips  pTTC = new CCustomToolTipCtrl(this);
  
  for (int i=0;;i++)
    {
    Strng NameI;
    NameI.Set("Cfg%02i", i);
    Strng Name(ScdPFUser.RdStr("Configuration", NameI(), ""));
    if (Name.GetLength()>0)
      {
      /*if (FileExists(Name()))  
        AddACfg(Name, false, false);
      else
        dbgpln("Cfg not found : %s", Name());*/
      AddACfg(Name, false, false);
      }
    else
      break;
    }

  const int FileArrayLen = m_FileArray.GetSize();
  for (int Cnt=0; Cnt<FileArrayLen; Cnt++)
    {
    dbgpln("Use %s", (const char *)m_NameAndDescArray[Cnt]);
    }
 
  CBitmap BM;                                           
  VERIFY(BM.LoadBitmap(IDB_SPLISTIMGS));
  VERIFY(ImgList.Create(12, 12, FALSE, 0, 3));
  VERIFY(ImgList.Add(&BM, (CBitmap*)NULL)>=0);
  m_MdlListC.SetImageList(&ImgList, LVSIL_SMALL);

  m_MdlListC.InsertColumn(0, "Configuration",  LVCFMT_LEFT, 10);
  m_MdlListC.InsertColumn(1, "Folder",           LVCFMT_LEFT, 10);
  m_MdlListC.SetExtendedStyle(m_MdlListC.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

  Strng TPath;
  for (i=0; i<FileArrayLen; i++)
    {
    int Img=(m_ExistsArray[i] ? 0 : 2);
    int Pos=m_MdlListC.InsertItem(i, (const char*)(m_NameAndDescArray[i]), Img);
    TPath.FnDrivePath((char*)(const char*)(m_FileArray[i]));
    m_MdlListC.SetItemText(Pos, 1, TPath());
    m_MdlListC.SetItemData(Pos, i);
    }

  int W=m_MdlListC.GetStringWidth("XXXXXXXXXXXXXXXXXXXXXXXXXX");

  int Col0Len=ScdPFUser.RdInt("NewPrjDlg", "Column0", -1);
  int Col1Len=ScdPFUser.RdInt("NewPrjDlg", "Column1", -1);

  if (Col0Len<0)
    {
    Col0Len=W;
    Col1Len=W;
    for (i=0; i<FileArrayLen; i++)
      {
      const char* p=(const char*)(m_NameAndDescArray[i]);
      Col0Len=Max(Col0Len, m_MdlListC.GetStringWidth((const char*)(m_NameAndDescArray[i]))+20);
      Col1Len=Max(Col1Len, m_MdlListC.GetStringWidth((const char*)(m_FileArray[i]))+20);
      }
    }

  m_MdlListC.SetColumnWidth(0, Col0Len);
  m_MdlListC.SetColumnWidth(1, Col1Len);

//  CString ModelCfg = ScdPFUser.RdStr("NewPrjDlg", "LastCfgFile", "");
  Strng ModelCfg = ScdPFUser.RdStr("NewPrjDlg", "LastCfgFile", "");
  if (ModelCfg.Length()==0)
    SelectItem(0);
  else
    {
    for (int i=0; i<m_FileArray.GetSize(); i++)
      {
//      CString File=m_MdlListC.GetItemText(i, 1);
      if (ModelCfg==m_FileArray[i])
        break;
      }

    LV_FINDINFO FindInfo;
    FindInfo.flags=LVFI_PARAM;    //see below
    FindInfo.lParam=i;
    int Pos=m_MdlListC.FindItem(&FindInfo, -1);
    SelectItem(Pos);
    }

  if (!m_bForNewPrj)
    {
    SetDlgItemText(IDC_CFG_SELECT_TXT, m_bForEdit ? "Select model configuration:" : "Select model configuration to be used (loading project):");
    SetWindowText(m_bForEdit ? "Project Configurations" : "Loading Project");
    CButton & NewGrf = *(CButton*)GetDlgItem(IDC_NEWGRFWNDS);
    CButton & NewTrnd = *(CButton*)GetDlgItem(IDC_NEWTRENDWNDS);
    CStatic & DllHead = *(CStatic*)GetDlgItem(IDC_DLLPATHHEAD);
    CEdit   & DllPath = *(CEdit*)GetDlgItem(IDC_DLLPATH);
    NewGrf.EnableWindow(FALSE);
    NewGrf.ShowWindow(SW_HIDE);
    NewTrnd.EnableWindow(FALSE);
    NewTrnd.ShowWindow(SW_HIDE);
    //Move Controls
    CRect RG, RH, RP;
    NewGrf.GetWindowRect(&RG);
    DllHead.GetWindowRect(&RH);
    DllPath.GetWindowRect(&RP);
    ScreenToClient(&RG);
    ScreenToClient(&RH);
    ScreenToClient(&RP);

    RH.left=RG.left;
    RP.left=RG.left;
    DllHead.MoveWindow(&RH);
    DllPath.MoveWindow(&RP);
    }
  else
    {
    SetDlgItemText(IDC_CFG_SELECT_TXT, "Select model configuration to be used (creating new project):");
    CStatic & DllHead = *(CStatic*)GetDlgItem(IDC_DLLPATHHEAD);
    CEdit   & DllPath = *(CEdit*)GetDlgItem(IDC_DLLPATH);
    DllPath.EnableWindow(FALSE);
    DllPath.ShowWindow(SW_HIDE);
    DllHead.EnableWindow(FALSE);
    DllHead.ShowWindow(SW_HIDE);
    }

  UpdateData(FALSE);

  if (m_PrjFile())
    {
    Strng T;
    T.Set("Loading %s", m_PrjFile());
    SetWindowText(T());
    }
  else
    {
    if (m_bForNewPrj)
      SetWindowText("Creating a New Project");
    }

  UpdateDialogControls(this, FALSE);

  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CNewPrjDlg::PreTranslateMessage(MSG* pMsg) 
  {
//ToolTips  if (pTTC && HelpMngr.ShowToolTips())
//ToolTips    pTTC->RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CNewPrjDlg::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
//ToolTips  if (pTTC && HelpMngr.ShowToolTips())
//ToolTips    return pTTC->OnToolTipNotify(pNMHDR, CNewPrjDlg::IDD);
  return FALSE;
  }

//---------------------------------------------------------------------------

void CNewPrjDlg::SelectItem(int Pos)
  {
  Pos = Range(0, Pos, m_MdlListC.GetItemCount()-1);
  m_MdlListC.SetItemState(Pos, -1, LVIS_SELECTED);
  m_MdlListC.EnsureVisible(Pos, FALSE);
  }

//---------------------------------------------------------------------------

void CNewPrjDlg::OnOK() 
  {
  UpdateData(TRUE);

  int iList = m_MdlListC.GetNextItem(-1, LVNI_SELECTED);
  if (iList==LB_ERR)
    iList=-1;

  CString NameDesc;
  CString File;
  if (iList>=0)
    {
    NameDesc = m_MdlListC.GetItemText(iList, 0);
    File=m_MdlListC.GetItemText(iList, 1);
    }

  // Because of sort Find original index
  Strng Path;
  m_iModelCfg=-1;
  for (int i=0; i<m_NameAndDescArray.GetSize(); i++)
    {
    Path.FnDrivePath((char*)(const char*)m_FileArray[i]);
    if (NameDesc==m_NameAndDescArray[i] && Path==File)
      {
      m_iModelCfg=i;
      break;
      }
    }

  for (i=0; i<m_FileArray.GetSize(); i++)
    {
    Strng NameI;
    NameI.Set("Cfg%02i", i);
    ScdPFUser.WrStr("Configuration", NameI(), (const char*)m_FileArray[i]);
    }

  for ( ; ; i++)
    {
    Strng NameI;
    NameI.Set("Cfg%02i", i);
    Strng Name(ScdPFUser.RdStr("Configuration", NameI(), "::::"));
    if (Name!="::::")
      ScdPFUser.WrStr("Configuration", NameI(), "");
    else
      break;
    }

  ScdPFUser.WrStr("NewPrjDlg", "LastCfgFile", m_iModelCfg >=0 ? (const char*)m_FileArray[m_iModelCfg] : "");
  
  ScdPFUser.WrInt("NewPrjDlg", "Column0", m_MdlListC.GetColumnWidth(0));
  ScdPFUser.WrInt("NewPrjDlg", "Column1", m_MdlListC.GetColumnWidth(1));
  
  CDialog::OnOK();
  }

//---------------------------------------------------------------------------

int CNewPrjDlg::AddACfg(Strng & FileName, flag AllowConvert, flag DoDllLoad)
  {
  Strng ACfg;
  ACfg.FnName(FileName());
  
  for (int i=0;  i<m_FileArray.GetSize(); i++)
    if (FileName.XStrICmp(m_FileArray[i])==0)
      return i;

  flag FExists = FileExists(FileName());

  if (FExists)
    {
    CModelInfoUpgrade InfoU;
    if (AllowConvert)
      InfoU.CheckCfgFileFormat(FileName(), false);
    }

  bool CfgInfoOK = true;
  CModelInfo Info;
  if (FExists)
    {
    CfgInfoOK = (Info.GetCfgInfo(FileName(), DoDllLoad, false)>=2);
    }
  if (CfgInfoOK)
    {
    m_NameArray.Add(ACfg());
    m_DescArray.Add(Info.m_sDesc());
    if (Info.m_sDesc.Length()>0)
      {
      ACfg+=" (";
      ACfg+=Info.m_sDesc;
      ACfg+=")";
      }
    m_NameAndDescArray.Add(ACfg());
    m_ExistsArray.Add(FExists);
    int i=m_FileArray.Add(FileName());
    return i;
    }
  return -1;
  }

//---------------------------------------------------------------------------

void CNewPrjDlg::OnDblclkMdllist2(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  //m_bForNewPrj
  if (m_bForEdit)
    OnEditCfg();
  else
    OnOK();	
	*pResult = 0;
  }

//---------------------------------------------------------------------------

int CNewPrjDlg::TryAddUpdateCfg(char* pCfgFile)
  {
  Strng CfgFile(pCfgFile);
  Strng Name, DataLibPath, BasePath, ConfigPath, E;
  Name.FnNameExt(CfgFile());
  if (Name.XStrICmp("models.cfg")==0)
    {
    DataLibPath.FnDrivePath(CfgFile());
    DataLibPath.FnClearEndBSlash();
    Name.FnName(DataLibPath());
    if (Name.XStrICmp("data_lib")==0)
      {
      BasePath.FnDrivePath(DataLibPath());
      BasePath.FnClearEndBSlash();
      Name.FnName(BasePath());

      CStringDlg CU("Configuration Upgrade", "Name:", Name(), this);
      
      if (CU.DoModal()==IDOK)
        {
        Strng NewCfgFile(BasePath);
        NewCfgFile.FnCheckEndBSlash();
        NewCfgFile+=CU.Data();
        NewCfgFile.FnCheckExtension("cfg");
        if (MoveFile(CfgFile(), NewCfgFile()))
          {
          CfgFile=NewCfgFile;

          ConfigPath=BasePath;
          //INCOMPLETECODE1("CfgFolderName ???")
          ConfigPath.FnMakeDataFolder(DefCfgFolderName()); // this will be the old style cfg and hence CfgFiles is OK
          ConfigPath.FnClearEndBSlash();
          if (MoveFile(DataLibPath(), ConfigPath()))
            {}
          else
            {
            E.GetSystemErrorStr(GetLastError());
            LogError("SysCAD", LF_Exclamation, 
              "Config folder not renamed :\n%s\n%s",ConfigPath(),E());
            }
          }
        else
          {
          E.GetSystemErrorStr(GetLastError());
          LogError("SysCAD", LF_Exclamation, 
            "Config file not moved :\n%s\n%s",CfgFile(),E());
          }
        }
      }
    }
  
  int Index=CNewPrjDlg::AddACfg(CfgFile, true/*false*/, true);  //CNM
  if (Index<0)
    {
    LogWarning("SysCAD", 0, "Configuration not Added");
    }
  return Index;
  }

//---------------------------------------------------------------------------

int CNewPrjDlg::FindOrigIndex(CString& Cfg, CString& File)
  {// Because of sort Find original index...
  Strng Path;
  for (int i=0; i<m_FileArray.GetSize(); i++)
    {
    Path.FnDrivePath((char*)(const char*)m_FileArray[i]);
    if (Cfg==m_NameAndDescArray[i] && Path==File)
      return i;
    }
  return -1;
  }

//---------------------------------------------------------------------------

int CNewPrjDlg::RemoveACfg(int iCfg)
  {
  ASSERT(iCfg>=0);
  CString File, Cfg;
  Cfg = m_MdlListC.GetItemText(iCfg, 0);
  File = m_MdlListC.GetItemText(iCfg, 1);

  int jCfg = FindOrigIndex(Cfg, File);
  
  if (jCfg>=0)
    {
    m_MdlListC.DeleteItem(iCfg);

    m_NameArray.RemoveAt(jCfg);
    m_DescArray.RemoveAt(jCfg);
    m_NameAndDescArray.RemoveAt(jCfg);
    m_ExistsArray.RemoveAt(jCfg);
    m_FileArray.RemoveAt(jCfg);
    return jCfg;
    }
  return -1;
  }

//---------------------------------------------------------------------------

void CNewPrjDlg::OnRemove() 
  {
  int iCfg = m_MdlListC.GetNextItem(-1, LVNI_SELECTED);
  if (iCfg==LB_ERR)
    return;

  if (RemoveACfg(iCfg)>=0 && m_MdlListC.GetItemCount()>0)
    SelectItem(iCfg);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CNewPrjDlg::OnBrowse() 
  {
  Strng CfgFile, DataLibPath, BasePath, ConfigPath, Name, E;
  
  Strng InitDir(ScdPFUser.RdStr("Folders", "CfgSearchPath", m_BrowseFolder()/*StartupDirectory()*/));
  
  CSCDFileDialog Dlg(true, NULL, "*.cfg", 
    OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, 
    "SysCAD Configurations(*.cfg)|*.cfg||", this);
  Dlg.m_ofn.lpstrInitialDir = InitDir();
  Dlg.m_ofn.lpstrTitle = "Browse for Configurations";
  if (Dlg.DoModal()==IDOK)
    CfgFile=Dlg.GetPathName();
  
  if (CfgFile())
    {
    m_BrowseFolder.FnDrivePath(CfgFile());
    ScdPFUser.WrStr("Folders", "CfgSearchPath", m_BrowseFolder());
    TryAddUpdateCfg(CfgFile());

    UpdateData(FALSE);

    Strng Path;
    for (int i=0; i<m_FileArray.GetSize(); i++)
      {
      if (CfgFile==m_FileArray[i])
        {
        LV_FINDINFO FindInfo;
        FindInfo.flags=LVFI_PARAM;
        FindInfo.lParam=i;
        int Pos=m_MdlListC.FindItem(&FindInfo, -1);
        SelectItem(Pos);
        break;
        }
      }
    }
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CNewPrjDlg::OnEditCfg()
  {
  int iCfg = m_MdlListC.GetNextItem(-1, LVNI_SELECTED);
  if (iCfg==LB_ERR)
    return;

  CString File, Cfg;
  Cfg = m_MdlListC.GetItemText(iCfg, 0);
  File = m_MdlListC.GetItemText(iCfg, 1);

  int jCfg = FindOrigIndex(Cfg, File);
  
  if (jCfg>=0)
    {
    if (!m_ExistsArray[jCfg])
      return;
    Strng FileName(m_FileArray[jCfg]);

    DWORD Att;
    if (FileAttributes(FileName(), Att) && (Att & FILE_ATTRIBUTE_READONLY))
      {
      AfxMessageBox("Configuration file is READONLY\n\nEdit Cancelled", MB_ICONINFORMATION|MB_OK);
      UpdateDialogControls(this, FALSE);
      return;
      }
    CModelInfoUpgrade InfoU;
    int Fmt = InfoU.CheckCfgFileFormat(FileName(), false);
    BOOL OldEdit = (Fmt==0);
    if (OldEdit)
      {
      HANDLE hCfgEditProcess = NULL;
      //FileName.FnCheckExtension("cfg");
      FileName.FnSearchExpand();
      Strng NPad=TxtEditCmdString(FileName());
      STARTUPINFO si;
      memset(&si, 0, sizeof(si));
      si.cb = sizeof(si);
      si.wShowWindow = SW_SHOWDEFAULT;
      PROCESS_INFORMATION pi;
      if (CreateProcess(NULL, NPad(), NULL, NULL, FALSE, 0, NULL, PrjFiles(), &si, &pi))
        {
        hCfgEditProcess = pi.hProcess;
        //dwCfgEditProcessId = pi.dwProcessId;
        }
      }
    else
      {
      CWaitCursor Wait;
      AfxGetMainWnd()->BringWindowToTop();
      ShowWindow(SW_HIDE);
      CDlgBusy::Open("\nEdit Configuration");
      CDlgBusy::SetLine(3, FileName());
      CMdlCfgSheet CfgEdit(m_dwLicCategory, FileName(), "", AfxGetMainWnd(), 0);
      BOOL Ok = (CfgEdit.pCfgPage!=NULL);
      if (Ok)
        Ok = (CfgEdit.DoModal()==IDOK);
      CDlgBusy::Close();
      ShowWindow(SW_NORMAL);
      if (Ok)
        {
        RemoveACfg(iCfg);
        jCfg = AddACfg(FileName, true, false);
        m_MdlListC.DeleteAllItems();
        UpdateData(FALSE);
        LV_FINDINFO FindInfo;
        FindInfo.flags=LVFI_PARAM;
        FindInfo.lParam=jCfg;
        int Pos=m_MdlListC.FindItem(&FindInfo, -1);
        SelectItem(Pos);
        }
      }
    }
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CNewPrjDlg::DoNewCfg(char* pSrcCfgFile)
  {
  CNewCfgDlg Dlg(pSrcCfgFile, this);
  if (Dlg.DoModal()==IDOK)
    {
    Strng Msg;
    Strng NewCfgFile=((char*)(const char*)Dlg.m_sCfgFile);
    Strng NewCfgFiles=((char*)(const char*)Dlg.m_sCfgFiles);
    Strng Desc=((char*)(const char*)Dlg.m_sDescription);
    BOOL Ok = (NewCfgFile.Len()>0);
    if (Ok)
      {
      NewCfgFile.FnCheckExtension("cfg");
      NewCfgFile.FnSearchExpand();
      if (FileExists(NewCfgFile()))
        {
        Msg.Set("Model configuration file allready exists!  Overwrite?\n\n%s", NewCfgFile());
        Ok = (AfxMessageBox(Msg(), MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2)==IDYES);
        if (Ok)
          DeleteFile(NewCfgFile());
        }
      }
    if (Ok)
      {
      Strng DstFold;
      DstFold.FnDrivePath(NewCfgFile());
      DstFold.FnClearEndBSlash();
      if (!FileExists(DstFold()))
        {//create destination folder...
        Strng E;
        if (FnCreatePath(DstFold(), E))
          LogNote("SysCAD", 0, "New folder created :\n%s", DstFold());
        else
          LogError("SysCAD", LF_Exclamation,
                 "Folder not created :\n%s\n%s", DstFold(), E());
        }

      // Make Relative Names
      Strng NewCfgFilePath(NewCfgFile);
      NewCfgFilePath.FnDrivePath();
      NewCfgFilePath.FnCheckEndBSlash();

      Strng NewCfgFilesRel(NewCfgFiles);
      NewCfgFilesRel.FnCheckEndBSlash();
      NewCfgFilesRel.FnMakeFileRelative(NewCfgFilePath(), false);
      FnContractPath(NewCfgFilesRel, NULL, NewCfgFilePath(), "$CfgFilePath\\", false);


      if (pSrcCfgFile)
        {//copy from existing cfg file...
        if (FileExists(pSrcCfgFile))
          {
          ::CopyFile(pSrcCfgFile, NewCfgFile(), true);
          CProfINIFile pf(NewCfgFile());
          Strng NewDesc(pSrcCfgFile);
          NewDesc.FnName();
          NewDesc += " projects";
          Msg = pf.RdStr("General", "Description", NewDesc());
          NewDesc = "Copy of ";
          NewDesc += Msg;
          pf.SetProfFilename(NewCfgFile());
          pf.WrStr("General", "Description", NewDesc());
          pf.WrLong("General", "PrjFileVersion", PrjFileVerNo());
          }
        else
          {
          Msg.Set("Model configuration source file does not exist!\n\n%s", pSrcCfgFile);
          AfxMessageBox(Msg(), MB_ICONSTOP);
          Ok = false;
          }
        }
      else
        {
        CProfINIFile pf(NewCfgFile());
        pf.WrStr("General", "Description", "xxx");
        if (!FileExists(NewCfgFile()))
          {
          Msg.Set("Unable to create model configuration file!\n\n%s", NewCfgFile());
          AfxMessageBox(Msg(), MB_ICONSTOP);
          Ok = false;
          }
        else
          {
          CModelInfo::MakeDefaultNewCfgFile(NewCfgFile(), NewCfgFilesRel(), Desc());
          }
        }   

      if (!FileExists(NewCfgFiles()))
        {
        Strng E;
        if (FnCreatePath(NewCfgFiles(), E))
          LogNote("SysCAD", 0, "New folder created :\n%s", NewCfgFiles());
        else
          LogError("SysCAD", LF_Exclamation,
                 "Folder not created :\n%s\n%s", NewCfgFiles(), E());
        }

      Strng DstCfgDB = NewCfgFiles();
      DstCfgDB += "\\";
      DstCfgDB += CfgDBFileName();
      if (!FileExists(DstCfgDB()))
        {//create (or copy) mdb database...
        bool DoCreateMDB = true;
        if (pSrcCfgFile)
          {//try copy original specie mdb database
          CProfINIFile OldCfg(pSrcCfgFile);
          Strng OldCfgFiles = OldCfg.RdStr("General", "CfgFiles", "");
          long OldCfgFilePrjFileVerNo=OldCfg.RdLong("General", "PrjFileVersion", -1);

          if (OldCfgFiles())
            {
            Strng OldCfgFullPath;
            OldCfgFullPath.FnDrivePath(pSrcCfgFile);
            ::FnExpandPath(OldCfgFiles, NULL, OldCfgFullPath(), "$CfgFilePath\\");
            }
          else
            {
            OldCfgFiles.FnDrivePath(pSrcCfgFile);
            OldCfgFiles.FnCheckEndBSlash();
            OldCfgFiles+=DefCfgFolderName();
            OldCfgFiles+="\\";
            }

          Strng SrcCfgDB;
          SrcCfgDB = OldCfgFiles;
          SrcCfgDB += CfgDBFileName();
          if (FileExists(SrcCfgDB()))
            {
            DoCreateMDB = false;
            ::CopyFile(SrcCfgDB(), DstCfgDB(), true);
            LogNote("SysCAD", 0, "Copy database from:\n%s", SrcCfgDB());
            }
          }
        if (DoCreateMDB)
          {
          if (CSCDDatabase::DoSDBCreate(DstCfgDB()))
            LogNote("SysCAD", 0, "Created database :\n%s", DstCfgDB());
          else
            LogError("SysCAD", LF_Exclamation, "Database not created :\n%s", DstCfgDB());
          }
        }
      }
    if (Ok)
      {
      if (TryAddUpdateCfg(NewCfgFile())>=0)
        {
        Strng DstFold,E;
        if (!FileExists(NewCfgFiles()) && !FnCreatePath(NewCfgFiles(), E))
          {
          LogError("SysCAD", LF_Exclamation,"Folder not created :\n%s\n%s", NewCfgFiles(), E());
          }
        }

      UpdateData(FALSE);

      Strng Path;
      for (int i=0; i<m_FileArray.GetSize(); i++)
        {
        if (NewCfgFile==m_FileArray[i])
          {
          LV_FINDINFO FindInfo;
          FindInfo.flags=LVFI_PARAM;
          FindInfo.lParam=i;
          int Pos=m_MdlListC.FindItem(&FindInfo, -1);
          SelectItem(Pos);
          break;
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CNewPrjDlg::OnNewCfg() 
  {
  DoNewCfg(NULL);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CNewPrjDlg::OnCopyCfg() 
  {
  int iCfg = m_MdlListC.GetNextItem(-1, LVNI_SELECTED);
  Strng FileName;
  if (iCfg!=LB_ERR)
    {
    CString File, Cfg;
    Cfg = m_MdlListC.GetItemText(iCfg, 0);
    File = m_MdlListC.GetItemText(iCfg, 1);
    int jCfg = FindOrigIndex(Cfg, File);
  
    if (jCfg>=0 && m_ExistsArray[jCfg])
      {
      FileName = m_FileArray[jCfg];
      }
    }

  DoNewCfg(FileName());
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

static char *ExcludeDir[]=
  {
  ".",
  "..",
  BackupFilesFolderName(),
  "$Old",
  NULL
  };

void BuildFolderList(char* pRoot, int Depth, CStringArray& List)
  {
  WIN32_FIND_DATA fd;
  CString Root = pRoot;
  if (Root.GetLength()==0 || Root[Root.GetLength()-1]!='\\')
    Root += '\\';
  Strng s(Root);
  s += "*.*";
  HANDLE H = FindFirstFile(s(), &fd);
  flag AllDone = (H==INVALID_HANDLE_VALUE);
  while (!AllDone)
    {
    if (fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
      {
      for (int i=0; ExcludeDir[i]; i++)
        if (_stricmp(fd.cFileName, ExcludeDir[i])==0)
          break;
      if (ExcludeDir[i]==NULL)
        {
        CString fn = Root;
        fn += fd.cFileName;
        List.Add(fn);
        if (Depth>0)
          BuildFolderList((char*)(const char*)fn, Depth-1, List);
        }
      }
    AllDone = !FindNextFile(H, &fd);
    }
  FindClose(H);
  }

//---------------------------------------------------------------------------

void CNewPrjDlg::OnUpdateButton(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_MdlListC.GetItemCount()>0 && m_MdlListC.GetNextItem(-1, LVNI_SELECTED)!=LB_ERR);
  //pCmdUi->Enable(m_iModelCfg>=0 && m_ExistsArray[m_iModelCfg]);
  }

//---------------------------------------------------------------------------
//===========================================================================
//
//
//
//===========================================================================

int CModelInfo::CfgInfoFmt(char * FileName)
  {
  CProfINIFile Cfg(FileName);
  
  if (!FileExists(FileName))
    return -1;

  char Buff[16];
  if (Cfg.RdSection("General", Buff, sizeof(Buff))>0)
    return 1;
  return 0;
  }

//---------------------------------------------------------------------------

inline char * NextActualToken(char * Skip, Strng & CurTkn, CTokenFile &Tkns)
  {
  CurTkn=Tkns.NextToken();
  if (Skip && CurTkn.XStrICmp(Skip)==0)
    CurTkn=Tkns.NextToken();
  return CurTkn();
  }

// --------------------------------------------------------------------------

inline flag ListContinues(Strng & CurTkn, CTokenFile &Tkns)
  {
  CurTkn=Tkns.NextToken();
  if (CurTkn.XStrICmp(",")==0)
    {
    CurTkn=Tkns.NextToken();
    return true;
    }
  return false;
  }

// --------------------------------------------------------------------------

flag CheckEqnParam(Strng & CurTkn, CTokenFile &Tkns, Strng &Param)
  {
  CurTkn=Tkns.NextToken();
  if (CurTkn.XStrICmp("=")==0) // for TSpline Sig=1 Etc
    {
    Param+="=";
    Param+=Tkns.NextToken();
    return true;
    }
  else
    Tkns.SetReturnSameToken();
  return false;
  }

// --------------------------------------------------------------------------

static void inline WRCFG(CProfINIFile &Cfg, char*A,char*B,int N,char*V)
  {
  Strng S;
  S.Set("%s%04i",B,N);
  Cfg.WrStr(A, S(), V);
  };

static void inline WRCFG(CProfINIFile &Cfg, char*A,char*B,int N, char*C, char*V)
  {
  Strng S;
  S.Set("%s%02i.%s",B,N,C);
  Cfg.WrStr(A, S(), V);
  };

static void inline WRCFG(CProfINIFile &Cfg, char*A,char*B,int N, char*C, int M,char*V)
  {
  Strng S;
  S.Set("%s%02i.%s%03i",B,N,C,M);
  Cfg.WrStr(A, S(), V);
  };

static void inline WRCFG4(CProfINIFile &Cfg, char*A,char*B,int N, char*C, char*V)
  {
  Strng S;
  S.Set("%s%04i.%s",B,N,C);
  Cfg.WrStr(A, S(), V);
  };

static void inline WRCFG4(CProfINIFile &Cfg, char*A,char*B,int N, char*C, int M,char*V)
  {                     
  Strng S;
  S.Set("%s%04i.%s%03i",B,N,C,M);
  Cfg.WrStr(A, S(), V);
  };

/*
Size_Series: "Cleveland", mm, 0.0,0.03,0.05,0.106,0.3,0.6,0.85,1.18,1.7,3.35,10.0,25.0; 
Size_Distribution: "Cleveland" 
  Series = "Cleveland", BottomSizeDisp=0.0 mm, TopSizeDisp=65.0 mm
  Species = KCl, NaCl{Default}, InSols

becomes
  ["SizeDistributions"]
  Distribution_Graphics=1
  Partition_Graphics=1
  Intervals_Ascending=0
  Series0000=Tyler_Series
  Series01.Name=Cleveland,mm
  Series01.Pt000=0.0
  Series1003=0.03
  Series1004=0.05
  Series1005=0.106
  Series1006=0.3
  Series1007=0.6
  Series1008=0.85
  Series1009=1.18
  Series1010=1.7
  Series1011=3.35
  Series1012=10.0
  Series1013=25.0 
  'Series0001=Cleveland,mm,0.0,0.03,0.05,0.106,0.3,0.6,0.85,1.18,1.7,3.35,10.0,25.0 
  Distribution01.Name=Cleveland
  Distribution01.Series=Cleveland,mm,0,1,2,3,
  Distribution01.Species=KCl,NaCl{Default},InSols
//Size_Distribution: "OreSize" 
  Series="Tyler", BottomSize= 100 um, BottomSizeDisp= 10 um, TopSize =100000 um, TopSizeDisp =1000000 um
  Species=Ore{MgCO3,Default}, Ore1 {MgSO4}
;  Species=Ore
  Grade=Au
  BondWI=15,14,10,10,10,11,10,10,10;,End
  ;//Leachability=
          
      
*/
static void UpgradeSeries(CProfINIFile &Cfg, Strng & CurTkn, CTokenFile &Tkns, int SerNo)
  {
  if (CurTkn.XStrICmp(":")==0)
    CurTkn=Tkns.NextToken();
  Strng SeriesName(CurTkn);
  NextActualToken(",", CurTkn, Tkns);
  Strng CnvStr(CurTkn);
  NextActualToken(",", CurTkn, Tkns);

  SeriesName.LRTrim();
  CnvStr.LRTrim();
  WRCFG(Cfg, "SizeDistributions", "Size", SerNo, "Name", SeriesName());
  WRCFG(Cfg, "SizeDistributions", "Size", SerNo, "Cnvs", CnvStr());

  int n=0;
  while (!Tkns.AtEOF() && (Tkns.GetCurTokenType()==TokNumeric))
    {
    Strng S=CurTkn();
    S.LRTrim();
    WRCFG(Cfg, "SizeDistributions", "Size", SerNo, "Pt",n, S());
    if (!ListContinues(CurTkn, Tkns))
      break;
    n++;
    }
  }

// --------------------------------------------------------------------------

static void UpgradeTyler(CProfINIFile &Cfg, Strng & CurTkn, CTokenFile &Tkns, int SerNo)
  {

  static double TylerSeriesInit[] = 
    { 
      1.66, 2.23, 3.31, 4.69, 6.63, 9.37, 13.26, 
      18.75,  26.52, 37.5, 53.03, 75,
      106.07, 150, 212.13, 300, 424.3, 600,
      848.53, 1200, 1697, 2400, 3394, 4800, 6788,
      9600, 13576, 19200, 27153, 38400, 54306, 76800,
      108612, 153600, 217223, 307200, 434446, 614400
    };

  if (CurTkn.XStrICmp(":")==0)
    CurTkn=Tkns.NextToken();
  Strng SeriesName(CurTkn);
  NextActualToken(",", CurTkn, Tkns);
  Strng CnvStr(CurTkn);
  NextActualToken(",", CurTkn, Tkns);

  SeriesName.LRTrim();
  CnvStr.LRTrim();
  WRCFG(Cfg, "SizeDistributions", "Size", SerNo, "Name", SeriesName());
  WRCFG(Cfg, "SizeDistributions", "Size", SerNo, "Cnvs", CnvStr());
  
  CDataCnv *pC=gs_CnvsMngr.FindSecCnv(DC_L, CnvStr());
  if (!pC)
    {
    }

  double LoLim=0.0;
  double HiLim=1.0e6;
  int i=0;
  while (i<2 && !Tkns.AtEOF() && (Tkns.GetCurTokenType()==TokNumeric))
    {
    double D=SafeAtoF(CurTkn());
    if (pC)
      D=pC->Normal(D, CnvStr());
    if (i==0)
      LoLim=D;
    else if (i==1)
      HiLim=D;
    if (!ListContinues(CurTkn, Tkns))
      break;
    i++;
    }

  int n=0;
  int N=sizeof(TylerSeriesInit)/sizeof(TylerSeriesInit[0]);
  for (i=0; i<N; i++)
    {
    double I=TylerSeriesInit[i]*1.0e-6;
    if (I>=LoLim && I<HiLim)
      {
      if (pC)
        I=pC->Human(I, CnvStr());
      Strng S;
      S.Set("%.6g",I);
      WRCFG(Cfg, "SizeDistributions", "Size", SerNo, "Pt",n, S());
      //WRCFG(Cfg, "SizeDistributions", "Series", SerNo*1000+1+n, S());
      n++;
      }
    }
  }

// --------------------------------------------------------------------------

static void UpgradeDistribution(CProfINIFile &Cfg, Strng & CurTkn, CTokenFile &Tkns, int DistNo)
  {
  if (CurTkn.XStrICmp(":")==0)
    CurTkn=Tkns.NextToken();
  Strng DistName(CurTkn);

  Strng SeriesName;
  Strng BottomSize;
  Strng TopSize;
  Strng BottomSizeDisp;
  Strng TopSizeDisp;
  Strng DefaultSpecie;
  CSArray Species;
  CSArray Columns;

  //int n=0;
  while (!Tkns.AtEOF())
    {
    Strng What=NextActualToken(NULL, CurTkn, Tkns);
    if (What.XStrICmp("Series")==0)
      {
      NextActualToken("=", CurTkn, Tkns);
      SeriesName=CurTkn;
      while (ListContinues(CurTkn, Tkns))
        {
        if (CurTkn.XStrICmp("BottomSize")==0)
          {
          NextActualToken("=", CurTkn, Tkns);
          BottomSize=CurTkn(); // Value
          CurTkn=Tkns.NextToken();
          BottomSize+=" ";
          BottomSize+=CurTkn(); // CnvStr
          }
        else if (CurTkn.XStrICmp("TopSize")==0)
          {
          NextActualToken("=", CurTkn, Tkns);
          TopSize=CurTkn(); // Value
          CurTkn=Tkns.NextToken();
          TopSize+=" ";
          TopSize+=CurTkn(); // CnvStr
          }
        else if (CurTkn.XStrICmp("BottomSizeDisp")==0)
          {
          NextActualToken("=", CurTkn, Tkns);
          BottomSizeDisp=CurTkn(); // Value
          CurTkn=Tkns.NextToken();
          BottomSizeDisp+=" ";
          BottomSizeDisp+=CurTkn(); // CnvStr
          }
        else if (CurTkn.XStrICmp("TopSizeDisp")==0)
          {
          NextActualToken("=", CurTkn, Tkns);
          TopSizeDisp=CurTkn(); // Value
          CurTkn=Tkns.NextToken();
          TopSizeDisp+=" ";
          TopSizeDisp+=CurTkn(); // CnvStr
          }
        }
      Tkns.SetReturnSameToken(true);
      }
    else if (What.XStrICmp("Species")==0)
      {
      NextActualToken("=", CurTkn, Tkns);
      int N=0, Done=false;
      Strng S;
      while (!Tkns.AtEOF() && !Done)
        {
        if (CurTkn.XStrICmp("Default")==0)
          S="$Default$";
        else
          S=CurTkn();
        if (!ListContinues(CurTkn, Tkns))
          {
          if (CurTkn.XStrICmp("{")==0)
            {
            NextActualToken("{", CurTkn, Tkns);
            while (!Tkns.AtEOF() && !Done)
              {
              S+=",";
              if (CurTkn.XStrICmp("Default")==0)
                S+="$Default$";
              else
                S+=CurTkn();
              if (!ListContinues(CurTkn, Tkns)) // Skip ,/}
                {
                if (!ListContinues(CurTkn, Tkns))
                  Done=true;
                break;
                }
              }
            }
          else
            Done=true;
          }
        Species.Add(S);
        N++;
        }
      Tkns.SetReturnSameToken(true);
      }
    else if (What.XStrICmp("Grade")==0)
      {
      NextActualToken("=", CurTkn, Tkns);
      while (!Tkns.AtEOF())
        {
        // CurTkn();
        // Turf these
        if (!ListContinues(CurTkn, Tkns))
          {
          Tkns.SetReturnSameToken(true);
          break;
          }
        }
      }
    else if (What.XStrICmp("BondWI")==0)
      {
      NextActualToken("=", CurTkn, Tkns);
      while (!Tkns.AtEOF())
        {
        // CurTkn();
        // Turf these
        if (!ListContinues(CurTkn, Tkns))
          {
          Tkns.SetReturnSameToken(true);
          break;
          }
        }
      }
    else if (What.XStrICmp("Columns")==0)
      {
      CurTkn=Tkns.NextToken();
      while (!Tkns.AtEOF())
        {
        if (CurTkn.XStrICmp("End")==0)
          {
          CurTkn=Tkns.NextToken();
          break;
          }
        else if (CurTkn.XStrICmp("Columns")==0)
          {
          CurTkn=Tkns.NextToken();
          if (CurTkn.XStrICmp(":")==0)
            CurTkn=Tkns.NextToken(); // SkipEnd
          //break;
          }
        else
          {
          Strng ColName, S;
          int ColOpt=0;
          ColName=CurTkn();
          NextActualToken("=", CurTkn, Tkns);
          ColOpt=SafeAtoI(CurTkn());
          CurTkn=Tkns.NextToken();
          S.Set("%i,%s", ColOpt, ColName());
          Columns.Add(S);
          }
        if (CurTkn.XStrICmp(",")==0)
          CurTkn=Tkns.NextToken();
        }
      Tkns.SetReturnSameToken(true);
      }
    else
      break;
    }

  WRCFG(Cfg, "SizeDistributions", "Dist", DistNo, "Name", DistName());

  //Strng S;
  //S.Set("%s,%s,%s,%s,%s", 
  //      SeriesName(),
  //      BottomSize()?BottomSize():"",
  //      TopSize()?TopSize():"",
  //      BottomSizeDisp()?BottomSizeDisp():"",
  //      TopSizeDisp()?TopSizeDisp():"");
  WRCFG(Cfg, "SizeDistributions", "Dist", DistNo, "Series",    SeriesName());
  WRCFG(Cfg, "SizeDistributions", "Dist", DistNo, "BSize",     BottomSize()?BottomSize():"");
  WRCFG(Cfg, "SizeDistributions", "Dist", DistNo, "TSize",     TopSize()?TopSize():"");
  WRCFG(Cfg, "SizeDistributions", "Dist", DistNo, "BSizeDisp", BottomSizeDisp()?BottomSizeDisp():"");
  WRCFG(Cfg, "SizeDistributions", "Dist", DistNo, "TSizeDisp", TopSizeDisp()?TopSizeDisp():"");

  Strng S;
  for (int i=0; i<Species.GetSize(); i++)
    {
    S = Species[i];
    if (S.Find("$Default$")>0)
      {
      S = S.Left(S.Len()-strlen("$Default$")-1);
      DefaultSpecie = S;
      }
    WRCFG(Cfg, "SizeDistributions", "Dist", DistNo, "Sp", i, S());
    }
  WRCFG(Cfg, "SizeDistributions", "Dist", DistNo, "DefaultSpecie", DefaultSpecie()?DefaultSpecie():"");

  for (i=0; i<Columns.GetSize(); i++)
    WRCFG(Cfg, "SizeDistributions", "Dist", DistNo, "Col", i, Columns[i]());

  }

// --------------------------------------------------------------------------

bool CModelInfo::GetCfgFileLocation(CProfINIFile & Cfg)
  {
  Strng CfgPath, CfgDrive, CfgFullPath, CfgName;
  CfgPath.FnDir(m_sCfgFile());
  CfgDrive.FnDrive(m_sCfgFile());
  CfgFullPath.FnDrivePath(m_sCfgFile());
  CfgName.FnNameExt(m_sCfgFile());

  m_sCfgFiles=CfgFullPath;
  m_sCfgFiles.FnCheckEndBSlash();

  // Test For SysCAD.MDB This must now exist alongside CfgFile
  Strng ScdMDB, NewCfg;
  ScdMDB.Set("%s%s", m_sCfgFiles(), CfgDBFileName());
  if (!FileExists(ScdMDB()))
    {
    Strng Msg;
    ScdMDB.Set("%s%s\\%s", m_sCfgFiles(), DefCfgFolderName(), CfgDBFileName());
    if (FileExists(ScdMDB()))
      {
      NewCfg.FnDrivePath(ScdMDB());
      NewCfg.FnCheckEndBSlash();
      NewCfg+=CfgName();
      Msg.Set("Configuration File & Specie DB File must be in the same Folder:\n\n"
        "Specie DB File found in folder below Configuration File:\n\n%s\n%s\n\n"
        "Move the Configuration file ?", m_sCfgFile(), ScdMDB());
      }
    else
      {
      Msg.Set("Configuration File & Specie DB File must be in the same Folder:\n\n"
        "Configuration File found in folder:\n\n%s\n\n"
        "Browse for SpecieDB File ?", m_sCfgFile());
      if (AfxMessageBox(Msg(), MB_ICONEXCLAMATION|MB_YESNO)==IDNO)
        return false;
      // Browse ???

      Strng InitDir(m_sCfgFiles());
     
      CSCDFileDialog Dlg(true, NULL, "SysCAD.MDB", 
        OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, 
        "SysCAD Specie Databases(*.mdb)|*.mdb||", AfxGetMainWnd());
      Dlg.m_ofn.lpstrInitialDir = InitDir();
      Dlg.m_ofn.lpstrTitle = "Browse for Specie Database";
      if (Dlg.DoModal()==IDOK)
        {
        ScdMDB=Dlg.GetPathName();
        //ScdMDB.Set("%s%s\\%s", m_sCfgFiles(), DefCfgFolderName(), CfgDBFileName());
        if (FileExists(ScdMDB()))
          {
          NewCfg.FnDrivePath(ScdMDB());
          NewCfg.FnCheckEndBSlash();
          NewCfg+=CfgName();
          Msg.Set("Configuration File & Specie DB File must be in the same Folder:\n\n"
            "Specie DB File found in folder:\n\n%s\n%s\n\n"
            "Move the Configuration file ?", m_sCfgFile(), ScdMDB());
          }
        else
          return false;
        }
      else
        return false;
      }

    if (AfxMessageBox(Msg(), MB_ICONEXCLAMATION|MB_YESNO)==IDNO)
      return false;
 
    if (!MoveCfgFile(m_sCfgFile, NewCfg()))
      return false;

    m_sName.FnName(m_sCfgFile());

    Cfg.SetProfFilename(m_sCfgFile());

    CfgPath.FnDir(m_sCfgFile());
    CfgDrive.FnDrive(m_sCfgFile());
    CfgFullPath.FnDrivePath(m_sCfgFile());
    CfgName.FnNameExt(m_sCfgFile());
    }

  m_sCfgFiles=CfgFullPath;
  m_sCfgFiles.FnCheckEndBSlash();

  return true;  
  }

//---------------------------------------------------------------------------
// return 0 - file missing
//        1 - file format bad 
//        2 - file OK - dll(s) missing
//        3 - file OK - dll(s) loaded

byte CModelInfo::GetCfgInfo(char * FileName, flag DoDllLoad, flag Whinge)//, flag WhingeLoudly)
  {
  if (!FileExists(FileName))
    return 0;

  switch (CfgInfoFmt(FileName))
    {
    case 0:
      {
      LogError("SysCAD", 0, "Bad config file format '%s'", FileName);
      return 1;
      }
    case 1: 
      break;
    default:
      return 1;
    }

  m_sCfgFile=FileName;
  m_sName.FnName(FileName);

  flag OK=false;
  m_sDesc="";
  m_sDLLList="";
  m_sCfgFiles="";

  CSArray DLLs;

  //Strng Fn = FileName;
  
  m_sCfgFile.FnSearchExpand(FileName, AF_All|AF_BackupFiles);//, &Where);

  CProfINIFile Cfg(m_sCfgFile());

  flag DLLListOK=false;
  flag DllListSpecd=false;


  m_sDesc=Cfg.RdStr("General", "Description", "");
  long CfgFilePrjFileVerNo=Cfg.RdLong("General", "PrjFileVersion", -1);

  if (!GetCfgFileLocation(Cfg))
    return 0;

  //get list of model DLLs...
  Strng DllName;
  for (int i=0; ; i++)
    {
    Strng Tag;
    Tag.Set("M%04i",i);
    DllName=Cfg.RdStr("ModelDLLs", Tag(), "");
    if (DllName())
      {
      DllName.FnCheckExtension("DLL");
      if (_stricmp("compres1.dll", DllName())!=0)
        {
        DllListSpecd=True;
        DLLs.Add(DllName);
        }
      }
    else
      break;
    }
  /*//add ScdMDK user DLLs to list...
  int FirstCOMDLL = DLLs.GetSize();
  for (i=0; ; i++)
    {
    Strng Tag;
    Tag.Set("C%04i",i);
    DllName=Cfg.RdStr("COMModelDLLs", Tag(), "");
    if (DllName())
      {
      DllListSpecd=True;
      DllName.FnCheckExtension("DLL");
      DLLs.Add(DllName);
      }
    else
      break;
    }*/

  /*//add ScdMDK user DLLs to list...
  int FirstSMDKDLL = DLLs.GetSize();
  for (i=0; ; i++)
    {
    Strng Tag;
    Tag.Set("U%04i",i);
    DllName=Cfg.RdStr("SMDKUserDLLs", Tag(), "");
    if (DllName())
      {
      DllListSpecd=True;
      DllName.FnCheckExtension("DLL");
      DLLs.Add(DllName);
      }
    else
      break;
    }*/

  if (DLLs.GetSize())
    {
    DLLListOK=True;
    // Do these DLL's Exist
    for (int i=0; i<DLLs.GetSize(); i++)
      {
      Strng FullName;
      if (IsDLLOK(CfgName(), DLLs[i](), DllFilesPath(), FullName, true, false/*i<FirstCOMDLL, i>=FirstSMDKDLL*/, Whinge))
        {
        if (m_sDLLList.Length()>0)
          m_sDLLList+=",";
        m_sDLLList+=FullName();
        }
      else
        DLLListOK=false;
      }
    }
  else
    DLLListOK=false;

  if (!DLLListOK && Whinge)
    if (DllListSpecd)
      LogWarning(CfgName(), LF_Exclamation, "Invalid Model DLL List");
    else
      LogWarning(CfgName(), LF_Exclamation, "No Model DLL List specified");
  
  return DLLListOK ? 3 : 2;
  }

//---------------------------------------------------------------------------

int CModelInfoUpgrade::CheckCfgFileFormat(char *Filename, flag SetFlags)
  {
  int Fmt=-1;
  switch (CModelInfo::CfgInfoFmt(Filename))
    {
    case 0:
      {
      Fmt=0;
      LogError("Project", LF_Exclamation, "Incorrect Config File file format");
      break;
      }
    case 1:
      {
      if (SCDFileFormat()==0)
        {
        Fmt=0;
        LogError("Project", LF_Exclamation, "Incorrect System file format");
        }
      else
        {
        Fmt=1;
        if (SetFlags)
          SetPrjFileFormat(1);
        }
      break;
      }
    default:
      LogError("Project", LF_Exclamation, "File %s missing", Filename);
    }
  return Fmt;
  }

//---------------------------------------------------------------------------

flag CModelInfoUpgrade::CheckDBFileFormat(char *FilesPath)
  {
  flag OK=true;

  if (SCDFileFormat()==1)
    {
    CDlgBusy::Open(" ");
    CDlgBusy::SetLine(2,"Upgrading CFG File");

    CUpGrdCompletion UGC(m_OldFilePath());

    // Upgrade Databases
    Strng SDBCfg, D, M, C, C1;
    SDBCfg.FnDrivePath(FilesPath);
    //SDBCfg.FnMakeCfgDataFolder();
    //D=SDBCfg;
    //D+=".";
    //if (!FileExists(D()))
    //  {
    //  SDBCfg.FnDrivePath(Filename);
    //  SDBCfg+=CfgFilesAlias();
    //  D=SDBCfg;
    //  D+=".";
    //  }
    //if (FileExists(D()))
    //  {
    M=SDBCfg;
    C=SDBCfg;
    C1=SDBCfg;
    M+=CfgDBFileName();
    C+="SpecieDB.csv";
    C1+="ScdCnvs.csv";
    flag MDBOK=FileExists(M());
    flag CSVOK=FileExists(C());
    flag CSV1OK=FileExists(C1());
    if (!MDBOK && (CSVOK || CSV1OK))
      {
      CDlgBusy::SetLine(2,"Upgrading Database File");
      //if (!CSCDDatabase::UpgradeCSVDB(SDBCfg(), UGC))
      OK=false;
      }
    //  }

    UGC.Complete(OK);
 
    CDlgBusy::Close();
    }
  return OK;
  }

//---------------------------------------------------------------------------

flag CModelInfo::GetSetName(char * PrjFile, flag ForNew, int OldPrjFileVer, DWORD LicCategory, LPCTSTR RqdCfgFile)
  {
  CWaitCursor Wait;

  BOOL fNewTrendRqd=false;
  BOOL fNewGraphRqd=false;
  
  if (!RqdCfgFile)
    {
    CNewPrjDlg Dlg(PrjFile, ForNew, false, LicCategory, AfxGetMainWnd());
    if (Dlg.DoModal()==IDCANCEL || Dlg.ModelCfg()<0)
      return false;

    if (!GetCfgInfo(Dlg.File(), true, true))
      return false;
    fNewTrendRqd = Dlg.m_NewTrend;
    fNewGraphRqd = Dlg.m_NewGraph;
    }
  else
    {
    if (!GetCfgInfo((LPTSTR)RqdCfgFile, true, true))
      return false;
    }

  fNewTrend = fNewTrendRqd;
  fNewGraph = fNewGraphRqd;
  return True;
  }

//---------------------------------------------------------------------------

flag CModelInfo::EditCfgFile(char *CfgFile, DWORD LicCategory)
  {
  flag OK=true;
  if (CfgFile==NULL)
    {
    CNewPrjDlg Dlg("", false, true, LicCategory, AfxGetMainWnd());
    OK=(Dlg.DoModal()==IDOK);
    }
  else
    {
    CMdlCfgSheet CfgEdit(LicCategory, CfgFile, "", AfxGetMainWnd(), 0);
    OK=(CfgEdit.DoModal()==IDOK);
    }

  return OK;
  }

//---------------------------------------------------------------------------

flag CModelInfo::EnsureCfgIsInList()
  {
  if (m_sCfgFile.FindI(BackupFilesFolderName())>=0)
    return false; //cfg file is in a $LC folder, do NOT add

  Strng NameI;
  for (int i=0;;i++)
    {
    NameI.Set("Cfg%02i", i);
    Strng Name(ScdPFUser.RdStr("Configuration", NameI(), ""));
    if (Name.GetLength()>0)
      {
      if (_stricmp(Name(), m_sCfgFile())==0)  
        return false; //cfg file is already in list
      }
    else
      break;
    }

  ScdPFUser.WrStr("Configuration", NameI(), m_sCfgFile());
  return true;  //cfg file added to list
  }

//---------------------------------------------------------------------------

void CModelInfo::MakeDefaultNewCfgFile(char* pCfgFile, char* pCfgFiles, char * Desc)
  {
  CProfINIFile Cfg(pCfgFile);
  Strng CfgFiles(pCfgFiles);
  Cfg.WrStr("General", "Description", Desc);
  Cfg.WrInt("General", "PrjFileVersion", PrjFileVerNo());
  Cfg.WrInt("General", "TagMonitor", 0);
  Cfg.WrInt("General", "UseStdFns4H2O", 1);

  Cfg.WrDouble("General", "Std_Temp", Std_T);
  Cfg.WrDouble("General", "Std_Press", Std_P);
  Cfg.WrDouble("General", "Norm_Temp", Norm_T);
  Cfg.WrDouble("General", "Norm_Press", Norm_P);
  Cfg.WrDouble("General", "Minimum_Temp", C_2_K(0.0));
  Cfg.WrDouble("General", "Maximum_Temp", C_2_K(1000.0));
  Cfg.WrDouble("General", "Minimum_Press", 50.0);
  Cfg.WrDouble("General", "Maximum_Press", 350.0);
  Cfg.WrStr("General", "Atmospheric_Press", "101.287, -11.836e-3, 0.47931e-6");
  Cfg.WrStr("General", "Flash_Component", "H2O");
#if WithSIMPLESPMDL
  Cfg.WrStr("General", "Default_SpModel", BaseSpModelName);
#else
  Cfg.WrStr("General", "Default_SpModel", MassWtMnSpModelName);
#endif
  Cfg.WrStr("ModelDLLs",  "M0000", "Basic1.dll");
  Cfg.WrStr("ModelDLLs",  "M0001", "Control1.dll");
  Cfg.WrStr("ModelDLLs",  "M0002", "FlwLib.dll");
  Cfg.WrStr("ModelDLLs",  "M0003", "MdlBase.dll");
  Cfg.WrStr("HelpDLLs",   "H0000", "kw_mdls.hlp");
  Cfg.WrStr("Species", "S0000", "4,Total");
  Cfg.WrStr("Species", "S0000", "3,Vapours");
  Cfg.WrStr("Species", "S0001", "2,Liquids");
  Cfg.WrStr("Species", "S0002", "1,Solids");
  Cfg.WrStr("Species", "S0003", "5,---Solids---");
  Cfg.WrStr("Species", "S0004", "5,---Liquids---");
  Cfg.WrStr("Species", "S0005", "0,H2O(l),25,25");
  Cfg.WrStr("Species", "S0006", "5,---Vapours---");
  Cfg.WrStr("Species", "S0007", "0,H2O(g),25,25");
  Cfg.WrStr("Selectable", "S0000", "0,FlwUnit");
  Cfg.WrStr("Selectable", "S0001", "1,Feed_XPG-1");
  Cfg.WrStr("Selectable", "S0002", "1,Tank-1");
  Cfg.WrStr("Selectable", "S0003", "1,Tie-1");
  Cfg.WrStr("Selectable", "S0004", "1,MakeupSrc-1");
  Cfg.WrStr("Selectable", "S0005", "1,BleedSnk-1");
  Cfg.WrStr("Selectable", "S0006", "1,GControl");
  Cfg.WrStr("Selectable", "S0007", "1,PID_Cont-1");
  Cfg.WrStr("Selectable", "S0008", "1,Pipe-1");
  Cfg.WrStr("Selectable", "S0009", "1,Simple");
  }

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
SFEMdlLib::SFEMdlLib()
  {
  hSFELib=NULL;
  }

//---------------------------------------------------------------------------

SFEMdlLib::~SFEMdlLib()
  {
  }

//---------------------------------------------------------------------------

flag SFEMdlLib::Initialise(pchar ModelCfg, pchar FlwLibTag, pchar EO_LocationTag)
  {
  return true;
  }

//---------------------------------------------------------------------------

flag SFEMdlLib::Terminate()
  {
  return true;
  }

//---------------------------------------------------------------------------

flag SFEMdlLib::Load(pchar MdlDLL)
  {
  sMdlDLLName.FnNameExt(MdlDLL);
  sMdlDLLName.FnCheckExtension("DLL");
  sMdlDLLPath.FnDrivePath(MdlDLL);//MdlDLL;
  flag Connected=0;

  #if dbgClnt
  if (dbgDumpTagClasses())
    {
    dbgpln("TagClasses - Before Load");
    TagObjClass::DumpAll();
    }
  #endif

  hSFELib=MyAfxLoadLibrary(sMdlDLLName(), DllFilesPath());
  dbgpln("%s - %s", sMdlDLLName(), hSFELib ? "Loaded" : "NOT Loaded");

  return hSFELib!=NULL;
  }

//---------------------------------------------------------------------------

flag SFEMdlLib::UnLoad()//pchar FlwEngineDLL_) //, CWnd* MsgWnd) //, pchar Location_)
  {
  if (lDoFreeLibraries)
    {
    BOOL bResult = AfxFreeLibrary(hSFELib);
    //if (bResult)
    //  hSFELib=NULL;
    #ifndef _RELEASE
    if (!bResult)
      ::AfxTrace(_T("AfxFreeLibrary for %s FAILED!\n"), _T(sMdlDLLName()));
    #endif
    }

  hSFELib=NULL;

//  dbgpln("TagClasses - After UnLoad");
//  TagObjClass::DumpAll();
  return true;
  }

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

typedef void (*fn_InitCfg)(void);

flag SFEMdlLib::InitCfg()
  {
  if (hSFELib)
    {
    fn_InitCfg pFn = (fn_InitCfg)GetProcAddress(hSFELib, "InitCfg");
    if (pFn)
      {
      pFn();
      }

    typedef bool (*fn_LoadInitialise)(void);
    fn_LoadInitialise pFnLI=(fn_LoadInitialise)GetProcAddress(hSFELib, "LoadInitialise");
    if (pFnLI)
      {
      if (!pFnLI())
        {
        LogError("Project", LF_Exclamation, "Initialisation error for %s", sMdlDLLName());
        return false;
        }
      }
    }
  return true;
  }


//---------------------------------------------------------------------------

flag /*Project::*/CheckForDuplicateClassIds()
  {
  flag OK=true;

  for (pTagObjClass p=TagObjClass::FirstClass(); p; p=p->NextClass())
    {
    pTagObjClass pOther=p->FindDuplicateClassId(NULL);
    if (pOther)
      if (!p->DuplicateExists())
        {
        p->SetDuplicateExists(true);
        pOther->SetDuplicateExists(true);
        OK=false;
        LogError("Project", 0,"Duplicate ClassId found %s", p->ClassId());
        }
    }
  return OK;
  }

void InitClassLicenses(dword dwLic)
  {
  dwLic &= TOC_MDL_MASK;
  TagObjClass* pTOC = TagObjClass::FirstClass();
  while (pTOC)
    {
    pTOC->TestLicenseOK(dwLic, false);


    dbgpln("Class: %-10s %-10s %s", pTOC->LicenseOK()?"Licensed":"  ", pTOC->Selectable()?"Selectable":"", pTOC->ClassName());
    pTOC = pTOC->NextClass();
    }
  }

flag SFEMdlLibArray::LoadFlwDLLs(Strng &DLLList, pchar FlwLibTag, pchar EO_LocationTag, SFEMdlLib * pTheFlwLib)
  {
  //SFEFlwLib     *pFlwLib;

  flag OK=true;

#if DoDDBChecks
  DataDefnBlk::ClearAllChecks();
#endif

  Strng DLL("FLWLIB.DLL");
  CDlgBusy::Open("\nLoading Models :");
  CDlgBusy::SetLine(3, "%s : %s", CfgName(), DLL());
  //pFlwLib=new SFEFlwLib;
  if (!pTheFlwLib->Load(DLL()))
    {
    OK=false;
    //delete pFlwLib;
    //pFlwLib=NULL;
    LogError("Project", LF_Exclamation, "%s not loaded", DLL());
    }
  else
    Add(pTheFlwLib);

  DLLList.Upper();
  while (OK && DLLList.Length()>0)
    {
    DLL=DLLList;
    int i=DLL.XStrCSpn(",");
    DLL.SetLength(i);
    DLLList=DLLList.Right(Max(0, DLLList.Length()-(i+1)));

    Strng Fn(DLL);
    Fn.FnNameExt();
    if (_stricmp(Fn(), "FLWLIB.DLL")!=0) //don't try load flwlib again
      {
      CDlgBusy::SetLine(3, "%s : %s", CfgName(), DLL());

      SFEMdlLib *pMdlLib=new SFEMdlLib;

      if (!pMdlLib->Load(DLL()))
        {
        Strng E;
        DWORD Err=GetLastError();
        E.GetSystemErrorStr(Err);
        LogError("Project", 0, "%s load error:%d  %s", DLL(), Err, E());
        OK=false;
        delete pMdlLib;
        LogError("Project", LF_Exclamation, "%s (or dependent DLL) not loaded", DLL());
        }
      else
        {
        Add(pMdlLib);
        }
      }
    }

  if (!OK)
    UnLoadFlwDLLs(false);
 
  CDlgBusy::SetLine(1, "\nInitialising Flow Engine\n");
  if (pTheFlwLib && OK)
    {
    OK=CheckForDuplicateClassIds();
    if (OK)
      OK=pTheFlwLib->Initialise(CfgFile(), FlwLibTag, EO_LocationTag);
    if (OK)
      {
      const int Cnt = GetSize();
      for (int i=0; i<Cnt && OK; i++)
        OK=ElementAt(i)->InitCfg();
      }
    if (!OK)
      UnLoadFlwDLLs(false);
    if (OK)
      InitClassLicenses(gs_License.LicCatagories());
    }

  CDlgBusy::Close();
  return OK;
  }

//---------------------------------------------------------------------------

flag SFEMdlLibArray::UnLoadFlwDLLs(flag DoTerminate)
  {
#if DoDDBChecks
  if (0)
    DataDefnBlk::DumpAllChecks();
  DataDefnBlk::ClearAllChecks();
#endif

  const int Cnt = GetSize();
  if (Cnt>0)
    {

    CDlgBusy::Open("\nUnLoading Models");
    if (DoTerminate)
      for (int i=Cnt-1; i>=0; i--)
        ElementAt(i)->Terminate();

#if HEAP_SPARES
    StkSparesList::ClearAll();
#endif


    //delete pFlwLib;
    //pFlwLib=NULL;

    for (int i=Cnt-1; i>=0; i--)
      {
      CDlgBusy::SetLine(1, "UnLoading Models :\n\n%s : %s", CfgName(),ElementAt(i)->sMdlDLLName());
      ::AfxTrace(_T("UnLoading Models : %s : %s\n"), _T(CfgName()),_T(ElementAt(i)->sMdlDLLName()));//_T("%s"), _T(sz))
      ElementAt(i)->UnLoad();
      Sleep(200);
      if (i>0)
        delete ElementAt(i);
      else
        {
        //delete pFlwLib;
        }
      }

    dbgpln("Class            Instances");
    for (TagObjClass *p=TagObjClass::FirstClass(); p; p=p->NextClass())
      dbgpln("%-30.30s  %5i",p->ShortDesc() ? p->ShortDesc() : p->ClassId(), p->Instances()); //p is sometimes invalid pointer! (eg after DLL failed to load because of dependent DLL)

    RemoveAll();
    //pFlwLib=NULL;
    CDlgBusy::Close();
    }
//  pFlwLib=NULL;
  return true;
  }

//==========================================================================
//
//
//
//==========================================================================

void CTagRefInfoItem::Copy(const CTagRef & X, bool Reverse)
  {
  if (0)
    {
    dbgpln("[%5i] %08x -------------------------------------", X.m_nMyAllocNo, &X);
    static const LPCSTR XX[] = {"Near","Far","IO","???"};
    dbgpln("  IsLcl  : %s", X.m_bIsLcl?"Lcl":"");
    dbgpln("  RefID  : %s", X.m_sRefID());
    dbgpln("     Tag : %s", X.m_sRefTag());
    dbgpln("  RmtID  : %s", X.m_sRmtID());
    dbgpln("     Tag : %s", X.m_sRmtTag());
    if (X.m_bMustGet || X.m_bMustSet)
      dbgpln("  Dirn   : %s%s", X.m_bMustGet?"Get":"", X.m_bMustSet?"Set":"");
    else
      dbgpln("  Dirn   : ------------------------------ ???");
    dbgpln("  Sets   : %i", X.SetCnt());
    dbgpln("  Gets   : %i", X.GetCnt());
    }

  Strng sRefTagOnly, sRefCnv;
  TaggedObject::SplitTagCnv(X.m_sRefTag(), sRefTagOnly, sRefCnv);
  if (sRefTagOnly.Find('.')>=0)
    sRefTagOnly = sRefTagOnly.Left(sRefTagOnly.Find('.'));
  m_sCnv=X.m_sCnv;
  if (1)//!Reverse)
    {
    m_fIsGetRef=X.m_bMustGet;
    m_fIsSetRef=X.m_bMustSet;
    m_fIsIO=X.m_iIOId>=0;
    m_iGetCnt=X.GetCnt();
    m_iSetCnt=X.SetCnt();
    m_sRefTag=X.m_sRefTag;
    m_sRmtTag=X.m_sRmtTag;
    m_sRefID=X.m_sRefID;
    m_sRmtID=X.m_sRmtID;
    }
  else
    {
    }
  };

//===========================================================================
//
//
//
//===========================================================================

static void AddTxtFld(CString &S, LPCTSTR Fmt, ...)
  {
  char b[16384];
  va_list argptr;
  va_start(argptr, Fmt);
  VERIFY(vsprintf(b, Fmt, argptr) < sizeof(b));
  va_end(argptr);
  S+=b;
  S+="\t";
  }
static void AddTxtLine(CString &S, LPCTSTR Fmt, ...)
  {
  char b[16384];
  va_list argptr;
  va_start(argptr, Fmt);
  VERIFY(vsprintf(b, Fmt, argptr) < sizeof(b));
  va_end(argptr);
  S+=b;
  S+="\r\n";
  }

static void DumpStrD(CString &S, CNodeBalanceInfo &B, LPCTSTR Name, LPCTSTR Cnv, bool AppendTag, LPCTSTR Tag, int TagIndex, LPCTSTR Fmt, double V, LPCTSTR End1="", LPCTSTR End2="")
  {
  CString T;
  if (AppendTag && Tag && strlen(Tag)>0)
    {
    T.Format("%s.%s", B.m_NodeTag, Tag);
    Tag=T;
    }
  AddTxtFld(S, Name);
  AddTxtFld(S, Cnv);
  AddTxtFld(S, Tag, TagIndex);
  AddTxtFld(S, Fmt, V);
  AddTxtFld(S, End1);
  AddTxtFld/*Line*/(S, End2);
  }

static void DumpStrS(CString &S, CNodeBalanceInfo &B, LPCTSTR Name, LPCTSTR Cnv, bool AppendTag, LPCTSTR Tag, LPCTSTR Fmt, LPCTSTR V, LPCTSTR End1="", LPCTSTR End2="")
  {
  CString T;
  if (AppendTag && Tag && strlen(Tag)>0)
    {
    T.Format("%s.%s", B.m_NodeTag, Tag);
    Tag=T;
    }
  AddTxtFld(S, Name);
  AddTxtFld(S, Cnv);
  AddTxtFld(S, Tag);
  AddTxtFld(S, Fmt, V);
  AddTxtFld(S, End1);
  AddTxtLine(S, End2);
  }

static void DumpStrH(CString &S, CNodeBalanceInfo &B, LPCTSTR Name, LPCTSTR Cnv, bool AppendTag, LPCTSTR Tag)
  {
  CString T;
  if (AppendTag && Tag && strlen(Tag)>0)
    {
    T.Format("%s.%s", B.m_NodeTag, Tag);
    Tag=T;
    }
  AddTxtFld(S, Name);
  AddTxtFld(S, Cnv);
  AddTxtFld(S, Tag);
  }

void DumpAdtBalance(DWORD Options, CWnd *pWnd, CNodeAuditInfo & Audit)
  {
  // SUB_NA_ADT_MASS 
  // SUB_NA_ADT_MASSTOTAL
  // SUB_NA_ADT_HEAT 
  // SUB_NA_ADT_HEATTOTAL

  CopyTextToClipboard(pWnd ? pWnd : AfxGetMainWnd(), (LPTSTR)(LPCTSTR)Audit.m_Report);

  };

void DumpClsBalance(bool DoCompact, bool DoSummary, CWnd *pWnd, CNodeBalanceInfo & Balance)
  {
  bool Combine=false;
  CNodeBalanceInfo CombBalance;
  if (DoCompact)
    {
    Combine=true;
    CombBalance.Combine(Balance);
    }
  CNodeBalanceInfo &B=Combine ? CombBalance : Balance;
  CString S;

  CArray <CNodeBalanceIO*, CNodeBalanceIO*> IOs;
  IOs.SetSize(B.m_Feeds.GetCount()+B.m_Prods.GetCount());

  B.DoCalculations(false);

  CString Separ1("---------------");
  CString Separ2("----");
  CString ScdHdr1;
  CString ScdHdr2;

  ScdHdr1.Format("SysCAD_TagList(\"Balance\",V,%i)", DoSummary?25:25);
  ScdHdr2.Format("SysCAD_Tags(\"Balance\",H,2,%i)", DoSummary?16:21);

  DumpStrS(S, B, "Tag:",      B.m_NodeTag,  false, "", "",       "");

  for (int iJoin=0; iJoin<B.m_Joins.GetSize(); iJoin++)
    {
    int nIO=0;
    for (int i=0; i<B.m_Feeds.GetCount(); i++)
      if (B.m_Feeds[i].m_iJoinId==iJoin)
        IOs[nIO++]=&B.m_Feeds[i];
    for (int i=0; i<B.m_Prods.GetCount(); i++)
      if (B.m_Prods[i].m_iJoinId==iJoin)
        IOs[nIO++]=&B.m_Prods[i];

    CNodeBalanceJoin &J=B.m_Joins[iJoin];

    int nFeeds=0;
    for (int i=0; i<nIO; i++)
      {
      if (IOs[i]->m_iSgn>0)
        nFeeds++;
      }

    CString SumStr("=");
    CString STmp;
    if (nFeeds<nIO)
      {
      LPCTSTR pSign="";
      int RelCol=-nIO+nFeeds;
      SumStr+="(";
      for (int i=nFeeds; i<nIO; i++)
        {
        STmp.Format("%sRC[%i]", pSign, RelCol++);
        SumStr+=STmp;
        pSign="+";
        }
      SumStr+=")";
      }
    if (nFeeds>0)
      {
      LPCTSTR pSign="";
      SumStr+="-(";
      pSign="";
      int RelCol=-nIO;
      for (int i=0; i<nFeeds; i++)
        {
        STmp.Format("%sRC[%i]", pSign, RelCol++);
        SumStr+=STmp;
        pSign="+";
        }
      SumStr+=")";
      }

    int CO=-(1+nIO);
    //int ROm=-(30+(DoSummary ? 0:0));
    //int ROs=-(44+(DoSummary ? -5:0));
    //int ROf=-(46+(DoSummary ? -6:0));
    int ROm=-(28+(DoSummary ? 0:0));
    int ROs=-(42+(DoSummary ? -5:0));
    int ROf=-(44+(DoSummary ? -6:0));

    CString CfStrMs;
    CString CfStrHs;
    CString CfStrHf;
    CString ProdCpT;
    CString CpStr;
    CString CpDiffStr;

    CfStrMs.Format("=(R[%i]C[%i]+R[%i]C[%i]+R[%i]C[%i]-R[%i]C[%i])",
                      ROm+0,CO,  ROm+1,CO,  ROm+2,CO,  ROm+3, CO);

    CfStrHs.Format("=(R[%i]C[%i]+R[%i]C[%i]+R[%i]C[%i]+R[%i]C[%i]+R[%i]C[%i]+R[%i]C[%i]+R[%i]C[%i])-(R[%i]C[%i])-(R[%i]C[%i])",
                      ROs+11,CO, ROs+12,CO, ROs+13,CO, ROs+8,CO,  ROs+9,CO,  ROs+10,CO, ROs+15,CO,   ROs+14,CO,   ROs+18,CO);
    CfStrHf.Format("=(R[%i]C[%i]+R[%i]C[%i]+R[%i]C[%i]+R[%i]C[%i]+R[%i]C[%i]+R[%i]C[%i])-(R[%i]C[%i])-(R[%i]C[%i])",
                      ROf+4,CO,  ROf+5,CO,  ROf+6,CO,  ROf+8,CO,  ROf+9,CO,  ROf+10,CO,   ROf+7,CO,    ROf+20,CO);

    CpStr.Format("=R[%i]C*0.5*(R[%i]C+R[%i]C)*R[%i]C",
                   DoSummary?-12:-18,        DoSummary?-7:-9,    DoSummary?-8:-10,    DoSummary?-11:-17);
    CpDiffStr.Format("=100*(R[%i]C-R[%i]C)/max(1e-30,abs(R[%i]C))",
                   DoSummary?-4:-5,    -1,                   DoSummary?-4:-5);

    int nSpace=3+(nIO-2-1);
    CString sJoin;
    sJoin.Format("%i", iJoin);
    DumpStrS(S, B, "Join:",                   sJoin,      false,  ScdHdr1,                                          "%s",     "",                 "");// "Fields");
    DumpStrD(S, B, "Mass In",                 "kg/s",     true,  "Closure.[%i].MassIn (kg/s)",                iJoin, "%.18g", J.m_JoinMassFlwIn,  "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Mass flow in through IO"                                                    );
    DumpStrD(S, B, "Mass Gain",               "kg/s",     true,  "Closure.[%i].MassGain (kg/s)",              iJoin, "%.18g", J.m_JoinMassGain,   "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Mass added by the Model"                                                    );
    DumpStrD(S, B, "Mass Gain Rct",           "kg/s",     true,  "Closure.[%i].MassGain.RCT (kg/s)",          iJoin, "%.18g", J.m_RCTMassGain,    "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Mass added by a Reaction Block"                                             );
    DumpStrD(S, B, "Mass Out",                "kg/s",     true,  "Closure.[%i].MassOut (kg/s)",               iJoin, "%.18g", J.m_JoinMassFlwOut, "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Mass flow out through IO"                                                   );
    DumpStrD(S, B, "Total Heat In",           "kJ/s",     true,  "Closure.[%i].TotalHeatIn (kJ/s)",           iJoin, "%.18g", J.m_JoinHfIn,       "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Total Heat flowing in (Sensible + Heat of formation)"                       );
    DumpStrD(S, B, "Total Heat Gain",         "kJ/s",     true,  "Closure.[%i].TotalHeatGain (kJ/s)",         iJoin, "%.18g", J.m_JoinHfGain,     "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Total Heat Associated with MassGain"                                        );
    DumpStrD(S, B, "Total Heat Gain Rct",     "kJ/s",     true,  "Closure.[%i].TotalHeatGain.RCT (kJ/s)",     iJoin, "%.18g", J.m_RCTHfGain,      "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Total Heat Associated with Rct.MassGain"                                    );
    DumpStrD(S, B, "Total Heat Out",          "kJ/s",     true,  "Closure.[%i].TotalHeatOut (kJ/s)",          iJoin, "%.18g", J.m_JoinHfOut,      "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Total Heat flowing out"                                                     );
    DumpStrD(S, B, "Power Input",             "kJ/s",     true,  "Closure.[%i].PowerIn (kJ/s)",               iJoin, "%.18g", J.m_JoinPowerIn,    "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Heat flow into join from External Heat Source"                              );
    DumpStrD(S, B, "Power Input Rct",         "kJ/s",     true,  "Closure.[%i].PowerIn.RCT (kJ/s)",           iJoin, "%.18g", J.m_RCTPowerIn,     "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Heat flow into join from External Heat Source cfg in Reaction Block"        );
    DumpStrD(S, B, "Power Input EHX",         "kJ/s",     true,  "Closure.[%i].PowerIn.EHX (kJ/s)",           iJoin, "%.18g", J.m_EHXPowerIn,     "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Heat flow into join from External Heat Source cfg in Environmental Sources" );
    DumpStrD(S, B, "Sens Heat In",            "kJ/s",     true,  "Closure.[%i].SensibleHeatIn (kJ/s)",        iJoin, "%.18g", J.m_JoinHsIn,       "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Sensible Heat flowing in"                                                   );
    DumpStrD(S, B, "Sens Heat Gain",          "kJ/s",     true,  "Closure.[%i].SensibleHeatGain (kJ/s)",      iJoin, "%.18g", J.m_JoinHsGain,     "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Sensible Heat Associated with MassGain"                                     );
    DumpStrD(S, B, "Sens Heat Gain Rct",      "kJ/s",     true,  "Closure.[%i].SensibleHeatGain.RCT (kJ/s)",  iJoin, "%.18g", J.m_RCTHsGain,      "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Sensible Heat Associated with Rct.MassGain"                                 );
    DumpStrD(S, B, "Sens Heat Out",           "kJ/s",     true,  "Closure.[%i].SensibleHeatOut (kJ/s)",       iJoin, "%.18g", J.m_JoinHsOut,      "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Sensible Heat flowing out"                                                  );
    DumpStrD(S, B, "VLE Hf Change @ 0",       "kJ/s",     true,  "Closure.[%i].VLE.HfChange@0 (kJ/s)",        iJoin, "%.18g", J.m_VLEHsGain,      "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Sensible Heat Change due to Phase changes"                                  );
    DumpStrD(S, B, "HOR.Std @ 0",             "kJ/s",     true,  "Closure.[%i].HOR.Std@0 (kJ/s)",             iJoin, "%.18g", J.m_RCTHOR_Std0,    "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "HOR @ 0C Calc by the 'Standard' Specie Model (relies on SDB)"               );
    DumpStrD(S, B, "HOR.Mdl @ 0",             "kJ/s",     true,  "Closure.[%i].HOR.Mdl@0 (kJ/s)",             iJoin, "%.18g", J.m_RCTHOR_Mdl0,    "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "HOR @ 0C Calc by the 'Current Specie Model' (may rely on SDB)"              );
    DumpStrD(S, B, "HOR.Used @ 0",            "kJ/s",     true,  "Closure.[%i].HOR.Used@0 (kJ/s)",            iJoin, "%.18g", J.m_RCTHOR_Used0,   "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "HOR @ 0C Actually used"                                                     );
    DumpStrD(S, B, "Sens Heat Change",        "kJ/s",     true,  "Closure.[%i].SensibleHeatChg (kJ/s)",       iJoin, "%.18g", J.m_JoinHsChg,      "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Sensible Heat Change"                                                       );
    DumpStrD(S, B, "HOR.Diff @ 0",            "kJ/s",     true,  "Closure.[%i].HOR.Diff@0 (kJ/s)",            iJoin, "%.18g", J.m_RCTHOR_Diff0,   "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "HOR Difference @ 0C (UsedHOR-MdlHOR)"                                       );
    DumpStrD(S, B, "Total Heat Change",       "kJ/s",     true,  "Closure.[%i].TotalHeatChg (kJ/s)",          iJoin, "%.18g", J.m_JoinHfChg,      "", "");    for (int iSpc=0; iSpc<nSpace; iSpc++) AddTxtFld(S, "");                   AddTxtLine(S, "Total Heat Change"                                                          );

    if (nIO>0)
      {
      DumpStrH(S, B, "",                        "",         false, "");                   for (int i=0; i<nIO; i++) AddTxtFld(S, Separ1);                                             AddTxtFld(S, Separ1);   AddTxtFld(S, Separ1);    AddTxtFld(S, Separ2);    AddTxtLine(S, "");
      DumpStrH(S, B, "",                        "",         false, "");                   for (int i=0; i<nIO; i++) AddTxtFld(S, "%s", IOs[i]->m_iSgn>0?"Feed":"Product");            AddTxtFld(S, "Sum");    AddTxtFld(S, "Balance"); AddTxtFld(S, "Note");    AddTxtLine(S, "Notes");
      DumpStrH(S, B, "",                        "",         false, "");                   for (int i=0; i<nIO; i++) AddTxtFld(S, Separ1);                                             AddTxtFld(S, Separ1);   AddTxtFld(S, Separ1);    AddTxtFld(S, Separ2);    AddTxtLine(S, "For reference formulas to work, ensure 'R1C1 reference Style' is checked in Options|General before pasting data");
      DumpStrH(S, B, "IOPort",                  "",         false, "");                   for (int i=0; i<nIO; i++) AddTxtFld(S, "%s", (LPCTSTR)IOs[i]->m_IOPort);                    AddTxtFld(S, " ");      AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "All balances should ideally be zero");
      DumpStrH(S, B, "Stream",                  "",         false, ScdHdr2);              for (int i=0; i<nIO; i++) AddTxtFld(S, (IOs[i]->m_iSgn>0?"%s.Qo":"%s.Qi"), IOs[i]->m_Tag);  AddTxtFld(S, " ");      AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "A: Mass Imbalance implies problem in model");
      DumpStrH(S, B, "SpModel",                 "",         false, "Model");              for (int i=0; i<nIO; i++) AddTxtFld(S, "%s", (LPCTSTR)IOs[i]->m_SpModelName);               AddTxtFld(S, " ");      AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "B: Sensible Heat Imbalance is due to reactions or similar");
      DumpStrH(S, B, "Qm",                      "kg/s",     false, "QMt (kg/s)");         for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_Qm);                              AddTxtFld(S, SumStr);   AddTxtFld(S, CfStrMs);   AddTxtFld(S, "A");       AddTxtLine(S, "C: The sum is the difference between HOR to the SpModel and that actually used");
      DumpStrH(S, B, "T",                       "C",        false, "T (C)");              for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", K2C(IOs[i]->m_T));                          AddTxtFld(S, "");       AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "D: Estimate calculation using average Cp (SysCAD uses integration)");
      DumpStrH(S, B, "P",                       "kPag",     false, "P (kPag)");           for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_P-AtmosPress(0));                 AddTxtFld(S, "");       AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "");
      if (!DoSummary) {
      DumpStrH(S, B, "Density",                 "kg/m^3",   false, "Rho (kg/m^3)");       for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_Rho);                             AddTxtFld(S, "");       AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "");
      DumpStrH(S, B, "Sf",                      "%%",       false, "Sf (%%)");            for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_Sf*100);                          AddTxtFld(S, "");       AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "");
      DumpStrH(S, B, "Lf",                      "%%",       false, "Lf (%%)");            for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_Lf*100);                          AddTxtFld(S, "");       AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "");
      DumpStrH(S, B, "Vf",                      "%%",       false, "Vf (%%)");            for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_Vf*100);                          AddTxtFld(S, "");       AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "");
      }
      DumpStrH(S, B, "",                        "",         false, "");                   for (int i=0; i<nIO; i++) AddTxtFld(S, Separ1);                                             AddTxtFld(S, Separ1);   AddTxtFld(S, Separ1);    AddTxtFld(S, Separ2);    AddTxtLine(S, "Abbreviations");
      DumpStrH(S, B, "Cp@0",                    "kJ/kg.C",  false, "Cp@0 (kJ/kg.C)");     for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_msCp0);                           AddTxtFld(S, "");       AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "1) Rct - Reaction");
      DumpStrH(S, B, "Cp@T",                    "kJ/kg.C",  false, "Cp@T (kJ/kg.C)");     for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_msCpT);                           AddTxtFld(S, "");       AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "2) HOR - Heat of Reaction");
      DumpStrH(S, B, "Hs@T",                    "kJ/kg",    false, "Hs@T (kJ/kg)");       for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_msHsT);                           AddTxtFld(S, "");       AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "3) EHX - Environmental Heat Exchange");
      if (!DoSummary) {
      DumpStrH(S, B, "Hz@T",                    "kJ/kg",    false, "Hz@T (kJ/kg)");       for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_msHzT);                           AddTxtFld(S, "");       AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "4) SDB - Specie Database");
      }
      DumpStrH(S, B, "Hf@T",                    "kJ/kg",    false, "Hf@T (kJ/kg)");       for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_msHfT);                           AddTxtFld(S, "");       AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, DoSummary ? "4) SDB - Specie Database" : "");
      DumpStrH(S, B, "",                        "",         false, "");                   for (int i=0; i<nIO; i++) AddTxtFld(S, Separ1);                                             AddTxtFld(S, Separ1);   AddTxtFld(S, Separ1);    AddTxtFld(S, Separ2);    AddTxtLine(S, "");
      DumpStrH(S, B, "totHs@T",                 "kJ/s",     false, "totHs   (kJ/s)");     for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_totHsT);                          AddTxtFld(S, SumStr);   AddTxtFld(S, CfStrHs);   AddTxtFld(S, "B");       AddTxtLine(S, "");
      if (!DoSummary) {
      DumpStrH(S, B, "totHz@T",                 "kJ/s",     false, "totHz   (kJ/s)");     for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_totHzT);                          AddTxtFld(S, SumStr);   AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "");
      }
      DumpStrH(S, B, "totHf@T",                 "kJ/s",     false, "totHf@T (kJ/s)");     for (int i=0; i<nIO; i++) AddTxtFld(S, "%.18g", IOs[i]->m_totHfT);                          AddTxtFld(S, SumStr);   AddTxtFld(S, CfStrHf);   AddTxtFld(S, "C");       AddTxtLine(S, "");
      DumpStrH(S, B, "",                        "",         false, "");                   for (int i=0; i<nIO; i++) AddTxtFld(S, Separ1);                                             AddTxtFld(S, Separ1);   AddTxtFld(S, Separ1);    AddTxtFld(S, Separ2);    AddTxtLine(S, "");

      DumpStrH(S, B, "totHs@T.Est",             "kJ/s",     false, "");                   for (int i=0; i<nIO; i++) AddTxtFld(S, CpStr);/*"%.18g", IOs[i]->m_totHsTEst);*/            AddTxtFld(S, SumStr);   AddTxtFld(S, "");        AddTxtFld(S, "D");       AddTxtLine(S, "");
      DumpStrH(S, B, "Difference",              "%%",       false, "");                   for (int i=0; i<nIO; i++) AddTxtFld(S, CpDiffStr);/*"%.18g", IOs[i]->m_RDtotHsT*100);*/     AddTxtFld(S, "");       AddTxtFld(S, "");        AddTxtFld(S, "");        AddTxtLine(S, "");
      DumpStrH(S, B, "",                        "",         false, "");                   for (int i=0; i<nIO; i++) AddTxtFld(S, Separ1);                                             AddTxtFld(S, Separ1);   AddTxtFld(S, Separ1);    AddTxtFld(S, Separ2);    AddTxtLine(S, "");
      AddTxtLine(S, "");

      }
    }
  CopyTextToClipboard(pWnd ? pWnd : AfxGetMainWnd(), (LPTSTR)(LPCTSTR)S);
  };

//===========================================================================
//
//
//
//===========================================================================
