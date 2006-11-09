//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __BayerSM_H_
#define __BayerSM_H_
#include "md_headers.h"
#include <vector>

class Bayer; // forward declare

//===========================================================================

// A helper class to assist with calculating iterative concentration and density calculations.
class CBayerConcs
{
 public:
  static bool     NaFactorOK;
  static MArray   NaFactor;
  
  Bayer    *pBayerMdl;
  MArray          Liq; //array of concentrations at 25C as Na2CO3 Equiv
  
  double          Density25;    // Density @ 25 C
  double denOld;    // Start with this rather than 1000.0 to accelerate convergence
  CBayerConcs(Bayer *pMdl);
  void            Zero();
  bool            Converge(MArray & MA);
  double          LTotalSodium(MArray & MA);
  double          LTotalInorganicSodium(MArray & MA);
  double          LiquorDensity(double T_, MArray & MA);
  static double   WaterDensity(double T_);
  static double   LiquorDensEqn1(double Tc,
				 double A, double C,
				 double L1,    // Carbonate
				 double L2,    // Chloride
				 double L3,    // Sulphate
				 double L4,    // Oxalate
				 double L5     // Organic Soda
				 );
};


//===========================================================================

class Bayer : public MSpModelBase, public MIBayer
  {
  friend class CBayerConcs;
  protected:
    CBayerConcs LiqConcs25;

    //globals...
    static double sm_dMinSolCp;
    static double sm_dH2OTestFrac0;
    static double sm_dH2OTestFrac1;
    static double sm_dBPE_Factor;
    static double sm_dRho_Factor;


    static double sm_dK1_BoundSodaCalc;
    static double sm_dGrowth_TOC;
    static double sm_dGrowth_s;
    static double sm_dGrowth_fc;
    static double sm_dGrowth_eq;
    static double sm_dGrowth_n;
    static double sm_dGrowth_sol;
    static double sm_dGrowth_ssa;
    







    static byte sm_iVersion;
    static byte   sm_iASatMethod;
    static byte   sm_iTSEMethod;
    //static byte   sm_iRqdCalcMethod;

    //bayer feed calculator...
    bool       fDoCalc;
    double     dRqd_AtoC, dRqd_C, dRqd_CtoS;
    double     dRqd_Sul, dRqd_Sil, dRqd_Salt;
    //double     dRqd_Org, dRqd_OrgRatio;
    double     dRqd_Ox, dRqd_Org;
    double     dRqd_SolFrac;
    double dRqd_Ca;
    

    static     CArray <int, int> LclParm;
  
  public:
	  Bayer(TaggedObject *pNd);
	  ~Bayer();

    bool            ValidateDataFields();

    // Standard Methods
	  bool            get_IsBaseClassOf(LPCTSTR OtherProgID);
	  LPCTSTR         get_PreferredModelProgID();
	  double          get_Density(long Phases, double T, double P, MArray * pMA);
	  double          get_msEnthalpy(long Phases, double T, double P, MArray * pMA);
	  double          get_msEntropy(long Phases, double T, double P, MArray * pMA);
	  double          get_msCp(long Phases, double T, double P, MArray * pMA);
	  double          get_SaturationT(double P, MArray * pMA);
	  double          get_SaturationP(double T, MArray * pMA);
	  double          get_DynamicViscosity(long Phases, double T, double P, MArray * pMA);
    double          get_ThermalConductivity(long Phases, double T, double P, MArray * pMA);

    // Define accessable "properties"
    long            DefinedPropertyCount();
    long            DefinedPropertyInfo(long Index, MPropertyInfo & Info);
    DWORD           GetPropertyVisibility(long Index);
    void            GetPropertyValue(long Index, ULONG Phase, double T, double P, MPropertyValue & Value);
    void            PutPropertyValue(long Index, MPropertyValue & Value);

    // MIBayer overrides
    double          CausticConc(double T_);
    double          AluminaConc(double T_);
    double          SodaConc(double T_);
    double          SodiumCarbonateConc(double T_);
    double          SodiumSulphateConc(double T_);
    double          SodiumOxalateConc(double T_);
    double          SolidsConc(double T_);
    double          TOC(double T_);
    double          AtoC();
    double          CtoS();
    double          CltoC();
    double          Na2SO4toC();
    double          THAMassFlow();
    double          THADens(double T_);
    double          IonicStrength();
    double          AluminaConcSat(double T_);
    double          FreeCaustic(double T_);
    double          AluminaSolubility(double T_) { return AluminaConcSat(T_); };

  protected:
    DDEF_Flags      FixSpEditFlags(DDEF_Flags Flags, int iSpid, bool AsParms);
    double          DRatio(double Tc);
    double          BoundSodaSat(double T_);
    double          LiqCpCalc(MArray & MA, double Tc);
    double          LiqHCalc(MArray & MA, double Tc);
    double          BoilPtElev(MArray & MA, double T);
    void            InputCalcs(bool DoIt, double T_);
    void LiquorComposition(bool);
    double          Molality(MArray & MA, double &, double &);
    double          DigAtoCEquil(double);
    double          PFAtoCEquil(double);
    double          PFSSN(double);
  public:
    //Other properties
    double          LVolume25();
    double          SLVolume25();
    double			    LDensity25();
    double			    SLDensity25();
    double          TotalNa25();
    //double          TotalSodium();
    double          AtoCSaturation(double T_);
    double          SSNRatio(double T_);
    double          Na2CO3toS();
    double          OrganateConc25();
    double          OxalateConc25();
    double          TotalOrganics25();
    double          ChlorineConc25();
    double          SulphateConc25();
    double          TOS(double T_);
    double          TOS25();
    double	    TOC25();
    double          TOStoTOC();
    double	    SolidsConc25();
    double          TOOCtoC();
    double          AluminaSSN(double T_);
    double          NaOnCS();
  };

#endif
