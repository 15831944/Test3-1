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
  //m_Accumulator("Accumulator", this),
  m_RB(this, false),
  m_PBPCtrl0(PBPC_MinFd, Std_P, true),
  m_FTM(this),
  m_Leak(eDIO_Leak, this, true, false, "Leak", IOId_XferLeak, IOId_AreaLeakI),
  m_Evap(this)
#if (WITHBLOCKEVALUATOR)
  , m_BlkEval(this, &m_RB, NULL, &EHX, NULL/*&m_VLE*/, &m_Evap)
#endif
  {
  AttachClassInfo(nc_MXfer,NULL);
  Joins.SetSize(1);
  m_NetworkIsolator=false;
  m_Accumulator=NULL;
  m_AccIn=NULL;
  m_AccOut=NULL;
  m_AccCalc=NULL;
  SetAllowedModes(false, NM_All|SM_Direct|SM_Inline|HM_All);
  //SetInlineIntegral(true);
  };

MN_Xfer::~MN_Xfer()
  {
  }

// -------------------------------------------------------------------------

flag MN_Xfer::DataXchg(DataChangeBlk & DCB)
  {
  if (DCB.dwUserInfo==100 && m_Leak.DataXchg(DCB))
    return 1;
#if (WITHBLOCKEVALUATOR)    
  if (m_BlkEval.DataXchg(DCB))
    return 1;
#endif
  if (m_RB.DataXchg(DCB))
    return 1;

  switch(DCB.lHandle)
    {
    case xidNetworkIsolator:
      {
      if (DCB.rB)
        {
        
        if (!DCB.ForFileSnpScn())
          DoGlobalDisconnect();

        m_NetworkIsolator=*DCB.rB!=0;
        if (m_NetworkIsolator)
          {
          m_Accumulator=new SpContainer("Accumulator", this, TOA_Embedded);
          //m_Accumulator.ChangeShape(&SpShPipeClass);
          m_Accumulator->SetClosed(true);
          m_Accumulator->SetPressAsSet(true);
          m_Accumulator->SetStateAction(IE_Integrate);
          m_Accumulator->dwDisplay = SPDF_DynMode;
          m_AccIn=new CDirectFlwIO(eDIO_Accum, this, false, true, "Accumulation");
          m_AccOut=new CDirectFlwIO(eDIO_Deplete, this, false, true, "Depletion");
          m_AccCalc=new CDirectAccCalc(this, *m_AccIn, *m_AccOut);

          //Joins.SetSize(2);
          }
        else
          {
          delete m_AccCalc;
          delete m_AccOut;
          delete m_AccIn;
          delete m_Accumulator;
          m_AccCalc=NULL;
          m_AccOut=NULL;
          m_AccIn=NULL;
          m_Accumulator=NULL;

          //Joins.SetSize(1);
          }
        if (!DCB.ForFileSnpScn())
          DoGlobalConnect();
        }
      DCB.B=m_NetworkIsolator;
      return 1;
      }
    }
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

#if (WITHBLOCKEVALUATOR)    
  if (!m_BlkEval.ValidateData(VDB))
    OK=false;
#endif

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
#if (WITHBLOCKEVALUATOR)
  CFlange * pFlng=m_BlkEval.GetFlange(IOId);
  if (pFlng)
    return pFlng;
#endif

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

// ---------------------------------------------------------------------------

flag MN_Xfer::InitialiseSolution()
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      break;
    case SM_Inline:
    case SM_Buffered:
      if (m_NetworkIsolator)
        m_Accumulator->SetStateAction(IE_SaveState);
      break;
    }

  return 1;
  };

//--------------------------------------------------------------------------

void MN_Xfer::SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn)
  {
  return SetDatums_Node(Pass, List, IOIn, NULL);
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
  if (m_NetworkIsolator)
    m_Accumulator->SetState(RqdState);
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

bool MN_Xfer::PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo)
  {
  if (!FlwNode::DoPropagateNetInfo(Ctrl, IONo, false))
    return false;

  if (!m_NetworkIsolator)
    {
    for (int i=0; i<NoFlwIOs(); i++)
      Nd_Rmt(i)->PropagateNetInfo(Ctrl, IOIONo_Rmt(i));
    }

  return true;
  };

