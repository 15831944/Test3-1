//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "smdk_precip.h"

//====================================================================================
//#pragma optimize("", off)

static MInitialiseTest InitTest("SMDK_Precip");

static MSpeciePtr   spAlumina          (InitTest, "Al2O3(l)", false);
static MSpeciePtr   spWater            (InitTest, "H2O(l)", false);
static MSpeciePtr   spTHA              (InitTest, "Al2O3.3H2O(s)", false);
static MSpeciePtr   spCausticSoda      (InitTest, "NaOH(l)", false);
static MSpeciePtr   spOccSoda          (InitTest, "Na2O(s)", false);
static MSpeciePtr   spSodiumCarbonate  (InitTest, "Na2CO3(l)", false);
//static MSpeciePtr   spSodiumOxalate    (InitTest, "Na2C2O4(l)", false);

enum GrowthRateMethods { GRM_White, GRM_Cresswell, GRM_WhiteBateman };
enum SodaPrecipMethods { SPM_Sang, SPM_Ohkawa };
enum ThermalHeatLossMethods { THL_None, THL_TempDrop, THL_FixedHeatFlow, THL_HeatFlowFrac };
enum ReactionHeatMethods { RHM_Calc, RHM_Override1 };

//====================================================================================

const long idFeed  = 0;
const long idProd  = 1;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;        PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",          "Feed",      idFeed,    1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Product",       "Prod",      idProd,    1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

static double Drw_CPrecipitator[] = 
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
  SetTreeDescription("Demo:Precipitator");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina);
  };

//---------------------------------------------------------------------------

CPrecipitator::CPrecipitator(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  //default values...
  bOnLine = 1;
  dTempDropRqd = 0.5;
  dThermalLossRqd = 1500.0;
  dThermalLossFrac = 0.0002;
  iGrowthRateMethod  = GRM_Cresswell;
  iSodaPrecipMethod  = SPM_Ohkawa;
  iThermalLossMethod = THL_TempDrop;
  iRctHeatMethod     = RHM_Calc;
  dHOR_a             = 400.0;
  dHOR_b             = 1.0;
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
  m_AOutEst     = dNAN;
  m_TOutEst     = dNAN;
  m_xPrev       = dNAN;
  m_XSodaPrev   = dNAN;

  dTankVol       = 1000.0;

  dSolPrecip  = 0.0;
  dOccSoda    = 0.0;
  dGrowthRate = 0.0;
  dGrowthRateFactor = 0.0;
  dYield      = 0.0;
  dTin        = C2K(25.0);
  dTout       = C2K(25.0);
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
  dReactionHeat  = 0.0;
  }

//---------------------------------------------------------------------------

