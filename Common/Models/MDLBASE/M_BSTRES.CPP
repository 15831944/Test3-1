//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __M_BSTRES_CPP
#include "m_bstres.h"

//#include "optoff.h"

//===========================================================================

//===========================================================================

MN_BstRes::MN_BstRes(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  m_VLE(this, VLEF_QPFlash),
  m_FEP(NULL, FBOpt_HasSimpleRes, 1.0),
  m_FTB(this, &m_FEP),
  m_LeakI(eDIO_Leak, this, true, false, "Inlet_Leak", IOId_2IOLeakI, IOId_AreaLeakI),
  m_LeakO(eDIO_Leak, this, true, false, "Outlet_Leak", IOId_2IOLeakO, IOId_AreaLeakO)
  {

  bOnLine=1;
  bIsentropic=0;
  iPressMethod=BRPM_None;
  //dTempDrop=dNAN;
  dPressBoost_Rqd=0.0;
  dPout_Rqd=dNAN;
  dTout_Rqd=dNAN;
  dDuty_Rqd=0.0;

  dTout=Std_T; //actual
  dPout=Std_P; //actual
  dDuty=0.0; //actual

  AttachClassInfo(nc_MBst, NULL, &NullFlwGroup);

  m_FEP.SetFlowModeSrc(this, false);
  }

// -------------------------------------------------------------------------

void MN_BstRes::FinaliseConstruction() 
  {

  Init_NLinks(1);

  SetAllowedSolveModes(SM_Direct/*|SM_Inline|SM_Buffered*/);// "Transfer", "Inline", NULL);
  SetAllowedFlowModes();

  if (GlblDynFlowMode())
    {
    if (OrigPrjFileVerNo()<=82)
      SetFlowModeReqd(LFM_Xfer);
    else
      SetFlowModeReqd(LFM_Simple);// LFM_Xfer);
    }
  else
    SetFlowModeReqd(LFM_Full);
  // Must do this here because m_FEP.Group() only gets set in derived class constructor
  m_Links[0].FB.AssignFlwEqnGroup(m_FEP.Group(), m_FEP.Group().Default(), this);
  m_Links[0].FB.SetParentFlwEqn(&m_FEP);
  m_Links[0].FB.SetFlowModeSrc(this, false);
  };

// -------------------------------------------------------------------------

void MN_BstRes::ResetData(flag Complete)
  {
  //dPout=Std_P;
  //dDuty=0.0;

  for (int i=0; i<NoFlwIOs(); i++)
    for (int j=0; j<NIOFBs(0); j++)
      IOFB(i,j)->ResetData(Complete);
  }

//--------------------------------------------------------------------------

flag MN_BstRes::DataXchg(DataChangeBlk & DCB)
  {
  if (m_VLE.DataXchg(DCB))
    return 1;
  if (m_FTB.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==1 && m_LeakI.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==2 && m_LeakO.DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidPressMethod:
      if (DCB.rB)
        {
        iPressMethod=*DCB.rB;
        switch (iPressMethod)
          {
          case BRPM_None : break;
          case BRPM_Fixed: if (!Valid(dPout_Rqd)) dPout_Rqd=dPout; break;
          case BRPM_Boost: break;
          case BRPM_Drop : break;
          case BRPM_Atmos: break;
          case BRPM_SatP : break; 
          }
        }
      DCB.B=iPressMethod;
      return 1;
    case xidPBoostRqd:
      if (DCB.rD)
        dPressBoost_Rqd=*DCB.rD;
      DCB.D=dPressBoost_Rqd;
      return 1;
    case xidPDropRqd:
      if (DCB.rD)
        dPressBoost_Rqd=((*DCB.rD)*-1.0);
      DCB.D=(dPressBoost_Rqd*-1.0);
      return 1;
    case xidPoutRqd:
      if (DCB.rD)
        dPout_Rqd=*DCB.rD;
      DCB.D=dPout_Rqd;
      return 1;
    case xidToutRqd:
      if (DCB.rD)
        dTout_Rqd=*DCB.rD;
      DCB.D=dTout_Rqd;
      return 1;
    case xidDutyRqd:
      if (DCB.rD)
        dDuty_Rqd=*DCB.rD;
      DCB.D=dDuty_Rqd;
      return 1;
    case xidDuty:
      if (DCB.rD)
        dDuty=*DCB.rD;
      DCB.D=dDuty;
      return 1;
    case xidQmEstSrc:
      DCB.pC=GetQmEstSrc2IO() ? GetQmEstSrc2IO()->FullObjTag() : "";
      return true;
    case xidQmEstType:
      DCB.pC=(LPTSTR)GetQmEstType2IO();
      return true;
    case xidQmEst:
      DCB.D=GetQmEst2IO(DCB.rD);
      return 1;
    case xidQm:
      DCB.D=GetQm2IO(DCB.rD);
      return 1;
    };

  return MdlNode::DataXchg(DCB);
  }

//--------------------------------------------------------------------------

flag MN_BstRes::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);

  if (!m_FTB.ValidateData(VDB))
    OK=0;
  if (!m_LeakI.ValidateData(VDB))
    OK=false;
  if (!m_LeakO.ValidateData(VDB))
    OK=false;
  return OK;
  };

