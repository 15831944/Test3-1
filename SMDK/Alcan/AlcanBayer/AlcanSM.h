//===========================================================================
//=== SysCAD SMDK - Alcan Bayer Properties Model 2003 : Alcan, Kenwalt    ===
//===========================================================================
#ifndef __AlcanSM_H_
#define __AlcanSM_H_
#include "md_headers.h"
#include <vector>

class CAlcanSM; // forward declare

//===========================================================================

// A helper class to assist with calculating iterative concentration and density
// calculations.
class CAlcanConcs
  {
  public:
    static bool     NaFactorOK;
    static MArray   NaFactor;

    CAlcanSM       *pBayerMdl;
    MArray          Liq; //array of concentrations at 25C as Na2CO3 Equiv

    double          Density25;    // Density @ 25 C

    CAlcanConcs(CAlcanSM *pMdl);
    void            Zero();
    bool            Converge(MArray & MA);
    double          LTotalSodium(MArray & MA);
    double          LiquorDensity(double T_, MArray & MA);
    static double   LiquorDensEqn1(double Tc, double WTNa, double WAlumina);
    static double   LiquorDensEqn2(double Tc, double XNaC, double XNa, double XAl);
    static double   LiquorDensEqn3(double Tc, double XAl2O3, double XCaust, double XTTS, double XOC);
  };

//===========================================================================

class CAlcanSM : public MSpModelBase, public MIBayer
  {
  friend class CAlcanConcs;
  protected:
    CAlcanConcs LiqConcs25;

    //globals...
    static byte   sm_iASatMethod;
    static byte   sm_iDensityMethod;
    static byte   sm_iGrowthMethod;
    static byte   sm_iCPMethod;
    static byte   sm_iBPEMethod;
    static double sm_dMinSolCp;
    static double sm_dH2OTestFrac0;
    static double sm_dH2OTestFrac1;
    static double sm_Morphology;
    static double sm_Nucleation;
    static double sm_Na2OFactor;
    static double sm_CarbonFactor;

    bool       fDoCalc;
    bool       m_bRqdAt25;
    double     dRqd_AtoC, dRqd_C, dRqd_CtoS;
    double     dRqd_Sul, dRqd_Sil, dRqd_Salt;
    double     dRqd_P2O5, dRqd_Fl;
    double     dRqd_Ox, dRqd_TOC;
    double     dRqd_SolFrac, dRqd_SolConc;
    double     dRqd_SolOx, dRqd_SolOccSoda;

    static     CArray <int, int> LclParm;
  
  public:
	  CAlcanSM(TaggedObject * pNd);
	  ~CAlcanSM();

    bool            ValidateData();

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

    long            DefinedPropertyCount();
    long            DefinedPropertyInfo(long Index, MPropertyInfo & Info);

    DWORD           GetPropertyVisibility(long Index);
    void            GetPropertyValue(long Index, ULONG Phase, double T, double P, MPropertyValue & Value);
    void            PutPropertyValue(long Index, MPropertyValue & Value);

    // User Stuff
    //DDEF_Flags      FixSpEditFlags(DDEF_Flags Flags, int iSpid, bool AsParms);
    double          LiqCpCalc(MArray & MA, double T);
    double          BoilPtElev(MArray & MA, double T);
    void            InputCalcs(bool DoIt, double T_);

    // User Props
    double          THAMassFlow();
    double          THADens(double T_);
    double          CausticConc(double T_);
    double          AluminaConc(double T_);
    double          SodaConc(double T_);
    double          SodiumCarbonateConc(double T_);
    double          SodiumSulphateConc(double T_);
    double          SodiumOxalateConc(double T_);
    double          SolidsConc(double T_);
    double          IonicStrength();
    double          AluminaConcSat(double T_);
    double          IonicStrengthBoehmite();
    double          AluminaSolubility(double T_) { return AluminaConcSat(T_); };
    double          OxalateSolubility(double T_);
    double          SulphateSolubility(double T_) { return 0.0; };
    double          CarbonateSolubility(double T_) { return 0.0; };
    double          BoehmiteSolubility(double T_);
    double          AtoCSaturation(double T_);
    double          SSNRatio(double T_);
    double          AluminaSSN(double T_);
    double          TOC(double T_);
    double          FreeCaustic(double T_);
    double          AtoC();
    double          CtoS();
    double          CltoC();
    double          Na2SO4toC();
    double          Na2CO3toS();
    double          MR();
    double          AA(); //Total Na conc as Na2O equivilant
    double          TT(); //Caustic conc as Na2O equivilant
    double          SPO();
    double          Na2OSolFrac();

    double			    LDensity25();
    double			    SLDensity25();
    double          LVolume25();
    double          SLVolume25();

    double          TotalSodium();

    double          ChlorineConc25();
    double          SulphateConc25();
    double          OrganicO11Conc25();
    double          OrganateConc25();
    double          OxalateConc25();
    double          TotalOrganicsConc25();
    double          TotalSodiumConc25();
    double			    TOC25();
    double		    	SolidsConc25();

    double          SodiumOxalate_Conc();
    double          OrganicO11_Conc();
    double          Sulphate_Conc();
    double          Chlorine_Conc();
    double          Fluoride_Conc();
    double          Silicate_Conc();

    double          GrowthRate();
    double          AgglomerationRate();
    double          NucleationRate(double SpecSurface);
  };

//===========================================================================
//===========================================================================

#endif //__AlcanSM_H_
