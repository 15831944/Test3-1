//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __BBTRANSFER_CPP
#include "BBtransfer.h"

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

static IOAreaRec BBNodeIOAreaList[] = 
  {
    {"",              "Feed0"    , ioidFeed0, LIO_In0 ,  nc_MLnk, 1, 10, IOIsBuffer|IOGRP(1), 0.0},
    {"",              "Feed1"    , ioidFeed1, LIO_In1 ,  nc_MLnk, 0, 10, IOIsBuffer|IOGRP(1), 0.0},
    {"",              "Feed2"    , ioidFeed2, LIO_In2 ,  nc_MLnk, 0, 10, IOIsBuffer|IOGRP(1), 0.0},
    {"",              "Product0" , ioidProd0, LIO_Out0,  nc_MLnk, 1,  1, IOIsBuffer|IOGRP(1), 0.0},
    {"",              "Product1" , ioidProd1, LIO_Out1,  nc_MLnk, 0,  1, IOIsBuffer|IOGRP(1), 0.0F},
    {"",              "Product2" , ioidProd2, LIO_Out2,  nc_MLnk, 0,  1, IOIsBuffer|IOGRP(1), 0.0F},
    {"",              "Product3" , ioidProd3, LIO_Out ,  nc_MLnk, 0,  1, IOIsBuffer|IOGRP(1), 0.0F},
    {"",              "Product4" , ioidProd4, LIO_Out ,  nc_MLnk, 0,  1, IOIsBuffer|IOGRP(1), 0.0F},
    {NULL} 
  };

//define default graphic symbol
static double Drw_BBNode[] = { DD_Poly, -8,-7, -8,7, 8,7, 8,-7, -8,-7,
                         DD_End };

//===========================================================================

IMPLEMENT_MODELUNIT(CBBTransfer, "CBBTransfer", "", Drw_BBNode, "Tank", "BBN", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_SMDKRUNTIME,
                    "Black Box - Node",
                    "Custom Black Box Models")

CBBTransfer::CBBTransfer (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Xfer(pClass_, TagIn, pAttach, eAttach), CBBBase(this)
  {
  AttachClassInfo(nc_Process, BBNodeIOAreaList);

  //ChangeMethod(1);
  }

//--------------------------------------------------------------------------

CBBTransfer::~CBBTransfer()
  {
  }

//--------------------------------------------------------------------------

void CBBTransfer::SetSubClass(pchar MdlName)
  {
  delete m_pMethod;
  m_pMethod=NULL; // new ....

  m_pMethod=dynamic_cast<MBaseMethod*>(Class()->SubConstruct()->Construct(this));
  m_pMethod->Init(this);
  m_pMethod->Init();

  MN_Xfer::SetSubClass(MdlName);
  CBBBase::SetSubClass(MdlName);
  };

//--------------------------------------------------------------------------

void CBBTransfer::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);

  DDB.Visibility();
  DDB.Text("");
  BuildDataDefnShowIOs(DDB);

  if (m_pMethod)
    DoBuildDataDefn(DDB);
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CBBTransfer::DataXchg(DataChangeBlk & DCB)
  {
  if (m_pMethod && DoDataXchg(DCB))
    return 1;

  if (MN_Xfer::DataXchg(DCB))
    return 1;

  return 0;
  }

//--------------------------------------------------------------------------

flag CBBTransfer::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK = MN_Xfer::ValidateData(VDB);
  OK = DoValidateData(VDB) && OK;
  return OK;
  }

//--------------------------------------------------------------------------

flag CBBTransfer::PreStartCheck()
  {
  if (!MN_Xfer::PreStartCheck())
    return false;
  return DoPreStartCheck();
  }

// ---------------------------------------------------------------------------

flag CBBTransfer::InitialiseSolution()
  {
  MN_Xfer::InitialiseSolution();
  DoInitialiseSolution();
  return 1;
  };

//--------------------------------------------------------------------------

void CBBTransfer::StartSolution() { DoStartSolution(); }
void CBBTransfer::StartStep()     { DoStartStep();     }

//--------------------------------------------------------------------------

bool CBBTransfer::PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo)
  {
  if (!m_pMethod)
    return false;
  return Method.PropagateNetInfo(Ctrl, IONo);
  };

//---------------------------------------------------------------------------

void CBBTransfer::ConfigureJoins()
  {
  if (DoConfigureJoins())
    return;

  Init_NoJoins(1);
  int i;
  switch (NetMethod())
    {
    case NM_Probal:
      for (i=0; (i<NoProcessIOs()); i++)
        SetIO_Join(i, 0);
      break;
    case NM_Dynamic:
      for (i=0; (i<NoProcessIOs()); i++)
        SetIO_Open(i, 0, false, ESS_Denied);
      break;
    }
  }

//--------------------------------------------------------------------------

void CBBTransfer::EvalPBMakeUpReqd(long JoinMask)
  {
  DoEvalPBMakeUpReqd(JoinMask);
  };

//--------------------------------------------------------------------------

void CBBTransfer::EvalPBMakeUpAvail(long JoinMask)
  {
  DoEvalPBMakeUpAvail(JoinMask);
  };

//--------------------------------------------------------------------------

void CBBTransfer::EvalJoinPressures(long JoinMask)
  {
  if (!DoEvalJoinPressures(JoinMask))
    MN_Xfer::EvalJoinPressures(JoinMask);
  };

//--------------------------------------------------------------------------

void CBBTransfer::EvalJoinFlows(int JoinNo)
  {
  if (!DoEvalJoinFlows(JoinNo))
    MN_Xfer::EvalJoinFlows(JoinNo);
  };

