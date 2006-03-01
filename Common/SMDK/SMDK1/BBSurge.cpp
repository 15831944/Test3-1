//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __BBSURGE_CPP
#include "BBSurge.h"

//====================================================================================

//XID xidModelMethod   = MdlBsXID(10010);

//====================================================================================

//define Input/Output connections
const byte ioidFeed0  = 0;
const byte ioidFeed1  = 1;
const byte ioidFeed2  = 2;
const byte ioidProd0  = 3;
const byte ioidProd1  = 4;
const byte ioidProd2  = 5;
const byte ioidProd3  = 6;
const byte ioidProd4  = 7;

static IOAreaRec BBSurgeIOAreaList[] = {
  {"",              "Feed0"    , ioidFeed0, LIO_In0 ,  nc_MLnk, 1, 10, 0, 0.0F},
  {"",              "Feed1"    , ioidFeed1, LIO_In1 ,  nc_MLnk, 0, 10, 0, 0.0F},
  {"",              "Feed2"    , ioidFeed2, LIO_In2 ,  nc_MLnk, 0, 10, 0, 0.0F},
  {"",              "Product0" , ioidProd0, LIO_Out0,  nc_MLnk, 1,  1, 0, 0.0F},
  {"",              "Product1" , ioidProd1, LIO_Out1,  nc_MLnk, 0,  1, 0, 0.0F},
  {"",              "Product2" , ioidProd2, LIO_Out2,  nc_MLnk, 0,  1, 0, 0.0F},
  {"",              "Product3" , ioidProd3, LIO_Out ,  nc_MLnk, 0,  1, 0, 0.0F},
  {"",              "Product4" , ioidProd4, LIO_Out ,  nc_MLnk, 0,  1, 0, 0.0F},
  {NULL} };

//define default graphic symbol
static double Drw_BBSurge[] = { DD_Poly, -10,-9, -10,9, 10,9, 10,-9, -10,-9,
                         DD_End };

//===========================================================================

//#include "optoff.h"

IMPLEMENT_MODELUNIT(CBBSurge, "CBBSurge", "", Drw_BBSurge, "Tank", "BBS", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_BLACKBOX,
                    "Black Box - Surge",
                    "Custom Black Box Models containing Surge capacity")

CBBSurge::CBBSurge (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach), CBBBase(this)
  {
  AttachClassInfo(nc_Process, BBSurgeIOAreaList);
  Contents.SetClosed(False);

  //ChangeMethod(1);
  }

//--------------------------------------------------------------------------

CBBSurge::~CBBSurge()
  {
  }

//--------------------------------------------------------------------------

void CBBSurge::SetSubClass(pchar MdlName)
  {
  delete m_pMethod;
  m_pMethod=NULL; // new ....

  m_pMethod=dynamic_cast<MBaseMethod*>(Class()->SubConstruct()->Construct(this));
  m_pMethod->Init(this);
  m_pMethod->Init();

  MN_Surge::SetSubClass(MdlName);

  CBBBase::SetSubClass(MdlName);
  };

//--------------------------------------------------------------------------

void CBBSurge::BuildDataDefn(DataDefnBlk & DDB)
  {

  DDB.BeginStruct(this);
  DDB.Visibility(SM_DynBoth|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Text    ("");
  DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, isResult|noFile|noSnap);
  DDB.Double  ("Temperature",  "T",     DC_T,    "C",      xidTemp,        this, isResult|noFile|noSnap);
  DDB.Double  ("Density",      "Rho",   DC_Rho,  "kg/m^3", xidRho,         this, isResult|noFile|noSnap);
  DDB.Double  ("Level",        "Lvl",   DC_Frac, "%",      xidLevel,       this, isResult|noFile|noSnap);
  DDB.Text("");

  DDB.Visibility();

  if (m_pMethod)
    {
    if (MethodImpl.m_dwMethOptions & MO_ShowIOs)
      BuildDataDefnShowIOs(DDB);
    if (MethodImpl.m_dwMethOptions & MO_ShowIOOpts)
      BuildDataDefnIOOpts(DDB);
    if (DynamicMode() && (MethodImpl.m_dwMethOptions & MO_ShowContents))
      DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DoBuildDataDefn(DDB);
    }

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CBBSurge::DataXchg(DataChangeBlk & DCB)
  {
  if (m_pMethod && DoDataXchg(DCB))
    return 1;

  if (MN_Surge::DataXchg(DCB))
    return 1;

  return 0;
  }

//--------------------------------------------------------------------------

flag CBBSurge::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK = MN_Surge::ValidateData(VDB);
  OK = DoValidateData(VDB) && OK;
  return OK;
  }

