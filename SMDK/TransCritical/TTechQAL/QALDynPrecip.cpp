//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//           QAL Classifier Model 2004 - Transcritical Technologies/ QAL 
//   Time-stamp: <2007-05-23 03:58:13 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __QALPRECIP_CPP
#include "qaldynprecip.h"



static MInitialiseTest InitTest("DynPrecip");


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
static MSpeciePtr  spOxalate       (InitTest, "Na2C2O4(s)", false);


enum PrecipMethod {GRM_Fixed, GRM_White, GRM_CAR_QAL, GRM_TTTest};
enum CoolType{COOL_dQ, COOL_dT, COOL_Hx};
enum EvapType{EVAP_NONE, EVAP_FIXED, EVAP_dT};
enum ICool{COOL_NONE, COOL_INTERNAL, COOL_EXTERNAL};



enum PrecipComponents {iALUMINA, iOCC_SODA, iBOUND_ORGANICS, iWATER, nPRECIPCOMPS};

enum {GRM_AAEq, GRM_sigma};

enum {PB_None, PB_SSA, PB_PSD};  // Particle balance model

enum {idFeed, idUflow, idOflow};


enum ThermalHeatLossMethods {THL_None, THL_TempDrop, THL_FixedHeatFlow, THL_Ambient,  THL_FixedTemp};

///  Utility Class for heat exchanger






static MInOutDefStruct s_IODefs[]=
  {
    { "Feed",      "Feed",   idFeed,  1, 10,  0,  1.0f,   MIO_In |MIO_Material },
    { "Underflow", "UFlow",  idUflow,  1,  1,  0,  1.0f,  MIO_Out|MIO_Material },
    { "Overflow",  "OFlow",  idOflow, 0,  1,  1,  1.0f,   MIO_Out|MIO_Material },
    
    
    { NULL },
  };

double Drw_CDynPrecipitator[] = 
  { 
    MDrw_Poly, -5,10, -5,-10, 5,-10, 5,10,
    MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CDynPrecipitator, "DynPrecipitator", DLL_GroupName)

void CDynPrecipitator_UnitDef::GetOptions()
{
  SetDefaultTag("PC");
  SetDrawing("Tank", Drw_CDynPrecipitator);
  SetTreeDescription("QAL:DynamicPrecipitator");
  SetDescription("TODO: QAL PB Model");
  SetModelSolveMode(MSolveMode_All);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina);
};

//---------------------------------------------------------------------------




CDynPrecipitator::CDynPrecipitator(MUnitDefBase *pUnitDef, TaggedObject * pNd) :  
  MBaseMethod(pUnitDef, pNd)
{   //default values...


  m_bEvapConnected = 0;

  bKernelOK = false;
  bOnLine = 1;
  dTempDropRqd = 0.5;
  dFixedTempRqd = C2K(70);
  dThermalLossRqd = 1500.0;
  iThermalLossMethod = THL_TempDrop;
  iCoolMethod=0;

  dTankVol = 3000;
  dAggMinLim = .5;
  iPSD=0;
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
  dtact = 0.5;
  m_ddeltaT = 1.;
  ER_White      = 7200.0;
  K_White       = 1.96e9;
  gF_White      = 1.0;




  m_dSurfaceAct = 1.0;
  m_dSodaConst = 9e-3;
  dGrowthRateFactor = 1.0;
  dAgglRateFactor = 1.0;
  dNuclRateFactor = 1.0;
  
  
  m_dConvergenceLimit = 1.0e-6;
  alreadySolved = false;
  m_lItermax = 1000;

}





//---------------------------------------------------------------------------

CDynPrecipitator::~CDynPrecipitator()
{
//   delete[] x;
//   delete[] xo;


}


bool CDynPrecipitator::ValidateDataFields()
  {//ensure parameters are within expected ranges
  return true;
  }

//---------------------------------------------------------------------------

void CDynPrecipitator::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------




