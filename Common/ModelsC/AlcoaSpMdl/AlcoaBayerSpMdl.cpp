//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __ALCOABAYERSPMDL_CPP
#include "sp_db.h"
#include "AlcoaBayerSpMdl.h"
#include "errorlog.h"
//#include "optoff.h"

// ==========================================================================

#define dbgModels WITHDEBUG
#if dbgModels
#include "dbgmngr.h"
static CDbgMngr dbgSpecies ("BASIC", "Species");
#endif

// ==========================================================================
//
// Alcoa Bayer Specie Model.
//
// ==========================================================================

IMPLEMENT_SPMODEL(AlcoaBayer, "AlcoaBayer", "", TOC_ALL|TOC_GRP_ALUMINA|TOC_ALUMINA, "Alcoa Bayer", "Alcoa Bayer Model");
IMPLEMENT_SPARES(AlcoaBayer, 100);

static MInitialiseTest InitTest(&AlcoaBayerClass);

// Specie Access 
SPECIEBLK_L(InitTest, Water,             "H2O(l)",        false);
SPECIEBLK_L(InitTest, Alumina,           "Al2O3(l)",      false);
SPECIEBLK_L(InitTest, CausticSoda,       "NaOH(l)",       false);
SPECIEBLK_L(InitTest, SodiumCarbonate,   "Na2CO3(l)",     false);
SPECIEBLK_L(InitTest, SodiumChloride,    "NaCl(l)",       false);
SPECIEBLK_L(InitTest, SodiumOxalate,     "Na2C2O4(l)",    false);
SPECIEBLK_L(InitTest, SilicateLiq,       "SiO2(l)",       false);
SPECIEBLK_L(InitTest, SodiumSulphate,    "Na2SO4(l)",     false);
SPECIEBLK_L(InitTest, Organics,          "Na2C5O7(l)",    false);

SPECIEBLK_S(InitTest, OccSoda,           "Na2O(s)",       false);
SPECIEBLK_S(InitTest, THA,               "Al2O3.3H2O(s)", false);

SPECIEBLK_V(InitTest, Steam,             "H2O(g)",        true);

const double H2OTestFrac = 0.98;
const double MW_C        = 12.011;
const double MW_NaAlO2   = 81.970107; //22.989768 + 26.981539 + (2*15.9994)
const double MW_NaNOOC   = 122.0;
const double MW_NaInOrg  = 59.6;

enum BDM_BayerDensityMethod { BDM_Original, BDM_Alcoa };
enum CPM_HeatCapacityMethod { CPM_Original, CPM_Alcoa };
enum BPM_BPEMethod { BPM_Dewey, BPM_Adamson, BPM_Alcoa, BPM_Alcoa2 };
enum FCM_FeedCalcMethod { FCM_Original, FCM_Alcoa2 };

double AlcoaBayer::dBPEFactor = 1.0;
double AlcoaBayer::dRhoFactor = 1.0;
double AlcoaBayer::dMinSolCp = 1.046;
double AlcoaBayer::sm_dNaOrg_to_NOOC = 3.16;
double AlcoaBayer::sm_dNaInOrg_to_TA = 0.0236;
double AlcoaBayer::sm_dNaOx_to_TA = 0.0125;
byte AlcoaBayer::sm_iBPEMethod = BPM_Alcoa;
byte AlcoaBayer::sm_iDensityMethod = BDM_Alcoa;
byte AlcoaBayer::sm_iCPMethod = CPM_Alcoa;
byte AlcoaBayer::sm_iFeedCalcMethod = FCM_Original;

// ==========================================================================
//
// A special class to assist with calculating iterative concentration and density
// calculations.
//
// ==========================================================================

flag     CConcentrations::NaFactorOK=0;
SpVector CConcentrations::NaFactor(0);

CConcentrations::CConcentrations(AlcoaBayer *pMdl) 
  { 
  pBayerMdl = pMdl;
  Liq.SetSize(SDB.Count());
  Zero(); 

  if (!NaFactorOK)
    { 
    for (int sn=0; sn<SDB.Count(); sn++)
      NaFactor.VValue[sn]=1.0; //For all the species without sodium ions
    NaFactor.VValue[::CausticSoda.LiqPhInx()]     = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::CausticSoda.MW);    // NaOH         39.9971
    NaFactor.VValue[::SodiumCarbonate.LiqPhInx()] = 1.0;                                                      // Na2CO3       105.989
    NaFactor.VValue[::SodiumOxalate.LiqPhInx()]   = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumOxalate.MW);  // Na2C2O4      133.999
    NaFactor.VValue[::SodiumChloride.LiqPhInx()]  = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumChloride.MW); // NaCl         58.4425
    NaFactor.VValue[::SodiumSulphate.LiqPhInx()]  = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumSulphate.MW); // Na2SO4       142.043
    //NaFactor[::SodiumSilicate.LiqPhInx()]  = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumSilicate.MW);
    NaFactor.VValue[::Organics.LiqPhInx()]        = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::Organics.MW);       // Na2C5O7

    NaFactorOK = true; 
    }
  }

// --------------------------------------------------------------------------

void CConcentrations::Zero()
  {
  Density = 1100.0;
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

  static CToleranceBlock s_Tol(TBF_Both, "AlcoaSpMdl:Density", 1e-12, 1e-8, 100);

  const double T_ = C2K(25.0);
  double Tc = K2C(T_);
  // Converge Liquor Conc. All sodium concentrations expressed as Na2CO3
  int IterCount = s_Tol.GetMaxIters();//100;
  double OldDensity = Density*1.1;
  while ( fabs(OldDensity - Density) > s_Tol.GetAbs()/*0.00005*/  && --IterCount>0)
    {
    Density = Range(0.0001, Density, 10000.0);
    for (int sn=0; sn<SDB.Count(); sn++)
      {
      if (SDB[sn].IsLiq())
        {
        //double mm=MA[sn];
        Liq[sn] = MA[sn] / TLiq * Density * NaFactor[sn];
        }
      }
    
    OldDensity = Density;
    Density = LiquorDensity(T_, MA);
    } // end of while

  //pBayerMdl->SetCI((IterCount==0), 1, True, True);

  #if dbgModels
  if (dbgSpecies ())
    {
    dbgpln("Concs           Tc:%10.2f",Tc);
    dbgpln("           Density:%10.2f",Density);
    for (int sn=0; sn<SDB.Count(); sn++)
      if (SDB[sn].IsLiq())
      dbgpln("%18.18s:%12.6f  MW:%11.5f NaFactor:%9.5f",SDB[sn].SymOrTag(),Liq[sn],SDB[sn].MoleWt(),NaFactor[sn]);
    }
  #endif
  return (IterCount>=0);
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
         + MA[Organics.LiqPhInx()]       * NaFactor[::Organics.LiqPhInx()] );
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
  double RHO = RHO25 * (1.0
            -(0.0005021858*0.85*(Tc-25))
            -(0.0000011881*0.85*(Tc-25)*(Tc-25)) ); // Density at T
  return (RHO * 1000.0); // kg/m^3
  }

// --------------------------------------------------------------------------

double CConcentrations::LiquorDensEqn2(long Fidelity, double Tc, double A, double S, double C, double toc, double SO4, double Cl)
  {
  const double B58 = S - C;
  const double B59 = 0.000797*C - 0.0000003087*C*C;
  const double B60 = -0.0000004082*C*Tc - 0.0000006208*A*C;
  const double B61 = 0.0006455*A + 0.0000009582*A*A;
  const double B62 = 0.001008*B58 - 0.0000005589*B58*B58 - 0.000001759*B58*A;
  const double B63 = 0.00167*toc + 0.000554*SO4 + 0.00052*Cl;
  const double B64 = SDB[Water.LiqPhInx()].Density(Fidelity, C2K(Tc), Std_P, NULL, NULL)/1000.0;
  return (B59+B60+B61+B62+B63+B64) * 1000.0;
  }

// --------------------------------------------------------------------------