//---------------------------------------------------------------------------

void MN_Xfer::ConfigureJoins()
  {
  if (m_NetworkIsolator)
    {
    for (int i=0; i<NoProcessIOs(); i++)
      {
      if (Nd_Rmt(i)->UserMethod()==LFM_Xfer)
        SetIO_Open(i, 1, false, ESS_Denied);
      else
        SetIO_Open(i, 0, false, ESS_Denied);
      }
    }
  else
    {
    for (int i=0; i<NoProcessIOs(); i++)
      SetIO_Join(i, 0);
    }

  for (int ii=NoProcessIOs(), jj=1+1; ii<NoFlwIOs(); ii++, jj++)
    SetIO_Direct(ii, jj);
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

void MN_Xfer::StartStep()
  {
  MdlNode::StartStep();
  //m_CnStart.AtStart(m_Accu Contents);
  //m_Accumulator->m_MixSet.StartStep();
  //m_SrgCtrl.StartStep();
  if (m_NetworkIsolator)
    m_AccCalc->Start(0, *m_Accumulator);
  }

//--------------------------------------------------------------------------
/*#F:This determines the pressure of the contents and the pressure at each
inlet and outlet of the surge unit.*/
void MN_Xfer::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      const bool OldCode = false;
      if (OldCode)
        {
        if (NoProcessJoins()>=1)
          EvalPBJoinPressure(0, &m_PBPCtrl0, m_QFeed());//&QProd());
        //if (NoProcessJoins()>=2)
        //  EvalPBJoinPressure(1);
        }
      else
        {
        if (NoProcessJoins()>=1)
          EvalPBJoinPressure(0, &m_PBPCtrl0, m_QProd());
        if (NoProcessJoins()>=2)
          EvalPBJoinPressure(1);
        }
      break;
      }
    //case SM_Inline:
    case NM_Dynamic:
      MdlNode::EvalJoinPressures(JoinMask);
      break;
    }
  };
// -------------------------------------------------------------------------

flag MN_Xfer::ChangeMode(dword NewMode, dword OldMode)
  {
  if (NewMode==SM_Direct && !GSM.Enabled())
    GSM.Open();

  return True;
  };

// --------------------------------------------------------------------------

flag MN_Xfer::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  if (m_Leak.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;

  return IOFB(IONo, FE)->EvaluateFlwEqn(Task, pProps, true, false, 1.0, &IOFBFlng_Rmt(IONo)->PhD(), NULL);
  };

// --------------------------------------------------------------------------

void MN_Xfer::EvalProductsInit(EvalProductsInitTasks Task)
  {
  EvalProductsInit_Node(Task, 0x0ffffff);
  }

// --------------------------------------------------------------------------

//void MN_Surge::EvalProductsInit(EvalProductsInitTasks Task)
//  {
//  //EvalProductsInit_SurgeLevel(Task, Contents, First64IOIds);
//  //EvalProductsInit_Node(FwdEval);
//  }

//--------------------------------------------------------------------------
/*#F:This determines what material is leaving through each outlet,
and rate of change of the contents resulting from the flow, of material, out of each outlet
of the surge unit.*/
//void MN_Surge::EvalProductsSurge(CNodeEvalIndex & NEI)
//  {
//  ASSERT(NetDynamicMethod());
//  if (!InlineIntegral())
//    {
//    dword IOMsk=First64IOIds;
//    SigmaQInPMin(QFeed(), som_ALL, IOMsk);
//    if (0)
//      VLE.QPFlash(QFeed(), m_Accumulator->Press(), 0.0, VLEF_Null);
//    EvalProducts_SurgeLevel(ContentHgtOrd, &QFeed, &m_SrgCtrl);
//    //EvalProducts_SurgeLevel(Contents, QFeed, IOMsk, PMU());
//    }
//  }

// --------------------------------------------------------------------------

