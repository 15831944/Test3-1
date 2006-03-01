//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __M_XFER_CPP
#include "m_xfer.h"

//===========================================================================

MN_Xfer::MN_Xfer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  m_QFeed("QFeed", this),
  m_QProd("QProd", this),
  m_RB(this, false),
  m_PBPCtrl0(PBPC_MinFd, Std_P, true),
  m_FTM(this),
  m_Leak(eDIO_Leak, this, true, false, "Leak", IOId_XferLeak, IOId_AreaLeakIn)
  {
  AttachClassInfo(nc_MXfer,NULL);
  Joins.SetSize(1);
  };

// -------------------------------------------------------------------------

flag MN_Xfer::DataXchg(DataChangeBlk & DCB)
  {
  if (DCB.dwUserInfo==100 && m_Leak.DataXchg(DCB))
    return 1;
  if (m_RB.DataXchg(DCB))
    return 1;
  return MdlNode::DataXchg(DCB);
  }

//--------------------------------------------------------------------------

flag MN_Xfer::ValidateData(ValidateDataBlk & VDB)
  {
  bool OK=true;
  if (!m_Leak.ValidateData(VDB))
    OK=false;
  
  if (!MdlNode::ValidateData(VDB))
    OK=false;

  return OK;
  };

//--------------------------------------------------------------------------

//flag MN_Xfer::GlobalConnect()
//  {
//  flag OK=FlwNode::GlobalConnect();
//  if (!m_Leak.GlobalConnect())
//    OK=false;
//  m_Leak.GlobalConnectFinal();
//  return OK;
//  };
//
//// -------------------------------------------------------------------------
//
//flag MN_Xfer::GlobalDisConnect()
//  {
//  flag OK=FlwNode::GlobalDisConnect();
//  m_Leak.GlobalDisConnect();
//  m_Leak.GlobalConnectFinal();
//  return 1;
//  };

//--------------------------------------------------------------------------

CFlange * MN_Xfer::GetFlange(int IOId)
  {
  return NULL;
  };

//--------------------------------------------------------------------------

void MN_Xfer::PostConnect(int IONo)
  {
  MdlNode::PostConnect(IONo);
  if (IOId_Self(IONo)==IOId_XferLeak)
    m_Leak.PostConnect(IONo);
  else
    IOFB(IONo,0)->AssignFlwEqnGroup(TeeEntryGroup, TeeEntryGroup.Default(), this);
  };

//---------------------------------------------------------------------------

void MN_Xfer::PreDisConnect(int IONo)
  {
  if (IOId_Self(IONo)==IOId_XferLeak)
    m_Leak.PreDisConnect(IONo);
  MdlNode::PreDisConnect(IONo);
  }

//--------------------------------------------------------------------------

Z_States MN_Xfer::SetDatums(int Pass, int IOIn, double Zi, Z_States Z_State_Src)
  {
  return SetDatums_Node(Pass, NULL, IOIn, Zi, Z_State_Src);
  };

//--------------------------------------------------------------------------

flag MN_Xfer::Set_Sizes()
  {
  for (int i=0; i<NoFlwIOs(); i++)
    {
    double A=IOFB_Rmt(i,0)->Area();
    IOFB(i,0)->SetArea(A);
    IOFB(i,0)->SetActLength(0.0);
    IOFB(i,0)->SetFitLength(0.0);
    }
  return True;
  };

//--------------------------------------------------------------------------

