//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "datacnvs.h"
#include "flwnode.h"
#define  __SHLTUBE_CPP
#include "shltube.h"
#include "dbgmngr.h"
//#include "optoff.h"

#define dbgHeatX           WITHDEBUG

#if (0 && dbgHeatX && !defined(_RELEASE))
#define CHECKTEMPS(h, c, x) if (h.To>h.Ti+x || h.To<c.Ti-x || c.To>h.Ti+x || c.To<c.Ti-x) DoBreak();
#else
#define CHECKTEMPS(h, c, x)
#endif

#if dbgHeatX
static CDbgMngr  dbgHeater   ("HeatXChg",  "ShellTube");
#endif

#define ForceSteam 0

static MInitialiseTest InitTest(&CHXShlTubeClass);
SPECIEBLK_L(InitTest, H2OLiq, "H2O(l)", true);
SPECIEBLK_V(InitTest, H2OVap, "H2O(g)", true);

#define BOX(x,y,w,h) x,y, x,y+h, x+w,y+h, x+w,y, x,y

// ==========================================================================
//
//
//
// ==========================================================================

XID xidHXRqdOpMode = ModelXID(1000);
XID xidHXSd_dT     = ModelXID(1001);

//==========================================================================
/*#D:#T:Shell & Tube Heat Exchanger(1)
#X:#h<General Description>#n
The Shell and Tube Heat Exchanger is used to transfer energy from one stream to
another.  It is primarily used to transfer latent heat from a condensing vapour
stream, currently only steam, to a liquor stream.  The liquor stream may contain
liquids and solids.  The unit may also be used for sensible heat exchange between
two fluids without any phase changes.

#nThere are two operational modes for the Shell and Tube Heat Exchanger,
#na) as a stand-alone unit or
#nb) as part of a Flash Train.
#nThe operational mode is decided by the overall configuration of the flowsheet in
which the unit is located.  If the heat exchanger is correctly connected to other
units such as Barometric Condensers, Heat Exchangers and Flash Tanks, the model may
become part of the entire Flash Train.  The user does not have to specify that the
unit is part of the Flash Train, SysCAD will do this automatically.  Refer to Flash
Train for the rules governing this behaviour.

#nIf the heat exchanger is inserted as part of a Flash Train, see Flash Train for a
description of the theory and variables.  This documentation will only discuss the
variables for a 'stand alone' heat exchanger.

#nAn Environmental Heat Loss may be included in the unit.  This allows the user to
specify a heat loss, or gain, between the unit and the environment.
#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
liquor in    Input          1              20#n
steam        Input          1              20#n
liquor out   Output         1              1#n
condensate   Output         1              1#n
vent         Output         0              1#n
#a#n
#n#h<Variables to be supplied by the user>#n
#i<EnvironHX> : This can be used to switch EHX Section (Environmental Heat Exchanger).
If this box is checked then the associated page, EHX may become visible and may be
configured. Note: The user does not have to configure an environmental heat exchange,
even if this block is checked.#n
#i<EHX.Model> : If there is heat transfer with the environment, this determines the model
to be used.#n
#i<QmVentRqd> : The required amount of non-condensables or steam which is lost to the
vent.  Note The unit calculates  the amount of steam that can be condensed and sends
the excess steam to the vent.  If this number is greater than the required steam loss,
the unit will send the calculated amount of steam to the vent, and not the user required
flow.#n
#i<HTC> : The Overall Heat Transfer Coefficient#n
#i<Area> : The area available for heat transfer.#n
#i<LMTDFact> : The LMTD factor of the heat exchanger.  This is usually 100%.#n
#n#n
#h<Associated variables>#n
#i<Duty> : The calculated duty of the heat exchanger#n
#i<LMTD> : The calculated Log Mean Temperature Difference.#n
#i<m_Pri.Mode> : The unit determines by  which mode the primary stream will transfer heat
and displays the mode here.#n
#i<m_Pri.Ti> : The temperature of the primary stream entering the unit.#n
#i<m_Pri.To> : Primary Outlet temperature#n
#i<m_Sec.Mode> : The unit determines by  which mode the secondary stream will transfer heat
and displays the mode here.#n
#i<m_Sec.Ti> : The temperature of the secondary stream entering the unit.#n
#i<m_Sec.To> : Secendary Outlet temperature#n
#n#n
#n#h<Assumptions & Limitations>#n
Given only the liquor streams then assume that all steam is condensing in order to calculate
the steam usage.
#n
#n#h<Other>#n
Default model prefix:ST#n
Short name:ShllTube#n
Model type:Unit#n
#G:Units
*/

// This Has define to be replaced with (puresat + bpe) if neccesary
#define BPE 0.0

//===========================================================================
//
//
//
//===========================================================================

static DDBValueLst DDBOpMode[]={
  {STHX_Inoperative      ,   "Inoperative"},
  {STHX_LiquorLiquor     ,   "Liquor/Liquor"},
  {STHX_LiquorGas        ,   "Liquor/Gas"},
  {STHX_GasGas           ,   "Gas/Gas"},
  {STHX_FullyCondensing  ,   "FullyCondensing"},
  //{STHX_PartialCondensing, "PartialCondensing"},
  {0}};

IMPLEMENT_HXB(CHXShlTube, "HXShTb", "", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT, "Shell & Tube HX", "Shell & Tube HeatExchangeBlock");
CHXShlTube::CHXShlTube(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CHXBlock(pClass_, Tag_, pAttach, eAttach),
  VLE(this, VLEF_QPFlash),
  m_FTC(dynamic_cast<FlwNode*>(pAttach))
  {
  //m_iRqdOpMode=STHX_FullyCondensing; this causes a bug!?!? it doesn't condense ANY steam
  m_iRqdOpMode=STHX_LiquorLiquor;
  m_iActOpMode=STHX_Inoperative;

  m_bExtraCond=(PrjFileVerNo()>81 ? 0 : 1);

  m_dDuty=0.0;
  m_dTheoryDuty=0.0;
  m_dLMTD=0.0;
  m_dLMTDFactor=1.0;
  m_dHTC=1.0;//0.980;
  m_dArea=10.0;//1.0;
  m_dApproachDT=0.1;

  m_fPriIsHot=0;

  m_dV2LQ=0.0;
  m_dV2LQZeroDuty=0.0;
  m_dV2LQVFrac=0.0;
  m_dV2LQLFrac=0.0;
  m_bLvlEffOn=true;
  m_dQmSteamCond=0.0;
  }

//--------------------------------------------------------------------------

CHXShlTube::~CHXShlTube()
  {
  };

//--------------------------------------------------------------------------

void CHXShlTube::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, NULL, NULL, DDB_RqdPage);
  DDB.Text("");
  DDB.Visibility();

  DDB.Byte  ("RqdOpMode",     "", DC_,     "",         xidHXRqdOpMode,    this, isParmStopped, DDBOpMode);
  DDB.Byte  ("ActOpMode",     "", DC_,     "",         &m_iActOpMode,     this, isResult, DDBOpMode);

  DDB.CheckBox("On",          "", DC_,     "",         &m_fOn,            this, isParm|SetOnChange);
  DDB.Double("HTC",           "", DC_HTC,  "kW/m^2.K", &m_dHTC,           this, isParm);
  DDB.Double("Area",          "", DC_Area, "m^2",      &m_dArea,          this, isParm);
  DDB.Visibility(NSHM_All, m_iRqdOpMode!=STHX_FullyCondensing);
  DDB.Double("ApproachDT",    "", DC_dT,   "C",        &m_dApproachDT,    this, isParm);

  DDB.Visibility(SM_Buffered|HM_All, m_bLvlEffOn);
  DDB.Byte("CrossSection",    "", DC_,     "",          &m_iXSect,        this, isParm|AffectsStruct, DDBXSect);
  DDB.Double ("MaxEffLvl",    "", DC_Frac, "%",         &m_dMxEffLvl,     this, isParm);
  DDB.Double ("MinEffLvl",    "", DC_Frac, "%",         &m_dMnEffLvl,     this, isParm);
  DDB.Double ("MaxEff",       "", DC_Frac, "%",         &m_dMxEff,        this, isParm);
  DDB.Double ("MinEff",       "", DC_Frac, "%",         &m_dMnEff,        this, isParm);
  DDB.Double ("Level",        "", DC_Frac, "%",         xidHXLevel,       this, isResult);
  DDB.Double ("NonCondense",  "", DC_Frac, "%",         xidHXNonCondenseFrac, this, isResult);
  DDB.Double ("EffectArea",   "", DC_Area, "m^2",       xidHXEffArea,     this, isResult|noFileAtAll);

  DDB.Visibility();
  DDB.Double("U*A",           "", DC_UA,   "kW/K",     xidHXEffUA,        this, isResult|noFileAtAll);
  DDB.Double("LMTD",          "", DC_dT,   "C",        &m_dLMTD,          this, isResult);
  DDB.Double("UALMTD",        "", DC_Pwr,  "kW",       &m_dTheoryDuty,    this, isResult);//Theoretical Duty
  DDB.Double("Duty",          "", DC_Pwr,  "kW",       &m_dDuty,          this, isResult);
  DDB.Double("TheorArea",     "", DC_Area, "m^2",      xidHXAreaUsed,     this, isResult/*|InitHidden*/|noFileAtAll);
  DDB.Double("LMTDFact",      "", DC_Frac, "%",        &m_dLMTDFactor,    this, (m_iSolveMode==HXM_LMTD) ? isParm : isResult|0);
  DDB.Text("");

  m_FTC.BuildDataDefn(DDB);

  DDB.Visibility(NSHM_All, m_iRqdOpMode==STHX_FullyCondensing);
  DDB.CheckBox("IgnoreAreaLimit", "", DC_, "",    &m_bExtraCond,     this, isParm);
  DDB.Double("NonCondVentFrac", "", DC_Frac, "%",      &m_dNonCondVentFrac,this, isParm);
  DDB.Visibility();
  DDB.Double("QmVentRqd",     "", DC_Qm,   "kg/s",     &m_QmVentRqd,      this, isParm);//steam vent required
  DDB.Visibility(NSHM_All, m_iRqdOpMode==STHX_FullyCondensing);
  DDB.Double("QmSteamCond",   "", DC_Qm,   "kg/s",     &m_dQmSteamCond,   this, isResult);
  DDB.Visibility();
  DDB.Text("");

  DDB.Page("..", DDB_OptPage);

  m_Pri.BuildDataDefn("m_Pri", DDB, this, true, !m_Pri.fConnected, true);
  if (!SolveBufferedMethod())
    m_Sec.BuildDataDefn("m_Sec", DDB, this, true, !m_Sec.fConnected, false);
  else
    m_Sec.BuildDataDefn("m_Sec", DDB, this, false, !m_Sec.fConnected, false);

  VLE.BuildDataDefn(DDB);

  DDB.Text("");
  DDB.EndStruct();
  }