//--------------------------------------------------------------------------

Z_States MN_BstRes::SetDatums(int Pass, int IOIn, double Zi, Z_States Z_State_Src)
  {
  return SetDatums_Node(Pass, NULL, IOIn, Zi, Z_State_Src);
  };

//--------------------------------------------------------------------------

flag MN_BstRes::Set_Sizes()
  {
  return True;
  };

//--------------------------------------------------------------------------

void MN_BstRes::StartSolution()
  {
  if (m_LeakI.ConnectError || m_LeakO.ConnectError)
    {
    LogError(FullObjTag(), 0, "Stopping Leak(s) not connected");
    ExecObj()->XStop();
    }
  }

//--------------------------------------------------------------------------

CFlange * MN_BstRes::GetFlange(int IOId)
  {
  switch (IOId)
    {
    case IOId_2IOLeakI: return &m_LeakI.xFlange; 
    case IOId_2IOLeakO: return &m_LeakO.xFlange; 
    }
  return NULL;
  };

//--------------------------------------------------------------------------

void MN_BstRes::PostConnect(int IONo)
  {
  MdlNode::PostConnect(IONo);

  switch (IOId_Self(IONo))
    {
    case IOId_2IOIn   : 
      IOFB(IONo,0)->SetParentFlwEqn(NULL); 
      IOFB(IONo,0)->SetFlowModeSrc(this, false);
      break;//, &m_FEP);  break;
    case IOId_2IOOut  : 
      IOFB(IONo,0)->SetParentFlwEqn(NULL);
      IOFB(IONo,0)->SetFlowModeSrc(this, false);
      break;
    case IOId_2IOLeakI: 
      m_LeakI.PostConnect(IONo);
      break;
    case IOId_2IOLeakO: 
      m_LeakO.PostConnect(IONo);
      break;
    }
  };

//---------------------------------------------------------------------------

void MN_BstRes::PreDisConnect(int IONo)
  {
  switch (IOId_Self(IONo))
    {
    case IOId_2IOLeakI: m_LeakI.PreDisConnect(IONo); break;
    case IOId_2IOLeakO: m_LeakO.PreDisConnect(IONo); break;
    }
  MdlNode::PreDisConnect(IONo);
  }

//---------------------------------------------------------------------------

bool MN_BstRes::PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo)
  {
  if (!FlwNode::DoPropagateNetInfo(Ctrl, IONo, true))
    return false;

  for (int i=0; i<NoFlwIOs(); i++)
    {
    if (i!=IONo)
      Nd_Rmt(i)->PropagateNetInfo(Ctrl, IOIONo_Rmt(i));
    }

  return true;
  };