//---------------------------------------------------------------------------


// Modified for NaAl[OH]4 as primary species.
void CDynPrecipitator::AdjustMasses(MStream & Prod)
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
  CausticMass = InCausticMass  + x[iALUMINA]*(Alpha-1) - x[iOCC_SODA];
  OrganicMass = InOrganicMass - x[iBOUND_ORGANICS];
  BoundOrganicMass = InBoundOrganicMass + x[iBOUND_ORGANICS];
  BoundSodaMass = InBoundSodaMass + x[iOCC_SODA];
  WaterMass = InWaterMass - x[3];   // Water is only lost through evaporation
  //  if (m_bUpdateT) Prod.MarkStateChanged();    // Manual fixup of heat loads eventually...
}



  


void CDynPrecipitator::AdjustT(MStream &Prod) 
{

  double dHeatIn = Feed.totHz();
  dReactionHeat = x[iALUMINA]*GibbsiteHOR(K2C(Prod.T));
  double newTotDH = m_dOldTotDH*m_dDamping+(m_dTotThermalLoss-dReactionHeat)*(1-m_dDamping);


  if (iThermalLossMethod == THL_TempDrop) 
    Tank.T = Feed.T - dTempDropRqd;
  else if (iThermalLossMethod == THL_FixedTemp)
     Tank.T = dFixedTempRqd;
  else
    Tank.Set_totHz(dHeatIn - newTotDH);
  
  
  m_dOldTotDH = newTotDH;
}

  
  



void CDynPrecipitator::AdjustSSA(MStream &Prod) 
{
}






//---------------------------------------------------------------------------
// All the precipitation rate calcs are done in this routine 
// Takes the product composition and calculates rates from this
// Rates (in kg/s) are put into global x
//---------------------------------------------------------------------------
void CDynPrecipitator::EvalPrecipRates(MStream & Prod, double T) 
{
  
  PBRates();
  double deltaT = m_ddeltaT;
  for (int i=0; i<nClasses; i++) {
    if (Ndot[i] < 0) {
      double dtm = -n[i]*(1.0 - 0.7)/Ndot[i];
      if (dtm < deltaT) deltaT = dtm;
    }
  }

  double sumar = 0.0, sumv = 0.0, sumn = 0.0;
  for (int i=0; i<nClasses; i++) {
    NewN[i] = n[i] + Ndot[i]*deltaT;
    double dd = D[i];
    double dd2 = dd*dd;
    double dd3=dd2*dd;
    sumar+= dd2*NewN[i];
    sumv += dd3*NewN[i];
    sumn += NewN[i];
  }
  double newVol = PI/6*sumv*DIST_CORR*1.0e-18;


  xo[0] = x[0]; xo[1]=x[1]; xo[2]=x[2]; xo[3]=x[3];
  m_dSolidsOut = newVol*Tank.Mass()*2420;     //Tank.Density(MP_Sol);
  x[iALUMINA] = (m_dSolidsOut-m_dSolidsIn);  // Precipitation is as trihydrate



  x[iBOUND_ORGANICS]=0.;
  //x[iOCC_SODA]=x[iALUMINA]*dSoda/100.;
  //x[iOCC_SODA] = 0;
  //x[iALUMINA] -= x[iOCC_SODA];
  x[iWATER]= 0.0;
}

//---------------------------------------------------------------------------
// All the thermal loss calcs are done in this routine 
// Also calculate the water loss through evaporation
// m_dTotThermalLoss is the total thermal loss, 
//---------------------------------------------------------------------------

void CDynPrecipitator::EvalLosses(MStream & Prod)
{

  
}

void CDynPrecipitator::AdjustPSD(MStream & Prod) 
{

}


//------------------------------------------------------------------------
//  Check for convergence of the iteration
//
//
//------------------------------------------------------------------------



