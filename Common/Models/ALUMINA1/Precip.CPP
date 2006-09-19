//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include "sc_defs.h"
#define  __PRECIP_CPP
#include "alspmdlb.h"
#include "precip.h"
#include "..\SizeDst1\sqssa.h"
//#include "optoff.h"

//==========================================================================
/*#D:#T:Precipitator
The Precipitator is modelled as a continuous stirred tank reactor with gibbsite 
crystallisation precipitation.  The model uses a precipitation rate equation 
developed by S. Rosenberg and S. Healy1.  The model calculates yield, flow solids 
in tank, residence time, specific surface area of hydrate, bound soda, and the 
caustic concentration and alumina concentration in the tank.  The Precipitator 
receives up to 20 feed streams and calculates a single overflow stream.  The 
model does not take into account by-pass flow.
#n#n
#n#u<#l<#b<References>>>#n#n
1) Rosenberg, S.P., Healy, S.J. "A Thermodynamic model for Gibbsite solubility 
in Bayer liquors", Fourth International Alumina Quality workshop, June, 1996.#n
#n#n
#n#u<#l<#b<Inputs and Outputs>>>#n#w
Label    Input/Output  Minimum number  Maximum number#n
Feed         Input          1              20#n
Product      Output         1              1#n
#a
To function the Precipitator must have at least one feed and the product stream 
connected.
#n#n
#n#u<#l<#b<Model variables>>>#n#n
#i<V> : The volume of the unit#n
#i<E/R> : The Activation Energy (E) divided by the Gas Constant (R).  Published 
values are given in Table 2 in the help file.  The default is 8500.#n
#i<K> : The Growth Rate Constant.  The default value is 6*10^8.#n
#i<Temp Drop> : The user may specify a temperature drop in the unit due to heat 
losses to ambient.  The valid range is 0 to 0.99 0C.#n

#n#h<Other>#n
Default model prefix:PP#n
Short name:Precip#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

#define NewPrecipCode2 1
#define NewPrecipCode3 0

const byte ioid_Feed         = 0;
const byte ioid_Product      = 1;

static IOAreaRec PrecipIOAreaList[] =
  {{"Feed",     "Feed",      ioid_Feed    , LIO_In0 ,    nc_MLnk, 1, 20,     0, 1.0f},
   {"Product", "Product",    ioid_Product , LIO_Out0,    nc_MLnk, 1,  1,     0, (float)0.5},
   SPILL2AREA("Spills", IOId_Spill2Area),
   VENT2AREA("Vents",   IOId_Vent2Area),
   {NULL}}; //This lists the areas of the model where links can be attached.


static double Drw_Precip[] = { DD_Poly, -5,10, -5,-10, 5,-10, 5,10,
                      DD_End }; //This provides the default drawing of the Precipitator.

enum YieldMethods { YM_None, YM_Yield1, YM_Yield2, YM_Yield3 };
enum GrowthRateMethods { GRM_White, GRM_Cresswell, GRM_WhiteBateman };
enum SodaPrecipMethods { SPM_Sang, SPM_Ohkawa };
enum ThermalHeatLossMethods { THL_None, THL_TempDrop, THL_FixedHeatFlow, THL_HeatFlowFrac };
enum ReactionHeatMethods { RHM_Calc, RHM_Override1 };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Precip, "Precip", "1", Drw_Precip, "Tank", "PP", TOC_ALL|TOC_GRP_ALUMINA|TOC_ALUMINA,
                    "Process:Unit:Precipitator",   
                    "Generic Precipitator") 

static MInitialiseTest InitTest(&PrecipClass);
SPECIEBLK_L(InitTest, Water,           "H2O(l)",        false);
SPECIEBLK_L(InitTest, Alumina,         "Al2O3(l)",      false);
SPECIEBLK_L(InitTest, SodiumOxalate,   "Na2C2O4(l)",    false);
SPECIEBLK_L(InitTest, CausticSoda,     "NaOH(l)",       false);
SPECIEBLK_L(InitTest, SodiumCarbonate, "Na2CO3(l)",     false);
SPECIEBLK_S(InitTest, THA,             "Al2O3.3H2O(s)", false);
SPECIEBLK_S(InitTest, OccSoda,         "Na2O(s)",       false);
SPECIEBLK_S(InitTest, OxalateSol,      "Na2C2O4(s)",    false);

Precip::Precip(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(PrecipIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(False);

  bOnLine = 1;
  fMode   = YM_None;
  ER_1    = 7200.0;
  K_1     = 1.96e7;//1.96e10;
  Ksoda_1 = 0.00127;

  TempDropRqd = 0.5;
  ThermalLossRqd = 1500.0;
  ThermalLossFrac = 0.0002;
  //LossFac = 0.005;
  AggParm = 1.0;
  iGrowthRateMethod  = GRM_Cresswell;
  iSodaPrecipMethod  = SPM_Ohkawa;
  iThermalLossMethod = THL_TempDrop;
  iRctHeatMethod     = RHM_Calc;
  dHOR_a             = 400.0;
  dHOR_b             = 1.0;
  dHORDiff           = 0.0;

  fFLPostPrecip = 1;
  FeedShapeFact = 0.75;
  ProdShapeFact = FeedShapeFact;
  ER_White      = 7200.0;
  ER_Cresswell  = 7600.0;
  ER_WhBateman  = 8500.0;
  K_White       = 1.96e10;
  K_Cresswell   = 15.0;
  K_WhBateman   = 7.4e12;
  gF_White      = 1.0;
  gF_Cresswell  = 1.0;
  gF_WhBateman  = 1.0;
  Tref          = 343.25;
  KSoda_Sang    = 0.000474;
  KSoda_Ohkawa  = 0.00127;
  Approach      = 0.5;
  m_AOutEst     = dNAN;
  m_TOutEst     = dNAN;
  m_xPrev       = dNAN;
  m_XSodaPrev   = dNAN;

  TankVol       = 1000.0;

  dSolPrecip  = 0.0;
  dOccSoda    = 0.0;
  dShapeRatio = FeedShapeFact/ProdShapeFact;
  dGrowthRate = 0.0;
  dGrowthRateFactor = 0.0;
  dSodaFac    = 0.0;
  dYield      = 0.0;
  dTin        = Std_T;
  dTout       = Std_T;
  dDiamin     = 0.0;
  dDiamout    = 0.0;
  dSALin      = 0.0;
  dSALout     = 0.0;
  dQvin       = 0.0;
  dQvout      = 0.0;
  dACin       = 0.0;
  dACout      = 0.0;
  dASat       = 0.0;
  dResidenceTime = 0.0;
  dThermalLoss   = 0.0;
  dThermTempDrop = 0.0;
  //m_dReactionHeat  = 0.0;
  m_StdHfFd0  = 0;
  m_StdHfPr0  = 0;
  m_StdHOR0  = 0;
  m_MdlHfFd0  = 0;
  m_MdlHfPr0  = 0;
  m_MdlHOR0  = 0;
  m_UsedHOR0 = 0;
  m_StdHfFdT  = 0;
  m_StdHfPrT  = 0;
  m_StdHORT  = 0;
  m_MdlHfFdT  = 0;
  m_MdlHfPrT  = 0;
  m_MdlHORT  = 0;
  m_UsedHORT = 0;
  }

//--------------------------------------------------------------------------

void Precip::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  static DDBValueLst DDB0[]={
    {YM_None,  "Simple" },
    {YM_Yield1,"YieldMethod"},
#if NewPrecipCode2
    {YM_Yield2,"YieldMethod2003"},
#endif
#if NewPrecipCode3
    {YM_Yield3,"YieldMethod2003x"},
#endif
    {0}};
  static DDBValueLst DDB1[]={
    {GRM_White,    "White"},
    {GRM_Cresswell,"Cresswell"},
    {GRM_WhiteBateman,"WhiteBateman"},
    {0}};
  static DDBValueLst DDB2[]={
    {SPM_Sang,  "Sang"},
    {SPM_Ohkawa,"Ohkawa"},
    {0}};
  static DDBValueLst DDB3[]={
    {THL_None,          "None" },
    {THL_TempDrop,      "TempDrop"},
    {THL_FixedHeatFlow, "FixedLoss"},
    {THL_HeatFlowFrac,  "LossFraction"},
    {0}};
  static DDBValueLst DDB4[]={
    {RHM_Calc,          "Calculated" },
    {RHM_Override1,     "Override1"},
    {0}};

  DDB.Visibility();
  DDB.Text  ("");
  DDB.CheckBox    ("On",               "",DC_,       "",       &bOnLine         , this, isParm|SetOnChange);
  DDB.Text  ("");
  RB.Add_OnOff(DDB);//, False);
  DDB.Text("");
  DDB.Text  ("Requirements");
  DDB.Double("TankVol",	       "V",    DC_Vol,  "m^3",   &TankVol         , this,	isParm);
  DDB.Byte  ("Method",         "",     DC_,     "",      &fMode           , this, isParm|SetOnChange, DDB0);

  DDB.Visibility(NSHM_All, fMode==YM_Yield1);
	DDB.Double("",               "E/R",  DC_T,    "K",     &ER_1            , this,	isParm);
	DDB.Double("",	             "K",    DC_,     "",      &K_1             , this,	isParm);
  //DDB.Text  ("Occluded Soda Values");
	DDB.Double("",               "Ksoda",DC_,     "",      &Ksoda_1         , this,	isParm);

  DDB.Visibility(NSHM_All, fMode==YM_Yield3);
    //DDB.Text("Alumina Precipitation Values");
  //DDB.Double("LossFac",        "",     DC_Frac, "%",     &LossFac         , this,	isParm);
  DDB.Double("AgglomFac",	     "",     DC_,     "",      &AggParm         , this,	isParm);
  DDB.Double("FeedShapeFactor","",     DC_,     "",      &FeedShapeFact   , this,	isParm);
  DDB.Double("ProdShapeFactor","",     DC_,     "",      &ProdShapeFact   , this,	isParm);
  DDB.Visibility(NSHM_All, fMode==YM_Yield2 || fMode==YM_Yield3);
  DDB.Byte  ("GrowthMethod",   "",     DC_,     "",      &iGrowthRateMethod,this, isParm|SetOnChange, DDB1);
  DDB.Visibility(NSHM_All, (fMode==YM_Yield2 || fMode==YM_Yield3) && iGrowthRateMethod==GRM_White);
  DDB.Double("ER_White",       "",     DC_T,    "K",     &ER_White        , this,	isParm);
  DDB.Double("K_White",	       "",     DC_,     "",      &K_White         , this,	isParm);
  DDB.Double("gF_White",    	 "",     DC_,     "",      &gF_White        , this,	isParm);
  DDB.Visibility(NSHM_All, (fMode==YM_Yield2 || fMode==YM_Yield3) && iGrowthRateMethod==GRM_Cresswell);
  DDB.Double("ER_Cresswell",   "",     DC_T,    "K",     &ER_Cresswell    , this,	isParm);
  DDB.Double("Tref",           "",     DC_T,    "K",     &Tref            , this,	isParm|InitHidden);
  DDB.Double("K_Cresswell",	   "",     DC_,     "",      &K_Cresswell     , this,	isParm);
  DDB.Double("gF_Cresswell",	 "",     DC_,     "",      &gF_Cresswell    , this,	isParm);
  DDB.Visibility(NSHM_All, (fMode==YM_Yield2 || fMode==YM_Yield3) && iGrowthRateMethod==GRM_WhiteBateman);
  DDB.Double("ER_WhiteBateman","",       DC_T,    "K",     &ER_WhBateman    , this,	isParm);
  DDB.Double("K_WhiteBateman", "",       DC_,     "",      &K_WhBateman     , this,	isParm);
  DDB.Double("gF_WhiteBateman","",       DC_,     "",      &gF_WhBateman    , this,	isParm);
  DDB.Visibility(NSHM_All, fMode==YM_Yield2 || fMode==YM_Yield3);
  DDB.Byte  ("SodaPrecipMethod","",      DC_,     "",      &iSodaPrecipMethod,this, isParm|SetOnChange, DDB2);
  DDB.Visibility(NSHM_All, (fMode==YM_Yield2 || fMode==YM_Yield3) && iSodaPrecipMethod==SPM_Sang);
  DDB.Double("Ksoda_Sang",     "",       DC_,     "",      &KSoda_Sang      , this,	isParm);
  DDB.Visibility(NSHM_All, (fMode==YM_Yield2 || fMode==YM_Yield3) && iSodaPrecipMethod==SPM_Ohkawa);
  DDB.Double("Ksoda_Ohkawa",   "",       DC_,     "",      &KSoda_Ohkawa    , this,	isParm);
  DDB.Visibility(NSHM_All, fMode==YM_Yield3);
  DDB.Double("OxApproach",     "",       DC_Conc, "g/L",   &Approach        , this, isParm);
  DDB.Visibility();
  DDB.Byte  ("ReactionHeatMethod","",    DC_,     "",      &iRctHeatMethod, this, isParm|SetOnChange, DDB4);
  DDB.Visibility(NSHM_All, iRctHeatMethod==RHM_Override1);
  DDB.Double("HOR_a",          "",       DC_,     "",      &dHOR_a        , this, isParm);
  DDB.Double("HOR_b",          "",       DC_,     "",      &dHOR_b        , this, isParm);
  DDB.Visibility();
  DDB.Byte  ("ThermalLossMethod","",     DC_,     "",      &iThermalLossMethod,this, isParm|SetOnChange, DDB3);
  DDB.Visibility(NSHM_All, iThermalLossMethod==THL_TempDrop);
	DDB.Double("Temp_Drop",      "",       DC_dT,   "C",     &TempDropRqd   , this, isParm);
  DDB.Visibility(NSHM_All, iThermalLossMethod==THL_FixedHeatFlow);
  DDB.Double("ThermalLossRqd", "",       DC_Pwr,  "kW",    &ThermalLossRqd, this, isParm);
  DDB.Visibility(NSHM_All, iThermalLossMethod==THL_HeatFlowFrac);
  DDB.Double("ThermalLossFrac","",       DC_Frac, "%",     &ThermalLossFrac,this, isParm);
  DDB.Visibility();

  DDB.Text  ("");
  DDB.Text  ("Results Tank");
  DDB.Double("ResidenceTime",  "",       DC_Time, "h",     &dResidenceTime, this,	isResult);
  DDB.Double("Yield",          "",       DC_Conc, "kg/m^3",&dYield        , this,	isResult);
	DDB.Double("THAPrecip",      "",       DC_Qm,   "kg/s",  &dSolPrecip    , this,	isResult);
  DDB.Double("OccludedSoda",   "",       DC_Qm,   "kg/s",  &dOccSoda      , this,	isResult);
  DDB.Text  ("Results");
  DDB.Visibility();
  DDB.Double("ASat",           "",       DC_Conc, "g/L",   &dASat         , this, isResult);
  DDB.Visibility(NSHM_All, fMode==YM_Yield3);
  DDB.Double("ShapeRatio",     "",       DC_,     "",      &dShapeRatio   , this,	isResult|InitHidden|noFile);
  DDB.Visibility(NSHM_All, fMode==YM_Yield1 || fMode==YM_Yield2 || fMode==YM_Yield3);
  DDB.Double("GrowthRateFactor", "kG",   DC_Ldt,  "m/s",   &dGrowthRateFactor   , this,	isResult);
  DDB.Double("GrowthRate",     "",       DC_,     "",      &dGrowthRate   , this,	isResult);
  DDB.Visibility(NSHM_All, fMode==YM_Yield3);
  DDB.Double("SodaFactor",     "",       DC_,     "",      &dSodaFac      , this,	isResult);
  DDB.Visibility();
	DDB.Double("ACin",           "",       DC_,     "",      &dACin         , this, isResult/*|noFile*/);
	DDB.Double("ACout",          "",       DC_,     "",      &dACout        , this, isResult);
	DDB.Double("TempIn",         "Ti",     DC_T,    "C",     &dTin          , this, isResult/*|noFile*/);
	DDB.Double("TempOut",        "To",     DC_T,    "C",     &dTout         , this, isResult);
  //DDB.Double("ReactionHeat",   "",       DC_Pwr,  "kW",    &dReactionHeat , this, isResult|InitHidden);
  DDB.Double("StdHfFd@0",      "",       DC_Pwr,  "kW",    &m_StdHfFd0    , this, isResult|InitHidden);
  DDB.Double("StdHfPr@0",      "",       DC_Pwr,  "kW",    &m_StdHfPr0    , this, isResult|InitHidden);
  DDB.Double("StdHOR@0",       "",       DC_Pwr,  "kW",    &m_StdHOR0     , this, isResult|InitHidden);
  DDB.Double("StdHfFd@T",      "",       DC_Pwr,  "kW",    &m_StdHfFdT    , this, isResult|InitHidden);
  DDB.Double("StdHfPr@T",      "",       DC_Pwr,  "kW",    &m_StdHfPrT    , this, isResult|InitHidden);
  DDB.Double("StdHOR@T",       "",       DC_Pwr,  "kW",    &m_StdHORT     , this, isResult|InitHidden);
  DDB.Double("MdlHfFd@0",      "",       DC_Pwr,  "kW",    &m_MdlHfFd0    , this, isResult|InitHidden);
  DDB.Double("MdlHfPr@0",      "",       DC_Pwr,  "kW",    &m_MdlHfPr0    , this, isResult|InitHidden);
  DDB.Double("MdlHOR@0",       "",       DC_Pwr,  "kW",    &m_MdlHOR0     , this, isResult|InitHidden);
  DDB.Double("MdlHfFd@T",      "",       DC_Pwr,  "kW",    &m_MdlHfFdT    , this, isResult|InitHidden);
  DDB.Double("MdlHfPr@T",      "",       DC_Pwr,  "kW",    &m_MdlHfPrT    , this, isResult|InitHidden);
  DDB.Double("MdlHOR@T",       "",       DC_Pwr,  "kW",    &m_MdlHORT     , this, isResult|InitHidden);
  DDB.Double("UsedHOR@0",      "",       DC_Pwr,  "kW",    &m_UsedHOR0    , this, isResult|InitHidden);
  DDB.Double("UsedHOR@T",      "",       DC_Pwr,  "kW",    &m_UsedHORT    , this, isResult|InitHidden);
  //DDB.Double("ActMdl@0",       "",       DC_Pwr,  "kW",    &m_MdlHOR0     , this, isResult|InitHidden);
  DDB.Double("ThermalLoss",    "",       DC_Pwr,  "kW",    &dThermalLoss  , this, isResult);
  DDB.Double("ThermalTempDrop","",       DC_dT,   "C",     &dThermTempDrop, this, isResult);
  DDB.Visibility(NSHM_All, fMode==YM_Yield1 || fMode==YM_Yield2 || fMode==YM_Yield3);
	DDB.Double("PartDiamIn",     "Di",     DC_L,    "um",    &dDiamin       , this, isResult/*|noFile*/);
	DDB.Double("PartDiamOut",    "Do",     DC_L,    "um",    &dDiamout      , this, isResult/*|noFile*/);
  DDB.Double("SeedSurfAreaLIn","SALi",   DC_SurfAreaL, "m^2/L", &dSALin   , this, isResult/*|noFile*/|InitHidden);
  DDB.Double("SeedSurfAreaLOut","SALo",  DC_SurfAreaL, "m^2/L", &dSALout  , this, isResult/*|noFile*/|InitHidden);
  DDB.Visibility();
	DDB.Double("Vol_FlowIn",     "Qvi",    DC_Qv,   "L/s",   &dQvin         , this, isResult/*|noFile*/);
	DDB.Double("Vol_FlowOut",    "Qvo",    DC_Qv,   "L/s",   &dQvout        , this, isResult/*|noFile*/);
  DDB.Double("AOutEst",          "",     DC_,     "",      &m_AOutEst     , this, isParmConstruct|InitHidden);
  /*DDB.Visibility(NSHM_All, fMode==YM_Yield1 || fMode==YM_Yield2 || fMode==YM_Yield3);
  DDB.Double      ("BoundSoda",        "",DC_Frac,   "%",      &BoundSoda       , this, 0);
  DDB.Double      ("ACeq",             "",DC_,       "",       &ACeq            , this, 0);*/

  DDB.Text("");
  BuildDataDefnShowIOs(DDB);

  RB.BuildDataDefn(DDB);

  if (NetDynamicMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag Precip::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;

  return 0;
  }

