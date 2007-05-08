//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//           QAL Classifier Model 2004 - Transcritical Technologies/ QAL 
//   Time-stamp: <2007-05-08 06:14:15 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __QALPRECIP_CPP
#include "qalprecip.h"



static MInitialiseTest InitTest("Precip");


//static MSpeciePtr  spAlumina       (InitTest, "NaAl[OH]4(l)", false);
//static MSpeciePtr  spTHA           (InitTest, "Al[OH]3(s)", false);

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
  SetTreeDescription("QAL:Precipitator");
  SetDescription("TODO: QAL PB Model");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina);
};

//---------------------------------------------------------------------------


double calcVol(double *N) {
  double v = 0.0;
  for (int i=0; i<nClasses; i++) {
    v += pow(D[i], 3)*N[i];
  }
  return v;
}



CPrecipitator::CPrecipitator(MUnitDefBase *pUnitDef, TaggedObject * pNd) :  
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

CPrecipitator::~CPrecipitator()
{
//   delete[] x;
//   delete[] xo;


}


bool CPrecipitator::ValidateDataFields()
  {//ensure parameters are within expected ranges
  return true;
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
  CausticMass = InCausticMass  + x[iALUMINA]*(Alpha-1) - x[iOCC_SODA];
  OrganicMass = InOrganicMass - x[iBOUND_ORGANICS];
  BoundOrganicMass = InBoundOrganicMass + x[iBOUND_ORGANICS];
  BoundSodaMass = InBoundSodaMass + x[iOCC_SODA];
  WaterMass = InWaterMass - x[3];   // Water is only lost through evaporation
  //  if (m_bUpdateT) Prod.MarkStateChanged();    // Manual fixup of heat loads eventually...
}



  


void CPrecipitator::AdjustT(MStream &Prod) 
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

  
  



void CPrecipitator::AdjustSSA(MStream &Prod) 
{
}






//---------------------------------------------------------------------------
// All the precipitation rate calcs are done in this routine 
// Takes the product composition and calculates rates from this
// Rates (in kg/s) are put into global x
//---------------------------------------------------------------------------
void CPrecipitator::EvalPrecipRates(MStream & Prod, double T) 
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

void CPrecipitator::AdjustPSD(MStream & Prod) 
{
  putN(Prod);  
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


void CPrecipitator::DoResults(MStream & Prod)
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






void CPrecipitator::EvalProducts()
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

    OFlow.SetF(Tank, MP_All, 0.0);
    UFlow.SetF(Tank, MP_All, 1.0);


    bool streamOK=true;
    



    
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




void CPrecipitator::DoPrecip(MStream & Prod)
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
    converged = ConvergenceTest();   //// Final convergence test  
    Iter++;    
  }
  if (Iter==m_lItermax) {/// Convergence Failure, automatically increase damping and iterations???
  }
  putN(Prod);
  
  m_lIterations = Iter;
  Log.SetCondition(Iter==m_lItermax, 4, MMsg_Warning, "Precip loop failed to converge....");
}



// Precalculate Agglomeration Kernel and bin split factors

void CPrecipitator::AgglomKernel() {
  for (int i=0; i<nClasses; i++) {
    for (int j=0; j<=i; j++) {
      beta[i][j] = beta[j][i] = exp(-(D[i]*D[i]+D[j]*D[j])/1600.);
      // dbg.log("i, j = %3d %3d %12.5g", i, j, beta[i][j]);
    }
  }
}
  
