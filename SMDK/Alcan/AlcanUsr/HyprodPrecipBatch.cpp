//===========================================================================
//=== SysCAD SMDK - Hyprod Batch Precipitator Model 2003 : Alcan, Kenwalt ===
//===========================================================================

#include "stdafx.h"
#define  __HYPRODPRECIPBATCH_CPP
#include "HyprodCommon.h"
#include "HyprodPrecipBatch.h"

#include "comutil.h"
#pragma comment(lib, "comsupp.lib")

#define dbgEnabled 0
#if dbgEnabled
const bool dbgHPPrecipIO = true;
const bool dbgHPPrecipInt = true;
const bool dbgHP_Q_Vli_Kva = true;
const bool dbgPerformTHA = true;
const bool dbgHPPrecipSS = true;
#endif

#if ForceOptimizeOff
#pragma optimize("", off)
#endif

#define WithOneGPLScaling   1
#define WithGrowth          1
#define WithAgglomeration   1
#define WithNucleation      1

inline long ROUND(double x) { return (long)(x+0.5); }
inline long cINT(double x) { return (long)x; }
inline double SQR(double x) { return x*x; }

static MInitialiseTest InitTest("BatchPrecip");
static MSpeciePtr   spAlumina          (InitTest, "Al2O3(l)", false);
static MSpeciePtr   spWater            (InitTest, "H2O(l)", false);
static MSpeciePtr   spTHA              (InitTest, "Al2O3.3H2O(s)", false);
static MSpeciePtr   spCausticSoda      (InitTest, "NaOH(l)", false);
static MSpeciePtr   spOccSoda          (InitTest, "Na2O(s)", false);

//===========================================================================

//define connection ids for convenience
const int idSlurry  = 0;
const int idLiquor  = 1; //optional filtrate
const int idDrawOff = 2;

//define input & output connections
static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;          Id;  Rqd; Max; CnId, FracHgt;  Options;
    { "Slurry",         "Slurry",  idSlurry,  1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Liquor",         "Liquor",  idLiquor,  0,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "DrawOff",        "DrawOff", idDrawOff, 1,   1,    0,    0.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

//define default drawing symbol
static double BatchPrecipDraw[] = { MDrw_Poly,  -9,7,  -9,-5,  -6,-7,  -3,-5,  -3,7,
                           MDrw_Poly,  -3,-5,  0,-7,  3,-5, 3,7,
                           MDrw_Poly,  3,-5,  6,-7,  9,-5, 9,7,
                           MDrw_End};

//---------------------------------------------------------------------------
//macro to declare and add the model to the system
DEFINE_SURGE_UNIT(BatchPrecip, "BatchPrecip", DLL_GroupName)

void BatchPrecip_UnitDef::GetOptions()
  {
  SetDefaultTag("BP", true);
  SetDrawing("Tank", BatchPrecipDraw);
  SetTreeDescription("Process:Unit:Alcan Batch Precipitator");
  SetDescription("Alcan Batch Precipitator based on Hyprod model");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina|MLicense_Alcan);
  };

//---------------------------------------------------------------------------
//constructor
BatchPrecip::BatchPrecip(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  //default values...
  m_bOn               = 1;
  m_bAgglomONwhileFilling = 1;
  m_bUseCircTime      = 1;
  m_dNbTankAvailable  = 0.0;
  m_dFillingTempDrop  = 1.0;
  m_dSeedCharge       = 100.0;
  m_dCirculationTime  = 24.0*3600.0;
  m_dFillingTime      = 2.0*3600.0;
  m_dDrawOffTime      = 2.0*3600.0;
  m_dNbSeededDay      = 1.0;
  m_dSeedingTime      = 1.0*3600.0;
  m_dInTankSSA		  = 0.035;
  }

//---------------------------------------------------------------------------

void BatchPrecip::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

const int idDX_Version = 1;

