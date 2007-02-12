//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __MD_METHOD_CPP
#include "md_share1.h"
#include "md_defn.h"
#include "md_base.h"
#include "md_impl.h"
#include "md_method.h"
#include "m_base.h"
#include "bbtransfer.h"
#include "bbsurge.h"
//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
//
//
//===========================================================================

LPTSTR MethodBaseVersion="";

MUnitDefBase::MUnitDefBase(bool WithDevelopementChecks)
  {
  m_pClassDef=NULL;
  m_pDrawEntry=NULL;
  m_WithDevelopementChecks=WithDevelopementChecks;
  m_AddUsrPrefix=false;

#if DoDDBChecks
  if (m_WithDevelopementChecks)
    ::DevelopementChecksOn();
#endif
  }

MUnitDefBase::~MUnitDefBase()
  {
#if DoDDBChecks
  if (m_WithDevelopementChecks)
    ::DevelopementChecksOff();
#endif
  delete m_pClassDef;
  delete m_pDrawEntry;
  }

void MUnitDefBase::SetDefaultTag(LPCTSTR DefTag, bool AddUsrPrefix)
  {
  ASSERT(m_pClassDef!=NULL);
  m_pClassDef->sDefaultTag=DefTag;
  m_AddUsrPrefix=AddUsrPrefix;
  }

void MUnitDefBase::SetDrawing(LPCTSTR DrwGroup, double Drawing[])
  {
  delete m_pDrawEntry;
  m_pDrawEntry = new NodeDrawEntry((LPTSTR)m_pClassDef->ClassName(), MethodBaseVersion, (LPTSTR)DrwGroup, Drawing);
  m_pClassDef->sDrwGroup=DrwGroup;
  }

void MUnitDefBase::SetTreeDescription(LPCTSTR Desc)
  {
  m_pClassDef->sShortDesc=Desc;
  };

void MUnitDefBase::SetDescription(LPCTSTR Desc)
  {
  m_pClassDef->sDesc=Desc;
  };
//void MUnitDefBase::SetMethodFlags(dword Flags)
//  {
//  m_iMethodFlags=Flags;
//  };
//---------------------------------------------------------------------------

void MUnitDefBase::SetModelSolveMode(dword Mode)
  {
  dword DW = 0;
  if (Mode & MSolveMode_Probal) DW = (DW | TOC_PROBAL);
  if (Mode & MSolveMode_DynamicFlow) DW = (DW | TOC_DYNAMICFLOW);
  if (Mode & MSolveMode_DynamicFull) DW = (DW | TOC_DYNAMICFULL);
  //if (Mode & ??) DW = (DW | TOC_ELECTRICAL);
  m_pClassDef->dwCategory= (~TOC_SOLVE_MASK & m_pClassDef->dwCategory) | (TOC_SOLVE_MASK & DW);
  }

void MUnitDefBase::SetModelGroup(dword Group)
  {
  dword DW = TOC_GRP_GENERAL;
  if (Group & MGroup_Energy) DW = TOC_GRP_ENERGY;
  else if (Group & MGroup_Separation) DW = TOC_GRP_SEPAR;
  else if (Group & MGroup_PSD) DW = TOC_GRP_SIZEDST;
  else if (Group & MGroup_Transfer) DW = TOC_GRP_TRANSFER;
  else if (Group & MGroup_Alumina) DW = TOC_GRP_ALUMINA;
  else if (Group & MGroup_Electrical) DW = TOC_GRP_POWERDIST;
  m_pClassDef->dwCategory= (~TOC_GRP_MASK & m_pClassDef->dwCategory) | (TOC_GRP_MASK & DW);
  }

//undocumented License options...
const DWORD MLicense_QAL               = 0x00010000;
const DWORD MLicense_QALExtra          = 0x00020000;
const DWORD MLicense_Alcan             = 0x00040000;
const DWORD MLicense_RTTS              = 0x00080000;

void MUnitDefBase::SetModelLicense(dword License)
  {
  dword DW = TOC_SMDKRUNTIME; //ALL SMDK models MUST have this option set!!!
  if (License & MLicense_Standard) DW = (DW | TOC_STD_KENWALT);
  //if (License & MLicense_BlackBox) DW = (DW | TOC_SMDKRUNTIME);
  if (License & MLicense_HeatExchange) DW = (DW | TOC_HEATBAL);
  if (License & MLicense_ExtraHeatExchange) DW = (DW | TOC_HEATEXTRA);
  if (License & MLicense_PSD) DW = (DW | TOC_SIZEDIST);
  if (License & MLicense_Alumina) DW = (DW | TOC_ALUMINA);
  if (License & MLicense_Electrical) DW = (DW | TOC_POWERDIST);
  if (License & MLicense_User) DW = (DW | TOC_CLIENT);
  if (License & MLicense_RTTS) DW = (DW | TOC_RTTS);
  if (License & MLicense_Alcan) DW = (DW | TOC_ALCAN);
  if (License & MLicense_QALExtra) DW = (DW | TOC_QALEXTRA);
  if (License & MLicense_QAL) DW = (DW | TOC_QAL);
  m_pClassDef->dwCategory= (~TOC_MDL_MASK & m_pClassDef->dwCategory) | (TOC_MDL_MASK & DW);
  }

//---------------------------------------------------------------------------
//void MUnitDefBase::DevelopementChecksOn()
//  {
//  ::DevelopementChecksOn();
//  }
//void MUnitDefBase::DevelopementChecksOff()
//  {
//  ::DevelopementChecksOff();
//  }

//===========================================================================
//
//
//
//===========================================================================

MBaseMethodCommon::MBaseMethodCommon(MUnitDefBase *pUnitDef, TaggedObject * pNd) : MBaseDataCommon(pNd) 
  {
  m_pUnitDef=pUnitDef; 
  m_pImpl=NULL;
  m_pNd=dynamic_cast<MdlNode*>(pNd);
  //m_pDDB=NULL;
  //m_pDCB=NULL;
  //m_pVDB=NULL;
  m_pIODefs=NULL;
  //m_Utils.SetSize(0,8);
  };

//===========================================================================
//
//
//
//===========================================================================

class CTransferMethodClassDef : public TagObjClass
  {
  public:
    TaggedObject * ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
    TaggedObject * ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
    virtual TaggedObject * Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
    CTransferMethodClassDef(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClass, pchar pVersion_, pchar pDrwGroup_, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask);
  };