//--------------------------------------------------------------------------

CSpPropInfo* CBBTransfer::IOGetNetProps(int i, double Qm)
  {
  // MUST Call through to Method
  return NULL; // must improve
  };

//--------------------------------------------------------------------------

flag CBBTransfer::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  //IOFB(0,FE)->SetRegulator(m_FRB.What());
  //return IOFB(IONo,FE)->EvaluateFlwEqn(pProps, VPB.ActualPosition(this), &IOFBFlng_Rmt(0)->PhD(), &IOFBFlng_Rmt(1)->PhD());//, IOP_Flng(0), IOP_Flng(1));
  return DoEvalFlowEquations(Task, /*pProps,*/ IONo, FE, LnkNo);
  }

//--------------------------------------------------------------------------

void CBBTransfer::EvalProducts(CNodeEvalIndex & NEI)
  {
  DoEvalProducts(NEI);
  }

//--------------------------------------------------------------------------

void CBBTransfer::EvalDiscrete()                          { DoEvalDiscrete(); }
void CBBTransfer::EvalCtrlInitialise(eScdCtrlTasks Tasks) { DoEvalCtrlInitialise(Tasks); }
void CBBTransfer::EvalCtrlActions(eScdCtrlTasks Tasks)    { DoEvalCtrlActions(Tasks); }
void CBBTransfer::EvalCtrlStrategy(eScdCtrlTasks Tasks)   { DoEvalCtrlStrategy(Tasks); }
void CBBTransfer::EvalCtrlTerminate(eScdCtrlTasks Tasks)  { DoEvalCtrlTerminate(Tasks); }
void CBBTransfer::EvalStatistics(eScdCtrlTasks Tasks)     { DoEvalStatistics(Tasks); }
void CBBTransfer::EvalPowerAvailable()                    { DoEvalPowerAvailable(); }
void CBBTransfer::EvalPowerRequired()                     { DoEvalPowerRequired(); }
void CBBTransfer::EvalState()                             { DoEvalState(); }
void CBBTransfer::SetState(eScdMdlStateActs RqdState)     { DoSetState(RqdState); }

//--------------------------------------------------------------------------

void CBBTransfer::ClosureInfo()
  {
  if (m_Closure.DoFlows() || m_Closure.DoContent())
    {
    CClosureInfo &CI=m_Closure[0];
    if (NoFlwIOs()>0)
      MN_Xfer::ClosureInfo();
    MVector RefVec(CI.m_pRefMdl);
    MClosureInfo MI(m_Closure, RefVec);
    DoClosureInfo(MI);
    }
  };

//--------------------------------------------------------------------------

void CBBTransfer::OnAppActivate(BOOL bActive)
  {
  if (bActive)
    {
    DoOnAppActivate(bActive);
    MdlNode::OnAppActivate(bActive);
    }
  };

//--------------------------------------------------------------------------

int CBBTransfer::FilesUsed(CFilesUsedArray & Files)
  {
  DoFilesUsed(Files);
  MdlNode::FilesUsed(Files);
  return Files.GetCount();
  };

//--------------------------------------------------------------------------

dword CBBTransfer::ModelStatus()
  {
  dword Status=MN_Xfer::ModelStatus();
  if (NoFlwIOs()>1)
    {
    const int iIn  = (IOQmEst_In(0)>0.0 ? 0 : 1);
    Status |= (IOConduit(iIn)->QMass()>gs_DisplayZeroFlow ? FNS_UFlw : FNS_UNoFlw);
    }
  else if (NoFlwIOs()==1)
    Status |= FNS_Error;
  return Status;
  }

//--------------------------------------------------------------------------

flag CBBTransfer::GetModelAction(CMdlActionArray & Acts)
  {
  return DoGetModelAction(Acts);
  };
flag CBBTransfer::SetModelAction(CMdlAction & Act)
  {
  return DoSetModelAction(Act);
  };

//--------------------------------------------------------------------------

flag CBBTransfer::GetModelGraphic(CMdlGraphicArray & Grfs)
  {
  return DoGetModelGraphic(Grfs);
  };
flag CBBTransfer::OperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf)
  {
  return DoOperateModelGraphic(Wnd, Grf);
  };

//--------------------------------------------------------------------------

void    CBBTransfer::MacroMdlEvaluate(eScdMacroMdlEvals Eval) { DoMacroMdlEvaluate(Eval);      };
flag    CBBTransfer::MacroMdlValidNd(int iIONo)               { return DoMacroMdlValidNd(iIONo); };
void    CBBTransfer::MacroMdlAddNd(int iIONo)                 { DoMacroMdlAddNd(iIONo);          };
void    CBBTransfer::MacroMdlRemoveNd(int iIONo)              { DoMacroMdlRemoveNd(iIONo);       };
CMacroMdlBase* CBBTransfer::MacroMdlActivate()                { return DoMacroMdlActivate();     };
void    CBBTransfer::MacroMdlDeactivate()                     { DoMacroMdlDeactivate();          };

//--------------------------------------------------------------------------

flag CBBTransfer::CIStrng(int No, pchar & pS)
  {
  switch (No-CBContext())
    {
    case  1: pS="E\tIncorrect feed stream connection!";  return 1;
    case  2: pS="E\tIncorrect product stream connection!";  return 1;
    case  3: pS="E\tRequired product stream not connected!";  return 1;
    case  4: pS="W\tNo Size distributions defined";  return 1;
    case  5: pS="E\tTotal Size mass does NOT equal corresponding specie mass";  return 1;
    default:
      return MN_Xfer::CIStrng(No, pS);
    }
  }

//===========================================================================

