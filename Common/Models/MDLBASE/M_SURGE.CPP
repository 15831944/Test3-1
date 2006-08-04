//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __M_SURGE_CPP
#include "m_surge.h"
#include "dbgmngr.h"

//#include "optoff.h"

#define dbgMSurge       (0 || WITHDEBUG)

#if dbgMSurge
static CDbgMngr dbgEvalSrgDerivsDamped  ("MSurge",   "EvalSrgDerivsDamped");
static CDbgMngr dbgEvalConverge         ("MSurge",   "EvalConverge");
static CDbgMngr dbgODEOperate           ("MSurge",   "ODEOperate");
static CDbgMngr dbgODEOperateSpc        ("MSurge",   "ODEOperateSpc");
static CDbgMngr dbgODEOperateDbgBrk     ("MSurge",   "ODEOperateDbgBrk");
#endif

//===========================================================================

MN_Surge::MN_Surge(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  Contents("Content", this, TOA_Embedded),
  QFeed("QFeed", this),
  QProd("QProd", this),
  //QSigmaOut("QSigOut", this, TOA_Embedded),
  RB(this, true),
  HX(this, NULL),
  VLE(this, VLEF_QVFlash),
  //Preset(&VLE),
  m_PresetImg("PresetImg", this, TOA_Embedded),
  m_PBPCtrl0(PrjFileVerNo()<32 ? PBPC_MinFd : PBPC_Atmos, Std_P, true),
  m_EqThermals(this, &Contents),
  m_Spill(eDIO_Spill, this, false, true, "Spill", IOId_Spill2Area, IOId_AreaSpillI),
  m_Vent(eDIO_Vent, this, false, true, "Vent", IOId_Vent2Area, IOId_AreaVentI),
  m_AccIn(eDIO_Accum, this, false, true, "Accumulation"),
  m_AccOut(eDIO_Deplete, this, false, true, "Depletion"),
  m_AccCalc(this, m_AccIn, m_AccOut)
  {
  //pQFeed=NULL;
  //pQProd=NULL;
  AttachClassInfo(nc_Process,NULL, &PipeEntryGroup);
  Contents.SetStateAction(IE_Integrate);
  m_EqThermals.SetStateAction(IE_Integrate);
  if (SolveInlineMethod())
    m_EqThermals.m_bActive = false;
  Contents.SetVentExpandOK();
  Contents.SetVLE(&VLE);
  m_PresetImg.SetView(SVV_AsMassFrac);
  m_PresetImg.bSpeciesOnly=true;

  m_QmAcc=0.0;
  m_QvAcc=0.0;
  //pHX=NULL;

  SS_Lvl=0.5;

  GSM.SetIOIdRange(0, IOId_HX-1);

  bShowQFeed = 0;
  bShowQProd = 0;
  m_PrevQmV=0.0;
  m_PrevQmL=0.0;

  m_CyclicStorage.m_bOn=false;
  //m_CyclicStorage.m_bCycleIt=false;
  m_CyclicStorage.m_dFracRetained=0.01;
  m_CyclicStorage.m_dCycleTime=300.0;
  m_CyclicStorage.m_dTotalMass=0.0;
  m_CyclicStorage.m_dTotalVolume=0.0;

  POffset=0;

  fActiveHoldOK_PB=true;
  fActiveHoldOK_Dyn=true;
  m_fActiveHoldIfSS=true;

  SetAllowedModes(true, NM_All|SM_Direct|SM_Inline|SM_Buffered|HM_All);//"Transfer", "Inline", "Buffered");

  Contents.SetSpillEtc(&m_Spill, &m_Vent);
  };

// -------------------------------------------------------------------------

MN_Surge::~MN_Surge()
  {
  };

//--------------------------------------------------------------------------

void MN_Surge::AddMdlClosed(DataDefnBlk & DDB)
  {
  DDB.CheckBoxBtn("Closed",         "",  DC_,     "",      xidClosed,           this,
    isParmStopped|(!gs_Environment.HasGas() ? noView|noFileAtAll:0) , DDBYesNo);
  }

//--------------------------------------------------------------------------

void MN_Surge::AddMdlNetworked(DataDefnBlk & DDB)
  {
  if (PrjFileVerNo()<56 && DDB.DoingPutData())
    {
    DDB.CheckBoxBtn("Press_Damping",      "",  DC_,     "",      xidNetPressDamp,     this,
      isParmStopped|(!gs_Environment.HasGas() || !Contents.Closed() ? noView|noFileAtAll:0) , DDBYesNo);
    }
  DDB.CheckBoxBtn("Networked",      "",  DC_,     "",      xidNetPressDamp,     this,
    isParmStopped|(!gs_Environment.HasGas() || !Contents.Closed() ? noView|noFileAtAll:0) , DDBYesNo);
  }

// -------------------------------------------------------------------------

flag MN_Surge::DataXchg(DataChangeBlk & DCB)
  {
  if (RB.DataXchg(DCB))
    return 1;

  if (HX.DataXchg(DCB))
    return 1;
  if (VLE.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==1 && m_Spill.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==2 && m_Vent.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==3 && m_AccIn.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==4 && m_AccOut.DataXchg(DCB))
    return 1;

//  if (PresetImg.DataXchg(DCB))
//    return 1;

  switch (DCB.lHandle)
    {
    case xidClosed:
      if (DCB.rB)
        Contents.SetClosed(*DCB.rB, DCB.ForView());
      DCB.B=Contents.Closed();
      return 1;
    case xidNetPressDamp:
      if (DCB.rB)
        Contents.SetNetPressDamp(*DCB.rB);
      DCB.B=Contents.NetPressDamp();
      return 1;
    case xidRho:
      DCB.D=Contents.Rho(som_ALL);
      return 1;
    case xidNRho:
      DCB.D=Contents.NRho(som_ALL);
      return 1;
    case xidLevel:
      DCB.D=Contents.RLevel();
      return 1;
    case xidPMean:
      DCB.D=Contents.Press();
      return 1;
    case xidSolFrac:
      DCB.D=Contents.MassFrac(som_Sol);
      return 1;
    case xidLiqFrac:
      DCB.D=Contents.MassFrac(som_Liq);
      return 1;
    case xidVapFrac:
      DCB.D=Contents.MassFrac(som_Gas);
      return 1;
    case xidMoleVapFrac:
      DCB.D=Contents.MoleFrac(som_Gas);
      return 1;
    case xidTemp:
      DCB.D=Contents.Temp();
      return 1;
    case xidPBSurgeCycleIt:
      if (DCB.rB && *DCB.rB)
        SetCyclicStorage();
      DCB.B=0;
      return 1;
    };

  return MdlNode::DataXchg(DCB);
  }

//---------------------------------------------------------------------------

flag MN_Surge::ValidateData(ValidateDataBlk & VDB)
  {
  Set_JoinP(0, Contents.Press());
//  Set_JoinP_Stb(0, Contents.Press());
//  Set_JoinP_Imp(0, Contents.Press());
  m_EqThermals.ValidateData(VDB);
  return MdlNode::ValidateData(VDB);

  }

//--------------------------------------------------------------------------

long MN_Surge::CheckDataRanges(bool CheckIO)
  {
  return MdlNode::CheckDataRanges(CheckIO)+Contents.CheckDataRanges();
  };

//--------------------------------------------------------------------------

CFlange * MN_Surge::GetFlange(int IOId)
  {
  switch (IOId)
    {
    case IOId_Spill2Area:   return &m_Spill.Flange;
    case IOId_Vent2Area:    return &m_Vent.Flange;
    //case IOId_AccIn2Area:   return &m_AccIn.Flange;
    //case IOId_AccOut2Area:  return &m_AccOut.Flange;
    }
  return NULL;
  };

//--------------------------------------------------------------------------

void MN_Surge::PostConnect(int IONo)
  {
  if (!PostConnectDirect(IONo))
    {
    MdlNode::PostConnect(IONo);
    IOFB(IONo,0)->AssignFlwEqnGroup(PipeEntryGroup, PipeEntryGroup.Default(), this);
    }
  };

//---------------------------------------------------------------------------

void MN_Surge::PreDisConnect(int IONo)
  {
  if (!PreDisConnectDirect(IONo))
    MdlNode::PreDisConnect(IONo);
  }

// ---------------------------------------------------------------------------

flag MN_Surge::InitialiseSolution()
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      Contents.dwDisplay=SPDF_ShapeData|SPDF_PBMode;
      Contents.SetStateAction(IE_Disabled);
      m_EqThermals.SetStateAction(IE_Disabled);
      break;
    case SM_Inline:
    case SM_Buffered:
      Contents.dwDisplay=SPDF_All;
      Contents.SetStateAction(SolveInlineMethod() ? IE_SaveState : IE_Integrate);
      if (SolveInlineMethod())
        m_EqThermals.SetStateAction(SolveInlineMethod() ? IE_SaveState : IE_Integrate);
      else
        m_EqThermals.SetStateAction(IE_Disabled);
      break;
    }

  return 1;
  };

