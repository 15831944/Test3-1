/================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#include "sc_defs.h"
#include "datacnvs.h"
#include "flwnode.h"

#define  __PRIREFORM_CPP
#include "prireform.h"

#include "dbgmngr.h"
#include "debugnew.h"

//#include "optoff.h"

#define dbgModel          01

#if (0 && dbgModel  && !defined(_RELEASE))
#define CHECKTEMPS(h, c, x) if (h.To>h.Ti+x || h.To<c.Ti-x || c.To>h.Ti+x || c.To<c.Ti-x) DoBreak();
#else
#define CHECKTEMPS(h, c, x)
#endif

#if dbgModel
static CDbgMngr  dbgPriReform    ("Petro1",  "PriReform");
#endif

#define BOX(x,y,w,h) x,y, x,y+h, x+w,y+h, x+w,y, x,y

XID xidBurnCount = ModelXID(1001);

//===========================================================================
//
//
//
//===========================================================================

const byte ioidAir     = 0;
const byte ioidFuel    = 1;
const byte ioidExhaust = 2;
const byte ioidFalseAir   = 3;
const byte ioidGasIn   = 4;
const byte ioidGasOut  = 5;

const IDMask m_PriIOMask=Id_2_Mask(ioidAir)|
                      Id_2_Mask(ioidFuel)|
                      Id_2_Mask(ioidExhaust)|
                      Id_2_Mask(ioidFalseAir);

const IDMask m_SecIOMask=Id_2_Mask(ioidGasIn)|
                      Id_2_Mask(ioidGasOut);//|
                      //Id_2_Mask(ioidGasVent);

static IOAreaRec m_PriReformIOAreaList[] =
  { {"",              "AirIn" ,  ioidAir,       LIO_In ,  nc_MLnk, 1, 20, IOGRP(1)|IOPipeEntry, 1.0f, {0,0,0,0}, NULL, &ValveGroup},//, "Side[iP].I"},
    {"",              "FuelIn" , ioidFuel ,     LIO_In ,  nc_MLnk, 1, 20, IOGRP(1)|IOPipeEntry, 1.0f, {0,0,0,0}, NULL, &ValveGroup},//, "Side[iP].I"},
    {"",              "Exhaust", ioidExhaust,   LIO_Out,  nc_MLnk, 1,  5, IOGRP(1)|IOPipeEntry, 1.0f, {0,0,0,0}, NULL, &PipeEntryGroup},//, "Side[iP].O"},
    {"",              "FalseAir",ioidFalseAir,  LIO_In ,  nc_MLnk, 1,  5, IOGRP(1)|IOPipeEntry, 1.0f, {0,0,0,0}, NULL, &PipeEntryGroup},//, "Side[iP].O"},
    {"",              "GasIn" ,  ioidGasIn ,    LIO_In ,  nc_MLnk, 1, 20, IOGRP(2)|IOPipeEntry, 1.0f, {0,0,0,0}, NULL, &PipeEntryGroup},//, "Side[iS].I"},
    {"",              "GasOut",  ioidGasOut,    LIO_Out,  nc_MLnk, 1,  5, IOGRP(2)|IOPipeEntry, 1.0f, {0,0,0,0}, NULL, &PipeEntryGroup},//, "Side[iS].O"},
    {NULL}};


static double Drw_m_PriReform[] = {DD_Poly, -6,-12, -6,12, 6,12, 6,-12, -6,-12,
                            //DD_Poly, -5,-6, -5,-3, -4,3, -3,-3, -2,3, -1,-3, 0,3,
                            //         1,-3, 2,3, 3,-3, 4,3, 5,-3, 5,-6,
                            DD_End };

IMPLEMENT_MODELUNIT(PriReform, "PriReformer", "1", Drw_m_PriReform, "MixerSplitter", "PR",
                    TOC_DYNAMICFULL|TOC_USER,
                    "Primary Reformer(1)",
                    "Primary Reformer")

byte InitDetect[7] = {2,4,7,9,7,4,2};

