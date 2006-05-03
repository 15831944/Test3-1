//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __ALUNORTESPMDL_CPP
#include "sp_db.h"
#include "alunortespmdl.h"
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
// Generic Bayer Specie Model.
//
// ==========================================================================

IMPLEMENT_SPMODEL(AlunorteBayer, "AlBayer", "", TOC_ALL|TOC_GRP_ALUMINA|TOC_USER|TOC_ALUMINA, "Alunorte Bayer", "Alunorte Bayer Properties Model");
IMPLEMENT_SPARES(AlunorteBayer, 100);

static MInitialiseTest InitTest(&AlunorteBayerClass);

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

// ==========================================================================

enum DLM_DefineLiquorMethod { DLM_TotOrganics, DLM_TOC };
enum BDM_BayerDensityMethod { BDM_Original, BDM_AlunorteFeb2003 };
enum BPM_BPEMethod { BPM_Dewey, BPM_Adamson, BPM_AlunorteFeb2003 };
enum CPM_CpMethod { CPM_Original, CPM_AlunorteFeb2003 };
enum VSM_ViscosityMethod { VSM_Standard, VSM_AlunorteFeb2003 };

double AlunorteBayer::dBPEFactor = 1.0;
double AlunorteBayer::dRhoFactor = 1.0;
double AlunorteBayer::sm_dMinSolCp = 1.046;
byte AlunorteBayer::sm_iRqdCalcMethod = DLM_TOC;
byte AlunorteBayer::sm_iBPEMethod = BPM_Dewey;
byte AlunorteBayer::sm_iDensityMethod = BDM_Original;
byte AlunorteBayer::sm_iCpMethod = CPM_Original;
byte AlunorteBayer::sm_iViscMethod = VSM_Standard;

// ==========================================================================
//
// A special class to assist with calculating iterative concentration and density
// calculations.
//
// ==========================================================================

flag     CConcentrations::NaFactorOK=0;
SpVector CConcentrations::NaFactor(true);

CConcentrations::CConcentrations(AlunorteBayer *pMdl) 
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

  static CToleranceBlock s_Tol(TBF_Both, "AlunorteSpMdl:Density", 1e-12, 1e-8, 100);

  const double Tc = 25.0;
  const double T_ = C2K(Tc);
  // Converge Liquor Conc. All sodium concentrations expressed as Na2CO3
  int IterCount = s_Tol.GetMaxIters();//100;
  double OldDensity = Density;
  while (1)
    {
    for (int sn=0; sn<SDB.Count(); sn++)
      {
      if (SDB[sn].IsLiq())
        Liq[sn] = MA[sn] / TLiq * Density * NaFactor[sn];
        }
    Density = Range(0.0001, LiquorDensity(T_, MA), 10000.0);
    if (fabs(OldDensity - Density) < s_Tol.GetAbs() || --IterCount==0)
      break;
    OldDensity = Density;
    } // end of while

  Density = Max(0.001, Density);
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
         + MA[Organics.LiqPhInx()]       * NaFactor[::Organics.LiqPhInx()] );
         //+ MA[SodiumSilicate.LiqPhInx()] * NaFactor[::SodiumSilicate.LiqPhInx()] );
  return TSodium;
  }

// --------------------------------------------------------------------------

