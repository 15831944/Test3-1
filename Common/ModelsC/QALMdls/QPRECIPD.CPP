//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#include "flwnode.h"
#define  __QPRECIPD_CPP
#include "qprecipd.h"
#include "alspmdlb.h"
#include "qbspmdl.h"

//#include "optoff.h"

typedef SMBayerBase SMBayerB;

//==========================================================================

static MInitialiseTest InitTest(&QPrecipDClass);
//Specie Access 
SPECIEBLK_L(InitTest, Water,                 "H2O(l)",           false);
SPECIEBLK_L(InitTest, SodiumAluminate,       "NaAl[OH]4(l)",     false);
SPECIEBLK_L(InitTest, SodiumCarbonate,       "Na2CO3(l)",        false);
SPECIEBLK_L(InitTest, CausticSoda,           "NaOH(l)",          false);
SPECIEBLK_S(InitTest, Gibbsite,              "Al[OH]3(s)",       false);

SPECIEBLK_S(InitTest, InertAlumina,          "Al2O3(s)",         false);

//"optional" species...
SPECIEBLK_L(InitTest, OrgSodaL,              "Na2C5.2O7.2(l)",   true);
SPECIEBLK_L(InitTest, CausticL,              "NaOH(l)",          true);
SPECIEBLK_S(InitTest, OrgSodaS,              "Na2C5.2O7.2*(s)",  true);
SPECIEBLK_S(InitTest, CausticS,              "NaOH*(s)",         true);

//Extra engineering conversion units
DATACNV_PRI(DC_TCONST, "dC/s.C",               0.0, 1.0, "Therm Const", true);
DATACNV_SEC(DC_TCONST, "dC/h.C",  3600.0, 0.0, 1.0, 1000, true);
DATACNV_PRI(DC_FracGL, "/kg/m^3",              1.0, 100.0, "dPrecipRate", true);
DATACNV_SEC(DC_FracGL, "%/kg/m^3", 100.0, 0.0, 1.0, 100.0, true);
DATACNV_SEC(DC_FracGL, "%/g/L",    100.0, 0.0, 1.0, 100.0, true);

const double MW_C        = 12.011;

//==========================================================================

const byte ioid_Feed         = 0;
const byte ioid_Product      = 1;
const byte ioid_SecFeed      = 2;
const byte ioid_SecProduct   = 3;

static IOAreaRec QPrecipIOAreaList[] =
  {{"Feed",     "Feed",     ioid_Feed      , LIO_In ,    nc_MLnk, 1, 10,     0, 1.0f},
   {"Overflow", "Overflow", ioid_Product   , LIO_Out,    nc_MLnk, 1,  1,     0, (float)0.9},
   {"SecFeed",  "SecFeed",  ioid_SecFeed   , LIO_In ,    nc_MLnk, 0, 10,     0, 1.0f},
   {"Side",     "Side",     ioid_SecProduct, LIO_Out,    nc_MLnk, 0,  1,     0, (float)0.5},
   {NULL}}; //This lists the areas of the model where links can be attached.


static double Drw_QPrecip[] = {  DD_Poly, -5,14, -5,-14, 0,-16, 5,-14, 5,14, -5,14,
                               DD_End }; 

//--------------------------------------------------------------------------

byte QPrecipD::iMethod=3; //global

enum PM_PrecipMethod { PM_Original, PM_Feb99, PM_June01, PM_June01b, PM_May03, PM_May03b };
enum SSM_SecSplitMode { SSM_None, SSM_Frac, SSM_Qm, SSM_Qv };

IMPLEMENT_MODELUNIT(QPrecipD, "QPrecip", "D", Drw_QPrecip, "Tank", "PCP", TOC_ALL|TOC_GRP_ALUMINA|TOC_QAL,
                    "Process:Unit:QAL Precipitator",
                    "The precipitator models a continuous gibbsite crystallisation "
                    "precipitation tank. The model uses a precipitation rate equation " 
                    "developed by B. Cristol. The model calculates yield, flow solids "
                    "in tank, residence time, specific surface area of hydrate, bound "
                    "soda, and the caustic concentration and alumina concentration in "
                    "the tank. The precipitator receives up to 20 feed streams and "
                    "calculates a single overflow stream. The model does not take into "
                    "account by-pass flow.")

QPrecipD::QPrecipD(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(QPrecipIOAreaList);
  Contents.SetClosed(False);
//  Contents.SetAllowAdjust(FALSE);
  //EHX.Open(&CEHX_LossPerQmClass);
  
  bOnLine=1;
  iSecSplitMethod=SSM_None;
  TankVol=2000.0;
  ActivationEnergy=7600.0;
  K0=1.42e10;//6e8;//1.42E14
  PoisonFactor=1.0;
  PercM45=0.2;
  SplitRqd=0.0;

  ThermalConst=0.0/3600.0;
  AmbientTemp=C_2_K(25.0);

  k_TOC = QSMBayer::dGrowth_k_TOC;//0.0023;
  n_s   = QSMBayer::dGrowth_n_s;//-1.0;
  n_fc  = QSMBayer::dGrowth_n_fc;//-2.0;
  n_eq  = QSMBayer::dGrowth_n_eq;//0.0;
  n_    = QSMBayer::dGrowth_n_;//0.0;
  n_sol = QSMBayer::dGrowth_n_sol;//1.0;
  n_ssa = QSMBayer::dGrowth_n_ssa;//0.6;
  n_s2 = 0.0;
  n_fc2 = 0.0;
  Damping = 0.0;
  MaxACIter = 100;
  dMaxT = C_2_K(80.0);
  dMinC = 200.0;
  PrecRateDamping = 0.0;
  Sol = 0.0;
  S_out = 0.0;

  SSA=0.0;
  TSA=0.0;
  Yield=0.0;
  BoundSoda=0.0;
  IonicStrength=0.0;
  Aeq=0.0;
  ResidenceTime=0.0;
  dSolPrecip=0.0;
  dThermalLoss=0.0;
  dExtraDuty=0.0;
  dReactionHeat=0.0;
  bWithHOR=false;
  dPrecipRate=dNAN;
  dPrevTout=dNAN;

  ACin=0.0;
  ACeq=0.0;
  ACout=0.0;

#if WithSzStuff
  CSD_DistDefn *pD=SD_Defn.FindDistributionFor(Gibbsite.SolPhInx());
  Fn.SetSize(pD ? pD->NIntervals() : 0);
#endif

  fBoundSpcsOK=OrgSodaS.SolPhInx()>=0 &&
               CausticS.SolPhInx()>=0 &&
               OrgSodaL.LiqPhInx()>=0 &&
               CausticL.LiqPhInx()>=0;
  fDoBoundStuff=false;
  dK1=QSMBayer::dK1_BoundSodaSat;//2.35;
  dBndOrgSoda=0.1;
  dSSAAdj=1.0;
//  SfeSD_Defn &XX=SD_Defn;
  }

// -------------------------------------------------------------------------

QPrecipD::~QPrecipD()
  {
  }

// -------------------------------------------------------------------------

static DDBValueLst DDBVersion[] = {
  {PM_Original, "Original"},
  {PM_Feb99,    "Revised:Feb/99"},
  {PM_June01,   "Revised:Jun/01"},
  {PM_June01b,  "Revised:Jun/01b"}, //QAL
  {PM_May03,    "Revised:May/03"},  //KaiserAl
  {PM_May03b,   "Revised:May/03b"},  //KaiserAl
  {0}};
