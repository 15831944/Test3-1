//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __VLVIPRV_CPP
#include "vlviprv.h"
#include "scd_wm.h"
#include "dbgmngr.h"

//#include "optoff.h"

#define  dbg4Port   (WITHDEBUG)

#if (dbg4Port)
//static CDbgMngr dbgLoad     ("ValveIsoPRV", "Load");
//static CDbgMngr dbgOperate  ("ValveIsoPRV", "Operate");
#endif


XID xidValveDP        = ModelXID(1);
//XID xidValveModeAct       = ModelXID(2);
//XID xidValveModeRqd       = ModelXID(3);

double Drw_VlvIsoPRV[] = 
  {
  DD_Poly, -4,3, -4,-3, 0,0, 4,3, 4,-3, -4,3,
  DD_Poly, -2,4, 2,4, //0,5, -2,4,
  DD_Arc3, 2,4, 0,6, -2,4, //0,5, -2,4,
  DD_Poly, 0,0, 0,7, 4,7, 0,0,
  DD_End 
  };

//const long Max4WayIOPorts=4;

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(CIsoPRV , "VlvIsoPRV", "1", Drw_VlvIsoPRV, "VlvIsoPRV", "PRV", TOC_ALL|TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_MINESERVE,
                    "Process:Piping:VlvIsoPRV(1)",
                    "Isolating Pressure Regulating Valve");

//===========================================================================

CIsoPRV ::CIsoPRV (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
MdlNode(pClass_, TagIn, pAttach, eAttach),
m_FEP(NULL, FBOpt_AsValve, 1.0)
  {
  AttachClassInfo(nc_Process, TwoIOAreaList);
  m_FEP.AssignFlwEqnGroup(NullFlwGroup, NullFlwGroup.Default()/*"VE_Line"*/, this);
  m_FEP.SetOptions(FBDDOpt_WithDPSpec, 1.0);

  Joins.SetSize(1);        
  
  m_PFeed=Std_P;
  };

// -------------------------------------------------------------------------
/*#F:This provides access to the variables of the model.*/
void CIsoPRV ::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  BuildDataDefnElevation(DDB);

  DDB.Text("");

  //DDB.CheckBoxBtn("On",     "", DC_,    "",   xidStateOn,                this, isParm|SM_DynBoth);
  DDB.Double("PressOutReqd", "POutRqd",  DC_P,    "kPag",   &m_PFeed,  this, isParm|SM_DynBoth);
  DDB.Text("");

  DDB.Double("PressureDrop", "PDrop",    DC_DP,    "kPa",   xidValveDP,  this, SM_DynBoth);

  DDB.Page  ("..");
  //m_FEP.BuildDataDefn(FBDDOpt_WithEqn, DDB, this, NULL);

  DDB.Text("");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  DDB.EndStruct();
  };

// -------------------------------------------------------------------------

flag CIsoPRV::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return true;

  if (m_FEP.DataXchg(DCB))
    return true;

  switch (DCB.lHandle)
    {
    case xidValveDP:
      if (NoProcessIOs()>1)
        DCB.D=IOP_Flng(0)-IOP_Flng(1);
      else
        DCB.D=dNAN;
      return 1;
    }
  return MdlNode::DataXchg(DCB);
  }

//--------------------------------------------------------------------------

void CIsoPRV ::StartStep()
  {
  }

//--------------------------------------------------------------------------

void CIsoPRV::EvalCtrlActions()
  {
  MdlNode::EvalCtrlActions();
  };

//--------------------------------------------------------------------------

flag CIsoPRV::ValidateData(ValidateDataBlk & VDB)
  {

  flag   OK=m_FEP.ValidateData(VDB);
  return MdlNode::ValidateData(VDB) && OK;
  };

//--------------------------------------------------------------------------