double CConcentrations::LiquorDensEqn1(double Tc, double WTNa, double WAlumina)
  {
  const double RHO25 = 0.982
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

double CConcentrations::LiquorDensEqn2(double Tc, double Al2O3, double Caust, double TTS, double TOC)
  {
  double RHO = 1.02767 - 0.0007015008 * Tc - 0.0000007140195 * Tc * Tc 
        + 0.0003631629 * Al2O3 + 0.0000003396664 * Al2O3 * Al2O3 
        - 0.00005931031 * Caust
        + 0.000794539 * TTS - 0.0000002692805 * TTS * TTS
        + 0.002644698 * TOC + 0.0000003134875 * Tc * Caust
        + 0.04716374 * Al2O3 / GTZ(Caust);
  return (RHO * 1000.0); // kg/m^3
  }

// --------------------------------------------------------------------------

double CConcentrations::LiquorDensity(double T_, CSysVector & MA)
  {
  double TLiq=MA.Sum(som_Liq);        //Total Liquid
  if (TLiq<1.0e-9)
    return 0.0;
  
  const double Tc = K_2_C(T_);                // Convert from Kelvin
  double RHO = 0.0;
  switch (AlunorteBayer::sm_iDensityMethod)
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
      RHO = LiquorDensEqn1(Tc, WTNa, WAlumina);
      #if dbgModels
      if (dbgSpecies())
        dbgpln("Liq Dens Tc:%10.2f WTNa:%12.6g WAlumina:%12.6g RHO:%12.6g", Tc, WTNa, WAlumina, RHO);
      #endif
      break;
      }
    case BDM_AlunorteFeb2003:
      {//Alunorte Feb2003
      /*
      SysCADBayerDen = GetTag("P_2.Qi.LRho25 (kg/m^3)")
      Al2O3_gpl = GetTag("P_2.Qi.A (g/L)")
      Caust_gpl = GetTag("P_2.Qi.C (g/L)")
      OC_gpl = GetTag("P_2.Qi.TOC (g/L)")
      Tot_soda = GetTag("P_2.Qi.TotalNa (g/L)")  
      TTS_gpl = Tot_soda

      BAYER_den = 1.02767 - 0.0007015008 * Temp_C - 0.0000007140195 * Temp_C * Temp_C 
                  + 0.0003631629 * Al2O3_gpl + 0.0000003396664 * Al2O3_gpl * Al2O3_gpl 
                  - 0.00005931031 * Caust_gpl 
                  + 0.000794539 * TTS_gpl - 0.0000002692805 * TTS_gpl * TTS_gpl
                  + 0.002644698 * OC_gpl + 0.0000003134875 * Temp_C * Caust_gpl 
                  + 0.04716374 * Al2O3_gpl / Caust_gpl
      */
      /*const double TTS = Liq[CausticSoda.LiqPhInx()]
          + Liq[SodiumCarbonate.LiqPhInx()]
          + Liq[SodiumOxalate.LiqPhInx()]
          + Liq[SodiumChloride.LiqPhInx()]
          + Liq[SodiumSulphate.LiqPhInx()]
          + Liq[Organics.LiqPhInx()];//TotalSodiumConc();
      const double Al2O3 = Liq[::Alumina.LiqPhInx()];//AluminaConc(C_2_K(25.0));
      const double Caust = Liq[::CausticSoda.LiqPhInx()];//CausticConc(C_2_K(25.0));
      const double TOC = (Liq[::Organics.LiqPhInx()]*5.0 + Liq[::SodiumOxalate.LiqPhInx()]*2.0)*MW_C/::SodiumCarbonate.MW;//TOC(C_2_K(25.0));
      RHO = LiquorDensEqn2(Tc, Al2O3, Caust, TTS, TOC);*/
      const double TTS = Liq[CausticSoda.LiqPhInx()] + Liq[SodiumCarbonate.LiqPhInx()];
      const double Al2O3 = Liq[::Alumina.LiqPhInx()];
      const double Caust = Liq[::CausticSoda.LiqPhInx()];
      const double TOC = (Liq[::Organics.LiqPhInx()]*5.0 + Liq[::SodiumOxalate.LiqPhInx()]*2.0)*MW_C/::SodiumCarbonate.MW;
      RHO = LiquorDensEqn2(Tc, Al2O3, Caust, TTS, TOC);
      break;
      }
    }
  return  RHO;
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
const word xidRqdTOC              = 30042;
const word xidRqdOrgRatio         = 30043;
const word xidRqdOxalate          = 30044;
const word xidOxalate_Conc25      = 30045;
const word xidNaCl_Conc25         = 30046;
const word xidNa2SO4_Conc25       = 30047;
const word xidCausticConc25_Na2O  = 30048;
const word xidSodaConc25_Na2O     = 30049;
const word xidTotalNaConc25_Na2O  = 30050;
const word xidAtoC25_Na2O         = 30051;
const word xidBayerBPE            = 30052;

CArray <int, int> AlunorteBayer::LclParm;

AlunorteBayer::AlunorteBayer(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach):
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
  dRqd_Org  = 50.0;
  dRqd_Salt = 15.0;
  dRqd_SolFrac  = 0.0;
  dRqd_OrgRatio = 0.9;
  }

// --------------------------------------------------------------------------

AlunorteBayer::~AlunorteBayer()
  {
  }

// --------------------------------------------------------------------------

DDEF_Flags AlunorteBayer::FixSpEditFlags(DDEF_Flags Flags, int iSpid, flag AsParms)
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