//---------------------------------------------------------------------------

flag Precip::ValidateData(ValidateDataBlk & VDB)
  {
  Tref=ValidateRange(VDB, "Tref", C2K(20.0), Tref, C2K(110.0));
  return MN_Surge::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

void Precip::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NoProcessJoins(); j++)
        SetPBJoinPressure(j, AtmosPress(IODatum_Term(j)), true, true);
      break;
      }
    case NM_Dynamic:
      MdlNode::EvalJoinPressures(JoinMask);
      break;
    }
  };
 
//--------------------------------------------------------------------------

void Precip::EvalJoinFlows(int JoinNo)
  {
  switch (NetMethod())
    {
    case NM_Probal:
    case NM_Dynamic:
      break;
    }
  };

//--------------------------------------------------------------------------

void Precip::AdjustMasses(SpConduit & Qp, MIBayer & Bm, double & x, double SodaFac)
  {
  double &AluminaMass  = *Qp.VPtr[Alumina.LiqPhInx()];     // Al2O3
  double &WaterMass    = *Qp.VPtr[Water.LiqPhInx()];       // H2O
  double &THAMass      = *Qp.VPtr[THA.SolPhInx()];         // Al2O3.3H2O (Gibbsite)
  double &CausticMass  = *Qp.VPtr[CausticSoda.LiqPhInx()]; // NaOH
  double &Na2OMass     = *Qp.VPtr[OccSoda.SolPhInx()];     // Na2O

  const double MW_H2O       = ::Water.MW;            //18.0152800
  const double MW_Na2O      = ::OccSoda.MW;          //61.9789360
  const double MW_NaOH      = ::CausticSoda.MW;      //39.9971080
  const double MW_Al2O3     = ::Alumina.MW;          //101.961278
  const double MW_Al2O3_3H2O= ::THA.MW;              //156.007118

  const double Fact = MW_Al2O3/MW_Al2O3_3H2O; // 0.654;

  double XSoda = x/100.0*SodaFac;

  if (fabs(XSoda)>1.0e-9)
    {
    //check for limit problem (specie availability)...
    if (Na2OMass + XSoda<1e-9)
      { //set to limit
      XSoda = 1e-9 - Na2OMass;
      x = XSoda*100.0/SodaFac;
      }
    if (CausticMass - (MW_NaOH*2)/MW_Na2O*XSoda<1e-12)
      { //set to limit
      XSoda = (CausticMass - 1e-12)*MW_Na2O/(MW_NaOH*2);
      if (Na2OMass + XSoda<1e-9)
        XSoda = 1e-9 - Na2OMass;
      x = XSoda*100.0/SodaFac;
      }
    }
  //adjust masses...
  AluminaMass = AluminaMass - Fact*x;
  THAMass     = THAMass     + x;
  WaterMass   = WaterMass   - (1.0-Fact)*x + MW_H2O/MW_Na2O*XSoda;
  CausticMass = CausticMass - (MW_NaOH*2)/MW_Na2O*XSoda;
  Na2OMass    = Na2OMass    + XSoda;
  }

