//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include <stdafx.h>
#define  __ALCANBAYERSPMDL_CPP
#include "sp_db.h"
#include "AlcanBayerSpMdl.h"
#include "errorlog.h"
//#include "optoff.h"

// ==========================================================================

#define dbgModels WITHDEBUG
#if dbgModels                                                                  
#include "dbgmngr.h"
static CDbgMngr dbgSpecies    ("AlcanSpMdl", "Species");
static CDbgMngr dbgPerformTHA ("AlcanSpMdl", "PerformTHA");
static CDbgMngr dbgSetSizeData("AlcanSpMdl", "SetSizeData");
#endif

// ==========================================================================
//
// Alcan Bayer Specie Model.
//
// ==========================================================================

static MInitialiseTest InitTest(&ASMBayerClass);
// Specie Access 
SPECIEBLK_L(InitTest, Water,             "H2O(l)",        false);
SPECIEBLK_L(InitTest, Alumina,           "Al2O3(l)",      false);
SPECIEBLK_L(InitTest, CausticSoda,       "NaOH(l)",       false);
SPECIEBLK_L(InitTest, SodiumChloride,    "NaCl(l)",       false);
SPECIEBLK_L(InitTest, SodiumSulphate,    "Na2SO4(l)",     false);
SPECIEBLK_L(InitTest, SodiumCarbonate,   "Na2CO3(l)",     false);
SPECIEBLK_L(InitTest, SodiumFluoride,    "NaF(l)",        false);
SPECIEBLK_L(InitTest, SodiumOxalate,     "Na2C2O4(l)",    false); //organic
SPECIEBLK_L(InitTest, Organics,          "Na2C5O7(l)",    false); //organic
SPECIEBLK_L(InitTest, Organics1,         "Na2C5O11(l)",   false); //organic
SPECIEBLK_L(InitTest, SilicateLiq,       "SiO2(l)",       false);
SPECIEBLK_L(InitTest, DiphosphorusLiq,   "P2O5(l)",       false);

SPECIEBLK_S(InitTest, OccSoda,           "Na2O(s)",       false);
SPECIEBLK_S(InitTest, THA,               "Al2O3.3H2O(s)", false);
SPECIEBLK_S(InitTest, SodiumOxalateSol,  "Na2C2O4(s)",    false);

SPECIEBLK_V(InitTest, Steam,             "H2O(g)",        true);

const double H2OTestFrac = 0.98;
const double MW_C        = 12.011;

// ==========================================================================
// Specie Modal Global Constants

enum DLM_DefineLiquorMethod { DLM_TotOrganics, DLM_TOC };
enum BPM_BPEMethod { BPM_Dewey, BPM_Adamson };
enum ASM_AlSolubilityMethod { ASM_Original, ASM_March2002, ASM_May2003, ASM_Hyprod };
enum BDM_BayerDensityMethod { BDM_Original, BDM_March2002, BDM_July2003, BDM_MD };
enum GRM_GrowthRateMethod { GRM_Original, GRM_Nov2003, GRM_Hyprod };
enum CPM_CpMethod { CPM_Original, CPM_Hyprod };

double ASMBayer::dBPEFactor      = 1.0;
double ASMBayer::dRhoFactor      = 1.0;
double ASMBayer::sm_dMinSolCp    = 1.046;
byte ASMBayer::sm_iRqdCalcMethod = DLM_TOC;
byte ASMBayer::sm_iASatMethod    = ASM_March2002;
byte ASMBayer::sm_iDensityMethod = BDM_March2002;
byte ASMBayer::sm_iGrowthMethod  = GRM_Nov2003;
byte ASMBayer::sm_iCpMethod      = CPM_Original;
byte ASMBayer::sm_iBPEMethod     = BPM_Dewey;
double ASMBayer::sm_Morphology   = 1.0;
double ASMBayer::sm_Nucleation   = 0.01;
double ASMBayer::sm_Na2OFactor   = 1.0;
double ASMBayer::sm_CarbonFactor = 2.0;

// ==========================================================================
//
// A special class to assist with calculating iterative concentration and density
// calculations.
//
// ==========================================================================

flag     CConcentrations::NaFactorOK=0;
SpVector CConcentrations::NaFactor(true);

CConcentrations::CConcentrations(ASMBayer *pMdl) 
  { 
  pBayerMdl = pMdl;
  Liq.SetSize(SDB.Count());
  Zero(); 

  if (!NaFactorOK)
    { 
    for (int sn=0; sn<SDB.Count(); sn++)
      NaFactor.VValue[sn]=1.0; //For all the species without sodium ions
    NaFactor.VValue[::CausticSoda.LiqPhInx()]     = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::CausticSoda.MW);     //NaOH       39.9971
    NaFactor.VValue[::SodiumCarbonate.LiqPhInx()] = 1.0;                                                       //Na2CO3     105.989
    NaFactor.VValue[::SodiumOxalate.LiqPhInx()]   = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumOxalate.MW);   //Na2C2O4    133.999
    NaFactor.VValue[::SodiumChloride.LiqPhInx()]  = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumChloride.MW);  //NaCl       58.4425
    NaFactor.VValue[::SodiumSulphate.LiqPhInx()]  = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumSulphate.MW);  //Na2SO4     142.043
    NaFactor.VValue[::SodiumFluoride.LiqPhInx()]  = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumFluoride.MW);  //NaF
    //NaFactor.VValue[::SodiumSilicate.LiqPhInx()]  = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumSilicate.MW);
    NaFactor.VValue[::Organics.LiqPhInx()]        = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::Organics.MW);        //Na2C5O7    218.030
    NaFactor.VValue[::Organics1.LiqPhInx()]       = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::Organics1.MW);       //Na2C5O11   282.028

    NaFactorOK = true; 
    }
  }

// --------------------------------------------------------------------------

void CConcentrations::Zero()
  {
  Density25 = 1100.0;
  for (int sn=0; sn<SDB.Count(); sn++)
    Liq[sn] = 0.0;
  }

// --------------------------------------------------------------------------

flag CConcentrations::Converge(CSysVector & MA)
  {
  double TLiq = MA.Sum(som_Liq);
  if (TLiq<1.0e-9)
    {
    Zero();
    return true;
    }

  static CToleranceBlock s_Tol(TBF_Both, "AlcanSpMdl:Density", 1e-12, 1e-8, 100);

  const double Tc = 25.0;
  const double T_ = C2K(Tc);
  // Converge Liquor Conc. All sodium concentrations expressed as Na2CO3
  int IterCount = s_Tol.GetMaxIters();//100;
  double OldDensity = Density25;
  while (1)
    {
    for (int sn=0; sn<SDB.Count(); sn++)
      {
      if (SDB[sn].IsLiq())
        Liq[sn] = MA[sn] / TLiq * Density25 * NaFactor[sn];
        }
    Density25 = Range(0.0001, LiquorDensity(T_, MA), 10000.0);
    if (fabs(OldDensity - Density25) < s_Tol.GetAbs() || --IterCount==0)
      break;
    OldDensity = Density25;
    } // end of while

  Density25 = Max(0.001, Density25);
  //pBayerMdl->SetCI((IterCount==0), 1, True, True);

  #if dbgModels
  if (dbgSpecies ())
    {
    dbgpln("Concs           Tc:%10.2f",Tc);
    dbgpln("           Density:%10.2f",Density25);
    for (int sn=0; sn<SDB.Count(); sn++)
      if (SDB[sn].IsLiq())
      dbgpln("%18.18s:%12.6f  MW:%11.5f NaFactor:%9.5f",SDB[sn].SymOrTag(),Liq[sn],SDB[sn].MoleWt(), NaFactor[sn]);
    }
  #endif
  return (IterCount>0);
  }

//---------------------------------------------------------------------------

double CConcentrations::LTotalSodium(CSysVector & MA)
  {
  //Expressed as Na2CO3
  double TSodium = 
         ( MA[CausticSoda.LiqPhInx()]    * NaFactor[::CausticSoda.LiqPhInx()]
         + MA[SodiumCarbonate.LiqPhInx()]
         + MA[SodiumOxalate.LiqPhInx()]  * NaFactor[::SodiumOxalate.LiqPhInx()]
         + MA[SodiumChloride.LiqPhInx()] * NaFactor[::SodiumChloride.LiqPhInx()]
         + MA[SodiumSulphate.LiqPhInx()] * NaFactor[::SodiumSulphate.LiqPhInx()]
         + MA[SodiumFluoride.LiqPhInx()] * NaFactor[::SodiumFluoride.LiqPhInx()]
         + MA[Organics.LiqPhInx()]       * NaFactor[::Organics.LiqPhInx()]
         + MA[Organics1.LiqPhInx()]      * NaFactor[::Organics1.LiqPhInx()] );
         //+ MA[SodiumSilicate.LiqPhInx()] * NaFactor[::SodiumSilicate.LiqPhInx()] );
  return TSodium;
  }

// --------------------------------------------------------------------------

double CConcentrations::LiquorDensEqn1(double Tc, double WTNa, double WAlumina)
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

double CConcentrations::LiquorDensEqn2(double Tc, double XNaC, double XNa, double XAl)
  {
  return (1.0+1.71*XNaC) * (1006.0+(1200.0+872.0*XNa-0.291*Tc)*XNa
                   +(1060.0-0.664*Tc)*XAl-(0.242+0.00238*Tc)*Tc);
  }

// --------------------------------------------------------------------------

double CConcentrations::LiquorDensEqn3(double Tc, double XAl2O3, double XCaust, double XTTS, double XOC)
  {
  double RHO = 1.02767 - 0.0007015008*Tc - 0.0000007140195*Tc*Tc + 
               0.0003631629*XAl2O3 + 0.0000003396664*XAl2O3*XAl2O3 - 
               0.00005931031*XCaust + 
               0.000794539*XTTS - 0.0000002692805*XTTS*XTTS + 
               0.002644698*XOC + 0.0000003134875*Tc*XCaust + 0.04716374*XAl2O3/GTZ(XCaust); // Density at T
  return (RHO * 1000.0); // kg/m^3
  }

// --------------------------------------------------------------------------

double CConcentrations::LiquorDensity(double T_, CSysVector & MA)
  {
  /*Liquor Density with mass fractions*/
  double TLiq=MA.Sum(som_Liq); //Total Liquid
  if (TLiq<1.0e-9)
    return 0.0;

  double Tc = K2C(T_);
  switch (ASMBayer::sm_iDensityMethod)
    {
    case BDM_Original:
      {
      // Version independent of caustic concentration
      // Calculates density in gm/cc
      // By J.W.Mulloy
      double TNa=LTotalSodium(MA);        // Total Sodium Compounds as Na2CO3 kg/s
      double Alumina=MA[::Alumina.LiqPhInx()];  // Alumina kg/s
      double WTNa=TNa*100.0/TLiq;         // Wgt % of TNa
      double WAlumina=Alumina*100.0/TLiq; // Wgt % of Alumina
      double RHO = LiquorDensEqn1(Tc, WTNa, WAlumina);

      #if dbgModels
      if (dbgSpecies ())
        dbgpln("Liq Dens Tc:%10.2f WTNa:%12.6g WAlumina:%12.6g Rho:%12.6g",Tc,WTNa,WAlumina,RHO);
      #endif

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
      double XAl   = MA[::Alumina.LiqPhInx()]/TLiq; //fraction of Al
      double TNa   = LTotalSodium(MA)*MW_Na2O/MW_Na2CO3;
      double TCaus = MA[::CausticSoda.LiqPhInx()]*NaFactor[::CausticSoda.LiqPhInx()]*MW_Na2O/MW_Na2CO3;
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

      if (0)
        {
        //as mass fractions...
        //NB: This is invalid for full range of T !!!
        double Al2O3_f   = MA[::Alumina.LiqPhInx()]/TLiq;
        double NaCl_f    = MA[::SodiumChloride.LiqPhInx()]/TLiq;
        double Na2CO3_f  = MA[::SodiumCarbonate.LiqPhInx()]/TLiq;
        double Na2SO4_f  = MA[::SodiumSulphate.LiqPhInx()]/TLiq;
        double Na2C2O4_f = MA[::SodiumOxalate.LiqPhInx()]/TLiq;
        double NaTT_f    = MA[::CausticSoda.LiqPhInx()]/TLiq*MW_Na2O/(MW_NaOH*2);
        double Caust_f   = NaTT_f*MW_Na2CO3/MW_Na2O;
        double TTS_f     = Na2CO3_f + Caust_f + NaCl_f*MW_Na2CO3/(MW_NaCl*2) + Na2SO4_f*MW_Na2CO3/MW_Na2SO4;
        double OrgC_f    = (MA[::Organics.LiqPhInx()]/TLiq*5.0)*MW_C/MW_Na2C5O7 + (MA[::Organics1.LiqPhInx()]/TLiq*5.0)*MW_C/MW_Na2C5O11;
        double OC_f      = OrgC_f + Na2C2O4_f*(MW_C*2)/MW_Na2C2O4;
        double RHO_ = LiquorDensEqn3(Tc, Al2O3_f*1000, Caust_f*1000, TTS_f*1000, OC_f*1000);

        double xNa2O = MA[::CausticSoda.LiqPhInx()]/TLiq * MW_Na2CO3/(2.0*MW_NaOH);
        double xAl   = MA[::Alumina.LiqPhInx()]/TLiq;
        double xNaC  = MA[::SodiumCarbonate.LiqPhInx()]/TLiq;
        double xOrgC = (MA[::Organics.LiqPhInx()]/TLiq*5.0)*MW_C/MW_Na2C5O7 + (MA[::Organics1.LiqPhInx()]/TLiq*5.0)*MW_C/MW_Na2C5O11 + MA[::SodiumOxalate.LiqPhInx()]/TLiq*(MW_C*2)/MW_Na2C2O4;
        double RHO_x = 1.002545 + 0.831696*xAl + 1.544015*xNaC + 1.44104*xNa2O + 
                       3.220858*xAl*xAl + 28.1982*xOrgC*xOrgC - 9.186E-6*Tc*Tc - 
                       127.655*xOrgC*xOrgC*xOrgC + 4.88E-08*Tc*Tc*Tc - 
                       8.41345*xAl*xOrgC - 14.9966*xNaC*xOrgC; 
        RHO_x *= 1000.0;
        }

      //as concentrations...
      double Al2O3_c   = Liq[::Alumina.LiqPhInx()];
      double NaCl_c    = Liq[::SodiumChloride.LiqPhInx()];
      double Na2CO3_c  = Liq[::SodiumCarbonate.LiqPhInx()];
      double Na2SO4_c  = Liq[::SodiumSulphate.LiqPhInx()];
      //double NaTT_c    = Liq[::CausticSoda.LiqPhInx()]*MW_Na2O/MW_Na2CO3;
      //double Caust_c   = NaTT_c*MW_Na2CO3/MW_Na2O;
      double Caust_c   = Liq[::CausticSoda.LiqPhInx()];
      double TTS_c     = Na2CO3_c + Caust_c + NaCl_c + Na2SO4_c;
      double OC_c      = (Liq[::Organics.LiqPhInx()]*5.0 + Liq[::SodiumOxalate.LiqPhInx()]*2.0 + Liq[::Organics1.LiqPhInx()]*5.0)*MW_C/MW_Na2CO3; //ie TOC
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
      double XAl   = MA[::Alumina.LiqPhInx()]/TLiq; //fraction of Al
      double TNa   = LTotalSodium(MA)*MW_Na2O/MW_Na2CO3;
      double TCaus = MA[::CausticSoda.LiqPhInx()];//THIS IS THE CHANGE
      double XNaC  = (TNa-TCaus)/TLiq; //fraction of (TotalNa less Caustic)
      double XNa   = TNa/TLiq; //fraction of TotalNa
      double RHO = LiquorDensEqn2(Tc, XNaC, XNa, XAl);
      return RHO;
      }
    }
  return 1000.0;
  }

// ==========================================================================
//
//                      Alcan Bayer Liquor Specie Model
//
// ==========================================================================
  
// must be in Range 0x0001 to 0x8000
const dword SMVF_LiqConcOK   = 0x00000001;

const word xidSlurryAluminaConc25 = 30006;
const word xidSlurryCausticConc25 = 30007;
const word xidSlurrySodaConc25    = 30008;
const word xidSlurryAtoC25        = 30009;
const word xidSlurryCtoS25        = 30010;
const word xidSlurryCltoC25       = 30011;
const word xidSlurrySodiumCarbConc25 = 30012;
const word xidSlurryFreeCaustic25 = 30013;
const word xidSlurrySolidsConc25  = 30014;
const word xidLiquidVolume25      = 30015;
const word xidSlurryVolume25      = 30016;
const word xidSlurryRho25         = 30017;
const word xidLiquidRho25         = 30018;
const word xidSlurryVapPress      = 30019;

const word xidOrganateConc25      = 30020;
const word xidOxalateConc25       = 30021;
const word xidTotalOrgConc25      = 30022;
const word xidSlurryTOOC25        = 30023;
const word xidSlurryTOC25         = 30024;
const word xidNaClConc25          = 30025;
const word xidNa2SO4Conc25        = 30026;
const word xidTotalNaConc25       = 30027;
const word xidOrganicO11Conc25    = 30028;
const word xidSlurryAluminaConc   = 30029;
const word xidSlurryCausticConc   = 30030;
const word xidSlurrySodaConc      = 30031;
const word xidSlurryTOC           = 30032;
const word xidSlurrySolidsConc    = 30033;
const word xidASaturation         = 30034;
const word xidAtoCSaturation      = 30035;
const word xidSSNRatio            = 30036;
const word xidAluminaSSN          = 30037;
const word xidSlurryMR            = 30038;
const word xidSlurryAA            = 30039;
const word xidSlurryTT            = 30040;
const word xidSlurryNa2CO3toS25   = 30041;
const word xidOxalate_Conc25      = 30042;
const word xidOrganicO11_Conc25   = 30043;
const word xidNaCl_Conc25         = 30044;
const word xidNa2SO4_Conc25       = 30045;
const word xidNaF_Conc25          = 30046;
const word xidSilicateConc25      = 30047;
const word xidSlurryTOC25_        = 30048;
const word xidRqdTOC              = 30049;
const word xidRqdOrgRatio         = 30050;
const word xidRqdOxalate          = 30051;
const word xidIonicStrength       = 30052;
const word xidGrowthRate          = 30053;
const word xidBayerBPE            = 30054;
const word xidOxalateSolubility   = 30055;
const word xidBoehmiteSolubility  = 30056;
const word xidNa2OSolFrac         = 30057;
const word xidSPOFrac             = 30058;

const word xidSetSzSpecs          = 30999;

IMPLEMENT_SPMODEL(ASMBayer, "ABayer", "", TOC_ALL|TOC_GRP_ALUMINA|TOC_ALCAN, "Alcan Bayer", "Alcan Bayer Model");
IMPLEMENT_SPARES(ASMBayer, 100);

CArray <int, int> ASMBayer::LclParm;

ASMBayer::ASMBayer(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach):
  SMBayerBase(pClass_, Tag_, pAttach, eAttach),
  //LiqConcs(this),
  LiqConcs25(this)
  {
  fDoCalc       = false;
  fWasDoCalc    = true;

  dRqd_AtoC     = 0.4;
  dRqd_C        = 220.0;
  dRqd_CtoS     = 0.85;
  dRqd_Sul      = 2.0;//20.0;
  dRqd_Sil      = 0.5;//1.0;
  dRqd_Org      = 25.0;//50.0;
  dRqd_OrgRatio = 0.14;//0.9;
  dRqd_Salt     = 5.0;//15.0;
  dRqd_Fl       = 0.5;//5.0;
  dRqd_P2O5     = 0.5;//4.0;
  dRqd_Ox       = 3.5;
  dRqd_TOC      = 13.0;
  dRqd_SolFrac  = 0.0;
  dRqd_SolConc  = dNAN;
  dRqd_SolOx    = 0.0;
  dRqd_SolOccSoda = 0.003;
  }

// --------------------------------------------------------------------------

ASMBayer::~ASMBayer()
  {
  }

// --------------------------------------------------------------------------

DDEF_Flags ASMBayer::FixSpEditFlags(DDEF_Flags Flags, int iSpid, flag AsParms)
  {
  if (fDoCalc && LclParm.GetSize()!=SDB.Count())
    {
    LclParm.SetSize(SDB.Count());
    for (int i=0; i<SDB.Count(); i++)
      LclParm[i]=true;

    LclParm[CausticSoda.LiqPhInx()] = false;
    LclParm[SilicateLiq.LiqPhInx()] = false;
    LclParm[SodiumCarbonate.LiqPhInx()] = false;
    LclParm[SodiumChloride.LiqPhInx()] = false;
    LclParm[SodiumSulphate.LiqPhInx()] = false;
    LclParm[SodiumOxalate.LiqPhInx()] = false;
    LclParm[SodiumFluoride.LiqPhInx()] = false;
    LclParm[Alumina.LiqPhInx()] = false;
    LclParm[Organics.LiqPhInx()] = false;
    LclParm[Organics1.LiqPhInx()] = false;
    LclParm[Water.LiqPhInx()] = false;
    LclParm[DiphosphorusLiq.LiqPhInx()] = false;

    LclParm[OccSoda.SolPhInx()] = false;
    LclParm[SodiumOxalateSol.SolPhInx()] = false;
    }
 
  if (fDoCalc || (fDoCalc!=fWasDoCalc))
    {
    for (int i=0; i<LclParm.GetCount(); i++)
      if (!LclParm[i])
        SetMassInUse(i, fDoCalc);
    if (dRqd_SolFrac>0.0)
      {
      for (int i=0; i<SDB.Count(); i++)
        if (SDB[i].IsSol())
          SetMassInUse(i, fDoCalc);
      }
    }
  fWasDoCalc=fDoCalc;

  return (Flags&~(isParm|isResult)) | ((AsParms && (!fDoCalc || LclParm[iSpid])) ? DDEF_PARAM : DDEF_RESULT); 
  }