static DDBValueLst DDBSecSplitMethod[] = {
  {SSM_None, "None"},
  {SSM_Frac, "MassFrac"},
  {SSM_Qm,   "MassFlow"},
  {SSM_Qv,   "VolumeFlow"},
  {0}};
static DDBValueLst DDBPrcpFuns[]=
  {
    {PF_Ignore,    "Ignore"},
    {PF_ConstG,    "FixedGrowth"},
    {PF_ConstD,    "FixedDestruction"},
    {PF_LinearG,   "LinearGrowth"},
    {PF_LinearD,   "LinearDestruction"},
    {0}
  };

void QPrecipD::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  dword Flags0=(iMethod==PM_Original ? 0 : noView);
  dword Flags1=(iMethod==PM_Feb99    ? 0 : noView);
  dword Flags2=(iMethod==PM_June01   ? 0 : noView);
  dword Flags3=(iMethod==PM_June01b  ? 0 : noView);
  dword Flags4=(iMethod==PM_May03 || iMethod==PM_May03b    ? 0 : noView);
  dword Flags04=(iMethod==PM_Original || iMethod==PM_May03 || iMethod==PM_May03b ? 0 : noView);
  dword Flags014=(iMethod==PM_Original || iMethod==PM_Feb99 || iMethod==PM_May03 || iMethod==PM_May03b ? 0 : noView);
  dword Flags23=(iMethod==PM_June01 || iMethod==PM_June01b ? 0 : noView);
  dword Flags123=(iMethod==PM_Feb99 || iMethod==PM_June01 || iMethod==PM_June01b ? 0 : noView);
  dword Flags234=(iMethod==PM_June01 || iMethod==PM_June01b || iMethod==PM_May03 || iMethod==PM_May03b ? 0 : noView);
  dword FlagsB=(fBoundSpcsOK ? 0 : noView);

  DDB.Visibility();
  DDB.CheckBox    ("On",               "",DC_,       "",       &bOnLine         , this, isParm|SetOnChange);
  DDB.Byte        ("Method",           "",DC_,       "",       &iMethod         , this, isParm|SetOnChange, DDBVersion);
  DDB.Text        ("");
  DDB.Double      ("TankVol",          "",DC_Vol,    "kL",     &TankVol         , this, isParm);
  DDB.Double      ("ActivationEnergy", "",DC_T,      "K",      &ActivationEnergy, this, isParm);
  DDB.Double      ("K0",               "",DC_,       "",       &K0              , this, isParm);
  DDB.Double      ("PoisonFactor",     "",DC_,       "",       &PoisonFactor    , this, isParm|Flags014);
  DDB.Double      ("PercM45",          "",DC_Frac,   "%",      &PercM45         , this, isParm|Flags04);
  DDB.Double      ("K1",               "",DC_,       "",       &dK1             , this, isParm);
  DDB.Double      ("n_s_",             "",DC_,       "",       &n_s2            , this, isParm|Flags4);
  DDB.Double      ("n_fc_",            "",DC_,       "",       &n_fc2           , this, isParm|Flags4);
  DDB.Double      ("k_TOC",            "",DC_,       "",       &k_TOC           , this, isParm|Flags23);
  DDB.Double      ("n_s",              "",DC_,       "",       &n_s             , this, isParm|Flags23);
  DDB.Double      ("n_fc",             "",DC_,       "",       &n_fc            , this, isParm|Flags23);
  DDB.Double      ("n_eq",             "",DC_,       "",       &n_eq            , this, isParm|Flags23);
  DDB.Double      ("n_",               "",DC_,       "",       &n_              , this, isParm|Flags23);
  DDB.Double      ("n_sol",            "",DC_,       "",       &n_sol           , this, isParm|Flags23);
  DDB.Double      ("n_ssa",            "",DC_,       "",       &n_ssa           , this, isParm|Flags23);
  DDB.Double      ("Damping",          "",DC_,       "",       &Damping         , this, isParm|Flags23);
  DDB.Byte        ("MaxACIter",        "",DC_,       "",       &MaxACIter       , this, isParm|Flags3);
  DDB.Double      ("PrecRateDamping",  "",DC_,       "",       &PrecRateDamping , this, isParm);
  DDB.Double      ("MaxT",             "",DC_T,      "dC",     &dMaxT           , this, isParm);
  DDB.Double      ("MinCausticConc",   "",DC_Conc,   "g/L",    &dMinC           , this, isParm);
  DDB.Double      ("Sol",              "",DC_,       "",       &Sol             , this, 0);
  DDB.Double      ("S_out",            "",DC_,       "",       &S_out           , this, 0|Flags234);

  DDB.CheckBoxBtn ("Add_ReactionHeat", "",DC_,       "",       &bWithHOR        , this, isParm, DDBYesNo);
  DDB.CheckBoxBtn ("AddBoundSoda",     "",DC_,       "",       &fDoBoundStuff   , this, isParm|FlagsB, DDBYesNo);
  DDB.Double      ("BoundSoda_OrgPart","",DC_Frac,   "%",      &dBndOrgSoda     , this, isParm|FlagsB);
  DDB.Double      ("SSA_Adjust",       "",DC_Frac,   "",       &dSSAAdj         , this, isParm|Flags04);
  DDB.Double      ("SSA",              "",DC_SurfAreaM, "m^2/g", &SSA           , this, (iMethod!=PM_Original && iMethod!=PM_May03 && iMethod!=PM_May03b) ? isParm : 0);
  DDB.Double      ("TotSeedSurfArea",  "",DC_SurfAreaL, "m^2/L", &TSA           , this, 0);
  DDB.Double      ("Yield",            "",DC_Conc,   "kg/m^3", &Yield           , this, 0);
  DDB.Double      ("BoundSoda",        "",DC_Frac,   "%",      &BoundSoda       , this, 0);
  DDB.Double      ("ResidenceTime",    "",DC_Time,   "h",      &ResidenceTime   , this, 0);
  DDB.Double      ("ACin",             "",DC_,       "",       &ACin            , this, 0);
  DDB.Double      ("IonicStrength",    "",DC_,       "",       &IonicStrength   , this, 0|InitHidden);
  DDB.Double      ("Aeq",              "",DC_Conc,   "kg/m^3", &Aeq             , this, 0|InitHidden);
  DDB.Double      ("ACeq",             "",DC_,       "",       &ACeq            , this, 0);
  DDB.Double      ("ACout",            "",DC_,       "",       &ACout           , this, 0);
  DDB.Double      ("SolidsPrecip",     "",DC_Qm,     "kg/s",   &dSolPrecip      , this, 0);
  
                  
  DDB.Double      ("ThermalConst",     "",DC_TCONST, "dC/h.C", &ThermalConst    , this, isParm);
  DDB.Double      ("AmbientTemp",      "",DC_T,      "dC",     &AmbientTemp     , this, isParm);
  DDB.Double      ("ThermalLoss",      "",DC_Pwr,    "kW",     &dThermalLoss    , this, 0);
  DDB.Double      ("ReactionHeat",     "",DC_Pwr,    "kW",     &dReactionHeat   , this, 0);
  DDB.Byte        ("SplitMethod",      "",DC_,       "",       &iSecSplitMethod , this, isParm|SetOnChange, DDBSecSplitMethod);
  DDB.Visibility(NSHM_All, iSecSplitMethod!=SSM_None);
  if (iSecSplitMethod==SSM_Frac)
    DDB.Double      ("RqdSideFrac",      "",DC_Frac,   "%",      &SplitRqd        , this, isParm);
  else if (iSecSplitMethod==SSM_Qm)
    DDB.Double      ("RqdSideQm",        "",DC_Qm,     "kg/s",   &SplitRqd        , this, isParm);
  if (iSecSplitMethod==SSM_Qv)
    DDB.Double      ("RqdSideQv",        "",DC_Qv,     "m^3/s",  &SplitRqd        , this, isParm);
  DDB.Double      ("SecSideDuty",        "",DC_Pwr,    "kW",     &dExtraDuty      , this, 0|InitHidden);
  DDB.Double      ("PrevT_out",          "",DC_T,      "dC",     &dPrevTout       , this, 0|noView);
  DDB.Visibility();