CToleranceBlock Precip::sm_SolTol(TBF_BothSys, "Precip::Solubility", 0, 1.0e-12, 100, TBF_Fixed|TBF_UseAbs|TBF_UseRel|TBF_UseMax);

double Precip::PerformAluminaSolubility(SpConduit & Qp, MIBayer & Bm, double TRqd, double ARqd, double THARqd, double NoPerSec, double Na2OFac, double SSat, double Cin, flag & ConvergeErr)
  {
  //   2 AlO2 + 4 H2O <==> Al2O3.3H2O + 2 OH
  //or Na2O + Al2O3 + 4 H2O <==> Al2O3.3H2O + 2 NaOH

  // x is the Fraction of Alumina which precipitates as the hydrate
  // ie x is deposition rate of new THA crystal


  bool AdjustT=!Valid(TRqd);
  double T = AdjustT ? Qp.Temp() : TRqd;

  const double ESoda=2535.0; //constant 2535K
  double A = Bm.AluminaConc(T);

  double &AluminaMass  = *Qp.VPtr[Alumina.LiqPhInx()];     // Al2O3

  const double MW_Al2O3     = ::Alumina.MW;          //101.961278
  const double MW_Al2O3_3H2O= ::THA.MW;              //156.007118
  const double Fact = MW_Al2O3/MW_Al2O3_3H2O; // 0.654;

  for (int Iter=sm_SolTol.GetMaxIters(); Iter; Iter--)
    {
    double x = AluminaMass*(1.0-ARqd/GTZ(A))/Fact;

    //Soda Precipitation...
    double SodaFac;
    if (Valid(Cin))
      {//Sang
      const double xx = SSat*Cin;
      SodaFac = Na2OFac*xx*xx;
      }
    else
      {//Ohkawa, Tsuneizumi and Hirao
      SodaFac = Na2OFac*exp(ESoda/T)*SSat*SSat;
      }

    AdjustMasses(Qp, Bm, x, SodaFac);

    if (iRctHeatMethod==RHM_Override1)
      {
      Qp.SetTemp(T);
      }
    else
      {
      Qp.Model()->ClrStatesOK();
      if (AdjustT)
        T=Qp.Temp();
      }

    A = Bm.AluminaConc(T);
    if (sm_SolTol.ConvergedVV(A, ARqd))
      break;
    if (fabs(x)<1e-22)
      {//problem!!!
      Iter=0;
      break;
      }
    }
  ConvergeErr = (Iter==0);
  //SetCI(5, ConvergeErr);

  if (NoPerSec>0.0)
    {
    CSzSSA * pSSA = CSzSSA::Ptr(Qp.Model(), false);
    pSSA->SetSAMFromFlow(Bm.THAMassFlow(), NoPerSec);
    }

  return Bm.AluminaConc(T);
  }