// --------------------------------------------------------------------------

void ASMBayer::BuildDataDefn_Vars(DataDefnBlk & DDB)
  {
  static DDBValueLst DDB0[]={
    {BPM_Dewey, "Dewey"},
    {BPM_Adamson, "Adamson"},
    {0}};
  static DDBValueLst DDB1[]={
    {DLM_TotOrganics, "TotOrganics and Ratio"},
    {DLM_TOC, "TOC and Oxalate"},
    {0}};
  static DDBValueLst DDB2[]={
    {ASM_Original, "Original"},
    {ASM_March2002, "March2002"},
    {ASM_May2003, "May2003"},
    {ASM_Hyprod, "Hyprod"},
    {0}};
  static DDBValueLst DDB3[]={
    {BDM_Original, "Original"},
    {BDM_March2002, "March2002"},
    {BDM_July2003, "July2003"},
    {BDM_MD, "MD"},
    {0}};
  static DDBValueLst DDB4[]={
    {GRM_Original, "Original"},
    {GRM_Nov2003, "Nov2003"},
    {GRM_Hyprod, "Hyprod"},
    {0}};
  static DDBValueLst DDB5[]={
    {CPM_Original, "Original"},
    {CPM_Hyprod, "Hyprod"},
    {0}};

  if (UseAsMass() || UseAsImage())
    {
    DDB.Text("Global Bayer Constants and Options...");
    DDB.Byte    ("BPE_Method",      "",   DC_,      "",        &sm_iBPEMethod,   this,  isParm, DDB0); //global constant
    DDB.Byte    ("DensityMethod",   "",   DC_,      "",        &sm_iDensityMethod, this, isParm, DDB3); //global constant
    DDB.Byte    ("ASatMethod",      "",   DC_,      "",        &sm_iASatMethod,  this,  isParm, DDB2); //global constant
    DDB.Byte    ("GrowthMethod",    "",   DC_,      "",        &sm_iGrowthMethod, this, isParm, DDB4); //global constant
    DDB.Byte    ("CpMethod",        "",   DC_,      "",        &sm_iCpMethod,    this,  isParm, DDB5); //global constant
    DDB.Double  ("BPE_Factor",      "",   DC_Frac,  "%",       &dBPEFactor,      this,  isParm|InitHidden); //global constant
    DDB.Double  ("Rho_Factor",      "",   DC_Frac,  "%",       &dRhoFactor,      this,  isParm|InitHidden); //global constant
    DDB.Double  ("MinSolidsCp",     "",   DC_CpMs,  "kJ/kg.C", &sm_dMinSolCp,    this,  isParm|InitHidden); //global constant
    DDB.Double  ("Morphology",      "",   DC_,      "",        &sm_Morphology,   this,  isParm); //global constant
    DDB.Double  ("Nucleation",      "",   DC_,      "",        &sm_Nucleation,   this,  isParm); //global constant
    DDB.Double  ("Na2OFactor",      "",   DC_,      "",        &sm_Na2OFactor,   this,  isParm); //global constant
    DDB.Visibility(NSHM_All, sm_iASatMethod==ASM_May2003 || sm_iASatMethod==ASM_Hyprod);                          
    DDB.Double  ("CarbonFactor",    "",   DC_,      "",        &sm_CarbonFactor, this,  isParm); //global constant
    DDB.Visibility();                                                                               

    DDB.Text("Bayer");
    DDB.CheckBoxBtn("DefineLiquor", "",   DC_,      "",        &fDoCalc,         this,  isParm);
    if (PrjFileVerNo()<79)
      DDB.Byte    ("DefnLiqMethod",   "",   DC_,      "",        &sm_iRqdCalcMethod, this,  isParm|SetOnChange, DDB1); //global constant
    DDB.Visibility(NSHM_All, fDoCalc);                          
    DDB.Double  ("Rqd_A/C",         "",   DC_,      "",        &dRqd_AtoC,       this,  isParm);
    DDB.Double  ("Rqd_C/S",         "",   DC_,      "",        &dRqd_CtoS,       this,  isParm);
    DDB.Double  ("Rqd_C",           "",   DC_Conc,  "g/L",     &dRqd_C,          this,  isParm);
    if (PrjFileVerNo()<79)
      {//old
      DDB.Double  ("Rqd_Organic",     "",   DC_Conc,  "g/L",     &dRqd_Org,        this,  sm_iRqdCalcMethod==DLM_TOC ? isResult : isParm);
      DDB.Double  ("Rqd_TOC",         "",   DC_Conc,  "g/L",     xidRqdTOC,        this,  (sm_iRqdCalcMethod==DLM_TOC ? isParm : isResult)|noFile|noSnap);
      DDB.Double  ("Rqd_OrgRatio",    "",   DC_Frac,  "%",       xidRqdOrgRatio,   this,  sm_iRqdCalcMethod==DLM_TOC ? isResult : isParm);
      DDB.Double  ("Rqd_Oxalate",     "",   DC_Conc,  "g/L",     xidRqdOxalate,    this,  (sm_iRqdCalcMethod==DLM_TOC ? isParm : isResult)|noFile|noSnap);
      }
    else
      {
      DDB.Double  ("Rqd_Oxalate",    "",   DC_Conc,  "g/L",     &dRqd_Ox,         this,  isParm);
      DDB.Double  ("Rqd_TOC",        "",   DC_Conc,  "g/L",     &dRqd_TOC,        this,  isParm);
      }
    DDB.Double  ("Rqd_Na2SO4",      "",   DC_Conc,  "g/L",     &dRqd_Sul,        this,  isParm);
    DDB.Double  ("Rqd_NaCl",        "",   DC_Conc,  "g/L",     &dRqd_Salt,       this,  isParm);
    DDB.Double  ("Rqd_NaF",         "",   DC_Conc,  "g/L",     &dRqd_Fl,         this,  isParm);
    DDB.Double  ("Rqd_P2O5",        "",   DC_Conc,  "g/L",     &dRqd_P2O5,       this,  isParm);
    DDB.Double  ("Rqd_SiO2",        "",   DC_Conc,  "g/L",     &dRqd_Sil,        this,  isParm);
    DDB.Double  ("Rqd_SolConc",     "",   DC_Conc,  "g/L",     &dRqd_SolConc,    this,  isParm|NAN_OK);
    if (PrjFileVerNo()>=66)
      DDB.Double  ("Rqd_SolFrac",   "",   DC_Frac,  "%",       &dRqd_SolFrac,    this,  Valid(dRqd_SolConc)?0:isParm);
    else
      DDB.Double  ("SolidsFrac",    "",   DC_Frac,  "%",       &dRqd_SolFrac,    this,  Valid(dRqd_SolConc)?0:isParm);
    DDB.Visibility(dRqd_SolFrac>0.0);                                                                               
    DDB.Double  ("Rqd_OccSoda",     "",   DC_Frac,  "%",       &dRqd_SolOccSoda, this,  isParm);
    DDB.Double  ("Rqd_SPO",         "",   DC_Frac,  "%",       &dRqd_SolOx,      this,  isParm);
    DDB.Visibility();                                                                               
    DDB.Text("");
    }

  if (1)
    {
    SpModelEx::BuildDataDefn_Vars(DDB);

    Strng Tg(DDB.BlockTag());
    Tg+="..";
    DDB.Page(Tg(), DDB_OptPage);
    DDB.Text("Bayer Liquor Values @ 25");
    DDB.Double("AluminaConc",   "A",  DC_Conc,  "g/L",     xidSlurryAluminaConc25, this,  isResult|noFile|noSnap);
    DDB.Double("CausticConc",   "C",  DC_Conc,  "g/L",     xidSlurryCausticConc25, this,  isResult|noFile|noSnap);
    DDB.Double("SodaConc",      "S",  DC_Conc,  "g/L",     xidSlurrySodaConc25,    this,  isResult|noFile|noSnap);
    DDB.Double("A/C",           "",   DC_,      "",        xidSlurryAtoC25,        this,  isResult|noFile|noSnap);
    DDB.Double("C/S",           "",   DC_,      "",        xidSlurryCtoS25,        this,  isResult|noFile|noSnap);
    DDB.Double("TOC",           "",   DC_Conc,  "g/L",     xidSlurryTOC25,         this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("MR",            "",   DC_,      "",        xidSlurryMR,            this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("AA",            "",   DC_Conc,  "g/L",     xidSlurryAA,            this,  isResult|noFile|noSnap|InitHidden); //C as Na2O equivilant
    DDB.Double("TT",            "",   DC_Conc,  "g/L",     xidSlurryTT,            this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("Cl/C",          "",   DC_,      "",        xidSlurryCltoC25,       this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("Na2CO3/S",      "",   DC_,      "",        xidSlurryNa2CO3toS25,   this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("SodiumCarbConc","",   DC_Conc,  "g/L",     xidSlurrySodiumCarbConc25,this,isResult|noFile|noSnap|InitHidden);
    DDB.Double("FreeCaustic",   "FC", DC_Conc,  "g/L",     xidSlurryFreeCaustic25, this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("SolidsConc",    "",   DC_Conc,  "g/L",     xidSlurrySolidsConc25,  this,  isResult|noFile|noSnap);
    DDB.Double("OccSoda",       "",   DC_Frac,  "%",       xidNa2OSolFrac,         this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("SPO",           "",   DC_Frac,  "%",       xidSPOFrac,             this,  isResult|noFile|noSnap|InitHidden);

    DDB.Text("");
    DDB.Text("Concentrations @ 25, as Na2CO3 equivalent");
    DDB.Double("OrganicO11",    "",   DC_Conc,  "g/L",     xidOrganicO11Conc25,    this,  isResult|noFile|noSnap);
    DDB.Double("Organates",     "",   DC_Conc,  "g/L",     xidOrganateConc25,      this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("Oxalate",       "",   DC_Conc,  "g/L",     xidOxalateConc25,       this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("TotalOrg",      "",   DC_Conc,  "g/L",     xidTotalOrgConc25,      this,  isResult|noFile|noSnap);
    //DDB.Double("TOOC",          "",   DC_Conc,  "g/L",     xidSlurryTOOC25,        this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("NaCl",          "",   DC_Conc,  "g/L",     xidNaClConc25,          this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("Na2SO4",        "",   DC_Conc,  "g/L",     xidNa2SO4Conc25,        this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("TotalNa",       "",   DC_Conc,  "g/L",     xidTotalNaConc25,       this,  isResult|noFile|noSnap);
    
    DDB.Text("");
    DDB.Text("Bayer Liquor Values @ 25");
    DDB.Double("LVolFlow25",    "",   DC_Qv,    "L/s",     xidLiquidVolume25,      this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("SLVolFlow25",   "",   DC_Qv,    "L/s",     xidSlurryVolume25,      this,  isResult|noFile|noSnap);
    DDB.Double("LRho25",        "",   DC_Rho,   "kg/m^3",  xidLiquidRho25,         this,  isResult|noFile|noSnap);
    DDB.Double("SLRho25",       "",   DC_Rho,   "kg/m^3",  xidSlurryRho25,         this,  isResult|noFile|noSnap);
    //DDB.Double("VapPress",     "Vp",  DC_P,     "kPa",     xidSlurryVapPress,      this,  isResult|noFile|noSnap);
    DDB.Double("OrganicO11*",   "",   DC_Conc,  "g/L",     xidOrganicO11_Conc25,   this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("Oxalate*",      "",   DC_Conc,  "g/L",     xidOxalate_Conc25,      this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("TOC*",          "",   DC_Conc,  "g/L",     xidSlurryTOC25_,        this,  isResult|noFile|noSnap|InitHidden);//duplicate of TotOrgCarbon
    DDB.Double("NaCl*",         "",   DC_Conc,  "g/L",     xidNaCl_Conc25,         this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("Na2SO4*",       "",   DC_Conc,  "g/L",     xidNa2SO4_Conc25,       this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("NaF*",          "",   DC_Conc,  "g/L",     xidNaF_Conc25,          this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("SiO2*",         "",   DC_Conc,  "g/L",     xidSilicateConc25,      this,  isResult|noFile|noSnap|InitHidden);

    DDB.Text("");
    DDB.Text("Bayer Liquor Values @ Temp");
    DDB.Double("AluminaConcT", "A@T", DC_Conc,  "g/L",     xidSlurryAluminaConc,   this,  isResult|noFile);
    DDB.Double("CausticConcT", "C@T", DC_Conc,  "g/L",     xidSlurryCausticConc,   this,  isResult|noFile);
    DDB.Double("SodaConcT",    "S@T", DC_Conc,  "g/L",     xidSlurrySodaConc,      this,  isResult|noFile);
    DDB.Double("TOC@T",         "",   DC_Conc,  "g/L",     xidSlurryTOC,           this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("SolidsConcT",   "",   DC_Conc,  "g/L",     xidSlurrySolidsConc,    this,  isResult|noFile);
    DDB.Double("BoilPtElev",    "",   DC_dT,    "C",       xidBayerBPE,            this,  isResult|noFile);

    DDB.Text("Bayer Liquor Precipitation Values @ Temp");
    DDB.Double("A_Saturation", "ASat",DC_Conc,  "g/L",     xidASaturation,         this,  isResult|noFile|noSnap);
    DDB.Double("OxalateSolubility", "OxSol",DC_Conc, "g/L", xidOxalateSolubility,  this,  isResult|noFile|noSnap);
    DDB.Double("BoehmiteSolubility", "BoehmiteSol",DC_Conc, "g/L", xidBoehmiteSolubility, this,  isResult|noFile|noSnap);
    DDB.Double("A/C_Saturation", "A/CSat", DC_, "",        xidAtoCSaturation,      this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("SSN_Ratio",     "",   DC_,      "",        xidSSNRatio,            this,  isResult|noFile|noSnap);
    //DDB.Double("Alumina_SSN",   "",   DC_,      "",        xidAluminaSSN,          this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("IonicStrength", "I",  DC_,      "",        xidIonicStrength,       this,  isResult|noFile|noSnap|InitHidden);
    DDB.Double("GrowthRate",    "GRate", DC_Ldt,   "um/s", xidGrowthRate,          this,  isResult|noFile|noSnap|InitHidden);
    }

  //DDB.Text("");
  }

// --------------------------------------------------------------------------

flag ASMBayer::DataXchg(DataChangeBlk & DCB)
  {
  if (SMBayerBase::DataXchg(DCB))
    return true;

  switch (DCB.lHandle)
    {
    case xidSlurryCausticConc:     DCB.D=CausticConc(Temp()); return 1;
    case xidSlurryAluminaConc:     DCB.D=AluminaConc(Temp()); return 1;
    case xidSlurrySodaConc:        DCB.D=SodaConc(Temp());    return 1;
    case xidSlurryCausticConc25:   DCB.D=CausticConc(C_2_K(25.0)); return 1;
    case xidSlurryAluminaConc25:   DCB.D=AluminaConc(C_2_K(25.0)); return 1;
    case xidSlurrySodaConc25:      DCB.D=SodaConc(C_2_K(25.0));    return 1;
    case xidSlurrySodiumCarbConc25:DCB.D=SodiumCarbonateConc(C_2_K(25.0));    return 1;
    case xidSlurryAtoC25:          DCB.D=AtoC(); return 1;
    case xidSlurryCtoS25:          DCB.D=CtoS(); return 1;
    case xidSlurryCltoC25:         DCB.D=CltoC(); return 1;
    case xidSlurryNa2CO3toS25:     DCB.D=Na2CO3toS(); return 1;
    case xidSlurryMR:              DCB.D=MR(); return 1;
    case xidSlurryAA:              DCB.D=AA(); return 1;
    case xidSlurryTT:              DCB.D=TT(); return 1;
    case xidTotalNaConc25:         DCB.D=TotalSodiumConc(); return 1;
    case xidASaturation:           DCB.D=AluminaConcSat(Temp()); return 1;
    case xidOxalateSolubility:     DCB.D=OxalateSolubility(Temp()); return 1;
    case xidBoehmiteSolubility:    DCB.D=BoehmiteSolubility(Temp()); return 1;
    case xidIonicStrength:         DCB.D=IonicStrength(); return 1;
    case xidGrowthRate:            DCB.D=GrowthRate()*1.0e-6; return 1;
    case xidAtoCSaturation:        DCB.D=AtoCSaturation(Temp()); return 1;
    case xidSSNRatio:              DCB.D=SSNRatio(Temp()); return 1;
    case xidAluminaSSN:            DCB.D=AluminaSSN(Temp()); return 1;
    case xidSlurryTOOC25:          DCB.D=TOOC(C_2_K(25.0)); return 1;
    case xidSlurryTOC25:           DCB.D=TOC(C_2_K(25.0)); return 1;
    case xidSlurryTOC:             DCB.D=TOC(Temp()); return 1;
    case xidSlurrySolidsConc:      DCB.D=SolidsConc(Temp()); return 1;
    case xidSlurrySolidsConc25:    DCB.D=SolidsConc(C_2_K(25.0)); return 1;
    case xidSlurryFreeCaustic25:   DCB.D=FreeCaustic(C_2_K(25.0)); return 1;
    case xidBayerBPE:              DCB.D=BoilPtElev(Temp()); return 1;
    case xidOrganicO11Conc25:      DCB.D=OrganicO11Conc(); return 1;
    case xidOrganateConc25:        DCB.D=OrganateConc(); return 1;
    case xidTotalOrgConc25:        DCB.D=TotalOrganics(); return 1;
    case xidOxalateConc25:         DCB.D=SodiumOxalateConc(C_2_K(25.0)); return 1;
    case xidNaClConc25:            DCB.D=ChlorineConc(); return 1;
    case xidNa2SO4Conc25:          DCB.D=SulphateConc(); return 1;
    case xidNa2OSolFrac:           DCB.D=Na2OSolFrac(); return 1;
    case xidSPOFrac:               DCB.D=SPO(); return 1;

    //case xidSlurryVapPress:        DCB.D=SaturationP(Temp()); return 1;
    case xidLiquidRho25:           DCB.D=Rho(som_Liq, C_2_K(25.0), Press()); return 1;
    case xidSlurryRho25:           DCB.D=Rho(som_SL,  C_2_K(25.0), Press()); return 1;
    case xidLiquidVolume25:        DCB.D=LVolume25(); return 1;
    case xidSlurryVolume25:        DCB.D=SLVolume25(); return 1;
    case xidOrganicO11_Conc25:     DCB.D=OrganicO11_Conc(); return 1;
    case xidOxalate_Conc25:        DCB.D=SodiumOxalate_Conc(); return 1;
    case xidNaCl_Conc25:           DCB.D=Chlorine_Conc(); return 1;
    case xidNa2SO4_Conc25:         DCB.D=Sulphate_Conc(); return 1;
    case xidNaF_Conc25:            DCB.D=Fluoride_Conc(); return 1;
    case xidSilicateConc25:        DCB.D=Silicate_Conc(); return 1;
    case xidSlurryTOC25_:          DCB.D=TOC(C_2_K(25.0)); return 1;

    //dRqd_TOC = dRqd_Org*MW_C/MW_Na2CO3*(5.0 - (3.0 * dRqd_OrgRatio));
    case xidRqdTOC:
      {
      const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989
      if (DCB.rD)
        {
        if (sm_iRqdCalcMethod==DLM_TOC)
          {//OxConc must remain constant
          double PrevOx = dRqd_Org * dRqd_OrgRatio;
          dRqd_Org = (((*DCB.rD)*MW_Na2CO3/MW_C) + (3.0 * PrevOx))/5.0;
          dRqd_OrgRatio = PrevOx/GTZ(dRqd_Org);
          }
        }
      DCB.D = dRqd_Org*MW_C/MW_Na2CO3*(5.0 - (3.0 * dRqd_OrgRatio));
      return 1;
      }
    case xidRqdOrgRatio:
      {
      const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989
      if (DCB.rD)
        {
        if (!DCB.ForFileSnpScn() && sm_iRqdCalcMethod==DLM_TOC)
          {
          double PrevTOC = dRqd_Org*MW_C/MW_Na2CO3*(5.0 - (3.0 * dRqd_OrgRatio));
          dRqd_OrgRatio = Range(0.0, *DCB.rD, 1.0);
          dRqd_Org = PrevTOC*MW_Na2CO3/MW_C/(5.0 - (3.0 * dRqd_OrgRatio));
          }
        else
          {
          dRqd_OrgRatio = Range(0.0, *DCB.rD, 1.0);
          if (PrjFileVerNo()<79)
            {
            dRqd_Ox = dRqd_Org * dRqd_OrgRatio;
            dRqd_TOC = dRqd_Org*MW_C/MW_Na2CO3*(5.0 - (3.0 * dRqd_OrgRatio));
            }
          }
        }
      DCB.D = dRqd_OrgRatio;
      return 1;
      }
    case xidRqdOxalate:
      {
      const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989
      if (DCB.rD)
        {
        if (sm_iRqdCalcMethod==DLM_TOC)
          {//TOC must remain constant
          double PrevTOC = dRqd_Org*MW_C/MW_Na2CO3*(5.0 - (3.0 * dRqd_OrgRatio));
          dRqd_OrgRatio = 5.0/GTZ((PrevTOC/GTZ(*DCB.rD)*MW_Na2CO3/MW_C) + 3.0);
          dRqd_Org = PrevTOC*MW_Na2CO3/MW_C/(5.0 - (3.0 * dRqd_OrgRatio));
          }
        }
      DCB.D = dRqd_Org * dRqd_OrgRatio;
      return 1;
      }

    }
  return 0;
  }

//---------------------------------------------------------------------------

flag ASMBayer::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=SpModelEx::ValidateData(VDB);
  if (fDoCalc)
    {    
    dRqd_AtoC = ValidateRange(VDB, "dRqd_AtoC", 0.0, dRqd_AtoC,    0.9);
    dRqd_C    = ValidateRange(VDB, "dRqd_C", 0.0, dRqd_C,       9000.0);
    dRqd_CtoS = ValidateRange(VDB, "dRqd_CtoS", 0.1, dRqd_CtoS,    1.0);
    dRqd_Sul  = ValidateRange(VDB, "dRqd_Sul", 0.0, dRqd_Sul,     500.0);
    dRqd_Sil  = ValidateRange(VDB, "dRqd_Sil", 0.0, dRqd_Sil,     500.0);
    dRqd_Salt = ValidateRange(VDB, "dRqd_Salt", 0.0, dRqd_Salt,   500.0);
    dRqd_Fl   = ValidateRange(VDB, "dRqd_Fl", 0.0, dRqd_Fl,       500.0);
    dRqd_P2O5 = ValidateRange(VDB, "dRqd_P2O5", 0.0, dRqd_P2O5,   500.0);
    dRqd_Org  = ValidateRange(VDB, "dRqd_Org", 0.0, dRqd_Org,     500.0);
    //dRqd_OrgRatio = ValidateRange(VDB, "dRqd_OrgRatio", 0.0, dRqd_OrgRatio, 1.0);
    dRqd_Ox   = ValidateRange(VDB, "dRqd_Ox", 0.0, dRqd_Ox,       500.0);
    dRqd_TOC  = ValidateRange(VDB, "dRqd_TOC", 0.0, dRqd_TOC,     500.0);
    dRqd_SolFrac  = ValidateRange(VDB, "dRqd_SolFrac", 0.0, dRqd_SolFrac,  0.99);
    dRqd_SolConc  = ValidateRange(VDB, "dRqd_SolConc", 0.0, dRqd_SolConc,  2400.0);
    dRqd_SolOx    = ValidateRange(VDB, "dRqd_SolOx", 0.0, dRqd_SolOx, 0.95);
    dRqd_SolOccSoda = ValidateRange(VDB, "dRqd_SolOccSoda", 0.0, dRqd_SolOccSoda, 0.95);

    //SetCI(2, (dRqd_S < 1e-6) || (dRqd_C <1e-6));
    //SetCI(3, dRqd_C > dRqd_S);
    
    // CNM - When adding RawMassFlow for Tears
    //if (Usage()!=SPMU_Flow)
    //  SetView(SVV_AsMassFrac); //forces view option to mass fraction
    DoInputCalcs();
    }
  
  return OK;
  }

//---------------------------------------------------------------------------

void ASMBayer::DoInputCalcs()
  {
  switch (GetView())
    {
    case SVV_AsRawFrac:
    case SVV_AsRawMass:
    case SVV_AsRawMassFlow:
      fDoCalc=false;
      return;
    }

  const double MW_Na2CO3    = ::SodiumCarbonate.MW;  //105.989
  const double MW_Na2O      = ::OccSoda.MW;          //61.9789360
  const double MW_Al2O3     = ::Alumina.MW;          //101.961278
  const double MW_THA       = ::THA.MW;              //156.007118
  const double MW_Na2C2O4   = ::SodiumOxalate.MW;    //133.999
  const double MW_Na2C5O11  = ::Organics1.MW;        //282.028

  //calculator to determine Liquor feed fractions based on requirements at 25C...
  double DensL = 1100.0;
  double DensS = 2420.0;
  const bool FindingConc=Valid(dRqd_SolConc);
  const bool WantSolids=(FindingConc && dRqd_SolConc>0.0) || (dRqd_SolFrac>1e-6);
  double TotalMass = Mass(som_SL);
  if (WantSolids)
    {
    if (FindingConc)
      dRqd_SolFrac=Range(0.0, (dRqd_SolConc*DensS)/(dRqd_SolConc*DensS + DensL*DensS - DensL*dRqd_SolConc), 0.99); // Initial Guess
    if (Mass(som_Sol)<1.0e-6 && TotalMass>UsableMass)
      {//assume THA solids required
      LogNote(FullObjTag(), 0, "Assume solids are THA for Bayer feed calculator");
      VValue[THA.SolPhInx()]=TotalMass*dRqd_SolFrac;
      const double Scl=(TotalMass*(1.0-dRqd_SolFrac))/GTZ(Mass(som_Liq));
      ScaleMass(som_Liq, Scl);
      TotalMass = Mass(som_SL);
      }
    //adjust solids Oxalate and OccSoda
    if (dRqd_SolOx>0.0 || dRqd_SolOccSoda>0.0)
      {
      const int OxIndex = SodiumOxalateSol.SolPhInx();
      const int OSIndex = OccSoda.SolPhInx();
      const double SolMass = Mass(som_Sol);
      const double PrevOther = SolMass - VValue[OxIndex] - VValue[OSIndex];
      if (dRqd_SolOx>0.0)
        VValue[OxIndex] = dRqd_SolOx*SolMass;
      else
        VValue[OxIndex] = 0.0;
      if (dRqd_SolOccSoda>0.0)
        VValue[OSIndex] = Min(dRqd_SolOccSoda/MW_Na2O*MW_Na2CO3/MW_THA*MW_Al2O3, 1.0-dRqd_SolOx)*SolMass;
      else
        VValue[OSIndex] = 0.0;
      //now scale all the other solids...
      const double NewOther = SolMass - VValue[OxIndex] - VValue[OSIndex];
      const double Scale = NewOther/GTZ(PrevOther);
      SpecieIter I(som_Sol);
      for (int i=-1; I.Loop(i); )
        {
        if (i!=OxIndex && i!=OSIndex)
          VValue[i] *= Scale;
        }

      ////to be sure...
      //const double Scl=(TotalMass*(1.0-dRqd_SolFrac))/GTZ(Mass(som_Liq));
      //ScaleMass(som_Liq, Scl);
      TotalMass = Mass(som_SL);
      }
    }
  else
    {
    dRqd_SolFrac = 0.0;
    }

  //----------------------------------------------
  const double LiqMassCalc = (VValue[Water.LiqPhInx()] + VValue[CausticSoda.LiqPhInx()] + VValue[Alumina.LiqPhInx()] + 
                        VValue[SodiumCarbonate.LiqPhInx()] + VValue[SodiumChloride.LiqPhInx()] + VValue[SodiumFluoride.LiqPhInx()] +
                        VValue[SodiumSulphate.LiqPhInx()] + VValue[SilicateLiq.LiqPhInx()] + VValue[SodiumOxalate.LiqPhInx()] + VValue[Organics.LiqPhInx()] +
                        VValue[Organics1.LiqPhInx()] + VValue[DiphosphorusLiq.LiqPhInx()]);
  const double LiqTtl = Mass(som_Liq);
  const double OtherLiqMass = LiqTtl - LiqMassCalc;
  const double VapMass = Mass(som_Gas);
  double LiqMass = TotalMass*(1.0-dRqd_SolFrac);
  if (OtherLiqMass>LiqMass && OtherLiqMass>0.0)
    {
    if (dRqd_SolFrac>0.0)
      LogWarning(FullObjTag(), 0, "Invalid Bayer feed calculator: large solids plus other liquids required");
    else
      LogWarning(FullObjTag(), 0, "Invalid Bayer feed calculator: large other liquids required");
    }

  if (LiqMass-OtherLiqMass > 1e-6)  // There are liquids required in the feed.
    {
    double WaterUsed = 0.0;
    const double P = Press();
    for (bool Done=false; !Done; )
      {
      dRqd_SolFrac=Range(0.0, dRqd_SolFrac, 0.99);
      LiqMass = TotalMass*(1.0-dRqd_SolFrac);

      double Density25;

      double Al, Caustic, Carbonate, Salt, NaSulphate, Silicate, Phosph, NaFluoride;//, Organic;
      double water, Derr, Org1, Org2, Org3;
      double Ox, Og, Og1, Other;
      if (PrjFileVerNo()<79)
        {//old
        if (dRqd_OrgRatio>1.0)
          {
          LogWarning(FullObjTag(), 0, "Required Organic Ratio adjusted (cannot be greater than 1)");
          dRqd_OrgRatio = 1.0;
          }
        Ox = dRqd_Org * dRqd_OrgRatio;
        //Assume split between organics Na2C5O7 and Na2C5O11 is 100% Na2C5O11
        Og = 0.0; //Og ==> Na2C5O7
        Og1 = dRqd_Org - Ox - Og; //Og1 ==> Na2C5O11
        }
      else
        {
        const double minTOC = dRqd_Ox*2.0/MW_Na2C2O4*MW_C;
        double TocUsed = dRqd_TOC;
        if (dRqd_TOC<minTOC)
          {
          LogWarning(FullObjTag(), 0, "Required TOC too low based on specified Oxalate");
          TocUsed = minTOC; //this will give Og of zero
          }
        Ox = dRqd_Ox;
        Og = 0.0; //Og ==> Na2C5O7
        Og1 = (TocUsed/MW_C - dRqd_Ox*2.0/MW_Na2C2O4)*MW_Na2C5O11/5.0; //Og1 ==> Na2C5O11
        }

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
              if (PrjFileVerNo()<79)
                {
                Tna = (dRqd_C/dRqd_CtoS + Ox + Og + Og1 + (dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4 + dRqd_Fl/(2*MW_NaF)) * MW_Na2CO3)
                      * Vol/(1000.0*Lm)*100.0; //Wgt % of Total Na
                }
              else
                {
                Tna = (dRqd_C/dRqd_CtoS + (Ox/MW_Na2C2O4 + Og/MW_Na2C5O7 + Og1/MW_Na2C5O11 + dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4 + dRqd_Fl/(2*MW_NaF)) * MW_Na2CO3)
                      * Vol/(1000.0*Lm)*100.0; //Wgt % of Total Na
                }
              Tcc = dRqd_C/(Dens1*1000.0)*100.0;//Wgt % of Caustic
              Tal = dRqd_AtoC * Tcc; //Wgt % of Alumina
              Density25 = CConcentrations::LiquorDensEqn1(25.0, Tna, Tal)/1000.0;
              break;
              }
            case BDM_March2002:
            case BDM_MD:
              {
              if (PrjFileVerNo()<79)
                {
                Tna = (dRqd_C/dRqd_CtoS + Ox + Og + Og1 + (dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4 + dRqd_Fl/(2*MW_NaF)) * MW_Na2CO3)
                      * Vol/(1000.0*Lm)*100.0; //Wgt % of Total Na
                }
              else
                {
                Tna = (dRqd_C/dRqd_CtoS + (Ox/MW_Na2C2O4 + Og/MW_Na2C5O7 + Og1/MW_Na2C5O11 + dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4 + dRqd_Fl/(2*MW_NaF)) * MW_Na2CO3)
                      * Vol/(1000.0*Lm)*100.0; //Wgt % of Total Na
                }
              XAl   = (dRqd_AtoC*dRqd_C/(Dens1*1000)*Lm)/Lm;
              TNa   = Tna/100.0*Lm*MW_Na2O/MW_Na2CO3;
              TCaus = dRqd_C/(Dens1*1000)*Lm;
              if (sm_iDensityMethod==BDM_March2002)
                TCaus *= (MW_Na2O/MW_Na2CO3);
              XNaC  = (TNa-TCaus)/Lm;
              XNa   = TNa/Lm;
              Density25 = CConcentrations::LiquorDensEqn2(25.0, XNaC, XNa, XAl)/1000.0;
              break;
              }
            case BDM_July2003:
              {
              double TTS = dRqd_C/dRqd_CtoS + (dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4)*MW_Na2CO3;
              double TOC;
              if (PrjFileVerNo()<79)
                TOC = dRqd_Org*MW_C/MW_Na2CO3*(5.0 - (3.0 * dRqd_OrgRatio));
              else
                TOC = dRqd_TOC;
              Density25 = CConcentrations::LiquorDensEqn3(25.0, dRqd_AtoC*dRqd_C, dRqd_C, TTS, TOC)/1000.0;
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
        if (PrjFileVerNo()<79)
          {//old
          Org1       = Ox * MW_Na2C2O4/MW_Na2CO3 * Vol;
          Org2       = Og * MW_Na2C5O7/MW_Na2CO3 * Vol;
          Org3       = Og1 * MW_Na2C5O11/MW_Na2CO3 * Vol;
          }
        else
          {
          Org1       = dRqd_Ox * Vol;
          Org2       = Og * Vol;
          Org3       = Og1 * Vol;
          }
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
        VValue[CausticSoda.LiqPhInx()]     = Caustic*LiqMass;
        VValue[Alumina.LiqPhInx()]         = Al*LiqMass;
        VValue[SodiumCarbonate.LiqPhInx()] = Carbonate*LiqMass;
        VValue[SodiumChloride.LiqPhInx()]  = Salt*LiqMass;
        VValue[SodiumSulphate.LiqPhInx()]  = NaSulphate*LiqMass;
        VValue[SilicateLiq.LiqPhInx()]     = Silicate*LiqMass;
        VValue[DiphosphorusLiq.LiqPhInx()] = Phosph*LiqMass;
        VValue[SodiumOxalate.LiqPhInx()]   = Org1*LiqMass;
        VValue[SodiumFluoride.LiqPhInx()]  = NaFluoride*LiqMass;
        VValue[Organics.LiqPhInx()]        = Org2*LiqMass;
        VValue[Organics1.LiqPhInx()]       = Org3*LiqMass;
        VValue[Water.LiqPhInx()]           = water*LiqMass;
        }
      else
        {
        // Default to water
        VValue[Water.LiqPhInx()]           = GEZ(LiqMass-OtherLiqMass);
        VValue[CausticSoda.LiqPhInx()]     = 0.0;
        VValue[Alumina.LiqPhInx()]         = 0.0;
        VValue[SodiumCarbonate.LiqPhInx()] = 0.0;
        VValue[SodiumChloride.LiqPhInx()]  = 0.0;
        VValue[SodiumSulphate.LiqPhInx()]  = 0.0;
        VValue[SilicateLiq.LiqPhInx()]     = 0.0;
        VValue[DiphosphorusLiq.LiqPhInx()] = 0.0;
        VValue[SodiumOxalate.LiqPhInx()]   = 0.0;
        VValue[SodiumFluoride.LiqPhInx()]  = 0.0;
        VValue[Organics.LiqPhInx()]        = 0.0;
        VValue[Organics1.LiqPhInx()]       = 0.0;
        }
      WaterUsed=VValue[Water.LiqPhInx()];
      
      if (FindingConc)
        {
        double PrevSF=dRqd_SolFrac;
        DensL=Density25*1000;
        DensS=SpModel::Rho(som_Sol, C_2_K(25.0), P);
        dRqd_SolFrac=Max(0.0, (dRqd_SolConc*DensS)/(dRqd_SolConc*DensS + DensL*DensS - DensL*dRqd_SolConc));
        if (fabs(PrevSF-dRqd_SolFrac)<1e-9)
          Done=true;
        }
      else
        Done=true;
      }// while
    if (WaterUsed<1.0e-9)
      LogWarning(FullObjTag(), 0, "Bayer feed calculator water set to zero");
    }
  
  //--------------------------------------------
  double MSol=Mass(som_Sol);
  if (WantSolids && MSol>1.0e-6)
    {
    double MOther=Mass(som_SL)-MSol;
    double Scl=dRqd_SolFrac*MOther/(MSol*(1.0-dRqd_SolFrac));
    ScaleMass(som_Sol, Scl);
    ClrCI(4);
    }
  else
    {
    // Set solid fractions to zero.
    ScaleMass(som_Sol, 0.0);
    SetCI(4, WantSolids);
    }
  }

//---------------------------------------------------------------------------

double ASMBayer::THAMassFlow()
  {
  return m_M[::THA.SolPhInx()];
  }

//---------------------------------------------------------------------------

double ASMBayer::THADens(double T_)
  {
  return SDB[::THA.SolPhInx()].Density(m_iFidelity, T_, Std_P, &m_Ovr, NULL);
  }

//---------------------------------------------------------------------------

double ASMBayer::CausticConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[::CausticSoda.LiqPhInx()] * DRatio);
  }

//---------------------------------------------------------------------------

double ASMBayer::AluminaConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[::Alumina.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double ASMBayer::SodaConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/LiqConcs25.Density25);
  double liq25S = LiqConcs25.Liq[::CausticSoda.LiqPhInx()]
                  //+ LiqConcs25.Liq[::SodiumAluminate.LiqPhInx()]
                  + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()];

  return Max(0.0, liq25S * DRatio); 
  }

//---------------------------------------------------------------------------

double ASMBayer::SodiumCarbonateConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[SodiumCarbonate.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double ASMBayer::SodiumSulphateConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[SodiumSulphate.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double ASMBayer::SodiumOxalateConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/LiqConcs25.Density25);
  return Max(0.0, LiqConcs25.Liq[SodiumOxalate.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double ASMBayer::SPO()
  {//Solid Phase Oxalate
  const double mt=Mass(som_Sol);
  return (mt>=UsableMass) ? m_M[::SodiumOxalateSol.SolPhInx()]/mt : 0.0;
  }

//---------------------------------------------------------------------------

double ASMBayer::Na2OSolFrac()
  {
  const double mt=Mass(som_Sol);
  return (mt>=UsableMass) ? (m_M[::OccSoda.SolPhInx()]/mt * ::OccSoda.MW / ::SodiumCarbonate.MW * ::THA.MW / ::Alumina.MW) : 0.0;
  }

//---------------------------------------------------------------------------

double ASMBayer::SolidsConc(double T_)
  {
  const double mt=Mass(som_Sol);
  return (mt>=UsableMass) ? mt/GTZ(Mass(som_SL))*Rho(som_SL, T_, Press(), pMArray()) : 0.0;
  }

//---------------------------------------------------------------------------

double ASMBayer::LVolume25()
  {
  const double mt=Mass(som_Liq);
  return ((mt>=UsableMass) ? (mt / Rho(som_Liq, C_2_K(25.0), Press())) : 0.0);
  }

double ASMBayer::SLVolume25()
  {
  const double mt=Mass(som_SL);
  return ((mt>=UsableMass) ? (mt / Rho(som_SL, C_2_K(25.0), Press())) : 0.0);
  }

//---------------------------------------------------------------------------

double ASMBayer::LDensity25()
  {
  return Rho(som_Liq, C_2_K(25.0), Press());
  }

double ASMBayer::SLDensity25()
  {
  return Rho(som_SL, C_2_K(25.0), Press());
  }

//---------------------------------------------------------------------------

double ASMBayer::TotalSodiumConc()
  {//TotalNa Conc @ 25
  LiqConcs25.Converge(MArray());
  double Sodium = LiqConcs25.Liq[::CausticSoda.LiqPhInx()]       // NaOH
                  + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()] // Na2CO3
                  + LiqConcs25.Liq[SodiumOxalate.LiqPhInx()]     // Na2C2O4
                  + LiqConcs25.Liq[Organics.LiqPhInx()]          // Na2C5O7
                  + LiqConcs25.Liq[Organics1.LiqPhInx()]         // Na2C5O11
                  + LiqConcs25.Liq[SodiumChloride.LiqPhInx()]    // NaCl
                  + LiqConcs25.Liq[SodiumSulphate.LiqPhInx()]    // Na2SO4
                  + LiqConcs25.Liq[SodiumFluoride.LiqPhInx()];   // NaF
  return Sodium;
  }

//---------------------------------------------------------------------------

double ASMBayer::OrganateConc()
  {//Organic Na2C5O7 Conc @ 25
  LiqConcs25.Converge(MArray());
  double Organate = LiqConcs25.Liq[::Organics.LiqPhInx()];
  return Organate;
  }

//---------------------------------------------------------------------------

double ASMBayer::OrganicO11Conc()
  {//Organic Na2C5O11 Conc @ 25
  LiqConcs25.Converge(MArray());
  double OrganicO11 = LiqConcs25.Liq[::Organics1.LiqPhInx()];
  return OrganicO11;
  }

//---------------------------------------------------------------------------

double ASMBayer::ChlorineConc()
  {
  LiqConcs25.Converge(MArray());
  double Chlorine = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()];
  return Chlorine;
  }

//---------------------------------------------------------------------------

double ASMBayer::SulphateConc()
  {
  LiqConcs25.Converge(MArray());
  double Sulphate = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()];
  return Sulphate;
  }

//---------------------------------------------------------------------------

double ASMBayer::TotalOrganics()
  {
  LiqConcs25.Converge(MArray());
  double Organics = LiqConcs25.Liq[::Organics.LiqPhInx()] + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]
                  + LiqConcs25.Liq[::Organics1.LiqPhInx()];
  return Organics;
  }

//---------------------------------------------------------------------------

double ASMBayer::TOOC(double T_)
  {//TOOC as Na2CO3 equivilent
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/LiqConcs25.Density25);
  double Tooc = (LiqConcs25.Liq[::Organics.LiqPhInx()]*5.0 + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]*2.0 + LiqConcs25.Liq[::Organics1.LiqPhInx()]*5.0);
  return Max(0.0, Tooc*DRatio);
  }

//---------------------------------------------------------------------------

double ASMBayer::TOOC_(double T_)
  {//NOT as Na2CO3
  //for Organics Na2C5O11 : 0.21294 = MW_Carbon*5.0/MW_Organics = 12.011 * 5.0 / 282.027936
  //for Organics Na2C5O7 : 0.27544 = MW_Carbon*5.0/MW_Organics = 12.011 * 5.0 / 218.030336
  //for SodiumOxalate Na2C2O4 : 0.17927 = MW_Carbon*2.0/MW_SodiumOxalate = 12.011 * 2.0 / 133.999136
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/LiqConcs25.Density25);
  double Tooc  = (LiqConcs25.Liq[::Organics.LiqPhInx()]/LiqConcs25.NaFactor[::Organics.LiqPhInx()]*0.27544 
               + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumOxalate.LiqPhInx()]*0.17927
               + LiqConcs25.Liq[::Organics1.LiqPhInx()]/LiqConcs25.NaFactor[::Organics1.LiqPhInx()]*0.21294);
  return Max(0.0, Tooc*DRatio);
  }

//---------------------------------------------------------------------------

double ASMBayer::TOC(double T_)
  {
  //TOOC = TOC*106/12
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/LiqConcs25.Density25);
  double Toc = (LiqConcs25.Liq[::Organics.LiqPhInx()]*5.0 + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]*2.0 + LiqConcs25.Liq[::Organics1.LiqPhInx()]*5.0)*MW_C/::SodiumCarbonate.MW;
#ifndef _RELEASE
  double Toc_ = (LiqConcs25.Liq[::Organics.LiqPhInx()]/LiqConcs25.NaFactor[::Organics.LiqPhInx()]*0.27544 
               + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumOxalate.LiqPhInx()]*0.17927
               + LiqConcs25.Liq[::Organics1.LiqPhInx()]/LiqConcs25.NaFactor[::Organics1.LiqPhInx()]*0.21294);
  //ASSERT(Toc==Toc_);
#endif
  return Max(0.0, Toc*DRatio);
  //for Organics Na2C5O11 : 0.21294 = MW_Carbon*5.0/MW_Organics = 12.011 * 5.0 / 282.027936
  //for Organics Na2C5O7 : 0.27544 = MW_Carbon*5.0/MW_Organics = 12.011 * 5.0 / 218.030336
  //for SodiumOxalate Na2C2O4 : 0.17927 = MW_Carbon*2.0/MW_SodiumOxalate = 12.011 * 2.0 / 133.999136
  }

//---------------------------------------------------------------------------

double ASMBayer::SodiumOxalate_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray());
  double Oxalate = LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()] / LiqConcs25.NaFactor[::SodiumOxalate.LiqPhInx()];
  return Oxalate;
  }