void   MN_Xfer::SetState(eScdMdlStateActs RqdState)
  {
  MdlNode::SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
    case MSA_ZeroFlows:
    case MSA_Empty:
    case MSA_PreSet:
      {
      ResetData(false);

      double P0 = NoFlwIOs() ? AtmosPress(IODatum_Term(0)) : Std_P;
      Set_JoinP(0, P0);
      Set_JoinP_Est(0, P0);
      Set_JoinP_Max(0, dNAN);
      Set_JoinP_MaxSet(0, dNAN);
      for (int i=0; i<NoFlwIOs(); i++)
        {
        Set_IOP_Flng(i, P0);
        Set_IOP_Self(i, P0);
        Set_IOP_Est_Self(i, P0);
        Set_IOP_Est_Flng(i, P0);
        IOConduit(i)->SetState(RqdState);
        if (IOConduitIO(i))
          IOConduitIO(i)->SetState(RqdState);
        if (IOConduitIn(i))
          IOConduitIn(i)->SetState(RqdState);
        SetIOQm_In(i, 0.0);
        SetIOQmEst_In(i, 0.0);
        SetIOQmSpace_Self(i, 0.0);
        SetIOQmAvail_Self(i, 0.0);
        }
      }
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  };

//---------------------------------------------------------------------------

bool MN_Xfer::PropagateNetInfo(eScdPropagateNetTasks Task, long IONo, long Info, bool Start)
  {
  if (!FlwNode::PropagateNetInfo(Task, IONo, Info, Start))
    return false;

  dbgpln("PropagateNetInfo %s", FullObjTag());

  switch (Info)
    {
    case eScdSetFlowMode :
      {
      //m_FEP.FlwRqd().SetPressOn(Info.m_PressOn);
      break;
      }
    }
  for (int i=0; i<NoFlwIOs(); i++)
    Nd_Rmt(i)->PropagateNetInfo(Task, IOIONo_Rmt(i), Info, false);

  return true;
  };

//---------------------------------------------------------------------------

void MN_Xfer::ConfigureJoins()
  {
  for (int i=0; i<NoFlwIOs(); i++)
    SetIO_Join(i, 0);
  };

//--------------------------------------------------------------------------

void MN_Xfer::StartSolution()
  {
  if (m_Leak.ConnectError)
    {
    LogError(FullObjTag(), 0, "Stopping Leak(s) not connected");
    ExecObj()->XStop();
    }
  }

//--------------------------------------------------------------------------

//void MN_Xfer::SetJoinPressure(int iSolnTyp, int iJoinId, double P)
//  {
//  if (iJoinId==0)
//    MdlNode::SetJoinPressure(iSolnTyp, PB, P);
//  FlwNode::SetJoinPressure(iSolnTyp, iJoinId, P);
//  };
//
////--------------------------------------------------------------------------
//
//double MN_Xfer::GetJoinPressure(int iSolnTyp, int iJoinId)
//  {
//  if (iJoinId==0)
//    return MdlNode::GetJoinPressure(iSolnTyp, PB);
//  return FlwNode::GetJoinPressure(iSolnTyp, iJoinId);
//  };

//--------------------------------------------------------------------------
/*#F:This determines the pressure of the contents and the pressure at each
inlet and outlet of the surge unit.*/
void MN_Xfer::EvalJoinPressures(long JoinMask)
  {
  switch (SolveMode())
    {
    case SM_Probal:
    case SM_DynXfer:
      {
      const bool OldCode = false;
      if (OldCode)
        {
        if (NJoins()>=1)
          EvalPBJoinPressure(0, &m_PBPCtrl0, m_QFeed());//&QProd());
        //if (NJoins()>=2)
        //  EvalPBJoinPressure(1);
        }
      else
        {
        if (NJoins()>=1)
          EvalPBJoinPressure(0, &m_PBPCtrl0, m_QProd());
        if (NJoins()>=2)
          EvalPBJoinPressure(1);
        }
      break;
      }
    //case SM_DynXfer:
    case SM_DynFull:
      MdlNode::EvalJoinPressures(JoinMask);
      break;
    }
  };
// -------------------------------------------------------------------------

flag MN_Xfer::ChangeMode(dword NewMode, dword OldMode)
  {
  if (NewMode==SM_Probal && !GSM.Enabled())
    GSM.Open();

  return True;
  };