PriReform::PriReform(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  FuelRB(this, true),
  GasRB(this, false),
  m_HotBox("HotBox", this, TOA_Embedded),
  //m_HotBoxPreset(NULL),
  m_HotBoxImg("PresetImg", this, TOA_Embedded),
  m_CombFeed("CombFeed", this, TOA_Embedded)

  //Shell("Shell", this, TOA_Embedded)
  {
  AttachClassInfo(nc_Process, m_PriReformIOAreaList);
  for (int b=0; b<84; b++)
    m_dBurnerOpen[b]=1;
  m_BurnCount=84;

  for (int r=0; r<7; r++)
    {
    m_Detect[r].m_bOn=1;
    m_Detect[r].m_iRow=r;
    m_Detect[r].m_iCol=InitDetect[r];
    }

  m_HotBox.SetClosed(True);
  m_HotBox.SetPreset(&m_HotBoxPreset, &m_HotBoxImg);
  //m_HotBox.SetAllowPreset(TRUE);

  EHX.Open(&CEHX_LossPerQmClass);

  FuelRB.Open();
  GasRB.Open();

  m_dCatMass=15000;
  m_dCatTemp=StdTemp();
  m_dCatCp=1.1;
  m_dCatHeatIn=0.0;
  m_dCatHeat2Gas=0.0;
  m_dCatTempDeriv=0;
  m_dCatUA=0;

  m_States.Add(&m_dCatTemp, &m_dCatTempDeriv, &m_dCatTemp);
  }

//--------------------------------------------------------------------------

PriReform::~PriReform()
  {
  }

//--------------------------------------------------------------------------

void PriReform::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, NULL, NULL, DDB_NoPage);
  DDB.Text(" ");

  EHX.Add_OnOff(DDB, False);

  DDB.Visibility(SM_DynBoth|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Visibility();

  DDEF_Flags OldVis=DDB.GetVisibility();

  DDB.SetVisibility(OldVis);

  DDB.Text(" ");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  DDB.Text(" ");

  for (int r=0; r<7; r++)
    {
    Strng Tg;
    Tg.Set("Detector-%i", r);
    if (DDB.BeginStruct(this, Tg()))
      {
      DDB.CheckBoxBtn("On",  "", DC_, "", &m_Detect[r].m_bOn, this, isParm); 
      DDB.Byte       ("Row", "", DC_, "", &m_Detect[r].m_iRow, this, isParm); 
      DDB.Byte       ("Col", "", DC_, "", &m_Detect[r].m_iCol, this, isParm); 
      }
    DDB.EndStruct();
    }

  DDB.Double("BurnersOn", "", DC_,    "", &m_BurnCount,     this, 0);

  int b=0;
  for (r=0; r<7; r++)
    {
    Strng Tg;
    Tg.Set("Row-%c", 'A'+r);
    DDB.BeginObject(this, Tg(), "BurnRow", NULL, DDB_OptPage, -1, MarkerClosed);//, Marker);
    for (int c=0; c<12; c++)
      {
      Strng Tg;
      Tg.Set("Burn-%i", c);

      DDB.Float(Tg(), "", DC_Frac, "%", &m_dBurnerOpen[b], this, isParm); 
      b++;
      }
    DDB.EndStruct();
    }
  
  if (DDB.BeginStruct(this, "Catalyst"))
    {
    DDB.Double("Mass",      "", DC_M,    "kg",       &m_dCatMass,     this, isParm);
    DDB.Double("Temp",      "", DC_T,    "dC",       &m_dCatTemp,     this, isParm);
    DDB.Double("Cp",        "", DC_CpMs,   "kJ/kg.C",  &m_dCatCp,       this, isParm);
    DDB.Double("HeatIn",    "", DC_Pwr,  "kW",       &m_dCatHeatIn,   this, isParm);
    DDB.Double("Heat2Gas",  "", DC_Pwr,  "kW",       &m_dCatHeat2Gas, this, isParm);
    DDB.Double("UA",        "", DC_UA,   "kW/K",     &m_dCatUA,       this, isParm);
    }
  DDB.EndStruct();

  DDB.Object(&m_HotBox, this, NULL, NULL, DDB_RqdPage);
  DDB.Object(&m_HotBoxImg, this, NULL, NULL, DDB_RqdPage);

  FuelRB.BuildDataDefn(DDB, "FuelRB", NULL, DDB_RqdPage, 1);
  GasRB.BuildDataDefn(DDB, "GasRB", NULL, DDB_RqdPage, 2);

  EHX.BuildDataDefn(DDB);

  DDB.EndStruct();
  }

//---------------------------------------------------------------------------

flag PriReform::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;

  if (m_HotBox.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==1 && FuelRB.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==2 && GasRB.DataXchg(DCB))
    return 1;

  return 0;
  }

//---------------------------------------------------------------------------