//  DDB.Text    ("");
  RB.Add_OnOff(DDB, False);
  
  DDB.Visibility();

  //EHX.Add_OnOff(DDB, False);
  //EHX.BuildDataDefn(DDB);

  BuildDataDefnShowIOs(DDB);
  
#if WithSzStuff
  DDB.Page("Growth Fns");
  Strng Tg;

  CSD_DistDefn *pD=SD_Defn.FindDistributionFor(Gibbsite.SolPhInx());
  int nIntervals=Fn.GetSize();
  DDB.BeginArray(this, "Fn", "PrcpFuns", nIntervals, 0);
  for (int i=0; i<nIntervals; i++)
    {
    Tg.Set("I%i", i);
//    Tg.Set("I%.1f", pD->Intervals[i]);
//
    DDB.BeginElement(this, Tg());
//    Tg.Set("<%i", i*2);
    if (i<nIntervals-1)
      Tg.Set("<%.1fum", pD->Intervals[i]*1.0e6);
    else
      Tg.Set("TopSize");
    DDB.TagComment(Tg());

    DDB.Byte("Type",  "",  DC_ , "",  &Fn[i].iType,    this, isParm|SetOnChange, DDBPrcpFuns);
    // Constant
    DDB.Visibility(NSHM_All, Fn[i].iType==PF_ConstG || Fn[i].iType==PF_ConstD);
    DDB.Double ("C",  "",  DC_Frac , "%",  &Fn[i].C,    this, isParm);
    // Linear
    DDB.Visibility(NSHM_All, Fn[i].iType==PF_LinearG || Fn[i].iType==PF_LinearD);
    DDB.Double ("A",  "",  DC_FracGL , "%/g/L",  &Fn[i].A,    this, isParm);
    DDB.Double ("B",  "",  DC_Frac ,   "%",      &Fn[i].B,    this, isParm);
    
  //  DDB.Text("");
    }
  DDB.EndArray();
//  DDB.Page("Growth Fns");
//            SQSzDist1::SplitSolids(Info, Qm(), Qo, Qu);
#endif

  RB.BuildDataDefn(DDB);
  DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);


//  if (Contents.SpillExists())
//    DDB.Object(&Contents.OverFlow(), this, NULL, NULL, DDB_RqdPage);
  DDB.EndStruct();
 
  };

//--------------------------------------------------------------------------

flag QPrecipD::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;

  return 0;
  }

//---------------------------------------------------------------------------

flag QPrecipD::ValidateData(ValidateDataBlk & VDB)
  {
  return MN_Surge::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

void QPrecipD::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NoProcessJoins(); j++)
        {
        double P = PBPress();//AtmosPress(IODatum_Term(j));
        SetJoinPressure(j, P, true, true);
        }
      break;
      }
    case NM_Dynamic:
      MN_Surge::EvalJoinPressures(JoinMask);
      break;
    }
  };
 
//--------------------------------------------------------------------------

void QPrecipD::EvalJoinFlows(int JoinNo)
  {
  switch (NetMethod())
    {
    case NM_Probal:
    case NM_Dynamic:
      break;
    }
  };
 
//--------------------------------------------------------------------------

void QPrecipD::EvalSteadyState()
  {
  };

//--------------------------------------------------------------------------

class ACoutFnd : public MRootFinderBase
  {
  public:
    SpConduit &Qp;
    SMBayerB  &Bm;
    double H0; 
    double TempIn;
    double SodiumAluminateMoles;
    double GibbsiteMoles;
    double CausticSodaMoles;

    double T;           // deg C
    double VIn;         // kL/h
    double VLiq;        // kL/h
    double MSolids;     // t/h Gibbsite
    double ResTimeHrs;  // Hours

    double ThermalConst;
    double AmbientTemp;
    double TankVol;
    double &dThermalLoss;
    double &dExtraDuty;
    double &dReactionHeat;

    double C;           // g/l
    double C_at25;      // g/l
    double FC;          // g/l
    double C2S;         // ratio
    double Cl2C;        // ratio
    double TOOC2C;      // ratio
    double K0;
    double K;           // Kinetic Rate constant
    double PoisonFactor; // MJM
    double &ACin;       // Feed A/C ratio
    double &ACeq;       // Equilibrium A/C ratio
    double &ACout;      // Product A/C ratio
    double I;           // Equilibrium Intermediate (Ionic Strength)
    double E;           // Equilibrium Intermediate
    double Aeq;         // Equilibrium A Conc

    // new method Params
    double n_s2;
    double n_fc2;
    double k_TOC;
    double n_s;
    double n_fc;
    double n_eq;
    double n_;
    double n_sol;
    double n_ssa;
    double Damping;
    byte   MaxACIter;
    double Sol;
    double S_out;

    double PercM45;
    double ActivationEnergy;
    double SSA;
    double TSA;
    flag   bWithHOR;
    byte   iMethod;
    double dSSAAdj;
    static CToleranceBlock s_Tol;

    ACoutFnd(SpConduit &Qp_, double * pThermalLoss, double * pExtraDuty, double * pReactionHeat, double * pACin, double * pACeq, double * pACout);
    double Function(double x);
  };

CToleranceBlock ACoutFnd::s_Tol(TBF_BothSys, "Precip:ACFnd", 0, 1.0e-8);

//--------------------------------------------------------------------------
    
ACoutFnd::ACoutFnd(SpConduit &Qp_, double * pThermalLoss, double * pExtraDuty, double * pReactionHeat, double * pACin, double * pACeq, double * pACout) :
  MRootFinderBase("Precip.ACFnd", s_Tol),//1.0e-8), 
    Qp(Qp_), 
    Bm(*((SMBayerB*)Qp.Model())),
    dThermalLoss(*pThermalLoss),
    dExtraDuty(*pExtraDuty),
    dReactionHeat(*pReactionHeat),
    ACin(*pACin),
    ACeq(*pACeq),
    ACout(*pACout)
  {

  TempIn=Bm.Temp();
  H0=Bm.totHf(som_ALL, Bm.Temp(), Bm.Press());
  SodiumAluminateMoles=Bm.VMole[SodiumAluminate.LiqPhInx()];
  GibbsiteMoles=Bm.VMole[Gibbsite.SolPhInx()];
  CausticSodaMoles=Bm.VMole[CausticSoda.LiqPhInx()];
  }

