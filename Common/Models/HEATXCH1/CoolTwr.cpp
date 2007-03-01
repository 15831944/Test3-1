//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 
#include "stdafx.h"
#include "sc_defs.h"
#include "flwnode.h"
#define  __COOLTWR_CPP
#include "cooltwr.h"
//#include "optoff.h"

//==========================================================================

#define dbgModels         WITHDEBUG
#if dbgModels   
#include "dbgmngr.h"
static CDbgMngr dbgFlashTrain ("HeatXch1", "FlashTrain");
#endif

//==========================================================================

enum CTM_Methods { CTM_Simple, CTM_Merkel };
enum MCT_Methods { MCT_KaVL, MCT_TOut };
enum WLM_Methods { WLM_None, WLM_Frac, WLM_Qm, WLM_DriftBlowdown };

const byte ioid_Feed         = 0;
const byte ioid_Liq          = 1;
const byte ioid_Loss         = 2;
const byte ioid_Vap          = 3;

static IOAreaRec CoolingTowerIOAreaList[] =
  {{"Feeds",    "Feed"    , ioid_Feed , LIO_In0 ,    nc_MLnk, 1, 20, IOGRP(1)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 0.0f},
   {"Liquor",   "Liquor"  , ioid_Liq  , LIO_Out0,    nc_MLnk, 1,  1, IOGRP(1)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 0.0f},
   {"LiqLoss",  "LiqLoss" , ioid_Loss , LIO_Out ,    nc_MLnk, 0,  1, IOGRP(1)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 0.0f},
   {"Vapour",   "Vapour"  , ioid_Vap  , LIO_Out ,    nc_MLnk, 1,  1, IOGRP(1)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 1.0f},
   SPILL2AREA("Spills", IOId_Spill2Area),
   VENT2AREA("Vents",   IOId_Vent2Area),
   {NULL}}; //This lists the areas of the model where links can be attached.


static double Drw_CoolingTower[] = { 
                                 DD_Poly, -14,-14, -14,-6, 14,-6, 14,-14, -14,-14,
                                 DD_Poly, 9,-7, 7,17, -1,17, -3,-7,
                                 DD_Poly, -8,-7, -8,15, -1,15,
                                 DD_End }; 

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(CoolingTower, "CoolTwr", "", Drw_CoolingTower, "HeatTransfer", "CT", TOC_PROBAL|TOC_GRP_ENERGY|TOC_HEATEXTRA,
//                    "HX - Cooling Tower",
                    "Heat Transfer:Cooling Tower",
                    "Simple cooling tower model. The unit calculates the amount "
                    "of water to be evaporated to acheive the specified requirements.")

static MInitialiseTest InitTest(&CoolingTowerClass);
SPECIEBLK_L(InitTest, H2OLiq, "H2O(l)", true);
SPECIEBLK_V(InitTest, H2OVap, "H2O(g)", true);

