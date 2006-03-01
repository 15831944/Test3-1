//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2005-02-18 09:44:33 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#include "stdafx.h"
#include "TTPrecip.h"

//=========================================================================== 
//#pragma optimize("", off)


#if TTDEBUG
#include "ttdebug.cpp"
#endif



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



enum PrecipMethod {GRM_Fixed, GRM_White, GRM_CAR_QAL, GRM_TTTest};
enum ThermalHeatLossMethods {THL_None, THL_TempDrop, THL_FixedHeatFlow, THL_Ambient};
enum CoolType{COOL_dQ, COOL_dT, COOL_Hx};
enum EvapType{EVAP_NONE, EVAP_FIXED, EVAP_dT};

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
    { "CoolIn",  "CoolI", idCoolI, 0,  1,  1,  1.0f,  MIO_In |MIO_Material },
    { "Evap",    "Evap",  idEvap,  0,  1,  0,  1.0f,  MIO_Out|MIO_Material },
    
    
    { NULL },
  };

double Drw_CPrecipitator[] = 
  { 
    MDrw_Poly, -5,10, -5,-10, 5,-10, 5,10,
    MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CPrecipitator, "Precipitator", DLL_GroupName)

void CPrecipitator_UnitDef::GetOptions()
{
  SetDefaultTag("PC", true);
  SetDrawing("Tank", Drw_CPrecipitator);
  SetTreeDescription("CAR:Precipitator");
  SetDescription("TODO: CAR Precip Model");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina);
};

//---------------------------------------------------------------------------

CPrecipitator::CPrecipitator(MUnitDefBase * pUnitDef, TaggedObject * pNd) : CPrecip(pUnitDef, pNd), m_RB(this, false, "RB")
{   //default values...
  x=new double[4];
  xo = new double[4];

  ER_White      = 7200.0;
  K_White       = 1.96e9;
  gF_White      = 1.0;


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
  

}

//---------------------------------------------------------------------------