CPrecipitator::~CPrecipitator()
  {
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
    {GRM_White,    "White"},
    {GRM_Cresswell,"Cresswell"},
    {GRM_WhiteBateman,"WhiteBateman"},
    {0}};
  static MDDValueLst DDB2[]={
    {SPM_Sang,  "Sang"},
    {SPM_Ohkawa,"Ohkawa"},
    {0}};
  static MDDValueLst DDB3[]={
    {THL_None,          "None" },
    {THL_TempDrop,      "TempDrop"},
    {THL_FixedHeatFlow, "FixedLoss"},
    {THL_HeatFlowFrac,  "LossFraction"},
    {0}};
  static MDDValueLst DDB4[]={
    {RHM_Calc,          "Calculated" },
    {RHM_Override1,     "Override1"},
    {0}};

  DD.Text  ("");
  DD.CheckBox("On", "",  &bOnLine, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Text("");
  DD.Text  ("Requirements");
  DD.Double("TankVol", "",	        &dTankVol         ,MF_PARAMETER, MC_Vol("m^3"));
  DD.Long  ("GrowthMethod", "", (long*)&iGrowthRateMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB1);
  DD.Show(iGrowthRateMethod==GRM_White);
  DD.Double("ER_White", "",         &ER_White        ,MF_PARAMETER, MC_T("K"));
  DD.Double("K_White", "",	        &K_White         ,MF_PARAMETER, MC_);
  DD.Double("gF_White", "",    	    &gF_White        ,MF_PARAMETER, MC_);
  DD.Show(iGrowthRateMethod==GRM_Cresswell);
  DD.Double("ER_Cresswell", "",     &ER_Cresswell    ,MF_PARAMETER, MC_T("K"));
  DD.Double("Tref", "",             &Tref            ,MF_PARAMETER|MF_INIT_HIDDEN, MC_T("K"));
  DD.Double("K_Cresswell", "",	    &K_Cresswell     ,MF_PARAMETER, MC_);
  DD.Double("gF_Cresswell", "",	    &gF_Cresswell    ,MF_PARAMETER, MC_);
  DD.Show(iGrowthRateMethod==GRM_WhiteBateman);
  DD.Double("ER_WhiteBateman", "",  &ER_WhBateman    ,MF_PARAMETER, MC_T("K"));
  DD.Double("K_WhiteBateman", "",   &K_WhBateman     ,MF_PARAMETER, MC_);
  DD.Double("gF_WhiteBateman", "",  &gF_WhBateman    ,MF_PARAMETER, MC_);
  DD.Show();
  DD.Long  ("SodaPrecipMethod", "", &iSodaPrecipMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB2);
  DD.Show(iSodaPrecipMethod==SPM_Sang);
  DD.Double("Ksoda_Sang", "",       &KSoda_Sang      ,MF_PARAMETER, MC_);
  DD.Show(iSodaPrecipMethod==SPM_Ohkawa);
  DD.Double("Ksoda_Ohkawa", "",     &KSoda_Ohkawa    ,MF_PARAMETER, MC_);
  DD.Show();
  DD.Long  ("ReactionHeatMethod", "",&iRctHeatMethod ,MF_PARAMETER|MF_SET_ON_CHANGE, DDB4);
  DD.Show(iRctHeatMethod==RHM_Override1);
  DD.Double("HOR_a", "",            &dHOR_a          ,MF_PARAMETER, MC_);
  DD.Double("HOR_b", "",            &dHOR_b          ,MF_PARAMETER, MC_);
  DD.Show();
  DD.Long  ("ThermalLossMethod", "",&iThermalLossMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB3);
  DD.Show(iThermalLossMethod==THL_TempDrop);
	DD.Double("Temp_Drop", "",        &dTempDropRqd     ,MF_PARAMETER, MC_dT("C"));
  DD.Show(iThermalLossMethod==THL_FixedHeatFlow);
  DD.Double("ThermalLossRqd", "",   &dThermalLossRqd  ,MF_PARAMETER, MC_Pwr("kW"));
  DD.Show(iThermalLossMethod==THL_HeatFlowFrac);
  DD.Double("ThermalLossFrac", "",  &dThermalLossFrac ,MF_PARAMETER, MC_Frac("%"));
  DD.Show();

  DD.Text  ("");
  DD.Text  ("Results Tank");
  DD.Double("ResidenceTime", "",    &dResidenceTime ,MF_RESULT, MC_Time("h"));
  DD.Double("Yield", "",            &dYield         ,MF_RESULT, MC_Conc("kg/m^3"));
	DD.Double("THAPrecip", "",        &dSolPrecip     ,MF_RESULT, MC_Qm("kg/s"));
  DD.Double("OccludedSoda", "",     &dOccSoda       ,MF_RESULT, MC_Qm("kg/s"));
  DD.Text  ("Results");         
  DD.Show();                    
  DD.Double("ASat", "",             &dASat          ,MF_RESULT, MC_Conc("g/L"));
  DD.Double("GrowthRateFactor", "", &dGrowthRateFactor,MF_RESULT, MC_Ldt("m/s"));
  DD.Double("GrowthRate", "",       &dGrowthRate    ,MF_RESULT, MC_);
  DD.Show();                    
	DD.Double("ACin", "",             &dACin          ,MF_RESULT, MC_);
	DD.Double("ACout", "",            &dACout         ,MF_RESULT, MC_);
	DD.Double("TempIn", "",           &dTin           ,MF_RESULT, MC_T("C"));
	DD.Double("TempOut", "",          &dTout          ,MF_RESULT, MC_T("C"));
  DD.Double("ReactionHeat", "",     &dReactionHeat  ,MF_RESULT|MF_INIT_HIDDEN, MC_Pwr("kW"));
  DD.Double("ThermalLoss", "",      &dThermalLoss   ,MF_RESULT, MC_Pwr("kW"));
  DD.Double("ThermalTempDrop", "",  &dThermTempDrop ,MF_RESULT, MC_dT("C"));
	DD.Double("PartDiamIn", "",       &dDiamin        ,MF_RESULT, MC_L("um"));
	DD.Double("PartDiamOut", "",      &dDiamout       ,MF_RESULT, MC_L("um"));
  DD.Double("SeedSurfAreaLIn", "",  &dSALin         ,MF_RESULT|MF_INIT_HIDDEN, MC_SurfAreaL("m^2/L"));
  DD.Double("SeedSurfAreaLOut", "", &dSALout        ,MF_RESULT|MF_INIT_HIDDEN, MC_SurfAreaL("m^2/L"));
	DD.Double("Vol_FlowIn", "",       &dQvin          ,MF_RESULT, MC_Qv("L/s"));
	DD.Double("Vol_FlowOut", "",      &dQvout         ,MF_RESULT, MC_Qv("L/s"));
  DD.Double("AOutEst", "",          &m_AOutEst      ,MF_PARAMETER|MF_NO_VIEW, MC_);
  DD.Text("");
  }

