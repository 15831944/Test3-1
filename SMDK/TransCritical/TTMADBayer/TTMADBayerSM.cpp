//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//    Alumina2 Generic Bayer Model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-08-29 16:45:52 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================
#include "stdafx.h"
#include "TTMADBayerSM.h"
#include "math.h"
#include "md_headers.h"
#pragma comment(lib, "rpcrt4.lib")

#define FORCECNVG 0
//#pragma optimize("", off)

#if (1 && DBG_MVECTOR)
#define DUMPIT0(S, X)           DumpIt(S,X)
#define DUMPIT1(S, X, A)        DumpIt(S,X,A)
#define DUMPIT2(S, X, A, B)     DumpIt(S,X,A,B)
#define DUMPIT3(S, X, A, B, C)  DumpIt(S,X,A,B,C)
#else
#define DUMPIT0(S, X)           X
#define DUMPIT1(S, X, A)        X
#define DUMPIT2(S, X, A, B)     X
#define DUMPIT3(S, X, A, B, C)  X
#endif
//===========================================================================
//
//
//
//===========================================================================

DEFINE_SPECIEMODEL(Bayer)
IMPLEMENT_SPECIEMODEL(Bayer, "BayerSM", "*Bayer", "SMDK Generic Bayer Specie Model", DLL_GroupName)

//===========================================================================
// Specie Access
static MInitialiseTest InitTest("BayerSMClass");



MSpeciePtr   Water             (InitTest, "H2O(l)",        false);
MSpeciePtr   Alumina           (InitTest, "NaAl[OH]4(l)",  false);
MSpeciePtr   CausticSoda       (InitTest, "NaOH(l)",       false);
MSpeciePtr   SodiumChloride    (InitTest, "NaCl(l)",       false);
MSpeciePtr   SodiumSulphate    (InitTest, "Na2SO4(l)",     false);
MSpeciePtr   SodiumCarbonate   (InitTest, "Na2CO3(l)",     false);
MSpeciePtr   SodiumOxalate     (InitTest, "Na2C2O4(l)",    false); //organic
MSpeciePtr   SodiumSilicate    (InitTest, "Na2SiO3(l)",    false); 
MSpeciePtr   Organics          (InitTest, "Na2C5.2O7.2(l)", false); //organic
MSpeciePtr   OccSoda           (InitTest, "NaOH*(s)",       false);
MSpeciePtr   THA               (InitTest, "Al[OH]3(s)", false);
MSpeciePtr   AluminaSolid      (InitTest, "Al2O3(s)",      false);

MSpeciePtr   Steam             (InitTest, "H2O(g)",        true);

// ==========================================================================
// Specie Properties Modal Global Parameters

const double MW_C        = 12.011;
const double MW_Cl       = 35.4527;
const double MW_SO4      = 96.0636; //32.066+(4*15.9994)

enum ASM_AlSolubilityMethod { ASM_Rosenberg, ASM_MAD99, ASM_MAD05};
enum TSE_TSEMethod { TSE_Original, TSE_97 };

double Bayer::sm_dMinSolCp      = 1.046;
double Bayer::sm_dBPE_Factor  = 1.;
double Bayer::sm_dRho_Factor  = 1.;
double Bayer::sm_dH2OTestFrac0  = 0.98;
double Bayer::sm_dH2OTestFrac1  = 0.95;


double Bayer::sm_dK1_BoundSodaCalc = 2.35 ;
double Bayer::sm_dGrowth_TOC =	    0.00 ;
double Bayer::sm_dGrowth_s =	    -1.00;
double Bayer::sm_dGrowth_fc =	    -2.00;
double Bayer::sm_dGrowth_eq =	    0    ;
double Bayer::sm_dGrowth_n =	    1.70 ;
double Bayer::sm_dGrowth_sol =	    1.00 ;
double Bayer::sm_dGrowth_ssa =	    0.60 ;

byte Bayer::sm_iASatMethod      = ASM_MAD99;
byte Bayer::sm_iTSEMethod       = TSE_97;

// ==========================================================================
//
// A helper class to assist with calculating iterative concentration and density
// calculations.
//
// ==========================================================================

bool   CBayerConcs::NaFactorOK=0;
MArray CBayerConcs::NaFactor;

CBayerConcs::CBayerConcs(Bayer *pMdl)
  {
  MInitialise(); // this must be called to initialise the SMDK library if neccessary
  pBayerMdl = pMdl;
  Zero();
  denOld = 1000.;
  if (!NaFactorOK) {
    for (int sn=0; sn<gs_MVDefn.Count(); sn++)
      NaFactor[sn]=1.0; //For all the species without sodium ions
    NaFactor[::CausticSoda]     = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::CausticSoda.MW);      
    NaFactor[::SodiumCarbonate] = 1.0;                                                        
    NaFactor[::SodiumOxalate]   = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumOxalate.MW);    
    NaFactor[::SodiumChloride]  = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumChloride.MW);   
    NaFactor[::SodiumSulphate]  = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumSulphate.MW);
    NaFactor[::Alumina]  =  ::AluminaSolid.MW / (2.0 * ::Alumina.MW);
    NaFactor[::SodiumSilicate]  = 1.0 * ::SodiumCarbonate.MW / (1.0 * ::SodiumSilicate.MW);
    NaFactor[::Organics]       = 1.0 * ::SodiumCarbonate.MW / (1.0 * ::Organics.MW);    

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

bool CBayerConcs::Converge(MArray & MA, bool SetValid)
  {
  //if (0 || pBayerMdl->Dbg.Marked())
  //  pBayerMdl->Dbg.PrintLn("%3s %-20s  Rho25) %.14g", ">", "", Density25);
  double TLiq = MA.Mass(MP_Liq);
  if (TLiq<1.0e-9)
    {
    Zero();
    pBayerMdl->SetStateValid(0, SetValid);
    return true;
    }

  static MToleranceBlock s_Tol(TBF_Both, "BABayerSM:Density", 1e-12, 1e-8, 100);

  const double Tc = 25.0;
  const double T_ = C2K(Tc);
  // Converge Liquor Conc. All sodium concentrations expressed as Na2CO3
  int IterCount = s_Tol.GetMaxIters();//100;
  double OldDensity = Density25;
  while (1)
    {
    Density25 = Range(0.0001, Density25, 10000.0);
    for (int sn=0; sn<gs_MVDefn.Count(); sn++) 
      {
      if (gs_MVDefn[sn].IsLiquid())
        {
        Liq[sn] = MA[sn] / TLiq * Density25 * NaFactor[sn];
        }
      }
    Liq[CausticSoda] = (MA[CausticSoda] + MA[Alumina]*::CausticSoda.MW/::Alumina.MW)/TLiq*NaFactor[::CausticSoda]*Density25;


    Density25 = LiquorDensity(T_, MA);
    if  (fabs(OldDensity - Density25) < s_Tol.GetAbs() || --IterCount==0) break;
    OldDensity = Density25;

    } // end of while
  Density25 = Max(0.001, Density25);
  //if (0 || pBayerMdl->Dbg.Marked())
  //  pBayerMdl->Dbg.PrintLn("%3s %-20s  Rho25) %.14g", "<", "", Density25);

  pBayerMdl->SetStateValid(0, SetValid);
  return (IterCount>=0);
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
//+ MA[Organics]       * NaFactor[::Organics]
+ MA[Organics]      * NaFactor[::Organics] );
//+ MA[SodiumSilicate] * NaFactor[::SodiumSilicate] );
return TSodium;
  }

//---------------------------------------------------------------------------

double CBayerConcs::LTotalInorganicSodium(MArray & MA)
  {
  //Expressed as Na2CO3
  double TInorganicSodium =
    ( MA[CausticSoda]    * NaFactor[::CausticSoda]
  + MA[SodiumCarbonate]
+ MA[SodiumChloride] * NaFactor[::SodiumChloride]
+ MA[SodiumSulphate] * NaFactor[::SodiumSulphate]);


return TInorganicSodium;
  }

// --------------------------------------------------------------------------