void BatchPrecip::BuildDataFields()
  {
 static MDDValueLst DDHeatBalance[]=
    {
    {eHBal_Normal, "Normal"},
    {eHBal_ImposedTemp, "Set End Temperature"},
  //  {eHBal_OptimumTemp, "Optimum Temperature"},
    {0}
    };
  DD.Text     ("");
  DD.CheckBox ("On", "",   &m_bOn,  MF_PARAMETER);
  //DD.Text     ("");
  //DD.Text     ("Global options...");
  //DD.Double   ("AmbientTemp",          &HyprodPrecipHelper::m_dAmbientTemp,         MF_PARAMETER, MC_T("C") );
  //DD.Double   ("WindSpeed",            &HyprodPrecipHelper::m_dWindSpeed,           MF_PARAMETER, MC_Ldt);
  //DD.CheckBox ("CompletePopulation",   &Precipitator::sm_bCompletePopulation,  MF_PARAMETER);
  DD.Text     ("Global options...");
  DD.String("Version", "", idDX_Version, MF_RESULT);
  DD.CheckBox ("CompletePopulation", "", &sm_bCompletePopulation, MF_PARAM_STOPPED);
  DD.Show(!sm_bCompletePopulation);
  DD.CheckBox ("UseSSA_from_PopRun", "", &sm_bUsePrevPSD, MF_PARAM_STOPPED);
  DD.Show( (!sm_bUsePrevPSD)&&(!sm_bCompletePopulation)  );
  DD.Double("InTankSSA_of_solids", "", &m_dInTankSSA, MF_PARAMETER,  MC_);  

  DD.Show();
  DD.Text     ("");
  DD.Text     ("Configuration");
  DD.Double   ("Volume", "",               &m_dVolume,              MF_PARAMETER,  MC_Vol ("m^3"));
  DD.Double   ("KvFac", "",                &m_dKvFac,               MF_PARAMETER,  MC_);
  DD.Double   ("Level", "",                &m_dLevel,               MF_PARAMETER,  MC_Frac("%"));
                      
  static MDDValueLst DDModels[]=
    {
    {eAgMdl_dlev,     "WellAgit"},
    {eAgMdl_LowAgit,  "LowAgit"},
    {eAgMdl_HighAgit, "HighAgit"},
    {eAgMdl_SizeInd,  "SizeInd"},
    {eAgMdl_EquiSize, "EquiSize"},
    {eAgMdl_Groneweg, "Groneweg"},
    {0}
    };

   static MDDValueLst NucleatModels[]=
    {
    {eNucl_Hyprod,     "Original Hyprod Model"},
    {eNucl_NBrown, "N.Brown batch model"},
	{eNucl_LowShearRate, "Low shear model"},
	{eNucl_HighShearRate, "High shear model"},
	{eNucl_LowSolidsConc, "Low Solids model"}, 
    {eNucl_Hybrid, "New Hybrid model"}, 
    {0}
    };

  DD.Long("Agglom.Model", "", (long*)&m_eAgloModel,            MF_PARAMETER|MF_SET_ON_CHANGE, DDModels);
  DD.Show(m_eAgloModel==eAgMdl_dlev);
  DD.Double("Param.WellAgit", "",  &m_dAgloParam[eAgMdl_dlev],      MF_PARAMETER);
  DD.Show(m_eAgloModel==eAgMdl_LowAgit);
  DD.Double("Param.LowAgit", "",   &m_dAgloParam[eAgMdl_LowAgit],   MF_PARAMETER);
  DD.Show(m_eAgloModel==eAgMdl_HighAgit);
  DD.Double("Param.HighAgit", "",  &m_dAgloParam[eAgMdl_HighAgit],  MF_PARAMETER);
  DD.Show(m_eAgloModel==eAgMdl_SizeInd);
  DD.Double("Param.SizeInd", "",   &m_dAgloParam[eAgMdl_SizeInd],   MF_PARAMETER);
  DD.Show(m_eAgloModel==eAgMdl_EquiSize);
  DD.Double("Param.EquiSize", "",  &m_dAgloParam[eAgMdl_EquiSize],  MF_PARAMETER);
  DD.Show(m_eAgloModel==eAgMdl_Groneweg);
  DD.Double("Param.Groneweg", "",  &m_dAgloParam[eAgMdl_Groneweg],  MF_PARAMETER);
  DD.Show();
  DD.Long("Nucleat.Model", "", (long*)&m_eNuclModel , MF_PARAMETER|MF_SET_ON_CHANGE, NucleatModels );
  DD.Show(m_eNuclModel==eNucl_LowShearRate || m_eNuclModel==eNucl_HighShearRate ||  m_eNuclModel== eNucl_LowSolidsConc || m_eNuclModel==eNucl_Hybrid);
  DD.Double("ShearRate", "", &m_eShearRate, MF_PARAMETER,  MC_);  
  DD.Show();
  DD.Text("");
  DD.Long("ThermalBalance", "", (long*)&m_eHeatBalance, MF_PARAMETER|MF_SET_ON_CHANGE, DDHeatBalance);
  DD.Show( m_eHeatBalance == eHBal_Normal );
  DD.Double("Surface", "",   &m_dSurface,             MF_PARAMETER,  MC_Area("m^2"));
  DD.Double("UCoef", "",                &m_dUCoef,               MF_PARAMETER,  MC_HTC("kcal/h.m^2.C"));
  DD.Double("InTankCooling", "",  &m_dInTankCooling, ((! m_eHeatBalance == eHBal_Normal) ? MF_RESULT : MF_PARAMETER),MC_Pwr("kcal/h"));
  DD.Show(m_eHeatBalance == eHBal_ImposedTemp );
  DD.Double("DrawOffTemperature", "", &m_dTempImpos,     MF_PARAMETER|MF_NAN_OK                           ,MC_T("C"));
 
  DD.Show();
  DD.Text("");
  DD.Text("Batch Control");
  DD.CheckBox ("CirculationTimeSpecified", "",   &m_bUseCircTime,  MF_PARAMETER);
  DD.Double("CirculationTime", "",   &m_dCirculationTime,  MF_PARAMETER ,MC_Time("h"));
  DD.Double("FillingTime", "",    &m_dFillingTime,  MF_PARAMETER ,MC_Time("h"));
  DD.Double("SeedingTime", "",    &m_dSeedingTime,  MF_PARAMETER ,MC_Time("h"));
  DD.CheckBox ("AgglomerationWhileFilling", "", &m_bAgglomONwhileFilling, MF_PARAMETER);
  DD.Double("DrawOffTime", "",    &m_dDrawOffTime,  MF_PARAMETER ,MC_Time("h"));
  DD.Double("TanksInUse", "",     &m_dNbTankAvailable, (m_dCirculationTime ? MF_PARAMETER : MF_RESULT), MC_);
  DD.Double("SeededPerDay", "",   &m_dNbSeededDay,  (m_dCirculationTime ? MF_RESULT : MF_PARAMETER), MC_);
  DD.Double("FillingTempDrop", "", &m_dFillingTempDrop,  MF_PARAMETER, MC_dT("C"));
  //DD.Double("SeedCharge",   &m_dSeedCharge, MF_PARAMETER, MC_);
  DD.Text("");
  DD.Text("Results");
  DD.Double("Loss2Ambient", "", &m_dLoss2Ambient, MF_RESULT, MC_Pwr("kcal/h"));
  DD.Double("ResidenceTime", "", &m_dResTime, MF_RESULT, MC_Time("h"));
  DD.Double("Yield", "", &m_dYield, MF_RESULT, MC_Conc("g/L"));
  DD.Show(!sm_bCompletePopulation);
  DD.CheckBox ("PrevPSDUsed", "", &m_bPrevPSDUsed, MF_RESULT);
  DD.Show();
  DD.Double("Ti", "", &m_dTempIn, MF_RESULT, MC_T("C"));
  DD.Double("To", "", &m_dTempOut, MF_RESULT, MC_T("C"));
  DD.Double("ACin", "", &m_dACIn, MF_RESULT, MC_);
  DD.Double("ACout", "", &m_dACOut, MF_RESULT, MC_);
  DD.Double("SolidConcIn", "", &m_dSolidConcIn, MF_RESULT, MC_Conc("g/L"));
  DD.Double("SolidConcOut", "", &m_dSolidConcOut, MF_RESULT, MC_Conc("g/L"));

  DD.Object(m_QProd, MDD_RqdPage);
  }
