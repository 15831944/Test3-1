//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "DemoBayerSM.h"
#include "math.h"
#include "md_headers.h"
#pragma comment(lib, "rpcrt4.lib")
#pragma optimize("", off)

//===========================================================================
//
//
//
//===========================================================================

DEFINE_SPECIEMODEL(CDemoBayerSM)
IMPLEMENT_SPECIEMODEL(CDemoBayerSM, "DemoBayerSM", "*Demonstration Bayer", "Demonstration User Bayer Specie Model", DLL_GroupName)

//===========================================================================
// Specie Access
static MInitialiseTest InitTest("CDemoBayerSMClass");

MSpeciePtr   Water             (InitTest, "H2O(l)",        false);
MAqSpeciePtr Alumina           (InitTest, "Al2O3(l)",      false);
MAqSpeciePtr CausticSoda       (InitTest, "NaOH(l)",       false);
MAqSpeciePtr SodiumChloride    (InitTest, "NaCl(l)",       false);
MAqSpeciePtr SodiumSulphate    (InitTest, "Na2SO4(l)",     false);
MAqSpeciePtr SodiumCarbonate   (InitTest, "Na2CO3(l)",     false);
//MAqSpeciePtr SodiumFluoride    (InitTest, "NaF(l)",        false);
MAqSpeciePtr SodiumOxalate     (InitTest, "Na2C2O4(l)",    false); //organic
MAqSpeciePtr Organics          (InitTest, "Na2C5O7(l)",    false); //organic
MAqSpeciePtr SilicateLiq       (InitTest, "SiO2(l)",       false);

MSpeciePtr   OccSoda           (InitTest, "Na2O(s)",       false);
MSpeciePtr   THA               (InitTest, "Al2O3.3H2O(s)", false);
MSpeciePtr   AluminaSolid      (InitTest, "Al2O3(s)",      false);

MSpeciePtr   Steam             (InitTest, "H2O(g)",        true);

// ==========================================================================
// Specie Properties Modal Global Parameters

const double MW_C        = 12.011;

enum DLM_DefineLiquorMethod { DLM_TotOrganics, DLM_TOC };
enum BDM_BayerDensityMethod { BDM_Original, BDM_Other };
enum BPM_BPEMethod { BPM_Dewey, BPM_Adamson };

double CDemoBayerSM::sm_dMinSolCp = 1.046;
double CDemoBayerSM::sm_dH2OTestFrac0  = 0.98;
double CDemoBayerSM::sm_dH2OTestFrac1  = 0.96;
//byte CDemoBayerSM::sm_iRqdCalcMethod = DLM_TOC;
byte CDemoBayerSM::sm_iDensityMethod = BDM_Original;
byte CDemoBayerSM::sm_iBPEMethod  = BPM_Dewey;

// ==========================================================================
//
// A helper class to assist with calculating iterative concentration and density
// calculations.
//
// ==========================================================================

bool   CBayerConcs::NaFactorOK=0;
MArrayI CBayerConcs::NaFactor;

CBayerConcs::CBayerConcs(CDemoBayerSM *pMdl)
  {
  MInitialise(); // this must be called to initialise the SMDK library if neccessary
  pBayerMdl = pMdl;
  Zero();

  if (!NaFactorOK)
    {
    for (int sn=0; sn<gs_MVDefn.Count(); sn++)
      NaFactor[sn]=1.0; //For all the species without sodium ions
    NaFactor[::CausticSoda]     = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::CausticSoda.MW);      
    NaFactor[::SodiumCarbonate] = 1.0;                                                        
    NaFactor[::SodiumOxalate]   = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumOxalate.MW);    
    NaFactor[::SodiumChloride]  = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumChloride.MW);   
    NaFactor[::SodiumSulphate]  = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumSulphate.MW);   
    //NaFactor[::SodiumFluoride]  = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumFluoride.MW);   
    //NaFactor[::SodiumSilicate]  = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumSilicate.MW);
    NaFactor[::Organics]        = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::Organics.MW);         

    NaFactorOK = true;
    }
  }

// --------------------------------------------------------------------------

void CBayerConcs::Zero()
  {
  Density25 = 1100.0;
  for (int sn=0; sn<gs_MVDefn.Count(); sn++)
    Liq[sn] = 0.0;
  }

// --------------------------------------------------------------------------

bool CBayerConcs::Converge(MArray & MA)
  {
  double TLiq = MA.Mass(MP_Liq);
  if (TLiq<1.0e-9)
    {
    Zero();
    return true;
    }

  static MToleranceBlock s_Tol(TBF_Both, "DemoBayerSM:Density", 1e-12, 1e-8, 100);

  const double Tc = 25.0;
  const double T_ = C_2_K(Tc);
  // Converge Liquor Conc. All sodium concentrations expressed as Na2CO3
  int IterCount = s_Tol.GetMaxIters();//100;
  double OldDensity = Density25;
  while (1)
    {
    for (int sn=0; sn<gs_MVDefn.Count(); sn++)
      {
      if (gs_MVDefn[sn].IsLiquid())
        Liq[sn] = MA[sn] / TLiq * Density25 * NaFactor[sn];
        }

    Density25 = Range(0.0001, LiquorDensity(T_, MA), 10000.0);
    if (fabs(OldDensity - Density25) < s_Tol.GetAbs() || --IterCount==0)
      break;
    OldDensity = Density25;
    } // end of while
  Density25 = Max(0.001, Density25);
  return (IterCount>0);
  }

//---------------------------------------------------------------------------

double CBayerConcs::LTotalSodium(MArray & MA)
  {
  //Expressed as Na2CO3
  double TSodium =
         ( MA[CausticSoda]    * NaFactor[::CausticSoda]
         + MA[SodiumCarbonate]
         + MA[SodiumOxalate]  * NaFactor[::SodiumOxalate]
         + MA[SodiumChloride] * NaFactor[::SodiumChloride]
         + MA[SodiumSulphate] * NaFactor[::SodiumSulphate]
         //+ MA[SodiumFluoride] * NaFactor[::SodiumFluoride]
         + MA[Organics]       * NaFactor[::Organics] );
         //+ MA[SodiumSilicate] * NaFactor[::SodiumSilicate] );
  return TSodium;
  }

// --------------------------------------------------------------------------

double CBayerConcs::LiquorDensEqn1(double Tc, double WTNa, double WAlumina)
  {
  double RHO25 = 0.982
    +( 0.01349855*WTNa)
    +(-0.00024948*WTNa*WTNa)
    +( 0.00000273*WTNa*WTNa*WTNa)
    +( 0.00208035*WAlumina)
    +( 0.00004113*WAlumina*WAlumina)
    +(-0.00000728*WAlumina*WAlumina*WAlumina)
    +( 0.00033367*WTNa*WAlumina);     // Density at 25
  if (Tc==25.0)
    return (RHO25 * 1000.0); // kg/m^3
  double RHO = RHO25 * (1.0
            -(0.0005021858*0.85*(Tc-25))
            -(0.0000011881*0.85*(Tc-25)*(Tc-25)) ); // Density at T
  return (RHO * 1000.0); // kg/m^3
  }

// --------------------------------------------------------------------------

double CBayerConcs::LiquorDensity(double T_, MArray & MA)
  {
  /*Liquor Density with mass fractions*/
  double TLiq=MA.Mass(MP_Liq); //Total Liquid
  if (TLiq<1.0e-9)
    return 0.0;

  double Tc = K2C(T_);
  switch (CDemoBayerSM::sm_iDensityMethod)
    {
    case BDM_Original:
      {
      // Version independent of caustic concentration
      // Calculates density in gm/cc
      // By J.W.Mulloy
      double TNa=LTotalSodium(MA);        // Total Sodium Compounds as Na2CO3 kg/s
      double Alumina=MA[::Alumina];  // Alumina kg/s
      double WTNa=TNa*100.0/TLiq;         // Wgt % of TNa
      double WAlumina=Alumina*100.0/TLiq; // Wgt % of Alumina
      double RHO = LiquorDensEqn1(Tc, WTNa, WAlumina);
      return  RHO;
      }
    case BDM_Other:
      {
      // TODO: Some other density correlation...
      double TNa=LTotalSodium(MA);        // Total Sodium Compounds as Na2CO3 kg/s
      double Alumina=MA[::Alumina];  // Alumina kg/s
      double WTNa=TNa*100.0/TLiq;         // Wgt % of TNa
      double WAlumina=Alumina*100.0/TLiq; // Wgt % of Alumina
      double RHO = LiquorDensEqn1(Tc, WTNa, WAlumina);
      return RHO;
      }
    }
  return 1000.0;
  }

//===========================================================================
//
//
//
//===========================================================================