//---------------------------------------------------------------------------

bool CPrecipitator::ValidateDataFields()
  {//ensure parameters are within expected ranges
  Tref = Range(C2K(20.0), Tref, C2K(110.0));
  return true;
  }

//---------------------------------------------------------------------------

void CPrecipitator::AdjustMasses(MVector & Prod, double & x, double SodaFac)
  {
  double &AluminaMass  = Prod.MassVector[spAlumina];     // Al2O3
  double &WaterMass    = Prod.MassVector[spWater];       // H2O
  double &THAMass      = Prod.MassVector[spTHA];         // Al2O3.3H2O
  double &CausticMass  = Prod.MassVector[spCausticSoda]; // NaOH
  double &Na2OMass     = Prod.MassVector[spOccSoda];     // Na2O

  const double MW_H2O       = spWater.MW;            //18.0152800
  const double MW_Na2O      = spOccSoda.MW;          //61.9789360
  const double MW_NaOH      = spCausticSoda.MW;      //39.9971080
  const double MW_Al2O3     = spAlumina.MW;          //101.961278
  const double MW_Al2O3_3H2O= spTHA.MW;              //156.007118

  const double FacAl = MW_Al2O3/MW_Al2O3_3H2O; //Alpha = 0.6538; // Ratio of molecular weights (Al2O3/Al2O3.3H2O)

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
  AluminaMass = AluminaMass - FacAl*x;
  THAMass     = THAMass     + x;
  WaterMass   = WaterMass   - (1.0-FacAl)*x + MW_H2O/MW_Na2O*XSoda;
  CausticMass = CausticMass - (MW_NaOH*2)/MW_Na2O*XSoda;
  Na2OMass    = Na2OMass    + XSoda;
  }

//---------------------------------------------------------------------------

double CPrecipitator::PerformAluminaSolubility(MVector & Prod, double TRqd, double ARqd, double SSat, double Cin, bool & ConvergeErr)
  {
  //   2 AlO2 + 4 H2O <==> Al2O3.3H2O + 2 OH
  //or Na2O + Al2O3 + 4 H2O <==> Al2O3.3H2O + 2 NaOH

  // x is the Fraction of Alumina which precipitates as the hydrate
  // ie x is deposition rate of new THA crystal

  bool AdjustT=!Valid(TRqd);
  double T = AdjustT ? Prod.T : TRqd;

  MIBayer & ProdB=*Prod.FindIF<MIBayer>();
  double A = ProdB.AluminaConc(T);

  double &AluminaMass  = Prod.MassVector[spAlumina];     // Al2O3

  const double MW_Al2O3     = spAlumina.MW;          //101.961278
  const double MW_Al2O3_3H2O= spTHA.MW;              //156.007118
  const double FacAl = MW_Al2O3/MW_Al2O3_3H2O; //Alpha = 0.6538; // Ratio of molecular weights (Al2O3/Al2O3.3H2O)

  const double ESoda = 2535.0; //constant 2535K
  const double KSodaFac = (iSodaPrecipMethod==SPM_Sang ? KSoda_Sang : KSoda_Ohkawa);

  for (int Iter=100; Iter; Iter--)
    {
    double x = AluminaMass*(1.0-ARqd/GTZ(A))/FacAl;

    //Soda Precipitation...
    double SodaFac;
    if (Valid(Cin))
      {//Sang
      const double xx = SSat*Cin;
      SodaFac = KSodaFac*xx*xx;
      }
    else
      {//Ohkawa, Tsuneizumi and Hirao
      SodaFac = KSodaFac*exp(ESoda/T)*SSat*SSat;
      }

    AdjustMasses(Prod, x, SodaFac);

    if (iRctHeatMethod==RHM_Override1)
      {
      Prod.MarkStateChanged(); //this forces recalculation of temperature / enthalpy based on new stream makeup
      Prod.SetTP(T, Prod.P);
      }
    else
      {
      Prod.MarkStateChanged(); //this forces recalculation of temperature / enthalpy based on new stream makeup
      if (AdjustT)
        T=Prod.T;
      }

    A = ProdB.AluminaConc(T);
    if (fabs(A-ARqd)<1.0e-12)
      break;
    if (fabs(x)<1e-22)
      {//problem!!!
      Iter=0;
      break;
      }
    }
  ConvergeErr = (Iter==0);

  return ProdB.AluminaConc(T);
  }