double CBayerConcs::LiquorDensEqn1(double T_C,
                                   double A, double C,
                                   double L1,    // Carbonate
                                   double L2,    // Chloride
                                   double L3,    // Sulphate
                                   double L4,    // Oxalate
                                   double L5     // Organic Soda
                                   )
  {
  double U, V, W, Z;  
  double A1 = 0.5 * (1. - 0.0006823882 * (T_C - 20.) 
    - 0.000004896378 * pow (T_C - 20.,  2));
  // "A" for other Caustics & Temps
  double A2 = 0.5 * (1. - 0.000952832 * (T_C - 20.) 
    - 0.00000264664 * pow (T_C - 20.,  2));

  // The following is to provide a smooth transition between a1 and a2
  // with no step discontinuity.  This is done by cubic spline interpolation
  // between 95 and 105 gpl Caustic, and between 146 and 156 °C.
  double B1 = (C - 95.) / 10.;
  double B2 = (T_C - 146.) / 10.;
  if ( B1 < 0 )  B1 = 0.;
  if ( B1 > 1 )  B1 = 1.;
  if ( B2 < 0 )  B2 = 0.;
  if ( B2 > 1 )  B2 = 1.;

  if ( (B2 > B1) ) B1 = B2;
  U = A1 + 6. * B1*B1 * (0.5 - B1 / 3.) * (A2 - A1);
  //Determine V:
  V = (C - 130.) * 0.00004;
  //Determine W:
  W = 0.0005 * sqrt(1000000. 
    + 4000. * (0.8347 * C + 
    (0.74 + C * 0.00042) * A + 
    1.048 * L1 
    + 0.8007 * L2 + 
    1.2517 * L3 + 1.35 * L4+ 1.35*L5));   
  //Determine Z:
  if ( L1 < C ) {

    Z = 1.0018 * pow(10.,(-3.36587 + 0.01136 * L1));
    }
  else
    Z = 0.0004306555;  // C/S = 1, ratio should never be below 0.5
  return 998.2 * (U + V + W + Z);
  }



// --------------------------------------------------------------------------

double CBayerConcs::LiquorDensity(double T_, MArray & MA)
  {
  /*Liquor Density with mass fractions*/


  double Tc = K2C(T_);
  double C = Liq[CausticSoda];
  double A = Liq[Alumina];
  double CO4 = Liq[SodiumCarbonate];
  double Cl = Liq[SodiumChloride];
  double SO4 = Liq[SodiumSulphate];
  double Ox  = Liq[SodiumOxalate];
  double TOC =  Liq[Organics];
  return  LiquorDensEqn1( Tc, A, C, CO4, Cl, SO4, Ox, TOC);


  }

//===========================================================================
//
//
//
//===========================================================================

Bayer::Bayer(TaggedObject *pNd) : LiqConcs25(this)
  {
  MInitialise(); // this must be called to initialise the SMDK library if neccessary

  fDoCalc  = false;
  dRqd_AtoC = 0.4;
  dRqd_C    = 220.0;
  dRqd_CtoS = 0.85;
  dRqd_Sul  = 20.0;
  dRqd_Sil  = 1.0;
  dRqd_Ox   = 20.0;
  dRqd_Org  = 12.0;
  dRqd_Salt = 5.0;
  dRqd_SolFrac  = 0.0;
  }

//---------------------------------------------------------------------------

Bayer::~Bayer()
  {
  }

//---------------------------------------------------------------------------

bool Bayer::get_IsBaseClassOf(LPCTSTR OtherProgID)
  {
  return false;
  //return S_OK;
  }

//---------------------------------------------------------------------------

LPCTSTR Bayer::get_PreferredModelProgID()
  {
  return NULL;
  }

//---------------------------------------------------------------------------

bool Bayer::ValidateDataFields()
  {
  bool OK=MSpModelBase::ValidateDataFields();
  LiquorComposition(fDoCalc);
  return OK;
  }

//---------------------------------------------------------------------------

double Bayer::get_Density(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  const double MSol=(Phases & MP_Sol) ? MA.Mass(MP_Sol) : 0.0;
  const double MLiq=(Phases & MP_Liq) ? MA.Mass(MP_Liq) : 0.0;
  const double MGas=(Phases & MP_Gas) ? MA.Mass(MP_Gas) : 0.0;
  const double MTot=GTZ(MSol+MLiq+MGas);
  const double MSL = MSol+MLiq;
  if (MSL<UsableMass || MSL/MTot<1.0e-6)
    return MSpModelBase::get_Density(Phases, T, P, &MA);

  const double WaterFrac = (MLiq>1e-6 ? MA[Water]/MLiq : 0.0);
  if (WaterFrac>sm_dH2OTestFrac0)
    return MSpModelBase::get_Density(Phases, T, P, &MA);

  const double FSol=MSol/MTot;
  const double FLiq=MLiq/MTot;
  double Dl=1.0;
  if (FLiq>1.0e-9)
    {
    CheckConverged(pMA);

    Dl = LiqConcs25.LiquorDensity(T, MA);
    if (WaterFrac>sm_dH2OTestFrac1)
      {
      const double Std_Dl = MSpModelBase::get_Density(MP_Liq, T, P, &MA);
      const double Std_Prop = Min(1.0, (WaterFrac-sm_dH2OTestFrac1)/(sm_dH2OTestFrac0-sm_dH2OTestFrac1));
      Dl = (Dl*(1.0-Std_Prop)) + (Std_Dl*Std_Prop);
      }
    }

  return DUMPIT0("Density", DensityMix(FSol, dNAN, FLiq, Dl, (1.0-FSol-FLiq), dNAN, T, P, MA));
  }

//---------------------------------------------------------------------------

double Bayer::LiqCpCalc(MArray & MA, double Tc)
  {
  CheckConverged(&MA);

  const double A = LiqConcs25.Liq[::Alumina];
  const double C = LiqConcs25.Liq[::CausticSoda];
  const double K1 = 0.99639 - 3.90998e-4 * C - 5.382e-4 * A + (2.46493e-7 * C + 5.7186e-7 * A) * C;
  const double K2 = -1.51278e-4 - 1.8658e-7 * A + 1.07766e-7 * C;
  const double K3 = 2.1464e-6;
  return 4.184*(K1 + Tc*(K2+Tc*K3));
  }

double Bayer::LiqHCalc(MArray & MA, double Tc)
  {
  CheckConverged(&MA);

  const double A = LiqConcs25.Liq[::Alumina];
  const double C = LiqConcs25.Liq[::CausticSoda];
  const double K1 = 0.99639 - 3.90998e-4 * C - 5.382e-4 * A + (2.46493e-7 * C + 5.7186e-7 * A) * C;
  const double K2 = -1.51278e-4 - 1.8658e-7 * A + 1.07766e-7 * C;
  const double K3 = 2.1464e-6;

  const double ld25 = LiqConcs25.LiquorDensity(C2K(25), MA);
  const double fc = FreeCaustic(C2K(Tc), &MA);



  return /*0.02871*18.86917*fc*fc/ld25 +*/ 4.184*(K1 + Tc*(K2/2+Tc*K3/3))*Tc;
  }


//---------------------------------------------------------------------------

double Bayer::get_msEnthalpy(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

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
    Hl = LiqHCalc(MA, Tc) /*LiqCpCalc(MA, Tc) * Tc*/;
    if (WaterFrac>sm_dH2OTestFrac1)
      {
      const double Std_Hl = MSpModelBase::get_msEnthalpy(MP_Liq, T, P, &MA);
      const double Std_Prop = Min(1.0, (WaterFrac-sm_dH2OTestFrac1)/(sm_dH2OTestFrac0-sm_dH2OTestFrac1));
      Hl = (Hl*(1.0-Std_Prop)) + (Std_Hl*Std_Prop);
      }
    }
  double Hs=(FSol>1.0e-9) ? gs_MVDefn.msHz(MP_Sol, T, P, PropOverides, &MA) : 0.0;
  return DUMPIT0("msEnthalpy", msEnthalpyMix(FSol, Hs, FLiq, Hl, (1.0-FSol-FLiq), dNAN, T, P, MA));
  }

//---------------------------------------------------------------------------