CDemoBayerSM::CDemoBayerSM(TaggedObject *pNd) : LiqConcs25(this)
  {
  MInitialise(); // this must be called to initialise the SMDK library if neccessary

  fDoCalc   = false;
  //feed calculator defaults...
  dRqd_AtoC = 0.4;
  dRqd_C    = 220.0;
  dRqd_CtoS = 0.85;
  dRqd_Sul  = 20.0;
  dRqd_Sil  = 1.0;
  dRqd_Ox   = 45.0;
  dRqd_TOC  = 12.0;
  dRqd_Salt = 15.0;
  //dRqd_Fl   = 5.0;
  dRqd_SolFrac  = 0.0;
  dRqd_SolConc  = dNAN;
  }

//---------------------------------------------------------------------------

CDemoBayerSM::~CDemoBayerSM()
  {
  }

//---------------------------------------------------------------------------

bool CDemoBayerSM::get_IsBaseClassOf(LPCTSTR OtherProgID)
	{
  return false;
  //return S_OK;
	}

//---------------------------------------------------------------------------

LPCTSTR CDemoBayerSM::get_PreferredModelProgID()
	{
  return NULL;
  }

//---------------------------------------------------------------------------

void CDemoBayerSM::BuildDataFields()
  { 
  };              

//---------------------------------------------------------------------------

bool CDemoBayerSM::ExchangeDataFields()
  {
  return false;
  };

//---------------------------------------------------------------------------

bool CDemoBayerSM::ValidateDataFields()
  { 
  //_asm int 3;
  bool OK=MSpModelBase::ValidateDataFields();
  InputCalcs(fDoCalc, Temperature);
  return true; 
  }; 

//---------------------------------------------------------------------------

//bool CDemoBayerSM::ValidateData()
//  {
//  _asm int 3;
//  bool OK=MSpModelBase::ValidateData();
//  InputCalcs(fDoCalc, Temperature);
//  return OK;
//  }

//---------------------------------------------------------------------------

