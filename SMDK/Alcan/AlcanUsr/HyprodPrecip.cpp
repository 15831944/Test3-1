//===========================================================================
//=== SysCAD SMDK - Hyprod Precipitator Model 2003 : Alcan, Kenwalt       ===
//===========================================================================

#include "stdafx.h"
#define  __HYPRODPRECIP_CPP
#include "HyprodPrecip.h"

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

//===========================================================================

inline long ROUND(double x) { return (long)(x+0.5); }
inline long cINT(double x) { return (long)x; }
inline double SQR(double x) { return x*x; }

static MInitialiseTest InitTest("Precipitator");
static MAqSpeciePtr spAlumina          (InitTest, "Al2O3(l)", false);
static MSpeciePtr   spWater            (InitTest, "H2O(l)", false);
static MSpeciePtr   spTHA              (InitTest, "Al2O3.3H2O(s)", false);
static MAqSpeciePtr spCausticSoda      (InitTest, "NaOH(l)", false);
static MSpeciePtr   spOccSoda          (InitTest, "Na2O(s)", false);

//===========================================================================
const int idFeed=0;
const int idProd=1;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;          Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",    idFeed,   1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Product",        "Product", idProd,   1,   1,    0,    0.5f,  MIO_Out|MIO_Material },
    { NULL },
  };

static double PrecipDraw[] = { MDrw_Poly,  -5,10,  -5,-10,  5,-10,  5,10,
                      MDrw_End};
//---------------------------------------------------------------------------
DEFINE_SURGE_UNIT(Precipitator, "Precip", DLL_GroupName)

void Precipitator_UnitDef::GetOptions()
  {
  SetDefaultTag("PC", true);
  SetDrawing("Tank", PrecipDraw);
  SetTreeDescription("Process:Unit:Alcan Precipitator");
  SetDescription("Alcan Continuous Precipitator based on Hyprod model");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina|MLicense_Alcan);
  };

//---------------------------------------------------------------------------
//constructor
Precipitator::Precipitator(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd), m_QProd(this, "QProd")
  {
  //default values...
  m_bOn               = 1;
  m_bBypassOn         = 0;
  m_dBypass           = 0.05;

  m_dBypassFlow       = 0.0;
  m_dInTankSSA		  = 0.035;

  //m_iAlumina          = gs_MVDefn.Lookup("Al2O3(l)");
  //m_dMwtAl2O3         = gs_MVDefn[m_iAlumina].MolecularWt();     //101.961278                               
                                                          
  if (0)
    {// --- Start for test purposes (debug) ---
    static int xx=1;
    if (xx)
      {
      xx=0;

      long N=gs_MVDefn.Count();
      for (int i=0; i<N; i++)
        {
        MSpecieDefn &S=gs_MVDefn[i];
        Dbg.Print("%3i ", i);
        Dbg.Print("%10.2f ", S.MolecularWt());
        Dbg.Print("%10.2f ", S.Density());
        Dbg.Print("%10.2f ", S.TempLimitLo());
        Dbg.Print("%10.2f ", S.TempLimitHi());
        Dbg.Print("%10.2f ", S.msHs());
        Dbg.Print("%10.2f ", S.msCp());
        Dbg.Print("%10.2f ", S.VapourP());
        Dbg.Print("%-25s ", S.Symbol());
        Dbg.PrintLn("%s", S.Tag());
        }

      if (1) // ByName
        {
        LPCTSTR XXX[]={
          "H2O(g)",
            "C2H2F4(g)",
            "O2(g)",
            "CO2(g)",
            "CO2B(g)",
            "H2(g)",
            "H2O(l)",
            "Al2O3(l)",
            "Na2C2O4(l)",
            "Na2C5O7(l)",
            "Steam(g)",
            "Tetrafluoroethane(g)",
            "Oxygen(g)",
            "Carbon_dioxide(g)",
            "Carbon_dioxide_B(g)",
            "Hydrogen(g)",
            "Water(l)",
            "Soluble_alumina(l)",
            "Sodium_oxalate(l)",
            "Organics7(l)",
            //"XXXX",
            NULL,
          };

        i=0;
        while (XXX[i])
          {
          MSpecieDefn &S=gs_MVDefn[XXX[i]];
          Dbg.Print("%3i ", i);
          Dbg.Print("%10.2f ", S.MolecularWt());
          Dbg.Print("%10.2f ", S.Density());
          Dbg.Print("%10.2f ", S.TempLimitLo());
          Dbg.Print("%10.2f ", S.TempLimitHi());
          Dbg.Print("%10.2f ", S.msHs());
          Dbg.Print("%10.2f ", S.msCp());
          Dbg.Print("%10.2f ", S.VapourP());
          Dbg.Print("%-25s ", S.Symbol());
          Dbg.PrintLn("%s", S.Tag());
          i++;
          }
        }
      }
    // --- End for test purposes (debug) ---
    }
  }