TaggedObject * CTransferMethodClassDef::ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrp(pClass, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CTransferMethodClassDef::ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrpDesc(pShortDesc, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CTransferMethodClassDef::Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  {
  CBBTransfer * pn = new CBBTransfer(this, pTag, pAttach, eAttach);
  SetTypeId(NULL); // will be set by Init
  if (SubClassId())
    pn->SetSubClass(SubClassId());

  return pn;
  };

CTransferMethodClassDef::CTransferMethodClassDef(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClassId_, pchar pVersion_, pchar pDrwGroup_, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask) : \
TagObjClass(pClassName, pGroup_, pClassId_, pSubClassId_, pVersion_, pDrwGroup_, pDefTag_, dwCat, pShortDesc, pDesc, dwSelectMask)
  {
//    sSubClassId=pSubClassId_;
  sMdlLibName=MDLLIBNAME;
  };

//---------------------------------------------------------------------------

static double DefaultDrawing[] = { DD_Poly, -5,-5, -5,5, 5,5, 5,-5, -5,-5, DD_End };

MTransferUnitDefBase::MTransferUnitDefBase(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR DLL, bool WithDevelopementChecks) :
  MUnitDefBase(WithDevelopementChecks)
  {
  if (stricmp(DLL,"ilukaunits")==0)
    {
    m_pClassDef=new CTransferMethodClassDef((LPTSTR)pClass, FlwUnitGrp, (LPTSTR)pClass, (LPTSTR)pSubClass, MethodBaseVersion, 
                       "Xfer", "X", TOC_PROBAL|TOC_DYNAMICFLOW|TOC_STD_KENWALT, (LPTSTR)ShortDesc, (LPTSTR)ShortDesc, 0xffffffff);
    }
  else if (stricmp(DLL,"alumina2")==0)
    {
    m_pClassDef=new CTransferMethodClassDef((LPTSTR)pClass, FlwUnitGrp, (LPTSTR)pClass, (LPTSTR)pSubClass, MethodBaseVersion, 
                       "Xfer", "X", TOC_PROBAL|TOC_DYNAMICFLOW|TOC_GRP_ALUMINA|TOC_ALUMINA, (LPTSTR)ShortDesc, (LPTSTR)ShortDesc, 0xffffffff);
    }
  else if (stricmp(DLL,"heatxch2")==0)
    {
    m_pClassDef=new CTransferMethodClassDef((LPTSTR)pClass, FlwUnitGrp, (LPTSTR)pClass, (LPTSTR)pSubClass, MethodBaseVersion, 
                       "Xfer", "X", TOC_PROBAL|TOC_DYNAMICFLOW|TOC_GRP_ENERGY|TOC_HEATBAL, (LPTSTR)ShortDesc, (LPTSTR)ShortDesc, 0xffffffff);
    }
  else
    {
    m_pClassDef=new CTransferMethodClassDef((LPTSTR)pClass, FlwUnitGrp, (LPTSTR)pClass, (LPTSTR)pSubClass, MethodBaseVersion,
                       "Xfer", "X", TOC_PROBAL|TOC_GRP_GENERAL|TOC_SMDKRUNTIME, (LPTSTR)ShortDesc, (LPTSTR)ShortDesc, 0xffffffff);
    }
  m_pClassDef->SetMdlLibName(DLL);
  m_pClassDef->SetSubConstruct(this);

  m_pDrawEntry = new NodeDrawEntry((LPTSTR)pClass, MethodBaseVersion, "SMDKXfer", DefaultDrawing);

  m_sClassName = pClass;
  }

//===========================================================================
//
//
//
//===========================================================================

class CSurgeMethodClassDef : public TagObjClass
    {
    public:
      TaggedObject * ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      TaggedObject * ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      virtual TaggedObject * Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      CSurgeMethodClassDef(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClass, pchar pVersion_, pchar pDrwGroup, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask);
    };

TaggedObject * CSurgeMethodClassDef::ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrp(pClass, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CSurgeMethodClassDef::ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrpDesc(pShortDesc, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CSurgeMethodClassDef::Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  {
  CBBSurge * pn = new CBBSurge(this, pTag, pAttach, eAttach);
  SetTypeId(NULL); // will be set by Init
  if (SubClassId())
    pn->SetSubClass(SubClassId());

  return pn;
  };

CSurgeMethodClassDef::CSurgeMethodClassDef(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClassId_, pchar pVersion_, pchar pDrwGroup, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask) : \
TagObjClass(pClassName, pGroup_, pClassId_, pSubClassId_, pVersion_, pDrwGroup, pDefTag_, dwCat, pShortDesc, pDesc, dwSelectMask)
  {
  //sSubClassId=pSubClassId_;
  sMdlLibName=MDLLIBNAME;
  };

//---------------------------------------------------------------------------

MSurgeUnitDefBase::MSurgeUnitDefBase(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR DLL, bool WithDevelopementChecks) :
  MUnitDefBase(WithDevelopementChecks)
  {
  m_pClassDef= new CSurgeMethodClassDef((LPTSTR)pClass, FlwUnitGrp, (LPTSTR)pClass, (LPTSTR)pSubClass, MethodBaseVersion,
                       "Tank", "S", TOC_PROBAL|TOC_GRP_GENERAL|TOC_SMDKRUNTIME, (LPTSTR)ShortDesc, (LPTSTR)ShortDesc, 0xffffffff);
  m_pClassDef->SetMdlLibName(DLL);
  m_pClassDef->SetSubConstruct(this);

  m_pDrawEntry = new NodeDrawEntry((LPTSTR)pClass, MethodBaseVersion, "SMDKSurge", DefaultDrawing);

  m_sClassName = pClass;
  //m_iMethodFlags = 0;
  }


//===========================================================================
//
//
//
//===========================================================================

MClosureInfo::MClosureInfo(CClosureInfoArray & CIA, MVector & RefMdl) : \
  m_CIA(CIA)
  {
  //m_DoFlows=DoFlows;
  };

bool MClosureInfo::DoFlows()                                              { return m_CIA.DoFlows();                   };
bool MClosureInfo::DoContent()                                            { return m_CIA.DoContent();                 };
//int  MClosureInfo::JoinID()                                             { return m_CI.m_JoinId;                     };
void MClosureInfo::AddHeatFlwIn(int Join, double HfFlow, double HsFlow)   { m_CIA[Join].m_HfGain+=HfFlow;  m_CIA[Join].m_HsGain+=HsFlow; };
void MClosureInfo::AddHeatFlwOut(int Join, double HfFlow, double HsFlow)  { m_CIA[Join].m_HfLoss+=HfFlow;  m_CIA[Join].m_HsLoss+=HsFlow; };
void MClosureInfo::AddPowerIn(int Join, double PowerIn)                   { m_CIA[Join].m_PowerIn+=PowerIn;           };
void MClosureInfo::AddMassFlwIn(int Join, MVector & MassFlow)             { m_CIA[Join].m_MassGain+=MassFlow.Mass();  };
void MClosureInfo::AddMassFlwOut(int Join, MVector & MassFlow)            { m_CIA[Join].m_MassLoss+=MassFlow.Mass();  };
void MClosureInfo::AddContentHeat(int Join, double Heat)                  { m_CIA[Join].m_ContentHeat+=Heat;          };
void MClosureInfo::AddContentMass(int Join, MVector & Mass)               { m_CIA[Join].m_ContentMass+=Mass.Mass();   };

//===========================================================================
//
//
//
//===========================================================================

MBaseMethod::MBaseMethod(MUnitDefBase * pUnitDef, TaggedObject * pNd) :
  MBaseMethodCommon(pUnitDef, pNd),
  DD(this),
  DX(this),
  DV(this),
  Joins(this),
  FlwIOs(this),
  CtrlIOs(this)
  {
  m_pImpl = new MBaseMethodImpl;
  }

//---------------------------------------------------------------------------

MBaseMethod::~MBaseMethod()
  {
  delete m_pImpl;
  }

//---------------------------------------------------------------------------

void MBaseMethod::Init(MdlNode * pNd)
  {
  m_pNd=pNd;
  Log.Init(m_pNd);
  Dbg.Init(m_pNd);
  m_pNd->Class()->SetTypeId(&typeid(*this));
  }

//---------------------------------------------------------------------------

void MBaseMethod::SetIODefinition(MInOutDefStruct * pDefs)
  {
  m_pIODefs=pDefs;
  m_nJoins=0;
  IOAreaRec IOA[100+1];
  for (int i=0; pDefs[i].m_sDesc; i++)
    {
    ASSERT_ALWAYS(i<100, "Too many IO Definitions in SetIODefinition()", __FILE__, __LINE__);
    IOAreaRec & N=IOA[i];
    MInOutDefStruct & D=pDefs[i];
    memset(&IOA[i], 0, sizeof(IOAreaRec));
    N.SetIODesc(D.m_sDesc);
    N.SetIOName(D.m_sName);

    if (D.m_dwOptions & MIO_In)   N.m_Dirn |= LIO_In;
    if (D.m_dwOptions & MIO_Out)  N.m_Dirn |= LIO_Out;

    N.m_Id      = D.m_lId;
    N.m_nIORqd  = D.m_nRqd;
    N.m_nIOMax  = D.m_nMax;
    N.m_dFracHgt = D.m_dFracHgt;

    m_nJoins    =  Max(m_nJoins, (MCN_IOMask & D.m_lCnId)+1);

    if (D.m_dwOptions & MIO_Material)       N.m_dwType = nc_MLnk;
    if (D.m_dwOptions & MIO_Control)        N.m_dwType = nc_CLnk;
    if (D.m_dwOptions & MIO_Electrical)     N.m_dwType = nc_ELnk;

    // TODO Fix these
    if (D.m_dwOptions & MIO_Transfer)       N.m_dwFlags |= dwIOSetXfer;//dwIOIsBuffer;
    if (D.m_dwOptions & MIO_Overflow)       N.m_dwFlags |= dwIOOverflow;
    if (D.m_dwOptions & MIO_GasVent)        N.m_dwFlags |= dwIOGasVent;
    if (D.m_dwOptions & MIO_PipeEntry)      N.m_dwFlags |= dwIOPipeEntry;
    if (D.m_dwOptions & MIO_PipeJoin)       N.m_dwFlags |= dwIOPipeJoin;
    if (D.m_dwOptions & MIO_ApertureHoriz)  N.m_dwFlags |= dwIOApertureHoriz;
    }

  memset(&IOA[i], 0, sizeof(IOAreaRec));

  m_pNd->AttachIOAreas(IOA, NULL, true);

  };

bool MBaseMethod::PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo)
  {
  if (!m_pNd->FlwNode::DoPropagateNetInfo(Ctrl, IONo, true))
    return false;

  MInOutDefStruct *pIODef;
  if (IONo>=0)
    {
    pIODef = m_pIODefs;
    int Id=m_pNd->IOId_Self(IONo);
    while (pIODef->m_sName!=NULL && pIODef->m_lId<Id)
      pIODef++;
    ASSERT(pIODef->m_lId==Id);
    }
  else
    pIODef=NULL;

  MInOutDefStruct *pDefs = m_pIODefs;
  for (int i=0; i<m_pNd->NoFlwIOs(); i++)
    {
    int Id=m_pNd->IOId_Self(i);
    while (pDefs->m_sName!=NULL &&  pDefs->m_lId<Id)
      pDefs++;
    ASSERT(pDefs->m_lId==Id);
    if (pIODef==NULL || ((MCN_IOMask & pDefs->m_lCnId)==(MCN_IOMask & pIODef->m_lCnId)))
      m_pNd->Nd_Rmt(i)->PropagateNetInfo(Ctrl, m_pNd->IOIONo_Rmt(i));
    }

  return true;
  }

//---------------------------------------------------------------------------

bool MBaseMethod::ConfigureJoins()
  {
  m_pNd->Init_NoJoins(m_nJoins);
  if (1) // default for all modes //m_pNd->NetProbalMethod() || m_pNd->SolveInlineMethod())
    {
    MInOutDefStruct *pDefs = m_pIODefs;
    for (int i=0; i<m_pNd->NoProcessIOs(); i++)
      {
      int Id=m_pNd->IOId_Self(i);
      while (pDefs->m_sName!=NULL &&  pDefs->m_lId<Id)
        pDefs++;
      ASSERT(pDefs->m_lId==Id);
      
      long CnId=(MCN_IOMask & pDefs->m_lCnId);
      if (MCN_Open & pDefs->m_lCnId)
        m_pNd->SetIO_Open(i, CnId, 
          /*Contents.Closed() && Contents.NetPressDamp()*/ false, 
          /*SolveInlineMethod() ? ESS_Denied :*/ ESS_Allowed);
      else if (MCN_Closed & pDefs->m_lCnId)
        m_pNd->SetIO_Closed(i, CnId);
      else // default
        m_pNd->SetIO_Join(i, CnId);
      }
    }
  else
    {
//    INCOMPLETECODE(__FILE__, __LINE__);
    }
  return true;
  }



//---------------------------------------------------------------------------

LPCTSTR MBaseMethod::getTag()                                 { return (LPCTSTR)(m_pNd->FullObjTag()); }
LPCTSTR MBaseMethod::getClassId()                             { return (LPCTSTR)(m_pNd->ClassId()); }
bool MBaseMethod::getDbgBrk()                                 { return m_pNd->fDoDbgBrk!=0; };

//---------------------------------------------------------------------------

bool MBaseMethod::getIsNetProbal()          { return m_pNd->NetProbalMethod()!=0; };
bool MBaseMethod::getIsNetDynamic()         { return m_pNd->NetDynamicMethod()!=0; };

bool MBaseMethod::getIsSolveDirect()        { return m_pNd->SolveDirectMethod()!=0; };
bool MBaseMethod::getIsSolveInline()        { return m_pNd->SolveInlineMethod()!=0; };
bool MBaseMethod::getIsSolveBuffered()      { return m_pNd->SolveBufferedMethod()!=0; };

double MBaseMethod::getTime()               { return ICGetTime(); };
double MBaseMethod::getDeltaTime()          { return ICGetTimeInc(); };
long MBaseMethod::getJoinMask()             { return m_lJoinMask; };

void MBaseMethod::SetIdleRequired(char* pNote/*=NULL*/)
  {
  if (pNote && pNote[0]!=0)
    LogError(m_pNd->FullObjTag(), 0, "SysCAD paused: %s", pNote); 
  else
    LogError(m_pNd->FullObjTag(), 0, "SysCAD paused by model"); 
  m_pNd->ExecObj()->XIdle();
  }

void MBaseMethod::SetStopRequired(char* pNote/*=NULL*/)
  { 
  if (pNote && pNote[0]!=0)
    LogError(m_pNd->FullObjTag(), 0, "SysCAD stopped: %s", pNote);
  else
    LogError(m_pNd->FullObjTag(), 0, "SysCAD stopped by model");
  m_pNd->ExecObj()->XStop(); 
  }

//---------------------------------------------------------------------------
//
//            Utilities
//
//---------------------------------------------------------------------------

void MBaseMethod::MakeUpNodeTransferAvail(int JoinId)
  {
  m_pNd->MakeUpNodeTransferAvail(JoinId);
  }
void MBaseMethod::MakeUpNodeTransferReqd(int JoinId)
  {
  m_pNd->MakeUpNodeTransferReqd(JoinId);
  }

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

double MJoin::getP()
  {
  return m_pNd->JoinPBlk(m_iJoin).P;
  };
void   MJoin::putP(double P)
  {
  if (m_pNd->NetProbalMethod())
    m_pNd->SetJoinPressure(m_iJoin, P, true, true);
  else
    {
    INCOMPLETECODE(__FILE__, __LINE__);
    //m_pNd->SetJoinPressure;
    }
  };

// OTHER Methods at Bottom of file
double MJoin::GetProbalPIn(MProbalPCtrl *pPC)                     { return m_pNd->MeasureJoinPressure(m_iJoin, pPC ? pPC->m_pPCtrl:NULL); };
void   MJoin::SetProbalP(MProbalPCtrl *pPC)                       { m_pNd->SetJoinPressure(m_iJoin, pPC ? pPC->m_pPCtrl:NULL); };
void   MJoin::SetProbalP(double P, bool DoInputs, bool DoOutputs) { m_pNd->SetJoinPressure(m_iJoin, P, DoInputs, DoOutputs); };
void   MJoin::EvalProbalP(MProbalPCtrl *pPC, SpModel * pMdl)      { m_pNd->EvalJoinPressure(m_iJoin, pPC ? pPC->m_pPCtrl:NULL, pMdl?pMdl:NULL); };

//---------------------------------------------------------------------------

long MJoins::getCount()                                       { return m_pNd->NoProcessJoins(); };
void MJoins::putCount(long N)                                 { return m_pNd->Init_NoJoins(N); };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

//long MBaseMethodImpl::NoFlwsIn()
//  {
//  if (m_NoFlwsIn<0)
//    {
//    int N=m_pNd->NoFlwIOs();
//    m_NoFlwsIn=0;
//    for (int i=0; i<N; i++)
//      {
//      if (m_pNd->IO_In(i))
//        m_NoFlwsIn++;
//      }
//    }
//  return m_NoFlwsIn;
//  }
//
////---------------------------------------------------------------------------
//
//long MBaseMethodImpl::NoFlwsOut()
//  {
//  if (m_NoFlwsOut<0)
//    {
//    int N=m_pNd->NoFlwIOs();
//    m_NoFlwsOut=0;
//    for (int i=0; i<N; i++)
//      {
//      if (m_pNd->IO_Out(i))
//        m_NoFlwsOut++;
//      }
//    }
//  return m_NoFlwsOut;
//  }

//===========================================================================

long    MFlow::getId()                   { return m_pNd->IOId_Self(m_iIONo); }
long    MFlow::getIdRmt()                { return m_pNd->IOId_Rmt(m_iIONo); };
bool    MFlow::getIsFlwIn()              { return m_pNd->IO_In(m_iIONo)!=0; };
bool    MFlow::getIsFlwOut()             { return m_pNd->IO_Out(m_iIONo)!=0; };
double  MFlow::getEstFlwIn()             { return m_pNd->IOQmEst_In(m_iIONo); };
double  MFlow::getEstFlwOut()            { return m_pNd->IOQmEst_Out(m_iIONo); };
double  MFlow::getFlwIn()                { return m_pNd->IOQm_In(m_iIONo); };
double  MFlow::getFlwOut()               { return m_pNd->IOQm_Out(m_iIONo); };


    //double         IOQm_In(int i)        { return IOFB(i,0)->GetQm(); };
    //double         IOQm_Out(int i)       { return -IOFB(i,0)->GetQm(); };
    //double         IOQmEst_In(int i)     { return IOFB(i,0)->GetQmEst(); };
    //double         IOQmEst_Out(int i)    { return -IOFB(i,0)->GetQmEst(); };
    //double         IOQmPrvPB_In(int i)   { return IOFB(i,0)->GetQmPrvPB(); };
    //double         IOQmPrvPB_Out(int i)  { return -IOFB(i,0)->GetQmPrvPB(); };
    //flag           IO_In(int i)          { return FlwPos(IOQmEst_In(i)); };
    //flag           IO_InGEZ(int i)       { return FlwGEZ(IOQmEst_In(i)); };
    //flag           IO_Out(int i)         { return FlwPos(IOQmEst_Out(i)); };
    //flag           IO_OutGEZ(int i)      { return FlwGEZ(IOQmEst_Out(i)); };
    //flag           IO_Zero(int i)        { return FlwZero(IOQmEst_In(i)); };

//double MFlowIOs::getEstFlwIO_Id(long Id, long Index/*=0*/)  { long i=getFlwIndexId   (Id, Index); return i>=0 ? m_pNd->IOQmEst_In(i):0; };
//double MFlowIOs::getEstFlwIn_Id(long Id, long Index/*=0*/)  { long i=getFlwIndexIdIn (Id, Index); return i>=0 ? m_pNd->IOQmEst_In(i):0; };
//double MFlowIOs::getEstFlwOut_Id(long Id, long Index/*=0*/) { long i=getFlwIndexIdOut(Id, Index); return i>=0 ? m_pNd->IOQmEst_Out(i):0; };
//double MFlowIOs::getFlwIO_Id(long Id, long Index/*=0*/)     { long i=getFlwIndexId   (Id, Index); return i>=0 ? m_pNd->IOQm_In(i):0; };
//double MFlowIOs::getFlwIn_Id(long Id, long Index/*=0*/)     { long i=getFlwIndexIdIn (Id, Index); return i>=0 ? m_pNd->IOQm_In(i):0; };
//double MFlowIOs::getFlwOut_Id(long Id, long Index/*=0*/)    { long i=getFlwIndexIdOut(Id, Index); return i>=0 ? m_pNd->IOQm_Out(i):0; };

bool    MFlow::getIsMakeUpReqd()     { return m_pNd->IOFlange(m_iIONo)->IsMakeUpReqd()!=0; };
bool    MFlow::getIsMakeUpReqdLcl()  { return m_pNd->IOFlange(m_iIONo)->IsMakeUpReqdLcl()!=0; };
bool    MFlow::getIsMakeUpAvail()    { return m_pNd->IOFlange(m_iIONo)->IsMakeUpAvail()!=0; };


double  MFlow::getMakeUpReqd()       { return m_pNd->IOFlange(m_iIONo)->MakeUpReqd();    };
double  MFlow::getMakeUpReqdLcl()    { return m_pNd->IOFlange(m_iIONo)->MakeUpReqdLcl(); };
double  MFlow::getMakeUpAvail()      { return m_pNd->IOFlange(m_iIONo)->MakeUpAvail();   };

void   MFlow::putMakeUpReqd(double Rqd)
  {
  if (Finite(Rqd))
    m_pNd->IOFlange(m_iIONo)->SetMakeUpReqd(Rqd);
  else
    m_pNd->IOFlange(m_iIONo)->ClrMakeUpReqd();
  };
void   MFlow::putMakeUpReqdLcl(double Rqd)
  {
  if (Finite(Rqd))
    m_pNd->IOFlange(m_iIONo)->SetMakeUpReqdLcl(Rqd);
  else
    m_pNd->IOFlange(m_iIONo)->ClrMakeUpReqdLcl();
  };
void   MFlow::putMakeUpAvail(double Avl)
  {
  if (Finite(Avl))
    m_pNd->IOFlange(m_iIONo)->SetMakeUpAvail(Avl);
  else
    m_pNd->IOFlange(m_iIONo)->ClrMakeUpAvail();
  };


double   MFlow::getAbsHeight()
  {
  INCOMPLETECODE(__FILE__, __LINE__)
  //return m_pNd->IORelFracHgt_Flng(m_iIONo)*m_pSDD[iSurge].m_dShpHeight;
  return 0;
  };
void     MFlow::putAbsHeight(double H)
  {
  INCOMPLETECODE(__FILE__, __LINE__)
  //m_pSDD[iSurge].m_IOIdMask|=1<<m_pNd->IOId_Self(iIO);
  //m_pNd->SetIORelFracHgt_Flng(iIO, H/GTZ(m_pSDD[iSurge].m_dShpHeight));
  };
double    MFlow::getRelHeight()
  {
  return m_pNd->IORelFracHgt_Flng(m_iIONo);
  };
void     MFlow::putRelHeight(double H)
  {
  INCOMPLETECODE(__FILE__, __LINE__)
  //m_pSDD[iSurge].m_IOIdMask|=1<<m_pNd->IOId_Self(m_iIONo);
  //m_pNd->SetIORelFracHgt_Flng(iIO, H);
  };

//===========================================================================

MTearPriorities MFlow::getTearPriority()                { return (MTearPriorities)m_pNd->IOFlange(m_iIONo)->TearPriority();        };
void MFlow::putTearPriority(MTearPriorities Priority)   { m_pNd->IOFlange(m_iIONo)->SetTearPriority(Priority);    };
MTearTypes MFlow::getTearTypeRequired()                 { return (MTearTypes)m_pNd->IOFlange(m_iIONo)->RqdTearType();         };
void MFlow::putTearTypeRequired(MTearTypes Type)        { m_pNd->IOFlange(m_iIONo)->SetRqdTearType(Type);         };

//===========================================================================

double          MFlowBlk::getDPb()                        {  return m_pNd->IOFB(m_iIONo,0)->m_dPb;  }
void            MFlowBlk::putDPb(double DP)               {  m_pNd->IOFB(m_iIONo,0)->SetDPb(DP, 0); }
void            MFlowBlk::SetDPb(double DP, double dDP)   {  m_pNd->IOFB(m_iIONo,0)->SetDPb(DP, dDP);  }

double          MFlowBlk::getDPq()                        {  return m_pNd->IOFB(m_iIONo,0)->m_dPb;  }
void            MFlowBlk::putDPq(double DP)               {  m_pNd->IOFB(m_iIONo,0)->SetDPq(DP, 0); }
void            MFlowBlk::SetDPq(double DP, double dDP)   {  m_pNd->IOFB(m_iIONo,0)->SetDPq(DP, dDP);  }

double          MFlowBlk::getQm()                         {  return m_pNd->IOFB(m_iIONo,0)->m_Qm;  }
double          MFlowBlk::getQmEst()                      {  return m_pNd->IOFB(m_iIONo,0)->m_QmEst;  }

double          MFlowBlk::getDPz()                        {  return m_pNd->IOFB(m_iIONo,0)->m_dPz;  }
void            MFlowBlk::putDPz(double DP)               {  m_pNd->IOFB(m_iIONo,0)->SetDPz(DP, 0); }
void            MFlowBlk::SetDPz(double DP, double dDP)   {  m_pNd->IOFB(m_iIONo,0)->SetDPz(DP, dDP);  }

void            MFlowBlk::SetQmFree()                     {  m_pNd->IOFB(m_iIONo,0)->SetQmFree(); }
void            MFlowBlk::SetXferMode()                   {  m_pNd->IOFB(m_iIONo,0)->SetXferMode(); };
void            MFlowBlk::SetXferCapacity(double QmCap)   {  m_pNd->IOFB(m_iIONo,0)->SetXferCapacity(QmCap); }
void            MFlowBlk::SetQmReqd(double QmReqd, bool IsDegFree, float MaxNode, float MaxFlng) {  m_pNd->IOFB(m_iIONo,0)->SetQmReqd(QmReqd, IsDegFree, MaxNode, MaxFlng); }

void            MFlowBlk::SetDerivsBad(bool On)           {  m_pNd->IOFB(m_iIONo,0)->SetDerivsBad(On); }
void            MFlowBlk::SetFunctOfPress(bool On)        {  m_pNd->IOFB(m_iIONo,0)->SetFunctOfPress(On); };


double          MFlowBlk::MeanPress()                     {  return m_pNd->IOFB(m_iIONo,0)->MeanPress(); };
double          MFlowBlk::MeanFlngPress()                 {  return m_pNd->IOFB(m_iIONo,0)->MeanFlngPress(); }
double          MFlowBlk::MeanNodePress()                 {  return m_pNd->IOFB(m_iIONo,0)->MeanNodePress(); }
double          MFlowBlk::MeanFBPress()                   {  return m_pNd->IOFB(m_iIONo,0)->MeanFBPress(); };

double          MFlowBlk::getVelocity()                   {  return m_pNd->IOFB(m_iIONo,0)->Velocity(); };
                                             
double          MFlowBlk::getRegulation()                 {  return m_pNd->IOFB(m_iIONo,0)->Regulation(); };
void            MFlowBlk::putRegulation(double A)         {  return m_pNd->IOFB(m_iIONo,0)->SetRegulation(A); };

int             MFlowBlk::QmSign()                                        {  return m_pNd->IOFB(m_iIONo,0)->QmSign(); };
double          MFlowBlk::QmMeas(double Scl)                              {  return m_pNd->IOFB(m_iIONo,0)->QmMeas(Scl); };
double          MFlowBlk::QvMeas(double Scl)                              {  return m_pNd->IOFB(m_iIONo,0)->QvMeas(Scl); };                       
double          MFlowBlk::VelMeas(double Scl)                             {  return m_pNd->IOFB(m_iIONo,0)->VelMeas(Scl); };                       
double          MFlowBlk::DQmMeas(double Scl)                             {  return m_pNd->IOFB(m_iIONo,0)->DQmMeas(Scl); };                       
double          MFlowBlk::RangeFlow(double Qm, double LoLimit)            {  return FlwBlk::RangeFlow(Qm, LoLimit); };          
double          MFlowBlk::SetQmMeasRange(double Rho, double LoQm)         {  return m_pNd->IOFB(m_iIONo,0)->SetQmMeasRange(Rho, LoQm); };                   
double          MFlowBlk::SetQvMeasRange(double Rho, double LoVol)        {  return m_pNd->IOFB(m_iIONo,0)->SetQvMeasRange(Rho, LoVol); };     
double          MFlowBlk::SetVelMeasRange(double Rho, double Area, double LoVel)   {  return m_pNd->IOFB(m_iIONo,0)->SetVelMeasRange(Rho, Area, LoVel); }; 

MPressureBlk    MFlowBlk::getNodePBlk()              { return MPressureBlk(&m_pNd->IOFB(m_iIONo,0)->NodePB()); };
MPressureBlk    MFlowBlk::getFlngPBlk()              { return MPressureBlk(&m_pNd->IOFB(m_iIONo,0)->FlngPB()); };



//===========================================================================

double MPressureBlk::getPEst()                      { return m_pPB->PEst; };

//===========================================================================

//void MFlowIOs::CheckMIOStreams()
//  {
//  long N=m_pNd->NoIOs();
//  MStreams &C=m_pImpl->m_IOStreams;
//  if (!m_pImpl->m_bIOStreamsValid)
//    {
//    C.SetSize(N);
//    for (int i=0; i<N; i++)
//      C[i].Attach(m_pNd->IOConduit(i));
//    m_pImpl->m_bIOStreamsValid=true;
//    }
//  // Copy Size Data Here
//  }

//---------------------------------------------------------------------------

long MFlowIOs::getCount(long Id)
  {
  long N=m_pNd->NoIOs();
  if (Id<0)
    return N;
  long Cnt=0;
  for (long i=0; i<N; i++)
    if (m_pNd->IOId_Self(i)==Id)
      Cnt++;
  return Cnt;
  }

//---------------------------------------------------------------------------

long MFlowIOs::getFirst(long Id)
  {
  if (Id<0)
    return 0;
  long N=m_pNd->NoIOs();
  for (long i=0; i<N; i++)
    if (m_pNd->IOId_Self(i)==Id)
      return i;
  return -1;
  }

//---------------------------------------------------------------------------

long MFlowIOs::getNext(long CurIndex, long Id)
  {
  long N=m_pNd->NoIOs();
  for (long i=CurIndex+1; i<N; i++)
    if (m_pNd->IOId_Self(i)==Id)
      return i;
  return -1;
  }

//---------------------------------------------------------------------------

long MFlowIOs::getFirst_In(long Id)
  {
  long N=m_pNd->NoIOs();
  for (long i=0; i<N; i++)
    if ((Id<0 || m_pNd->IOId_Self(i)==Id) && m_pNd->IO_In(i))
      return i;
  return -1;
  }

//---------------------------------------------------------------------------

long MFlowIOs::getNext_In(long CurIndex, long Id)
  {
  long N=m_pNd->NoIOs();
  for (long i=CurIndex+1; i<N; i++)
    if ((Id<0 || m_pNd->IOId_Self(i)==Id) && m_pNd->IO_In(i))
      return i;
  return -1;
  }

//---------------------------------------------------------------------------

long MFlowIOs::getFirst_Out(long Id)
  {
  long N=m_pNd->NoIOs();
  for (long i=0; i<N; i++)
    if ((Id<0 || m_pNd->IOId_Self(i)==Id) && m_pNd->IO_Out(i))
      return i;
  return -1;
  }

//---------------------------------------------------------------------------

long MFlowIOs::getNext_Out(long CurIndex, long Id)
  {
  long N=m_pNd->NoIOs();
  for (long i=CurIndex+1; i<N; i++)
    if ((Id<0 || m_pNd->IOId_Self(i)==Id) && m_pNd->IO_Out(i))
      return i;
  return -1;
  }

//---------------------------------------------------------------------------

//long   MFlowIOs::getId(long Index/*=0*/)                   { return m_pNd->IOId_Self(Index); };
//---------------------------------------------------------------------------

//MContainer MBaseMethod::getContainer(long index)
//  {
//  long N=m_pNd->NoContainers();
//  if (index>=0 && index<N)
//    {
//    //MContainers &M=m_pImpl->m_SpContainers;
//    //if (M.GetSize()<N)
//    //  {
//    //  M.SetSize(N);
//    //  for (int i=0; i<N; i++)
//    //    M[i].Attach(m_pNd->Container(index));
//    //  }
//
//    return MContainer(m_pNd->Container(index));
//    }
//  return MContainer();
//  }

//---------------------------------------------------------------------------

MStream  MFlow::getStream()
  {
  if (m_iIONo>=0 && m_iIONo<m_pNd->NoIOs())
    return MStream(m_pNd->IOConduit(m_iIONo));
  return MStream();
  };

//---------------------------------------------------------------------------
//
//MStream MFlowIOs::getStreamIn(long Index/*= 0*/)
//  {
//  //CheckMIOStreams();
//  long N=m_pNd->NoIOs();
//  for (long i=0; i<N; i++)
//    if (m_pNd->IO_In(i) && (--Index<0))
//      return MStream(m_pNd->IOConduit(i));
//  return MStream();
//  };
//
////---------------------------------------------------------------------------
//
//MStream MFlowIOs::getStreamOut(long Index/*= 0*/)
//  {
//  //CheckMIOStreams();
//  long N=m_pNd->NoIOs();
//  for (long i=0; i<N; i++)
//    if (m_pNd->IO_Out(i) && (--Index<0))
//      return MStream(m_pNd->IOConduit(i));
//  return MStream();
//  };
//
//---------------------------------------------------------------------------

void MFlow::SetOpen(long iJoinId, bool Damped)            { m_pNd->SetIO_Open(m_iIONo, iJoinId, Damped, ESS_Allowed); };
void MFlow::SetClosed(long iJoinId)                       { m_pNd->SetIO_Closed(m_iIONo, iJoinId); };
void MFlow::SetJoin(long iJoinId)                         { m_pNd->SetIO_Join(m_iIONo, iJoinId); };
void MFlow::SetP(double P)                                { m_pNd->Set_IOP_Self(m_iIONo, P); };
void MFlow::SetP(double P, double SLDensity, double Head) { m_pNd->Set_IOP_RhoH_Self(m_iIONo, P, SLDensity, Head); };
//void MFlowIOs::SetIOP_Id(long Id, long Index, double P) { m_pNd->Set_IOP_Self(FlwIndexId[Id][Index], P); };
//void MFlowIOs::SetIOP_Id(long Id, long Index, double P, double SLDensity, double Head) { m_pNd->Set_IOP_RhoH_Self(Index, P, SLDensity, Head); };

double MFlow::getIOPJoin()        { return m_pNd->IOP_Self(m_iIONo); };
double MFlow::getIOPFlange()      { return m_pNd->IOP_Flng(m_iIONo); };
double MFlow::getDatum()          { return m_pNd->IODatum_Term(m_iIONo); };

//---------------------------------------------------------------------------

//double MFlowIOs::AddMixtureIn(MStream & S, long Count/*=255*/, DWORD PhaseM/*=MP_All*/)
//double MFlowIOs::AddMixtureIn(MStream & S, DWORD PhaseM/*=MP_All*/)
//  {
//  S.CheckAttached();
//  //Cd.SetToZero();
//  DWORD IOIds=0xffffffff;
//  m_pNd->SigmaQInPMin(S.Cd, som_ALL, IOIds);
//  return S.Cd.QMass();
//  };

//---------------------------------------------------------------------------

//double MFlowIOs::AddMixtureIn_Id(MStream & S, long Id/*=-1*/, long Count/*=255*/, DWORD PhaseM/*=MP_All*/)
double MFlowIOs::AddMixtureIn_Id(MStream & S, long Id/*=-1*/, DWORD PhaseM/*=MP_All*/)
  {
  S.CheckAttached();
  //Cd.SetToZero();
  ASSERT(Id<32);
  DWORD IOIds=(Id<0)?0xffffffff:(1<<Id);

  m_pNd->SigmaQInPMin(S.Cd, som_ALL, IOIds);
  return S.Cd.QMass();
  };

//---------------------------------------------------------------------------

//long     MFlowIOs::MSurgeIF::getCount() { return m_nSrg; };
//void     MFlowIOs::MSurgeIF::putCount(long n)
//  {
//  for (int i=0; i<n; i++)
//    {
//    m_pSDD[i].m_IOIdMask=0;
//    m_pSDD[i].m_dLowestIOPt=0;
//    m_pSDD[i].m_dShpBaseFromDatum=0;
//    m_pSDD[i].m_dShpDatumPt=0;
//    m_pSDD[i].m_dShpHeight=1;
//    }
//
//  m_nSrg=n;
//  m_pSDD[n].m_IOIdMask=0;
//  }
//double   MFlowIOs::MSurgeIF::getHeight(long iSurge)
//  {
//  return m_pSDD[iSurge].m_dShpHeight;
//  };
//void     MFlowIOs::MSurgeIF::putHeight(long iSurge, double H)
//  {
//  m_pSDD[iSurge].m_dShpHeight=H;
//  };
//double   MFlowIOs::MSurgeIF::getAbsHeight(long iSurge, long iIO)
//  {
//  return m_pNd->IORelFracHgt_Flng(iIO)*m_pSDD[iSurge].m_dShpHeight;
//  };
//void     MFlowIOs::MSurgeIF::putAbsHeight(long iSurge, long iIO, double H)
//  {
//  m_pSDD[iSurge].m_IOIdMask|=1<<m_pNd->IOId_Self(iIO);
//  m_pNd->SetIORelFracHgt_Flng(iIO, H/GTZ(m_pSDD[iSurge].m_dShpHeight));
//  };
//double   MFlowIOs::MSurgeIF::getRelHeight(long iSurge, long iIO)
//  {
//  return m_pNd->IORelFracHgt_Flng(iIO);
//  };
//void     MFlowIOs::MSurgeIF::putRelHeight(long iSurge, long iIO, double H)
//  {
//  m_pSDD[iSurge].m_IOIdMask|=1<<m_pNd->IOId_Self(iIO);
//  m_pNd->SetIORelFracHgt_Flng(iIO, H);
//  };

//---------------------------------------------------------------------------

long MCtrlIOs::getNoCtrlIOs() { return m_pNd->NoCIOs(); };

//===========================================================================
//
//
//
//===========================================================================

//class DllImportExport CReactionBase
//  {
//  protected:
//    flag           bOn;
//    flag           fEnabled;
//    CReactionBlock * pRB;
//    pFlwNode       pNd;
//  public:
//    CReactionBase(pFlwNode pAttach) { pRB=NULL; pNd=pAttach; fEnabled=bOn=0; };
//    ~CReactionBase() { delete pRB; };
//
//    CReactionBlock * operator() () { return bOn ? pRB : NULL; };
//
//    flag           Open();
//    flag           Load(char * pRCTFn) { return Open() ? pRB->Load(pRCTFn) : False; };
//    void           Close();
//    void           Enable() { bOn=1; };
//    void           Disable() { bOn=0; };
//
//    flag           Enabled() { return fEnabled && (pRB!=NULL) && bOn; };
//    flag           OnAndOK() { return Enabled() && pRB->OnAndOK(); };
//    void           Add_OnOff(DataDefnBlk &DDB, dword Flags=isParmStopped);
//    void           BuildDataDefn(DataDefnBlk &DDB, pchar pTag_=NULL, pchar pTagComment=NULL, DDBPages PageIs = DDB_OptPage, dword UserInfo=0);
//    flag           DataXchg(DataChangeBlk & DCB);
//
//    flag           ValidateData(ValidateDataBlk & VDB) { return Enabled() ? pRB->ValidateData(VDB) : True; };
//
//    double         HfSumTot(bool Pr=false)   { return Enabled() ? pRB->HfSumTot(Pr) : 0.0; };
//    double         HfSumSrc(bool Pr=false)   { return Enabled() ? pRB->HfSumSrc(Pr) : 0.0; };
//    double         HfSumSnk(bool Pr=false)   { return Enabled() ? pRB->HfSumSnk(Pr) : 0.0; };
//    //double         H0SumSrc()   { return Enabled() ? pRB->H0SumSrc() : 0.0; };
//    //double         H0SumSnk()   { return Enabled() ? pRB->H0SumSnk() : 0.0; };
//    //double         HsSumSrc()   { return Enabled() ? pRB->HsSumSrc() : 0.0; };
//    //double         HsSumSnk()   { return Enabled() ? pRB->HsSumSnk() : 0.0; };
//    double         HfSumRct(bool Pr=false)   { return Enabled() ? pRB->HfSumRct(Pr) : 0.0; };
//    double         HfErr(bool Pr=false)      { return Enabled() ? pRB->HfErr(Pr)    : 0.0; };
//    double         HsSumHX()    { return Enabled() ? pRB->HsSumHX()  : 0.0; };
//    double         HsSumPwr()   { return Enabled() ? pRB->HsSumPwr() : 0.0; };
//    SpMArray *     MsSumSrc()   { return Enabled() ? pRB->MsSumSrc() : NULL; };
//    SpMArray *     MsSumSnk()   { return Enabled() ? pRB->MsSumSnk() : NULL; };
//
//    double         EstFinalT()  { return Enabled() ? pRB->EstFinalT() : Std_T; };
//
//    void           StartStep()      { if (Enabled()) pRB->StartStep(); };
//    void           EvalProducts(SpConduit & C, double EstFinalT=dNAN/*, bool FindFinalT=false*/) { if (Enabled()) pRB->EvalProducts(C, EstFinalT/*, FindFinalT*/); };
//    void           EvalDerivs(SpContainer & M, SpConduit & Fd, SpConduit & Pr) { if (Enabled()) pRB->EvalDerivs(M, Fd, Pr); };
//    void           ConvergeStates(SpContainer & M, SpConduit & Fd, SpConduit & Pr) { if (Enabled()) pRB->ConvergeStates(M, Fd, Pr); };
//    void           EvalDiscrete(SpContainer & M, SpConduit & Fd, SpConduit & Pr) { if (Enabled()) pRB->EvalDiscrete(M, Fd, Pr); };
//#if DOPBSMOOTHING
//    void           GetMsAGData(CMSJoin & AG)              { if (Enabled()) pRB->GetMsAGData(AG); };
//#endif
//
//    void           OnAppActivate(BOOL bActive)            { if (Enabled()) pRB->OnAppActivate(bActive); };
//    int            FilesUsed(CFilesUsedArray & Files)      { return pRB ? pRB->FilesUsed(Files) : 0; };
//    flag           GetOtherData(FilingControlBlock &FCB)  { return pRB ? pRB->GetOtherData(FCB) : false; };
//    flag           PutOtherData(FilingControlBlock &FCB)  { return pRB ? pRB->PutOtherData(FCB) : false; };
//  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

MMethodUtility::MMethodUtility(MBaseMethodCommon *pCom, LPCTSTR Tag) : \
  MBaseMethodCommonRef(pCom),
  m_sTag(Tag)
  {
//  m_pCommon=pCom;
  m_pCommon->m_Utils.Add(this);
  };
MMethodUtility::~MMethodUtility()
  {
  MUtilArray &U=m_pCommon->m_Utils;
  for (int i=0; i<U.GetSize(); i++)
    if (U[i]==this)
      {
      U.RemoveAt(i);
      break;
      };
  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

MProbalPCtrl::MProbalPCtrl(MBaseMethodCommon *pCom, LPCTSTR Tag, long Method, double PRqd, bool MdlAvail) : \
MMethodUtility(pCom, Tag)
  {
  m_pPCtrl= new CRqdPressCtrl(Method, PRqd, MdlAvail);
  };

MProbalPCtrl::~MProbalPCtrl()
  {
  delete m_pPCtrl;
  };

void    MProbalPCtrl::BuildDataFields(long Allowed) { m_pPCtrl->BuildDataDefn(*m_pCommon->m_pDDB, m_pNd, true, Allowed, 0); };
void    MProbalPCtrl::CalculateResult()             { m_pPCtrl->CalculateResult() ; };

long    MProbalPCtrl::getMethod()                   { return m_pPCtrl->Method(); };
void    MProbalPCtrl::putMethod(long Method)        { m_pPCtrl->SetMethod(Method); };
double  MProbalPCtrl::getPRqd()                     { return m_pPCtrl->PRqd(); };
void    MProbalPCtrl::putPRqd(double PRqd)          { m_pPCtrl->SetPRqd(PRqd); };
double  MProbalPCtrl::getTRqd()                     { return m_pPCtrl->TRqd(); };
void    MProbalPCtrl::putTRqd(double TRqd)          { m_pPCtrl->SetTRqd(TRqd); };
double  MProbalPCtrl::getPMeas()                    { return m_pPCtrl->PMeas(); };
void    MProbalPCtrl::putPMeas(double PMeas)        { m_pPCtrl->SetPMeas(PMeas); };
double  MProbalPCtrl::getP2Set()                    { return m_pPCtrl->P2Set(); };
void    MProbalPCtrl::putP2Set(double P2Set)        { m_pPCtrl->SetP2Set(P2Set); };

void    MProbalPCtrl::SetP2Set(double P2Set, bool DoInputs, bool DoOutputs) { m_pPCtrl->SetP2Set(P2Set, DoInputs, DoOutputs); };

/*
    void BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pObj, long Allowed);
    void Set(long Method, double PRqd, bool MdlAvail)
      {
      m_iMethod    = Method;
      m_fPRqd      = PRqd;
      m_fTRqd      = Std_T;
      m_fMdlAvail  = MdlAvail;
      }
    void SetMdl(SpModelOwner *pMdl)
      {
      m_pMdl       = pMdl;
      m_fMdlAvail  = pMdl!=NULL;
      }
    void CalculateResult();

    void SetMethod(long Method) { m_iMethod = Method; }
    void SetPRqd(double PRqd)   { m_fPRqd   = PRqd; }
    void SetTRqd(double TRqd)   { m_fTRqd   = TRqd; }
    void SetPMeas(double PMeas) { m_fPMeas  = PMeas; }
    void SetP2Set(double P2Set, bool DoInputs, bool DoOutputs)
      {
      m_fP2Set = P2Set;
      m_fDoInputs = DoInputs;
      m_fDoOutputs = DoOutputs;
      };
    long Method()                       { return m_iMethod; }
    double PRqd()                       { return m_fPRqd; };
    double TRqd()                       { return m_fTRqd; };
    double PMeas()                      { return m_fPMeas; };
    double P2Set()                      { return m_fP2Set; };
    bool   DoInputs()                   { return m_fDoInputs; };
    bool   DoOutputs()                  { return m_fDoOutputs; };
    SpModelOwner *Mdl()                 { return m_pMdl; };

    flag   IgnoreLoQm()                 { return m_fIgnoreLoQm; };
    void   SetIgnoreLoQm(flag On)       { m_fIgnoreLoQm=On; };
    double LowQmFrac()                  { return m_fLowQmFrac; };
    void   SetLowQmFrac(double LoQm)    { m_fLowQmFrac=LoQm; };
    double LowQmFracNC()                { return m_fLowQmFracNC; };
    void   SetLowQmFracNC(double LoQm)  { m_fLowQmFracNC=LoQm; };

*/
//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

MReactionBlk::MReactionBlk(MBaseMethodCommon *pCom, bool IsSurgeType, LPCTSTR Tag) : \
MMethodUtility(pCom, Tag)
  {
  m_pRB = new CReactionBase(m_pNd, IsSurgeType);
  m_pRB->Open();
  m_pRB->Enable();
  };

MReactionBlk::~MReactionBlk()
  {
  delete m_pRB;
  };

bool    MReactionBlk::Open()                      { return m_pRB->Open()!=0; };
bool    MReactionBlk::Load(LPCTSTR pRCTFn)        { return m_pRB->Load((LPSTR)pRCTFn)!=0; };
void    MReactionBlk::Close()                     { m_pRB->Close()   ; };
void    MReactionBlk::Enable()                    { m_pRB->Enable()  ; };
void    MReactionBlk::Disable()                   { m_pRB->Disable() ; };

bool    MReactionBlk::Enabled()                   { return m_pRB->Enabled()!=0; };
bool    MReactionBlk::OnAndOK()                   { return m_pRB->OnAndOK()!=0; };
void    MReactionBlk::OnOffCheckBox(DWORD Flags)  { m_pRB->Add_OnOff(*m_pCommon->m_pDDB, Flags); };;

void    MReactionBlk::BuildDataFields()           { if (m_pRB->Enabled()) m_pRB->BuildDataDefn(*m_pCommon->m_pDDB, (LPSTR)(LPCTSTR)m_sTag); };
bool    MReactionBlk::ExchangeDataFields()        { return m_pRB->DataXchg(*m_pCommon->m_pDCB)!=0; };
bool    MReactionBlk::ValidateDataFields()        { return m_pRB->ValidateData(*m_pCommon->m_pVDB)!=0; };

double  MReactionBlk::HfSumTot(bool RefToProd)    { return m_pRB->HfSumTot(RefToProd); };
double  MReactionBlk::HfSumSrc(bool RefToProd)    { return m_pRB->HfSumSrc(RefToProd); };
double  MReactionBlk::HfSumSnk(bool RefToProd)    { return m_pRB->HfSumSnk(RefToProd); };
double  MReactionBlk::HfSumRct(bool RefToProd)    { return m_pRB->HfSumRct(RefToProd); };
double  MReactionBlk::HfErr(bool RefToProd)       { return m_pRB->HfErr(RefToProd); };
double  MReactionBlk::HsSumHX()                   { return m_pRB->HsSumHX();        };
double  MReactionBlk::HsSumPwr()                  { return m_pRB->HsSumPwr();       };

double  MReactionBlk::EstFinalT()                 { return m_pRB->EstFinalT(); };

void    MReactionBlk::EvalProducts(MStream & S, double EstFinalT) { m_pRB->EvalProducts(S.Cd, EstFinalT); };

void    MReactionBlk::OnAppActivate(bool bActive)           { m_pRB->OnAppActivate(bActive); };
int     MReactionBlk::FilesUsed(CFilesUsedArray & Files)     { return m_pRB->FilesUsed(Files); };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

MEnvironHXBlk::MEnvironHXBlk(MBaseMethodCommon *pCom, dword Options, LPCTSTR Tag) : \
MMethodUtility(pCom, Tag)
  {
  m_pEHX = new CEnvironHXBase(m_pNd, (LPTSTR)Tag);
  m_pEHX->Open();
  //m_pEHX->Enable();
  };

MEnvironHXBlk::~MEnvironHXBlk()
  {
  delete m_pEHX;
  };                        
           
//bool    MEnvironHXBlk::Open(bool Fixed)         { return m_pEHX->Open(m_pNd, Fixed!=0)!=0; };
//void    MEnvironHXBlk::Close()                  { m_pEHX->Close()   ; };
void    MEnvironHXBlk::Enable()                 { m_pEHX->Open(&CEHX_LossPerQmClass)  ; };
void    MEnvironHXBlk::Disable()                { m_pEHX->Close() ; };

bool    MEnvironHXBlk::Enabled()                { return m_pEHX->Enabled()!=0; };
//bool    MEnvironHXBlk::OnAndOK()                { return m_pEHX->OnAndOK()!=0; };
void    MEnvironHXBlk::OnOffCheckBox(DWORD Flags)  { m_pEHX->Add_OnOff(*m_pCommon->m_pDDB, Flags); };;
void    MEnvironHXBlk::BuildDataFields()        { if (m_pEHX->Enabled()) m_pEHX->BuildDataDefn(*m_pCommon->m_pDDB, (LPSTR)(LPCTSTR)m_sTag); };
bool    MEnvironHXBlk::ExchangeDataFields()     { return m_pEHX->DataXchg(*m_pCommon->m_pDCB)!=0; };
bool    MEnvironHXBlk::ValidateDataFields()     { return m_pEHX->ValidateData(*m_pCommon->m_pVDB)!=0; };

void MEnvironHXBlk::EvalProducts(MStream &S, double FinalTEst)
  { m_pEHX->EvalProducts(S, FinalTEst); };
void MEnvironHXBlk::EvalProducts(MStream &S, double Len, double Diam, double FinalTEst)
  { m_pEHX->EvalProductsPipe(S, Len, Diam, FinalTEst); };

double MEnvironHXBlk::HeatFlow()
  { return m_pEHX->HeatFlow(); };

//double MEnvironHXBlk::SaturationTotalP(MVector & Mdl, double T)
//  { return m_pEHX->SaturationTotalP(T, *Mdl.SpMdl); };
//
//void    MEnvironHXBlk::QMVapFlash(MStream &Q, double VapMass, double Duty, DWORD Flags)
//  { return m_pEHX->QMVapFlash(Q.Cd, VapMass, Duty, Flags); };
//void    MEnvironHXBlk::QMVapFlash(MStream &Ql, MStream &Qv, double VapMass, double Duty, DWORD Flags)
//  { return m_pEHX->QMVapFlash(Ql.Cd, Qv.Cd, VapMass, Duty, Flags); };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

MVLEBlk::MVLEBlk(MBaseMethodCommon *pCom, dword Options, LPCTSTR Tag) : \
MMethodUtility(pCom, Tag)
  {
  m_pVLE = new CVLEBase(m_pNd, Options);
  m_pVLE->Open();
  m_pVLE->Enable();
  };

MVLEBlk::~MVLEBlk()
  {
  delete m_pVLE;
  };

//bool    MVLEBlk::Open(bool Fixed)         { return m_pVLE->Open(m_pNd, Fixed!=0)!=0; };
//void    MVLEBlk::Close()                  { m_pVLE->Close()   ; };
void    MVLEBlk::Enable()                 { m_pVLE->Enable()  ; };
void    MVLEBlk::Disable()                { m_pVLE->Disable() ; };

bool    MVLEBlk::Enabled()                { return m_pVLE->Enabled()!=0; };
//bool    MVLEBlk::OnAndOK()                { return m_pVLE->OnAndOK()!=0; };
void    MVLEBlk::OnOffCheckBox(DWORD Flags)  { m_pVLE->Add_OnOff(*m_pCommon->m_pDDB, Flags); };;
void    MVLEBlk::BuildDataFields()        { if (m_pVLE->Enabled()) m_pVLE->BuildDataDefn(*m_pCommon->m_pDDB, (LPSTR)(LPCTSTR)m_sTag); };
bool    MVLEBlk::ExchangeDataFields()     { return m_pVLE->DataXchg(*m_pCommon->m_pDCB)!=0; };
bool    MVLEBlk::ValidateDataFields()     { return m_pVLE->ValidateData(*m_pCommon->m_pVDB)!=0; };

double  MVLEBlk::FlashVapFrac(MVector &Mdl)
  { return m_pVLE->FlashVapFrac(*Mdl.SpMdl); };
void    MVLEBlk::SetFlashVapFrac(MVector &Mdl, double Temp, double Press, double VapFrac, DWORD Flags)
  { return m_pVLE->SetFlashVapFrac(*Mdl.SpMdl, Temp, Press, VapFrac, Flags); };
void    MVLEBlk::SetFlashVapFrac(MVector &Mdl, double VapFrac, DWORD Flags)
  { return m_pVLE->SetFlashVapFrac(*Mdl.SpMdl, VapFrac, Flags); };
void    MVLEBlk::SetFlashVapFrac(MStream &Q, double Temp, double Press, double VapFrac, DWORD Flags)
  { return m_pVLE->SetFlashVapFrac(*Q.SpMdl, Temp, Press, VapFrac, Flags); };
void    MVLEBlk::SetFlashVapFrac(MStream &Q, double VapFrac, DWORD Flags)
  { return m_pVLE->SetFlashVapFrac(*Q.SpMdl, VapFrac, Flags); };
void    MVLEBlk::TPFlash(MVector &Mdl, double Temp, double Press, DWORD Flags)
  { return m_pVLE->TPFlash(*Mdl.SpMdl, Temp, Press, Flags); };

//void            AddQVDerivs(SpContainer &C, double Duty, DWORD Flags);


void    MVLEBlk::QPFlash(MStream &Q, double Press, double Duty, DWORD Flags)
  { m_pVLE->QPFlash(Q.Cd, Press, Duty, Flags); };
void    MVLEBlk::QPFlash(MStream &Ql, MStream &Qv, double Press, double Duty, DWORD Flags)
  { m_pVLE->QPFlash(Ql.Cd, Qv.Cd, Press, Duty, Flags); };
void    MVLEBlk::QVFlash(MContainer &Cn, double Duty, DWORD Flags)
  { m_pVLE->QVFlash(Cn.Cn, Duty, Flags); };


double MVLEBlk::SaturationP(MVector & Mdl, double T)
  { return m_pVLE->SaturationP(T, *Mdl.SpMdl); };

double MVLEBlk::SaturationTotalP(MVector & Mdl, double T)
  { return m_pVLE->SaturationTotalP(T, *Mdl.SpMdl); };

void    MVLEBlk::QMVapFlash(MStream &Q, double VapMass, double Duty, DWORD Flags)
  { return m_pVLE->QMVapFlash(Q.Cd, VapMass, Duty, Flags); };
void    MVLEBlk::QMVapFlash(MStream &Ql, MStream &Qv, double VapMass, double Duty, DWORD Flags)
  { return m_pVLE->QMVapFlash(Ql.Cd, Qv.Cd, VapMass, Duty, Flags); };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

void MBaseMethod::RegisterFlashTrainNode(int IOId, bool fModel, const type_info * pNdVarTypeId)
  {
  m_pNd->Class()->SetTypeId(&typeid(*this));
  FlwNode::RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(*this), IOId, fModel, pNdVarTypeId);
  };

//---------------------------------------------------------------------------

MFT_Flash::MFT_Flash(MBaseMethodCommon *pCom, LPCTSTR Tag) : \
MMethodUtility(pCom, Tag)
  {
  m_pFlash = new CFT_Flash(m_pNd);
  };
MFT_Flash::~MFT_Flash()
  {
  delete m_pFlash;
  };

MFT_Flash::operator CMacroMdlBase *()
  {
  return dynamic_cast<CMacroMdlBase *>(m_pFlash);
  };

const type_info * MFT_Flash::TypeInfo()
  {
  return &typeid(*m_pFlash);
  };


void    MFT_Flash::BuildDataFields()                                  { m_pFlash->BuildDataDefn(*m_pCommon->m_pDDB);           };
bool    MFT_Flash::ExchangeDataFields()                               { return 0; };//m_pFlash->DataXchg(*m_pCommon->m_pDCB)!=0;  };
bool    MFT_Flash::ValidateDataFields()                               { return 1; };//m_pFlash->ValidateData(*m_pCommon->m_pVDB)!=0; };
void    MFT_Flash::SetState(eScdMdlStateActs RqdState)                { m_pFlash->SetState(RqdState);               };

bool    MFT_Flash::getActive()                                        { return m_pFlash->Active()!=0;               };

double  MFT_Flash::getFeedP()                                         { return m_pFlash->m_dFeedP;                  };
void    MFT_Flash::putFeedP(double FeedP)                             { m_pFlash->m_dFeedP=FeedP;                   };
double  MFT_Flash::getFeedSaturationP()                               { return m_pFlash->m_dFeedVapP;               };
void    MFT_Flash::putFeedSaturationP(double FeedSaturationP)         { m_pFlash->m_dFeedVapP=FeedSaturationP;      };
double  MFT_Flash::getFlashP()                                        { return m_pFlash->m_dFlashP;                 };
void    MFT_Flash::putFlashP(double FlashP)                           { m_pFlash->m_dFlashP=FlashP;                 };
double  MFT_Flash::getFlashT()                                        { return m_pFlash->m_dFlashT;                 };
void    MFT_Flash::putFlashT(double FlashT)                           { m_pFlash->m_dFlashT=FlashT;                 };
double  MFT_Flash::getEstFlashP()                                     { return m_pFlash->m_dPRqd;                   };
void    MFT_Flash::putEstFlashP(double EstFlashP)                     { m_pFlash->m_dPRqd=EstFlashP;                };
double  MFT_Flash::getVapourFlow()                                    { return m_pFlash->m_dQVap;                   };
void    MFT_Flash::putVapourFlow(double VapourFlow)                   { m_pFlash->m_dQVap=VapourFlow;               };
double  MFT_Flash::getMinFlshTrainP()                                 { return ::MinFlshTrainP;                     };
double  MFT_Flash::getSuctionP()                                      { return m_pFlash->m_dSuctionP;               };

double  MFT_Flash::getPEval()                                         { return m_pFlash->m_dPEval;                  };
void    MFT_Flash::putPEval(double PEval)                             { m_pFlash->m_dPEval=PEval;                   };
double  MFT_Flash::getVapourFlowEval()                                { return m_pFlash->m_dQVapEval;               };
void    MFT_Flash::putVapourFlowEval(double VapourFlowEval)           { m_pFlash->m_dQVapEval=VapourFlowEval;       };
double  MFT_Flash::getVapourFlowSensEval()                            { return m_pFlash->m_dDQvDp;                  };
void    MFT_Flash::putVapourFlowSensEval(double VapourFlowSensEval)   { m_pFlash->m_dDQvDp=VapourFlowSensEval;      };
bool    MFT_Flash::getVapourFlowSensGood()                            { return m_pFlash->m_fDQvDpGood!=0;           };
void    MFT_Flash::putVapourFlowSensGood(bool VapourFlowSensGood)     { m_pFlash->m_fDQvDpGood=VapourFlowSensGood;  };


//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

MFT_Condenser::MFT_Condenser(MBaseMethodCommon *pCom, LPCTSTR Tag) : \
MMethodUtility(pCom, Tag)
  {
  m_pCondenser = new CFT_Condenser(m_pNd);
  };
MFT_Condenser::~MFT_Condenser()
  {
  delete m_pCondenser;
  };

MFT_Condenser::operator        CMacroMdlBase *()
  {
  return dynamic_cast<CMacroMdlBase *>(m_pCondenser);
  };

const type_info * MFT_Condenser::TypeInfo()
  {
  return &typeid(*m_pCondenser);
  };

void    MFT_Condenser::BuildDataFields()                        { m_pCondenser->BuildDataDefn(*m_pCommon->m_pDDB);         };//, (LPSTR)(LPCTSTR)m_sTag); };
bool    MFT_Condenser::ExchangeDataFields()                     { return 0;                                     };//m_pCondenser->DataXchg(*m_pCommon->m_pDCB)!=0; };
bool    MFT_Condenser::ValidateDataFields()                     { return 1;                                     };//m_pCondenser->ValidateData(*m_pCommon->m_pVDB)!=0; };
void    MFT_Condenser::SetState(eScdMdlStateActs RqdState)      { m_pCondenser->SetState(RqdState);             };

bool    MFT_Condenser::getActive()                              { return m_pCondenser->Active()!=0;             };

double  MFT_Condenser::getSuctionP()                            { return m_pCondenser->m_dSuctionP;             };
void    MFT_Condenser::putSuctionP(double SuctionP)             { m_pCondenser->m_dSuctionP=SuctionP;           };
double  MFT_Condenser::getVapourFlow()                          { return m_pCondenser->m_dQCond;                };
void    MFT_Condenser::putVapourFlow(double VapourFlow)         { m_pCondenser->m_dQCond=VapourFlow;            };
double  MFT_Condenser::getVapourFlowReqd()                      { return m_pCondenser->m_dQRqd;                 };
void    MFT_Condenser::putVapourFlowReqd(double VapourFlowReqd) { m_pCondenser->m_dQRqd=VapourFlowReqd;         };
double  MFT_Condenser::getMinSaturationP()                      { return m_pCondenser->m_dMinSatPress;          };
void    MFT_Condenser::putMinSaturationP(double MinSaturationP) { m_pCondenser->m_dMinSatPress=MinSaturationP;  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

//MFT_Mixer::MFT_Mixer(MBaseMethodCommon *pCom, LPCTSTR Tag) : \
//MMethodUtility(pCom, Tag)
//  {
//  m_pMix = new CFT_Mixer;
//  };
//MFT_Mixer::~MFT_Mixer()
//  {
//  delete m_pMix;
//  };
//void    MFT_Mixer::BuildDataFields()                        { m_pMix->BuildDataDefn(*m_pCommon->m_pDDB, (LPSTR)(LPCTSTR)m_sTag); };
//bool    MFT_Mixer::ExchangeDataFields()                     { return m_pMix->DataXchg(*m_pCommon->m_pDCB)!=0; };
//bool    MFT_Mixer::ValidateDataFields()                     { return m_pMix->ValidateData(*m_pCommon->m_pVDB)!=0; };
//void    MFT_Mixer::Activate(CMacroMdl * pMMNd, pFlwNode pNd){ return m_pMix->Activate(pMMNd, pNd); };
//void    MFT_Mixer::Deactivate()                             { return m_pMix->Deactivate();         };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
