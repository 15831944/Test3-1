//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "BATCBayerSM.h"
#include "math.h"
#include "md_headers.h"
#pragma comment(lib, "rpcrt4.lib")
//#pragma optimize("", off)

#define IncludeBrahma 1

//===========================================================================
//
//
//
//===========================================================================

DEFINE_SPECIEMODEL(BATCBayerSM)
IMPLEMENT_SPECIEMODEL(BATCBayerSM, "BATCBayerSM", "*BATC Bayer", "Bauxite and Alumina Technology Centre Bayer Specie Model", DLL_GroupName)

//===========================================================================
// Specie Access
static MInitialiseTest InitTest("BATCBayerSMClass");

MSpeciePtr   Water             (InitTest, "H2O(l)",        false);
MSpeciePtr   Alumina           (InitTest, "Al2O3(l)",      false);
MSpeciePtr   CausticSoda       (InitTest, "NaOH(l)",       false);
MSpeciePtr   SodiumChloride    (InitTest, "NaCl(l)",       false);
MSpeciePtr   SodiumSulphate    (InitTest, "Na2SO4(l)",     false);
MSpeciePtr   SodiumCarbonate   (InitTest, "Na2CO3(l)",     false);
MSpeciePtr   SodiumFluoride    (InitTest, "NaF(l)",        false);
MSpeciePtr   SodiumOxalate     (InitTest, "Na2C2O4(l)",    false); //organic
//MSpeciePtr   Organics          (InitTest, "Na2C5O7(l)",    false); //organic
MSpeciePtr   Organics1         (InitTest, "NaOrg(l)",      false); //organic NaC2.21O2.275H3.63
MSpeciePtr   SilicateLiq       (InitTest, "SiO2(l)",       false);
//MSpeciePtr   Potassium         (InitTest, "K2O(l)",        false); 

MSpeciePtr   OccSoda           (InitTest, "Na2O(s)",       false);
MSpeciePtr   THA               (InitTest, "Al2O3.3H2O(s)", false);
MSpeciePtr   AluminaSolid      (InitTest, "Al2O3(s)",      false);

MSpeciePtr   Steam             (InitTest, "H2O(g)",        true);

// ==========================================================================
// Specie Properties Modal Global Parameters

const double MW_C        = 12.011;
const double MW_Cl       = 35.4527;
const double MW_F        = 18.9984032;
const double MW_SO4      = 96.0636; //32.066+(4*15.9994)

//enum DLM_DefineLiquorMethod { DLM_TotOrganics, DLM_TOC };
enum BDM_BayerDensityMethod { BDM_Original, BDM_WRS, BDM_BPD1, BDM_BPD2, BDM_BRAHMA, BDM_BRAHMA_OLD, BDM_BRAHMA25 };
enum CPM_HeatCapacityMethod { CPM_Original, CPM_WRS };
enum ASM_AlSolubilityMethod { ASM_LiqInfo5, ASM_LiqInfo6, ASM_BRAHMA };
enum BPM_BPEMethod { BPM_Dewey, BPM_Adamson, BPM_BPD1, BPM_BPD2 };

double BATCBayerSM::sm_dMinSolCp      = 1.046;
double BATCBayerSM::sm_dH2OTestFrac0  = 1.0;//0.98;
double BATCBayerSM::sm_dH2OTestFrac1  = 1.0;//0.95;
//byte BATCBayerSM::sm_iRqdCalcMethod   = DLM_TOC;
byte BATCBayerSM::sm_iDensityMethod   = BDM_WRS;//BDM_Original;
byte BATCBayerSM::sm_iCPMethod        = CPM_WRS;//CPM_Original;
byte BATCBayerSM::sm_iASatMethod      = ASM_LiqInfo6;
byte BATCBayerSM::sm_iBPEMethod       = BPM_Dewey;

// ==========================================================================
//
// A helper class to assist with calculating iterative concentration and density
// calculations.
//
// ==========================================================================

bool   CBayerConcs::NaFactorOK=0;
MArray CBayerConcs::NaFactor;

CBayerConcs::CBayerConcs(BATCBayerSM *pMdl)
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
    //NaFactor[::Organics]        = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::Organics.MW);    
    NaFactor[::Organics1]       = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::Organics1.MW);    

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

  static MToleranceBlock s_Tol(TBF_Both, "BABayerSM:Density", 1e-12, 1e-8, 100);

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
         + MA[SodiumFluoride] * NaFactor[::SodiumFluoride]
         //+ MA[Organics]       * NaFactor[::Organics]
         + MA[Organics1]      * NaFactor[::Organics1] );
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
         + MA[SodiumSulphate] * NaFactor[::SodiumSulphate]
         + MA[SodiumFluoride] * NaFactor[::SodiumFluoride] );
  return TInorganicSodium;
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

double CBayerConcs::LiquorDensEqn2(double T_, double S, double CtoS, double AtoC, double CltoS, double FtoS, double SO4toS, double TOCtoS)
  {
  double K2OtoS = 0.0; //not used
  double c = S * CtoS * 0.001;
  double a = c * AtoC;
  double RHOH2O = WaterDensity(T_);
  double CI = c - S * K2OtoS * 0.001;
  double WNaOH = (321.0 * CI - 333.0 * a) * 2.3521;
  double WNaAl = a * 1607.887;
  double TOStoS = TOCtoS * 2.0;

  double Fact = 1.376; //Fact = 1.376 for low temp 1.26428 for high temp plant combines the MW with gOC/gOrg
  double WTOS = S * TOStoS * Fact;
  double WKOH = S * K2OtoS * 1.191239;
  double WNaCl = S * CltoS * 1.64838;
  double WNaF = S * FtoS * 2.210091;
  double WNa2SO4 = S * SO4toS * 1.478635;
  double WNa2CO3 = Max(0.0, S - 1000.0 * c);
  double WSOL = WNaOH + WNaAl + WTOS + WKOH + WNaCl + WNaF + WNa2SO4 + WNa2CO3;
  double WH2O = Max(0.0, 1000.0 - WSOL);
  double VH2O = WH2O / RHOH2O;
  double VKOH = WKOH * 0.160095;
  double VNa2SO4 = WNa2SO4 * 0.142179;
  double VNa2CO3 = Max(0.0, WNa2CO3 * (0.3484 * Pow((WNa2CO3 / GTZ(WH2O)),0.5) - 0.0591));
  double VNaCl = WNaCl * 0.320909;
  //VOrg = 0.71 for low temp plant ; 0.2 for high temp plant; 0.6 for WAPL
  double VOrg = 0.6;
  double VTOS = WTOS * VOrg;
  double VNaF = WNaF * 0.074;
  double F1 = WNaAl / GTZ(WH2O);
  double F2 = WNaOH / GTZ(WH2O);
  double V = 0.75 * WNaAl * F1 + 0.82926 * WNaOH * F2 - 1.4985 * WNaAl * F1 * F1 - 0.81711 * WNaOH * F2 * F2 + 0.7212 * WNaAl * F1 * F1 * F1 + 0.33201 * WNaOH * F2 * F2 * F2;
  V = V + 50.0 * Pow((1000 - WH2O) / GTZ(WH2O), 2.0);// for low temp
  //V = V+23.9*((1000-WH2O)/GTZ(WH2O));// for high temp plant
  double TVOL = V + VH2O + VKOH + VNa2SO4 + VNa2CO3 + VNaCl + VNaF + VTOS;
  double RHOT = 1000.0 / GTZ(TVOL);
  return (RHOT * 1000.0);
  }

// --------------------------------------------------------------------------

double CBayerConcs::LiquorDensEqn3(double T_, double S, double A, double C)
  {
  const double Rac = A/NZ(C);
  const double Rcs = C/NZ(S);
  const double Rso4 = 73.4;
  const double Rcl = 31.6;
  const double Roc = 76.6;
  const double R1 = (8383.2 - (464.8 - 4886.4*Rac)*Rcs + 11.8*Rso4 + 9.81*Rcl + 12.85*Roc)*1e-4;
  const double R2 = (2.69*Rac*Rac - 3.71)*Rcs*1e-4;
  const double RHO = 1548.15 + R1*S + R2*S*S - 1.10913*T_ - 59651.0/T_;
  return RHO;
  }

// --------------------------------------------------------------------------

double CBayerConcs::LiquorDensEqn4(double T_, double S, double A, double C, double Rso4, double Rcl, double Roc)
  {
  const double Rac = A/NZ(C);
  const double Rcs = C/NZ(S);
  const double R1 = (8383.2 - (464.8 - 4886.4*Rac)*Rcs + 11.8*Rso4 + 9.81*Rcl + 12.85*Roc)*1e-4;
  const double R2 = (2.69*Rac*Rac - 3.71)*Rcs*1e-4;
  const double RHO = 1548.15 + R1*S + R2*S*S - 1.10913*T_ - 59651.0/T_;
  return RHO;
  }

// --------------------------------------------------------------------------

#include "brama_liqden.cpp"

// --------------------------------------------------------------------------

double CBayerConcs::WaterDensity(double T_)
  {
  //const double TR = 1.0 - (K2C(T_)+273.15)/647.3;
  //kga 15/2/2006: Do not allow TR to be negative (ie assume maximum temperature = 647.3K
  const double TR = Max(0.01, 1.0 - (K2C(T_)+273.15)/647.3);
  const double TR2 = TR*TR;
  const double a = 1497.2*Pow(TR,0.975);
  const double b = 1688000.0*Pow(TR,20.0) - 7439.0*Pow(TR,6.0) + 2002.4*TR2 - 1763.3;
  const double c = 2.8436 - 0.025096/TR + 0.00008007/TR2;
  const double RhoW = 0.001 * ( 576.44 + a + TR2*b - 1.0/TR*c);
  return RhoW;
  }

// --------------------------------------------------------------------------