//---------------------------------------------------------------------------

void Precipitator::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------
const int idDX_Version = 1;

void Precipitator::BuildDataFields()
  {
  static MDDValueLst DDHeatBalance[]=
    {
    {eHBal_Normal, "Normal"},
    {eHBal_ImposedTemp, "Imposed Temperature"},
    {eHBal_OptimumTemp, "Optimum Temperature"},
    {0}
    };

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
  DD.CheckBox ("On", "", &m_bOn, MF_PARAMETER);
  DD.Double   ("Volume", "", &m_dVolume, MF_PARAMETER, MC_Vol ("m^3"));
  DD.Double   ("Level", "", &m_dLevel, MF_PARAMETER, MC_Frac("%"));

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

  DD.Show(sm_bCompletePopulation);
  DD.Long("Agglom.Model", "", (long*)&m_eAgloModel, MF_PARAMETER|MF_SET_ON_CHANGE, DDModels);

  DD.Show(sm_bCompletePopulation && m_eAgloModel==eAgMdl_dlev);
  DD.Double("Param.WellAgit", "",  &m_dAgloParam[eAgMdl_dlev],      MF_PARAMETER);
  DD.Show(sm_bCompletePopulation && m_eAgloModel==eAgMdl_LowAgit);
  DD.Double("Param.LowAgit", "",   &m_dAgloParam[eAgMdl_LowAgit],   MF_PARAMETER);
  DD.Show(sm_bCompletePopulation && m_eAgloModel==eAgMdl_HighAgit);
  DD.Double("Param.HighAgit", "",  &m_dAgloParam[eAgMdl_HighAgit],  MF_PARAMETER);
  DD.Show(sm_bCompletePopulation && m_eAgloModel==eAgMdl_SizeInd);
  DD.Double("Param.SizeInd", "",   &m_dAgloParam[eAgMdl_SizeInd],   MF_PARAMETER);
  DD.Show(sm_bCompletePopulation && m_eAgloModel==eAgMdl_EquiSize);
  DD.Double("Param.EquiSize", "",  &m_dAgloParam[eAgMdl_EquiSize],  MF_PARAMETER);
  DD.Show(sm_bCompletePopulation && m_eAgloModel==eAgMdl_Groneweg);
  DD.Double("Param.Groneweg", "",  &m_dAgloParam[eAgMdl_Groneweg],  MF_PARAMETER);
  DD.Show();
  DD.Double("KvFac", "", &m_dKvFac, MF_PARAMETER,  MC_);  
  DD.Long("Nucleat.Model", "", (long*)&m_eNuclModel , MF_PARAMETER|MF_SET_ON_CHANGE, NucleatModels );
  DD.Show(m_eNuclModel==eNucl_LowShearRate || m_eNuclModel==eNucl_HighShearRate ||  m_eNuclModel== eNucl_LowSolidsConc || m_eNuclModel==eNucl_Hybrid);
  DD.Double("ShearRate", "", &m_eShearRate, MF_PARAMETER,  MC_);  
  DD.Show();
  DD.Text("");
  DD.Long("ThermalBalance", "", (long*)&m_eHeatBalance, MF_PARAMETER|MF_SET_ON_CHANGE, DDHeatBalance);
  DD.Show( m_eHeatBalance == eHBal_Normal );
  DD.Double("Surface", "", &m_dSurface, MF_PARAMETER,  MC_Area("m^2"));
  DD.Double("UCoef", "", &m_dUCoef, MF_PARAMETER,  MC_HTC("kcal/h.m^2.C"));
  DD.Double("InTankCooling", "", &m_dInTankCooling, ( (! m_eHeatBalance == eHBal_Normal) ? MF_RESULT : MF_PARAMETER), MC_Pwr("kcal/h"));
  DD.Show( m_eHeatBalance == eHBal_ImposedTemp );
  DD.Double   ("TempImposed", "", &m_dTempImpos, MF_PARAMETER, MC_T("C"));
  DD.Show();
  DD.Text("");
  DD.CheckBox ("BypassEnabled", "", &m_bBypassOn, MF_PARAMETER);
  DD.Show(m_bBypassOn);
  DD.Double("Bypass", "", &m_dBypass, MF_PARAMETER, MC_Frac("%"));
  DD.Show();
  DD.Text("");
  DD.Text("Results..");
  DD.Show(m_bBypassOn);
  DD.Double("BypassFlow", "", &m_dBypassFlow, MF_RESULT, MC_Qm("t/h"));
  DD.Show();
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

bool Precipitator::ExchangeDataFields()
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

bool Precipitator::ValidateDataFields()
  {//ensure parameters are within expected ranges
  m_dVolume  = DV.ValidateRange("Volume", 1.0, m_dVolume, 1000000);
  m_dSurface = DV.ValidateRange("Surface", 1.0, m_dSurface, 300000);
  m_dKvFac   = DV.ValidateRange("KvFac", 0.0, m_dKvFac, 30);
  m_dUCoef   = DV.ValidateRange("UCoef", 0.0, m_dUCoef, 2000);
  m_dLevel   = DV.ValidateRange("Level", 0.01, m_dLevel, 1);

  if ( m_eHeatBalance == eHBal_Normal)
    m_dTempImpos = DV.ValidateRange("TempImposed", C2K(45.0), m_dTempImpos, C2K(105.0));
  if (m_bBypassOn)
    m_dBypass = DV.ValidateRange("Bypass", 0.01, m_dBypass, 0.99);

  return true;
  }

//---------------------------------------------------------------------------

void Precipitator::EvalProducts()
  {
  if (!IsSolveDirect)//(!IsProbal)
    return;
  try
    {
    MStreamI Feed;
    FlwIOs.AddMixtureIn_Id(Feed, idFeed); //sum all input streams
    MStream & Prod = FlwIOs[FlwIOs.First[idProd]].Stream; //get a reference to the single output stream

    Prod = Feed; //set output stream = input stream
    m_dThermalLoss = 0.0;
    m_dYield = 0.0;
    m_dTempIn = Feed.T;
    MIBayer & FeedB=*Feed.FindIF<MIBayer>();
    if (!IsNothing(FeedB))
      {
      m_dACIn = FeedB.AtoC();
      m_dSolidConcIn = FeedB.SolidsConc(C2K(25.0));
      }
    else
      {
      m_dACIn = 0.0;
      m_dSolidConcIn = 0.0;
      }
    if (m_bOn && Feed.Mass()>UsableMass)
      {
      MIBayer & ProdB=*Prod.FindIF<MIBayer>();
      MISSA & FeedSSA = *Feed.FindIF<MISSA>();
	  MIPSD & FeedSz = *Feed.FindIF<MIPSD>();
      Log.SetCondition(IsNothing(ProdB), 1, MMsg_Warning, "Bad Feed Stream - Not Bayer Model"); //expect stream to have bayer properties
      Log.SetCondition(IsNothing(FeedSSA), 3, MMsg_Warning, "Bad Feed Stream - No SSA Data");

	  /* meansize =FeedSSA.PartDiamFromSAM;
	  if( meansize > 500)
		{
		SetStopRequired("PSD has deviated from typical conditions!");
		Log.SetCondition( true , 5, MMsg_Error, "PSD has deviated from typical conditions: Reinitialise!");
		}*/

      if (!IsNothing(FeedB) && !IsNothing(ProdB) && !IsNothing(FeedSSA))
        {
        RunSteady(Feed, Prod);
        
        double H0 = Feed.totHf(MP_All, Feed.T, Feed.P);
        m_dThermalLoss = H0-Prod.totHf(MP_All, Prod.T, Prod.P);

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
      m_dACOut = m_dACIn;
      m_dSolidConcOut = m_dSolidConcIn;
      Log.ClearCondition(1);
      Log.ClearCondition(2);
      Log.ClearCondition(3);
      }
    m_dTempOut = Prod.T;
    }
  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    //SetIdleRequired("Phone Denis");
    }
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    //SetIdleRequired("Phone Denis");
    }
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    //SetIdleRequired("Phone Denis");
    }
  catch (...)
    {
    Log.Message(MMsg_Error, "Some Unknown Exception occured");
    //SetIdleRequired("Phone Denis");
    }
  }

