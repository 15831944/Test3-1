//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-11-13 11:21:00 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================


#include "stdafx.h"
#include "Precipitator.h"

//=========================================================================== 



//   Define the species in the particular precipiation model...

static MInitialiseTest InitTest("Precip");
static MSpeciePtr  spAlumina       (InitTest, "NaAl[OH]4(l)", false);
static MSpeciePtr  spWater         (InitTest, "H2O(l)", false);
static MSpeciePtr  spWaterVapor    (InitTest, "H2O(g)", false);
static MSpeciePtr  spTHA           (InitTest, "Al[OH]3(s)", false);
static MSpeciePtr  spCausticSoda   (InitTest, "NaOH(l)", false);
static MSpeciePtr  spOccSoda       (InitTest, "Na2O(s)", false);
static MSpeciePtr  spCarbonate     (InitTest, "Na2CO3(l)", false);
static MSpeciePtr  spBoundSoda     (InitTest, "NaOH*(s)", false);
static MSpeciePtr  spOrganics      (InitTest, "Na2C5.2O7.2(l)", false);
static MSpeciePtr  spBoundOrganics (InitTest, "Na2C5.2O7.2*(s)", false);



enum PrecipMethod {GRM_Fixed, GRM_White, GRM_TTTest};
enum ThermalHeatLossMethods {THL_None, THL_TempDrop, THL_FixedHeatFlow, THL_Ambient};
enum CoolType{COOL_dQ, COOL_dT, COOL_Hx};
enum EvapType{EVAP_NONE, EVAP_FIXED, EVAP_dT};
enum ICool{COOL_NONE, COOL_INTERNAL, COOL_EXTERNAL};


enum PrecipComponents {iALUMINA, iBOUND_SODA, iBOUND_ORGANICS, iWATER, nPRECIPCOMPS};

const long idFeed  = 0;
const long idProd  = 1;
const long idCoolO  = 2;
const long idCoolI  = 3;
const long idEvap  = 4;

static MInOutDefStruct s_IODefs[]=
  {
    { "Feed",    "Feed",  idFeed,  1, 10,  0,  1.0f,  MIO_In |MIO_Material },
    { "Product", "Prod",  idProd,  1,  1,  0,  1.0f,  MIO_Out|MIO_Material },
    { "CoolOut", "CoolO", idCoolO, 0,  1,  1,  1.0f,  MIO_Out|MIO_Material },
    { "CoolIn",  "CoolI", idCoolI, 0,  5,  1,  1.0f,  MIO_In |MIO_Material },
    { "Evap",    "Evap",  idEvap,  0,  1,  0,  1.0f,  MIO_Out|MIO_Material },
    
    
    { NULL },
  };

double Drw_CPrecipitator[] = 
  { 
    MDrw_Poly, -5,10, -5,-10, 5,-10, 5,10,
    MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CPrecipitator, "Alumina2*Precipitator", DLL_GroupName)

void CPrecipitator_UnitDef::GetOptions()
{
  SetDefaultTag("PC");
  SetDrawing("Tank", Drw_CPrecipitator);
  SetTreeDescription("Process:Unit:Precipitator(2)");
  SetDescription("TODO: Precip Model");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  //SetModelLicense(MLicense_HeatExchange|MLicense_Alumina);
};

//---------------------------------------------------------------------------



static double LMTD(double TbTi, double TbTo, double ShTi, double ShTo)
{
  double gttd = ShTo - TbTi;
  double lttd = ShTi - TbTo;
  return (gttd==lttd) ? gttd : (gttd-lttd)/log(GTZ(fabs(gttd))/(GTZ(fabs(lttd))));
}



class CCoolerFn : public MRootFinder
  {
  public:
    CCoolerFn(double _UA, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO) : 
      MRootFinder("Basic Hx calc" ,s_Tol),
      UA(_UA), m_TubeI(TubeI), m_ShellI(ShellI), m_TubeO(TubeO), m_ShellO(ShellO) 
      {
      }
    double Function(double TubeTOut) 
      {           

      double q = m_TubeI.totHz(MP_All, TubeTOut)-m_TubeI.totHz(MP_All);
      m_TubeO.Set_totHz(m_TubeI.totHz()+q);
      m_ShellO.Set_totHz(m_ShellI.totHz()-q);
      double lmtd=LMTD(m_TubeI.T, TubeTOut, m_ShellI.T , m_ShellO.T);
      double qp = UA*lmtd;
      return q-qp;
      };

  public:
    MStream &m_TubeI;
    MStream &m_ShellI;
    MStream &m_TubeO;
    MStream &m_ShellO;
    //  double Duty;

    double UA;
    static MToleranceBlock s_Tol;
  };

MToleranceBlock CCoolerFn::s_Tol(TBF_Both, "Precip:Simple", 0.00005, 0, 100);