//--------------------------------------------------------------------------

CToleranceBlock Precip::sm_AOutTol(TBF_BothSys, "Precip:AOutFnd", 0, 1.0e-12, 100, TBF_Fixed|TBF_UseAbs|TBF_UseRel|TBF_UseMax);

void Precip::DoPrecip(SpConduit & Qp)
  {
  dThermalLoss = 0.0;
  dYield  = 0.0;
  dSALin  = 0.0;
  dDiamin = 0.0;
  bool IsOff = false;
  MIBayer * pBm =Qp.Model()->GetMIBayer();
  if (bOnLine)
    {
    //check feed stream is Bayer Properties Model...
    SetCI(1, pBm==NULL);
    if (pBm)
      {
      CSzSSA * pSSA = CSzSSA::Ptr(Qp.Model(), false);
      MIBayer & Bm=*pBm;
      double LiqIn   = Qp.QMass(som_Liq); // kg/s
			double SolIn   = Qp.QMass(som_Sol); // kg/s
      double THAIn   = Qp.VMass[THA.SolPhInx()]; // kg/s
      double OcNaIn  = Qp.VMass[OccSoda.SolPhInx()]; // kg/s
      double Al2O3In = Qp.VMass[Alumina.LiqPhInx()]; // kg/s
      double T       = Qp.Temp(); // temperature (K)
      double P       = Qp.Press();
      double Ain     = Bm.AluminaConc(T); //Al2O3
      double Cin     = Bm.CausticConc(T);
      double Sin     = Bm.SodaConc(T);
      double ASat    = Bm.AluminaConcSat(T); // Equilibrium Al2O3 liquor concentration @ T
      double Qvin    = Qp.QVolume(som_Liq)*3600.0; // Liquor volumetric flow rate @ T (m3/h)
      double Qvin25  = Qp.QVolume(som_Liq, C2K(25.0), P)*3600.0; // m3/h
      double Rhos    = Bm.THADens(T);
      double SSat    = (Ain-ASat)/GTZ(Cin);
      double NoPerSec= pSSA ? pSSA->PartNumPerSec() : 0.0;
      double Hfin    = Qp.totHf(som_ALL, T, P);
      double CpIn    = Qp.totCp(som_ALL, T, P);
      dTin    = T;
      dACin   = Bm.AtoC();
      dQvin   = Qp.QVolume();
      if (pSSA)
        {
  		  dSALin  = pSSA->SpecificSurfaceAreaVol(); // m^2/L
        dDiamin = pSSA->PartDiamFromSAM();
        }

      //evaluate reactions (if present)...
      RB.EvalProducts(Qp);

      m_StdHfFd0=0.0;
      m_StdHfPr0=0.0;
      m_StdHOR0=0.0;
      m_StdHfFdT=0.0;
      m_StdHfPrT=0.0;
      m_StdHORT=0.0;
      m_MdlHfFd0=0.0;
      m_MdlHfPr0=0.0;
      m_MdlHOR0=0.0;
      m_MdlHfFdT=0.0;
      m_MdlHfPrT=0.0;
      m_MdlHORT=0.0;

      SetCI(5, (fMode != YM_None) && pSSA==NULL);
			if (pSSA && SSat > 1e-6)
        {
        // Calc Heat of reaction using the Specie Model
        StkSpConduit Std("Std", chLINEID(), this);
        Std().ChangeModel("Standard", false); // Standard SysCAD
        double TFeed=Qp.Temp();
        double PFeed=Qp.Press();
        m_StdHfFd0=Std().totHf(som_ALL, ZeroCinK, PFeed, Qp);
        m_StdHfFdT=Std().totHf(som_ALL, TFeed, PFeed, Qp);
        m_MdlHfFd0=Qp.totHf(som_ALL, ZeroCinK, PFeed);
        m_MdlHfFdT=Qp.totHf(som_ALL, TFeed, PFeed);

        const double MW_H2O       = ::Water.MW;            //18.0152800
        const double MW_Na2O      = ::OccSoda.MW;          //61.9789360
        const double MW_Al2O3     = ::Alumina.MW;          //101.961278
        const double MW_Al2O3_3H2O= ::THA.MW;              //156.007118
        const double MW_NaOH      = ::CausticSoda.MW;      //39.9971080
        const double MW_Na2C2O4   = ::SodiumOxalate.MW;    //133.999136
        const double MW_Na2CO3    = ::SodiumCarbonate.MW;  //105.989
        //const double MW_NaCl      = ::SodiumChloride.MW;   //58.4425
        //const double MW_Na2SO4    = ::SodiumSulphate.MW;   //142.043
        //const double MW_Na2C5O7   = ::Organics.MW;         //218.030
        const double FacAl = MW_Al2O3/MW_Al2O3_3H2O; //Alpha = 0.6538; // Ratio of molecular weights (Al2O3/Al2O3.3H2O)
        const double AluminaMass_in  = Qp.VMass[Alumina.LiqPhInx()];     // Al2O3
        const double Na2OMass_in     = Qp.VMass[OccSoda.SolPhInx()];     // Na2O
        if (fMode == YM_Yield1)
					{
          const double Alpha   = FacAl; //102./156.;
          if (1)
            {
            //using previous estimates stuffs things up!!! The OccSoda goes WRONG! Until resolved do not use prev values!
            m_TOutEst = dNAN;
            m_xPrev = dNAN;
            }
          else
            {
            if (!Valid(m_TOutEst))
              m_TOutEst = T;
            if (Valid(m_xPrev) && Valid(m_XSodaPrev))
              {
              AdjustMasses(Qp, Bm, m_xPrev, m_XSodaPrev); //based on previous solution
              }
            }
					if (!Valid(m_AOutEst) || m_AOutEst<1e-6)
						m_AOutEst = 0.99*Ain;
          flag ConvergeErr;
  				m_AOutEst = PerformAluminaSolubility(Qp, Bm, m_TOutEst, m_AOutEst, dNAN, NoPerSec, Ksoda_1, SSat, dNAN, ConvergeErr);

					//this could be more effecient??? Use Brent ???
          double Qvout25, Cout, AoutPrev, V, Afact;
          int ZeroCnt=0;
					for (int Iter=100; Iter; Iter--)
						{
						T        = Qp.Temp(); // K
						Cout     = Bm.SodaConc(T); //g/l
						AoutPrev = m_AOutEst;
						Qvout25  = (Alpha*Rhos-Ain)/(Alpha*Rhos-m_AOutEst)*Qvin25;
						V        = TankVol*1000.0; // Liters
            Afact    = Sqr((m_AOutEst-ASat)/GTZ(Cout));
            dGrowthRateFactor = K_1*Exps(-ER_1/T); // Growth rate White  //m/s
            dGrowthRate = dGrowthRateFactor*Afact;
						m_AOutEst = (Qvin25*Ain - dGrowthRate*dSALin*V)/GTZ(Qvout25);
						m_AOutEst = GTZ(0.5*(m_AOutEst+AoutPrev));
    				m_AOutEst = PerformAluminaSolubility(Qp, Bm, T, m_AOutEst, dNAN, NoPerSec, Ksoda_1/*1.27e-3*/, SSat, dNAN, ConvergeErr);
    
            if (iRctHeatMethod==RHM_Override1)
              {
              //NB: msCp * MassFlow = totCp
              double Cp = Qp.totCp(som_ALL, T, P);
              double THAOut  = Qp.VMass[THA.SolPhInx()];
              double SolPrecip = THAOut - THAIn; //kg/s
              double CalcHOR = dHOR_a + (dHOR_b * K2C(T)); //kJ/kg
              double enthIn = CpIn * K2C(dTin);
              //double NewT = (SolPrecip*CalcHOR + enthIn) / Cp;
              double NewT = (SolPrecip*CalcHOR + enthIn) / CpIn;

              if (1)
                {
                Qp.Set_totHz(Qp.totHz(som_ALL, TFeed, PFeed)+CalcHOR*SolPrecip);
                }
              else
                {
                double enthIn = CpIn * K2C(dTin);
                double NewT = (SolPrecip*CalcHOR + enthIn) / Cp;
                Qp.SetTemp(C2K(NewT));
                }

              m_UsedHOR0=-dHOR_a*SolPrecip;
              m_UsedHORT=-CalcHOR*SolPrecip;
              }

            if (fabs(m_AOutEst-AoutPrev)<1.0e-9*Ain)
              {
              m_TOutEst = T;
              const double AluminaMass_out  = Qp.VMass[Alumina.LiqPhInx()];     // Al2O3
              m_xPrev = (AluminaMass_in - AluminaMass_out)/FacAl;
              const double Na2OMass_out     = Qp.VMass[OccSoda.SolPhInx()];     // Na2O
              m_XSodaPrev = (Na2OMass_out - Na2OMass_in);
							break;
              }

            if (m_AOutEst<1e-6)
              ZeroCnt++;
            if (ZeroCnt>3)
              Iter = 1; //problem...
						}
	        SetCI(2, Iter==0);
	        SetCI(3, m_AOutEst<1e-6);
	        SetCI(4, ConvergeErr);
          if (ConvergeErr || Iter<2)
            {//was error, do not use previous estimate
            m_TOutEst = dNAN;
            m_xPrev = dNAN;
            m_XSodaPrev = dNAN;
            }
					}
#if NewPrecipCode2
        else if (fMode==YM_Yield2)
					{
          //The growth and SodaFactor equations are using C, etc as Na2CO3 equivilant, 
          //shouldn't they be as Na2O equivilant!!!
          const double Alpha   = FacAl; //102./156.;
          double KSodaFac = (iSodaPrecipMethod==SPM_Sang ? KSoda_Sang : KSoda_Ohkawa);
          double Cin_ = (iSodaPrecipMethod==SPM_Sang ? Cin*MW_Na2O/MW_Na2CO3 : dNAN);
          SSat = (Ain-ASat)/GTZ(Cin*MW_Na2O/MW_Na2CO3); //Na2O equiv

bool bUsePrevAsEstimate = false;
          if (bUsePrevAsEstimate)
            {
            if (!Valid(m_TOutEst))
              m_TOutEst = T;
            if (Valid(m_xPrev) && Valid(m_XSodaPrev))
             AdjustMasses(Qp, Bm, m_xPrev, m_XSodaPrev); //based on previous solution
  					if (!Valid(m_AOutEst) || m_AOutEst<1e-6)
	  					m_AOutEst = 0.98*Ain;
            }
          else
            {
            m_TOutEst = T;
            m_AOutEst = ((ASat+Ain)/2.0)*1.01;
            }
          flag ConvergeErr;
  				m_AOutEst = PerformAluminaSolubility(Qp, Bm, m_TOutEst, m_AOutEst, dNAN, NoPerSec, KSodaFac, SSat, Cin_, ConvergeErr);

					//this could be more effecient??? Use Brent ???
					double V = TankVol*1000.0; // Liters
          double Qvout25, Cout, AoutPrev, Afact;
          int ZeroCnt=0;
					for (int Iter=100; Iter; Iter--)
						{
						T        = Qp.Temp(); // K
						Cout     = Bm.SodaConc(T)*MW_Na2O/MW_Na2CO3; //Na2O equiv
						AoutPrev = m_AOutEst;
						Qvout25  = (Alpha*Rhos-Ain)/(Alpha*Rhos-m_AOutEst)*Qvin25;
            /*if (iSodaPrecipMethod==SPM_Sang)
              Afact    = Sqr((m_AOutEst-ASat));
            else
              Afact    = Sqr((m_AOutEst-ASat)/GTZ(Cout));*/
            Afact    = Sqr((m_AOutEst-ASat)/GTZ(Cout));
            //dGrowthRateFactor = K_1*Exps(-ER_1/T); // Growth rate
            if (iGrowthRateMethod==GRM_White)
              {
              double x = Exps(-ER_White/T);
              dGrowthRateFactor = gF_White*K_White*x;
              }
            else if (iGrowthRateMethod==GRM_Cresswell)
              {
              double C25 = Bm.CausticConc(C2K(25.0)); // Caustic concentration @ 25C (gNa2CO3/l)
		          double m_C25 = C25*MW_Na2O/MW_Na2CO3; // Caustic concentration @ 25C (gNa2O/l)
		          double x = Exps(-ER_Cresswell*((1.0/T)-(1.0/Tref)));
              dGrowthRateFactor = gF_Cresswell*K_Cresswell*x/sqrt(m_C25/100.0);
              }
            else if (iGrowthRateMethod==GRM_WhiteBateman)
              {
              double C25 = Bm.CausticConc(C2K(25.0)); // Caustic concentration @ 25C (gNa2CO3/l)
		          double m_C25 = C25*MW_Na2O/MW_Na2CO3; // Caustic concentration @ 25C (gNa2O/l)
		          double x = Exps(-ER_WhBateman/T);
              dGrowthRateFactor = gF_WhBateman*K_WhBateman*x/sqrt(m_C25);
              }
            dGrowthRate = dGrowthRateFactor*Afact;
						m_AOutEst = (Qvin25*Ain - dGrowthRate*dSALin/1000.0*V)/GTZ(Qvout25);
						m_AOutEst = GTZ(0.5*(m_AOutEst+AoutPrev));
            //Should SSat be recalulated as (Aout-ASat)/GTZ(Cout) ? (or does Afact effectively do this?)
    				m_AOutEst = PerformAluminaSolubility(Qp, Bm, T, m_AOutEst, dNAN, NoPerSec, KSodaFac, SSat, Cin_, ConvergeErr);

            if (iRctHeatMethod==RHM_Override1)
              {
              //NB: msCp * MassFlow = totCp
              double Cp = Qp.totCp(som_ALL, T, P);
              double THAOut  = Qp.VMass[THA.SolPhInx()];
              double SolPrecip = THAOut - THAIn; //kg/s
              double CalcHOR = dHOR_a + (dHOR_b * K2C(T)); //kJ/kg
              if (1)
                {
                Qp.Set_totHz(Qp.totHz(som_ALL, TFeed, PFeed)+CalcHOR*SolPrecip);
                }
              else
                {
                double enthIn = CpIn * K2C(dTin);
                double NewT = (SolPrecip*CalcHOR + enthIn) / Cp;
                Qp.SetTemp(C2K(NewT));
                }

              m_UsedHOR0=-dHOR_a*SolPrecip;
              m_UsedHORT=-CalcHOR*SolPrecip;
              }

            if (fabs(m_AOutEst-AoutPrev)<1.0e-9*Ain)
              {
              m_TOutEst = T;
              const double AluminaMass_out  = Qp.VMass[Alumina.LiqPhInx()];     // Al2O3
              m_xPrev = (AluminaMass_in - AluminaMass_out)/FacAl;
              const double Na2OMass_out     = Qp.VMass[OccSoda.SolPhInx()];     // Na2O
              m_XSodaPrev = (Na2OMass_out - Na2OMass_in);
							break;//found a solution!
              }

            if (m_AOutEst<1e-6)
              ZeroCnt++;
            if (ZeroCnt>3)
              Iter = 1; //problem...
						}
	        SetCI(2, Iter==0);
	        SetCI(3, m_AOutEst<1e-6);
	        SetCI(4, ConvergeErr);
					}
#endif
#if NewPrecipCode3
        else if (fMode == YM_Yield3)
					{
          const double Feed_Al2O3    = Al2O3In/1000.0*3600.0; // Mass flow rate of Al2O3 (t/h)
          const double Feed_FlowLiqT = Qvin; // Liquor volumetric flow rate @ T (m3/h)
          const double Feed_Tliq     = LiqIn/1000.0*3600.0; // Liquor flow rate (t/h)
          const double Feed_Tsol     = Qp.QMass(som_Sol)/1000.0*3600.0; // Solids flow rate (t/h)
          const double Feed_Na2C2O4  = Qp.Qm(SodiumOxalate.LiqPhInx())/1000.0*3600.0; // Mass flow rate of oxalate (t/h)
          const double Feed_L32      = pSSA->PartDiamFromSAM()*1.0e6; // Particle Sauter mean size (um)

          const double Fd_THA        = Qp.Qm(THA.SolPhInx());
          const double Fd_Na2O       = Qp.Qm(OccSoda.SolPhInx());
          const double Fd_Na2C2O4s   = Qp.Qm(OxalateSol.SolPhInx());
          const double Fd_H2Ol       = Qp.Qm(Water.LiqPhInx());
          const double Fd_Al2O3      = Qp.Qm(Alumina.LiqPhInx());
          const double Fd_NaOH       = Qp.Qm(CausticSoda.LiqPhInx());
          const double Fd_Na2C2O4l   = Qp.Qm(SodiumOxalate.LiqPhInx());

          //todo: get FeedShapeFact from input stream similar to diameter
          dShapeRatio = FeedShapeFact/ProdShapeFact;

          double Ai = 1000.0*Feed_Al2O3/Feed_FlowLiqT; // Feed:Al2O3 liquor concentration @ T
          double Aeq = ASat; // Feed:Equilibrium Al2O3 liquor concentration @ T
          // First estimate of product Al2O3 liquor concentration: Slightly less than Ai if Ai>Aeq, otherwise exactly equal to Ai
          double Ao = Aeq+(Ai-Aeq)*(Ai>Aeq ? 0.99 : 1);
          double oldAo;
	        if (1)
            {
            double LossFac = 0.005;
            // Heat Losses
            const double Feed_THeat    = (T-273.15)*Bm.msCp(som_ALL, T, Std_P)*(Feed_Tliq+Feed_Tsol); // Energy flux (kJ/h)
            double HeatLoss = -LossFac*Feed_THeat;
            }

          dGrowthRateFactor = 0.0;
          const int MaxIter = 100;
          int	Iter=0;
          do
            {
            oldAo = Ao;
            Aeq = Bm.AluminaConcSat(T);
            double Prod_DenLiq = Bm.Rho(som_Liq, T, Std_P)/1000.0; // Liquor density @ T (t/m3)
            // Eqn (16.18): Estimate of Product Liquor Volumetric flow rate (m3/h) for this iteration
            double QLo = (Feed_Tliq*FacAl - Feed_FlowLiqT*0.001*Ai)/(Prod_DenLiq*FacAl - 0.001*Ao);

            //Crystal growth rate...
            if (iGrowthRateMethod==GRM_White)
              {
              double x = Exps(-ER_White/T);
              dGrowthRateFactor = gF_White*K_White*x;
              }
            else if (iGrowthRateMethod==GRM_Cresswell)
              {
              double C25 = Bm.CausticConc(C2K(25.0)); // Caustic concentration @ 25C (gNa2CO3/l)
		          double m_C25 = C25*MW_Na2O/MW_Na2CO3; // Caustic concentration @ 25C (gNa2O/l)
		          double x = Exps(-ER_Cresswell*((1.0/T)-(1.0/Tref)));
              dGrowthRateFactor = gF_Cresswell*K_Cresswell*x/sqrt(m_C25/100.0);
              }
            else if (iGrowthRateMethod==GRM_WhiteBateman)
              {
              double C25 = Bm.CausticConc(C2K(25.0)); // Caustic concentration @ 25C (gNa2CO3/l)
		          double m_C25 = C25*MW_Na2O/MW_Na2CO3; // Caustic concentration @ 25C (gNa2O/l)
		          double x = Exps(-ER_WhBateman/T);
              dGrowthRateFactor = gF_WhBateman*K_WhBateman*x/sqrt(m_C25);
              }

            // Eqn (16.21): "beta" factor
            double Prod_NaOH     = Bm.M[CausticSoda.LiqPhInx()]/1000.0*3600.0; // Mass flow rate of NaOH (t/h)
            double Prod_DenSol   = Bm.THADens(T)/1000.0; //Bm.Rho(som_Sol, T, Std_P)/1000.0; // Solids density (t/m3)
            double Prod_L32      = pSSA->PartDiamFromSAM()*1.0e6; // Particle Sauter mean size (um)
            double Prod_Tsol     = Bm.Mass(som_Sol)/1000.0*3600.0; // Solids flow rate (t/h)
            double Prod_NbrFlux  = (Prod_Tsol/Prod_DenSol)*(6.0/PI)*1.0/pow(1e-6*Prod_L32,3);
            //double ProdShapeFact = (Iter==0 ? 0.75 : 0.7); //testing
            double Prod_SurfFlux = Prod_NbrFlux*(PI*pow(1e-6*Prod_L32,2))/ProdShapeFact; // Particle surface area flux (m2/h)
            double Prod_FlowLiqT = Bm.Volume(som_Liq)*3600.0;  // Liquor volumetric flow rate @ T (m3/h)
            double Prod_SurfArea = Prod_SurfFlux/Prod_FlowLiqT; // Solids Specific surface area (m2/m3 Liquor)
            double beta = 2.0*pow(Prod_NaOH*(MW_Na2O/(MW_NaOH*2)),2)/(Prod_DenSol*Prod_SurfFlux*TankVol*FacAl*(1e-6*dGrowthRateFactor));

            // See Eqn (16.23): "b": Coefficient of linear term
            double b = beta-2.0*Aeq/1000.0;

            // Eqn (16.23): Discriminant of quadratic
            double disc = beta*(beta+4*0.001*(Ai*(Feed_FlowLiqT/QLo)-Aeq));

            // Eqn (16.24): New estimate of Ao
		        if (disc>=0.0) 
              Ao = 500.0*(-b+sqrt(disc));   // factor 500 is .5*1000 (m3/l)
		        else
              Ao = Ai;

            // Deposition rate of new THA crystal: must be non negative, cannot be postive unless Ao>Aeq
            double dTHA = 0.0;
            if (Ao>Aeq)
              {
              // Eqn (16.5)
              dTHA = 0.001*(Feed_FlowLiqT*Ai - QLo*Ao)/FacAl;
              dTHA = Max(0.0, dTHA);
              }

            // Definition: Caustic concentration (Na2Obasis, @T)
            double m_CT = 1000.0*(MW_Na2O/(MW_NaOH*2))*Prod_NaOH/Prod_FlowLiqT;

            // Eqn (16.38): Supersaturation (Na2O A/C basis)
		        double SSat = (Ao-Aeq)/GTZ(m_CT);
            

            //Soda Precipitation
            // Deposition rate of new Na2O crystal
            if (iSodaPrecipMethod==SPM_Sang)
              {
              const double x = SSat*m_CT;
              dSodaFac = (KSoda_Sang*x*x);
              }
            else if (iSodaPrecipMethod==SPM_Ohkawa)
              {
              const double SodaER = 2535.0; // E/R factor Kelvin
              const double x = exp(SodaER/T);
              dSodaFac = (KSoda_Ohkawa*SSat*SSat*x);
              }
            double dNa2O = (dTHA/100.0)*dSodaFac;

            // Eqn (16.51): Oxalate Equilibrium concentration  (ALTERNATIVE MODELS POSSIBLE)
            double OxEquil = Bm.OxalateEquilibrium(T) + Approach;
            // Actual Oxalate concentration based on feed
            double OxConc = 1000.0*Feed_Na2C2O4/QLo;
            // Oxalate Precipitation Rate
            double dNa2C2O4 = QLo*(OxConc-OxEquil)/1000.0;

            //-------------------------------------------------------------------------
            //adjust composition:
            Bm.M[THA.SolPhInx()]            = Fd_THA + (dTHA/3.6);
            Bm.M[OccSoda.SolPhInx()]        = Fd_Na2O + (dNa2O/3.6);
            Bm.M[OxalateSol.SolPhInx()]     = Fd_Na2C2O4s + (dNa2C2O4/3.6);
            Bm.M[Water.LiqPhInx()]          = Fd_H2Ol - (MW_H2O*3/MW_Al2O3_3H2O)*dTHA/3.6 + (MW_H2O/MW_Na2O)*dNa2O/3.6;
            Bm.M[Alumina.LiqPhInx()]        = Fd_Al2O3 - (MW_Al2O3/MW_Al2O3_3H2O)*dTHA/3.6;
            Bm.M[CausticSoda.LiqPhInx()]    = Fd_NaOH - (MW_NaOH*2/MW_Na2O)*dNa2O/3.6;
            Bm.M[SodiumOxalate.LiqPhInx()]  = Fd_Na2C2O4l - dNa2C2O4/3.6;

            // Sensible Heat in product
            //Prod[THeat]=Feed[THeat]+dTHA*4.184*170+HeatLoss;
            //double NewTHeat = Feed_THeat+(dTHA*4.184*170)+HeatLoss;
            Bm.ClrStatesOK(); //clear to force recalc of properties
            double NewTemp = Qp.Temp();

            // Surface Properties
            // Eqn(16.40)
            Prod_Tsol = Bm.Mass(som_Sol)/1000.0*3600.0; // Solids flow rate (t/h)
            double New_L32 = Feed_L32*pow(dShapeRatio*Prod_Tsol/Feed_Tsol, AggParm/3.0);
            pSSA->SetSAMFromPartDiam(New_L32/1.0e6);
            //todo: set outlet ProdShapeFact similar to diameter

            Aeq = Bm.AluminaConcSat(T);

            if (1)
              {
              // Growth rate (check purposes only, should produce G1==G2 on convergence)
              // Eqn(16.11): Growth rate based on supersaturation
              double G1 = dGrowthRateFactor*pow((Ao-Aeq)/m_CT,2);
              // Eqn(16.9): Growth rate based on crystal deposition
              Prod_L32 = New_L32;
              Prod_FlowLiqT = Bm.Volume(som_Liq)*3600.0;  // Liquor volumetric flow rate @ T (m3/h)
              Prod_Tsol = Bm.Mass(som_Sol)/1000.0*3600.0; // Solids flow rate (t/h)
              Prod_NbrFlux = (Prod_Tsol/Prod_DenSol)*(6.0/PI)*1.0/pow(1e-6*Prod_L32,3);
              Prod_SurfFlux = Prod_NbrFlux*(PI*pow(1e-6*Prod_L32,2))/ProdShapeFact; // Particle surface area flux (m2/h)
              Prod_SurfArea = Prod_SurfFlux/Prod_FlowLiqT; // Solids Specific surface area (m2/m3 Liquor)
              double G2 = 1.0e6*2.0*dTHA/(Prod_DenSol*Prod_SurfArea*TankVol);
              }

            Iter++;
            }
          while (((Iter<3) || (fabs(Ao-oldAo)>0.0005)) && (Iter<MaxIter));
	        SetCI(2, Iter==MaxIter);
          }
#endif            
        else if (fMode == YM_None)
          {
          }

        // Calc Heat of reaction using the Specie Model
        m_StdHfPr0=Std().totHf(som_ALL, ZeroCinK, PFeed, Qp);      
        m_StdHfPrT=Std().totHf(som_ALL, TFeed, PFeed, Qp);
        m_MdlHfPr0=Qp.totHf(som_ALL, ZeroCinK, PFeed);
        m_MdlHfPrT=Qp.totHf(som_ALL, TFeed, PFeed);
        m_StdHOR0=m_StdHfPr0-m_StdHfFd0;
        m_StdHORT=m_StdHfPrT-m_StdHfFdT;
        m_MdlHOR0=m_MdlHfPr0-m_MdlHfFd0;
        m_MdlHORT=m_MdlHfPrT-m_MdlHfFdT;
        
        if (iRctHeatMethod!=RHM_Override1)
          {
          m_UsedHOR0=m_MdlHOR0;
          m_UsedHORT=m_MdlHORT;
          }

        }
      
      // apply Thermal Losses after precipitation...
      double T1;
      double H1 = Qp.totHf(som_ALL, Qp.Temp(), P);
      switch (iThermalLossMethod)
        {
        case THL_None: 
          dThermalLoss = 0.0;
          dThermTempDrop = 0.0;
          break;
        case THL_TempDrop: 
          //H1 = Qp.totHf(som_ALL, Qp.Temp(), P);
          Qp.SetTemp(Qp.Temp()-TempDropRqd);
          dThermalLoss = H1 - Qp.totHf(som_ALL, Qp.Temp(), P);
          dThermTempDrop = TempDropRqd;
          break;
        case THL_FixedHeatFlow: 
          T1 = Qp.Temp();
          //H1 = Qp.totHf(som_ALL, Qp.Temp(), P);
          Qp.Set_totHf(H1-ThermalLossRqd);
          dThermalLoss = ThermalLossRqd;
          dThermTempDrop = T1-Qp.Temp();
          break;
        case THL_HeatFlowFrac: 
          T1 = Qp.Temp();
          dThermalLoss = -Hfin*ThermalLossFrac;
          //H1 = Qp.totHf(som_ALL, Qp.Temp(), P);
          Qp.Set_totHf(H1-dThermalLoss);
          dThermTempDrop = T1-Qp.Temp();
          break;
        }
      
      //results...
      dResidenceTime = TankVol/GTZ(Qp.QVolume(som_SL));      // Hours
      dTout          = Qp.Temp();
  		dSALout        = (pSSA ? pSSA->SpecificSurfaceAreaVol() : 0.0);
      dDiamout       = (pSSA ? pSSA->PartDiamFromSAM() : 0.0);
      dACout         = Bm.AtoC();
      double Cout    = Bm.CausticConc(Qp.Temp());
      dYield         = Cout*(dACin-dACout);
      dASat          = Bm.AluminaConcSat(Qp.Temp());
      dQvout         = Qp.QVolume();
			double SolOut  = Qp.QMass(som_Sol);
      double THAOut  = Qp.VMass[THA.SolPhInx()];
      double OcNaOut = Qp.VMass[OccSoda.SolPhInx()];
			dSolPrecip     = THAOut - THAIn;
      dOccSoda       = OcNaOut - OcNaIn;

      }
    else
      {
      IsOff = true;
      }
    }
  else
    {
    IsOff = true;
    }
  if (IsOff)
    {
    //results...
    dTin    = Qp.Temp();
    dTout   = dTin;
    dQvin   = Qp.QVolume();
    dQvout  = dQvin;
    dACin   = (pBm ? pBm->AtoC() : 0.0);
    dACout  = dACin;
    dGrowthRateFactor = 0.0;
    dGrowthRate    = 0.0;
    dThermalLoss   = 0.0;
    dThermTempDrop = 0.0;
    m_StdHfFd0      = 0.0;
    m_StdHfPr0      = 0.0;
    m_StdHOR0      = 0.0;
    m_StdHfFdT      = 0.0;
    m_StdHfPrT      = 0.0;
    m_StdHORT      = 0.0;
    m_MdlHfFd0      = 0.0;
    m_MdlHfPr0      = 0.0;
    m_MdlHOR0      = 0.0;
    m_MdlHfFdT      = 0.0;
    m_MdlHfPrT      = 0.0;
    m_MdlHORT      = 0.0;
    m_UsedHOR0     = 0.0;
    m_UsedHORT     = 0.0;
    //dReactionHeat  = 0.0;
    dResidenceTime = 0.0;
  	dSALout        = 0.0;
    dDiamout       = 0.0;
    dYield         = 0.0;
    dASat          = 0.0;
		dSolPrecip     = 0.0;
    dOccSoda       = 0.0;
    }
  }