// --------------------------------------------------------------------------

flag MN_Xfer::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  if (m_Leak.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;

  return IOFB(IONo, FE)->EvaluateFlwEqn(Task, pProps, 1.0, &IOFBFlng_Rmt(IONo)->PhD(), NULL);
  };

// --------------------------------------------------------------------------

void MN_Xfer::EvalProductsInit(EvalProductsInitTasks Task)
  {
  EvalProductsInit_Node(Task, 0x0ffffff);
  }

// --------------------------------------------------------------------------

void MN_Xfer::EvalProducts(long JoinMask)
  {
  if (NJoins()>0)
    {
    switch (SolveMode())
      {
      case SM_Probal:
        if (NJoins()>0)
          Xfer_EvalProducts(0, Joins[0].Pressure(), &m_QFeed, &m_QProd, m_RB(), GSM(), EHX(), &m_PBPCtrl0);
        break;
      case SM_DynXfer:
      case SM_DynFull:
        for (int j=0; j<NJoins(); j++)
          Xfer_EvalProducts(j, Joins[j].Pressure(), &m_QFeed, &m_QProd, m_RB(), GSM(), EHX());
        break;
      }                
    }
  };

//--------------------------------------------------------------------------

void MN_Xfer::EvalDiscrete() 
  {
  MdlNode::EvalDiscrete();
  m_Leak.EvalDiscrete();
  };

//--------------------------------------------------------------------------

void MN_Xfer::EvalPBMakeUpReqd(long JoinMask)
  {
  if (GSM.Enabled())
    GSM.MakeUpNodeTransferReqd(0);
  MakeUpNodeTransferReqd(0);
  };

//--------------------------------------------------------------------------

void MN_Xfer::EvalPBMakeUpAvail(long JoinMask)
  {
  if (GSM.Enabled())
    GSM.MakeUpNodeTransferAvail(0);
  MakeUpNodeTransferAvail(0);
  };

//--------------------------------------------------------------------------

flag MN_Xfer::MacroMdlValidNd(int iIONo)
  {
  if (!GSM.Enabled())
    return False;

// if (IOQmEst_Out(iIONo))
  if (IODesc_Rmt(iIONo)->Dirn==LIO_In)
    {
    byte QmMd=GSM.QmMode(iIONo);
    if (QmMd!=GSA_QmDemand)
      return False;
    }
  return True;
  };

//--------------------------------------------------------------------------

void MN_Xfer::MacroMdlEvaluate(eScdMacroMdlEvals Eval)
  {
  };

//--------------------------------------------------------------------------

CMacroMdlBase* MN_Xfer::MacroMdlActivate()
  {
  return &m_FTM;
  };

//--------------------------------------------------------------------------

void MN_Xfer::MacroMdlDeactivate()
  {
  };

//--------------------------------------------------------------------------

void MN_Xfer::ClosureInfo()
  {
  bool InRange=true;
  for (int i=0; i<NoFlwIOs(); i++)
    InRange = InRange && IOConduit(i)->SMFnsInRange();
  FlwNode::SetCI(29, !InRange);

  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI= m_Closure[0];
    if (m_RB.Enabled())
      m_RB.GetClosureInfo(CI);
    if (EHX.Enabled())
      CI.m_EHXPowerIn += EHX.HeatFlow();
    }
  };

//--------------------------------------------------------------------------