//--------------------------------------------------------------------------

void CBBSurge::PostConnect(int IONo)
  {
  MN_Surge::PostConnect(IONo);
  IOFB(IONo,0)->AssignFlwEqnGroup(PipeEntryGroup, PipeEntryGroup.Default(), this);
  DoPostConnect(IONo);
  };

//---------------------------------------------------------------------------

void CBBSurge::PreDisConnect(int IONo)
  {
  DoPreDisConnect(IONo);
  MN_Surge::PreDisConnect(IONo);
  }

//--------------------------------------------------------------------------

Z_States CBBSurge::SetDatums(int Pass, int IOIn, double Zi, Z_States Z_State_Src)
  {
  CSetDatumsData SDD[10+1]=
    {
//      {First64IOIds, &Contents},
      {0},
      {0},
      {0},
      {0},
      {0},
      {0},
      {0},
      {0},
      {0},
      {0},
      {0}
    };
  DoSetDatums(SDD, 10);
  return SetDatums_Node(Pass, SDD, IOIn, Zi, Z_State_Src);
  };

//--------------------------------------------------------------------------

void CBBSurge::SetDatumsDone()
  {
  MN_Surge::SetDatumsDone();
//  SortSurgeIOData SDD[]=
//    {
//      {First64IOIds, &Contents, &ContentHgtOrd},
//      {0}
//    };
//  SortSurgeIO(SDD);
  };

//--------------------------------------------------------------------------

flag CBBSurge::Set_Sizes()
  {
  for (long i=0; i<NoFlwIOs(); i++)
    if (IOPipeEntry_Self(i))
      {
      double A=IOFB_Rmt(i,0)->Area();
      IOFB(i,0)->SetArea(A);
      IOFB(i,0)->SetActLength(0.0);
      IOFB(i,0)->SetFitLength(0.0);
      }


  return True;
  };

//--------------------------------------------------------------------------

flag CBBSurge::PreStartCheck()
  {
  if (!MN_Surge::PreStartCheck())
    return false;
  return DoPreStartCheck();
  }

//--------------------------------------------------------------------------

void CBBSurge::StartSolution() { DoStartSolution(); }
void CBBSurge::StartStep()     { DoStartStep();     }

// ---------------------------------------------------------------------------

flag CBBSurge::InitialiseSolution()
  {
  MN_Surge::InitialiseSolution();
  DoInitialiseSolution();
  return 1;
  };

//--------------------------------------------------------------------------

bool CBBSurge::PropagateNetInfo(eScdPropagateNetTasks Task, long IONo, long Info, bool Start)
  {
_asm int 3;
//if (!m_pMethod)
  //  return false;
  //return Method.PropagateNetInfo(Task, Info, Start);
  };

//---------------------------------------------------------------------------

void CBBSurge::OnSetUserMethod()
  {
  DoOnSetUserMethod();
  }

//---------------------------------------------------------------------------

void CBBSurge::ConfigureJoins()
  {
  if (!DoConfigureJoins())
    MN_Surge::ConfigureJoins();
  }

//--------------------------------------------------------------------------

void CBBSurge::EvalPBMakeUpReqd(long JoinMask)
  {
  DoEvalPBMakeUpReqd(JoinMask);
  };

//--------------------------------------------------------------------------

void CBBSurge::EvalPBMakeUpAvail(long JoinMask)
  {
  DoEvalPBMakeUpAvail(JoinMask);
  };

//--------------------------------------------------------------------------

void CBBSurge::EvalJoinPressures(long JoinMask)
  {
  if (!DoEvalJoinPressures(JoinMask))
    MN_Surge::EvalJoinPressures(JoinMask);
  };

//--------------------------------------------------------------------------

void CBBSurge::EvalJoinFlows(int JoinNo)
  {
  if (!DoEvalJoinFlows(JoinNo))
    MN_Surge::EvalJoinFlows(JoinNo);
  };

//--------------------------------------------------------------------------

CSpPropInfo* CBBSurge::IOGetNetProps(int i, double Qm)
  {
  // MUST Call through to Method
  return NULL; // must improve
  };

//--------------------------------------------------------------------------