//--------------------------------------------------------------------------

void Precip::EvalProducts(CNodeEvalIndex & NEI)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      {
      int ioProd = IOWithId_Self(ioid_Product);
      ASSERT(ioProd>=0);
      SpConduit & Qp=*IOConduit(ioProd); //Qp product

      Qp.QZero();
      SigmaQInPMin(Qp, som_ALL, Id_2_Mask(ioid_Feed)); //set product = feed

      double T=Qp.Temp();

      DoPrecip(Qp);
      }
      break;
    default:
      MN_Surge::EvalProducts(NEI);
    }
  }

//--------------------------------------------------------------------------

void Precip::ClosureInfo() 
  { 
  MN_Surge::ClosureInfo();
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    if (NoFlwIOs()>0)
      {
      CI.m_EHXPowerIn+=-dThermalLoss;
      //if (iRctHeatMethod==RHM_Override1)
      //  CI.AddHeatFlowIn(-dReactionHeat);
      CI.m_RCTHOR_Std0+=m_StdHOR0;
      CI.m_RCTHOR_Mdl0+=m_MdlHOR0;
      CI.m_RCTHOR_Used0+=m_UsedHOR0;
      CI.m_RCTHOR_Diff0=CI.m_RCTHOR_Used0-CI.m_RCTHOR_Mdl0;
      }
    }
  }

//--------------------------------------------------------------------------

flag Precip::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tBad Feed Stream - Not Bayer Model"; return 1;
    case 2: pS="W\tA at outlet not converging"; return 1;
    case 3: pS="W\tA at outlet is zero"; return 1;
    case 4: pS="W\tCannot converge PrecipTHA Alumina Conc"; return 1;
    case 5: pS="W\tBad Feed Stream - No SSA data"; return 1;
    default:                                               
      return MN_Surge::CIStrng(No, pS);
    }
  }

//--------------------------------------------------------------------------
//==========================================================================
//
//
//
//==========================================================================
    