//---------------------------------------------
/*** Requested changes 26/6/01...
> -----Original Message-----
> From:	Thomas, Daniel 
> Sent:	Monday, 25 June 2001 16:38
> To:	Frost, Ian
> Cc:	Garner, Brett
> Subject:	Precip Rate Equation Changes
> 
> Ian,
> 
> Required changes to rate equation:
> 
> New:
> *****************************************************
>     case 1:
>       Sol=MSolids*1000./GTZ(VLiq);     // g solids per L liq
>   
>       // Ionic Strength
>       I = 
> C/106 * (0.01887*106 + 1.5532*Cl2C*2 + 1.3437*(1-C2S) / GTZ((C2S)) +
> 2.2571*CNa2SO4/C + 1.1997*0.01887*TOOC2C*12)
> 
>       // Equilibrium A/C ratio
>       Aeq = 
> 0.96197*C / (1 + ((10 ^ (-8.7168 * (GTZ(I))^0.5 / (1 + (GTZ(I))^0.5) +
> 0.7767 * I - 0.2028 * (GTZ(I)) ^ 1.5)) / (exp(-30960 / (8.31441 * T)))))
> 
      ACeq = Aeq/C;
>       K =
> K0*exp(-ActivationEnergy/T)*exp(-TOOC2C*C*k_TOC) * GTZ(S_out)^n_s *
> GTZ(FC)^n_fc * GTZ(ACeq)^n_eq 
> 
> 
> 
> // Product A/C ratio
>  
> // ACout=ACin - K * ResTimeHrs * GTZ(ACout-ACeq)^n * GTZ(Sol)^n_sol *
> GTZ(SSA)^n_ssa 
> 
> // Note this is iterative and can become unstable particularly in tanks
> with high precipitation rate.
> // Stability can improved by using:
> 
> ACout= Damping * ACout + (1-Damping) * (ACin - K * ResTimeHrs *
> GTZ(ACout-ACeq)^n * GTZ(Sol)^n_sol * GTZ(SSA)^n_ssa)
> 
> // Damping = 0.9 works pretty well.
> // I presume at the start of each Syscad iteration the starting value of
> ACout will be remembered from the previous iteration.
> 
> ;
>       break;
> 
> ****************************************************
> New variables introduced:
> 
> Sol:  solids concentration in grams per litre of liquor (not per litre of
> slurry)
> S_out: Soda concentration at tank outlet, = C/C2S
> 
*****************************************************
Check that FC: free caustic at tank outlet, = C * (1-1.0395 * A/C)

> *****************************************************
> Input Parameters Introduced:
> k_TOC: Rate term for TOC effect on rate - current value is 0.0023
> n_s:  exponent for soda-dependence of rate constant, current value is -1
> n_fc:  exponent for free-caustic-dependence of rate constant, current
> value is -2
> n_eq: exponent for equilibrium-A/C-dependence of rate constant, current
> value is 0
> n_sol: exponent for solids-dependence of precipitation rate, current value
> is 1
> n_ssa: exponent for SSA-dependence of precipitation rate, current value is
> 0.6
> Damping:  Damping factor; 0=no damping, 1=infinite damping
> 
> 
> Daniel.
---------------------------------------------------------------------- 
***/
//  Solub = ret_val = cs * (temp * 0.005713 + cs * 9.77e-4 - 0.2641);
//  Solub = ret_val = C * (T * 0.005713 + C * 9.77e-4 - 0.2641);
//  Solub = ret_val = aC^2+bCT+cC;