double CConcentrations::LiquorDensity(double T_, CSysVector & MA)
  {
  double TLiq=MA.Sum(som_Liq);        //Total Liquid
  if (TLiq<1.0e-9)
    return 0.0;
  
  double Tc = K2C(T_);
  switch (AlcoaBayer::sm_iDensityMethod)
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
        dbgpln("Liq Dens Tc:%10.2f WTNa:%12.6g WAlumina:%12.6g RHO:%12.6g", Tc, WTNa, WAlumina, RHO);
      #endif
      return RHO;
      }
    case BDM_Alcoa:
      {
      //Based on Alcoa Excel file 'props.xlm' provided by Andrew Maggs and Kristen Dore 30/3/2005
      //Notes in props.xlm indicate original programed by Peter Hay 1998 Alcoa Pinjarra Technical Department
      //B10=Total.Alkali g/l
      //B11=Total.Caustic g/l
      //B12=Alumina g/l
      //B13=TOC g/l
      //B14=Na2SO4 g/l
      //B15=NaCl g/l
      //B16=NaF g/l
      //=ARGUMENT(,1,B57) //Temperature (C)
      //=SET.VALUE(B58,$B$10-$B$11)
      //=SET.VALUE(B59,0.000797*B11-0.0000003087*B11^2)
      //=SET.VALUE(B60,-0.0000004082*B11*B57-0.0000006208*B12*B11)
      //=SET.VALUE(B61,0.0006455*B12+0.0000009582*B12^2)
      //=SET.VALUE(B62,0.001008*B58-0.0000005589*B58^2-0.000001759*B58*B12)
      //=SET.VALUE(B63,0.00167*B13+0.000554*B15+0.00052*B14)
      //=SET.VALUE(B64,Water.Density(B57))
      //=SET.VALUE(B65,B59+B60+B61+B62+B63+B64)
      const double A   = Liq[::Alumina.LiqPhInx()];
      const double S   = Liq[::CausticSoda.LiqPhInx()] + Liq[::SodiumCarbonate.LiqPhInx()];
      const double C   = Liq[::CausticSoda.LiqPhInx()];
      const double toc = (Liq[::Organics.LiqPhInx()]*5.0 + Liq[::SodiumOxalate.LiqPhInx()]*2.0)*MW_C/::SodiumCarbonate.MW;
      const double SO4 = Liq[::SodiumSulphate.LiqPhInx()]/NaFactor[::SodiumSulphate.LiqPhInx()];//NOT as Na2CO3
      const double Cl  = Liq[::SodiumChloride.LiqPhInx()]/NaFactor[::SodiumChloride.LiqPhInx()];//NOT as Na2CO3
      double RHO = LiquorDensEqn2(pBayerMdl->Fidelity(), Tc, A, S, C, toc, SO4, Cl);
      return RHO;
      }
    }
  return 1000.0;
  }

// ==========================================================================
//
//                        Bayer Liquor Specie Model
//
// ==========================================================================
  
const word xidSlurryAluminaConc25 = 30006;
const word xidSlurryCausticConc25 = 30007;
const word xidSlurrySodaConc25    = 30008;
const word xidSlurryAtoC25        = 30009;
const word xidSlurryCtoS25        = 30010;
const word xidSlurryCltoC25       = 30011;
const word xidSlurrySodiumCarbConc25 = 30012;
const word xidSlurrySolidsConc25  = 30013;
const word xidLiquidVolume25      = 30014;
const word xidSlurryVolume25      = 30015;
const word xidSlurryRho25         = 30016;
const word xidLiquidRho25         = 30017;
const word xidSlurryVapPress      = 30018;
const word xidSlurryFreeCaustic25 = 30019;

const word xidOrganateConc25      = 30020;
const word xidOxalateConc25       = 30021;
const word xidTotalOrgConc25      = 30022;
const word xidSlurryTOOC25        = 30023;
const word xidSlurryTOC25         = 30024;
const word xidNaClConc25          = 30025;
const word xidNa2SO4Conc25        = 30026;
const word xidTotalNaConc25       = 30027;

const word xidSlurryAluminaConc   = 30030;
const word xidSlurryCausticConc   = 30031;
const word xidSlurrySodaConc      = 30032;
const word xidSlurrySolidsConc    = 30033;
const word xidSlurryTOOC          = 30034;
const word xidSlurryTOC           = 30035;
const word xidIonicStrength       = 30036;
const word xidOxalateEquilibrium  = 30037;
const word xidASaturation         = 30038;
const word xidAtoCSaturation      = 30039;
const word xidSSNRatio            = 30040;
const word xidAluminaSSN          = 30041;
const word xidOxalate_Conc25      = 30045;
const word xidNaCl_Conc25         = 30046;
const word xidNa2SO4_Conc25       = 30047;
const word xidCausticConc25_Na2O  = 30048;
const word xidSodaConc25_Na2O     = 30049;
const word xidTotalNaConc25_Na2O  = 30050;
const word xidAtoC25_Na2O         = 30051;
const word xidBayerBPE            = 30052;

const word xidActualNaOx_to_TA    = 30060;
const word xidExpectedOxalate     = 30061;
const word xidOxalateError        = 30062;
const word xidNonOxOrg_NaSalts    = 30063;
const word xidInOrg_NaSalts       = 30064;

CArray <int, int> AlcoaBayer::LclParm;

AlcoaBayer::AlcoaBayer(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach):
  SMBayerBase(pClass_, Tag_, pAttach, eAttach),
  //LiqConcs(this),
  LiqConcs25(this)
  {
  fDoCalc  = false;
  dRqd_AtoC = 0.4;
  dRqd_C    = 220.0;
  dRqd_CtoS = 0.85;
  dRqd_Sul  = 20.0;
  dRqd_Sil  = 1.0;
  dRqd_Salt = 15.0;
  dRqd_SolFrac = 0.0;
  dRqd_Ox   = 25.0;
  dRqd_TOC  = 30.0;
  }

// --------------------------------------------------------------------------

AlcoaBayer::~AlcoaBayer()
  {
  }

// --------------------------------------------------------------------------

DDEF_Flags AlcoaBayer::FixSpEditFlags(DDEF_Flags Flags, int iSpid, flag AsParms)
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
    LclParm[SodiumOxalate.LiqPhInx()]  = false;
    LclParm[Alumina.LiqPhInx()]  = false;
    LclParm[Organics.LiqPhInx()] = false;
    LclParm[Water.LiqPhInx()] = false;
    }
 
  return (Flags&~(isParm|isResult)) | ((AsParms && (!fDoCalc || LclParm[iSpid])) ? DDEF_PARAM: DDEF_RESULT); 
  }

// --------------------------------------------------------------------------