//---------------------------------------------------------------------------

flag CHXShlTube::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidHXRqdOpMode:
      if (DCB.rB)
        {
        m_iRqdOpMode=*DCB.rB;
        if (m_iRqdOpMode==STHX_FullyCondensing)
          VLE.Open();
        else
          VLE.Close();
        }
      DCB.B=m_iRqdOpMode;
      return 1;
    default:
      break;
    }

  if (VLE.DataXchg(DCB))
    return 1;
  if (CHXBlock::DataXchg(DCB))
    return 1;

  return 0;
  }

//---------------------------------------------------------------------------

flag CHXShlTube::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=true;//MN_Surge::ValidateData(ValidateDataBlk & VDB);
  if (!VLE.ValidateData(VDB))
    OK=false;
  if (!CHXBlock::ValidateData(VDB))
    OK=false;
  return OK;
  }

//===========================================================================
//
//  Condensate Finder
//
//===========================================================================

class ShlTubeCondFnd : public MRootFinderBase
  {
  protected:
    CHXShlTube & Blk;
    CHXSide    & HotSd;
    CHXSide    & ColdSd;
    CVLEBase    & VLE;
    SpConduit  & QMem;
    static CToleranceBlock s_Tol;

  public:
    ShlTubeCondFnd(CHXShlTube* Blk_, CHXSide & HotSd_, CHXSide & ColdSd_, SpConduit &QMemS);
    ~ShlTubeCondFnd();
    LPCTSTR ObjTag() { return (LPCTSTR)Blk.FullObjTag(); };
    //void FlashIt(double Extent, CHXSide & HotSd, CHXSide & ColdSd);
    double Function(double x);
    double TheoryDuty();
  };

CToleranceBlock ShlTubeCondFnd::s_Tol(TBF_Both, "ShellTube:CondensFinder", 0.0, 1.0e-12);

//---------------------------------------------------------------------------

ShlTubeCondFnd::ShlTubeCondFnd(CHXShlTube* Blk_, CHXSide & HotSd_, CHXSide & ColdSd_, SpConduit &QMemS) :
  Blk(*Blk_), HotSd(HotSd_), ColdSd(ColdSd_),
  MRootFinderBase("BaseFnd", s_Tol),//1.0e-12),
  QMem(QMemS),
  VLE(Blk_->VLE)
  {
  HotSd.m_pCd->SetBadTempOK(true);
  ColdSd.m_pCd->SetBadTempOK(true);
  };

//---------------------------------------------------------------------------

ShlTubeCondFnd::~ShlTubeCondFnd()
  {
  HotSd.m_pCd->SetBadTempOK(false);
  ColdSd.m_pCd->SetBadTempOK(false);
  };

//---------------------------------------------------------------------------

double ShlTubeCondFnd::TheoryDuty()
  {
  CHECKTEMPS(HotSd, ColdSd, 1000);
  double TDuty=0.0;
  if (Blk.EffUA()>1.0e-6)
    {
    Blk.m_dLMTD=CHXBlock::LMTDSat(ColdSd.Ti, ColdSd.To, HotSd.SatT-BPE);
    TDuty = Blk.EffUA() * Blk.m_dLMTD * Blk.m_dLMTDFactor;
    }
  else
    {
    Blk.m_dLMTD=0.0;
    TDuty = 0.0;
    }

  return TDuty;
  };

//---------------------------------------------------------------------------

double ShlTubeCondFnd::Function(double Amount)
  {
  HotSd.m_pCd->QSetM(QMem, som_ALL, Amount, QMem.Press());
  HotSd.Ti = HotSd.m_pCd->Temp();
  HotSd.Ci = HotSd.m_pCd->totCp();
  HotSd.Cp = HotSd.m_pCd->msCp();
  HotSd.Qm = HotSd.m_pCd->QMass();
  HotSd.Hi = HotSd.m_pCd->totHf();

  double FTemp=Max(ColdSd.Ti, HotSd.SatT-BPE);
  double FPress=HotSd.Po;
  VLE.SetFlashVapFrac(*HotSd.m_pCd->Model(), FTemp, FPress, 0.0, VLEF_Null);
  HotSd.To=FTemp;//HotSd.m_pCd->Temp();
  Blk.m_dDuty=fabs(HotSd.m_pCd->totHf()-HotSd.Hi);

  ColdSd.m_pCd->Set_totHf(ColdSd.Hi+ColdSd.Sgn*Blk.m_dDuty+ColdSd.dHEnv+HotSd.dHEnv);
  ColdSd.To=ColdSd.m_pCd->Temp();
  // Temp Returned Could have small errors (due to convergence methods)
  // Check To is on the correct side of Ti.
  if (ColdSd.Sgn>0) // Heating Up
    ColdSd.To=Max(ColdSd.Ti+1.0e-3, ColdSd.To);
  else
    ColdSd.To=Min(ColdSd.Ti-1.0e-3, ColdSd.To);
  ColdSd.iMode=QPF_Sensible;
  HotSd.iMode=QPF_Condensing;
  ColdSd.Ho=ColdSd.m_pCd->totHf();
  HotSd.Ho=HotSd.m_pCd->totHf();
  CHECKTEMPS(HotSd, ColdSd,1000);

  double TDuty=GEZ(TheoryDuty());
  double Diff=fabs(Blk.m_dDuty)-TDuty;
  #if (dbgF)
  //if (fabs(Diff)<100.0)
  dbgpln("CF: X:%12.4f  Qp:%10.4f Qs:%10.4f dHp:%14.2f dHs:%14.4f ADty:%14.4f TDty:%14.4f Vf:%8.4f %s",
          x, m_Pri.Qm, m_Sec.Qm, m_Pri.Hi-m_Pri.Ho, m_Sec.Hi-m_Sec.Ho, fabs(Blk.m_dDuty), TDuty,
          m_Sec.m_pCd->MassFrac(som_Gas), Blk.FullObjTag());
  #endif

  return Diff;
  };

//===========================================================================
//
//  Condensate Finder
//
//===========================================================================

class ShlTubeCondLimitFnd : public MRootFinderBase
  {
  protected:
    CHXShlTube & ST;
    CHXSide    & HotSd;
    CHXSide    & ColdSd;
    static CToleranceBlock s_Tol;

  public:
    ShlTubeCondLimitFnd(CHXShlTube* Blk_, CHXSide & HotSd_, CHXSide & ColdSd_);
    ~ShlTubeCondLimitFnd() {};
    LPCTSTR ObjTag() { return (LPCTSTR)ST.FullObjTag(); };
    double Function(double x);
  };

CToleranceBlock ShlTubeCondLimitFnd::s_Tol(TBF_Both, "ShellTube:CondensLimitFinder", 0.0, 1.0e-12);

//---------------------------------------------------------------------------

ShlTubeCondLimitFnd::ShlTubeCondLimitFnd(CHXShlTube* Blk_, CHXSide & HotSd_, CHXSide & ColdSd_) :
  ST(*Blk_), HotSd(HotSd_), ColdSd(ColdSd_),
  MRootFinderBase("BaseFnd", s_Tol)
  {
  };

//---------------------------------------------------------------------------