void AlunorteBayer::BuildDataDefn_Vars(DataDefnBlk & DDB)
  {
  static DDBValueLst DDB0[]={
    {BPM_Dewey, "Dewey"},
    {BPM_Adamson, "Adamson"},
    {BPM_AlunorteFeb2003, "AlunorteFeb2003"},
    {0}};
  static DDBValueLst DDB1[]={
    {DLM_TotOrganics, "TotOrganics and Ratio"},
    {DLM_TOC, "TOC and Oxalate"},
    {0}};
  static DDBValueLst DDB2[]={
    {BDM_Original, "Original"},
    {BDM_AlunorteFeb2003, "AlunorteFeb2003"},
    {0}};
  static DDBValueLst DDB3[]={
    {CPM_Original, "Original"},
    {CPM_AlunorteFeb2003, "AlunorteFeb2003"},
    {0}};
  static DDBValueLst DDB4[]={
    {VSM_Standard, "Standard"},
    {VSM_AlunorteFeb2003, "AlunorteFeb2003"},
    {0}};

  if (UseAsMass() || UseAsImage())
    {
    DDB.Text("Global Bayer Constants and Options...");
    DDB.Byte    ("BPE_Method",      "",   DC_,      "",        &sm_iBPEMethod,   this,  isParm, DDB0); //global constant
    DDB.Byte    ("DensityMethod",   "",   DC_,      "",        &sm_iDensityMethod, this, isParm|SetOnChange, DDB2); //global constant
    DDB.Byte    ("Cp_Method",       "",   DC_,      "",        &sm_iCpMethod,    this,  isParm, DDB3); //global constant
    DDB.Byte    ("ViscosityMethod", "",   DC_,      "",        &sm_iViscMethod,  this,  isParm, DDB4); //global constant
    DDB.Double  ("BPE_Factor",      "",   DC_Frac,  "%",       &dBPEFactor,      this,  isParm|InitHidden); //global constant
    DDB.Double  ("Rho_Factor",      "",   DC_Frac,  "%",       &dRhoFactor,      this,  isParm|InitHidden); //global constant
    DDB.Double  ("MinSolidsCp",     "",   DC_CpMs,  "kJ/kg.C", &sm_dMinSolCp,    this,  isParm|InitHidden); //global constant
    DDB.Text("Bayer");
    DDB.CheckBoxBtn("DefineLiquor", "",   DC_,      "",        &fDoCalc,         this,  isParm|SetOnChange);
    DDB.Byte    ("DefnLiqMethod",   "",   DC_,      "",        &sm_iRqdCalcMethod, this, isParm|SetOnChange, DDB1); //global constant
    DDB.Visibility(SHM_All, fDoCalc);                                                         
    DDB.Double  ("Rqd_A/C",         "",   DC_,      "",        &dRqd_AtoC,       this,  isParm);
    DDB.Double  ("Rqd_C/S",         "",   DC_,      "",        &dRqd_CtoS,       this,  isParm);
    DDB.Double  ("Rqd_C",           "",   DC_Conc,  "g/L",     &dRqd_C,          this,  isParm);
    DDB.Double  ("Rqd_SiO2",        "",   DC_Conc,  "g/L",     &dRqd_Sil,        this,  isParm);
    DDB.Double  ("Rqd_Organic",     "",   DC_Conc,  "g/L",     &dRqd_Org,        this,  sm_iRqdCalcMethod==DLM_TOC ? isResult : isParm);
    DDB.Double  ("Rqd_TOC",         "",   DC_Conc,  "g/L",     xidRqdTOC,        this,  (sm_iRqdCalcMethod==DLM_TOC ? isParm : isResult)|noFile|noSnap);
    DDB.Double  ("Rqd_OrgRatio",    "",   DC_Frac,  "%",       xidRqdOrgRatio,   this,  sm_iRqdCalcMethod==DLM_TOC ? isResult : isParm);
    DDB.Double  ("Rqd_Oxalate",     "",   DC_Conc,  "g/L",     xidRqdOxalate,    this,  (sm_iRqdCalcMethod==DLM_TOC ? isParm : isResult)|noFile|noSnap);
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
  DDB.Double("TOOC",          "",   DC_Conc,  "g/L",     xidSlurryTOOC25,        this,  isResult|noFile|noSnap|InitHidden);
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
  DDB.Double("BoilPtElev",    "",   DC_dT,    "C",       xidBayerBPE,            this,  isResult|noFile);
  
  DDB.Text("");
  DDB.Text("Bayer Liquor Precipitation Values @ Temp");
  DDB.Double("A_Saturation", "ASat",DC_Conc,  "g/L",     xidASaturation,         this,  isResult|noFile|noSnap);
  DDB.Double("A/C_Saturation", "A/CSat", DC_, "",        xidAtoCSaturation,      this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("SSN_Ratio",     "",   DC_,      "",        xidSSNRatio,            this,  isResult|noFile|noSnap);
  //DDB.Double("Alumina_SSN",   "",   DC_,      "",        xidAluminaSSN,          this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("IonicStrength", "I",  DC_,      "",        xidIonicStrength,       this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("OxalateEq",     "",     DC_Conc,  "g/L",   xidOxalateEquilibrium,  this,  isResult|noFile|noSnap|InitHidden);

  }

// --------------------------------------------------------------------------

flag AlunorteBayer::DataXchg(DataChangeBlk & DCB)
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
    case xidBayerBPE:              DCB.D=BoilPtElev(Temp()); return 1;

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
          dRqd_OrgRatio = Range(0.0, *DCB.rD, 1.0);
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

flag AlunorteBayer::ValidateData(ValidateDataBlk & VDB)
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
    dRqd_Org  = ValidateRange(VDB, "dRqd_Org", 0.0, dRqd_Org,     500.0);
    //dRqd_OrgRatio = ValidateRange(VDB, "dRqd_OrgRatio", 0.0, dRqd_OrgRatio, 1.0);
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

void AlunorteBayer::DoInputCalcs(double T_)
  {
  switch (GetView())
    {
    case SVV_AsRawFrac:
    case SVV_AsRawMass:
    case SVV_AsRawMassFlow:
      fDoCalc=false;
      return;
    }

  const double Tcent = K2C(T_);

  //calculator to determine Liquor feed fractions based on requirements at 25C...
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

    if (dRqd_OrgRatio>1.0)
      {
      LogWarning(FullObjTag(), 0, "Required Organic Ratio adjusted (cannot be greater than 1)");
      dRqd_OrgRatio = 1.0;
      }
    Ox = dRqd_Org * dRqd_OrgRatio;
    Og = dRqd_Org - Ox;

    if (dRqd_C > 1e-6)
      {
      const double MW_NaOH     = ::CausticSoda.MW;      //39.9971
      const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989
      const double MW_NaCl     = ::SodiumChloride.MW;   //58.4425
      const double MW_Na2SO4   = ::SodiumSulphate.MW;   //142.043
      const double MW_Na2C2O4  = ::SodiumOxalate.MW;    //133.999
      const double MW_Na2C5O7  = ::Organics.MW;         //218.030
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
            Tna = (dRqd_C/dRqd_CtoS + Ox + Og + (dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4) * MW_Na2CO3)
                  * Vol/(1000.0*Lm)*100.0;     //Wgt % of Total Na
            Tcc = dRqd_C/(Dens1*1000.0)*100.0; //Wgt % of Caustic
            Tal = dRqd_AtoC * Tcc;             //Wgt % of Alumina
            Density25 = CConcentrations::LiquorDensEqn1(25.0, Tna, Tal)/1000.0;
            break;
            }
          case BDM_AlunorteFeb2003:
            {
            Tna = (dRqd_C/dRqd_CtoS + Ox + Og + (dRqd_Salt/(2*MW_NaCl) + dRqd_Sul/MW_Na2SO4) * MW_Na2CO3);
            double TOC = dRqd_Org*MW_C/MW_Na2CO3*(5.0 - (3.0 * dRqd_OrgRatio));
            Density25 = CConcentrations::LiquorDensEqn2(25.0, dRqd_AtoC*dRqd_C, dRqd_C, Tna, TOC)/1000.0;
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
      Org1       = Ox * MW_Na2C2O4/MW_Na2CO3 * Vol;
      Org2       = Og * MW_Na2C5O7/MW_Na2CO3 * Vol;
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
      VValue[Organics.LiqPhInx()]         = Org2 * LiqMass;
      VValue[SodiumOxalate.LiqPhInx()]    = Org1 * LiqMass;
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

double AlunorteBayer::THAMassFlow()
  {
  return m_M[::THA.SolPhInx()];
  }

//---------------------------------------------------------------------------

double AlunorteBayer::THADens(double T_)
  {
  return SDB[::THA.SolPhInx()].Density(Fidelity(), T_, Std_P, &m_Ovr, NULL);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::CausticConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  return Max(0.0, LiqConcs25.Liq[::CausticSoda.LiqPhInx()] * DRatio);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::AluminaConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  return Max(0.0, LiqConcs25.Liq[::Alumina.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::SodaConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  double liq25S = LiqConcs25.Liq[::CausticSoda.LiqPhInx()]
                  //+ LiqConcs25.Liq[::SodiumAluminate.LiqPhInx()]
                  + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()];

  return Max(0.0, liq25S * DRatio); 
  }

//---------------------------------------------------------------------------

double AlunorteBayer::SodiumCarbonateConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  return Max(0.0, LiqConcs25.Liq[SodiumCarbonate.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::SodiumSulphateConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  return Max(0.0, LiqConcs25.Liq[SodiumSulphate.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::SodiumOxalateConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  return Max(0.0, LiqConcs25.Liq[SodiumOxalate.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::SolidsConc(double T_)
  {
  const double mt=Mass(som_Sol);
  return (mt>=UsableMass) ? mt/GTZ(Mass(som_SL))*Rho(som_SL, T_, Press(), pMArray()) : 0.0;
  }

//---------------------------------------------------------------------------

double AlunorteBayer::LVolume25()
  {
  const double mt=Mass(som_Liq);
  return ((mt>=UsableMass) ? (mt / Rho(som_Liq, C_2_K(25.0), Press())) : 0.0);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::SLVolume25()
  {
  const double mt=Mass(som_SL);
  return ((mt>=UsableMass) ? (mt / Rho(som_SL, C_2_K(25.0), Press())) : 0.0);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::TotalSodiumConc()
  {
  //NBNB: Must make equivilant change in DoInputCalcs!
  LiqConcs25.Converge(MArray());
  double Sodium = LiqConcs25.Liq[::CausticSoda.LiqPhInx()] + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]
                  + LiqConcs25.Liq[SodiumOxalate.LiqPhInx()] + LiqConcs25.Liq[Organics.LiqPhInx()]
                  + LiqConcs25.Liq[SodiumChloride.LiqPhInx()] + LiqConcs25.Liq[SodiumSulphate.LiqPhInx()];
  return Sodium;
  }

//---------------------------------------------------------------------------

double AlunorteBayer::OrganateConc()
  {
  LiqConcs25.Converge(MArray());
  double Organate = LiqConcs25.Liq[::Organics.LiqPhInx()];
  return Organate;
  }

//---------------------------------------------------------------------------

double AlunorteBayer::ChlorineConc()
  {
  LiqConcs25.Converge(MArray());
  double Chlorine = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()];
  return Chlorine;
  }

//---------------------------------------------------------------------------

double AlunorteBayer::SulphateConc()
  {
  LiqConcs25.Converge(MArray());
  double Sulphate = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()];
  return Sulphate;
  }

//---------------------------------------------------------------------------

double AlunorteBayer::TotalOrganics()
  {
  LiqConcs25.Converge(MArray());
  double Organics = LiqConcs25.Liq[::Organics.LiqPhInx()] + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()];
  return Organics;
  }

//---------------------------------------------------------------------------

double AlunorteBayer::TOOC(double T_)
  {//TOOC as Na2CO3 equivilent
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  double Tooc = (LiqConcs25.Liq[::Organics.LiqPhInx()]*5.0 + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]*2.0);
  return Max(0.0, Tooc*DRatio);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::TOC(double T_)
  {//TOC
  //TOOC = TOC*106/12
  LiqConcs25.Converge(MArray());
  double DRatio = (T_==C_2_K(25.0) ? 1.0 : LiqConcs25.LiquorDensity(T_, MArray())/GTZ(LiqConcs25.Density));
  double Toc = (LiqConcs25.Liq[::Organics.LiqPhInx()]*5.0 + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]*2.0)*MW_C/::SodiumCarbonate.MW;
#ifndef _RELEASE
  double Toc_ = (LiqConcs25.Liq[::Organics.LiqPhInx()]/LiqConcs25.NaFactor[::Organics.LiqPhInx()]*MW_C*5.0/::Organics.MW + 
                 LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumOxalate.LiqPhInx()]*MW_C*2.0/::SodiumOxalate.MW);
  //ASSERT(Toc==Toc_);
#endif
  return Max(0.0, Toc*DRatio);
  //for Organics Na2C5O7 : 0.27544 = MW_Carbon*5.0/MW_Organics = 12.011 * 5.0 / 218.030336
  //for SodiumOxalate Na2C2O4 : 0.17927 = MW_Carbon*2.0/MW_SodiumOxalate = 12.011 * 2.0 / 133.999136
  }

//---------------------------------------------------------------------------

double AlunorteBayer::SodiumOxalate_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray());
  double Oxalate = LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()] / LiqConcs25.NaFactor[::SodiumOxalate.LiqPhInx()];
  return Oxalate;
  }

double AlunorteBayer::Sulphate_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray());
  double Sulphate = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()] / LiqConcs25.NaFactor[::SodiumSulphate.LiqPhInx()];
  return Sulphate;
  }

double AlunorteBayer::Chlorine_Conc()
  {//NOT as Na2CO3
  LiqConcs25.Converge(MArray());
  double Chlorine = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()] / LiqConcs25.NaFactor[::SodiumChloride.LiqPhInx()];
  return Chlorine;
  }