double CBayerConcs::LiquorDensity(double T_, MArray & MA)
  {
  /*Liquor Density with mass fractions*/
  double TLiq=MA.Mass(MP_Liq); //Total Liquid kg/s

  if (TLiq<1.0e-9)
    return 0.0;

  double Tc = K2C(T_);
  switch (BATCBayerSM::sm_iDensityMethod)
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
    case BDM_WRS:
      {
      //Referance: Bayer Process Properties BATC SysCAD Model
/*t = temperature, °C
S = S, g of S per kg liquor
AtoC = A/C
CtoS = C/S
CltoS = kg Cl- ions per kg S
FtoS = kg F- ions per kg S
SO4toS = kg SO4- ions per kg S
TOCtoS = kg TOC per kg S
K2OtoS = kg K- ions per kg S

Function RHOT(t, S, AtoC, CtoS, CltoS, FtoS, SO4toS, TOCtoS, K2OtoS)
' calculate the density of Bayer liquor
' S in g/kg, t in °C
On Error GoTo eh_rhot
C = S * CtoS * 0.001
a = C * AtoC
RHOH2O = RHOW(t) (Note this is a function call for water density – use default SysCAD function in place)
CI = C - S * K2OtoS * 0.001
WNaOH = (321 * CI - 333 * a) * 2.3521
WNaAl = a * 1607.887
TOStoS = TOCtoS * 2
' Fact = 1.376 for low temp 1.26428 for high temp plant combines the MW with gOC/gOrg
Fact = 1.376
WTOS = S * TOStoS * Fact
WKOH = S * K2OtoS * 1.191239
WNaCl = S * CltoS * 1.64838
WNaF = S * FtoS * 2.210091
WNa2SO4 = S * SO4toS * 1.478635
WNa2CO3 = S - 1000 * C
WSOL = WNaOH + WNaAl + WTOS + WKOH + WNaCl + WNaF + WNa2SO4 + WNa2CO3
WH2O = 1000 - WSOL
VH2O = WH2O / RHOH2O
VKOH = WKOH * 0.160095
VNa2SO4 = WNa2SO4 * 0.142179
VNa2CO3 = WNa2CO3 * (0.3484 * (WNa2CO3 / WH2O) ^ 0.5 - 0.0591)
VNaCl = WNaCl * 0.320909
' VOrg = 0.71 for low temp plant ; 0.2 for high temp plant; 0.6 for WAPL
VOrg = 0.6
VTOS = WTOS * VOrg
VNaF = WNaF * 0.074
F1 = WNaAl / WH2O
F2 = WNaOH / WH2O
V = 0.75 * WNaAl * F1 + 0.82926 * WNaOH * F2 - 1.4985 * WNaAl * F1 ^ 2 - 0.81711 * WNaOH * F2 ^ 2 + 0.7212 * WNaAl * F1 ^ 3 + 0.33201 * WNaOH * F2 ^ 3
V = V + 50 * ((1000 - WH2O) / WH2O) ^ 2 ' for low temp
 V=V+23.9*((1000-WH2O)/WH2O) for high temp plant
TVOL = V + VH2O + VKOH + VNa2SO4 + VNa2CO3 + VNaCl + VNaF + VTOS
RHOT = 1000 / TVOL
*/
      const double S = (MA[SodiumCarbonate] + MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW);
      const double A = MA[Alumina];
      const double C = MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW;
      const double AtoC = A/NZ(C);
      const double CtoS = C/NZ(S);
      const double Cl = MA[SodiumChloride]/::SodiumChloride.MW*MW_Cl;
      const double CltoS = Cl/NZ(S); //kg per kg S
      const double F = MA[SodiumFluoride]/::SodiumFluoride.MW*MW_F;
      const double FtoS = F/NZ(S); //kg per kg S
      const double SO4 = MA[SodiumSulphate]/::SodiumSulphate.MW*MW_SO4;
      const double SO4toS = SO4/NZ(S); //kg per kg S
      const double StoLiq = S*1000.0/TLiq;//g of S per kg liquor
      const double TOC = (/*MA[Organics]*5.0/::Organics.MW + */MA[SodiumOxalate]*2.0/::SodiumOxalate.MW)*MW_C + MA[Organics1]*2.21/::Organics1.MW*MW_C;
      const double TOCtoS = TOC/NZ(S); //kg per kg S
      double RHO = LiquorDensEqn2(T_, StoLiq, CtoS, AtoC, CltoS, FtoS, SO4toS, TOCtoS);
      return RHO;
      }
    case BDM_BPD1:
      {
      //Referance: Bayer Process Properties BATC SysCAD Model
      //2.2	Liquor Density – Basis of Process Design.
      //Note the liquor sulphate, chloride and organic carbon concentrations are fixed.  
      //Function variables:
      //A	=	Al2O3 concentration (g/L liquor)
      //C	=	NaOH & NaAl(OH)4 concentration, expressed as grams Na2CO3 / L liquor @ 25°C
      //S	=	NaOH, NaAl(OH)4 plus Na2CO3 expressed as grams Na2CO3 / L liquor @ 25°C
      //RAC 	=	A/C ratio
      //RCS	=	C/S ratio
      //RSO4 	=	73.4  g SO4 / kg S  (SO4 ions not Na2SO4)
      //RCl 	=	31.6  g Cl / kg S (Cl ions not NaCl)
      //ROC 	=	76.6  g organic carbon / kg S 
      //RF 	=	2.7  g F / kg S
      double rho = 1000.0;
      const double S = (MA[SodiumCarbonate] + MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW)/TLiq*rho;
      const double A = MA[Alumina]/TLiq*rho;
      const double C = (MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW)/TLiq*rho;
      double RHO = LiquorDensEqn3(T_, S, A, C);
      return RHO;
      }
    case BDM_BPD2:
      {
      //Referance: Bayer Process Properties BATC SysCAD Model
      //2.2	Liquor Density – Basis of Process Design.
      //Revised as per email from Melissa Rothnie 28 September 2005
      //now uses actual concentrations for SO4, Cl and OC ions
      //Function variables:
      //A	=	Al2O3 concentration (g/L liquor)
      //C	=	NaOH & NaAl(OH)4 concentration, expressed as grams Na2CO3 / L liquor @ 25°C
      //S	=	NaOH, NaAl(OH)4 plus Na2CO3 expressed as grams Na2CO3 / L liquor @ 25°C
      //RAC 	=	A/C ratio
      //RCS	=	C/S ratio
      //RSO4 	=	73.4  g SO4 / kg S  (SO4 ions not Na2SO4)
      //RCl 	=	31.6  g Cl / kg S (Cl ions not NaCl)
      //ROC 	=	76.6  g organic carbon / kg S 
      //RF 	=	2.7  g F / kg S
      double rho = 1000.0;//guess, then iterate...???
      const double S_ = (MA[SodiumCarbonate] + MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW);
      const double S = (MA[SodiumCarbonate] + MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW)/TLiq*rho;
      const double A = MA[Alumina]/TLiq*rho;
      const double C = MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW/TLiq*rho;
      const double SO4 = MA[SodiumSulphate]/::SodiumSulphate.MW*MW_SO4;
      const double Cl = MA[SodiumChloride]/::SodiumChloride.MW*MW_Cl;
      const double TOC = (/*MA[Organics]*5.0/::Organics.MW + */MA[SodiumOxalate]*2.0/::SodiumOxalate.MW)*MW_C + MA[Organics1]*2.21/::Organics1.MW*MW_C;
      const double Rso4 = SO4/NZ(S_);
      const double Rcl = Cl/NZ(S_);
      const double Roc = TOC/NZ(S_);
      double RHO = LiquorDensEqn4(T_, S, A, C, Rso4, Rcl, Roc);
      return RHO;
      }
    case BDM_BRAHMA25: 
      {
      double C = Liq[CausticSoda];
      double S = C+Liq[SodiumCarbonate];
      double A = Liq[Alumina];
      double Cl = Liq[SodiumChloride]*35.453*2/SodiumCarbonate.MW;
      double SO4 = Liq[SodiumSulphate]*96.07/SodiumCarbonate.MW;
      double TOC =  Liq[Organics1]*(12.11*2.21/(89.5951)) + Liq[SodiumOxalate]*(2*12.11/134.012);
      return LiquorDensEqn6( Tc, C,  S,  A, Cl, SO4, TOC);
      }
    case BDM_BRAHMA:
      {
      double den = 1000.;
      double denOld;
      do {
	      denOld = den;
	      double r = den/TLiq;
	      double C = MA[CausticSoda]*r*NaFactor[CausticSoda] ;
	      double S = C+MA[SodiumCarbonate]*r;
	      double A = MA[Alumina]*r;

	      double Cl = MA[SodiumChloride]*r /(58.44/35.45);
	      double SO4 = MA[SodiumSulphate]*r /(142.0431/96.07);
	      double TOC =  (MA[Organics1]*(12.11*2.21/(89.5951)) + MA[SodiumOxalate]*(2*12.11/134.012))*r;
	      den =  LiquorDensEqn6( Tc, C,  S,  A, Cl, SO4, TOC);
      } while (fabs(1.-den/denOld) > 1.0e-5);
      return den;
      }
    case BDM_BRAHMA_OLD:
      {
      double den = 1000.;
      double denOld;
      do {
	      denOld = den;
	      double r = den/TLiq;
	      double C = MA[CausticSoda]*r / (2*39.99/105.99);
	      double S = C+MA[SodiumCarbonate]*r;
	      double A = MA[Alumina]*r;

	      double Cl = MA[SodiumChloride]*r /(58.44/35.45);
	      double SO4 = MA[SodiumSulphate]*r /(142.07/96.07);
	      double TOS =  (MA[Organics1]*(105.99/(2*89.569)) + MA[SodiumOxalate]*105.99/134.012)*r;
	      den =  LiquorDensEqn5( Tc, C,  S,  A, Cl, SO4, TOS);
	      // pBayerMdl->p("den = %12.5g", den);
      } while (fabs(1.-den/denOld) > 1.0e-5);
      return den;
    }
    }//end switch
  return 1000.0;
  }

//===========================================================================
//
//
//
//===========================================================================

BATCBayerSM::BATCBayerSM(TaggedObject *pNd) : LiqConcs25(this)
  {
  MInitialise(); // this must be called to initialise the SMDK library if neccessary

  fDoCalc  = false;
  //feed calculator defaults...
  dRqd_AtoC = 0.4;
  dRqd_C    = 220.0;
  dRqd_CtoS = 0.85;
  dRqd_Sul  = 20.0;
  dRqd_Sil  = 1.0;
  //dRqd_Org  = 50.0;
  //dRqd_OrgRatio = 0.9;
  dRqd_Ox   = 45.0;
  dRqd_TOC  = 12.0;
  dRqd_Salt = 15.0;
  dRqd_Fl   = 5.0;
  dRqd_SolFrac  = 0.0;
  dRqd_SolConc  = dNAN;
  }

//---------------------------------------------------------------------------

BATCBayerSM::~BATCBayerSM()
  {
  }

//---------------------------------------------------------------------------

bool BATCBayerSM::get_IsBaseClassOf(LPCTSTR OtherProgID)
	{
  return false;
  //return S_OK;
	}

//---------------------------------------------------------------------------

LPCTSTR BATCBayerSM::get_PreferredModelProgID()
	{
  return NULL;
  }

//---------------------------------------------------------------------------

bool BATCBayerSM::ValidateData()
  {
  bool OK=MSpModelBase::ValidateData();
  InputCalcs(fDoCalc, Temperature);
  return OK;
  }

//---------------------------------------------------------------------------

double BATCBayerSM::get_Density(long Phases, double T, double P, MArray *pMA)
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
    LiqConcs25.Converge(MA);
    Dl = LiqConcs25.LiquorDensity(T, MA);
    if (WaterFrac>sm_dH2OTestFrac1)
      {
      const double Std_Dl = MSpModelBase::get_Density(MP_Liq, T, P, &MA);
      const double Std_Prop = Min(1.0, (WaterFrac-sm_dH2OTestFrac1)/(sm_dH2OTestFrac0-sm_dH2OTestFrac1));
      Dl = (Dl*(1.0-Std_Prop)) + (Std_Dl*Std_Prop);
      }
    }

  return DensityMix(FSol, dNAN, FLiq, Dl, (1.0-FSol-FLiq), dNAN, T, P, MA);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::LiqCpCalc(MArray & MA, double Tc)
  {
  double Cpl = 0.0;
  LiqConcs25.Converge(MA);
  const double MLiq = MA.Mass(MP_Liq); //Total Liquid kg/s

  switch (sm_iCPMethod)
    {
    case CPM_Original:
      {
      // Heat Capacity of Bayer Liquor Kcal/kg.degC
      // A function of the weight fraction of TNa and Al2O3
      // From published Russian data

      const double TNaAsCO3    = LiqConcs25.LTotalSodium(MA);
      const double TNaAsCO3Use = Max(TNaAsCO3, MLiq*0.19); // Only Valid for  TNaAsCO3/FLiq > 0.19
      const double TNa         = 100.0*TNaAsCO3*::OccSoda.MW/::SodiumCarbonate.MW/MLiq; //61.98=MW Na2O, 105.99=MW Na2CO3
      const double TAl2O3      = 100.0*MA[Alumina]/MLiq;
      Cpl = -0.020113606661083*TNa
            +0.001081165172606*TNa*TNa
            -0.000022606160779*TNa*TNa*TNa
            -0.004597725999883*TAl2O3
            -0.000001053264708*TAl2O3*TAl2O3
            -0.00000218836287*TAl2O3*TAl2O3*TAl2O3;
      // Scale for more dilute Liquors
      Cpl = 1.0275057375729+(TNaAsCO3/TNaAsCO3Use*Cpl);

      Cpl = KCal_2_kJ(Cpl); // kJ/kg.degC   (KCal_2_kJ=4.184)
      break;
      }
    case CPM_WRS:
      {
      // Heat Capacity of Bayer Liquor Kcal/kg.degC
      //Referance: Bayer Process Properties BATC SysCAD Model
      const double S = (MA[SodiumCarbonate] + MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW);
      const double A = MA[Alumina];
      const double C = MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW;
      const double AtoC = A/NZ(C);
      const double CtoS = C/NZ(S);
      const double Cl = MA[SodiumChloride]/::SodiumChloride.MW*MW_Cl;
      const double CltoS = Cl/NZ(S)*1000.0; //g per kg S
      //const double F = MA[SodiumFluoride]/::SodiumFluoride.MW*MW_F;
      //const double FtoS = F/NZ(S)*1000.0; //g per kg S
      const double SO4 = MA[SodiumSulphate]/::SodiumSulphate.MW*MW_SO4;
      const double SO4toS = SO4/NZ(S)*1000.0; //g per kg S
      //const double TOC = (MA[Organics]*5.0/::Organics.MW + MA[SodiumOxalate]*2.0/::SodiumOxalate.MW)*MW_C + MA[Organics1]*2.21/::Organics1.MW*MW_C;
      //const double TOCtoS = TOC/NZ(S)*1000.0; //g per kg S
      const double StoLiq = S*1000.0/MLiq;//g of S per kg liquor
      
      const double D25 = LiqConcs25.Density25/1000.0;
      const double Sgl = StoLiq * D25;
      const double R3  = (-2236.1 + (598.99 - 2252.95*AtoC)*CtoS - 3.24526*SO4toS - 7.53787*CltoS)*0.000001;
      const double R4  = (1.79983 - (0.76776 - 2.39436*AtoC)*CtoS)*CtoS*0.000001;
      const double D   = 8.98685*0.000001;
      const double E   = -1.0*(633.401 + 0.451216*Sgl*CtoS)*0.000001;
      const double F   = 4.17188 + R3*Sgl + R4*Sgl*Sgl;
      Cpl = D*Tc*Tc/3.0 + E*Tc/2.0 + F;
      break;
      }
    }

  return Cpl;
  }

//---------------------------------------------------------------------------

double BATCBayerSM::get_msEnthalpy(long Phases, double T, double P, MArray *pMA)
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
    Hl = LiqCpCalc(MA, Tc) * Tc;
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