void CDynPrecipitator::DoResults(MStream & Prod)
{
  //m_dNuclN = m_PSDin.getN()-m_PSD.getN();
  
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
  m_dACeq = ProdB.AluminaConcSat(Prod.T)/Cout;
  m_dSSat = ProdB.AtoC()/m_dACeq;
  dYield         = Cout*(dACin-dACout);
  double SolIn = Feed.MassVector[spTHA];
  double SolOut  = Prod.MassVector[spTHA];
  dTHAPrecip     = SolOut - SolIn;
  dAlPrecip = dTHAPrecip*102/156;
  SolIn = Feed.Mass(MP_Sol);
  SolOut = Prod.Mass(MP_Sol);
  dSolPrecip = SolOut - SolIn;
  dSolConc = Prod.Mass(MP_Sol)/dQvout25;
  m_dLiquorTin = Prod.T;
  dTin = Feed.T;
  dTout = Prod.T;

  m_dNucleation = m_dNuclN * PI/6*1.4618*Sqr(L1)*L1*1e-18*Prod.Density(MP_Sol);
  dResidenceTime = dTankVol/GTZ(Prod.Volume(MP_SL));
  dFeedNTot = Feed.Volume(MP_All, C2K(25))*fpsd.getNTot();
  dProdNTot = Prod.Volume(MP_All, C2K(25))*m_dNumber;
  
}






