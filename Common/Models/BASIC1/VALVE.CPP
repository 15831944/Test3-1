//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __VALVE_CPP
#include "valve.h"
//#include "optoff.h"

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Valve(1)
#X:#h<General Description>#nThe general purpose valve controls the flow and
pressure drop in the pipe system. This valve can be configured by selecting
the valve logic (valve control strategy) and valve flow equation, that best
match the operation of the actual valve. The valve is always connected from
an outlet point of one pipe to an inlet point of another. It is commonly used
to regulate the flow of liquids, slurries and gases. The valve only has one
inlet and one outlet point.
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
In           Input          1              1#n
Out          Output         1              1#n
#a#n
#h<Variables to be supplied by the user>#n
#i<Z_Reqd> : Datum.#n
#i<Posn.Type> : This allows the user to select the #x<valve position logic>
(valve control strategy) which best suits the needs of the valve. The
associated variables must be confiqured.#n
#i<Eqn.Name> : This allows the user to select the required model from
the #x<valve flow equations> which calculates the pressure drop across
the valve. The associated variables must be confiqured.#n
#i<VLEquilibrium> : This can be used to switch #x<Vapour Liquid Equilibrium Logic>
on. If it is on then the associated variables (VLE) must be configured.#n
#i<Qio.Model> : This allows the selection of the model that is to be used to calculate
how the species are combined, this is normally done using a mass weighted mean.#n
#i<Qio.View> : This allows the selection of how the list of species is being displayed.
Options such as MassFlow, MoleFlow, VolFlow, MassFrac, ElemntFlow, CompntConc etc can
be selected.#n
#i<Qio.Phase> : This allows the selection of which phase(s) for the list of species is
being displayed. Options such as All, Solids, Liquids and Vapours can be selected.#n
#n#n
#h<Associated variables>#n
#i<Qm> : The mass flow of the material through the unit.#n
#i<Qv> : The volumetric flowrate of the material through the unit.#n
#i<NQv> : The normalized volumetric flowrate, ie the flowrate measured at STP,
of the material within the unit. This will only differ from "Qv" when pressurized
gases are being conveyed, since the volume of a gas is proportional to its pressure.#n
#i<T> : Temperature of the material within the unit.#n
#i<Rho> : The density of the material within the unit.#n
#i<Z> : The height of the unit, in relation to the common datum.#n
#i<SrcIO> : The tag identifying the source (upstream) pipe to which the input of the unit is connected.#n
#i<DstIO> : The tag identifying the destination (downstream) pipe to which the output of the unit is connected.#n
#i<Pi> : The pressure at the inlet of the unit.#n
#i<dP> : The total pressure drop across the unit.#n
#i<Po> : The pressure at the outlet of the unit.#n
#n
#n#h<Other>#n
Default model prefix:VLV#n
Short name:Valve#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

static double Drw_Valve_GP[] = { DD_Poly, -4,3, -4,-3, 4,3, 4,-3, -4,3,
                        DD_End };

const int CIOId_Pos     = CtrlIOId(0);
const int EIOId_Supply  = ElecIOId(0);

IOAreaRec ValveIOAreaList[] =
  {
    {"Input",         "In",        IOId_2IOIn,     LIO_In0,  nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"Input",         "I",         IOId_2IOIn,     LIO_In,   nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"",              "Src",       IOId_2IOIn,     LIO_In,   nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"",              "S",         IOId_2IOIn,     LIO_In,   nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"Output",        "Out",       IOId_2IOOut,    LIO_Out0, nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"Output",        "O",         IOId_2IOOut,    LIO_Out,  nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"",              "Dst",       IOId_2IOOut,    LIO_Out,  nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"",              "D",         IOId_2IOOut,    LIO_Out,  nc_MLnk, 1,1, IOOptsHide|IOGRP(1)},
    {"Leak_In",       "LeakI",     IOId_2IOLeakI,  LIO_Out,  nc_MLnk, 0,1, IOOptsHide|IOEqnsHide|IOHidden},
    {"Leak_Out",      "LeakO",     IOId_2IOLeakO,  LIO_Out,  nc_MLnk, 0,1, IOOptsHide|IOEqnsHide|IOHidden},
    {"",              "Position",  CIOId_Pos,      LIO_In,   nc_CLnk, 0,1},
    {"ElecSupply",    "ElecIn",    EIOId_Supply,   LIO_In,   nc_ELnk, 0,50},
    {NULL}
  };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Valve_GP, "Valve", "1", Drw_Valve_GP, "Valve", "VLV", TOC_ALL|TOC_GRP_TRANSFER|TOC_STD_KENWALT,
                    "Process:Piping:Valve(1)",
                    "General Purpose Valve")