double ACoutFnd::Function(double x) 
  { 
  // x is the Moles of Alumina which precipitates as the hydrate i.e. x is PrecipRate

  Bm.m_M.VValue[SodiumAluminate.LiqPhInx()] = Max(0.0, (SodiumAluminateMoles - x) * SodiumAluminate.MW);
  Bm.m_M.VValue[Gibbsite.SolPhInx()]        = Max(0.0, (GibbsiteMoles + x       ) * Gibbsite.MW);
  Bm.m_M.VValue[CausticSoda.LiqPhInx()]     = Max(0.0, (CausticSodaMoles + x    ) * CausticSoda.MW);

  Bm.ClrStatesOK();

  // Option - Const Temp + Heat of Reaction
  //dReactionHeat=1030.0*x*SodiumAluminate.MW;Changed I Frost 15 April 99
  dReactionHeat = 36465.0*x;
  Bm.Set_totHf(H0+(bWithHOR ? dReactionHeat : 0.0));
  
  // Thermal Losses.
  double H1 = Bm.totHf(som_ALL, Bm.Temp(), Bm.Press(), Bm.pMArray());
  double dT = Range(0.0, ThermalConst*ResTimeHrs*3600.0, 0.99)*(Bm.Temp()-AmbientTemp);
  Bm.SetTemp(Bm.Temp()-dT);
  double H2 = Bm.totHf(som_ALL, Bm.Temp(), Bm.Press(), Bm.pMArray());
  dThermalLoss = H1-H2;
  Bm.Set_totHf(H2-dExtraDuty);

  // Final Temperature
  T      = Bm.Temp();
  C      = Bm.CausticConc(T);           // g/l
  FC     = Bm.FreeCaustic(T);           // g/l
  C_at25 = Bm.CausticConc(C_2_K(25.0)); // g/l
  //KGA:24/6/2002 added 3 lines below to recalc ratios (C2S, Cl2C, TOOC2C)
  C2S    = Bm.CtoS();                   // ratio
  Cl2C   = Bm.CltoC();                  // ratio
  TOOC2C = Bm.TOC(T)*::SodiumCarbonate.MW/MW_C/GTZ(C); // ratio, TOOC = TOC*106/12
//int ii = Bm.PropertyIndex("FreeCausic");
//double FC_ = (ii>=0 ? Bm.PropertyCount[ii].DValue(MP_All, T, Bm.Press()) : 0.0;
  
  double Na2SO4_2C = Bm.Na2SO4toC(); //ratio

  // Residence time
  ResTimeHrs = TankVol/GTZ(Bm.Volume(som_SL)*3600.0);      // Hours
  
  PoisonFactor = Range(0.0, PoisonFactor, 1.0);

  VLiq = Bm.Volume(som_Liq)*3600.;       // kL/h
  const double R = R_c;//8.31441; //Universal Gas Constant

  switch (iMethod)
    {
    case PM_Original:
      {
      // From BCPREC6.XLS
      SSA = dSSAAdj*(0.0298+0.00077*PercM45*100.0);  // m^2/g
      Sol = MSolids*1000.0/GTZ(VLiq);  // g solids per L liq
      TSA = SSA*Sol;                   // m^2/L

      I=0.0;
  
      K = K0*exp(-ActivationEnergy/T)*TSA*(1.0295-0.0022*C+(0.8109*TOOC2C-0.0043*TOOC2C*C)*PoisonFactor);
      // Above is original equation for K, followed by data generated in Jan 98, followed by data in March 98
      //K = K0*exp(-ActivationEnergy/T)*TSA*(0.9973-0.0020*C+(0.8501*TOOC2C-0.0040*TOOC2C*C)*PoisonFactor)+0.0964*C2S;
      //K = K0*exp(-ActivationEnergy/T)*TSA*(0.1960-0.0013*C+(0.5251*TOOC2C-0.0025*TOOC2C*C)*PoisonFactor+0.5537*C2S);

      // Equilibrium A/C ratio
      ACeq = 0.006797*K_2_C(T) + 0.000983*C - 0.211044*C2S + 0.000154*TOOC2C*C + 0.000991*Cl2C*C - 0.203307;
      Aeq = ACeq*C;

      // Product A/C ratio
      const double ACdiff = Max(0.0, ACin-ACeq);
      ACout = ACeq+(-1.+Sqrt(GEZ(1.+4.*K*ResTimeHrs*ACdiff)))/GTZ(2.*K*ResTimeHrs);
      break;
      }
    case PM_Feb99:
      {
      Sol = MSolids*1000.0/GTZ(VLiq);  // g solids per L liq
      TSA = SSA*Sol;                   // m^2/L
  
      //TOOC = TOC*106/12
      double TOOC=Bm.TOC(C_2_K(25.0))*::SodiumCarbonate.MW/MW_C;     // g/l
      //double TOOC=Bm.TOC(T)*::SodiumCarbonate.MW/MW_C; kga:6/9/01 shouldn't the above be this?

      K = K0*exp(-ActivationEnergy/T)*TSA*C/GTZ(FC*FC)*(1-0.00081*TOOC)*PoisonFactor;

      const double CNa2SO4=Bm.SodiumSulphateConc(T);
      // Equilibrium A/C ratio
      // Ionic Strength
      //I = C*(0.01887+1.5532*Cl2C/58.44+1.3437*(1-C2S)/C2S/105.99+2.2571*CNa2SO4/124.04+1.1997*0.01887*TOOC2C*12/105.99);
	    // Revised by I Frost as per B Garner request, 17/3/99
	    I = C/105.99*(0.01887*105.99+1.5532*Cl2C*2+1.3437*(1-C2S)/C2S+2.2571*CNa2SO4/C+1.1997*0.01887*TOOC2C*12);
      const double I2 = Sqrt(I);
      const double E = -8.7168*I2/(1+I2)+0.7767*I-0.2028*Pow(I, 3./2.);
      const double tmp_ = -30960.0 / (R * T);
      Aeq = 0.96197*C/(1.0+(Pow(10.0, E)/Exps(tmp_)));
      ACeq = Aeq/C;
      //ACeq = 0.006797*K_2_C(T) + 0.000983*C - 0.211044*C2S + 0.000154*TOOC2C*C + 0.000991*Cl2C*C - 0.203307;

      // Product A/C ratio
      const double ACdiff = Max(0.0, ACin-ACeq);
      ACout = ACeq + (-1.+Sqrt(GEZ(1.+4.*K*ResTimeHrs*ACdiff))) / GTZ(2.*K*ResTimeHrs);
      S_out = C/GTZ(C2S);
      break;
      }
    case PM_June01: //new method 26/6/01
    case PM_June01b: //new method 29/6/01
      {
      Sol = MSolids*1000.0/GTZ(VLiq);  // g solids per L liq
      TSA = SSA*Sol;                   // m^2/L

      #ifndef _RELEASE
      const double CNa2SO4 = Bm.SodiumSulphateConc(T);
      const double CNa2SO4_2C = CNa2SO4/C;
      double CNa2SO4_2C_Diff = CNa2SO4_2C - Na2SO4_2C; //todo, check!?
      #endif

      // Ionic Strength
      I = C/105.99 * (0.01887*105.99 + 1.5532*Cl2C*2.0 + 1.3437*(1.0-C2S)/GTZ(C2S) + 2.2571*Na2SO4_2C + 1.1997*0.01887*TOOC2C*12.011);
 
      S_out = C/GTZ(C2S);

      // Equilibrium A/C ratio
      const double a0 = -8.7168;//-9.2082;
      const double a3 = -0.7767;//-0.8743;
      const double a4 = 0.2028;//0.2149;
      const double dG = -30960.0;
      const double I2 = Sqrt(I);
      const double E = a0*I2 / (1.0+I2) - a3*I - a4*Pow(GTZ(I),1.5);
      const double Keq = Exps(dG/(R*T));
      Aeq = 0.96197*C / (1.0 + (Pow(10.0, E) / Keq));
      //kga: Aeq is "same as" Solub function in corr.cpp
      //kga: Aeq is "same as" AluminaConcSat function in qbspmdl.cpp
      ACeq = Aeq/GTZ(C);

      const double a=Exps(-ActivationEnergy/T);
      const double b=Exps(-TOOC2C*C*k_TOC);
      const double c=Pow(GTZ(S_out), n_s);
      const double d=Pow(GTZ(FC), n_fc);
      const double e=Pow(GTZ(ACeq), n_eq);
      //K = K0*exp(-ActivationEnergy/T) * Exps(-TOOC2C*C*k_TOC) * Pow(GTZ(S_out),n_s) * Pow(GTZ(FC),n_fc) * Pow(GTZ(ACeq),n_eq);
      K = K0*a*b*c*d*e;
 
      // Product A/C ratio
      //ACout = ACin - K * ResTimeHrs * Pow(GTZ(ACout-ACeq),n_) * Pow(GTZ(Sol),n_sol) * Pow(GTZ(SSA),n_ssa);
      /*
      // Note this is iterative and can become unstable particularly in tanks with high precipitation rate.
      // Stability can improved by using:
      const double NewACout = ACin - K * ResTimeHrs * Pow(GTZ(ACout-ACeq),n_) * Pow(GTZ(Sol),n_sol) * Pow(GTZ(SSA),n_ssa);
      ACout = Damping*ACout + (1.0-Damping)*NewACout;
      // Damping = 0.9 works pretty well.
      */
      // kga : Damping does not work because it fights with SysCAD Brent solver

      if (iMethod==2)
        {
        ACout = ACin - K * ResTimeHrs * Pow(GTZ(ACout-ACeq),n_) * Pow(GTZ(Sol),n_sol) * Pow(GTZ(SSA),n_ssa);
        }
      else
        {
        const double d1 = K * ResTimeHrs * Pow(GTZ(Sol),n_sol) * Pow(GTZ(SSA),n_ssa);
        //ACout = ACin - d1 * Pow(GTZ(ACout-ACeq),n_);
        const double Tol = 1.0e-8;
        double ACprev;
        for (int Iter=0; Iter<MaxACIter; Iter++)
          {
          ACprev = ACout;
          const double d2 = Pow(GTZ(ACin-ACeq),n_);
          if (ACin - d1 * d2 > ACeq)
            {
            //Should converge by damped direct substitution using the original form of equation;
            ACout = Damping * ACprev + (1.0-Damping) * (ACin - d1 * d2);
            }
          else
            {
            //Re-arrange the above equation to express the RHS ACout in terms of the LHS ACout.
            // This will improve stability in cases where precipitation rate is very high.
            const double dd1 = (ACin - ACprev)/d1;
            const double dd2 = 1.0/GTZ(n_);
            ACout = Damping * ACprev + (1.0-Damping) * (ACeq + Pow(GTZ(dd1),dd2));
            }
          if (fabs(ACout-ACprev)<Tol)
            break;
          }

        if (Iter==MaxACIter)
          {//failed to converge...
          //ACout=?
          }
        }

      #ifndef _RELEASE
      //Check that FC : free caustic at tank outlet = C * (1.0 - 1.0395*A/C)
      const double FC2 = C * (1.0 - 1.0395*ACout);
      const double FC_Diff = FC-FC2; //sanity check
      #endif
      break;
      }
    case PM_May03: //new method 15/5/03
    case PM_May03b: //new method 15/5/03
      {
      /*From John McFeaters May 2003:
      What I would like to do is develop a precip equations for the Kaiser model that:
      (1)	uses the same relation for gibbsite equilibrium as that selected for elsewhere in the model.
      (2)	Uses the second order closed form dependence (we don't have enough data to fit too many constants and using the second order assumption I think is reasonably OK)
      (3)	Has adjustable FC and C/S terms in them of the form (1/FC)^a and (C/S)^b
      (4)	Has a simplified organics term like the case 1

      So something like this:

      As before:
        SSA = dSSAAdj * (0.0298+0.00077*PercM45*100.0)  [m^2/g]
        TSA = SSA * MSolids * 1000. / (VLiq)            [m^2/L]

      The K relation like case 1 but with the poison factor operating only on the TOOC term plus a free caustic and soda term

        K = K0 * exp(-Ea/T) * TSA * (1 - 0.00081*TOOC*PoisonFactor) * (1/FC)^a * (1/S)^b

        ACout = ACeq + (-1. + Sqrt(1. + 4.*K*ResTimeHrs*(ACin-ACeq))) / (2.*K*ResTimeHrs)

      We might also want to look at a variation with C/S instead of soda. I think the biggest thing is to get the right A/c equilibrium equation in there.
      */
      SSA = dSSAAdj*(0.0298+0.00077*PercM45*100.0);  // m^2/g
      Sol = MSolids*1000.0/GTZ(VLiq);  // g solids per L liq
      TSA = SSA*Sol;                   // m^2/L

      S_out = C/GTZ(C2S);
      //TOOC = TOC*106/12
      double TOOC=Bm.TOC(C_2_K(25.0))*::SodiumCarbonate.MW/MW_C;     // g/l

      K = K0 * exp(-ActivationEnergy/T) * TSA * (1.0 - 0.00081*TOOC*PoisonFactor) * Pow(1.0/GTZ(FC), n_fc2) * Pow(1.0/GTZ(S_out), n_s2);
                       
double aeq=Bm.AluminaConcSat(T);      
double aeq25=Bm.AluminaConcSat(C2K(25.));      
      // Calc AEq:
      const double a0 = -9.11181236102574;
      const double a3 = -0.902799187095254;
      const double a4 = 0.239197078637227;
      const double k1 = 0.86415865213305; //1.72831730426610/2
      const double k2 = 0.61809588818785;
      const double k3 = 1.70971468759441; //1.70971468759441/MW_Na2CO3*MW_Na2SO4*0.746172816
      const double k4 = 1.4303348027741; //2.86068025913951/0.01887/MW_Na2CO3
      const double dG = -30960.0;

      const double cNaOH   = C;
      const double cNaCl   = Cl2C*C;
      const double cNa2CO3 = (1.0-C2S)/GTZ(C2S)*C; //C2S = C/(C+CO3)  => CO3 = C*(1-C2S)/C2S
      const double cNa2SO4 = Na2SO4_2C*C;
      const double cTOC    = TOOC2C*12.011/105.989*C; //TOOC = TOC*106/12
      // Ionic Strength
      I = 0.01887*cNaOH + k1*cNaCl/58.4425 + k2*cNa2CO3/105.989 + k3*cNa2SO4/142.043 + k4*0.01887*cTOC;
    
      // Equilibrium A/C ratio
      const double I2  = Sqrt(I);
      const double E   = a0*I2 / (1.0+I2) - a3*I - a4*Pow(GTZ(I),1.5);
      const double Keq = Exps(dG/(R*T));
      Aeq = 0.96197*C / (1.0 + (Pow(10.0, E) / Keq));
      //kga: Aeq is "same as" Kaiser AluminaConcSat function in qbspmdl.cpp
      ACeq = Aeq/GTZ(C); 
      if (iMethod==PM_May03b)
        ACeq = Bm.AluminaConcSat(T);

      // Product A/C ratio
      const double ACdiff = Max(0.0, ACin-ACeq);
      ACout = ACeq + (-1. + Sqrt(GEZ(1. + 4.*K*ResTimeHrs*ACdiff))) / GTZ(2.*K*ResTimeHrs);
      break;
      }
    }
  
  return Bm.AtoC()-ACout;
  };

