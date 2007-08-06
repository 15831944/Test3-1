//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"

#include "sc_defs.h"
#define  __PIPETERM_CPP
#include "pipeterm.h"
#include "scd_wm.h"
//#include "optoff.h"

//==========================================================================

static double Drw_PipeTerm[] = { DD_Poly, -3,3, 3,0, -3,-3, -3,3, 
                                 DD_TagPos, 0, -6.5,
                                 DD_End };

static IOAreaRec PipeTermIOAreaList[] =
  {{"Inputof Stack",       "In"  , 0, LIO_In0 ,  nc_MLnk, 0, 1, IOPipeEntry},
   {NULL}};

//==========================================================================

IMPLEMENT_MODELUNIT(CPipeTerm, "PipeTerm", "1",Drw_PipeTerm, "Feed", "T", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                    "Process:Unit:Pipe_Terminator",
                   "Pipe Terminator")

//==========================================================================

CPipeTerm::CPipeTerm (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  //Contents("Content", this, TOA_Embedded),
  m_Flows("Flows", this, TOA_Embedded),
  m_VPB("VL_Basic", this, NULL),
  m_FRB("Reg", FBRShow_All, FBReg_Pi|FBReg_Qm|FBReg_Qv|FBReg_QvStep, FBReg_Off)
  {
  SetAllowedModes(false, NM_All|SM_Direct/*|SM_Inline|SM_Buffered*/|HM_All|LFM_Simple|LFM_Linear|LFM_Full);// "Transfer", "Inline", NULL);
  //SetAllowedFlowModes();

  AttachClassInfo(nc_MSrcSnk,NULL);
  //FEP.AssignFlwEqnGroup(PipeGroup, PipeGroup.Default(), this);
  AttachIOAreas(PipeTermIOAreaList, &ValveGroup, &ValveGroup);
  //Initialise();
  m_dHead=0;
  m_dPOut=Std_P;
  
  };

// -------------------------------------------------------------------------

CPipeTerm::~CPipeTerm ()
  {
  };

// -------------------------------------------------------------------------

void CPipeTerm::ResetData(flag Complete)
  {
  for (int i=0; i<NoFlwIOs(); i++)
    for (int j=0; j<NIOFBs(i); j++)
      IOFB(i,j)->ResetData(Complete);
  }

//--------------------------------------------------------------------------

void CPipeTerm::BuildDataDefn(DataDefnBlk &DDB)
  {
  DDB.BeginStruct(this, NULL, NULL, DDB_NoPage);

  DDB.Text   ("Flow Conditions");
  DDB.Double ("Mass_Flow",    "Qm",       DC_Qm,   "kg/s",   xidQm,         this, isResult|noFile|noSnap);
  DDB.Double ("Vol_Flow",     "Qv",       DC_Qv,   "L/s",    xidQv,         this, isResult|noFile|noSnap);
  DDB.Double ("NVol_Flow",    "NQv",      DC_NQv,  "NL/s",   xidNQv,        this, isResult|noFile|noSnap);
  DDB.Double ("Temperature",  "T",        DC_T,    "C",      xidTemp,       this, isResult|noFile|noSnap);
  DDB.Double ("Density",      "Rho",      DC_Rho,  "kg/m^3", xidRho,        this, isResult|noFile|noSnap);
  DDB.Double ("NDensity",     "NRho",     DC_Rho,  "kg/m^3", xidNRho,       this, isResult|noFile|noSnap);
  DDB.Double ("PressureIn",   "PIn",      DC_P,    "kPag",   xidPIn,        this, isResult|noFile|noSnap);
  DDB.Double ("PressureOut",  "POut",     DC_P,    "kPag",   xidPOut,       this, isResult|noFile|noSnap);

  DDB.Visibility(NSHM_All);
  flag ParmOK=true;
  
  DDB.Visibility();
  DDB.Text  ("");
  DDB.Double("PressureOutRqd","POutRqd",  DC_P,    "kPag",   &m_dPOut,       this, isParm);
  DDB.Double("Head",          "",         DC_L,    "m",      &m_dHead,       this, isParm);

  BuildDataDefnElevation(DDB);
  
  DDB.Text  ("");
  m_FRB.BuildDataDefn(DDB, this, true);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.Text  ("");
  m_VPB.BuildDataDefn(DDB, this, "Posn", 1);
  DDB.Visibility(NSHM_All);

  if (NoFlwIOs()>0 && !DDB.ForFileSnpScn())
    IOFB(0,0)->BuildDataDefn(FBDDOpt_WithAll, DDB, this, "CtrlEqn", 2);

  DDB.Visibility();
  DDB.Object(&m_Flows, this, NULL, NULL, DDB_RqdPage);

  DDB.Text("");
  DDB.EndStruct();
  }

//// --------------------------------------------------------------------------
//
//flag CPipeTerm::DataXchg(DataChangeBlk & DCB)
//  {
//  if (NIOs>=1 && DCB.dwUserInfo==1 && IOFB_Rmt(0,0)->DataXchg(DCB))
//    return 1;
//  if (NIOs>=2 && DCB.dwUserInfo==2 && IOFB_Rmt(1,0)->DataXchg(DCB))
//    return 1;
//  if (MN_Lnk::DataXchg(DCB))
//    return 1;
//	if (RB.DataXchg(DCB))
//    return 1;
//  
//  return 0;
//  }

// -------------------------------------------------------------------------

flag CPipeTerm::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
//    case xidSSMode:
//      if (DCB.rB)
//        {
//        SSMode=*DCB.rB;
//        if (SSMode & SSM_ConstP)
//          m_iCFWhat &= ~CFWhatFlow;
//        }
//      DCB.B=SSMode;
//      return 1;
//    case xidQmRqd:
//      if (DCB.rD)
//        Contents.SetQmRqdTo(GEZ(*DCB.rD), true);
//      DCB.D=Contents.QmRqd();
//      return 1;
//    case xidQvRqd:
//      if (DCB.rD)
//        Contents.SetQvRqdTo(GEZ(*DCB.rD), !DCB.ForFiling());// CNM true); recovery of QAL
//      DCB.D=Contents.QvRqd();
//      return 1;
//    case xidNQvRqd:
//      if (DCB.rD)
//        Contents.SetNQvRqdTo(GEZ(*DCB.rD), !DCB.ForFiling());// CNM true); recovery of QAL
//      DCB.D=Contents.NQvRqd();
//      return 1;
//    case xidTRqd:
//      if (DCB.rD)
//        Contents.SetTRqdTo(Range(CDB.MinT, *DCB.rD, CDB.MaxT));
//      DCB.D=Contents.TRqd();
//      return 1;
//    case xidPRqd:
//      if (DCB.rD)
//        Contents.SetPRqdTo(Max(1.0, *DCB.rD));
////        Contents.SetPRqdTo(Range(CDB.MinP, Max(1.0, *DCB.rD), CDB.MaxP));
//      DCB.D=Contents.PRqd();
//      return 1;
    case xidRho:
      DCB.D=m_Flows.Rho(som_ALL);
      return 1;
//    case xidLevel:
//      DCB.D=dNAN;
//      return 1;
//    case xidPMean:
//      if (DCB.rD)
//        m_Flows.SetPress(Max(1.0, *DCB.rD));
//      DCB.D=m_Flows.Press();
//      return 1;
//    case xidSolFrac:
//      DCB.D=m_Flows.MassFrac(som_Sol);
//      return 1;
//    case xidLiqFrac:
//      DCB.D=m_Flows.MassFrac(som_Liq);
//      return 1;
//    case xidVapFrac:
//      DCB.D=m_Flows.MassFrac(som_Gas);//Frac();
//      return 1;
//    case xidMoleVapFrac:
//      DCB.D=m_Flows.MoleFrac(som_Gas);
//      return 1;
    case xidTemp:
      if (DCB.rD)
        m_Flows.SetTemp(*DCB.rD);
      DCB.D=m_Flows.Temp();//som_ALL);
      return 1;
    }

  if (MdlNode::DataXchg(DCB))
    return 1;
  if (m_FRB.DataXchg(DCB, this))
    return 1;

  switch (DCB.dwUserInfo)
    {
    case 1: 
      if (m_VPB.DataXchg(DCB)) 
        return 1;
      break;
    case 2: 
      if (NoFlwIOs()>0 && !DCB.ForFileSnpScn())
        if (IOFB(0,0)->DataXchg(DCB)) 
          return 1;
      break;
    }

  return 0;
  }

