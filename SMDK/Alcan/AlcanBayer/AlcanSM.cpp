//==============================================================================
//=== SysCAD SMDK - Alcan Bayer Properties Model 2004, 2006 : Alcan, Kenwalt ===
//==============================================================================
#include "stdafx.h"
#include "AlcanSM.h"
#include "math.h"
#include "md_headers.h"
#pragma comment(lib, "rpcrt4.lib")

#define DebugOn 0  //set this to 1 for debug testing

#if DebugOn
#pragma optimize("", off)
#endif
// ==========================================================================
//
//
//
// ==========================================================================
                                 
DEFINE_SPECIEMODEL(CAlcanSM)
IMPLEMENT_SPECIEMODEL(CAlcanSM, "AlcanSM", "*Alcan Bayer", "*Alcan Bayer Specie Model", DLL_GroupName)

// ==========================================================================
// Specie Access
static MInitialiseTest InitTest("CAlcanSMClass");
MSpeciePtr   Water             (InitTest, "H2O(l)",        false);
MSpeciePtr   Alumina           (InitTest, "Al2O3(l)",      false);
MSpeciePtr   CausticSoda       (InitTest, "NaOH(l)",       false);
MSpeciePtr   SodiumChloride    (InitTest, "NaCl(l)",       false);
MSpeciePtr   SodiumSulphate    (InitTest, "Na2SO4(l)",     false);
MSpeciePtr   SodiumCarbonate   (InitTest, "Na2CO3(l)",     false);
MSpeciePtr   SodiumFluoride    (InitTest, "NaF(l)",        false);
MSpeciePtr   SodiumOxalate     (InitTest, "Na2C2O4(l)",    false); //organic
MSpeciePtr   Organics          (InitTest, "Na2C5O7(l)",    false); //organic
MSpeciePtr   Organics1         (InitTest, "Na2C5O11(l)",   false); //organic
MSpeciePtr   SilicateLiq       (InitTest, "SiO2(l)",       false);
MSpeciePtr   DiphosphorusLiq   (InitTest, "P2O5(l)",       false);

MSpeciePtr   OccSoda           (InitTest, "Na2O(s)",       false);
MSpeciePtr   THA               (InitTest, "Al2O3.3H2O(s)", false);
MSpeciePtr   SodiumOxalateSol  (InitTest, "Na2C2O4(s)",    false);
//MSpeciePtr   AluminaSolid      (InitTest, "Al2O3(s)",      false);

MSpeciePtr   Steam             (InitTest, "H2O(g)",        true);

// ==========================================================================
// Specie Properties Modal Global Parameters

const double MW_C        = 12.011;

enum DLM_DefineLiquorMethod { DLM_TotOrganics, DLM_TOC };
enum ASM_AlSolubilityMethod { ASM_Original, ASM_March2002, ASM_May2003, ASM_Hyprod };
enum CPM_HeatCapacityMethod { CPM_Original, CPM_Hyprod, CPM_Langa };
enum BDM_BayerDensityMethod { BDM_Original, BDM_March2002, BDM_July2003, BDM_MD };
enum GRM_GrowthRateMethod { GRM_Original, GRM_Nov2003, GRM_Hyprod };
enum BPM_BPEMethod { BPM_Dewey, BPM_Adamson };

double CAlcanSM::sm_dMinSolCp    = 1.046;
double CAlcanSM::sm_dH2OTestFrac0  = 0.98;
double CAlcanSM::sm_dH2OTestFrac1  = 0.96;
byte CAlcanSM::sm_iASatMethod    = ASM_March2002;//ASM_May2003;
byte CAlcanSM::sm_iDensityMethod = BDM_March2002;//BDM_July2003;
byte CAlcanSM::sm_iGrowthMethod  = GRM_Nov2003;
byte CAlcanSM::sm_iBPEMethod     = BPM_Dewey;
byte CAlcanSM::sm_iCPMethod      = CPM_Original;
double CAlcanSM::sm_Morphology   = 1.0;
double CAlcanSM::sm_Nucleation   = 0.01;
double CAlcanSM::sm_Na2OFactor   = 1.0;
double CAlcanSM::sm_CarbonFactor = 2.0;

// ==========================================================================
//
// A helper class to assist with calculating iterative concentration and density
// calculations.
//
// ==========================================================================

bool   CAlcanConcs::NaFactorOK=0;
MArray CAlcanConcs::NaFactor;//(0.0);

CAlcanConcs::CAlcanConcs(CAlcanSM *pMdl)
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
    NaFactor[::SodiumFluoride]  = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumFluoride.MW);   
    //NaFactor[::SodiumSilicate]  = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumSilicate.MW);
    NaFactor[::Organics]        = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::Organics.MW);         
    NaFactor[::Organics1]       = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::Organics1.MW);        

    NaFactorOK = true;
    }
  }

// --------------------------------------------------------------------------

void CAlcanConcs::Zero()
  {
  Density25 = 1100.0;
  for (int sn=0; sn<gs_MVDefn.Count(); sn++)
    Liq[sn] = 0.0;
  }

// --------------------------------------------------------------------------

bool CAlcanConcs::Converge(MArray & MA)
  {
  double TLiq = MA.Mass(MP_Liq);
  if (TLiq<1.0e-9)
    {
    Zero();
    return true;
    }

  static MToleranceBlock s_Tol(TBF_Both, "AlcanSM:Density", 1e-12, 1e-8, 100);

  const double Tc = 25.0;
  const double T_ = C2K(Tc);
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
  return (IterCount>=0);
  }

//---------------------------------------------------------------------------

double CAlcanConcs::LTotalSodium(MArray & MA)
  {
  //Expressed as Na2CO3
  double TSodium =
         ( MA[CausticSoda]    * NaFactor[::CausticSoda]
         + MA[SodiumCarbonate]
         + MA[SodiumOxalate]  * NaFactor[::SodiumOxalate]
         + MA[SodiumChloride] * NaFactor[::SodiumChloride]
         + MA[SodiumSulphate] * NaFactor[::SodiumSulphate]
         + MA[SodiumFluoride] * NaFactor[::SodiumFluoride]
         + MA[Organics]       * NaFactor[::Organics]
         + MA[Organics1]      * NaFactor[::Organics1] );
         //+ MA[SodiumSilicate] * NaFactor[::SodiumSilicate] );
  return TSodium;
  }

// --------------------------------------------------------------------------

double CAlcanConcs::LiquorDensEqn1(double Tc, double WTNa, double WAlumina)
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

double CAlcanConcs::LiquorDensEqn2(double Tc, double XNaC, double XNa, double XAl)
  {
  return (1.0+1.71*XNaC) * (1006.0+(1200.0+872.0*XNa-0.291*Tc)*XNa
                   +(1060.0-0.664*Tc)*XAl-(0.242+0.00238*Tc)*Tc);
  }

// --------------------------------------------------------------------------

double CAlcanConcs::LiquorDensEqn3(double Tc, double XAl2O3, double XCaust, double XTTS, double XOC)
  {
  double RHO = 1.02767 - 0.0007015008*Tc - 0.0000007140195*Tc*Tc +
               0.0003631629*XAl2O3 + 0.0000003396664*XAl2O3*XAl2O3 -
               0.00005931031*XCaust +
               0.000794539*XTTS - 0.0000002692805*XTTS*XTTS +
               0.002644698*XOC + 0.0000003134875*Tc*XCaust + 0.04716374*XAl2O3/GTZ(XCaust); // Density at T
  return (RHO * 1000.0); // kg/m^3
  }

// --------------------------------------------------------------------------

