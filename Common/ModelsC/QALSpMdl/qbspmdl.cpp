//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __QBSPMDL_CPP
#include "sp_db.h"
#include "qbspmdl.h"
//#include "optoff.h"

// ==========================================================================

#define dbgModels 01
#if dbgModels
#include "dbgmngr.h"
static CDbgMngr dbgSpecies          ("QALSpMdl", "Species");
//static CDbgMngr dbgGetSetComposition("QALSpMdl", "GetSetComposition");
#endif

// ==========================================================================
//
// QAL Bayer Specie Model.
//
// ==========================================================================

IMPLEMENT_SPMODEL(QSMBayer, "QBayer", "", TOC_ALL|TOC_GRP_ALUMINA|TOC_QAL, "QAL Bayer", "QAL Bayer Model");
IMPLEMENT_SPARES(QSMBayer, 100);

static MInitialiseTest InitTest(&QSMBayerClass);

//Specie Access 
SPECIEBLK_L(InitTest, Water,                 "H2O(l)",                                 false);
SPECIEBLK_L(InitTest, SodiumAluminate,       "NaAl[OH]4(l)",                           false);
SPECIEBLK_L(InitTest, CausticSoda,           "NaOH(l)",                                false);
SPECIEBLK_L(InitTest, SodiumCarbonate,       "Na2CO3(l)",                              false);
SPECIEBLK_L(InitTest, SodiumOxalate,         "Na2C2O4(l)",                             false);
SPECIEBLK_L(InitTest, SodiumChloride,        "NaCl(l)",                                false);
SPECIEBLK_L(InitTest, SodiumSulphate,        "Na2SO4(l)",                              false);
SPECIEBLK_L(InitTest, OrganicSoda,           "Na2C5.2O7.2(l)",                         false);
SPECIEBLK_L(InitTest, SodiumSilicate,        "Na2SiO3(l)",                             false);
SPECIEBLK_S(InitTest, DSP1,                  "Na2O.2SiO2.Al2O3.2H2O.1/3Na2CO3(s)",     false);
SPECIEBLK_S(InitTest, DSP2,                  "Na2O.2SiO2.Al2O3.2H2O.2/3NaCl(s)",       false);
SPECIEBLK_S(InitTest, DSP3,                  "Na2O.2SiO2.Al2O3.2H2O.2/3NaAl[OH]4(s)",  false);
SPECIEBLK_S(InitTest, InertAlumina,          "Al2O3(s)",                               false);
SPECIEBLK_V(InitTest, Steam,                 "H2O(g)",                                 false);

SPECIEBLK_S(InitTest, THA,                   "Al[OH]3(s)",                             false);

SPECIEBLK_S(InitTest, BoundSoda,               "NaOH*(s)",                             false); //required by PSD
SPECIEBLK_S(InitTest, BoundOrgSoda,            "Na2C5.2O7.2*(s)",                      false); //required by PSD

const double H2OTestFrac = 0.98;
const double MW_C        = 12.011;

// ==========================================================================
//
// A special class to assist with calculating iterative concentration and density
// calculations.
//
// ==========================================================================

flag     CQConcentrations::NaFactorOK=0;
SpVector CQConcentrations::NaFactor(0.0);

CQConcentrations::CQConcentrations(QSMBayer *pMdl) 
  { 
  pBayerMdl = pMdl;
  Liq.SetSize(SDB.Count());
  Zero(); 

  if (!NaFactorOK)
    {
    for (int sn=0; sn<SDB.Count(); sn++)
      //NaFactor[sn]=1.0; //For all the species without sodium ions
      NaFactor.VValue[sn]=0.0;
    NaFactor.VValue[::SodiumAluminate.LiqPhInx()] = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumAluminate.MW); // NaAl[OH]4    118.001
    NaFactor.VValue[::CausticSoda.LiqPhInx()]     = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::CausticSoda.MW);     // NaOH         39.9971
    NaFactor.VValue[::SodiumCarbonate.LiqPhInx()] = 1.0;                                                       // Na2CO3       105.989
    NaFactor.VValue[::SodiumOxalate.LiqPhInx()]   = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumOxalate.MW);   // Na2C2O4      133.999
    NaFactor.VValue[::SodiumChloride.LiqPhInx()]  = 1.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumChloride.MW);  // NaCl         58.4425
    NaFactor.VValue[::SodiumSulphate.LiqPhInx()]  = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumSulphate.MW);  // Na2SO4       142.043
    NaFactor.VValue[::OrganicSoda.LiqPhInx()]     = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::OrganicSoda.MW);     // Na2C5.2O7.2  223.632 
    NaFactor.VValue[::SodiumSilicate.LiqPhInx()]  = 2.0 * ::SodiumCarbonate.MW / (2.0 * ::SodiumSilicate.MW);  // Na2SiO3      122.063
    NaFactor.VValue[::DSP1.SolPhInx()]        = 1.33333 * ::SodiumCarbonate.MW / (2.0 * ::DSP1.MW);
    NaFactor.VValue[::DSP2.SolPhInx()]        = 1.33333 * ::SodiumCarbonate.MW / (2.0 * ::DSP2.MW);
    NaFactor.VValue[::DSP3.SolPhInx()]        = 1.33333 * ::SodiumCarbonate.MW / (2.0 * ::DSP3.MW);

    dbgpln("NaFactor %s = %8.6f", "SodiumAluminate", NaFactor[::SodiumAluminate.LiqPhInx()]);
    dbgpln("NaFactor %s = %8.6f", "CausticSoda    ", NaFactor[::CausticSoda.LiqPhInx()]    );
    dbgpln("NaFactor %s = %8.6f", "SodiumCarbonate", NaFactor[::SodiumCarbonate.LiqPhInx()]);
    dbgpln("NaFactor %s = %8.6f", "SodiumOxalate  ", NaFactor[::SodiumOxalate.LiqPhInx()]  );
    dbgpln("NaFactor %s = %8.6f", "SodiumChloride ", NaFactor[::SodiumChloride.LiqPhInx()] );
    dbgpln("NaFactor %s = %8.6f", "SodiumSulphate ", NaFactor[::SodiumSulphate.LiqPhInx()] );
    dbgpln("NaFactor %s = %8.6f", "OrganicSoda    ", NaFactor[::OrganicSoda.LiqPhInx()]    );
    dbgpln("NaFactor %s = %8.6f", "SodiumSilicate ", NaFactor[::SodiumSilicate.LiqPhInx()] );
    dbgpln("NaFactor %s = %8.6f", "DSP1           ", NaFactor[::DSP1.SolPhInx()]);
    dbgpln("NaFactor %s = %8.6f", "DSP2           ", NaFactor[::DSP2.SolPhInx()]);
    dbgpln("NaFactor %s = %8.6f", "DSP3           ", NaFactor[::DSP3.SolPhInx()]);
    
    NaFactorOK = True;
    }
  }

// --------------------------------------------------------------------------

void CQConcentrations::Zero()
  {
  Density = 1100.0;
  Alumina = 0.0;
  CausticSoda = 0.0;
  for (int sn=0; sn<SDB.Count(); sn++)
    Liq[sn] = 0.0;
  }

// --------------------------------------------------------------------------

inline double NaAluminate2Alumina() { return ::InertAlumina.MW/(2.0*::SodiumAluminate.MW); }; //=101.96/(2*118.001)=0.43204
inline double NaAluminate2Caustic() { return (2.0*::CausticSoda.MW)/(2.0*::SodiumAluminate.MW); } //=2.0*39.997/(2*118.001)=0.33896

// --------------------------------------------------------------------------

flag CQConcentrations::Converge(CSysVector & MA)
  {
  const double TLiq = MA.Sum(som_Liq);
  if (TLiq<1.0e-9)
    {
    Zero();
    return true;
    }

  const double T_ = C2K(25.0);
  #if dbgModels
  const double Tc = K2C(T_);
  #endif
  int MaxReached=0;
  int IterCount = 100;
  double OldDensity = Density*1.1;
  //while ( (fabs(OldDensity - Density) > 0.00005) && --IterCount>0)
  while ( (fabs(OldDensity - Density) > 0.000001) && --IterCount>0)
    {
    Density = Range(0.0001, Density, 10000.0);
    if (Density>3500.0)
      {
      Density = 1100.0; //new guess
      MaxReached++;
      }
    for (int sn=0; sn<SDB.Count(); sn++)
      {
      if (SDB[sn].IsLiq())
        {
        //#ifndef _RELEASE
        //double mm=MA[sn];
        //if (mm<0.0)
        //  {
        //  Strng ss = SDB[sn].SymOrTag();
        //  ASSERT_ALWAYS(FALSE, "QAL ConcConverge unexpected -ve !");
        //  }
        //#endif
        Liq[sn] = GTZ(MA[sn]) / TLiq * Density * NaFactor[sn];
        }
      }
    
    Alumina = ((MA[::SodiumAluminate.LiqPhInx()]) * NaAluminate2Alumina())/
               TLiq * Density * 1.0; // Factor Alumina == 1.0;

    CausticSoda = (MA[::CausticSoda.LiqPhInx()]+MA[::SodiumAluminate.LiqPhInx()]*NaAluminate2Caustic())/
                   TLiq * Density * NaFactor[::CausticSoda.LiqPhInx()];

    OldDensity = Density;
    if (MaxReached>1)
      break;
    Density = LiquorDensity(T_);
    } // end of while

  //pBayerMdl->SetCI((IterCount==0), 1, True, True);

  #if dbgModels
  if (dbgSpecies ())
    {
    dbgpln("Concs           Tc:%10.2f", Tc);
    dbgpln("           Density:%10.2f", Density);
    for (int sn=0; sn<SDB.Count(); sn++)
      if (SDB[sn].IsLiq())
        dbgpln("%18.18s:%12.6f  MW:%11.5f NaFactor:%9.5f", SDB[sn].SymOrTag(), Liq[sn], SDB[sn].MoleWt(), NaFactor[sn]);
    }
  #endif
  return (IterCount>=0);
  }

// --------------------------------------------------------------------------