void MN_Xfer::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (NoProcessJoins()>0)
    {
    switch (SolveMethod())
      {
      case SM_Direct:
        if (NoProcessJoins()>0)
#if (WITHBLOCKEVALUATOR)
          Xfer_EvalProducts(0, Joins[0].Pressure(), &m_QFeed, &m_QProd, m_RB(), GSM(), EHX(), &m_PBPCtrl0, &m_BlkEval);
#else
          Xfer_EvalProducts(0, Joins[0].Pressure(), &m_QFeed, &m_QProd, m_RB(), GSM(), EHX(), &m_PBPCtrl0);
#endif
        break;
      case SM_Inline:
      case SM_Buffered:
        {
        if (NoProcessJoins()>=1)
#if (WITHBLOCKEVALUATOR)
          Xfer_EvalProducts(0, Joins[0].Pressure(), &m_QFeed, &m_QProd, m_RB(), GSM(), EHX(), NULL, &m_BlkEval);
#else
          Xfer_EvalProducts(0, Joins[0].Pressure(), &m_QFeed, &m_QProd, m_RB(), GSM(), EHX());
#endif
//        for (int j=0; j<NoProcessJoins(); j++)
//#if (WITHBLOCKEVALUATOR)
//          Xfer_EvalProducts(j, Joins[j].Pressure(), &m_QFeed, &m_QProd, m_RB(), GSM(), EHX(), NULL, &m_BlkEval);
//#else
//          Xfer_EvalProducts(j, Joins[j].Pressure(), &m_QFeed, &m_QProd, m_RB(), GSM(), EHX());
//#endif
        }
        break;
      }                
    }
  };

//--------------------------------------------------------------------------