double CAlcanConcs::LiquorDensity(double T_, MArray & MA)
  {
  /*Liquor Density with mass fractions*/
  double TLiq=MA.Mass(MP_Liq); //Total Liquid
  if (TLiq<1.0e-9)
    return 0.0;

  double Tc = K2C(T_);
  switch (CAlcanSM::sm_iDensityMethod)
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
    case BDM_March2002:
      {
/*Liquor Density with mass fractions
XAl  = mass fraction AL2O3 aq
XNa  = mass fraction soda as Na2O
XNaC = mass fraction non caustic soda species as Na2O
Temp in degrees C
double LiqDenMassFrc(double TempC, double XNa2O, double XAl, double XNaC)
  {
  double XNa = XNaC + XNa2O;
  double RHO = (1+1.71*XNaC)*(1006+(1200+872*XNa-0.291*TempC)*XNa
             +(1060-0.664*TempC)*XAl-(0.242+0.00238*TempC)*TempC);
  return RHO;
  }*/
      const double MW_Na2O      = ::OccSoda.MW;          //61.9789360
      const double MW_Na2CO3    = ::SodiumCarbonate.MW;  //105.989
      double XAl   = MA[::Alumina]/TLiq; //fraction of Al
      double TNa   = LTotalSodium(MA)*MW_Na2O/MW_Na2CO3;
      double TCaus = MA[::CausticSoda]*NaFactor[::CausticSoda]*MW_Na2O/MW_Na2CO3;
      double XNaC  = (TNa-TCaus)/TLiq; //fraction of (TotalNa less Caustic)
      double XNa   = TNa/TLiq; //fraction of TotalNa
      double RHO = LiquorDensEqn2(Tc, XNaC, XNa, XAl);
      return RHO;
      }
    case BDM_July2003:
      {
      const double MW_Na2O      = ::OccSoda.MW;          //61.9789360
      const double MW_Na2CO3    = ::SodiumCarbonate.MW;  //105.989
      const double MW_Na2SO4    = ::SodiumSulphate.MW;   //142.043
      const double MW_NaCl      = ::SodiumChloride.MW;   //58.4425
      const double MW_NaOH      = ::CausticSoda.MW;      //39.9971080
      const double MW_Na2C2O4   = ::SodiumOxalate.MW;    //133.999
      const double MW_Na2C5O7   = ::Organics.MW;         //218.030
      const double MW_Na2C5O11  = ::Organics1.MW;        //282.028

      //as concentrations...
      double Al2O3_c   = Liq[::Alumina];
      double NaCl_c    = Liq[::SodiumChloride];
      double Na2CO3_c  = Liq[::SodiumCarbonate];
      double Na2SO4_c  = Liq[::SodiumSulphate];
      double Caust_c   = Liq[::CausticSoda];
      double TTS_c     = Na2CO3_c + Caust_c + NaCl_c + Na2SO4_c;
      double OC_c      = (Liq[::Organics]*5.0 + Liq[::SodiumOxalate]*2.0 + Liq[::Organics1]*5.0)*MW_C/MW_Na2CO3; //ie TOC
      double RHO = LiquorDensEqn3(Tc, Al2O3_c, Caust_c, TTS_c, OC_c);
      return RHO;
      }
    case BDM_MD:
      {
/*Implemented for comparison to Alcan Gove Malloy Donaldson model. 
This is a copy of BDM_March2002 with change as per information from Catherine Venz Jan 2005.
In MD the caustic soda (as NaOH) is subtracted from the total soda (as Na2O).   
In SysCAD the caustic soda (as Na2O) is subtracted from the total soda (as Na2O). 
*/
      const double MW_Na2O      = ::OccSoda.MW;          //61.9789360
      const double MW_Na2CO3    = ::SodiumCarbonate.MW;  //105.989
      double XAl   = MA[::Alumina]/TLiq; //fraction of Al
      double TNa   = LTotalSodium(MA)*MW_Na2O/MW_Na2CO3;
      double TCaus = MA[::CausticSoda];//THIS IS THE CHANGE
      double XNaC  = (TNa-TCaus)/TLiq; //fraction of (TotalNa less Caustic)
      double XNa   = TNa/TLiq; //fraction of TotalNa
      double RHO = LiquorDensEqn2(Tc, XNaC, XNa, XAl);
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

CAlcanSM::CAlcanSM(TaggedObject * pNd) : LiqConcs25(this)
  {
  MInitialise(); // this must be called to initialise the SMDK library if neccessary

  fDoCalc  = false;
  m_bRqdAt25 = true;
  dRqd_AtoC = 0.4;
  dRqd_C    = 220.0;
  dRqd_CtoS = 0.85;
  dRqd_Sul      = 2.0;//20.0;
  dRqd_Sil      = 0.5;//1.0;
  //dRqd_Org      = 25.0;//50.0;
  //dRqd_OrgRatio = 0.14;//0.9;
  dRqd_Salt     = 5.0;//15.0;
  dRqd_Fl       = 0.5;//5.0;
  dRqd_P2O5     = 0.5;//4.0;
  dRqd_Ox       = 3.5;
  dRqd_TOC      = 13.0;
  dRqd_SolFrac  = 0.0;
  dRqd_SolConc  = dNAN;
  dRqd_SolOx    = 0.0;
  dRqd_SolOccSoda = 0.003;
  //Dbg.PrintLn("CAlcanSM::CAlcanSM         : %08x", this);
  }

//---------------------------------------------------------------------------

CAlcanSM::~CAlcanSM()
  {
  //Dbg.PrintLn("CAlcanSM::~CAlcanSM        : %08x", this);
  }

//---------------------------------------------------------------------------

void CAlcanSM::BuildDataFields()
  { 
  };              

//---------------------------------------------------------------------------

bool CAlcanSM::ExchangeDataFields()
  {
  return false;
  };

//---------------------------------------------------------------------------

bool CAlcanSM::ValidateDataFields()
  {
  bool OK=MSpModelBase::ValidateDataFields();
  /*if (fDoCalc)
    {    
    dRqd_AtoC = ValidateRange(VDB, "dRqd_AtoC", 0.0, dRqd_AtoC,    0.9);
    dRqd_C    = ValidateRange(VDB, "dRqd_C", 0.0, dRqd_C,       9000.0);
    dRqd_CtoS = ValidateRange(VDB, "dRqd_CtoS", 0.1, dRqd_CtoS,    1.0);
    dRqd_Sul  = ValidateRange(VDB, "dRqd_Sul", 0.0, dRqd_Sul,     500.0);
    dRqd_Sil  = ValidateRange(VDB, "dRqd_Sil", 0.0, dRqd_Sil,     500.0);
    dRqd_Salt = ValidateRange(VDB, "dRqd_Salt", 0.0, dRqd_Salt,   500.0);
    dRqd_Fl   = ValidateRange(VDB, "dRqd_Fl", 0.0, dRqd_Fl,       500.0);
    dRqd_P2O5 = ValidateRange(VDB, "dRqd_P2O5", 0.0, dRqd_P2O5,   500.0);
    dRqd_Ox   = ValidateRange(VDB, "dRqd_Ox", 0.0, dRqd_Ox,       500.0);
    dRqd_TOC  = ValidateRange(VDB, "dRqd_TOC", 0.0, dRqd_TOC,     500.0);
    dRqd_SolFrac  = ValidateRange(VDB, "dRqd_SolFrac", 0.0, dRqd_SolFrac,  0.99);
    dRqd_SolConc  = ValidateRange(VDB, "dRqd_SolConc", 0.0, dRqd_SolConc,  2400.0);
    dRqd_SolOx    = ValidateRange(VDB, "dRqd_SolOx", 0.0, dRqd_SolOx, 0.95);
    dRqd_SolOccSoda = ValidateRange(VDB, "dRqd_SolOccSoda", 0.0, dRqd_SolOccSoda, 0.95);
    }*/
  InputCalcs(fDoCalc, Temperature);
  return OK;
  }

//---------------------------------------------------------------------------

bool CAlcanSM::get_IsBaseClassOf(LPCTSTR OtherProgID)
	{
  return false;
	}

//---------------------------------------------------------------------------

LPCTSTR CAlcanSM::get_PreferredModelProgID()
	{
  return NULL;
  }

//---------------------------------------------------------------------------

double CAlcanSM::get_Density(long Phases, double T, double P, MArray * pMA)
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
  //SMFnRanges.SetInRange(eSpFn_Rho, true);

  return DensityMix(FSol, dNAN, FLiq, Dl, (1.0-FSol-FLiq), dNAN, T, P, MA);
  }

//---------------------------------------------------------------------------

double CAlcanSM::LiqCpCalc(MArray & MA, double T)
  {     
  switch (sm_iCPMethod)
    {
    case CPM_Original:  
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
      return Cpl;
      }
    case CPM_Hyprod:
      {
      /*function TComposition.GetLiqCp:real;
      // gives Cp in Kcal/Kg
      var tts,temp:real;
      BEGIN
	      TTS:= cCarb + cCaus;
	      //Vaud.Tech.Data Book 4.1.2
	      If cCaus>0 then
		      result:= ( 3870.9 +2.79145*temperature -7.07695E-3*SQR(temperature)
		      -1.33106*cAlum +4.88062e-4*SQR(cAlum) +3.3998*cCaus -4.5685e-4*SQR(cCaus)
		      -4.17734*TTS -1.84963E-3*temperature*cCaus ) / 4186
	      else  // water
	      begin   // ref Chem.Eng. nov 91 p236
		      temp := ( temperature*1.8 + 32 );// convert to F
		      result:=  ( 1.0152 -3.6171E-4*temp +2.1709E-6*SQR(temp)
				      -2.9831e-9*power(temp,3));
	      end;
      END;*/
      double Cpl;
      LiqConcs25.Converge(MA);
      const double CCaus = LiqConcs25.Liq[::CausticSoda];//CausticConc(C2K(25.0));
      if (CCaus>0.0)
	      {
        const double CCarb = LiqConcs25.Liq[::SodiumCarbonate];//SodiumCarbonateConc(C2K(25.0));
        const double CAlum = LiqConcs25.Liq[::Alumina];//AluminaConc(C2K(25.0));
	      const double TTS   = CCarb + CCaus;
	      const double Tc    = K2C(T);
	      //Vaud.Tech.Data Book 4.1.2
	      Cpl = (3870.9 + 2.79145*Tc - 7.07695E-3*Tc*Tc
		      - 1.33106*CAlum + 4.88062e-4*CAlum*CAlum 
		      + 3.3998*CCaus - 4.5685e-4*CCaus*CCaus
		      - 4.17734*TTS - 1.84963E-3*Tc*CCaus ) / 4186.0;
	      Cpl = KCal_2_kJ(Cpl); // kJ/kg.degC   (KCal_2_kJ=4.184)
	      }
      else  // assume water
	      {
	      // ref Chem.Eng. nov 91 p236
	      const double Tf = ( K2C(T)*1.8 + 32.0); // convert to F
	      Cpl = ( 1.0152 -3.6171E-4*Tf +2.1709E-6*Tf*Tf
			      -2.9831e-9*pow(Tf,3));
	      Cpl = KCal_2_kJ(Cpl); // kJ/kg.degC   (KCal_2_kJ=4.184)
	      }
      return Cpl;
      }
    case CPM_Langa:
      {
      //As per email from Catherine Venz dated 25/07/06
      //Langa Model: 
      //Cpl = (0.99639 - 0.000390998*TNa  - 0.00053832*Al + 0.000000246493*TNa*TNa +0.00000057186*TNa*Al - 0.000000186581*Al*TempC - 0.000000107766*TNa*TempC - 0.000151278*TempC + 0.0000021464*TempC*TempC) * 4.186 
      //where Cpl is in       kJ/kg.C 
      //      TempC is in     degrees C 
      //      TNa is          g Na2CO3 / L 
      //      Al is           g Al2O3 / L 
      double Cpl;
      LiqConcs25.Converge(MA);
      const double Tc = K2C(T);
      //const double TNa = LiqConcs25.LTotalSodium(MA); //g Na2CO3 / L 
      const double TNaConc = 
                  LiqConcs25.Liq[::CausticSoda]     // NaOH
                + LiqConcs25.Liq[::SodiumCarbonate] // Na2CO3
                + LiqConcs25.Liq[SodiumOxalate]     // Na2C2O4
                + LiqConcs25.Liq[Organics]          // Na2C5O7
                + LiqConcs25.Liq[Organics1]         // Na2C5O11
                + LiqConcs25.Liq[SodiumChloride]    // NaCl
                + LiqConcs25.Liq[SodiumSulphate]    // Na2SO4
                + LiqConcs25.Liq[SodiumFluoride];   // NaF
      const double CAlum = LiqConcs25.Liq[::Alumina];
      Cpl = (0.99639 - 0.000390998*TNaConc  - 0.00053832*CAlum + 
             0.000000246493*TNaConc*TNaConc + 0.00000057186*TNaConc*CAlum - 
             0.000000186581*CAlum*Tc - 0.000000107766*TNaConc*Tc - 
             0.000151278*Tc + 0.0000021464*Tc*Tc) * 4.186;
      //Cpl = KCal_2_kJ(Cpl); // kJ/kg.degC   (KCal_2_kJ=4.184)
      return Cpl;
      }
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

double CAlcanSM::get_msEnthalpy(long Phases, double T, double P, MArray * pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  //SMFnRanges.SetInRange(eSpFn_H, true);

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
    Hl = LiqCpCalc(MA, T) * Tc;
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

double CAlcanSM::get_msEntropy(long Phases, double T, double P, MArray * pMA)
  {
  //SMFnRanges.SetInRange(eSpFn_S, true);
  return MSpModelBase::get_msEntropy(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double CAlcanSM::get_msCp(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  //SMFnRanges.SetInRange(eSpFn_Cp, true);

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
    Cpl = LiqCpCalc(MA, T);
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

double CAlcanSM::get_SaturationT(double P, MArray * pMA)
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

double CAlcanSM::get_SaturationP(double T, MArray * pMA)
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
  if (WaterFrac>sm_dH2OTestFrac1)
    {
    const double Std_SatP = MSpModelBase::get_SaturationP(T, &MA);
    const double Std_Prop = Min(1.0, (WaterFrac-sm_dH2OTestFrac1)/(sm_dH2OTestFrac0-sm_dH2OTestFrac1));
    SatP = (SatP*(1.0-Std_Prop)) + (Std_SatP*Std_Prop);
    }
  return SatP;
  }

//---------------------------------------------------------------------------

double CAlcanSM::BoilPtElev(MArray & MA, double T)
  {
  double TLiq=MA.Mass(MP_Liq); // Total Liquid kg/s
  //SMFnRanges.SetInRange(eSpFn_BPE, true);
  if (TLiq<1.0e-9)
    return 0.0;

  double BPE = 0.0;
  if (sm_iBPEMethod==BPM_Dewey)
    {
	  // Gram ion per kg liq
	  const double giAl2O3    = 1000 * MA[Alumina] / TLiq * 2.0 * 81.97 / ::Alumina.MW; // 81.97 = MW of NaAlO2
	  const double giNaOH     = 1000 * MA[CausticSoda] / TLiq
			  - (1000 * MA[Alumina] / TLiq * 2.0 * ::CausticSoda.MW/::Alumina.MW);
	  const double giNa2CO3   = 1000 * MA[SodiumCarbonate] / TLiq;
	  const double giNaCl     = 1000 * MA[SodiumChloride] / TLiq;
	  const double giNa2SO4   = 1000 * MA[SodiumSulphate] / TLiq;
	  const double giNaF      = 1000 * MA[SodiumFluoride] / TLiq;
	  const double giNa2C5O7  = 1000 * MA[Organics] / TLiq;
	  const double giNa2C5O11 = 1000 * MA[Organics1] / TLiq;
	  const double giNa2C2O4  = 1000 * MA[SodiumOxalate] / TLiq;

	  const double giSum = giAl2O3 + giNaOH + giNa2CO3 + giNaCl + giNa2SO4 + giNaF + giNa2C5O7 + giNa2C5O11 + giNa2C2O4;

	  const double gH2OperKgLiq = 1000.0 - giSum;

	  //Gram moles per kg liq
	  const double gmAl2O3    = giAl2O3 / 81.97; // 81.97 = MW of NaAlO2
	  const double gmNaOH     = giNaOH / ::CausticSoda.MW;
	  const double gmNa2CO3   = giNa2CO3 / ::SodiumCarbonate.MW;
	  const double gmNaCl     = giNaCl / ::SodiumChloride.MW;
	  const double gmNa2SO4   = giNa2SO4 / ::SodiumSulphate.MW;
	  const double gmNaF      = giNaF / ::SodiumFluoride.MW;
	  const double gmNa2C5O7  = giNa2C5O7 / ::Organics.MW;
	  const double gmNa2C5O11 = giNa2C5O11 / ::Organics1.MW;
	  const double gmNa2C2O4  = giNa2C2O4 / ::SodiumOxalate.MW;

	  const double gmSum = gmAl2O3 + gmNaOH + gmNa2CO3 + gmNaCl + gmNa2SO4 + gmNaF + gmNa2C5O7 + gmNa2C5O11 + gmNa2C2O4;

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

    }
  else
    {
    //sm_iBPEMethod==BPM_Adamson
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

    LiqConcs25.Converge(MA);
    const double Sodium = LiqConcs25.Liq[::CausticSoda] + LiqConcs25.Liq[::SodiumCarbonate]
                        + LiqConcs25.Liq[::SodiumOxalate] + LiqConcs25.Liq[::Organics] + LiqConcs25.Liq[::Organics1]
                        + LiqConcs25.Liq[::SodiumChloride] + LiqConcs25.Liq[::SodiumSulphate] + LiqConcs25.Liq[::SodiumFluoride];
    const double x1 = Sodium*::OccSoda.MW/::SodiumCarbonate.MW;
    const double x2 = K2C(T);
    const double x1_2 = x1*x1;
    const double x2_2 = x2*x2;
    BPE = a + b*x1 + c*x2 + d*x1_2 + e*x2_2 + f*x1*x2 + g*x1*x1_2 + h*x2*x2_2 + i*x1*x2_2 + j*x1_2*x2;
    }
	
	return BPE;
  }

//---------------------------------------------------------------------------

double CAlcanSM::get_DynamicViscosity(long Phases, double T, double P, MArray * pMA)
  {
  return MSpModelBase::get_DynamicViscosity(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double CAlcanSM::get_ThermalConductivity(long Phases, double T, double P, MArray * pMA)
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
 idASatMethod       		   ,
 idGrowthMethod       	   ,
 idCPMethod       		     ,
 idMinSolCp       		     ,
 idMorphology       		   ,
 idNucleation       		   ,
 idNa2OFactor       		   ,
 idCarbonFactor       	   ,
 idH2OTestFrac0    	       ,
 idH2OTestFrac1            , 

 idDefineLiquor			       ,
 idRqd_A_to_C				       ,
 idRqd_C_to_S				       ,
 idRqd_C					         ,
 idRqd_Oxalate			       ,
 idRqd_TOC				         ,
 idRqd_Na2SO4				       ,
 idRqd_NaCl				         ,
 idRqd_NaF				         ,
 idRqd_P2O5                ,
 idRqd_SiO2				         ,
 idRqd_SolConc			       ,
 idRqd_SolFrac			       ,
 idRqd_SolOccSoda			     ,
 idRqd_SolOx			         ,

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
 idOrganicO11Conc25        ,
 idOrganateConc25			     ,
 idOxalateConc25			     ,
 idTotalOrganics25		     ,
 idChlorineConc25			     ,
 idSulphateConc25			     ,
 idTotalNa25				       ,

 idSeparator2				       ,

 idMR					             ,
 idAA					             ,
 idTT					             ,
 idSeparator3				       ,

 idLVolume25				       ,
 idSLVolume25				       ,
 idLDensity25 			       ,
 idSLDensity25			       ,

 idSeparator4				       ,	

 idAluminaConc			       ,
 idSodiumCarbonateConc     ,
 idTOC					           ,
 idSolidsConc				       ,

 idSeparator5				       ,	

 idCausticConc			       ,
 idSodaConc				         ,

 idSeparator6				       ,	

 idAluminaSolubility	     ,
 idOxalateSolubility	     ,
 idBoehmiteSolubility      ,
 idAtoCSaturation			     ,
 idSSNRatio				         ,

 idSeparator7				       ,	

 idBoilPtElev				       ,
 idNa2OSolFrac             ,
 idSPOFrac                 ,
 idTHAMassFlow   			     ,
 idTHADens				         ,
 idGrowthRate              ,

  idMPI_EndOfProps		   
  };

//---------------------------------------------------------------------------

long CAlcanSM::DefinedPropertyCount()
  {
  return idMPI_EndOfProps + 1 + MSpModelBase::DefinedPropertyCount();
  }

//---------------------------------------------------------------------------

static MPropertyInfo::MStringValueP SVASat[]={
  {"Original",  ASM_Original  },
  {"March2002", ASM_March2002 },
  {"May2003",   ASM_May2003   },
  {"Hyprod",    ASM_Hyprod    },
  {0}};
static MPropertyInfo::MStringValueP SVDens[]={
  { "Original",  BDM_Original  },
  { "March2002", BDM_March2002 },
  { "July2003",  BDM_July2003  },
  { "MD",        BDM_MD  },
  {0}};
static MPropertyInfo::MStringValueP SVGrw[]={
  { "Original", GRM_Original },
  { "Nov2003",  GRM_Nov2003  },
  { "Hyprod",   GRM_Hyprod   },
  {0}};
static MPropertyInfo::MStringValueP SVCp[]={
  {"Original",  CPM_Original },
  {"Hyprod",	  CPM_Hyprod   },
  {"Langa",     CPM_Langa    },
  {0}};
static MPropertyInfo::MStringValueP SVBPE[]={
  {"Dewey",   BPM_Dewey   },
  {"Adamson", BPM_Adamson },
  {0}};

long CAlcanSM::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {//define a list of all properties: This sets what is displayed in the access window.
  long Inx=Index-MSpModelBase::DefinedPropertyCount();
  switch(Inx)
    {
    case idDensityMethod   	: Info.SetText("--Global options---------");
                              Info.SetStructName("Config"); //NB: The "struct name" is used as part of tag!
                              Info.Set(ePT_Long,      "", "DensityMethod",          SVDens,                     MP_GlobalProp|MP_Parameter, "Global density method"); return Inx;
    case idBPEMethod       	: Info.Set(ePT_Long,      "", "BPEMethod",              SVBPE,                      MP_GlobalProp|MP_Parameter, "Global BPE method"); return Inx;
    case idASatMethod			  : Info.Set(ePT_Long,      "", "ASatMethod",             SVASat,					            MP_GlobalProp|MP_Parameter, "Global ASat Method"); return Inx;
    case idGrowthMethod		  : Info.Set(ePT_Long,      "", "GrowthMethod",           SVGrw,						          MP_GlobalProp|MP_Parameter, "Global Growth Method used by Precip"); return Inx;
    case idCPMethod			    : Info.Set(ePT_Long,      "", "CPMethod",               SVCp,                       MP_GlobalProp|MP_Parameter, "Global CP method"); return Inx;
    case idMinSolCp        	: Info.Set(ePT_Double,    "", "MinSolCp",               MC_CpMs,  "kJ/kg.C", 0, 0,  MP_GlobalProp|MP_Parameter|MF_InitHidden, "Global minimum solids Cp"); return Inx;
    case idMorphology			  : Info.Set(ePT_Double,    "", "Morphology",             MC_,      "",        0, 0,	MP_GlobalProp|MP_Parameter, "Global factor"); return Inx;
    case idNucleation			  : Info.Set(ePT_Double,    "", "Nucleation",             MC_,      "",        0, 0,	MP_GlobalProp|MP_Parameter, "Global factor"); return Inx;
    case idNa2OFactor		  	: Info.Set(ePT_Double,    "", "Na2OFactor",             MC_,      "",        0, 0,	MP_GlobalProp|MP_Parameter, "Global factor"); return Inx;
    case idCarbonFactor  		: Info.Set(ePT_Double,    "", "CarbonFactor",           MC_,      "",        0, 0,	MP_GlobalProp|MP_Parameter, "Global factor"); return Inx;
    case idH2OTestFrac0     : Info.Set(ePT_Double,    "", "WaterTestFrac0",         MC_Frac,  "%",   0, 0,      MP_GlobalProp|MP_Parameter|MP_InitHidden, "Global water fraction above which Standard properties are used"); return Inx;
    case idH2OTestFrac1     : Info.Set(ePT_Double,    "", "WaterTestFrac1",         MC_Frac,  "%",   0, 0,      MP_GlobalProp|MP_Parameter|MP_InitHidden, "Global water fraction above which proportional amounts of two properties models are used"); return Inx;

    case idDefineLiquor			: Info.SetText("--Calculator-------------");
                              Info.SetStructName("Calc"); //NB: The "struct name" is used as part of tag!
                              Info.Set(ePT_Bool,      "",   "DefineLiquor",         MC_,      "",    0, 0,    MP_ConfigProp|MP_Parameter|MP_CheckBox, ""); return Inx;
    case idRqd_A_to_C			  : Info.Set(ePT_Double,    "",   "Rqd_A/C",              MC_,      "",    0, 0,    MP_ConfigProp|MP_Parameter, "The required Alumina/Caustic Ratio"); return Inx;
    case idRqd_C_to_S			  : Info.Set(ePT_Double,    "",   "Rqd_C/S",              MC_,      "",    0, 0,    MP_ConfigProp|MP_Parameter, "The required Caustic/Soda Ratio"); return Inx;
    case idRqd_C				    : Info.Set(ePT_Double,    "",   "Rqd_C",                MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Caustic Conc expressed as Carbonate @ 25"); return Inx;
    case idRqd_Oxalate			: Info.Set(ePT_Double,    "",   "Rqd_Oxalate",          MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Oxalate Concentration @ 25"); return Inx;
    case idRqd_TOC				  : Info.Set(ePT_Double,    "",   "Rqd_TOC",              MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Total Organic Carbon Conc expressed as Carbon @ 25"); return Inx;
    case idRqd_Na2SO4			  : Info.Set(ePT_Double,    "",   "Rqd_Na2SO4",           MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Sodium Sulphate Concentration @ 25"); return Inx;
    case idRqd_NaCl				  : Info.Set(ePT_Double,    "",   "Rqd_NaCl",             MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Sodium Chloride Concentration @ 25"); return Inx;
    case idRqd_NaF			    : Info.Set(ePT_Double,    "",   "Rqd_NaF",              MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Sodium Fluoride Concentration @ 25"); return Inx;
    case idRqd_P2O5			    : Info.Set(ePT_Double,    "",   "Rqd_P2O5",             MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Diphosphorus Concentration @ 25"); return Inx;
    case idRqd_SiO2				  : Info.Set(ePT_Double,    "",   "Rqd_SiO2",             MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Silica Concentration @ 25"); return Inx;
    case idRqd_SolConc			: Info.Set(ePT_Double,    "",   "Rqd_SolConc",          MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter|MP_NanOK, "The required Solids Conc"); return Inx;
    case idRqd_SolFrac			: Info.Set(ePT_Double,    "",   "Rqd_SolFrac",          MC_Frac,  "%",   0, 0,    MP_ConfigProp|MP_Parameter, "The required Solids Fraction"); return Inx;
    case idRqd_SolOccSoda		: Info.Set(ePT_Double,    "",   "Rqd_OccSoda",          MC_Frac,  "%",   0, 0,    MP_ConfigProp|MP_Parameter, "The required fraction of OccSoda in solids"); return Inx;
    case idRqd_SolOx	    	: Info.Set(ePT_Double,    "",   "Rqd_SPO",              MC_Frac,  "%",   0, 0,    MP_ConfigProp|MP_Parameter, "The required fraction of Oxalate in solids"); return Inx;

    case idAluminaConc25		: Info.SetStructName("Props"); //NB: The "struct name" is used as part of tag! Recomend "Props" for compatability with other properties models.
                              Info.SetText("--Concentration and Ratio @ 25,-----------");
                              Info.Set(ePT_Double,    "A", "Alumina@25",            MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Alumina Concentration @ 25"); return Inx;
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
                              Info.Set(ePT_Double,    "C", "Caustic@25",			      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Caustic Concentration @ 25"); return Inx;
    case idSodaConc25			  : Info.Set(ePT_Double,    "S", "Soda@25",				        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Soda Concentration @ 25"); return Inx;
    case idCarbonateConc25	: Info.Set(ePT_Double,    "", "Carbonate@25",			      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "SodiumCarbonate Concentration @ 25"); return Inx;
    case idOrganicO11Conc25	: Info.Set(ePT_Double,    "", "OrganicO11@25",          MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Organic11 Concentration @ 25"); return Inx;
    case idOrganateConc25		: Info.Set(ePT_Double,    "", "Organate@25",			      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Organate Concentration @ 25"); return Inx;
    case idOxalateConc25		: Info.Set(ePT_Double,    "", "Oxalate@25",				      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "SodiumOxalate Concentration @ 25"); return Inx;
    case idTotalOrganics25	: Info.Set(ePT_Double,    "", "TotalOrganics@25",       MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Total Organic Concentration @ 25"); return Inx;
    case idChlorineConc25		: Info.Set(ePT_Double,    "", "Chloride@25",			      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "SodiumChloride Concentration @ 25"); return Inx;
    case idSulphateConc25		: Info.Set(ePT_Double,    "", "Sulphate@25",			      MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "SodiumSulphate Concentration @ 25"); return Inx;
    case idTotalNa25			  : Info.Set(ePT_Double,    "", "TotalNa@25",             MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Total Sodium Concentration @ 25"); return Inx;

    case idSeparator2			  : Info.SetText("..."); return Inx;

    case idMR					      : Info.SetText("--Ratio and Conc @ 25, as Na2O Equiv------");
                              Info.Set(ePT_Double,    "", "MR",                     MC_,    "",        0,      0,  MP_Result|MP_NoFiling, "Mole Flow Ratio of Caustic:Alumina in Liquor phase"); return Inx;
    case idAA					      : Info.Set(ePT_Double,    "", "AA",                     MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Total Sodium concentration, expressed as grams Na2O/L liquor @ 25ºC"); return Inx;
    case idTT					      : Info.Set(ePT_Double,    "", "TT",                     MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "NaOH concentration, expressed as grams Na2O/L liquor @ 25ºC"); return Inx;

    case idSeparator3		  	: Info.SetText("..."); return Inx;

    case idLVolume25			  : Info.SetText("--Volume and Density @ 25-----------------");
                              Info.Set(ePT_Double,    "", "LVolume@25",               MC_Qv,   "L/s",    0,      0,  MP_Result|MP_NoFiling, "Liquor Volumetric flowrate @ 25"); return Inx;
    case idSLVolume25			  : Info.Set(ePT_Double,    "", "SLVolume@25",              MC_Qv,   "L/s",    0,      0,  MP_Result|MP_NoFiling, "Slurry Volumetric flowrate @ 25"); return Inx;
    case idLDensity25 		  : Info.Set(ePT_Double,    "", "LDensity@25",              MC_Rho,  "g/L",    0,      0,  MP_Result|MP_NoFiling, "Liquor Density @ 25"); return Inx;
    case idSLDensity25		  : Info.Set(ePT_Double,    "", "SLDensity@25",             MC_Rho,  "g/L",    0,      0,  MP_Result|MP_NoFiling, "Slurry Density @ 25"); return Inx;

    case idSeparator4			  : Info.SetText("..."); return Inx;

    case idAluminaConc			: Info.SetText("--Concentration @ T-----------------------");
                              Info.Set(ePT_Double,    "", "Alumina@T",				        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Alumina Concentration @ T"); return Inx;
    case idSodiumCarbonateConc: Info.Set(ePT_Double,  "", "Carbonate@T",			        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling|MF_InitHidden, "SodiumCarbonate Concentration @ T"); return Inx;
    case idTOC					    : Info.Set(ePT_Double,    "", "TOC@T",                    MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Total Organic Carbon Concentration @ T"); return Inx;
    case idSolidsConc			  : Info.Set(ePT_Double,    "", "SolidsConc@T",             MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Solids Concentration @ T"); return Inx;

    case idSeparator5			  : Info.SetText("..."); return Inx;

    case idCausticConc			: Info.SetText("--Concentration @ T, as Na2CO3 Equiv -----");
                              Info.Set(ePT_Double,    "", "Caustic@T",                MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Caustic Concentration @ T"); return Inx;
    case idSodaConc				  : Info.Set(ePT_Double,    "", "Soda@T",                   MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Soda Concentration @ T"); return Inx;

    case idSeparator6			  : Info.SetText("..."); return Inx;

    case idAluminaSolubility: Info.SetText("--Other Bayer Liquor Properties @ T-------");
                              Info.Set(ePT_Double,    "", "AluminaSolubility",        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Alumina Saturation Concentration @ T"); return Inx;
    case idOxalateSolubility: Info.Set(ePT_Double,    "", "OxalateSolubility",        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Oxalate Solubility Concentration @ T"); return Inx;
    case idBoehmiteSolubility: Info.Set(ePT_Double,   "", "BoehmiteSolubility",       MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling, "Boehmite Solubility Concentration @ T"); return Inx;
    case idAtoCSaturation		: Info.Set(ePT_Double,    "", "ASat_To_C",                MC_,     "",       0,      0,  MP_Result|MP_NoFiling, "Alumina Saturation to Caustic ratio @ T"); return Inx;
    case idSSNRatio				  : Info.Set(ePT_Double,    "", "SSNRatio",                 MC_,     "",       0,      0,  MP_Result|MP_NoFiling, "A/C actual to ASat/C ratio @ T"); return Inx;

    case idSeparator7			  : Info.SetText("..."); return Inx;

    case idBoilPtElev			  : Info.SetText("--Other-------");
                              Info.Set(ePT_Double,    "", "BoilPtElev",               MC_dT,   "C",      0,      0,  MP_Result|MP_NoFiling, "Boiling Point Elevation"); return Inx;
    case idNa2OSolFrac      : Info.Set(ePT_Double,    "", "OccSoda",                  MC_Frac,  "%",     0,      0,  MP_Result|MP_NoFiling, "Fraction of OccSoda in solids"); return Inx;
    case idSPOFrac          : Info.Set(ePT_Double,    "", "SPO",                      MC_Frac,  "%",     0,      0,  MP_Result|MP_NoFiling, "Fraction of Oxalate in solids"); return Inx;
    case idTHAMassFlow   		: Info.Set(ePT_Double,    "", "THAMassFlow",              MC_Qm,   "kg/s",   0,      0,  MP_Result|MP_NoFiling, "THA flow rate"); return Inx;
    case idTHADens				  : Info.Set(ePT_Double,    "", "THADens",                  MC_Rho,  "kg/m^3", 0,      0,  MP_Result|MP_NoFiling, "THA Density"); return Inx;
    case idGrowthRate			  : Info.Set(ePT_Double,    "", "GrowthRate",               MC_Ldt,  "um/s",   0,      0,  MP_Result|MP_NoFiling, "Growth Rate"); return Inx;

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

DWORD CAlcanSM::GetPropertyVisibility(long Index)
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
    case idRqd_NaF:
    case idRqd_P2O5:
    case idRqd_SiO2: 
    case idRqd_SolConc: 
    case idRqd_SolFrac: return fDoCalc ? ePVis_All : (ePVis_DynFull|ePVis_DynFlow|ePVis_Probal|ePVis_File);
    case idRqd_SolOccSoda:
    case idRqd_SolOx: return (fDoCalc && dRqd_SolFrac>0.0) ? ePVis_All : (ePVis_DynFull|ePVis_DynFlow|ePVis_Probal|ePVis_File);
    case idCarbonFactor: return (sm_iASatMethod==ASM_May2003 || sm_iASatMethod==ASM_Hyprod) ? ePVis_All : (ePVis_DynFull|ePVis_DynFlow|ePVis_Probal|ePVis_File);
    default:  return ePVis_All; 
    }
  return ePVis_All;
  }

//---------------------------------------------------------------------------

void CAlcanSM::GetPropertyValue(long Index, ULONG Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {//define method of retrieving values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    case idDensityMethod		: Value=sm_iDensityMethod;                  return; 
    case idBPEMethod        : Value=sm_iBPEMethod;						          return; 
    case idASatMethod			  : Value=sm_iASatMethod;                     return; 
    case idCPMethod        	: Value=sm_iCPMethod;						            return; 
    case idGrowthMethod			: Value=sm_iGrowthMethod;                   return; 
    case idMinSolCp				  : Value=CAlcanSM::sm_dMinSolCp;             return; 
    case idMorphology			  : Value=CAlcanSM::sm_Morphology;            return; 
    case idNucleation			  : Value=CAlcanSM::sm_Nucleation;            return; 
    case idNa2OFactor			  : Value=CAlcanSM::sm_Na2OFactor;            return; 
    case idCarbonFactor			: Value=CAlcanSM::sm_CarbonFactor;          return; 
    case idH2OTestFrac0     : Value=sm_dH2OTestFrac0;                   return; 
    case idH2OTestFrac1     : Value=sm_dH2OTestFrac1;                   return; 

    case idDefineLiquor			: Value=fDoCalc;                            return; 
    case idRqd_A_to_C			  : Value=dRqd_AtoC;                          return;
    case idRqd_C_to_S			  : Value=dRqd_CtoS;                          return;
    case idRqd_C				    : Value=dRqd_C;                             return;
    case idRqd_Oxalate			: Value=dRqd_Ox;                            return;
    case idRqd_TOC				  : Value=dRqd_TOC;                           return;
    case idRqd_Na2SO4			  : Value=dRqd_Sul;                           return;
    case idRqd_NaCl				  : Value=dRqd_Salt;                          return;
    case idRqd_NaF			    : Value=dRqd_Fl;                            return;
    case idRqd_P2O5 		    : Value=dRqd_P2O5;                          return;
    case idRqd_SiO2				  : Value=dRqd_Sil;                           return;
    case idRqd_SolConc			: Value=dRqd_SolConc;                       return;
    case idRqd_SolFrac			: Value=dRqd_SolFrac;                       return;
    case idRqd_SolOccSoda		: Value=dRqd_SolOccSoda;                    return;
    case idRqd_SolOx			  : Value=dRqd_SolOx;                         return;

    case idNa2OSolFrac      : Value=Na2OSolFrac();                      return; 
    case idSPOFrac          : Value=SPO();                              return; 
    case idTHAMassFlow			: Value=THAMassFlow();                      return; 
    case idTHADens				  : Value=THADens(T);                         return; 
    case idGrowthRate       : Value=GrowthRate()*1.0e-6;                return; 
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

    case idBoilPtElev			  : Value=getBoilingPtElevation(P, NULL);     return;
    case idMR					      : Value=MR();                               return; 
    case idAA					      : Value=AA();                               return; 
    case idTT					      : Value=TT();                               return; 
    case idLVolume25			  : Value=LVolume25();                        return; 
    case idSLVolume25			  : Value=SLVolume25();                       return; 
    case idOrganicO11Conc25 : Value=OrganicO11Conc25();                 return; 
    case idOrganateConc25		: Value=OrganateConc25();                   return; 
    case idOxalateConc25		: Value=OxalateConc25();                    return; 
    case idTotalOrganics25	: Value=TotalOrganicsConc25();              return; 
    case idChlorineConc25		: Value=ChlorineConc25();                   return; 
    case idSulphateConc25		: Value=SulphateConc25();                   return; 
    case idTotalNa25			  : Value=TotalSodiumConc25();                return; 

    case idAluminaSolubility: Value=AluminaSolubility(T);               return; 
    case idOxalateSolubility: Value=OxalateSolubility(T);               return; 
    case idBoehmiteSolubility: Value=BoehmiteSolubility(T);             return; 
    case idAtoCSaturation		: Value=AtoCSaturation(T);                  return; 
    case idSSNRatio				  : Value=SSNRatio(T);                        return; 

    case idLDensity25			  : Value=LDensity25();                       return; 
    case idSLDensity25			: Value=SLDensity25();                      return; 
		
    default: Value = 0.0;
    }
  }

//---------------------------------------------------------------------------

void CAlcanSM::PutPropertyValue(long Index, MPropertyValue & Value)
  {//define method of setting values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    case idDensityMethod  : sm_iDensityMethod=(byte)(long)Value;  return;
    case idASatMethod		  : sm_iASatMethod=(byte)(long)Value;     return;
    case idCPMethod       : sm_iCPMethod=(byte)(long)Value;		    return; 
    case idBPEMethod      : sm_iBPEMethod=(byte)(long)Value;		  return; 
    case idGrowthMethod	  : sm_iGrowthMethod=(byte)(long)Value;   return;
    case idMinSolCp			  : CAlcanSM::sm_dMinSolCp=Value;            return;
    case idMorphology		  : CAlcanSM::sm_Morphology=Value;        return;
    case idNucleation		  : CAlcanSM::sm_Nucleation=Value;        return;
    case idNa2OFactor		  : CAlcanSM::sm_Na2OFactor=Value;        return;
    case idCarbonFactor	  : CAlcanSM::sm_CarbonFactor=Value;      return;
    case idH2OTestFrac0   : sm_dH2OTestFrac0=Range(0.0, (double)Value, 1.0); return; 
    case idH2OTestFrac1   : sm_dH2OTestFrac1=Range(0.0, (double)Value, sm_dH2OTestFrac0); return; 

    case idDefineLiquor		: fDoCalc=Value;                        return;
    case idRqd_A_to_C		  : dRqd_AtoC=Value;                      return;
    case idRqd_C_to_S		  : dRqd_CtoS=Value;                      return;
    case idRqd_C			    : dRqd_C=Value;                         return;
    case idRqd_Oxalate		: dRqd_Ox=Value;                        return;
    case idRqd_TOC			  : dRqd_TOC=Value;                       return;
    case idRqd_Na2SO4		  : dRqd_Sul=Value;                       return;
    case idRqd_NaCl			  : dRqd_Salt=Value;                      return;
    case idRqd_NaF		    : dRqd_Fl=Value;                        return;
    case idRqd_P2O5		    : dRqd_P2O5=Value;                      return;
    case idRqd_SiO2			  : dRqd_Sil=Value;                       return;
    case idRqd_SolConc		: dRqd_SolConc=Value;                   return;
    case idRqd_SolFrac		: dRqd_SolFrac=Range(0.0, (double)Value, 0.99); return;
    case idRqd_SolOccSoda	: dRqd_SolOccSoda=Range(0.0, (double)Value, 0.95); return;
    case idRqd_SolOx			: dRqd_SolOx=Range(0.0, (double)Value, 0.95); return;
    }
  }

//===========================================================================
//
//
//
//===========================================================================

void CAlcanSM::InputCalcs(bool DoIt, double T_)
  {
  switch (View)
    {
    case SVV_AsRawFrac:
    case SVV_AsRawMass:
    case SVV_AsRawMassFlow:
      fDoCalc=false;
      return;
    }

  if (DoIt)
    {
    const double MW_Na2CO3    = ::SodiumCarbonate.MW;  //105.989
    const double MW_Na2O      = ::OccSoda.MW;          //61.9789360
    const double MW_Al2O3     = ::Alumina.MW;          //101.961278
    const double MW_THA       = ::THA.MW;              //156.007118
    const double MW_Na2C2O4   = ::SodiumOxalate.MW;    //133.999
    const double MW_Na2C5O11  = ::Organics1.MW;        //282.028

    //calculator to determine Liquor feed fractions based on requirements 
    //specified as Na2CO3 equivilant at 25C...

    Dbg.PrintLn("==================");
    double DensL = 1100.0;
    double DensS = 2420.0;
    const bool FindingConc=Valid(dRqd_SolConc);
    const bool WantSolids=(FindingConc && dRqd_SolConc>0.0) || (dRqd_SolFrac>1e-6);
    double TotalMass = Mass(MP_SL);
    if (WantSolids)
      {
      if (FindingConc)
        dRqd_SolFrac=Range(0.0, (dRqd_SolConc*DensS)/(dRqd_SolConc*DensS + DensL*DensS - DensL*dRqd_SolConc), 0.99); // Initial Guess
      if (Mass(MP_Sol)<1.0e-6 && TotalMass>UsableMass)
        {//assume THA solids required
        Log.Message(MMsg_Note, "Assume solids are THA for Bayer feed calculator");
        M[THA]=TotalMass*dRqd_SolFrac;
        const double Scl=(TotalMass*(1.0-dRqd_SolFrac))/GTZ(Mass(MP_Liq));
        ScaleMass(MP_Liq, Scl);
        TotalMass = Mass(MP_SL);
        }
      //adjust solids Oxalate and OccSoda
      if (dRqd_SolOx>0.0 || dRqd_SolOccSoda>0.0)
        {
        const int OxIndex = SodiumOxalateSol;
        const int OSIndex = OccSoda;
        const double SolMass = Mass(MP_Sol);
        const double PrevOther = SolMass - M[OxIndex] - M[OSIndex];
        if (dRqd_SolOx>0.0)
          M[OxIndex] = dRqd_SolOx*SolMass;
        else
          M[OxIndex] = 0.0;
        if (dRqd_SolOccSoda>0.0)
          M[OSIndex] = Min(dRqd_SolOccSoda/MW_Na2O*MW_Na2CO3/MW_THA*MW_Al2O3, 1.0-dRqd_SolOx)*SolMass;
        else
          M[OSIndex] = 0.0;
        //now scale all the other solids...
        const double NewOther = SolMass - M[OxIndex] - M[OSIndex];
        const double Scale = NewOther/GTZ(PrevOther);
        for (int sn=0; sn<gs_MVDefn.Count(); sn++)
          {
          if (gs_MVDefn[sn].IsLiquid() && sn!=OxIndex && sn!=OSIndex)
            {
            M[sn] *= Scale;
            }
          }

        ////to be sure...
        //const double Scl=(TotalMass*(1.0-dRqd_SolFrac))/GTZ(Mass(MP_Liq));
        //ScaleMass(MP_Liq, Scl);
        TotalMass = Mass(MP_SL);
        }
      }
    else
      {
      dRqd_SolFrac = 0.0;
      }

    //----------------------------------------------
    const double LiqMassCalc = (M[Water] + M[CausticSoda] + M[Alumina] + 
                          M[SodiumCarbonate] + M[SodiumChloride] + M[SodiumFluoride] +
                          M[SodiumSulphate] + M[SilicateLiq] + M[SodiumOxalate] + M[Organics] +
                          M[Organics1] + M[DiphosphorusLiq]);
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
      {
      double WaterUsed = 0.0;
      const double P = Pressure;
      for (bool Done=false; !Done; )
        {
        dRqd_SolFrac=Range(0.0, dRqd_SolFrac, 0.99);
        LiqMass = TotalMass*(1.0-dRqd_SolFrac);

        double Density25;

        double Al, Caustic, Carbonate, Salt, NaSulphate, Silicate, Phosph, NaFluoride;
        double water, Derr, Org1, Org2, Org3;
        double Ox, Og, Og1, Other;
        const double minTOC = dRqd_Ox*2.0/MW_Na2C2O4*MW_C;
        double TocUsed = dRqd_TOC;
        if (dRqd_TOC<minTOC)
          {
          Log.Message(MMsg_Warning, "Required TOC too low based on specified Oxalate");
          TocUsed = minTOC; //this will give Og of zero
          }
        Ox = dRqd_Ox;
        Og = 0.0; //Og ==> Na2C5O7
        Og1 = (TocUsed/MW_C - dRqd_Ox*2.0/MW_Na2C2O4)*MW_Na2C5O11/5.0; //Og1 ==> Na2C5O11

        if (dRqd_C > 1e-6)
          {
          const double MW_NaOH      = ::CausticSoda.MW;      //39.9971080
          const double MW_NaCl      = ::SodiumChloride.MW;   //58.4425
          const double MW_Na2SO4    = ::SodiumSulphate.MW;   //142.043
          const double MW_NaF       = ::SodiumFluoride.MW;   //
          const double MW_Na2C5O7   = ::Organics.MW;         //218.030
          //dRqd_TOC = dRqd_Org*MW_C/MW_Na2CO3*(5.0 - (3.0 * dRqd_OrgRatio));
          // Calculate density based on factors.
          double Lm = LiqMass;
          double Dens1 = DensL/1000.0;//1.100;  // First guess at density
          double Vol, Tal, Tna, Tcc;
          double XAl, TNa, TCaus, XNaC, XNa;
          for (int j=0; j<100; j++)
            {
            Vol = Lm/Dens1;

            switch (sm_iDensityMethod)
              {
              case BDM_Original:
                {
                Tna = (dRqd_C/dRqd_CtoS + (Ox/MW_Na2C2O4 + Og/MW_Na2C5O7 + Og1/MW_Na2C5O11 + dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4 + dRqd_Fl/(2*MW_NaF)) * MW_Na2CO3)
                      * Vol/(1000.0*Lm)*100.0; //Wgt % of Total Na
                Tcc = dRqd_C/(Dens1*1000.0)*100.0;//Wgt % of Caustic
                Tal = dRqd_AtoC * Tcc; //Wgt % of Alumina
                Density25 = CAlcanConcs::LiquorDensEqn1(25.0, Tna, Tal)/1000.0;
                break;
                }
              case BDM_March2002:
              case BDM_MD:
                {
                Tna = (dRqd_C/dRqd_CtoS + (Ox/MW_Na2C2O4 + Og/MW_Na2C5O7 + Og1/MW_Na2C5O11 + dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4 + dRqd_Fl/(2*MW_NaF)) * MW_Na2CO3)
                      * Vol/(1000.0*Lm)*100.0; //Wgt % of Total Na
                XAl   = (dRqd_AtoC*dRqd_C/(Dens1*1000)*Lm)/Lm;
                TNa   = Tna/100.0*Lm*MW_Na2O/MW_Na2CO3;
                TCaus = dRqd_C/(Dens1*1000)*Lm;
                if (sm_iDensityMethod==BDM_March2002)
                  TCaus *= (MW_Na2O/MW_Na2CO3);
                XNaC  = (TNa-TCaus)/Lm;
                XNa   = TNa/Lm;
                Density25 = CAlcanConcs::LiquorDensEqn2(25.0, XNaC, XNa, XAl)/1000.0;
                break;
                }
              case BDM_July2003:
                {
                double TTS = dRqd_C/dRqd_CtoS + (dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4)*MW_Na2CO3;
                Density25 = CAlcanConcs::LiquorDensEqn3(25.0, dRqd_AtoC*dRqd_C, dRqd_C, TTS, dRqd_TOC)/1000.0;
                break;
                }
              }

            Derr = fabs(Dens1 - Density25);
            if ( Derr < 1e-9)
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
          NaFluoride = dRqd_Fl * Vol;
          NaSulphate = dRqd_Sul * Vol;
          Silicate   = dRqd_Sil * Vol;
          Phosph     = dRqd_P2O5 * Vol;
          Org1       = dRqd_Ox * Vol;
          Org2       = Og * Vol;
          Org3       = Og1 * Vol;
          Other      = OtherLiqMass/LiqMass*Vol*Dens1*1000.0;

          double TmpTot = Org1 + Org2 + Org3 + NaSulphate + NaFluoride + Silicate + Phosph + Salt + Carbonate + Al + Caustic;
          TmpTot += Other;
          double Lmass = Vol * Density25 * 1000.0;
          if (TmpTot <= Lmass)
            {
            water  = Lmass - TmpTot;
            TmpTot = TmpTot + water;
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
          NaSulphate /= TmpTot;
          NaFluoride /= TmpTot;
          Silicate   /= TmpTot;
          Phosph     /= TmpTot;
          Org1       /= TmpTot;
          Org2       /= TmpTot;
          Org3       /= TmpTot;
          water      /= TmpTot;
          //-------------------------------------------------------

          // Set masses of the liquid species.
          M[CausticSoda]=Caustic*LiqMass;
          M[Alumina]=Al*LiqMass;
          M[SodiumCarbonate]=Carbonate*LiqMass;
          M[SodiumChloride]=Salt*LiqMass;
          M[SodiumSulphate]=NaSulphate*LiqMass;
          M[SilicateLiq]=Silicate*LiqMass;
          M[DiphosphorusLiq]=Phosph*LiqMass;
          M[SodiumOxalate]=Org1 * LiqMass;
          M[SodiumFluoride]=NaFluoride * LiqMass;
          M[Organics]=Org2 * LiqMass;
          M[Organics1]=Org3 * LiqMass;
          M[Water]=water*LiqMass;
          }
        else
          {
          // Default to water
          M[Water]=LiqMass;
          M[CausticSoda]=0.0;
          M[Alumina]=0.0;
          M[SodiumCarbonate]=0.0;
          M[SodiumChloride]=0.0;
          M[SodiumSulphate]=0.0;
          M[SilicateLiq]=0.0;
          M[DiphosphorusLiq]=0.0;
          M[SodiumOxalate]=0.0;
          M[SodiumFluoride]=0.0;
          M[Organics]=0.0;
          M[Organics1]=0.0;
          }
        WaterUsed=M[Water];
        
        if (FindingConc && WantSolids)
          {
          double PrevSF=dRqd_SolFrac;
          DensL=Density25*1000;
          DensS=MSpModelBase::get_Density(MP_Sol, C_2_K(25.0), P, NULL);
          dRqd_SolFrac=Max(0.0, (dRqd_SolConc*DensS)/(dRqd_SolConc*DensS + DensL*DensS - DensL*dRqd_SolConc));
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

double CAlcanSM::THAMassFlow()
  {
  MArray M(this);
  return M[::THA];
  }

//---------------------------------------------------------------------------

double CAlcanSM::THADens(double T_)
  {
  return gs_MVDefn[::THA].Density(T_, StdP, NULL);
  }

//---------------------------------------------------------------------------

double CAlcanSM::CausticConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[::CausticSoda] * DRatio);
  }

//---------------------------------------------------------------------------

double CAlcanSM::AluminaConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[::Alumina]*DRatio);
  }

//---------------------------------------------------------------------------

double CAlcanSM::SodaConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  double liq25S = LiqConcs25.Liq[::CausticSoda]
                  //+ LiqConcs25.Liq[::SodiumAluminate]
                  + LiqConcs25.Liq[::SodiumCarbonate];

  return Max(0.0, liq25S * DRatio);
  }

//---------------------------------------------------------------------------

double CAlcanSM::SodiumCarbonateConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[SodiumCarbonate]*DRatio);
  }

//---------------------------------------------------------------------------

double CAlcanSM::SodiumSulphateConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[SodiumSulphate]*DRatio);
  }

//---------------------------------------------------------------------------

double CAlcanSM::SodiumOxalateConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[SodiumOxalate]*DRatio);
  }

//---------------------------------------------------------------------------

double CAlcanSM::SPO()
  {//Solid Phase Oxalate
  const double mt=Mass(MP_Sol);
  return (mt>=UsableMass) ? M[::SodiumOxalateSol]/mt : 0.0;
  }

//---------------------------------------------------------------------------

double CAlcanSM::Na2OSolFrac()
  {
  const double mt=Mass(MP_Sol);
  return (mt>=UsableMass) ? (M[::OccSoda]/mt * ::OccSoda.MW / ::SodiumCarbonate.MW * ::THA.MW / ::Alumina.MW) : 0.0;
  }

//---------------------------------------------------------------------------

double CAlcanSM::SolidsConc(double T_)
  {
  const double mt=Mass(MP_Sol);
  return (mt>=UsableMass) ? mt/GTZ(Mass(MP_SL))*get_Density(MP_SL, T_, Pressure, NULL) : 0.0;
  }
double CAlcanSM::SolidsConc25()
  {
  const double mt=Mass(MP_Sol);
  return (mt>=UsableMass) ? mt/GTZ(Mass(MP_SL))*get_Density(MP_SL, C_2_K(25.0), Pressure, NULL) : 0.0;
  }

//---------------------------------------------------------------------------

double CAlcanSM::LVolume25()
  {
  return (Mass(MP_Liq) / get_Density(MP_Liq, C_2_K(25.0), Pressure, NULL));
  }

//---------------------------------------------------------------------------

double CAlcanSM::SLVolume25()
  {
  return (Mass(MP_SL) / get_Density(MP_SL, C_2_K(25.0), Pressure, NULL));
  }

//---------------------------------------------------------------------------

double CAlcanSM::TotalSodiumConc25()
  {//TotalNa Conc @ 25
  LiqConcs25.Converge(MArray(this));
  //double TSodium = LiqConcs25.LTotalSodium(MA); // The sodium components in liq.
  double Sodium = LiqConcs25.Liq[::CausticSoda]       // NaOH
                  + LiqConcs25.Liq[::SodiumCarbonate] // Na2CO3
                  + LiqConcs25.Liq[SodiumOxalate]     // Na2C2O4
                  + LiqConcs25.Liq[Organics]          // Na2C5O7
                  + LiqConcs25.Liq[Organics1]         // Na2C5O11
                  + LiqConcs25.Liq[SodiumChloride]    // NaCl
                  + LiqConcs25.Liq[SodiumSulphate]    // Na2SO4
                  + LiqConcs25.Liq[SodiumFluoride];   // NaF
  return Sodium;
  }

//---------------------------------------------------------------------------

double CAlcanSM::TOC(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  double Toc = (LiqConcs25.Liq[::Organics]*5.0 + LiqConcs25.Liq[::SodiumOxalate]*2.0 + LiqConcs25.Liq[::Organics1]*5.0)*MW_C/::SodiumCarbonate.MW;
  return Max(0.0, Toc*DRatio);
  //for Organics Na2C5O11 : 0.21294 = MW_Carbon*5.0/MW_Organics = 12.011 * 5.0 / 282.027936
  //for Organics Na2C5O7 : 0.27544 = MW_Carbon*5.0/MW_Organics = 12.011 * 5.0 / 218.030336
  //for SodiumOxalate Na2C2O4 : 0.17927 = MW_Carbon*2.0/MW_SodiumOxalate = 12.011 * 2.0 / 133.999136
  }
double CAlcanSM::TOC25()
  {
  LiqConcs25.Converge(MArray(this));
  return (LiqConcs25.Liq[::Organics]*5.0 + LiqConcs25.Liq[::Organics1]*5.0 + LiqConcs25.Liq[::SodiumOxalate]*2.0)*MW_C/::SodiumCarbonate.MW;
  }

//---------------------------------------------------------------------------

double CAlcanSM::SodiumOxalate_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray(this));
  double Oxalate = LiqConcs25.Liq[::SodiumOxalate] / LiqConcs25.NaFactor[::SodiumOxalate];
  return Oxalate;
  }

double CAlcanSM::OrganicO11_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray(this));
  double OrganicO11 = LiqConcs25.Liq[::Organics1] / LiqConcs25.NaFactor[::Organics1];
  return OrganicO11;
  }

double CAlcanSM::Sulphate_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray(this));
  double Sulphate = LiqConcs25.Liq[::SodiumSulphate] / LiqConcs25.NaFactor[::SodiumSulphate];
  return Sulphate;
  }

double CAlcanSM::Chlorine_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray(this));
  double Chlorine = LiqConcs25.Liq[::SodiumChloride] / LiqConcs25.NaFactor[::SodiumChloride];
  return Chlorine;
  }

double CAlcanSM::Fluoride_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray(this));
  double Fluoride = LiqConcs25.Liq[::SodiumFluoride] / LiqConcs25.NaFactor[::SodiumFluoride];
  return Fluoride;
  }

double CAlcanSM::Silicate_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray(this));
  double Silicate = LiqConcs25.Liq[::SilicateLiq] / LiqConcs25.NaFactor[::SilicateLiq];
  return Silicate;
  }

//---------------------------------------------------------------------------

double CAlcanSM::IonicStrength()
  {
  const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989..
  const double MW_Na2O     = ::OccSoda.MW;          //61.9789..
  const double MW_NaCl     = ::SodiumChloride.MW;   //58.4425..
  const double MW_Na2SO4   = ::SodiumSulphate.MW;   //142.043..
  LiqConcs25.Converge(MArray(this));
  double I;
  switch (sm_iASatMethod)
    {
    case ASM_Original:
      {
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
    case ASM_March2002:
      {
      const double K1 = 0.6499;
      const double K2 = 0.9909;
      const double K3 = 0.9107;
      const double K4 = 1.5062;
      const double SodaNaC  = LiqConcs25.Liq[::CausticSoda];
      const double NaClgpl  = LiqConcs25.Liq[::SodiumChloride];
      const double NaCgpl_  = LiqConcs25.Liq[::SodiumCarbonate];
      const double NaSgpl   = LiqConcs25.Liq[::SodiumSulphate];
      const double cTOC     = TOC(C_2_K(25.0));
      const double NaCgpl   = NaCgpl_*MW_C/MW_Na2CO3; //convert to carbon basis
      I = 0.01887*SodaNaC + K1*NaClgpl/MW_NaCl + K2*NaCgpl/MW_C + K3*NaSgpl/MW_Na2SO4 + K4*0.01887*cTOC;
      return I;
      }
    case ASM_May2003:
      {
      /* Based on Rosenberg, Healy. 2003 Data from Alcan Hyprod provided by Denis Audet.*/
      const double k1 = 0.717;
      const double k2 = 0.787;
      const double k3 = 0.770;
      const double k4 = sm_CarbonFactor;//2.0;
      const double cTOC    = TOC(C_2_K(25.0));
      const double cNaCl   = LiqConcs25.Liq[::SodiumChloride] /LiqConcs25.NaFactor[::SodiumChloride];
      const double cNa2CO3 = LiqConcs25.Liq[::SodiumCarbonate]/LiqConcs25.NaFactor[::SodiumCarbonate];
      const double cNa2SO4 = LiqConcs25.Liq[::SodiumSulphate] /LiqConcs25.NaFactor[::SodiumSulphate];
      const double cNaOH   = LiqConcs25.Liq[::CausticSoda];
      const double cC      = cNaOH;
      I = 0.01887*cNaOH + k1*cNaCl/MW_NaCl + k2*cNa2CO3/MW_Na2CO3 + k3*cNa2SO4/MW_Na2SO4 + k4*0.01887*cTOC;
      return I;
      }
    case ASM_Hyprod:
      {
      /* Based on Rosenberg, Healy. 2003 modified to correspond to Hyprod by D.Audet 27Nov03.*/
      const double k1 = 0.717;
      const double k2 = 0.787;
      const double k3 = 0.770;
      const double k4 = sm_CarbonFactor;//2.0;
      const double cTOC    = TOC(273+25.0);
      const double cNaCl   = LiqConcs25.Liq[::SodiumChloride] /LiqConcs25.NaFactor[::SodiumChloride];
      const double cNa2CO3 = LiqConcs25.Liq[::SodiumCarbonate]/LiqConcs25.NaFactor[::SodiumCarbonate];
      const double cNa2SO4 = LiqConcs25.Liq[::SodiumSulphate] /LiqConcs25.NaFactor[::SodiumSulphate];
      const double cNaOH   = LiqConcs25.Liq[::CausticSoda];
      const double cC      = cNaOH;
      I = cNaOH/53.0 + k1*cNaCl/58.5 + k2*cNa2CO3/106.0 + k3*cNa2SO4/142.0 + k4/53.0*cTOC;
      return I;
      }
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

double CAlcanSM::AluminaConcSat(double T_)
  {
  const double I = IonicStrength();
  const double I2 = Sqrt(I);
  const double cNaOH   = LiqConcs25.Liq[::CausticSoda];
  switch (sm_iASatMethod)
    {
    case ASM_Original:
      {
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
    case ASM_March2002:
      {
      const double alpha0 = -8.9263;
      const double alpha3 = -0.7288;
      const double alpha4 = 0.1908;
      const double R  = R_c;//8.314;
      const double dG = -32367.0;
      const double logGamOH = alpha0*I2/(1.0 + I2) - alpha3*I  - alpha4*pow(I,1.5);
      const double GamOH    = pow(10.0, logGamOH);
      const double Keq      = Exps(dG/(T_*R));
      const double ASat     = 0.96197*cNaOH/(1.0 + GamOH/Keq);
      return ASat;
      }
    case ASM_May2003:
      {
      /* Based on Rosenberg, Healy. 2003 Data from Alcan Hyprod provided by Denis Audet.*/
      const double a0 = -8.913464126;
      const double a3 = -0.7445847;
      const double a4 = 0.18990233;
      const double dG = -31658.79;//-30960.0; //Gibbs energy of dissolution
      const double R  = R_c;//8.3145; //Universal Gas Constant 8.314472
      const double X    = a0*I2/(1.0+I2) - a3*I - a4*Pow(I, 3.0/2.0);
      const double Keq  = Exps(dG/(R*T_)); //Equilibrium Constant
      const double ASat = 0.96197*cNaOH/(1.0+(Pow(10.0, X)/Keq));
      return ASat;
      }
    case ASM_Hyprod:
      {
      /* Based on Rosenberg, Healy. 2003 modified to correspond to Hyprod by D.Audet 27Nov03.*/
      const double a0 = -9.09065;
      const double a3 = -0.76968;
      const double a4 = 0.19483;
      const double dG = -32131.39;//-30960.0; //Gibbs energy of dissolution
      const double R  = R_c;//8.3145; //Universal Gas Constant 8.314472
      const double X    = a0*I2/(1.0+I2) - a3*I - a4*Pow(I, 3.0/2.0);
      const double Keq  = Exps(dG/(R*(T_-0.16))); //Equilibrium Constant
      const double ASat = 0.96197*cNaOH/(1.0+(Pow(10.0, X)/Keq));
      return ASat;
      }
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

double CAlcanSM::IonicStrengthBoehmite()
  {
  const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989..
  const double MW_Na2O     = ::OccSoda.MW;          //61.9789..
  const double MW_NaCl     = ::SodiumChloride.MW;   //58.4425..
  const double MW_Na2SO4   = ::SodiumSulphate.MW;   //142.043..
  /* Based on Rosenberg, Healy. Modified by D.Audet.*/
  const double k1 = 0.717;
  const double k2 = 0.787;
  const double k3 = 0.770;
  const double k4 = sm_CarbonFactor;//2.0;
  LiqConcs25.Converge(MArray(this));
  const double cTOC    = TOC(C_2_K(25.0));
  const double cNaCl   = LiqConcs25.Liq[::SodiumChloride] /LiqConcs25.NaFactor[::SodiumChloride];
  const double cNa2CO3 = LiqConcs25.Liq[::SodiumCarbonate]/LiqConcs25.NaFactor[::SodiumCarbonate];
  const double cNa2SO4 = LiqConcs25.Liq[::SodiumSulphate] /LiqConcs25.NaFactor[::SodiumSulphate];
  const double cNaOH   = LiqConcs25.Liq[::CausticSoda];
  const double cC      = cNaOH;
  const double I = 0.01887*cNaOH + k1*cNaCl/MW_NaCl + k2*cNa2CO3/MW_Na2CO3 + k3*cNa2SO4/MW_Na2SO4 + k4*0.01887*cTOC;
  return I;
  }

//---------------------------------------------------------------------------

double CAlcanSM::BoehmiteSolubility(double T_)
  {
  /*Boehmite Solubility Reference: Denis Audet Q03 - DRA - Development of a new Boehmite solubility equation.doc 
  Similar to Alumina Solubility except that the constant are different.*/
  const double I = IonicStrengthBoehmite();
  const double I2 = Sqrt(I);
  const double cNaOH = LiqConcs25.Liq[::CausticSoda];
  //Based on Rosenberg, Healy. but developed from S.OSTAP & R.Lemay data.
  const double a0 = -4.9564; 
  const double a3 =  0.4031; 
  const double a4 = -0.1010; 
  const double dG = -21172.; //Gibbs energy of dissolution 
  const double R  = R_c;//8.3145; //Universal Gas Constant 8.314472 
  const double X    = a0*I2/(1.0+I2) + a3*I + a4*Pow(I, 3.0/2.0); 
  const double Keq  = Exps(dG/(R*(T_))); //Equilibrium Constant 
  const double BoehmiteSol = 0.96197*cNaOH/(1.0+(Pow(10.0, X)/Keq)); 
  return BoehmiteSol; 
  }

//---------------------------------------------------------------------------

double CAlcanSM::AtoCSaturation(double T_)
  {
  double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AluminaConcSat(T_) / GTZ(C);
  return ACsat;
  }

//---------------------------------------------------------------------------

double CAlcanSM::SSNRatio(double T_)
  {// AtoC actual / AtoC saturation ==> A/ASat
  const double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AtoCSaturation(T_);
  return AtoC() / GTZ(ACsat);
  }

//---------------------------------------------------------------------------

double CAlcanSM::AluminaSSN(double T_)
  {// (Supersaturated alumina) / (free caustic) 
   // The measure is gpl of alumina (as alumina) above the saturation concentration divided by free caustic
  const double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AtoCSaturation(T_);
  const double FC = FreeCaustic(C_2_K(25.0)); 
  const double Al_FC_SSN = C * (AtoC() - ACsat)/GTZ(FC);
  return Al_FC_SSN;
  }

//---------------------------------------------------------------------------

double CAlcanSM::OxalateSolubility(double T_)
  {
//Reference from Denis Audet May 2005:
//F.Tahiani, S.Buckingham, Report KR-86/019, Review of Information on Evaporative Crystallization... 
//This equation is an INTERNAL Alcan equation but based on a collection of available literature.
//NaOX solubility = exp(2.408502 + 0.014688*T in C - 0.003766*Na2SO4Conc - 0.005045*SodiumCarbonateConc + 0.004894*Al2O3Conc - 0.015613*CausticConc + 0.000011096*CausticConc^2)
  const double Tc = K2C(T_);
  LiqConcs25.Converge(MArray(this));
  const double Na2SO4Conc = LiqConcs25.Liq[::SodiumSulphate]  / LiqConcs25.NaFactor[::SodiumSulphate];
  const double Na2CO3Conc = LiqConcs25.Liq[::SodiumCarbonate] / LiqConcs25.NaFactor[::SodiumCarbonate];
  const double Al2O3Conc  = LiqConcs25.Liq[::Alumina]         / LiqConcs25.NaFactor[::Alumina];
  const double C = LiqConcs25.Liq[::CausticSoda];
  const double OxSol = Exps(2.408502 + 0.014688*Tc - 0.003766*Na2SO4Conc - 0.005045*Na2CO3Conc + 0.004894*Al2O3Conc - 0.015613*C + 0.000011096*C*C);
  return OxSol;
  }

//---------------------------------------------------------------------------

double CAlcanSM::FreeCaustic(double T_)
  {
  //throw new MMdlException(0, "FreeCaustic(T) not implemented in Alcan Specie Model");
  //return 0.0;
  return CausticConc(T_) * (1.0 - AtoC()*::SodiumCarbonate.MW/::Alumina.MW);
  }

//---------------------------------------------------------------------------

double CAlcanSM::AtoC()
  {
  LiqConcs25.Converge(MArray(this));
  return LiqConcs25.Liq[::Alumina] / GTZ(LiqConcs25.Liq[::CausticSoda]);
  }

//---------------------------------------------------------------------------

double CAlcanSM::CtoS()
  {
  LiqConcs25.Converge(MArray(this));
  return LiqConcs25.Liq[::CausticSoda]
         / GTZ(LiqConcs25.Liq[::CausticSoda] + LiqConcs25.Liq[::SodiumCarbonate]);
  }

//---------------------------------------------------------------------------

double CAlcanSM::CltoC()
  {
  LiqConcs25.Converge(MArray(this));
  return (LiqConcs25.Liq[SodiumChloride])/GTZ(LiqConcs25.Liq[::CausticSoda]);
  }

//---------------------------------------------------------------------------

double CAlcanSM::Na2CO3toS()
  {
  LiqConcs25.Converge(MArray(this));
  return LiqConcs25.Liq[::SodiumCarbonate]
         / GTZ(LiqConcs25.Liq[::CausticSoda] + LiqConcs25.Liq[::SodiumCarbonate]);
  }

//---------------------------------------------------------------------------

double CAlcanSM::MR()
  {//ratio of Caustic molar flow to liq Alumina molar flow
  double MR = (M[::CausticSoda]/::CausticSoda.MW / 2.0) / GTZ(M[::Alumina]/::Alumina.MW);
  //LiqConcs25.Converge(MArray(this));
  //double MR = (LiqConcs25.Liq[::CausticSoda]/::CausticSoda.MW/2.0) / GTZ(LiqConcs25.Liq[::Alumina]/::Alumina.MW);
  return MR;
  }

//---------------------------------------------------------------------------

double CAlcanSM::AA()
  {//Total Na Conc as Na2O equivilant @25
  double AANa2O = TotalSodiumConc25()*::OccSoda.MW/::SodiumCarbonate.MW; //61.98=MW Na2O, 105.99=MW Na2CO3
  return AANa2O;
  }

//---------------------------------------------------------------------------

double CAlcanSM::TT()
  {//Caustic Conc as Na2O equivilant @25
  LiqConcs25.Converge(MArray(this));
  double TTNa2O = LiqConcs25.Liq[::CausticSoda]*::OccSoda.MW/::SodiumCarbonate.MW; //61.98=MW Na2O, 105.99=MW Na2CO3
  return TTNa2O;
  }

//---------------------------------------------------------------------------

double CAlcanSM::Na2SO4toC()
  {
  LiqConcs25.Converge(MArray(this));
  return (LiqConcs25.Liq[SodiumSulphate])/GTZ(LiqConcs25.Liq[::CausticSoda]);
  }

//---------------------------------------------------------------------------

double CAlcanSM::OrganicO11Conc25()
  {//Organic Na2C5O11 Conc @ 25
  LiqConcs25.Converge(MArray(this));
  double OrganicO11 = LiqConcs25.Liq[::Organics1];
  return OrganicO11;
  }

//---------------------------------------------------------------------------

double CAlcanSM::OrganateConc25()
  {//Organic Na2C5O7 Conc @ 25
  LiqConcs25.Converge(MArray(this));
  double Organate = LiqConcs25.Liq[::Organics];
  return Organate;
  }

//---------------------------------------------------------------------------

double CAlcanSM::OxalateConc25()
  {
  LiqConcs25.Converge(MArray(this));
  double Oxalate = LiqConcs25.Liq[::SodiumOxalate];
  return Oxalate;
  }

//---------------------------------------------------------------------------

double CAlcanSM::TotalOrganicsConc25()
  {
  LiqConcs25.Converge(MArray(this));
  double Organics = LiqConcs25.Liq[::Organics] + LiqConcs25.Liq[::Organics1] + LiqConcs25.Liq[::SodiumOxalate];
  return Organics;
  }

//---------------------------------------------------------------------------

double CAlcanSM::ChlorineConc25()
  {
  LiqConcs25.Converge(MArray(this));
  double Chlorine = LiqConcs25.Liq[::SodiumChloride];
  return Chlorine;
  }

//---------------------------------------------------------------------------

double CAlcanSM::SulphateConc25()
  {
  LiqConcs25.Converge(MArray(this));
  double Sulphate = LiqConcs25.Liq[::SodiumSulphate];
  return Sulphate;
  }

//---------------------------------------------------------------------------

double CAlcanSM::LDensity25()
  {
  return (get_Density(MP_Liq, C_2_K(25.0), Pressure, NULL));
  }

double CAlcanSM::SLDensity25()
  {
  return (get_Density(MP_SL, C_2_K(25.0), Pressure, NULL));
  }

// --------------------------------------------------------------------------
//
//double CAlcanSM::SpecieConc(double T_, int iSpNo, PhMask PhaseM)
//  {
//  if (!gs_MVDefn[iSpNo].IsLiq())
//    return SpModelEx::SpecieConc(T_, iSpNo, PhaseM);
//
//  LiqConcs25.Converge(MArray(this));
//
//  double C = LiqConcs25.Liq[iSpNo];
//
//  if (fabs(T_ - C_2_K(25.0))>0.1)
//    return C * LiqConcs25.LiquorDensity(T_, MArray(this))/ LiqConcs25.Density25;
//  return C;
//  }
//
// --------------------------------------------------------------------------

//double CAlcanSM::PhaseConc(double T_, PhMask Ph, PhMask PhaseM)
//  {
//  // To Be Improved
//  return SpModelEx::PhaseConc(T_, Ph, PhaseM);
//  }

//--------------------------------------------------------------------------

double CAlcanSM::GrowthRate()
  {
  const double MW_Al2O3     = ::Alumina.MW;          //101.961278
  const double MW_Al2O3_3H2O= ::THA.MW;              //156.007118
  switch (sm_iGrowthMethod)
    {
    case GRM_Original:
      {//perhaps delete this?
      const double T   = Temperature; //Kelvin
      const double CAl = AluminaConc(T);
      const double CEq = AluminaConcSat(T);
      if (CAl > CEq)
        {
        const double Dens    = THADens(T)/1000.0; //2.42
        const double K       = sm_Morphology * 8.267e15* exp(-12763/(T-0.16));
        const double CCaus   = CausticConc(T);
        const double FC      = CCaus - 1.039*CAl;
        const double FCeq    = CCaus - 1.039*CEq;
        const double d       = (CAl/FC - CEq/FCeq) / GTZ(CEq/FCeq);
        const double RatePerHour = 2.0*(MW_Al2O3_3H2O/MW_Al2O3)/Dens * K * Sqr(d);
        return RatePerHour/3600.0; // um/s
        }
      break;
      }
    case GRM_Nov2003:
    case GRM_Hyprod:
      {
      const double i = IonicStrength();
      if (i>0.0)
        {
        const double T       = Temperature; //Kelvin
        const double CAl     = AluminaConc(C2K(25.0));
        const double CEq     = AluminaConcSat(T);
        const double CCaus   = CausticConc(C2K(25.0));
        const double FC      = CCaus - 1.039 * CAl; 
        const double FCeq    = CCaus - 1.039 * CEq; 
        //K := flowsheet.morphology * 66.14 * 0.01145 * 8.267E15 * Exp(-12789 / (273 + Temperature)) * Sqr((Cal / FC - Ceq / FCeq) / (Ceq / FCeq)); 
        const double d       = (CAl/GTZ(FC) - CEq/GTZ(FCeq)) / GTZ(CEq/GTZ(FCeq));
        double T_ = (sm_iGrowthMethod==GRM_Hyprod ? T-0.16 : T);
        const double K       = sm_Morphology * 1.0143E16 * exp(-11800.0/T_) * Sqr(d) / Sqr(i); 
        const double Dens    = THADens(T)/1000.0; //2.42
        double RatePerHour   = 2.0*(MW_Al2O3_3H2O/MW_Al2O3)/Dens * K; 
        return RatePerHour/3600.0; // um/s
        }
      break;
      }
    }
  return 0.0; 
  };

//--------------------------------------------------------------------------

double CAlcanSM::AgglomerationRate()
  {
  return 0;
  };

//--------------------------------------------------------------------------

double CAlcanSM::NucleationRate(double SpecSurface)
  {
  double T=Temperature;
  double T_ = (sm_iGrowthMethod==GRM_Hyprod ? T-0.16 : T);
  double RatePerHour=sm_Nucleation*exp(34194.0/(T_)-96.4)*Sqr(AluminaConcSat(T)-AluminaConc(T)) *
         pow(SolidsConc(T)*SpecSurface, 3.17);
  return RatePerHour/3600.0;
  };


//==========================================================================
//
//
//
//==========================================================================

#ifdef emailfromjohngorst
/* 11 Feb 2002: email from John Gorst:*/

//Density

/*Liquor Density with mass fractions
XAl  = mass fraction AL2O3 aq
XNa  = mass fraction soda as Na2O
XNaC = mass fraction non caustic soda species as Na2O
Temp in degrees C*/
double LiqDenMassFrc(double TempC, double XNa2O, double XAl, double XNaC)
  {
  double XNa = XNaC + XNa2O;
  double RHO = (1+1.71*XNaC)*(1006+(1200+872*XNa-0.291*TempC)*XNa
             +(1060-0.664*TempC)*XAl-(0.242+0.00238*TempC)*TempC);
  return RHO;
  }


/*
Alumina Equilibrium

NaOgpl   =  g/l Na2O @25 C
NaCl     =  g/l NaCl  @25 C
NaC      =  g/l Na2CO3  @25 C
NaS      =  g/l Na2SO4  @25 C
TOC      =  g/l Organics including Na2C2O4  as C  @25 C
*/
double LiqAlEqGov(double temp, double NaOgpl, double NaClgpl, double NaCgpl, double NaSgpl, double TOC)
{
  double I,logGamOH,GamOH,Keq,SodaNaC;

  const double alpha0 = -8.9263;
  const double alpha3 = -0.7288;
  const double alpha4 = 0.1908;
  const double K1 = 0.6499;
  const double K2 = 0.9909;
  const double K3 = 0.9107;
  const double K4 = 1.5062;
  const double K5 = 0.01887;

  // Soda as Carbonate
  SodaNaC = NaOgpl*105.99/61.98;

  I = (K5*SodaNaC + K1*NaClgpl/58.44 + K2*NaCgpl/12.01 + K3*NaSgpl/142.04 + K4*0.01887*TOC);

  logGamOH = alpha0*sqrt(I)/(1 + sqrt(I)) - alpha3*I  - alpha4*pow(I,1.5);

  GamOH = pow(10.0L, logGamOH);

  Keq = exp(-32367.0/(temp+273.15)/8.314);
  double dummy = SodaNaC/(1.0 + GamOH/Keq)*0.96197;

  dummy=0.96197*SodaNaC/(1.0 + GamOH/Keq);
  return  dummy;

}

/**********************************************************************
This e-mail and any attachments may be confidential.
You must not disclose or use the information contained in
this e-mail if you are not the intended recipient.

If you have received this e-mail in error, please notify
Nabalco Pty Ltd immediately and delete the e-mail and all
copies.

Nabalco does not guarantee that this e-mail is virus or
error free.  The attached files are provided and may only
be used on the basis that the user assumes all
responsibility for any loss, damage or consequence
resulting directly or indirectly from the use of the attached
files, whether caused by the negligence of the sender or
not.  The content and opinions in this e-mail may be those
of the author and are not necessarily those of Nabalco.*/


#endif

#ifdef emailfromjohngorst29July03
/* 29 July 2003: email from John Gorst for new density correlation:*/
LiqDenConc ( TempC, Algpl, NaTTgpl, NaCgpl, NaOXgpl, NaClgpl, ORGCgpl, NaSgpl )

Return Value

Calculates a rigorous Liquor Density in g/cc

Arguments

TempC : Temperature in degrees C.
Algpl : g/l Al2O3 at 25 C
NaTTgpl : TT Na g/l as Na2O at 25 C
NaCgpl : Na2CO3 Concentration as Na2CO3 in g/l @ 25 degrees C. This parameter is optional. The default value is NaTTgpl * 0.32
NaOxgpl : Na2C2O4 Concentration in g/l @ 25 degrees C. This parameter is optional. The default value is 0.0 g/l
NaClgpl : NaCL Concentration in g/l @ 25 degrees C. This parameter is optional. The default value is 0.0 g/l
ORGCgpl : Non Oxalate Organic Carbon (NOOC) as C in g/l @ 25 degrees C. This parameter is optional. The default value is 0.0 g/l
NaSgpl : Na2SO4 Concentration as in g/l @ 25 degrees C. This parameter is optional. The default value is 0.0 g/l

Notes

Created by John Gorst from the Alcan Density Correlation described in

Cousineau, P.G., Alcan Research Report, Physical Properties of Bayer Liquors and Slurries, KR-87/017, 94/08/25

and reviewed in

Pilgrim, D. "2003-DLP-Gove Bayer Liquor Density.doc", Alcan Gove Technical Memo. 2003

Equation

BAYER_den = 1.02767 - 0.0007015008 * Temp_C - 0.0000007140195 * Temp_C ^ 2 + 0.0003631629 * Al2O3_gpl + 0.0000003396664 * Al2O3_gpl ^ 2 - 0.00005931031 * Caust_gpl + 0.000794539 * TTS_gpl - 0.0000002692805 * TTS_gpl ^ 2 + 0.002644698 * OC_gpl + 0.0000003134875 * Temp_C * Caust_gpl + 0.04716374 * Al2O3_gpl / Caust_gpl

where

Caust_gpl: = NaTTgpl *106/62
TTS_gpl : = NaCgpl *+ Caust_gpl + NaClgpl*106/117.0 + NaSgpl*106/148.0
OC_gpl : = OC_gpl + NaOxgpl*24.0/138.0
#endif

#ifdef emailfromjohngorst5Aug03
/* 5 August 2003: email from John Gorst for new density correlation:*/
LiqDenMassFrc ( TempC, xNa2O, xAl, xNaC, xOrgC )

Return Value

Calculates a rigorous Liquor Density from mass fractions in g/cc

Arguments

TempC  Temperature units are degrees C.
xNa2O  : Caustic mass fraction = mass Caustic as Na2CO3  / Total Mass
xAl : Al2O3 mass fraction = mass Aqueous Alumina as Al2O3  / Total Mass
xNaC : Carbonate mass fraction = mass Na2CO3  / Total Mass
xOrgC :  Total Organic Carbon Mass fraction = mass TOC as C / Total Mass

Notes

Created by John Gorst from Data presented by

Cousineau, P.G., Alcan Research Report, Physical Properties of Bayer Liquors and Slurries, KR-87/017, 94/08/25

and

Pilgrim, D. "2003-DLP-Gove Bayer Liquor Density.doc", Alcan Gove Technical Memo. 2003

Data was regressed using Matlab Statistical Toolbox

Equation

1.002545 + 0.831696*xAl + 1.544015*NaC + 1.44104*xNa2O + 3.220858*xAl^2 + 28.1982*xOrgC^2 -9.186E-6*TempC^2 -127.655*xOrgC^3 + 4.88E-08*TempC^3 -8.41345*xAl*xOrgC -14.9966*xNaC*xOrgC
#endif

//
