//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __BATCBayerSM_H_
#define __BATCBayerSM_H_
#include "..\..\..\SMDK\include\md_headers.h"
#include <vector>

class BATCBayerSM; // forward declare

//===========================================================================

// A helper class to assist with calculating iterative concentration and density calculations.
class CBayerConcs
  {
  public:
    static bool     NaFactorOK;
    static MArray   NaFactor;

    BATCBayerSM    *pBayerMdl;
    MArray          Liq; //array of concentrations at 25C as Na2CO3 Equiv

    double          Density25;    // Density @ 25 C

    CBayerConcs(BATCBayerSM *pMdl);
    void            Zero();
    bool            Converge(MArray & MA);
    double          LTotalSodium(MArray & MA);
    double          LTotalInorganicSodium(MArray & MA);
    double          LiquorDensity(double T_, MArray & MA);
    static double   WaterDensity(double T_);
    static double   LiquorDensEqn1(double Tc, double WTNa, double WAlumina);
    static double   LiquorDensEqn2(double T_, double S, double CtoS, double AtoC, double CltoS, double FtoS, double SO4toS, double TOCtoS);
    static double   LiquorDensEqn3(double T_, double S, double A, double C);
    static double   LiquorDensEqn4(double T_, double S, double A, double C, double Rso4, double Rcl, double Roc);
  };

//===========================================================================

class BATCBayerSM : public MSpModelBase, public MIBayer
  {
  friend class CBayerConcs;
  protected:
    CBayerConcs LiqConcs25;

    //globals...
    static double sm_dMinSolCp;
    static double sm_dH2OTestFrac0;
    static double sm_dH2OTestFrac1;
    static byte   sm_iDensityMethod;
    static byte   sm_iCPMethod;
    static byte   sm_iASatMethod;
    static byte   sm_iBPEMethod;
    //static byte   sm_iRqdCalcMethod;

    //bayer feed calculator...
    bool       fDoCalc;
    double     dRqd_AtoC, dRqd_C, dRqd_CtoS;
    double     dRqd_Sul, dRqd_Sil, dRqd_Salt;
    //double     dRqd_Org, dRqd_OrgRatio;
    double     dRqd_Ox, dRqd_TOC;
    double     dRqd_Fl;
    double     dRqd_SolFrac;
    double     dRqd_SolConc;

    //static     CArray <int, int> LclParm;
  
  public:
	  BATCBayerSM(TaggedObject *pNd);
	  ~BATCBayerSM();

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

    // Define accessable "properties"
    long            DefinedPropertyCount();
    long            DefinedPropertyInfo(long Index, MPropertyInfo & Info);
    DWORD           GetPropertyVisibility(long Index);
    void            GetPropertyValue(long Index, ULONG Phase, double T, double P, MPropertyValue & Value);
    void            PutPropertyValue(long Index, MPropertyValue & Value);
    static  void    GetPropertyValueGlobal(long Index, ULONG Phase, double T, double P, MPropertyValue & Value) {};
    static  void    PutPropertyValueGlobal(long Index, MPropertyValue & Value) {};

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
    double          OxalateSolubility(double T_);
    double          SulphateSolubility(double T_);
    double          CarbonateSolubility(double T_);

  protected:
    DDEF_Flags      FixSpEditFlags(DDEF_Flags Flags, int iSpid, bool AsParms);
    double          LiqCpCalc(MArray & MA, double Tc);
    double          BoilPtElev(MArray & MA, double T);
    void            InputCalcs(bool DoIt, double T_);

  public:
    //Other properties
    double          LVolume25();
    double          SLVolume25();
    double			    LDensity25();
    double			    SLDensity25();
    double          TotalSodiumConc25();
    //double          TotalSodium();
    double          AtoCSaturation(double T_);
    double          SSNRatio(double T_);
    double          Na2CO3toS();
    double          OrganateConc25();
    double          OxalateConc25();
    double          TotalOrganicsConc25();
    double          ChlorineConc25();
    double          SulphateConc25();
    double          FluorideConc25();
    double          TOS(double T_);
    double          TOS25();
    double			    TOC25();
    double          TOStoTOC();
    double			    SolidsConc25();

  };

#endif