CPrecipitator::~CPrecipitator()
{
  delete[] x;
  delete[] xo;
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
    {GRM_CAR_QAL, "CAR-QAL"},
    {GRM_TTTest,  "Test"},
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

  DD.Text  ("");
#if TTDEBUG
  DD.CheckBox("TTDBG", "",  &bTTDebug, MF_PARAMETER);
#endif



  DD.CheckBox("On", "",  &bOnLine, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Int.Cooling", "", &m_bInternalCool, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Ext.Cooling", "", &m_bExternalCool, MF_PARAMETER|MF_SET_ON_CHANGE);
  //  DD.CheckBox("ReactionB", "", &bRB, MF_PARAMETER|MF_SET_ON_CHANGE);
  m_RB.OnOffCheckBox();

  DD.Long  ("Evaporation", "", (long*)&iEvapMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB6);
  DD.Show(iEvapMethod!=0);
  DD.Double("Evap.Rate", "",  &m_dEvapRate ,iEvapMethod==EVAP_dT?MF_RESULT:MF_PARAMETER, MC_Qm("kg/s"));
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
  DD.CheckBox("Update.T", "",  &m_bUpdateT, MF_PARAMETER|MF_SET_ON_CHANGE);
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
  DD.Double("ACin", "",             &dACin          ,MF_RESULT, MC_);
  DD.Double("ACout", "",            &dACout         ,MF_RESULT, MC_);
  DD.Double("ACequil", "",          &m_dACeq        ,MF_RESULT, MC_);


  DD.Double("TempIn", "",           &dTin           ,MF_RESULT, MC_T("C"));
  DD.Double("TempOut", "",          &dTout          ,MF_RESULT, MC_T("C"));
  DD.Double("ReactionHeat", "",     &dReactionHeat  ,MF_RESULT, MC_Pwr("kW"));
  DD.Text("");

  DD.Page("Precip");
  DD.Long  ("GrowthMethod", "", (long*)&iGrowthRateMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB1);
  DD.Double("Thermal.Damping", "",   &m_dDamping, MF_PARAMETER, MC_);
  DD.Double("Mass.Damping", "",   &m_dMassDamping, MF_PARAMETER, MC_);
  DD.Show (iGrowthRateMethod==GRM_Fixed);
  DD.Double("Precip.Rate", "",      &dGrowthRate    ,MF_PARAMETER, MC_Qm("kg/s"));
  DD.Show (iGrowthRateMethod==GRM_White);
  DD.Double("ER_White", "",         &ER_White        ,MF_PARAMETER, MC_T("K"));
  DD.Double("K_White", "",	        &K_White         ,MF_PARAMETER, MC_);
  DD.Double("gF_White", "",    	    &gF_White        ,MF_PARAMETER, MC_);


  DD.Show (iGrowthRateMethod==GRM_CAR_QAL);
  DD.Double("ActivationEnergy", "",    	    &m_dActivationEnergy    ,MF_PARAMETER, MC_T("K"));
  DD.Double("K0", "",    	    &m_dK0   		    ,MF_PARAMETER, MC_);
  DD.Double("K1", "",    	    &m_dK1   		    ,MF_PARAMETER, MC_);
  DD.Double("k_TOC", "",    	    &m_dk_TOC		    ,MF_PARAMETER, MC_);
  DD.Double("n_s", "",    	    &m_dn_s  		    ,MF_PARAMETER, MC_);
  DD.Double("n_fc ", "",    	    &m_dn_fc 		    ,MF_PARAMETER, MC_);
  DD.Double("n_eq ", "",    	    &m_dn_eq 		    ,MF_PARAMETER, MC_);
  DD.Double("n_", "",    	    &m_dn_   		    ,MF_PARAMETER, MC_);
  DD.Double("n_sol", "",    	    &m_dn_sol		    ,MF_PARAMETER, MC_);
  DD.Double("n_ssa", "",    	    &m_dn_ssa		    ,MF_PARAMETER, MC_);
  DD.Double("SSAin", "",    	    &m_dSSAin		    ,MF_RESULT, MC_SurfAreaM);
  DD.Double("SSA", "",    	    &m_dSSA		    ,MF_PARAMETER, MC_SurfAreaM);
  DD.Double("BoundSoda_OrgPart","", &m_dBndOrgSoda          ,MF_PARAMETER, MC_Frac("%"));
      //DD.Show(bRB);

  
  DD.Show (iGrowthRateMethod==GRM_TTTest);
  DD.Double("Rate.Factor", "", &dGrowthRateFactor,   MF_PARAMETER, MC_Qm("kg/s"));

  DD.Show();

  DD.Show(iEvapMethod!=0);
  DD.Double("Evap.Thermal.Loss", "",   &m_dEvapThermalLoss  ,MF_RESULT, MC_Pwr("kW"));
  DD.Double("Evap.Mass.Loss", "", &m_dEvapMassLoss, MF_RESULT, MC_Qm("kg/s"));

  DD.Show();
  DD.Show(iThermalLossMethod==THL_Ambient);
  DD.Double("Env.Thermal.Loss", "", &m_dEnvironmentLoss, MF_RESULT, MC_Pwr("kW"));  
  DD.Show();
  DD.Double("Total.Thermal.Loss", "", &m_dTotThermalLoss, MF_RESULT, MC_Pwr("kW"));
  DD.Text("Stream.Enthalpy");
  DD.Double("HfIn", "", &m_dHIn, MF_RESULT, MC_Pwr);
  DD.Double("HfEvap", "", &m_dHEvap, MF_RESULT, MC_Pwr);
  DD.Double("HfOut", "", &m_dHOut, MF_RESULT, MC_Pwr);
  DD.Double("HfBal", "", &m_dHBal, MF_RESULT, MC_Pwr);
  DD.Text("");
  //m_RB.BuildDataFields();
  DD.Show();

  DD.Show(m_bInternalCool || m_bExternalCool);
  DD.Page("Cooler");
  DD.Show(m_bInternalCool);
  DD.Long  ("Cooling.Type", "", (long*)&iCoolMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB5);
  DD.Show(m_bInternalCool && iCoolMethod);
  DD.Double("Cooling.Flow",  "", &m_dCoolFlow, MF_PARAMETER, MC_Qm("kg/s"));
  
  
  

  DD.Show(m_bInternalCool && (iCoolMethod==COOL_dT));
  DD.Double("dT", "", &m_dCooldT, MF_PARAMETER, MC_dT("C")); 

  DD.Show(m_bInternalCool && (iCoolMethod==COOL_dQ));
  DD.Double("dQ",  "", &m_dCooldQ, MF_PARAMETER, MC_Pwr("kW")); 

  DD.Show(m_bInternalCool && (iCoolMethod==COOL_Hx));
  DD.Double("Cooling.Flow",  "", &m_dCoolFlow, MF_PARAMETER, MC_Qm("kg/s"));
  DD.Double("HX.Area",  "", &m_dCoolArea, MF_PARAMETER, MC_Area("m^2")); 
  DD.Double("HX.HTC",  "", &m_dCoolHTC, MF_PARAMETER, MC_HTC);
  DD.Double("Water.Flow",  "", &m_dCoolWaterFlow, MF_PARAMETER, MC_Qm("kg/s"));
  DD.Double("Water.Tin",  "", &m_dCoolWaterTin, MF_PARAMETER, MC_T("C"));
  DD.Show(m_bExternalCool);
  DD.Double("Ext.Vol.Flow", "", &m_dExtCoolVolFlow, MF_PARAMETER, MC_Qv("m^3/s")); // Ext Cooling.Flow
  DD.Double("Ext.Cooling.Flow", "", &m_dExtCoolFlow, MF_RESULT, MC_Qm("kg/s")); // Ext Cooling.Flow
  DD.Double("Ext.Cooling.Temp", "", &m_dExtCoolTemp, MF_RESULT, MC_T("C")); // Ext Cooling.Temp
  DD.Double("Ext.Cooling.Rate", "", &m_dExtCoolRate, MF_RESULT, MC_Pwr("kW")); // Ext Cooling Rate

  DD.Show();
  if (m_RB.Enabled())
    {
    DD.Page("RB", false); //when reaction is on, always force this onto a new "RB" tab page
    }
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

#ifdef MASS_DAMPING
  // Mass damping terms... may only be needed at very high rates
  x[0] = xo[0]*m_dMassDamping+x[0]*(1-m_dMassDamping);
  x[1] = xo[1]*m_dMassDamping+x[1]*(1-m_dMassDamping);
  x[2] = xo[2]*m_dMassDamping+x[2]*(1-m_dMassDamping);
#endif
  ///
  // adjust masses...   x is mass of precipitated Gibbsite
  AluminaMass = InAluminaMass - Alpha*x[0];
  THAMass     = InTHAMass     + x[0];
  CausticMass = InCausticMass  + x[0]*(Alpha-1) - x[1];
  OrganicMass = InOrganicMass - x[2];
  BoundOrganicMass = InBoundOrganicMass + x[2];
  BoundSodaMass = InBoundSodaMass + x[1];
  WaterMass = InWaterMass - x[3];
  if (m_bUpdateT) Prod.MarkStateChanged();    // Manual fixup of heat loads eventually...
}


