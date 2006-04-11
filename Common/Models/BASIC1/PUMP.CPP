//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __PUMP_CPP
#include "pump.h"
#include "power.h"

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Pump(1)
#X:#h<General Description>#nThe general purpose pump increases the pressure
of the material that flows through it. The pump can be configured by selecting
the pump logic (speed control strategy) and pump flow equation, that best
match the operation of the actual pump. It is always connected from an outlet
point on one pipe to an inlet point of another. It is commonly used to boost
the pressures of liquids, slurries and gases. The pump only has one inlet and
one outlet point.
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
In           Input          1              1#n
Out          Output         1              1#n
#a#n
#h<Variables to be supplied by the user>#n
#i<VLEquilibrium> : This can be used to switch #x<Vapour Liquid Equilibrium Logic>
on. If it is on then the associated variables (VLE) must be configured.#n
#i<Z_Reqd> : The required datum of the pump.#n
#i<Speed.Type> : This allows the user to select the #x<pump speed logic>
control strategy. The associated variables must be confiqured.#n
#i<Eqn.Name> : This allows the user to select the required model from the
#x<pump flow equations> which calculates the pressure drop across the pump.
The associated variables must be confiqured.#n
#n#n
#h<Associated variables>#n
#i<Qm> : The mass flow of the material through the unit.#n
#i<Qv> : The volumetric flowrate of the material through the unit.#n
#i<NQv> : The normalized volumetric flowrate, ie the flowrate measured at STP,
of the material within the unit. This will only differ from "Qv" when pressurized
gases are being conveyed, since the volume of a gas is proportional to its pressure.#n
#i<Rho> : The density of the material within the unit.#n
#i<Vf> : Vapour Fraction, the percentage of vapours within the unit.#n
#i<mVf> : The Molar percentage of vapours within the unit.#n
#i<dPb> : The change in pressure boost across the unit.#n
#i<dPq> : The pressure drop in the pump caused by the flow of material within the unit.#n
#i<Pi> : The pressure at the inlet of the unit.#n
#i<Po> : The pressure at the outlet of the unit.#n
#i<Z> : The height of the unit, in relation to the common datum.#n
#n
#n#h<Other>#n
Default model prefix:PMP#n
Short name:Pump#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

double Drw_B_Pump[] = { DD_Arc, 0, 0, 4,
                      DD_Poly, 0,-4, 5,-4, 5,-2, 0,-2,
                      DD_End };

//XID xidPressMethod = MdlBsXID(4801);

const int CIOId_Spd    = CtrlIOId(0);
const int EIOId_Supply = ElecIOId(0);

IOAreaRec PumpIOAreaList[] =
  {
    {"Input",         "In",        IOId_2IOIn,     LIO_In0,  nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"Input",         "I",         IOId_2IOIn,     LIO_In,   nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"",              "Src",       IOId_2IOIn,     LIO_In,   nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"",              "S",         IOId_2IOIn,     LIO_In,   nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"Output",        "Out",       IOId_2IOOut,    LIO_Out0, nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"Output",        "O",         IOId_2IOOut,    LIO_Out,  nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"",              "Dst",       IOId_2IOOut,    LIO_Out,  nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"",              "D",         IOId_2IOOut,    LIO_Out,  nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    LEAKS2AREA("LeakI",     IOId_2IOLeakI),  
    LEAKS2AREA("LeakO",     IOId_2IOLeakO),  
    //{"Leak_In",       "LeakI",     IOId_2IOLeakI,  LIO_Out,  nc_MLnk, 0,1, IOOptsHide|IOEqnsHide|IOHidden},
    //{"Leak_Out",      "LeakO",     IOId_2IOLeakO,  LIO_Out,  nc_MLnk, 0,1, IOOptsHide|IOEqnsHide|IOHidden},
    {"",              "Speed",     CIOId_Spd,      LIO_In,   nc_CLnk, 0,1},
    {"ElecSupply",    "ElecIn",    EIOId_Supply,   LIO_In,   nc_ELnk, 0,50, 0, 0.0, {0,0,0,0}, NULL, NULL, "Supply"},
    {NULL}
  };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Pump_GP, "Pump", "1", Drw_B_Pump, "Pump", "PMP", TOC_ALL|TOC_GRP_TRANSFER|TOC_STD_KENWALT,
                    "Process:Piping:Pump(1)",
                    "General Purpose Pump")

