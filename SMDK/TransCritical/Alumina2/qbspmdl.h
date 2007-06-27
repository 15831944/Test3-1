//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef  __QBSPMDL_H
#define  __QBSPMDL_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif  
#ifndef __SP_MODEL_H
  #include <sp_model.h>
#endif  

#define MDLBASE
//#define BASIC1
#include "models.h"

#ifndef __ALSPMDLB_H
  #include <alspmdlb.h>
#endif  
 
#ifdef __QBSPMDL_CPP
  #define DllImportExport DllExport
#elif !defined(QALSPMDL)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//                           QAL Bayer Specie Model
//
// ===========================================================================

DEFINE_SPMODEL(QSMBayer)

typedef SMBayerBase SMBayerB;

class DllImportExport CQConcentrations
  {
  public:
    static flag     NaFactorOK;
    static SpVector NaFactor;

    QSMBayer      *pBayerMdl;
    CArray<double, double&> Liq;

    double         Density;
    double         Alumina;
    double         CausticSoda;

    CQConcentrations(QSMBayer *pMdl);
    void Zero();
    flag Converge(CSysVector & MA);
    //double LTotalSodium(CSysVector & MA);
    double LiquorDensity(double T_);
  };

// ===========================================================================

class DllImportExport QSMBayer : public SMBayerBase
  {
  DEFINE_SPARES(QSMBayer)

  public:
    static double dK1_BoundSodaSat;
    static double dGrowth_k_TOC; //Rate term for TOC effect on rate - current value is 0.0023
    static double dGrowth_n_s;   //exponent for soda-dependence of rate constant, current value is -1
    static double dGrowth_n_fc;  //exponent for free-caustic-dependence of rate constant, current value is -2
    static double dGrowth_n_eq;  //exponent for equilibrium-A/C-dependence of rate constant, current value is 0
    static double dGrowth_n_;    //exponent for ??
    static double dGrowth_n_sol; //exponent for solids-dependence of precipitation rate, current value is 1
    static double dGrowth_n_ssa; //exponent for SSA-dependence of precipitation rate, current value is 0.6
  protected:
    static double sm_dBPEFactor;
    static double sm_dRhoFactor;
    static double sm_dMinSolCp;
    static byte sm_iSatTEqn;
    static byte sm_iQBMVer;
    static byte sm_iASatMethod;

    CQConcentrations LiqConcs25;

    flag			 fDoCalc;
	  double		 dRqd_AtoC, dRqd_C, dRqd_CtoS;
	  //double		 dRqd_Sul, dRqd_Sil, dRqd_Org, dRqd_OrgRatio, dRqd_Salt;
	  double		 dRqd_Solids;
    static		 CArray <int, int> LclParm;

  public:
    QSMBayer(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~QSMBayer();

    virtual bool   IsBaseClassOf(pSpModel pOther) 
      { return (dynamic_cast<QSMBayer*>(pOther)) != NULL; };

    virtual void   BuildDataDefn_Vars(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    double         THAMassFlow();
    double         THADens(double T_);
    double         CausticConc(double T_);
    double         AluminaConc(double T_);
    double         SodaConc(double T_);
    double         SodiumCarbonateConc(double T_);
    double         SodiumSulphateConc(double T_);
    double         SodiumOxalateConc(double T_);
    double         SolidsConc(double T_);
    double         IonicStrength();
    double         AluminaConcSat(double T_);
    double         OxalateEquilibrium(double T_);
    double         BoundSodaSat(double T_);
    double         AtoCSaturation(double T_);
    double         SSNRatio(double T_);
    double         AluminaSSN(double T_);
    //double         PerformAluminaSolubility(double TRqd, double ARqd, double THARqd, double NoPerSec, double Na2OFac, double SSat, flag & ConvergeErr);

    // for PSD Precip
    double         OX(double T_);
    double         OC(double T_);
    void           DumpState();

    //Jan 2002 : new virtuals for PSD...
    double         SodiumFlourideConc(double T_);
    double         QuartzConc(double T_);
    double         HeamatiteConc(double T_);

    double         BoundSodaFrac();
    double         BoundSodaOrgFrac();
    //... end for new virtuals for PSD

    double         TOOC(double T_);
    double         TOC(double T_);
    double         AtoC();
    double         CtoS();
    double         CltoC();
    double         DigAtoCEquil();
    double         TOOCtoC();
    double         Na2SO4toC();
    double         FreeCaustic(double T_);

    double         LVolume25();
    double         SLVolume25();
    //double         TotalSodium25() { return LTotalSodium25(); };
    double         OrganateConc();
    double         TotalOrganics();
    double         ChlorineConc();
    double         SulphateConc();
    
    double         TotalNaOnCS(CSysVector * pMA=NULL);
    double         LTotalSodiumConc(CSysVector * pMA=NULL);
  
    //double         TotalSodium(CSysVector * pMA=NULL);
    //double         BoilPtElev(double T_, CSysVector * pMA=NULL);

    virtual DDEF_Flags FixSpEditFlags(DDEF_Flags Flags, int iSpid, flag AsParms);
	  virtual double RefTemp() { return C_2_K(0.0); };
    virtual double msCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);
    virtual double msHm(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);
    virtual double SaturationP(double T_, CSysVector * pMA=NULL);
    virtual double SaturationT(double P_, CSysVector * pMA=NULL);
    virtual double Rho(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
    virtual double SpecieConc(double T_, int iSpNo, PhMask PhaseM);
	  virtual double PhaseConc(double T_, PhMask Ph, PhMask PhaseM=som_ALL);
    virtual double BoilingPtElevation(double P_, CSysVector * pMA=NULL);
    virtual double DynamicViscosity(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
    virtual double ThermalConductivity(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);

  
  protected:
    void           DoInputCalcs();
    
  public:
    // ConditionBlk Override
    DEFINE_CI(QSMBayer, SMBayerBase, 8);
  };

// ==========================================================================
//
//                                  End
//
// ==========================================================================

#undef DllImportExport

#endif