//--------------------------------------------------------------------------

void QPrecipD::EvalProducts(CNodeEvalIndex & NEI)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      {
      const int IOFeed = IOWithId_Self(ioid_Feed);
      const int IOProd = IOWithId_Self(ioid_Product);
      const int IOSecFeed = IOWithId_Self(ioid_SecFeed);
      const int IOSecProd = IOWithId_Self(ioid_SecProduct);
      ASSERT(IOProd>=0 && IOFeed>=0);
      SpConduit & Qp=*IOConduit(IOProd);

      //Qp.SetModel(&SMBayerBaseClass);
      //Qp.SetModel(&QSMBayerClass);
      Qp.QZero();
      if (bOnLine)
        {
        SigmaQInPMin(Qp, som_ALL, Id_2_Mask(ioid_Feed));
        }
      else
        {
        SigmaQInPMin(Qp, som_ALL, Id_2_Mask(ioid_Feed)|Id_2_Mask(ioid_SecFeed));
        return;
        }

      SMBayerB * pBm = dynamic_cast<SMBayerB*>(Qp.Model());
      SetCI(8, pBm==NULL);
      if (pBm)
        {
        SMBayerB & Bm=*pBm;

        dExtraDuty = 0.0;
        StkSpConduit Qsf("Qsf", chLINEID(), this);
        Qsf().QZero();
        if (IOSecFeed>=0)
          {
          SigmaQInPMin(Qsf(), som_ALL, Id_2_Mask(ioid_SecFeed));
          if (Valid(dPrevTout))
            {
            const double sfH0 = Qsf().totHf();
            Qsf().SetTemp(Range(Qsf().Temp(), dPrevTout, dPrevTout*1.1));
            const double sfH1 = Qsf().totHf();
            dExtraDuty = sfH1-sfH0;
            }
          }

        RB.EvalProducts(Qp);
        //EHX.EvalProducts(Qp);

        Damping = Range(0.0, Damping, 1.0);
        MaxACIter = Range((byte)1, MaxACIter, (byte)1000);
        PrecRateDamping = Range(0.0, PrecRateDamping, 1.0);
        dMaxT = Range(C_2_K(65.0), dMaxT, C_2_K(130.0));
        dMinC = Range(50.0, dMinC, 300.0);
        TankVol = Max(1.0, TankVol);

        const double SolConc0 = Bm.SolidsConc(C_2_K(25.0));
        const double SolQmIn  = Qp.QMass(som_Sol);

        dThermalLoss    = 0.0;
        ACoutFnd ACF(Qp, &dThermalLoss, &dExtraDuty, &dReactionHeat, &ACin, &ACeq, &ACout);
        ACF.VIn         = Qp.QVolume(som_SL)*3600.0;          // kL/h
        ACF.VLiq        = Qp.QVolume(som_Liq)*3600.0;         // kL/h
        ACF.MSolids     = Qp.VMass[Gibbsite.SolPhInx()]*3600.0/1000.0; // t/h
        ACF.T           = Qp.Temp();
        ACF.ThermalConst= ThermalConst;
        ACF.AmbientTemp = AmbientTemp;
        ACF.TankVol     = TankVol;

        // Residence Time (Initial Guess)
        ACF.ResTimeHrs  = TankVol/GTZ(Bm.Volume(som_SL)*3600.0);       // Hours
        ACF.ActivationEnergy = ActivationEnergy;
        ACF.PercM45     = PercM45;
        ACF.bWithHOR    = bWithHOR;
        ACF.iMethod     = iMethod;
        ACF.SSA         = SSA;
        ACF.TSA         = TSA;
        ACF.dSSAAdj     = dSSAAdj;
      
        ACF.n_s2  = n_s2;
        ACF.n_fc2 = n_fc2;
        ACF.k_TOC = k_TOC;
        ACF.n_s   = n_s;
        ACF.n_fc  = n_fc;
        ACF.n_eq  = n_eq;
        ACF.n_    = n_;
        ACF.n_sol = n_sol;
        ACF.n_ssa = n_ssa;
        ACF.Damping = Damping;
        ACF.MaxACIter = MaxACIter;
        ACF.Sol = 0.0;
        ACF.S_out = 0.0;

        if (ACF.MSolids>1.0e-3)
          {
          ACF.C_at25 = Bm.CausticConc(C_2_K(25.0)); // g/l
          ACF.C      = Bm.CausticConc(ACF.T);       // g/l
          ACF.FC     = Bm.FreeCaustic(ACF.T);       // g/l
          ACF.C2S    = Bm.CtoS();                   // ratio
          ACF.Cl2C   = Bm.CltoC();                  // ratio
          ACF.TOOC2C = Bm.TOC(ACF.T)*::SodiumCarbonate.MW/MW_C/GTZ(ACF.C); // ratio, TOOC = TOC*106/12
          ACF.ACin   = Bm.AtoC();                   // ratio

          // Check Ranges - issue warning if outside range (this is on input only, not during convergence!)
          SetCI(1, !InRange(C_2_K(60.0), ACF.T, C_2_K(80.0)));//dMaxT));
          SetCI(2, !InRange(200., ACF.C, 500.0));
          SetCI(3, !InRange(0.73, ACF.C2S, 0.98));
          //SetCI(4, !InRange(0.02, ACF.Cl2C, 0.050));
          SetCI(4, !InRange(0.005, ACF.Cl2C, 0.070));//kga changed 7/7/04
          SetCI(5, !InRange(0.01, ACF.TOOC2C, 1.39));

          //ACF.T      = Range(C_2_K(60.0), ACF.T, C_2_K(80.0));
          ACF.T      = Range(C_2_K(60.0), ACF.T, dMaxT); //kga changed 27/8/01
          //ACF.C      = Range(200., ACF.C, 500.0);
          ACF.C      = Range(dMinC, ACF.C, 500.0); //kga changed 30/8/01
          ACF.C_at25 = Range(dMinC, ACF.C_at25, 500.0);
          ACF.C2S    = Range(0.73, ACF.C2S, 0.98);
          //ACF.Cl2C   = Range(0.02, ACF.Cl2C, 0.050);  // MJM 24/6/96
          ACF.Cl2C   = Range(0.005, ACF.Cl2C, 0.070);  // MJM 24/6/96
          ACF.TOOC2C = Range(0.01, ACF.TOOC2C, 1.39);  // MJM 24/6/96
        
          // Kinetic Rate constant
          ACF.K0=K0;
          ACF.PoisonFactor=PoisonFactor;
      
          // now converge the amount precipitated/dissolved to give an A/C of ACout
          // Reaction: 1 SodiumAluminate ==> 1 CausticSoda + 1 Gibbsite
      
          double MaxMolesPrec=Bm.VMole[SodiumAluminate.LiqPhInx()]; 
          double MaxMolesDiss=Min(Bm.VMole[Gibbsite.SolPhInx()], Bm.VMole[CausticSoda.LiqPhInx()]);

          double PrevPrecipRate = dPrecipRate;
          ACF.SetTarget(0.0);
          if (Valid(dPrecipRate))// && (dPrecipRate<-MaxMolesDiss || dPrecipRate>MaxMolesPrec))
            {
            ACF.Damping = Damping;
            dPrecipRate = Range(-MaxMolesDiss, dPrecipRate, MaxMolesPrec); //prev value out of new range!!!
            ACF.SetEstimate(dPrecipRate, 1.0);
            dPrecipRate=dNAN;
            }
          else
            ACF.Damping = 0.0; //assume previous value of ACout is BAD

          SetCI(6, False);
          flag Ok = false;
          int iRet = ACF.Start(-MaxMolesDiss, MaxMolesPrec);
          if (iRet==RF_EstimateOK) //estimate is good, solve not required
            {
            Ok = true;
            }
          else
            {
            if (iRet==RF_BadEstimate)
              iRet = ACF.Start(-MaxMolesDiss, MaxMolesPrec); // Restart
            if (iRet==RF_OK)
              {
              if (ACF.Solve_Brent()==RF_OK)
                {
                Ok = true;
                }
              }
            else
              SetCI(6, True);
            }

          if (Ok)
            {
            dPrecipRate=ACF.Result();
            SSA = ACF.SSA;
            TSA = ACF.TSA;
            IonicStrength = ACF.I;
            Aeq = ACF.Aeq;
            SetCI(7, False);
            }
          else
            SetCI(7, True);

          // Yield
          Yield = ACF.C*(ACF.ACin-ACF.ACout);
      
          // Bound Soda
          //double k1xOld = dK1*(0.000598*ACF.C - 0.00036*K_2_C(ACF.T) + 0.019568*ACF.TOOC2C)*(1.0 - 0.758*ACF.C2S);
          //double BoundSodaOld = k1xOld*Sqr(ACF.ACout-ACF.ACeq);
          //kga 20/6/2002 : changed bound soda calc to use C at 25dC as advised by Brett Garner (based on example PGM)
          double k1x = dK1 * (0.000598*ACF.C_at25 - 0.00036*K_2_C(ACF.T) + 0.019568*ACF.TOOC2C) * (1.0 - 0.758*ACF.C2S);
          BoundSoda = k1x * Sqr(ACF.ACout-ACF.ACeq);
          //kga: BoundSoda is "same as" SodaC function in corr.cpp
          //kga: BoundSoda is "same as" BoundSodaSat function in qbspmdl.cpp

          if (Valid(dPrecipRate) && Valid(PrevPrecipRate))
            dPrecipRate = PrecRateDamping*PrevPrecipRate + (1.0-PrecRateDamping)*dPrecipRate;

          if (fBoundSpcsOK && fDoBoundStuff && Valid(dPrecipRate))
            {
            const double GibbsRate = dPrecipRate*Gibbsite.MW;
            if (GibbsRate>=0.0)
              {
              #define NewBSodaCalc 1
              #if NewBSodaCalc
              //kga,mxh 8/7/2002 : changed two lines above to these:
              //BndSoda = BoundSoda*(GibbsRate*101.96/(78*2))*(1.0-dBndOrgSoda)*((2*40)/62)  
              //;convert 2 Al[OH]3 to Al2O3 and 2 NaOH to Na2O, also, dBndOrgSoda is wrong way round.
              //BndOrgSoda = BoundSoda*(GibbsRate*101.96/(78*2))*(dBndOrgSoda)*(223.63/62)
              //;convert 2 Al[OH]3 to Al2O3 and Na2C5.2O7.2 to Na2O.
              
              double BndSoda    = BoundSoda*(GibbsRate*::InertAlumina.MW/(2.0*78/*::THA.MW*/))*(1.0-dBndOrgSoda)*((2.0*::CausticSoda.MW)/61.9789); //convert 2 Al[OH]3 to Al2O3 and 2 NaOH to Na2O
              double BndOrgSoda = BoundSoda*(GibbsRate*::InertAlumina.MW/(2.0*78/*::THA.MW*/))*(dBndOrgSoda)*(::OrgSodaL.MW/61.9789); //convert 2 Al[OH]3 to Al2O3 and Na2C5.2O7.2 to Na2O
              #else
              double BndSoda    = BoundSoda*GibbsRate*dBndOrgSoda;
              double BndOrgSoda = BoundSoda*GibbsRate*(1.0-dBndOrgSoda);
              #endif

              Bm.m_M.VValue[OrgSodaS.SolPhInx()] += BndOrgSoda;
              Bm.m_M.VValue[CausticS.SolPhInx()] += BndSoda;
              Bm.m_M.VValue[OrgSodaL.LiqPhInx()] -= BndOrgSoda;
              Bm.m_M.VValue[CausticL.LiqPhInx()] -= BndSoda;
              }
            else
              {
              }
            int xxx=0;
            }

#if WithSzStuff
          // Size Dist Stuff
          SQSzDist1 *pQ=SQSzDist1::FindQual(Qp.Model());
          CSD_Distribution *pD=pQ ? pQ->FindDistributionFor(Gibbsite.SolPhInx()) : NULL;
          CSD_SpDist *pSpD=pQ ? pQ->FindSpDistributionFor(Gibbsite.SolPhInx()) : NULL;
          if (pSpD)
            {
            SQSzDist1 &Q=*pQ;
            CSD_Distribution &D=*pD;
            CSD_SpDist &SpD=*pSpD;

            double SolConc1=Bm.SolidsConc(C_2_K(25.0));

            int n=D.NIntervals();
              // Adjust Distribution as required
            for (int i=0; i<n; i++)
              {
              switch (Fn[i].iType)
                {
                case PF_Ignore:
                  break;
                case PF_ConstG:
                  SpD.FracPass[i]+=SpD.FracPass[i]*(Fn[i].C);
                  break;
                case PF_ConstD:
                  SpD.FracPass[i]-=SpD.FracPass[i]*(Fn[i].C);
                  break;
                case PF_LinearG:
                  SpD.FracPass[i]+=SpD.FracPass[i]*(Fn[i].A*SolConc1+Fn[i].B);
                  break;
                case PF_LinearD:
                  SpD.FracPass[i]-=SpD.FracPass[i]*(Fn[i].A*SolConc1+Fn[i].B);
                  break;
                }
              }

            SpD.FracPass.Normalise();
            int xxx=0;
  //          Qu.QSetM(Qm(), som_ALL, 0.0, Std_P);
  //          Qo.QSetM(Qm(), som_ALL, 0.0, Std_P);
  //
  //          SQSzDistFractionInfo Info(PartCrv, 0); 
  //          SQSzDist1::SplitSolids(Info, Qm(), Qo, Qu);
  //
  //          double Msu=Info.m_CoarseMass;
  //          double Mlu=Range(0.0, RhoL*Msu*(1./ConcUs-1./RhoS), Ml);
  //          Qu.QAddM(Qm(), som_Liq, Mlu);
  //          Qo.QAddM(Qm(), som_Liq, GEZ(Ml-Mlu));
  //
  //          ClrCI(1);
  //          break;
            }
#endif
        
          }
        else
          {
          SSA=0.0;
          TSA=0.0;
          Yield=0.0;
          BoundSoda=0.0;
          IonicStrength=0.0;
          Aeq=0.0;
          }

        if (!Valid(dPrevTout))
          dPrevTout = Qp.Temp();
        else
          {
          double T = Qp.Temp();
          dPrevTout = (0.2*T) + (0.8*dPrevTout);//damping
          }

        dSolPrecip = Qp.QMass(som_Sol) - SolQmIn;

        Qp.QAddF(Qsf(), som_ALL, 1.0); //add secondary feed
        if (IOSecProd>=0 && iSecSplitMethod!=SSM_None)
          {//split to secondary side outlet
          SpConduit & Qs=*IOConduit(IOSecProd);
          double QmTot = Qp.QMass(som_ALL);
          double Frac=0.0;
          switch (iSecSplitMethod)
            {
            case SSM_Frac: Frac = Range(0.0, SplitRqd, 1.0); break;
            case SSM_Qm: Frac = Range(0.0, SplitRqd/GTZ(QmTot), 1.0); break;
            case SSM_Qv: Frac = Range(0.0, SplitRqd/GTZ(Qp.QVolume(som_ALL)), 1.0); break;
            }
          Qs.QSetM(Qp, som_ALL, QmTot*Frac, Std_P);
          Qp.QAdjustQmTo(som_ALL, QmTot*(1.0-Frac));
          }

        Sol = ACF.Sol;
        S_out = ACF.S_out;
        ResidenceTime=ACF.ResTimeHrs*3600.0;            // Secs
        }
      else
        MN_Surge::EvalProducts(NEI);
      break;
      }
    default:
      MN_Surge::EvalProducts(NEI);
    }
  }
    