void CPrecipitator::AdjustT(MStream &Prod) 
{
  double &EvapWater = Evap.MassVector[spWaterVapor];
  EvapWater = x[3];
  double dHeatIn = Feed.totHf();
  dReactionHeat = x[0]*GibbsiteHOR(Prod.T - 273.15);
  double newTotDH = m_dOldTotDH*m_dDamping+(m_dTotThermalLoss-dReactionHeat)*(1-m_dDamping);
  if (iThermalLossMethod==THL_TempDrop) {  // Override all other heat loss calcs
    Prod.T = Feed.T - dTempDropRqd;
  } else {
    Prod.Set_totHf(dHeatIn - newTotDH);
  }
  
  m_dOldTotDH = newTotDH;
}


void CPrecipitator::AdjustSSA(MStream &Prod) 
{
  MIBayer & ProdB=Prod.IF<MIBayer>(false);
  MISSA & ProdSSA = Prod.IF<MISSA>(false);    // We know that the stream conditions are satified
//   if (NoPerSec>0.0) {
//     ProdSSA.SetSAMFromFlow(ProdB.THAMassFlow(), NoPerSec);   // Adjust SSA
//   }
  ProdSSA.SetSAM(m_dSSA, false);
  

}




//---------------------------------------------------------------------------


void CPrecipitator::DoPrecip(MStream & Prod)
{

  bool converged = false;
  int Iter=0;    // Number of iterations in main loop
  m_dOldTotDH = 0.0;
  while (!converged && Iter < ITERMAX) {    // Main iterative loop on final temperature and concentration
    PrecipRates(Prod);    /// Determine the precipitation rates 
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
void CPrecipitator::PrecipRates(MStream & Prod, double T) 
{
  T = Prod.T;   // May want to do the rate calculation at other temperature
  
  MIBayer & ProdB=Prod.IF<MIBayer>(false);
  MISSA & ProdSSA = Prod.IF<MISSA>(false);
  dResidenceTime = dTankVol/GTZ(Prod.Volume(MP_SL));
  xo[0]=x[0];xo[1]=x[1];xo[2]=x[2];xo[3]=x[3];   // Save old values for convergence test, damping
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
    x[0] = dGrowthRate;
    x[1] = x[2] = 0.0;
    break;
  case GRM_White:  
    dGrowthRateFactor = gF_White*K_White*Exps(-ER_White/T);
    dGrowthRate = dGrowthRateFactor*Sqr(dSSat);
    x[0] = dGrowthRate*dSALin*dTankVol/1000.;
    x[1] = x[2] = 0.0;
    break;
  case GRM_CAR_QAL: 
    { // Alumina Precipitation.... as per QPRECIPD.cpp
      MIBayer & FeedB = Feed.IF<MIBayer>(false);
      double AMassIn = Feed.MassVector[spAlumina]*MW_Al2O3/(2*MW_Alumina);  //As Al203
      double C  = ProdB.CausticConc(T);
      double CtoS = ProdB.CtoS();
      double S_out = C/CtoS;
      double FC = ProdB.FreeCaustic(T);
      double ACeq = ProdB.AluminaConcSat(T)/C;
      double a=Exps(-m_dActivationEnergy/T);
      //double b=Exps(-ProdB.TOOC(T)*m_dk_TOC);
      double b=Exps(-ProdB.TOC(T)*MW_Na2CO3/MW_C*m_dk_TOC);
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
      double C25 = ProdB.CausticConc(C2K(25));
      double xx = (AMassIn - ACoutEst*C25*VolOut)*2*MW_Alumina/MW_Al2O3;
      x[0] = xx;
      
      // Bound Soda calculations... slurped from QAL file
      double k1x = m_dK1 * (0.000598*C25 - 0.00036*K2C(T) + 0.019568*ProdB.TOC(T)*MW_Na2CO3/MW_C/C) * (1.0 - 0.758*CtoS);
      double BoundSoda = k1x * Sqr(ACoutEst-ACeq);
      
      if (x[0]>=0.0) {
	double BndSoda    = BoundSoda*(x[0]*MW_Al2O3/(2.0*MW_THA))*(1.0-m_dBndOrgSoda)*((2.0*MW_NaOH)/MW_Na2O);
	double BndOrgSoda = BoundSoda*(x[0]*MW_Al2O3/(2.0*MW_THA))*(m_dBndOrgSoda)*(MW_OrganicSoda/MW_Na2O);
	/*  If represented as Na2O
	    double BndSoda    = BoundSoda*GibbsRate*dBndOrgSoda;
	    double BndOrgSoda = BoundSoda*GibbsRate*(1.0-dBndOrgSoda);
	*/	
	x[1] = BndSoda;
	x[2] = BndOrgSoda;

      }
      

    }
    
    break;
  case GRM_TTTest:
    dGrowthRate = dGrowthRateFactor*Sqr(dSSat);
    x[0] =  dGrowthRate*dSALin*dTankVol;
    x[1]=0.0;
    x[2]=0.0;
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
  /// Heat Loss to external cooling, eg Barriquands
  
  double extCoolRate = (m_bExternalCool ? m_dExtCoolRate : 0.0);    // kW
  
  /// Heat Loss to internal cooling, eg draft tube coolers
  double intCoolRate = 0.0;
  

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
  
  m_dTotThermalLoss = m_dEnvironmentLoss + m_dEvapThermalLoss + extCoolRate + intCoolRate;
  
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

// #ifdef TTDEBUG
//   p("Error  = %12.4g ", err);
// #endif
  if (err < 1.0e-10) 
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
  double dQvout25 = Prod.Volume(MP_All, C2K(25));
  double Cout    = ProdB.CausticConc(Prod.T);
  m_dACeq = ProdB.AluminaConcSat(Prod.T)/Cout;
  m_dSSat = ProdB.AtoC()/m_dACeq;
  dYield         = Cout*(dACin-dACout);

  double SolIn = Feed.MassVector[spTHA];
  double SolOut  = Prod.MassVector[spTHA];
  dTHAPrecip     = SolOut - SolIn;
  SolIn = Feed.Mass(MP_Sol);
  SolOut = Prod.Mass(MP_Sol);
  dSolPrecip = SolOut - SolIn;
  dSolConc = Prod.Mass(MP_Sol)/dQvout25;
}


void CPrecipitator::EvalProducts()
  {
  try {
    x[0] = x[1] = x[2] = x[3] = 0.0;

    FlwIOs.AddMixtureIn_Id(Feed, idFeed);
    MStream & Prod = FlwIOs[FlwIOs.First[idProd]].Stream; //Reference to the output stream
    MStream CoolIn;
    MStream &CoolOut = FlwIOs[FlwIOs.First[idCoolO]].Stream;
    MStream &Evap1 = FlwIOs[FlwIOs.First[idEvap]].Stream;
    Evap.SetF(Feed, MP_All, 0.0);
    
    Prod = Feed;
    if (m_bExternalCool) {
      FlwIOs.AddMixtureIn_Id(CoolIn, idCoolI);
      m_dExtCoolTemp = CoolIn.T;                  // The temperature of the return stream
      if (1) {
	m_dExtCoolRate = CoolOut.totHf()-CoolIn.totHf();
      }
      
    }
    
    m_dHIn = Feed.totHf();
    
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

      if (m_bExternalCool) {
	double den = Prod.Density();
	m_dExtCoolFlow = m_dExtCoolVolFlow*den;
	CoolOut.SetM(Prod, MP_All, m_dExtCoolFlow);
      }
      DoResults(Prod);

      Evap1 = Evap;
      Evap1.T = Prod.T;
      m_dHEvap = Evap1.totHf();
      m_dHOut = Prod.totHf();
      m_dHBal = m_dHIn - m_dHEvap - m_dHOut;
      dTout = Prod.T;
      dTin = Feed.T;
#if TTDEBUG
      p("bExtCool = %d", m_bExternalCool);
#endif
    } else  {   // Just tidy up and put some sensible stuff in the results...
      dTin    = Feed.T;
      dTout   = dTin;
      dACin   = (IsNothing(ProdB) ? 0.0 : ProdB.AtoC());
      dACout  = dACin;
      dReactionHeat  = 0.0;
      dResidenceTime = 0.0;
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