Valve_GP::Valve_GP(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_BstRes(pClass_, TagIn, pAttach, eAttach),
  m_VPB("VL_Basic", this, &m_Pwr),
  m_FRB("Reg"),
  m_Pwr("Supply", this, TOA_Embedded),
  m_PosVar(BXO_Blank)//"525v_A/C")
  {
  m_Pwr.Initialise("525v_A/C");
  AttachIOAreas(ValveIOAreaList, &NullFlwGroup/*ValveGroup*/, &NullFlwGroup);
  m_FEP.AssignFlwEqnGroup(ValveGroup, ValveGroup.Default()/*"VE_Line"*/, this);
  m_FEP.SetOptions(FBOpt_AsValve, 1.0);
  //m_VPB.AssignLogicGroup("VPL_Smpl", this);
  //Initialise();
  dPressBoost_Rqd=-10.0;
  iPressMethod = BRPM_Drop;

  FinaliseConstruction();
  }

//--------------------------------------------------------------------------
/*
void Valve_GP::Initialise()
  {
  FB.Initialise();
  }
*/
//--------------------------------------------------------------------------

void Valve_GP::BuildDataDefn(DataDefnBlk &DDB)
  {
  static DDBValueLst DDB1[]={
    {BRPM_None, "None"},
    {BRPM_Drop, "Drop"},
    {BRPM_Fixed, "Fixed"},
    {BRPM_Atmos, "Atmos"},
    {0}};
  DDB.BeginStruct(this);
  if (SolveDirectMethod())
    {
    DDB.Text    ("");
    DDB.Text    ("Requirements");
    DDB.Byte    ("PressMethod",     "",     DC_,      "",      xidPressMethod,  this, isParm|SetOnChange, DDB1);
    DDB.Visibility(SHM_All, iPressMethod==BRPM_Drop);
    DDB.Double  ("PressDropRqd",    "PDrop", DC_DP,   "kPa",   xidPDropRqd,     this, isParm);
    DDB.Visibility(SHM_All, iPressMethod==BRPM_Fixed);
    DDB.Double  ("OutletPressRqd",  "PRqd", DC_P,     "kPag",  xidPoutRqd,      this, isParm|NAN_OK);
    DDB.Visibility();
    m_VLE.Add_OnOff(DDB);
    DDB.Text    ("");
    DDB.Text    ("Results");
    DDB.Double  ("Mass_Flow",       "Qm",   DC_Qm,    "kg/s",  xidQm,           this, isResult|noFile|noSnap);
    DDB.Double  ("TemperatureIn",   "Ti",   DC_T,     "C",     xidTempIn,       this, isResult|noFile|noSnap);
    DDB.Double  ("TemperatureOut",  "To",   DC_T,     "C",     xidTempOut,      this, isResult|noFile|noSnap);
    DDB.Double  ("PressIn",         "Pi",   DC_P,     "kPag",  xidPIn,          this, isResult|isParmConstruct);
    DDB.Double  ("Press_Change",    "dP",   DC_DP,    "kPa",   xiddP,           this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("PressOut",        "Po",   DC_P,     "kPag",  xidPOut,         this, isResult|isParmConstruct);
    DDB.Double  ("VapourFracIn",    "Vfi",  DC_Frac,  "%",     xidVapFracIn,    this, isResult|noFile|noSnap);
    DDB.Double  ("VapourFracOut",   "Vfo",  DC_Frac,  "%",     xidVapFracOut,   this, isResult|noFile|noSnap);
    //DDB.Double  ("SatT",            "",     DC_T,     "C",     &dSatTOut,       this, isResult|noFile|noSnap);
    //DDB.Double  ("SatP",            "",     DC_P,     "kPag",  &dSatPOut,       this, isResult|noFile|noSnap|InitHidden);
    //DDB.Double  ("H2O_Frac",        "",     DC_Frac,  "%",     &dH20Frac,       this, isResult|noFile|noSnap|InitHidden);*/
    if (0)
      {
      DDB.Text    ("");
      DDB.Double  ("Vol_Flow",        "Qv",   DC_Qv,    "L/s",   xidQv,           this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("NVol_Flow",       "NQv",  DC_NQv,   "NL/s",  xidNQv,          this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Temperature",     "T",    DC_T,     "C",     xidTemp,         this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Pressure",        "P",    DC_P,     "kPag",  xidPMean,        this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Density",         "Rho",  DC_Rho,   "kg/m^3",xidRho,          this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Vap_Frac",        "Vf",   DC_Frac,  "%",     xidVapFrac,      this, isResult|noFile|noSnap|InitHidden);
      }
    DDB.Double  ("PrevPOut",        "",     DC_P,     "kPag",  &dPout,          this, isParm|noView);
    DDB.Double  ("PrevDuty",        "",     DC_Pwr,   "kW",    &dDuty,          this, isParm|noView);
    }
  else
    {
    DDB.Text    ("");
    DDB.Double  ("Mass_Flow_Est",   "QmEst",DC_Qm,    "kg/s",  xidQmEst,        this, isResult|noFile|noSnap);
    DDB.Double  ("Mass_Flow",       "Qm",   DC_Qm,    "kg/s",  xidQm,           this, isResult|0);
    DDB.Double  ("Vol_Flow",        "Qv",   DC_Qv,    "L/s",   xidQv,           this, isResult|noFile|noSnap);
    DDB.Double  ("NVol_Flow",       "NQv",  DC_NQv,   "NL/s",  xidNQv,          this, isResult|noFile|noSnap);
    DDB.Double  ("",                "QmPrvPB",DC_Qm,  "kg/s",  xidFEQmPrvPB,    this, isResult|InitHidden);
    //DDB.Double  ("",                "QmPB", DC_Qm,    "kg/s",  xidFEQmMemPB,    this, isResult|InitHidden);
    DDB.Double  ("Temperature",     "T",    DC_T,     "C",     xidTemp,         this, isResult|noFile|noSnap);
    //DDB.Double  ("Pressure",        "P",    DC_P,     "kPag",  xidPMean,        this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("Density",         "Rho",  DC_Rho,   "kg/m^3",xidRho,          this, isResult|noFile|noSnap);
    DDB.Double  ("NDensity",        "NRho",  DC_Rho,  "kg/m^3",xidNRho,         this, isResult|noFile|noSnap);
    BuildDataDefnElevation(DDB);
    DDB.Text    ("");
    //DDB.Double  ("VapourFrac",      "Vf",   DC_Frac,  "%",     xidVapFrac,      this, isResult|noFile|noSnap);
    //DDB.Double  ("mVapourFrac",     "mVf",  DC_Frac,  "%",     xidMoleVapFrac,  this, isResult|noFile|noSnap);
    //DDB.Text   ("Pressure Audit", Marker|MarkerClosed);
    DDB.Double  ("Press_InEst",     "PiEst",DC_P,     "kPag",  xidPInEst,       this, isResult|isParmConstruct|InitHidden);
    DDB.Double  ("Press_Input",     "Pi",   DC_P,     "kPag",  xidPIn,          this, isResult);
    DDB.Double  ("Press_Change",    "dP",   DC_DP,    "kPa",   xiddP,           this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("Press_Output",    "Po",   DC_P,     "kPag",  xidPOut,         this, isResult);
    DDB.Double  ("Press_OutEst",    "PoEst",DC_P,     "kPag",  xidPOutEst,      this, isResult|isParmConstruct|InitHidden);
    DDB.Double  ("",                "dPbX", DC_DP,    "kPa",   xidFEDPbX,       this, isResult|isParmConstruct|InitHidden);
    }

  DDB.Text   ("");
  BuildDataDefnShowIOs(DDB);

  DDB.Visibility();

  if (SolveDirectMethod())
    {
    m_VLE.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);
    }
  else
    {
    DDB.Page   ("Parms");
    m_VLE.Add_OnOff(DDB);
    m_LeakI.Add_OnOff(DDB,0,101);
    m_LeakO.Add_OnOff(DDB,0,102);
    
    DDB.Text   ("");

    
    m_FEP.BuildDataDefn(FBDDOpt_WithEqn|FBDDOpt_WithQmSpec|FBDDOpt_WithDPSpec, DDB, this, NULL, -1, DDB_RqdPage);
    //m_FEP.BuildDataDefn(FBDDOpt_WithEqn|FBBFlowOptions(false), DDB, this, NULL, -1, DDB_RqdPage);

    if (NoFlwIOs()>0 && !DDB.ForFileSnpScn() && (!DDB.ForView() || dbgfile()))
      IOFB(0,0)->BuildDataDefn(FBDDOpt_WithData, DDB, this, "In", 1);

    DDB.Text("");
    m_VPB.BuildDataDefn(DDB, this, "Posn", 1);
    m_FRB.BuildDataDefn(DDB, this, FlowMode()==LFM_Linear||FlowMode()==LFM_Full);
    m_Pwr.BuildDataDefn(DDB);

    m_VLE.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);

    if (NoFlwIOs()>0)
      DDB.Object(IOConduit(0), this);

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

//--------------------------------------------------------------------------

flag Valve_GP::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_BstRes::DataXchg(DCB))
    return 1;
  if (m_VPB.DataXchg(DCB))
    return 1;
  if (m_FEP.DataXchg(DCB))
    return 1;
  if (m_FRB.DataXchg(DCB, this))
    return 1;
  if (DCB.dwUserInfo==101 && m_LeakI.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==102 && m_LeakO.DataXchg(DCB))
    return 1;

  return 0;
  }

//--------------------------------------------------------------------------

flag Valve_GP::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_BstRes::ValidateData(VDB);
  if (!m_FEP.ValidateData(VDB))
    OK=False;
  if (!m_FRB.ValidateData(VDB, this))
    OK=false;
  return OK;
  }

//--------------------------------------------------------------------------

flag Valve_GP::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  Acts.SetSize(0);
  Acts.SetAtGrow(0, MdlAction(0, MAT_State, !m_VPB.On(), "Open", 1));
  Acts.SetAtGrow(1, MdlAction(1, MAT_State, m_VPB.On(),  "Close", 0));
  Acts.SetAtGrow(2, MdlAction(2, MAT_Value, m_VPB.On(),  "Manual Posn (%)", true,
                                      m_VPB.ManualPosition(this)*100, 0.0, 100.0,     
                                      m_VPB.ActualPosition(this)*100));                
  Acts.SetAtGrow(3, MdlAction(3, MAT_Switch));
  return True;
  }