//--------------------------------------------------------------------------

void QPrecipD::ClosureInfo() 
  { 
  if (NoFlwIOs()>0)
    {
    MN_Surge::ClosureInfo();
    if (m_Closure.DoFlows())
      {
      CClosureInfo &CI=m_Closure[0];
      if (bOnLine)
        {
        CI.m_PowerIn+=-dThermalLoss;
        CI.m_PowerIn+=dExtraDuty;
        if (EHX.Enabled())
          CI.m_EHXPowerIn += EHX.HeatFlow();
        }
      }
    }
  };

//--------------------------------------------------------------------------

flag QPrecipD::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tTemperature out of Range";          return 1;
    case 2: pS="W\tCaustic Conc out of Range";         return 1;
    case 3: pS="W\tCarbon to Sulphur out of Range";    return 1;
    case 4: pS="W\tChlorine to Carbon out of Range";   return 1;
    case 5: pS="W\tTOOC to Carbon out of Range";       return 1;
    case 6: pS="W\tA/C Convergence not Starting";      return 1;
    case 7: pS="W\tA/C not Converging";                return 1;
    case 8: pS="W\tBad Feed Stream - Not Bayer Model"; return 1;
    default:                                               
      return MN_Surge::CIStrng(No, pS);
    }
  };

//==========================================================================
//
//
//
//==========================================================================