flag PriReform::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);

  for (int r=0; r<7; r++)
    {
    m_Detect[r].m_iRow = Range(byte(0), m_Detect[r].m_iRow, byte(7-1));
    m_Detect[r].m_iCol = Range(byte(0), m_Detect[r].m_iCol, byte(12-1));
    }

  return OK;
  }

//--------------------------------------------------------------------------

flag PriReform::InitialiseSolution()
  {
  flag OK=MdlNode::InitialiseSolution();

  m_States.RemoveAll();

  switch (SolveMethod())
    {
    case SM_Inline:
    case SM_Buffered:
      m_HotBox.SetStateAction(InlineIntegral() ? IE_SaveState : IE_Integrate);
      m_States.Add(&m_dCatTemp, &m_dCatTempDeriv, &m_dCatTemp);
      break;
    }

  return OK;
  }

//---------------------------------------------------------------------------

//void PriReform::StartSolution()
//  {
//  }

//--------------------------------------------------------------------------

static const IDMask HBMsk=Id_2_Mask(ioidAir)|Id_2_Mask(ioidFuel)|Id_2_Mask(ioidExhaust)|Id_2_Mask(ioidFalseAir);
static const IDMask GasMsk=Id_2_Mask(ioidGasIn)|Id_2_Mask(ioidGasOut);

Z_States PriReform::SetDatums(int Pass, int IOIn, double Zi, Z_States Z_State_Src)
  {
  CSetDatumsData SDD[]=
    {
      {HBMsk, &m_HotBox},
      {GasMsk, NULL},
      {0}
    };
  return SetDatums_Node(Pass, SDD, IOIn, Zi, Z_State_Src);
  };

//--------------------------------------------------------------------------

void PriReform::SetDatumsDone()
  {
  MdlNode::SetDatumsDone();
  SortSurgeIOData SDD[]=
    {
      {HBMsk, &m_HotBox, &m_HotBoxHgtOrd},
      {0}
    };
  SortSurgeIO(SDD);
  }

//--------------------------------------------------------------------------

flag PriReform::Set_Sizes()
  {
  return True;
  };

//--------------------------------------------------------------------------

void   PriReform::SetState(eScdMdlStateActs RqdState)
  {
  MdlNode::SetState(RqdState);
  m_HotBox.SetState(RqdState);

  //  int s;
  switch (RqdState)
    {
    case MSA_PBInit:
      break;
    case MSA_Empty:
      //m_HotBox.ZeroMass();
      //m_HotBox.ZeroDeriv();
      break;
    case MSA_PreSet:
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  //return 0.0;
  };

//---------------------------------------------------------------------------

void PriReform::PostConnect(int IONo)
  {
  MdlNode::PostConnect(IONo);

  }

//---------------------------------------------------------------------------

void PriReform::ConfigureJoins()
  {
  switch (NetMethod())
    {
    case NM_Dynamic:
      {
      Set_NJoins(2);
      for (int i=0; i<NoFlwIOs(); i++)
        if (IOId_Self(i)<=ioidFalseAir)
          SetIO_Open(i, 0, false, ESS_Allowed);
        else
          SetIO_Join(i, 1);
      break;
      }
    }
  }

//--------------------------------------------------------------------------

void PriReform::StartStep()
  {
  m_HBStart.AtStart(m_HotBox);
  
  m_BurnCount=0;
  for (int b=0; b<84; b++)
    m_BurnCount+=m_dBurnerOpen[b];

  for (int d=0; d<7; d++)
    {
    b=m_Detect[d].m_iRow*12+m_Detect[d].m_iCol;
    m_Detect[d].m_bOn=(m_dBurnerOpen[b]>0.001);
    }

  }

//--------------------------------------------------------------------------

void PriReform::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      break;
    default:;
      {
      //#if dbgModel
      //dbgpln("EvalJoinPressures:%s",FullObjTag());
      //#endif
      if (fDoDbgBrk)
        { int xxx=0; }

      IOP_RhoH_Info RhoHInfo(m_HotBox);
      double Pm=ContainerMeanPress(RhoHInfo, 0);
      Set_JoinP(0, Pm);
      for (int i=0; (i<NoFlwIOs()) && (IOId_Self(i)<=ioidFalseAir); i++)
        Set_IOP_RhoH_Self(i,Pm,RhoHInfo);
      
      Pm=JoinP(1);
      for (i=0; i<NoFlwIOs(); i++)
        if (IOId_Self(i)>=ioidGasIn && IOId_Self(i)<=ioidGasOut)
          Set_IOP_RhoH_Self(i,Pm,1.0,0.0);

      break;
      }
    }
  };