//--------------------------------------------------------------------------

flag Valve_GP::SetModelAction(Strng & Tag, MdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
      m_VPB.SetOn(Act.iValue);
      break;
    case 1:
      m_VPB.SetOn(Act.iValue);
      break;
    case 2:
      m_VPB.SetManualPosition(Act.dValue*0.01);
      break;
    case 3:
      m_VPB.SetOn(!m_VPB.On());
      break;
    }
  return true;
  }

//--------------------------------------------------------------------------

flag Valve_GP::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
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
      return FB.EvaluateFlwEqn(Task, pProps, m_VPB.On(), true, m_VPB.ActualPosition(this), &IOFBFlng_Rmt(0)->PhD(), &IOFBFlng_Rmt(1)->PhD());
    FB.SetQmReqd(0.0);
    return true;
    }
  return false; 
  }

//--------------------------------------------------------------------------

void Valve_GP::Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg)
  {
  m_VPB.Ctrl_ConnIDStr(i, ID, Tg);
  }

//--------------------------------------------------------------------------

bool Valve_GP::IsXRefListActive() { return m_VPB.On()!=0; }

//---------------------------------------------------------------------------

int Valve_GP::UpdateXRefLists(CXRefBuildResults & Results)
  {
  if (1)//m_VPB.On())
    {
    FnMngrClear();
    int FunctNo = 0;
    Strng S;
    S.Set("%s.%s", Tag(), "Posn.Reqd");
    int RetCode1 = m_PosVar.UpdateXRef(NULL, 0, 1, FunctNo, this, CIOId_Pos, S(), S(), "Value:Position", Results);
    FnMngrTryUpdateXRefLists(Results);
    }

  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void Valve_GP::UnlinkAllXRefs()
  {
  FnMngrClear();
  FnMngrTryUnlinkAllXRefs();

  m_PosVar.UnlinkXRefs();

  CNodeXRefMngr::UnlinkAllXRefs();
  };
//--------------------------------------------------------------------------

void Valve_GP::EvalCtrlActions()
  {
  if (!m_FRB.On())
    {
    if (NoCIOs()>0)
      {
      double D;
      if (m_PosVar.GetValue(D))
        m_VPB.SetReqdPosition(D);
      }

    m_VPB.EvalCtrlActions(this);
    }
  }

//---------------------------------------------------------------------------

dword Valve_GP::ModelStatus()
  {
  dword Status=MN_BstRes::ModelStatus();
  if (!gs_Exec.Stopped() && SolveDynamicMethod())
    {
    Status |= ((m_VPB.On() && m_VPB.ActualPosition(this)>0.0) ? FNS_On : FNS_Off);
    }
  return Status;
  }

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Valve - Non Return(1)
#X:#h<General Description>#nThe non-return valve allows the material to flow
in one direction only. This valve can be configured by selecting the valve
flow equation, that best matches the operation of the actual valve. The valve
is always connected from an outlet point of one pipe to an inlet point of
another. It is commonly used to regulate the flow of liquids, slurries and
gases. The valve only has one inlet and one outlet point.
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
In           Input          1              1#n
Out          Output         1              1#n
#a#n
#h<Variables to be supplied by the user>#n
#i<Z_Reqd> : Datum.#n
#i<Eqn.Name> : This allows the user to select the model from the #x<valve
flow equations> which calculates the pressure drop across the valve.
The associated variables must be confiqured.#n
#n#n
#h<Associated variables>#n
#i<Qm> : The mass flow of the material through the unit.#n
#i<Qv> : The volumetric flowrate of the material through the unit.#n
#i<NQv> : The normalized volumetric flowrate, ie the flowrate measured at STP,
of the material within the unit. This will only differ from "Qv" when pressurized
gases are being conveyed, since the volume of a gas is proportional to its pressure.#n
#i<T> : Temperature of the material within the unit.#n
#i<Rho> : The density of the material within the unit.#n
#i<Z> : The height of the unit, in relation to the common datum.#n
#i<SrcIO> : The tag identifying the source (upstream) pipe to which the input of the unit is connected.#n
#i<DstIO> : The tag identifying the destination (downstream) pipe to which the output of the unit is connected.#n
#i<Pi> : The pressure at the inlet of the unit.#n
#i<dP> : The total pressure drop across the unit.#n
#i<Po> : The pressure at the outlet of the unit.#n
#n
#n#h<Other>#n
Default model prefix:VNR#n
Short name:Valve_NR#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

static double Drw_Valve_NR[] = { DD_Poly, -4,3, -4,-3, 0,0,
                        DD_Poly, 4,3, 4,-3, -4,3,
                        DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Valve_NR, "Valve_NR", "1", Drw_Valve_NR, "Valve", "VNR", TOC_ALL|TOC_GRP_TRANSFER|TOC_STD_KENWALT,
                    "Process:Piping:Valve - Non Return(1)",
                    "Non return valve")

Valve_NR::Valve_NR(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
   MN_BstRes(pClass_, TagIn, pAttach, eAttach)//,
   //m_VPB("VL_Basic", this)
  {
  AttachIOAreas(TwoIOAreaList, &ChkValveGroup, &NullFlwGroup);
  m_FEP.AssignFlwEqnGroup(ChkValveGroup, ChkValveGroup.Default()/*"VE_Line"*/, this);
  //Initialise();
  fForceOpen=false;
  dPressBoost_Rqd=-10.0;
  }

//--------------------------------------------------------------------------
/*
void Valve_NR::Initialise()
  {
  FB.Initialise();
  }
*/
//--------------------------------------------------------------------------

void Valve_NR::BuildDataDefn(DataDefnBlk &DDB)
  {
  static DDBValueLst DDB1[]={
    {BRPM_None, "None"},
    {BRPM_Drop, "Drop"},
    {BRPM_Fixed, "Fixed"},
    {BRPM_Atmos, "Atmos"},
    {0}};
  DDB.BeginStruct(this);
  if (SolveDirectMethod())
    {
    DDB.Text    ("");
    DDB.Text    ("Requirements");
    DDB.Byte    ("PressMethod",     "",     DC_,      "",      xidPressMethod,  this, isParm|SetOnChange, DDB1);
    DDB.Visibility(SHM_All, iPressMethod==BRPM_Drop);
    DDB.Double  ("PressDropRqd",    "PDrop", DC_DP,   "kPa",   xidPDropRqd,     this, isParm);
    DDB.Visibility(SHM_All, iPressMethod==BRPM_Fixed);
    DDB.Double  ("OutletPressRqd",  "PRqd", DC_P,     "kPag",  xidPoutRqd,      this, isParm|NAN_OK);
    DDB.Visibility();
    m_VLE.Add_OnOff(DDB);
    DDB.Text    ("");
    DDB.Text    ("Results");
    DDB.Double  ("Mass_Flow",       "Qm",   DC_Qm,    "kg/s",  xidQm,           this, isResult|noFile|noSnap);
    DDB.Double  ("TemperatureIn",   "Ti",   DC_T,     "C",     xidTempIn,       this, isResult|noFile|noSnap);
    DDB.Double  ("TemperatureOut",  "To",   DC_T,     "C",     xidTempOut,      this, isResult|noFile|noSnap);
    DDB.Double  ("PressIn",         "Pi",   DC_P,     "kPag",  xidPIn,          this, isResult|isParmConstruct);
    DDB.Double  ("Press_Change",    "dP",   DC_DP,    "kPa",   xiddP,           this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("PressOut",        "Po",   DC_P,     "kPag",  xidPOut,         this, isResult|isParmConstruct);
    DDB.Double  ("VapourFracIn",    "Vfi",  DC_Frac,  "%",     xidVapFracIn,    this, isResult|noFile|noSnap);
    DDB.Double  ("VapourFracOut",   "Vfo",  DC_Frac,  "%",     xidVapFracOut,   this, isResult|noFile|noSnap);
    //DDB.Double  ("SatT",            "",     DC_T,     "C",     &dSatTOut,       this, isResult|noFile|noSnap);
    //DDB.Double  ("SatP",            "",     DC_P,     "kPag",  &dSatPOut,       this, isResult|noFile|noSnap|InitHidden);
    //DDB.Double  ("H2O_Frac",        "",     DC_Frac,  "%",     &dH20Frac,       this, isResult|noFile|noSnap|InitHidden);*/
    if (0)
      {
      DDB.Text    ("");
      DDB.Double  ("Vol_Flow",        "Qv",   DC_Qv,    "L/s",   xidQv,           this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("NVol_Flow",       "NQv",  DC_NQv,   "NL/s",  xidNQv,          this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Temperature",     "T",    DC_T,     "C",     xidTemp,         this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Pressure",        "P",    DC_P,     "kPag",  xidPMean,        this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Density",         "Rho",  DC_Rho,   "kg/m^3",xidRho,          this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Vap_Frac",        "Vf",   DC_Frac,  "%",     xidVapFrac,      this, isResult|noFile|noSnap|InitHidden);
      }
    DDB.Double  ("PrevPOut",        "",     DC_P,     "kPag",  &dPout,          this, isParm|noView);
    DDB.Double  ("PrevDuty",        "",     DC_Pwr,   "kW",    &dDuty,          this, isParm|noView);
    }
  else
    {
    DDB.Text    ("");
    DDB.Double  ("Mass_Flow_Est",   "QmEst",DC_Qm,    "kg/s",  xidQmEst,        this, isResult|noFile|noSnap);
    DDB.Double  ("Mass_Flow",       "Qm",   DC_Qm,    "kg/s",  xidQm,           this, isResult|0);
    DDB.Double  ("Vol_Flow",        "Qv",   DC_Qv,    "L/s",   xidQv,           this, isResult|noFile|noSnap);
    DDB.Double  ("NVol_Flow",       "NQv",  DC_NQv,   "NL/s",  xidNQv,          this, isResult|noFile|noSnap);
    DDB.Double  ("",                "QmPrvPB",DC_Qm,  "kg/s",  xidFEQmPrvPB,    this, isResult|InitHidden);
    //DDB.Double  ("",                "QmPB", DC_Qm,    "kg/s",  xidFEQmMemPB,    this, isResult|InitHidden);
    DDB.Double  ("Temperature",     "T",    DC_T,     "C",     xidTemp,         this, isResult|noFile|noSnap);
    //DDB.Double  ("Pressure",        "P",    DC_P,     "kPag",  xidPMean,        this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("Density",         "Rho",  DC_Rho,   "kg/m^3",xidRho,          this, isResult|noFile|noSnap);
    DDB.Double  ("NDensity",        "NRho", DC_Rho,   "kg/m^3",xidNRho,         this, isResult|noFile|noSnap);
    BuildDataDefnElevation(DDB);
    DDB.Text    ("");
    //DDB.Double  ("VapourFrac",      "Vf",   DC_Frac,  "%",     xidVapFrac,      this, isResult|noFile|noSnap);
    //DDB.Double  ("mVapourFrac",     "mVf",  DC_Frac,  "%",     xidMoleVapFrac,  this, isResult|noFile|noSnap);
    //DDB.Text   ("Pressure Audit", Marker|MarkerClosed);
    DDB.Double  ("Press_InEst",     "PiEst",DC_P,     "kPag",  xidPInEst,       this, isResult|isParmConstruct|InitHidden);
    DDB.Double  ("Press_Input",     "Pi",   DC_P,     "kPag",  xidPIn,          this, isResult);
    DDB.Double  ("Press_Change",    "dP",   DC_DP,    "kPa",   xiddP,           this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("Press_Output",    "Po",   DC_P,     "kPag",  xidPOut,         this, isResult);
    DDB.Double  ("Press_OutEst",    "PoEst",DC_P,     "kPag",  xidPOutEst,      this, isResult|isParmConstruct|InitHidden);
    DDB.Double  ("",                "dPbX", DC_DP,    "kPa",   xidFEDPbX,       this, isResult|isParmConstruct|InitHidden);
    }

  DDB.Text   ("");
  BuildDataDefnShowIOs(DDB);

  if (SolveDirectMethod())
    {
    m_VLE.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);
    }
  else
    {
    static DDBValueLst DDB0[]={
      {0,  "Open"},
      {1,  "Closed"},
      {0}};
    DDB.Page   ("Parms");
    DDB.Text   ("");
    DDB.CheckBox("ForceOpen",   "",       DC_,     "",      &fForceOpen   ,this, isParm|SetOnChange/*|InitHidden*/);
    if (!fForceOpen)
      {
      DDB.Double ("OpenP",        "OpenP",  DC_P,    "kPa",   xidUDFOpenP   ,this, isParm);
      DDB.Long   ("UDFActionCount", "",     DC_,     "",      xidUDFActCnt  ,this, 0|InitHidden);
      DDB.Bool   ("State",        "",       DC_,     "",      xidUDFState   ,this, 0, DDB0);
      }
    //DDB.Double ("TempDrop",     "",       DC_dT,   "C",     &dTempDrop,    this, isParm|NAN_OK);
    //DDB.Text   ("");
    //VLE.Add_OnOff(DDB);

    m_FEP.BuildDataDefn(FBDDOpt_WithEqn|FBDDOpt_WithDPSpec, DDB, this, NULL, -1, DDB_RqdPage);
    if (NoFlwIOs()>0 && !DDB.ForFileSnpScn() && (!DDB.ForView() || dbgfile()))
      IOFB(0,0)->BuildDataDefn(FBDDOpt_WithData, DDB, this, "In", 1);

    //DDB.Text("");
    //m_VPB.BuildDataDefn(DDB, this, "Posn", 1);

    //VLE.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);

    if (NoFlwIOs()>0)
      DDB.Object(IOConduit(0), this);
    }

  DDB.EndStruct();
  }

// --------------------------------------------------------------------------

flag Valve_NR::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_BstRes::DataXchg(DCB))
    return 1;
  if (m_FEP.DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidUDFOpenP:
      if (NoFlwIOs()>0)
        {
        if (DCB.rD)
          IOFB(0,0)->SetUDFOpenP(*DCB.rD);
        DCB.D=IOFB(0,0)->UDFOpenP();
        }
      else
        DCB.D=0.0;
      return 1;
    case xidUDFActCnt:
      if (NoFlwIOs()>0)
        {
        if (DCB.rL)
          IOFB(0,0)->SetUDFActionCount(*DCB.rL);
        DCB.L=IOFB(0,0)->UDFActionCount();
        }
      else
        DCB.L=0;
      return 1;
    case xidUDFState:
      if (NoFlwIOs()>0)
        {
        DCB.B=IOFB(0,0)->UDFIsClosed();
        }
      else
        DCB.B=0;
      return 1;
    }

  return 0;
  }

//--------------------------------------------------------------------------

flag Valve_NR::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_BstRes::ValidateData(VDB);
  if (!m_FEP.ValidateData(VDB))
    OK=False;
  return OK;
  }

//--------------------------------------------------------------------------

flag Valve_NR::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  //MdlAction {MAT_NULL, MAT_State, MAT_Value};
  MdlAction M0(0, MAT_State, !fForceOpen, "Force Open", 1);
  MdlAction M1(1, MAT_State, fForceOpen,  "Operate", 0);

  Acts.SetSize(0);
  Acts.SetAtGrow(0, M0);
  Acts.SetAtGrow(1, M1);
  return True;
  }