void AlcoaBayer::BuildDataDefn_Vars(DataDefnBlk & DDB)
  {
  static DDBValueLst DDB0[]={
    {BPM_Dewey, "Dewey"},
    {BPM_Adamson, "Adamson"},
    {BPM_Alcoa, "Alcoa"},
    {BPM_Alcoa2, "AlcoaFixedOrg"},
    {0}};
  static DDBValueLst DDB1[]={
    {BDM_Original, "Original"},
    {BDM_Alcoa, "Alcoa"},
    {0}};
  static DDBValueLst DDB2[]={
    {CPM_Original, "Original"},
    {CPM_Alcoa, "Alcoa"},
    {0}};
  static DDBValueLst DDB3[]={
    {FCM_Original, "Original"},
    {FCM_Alcoa2, "AlcoaFixedOx"},
    {0}};

  if (UseAsMass() || UseAsImage())
    {
    DDB.Byte    ("Density_Method",  "",   DC_,      "",        &sm_iDensityMethod, this,  isParm, DDB1); //global constant
    DDB.Byte    ("Cp_Method",       "",   DC_,      "",        &sm_iCPMethod,    this,  isParm, DDB2); //global constant
    DDB.Byte    ("BPE_Method",      "",   DC_,      "",        &sm_iBPEMethod,   this,  isParm, DDB0); //global constant
    DDB.Byte    ("FeedCalc_Method", "",   DC_,      "",        &sm_iFeedCalcMethod, this,  isParm, DDB3); //global constant
    DDB.Double  ("BPE_Factor",      "",   DC_Frac,  "%",       &dBPEFactor,      this,  isParm|InitHidden); //global constant
    DDB.Double  ("Rho_Factor",      "",   DC_Frac,  "%",       &dRhoFactor,      this,  isParm|InitHidden); //global constant
    DDB.Double  ("MinSolidsCp",     "",   DC_CpMs,  "kJ/kg.C", &dMinSolCp,       this,  isParm|InitHidden); //global constant
    DDB.Double  ("NaOrg/NOOC",      "",   DC_,      "",        &sm_dNaOrg_to_NOOC, this,  isParm|InitHidden); //global constant
    DDB.Double  ("NaInOrg/TA",      "",   DC_,      "",        &sm_dNaInOrg_to_TA, this,  isParm|InitHidden); //global constant
    DDB.Double  ("NaOx/TA",         "",   DC_,      "",        &sm_dNaOx_to_TA,  this,  isParm|InitHidden); //global constant
    DDB.Text("Bayer");
    DDB.CheckBoxBtn("DefineLiquor", "",   DC_,      "",        &fDoCalc,         this,  isParm|SetOnChange);
    DDB.Visibility(NSHM_All, fDoCalc);                                                         
    DDB.Double  ("Rqd_A/C",         "",   DC_,      "",        &dRqd_AtoC,       this,  isParm);
    DDB.Double  ("Rqd_C/S",         "",   DC_,      "",        &dRqd_CtoS,       this,  isParm);
    DDB.Double  ("Rqd_C",           "",   DC_Conc,  "g/L",     &dRqd_C,          this,  isParm);
    DDB.Double  ("Rqd_SiO2",        "",   DC_Conc,  "g/L",     &dRqd_Sil,        this,  isParm);
    DDB.Double  ("Rqd_Oxalate",     "",   DC_Conc,  "g/L",     &dRqd_Ox,         this,  sm_iFeedCalcMethod==FCM_Alcoa2 ? isResult : isParm);
    DDB.Double  ("Rqd_TOC",         "",   DC_Conc,  "g/L",     &dRqd_TOC,        this,  isParm);
    DDB.Double  ("Rqd_Na2SO4",      "",   DC_Conc,  "g/L",     &dRqd_Sul,        this,  isParm);
    DDB.Double  ("Rqd_NaCl",        "",   DC_Conc,  "g/L",     &dRqd_Salt,       this,  isParm);
    DDB.Double  ("SolidsFrac",      "",   DC_Frac,  "%",       &dRqd_SolFrac,    this,  isParm);
    DDB.Visibility();                                                                               
    DDB.Text("");
    }

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
  DDB.Double("Cl/C",          "",   DC_,      "",        xidSlurryCltoC25,       this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("TOC",           "",   DC_Conc,  "g/L",     xidSlurryTOC25,         this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("SodiumCarbConc","",   DC_Conc,  "g/L",     xidSlurrySodiumCarbConc25,this,isResult|noFile|noSnap|InitHidden);
  DDB.Double("FreeCaustic",   "FC", DC_Conc,  "g/L",     xidSlurryFreeCaustic25, this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("SolidsConc",    "",   DC_Conc,  "g/L",     xidSlurrySolidsConc25,  this,  isResult|noFile|noSnap);

  DDB.Text("");
  DDB.Text("Concentrations @ 25, as Na2CO3 equivalent");
  DDB.Double("Organates",     "",   DC_Conc,  "g/L",     xidOrganateConc25,      this,  isResult|noFile|noSnap);
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
  DDB.Double("Oxalate*",      "",   DC_Conc,  "g/L",     xidOxalate_Conc25,      this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("NaCl*",         "",   DC_Conc,  "g/L",     xidNaCl_Conc25,         this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("Na2SO4*",       "",   DC_Conc,  "g/L",     xidNa2SO4_Conc25,       this,  isResult|noFile|noSnap|InitHidden);
  //DDB.Double("VapPress",     "Vp",  DC_P,     "kPa",     xidSlurryVapPress,      this,  isResult|noFile|noSnap);

  DDB.Text("");
  DDB.Text("Bayer Liquor Values @ 25, as Na2O equivalent");
  DDB.Double("CausticConc_Na2O", "C_Na2O", DC_Conc, "g/L", xidCausticConc25_Na2O,  this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("SodaConc_Na2O", "S_Na2O", DC_Conc,"g/L",     xidSodaConc25_Na2O,     this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("TotalNa_Na2O",  "",     DC_Conc,  "g/L",     xidTotalNaConc25_Na2O,  this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("RP",            "",     DC_,      "",        xidAtoC25_Na2O,         this,  isResult|noFile|noSnap);

  DDB.Text("");
  DDB.Text("Bayer Liquor Values @ Temp");
  DDB.Double("AluminaConcT", "A@T", DC_Conc,  "g/L",     xidSlurryAluminaConc,   this,  isResult|noFile);
  DDB.Double("CausticConcT", "C@T", DC_Conc,  "g/L",     xidSlurryCausticConc,   this,  isResult|noFile);
  DDB.Double("SodaConcT",    "S@T", DC_Conc,  "g/L",     xidSlurrySodaConc,      this,  isResult|noFile);
  //DDB.Double("TOOC@T",        "",   DC_Conc,  "g/L",     xidSlurryTOOC,          this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("TOC@T",         "",   DC_Conc,  "g/L",     xidSlurryTOC,           this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("SolidsConcT",   "",   DC_Conc,  "g/L",     xidSlurrySolidsConc,    this,  isResult|noFile);
  
  DDB.Text("");
  DDB.Text("Bayer Liquor Precipitation Values @ Temp");
  DDB.Double("A_Saturation", "ASat",DC_Conc,  "g/L",     xidASaturation,         this,  isResult|noFile|noSnap);
  DDB.Double("A/C_Saturation", "A/CSat", DC_, "",        xidAtoCSaturation,      this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("SSN_Ratio",     "",   DC_,      "",        xidSSNRatio,            this,  isResult|noFile|noSnap);
  //DDB.Double("Alumina_SSN",   "",   DC_,      "",        xidAluminaSSN,          this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("IonicStrength", "I",  DC_,      "",        xidIonicStrength,       this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("OxalateEq",     "",     DC_Conc,  "g/L",   xidOxalateEquilibrium,  this,  isResult|noFile|noSnap|InitHidden);

  DDB.Text("");
  DDB.Text("Other");
  DDB.Double("BoilPtElev",    "",   DC_dT,    "C",       xidBayerBPE,            this,  isResult|noFile);
  DDB.Text("");
  DDB.Text("Other Alcoa Values @ 25");
  DDB.Double("Actual_NaOx/TA", "",    DC_,    "",        xidActualNaOx_to_TA,    this,  isResult|noFile|noSnap);
  DDB.Double("ExpectedOxalate", "",   DC_Conc, "g/L",    xidExpectedOxalate,     this,  isResult|noFile|noSnap);
  DDB.Double("OxalateError",    "",   DC_Frac, "%",      xidOxalateError,        this,  isResult|noFile|noSnap);
  DDB.Double("NonOxOrg_SodiumSalts", "", DC_Conc, "g/L", xidNonOxOrg_NaSalts,    this,  isResult|noFile|noSnap);
  DDB.Double("InOrg_SodiumSalts", "", DC_Conc, "g/L",    xidInOrg_NaSalts,       this,  isResult|noFile|noSnap);
  }

// --------------------------------------------------------------------------

flag AlcoaBayer::DataXchg(DataChangeBlk & DCB)
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
    case xidTotalNaConc25:         DCB.D=TotalSodiumConc(); return 1;
    case xidIonicStrength:         DCB.D=IonicStrength(); return 1;
    case xidASaturation:           DCB.D=AluminaConcSat(Temp()); return 1;
    case xidAtoCSaturation:        DCB.D=AtoCSaturation(Temp()); return 1;
    case xidSSNRatio:              DCB.D=SSNRatio(Temp()); return 1;
    case xidOxalateEquilibrium:    DCB.D=OxalateEquilibrium(Temp()); return 1;
    case xidAluminaSSN:            DCB.D=AluminaSSN(Temp()); return 1;
    case xidSlurryTOOC25:          DCB.D=TOOC(C_2_K(25.0)); return 1;
    case xidSlurryTOOC:            DCB.D=TOOC(Temp()); return 1;
    case xidSlurryTOC25:           DCB.D=TOC(C_2_K(25.0)); return 1;
    case xidSlurryTOC:             DCB.D=TOC(Temp()); return 1;
    case xidSlurrySolidsConc:      DCB.D=SolidsConc(Temp()); return 1;
    case xidSlurrySolidsConc25:    DCB.D=SolidsConc(C_2_K(25.0)); return 1;
    case xidSlurryFreeCaustic25:   DCB.D=FreeCaustic(C_2_K(25.0)); return 1;
    case xidBayerBPE:              DCB.D=BoilingPtElevation(Press()); return 1;//BoilPtElev(Temp()); return 1;WRONG

    case xidOrganateConc25:        DCB.D=OrganateConc(); return 1;
    case xidTotalOrgConc25:        DCB.D=TotalOrganics(); return 1;
    case xidOxalateConc25:         DCB.D=SodiumOxalateConc(C_2_K(25.0)); return 1;
    case xidNaClConc25:            DCB.D=ChlorineConc(); return 1;
    case xidNa2SO4Conc25:          DCB.D=SulphateConc(); return 1;

    //case xidSlurryVapPress:        DCB.D=SaturationP(Temp()); return 1;
    case xidLiquidRho25:           DCB.D=Rho(som_Liq, C_2_K(25.0), Press()); return 1;
    case xidSlurryRho25:           DCB.D=Rho(som_SL,  C_2_K(25.0), Press()); return 1;
    case xidSlurryVolume25:        DCB.D=SLVolume25(); return 1;
    case xidLiquidVolume25:        DCB.D=LVolume25(); return 1;
    case xidOxalate_Conc25:        DCB.D=SodiumOxalate_Conc(); return 1;
    case xidNaCl_Conc25:           DCB.D=Chlorine_Conc(); return 1;
    case xidNa2SO4_Conc25:         DCB.D=Sulphate_Conc(); return 1;

    case xidCausticConc25_Na2O:    DCB.D=CausticConc_Na2O(C_2_K(25.0)); return 1;
    case xidSodaConc25_Na2O:       DCB.D=SodaConc_Na2O(C_2_K(25.0)); return 1;
    case xidTotalNaConc25_Na2O:    DCB.D=TotalSodiumConc_Na2O(); return 1;
    case xidAtoC25_Na2O:           DCB.D=AtoC_Na2O(); return 1;

    case xidActualNaOx_to_TA:      DCB.D=SodiumOxalate_Conc()/GTZ(SodaConc(C_2_K(25.0))); return 1;
    case xidExpectedOxalate:       DCB.D=(TOC(C_2_K(25.0))>0.0 ? SodaConc(C_2_K(25.0))*sm_dNaOx_to_TA : 0.0); return 1;
    case xidOxalateError:          
      if (TOC(C_2_K(25.0))>0.0)
        {
        const double ExpectedOxalate = SodaConc(C_2_K(25.0))*sm_dNaOx_to_TA;
        const double Oxalate = SodiumOxalate_Conc();
        DCB.D=fabs(ExpectedOxalate - Oxalate)/ExpectedOxalate;
        }
      else
        DCB.D=0.0;
      return 1;
    case xidNonOxOrg_NaSalts:      DCB.D=(TOC(C_2_K(25.0))-SodiumOxalate_Conc()*2*MW_C/::SodiumOxalate.MW)*sm_dNaOrg_to_NOOC; return 1;
    case xidInOrg_NaSalts:
      if (TOC(C_2_K(25.0))>0.0 && Chlorine_Conc() && Sulphate_Conc())
        DCB.D=SodaConc(C_2_K(25.0))*sm_dNaInOrg_to_TA;
      else
        DCB.D=0.0;
      return 1;

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
    //==>OrgRatio=f(OxConc,toc)
    //==> OrgRatio = OxConc/tor
    //==> OrgRatio = (OxConc*5)/(toc/12*106 + 3*OxConc)
    //==> OrgRatio = 5/(toc/OxConc/12*106 + 3)

    }
  return 0;
  }

//---------------------------------------------------------------------------

flag AlcoaBayer::ValidateData(ValidateDataBlk & VDB)
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
    dRqd_Ox   = ValidateRange(VDB, "dRqd_Ox", 0.0, dRqd_Ox,       500.0);
    dRqd_TOC  = ValidateRange(VDB, "dRqd_TOC", 0.0, dRqd_TOC,     500.0);
    dRqd_SolFrac = ValidateRange(VDB, "dRqd_SolFrac", 0.0, dRqd_SolFrac, 0.99);

    //SetCI(2, (dRqd_S < 1e-6) || (dRqd_C <1e-6));
    //SetCI(3, dRqd_C > dRqd_S);
    
    // CNM - When adding RawMassFlow for Tears
    //if (Usage()!=SPMU_Flow)
    //  SetView(SVV_AsMassFrac); //forces view option to mass fraction
    DoInputCalcs(Temp());
    }
  
  return OK;
  }

//---------------------------------------------------------------------------

void AlcoaBayer::DoInputCalcs(double T_)
  {
  switch (GetView())
    {
    case SVV_AsRawFrac:
    case SVV_AsRawMass:
    case SVV_AsRawMassFlow:
      fDoCalc=false;
      return;
    }

  const bool WantSolids=(dRqd_SolFrac>1e-6);
  double TotalMass = Mass(som_SL);
  if (WantSolids)
    {
    if (Mass(som_Sol)<1.0e-6)
      {//assume THA solids required
      LogNote(FullObjTag(), 0, "Assume solids are THA for Bayer feed calculator");
      VValue[THA.SolPhInx()]=TotalMass*dRqd_SolFrac;
      double Scl=(TotalMass*(1.0-dRqd_SolFrac))/GTZ(Mass(som_Liq));
      ScaleMass(som_Liq, Scl);
      TotalMass = Mass(som_SL);
      }
    }
  else
    {
    dRqd_SolFrac = 0.0;
    }

  //----------------------------------------------
  const double LiqMassCalc = (VValue[Water.LiqPhInx()] + VValue[CausticSoda.LiqPhInx()] + VValue[Alumina.LiqPhInx()] + 
                        VValue[SodiumCarbonate.LiqPhInx()] + VValue[SodiumChloride.LiqPhInx()] + /*VValue[SodiumFluoride.LiqPhInx()] +*/
                        VValue[SodiumSulphate.LiqPhInx()] + VValue[SilicateLiq.LiqPhInx()] + VValue[SodiumOxalate.LiqPhInx()] + VValue[Organics.LiqPhInx()]);
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
    double Al, Caustic, Carbonate, Salt, NaSulphate, Silicate;//, Organic;
    double water, Derr, Org1, Org2, Ox, Og, Other;

    //Toc = (Og_*5 + Ox_*2)*MW_C/MW_Na2CO3
    //Og = Og_*MW_Na2C5O7/MW_Na2CO3
    //Ox = Ox_*MW_Na2C2O4/MW_Na2CO3
    //==>Toc = (Og*5/MW_Na2C5O7 + Ox*2/MW_Na2C2O4)*MW_C
    //==>Og = (Toc/MW_C - Ox*2/MW_Na2C2O4)*MW_Na2C5O7/5
    const double MW_Na2C2O4   = ::SodiumOxalate.MW;    //133.999
    const double MW_Na2C5O7   = ::Organics.MW;         //218.030
    if (sm_iFeedCalcMethod==FCM_Alcoa2)
      {
      const double S = dRqd_C/dRqd_CtoS;
      dRqd_Ox = S*sm_dNaOx_to_TA;
      }
    const double minTOC = dRqd_Ox*2.0/MW_Na2C2O4*MW_C;
    double TocUsed = dRqd_TOC;
    if (dRqd_TOC<minTOC)
      {
      LogWarning(FullObjTag(), 0, "Required TOC too low based on required Oxalate");
      TocUsed = minTOC; //this will give Og of zero
      }
    Ox = dRqd_Ox;
    Og = (TocUsed/MW_C - dRqd_Ox*2.0/MW_Na2C2O4)*MW_Na2C5O7/5.0;
    if (dRqd_C > 1e-6)
      {
      const double MW_NaOH     = ::CausticSoda.MW;      //39.9971
      const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989
      const double MW_NaCl     = ::SodiumChloride.MW;   //58.4425
      const double MW_Na2SO4   = ::SodiumSulphate.MW;   //142.043
      //dRqd_TOC = dRqd_Org*MW_C/MW_Na2CO3*(5.0 - (3.0 * dRqd_OrgRatio));
      // Calculate density based on factors.
      double Lm = LiqMass;
      double Dens1 = 1.100;  // First guess at density
      double Vol, Tal, Tna, Tcc;
      double Density25;
      for (int j=0; j<100; j++)
        {
        Vol = Lm/Dens1;
        switch (sm_iDensityMethod)
          {
          case BDM_Original:
            {
            Tna = (dRqd_C/dRqd_CtoS + (Ox/MW_Na2C2O4 + Og/MW_Na2C5O7 + dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4) * MW_Na2CO3)
                  * Vol/(1000.0*Lm)*100.0;     //Wgt % of Total Na
            Tcc = dRqd_C/(Dens1*1000.0)*100.0; //Wgt % of Caustic
            Tal = dRqd_AtoC * Tcc;             //Wgt % of Alumina
            Density25 = CConcentrations::LiquorDensEqn1(25.0, Tna, Tal)/1000.0;
            break;
            }
          case BDM_Alcoa:
            {
            const double A   = dRqd_AtoC*dRqd_C;
            const double S   = dRqd_C/dRqd_CtoS;
            const double C   = dRqd_C;
            const double toc = dRqd_TOC;
            const double SO4 = dRqd_Sul;
            const double Cl  = dRqd_Salt;
            Density25 = CConcentrations::LiquorDensEqn2(Fidelity(), 25.0, A, S, C, toc, SO4, Cl)/1000.0;
            break;
            }
          }

        Derr = fabs(Dens1 - Density25);
        if (Derr < 1e-6)
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
      NaSulphate = dRqd_Sul * Vol;
      Silicate   = dRqd_Sil * Vol;
      Org1       = dRqd_Ox * Vol;
      Org2       = Og * Vol;
      Other      = OtherLiqMass/LiqMass*Vol*Dens1*1000.0;

      double TmpTot = Org1 + Org2 + NaSulphate + Silicate + Salt + Carbonate + Al + Caustic;
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
      NaSulphate /= TmpTot;
      Silicate   /= TmpTot;
      Org1       /= TmpTot;
      Org2       /= TmpTot;
      water      /= TmpTot;
      //-------------------------------------------------------

      // Set masses of the liquid species.
      VValue[Water.LiqPhInx()]            = water*LiqMass;
      VValue[CausticSoda.LiqPhInx()]      = Caustic*LiqMass;
      VValue[Alumina.LiqPhInx()]          = Al*LiqMass;
      VValue[SodiumCarbonate.LiqPhInx()]  = Carbonate*LiqMass;
      VValue[SodiumChloride.LiqPhInx()]   = Salt*LiqMass;
      VValue[SodiumSulphate.LiqPhInx()]   = NaSulphate*LiqMass;
      VValue[SilicateLiq.LiqPhInx()]      = Silicate*LiqMass;
      VValue[SodiumOxalate.LiqPhInx()]    = Org1 * LiqMass;
      VValue[Organics.LiqPhInx()]         = Org2 * LiqMass;
      }
    else
      {
      // Default to water
      VValue[Water.LiqPhInx()]            = LiqMass;
      VValue[CausticSoda.LiqPhInx()]      = 0.0;
      VValue[Alumina.LiqPhInx()]          = 0.0;
      VValue[SodiumCarbonate.LiqPhInx()]  = 0.0;
      VValue[SodiumChloride.LiqPhInx()]   = 0.0;
      VValue[SodiumSulphate.LiqPhInx()]   = 0.0;
      VValue[SilicateLiq.LiqPhInx()]      = 0.0;
      VValue[SodiumOxalate.LiqPhInx()]    = 0.0;
      VValue[Organics.LiqPhInx()]         = 0.0;
      }
    WaterUsed=VValue[Water.LiqPhInx()];
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

double AlcoaBayer::THAMassFlow()
  {
  return m_M[::THA.SolPhInx()];
  }

//---------------------------------------------------------------------------

double AlcoaBayer::THADens(double T_)
  {
  return SDB[::THA.SolPhInx()].Density(Fidelity(), T_, Std_P, &m_Ovr, NULL);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::CausticConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  return Max(0.0, LiqConcs25.Liq[::CausticSoda.LiqPhInx()] * DRatio);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::AluminaConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  return Max(0.0, LiqConcs25.Liq[::Alumina.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::SodaConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  double liq25S = LiqConcs25.Liq[::CausticSoda.LiqPhInx()]
                  //+ LiqConcs25.Liq[::SodiumAluminate.LiqPhInx()]
                  + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()];

  return Max(0.0, liq25S * DRatio); 
  }

//---------------------------------------------------------------------------

double AlcoaBayer::SodiumCarbonateConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  return Max(0.0, LiqConcs25.Liq[SodiumCarbonate.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::SodiumSulphateConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  return Max(0.0, LiqConcs25.Liq[SodiumSulphate.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::SodiumOxalateConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  return Max(0.0, LiqConcs25.Liq[SodiumOxalate.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::SolidsConc(double T_)
  {
  const double mt=Mass(som_Sol);
  return (mt>=UsableMass) ? mt/GTZ(Mass(som_SL))*Rho(som_SL, T_, Press(), pMArray()) : 0.0;
  }

//---------------------------------------------------------------------------

double AlcoaBayer::LVolume25()
  {
  const double mt=Mass(som_Liq);
  return ((mt>=UsableMass) ? (mt / Rho(som_Liq, C_2_K(25.0), Press())) : 0.0);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::SLVolume25()
  {
  const double mt=Mass(som_SL);
  return ((mt>=UsableMass) ? (mt / Rho(som_SL, C_2_K(25.0), Press())) : 0.0);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::TotalSodiumConc()
  {
  //NBNB: Must make equivilant change in DoInputCalcs!
  LiqConcs25.Converge(MArray());
  double Sodium = LiqConcs25.Liq[::CausticSoda.LiqPhInx()] + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]
                  + LiqConcs25.Liq[SodiumOxalate.LiqPhInx()] + LiqConcs25.Liq[Organics.LiqPhInx()]
                  + LiqConcs25.Liq[SodiumChloride.LiqPhInx()] + LiqConcs25.Liq[SodiumSulphate.LiqPhInx()];
  return Sodium;
  }

//---------------------------------------------------------------------------

double AlcoaBayer::OrganateConc()
  {
  LiqConcs25.Converge(MArray());
  double Organate = LiqConcs25.Liq[::Organics.LiqPhInx()];
  return Organate;
  }

//---------------------------------------------------------------------------

double AlcoaBayer::ChlorineConc()
  {
  LiqConcs25.Converge(MArray());
  double Chlorine = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()];
  return Chlorine;
  }

//---------------------------------------------------------------------------

double AlcoaBayer::SulphateConc()
  {
  LiqConcs25.Converge(MArray());
  double Sulphate = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()];
  return Sulphate;
  }

//---------------------------------------------------------------------------

double AlcoaBayer::TotalOrganics()
  {
  LiqConcs25.Converge(MArray());
  double Organics = LiqConcs25.Liq[::Organics.LiqPhInx()] + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()];
  return Organics;
  }

//---------------------------------------------------------------------------

double AlcoaBayer::TOOC(double T_)
  {//TOOC as Na2CO3 equivilent
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  double Tooc = (LiqConcs25.Liq[::Organics.LiqPhInx()]*5.0 + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]*2.0);
  return Max(0.0, Tooc*DRatio);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::TOC(double T_)
  {//TOC
  //TOOC = TOC*106/12
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  double Toc = (LiqConcs25.Liq[::Organics.LiqPhInx()]*5.0 + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]*2.0)*MW_C/::SodiumCarbonate.MW;
  return Max(0.0, Toc*DRatio);
  //for Organics Na2C5O7 : 0.27544 = MW_Carbon*5.0/MW_Organics = 12.011 * 5.0 / 218.030336
  //for SodiumOxalate Na2C2O4 : 0.17927 = MW_Carbon*2.0/MW_SodiumOxalate = 12.011 * 2.0 / 133.999136
  }

//---------------------------------------------------------------------------

double AlcoaBayer::SodiumOxalate_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray());
  double Oxalate = LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()] / LiqConcs25.NaFactor[::SodiumOxalate.LiqPhInx()];
  return Oxalate;
  }

double AlcoaBayer::Sulphate_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray());
  double Sulphate = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()] / LiqConcs25.NaFactor[::SodiumSulphate.LiqPhInx()];
  return Sulphate;
  }

double AlcoaBayer::Chlorine_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray());
  double Chlorine = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()] / LiqConcs25.NaFactor[::SodiumChloride.LiqPhInx()];
  return Chlorine;
  }

//---------------------------------------------------------------------------

double AlcoaBayer::CausticConc_Na2O(double T_)
  {
  return CausticConc(T_)*::OccSoda.MW/::SodiumCarbonate.MW;
  }

double AlcoaBayer::SodaConc_Na2O(double T_)
  {
  return SodaConc(T_)*::OccSoda.MW/::SodiumCarbonate.MW;
  }

double AlcoaBayer::TotalSodiumConc_Na2O()
  {
  return TotalSodiumConc()*::OccSoda.MW/::SodiumCarbonate.MW;
  }

double AlcoaBayer::AtoC_Na2O()
  {
  return AluminaConc(C_2_K(25.0))/GTZ(CausticConc_Na2O(C_2_K(25.0)));
  }

//---------------------------------------------------------------------------

double AlcoaBayer::IonicStrength()
  {
  //Always calculated at 25 !!!
  const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989..
  const double MW_NaCl     = ::SodiumChloride.MW;   //58.4425..
  const double MW_Na2SO4   = ::SodiumSulphate.MW;   //142.043..
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
  LiqConcs25.Converge(MArray());
  const double cTOC    = TOC(C_2_K(25.0));
  const double cNaCl   = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()] /LiqConcs25.NaFactor[::SodiumChloride.LiqPhInx()];
  const double cNa2CO3 = LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumCarbonate.LiqPhInx()];
  const double cNa2SO4 = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()] /LiqConcs25.NaFactor[::SodiumSulphate.LiqPhInx()];
  const double cNaOH   = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];
  I = 0.01887*cNaOH + k1*cNaCl/MW_NaCl + k2*cNa2CO3/MW_Na2CO3 + k3*cNa2SO4/MW_Na2SO4 + k4*0.01887*cTOC;
  return I;
  }