double ShlTubeCondLimitFnd::Function(double x)
  {
  double HotSdToEst=Min(HotSd.Ti, HotSd.SatT-BPE);
  ST.VLE.SetFlashVapFrac(*HotSd.m_pCd->Model(), HotSdToEst, HotSd.Po, x, VLEF_Null);
  HotSd.To=HotSd.m_pCd->Temp();
  ST.m_dDuty=fabs(HotSd.m_pCd->totHf()-HotSd.Hi);
  HotSd.iMode=QPF_Condensing;
  HotSd.Ho=HotSd.m_pCd->totHf();

  ColdSd.m_pCd->SetPress(ColdSd.Po);
  ColdSd.m_pCd->Set_totHf(ColdSd.Hi+ColdSd.Sgn*ST.m_dDuty+ColdSd.dHEnv+HotSd.dHEnv);
  ColdSd.To=ColdSd.m_pCd->Temp();
  ColdSd.iMode=QPF_Sensible;
  ColdSd.Ho=ColdSd.m_pCd->totHf();

  double MxDuty = ST.EffUA()*ST.LMTDSat(ColdSd.Ti, ColdSd.To, HotSd.To);
  double Diff = ST.m_dDuty-MxDuty;
  return Diff;
  };

//===========================================================================
//
//
//
//===========================================================================

//const double ApproachDT=0.1;

void CHXShlTube::FindLimitingDuties(byte OpMode, CHXSide& HotSd, CHXSide& ColdSd, CHXSdInfo &HotI, CHXSdInfo &ColdI)
  {
  CHECKTEMPS(HotSd, ColdSd,1);

  double dMaxDuty1=1e30;
  if (OpMode==STHX_FullyCondensing)
    {
    // All Hot Condensing .. No Change in Cold
    //dMaxDuty1=EffUA()*LMTDSat(ColdSd.Ti, ColdSd.Ti, HotSd.SatT-BPE);
    double HotSdToEst=Min(HotSd.Ti, HotSd.SatT-BPE);
    HotSd.m_pCd->SetTemp(ColdSd.Ti);
    HotSd.MaxNfDuty=HotSd.Hi-HotSd.m_pCd->totHf();
    VLE.SetFlashVapFrac(*HotSd.m_pCd->Model(), HotSdToEst, HotSd.Po, 0.0, VLEF_Null);
    HotSd.MaxFlDuty=HotSd.Hi-HotSd.m_pCd->totHf();
    }
  else
    {
    HotSd.m_pCd->SetTemp(Min(ColdSd.Ti+m_dApproachDT, HotSd.Ti));
    HotSd.MaxNfDuty=HotSd.Hi-HotSd.m_pCd->totHf();
    HotSd.MaxFlDuty=HotSd.MaxNfDuty;
    }


  double dMaxDuty2=1e30;
  if (OpMode==STHX_FullyCondensing)
    {
    // All Cold To Max Temp .. No Hot Condensing
    ColdSd.m_pCd->SetTemp(HotSd.Ti);
    ColdSd.MaxNfDuty=ColdSd.m_pCd->totHf()-ColdSd.Hi;
    ColdSd.MaxFlDuty=ColdSd.MaxNfDuty;
    }
  else
    {
    // All Cold To Max Temp .. No Hot Change
    ColdSd.m_pCd->SetTemp(Max(HotSd.Ti-m_dApproachDT, ColdSd.Ti));
    ColdSd.MaxNfDuty=ColdSd.m_pCd->totHf()-ColdSd.Hi;
    ColdSd.MaxFlDuty=ColdSd.MaxNfDuty;
    }

  m_dMaxDuty=EffUA()*fabs(ColdSd.Ti-HotSd.Ti);

  HotSd.MaxNfDuty=Min(m_dMaxDuty, HotSd.MaxNfDuty);
  ColdSd.MaxNfDuty=Min(m_dMaxDuty, ColdSd.MaxNfDuty);
  HotSd.MaxFlDuty=Min(m_dMaxDuty, HotSd.MaxFlDuty);
  ColdSd.MaxFlDuty=Min(m_dMaxDuty, ColdSd.MaxFlDuty);

  m_dMaxNfDuty=Min(ColdSd.MaxNfDuty, HotSd.MaxNfDuty);
  m_dMaxFlDuty=Min(ColdSd.MaxFlDuty, HotSd.MaxFlDuty);
  }

//===========================================================================

double CHXShlTube::CondenseAll(CHXSide& HotSd, CHXSide& ColdSd, CHXSdInfo &HotI, CHXSdInfo &ColdI, byte flags)//, flag AllowVent, flag LimitDuty)
  {
  flag fCoCurrent=false;
  double VentFlow=0.0;

  double HotSdToEst=Min(HotSd.Ti, HotSd.SatT-BPE);
  double OrigVapFrac=VLE.FlashVapFrac(*HotSd.m_pCd->Model());
  VLE.SetFlashVapFrac(*HotSd.m_pCd->Model(), HotSdToEst, HotSd.Po, 0.0, VLEF_Null);
  HotSd.To=HotSd.m_pCd->Temp();
  m_dDuty=fabs(HotSd.m_pCd->totHf()-HotSd.Hi);
  if (flags & CA_AllowVent)
    {
    if (m_dDuty>m_dMaxFlDuty) // ?? Is this correct ???
      {
      double VentFrac=(m_dDuty-m_dMaxFlDuty)/GTZ(m_dDuty);
      // Adjust for what existed
      VentFrac = Range(0.0, VentFrac*(1.0-OrigVapFrac), 1.0);
      VLE.SetFlashVapFrac(*HotSd.m_pCd->Model(), HotSdToEst, HotSd.Po, VentFrac, VLEF_Null);
      VentFlow=HotSd.m_pCd->QMass(som_Gas);
      HotSd.AddGasToVent(VentFlow, false);
      m_dDuty=m_dMaxFlDuty;
      }
    m_dDuty=Min(m_dDuty, HotSd.MaxFlDuty);
    }
  else if (flags & CA_LimitDuty)
      {
      double MaxDuty=Min(ColdSd.MaxNfDuty, HotSd.MaxFlDuty);
      if (m_dDuty>MaxDuty) // ?? Is this correct ???
        {
        double VapFrac=(m_dDuty-MaxDuty)/GTZ(m_dDuty);
        m_dDuty=Min(m_dDuty, MaxDuty);
        // Adjust for what existed
        VapFrac = Range(0.0, VapFrac*(1.0-OrigVapFrac), 1.0);
        VLE.SetFlashVapFrac(*HotSd.m_pCd->Model(), HotSdToEst, HotSd.Po, VapFrac, VLEF_Null);
        }
      }
  HotSd.iMode=QPF_Condensing;
  HotSd.Ho=HotSd.m_pCd->totHf();

  ColdSd.m_pCd->SetPress(ColdSd.Po);
  ColdSd.m_pCd->Set_totHf(ColdSd.Hi+ColdSd.Sgn*m_dDuty+ColdSd.dHEnv+HotSd.dHEnv);
  ColdSd.To=ColdSd.m_pCd->Temp();
  ColdSd.iMode=QPF_Sensible;
  ColdSd.Ho=ColdSd.m_pCd->totHf();

  CHECKTEMPS(HotSd, ColdSd, 1000);

  return VentFlow;
  }

//===========================================================================