flag CBBSurge::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  //IOFB(0,FE)->SetRegulator(m_FRB.What());
  //return IOFB(IONo,FE)->EvaluateFlwEqn(pProps, VPB.ActualPosition(this), &IOFBFlng_Rmt(0)->PhD(), &IOFBFlng_Rmt(1)->PhD());//, IOP_Flng(0), IOP_Flng(1));
  return DoEvalFlowEquations(Task, /*pProps,*/ IONo, FE, LnkNo);
  }

//--------------------------------------------------------------------------

void CBBSurge::EvalProducts(long JoinMask)
  {
  //MN_Surge::EvalProducts(JoinMask);
  DoEvalProducts(JoinMask);
  }

//--------------------------------------------------------------------------

void CBBSurge::EvalIntegral(long JoinMask)
  {
  DoEvalIntegral(JoinMask);
  SetIntegralDone(true);
  }

//--------------------------------------------------------------------------

void CBBSurge::EvalDiscrete()                       { DoEvalDiscrete(); }
void CBBSurge::EvalCtrlInitialise()                 { DoEvalCtrlInitialise(); }
void CBBSurge::EvalCtrlActions()                    { DoEvalCtrlActions(); }
void CBBSurge::EvalCtrlStrategy()                   { DoEvalCtrlStrategy(); }
void CBBSurge::EvalCtrlTerminate()                  { DoEvalCtrlTerminate(); }
void CBBSurge::EvalStatistics()                     { DoEvalStatistics(); }
void CBBSurge::EvalPowerAvailable()                 { DoEvalPowerAvailable(); }
void CBBSurge::EvalPowerRequired()                  { DoEvalPowerRequired(); }
void CBBSurge::EvalState()                          { DoEvalState(); }
void CBBSurge::SetState(eScdMdlStateActs RqdState)  { DoSetState(RqdState); }

//--------------------------------------------------------------------------

void CBBSurge::ClosureInfo()
  {

  if (m_Closure.DoFlows() || m_Closure.DoContent())
    {
    CClosureInfo &CI=m_Closure[0];
    if (NoFlwIOs()>0)
      MN_Surge::ClosureInfo();
    MVector RefVec(CI.m_pRefMdl);
    MClosureInfo MI(m_Closure, RefVec);
    DoClosureInfo(MI);
    }
  };

//--------------------------------------------------------------------------

void CBBSurge::OnAppActivate(BOOL bActive)
  {
  DoOnAppActivate(bActive);
  };

//--------------------------------------------------------------------------

int CBBSurge::FilesUsed(CFilesUsedArray & Files)
  {
  return DoFilesUsed(Files);
  };

//--------------------------------------------------------------------------

dword CBBSurge::ModelStatus()
  {
  dword Status=MN_Surge::ModelStatus();
  //if (NoFlwIOs()>1)
  //  {
  //  const long iIn  = (IOQmEst_In(0)>0.0 ? 0 : 1);
  //  Status |= (IOConduit(iIn)->QMass()>UsableMass ? FNS_UFlw : FNS_UNoFlw);
  //  }
  //else if (NoFlwIOs()==1)
  //  Status |= FNS_Error;
  return Status;
  }

//--------------------------------------------------------------------------

void    CBBSurge::MacroMdlEvaluate(eScdMacroMdlEvals Eval)  { DoMacroMdlEvaluate(Eval);      };
flag    CBBSurge::MacroMdlValidNd(int iIONo)                { return DoMacroMdlValidNd(iIONo); };
void    CBBSurge::MacroMdlAddNd(int iIONo)                  { DoMacroMdlAddNd(iIONo);          };
void    CBBSurge::MacroMdlRemoveNd(int iIONo)               { DoMacroMdlRemoveNd(iIONo);       };
CMacroMdlBase* CBBSurge::MacroMdlActivate()                 { return DoMacroMdlActivate();     };
void    CBBSurge::MacroMdlDeactivate()                      { DoMacroMdlDeactivate();          };

//--------------------------------------------------------------------------

flag CBBSurge::CIStrng(int No, pchar & pS)
  {
  switch (No-CBContext())
    {
    case  1: pS="E\tIncorrect feed stream connection!";  return 1;
    case  2: pS="E\tIncorrect product stream connection!";  return 1;
    case  3: pS="E\tRequired product stream not connected!";  return 1;
    case  4: pS="W\tNo Size distributions defined";  return 1;
    case  5: pS="E\tTotal Size mass does NOT equal corresponding specie mass";  return 1;
    default:
      return MN_Surge::CIStrng(No, pS);
    }
  }

//===========================================================================