//---------------------------------------------------------------------------

void CPrecipitator::DoPrecip(MVector & Prod)
  {
  MIBayer & ProdB=*Prod.FindIF<MIBayer>();
  //MIPSD & ProdSz = *Prod.FindIF<MIPSD>();
  MISSA & ProdSSA = *Prod.FindIF<MISSA>();
  //Log.SetCondition(IsNothing(ProdSz), 2, MMsg_Error, "Bad Feed Stream - No Size Distribution");
  Log.SetCondition(IsNothing(ProdSSA), 3, MMsg_Error, "Bad Feed Stream - No SSA Data");
  double LiqIn   = Prod.Mass(MP_Liq); // kg/s
	double SolIn   = Prod.Mass(MP_Sol); // kg/s
  double THAIn   = Prod.MassVector[spTHA]; // kg/s
  double OcNaIn  = Prod.MassVector[spOccSoda]; // kg/s
  double Al2O3In = Prod.MassVector[spAlumina]; // kg/s
  double T       = Prod.T; // temperature (K)
  double P       = Prod.P;
  double Hfin    = Prod.totHf(MP_All, T, P);
  double CpIn    = Prod.totCp(MP_All, T, P);
  double Qvin    = Prod.Volume(MP_Liq)*3600.0; // Liquor volumetric flow rate @ T (m3/h)
  double Qvin25  = Prod.Volume(MP_Liq, C2K(25.0), P)*3600.0; // m3/h
  double Ain     = ProdB.AluminaConc(T); //Al2O3
  double Cin     = ProdB.CausticConc(T);
  double Sin     = ProdB.SodaConc(T);
  double ASat    = ProdB.AluminaConcSat(T); // Equilibrium Al2O3 liquor concentration @ T
  double Rhos    = ProdB.THADens(T);
  double SSat    = (Ain-ASat)/GTZ(Cin);
  double NoPerSec= IsNothing(ProdSSA) ? 0.0 : ProdSSA.PartNumPerSec();
  dTin    = T;
  dACin   = ProdB.AtoC();
  dQvin   = Prod.Volume();
  if (!IsNothing(ProdSSA))
    {
  	dSALin  = ProdSSA.SpecificSurfaceAreaVol(); // m^2/L
    dDiamin = ProdSSA.PartDiamFromSAM();
    }

  if (!IsNothing(ProdSSA) && SSat > 1.0e-6)
    {
    const double MW_H2O       = spWater.MW;            //18.0152800
    const double MW_Na2O      = spOccSoda.MW;          //61.9789360
    const double MW_Al2O3     = spAlumina.MW;          //101.961278
    const double MW_Al2O3_3H2O= spTHA.MW;              //156.007118
    const double MW_NaOH      = spCausticSoda.MW;      //39.9971080
    //const double MW_Na2C2O4   = spSodiumOxalate.MW;    //133.999136
    const double MW_Na2CO3    = spSodiumCarbonate.MW;  //105.989
    const double FacAl = MW_Al2O3/MW_Al2O3_3H2O; //Alpha = 0.6538; // Ratio of molecular weights (Al2O3/Al2O3.3H2O)
    const double AluminaMass_in  = Prod.MassVector[spAlumina]; // kg/s Al2O3
    const double Na2OMass_in     = Prod.MassVector[spOccSoda]; // kg/s Na2O

    //The growth and SodaFactor equations are using C, etc as Na2CO3 equivilant, 
    //shouldn't they be as Na2O equivilant!!!
    const double Alpha   = FacAl; //102./156.;
    double Cin_ = (iSodaPrecipMethod==SPM_Sang ? Cin*MW_Na2O/MW_Na2CO3 : dNAN);
    SSat = (Ain-ASat)/GTZ(Cin*MW_Na2O/MW_Na2CO3); //Na2O equiv

    bool bUsePrevAsEstimate = false;
    if (bUsePrevAsEstimate)
      {//THIS FAILS!!!
      if (!Valid(m_TOutEst))
        m_TOutEst = T;
      if (Valid(m_xPrev) && Valid(m_XSodaPrev))
        AdjustMasses(Prod, m_xPrev, m_XSodaPrev); //based on previous solution
  		if (!Valid(m_AOutEst) || m_AOutEst<1e-6)
	  		m_AOutEst = 0.98*Ain;
      }
    else
      {
      m_TOutEst = T;
      m_AOutEst = ((ASat+Ain)/2.0)*1.01;
      }

    bool ConvergeErr;
  	m_AOutEst = PerformAluminaSolubility(Prod, m_TOutEst, m_AOutEst, SSat, Cin_, ConvergeErr);
    if (NoPerSec>0.0)
      {
      ProdSSA.SetSAMFromFlow(ProdB.THAMassFlow(), NoPerSec);
      }

		//----------------------------------------
    double V = dTankVol*1000.0; // Liters
    double Qvout25, Cout, AoutPrev, Afact;
    int ZeroCnt=0;
		for (int Iter=100; Iter; Iter--)
			{
			T        = Prod.T; // K
			Cout     = ProdB.SodaConc(T)*MW_Na2O/MW_Na2CO3; //Na2O equiv
			AoutPrev = m_AOutEst;
			Qvout25  = (Alpha*Rhos-Ain)/(Alpha*Rhos-m_AOutEst)*Qvin25;
      Afact    = Sqr((m_AOutEst-ASat)/GTZ(Cout));
      if (iGrowthRateMethod==GRM_White)
        {
        double x = Exps(-ER_White/T);
        dGrowthRateFactor = gF_White*K_White*x;
        }
      else if (iGrowthRateMethod==GRM_Cresswell)
        {
        double C25 = ProdB.CausticConc(C2K(25.0)); // Caustic concentration @ 25C (gNa2CO3/l)
		    double m_C25 = C25*MW_Na2O/MW_Na2CO3; // Caustic concentration @ 25C (gNa2O/l)
		    double x = Exps(-ER_Cresswell*((1.0/T)-(1.0/Tref)));
        dGrowthRateFactor = gF_Cresswell*K_Cresswell*x/sqrt(m_C25/100.0);
        }
      else if (iGrowthRateMethod==GRM_WhiteBateman)
        {
        double C25 = ProdB.CausticConc(C2K(25.0)); // Caustic concentration @ 25C (gNa2CO3/l)
		    double m_C25 = C25*MW_Na2O/MW_Na2CO3; // Caustic concentration @ 25C (gNa2O/l)
		    double x = Exps(-ER_WhBateman/T);
        dGrowthRateFactor = gF_WhBateman*K_WhBateman*x/sqrt(m_C25);
        }
      dGrowthRate = dGrowthRateFactor*Afact;
			m_AOutEst = (Qvin25*Ain - dGrowthRate*dSALin/1000.0*V)/GTZ(Qvout25);
			m_AOutEst = GTZ(0.5*(m_AOutEst+AoutPrev));
      //Should SSat be recalulated as (Aout-ASat)/GTZ(Cout) ? (or does Afact effectively do this?)
    	m_AOutEst = PerformAluminaSolubility(Prod, T, m_AOutEst, SSat, Cin_, ConvergeErr);
      if (NoPerSec>0.0)
        {
        ProdSSA.SetSAMFromFlow(ProdB.THAMassFlow(), NoPerSec);
        }

      if (iRctHeatMethod==RHM_Override1)
        {
        //NB: msCp * MassFlow = totCp
        double Cp = Prod.totCp(MP_All, T, P);
        double THAOut  = Prod.MassVector[spTHA];
        double SolPrecip = THAOut - THAIn; //kg/s
        double CalcHOR = dHOR_a + (dHOR_b * K2C(T)); //kJ/kg
        double enthIn = CpIn * K2C(dTin);
        double NewT = (SolPrecip*CalcHOR + enthIn) / Cp;
        Prod.SetTP(C2K(NewT), P);
        }

      if (fabs(m_AOutEst-AoutPrev)<1.0e-9*Ain)
        {
        m_TOutEst = T;
        const double AluminaMass_out  = Prod.MassVector[spAlumina];     // Al2O3
        m_xPrev = (AluminaMass_in - AluminaMass_out)/FacAl;
        const double Na2OMass_out     = Prod.MassVector[spOccSoda];     // Na2O
        m_XSodaPrev = (Na2OMass_out - Na2OMass_in);
				break;//found a solution!
        }

      if (m_AOutEst<1e-6)
        ZeroCnt++;
      if (ZeroCnt>3)
        Iter = 1; //problem...
			}
    Log.SetCondition(Iter==0, 4, MMsg_Warning, "A at outlet not converging");
    Log.SetCondition(m_AOutEst<1.0e-6, 5, MMsg_Warning, "A at outlet is zero");
    Log.SetCondition(ConvergeErr, 6, MMsg_Warning, "Cannot converge PrecipTHA Alumina Conc");
		}

  //----------------------------------------
  //calculate reaction heat...
  //double H0 = Bm.totHf(MP_All, dTin, P);
  double H0 = Prod.totHf(MP_All, T, P);
  dReactionHeat = Hfin - H0;

  // apply Thermal Losses after precipitation...
  double T1;
  double H1 = Prod.totHf(MP_All, Prod.T, P);
  switch (iThermalLossMethod)
    {
    case THL_None: 
      dThermalLoss = 0.0;
      dThermTempDrop = 0.0;
      break;
    case THL_TempDrop: 
      Prod.SetTP(Prod.T-dTempDropRqd, P);
      dThermalLoss = H1 - Prod.totHf(MP_All, Prod.T, P);
      dThermTempDrop = dTempDropRqd;
      break;
    case THL_FixedHeatFlow: 
      T1 = Prod.T;
      Prod.Set_totHf(H1-dThermalLossRqd);
      dThermalLoss = dThermalLossRqd;
      dThermTempDrop = T1-Prod.T;
      break;
    case THL_HeatFlowFrac: 
      T1 = Prod.T;
      dThermalLoss = -Hfin*dThermalLossFrac;
      Prod.Set_totHf(H1-dThermalLoss);
      dThermTempDrop = T1-Prod.T;
      break;
    }
  
  //results...
  dResidenceTime = dTankVol/GTZ(Prod.Volume(MP_SL));      // Hours
  dTout          = Prod.T;
  dSALout        = (IsNothing(ProdSSA) ? 0.0 : ProdSSA.SpecificSurfaceAreaVol());
  dDiamout       = (IsNothing(ProdSSA) ? 0.0 : ProdSSA.PartDiamFromSAM());
  dACout         = ProdB.AtoC();
  double Cout    = ProdB.CausticConc(Prod.T);
  dYield         = Cout*(dACin-dACout);
  dASat          = ProdB.AluminaConcSat(Prod.T);
  dQvout         = Prod.Volume();
	double SolOut  = Prod.Mass(MP_Sol);
  double THAOut  = Prod.MassVector[spTHA];
  double OcNaOut = Prod.MassVector[spOccSoda];
	dSolPrecip     = THAOut - THAIn;
  dOccSoda       = OcNaOut - OcNaIn;
  }