//---------------------------------------------------------------------------

double AlunorteBayer::CausticConc_Na2O(double T_)
  {
  return CausticConc(T_)*::OccSoda.MW/::SodiumCarbonate.MW;
  }

double AlunorteBayer::SodaConc_Na2O(double T_)
  {
  return SodaConc(T_)*::OccSoda.MW/::SodiumCarbonate.MW;
  }

double AlunorteBayer::TotalSodiumConc_Na2O()
  {
  return TotalSodiumConc()*::OccSoda.MW/::SodiumCarbonate.MW;
  }

double AlunorteBayer::AtoC_Na2O()
  {
  return AluminaConc(C_2_K(25.0))/GTZ(CausticConc_Na2O(C_2_K(25.0)));
  }

//---------------------------------------------------------------------------

double AlunorteBayer::IonicStrength()
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

double AlunorteBayer::AluminaConcSat(double T_)
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

double AlunorteBayer::AtoCSaturation(double T_)
  {
  double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AluminaConcSat(T_) / GTZ(C);
  return ACsat;
  }

//---------------------------------------------------------------------------

double AlunorteBayer::SSNRatio(double T_)
  {// AtoC actual / AtoC saturation ==> A/ASat
  const double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AtoCSaturation(T_);
  return AtoC() / GTZ(ACsat);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::AluminaSSN(double T_)
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
double AlunorteBayer::OxalateEquilibrium(double T_)//, double Approach)
  {
  const double LiqQv    = LVolume25(); // m^3/s
  if (LiqQv<1.0e-12)
    return 0.0;
  const double C        = CausticConc(C_2_K(25.0)); // g/l
  const double Na2CO3Qm = m_M[SodiumCarbonate.LiqPhInx()]; // kg/s
  const double OxEquil  = 7.62*Exps(0.012*K2C(T_) - (::OccSoda.MW/::SodiumCarbonate.MW)*(0.016*C + 0.011*Na2CO3Qm/LiqQv));
  return OxEquil; //(OxEquil+Approach);
  }

//---------------------------------------------------------------------------

//CToleranceBlock AlunorteBayer::sm_SolTol(TBF_BothSys, "AlunorteBayer:Solubility", 0, 1.0e-12, 100, TBF_Fixed|TBF_UseAbs|TBF_UseRel|TBF_UseMax);

//
//double AlunorteBayer::PerformAluminaSolubility(double TRqd, double ARqd, double THARqd, double NoPerSec, double Na2OFac, double SSat, flag & ConvergeErr)
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

double AlunorteBayer::FreeCaustic(double T_)
  {
  return CausticConc(T_) * (1.0 - AtoC()*::SodiumCarbonate.MW/::Alumina.MW);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::AtoC()
  {
  LiqConcs25.Converge(MArray());
  return LiqConcs25.Liq[::Alumina.LiqPhInx()] / GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::CtoS()
  {
  LiqConcs25.Converge(MArray());
  return LiqConcs25.Liq[::CausticSoda.LiqPhInx()] 
         / GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()] + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::CltoC()
  {
  LiqConcs25.Converge(MArray());
  return (LiqConcs25.Liq[SodiumChloride.LiqPhInx()])/GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::TOOCtoC()
  {
  double cTOC = TOOC(C_2_K(25.0));
  return (cTOC/GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()]));
  }

//---------------------------------------------------------------------------

double AlunorteBayer::Na2SO4toC()
  {
  LiqConcs25.Converge(MArray());
  return (LiqConcs25.Liq[SodiumSulphate.LiqPhInx()])/GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::BoilingPtElevation(double P_, CSysVector * pMA)
  {
  double T_=Temp();
  double BPE=BoilPtElev(T_, pMA);
  //return BPE;
  return Max(0.0, BPE);
  }

//---------------------------------------------------------------------------

double AlunorteBayer::BoilPtElev(double T_, CSysVector * pMA)
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
      break;
      }
    case BPM_Adamson:
      {
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
      break;
      }
    case BPM_AlunorteFeb2003:
      {//Alunorte Feb2003
      /*
      Double SysCADBayerBPE*@("T", "C")
      Double BAYER_bpe*@("T", "C"), S0, S1, Temp_C("T", "C")
      Double Tot_Soda("Conc","g/L")  	

      SysCADBayerBPE = GetTag("P_2.Qi.BPE (C)")
      Temp_C = GetTag("P_2.T (C)")
      Tot_soda = GetTag("P_2.Qi.TotalNa (g/L)")  

      S0 = 0.00663283 * Tot_soda + 0.0000671229 * Tot_soda * Tot_soda
      S1 = 0.028637 * Tot_soda + 0.0000517277 * Tot_soda * Tot_soda
      BAYER_bpe = ((S1 - S0) * Temp_C + 100 * S0) / (S1 - S0 + 180)
      */
      const double Tc = K_2_C(T_);
      const double Tot_soda = TotalSodiumConc();
      const double S0 = 0.00663283 * Tot_soda + 0.0000671229 * Tot_soda * Tot_soda;
      const double S1 = 0.028637 * Tot_soda + 0.0000517277 * Tot_soda * Tot_soda;
      BPE = ((S1 - S0) * Tc + 100.0 * S0) / GTZ(S1 - S0 + 180.0);
      break;
      }
    }
  return BPE*dBPEFactor;
  }