//--------------------------------------------------------------------------

CSpPropInfo* PriReform::IOGetNetProps(int i, double Qm)
  {
  if (IOId_Self(i)<=ioidFalseAir)
    DoIOGetNetProps_Surge(i, Qm);
  return IONetProps(i);//MdlNode::IOGetNetProps(i, Qm);
  }

//--------------------------------------------------------------------------

flag PriReform::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  int id=IOId_Self(IONo);
  switch (IOId_Self(IONo))
    {
    case ioidFuel:
      return IOFB(IONo,FE)->EvaluateFlwEqn(Task, pProps, true, false, m_BurnCount/84.0, &IOFBFlng_Rmt(IONo)->PhD(), NULL);
    case ioidAir:
    case ioidFalseAir:
    default:
      return IOFB(IONo,FE)->EvaluateFlwEqn(Task, pProps, true, false, 1.0, &IOFBFlng_Rmt(IONo)->PhD(), NULL);
    }
  };

//--------------------------------------------------------------------------

void PriReform::EvalProductsSurge(long JoinMask)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      break;
    case SM_Inline:
    case SM_Buffered:
      {
      SigmaQInPMin(m_CombFeed, som_ALL, HBMsk);
      EvalProducts_SurgeLevel(m_HotBoxHgtOrd, &m_CombFeed);
      }
    }
  }

//--------------------------------------------------------------------------

void PriReform::EvalProducts(long JoinMask)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      break;
    case SM_Inline:
    case SM_Buffered:
      {
      int In  = (IO_In(ioidGasIn) ? ioidGasIn : ioidGasOut);
      int Out = (IO_In(ioidGasIn) ? ioidGasOut: ioidGasIn );

      SpConduit &Fi=*IOConduit(In);
      SpConduit &Fo=*IOConduit(Out);

      if (IO_Zero(In))
        {
        Fi.QSetTraceMass();//QZero();
        Fo.QSetTraceMass();//QZero();
        IOFB(In,0)->SetQm(0.0);
        }
      else
        {
        Fo.QCopy(Fi);
        Fo.SetTemp(m_dCatTemp);
        m_dCatHeat2Gas=Fo.totHf()-Fi.totHf();
       
#pragma chCHECKIT(this EvalProducts needs to be checked)
        GasRB.EvalProducts(Fo, m_dCatTemp);//, false);
        m_dCatHeat2Gas+=GasRB.HfSumRct();

        double x1=GasRB.HsSumHX();
        double x2=GasRB.HfSumRct();
        double x3=0.0;

//        IOFB(In,0)->SetQm(Sign(IOFB(In,0)->GetQm())*Fo.QMass(som_ALL));
//        IOFB(In,0)->EvalProducts(IOP_Flng(In), IOP_Flng(Out), Fo);//);
//        //if (Valid(dTempDrop))
//        //  Fo.SetTemp(Fi.Temp()-dTempDrop);
//       
        }
      }
      break;
    }
  }

//--------------------------------------------------------------------------

void PriReform::EvalDerivs(long JoinMask)
  {
  m_HotBox.ZeroDeriv();
  m_dCatTempDeriv=0;
  if (!IntegralDone() && !GetActiveHold())
    {
    m_dCatHeatIn=(m_HotBox.Temp()-m_dCatTemp)*m_dCatUA;
    m_dCatTempDeriv=(m_dCatHeatIn-m_dCatHeat2Gas)/(m_dCatMass*m_dCatCp);

    for (int i = 0; i < NoFlwIOs(); i++)
      if (IOId_Self(i)<=ioidFalseAir)
        m_HotBox.AddDeriv(*IOConduit(i), IOSign(i));
    m_HotBox.SetDuty(m_HotBox.Duty()-m_dCatHeatIn);
    }
  };

//--------------------------------------------------------------------------