//---------------------------------------------------------------------------

double AlcoaBayer::AluminaConcSat(double T_)
  {
  const double I = IonicStrength();
  const double I2 = Sqrt(I);
  const double cNaOH   = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];
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

double AlcoaBayer::AtoCSaturation(double T_)
  {
  double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AluminaConcSat(T_) / GTZ(C);
  return ACsat;
  }

//---------------------------------------------------------------------------

double AlcoaBayer::SSNRatio(double T_)
  {// AtoC actual / AtoC saturation ==> A/ASat
  const double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AtoCSaturation(T_);
  return AtoC() / GTZ(ACsat);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::AluminaSSN(double T_)
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
//======================================== OxalateEquilibrium ===============================
// Function calculates the equilibrium concentration (g/l) of oxalate, based on stream properties.
// (British Aluminium - Burnt Island)
double AlcoaBayer::OxalateEquilibrium(double T_)//, double Approach)
  {
  const double LiqQv    = LVolume25(); // m^3/s
  if (LiqQv<1.0e-12)
    return 0.0;
  const double C        = CausticConc(C_2_K(25.0)); // g/l
  const double Na2CO3Qm = m_M[SodiumCarbonate.LiqPhInx()]; // kg/s
  const double OxEquil  = 7.62*Exps(0.012*K2C(T_) - (::OccSoda.MW/::SodiumCarbonate.MW)*(0.016*C + 0.011*Na2CO3Qm/GTZ(LiqQv)));
  return OxEquil; //(OxEquil+Approach);
  }

//---------------------------------------------------------------------------

//CToleranceBlock AlcoaBayer::sm_SolTol(TBF_BothSys, "AlcoaBayer:Solubility", 0, 1.0e-12, 100, TBF_Fixed|TBF_UseAbs|TBF_UseRel|TBF_UseMax);

//
//double AlcoaBayer::PerformAluminaSolubility(double TRqd, double ARqd, double THARqd, double NoPerSec, double Na2OFac, double SSat, flag & ConvergeErr)
//  {
//  //   2 AlO2 + 4 H2O <==> Al2O3.3H2O + 2 OH
//  //or Na2O + Al2O3 + 4 H2O <==> Al2O3.3H2O + 2 NaOH
//
//  // x is the Fraction of Alumina which precipitates as the hydrate
//  // ie x is deposition rate of new THA crystal
//
//  bool AdjustT=!Valid(TRqd);
//  double T = AdjustT ? Temp() : TRqd;
//
//  const double ESoda=2535.0; //constant 2535K
//  double A = AluminaConc(T);
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
//  for (int Iter=sm_SolTol.GetMaxIters(); Iter; Iter--)
//    {
//    double x = AluminaMass*(1.0-ARqd/GTZ(A))/Fact;
//
//    //Soda Precipitation...
//    double SodaFac;
//    if (1)
//      {//Ohkawa, Tsuneizumi and Hirao
//      SodaFac = Na2OFac*exp(ESoda/T)*SSat*SSat;
//      }
//    else
//      {//Sang
//      //SSat = (Ain-ASat)/GTZ(Cin);
//		  //==> (Ain-ASat) = SSat/CT
//      //CT is Caustic conc at T 
//      //const double xx = SSat*CT;
//      //SodaFac = (Na2OFac*xx*xx);
//      }
//    double XSoda = x/100.0*SodaFac;
//
//    //check for limit problem (specie availability)...
//    if (Na2OMass + XSoda<1e-9)
//      { //set to limit
//      XSoda = 1e-9 - Na2OMass;
//      x = XSoda*100.0/SodaFac;
//      }
//    if (CausticMass - (MW_NaOH*2)/MW_Na2O*XSoda<1e-12)
//      { //set to limit
//      XSoda = (CausticMass - 1e-12)*MW_Na2O/(MW_NaOH*2);
//      if (Na2OMass + XSoda<1e-9)
//        XSoda = 1e-9 - Na2OMass;
//      x = XSoda*100.0/SodaFac;
//      }
//
//    //adjust masses...
//    AluminaMass = AluminaMass - Fact*x;
//    THAMass     = THAMass     + x;
//    WaterMass   = WaterMass   - (1.0-Fact)*x + MW_H2O/MW_Na2O*XSoda;
//    CausticMass = CausticMass - (MW_NaOH*2)/MW_Na2O*XSoda;
//    Na2OMass    = Na2OMass    + XSoda;
//
//    ClrStatesOK();
//    if (AdjustT)
//      T=Temp();
//    
//    A = AluminaConc(T);
//    if (sm_SolTol.ConvergedVV(A, ARqd))
//      break;
//    if (fabs(x)<1e-22)
//      {//problem!!!
//      Iter=0;
//      break;
//      }
//    }
//  ConvergeErr = (Iter==0);
//  //SetCI(5, ConvergeErr);
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

double AlcoaBayer::FreeCaustic(double T_)
  {
  return CausticConc(T_) * (1.0 - AtoC()*::SodiumCarbonate.MW/::Alumina.MW);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::AtoC()
  {
  LiqConcs25.Converge(MArray());
  return LiqConcs25.Liq[::Alumina.LiqPhInx()] / GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::CtoS()
  {
  LiqConcs25.Converge(MArray());
  return LiqConcs25.Liq[::CausticSoda.LiqPhInx()] 
         / GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()] + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::CltoC()
  {
  LiqConcs25.Converge(MArray());
  return (LiqConcs25.Liq[SodiumChloride.LiqPhInx()])/GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::TOOCtoC()
  {
  double cTOC = TOOC(C_2_K(25.0));
  return (cTOC/GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()]));
  }

//---------------------------------------------------------------------------

double AlcoaBayer::Na2SO4toC()
  {
  LiqConcs25.Converge(MArray());
  return (LiqConcs25.Liq[SodiumSulphate.LiqPhInx()])/GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::BoilingPtElevation(double P_, CSysVector * pMA)
  {
  if (0)
    {
    double A=SaturationT(P_, pMA);
    double B=PureSaturationT(P_, pMA);
    dbgpln("SMBayer::BoilingPtElevation %10.3f %10.3f %10.3f %10.3f", P_, A-B, A, B);
    }
  return SaturationT(P_, pMA) - PureSaturationT(P_, pMA);

  //double T_=Temp();
  //double BPE=BoilPtElev(T_, pMA);
  ////return BPE;
  //return Max(0.0, BPE);
  }

//---------------------------------------------------------------------------

double AlcoaBayer::BoilPtElev(double T_, CSysVector * pMA/*=NULL*/)
  {
  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  double TLiq=MA.Sum(som_Liq); // Total Liquid kg/s
  if (TLiq<1.0e-9)
    return 0.0;

  double BPE = 0.0;
  switch (sm_iBPEMethod)
    {
    case BPM_Dewey:
      {
      if (MA[Alumina.LiqPhInx()]<1.0e-6 && MA[CausticSoda.LiqPhInx()]<1.0e-6 && MA[SodiumCarbonate.LiqPhInx()]<1.0e-6)
        return 0.0;
      // Gram ion per kg liq
      double giAl2O3 = 1000 * MA[Alumina.LiqPhInx()] / TLiq * 2 * MW_NaAlO2 / ::Alumina.MW; // 81.97 = MW of NaAlO2
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
      double gmAl2O3 = giAl2O3 / MW_NaAlO2; // 81.97 = MW of NaAlO2
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
      break;
      }
    case BPM_Adamson:
      {
      //Equation: f(x1,x2) = a+b*x1+c*x2+d*x1^2+e*x2^2+f*x1*x2+g*x1^3+h*x2^3+i*x1*x2^2+j*x1^2*x2
      //Constants a to f determined by equation data fit to above equation based on defined data
      //from published Adamson data, where x1 is Na2O conc g/L @ 25C; and x2 is temp in degC.
      LiqConcs25.Converge(MA);
      if (LiqConcs25.Liq[::CausticSoda.LiqPhInx()]<1.0e-6 && LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]<1.0e-6)
        return 0.0;

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

      double Sodium = LiqConcs25.Liq[::CausticSoda.LiqPhInx()] + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]
                    + LiqConcs25.Liq[SodiumOxalate.LiqPhInx()] + LiqConcs25.Liq[Organics.LiqPhInx()]
                    + LiqConcs25.Liq[SodiumChloride.LiqPhInx()] + LiqConcs25.Liq[SodiumSulphate.LiqPhInx()];
      const double x1 = Sodium*::OccSoda.MW/::SodiumCarbonate.MW;
      const double x2 = K2C(T_);
      const double x1_2 = x1*x1;
      const double x2_2 = x2*x2;
      BPE = a + b*x1 + c*x2 + d*x1_2 + e*x2_2 + f*x1*x2 + g*x1*x1_2 + h*x2*x2_2 + i*x1*x2_2 + j*x1_2*x2;
      break;
      }
    case BPM_Alcoa:
    case BPM_Alcoa2:
      {
      //Based on Alcoa Excel file 'props.xlm' provided by Andrew Maggs and Kristen Dore 30/3/2005
      //Notes in props.xlm indicate original programed by Peter Hay 1998 Alcoa Pinjarra Technical Department
      //B3=Temperature °C
      //B4=Total.Alkali g/l
      //B5=Total.Caustic g/l
      //B6=Alumina g/l
      //B7=TOC g/l
      //B8=Na2SO4 g/l
      //B9=NaCl g/l
      //B10=NaF g/l
      //-----
      //D4=NaOrg/NOOC=3.16
      //D5=MW_NaNOOC=122
      //D6=NaInOrg/TA=0.0236
      //D8=NaOx/TA=0.0125

      //Bayer.Components  Bayer Liquor Components g/l @ 25°C
      //=SET.VALUE(B13,(B5/105.988-B6/101.961)*79.994)	Free NaOH g/l
      //=SET.VALUE(B14,B6*163.941/101.961)	Sodium Aluminate as NaAlO2 g/l
      //=SET.VALUE(B15,B4-B5)	Sodium Carbonate g/l
      //=SET.VALUE(B16,IF(B7>0,B4*$D$8,0))	Sodium Oxalate g/l
      //=SET.VALUE(B17,(B7-B16*24.022/133.999)*$D$4)	Non Oxalate Organic Sodium Salts g/l
      //=SET.VALUE(B18,IF(AND(B8>0,B9>0,B7>0),B4*$D$6,0))	Inorganic Sodium Salts g/l

      //=SET.VALUE(B34,1000*B59)	Total Mass g/l 25°C
      //=SET.VALUE(B35,SUM(B13:B18)+SUM(B8:B9))	Total Solute g/l 25°C
      //=SET.VALUE(B36,B34-B35)	Water g/l 25°C

      //Bayer.VLE.Calc	Subroutine	Ln(Water Activity)=-e^(A-B*(T/C+1)^X)*mf^(D+E*T)
      //=Bayer.Components()		
      //=Bayer.H2O.Calc()		
      //=SET.VALUE(B205,$B$36/18.015)	Moles of water in Bayer Liquor  m/l
      //=SET.VALUE(B206,$B$13/39.997)	Free NaOH
      //=SET.VALUE(B207,$B$14/81.97)	Sodium Aluminate as NaAlO2
      //=SET.VALUE(B208,$B$15/105.988)	Sodium Carbonate
      //=SET.VALUE(B209,$B$16/133.999)	Sodium Oxalate
      //=SET.VALUE(B210,$B$8/142.043)	Na2SO4
      //=SET.VALUE(B211,$B$9/58.442)	NaCl
      //=SET.VALUE(B212,$B$17/$D$5)	Organic Sodium Salts
      //=SET.VALUE(B213,$B$18/$D$7)	Inorganic Sodium Salts
      //=SET.VALUE(B214,2*SUM(B206:B213))	Moles of dissociated ions in Bayer Liquor  m/l
      //=SET.VALUE(B215,B214/(B214+B205))	Mole frac of dissociated ions in Bayer Liquor
      //Bayer.VLE.Calc1		
      //=SET.VALUE(B217,($B$3/375.8+1)^5.17)	(T/C+1)^X
      //=SET.VALUE(B218,3.3697-0.2166*B217)	(A-B*(T/C+1)^X)
      //=SET.VALUE(B219,2.961+0.001856*$B$3)	(D+E*T)
      //=SET.VALUE(B220,EXP(-EXP(B218)*B215^B219))	Activity of Water
      //=SET.VALUE(B221,Water.Psat($B$3))	P0H2O
      //=SET.VALUE(B222,B221*(1-B215)*B220)	PxH2O
      //=RETURN()		

      //=Bayer.VLE.Calc()
      //=SET.VALUE(B162,$B$3-Water.Tsat(B222))
      //=RETURN(B162)
      const double MW_H2O       = ::Water.MW;           //18.0152800
      const double MW_Al2O3     = ::Alumina.MW;         //101.961278
      const double MW_NaOH      = ::CausticSoda.MW;     //39.9971
      const double MW_Na2CO3    = ::SodiumCarbonate.MW; //105.989
      const double MW_NaCl      = ::SodiumChloride.MW;  //58.4425
      const double MW_Na2SO4    = ::SodiumSulphate.MW;  //142.043
      const double MW_Na2C2O4   = ::SodiumOxalate.MW;   //133.999
      const double MW_Na2C5O7   = ::Organics.MW;        //218.030

      LiqConcs25.Converge(MA);
      const double Tc  = K2C(T_);
      const double A   = LiqConcs25.Liq[::Alumina.LiqPhInx()];
      const double S   = LiqConcs25.Liq[::CausticSoda.LiqPhInx()] + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()];
      const double C   = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];
      const double toc = (LiqConcs25.Liq[::Organics.LiqPhInx()]*5.0 + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]*2.0)*MW_C/::SodiumCarbonate.MW;
      const double SO4 = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumSulphate.LiqPhInx()];//NOT as Na2CO3
      const double Cl  = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumChloride.LiqPhInx()];//NOT as Na2CO3
      const double Ox  = LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumOxalate.LiqPhInx()];//NOT as Na2CO3
      const double Og  = LiqConcs25.Liq[::Organics.LiqPhInx()]/LiqConcs25.NaFactor[::Organics.LiqPhInx()];//NOT as Na2CO3

      const double B13 = (C/MW_Na2CO3 - A/MW_Al2O3)*2*MW_NaOH; //Free NaOH  as NaAlO2 g/l
      const double B14 = A*2*MW_NaAlO2/MW_Al2O3; //Sodium Aluminate as NaAlO2 g/l
      const double B15 = S-C; //Sodium Carbonate g/l		
      //const double B16 = (toc>0.0 ? S*sm_dNaOx_to_TA : 0.0); //Sodium Oxalate g/l		
      const double B16 = Ox; //Sodium Oxalate g/l		
      const double B17 = (toc-B16*2*MW_C/MW_Na2C2O4)*sm_dNaOrg_to_NOOC; //Non Oxalate Organic Sodium Salts g/l
      const double B18 = (toc>0.0 && SO4>0.0 && Cl>0.0) ? S*sm_dNaInOrg_to_TA : 0.0; //Inorganic Sodium Salts g/l
      const double rho25 = LiqConcs25.LiquorDensity(C2K(25), MA);
      const double B34 = rho25; //Total Mass g/l 25°C
      double B35 = B13+B14+B15+SO4+Cl;
      if (sm_iBPEMethod==BPM_Alcoa)
        {
        B35 += Ox+Og; //Total Solute g/l 25°C
        }
      else
        {
        B35 += B16+B17+B18; //Total Solute g/l 25°C
        }
      //const double B36 = LiqConcs25.Liq[::Water.LiqPhInx()]; //Water g/l 25°C
      const double B36 = B34-B35; //Water g/l 25°C
      if (B35<1.0e-6)// || B36_<1.0e-6)
        return 0.0;

      const double B205 = B36/MW_H2O; //Moles of water in Bayer Liquor  m/l
      const double B206 = B13/MW_NaOH; //Free NaOH
      const double B207 = B14/MW_NaAlO2; //Sodium Aluminate as NaAlO2
      const double B208 = B15/MW_Na2CO3; //Sodium Carbonate
      //const double B209 = B16/MW_Na2C2O4; //Sodium Oxalate
      const double B209 = Ox/MW_Na2C2O4; //???
      const double B210 = SO4/MW_Na2SO4; //Na2SO4
      const double B211 = Cl/MW_NaCl;	//NaCl
      double B212,B213;
      if (sm_iBPEMethod==BPM_Alcoa)
        {
        B212 = Og/MW_Na2C5O7; //Organic Sodium Salts
        B213 = 0.0; //Inorganic Sodium Salts
        }
      else
        {
        B212 = B17/MW_NaNOOC; //Organic Sodium Salts  (MW NaNOOC = 122)
        B213 = B18/MW_NaInOrg; //Inorganic Sodium Salts  (MW Na(InOrg) = 59.6)
        }
      const double B214 = 2*(B206+B207+B208+B209+B210+B211+B212+B213); //Moles of dissociated ions in Bayer Liquor  m/l
      const double B215 = B214/(B214+B205); //Mole frac of dissociated ions in Bayer Liquor

      const double B217 = Pow(Tc/375.8+1,5.17); //(T/C+1)^X
      const double B218 = 3.3697 - 0.2166*B217; //(A-B*(T/C+1)^X)
      const double B219 = 2.961 + 0.001856*Tc; //(D+E*T)
      const double B220 = Exps(-Exps(B218)*Pow(B215,B219)); //Activity of Water
      const double B221 = SpModelEx::SaturationP(T_, &MA); //P0H2O
      const double B222 = B221*(1-B215)*B220; //PxH2O

      const double tsat = K2C(SpModelEx::SaturationT(B222, &MA));
      BPE = Tc-tsat;
      if (BPE<0.0)
        {
        BPE=0.0;
        }
      break;
      }
    }
  if (BPE<0.0)
    {
    int xx=0;
    }
  return BPE*dBPEFactor;
  }