double Bayer::get_msEntropy(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_msEntropy(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double Bayer::get_msCp(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  const double MSol=(Phases & MP_Sol) ? MA.Mass(MP_Sol) : 0.0;
  const double MLiq=(Phases & MP_Liq) ? MA.Mass(MP_Liq) : 0.0;
  const double MGas=(Phases & MP_Gas) ? MA.Mass(MP_Gas) : 0.0;
  const double MTot=GTZ(MSol+MLiq+MGas);
  const double MSL = MSol+MLiq;
  if (MSL<UsableMass || MSL/MTot<1.0e-6)
    return MSpModelBase::get_msCp(Phases, T, P, &MA);

  const double WaterFrac = (MLiq>1e-6 ? MA[Water]/MLiq : 0.0);
  if (WaterFrac>sm_dH2OTestFrac0)
    return MSpModelBase::get_msCp(Phases, T, P, &MA);

  const double FSol=MSol/MTot;
  const double FLiq=MLiq/MTot;

  double Cpl=0.0;
  if (FLiq>1.0e-9)
    {
    Cpl = LiqCpCalc(MA, K_2_C(T));
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

double Bayer::get_SaturationT(double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  CheckConverged(&MA);

  double PureSatT = MSpModelBase::get_SaturationT(P, &MA);
  if (MA.Mass(MP_Liq)/GTZ(MA.Mass())<1.0e-6)
    return DUMPIT1("SaturationTPure", PureSatT, P);


  const double TotNa2C = NaOnCS();
  double SatT = (4737.8+0.479859*LiqConcs25.Liq[::CausticSoda]*TotNa2C)/
    (17.43407-log(Max(1.0, P)));
  SatT = Max(SatT, PureSatT);
  return DUMPIT3("SaturationT", SatT, P, TotNa2C, LiqConcs25.Liq[::CausticSoda]);

  }

//---------------------------------------------------------------------------

double Bayer::get_SaturationP(double T, MArray *pMa)
  {

  return DUMPIT1("SaturationP", exp ( 17.43407 - ( 4737.8 + 0.479859 * LiqConcs25.Liq[::CausticSoda] * NaOnCS() ) / Max(200.0, T) ), T);

  }

//---------------------------------------------------------------------------

double Bayer::BoilPtElev(MArray & MA, double P)
  {

  double PureSatT = MSpModelBase::get_SaturationT(P, &MA);

  const double Sodium = TotalNa25();
  return DUMPIT1("BoilPtElev", get_SaturationT(P, &MA)  -  PureSatT, P);
  }

//---------------------------------------------------------------------------

double Bayer::get_DynamicViscosity(long Phases, double T, double P, MArray *pMA)
  {
  if (Phases & MP_Gas)
    return MSpModelBase::get_DynamicViscosity(Phases, T, P, pMA); //todo : check!

  MArray MA(pMA ? (*pMA) : this);

  CheckConverged(&MA);

  double Tc = Max(-41.0, K_2_C(T));
  const double C = LiqConcs25.Liq[::CausticSoda];
  const double A = LiqConcs25.Liq[::Alumina];

  return exp ( 0.58-1.816*log(Tc+42.0) + 
    (0.5*C + 0.43*A) / (Tc+42.0) );


  }

//---------------------------------------------------------------------------
//  Not implemented; use base case
double Bayer::get_ThermalConductivity(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_ThermalConductivity(Phases, T, P, pMA);
  }

//===========================================================================
//
//
//
//===========================================================================
// The properties that are defined for visibility in the SysCAD Access properties window
// in addition, these properties are accessible in code by name.

enum 
  {
  idASatMethod         ,
  idTSEMethod       	,





  idBPE_Factor,
  idRho_Factor,
  idMinSolCp        	,

  idK1_BoundSodaCalc,
  idGrowth_TOC,
  idGrowth_s,
  idGrowth_fc,
  idGrowth_eq,
  idGrowth_n,
  idGrowth_sol,
  idGrowth_ssa,

  idDefineLiquor		  ,
  idRqd_AtoC			  ,
  idRqd_CtoS				  ,
  idRqd_C					    ,
  idRqd_Sul				  ,
  idRqd_Salt				    ,
  idRqd_Ox			  ,
  idRqd_Org				    ,
  idRqd_Sil				    ,
  idRqd_SolFrac			  ,

  idAluminaConc25			,
  idCausticConc25			,
  idSodaConc25				  ,
  idAtoC					,
  idCtoS					,
  idCltoC	      ,
  idTOC25					    ,
  idCarbonateConc25		,
  idSolidsConc25			  ,
  idFreeCaustic25      ,
  idDigAtoCEquil,
  idNaOnCS, 
  idTOS25					    ,
  idNaSO4Conc25			  ,
  idNaClConc25				  ,
  idNa2C2O4Conc25			,
  idTOStoTOC           ,

  idSeparator1				  ,

  idOxalateConc25			,
  idTotalOrganics25		,
  idChlorineConc25			,
  idSulphateConc25			,
  idTotalNa25				  ,
  idOrganateConc25			,

  idSeparator2				  ,

  idLVolume25				  ,
  idSLVolume25				  ,
  idLDensity25 			  ,
  idSLDensity25			  ,

  idSeparator3				  ,	
  // At temperature  
  idAluminaConc			  ,
  idCausticConc			  ,
  idSodaConc				    ,
  idTOC					      ,
  idSolidsConc				  ,
  idSodiumCarbonateConc,

  idSeparator4				  ,	


  //  idSeparator5				  ,	

  idAluminaConcSat   ,
  idAtoCSaturation			,
  idSSNRatio				    ,
  idAluminaSSN ,
  idIonicStrength    ,
  idBoilPtElev				  ,
  idPFAtoCEquil,
  idPFSSN,
  idBoundSodaSat  ,
  idTHAMassFlow   			,
  idTHADens				    ,


  idMPI_EndOfProps 
  };

//---------------------------------------------------------------------------

long Bayer::DefinedPropertyCount()
  {
  return idMPI_EndOfProps + 1 + MSpModelBase::DefinedPropertyCount();
  }

//---------------------------------------------------------------------------

/*static MPropertyInfo::MStringValueP SVOrg[]={
//{"Organics and Ratio", DLM_TotOrganics},
{ "TOC and Oxalate", DLM_TOC},
{0}};*/
static MPropertyInfo::MStringValueP SVASat[]=
  {
    { "Rosenberg", ASM_Rosenberg },
    { "MAD99", ASM_MAD99   },
    { "MAD05", ASM_MAD05 },
    {0}
  };
static MPropertyInfo::MStringValueP SVTSE[]=
  {
    { "Original", TSE_Original },
    { "Revised97", TSE_97   },
    {0}
  };


#define MP_GPH  MP_GlobalProp|MP_Parameter|MP_InitHidden
#define MP_CP   MP_ConfigProp|MP_Parameter
#define MP_RN   MP_Result|MP_NoFiling
#define MP_RNH  MP_Result|MP_NoFiling|MP_InitHidden

long Bayer::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {//define a list of all properties: This sets what is displayed in the access window.
  long Inx=Index-MSpModelBase::DefinedPropertyCount();
  switch(Inx)
    {
    case idASatMethod :
      Info.SetText("Bayer Properties");
      Info.Set(ePT_Long, "", "ASatMethod",SVASat,    MP_GlobalProp|MP_Parameter,  "Global ASat Method"); return Inx;
    case idTSEMethod  	: Info.Set(ePT_Long, "", "TSEMethod", SVTSE,   MP_GlobalProp|MP_Parameter,  "Global TSE method"); return Inx;

    case idBPE_Factor   : Info.Set(ePT_Double,    "",  "BPE_Factor", MC_Frac, "%", 0,0,   MP_GPH, "BPE Factor"); return Inx;
    case idRho_Factor   : Info.Set(ePT_Double,    "",  "Rho_Factor", MC_Frac, "%", 0,0,   MP_GPH, "Rho Factor"); return Inx;
    case idMinSolCp   	: Info.Set(ePT_Double,    "", "MinSolCp",  MC_CpMs,  "kJ/kg.C", 0, 0,  MP_GPH, "Global minimum solids Cp"); return Inx;


    case idK1_BoundSodaCalc: Info.Set(ePT_Double,    "", "K1_BoundSodaCalc",  MC_,  "", 0, 0,  MP_GPH, "K1 for bound soda calc"); return Inx;
    case idGrowth_TOC   : Info.Set(ePT_Double,    "", "Growth_TOC",           MC_,  "", 0, 0,  MP_GPH, "Growth TOC"); return Inx;
    case idGrowth_s   	: Info.Set(ePT_Double,    "", "Growth_s",             MC_,  "", 0, 0,  MP_GPH, "Growth S"); return Inx;
    case idGrowth_fc   	: Info.Set(ePT_Double,    "", "Growth_fc",            MC_,  "", 0, 0,  MP_GPH, "Growth fc"); return Inx;
    case idGrowth_eq   	: Info.Set(ePT_Double,    "", "Growth_eq",            MC_,  "", 0, 0,  MP_GPH, "Growth eq"); return Inx;
    case idGrowth_n   	: Info.Set(ePT_Double,    "", "Growth_n",             MC_,  "", 0, 0,  MP_GPH, "Growth n"); return Inx;
    case idGrowth_sol   : Info.Set(ePT_Double,    "", "Growth_sol",           MC_,  "", 0, 0,  MP_GPH, "Growth sol"); return Inx;
    case idGrowth_ssa   : Info.Set(ePT_Double,    "", "Growth_ssa",           MC_,  "", 0, 0,  MP_GPH, "Growth SSA"); return Inx;




    case idDefineLiquor : 
      Info.SetText("--Calculator-------------");
      Info.SetStructName("Calc"); //NB: The "struct name" is used as part of tag!
      Info.Set(ePT_Bool,"", "DefineLiquor",    MC_, "",    0, 0,    MP_CP|MP_CheckBox, ""); return Inx;
    case idRqd_AtoC	    : Info.Set(ePT_Double,   "", "Rqd_A/C",    MC_, "",    0, 0,    MP_CP, "The required Alumina/Caustic Ratio"); return Inx;
    case idRqd_CtoS	    : Info.Set(ePT_Double,   "", "Rqd_C/S",    MC_, "",    0, 0,    MP_CP, "The required Caustic/Soda Ratio"); return Inx;
    case idRqd_C	    : Info.Set(ePT_Double,   "", "Rqd_C",      MC_Conc,  "g/L", 0, 0,    MP_CP, "The required Caustic Conc expressed as Carbonate @ 25"); return Inx;
    case idRqd_Sul	    : Info.Set(ePT_Double,   "", "Rqd_Na2SO4", MC_Conc,  "g/L", 0, 0,    MP_CP, "The required Sodium Sulphate Concentration @ 25"); return Inx;
    case idRqd_Salt	    : Info.Set(ePT_Double,   "", "Rqd_NaCl",   MC_Conc,  "g/L", 0, 0,    MP_CP, "The required Sodium Chloride Concentration @ 25"); return Inx;
    case idRqd_Ox	    : Info.Set(ePT_Double,   "", "Rqd_Oxalate",MC_Conc,  "g/L", 0, 0,    MP_CP, "The required Oxalate Concentration @ 25"); return Inx;
    case idRqd_Org	    : Info.Set(ePT_Double,   "", "Rqd_Organics",    MC_Conc,  "g/L", 0, 0,    MP_CP, "The required Total Non Oxalate Organics "); return Inx;
    case idRqd_Sil	    : Info.Set(ePT_Double,   "", "Rqd_SiO2",   MC_Conc,  "g/L", 0, 0,    MP_CP, "The required Silica Concentration @ 25"); return Inx;
      //    case idRqd_SolConc	    : Info.Set(ePT_Double,   "", "Rqd_SolConc",MC_Conc,  "g/L", 0, 0,    MP_CP|MP_NanOK, "The required Solids Conc (*=unused)"); return Inx;
    case idRqd_SolFrac	    : Info.Set(ePT_Double,   "", "Rqd_SolFrac",MC_Frac,  "%",   0, 0,    MP_CP, "The required Solids Fraction"); return Inx;

    case idAluminaConc25 : 
      Info.SetStructName("Props"); //NB: The "struct name" is used as part of tag! Recomend "Props" for compatability with other properties models.
      Info.SetText("Bayer Liquor Values @ 25");
      Info.Set(ePT_Double,    "A", "AluminaConc",MC_Conc, "g/L",    0, 0,  MP_RN,    "Alumina Concentration @ 25"); return Inx;
    case idCausticConc25    : 

      Info.Set(ePT_Double,    "C", "CausticConc",   MC_Conc, "g/L",    0, 0,  MP_RN,    "Caustic Concentration @ 25"); return Inx;

    case idAtoC		    : Info.Set(ePT_Double,    "", "A/C",  MC_,"",  0, 0,  MP_RN,    "A to C ratio"); return Inx;
    case idCtoS		    : Info.Set(ePT_Double,    "", "C/S",  MC_,"",  0, 0,  MP_RN,    "C to S ratio"); return Inx;
    case idCltoC	    : Info.Set(ePT_Double,    "", "Cl/C", MC_,"",  0, 0,  MP_RN,    "Cl to C ratio"); return Inx;
    case idTOStoTOC 	    : Info.Set(ePT_Double,    "", "TOS/TOC",   MC_,"",  0, 0,  MP_RNH,    "TOS to TOC ratio"); return Inx;
    case idTOC25	    : Info.Set(ePT_Double,    "", "TOC",    MC_Conc, "g/L",    0, 0,  MP_RN,  "Total Organic Carbon Concentration @ 25"); return Inx;
    case idTOS25	    : Info.Set(ePT_Double,    "", "TOS@25",    MC_Conc, "g/L",    0, 0,  MP_RNH,  "Total Organic Soda Concentration @ 25"); return Inx;
    case idNaSO4Conc25	    : Info.Set(ePT_Double,    "", "Sulphate*@25",  MC_Conc, "g/L",    0, 0,  MP_RNH,    "SodiumSulphate Concentration @ 25"); return Inx;
    case idNaClConc25	    : Info.Set(ePT_Double,    "", "Chloride*@25",  MC_Conc, "g/L",    0, 0,  MP_RNH,    "SodiumChloride Concentration @ 25"); return Inx;
    case idNa2C2O4Conc25    : Info.Set(ePT_Double,    "", "Oxalate*@25",	   MC_Conc, "g/L",    0, 0,  MP_RNH,    "SodiumOxalate Concentration @ 25"); return Inx;
    case idFreeCaustic25    : Info.Set(ePT_Double,    "FC", "FreeCaustic", MC_Conc, "g/L",    0, 0,  MP_RNH,    "Free Caustic Concentration @ 25"); return Inx;
    case idSolidsConc25	    : Info.Set(ePT_Double,    "", "SolidsConc",  MC_Conc, "g/L",    0, 0,  MP_RN,    "Solids Concentration @ 25");return Inx;

    case idSeparator1	    : Info.SetText("..."); return Inx;

    case idSodaConc25:
      Info.Set(ePT_Double,    "S", "SodaConc",		MC_Conc, "g/L",    0, 0,  MP_RN,    "Soda Concentration @ 25"); return Inx;
    case idCarbonateConc25  : Info.Set(ePT_Double,    "", "SodiumCarbConc",	   MC_Conc, "g/L",    0, 0,  MP_RN,    "SodiumCarbonate Concentration @ 25"); return Inx;
    case idSulphateConc25   : Info.Set(ePT_Double,    "", "Na2SO4",   MC_Conc, "g/L",    0, 0,  MP_RN,    "SodiumSulphate Concentration @ 25"); return Inx;
    case idChlorineConc25   : Info.Set(ePT_Double,    "", "NaCl",   MC_Conc, "g/L",    0, 0,  MP_RNH,    "SodiumChloride Concentration @ 25"); return Inx;
    case idOxalateConc25    :       Info.SetText("--Concentrations @ 25, as Na2CO3 Equiv----");

      Info.Set(ePT_Double,    "", "Oxalate",   MC_Conc, "g/L",    0, 0,  MP_RN,    "SodiumOxalate Concentration @ 25"); return Inx;
    case idOrganateConc25   : Info.Set(ePT_Double,    "", "Organate@25",	   MC_Conc, "g/L",    0, 0,  MP_RNH,    "Organate Concentration @ 25"); return Inx;
    case idTotalOrganics25  : Info.Set(ePT_Double,    "", "TOOC",    MC_Conc, "g/L",    0, 0,  MP_RN,    "Total Organic Concentration @ 25"); return Inx;
    case idTotalNa25	    : Info.Set(ePT_Double,    "", "TotalNa",MC_Conc, "g/L",    0, 0,  MP_RN,    "Total Sodium Concentration @ 25"); return Inx;

    case idSeparator2  	  : Info.SetText("..."); return Inx;

    case idLVolume25	  : Info.SetText("Bayer Liquor Values @25-----------------");
      Info.Set(ePT_Double,    "", "LVolFlow25",MC_Qv,   "L/s",    0, 0,  MP_RN,    "Liquor Volumetric flowrate @ 25"); return Inx;
    case idSLVolume25	  : Info.Set(ePT_Double,    "", "SLVolFlow25",    MC_Qv,   "L/s",    0, 0,  MP_RN,    "Slurry Volumetric flowrate @ 25"); return Inx;
    case idLDensity25     : Info.Set(ePT_Double,    "", "LRho25",    MC_Rho,  "g/L",    0, 0,  MP_RN,    "Liquor Density @ 25"); return Inx;
    case idSLDensity25    : Info.Set(ePT_Double,    "", "SLRho25",   MC_Rho,  "g/L",    0, 0,  MP_RN,    "Slurry Density @ 25"); return Inx;

    case idSeparator3	  : Info.SetText("..."); return Inx;

    case idAluminaConc	  : Info.SetText("Bayer Liquor Values @ Temp");
      Info.Set(ePT_Double,    "A@T", "AluminaConcT",   MC_Conc, "g/L",    0, 0,  MP_RN,    "Alumina Concentration @ T"); return Inx;
    case idSodiumCarbonateConc : Info.Set(ePT_Double,  "C@T", "Carbonate@T",   MC_Conc, "g/L",    0, 0,  MP_RNH,    "SodiumCarbonate Concentration @ T"); return Inx;
    case idTOC	          : Info.Set(ePT_Double,    "", "TOC@T",MC_Conc, "g/L",    0, 0,  MP_RN,  "Total Organic Carbon Concentration @ T"); return Inx;
    case idSolidsConc	  : Info.Set(ePT_Double,    "", "SolidsConcT",   MC_Conc, "g/L",    0, 0,  MP_RN,    "Solids Concentration @ T"); return Inx;

    case idSeparator4	  : Info.SetText("..."); return Inx;

    case idCausticConc	  : 
      Info.Set(ePT_Double,    "", "CausticConcT", MC_Conc, "g/L",    0, 0,  MP_RN,    "Caustic Concentration @ T"); return Inx;
    case idSodaConc  : Info.Set(ePT_Double,    "S@T", "SodaConcT",    MC_Conc, "g/L",    0, 0,  MP_RN,    "Soda Concentration @ T"); return Inx;

      //    case idSeparator5	  : Info.SetText("..."); return Inx;

    case idBoilPtElev     : 
      Info.Set(ePT_Double,    "", "BoilPtElev",MC_dT,   "C", 0, 0,  MP_RN,  "Boiling Point Elevation"); return Inx;
    case idNaOnCS         : Info.Set(ePT_Double,    "", "NaOnCS",   MC_,  "", 0, 0,  MP_RNH,    "Total Sodium on Caustic"); return Inx;

    case idIonicStrength  : Info.Set(ePT_Double,    "I", "IonicStrength", MC_, "",    0, 0,  MP_RN,  "Ionic Strength"); return Inx;
    case idDigAtoCEquil   : Info.Set(ePT_Double,    "", "DigA/CEquil", MC_, "",    0, 0,  MP_RN,  "Digester A/C Equilibrium"); return Inx;
    case idPFAtoCEquil    : Info.Set(ePT_Double,    "", "PFA/CEquil", MC_, "",    0, 0,  MP_RNH,  "Press Floor A/C Equilibrium"); return Inx;
    case idPFSSN          : Info.Set(ePT_Double,    "", "PFSSN" , MC_, "",    0, 0,  MP_RNH,  "Press Floor Supersaturation"); return Inx;
    case idBoundSodaSat   : Info.Set(ePT_Double,    "", "BoundSodaSat", MC_Frac, "%",    0, 0,  MP_RN,  "Bound Soda Saturation"); return Inx;
    case idAluminaConcSat : Info.SetText("--Other Bayer Liquor Properties @ T-------");
      Info.Set(ePT_Double,    "ASat", "A_Saturation", MC_Conc, "g/L",    0, 0,  MP_RN,  "Alumina Saturation Concentration @ T"); return Inx;
    case idAtoCSaturation : Info.Set(ePT_Double,    "A/CSat", "A/C_Saturation", MC_,"",  0, 0,  MP_RNH,  "Alumina Saturation to Caustic ratio @ T"); return Inx;
    case idSSNRatio       : Info.Set(ePT_Double,    "", "SSN_Ratio",  MC_,"",  0, 0,  MP_RN,  "A/C actual to ASat/C ratio @ T"); return Inx;
    case idAluminaSSN     : Info.Set(ePT_Double,    "", "Alumina_SSN",  MC_,"",  0, 0,  MP_RN,  "Supersaturated Alumina/FreeCaustic"); return Inx;
    case idTHAMassFlow    : Info.Set(ePT_Double,    "", "THAMassFlow",    MC_Qm,   "kg/s",   0, 0,  MP_RN,    "THA flow rate"); return Inx;
    case idTHADens        : Info.Set(ePT_Double,    "", "THADens",   MC_Rho,  "kg/m^3", 0, 0,  MP_RNH,    "THA Density"); return Inx;


    case idMPI_EndOfProps : return MPI_EndOfProps;    

    default: 
      {
      char Buff[16];
      sprintf(Buff, "Unused%d", Inx);
      Info.Set(ePT_Double, "", Buff, MC_, "", 0, 0, MP_Null, ""); 
      return Inx;
      }
    }
  return MPI_MoreProps;
  }

//---------------------------------------------------------------------------

DWORD Bayer::GetPropertyVisibility(long Index)
  {//determine visibility of list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    //case idDefnLiqMethod: 
    case idRqd_AtoC: 
    case idRqd_CtoS: 
    case idRqd_C: 
    case idRqd_Ox: 
    case idRqd_Org: 
    case idRqd_Sul: 
    case idRqd_Salt: 
    case idRqd_Sil: 
      //    case idRqd_SolConc: 
    case idRqd_SolFrac: return fDoCalc ? ePVis_All : (ePVis_DynFull|ePVis_DynFlow|ePVis_Probal|ePVis_File);
      //case idRqd_Organic: 
      //case idRqd_OrgRatio: return fDoCalc ? ePVis_All : (ePVis_DynFull|ePVis_DynFlow|ePVis_Probal|ePVis_File);
    default:  return ePVis_All; 
    }
  return ePVis_All;
  }

//---------------------------------------------------------------------------
// Stringifying operators GV...  Get Value
// Just tidy up the case structure.

#define GV(x, type) case id##x :Value = type##x; return 
#define GVAL(x)   case id##x : Value = d##x; return
#define GVAL2(v, x) case id##v : CheckConverged(); Value = x; return
#define GVALF(x) case id##x : Value = x##(); return
#define GVALFT(x) case id##x : Value = x##(T); return

void Bayer::GetPropertyValue(long Index, ULONG Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {//define method of retrieving values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    GV(ASatMethod, sm_i);
    GV(TSEMethod,  sm_i);

    GV(BPE_Factor, sm_d);
    GV(Rho_Factor, sm_d);
    GV(MinSolCp,   sm_d) ;
    GV(K1_BoundSodaCalc, sm_d);
    GV(Growth_TOC, sm_d);
    GV(Growth_s,   sm_d);
    GV(Growth_fc,  sm_d);
    GV(Growth_eq,  sm_d);
    GV(Growth_n,   sm_d);
    GV(Growth_sol, sm_d);
    GV(Growth_ssa, sm_d);

    GVAL2(DefineLiquor, fDoCalc); 
    GVAL(Rqd_AtoC);
    GVAL(Rqd_CtoS);
    GVAL(Rqd_C);
    GVAL(Rqd_Ox);
    GVAL(Rqd_Org);
    GVAL(Rqd_Sul);
    GVAL(Rqd_Salt);
    GVAL(Rqd_Sil);
    GVAL(Rqd_SolFrac);

    GVALF(THAMassFlow);
    GVALFT(THADens);
    GVALFT(CausticConc);
    GVALFT(AluminaConc);
    GVALFT(SodaConc);
    GVALFT(SodiumCarbonateConc);

    GVAL2(CausticConc25, LiqConcs25.Liq[::CausticSoda]);
    GVAL2(AluminaConc25, LiqConcs25.Liq[::Alumina]);
    GVAL2(SodaConc25, LiqConcs25.Liq[::CausticSoda]+ LiqConcs25.Liq[::SodiumCarbonate]);
    GVAL2(CarbonateConc25, LiqConcs25.Liq[::SodiumCarbonate]);
    GVAL2(NaClConc25, LiqConcs25.Liq[::SodiumChloride]*::SodiumChloride.MW*2/::SodiumCarbonate.MW);
    GVAL2(NaSO4Conc25, LiqConcs25.Liq[::SodiumSulphate]*::SodiumSulphate.MW/::SodiumCarbonate.MW);
    GVAL2(Na2C2O4Conc25,LiqConcs25.Liq[::SodiumOxalate]*::SodiumOxalate.MW/::SodiumCarbonate.MW);
    GVALFT(SolidsConc);
    GVALF(SolidsConc25);
    GVAL2(FreeCaustic25,FreeCaustic(C_2_K(25.0)));
    GVALFT(TOC);
    GVALF(TOC25);
    GVALF(TOS25);


    GVALF(AtoC);
    GVALF(CtoS);
    GVALF(CltoC);
    GVALF(TOStoTOC);

    case idBoilPtElev	: Value=BoilPtElev(MArray(this), P);        return; 
      GVALF(LVolume25);
      GVALF(SLVolume25);
      GVALF(OrganateConc25);
      GVALF(OxalateConc25);
      GVALF(TotalOrganics25);
      GVALF(ChlorineConc25);
      GVALF(SulphateConc25);
      GVALF(TotalNa25);
      GVALFT(AluminaConcSat);
      GVALF(IonicStrength);
      GVALFT(DigAtoCEquil);
      GVALFT(PFAtoCEquil);
      GVALFT(PFSSN);

      GVALFT(BoundSodaSat);
      GVALFT(AtoCSaturation);
      GVALFT(SSNRatio);
      GVALFT(AluminaSSN);
      GVALF(LDensity25);
      GVALF(SLDensity25);
      GVALF(NaOnCS);

    default: Value=0.0; return;
    }
  }

//---------------------------------------------------------------------------
// Stringifying operators  Put Value....
#define PV(x, type) case id##x : type##x = Value; return 
#define PVAL(x)   case id##x : d##x = Value; return


void Bayer::PutPropertyValue(long Index, MPropertyValue & Value)
  {//define method of setting values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    case idASatMethod		  : sm_iASatMethod=(byte)(long)Value;     return;
    case idTSEMethod      : sm_iTSEMethod=(byte)(long)Value;      return; 
      PV(MinSolCp,sm_d);
    case idDefineLiquor		: fDoCalc=Value;     return;
      PVAL(Rqd_AtoC);
      PVAL(Rqd_CtoS);
      PVAL(Rqd_C); 
      PVAL(Rqd_Ox);
      PVAL(Rqd_Org);
      PVAL(Rqd_Sul);
      PVAL(Rqd_Salt);
      PVAL(Rqd_Sil);
      PVAL(Rqd_SolFrac);
    }
  }


// For a liquor composed *entirely* of standard components, determine composition from concentrations
// Since the density correlation is given in terms of these known concentrations, we immediately know the
// density and then can find the mass fractions. If the total solids (mass fraction) are also specified, 
// find the overall composition since the actual liquor makeup is unchanged.

void Bayer::LiquorComposition(bool DoIt) // Determine mass fractions in liquor from _liquid_ concentrations
  {

  if (!DoIt) 
    {
    Log.ClearCondition(4);
    return;
    }
  //Change inputs if out of range
  dRqd_AtoC   = Range(0.0, dRqd_AtoC,    0.9);
  dRqd_C      = Range(0.0, dRqd_C,       900.0);
  dRqd_CtoS   = Range(0.1, dRqd_CtoS,    1.0);
  dRqd_Sul    = Range(0.0, dRqd_Sul,     500.0);
  dRqd_Sil    = Range(0.0, dRqd_Sil,     500.0);
  dRqd_Salt   = Range(0.0, dRqd_Salt,    500.0);
  dRqd_Ox     = Range(0.0, dRqd_Ox,      500.0);
  dRqd_Org    = Range(0.0, dRqd_Org,     500.0);
  dRqd_SolFrac = Range(0.0, dRqd_SolFrac, .99);


  const double C = dRqd_C;
  const double S = dRqd_C/dRqd_CtoS;
  const double A = dRqd_C*dRqd_AtoC;
  const double Ca = S-C;
  const double Cl = dRqd_Salt; 
  const double SO4 = dRqd_Sul;
  const double Ox = dRqd_Ox;
  const double Org = dRqd_Org;


  const double density25 =CBayerConcs::LiquorDensEqn1(25.0, A, C, Ca , Cl, SO4, Ox, Org);
  const double dco3 = ::SodiumCarbonate.MW * density25;



  double _Alumina    = dRqd_AtoC * dRqd_C * (2.0 * ::Alumina.MW)/::AluminaSolid.MW/  density25;

  double _Caustic    = dRqd_C * 2.0* ::CausticSoda.MW/::SodiumCarbonate.MW/density25   - _Alumina*::CausticSoda.MW/::Alumina.MW; 
  double _Carbonate  = (dRqd_C/dRqd_CtoS - dRqd_C) / density25;
  double _Salt       = dRqd_Salt * 2.0*::SodiumChloride.MW/dco3; // NaCl
  double _Sulphate   = dRqd_Sul * ::SodiumSulphate.MW/ dco3;
  double _Silicate   = dRqd_Sil * ::SodiumSilicate.MW/dco3;
  double _Oxalate    = dRqd_Ox * ::SodiumOxalate.MW / dco3;
  double _Organics   = dRqd_Org * ::Organics.MW / dco3;
  double tot = _Caustic + _Alumina + _Carbonate + _Salt + _Sulphate + _Silicate + _Oxalate + _Organics;

  if (tot > 1.0) 
    {
    Log.Message(MMsg_Warning, "Bayer feed calculator: Bad liquor description - reduce concentrations!");
    M[Water]=1.0;
    M[CausticSoda]=0.0;
    M[Alumina]=0.0;
    M[SodiumCarbonate]=0.0;
    M[SodiumChloride]=0.0;
    M[SodiumSulphate]=0.0;
    M[SodiumSilicate]=0.0;
    M[SodiumOxalate]=0.0;
    M[Organics]=0.0;
    } 
  else 
    {
    double ff = 1.-dRqd_SolFrac;  //  ff fluid fraction
    M[Water] = (1.0-tot)*ff;
    M[CausticSoda]=_Caustic*ff ;
    M[Alumina]=_Alumina*ff;
    M[SodiumCarbonate]=_Carbonate*ff;
    M[SodiumChloride]=_Salt*ff;
    M[SodiumSulphate]=_Sulphate*ff;
    M[SodiumSilicate]=_Silicate*ff;
    M[SodiumOxalate]=_Oxalate*ff;
    M[Organics]=_Organics*ff;

    M[THA] = dRqd_SolFrac;
    }
  }


// Digester A/C Equilibrium


double Bayer::DigAtoCEquil(double T_)
  {

  double Tc = K2C(T_);

  CheckConverged();

  double C = LiqConcs25.Liq[::CausticSoda];
  if (C<1.0e-9) 
    return 0.0;
  double V = (LiqConcs25.Liq[::SodiumCarbonate] 
+ LiqConcs25.Liq[::SodiumSulphate] 
+ LiqConcs25.Liq[::SodiumOxalate] 
+ LiqConcs25.Liq[::SodiumChloride] 
+ LiqConcs25.Liq[::Organics])/C;

return (.11992+ .00266125*Tc)*(1+.0869*V)-(10.22+.02975*Tc)/C;

  }

double Bayer::PFAtoCEquil(double T)
  {
  double Tc = K2C(T);
  const double PFAtoC = AtoC();
  const double PFCtoS = CtoS();
  const double C = LiqConcs25.Liq[::CausticSoda];
  const double PFTOOCtoC = TOOCtoC();
  const double PFCltoC = CltoC();

  const double PF_Equ = -.2581+ 0.006797 *Tc - .133368*PFCtoS +
    C*(.000997 + .000154*PFTOOCtoC + 0.000991*PFCltoC);

  return PF_Equ;
  }


double Bayer::PFSSN(double T) {
  const double pfequ = PFAtoCEquil(T);
  const double pfatoc = AtoC();
  return (pfatoc-pfequ)*(1-1.0395*pfatoc);


  }



// 
double Bayer::DRatio(double T_) 
  {
  return LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25;
  }

//---------------------------------------------------------------------------

double Bayer::CausticConc(double T_, MArray *pMA)
  {
  CheckConverged(pMA);

  return Max(0.0, LiqConcs25.Liq[::CausticSoda] * DRatio(T_));
  }

//---------------------------------------------------------------------------

double Bayer::CausticConc(double T_)
  {
  return CausticConc(T_, &MArray(this));
  }

//---------------------------------------------------------------------------

double Bayer::AluminaConc(double T_)
  {
  CheckConverged();

  return Max(0.0, LiqConcs25.Liq[::Alumina]*DRatio(T_));
  }

//---------------------------------------------------------------------------

double Bayer::SodaConc(double T_)
  {
  CheckConverged();

  double liq25S = LiqConcs25.Liq[::CausticSoda]
+ LiqConcs25.Liq[::SodiumCarbonate];

return Max(0.0, liq25S * DRatio(T_));
  }

//---------------------------------------------------------------------------

double Bayer::SodiumCarbonateConc(double T_)
  {
  CheckConverged();

  return Max(0.0, LiqConcs25.Liq[SodiumCarbonate]*DRatio(T_));
  }

//---------------------------------------------------------------------------

double Bayer::SodiumSulphateConc(double T_)
  {
  CheckConverged();

  return Max(0.0, LiqConcs25.Liq[SodiumSulphate]*DRatio(T_));
  }

//---------------------------------------------------------------------------

double Bayer::SodiumOxalateConc(double T_)
  {
  CheckConverged();

  return Max(0.0, LiqConcs25.Liq[SodiumOxalate]*DRatio(T_));
  }

//---------------------------------------------------------------------------

double Bayer::SolidsConc(double T_)
  {
  const double mt=Mass(MP_Sol);
  return (mt>=UsableMass) ? mt/GTZ(Mass(MP_SL))*get_Density(MP_SL, T_, Pressure, NULL) : 0.0;
  }
double Bayer::SolidsConc25()
  {
  const double mt=Mass(MP_Sol);
  return (mt>=UsableMass) ? mt/GTZ(Mass(MP_SL))*get_Density(MP_SL, C_2_K(25.0), Pressure, NULL) : 0.0;
  }



double Bayer::TOC(double T_)
  {
  CheckConverged();

  double Toc = (LiqConcs25.Liq[::SodiumOxalate]*2.0 + LiqConcs25.Liq[::Organics]*5.2)*MW_C/::SodiumCarbonate.MW;
  return Max(0.0, Toc*DRatio(T_));
  }

double Bayer::TOC25()
  {
  CheckConverged();

  return (LiqConcs25.Liq[::SodiumOxalate]*2.0)*MW_C/::SodiumCarbonate.MW
    + LiqConcs25.Liq[::Organics]*5.2*MW_C/::SodiumCarbonate.MW;
  }
//---------------------------------------------------------------------------

double Bayer::TOS(double T_)
  {
  CheckConverged();

  const double InorganicSoda = LiqConcs25.Liq[::SodiumSulphate] + LiqConcs25.Liq[::SodiumChloride] 
+  LiqConcs25.Liq[::SodiumCarbonate] + LiqConcs25.Liq[::CausticSoda];
const double TSodium = InorganicSoda + LiqConcs25.Liq[::SodiumOxalate] + LiqConcs25.Liq[::Organics];
return (TSodium - InorganicSoda)*DRatio(T_);
  }

double Bayer::TOS25()
  {
  CheckConverged();

  const double InorganicSoda = 
    LiqConcs25.Liq[::SodiumSulphate] + 
    LiqConcs25.Liq[::SodiumChloride] + 
    LiqConcs25.Liq[::SodiumCarbonate] + 
    LiqConcs25.Liq[::CausticSoda] + 
    LiqConcs25.Liq[::SodiumSilicate];
  const double TSodium = InorganicSoda + LiqConcs25.Liq[::SodiumOxalate] + LiqConcs25.Liq[::Organics];
  return TSodium - InorganicSoda;
  }

//---------------------------------------------------------------------------

double Bayer::TOStoTOC()
  {
  const double TOC = TOC25();
  if (TOC>1.0e-6) {
    const double TOS = TOS25();
    return TOS/TOC;
    }
return 0.0;
  }

//---------------------------------------------------------------------------

double Bayer::FreeCaustic(double T_, MArray *pMA)
  {
  return CausticConc(T_, pMA) * (1.0 - AtoC(pMA)*::SodiumCarbonate.MW/::AluminaSolid.MW);
  }

//---------------------------------------------------------------------------

double Bayer::FreeCaustic(double T_)
  {
  return FreeCaustic(T_, &MArray(this));
  }

//---------------------------------------------------------------------------

double Bayer::AtoC(MArray *pMA)
  {
  CheckConverged(pMA);

  return LiqConcs25.Liq[::Alumina] / GTZ(LiqConcs25.Liq[::CausticSoda]);
  }

//---------------------------------------------------------------------------

double Bayer::AtoC()
  {
  return AtoC(&MArray(this));
  }

//---------------------------------------------------------------------------

double Bayer::CtoS()
  {
  CheckConverged();

  return LiqConcs25.Liq[::CausticSoda]
/ GTZ(LiqConcs25.Liq[::CausticSoda] + LiqConcs25.Liq[::SodiumCarbonate]);
  }

//---------------------------------------------------------------------------

double Bayer::CltoC()
  {
  CheckConverged();

  return (LiqConcs25.Liq[SodiumChloride])/GTZ(LiqConcs25.Liq[::CausticSoda]);
  }

//---------------------------------------------------------------------------

double Bayer::Na2SO4toC()
  {
  CheckConverged();

  return (LiqConcs25.Liq[SodiumSulphate])/GTZ(LiqConcs25.Liq[::CausticSoda]);
  }

//---------------------------------------------------------------------------

double Bayer::THAMassFlow()
  {
  MArray M(this);
  return M[::THA];
  }

//---------------------------------------------------------------------------

double Bayer::THADens(double T_)
  {
  return gs_MVDefn[::THA].Density(T_, StdP, NULL);
  }

//---------------------------------------------------------------------------

double Bayer::IonicStrength()
  {
  const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989..
  const double MW_NaCl     = ::SodiumChloride.MW;   //58.4425..
  const double MW_Na2SO4   = ::SodiumSulphate.MW;   //142.043..
  CheckConverged();

  double I;
  const double C = LiqConcs25.Liq[::CausticSoda];
  switch (sm_iASatMethod)
    {
    case ASM_Rosenberg:
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
      I = 0.01887*C + k1*cNaCl/MW_NaCl + k2*cNa2CO3/MW_Na2CO3 + k3*cNa2SO4/MW_Na2SO4 + k4*0.01887*cTOC;
      return I;
      }
    case ASM_MAD99:
    case ASM_MAD05:

      {
      //double c1 = 0.0;
      const double k1 = 1.5532;//0.9346;
      const double k2 = 1.3437;//2.0526;
      const double k3 = 2.2571;//2.1714;
      const double k4 = 1.1997;//1.6734;
      const double Cl2C    = CltoC();
      const double C2S     = CtoS();
      const double SO4_2C  = SulphateConc25()/GTZ(LiqConcs25.Liq[::CausticSoda]);
      const double TOOC2C  = TOOCtoC();
      //double s = C2S/c1;
      I   = C * (0.01887 + (k1*Cl2C*2.0 + k2*(1.0-C2S)/GTZ(C2S) + k3*SO4_2C + k4*0.01887*TOOC2C*MW_C)/::SodiumCarbonate.MW);
      return I;
      }
    }
  return 0.0;
  }

//---------------------------------------------------------------------------
// The original SySCAD implementation had a major bug in that the switch/case constructs had no
// break statements, hence the code always fell though all the cases to the final one, ie Alpart02

double Bayer::AluminaConcSat(double T_)
  {
  T_ = Max(1.0, T_);
  const double I = IonicStrength();
  const double I2 = Sqrt(I);
  const double C   = LiqConcs25.Liq[::CausticSoda];
  switch (sm_iASatMethod)
    {
    case ASM_Rosenberg:
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
      const double ASat = 0.96197*C/(1.0+(Pow(10.0, X)/Keq));
      return DUMPIT1("AluminaConcSat", ASat, T_);
      }
    case ASM_MAD99:
      { // This is ACTUALLY the ALPART correlation, included so as to allow compatibility with the
      // old method (see bug above...)
      const double a0 = -9.11181236102574;
      const double a3 = -0.902799187095254;
      const double a4 = 0.239197078637227;
      const double dG = -30960.0;
      const double X   = a0*I2 / (1.0+I2) - a3*I - a4*Pow(GTZ(I),1.5);
      const double Keq = Exps(dG/(R_c*T_));
      return  DUMPIT1("AluminaConcSat", 0.96197*C / (1.0 + (Pow(10.0, X) / Keq)), T_);
      }
    case ASM_MAD05:
      {
      const double a0 = -8.7168; //-9.2082;
      const double a3 = -0.7767; //-0.8743;
      const double a4 =  0.2028;
      const double dG = -30960.0;
      const double E   = a0*I2 / (1.0+I2) - I*(a3 + a4*I2);
      const double Keq = Exps(dG/(R_c*T_));
      return DUMPIT1("AluminaConcSat", 0.96197*C / (1.0 + (Pow(10.0, E) / Keq)), T_);
      }


    }
  return DUMPIT1("AluminaConcSat", 0.0, T_);
  }