double ASMBayer::OrganicO11_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray());
  double OrganicO11 = LiqConcs25.Liq[::Organics1.LiqPhInx()] / LiqConcs25.NaFactor[::Organics1.LiqPhInx()];
  return OrganicO11;
  }

double ASMBayer::Sulphate_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray());
  double Sulphate = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()] / LiqConcs25.NaFactor[::SodiumSulphate.LiqPhInx()];
  return Sulphate;
  }

double ASMBayer::Chlorine_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray());
  double Chlorine = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()] / LiqConcs25.NaFactor[::SodiumChloride.LiqPhInx()];
  return Chlorine;
  }

double ASMBayer::Fluoride_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray());
  double Fluoride = LiqConcs25.Liq[::SodiumFluoride.LiqPhInx()] / LiqConcs25.NaFactor[::SodiumFluoride.LiqPhInx()];
  return Fluoride;
  }

double ASMBayer::Silicate_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray());
  double Silicate = LiqConcs25.Liq[::SilicateLiq.LiqPhInx()] / LiqConcs25.NaFactor[::SilicateLiq.LiqPhInx()];
  return Silicate;
  }

//---------------------------------------------------------------------------

double ASMBayer::IonicStrength()
  {
  const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989..
  const double MW_Na2O     = ::OccSoda.MW;          //61.9789..
  const double MW_NaCl     = ::SodiumChloride.MW;   //58.4425..
  const double MW_Na2SO4   = ::SodiumSulphate.MW;   //142.043..
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
      LiqConcs25.Converge(MArray());
      const double cTOC    = TOC(C_2_K(25.0));
      const double cNaCl   = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()] /LiqConcs25.NaFactor[::SodiumChloride.LiqPhInx()];
      const double cNa2CO3 = LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumCarbonate.LiqPhInx()];
      const double cNa2SO4 = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()] /LiqConcs25.NaFactor[::SodiumSulphate.LiqPhInx()];
      const double cNaOH   = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];
      I = 0.01887*cNaOH + k1*cNaCl/MW_NaCl + k2*cNa2CO3/MW_Na2CO3 + k3*cNa2SO4/MW_Na2SO4 + k4*0.01887*cTOC;
      return I;
      }
    case ASM_March2002:
      {
      const double K1 = 0.6499;
      const double K2 = 0.9909;
      const double K3 = 0.9107;
      const double K4 = 1.5062;
      LiqConcs25.Converge(MArray());
      const double SodaNaC  = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];
      const double NaClgpl  = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()];
      const double NaCgpl_  = LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()];
      const double NaSgpl   = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()];
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
      LiqConcs25.Converge(MArray());
      const double cTOC    = TOC(C_2_K(25.0));
      const double cNaCl   = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()] /LiqConcs25.NaFactor[::SodiumChloride.LiqPhInx()];
      const double cNa2CO3 = LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumCarbonate.LiqPhInx()];
      const double cNa2SO4 = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()] /LiqConcs25.NaFactor[::SodiumSulphate.LiqPhInx()];
      const double cNaOH   = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];
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
      LiqConcs25.Converge(MArray());
      const double cTOC    = TOC(273+25.0);
      const double cNaCl   = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()] /LiqConcs25.NaFactor[::SodiumChloride.LiqPhInx()];
      const double cNa2CO3 = LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumCarbonate.LiqPhInx()];
      const double cNa2SO4 = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()] /LiqConcs25.NaFactor[::SodiumSulphate.LiqPhInx()];
      const double cNaOH   = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];
      const double cC      = cNaOH;
      I = cNaOH/53.0 + k1*cNaCl/58.5 + k2*cNa2CO3/106.0 + k3*cNa2SO4/142.0 + k4/53.0*cTOC;
      return I;
      }
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