CoolingTower::CoolingTower(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(CoolingTowerIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(true);
  Contents.SetPreset(&m_Preset, &m_PresetImg);

  iMethod          = CTM_Merkel;
  iMerkelCalcType  = MCT_TOut;
  iLossMethod      = WLM_DriftBlowdown;
  dAirWetBulbT     = C2K(30.0);   
  dApproachT       = 5.0;     
  dKaVL            = 1.5;
  dLGRatio         = 0.98;
  dDriftLossFrac   = 0.002;
  dEvapFactor      = 0.00085;
  iCycles          = 5;
  dRqdLossFrac     = 0.02;
  dRqdLossQm       = 10.0;
  dMaxEvapFrac     = 1.0;
  dAirCp           = 0.98;
  dAirDryBulbT     = C2K(25.0);
  
  dQmIn            = 0.0;
  dAirQmIn         = 0.0;
  dTempKFeed       = Std_T;
  dFinalP          = Std_P;
  dFinalT          = dAirWetBulbT;
  dTempDrop        = 0.0;
  dHeatFlow        = 0.0;
  dDuty            = 0.0;
  dEvapFrac        = 0.0;
  dQmWaterEvap     = 0.0;
  dDriftLossQm     = 0.0;
  dBlowdownLossQm  = 0.0;
  dEvapLossQm      = 0.0;
  dLossQm          = 0.0;
  dTotalLossQm     = 0.0;
  dAirEnthOut      = 0.0;
  dAirTOut         = dAirWetBulbT;
  dAirTRise        = 0.0;
  dAirMixQm        = 0.0;
  dAirMixCp        = 0.0;
  dAirMixT         = dAirWetBulbT;

  //EHX.Open(&CEHX_LossPerQmClass);
  m_VLE.Open(NULL, true);
  }

//--------------------------------------------------------------------------

CoolingTower::~CoolingTower()
  {
  }

//--------------------------------------------------------------------------

void CoolingTower::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, isResult|0);
  DDB.Double  ("Temperature",  "T",     DC_T,    "C",      xidTemp,        this, isResult|0);
  DDB.Double  ("Density",      "Rho",   DC_Rho,  "kg/m^3", xidRho,         this, isResult|0);
  DDB.Double  ("Level",        "Lvl",   DC_Frac, "%",      xidLevel,       this, isResult|0);
  DDB.Double  ("Mass_Accum",   "QmAcc", DC_Qm,  "kg/s",    &m_QmAcc,       this, isResult|0);
  DDB.Double  ("Vol_Accum",    "QvAcc", DC_Qv,  "L/s",     &m_QvAcc,       this, isResult|0);
  AddMdlNetworked(DDB);

  DDB.Visibility(NM_Probal|SM_All|HM_All);
  static DDBValueLst DDB0[]={
    {CTM_Simple, "Simple"},
    {CTM_Merkel, "Merkel"},
    {0}};
  static DDBValueLst DDB1[]={
    {MCT_KaVL, "KaV/L"},
    {MCT_TOut, "OutletT"},
    {0}};
  static DDBValueLst DDB2[]={
    {WLM_None, "None"},
    {WLM_Frac, "MassFrac"},
    {WLM_Qm, "MassFlow"},
    {WLM_DriftBlowdown, "Drift&Blowdown"},
    {0}};

  //TODO: many of these variables should be calculated using xid_xxx rather than storing everything!
  DDB.Text    ("");
  DDB.Byte    ("Method",               "", DC_,     "",      &iMethod,        this, isParm|SetOnChange, DDB0);
  DDB.Text    ("");
  DDB.Text    ("Characteristics");
  DDB.Visibility(NM_Probal|SM_All|HM_All, iMethod==CTM_Merkel);
  DDB.Byte    ("CalcType",             "", DC_,     "",      &iMerkelCalcType,this, isParm|SetOnChange, DDB1);
  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.Double  ("AirWetBulbT",          "", DC_T,    "C",     &dAirWetBulbT,   this, isParm);
  DDB.Double  ("ApproachTemp","ApproachT", DC_dT,   "C",     &dApproachT,     this, (iMethod==CTM_Merkel && iMerkelCalcType==MCT_TOut) ? isResult : isParm);
  DDB.Visibility(NM_Probal|SM_All|HM_All, iMethod==CTM_Merkel);
  DDB.Double  ("LG_Ratio",             "", DC_,     "",      &dLGRatio,       this, isParm);
  DDB.Double  ("KaVL",                 "", DC_,     "",      &dKaVL,          this, (iMethod==CTM_Merkel && iMerkelCalcType==MCT_TOut) ? isParm : isResult);
  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.Double  ("FeedQm",               "", DC_Qm,   "kg/s",  &dQmIn,          this, isResult);
  DDB.Double  ("TempFeed",       "Feed.T", DC_T,    "C",     &dTempKFeed,     this, isResult);
  DDB.Double  ("TempDrop",        "TDrop", DC_dT,   "C",     &dTempDrop,      this, isResult);
  DDB.Double  ("FinalT",               "", DC_T,    "C",     &dFinalT,        this, isResult);
  //DDB.Double  ("HeatFlow",             "", DC_Pwr,  "kW",    &dHeatFlow,      this, isResult); perhaps remove???
  DDB.Double  ("HeatTransfer",         "", DC_Pwr,  "kW",    &dDuty,          this, isResult);
  DDB.Double  ("FinalP",               "", DC_P,    "kPag",  &dFinalP,        this, isResult|InitHidden);
  DDB.Text    ("");
  DDB.Text    ("Water Loss/Makeup");
  DDB.Byte    ("LossMethod",           "", DC_,     "",      &iLossMethod,    this, isParm|SetOnChange, DDB2);
  DDB.Visibility(NM_Probal|SM_All|HM_All, iLossMethod==WLM_DriftBlowdown);
  DDB.Double  ("DriftLoss",            "", DC_Frac, "%",     &dDriftLossFrac, this, isParm);
  DDB.Long    ("Cycles",               "", DC_,     "",      &iCycles,        this, isParm);
  DDB.Visibility(NM_Probal|SM_All|HM_All, iLossMethod==WLM_Frac);
  DDB.Double  ("RqdLossFrac",          "", DC_Frac, "%",     &dRqdLossFrac,   this, isParm);
  DDB.Visibility(NM_Probal|SM_All|HM_All, iLossMethod==WLM_Qm);
  DDB.Double  ("RqdLossQm",            "", DC_Qm,   "kg/s",  &dRqdLossQm,     this, isParm);
  DDB.Visibility(NM_Probal|SM_All|HM_All, iMethod==CTM_Merkel);
  DDB.Double  ("EvapFactor",           "", DC_,     "",      &dEvapFactor,    this, isParm);
  DDB.Visibility(NM_Probal|SM_All|HM_All, iMethod==CTM_Simple);
  DDB.Double  ("MaxEvapFrac",          "", DC_Frac, "%",     &dMaxEvapFrac,   this, isParm);
  DDB.Visibility(NM_Probal|SM_All|HM_All, iLossMethod==WLM_DriftBlowdown);
  DDB.Double  ("DriftLossQm",          "", DC_Qm,   "kg/s",  &dDriftLossQm,   this, isResult);
  DDB.Double  ("BlowdownLossQm",       "", DC_Qm,   "kg/s",  &dBlowdownLossQm,this, isResult);
  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.Double  ("LossQm",               "", DC_Qm,   "kg/s",  &dLossQm,        this, isResult);
  DDB.Double  ("EvapLossQm",           "", DC_Qm,   "kg/s",  &dEvapLossQm,    this, isResult);
  DDB.Double  ("TotalLossQm",          "", DC_Qm,   "kg/s",  &dTotalLossQm,   this, isResult|InitHidden);
  DDB.Double  ("WaterEvapQm",    "EvapQm", DC_Qm,   "kg/s",  &dQmWaterEvap,   this, isResult|InitHidden);
  DDB.Double  ("WaterVapFrac",         "", DC_Frac, "%",     &dEvapFrac,      this, isResult);
  if (iMethod==CTM_Merkel)
    {
    DDB.Text    ("");
    DDB.Text    ("Air-Water Mixture Estimates");
    }
  DDB.Visibility(NM_Probal|SM_All|HM_All, iMethod==CTM_Merkel);
  DDB.Double  ("AirEnthOut",           "", DC_HMs,  "kJ/kg", &dAirEnthOut,    this, isResult);
  DDB.Double  ("HeatAvailable",        "", DC_Pwr,  "kW",    &dDuty,          this, isResult);
  DDB.Double  ("AirQm",                "", DC_Qm,   "kg/s",  &dAirQmIn,       this, isResult);
  DDB.Double  ("AirCp",                "", DC_CpMs, "kJ/kg.C",&dAirCp,        this, isParm);
  DDB.Double  ("AirInDryBulbT",        "", DC_T,    "C",     &dAirDryBulbT,   this, isParm);
  DDB.Double  ("AirTRise",             "", DC_dT,   "C",     &dAirTRise,      this, isResult);
  DDB.Double  ("AirOutT",              "", DC_T,    "C",     &dAirTOut,       this, isResult);
  DDB.Double  ("AirWaterMixQm",        "", DC_Qm,   "kg/s",  &dAirMixQm,      this, isResult);
  DDB.Double  ("AirWaterMixCpEst",     "", DC_CpMs, "kJ/kg.C",&dAirMixCp,     this, isResult);
  DDB.Double  ("AirWaterMixTEst",      "", DC_T,    "C",     &dAirMixT,       this, isResult);
  
  //DDB.Visibility(NM_Probal|SM_All|HM_All);
  //RB.Add_OnOff(DDB, False);
  //EHX.Add_OnOff(DDB, False);
  
  DDB.Visibility();
  

  DDB.Text  (" ");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);
  
  //RB.BuildDataDefn(DDB);
  //EHX.BuildDataDefn(DDB);
  //VLE.BuildDataDefn(DDB);

  if (NetDynamicMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);

    //if (Contents.SpillExists())
    //  DDB.Object(&Contents.OverFlow(), this, NULL, NULL, DDB_RqdPage);
    }
  DDB.EndStruct();
 
  }