// -------------------------------------------------------------------------

flag CPipeTerm::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);
  if (!m_FRB.ValidateData(VDB, this))
    OK=false;

  m_dHead=GEZ(m_dHead);

  if(NoFlwIOs()>=1 && !IOFB(0,0)->ValidateData(VDB))
    OK=false;

  EvalState();

  //if (AllowMineServe())
  //  SetCI(1, m_FRB.What()== FBReg_Pi || m_FRB.What()==FBReg_Po);

  if (!OK)
	  { int xxx=0; }
  return OK;
  };

// -------------------------------------------------------------------------

flag CPipeTerm::PreStartCheck()
  {
  return MdlNode::PreStartCheck();
  };

//--------------------------------------------------------------------------

flag CPipeTerm::GetModelAction(CMdlActionArray & Acts)
  {
  //CMdlAction {MAT_NULL, MAT_State, MAT_Value};
  CMdlAction M0(0, MAT_State, !m_VPB.On(), "Open", 1);
  CMdlAction M1(1, MAT_State, m_VPB.On(),  "Close", 0);
  CMdlAction M2(2, MAT_Value, m_VPB.On(),  "Manual Posn (%)", true,
                  m_VPB.ManualPosition(this)*100, 0.0, 100.0,
                  m_VPB.ActualPosition(this, &m_FRB)*100);

  Acts.SetSize(0);
  Acts.SetAtGrow(0, M0);
  Acts.SetAtGrow(1, M1);
  Acts.SetAtGrow(2, M2);
  Acts.SetAtGrow(3, CMdlAction(3, MAT_Switch));
  return True;
  };