double ASMBayer::AluminaConcSat(double T_)
  {
  const double I = IonicStrength();
  const double I2 = Sqrt(I);
  const double cNaOH   = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];
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

double ASMBayer::IonicStrengthBoehmite()
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
  LiqConcs25.Converge(MArray());
  const double cTOC    = TOC(C_2_K(25.0));
  const double cNaCl   = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()] /LiqConcs25.NaFactor[::SodiumChloride.LiqPhInx()];
  const double cNa2CO3 = LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumCarbonate.LiqPhInx()];
  const double cNa2SO4 = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()] /LiqConcs25.NaFactor[::SodiumSulphate.LiqPhInx()];
  const double cNaOH   = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];
  const double cC      = cNaOH;
  const double I = 0.01887*cNaOH + k1*cNaCl/MW_NaCl + k2*cNa2CO3/MW_Na2CO3 + k3*cNa2SO4/MW_Na2SO4 + k4*0.01887*cTOC;
  return I;
  }

//---------------------------------------------------------------------------

double ASMBayer::BoehmiteSolubility(double T_)
  {
  /*Boehmite Solubility Reference: Denis Audet Q03 - DRA - Development of a new Boehmite solubility equation.doc 
  Similar to Alumina Solubility except that the constant are different.*/
  const double I = IonicStrengthBoehmite();
  const double I2 = Sqrt(I);
  const double cNaOH = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];
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

double ASMBayer::AtoCSaturation(double T_)
  {
  double C = CausticConc(T_);//should this be at 25 !?
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AluminaConcSat(T_) / GTZ(C);
  return ACsat;
  }

//---------------------------------------------------------------------------

double ASMBayer::SSNRatio(double T_)
  {// AtoC actual / AtoC saturation ==> A/ASat
  const double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AtoCSaturation(T_);
  return AtoC() / GTZ(ACsat);
  }

//---------------------------------------------------------------------------

double ASMBayer::AluminaSSN(double T_)
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

double ASMBayer::OxalateSolubility(double T_)
  {
  if (0)
    {
    //======================================== OxalateEquilibrium ===============================
    // Function calculates the equilibrium concentration (g/l) of oxalate, based on stream properties.
    // (British Aluminium - Burnt Island)
    const double C        = CausticConc(C_2_K(25.0)); // g/l
    const double Na2CO3Qm = m_M[SodiumCarbonate.LiqPhInx()]; // kg/s
    const double LiqQv    = LVolume25(); // m^3/s
    const double OxEquil = 7.62*Exps(0.012*K2C(T_) - (::OccSoda.MW/::SodiumCarbonate.MW)*(0.016*C + 0.011*Na2CO3Qm/GTZ(LiqQv)));
    return OxEquil; //(OxEquil+Approach);
    }
  else
    {
//Reference from Denis Audet May 2005:
//F.Tahiani, S.Buckingham, Report KR-86/019, Review of Information on Evaporative Crystallization... 
//This equation is an INTERNAL Alcan equation but based on a collection of available literature.
//NaOX solubility = exp(2.408502 + 0.014688*T in C - 0.003766*Na2SO4Conc - 0.005045*SodiumCarbonateConc + 0.004894*Al2O3Conc - 0.015613*CausticConc + 0.000011096*CausticConc^2)
    const double Tc = K2C(T_);
    LiqConcs25.Converge(MArray());
    const double Na2SO4Conc = LiqConcs25.Liq[SodiumSulphate.LiqPhInx()]    / LiqConcs25.NaFactor[::SodiumSulphate.LiqPhInx()];
    const double Na2CO3Conc = LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()] / LiqConcs25.NaFactor[::SodiumCarbonate.LiqPhInx()];
    const double Al2O3Conc  = LiqConcs25.Liq[::Alumina.LiqPhInx()]         / LiqConcs25.NaFactor[::Alumina.LiqPhInx()];
    const double C = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];
    const double OxSol = Exps(2.408502 + 0.014688*Tc - 0.003766*Na2SO4Conc - 0.005045*Na2CO3Conc + 0.004894*Al2O3Conc - 0.015613*C + 0.000011096*C*C);
    return OxSol;
    }
  return 0.0;
  }

//---------------------------------------------------------------------------
#define WithSpBlks 0

//double ASMBayer::PerformAluminaSolubility(double TRqd, double ARqd, double THARqd, double NoPerSec, double Na2OFac, double SSat, flag & ConvergeErr)
//  {
//  //   2 AlO2 + 4 H2O <==> Al2O3.3H2O + 2 OH
//  //or Na2O + Al2O3 + 4 H2O <==> Al2O3.3H2O + 2 NaOH
//
//  // THADelta is the Fraction of Alumina which precipitates as the hydrate
//  // ie THADelta is deposition rate of new THA crystal
//
//  const double ESoda=2535.0; //constant 2535K
//
//  bool AdjustT=!Valid(TRqd);
//  bool SetTHA=Valid(THARqd);
//  
//  #if dbgModels                                                                  
//  if (dbgPerformTHA ())
//    {
//    dbgp("PerformAluminaSolubility:");
//    dbgp(Valid(ARqd)?" ARqd:%12.4f":" ARqd:            ", ARqd);
//    dbgp(Valid(THARqd)?" THARqd:%12.4f":" THARqd:            ", THARqd);
//    dbgpln("");
//    }
//  #endif
//  
//  double T=AdjustT? Temp() : TRqd;
//
//  double &AluminaMass  = M[Alumina.LiqPhInx()];     // Al2O3
//  double &WaterMass    = M[Water.LiqPhInx()];       // H2O
//  double &THAMass      = M[THA.SolPhInx()];         // Al2O3.3H2O
//  double &CausticMass  = M[CausticSoda.LiqPhInx()]; // NaOH
//  double &Na2OMass     = M[OccSoda.SolPhInx()];     // Na2O
//
//  const double MW_H2O       = ::Water.MW;            //18.0152800
//  const double MW_Na2O      = ::OccSoda.MW;          //61.9789360
//  const double MW_NaOH      = ::CausticSoda.MW;      //39.9971080
//  const double MW_Al2O3     = ::Alumina.MW;          //101.961278
//  const double MW_Al2O3_3H2O= ::THA.MW;              //156.007118
//
//  const double Fact = MW_Al2O3/MW_Al2O3_3H2O; // 0.654;
//
//  #if WithSpBlks
//  dbgp("===M: ");
//  dbgp(" Alum:%16.8f", AluminaMass);
//  dbgp(" H2O:%16.8f", WaterMass  );
//  dbgp(" THA:%16.8f", THAMass    );
//  dbgp(" NaOH:%16.8f", CausticMass);
//  dbgp(" Na2O:%16.8f", Na2OMass   );
//  dbgpln("");
//  #endif      
//
//  for (int Iter=100; Iter; Iter--)
//    {
//    if (AdjustT)
//      T=Temp();
//
//    double A=AluminaConc(T);
//    double THA=SolidsConc(T);
//    double THADelta;
//    if (SetTHA)
//      {
//      if (fabs(THA-THARqd)<1.0e-12*THARqd)
//        break;
//      THADelta = THAMass*(THARqd/GTZ(THA)-1.);
//      }
//    else
//      {
//      if (fabs(A-ARqd)<1.0e-12*ARqd)
//        break;
//      THADelta = AluminaMass*(1.0-ARqd/GTZ(A))/Fact;
//      }
//
//    //const double LclVar = Na2OFac*exp(ESoda/T)*SSat*SSat;
//    //double XSoda = THADelta/100.0*LclVar;
//    const double LclVar = Na2OFac*1.68e-7*exp(ESoda/T)*Pow(SSat, 1.0955);
//    double XSoda = THADelta*LclVar;
//    //check for limit problem (specie availability)...
//
//    double dAluminaMass = - Fact*THADelta;
//    double dTHAMass     = + THADelta;
//    double dWaterMass   = - (1.0-Fact)*THADelta + MW_H2O/MW_Na2O*XSoda;
//    double dCausticMass = - 2*MW_NaOH/MW_Na2O*XSoda;
//    double dNa2OMass    = + XSoda;
//    double Scl;
//
//    for(;;)
//      {
//      Scl=1;
//      if (AluminaMass + Scl*dAluminaMass <0)
//        Scl=Min(Scl, 1-(AluminaMass + Scl*dAluminaMass)/(dAluminaMass));
//      if (THAMass     + Scl*dTHAMass     <0)
//        Scl=Min(Scl, 1-(THAMass + Scl*dTHAMass)/(dTHAMass));
//      if (WaterMass   + Scl*dWaterMass   <0)
//        Scl=Min(Scl, 1-(WaterMass + Scl*dWaterMass)/(dWaterMass));
//      if (CausticMass + Scl*dCausticMass <0)
//        Scl=Min(Scl, 1-(CausticMass + Scl*dCausticMass)/(dCausticMass));
//      if (Na2OMass    + Scl*dNa2OMass    <0)
//        Scl=Min(Scl, 1-(Na2OMass + Scl*dNa2OMass)/(dNa2OMass));
//      
//      if (Scl<(1-1e-12))
//        {
//        dAluminaMass *= Scl; 
//        dTHAMass     *= Scl; 
//        dWaterMass   *= Scl; 
//        dCausticMass *= Scl; 
//        dNa2OMass    *= Scl; 
//        }
//      else
//        break;
//      }
//    
//    if (fabs(dAluminaMass)<1e-22)
//      {//problem!!!
//      Iter=0;
//      break;
//      }
//    //if (Na2OMass + XSoda<1e-20)
//    //  { //set to limit
//    //  XSoda = 1e-20 - Na2OMass;
//    //  THADelta = XSoda*100.0/LclVar;
//    //  }
//    //if (CausticMass - 2*MW_NaOH/MW_Na2O*XSoda<1e-23)
//    //  { //set to limit
//    //  XSoda = (CausticMass - 1e-23)*MW_Na2O/(2*MW_NaOH);
//    //  if (Na2OMass + XSoda<1e-20)
//    //    XSoda = 1e-20 - Na2OMass;
//    //  THADelta = XSoda*100.0/LclVar;
//    //  }
//
//    //adjust masses...
//    AluminaMass = AluminaMass + dAluminaMass;
//    THAMass     = THAMass     + dTHAMass;    
//    WaterMass   = WaterMass   + dWaterMass;  
//    CausticMass = CausticMass + dCausticMass;
//    Na2OMass    = Na2OMass    + dNa2OMass;   
//
//    #if WithSpBlks
//    dbgp("   M: ");
//    dbgp(" Alum:%16.8f", AluminaMass);
//    dbgp(" H2O:%16.8f", WaterMass  );
//    dbgp(" THA:%16.8f", THAMass    );
//    dbgp(" NaOH:%16.8f", CausticMass);
//    dbgp(" Na2O:%16.8f", Na2OMass   );
//    dbgp(" Scl:%16.8f", Scl);
//    dbgpln("");
//    #endif      
//
//    #if dbgModels                                                                  
//    if (dbgPerformTHA ())
//      {
//      dbgp("                        :");
//      dbgp("    A:%12.4f", A);
//      dbgp("    THA:%12.4f", THA);
//      dbgp("    Iter:%5i", Iter);
//      dbgpln("");
//      }
//    #endif
//    
//    ClrStatesOK();
//    }
//
//  ConvergeErr = (Iter==0);
//  SetCI(5, ConvergeErr);
//
//  if (NoPerSec>0.0)
//    {
//    SetSAMFromFlow(THAMassFlow(), NoPerSec);
//    }
//
//  return AluminaConc(T);
//  }
//
//---------------------------------------------------------------------------

double ASMBayer::FreeCaustic(double T_)
  {
  return CausticConc(T_) * (1.0 - AtoC()*::SodiumCarbonate.MW/::Alumina.MW);
  }

//---------------------------------------------------------------------------

double ASMBayer::AtoC()
  {
  LiqConcs25.Converge(MArray());
  return LiqConcs25.Liq[::Alumina.LiqPhInx()] / GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double ASMBayer::CtoS()
  {
  LiqConcs25.Converge(MArray());
  return LiqConcs25.Liq[::CausticSoda.LiqPhInx()] 
         / GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()] + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double ASMBayer::CltoC()
  {
  LiqConcs25.Converge(MArray());
  return (LiqConcs25.Liq[SodiumChloride.LiqPhInx()])/GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double ASMBayer::Na2CO3toS()
  {
  LiqConcs25.Converge(MArray());
  return LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]
         / GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()] + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double ASMBayer::MR()
  {//ratio of Caustic molar flow to liq Alumina molar flow
  CSysVector &MA = MArray();
  double MR = (MA[::CausticSoda.LiqPhInx()]/::CausticSoda.MW / 2.0) / GTZ(MA[::Alumina.LiqPhInx()]/::Alumina.MW);
  //LiqConcs25.Converge(MArray());
  //double MR = (LiqConcs25.Liq[::CausticSoda.LiqPhInx()]/::CausticSoda.MW/2.0) / GTZ(LiqConcs25.Liq[::Alumina.LiqPhInx()]/::Alumina.MW);
  return MR;
  }

//---------------------------------------------------------------------------

double ASMBayer::AA()
  {//Total Na Conc as Na2O equivilant @25
  double AANa2O = TotalSodiumConc()*::OccSoda.MW/::SodiumCarbonate.MW; //61.98=MW Na2O, 105.99=MW Na2CO3
  return AANa2O;
  }

//---------------------------------------------------------------------------

double ASMBayer::TT()
  {//Caustic Conc as Na2O equivilant @25
  LiqConcs25.Converge(MArray());
  double TTNa2O = LiqConcs25.Liq[::CausticSoda.LiqPhInx()]*::OccSoda.MW/::SodiumCarbonate.MW; //61.98=MW Na2O, 105.99=MW Na2CO3
  return TTNa2O;
  }

//---------------------------------------------------------------------------

double ASMBayer::TOOCtoC()
  {
  double cTOC = TOOC(C_2_K(25.0));
  return (cTOC/GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()]));
  }