//--------------------------------------------------------------------------

flag CoolingTower::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;

  return 0;
  }

//---------------------------------------------------------------------------

flag CoolingTower::ValidateData(ValidateDataBlk & VDB)
  {
  return MN_Surge::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

flag CoolingTower::InitialiseSolution() 
  {
  MN_Surge::InitialiseSolution();
  return True;
  }

//--------------------------------------------------------------------------

void CoolingTower::EvalJoinPressures(long JoinMask)
  {
  MN_Surge::EvalJoinPressures(JoinMask);
  };
 
//--------------------------------------------------------------------------

void CoolingTower::SetState(eScdMdlStateActs RqdState)
  {
  MN_Surge::SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
      //FTC.SetState(RqdState);
      break;
    case MSA_ZeroFlows:
    case MSA_Empty:
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  }

//---------------------------------------------------------------------------

//===========================================================================

class EvapFnd : public MRootFinderBase
  {
  public:
    SpConduit &Cd;
    double RqdT, RqdP;
    static CToleranceBlock s_Tol;
    CVLEBase & m_VLE;

    EvapFnd(SpConduit &Cd_, double t, double p, CVLEBase & VLE) :
      Cd(Cd_),
      MRootFinderBase("CoolTower:EvapFnd", s_Tol),
      m_VLE(VLE)
      {
      RqdT=t;
      RqdP=p;
      };
    LPCTSTR ObjTag() { return (LPCTSTR)Cd.FullObjTag(); };
    double Function(double x);
  };

CToleranceBlock EvapFnd::s_Tol(TBF_Both, "CoolTower:EvapFnd", 0.0, 1.0e-9);

double EvapFnd::Function(double x)
  {
  m_VLE.SetFlashVapFrac(Cd, x, 0);
  //Cd.Model()->ClrStatesOK();
  Cd.SetTemp(RqdT);
  Cd.SetPress(RqdP);
  double h=Cd.totHf();
  return h;
  }

//--------------------------------------------------------------------------

const double MerkelTMn = C2K(-106.5);
const double MerkelTMx = C2K(70.0);

double AirEnth(double T)
  {
  ASSERT_RDB(K2C(T)>-106.666666666666 && K2C(T)<70.0000001, "Temperature range problem!", __FILE__, __LINE__);
  const double Tc = Range(-106.666666666667, K2C(T), 70.0);
  if (Tc<-45.5555555555557)
    return 1.0086*Tc + 18.092; //y = 1.0086x + 18.092
  if (Tc<-23.3333333333334)
    return 0.0026*Tc*Tc + 1.2353*Tc + 23.016; //y = 0.0026x2 + 1.2353x + 23.016
  if (Tc<-12.2222222222223)
    return 0.0082*Tc*Tc + 1.4875*Tc + 25.788; //y = 0.0082x2 + 1.4875x + 25.788
  if (Tc<0.0000000001)
    return 0.0199*Tc*Tc + 1.7523*Tc + 27.309; //y = 0.0199x2 + 1.7523x + 27.309
  if (Tc<12.2222222222223)
    return exp(-0.0005*Tc*Tc + 0.0599*Tc + 3.3099); //y = exp(-0.0005x2 + 0.0599x + 3.3099)
  if (Tc<30.0000000001)
    return exp(-8E-05*Tc*Tc + 0.0487*Tc + 3.3802); //y = exp(-8E-05x2 + 0.0487x + 3.3802)
  if (Tc<50.0000001)
    return exp(-1.51312249 + 0.0000769787611*(Tc+255.676741)*(Tc+255.676741)); //y = exp(-1.51312249 + 0.0000769787611*(x+255.676741)^2)
  //if (Tc<70.0000001)
    return exp(3.30966428 + 0.00023039444*(Tc+51.4242508)*(Tc+51.4242508)); //y = exp(3.30966428 + 0.00023039444*(x+51.4242508)^2)
  }

double AirEnth_BTU(double T)
  {
  ASSERT_RDB(T>=-160.0 && T<=158.0, "Temperature range problem!", __FILE__, __LINE__);
  const double Tf = Range(-160.0, T, 158.0);
  if (Tf<-50.0)
    return 0.241*Tf + 0.0699; //y = 0.241x + 0.0699
  if (Tf<-10.0)
    return 0.000273*Tf*Tf + 0.2675*Tf + 0.724; //y = 0.000273x2 + 0.2675x + 0.724
  if (Tf<10.0)
    return 0.0011*Tf*Tf + 0.2857*Tf + 0.8348; //y = 0.0011x2 + 0.2857x + 0.8348
  if (Tf<32.0)
    return 0.002638*Tf*Tf + 0.2501*Tf + 1.0456; //y = 0.002638x2 + 0.2501x + 1.0456
  if (Tf<50.0)
    return exp(-0.000188*Tf*Tf + 0.0456*Tf + 1.2004); //y = exp(-0.000188x2 + 0.0456x + 1.2004)
  if (Tf<88.0)
    return exp(-3E-05*Tf*Tf + 0.0295*Tf + 1.611); //y = exp(-3E-05x2 + 0.0295x + 1.611)
  if (Tf<126.0)
    return exp(-2.43021649 + 0.0000235036734*(Tf+434.020949)*(Tf+434.020949)); //y = exp(-2.43021649 + 0.0000235036734*(x+434.020949)^2)
  //if (Tf<158.0)
    return exp(2.64525867 + 0.0000757592508*(Tf+48.1132091)*(Tf+48.1132091)); //y = exp(2.64525867 + 0.0000757592508*(x+48.1132091)^2)
  }

//===========================================================================

class MerkelTempFnd : public MRootFinderBase
  {
  public:
    SpConduit &Cd;
    CoolingTower &M;
    double T1;
    double h2;
    static CToleranceBlock s_Tol;

    MerkelTempFnd(SpConduit &Cd_, CoolingTower &M_, double T1_) :
      Cd(Cd_),
      M(M_),
      MRootFinderBase("CoolTower:TempFnd", s_Tol)
      {
      T1 = T1_;
      };
    LPCTSTR ObjTag() { return (LPCTSTR)Cd.FullObjTag(); };
    double Function(double x);
  };

CToleranceBlock MerkelTempFnd::s_Tol(TBF_Both, "CoolTower:TempFnd", 0.0, 1.0e-6);

double MerkelTempFnd::Function(double x)
  {
  double T2 = x;
  const double LG = M.dLGRatio;
  const double TAf = K2F(M.dAirWetBulbT);
  const double T1f = K2F(T1);
  const double T2f = K2F(T2);
  const double dTf = T1f - T2f;
  const double hw_T2a = AirEnth_BTU(T2f);
  const double hw_T2b = AirEnth_BTU(T2f+0.1*dTf);
  const double hw_T2c = AirEnth_BTU(T2f+0.4*dTf);
  const double hw_T1a = AirEnth_BTU(T1f-0.4*dTf);
  const double hw_T1b = AirEnth_BTU(T1f-0.1*dTf);
  const double hw_T1c = AirEnth_BTU(T1f);
  const double ha_TA1a = AirEnth_BTU(TAf);
  const double ha_TA1b = ha_TA1a + 0.1*LG*dTf;
  const double ha_TA1c = ha_TA1a + 0.4*LG*dTf;
  const double ha_TA2c = ha_TA1a + LG*dTf;
  const double ha_TA2b = ha_TA2c - 0.1*LG*dTf;
  const double ha_TA2a = ha_TA2c - 0.4*LG*dTf;
  const double dh1 = 1.0/(hw_T2b-ha_TA1b);
  const double dh2 = 1.0/(hw_T2c-ha_TA1c);
  const double dh3 = 1.0/(hw_T1a-ha_TA2a);
  const double dh4 = 1.0/(hw_T1b-ha_TA2b);
  const double dh = dh1+dh2+dh3+dh4;
  const double KaVL_BTU = dTf/4.0*dh;
  h2 = ha_TA2c;
  return KaVL_BTU;
  }

//--------------------------------------------------------------------------

void CoolingTower::EvalProducts(CNodeEvalIndex & NEI)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      {
      const int wi = H2OLiq();
      const int si = H2OVap();
      const int ioLiq = IOWithId_Self(ioid_Liq);
      const int ioVap = IOWithId_Self(ioid_Vap);
      const int ioLoss = IOWithId_Self(ioid_Loss);
      SpConduit & Ql=*IOConduit(ioLiq);
      SpConduit & Qv=*IOConduit(ioVap);
      
      StkSpConduit QMix("Mix", chLINEID(), this);

      iCycles = Max(2L, iCycles);
      dLGRatio = Range(0.01, dLGRatio, 10.0);
      ClrCI(3);
      ClrCI(4);
      ClrCI(5);
      ClrCI(6);
      dDuty = 0.0;
      SigmaQInPMin(QMix(), som_ALL, Id_2_Mask(ioid_Feed));
      Ql.QCopy(QMix());
      Qv.QCopy(QMix());

      const double QmWaterLiqIn = QMix().VMass[wi];
      const double QmWaterVapIn = QMix().VMass[si];
      const double QmVapIn = QMix().QMass(som_Gas);
      dQmIn = QMix().QMass(som_ALL);
      const flag HasFlw = (dQmIn>UsableMass);
      dTempKFeed = QMix().Temp();
      const double TotHfAtFeedT_Before = QMix().totHf(som_ALL, dTempKFeed, QMix().Press());
      
      //RB.EvalProducts(QMix);
      //EHX.EvalProducts(QMix);

      //double POut = AtmosPress(); //force outlet to Atmos P
      double POut = Std_P; //force outlet to Std_P

      //double AvgCellAgeY=AvgCellAge/(365.25*24.0*60.0*60.0);   
      //double TimeSinceDescaleM=TimeSinceDescale/(365.25*24.0*60.0*60.0/12.0);   
      //double RqdLiqTemp = AirWetBulbT + Approach + AvgCellAgeY*1.1 + TimeSinceDescaleM/12.0*4.0;
      double RqdLiqTemp = dAirWetBulbT + dApproachT;
      double T1 = QMix().Temp();
      double T2 = RqdLiqTemp;
      dLossQm = 0.0;
      bool ValidData;
      switch (iMethod)
        {
        case CTM_Simple: ValidData = (RqdLiqTemp<T1); break;
        case CTM_Merkel: ValidData = (iMerkelCalcType==MCT_TOut ? (dAirWetBulbT<T1) : (RqdLiqTemp<T1)); break;
        }
      if (!HasFlw)
        ValidData = false;
      double RqdLiqTempUsed;
      if (ValidData)
        {
        m_VLE.SetHfInAtZero(QMix());
        if (iMethod==CTM_Simple)
          {
          //const double h1 = QMix().totHf();
          RqdLiqTempUsed = RqdLiqTemp;
          EvapFnd EF(QMix(), RqdLiqTempUsed, POut, m_VLE);//QMix().Press());
          EF.SetTarget(QMix().totHf());
          if (Valid(dEvapFrac))
            {
            EF.SetEstimate(dEvapFrac, 1.0);
            //dEvapFrac = dNAN;
            }
          flag Ok = false;
          dMaxEvapFrac = Range(0.01, dMaxEvapFrac, 1.0);
          int iRet=EF.Start(0.0, dMaxEvapFrac);
          if (iRet==RF_EstimateOK) //estimate is good, solve not required
            {
            Ok = true;
            }
          else
            {
            if (iRet==RF_BadEstimate)
              iRet = EF.Start(0.0, dMaxEvapFrac); // Restart
            if (iRet==RF_OK)
              if (EF.Solve_Brent()==RF_OK)
                Ok = true;
            }
          dEvapFrac = EF.Result(); //use result regardless
          if (!Ok)
            {
            SigmaQInPMin(QMix(), som_ALL, Id_2_Mask(ioid_Feed));
            m_VLE.SetFlashVapFrac(QMix(), dEvapFrac, 0);
            QMix().SetPress(POut);
            RqdLiqTempUsed = QMix().Temp();
            }
          //const double h2 = QMix().totHf();
          //dDuty = h1-h2;
          //todo calculate dDuty
          }
        else
          {
          dAirCp = Max(0.000001, dAirCp);
          dAirWetBulbT = Range(MerkelTMn, dAirWetBulbT, MerkelTMx-10.0);
          if (T1<MerkelTMn || T1>MerkelTMx)
            {
            SetCI(6, true);
            T1 = Range(MerkelTMn, T1, MerkelTMx);
            }

          MerkelTempFnd Fnd(QMix(), *this, T1);
          if (iMerkelCalcType==MCT_KaVL)
            {
            if (T2<MerkelTMn || T2>MerkelTMx)
              {
              SetCI(5, true);
              T2 = Range(MerkelTMn, T2, MerkelTMx);
              RqdLiqTemp = T2;
              }
            dKaVL = Fnd.Function(T2);
            }
          else
            {
            Fnd.SetTarget(dKaVL);
            //Note that for high LG_Ratio (eg>1.0) then ApproachT is higher.

            double Mn = dAirWetBulbT+(T1-dAirWetBulbT)*0.005;
            const double Mx = dAirWetBulbT+(T1-dAirWetBulbT)*0.999;//T1-0.001;
            if (Valid(RqdLiqTemp) && RqdLiqTemp>Mn && RqdLiqTemp<Mx)
              {
              Fnd.SetEstimate(RqdLiqTemp, 1.0);
              //RqdLiqTemp = dNAN;
              }
            flag Ok = false;
            int iRet=Fnd.Start(Mn, Mx);
            if (iRet==RF_EstimateOK) //estimate is good, solve not required
              {
              Ok = true;
              }
            else
              {
              double KaVL_MnTest = Fnd.Function(Mn);
              if (KaVL_MnTest<0.0)
                {
                //Crude fix to find min temp that doesn't cause KaV/L to be negative...
                double fr = 0.01;
                while (KaVL_MnTest<0.0 && fr<0.9)
                  {
                  Mn = dAirWetBulbT+(T1-dAirWetBulbT)*fr;
                  KaVL_MnTest = Fnd.Function(Mn);
                  fr += 0.01;
                  }
                iRet = Fnd.Start(Mn, Mx); // Restart
                }
              if (iRet==RF_OK)
                if (Fnd.Solve_Brent()==RF_OK)
                  Ok = true;
              }
            RqdLiqTemp = Fnd.Result(); //use result regardless
            T2 = RqdLiqTemp;
            if (!Ok)
              {
              const double KaVL_Calc = Fnd.Function(T2);
              SetCI(3, fabs(KaVL_Calc-dKaVL)>1.0e-6);
              }
            dApproachT = RqdLiqTemp - dAirWetBulbT;
            }
         
          dEvapLossQm = dQmIn * dEvapFactor * (C2F(T1)-C2F(T2));//Evaporation Loss: WLe =  Wc EvapFactor dT

          RqdLiqTempUsed = RqdLiqTemp;

          dEvapFrac = Min(dMaxEvapFrac, (dEvapLossQm+QmWaterVapIn)/GTZ(QmWaterLiqIn+QmWaterVapIn));
          const double h1 = QMix().totHf();
          m_VLE.SetFlashVapFrac(QMix(), dEvapFrac, 0);
          QMix().SetPress(POut);
          QMix().SetTemp(RqdLiqTempUsed);
          const double h2 = QMix().totHf();
          dDuty = h1-h2;

          //dAirEnthOut = AirEnth(T2);
          dAirEnthOut = Fnd.h2 / 0.430210432; //convert from Btu/lb to kJ/kg
          dAirQmIn = dQmIn/dLGRatio;
          dAirTRise = dDuty/GTZ(dAirQmIn)/dAirCp;
          dAirTOut = dAirDryBulbT + dAirTRise;
          dAirMixQm = dAirQmIn + dEvapLossQm;
          const double EvapLossCp = Qv.msCp();
          dAirMixCp = dAirQmIn/GTZ(dAirMixQm)*dAirCp + dEvapLossQm/GTZ(dAirMixQm)*EvapLossCp;
          dAirMixT = dAirQmIn/GTZ(dAirMixQm)*dAirTOut + dEvapLossQm/GTZ(dAirMixQm)*T2;
          }

        double QmWaterVapOut = QMix().VMass[si];
        dQmWaterEvap = Max(0.0, QmWaterVapOut - QmWaterVapIn);
        if (iMethod==CTM_Simple)
          dEvapLossQm=dQmWaterEvap;
        switch (iLossMethod)
          {
          case WLM_None: dLossQm = 0.0; break;
          case WLM_Frac: dLossQm = dQmIn * dRqdLossFrac; break;
          case WLM_Qm: dLossQm = dRqdLossQm; break;
          case WLM_DriftBlowdown:
            dDriftLossQm = dQmIn * dDriftLossFrac;//Drift Loss: WLd = % of water flow
            dBlowdownLossQm = dEvapLossQm/(iCycles-1);//Blowdown Loss: WLb = WLe / (cycles - 1)
            dLossQm = dDriftLossQm+dBlowdownLossQm;
            break;
          }
        if (dLossQm>dQmIn-dEvapLossQm-QmVapIn)
          {
          SetCI(4, true);
          dLossQm = dQmIn-dEvapLossQm-QmVapIn;
          }
        m_VLE.AddHfOutAtZero(QMix());
        }
      else
        {
        RqdLiqTempUsed = T1;
        dEvapFrac = 0.0;
        dLossQm = 0.0;
        dDriftLossQm = 0.0;
        dBlowdownLossQm = 0.0;
        dEvapLossQm = 0.0;
        dQmWaterEvap = 0.0;
        //if (iMethod==CTM_Merkel)
          {
          dAirEnthOut = TotHfAtFeedT_Before / 0.430210432; //convert from Btu/lb to kJ/kg
          dAirQmIn = 0.0;
          dAirTRise = 0.0;
          dAirTOut = dTempKFeed;
          dAirMixQm = 0.0;
          dAirMixCp = 0.0;
          dAirMixT = dTempKFeed;
          }
        }

      //QMix.ChangeModel(&SMSteamClass);
      const double TotHfAtFeedT_After = QMix().totHf(som_ALL, dTempKFeed, QMix().Press());
      Qv.QSetF(QMix(), som_Gas, 1.0);
      Qv.SetPress(POut);
      Qv.SetTemp(RqdLiqTempUsed);
      if (ioLoss<0)
        {
        Ql.QSetF(QMix(), som_SL, 1.0);
        Ql.SetPress(POut);
        Ql.SetTemp(RqdLiqTempUsed);
        }
      else
        {
        SpConduit & Qloss=*IOConduit(ioLoss);
        const double Qsl = QMix().QMass(som_SL);
        const double f = dLossQm/GTZ(Qsl);
        Ql.QSetF(QMix(), som_SL, 1.0-f);
        Ql.SetPress(POut);
        Ql.SetTemp(RqdLiqTempUsed);
        Qloss.QCopy(QMix());
        Qloss.QSetF(QMix(), som_SL, f);
        Qloss.SetPress(POut);
        Qloss.SetTemp(RqdLiqTempUsed);
        }

      //results...
      dTotalLossQm = dLossQm+dEvapLossQm;
      dHeatFlow = TotHfAtFeedT_After - TotHfAtFeedT_Before; //what exactly is this???
      dFinalP = Ql.Press();
      dFinalT = Ql.Temp();
      dTempDrop = T1 - dFinalT;
      SetCI(1, HasFlw && RqdLiqTemp>T1);
      SetCI(2, HasFlw && RqdLiqTempUsed>RqdLiqTemp);
      break;
      }
    default:
      MN_Surge::EvalProducts(NEI);
    }
  }
    
//--------------------------------------------------------------------------

void CoolingTower::ClosureInfo()
  {
  MN_Surge::ClosureInfo();
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    CI.m_PowerIn+=-dDuty;
    CI.m_HfGainAtZero+=m_VLE.HfGainAtZero();
    }
  }

//--------------------------------------------------------------------------

flag CoolingTower::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tFeed temperature lower than temperature requirements";  return 1;
    case 2: pS="W\tCannot achieve temperature requirements";               return 1;
    case 3: pS="W\tCannot achieve KaV/L requirements";                     return 1;
    case 4: pS="W\tCannot achieve loss requirements";                      return 1;
    case 5: pS="E\tTemperature requirements out of valid range";           return 1;
    case 6: pS="E\tFeed temperature out of valid range";                   return 1;
    default:                                               
      return MN_Surge::CIStrng(No, pS);
    }
  }


//==========================================================================
//
//
//
//==========================================================================
