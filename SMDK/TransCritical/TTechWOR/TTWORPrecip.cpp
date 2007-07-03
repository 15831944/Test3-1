//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-09-28 14:35:50 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================

#include "stdafx.h"
#include "TTWORPrecip.h"

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


double Drw_CPrecipitator[] = 
  { 
    MDrw_Poly, -5,10, -5,-10, 5,-10, 5,10,
    MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CPrecipitator, "Precipitator", DLL_GroupName)

void CPrecipitator_UnitDef::GetOptions()
{
  SetDefaultTag("PC");
  SetDrawing("Tank", Drw_CPrecipitator);
  SetTreeDescription("Worsley:Precipitator");
  SetDescription("TODO: Worsley Precip Model");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina);
};

//---------------------------------------------------------------------------



long   CPrecipitator::m_lGrowthTerm = GRM_sigma;
long   CPrecipitator::m_lNuclTerm = GRM_AAEq;
long   CPrecipitator::m_lSodaTerm = 0;
double   CPrecipitator::m_dActEGrowth = -12260.;
double   CPrecipitator::m_dActENucleation = 8344.   ;
double   CPrecipitator::m_dActESoda = 2535.;
double   CPrecipitator::m_dK_G = 2.18e15  	 ;
double   CPrecipitator::m_dK_N =	2.211e-6 ;
double   CPrecipitator::m_dn_s = 1.0;
double   CPrecipitator::m_dgamma_2 = 1.00;
double   CPrecipitator::m_dgamma_g = 2.0;
double   CPrecipitator::m_dgamma_N = 2.0;
double   CPrecipitator::m_dgamma_s = 2.00;



#include "wordata.cpp"






CPrecipitator::CPrecipitator(MUnitDefBase *pUnitDef, TaggedObject * pNd) :  
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

CPrecipitator::~CPrecipitator()
{
//   delete[] x;
//   delete[] xo;


}

//---------------------------------------------------------------------------

void CPrecipitator::Init()
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
void CPrecipitator::AdjustMasses()
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

  


void CPrecipitator::AdjustT() 
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

  
  



void CPrecipitator::AdjustSSA() 
{
}




//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// All the precipitation rate calcs are done in this routine 
// Takes the tank composition and calculates rates from this
// Rates (in kg/s) are put into global x
//---------------------------------------------------------------------------
void CPrecipitator::EvalPrecipRates(MStream & Tank) 
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

void CPrecipitator::EvalLosses(MStream &Tank)
{
  
}

void CPrecipitator::AdjustPSD() 
{
  m_PSD1 = BrahmaPSD(NewN);
  m_PSD1.put(Tank);  
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


void CPrecipitator::DoClassifier(MStream & OFlow, MStream &UFlow) 
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


void CPrecipitator::DoResults()
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
  double ACTank = TankB.AtoC();
  m_dSSat = ACTank/m_dACeq;
  m_dBSSat = (ACTank - m_dACeq)/(m_dACeq*(1-1.039*ACTank));
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






void CPrecipitator::EvalProducts()
{
  try {
    x[0] = x[1] = x[2] = x[3] = 0.0;
    FlwIOs.AddMixtureIn_Id(Feed, idFeed);
    MStream & UFlow = FlwIOs[FlwIOs.First[idUflow]].Stream; //Reference to the output stream
    MStream & OFlow = FlwIOs[FlwIOs.First[idOflow]].Stream; //Reference to the output stream
    MStreamI OutFlow;
    
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
      if (bClassOn)   // Online and have Bayer and SSA properties...
	DoClassifier(OFlow, UFlow);

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




void CPrecipitator::ClosureInfo(MClosureInfo & CI)
  {//ensure heat balance
  if (CI.DoFlows())
    {
    CI.AddPowerIn(0, -dThermalLoss);
    }
  }




void CPrecipitator::DoPrecip()
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







void CPrecipitator::displayPSD(MStream &ms, int scrn)
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

void CPrecipitator::displayPSD(BrahmaPSD &p, int scrn) {
  for (int i=0; i<=nClasses; i++) {
    dd[scrn][i]=p.getN(i);
  }
  dd[scrn][26] = p.getN();
  dd[scrn][27] = p.getArea();
  dd[scrn][28] = p.getVol();
  dd[scrn][29]=0.0;
  
}

double CEquil(MStream & s, double T) {
  MStream s1 = s;

  MIBayer & sB=s.IF<MIBayer>(false);

  double asat = sB.AluminaConcSat(T);
  return asat;
}

#include "worcalc.cpp"