void CPrecipitator::BinFactors() {
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





void CPrecipitator::displayPSD(MStream &ms, int scrn)
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
double CPrecipitator::AgglomRate() {
  double ssat = m_dSSat/(MW_Na2O/(2*MW_NaOH));   // Caustic as Na2O
  dAgglomRate =  5.0e-10*pow(ssat, 4)/3600;
  return dAgglomRate;
}


// Just the White method ala CAR for the moment...
double CPrecipitator::GrowthRate() {
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

double CPrecipitator::NucleationRate() {
  if (iGrowthRateMethod==GRM_CAR_QAL) {
    MIBayer & TankB=Tank.IF<MIBayer>(false);
    const double T = Tank.T;
    const double SPO = Tank.MassVector[spOxalate]/Tank.MassVector[spAlumina];
    const double ssat = TankB.AluminaConc(T)/TankB.AluminaConcSat(T);
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


void CPrecipitator::Agglomeration() {
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

void CPrecipitator::Growth()
{
  for (int i = 1; i<nClasses-1; i++) {
    double g = n[i]*AREA_CORR/DIST_CORR*dGrowthRate/D[i]*1.0e6*dGrowthRateFactor;
    Ndot[i] -= g;
    Ndot[i+1] += g;
  }
}

void CPrecipitator::Nucleation()
{
  Ndot[1] += dNuclRate*dNuclRateFactor;
  dNuclYield = PI/6.*pow(D[1],3)*2420*dNuclRate*dNuclRateFactor*1.0e-18/3600;
}

void CPrecipitator::Attrition() {}
void CPrecipitator::AttritionRate() {}


/// Calculate all the particle balance rates...
/// Ndot[i] ... rate of change of n[i], #/s/kg

void CPrecipitator::PBRates() 
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
  for (int i=1; i<=nClasses; i++) {
    
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




// Get the volume in bin i
double QALPSD::getVol(int i) 
{
  return 0.;

}


double QALPSD::getMass()
{
  return 0.;  
}
double QALPSD::getMass(int i)
{  
return 0.;
}
double QALPSD::getFrac(int i)
{
  double D2 = Sqr(D[i]);
  double D3 = D2*D[i];
  double avvol = PI/6.*D3*DIST_CORR*1.0e-18; 
  return n[i]*avvol/m_dVol;
}


QALPSD::QALPSD(MStream &s) 
{  

  MIPSD & sP = s.IF<MIPSD>(false);
  double &THAMass      = s.MassVector[spTHA];         // Al[OH]3(s)
  

  double solidsContent = THAMass/s.Mass();// kg PSD solids/kg slurry @ref
  
  //dbg.log("solids content %12.7f", solidsContent);

double sumar = 0.0, sumv=0.0, sumn=0.0;
  for (int i = 0; i<nClasses; i++) {
    double D = sP.getSize(i)*1.0e6;
    double D2 = Sqr(D/*[i]*/);
    double D3 = D2*D/*[i]*/;
    double avvol = PI/6.*D3*DIST_CORR*1.0e-18;
    if (i==0) avvol = PI/6.*D3*.125*1.0e-18;
    n[i] = solidsContent*sP.getFrac(i)/avvol/2420;
    sumn += n[i];
    sumar += D2*n[i];
    sumv += D3*n[i];
  }
  m_dArea = PI*1.0e-12*AREA_CORR*sumar;
  m_dVol = PI/6*1.0e-18*DIST_CORR*sumv;
  m_dNumber = sumn;
  

}          // Fetch all the numbers to local N, return total



void CPrecipitator::getN(MStream &s)
{  
  QALPSD p(s);
  for (int i=0; i<nClasses; i++) n[i] = p.getN(i);
  m_dArea = p.getArea();
  m_dVol = p.getVol();
  m_dNumber = p.getNTot();
  
}          // Fetch all the numbers to local N, return total


void CPrecipitator::putN(MStream &s)
{
  MIPSD & sP = s.IF<MIPSD>(false);

  for (int i=0; i<nClasses; i++) {
    double avvol = PI/6.*pow(D[i],3)*DIST_CORR*1.0e-18;
    double solidsContent = s.MassVector[spTHA]/s.Mass();
    double z = n[i]*avvol*2420/solidsContent;

    
    sP.putFrac(i, z);
  }
      
}         // Put local N to stream







#include "qaldata.cpp"