//---------------------------------------------------------------------------


double Bayer::LVolume25()
  {
  const double mt=Mass(MP_Liq);
  return ((mt>=UsableMass) ? (mt / get_Density(MP_Liq, C_2_K(25.0), Pressure, NULL)) : 0.0);
  }

double Bayer::SLVolume25()
  {
  const double mt=Mass(MP_SL);
  return ((mt>=UsableMass) ? (mt / get_Density(MP_SL, C_2_K(25.0), Pressure, NULL)) : 0.0);
  }

//---------------------------------------------------------------------------

double Bayer::LDensity25()
  {
  return (get_Density(MP_Liq, C_2_K(25.0), Pressure, NULL));
  }

double Bayer::SLDensity25()
  {
  return (get_Density(MP_SL, C_2_K(25.0), Pressure, NULL));
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double Bayer::AtoCSaturation(double T_)
  {
  double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AluminaConcSat(T_) / GTZ(C);
  return ACsat;
  }

//---------------------------------------------------------------------------


double Bayer::BoundSodaSat(double T_)
  {
  CheckConverged();

  const double C = LiqConcs25.Liq[::CausticSoda];
  const double C_atT  = C*DRatio(T_);
  const double Aeq    = AluminaConcSat(T_);
  const double ACeq   = Aeq/GTZ(C_atT);
  const double AC     = AtoC();
  const double TOOC2C = TOOCtoC();
  const double C2S    = CtoS();
  const double X      = sm_dK1_BoundSodaCalc * (0.000598*C - 0.00036*K_2_C(T_) + 0.019568*TOOC2C) * (1.0 - 0.758*C2S);
  const double BoundSoda = X * Sqr(AC-ACeq);
  return DUMPIT0("BoundSoda", BoundSoda);

  }

double Bayer::SSNRatio(double T_)
  {   // AtoC actual / AtoC saturation ==> A/ASat
  const double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AtoCSaturation(T_);
  return AtoC() / GTZ(ACsat);
  }

//---------------------------------------------------------------------------




double Bayer::TotalNa25()
  {//TotalNa Conc @ 25 expressed as Na2CO3
  CheckConverged();

  double Sodium = LiqConcs25.Liq[::CausticSoda]       // NaOH+NaAl[OH]4
+ LiqConcs25.Liq[::SodiumCarbonate] // Na2CO3
+ LiqConcs25.Liq[SodiumOxalate]     // Na2C2O4
+ LiqConcs25.Liq[SodiumSilicate]    // Na2SiO3
+ LiqConcs25.Liq[Organics]         //  Na2C5.2O7.2
+ LiqConcs25.Liq[SodiumChloride]    // NaCl
+ LiqConcs25.Liq[SodiumSulphate];    // Na2SO4

return Sodium;
  }

//---------------------------------------------------------------------------

double Bayer::NaOnCS() 
  { 
  return TotalNa25()/GTZ(LiqConcs25.Liq[::CausticSoda]);
  }


double Bayer::Na2CO3toS()
  {
  CheckConverged();

  return LiqConcs25.Liq[::SodiumCarbonate]
/ GTZ(LiqConcs25.Liq[::CausticSoda] + LiqConcs25.Liq[::SodiumCarbonate]);
  }

//---------------------------------------------------------------------------

double Bayer::OrganateConc25()
  {//Organic Na2C5O7 + NaOrg Conc @ 25
  CheckConverged();

  double Organate = LiqConcs25.Liq[::Organics];
  return Organate;
  }

//---------------------------------------------------------------------------

double Bayer::OxalateConc25()
  {
  CheckConverged();

  double Oxalate = LiqConcs25.Liq[::SodiumOxalate];
  return Oxalate;
  }

//---------------------------------------------------------------------------

double Bayer::TotalOrganics25()
  {
  CheckConverged();

  double Organics = LiqConcs25.Liq[::Organics] + LiqConcs25.Liq[::SodiumOxalate];
  return Organics;
  }
//---------------------------------------------------------------------------

double Bayer::ChlorineConc25()
  {
  CheckConverged();

  double Chlorine = LiqConcs25.Liq[::SodiumChloride];
  return Chlorine;
  }

//---------------------------------------------------------------------------

double Bayer::SulphateConc25()
  {
  CheckConverged();

  double Sulphate = LiqConcs25.Liq[::SodiumSulphate];
  return Sulphate;
  }

//---------------------------------------------------------------------------



double Bayer::TOOCtoC()
  {
  CheckConverged();

  return (LiqConcs25.Liq[::SodiumOxalate]*2.0 + LiqConcs25.Liq[::Organics]*5.2)/GTZ(LiqConcs25.Liq[::CausticSoda]);
  }



double Bayer::AluminaSSN(double T_)
  {
  /* the parameter (supersaturated alumina) / (free caustic) was found at MAD
  to be a very good measure of clarification and press floor stability - it
  was the best measure of the propensity for homogeneous nucleation of a
  pregnant liquor. The measure is gpl of alumina (as alumina) above the
  saturation concentration divided by free caustic - i.e. if there are 112 gpl
  of Al2O3 and A-sat is 75 gpl then the supersaturated alumina is (112 - 75) =
  37 gpl. If the free caustic was 70 gpl (as Na2CO3) then the supersaturation
  number is (37/70) = 0.53. Maybe as "Alumina_SSN"
  Al_FC_SSN = C * (AC - AC_eq) / FC  */

  CheckConverged();

  const double C = LiqConcs25.Liq[::CausticSoda];
  if (C < 1.0e-12)
    return   DUMPIT1("AluminaSSN", 0.0, T_);
  const double ACsat = AtoCSaturation(T_);
  const double FC = FreeCaustic(C_2_K(25.0));
  const double Al_FC_SSN = C * (AtoC() - ACsat)/GTZ(FC);
  return DUMPIT1("AluminaSSN", Al_FC_SSN, T_);
  }


void Bayer::CheckConverged(MArray *pMA)
  {
  //static int xxx=0;
  //xxx++;
  //if (1 || Dbg.Marked())
  //  {
  //  Dbg.Print("CC: %4i %-20s", xxx, Tag);
  //  Dbg.PrintLn("");
  //  }

  MArray MA(pMA ? (*pMA) : this);
  if (pMA)
    LiqConcs25.Converge(MA, false);
  else if ( !TestStateValid(0) || FORCECNVG )
    LiqConcs25.Converge(MA, true);
  }

#if DBG_MVECTOR

double  Bayer::DumpIt(LPCTSTR FnTag, double V, double x1, double x2, double x3)
  {
  //static xxx=0;
  //xxx++;
  //if (1 || Dbg.Marked())
  //  {
  //  if (Valid(x3))
  //    Dbg.PrintLn("%5i] %-20s %-30s %25.12g  (%25.12g %25.12g %25.12g)", xxx, FnTag, Tag, V, x1,x2,x3);
  //  else if (Valid(x2))
  //    Dbg.PrintLn("%5i] %-20s %-30s %25.12g  (%25.12g %25.12g)", xxx, FnTag, Tag, V, x1,x2);
  //  else if (Valid(x1))
  //    Dbg.PrintLn("%5i] %-20s %-30s %25.12g  (%25.12g)", xxx, FnTag, Tag, V, x1);
  //  else
  //    Dbg.PrintLn("%5i] %-20s %-30s %25.12g  ()", xxx, FnTag, Tag, V);
  //  }

  return V;
  };

bool Bayer::TestStateValid(int iNo)
  {
  //static int xxx=0;
  //xxx++;
  //if (1 || Dbg.Marked())
  //  {
  //  m_dbgM.SetSize(gs_MVDefn.Count());
  //  Dbg.Print("%3s %4i %-20s ", MSpModelBase::TestStateValid(iNo)?"OK":"", xxx, Tag);
  //  Dbg.Print(" Rho25) %.14g", LiqConcs25.Density25);

  //  if (fabs(Vector.msHfRaw()-m_dbgtotHf)>1e-12)
  //    Dbg.Print(" Hf) %.14g>%.14g", m_dbgtotHf, Vector.msHfRaw());
  //  m_dbgtotHf=Vector.msHfRaw();
  //  for (int i=0; i<gs_MVDefn.Count(); i++) 
  //    {
  //    if (fabs(M[i]-m_dbgM[i])>1e-12)
  //      Dbg.Print(" %i) %.14g>%.14g", i, m_dbgM[i], M[i]);
  //    m_dbgM[i]=M[i];
  //    }
  //  Dbg.PrintLn("");
  //  }

  return MSpModelBase::TestStateValid(iNo);
  };

#endif