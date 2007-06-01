//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ALCANBAYERSPMDL_H
#define  __ALCANBAYERSPMDL_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif  
#ifndef __SP_MODEL_H
  #include <sp_model.h>
#endif  

#define MDLBASE
#define SIZEDST1
#include "models.h"

#ifndef __ALSPMDLB_H
  #include <alspmdlb.h>
#endif  
 
#ifdef __ALCANBAYERSPMDL_CPP
  #define DllImportExport DllExport
#elif !defined(AlcanSpMdl)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// Props need to be added back in when the SMDK Bayer Props model is working properly!
//NB: Must include this because it is referenced by SMDK!!!
#define WithProps 1

// ===========================================================================
//
//                          Alcan Bayer Specie Model
//
// ===========================================================================

const long    MaxHPSizeClasses=100;

//TODO HPDSize & HPSMin must come from Distribution
const double  HPDSize=4;
const double  HPSMin=0;
//TODO What is HPMStep
const long    HPMStep=10;

enum eAlcanPSDTypes { eAlcanPSD_Log, eAlcanPSD_Rosin, eAlcanPSD_Table, eAlcanPSD_File};

class DllImportExport ASMSzPersonality: public SpQPersonality 
  {
  protected:
    SQSzDist1 &m_Q;

    eAlcanPSDTypes m_eType;

    struct
      {
      double   m_dMedian;
      double   m_dQS;    
      double   m_dm149;  
      double   m_dm106;  
      double   m_dm74;   
      double   m_dm53;   
      double   m_dm45;   
      double   m_dm33;   
      double   m_dm20;   
      double   m_dq1;    
      double   m_dq3;    
      double   m_dd10;   
      double   m_dd90;
      double   m_dCrit;
      } m_Rqd;//, m_Meas;

  public:
    ASMSzPersonality(SQSzDist1 *pQ);
    virtual void  BuildDataDefn(DataDefnBlk & DDB);
    virtual flag  DataXchg(DataChangeBlk & DCB);

    void          SetDistribution();
    //void          ComputeMedian(long NIntervals, double SizeClass[], double wCumul[]);

    double        NormInv(double x);
    double        NormDist(double x);
    double        NormDens(double x);

    void          RegressXY (const long Count, const double x[], const double y[], double RegRslt[]);
    void          RegressLN (const long Count, const double Diam[], const double WPrct[], double RegRslt[]);
    void          RegressRR (const long Count, const double Diam[], const double WPrct[], double RegRslt[]);
    void          HyprodDataFromLN (double RegRslt[]);
    void          HyprodDataFromRR (double RegRslt[]);
    void          HyprodDataFromTable(long Count, double Diam[], double WPrct[], double RegRslt[]);
    void          HyprodDist(long NIntervals, double SizeClass[], double wCumul[]);

  };

// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_SPMODEL(ASMBayer)

class DllImportExport CConcentrations
  {
  public:
    static flag     NaFactorOK;
    static SpVector NaFactor;

    ASMBayer      *pBayerMdl;
    CArray<double, double&> Liq; //array of concentrations at 25C as Na2CO3 Equiv

    double         Density25;

    CConcentrations(ASMBayer *pMdl);
    void Zero();
    flag Converge(CSysVector & MA);
    double LTotalSodium(CSysVector & MA);
    double LiquorDensity(double T_, CSysVector & MA);
    static double LiquorDensEqn1(double Tc, double WTNa, double WAlumina);
    static double LiquorDensEqn2(double Tc, double XNaC, double XNa, double XAl);
    static double LiquorDensEqn3(double Tc, double XAl2O3, double XCaust, double XTTS, double XOC);
  };

// ===========================================================================