//--------------------------------------------------------------------------

flag CPipeTerm::SetModelAction(CMdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
      m_VPB.SetOn(Act.iValue!=0);
      break;
    case 1:
      m_VPB.SetOn(Act.iValue!=0);
      break;
    case 2:
      m_VPB.SetManualPosition(Act.dValue*0.01);
      break;
    case 3:
      m_VPB.SetOn(!m_VPB.On());
      break;
    }
  return true;
  };

//---------------------------------------------------------------------------

void CPipeTerm::PostConnect(int IONo)
  {
//IOFB(IONo,0)->AssignFlwEqnGroup(ValveGroup, ValveGroup.Default(), this);
  MdlNode::PostConnect(IONo);
  };

//--------------------------------------------------------------------------

long CPipeTerm::NodeFlwTask(NodeFlwTasks Task)
  {
  if (Task==NFT_PBQueryRemove)
    {
    //if (fCrossConnected || NoFlwIOs()>1)
    //  return 0;
    //if (NoFlwIOs()==0 || IO_In(0))
    //  return 0;
    //
    //long nIn=0;
    //long nOut=0;
    //for (int i=0; i<NoFlwIOs(); i++)
    //  {
    //  if (IO_In(i))
    //    nIn++;
    //  else if (IO_Out(i))
    //    nOut++;
    //  }
    //    
    //if (nIn>0 || nOut>0)
    //  return 0;
    //else
    //  return 1;
    
    return 0;
    }
  else
    return MdlNode::NodeFlwTask(Task);
  }

//--------------------------------------------------------------------------

void CPipeTerm::SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn)
  {
  SetDatums_Node(Pass, List, IOIn, NULL);
  };

//--------------------------------------------------------------------------

flag CPipeTerm::Set_Sizes()
  {
  return true;
  };

//--------------------------------------------------------------------------

void CPipeTerm::StartSolution()
  {
  }

//--------------------------------------------------------------------------

void CPipeTerm::StartStep()
  {
  //dTime=ICGetTimeInc();
  }

//---------------------------------------------------------------------------

void CPipeTerm::ConfigureJoins()
  {
  for (int i=0; i<NoProcessIOs(); i++)
    SetIO_Open(i, 0, false, ESS_Denied);
  };

//--------------------------------------------------------------------------

void CPipeTerm::EvalJoinPressures(long JoinMask)
  {
  for (int i = 0; i < NoFlwIOs(); i++)
    {
    Set_IOP_Self(i,m_dPOut);
    if (NetMethod()==NM_Probal)
      {
      Set_IOP_Flng(i,m_dPOut);
      Clr_RhoH_Self(i);
      }
    else
      Set_IOP_RhoH_Self(i, m_dPOut, m_Flows.Rho(som_SL), m_dHead);
    }
  };