//--------------------------------------------------------------------------

flag Valve_NR::SetModelAction(Strng & Tag, MdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
      fForceOpen=Act.iValue!=0;
//      m_VPB.SetOn(Act.iValue);
      break;
    case 1:
      fForceOpen=Act.iValue!=0;
//      m_VPB.SetOn(Act.iValue);
      break;
//    case 2:
//      m_VPB.SetManualPosition(Act.dValue*0.01);
//      break;
    }
  return true;
  }

//--------------------------------------------------------------------------

void Valve_NR::ConfigureJoins()
  {
  //double OpenPress = 1.0, CloseP=-1.0;
  //SetUniDirectionalFlow(True, 1.0, -1.0);
  MN_BstRes::ConfigureJoins();
  if (NoFlwIOs()>0)
    {
    if (fForceOpen)
      IOFB(0,0)->SetBiDirectionalFlow();
    else
      IOFB(0,0)->SetUniDirectionalFlow(True, IOFB(0,0)->UDFOpenP(), 0.0);//FB.UDFCloseP());//OpenPress, CloseP);
    }
  }

//--------------------------------------------------------------------------

flag Valve_NR::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  if (IONo>0)
    return False;
  int IOi= (IO_In(0) ? 0 : 1);
  return IOFB(0,FE)->EvaluateFlwEqn(Task, pProps, true, false, 1.0, &IOFBFlng_Rmt(0)->PhD(), &IOFBFlng_Rmt(1)->PhD());
  }

//---------------------------------------------------------------------------

dword Valve_NR::ModelStatus()
  {
  dword Status=MN_BstRes::ModelStatus();
  if (SolveDynamicMethod())
    {
    if (NoFlwIOs() && IOFB(0,0)->UDFIsClosed())
      Status |= FNS_Off;
    //if (NoFlwIOs())
    //  Status |= (!IOFB(0,0)->UDFIsClosed() ? FNS_On : FNS_Off);
    }
  return Status;
  }

//==========================================================================
//
//
//
//==========================================================================