//--------------------------------------------------------------------------

void Precipitator::ClosureInfo(MClosureInfo & CI)
  {
  if (CI.DoFlows())
    {
    CI.AddPowerIn(0, -m_dThermalLoss);
    }
  }
//{****************************************************************************}
//{  procedure PRECIP evaluates the steady state values in a precipitator.
//
//               With Full Population Balance                                }
//{****************************************************************************}

//{****************************************************************************}

bool Precipitator::Precip(MVector & Feed, MVector & Prod, double Factor)
  {
  long m;
  double  gpl1,
          Time, dTime,
          ResTimeInfinite, ResTime;

  double  FeedHyPsd[MaxHPSizeClasses], FeedSSurf;
  double  ProdSSurf, ProdNNtl;

  const double Na2OFac = get_Na2OFactor(Feed);

  MIBayer & FeedB = *Feed.GetIF<MIBayer>();
  MIBayer & ProdB = *Prod.GetIF<MIBayer>();
  MIPSD & FeedSz = *Feed.FindIF<MIPSD>();
  MIPSD & ProdSz = *Prod.FindIF<MIPSD>();
  MISSA & FeedSSA = *Feed.FindIF<MISSA>();
  MISSA & ProdSSA = *Prod.FindIF<MISSA>();
  Log.SetCondition(IsNothing(FeedSz) || IsNothing(ProdSz), 2, MMsg_Error, "Bad Feed Stream - No Size Distribution");
  Log.SetCondition(IsNothing(FeedSSA) || IsNothing(ProdSSA), 3, MMsg_Error, "Bad Feed Stream - No SSA Data");

  if (!IsNothing(ProdSz) && !IsNothing(ProdSSA))
    {
    if (!InitSizeData(Prod))
      return false;

    double *ProdPSD=ProdSz.getFracVector(0);
    MassFrac2HyprodDist(SizeClass, ProdPSD, ProdHyPsd, NIntervals, ProdSSurf);

    double *FeedPSD=FeedSz.getFracVector(0);
    MassFrac2HyprodDist(SizeClass, FeedPSD, FeedHyPsd, NIntervals, FeedSSurf);

    InitAgloData(m_eAgloModel, m_dAgloParam[int(m_eAgloModel)]);

    double TFeed=Feed.T;
    double TProd=Prod.T;
    double FeedVolFlow=Feed.Volume();
    double ProdVolFlow=Prod.Volume();
    bool   result = false;

    //adjusting the PSD from 1 gpl to real conc....
    gpl1 = ProdB.SolidsConc(C2K(25.0));
    for (m=0; m<NIntervals; m++)
      ProdHyPsd[m]= gpl1 * ProdHyPsd[m];

    Time = 0.0;
    ResTime = m_dVolume*m_dLevel/ProdVolFlow;
    ResTimeInfinite = 5.0*ResTime;  //{give more Time to stabilise for the AGGLOM.}
    //PrecipRate = - Growth(Calum)*2.42 /2 /(156/102) * gpl1*(1 / (1 - gpl1/THADens))* psd.Ssurf;

    CalcSSurgNNtl(SizeClass, ProdHyPsd, NIntervals, ProdSSurf, ProdNNtl);
    if ( get_PrecipRate(Prod, ProdSSurf) <= 0 ) //!! THE LOOP IS CALCULT. ONLY if PRECIP HAPPENS
      {
      long IntegCount = 0;
      while ( Time < ResTimeInfinite )
        { 
        // adjustement of Time step according to reaction rate
        const double PRate = get_PrecipRate(Prod, ProdSSurf);
        dTime = (-1*Factor)/NZ(PRate);

        // dTime= - flowsheet.LiquorDensity /PrecipRate;
        if (dTime>ResTimeInfinite)
          dTime=ResTimeInfinite/2.0;
        Time += dTime;

        //initialise deltaPSD
        for (m=0; m<NIntervals; m++)
          dPSD[m]=0.0;

        //Variation due to Difference between INput and OUTput
        double FeedSolidsConc=FeedB.SolidsConc(C2K(25.0));
        //flow of the cur_eq is included in Const_boucle
        double Const_boucle = Min(1.0, dTime/ResTime);
        for (m=0; m<NIntervals; m++)
          dPSD[m]=(FeedHyPsd[m]*FeedSolidsConc*FeedVolFlow/ProdVolFlow-ProdHyPsd[m])*Const_boucle;

        //variation of Particles due to Agglomeration by Collision
        const double GRate2 = ProdB.GrowthRate();
        ApplyAgglom(GRate2, dTime, m_dKvFac, ProdNNtl);

		    //variation due to Nucleation in the Smallest Classes ONLY
        const double NRate = get_NucleationRate(m_eNuclModel, Prod, ProdSSurf, m_eShearRate);
        dPSD[0] += NRate*dTime;

        //variation due to Growth Only
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

		//            // mfshyd=VolVolFlowTotal*gpl1 -mfsSoda -mfsOxal -mfsSio2;
        double T0 = Prod.T;
        double Ain2 = ProdB.AluminaConc(C2K(25.0));
        double Cin2 = ProdB.CausticConc(C2K(25.0));
        double ASat2 = ProdB.AluminaConcSat(T0);
        double SSat2 = (Ain2-ASat2)/GTZ(Cin2);
        double NoPerSec2 = ProdSSA.PartNumPerSec();
        bool CErr;
        MassBalance(Prod, dNAN, dNAN, gpl1, 0/*NoPerSec2*/, Na2OFac, SSat2, CErr);

        double T1 = Prod.T;
        HeatBalance(Feed, Prod, T0);  // Éliminé pour vérifier la temp ?
        ProdVolFlow = Prod.Volume();
        IntegCount++;
        }; // end of integration loop

      //readjusting the PSD to 1 gpl...
      for (m=0; m<NIntervals; m++)
        ProdHyPsd[m] = ProdHyPsd[m] / gpl1;
      }

    HyprodDist2MassFrac(SizeClass, ProdHyPsd, ProdPSD, NIntervals);
    }

  return true;
  } // precip