//--------------------------------------------------------------------------

flag CPipeTerm::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  if (IONo>0 || FE>0)
    return False;

  if (Task!=FET_GetMode)
    IOFB(0,FE)->SetRegulator(m_FRB.What());

  return IOFB(0,FE)->EvaluateFlwEqn(Task, pProps, m_VPB.On()!=0, false, m_VPB.ActualPosition(this, &m_FRB), &IOFBFlng_Rmt(0)->PhD(), NULL);
  };

//--------------------------------------------------------------------------

void CPipeTerm::EvalState()
  {
  };

//--------------------------------------------------------------------------

void CPipeTerm::EvalCtrlActions(eScdCtrlTasks Tasks)
  {
    //if (NoCtrlIOs()>0)
    //  {
    //  int io=CIOWithId_Self(CIOId_Pos);
    //  if (io>=0)
    //    m_VPB.SetReqdPosition(CIO_Value(io));
    //  }

  //if (!m_FRB.On())
  m_VPB.EvalCtrlActions(this);
  }

//--------------------------------------------------------------------------

void CPipeTerm::ClosureInfo()
  {
  int Dirn=0;
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    for (int i=0;i<NoFlwIOs(); i++)
      {
      SpConduit &C=*IOConduit(i);
      if (IO_In(i))
        {
        CI.m_HfLoss += C.totHf();
        CI.m_HsLoss += C.totHs();
        CI.m_MassLoss  += C.QMass();
        }
      else if (IO_Out(i))
        {
        CI.m_HfGain += C.totHf();
        CI.m_HsGain += C.totHs();
        CI.m_MassGain  += C.QMass();
        }
      }
    }
  };

// --------------------------------------------------------------------------

void CPipeTerm::EvalProductsInit(EvalProductsInitTasks Task)
  {
  for (int i=0;i<NoFlwIOs(); i++)
    if (IO_Out(i) || IO_Zero(i))
      EvalProductsInit_Source(Task, i, IOConduit(i)->QMass(som_SL), IOConduit(i)->QMass(som_Gas));
  }

//--------------------------------------------------------------------------

void CPipeTerm::EvalProducts(CNodeEvalIndex & NEI)
  {

  int i, nOut=0, nIn=0;
  switch (NetMethod())
    {
    case NM_Probal:
    case NM_Dynamic:
      for (i=0;i<NoFlwIOs(); i++)
        if (IO_Out(i) || IO_Zero(i))
          {
          SpConduit& Q = *IOConduit(i);
          //Q.QZero();
          //m_Flows.QZero();
          Q.QSetM(m_Flows, som_ALL, IOQmEst_Out(i), IOP_Self(i));
          }
        else if (IO_In(i)) // Defined Input
          {
          m_Flows.QSetF(*IOConduit(i), som_ALL, 1.0, IOP_Self(i));
          }
      break;
    }
  }

//--------------------------------------------------------------------------

void CPipeTerm::EvalDerivs(CNodeEvalIndex & NEI)
  {
  };

//--------------------------------------------------------------------------

void CPipeTerm::EvalDiscrete()
  {
  m_VPB.EvalDiscrete(this);
  }

//--------------------------------------------------------------------------

dword  CPipeTerm::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (NoFlwIOs()>0)
    {
    Status |= ((m_VPB.On() && m_VPB.ActualPosition(this, &m_FRB)>0.0) ? FNS_On : FNS_Off);
  
    Status |= (IOConduit(0)->QMass()>gs_DisplayZeroFlow ? FNS_UFlw : FNS_UNoFlw);

    int HasFlw=0;
    double TFlw=0.0;
    for (int i=0; i<NoFlwIOs(); i++)
      {
      TFlw+=IOQm_In(i);
      if (IOConduit(i)->QMass()>gs_DisplayZeroFlow)
        HasFlw=1;
      }
    Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
    if (NoFlwIOs()==1)
      {
      if (TFlw>1.0e-6)
        Status |= FNS_IsSnk;
      else if (TFlw<-1.0e-6)
        Status |= FNS_IsSrc;
      }
    }

  return Status;
  };

//--------------------------------------------------------------------------

flag CPipeTerm::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="E\tBad Regulator Configuration"; return 1;
    default:                                               
      return MdlNode::CIStrng(No, pS);
    }
  };

//==========================================================================