void CHXShlTube::EvalProducts(pFlwNode pNd)
  {
  m_iActOpMode=m_iRqdOpMode;
  m_dMaxDuty=0.0;
  flag SecCondensing=0;
  flag CoCurrent=false;
  flag DutyErr=false;
  double SteamFlowInCondensate=0.0;
  m_dQmSteamCond=0.0;

  if (m_iActOpMode!=STHX_Inoperative && TstNZ(m_Pri.Ci) && TstNZ(m_Sec.Ci) && m_fOn)
    {
    if (!SolveBufferedMethod())
      {
      if (m_FTC.Active() && m_iActOpMode!=STHX_FullyCondensing)
        {
        m_iActOpMode=STHX_FullyCondensing;
        CHXBlock::SetCI(4);
        }
      else if (m_Pri.m_pCn || m_Sec.m_pCn)
        {
        CHXBlock::ClrCI(4);
        m_iActOpMode=STHX_LiquorLiquor;
        }
      else
        CHXBlock::ClrCI(4);
      }

    m_fPriIsHot = (m_Pri.Ti>m_Sec.Ti);
    m_fPriIsHot = false;// CNM Hard Wired
    m_Pri.Sgn=(m_fPriIsHot ? -1 : 1);
    m_Sec.Sgn=-m_Pri.Sgn;

    CHXSide &HotSd = (m_fPriIsHot ? m_Pri : m_Sec);
    CHXSide &ColdSd = (m_fPriIsHot ? m_Sec : m_Pri);
    CHXSdInfo &HotSdI = (m_fPriIsHot ? PriI : SecI);
    CHXSdInfo &ColdSdI = (m_fPriIsHot ? SecI : PriI);

    switch (m_iActOpMode)
      {
      case STHX_LiquorLiquor:
      case STHX_LiquorGas:
      case STHX_GasGas:
        {
        // Vents & Bypasses
        HotSd.SendGasToVent(false);

        // Do Reactions Etc
        //StkSpConduit QMemS("QMemS", pNd);
        //QMemS().QSetF(*HotSd.m_pCd, som_ALL, 1.0, HotSd.Pi);

        const double CVf=ColdSd.m_pCd->MassFrac(som_Gas);
        const double HVf=HotSd.m_pCd->MassFrac(som_Gas);

        ColdSd.EvalProducts(ColdSd.To);
        HotSd.EvalProducts(HotSd.To);

        ColdSd.dHEnv=ColdSd.m_pCd->totHf()-ColdSd.Hi;
        HotSd.dHEnv=HotSd.m_pCd->totHf()-HotSd.Hi;

        // Find Mode Of Operation
        FindLimitingDuties(m_iActOpMode, HotSd, ColdSd, HotSdI, ColdSdI);

        if (CVf>0.5 && HVf>0.5)
          m_iActOpMode=STHX_GasGas;
        else if (CVf<=0.5 && HVf<=0.5)
          m_iActOpMode=STHX_LiquorLiquor;
        else
          m_iActOpMode=STHX_LiquorGas;

        if (m_Pri.Qm>UsableMass && m_Sec.Qm>UsableMass)
          {//only do the work IF there or flows in hot AND cold sides
        CHXDutyFinder DutyFnd(this, HXF_CounterCurrent, HXM_LMTD);
        DutyFnd.SetTarget(0.0);

        double MaxHXExtent=1.0;
        switch (DutyFnd.Start(0.0, MaxHXExtent))
          {
          case RF_OK:
            if (DutyFnd.Solve_Brent()==RF_OK)
              break;
            //SetCondition(2);
            break;
          case RF_LoLimit:
            DutyFnd.Function(0.0);
            break;
          case RF_HiLimit:
            // Flash the output
            DutyFnd.Function(MaxHXExtent);
            break;
          }
          }
        else
          {
          m_Pri.Duty=m_Pri.dHEnv;
          m_Sec.Duty=m_Sec.dHEnv;
          m_dEffect = 0.0;
          }

        m_dLMTD=LMTD(false, ColdSd.Ti, ColdSd.To, HotSd.Ti, HotSd.To);

        break;
        }
      case STHX_FullyCondensing:
        {
        // Vents & Bypasses
        HotSd.KeepByPassLiquor(HotSd.Pi);
        HotSd.SendGasToVent(true);

        // Do Reactions Etc
        StkSpConduit QMemS("QMemS", chLINEID(), pNd);
        QMemS().QSetF(*HotSd.m_pCd, som_ALL, 1.0, HotSd.Pi);

        #ifndef _RELEASE
        double PVf=ColdSd.m_pCd->MassFrac(som_Gas);
        double SVf=HotSd.m_pCd->MassFrac(som_Gas);
        const double PQm=ColdSd.m_pCd->QMass();
        const double SQm=HotSd.m_pCd->QMass();
        const double QQm=QMemS().QMass();
        #endif

        VLE.SetHfInAtZero(*HotSd.m_pCd);

        ColdSd.EvalProducts(ColdSd.To);
        HotSd.EvalProducts(HotSd.To);

        ColdSd.dHEnv=ColdSd.m_pCd->totHf()-ColdSd.Hi;
        HotSd.dHEnv=HotSd.m_pCd->totHf()-HotSd.Hi;

        const double SteamBefore=HotSd.m_pCd->VMass[H2OVap()];

        // Find Mode Of Operation
        FindLimitingDuties(m_iActOpMode, HotSd, ColdSd, HotSdI, ColdSdI);

        // Estimate the required amount able to Condense
        if (m_FTC.Active())
          {
#if ForceSteam
          m_FTC.m_dQCond = HotSd.WaterQm+HotSd.SteamQm;
#else
          m_FTC.m_dQCond = HotSd.Qm;
#endif
          m_FTC.m_dQCool = ColdSd.Qm;
          VLE.SetFlashVapFrac(*HotSd.m_pCd->Model(), Max(HotSd.SatT-BPE, CoCurrent ? ColdSd.To : ColdSd.Ti), HotSd.Po, 0.0, VLEF_Null);
          double OldQRqd=m_FTC.m_dQRqd;
          double MaxFlQm=ColdSd.MaxFlDuty/GTZ(HotSd.Hi/NZ(HotSd.Qm)-HotSd.m_pCd->msHf());

          ShlTubeCondFnd CondFnd(this, HotSd, ColdSd, QMemS());
          CondFnd.SetTarget(0.0);
          // NBNB 2.0e-6 must be fixed properly
          // ... it should be 2*MinSpMass - to go into SpModel.H
          MaxFlQm=Max(4.0e-6, MaxFlQm);

          int iStrtErr=CondFnd.Start(Max(2.0e-6, 1.0e-6*MaxFlQm), MaxFlQm);
          switch (iStrtErr)
            {
            case RF_OK:
              {
              int iRet=CondFnd.Solve_Brent();
              switch (iRet)
                {
                case RF_OK:
                  m_FTC.m_dQRqd=CondFnd.Result();
                  break;
                default: LogError(FullObjTag(), 0, "Condensate Finder not converged"); break;
                }
              break;
              }
            case RF_LoLimit:
              CondFnd.Function(1.0e-6*MaxFlQm);
              m_FTC.m_dQRqd=1.0e-6*MaxFlQm;
              break;
            case RF_HiLimit:
              // Flash the output
              CondFnd.Function(MaxFlQm);
              m_FTC.m_dQRqd=MaxFlQm;
              break;
            }
//dbgpln("ShlTube: %10.6f  %s", m_FTC.dQRqd, FullObjTag());
          #if dbgF
          for (double i=0; i<20; i+=i<3.0 ? 0.02 : 0.5)
            {
            CondFnd.Function(i); // Cool & Condense
            }
          #endif

          // Prevent oscillations
//CNM ???          m_FTC.dQRqd=Max(Max(1.0e-6,0.5*OldQRqd), m_FTC.dQRqd);

          //flag CondCnvBad=!ConvergedVV(m_FTC.dQRqd, m_FTC.dQCond, EqnCB().Cfg.dEps_Abs, EqnCB().Cfg.dEps_Rel*10);
          //CHXBlock::SetCI(1, CondCnvBad);
          //relax tolerance for message display...
          double d1 = EqnCB().Cfg.dEps_Abs*5.0;
          double d2 = EqnCB().Cfg.dEps_Rel*4.0*5.0;
          const flag CondCnvBad=!ConvergedVV(m_FTC.m_dQRqd, m_FTC.m_dQCond, d1, d2);
          if (CondCnvBad)
            CHXBlock::SetCI(1, "W\tCondensate Not Converged (%7.4f kg/s)", fabs(m_FTC.m_dQRqd-m_FTC.m_dQCond));
          else
            CHXBlock::ClrCI(1);

          if (CondCnvBad)
            {
            //dbgpln("Bad      %16.8f %16.8f",m_FTC.dQRqd, m_FTC.dQCond);
            }

          if (pAttachedTo->fDoDbgBrk)
            {
            int xxx=0;
            }

          HotSd.m_pCd->QCopy(QMemS());
          HotSd.MeasureHXDataCd(NULL);

          double VentFlow=CondenseAll(HotSd, ColdSd, HotSdI, ColdSdI, CA_LimitDuty);
          // No Vent Flow
          }
        else
          {
          // Add to Vent Gas
          double VentFlow;
          if (!SolveBufferedMethod())
            {
            VentFlow=CondenseAll(HotSd, ColdSd, HotSdI, ColdSdI, (m_bExtraCond ? CA_LimitNone : CA_LimitDuty));

            //now check if the actual duty exceeds maximum allowed...
            const double LMTDSat_ = LMTDSat(ColdSd.Ti, ColdSd.To, HotSd.To);
            double MxDuty = EffUA()*LMTDSat_;
            if (m_dDuty>MxDuty)
              {//too much condensed!!! Solve to determine flash amount...
              if (!m_bExtraCond)
                {
                ShlTubeCondLimitFnd CondFnd(this, HotSd, ColdSd);
                CondFnd.SetTarget(0.0);
                int iStrtErr=CondFnd.Start(0.0, 1.0);
                double Vfrac=0.0;
                switch (iStrtErr)
                  {
                  case RF_OK:
                    {
                    int iRet=CondFnd.Solve_Brent();
                    switch (iRet)
                      {
                      case RF_OK:
                        Vfrac=CondFnd.Result();
                        break;
                      default: LogError(FullObjTag(), 0, "Condensate Finder not converged"); break;
                      }
                    break;
                    }
                  case RF_LoLimit:
                    CondFnd.Function(0.0);
                    Vfrac=0.0;
                    break;
                  case RF_HiLimit:
                    // Flash the output
                    CondFnd.Function(1.0);
                    Vfrac=1.0;
                    break;
                  }
                }
              MxDuty = EffUA()*LMTDSat(ColdSd.Ti, ColdSd.To, HotSd.To);
              }
            DutyErr = (m_dDuty-MxDuty>1.0e-6);
            }
          else
            VentFlow=CondenseAll(HotSd, ColdSd, HotSdI, ColdSdI, CA_AllowVent);
          }

        m_Pri.Duty=m_dDuty*m_Pri.Sgn + m_Pri.dHEnv;
        m_Sec.Duty=m_dDuty*m_Sec.Sgn + m_Sec.dHEnv;

        ColdSd.SatP = ColdSd.m_pCd->SaturationP(ColdSd.To);
        HotSd.SatP = HotSd.m_pCd->SaturationP(HotSd.To);

//        ColdSd.m_pCd->Model()->ClrStatesOK();
//        ColdSd.m_pCd->Model()->EquilibrateAll();
//        HotSd.m_pCd->Model()->ClrStatesOK();
//        HotSd.m_pCd->Model()->EquilibrateAll();

        m_dLMTD=LMTDSat(ColdSd.Ti, ColdSd.To, HotSd.SatT);
        // Temperature Crossover Error
        CHXBlock::SetCI(3, m_dLMTD<0.0);

        if (m_dLMTD<0.0)
          m_FTC.m_dQRqd=1.0e-20;

        m_FTC.m_dMinSatPress=HotSd.m_pCd->SaturationP(CoCurrent ? HotSd.Ti : ColdSd.Ti);

        HotSd.AddByPassLiquor();

        VLE.AddHfOutAtZero(*HotSd.m_pCd);

        SteamFlowInCondensate=HotSd.m_pCd->VMass[H2OVap()];
        m_dQmSteamCond = SteamBefore-SteamFlowInCondensate;

        break;
        }
      default:
        break;
      }
    m_dTheoryDuty=EffUA()*m_dLMTD;
    }
  else
    {
    m_iActOpMode=STHX_Inoperative;
    m_Pri.To=m_Pri.Ti;
    m_Pri.Ho=m_Pri.Hi;
    m_Sec.To=m_Sec.Ti;
    m_Sec.Ho=m_Sec.Hi;
    m_dLMTD=0.0;
    m_dDuty=0.0;
    m_dTheoryDuty=EffUA()*m_dLMTD;
    m_Pri.iMode=QPF_Sensible;
    m_Sec.iMode=QPF_Sensible;
    CHXBlock::ClrCI(3);
    //must set m_FTC properly!
    if (m_FTC.Active())
      {
      double MaxFlQm=4.0e-6;
      m_FTC.m_dQRqd=1.0e-6*MaxFlQm;
      }
    m_FTC.m_dMinSatPress=10000.0;
    if (m_pVent)
      m_pVent->QZero();
    }

  #if dbgHeatX
  if (dbgHeater())
    {
    dbgpln("HX:         Q dHp:%14.2f      Ti     To      %s", fabs(m_dDuty), FullObjTag());
    dbgpln("   %10.4f     %14.2f   %6.2f  %6.2f(%6.2f)",m_Pri.Qm, m_Pri.Hi-m_Pri.m_pCd->totHf(), K_2_C(m_Pri.Ti), K_2_C(m_Pri.To), K_2_C(m_Pri.m_pCd->Temp()));
    dbgpln("   %10.4f     %14.2f   %6.2f  %6.2f(%6.2f)",m_Sec.Qm, m_Sec.Hi-m_Sec.m_pCd->totHf(), K_2_C(m_Sec.Ti), K_2_C(m_Sec.To), K_2_C(m_Sec.m_pCd->Temp()));

    //dbgpln("HX: Qp:%10.4f Qs:%10.4f dHp:%14.2f dHs:%14.4f ADty:%14.4f PTi:%6.2 PTo:%6.2(%6.2) STi:%6.2 STo:%6.2(%6.2) %s",
    //        m_Pri.Qm, m_Sec.Qm, m_Pri.Hi-m_Pri.Ho, m_Sec.Hi-m_Sec.Ho, fabs(m_dDuty),
    //        m_Pri.Ti, m_Pri.To, m_Pri.m_pCd->Temp(), m_Sec.Ti, m_Sec.To, m_Sec.m_pCd->Temp(), FullObjTag());
    }
  #endif

  m_Pri.fConnected=0;
  m_Sec.fConnected=0;
  if (m_iActOpMode==STHX_FullyCondensing)
    m_QmVent=m_pVent ? m_pVent->VMass[H2OVap()] : 0.0;
  else
    m_QmVent=m_pVent ? m_pVent->QMass() : 0.0;
  CHXBlock::SetCI(7, !SolveBufferedMethod() && m_pVent && fabs(m_QmVent-m_QmVentRqd)>1.0e-9);
  CHXBlock::SetCI(8, !SolveBufferedMethod() && m_iActOpMode==STHX_FullyCondensing && SteamFlowInCondensate>1.0e-6);
  CHXBlock::SetCI(9, !SolveBufferedMethod() && DutyErr && !m_bExtraCond);
  }