//--------------------------------------------------------------------------

void MN_Surge::SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn)
  {
  CSetDatumsData SDD[]=
    {
      {First64IOIds, &Contents},
      {0}
    };
  SetDatums_Node(Pass, List, IOIn, SDD);
  };

//--------------------------------------------------------------------------

void MN_Surge::SetDatumsDone()
  {
  MdlNode::SetDatumsDone();
  SortSurgeIOData SDD[]=
    {
      {First64IOIds, &Contents, &ContentHgtOrd},
      {0}
    };
  SortSurgeIO(SDD);
  };

//--------------------------------------------------------------------------

flag MN_Surge::Set_Sizes()
  {
  for (int i=0; i<NoFlwIOs(); i++)
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

void MN_Surge::SetState(eScdMdlStateActs RqdState)
  {
  MdlNode::SetState(RqdState);
  Contents.SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
      break;
    case MSA_Empty:
      m_PrevQmV=0.0;
      m_PrevQmL=0.0;
      break;
    case MSA_PreSet:
      m_PrevQmV=0.0;
      m_PrevQmL=0.0;
      break;
    case MSA_ZeroFlows:
      m_PrevQmV=0.0;
      m_PrevQmL=0.0;
      break;
    case MSA_EmptySpillTargets:
      if (_stricmp(FullObjTag(), "Floor")==0)
        Contents.SetState(MSA_Empty);
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  };

//--------------------------------------------------------------------------

long MN_Surge::NodeFlwTask(NodeFlwTasks Task)
  {
  if(!m_CyclicStorage.m_bOn)
    return MdlNode::NodeFlwTask(Task);

  switch (Task)
    {
    case NFT_PBQueryReInit:
      return 0;
    case NFT_PBInit:
      return 0;
    case NFT_PBReInit:
      return 0;
    case NFT_PBQueryRemove:
      return 0;
    case NFT_PBRemove:
      return 0;
    default:
      return MdlNode::NodeFlwTask(Task);
    }
  return 0;
  };

//---------------------------------------------------------------------------

void MN_Surge::ConfigureJoins()
  {
  const int JnId_Surge=0;
  const int JnId_HX   =1;
  const int JnId_Spill=2;
  switch (NetMethod())
    {
    case NM_Probal:
      for (int i=0; i<NoProcessIOs(); i++)
        {
        int Id=IOId_Self(i);
        if (IsMLnkIOId(Id))
          {
          if (Id<IOId_HX)
            SetIO_Join(i, JnId_Surge);
          else
            SetIO_Join(i, JnId_HX);
          }
        }
      break;
    case NM_Dynamic:
      for (int i=0; i<NoProcessIOs(); i++)
        {
        int Id=IOId_Self(i);
        if (IsMLnkIOId(Id))
          {
          if (Id<IOId_HX)
            SetIO_Open(i, JnId_Surge, Contents.Closed() && Contents.NetPressDamp(), SolveInlineMethod() ? ESS_Denied : ESS_Allowed);
          else
            SetIO_Join(i, JnId_HX);
          }
        else
          SetIO_Direct(i, JnId_Surge);
        }
      for (int ii=NoProcessIOs(), jj=JnId_Surge+1; ii<NoFlwIOs(); ii++, jj++)
        SetIO_Direct(ii, jj);
      break;
    }
  };

//--------------------------------------------------------------------------

void MN_Surge::StartStep()
  {
  MdlNode::StartStep();
  m_CnStart.AtStart(Contents);

  if (Contents.MeanResTimeCalcsReqd())
    Contents.InitMeanResTimeCalcs();

  Contents.m_MixSet.StartStep();
  m_SrgCtrl.StartStep();
  m_AccCalc.Start(0, Contents);

  if (1)  //restore Qm/QvAcc
    {
    m_QmAcc=Contents.Mass(som_SL);
    m_QvAcc=Contents.Volume(som_SL);
    }
  }

//--------------------------------------------------------------------------

//void MN_Surge::EvalFlowInfo()
//  {
//  MdlNode::EvalFlowInfo();
//  }

//--------------------------------------------------------------------------
/*#F:This determines the pressure of the contents and the pressure at each
inlet and outlet of the surge unit.*/
void MN_Surge::EvalJoinPressures(long JoinMask)
  {
  if (NoFlwIOs()>0)
    switch (NetMethod())
      {
      case NM_Probal:
        if (NoProcessJoins()>=1)
          EvalPBJoinPressure(0, &m_PBPCtrl0, QProd());
        if (NoProcessJoins()>=2)
          EvalPBJoinPressure(1);
        break;
      case NM_Dynamic:
        {
        #if dbgEvalPres
        dbgpln("EvalJoinPressures:%s",FullObjTag());
        #endif

        IOP_RhoH_Info RhoHInfo(Contents);
        double Pm=ContainerMeanPress(RhoHInfo, POffset);
        Set_JoinP(0, Pm);
        for (int i=0; (i<NoProcessIOs()) && (IOId_Self(i)<IOId_HX); i++)
          Set_IOP_RhoH_Self(i,Pm,RhoHInfo);
        }
        break;
      }
  };


//--------------------------------------------------------------------------

CSpPropInfo* MN_Surge::IOGetNetProps(int i, double Qm)
  {
  int Id=IOId_Self(i);
  if (Id!=IOId_HX)
    DoIOGetNetProps_Surge(i, Qm);
  return IONetProps(i);//MdlNode::IOGetNetProps(i, Qm);
  }

//--------------------------------------------------------------------------

flag MN_Surge::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  if (m_Spill.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  if (m_Vent.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  if (m_AccIn.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  if (m_AccOut.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  switch (Task)
    {
    case FET_SetQm:
      IOFB(IONo, FE)->SetDPb(0.0, 0.0);
      IOFB(IONo, FE)->SetDPbX(0.0);
      IOFB(IONo, FE)->SetDPq(0.0, 0.0);
      break;
    case FET_CalcDP:
      IOFB(IONo, FE)->EvaluateFlwEqn(Task, pProps, true, false, 1.0, &IOFBFlng_Rmt(IONo)->PhD(), NULL);//, IOP_Self(IONo), IOP_Flng(IONo));//, *IOConduit(IONo), IC);
    }
  return true;
  };

//--------------------------------------------------------------------------
/*#F:This determines the state of the contents contained within the surge unit.*/
void MN_Surge::EvalState()
  {
  Contents.SetCI(1, Contents.Spill.Flowing());
  Contents.SetCI(4, Contents.Vent.Flowing());
  Contents.TestLevelLimits();
  };

// --------------------------------------------------------------------------

void MN_Surge::EvalProductsInit(EvalProductsInitTasks Task)
  {
  EvalProductsInit_SurgeLevel(Task, Contents, First64IOIds);
  //EvalProductsInit_Node(FwdEval);
  }

//--------------------------------------------------------------------------
/*#F:This determines what material is leaving through each outlet,
and rate of change of the contents resulting from the flow, of material, out of each outlet
of the surge unit.*/
void MN_Surge::EvalProductsSurge(CNodeEvalIndex & NEI)
  {
  if (fDoDbgBrk)
    { int xxx=0; }
  ASSERT(NetDynamicMethod());
  if (SolveBufferedMethod())
    {
    dword IOMsk=First64IOIds;
    SigmaQInPMin(QFeed(), som_ALL, IOMsk);
    if (0)
      VLE.QPFlash(QFeed(), Contents.Press(), 0.0, VLEF_Null);
    EvalProducts_SurgeLevel(ContentHgtOrd, &QFeed, &m_SrgCtrl);
    //EvalProducts_SurgeLevel(Contents, QFeed, IOMsk, PMU());
    }
  }

//--------------------------------------------------------------------------
/*#F:This determines what material is leaving through each outlet,
and rate of change of the contents resulting from the flow, of material, out of each outlet
of the surge unit.*/
void MN_Surge::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (fDoDbgBrk)
    { int xxx=0; }
  switch (SolveMethod())
    {
    case SM_Direct:
      if (m_CyclicStorage.m_bOn)
        {
        SetCyclicStorageOut();
        }
      else if (NoProcessJoins()==2)
        {
        Xfer_EvalProducts_Info I0(0, Joins[0].Pressure(), &QFeed, &QProd, RB(), GSM(), EHX(), &m_PBPCtrl0);
        CPBPressCtrl PBCtrl1;
        Xfer_EvalProducts_Info I1(1, Joins[1].Pressure(), NULL, NULL, NULL, NULL, NULL, &PBCtrl1);
        Xfer_EvalProducts(I0, I1, &HX);
        }
      else if (NoProcessJoins()>=1)
        Xfer_EvalProducts(0, Joins[0].Pressure(), &QFeed, &QProd, RB(), GSM(), EHX(), &m_PBPCtrl0);
      break;
    case SM_Inline:
      SigmaQInPMin(QFeed(), som_ALL, First64IOIds);
      EvalProducts_SurgeLevel(ContentHgtOrd, &QFeed, &m_SrgCtrl);
      //EvalProducts_SurgeLevel(Contents, QFeed, First64IOIds, PMU());
      // fall thru
    case SM_Buffered:
      if (NoProcessJoins()>=2)
        Xfer_EvalProducts(1, Joins[1].Pressure(), NULL, &QProd(), NULL, NULL, NULL);//RB());

      if (HX())
        {
        HX.SetPrimary(NULL, Contents, som_ALL, 1.0, Contents.Press());
        if (NIOsWithId_Self(IOId_HX)>=2)
          {
          for (int ioOut1 = IOWithId_Self(IOId_HX); ioOut1<NoFlwIOs(); ioOut1++)
            if (IO_Out(ioOut1))
              break;
          if (ioOut1<NoFlwIOs())
            {
            SigmaQInPMin(*IOConduit(ioOut1), som_ALL, IOId_HX, IOId_HX);
            HX.SetSecondary(NULL, *IOConduit(ioOut1), IOP_Self(ioOut1));
            }
          }

        HX.EvalProducts();

        }
      if (SolveInlineMethod())
        {
        EvalIntegral(NEI);
        }
      break;
    }
  }

//--------------------------------------------------------------------------
/*#F:This determines what material is arriving through each inlet,
and rate of change of the contents resulting from the flow, of material, into each inlet
of the surge unit. It a/lso calculates the mass and volume accumulation of the surge unit,
whether the surge unti is to overflow and, if so, the quantity of material that is to
overflow.*/

void MN_Surge::EvalDerivs(CNodeEvalIndex & NEI)
  {
  const int DoLast10=01;

  //dbgpln("EvalDerivs   - %s %s %s %s", SolveInlineMethod()?"Inline":"      ", IntegralDone()?"IntDone":"       ", GetActiveHold()?"Hold":"    ", Tag());

  Contents.ZeroDeriv();


  if (/*!IntegralDone() &&*/ !GetActiveHold())
    {
    for (int i = 0; i < NoFlwIOs(); i++)
      {
      int Id=IOId_Self(i);
      if (Id!=IOId_HX)
        {
        SpConduit & Cd=*IOConduit(i);
        double FracAbove=(IOAbsFracHgt_Term(i, Contents)-Contents.Level())/GTZ(IOAperture(i));
        FracAbove=Range(0.0, FracAbove, 1.0);
        double EntR=FracAbove*IOEntrainRateA(i)*Cd.QMass(som_SL)+
                    (1.0-FracAbove)*IOEntrainRateB(i)*Cd.QMass(som_Gas);
        static int xx=0;
        if (xx && (IOSign(i)>0))
          VLE.QPFlash(Cd, Contents.Press(), 0.0, VLEF_Null);
        Contents.AddDeriv(Cd, IOSign(i), EntR);
        }
      }
    //dbgpln("EvalDerivs   - AddInflow ");

    //    CEquipmentThermals
    m_EqThermals.EvalDerivs();

  //  if (RB())
  //    {
  //    StkSpConduit QOutEst;
  //    SigmaQOut(QOutEst(), First64IOIds);
  //    RB.EvalDerivs(Contents, QInEst, QOutEst());
  //    }

    }
  if (HX())
    HX.EvalDerivs();

  VLE.AddQVDerivs(Contents, 0, 0);

  };

//--------------------------------------------------------------------------

void MN_Surge::ODEOperate(CODEDataBlock & ODB)
  {
  bool DoIt=true;
  switch (ODB.m_Cmd)
    {
    case eStateAdvance:
      DoIt=!IntegralDone();
    case eStateConverge:
    case eStateLoad:
    case eStateDiskLoad:
      {
      if (DoIt && !GetActiveHold())
        {
        #if dbgMSurge
        if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
          {
          dbgp("ODE:---  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f",
            m_CnStart.DTemp(Contents), Contents.Temp(),
            m_CnStart.DPress(Contents), Contents.Press(),
            m_CnStart.DMass(Contents), Contents.Mass(),
            m_CnStart.DLevel(Contents)*100, Contents.Level()*100, FullObjTag());
          if (dbgODEOperateSpc())
            for (int s=0; s<SVValueCount(); s++)
              dbgp(" %12.5g", Contents.MArray()[s]);
          dbgpln(" %s.Contents", FullObjTag());
          }
        #endif
        Contents.ODEOperate(ODB);
        m_EqThermals.ODEOperate(ODB);

        #if dbgMSurge
        if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
          {
          dbgp("ODE:Int  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f",
            m_CnStart.DTemp(Contents), Contents.Temp(),
            m_CnStart.DPress(Contents), Contents.Press(),
            m_CnStart.DMass(Contents), Contents.Mass(),
            m_CnStart.DLevel(Contents)*100, Contents.Level()*100, FullObjTag());
          if (dbgODEOperateSpc())
            for (int s=0; s<SVValueCount(); s++)
              dbgp(" %12.5g", Contents.MArray()[s]);
          dbgpln(" %s.Contents", FullObjTag());
          }
        #endif

        ConvergeStates(CConvergeStateBlk(ODB));
        #if dbgMSurge
        if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
          {
          dbgp("ODE:Cvg  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f",
            m_CnStart.DTemp(Contents), Contents.Temp(),
            m_CnStart.DPress(Contents), Contents.Press(),
            m_CnStart.DMass(Contents), Contents.Mass(),
            m_CnStart.DLevel(Contents)*100, Contents.Level()*100);
          if (dbgODEOperateSpc())
            for (int s=0; s<SVValueCount(); s++)
              dbgp(" %12.5g", Contents.MArray()[s]);
          dbgpln(" %s.Contents", FullObjTag());
          }
        #endif
        }
      break;
      }
    case eStateFixDV:
    case eStateTest:
    case eStateSave:
    case eStateDiskSave:
      {
      Contents.ODEOperate(ODB);
      m_EqThermals.ODEOperate(ODB);
      break;
      }
    }
  };

//--------------------------------------------------------------------------

void MN_Surge::EvalIntegral(CNodeEvalIndex & NEI)
  {
  //dbgpln("EvalIntegral - %s %s %s %s", SolveInlineMethod()?"Inline":"      ", IntegralDone()?"IntDone":"       ", GetActiveHold()?"Hold":"    ", Tag());

  flag VLEOn=0 && VLE.Enabled();
  dword IOMsk=First64IOIds;
  StkSpConduit QPF("QPF", chLINEID(), this);
  //if (SolveInlineMethod())
  //  {
  //  SigmaQInPMin(QFeed(), som_ALL, IOMsk);
  //  //VLE.QPFlash(QFeed(), Contents.Press(), 0.0, VLEF_Null);
  //  EvalProducts_SurgeLevel(ContentHgtOrd, QFeed());
  //  //EvalProducts_SurgeLevel(Contents, QFeed(), IOMsk, PMU());
  //  }

  Contents.ZeroDeriv();
  #if dbgMSurge
  if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
    {
    dbgp("           %*s",151-24,"");
    if (dbgODEOperateSpc())
      for (int s=0; s<SVValueCount(); s++)
        dbgp(" %12.12s", SVImg(s).Tag());
    dbgpln(" %s.Contents", FullObjTag());
    dbgp("InI:---  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f",
      m_CnStart.DTemp(Contents), Contents.Temp(),
      m_CnStart.DPress(Contents), Contents.Press(),
      m_CnStart.DMass(Contents), Contents.Mass(),
      m_CnStart.DLevel(Contents)*100, Contents.Level()*100, FullObjTag());
    if (dbgODEOperateSpc())
      for (int s=0; s<SVValueCount(); s++)
        dbgp(" %12.5g", Contents.MArray()[s]);
    dbgpln(" %s.Contents", FullObjTag());
    }
  #endif
  for (int i = 0; i < NoProcessIOs(); i++)
    if (IOIdMask_Self(i) & IOMsk)
      {
      SpConduit * Cd;
      if (VLEOn && IO_In(i))
        {
        QPF->QSetF(*IOConduit(i), som_ALL, 1.0);
        VLE.QPFlash(QPF(), Contents.Press(), 0.0, VLEF_Null);
        Cd=&QPF();
        }
      else
        Cd=IOConduit(i);
      double FracAbove=(IOAbsFracHgt_Term(i, Contents)-Contents.Level())/GTZ(IOAperture(i));
      FracAbove=Range(0.0, FracAbove, 1.0);
      double EntR=FracAbove*IOEntrainRateA(i)*Cd->QMass(som_SL)+
                  (1.0-FracAbove)*IOEntrainRateB(i)*Cd->QMass(som_Gas);
      Contents.AddDeriv(*Cd, IOSign(i), EntR);
      #if dbgMSurge
      if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
        {
        dbgp("dM :IO   > Sgn:%2i EntR:%10.4f CdQm:%10.4f %*s",IOSign(i),EntR,Cd->QMass(),151-39-24,"");
        if (dbgODEOperateSpc())
          for (int s=0; s<SVValueCount(); s++)
            dbgp(" %12.5g", Contents.pModel->m_DC.m_pFd->m_M[s]*ICGetTimeInc());
        dbgpln(" %s", Nd_Rmt(i)->FullObjTag());
        dbgp("         < %*s",151-24,"");
        if (dbgODEOperateSpc())
          for (int s=0; s<SVValueCount(); s++)
            dbgp(" %12.5g", Contents.pModel->m_DC.m_pPr->m_M[s]*ICGetTimeInc());
        dbgpln(" %s %s", IOQmEst_In(i)?"IN":"", IOQmEst_Out(i)?"Out":"");
        }
      #endif
      }

  //dbgpln("EvalIntegral - AddInflow");
  //Contents.AddInFlowDeriv();

  if (Contents.SpillExists())
    {
    //if (Contents.Spill.Cd.QMass()>0)
    //  dbgpln("EvalIntegral - AddDeriv Overflow: %14.3f %s", Contents.Spill.Cd.QMass(), FullObjTag());
    Contents.AddDeriv(Contents.Spill.Cd, -1.0);
    }
  if (Contents.VentExists())
    {
    //if (Contents.Vent.Cd.QMass()>0)
    //  dbgpln("EvalIntegral - AddDeriv Vent    : %14.3f %s", Contents.Vent.Cd.QMass(), FullObjTag());
    Contents.AddDeriv(Contents.Vent.Cd, -1.0);
    }
  #if dbgMSurge
  if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
    {
    dbgp("dM :Ovr  > %*s",151-24,"");
    if (dbgODEOperateSpc())
      for (int s=0; s<SVValueCount(); s++)
        dbgp(" %12.5g", Contents.pModel->m_DC.m_pFd->m_M[s]*ICGetTimeInc());
    dbgpln(" %s.Contents", FullObjTag());
    dbgp("         < %*s",151-24,"");
    if (dbgODEOperateSpc())
      for (int s=0; s<SVValueCount(); s++)
        dbgp(" %12.5g", Contents.pModel->m_DC.m_pPr->m_M[s]*ICGetTimeInc());
    dbgpln("");
    }
  #endif

  Contents.pModel->m_DC.Finalise(Contents.pModel, ICGetTimeInc());
  //Contents.m_DC.Finalise(this, ICGetTimeInc());

  #if dbgMSurge
  if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
    {
    dbgp("dM :Final> %*s",151-24,"");
    if (dbgODEOperateSpc())
      for (int s=0; s<SVValueCount(); s++)
        dbgp(" %12.5g", Contents.pModel->m_DC.m_pFd->m_M[s]*ICGetTimeInc());
    dbgpln(" %s.Contents", FullObjTag());
    dbgp("         < %*s",151-24,"");
    if (dbgODEOperateSpc())
      for (int s=0; s<SVValueCount(); s++)
        dbgp(" %12.5g", Contents.pModel->m_DC.m_pPr->m_M[s]*ICGetTimeInc());
    dbgpln("");
    dbgp("         = %*s",151-24,"");
    if (dbgODEOperateSpc())
      for (int s=0; s<SVValueCount(); s++)
        dbgp(" %12.5g", (Contents.pModel->m_DC.m_pFd->m_M[s]-Contents.pModel->m_DC.m_pPr->m_M[s])*ICGetTimeInc());
    dbgpln("");
    }
  #endif

  if (!GetActiveHold())
    {
    Contents.ApplyDerivs(ICGetTimeInc(), true);

    VLE.QVFlash(Contents, 0.0, VLEF_Null);
    }

  #if dbgMSurge
  if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
    {
    dbgp("InI:End  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f",
      m_CnStart.DTemp(Contents), Contents.Temp(),
      m_CnStart.DPress(Contents), Contents.Press(),
      m_CnStart.DMass(Contents), Contents.Mass(),
      m_CnStart.DLevel(Contents)*100, Contents.Level()*100);
    if (dbgODEOperateSpc())
      for (int s=0; s<SVValueCount(); s++)
        dbgp(" %12.5g", Contents.MArray()[s]);
    dbgpln(" %s.Contents", FullObjTag());
    }
  #endif
  SetIntegralDone(true);
  }

//--------------------------------------------------------------------------

void MN_Surge::ConvergeStates(CConvergeStateBlk &CSB)
  {
  ASSERT(NetDynamicMethod());

  if (RB())
    {
    #if dbgMSurge
    if (dbgEvalConverge())
      dbgpln("MN_Surge::ConvergeStates - RB %s", FullObjTag());
    #endif
    Contents.SetMeanResTimeCalcsReqd(true);
    RB.ConvergeStates(CSB, Contents, Contents.MeanResTimes());
    #if dbgMSurge
    if (dbgEvalConverge())
      dbgpln("-------------------------------------- ");
    #endif
    };

  if (!IntegralDone() && !GetActiveHold())
    VLE.QVFlash(Contents, 0.0, VLEF_Null);//, NULL, NULL, NULL);

  m_EqThermals.ConvergeStates(CSB);
  }

//--------------------------------------------------------------------------

/*#F:This is called once, after the iteration process and allows the discreet
manipulation of the contents contained by the surge unit.*/

void MN_Surge::EvalDiscrete()
  {
  //dbgpln("EvalDiscrete - %s %s", GetActiveHold()?"HOLD":"", Tag());

  if (GetActiveHold())
    return;

  if (Contents.MeanResTimeCalcsReqd())
    {
    SpConduit &Fd=*GetWrkConduit("CSFd", chLINEID());
    SpConduit &Pr=*GetWrkConduit("CSPr", chLINEID());
    SigmaQInPMin(Fd, som_ALL, First64IOIds);
    SigmaQOut(Pr, som_ALL, First64IOIds);
    Contents.CompleteMeanResTimeCalcs(Fd, Pr);
    }

  //if (Contents.SpillExists())
  //  {
  //  SpDirectCd & O=Contents.OverFlow;
  //  if (O.QMass()>1e-20)
  //    dbgpln("EvalDiscrete - AddTotal: %14.3f %s", O.QMass(), Tag());
  //  O.SetTotal(O.Total()+O.QMass()*ICGetTimeInc());
  //  }
  //if (Contents.VentExists())
  //  {
  //  SpDirectCd & O=Contents.GasVent();
  //  O.SetTotal(O.Total()+O.QMass()*ICGetTimeInc());
  //  }

  // Qualities
  for (int i = 0;  (i<NoProcessIOs()) && (IOId_Self(i)!=IOId_HX); i++)
    if (IO_In(i))
      Contents.AddDiscrete(*IOConduit(i), IOSign(i));

  m_AccCalc.Complete(0, Contents, ICGetTimeInc());

  if (1)  //restore Qm/QvAcc
    {
    m_QmAcc=Contents.Mass(som_SL)-m_QmAcc;
    m_QvAcc=Contents.Volume(som_SL)-m_QvAcc;
    }
  }

//---------------------------------------------------------------------------

flag MN_Surge::EvalPressureSens()
  {
  double BigDiff=0.0;

  Contents.SetHoldVentExpand();
  ContStartInfo Start(Contents);
  SetStaticHeadSensitivities(0, Contents, Start);

  // Must be closed and have contents
  if (!Contents.Closed() || Contents.Mass()< 1.0e-6)
    {
    ZeroVLSensitivities(0);
    Contents.ClrHoldVentExpand();
    return True;
    }

  SetVLSensitivities(0, Contents, Start, true);
  // Flash to Equilibrium
  SetEquilibratingV2L(0, Contents, Start);

  RestoreContents(Contents, Start);

  Contents.ClrHoldVentExpand();

  return true;//False;
  };

// --------------------------------------------------------------------------

void MN_Surge::ApplyDerivs(double dTime, flag DoDbg)
  {
  if (GetActiveHold())
    return;

  Contents.ApplyDerivs(dTime, true);
  }

//---------------------------------------------------------------------------

void MN_Surge::EvalStatistics(eScdCtrlTasks Tasks)
  {
  Contents.EvalStatistics(Tasks);
  MdlNode::EvalStatistics(Tasks);
  }

//--------------------------------------------------------------------------
//kga 9/1/2002: copied EvalPBMakeUpReqd and EvalPBMakeUpAvail from m_xfer.cpp so that with
//              GM on in demand and demand flows specified in tank outlet pipes, the makeup mode
//              stuff works as for ties!

//kga 18/2/2002: in some unique conditions (eg in contact-heater, flash-tank, etc with demand) this causes problems,
//               so for now only attempt this if GM on (eg tank)
void MN_Surge::EvalPBMakeUpReqd(long JoinMask)
  {
  if (GSM.Enabled())
    {
    //if (GSM.Enabled())
      GSM.MakeUpNodeTransferReqd(0);
    MakeUpNodeTransferReqd(0);
    }
  else
    MdlNode::EvalPBMakeUpReqd(JoinMask);
  };

//--------------------------------------------------------------------------

void MN_Surge::EvalPBMakeUpAvail(long JoinMask)
  {
  if (GSM.Enabled())
    {
    //if (GSM.Enabled())
      GSM.MakeUpNodeTransferAvail(0);
    MakeUpNodeTransferAvail(0);
    }
  else
    MdlNode::EvalPBMakeUpAvail(JoinMask);
  };

//--------------------------------------------------------------------------

void MN_Surge::SetCyclicStorage()
  {
  if (m_CyclicStorage.m_bOn)
    {
    dword IOMsk=First64IOIds;
    SigmaQInPMin(QFeed(), som_ALL, IOMsk);
    SigmaQOut(QProd(), som_ALL, IOMsk);

    Contents.SelectModel(&QFeed(), false);
    QProd().SelectModel(&Contents, true);

    double TotalMassIn=QFeed().QMass()*m_CyclicStorage.m_dCycleTime;
    double TotalMassOut=QProd().QMass()*m_CyclicStorage.m_dCycleTime;
    Contents.SubM(QProd(), som_ALL, TotalMassOut);
    Contents.AddM(QFeed(), som_ALL, TotalMassIn);
    m_CyclicStorage.m_dTotalMass=Contents.Mass();
    m_CyclicStorage.m_dTotalVolume=Contents.Volume();

    };
  }

//--------------------------------------------------------------------------

void MN_Surge::SetCyclicStorageOut()
  {
  if (m_CyclicStorage.m_bOn)
    {
    double QOut=0;
    dword IOMsk=First64IOIds;
    for (int i=0; i<NoFlwIOs(); i++)
      {
      if (IO_Out(i) && (IOIdMask_Self(i)&IOMsk))
        QOut+=IOQm_Out(i);
      }
    QOut=GTZ(QOut);

    double TotalMassOut=Contents.Mass()*Range(0.0, 1.0-m_CyclicStorage.m_dFracRetained, 1.0)/m_CyclicStorage.m_dCycleTime;

    for (i=0; i<NoFlwIOs(); i++)
      {
      if (IO_Out(i) && (IOIdMask_Self(i)&IOMsk))
        IOConduit(i)->QSetM(Contents, som_ALL, TotalMassOut*IOQm_Out(i)/QOut);
      }
    };
  }

//  return 0;
//  }

//--------------------------------------------------------------------------

void MN_Surge::ClosureInfo()
  {
  bool InRange=true;
  for (int i=0; i<NoFlwIOs(); i++)
    InRange = InRange && IOConduit(i)->SMFnsInRange();
  InRange = InRange && Contents.SMFnsInRange();
  FlwNode::SetCI(29, !InRange);

  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    if (RB.Enabled())
      RB.GetClosureInfo(CI);
    if (EHX.Enabled())
      CI.m_EHXPowerIn += EHX.HeatFlow();

    //if (Contents.SpillFlowing())
    //  {
    //  CI.m_HfLoss   += Contents.OverFlow.totHf();
    //  CI.m_HsLoss   += Contents.OverFlow.totHs();
    //  CI.m_MassLoss += Contents.OverFlow.QMass();
    //  }
    //if (Contents.VentFlowing())
    //  {
    //  CI.m_HfLoss   += Contents.GasVent.totHf();
    //  CI.m_HsLoss   += Contents.GasVent.totHs();
    //  CI.m_MassLoss += Contents.GasVent.QMass();
    //  }

    if (!m_CyclicStorage.m_bOn)
      return;

    }
  else
    { 
    if (m_Closure.GetSize()>0)
      {
      CClosureInfo &CI=m_Closure[0];
      CI.m_pRefMdl      = Contents.Model();
      CI.m_ContentMass  = Contents.Mass();
      CI.m_ContentHeat  = Contents.totHf();
      }
    }        
  };

//--------------------------------------------------------------------------

void MN_Surge::DumpDerivs()
  {
  if (fDoDbgBrk)
    {
    dbgpln("--Srg %-12s", FullObjTag());
    //double dmv=0.0,dml=0.0;
    //for (short s=0; s < ??SVValueCount(); s++)
    //  {
    //  if (SDB[s].IsGas())
    //    dmv+=Contents.SpdMass(s);
    //  else
    //    dml+=Contents.SpdMass(s);
    //  }
    //dbgpln("            Derivs : %14.6g  %14.6g | %14.6g  %14s %14s  ",
    //  dml,dmv,Contents.Duty(),"","");
    //dbgpln("            dMass/H:[%14.6g][%14.6g]|[%14.6g] %14s %14s  ",
    //  ICGetTimeInc()*dml,ICGetTimeInc()*dmv,ICGetTimeInc()*Contents.Duty(),"","");
    dbgpln("            Mass/H :[%14.6g][%14.6g]|[%14.6g] %14.6g %14.3fC ",
      Contents.Mass(som_SL),Contents.Mass(som_Gas),Contents.totHf(),
      Contents.totHf()/GTZ(Contents.Mass(som_ALL)),
      K2C(Contents.Temp()));
    for (int i = 0; i < NoFlwIOs(); i++)
      if (IO_In(i))
        dbgpln("            In  >> :[%14.6g][%14.6g]|[%14.6g] %14.6g %14.3fC %s",
               IOConduit(i)->QMass(som_SL), IOConduit(i)->QMass(som_Gas) ,
               IOConduit(i)->totHf(),
               IOConduit(i)->totHf()/GTZ(IOConduit(i)->QMass(som_ALL)),
               K2C(IOConduit(i)->Temp()), Nd_Rmt(i)->FullObjTag());
    for (i = 0; i < NoFlwIOs(); i++)
      if (IO_Out(i))
        dbgpln("            Out << :[%14.6g][%14.6g]|[%14.6g] %14.6g %14.3fC %s",
               IOConduit(i)->QMass(som_SL), IOConduit(i)->QMass(som_Gas) ,
               IOConduit(i)->totHf(),
               IOConduit(i)->totHf()/GTZ(IOConduit(i)->QMass(som_ALL)),
               K2C(IOConduit(i)->Temp()), Nd_Rmt(i)->FullObjTag());
    }
  }

//--------------------------------------------------------------------------

#if DOPBSMOOTHING
void MN_Surge::EvalMsAGDatas()
  {
  if (MsJoins.GetSize()<1)
    MsJoins.SetSize(1);
  if (RB.Enabled())
    RB.GetMsAGData(MsJoins[0]);
  else
    {
    //MsJoins[0].Clear();
    MsJoins[0].m_fDataIsValid=false;
    MsJoins[0].m_ConstBG.Clear();
    MsJoins[0].m_ConstAG.Clear();
    MsJoins[0].m_PropBG.Clear();
    MsJoins[0].m_PropAG.Clear();
    MsJoins[0].m_Gamma.Clear();
    }


  MdlNode::EvalMsAGDatas();
  };
#endif

//--------------------------------------------------------------------------

dword MN_Surge::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (GetStatusAsStopped())
    {
    Status |= FNS_UNoFlw;
    //Status |= (SolveMethod()==SM_Inline  ? FNS_UFlw : FNS_UNoFlw);
    }
  else if (NoFlwIOs())
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
  return Status;
  };

// ===========================================================================