void MN_BstRes::ConfigureJoins()
  {
  if (NetMethod()==NM_Probal)
    {
    for (int i=0; i<NoProcessIOs(); i++)
      SetIO_Join(i, 0); 
    }
  else
    {
    for (int i=0; i<NoProcessIOs(); i++)
      {
      switch (IOId_Self(i))
        {
        case IOId_2IOIn   : SetIO_Join(i, 0); break;
        case IOId_2IOOut  : SetIO_Join(i, 1); break;
        case IOId_2IOLeakI: SetIO_Join(i, 0); break;
        case IOId_2IOLeakO: SetIO_Join(i, 1); break;
        }
      }
    LinkJoins(0, 0, 1, &m_FEP);

    SetJoin_HasVolume(0, false, true);
    SetJoin_HasVolume(1, false, true);
    }
  }

//---------------------------------------------------------------------------

void MN_BstRes::EvalJoinPressures(long JoinMask)
  {
  if (NoFlwIOs()>0)
    if (NetMethod()==NM_Probal)
      {
      for (int j=0; j<NoJoins(); j++)
        {
        //double Pm=GetPBInputPressure(j);
        //SetPBJoinPressure(j, Pm);
        SetPBJoinPressure(j, dPout);
        }
      }
    else
      MdlNode::EvalJoinPressures(JoinMask);
  }

//---------------------------------------------------------------------------