//===========================================================================

void CHXShlTube::EvalPressureSens(pFlwNode pNd, SpContainer & Contents, int iJoin, IDMask idMask)
  {
  // Total Energy Transfer
//TODO Get Signs correct
  double Duty =-m_dDuty*ICGetTimeInc();
  // Initial Conditions
  CSysVector &ContentsMA = Contents.MArray();
  SpMArray MassStartA(ContentsMA);
  double totH0 = Contents.totHf();
//  double dtotH0=Contents.dtotEnthalpy();
  double PresStart  = Contents.Press();
  double MassStart  = Contents.Mass();
  double MassStartV = Contents.Mass(som_Gas);
  double MassStartL = Contents.Mass(som_SL);

  Contents.SetHoldVentExpand();

  // Vapour Sensitivity
  double QMassV    = 1.0;
  double DeltaFrac = Range(1.0e-6, 0.01*Max(1.0, MassStartV)/NZ(fabs(QMassV)), 1.0);
  Contents.AddM(Contents, som_Gas, DeltaFrac);
  double PSensV = (Contents.Press()-PresStart)/DeltaFrac;
  RestoreContents(Contents, MassStartA, totH0);

  // Liquid Sensitivity
  double QMassL = 1.0;
  DeltaFrac     = Range(1.0e-6, 0.01*Max(1.0, MassStartL)/NZ(fabs(QMassL)), 1.0);
  Contents.AddM(Contents, som_SL, DeltaFrac);
  double PSensL = (Contents.Press()-PresStart)/DeltaFrac;
  RestoreContents(Contents, MassStartA, totH0);
  // Set them
//TODO Replace with SetVLSensitivities(...)
  for (int i=0; i<pNd->NoFlwIOs(); i++)
    if (pNd->IOIdMask_Self(i)&idMask)
      pNd->Set_IO_PSensVL(i, PSensV, PSensL);

  // Flash to Equilibrium
  if (VLE.Enabled())
    {
    // Transfer due to movement toward equil
    double MV0,MV1,DeltaMass;
    MV0 = Contents.Mass(som_Gas);

    VLE.QVFlash(Contents, Duty, VLEF_Null);
    MV1   = Contents.Mass(som_Gas);
    m_dV2LQ = (MV0-MV1)/ICGetTimeInc();

    pNd->SetJoinV2LFlow(iJoin, m_dV2LQ);//, V2LQVFrac, V2LQLFrac);
    //dbgpln("HXSens V:%14.6g L:%14.6g V2L:%14.6g %s", PSensV, PSensL, dV2LQ, FullObjTag());

    SpMArray MassEquilA(ContentsMA);
    double totHA = Contents.totHf();
    double PresEquil  = Contents.Press();
    double MassEquil  = Contents.Mass();
    double MassEquilV = Contents.Mass(som_Gas);
    double MassEquilL = Contents.Mass(som_SL);

    CJoinRec & J=pNd->Joins[iJoin];
    for (int c=0; c<J.NConns(); c++)
      {
      int i=J.IONo(c);
      SpConduit &Cd=*pNd->IOConduit(i);

      RestoreContents(Contents, MassEquilA, totHA);
      // Transfer to retain equil due to movement of vapour
      DeltaMass = Max(1.0e-6, 0.01*MassEquilV);
      Contents.AddM(Cd, som_Gas, DeltaMass);
      MV0 = Contents.Mass(som_Gas);
      VLE.QVFlash(Contents, 0.0, VLEF_Null);
      MV1 = Contents.Mass(som_Gas);
      m_dV2LQVFrac = (MV0-MV1)/DeltaMass;
//      RestoreContents(Contents, MassEquilA, totHA);

      // Transfer to retain equil due to movement of liquor
      RestoreContents(Contents, MassEquilA, totHA);
      DeltaMass = Max(1.0e-6, 0.01*MassEquilL);
      Contents.AddM(Cd, som_SL, DeltaMass);
      MV0 = Contents.Mass(som_Gas);
      VLE.QVFlash(Contents, 0.0, VLEF_Null);
      MV1 = Contents.Mass(som_Gas);
      m_dV2LQLFrac = (MV0-MV1)/DeltaMass;
//    RestoreContents(MassEquilA, totH0);

      pNd->Set_IO_V2LQmFact(i, m_dV2LQVFrac, m_dV2LQLFrac);

      //dbgpln("                                                         "
      //       "V2LV:%14.6g V2LL:%14.6g ", dV2LQVFrac, dV2LQLFrac);
      }
    RestoreContents(Contents, MassStartA, totH0);
    }
  else
    {
    //dbgpln("HXSens V:%14.6g L:%14.6g     %14s      %14s      %14s %s", PSensV, PSensL, "", "", "", FullObjTag());
    pNd->SetJoinV2LFlow(iJoin, 0.0);// 0.0, 0.0);
    }

  Contents.ClrHoldVentExpand();
  }

//===========================================================================

void CHXShlTube::RestoreContents(SpContainer &Contents, SpMArray & M, double H0)
  {
  Contents.MArray().Set(M, som_ALL, 1.0);
  Contents.Set_totHf(H0);
  };

//===========================================================================
//===========================================================================

const byte ioidST_TubeIn    = 0;
const byte ioidST_TubeOut   = 1;
const byte ioidST_ShellIn   = 2;
const byte ioidST_ShellOut  = 3;
const byte ioidST_ShellVent = 4;