Pump_GP::Pump_GP(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_BstRes(pClass_, TagIn, pAttach, eAttach),
  m_FRB("Limit", FBRShow_What|FBRShow_Reqd|FBRShow_Cur, FBReg_Off|FBReg_Pi|FBReg_Qm|FBReg_Qv|FBReg_QvStep, FBReg_Pi, 1.0, 5.0, (float)Std_P, 0.0, 0.0, 1.0),
  m_Pwr("Supply", this, TOA_Embedded),
  m_PSB("PL_Basic", this, &m_Pwr),
  m_SpdVar(BXO_Blank)
  {
  m_Pwr.Initialise("525v_A/C");
  //AttachIOAreas(PumpIOAreaList, &PumpGroup, &NullFlwGroup);
  AttachIOAreas(PumpIOAreaList, &NullFlwGroup, &NullFlwGroup);
  m_FEP.AssignFlwEqnGroup(PumpGroup, PumpGroup.Default(), this);
  m_FEP.SetOptions(0, 1.0);
  //m_dFlwReg = 1.0;
  bIsentropic = 1;
  iPressMethod = BRPM_Boost;
  dPressBoost_Rqd=10.0;
  dPout_Rqd=dNAN;

  FinaliseConstruction();
  }

//--------------------------------------------------------------------------

void Pump_GP::BuildDataDefn(DataDefnBlk & DDB)
  {
  static DDBValueLst DDB1[]={
    {BRPM_None, "None"},
    {BRPM_Boost, "Boost"},
    {BRPM_Fixed, "Fixed"},
    {0}};
  DDB.BeginStruct(this);
  if (SolveDirectMethod())
    {
    DDB.Text    ("");
    DDB.Text    ("Requirements");
    DDB.Byte    ("PressMethod",     "",     DC_,      "",      xidPressMethod,  this, isParm|SetOnChange, DDB1);
    DDB.Visibility(SHM_All, iPressMethod==BRPM_Boost);
    DDB.Double  ("PressBoostRqd",   "PBoost", DC_DP,  "kPa",   xidPBoostRqd,    this, isParm);
    DDB.Visibility(SHM_All, iPressMethod==BRPM_Fixed);
    DDB.Double  ("OutletPressRqd",  "PRqd", DC_P,     "kPag",  xidPoutRqd,      this, isParm|NAN_OK);
    DDB.Visibility();
    m_VLE.Add_OnOff(DDB);

    DDB.Text    ("");
    DDB.Text    ("Results");
    DDB.Double  ("",                "Work", DC_Pwr,   "kW",    xidDuty,         this, isResult);
    DDB.Double  ("Mass_Flow",       "Qm",   DC_Qm,    "kg/s",  xidQm,           this, isResult|noFile|noSnap);
    DDB.Double  ("TemperatureIn",   "Ti",   DC_T,     "C",     xidTempIn,       this, isResult|noFile|noSnap);
    DDB.Double  ("TemperatureOut",  "To",   DC_T,     "C",     xidTempOut,      this, isResult|noFile|noSnap);
    DDB.Double  ("PressIn",         "Pi",   DC_P,     "kPag",  xidPIn,          this, isResult|isParmConstruct);
    DDB.Double  ("Press_Change",    "dP",   DC_DP,    "kPa",   xiddP,           this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("PressOut",        "Po",   DC_P,     "kPag",  xidPOut,         this, isResult|isParmConstruct);
    DDB.Double  ("VapourFracIn",    "Vfi",  DC_Frac,  "%",     xidVapFracIn,    this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("VapourFracOut",   "Vfo",  DC_Frac,  "%",     xidVapFracOut,   this, isResult|noFile|noSnap|InitHidden);
    //DDB.Double  ("SatT",            "",     DC_T,     "C",     &dSatTOut,       this, isResult|noFile|noSnap);
    //DDB.Double  ("SatP",            "",     DC_P,     "kPag",  &dSatPOut,       this, isResult|noFile|noSnap|InitHidden);
    //DDB.Double  ("H2O_Frac",        "",     DC_Frac,  "%",     &dH20Frac,       this, isResult|noFile|noSnap|InitHidden);*/
    if (0)
      {
      DDB.Text    ("");
      DDB.Double  ("Vol_Flow",        "Qv",   DC_Qv,    "L/s",   xidQv,           this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("NVol_Flow",       "NQv",  DC_NQv,   "NL/s",  xidNQv,          this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Temperature",     "T",    DC_T,     "C",     xidTemp,         this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Density",         "Rho",  DC_Rho,   "kg/m^3",xidRho,          this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Vap_Frac",        "Vf",   DC_Frac,  "%",     xidVapFrac,      this, isResult|noFile|noSnap|InitHidden);
      }
    DDB.Double  ("PrevPOut",        "",     DC_P,     "kPag",  &dPout,          this, isParm|noView);
    DDB.Double  ("PrevDuty",        "",     DC_Pwr,   "kW",    &dDuty,          this, isParm|noView);
    }
  else
    {
    DDB.Text    ("");
    DDB.Double  ("Mass_Flow",       "Qm",   DC_Qm,    "kg/s",  xidQm,           this, isResult|noFile|noSnap);
    DDB.Double  ("Vol_Flow",        "Qv",   DC_Qv,    "L/s",   xidQv,           this, isResult|noFile|noSnap);
    DDB.Double  ("NVol_Flow",       "NQv",  DC_NQv,   "NL/s",  xidNQv,          this, isResult|noFile|noSnap);
    DDB.Double  ("",                "QmPrvPB",DC_Qm,  "kg/s",  xidFEQmPrvPB,    this, isResult|InitHidden);
    //DDB.Double  ("",                "QmPB", DC_Qm,    "kg/s",  xidFEQmMemPB,    this, isResult|InitHidden);

    DDB.Double  ("Temperature",     "T",    DC_T,     "C",     xidTemp,         this, isResult|noFile|noSnap);
    DDB.Double  ("Pressure",        "P",    DC_P,     "kPag",  xidPMean,        this, isResult|noFile|noSnap);
    DDB.Double  ("Density",         "Rho",  DC_Rho,   "kg/m^3",xidRho,          this, isResult|noFile|noSnap);
    DDB.Double  ("NDensity",        "NRho", DC_Rho,   "kg/m^3",xidNRho,         this, isResult|noFile|noSnap);
    BuildDataDefnElevation(DDB);
    DDB.Text    ("");
    DDB.Double  ("VapourFrac",      "Vf",   DC_Frac,  "%",     xidVapFrac,      this, isResult|noFile|noSnap);
    DDB.Double  ("mVapourFrac",     "mVf",  DC_Frac,  "%",     xidMoleVapFrac,  this, isResult|noFile|noSnap);
    DDB.Text    ("");
    DDB.Double  ("Boost_Head",      "dPb",  DC_DP,    "kPa",   xidFEDPb,        this, isResult|0);
    DDB.Double  ("Dynamic_Head",    "dPq",  DC_DP,    "kPa",   xidFEDPq,        this, isResult|0);
    DDB.Double  ("Press_InEst",     "PiEst",DC_P,     "kPag",  xidPInEst,       this, isResult|isParmConstruct|InitHidden);
    DDB.Double  ("Press_Input",     "Pi",   DC_P,     "kPag",  xidPIn,          this, isResult|0);
    DDB.Double  ("Press_Change",    "dP",   DC_DP,    "kPa",   xiddP,           this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("Press_Output",    "Po",   DC_P,     "kPag",  xidPOut,         this, isResult|0);
    DDB.Double  ("Press_OutEst",    "PoEst",DC_P,     "kPag",  xidPOutEst,      this, isResult|isParmConstruct|InitHidden);
    m_VLE.Add_OnOff(DDB);
    m_LeakI.Add_OnOff(DDB,0, 101);
    m_LeakO.Add_OnOff(DDB,0, 102);
    }

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);

  DDB.Visibility();

  if (SolveDirectMethod())
    {
    m_VLE.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);
    }
  else
    {
    DDB.Page("Parms");
    DDB.Text("");
    m_FEP.BuildDataDefn(FBDDOpt_WithEqn|FBDDOpt_WithQmSpec, DDB, this);
    //m_FEP.BuildDataDefn(FBDDOpt_WithEqn, DDB, this);
    m_PSB.BuildDataDefn(DDB, this, "Speed", 1);
    m_FRB.BuildDataDefn(DDB, this, FlowMode()==LFM_Linear||FlowMode()==LFM_Full);
    m_Pwr.BuildDataDefn(DDB);
    m_VLE.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);
    if (NoFlwIOs()>0 && !DDB.ForFileSnpScn() && (!DDB.ForView() || dbgfile()))
      IOFB(0,0)->BuildDataDefn(FBDDOpt_WithData, DDB, this, "In", 1);
    DDB.Text("");

    }

  DDB.Visibility();
  DDB.Text("");
  m_FTB.BuildDataDefn(DDB, "Thermal", NULL, DDB_RqdPage, FEOF_WithPVConst);

  if (SolveDynamicMethod() && (m_LeakI.Enabled || m_LeakO.Enabled))
    {
    DDB.Page("AreaIO", DDB_RqdPage);
    m_LeakI.BuildDataDefn(DDB, NULL, DDB_NoPage, 101);
    m_LeakO.BuildDataDefn(DDB, NULL, DDB_NoPage, 102);
    }

  DDB.EndStruct();
  }

// --------------------------------------------------------------------------

flag Pump_GP::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_BstRes::DataXchg(DCB))
    return 1;
  if (m_FEP.DataXchg(DCB))
    return 1;
  if (m_PSB.DataXchg(DCB))
    return 1;
  if (m_FRB.DataXchg(DCB, this))
    return 1;
  if (m_Pwr.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==101 && m_LeakI.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==102 && m_LeakO.DataXchg(DCB))
    return 1;

  return 0;
  }

// -------------------------------------------------------------------------

flag Pump_GP::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_BstRes::ValidateData(VDB);
  if (!m_FEP.ValidateData(VDB))
    OK=False;
  if (!m_FRB.ValidateData(VDB, this))
    OK=false;
  return OK;
  }