//---------------------------------------------------------------------------

void CPrecipitator::EvalProducts()
  {
  try
    {
    MStream Feed;
    FlwIOs.AddMixtureIn_Id(Feed, idFeed);
    MStream & Prod = FlwIOs[FlwIOs.First[idProd]].Stream; //get reference to the actual output stream

    Prod = Feed;

    dThermalLoss = 0.0;
    dYield  = 0.0;
    dSALin  = 0.0;
    dDiamin = 0.0;
    MIBayer & ProdB=*Prod.FindIF<MIBayer>();
    bool IsOff = true;
    if (bOnLine)
      {
      Log.SetCondition(IsNothing(ProdB), 1, MMsg_Warning, "Bad Feed Stream - Not Bayer Model"); //expect stream to have bayer properties
      if (!IsNothing(ProdB))
        {
        IsOff = false;
        DoPrecip(Prod);
        }
      }
    if (IsOff)
      {
      //results...
      dTin    = Prod.T;
      dTout   = dTin;
      dQvin   = Prod.Volume();
      dQvout  = dQvin;
      dACin   = (IsNothing(ProdB) ? 0.0 : ProdB.AtoC());
      dACout  = dACin;
      dGrowthRateFactor = 0.0;
      dGrowthRate    = 0.0;
      dThermalLoss   = 0.0;
      dThermTempDrop = 0.0;
      dReactionHeat  = 0.0;
      dResidenceTime = 0.0;
  	  dSALout        = 0.0;
      dDiamout       = 0.0;
      dYield         = 0.0;
      dASat          = 0.0;
		  dSolPrecip     = 0.0;
      dOccSoda       = 0.0;
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

//====================================================================================