const IDMask ShellIOMask = Id_2_Mask(ioidST_ShellIn)|
                           Id_2_Mask(ioidST_ShellOut)|
                           Id_2_Mask(ioidST_ShellVent);
const IDMask TubeIOMask  = Id_2_Mask(ioidST_TubeIn)|
                           Id_2_Mask(ioidST_TubeOut);


static IOAreaRec ShellTubeIOAreaList[] =
  {{"",              "Liquor_In",  ioidST_TubeIn,    LIO_In0 ,  nc_MLnk, 1, 20, IOGRP(1)|IOPipeEntry|IOApertureHoriz, 1.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iP].I"},
   {"",              "Liquor_Out", ioidST_TubeOut,   LIO_Out0,  nc_MLnk, 1,  1, IOGRP(1)|IOPipeEntry|IOApertureHoriz, 0.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iP].O"},
   {"",              "Steam",      ioidST_ShellIn,   LIO_In1 ,  nc_MLnk, 1, 20, IOGRP(2)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 1.0f, {0,0,0,0}, NULL, &PipeEntryGroup},//, "Side[iS].I"},
   {"",              "Condensate", ioidST_ShellOut,  LIO_Out1,  nc_MLnk, 1,  1, IOGRP(2)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 0.0f, {0,0,0,0}, NULL, &PipeEntryGroup},//, "Side[iS].O"},
   {"",              "Vent",       ioidST_ShellVent, LIO_Out ,  nc_MLnk, 0,  1, IOGRP(2)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 1.0f, {0,0,0,0}, NULL, &PipeEntryGroup},//, "Side[iS].O"},
   {NULL}};


static double Drw_ShellTube[] = { DD_Poly, -6,7, -4,7, -4,-7, -6,-7, -6,7,
                                DD_Poly,  6,7,  4,7,  4,-7,  6,-7,  6,7,
                                DD_Poly, -4,6, 4,6,
                                DD_Poly, -4,-6, 4,-6,
                                DD_Arc3, -6,6, -10,0, -6,-6,
                                DD_Arc3,  6,6,  10,0,  6,-6,
                                DD_End };

IMPLEMENT_MODELUNIT(ShellTube, "ShllTube", "1", Drw_ShellTube, "HeatExchange", "ST", TOC_ALL|TOC_GRP_ENERGY|TOC_HEATBAL,
//                    "HX - Shell & Tube Heat Exchanger(1)",
                    "Heat Transfer:Shell & Tube Heat Exchanger(1)",
                    "With all streams connected,  calculate the outlet temperature and duty. "
                    "With liquor streams connected, specify the outlet temperature and steam "
                    "pressure, assume all steam condensate, and calculate steam usage.")

ShellTube::ShellTube(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  LastVent("LastVent", this, TOA_Embedded),
  HX(this, &CHXShlTubeClass, true),
  m_HXJ(this, ioidST_TubeIn, ioidST_TubeOut, ioidST_ShellIn, ioidST_ShellOut),
  Shell("Shell", this, TOA_Embedded),
  //Preset(&((CHXShlTube*)HX())->VLE),
  FBP(NULL, 0, 1.0),
  FBS(NULL, 0, 1.0)
  {
  AttachClassInfo(nc_Process, ShellTubeIOAreaList, &HeatXDPGroup);
  Shell.SetClosed(true);
  Shell.SetPreset(&m_Preset,NULL);
  Shell.SetVLE(&((CHXShlTube*)HX())->VLE);

//  FBP.AssignFlwEqnGroup(HeatXDPGroup, HeatXDPGroup.Default(), this);
//  FBS.AssignFlwEqnGroup(PipeEntryGroup, PipeEntryGroup.Default(), this);

  pHXST=(CHXShlTube*)HX();

  //EHX.Open(&CEHX_LossPerQmClass);

  RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(ShellTube), ioidST_ShellIn, mmio_MODEL, &typeid(CFT_Condenser));
  }

//--------------------------------------------------------------------------

void ShellTube::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, NULL, NULL, DDB_NoPage);
  DDB.Text("");

  EHX.Add_OnOff(DDB);

  DDB.Text("");
  EHX.BuildDataDefn(DDB);

  DDB.Text("");
  BuildDataDefnShowIOs(DDB);
  if (SolveBufferedMethod())
    BuildDataDefnIOOpts(DDB);

  HX.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);

  if (SolveBufferedMethod())
    DDB.Object(&Shell, this, NULL, NULL, DDB_RqdPage);

  DDB.Object(&LastVent, this, NULL, NULL, DDB_RqdPage);

  DDB.EndStruct();
  }

//---------------------------------------------------------------------------

flag ShellTube::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;
  if (HX.DataXchg(DCB))
    return 1;
//  switch (DCB.dwUserInfo)
//    {
//    case 1 : FBP.DataXchg(DCB); return 1;
//    case 2 : FBS.DataXchg(DCB); return 1;
//    }

  return 0;
  }

//---------------------------------------------------------------------------

flag ShellTube::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);
  return OK;
  }

//--------------------------------------------------------------------------

flag ShellTube::InitialiseSolution()
  {
  flag OK=MdlNode::InitialiseSolution();
  switch (SolveMethod())
    {
    case SM_Direct:
      Shell.SetStateAction(IE_Disabled);
      break;
    case SM_Inline:
      Shell.SetStateAction(IE_SaveState);
      break;
    case SM_Buffered:
      Shell.SetStateAction(IE_Integrate);
      break;
    }
  return OK;
  }

//---------------------------------------------------------------------------

void ShellTube::StartSolution()
  {
  LastVent.QSetTraceMass();
  }

//--------------------------------------------------------------------------

void ShellTube::SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn)
  {
  CSetDatumsData SDD[]=
    {
      {First64IOIds, &Shell},
      {0}
    };
  SetDatums_Node(Pass, List, IOIn, SDD);
  };

//--------------------------------------------------------------------------

void ShellTube::SetDatumsDone()
  {
  MdlNode::SetDatumsDone();
 
  SortSurgeIOData SDD[]=
    {
      { First64IOIds,   &Shell, &ShellHgtOrd},
      { 0},
    };
  SortSurgeIO(SDD);
  };

//--------------------------------------------------------------------------

flag ShellTube::Set_Sizes()
  {
  return true;
  };

//--------------------------------------------------------------------------

void   ShellTube::SetState(eScdMdlStateActs RqdState)
  {
  MdlNode::SetState(RqdState);
  Shell.SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
      pHXST->m_FTC.SetState(RqdState);
      break;
    case MSA_Empty:
      break;
    case MSA_PreSet:
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  };

//--------------------------------------------------------------------------

bool ShellTube::PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo)
  {
  if (!MdlNode::DoPropagateNetInfo(Ctrl, IONo, false))
    return false;

  IDMask IOInMsk=IONo>=0 ? Id_2_Mask(IOId_Self(IONo)) : First64IOIds;
  for (int i=0; i<NoFlwIOs(); i++)
    {
    if ((ShellIOMask & IOInMsk & Id_2_Mask(IOId_Self(i)))!=0 ||
        (TubeIOMask  & IOInMsk & Id_2_Mask(IOId_Self(i))))
      Nd_Rmt(i)->PropagateNetInfo(Ctrl, IOIONo_Rmt(i));
    }

  return true;
  };

//--------------------------------------------------------------------------

void ShellTube::PostConnect(int IONo)
  {
  MdlNode::PostConnect(IONo);

  if (IOId_Self(IONo)>=ioidST_TubeIn && IOId_Self(IONo)<=ioidST_TubeOut)
    IOFB(IONo,0)->SetParentFlwEqn(&FBP);
  else if (IOId_Self(IONo)>=ioidST_ShellIn && IOId_Self(IONo)<=ioidST_ShellOut)
    IOFB(IONo,0)->AssignFlwEqnGroup(PipeEntryGroup, PipeEntryGroup.Default(), this);
    //IOFB(IONo,0)->AttachFlwEqn(&FBS);
  };

//---------------------------------------------------------------------------

void ShellTube::ConfigureJoins()
  {
  if ((NetMethod()==NM_Probal) || SolveMethod()!=SM_Buffered)
    {
    m_HXJ.SetJoinIds(0, true, true);
    flag fP=m_HXJ.PriIsJoined();
    for (int i=0; i<NoProcessIOs(); i++)
      {
      if (IOId_Self(i)>=ioidST_TubeIn && IOId_Self(i)<=ioidST_TubeOut)
        {
        //IOFlange(i)->SetTearPriority(TP_Normal);
        IOFlange(i)->SetTearPriority(TP_First);
        if (fP)
          SetIO_Join(i, m_HXJ.P.JoinId());
        else
          SetIO_Closed(i, m_HXJ.P.JoinId());
        }
      }

    flag fS=m_HXJ.SecIsJoined();
    for (i=0; i<NoProcessIOs(); i++)
      {
      if (IOId_Self(i)>=ioidST_ShellIn && IOId_Self(i)<=ioidST_ShellOut)
        {
        // if Fully Condensing then try to prevent Tears
        //IOFlange(i)->SetTearPriority((1 && (IOId_Self(i)==ioidST_ShellIn)) ? TP_First: TP_Normal);
        //IOFlange(i)->SetTearPriority(TP_First);
        IOFlange(i)->SetTearPriority(TP_Normal);
        //IOFlange(i)->SetTearPriority((IOId_Self(i)==ioidST_ShellOut) ? TP_First: TP_Normal);

        if (fS)
          SetIO_Join(i, m_HXJ.S.JoinId());
        else
          SetIO_Closed(i, m_HXJ.S.JoinId());
        }
      }
    }
  else
    {
    m_HXJ.SetJoinIds(0, true, false);
    flag fP=m_HXJ.PriIsJoined();
    for (int i=0; i<NoProcessIOs(); i++)
      if (IOId_Self(i)>=ioidST_TubeIn && IOId_Self(i)<=ioidST_TubeOut)
        if (fP)
          SetIO_Join(i, m_HXJ.P.JoinId());
        else
          SetIO_Closed(i, m_HXJ.P.JoinId());
    for (i=0; i<NoProcessIOs(); i++)
      if (IOId_Self(i)>=ioidST_ShellIn && IOId_Self(i)<=ioidST_ShellVent)
        SetIO_Open(i, m_HXJ.SecJoinId(), true, SolveInlineMethod() ? ESS_Denied : ESS_Allowed);
    }
  }