void MN_Xfer::DumpDerivs()
  {
  dbgpln("--Xfr %-12s", FullObjTag());
  for (int i = 0; i < NoFlwIOs(); i++)
    if (IO_In(i))
      dbgpln("            In  >> :[%14.6g][%14.6g]|[%14.6g] %14.6g %14.3fC %s",
             IOConduit(i)->QMass(som_SL), IOConduit(i)->QMass(som_Gas) ,
             IOConduit(i)->msHz(), 
             IOConduit(i)->totHf()/GTZ(IOConduit(i)->QMass(som_ALL)),
             K2C(IOConduit(i)->Temp()), Nd_Rmt(i)->FullObjTag());
  for (i = 0; i < NoFlwIOs(); i++)
    if (IO_Out(i))
      dbgpln("            Out << :[%14.6g][%14.6g]|[%14.6g] %14.6g %14.3fC %s",
             IOConduit(i)->QMass(som_SL), IOConduit(i)->QMass(som_Gas) ,
             IOConduit(i)->msHz(), 
             IOConduit(i)->totHf()/GTZ(IOConduit(i)->QMass(som_ALL)),
             K2C(IOConduit(i)->Temp()), Nd_Rmt(i)->FullObjTag());
  }

//--------------------------------------------------------------------------

dword MN_Xfer::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (gs_Exec.Stopped())
    {
    Status |= FNS_UNoFlw;
    //Status |= (m_FEP.FlwRqdActive() ? FNS_UFlw : FNS_UNoFlw);
    }
  else
    {
    if (NoFlwIOs())
      {
      int HasFlw=0;
      double TFlw=0;
      for (int i=0; i<NoFlwIOs(); i++)
        {
        TFlw+=IOQm_In(i);
        if (IOConduit(i)->QMass()>UsableMass)
          HasFlw=1;
        }
      Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
      if (TFlw>1.0e-6)
        Status |= FNS_NettQmP;
      else if (TFlw<-1.0e-6)
        Status |= FNS_NettQmN;
      }
    }
  return Status;
  };

//==========================================================================
//
//
//
//==========================================================================

XID xidKConst = MdlBsXID(17001);
XID xidKFact  = MdlBsXID(17000);
XID xidPDiam  = MdlBsXID(17002);
XID xidVDiam  = MdlBsXID(17003);

#define DllImportExport