void MN_Xfer::EvalIntegral(CNodeEvalIndex & NEI)
  {
  //dbgpln("EvalIntegral - %s %s %s %s", InlineIntegral()?"Inline":"      ", IntegralDone()?"IntDone":"       ", GetActiveHold()?"Hold":"    ", Tag());
  if (m_NetworkIsolator)
    {

    //flag VLEOn=0 && VLE.Enabled();
    dword IOMsk=First64IOIds;
    //StkSpConduit QPF("QPF", chLINEID(), this);

    m_Accumulator->ZeroDeriv();
#if dbgMSurge
    if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
      {
      dbgp("           %*s",151-24,"");
      if (dbgODEOperateSpc())
        for (int s=0; s<SVValueCount(); s++)
          dbgp(" %12.12s", SVImg(s).Tag());
      dbgpln(" %s.Contents", FullObjTag());
      dbgp("InI:---  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f",
        m_CnStart.DTemp(Contents), m_Accumulator->Temp(),
        m_CnStart.DPress(Contents), m_Accumulator->Press(),
        m_CnStart.DMass(Contents), m_Accumulator->Mass(),
        m_CnStart.DLevel(Contents)*100, m_Accumulator->Level()*100, FullObjTag());
      if (dbgODEOperateSpc())
        for (int s=0; s<SVValueCount(); s++)
          dbgp(" %12.5g", m_Accumulator->MArray()[s]);
      dbgpln(" %s.Contents", FullObjTag());
      }
#endif
    for (int i = 0; i < NoProcessIOs(); i++)
      {
      SpConduit * Cd;
      //if (VLEOn && IO_In(i))
      //  {
      //  QPF->QSetF(*IOConduit(i), som_ALL, 1.0);
      //  VLE.QPFlash(QPF(), m_Accumulator->Press(), 0.0, VLEF_Null);
      //  Cd=&QPF();
      //  }
      //else
      Cd=IOConduit(i);
      //double FracAbove=(IOAbsFracHgt_Term(i, Contents)-m_Accumulator->Level())/GTZ(IOAperture(i));
      //FracAbove=Range(0.0, FracAbove, 1.0);
      double EntR=1.0;//FracAbove*IOEntrainRateA(i)*Cd->QMass(som_SL)+
      //(1.0-FracAbove)*IOEntrainRateB(i)*Cd->QMass(som_Gas);
      m_Accumulator->AddDeriv(*Cd, IOSign(i), EntR);
#if dbgMSurge
      if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
        {
        dbgp("dM :IO   > Sgn:%2i EntR:%10.4f CdQm:%10.4f %*s",IOSign(i),EntR,Cd->QMass(),151-39-24,"");
        if (dbgODEOperateSpc())
          for (int s=0; s<SVValueCount(); s++)
            dbgp(" %12.5g", m_Accumulator->pModel->m_DC.m_pFd->m_M[s]*ICGetTimeInc());
        dbgpln(" %s", Nd_Rmt(i)->FullObjTag());
        dbgp("         < %*s",151-24,"");
        if (dbgODEOperateSpc())
          for (int s=0; s<SVValueCount(); s++)
            dbgp(" %12.5g", m_Accumulator->pModel->m_DC.m_pPr->m_M[s]*ICGetTimeInc());
        dbgpln(" %s %s", IOQmEst_In(i)?"IN":"", IOQmEst_Out(i)?"Out":"");
        }
#endif
      }

    //dbgpln("EvalIntegral - AddInflow");
    //m_Accumulator->AddInFlowDeriv();

    //if (m_Accumulator->SpillExists())
    //  {
    //  //if (m_Accumulator->Spill.Cd.QMass()>0)
    //  //  dbgpln("EvalIntegral - AddDeriv Overflow: %14.3f %s", m_Accumulator->Spill.Cd.QMass(), FullObjTag());
    //  m_Accumulator->AddDeriv(m_Accumulator->Spill.Cd, -1.0);
    //  }
    //if (m_Accumulator->VentExists())
    //  {
    //  //if (m_Accumulator->Vent.Cd.QMass()>0)
    //  //  dbgpln("EvalIntegral - AddDeriv Vent    : %14.3f %s", m_Accumulator->Vent.Cd.QMass(), FullObjTag());
    //  m_Accumulator->AddDeriv(m_Accumulator->Vent.Cd, -1.0);
    //  }
#if dbgMSurge
    if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
      {
      dbgp("dM :Ovr  > %*s",151-24,"");
      if (dbgODEOperateSpc())
        for (int s=0; s<SVValueCount(); s++)
          dbgp(" %12.5g", m_Accumulator->pModel->m_DC.m_pFd->m_M[s]*ICGetTimeInc());
      dbgpln(" %s.Contents", FullObjTag());
      dbgp("         < %*s",151-24,"");
      if (dbgODEOperateSpc())
        for (int s=0; s<SVValueCount(); s++)
          dbgp(" %12.5g", m_Accumulator->pModel->m_DC.m_pPr->m_M[s]*ICGetTimeInc());
      dbgpln("");
      }
#endif

    m_Accumulator->pModel->m_DC.Finalise(m_Accumulator->pModel, ICGetTimeInc());
    //m_Accumulator->m_DC.Finalise(this, ICGetTimeInc());

#if dbgMSurge
    if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
      {
      dbgp("dM :Final> %*s",151-24,"");
      if (dbgODEOperateSpc())
        for (int s=0; s<SVValueCount(); s++)
          dbgp(" %12.5g", m_Accumulator->pModel->m_DC.m_pFd->m_M[s]*ICGetTimeInc());
      dbgpln(" %s.Contents", FullObjTag());
      dbgp("         < %*s",151-24,"");
      if (dbgODEOperateSpc())
        for (int s=0; s<SVValueCount(); s++)
          dbgp(" %12.5g", m_Accumulator->pModel->m_DC.m_pPr->m_M[s]*ICGetTimeInc());
      dbgpln("");
      dbgp("         = %*s",151-24,"");
      if (dbgODEOperateSpc())
        for (int s=0; s<SVValueCount(); s++)
          dbgp(" %12.5g", (m_Accumulator->pModel->m_DC.m_pFd->m_M[s]-m_Accumulator->pModel->m_DC.m_pPr->m_M[s])*ICGetTimeInc());
      dbgpln("");
      }
#endif

    if (!GetActiveHold())
      {
      m_Accumulator->ApplyDerivs(ICGetTimeInc(), true);

      //VLE.QVFlash(Contents, 0.0, VLEF_Null);
      }

    //#if dbgMSurge
    //if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
    //  {
    //  dbgp("InI:End  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f",
    //    m_CnStart.DTemp(Contents), m_Accumulator->Temp(),
    //    m_CnStart.DPress(Contents), m_Accumulator->Press(),
    //    m_CnStart.DMass(Contents), m_Accumulator->Mass(),
    //    m_CnStart.DLevel(Contents)*100, m_Accumulator->Level()*100);
    //  if (dbgODEOperateSpc())
    //    for (int s=0; s<SVValueCount(); s++)
    //      dbgp(" %12.5g", m_Accumulator->MArray()[s]);
    //  dbgpln(" %s.Contents", FullObjTag());
    //  }
    //#endif
    }
  SetIntegralDone(true);
  }