//--------------------------------------------------------------------------

void ShellTube::StartStep()
  {
  m_ShellStart.AtStart(Shell);
  }

//--------------------------------------------------------------------------

void ShellTube::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NoProcessJoins(); j++)
        {
        double Pj=GetPBInputPressure(j);

        if (j==1 && pHXST->m_FTC.Active())
          pHXST->m_FTC.SetSuctionP(Pj);

        SetPBJoinPressure(j, Pj, true, true);
        #if dbgFlwNode
        if (dbgEvalJoinPress(sTag()))
          dbgpln("EJP: %12.2f  %s[%i]", Pm, sTag(), j );
        #endif
        }
      }
      break;
    default:;
      {
      {
      #if dbgEvalPres
      dbgpln("EvalJoinPressures:%s",FullObjTag());
      #endif

      IOP_RhoH_Info RhoHInfo(Shell);
      double Pm=ContainerMeanPress(RhoHInfo, 0.0);
      JoinPBlk(m_HXJ.SecJoinId()).P=Pm;
      for (int i=0; i<NoFlwIOs(); i++)
        if (IOId_Self(i)>=ioidST_ShellIn && IOId_Self(i)<=ioidST_ShellVent)
          Set_IOP_RhoH_Self(i,Pm,RhoHInfo);
      }
      break;
      }
    }
  };

//--------------------------------------------------------------------------

/*flag ShellTube::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo)
  {
  if (IONo==m_HXJ.P.IOFlwOut())
    {
    IOFB(IONo)->EvaluateFlwEqn(1.0);
    return true;
    }
  if (IONo==m_HXJ.S.IOFlwOut())
    {
    IOFB(IONo)->EvaluateFlwEqn(1.0);
    return true;
    }
  return false;
  }; */

//--------------------------------------------------------------------------

void ShellTube::EvalProducts(CNodeEvalIndex & NEI)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
    case SM_Inline:
      {
      double ShellPIn=0.0;
      if (m_HXJ.PriIsJoined() && m_HXJ.PriConnected())
        {
        const int idPri = m_HXJ.P.IOFlwOut();
        SpConduit & CdOut=*IOConduit(idPri);
        double TubePIn=SigmaQInPMin(CdOut, som_ALL, ioidST_TubeIn, ioidST_TubeIn);//Out);
        HX.SetPrimary(NULL, CdOut, TubePIn);
        }
      else
        HX.ClrPrimary();
      if (m_HXJ.SecIsJoined() && m_HXJ.SecConnected())
        {
        const int idSec = m_HXJ.S.IOFlwOut();
        SpConduit & CdOut=*IOConduit(idSec);
        ShellPIn=SigmaQInPMin(CdOut, som_ALL, ioidST_ShellIn, ioidST_ShellIn);//som_Gas);
        if (SolveInlineMethod() && pHXST->m_iRqdOpMode==STHX_FullyCondensing)
          ShellPIn = CdOut.SaturationP(CdOut.Temp()); //temp kludge
        HX.SetSecondary(NULL, CdOut, ShellPIn, EHX());
        }
      else
        HX.ClrSecondary();
      HX.SetVent(LastVent, ShellPIn);

      if (m_HXJ.SecIsJoined() && m_HXJ.SecConnected())
        {
#if ForceSteam
        pHXST->m_FTC.m_dQCond = pHXST->m_Sec.WaterQm+pHXST->m_Sec.SteamQm;
#else
        pHXST->m_FTC.m_dQCond = pHXST->m_Sec.Qm;
#endif

        if (pHXST->m_FTC.Active())
          {
          double R=pHXST->m_FTC.m_dQRqd+LastVent.QMass();
          double A=0;
          for (int i=0; i<NoFlwIOs(); i++)
            if (IOId_Self(i)==ioidST_ShellIn)
              if (IO_In(i) && IOFlange(i)->IsMakeUpAvail())
                A+=IOFlange(i)->MakeUpAvail();
          A=GTZ(A);
          for (i=0; i<NoFlwIOs(); i++)
            if (IOId_Self(i)==ioidST_ShellIn)
              if (IO_In(i) && IOFlange(i)->IsMakeUpAvail())
                // Distribute according to MakeUpAvail
                IOFlange(i)->SetMakeUpReqd(R*Max(1.0e-9, IOFlange(i)->MakeUpAvail()/A));
          }
        }

      HX.EvalProducts();

      if (m_HXJ.SecIsJoined())
        {
        // Copy VentConduit to Output if present
        const int idVent=IOWithId_Self(ioidST_ShellVent);
        if (idVent>=0)
          IOConduit(idVent)->QSetF(LastVent, som_ALL, 1.0, ShellPIn);
        }
      }
    break;
    case SM_Buffered:
      {
      //      SigmaQInPMin(QInEst, som_ALL, ShellIOMask);
      //      EvalProducts_SurgeLevel(Shell, QInEst, ShellIOMask);
      //
      //      if (m_HXJ.PriIsJoined())
      //        Xfer_EvalProducts(m_HXJ.P.JoinId(), Joins[m_HXJ.P.JoinId()].Pressure(), NULL, &QSigmaOut, NULL, NULL, NULL);
      ////      if (NoProcessJoins()>=1)
      ////        Xfer_EvalProducts(0, Joins[0].Pressure(), &QSigmaOut, NULL, NULL, NULL);//RB());
      //
      //      if (m_HXJ.PriIsJoined() && m_HXJ.PriConnected())
      //        {
      //        rSpConduit CdOut=*IOConduit(m_HXJ.P.IOFlwOut());
      //        double TubePIn=SigmaQInPMin(CdOut, som_ALL, ioidST_TubeIn, ioidST_TubeIn);//Out);
      //        HX.SetPrimary(CdOut, TubePIn);
      //        }
      //      else
      //        HX.ClrPrimary();
      //      HX.SetSecondary(Shell, Shell.Press());
      //      HX.EvalProducts(JoinMask);
      double PressInP=Std_P;
      double PressInS=Std_P;
      //if (iPriSgMd==HXSM_Shell)
      //  {
      //  SigmaQInPMin(m_HXJ.P.Qi(), som_ALL, PriIOMask);
      //  EvalProducts_SurgeLevel(*pPriSg, m_HXJ.P.Qi(), PriIOMask);
      //  }
      //else if (m_HXJ.PriIsJoined())
      //#if NewXferEval
      PressInP=Joins[m_HXJ.P.JoinId()].Pressure();
      Xfer_EvalProductsIn_(m_HXJ.P.JoinId(), PressInP, &m_HXJ.P.Qi(), &m_HXJ.P.Qo(), NULL);
      Xfer_EvalProductsWork_(&m_HXJ.P.Qi(), &m_HXJ.P.Qo(), NULL, NULL);
      //#else
      //      PressInP=Xfer_EvalProductsIn(m_HXJ.P.JoinId(), Joins[m_HXJ.P.JoinId()].Pressure(), &m_HXJ.P.Qi(), &m_HXJ.P.Qo(), NULL, NULL, NULL);
      //#endif

      const IDMask SecIOMask=Id_2_Mask(ioidST_ShellIn)|
        Id_2_Mask(ioidST_ShellOut)|
        Id_2_Mask(ioidST_ShellVent);

      //const byte ioidST_TubeIn    = 0;
      //const byte ioidST_TubeOut   = 1;
      //const byte ioidST_ShellIn   = 2;
      //const byte ioidST_ShellOut  = 3;
      //const byte ioidST_ShellVent = 4;


      //if (iSecSgMd==HXSM_Shell)
      //  {
      SigmaQInPMin(m_HXJ.S.Qi(), som_ALL, SecIOMask);
      //HX.SetL
      EvalProducts_SurgeLevel(ShellHgtOrd, &m_HXJ.S.Qi());
      //  }
      //else if (m_HXJ.SecIsJoined())
      //  PressInS=Xfer_EvalProductsIn(m_HXJ.S.JoinId(), Joins[m_HXJ.S.JoinId()].Pressure(), &m_HXJ.S.Qi(), &m_HXJ.S.Qo(), NULL, NULL, NULL);

      //if (pPriSg)
      //  HX.SetPrimary(*pPriSg, pPriSg->Press());
      //else if (m_HXJ.PriIsJoined() && m_HXJ.PriConnected())
      //  {
      //  //rSpConduit CdOut=*IOConduit(m_HXJ.P.IOFlwOut());
      //  //double TubePIn=SigmaQInPMin(CdOut, som_ALL, ioidPIn, ioidPIn);//Out);
      HX.SetPrimary(NULL, m_HXJ.P.Qo(), PressInP);
      //  }
      //else
      //  HX.ClrPrimary();

      //if (pSecSg)
      HX.SetSecondary(NULL, Shell, som_Gas, 0.5, Shell.Press());
      //else if (m_HXJ.SecIsJoined() && m_HXJ.SecConnected())
      //  {
      //  //rSpConduit CdOut=*IOConduit(m_HXJ.S.IOFlwOut());
      //  //double TubePIn=SigmaQInPMin(CdOut, som_ALL, ioidSIn, ioidSIn);//Out);
      //  HX.SetSecondary(m_HXJ.S.Qo(), PressInS);
      //  }
      //else
      //  HX.ClrSecondary();
      //    //  HX.SetSecondary(Shell, Shell.Press());
      //
      HX.EvalProducts();

      //if (iPriSgMd==HXSM_Shell)
      //  {
      //  }
      //else if (m_HXJ.PriIsJoined())
      //#if NewXferEval
      Xfer_EvalProductsOut_(m_HXJ.P.JoinId(), &m_HXJ.P.Qi(), &m_HXJ.P.Qo(), NULL, NULL);
      //#else
      //      Xfer_EvalProductsOut(m_HXJ.P.JoinId(), Joins[m_HXJ.P.JoinId()].Pressure(), &m_HXJ.P.Qi(), &m_HXJ.P.Qo(), NULL, NULL, NULL);
      //#endif

      //if (iSecSgMd==HXSM_Shell)
      //  {
      //  }
      //else if (m_HXJ.SecIsJoined())
      //  Xfer_EvalProductsOut(m_HXJ.S.JoinId(), Joins[m_HXJ.S.JoinId()].Pressure(), &m_HXJ.S.Qi(), &m_HXJ.S.Qo(), NULL, NULL, NULL);

      }
    break;
    }

  if (0)
    {
    dbgpln("ShellTube Out                                    %s", FullObjTag());
    if (IOWithId_Self(ioidST_TubeIn)>=0)
      {
      SpConduit &Cd=*IOConduit(IOWithId_Self(ioidST_TubeIn));
      dbgpln("%-10s %10.2fkg/s %10.2fC", "TubeIn", Cd.QMass(), K2C(Cd.Temp()));
      }
    if (IOWithId_Self(ioidST_TubeOut)>=0)
      {
      SpConduit &Cd=*IOConduit(IOWithId_Self(ioidST_TubeOut));
      dbgpln("%-10s %10.2fkg/s %10.2fC", "TubeOut", Cd.QMass(), K2C(Cd.Temp()));
      }
    if (IOWithId_Self(ioidST_ShellIn)>=0)
      {
      SpConduit &Cd=*IOConduit(IOWithId_Self(ioidST_ShellIn));
      dbgpln("%-10s %10.2fkg/s %10.2fC", "ShellIn", Cd.QMass(), K2C(Cd.Temp()));
      }
    if (IOWithId_Self(ioidST_ShellOut)>=0)
      {
      SpConduit &Cd=*IOConduit(IOWithId_Self(ioidST_ShellOut));
      dbgpln("%-10s %10.2fkg/s %10.2fC", "ShellOut", Cd.QMass(), K2C(Cd.Temp()));
      }
    if (IOWithId_Self(ioidST_ShellVent)>=0)
      {
      SpConduit &Cd=*IOConduit(IOWithId_Self(ioidST_ShellVent));
      dbgpln("%-10s %10.2fkg/s %10.2fC", "ShellVent", Cd.QMass(), K2C(Cd.Temp()));
      }
    }  
  }