flag CIsoPRV::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  Acts.SetSize(0);
  //switch (m_State.m_iMethod)
  //  {
  //  case Meth_None:
  //    for (int i=0; i<m_ModeStrs.Length(); i++)
  //      {
  //      DDBValueLst * L=m_ModeStrs.Item(i);
  //      Acts.SetAtGrow(i, MdlAction(i, MAT_State, m_iVlvModeAct!=L->m_lVal, L->m_pStr, false, L->m_lVal));
  //      }
  //    break;
  //  case Meth_OnOff:
  //    Acts.SetAtGrow(0, MdlAction(0, MAT_State, !m_State.m_bOn, "On", 1));
  //    Acts.SetAtGrow(1, MdlAction(1, MAT_State, m_State.m_bOn, "Off", 0));
  //    Acts.SetAtGrow(2, MdlAction(2, MAT_Switch));
  //    break;
  //  }
  return True;
  }

//--------------------------------------------------------------------------

flag CIsoPRV::SetModelAction(Strng & Tag, MdlAction & Act)
  {
  //switch (m_State.m_iMethod)
  //  {
  //  case Meth_None:
  //    {
  //    DDBValueLst * L=m_ModeStrs.Item(Act.iIndex);
  //    SetVlvMode(L->m_lVal, false);
  //    break;
  //    }
  //  case Meth_OnOff:
  //    switch (Act.iIndex)
  //      {
  //      case 0:
  //      case 1:
  //        m_State.m_bOn=Act.iValue!=0;
  //        break;
  //      case 2:
  //        m_State.m_bOn=!m_State.m_bOn;
  //        break;
  //      }
  //    SetVlvMode(m_State.m_bOn ? m_State.m_iOnMode : m_State.m_iOffMode, false);
  //    break;
  //  }
  return true;
  }

//---------------------------------------------------------------------------

void CIsoPRV ::PostConnect(int IONo)
  {
  MdlNode::PostConnect(IONo);
  IOFB(IONo,0)->AssignFlwEqnGroup(NullFlwGroup, NullFlwGroup.Default(), this);
  IOFB(IONo,0)->SetParentFlwEqn(&m_FEP);
  };

//---------------------------------------------------------------------------

void CIsoPRV ::PreDisConnect(int IONo)
  {
  MdlNode::PreDisConnect(IONo);
  }

//--------------------------------------------------------------------------

Z_States CIsoPRV ::SetDatums(int Pass, int IOIn, double Zi, Z_States Z_State_Src)
  {
  return SetDatums_Node(Pass, NULL, IOIn, Zi, Z_State_Src);
  };

//--------------------------------------------------------------------------

flag CIsoPRV ::Set_Sizes()
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

void   CIsoPRV ::SetState(eScdMdlStateActs RqdState)
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
      for (int j=0; j<2; j++)
        {
        Set_JoinP(j, P0);
        Set_JoinP_Est(j, P0);
        Set_JoinP_Max(j, dNAN);
        Set_JoinP_MaxSet(j, dNAN);
        }
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

bool CIsoPRV::PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo)
  {
  if (!FlwNode::DoPropagateNetInfo(Ctrl, IONo, false))
    return false;

  for (int i=0; i<NoFlwIOs(); i++)
    {
    if (i!=IONo)
      Nd_Rmt(i)->PropagateNetInfo(Ctrl, IOIONo_Rmt(i));
    }

  return true;
  };

//---------------------------------------------------------------------------

void CIsoPRV::ConfigureJoins()
  {
  Set_NJoins(1);
  //if (IOId_2IOIn
  for (int i=0; i<NoProcessIOs(); i++)
    {
    //switch (IOId_Self(i))
    //  {
    //  case IOId_2IOIn : 
        SetIO_Open(i, 0, false, ESS_Denied);  
      //  break;
      //case IOId_2IOOut: 
      //  SetIO_Open(i, 0, false, ESS_Denied);  
      //  break;
      //}
    }
  };

//--------------------------------------------------------------------------
/*#F:This determines the pressure of the contents and the pressure at each
inlet and outlet of the surge unit.*/
void CIsoPRV ::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      if (NJoins()>=1)
        EvalPBJoinPressure(0, NULL, NULL);//&QProd());
      break;
    case NM_Dynamic:
      Set_JoinP_Est(0,m_PFeed);
      Set_JoinP(0,m_PFeed);
      break;
    }
  MdlNode::EvalJoinPressures(JoinMask);
  };