//--------------------------------------------------------------------------

int Pump_GP::AutoWiring(AutoWireJob AWJ)
  {
  switch (AWJ)
    {
    case AW_Prepare:
      m_Pwr.SetConnectRqd("Wired");
      break;
    }
  return 0;
  };

//--------------------------------------------------------------------------

void Pump_GP::CollectElectrics(CNodeElectricsArray & TSCA)
  {
  MN_BstRes::CollectElectrics(TSCA);

  CNodeElectrics &TSP=m_Electrics;
  if (m_Pwr.TermStrip())
    TSP.AddTermStrip(m_Pwr.TermStrip());
  };

//--------------------------------------------------------------------------

void Pump_GP::ConnectElectrics()
  {
  MN_BstRes::ConnectElectrics();
  if (m_Pwr.TermStrip())
    m_Pwr.ConnectPower();
  };

//--------------------------------------------------------------------------

flag Pump_GP::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  Acts.SetSize(0);
  Acts.SetAtGrow(0, MdlAction(0, MAT_State, !m_PSB.On(), "Start", 1));
  Acts.SetAtGrow(1, MdlAction(1, MAT_State, m_PSB.On(),  "Stop", 0));
  Acts.SetAtGrow(2, MdlAction(2, MAT_Value, m_PSB.On(),  "Speed (%)", true,
                                    m_PSB.ManualSpeed(this)*100, 0.0, 100.0,      
                                    m_PSB.Speed(this, &m_FRB)*100));                       
  Acts.SetAtGrow(3, MdlAction(3, MAT_Switch));
  return True;
  }