//--------------------------------------------------------------------------

void MN_Xfer::ConvergeStates(CConvergeStateBlk &CSB)
  {
  //ASSERT(NetDynamicMethod());

  //if (RB())
  //  {
  //  #if dbgMSurge
  //  if (dbgEvalConverge())
  //    dbgpln("MN_Surge::ConvergeStates - RB %s", FullObjTag());
  //  #endif
  //  m_Accumulator->SetMeanResTimeCalcsReqd(true);
  //  RB.ConvergeStates(CSB, Contents, m_Accumulator->MeanResTimes());
  //  #if dbgMSurge
  //  if (dbgEvalConverge())
  //    dbgpln("-------------------------------------- ");
  //  #endif
  //  };

  //if (!IntegralDone() && !GetActiveHold())
  //  VLE.QVFlash(Contents, 0.0, VLEF_Null);//, NULL, NULL, NULL);

  //m_EqThermals.ConvergeStates(CSB);
  }

//--------------------------------------------------------------------------

/*#F:This is called once, after the iteration process and allows the discreet
manipulation of the contents contained by the surge unit.*/

void MN_Xfer ::EvalDiscrete()
  {
  MdlNode::EvalDiscrete();
  if (GetActiveHold())
    return;
  //dbgpln("EvalDiscrete - %s %s", GetActiveHold()?"HOLD":"", Tag());

  //if (m_Accumulator->MeanResTimeCalcsReqd())
  //  {
  //  SpConduit &Fd=*GetWrkConduit("CSFd", chLINEID());
  //  SpConduit &Pr=*GetWrkConduit("CSPr", chLINEID());
  //  SigmaQInPMin(Fd, som_ALL, First64IOIds);
  //  SigmaQOut(Pr, som_ALL, First64IOIds);
  //  m_Accumulator->CompleteMeanResTimeCalcs(Fd, Pr);
  //  }

  //if (m_Accumulator->SpillExists())
  //  {
  //  SpDirectCd & O=m_Accumulator->OverFlow;
  //  if (O.QMass()>1e-20)
  //    dbgpln("EvalDiscrete - AddTotal: %14.3f %s", O.QMass(), Tag());
  //  O.SetTotal(O.Total()+O.QMass()*ICGetTimeInc());
  //  }
  //if (m_Accumulator->VentExists())
  //  {
  //  SpDirectCd & O=m_Accumulator->GasVent();
  //  O.SetTotal(O.Total()+O.QMass()*ICGetTimeInc());
  //  }

  if (m_NetworkIsolator)
    {
    // Qualities
    for (int i = 0;  (i<NoProcessIOs()); i++)
      if (IO_In(i))
        m_Accumulator->AddDiscrete(*IOConduit(i), IOSign(i));

    m_AccCalc->Complete(0, *m_Accumulator, ICGetTimeInc());
    }

  m_Leak.EvalDiscrete();
  }

//--------------------------------------------------------------------------

//void MN_Xfer::EvalDiscrete() 
//  {
//  MdlNode::EvalDiscrete();
//  if (GetActiveHold())
//    return;
//
//  m_Leak.EvalDiscrete();
//  };

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
  if (!GSM.Enabled() || m_NetworkIsolator)
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
  if (GetStatusAsStopped())
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

//--------------------------------------------------------------------------

flag MN_Xfer::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tAccumulator Content > 10 Residence Times";   return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
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

//--------------------------------------------------------------------------