DEFINE_TAGOBJ(TE_KFact);
class TE_KFact : public CFlwEqn
  {
  enum TeeTypes { TT_Straight, TT_Branch};
  static DDBValueLst DDBTT[];
  public:
    //double         KFact, KFactOp, PwrLaw;//, OpDP, OpVol,OpNVol;
    byte              iType;
    double            dPipeArea;
    double            dDensMeas;
    double            dViscMeas;
    double            dVelMeas;
    double            dFricFac;
    DualDbl           KConst;
    CFrictionFactor   m_FF;
    double            m_dDP;

    TE_KFact(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~TE_KFact();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

DDBValueLst TE_KFact::DDBTT[]={
      {TT_Straight, "Straight"},
      {TT_Branch,   "Branch"},
      {0}};

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_FLWEQN(TE_KFact, TeeEntryGroup.Name(), "TE_KFact", "", TOC_SYSTEM,
                  "K Factor",
                  "K Factor - Darcy");

TE_KFact::TE_KFact(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) : \
CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  iType=TT_Straight;
  dPipeArea=0.1;
  dDensMeas=0;
  dViscMeas=0;
  dVelMeas=0;
  dFricFac=0;
  m_dDP=0;
  };
//--------------------------------------------------------------------------

TE_KFact::~TE_KFact()
  {
  }

//--------------------------------------------------------------------------

void TE_KFact::BuildDataDefn(DataDefnBlk & DDB)
  {
  KConst.BuildDataDefn(DDB, "KConst", "", DC_,    "",      xidKConst, NULL,   "Initial", "Required");
  DDB.Byte   ("Type",       "",           DC_,    "",      &iType,  NULL,  isParm|SetOnChange, DDBTT);
  DDB.Double ("PipeDiam",   "D-Pipe",     DC_L,   "mm",    xidPDiam,  NULL,  isParmStopped);
  DDB.Double ("Density",    "Rho",        DC_Rho, "kg/m^3",&dDensMeas, NULL, isResult);
  DDB.Double ("Viscosity",  "Visc",       DC_Visc,"cP",    &dViscMeas, NULL, isResult);
  DDB.Double ("Velocity",   "Vel",        DC_Ldt, "m/s",   &dVelMeas,  NULL, isResult);
  DDB.Double ("FricFac",    "fT",         DC_,    "",      &dFricFac,  NULL, isResult);
  m_PhD.KFact.BuildDataDefn(DDB, "ResistCoeff", "K", DC_, "", xidKFact, NULL, "Calculated", "Required");
  DDB.Double ("PressDrop",   "DP",        DC_DP,    "kPa",    &m_dDP,     NULL, isResult);
  BuildDataDefnOveride(DDB);
  };

// --------------------------------------------------------------------------

flag TE_KFact::DataXchg(DataChangeBlk & DCB)
  {
  if (KConst.DataXchg(DCB, xidKConst, this))
    return 1;
  if (m_PhD.KFact.DataXchg(DCB, xidKFact, this))
    return 1;
  switch (DCB.lHandle)
    {
    case xidPDiam: DCB.D=Area2Diam(dPipeArea); return 1 ;
    };
  return CFlwEqn::DataXchg(DCB);
  }

//--------------------------------------------------------------------------

flag TE_KFact::ValidateData(ValidateDataBlk & VDB)
  {
  KConst.SetVal(iType==TT_Branch ? 50.0 : 10.0, this);
  KConst.SetVal(ValidateRange(VDB, "KConst", 0.001, KConst.Val(), 1000.0), this);
  if (Valid(m_PhD.KFact.Ovr()))
    m_PhD.KFact.SetOvr(ValidateRange(VDB, "K", 0.0, m_PhD.KFact.Ovr(), 1.0E6), this);
  return True;
  }

//--------------------------------------------------------------------------

//void TE_KFact::StartSolution(rFlwEqnBlk FE)
//  {
//  };

//--------------------------------------------------------------------------

flag TE_KFact::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  dDensMeas=Max(0.001, FE.MeanRho(pProps));
  dViscMeas=FE.MeanViscosity(pProps);
  if (pPhD0 && pPhD0->IsPipe())
    dPipeArea=pPhD0->Area();
  else if (pPhD1 && pPhD1->IsPipe())
    dPipeArea=pPhD1->Area();
  else
    {};

  m_PhD.SetArea(dPipeArea);

  dVelMeas=FE.SetVelMeasRange(dDensMeas, FE.Area(), 0.001);

  double DqScl=1.001;
  double D=Area2Diam(dPipeArea);
  dFricFac=m_FF.Turbulent(D, 5.0e-5, dDensMeas, dViscMeas);
  m_PhD.KFact.SetVal(KConst()*dFricFac, this);
  double DP=PressDropKFact(FE.VelMeas(), dDensMeas, m_PhD.KFact(), 1.0);
  double DP1=PressDropKFact(FE.VelMeas(DqScl), dDensMeas, m_PhD.KFact(), 1.0);

  double OnePhDPQ=-FE.QmSign()*DP;
  double OnePhDPZ=/*FE.QmSign()**/-0.001*9.81*FE.Rise()*dDensMeas;
  FE.SetVelocity(FE.QmSign()*FE.VelMeas());
  FE.SetDPq(OnePhDPQ, -FE.QmSign()*(DP1-DP)/FE.DQmMeas(DqScl));
  FE.SetDPa(0.0, 0.0);
  FE.SetSlipRatio();
  FE.SetVoidFraction();
  FE.SetOnePhPart();
  FE.SetTempOut(4);

  FE.SetDPz(OnePhDPZ);

  m_dDP=DP;

//dbgpln("%14.4g %14.4g %14.4g %s", dVelMeas, FE.QmMeas(), DP, FullObjTag());

  ASSERT_ALWAYS(Valid(FE.DPq()) && _finite(FE.DPq()), "Bad Friction Value")

  return True;
  };