void PriReform::ODEOperate(CODEDataBlock & ODB)
  {
  switch (ODB.m_Cmd)
    {
    case eStateAdvance:
    case eStateConverge:
    case eStateLoad:
    case eStateDiskLoad:
      {
      m_HotBox.ODEOperate(ODB);
      ODB.m_pFn(ODB, m_States);
   
      if (0)
        dbgpln("ODE:Int  : T:%+10.3f = %10.3f P:%+10.3f = %10.3f M:%+13.6f = %13.6f L:%+10.3f = %10.3f %s.m_HotBox", 
          m_HBStart.DTemp(m_HotBox), m_HotBox.Temp(),
          m_HBStart.DPress(m_HotBox), m_HotBox.Press(),
          m_HBStart.DMass(m_HotBox), m_HotBox.Mass(),
          m_HBStart.DLevel(m_HotBox)*100, m_HotBox.Level()*100, FullObjTag()); 
      
      ConvergeStates(CConvergeStateBlk(ODB));
    
      if (0)
        dbgpln("ODE:Chk  : T:%+10.3f = %10.3f P:%+10.3f = %10.3f M:%+13.6f = %13.6f L:%+10.3f = %10.3f %s.m_HotBox", 
          m_HBStart.DTemp(m_HotBox), m_HotBox.Temp(),
          m_HBStart.DPress(m_HotBox), m_HotBox.Press(),
          m_HBStart.DMass(m_HotBox), m_HotBox.Mass(),
          m_HBStart.DLevel(m_HotBox)*100, m_HotBox.Level()*100, FullObjTag()); 
      break;
      }
    case eStateFixDV:
    case eStateTest:
    case eStateSave:
    case eStateDiskSave:
      {
      m_HotBox.ODEOperate(ODB);
      ODB.m_pFn(ODB, m_States);
      break;
      }
    }
  };

//--------------------------------------------------------------------------

/*#F:This is called once, after the iteration process and allows the discreet
manipulation of the m_HotBox contained by the surge unit.*/

void PriReform::EvalDiscrete()
  {
  }

//---------------------------------------------------------------------------

flag PriReform::EvalPressureSens()
  {

  m_HotBox.SetHoldVentExpand();
  ContStartInfo Start(m_HotBox);
  SetStaticHeadSensitivities(0, m_HotBox, Start);

  // Must be closed and have m_HotBox
  if (!m_HotBox.Closed() || m_HotBox.Mass()< 1.0e-6)
    {
    ZeroVLSensitivities(0);
    m_HotBox.ClrHoldVentExpand();
    return True;
    }

  SetVLSensitivities(0, m_HotBox, Start, true);
  // Flash to Equilibrium
  SetEquilibratingV2L(0, m_HotBox, Start);

  RestoreContents(m_HotBox, Start);

  m_HotBox.ClrHoldVentExpand();

  return true;//False;
  };

//---------------------------------------------------------------------------

void PriReform::ApplyDerivs(double dTime, flag DoDbg)
  {
  if (GetActiveHold())
    return;

  m_HotBox.ApplyDerivs(dTime, true);
  }

//--------------------------------------------------------------------------

void PriReform::ConvergeStates(CConvergeStateBlk &CSB)
  {
  ASSERT(SolveDynamicMethod());

  flag Ignition=0;
  for (int d=0; d<7 && !Ignition; d++)
    Ignition=m_Detect[d].m_bOn;

  if (FuelRB() && Ignition)
    {
    #if dbgModel
    if (dbgPriReform())
      dbgpln("PriReform::ConvergeStates - RB %s", FullObjTag());
    #endif
    m_HotBox.SetMeanResTimeCalcsReqd(true);
    FuelRB.ConvergeStates(CSB, m_HotBox, m_HotBox.MeanResTimes());
    #if dbgModel
    if (dbgPriReform())
      dbgpln("-------------------------------------- ");
    #endif
    };

//  if (!IntegralDone() && !GetActiveHold())
//    VLE.QVFlash(m_HotBox, 0.0, VLEF_Null);//, NULL, NULL, NULL);
  }

//===========================================================================

void PriReform::ClosureInfo()
  {
  if (NoFlwIOs()>0)
    {
    MdlNode::ClosureInfo();
    if (m_Closure.DoFlows())
      {
      CClosureInfo &CI=m_Closure[0];
      if (EHX.Enabled())
        CI.m_EHXPowerIn += EHX.HeatFlow();
      }
    }
  };

//--------------------------------------------------------------------------

dword PriReform::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();

  if (NoFlwIOs())
    {
    int ioP=IOWithId_Self(ioidAir);
    int ioS=IOWithId_Self(ioidGasIn);

    double FlwP=ioP>=0 ? IOQm_In(ioP) : 0.0;
    double FlwS=ioS>=0 ? IOQm_In(ioS) : 0.0;
    int HasFlw=(fabs(FlwP)>UsableMass && fabs(FlwS)>UsableMass);
    Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
    }
  return Status;
  };

//===========================================================================