//--------------------------------------------------------------------------

flag Pump_GP::SetModelAction(Strng & Tag, MdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
      m_PSB.SetOn(Act.iValue);
      break;
    case 1:
      m_PSB.SetOn(Act.iValue);
      break;
    case 2:
      m_PSB.SetManualSpeed(Act.dValue*0.01);
      break;
    case 3:
      m_PSB.SetOn(!m_PSB.On());
      break;
    }
  return true;
  }

// --------------------------------------------------------------------------

void Pump_GP::ConfigureJoins()
  {
  MN_BstRes::ConfigureJoins();
  if (NetMethod()==NM_Dynamic)
    {
    SetJoin_HasVolume(0, false, true);
    SetJoin_PumpNd(0, false);           // if this is true then the pump becomes a 'tie'
    SetJoin_HasVolume(1, false, true);
    SetJoin_PumpNd(1, false);           // if this is true then the pump becomes a 'tie'
    }
  }

// --------------------------------------------------------------------------

flag Pump_GP::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  if (m_LeakI.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  if (m_LeakO.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  if (LnkNo>=0)
    {
    FlwBlk &FB=m_Links[LnkNo].FB;
    if (Task!=FET_GetMode)
      FB.SetRegulator(m_FRB.What());
    if (NoFlwIOs()>=2 && IOId_Self(0)==IOId_2IOIn && IOId_Self(1)==IOId_2IOOut)
      {
      //bool ShowBug=false;
      //switch (m_FRB.What())
      //  {
      //  case FBReg_Pi:
      //    ShowBug= (m_FRB.Ctrl()<1e-6 && IOPB(0,0).PEst > m_FRB.Target());
      //    break;
      //  }
      //SetCI(1, ShowBug);
      return FB.EvaluateFlwEqn(Task, pProps, m_PSB.On()!=0, true, m_PSB.Speed(this, &m_FRB)/**Max(1e-3, m_FRB.Ctrl())*/, &IOFBFlng_Rmt(0)->PhD(), &IOFBFlng_Rmt(1)->PhD());
      }
    FB.SetQmReqd(0.0);
    return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

void Pump_GP::Ctrl_ConnIDStr(int i, Strng & ID, Strng &Tg)
  {
  return m_PSB.Ctrl_ConnIDStr(i, ID, Tg);
  }

//--------------------------------------------------------------------------

bool Pump_GP::IsXRefListActive() { return m_PSB.On()!=0; }

//---------------------------------------------------------------------------

int Pump_GP::UpdateXRefLists(CXRefBuildResults & Results)
  {
  if (1)//m_PSB.On())
    {
    FnMngrClear();
    int FunctNo = 0;
    Strng S;
    S.Set("%s.%s", Tag(), "Speed.Reqd");
    int RetCode1 = m_SpdVar.UpdateXRef(NULL, 0, 1, FunctNo, this, CIOId_Spd, S(), S(), "Pump:Speed", Results);
    FnMngrTryUpdateXRefLists(Results);
    }

  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void Pump_GP::UnlinkAllXRefs()
  {
  FnMngrClear();
  FnMngrTryUnlinkAllXRefs();

  m_SpdVar.UnlinkXRefs();

  CNodeXRefMngr::UnlinkAllXRefs();
  };
// --------------------------------------------------------------------------

void Pump_GP::EvalCtrlActions()
  {
  double D;
  if (m_SpdVar.GetValue(D))
    m_PSB.SetSpeed(D);
  m_PSB.EvalCtrlActions(this);
  }

//---------------------------------------------------------------------------

void Pump_GP::EvalPowerRequired()
  {
  m_Pwr.Zero();
  double ShftPwr=m_FEP.ShaftPower();
  if (Valid(ShftPwr))
    m_Pwr.SetPower(ShftPwr);
  }

//---------------------------------------------------------------------------

dword Pump_GP::ModelStatus()
  {
  dword Status=MN_BstRes::ModelStatus();
  if (!gs_Exec.Stopped() && SolveDynamicMethod())
    {
    Status |= ((m_PSB.On() && fabs(m_PSB.Speed(this, &m_FRB))>0.0) ? FNS_On : FNS_Off);
    }
  return Status;
  }

//---------------------------------------------------------------------------

flag Pump_GP::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tPotential Bug - Switch OFF Regulator"; return 1;
    default:
      return MN_BstRes::CIStrng(No, pS);
    }
  }
//==========================================================================
//
//
//
//==========================================================================