CPrecipitator::CPrecipitator(MUnitDefBase *pUnitDef, TaggedObject * pNd) :  
  MBaseMethod(pUnitDef, pNd),
  m_RB(this, false, "RB"), 
  Evap(this, "Evap")
{   //default values...
//   x=new double[4];
//   xo = new double[4];


  m_bEvapConnected = 0;

  m_dActivationEnergy =	7734.00;
  m_dK0	= 2.29e+13;
  m_dK1 = 1.75;
  m_dk_TOC = 0.0023000;
  m_dn_s = -1.00;
  m_dn_fc = -2.00;
  m_dn_eq = 0;
  m_dn_ = 1.70;
  m_dn_sol = 1.00;
  m_dn_ssa = 0.60;

  bOnLine = 1;
  dTempDropRqd = 0.5;
  dThermalLossRqd = 1500.0;
  iThermalLossMethod = THL_TempDrop;
  iCoolMethod=0;

  m_bEvapConnected = 0;
  
  ER_White      = 7200.0;
  K_White       = 1.96e10;
  gF_White      = 1.0;
  m_dK_Soda = .00127;    // K for soda inclusion
  m_dE_Soda = 2535.;    // E for soda inclusion


  dTankVol       = 1000.0;

  dSolPrecip  = 0.0;
  dGrowthRate = 0.0;
  dGrowthRateFactor = 0.0;
  dYield      = 0.0;
  dTin        = C2K(25.0);
  dTout       = C2K(25.0);
  dDiamin     = 0.0;
  dSALin      = 0.0;
  dQvin       = 0.0;
  dQvout      = 0.0;
  dACin       = 0.0;
  dACout      = 0.0;
  dResidenceTime = 0.0;
  dThermalLoss   = 0.0;
  dReactionHeat  = 0.0;
  m_dCoolOutTotHz = 0.0;
  iCoolType = 0;
  
  

}

//---------------------------------------------------------------------------

CPrecipitator::~CPrecipitator()
{
//   delete[] x;
//   delete[] xo;
}

//---------------------------------------------------------------------------

void CPrecipitator::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------

const long OM_Simple      = 0;
const long OM_Condensing  = 1;