void CDynPrecipitator::EvalProducts()
  {
  try {
    x[0] = x[1] = x[2] = x[3] = 0.0;
    FlwIOs.AddMixtureIn_Id(Feed, idFeed);
    MStream & UFlow = FlwIOs[FlwIOs.First[idUflow]].Stream; //Reference to the output stream
    MStream & OFlow = FlwIOs[FlwIOs.First[idOflow]].Stream; //Reference to the output stream
    
    //    if (!alreadySolved || UFlow.MassFlow() < 1.0e-3)
    Tank = Feed;
      //else
      //Tank = UFlow;
    UFlow = Tank;
    OFlow = Tank;
    OFlow.SetF(Tank, MP_All, 0.0);
    UFlow.SetF(Tank, MP_All, 1.0);


    bool streamOK=true;
    
    if (!bOnLine) return;

    // Check for existence of Bayer Property... 
    MIBayer & FeedB=Feed.IF<MIBayer>(false);   // Do the checks up front

    Log.SetCondition(IsNothing(FeedB), 1, MMsg_Warning, "Bad Feed Stream - Not BATC Bayer Model");


    // Try PSD
    MIPSD & FeedPSD = Feed.IF<MIPSD>(false);
    if (!IsNothing(FeedPSD)) 
      lPBType = PB_PSD;
    else {   // Try SSA...
     MISSA & FeedSSA = Feed.IF<MISSA>(false);
     if (!IsNothing(FeedSSA))
       lPBType = PB_SSA;
     else
       lPBType = PB_None;
    }


    Log.SetCondition(lPBType==PB_None, 1, MMsg_Warning, "Bad Feed Stream - No PSD or SSA Property");

    if (IsNothing(FeedB) || IsNothing(FeedPSD)) streamOK = false;
    if (!IsNothing(FeedPSD)) {
      displayPSD(Feed, 2);
    }


    if (bOnLine && streamOK) {  // Online and have Bayer and SSA properties...
      fpsd =QALPSD(Feed);
      m_dSolidsIn = fpsd.getVol()*Feed.Mass()*2420;      // Feed.Density(MP_Sol);
    
      if (!bKernelOK) {
	AgglomKernel();
	BinFactors();
	bKernelOK = true;
      }

      DoPrecip(Tank);
      
      displayPSD(Feed, 4);
      displayPSD(Tank, 5);
      DoResults(Tank);
      UFlow = Tank;
      alreadySolved = true;

    } else  {   // Just tidy up and put some sensible stuff in the results...
       Log.Message(MMsg_Warning, "Stream Problem...");
      
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




void CDynPrecipitator::ClosureInfo(MClosureInfo & CI)
  {//ensure heat balance
  if (CI.DoFlows())
    {
    CI.AddPowerIn(0, -dThermalLoss);
    }
  }




void CDynPrecipitator::DoPrecip(MStream & Prod)
{

  bool converged = false;
  int Iter=0;    // Number of iterations in main loop
  m_dOldTotDH = 0.0;


  getN(Prod);
  m_dSSA = m_dArea;
  while (!converged && Iter < m_lItermax) {    // Main iterative loop on final temperature and concentration
    EvalPrecipRates(Prod);    /// Determine the precipitation rates
    EvalLosses(Prod);     /// Evaluate the thermal losses
    AdjustMasses(Prod);     /// Adjust the mass flows
    AdjustT(Prod);        /// Adjust temperature for thermal losses, reaction heats etc
 
    
    //    AdjustSSA(Prod);      /// Adjust SSA
    //AdjustPSD(Prod);      /// Adjust PSD
    converged = true;   //// Final convergence test  
    Iter++;    
  }
  if (Iter==m_lItermax) {/// Convergence Failure, automatically increase damping and iterations???
  }
  putN(Prod);
  
  m_lIterations = Iter;
  Log.SetCondition(Iter==m_lItermax, 4, MMsg_Warning, "Precip loop failed to converge....");
}



// Precalculate Agglomeration Kernel and bin split factors

void CDynPrecipitator::AgglomKernel() {
  for (int i=0; i<nClasses; i++) {
    for (int j=0; j<=i; j++) {
      beta[i][j] = beta[j][i] = exp(-(D[i]*D[i]+D[j]*D[j])/1600.);
      // dbg.log("i, j = %3d %3d %12.5g", i, j, beta[i][j]);
    }
  }
}
  
void CDynPrecipitator::BinFactors() {
  int k;
  double alpha = 2;
  double a1 = alpha-1;
  for (int i=0; i<nClasses; i++) {
    for (int j=0; j<i; j++) {
      k = klf(i,j);
      bsplit[i][j] = (alpha - pow(alpha, i-k) - pow(alpha, j-k))/a1;
      // dbg.log("i, j = %3d %3d %12.5g", i, j, bsplit[i][j]);
    }
    bsplit[i][i] = .5;
  }
}





void CDynPrecipitator::displayPSD(MStream &ms, int scrn)
{
  MIPSD & mpsd = ms.IF<MIPSD>(false);   /// Does the mud stream have PSD...
  int ic = mpsd.getSizeCount();
  if (ic>40) ic=40;
  double x=0.0;
  QALPSD ps(ms);
  for (int i=0; i<ic; i++) {
    switch (iPSD) {
    case 0:
      x = ps.getFrac(i);
      break;
    case 1: 
      x = ps.getN(i);
      break;
    case 2: 
      x = mpsd.getSize(i);
      break;
      
    }
    dd[scrn][i]=x;
  }
}


// Ilievski Correlation (1991)
// Note this uses Caustic as Na2O
double CDynPrecipitator::AgglomRate() {
  double ssat = m_dSSat/(MW_Na2O/(2*MW_NaOH));   // Caustic as Na2O
  dAgglomRate =  5.0e-10*pow(ssat, 4)/3600;
  return dAgglomRate;
}


// Just the White method ala CAR for the moment...
double CDynPrecipitator::GrowthRate() {
  const  double T = Tank.T;

  switch (iGrowthRateMethod) {
  case GRM_Fixed:  // dGrowthRate given directly in access window
    return dGrowthRate;
  case GRM_White: 
    {
      double ssat=m_dSSat;
      if (ssat < 0) ssat=0.0;
      
      dGrowthRate = gF_White*K_White*Exps(-ER_White/Tank.T)*Sqr(ssat)/1000000./3600.;  //  m/s
    }
  case GRM_CAR_QAL:
    {
      
      MIBayer & TankB = Tank.IF<MIBayer>(false);
      double C  = TankB.CausticConc(T);
      double C25 = TankB.CausticConc(C2K(25));
      double CtoS = TankB.CtoS();
      double S_out = C25/CtoS;
      //      double FC = TankB.FreeCaustic(T);
      double FC = TankB.FreeCaustic(C2K(25));
      double ACeq = TankB.AluminaConcSat(T)/C;
      double TOOC=TankB.TOC(C2K(25))*MW_Na2CO3/MW_C;
      double a=Exps(-m_dActivationEnergy/T);
      double b=Exps(-TOOC*m_dk_TOC);
      double c=Pow(GTZ(S_out), m_dn_s);
      double d=Pow(GTZ(FC), m_dn_fc);
      double e=Pow(GTZ(ACeq), m_dn_eq);

      double K = m_dK0*a*b*c*d*e;
      double ACout = TankB.AtoC();
      double VLiq = Tank.Volume()*3600.;
      double MSolids     = Tank.MassVector[spTHA]*3600.0/1000.0;
      double Sol = MSolids*1000.0/GTZ(VLiq);
      double deltaAC = K * dResidenceTime/3600 *
	Pow(GTZ(ACout-ACeq),m_dn_) * Pow(GTZ(Sol),m_dn_sol) * Pow(GTZ(m_dSSA),m_dn_ssa);
      const double denS = Tank.Density(MP_Sol);
      const double XG = Tank.MassVector[spAlumina]/Tank.MassVector[spTHA];
      const double alpha = 117.963/77.963;
      dGrowthRate = 2./denS*XG/alpha/ACout/m_dArea*deltaAC;

    }

  }
  dGrowthYield = m_dArea*dGrowthRate/2*2420;   // kg hydrate /s /kg slurry
  
  return dGrowthRate;
}

// Misra 1970

double CDynPrecipitator::NucleationRate() {
  if (iGrowthRateMethod==GRM_CAR_QAL) {
    MIBayer & TankB=Tank.IF<MIBayer>(false);
    const double T = Tank.T;
    const double SPO = Tank.MassVector[spOxalate]/Tank.MassVector[spAlumina];
    const double A = TankB.AluminaConc(T);
    const double ASAT = TankB.AluminaConcSat(T);
      
    const double ssat = A/ASAT;
    /// Insert Code here....
    dNuclRate = (1+1500*SPO)*exp(2100*(1./343. - 1/T))*pow(ssat-1, 6)*TankB.SodaConc(T);
    
    return dNuclRate*m_dArea/3600*1000000;
  } else {
    double ssat=m_dSSat;
    if (ssat < 0) ssat=0.0;
    dNuclRate = 5.0e8*pow(ssat,4)*m_dSSA/3600;
    return dNuclRate;  
  }
  
}    


	



// Calculate numbers for agglomeration...
// Loop over all pairs of bins < NClasses
// Calculate agglomeration rate for these bins
// and destination bin(s)


void CDynPrecipitator::Agglomeration() {
  for (int i=0; i<nClasses-1; i++) {
    // Special case when j=i
    // ie agglomeration of particles of equal sizes.
    int k = i+1;
    double NN = beta[i][i]*Sqr(n[i])*dAgglRateFactor*dAgglomRate;
    Ndot[i] -= NN;
    Ndot[k] += bsplit[i][i]*NN;
    // And the rest
    for (int j=0; j<i; j++) {
      k = i;  // Destination bin
      if (k+1 < nClasses) {
	NN = beta[i][j]*n[i]*n[j]*dAgglRateFactor*dAgglomRate;
	Ndot[i] -= NN;
	Ndot[j] -= NN;
	Ndot[k] += bsplit[i][j]*NN;
	Ndot[k+1] += (1-bsplit[i][j])*NN;
	//dbg.log("i,j %3d, %3d, NN = %12.5g", i, j, NN);	
      }
    }
  }
}


// Numbers for Growth

void CDynPrecipitator::Growth()
{
  for (int i = 1; i<nClasses-1; i++) {
    double g = n[i]*AREA_CORR/DIST_CORR*dGrowthRate/D[i]*1.0e6*dGrowthRateFactor;
    Ndot[i] -= g;
    Ndot[i+1] += g;
  }
}

void CDynPrecipitator::Nucleation()
{
  Ndot[1] += dNuclRate*dNuclRateFactor;
  dNuclYield = PI/6.*pow(D[1],3)*2420*dNuclRate*dNuclRateFactor*1.0e-18/3600;
}

void CDynPrecipitator::Attrition() {}
void CDynPrecipitator::AttritionRate() {}


/// Calculate all the particle balance rates...
/// Ndot[i] ... rate of change of n[i], #/s/kg

void CDynPrecipitator::PBRates() 
{
  for (int i=0; i<nClasses; i++) Ndot[i] = (Feed.Mass()*fpsd.getN(i) - Tank.Mass()*n[i])/(Tank.Density()*dTankVol);

  if (bAgglomOn) {
    AgglomRate();
    Agglomeration();
  }
  if (bNuclOn) {
    NucleationRate();
    Nucleation();
  }
  if (bGrowthOn) {
    GrowthRate();
    Growth();
  }
  if(bAttritionOn) {
    AttritionRate();
    Attrition();
  }

  // Save the values for display
  for (int i=0; i<nClasses; i++) {
    
    switch (iDType) {
    case 0:
      dd[6][i]=0;
      break;
    case 1:
      dd[6][i]=0;
      break;
    case 2:
      dd[6][i]=Ndot[i];
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






void CDynPrecipitator::getN(MStream &s)
{  
  QALPSD p(s);
  for (int i=0; i<nClasses; i++) n[i] = p.getN(i);
  m_dArea = p.getArea();
  m_dVol = p.getVol();
  m_dNumber = p.getNTot();
  
}          // Fetch all the numbers to local N, return total


void CDynPrecipitator::putN(MStream &s)
{
  MIPSD & sP = s.IF<MIPSD>(false);
  
  double solidsContent = s.MassVector[spTHA]/GTZ(s.Mass());
  if (fabs(solidsContent) < 1.0e-10) return;

  double avvol = PI/6.*D[0]*D[0]*D[0]*.125*1.0e-18;
  double z = n[0]*avvol*2420/solidsContent;
  sP.putFrac(0, z);

  for (int i=1; i<nClasses; i++) {
    avvol = PI/6.*pow(D[i],3)*DIST_CORR*1.0e-18;
    double z = n[i]*avvol*2420/solidsContent;
    sP.putFrac(i, z);
  }
      
}         // Put local N to stream







void CDynPrecipitator::BuildDataFields()
{
  static MDDValueLst DDB1[]={
    {GRM_Fixed,   "Fixed" },
    {GRM_White,   "White"},
    {GRM_CAR_QAL, "CAR-QAL"},
    {0}};

  static MDDValueLst DDB2[]={
    {0,   "Fraction" },
    {1,   "Number"},
    {2,   "F*"},
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

  static MDDValueLst DDB14[]={
    {0,   "Growth" },
    {1,   "Aggl (N/s/m^3)"},
    {2,   "deltaN"},
    {3,   "oldN"},
    {4,   "newN"},
    {0}};


  DD.Text  ("");

#ifdef TTDEBUG
  DD.CheckBox("TTDBG", "",  &bTTDebug, MF_PARAMETER);
#endif


  DD.CheckBox("On", "",  &bOnLine, MF_PARAMETER|MF_SET_ON_CHANGE);
  //  DD.CheckBox("Int.Cooling", "", &m_bInternalCool, MF_PARAMETER|MF_SET_ON_CHANGE);
  //DD.CheckBox("Ext.Cooling", "", &m_bExternalCool, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Text("");
  DD.CheckBox("Growth.On", "",  &bGrowthOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Agglom.On", "",  &bAgglomOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Nucleation.On", "",  &bNuclOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Attrition.On", "",  &bAttritionOn, MF_PARAMETER|MF_SET_ON_CHANGE);


  DD.Long  ("Cooling", "", &iCoolType, MF_PARAMETER|MF_SET_ON_CHANGE, DDB7);

  //m_RB.OnOffCheckBox();

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
  DD.Double("SolidsIn",  "",    &m_dSolidsIn, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("SolidsOut",  "",    &m_dSolidsOut, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("NucleationYield", "", &dNuclYield, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("GrowthYield", "", &dGrowthYield, MF_RESULT, MC_Qm("kg/s"));

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
  DD.Double("Acceleration", "", &m_ddeltaT, MF_PARAMETER|MF_INIT_HIDDEN, MC_);
  DD.Double("Thermal.Damping", "",   &m_dDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  DD.Double("Mass.Damping", "",   &m_dMassDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  DD.Long("Iterations", "", &m_lIterations, MF_RESULT|MF_NO_FILING);
  DD.Long("IterMax", "", &m_lItermax, MF_PARAMETER);

  // DD.Double("Vol.Damping", "",   &m_dVolDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  
  DD.Show (iGrowthRateMethod==GRM_Fixed);
  DD.Double("Precip.Rate", "",      &dGrowthRate    ,MF_PARAMETER, MC_Ldt("um/h"));
  DD.Show (iGrowthRateMethod==GRM_White);
  DD.Double("ER_White", "",         &ER_White        ,MF_PARAMETER, MC_T("K"));
  DD.Double("K_White", "",	    &K_White         ,MF_PARAMETER, MC_);
  DD.Double("gF_White", "",    	    &gF_White        ,MF_PARAMETER, MC_);


  DD.Show (iGrowthRateMethod==GRM_CAR_QAL);
  DD.Double("ActivationEnergy", "",    	    &m_dActivationEnergy    ,MF_PARAMETER, MC_T("K"));
  DD.Double("K0", "",    	    &m_dK0   		    ,MF_PARAMETER, MC_);
  //DD.Double("K1", "",    	    &m_dK1   		    ,MF_PARAMETER, MC_);
  DD.Double("k_TOC", "",    	    &m_dk_TOC		    ,MF_PARAMETER, MC_);
  DD.Double("n_s", "",    	    &m_dn_s  		    ,MF_PARAMETER, MC_);
  DD.Double("n_fc", "",    	    &m_dn_fc 		    ,MF_PARAMETER, MC_);
  DD.Double("n_eq", "",    	    &m_dn_eq 		    ,MF_PARAMETER, MC_);
  DD.Double("n_", "",    	    &m_dn_   		    ,MF_PARAMETER, MC_);
  DD.Double("n_sol", "",    	    &m_dn_sol		    ,MF_PARAMETER, MC_);
  DD.Double("n_ssa", "",    	    &m_dn_ssa		    ,MF_PARAMETER, MC_);

  
  DD.Show();
  DD.Text("Fudge Factors");
  DD.Double("Growth.Rate.Factor", "", &dGrowthRateFactor,   MF_PARAMETER, MC_);
  DD.Double("Agglom.Rate.Factor", "", &dAgglRateFactor, MF_PARAMETER, MC_);
  DD.Double("Nucl.Rate.Factor", "", &dNuclRateFactor, MF_PARAMETER, MC_);

  DD.Show();
  DD.Text("Results");
  DD.Double("Growth.Rate", "",       &dGrowthRate          ,MF_RESULT|MF_NO_FILING, MC_Ldt("um/h"));
  DD.Double("Agglom.Rate", "",       &dAgglomRate          ,MF_RESULT|MF_NO_FILING, MC_);
  DD.Double("Nucl.Rate", "",       &dNuclRate              ,MF_RESULT|MF_NO_FILING, MC_);

  DD.Double("SSAin", "",    	    &m_dSSAin		    ,MF_RESULT, MC_);//SurfAreaM);
  DD.Double("SSA", "",    	    &m_dSSA		    ,MF_RESULT, MC_);//SurfAreaM);
 

  DD.Text("Thermal and Mass Balance");
  DD.Double("Mass.Flow.In", "",       &dQmin          ,MF_RESULT|MF_NO_FILING, MC_Qm("t/d"));
  DD.Double("Mass.Flow.Out", "",      &dQmout         ,MF_RESULT|MF_NO_FILING, MC_Qm("t/d"));
  DD.Show(iEvapMethod);
  DD.Double("Evap.Mass.Loss", "", &m_dEvapRate, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("Evap.Thermal.Loss", "",   &m_dEvapThermalLoss  ,MF_RESULT, MC_Pwr("kW"));
  DD.Show(iThermalLossMethod==THL_Ambient);
  DD.Double("Env.Thermal.Loss", "", &m_dEnvironmentLoss, MF_RESULT, MC_Pwr("kW"));  
  DD.Show(iCoolType);
  DD.Double("Cooler.Thermal.Loss", "", &m_dCoolRate, MF_RESULT, MC_Pwr("kW")); // Ext Cooling Rate
  DD.Double("ReactionHeat", "",     &dReactionHeat  ,MF_RESULT, MC_Pwr("kW"));
  DD.Double("Total.Thermal.Loss", "", &m_dTotThermalLoss, MF_RESULT, MC_Pwr("kW"));
  DD.Text("Stream.Enthalpy");
  DD.Double("HzIn", "", &m_dHIn, MF_RESULT, MC_Pwr);
  DD.Double("HzEvap", "", &m_dHEvap, MF_RESULT, MC_Pwr);
  DD.Double("HzOut", "", &m_dHOut, MF_RESULT, MC_Pwr);
  DD.Double("HzBal", "", &m_dHBal, MF_RESULT, MC_Pwr);


  DD.Show();
  DD.Page("Size Inlet");

  DD.Long("PSD.Display", "", &iPSD, MF_PARAMETER|MF_SET_ON_CHANGE, DDB2);
  const int NumSpecies = gs_MVDefn.Count();
  for (int i=0; i<nClasses; i++) { 
    std::stringstream os;
    os << "Size " << std::setw(3) << i << std::setw(12) << D[i] ;
    DD.Double(os.str().c_str(), "", dd[2]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  

//   DD.Page("Size In");
//   for (int i=0; i<nClasses; i++) {
//     std::stringstream os;
//     os << "Size " << std::setw(3) << i;
//     DD.Double(os.str().c_str(), "", dd[4]+i, MF_RESULT|MF_NO_FILING, MC_None);
//   }
//   DD.Text("");
//   DD.Double("Number", "", dd[4]+26, MF_RESULT|MF_NO_FILING, MC_None);
//   DD.Double("Area", "", dd[4]+27, MF_RESULT|MF_NO_FILING, MC_None);
//   DD.Double("Vol", "", dd[4]+28, MF_RESULT|MF_NO_FILING, MC_None);
//   DD.Double("Mass", "", dd[4]+29, MF_RESULT|MF_NO_FILING, MC_None);

  DD.Page("Size Tank");
  for (int i=0; i<nClasses; i++) {
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

  for (int i=0; i<nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[6]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }



  DD.Show(iCoolType);
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
  DD.Show(iCoolType);
  DD.Double("Cooling.Rate", "", &m_dCoolRate, MF_RESULT, MC_Pwr("kW")); // Ext Cooling Rate





  if (!m_bEvapConnected && iEvapMethod!=EVAP_NONE) //if optional Evap is NOT connected and evaporation functionality is required show the output stream
    DD.Object(Evap, MDD_RqdPage);

  //DD.Show(m_RB.Enabled());  
  //DD.Page("RB");
  //m_RB.BuildDataFields();
  //DD.Show();
}
