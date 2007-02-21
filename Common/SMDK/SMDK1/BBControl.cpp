//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __BBCONTROL_CPP
#include "BBControl.h"

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

static IOAreaRec BBNodeIOAreaList[] = {
  //{"",              "Feed0"    , ioidFeed0, LIO_In0 ,  nc_MLnk, 1, 10, dwIOIsBuffer|IOGRP(1), 0.0F},
  //{"",              "Feed1"    , ioidFeed1, LIO_In1 ,  nc_MLnk, 0, 10, dwIOIsBuffer|IOGRP(1), 0.0F},
  //{"",              "Feed2"    , ioidFeed2, LIO_In2 ,  nc_MLnk, 0, 10, dwIOIsBuffer|IOGRP(1), 0.0F},
  //{"",              "Product0" , ioidProd0, LIO_Out0,  nc_MLnk, 1,  1, dwIOIsBuffer|IOGRP(1), 0.0F},
  //{"",              "Product1" , ioidProd1, LIO_Out1,  nc_MLnk, 0,  1, dwIOIsBuffer|IOGRP(1), 0.0F},
  //{"",              "Product2" , ioidProd2, LIO_Out2,  nc_MLnk, 0,  1, dwIOIsBuffer|IOGRP(1), 0.0F},
  //{"",              "Product3" , ioidProd3, LIO_Out ,  nc_MLnk, 0,  1, dwIOIsBuffer|IOGRP(1), 0.0F},
  //{"",              "Product4" , ioidProd4, LIO_Out ,  nc_MLnk, 0,  1, dwIOIsBuffer|IOGRP(1), 0.0F},
  {NULL} };

//define default graphic symbol
static double Drw_BBNode[] = 
  { 
  DD_Poly, -8,-8, -8,8, 8,8, 8,-8, -8,-8,
  DD_End 
  };

//===========================================================================

IMPLEMENT_MODELUNIT(CBBControl, "CBBControl", "", Drw_BBNode, "Ctrl", "BBC", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_SMDKRUNTIME,
                    "Black Box - Node",
                    "Custom Black Box Models")

CBBControl::CBBControl (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach), CBBBase(this)
  {
  AttachClassInfo(nc_Control, BBNodeIOAreaList);

  //ChangeMethod(1);
  }

//--------------------------------------------------------------------------

CBBControl::~CBBControl()
  {
  }

//--------------------------------------------------------------------------

void CBBControl::SetSubClass(pchar MdlName)
  {
  delete m_pMethod;
  m_pMethod=NULL; // new ....

  m_pMethod=dynamic_cast<MBaseMethod*>(Class()->SubConstruct()->Construct(this));
  m_pMethod->Init(this);
  m_pMethod->Init();

  MdlNode::SetSubClass(MdlName);
  CBBBase::SetSubClass(MdlName);
  };

//--------------------------------------------------------------------------

void CBBControl::BuildDataDefn(DataDefnBlk & DDB)
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

flag CBBControl::DataXchg(DataChangeBlk & DCB)
  {
  if (m_pMethod && DoDataXchg(DCB))
    return 1;

  if (MdlNode::DataXchg(DCB))
    return 1;

  return 0;
  }

//--------------------------------------------------------------------------

flag CBBControl::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK = MdlNode::ValidateData(VDB);
  OK = DoValidateData(VDB) && OK;
  return OK;
  }

//--------------------------------------------------------------------------

flag CBBControl::PreStartCheck()
  {
  if (!MdlNode::PreStartCheck())
    return false;
  return DoPreStartCheck();
  }

// ---------------------------------------------------------------------------

flag CBBControl::InitialiseSolution()
  {
  MdlNode::InitialiseSolution();
  DoInitialiseSolution();
  return 1;
  };

//--------------------------------------------------------------------------

void CBBControl::StartSolution() { DoStartSolution(); }
void CBBControl::StartStep()     { DoStartStep();     }

//--------------------------------------------------------------------------

//bool CBBControl::PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo)
//  {
//  if (!m_pMethod)
//    return false;
//  return Method.PropagateNetInfo(Ctrl, IONo);
//  };
//
////---------------------------------------------------------------------------
//
//void CBBControl::ConfigureJoins()
//  {
//  if (DoConfigureJoins())
//    return;
//
//  Init_NoJoins(1);
//  int i;
//  switch (NetMethod())
//    {
//    case NM_Probal:
//      for (i=0; (i<NoProcessIOs()); i++)
//        SetIO_Join(i, 0);
//      break;
//    case NM_Dynamic:
//      for (i=0; (i<NoProcessIOs()); i++)
//        SetIO_Open(i, 0, false, ESS_Denied);
//      break;
//    }
//  }
//
////--------------------------------------------------------------------------
//
//void CBBControl::EvalPBMakeUpReqd(long JoinMask)
//  {
//  DoEvalPBMakeUpReqd(JoinMask);
//  };
//
////--------------------------------------------------------------------------
//
//void CBBControl::EvalPBMakeUpAvail(long JoinMask)
//  {
//  DoEvalPBMakeUpAvail(JoinMask);
//  };
//
////--------------------------------------------------------------------------
//
//void CBBControl::EvalJoinPressures(long JoinMask)
//  {
//  if (!DoEvalJoinPressures(JoinMask))
//    MdlNode::EvalJoinPressures(JoinMask);
//  };
//
////--------------------------------------------------------------------------
//
//void CBBControl::EvalJoinFlows(int JoinNo)
//  {
//  if (!DoEvalJoinFlows(JoinNo))
//    MdlNode::EvalJoinFlows(JoinNo);
//  };
//
////--------------------------------------------------------------------------
//
//CSpPropInfo* CBBControl::IOGetNetProps(int i, double Qm)
//  {
//  // MUST Call through to Method
//  return NULL; // must improve
//  };
//
////--------------------------------------------------------------------------
//
//flag CBBControl::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
//  {
//  //IOFB(0,FE)->SetRegulator(m_FRB.What());
//  //return IOFB(IONo,FE)->EvaluateFlwEqn(pProps, VPB.ActualPosition(this), &IOFBFlng_Rmt(0)->PhD(), &IOFBFlng_Rmt(1)->PhD());//, IOP_Flng(0), IOP_Flng(1));
//  return DoEvalFlowEquations(Task, /*pProps,*/ IONo, FE, LnkNo);
//  }
//
////--------------------------------------------------------------------------
//
//void CBBControl::EvalProducts(CNodeEvalIndex & NEI)
//  {
//  DoEvalProducts(NEI);
//  }
//
//--------------------------------------------------------------------------