double CQConcentrations::LiquorDensity(double T_)
  {
  double Tc=K_2_C(T_);
  double LCSodiumAluminate = Liq[::SodiumAluminate.LiqPhInx()];
  double LCSodiumCarbonate = Liq[::SodiumCarbonate.LiqPhInx()] ;
  double LCSodiumChloride  = Liq[::SodiumChloride.LiqPhInx()] ;
  double LCSodiumSulphate  = Liq[::SodiumSulphate.LiqPhInx()] ;
  double LCSodiumOxalate   = Liq[::SodiumOxalate.LiqPhInx()];
  double LCOrganicSoda     = Liq[::OrganicSoda.LiqPhInx()];

  double A, C, B, D, dNoLiqComp, a1, a2, b1, b2;
  a1 = 0.5 * (1 - 6.823882e-4 * (Tc-20) - 4.896378e-6 * Sqr(Tc-20));
  a2 = 0.5 * (1 - 9.52832e-4 * (Tc-20) - 2.64664e-6 * Sqr(Tc-20));
  b1 = Range(0.0, (CausticSoda - 95) /10, 1.0);
  b2 = Range(0.0, (Tc - 146) / 10, 1.0);
  b1 = Max(b1, b2);

  A = a1+6.0*b1*b1*(0.5-b1/3.0)*(a2-a1);

  // check if there are 6 or more liquor components
  dNoLiqComp = Alumina * LCSodiumCarbonate
               * LCSodiumChloride * LCSodiumSulphate
               * ( LCSodiumOxalate + LCOrganicSoda );

  if (CausticSoda>130.0 && dNoLiqComp>0.0 )
    B = 4e-5 * (CausticSoda - 130.0);
  else
    B = 0.0;

  C = 5e-4 * Sqrt( 1e6 + 4e3 * ( 0.8347 * CausticSoda
                                 + ( 0.74 + 4.2e-4 * CausticSoda) * Alumina
                                 + 1.048 * LCSodiumCarbonate
                                 + 0.8007 * LCSodiumChloride
                                 + 1.2517 * LCSodiumSulphate
                                 + 1.35 * LCSodiumOxalate
                                 + 1.35 * LCOrganicSoda
                               )
                     );

  if (LCSodiumCarbonate>=0.0)
    D = 1.0018 * Pow(10, Min(10.0, -3.36587 + 0.01136*LCSodiumCarbonate));
  else
    D = 0.0;

  #if dbgModels
  if (dbgSpecies ())
    dbgpln("Liq Dens Tc:%10.2f A:%12.6g B:%12.6g C:%12.6g D:%12.6g Rho:%10.2f",Tc,A,B,C,D,0.9982 * ( A + B + C + D ) * 1000.0);
  #endif
  return  0.9982 * ( A + B + C + D ) * 1000.0;
  }

// ==========================================================================
//
//                        QAL Bayer Liquor Specie Model
//
// ==========================================================================

const word xidQSlurryAluminaConc25 = 30305;
const word xidQSlurryCausticConc25 = 30306;
const word xidQSlurrySodaConc25    = 30307;
const word xidQSlurrySolidsConc25  = 30308;
const word xidQSlurryAtoC25        = 30309;
const word xidQSlurryCtoS25        = 30310;
const word xidQSlurryCltoC25       = 30311;
const word xidQSlurryAluminaConc   = 30312;
const word xidQSlurryCausticConc   = 30313;
const word xidQSlurrySodaConc      = 30314;
const word xidQSlurrySolidsConc    = 30315;
const word xidQSlurryVapPress      = 30316;
const word xidQLiquidVolume25      = 30317;
const word xidQSlurryVolume25      = 30318;
const word xidQSlurryRho25         = 30319;
const word xidQLiquidRho25         = 30320;
const word xidQTotalSodium         = 30321;
const word xidQIonicStrength       = 30322;
const word xidQASaturation         = 30323;
const word xidQBoundSodaSat        = 30324;
const word xidQTotalNaConc25       = 30325;
const word xidQOrganateConc25      = 30326;
const word xidQOxalateConc25       = 30327;
const word xidQNaClConc25          = 30328;
const word xidQNa2SO4Conc25        = 30329;
const word xidQSlurrySodiumCarbConc25 = 30330;
const word xidQTotalOrgConc25      = 30331;
const word xidQSlurryTOOC25        = 30332;
const word xidQSlurryTOC25         = 30333;
const word xidQSlurryTOC           = 30334;
const word xidQSlurryFreeCaustic25 = 30335;
const word xidQAtoCSaturation      = 30336;
const word xidQSSNRatio            = 30337;
const word xidQAluminaSSN          = 30338;

const word xidQDigAtoCEquil25      = 30352;
const word xidQSlurryNaOnCs        = 30354;

  
enum TSE_Methods { TSE_Original, TSE_Revised97 };
enum QBM_Methods { QBM_Original, QBM_Revised1 }; //QAL Bayer Model Method
enum QASat_Methods { QASat_Rosenberg, QASat_QAL99, QASat_Alpart02 };

byte QSMBayer::sm_iSatTEqn     = TSE_Original;
byte QSMBayer::sm_iQBMVer      = QBM_Revised1;
byte QSMBayer::sm_iASatMethod  = QASat_QAL99;
double QSMBayer::sm_dBPEFactor = 1.0;
double QSMBayer::sm_dRhoFactor = 1.0;
double QSMBayer::sm_dMinSolCp  = 1.046;
double QSMBayer::dK1_BoundSodaSat = 2.35;
double QSMBayer::dGrowth_k_TOC = 0.0023;
double QSMBayer::dGrowth_n_s   = -1.0;
double QSMBayer::dGrowth_n_fc  = -2.0;
double QSMBayer::dGrowth_n_eq  = 0.0;
double QSMBayer::dGrowth_n_    = 1.7;
double QSMBayer::dGrowth_n_sol = 1.0;
double QSMBayer::dGrowth_n_ssa = 0.6;

CArray <int, int> QSMBayer::LclParm;

QSMBayer::QSMBayer(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach):
  SMBayerBase(pClass_, Tag_, pAttach, eAttach),
  //LiqConcs(this),
  LiqConcs25(this)
  {
  fDoCalc  = false;
  dRqd_AtoC = 0.38;
  dRqd_C    = 750.0;
  dRqd_CtoS = 0.95;
  //dRqd_Sul  = 2.0;
  //dRqd_Sil  = 1.0;
  //dRqd_Org  = 20.0;
  //dRqd_Salt = 10.0;
  dRqd_Solids = 0.0;
  //dRqd_OrgRatio = 0.9;
  }

// --------------------------------------------------------------------------

QSMBayer::~QSMBayer()
  {
  }

// --------------------------------------------------------------------------

DDEF_Flags QSMBayer::FixSpEditFlags(DDEF_Flags Flags, int iSpid, flag AsParms)
  {
  if (fDoCalc && LclParm.GetSize()!=SDB.Count())
    {
    LclParm.SetSize(SDB.Count());
    for (int i=0; i<SDB.Count(); i++)
      LclParm[i]=true;

    LclParm[SodiumAluminate.LiqPhInx()] = false;
    LclParm[SodiumCarbonate.LiqPhInx()] = false;
    LclParm[CausticSoda.LiqPhInx()] = false;
    //LclParm[Water.LiqPhInx()] = false;
    }

  return (Flags&~(isParm|isResult)) | ((AsParms && (!fDoCalc || LclParm[iSpid])) ? DDEF_PARAM: DDEF_RESULT); 
  }

// --------------------------------------------------------------------------

