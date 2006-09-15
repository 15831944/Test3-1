//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-09-13 12:36:54 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================

#include "stdafx.h"
#include "TTWORClassPrecip.h"

//=========================================================================== 




//   Define the species in the particular precipiation model...

static MInitialiseTest InitTest("Precip");


//static MSpeciePtr  spAlumina       (InitTest, "NaAl[OH]4(l)", false);
//static MSpeciePtr  spTHA           (InitTest, "Al[OH]3(s)", false);

static MSpeciePtr  spAlumina       (InitTest, "Al2O3(l)", false);
static MSpeciePtr  spTHA           (InitTest, "Al2O3.3H2O(s)", false);
static MSpeciePtr  spWater         (InitTest, "H2O(l)", false);
static MSpeciePtr  spWaterVapor    (InitTest, "H2O(g)", false);
static MSpeciePtr  spCausticSoda   (InitTest, "NaOH(l)", false);
static MSpeciePtr  spOccSoda       (InitTest, "Na2O(s)", false);
static MSpeciePtr  spCarbonate     (InitTest, "Na2CO3(l)", false);
static MSpeciePtr  spOrganics      (InitTest, "NaOrg(l)", false);
// static MSpeciePtr  spBoundOrganics (InitTest, "Na2C5.2O7.2*(s)", false);


enum PrecipComponents {iALUMINA, iOCC_SODA, iBOUND_ORGANICS, iWATER, nPRECIPCOMPS};

enum {GRM_AAEq, GRM_sigma};

enum {PB_None, PB_SSA, PB_PSD};  // Particle balance model



const long idFeed  = 0;
const long idUflow  = 1;
const long idOflow  = 2;

static MInOutDefStruct s_IODefs[]=
  {
    { "Feed",      "Feed",   idFeed,  1, 10,  0,  1.0f,   MIO_In |MIO_Material },
    { "Underflow", "UFlow",  idUflow,  1,  1,  0,  1.0f,  MIO_Out|MIO_Material },
    { "Overflow",  "OFlow",  idOflow, 0,  1,  1,  1.0f,   MIO_Out|MIO_Material },
    
    
    { NULL },
  };


double Drw_CClassPrecipitator[] = 
  { 
    MDrw_Poly, -5,10, -5,-10, 5,-10, 5,10,
    MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CClassPrecipitator, "ClassifyingPrecipitator", DLL_GroupName)

void CClassPrecipitator_UnitDef::GetOptions()
{
  SetDefaultTag("PC");
  SetDrawing("Tank", Drw_CClassPrecipitator);
  SetTreeDescription("Worsley:ClassifyingPrecipitator");
  SetDescription("TODO: Worsley Precip Model");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina);
};

//---------------------------------------------------------------------------



long   CClassPrecipitator::m_lGrowthTerm = GRM_sigma;
long   CClassPrecipitator::m_lNuclTerm = GRM_AAEq;
long   CClassPrecipitator::m_lSodaTerm = 0;
double   CClassPrecipitator::m_dActEGrowth = -12260.;
double   CClassPrecipitator::m_dActENucleation = 8344.   ;
double   CClassPrecipitator::m_dActESoda = 2535.;
double   CClassPrecipitator::m_dK_G = 2.18e15  	 ;
double   CClassPrecipitator::m_dK_N =	2.211e-6 ;
double   CClassPrecipitator::m_dn_s = 1.0;
double   CClassPrecipitator::m_dgamma_2 = 1.00;
double   CClassPrecipitator::m_dgamma_g = 2.0;
double   CClassPrecipitator::m_dgamma_N = 2.0;
double   CClassPrecipitator::m_dgamma_s = 2.00;



enum PrecipMethod {GRM_Fixed, GRM_White, GRM_CAR_QAL, GRM_TTTest};
enum CoolType{COOL_dQ, COOL_dT, COOL_Hx};
enum EvapType{EVAP_NONE, EVAP_FIXED, EVAP_dT};
enum ICool{COOL_NONE, COOL_INTERNAL, COOL_EXTERNAL};

enum ClassifierMethod {CM_Molerus, CM_Lynch, CM_Plitt };


enum ThermalHeatLossMethods {THL_None, 
			     THL_TempDrop, 
			     THL_FixedHeatFlow, 
			     THL_Ambient,
			     THL_FixedTemp};