double BATCBayerSM::get_msEntropy(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_msEntropy(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::get_msCp(long Phases, double T, double P, MArray *pMA)
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

double BATCBayerSM::get_SaturationT(double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  if (MA.Mass(MP_SL)/GTZ(MA.Mass())<1.0e-6)
    return MSpModelBase::get_SaturationT(P, &MA);

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

double BATCBayerSM::get_SaturationP(double T, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  if (MA.Mass(MP_SL)/GTZ(MA.Mass())<1.0e-6)
    return MSpModelBase::get_SaturationP(T, &MA);

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
  double SatP = MSpModelBase::get_SaturationP(T-BPE, &MA);
  if (WaterFrac>sm_dH2OTestFrac1)
    {
    const double Std_SatP = MSpModelBase::get_SaturationP(T, &MA);
    const double Std_Prop = Min(1.0, (WaterFrac-sm_dH2OTestFrac1)/(sm_dH2OTestFrac0-sm_dH2OTestFrac1));
    SatP = (SatP*(1.0-Std_Prop)) + (Std_SatP*Std_Prop);
    }
  return SatP;
  }

//---------------------------------------------------------------------------

#include "brahma_util.cpp"

//---------------------------------------------------------------------------

double BATCBayerSM::BoilPtElev(MArray & MA, double T)
  {
  double TLiq=MA.Mass(MP_Liq); // Total Liquid kg/s
  if (TLiq<1.0e-9)
    return 0.0;

  double BPE = 0.0;
  switch (sm_iBPEMethod)
    {
    case BPM_Dewey:
      {
      if (MA[Alumina]<1.0e-6 && MA[CausticSoda]<1.0e-6 && MA[SodiumCarbonate]<1.0e-6)
        return 0.0;

      // Gram ion per kg liq
      const double giAl2O3    = 1000 * MA[Alumina] / TLiq * 2.0 * 81.97 / ::Alumina.MW; // 81.97 = MW of NaAlO2
      const double giNaOH     = 1000 * MA[CausticSoda] / TLiq
            - (1000 * MA[Alumina] / TLiq * 2.0 * ::CausticSoda.MW/::Alumina.MW);
      const double giNa2CO3   = 1000 * MA[SodiumCarbonate] / TLiq;
      const double giNaCl     = 1000 * MA[SodiumChloride] / TLiq;
      const double giNa2SO4   = 1000 * MA[SodiumSulphate] / TLiq;
      const double giNaF      = 1000 * MA[SodiumFluoride] / TLiq;
      //const double giNa2C5O7  = 1000 * MA[Organics] / TLiq;
      const double giNaOrg    = 1000 * MA[Organics1] / TLiq;
      const double giNa2C2O4  = 1000 * MA[SodiumOxalate] / TLiq;

      const double giSum = giAl2O3 + giNaOH + giNa2CO3 + giNaCl + giNa2SO4 + giNaF + /*giNa2C5O7 + */giNaOrg + giNa2C2O4;

      const double gH2OperKgLiq = 1000.0 - giSum;

      //Gram moles per kg liq
      const double gmAl2O3    = giAl2O3 / 81.97; // 81.97 = MW of NaAlO2
      const double gmNaOH     = giNaOH / ::CausticSoda.MW;
      const double gmNa2CO3   = giNa2CO3 / ::SodiumCarbonate.MW;
      const double gmNaCl     = giNaCl / ::SodiumChloride.MW;
      const double gmNa2SO4   = giNa2SO4 / ::SodiumSulphate.MW;
      const double gmNaF      = giNaF / ::SodiumFluoride.MW;
      //const double gmNa2C5O7  = giNa2C5O7 / ::Organics.MW;
      const double gmNaOrg    = giNaOrg / ::Organics1.MW;
      const double gmNa2C2O4  = giNa2C2O4 / ::SodiumOxalate.MW;

      const double gmSum = gmAl2O3 + gmNaOH + gmNa2CO3 + gmNaCl + gmNa2SO4 + gmNaF + /*gmNa2C5O7 + */gmNaOrg + gmNa2C2O4;

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
      break;
      }
    case BPM_BPD1:
      {
      //Referance: Bayer Process Properties BATC SysCAD Model
      //2.8	Boiling Point Elevation – Basis of Process Design
      //Note that concentrations for SO4, Cl and OC ions are fixed.

      LiqConcs25.Converge(MA);
      const double S = (LiqConcs25.Liq[::CausticSoda] + LiqConcs25.Liq[::SodiumCarbonate]);
      if (S<1.0e-6)
        return 0.0;
      const double A = LiqConcs25.Liq[::Alumina];
      const double C = LiqConcs25.Liq[::CausticSoda];
      //const double RhoT = CBayerConcs::LiquorDensEqn3(T, S, A, C);
      const double Rho25 = CBayerConcs::LiquorDensEqn3(C2K(25.0), S, A, C);
      const double Rac = A/NZ(C);
      const double Rcs = C/NZ(S);
      //const double Rso4 = 73.4;
      //const double Rcl = 31.6;
      //const double Roc = 76.6;
      const double S_ = (MA[SodiumCarbonate] + MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW);
      const double SO4 = MA[SodiumSulphate]/::SodiumSulphate.MW*MW_SO4;
      const double Rso4 = SO4/NZ(S_)*1000.0; //g per kg S
      const double Cl = MA[SodiumChloride]/::SodiumChloride.MW*MW_Cl;
      const double Rcl = Cl/NZ(S_)*1000.0; //g per kg S
      const double TOC = (/*MA[Organics]*5.0/::Organics.MW + */MA[SodiumOxalate]*2.0/::SodiumOxalate.MW)*MW_C + MA[Organics1]*2.21/::Organics1.MW*MW_C;
      const double Roc = TOC/NZ(S_)*1000.0; //g per kg S
      //const double Rho25 = CBayerConcs::LiquorDensEqn4(T, S, A, C, Rso4, Rcl, Roc);
      const double R5 = 1000.0 + 823.53*Rac - 245.28*Rcs;
      const double R6 = 1.4792*Rso4 + 1.6479*Rcl + 2.9247*Roc;
      const double R7 = 943.4*(1+Rcs) + 1.0417*Rso4 + 2.8169*Rcl + 2.2831*Roc;
      //const double Molality = (10*R7*S)/((1000.0*RhoT) - (R5+R6)*S); //Molality (gmole/kg H2O)
      const double Molality = (10*R7*S)/((1000.0*Rho25) - (R5+R6)*S); //Molality (gmole/kg H2O)
      BPE = (8.356e-4 + 7.1949e-3*Pow(Molality,0.66))*K2C(T) + 0.2649*Pow(Molality,1.6) - 0.1159;
      break;
      }
    case BPM_BPD2:
      {
      //Referance: Bayer Process Properties BATC SysCAD Model
      //2.8	Boiling Point Elevation – Basis of Process Design
      //Revised as per email from Melissa Rothnie 28 September 2005
      //now uses actual concentrations for SO4, Cl and OC ions; and user selected density correlation.

      LiqConcs25.Converge(MA);
      const double S = (LiqConcs25.Liq[::CausticSoda] + LiqConcs25.Liq[::SodiumCarbonate]);
      if (S<1.0e-6)
        return 0.0;
      const double A = LiqConcs25.Liq[::Alumina];
      const double C = LiqConcs25.Liq[::CausticSoda];
      const double Rho25 = LDensity25();
      const double Rac = A/NZ(C);
      const double Rcs = C/NZ(S);
      const double S_ = (MA[SodiumCarbonate] + MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW);
      const double SO4 = MA[SodiumSulphate]/::SodiumSulphate.MW*MW_SO4;
      const double Rso4 = SO4/NZ(S_)*1000.0; //g per kg S
      const double Cl = MA[SodiumChloride]/::SodiumChloride.MW*MW_Cl;
      const double Rcl = Cl/NZ(S_)*1000.0; //g per kg S
      const double TOC = (/*MA[Organics]*5.0/::Organics.MW + */MA[SodiumOxalate]*2.0/::SodiumOxalate.MW)*MW_C + MA[Organics1]*2.21/::Organics1.MW*MW_C;
      const double Roc = TOC/NZ(S_)*1000.0; //g per kg S
      const double R5 = 1000.0 + 823.53*Rac - 245.28*Rcs;
      const double R6 = 1.4792*Rso4 + 1.6479*Rcl + 2.9247*Roc;
      const double R7 = 943.4*(1+Rcs) + 1.0417*Rso4 + 2.8169*Rcl + 2.2831*Roc;
      const double Molality = (10*R7*S)/((1000.0*Rho25) - (R5+R6)*S); //Molality (gmole/kg H2O)
      BPE = (8.356e-4 + 7.1949e-3*Pow(Molality,0.66))*K2C(T) + 0.2649*Pow(Molality,1.6) - 0.1159;
      break;
      }
    case BPM_Adamson:
      {
      LiqConcs25.Converge(MA);
      if (LiqConcs25.Liq[::CausticSoda]<1.0e-6 && LiqConcs25.Liq[::SodiumCarbonate]<1.0e-6)
        return 0.0;
      
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
                          + LiqConcs25.Liq[::SodiumOxalate] + /*LiqConcs25.Liq[::Organics] + */LiqConcs25.Liq[::Organics1]
                          + LiqConcs25.Liq[::SodiumChloride] + LiqConcs25.Liq[::SodiumSulphate] + LiqConcs25.Liq[::SodiumFluoride];
      const double x1 = Sodium*::OccSoda.MW/::SodiumCarbonate.MW;
      const double x2 = K2C(T);
      const double x1_2 = x1*x1;
      const double x2_2 = x2*x2;
      BPE = a + b*x1 + c*x2 + d*x1_2 + e*x2_2 + f*x1*x2 + g*x1*x1_2 + h*x2*x2_2 + i*x1*x2_2 + j*x1_2*x2;
      break;
      }
    }

  return BPE;
  }

//---------------------------------------------------------------------------

double BATCBayerSM::get_DynamicViscosity(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_DynamicViscosity(Phases, T, P, pMA);
  /*
  //equation valid in range 20C to 80C. Equation does not account for affect of impuraties
  MArray MA(pMA ? (*pMA) : this);

  const double MSol=(Phases & MP_Sol) ? MA.Mass(MP_Sol) : 0.0;
  const double MLiq=(Phases & MP_Liq) ? MA.Mass(MP_Liq) : 0.0;
  const double MGas=(Phases & MP_Gas) ? MA.Mass(MP_Gas) : 0.0;
  const double MTot=GTZ(MSol+MLiq+MGas);
  if (MLiq<UsableMass)
    return MSpModelBase::get_DynamicViscosity(Phases, T, P, pMA);

  const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989
  const double MW_Na2O      = ::OccSoda.MW;          //61.9789360
  const double MW_NaOH      = ::CausticSoda.MW;      //39.9971080
  LiqConcs25.Converge(MA);
  //const double DensL=get_Density(MP_Liq, T, P, pMA);
  const double DensL=LiqConcs25.LiquorDensity(T, MA);
  const double A = LiqConcs25.Liq[::Alumina]; //Mass Conc. of Al2O3 per litre.	A(as Al2O3)
  const double C1 = LiqConcs25.Liq[::CausticSoda]/MW_Na2CO3*MW_Na2O; //Concentration of Na2O @ 25°C.	C(as Na2O)
  const double C2 = C1*MW_NaOH/MW_Na2O*2.0; //Mass Conc. NaOH per litre.	C(as NaOH)
  const double Tc = Range(0.0, K2C(T), 80.0); //temperature
  const double Tc2 = Tc*Tc;
  const double d1 = (0.0857-0.00658*Tc+0.0000023*Tc2) + (A/1000.0)*(3.56-0.0357*Tc+0.000184*Tc) + (C2/1000.0)*(3.23-0.0034*A-0.1246*Tc+0.00204*Tc2-0.00001107*Tc2*Tc);
  double ViscL = Pow(10, d1); //Liquid viscosity
  //to account for high temperatures: Visc(T) = Visc(80) * WaterVisc(T)/WaterVisc(80).
  if (Tc>80.0)
    {
    double ViscWaterT = MSpModelBase::get_DynamicViscosity(MP_Liq, T, P, pMA); //is there a beter way to call the water properties directly???
    double ViscWater80 = MSpModelBase::get_DynamicViscosity(MP_Liq, C2K(80.0), P, pMA); //is there a beter way to call the water properties directly???
    ViscL *= (ViscWaterT/GTZ(ViscWater80));
    }
  double ViscSL = ViscL;
  if (MSol>0.0)
    {
    const double DensS = MSpModelBase::get_Density(MP_Sol, T, P, pMA);
    const double Sf = MSol/MTot;
    const double Vs = 1.0/(1.0-(1.0-1.0/Sf)*DensS/GTZ(DensL)); //Vol fraction solids
    const double Ratio = 1.0 + 2.5*Vs + 14.1*Vs*Vs + 0.00237*exp(16.0*Vs); //Slurry viscosity/liquid viscosity
    ViscSL = Ratio * ViscL;
    }
  return ViscSL;
  //return MSpModelBase::get_DynamicViscosity(Phases, T, P, pMA);
  */
  }

//---------------------------------------------------------------------------

double BATCBayerSM::get_ThermalConductivity(long Phases, double T, double P, MArray *pMA)
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
 idDensityMethod   		,
 idCPMethod			  	  ,
 idASatMethod         ,
 idBPEMethod       		,
 idMinSolCp        		,
 idH2OTestFrac0    		,
 idH2OTestFrac1    		,

 idDefineLiquor			  ,
 idRqd_A_to_C				  ,
 idRqd_C_to_S				  ,
 idRqd_C					    ,
 idRqd_Na2SO4				  ,
 idRqd_NaCl				    ,
 idRqd_NaF				    ,
 idRqd_Oxalate			  ,
 idRqd_TOC				    ,
 idRqd_SiO2				    ,
 idRqd_SolConc			  ,
 idRqd_SolFrac			  ,

 idAluminaConc25			,
 idAtoC					      ,
 idCtoS					      ,
 idCltoC					    ,
 idTOStoTOC           ,
 idNa2SO4toC				  ,
 idNa2CO3toS				  ,
 idTOC25					    ,
 idTOS25					    ,
 idNaSO4Conc25			  ,
 idNaClConc25				  ,
 idNaFConc25				  ,
 idNa2C2O4Conc25			,
 idFreeCaustic25      ,
 idSolidsConc25			  ,

 idSeparator1				  ,

 idCausticConc25			,
 idSodaConc25				  ,
 idCarbonateConc25		,
 idOrganateConc25			,
 idOxalateConc25			,
 idTotalOrganics25		,
 idChlorineConc25			,
 idSulphateConc25			,
 idFluorideConc25			,
 idTotalNa25				  ,

 idSeparator2				  ,

 idLVolume25				  ,
 idSLVolume25				  ,
 idLDensity25 			  ,
 idSLDensity25			  ,

 idSeparator3				  ,	

 idAluminaConc			  ,
 idSodiumCarbonateConc,
 idTOC					      ,
 idSolidsConc				  ,

 idSeparator4				  ,	

 idCausticConc			  ,
 idSodaConc				    ,

 idSeparator5				  ,	

 idBoilPtElev				  ,
 idAluminaConcSat			,
 idAtoCSaturation			,
 idSSNRatio				    ,
 idTHAMassFlow   			,
 idTHADens				    ,
 idOxalateEquilibrium	,
 idSulphateEquilibrium,
 idCarbonateEquilibrium,

 idMPI_EndOfProps };

//---------------------------------------------------------------------------

long BATCBayerSM::DefinedPropertyCount()
  {
  return idMPI_EndOfProps + 1 + MSpModelBase::DefinedPropertyCount();
  }

//---------------------------------------------------------------------------

/*static MPropertyInfo::MStringValueP SVOrg[]={
  //{"Organics and Ratio", DLM_TotOrganics},
  { "TOC and Oxalate", DLM_TOC},
  {0}};*/
static MPropertyInfo::MStringValueP SVASat[]={
  { "LiqInfo5", ASM_LiqInfo5 },
  { "LiqInfo6", ASM_LiqInfo6   },
#if IncludeBrahma
  { "BRAHMA", ASM_BRAHMA },
#endif
  {0}};
static MPropertyInfo::MStringValueP SVDens[]={
  { "Original", BDM_Original  },
  { "Reynolds-WRS", BDM_WRS },
  //{ "BPD", BDM_BPD1 },
  //{ "BPD_Sept05", BDM_BPD2 },
#if IncludeBrahma
  { "BRAHMA", BDM_BRAHMA},
  { "BRAHMA(old)", BDM_BRAHMA_OLD},
  { "BRAHMA25", BDM_BRAHMA25},
#endif
  {0}};
static MPropertyInfo::MStringValueP SVCp[]={
  { "Original",  CPM_Original   },
  { "Reynolds-WRS",	CPM_WRS },
  {0}};
static MPropertyInfo::MStringValueP SVBPE[]={
  { "Dewey",   BPM_Dewey   },
  { "Adamson", BPM_Adamson },
  { "BPD Original Density", BPM_BPD1 },//{ "BPD", BPM_BPD1 },
  { "BPD Global Density", BPM_BPD2 },//{ "BPD_Sept05", BPM_BPD2 },
  {0}};

long BATCBayerSM::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {//define a list of all properties: This sets what is displayed in the access window.
  long Inx=Index-MSpModelBase::DefinedPropertyCount();
  switch(Inx)
    {
    case idDensityMethod   	: Info.SetText("--Global options---------");
                              Info.SetStructName("Config"); //NB: The "struct name" is used as part of tag!
                              Info.Set(ePT_Long,      "", "DensityMethod",            SVDens,                     MP_GlobalProp|MP_Parameter,       "Global Density method"); return Inx;
    case idCPMethod			    : Info.Set(ePT_Long,      "", "CPMethod",                 SVCp,                       MP_GlobalProp|MP_Parameter,       "Global CP method"); return Inx;
    case idASatMethod			  : Info.Set(ePT_Long,      "", "ASatMethod",               SVASat,					            MP_GlobalProp|MP_Parameter,       "Global ASat Method"); return Inx;
    case idBPEMethod       	: Info.Set(ePT_Long,      "", "BPEMethod",                SVBPE,                      MP_GlobalProp|MP_Parameter,       "Global BPE method"); return Inx;
    case idMinSolCp        	: Info.Set(ePT_Double,    "", "MinSolCp",                 MC_CpMs,  "kJ/kg.C", 0, 0,  MP_GlobalProp|MP_Parameter|MP_InitHidden, "Global minimum solids Cp"); return Inx;
    case idH2OTestFrac0     : Info.Set(ePT_Double,    "", "WaterTestFrac0",           MC_Frac,  "%",   0, 0,      MP_GlobalProp|MP_Parameter|MP_InitHidden, "Global water fraction above which Standard properties are used"); return Inx;
    case idH2OTestFrac1     : Info.Set(ePT_Double,    "", "WaterTestFrac1",           MC_Frac,  "%",   0, 0,      MP_GlobalProp|MP_Parameter|MP_InitHidden, "Global water fraction above which proportional amounts of two properties models are used"); return Inx;

    case idDefineLiquor			: Info.SetText("--Calculator-------------");
                              Info.SetStructName("Calc"); //NB: The "struct name" is used as part of tag!
                              Info.Set(ePT_Bool,     "", "DefineLiquor",              MC_,      "",    0, 0,    MP_ConfigProp|MP_Parameter|MP_CheckBox, ""); return Inx;
    case idRqd_A_to_C			  : Info.Set(ePT_Double,   "", "Rqd_A/C",                   MC_,      "",    0, 0,    MP_ConfigProp|MP_Parameter, "The required Alumina/Caustic Ratio"); return Inx;
    case idRqd_C_to_S			  : Info.Set(ePT_Double,   "", "Rqd_C/S",                   MC_,      "",    0, 0,    MP_ConfigProp|MP_Parameter, "The required Caustic/Soda Ratio"); return Inx;
    case idRqd_C				    : Info.Set(ePT_Double,   "", "Rqd_C",                     MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Caustic Conc expressed as Carbonate @ 25"); return Inx;
    case idRqd_Na2SO4			  : Info.Set(ePT_Double,   "", "Rqd_Na2SO4",                MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Sodium Sulphate Concentration @ 25"); return Inx;
    case idRqd_NaCl				  : Info.Set(ePT_Double,   "", "Rqd_NaCl",                  MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Sodium Chloride Concentration @ 25"); return Inx;
    case idRqd_NaF			    : Info.Set(ePT_Double,   "", "Rqd_NaF",                   MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Sodium Fluoride Concentration @ 25"); return Inx;
    case idRqd_Oxalate			: Info.Set(ePT_Double,   "", "Rqd_Oxalate",               MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Oxalate Concentration @ 25"); return Inx;
    case idRqd_TOC				  : Info.Set(ePT_Double,   "", "Rqd_TOC",                   MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Total Organic Carbon Conc expressed as Carbon @ 25"); return Inx;
    case idRqd_SiO2				  : Info.Set(ePT_Double,   "", "Rqd_SiO2",                  MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The required Silica Concentration @ 25"); return Inx;
    case idRqd_SolConc			: Info.Set(ePT_Double,   "", "Rqd_SolConc",               MC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter|MP_NanOK, "The required Solids Conc (*=unused)"); return Inx;
    case idRqd_SolFrac			: Info.Set(ePT_Double,   "", "Rqd_SolFrac",               MC_Frac,  "%",   0, 0,    MP_ConfigProp|MP_Parameter, "The required Solids Fraction"); return Inx;


    case idAluminaConc25		: Info.SetStructName("Props"); //NB: The "struct name" is used as part of tag! Recomend "Props" for compatability with other properties models.
                              Info.SetText("--Concentration and Ratio @ 25,-----------");
                              Info.Set(ePT_Double,    "", "Alumina@25",               MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Alumina Concentration @ 25"); return Inx;
    case idAtoC					    : Info.Set(ePT_Double,    "", "A/C",                      MC_,     "",       0,      0,  MP_Result|MP_NoFiling,    "A to C ratio"); return Inx;
    case idCtoS					    : Info.Set(ePT_Double,    "", "C/S",                      MC_,     "",       0,      0,  MP_Result|MP_NoFiling,    "C to S ratio"); return Inx;
    case idCltoC				    : Info.Set(ePT_Double,    "", "Cl/C",                     MC_,     "",       0,      0,  MP_Result|MP_NoFiling|MP_InitHidden,    "Cl to C ratio"); return Inx;
    case idTOStoTOC 		    : Info.Set(ePT_Double,    "", "TOS/TOC",                  MC_,     "",       0,      0,  MP_Result|MP_NoFiling,    "TOS to TOC ratio"); return Inx;
    case idNa2SO4toC			  : Info.Set(ePT_Double,    "", "SO4/C",                    MC_,     "",       0,      0,  MP_Result|MP_NoFiling|MP_InitHidden,    "Na2SO4 to Caustic ratio"); return Inx;
    case idNa2CO3toS			  : Info.Set(ePT_Double,    "", "CO3/S",                    MC_,     "",       0,      0,  MP_Result|MP_NoFiling|MP_InitHidden,    "Na2CO3 to Soda ratio"); return Inx;
    case idTOC25				    : Info.Set(ePT_Double,    "", "TOC@25",                   MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,       "Total Organic Carbon Concentration @ 25"); return Inx;
    case idTOS25				    : Info.Set(ePT_Double,    "", "TOS@25",                   MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,       "Total Organic Soda Concentration @ 25"); return Inx;
    case idNaSO4Conc25			: Info.Set(ePT_Double,    "", "Sulphate*@25",			        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "SodiumSulphate Concentration @ 25"); return Inx;
    case idNaClConc25			  : Info.Set(ePT_Double,    "", "Chloride*@25",			        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling|MP_InitHidden,    "SodiumChloride Concentration @ 25"); return Inx;
    case idNaFConc25			  : Info.Set(ePT_Double,    "", "Fluoride*@25",			        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling|MP_InitHidden,    "SodiumFluoride Concentration @ 25"); return Inx;
    case idNa2C2O4Conc25		: Info.Set(ePT_Double,    "", "Oxalate*@25",			        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "SodiumOxalate Concentration @ 25"); return Inx;
    case idFreeCaustic25		: Info.Set(ePT_Double,    "", "FreeCaustic@25",           MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling|MP_InitHidden,    "Free Caustic Concentration @ 25"); return Inx;
    case idSolidsConc25			: Info.Set(ePT_Double,    "", "SolidsConc@25",            MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Solids Concentration @ 25");return Inx;

    case idSeparator1			  : Info.SetText("..."); return Inx;

    case idCausticConc25		: Info.SetText("--Concentrations @ 25, as Na2CO3 Equiv----");
                              Info.Set(ePT_Double,    "", "Caustic@25",				        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Caustic Concentration @ 25"); return Inx;
    case idSodaConc25			  : Info.Set(ePT_Double,    "", "Soda@25",				          MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Soda Concentration @ 25"); return Inx;
    case idCarbonateConc25	: Info.Set(ePT_Double,    "", "Carbonate@25",			        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "SodiumCarbonate Concentration @ 25"); return Inx;
    case idSulphateConc25		: Info.Set(ePT_Double,    "", "Sulphate@25",			        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "SodiumSulphate Concentration @ 25"); return Inx;
    case idChlorineConc25		: Info.Set(ePT_Double,    "", "Chloride@25",			        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling|MP_InitHidden,    "SodiumChloride Concentration @ 25"); return Inx;
    case idFluorideConc25		: Info.Set(ePT_Double,    "", "Fluoride@25",			        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling|MP_InitHidden,    "SodiumFluoride Concentration @ 25"); return Inx;
    case idOxalateConc25		: Info.Set(ePT_Double,    "", "Oxalate@25",				        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "SodiumOxalate Concentration @ 25"); return Inx;
    case idOrganateConc25		: Info.Set(ePT_Double,    "", "Organate@25",			        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling|MP_InitHidden,    "Organate Concentration @ 25"); return Inx;
    case idTotalOrganics25	: Info.Set(ePT_Double,    "", "TotalOrganics@25",         MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Total Organic Concentration @ 25"); return Inx;
    case idTotalNa25			  : Info.Set(ePT_Double,    "", "TotalNa@25",               MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Total Sodium Concentration @ 25"); return Inx;

    case idSeparator2		  	: Info.SetText("..."); return Inx;

    case idLVolume25			  : Info.SetText("--Volume and Density @ 25-----------------");
                              Info.Set(ePT_Double,    "", "LVolume@25",               MC_Qv,   "L/s",    0,      0,  MP_Result|MP_NoFiling,    "Liquor Volumetric flowrate @ 25"); return Inx;
    case idSLVolume25			  : Info.Set(ePT_Double,    "", "SLVolume@25",              MC_Qv,   "L/s",    0,      0,  MP_Result|MP_NoFiling,    "Slurry Volumetric flowrate @ 25"); return Inx;
    case idLDensity25 		  : Info.Set(ePT_Double,    "", "LDensity@25",              MC_Rho,  "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Liquor Density @ 25"); return Inx;
    case idSLDensity25		  : Info.Set(ePT_Double,    "", "SLDensity@25",             MC_Rho,  "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Slurry Density @ 25"); return Inx;

    case idSeparator3			  : Info.SetText("..."); return Inx;

    case idAluminaConc			: Info.SetText("--Concentration @ T-----------------------");
                              Info.Set(ePT_Double,    "", "Alumina@T",				        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Alumina Concentration @ T"); return Inx;
    case idSodiumCarbonateConc: Info.Set(ePT_Double,  "", "Carbonate@T",			        MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling|MP_InitHidden,    "SodiumCarbonate Concentration @ T"); return Inx;
    case idTOC					    : Info.Set(ePT_Double,    "", "TOC@T",                    MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,       "Total Organic Carbon Concentration @ T"); return Inx;
    case idSolidsConc			  : Info.Set(ePT_Double,    "", "SolidsConc@T",             MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Solids Concentration @ T"); return Inx;

    case idSeparator4			  : Info.SetText("..."); return Inx;

    case idCausticConc			: Info.SetText("--Concentration @ T, as Na2CO3 Equiv -----");
                              Info.Set(ePT_Double,    "", "Caustic@T",                MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Caustic Concentration @ T"); return Inx;
    case idSodaConc				  : Info.Set(ePT_Double,    "", "Soda@T",                   MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Soda Concentration @ T"); return Inx;

    case idSeparator5			  : Info.SetText("..."); return Inx;

    case idBoilPtElev   		: Info.SetText("--Other Bayer Liquor Properties @ T-------");
                              Info.Set(ePT_Double,    "", "BoilPtElev",               MC_dT,   "C",      0,      0,  MP_Result|MP_NoFiling,       "Boiling Point Elevation"); return Inx;
    case idAluminaConcSat		: Info.Set(ePT_Double,    "", "AluminaSatConc",           MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,       "Alumina Saturation Concentration @ T"); return Inx;
    case idAtoCSaturation		: Info.Set(ePT_Double,    "", "ASat_To_C",                MC_,     "",       0,      0,  MP_Result|MP_NoFiling|MP_InitHidden,       "Alumina Saturation to Caustic ratio @ T"); return Inx;
    case idSSNRatio				  : Info.Set(ePT_Double,    "", "SSNRatio",                 MC_,     "",       0,      0,  MP_Result|MP_NoFiling,       "A/C actual to ASat/C ratio @ T"); return Inx;
    case idTHAMassFlow   		: Info.Set(ePT_Double,    "", "THAMassFlow",              MC_Qm,   "kg/s",   0,      0,  MP_Result|MP_NoFiling,    "THA flow rate"); return Inx;
    case idTHADens				  : Info.Set(ePT_Double,    "", "THADens",                  MC_Rho,  "kg/m^3", 0,      0,  MP_Result|MP_NoFiling|MP_InitHidden,    "THA Density"); return Inx;
    case idOxalateEquilibrium	  : Info.Set(ePT_Double, "", "OxalateSolubility",       MC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,       "Oxalate Equilibrium Concentration @ T"); return Inx;
    case idSulphateEquilibrium	: Info.Set(ePT_Double, "", "SulphateSolubility",      MC_,     "",       0,      0,  MP_Result|MP_NoFiling,       "Sulphate Equilibrium Concentration @ T (g/kg)"); return Inx;
    case idCarbonateEquilibrium	: Info.Set(ePT_Double, "", "CarbonateSolubility",     MC_,     "",       0,      0,  MP_Result|MP_NoFiling,       "Carbonate Equilibrium Concentration @ T (g/kg)"); return Inx;

    case idMPI_EndOfProps		: return MPI_EndOfProps;    

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

DWORD BATCBayerSM::GetPropertyVisibility(long Index)
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
    case idRqd_SiO2: 
    case idRqd_SolConc: 
    case idRqd_SolFrac: return fDoCalc ? ePVis_All : (ePVis_DynFull|ePVis_DynFlow|ePVis_Probal|ePVis_File);
    //case idRqd_Organic: 
    //case idRqd_OrgRatio: return fDoCalc ? ePVis_All : (ePVis_DynFull|ePVis_DynFlow|ePVis_Probal|ePVis_File);
    default:  return ePVis_All; 
    }
  return ePVis_All;
  }

//---------------------------------------------------------------------------

void BATCBayerSM::GetPropertyValue(long Index, ULONG Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {//define method of retrieving values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    case idDensityMethod    : Value=sm_iDensityMethod;                  return; 
    case idCPMethod        	: Value=sm_iCPMethod;						            return; 
    case idASatMethod			  : Value=sm_iASatMethod;                     return; 
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
    case idRqd_TOC			  	: Value=dRqd_TOC;                           return;
    case idRqd_Na2SO4			  : Value=dRqd_Sul;                           return;
    case idRqd_NaCl				  : Value=dRqd_Salt;                          return;
    case idRqd_NaF			    : Value=dRqd_Fl;                            return;
    case idRqd_SiO2				  : Value=dRqd_Sil;                           return;
    case idRqd_SolConc			: Value=dRqd_SolConc;                       return;
    case idRqd_SolFrac			: Value=dRqd_SolFrac;                       return;

    case idTHAMassFlow			: Value=THAMassFlow();                      return; 
    case idTHADens				  : Value=THADens(T);                         return; 
    case idCausticConc			: Value=CausticConc(T);                     return; 
    case idAluminaConc			: Value=AluminaConc(T);                     return; 
    case idSodaConc				  : Value=SodaConc(T);                        return; 
    case idSodiumCarbonateConc	: Value=SodiumCarbonateConc(T);             return; 

    case idCausticConc25		: Value=LiqConcs25.Liq[::CausticSoda];      return; 
    case idAluminaConc25		: Value=LiqConcs25.Liq[::Alumina];          return; 
    case idSodaConc25			  : Value=LiqConcs25.Liq[::CausticSoda]+ LiqConcs25.Liq[::SodiumCarbonate]; return; 

    case idCarbonateConc25	: Value=LiqConcs25.Liq[::SodiumCarbonate];  return; 
    case idNaClConc25			  : Value=LiqConcs25.Liq[::SodiumChloride]*::SodiumChloride.MW*2/::SodiumCarbonate.MW;   return; 
    case idNaFConc25			  : Value=LiqConcs25.Liq[::SodiumFluoride]*::SodiumFluoride.MW*2/::SodiumCarbonate.MW;   return; 
    case idNaSO4Conc25			: Value=LiqConcs25.Liq[::SodiumSulphate]*::SodiumSulphate.MW/::SodiumCarbonate.MW;   return; 
    case idNa2C2O4Conc25		: Value=LiqConcs25.Liq[::SodiumOxalate]*::SodiumOxalate.MW/::SodiumCarbonate.MW;    return; 
    case idSolidsConc			  : Value=SolidsConc(T);                      return; 
    case idSolidsConc25			: Value=SolidsConc25();                     return; 
    case idFreeCaustic25    : Value=FreeCaustic(C_2_K(25.0));           return; 
    case idTOC					  : Value=TOC(T);                             return; 
    case idTOC25				  : Value=TOC25();							              return; 
    case idTOS25				  : Value=TOS25();							              return; 

    case idAtoC					  : Value=AtoC();                             return; 
    case idCtoS					  : Value=CtoS();                             return; 
    case idCltoC				  : Value=CltoC();                            return; 
    case idTOStoTOC				: Value=TOStoTOC();                         return; 
    case idNa2SO4toC			: Value=Na2SO4toC();                        return; 
    case idNa2CO3toS			: Value=Na2CO3toS();                        return; 

    case idBoilPtElev			: Value=BoilPtElev(MArray(this), T);        return; 
    case idLVolume25			: Value=LVolume25();                        return; 
    case idSLVolume25			: Value=SLVolume25();                       return; 
    case idOrganateConc25		: Value=OrganateConc25();                   return; 
    case idOxalateConc25		: Value=OxalateConc25();                    return; 
    case idTotalOrganics25	: Value=TotalOrganicsConc25();              return; 
    case idChlorineConc25		: Value=ChlorineConc25();                   return; 
    case idSulphateConc25		: Value=SulphateConc25();                   return; 
    case idFluorideConc25		: Value=FluorideConc25();                   return; 
    case idTotalNa25			  : Value=TotalSodiumConc25();                return; 

    case idAluminaConcSat		: Value=AluminaConcSat(T);                  return; 
    case idOxalateEquilibrium	  : Value=OxalateSolubility(T);           return; 
    case idSulphateEquilibrium	: Value=SulphateSolubility(T);          return; 
    case idCarbonateEquilibrium	: Value=CarbonateSolubility(T);         return; 
    case idAtoCSaturation		: Value=AtoCSaturation(T);                  return; 
    case idSSNRatio				: Value=SSNRatio(T);                        return; 

    case idLDensity25			: Value=LDensity25();                        return; 
    case idSLDensity25			: Value=SLDensity25();                       return; 
		
    default: Value=0.0; return;
    }
  }

//---------------------------------------------------------------------------

void BATCBayerSM::PutPropertyValue(long Index, MPropertyValue & Value)
  {//define method of setting values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    case idDensityMethod  : sm_iDensityMethod=(byte)(long)Value;  return; 
    case idCPMethod       : sm_iCPMethod=(byte)(long)Value;		    return; 
    case idASatMethod		  : sm_iASatMethod=(byte)(long)Value;     return;
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
    case idRqd_NaF		    : dRqd_Fl=Value;                        return;
    case idRqd_SiO2			  : dRqd_Sil=Value;                       return;
    case idRqd_SolConc		: dRqd_SolConc=Value;                   return;
    case idRqd_SolFrac		: dRqd_SolFrac=Value;                   return;
    }
  }

//---------------------------------------------------------------------------
//A:--------------------------------
    //toc = (OrConc*5 + OxConc*2) *12/106
    //tor = OrConc + OxConc
    //OxConc = tor * OrgRatio
    //OrConc = tor - OxConc   ==> OrConc = tor - tor*OrgRatio  ==>  OrConc = tor*(1 - OrgRatio)
    //tor=f(toc,OrgRatio):
    //==> toc = (tor*5*(1 - OrgRatio) + tor*OrgRatio*2)*12/106
    //==> toc = tor*(5*(1 - OrgRatio) + 2*OrgRatio)*12/106
    //==> toc = tor*(5 - 3*OrgRatio)*12/106
    //==> tor = toc/(5 - 3*OrgRatio)/12*106
    //tor=f(toc,OxConc):
    //==> tor = toc/(5 - 3*OxConc/tor)/12*106
    //==> tor = toc*tor/(5*tor-3*OxConc)/12*106
    //==> 5*tor-3*OxConc = toc/12*106
    //==> 5*tor = toc/12*106 + 3*OxConc
    //==> tor = (toc/12*106 + 3*OxConc)/5
    //OrgRatio=f(OxConc,toc)
    //==> OrgRatio = OxConc/tor
    //==> OrgRatio = (OxConc*5)/(toc/12*106 + 3*OxConc)
    //==> OrgRatio = 5/(toc/OxConc/12*106 + 3)
    //toc=f(tor,OrgRatio)
    //==> tor = toc/(5 - 3*OrgRatio)/12*106
    //==> toc = tor/(5 - 3*OrgRatio)*12/106

//B:--------------------------------
    //toc = (OrConc*2.21*2 + OxConc*2) *12/106
    //tor = OrConc + OxConc
    //OxConc = tor * OrgRatio
    //OrConc = tor - OxConc   ==> OrConc = tor - tor*OrgRatio  ==>  OrConc = tor*(1 - OrgRatio)
    //tor=f(toc,OrgRatio):
    //==> toc = (tor*2.21*2*(1 - OrgRatio) + tor*OrgRatio*2)*12/106
    //==> toc = tor*(2.21*2*(1 - OrgRatio) + 2*OrgRatio)*12/106
    //==> toc = tor*(2.21*2 + (2-2.21*2)*OrgRatio)*12/106
    //==> tor = toc/(2.21*2 + (2-2.21*2)*OrgRatio)/12*106
    //tor=f(toc,OxConc):
    //==> tor = toc/(2.21*2 + (2-2.21*2)*OxConc/tor)/12*106
    //==> tor = toc*tor/(2.21*2*tor + (2-2.21*2)*OxConc)/12*106
    //==> 2.21*2*tor + (2-2.21*2)*OxConc = toc/12*106
    //==> 2.21*2*tor = toc/12*106 - (2-2.21*2)*OxConc
    //==> tor = (toc/12*106 - (2-2.21*2)*OxConc)/(2.21*2)
    //OrgRatio=f(OxConc,toc)
    //==> OrgRatio = OxConc/tor
    //==> OrgRatio = (OxConc*2.21*2)/(toc/12*106 - (2-2.21*2)*OxConc)
    //==> OrgRatio = 2.21*2/(toc/OxConc/12*106 - (2-2.21*2))
    //toc=f(tor,OrgRatio)
    //==> tor = toc/(2.21*2 + (2-2.21*2)*OrgRatio)/12*106
    //==> toc = tor/(2.21*2 + (2-2.21*2)*OrgRatio)*12/106

void BATCBayerSM::InputCalcs(bool DoIt, double T_)
  {
  /*switch (View)
    {
    case SVV_AsRawFrac:
    case SVV_AsRawMass:
    case SVV_AsRawMassFlow:
      fDoCalc=false;
      return;
    }*/
  if (DoIt)
    {
    //Change inputs if out of range
    dRqd_AtoC   = Range(0.0, dRqd_AtoC,    0.9);
    dRqd_C      = Range(0.0, dRqd_C,       9000.0);
    dRqd_CtoS   = Range(0.1, dRqd_CtoS,    1.0);
    dRqd_Sul    = Range(0.0, dRqd_Sul,     500.0);
    dRqd_Sil    = Range(0.0, dRqd_Sil,     500.0);
    dRqd_Salt   = Range(0.0, dRqd_Salt,    500.0);
    dRqd_Fl     = Range(0.0, dRqd_Fl,      500.0);
    dRqd_Ox     = Range(0.0, dRqd_Ox,      500.0);
    dRqd_TOC    = Range(0.0, dRqd_TOC,     500.0);
    dRqd_SolFrac= Range(0.0, dRqd_SolFrac, 0.99);
    //if (FindingConc)
    //  dRqd_SolConc = Range(0.0, dRqd_SolConc, 5000.0);

    //calculator to determine Liquor feed fractions based on requirements at 25C...
    double DensL = 1100.0;
    double DensS = 2420.0;
    const bool FindingConc=Valid(dRqd_SolConc);
    const bool WantSolids=(FindingConc && dRqd_SolConc>1e-6) || (dRqd_SolFrac>1e-6);
    double TotalMass = Mass(MP_SL);
    if (WantSolids)
      {
      if (FindingConc)
        dRqd_SolFrac=Range(0.0, (dRqd_SolConc*DensS)/(dRqd_SolConc*DensS + DensL*DensS - DensL*dRqd_SolConc), 0.99); // Initial Guess
      if (Mass(MP_Sol)<1.0e-6 && TotalMass>UsableMass)
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
    const double LiqMassCalc = (M[Water] + M[CausticSoda] + M[Alumina] + M[SodiumCarbonate] + M[SodiumChloride] + M[SodiumFluoride] +
                          M[SodiumSulphate] + M[SilicateLiq] + M[SodiumOxalate] + /*M[Organics] + */M[Organics1]);
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
      bool BadDensity = false;
      double WaterUsed = 0.0;
      const double P = Pressure;
      for (bool Done=false; !Done; )
        {
        dRqd_SolFrac=Range(0.0, dRqd_SolFrac, 0.99);
        LiqMass = TotalMass*(1.0-dRqd_SolFrac);

        double Density25;

        double Al, Caustic, Carbonate, Salt, NaSulphate, Silicate, NaFluoride;
        double water, Derr, Org1, Org2;
        double Ox, Og, Other;

        const double MW_Na2C2O4   = ::SodiumOxalate.MW;    //133.999
        //const double MW_Na2C5O7   = ::Organics.MW;         //218.030
        const double MW_NaOrg     = ::Organics1.MW;        //89.5915
        const double minTOC = dRqd_Ox*2.0/MW_Na2C2O4*MW_C;
        double TocUsed = dRqd_TOC;
        if (dRqd_TOC<minTOC)
          {
          //LogWarning(FullObjTag(), 0, "Required TOC too low based on specified Oxalate");
          TocUsed = minTOC; //this will give Og of zero
          }
        Ox = dRqd_Ox;
        Og = (TocUsed/MW_C - dRqd_Ox*2.0/MW_Na2C2O4)*MW_NaOrg/2.21;
        if (dRqd_C>1.0e-6)
          {
          const double MW_NaOH      = ::CausticSoda.MW;      //39.9971080
          const double MW_Na2CO3    = ::SodiumCarbonate.MW;  //105.989
          const double MW_NaCl      = ::SodiumChloride.MW;   //58.4425
          const double MW_Na2SO4    = ::SodiumSulphate.MW;   //142.043
          const double MW_NaF       = ::SodiumFluoride.MW;   //
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
                Tna = (dRqd_C/dRqd_CtoS + (Ox/MW_Na2C2O4 + Og/(2*MW_NaOrg) + dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4 + dRqd_Fl/(2*MW_NaF)) * MW_Na2CO3)
                  * Vol/(1000.0*Lm)*100.0; //Wgt % of Total Na
                Tcc = dRqd_C/(Dens1*1000.0)*100.0;//Wgt % of Caustic
                Tal = dRqd_AtoC * Tcc; //Wgt % of Alumina
                Density25 = CBayerConcs::LiquorDensEqn1(25.0, Tna, Tal)/1000.0;
                break;
                }
              case BDM_WRS:
                {
                const double S = dRqd_C/dRqd_CtoS*Lm/Dens1;
                const double AtoC = dRqd_AtoC;
                const double CtoS = dRqd_CtoS;
                const double CltoS = dRqd_Salt*MW_Cl/MW_NaCl*Lm/Dens1/NZ(S);
                const double FtoS = dRqd_Fl*MW_F/MW_NaF*Lm/Dens1/NZ(S);
                const double SO4toS = dRqd_Sul*MW_SO4/MW_Na2SO4*Lm/Dens1/NZ(S);
                const double OxFlowCarbon = Ox*2.0/MW_Na2C2O4*MW_C;
                const double OgFlowCarbon = Og*2.21/MW_NaOrg*MW_C;
                const double TOCtoS = (OxFlowCarbon+OgFlowCarbon)*Lm/Dens1/NZ(S);
                const double StoLiq = S/Lm;//g of S per kg liquor
                Density25 = CBayerConcs::LiquorDensEqn2(C2K(25.0), StoLiq, CtoS, AtoC, CltoS, FtoS, SO4toS, TOCtoS)/1000.0;
                if (Density25<0.1)
                  {//todo: try fix bug for when this goes tiny?
                  BadDensity = true;
                  Density25 = 0.1;
                  }
                break;
                }
              case BDM_BPD2: //NOT implemented, use BDM_BPD1
              case BDM_BPD1:
                {
                if (0)
                  {
                  const double S = dRqd_C/dRqd_CtoS*Lm/Dens1;
                  const double A = dRqd_AtoC*dRqd_C*Lm/Dens1;
                  const double C = dRqd_C*Lm/Dens1;
                  Density25 = CBayerConcs::LiquorDensEqn3(C2K(25.0), S, A, C)/1000.0;
                  }
                const double S = dRqd_C/dRqd_CtoS*Lm/Dens1;
                const double AtoC = dRqd_AtoC;
                const double CtoS = dRqd_CtoS;
                const double CltoS = dRqd_Salt*MW_Cl/MW_NaCl*Lm/Dens1/NZ(S);
                const double FtoS = dRqd_Fl*MW_F/MW_NaF*Lm/Dens1/NZ(S);
                const double SO4toS = dRqd_Sul*MW_SO4/MW_Na2SO4*Lm/Dens1/NZ(S);
                const double OxFlowCarbon = Ox*2.0/MW_Na2C2O4*MW_C;
                const double OgFlowCarbon = Og*2.21/MW_NaOrg*MW_C;
                const double TOCtoS = (OxFlowCarbon+OgFlowCarbon)*Lm/Dens1/NZ(S);
                const double StoLiq = S/Lm;//g of S per kg liquor
                Density25 = CBayerConcs::LiquorDensEqn2(C2K(25.0), StoLiq, CtoS, AtoC, CltoS, FtoS, SO4toS, TOCtoS)/1000.0;
                break;
                }
              case BDM_BRAHMA25:
              case BDM_BRAHMA:
                {
                const double C = dRqd_C;
                const double S = dRqd_C/dRqd_CtoS;
                const double A = dRqd_C*dRqd_AtoC;
                const double Cl = dRqd_Salt*MW_Cl/MW_NaCl; 
                const double F = dRqd_Fl*MW_F/MW_NaF;
                const double SO4 = dRqd_Sul*MW_SO4/MW_Na2SO4;
                const double TOC = dRqd_Ox*2*12.11/134.012 + Og*12.11*2.21/89.5951;	  
                Density25 = CBayerConcs::LiquorDensEqn5(25.0, dRqd_C,  S,  A, Cl, SO4, TOC)/1000.;
                break;
                }		  
              case BDM_BRAHMA_OLD:
                {
                const double C = dRqd_C;
                const double S = dRqd_C/dRqd_CtoS;
                const double A = dRqd_C*dRqd_AtoC;
                const double Cl = dRqd_Salt*MW_Cl/MW_NaCl; 
                const double F = dRqd_Fl*MW_F/MW_NaF;
                const double SO4 = dRqd_Sul*MW_SO4/MW_Na2SO4;
                const double TOS = dRqd_Ox;
                Density25 = CBayerConcs::LiquorDensEqn4(25.0, dRqd_C,  S,  A, Cl, SO4, TOS)/1000.;
                break;
              }
              default:
                //ERROR NOT IMPLEMENTED!!!
                break;
              }//end switch
            Derr = fabs(Dens1 - Density25);
            if ( Derr < 1.0e-10)
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
          Org1       = dRqd_Ox * Vol;
          Org2       = Og * Vol;
          Other      = OtherLiqMass/LiqMass*Vol*Dens1*1000.0;

          double TmpTot = Org1 + Org2 + NaSulphate + NaFluoride + Silicate + Salt + Carbonate + Al + Caustic;
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
          NaFluoride /= TmpTot;
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
          M[SodiumFluoride]=NaFluoride*LiqMass;
          M[SodiumSulphate]=NaSulphate*LiqMass;
          M[SilicateLiq]=Silicate*LiqMass;
          M[SodiumOxalate]=Org1*LiqMass;
          //M[Organics]=0.0;
          M[Organics1]=Org2*LiqMass;
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
          M[SodiumFluoride]=0.0;
          M[SodiumSulphate]=0.0;
          M[SilicateLiq]=0.0;
          M[SodiumOxalate]=0.0;
          //M[Organics]=0.0;
          M[Organics1]=0.0;
          }
        WaterUsed=M[Water];

        if (FindingConc)
          {
          double PrevSF=dRqd_SolFrac;
          DensL=Density25*1000.0;
          DensS=MSpModelBase::get_Density(MP_Sol, C_2_K(25.0), P, NULL);
          dRqd_SolFrac=Max(0.0, (dRqd_SolConc*DensS)/(dRqd_SolConc*DensS + DensL*DensS - DensL*dRqd_SolConc));
          if (fabs(PrevSF-dRqd_SolFrac)<1e-9)
            Done=true;
          }
        else
          Done=true;
        }// while
      if (BadDensity)
        Log.Message(MMsg_Warning, "Bayer feed calculator Bad liquor density!");
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
  
double BATCBayerSM::CausticConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[::CausticSoda] * DRatio);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::AluminaConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[::Alumina]*DRatio);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::SodaConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  double liq25S = LiqConcs25.Liq[::CausticSoda]
                  //+ LiqConcs25.Liq[::SodiumAluminate]
                  + LiqConcs25.Liq[::SodiumCarbonate];

  return Max(0.0, liq25S * DRatio);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::SodiumCarbonateConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[SodiumCarbonate]*DRatio);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::SodiumSulphateConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[SodiumSulphate]*DRatio);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::SodiumOxalateConc(double T_)
  {
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[SodiumOxalate]*DRatio);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::SolidsConc(double T_)
  {
  const double mt=Mass(MP_Sol);
  return (mt>=UsableMass) ? mt/GTZ(Mass(MP_SL))*get_Density(MP_SL, T_, Pressure, NULL) : 0.0;
  }
double BATCBayerSM::SolidsConc25()
  {
  const double mt=Mass(MP_Sol);
  return (mt>=UsableMass) ? mt/GTZ(Mass(MP_SL))*get_Density(MP_SL, C_2_K(25.0), Pressure, NULL) : 0.0;
  }

//---------------------------------------------------------------------------

/*double BATCBayerSM::TOOC(double T_)
  {//TOOC as Na2CO3 equivilent;  TOOC = TOC*106/12
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  double Tooc = (LiqConcs25.Liq[::Organics]*5.0 + LiqConcs25.Liq[::Organics1]*2.21 + LiqConcs25.Liq[::SodiumOxalate]*2.0);
  return Max(0.0, Tooc*DRatio);
  }*/

//---------------------------------------------------------------------------

double BATCBayerSM::TOC(double T_)
  {
  //TOOC = TOC*106/12
  LiqConcs25.Converge(MArray(this));
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray(this))/LiqConcs25.Density25);
  double Toc = (/*LiqConcs25.Liq[::Organics]*5.0 + */LiqConcs25.Liq[::SodiumOxalate]*2.0)*MW_C/::SodiumCarbonate.MW
      + LiqConcs25.Liq[::Organics1]*2.21*MW_C/::SodiumCarbonate.MW*2.0;
  return Max(0.0, Toc*DRatio);
  }

double BATCBayerSM::TOC25()
  {
  //TOOC = TOC*106/12
  LiqConcs25.Converge(MArray(this));
  return (/*LiqConcs25.Liq[::Organics]*5.0 + */LiqConcs25.Liq[::SodiumOxalate]*2.0)*MW_C/::SodiumCarbonate.MW
      + LiqConcs25.Liq[::Organics1]*2.21*MW_C/::SodiumCarbonate.MW*2.0;
  }
//---------------------------------------------------------------------------

double BATCBayerSM::TOS(double T_)
  {
  MArray MA(this);
  LiqConcs25.Converge(MA);
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MA)/LiqConcs25.Density25);
  const double InorganicSoda = LiqConcs25.Liq[::SodiumSulphate] + LiqConcs25.Liq[::SodiumChloride] 
         + LiqConcs25.Liq[::SodiumFluoride] + LiqConcs25.Liq[::SodiumCarbonate] + LiqConcs25.Liq[::CausticSoda];
  const double TSodium = InorganicSoda + LiqConcs25.Liq[::SodiumOxalate] + /*LiqConcs25.Liq[::Organics] + */LiqConcs25.Liq[::Organics1];
  return (TSodium - InorganicSoda)*DRatio;
  }

double BATCBayerSM::TOS25()
  {
  MArray MA(this);
  LiqConcs25.Converge(MA);
  const double InorganicSoda = LiqConcs25.Liq[::SodiumSulphate] + LiqConcs25.Liq[::SodiumChloride] 
         + LiqConcs25.Liq[::SodiumFluoride] + LiqConcs25.Liq[::SodiumCarbonate] + LiqConcs25.Liq[::CausticSoda];
  const double TSodium = InorganicSoda + LiqConcs25.Liq[::SodiumOxalate] + /*LiqConcs25.Liq[::Organics] + */LiqConcs25.Liq[::Organics1];
  return TSodium - InorganicSoda;
  }

//---------------------------------------------------------------------------

double BATCBayerSM::TOStoTOC()
  {
  MArray MA(this);
  LiqConcs25.Converge(MA);
  const double TOC = (/*LiqConcs25.Liq[::Organics]*5.0 + */LiqConcs25.Liq[::SodiumOxalate]*2.0)*MW_C/::SodiumCarbonate.MW
       + LiqConcs25.Liq[::Organics1]*2.21*MW_C/::SodiumCarbonate.MW*2.0;
  if (TOC>1.0e-6)
    {
    const double InorganicSoda = LiqConcs25.Liq[::SodiumSulphate] + LiqConcs25.Liq[::SodiumChloride] 
         + LiqConcs25.Liq[::SodiumFluoride] + LiqConcs25.Liq[::SodiumCarbonate] + LiqConcs25.Liq[::CausticSoda];
    const double TSodium = InorganicSoda + LiqConcs25.Liq[::SodiumOxalate] + /*LiqConcs25.Liq[::Organics] + */LiqConcs25.Liq[::Organics1];
    const double TOS = TSodium - InorganicSoda;
    return TOS/TOC;
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

double BATCBayerSM::FreeCaustic(double T_)
  {
  return CausticConc(T_) * (1.0 - AtoC()*::SodiumCarbonate.MW/::Alumina.MW);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::AtoC()
  {
  LiqConcs25.Converge(MArray(this));
  return LiqConcs25.Liq[::Alumina] / GTZ(LiqConcs25.Liq[::CausticSoda]);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::CtoS()
  {
  LiqConcs25.Converge(MArray(this));
  return LiqConcs25.Liq[::CausticSoda]
         / GTZ(LiqConcs25.Liq[::CausticSoda] + LiqConcs25.Liq[::SodiumCarbonate]);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::CltoC()
  {
  LiqConcs25.Converge(MArray(this));
  return (LiqConcs25.Liq[SodiumChloride])/GTZ(LiqConcs25.Liq[::CausticSoda]);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::Na2SO4toC()
  {
  LiqConcs25.Converge(MArray(this));
  return (LiqConcs25.Liq[SodiumSulphate])/GTZ(LiqConcs25.Liq[::CausticSoda]);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::THAMassFlow()
  {
  MArray M(this);
  return M[::THA];
  }

//---------------------------------------------------------------------------

double BATCBayerSM::THADens(double T_)
  {
  return gs_MVDefn[::THA].Density(T_, StdP, NULL);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::IonicStrength()
  {
  const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989..
  const double MW_Na2O     = ::OccSoda.MW;          //61.9789..
  const double MW_NaCl     = ::SodiumChloride.MW;   //58.4425..
  const double MW_Na2SO4   = ::SodiumSulphate.MW;   //142.043..
  LiqConcs25.Converge(MArray(this));
  double I;
  switch (sm_iASatMethod)
    {
    case ASM_LiqInfo5:
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
    case ASM_BRAHMA: //use LiqInfo6...
    case ASM_LiqInfo6:
      {
      const double k1 = 0.9041;
      const double k2 = 1.0826;
      const double k3 = 1.1589;
      const double k4 = 1.1659;
      const double cTOC    = TOC(C_2_K(25.0));
      const double cNaCl   = LiqConcs25.Liq[::SodiumChloride] /LiqConcs25.NaFactor[::SodiumChloride];
      const double cNa2CO3 = LiqConcs25.Liq[::SodiumCarbonate]/LiqConcs25.NaFactor[::SodiumCarbonate];
      const double cNa2SO4 = LiqConcs25.Liq[::SodiumSulphate] /LiqConcs25.NaFactor[::SodiumSulphate];
      const double cNaOH   = LiqConcs25.Liq[::CausticSoda];
      I = 0.01887*cNaOH + k1*cNaCl/MW_NaCl + k2*cNa2CO3/MW_Na2CO3 + k3*cNa2SO4/MW_Na2SO4 + k4*0.01887*cTOC;
      return I;
      }
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

double BATCBayerSM::AluminaConcSat(double T_)
  {
  T_ = Max(1.0, T_);
  switch (sm_iASatMethod)
    {
    case ASM_LiqInfo5:
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
    case ASM_LiqInfo6:
      {
      const double I = IonicStrength();
      const double I2 = Sqrt(I);
      const double cNaOH   = LiqConcs25.Liq[::CausticSoda];
      const double a0 = -8.5733;
      const double a3 = -0.7370;
      const double a4 = 0.1918;
      const double dG = -30639.0; //Gibbs energy of dissolution
      const double R  = R_c;//8.3145; //Universal Gas Constant 8.314472
      const double X    = a0*I2/(1.0+I2) - a3*I - a4*Pow(I, 3.0/2.0);
      const double Keq  = Exps(dG/(R*T_)); //Equilibrium Constant
      const double ASat = 0.96197*cNaOH/(1.0+(Pow(10.0, X)/Keq));
      return ASat;
      }
    case ASM_BRAHMA:
      {
      #include "brahma_acsat.cpp"
      }
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

double BATCBayerSM::OxalateSolubility(double T_)
  {
  if (0)
    {
    // Function calculates the equilibrium concentration (g/l) of oxalate, based on stream properties.
    // (British Aluminium - Burnt Island)
    const double C        = CausticConc(C_2_K(25.0)); // g/l
    const double Na2CO3Qm = M[SodiumCarbonate]; // kg/s
    const double LiqQv    = LVolume25(); // m^3/s
    const double OxEquil = 7.62*Exps(0.012*K2C(T_) - (::OccSoda.MW/::SodiumCarbonate.MW)*(0.016*C + 0.011*Na2CO3Qm/GTZ(LiqQv)));
    return OxEquil; //(OxEquil+Approach);
    }
  else
    {
/*
Function variables:

t 			= temperature of stream, °C
a 			= A g/L
C 			= C g/L
S 		 	= S g/L
Na2SO4	= Na2SO4 g/L
NaCl	 	= NaCl g/L
TOC  	 	= TOC g/L
Oxal ()		= equilibrium oxalate concentration g/L Na2C2O4

Function Oxal(t, a, C, S, Na2SO4, NaCl, TOC)
' Process Chemistry Oxalate equilibrium relationship based on plant data for LPP filtrate
Delta = 0.5 ' gpl
' "Delta" gpl added to fit seed poisoning/residence time effects away from equilibrium.
On Error GoTo eh_oxalate
Oxal = Delta + 190 * Exp(-1166.4 / (t + 273) + 0.511 * Log(t + 273) + 0.00007 * t ^ 2 - 1.7252 * Log(-0.0171 * a + 0.0482 * C + 0.0248 * (S - C) + 0.0214 * Na2SO4 + 0.054 * NaCl + 0.08 * TOC))
Exit Function
eh_oxalate:
Oxal = 0
Exit Function
End Function
*/
    //Referance: Bayer Process Properties BATC SysCAD Model
    //Process Chemistry Oxalate Equilibrium relationship based on plant data for LPP filtrate
    LiqConcs25.Converge(MArray(this));
    const double Delta = 0.5; //gpl
    //"Delta" gpl added to fit seed poioning/residence time effects away from equilibrium
    const double A = LiqConcs25.Liq[::Alumina];
    const double S = LiqConcs25.Liq[::CausticSoda]+LiqConcs25.Liq[::SodiumCarbonate];
    if (A>1e-5 && S>1e-5)
      {
      const double C = LiqConcs25.Liq[::CausticSoda];
      const double Na2SO4 = LiqConcs25.Liq[::SodiumSulphate]*::SodiumSulphate.MW/::SodiumCarbonate.MW;
      const double NaCl = LiqConcs25.Liq[SodiumChloride]*::SodiumChloride.MW*2/::SodiumCarbonate.MW;
      const double TOC = TOC25();
      const double d1 = -0.0171*A + 0.0482*C + 0.0248*(S-C) + 0.0214*Na2SO4 + 0.054*NaCl + 0.08*TOC;
      if (d1>0.0)
        {
        const double Tc = K2C(T_);
        const double d2 = -1166.4/(Tc+273.0) + 0.511*log(Tc+273.0) + 0.00007*Tc*Tc - 1.7252*log(d1);
        const double OxEquil = Delta + 190.0*exp(d2);
        return OxEquil;
        }
      }
    }
  return 0.0;
  }

//---------------------------------------------------------------------------
/*
TODO: Check this...
Solubilty equations return crazy numbers for dilute liquor. Eg:
Seed_W.Content.Calc.Rqd_A/C	0.748
Seed_W.Content.Calc.Rqd_C/S	0.280
Seed_W.Content.Calc.Rqd_C (g/L)	5.256
Seed_W.Content.Calc.Rqd_Na2SO4 (g/L)	2.815
Seed_W.Content.Calc.Rqd_NaCl (g/L)	0.719
Seed_W.Content.Calc.Rqd_NaF (g/L)	0
Seed_W.Content.Calc.Rqd_Oxalate (g/L)	0.836
Seed_W.Content.Calc.Rqd_TOC (g/L)	1.183
Seed_W.Content.Calc.Rqd_SiO2 (g/L)	0
Seed_W.Content.Calc.Rqd_SolConc (g/L)	*
Seed_W.Content.Calc.Rqd_SolFrac (%)	0
*/

double BATCBayerSM::SulphateSolubility(double T_)
  {
  //Referance: Bayer Process Properties BATC SysCAD Model
  //The sulphate solubility correlation format is based on the paper published in Light Metals 1983 by Eddie Schiller.
  //Constants have been refitted to Worsley liquor by Iwan Hiralal, Billiton.
  
  MArray MA(this);
  double TLiq=MA.Mass(MP_Liq); // Total Liquid kg/s
  if (TLiq<1.0e-6 || MA[SodiumCarbonate]<1.0e-6 || MA[CausticSoda]<1.0e-6  || MA[SodiumSulphate]<1.0e-6)
    return 0.0;

  const double S = (MA[SodiumCarbonate] + MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW);
  const double A = MA[Alumina];
  const double C = MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW;
  const double AtoC = A/NZ(C);
  const double CtoS = C/NZ(S);
  const double Cl = MA[SodiumChloride]/::SodiumChloride.MW*MW_Cl;
  const double CltoS = Cl/NZ(S); //kg per kg S
  const double F = MA[SodiumFluoride]/::SodiumFluoride.MW*MW_F;
  const double FtoS = F/NZ(S); //kg per kg S
  const double SO4 = MA[SodiumSulphate]/::SodiumSulphate.MW*MW_SO4;
  double SO4toS = SO4/NZ(S); //kg per kg S
  const double StoLiq = S*1000.0/TLiq;//g of S per kg liquor
  const double TOC = (/*MA[Organics]*5.0/::Organics.MW + */MA[SodiumOxalate]*2.0/::SodiumOxalate.MW)*MW_C + MA[Organics1]*2.21/::Organics1.MW*MW_C;
  const double TOCtoS = TOC/NZ(S); //kg per kg S
  const double K2OtoS = 0.0;

  //A /= 1000.0;
  //C /= 1000.0;
  double tol = 1.0;
  double Sulptemp = 0.0;
  double Sulptemp2 = 1.0;
  int Iter=100;
  while (tol>0.001 && Iter--)
    {
// S in g/kg; Sulph as g/kg liq Na2SO4
// LOG returns natural to base e
// Sub function calculates molality
// FreeS calculates univalent diionisation of divalent molecules
    double C_ = StoLiq*CtoS*0.001;
    double A_ = C_*AtoC;
    double wOH = 321.0*C_ - 333.0*A_;
    double wAlO2 = 1157.0*A_;
    double wCl = StoLiq*CltoS;
    double wF = StoLiq*FtoS;
    double wCO3 = 0.566*(StoLiq - 1000.0*C_);
    double wSO4 = StoLiq*SO4toS;
    double TOStoS = TOCtoS*2.0;
    //Fact=0.830 for low temp plant, 0.942 for high temp plant
    double Fact = 0.83;//should be 2.28 for WAPL = 2*(89.9-23)/(1.2*106)* TOS/TOC
    double wOrg = StoLiq * TOCtoS * Fact;
    double tsoda = StoLiq + StoLiq*(TOStoS + CltoS*1.495 + FtoS*2.789 + SO4toS*1.104 - K2OtoS*1.125);
    double FreeS = (tsoda/106.0*2.0 - wCO3/60.0 - wSO4/96.0)/2.0*106.0;
    double wNa = 0.434*tsoda;
    double wK = 0.83*StoLiq*K2OtoS;
    double OH = wOH/17.0;
    double AlO2 = wAlO2/59.0;
    double Cl = wCl/35.5;
    double F = wF/19.0;
    double CO3 = wCO3/60.0;
    double SO4 = wSO4/96.0;
    double Org = wOrg/88.0;
    double Na = wNa/23.0;
    double K = wK/39.1;
    double wsum = wOH + wAlO2 + wCl + wF + wCO3 + wSO4 + wOrg + wNa + wK;
    double msum = OH + AlO2 + Cl + F + CO3 + SO4 + Org;
    double W = 1000.0 - wsum;
    double molality1 = msum*1000.0/W;

// Sub function calculates Na2SO4 at equilibrium in moles/kg H2O
// base from LM 1983 Eddie Schiller, Reynold's Metals Company
// Constants refitted to Wapl liquor by I.Hiralal BNL
    const double C1 = 1.2291;
    const double D1 = 1.2993;
    //C1 = 2.6069 ' original
    //D1 = 4.2731 ' original
    //C1 = 1.309449951 ' DJAntrobus Dec 02
    //D1 = 1.567839051 ' DJAntrobus Dec 02
    double d = FreeS/106.0*2.0/(W/1000.0);
    double molSO4 = exp(-C1*log(GTZ(d)) + D1); //weight of Na2SO4 (g / kg H2O)
    double EwSO4 = molSO4 * 142.04;
    Sulptemp = EwSO4*W/1000.0;
    tol = fabs(Sulptemp - Sulptemp2);
    Sulptemp2 = Sulptemp;
    SO4toS = EwSO4/StoLiq;
    }
  if (Iter<=0 && tol>0.01)
    return 0.0; //PROBLEM!!! (todo check/trap for this...)
  return Sulptemp;
  
  /*Function Sulph(S, AtoC, CtoS, CltoS, FtoS, SO4toS, TOCtoS, K2OtoS)
  // calculates empirical fit of Na2SO4 solubility in Worsley leach liquors
' S in g/kg; Sulph as g/kg liq Na2SO4
' LOG returns natural to base e
' Sub function calculates molality
' FreeS calculates univalent diionisation of divalent molecules


On Error GoTo eh_Sulph
tol = 1
Sulptemp = 0
Sulptemp2 = 1
Do Until tol < 0.001
    C = S * CtoS * 0.001
    a = C * AtoC
    wOH = 321 * C - 333 * a
    wAlO2 = 1157 * a
    wCl = S * CltoS
    wF = S * FtoS
    wCO3 = 0.566 * (S - 1000 * C)
    wSO4 = S * SO4toS
    TOStoS = TOCtoS * 2
' Fact=0.830 for low temp plant, 0.942 for high temp plant
    Fact = 0.83 ' should be 2.28 for WAPL = 2*(89.9-23)/(1.2*106)* TOS/TOC
    wOrg = S * TOCtoS * Fact
    tsoda = S + S * (TOStoS + CltoS * 1.495 + FtoS * 2.789 + SO4toS * 1.104 - K2OtoS * 1.125)
    FreeS = (tsoda / 106 * 2 - wCO3 / 60 - wSO4 / 96) / 2 * 106
    wNa = 0.434 * tsoda
    wK = 0.83 * S * K2OtoS
    OH = wOH / 17
    AlO2 = wAlO2 / 59
    Cl = wCl / 35.5
    F = wF / 19
    CO3 = wCO3 / 60
    SO4 = wSO4 / 96
    Org = wOrg / 88
    Na = wNa / 23
    K = wK / 39.1
    wsum = wOH + wAlO2 + wCl + wF + wCO3 + wSO4 + wOrg + wNa + wK
    msum = OH + AlO2 + Cl + F + CO3 + SO4 + Org
    W = 1000 - wsum
    molality1 = msum * 1000 / W

' Sub function calculates Na2SO4 at equilibrium in moles/kg H2O
' base from LM 1983 Eddie Schiller, Reynold's Metals Company
' Constants refitted to Wapl liquor by I.Hiralal BNL
  
  C1 = 1.2291
  D1 = 1.2993
'    C1 = 2.6069 ' original
'    D1 = 4.2731 ' original
'   C1 = 1.309449951 ' DJAntrobus Dec 02
'   D1 = 1.567839051 ' DJAntrobus Dec 02
    molSO4 = Exp(-C1 * Log(FreeS / 106 * 2 / (W / 1000)) + D1)
' weight of Na2SO4 (g / kg H2O)
    EwSO4 = molSO4 * 142.04
    Sulptemp = EwSO4 * W / 1000
    tol = Abs(Sulptemp - Sulptemp2)
    Sulptemp2 = Sulptemp
    SO4toS = EwSO4 / S
Loop
    Sulph = Sulptemp
Exit Function
eh_Sulph:
    Sulph = 0
End Function*/
  }

//---------------------------------------------------------------------------

double BATCBayerSM::CarbonateSolubility(double T_)
  {
  //Referance: Bayer Process Properties BATC SysCAD Model
  // calculates empirical fit of Na2CO3 solubility in Worsley leach liquors
  // S in g/kg; Carb as g/kg liq Na2CO3
  // LOG returns natural to base e
  // Sub function calculates molality
  // FreeS calculates univalent ionisation of molecules
  MArray MA(this);
  double TLiq=MA.Mass(MP_Liq); // Total Liquid kg/s
  if (TLiq<1.0e-6 || MA[SodiumCarbonate]<1.0e-6 || MA[CausticSoda]<1.0e-6)
    return 0.0;

  const double S = (MA[SodiumCarbonate] + MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW);
  const double A = MA[Alumina];
  const double C = MA[CausticSoda]/(2.0*::CausticSoda.MW)*::SodiumCarbonate.MW;
  const double AtoC = A/NZ(C);
  double CtoS = C/NZ(S);
  const double Cl = MA[SodiumChloride]/::SodiumChloride.MW*MW_Cl;
  const double CltoS = Cl/NZ(S); //kg per kg S
  const double F = MA[SodiumFluoride]/::SodiumFluoride.MW*MW_F;
  const double FtoS = F/NZ(S); //kg per kg S
  const double SO4 = MA[SodiumSulphate]/::SodiumSulphate.MW*MW_SO4;
  const double SO4toS = SO4/NZ(S); //kg per kg S
  double StoLiq = S*1000.0/TLiq;//g of S per kg liquor
  const double TOC = (/*MA[Organics]*5.0/::Organics.MW + */MA[SodiumOxalate]*2.0/::SodiumOxalate.MW)*MW_C + MA[Organics1]*2.21/::Organics1.MW*MW_C;
  const double TOCtoS = TOC/NZ(S); //kg per kg S
  const double K2OtoS = 0.0;

  double tol = 1.0;
  double Carbtemp = 0.0;
  double Carbtemp2 = 1.0;
  int Iter=100;
  while (tol>0.001 && Iter--)
    {
    double C_ = StoLiq*CtoS*0.001;
    double A_ = C_*AtoC;
    double wOH = 321.0*C_ - 333.0*A_;
    double wAlO2 = 1157.0*A_;
    double wCl = StoLiq*CltoS;
    double wF = StoLiq*FtoS;
    double wCO3 = 0.566*(StoLiq - 1000.0*C_);
    double wSO4 = StoLiq*SO4toS;
    double TOStoS = TOCtoS*2.0;
    //Fact=0.830 for low temp plant, 0.942 for high temp plant
    double Fact = 0.83;//should be 2.28 for WAPL = 2*(89.9-23)/(1.2*106)* TOS/TOC
    double wOrg = StoLiq * TOCtoS * Fact;
    double tsoda = StoLiq + StoLiq*(TOStoS + CltoS*1.495 + FtoS*2.789 + SO4toS*1.104 - K2OtoS*1.125);
    double FreeS = (tsoda/106.0*2.0 - wCO3/60.0 - wSO4/96.0)/2.0*106.0;
    double wNa = 0.434*tsoda;
    double wK = 0.83*StoLiq*K2OtoS;
    double OH = wOH/17.0;
    double AlO2 = wAlO2/59.0;
    double Cl = wCl/35.5;
    double F = wF/19.0;
    double CO3 = wCO3/60.0;
    double SO4 = wSO4/96.0;
    double Org = wOrg/88.0;
    double Na = wNa/23.0;
    double K = wK/39.1;
    double wsum = wOH + wAlO2 + wCl + wF + wCO3 + wSO4 + wOrg + wNa + wK;
    double msum = OH + AlO2 + Cl + F + CO3 + SO4 + Org;
    double W = 1000.0 - wsum;
    double molality1 = msum*1000.0/W;

    // Sub function calculates Na2CO3 at equilibrium in moles/kg H2O
    // base from LM 1983 Eddie Schiller, Reynold's Metals Company
    //A1 = 2.6642 ' original
    //B1 = 5.3677 ' original
    const double A1 = 1.1061; // fitted
    const double B1 = 1.7608; // fitted
    double d = FreeS/106.0*2.0/(W/1000.0);
    double molCO3 = exp(-A1*log(GTZ(d)) + B1);    //weight of Na2CO3 (g / kg H2O)
    double EwCO3 = molCO3*105.99;
    Carbtemp = EwCO3*W/1000.0;
    tol = fabs(Carbtemp - Carbtemp2);
    Carbtemp2 = Carbtemp;
    CtoS = C_*1000.0/(C_*1000.0 + EwCO3);
    StoLiq = (C_*1000.0 + EwCO3);
    }
  if (Iter<=0 && tol>0.01)
    return 0.0; //PROBLEM!!! (todo check/trap for this...)
  return Carbtemp;
  /*
  The carbonate solubility correlation is based on the paper published in Light Metals 1983 by Eddie Schiller.  Constants have also been refitted to Worsley liquor.

Function Carb(S, AtoC, CtoS, CltoS, FtoS, SO4toS, TOCtoS, K2OtoS)
' calculates empirical fit of Na2CO3 solubility in Worsley leach liquors
' S in g/kg; Carb as g/kg liq Na2CO3
' LOG returns natural to base e
' Sub function calculates molality
' FreeS calculates univalent ionisation of molecules
On Error GoTo eh_Carb
tol = 1
Carbtemp = 0
Carbtemp2 = 1
Do Until tol < 0.001
    C = S * CtoS * 0.001
    a = C * AtoC
    wOH = 321 * C - 333 * a
    wAlO2 = 1157 * a
    wCl = S * CltoS
    wF = S * FtoS
    wCO3 = 0.566 * (S - 1000 * C)
    wSO4 = S * SO4toS
    TOStoS = TOCtoS * 2
' Fact=0.830 for low temp plant, 0.942 for high temp plant
    Fact = 0.83  ' should be 2.28 for WAPL = 2*(89.9-23)/(1.2*106) * TOS/TOC
    wOrg = S * TOCtoS * Fact
    tsoda = S + S * (TOStoS + CltoS * 1.495 + FtoS * 2.789 + SO4toS * 1.104 - K2OtoS * 1.125)
    FreeS = (tsoda / 106 * 2 - wCO3 / 60 - wSO4 / 96) / 2 * 106
    wNa = 0.434 * tsoda
    wK = 0.83 * S * K2OtoS
    OH = wOH / 17
    AlO2 = wAlO2 / 59
    Cl = wCl / 35.5
    F = wF / 19
    CO3 = wCO3 / 60
    SO4 = wSO4 / 96
    Org = wOrg / 88
    Na = wNa / 23
    K = wK / 39.1
    wsum = wOH + wAlO2 + wCl + wF + wCO3 + wSO4 + wOrg + wNa + wK
    msum = OH + AlO2 + Cl + F + CO3 + SO4 + Org
    W = 1000 - wsum
    molality1 = msum * 1000 / W
' Sub function calculates Na2CO3 at equilibrium in moles/kg H2O
' base from LM 1983 Eddie Schiller, Reynold's Metals Company
'    A1 = 2.6642 ' original
'    B1 = 5.3677 ' original
    A1 = 1.1061 ' fitted
    B1 = 1.7608 ' fitted
    molCO3 = Exp(-A1 * Log(FreeS / 106 * 2 / (W / 1000)) + B1)
' weight of Na2CO3 (g / kg H2O)
    EwCO3 = molCO3 * 105.99
    Carbtemp = EwCO3 * W / 1000
    tol = Abs(Carbtemp - Carbtemp2)
    Carbtemp2 = Carbtemp
    CtoS = C * 1000 / (C * 1000 + EwCO3)
    S = (C * 1000 + EwCO3)
Loop
     Carb = Carbtemp
'    Carb = Log(FreeS / 106 * 2 / (W / 1000))
Exit Function
eh_Carb:
    Carb = 0
End Function
*/
  return 0.0;
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double BATCBayerSM::LVolume25()
  {
  const double mt=Mass(MP_Liq);
  return ((mt>=UsableMass) ? (mt / get_Density(MP_Liq, C_2_K(25.0), Pressure, NULL)) : 0.0);
  }

double BATCBayerSM::SLVolume25()
  {
  const double mt=Mass(MP_SL);
  return ((mt>=UsableMass) ? (mt / get_Density(MP_SL, C_2_K(25.0), Pressure, NULL)) : 0.0);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::LDensity25()
  {
  return (get_Density(MP_Liq, C_2_K(25.0), Pressure, NULL));
  }

double BATCBayerSM::SLDensity25()
  {
  return (get_Density(MP_SL, C_2_K(25.0), Pressure, NULL));
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double BATCBayerSM::AtoCSaturation(double T_)
  {
  double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AluminaConcSat(T_) / GTZ(C);
  return ACsat;
  }

//---------------------------------------------------------------------------

double BATCBayerSM::SSNRatio(double T_)
  {// AtoC actual / AtoC saturation ==> A/ASat
  const double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AtoCSaturation(T_);
  return AtoC() / GTZ(ACsat);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::TotalSodiumConc25()
  {//TotalNa Conc @ 25 expressed as Na2CO3
  LiqConcs25.Converge(MArray(this));
  double Sodium = LiqConcs25.Liq[::CausticSoda]       // NaOH
                  + LiqConcs25.Liq[::SodiumCarbonate] // Na2CO3
                  + LiqConcs25.Liq[SodiumOxalate]     // Na2C2O4
                  //+ LiqConcs25.Liq[Organics]          // Na2C5O7
                  + LiqConcs25.Liq[Organics1]         // NaC2.21O2.275H3.63
                  + LiqConcs25.Liq[SodiumChloride]    // NaCl
                  + LiqConcs25.Liq[SodiumSulphate]    // Na2SO4
                  + LiqConcs25.Liq[SodiumFluoride];   // NaF
  return Sodium;
  }

//---------------------------------------------------------------------------

/*double BATCBayerSM::TotalSodium()
  {//TotalNa Conc
  MArray MA(this);
  LiqConcs25.Converge(MA);
  double TSodium = LiqConcs25.LTotalSodium(MA); // The sodium components in liq.
  return TSodium;
  }*/

//---------------------------------------------------------------------------

double BATCBayerSM::Na2CO3toS()
  {
  LiqConcs25.Converge(MArray(this));
  return LiqConcs25.Liq[::SodiumCarbonate]
         / GTZ(LiqConcs25.Liq[::CausticSoda] + LiqConcs25.Liq[::SodiumCarbonate]);
  }

//---------------------------------------------------------------------------

double BATCBayerSM::OrganateConc25()
  {//Organic Na2C5O7 + NaOrg Conc @ 25
  LiqConcs25.Converge(MArray(this));
  double Organate = /*LiqConcs25.Liq[::Organics] + */LiqConcs25.Liq[::Organics1];
  return Organate;
  }

//---------------------------------------------------------------------------

double BATCBayerSM::OxalateConc25()
  {
  LiqConcs25.Converge(MArray(this));
  double Oxalate = LiqConcs25.Liq[::SodiumOxalate];
  return Oxalate;
  }

//---------------------------------------------------------------------------

double BATCBayerSM::TotalOrganicsConc25()
  {
  LiqConcs25.Converge(MArray(this));
  double Organics = /*LiqConcs25.Liq[::Organics] + */LiqConcs25.Liq[::Organics1] + LiqConcs25.Liq[::SodiumOxalate];
  return Organics;
  }
//---------------------------------------------------------------------------

double BATCBayerSM::ChlorineConc25()
  {
  LiqConcs25.Converge(MArray(this));
  double Chlorine = LiqConcs25.Liq[::SodiumChloride];
  return Chlorine;
  }

//---------------------------------------------------------------------------

double BATCBayerSM::SulphateConc25()
  {
  LiqConcs25.Converge(MArray(this));
  double Sulphate = LiqConcs25.Liq[::SodiumSulphate];
  return Sulphate;
  }

//---------------------------------------------------------------------------

double BATCBayerSM::FluorideConc25()
  {
  LiqConcs25.Converge(MArray(this));
  double Fluoride = LiqConcs25.Liq[::SodiumFluoride];
  return Fluoride;
  }

//---------------------------------------------------------------------------