//---------------------------------------------------------------------------

double AlunorteBayer::DynamicViscosity(PhMask Phase, double T_, double P_, CSysVector * pMA/*=NULL*/) 
  {
  switch (sm_iViscMethod)
    {
    case VSM_Standard:
      {
      return SMBayerBase::DynamicViscosity(Phase, T_, P_, pMA); 
      }
    case VSM_AlunorteFeb2003:
      {//Alunorte Feb2003
      /*
      Double SysCADLiqVisc*@("Visc", "cP")
      Double BAYER_u*@("Visc", "cP")
      Double IntermediateTerm

      SysCADLiqVisc = GetTag("P_2.Qi.LViscosity (cP)")
      Al2O3_gpl = GetTag("P_2.Qi.A (g/L)")
      Caust_gpl = GetTag("P_2.Qi.C (g/L)")
      OC_gpl = GetTag("P_2.Qi.TOC (g/L)")
      Tot_soda = GetTag("P_2.Qi.TotalNa (g/L)")  
      TTS_gpl = Tot_soda

      IntermediateTerm = 0.469882 - 0.02917544 * Temp_C + 0.0001328633 * Temp_C * Temp_C 
                        + 0.005523669 * Al2O3_gpl + 0.000007068143 * Al2O3_gpl * Al2O3_gpl 
                        + 0.005137472 * TTS_gpl - 0.00000120593 * TTS_gpl * TTS_gpl 
                        + 0.002069483 * OC_gpl 
                        - 0.00002033289 * Caust_gpl * Temp_C 
                        - 0.00004255638 * Al2O3_gpl / Caust_gpl

      BAYER_u = Exp(IntermediateTerm)
      */

      double mv=0;//pMA ? pMA->MassFrac(Phase) : Mass(Phase);
      double ml=0;//pMA ? pMA->MassFrac(Phase) : Mass(Phase);
      double VVisc=0.0;
      double LVisc=0.0;
      double Tc = K_2_C(T_);
      if (Phase & som_Gas)
        {
        mv=pMA ? pMA->Mass(som_Gas) : Mass(som_Gas);
        // ********** Vapour viscosity in mPa.s = cP
        if (m_Ovr.Use(SPO_VDynVisc))
          VVisc=m_Ovr.fVDynVisc/0.001; // will multiply by 0.001 later
        else if (Tc < 250)
          VVisc= 0.00003 * Tc + 0.0089;
        else if (Tc >= 250 && Tc < 310)
          VVisc= 0.00004 * Tc + 0.0062;
        else //if (Tc >= 310)
          VVisc= 0.000003 * Sqr(Tc) - 0.0017 * Tc + 0.2823;
      //  return Visc * 0.001;  // >> Pa.s
        }
      if (Phase & som_SL)
        {
        ml=pMA ? pMA->Mass(som_SL) : Mass(som_SL);
        /*if (Ovr.Use(SPO_LVisc))
          LVisc=Ovr.fLVisc;
        // ********** Water viscosity in mPa.s = cP
        else if (Tc > 0.01 && Tc < 100)
          LVisc = 1.4851 * Exps(-0.018 * Tc);
        else if (Tc >= 100 && Tc < 200)
          LVisc = 0.5629 * Exps(-0.0073 * Tc);
        else if (Tc >= 200 && Tc < 370)
          LVisc = -0.0004 * Tc + 0.2188;
        else
          LVisc = 1; // Hard Wired;*/
      
        if (m_Ovr.Use(SPO_LDynVisc))
          LVisc=m_Ovr.fLDynVisc/0.001; // will multiply by 0.001 later
        else
          {
          const double Tc = K_2_C(T_);
          //const double TTS = TotalSodiumConc();
          const double TTS = SodaConc(C_2_K(25.0));
          const double Al2O3 = AluminaConc(C_2_K(25.0));
          const double Caust = CausticConc(C_2_K(25.0));
          const double TOC_ = TOC(C_2_K(25.0));
          const double IntermediateTerm = 0.469882 - 0.02917544 * Tc + 0.0001328633 * Tc * Tc 
                          + 0.005523669 * Al2O3 + 0.000007068143 * Al2O3 * Al2O3 
                          + 0.005137472 * TTS - 0.00000120593 * TTS * TTS 
                          + 0.002069483 * TOC_ 
                          - 0.00002033289 * Caust * Tc 
                          - 0.00004255638 * Al2O3 / GTZ(Caust);
          LVisc = Exps(IntermediateTerm);
          }
        }

      //TODO THIS IS NOT REALLY CORRECT
      double VFrac=mv/GTZ(mv+ml);
      return (VVisc*VFrac+LVisc*(1-VFrac))*0.001;
      }
    }
  return SMBayerBase::DynamicViscosity(Phase, T_, P_, pMA); 
  }