void CPrecipitator::BuildDataFields()
{
  static MDDValueLst DDB1[]={
    {GRM_Fixed,   "Fixed" },
    {GRM_White,   "White"},
    {GRM_TTTest, "Test"},
    {0}};

  static MDDValueLst DDB3[]={
    {THL_None,          "None" },
    {THL_TempDrop,      "TempDrop"},
    {THL_FixedHeatFlow, "FixedLoss"},
    {THL_Ambient,  "Ambient"},
    {0}};
  static MDDValueLst DDB5[]={
    { COOL_dQ, "Fixed.dQ"},
    { COOL_dT, "Fixed.dT" },
    { COOL_Hx, "HeatExchange"},
    {0}};
  static MDDValueLst DDB6[]={
    { EVAP_NONE, "None"},
    { EVAP_FIXED, "Fixed"},
    { EVAP_dT, "Ambient" },
    {0}};

  static MDDValueLst DDB7[]={
    { COOL_NONE, "None"},
    { COOL_INTERNAL, "Internal"},
    { COOL_EXTERNAL, "External" },
    {0}};

  DD.Text  ("");

#ifdef TTDEBUG
  DD.CheckBox("TTDBG", "",  &bTTDebug, MF_PARAMETER);
#endif


  DD.CheckBox("On", "",  &bOnLine, MF_PARAMETER|MF_SET_ON_CHANGE);
  //  DD.CheckBox("Int.Cooling", "", &m_bInternalCool, MF_PARAMETER|MF_SET_ON_CHANGE);
  //DD.CheckBox("Ext.Cooling", "", &m_bExternalCool, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Long  ("Cooling", "", &iCoolType, MF_PARAMETER|MF_SET_ON_CHANGE, DDB7);

  m_RB.OnOffCheckBox();

  DD.Long  ("Evaporation", "", &iEvapMethod , MF_PARAMETER|MF_SET_ON_CHANGE, DDB6);

  DD.Show(iEvapMethod!=EVAP_NONE);
  DD.Double("Evap.Rate", "",  &m_dEvapRate ,iEvapMethod==EVAP_FIXED?MF_PARAMETER:MF_RESULT, MC_Qm("kg/s"));
  DD.Show(iEvapMethod==EVAP_dT);
  DD.Double("Evap.Per.degK", "",            &m_dEvapRateK         ,MF_PARAMETER, MC_Qm("kg/s"));

  DD.Show();
  DD.Long  ("ThermalLossMethod", "",&iThermalLossMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB3);
  DD.Show(iThermalLossMethod==THL_TempDrop);
  DD.Double("Temp_Drop", "",        &dTempDropRqd     ,MF_PARAMETER, MC_dT("C"));
  DD.Show(iThermalLossMethod==THL_FixedHeatFlow);
  DD.Double("ThermalLossRqd", "",   &dThermalLossRqd  ,MF_PARAMETER, MC_Pwr("kW"));
  DD.Show(iThermalLossMethod==THL_Ambient);
  DD.Double("ThermalLossAmbient", "",  &dThermalLossAmbient ,MF_PARAMETER, MC_UA);
  DD.Show();
  DD.Text("");
  DD.Text  ("Requirements");
  DD.Double("TankVol", "",	        &dTankVol         ,MF_PARAMETER, MC_Vol("m^3"));
  DD.Text  ("");
  DD.Text  ("Results Tank");
  DD.Double("ResidenceTime", "",    &dResidenceTime ,MF_RESULT, MC_Time("h"));
  DD.Double("SuperSat", "", &m_dSSat, MF_RESULT, MC_);
  DD.Double("Yield", "",            &dYield         ,MF_RESULT, MC_Conc("kg/m^3"));
  DD.Double("THA.Precip", "",        &dTHAPrecip     ,MF_RESULT, MC_Qm("kg/s"));
  DD.Double("Solids.Precip", "",    &dSolPrecip     ,MF_RESULT, MC_Qm("kg/s"));
  DD.Double("Solids.Conc", "",    &dSolConc     ,MF_RESULT,  MC_Conc("kg/m^3"));

    
  DD.Text  ("Results");         
  DD.Show();                    
  DD.Double("Vol_FlowIn", "",       &dQvin          ,MF_RESULT, MC_Qv("L/s"));
  DD.Double("Vol_FlowOut", "",      &dQvout         ,MF_RESULT, MC_Qv("L/s"));

  DD.Double("MassFlowIn", "",       &dQmin          ,MF_RESULT, MC_Qm("t/d"));
  DD.Double("MassFlowOut", "",      &dQmout         ,MF_RESULT, MC_Qm("t/d"));
  
  DD.Double("ACin", "",             &dACin          ,MF_RESULT, MC_);
  DD.Double("ACout", "",            &dACout         ,MF_RESULT, MC_);
  DD.Double("ACequil", "",          &m_dACeq        ,MF_RESULT, MC_);


  DD.Double("TempIn", "",           &dTin           ,MF_RESULT, MC_T("C"));
  DD.Double("TempOut", "",          &dTout          ,MF_RESULT, MC_T("C"));
  DD.Text("");
  
  DD.Page("Precip");
  DD.Long  ("GrowthMethod", "", (long*)&iGrowthRateMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB1);
  DD.Double("Convergence.Limit", "",   &m_dConvergenceLimit, MF_PARAMETER|MF_INIT_HIDDEN, MC_);
  DD.Double("Thermal.Damping", "",   &m_dDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  DD.Double("Mass.Damping", "",   &m_dMassDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  DD.Double("Vol.Damping", "",   &m_dVolDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  
  DD.Show (iGrowthRateMethod==GRM_Fixed);
  DD.Double("Precip.Rate", "",      &dGrowthRate    ,MF_PARAMETER, MC_Qm("kg/s"));
  DD.Show (iGrowthRateMethod==GRM_White);
  DD.Double("ER_White", "",         &ER_White        ,MF_PARAMETER, MC_T("K"));
  DD.Double("K_White", "",	        &K_White         ,MF_PARAMETER, MC_);
  DD.Double("gF_White", "",    	    &gF_White        ,MF_PARAMETER, MC_);
  DD.Double("K_SODA", "", &m_dK_Soda, MF_PARAMETER, MC_);
  DD.Double("E_SODA", "", &m_dE_Soda, MF_PARAMETER, MC_);
  


  DD.Show (iGrowthRateMethod==GRM_TTTest);
  DD.Double("ActivationEnergy", "",    	    &m_dActivationEnergy    ,MF_PARAMETER, MC_T("K"));
  DD.Double("K0", "",    	    &m_dK0   		    ,MF_PARAMETER, MC_);
  DD.Double("K1", "",    	    &m_dK1   		    ,MF_PARAMETER, MC_);
  DD.Double("k_TOC", "",    	    &m_dk_TOC		    ,MF_PARAMETER, MC_);
  DD.Double("n_s", "",    	    &m_dn_s  		    ,MF_PARAMETER, MC_);
  DD.Double("n_fc", "",    	    &m_dn_fc 		    ,MF_PARAMETER, MC_);
  DD.Double("n_eq", "",    	    &m_dn_eq 		    ,MF_PARAMETER, MC_);
  DD.Double("n_", "",    	    &m_dn_   		    ,MF_PARAMETER, MC_);
  DD.Double("n_sol", "",    	    &m_dn_sol		    ,MF_PARAMETER, MC_);
  DD.Double("n_ssa", "",    	    &m_dn_ssa		    ,MF_PARAMETER, MC_);
  DD.Double("SSAin", "",    	    &m_dSSAin		    ,MF_RESULT, MC_SurfAreaM);
  DD.Double("SSA", "",    	    &m_dSSA		    ,MF_PARAMETER, MC_SurfAreaM);

  DD.Show (iGrowthRateMethod!=0);

  DD.Double("BoundSoda_OrgPart","", &m_dBndOrgSoda          ,MF_PARAMETER, MC_Frac("%"));

  

  DD.Show();
  DD.Text("Thermal and Mass Balance");
  DD.Double("Mass.Flow.In", "",       &dQmin          ,MF_RESULT|MF_NO_FILING, MC_Qm("t/d"));
  DD.Double("Mass.Flow.Out", "",      &dQmout         ,MF_RESULT|MF_NO_FILING, MC_Qm("t/d"));
  DD.Show(iEvapMethod!=0);
  DD.Double("Evap.Mass.Loss", "", &m_dEvapRate, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("Evap.Thermal.Loss", "",   &m_dEvapThermalLoss  ,MF_RESULT, MC_Pwr("kW"));
  DD.Show(iThermalLossMethod==THL_Ambient);
  DD.Double("Env.Thermal.Loss", "", &m_dEnvironmentLoss, MF_RESULT, MC_Pwr("kW"));  
  DD.Show(iCoolType!=0);
  DD.Double("Cooler.Thermal.Loss", "", &m_dCoolRate, MF_RESULT, MC_Pwr("kW")); // Ext Cooling Rate
  DD.Double("ReactionHeat", "",     &dReactionHeat  ,MF_RESULT, MC_Pwr("kW"));
  DD.Double("Total.Thermal.Loss", "", &m_dTotThermalLoss, MF_RESULT, MC_Pwr("kW"));
  DD.Text("Stream.Enthalpy");
  DD.Double("HzIn", "", &m_dHIn, MF_RESULT, MC_Pwr);
  DD.Double("HzEvap", "", &m_dHEvap, MF_RESULT, MC_Pwr);
  DD.Double("HzOut", "", &m_dHOut, MF_RESULT, MC_Pwr);
  DD.Double("HzBal", "", &m_dHBal, MF_RESULT, MC_Pwr);

  DD.Show(iCoolType!=0);
  DD.Page("Cooler");
  DD.Show(iCoolType==COOL_INTERNAL);
  DD.CheckBox("Cooler.On", "",  &m_bCoolerOn, MF_PARAMETER);
  DD.Long  ("Cooling.Type", "", (long*)&iCoolMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB5);

  DD.Show(iCoolType==COOL_INTERNAL && (iCoolMethod==COOL_dT));
  DD.Double("dT", "", &m_dCooldT, MF_PARAMETER, MC_dT("C")); 

  DD.Show(iCoolType==COOL_INTERNAL && (iCoolMethod==COOL_dQ));
  DD.Double("dQ",  "", &m_dCooldQ, MF_PARAMETER, MC_Pwr("kW")); 

  DD.Show(iCoolType==COOL_INTERNAL && (iCoolMethod==COOL_Hx));
  DD.Double("HX.Area",  "", &m_dCoolArea, MF_PARAMETER, MC_Area("m^2")); 
  DD.Double("HX.HTC",  "", &m_dCoolHTC, MF_PARAMETER, MC_HTC);
  DD.Show(iCoolType==COOL_EXTERNAL || (iCoolType==COOL_INTERNAL && (iCoolMethod==COOL_Hx)));
  DD.CheckBox("By.Vol.Flow", "",  &m_bByVolFlow, MF_PARAMETER|MF_SET_ON_CHANGE);

  DD.Show(iCoolType==COOL_INTERNAL && (iCoolMethod==COOL_Hx));
  DD.Double("Cooling.Flow",  "", &m_dCoolFlow,
	    m_bByVolFlow ? MF_RESULT : MF_PARAMETER, MC_Qm("kg/s")); // Internal cooling flow
  DD.Double("Int.Vol.Flow", "", &m_dIntCoolVolFlow, 
	    m_bByVolFlow ? MF_PARAMETER : MF_RESULT, MC_Qv("m^3/s")); // By Volume

  DD.Double("Hx.UA", "", &m_dUA, MF_RESULT, MC_UA);
  DD.Double("Hx.LMTD", "", &m_dLMTD, MF_RESULT, MC_dT);

  DD.Double("Water.Flow",  "", &m_dCoolWaterFlow, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("Water.Vol.Flow",  "", &m_dCoolWaterFlowVol, MF_RESULT, MC_Qv);

  DD.Double("Water.Tin",  "", &m_dCoolWaterTin, MF_RESULT, MC_T("C"));
  DD.Double("Water.Tout",  "", &m_dCoolWaterTout, MF_RESULT, MC_T("C"));
  DD.Double("Liquor.Tin",  "", &m_dLiquorTin, MF_RESULT, MC_T("C"));
  DD.Double("Liquor.Tout",  "", &m_dLiquorTout, MF_RESULT, MC_T("C"));
  DD.Show(iCoolType==COOL_EXTERNAL);
  DD.Double("Ext.Vol.Flow", "", &m_dExtCoolVolFlow, 
	    m_bByVolFlow ? MF_PARAMETER : MF_RESULT, MC_Qv("m^3/s")); // Ext Cooling.Flow
  DD.Double("Ext.Cooling.Flow", "", &m_dExtCoolFlow, 
	    m_bByVolFlow ? MF_RESULT : MF_PARAMETER, MC_Qm("kg/s")); // Ext Cooling.Flow

  DD.Double("Ext.Cooling.Temp", "", &m_dExtCoolTemp, MF_RESULT, MC_T("C")); // Ext Cooling.Temp
  DD.Double("Ext.Cooling.totHz", "", &m_dCoolOutTotHz , MF_RESULT, MC_Pwr("kW")); // Ext Cooling Rate
  DD.Show(iCoolType!=0);
  DD.Double("Cooling.Rate", "", &m_dCoolRate, MF_RESULT, MC_Pwr("kW")); // Ext Cooling Rate


  if (!m_bEvapConnected && iEvapMethod!=EVAP_NONE) //if optional Evap is NOT connected and evaporation functionality is required show the output stream
    DD.Object(Evap, MDD_RqdPage);

  DD.Show(m_RB.Enabled());  
  DD.Page("RB");
  m_RB.BuildDataFields();
  DD.Show();
}

//---------------------------------------------------------------------------

bool CPrecipitator::ValidateDataFields()
  {//ensure parameters are within expected ranges
  return true;
  }


//---------------------------------------------------------------------------
// Modified for NaAl[OH]4 as primary species.
void CPrecipitator::AdjustMasses(MStream & Prod)
{
  double &InAluminaMass  = Feed.MassVector[spAlumina];     // NaAl[OH]4(l)
  double &InWaterMass    = Feed.MassVector[spWater];       // H2O
  double &InTHAMass      = Feed.MassVector[spTHA];         // NaAl[OH]3(s)
  double &InCausticMass  = Feed.MassVector[spCausticSoda]; // NaOH
  double &InBoundSodaMass = Feed.MassVector[spBoundSoda];
  double &InOrganicMass = Feed.MassVector[spOrganics];
  double &InBoundOrganicMass = Feed.MassVector[spBoundOrganics];

  double &AluminaMass  = Prod.MassVector[spAlumina];     // NaAl[OH]4(l)
  double &WaterMass    = Prod.MassVector[spWater];       // H2O
  double &THAMass      = Prod.MassVector[spTHA];         // NaAl[OH]3(s)
  double &CausticMass  = Prod.MassVector[spCausticSoda]; // NaOH
  double &BoundSodaMass = Prod.MassVector[spBoundSoda];
  double &OrganicMass = Prod.MassVector[spOrganics];
  double &BoundOrganicMass = Prod.MassVector[spBoundOrganics];

  if (m_dMassDamping>0.0) {
    // Mass damping terms... may only be needed at very high rates
    for (int i=0; i<nPRECIPCOMPS; i++)
    x[i] = xo[i]*m_dMassDamping+x[i]*(1-m_dMassDamping);
  }
  
  ///
  // adjust masses...   x is mass of precipitated Gibbsite
  AluminaMass = InAluminaMass - Alpha*x[iALUMINA];
  THAMass     = InTHAMass     + x[iALUMINA];
  CausticMass = InCausticMass  + x[iALUMINA]*(Alpha-1) - x[iBOUND_SODA];
  OrganicMass = InOrganicMass - x[iBOUND_ORGANICS];
  BoundOrganicMass = InBoundOrganicMass + x[iBOUND_ORGANICS];
  BoundSodaMass = InBoundSodaMass + x[iBOUND_SODA];
  WaterMass = InWaterMass - x[3];
  //  if (m_bUpdateT) Prod.MarkStateChanged();    // Manual fixup of heat loads eventually...
}


void CPrecipitator::AdjustT(MStream &Prod) 
{
  double &EvapWater = Evap.MassVector[spWaterVapor];
  EvapWater = x[3];
  double dHeatIn = Feed.totHz();
  dReactionHeat = x[iALUMINA]*GibbsiteHOR(K2C(Prod.T));
  double newTotDH = m_dOldTotDH*m_dDamping+(m_dTotThermalLoss-dReactionHeat)*(1-m_dDamping);
  if (iThermalLossMethod==THL_TempDrop) {  // Override all other heat loss calcs
    Prod.T = Feed.T - dTempDropRqd;
  } else {
    Prod.Set_totHz(dHeatIn - newTotDH);
  }
  
  m_dOldTotDH = newTotDH;
}


void CPrecipitator::AdjustSSA(MStream &Prod) 
{
  MIBayer & ProdB=Prod.IF<MIBayer>(false);
  MISSA & ProdSSA = Prod.IF<MISSA>(false);    // We know that the stream conditions are satified

  if (iGrowthRateMethod == GRM_TTTest) {
    ProdSSA.SetSAM(m_dSSA, false);            /// SSA is entered manually
  } else {
    if (NoPerSec>0.0) {
      ProdSSA.SetSAMFromFlow(ProdB.THAMassFlow(), NoPerSec);   // Adjust SSA
    }
  }
}




//---------------------------------------------------------------------------


void CPrecipitator::DoPrecip(MStream & Prod)
{

  bool converged = false;
  int Iter=0;    // Number of iterations in main loop
  m_dOldTotDH = 0.0;
  while (!converged && Iter < ITERMAX) {    // Main iterative loop on final temperature and concentration
    EvalPrecipRates(Prod);    /// Determine the precipitation rates 
    EvalLosses(Prod);     /// Evaluate the thermal losses
    AdjustMasses(Prod);     /// Adjust the mass flows
    AdjustT(Prod);        /// Adjust temperature for thermal losses, reaction heats etc

    if (m_RB.Enabled()) {            // Optional Reaction Block
      m_RB.EvalProducts(Prod);
    }
    
    AdjustSSA(Prod);      /// Adjust SSA

    converged = ConvergenceTest();   //// Final convergence test  
    Iter++;
    
  }
  if (Iter==ITERMAX) {/// Convergence Failure, automatically increase damping and iterations???
  }
  
  
  Log.SetCondition(Iter==ITERMAX, 4, MMsg_Warning, "No convergence of precip routine: try increased damping");
  
}


//---------------------------------------------------------------------------
// All the precipitation rate calcs are done in this routine 
// Takes the product composition and calculates rates from this
// Rates (in kg/s) are put into global x
//---------------------------------------------------------------------------
void CPrecipitator::EvalPrecipRates(MStream & Prod, double T) 
{
  T = Prod.T;   // May want to do the rate calculation at other temperature
  
  MIBayer & ProdB=Prod.IF<MIBayer>(false);
  MISSA & ProdSSA = Prod.IF<MISSA>(false);
  dResidenceTime = dTankVol/GTZ(Prod.Volume(MP_SL));
  for (int i=0; i<nPRECIPCOMPS; i++)
    xo[i]=x[i];   // Save old values for convergence test, damping
  double SAL = ProdSSA.SpecificSurfaceAreaVol();
  double SSA = ProdSSA.SpecificSurfaceAreaMass();
  double dSSat;
  if (ProdB.CausticConc(T) > 0)
    dSSat = (ProdB.AluminaConc(T)-ProdB.AluminaConcSat(T))/ProdB.CausticConc(T);
  else
    dSSat = 0.0; 
  if (dSSat < 0) dSSat=0.0;
  
  // Supersaturation...

  switch (iGrowthRateMethod) {
    
  case GRM_Fixed:
    x[iALUMINA] = dGrowthRate;
    x[1] = x[iBOUND_ORGANICS] = 0.0;
    break;
  case GRM_White: 
    {
      MIBayer & FeedB = Feed.IF<MIBayer>(false);
      double Ain = FeedB.AluminaConc(T);
      double Aout = ProdB.AluminaConc(T);
      dGrowthRateFactor = gF_White*K_White*Exps(-ER_White/T);
      dGrowthRate = dGrowthRateFactor*Sqr(dSSat);
      x[iALUMINA] = dGrowthRate*SAL*dTankVol/1000.;
      double sodaRate = m_dK_Soda*Sqr(dSSat)*Exps(m_dE_Soda/T)*(Aout - Ain)*dTankVol/1000.;
      if (sodaRate <0.0) sodaRate = 0.0;
      x[iBOUND_SODA] = sodaRate*(1.0-m_dBndOrgSoda);
      x[iBOUND_ORGANICS] = sodaRate*m_dBndOrgSoda;
      
			   
    }
    
    break;
  case GRM_TTTest: 
    { // Alumina Precipitation.... as per QPRECIPD.cpp
      MIBayer & FeedB = Feed.IF<MIBayer>(false);
      double C  = ProdB.CausticConc(T);
      double C25 = ProdB.CausticConc(C2K(25));
      double CtoS = ProdB.CtoS();
      double S_out = C25/CtoS;
      //      double FC = ProdB.FreeCaustic(T);
      double FC = ProdB.FreeCaustic(C2K(25));
      double ACeq = ProdB.AluminaConcSat(T)/C25;   
      double TOOC=ProdB.TOC(C2K(25))*MW_Na2CO3/MW_C;
      double a=Exps(-m_dActivationEnergy/T);
      double b=Exps(-TOOC*m_dk_TOC);
      double c=Pow(GTZ(S_out), m_dn_s);
      double d=Pow(GTZ(FC), m_dn_fc);
      double e=Pow(GTZ(ACeq), m_dn_eq);

      double K = m_dK0*a*b*c*d*e;
      double ACout = ProdB.AtoC();
      double VLiq = Prod.Volume()*3600.;
      double MSolids     = Prod.MassVector[spTHA]*3600.0/1000.0;
      double Sol = MSolids*1000.0/GTZ(VLiq);
      double deltaAC = K * dResidenceTime/3600 *
	Pow(GTZ(ACout-ACeq),m_dn_) * Pow(GTZ(Sol),m_dn_sol) * Pow(GTZ(m_dSSA),m_dn_ssa);
      double ACoutEst = dACin - deltaAC;
      double VolOut = Prod.Volume(MP_Liq, C2K(25.0));
      double xx = deltaAC*C25*VolOut*2*MW_Alumina/MW_Al2O3;
      if (xx<0.0) xx=0.0;
      x[iALUMINA] = xx;
      
      // Bound Soda calculations... slurped from QAL file
      double k1x = m_dK1 * (0.000598*C25 - 0.00036*K2C(T) + 0.019568*TOOC/C) * (1.0 - 0.758*CtoS);
      double BoundSoda = k1x * Sqr(ACoutEst-ACeq);
      
      if (x[iALUMINA]>=0.0) {
	double BndSoda    = BoundSoda*(x[iALUMINA]*MW_Al2O3/(2.0*MW_THA))*(1.0-m_dBndOrgSoda)*((2.0*MW_NaOH)/MW_Na2O);
	double BndOrgSoda = BoundSoda*(x[iALUMINA]*MW_Al2O3/(2.0*MW_THA))*(m_dBndOrgSoda)*(MW_OrganicSoda/MW_Na2O);
	/*  If represented as Na2O
	    double BndSoda    = BoundSoda*GibbsRate*dBndOrgSoda;
	    double BndOrgSoda = BoundSoda*GibbsRate*(1.0-dBndOrgSoda);
	*/	
	x[iBOUND_SODA] = BndSoda;
	x[iBOUND_ORGANICS] = BndOrgSoda;

      }
      

    }
    
    break;
  }

  return;
}

//---------------------------------------------------------------------------
// All the thermal loss calcs are done in this routine 
// Also calculate the water loss through evaporation
// m_dTotThermalLoss is the total thermal loss, 
//---------------------------------------------------------------------------

void CPrecipitator::EvalLosses(MStream & Prod)
{
  double T = Prod.T;
  double TA = AmbientTemp();
  /// Heat Loss to internal cooling, eg Barriquands
  if (!m_bCoolerOn && iCoolMethod==COOL_Hx) {
    m_dLiquorTout = Prod.T;
    m_dLMTD = 0.0;
    m_dCoolWaterTout = CoolIn.T;
  }
  m_dIntCoolRate=0.0;
  if (m_bCoolerOn && iCoolType==COOL_INTERNAL && bCoolIn && iCoolMethod == COOL_Hx ) {
    MStream TubeIn;
    MStream TubeOut;
    MStream CoolOut;
    if (m_bByVolFlow) 
      m_dCoolFlow = m_dIntCoolVolFlow*Prod.Density();
    else
      m_dIntCoolVolFlow = m_dCoolFlow/Prod.Density();

    TubeIn.SetM(Prod, MP_All, m_dCoolFlow);
    TubeOut.SetF(TubeIn, MP_All, 1.0); 
    CoolOut.SetF(CoolIn, MP_All, 1.0);

    if (TubeIn.MassFlow()>0 && CoolIn.MassFlow()>0) {

	m_dUA=m_dCoolArea*m_dCoolHTC;
	CCoolerFn Fn(m_dUA, TubeIn, CoolIn, TubeOut, CoolOut);

	double TubeOutT;
	double MxTbOutT=TubeIn.T;// No Transfer
	double MnTbOutT=CoolIn.T+0.001;
 	double qShell=-CoolIn.totHz()+CoolOut.totHz(MP_All, TubeIn.T);
 	double qTube= -TubeIn.totHz(MP_All, CoolIn.T)+TubeIn.totHz();
      
	if (qShell<qTube) // Limited By Shell - Tube TOut Limited
	  MnTbOutT=MxTbOutT - (qShell)/GTZ(qTube)*(MxTbOutT-MnTbOutT);

      switch (Fn.FindRoot(0, MnTbOutT, MxTbOutT)) {
      case RF_OK:       TubeOutT = Fn.Result();   break;
      case RF_LoLimit:  TubeOutT = MnTbOutT;      break;
      case RF_HiLimit:  TubeOutT = MxTbOutT;      break;
      default: 
	Log.Message(MMsg_Error, "TubeOutT not found - RootFinder:%s", Fn.ResultString(Fn.Error())); 
	TubeOutT=Fn.Result();
	break;
      }
      TubeOut.T = TubeOutT;
      m_dIntCoolRate = TubeIn.totHz() - TubeOut.totHz();
      m_dCoolWaterTout = CoolOut.T;
      m_dCoolWaterTin = CoolIn.T;
      m_dCoolWaterFlow = CoolIn.Mass();
      m_dCoolWaterFlowVol = CoolIn.Volume();
      m_dLiquorTout = TubeOut.T;
      m_dLMTD=fabs(LMTD(TubeIn.T, TubeOut.T, CoolIn.T, CoolOut.T));
    }



  }
    
  if (iCoolType==COOL_INTERNAL && iCoolMethod == COOL_dQ ) {
    m_dIntCoolRate = m_dCooldQ;
  }
  






  switch (iCoolType) {
  case COOL_NONE:
    m_dCoolRate = 0;
    break;
  case COOL_EXTERNAL:
    m_dCoolRate = m_dExtCoolRate;    // kW
    break;
  case COOL_INTERNAL:
  /// Heat Loss to internal cooling, eg draft tube coolers
    m_dCoolRate = m_dIntCoolRate;
    break;
  }
  

  /// Evaporation Rate
  switch (iEvapMethod) {
  case EVAP_dT:
    m_dEvapRate = m_dEvapRateK*(T-TA);       // kg/s
    x[3] = m_dEvapRate;
    break;
  case EVAP_FIXED:
    x[3] = m_dEvapRate;
    break;
  case EVAP_NONE:
    x[3] = 0;
    m_dEvapRate = 0.0;
    
  }  

  /// Evaporative Heat Loss ... need to fix this up using stream enthalpies
  m_dEvapThermalLoss = 2300*m_dEvapRate;   // kW...


  /// Heat Loss to ambient cooling
  m_dEnvironmentLoss = 0.0;
  switch (iThermalLossMethod) {
  case THL_Ambient:
    m_dEnvironmentLoss = (T-TA)*dThermalLossAmbient;  //kW
    break;
  case THL_FixedHeatFlow:
    m_dEnvironmentLoss = dThermalLossRqd;
    break;
  }
  
  m_dTotThermalLoss = m_dEnvironmentLoss + m_dEvapThermalLoss + m_dCoolRate;
  
}




//------------------------------------------------------------------------
//  Check for convergence of the iteration
//
//
//------------------------------------------------------------------------
bool CPrecipitator::ConvergenceTest() 
{
  double xmag = (x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);
  double err = (Sqr(x[0]-xo[0]) + Sqr(x[1]-xo[1]) +Sqr(x[2]-xo[2]))/GTZ(xmag);

  if (err < m_dConvergenceLimit) 
    return true;
  else
    return false;
}





void CPrecipitator::DoResults(MStream & Prod)
{
  MIBayer & FeedB=Feed.IF<MIBayer>(false);
  MIBayer & ProdB=Prod.IF<MIBayer>(false);
  

  dACin   = FeedB.AtoC();
  dACout = ProdB.AtoC();
  dQvin   = Feed.Volume();
  dQvout   = Prod.Volume();
  dQmin = Feed.Mass();
  dQmout = Prod.Mass();

  double dQvout25 = Prod.Volume(MP_All, C2K(25));
  double Cout    = ProdB.CausticConc(Prod.T);
  m_dACeq = ProdB.AluminaConcSat(Prod.T)/ProdB.CausticConc(C2K(25));
  m_dSSat = ProdB.AtoC()/m_dACeq;
  dYield         = Cout*(dACin-dACout);

  double SolIn = Feed.MassVector[spTHA];
  double SolOut  = Prod.MassVector[spTHA];
  dTHAPrecip     = SolOut - SolIn;
  SolIn = Feed.Mass(MP_Sol);
  SolOut = Prod.Mass(MP_Sol);
  dSolPrecip = SolOut - SolIn;
  dSolConc = Prod.Mass(MP_Sol)/dQvout25;
  m_dLiquorTin = Prod.T;
}


void CPrecipitator::CoolingSanityCheck() 
{
  if (m_dExtCoolTemp<273 || m_dExtCoolTemp > Feed.T+20) {
    m_dExtCoolTemp = Feed.T;
    m_dExtCoolRate = 0;
  }
  if (m_dExtCoolRate > 25000 || m_dExtCoolRate < -1000) {
    m_dExtCoolTemp = Feed.T;
    m_dExtCoolRate = 0;
  }
}

    


void CPrecipitator::EvalProducts()
  {
  try {
    x[0] = x[1] = x[2] = x[3] = 0.0;
    m_dPrevFlow = m_dExtCoolVolFlow;

    FlwIOs.AddMixtureIn_Id(Feed, idFeed);
    MStream & Prod = FlwIOs[FlwIOs.First[idProd]].Stream; //Reference to the output stream
    //optional connections...
    const int iCoolInIndex = FlwIOs.First[idCoolI];
    const int iCoolOutIndex = FlwIOs.First[idCoolO];
    const int iEvapIndex = FlwIOs.First[idEvap];
    bCoolIn = (iCoolInIndex>=0);
    bCoolOut = (iCoolOutIndex>=0);
    m_bEvapConnected = (iEvapIndex>=0);



//     MStream CoolIn;
//     MStream &CoolOut = FlwIOs[FlwIOs.First[idCoolO]].Stream;
//     MStream &Evap1 = FlwIOs[FlwIOs.First[idEvap]].Stream;
    Evap.SetF(Feed, MP_All, 0.0);
    
    Prod = Feed;

    Log.SetCondition(iCoolType && (!bCoolIn || !bCoolOut), 3, MMsg_Error, "Bad External cooling connections");

    if (iCoolType==COOL_EXTERNAL && bCoolIn && bCoolOut) {
      FlwIOs.AddMixtureIn_Id(CoolIn, idCoolI);
      MStream &CoolOut = FlwIOs[iCoolOutIndex].Stream;
      if (Valid(CoolIn.T) && fabs(CoolIn.Mass())>1.0e-5 && fabs(CoolIn.Mass())>1.0e-5) {
	m_dExtCoolTemp = CoolIn.T;                  // The temperature of the return stream
	//	m_dExtCoolRate = CoolOut.totHz()-CoolIn.totHz();
	m_dExtCoolRate = m_dCoolOutTotHz-CoolIn.totHz();
	//      CoolingSanityCheck();
      } else {
	m_dExtCoolRate = 0.0;
	m_dExtCoolTemp = Feed.T;
      }
    }
	
    if (iCoolType==COOL_INTERNAL && bCoolIn) {
      FlwIOs.AddMixtureIn_Id(CoolIn, idCoolI);
    }



    
    m_dHIn = Feed.totHz();
    
    dThermalLoss = 0.0;
    dYield  = 0.0;
    dSALin  = 0.0;
    dDiamin = 0.0;
    bool streamOK = true;

    MIBayer & ProdB=Prod.IF<MIBayer>(false);   // Do the checks up front
    Log.SetCondition(IsNothing(ProdB), 1, MMsg_Warning, "Bad Feed Stream - Not Bayer Model");

    MISSA & ProdSSA = Prod.IF<MISSA>(false);
    Log.SetCondition(IsNothing(ProdSSA), 1, MMsg_Warning, "Bad Feed Stream - No SSA Available");
    if (IsNothing(ProdB) || IsNothing(ProdSSA)) streamOK = false;

    dSALin  = ProdSSA.SpecificSurfaceAreaVol(); // m^2/L
    dDiamin = ProdSSA.PartDiamFromSAM();
    m_dSSAin = ProdSSA.SpecificSurfaceAreaMass();
    


    if (bOnLine && streamOK) {  // Online and have Bayer and SSA properties...
      DoPrecip(Prod);

      if (iCoolType==COOL_EXTERNAL && bCoolOut) {
	MStream &CoolOut = FlwIOs[iCoolOutIndex].Stream;
	double den = Prod.Density();
	if (m_bByVolFlow) {
	  m_dExtCoolFlow = (1-m_dVolDamping)*m_dExtCoolVolFlow*den + m_dVolDamping*m_dPrevFlow;
	  m_dPrevFlow = m_dExtCoolFlow;
	} else 
	  m_dExtCoolVolFlow = m_dExtCoolFlow/den;
	CoolOut.SetM(Prod, MP_All, m_dExtCoolFlow);
	m_dCoolOutTotHz = CoolOut.totHz();
      }
      if (iCoolType==COOL_INTERNAL && bCoolOut && bCoolIn && iCoolMethod==COOL_Hx) {
	MStream &CoolOut = FlwIOs[iCoolOutIndex].Stream;
	CoolOut.SetF(CoolIn, MP_All, 1.0);
	if (m_bCoolerOn)
	  CoolOut.T = m_dCoolWaterTout;
	else
	  CoolOut.T = CoolIn.T;
      }
      




      DoResults(Prod);

      Evap.T = Prod.T;
      if (m_bEvapConnected) {
        MStream &Evap1 = FlwIOs[iEvapIndex].Stream;
        Evap1 = Evap;
      }

//       Evap1 = Evap;
//       Evap1.T = Prod.T;
      m_dHEvap = Evap.totHz();
      m_dHOut = Prod.totHz();
      m_dHBal = m_dHIn - m_dHEvap - m_dHOut;
      dTout = Prod.T;
      dTin = Feed.T;

    } else  {   // Just tidy up and put some sensible stuff in the results...
      dTin    = Feed.T;
      dTout   = dTin;
      dACin   = (IsNothing(ProdB) ? 0.0 : ProdB.AtoC());
      dACout  = dACin;
      dReactionHeat  = 0.0;
      dResidenceTime = 0.0;
      dYield = 0.0;
      dTHAPrecip = 0.0;
      dSolPrecip = 0.0;
      if (iCoolType==COOL_INTERNAL && bCoolOut && bCoolIn && iCoolMethod==COOL_Hx) {
	MStream &CoolOut = FlwIOs[iCoolOutIndex].Stream;
	CoolOut.SetF(CoolIn, MP_All, 1.0);
	CoolOut.T = CoolIn.T;
	m_dCoolWaterTout = CoolIn.T;
	m_dCoolRate = 0.0;
      }
      
    }
  }
  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (...)
    {
    Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
  }

//--------------------------------------------------------------------------




void CPrecipitator::ClosureInfo(MClosureInfo & CI)
  {//ensure heat balance
  if (CI.DoFlows())
    {
    CI.AddPowerIn(0, -dThermalLoss);
    }
  }