void CBBControl::EvalDiscrete()                          { DoEvalDiscrete(); }
void CBBControl::EvalCtrlInitialise(eScdCtrlTasks Tasks) { DoEvalCtrlInitialise(Tasks); }
void CBBControl::EvalCtrlActions(eScdCtrlTasks Tasks)    { DoEvalCtrlActions(Tasks); }
void CBBControl::EvalCtrlStrategy(eScdCtrlTasks Tasks)   { DoEvalCtrlStrategy(Tasks); }
void CBBControl::EvalCtrlTerminate(eScdCtrlTasks Tasks)  { DoEvalCtrlTerminate(Tasks); }
void CBBControl::EvalStatistics(eScdCtrlTasks Tasks)     { DoEvalStatistics(Tasks); }
void CBBControl::EvalPowerAvailable()                    { DoEvalPowerAvailable(); }
void CBBControl::EvalPowerRequired()                     { DoEvalPowerRequired(); }
void CBBControl::EvalState()                             { DoEvalState(); }
void CBBControl::SetState(eScdMdlStateActs RqdState)     { DoSetState(RqdState); }

//--------------------------------------------------------------------------

void CBBControl::ClosureInfo()
  {
  if (m_Closure.DoFlows() || m_Closure.DoContent())
    {
    CClosureInfo &CI=m_Closure[0];
    if (NoFlwIOs()>0)
      MdlNode::ClosureInfo();
    MVector RefVec(CI.m_pRefMdl);
    MClosureInfo MI(m_Closure, RefVec);
    DoClosureInfo(MI);
    }
  };

//--------------------------------------------------------------------------

void CBBControl::OnAppActivate(BOOL bActive)
  {
  if (bActive)
    {
    DoOnAppActivate(bActive);
    MdlNode::OnAppActivate(bActive);
    }
  };

//--------------------------------------------------------------------------

int CBBControl::FilesUsed(CFilesUsedArray & Files)
  {
  DoFilesUsed(Files);
  MdlNode::FilesUsed(Files);
  return Files.GetCount();
  };

//--------------------------------------------------------------------------

dword CBBControl::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (NoFlwIOs()>1)
    {
    const int iIn  = (IOQmEst_In(0)>0.0 ? 0 : 1);
    Status |= (IOConduit(iIn)->QMass()>UsableMass ? FNS_UFlw : FNS_UNoFlw);
    }
  else if (NoFlwIOs()==1)
    Status |= FNS_Error;
  return Status;
  }

//--------------------------------------------------------------------------

flag CBBControl::GetModelAction(CMdlActionArray & Acts)
  {
  return DoGetModelAction(Acts);
  };
flag CBBControl::SetModelAction(CMdlAction & Act)
  {
  return DoSetModelAction(Act);
  };

//--------------------------------------------------------------------------

flag CBBControl::GetModelGraphic(CMdlGraphicArray & Grfs)
  {
  return DoGetModelGraphic(Grfs);
  };
flag CBBControl::OperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf)
  {
  return DoOperateModelGraphic(Wnd, Grf);
  };

//--------------------------------------------------------------------------

//void    CBBControl::MacroMdlEvaluate(eScdMacroMdlEvals Eval) { DoMacroMdlEvaluate(Eval);      };
//flag    CBBControl::MacroMdlValidNd(int iIONo)               { return DoMacroMdlValidNd(iIONo); };
//void    CBBControl::MacroMdlAddNd(int iIONo)                 { DoMacroMdlAddNd(iIONo);          };
//void    CBBControl::MacroMdlRemoveNd(int iIONo)              { DoMacroMdlRemoveNd(iIONo);       };
//CMacroMdlBase* CBBControl::MacroMdlActivate()                { return DoMacroMdlActivate();     };
//void    CBBControl::MacroMdlDeactivate()                     { DoMacroMdlDeactivate();          };

//--------------------------------------------------------------------------

flag CBBControl::CIStrng(int No, pchar & pS)
  {
  switch (No-CBContext())
    {
    case  1: pS="E\tIncorrect feed stream connection!";  return 1;
    case  2: pS="E\tIncorrect product stream connection!";  return 1;
    case  3: pS="E\tRequired product stream not connected!";  return 1;
    case  4: pS="W\tNo Size distributions defined";  return 1;
    case  5: pS="E\tTotal Size mass does NOT equal corresponding specie mass";  return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  }

//===========================================================================