void QSMBayer::BuildDataDefn_Vars(DataDefnBlk & DDB)
  {
  if (UseAsMass() || UseAsImage())
    {
    DDB.Text("Bayer");
    DDBValueLst DDBQBMVer[]={
      {QBM_Original, "Original"},
      {QBM_Revised1, "Revised1"},
      {0}};
    DDBValueLst DDBTSetEqn[]={
      {TSE_Original,  "Original"},
      {TSE_Revised97, "Revised97"},
      {0}};
    DDBValueLst DDBASatEqn[]={
      {QASat_Rosenberg,  "Rosenberg"},
      {QASat_QAL99, "QAL99"},
      {QASat_Alpart02, "Alpart02"},
      {0}};
    DDB.Double  ("BPE_Factor",      "",  DC_Frac,   "%",      &sm_dBPEFactor,  this,  isParm|InitHidden); //global constant
    DDB.Double  ("Rho_Factor",      "",  DC_Frac,   "%",      &sm_dRhoFactor,  this,  isParm|InitHidden); //global constant
    DDB.Double  ("MinSolidsCp",     "",  DC_CpMs,   "kJ/kg.C",&sm_dMinSolCp,   this,  isParm|InitHidden); //global constant
    DDB.Double  ("K1_BoundSodaCalc","",  DC_,       "",       &dK1_BoundSodaSat,this, isParm|InitHidden); //global constant
    DDB.Double  ("Growth_TOC",      "",  DC_,       "",       &dGrowth_k_TOC,  this,  isParm|InitHidden); //global constant
    DDB.Double  ("Growth_s",        "",  DC_,       "",       &dGrowth_n_s,    this,  isParm|InitHidden); //global constant
    DDB.Double  ("Growth_fc",       "",  DC_,       "",       &dGrowth_n_fc,   this,  isParm|InitHidden); //global constant
    DDB.Double  ("Growth_eq",       "",  DC_,       "",       &dGrowth_n_eq,   this,  isParm|InitHidden); //global constant
    DDB.Double  ("Growth_n",        "",  DC_,       "",       &dGrowth_n_,     this,  isParm|InitHidden); //global constant
    DDB.Double  ("Growth_sol",      "",  DC_,       "",       &dGrowth_n_sol,  this,  isParm|InitHidden); //global constant
    DDB.Double  ("Growth_ssa",      "",  DC_,       "",       &dGrowth_n_ssa,  this,  isParm|InitHidden); //global constant
    DDB.Byte  ("Version",           "",  DC_,       "",       &sm_iQBMVer,     this,  isParm, DDBQBMVer);
    DDB.Byte  ("TSatEqn",           "",  DC_,       "",       &sm_iSatTEqn,    this,  isParm, DDBTSetEqn);
    DDB.Byte  ("ASatMethod",        "",  DC_,       "",       &sm_iASatMethod, this,  isParm, DDBASatEqn);

    DDB.CheckBoxBtn("DefineLiquor", "",  DC_,       "",       &fDoCalc,        this,  isParm);
    DDB.Visibility(SHM_All, fDoCalc);                                                         
    DDB.Double  ("Rqd_A/C",         "",  DC_,       "",       &dRqd_AtoC,      this,  isParm);
    DDB.Double  ("Rqd_C/S",         "",  DC_,       "",       &dRqd_CtoS,      this,  isParm);
    DDB.Double  ("Rqd_C",           "",  DC_Conc,   "g/L",    &dRqd_C,         this,  isParm);
    DDB.Double  ("Rqd_Solids",      "",  DC_Conc,   "g/L",    &dRqd_Solids,    this,  isParm);
    DDB.Visibility();                                                                               
    DDB.Text("");
    }

  SpModelEx::BuildDataDefn_Vars(DDB);

  Strng Tg(DDB.BlockTag());
  Tg+="..";
  DDB.Page(Tg(), DDB_OptPage);
  DDB.Text("Bayer Liquor Values @ 25");
  DDB.Double("AluminaConc",   "A",  DC_Conc,  "g/L",     xidQSlurryAluminaConc25, this,  isResult|noFile|noSnap);
  DDB.Double("CausticConc",   "C",  DC_Conc,  "g/L",     xidQSlurryCausticConc25, this,  isResult|noFile|noSnap);
  DDB.Double("SodaConc",      "S",  DC_Conc,  "g/L",     xidQSlurrySodaConc25,    this,  isResult|noFile|noSnap);
  DDB.Double("A/C",           "",   DC_,      "",        xidQSlurryAtoC25,        this,  isResult|noFile|noSnap);
  DDB.Double("C/S",           "",   DC_,      "",        xidQSlurryCtoS25,        this,  isResult|noFile|noSnap);
  DDB.Double("Cl/C",          "",   DC_,      "",        xidQSlurryCltoC25,       this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("TOC",           "",   DC_Conc,  "g/L",     xidQSlurryTOC25,         this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("SodiumCarbConc","",   DC_Conc,  "g/L",     xidQSlurrySodiumCarbConc25,this,isResult|noFile|noSnap|InitHidden);
  DDB.Double("SolidsConc",    "",   DC_Conc,  "g/L",     xidQSlurrySolidsConc25,  this,  isResult|noFile|noSnap);
  DDB.Double("FreeCaustic",   "FC", DC_Conc,  "g/L",     xidQSlurryFreeCaustic25, this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("DigA/CEquil",   "",   DC_,      "",        xidQDigAtoCEquil25,      this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("NaOnCS",        "",   DC_,      "",        xidQSlurryNaOnCs,        this,  isResult|noFile|noSnap);
  
  DDB.Text("");
  DDB.Text("Concentrations @ 25, as Na2CO3 equivalent");
  //DDB.Double("Organates",     "",   DC_Conc,  "g/L",     xidQOrganateConc25,      this,  isResult|noFile|noSnap);
  DDB.Double("Oxalate",       "",   DC_Conc,  "g/L",     xidQOxalateConc25,       this,  isResult|noFile|noSnap|InitHidden);
  //DDB.Double("TotalOrg",      "",   DC_Conc,  "g/L",     xidQTotalOrgConc25,      this,  isResult|noFile|noSnap);
  DDB.Double("TOOC",          "",   DC_Conc,  "g/L",     xidQSlurryTOOC25,        this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("NaCl",          "",   DC_Conc,  "g/L",     xidQNaClConc25,          this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("Na2SO4",        "",   DC_Conc,  "g/L",     xidQNa2SO4Conc25,        this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("TotalNa",       "",   DC_Conc,  "g/L",     xidQTotalNaConc25,       this,  isResult|noFile|noSnap|InitHidden);

  DDB.Text("");
  DDB.Text("Bayer Liquor Values @ 25");
  DDB.Double("LVolFlow25",    "",   DC_Qv,    "L/s",     xidQLiquidVolume25,      this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("SLVolFlow25",   "",   DC_Qv,    "L/s",     xidQSlurryVolume25,      this,  isResult|noFile|noSnap);
  DDB.Double("LRho25",        "",   DC_Rho,   "kg/m^3",  xidQLiquidRho25,         this,  isResult|noFile|noSnap);
  DDB.Double("SLRho25",       "",   DC_Rho,   "kg/m^3",  xidQSlurryRho25,         this,  isResult|noFile|noSnap);
  //DDB.Double("VapPress",     "Vp",  DC_P,     "kPa",     xidQSlurryVapPress,      this,  isResult|noFile|noSnap);
  
  DDB.Text("");
  DDB.Text("Bayer Liquor Values @ Temp");
  DDB.Double("AluminaConcT", "A@T", DC_Conc,  "g/L",     xidQSlurryAluminaConc,   this,  isResult|noFile);
  DDB.Double("CausticConcT", "C@T", DC_Conc,  "g/L",     xidQSlurryCausticConc,   this,  isResult|noFile);
  DDB.Double("SodaConcT",    "S@T", DC_Conc,  "g/L",     xidQSlurrySodaConc,      this,  isResult|noFile);
  DDB.Double("TOC@T",         "",   DC_Conc,  "g/L",     xidQSlurryTOC,           this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("SolidsConcT",   "",   DC_Conc,  "g/L",     xidQSlurrySolidsConc,    this,  isResult|noFile);
  DDB.Text("Bayer Liquor Precipitation Values @ Temp");
  DDB.Double("A_Saturation", "ASat",DC_Conc,  "g/L",     xidQASaturation,         this,  isResult|noFile|noSnap);
  DDB.Double("BoundSodaSat",  "",   DC_Frac,  "%",       xidQBoundSodaSat,        this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("A/C_Saturation", "A/CSat", DC_, "",        xidQAtoCSaturation,      this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("SSN_Ratio",     "",   DC_,      "",        xidQSSNRatio,            this,  isResult|noFile|noSnap);
  DDB.Double("Alumina_SSN",   "",   DC_,      "",        xidQAluminaSSN,          this,  isResult|noFile|noSnap|InitHidden);
  DDB.Double("IonicStrength", "I",  DC_,      "",        xidQIonicStrength,       this,  isResult|noFile|noSnap|InitHidden);

  }

// --------------------------------------------------------------------------

flag QSMBayer::DataXchg(DataChangeBlk & DCB)
  {
  if (SMBayerBase::DataXchg(DCB))
    return true;

  switch (DCB.lHandle)
    {
    case xidQSlurryCausticConc:    DCB.D=CausticConc(Temp()); return 1;
    case xidQSlurryAluminaConc:    DCB.D=AluminaConc(Temp()); return 1;
    case xidQSlurrySodaConc:       DCB.D=SodaConc(Temp()); return 1;
    case xidQSlurryCausticConc25:  DCB.D=CausticConc(C_2_K(25.0)); return 1;
    case xidQSlurryAluminaConc25:  DCB.D=AluminaConc(C_2_K(25.0)); return 1;
    case xidQSlurrySodaConc25:     DCB.D=SodaConc(C_2_K(25.0)); return 1;
    case xidQSlurryAtoC25:         DCB.D=AtoC(); return 1;
    case xidQSlurryCtoS25:         DCB.D=CtoS(); return 1;
    case xidQTotalNaConc25:        DCB.D=LTotalSodiumConc(); return 1;
    case xidQIonicStrength:        DCB.D=IonicStrength(); return 1;
    case xidQASaturation:          DCB.D=AluminaConcSat(Temp()); return 1;
    case xidQBoundSodaSat:         DCB.D=BoundSodaSat(Temp()); return 1;
    case xidQAtoCSaturation:       DCB.D=AtoCSaturation(Temp()); return 1;
    case xidQSSNRatio:             DCB.D=SSNRatio(Temp()); return 1;
    case xidQAluminaSSN:           DCB.D=AluminaSSN(Temp()); return 1;
    case xidQSlurrySolidsConc:     DCB.D=SolidsConc(Temp()); return 1;
    case xidQSlurrySolidsConc25:   DCB.D=SolidsConc(C_2_K(25.0)); return 1;
    case xidQSlurryFreeCaustic25:  DCB.D=FreeCaustic(C_2_K(25.0)); return 1;
    
    case xidQDigAtoCEquil25:       DCB.D=DigAtoCEquil(); return 1;
    case xidQSlurryCltoC25:        DCB.D=CltoC(); return 1;
    case xidQSlurryNaOnCs:         DCB.D=TotalNaOnCS(); return 1;
    case xidQSlurryTOOC25:         DCB.D=TOOC(C_2_K(25.0)); return 1;
    case xidQSlurryTOC25:          DCB.D=TOC(C_2_K(25.0)); return 1;
    case xidQSlurryTOC:            DCB.D=TOC(Temp()); return 1;
    //case xidQSlurryViscosity:      DCB.D=DynamicViscosity(); return 1;
    //case xidQSlurryVapPress:       DCB.D=SaturationP(Temp()); return 1;
    case xidQSlurrySodiumCarbConc25:DCB.D=SodiumCarbonateConc(C_2_K(25.0)); return 1;

    //case xidQOrganateConc25:       DCB.D=OrganateConc(); return 1;
    //case xidQTotalOrgConc25:       DCB.D=TotalOrganics(); return 1;
    case xidQOxalateConc25:        DCB.D=SodiumOxalateConc(C_2_K(25.0)); return 1;
    case xidQNaClConc25:           DCB.D=ChlorineConc(); return 1;
    case xidQNa2SO4Conc25:         DCB.D=SulphateConc(); return 1;

    //case xidQSlurryVapPress:       DCB.D=SaturationP(Temp()); return 1;
    case xidQLiquidRho25:          DCB.D=Rho(som_Liq, C_2_K(25.0), Press()); return 1;
    case xidQSlurryRho25:          DCB.D=Rho(som_SL,  C_2_K(25.0), Press()); return 1;
    case xidQLiquidVolume25:       DCB.D=LVolume25(); return 1;
    case xidQSlurryVolume25:       DCB.D=SLVolume25(); return 1;
    }

  return 0;
  }

//---------------------------------------------------------------------------

flag QSMBayer::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=SpModelEx::ValidateData(VDB);
  //if (dGrowth_n_eq==0)
  //  dGrowth_n_eq=1.7;
  
  if (fDoCalc)
    {    
    dRqd_AtoC   = ValidateRange(VDB, "dRqd_AtoC", 0.2, dRqd_AtoC,        0.9);
    dRqd_C      = ValidateRange(VDB, "dRqd_C", 0.0, dRqd_C,           1500.0);
    dRqd_CtoS   = ValidateRange(VDB, "dRqd_CtoS", 0.6, dRqd_CtoS,        1.0);
    dRqd_Solids = ValidateRange(VDB, "dRqd_Solids", 0.0, dRqd_Solids, 1500.0);
    //dRqd_Sul  = ValidateRange(VDB, "dRqd_Sul", 0.0, dRqd_Sul,     500.0);
    //dRqd_Sil  = ValidateRange(VDB, "dRqd_Sil", 0.0, dRqd_Sil,     500.0);
    //dRqd_Salt = ValidateRange(VDB, "dRqd_Salt", 0.0, dRqd_Salt,    500.0);
    //dRqd_Org  = ValidateRange(VDB, "dRqd_Org", 0.0, dRqd_Org,     500.0);
    //dRqd_OrgRatio = ValidateRange(VDB, "dRqd_OrgRatio", 0.0, dRqd_OrgRatio, 1.0);

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

#define dbgCalc 0

void QSMBayer::DoInputCalcs()
  {
  switch (GetView())
    {
    case SVV_AsRawFrac:
    case SVV_AsRawMass:
    case SVV_AsRawMassFlow:
      fDoCalc=false;
      return;
    }

  const int  MaxOIters=100;
  const int  MaxIIters=20;
  const double RelTol=1.0e-10;

  int TolIsTight=false;
  double RTol=1.0e-2;
  double ATol=1.0e-8;

  double Err, MeasAtoC, MeasCtoS, MeasCaustic, MeasSolids, RqdMass;
  double PrevAtoC, PrevCtoS, PrevCaustic, PrevSolids;
  PrevAtoC=0.0;
  PrevCtoS=0.0;
  PrevCaustic=0.0;
  PrevSolids=0.0;

  double TotalMass=Mass();
  int iInner0=-1, iInner1, iInner2, iInner3;
//  int jInner0=-2, jInner1, jInner2, jInner3;

  double CnvDamp=0.5;
  #if dbgCalc
  dbgpln("------------");
  #endif
  for (int iOuter=0; iOuter<MaxOIters; iOuter++)
    {
    MeasAtoC=AtoC();
    double LclRqd_AtoC=CnvDamp*MeasAtoC + (1.0-CnvDamp)*dRqd_AtoC;
    MeasCtoS=CtoS();
    double LclRqd_CtoS=CnvDamp*MeasCtoS + (1.0-CnvDamp)*dRqd_CtoS;
    MeasCaustic=CausticConc(C_2_K(25.0));
    double LclRqd_Caustic=CnvDamp*MeasCaustic + (1.0-CnvDamp)*dRqd_C;

    PrevAtoC    = MeasAtoC;
    PrevCtoS    = MeasCtoS;
    PrevCaustic = MeasCaustic;

    for (iInner0=0; iInner0<MaxIIters; iInner0++)
      {
      MeasAtoC=AtoC();
      #if dbgCalc
      dbgp("AtoC %7.3f %7.3f ", LclRqd_AtoC, MeasAtoC);
      #endif
      if (ConvergedVV(LclRqd_AtoC, MeasAtoC, ATol, RTol, Err))
        {
        #if dbgCalc
        dbgpln("");
        #endif
        break;
        }
      if (m_M[SodiumAluminate.LiqPhInx()]<1.0e-6 && LclRqd_AtoC>1.0e-6)
        RqdMass=1.0e-6;
      else
        RqdMass=m_M[SodiumAluminate.LiqPhInx()]*Range(0.01, LclRqd_AtoC/ GTZ(MeasAtoC), 100.0);
      #if dbgCalc
      dbgpln(" %12.6f  %12.6f", RqdMass, m_M[SodiumAluminate.LiqPhInx()]);
      #endif

      SetVMass(SodiumAluminate.LiqPhInx(), *this, RqdMass);
      }

    // NB A Inverse Relationship
    for (iInner1=0; iInner1<MaxIIters; iInner1++)
      {
      MeasCtoS=CtoS();
      #if dbgCalc
      dbgp("CtoS %7.3f %7.3f ", LclRqd_CtoS, MeasCtoS);
      #endif
      if (ConvergedVV(LclRqd_CtoS, MeasCtoS, ATol, RTol, Err))
        {
        #if dbgCalc
        dbgpln("");
        #endif
        break;
        }
      if (m_M[SodiumCarbonate.LiqPhInx()]<1.0e-6 && LclRqd_CtoS<0.9999999)
        RqdMass=1.0e-6;
      else
        {
        double CausticConc=LiqConcs25.CausticSoda;
        double SodiumConc=LiqConcs25.Liq[SodiumCarbonate.LiqPhInx()];
        double RqdSodiumConc=CausticConc*(1.0/LclRqd_CtoS-1.0);

        RqdMass=m_M[SodiumCarbonate.LiqPhInx()]*Range(0.01, RqdSodiumConc/ GTZ(SodiumConc), 100.0);
        }
      #if dbgCalc
      dbgpln(" %12.6f  %12.6f", RqdMass, M[SodiumCarbonate.LiqPhInx()]);
      #endif
      SetVMass(SodiumCarbonate.LiqPhInx(), *this, RqdMass);
      }

    for (iInner2=0; iInner2<MaxIIters; iInner2++)
      {
      MeasCaustic=CausticConc(C_2_K(25.0));
      #if dbgCalc
      dbgp("Caus %7.3f %7.3f ", LclRqd_Caustic, MeasCaustic);
      #endif
      if (ConvergedVV(LclRqd_Caustic, MeasCaustic, ATol, RTol, Err))
        {
        #if dbgCalc
        dbgpln("");
        #endif
        break;
        }
      if (m_M[CausticSoda.LiqPhInx()]<1.0e-6 && LclRqd_Caustic>1.0e-6)
        RqdMass=1.0e-6;
      else
        RqdMass=m_M[CausticSoda.LiqPhInx()]*Range(0.01, LclRqd_Caustic/ GTZ(MeasCaustic), 100.0);
      #if dbgCalc
      dbgpln(" %12.6f  %12.6f", RqdMass, m_M[CausticSoda.LiqPhInx()]);
      #endif
      SetVMass(CausticSoda.LiqPhInx(), *this, RqdMass);
      }

    for (iInner3=0; iInner3<MaxIIters; iInner3++)
      {
      MeasSolids=SolidsConc(C_2_K(25.0));
      if (ConvergedVV(dRqd_Solids, MeasSolids, ATol, RTol, Err))
        break;
      ScaleMass(som_Sol, Range(0.01, dRqd_Solids/ GTZ(MeasSolids), 100.0));
      }

    ScaleMass(som_ALL, TotalMass/GTZ(Mass()));

    if (TolIsTight)
      {
      if (iInner0==0 && iInner1==0 && iInner2==0 && iInner3==0)
        break;
      }
    else
      {
      TolIsTight=(RTol<=RelTol);
      RTol=Max(RelTol, RTol*0.4);
      }
    }

  #if dbgCalc
  dbgpln("----- %i -------", iOuter);
  #endif
  SetCI(2, iInner0>0 || iInner1>0 || iInner2>0 || iInner3>0);
  }

//---------------------------------------------------------------------------

double QSMBayer::THAMassFlow()
  {
  //todo : check!
  //ASSERT_ALWAYS(0, "Not implemented");
  //return 0.0;
  return m_M[::THA.SolPhInx()];
  }

//---------------------------------------------------------------------------

double QSMBayer::THADens(double T_)
  {
  //todo : check!
  //ASSERT_ALWAYS(0, "Not implemented");
  //return 0.0;
  return SDB[::THA.SolPhInx()].Density(Fidelity(), T_, Std_P, &m_Ovr, NULL);
  }

//---------------------------------------------------------------------------

double QSMBayer::CausticConc(double T_)
  {
  //NOTE: CausticConc (C) includes CausticSoda (NaOH) AND CausticSoda found in SodiumAluminate (NaAl[OH]4)
  LiqConcs25.Converge(MArray());
  double DRatio   = LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
  //double liq25CS  = LiqConcs25.Liq[::CausticSoda.LiqPhInx()];
  double liq25CS  = LiqConcs25.CausticSoda;

  return Max(0.0, liq25CS * DRatio);
  }

//---------------------------------------------------------------------------

double QSMBayer::AluminaConc(double T_)
  {
  //NOTE: AluminaConc (A) includes Alumina found in SodiumAluminate (NaAl[OH]4)
  LiqConcs25.Converge(MArray());
  double DRatio=LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
  return Max(0.0, LiqConcs25.Alumina*DRatio);
  //return Max(0.0, LiqConcs25.Liq[::Alumina.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double QSMBayer::SodaConc(double T_)
  {
  //NOTE: SodaConc (S) includes CausticSoda (NaOH) AND CausticSoda found in SodiumAluminate (NaAl[OH]4) AND SodiumCarbonate (Na2CO3)
  LiqConcs25.Converge(MArray());
  double DRatio=LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
  return Max(0.0, (LiqConcs25.CausticSoda + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()])*DRatio);
  }

//---------------------------------------------------------------------------

double QSMBayer::SodiumCarbonateConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio=LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
  return Max(0.0, LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double QSMBayer::SodiumSulphateConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio=LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
  return Max(0.0, LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double QSMBayer::SodiumOxalateConc(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio=LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
  return Max(0.0, LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]*DRatio);
  }

//---------------------------------------------------------------------------

double QSMBayer::OX(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio=LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
  return Max(0.0, LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()]*DRatio)*(2*MW_C)/::SodiumOxalate.MW; //MW_Carbon=12.011
  };
double QSMBayer::OC(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio=LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
  return Max(0.0, LiqConcs25.Liq[::OrganicSoda.LiqPhInx()]*DRatio)*(5.2*MW_C)/::OrganicSoda.MW;
  };
void QSMBayer::DumpState()
  {
  CSysVector &M=MArray();

//#if dbgModels 
//  double mt0=M.Sum();
//#endif


  double &WaterM                    = *M.VPtr[Water.LiqPhInx()];
  double &SodiumAluminateM          = *M.VPtr[SodiumAluminate.LiqPhInx()];
  double &CausticSodaM              = *M.VPtr[CausticSoda.LiqPhInx()];
  double &SodiumCarbonateM          = *M.VPtr[SodiumCarbonate.LiqPhInx()];
  double &SodiumOxalateM            = *M.VPtr[SodiumOxalate.LiqPhInx()];
  double &SodiumChlorideM           = *M.VPtr[SodiumChloride.LiqPhInx()];
  double &SodiumSulphateM           = *M.VPtr[SodiumSulphate.LiqPhInx()];
  double &OrganicSodaM              = *M.VPtr[OrganicSoda.LiqPhInx()];
  //double &SodiumSilicateM         = *M.VPtr[SodiumSilicate.LiqPhInx()];
  //KGAFIX - double &DSP1M          = *M.VPtr[DSP1.SolPhInx()];
  //KGAFIX - double &DSP2M          = *M.VPtr[DSP2.SolPhInx()];
  //KGAFIX - double &DSP3M          = *M.VPtr[DSP3.SolPhInx()];
  //KGAFIX - double &HeamatiteM     = *M.VPtr[Heamatite.LiqPhInx()];
  //KGAFIX - double &InertAluminaM  = *M.VPtr[InertAlumina.SolPhInx()];
  //KGAFIX - double &QuartzM        = *M.VPtr[Quartz.SolPhInx()];
  double &THAM                      = *M.VPtr[THA.SolPhInx()];

  // Get Current of species if interrest
  double TotMass=0;

  TotMass+=WaterM;
  TotMass+=SodiumAluminateM;
  TotMass+=CausticSodaM;
  TotMass+=SodiumCarbonateM;
  TotMass+=SodiumOxalateM;
  TotMass+=SodiumChlorideM;
  TotMass+=SodiumSulphateM;
  TotMass+=OrganicSodaM;
  //TotMass+=SodiumSilicateM;
  //KGAFIX - TotMass+=DSP1M;
  //KGAFIX - TotMass+=DSP2M;
  //KGAFIX - TotMass+=DSP3M;
  //KGAFIX - TotMass+=HeamatiteM;
  //KGAFIX - TotMass+=InertAluminaM;
  //KGAFIX - TotMass+=QuartzM;
  TotMass+=THAM;


  dbgpln("      %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s",
          "TotMass", "WaterM", "SodiumAluminateM", "CausticSodaM", "SodiumCarbonateM", "SodiumSulphateM", 
          "SodiumChlorideM", "SodiumOxalateM", "OrganicSodaM", "THAM", "OX", "OC");
  dbgpln(" Mass %12.2f %12.2f %12.2f %12.2f %12.2f %12.2f %12.2f %12.2f %12.2f %12.2f %12.2f %12.2f",
          TotMass, WaterM, SodiumAluminateM, CausticSodaM, SodiumCarbonateM, SodiumSulphateM, 
          SodiumChlorideM, SodiumOxalateM, OrganicSodaM, THAM,
          SodiumOxalateM*(2*12)/SodiumOxalate.MW, OrganicSodaM*100*(5*12)/OrganicSoda.MW);
  dbgpln(" Frac %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%%",
          TotMass/GTZ(TotMass)*100, WaterM/GTZ(TotMass)*100, SodiumAluminateM/GTZ(TotMass)*100,
          CausticSodaM/GTZ(TotMass)*100, SodiumCarbonateM/GTZ(TotMass)*100, SodiumSulphateM/GTZ(TotMass)*100, 
          SodiumChlorideM/GTZ(TotMass)*100, SodiumOxalateM/GTZ(TotMass)*100, OrganicSodaM/GTZ(TotMass)*100, 
          THAM/GTZ(TotMass)*100, 
          SodiumOxalateM/GTZ(TotMass)*100*(2*12)/SodiumOxalate.MW, OrganicSodaM/GTZ(TotMass)*100*(5*12)/OrganicSoda.MW);

  };

//---------------------------------------------------------------------------

//double QSMBayer::SodiumFlourideConc(double T_)
//  {
//  return 0;
//  // KGAFIX
//  //LiqConcs25.Converge(MArray());
//  //double DRatio=LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
//  //return Max(0.0, LiqConcs25.Liq[SodiumFlouride.LiqPhInx()]*DRatio);
//  }
//
//double QSMBayer::QuartzConc(double T_)
//  {
//  return 0;
//  // KGAFIX
//  //LiqConcs25.Converge(MArray());
//  //double DRatio=LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
//  //return Max(0.0, LiqConcs25.Liq[Quartz.LiqPhInx()]*DRatio);
//  }
//double QSMBayer::HeamatiteConc(double T_)
//  {
//  return 0;
//  // KGAFIX
//  //LiqConcs25.Converge(MArray());
//  //double DRatio=LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
//  //return Max(0.0, LiqConcs25.Liq[Heamatite.LiqPhInx()]*DRatio);
//  }

double QSMBayer::BoundSodaFrac()
  {
  CSysVector &M=MArray();
  return (M[BoundSoda.SolPhInx()]+M[BoundOrgSoda.SolPhInx()])/GTZ(M.Sum(som_Sol));
  }

double QSMBayer::BoundSodaOrgFrac()
  {
  CSysVector &M=MArray();
  return (M[BoundOrgSoda.SolPhInx()])/GTZ(M[BoundSoda.SolPhInx()]+M[BoundOrgSoda.SolPhInx()]);
  }

//---------------------------------------------------------------------------

double QSMBayer::SolidsConc(double T_)
  {
  return Mass(som_Sol)/GTZ(Mass(som_SL))*Rho(som_SL, T_, Press(), pMArray());
  }

//---------------------------------------------------------------------------

double QSMBayer::LVolume25()
  {
  return (Mass(som_Liq) / Rho(som_Liq, C_2_K(25.0), Press()));
  }

//---------------------------------------------------------------------------

double QSMBayer::SLVolume25()
  {
  return (Mass(som_SL) / Rho(som_SL, C_2_K(25.0), Press()));
  }

//---------------------------------------------------------------------------

//double QSMBayer::TotalSodium25()
//  {
//  LiqConcs25.Converge(MArray());
//  double Sodium = LiqConcs25.Liq[::CausticSoda.LiqPhInx()] + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]
//                  + LiqConcs25.Liq[SodiumOxalate.LiqPhInx()] + LiqConcs25.Liq[Organics.LiqPhInx()]
//                  + LiqConcs25.Liq[SodiumChloride.LiqPhInx()] + LiqConcs25.Liq[SodiumSulphate.LiqPhInx()];
//  return Sodium;
//  }

//---------------------------------------------------------------------------

//double QSMBayer::OrganateConc()
//  {
//  LiqConcs25.Converge(MArray());
//  double Organate = LiqConcs25.Liq[::Organics.LiqPhInx()];
//  return Organate;
//  }

//---------------------------------------------------------------------------

double QSMBayer::ChlorineConc()
  {
  LiqConcs25.Converge(MArray());
  double Chlorine = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()];
  return Chlorine;
  }

//---------------------------------------------------------------------------

double QSMBayer::SulphateConc()
  {
  LiqConcs25.Converge(MArray());
  double Sulphate = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()];
  return Sulphate;
  }

//---------------------------------------------------------------------------

//double QSMBayer::TotalOrganics()
//  {
//  LiqConcs25.Converge(MArray());
//  double Organics = LiqConcs25.Liq[::Organics.LiqPhInx()] + LiqConcs25.Liq[::SodiumOxalate.LiqPhInx()];
//  return Organics;
//  }


//---------------------------------------------------------------------------

double QSMBayer::IonicStrength()
  {
  const double MW_Na2CO3   = ::SodiumCarbonate.MW;  //105.989..
  const double MW_NaCl     = ::SodiumChloride.MW;   //58.4425..
  const double MW_Na2SO4   = ::SodiumSulphate.MW;   //142.043..
  double I;
  switch (sm_iASatMethod)
    {
    case QASat_Rosenberg:
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
    case QASat_QAL99:
      {
      /* QAL Saturated Alumina concentration: Original from precip model in code QprecipD.cpp.
      Based on 1996 Rosenberg S.P, Healy S.J 'A Thermodynamic Model for Gibbsite Solubility in Bayer Liquors'.*/

      // Ionic Strength:
      // Know the following:  
      //   Cl2C = (Cl * 105.99 / (58.44 * 2)) / C         ; converts from g/L NaCl to g/L Na2CO3          [a]
      //   C2S = C/(C+CO3)  => CO3 = C*(1-C2S)/C2S  => CO3_2C = C*(1-C2S)/C2S/C  => CO3_2C = (1-C2S)/C2S  [b]
      //	 SO42C = (SO4 * 105.99 / 142.04) / C            ; converts from g/L NaCl to g/L Na2CO3          [c]
      //   TOOC = TOC*106/12  => TOC = TOOC/106*12        ; converts from g/L C to g/L Na2CO3	            [d]
      //   TOOC2C = (TOOC/C)															;                                               [e]
    
      //original form of equation for Aluminate Ionic Strength:
      //   I = 0.01887*C + k1*Cl/58.44 + k2*CO3/105.99 + k3*SO4/142.04 + k4*0.01887*TOC 
      //now, reformulate equation for Aluminate Ionic Strength:
      //=> I = C     * (0.01887     + k1*(Cl/58.44)/C     + k2*(CO3/106/C)     + k3*(SO4/142.04/C)     + k4*0.01887*(TOC/C))		         ;multiply  by C/C
      //=> I = C/106 * (0.01887*106 + k1*(Cl*106/58.44)/C + k2*(CO3*106/106/C) + k3*(SO4*106/142.04/C) + k4*0.01887*(TOC*106/C))         ;multiply  by 106/106
      //=> I = C/106 * (0.01887*106 + k1*(Cl2C*2)         + k2*(CO3/C)         + k3*(SO4*106/142.04/C) + k4*0.01887*(TOC*106/C))			   ;using [a]
      //=> I = C/106 * (0.01887*106 + k1*(Cl2C*2)         + k2*(1-C2S)/C2S     + k3*(SO4*106/142.04/C) + k4*0.01887*(TOC*106/C))			   ;using [b]
      //=> I = C/106 * (0.01887*106 + k1*(Cl2C*2)         + k2*(1-C2S)/C2S     + k3*SO42C              + k4*0.01887*(TOC*106/C))			   ;using [c]
      //=> I = C/106 * (0.01887*106 + k1*(Cl2C*2)         + k2*(1-C2S)/C2S     + k3*SO42C              + k4*0.01887*(TOOC/106*12*106/C)) ;using [d]
      //=> I = C/106 * (0.01887*106 + k1*(Cl2C*2)         + k2*(1-C2S)/C2S     + k3*SO42C              + k4*0.01887*(TOOC2C*12)			     ;using [e]

      const double k1 = 1.5532;//0.9346;
      const double k2 = 1.3437;//2.0526;
      const double k3 = 2.2571;//2.1714;
      const double k4 = 1.1997;//1.6734;
      LiqConcs25.Converge(MArray());
      const double Cl2C    = CltoC();
      const double C2S     = CtoS();
      //const double C_atT   = CausticConc(T_);
      //const double SO4_atT = SodiumSulphateConc(T_);
      //const double SO4_2C  = SO4_atT/GTZ(C_atT);
      const double SO4_2C  = SodiumSulphateConc(C2K(25.0))/GTZ(CausticConc(C2K(25.0)));
      const double TOOC2C  = TOOCtoC();
      const double C       = LiqConcs25.CausticSoda;
      I   = C/::SodiumCarbonate.MW * (0.01887*::SodiumCarbonate.MW + k1*Cl2C*2.0 + k2*(1.0-C2S)/GTZ(C2S) + k3*SO4_2C + k4*0.01887*TOOC2C*MW_C);
      return I;
      }
    case QASat_Alpart02:
      {
      /* Saturated Alumina concentration (ie Alumina concentration at Equilibrium)
      calculated using the formula provided by John McFeaters October 2002. 
      See PGM extract below:
      ;"temp" variables used in function
      Double T, C, AC, CS, Cl_C, SO4, TOC, CO3, I, Rho, x, AC_eq, FC, Al_FC_SSN, AC_ratio

      function CalcData(str Tag)
        StreamTag = Tag
              T_ = GetDynTag(strcat(StreamTag, ".Qi.T (dK)"))
              C_ = GetDynTag(strcat(StreamTag, ".Qi.C (g/L)"))
              AC_ = GetDynTag(strcat(StreamTag, ".Qi.A/C"))
              CS_ = GetDynTag(strcat(StreamTag, ".Qi.C/S"))
            Cl_C_ = GetDynTag(strcat(StreamTag, ".Qi.Cl/C"))
            Rho_ = GetDynTag(strcat(StreamTag, ".Qi.LRho25 (kg/m^3)")) / GetDynTag(strcat(StreamTag, ".Qi.LRho (kg/m^3)"))
            TOC_ = GetDynTag(strcat(StreamTag, ".Qi.TOOC (g/L)"))
            ;TOC_ = TOC_ * Rho_ * 0.113323401 kga this is WRONG use line below
            TOC_ = TOC_ * 0.113323401
            ;SO4_ = GetDynTag(strcat(StreamTag, ".Qi.CMC.Na2SO4 (g/L)")) kga this is WRONG use line below
            SO4_ = GetDynTag(strcat(StreamTag, ".Qi.CMC:L.Na2SO4 (g/L)"))
            SO4_ = SO4_ * Rho_ * 0.746172816
          CO32C_ = (1.0 - CS_) / CS_
     
            Ionic = C_ * ( 2.0 + 1.72831730426610*Cl_C_ + 0.61809588818785*CO32C_ ) 
            Ionic = Ionic + ( 1.70971468759441*SO4_  + 2.86068025913951*TOC_ )
            Ionic = Ionic / 105.9887

              I2_ = SQRT(Ionic)
              X_ = (-9.11181236102574 * I2_ / (1 + I2_)) - (-0.902799187095254 * Ionic) - (0.239197078637227 * pow(Ionic, 1.5))
            Keq_ = EXP(-30960 / (T_*8.3145) ) 
            AC_eq = 0.96197 / ( 1 + (pow(10.0, X_) / Keq_))
            A_eq = AC_eq * C_

        AC_ratio = AC_ / AC_eq

              FC = C_ * (1.0 - (1.0394993 * AC_))

        Al_FC_SSN = C_ * (AC_ - AC_eq) / FC

        return 0
      EndFunct
      */
      //NOTE: CausticConc (C) includes CausticSoda (NaOH) AND CausticSoda found in SodiumAluminate (NaAl[OH]4)
      // Know the following:  
      //   Cl2C = (Cl * 105.99 / (58.44 * 2)) / C         ; converts from g/L NaCl to g/L Na2CO3          [a]
      //  MW_Na2CO3/MW_Na2SO4=0.746172816                 ; ratio                                         [b]
      //Format provided by John McFeaters from Alpart:
      //    I = [C * (2 + k1_*Cl_C + k2_*CO32C) + (k3_*sO4*0.746172816 + k4_*toc)]/106                 
      //==> I = [C * (2 + k1_*Cl_C + k2_*CO32C) + (k3_*sO4*106/142 + k4_*toc)]/106
      //original form of equation for Aluminate Ionic Strength:
      //    I = 0.01887*C + k1*Cl/58.44 + k2*CO3/105.99 + k3*SO4/142.04 + k4*0.01887*TOC 
      //now, reformulate equation for Aluminate Ionic Strength:
      //==> I = C     * (0.01887     + k1*(Cl/58.44)/C     + k2*(CO3/106/C) )   + k3*(SO4/142.04)       + k4*0.01887*TOC		         ;multiply  by C/C
      //==> I = [C    * (0.01887*106 + k1*(Cl*106/58.44)/C + k2*(CO3*106/106/C))+ k3*(SO4*106/142.04)   + k4*0.01887*TOC*106]/106    ;multiply  by 106/106
      //==> I = [C    * (0.01887*106 + k1*(Cl2C*2)         + k2*CO32C )         + k3*(SO4*106/142.04)   + k4*0.01887*TOC*106]/106		 ;using [a]
      //k1_ = 1.72831730426610 = k1*2
      //k2_ = 0.61809588818785 = k2
      //k3_ = 1.70971468759441 = k3  (106/142/0.746172816=1)
      //k4_ = 2.86068025913951 = k4*0.01887*106

      const double k1 = 0.86415865213305; //1.72831730426610/2
      const double k2 = 0.61809588818785;
      const double k3 = 1.70971468759441; //1.70971468759441/MW_Na2CO3*MW_Na2SO4*0.746172816
      const double k4 = 1.4303348027741; //2.86068025913951/0.01887/MW_Na2CO3
      LiqConcs25.Converge(MArray());
      const double cTOC    = TOC(C_2_K(25.0));
      const double cNaCl   = LiqConcs25.Liq[::SodiumChloride.LiqPhInx()] /LiqConcs25.NaFactor[::SodiumChloride.LiqPhInx()];
      const double cNa2CO3 = LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]/LiqConcs25.NaFactor[::SodiumCarbonate.LiqPhInx()];
      const double cNa2SO4 = LiqConcs25.Liq[::SodiumSulphate.LiqPhInx()] /LiqConcs25.NaFactor[::SodiumSulphate.LiqPhInx()];
      const double cNaOH   = LiqConcs25.CausticSoda;
      const double C       = LiqConcs25.CausticSoda;
      I   = 0.01887*cNaOH + k1*cNaCl/::SodiumChloride.MW + k2*cNa2CO3/::SodiumCarbonate.MW + k3*cNa2SO4/::SodiumSulphate.MW + k4*0.01887*cTOC;
      return I;
      }
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

double QSMBayer::AluminaConcSat(double T_)
  {
  const double I = IonicStrength();
  const double I2 = Sqrt(I);
  double Aeq=0.0;
  switch (sm_iASatMethod)
    {
    case QASat_Rosenberg:
      {
      /* Saturated Alumina concentration (ie Alumina concentration at Equilibrium)
      calculated using the formula found in:
      Fourth International Alumina Quality Workshop  June 1996 Proceedings
      Rosenberg S.P, Healy S.J 'A Thermodynamic Model for Gibbsite Solubility
      in Bayer Liquors'.*/

      const double a0 = -9.2082;
      const double a3 = -0.8743;
      const double a4 = 0.2149;
      const double dG = -30960.0; //Gibbs energy of dissolution
      const double R  = R_c;//8.3145; //Universal Gas Constant 8.314472
      const double cC      = LiqConcs25.CausticSoda;
      const double X   = a0*I2/(1.0+I2) - a3*I - a4*Pow(I, 3.0/2.0);
      const double Keq = Exps(dG/(R*T_)); //Equilibrium Constant
      Aeq = 0.96197*cC/(1.0+(Pow(10.0, X)/Keq));
      }
    case QASat_QAL99:
      {
      /* QAL Saturated Alumina concentration: Original from precip model in code QprecipD.cpp.
      Based on 1996 Rosenberg S.P, Healy S.J 'A Thermodynamic Model for Gibbsite Solubility in Bayer Liquors'.*/
      const double a0 = -8.7168;//-9.2082;
      const double a3 = -0.7767;//-0.8743;
      const double a4 = 0.2028;//0.2149;
      const double dG = -30960.0;
      const double C   = LiqConcs25.CausticSoda;
      const double E   = a0*I2 / (1.0+I2) - a3*I - a4*Pow(GTZ(I),1.5);
      const double Keq = Exps(dG/(R_c*T_));
      Aeq = 0.96197*C / (1.0 + (Pow(10.0, E) / Keq));
      }
    case QASat_Alpart02:
      {
      /* Saturated Alumina concentration (ie Alumina concentration at Equilibrium)
      calculated using the formula provided by John McFeaters October 2002. */
      const double a0 = -9.11181236102574;
      const double a3 = -0.902799187095254;
      const double a4 = 0.239197078637227;
      const double dG = -30960.0;
      const double C   = LiqConcs25.CausticSoda;
      const double X   = a0*I2 / (1.0+I2) - a3*I - a4*Pow(GTZ(I),1.5);
      const double Keq = Exps(dG/(R_c*T_));
      Aeq = 0.96197*C / (1.0 + (Pow(10.0, X) / Keq));
      }
    }
  return Aeq;
  }

//---------------------------------------------------------------------------

double QSMBayer::BoundSodaSat(double T_)
  {
  //kga : added this calc 27/6/2002
  // QAL equation : Original from precip model in code QprecipD.cpp.
  LiqConcs25.Converge(MArray());
  const double DRatio = LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
  const double C_at25 = LiqConcs25.CausticSoda;
  const double C_atT  = LiqConcs25.CausticSoda*DRatio;
  const double Aeq    = AluminaConcSat(T_);
  const double ACeq   = Aeq/GTZ(C_atT);
  const double AC     = AtoC();
  const double TOOC2C = TOOCtoC();
  const double C2S    = CtoS();
  const double X      = dK1_BoundSodaSat * (0.000598*C_at25 - 0.00036*K_2_C(T_) + 0.019568*TOOC2C) * (1.0 - 0.758*C2S);
  const double BoundSoda = X * Sqr(AC-ACeq);
  return BoundSoda;
  }

//---------------------------------------------------------------------------

double QSMBayer::AtoCSaturation(double T_)
  {
  LiqConcs25.Converge(MArray());
  if (LiqConcs25.CausticSoda<1.0e-12)
    return 0.0;
  const double ACsat = AluminaConcSat(T_) / GTZ(LiqConcs25.CausticSoda);
  return ACsat;
  }

//---------------------------------------------------------------------------

double QSMBayer::SSNRatio(double T_)
  {// AtoC actual / AtoC saturation ==> A/ASat
  LiqConcs25.Converge(MArray());
  if (LiqConcs25.CausticSoda<1.0e-12)
    return 0.0;
  const double ACsat = AtoCSaturation(T_);
  return AtoC() / GTZ(ACsat);
  }

//---------------------------------------------------------------------------
//======================================== OxalateEquilibrium ===============================
// Function calculates the equilibrium concentration (g/l) of oxalate, based on stream properties.
// (British Aluminium - Burnt Island)
double QSMBayer::OxalateEquilibrium(double T_)//, double Approach)
  {
  ASSERT_ALWAYS(0, "Not implemented");
  return 0.0;
  /*const double LiqQv    = LVolume25(); // m^3/s
  if (LiqQv<1.0e-12)
    return 0.0;
  const double C        = CausticConc(C_2_K(25.0)); // g/l
  const double Na2CO3Qm = M[SodiumCarbonate.LiqPhInx()]; // kg/s
  const double OxEquil  = 7.62*Exps(0.012*K2C(T_) - (::OccSoda.MW/::SodiumCarbonate.MW)*(0.016*C + 0.011*Na2CO3Qm/LiqQv));
  return OxEquil; //(OxEquil+Approach);*/
  }

//---------------------------------------------------------------------------

double QSMBayer::AluminaSSN(double T_)
  {
/* the parameter (supersaturated alumina) / (free caustic) was found at QAL
to be a very good measure of clarification and press floor stability - it
was the best measure of the propensity for homogeneous nucleation of a
pregnant liquor. The measure is gpl of alumina (as alumina) above the
saturation concentration divided by free caustic - i.e. if there are 112 gpl
of Al2O3 and A-sat is 75 gpl then the supersaturated alumina is (112 - 75) =
37 gpl. If the free caustic was 70 gpl (as Na2CO3) then the supersaturation
number is (37/70) = 0.53. Maybe as "Alumina_SSN"
  Al_FC_SSN = C * (AC - AC_eq) / FC  */
  LiqConcs25.Converge(MArray());
  if (LiqConcs25.CausticSoda<1.0e-12)
    return 0.0;
  const double ACsat = AtoCSaturation(T_);
  const double FC = FreeCaustic(C_2_K(25.0));
  const double Al_FC_SSN = LiqConcs25.CausticSoda * (AtoC() - ACsat)/GTZ(FC);
  return Al_FC_SSN;
  }

//---------------------------------------------------------------------------
//
//double QSMBayer::PerformAluminaSolubility(double TRqd, double ARqd, double THARqd, double NoPerSec, double Na2OFac, double SSat, flag & ConvergeErr)
//  {
//  ASSERT_ALWAYS(0, "Not implemented");
//  return 0.0;
//  // x is the Fraction of Alumina which precipitates as the hydrate
//
//  /*double Fact=0.654;
//  double A=AluminaConc(T_);
//  for (int Iter=100; Iter; Iter--)
//    {
//    double &AluminaMass  = M[Alumina.LiqPhInx()];
//    double &WaterMass    = M[Water.LiqPhInx()];
//    double &THAMass      = M[THA.SolPhInx()];
//    double &CausticMass  = M[CausticSoda.LiqPhInx()];
//    double &Na2OMass     = M[Na2O.SolPhInx()];
//
//    double x     = AluminaMass*(1-ARqd/GTZ(A))/Fact;
//    double XSoda = x/100.0*Na2OFac*exp(2535/T_)*SSat*SSat;
//
//    AluminaMass += -Fact*x;
//    THAMass     += x;
//    WaterMass   += - (1-Fact)*x+18./62.*XSoda;
//    CausticMass += - 80./62.*XSoda;
//    Na2OMass    += + XSoda;
//
//    ClrStatesOK();
//    
//    A=AluminaConc(T_);
//    if (fabs(A-ARqd)<1.0e-6*ARqd)
//      break;
//    }
//  SetCI(5, Iter==0);
//
//  if (NoPerSec>0.0)
//    {
//    SetSAMFromFlow(THAMassFlow(), NoPerSec);
//    }
//
//  return AluminaConc(T_);*/
//  }

//---------------------------------------------------------------------------

double QSMBayer::AtoC()
  {
  LiqConcs25.Converge(MArray());
  //return LiqConcs25.Liq[::Alumina.LiqPhInx()] / GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()]);
  return LiqConcs25.Alumina / GTZ(LiqConcs25.CausticSoda);
  }

//---------------------------------------------------------------------------

double QSMBayer::DigAtoCEquil()
  {
  LiqConcs25.Converge(MArray());
  double Tc=GTZ(K_2_C(Temp()));
  const double SCarb  = LiqConcs25.Liq[SodiumCarbonate.LiqPhInx()];
  const double SChlor = LiqConcs25.Liq[SodiumChloride.LiqPhInx()];
  const double SOx    = LiqConcs25.Liq[SodiumOxalate.LiqPhInx()];
  const double OrgS   = LiqConcs25.Liq[OrganicSoda.LiqPhInx()];
  const double SSulph = LiqConcs25.Liq[SodiumSulphate.LiqPhInx()];
  const double CS     = LiqConcs25.CausticSoda;
  
  double AtoCE = (0.11992 + 0.00266125 * Tc)
               *
         ( 1.0 + 0.0869 * (SCarb + SChlor + SOx + OrgS + SSulph ) / GTZ(CS) )
               -
         (10.22+0.02975*Tc) / GTZ(CS)
               ;
  return AtoCE;
  }

//---------------------------------------------------------------------------

double QSMBayer::CtoS()
  {
  LiqConcs25.Converge(MArray());
  //return LiqConcs25.Liq[::CausticSoda.LiqPhInx()] 
  //       / GTZ(LiqConcs25.Liq[::CausticSoda.LiqPhInx()] + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]);
  return Max(0.0, LiqConcs25.CausticSoda) /
         GTZ(LiqConcs25.CausticSoda + LiqConcs25.Liq[::SodiumCarbonate.LiqPhInx()]);
  }

//---------------------------------------------------------------------------

double QSMBayer::CltoC()
  {
  LiqConcs25.Converge(MArray());
  return (LiqConcs25.Liq[SodiumChloride.LiqPhInx()])/GTZ(LiqConcs25.CausticSoda);
  }

//---------------------------------------------------------------------------

double QSMBayer::TOOCtoC()
  {
  LiqConcs25.Converge(MArray());
  return (LiqConcs25.Liq[SodiumOxalate.LiqPhInx()]*2.0 + LiqConcs25.Liq[OrganicSoda.LiqPhInx()]*5.2)/GTZ(LiqConcs25.CausticSoda);
  }

//---------------------------------------------------------------------------

double QSMBayer::TOOC(double T_)
  {
  LiqConcs25.Converge(MArray());
  double DRatio=LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
  return Max(0.0, (LiqConcs25.Liq[SodiumOxalate.LiqPhInx()]*2.0 + LiqConcs25.Liq[OrganicSoda.LiqPhInx()]*5.2)*DRatio);
  }

//---------------------------------------------------------------------------

double QSMBayer::TOC(double T_)
  {
  //TOOC = TOC*106/12
  LiqConcs25.Converge(MArray());
  double DRatio=LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
  return Max(0.0, (LiqConcs25.Liq[SodiumOxalate.LiqPhInx()]*2.0 + LiqConcs25.Liq[OrganicSoda.LiqPhInx()]*5.2)*DRatio*MW_C/::SodiumCarbonate.MW);
  }

//---------------------------------------------------------------------------

double QSMBayer::Na2SO4toC()
  {
  LiqConcs25.Converge(MArray());
  return (LiqConcs25.Liq[SodiumSulphate.LiqPhInx()])/GTZ(LiqConcs25.CausticSoda);
  }

//---------------------------------------------------------------------------

double QSMBayer::FreeCaustic(double T_)
  {
  return CausticConc(T_)*(1.0 - SodiumCarbonate.MW*AtoC()/InertAlumina.MW);
  }

//---------------------------------------------------------------------------

double QSMBayer::TotalNaOnCS(CSysVector * pMA)
  {
  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  LiqConcs25.Converge(MA);

  //The total sodium used for boiling point elevation, uses only the sodium
  //from the liquor compounds.
  //Total Sodium is the total amount of Sodium as deternined by AAS or XRF,
  //expressed as Na2CO3.
  double Na2C = (
             LiqConcs25.Liq[CausticSoda.LiqPhInx()]+
             LiqConcs25.Liq[SodiumAluminate.LiqPhInx()]+
             LiqConcs25.Liq[SodiumCarbonate.LiqPhInx()]+
             LiqConcs25.Liq[SodiumOxalate.LiqPhInx()]+
             LiqConcs25.Liq[SodiumChloride.LiqPhInx()]+
             LiqConcs25.Liq[SodiumSulphate.LiqPhInx()]+
             LiqConcs25.Liq[SodiumSilicate.LiqPhInx()]+
             LiqConcs25.Liq[OrganicSoda.LiqPhInx()] ) / GTZ(LiqConcs25.CausticSoda);

  return Na2C;
  }

//---------------------------------------------------------------------------

double QSMBayer::LTotalSodiumConc(CSysVector * pMA)
  {
  //Total Sodium is the total amount of Sodium as deternined by AAS or XRF,
  //expressed as Na2CO3 (in this case for liquor and solids compounds)
  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  LiqConcs25.Converge(MA);

  double TSodium = ( 
            LiqConcs25.Liq[CausticSoda.LiqPhInx()]+
            LiqConcs25.Liq[SodiumAluminate.LiqPhInx()]+
            LiqConcs25.Liq[SodiumCarbonate.LiqPhInx()]+
            LiqConcs25.Liq[SodiumOxalate.LiqPhInx()]+
            LiqConcs25.Liq[SodiumChloride.LiqPhInx()]+
            LiqConcs25.Liq[SodiumSulphate.LiqPhInx()]+
            LiqConcs25.Liq[SodiumSilicate.LiqPhInx()]+
            LiqConcs25.Liq[OrganicSoda.LiqPhInx()] );

  return TSodium;
  }

//---------------------------------------------------------------------------

double QSMBayer::BoilingPtElevation(double P_, CSysVector * pMA)
  {
  double T1 = SaturationT(P_, pMA);
  double PureSatT = SpModelEx::SaturationT(P_, pMA);
  return (T1-PureSatT)*sm_dBPEFactor; 
  }

//---------------------------------------------------------------------------

double QSMBayer::SaturationP(double T_, CSysVector * pMA)
  {
  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  if (sm_iQBMVer==QBM_Original)
    {
    LiqConcs25.Converge(MA);

    switch (sm_iSatTEqn)
      {
      case TSE_Revised97:
        return exp ( 17.43407 - ( 4737.8 + 0.479859 * LiqConcs25.CausticSoda * TotalNaOnCS(&MA) ) / Max(200.0, T_) );

      case TSE_Original:
      default:
        return exp ( 17.43407 - ( 4737.8 + 0.479859 * LiqConcs25.CausticSoda * TotalNaOnCS(&MA) ) / Max(200.0, T_) );
      }
    }
  else
    {
    if (MA.Sum(som_SL)/GTZ(MA.Sum())<1.0e-6)
      return SpModelEx::SaturationP(T_, &MA);

    double PureSatP = SpModelEx::SaturationP(T_, pMA);

    #if TestBayerForWater
    double TLiq = MA.Sum(som_Liq); // Total Liquid kg/s
    if ((TLiq>1e-6) && (MA[Water.LiqPhInx()]/TLiq>H2OTestFrac))
      return PureSatP;
    #endif
    
    LiqConcs25.Converge(MA);

    double SatP;
    switch (sm_iSatTEqn)
      {
      case TSE_Revised97:
        SatP = exp ( 17.43407 - ( 4737.8 + 0.479859 * LiqConcs25.CausticSoda * TotalNaOnCS(&MA) ) / Max(200.0, T_) );
        break;
      case TSE_Original:
      default:
        SatP = exp ( 17.43407 - ( 4737.8 + 0.479859 * LiqConcs25.CausticSoda * TotalNaOnCS(&MA) ) / Max(200.0, T_) );
        break;
      }
    //SetCI(?, SatP<PureSatP); //invalid bayer liquor!?
    SatP = Min(SatP, PureSatP);
    return SatP;
    }
  }

//---------------------------------------------------------------------------

double QSMBayer::SaturationT(double P_, CSysVector * pMA)
  {
  flag Local   = (pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  if (sm_iQBMVer==QBM_Original)
    {
    LiqConcs25.Converge(MA);

    switch (sm_iSatTEqn)
      {
      case TSE_Revised97:
        return (4737.8+0.479859*LiqConcs25.CausticSoda*TotalNaOnCS(&MA))/
                               (17.43407-log(Max(1.0, P_)));

      case TSE_Original:
      default:
        return (4737.8+0.479859*LiqConcs25.CausticSoda*TotalNaOnCS(&MA))/
                               (17.43407-log(Max(1.0, P_)));
        //CNM                         (17.43407-log(Max(10.0, P_)));
      }
    }
  else// if (iQBMVer==QBM_Revised1)
    {
    if (MA.Sum(som_SL)/GTZ(MA.Sum())<1.0e-6)
      return SpModelEx::SaturationT(P_, &MA);

    double PureSatT = SpModelEx::SaturationT(P_, pMA);

    #if TestBayerForWater
    double TLiq = MA.Sum(som_Liq); // Total Liquid kg/s
    if ((TLiq>1e-6) && (MA[Water.LiqPhInx()]/TLiq>H2OTestFrac))
      return PureSatT;
    #endif

    LiqConcs25.Converge(MA);

    const double TotNa2C = TotalNaOnCS(&MA);
    double SatT = (4737.8+0.479859*LiqConcs25.CausticSoda*TotNa2C)/
                               (17.43407-log(Max(1.0, P_)));
    /*switch (sm_iSatTEqn)
      {
      case TSE_Revised97:
        SatT = (4737.8+0.479859*LiqConcs25.CausticSoda*TotalNaOnCS(&MA))/
                               (17.43407-log(Max(1.0, P_)));
        break;
      case TSE_Original:
      default:
        SatT = (4737.8+0.479859*LiqConcs25.CausticSoda*TotalNaOnCS(&MA))/
                               (17.43407-log(Max(1.0, P_)));
        //CNM                         (17.43407-log(Max(10.0, P_)));
        break;
      }*/

    //SetCI(?, SatT<PureSatT); //invalid bayer liquor!?
    SatT = Max(SatT, PureSatT);
    return SatT;
    }
  }

//---------------------------------------------------------------------------

double QSMBayer::Rho(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);
  if (MA.Sum(som_SL)/GTZ(MA.Sum())<1.0e-6)
    return SpModelEx::Rho(Phase, T_, P_, &MA);

  SpMArray Mn(MA, Phase);
  Mn.Normalise();

  double FSol=Mn.Sum(som_Sol);
  double FLiq=Mn.Sum(som_Liq);
  double FVap=1.0-FSol-FLiq;
  if (sm_iQBMVer==QBM_Original)
    {
    double Dl=1.0,Ds=1.0, Dv=1.0;
    if (FLiq>1.0e-9)
      {
      //LiqConcs25.Converge(MA);
      LiqConcs25.Converge(Mn);
      Dl = LiqConcs25.LiquorDensity(T_);
      }
    if (FVap>1.0e-9)
      {
      Dv=SpModelEx::Rho(som_Gas, T_, P_, &Mn);
      }

    //return SDB.RhoMix(FSol, dNAN, FLiq, Dl, FVap, Dv, T_, P_, &m_Ovr, Mn);
    return 1.0/(FSol/Ds+FLiq/Dl+FVap/Dv);
    }
  else// if (sm_iQBMVer==QBM_Revised1)
    {
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
        Dl = sm_dRhoFactor*LiqConcs25.LiquorDensity(T_);
        }
      }
    //if (FVap>1.0e-9)
    //  Dv=SpModelEx::Rho(som_Gas, T_, P_, &Mn);

    return SDB.RhoMix(m_iFidelity, FSol, dNAN, FLiq, Dl, (1.0-FSol-FLiq), dNAN, T_, P_, &m_Ovr, Mn);
    }
  }

//---------------------------------------------------------------------------

double QSMBayer::msCp(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);
  if (MA.Sum(som_SL)/GTZ(MA.Sum())<1.0e-6)
    return SpModel::msCp(Phase, T_, P_, &MA, pTotalM);

  SpMArray Mn(MA, Phase);
  if (pTotalM)
    *pTotalM=Mn.Normalise();
  else
    Mn.Normalise();

  double FSol=Mn.Sum(som_Sol);
  double FLiq=Mn.Sum(som_Liq);
  if (sm_iQBMVer==QBM_Original)
    {
    double Cpl=0.0,Cps=0.0;
    double Tc = K_2_C(T_);
    if (FLiq>1.0e-9)
      {// Liquid
      LiqConcs25.Converge(Mn);

      double CausticC = LiqConcs25.CausticSoda;
      double AluminaC = LiqConcs25.Alumina;

      Cpl = 4.184 * ( 0.99639 - 3.90998e-4 * CausticC
                     - 5.3832e-4 * AluminaC
                     +CausticC*( 2.46493e-7 * CausticC + 5.7186e-7 * AluminaC)
                     -Tc*(1.8658e-7 * AluminaC + 1.07766e-7 * CausticC)
                     -1.51278e-4*Tc + 2.1464e-6*Tc*Tc);

      }
    if (FSol>1.0e-9)
      {
      //Solids
      double Ms=0.0;
      SpecieIter I(som_Sol);
      for (int sn=-1; I.Loop(sn); )
        {
        Cps+=Mn[sn] * SDB[sn].msCp(Fidelity(), T_, P_, &m_Ovr, Mn.SVData());
        Ms+=Mn[sn];
        }
      Cps/=Ms;
      }

  //  CSysVector Mv(Mn, som_Gas);
    return FSol*Cps+FLiq*Cpl+(1.0-FSol-FLiq)*SpModel::msCp(som_Gas, T_, P_);
    //return msCpMix(FSol, Cps, FLiq, Cpl, (1.0-FSol-FLiq), QSMSteam::msCp(som_Gas, T_, P_, Mn), T_, P_, Mn);
    }
  else// if (sm_iQBMVer==QBM_Revised1)
    {
    double Tc = K_2_C(T_);
    double Cpl=0.0;
    if (FLiq>1.0e-9)
      {// Liquid
      #if TestBayerForWater
      if (Mn[Water.LiqPhInx()]/FLiq>H2OTestFrac)
        Cpl = dNAN;
      else
      #endif
        {
        LiqConcs25.Converge(Mn);
        const double CausticC = LiqConcs25.CausticSoda;
        const double AluminaC = LiqConcs25.Alumina;
        const double K1 = (0.99639 - 3.90998e-4 * CausticC - 5.3832e-4 * AluminaC) + CausticC*(2.46493e-7 * CausticC + 5.7186e-7 * AluminaC);
        const double K2 = -1.51278e-4 -1.8658e-7 * AluminaC - 1.07766e-7 * CausticC;
        const double K3 = 2.1464e-6;

        Cpl = KCal_2_kJ(K1 + K2*Tc + K3*Tc*Tc); //(KCal_2_kJ=4.184)
        }
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
  }

// ==========================================================================

double QSMBayer::msHm(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  flag Local=(pMA==NULL);
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
  if (sm_iQBMVer==QBM_Original)
    {
    double Hl=0.0,Hs=0.0;
    double Tc = K_2_C(T_);
    if (FLiq>1.0e-9)
      {// Liquid
      LiqConcs25.Converge(Mn);
      const double CausticC = LiqConcs25.CausticSoda;
      const double AluminaC = LiqConcs25.Alumina;
      const double K1 = (0.99639 - 3.90998e-4 * CausticC - 5.3832e-4 * AluminaC) + CausticC*(2.46493e-7 * CausticC + 5.7186e-7 * AluminaC);
      const double K2 = -1.51278e-4 -1.8658e-7 * AluminaC - 1.07766e-7 * CausticC;
      const double K3 = 2.1464e-6;

      Hl = 4.184 * Tc * ( K1 +  K2 * Tc / 2.0 + K3 * Tc * Tc / 3.0);

      double A2C=AluminaC / GTZ(CausticC);
      double FreeCaustic=CausticC*(1.0-SodiumCarbonate.MW*A2C/InertAlumina.MW);
      double FreeCausticH=FreeCaustic*0.02871;
      double HContribution=18.86917*FreeCaustic/LiqConcs25.LiquorDensity(T_);

      Hl=Hl+FreeCausticH*HContribution;
      }
    if (FSol>1.0e-9)
      {//Solids
      double Ms=0.0;
      SpecieIter I(som_Sol);
      for (int sn=-1; I.Loop(sn); )
        {
        Hs+=Mn[sn] * (SDB[sn].msHs(Fidelity(), RefTemp(), T_, &m_Ovr, Mn.SVData()));// - SDB[sn].msEnthalpyZR(RefTemp(), ZeroCinK));//, P_);
        Ms+=Mn[sn];
        }
      Hs/=Ms;
      }

    //CSysVector Mv(Mn, som_Gas);
    return FSol*Hs+FLiq*Hl+(1.0-FSol-FLiq)*SpModel::msHz(som_Gas, T_, P_);
    //return msEnthalpyZRMix(FSol, Hs, FLiq, Hl, (1.0-FSol-FLiq), QSMSteam::msEnthalpyZR(som_Gas, T_, P_, Mn), T_, P_, Mn);
    }
  else// if (sm_iQBMVer==QBM_Revised1)
    {
    double Tc = K_2_C(T_);
    double Hl=0.0;
    if (FLiq>1.0e-9)
      {// Liquid
      #if TestBayerForWater
      if (Mn[Water.LiqPhInx()]/FLiq>H2OTestFrac)
        Hl = dNAN;
      else
      #endif
        {
        LiqConcs25.Converge(Mn);
        const double CausticC = LiqConcs25.CausticSoda;
        const double AluminaC = LiqConcs25.Alumina;
        const double K1 = (0.99639 - 3.90998e-4 * CausticC - 5.3832e-4 * AluminaC) + CausticC*(2.46493e-7 * CausticC + 5.7186e-7 * AluminaC);
        const double K2 = -1.51278e-4 -1.8658e-7 * AluminaC - 1.07766e-7 * CausticC;
        const double K3 = 2.1464e-6;

        Hl = KCal_2_kJ(Tc * (K1 +  K2 * Tc / 2.0 + K3 * Tc * Tc / 3.0)); //(KCal_2_kJ=4.184)

        double A2C=AluminaC / GTZ(CausticC);
        double FreeCaustic=CausticC*(1.0-SodiumCarbonate.MW*A2C/InertAlumina.MW);
        double FreeCausticH=FreeCaustic*0.02871;
        double HContribution=18.86917*FreeCaustic/LiqConcs25.LiquorDensity(T_);

        Hl = Hl+FreeCausticH*HContribution;
        }
      }

    double Hs=0.0;
    if (FSol>1.0e-9)
      {//Solids
      /*double Cps = SDB.msCp(som_Sol, T_, P_, &m_Ovr, Mn);
      Cps = Max(Cps, dMinSolCp);
      Hs = Cps*Tc;*/
      Hs = SDB.msHz(m_iFidelity, som_Sol, T_, P_, &m_Ovr, Mn);
      }

    return SDB.msHmMix(m_iFidelity, FSol, Hs, FLiq, Hl, (1.0-FSol-FLiq), dNAN, T_, P_, &m_Ovr, Mn);
    }
  }

// --------------------------------------------------------------------------

double QSMBayer::SpecieConc(double T_, int iSpNo, PhMask PhaseM)
  {
  if (!SDB[iSpNo].IsLiq())
    return SpModelEx::SpecieConc(T_, iSpNo, PhaseM);

  LiqConcs25.Converge(MArray());

  double C;
  if (iSpNo==::CausticSoda.LiqPhInx())
    C=LiqConcs25.CausticSoda;
  else
    C=LiqConcs25.Liq[iSpNo];

  if (fabs(T_ - C_2_K(25.0))>0.1)
    return C * LiqConcs25.LiquorDensity(T_)/GTZ(LiqConcs25.Density);
  return C;
  }

// --------------------------------------------------------------------------

double QSMBayer::PhaseConc(double T_, PhMask Ph, PhMask PhaseM)
  {
  // To Be Improved
  return SpModelEx::PhaseConc(T_, Ph, PhaseM);
  }

//--------------------------------------------------------------------------

//todo : check!   Viscosity
double QSMBayer::DynamicViscosity(PhMask Phase, double T_, double P_, CSysVector * pMA/*=NULL*/)
  {
  if (Phase & som_Gas)
    return SMBayerBase::DynamicViscosity(Phase, T_, P_, pMA); //todo : check!

  flag Local=(pMA==NULL);
  CSysVector &MA = (Local ? MArray() : *pMA);

  #if TestBayerForWater
  double TLiq = MA.Sum(som_Liq); // Total Liquid kg/s
  if (TLiq>1.0e-6 && MA[Water.LiqPhInx()]/TLiq>H2OTestFrac)
    return SMBayerBase::DynamicViscosity(Phase, T_, P_, pMA);
  #endif

  LiqConcs25.Converge(MA);
  double Tc = Max(-41.0, K_2_C(T_));
  return exp ( 0.58-1.816*log(Tc+42.0) + 
              (0.5*LiqConcs25.CausticSoda + 
               0.43*LiqConcs25.CausticSoda*LiqConcs25.Alumina / GTZ(LiqConcs25.CausticSoda)) /
               (Tc+42.0) );
  }

//--------------------------------------------------------------------------

double QSMBayer::ThermalConductivity(PhMask Phase, double T_, double P_, CSysVector * pMA/*=NULL*/)
  {
  return SMBayerBase::ThermalConductivity(Phase, T_, P_, pMA); 
  }

//--------------------------------------------------------------------------

flag QSMBayer::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tLiquor Density not converged"; return 1;
    case 2: pS="W\tRequirements not met"; return 1;
    default:                                               
      return SpModelEx::CIStrng(No, pS);
    }
  }

// ==========================================================================