//---------------------------------------------------------------------------

double AlunorteBayer::ThermalConductivity(PhMask Phase, double T_, double P_, CSysVector * pMA/*=NULL*/) 
  {
  return SMBayerBase::ThermalConductivity(Phase, T_, P_, pMA); 
  }
//---------------------------------------------------------------------------

double AlunorteBayer::TotalSodium(CSysVector * pMA)
  {
  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  LiqConcs25.Converge(MA);
  double TSodium = LiqConcs25.LTotalSodium(MA); // The sodium components in liq.

  return TSodium;
  }

//---------------------------------------------------------------------------

double AlunorteBayer::SaturationP(double T_, CSysVector * pMA)
  {
  // After Dewey - Light Metals 1981 p 185
  // Employs gram-ion calculation method - see also BpElev

  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

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

double AlunorteBayer::SaturationT(double P_, CSysVector * pMA)
  {
  // After Dewey - Light Metals 1981 p 185
  // Employs gram-ion calculation method - see also BpElev
  flag Local   = (pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

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

double AlunorteBayer::Rho(PhMask Phase, double T_, double P_, CSysVector * pMA)
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

double AlunorteBayer::LiqCpCalc(CSysVector & Mn, double FLiq, double T_)
  {
  double Cpl = 0.0;
  switch (sm_iCpMethod)
    {
    case CPM_Original:
      {//Original
      // Heat Capacity of Bayer Liquor Kcal/kg.degC
      // A function of the weight fraction of TNa and Al2O3
      // From published Russian data

      LiqConcs25.Converge(Mn);

      double TNaAsCO3    = LiqConcs25.LTotalSodium(Mn);
      double TNaAsCO3Use = Max(TNaAsCO3, FLiq*0.19); // Only Valid for  TNaAsCO3/FLiq > 0.19
      double TNa         = 100.0*TNaAsCO3*::OccSoda.MW/::SodiumCarbonate.MW/FLiq; //61.98=MW Na2O, 105.99=MW Na2CO3
      double TAl2O3      = 100.0*Mn[Alumina.LiqPhInx()]/FLiq;
      Cpl  = -0.020113606661083*TNa
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
    case CPM_AlunorteFeb2003:
      {//Alunorte Feb2003
      /*
      Double SysCADBayerCp*@("Cp(Ms)","kJ/kg.C")
      Double BAYER_cp*@("Cp(Ms)","kJ/kg.C"), TTS_gpl		
      Double Al2O3_gpl("Conc","g/L") , Caust_gpl("Conc","g/L") 
      Double Tot_Soda("Conc","g/L")  	

      SysCADBayerCp = GetTag("P_2.Qi.Cp@T (kJ/kg.C)")
      Al2O3_gpl = GetTag("P_2.Qi.A (g/L)")
      Caust_gpl = GetTag("P_2.Qi.C (g/L)")
      Tot_soda = GetTag("P_2.Qi.TotalNa (g/L)")  
      TTS_gpl = Tot_soda
      BAYER_cp = 3870.938 + 2.79145 * Temp_C - 0.00707695 * Temp_C * Temp_C 
                - 1.33106 * Al2O3_gpl + 0.000488062 * Al2O3_gpl * Al2O3_gpl 
                + 3.39998 * Caust_gpl + 0.000456852 * Caust_gpl * Caust_gpl 
                - 4.17734 * TTS_gpl - 0.00184963 * Temp_C * Caust_gpl		

      BAYER_cp = BAYER_cp / 1000
      */
      const double Tc = K_2_C(T_);
      //const double TTS = TotalSodiumConc();
      const double TTS = SodaConc(C_2_K(25.0));
      const double Al2O3 = AluminaConc(C_2_K(25.0));
      const double Caust = CausticConc(C_2_K(25.0));
      Cpl = 3870.938 + 2.79145 * Tc - 0.00707695 * Tc * Tc 
          - 1.33106 * Al2O3 + 0.000488062 * Al2O3 * Al2O3 
          + 3.39998 * Caust + 0.000456852 * Caust * Caust 
          - 4.17734 * TTS - 0.00184963 * Tc * Caust;
      Cpl /= 1000.0;
      break;
      }
    }
  return Cpl;
  }

//---------------------------------------------------------------------------

double AlunorteBayer::msCp(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
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

double AlunorteBayer::msHm(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
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
    // NB This will introduce a nonlinearity
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

double AlunorteBayer::SpecieConc(double T_, int iSpNo, PhMask PhaseM)
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

double AlunorteBayer::PhaseConc(double T_, PhMask Ph, PhMask PhaseM)
  {
  // To Be Improved
  return SpModelEx::PhaseConc(T_, Ph, PhaseM);
  }

//--------------------------------------------------------------------------

flag AlunorteBayer::CIStrng(int No, pchar & pS)
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