//---------------------------------------------------------------------------

double ASMBayer::Na2SO4toC()
  {
  LiqConcs25.Converge(MArray());
  return (LiqConcs25.Liq[SodiumSulphate.LiqPhInx()])/GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double ASMBayer::BoilingPtElevation(double P_, CSysVector * pMA)
  {
  double T_=Temp();
  double BPE=BoilPtElev(T_, pMA);
  //return BPE;
  return Max(0.0, BPE);
  }

//---------------------------------------------------------------------------

double ASMBayer::BoilPtElev(double T_, CSysVector * pMA)
  {
  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  double TLiq=MA.Sum(som_Liq); // Total Liquid kg/s
  if (TLiq<1.0e-9)
    return 0.0;

  double BPE = 0.0;
  if (sm_iBPEMethod==BPM_Dewey)
    {
    // Gram ion per kg liq
    double giAl2O3 = 1000 * MA[Alumina.LiqPhInx()] / TLiq * 2 * 81.97 / ::Alumina.MW; // 81.97 = MW of NaAlO2
    double giNaOH = 1000 * MA[CausticSoda.LiqPhInx()] / TLiq 
          - (1000 * MA[Alumina.LiqPhInx()] / TLiq * 2 * ::CausticSoda.MW/::Alumina.MW);
    double giNa2CO3 = 1000 * MA[SodiumCarbonate.LiqPhInx()] / TLiq;
    double giNaCl = 1000 * MA[SodiumChloride.LiqPhInx()] / TLiq;
    double giNa2SO4 = 1000 * MA[SodiumSulphate.LiqPhInx()] / TLiq;
    double giNa2C5O7 = 1000 * MA[Organics.LiqPhInx()] / TLiq;
    double giNa2C2O4 = 1000 * MA[SodiumOxalate.LiqPhInx()] / TLiq;

    double giSum = giAl2O3 + giNaOH + giNa2CO3 + giNaCl + giNa2SO4 + giNa2C5O7 + giNa2C2O4;

    double gH2OperKgLiq = 1000.0 - giSum;

    //Gram moles per kg liq
    double gmAl2O3 = giAl2O3 / 81.97; // 81.97 = MW of NaAlO2
    double gmNaOH = giNaOH / ::CausticSoda.MW;
    double gmNa2CO3 = giNa2CO3 / ::SodiumCarbonate.MW;
    double gmNaCl = giNaCl / ::SodiumChloride.MW;
    double gmNa2SO4 = giNa2SO4 / ::SodiumSulphate.MW;
    double gmNa2C5O7 = giNa2C5O7 / ::Organics.MW;
    double gmNa2C2O4 = giNa2C2O4 / ::SodiumOxalate.MW;

    double gmSum = gmAl2O3 + gmNaOH + gmNa2CO3 + gmNaCl + gmNa2SO4 + gmNa2C5O7 + gmNa2C2O4;

    // Total gmoles/kg H2O = molality
    //double gmSumPerH2O = 1000 * gmSum/gH2OperKgLiq;
    double gmSumPerH2O = 1000.0 * gmSum/Max(0.1, gH2OperKgLiq);

    // Boiling point elevation degC
    BPE = 0.00182 + 0.55379*Pow((gmSumPerH2O/10.0),7)
                + 0.0040625*gmSumPerH2O*T_
                + (1.0/T_) * (-286.66*gmSumPerH2O + 29.919*gmSumPerH2O*gmSumPerH2O
                +0.6228*Pow(gmSumPerH2O,3))
                - (0.032647*gmSumPerH2O*Pow((gmSumPerH2O*T_/1000.0),2))
                + (Pow(T_*0.001,5.0) * (5.9705*gmSumPerH2O
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
    double Sodium = LiqConcs25.Liq[::CausticSoda.LiqPhInx()] + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]
                  + LiqConcs25.Liq[SodiumOxalate.LiqPhInx()] + LiqConcs25.Liq[Organics.LiqPhInx()]
                  + LiqConcs25.Liq[SodiumChloride.LiqPhInx()] + LiqConcs25.Liq[SodiumSulphate.LiqPhInx()];
    const double x1 = Sodium*::OccSoda.MW/::SodiumCarbonate.MW;
    const double x2 = K2C(T_);
    const double x1_2 = x1*x1;
    const double x2_2 = x2*x2;
    BPE = a + b*x1 + c*x2 + d*x1_2 + e*x2_2 + f*x1*x2 + g*x1*x1_2 + h*x2*x2_2 + i*x1*x2_2 + j*x1_2*x2;
    }

  return BPE*dBPEFactor;
  }

//---------------------------------------------------------------------------

double ASMBayer::TotalSodium(CSysVector * pMA)
  {
  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  LiqConcs25.Converge(MA);
  double TSodium = LiqConcs25.LTotalSodium(MA); // The sodium components in liq.

  return TSodium;
  }

//---------------------------------------------------------------------------

double ASMBayer::SaturationP(double T_, CSysVector * pMA)
  {
  // After Dewey - Light Metals 1981 p 185
  // Employs gram-ion calculation method - see also BpElev

  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  if (MA.Sum(som_SL)/GTZ(MA.Sum())<1.0e-6)
    return SpModelEx::SaturationP(T_, &MA);

  #if TestBayerForWater
  double TLiq = MA.Sum(som_Liq); // Total Liquid kg/s
  if ((TLiq>1e-6) && (MA[Water.LiqPhInx()]/TLiq>H2OTestFrac))
    return SpModelEx::SaturationP(T_, pMA);
  #endif

  //converge...
  const int BPE_MaxIter = 8;
  const double BPE_Tol = 1.0e-3;
  double BPE = 0.0;
  for (int Iter=BPE_MaxIter; Iter; Iter--)
    {
    double NewBPE = BoilPtElev(T_-BPE, pMA);
    NewBPE = Range(0.0, NewBPE, 20.0);
    if (fabs(NewBPE-BPE)<BPE_Tol)
      break;
    BPE = NewBPE;
    }
  double SatP = SpModelEx::SaturationP(T_-BPE, &MA);

  #if dbgModels
  if (dbgSpecies() && DoDbgBrk())
    {
    dbgp("SatP @T:%12.5f BPE %12.5f  P%12.5f", K2C(T_), BPE, SatP);
    for (int s=0; s<SDB.Count(); s++)
      dbgp(" %12.5g", m_M[s]);
    dbgpln("  %s", FullObjTag());
    }
  #endif
  return SatP;
  }

//---------------------------------------------------------------------------

double ASMBayer::SaturationT(double P_, CSysVector * pMA)
  {
  // After Dewey - Light Metals 1981 p 185
  // Employs gram-ion calculation method - see also BpElev
  flag Local   = (pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  if (MA.Sum(som_SL)/GTZ(MA.Sum())<1.0e-6)
    return SpModelEx::SaturationT(P_, &MA);

  #if TestBayerForWater
  double TLiq = MA.Sum(som_Liq); // Total Liquid kg/s
  if ((TLiq>1e-6) && (MA[Water.LiqPhInx()]/TLiq>H2OTestFrac))
    return SpModelEx::SaturationT(P_, pMA);
  #endif

  double SatT = SpModelEx::SaturationT(P_, &MA);
  double BPE = BoilPtElev(SatT, pMA);
  return SatT+BPE;
  }

//---------------------------------------------------------------------------

double ASMBayer::Rho(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);
  const double QmSL = MA.Sum(som_SL);
  if (QmSL<UsableMass || QmSL/GTZ(MA.Sum())<1.0e-6)
    return SpModelEx::Rho(Phase, T_, P_, &MA);

  SpMArray Mn(MA, Phase);
  Mn.Normalise();
  
  double FSol=Mn.Sum(som_Sol);
  double FLiq=Mn.Sum(som_Liq);
  double FVap=1.0-FSol-FLiq;            
  double Dl=1.0;//,Ds=1.0, Dv=1.0;
  if (FLiq>1.0e-9)
    {
    #if TestBayerForWater
    if (Mn[Water.LiqPhInx()]/FLiq>H2OTestFrac)
      Dl = dNAN;
    else
    #endif
      {
      LiqConcs25.Converge(MA); //todo : check!  Should this be Mn instead of MA???
      Dl = dRhoFactor*LiqConcs25.LiquorDensity(T_, MA); //todo : check!  Should this be Mn instead of MA???
      }
    }
  //if (FVap>1.0e-9)
  //  Dv=SpModelEx::Rho(som_Gas, T_, P_, &Mn);

  return SDB.RhoMix(m_iFidelity, FSol, dNAN, FLiq, Dl, (1.0-FSol-FLiq), dNAN, T_, P_, &m_Ovr, Mn);
  }

//---------------------------------------------------------------------------

double ASMBayer::LiqCpCalc(CSysVector & Mn, double FLiq, double T)
  {
  switch (ASMBayer::sm_iCpMethod)
    {
    case CPM_Original:
      {
      // Heat Capacity of Bayer Liquor Kcal/kg.degC
      // A function of the weight fraction of TNa and Al2O3
      // From published Russian data

      LiqConcs25.Converge(Mn);

      double TNaAsCO3    = LiqConcs25.LTotalSodium(Mn);
      double TNaAsCO3Use = Max(TNaAsCO3, FLiq*0.19); // Only Valid for  TNaAsCO3/FLiq > 0.19
      double TNa         = 100.0*TNaAsCO3*::OccSoda.MW/::SodiumCarbonate.MW/FLiq; //61.98=MW Na2O, 105.99=MW Na2CO3
      double TAl2O3      = 100.0*Mn[Alumina.LiqPhInx()]/FLiq;
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
      LiqConcs25.Converge(Mn);
      const double CCaus = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];//CausticConc(C2K(25.0));
      if (CCaus>0.0)
        {
        const double CCarb = LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()];//SodiumCarbonateConc(C2K(25.0));
        const double CAlum = LiqConcs25.Liq[::Alumina.LiqPhInx()];//AluminaConc(C2K(25.0));
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
        double Tf = ( K2C(T)*1.8 + 32.0); // convert to F
        Cpl = ( 1.0152 -3.6171E-4*Tf +2.1709E-6*Tf*Tf
                 -2.9831e-9*pow(Tf,3));
        Cpl = KCal_2_kJ(Cpl); // kJ/kg.degC   (KCal_2_kJ=4.184)
        }
      return Cpl;
      }
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

double ASMBayer::msCp(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);
  const double QmSL = MA.Sum(som_SL);
  if (QmSL<UsableMass || QmSL/GTZ(MA.Sum())<1.0e-6)
    return SpModel::msCp(Phase, T_, P_, &MA, pTotalM);

  SpMArray Mn(MA, Phase);
  if (pTotalM)
    *pTotalM=Mn.Normalise();
  else
    Mn.Normalise();
  
  double FSol=Mn.Sum(som_Sol);
  double FLiq=Mn.Sum(som_Liq);

  double Cpl=0.0;
  if (FLiq>1.0e-9)
    {// Liquid
    #if TestBayerForWater
    if (Mn[Water.LiqPhInx()]/FLiq>H2OTestFrac)
      Cpl = dNAN;
    else
    #endif
      Cpl = LiqCpCalc(Mn, FLiq, T_);
    }

  double Cps=0.0;
  if (FSol>1.0e-9)
    {//Solids
    Cps = SDB.msCp(m_iFidelity, som_Sol, T_, P_, &m_Ovr, Mn);
    Cps = Max(Cps, sm_dMinSolCp);
    }

  return SDB.msCpMix(m_iFidelity, FSol, Cps, FLiq, Cpl, (1.0-FSol-FLiq), dNAN, T_, P_, &m_Ovr, Mn);
  //return SDB.msCpMix(FSol, dNAN, FLiq, Cpl, (1.0-FSol-FLiq), SpModel::msCp(som_Gas, T_, P_, &Mn), T_, P_, &m_Ovr, Mn);
  }

// ==========================================================================

double ASMBayer::msHm(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  flag Local   = (pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);
  if (MA.Sum(som_SL)/GTZ(MA.Sum())<1.0e-6)
    return SpModel::msHz(Phase, T_, P_, &MA, pTotalM);

  SpMArray Mn(MA, Phase);
  if (pTotalM)
    *pTotalM=Mn.Normalise();
  else
    Mn.Normalise();

  double FSol=Mn.Sum(som_Sol);
  double FLiq=Mn.Sum(som_Liq);
  double Tc=K_2_C(T_);
  // Ref Temp =0.0 dC;
  
  double Hl=0.0;
  if (FLiq>1.0e-9)
    {
    #if TestBayerForWater
    if (Mn[Water.LiqPhInx()]/FLiq>H2OTestFrac)
      Hl = dNAN;
    else
    #endif
      Hl = LiqCpCalc(Mn, FLiq, T_) * Tc;
    }
  
  double Hs=0.0;
  if (FSol>1.0e-9)
    {//Solids
    /*double Cps = SDB.msCp(som_Sol, T_, P_, &m_Ovr, Mn);
    Cps = Max(Cps, sm_dMinSolCp);
    Hs = Cps*Tc;*/
    Hs = SDB.msHz(m_iFidelity, som_Sol, T_, P_, &m_Ovr, Mn);
    }

  return SDB.msHmMix(m_iFidelity, FSol, Hs, FLiq, Hl, (1.0-FSol-FLiq), dNAN, T_, P_, &m_Ovr, Mn);
  //return SDB.msHmMix(FSol, dNAN, FLiq, Hl, (1.0-FSol-FLiq), SpModel::msHz(som_Gas, T_, P_, &Mn), T_, P_, &m_Ovr, Mn);
  }

// --------------------------------------------------------------------------

double ASMBayer::SpecieConc(double T_, int iSpNo, PhMask PhaseM)
  {
  if (!SDB[iSpNo].IsLiq())
    return SpModelEx::SpecieConc(T_, iSpNo, PhaseM);
  
  LiqConcs25.Converge(MArray());

  double C = LiqConcs25.Liq[iSpNo];
  
  if (fabs(T_ - C_2_K(25.0))>0.1)
    return C * LiqConcs25.LiquorDensity(T_, MArray())/ LiqConcs25.Density25;  
  return C;
  }

// --------------------------------------------------------------------------

double ASMBayer::PhaseConc(double T_, PhMask Ph, PhMask PhaseM)
  {
  // To Be Improved
  return SpModelEx::PhaseConc(T_, Ph, PhaseM);
  }

//--------------------------------------------------------------------------

double ASMBayer::GrowthRate()
  {
  const double MW_Al2O3     = ::Alumina.MW;          //101.961278
  const double MW_Al2O3_3H2O= ::THA.MW;              //156.007118
  switch (ASMBayer::sm_iGrowthMethod)
    {
    case GRM_Original:
      {//perhaps delete this?
      const double T   = Temp(); //Kelvin
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
        const double T       = Temp(); //Kelvin
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

double ASMBayer::AgglomerationRate()
  {
  return 0;
  };

//--------------------------------------------------------------------------

double ASMBayer::NucleationRate(double SpecSurface)
  {
  double T=Temp();
  double T_ = (sm_iGrowthMethod==GRM_Hyprod ? T-0.16 : T);
  double RatePerHour=sm_Nucleation*exp(34194.0/T_-96.4)*Sqr(AluminaConcSat(T)-AluminaConc(T)) * 
         pow(SolidsConc(T)*SpecSurface, 3.17);
  return RatePerHour/3600.0;
  };

//--------------------------------------------------------------------------

flag ASMBayer::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    //case 1: pS="W\tTemperature out of Range";          return 1;
    //case 2: pS="W\tCaustic Conc out of Range";         return 1;
    //case 3: pS="W\tCarbon to Sulphur out of Range";    return 1;
    //case 4: pS="W\tChlorine to Carbon out of Range";   return 1;
    //case 5: pS="W\tTOOC to Carbon out of Range";       return 1;
    //case 6: pS="W\tA/C Convergence not Starting";      return 1;
    //case 7: pS="W\tA/C not Converging";                return 1;
    case 1: pS="W\tLiquor Density not converged"; return 1;
    case 2: pS="W\t2 ???"; return 1;
    case 3: pS="W\t3 ???"; return 1;
    case 4: pS="W\tFeed calculator cannot provide required solids"; return 1;
    case 5: pS="W\tCannot converge PrecipTHA Alumina Conc"; return 1;
    default:                                               
      return SpModelEx::CIStrng(No, pS);
    }
  }

//--------------------------------------------------------------------------
#if WithProps
//===========================================================================
// The properties that are defined for visibility in the SysCAD Access properties window
// in adition, these properties are accesable in code by name.

enum {
idDensityMethod		  ,
idASatMethod				,
idGrowthMethod			,
idCPMethod				  ,
idBPEMethod				  ,
idMinSolCp				  ,
idMorphology				,
idNucleation				,
idNa2OFactor				,
idCarbonFactor			,

idDefineLiquor			,
idDefnLiqMethod		  ,
idRqd_A_to_C				,
idRqd_C_to_S				,
idRqd_C					    ,
idRqd_Organic			  ,
idRqd_OrgRatio			,
idRqd_TOC				    ,
idRqd_Oxalate			  ,
idRqd_Na2SO4				,
idRqd_NaCl				  ,
idRqd_NaF				    ,
idRqd_SiO2				  ,
idRqd_SolConc			  ,
idRqd_SolFrac			  ,

idAluminaConc25		  ,
idAtoC					    ,
idCtoS					    ,
idCltoC					    ,
idNa2SO4toC				  ,
idNa2CO3toS				  ,
idTOC25					    ,
idNa2C2O4Conc25		  ,
idNa2C5O11Conc25		,
idNaClConc25				,
idNaSO4Conc25			  ,
idNaFConc25				  ,
idSiO2Conc25				,
idSolidsConc25			,

idSeparator1				,

idCausticConc25		  ,
idSodaConc25				,
idCarbonateConc25	  ,
idOrganateConc25		,
idOrganicO11Conc		,
idOxalateConc25		  ,
idTotalOrganics25	  ,
idChlorineConc25		,
idSulphateConc25		,
idTotalNa25				  ,

idSeparator2				,

idMR						    ,
idAA						    ,
idTT						    ,

idSeparator3				,

idLVolume25				  ,
idSLVolume25				,
idLDensity25 			  ,
idSLDensity25			  ,

idSeparator4				,

idAluminaConc			  ,
idSodiumCarbonateConc,
idTOC					      ,
idSolidsConc				,

idSeparator5				,

idCausticConc			  ,
idSodaConc				  ,

idSeparator6				,

idAluminaConcSat		,
idIonicStrength		  ,
idAtoCSaturation		,
idSSNRatio				  ,
idTHAMassFlow   		,
idTHADens				    ,
idOxalateSolubility ,
idBoehmiteSolubility,
idBoilPtElev				,

idMPI_EndOfProps };

//---------------------------------------------------------------------------

long ASMBayer::DefinedPropertyCount()
  {
  return idMPI_EndOfProps + 1 + SpModelEx::DefinedPropertyCount();
  }

//--------------------------------------------------------------------------

static MPropertyInfo::MStringValueP SVOrg[]={
  {"TotOrganics and Ratio", DLM_TotOrganics},
  {"TOC and Oxalate",        DLM_TOC},
  {0}};
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
  { "MD", BDM_MD },
  {0}};
static MPropertyInfo::MStringValueP SVGrw[]={
  { "Original",  GRM_Original  },
  { "Nov2003", GRM_Nov2003 },
  { "Hyprod",  GRM_Hyprod  },
  {0}};
static MPropertyInfo::MStringValueP SVCp[]={
  {"Original",  CPM_Original   },
  {"Hyprod",	CPM_Hyprod },
  {0}};
static MPropertyInfo::MStringValueP SVBPE[]={
  {"Dewey",   BPM_Dewey   },
  {"Adamson", BPM_Adamson },
  {0}};

  long ASMBayer::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
    {//define a list of all properties
    long Inx=Index-SpModelEx::DefinedPropertyCount();
    switch(Inx)
      {
      case idDensityMethod	: Info.SetText("--Global options---------");
                              Info.SetStructName("Config"); //NB: The "struct name" is used as part of tag!
                              Info.Set(ePT_Long,      "DensityMethod",           "", SVDens,                    MP_GlobalProp|MP_Parameter,       "Global density method"); return Inx;
      case idASatMethod			: Info.Set(ePT_Long,      "ASatMethod",              "", SVASat,					MP_GlobalProp|MP_Parameter,       "Global ASat Method");          return Inx;
      case idGrowthMethod		: Info.Set(ePT_Long,      "GrowthMethod",            "", SVGrw,						MP_GlobalProp|MP_Parameter,       "Global Growth Method used by Precip");          return Inx;
      case idCPMethod			  : Info.Set(ePT_Long,      "CPMethod",                "", SVCp,                      MP_GlobalProp|MP_Parameter,       "Global CP method"); return Inx;
      case idBPEMethod			: Info.Set(ePT_Long,      "BPEMethod",               "", SVBPE,                     MP_GlobalProp|MP_Parameter,       "Global BPE method"); return Inx;
      case idMinSolCp			  : Info.Set(ePT_Double,    "MinSolCp",                "", DC_CpMs,  "kJ/kg.C", 0, 0, MP_GlobalProp|MP_Parameter|MF_InitHidden, ""); return Inx;
      case idMorphology			: Info.Set(ePT_Double,    "Morphology",              "", DC_,      "",        0, 0,	MP_GlobalProp|MP_Parameter,       "A Desc");          return Inx;
      case idNucleation			: Info.Set(ePT_Double,    "Nucleation",              "", DC_,      "",        0, 0,	MP_GlobalProp|MP_Parameter,       "A Desc");          return Inx;
      case idNa2OFactor			: Info.Set(ePT_Double,    "Na2OFactor",              "", DC_,      "",        0, 0,	MP_GlobalProp|MP_Parameter,       "A Desc");          return Inx;
      case idCarbonFactor		: Info.Set(ePT_Double,    "CarbonFactor",            "", DC_,      "",        0, 0,	MP_GlobalProp|MP_Parameter,       "A Desc");          return Inx;

/*      case idDefineLiquor		: Info.SetText("--Calculator-------------");
                              Info.SetStructName("Calc"); //NB: The "struct name" is used as part of tag!
                              Info.Set(ePT_Bool,      "DefineLiquor",           "", DC_,      "",    0, 0,    MP_ConfigProp|MP_Parameter|PIF_CheckBox, "");         return Inx;
      case idDefnLiqMethod	: Info.Set(ePT_Long,      "DefnLiqMethod",          "", SVOrg,                    MP_ConfigProp|MP_Parameter, "");                      return Inx;
      case idRqd_A_to_C			: Info.Set(ePT_Double,    "Rqd_A/C",                "", DC_,      "",    0, 0,    MP_ConfigProp|MP_Parameter, "The Reqd Alumina/Caustic Ratio");                      return Inx;
      case idRqd_C_to_S			: Info.Set(ePT_Double,    "Rqd_C/S",                "", DC_,      "",    0, 0,    MP_ConfigProp|MP_Parameter, "The Reqd Caustic/Soda Ratio");                      return Inx;
      case idRqd_C				  : Info.Set(ePT_Double,    "Rqd_C",                  "", DC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The Reqd Caustic Conc expressed as Carbonate @ 25");                      return Inx;
      case idRqd_Organic		: Info.Set(ePT_Double,    "Rqd_Organic",            "", DC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The Reqd total organic Carbon conc expressed as Carbonate @ 25");                      return Inx;
      case idRqd_OrgRatio		: Info.Set(ePT_Double,    "Rqd_OrgRatio",           "", DC_Frac,  "%",   0, 0,    MP_ConfigProp|MP_Parameter, "The Reqd Oxalate/Total Organic Carbon conc expressed as Carbonate @ 25");                      return Inx;
      case idRqd_TOC			  : Info.Set(ePT_Double,    "Rqd_TOC",                "", DC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The Reqd Total Organic Carbon Conc expressed as Carbon @ 25");                      return Inx;
      case idRqd_Oxalate		: Info.Set(ePT_Double,    "Rqd_Oxalate",            "", DC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The Reqd Oxalate conc expressed as Carbonate @ 25");                      return Inx;
      case idRqd_Na2SO4			: Info.Set(ePT_Double,    "Rqd_Na2SO4",             "", DC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The Reqd Sodium Sulphate Concentration @ 25");                      return Inx;
      case idRqd_NaCl			  : Info.Set(ePT_Double,    "Rqd_NaCl",               "", DC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The Reqd Sodium Chloride Concentration @ 25");                      return Inx;
      case idRqd_NaF			  : Info.Set(ePT_Double,    "Rqd_NaF",                "", DC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The Reqd Sodium Fluoride Concentration @ 25");                      return Inx;
      case idRqd_SiO2			  : Info.Set(ePT_Double,    "Rqd_SiO2",               "", DC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter, "The Reqd Silica Concentration @ 25");                      return Inx;
      case idRqd_SolConc		: Info.Set(ePT_Double,    "Rqd_SolConc",            "", DC_Conc,  "g/L", 0, 0,    MP_ConfigProp|MP_Parameter|PIF_NanOK, "The Required Solids Conc");            return Inx;
      case idRqd_SolFrac		: Info.Set(ePT_Double,    "Rqd_SolFrac",            "", DC_Frac,  "%",   0, 0,    MP_ConfigProp|MP_Parameter, "The Required Solids Fraction");                      return Inx;*/

      case idAluminaConc25	: Info.SetStructName("Props"); //NB: The "struct name" is used as part of tag! Recomend "Props" for compatability with other properties models.
                              Info.SetText("--Concentration and Ratio @ 25,-----------");
                              Info.Set(ePT_Double,    "Alumina@25",             "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Alumina Concentration @ 25"); return Inx;
      case idAtoC				    : Info.Set(ePT_Double,    "A/C",                    "", DC_,     "",       0,      0,  MP_Result|MP_NoFiling,    "A to C ratio"); return Inx;
      case idCtoS				    : Info.Set(ePT_Double,    "C/S",                    "", DC_,     "",       0,      0,  MP_Result|MP_NoFiling,    "C to S ratio"); return Inx;
      case idCltoC				  : Info.Set(ePT_Double,    "Cl/C",                   "", DC_,     "",       0,      0,  MP_Result|MP_NoFiling|MF_InitHidden,    "Cl to C ratio"); return Inx;
      case idNa2SO4toC			: Info.Set(ePT_Double,    "SO4/C",                  "", DC_,     "",       0,      0,  MP_Result|MP_NoFiling|MF_InitHidden,    "Na2SO4 to Caustic ratio"); return Inx;
      case idNa2CO3toS			: Info.Set(ePT_Double,    "CO3/S",                  "", DC_,     "",       0,      0,  MP_Result|MP_NoFiling|MF_InitHidden,    "Na2CO3 to Soda ratio"); return Inx;
      case idTOC25				  : Info.Set(ePT_Double,    "TOC@25",                 "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Total Organic Carbon Concentration @ 25"); return Inx;
      case idNa2C2O4Conc25	: Info.Set(ePT_Double,    "Oxalate*@25",			      "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "SodiumOxalate Concentration @ 25"); return Inx;
      case idNa2C5O11Conc25	: Info.Set(ePT_Double,    "OrganicO11*@25",         "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "OrganicsO11 Concentration @ 25"); return Inx;
      case idNaClConc25			: Info.Set(ePT_Double,    "Chloride*@25",			      "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "SodiumChloride Concentration @ 25"); return Inx;
      case idNaSO4Conc25		: Info.Set(ePT_Double,    "Sulphate*@25",			      "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "SodiumSulphate Concentration @ 25"); return Inx;
      case idNaFConc25			: Info.Set(ePT_Double,    "Fluoride*@25",           "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "SodiumFluoride Concentration @ 25"); return Inx;
      case idSiO2Conc25			: Info.Set(ePT_Double,    "Silicate*@25",           "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Soluble Silicate Concentration @ 25"); return Inx;
      case idSolidsConc25		: Info.Set(ePT_Double,    "SolidsConc@25",          "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Solids Concentration @ 25");return Inx;

      case idSeparator1			: Info.SetText("..."); return Inx;

      case idCausticConc25	: Info.SetText("--Concentrations @ 25, as Na2CO3 Equiv----");
                              Info.Set(ePT_Double,    "Caustic@25",				      "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Caustic Concentration @ 25"); return Inx;
      case idSodaConc25			: Info.Set(ePT_Double,    "Soda@25",				        "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Soda Concentration @ 25"); return Inx;
      case idCarbonateConc25: Info.Set(ePT_Double,    "Carbonate@25",			      "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "SodiumCarbonate Concentration @ 25"); return Inx;
      case idOrganateConc25	: Info.Set(ePT_Double,    "Organate@25",			      "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling|MF_InitHidden,    "Organate Concentration @ 25"); return Inx;
      case idOrganicO11Conc	: Info.Set(ePT_Double,    "OrganicO11Conc",         "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Organic11 Concentration @ 25"); return Inx;
      case idOxalateConc25	: Info.Set(ePT_Double,    "Oxalate@25",				      "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "SodiumOxalate Concentration @ 25"); return Inx;
      case idTotalOrganics25: Info.Set(ePT_Double,    "TotalOrganics@25",       "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Total Organic Concentration @ 25"); return Inx;
      case idChlorineConc25	: Info.Set(ePT_Double,    "Chloride@25",			      "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "SodiumChloride Concentration @ 25"); return Inx;
      case idSulphateConc25	: Info.Set(ePT_Double,    "Sulphate@25",			      "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "SodiumSulphate Concentration @ 25"); return Inx;
      case idTotalNa25			: Info.Set(ePT_Double,    "TotalNa@25",             "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Total Sodium Concentration @ 25"); return Inx;

      case idSeparator2			: Info.SetText("..."); return Inx;

      case idMR					    : Info.SetText("--Ratio and Conc @ 25, as Na2O Equiv------");
                              Info.Set(ePT_Double,    "MR",                     "", DC_,    "",        0,      0,  MP_Result|MP_NoFiling,    "Mole Flow Ratio of Caustic:Alumina in Liquor phase"); return Inx;
      case idAA					    : Info.Set(ePT_Double,    "AA",                     "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Total Sodium concentration, expressed as grams Na2O/L liquor @ 25ºC"); return Inx;
      case idTT					    : Info.Set(ePT_Double,    "TT",                     "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "NaOH concentration, expressed as grams Na2O/L liquor @ 25ºC"); return Inx;

      case idSeparator3			: Info.SetText("..."); return Inx;

      case idLVolume25			: Info.SetText("--Volume and Density @ 25-----------------");
                              Info.Set(ePT_Double,    "LVolume@25",             "", DC_Qv,   "L/s",    0,      0,  MP_Result|MP_NoFiling,    "Liquor Volumetric flowrate @ 25"); return Inx;
      case idSLVolume25			: Info.Set(ePT_Double,    "SLVolume@25",            "", DC_Qv,   "L/s",    0,      0,  MP_Result|MP_NoFiling,    "Slurry Volumetric flowrate @ 25"); return Inx;
      case idLDensity25 		: Info.Set(ePT_Double,    "LDensity@25",            "", DC_Rho,  "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Liquor Density @ 25"); return Inx;
      case idSLDensity25		: Info.Set(ePT_Double,    "SLDensity@25",           "", DC_Rho,  "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Slurry Density @ 25"); return Inx;

      case idSeparator4			: Info.SetText("..."); return Inx;

      case idAluminaConc		: Info.SetText("--Concentration @ T-----------------------");
                              Info.Set(ePT_Double,    "Alumina@T",				      "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Alumina Concentration @ T"); return Inx;
      case idSodiumCarbonateConc: Info.Set(ePT_Double,    "Carbonate@T",			  "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling|MF_InitHidden,    "SodiumCarbonate Concentration @ T"); return Inx;
      case idTOC				    : Info.Set(ePT_Double,    "TOC@T",                  "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Total Organic Carbon Concentration @ T"); return Inx;
      case idSolidsConc			: Info.Set(ePT_Double,    "SolidsConc@T",           "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Solids Concentration @ T"); return Inx;

      case idSeparator5			: Info.SetText("..."); return Inx;

      case idCausticConc		: Info.SetText("--Concentration @ T, as Na2CO3 Equiv -----");
                              Info.Set(ePT_Double,    "Caustic@T",              "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Caustic Concentration @ T"); return Inx;
      case idSodaConc			  : Info.Set(ePT_Double,    "SodaConc@T",             "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Soda Concentration @ T"); return Inx;

      case idSeparator6			: Info.SetText("..."); return Inx;

      case idAluminaConcSat	: Info.SetText("--Other Bayer Liquor Properties @ T-------");
                              Info.Set(ePT_Double,    "AluminaSatConc",         "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Alumina Saturation Concentration @ T"); return Inx;
      case idAtoCSaturation	: Info.Set(ePT_Double,    "ASat_To_C",              "", DC_,     "",       0,      0,  MP_Result|MP_NoFiling,    "Alumina Saturation to Caustic ratio @ T"); return Inx;
      case idIonicStrength	: Info.Set(ePT_Double,    "IonicStrength",          "", DC_,     "",       0,      0,  MP_Result|MP_NoFiling|MF_InitHidden,       "Ionic strength used in A Sat Calcs"); return Inx;
      case idSSNRatio			  : Info.Set(ePT_Double,    "SSNRatio",               "", DC_,     "",       0,      0,  MP_Result|MP_NoFiling,    "A/C actual to ASat/C ratio @ T"); return Inx;
      case idTHAMassFlow   	: Info.Set(ePT_Double,    "THAMassFlow",            "", DC_Qm,   "kg/s",   0,      0,  MP_Result|MP_NoFiling,    "THA flow rate"); return Inx;
      case idTHADens			  : Info.Set(ePT_Double,    "THADens",                "", DC_Rho,  "kg/m^3", 0,      0,  MP_Result|MP_NoFiling,    "THA Density"); return Inx;
      case idOxalateSolubility: Info.Set(ePT_Double,  "OxalateSolubility",      "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Oxalate Equilibrium Concentration @ T"); return Inx;
      case idBoehmiteSolubility: Info.Set(ePT_Double, "BoehmiteSolubility",     "", DC_Conc, "g/L",    0,      0,  MP_Result|MP_NoFiling,    "Boehmite Solubility Concentration @ T"); return Inx;
      case idBoilPtElev			: Info.Set(ePT_Double,    "BoilPtElev",             "", DC_dT,   "C",      0,      0,  MP_Result|MP_NoFiling,    "Boiling Point Elevation"); return Inx;

      case idMPI_EndOfProps	: return MPI_EndOfProps;    
      }
    return MPI_MoreProps;
    }

//---------------------------------------------------------------------------

DWORD ASMBayer::GetPropertyVisibility(long Index)
  {//determine visibility of list of all properties
  DWORD Vis = ePVis_All;
  switch (Index-SpModelEx::DefinedPropertyCount())
    {
    case idCarbonFactor: Vis = (sm_iASatMethod==ASM_May2003 || sm_iASatMethod==ASM_Hyprod) ? ePVis_All : 0; break;
    /*case idDefnLiqMethod: 
    case idRqd_A_to_C: 
    case idRqd_C_to_S: 
    case idRqd_C: 
    case idRqd_Organic: 
    case idRqd_TOC: 
    case idRqd_OrgRatio: 
    case idRqd_Oxalate: 
    case idRqd_Na2SO4: 
    case idRqd_NaCl: 
    case idRqd_NaF: 
    case idRqd_SiO2: 
    case idRqd_SolConc: 
    case idRqd_SolFrac: Vis = fDoCalc ? ePVis_All : (ePVis_DynFull|ePVis_DynFlow|ePVis_Probal|ePVis_File); break;*/
     default:  Vis = ePVis_All; 
    }
  Vis &= (~ePVis_View);//TODO: Add option for user to switch visibility for this to go on/off
  return Vis;//ePVis_All; 
  };

//---------------------------------------------------------------------------

void ASMBayer::GetPropertyValue(long Index, ULONG Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {//define method of retrieving values for list of all properties
  switch (Index-SpModelEx::DefinedPropertyCount())
    {
    case idDensityMethod		: Value=sm_iDensityMethod;                  return; 
    case idASatMethod			  : Value=sm_iASatMethod;                     return; 
    case idCPMethod        	: Value=sm_iCpMethod;						            return; 
    case idBPEMethod        : Value=sm_iBPEMethod;						          return; 
    case idGrowthMethod			: Value=sm_iGrowthMethod;                   return; 
    case idMinSolCp				  : Value=ASMBayer::sm_dMinSolCp;             return; 
    case idMorphology			  : Value=ASMBayer::sm_Morphology;            return; 
    case idNucleation			  : Value=ASMBayer::sm_Nucleation;            return; 
    case idNa2OFactor			  : Value=ASMBayer::sm_Na2OFactor;            return; 
    case idCarbonFactor			: Value=ASMBayer::sm_CarbonFactor;          return; 

/*    case idDefineLiquor			: Value=fDoCalc;                            return; 
    case idDefnLiqMethod		: Value=sm_iRqdCalcMethod;                  return; 
    case idRqd_A_to_C			  : Value=dRqd_AtoC;                          return;
    case idRqd_C_to_S			  : Value=dRqd_CtoS;                          return;
    case idRqd_C				    : Value=dRqd_C;                             return;
    case idRqd_Organic			: Value=dRqd_Org;                           return;
    case idRqd_TOC				  : Value=dRqd_Org*MW_C/::SodiumCarbonate.MW*(5.0 - (3.0 * dRqd_OrgRatio));   return;
    case idRqd_OrgRatio			: Value=dRqd_OrgRatio;                                                      return;
    case idRqd_Oxalate			: Value=dRqd_Org * dRqd_OrgRatio;                                           return;
    case idRqd_Na2SO4			  : Value=dRqd_Sul;                           return;
    case idRqd_NaCl				  : Value=dRqd_Salt;                          return;
    case idRqd_NaF			    : Value=dRqd_Fl;                            return;
    case idRqd_SiO2				  : Value=dRqd_Sil;                           return;
    case idRqd_SolConc			: Value=dRqd_SolConc;                       return;
    case idRqd_SolFrac			: Value=dRqd_SolFrac;                       return;*/

    case idTHAMassFlow			: Value=THAMassFlow();                      return; 
    case idTHADens				  : Value=THADens(T);                         return; 
    case idCausticConc			: Value=CausticConc(T);                     return; 
    case idAluminaConc			: Value=AluminaConc(T);                     return; 
    case idSodaConc				  : Value=SodaConc(T);                        return; 
    case idSodiumCarbonateConc	: Value=SodiumCarbonateConc(T);         return; 

    case idCausticConc25		: Value=CausticConc(C_2_K(25.0));           return; 
    case idAluminaConc25		: Value=AluminaConc(C_2_K(25.0));           return; 
    case idSodaConc25			  : Value=SodaConc(C_2_K(25.0));              return; 

    case idCarbonateConc25	: Value=SodiumCarbonateConc(C_2_K(25.0));   return; 
    case idNaClConc25			  : Value=ChlorineConc(); return;//LiqConcs25.Liq[::SodiumChloride.LiqPhInx()]*::SodiumChloride.MW*2/::SodiumCarbonate.MW;   return; 
    case idNaSO4Conc25			: Value=SulphateConc(); return;//LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()]*::SodiumSulphate.MW/::SodiumCarbonate.MW;   return; 
    case idNa2C2O4Conc25		: Value=LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]*::SodiumOxalate.MW/::SodiumCarbonate.MW;    return; 
    case idSolidsConc			  : Value=SolidsConc(T);                      return; 
    case idSolidsConc25			: Value=SolidsConc(C_2_K(25.0));            return; 
    case idTOC					    : Value=TOC(T);                             return; 
    case idTOC25				    : Value=TOC(C_2_K(25.0));							      return; 

    case idAtoC					    : Value=AtoC();                             return; 
    case idCtoS					    : Value=CtoS();                             return; 
    case idCltoC				    : Value=CltoC();                            return; 
    case idNa2SO4toC			  : Value=Na2SO4toC();                        return; 
    case idNa2CO3toS			  : Value=Na2CO3toS();                        return; 

    case idBoilPtElev			  : Value=BoilPtElev(T);                      return; 
    case idLVolume25			  : Value=LVolume25();                        return; 
    case idSLVolume25			  : Value=SLVolume25();                       return; 
    case idOrganateConc25		: Value=OrganateConc();                     return; 
    case idOxalateConc25		: Value=SodiumOxalateConc(C_2_K(25.0));     return; 
    case idTotalOrganics25	: Value=TotalOrganics();                    return; 
    case idChlorineConc25		: Value=ChlorineConc();                     return; 
    case idSulphateConc25		: Value=SulphateConc();                     return; 
    case idTotalNa25			  : Value=TotalSodiumConc();                  return; 

    case idAluminaConcSat		: Value=AluminaConcSat(T);                  return; 
    case idOxalateSolubility: Value=OxalateSolubility(T);               return; 
    case idBoehmiteSolubility: Value=BoehmiteSolubility(T);             return; 
    case idAtoCSaturation		: Value=AtoCSaturation(T);                  return; 
    case idSSNRatio				  : Value=SSNRatio(T);                        return; 

    case idLDensity25			  : Value=LDensity25();                       return; 
    case idSLDensity25			: Value=SLDensity25();                      return; 

    case  idMR					    : Value=MR();                               return; 
    case  idAA					    : Value=AA();                               return; 
    case  idTT					    : Value=TT();                               return; 
    case  idOrganicO11Conc	: Value=OrganicO11Conc();                   return; 
    case  idNa2C5O11Conc25	: Value=OrganicO11_Conc();                  return; 
    case  idNaFConc25			  : Value=Fluoride_Conc();                    return; 
    case  idSiO2Conc25			: Value=Silicate_Conc();                    return; 
    case  idIonicStrength		: Value=IonicStrength();					          return;
		/*  case  73: Value=GrowthRate();                       return; 
    case  74: Value=AgglomerationRate();                return; 
    case  75: Value=ASMBayer::sm_Na2OFactor;            return; */
    };
  };

//---------------------------------------------------------------------------

void ASMBayer::PutPropertyValue(long Index, MPropertyValue & Value)
  {//define method of setting values for list of all properties
  switch (Index-SpModelEx::DefinedPropertyCount())
    {
    case idDensityMethod  : sm_iDensityMethod=(byte)(long)Value;  return;
    case idASatMethod		  : sm_iASatMethod=(byte)(long)Value;     return;
    case idCPMethod       : sm_iCpMethod=(byte)(long)Value;		    return; 
    case idBPEMethod      : sm_iBPEMethod=(byte)(long)Value;		  return; 
    case idGrowthMethod	  : sm_iGrowthMethod=(byte)(long)Value;   return;
    case idMinSolCp			  : ASMBayer::sm_dMinSolCp=Value;            return;
    case idMorphology		  : ASMBayer::sm_Morphology=Value;        return;
    case idNucleation		  : ASMBayer::sm_Nucleation=Value;        return;
    case idNa2OFactor		  : ASMBayer::sm_Na2OFactor=Value;        return;
    case idCarbonFactor	  : ASMBayer::sm_CarbonFactor=Value;      return;

/*    case  idDefineLiquor  : fDoCalc=Value;                        return;
    case  idDefnLiqMethod : sm_iRqdCalcMethod=(byte)(long)Value;  return;
                                    
    case  idRqd_A_to_C    : dRqd_AtoC=Value;                      return;
    case  idRqd_C_to_S    : dRqd_CtoS=Value;                      return;
    case  idRqd_C         : dRqd_C=Value;                         return;
    case  idRqd_Organic   : dRqd_Org=Value;                       return;
    case  idRqd_TOC: if (sm_iRqdCalcMethod==DLM_TOC)
                {//OxConc must remain constant
                double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989
                double PrevOx = dRqd_Org * dRqd_OrgRatio;
                dRqd_Org = ((double(Value)*MW_Na2CO3/MW_C) + (3.0 * PrevOx))/5.0;
                dRqd_OrgRatio = PrevOx/GTZ(dRqd_Org);
                }
              return;
    case  idRqd_OrgRatio: if (/*!DCB.ForFileSnpScn() &&*//* sm_iRqdCalcMethod==DLM_TOC)
                {
                const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989
                double PrevTOC = dRqd_Org*MW_C/::SodiumCarbonate.MW*(5.0 - (3.0 * dRqd_OrgRatio));
                dRqd_OrgRatio = Range(0.0, double(Value), 1.0);
                dRqd_Org = PrevTOC*::SodiumCarbonate.MW/MW_C/(5.0 - (3.0 * dRqd_OrgRatio));
                }
              else
                dRqd_OrgRatio = Range(0.0, double(Value), 1.0);
              return;
    case  idRqd_Oxalate: if (sm_iRqdCalcMethod==DLM_TOC)
                {//TOC must remain constant
                const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989
                double PrevTOC = dRqd_Org*MW_C/::SodiumCarbonate.MW*(5.0 - (3.0 * dRqd_OrgRatio));
                dRqd_OrgRatio = 5.0/GTZ((PrevTOC/GTZ(double(Value))*::SodiumCarbonate.MW/MW_C) + 3.0);
                dRqd_Org = PrevTOC*::SodiumCarbonate.MW/MW_C/(5.0 - (3.0 * dRqd_OrgRatio));
                }
              return;
    case idRqd_Na2SO4		: dRqd_Sul=Value;                       return;
    case idRqd_NaCl			: dRqd_Salt=Value;                      return;
    case idRqd_NaF			: dRqd_Fl=Value;                        return;
    case idRqd_SiO2			: dRqd_Sil=Value;                       return;
    case idRqd_SolConc	: dRqd_SolConc=Value;                   return;
    case idRqd_SolFrac	: dRqd_SolFrac=Value;                   return;*/
    };
  };


/*long ASMBayer::DefinedPropertyCount() 
  {
  return 58 + SpModelEx::DefinedPropertyCount();
  }

long ASMBayer::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {
  long Inx=Index-SpModelEx::DefinedPropertyCount();
  switch(Inx)
    {
    case   0: Info.SetStructName("Props");
              Info.Set(ePT_Double,    "THAMassFlow",              "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case   1: Info.Set(ePT_Double,    "THADens",                  "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case   2: Info.Set(ePT_Double,    "CausticConc",              "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case   3: Info.Set(ePT_Double,    "AluminaConc",              "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case   4: Info.Set(ePT_Double,    "SodaConc",                 "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case   5: Info.Set(ePT_Double,    "SodiumCarbonateConc",      "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case   6: Info.Set(ePT_Double,    "SodiumSulphateConc",       "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case   7: Info.Set(ePT_Double,    "SodiumOxalateConc",        "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case   8: Info.Set(ePT_Double,    "SolidsConc",               "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case   9: Info.Set(ePT_Double,    "AluminaConcSat",           "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  10: Info.Set(ePT_Double,    "OxalateEquilibrium",       "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  11: Info.Set(ePT_Double,    "AtoCSaturation",           "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  12: Info.Set(ePT_Double,    "SSNRatio",                 "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  13: Info.Set(ePT_Double,    "TOOC",                     "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  14: Info.Set(ePT_Double,    "TOC",                      "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  15: Info.Set(ePT_Double,    "AtoC",                     "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  16: Info.Set(ePT_Double,    "CtoS",                     "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  17: Info.Set(ePT_Double,    "CltoC",                    "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  18: Info.Set(ePT_Double,    "TOOCtoC",                  "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  19: Info.Set(ePT_Double,    "Na2SO4toC",                "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  20: Info.Set(ePT_Double,    "Na2CO3toS",                "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  21: Info.Set(ePT_Double,    "MR",                       "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  22: Info.Set(ePT_Double,    "AA",                       "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  23: Info.Set(ePT_Double,    "TT",                       "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  24: Info.Set(ePT_Double,    "BoundSodaFrac",            "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  25: Info.Set(ePT_Double,    "BoundSodaOrgFrac",         "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  26: Info.Set(ePT_Double,    "LVolume25",                "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  27: Info.Set(ePT_Double,    "SLVolume25",               "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  28: Info.Set(ePT_Double,    "TotalSodiumConc",          "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  29: Info.Set(ePT_Double,    "OrganateConc",             "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  30: Info.Set(ePT_Double,    "OrganicO11Conc",           "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  31: Info.Set(ePT_Double,    "TotalOrganics",            "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  32: Info.Set(ePT_Double,    "ChlorineConc",             "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  33: Info.Set(ePT_Double,    "SulphateConc",             "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  34: Info.Set(ePT_Double,    "SodiumOxalate_Conc",       "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  35: Info.Set(ePT_Double,    "OrganicO11_Conc",          "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  36: Info.Set(ePT_Double,    "Sulphate_Conc",            "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  37: Info.Set(ePT_Double,    "TOOC_",                    "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  38: Info.Set(ePT_Double,    "Chlorine_Conc",            "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  39: Info.Set(ePT_Double,    "Fluoride_Conc",            "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  40: Info.Set(ePT_Double,    "Silicate_Conc",            "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  41: Info.Set(ePT_Double,    "TotalSodium",              "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,    "A Desc"); return Inx;
    case  42: Info.Set(ePT_Double,    "BoilPtElev",               "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  43: Info.Set(ePT_Double,    "GrowthRate",               "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  44: Info.Set(ePT_Double,    "AgglomerationRate",        "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  45: Info.Set(ePT_Double,    "Na2OFactor",               "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    //case  46: Info.Set(ePT_Double,    "PartDiamFromSAM",          "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    //case  47: Info.Set(ePT_Double,    "PartNumPerSec",            "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    //case  48: Info.Set(ePT_Double,    "PartNumPerLiter",          "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    //case  49: Info.Set(ePT_Double,    "SpecificSurfaceAreaVol",   "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    //case  50: Info.Set(ePT_Double,    "SpecificSurfaceAreaMass",  "", DC_,      "",     0,      0,  MP_Result|MP_NoFiling,       "A Desc"); return Inx;
    case  51: Info.Set(ePT_Double,    "BPEFactor",                "", DC_,      "",     0,      0,  MP_Parameter,       "A Desc"); return Inx;
    case  52: Info.Set(ePT_Double,    "RhoFactor",                "", DC_,      "",     0,      0,  MP_Parameter,       "A Desc"); return Inx;
    case  53: Info.Set(ePT_Double,    "MinSolCp",                 "", DC_CpMs,  "",     0,      0,  MP_Parameter,       "A Desc"); return Inx;
    case  54: Info.Set(ePT_Double,    "Morphology",               "", DC_,      "",     0,      0,  MP_Parameter,       "A Desc"); return Inx;
    case  55: Info.Set(ePT_Double,    "Nucleation",               "", DC_,      "",     0,      0,  MP_Parameter,       "A Desc"); return Inx;
    case  56: Info.Set(ePT_Double,    "Na2OFactor",               "", DC_,      "",     0,      0,  MP_Parameter,       "A Desc"); return Inx;
    case  57: Info.Set(ePT_Double,    "CarbonFactor",             "", DC_,      "",     0,      0,  MP_Parameter,       "A Desc"); return Inx;
    }
  return SpModelEx::DefinedPropertyInfo(Index, Info);
  };
                                                                                           
DWORD ASMBayer::GetPropertyVisibility(long Index)
  { 
  return SpModelEx::GetPropertyVisibility(Index); 
  };
*/                                                                                           
//void ASMBayer::GetPropertyValue(long Index, PhMask Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
/*  {                                                                                        
  switch (Index-SpModelEx::DefinedPropertyCount())                                                          
    {                                                                                   
    case   0: Value=THAMassFlow();                return ;
    case   1: Value=THADens(T);                   return ;
    case   2: Value=CausticConc(T);               return ;
    case   3: Value=AluminaConc(T);               return ;
    case   4: Value=SodaConc(T);                  return ;
    case   5: Value=SodiumCarbonateConc(T);       return ;
    case   6: Value=SodiumSulphateConc(T);        return ;
    case   7: Value=SodiumOxalateConc(T);         return ;
    case   8: Value=SolidsConc(T);                return ;
    case   9: Value=AluminaConcSat(T);            return ;
    case  10: Value=OxalateEquilibrium(T);        return ;
    case  11: Value=AtoCSaturation(T);            return ;
    case  12: Value=SSNRatio(T);                  return ;
    case  13: Value=TOOC(T);                      return ;
    case  14: Value=TOC(T);                       return ;
    case  15: Value=AtoC();                       return ;
    case  16: Value=CtoS();                       return ;
    case  17: Value=CltoC();                      return ;
    case  18: Value=TOOCtoC();                    return ;
    case  19: Value=Na2SO4toC();                  return ;
    case  20: Value=Na2CO3toS();                  return ;
    case  21: Value=MR();                         return ;
    case  22: Value=AA();                         return ;
    case  23: Value=TT();                         return ;
    case  24: Value=BoundSodaFrac();              return ;
    case  25: Value=BoundSodaOrgFrac();           return ;
    case  26: Value=LVolume25();                  return ;
    case  27: Value=SLVolume25();                 return ;
    case  28: Value=TotalSodiumConc();            return ;
    case  29: Value=OrganateConc();               return ;
    case  30: Value=OrganicO11Conc();             return ;
    case  31: Value=TotalOrganics();              return ;
    case  32: Value=ChlorineConc();               return ;
    case  33: Value=SulphateConc();               return ;
    case  34: Value=SodiumOxalate_Conc();         return ;
    case  35: Value=OrganicO11_Conc();            return ;
    case  36: Value=Sulphate_Conc();              return ;
    case  37: Value=TOOC_(T);                     return ;
    case  38: Value=Chlorine_Conc();              return ;
    case  39: Value=Fluoride_Conc();              return ;
    case  40: Value=Silicate_Conc();              return ;
    case  41: Value=TotalSodium();                return ;
    case  42: Value=BoilPtElev(T);                return ;
    case  43: Value=GrowthRate();                 return ;
    case  44: Value=AgglomerationRate();          return ;
    case  45: Value=Na2OFactor();                 return ;
    //case  46: Value=PartDiamFromSAM();            return ;
    //case  47: Value=PartNumPerSec();              return ;
    //case  48: Value=PartNumPerLiter();            return ;
    //case  49: Value=SpecificSurfaceAreaVol();     return ;
    //case  50: Value=SpecificSurfaceAreaMass();    return ;
    case  51: Value=ASMBayer::dBPEFactor;         return ;
    case  52: Value=ASMBayer::dRhoFactor;         return ;
    case  53: Value=ASMBayer::dMinSolCp;          return ;
    case  54: Value=ASMBayer::sm_Morphology;      return ;
    case  55: Value=ASMBayer::sm_Nucleation;      return ;
    case  56: Value=ASMBayer::sm_Na2OFactor;      return ;
    case  57: Value=ASMBayer::sm_CarbonFactor;    return ;
    };
  SpModelEx::GetPropertyValue(Index, Phase, T, P, Value);
  };

void ASMBayer::PutPropertyValue(long Index, MPropertyValue & Value)
  {                                                                                        
  switch (Index-SpModelEx::DefinedPropertyCount())                                                          
    {                                                                                   
    case  51: ASMBayer::dBPEFactor=Value; return;     
    case  52: ASMBayer::dRhoFactor=Value; return;      
    case  53: ASMBayer::dMinSolCp=Value; return;       
    case  54: ASMBayer::sm_Morphology=Value; return;
    case  55: ASMBayer::sm_Nucleation=Value; return;   
    case  56: ASMBayer::sm_Na2OFactor=Value; return;  
    case  57: ASMBayer::sm_CarbonFactor=Value; return; 
    };
  SpModelEx::PutPropertyValue(Index, Value);
  };
*/
#endif
// ==========================================================================
//
//                                  
//
// ==========================================================================

ASMSzPersonality::ASMSzPersonality(SQSzDist1 *pQ) : 
  SpQPersonality(pQ->pModel),
  m_Q(*pQ)
  {
  m_eType = eAlcanPSD_Rosin;

  m_Rqd.m_dMedian  = 50*1e-6;  //dNAN;
  m_Rqd.m_dd10     = dNAN;
  m_Rqd.m_dd90     = dNAN;
  m_Rqd.m_dq1      = dNAN;
  m_Rqd.m_dq3      = dNAN;
  m_Rqd.m_dQS      = 20*1e-6;  //dNAN;
  m_Rqd.m_dCrit    = 1e-6;

  m_Rqd.m_dm149    = dNAN;
  m_Rqd.m_dm106    = dNAN;
  m_Rqd.m_dm74     = dNAN;
  m_Rqd.m_dm53     = dNAN;
  m_Rqd.m_dm45     = dNAN;
  m_Rqd.m_dm33     = dNAN;
  m_Rqd.m_dm20     = dNAN;
  };

// --------------------------------------------------------------------------

void ASMSzPersonality::BuildDataDefn(DataDefnBlk & DDB)
  {
  //DDB.Page("ASz");


  DDB.BeginObject(m_pModel, "ASz", "AlcanSz", NULL, DDB_RqdPage, -1, 0);

  static DDBValueLst DDBPSDTypes[]={
    {eAlcanPSD_Log   , "Logrithmic"},
    {eAlcanPSD_Rosin , "Rosin-Rammler"},
    {eAlcanPSD_Table , "Table"},
    //{eAlcanPSD_File  , "File"},
    {0}};

  DDB.Text("");
  DDB.Text("Rqd Characteristics");
  DDB.Long  ("PSD_Type", "", DC_, "", (long*)&m_eType,  m_pModel, isParm, DDBPSDTypes);
  DDB.BeginStruct(m_pModel, "Rqd");
  DDB.Double("d10",      "", DC_L,    "um", &m_Rqd.m_dd10,     m_pModel, isParm|NAN_OK);
  //DDB.Double("q1",       "", DC_L,    "um", &m_Rqd.m_dq1,      m_pModel, isParm|NAN_OK); not used
  DDB.Double("Median",   "", DC_L,    "um", &m_Rqd.m_dMedian,  m_pModel, isParm|NAN_OK);
  //DDB.Double("q3",       "", DC_L,    "um", &m_Rqd.m_dq3,      m_pModel, isParm|NAN_OK); not used
  DDB.Double("d90",      "", DC_L,    "um", &m_Rqd.m_dd90,     m_pModel, isParm|NAN_OK);
  DDB.Double("QS",       "", DC_L,    "um", &m_Rqd.m_dQS,      m_pModel, isParm|NAN_OK);
  DDB.Double("DCrit",    "", DC_L,    "um", &m_Rqd.m_dCrit,    m_pModel, isParm);
  DDB.Text("");
  DDB.Double("m20",      "", DC_Frac, "%",  &m_Rqd.m_dm20,     m_pModel, isParm|NAN_OK);
  DDB.Double("m33",      "", DC_Frac, "%",  &m_Rqd.m_dm33,     m_pModel, isParm|NAN_OK);
  DDB.Double("m45",      "", DC_Frac, "%",  &m_Rqd.m_dm45,     m_pModel, isParm|NAN_OK);
  DDB.Double("m53",      "", DC_Frac, "%",  &m_Rqd.m_dm53,     m_pModel, isParm|NAN_OK);
  DDB.Double("m74",      "", DC_Frac, "%",  &m_Rqd.m_dm74,     m_pModel, isParm|NAN_OK);
  DDB.Double("m106",     "", DC_Frac, "%",  &m_Rqd.m_dm106,    m_pModel, isParm|NAN_OK);
  DDB.Double("m149",     "", DC_Frac, "%",  &m_Rqd.m_dm149,    m_pModel, isParm|NAN_OK);
  DDB.EndStruct();
  DDB.Text("");
  DDB.Button("Set", "", DC_, "", xidSetSzSpecs, m_pModel, isParm);

  DDB.EndObject();

  };

// --------------------------------------------------------------------------

flag ASMSzPersonality::DataXchg(DataChangeBlk & DCB)
  {
  //dbgpln("%i", DCB.lHandle);
  if (DCB.lHandle==xidSetSzSpecs)
    {
    if (DCB.rB && (*DCB.rB!=0))
      {
      SetDistribution();
//      ComputeMedian();
      };
    DCB.B=0;
    return 1;
    };
  return 0;
  };

// --------------------------------------------------------------------------

void ASMSzPersonality::SetDistribution()
  {
  double D25, D75, b, QS1;
  long Count;
  double Diam[MaxHPSizeClasses+1];  //Regression vector
  double wPrct[MaxHPSizeClasses+1]; //Regression vector
  double RegRslt[4];                //Regression result
  //DiamWeightData : variant ;        //Diam et % < que D : used to sort data
  //SortKey : variant ;
  //s1,s2 : string;

  #if dbgModels
  if (dbgSetSizeData ())
    dbgpln("-----------------------------------");
  #endif
  Count = 0 ;
  //Verifie si toutes les donnees PSD sont definie (Voir deuxieme tableau Dlg)
  if (0)//not allPSDDefine 
    {
    //Lecture et verification de la m_dMedian
    if ((m_Rqd.m_dMedian <= 0) && ((m_Rqd.m_dm45 <= 0) || (m_Rqd.m_dm74 <= 0))) 
      {
      //if MessageDlg (hyprod.GetMessage(45), mtInformation,[mbOK], 0) = mrOK 
      //  {
      //    F1Book1.free;
      //    raise EPsdInvalid.Create('');
      //    Exit
      //  };
      //SetCI();
      return;
      }
    };
  
  //Verification de la médiane
  if (Valid(m_Rqd.m_dMedian)) 
    {
    Diam[Count]  = m_Rqd.m_dMedian;
    wPrct[Count] = 0.50;
    Count++;
    };

  //Verification m_dm45
  if (Valid(m_Rqd.m_dm45) && (m_Rqd.m_dm45 > 0) && (m_Rqd.m_dm45 < 0.9999)) 
    {
    Diam[Count]  = 45*1e-6;
    wPrct[Count] = m_Rqd.m_dm45;
    Count++;
    };
  //Verification m_dm74
  if (Valid(m_Rqd.m_dm74) && (m_Rqd.m_dm74 > 0) && (m_Rqd.m_dm74 < 0.9999)) 
    {
    Diam[Count]  = 74*1e-6;
    wPrct[Count] = m_Rqd.m_dm74;
    Count++;
    };
  //Verification d10
  if (Valid(m_Rqd.m_dd10) && (m_Rqd.m_dd10 > 0)) 
    {
    Diam[Count]  = m_Rqd.m_dd10;
    wPrct[Count] = 0.10;
    Count++;
    };
  //Verification d90
  if (Valid(m_Rqd.m_dd90) && (m_Rqd.m_dd90 > 0)) 
    {
    Diam[Count]  = m_Rqd.m_dd90;
    wPrct[Count] = 0.90;
    Count++;
    };
  /*if (Valid(m_Rqd.m_dq1) && Valid(m_Rqd.m_dq3))
    {
    m_Rqd.m_dQS = m_Rqd.m_dq3 - m_Rqd.m_dq1;
    }*/
  if (Valid(m_Rqd.m_dQS) && Valid(m_Rqd.m_dMedian)) 
    {
    // Verification du QS et determine Q1 Q3
    if (m_eType == eAlcanPSD_Log) 
      {                               // Dist. Log-Normale
      // D75 = ( qs + sqrt( qs*qs + 4*d50*d50)) / 2.0 et D25 = (D75 - qs)
      D75 = (m_Rqd.m_dQS + Sqrt(m_Rqd.m_dQS*m_Rqd.m_dQS + 4*m_Rqd.m_dMedian*m_Rqd.m_dMedian))/ 2.;
      D25 = D75 - m_Rqd.m_dQS;
      Diam[Count]  = D25 ;
      wPrct[Count] = 0.25;
      Count++;
      Diam[Count]  = D75 ;
      wPrct[Count] = 0.75;
      Count++;
      }
    else
      {
      //construire //DiamWeightData pour QS > 0, treatment=1}
      D75 = m_Rqd.m_dMedian + m_Rqd.m_dQS / 2.0;               // initialisation
      do
        {
        b   = 0.69314718 / log(D75/m_Rqd.m_dMedian);
        D25 = exp (log(m_Rqd.m_dMedian) - 0.879386403/b);
        QS1 = D75 - D25 ;
        D75 = D75 - (QS1-m_Rqd.m_dQS) / 2.0;
        }
      while ((fabs((QS1-m_Rqd.m_dQS) / m_Rqd.m_dQS)) > 0.000001);

      Diam[Count]  = D25 ;
      wPrct[Count] = 0.25;
      Count++;
      Diam[Count]  = D75 ;
      wPrct[Count] = 0.75;
      Count++;
      LogNote("Set PSD", 0, "Diam[0]:%.12g  Diam[1]:%.12g  Diam[2]:%.12g", Diam[0], Diam[1], Diam[2]);
      }
    };

  // Seulement si toutes les donnees du PSD sont definies
  //(Voir Deuxieme tableau Dlg)
  if (1)//AllPSDDefine 
    {
    if (Valid(m_Rqd.m_dm149) && (m_Rqd.m_dm149 > 0) && (m_Rqd.m_dm149 < 0.9999))
      {
      Diam[Count]  = 149*1e-6;
      wPrct[Count] = m_Rqd.m_dm149;
      Count++;
      };
    if (Valid(m_Rqd.m_dm106) && (m_Rqd.m_dm106 > 0) && (m_Rqd.m_dm106 < 0.9999))
      {
      Diam[Count]  = 106*1e-6;
      wPrct[Count] = m_Rqd.m_dm106;
      Count++;
      };
    if (Valid(m_Rqd.m_dm53) && (m_Rqd.m_dm53 > 0)&& (m_Rqd.m_dm53 < 0.9999))
      {
      Diam[Count]  = 53*1e-6;
      wPrct[Count] = m_Rqd.m_dm53;
      Count++;
      };
    if (Valid(m_Rqd.m_dm33) && (m_Rqd.m_dm33 > 0) && (m_Rqd.m_dm33 <0.9999))
      {
      Diam[Count]  = 33*1e-6;
      wPrct[Count] = m_Rqd.m_dm33;
      Count++;
      };
    if (Valid(m_Rqd.m_dm20) && (m_Rqd.m_dm20 > 0) && (m_Rqd.m_dm20 < 0.9999))
      {
      Diam[Count]  = 20*1e-6;
      wPrct[Count] = m_Rqd.m_dm20;
      Count++;
      };
    };
  if (Count>=2)
    {
    switch (m_eType)
      {
      case eAlcanPSD_Log:
        {
        RegressLN (Count, Diam, wPrct, RegRslt);
        HyprodDataFromLN (RegRslt ) ;
        //TableRsltFromLN (RegRslt) ;
        break;
        };
      case eAlcanPSD_Rosin:
        {
        RegressRR(Count, Diam, wPrct, RegRslt);
        HyprodDataFromRR (RegRslt) ;
        //TableRsltFromRR (RegRslt) ;
        break;
        };
      case eAlcanPSD_Table:
        {
        HyprodDataFromTable(Count, Diam, wPrct, RegRslt);
        break;
        }
      }
    }
  else
    {
    LogWarning("Set PSD", 0, "Insufficient data to set PSD");
    }
  //F1Book1.free;
  };

// --------------------------------------------------------------------------

//void ASMSzPersonality::ComputeMedian(long NIntervals, double SizeClass[], double WCumul[])
//  {
//  //long      N;
//  //double    limit, MM, bb, M, NparTot, S;
//  //
//  //   class_size, summ: vps;
//
//
//  if (1) //(ps[10] >0) or (ps[20] >0) or (ps[30] >0)
//    {
//    double MeasDiams[]={20,33,45,53,74,106,149};
//    const  long NDMeas=sizeof(MeasDiams)/sizeof(MeasDiams[0]);
//    double MResult[NDMeas];
//    long i=0;
//    for (int j=0; j<NDMeas; j++)
//      {
//      while ((i<NIntervals) && (SizeClass[i]<MeasDiams[j]*1e-6)) 
//        i++; 
//      MResult[j]=WCumul[i-1]+(WCumul[i]-WCumul[i-1])*(MeasDiams[j]*1e-6-SizeClass[i-1])/GTZ(SizeClass[i]-SizeClass[i-1]);
//      }
//    
//    //m_Meas.m_dm20  = MResult[0];
//    //m_Meas.m_dm33  = MResult[1];
//    //m_Meas.m_dm45  = MResult[2];
//    //m_Meas.m_dm53  = MResult[3];
//    //m_Meas.m_dm74  = MResult[4];
//    //m_Meas.m_dm106 = MResult[5];
//    //m_Meas.m_dm149 = MResult[6];
//
//    double MeasWCumuls[]={10,25,50,75,90};
//    const  long NWMeas=sizeof(MeasWCumuls)/sizeof(MeasWCumuls[0]);
//    double WResult[NWMeas];
//
//    i=0;
//    for (j=0; j<NWMeas; j++)
//      {
//      while ((i<NIntervals) && (WCumul[i]<MeasWCumuls[j]*0.01)) 
//        i++; 
//      WResult[j]=SizeClass[i-1]+(SizeClass[i]-SizeClass[i-1])*(MeasWCumuls[j]*0.01-WCumul[i-1])/GTZ(WCumul[i]-WCumul[i-1]);
//      }
//
//    m_Meas.m_dd10     = WResult[0];
//    m_Meas.m_dq1      = WResult[1];
//    m_Meas.m_dMedian  = WResult[2];
//    m_Meas.m_dq3      = WResult[3];
//    m_Meas.m_dd90     = WResult[4];
//    m_Meas.m_dQS      = m_Meas.m_dq3-m_Meas.m_dq1;
//       
////    M=0; 
////    NparTot=0;
////    S=0;
////    for (i=0; i<NIntervals; i++)
////      {
////      M   = M +ps[N]* power(CLASS_SIZE[N],3);
////      NparTot= NparTot +ps[N];
////      S   = S + sqr(class_size[N])*ps[N];
////      }
////    M = M*PI*2.42/6E12;
////    Ssurf= S*PI/( M *1E12);
////    ppg= NparTot/ M;
////    NNTL=NparTot;
//    };
//  };

// --------------------------------------------------------------------------

double ASMSzPersonality::NormInv(double x)
  {
  //Cette fonction retourne l'écart type qui sera dépassé avec une
  //probabilité "x" (>0, <1) lorsque la distribution est normale.
  const double c0 = 2.515517;
  const double c1 = 0.802853;
  const double c2 = 0.010328;
  const double d1 = 1.432788;
  const double d2 = 0.189269;
  const double d3 = 0.001308;

  double t, Y;
  if ((x <= 0 ) || (x >= 1.0))// error 
    {
    //MessageDlg(Hyprod.GetMessage(50), mtError, [mbOK], 0);
    //raise EPsdInvalid.Create('');
    return 0;
    };
  if ((x > 0.0) && (x <= 0.5))
    {
    t = Sqrt(log(1/x/x));
    Y = t - ((c2*t + c1)*t + c0)/(((d3*t + d2)*t +d1)*t +1);
    return -Y;
    }
  else //  x > 0.5 and < 1.0  
    {
    t = Sqrt(log(1/((1-x)*(1-x))));
    Y = t - ((c2*t + c1)*t + c0)/(((d3*t + d2)*t +d1)*t +1);
    return Y;
    }
  return 0;
  }

// --------------------------------------------------------------------------

double ASMSzPersonality::NormDist(double x)
  {
  //Cette fonction retourne la probabilité P (comprise entre 0 et 1) qu'une variable
  //distribuée normalement soit inférieure à la valeur x (exprimée en écarts types).

  const double r = 0.2316419;
  const double b1 = 0.319381530;
  const double b2 = -0.356563782;
  const double b3 = 1.781477937;
  const double b4 = -1.821255978;
  const double b5 = 1.330274429;

  double t, Y;
  
  t = 1 / (1 + r * fabs(x));
  Y = NormDens(x)*((((b5*t + b4) * t + b3) * t + b2) * t + b1 )*t ;
  return ( x >= 0) ? 1 - Y : Y;
  };

// --------------------------------------------------------------------------

double ASMSzPersonality::NormDens(double x)
  {
  //Cette fonction calcule la densité f(x) d'une distribution normale N(0,1), c'est-à-dire
  //centrée à t := 0 et d'écart type s := 1. La fonction est évaluée à x écart type de la
  //moyenne.
  return exp(-x*x/2)/Sqrt(2*PI);
  }

// --------------------------------------------------------------------------

void ASMSzPersonality::RegressXY (const long Count, const double x[], const double y[], double RegRslt[])
  {
  // Retourne les coefficients (a, b) et les statistiques (StdErr, R2) de la
  //regression linéaire y = a + b*x dans le vecteur "RegRslt".
  double SS, Reg, SumX, SumY, SumXY, SumX2, SumY2;
  long i, N;

  SumX = 0.0;
  SumY = 0.0;
  SumXY = 0.0;
  SumX2 = 0.0;
  SumY2 = 0.0;
  N = Count ;
  for (i = 0; i<Count; i++)
    {
    SumX  += x[i];
    SumY  += y[i];
    SumXY += x[i]*y[i];
    SumX2 += x[i]*x[i];
    SumY2 += y[i]*y[i];
    };
  RegRslt[1]  = (SumXY - SumX*SumY/N) / (SumX2 - SumX*SumX/N) ;       // coeff. b
  RegRslt[0]  = (SumY - RegRslt[1]*SumX) / N ;                        // coeff. a
  SS          = SumY2 - SumY*SumY/N ;
  Reg         = (SumXY - SumX*SumY/N)*(SumXY - SumX*SumY/N) / (SumX2 - SumX*SumX/N) ;
  RegRslt[2]  = (N > 3) ? Sqrt( (SS - Reg) / (N - 2) ) : 0;
  RegRslt[3]  = Reg / SS ;                             // R square
  }

// --------------------------------------------------------------------------

void ASMSzPersonality::RegressLN (const long Count, const double Diam[], const double WPrct[], double RegRslt[])
  {
  double x[MaxHPSizeClasses+1];
  double y[MaxHPSizeClasses+1];
  long   i;

  for (i=0; i<Count; i++)
    {
    x[i] = log10(Diam[i]);
    y[i] = NormInv(WPrct[i]);
    #if dbgModels
    if (dbgSetSizeData ())
      dbgpln("RegressLN  %3i) %14.8f %14.8f %14.8f %14.8f", i, Diam[i], WPrct[i], x[i], y[i]);
    #endif
    };
  RegressXY (Count, x, y, RegRslt);
  };

// --------------------------------------------------------------------------

void ASMSzPersonality::RegressRR (const long Count, const double Diam[], const double WPrct[], double RegRslt[])
  {
  double x[MaxHPSizeClasses+1];
  double y[MaxHPSizeClasses+1];
  long   i;

  for (i=0; i<Count; i++)
    {
    x[i] = log10(Diam[i]) ;
    y[i] = log10(log10(1.0/(1.0 - WPrct[i])));
    #if dbgModels
    if (dbgSetSizeData ())
      dbgpln("RegressRR  %3i) %14.8f %14.8f %14.8f %14.8f", i, Diam[i], WPrct[i], x[i], y[i]);
    #endif
    };
  RegressXY (Count, x, y, RegRslt );
  };

// --------------------------------------------------------------------------

void ASMSzPersonality::HyprodDataFromLN (double RegRslt[])
  {
  long i;
  double WCumul[MaxHPSizeClasses+1];

  long NIntervals=m_Q.Distributions[0]->NIntervals();
  CSD_Intervals &Intervals=m_Q.Distributions[0]->Intervals();
  double * FracPass=&m_Q.Distributions[0]->PriSp[0]->FracPass[0];

  for (i = 0; i<NIntervals; i++)
    {
    const double D = Intervals[i];
    WCumul[i] = NormDist(RegRslt[0] + RegRslt[1]*log10(D)) ;
    WCumul[i] = Range(0.0, WCumul[i], 1.0);
    FracPass[i] = WCumul[i];
    if (i>0)
      FracPass[i] -= WCumul[i-1];
    #if dbgModels
    if (dbgSetSizeData ())
      dbgpln("HyprodData %3i) %14.8f %14.8f %14.8f", i, Intervals[i], WCumul[i], FracPass[i]);
    #endif
    }

  #if dbgModels
  HyprodDist(NIntervals, &Intervals[0], WCumul);
  #endif
  //ComputeMedian(NIntervals, &Intervals[0], WCumul);
  }; 

// --------------------------------------------------------------------------

void ASMSzPersonality::HyprodDataFromRR(double RegRslt[])
  {
  long i;
  double WCumul[MaxHPSizeClasses+1];

  long NIntervals=m_Q.Distributions[0]->NIntervals();
  CSD_Intervals &Intervals=m_Q.Distributions[0]->Intervals();
  double * FracPass=&m_Q.Distributions[0]->PriSp[0]->FracPass[0];

  LogNote("Set PSD", 0, "RegRslt[0]:%.12g  RegRslt[1]:%.12g", RegRslt[0], RegRslt[1]);

  double Sum = 0.0;
  for (i=0; i<NIntervals; i++)
    {
    //const double DMean = (i==0) ? (Intervals[i] / 2.0) : ((Intervals[i] + Intervals[i-1]) / 2.0);
    //WCumul[i] = (1.0 - 1.0 / (pow(10.0, pow(10.0, (RegRslt[0] + RegRslt[1]*log10(DMean))))));
    const double D = Intervals[i];
    const double a = (RegRslt[0] + RegRslt[1]*log10(D));
    if (a>1.2)
      {//near limit
      WCumul[i] = 1.0;
      }
    else
      {
      WCumul[i] = 1.0 - (1.0 / (pow(10.0, pow(10.0, a))));
      WCumul[i] = Range(0.0, WCumul[i], 1.0);
      }
    if (i==0)
      {//adjust for special case for first size fraction!!! (adjust based on count)
      const double DMean_a = D/2.0;
      const double DMean_b = (D+m_Rqd.m_dCrit)/2.0;
      FracPass[i] = WCumul[0] * pow(DMean_a, 3) / pow(DMean_b, 3);
      }
    else
      {
      FracPass[i] = WCumul[i] - WCumul[i-1];
      }
    Sum += FracPass[i];
    }

  //adjust to ensure sum is exactly 1.0
  double Sum2 = 0.0;
  for (i=NIntervals-1; i>0; i--)
    {
    FracPass[i] += (FracPass[i] * (1.0-Sum));
    if (FracPass[i]<1.0e-10)
      FracPass[i] = 0.0;
    Sum2 += FracPass[i];
    }
  FracPass[0] = Max(0.0, 1.0-Sum2);

  #if dbgModels
  HyprodDist(NIntervals, &Intervals[0], WCumul);
  #endif
  //ComputeMedian(NIntervals, &Intervals[0], WCumul);
  };

// --------------------------------------------------------------------------

void ASMSzPersonality::HyprodDataFromTable(long Count, double Diam[], double WPrct[], double RegRslt[])
  {
  long NIntervals=m_Q.Distributions[0]->NIntervals();
  CSD_Intervals &Intervals=m_Q.Distributions[0]->Intervals();
  double * FracPass=&m_Q.Distributions[0]->PriSp[0]->FracPass[0];

  long   i;
  double WCumul[MaxHPSizeClasses+1];

  bool Flag = true;
  while (Flag)     // sorting of the entries in ascending order of size
    {
    Flag = false;
    for (i=0; i<Count-1; i++)
      {
      if (Diam[i] > Diam[i+1]) 
        {
        double HoldSize =Diam[i];
        double HoldWPrct=WPrct[i];
        Diam[i]=Diam[i+1];
        WPrct[i]=WPrct[i+1];
        Diam[i+1]=HoldSize;
        WPrct[i+1]=HoldWPrct;
        Flag=true;
        }
      }
    }
  double Diamr[MaxHPSizeClasses+1];
  for (i=0; i<Count; i++)
    Diamr[i] = Diam[i]*1.0e6; //convert from m to um

  for (i=Count; i>0; i--)
    {
    Diamr[i] = Diamr[i-1]; // moving the data one position up ...
    WPrct[i] = WPrct[i-1]; 
    }
  Diamr[0]  = 0.000001;	   //  ... in order to insert the point ( 0 ,0)
  WPrct[0]  = 0.0;
  Count++;

  // interpolation of the cumulative distribution...
  i=0; 
  double x1;
  double x2=log10(Diamr[i+1]);
  double y1;
  double y2=log10(log10(1.0000001/(1.0-WPrct[i+1])));
  double nn=2.6; // force the slope  for the first part otherwise give a strange curve aspect 
  double bb=y2 - nn*x2; // ... then calculate the intercept
  double Sum = 0.0;
  for (int mm=0; mm<NIntervals; mm++)
	  {
    double xx = (mm+1)*HPDSize;
    while (xx > Diamr[i+1]) //MAKE SURE THAT WE INTERPOLATE AROUND THE XX POINT 
			{
      i++;
      if ( (Diamr[i+1] - Diamr[i]) < HPDSize )
        i++;
      x1=x2;
      y1=y2;
      if (i < Count-1)
  			{
        x2=log10(Diamr[i+1]);
        y2=log10(log10(1.0000001/(1.000001-WPrct[i+1])));
        if (fabs(x1-x2)<1.0e-18)
			    {
				  //	 Error_Message(22,'WARNING -- ERROR -- WARNING');
				  break;
			    }
        nn=(y1-y2)/(x1-x2);
        bb=y2 - nn*x2;
			  }
      else
        {
        nn=3.0; // fix the slope for the coarser END
        bb=y1 - nn*x1;
        }
			} 
    
    const double d1 = bb + nn*log10(xx);
    WCumul[mm] = 1.0-1.0/pow(10.0, pow(10.0, d1));
    WCumul[mm] = Range(0.0, WCumul[mm], 1.0);
	  if (mm==0)
		  {//adjust for special case for first size fraction!!! (adjust based on count)
		  FracPass[mm] = WCumul[0];
		  }
		else
		  {
		  FracPass[mm] = WCumul[mm] - WCumul[mm-1];
		  }
    if (FracPass[mm]<1.0e-12)
      FracPass[mm] = 0.0;
	  Sum += FracPass[mm];
	  }

  //adjust to ensure sum is exactly 1.0
  double Sum2 = 0.0;
  for (i=NIntervals-1; i>0; i--)
    {
    FracPass[i] += (FracPass[i] * (1.0-Sum));
    Sum2 += FracPass[i];
    }
  FracPass[0] = Max(0.0, 1.0-Sum2);

  #if dbgModels
  HyprodDist(NIntervals, &Intervals[0], WCumul);
  #endif
 };

// --------------------------------------------------------------------------

void ASMSzPersonality::HyprodDist(long NIntervals, double SizeClass[], double WCumul[])
//void MassFrac2HyprodDist( double SizeClass[],
//                          double MassFrac[],
//                          double PS[],
//                          long NIntervals,
//                          double &SSurf)
  {
  const double WFact  = PI*2.42/6*1e-12 ; //      { =  Pi/6 * 2.42 g/cm3 * 10^-12 cm3/µm3}
  const double SFact  = PI*1e-8;          //      { = Pi * 10^-8 cm2/µm2 }
  const double WSFact = WFact/SFact;      //      { = WFact / SFact }

  double PS[MaxHPSizeClasses+1];//, PCumul[MaxHPSizeClasses+1];
  double SCumul, PCumul;
  double SS, WS, DMean, SSurf;

  PCumul = 0.0;
  SCumul = 0.0;
  //WCumul[0] = 0.0 ;
  for (long i=0; i<NIntervals; i++)
    {
    DMean = (i==0) ? (SizeClass[i] / 2.0) : ((SizeClass[i] + SizeClass[i-1]) / 2.0);
    DMean *= 1e6; // to microns
    WS = WCumul[i];
    PS[i] = WS / pow(DMean, 3) / WFact;
    SS = WS / DMean / WSFact;
    PCumul += PS[i];
    SCumul += SS;                     // surface en cm2/g

    #if dbgModels
    if (dbgSetSizeData ())
      dbgpln("HyprodDist %3i) %14.8f %14.8f %10.3e", i, DMean, WS, PS[i]);
    #endif
    }
  //ppg := PCumul[50] ;
  SSurf = SCumul/10000.0;
  } //  {end procedure HyprodDist   }

// ==========================================================================
//
//                                  End
//
// ==========================================================================
//

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