flag MN_BstRes::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  if (m_LeakI.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  if (m_LeakO.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  if (IONo>0)
    return False;
  /*if (Valid(dPb_Rqd))
    {
    IOFB(0,0)->SetDPb(dPb_Rqd, 0.0);
    }*/
  return True;
  }

// --------------------------------------------------------------------------

void MN_BstRes::EvalProductsInit(EvalProductsInitTasks Task)
  {
  EvalProductsInit_Link(Task);
  }

// --------------------------------------------------------------------------

class CPumpTempFnd : public MRootFinderBase
  {
  public:
    static CToleranceBlock s_Tol;
    CPumpTempFnd(SpConduit &Cond, double RqdP_) : MRootFinderBase("PumpTempFnd", s_Tol), C(Cond) { RqdP=RqdP_; };
    double  Function(double x);
  protected:
    SpConduit & C;
    double RqdP;
  };

CToleranceBlock CPumpTempFnd::s_Tol(TBF_Both, "Pump:FinalTempFnd", 0.0, 1.0e-9);

//vary temperature at rqd pressure to maintain entropy...
double CPumpTempFnd::Function(double x)
  {
  C.SetTempPress(x, RqdP);
  double s = C.totSf();
  return s;
  }

void MN_BstRes::EvalProducts(long JoinMask)
  {
  if (NoFlwIOs()<2)
    return;

  if (!bOnLine && !IO_Zero(0))
    {
    const int In  = TwoIOInIndex();
    const int Out = OtherEnd(In);
    SpConduit & Fi=*IOConduit(In);
    SpConduit & Fo=*IOConduit(Out);
    dDuty = 0.0;
    Fo.QCopy(Fi);
    IOFB(0,0)->SetQm(Sign(IOFB(0,0)->GetQm())*Fo.QMass(som_ALL));
    if (SolveDirectMethod())
      {
      dPout = Fo.Press();
      dTout = Fo.Temp();
      }
    else
      {
      m_FTB.BeginEvalProducts(Fo);
      IOFB(0,0)->EvalProducts(IOP_Flng(In), IOP_Flng(Out), Fo, m_FTB);//);
      m_FTB.EndEvalProducts();
      }
    return;
    }

  if (SolveDirectMethod())
    {
    const int In  = TwoIOInIndex();
    const int Out = OtherEnd(In);
    SpConduit & Fi=*IOConduit(In);
    SpConduit & Fo=*IOConduit(Out);
    if (IO_Zero(0))
      {
      Fi.QSetTraceMass();//QZero();
      Fo.QSetTraceMass();//QZero();
      IOFB(0,0)->SetQm(0.0);
      dDuty = 0.0;
      dPout = Fo.Press();
      dTout = Fo.Temp();
      }
    else
      {
      Fo.QCopy(Fi);
      IOFB(0,0)->SetQm(Sign(IOFB(0,0)->GetQm())*Fo.QMass(som_ALL));
      dPout = Fo.Press();
      switch (iPressMethod)
        {
        case BRPM_None : break;
        case BRPM_Fixed: dPout = Range(1.0, dPout_Rqd, 1.0e15); break;
        case BRPM_Boost: 
        case BRPM_Drop : dPout = Range(1.0, dPout+dPressBoost_Rqd, 1.0e15); break;
        case BRPM_Atmos: dPout = AtmosPress(); break;
        case BRPM_SatP : /*dPout = todo*/ break; 
        }

      if (bIsentropic)
        {//for pump maintain entropy...
        const double TOut = Fo.Temp();
        const double h0 = Fo.totHf();
        const double s0 = Fo.totSf();
        CPumpTempFnd Fnd(Fo, dPout);
        Fnd.SetTarget(s0);
        if (Valid(dTout))
          {
          Fnd.SetEstimate(dTout, -1.0);
          //dTout = dNAN;
          }
        bool OK = 0;
        const double MnX = max(dTout-100.0, ZeroCinK); //WHAT LIMITS SHOULD BE USED???!!!
        const double MxX = dTout+200.0; //WHAT LIMITS SHOULD BE USED???!!!
        int iRet = Fnd.Start(MnX, MxX);
        if (iRet==RF_EstimateOK) //estimate is good, solve not required
          {
          OK=1;
          }
        else
          {
          if (iRet==RF_BadEstimate)
          iRet = Fnd.Start(MnX, MxX); // Restart
          if (iRet==RF_OK)
            if (Fnd.Solve_Brent()==RF_OK)
              {
              OK=1;
              }
          if (iRet==RF_LoLimit)
            {
            //dCalcT = MnX;
            }
          else if (iRet==RF_HiLimit)
            {
            //dCalcT = MxX;
            }
          }
        //SetCI(?, !OK); //Brent solve problem!
        dDuty = Fo.totHf() - h0;
        }
      else
        {
        if (Valid(dTout_Rqd))
          {
          const double h0 = Fo.totHf();
          Fo.SetTemp(dTout_Rqd);
          dDuty = Fo.totHf() - h0;
          }
        else if (fabs(dDuty_Rqd)>1.0e-12)
          {
          const double h0 = Fo.totHf();
          Fo.Set_totHf(h0+dDuty_Rqd);
          dDuty = dDuty_Rqd;
          }
        else
          dDuty = 0.0;
        const double h1 = Fo.totHf();
        Fo.SetPress(dPout);
        Fo.Set_totHf(h1); //restore enthalpy
        }
      m_VLE.QPFlash(Fo, IOP_Flng(Out), 0.0, VLEF_Null);
      dTout = Fo.Temp();
      }
    }
  else
    {//dynamic...
    CLinkRec & L=m_Links[0];

    if (fabs(L.FB.m_Qm)==0)
      {
      SpConduit & Fi=*IOConduit(0);
      SpConduit & Fo=*IOConduit(1);

      if (m_LeakI.Enabled)
        m_LeakI.EvalProducts(Fi, false);
      else
        Fi.QSetTraceMass();
      
      L.Cd.QSetTraceMass();
      m_FTB.BeginEvalProducts(L.Cd);
      L.FB.EvalProducts(L.PB(0).P, L.PB(1).P, L.Cd, m_FTB);
      m_FTB.EndEvalProducts();

      if (m_LeakO.Enabled)
        m_LeakO.EvalProducts(Fo, false);
      else
        Fo.QSetTraceMass();
      dDuty = 0.0;
      }
    else
      {
      int In=(L.FB.m_Qm>0 ? 0:1);
      int Out=OtherEnd(In);

      SpConduit & Fi=*IOConduit(In);
      SpConduit & Fo=*IOConduit(Out);

      Fo.QCopy(Fi);   

      //heat loss/gain for simple heater:
      const double PresO = Fo.Press();
      if (Valid(dTout_Rqd))
        {
        const double h0 = Fo.totHf();
        Fo.SetTemp(dTout_Rqd);
        dDuty = Fo.totHf() - h0;
        const double h1 = Fo.totHf();
        Fo.SetPress(PresO);
        Fo.Set_totHf(h1); //restore enthalpy
        }
      else if (fabs(dDuty_Rqd)>1.0e-12)
        {
        const double h0 = Fo.totHf();
        Fo.Set_totHf(h0+dDuty_Rqd);
        dDuty = dDuty_Rqd;
        const double h1 = Fo.totHf();
        Fo.SetPress(PresO);
        Fo.Set_totHf(h1); //restore enthalpy
        }
      else
        dDuty = 0.0;

      if (In==IOId_2IOIn && m_LeakI.Enabled)
        m_LeakI.EvalProducts(Fo, true);
      if (In==IOId_2IOOut && m_LeakO.Enabled)
        m_LeakO.EvalProducts(Fo, true);

      L.Cd.QCopy(Fo);

      m_FTB.BeginEvalProducts(Fo);
      L.FB.SetQm(Sign(L.FB.GetQm())*Fo.QMass(som_ALL));
      L.FB.EvalProducts(L.PB(In).P, L.PB(Out).P, L.Cd, m_FTB);

      if (m_VLE.Enabled())
        {
        m_FTB.AddVLEBegin();
        m_VLE.QPFlash(Fo, L.PB(Out).P, /*Valid(L.FB.ShaftPower()) ? L.FB.ShaftPower()/0.8 :*/ 0.0, VLEF_Null);
        m_FTB.AddVLEEnd();
        }
      m_FTB.EndEvalProducts();

      if (Out==IOId_2IOOut && m_LeakO.Enabled)
        m_LeakO.EvalProducts(Fo, true);
      if (Out==IOId_2IOIn && m_LeakI.Enabled)
        m_LeakI.EvalProducts(Fo, true);
      }
    }
  }

//--------------------------------------------------------------------------

void MN_BstRes::EvalDiscrete() 
  {
  MdlNode::EvalDiscrete();
  m_LeakI.EvalDiscrete();
  m_LeakO.EvalDiscrete();
  };

//---------------------------------------------------------------------------

void MN_BstRes::ClosureInfo()
  {
  bool InRange=true;
  for (int i=0; i<NoFlwIOs(); i++)
    InRange = InRange && IOConduit(i)->SMFnsInRange();
  FlwNode::SetCI(29, !InRange);

  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    CI.m_PowerIn+=dDuty;
    if (EHX.Enabled())
      CI.m_EHXPowerIn+=EHX.HeatFlow();
    m_FTB.GetClosureInfo(CI, false, false);
    }
  }

//---------------------------------------------------------------------------

dword  MN_BstRes::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (NoFlwIOs()>1)
    {
    if (GetStatusAsStopped())
      {
      Status |= (m_FEP.FlwRqdActive() ? FNS_UFlw : FNS_UNoFlw);
      }
    else
      {
      const int iIn  = (Valid(IOQmEst_In(0)) && IOQmEst_In(0)>0.0 ? 0 : 1);
      Status |= (IOConduit(iIn)->QMass()>UsableMass ? FNS_UFlw : FNS_UNoFlw);
      }
    }
  else if (NoFlwIOs()==1)
    Status|=FNS_Error;

  return Status;
  };

//===========================================================================