class DllImportExport ASMBayer : public SMBayerBase
  {
  DEFINE_SPARES(ASMBayer)
  friend class CConcentrations;

  protected:
    CConcentrations LiqConcs25;

    //globals...
    static double dBPEFactor;
    static double dRhoFactor;
    static double sm_dMinSolCp;
    static byte sm_iASatMethod;
    static byte sm_iDensityMethod;
    static byte sm_iGrowthMethod;
    static byte sm_iRqdCalcMethod;
    static byte sm_iCpMethod;
    static byte sm_iBPEMethod;
    static double sm_Morphology;
    static double sm_Nucleation;
    static double sm_Na2OFactor;
    static double sm_CarbonFactor;

  public:
    bool       fDoCalc, fWasDoCalc;
    double     dRqd_AtoC, dRqd_C, dRqd_CtoS;
    double     dRqd_Sul, dRqd_Sil, dRqd_Org, dRqd_OrgRatio, dRqd_Salt;
    double     dRqd_P2O5, dRqd_Fl;
    double     dRqd_Ox, dRqd_TOC;
    double     dRqd_SolFrac, dRqd_SolConc;
    double     dRqd_SolOx, dRqd_SolOccSoda;
    static     CArray <int, int> LclParm;

    ASMBayer(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~ASMBayer();

    virtual bool   IsBaseClassOf(pSpModel pOther) 
      { return (dynamic_cast<ASMBayer*>(pOther)) != NULL; };

    virtual void   BuildDataDefn_Vars(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    // ask Spmodel for the personality
    virtual SpQPersonality *GetQualityPersonality(SpQuality* pQ) 
      {
      SQSzDist1* pSQ=dynamic_cast<SQSzDist1*>(pQ);
      if (pSQ)
        return new ASMSzPersonality(pSQ);
      return NULL;
      }; 

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
    double         IonicStrengthBoehmite();
    double         AluminaSolubility(double T_) { return AluminaConcSat(T_); };
    double         OxalateSolubility(double T_);
    double         SulphateSolubility(double T_) { return 0.0; };
    double         CarbonateSolubility(double T_) { return 0.0; };
    double         BoehmiteSolubility(double T_);
    double         AtoCSaturation(double T_);
    double         SSNRatio(double T_);
    double         AluminaSSN(double T_);
    //double         PerformAluminaSolubility(double TRqd, double ARqd, double THARqd, double NoPerSec, double Na2OFac, double SSat, flag & ConvergeErr);
    double         TOOC(double T_);
    double         TOC(double T_);
    double         FreeCaustic(double T_);
    double         AtoC();
    double         CtoS();
    double         CltoC();
    double         TOOCtoC();
    double         Na2SO4toC();
    double         Na2CO3toS();
    double         MR();
    double         AA(); //Total Na conc as Na2O equivilant
    double         TT(); //Caustic conc as Na2O equivilant

    //Jan 2002 : new virtuals for PSD...
    //double         SodiumFlourideConc(double T_) { ASSERT_ALWAYS(true, "TODO:SodiumFlourideConc", __FILE__, __LINE__); return 0.0; };
    //double         QuartzConc(double T_) { ASSERT_ALWAYS(true, "TODO:QuartzConc", __FILE__, __LINE__); return 0.0; };
    //double         HeamatiteConc(double T_) { ASSERT_ALWAYS(true, "TODO:HeamatiteConc", __FILE__, __LINE__); return 0.0; };

    double         BoundSodaFrac() { ASSERT_ALWAYS(true, "TODO:BoundSodaFrac", __FILE__, __LINE__); return 0.0; };
    double         BoundSodaOrgFrac() { ASSERT_ALWAYS(true, "TODO:BoundSodaOrgFrac", __FILE__, __LINE__); return 0.0; };

    //double         OC(double T_) { ASSERT_ALWAYS(true, "TODO:OX", __FILE__, __LINE__); return 0.0; };
    //double         OX(double T_) { ASSERT_ALWAYS(true, "TODO:OC", __FILE__, __LINE__); return 0.0; };
    //void           DumpState() { ASSERT_ALWAYS(true, "TODO:DumpState", __FILE__, __LINE__); };
    //... end for new virtuals for PSD

    double         LVolume25();
    double         SLVolume25();
    double         LDensity25();
    double         SLDensity25();
    double         TotalSodiumConc();
    double         OrganateConc();
    double         OrganicO11Conc();
    double         TotalOrganics();
    double         ChlorineConc();
    double         SulphateConc();
    double         SodiumOxalate_Conc();
    double         OrganicO11_Conc();
    double         Sulphate_Conc();
    double         TOOC_(double T_);
    double         Chlorine_Conc();
    double         Fluoride_Conc();
    double         Silicate_Conc();
    double         SPO();
    double         Na2OSolFrac();

    double         TotalSodium(CSysVector * pMA=NULL);
    double         BoilPtElev(double T_, CSysVector * pMA=NULL);

    virtual DDEF_Flags FixSpEditFlags(DDEF_Flags Flags, int iSpid, flag AsParms);
    virtual double RefTemp() { return C_2_K(0.0); };
    virtual double msCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);
    virtual double msHm(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);
    virtual LPTSTR FlashDescription()  { return "Model"; };
    virtual double SaturationP(double T_, CSysVector * pMA=NULL, CSaturationDefn * pSatDefn=NULL);
    virtual double SaturationT(double P_, CSysVector * pMA=NULL, CSaturationDefn * pSatDefn=NULL);
    virtual double Rho(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
    virtual double SpecieConc(double T_, int iSpNo, PhMask PhaseM);
    virtual double PhaseConc(double T_, PhMask Ph, PhMask PhaseM=som_ALL);
    virtual double BoilingPtElevation(double P_, CSysVector * pMA=NULL, CSaturationDefn * pSatDefn=NULL);
    virtual double DynamicViscosity(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return SMBayerBase::DynamicViscosity(Phase, T_, P_, pMA); };
    virtual double ThermalConductivity(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return SMBayerBase::ThermalConductivity(Phase, T_, P_, pMA); };

    virtual double GrowthRate();
    virtual double AgglomerationRate();
    virtual double NucleationRate(double SpecSurface);

    static double Na2OFactor() { return sm_Na2OFactor; };

    QUALITY_TYPECAST()

#if WithProps
      // CPropertyBase virtuals 
    virtual LPCTSTR DefinedPropertyMapName()                              { return ClassId(); }
    virtual long    DefinedPropertyCount();
    virtual long    DefinedPropertyInfo(long Index, MPropertyInfo & Info);

    virtual DWORD   GetPropertyVisibility(long Index);
    virtual void    GetPropertyValue(long Index, PhMask Phase, double T, double P, MPropertyValue & Value);
    virtual void    PutPropertyValue(long Index, MPropertyValue & Value);
    static  void    GetPropertyValueGlobal(long Index, ULONG Phase, double T, double P, MPropertyValue & Value) {};
    static  void    PutPropertyValueGlobal(long Index, MPropertyValue & Value) {};
#endif

  protected:
    void           DoInputCalcs();
    double         LiqCpCalc(CSysVector & Mn, double FLiq, double T);
    
  public:
    // ConditionBlk Override
    DEFINE_CI(ASMBayer, SMBayerBase, 6);
  };

// ==========================================================================
//
//                                  End
//
// ==========================================================================

#undef DllImportExport

#endif