double CDemoBayerSM::get_Density(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  const double MSol=(Phases & MP_Sol) ? MA.Mass(MP_Sol) : 0.0;
  const double MLiq=(Phases & MP_Liq) ? MA.Mass(MP_Liq) : 0.0;
  const double MGas=(Phases & MP_Gas) ? MA.Mass(MP_Gas) : 0.0;
  const double MTot=GTZ(MSol+MLiq+MGas);
  const double MSL = MSol+MLiq;
  if (MSL<UsableMass || MSL/MTot<1.0e-6)
    return MSpModelBase::get_Density(Phases, T, P, pMA);

  const double WaterFrac = (MLiq>1e-6 ? MA[Water]/MLiq : 0.0);
  if (WaterFrac>sm_dH2OTestFrac0)
    return MSpModelBase::get_Density(Phases, T, P, &MA);

  const double FSol=MSol/MTot;
  const double FLiq=MLiq/MTot;
  double Dl=1.0;
  if (FLiq>1.0e-9)
    {
    LiqConcs25.Converge(MA);
    Dl = LiqConcs25.LiquorDensity(T, MA);
    if (WaterFrac>sm_dH2OTestFrac1)
      {
      const double Std_Dl = MSpModelBase::get_Density(MP_Liq, T, P, &MA);
      const double Std_Prop = Min(1.0, (WaterFrac-sm_dH2OTestFrac1)/(sm_dH2OTestFrac0-sm_dH2OTestFrac1));
      Dl = (Dl*(1.0-Std_Prop)) + (Std_Dl*Std_Prop);
      }
    }
  SMFnRanges.SetInRange(eSpFn_Rho, true);

  return DensityMix(FSol, dNAN, FLiq, Dl, (1.0-FSol-FLiq), dNAN, T, P, MA);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::LiqCpCalc(MArray & MA)
  {
  // Heat Capacity of Bayer Liquor Kcal/kg.degC
  // A function of the weight fraction of TNa and Al2O3
  // From published Russian data

  LiqConcs25.Converge(MA);
  const double MLiq = MA.Mass(MP_Liq); //Total Liquid kg/s

  double TNaAsCO3    = LiqConcs25.LTotalSodium(MA);
  double TNaAsCO3Use = Max(TNaAsCO3, MLiq*0.19); // Only Valid for  TNaAsCO3/FLiq > 0.19
  double TNa         = 100.0*TNaAsCO3*::OccSoda.MW/::SodiumCarbonate.MW/MLiq; //61.98=MW Na2O, 105.99=MW Na2CO3
  double TAl2O3      = 100.0*MA[Alumina]/MLiq;
  double Cpl  = -0.020113606661083*TNa
                +0.001081165172606*TNa*TNa
                -0.000022606160779*TNa*TNa*TNa
                -0.004597725999883*TAl2O3
                -0.000001053264708*TAl2O3*TAl2O3
                -0.00000218836287*TAl2O3*TAl2O3*TAl2O3;
  // Scale for more dilute Liquors
  Cpl = 1.0275057375729+(TNaAsCO3/TNaAsCO3Use*Cpl);

  Cpl = KCal_2_kJ(Cpl); // kJ/kg.degC   (KCal_2_kJ=4.184)
 
  //Dbg.PrintLn("CDemoBayerSM::LiqCpCalc %12.3f %12.3f %12.3f %7.2f", MA.Mass(), TNaAsCO3, TNaAsCO3Use, Cpl); 
  return Cpl;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::get_msEnthalpy(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  SMFnRanges.SetInRange(eSpFn_H, true);

  const double MSol=(Phases & MP_Sol) ? MA.Mass(MP_Sol) : 0.0;
  const double MLiq=(Phases & MP_Liq) ? MA.Mass(MP_Liq) : 0.0;
  const double MGas=(Phases & MP_Gas) ? MA.Mass(MP_Gas) : 0.0;
  const double MTot=GTZ(MSol+MLiq+MGas);
  const double MSL = MSol+MLiq;
  if (MSL<UsableMass || MSL/MTot<1.0e-6)
    return MSpModelBase::get_msEnthalpy(Phases, T, P, &MA);

  const double WaterFrac = (MLiq>1e-6 ? MA[Water]/MLiq : 0.0);
  if (WaterFrac>sm_dH2OTestFrac0)
    return MSpModelBase::get_msEnthalpy(Phases, T, P, &MA);

  const double FSol=MSol/MTot;
  const double FLiq=MLiq/MTot;
  const double Tc=K_2_C(T);

  double Hl=0.0;
  if (FLiq>1.0e-9)
    {
    Hl = LiqCpCalc(MA) * Tc;
    if (WaterFrac>sm_dH2OTestFrac1)
      {
      const double Std_Hl = MSpModelBase::get_msEnthalpy(MP_Liq, T, P, &MA);
      const double Std_Prop = Min(1.0, (WaterFrac-sm_dH2OTestFrac1)/(sm_dH2OTestFrac0-sm_dH2OTestFrac1));
      Hl = (Hl*(1.0-Std_Prop)) + (Std_Hl*Std_Prop);
      }
    }
  double Hs=(FSol>1.0e-9) ? gs_MVDefn.msHz(MP_Sol, T, P, PropOverides, &MA) : 0.0;

  return msEnthalpyMix(FSol, Hs, FLiq, Hl, (1.0-FSol-FLiq), dNAN, T, P, MA);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::get_msEntropy(long Phases, double T, double P, MArray *pMA)
  {
  SMFnRanges.SetInRange(eSpFn_S, true);
  return MSpModelBase::get_msEntropy(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::get_msCp(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  SMFnRanges.SetInRange(eSpFn_Cp, true);

  const double MSol=(Phases & MP_Sol) ? MA.Mass(MP_Sol) : 0.0;
  const double MLiq=(Phases & MP_Liq) ? MA.Mass(MP_Liq) : 0.0;
  const double MGas=(Phases & MP_Gas) ? MA.Mass(MP_Gas) : 0.0;
  const double MTot=GTZ(MSol+MLiq+MGas);
  const double MSL = MSol+MLiq;
  if (MSL<UsableMass || MSL/MTot<1.0e-6)
    return MSpModelBase::get_msCp(Phases, T, P, pMA);

  const double WaterFrac = (MLiq>1e-6 ? MA[Water]/MLiq : 0.0);
  if (WaterFrac>sm_dH2OTestFrac0)
    return MSpModelBase::get_msCp(Phases, T, P, &MA);

  const double FSol=MSol/MTot;
  const double FLiq=MLiq/MTot;

  double Cpl=0.0;
  if (FLiq>1.0e-9)
    {
    Cpl = LiqCpCalc(MA);
    if (WaterFrac>sm_dH2OTestFrac1)
      {
      const double Std_Cpl = MSpModelBase::get_msCp(MP_Liq, T, P, &MA);
      const double Std_Prop = Min(1.0, (WaterFrac-sm_dH2OTestFrac1)/(sm_dH2OTestFrac0-sm_dH2OTestFrac1));
      Cpl = (Cpl*(1.0-Std_Prop)) + (Std_Cpl*Std_Prop);
      }
    }
  double Cps=(FSol>1.0e-9) ? Max(sm_dMinSolCp, gs_MVDefn.msCp(MP_Sol, T, P, PropOverides, &MA)) : 0.0;

  return msCpMix(FSol, Cps, FLiq, Cpl, (1.0-FSol-FLiq), dNAN, T, P, MA);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::get_SaturationT(double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  if (MA.Mass(MP_SL)/GTZ(MA.Mass())<1.0e-6)
    return MSpModelBase::get_SaturationT(P, pMA);

  const double MLiq = MA.Mass(MP_Liq);
  const double WaterFrac = (MLiq>1e-6 ? MA[Water]/MLiq : 0.0);
  if (WaterFrac>sm_dH2OTestFrac0)
    return MSpModelBase::get_SaturationT(P, &MA);

  const double Std_SatT = MSpModelBase::get_SaturationT(P, &MA);
  const double BPE = BoilPtElev(MA, Std_SatT);
  double SatT = Std_SatT+BPE;
  if (WaterFrac>sm_dH2OTestFrac1)
    {
    const double Std_Prop = Min(1.0, (WaterFrac-sm_dH2OTestFrac1)/(sm_dH2OTestFrac0-sm_dH2OTestFrac1));
    SatT = (SatT*(1.0-Std_Prop)) + (Std_SatT*Std_Prop);
    }
  return SatT;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::get_SaturationP(double T, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  if (MA.Mass(MP_SL)/GTZ(MA.Mass())<1.0e-6)
    return MSpModelBase::get_SaturationP(T, pMA);

  const double MLiq = MA.Mass(MP_Liq);
  const double WaterFrac = (MLiq>1e-6 ? MA[Water]/MLiq : 0.0);
  if (WaterFrac>sm_dH2OTestFrac0)
    return MSpModelBase::get_SaturationP(T, &MA);

  //converge...
  const int BPE_MaxIter = 8;
  const double BPE_Tol = 1.0e-3;
  double BPE = 0.0;
  for (int Iter=BPE_MaxIter; Iter; Iter--)
    {
    double NewBPE = BoilPtElev(MA, T-BPE);
    NewBPE = Range(0.0, NewBPE, 20.0);
    if (fabs(NewBPE-BPE)<BPE_Tol)
      break;
    BPE = NewBPE;
    }
  double SatP = MSpModelBase::get_SaturationP(T-BPE, pMA);
  if (0)
    Dbg.PrintLn("DemoBayer SatP = %10.3f %10.3f %10.3f", T, BPE, SatP);

  if (WaterFrac>sm_dH2OTestFrac1)
    {
    const double Std_SatP = MSpModelBase::get_SaturationP(T, &MA);
    const double Std_Prop = Min(1.0, (WaterFrac-sm_dH2OTestFrac1)/(sm_dH2OTestFrac0-sm_dH2OTestFrac1));
    SatP = (SatP*(1.0-Std_Prop)) + (Std_SatP*Std_Prop);
    }
  return SatP;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::BoilPtElev(MArray & MA, double T)
  {//parameter T is the PureWaterSaturation temperature at the given pressure
  double TLiq=MA.Mass(MP_Liq); // Total Liquid kg/s
  
  SMFnRanges.SetInRange(eSpFn_BPE, true);

  if (TLiq<1.0e-9)
    return 0.0;

  double BPE = 0.0;
  if (sm_iBPEMethod==BPM_Dewey)
    {
    if (MA[Alumina]<1.0e-6 && MA[CausticSoda]<1.0e-6 && MA[SodiumCarbonate]<1.0e-6)
      {
      SMFnRanges.SetInRange(eSpFn_BPE, false);
      return 0.0;
      }

    // Gram ion per kg liq
    const double giAl2O3    = 1000 * MA[Alumina] / TLiq * 2.0 * 81.97 / ::Alumina.MW; // 81.97 = MW of NaAlO2
    const double giNaOH     = 1000 * MA[CausticSoda] / TLiq
          - (1000 * MA[Alumina] / TLiq * 2.0 * ::CausticSoda.MW/::Alumina.MW);
    const double giNa2CO3   = 1000 * MA[SodiumCarbonate] / TLiq;
    const double giNaCl     = 1000 * MA[SodiumChloride] / TLiq;
    const double giNa2SO4   = 1000 * MA[SodiumSulphate] / TLiq;
    //double giNaF      = 1000 * MA[SodiumFluoride] / TLiq;
    const double giNa2C5O7  = 1000 * MA[Organics] / TLiq;
    const double giNa2C2O4  = 1000 * MA[SodiumOxalate] / TLiq;

    const double giSum = giAl2O3 + giNaOH + giNa2CO3 + giNaCl + giNa2SO4 + /*giNaF + */giNa2C5O7 + giNa2C2O4;

    const double gH2OperKgLiq = 1000.0 - giSum;

    //Gram moles per kg liq
    const double gmAl2O3    = giAl2O3 / 81.97; // 81.97 = MW of NaAlO2
    const double gmNaOH     = giNaOH / ::CausticSoda.MW;
    const double gmNa2CO3   = giNa2CO3 / ::SodiumCarbonate.MW;
    const double gmNaCl     = giNaCl / ::SodiumChloride.MW;
    const double gmNa2SO4   = giNa2SO4 / ::SodiumSulphate.MW;
    //const double gmNaF      = giNaF / ::SodiumFluoride.MW;
    const double gmNa2C5O7  = giNa2C5O7 / ::Organics.MW;
    const double gmNa2C2O4  = giNa2C2O4 / ::SodiumOxalate.MW;

    const double gmSum = gmAl2O3 + gmNaOH + gmNa2CO3 + gmNaCl + gmNa2SO4 + /*gmNaF + */gmNa2C5O7 + gmNa2C2O4;

    // Total gmoles/kg H2O = molality
    const double gmSumPerH2O = 1000.0 * gmSum/Max(0.1, gH2OperKgLiq);

    // Boiling point elevation degC
    BPE = 0.00182 + 0.55379*Pow((gmSumPerH2O/10.0),7)
                + 0.0040625*gmSumPerH2O*T
                + (1.0/T) * (-286.66*gmSumPerH2O + 29.919*gmSumPerH2O*gmSumPerH2O
                              +0.6228*Pow(gmSumPerH2O,3))
                - (0.032647*gmSumPerH2O*Pow((gmSumPerH2O*T/1000.0),2))
                + (Pow(T*0.001,5.0) * (5.9705*gmSumPerH2O
                    -0.57532*gmSumPerH2O*gmSumPerH2O + 0.10417*Pow(gmSumPerH2O,3)));
    if (0)
      Dbg.PrintLn("DemoBayer %7.3f %10.3f %10.3f", T, gmSumPerH2O , BPE);
    }
  else
    {//sm_iBPEMethod==BPM_Adamson
    LiqConcs25.Converge(MA);
    if (LiqConcs25.Liq[::CausticSoda]<1.0e-6 && LiqConcs25.Liq[::SodiumCarbonate]<1.0e-6)
      {
      SMFnRanges.SetInRange(eSpFn_BPE, false);
      return 0.0;
      }
    //Equation: f(x1,x2) = a+b*x1+c*x2+d*x1^2+e*x2^2+f*x1*x2+g*x1^3+h*x2^3+i*x1*x2^2+j*x1^2*x2
    //Constants a to f determined by equation data fit to above equation based on defined data
    //from published Adamson data, where x1 is Na2O conc g/L @ 25C; and x2 is temp in degC.
    const double a = 0.007642857;
    const double b = 0.006184282;
    const double c = 2.92857E-05;
    const double d = 0.00010957;
    const double e = -3.80952E-08;
    const double f = 0.000208801;
    const double g = -8.61985E-10;
    const double h = 8.33556E-18;
    const double i = 1.7316E-10;
    const double j = -2.49763E-07;

    const double Sodium = LiqConcs25.Liq[::CausticSoda] + LiqConcs25.Liq[::SodiumCarbonate]
                        + LiqConcs25.Liq[::SodiumOxalate] + LiqConcs25.Liq[::Organics]
                        + LiqConcs25.Liq[::SodiumChloride] + LiqConcs25.Liq[::SodiumSulphate]; //+ LiqConcs25.Liq[::SodiumFluoride];
    const double x1 = Sodium*::OccSoda.MW/::SodiumCarbonate.MW;
    const double x2 = K2C(T);
    const double x1_2 = x1*x1;
    const double x2_2 = x2*x2;
    BPE = a + b*x1 + c*x2 + d*x1_2 + e*x2_2 + f*x1*x2 + g*x1*x1_2 + h*x2*x2_2 + i*x1*x2_2 + j*x1_2*x2;
    }

  return BPE;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::get_DynamicViscosity(long Phases, double T, double P, MArray *pMA)
  {

  return MSpModelBase::get_DynamicViscosity(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::get_ThermalConductivity(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_ThermalConductivity(Phases, T, P, pMA);
  }

//===========================================================================
//
//
//
//===========================================================================
// The properties that are defined for visibility in the SysCAD Access properties window
// in adition, these properties are accesable in code by name.

enum {
 idDensityMethod   		     ,
 idBPEMethod       		     ,
 idMinSolCp        		     ,
 idH2OTestFrac0    		,
 idH2OTestFrac1    		,

 idDefineLiquor			       ,
 idRqd_A_to_C				       ,
 idRqd_C_to_S				       ,
 idRqd_C					         ,
 idRqd_Oxalate			       ,
 idRqd_TOC				         ,
 idRqd_Na2SO4				       ,
 idRqd_NaCl				         ,
 //idRqd_NaF				         ,
 idRqd_SiO2				         ,
 idRqd_SolConc			       ,
 idRqd_SolFrac			       ,

 idAluminaConc25			     ,
 idAtoC					           ,
 idCtoS					           ,
 idCltoC					         ,
 idNa2SO4toC				       ,
 idNa2CO3toS				       ,
 idTOC25					         ,
 idNa2C2O4Conc25			     ,
 idNaClConc25				       ,
 idNaSO4Conc25			       ,
 idFreeCaustic25           ,
 idSolidsConc25			       ,

 idSeparator1				       ,

 idCausticConc25			     ,
 idSodaConc25				       ,
 idCarbonateConc25		     ,
 idOrganateConc25			     ,
 idOxalateConc25			     ,
 idTotalOrganics25		     ,
 idChlorineConc25			     ,
 idSulphateConc25			     ,
 idTotalNa25				       ,

 idSeparator2				       ,

 idLVolume25				       ,
 idSLVolume25				       ,
 idLDensity25 			       ,
 idSLDensity25			       ,

 idSeparator3				       ,	

 idAluminaConc			       ,
 idSodiumCarbonateConc     ,
 idTOC					           ,
 idSolidsConc				       ,

 idSeparator4				       ,	

 idCausticConc			       ,
 idSodaConc				         ,

 idSeparator5				       ,	

 idAluminaConcSat			     ,
 idAtoCSaturation			     ,
 idSSNRatio				         ,
 idOxalateEquilibrium	     ,

 idSeparator6				       ,	

 idBoilPtElev				       ,
 idTHAMassFlow   			     ,
 idTHADens				         ,

  idMPI_EndOfProps		   
  };

//---------------------------------------------------------------------------

long CDemoBayerSM::DefinedPropertyCount()
  {
  return idMPI_EndOfProps + 1 + MSpModelBase::DefinedPropertyCount();
  }

//---------------------------------------------------------------------------

static MPropertyInfo::MStringValueP SVDens[]={
  { "Original",  BDM_Original  },
  { "Other", BDM_Other },
  {0}};
static MPropertyInfo::MStringValueP SVBPE[]={
  {"Dewey",   BPM_Dewey   },
  {"Adamson", BPM_Adamson },
  {0}};

long CDemoBayerSM::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {//define a list of all properties: This sets what is displayed in the access window.
  long Inx=Index-MSpModelBase::DefinedPropertyCount();
  switch(Inx)
    {
    case idDensityMethod   	: Info.SetText("--Global options---------");
                              Info.SetStructName("Config"); //NB: The "struct name" is used as part of tag!
                              Info.Set(ePT_Long,      "", "DensityMethod",          SVDens,                     MP_GlobalProp|MP_Parameter, "Global density method"); return Inx;
    case idBPEMethod       	: Info.Set(ePT_Long,      "", "BPEMethod",              SVBPE,                      MP_GlobalProp|MP_Parameter, "Global BPE method"); return Inx;
    case idMinSolCp        	: Info.Set(ePT_Double,    "", "MinSolCp",               MC_CpMs,  "kJ/kg.C", 0, 0,  MP_GlobalProp|MP_Parameter|MF_InitHidden, "Global minimum solids Cp"); return Inx;
    case idH2OTestFrac0     : Info.Set(ePT_Double,    "", "WaterTestFrac0",         MC_Frac,  "%",   0, 0,      MP_GlobalProp|MP_Parameter|MP_InitHidden, "Global water fraction above which Standard properties are used"); return Inx;
    case idH2OTestFrac1     : Info.Set(ePT_Double,    "", "WaterTestFrac1",         MC_Frac,  "%",   0, 0,      MP_GlobalProp|MP_Parameter|MP_InitHidden, "Global water fraction above which proportional amounts of two properties models are used"); return Inx;

    case idDefineLiquor			: Info.SetText("--Calculator-------------");
                              Info.SetStructName("Calc"); //NB: The "struct name" is used as part of tag!
                              Info.Set(ePT_Bool,      "",   "DefineLiquor",         MC_,      "",    0, 0,    MP_ConfigProp|MP_Parameter|MP_CheckBox, ""); return Inx;
    case idRqd_A_to_C			  : Info.Set(ePT_Double,    "",   "Rqd_A/C",              MC_,      "",    0, 0,    MP_ConfigProp|MP_Parameter, "The required Alumina/Caustic Ratio");                      return Inx;
    case idRqd_C_to_S			  : Info.Set(ePT_Double,    "",   "Rqd_C/S",              MC_,      "",    0, 0,    MP_ConfigProp|MP_Parameter, "The required Caustic/Soda Ratio");                      return Inx;
    case idRqd_C				    : Info.Set(ePT_Double,    "",   "Rqd_C",                MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Caustic Conc expressed as Carbonate @ 25");                      return Inx;
    case idRqd_Oxalate			: Info.Set(ePT_Double,    "",   "Rqd_Oxalate",          MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Oxalate Concentration @ 25");                      return Inx;
    case idRqd_TOC				  : Info.Set(ePT_Double,    "",   "Rqd_TOC",              MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Total Organic Carbon Conc expressed as Carbon @ 25");                      return Inx;
    case idRqd_Na2SO4			  : Info.Set(ePT_Double,    "",   "Rqd_Na2SO4",           MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Sodium Sulphate Concentration @ 25");                      return Inx;
    case idRqd_NaCl				  : Info.Set(ePT_Double,    "",   "Rqd_NaCl",             MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Sodium Chloride Concentration @ 25");                      return Inx;
      //case idRqd_NaF			  : Info.Set(ePT_Double,    "",   "Rqd_NaF",              MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Sodium Fluoride Concentration @ 25");                      return Inx;
    case idRqd_SiO2				  : Info.Set(ePT_Double,    "",   "Rqd_SiO2",             MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Silica Concentration @ 25");                      return Inx;
    case idRqd_SolConc			: Info.Set(ePT_Double,    "",   "Rqd_SolConc",          MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter|MP_NanOK, "The required Solids Conc");            return Inx;
    case idRqd_SolFrac			: Info.Set(ePT_Double,    "",   "Rqd_SolFrac",          MC_Frac,  "%",   0, 0,    MP_ConfigProp|MP_Parameter, "The required Solids Fraction");                      return Inx;


    case idAluminaConc25		: Info.SetStructName("Props"); //NB: The "struct name" is used as part of tag! Recomend "Props" for compatability with other properties models.
                              Info.SetText("--Concentration and Ratio @ 25,-----------");
                              Info.Set(ePT_Double,    "", "Alumina@25",             MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Alumina Concentration @ 25"); return Inx;
    case idAtoC					    : Info.Set(ePT_Double,    "", "A/C",                    MC_,     "",       0,      0,  MP_Result|MP_NoFiling, "A to C ratio"); return Inx;
    case idCtoS					    : Info.Set(ePT_Double,    "", "C/S",                    MC_,     "",       0,      0,  MP_Result|MP_NoFiling, "C to S ratio"); return Inx;
    case idCltoC				    : Info.Set(ePT_Double,    "", "Cl/C",                   MC_,     "",       0,      0,  MP_Result|MP_NoFiling|MF_InitHidden, "Cl to C ratio"); return Inx;
    case idNa2SO4toC			  : Info.Set(ePT_Double,    "", "SO4/C",                  MC_,     "",       0,      0,  MP_Result|MP_NoFiling|MF_InitHidden, "Na2SO4 to Caustic ratio"); return Inx;
    case idNa2CO3toS			  : Info.Set(ePT_Double,    "", "CO3/S",                  MC_,     "",       0,      0,  MP_Result|MP_NoFiling|MF_InitHidden, "Na2CO3 to Soda ratio"); return Inx;
    case idTOC25				    : Info.Set(ePT_Double,    "", "TOC@25",                 MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Total Organic Carbon Concentration @ 25"); return Inx;
    case idNa2C2O4Conc25		: Info.Set(ePT_Double,    "", "Oxalate*@25",			      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "SodiumOxalate Concentration @ 25"); return Inx;
    case idNaClConc25			  : Info.Set(ePT_Double,    "", "Chloride*@25",			      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "SodiumChloride Concentration @ 25"); return Inx;
    case idNaSO4Conc25			: Info.Set(ePT_Double,    "", "Sulphate*@25",			      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "SodiumSulphate Concentration @ 25"); return Inx;
    case idFreeCaustic25		: Info.Set(ePT_Double,    "", "FreeCaustic@25",         MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling|MF_InitHidden, "Free Caustic Concentration @ 25"); return Inx;
    case idSolidsConc25			: Info.Set(ePT_Double,    "", "SolidsConc@25",          MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Solids Concentration @ 25");return Inx;

    case idSeparator1			  : Info.SetText("..."); return Inx;

    case idCausticConc25		: Info.SetText("--Concentrations @ 25, as Na2CO3 Equiv----");
                              Info.Set(ePT_Double,    "", "Caustic@25",				      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Caustic Concentration @ 25"); return Inx;
    case idSodaConc25			  : Info.Set(ePT_Double,    "", "Soda@25",				        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Soda Concentration @ 25"); return Inx;
    case idCarbonateConc25	: Info.Set(ePT_Double,    "", "Carbonate@25",			      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "SodiumCarbonate Concentration @ 25"); return Inx;
    case idOrganateConc25		: Info.Set(ePT_Double,    "", "Organate@25",			      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Organate Concentration @ 25"); return Inx;
    case idOxalateConc25		: Info.Set(ePT_Double,    "", "Oxalate@25",				      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "SodiumOxalate Concentration @ 25"); return Inx;
    case idTotalOrganics25	: Info.Set(ePT_Double,    "", "TotalOrganics@25",       MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Total Organic Concentration @ 25"); return Inx;
    case idChlorineConc25		: Info.Set(ePT_Double,    "", "Chloride@25",			      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "SodiumChloride Concentration @ 25"); return Inx;
    case idSulphateConc25		: Info.Set(ePT_Double,    "", "Sulphate@25",			      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "SodiumSulphate Concentration @ 25"); return Inx;
    case idTotalNa25			  : Info.Set(ePT_Double,    "", "TotalNa@25",             MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Total Sodium Concentration @ 25"); return Inx;

    case idSeparator2		  	: Info.SetText("..."); return Inx;

    case idLVolume25			  : Info.SetText("--Volume and Density @ 25-----------------");
                              Info.Set(ePT_Double,    "", "LVolume@25",               MC_Qv,   "L/s",    0,      0,  MP_Result|MP_NoFiling, "Liquor Volumetric flowrate @ 25"); return Inx;
    case idSLVolume25			  : Info.Set(ePT_Double,    "", "SLVolume@25",              MC_Qv,   "L/s",    0,      0,  MP_Result|MP_NoFiling, "Slurry Volumetric flowrate @ 25"); return Inx;
    case idLDensity25 		  : Info.Set(ePT_Double,    "", "LDensity@25",              MC_Rho,  "g/L",    0,      0,  MP_Result|MP_NoFiling, "Liquor Density @ 25"); return Inx;
    case idSLDensity25		  : Info.Set(ePT_Double,    "", "SLDensity@25",             MC_Rho,  "g/L",    0,      0,  MP_Result|MP_NoFiling, "Slurry Density @ 25"); return Inx;

    case idSeparator3			  : Info.SetText("..."); return Inx;

    case idAluminaConc			: Info.SetText("--Concentration @ T-----------------------");
                              Info.Set(ePT_Double,    "", "Alumina@T",				        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Alumina Concentration @ T"); return Inx;
    case idSodiumCarbonateConc: Info.Set(ePT_Double,  "", "Carbonate@T",			        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling|MF_InitHidden, "SodiumCarbonate Concentration @ T"); return Inx;
    case idTOC					    : Info.Set(ePT_Double,    "", "TOC@T",                    MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Total Organic Carbon Concentration @ T"); return Inx;
    case idSolidsConc			  : Info.Set(ePT_Double,    "", "SolidsConc@T",             MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Solids Concentration @ T"); return Inx;

    case idSeparator4			  : Info.SetText("..."); return Inx;

    case idCausticConc			: Info.SetText("--Concentration @ T, as Na2CO3 Equiv -----");
                              Info.Set(ePT_Double,    "", "Caustic@T",                MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Caustic Concentration @ T"); return Inx;
    case idSodaConc				  : Info.Set(ePT_Double,    "", "Soda@T",                   MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Soda Concentration @ T"); return Inx;

    case idSeparator5			  : Info.SetText("..."); return Inx;

    case idAluminaConcSat		: Info.SetText("--Other Bayer Liquor Properties @ T-------");
                              Info.Set(ePT_Double,    "", "AluminaSatConc",           MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Alumina Saturation Concentration @ T"); return Inx;
    case idAtoCSaturation		: Info.Set(ePT_Double,    "", "ASat_To_C",                MC_,     "",       0,      0,  MP_Result|MP_NoFiling, "Alumina Saturation to Caustic ratio @ T"); return Inx;
    case idSSNRatio				  : Info.Set(ePT_Double,    "", "SSNRatio",                 MC_,     "",       0,      0,  MP_Result|MP_NoFiling, "A/C actual to ASat/C ratio @ T"); return Inx;
    case idOxalateEquilibrium	: Info.Set(ePT_Double,  "", "OxalateEquilibrium",       MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Oxalate Equilibrium Concentration @ T"); return Inx;

    case idSeparator6			  : Info.SetText("..."); return Inx;

    case idBoilPtElev			  : Info.SetText("--Other-------");
                              Info.Set(ePT_Double,    "", "BoilPtElev",               MC_dT,   "C",      0,      0,  MP_Result|MP_NoFiling, "Boiling Point Elevation"); return Inx;
    case idTHAMassFlow   		: Info.Set(ePT_Double,    "", "THAMassFlow",              MC_Qm,   "kg/s",   0,      0,  MP_Result|MP_NoFiling, "THA flow rate"); return Inx;
    case idTHADens				  : Info.Set(ePT_Double,    "", "THADens",                  MC_Rho,  "kg/m^3", 0,      0,  MP_Result|MP_NoFiling, "THA Density"); return Inx;

    case idMPI_EndOfProps		: return MPI_EndOfProps;    

    default: 
      {
      ASSERT_ALWAYS(FALSE, "Should never get here", __FILE__, __LINE__);
      //char Buff[16];
      //sprintf(Buff, "Unused%d", Inx);
      //Info.Set(ePT_Double, "", Buff, MC_, "", 0, 0, MP_Result|MP_NoFiling, ""); 
      return Inx;
      }
    }
  return MPI_MoreProps;
  }

//---------------------------------------------------------------------------

DWORD CDemoBayerSM::GetPropertyVisibility(long Index)
  {//determine visibility of list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    //case idDefnLiqMethod: 
    case idRqd_A_to_C: 
    case idRqd_C_to_S: 
    case idRqd_C: 
    case idRqd_Oxalate: 
    case idRqd_TOC: 
    case idRqd_Na2SO4: 
    case idRqd_NaCl: 
    //case idRqd_NaF: 
    case idRqd_SiO2: 
    case idRqd_SolConc: 
    case idRqd_SolFrac: return fDoCalc ? ePVis_All : (ePVis_DynFull|ePVis_DynFlow|ePVis_Probal|ePVis_File);
    default:  return ePVis_All; 
    }
  return ePVis_All;
  }

//---------------------------------------------------------------------------

void CDemoBayerSM::GetPropertyValue(long Index, ULONG Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {//define method of retrieving values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    case idDensityMethod    : Value=sm_iDensityMethod;                  return; 
    case idBPEMethod        : Value=sm_iBPEMethod;                      return; 
    case idMinSolCp         : Value=sm_dMinSolCp;                       return; 
    case idH2OTestFrac0     : Value=sm_dH2OTestFrac0;                   return; 
    case idH2OTestFrac1     : Value=sm_dH2OTestFrac1;                   return; 

    case idDefineLiquor			: Value=fDoCalc;                            return; 
    //case idDefnLiqMethod		: Value=sm_iRqdCalcMethod;                  return; 
    case idRqd_A_to_C			  : Value=dRqd_AtoC;                          return;
    case idRqd_C_to_S			  : Value=dRqd_CtoS;                          return;
    case idRqd_C				    : Value=dRqd_C;                             return;
    case idRqd_Oxalate			: Value=dRqd_Ox;                            return;
    case idRqd_TOC				  : Value=dRqd_TOC;                           return;
    case idRqd_Na2SO4			  : Value=dRqd_Sul;                           return;
    case idRqd_NaCl				  : Value=dRqd_Salt;                          return;
    //case idRqd_NaF			    : Value=dRqd_Fl;                            return;
    case idRqd_SiO2				  : Value=dRqd_Sil;                           return;
    case idRqd_SolConc			: Value=dRqd_SolConc;                       return;
    case idRqd_SolFrac			: Value=dRqd_SolFrac;                       return;

    case idTHAMassFlow			: Value=THAMassFlow();                      return; 
    case idTHADens				  : Value=THADens(T);                         return; 
    case idCausticConc			: Value=CausticConc(T);                     return; 
    case idAluminaConc			: Value=AluminaConc(T);                     return; 
    case idSodaConc				  : Value=SodaConc(T);                        return; 
    case idSodiumCarbonateConc : Value=SodiumCarbonateConc(T);             return; 

    case idCausticConc25		: Value=LiqConcs25.Liq[::CausticSoda];      return; 
    case idAluminaConc25		: Value=LiqConcs25.Liq[::Alumina];          return; 
    case idSodaConc25			  : Value=LiqConcs25.Liq[::CausticSoda]+ LiqConcs25.Liq[::SodiumCarbonate]; return; 

    case idCarbonateConc25	: Value=LiqConcs25.Liq[::SodiumCarbonate];  return; 
    case idNaClConc25			  : Value=LiqConcs25.Liq[::SodiumChloride]*::SodiumChloride.MW*2/::SodiumCarbonate.MW;   return; 
    case idNaSO4Conc25			: Value=LiqConcs25.Liq[::SodiumSulphate]*::SodiumSulphate.MW/::SodiumCarbonate.MW;   return; 
    case idNa2C2O4Conc25		: Value=LiqConcs25.Liq[::SodiumOxalate]*::SodiumOxalate.MW/::SodiumCarbonate.MW;    return; 
    case idSolidsConc			  : Value=SolidsConc(T);                      return; 
    case idSolidsConc25			: Value=SolidsConc25();                     return; 
    case idFreeCaustic25    : Value=FreeCaustic(C_2_K(25.0));           return; 
    case idTOC					    : Value=TOC(T);                             return; 
    case idTOC25				    : Value=TOC25();							              return; 

    case idAtoC					    : Value=AtoC();                             return; 
    case idCtoS					    : Value=CtoS();                             return; 
    case idCltoC				    : Value=CltoC();                            return; 
    case idNa2SO4toC			  : Value=Na2SO4toC();                        return; 
    case idNa2CO3toS			  : Value=Na2CO3toS();                        return; 

    case idLVolume25			  : Value=LVolume25();                        return; 
    case idSLVolume25			  : Value=SLVolume25();                       return; 
    case idOrganateConc25		: Value=OrganateConc25();                   return; 
    case idOxalateConc25		: Value=OxalateConc25();                    return; 
    case idTotalOrganics25	: Value=TotalOrganicsConc25();              return; 
    case idChlorineConc25		: Value=ChlorineConc25();                   return; 
    case idSulphateConc25		: Value=SulphateConc25();                   return; 
    case idTotalNa25			  : Value=TotalSodiumConc25();                return; 

    case idAluminaConcSat		: Value=AluminaConcSat(T);                  return; 
    case idOxalateEquilibrium	: Value=OxalateEquilibrium(T);            return; 
    case idAtoCSaturation		: Value=AtoCSaturation(T);                  return; 
    case idSSNRatio				  : Value=SSNRatio(T);                        return; 
    case idBoilPtElev			  : Value=getBoilingPtElevation(P, NULL);     return;//BoilPtElev(MArray(this), T);        return; 

    case idLDensity25			  : Value=LDensity25();                       return; 
    case idSLDensity25			: Value=SLDensity25();                      return; 
		
    default: Value = 0.0;
    }
  }

//---------------------------------------------------------------------------

void CDemoBayerSM::PutPropertyValue(long Index, MPropertyValue & Value)
  {//define method of setting values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    case idDensityMethod  : sm_iDensityMethod=(byte)(long)Value;  return; 
    case idBPEMethod      : sm_iBPEMethod=(byte)(long)Value;      return; 
    case idMinSolCp       : sm_dMinSolCp=Value;                   return; 
    case idH2OTestFrac0   : sm_dH2OTestFrac0=Range(0.0, (double)Value, 1.0); return; 
    case idH2OTestFrac1   : sm_dH2OTestFrac1=Range(0.0, (double)Value, sm_dH2OTestFrac0); return; 

    case idDefineLiquor		: fDoCalc=Value;                        return;
    //case idDefnLiqMethod	: sm_iRqdCalcMethod=(byte)(long)Value;  return;
    case idRqd_A_to_C		  : dRqd_AtoC=Value;                      return;
    case idRqd_C_to_S		  : dRqd_CtoS=Value;                      return;
    case idRqd_C			    : dRqd_C=Value;                         return;
    case idRqd_Oxalate		: dRqd_Ox=Value;                        return;
    case idRqd_TOC			  : dRqd_TOC=Value;                       return;
    case idRqd_Na2SO4		  : dRqd_Sul=Value;                       return;
    case idRqd_NaCl			  : dRqd_Salt=Value;                      return;
    //case idRqd_NaF		  : dRqd_Fl=Value;                        return;
    case idRqd_SiO2			  : dRqd_Sil=Value;                       return;
    case idRqd_SolConc		: dRqd_SolConc=Value;                   return;
    case idRqd_SolFrac		: dRqd_SolFrac=Value;                   return;
    }
  }

//---------------------------------------------------------------------------

void CDemoBayerSM::InputCalcs(bool DoIt, double T_)
  {
  if (DoIt)
    {
    const bool FindingConc=Valid(dRqd_SolConc);

    //Change inputs if out of range
    dRqd_AtoC   = Range(0.0, dRqd_AtoC,    0.9);
    dRqd_C      = Range(0.0, dRqd_C,       9000.0);
    dRqd_CtoS   = Range(0.1, dRqd_CtoS,    1.0);
    dRqd_Sul    = Range(0.0, dRqd_Sul,     500.0);
    dRqd_Sil    = Range(0.0, dRqd_Sil,     500.0);
    dRqd_Salt   = Range(0.0, dRqd_Salt,    500.0);
    //dRqd_Fl     = Range(0.0, dRqd_Fl,      500.0);
    dRqd_Ox     = Range(0.0, dRqd_Ox,      500.0);
    dRqd_TOC    = Range(0.0, dRqd_TOC,     500.0);
    dRqd_SolFrac= Range(0.0, dRqd_SolFrac, 0.99);
    //if (FindingConc)
    //  dRqd_SolConc = Range(0.0, dRqd_SolConc, 5000.0);

    //calculator to determine Liquor feed fractions based on requirements at 25C...
    double DensL = 1100.0;
    double DensS = 2420.0;
    const bool WantSolids=(FindingConc && dRqd_SolConc>1e-6) || (dRqd_SolFrac>1e-6);
    double TotalMass = Mass(MP_SL);
    if (WantSolids)
      {
      if (FindingConc)
        dRqd_SolFrac=Range(0.0, (dRqd_SolConc*DensS)/(dRqd_SolConc*DensS + DensL*DensS - DensL*dRqd_SolConc), 0.99); // Initial Guess
      if (Mass(MP_Sol)<1.0e-6)
        {//assume THA solids required
        Log.Message(MMsg_Note, "Assume solids are THA for Bayer feed calculator");
        M[::THA] = TotalMass*dRqd_SolFrac;
        double Scl = (TotalMass*(1.0-dRqd_SolFrac))/GTZ(Mass(MP_Liq));
        ScaleMass(MP_Liq, Scl);
        TotalMass = Mass(MP_SL);
        }
      }
    else
      {
      dRqd_SolFrac = 0.0;
      }

    //----------------------------------------------
    const double LiqMassCalc = (M[Water] + M[CausticSoda] + M[Alumina] + M[SodiumCarbonate] + M[SodiumChloride] + /*M[SodiumFluoride] +*/
                          M[SodiumSulphate] + M[SilicateLiq] + M[SodiumOxalate] + M[Organics]);
    const double LiqTtl = Mass(MP_Liq);
    const double OtherLiqMass = LiqTtl - LiqMassCalc;
    const double VapMass = Mass(MP_Gas);
    double LiqMass = TotalMass*(1.0-dRqd_SolFrac);
    if (OtherLiqMass>LiqMass && OtherLiqMass>0.0)
      {
      if (dRqd_SolFrac>0.0)
        Log.Message(MMsg_Warning, "Invalid Bayer feed calculator: large solids plus other liquids required");
      else
        Log.Message(MMsg_Warning, "Invalid Bayer feed calculator: large other liquids required");
      }
    if (LiqMass-OtherLiqMass > 1e-6)  // There are liquids required in the feed.
    //if (TotalMass>1e-6 && (FindingConc?(dRqd_SolConc>=0):(dRqd_SolFrac<1-1e-6)))  // There are liquids in the feed.
      {
      double WaterUsed = 0.0;
      const double P = Pressure;
      for (bool Done=false; !Done; )
        {
        dRqd_SolFrac=Range(0.0, dRqd_SolFrac, 0.99);
        LiqMass = TotalMass*(1.0-dRqd_SolFrac);

        double Density25;

        double Al, Caustic, Carbonate, Salt, NaSulphate, Silicate;//, NaFluoride;
        double water, Derr, Org1, Org2;
        double Ox, Og, Other;

        const double MW_Na2C2O4   = ::SodiumOxalate.MW;    //133.999
        const double MW_Na2C5O7   = ::Organics.MW;         //218.030
        const double minTOC = dRqd_Ox*2.0/MW_Na2C2O4*MW_C;
        double TocUsed = dRqd_TOC;
        if (dRqd_TOC<minTOC)
          {
          //LogWarning(FullObjTag(), 0, "Required TOC too low based on specified Oxalate");
          TocUsed = minTOC; //this will give Og of zero
          }
        Ox = dRqd_Ox;
        //Ox_ = Ox*MW_MW_Na2CO3/MW_Na2C2O4 //Oxalate conc as carbonate      
        //Og_ = Og*MW_MW_Na2CO3/MW_Na2C5O7        
        //TOC = (Og_*5 + Ox_*2)*MW_C/MW_Na2CO3
        //==>TOC = (Og/MW_Na2C5O7*5 + Ox/MW_Na2C2O4*2)*MW_C
        //==>Og = (TOC/MW_C - Ox/MW_Na2C2O4*2)*MW_Na2C5O7/5
        Og = (TocUsed/MW_C - dRqd_Ox*2.0/MW_Na2C2O4)*MW_Na2C5O7/5.0;

        if (dRqd_C > 1e-6)
          {
          const double MW_NaOH      = ::CausticSoda.MW;      //39.9971080
          const double MW_Na2CO3    = ::SodiumCarbonate.MW;  //105.989
          const double MW_NaCl      = ::SodiumChloride.MW;   //58.4425
          const double MW_Na2SO4    = ::SodiumSulphate.MW;   //142.043
          //const double MW_NaF       = ::SodiumFluoride.MW;   //
          const double MW_Na2O      = ::OccSoda.MW;          //61.9789360
          // Calculate density based on factors.
          double Lm = LiqMass;
          double Dens1 = DensL/1000.0;//1.100;  // First guess at density
          double Vol, Tal, Tna, Tcc;
          for (int j=0; j<100; j++)
            {
            Vol = Lm/Dens1;

            switch (sm_iDensityMethod)
              {
              case BDM_Original:
                {
                Tna = (dRqd_C/dRqd_CtoS + (Ox/MW_Na2C2O4 + Og/MW_Na2C5O7 + dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4/* + dRqd_Fl/(2*MW_NaF)*/) * MW_Na2CO3)
                  * Vol/(1000.0*Lm)*100.0; //Wgt % of Total Na
                Tcc = dRqd_C/(Dens1*1000.0)*100.0;//Wgt % of Caustic
                Tal = dRqd_AtoC * Tcc; //Wgt % of Alumina
                Density25 = CBayerConcs::LiquorDensEqn1(25.0, Tna, Tal)/1000.0;
                break;
                }
              case BDM_Other:
                {
                Tna = (dRqd_C/dRqd_CtoS + (Ox/MW_Na2C2O4 + Og/MW_Na2C5O7 + dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4/* + dRqd_Fl/(2*MW_NaF)*/) * MW_Na2CO3)
                  * Vol/(1000.0*Lm)*100.0; //Wgt % of Total Na
                Tcc = dRqd_C/(Dens1*1000.0)*100.0;//Wgt % of Caustic
                Tal = dRqd_AtoC * Tcc; //Wgt % of Alumina
                Density25 = CBayerConcs::LiquorDensEqn1(25.0, Tna, Tal)/1000.0;
                break;
                }
              }

            Derr = fabs(Dens1 - Density25);
            if ( Derr < 1e-10)
              j = 100;
            else
              Dens1 = Density25;
            }

          //---------------------------------------------------
          // Now calculate Species as fractions based on density
          Caustic    = dRqd_C * 2.0*MW_NaOH/MW_Na2CO3 * Vol; //dRqd_C * 0.75474 * Vol;
          Al         = dRqd_AtoC * dRqd_C * Vol;
          Carbonate  = (dRqd_C/dRqd_CtoS - dRqd_C) * Vol;
          Salt       = dRqd_Salt * Vol; //NaCl
          //NaFluoride = dRqd_Fl * Vol;
          NaSulphate = dRqd_Sul * Vol;
          Silicate   = dRqd_Sil * Vol;
          Org1       = dRqd_Ox * Vol;
          Org2       = Og * Vol;
          Other      = OtherLiqMass/LiqMass*Vol*Dens1*1000.0;

          double TmpTot = Org1 + Org2 + NaSulphate + /*NaFluoride + */Silicate + Salt + Carbonate + Al + Caustic;
          TmpTot += Other;
          double Lmass = Vol * Density25 * 1000.0;
          if (TmpTot <= Lmass)
            {
            water  = Lmass - TmpTot;
            TmpTot += water;
            }
          else
            {
            water = 0.0;
            //Do not want Other to be adjusted
            TmpTot = (TmpTot-Other)/(LiqMass-OtherLiqMass)*LiqMass;
            }

          // Normalise the fractions
          Caustic    /= TmpTot;
          Al         /= TmpTot;
          Carbonate  /= TmpTot;
          Salt       /= TmpTot;
          //NaFluoride /= TmpTot;
          NaSulphate /= TmpTot;
          Silicate   /= TmpTot;
          Org1       /= TmpTot;
          Org2       /= TmpTot;
          water      /= TmpTot;

          //-------------------------------------------------------

          // Set masses of the liquid species.
          M[CausticSoda]=Caustic*LiqMass;
          M[Alumina]=Al*LiqMass;
          M[SodiumCarbonate]=Carbonate*LiqMass;
          M[SodiumChloride]=Salt*LiqMass;
          //M[SodiumFluoride]=NaFluoride*LiqMass;
          M[SodiumSulphate]=NaSulphate*LiqMass;
          M[SilicateLiq]=Silicate*LiqMass;
          M[SodiumOxalate]=Org1*LiqMass;
          M[Organics]=Org2*LiqMass;
          M[Water]=water*LiqMass;
          }
        else
          {
          // Default to water
          M[Water]=GEZ(LiqMass-OtherLiqMass);
          M[CausticSoda]=0.0;
          M[Alumina]=0.0;
          M[SodiumCarbonate]=0.0;
          M[SodiumChloride]=0.0;
          //M[SodiumFluoride]=0.0;
          M[SodiumSulphate]=0.0;
          M[SilicateLiq]=0.0;
          M[SodiumOxalate]=0.0;
          M[Organics]=0.0;
          }
        WaterUsed=M[Water];

        if (FindingConc)
          {
          double PrevSF=dRqd_SolFrac;
          DensL=Density25*1000.0;
          DensS=MSpModelBase::get_Density(MP_Sol, C_2_K(25.0), P, NULL);
          dRqd_SolFrac=(dRqd_SolConc*DensS)/(dRqd_SolConc*DensS + DensL*DensS - DensL*dRqd_SolConc);
          if (fabs(PrevSF-dRqd_SolFrac)<1e-9)
            Done=true;
          }
        else
          Done=true;
        }// while
      if (WaterUsed<1.0e-9)
        Log.Message(MMsg_Warning, "Bayer feed calculator water set to zero");
      }

    //--------------------------------------------
    double MSol=Mass(MP_Sol);
    if (WantSolids && MSol>1.0e-6)
      {
      double MOther=Mass(MP_SL)-MSol;
      double Scl=dRqd_SolFrac*MOther/(MSol*(1.0-dRqd_SolFrac));
      ScaleMass(MP_Sol, Scl);
      Log.ClearCondition(4);
      }
    else
      {
      // Set solid fractions to zero.
      ScaleMass(MP_Sol, 0.0);
      Log.SetCondition(WantSolids, 4, MMsg_Warning, "Solids Required");
      }
    }
  else
    Log.ClearCondition(4);
  }

//---------------------------------------------------------------------------

  
double CDemoBayerSM::CausticConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[::CausticSoda] * DRatio);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::AluminaConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[::Alumina]*DRatio);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::SodaConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  double liq25S = LiqConcs25.Liq[::CausticSoda]
                  //+ LiqConcs25.Liq[::SodiumAluminate]
                  + LiqConcs25.Liq[::SodiumCarbonate];

  return Max(0.0, liq25S * DRatio);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::SodiumCarbonateConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[SodiumCarbonate]*DRatio);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::SodiumSulphateConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[SodiumSulphate]*DRatio);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::SodiumOxalateConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[SodiumOxalate]*DRatio);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::SolidsConc(double T_)
  {
  const double mt=Mass(MP_Sol);
  return (mt>=UsableMass) ? mt/GTZ(Mass(MP_SL))*get_Density(MP_SL, T_, Pressure, NULL) : 0.0;
  }
double CDemoBayerSM::SolidsConc25()
  {
  const double mt=Mass(MP_Sol);
  return (mt>=UsableMass) ? mt/GTZ(Mass(MP_SL))*get_Density(MP_SL, C_2_K(25.0), Pressure, NULL) : 0.0;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::TOC(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  double Toc = (LiqConcs25.Liq[::Organics]*5.0 + LiqConcs25.Liq[::SodiumOxalate]*2.0)*MW_C/::SodiumCarbonate.MW;
  return Max(0.0, Toc*DRatio);
  }
double CDemoBayerSM::TOC25()
  {
  LiqConcs25.Converge(MArray(this));
  return (LiqConcs25.Liq[::Organics]*5.0 + LiqConcs25.Liq[::SodiumOxalate]*2.0)*MW_C/::SodiumCarbonate.MW;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::FreeCaustic(double T_)
  {
  return CausticConc(T_) * (1.0 - AtoC()*::SodiumCarbonate.MW/::Alumina.MW);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::AtoC()
  {
  LiqConcs25.Converge(MArray(this));
  return LiqConcs25.Liq[::Alumina] / GTZ(LiqConcs25.Liq[::CausticSoda]);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::CtoS()
  {
  LiqConcs25.Converge(MArray(this));
  return LiqConcs25.Liq[::CausticSoda]
         / GTZ(LiqConcs25.Liq[::CausticSoda] + LiqConcs25.Liq[::SodiumCarbonate]);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::CltoC()
  {
  LiqConcs25.Converge(MArray(this));
  return (LiqConcs25.Liq[SodiumChloride])/GTZ(LiqConcs25.Liq[::CausticSoda]);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::Na2SO4toC()
  {
  LiqConcs25.Converge(MArray(this));
  return (LiqConcs25.Liq[SodiumSulphate])/GTZ(LiqConcs25.Liq[::CausticSoda]);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::THAMassFlow()
  {
  MArray M(this);
  return M[::THA];
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::THADens(double T_)
  {
  return gs_MVDefn[::THA].Density(T_, StdP, NULL);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::IonicStrength()
  {
  const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989..
  const double MW_Na2O     = ::OccSoda.MW;          //61.9789..
  const double MW_NaCl     = ::SodiumChloride.MW;   //58.4425..
  const double MW_Na2SO4   = ::SodiumSulphate.MW;   //142.043..
  LiqConcs25.Converge(MArray(this));
  double I;
  /* Saturated Alumina concentration (ie Alumina concentration at Equilibrium)
  calculated using the formula found in:
  Fourth International Alumina Quality Workshop  June 1996 Proceedings
  Rosenberg S.P, Healy S.J 'A Thermodynamic Model for Gibbsite Solubility
  in Bayer Liquors'.*/
  const double k1 = 0.9346;
  const double k2 = 2.0526;
  const double k3 = 2.1714;
  const double k4 = 1.6734;
  const double cTOC    = TOC(C_2_K(25.0));
  const double cNaCl   = LiqConcs25.Liq[::SodiumChloride] /LiqConcs25.NaFactor[::SodiumChloride];
  const double cNa2CO3 = LiqConcs25.Liq[::SodiumCarbonate]/LiqConcs25.NaFactor[::SodiumCarbonate];
  const double cNa2SO4 = LiqConcs25.Liq[::SodiumSulphate] /LiqConcs25.NaFactor[::SodiumSulphate];
  const double cNaOH   = LiqConcs25.Liq[::CausticSoda];
  I = 0.01887*cNaOH + k1*cNaCl/MW_NaCl + k2*cNa2CO3/MW_Na2CO3 + k3*cNa2SO4/MW_Na2SO4 + k4*0.01887*cTOC;
  return I;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::AluminaConcSat(double T_)
  {
  const double I = IonicStrength();
  const double I2 = Sqrt(I);
  const double cNaOH   = LiqConcs25.Liq[::CausticSoda];
  // Saturated Alumina concentration (ie Alumina concentration at Equilibrium)
  //calculated using the formula found in:
  //Fourth International Alumina Quality Workshop  June 1996 Proceedings
  //Rosenberg S.P, Healy S.J 'A Thermodynamic Model for Gibbsite Solubility
  //in Bayer Liquors'.
  const double a0 = -9.2082;
  const double a3 = -0.8743;
  const double a4 = 0.2149;
  const double dG = -30960.0; //Gibbs energy of dissolution
  const double R  = R_c;//8.3145; //Universal Gas Constant 8.314472
  const double X    = a0*I2/(1.0+I2) - a3*I - a4*Pow(I, 3.0/2.0);
  const double Keq  = Exps(dG/(R*T_)); //Equilibrium Constant
  const double ASat = 0.96197*cNaOH/(1.0+(Pow(10.0, X)/Keq));
  return ASat;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::OxalateEquilibrium(double T_)
  {
  // Function calculates the equilibrium concentration (g/l) of oxalate, based on stream properties.
  // (British Aluminium - Burnt Island)
  const double C        = CausticConc(C_2_K(25.0)); // g/l
  const double Na2CO3Qm = M[SodiumCarbonate]; // kg/s
  const double LiqQv    = LVolume25(); // m^3/s
  const double OxEquil = 7.62*Exps(0.012*K2C(T_) - (::OccSoda.MW/::SodiumCarbonate.MW)*(0.016*C + 0.011*Na2CO3Qm/GTZ(LiqQv)));
  return OxEquil; //(OxEquil+Approach);
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double CDemoBayerSM::LVolume25()
  {
  const double mt=Mass(MP_Liq);
  return ((mt>=UsableMass) ? (mt / get_Density(MP_Liq, C_2_K(25.0), Pressure, NULL)) : 0.0);
  }

double CDemoBayerSM::SLVolume25()
  {
  const double mt=Mass(MP_SL);
  return ((mt>=UsableMass) ? (mt / get_Density(MP_SL, C_2_K(25.0), Pressure, NULL)) : 0.0);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::LDensity25()
  {
  return (get_Density(MP_Liq, C_2_K(25.0), Pressure, NULL));
  }

double CDemoBayerSM::SLDensity25()
  {
  return (get_Density(MP_SL, C_2_K(25.0), Pressure, NULL));
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double CDemoBayerSM::AtoCSaturation(double T_)
  {
  double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AluminaConcSat(T_) / GTZ(C);
  return ACsat;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::SSNRatio(double T_)
  {// AtoC actual / AtoC saturation ==> A/ASat
  const double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AtoCSaturation(T_);
  return AtoC() / GTZ(ACsat);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::TotalSodiumConc25()
  {//TotalNa Conc @ 25
  LiqConcs25.Converge(MArray(this));
  double Sodium = LiqConcs25.Liq[::CausticSoda]       // NaOH
                  + LiqConcs25.Liq[::SodiumCarbonate] // Na2CO3
                  + LiqConcs25.Liq[SodiumOxalate]     // Na2C2O4
                  + LiqConcs25.Liq[Organics]          // Na2C5O7
                  + LiqConcs25.Liq[SodiumChloride]    // NaCl
                  + LiqConcs25.Liq[SodiumSulphate];   // Na2SO4
                  //+ LiqConcs25.Liq[SodiumFluoride];   // NaF
  return Sodium;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::TotalSodium()//SpMArray * pMA)
  {
  MArray MA(this);
  LiqConcs25.Converge(MA);
  double TSodium = LiqConcs25.LTotalSodium(MA); // The sodium components in liq.
  return TSodium;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::Na2CO3toS()
  {
  LiqConcs25.Converge(MArray(this));
  return LiqConcs25.Liq[::SodiumCarbonate]
         / GTZ(LiqConcs25.Liq[::CausticSoda] + LiqConcs25.Liq[::SodiumCarbonate]);
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::OrganateConc25()
  {//Organic Na2C5O7 Conc @ 25
  LiqConcs25.Converge(MArray(this));
  double Organate = LiqConcs25.Liq[::Organics];
  return Organate;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::OxalateConc25()
  {
  LiqConcs25.Converge(MArray(this));
  double Oxalate = LiqConcs25.Liq[::SodiumOxalate];
  return Oxalate;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::TotalOrganicsConc25()
  {
  LiqConcs25.Converge(MArray(this));
  double Organics = LiqConcs25.Liq[::Organics] + LiqConcs25.Liq[::SodiumOxalate];
  return Organics;
  }
//---------------------------------------------------------------------------

double CDemoBayerSM::ChlorineConc25()
  {
  LiqConcs25.Converge(MArray(this));
  double Chlorine = LiqConcs25.Liq[::SodiumChloride];
  return Chlorine;
  }

//---------------------------------------------------------------------------

double CDemoBayerSM::SulphateConc25()
  {
  LiqConcs25.Converge(MArray(this));
  double Sulphate = LiqConcs25.Liq[::SodiumSulphate];
  return Sulphate;
  }

//---------------------------------------------------------------------------