//---------------------------------------------------------------------------

bool BatchPrecip::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
    case idDX_Version: 
      DX.String = VersionDescription;
      return true;
    }
  return false;
  }


//---------------------------------------------------------------------------

bool BatchPrecip::ValidateDataFields()
  {//ensure parameters are within expected ranges
  m_dVolume  = DV.ValidateRange("", 1.0,  m_dVolume , 1000000);
  m_dSurface = DV.ValidateRange("", 1.0,  m_dSurface , 300000);
  m_dKvFac   = DV.ValidateRange("", 0.0,     m_dKvFac , 30);
  m_dUCoef   = DV.ValidateRange("", 0.0,     m_dUCoef , 2000);
  m_dLevel   = DV.ValidateRange("", 1.0e-6,  m_dLevel , 1);
  
  if ( m_eHeatBalance == eHBal_ImposedTemp)
    m_dTempImpos = DV.ValidateRange("", C2K(1.0e-6), m_dTempImpos, C2K(150.0));

  return true;
  };

//---------------------------------------------------------------------------

void BatchPrecip::EvalProducts()
  {
  #if ForceOptimizeOff
  static Cnt = 0;
  if (stricmp(getTag(), "SpeciauxUE")==0)
    {
    Cnt++;//place breakpoint here to stop for specified model
    }
  #endif
  if (!IsSolveDirect)//(!IsProbal)
    return;
  bool Err = true;
  try
    {
    MStream Slurry;
    MStream Liquor;
    FlwIOs.AddMixtureIn_Id(Slurry, idSlurry); //sum all input slurry streams
    m_bHasLiquor = (FlwIOs.First[idLiquor]>=0); //test if liquor stream connected
    if (m_bHasLiquor)
      {
      FlwIOs.AddMixtureIn_Id(Liquor, idLiquor); //sum all input liquor streams
      //double LiquorSolids = Liquor.Mass();
      }
    MStream & Prod = FlwIOs[FlwIOs.First[idDrawOff]].Stream; //get a reference to the single output stream
    
    Prod = Slurry; //set output stream = input stream (including PSD data, etc)
    m_dThermalLoss = 0.0;
    m_dYield = 0.0;
    m_dTempIn = Slurry.T;
    MIBayer & SlurryB = *Slurry.FindIF<MIBayer>(); //get access to bayer properties interface for stream
    if (!IsNothing(SlurryB))
      {
      m_dACIn = SlurryB.AtoC();
      m_dSolidConcIn = SlurryB.SolidsConc(C2K(25.0));
      }
    else
      {
      m_dACIn = 0.0;
      m_dSolidConcIn = 0.0;
      }
    
    if (m_bOn && Slurry.Mass()>UsableMass)
      {
      MIBayer & ProdB = *Prod.FindIF<MIBayer>();
      Log.SetCondition(IsNothing(ProdB), 1, MMsg_Warning, "Bad Slurry Stream - Not Bayer Model"); //expect stream to have bayer properties
      
      if (!IsNothing(SlurryB) && !IsNothing(ProdB))
        {
		// ---   --- Thermal Losses. ------------
		// !! this should be done COMBINED with FLAHSING/evaporation as the Caustic concentration should increase
		Slurry.SetTP(Slurry.T -m_dFillingTempDrop, Slurry.P);
		Liquor.SetTP(Liquor.T -m_dFillingTempDrop, Liquor.P);

        double SlurryH0 = Slurry.totHf(MP_All, Slurry.T, Slurry.P);
		double LiquorH0 = (m_bHasLiquor ? Liquor.totHf(MP_All, Liquor.T, Liquor.P) : 0.0);
 
       //Prod.SetTemp(Prod.Temp()-m_dTempDrop);
	
	    RunSteady(Slurry, Liquor, Prod);
        
        m_dThermalLoss = (SlurryH0+LiquorH0)-Prod.totHf(MP_All, Prod.T, Prod.P);
        
        m_dACOut = ProdB.AtoC();
        m_dSolidConcOut = ProdB.SolidsConc(C2K(25.0));
        //m_dResTime = TankVol/GTZ(Prod.Volume(som_SL));
        double Cout = ProdB.CausticConc(Prod.T);
        m_dYield = Cout*(m_dACIn-m_dACOut);
        }
		  if (sm_bCompletePopulation)
			  m_QProd = Prod;//copy the content of the stream in the equipement only if STILL in Pop mode
      }
    else
      {
      if (m_bHasLiquor)
        {      
        Prod.AddF(Liquor, MP_All, 1.0);
        }
      MIBayer & ProdB = *Prod.FindIF<MIBayer>();
      if (!IsNothing(ProdB))
        {
        m_dACOut = ProdB.AtoC();
        m_dSolidConcOut = ProdB.SolidsConc(C2K(25.0));
        }
      else
        {
        m_dACOut = 0.0;
        m_dSolidConcOut = 0.0;
        }
      }
    m_dTempOut = Prod.T;
    Err = false;
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

  if (Err)
    {//Something is wrong!!!  Bug needs fixing!!! 
    //So lets set product=feed....
    MStream Feed;
    MStream & Prod = FlwIOs[FlwIOs.First[idDrawOff]].Stream; //get a reference to the single output stream

    FlwIOs.AddMixtureIn_Id(Feed, idSlurry); //sum all input slurry streams
    Prod = Feed;
    if (FlwIOs.First[idLiquor]>=0)
      {
      MStream Liquor;
      FlwIOs.AddMixtureIn_Id(Liquor, idLiquor); //sum all input liquor streams
      Prod.AddF(Liquor, MP_All, 1.0);
      }
    CString ProblemModel;
    ProblemModel = getTag();
    //SetStopRequired("Phone Denis!");
    }
  Log.SetCondition(Err, 5, MMsg_Error, "Error needs fixing!!!");
  }

//--------------------------------------------------------------------------

void BatchPrecip::ClosureInfo(MClosureInfo & CI)
  {
  if (CI.DoFlows())
    {
    CI.AddPowerIn(0, -m_dThermalLoss);
    }
  }

//{****************************************************************************}
//  THIS function perfrom the same role as MASSBALANCE for the continuous precipitator
double BatchPrecip::PerformAluminaSolubility(MVector & Vec, double TRqd, double ARqd, double THARqd, double NoPerSec, double Na2OFac, double SSat, bool & ConvergeErr)
  {
  //   2 AlO2 + 4 H2O <==> Al2O3.3H2O + 2 OH
  //or Na2O + Al2O3 + 4 H2O <==> Al2O3.3H2O + 2 NaOH

  // THADelta is the Fraction of Alumina which precipitates as the hydrate
  // ie THADelta is deposition rate of new THA crystal

  const double ESoda=2335.0; //constant 2535K

  bool AdjustT=!Valid(TRqd);
  bool SetTHA=Valid(THARqd);
  
  double T=AdjustT ? Vec.T : TRqd;


  double &AluminaMass  = Vec.MassVector[spAlumina];     // Al2O3
  double &WaterMass    = Vec.MassVector[spWater];       // H2O
  double &THAMass      = Vec.MassVector[spTHA];         // Al2O3.3H2O
  double &CausticMass  = Vec.MassVector[spCausticSoda]; // NaOH
  double &Na2OMass     = Vec.MassVector[spOccSoda];     // Na2O

  const double Fact = spAlumina.MW/spTHA.MW; // 0.654;

  MIBayer & BVec=*Vec.GetIF<MIBayer>();

  for (int Iter=100; Iter; Iter--)
    {
    if (AdjustT)
      T=Vec.T;
    double CC= BVec.CausticConc(C2K(25.0));
    double A=BVec.AluminaConc(C2K(25.0));
    double THA=BVec.SolidsConc(T);
    double THADelta;
    if (SetTHA)
      {
      if (fabs(THA-THARqd)<1.0e-12*THARqd)
        break;
      THADelta = THAMass*(THARqd/GTZ(THA)-1.0);
      }
    else
      {
      if (fabs(A-ARqd)<1.0e-12*ARqd)
        break;
      THADelta = AluminaMass*(1.0-ARqd/GTZ(A))/Fact;
      }
    double alumSSat = CC *SSat;
    const double LclVar = Na2OFac*7.1868e-8*exp(ESoda/T)*Pow(alumSSat, 1.0955);
    double XSoda = THADelta*LclVar;

    double dAluminaMass = - Fact*THADelta;
    double dTHAMass     = + THADelta;
    double dWaterMass   = - (1.0-Fact)*THADelta + spWater.MW/spOccSoda.MW*XSoda;
    double dCausticMass = - 2.0*spCausticSoda.MW/spOccSoda.MW*XSoda;
    double dNa2OMass    = + XSoda;
    double Scl;

    for(;;)
      {
      Scl=1;
      if (AluminaMass + Scl*dAluminaMass <0)
        Scl=Min(Scl, 1-(AluminaMass + Scl*dAluminaMass)/NZ(dAluminaMass));
      if (THAMass     + Scl*dTHAMass     <0)
        Scl=Min(Scl, 1-(THAMass + Scl*dTHAMass)/NZ(dTHAMass));
      if (WaterMass   + Scl*dWaterMass   <0)
        Scl=Min(Scl, 1-(WaterMass + Scl*dWaterMass)/NZ(dWaterMass));
      if (CausticMass + Scl*dCausticMass <0)
        Scl=Min(Scl, 1-(CausticMass + Scl*dCausticMass)/NZ(dCausticMass));
      if (Na2OMass    + Scl*dNa2OMass    <0)
        Scl=Min(Scl, 1-(Na2OMass + Scl*dNa2OMass)/NZ(dNa2OMass));
      
      if (Scl<(1-1e-12))
        {
        dAluminaMass *= Scl; 
        dTHAMass     *= Scl; 
        dWaterMass   *= Scl; 
        dCausticMass *= Scl; 
        dNa2OMass    *= Scl; 
        }
      else
        break;
      }
    
    if (fabs(dAluminaMass)<1e-22)
      {//problem!!!
      Iter=0;
      break;
      }

    //adjust masses...
    AluminaMass = AluminaMass + dAluminaMass;
    THAMass     = THAMass     + dTHAMass;    
    WaterMass   = WaterMass   + dWaterMass;  
    CausticMass = CausticMass + dCausticMass;
    Na2OMass    = Na2OMass    + dNa2OMass;   
    
    Vec.MarkStateChanged(); //this forces recalculation of temperature / enthalpy based on new stream makeup
    }

  ConvergeErr = (Iter==0);
  Log.SetCondition(ConvergeErr, 9, MMsg_Warning, "Cannot converge PrecipTHA Alumina Conc");
  if (ConvergeErr)
    {
    int xx=0; //place breakpoint here to trap this
    }

  MISSA & VecSSA=*Vec.GetIF<MISSA>();
  if (NoPerSec>0.0 && !IsNothing(VecSSA))
    {
    VecSSA.SetSAMFromFlow(BVec.THAMassFlow(), NoPerSec);
    }
  return BVec.AluminaConc(T);
  }

//{****************************************************************************}
//{****************************************************************************}

  bool BatchPrecip::PrecipBatchSS(double dTime, MVector & Prod, double CurLevel)
  {
  MIBayer & ProdB   = *Prod.GetIF<MIBayer>();
  MIPSD & ProdSz    = *Prod.FindIF<MIPSD>();
  MISSA & ProdSSA   = *Prod.FindIF<MISSA>();
  double TProd = Prod.getT();
  double gpl1 = ProdB.SolidsConc(TProd);
  double ProdVolFlow = Prod.Volume(MP_All);
  double SolidsInitial  = Prod.Mass(MP_Sol);

  double Sx;
  if (!sm_bCompletePopulation && sm_bUsePrevPSD)
    {
    MISSA & PrevSSA=*m_QProd.FindIF<MISSA>();// this is the SSA of the last Popul run to use in "NO POpul mode"
    if (IsNothing(PrevSSA))
      {
	   m_bPrevPSDUsed = 0;
      Sx=ProdSSA.SpecificSurfaceAreaMass(); //m^2/g//PROBLEM!!! Prev does not have SSA, use feed
      }
    else
      {
	  m_bPrevPSDUsed = 1;
      Sx=PrevSSA.SpecificSurfaceAreaMass(); //m^2/g
      }
    }
  else
    {
	m_bPrevPSDUsed = 0;
    Sx = m_dInTankSSA ; // the SSA value manually entered by the user in m^2/g
    }
  Sx=Range(0.020, Sx, 0.085);
 
  //=== mass balance ===
  double T0 = Prod.T;
  double Ain2 = ProdB.AluminaConc(C2K(25.0));
  double Cin2 = ProdB.CausticConc(C2K(25.0));
  double ASat2 = ProdB.AluminaConcSat(T0);
  double SSat2 = (Ain2-ASat2)/GTZ(Cin2);
  double NoPerSec2 = ProdSSA.PartNumPerSec();
  double enthIn = Prod.totCp(MP_All) * K2C(T0); // Enthalpie IN

  double PrecipRate1 = get_PrecipRate(Prod, Sx);
  gpl1 =  gpl1 - PrecipRate1 * dTime*156/102 ;

  bool CErr;
  const double Na2OFac = get_Na2OFactor(Prod);

  PerformAluminaSolubility(Prod, dNAN, dNAN, gpl1, 0, Na2OFac, SSat2, CErr);

  double T1 = Prod.T;
 
  double SolidsVariation =  (Prod.Mass(MP_Sol) - SolidsInitial); //* 0.97 ;// 0.97 is to compensate for the HYPROD error
  HeatBalance(Prod, SolidsVariation, enthIn, dTime, CurLevel);  // Éliminé pour vérifier la temp ?
  
  return true;
  }

//{****************************************************************************}

bool BatchPrecip::PrecipBatch(double dTime, MVector & Prod, double CurLevel, bool AgglomON )
  {
  long m;
  MIBayer & ProdB   = *Prod.GetIF<MIBayer>();
  MIPSD & ProdSz    = *Prod.FindIF<MIPSD>();
  MISSA & ProdSSA   = *Prod.FindIF<MISSA>();
  double TProd = Prod.getT();
  double gpl1 = ProdB.SolidsConc(TProd);
  double ProdVolFlow = Prod.Volume(MP_All);
  double SolidsInitial  = Prod.Mass(MP_Sol);

  double ProdSSurf, ProdNNtl;

  //adjusting the PSD from 1 gpl to real conc....
  for (m=0; m<NIntervals; m++)
    ProdHyPsd[m]= gpl1 * ProdHyPsd[m];

  #if ForceOptimizeOff
  for (m=0; m<NIntervals; m++)
    if (ProdHyPsd[m]<0.0)
      SetStopRequired("Phone Denis, negative PSD!");
  #endif
  CalcSSurgNNtl(SizeClass, ProdHyPsd, NIntervals, ProdSSurf, ProdNNtl);

  //initialise deltaPSD
  for (m=0; m<NIntervals; m++)
    dPSD[m]=0.0;

  //variation of Particles due to Agglomeration by Collision
  const double GRate2 = ProdB.GrowthRate();
  if (AgglomON )
	ApplyAgglom(GRate2, dTime, m_dKvFac, ProdNNtl);

  //variation due to Nucleation in the Smallest Classes ONLY
  const double NRate = get_NucleationRate(m_eNuclModel, Prod, ProdSSurf, m_eShearRate);
  dPSD[0] += NRate*dTime;

  // variation due to Growth Only
  const double GRate3 = ProdB.GrowthRate();
  const double Const_boucle3 = GRate3*dTime/HPDSize;
  dPSD[0] += Const_boucle3*( ProdHyPsd[0]-ProdHyPsd[2] )/2.0;
  for (m=1; m<NIntervals-1; m++)
    dPSD[m] += Const_boucle3*(ProdHyPsd[m-1]-ProdHyPsd[m+1])/2.0;

  // Finally Substr. or Adding Particles to the Each Classes
  for (m=0; m<NIntervals; m++)
    {
    ProdHyPsd[m] += dPSD[m];
    ProdHyPsd[m] = Max(0.0, ProdHyPsd[m]);
    }

  CalcSSurgNNtl(SizeClass, ProdHyPsd, NIntervals, ProdSSurf, ProdNNtl); //Update ProdSSurf and ProdNNtl

  double gpl1x=gpl1;
  gpl1=0.0;
  for (m=0; m<NIntervals; m++)            
    {
    gpl1=gpl1 + ProdHyPsd[m]*pow(SizeClass[m],3)/6E12;
    }
  gpl1 = gpl1 * PI * 2.42;// *ProdVolFlow;

  //readjusting the PSD to 1 gpl...
  for (m=0; m<NIntervals; m++)
    ProdHyPsd[m] = ProdHyPsd[m] / gpl1;

  //=== mass balance ===
  double T0 = Prod.T;
  double Ain2 = ProdB.AluminaConc(C2K(25.0));
  double Cin2 = ProdB.CausticConc(C2K(25.0));
  double ASat2 = ProdB.AluminaConcSat(T0);
  double SSat2 = (Ain2-ASat2)/GTZ(Cin2);
  double NoPerSec2 = ProdSSA.PartNumPerSec();
  double enthIn = Prod.totCp(MP_All) * K2C(T0); // Enthalpie IN

  bool CErr;
  const double Na2OFac = get_Na2OFactor(Prod);

  PerformAluminaSolubility(Prod, dNAN, dNAN, gpl1, 0/*NoPerSec2*/, Na2OFac, SSat2, CErr);

  double T1 = Prod.T;
 
  double SolidsVariation =  (Prod.Mass(MP_Sol) - SolidsInitial); //* 0.97 ;// 0.97 is to compensate for the HYPROD error
 //    mfshydPrev:=mfshyd;
//     mfshyd:=flowtotal*gpl1 -mfsSoda -mfsOxal -mfsSio2 ;the way it was done in HYPROD.. it is WRONG though

  HeatBalance(Prod, SolidsVariation, enthIn, dTime, CurLevel);  // Éliminé pour vérifier la temp ?
  
  return true;
  }

//{****************************************************************************}

bool BatchPrecip::BatchCycle(MVector & Slurry, MVector & Liquor, MVector & Prod)
  {
  double dt;
  MIBayer & SlurryB = *Slurry.GetIF<MIBayer>();
  MIBayer & ProdB   = *Prod.GetIF<MIBayer>();
  MIPSD & SlurrySz  = *Slurry.FindIF<MIPSD>();
  MIPSD & ProdSz    = *Prod.FindIF<MIPSD>();
  MISSA & SlurrySSA = *Slurry.FindIF<MISSA>();
  MISSA & ProdSSA   = *Prod.FindIF<MISSA>();
  Log.SetCondition(IsNothing(SlurrySz) || IsNothing(ProdSz), 2, MMsg_Error, "Bad Slurry Stream - No Size Distribution");
  Log.SetCondition(IsNothing(SlurrySSA) || IsNothing(ProdSSA), 3, MMsg_Error, "Bad Slurry Stream - No SSA Data");

  m_dCirculationTime = Max(1.0, m_dCirculationTime);
  m_dSeedingTime     = Max(1.0, m_dSeedingTime);
  m_dDrawOffTime     = Max(1.0, m_dDrawOffTime);
  m_dFillingTime     = Max(1.0, m_dFillingTime);
  m_dFillingTempDrop = Max(0.0, m_dFillingTempDrop);

  if (!IsNothing(ProdSz) && !IsNothing(ProdSSA))
    {
    if (!InitSizeData(Prod))
      return false;

    InitAgloData(m_eAgloModel, m_dAgloParam[int(m_eAgloModel)]);

    double *ProdPSD=ProdSz.getFracVector(0); //array of PSD data

    //Determine adjusted filtrate and slurry feed for "single" tank
    double SlurryTtlFlow = Slurry.Volume();
    if (SlurryTtlFlow<1.0e-6)
      return true;
    double LiquorTtlFlow = (m_bHasLiquor ? Liquor.Volume() : 0.0);
    if (m_bHasLiquor && LiquorTtlFlow<1.0e-6)
      return true; //todo error message
    double CombinedFlows = SlurryTtlFlow + LiquorTtlFlow;
    const double SlurryTtlMassFlow = Slurry.Mass();
    const double LiquorTtlMassFlow = (m_bHasLiquor ? Liquor.Mass() : 0.0);

    bool ContSeeding = !m_bHasLiquor;
    double FractionFiltrate = 0.0;
    double SeedFlow = 0.0;
    double FillFlow = 0.0;
    if (ContSeeding)
      {
      SeedFlow = (m_dVolume*m_dLevel) / (m_dFillingTime+m_dSeedingTime);// !! in that CASE filling means the PL&Seed filling time
      Slurry.AdjustMassTo(MP_All, SlurryTtlMassFlow*SeedFlow/SlurryTtlFlow);
      }
    else
      {
      FractionFiltrate = LiquorTtlFlow / (LiquorTtlFlow+SlurryTtlFlow);
      FillFlow = (m_dVolume*m_dLevel) * FractionFiltrate / m_dFillingTime;
      SeedFlow = (m_dVolume*m_dLevel) * (1.0-FractionFiltrate) / m_dSeedingTime;
      Liquor.AdjustMassTo(MP_All, LiquorTtlMassFlow*FillFlow/LiquorTtlFlow);
      Slurry.AdjustMassTo(MP_All, SlurryTtlMassFlow*SeedFlow/SlurryTtlFlow);
      //Liquor.SetF(Liquor, MP_All, FillFlow/LiquorTtlFlow);
      }

    /*if (m_dNbTankAvailable<=0.0)
      {
      Nb = (m_dFilltime + m_dSeedingTime + Recirc_t +DrawOff_t) * ( CombinedFlows) / (m_dVolume*level);
      }*/
    //todo: No Of Tanks???

    double iLevel;
    if (ContSeeding)
      {
      iLevel = 0.2;
      const double TimeSoFar = iLevel*(m_dVolume*m_dLevel) / Slurry.Volume();
      const double d = Slurry.Mass()* TimeSoFar ;//kg
      Prod.SetM(Slurry, MP_All, d);
      //const double d = iLevel*(m_dVolume*m_dLevel) / Slurry.Volume();
      //Prod.SetF(Slurry, MP_All, d);
      // les valeurs dans PrecipResult ne sont plus des debits mais des volumes et des masses;
      }
    else
      {
      iLevel = FractionFiltrate;
      const double d = Liquor.Mass()*m_dFillingTime;//kg
      Prod.SetM(Liquor, MP_All, d);
      // les valeurs dans PrecipResult ne sont plus des debits mais des volumes et des masses;
      // todo !!!!! reduire la temperature du filtrat et Flash
      }

    double ProdSSurf;
    MassFrac2HyprodDist(SizeClass, ProdPSD, ProdHyPsd, NIntervals, ProdSSurf); //gets PSD for 1 gpl

    double target = m_dLevel;

    //add seed until specified level (filling tank)
    while (iLevel < target)
      {
      double dt = 36.0; //seconds
      if ( (iLevel + Slurry.Volume()*dt/(m_dVolume*m_dLevel)) > target )
        dt = fabs( (target-iLevel) * (m_dVolume*m_dLevel) / Slurry.Volume());
      if (dt>0.0000001)
        {
        iLevel += (Slurry.Volume()*dt/(m_dVolume*m_dLevel));

        //MixBatch...
        HyprodDist2MassFrac(SizeClass, ProdHyPsd, ProdPSD, NIntervals);
        Prod.AddM(Slurry, MP_All, Slurry.Mass()*dt);
        MassFrac2HyprodDist(SizeClass, ProdPSD, ProdHyPsd, NIntervals, ProdSSurf); //gets PSD for 1 gpl

        //precip reaction...
        if (ProdB.SolidsConc(C2K(25.0))>5.0) // need a minimum solid conc. to precipitate
		   if (sm_bCompletePopulation)
				{
				if (!PrecipBatch(dt, Prod, iLevel,m_bAgglomONwhileFilling))// with population balance
					return false;
				}
			else
				{
				if (!PrecipBatchSS(dt, Prod, iLevel))// with SSA
					return false;
				}
        //application.processMessages;
        //if flowsheet.UserCancel then
        //  break;
        }
      else 
        iLevel = target;
      }//end while

    //precip reaction (tank is full)
    double DummyProdNNtl;
    CalcSSurgNNtl(SizeClass, ProdHyPsd, NIntervals, ProdSSurf, DummyProdNNtl);
    double PrecipRate = get_PrecipRate(Prod, ProdSSurf); //!! THE LOOP IS CALCULT. ONLY if PRECIP HAPPENS
    double Btime=0.0;
    if (PrecipRate<0.0)  
      while (Btime < m_dCirculationTime )
        {
        PrecipRate = get_PrecipRate(Prod, ProdSSurf);
        if (sm_bCompletePopulation)
			dt = -0.2/PrecipRate;
		else
			dt = -0.5/PrecipRate;
        if ( (Btime+dt) > m_dCirculationTime)
          dt = m_dCirculationTime - Btime;
        Btime += dt;
        if (sm_bCompletePopulation)
				{
				if (!PrecipBatch(dt, Prod, iLevel, true))// with population balance
					return false;
				}
			else
				{
				if (!PrecipBatchSS(dt, Prod, iLevel))// with SSA
					return false;
				if (sm_bUsePrevPSD)
					{
					//SetPSDfromPrev(Prod);
					}
				}
        //application.processMessages;
        //if flowsheet.UserCancel then
        //  break;
        }

    Prod.AdjustMassTo(MP_All, SlurryTtlMassFlow+LiquorTtlMassFlow);

    HyprodDist2MassFrac(SizeClass, ProdHyPsd, ProdPSD, NIntervals);
    }

  return true;
  } //BatchCycle

//{------------------------------------------------------------------------------
//Nom: BatchPrecip::RunSteady
//Description: Execute simulation
//------------------------------------------------------------------------------}

bool BatchPrecip::RunSteady(MVector & Slurry, MVector & Liquor, MVector & Prod)
  {
  if (BatchCycle(Slurry, Liquor, Prod))
    {
    Log.ClearCondition(4);
    }
  else
    {
    Log.SetCondition(true, 4, MMsg_Error, "BatchCycle failed");
    return false;
    }

  return true;
  }

//--------------------------------------------------------------------------