//------------------------------------------------------------------------------

bool Precipitator::RunSteady(MStream & Feed, MStream & Prod)
  {  
  MIBayer & ProdB = *Prod.GetIF<MIBayer>();
	MStreamI NonReactStream;// this is the stream of slurry that bypass the tank
  //FlwIOs.AddMixtureIn_Id(NonReactStream, idFeed); //sum all input streams
  NonReactStream = Feed;
  m_bPrevPSDUsed = 0;

	if (m_bBypassOn)
		{
		Prod.SetF(Feed, MP_All, 1.0 - m_dBypass);
		NonReactStream.SetF(Feed, MP_All, m_dBypass);
		Feed = Prod;
    m_dBypassFlow = NonReactStream.Mass();
    }
  else
    {
    m_dBypassFlow = 0.0;
    }

  int ErrCode = PrecipSS(Feed, Prod, m_QProd);
  switch (ErrCode)
		{
		case 0:
			HeatBalance(Feed, Prod, Prod.T);
			
			break;
		case -1: Log.SetCondition(6, MMsg_Error, "PrecipSS Failed - No Solids"); return false;
		case -2: Log.SetCondition(10, MMsg_Error, "PrecipSS Failed - No Caustic"); return false;
		case -3: Log.Message(MMsg_Note, "Pas Precipitation"); return false;
		case -4: Log.SetCondition(8, MMsg_Error, "PrecipSS Failed - Infinite solve loop"); return false;
		}

  if (m_eHeatBalance ==eHBal_OptimumTemp)
		{  
    const double CAl     = ProdB.AluminaConc(C2K(25.0));
    const double CCaus   = ProdB.CausticConc(C2K(25.0));
    const double CNaCl  = ProdB.CltoC() *CCaus ; //NOT as Na2CO3
    const double CCarb  = ProdB.SodiumCarbonateConc(C_2_K(25.0));
    const double CSulf  = ProdB.SodiumSulphateConc(C2K(25.0)); //NOT as Na2CO3
    const double cTOC   = ProdB.TOC(C_2_K(25.0));
    const double A = 4.736  + 0.006*cTOC  + 0.1217*( CCaus/53+ CCarb/106+ CSulf/142+CNaCl/58 );
    const double B = -2193.6;   
    m_dTempImpos =  B / ( log( 6378*CAl/CCaus/(6378-2*B) ) -A );
    ErrCode = PrecipSS(Feed, Prod, m_QProd);// recalculate the A/C at the optimum temperature
    }

  bool PopErr = false;
  if (sm_bCompletePopulation)
    {
    double intFactor = 0.5;
    while (!Precip(Feed, Prod, intFactor) && !PopErr)
      {
      intFactor = intFactor / 2.0;
      if (intFactor < 0.01)
        {
        Log.SetCondition(4, MMsg_Error, "Integration Factor < 0.01");
        PopErr = true;
        }
      if (!PopErr && !PrecipSS(Feed, Prod, m_QProd))
        {
        // The Specific Surface RUN IS ALWAYS DONE at least as an Estimate
        PopErr = true;
        }
      }
    }
  else
    {
    if (sm_bUsePrevPSD)
      {
      SetPSDfromPrev(Prod, m_QProd);
      }
    }

  if (m_bBypassOn)
		{
		Prod.AddF(NonReactStream, MP_All, 1.0); //MUST add this back to the feed
		Feed.AddF(NonReactStream, MP_All, 1.0);
		}

  if (PopErr)
    return false;
  return true;
  };

//---------------------------------------------------------------------------
//===========================================================================