// --------------------------------------------------------------------------

flag CIsoPRV ::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  int Id=IOId_Self(IONo);
  FlwBlk &FB=*IOFB(IONo, FE);
  switch (Id)
    {
    case IOId_2IOIn:
      {
      int i=IOWithId_Self(IOId_2IOOut);
      if (i>=0)
        FB.SetQmReqd(IOQmEst_Out(i));
      else
        FB.SetQmReqd(0);
      }
      break;
    case IOId_2IOOut:
      FB.SetQmFree();
      break;
    }
  //dbgpln("IONo:%3i port %3i  %6.1f %s", IONo, port, m_Ports[port].m_Open*100, FullObjTag());
  return true;//FB.EvaluateFlwEqn(Task, pProps, true, false, 1.0/*Reg*/, &IOFBFlng_Rmt(IONo)->PhD(), NULL);
  };

// --------------------------------------------------------------------------

void CIsoPRV ::EvalProductsInit(EvalProductsInitTasks Task)
  {
  EvalProductsInit_Node(Task, 0x0ffffff);
  }

// --------------------------------------------------------------------------

//bool CIsoPRV::EvalProductClusters(int Index, long & JoinMask)
//  {
//  //JoinMask=-1;
//  //return Index==0;
//
//  switch (Index)
//    {
//    case 0: JoinMask=0x00000001;          return true;
//    case 1: JoinMask=0x00000002;          return true;
//    case 3: JoinMask=(0xffffffff & ~0x3); return true;
//    }
//  return false;
//  };

// --------------------------------------------------------------------------

void CIsoPRV ::EvalProducts(long JoinMask)
  {
  if (NJoins()>0)
    {
    switch (SolveMethod())
      {
      case SM_Direct:
        if (NJoins()>0)
          Xfer_EvalProducts(0, Joins[0].Pressure(), NULL, NULL, NULL, NULL, NULL, NULL);
        break;
      case SM_Inline:
      case SM_Buffered:
        for (int j=0; j<NJoins(); j++)
          Xfer_EvalProducts(j, Joins[j].Pressure(), NULL, NULL, NULL, NULL, NULL);
        break;
      }
    }
  };

//--------------------------------------------------------------------------

void CIsoPRV ::EvalDiscrete()
  {
  MdlNode::EvalDiscrete();
  };

//--------------------------------------------------------------------------

void CIsoPRV ::EvalPBMakeUpReqd(long JoinMask)
  {
  if (GSM.Enabled())
    GSM.MakeUpNodeTransferReqd(0);
  MakeUpNodeTransferReqd(0);
  };

//--------------------------------------------------------------------------

void CIsoPRV ::EvalPBMakeUpAvail(long JoinMask)
  {
  if (GSM.Enabled())
    GSM.MakeUpNodeTransferAvail(0);
  MakeUpNodeTransferAvail(0);
  };

//--------------------------------------------------------------------------

void CIsoPRV ::ClosureInfo()
  {
  bool InRange=true;
  for (int i=0; i<NoFlwIOs(); i++)
    InRange = InRange && IOConduit(i)->SMFnsInRange();
  FlwNode::SetCI(29, !InRange);

  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI= m_Closure[0];
    }
  };

//--------------------------------------------------------------------------

void CIsoPRV ::DumpDerivs()
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

dword CIsoPRV ::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (GetStatusAsStopped())
    {
    Status |= (/*m_FEP.FlwRqdActive() ? FNS_UFlw :*/ FNS_UNoFlw);
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

      //switch (m_State.m_iMethod)
      //  {
      //  case Meth_None:
      //    Status |= (m_iVlvModeAct ? FNS_On : FNS_Off);
      //    break;
      //  case Meth_OnOff:
      //    Status |= (m_State.m_bOn ? FNS_On : FNS_Off);
      //    break;
      //  case Meth_Profile:
      //    Status |= (m_State.m_ActualPosn>0.5 ? FNS_On : FNS_Off);
      //    break;
      //  }
      }
    }
  return Status;
  };

//--------------------------------------------------------------------------

flag CIsoPRV::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\t???"; return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  }

//--------------------------------------------------------------------------