//---------------------------------------------------------------------------

double AlcoaBayer::TotalSodium(CSysVector * pMA)
  {
  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  LiqConcs25.Converge(MA);
  double TSodium = LiqConcs25.LTotalSodium(MA); // The sodium components in liq.

  return TSodium;
  }

//---------------------------------------------------------------------------

double AlcoaBayer::SaturationP(double T_, CSysVector * pMA, CSaturationDefn * pSatDefn)
  {
  // After Dewey - Light Metals 1981 p 185
  // Employs gram-ion calculation method - see also BpElev

  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  if (pSatDefn && pSatDefn->CmpIndex()>=0)
    return SpModelEx::SaturationP(T_, &MA, pSatDefn);

  if (MA.Sum(som_SL)/GTZ(MA.Sum())<1.0e-6)
    return SpModelEx::SaturationP(T_, &MA);

  #if TestBayerForWater
  // NB This will introduce a nonlinearity
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

double AlcoaBayer::SaturationT(double P_, CSysVector * pMA, CSaturationDefn * pSatDefn)
  {
  // After Dewey - Light Metals 1981 p 185
  // Employs gram-ion calculation method - see also BpElev
  flag Local   = (pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  if (pSatDefn && pSatDefn->CmpIndex()>=0)
    return SpModelEx::SaturationT(P_, &MA, pSatDefn);

  if (MA.Sum(som_SL)/GTZ(MA.Sum())<1.0e-6)
    return SpModelEx::SaturationT(P_, &MA);

  #if TestBayerForWater
  // NB This will introduce a nonlinearity
  double TLiq = MA.Sum(som_Liq); // Total Liquid kg/s
  if ((TLiq>1e-6) && (MA[Water.LiqPhInx()]/TLiq>H2OTestFrac))
    return SpModelEx::SaturationT(P_, pMA);
  #endif

  double SatT = SpModelEx::SaturationT(P_, &MA);
  double BPE = BoilPtElev(SatT, pMA);
  return SatT+BPE;
  }

//---------------------------------------------------------------------------

double AlcoaBayer::Rho(PhMask Phase, double T_, double P_, CSysVector * pMA)
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
    // NB This will introduce a nonlinearity
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

_declspec(dllimport) int SpModelExDbgOn;

double AlcoaBayer::LiqCpCalc(CSysVector & Mn, double T_)
  {
  double Cpl = 0.0;
  LiqConcs25.Converge(Mn);
  const double MLiq = Mn.Mass(som_Liq); //Total Liquid kg/s

  switch (sm_iCPMethod)
    {
    case CPM_Original:
      {
      // Heat Capacity of Bayer Liquor Kcal/kg.degC
      // A function of the weight fraction of TNa and Al2O3
      // From published Russian data

      const double TNaAsCO3    = LiqConcs25.LTotalSodium(Mn);
      const double TNaAsCO3Use = Max(TNaAsCO3, MLiq*0.19); // Only Valid for  TNaAsCO3/FLiq > 0.19
      const double TNa         = 100.0*TNaAsCO3*::OccSoda.MW/::SodiumCarbonate.MW/MLiq; //61.98=MW Na2O, 105.99=MW Na2CO3
      const double TAl2O3      = 100.0*Mn[Alumina.LiqPhInx()]/MLiq;
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
    case CPM_Alcoa:
      {
      //Based on Alcoa Excel file 'props.xlm' provided by Andrew Maggs and Kristen Dore 30/3/2005
      //Notes in props.xlm indicate original programed by Peter Hay 1998 Alcoa Pinjarra Technical Department
      //B10=Total.Alkali g/l
      //B11=Total.Caustic g/l
      //B12=Alumina g/l
      //B13=TOC g/l
      //B14=Na2SO4 g/l
      //B15=NaCl g/l
      //B16=NaF g/l
      //=ARGUMENT(,1,B57) //Temperature (C)
      //=SET.VALUE(B102,$B$10-$B$11)
      //=SET.VALUE(B103,0.000002971*B11^2-0.002826*B11)
      //=SET.VALUE(B104,-0.001209*B12-0.00001961*B12^2+0.00001296*B12*B11)
      //=SET.VALUE(B105,-0.002777*B102+0.00001415*B102*B12)
      //=SET.VALUE(B106,-0.004928*B15-0.002141*B14-0.003536*B13)
      //=SET.VALUE(B107,Water.SpecHeat($B$9))
      //=SET.VALUE(B108,B103+B104+B105+B106+B107)
      const double A   = LiqConcs25.Liq[::Alumina.LiqPhInx()];
      const double S   = LiqConcs25.Liq[::CausticSoda.LiqPhInx()] + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()];
      const double C   = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];
      const double toc = (LiqConcs25.Liq[::Organics.LiqPhInx()]*5.0 + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]*2.0)*MW_C/::SodiumCarbonate.MW;
      const double SO4 = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumSulphate.LiqPhInx()];//NOT as Na2CO3
      const double Cl  = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumChloride.LiqPhInx()];//NOT as Na2CO3
      const double B102 = S - C;
      const double B103 = 0.000002971*C*C - 0.002826*C;
      const double B104 = -0.001209*A - 0.00001961*A*A + 0.00001296*A*C;
      const double B105 = -0.002777*B102 + 0.00001415*B102*A;
      const double B106 = -0.004928*Cl - 0.002141*SO4 - 0.003536*toc;
      const double B107 = SDB[Water.LiqPhInx()].msCp(Fidelity(), T_, Std_P, NULL, NULL);
      Cpl = B103+B104+B105+B106+B107;
      break;
      }
    }

  return Cpl;
  }