void CClassPrecipitator::BuildDataFields()
{
  static MDDValueLst DDB1[]={
    {GRM_AAEq,   "A - Aeq" },
    {GRM_sigma,   "sigma"},
    {0}};

  static MDDValueLst DDB2[]={
    {0,   "Mass" },
    {1,   "Fraction"},
    {2,   "Number"},
    {0}};

  static MDDValueLst DDB3[]={
    {THL_None,          "None" },
    {THL_TempDrop,      "TempDrop"},
    {THL_FixedHeatFlow, "FixedLoss"},
    {THL_Ambient,  "Ambient"},
    {THL_FixedTemp, "FixedTemp"},
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

  static MDDValueLst DDB8[] = {
    {0,   "Quasi Timestep" },
    {1,   "Direct"},
    {0}};

  static MDDValueLst DDB13[]={
    {0,   "A/C - A/Ceq" },
    {1,   "Growth Rate"},
    {0}};


  static MDDValueLst DDB14[]={
    {0,   "Growth" },
    {1,   "Aggl (N/s/m^3)"},
    {2,   "deltaN"},
    {3,   "oldN"},
    {4,   "newN"},
    {0}};
 
  static MDDValueLst DDB15[]={
    {CM_Molerus,   "Molerus" },
    {CM_Lynch,   "Lynch"},
    {CM_Plitt, "Plitt"},
    {0}};



  DD.Text  ("");

#ifdef TTDEBUG
  DD.CheckBox("TTDBG", "",  &bTTDebug, MF_PARAMETER);
#endif
  DD.CheckBox("On", "",  &bOnLine, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Use.Saved.Tank", "",  &bTest, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Verbose", "",  &bVerbose, MF_PARAMETER|MF_SET_ON_CHANGE|MF_INIT_HIDDEN);

  DD.Text("");
  DD.CheckBox("Growth.On", "",  &bGrowthOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Agglom.On", "",  &bAgglomOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Nucleation.On", "",  &bNuclOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  //DD.CheckBox("Classif.On", "",  &bClassOn, MF_PARAMETER|MF_SET_ON_CHANGE);

  DD.Text("");
  DD.Text  ("Requirements");
  DD.Double("TankVol", "",	        &dTankVol         ,MF_PARAMETER, MC_Vol("m^3"));
  DD.Double("UFValve", "",	        &m_dUFrac         ,MF_PARAMETER, MC_Frac("%"));
  DD.Double("ShortCircuit", "",	        &m_dShort_Circ         ,MF_PARAMETER, MC_Frac("%"));

  DD.Long  ("ThermalLossMethod", "",&iThermalLossMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB3);
  DD.Show(iThermalLossMethod==THL_TempDrop);
  DD.Double("Temp_Drop", "",        &dTempDropRqd     ,MF_PARAMETER, MC_dT("C"));
  DD.Show(iThermalLossMethod==THL_FixedTemp);
  DD.Double("FixedTemp", "",        &dFixedTempRqd     ,MF_PARAMETER, MC_T("C"));
  DD.Show(iThermalLossMethod==THL_FixedHeatFlow);
  DD.Double("ThermalLossRqd", "",   &dThermalLossRqd  ,MF_PARAMETER, MC_Pwr("kW"));
  DD.Show(iThermalLossMethod==THL_Ambient);
  DD.Double("ThermalLossAmbient", "",  &dThermalLossAmbient ,MF_PARAMETER, MC_UA);
  DD.Show();
  DD.Long  ("Evaporation", "", &iEvapMethod , MF_PARAMETER|MF_SET_ON_CHANGE, DDB6);

  DD.Show(iEvapMethod!=EVAP_NONE);
  DD.Double("Evap.Rate", "",  &m_dEvapRate ,iEvapMethod==EVAP_FIXED?MF_PARAMETER:MF_RESULT, MC_Qm("kg/s"));
  DD.Show(iEvapMethod==EVAP_dT);
  DD.Double("Evap.Per.degK", "",            &m_dEvapRateK         ,MF_PARAMETER, MC_Qm("kg/s"));

  DD.Show();
  //DD.Long  ("Cooling", "", &iCoolType, MF_PARAMETER|MF_SET_ON_CHANGE, DDB7);
  


  DD.Text("");

  DD.Text  ("");
  DD.Text  ("Results Tank");
  DD.Double("ResidenceTime", "",    &dResidenceTime ,MF_RESULT, MC_Time("h"));
  DD.Double("SuperSat", "", &m_dSSat, MF_RESULT, MC_);
  DD.Double("SSA",  "", &dSSA, MF_RESULT, MC_SurfAreaM);
  DD.Double("Yield", "",            &dYield         ,MF_RESULT, MC_Conc("kg/m^3"));
  DD.Double("THA.Precip", "",        &dTHAPrecip     ,MF_RESULT, MC_Qm("kg/s"));
  DD.Double("AluminaPrecip", "",    &dAlPrecip     ,MF_RESULT, MC_Qm("kg/s"));
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
  DD.Double("GrowthRate", "",       &m_dGrowth          ,MF_RESULT, MC_Ldt("um/h"));
  DD.Double("NuclRate", "",         &m_dNucleation          ,MF_RESULT, MC_);
  DD.Double("NuclNum", "",          &m_dNuclN          ,MF_RESULT, MC_);
  DD.Double("AgglNum", "",          &m_dAgglomN          ,MF_RESULT, MC_);
  DD.Double("AgglMin", "",          &agmin,   MF_RESULT, MC_);
  DD.Double("BoundSoda",  "",       &dSoda,             MF_RESULT, MC_);
 

  DD.Text("");
  
  DD.Page("Precip");

  DD.Double("ConvergenceLimit", "",   &m_dConvergenceLimit, MF_PARAMETER|MF_INIT_HIDDEN, MC_);
  DD.Double("Acceleration", "",   &m_dAcc, MF_PARAMETER|MF_INIT_HIDDEN, MC_);
  DD.Double("AggMinLimit", "",   &dAggMinLim, MF_PARAMETER|MF_INIT_HIDDEN, MC_);

  DD.Double("ThermalDamping", "",   &m_dDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  DD.Double("MassDamping", "",   &m_dMassDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));

  DD.Long("Iterations", "", &m_lIterations, MF_RESULT|MF_NO_FILING);
  DD.Long("IterMax", "", &m_lItermax, MF_PARAMETER);
  


  DD.Text("");
  DD.Text("Plant Parameters");
  DD.Long("MaxAgglClass", "",   &m_lMaxAgglomClass, MF_PARAMETER);
  DD.Double("NuclPlantCorr", "", &m_dNuclPlantCorr, MF_PARAMETER, MC_);
  DD.Double("AgglomerisationConst", "", &m_dKAgglom, MF_PARAMETER, MC_);
  DD.Double("SurfaceActivity", "", &m_dSurfaceAct, MF_PARAMETER, MC_);
  DD.Double("SodaConst", "", &m_dSodaConst, MF_PARAMETER, MC_);
 
  

  DD.Text("");
  DD.Text("Linear Growth Rate Equation");
  DD.Double("GrowthConst", "",     &m_dK_G   	   ,MF_PARAMETER, MC_);
  DD.Double("GrowthActEnergy", "", &m_dActEGrowth  ,MF_PARAMETER, MC_T("K"));
  DD.Double("GrowthPwr", "",     &m_dgamma_g     ,MF_PARAMETER, MC_);
  DD.Long  ("GrowthTerm", "", &m_lGrowthTerm, MF_PARAMETER|MF_SET_ON_CHANGE, DDB1); 
  DD.Double("Sigma.Term", "",          &m_dsigma          ,MF_RESULT|MF_NO_FILING, MC_);
 

  DD.Text("");
  DD.Text("Nucleation Rate Equation");
  DD.Long  ("NucleationTerm", "", &m_lNuclTerm, MF_PARAMETER|MF_SET_ON_CHANGE, DDB1); 
  DD.Double("NucleationActEnergy", "", &m_dActENucleation       ,MF_PARAMETER, MC_T("K"));
  DD.Double("NucleationConst", "",    	       &m_dK_N	    ,MF_PARAMETER, MC_);
  DD.Double("NucleationPwr", "",    	       &m_dgamma_N 	    ,MF_PARAMETER, MC_);

  DD.Text("");
  DD.Text("Agglomeration Rate Equation");
  DD.Double("SolidDenPwr", "",    	       &m_dn_s  	    ,MF_PARAMETER, MC_);
  DD.Double("GrowthPwrAggl", "",    	       &m_dgamma_2	    ,MF_PARAMETER, MC_);

  DD.Text("");
  DD.Text("Soda Incorporation");
  DD.Long  ("SodaTerm", "", &m_lSodaTerm, MF_PARAMETER|MF_SET_ON_CHANGE, DDB13);
  DD.Double("SodaPwr", "", &m_dgamma_s, MF_PARAMETER, MC_);
  DD.Double("SodaActEnergy",     "", &m_dActESoda    ,MF_PARAMETER, MC_T("K"));


  DD.Page("PSD");
  DD.Double("Feed.N.Tot", "", &dFeedNTot, MF_RESULT, MC_);
  DD.Double("Prod.N.Tot", "", &dProdNTot, MF_RESULT, MC_);
    

  // Classification Stuff
  //DD.Show(bClassOn);

  DD.Page("Classification");

  DD.Long  ("ClassMethod", "", &iClassMethod , MF_PARAMETER|MF_SET_ON_CHANGE, DDB15);
  DD.Double("ExcessBypassUFlow", "",   &m_dBypass         ,MF_PARAMETER, MC_Frac("%"));
  DD.Double("MeasureSharpness",  "",   &m_dSharp_Index     ,MF_PARAMETER, MC_);
  DD.Double("CutSize",           "",   &m_dCut_Size        ,MF_PARAMETER, MC_);
  DD.Double("LiquorToUFlow",  "",      &m_dSlurry_split    ,MF_PARAMETER, MC_Frac("%"));
  DD.Double("OverPassOFlow",  "",      &m_dOverpass       ,MF_PARAMETER, MC_Frac("%"));
  DD.Double("L_0",  "",                &m_dL0              ,MF_PARAMETER, MC_);
  DD.Text("Results");
  DD.Double("SolidsSplit",  "",  &m_dxS        ,MF_RESULT, MC_Frac("%"));
  DD.Double("Efficiency",  "",   &m_dEff        ,MF_RESULT, MC_);
  DD.CheckBox("TargetSolids", "",  &bTargetSolids, MF_PARAMETER|MF_SET_ON_CHANGE);
  
  DD.Double("UFlowSolids", "", &m_dTargetSolids, bTargetSolids ? MF_PARAMETER : MF_RESULT, MC_Frac("%"));
  
   

  DD.Show();
  DD.Page("Inlet");

  DD.Long("PSD.Display", "", &iPSD, MF_PARAMETER|MF_SET_ON_CHANGE, DDB2);
  const int NumSpecies = gs_MVDefn.Count();
  for (int i=0; i<=nClasses; i++) { 
    std::stringstream os;
    os << "Size " << std::setw(3) << i << std::setw(12) << L[i] << " " << Lav[i] ;
    DD.Double(os.str().c_str(), "", dd[2]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  

  /*******************  
  DD.Page("Size In");
  for (int i=0; i<=nClasses; i++) { 
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[3]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }************************/

  DD.Page("Size In");
  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[4]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Text("");
  DD.Double("Number", "", dd[4]+26, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Area", "", dd[4]+27, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Vol", "", dd[4]+28, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Mass", "", dd[4]+29, MF_RESULT|MF_NO_FILING, MC_None);

  DD.Page("Size Tank");
  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[5]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Text("");
  DD.Double("Number", "", dd[5]+26, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Area", "", dd[5]+27, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Vol", "", dd[5]+28, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Mass", "", dd[5]+29, MF_RESULT|MF_NO_FILING, MC_None);
  
  DD.Page("Numbers...");
  //  DD.Page(DDB14[iDType].m_pStr);
  DD.Long("DType", "", &iDType, MF_PARAMETER|MF_SET_ON_CHANGE, DDB14);  

  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[6]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Object(SavedTank, MDD_RqdPage);

    
#ifdef TTDEBUG
  DD.Page("Growth...");

  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[7]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Page("Aggl...");

  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[8]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Page("Nin...");

  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[9]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }

  DD.Page("Nout...");

  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[10]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }

#endif



}

//---------------------------------------------------------------------------

bool CClassPrecipitator::ValidateDataFields()
  {//ensure parameters are within expected ranges
  return true;
  }






CClassPrecipitator::CClassPrecipitator(MUnitDefBase *pUnitDef, TaggedObject * pNd) :  
  MBaseMethod(pUnitDef, pNd), SavedTank(this, "Tank")
  //  Evap(this, "Evap")
{   //default values...
//   x=new double[4];
//   xo = new double[4];


  m_bEvapConnected = 0;

  bKernelOK = false;
  bVerbose = false;
  bOnLine = 1;
  dTempDropRqd = 0.5;
  dFixedTempRqd = C2K(70);
  dThermalLossRqd = 1500.0;
  iThermalLossMethod = THL_TempDrop;
  iCoolMethod=0;
  bTest = 0;

  dTankVol = 3000;
  dAggMinLim = .5;
  iPSD=1;
  dSolPrecip  = 0.0;
  dGrowthRate = 0.0;
  dGrowthRateFactor = 0.0;
  dYield      = 0.0;
  dTin        = C2K(25.0);
  dTout       = C2K(25.0);
  dQvin       = 0.0;
  dQvout      = 0.0;
  dACin       = 0.0;
  dACout      = 0.0;
  dResidenceTime = 0.0;
  dThermalLoss   = 0.0;
  dReactionHeat  = 0.0;
  m_dCoolOutTotHz = 0.0;
  dtact = 0.5;
  m_dAcc = 20.;

  // Planat Parameters
  m_lMaxAgglomClass = 18;
  m_dSurfaceAct = 1.0;
  m_dSodaConst = 9e-3;
  m_dKAgglom = 8.5e-13;
  m_dNuclPlantCorr = 300.0;  

  m_dTargetSolids = .05;
  

  alreadySolved = false;

  m_dBypass  = 0.05;
  m_dSharp_Index = 5.0  ;
  m_dCut_Size     = 100.0;
  m_dSlurry_split = .05;
  m_dOverpass = 0.0;
  m_dL0 = 0.0;

  m_lIterations = 0;
  m_lItermax = 50;

}



//---------------------------------------------------------------------------

CClassPrecipitator::~CClassPrecipitator()
{
//   delete[] x;
//   delete[] xo;


}

//---------------------------------------------------------------------------

void CClassPrecipitator::Init()
{
  SetIODefinition(s_IODefs);

#ifdef TTDEBUG
  dbg.tcltk_init();
#endif

}


//---------------------------------------------------------------------------

const long OM_Simple      = 0;
const long OM_Condensing  = 1;



//---------------------------------------------------------------------------
// Modified for NaAl[OH]4 as primary species.
void CClassPrecipitator::AdjustMasses()
{
  double &InAluminaMass  = Feed.MassVector[spAlumina];     // Al2O3(l)
  double &InWaterMass    = Feed.MassVector[spWater];       // H2O
  double &InTHAMass      = Feed.MassVector[spTHA];         // Al[OH]3(s)
  double &InCausticMass  = Feed.MassVector[spCausticSoda]; // NaOH
  double &InOccSodaMass = Feed.MassVector[spOccSoda];
  double &InOrganicMass = Feed.MassVector[spOrganics];
  //  double &InBoundOrganicMass = Feed.MassVector[spBoundOrganics];

  double &AluminaMass  = Tank.MassVector[spAlumina];     // Al2O3(l)
  double &WaterMass    = Tank.MassVector[spWater];       // H2O
  double &THAMass      = Tank.MassVector[spTHA];         // Al[OH]3(s)
  double &CausticMass  = Tank.MassVector[spCausticSoda]; // NaOH
  double &OccSodaMass = Tank.MassVector[spOccSoda];
  double &OrganicMass = Tank.MassVector[spOrganics];
  //  double &BoundOrganicMass = Tank.MassVector[spBoundOrganics];

  const int NumSpecies = gs_MVDefn.Count();
  for (int i=0; i<NumSpecies; i++)
    Tank.M[i] = Feed.M[i];
  


  

  if (m_dMassDamping>0.0) {
    // Mass damping terms... may only be needed at veryhigh rates
    for (int i=0; i<nPRECIPCOMPS; i++)
    x[i] = xo[i]*m_dMassDamping+x[i]*(1-m_dMassDamping);
  }
  
  ///
  // adjust masses...   x is mass of precipitated Gibbsite
  AluminaMass = InAluminaMass - Alpha*x[iALUMINA];

  THAMass     = InTHAMass     + x[iALUMINA];
  CausticMass = InCausticMass   - (2*MW_NaOH)/MW_Na2O*x[iOCC_SODA];
  //OrganicMass = InOrganicMass - x[iBOUND_ORGANICS];
  // BoundOrganicMass = InBoundOrganicMass + x[iBOUND_ORGANICS];
  OccSodaMass = InOccSodaMass + x[iOCC_SODA];
  WaterMass = InWaterMass - x[iWATER] - (1-Alpha)*x[iALUMINA]+MW_H2O/MW_Na2O*x[iOCC_SODA];
  //  if (m_bUpdateT) Tank.MarkStateChanged();    // Manual fixup of heat loads eventually...
}

  


void CClassPrecipitator::AdjustT() 
{

  double dHeatIn = Feed.totHz();
  dReactionHeat = x[iALUMINA]*GibbsiteHOR(K2C(Tank.T));
  double newTotDH = m_dOldTotDH*m_dDamping+(m_dTotThermalLoss-dReactionHeat)*(1-m_dDamping);


  if (iThermalLossMethod == THL_TempDrop) 
    Tank.T = Feed.T - dTempDropRqd;
  else if (iThermalLossMethod == THL_FixedTemp)
     Tank.T = dFixedTempRqd;
  else
    Tank.Set_totHz(dHeatIn - newTotDH);
  
  
  m_dOldTotDH = newTotDH;
}

  
  



void CClassPrecipitator::AdjustSSA() 
{
}




//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// All the precipitation rate calcs are done in this routine 
// Takes the tank composition and calculates rates from this
// Rates (in kg/s) are put into global x
//---------------------------------------------------------------------------
void CClassPrecipitator::EvalPrecipRates(MStream & Tank) 
{
  Numbers(Tank);

  xo[0] = x[0]; xo[1]=x[1]; xo[2]=x[2]; xo[3]=x[3];
  double solidsIn = m_PSDin.getVol()*Feed.Volume(MP_All, C2K(25.))*2420;      
  double solidsOut = m_PSD1.getVol()*Tank.Volume(MP_All, C2K(25.))*2420;     
  x[iALUMINA] = (solidsOut-solidsIn);  // Precipitation is as trihydrate
  double xx = x[iALUMINA];

  x[iBOUND_ORGANICS]=0.;
  x[iOCC_SODA]=x[iALUMINA]*dSoda/100.;
  //x[iOCC_SODA] = 0;
  x[iALUMINA] -= x[iOCC_SODA];
  x[iWATER]= 0.0;
}

//---------------------------------------------------------------------------
// All the thermal loss calcs are done in this routine 
// Also calculate the water loss through evaporation
// m_dTotThermalLoss is the total thermal loss, 
//---------------------------------------------------------------------------

void CClassPrecipitator::EvalLosses(MStream &Tank)
{
  
}

void CClassPrecipitator::AdjustPSD() 
{
  m_PSD1 = BrahmaPSD(NewN);
  m_PSD1.put(Tank);  
}


//------------------------------------------------------------------------
//  Check for convergence of the iteration
//
//
//------------------------------------------------------------------------

bool CClassPrecipitator::ConvergenceTest() 
{
  double xmag = (x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);
  double err = (Sqr(x[0]-xo[0]) + Sqr(x[1]-xo[1]) +Sqr(x[2]-xo[2]))/GTZ(xmag);
  //double err = 0.0;
  double nn=0;
  double nt =0;
  for (int i=1; i<=nClasses; i++) {
    if (NewN[i]==0 && n[i]==0)
      {}
    else
      err += Sqr((NewN[i]-n[i])/(NewN[i]+n[i]));
	//    nn+= Sqr(NewN[i]);
	//    nt+= Sqr(NewN[i]-n[i]);
    
  }
  // err += nt/GTZ(nn);
  for (int i=1; i<=nClasses; i++) n[i] = NewN[i];

  
  if (err < m_dConvergenceLimit) 
    return true;
  else
    return false;
}


void CClassPrecipitator::DoClassifier(MStream & OFlow, MStream &UFlow) 
{
  double pVol_U = 0.0, pVol_O=0.0, mO=0.0, mU=0.0;
  double eff, ab, nl[nClasses+1][2];
  double xS;
  
  MIPSD & FeedPSD = Feed.IF<MIPSD>(false);
  MIPSD & OFPSD = OFlow.IF<MIPSD>(false);
  MIPSD & UFPSD = UFlow.IF<MIPSD>(false);

  for (int i=0; i<=nClasses; i++) {
    if (m_dL0==0) ab=m_dBypass; else ab=m_dBypass*(1-Lav[i]/m_dL0);
    switch (iClassMethod) {
    case CM_Molerus:
      eff = ab + (1-ab) * (1-m_dOverpass) /
	( 1 + pow(Lav[i]/m_dCut_Size, -m_dSharp_Index) );
      break;
    case CM_Lynch:
      eff = ab + (1-ab) * (1-m_dOverpass) * 
	( exp(m_dSharp_Index*Lav[i]/m_dCut_Size) - 1 ) /
	( exp(m_dSharp_Index) +  exp(m_dSharp_Index*Lav[i]/m_dCut_Size) - 2 );
      break;
    case CM_Plitt:
      eff = ab + (1-ab) * (1-m_dOverpass) *
	( 1 - exp(-0.693147*pow(Lav[i]/m_dCut_Size, m_dSharp_Index)) );
      break;
    }
    nl[i][0]=FeedPSD.getFrac(i)*eff;
    nl[i][1]=FeedPSD.getFrac(i)*(1-eff);
    mU += nl[i][0];
    mO += nl[i][1];

    dd[7][i] = eff;
    
  }
  if (mO+mU !=0.0) 
    xS = mU/(mU+mO);
  else
    xS = 1.0;
  m_dxS = xS;
  m_dEff = eff;
  
  
  UFlow.SetF(Feed, MP_Liq, m_dSlurry_split);
  UFlow.AddF(Feed, MP_Sol, xS);
  OFlow.SetF(Feed, MP_Liq, 1.-m_dSlurry_split);
  OFlow.AddF(Feed, MP_Sol, 1.-xS);
  for (int i=0; i<=nClasses; i++) {
    if (xS<1.0) 
      OFPSD.putFrac(i, nl[i][1]/(1-xS));
    UFPSD.putFrac(i, nl[i][0]/xS);
  }
  OFPSD.Normalise();
  UFPSD.Normalise();
  
}


void CClassPrecipitator::DoResults()
{
  
  MIBayer & FeedB=Feed.IF<MIBayer>(false);
  MIBayer & TankB=Tank.IF<MIBayer>(false);


  
  m_dNuclN = m_PSDin.getN()-m_PSD.getN();
  dACin   = FeedB.AtoC();
  dACout = TankB.AtoC();
  dQvin   = Feed.Volume();
  dQvout   = Tank.Volume();
  dQmin = Feed.Mass();
  dQmout = Tank.Mass();

  double dQvout25 = Tank.Volume(MP_All, C2K(25));
  double Cout    = TankB.CausticConc(Tank.T);
  m_dACeq = TankB.AluminaConcSat(Tank.T)/Cout;
  m_dSSat = TankB.AtoC()/m_dACeq;
  dYield         = Cout*(dACin-dACout);
  double SolIn = Feed.MassVector[spTHA];
  double SolOut  = Tank.MassVector[spTHA];
  dTHAPrecip     = SolOut - SolIn;
  dAlPrecip = dTHAPrecip*102/156;
  SolIn = Feed.Mass(MP_Sol);
  SolOut = Tank.Mass(MP_Sol);
  dSolPrecip = SolOut - SolIn;
  dSolConc = Tank.Mass(MP_Sol)/dQvout25;
  m_dLiquorTin = Tank.T;
  dTin = Feed.T;
  dTout = Tank.T;

  m_dNucleation = m_dNuclN * PI/6*1.4618*Sqr(L[1])*L[1]*1e-18*Tank.Density(MP_Sol);
  dResidenceTime = dTankVol/GTZ(Tank.Volume(MP_SL));
  dFeedNTot = Feed.Volume(MP_All, C2K(25))*m_PSDin.getN();
  dProdNTot = Tank.Volume(MP_All, C2K(25))*m_PSD1.getN();
  dSSA = m_PSD1.getArea()/dSolConc/1000.;
  
}






void CClassPrecipitator::EvalProducts()
{
  try {
    x[0] = x[1] = x[2] = x[3] = 0.0;
    FlwIOs.AddMixtureIn_Id(Feed, idFeed);
    MStream & UFlow = FlwIOs[FlwIOs.First[idUflow]].Stream; //Reference to the output stream
    MStream & OFlow = FlwIOs[FlwIOs.First[idOflow]].Stream; //Reference to the output stream
    MStream OutFlow;
    
    if (SavedTank.MassFlow() < 1.0e-5 || !bTest) {
      Tank = Feed;
    } else {
      Tank = SavedTank;
    }
    
      
    
    OFlow = Feed;
    UFlow = Feed;
    OFlow.SetF(Feed, MP_All, 0.0);
    UFlow.SetF(Feed, MP_All, 1.0);



    bool streamOK=(Feed.Mass()>1.0e-4);
    


    if (streamOK) {
    // Check for existence of Bayer Property... 
    
      MIBayer & FeedB=Feed.IF<MIBayer>(false);   // Do the checks up front
      if (bVerbose)
	Log.SetCondition(IsNothing(FeedB), 1, MMsg_Warning,
		     "Bad Feed Stream - Not BATC Bayer Model");


      // Try PSD
      MIPSD & FeedPSD = Feed.IF<MIPSD>(false);
      if (!IsNothing(FeedPSD)) {
	lPBType = PB_PSD;
	m_PSDin = BrahmaPSD(Feed);
      }
      else {   // Try SSA...
	MISSA & FeedSSA = Feed.IF<MISSA>(false);
	if (!IsNothing(FeedSSA))
	  lPBType = PB_SSA;
	else
	  lPBType = PB_None;
      }


      if (bVerbose)
	Log.SetCondition(lPBType==PB_None, 1, MMsg_Warning, 
		       "Bad Feed Stream - No PSD or SSA Property");

      if (IsNothing(FeedB) || IsNothing(FeedPSD)) streamOK = false;
      if (!IsNothing(FeedPSD)) {
	displayPSD(Feed, 2);
      }
    } else {
      if (bVerbose)
	Log.SetCondition(!streamOK, 1, MMsg_Warning,
		     "Zero Flow");
    }
    if (bOnLine && streamOK) {  // Online and have Bayer and SSA properties...
      
      if (!bKernelOK) {
	AgglomKernel();
	bKernelOK = true;
      }

      



      DoPrecip();

      displayPSD(m_PSDin, 4);
      displayPSD(m_PSD1, 5);
      DoResults();
      UFlow = Tank;
      alreadySolved = true;
      SavedTank = Tank;
      DoClassifier(OFlow, UFlow);
      double USolids = UFlow.MassFlow(MP_Sol);
      double UTotal = UFlow.MassFlow(MP_All);

      if (bTargetSolids) {
	double sf = USolids/UTotal;
	double t = USolids/m_dTargetSolids; // required underflow
	if (t <= UTotal) { // Need to deliquor: xfer excess to oflow
	  double dl = UTotal-t;   // need to remove this much liquor
	  OFlow.AddM(UFlow, MP_Liq, dl);
	  UFlow.AddM(UFlow, MP_Liq, -dl);
	} else {
	  double al = t-UTotal;
	  double OLiq = OFlow.MassFlow(MP_Liq);
	  if (al<OLiq) {
	    OFlow.AddM(OFlow, MP_Liq, -al);
	    UFlow.AddM(OFlow, MP_Liq, al);
	  } else { 
	    Log.Message(MMsg_Warning, "Unable to achieve required underflow solids");
	  }


	}
	    
      } else {
	m_dTargetSolids = USolids/UTotal;
      }

    } else  {   // Just tidy up and put some sensible stuff in the results...

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




void CClassPrecipitator::ClosureInfo(MClosureInfo & CI)
  {//ensure heat balance
  if (CI.DoFlows())
    {
    CI.AddPowerIn(0, -dThermalLoss);
    }
  }




void CClassPrecipitator::DoPrecip()
{

  bool converged = false;
  int Iter=1;    // Number of iterations in main loop
  m_dOldTotDH = 0.0;
  MIBayer & tb=Tank.IF<MIBayer>(false);


  m_PSD = BrahmaPSD(Feed);
  m_PSD1 = BrahmaPSD(Tank);  
  m_PSD1.getN(n);

  while (!converged && Iter < m_lItermax) {    
    // Main iterative loop on final temperature and concentration
    m_lIterations = Iter;

    EvalPrecipRates(Tank);    /// Determine the precipitation rates

    EvalLosses(Tank);     /// Evaluate the thermal losses

    AdjustMasses();     /// Adjust the mass flows

    AdjustT();        /// Adjust temperature for thermal losses, reaction heats etc
 
    
    //    AdjustSSA(Tank);      /// Adjust SSA
    AdjustPSD();      /// Adjust PSD

    converged = ConvergenceTest();   //// Final convergence test  

    Iter++;
  }
  if (Iter==m_lItermax) {/// Convergence Failure, automatically increase damping and iterations???
  }
  m_lIterations = Iter;
  Log.SetCondition(Iter==m_lItermax, 4, MMsg_Warning, "No convergence of precip routine");
  
}







void CClassPrecipitator::displayPSD(MStream &ms, int scrn)
{
  MIPSD & mpsd = ms.IF<MIPSD>(false);   /// Does the mud stream have PSD...
  int ic = mpsd.getSizeCount();
  if (ic>40) ic=40;
  double x;
  BrahmaPSD p(ms);
  for (int i=0; i<ic; i++) {
    switch (iPSD) {
    case 0:
      x = mpsd.getMass(i);
      break;
    case 1: 
      x = mpsd.getFrac(i);
      break;
    case 2: 
      x = p.getN(i);
      break;
      
    }
    dd[scrn][i]=x;
  }
}

void CClassPrecipitator::displayPSD(BrahmaPSD &p, int scrn) {
  for (int i=0; i<=nClasses; i++) {
    dd[scrn][i]=p.getN(i);
  }
  dd[scrn][26] = p.getN();
  dd[scrn][27] = p.getArea();
  dd[scrn][28] = p.getVol();
  dd[scrn][29]=0.0;
  
}






//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-07-11 12:39:54 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================



void CClassPrecipitator::AgglomKernel() {
  double dm;
  int min;

  for (int i=0; i<=nClasses; i++) {
    for (int j=0; j<=nClasses; j++) {
      min = Min(i, j);
      dm = 1.0 - Sqr((Lav[i]+Lav[j])/Lav[m_lMaxAgglomClass]);
      if (dm<0.0)
	beta[i][j]=0;
      else
	beta[i][j] = Sqr(Lav[i]+Lav[j])/Lav[min]*dm;
    }
  }
  
}



void CClassPrecipitator::Agglomeration(double Growth, double SD) {
  double ag; 
  agmin=0.0;
  double fctr = m_dKAgglom/pow(SD, m_dgamma_2)*pow(Growth, m_dgamma_g); 

  for  (int i=1; i <= nClasses; i++) {
    if (beta[i-1][1]==0.0)
      aggl[i]=0.0;
    else {
      aggl[i]=0.0;
      if (i>1) {   /// A+ term
	ag=0.0;
	for (int j=0; j<=i-2 && beta[i-1][j]>0; j++)
	    ag+=exp((j-i+1)*.6931472)*beta[i-1][j]*n[j];
	aggl[i] = n[i-1]*ag + 0.5*beta[i-1][i-1]*Sqr(n[i-1]);
      }
      ag = 0.0;
      for  (int j=0; j<=i-1 && beta[i][j]>0; j++)   // A- term 1
	  ag += exp((j-i)*0.6931472) * beta[i][j] * n[j];
      aggl[i] -= n[i] * ag;
      agmin   -= n[i] * ag;
      ag = 0;
      for  (int j=i;  j <= nClasses  &&  beta[i][j]>0; j++)  // A- term 2
	  ag += beta[i][j] * n[j];
      aggl[i] -= n[i] * ag;
      agmin  -= n[i] * ag;
      aggl[i]*=fctr;

    }
  }
  agmin *= fctr;
}


     
double CClassPrecipitator::GrowthIn( MStream &s)
{
  
  double hulp, term;
  MIBayer & sB=s.IF<MIBayer>(false);   // Do the checks up front
  double T = s.T;
  double C = sB.CausticConc(T);
  if (C<ZeroLimit)
    {
    m_dsigma = 0.0;
    return 0.0;
    }

  double AC = sB.AtoC();
  double ASat = sB.AluminaConcSat(T);
  double ACEquil = ASat/C;

  switch (m_lGrowthTerm) {
  case GRM_sigma: {
    term = (AC-ACEquil) / (ACEquil*(1-1.0392*AC));
    break;
  }
  case GRM_AAEq: {
    term = sB.AluminaConc(T)-ASat;
    break;
  }
  }
  //m_dsigma = term;
  hulp = m_dSurfaceAct * m_dK_G *
    exp(m_dActEGrowth/T) * pow(fabs(term),m_dgamma_g);
  m_dsigma = m_dActEGrowth/T;

  if (AC > ACEquil)
    return  hulp;
  else 
    return 0.0;
    
}


double CClassPrecipitator::Soda(MStream &s) {

  double term;
  MIBayer & sB=s.IF<MIBayer>(false);   // Do the checks up front
  double T = s.T;
  double C = sB.CausticConc(T);
  if (C<ZeroLimit)
    {
    return 0.0;
    }
  switch (m_lSodaTerm) {
  case 0: {
    double AC = sB.AtoC();
    double ACEquil = sB.AluminaConcSat(T)/C;
    term  = AC-ACEquil; 
    break;
  }
  case 1:
    term = m_dGrowth*3600*1.0e9;
    break;
  }
  if (term <= 0.0) return 0.0;
  return m_dSodaConst*exp(m_dActESoda/T)*pow(fabs(term), m_dgamma_s);
}


// Nucleation Rate; particles into bin[1] per hour per m^3

double CClassPrecipitator::NucleationIn( MStream &s)  
{
  
  double term;
  MIBayer & sB=s.IF<MIBayer>(false);   // Do the checks up front
  double T = s.T;
  double C = sB.CausticConc(T);
  double ASat = sB.AluminaConcSat(T);
  switch (m_lNuclTerm) {
   
  case GRM_AAEq:     
    term  = sB.AluminaConc(T)-ASat;
    break;
  case GRM_sigma: {
    double AC = sB.AtoC();  
    double ACEquil = ASat/C;
    term  = (AC-ACEquil) / (ACEquil*(1-1.0392*AC));
    break;
  }
  }
  if (term <= 0) return 0;
  return m_dNuclPlantCorr * m_dK_N * exp(m_dActENucleation/T) *
             pow(fabs(term), m_dgamma_N) * m_PSD.getArea();
}
  

void CClassPrecipitator::Numbers(MStream & Tank) 
{

  static const double bayerRefTemp = 273.15+25;
  static const double r = 1.25992104989;
  static const double ConA = 3*r;     //{ 3.7798}
  static const double ConB = -3.;      //{-3.0000}

  double GrowthRate[nClasses+1];
  double deltaN[nClasses+1];

  double Growth = GrowthIn(Tank);

  m_dGrowth = Growth/1.0e6/3600;
  m_dNuclN = NucleationIn(Tank);

  dSoda = Soda(Tank);
  double SD = Tank.MassFlow(MP_Sol)/Tank.VolumeFlow(MP_All);
  double term;
  Agglomeration(Growth, SD);

  if (!bGrowthOn) Growth=0.0;
  // Check for largest growth???
  double gs = 0.0;
  for (int i=1; i<=nClasses; i++) {
    if (i<nClasses) 
      term = n[i]*ConB;
    else
      term = 0;
    if (i>1) term+=n[i-1]*ConA;
    GrowthRate[i] = Growth/L[i]*term/3600;
    gs += GrowthRate[i];
  }
  deltaN[0] = gs;

  for (int i=1; i<=nClasses; i++) {
    double dn = (Feed.VolumeFlow(MP_All, bayerRefTemp)*m_PSDin.getN(i)
		   -Tank.VolumeFlow(MP_All, bayerRefTemp)*n[i])/dTankVol;
    if (bAgglomOn) dn+=aggl[i]/3600; 
    if (bGrowthOn) dn+=GrowthRate[i];
    deltaN[i] = dn;
  }
  
  if (bNuclOn) {
      deltaN[1] += m_dNuclN/3600;
  }
  NewN[0] = 0;
  double acc = m_dAcc;
  // Reduce acceleration if iteration number is large... usually a sign that
  // there is overshoot
  if (m_lIterations > 500) acc /=2;
  if (m_lIterations > 1000) acc /=2;
  if (m_lIterations > 1500) acc /=2;
  


  // Check that none of the rates lead to negative numbers...			     
  for (int i=1; i<=nClasses; i++) {
    if (deltaN[i]<0) {
      double atmp = -(1-dAggMinLim)*n[i]/deltaN[i];
      if (atmp<acc) {
	acc = atmp;
      }
    }
  }
  
  for (int i=1; i<=nClasses; i++) {
    NewN[i] = n[i] + deltaN[i]*acc;
  }


  //  if (!err)
  for (int i=1; i<=nClasses; i++) {

    switch (iDType) {
    case 0:
      dd[6][i]=GrowthRate[i];
      break;
    case 1:
      dd[6][i]=aggl[i]/3600;
    break;
    case 2:
      dd[6][i]=deltaN[i];
      break;
    case 3:
      dd[6][i]=n[i];
      break;
    case 4:
      dd[6][i]=NewN[i];
      break;
    }
  }
  




}