//--------------------------------------------------------------------------

void ShellTube::EvalDerivs(CNodeEvalIndex & NEI)
  {
  Shell.ZeroDeriv();
  for (int i = 0; i < NoFlwIOs(); i++)
    if (IOIdMask_Self(i) & ShellIOMask)
      Shell.AddDeriv(*IOConduit(i), IOSign(i));
  //Shell.AddInFlowDeriv();

  if (Shell.SpillExists())
    Shell.AddDeriv(Shell.Spill.Cd, -1.0);
  if (Shell.VentExists())
    Shell.AddDeriv(Shell.Vent.Cd, -1.0);

  if (HX())
    HX.EvalDerivs();
  };

//--------------------------------------------------------------------------

void ShellTube::ConvergeStates(CConvergeStateBlk &CSB)
  {
  MdlNode::ConvergeStates(CSB);
  ASSERT(NetDynamicMethod());

  pHXST->VLE.QVFlash(Shell, 0.0, VLEF_Null);//, NULL, NULL, NULL);

  }

//--------------------------------------------------------------------------

/*#F:This is called once, after the iteration process and allows the discreet
manipulation of the contents contained by the surge unit.*/

void ShellTube::EvalDiscrete()
  {
  //if (Shell.SpillExists())
  //  {
  //  SpDirectCd & O=Shell.OverFlow();
  //  O.SetTotal(O.Total()+O.QMass()*ICGetTimeInc());
  //  }
  //if (Shell.VentExists())
  //  {
  //  SpDirectCd & O=Shell.GasVent();
  //  O.SetTotal(O.Total()+O.QMass()*ICGetTimeInc());
  //  }

  // Qualities
  for (int i = 0; i < NoFlwIOs(); i++)
    if (IOIdMask_Self(i) & ShellIOMask)
      Shell.AddDiscrete(*IOConduit(i), IOSign(i));
  }

//---------------------------------------------------------------------------

flag ShellTube::EvalPressureSens()
  {
  double BigDiff = 0.0;
  pHXST->m_dDutySens = pHXST->m_dDuty;
  // Must be closed and have contents
  if (Shell.Closed() && Shell.Mass()> 1.0e-6)
    pHXST->EvalPressureSens(this, Shell, m_HXJ.SecJoinId(), m_HXJ.S.IdMask());
  else
    ZeroVLSensitivities(m_HXJ.SecJoinId());

  return true;
  };


//===========================================================================
#if DOPBSMOOTHING
void ShellTube::EvalMsAGDatas()
  {
  MsJoins.SetSize(Joins.GetSize());
  //MsJoins[1].MOutC=LastVent.MArray();
  MsJoins[1].m_ConstBG.Copy(LastVent.MArray(), -1);

//  SpMArray M(LastVent.MArray());
//  //M*=-1.0;
//  SpMArray * AM[2]={NULL, &M};
  MdlNode::EvalMsAGDatas();
  };
#endif
//===========================================================================

void ShellTube::ClosureInfo()
  {
  if (NoFlwIOs()>0)
    {
    MdlNode::ClosureInfo();
    if (m_Closure.DoFlows())
      {
      CClosureInfo &CI0=m_Closure[0];
      CClosureInfo &CI1=m_Closure[1];
      CI0.m_PowerIn+=HX()->m_dDuty;
      CI1.m_PowerIn+=-HX()->m_dDuty;
      CI1.m_HfGainAtZero+=pHXST->VLE.HfGainAtZero();

      const int id=IOWithId_Self(ioidST_ShellVent);
      if (id<0)
        {
        CI0.m_HfLoss    += LastVent.totHf();
        CI0.m_HsLoss    += LastVent.totHs();
        CI0.m_MassLoss  += LastVent.QMass();
        }
      }
    }
  };

// --------------------------------------------------------------------------

void ShellTube::MacroMdlEvaluate(eScdMacroMdlEvals Eval)
  {
  if (pHXST->m_FTC.Active())
    {
    switch (Eval)
      {
      case 0:
        if (pHXST->m_FTC.Active())
          {
#if DOPBSMOOTHING
          int n=0;
          for (int i=0; i<NoFlwIOs(); i++)
            if (IOId_Self(i)==ioidST_TubeIn)
              {
              CMSIO C;
              C.SetSrc(this, i);
              IOMSIndex(i)->SetMustExist(true);
              pHXST->m_FTC.CoolLnks.SetAtGrow(n++, C);
              pHXST->m_FTC.dVapLoss=LastVent.QMass();
              }
          pHXST->m_FTC.CoolLnks.SetSize(n);
#endif
          }
        break;
      }
    }
  };

//--------------------------------------------------------------------------

dword ShellTube::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();

  if (NoFlwIOs())
    {
    const int ioSH=IOWithId_Self(ioidST_ShellIn);
    const int ioTB=IOWithId_Self(ioidST_TubeIn);

    double FlwSh=ioSH>=0 ? IOQm_In(ioSH) : 0.0;
    double FlwTb=ioTB>=0 ? IOQm_In(ioTB) : 0.0;
    //int HasFlw=(fabs(FlwSh)>UsableMass && fabs(FlwTb)>UsableMass);
    int HasFlw=(fabs(FlwSh)>UsableMass || fabs(FlwTb)>UsableMass);
    Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
    }
  return Status;
  };

//===========================================================================
//
//
//
//===========================================================================