//---------------------------------------------------------------------------

double AlcoaBayer::msCp(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
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
    // NB This will introduce a nonlinearity
    if (Mn[Water.LiqPhInx()]/FLiq>H2OTestFrac)
      Cpl = dNAN;
    else
    #endif
      Cpl = LiqCpCalc(Mn, T_);
    }

  double Cps=0.0;
  if (FSol>1.0e-9)
    {//Solids
    Cps = SDB.msCp(m_iFidelity, som_Sol, T_, P_, &m_Ovr, Mn);
    Cps = Max(Cps, dMinSolCp);
    }

  return SDB.msCpMix(m_iFidelity, FSol, Cps, FLiq, Cpl, (1.0-FSol-FLiq), dNAN, T_, P_, &m_Ovr, Mn);
  //return SDB.msCpMix(FSol, dNAN, FLiq, Cpl, (1.0-FSol-FLiq), SpModel::msCp(som_Gas, T_, P_, &Mn), T_, P_, &m_Ovr, Mn);
  }

// ==========================================================================

double AlcoaBayer::msHm(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  flag Local   = (pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);
  if (MA.Sum(som_SL)/GTZ(MA.Sum())<1.0e-6)
    {
    double Ret=SpModel::msHz(Phase, T_, P_, &MA, pTotalM);
    if (0)
      dbgpln("AlcoaBayer::msHm TP:%7.2f %7.2f M:%7.2f =%12.3f", T_, P_, MA.Sum(), Ret);
    return Ret;
    }

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
    // NB This will introduce a nonlinearity
    if (Mn[Water.LiqPhInx()]/FLiq>H2OTestFrac)
      Hl = dNAN;
    else
    #endif
      Hl = LiqCpCalc(Mn, T_) * Tc;
    }
  
  double Hs=0.0;
  if (FSol>1.0e-9)
    {//Solids
    /*double Cps = SDB.msCp(som_Sol, T_, P_, &m_Ovr, Mn);
    Cps = Max(Cps, dMinSolCp);
    Hs = Cps*Tc;*/
    Hs = SDB.msHz(m_iFidelity, som_Sol, T_, P_, &m_Ovr, Mn);
    }

  double Ret=SDB.msHmMix(m_iFidelity, FSol, Hs, FLiq, Hl, (1.0-FSol-FLiq), dNAN, T_, P_, &m_Ovr, Mn);
  if (0)
    dbgpln("AlcoaBayer::msHm %2i S:%7.2f %7.2f L:%7.2f %7.2f TP:%7.2f %7.2f =%12.3f",
      m_iFidelity, FSol, Hs, FLiq, Hl, T_, P_, Ret);

  return Ret;
    //return SDB.msHmMix(FSol, dNAN, FLiq, Hl, (1.0-FSol-FLiq), SpModel::msHz(som_Gas, T_, P_, &Mn), T_, P_, &m_Ovr, Mn);
  }

// --------------------------------------------------------------------------

double AlcoaBayer::SpecieConc(double T_, int iSpNo, PhMask PhaseM)
  {
  if (!SDB[iSpNo].IsLiq())
    return SpModelEx::SpecieConc(T_, iSpNo, PhaseM);
  
  LiqConcs25.Converge(MArray());

  double C = LiqConcs25.Liq[iSpNo];
  
  if (fabs(T_ - C_2_K(25.0))>0.1)
    return C * LiqConcs25.LiquorDensity(T_, MArray())/ GTZ(LiqConcs25.Density);  
  return C;
  }

// --------------------------------------------------------------------------

double AlcoaBayer::PhaseConc(double T_, PhMask Ph, PhMask PhaseM)
  {
  // To Be Improved
  return SpModelEx::PhaseConc(T_, Ph, PhaseM);
  }

//--------------------------------------------------------------------------

flag AlcoaBayer::CIStrng(int No, pchar & pS)
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

// ==========================================================================
//
//                                  End
//
// ==========================================================================
//

