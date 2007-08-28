//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __AmiraSM_H_
#define __AmiraSM_H_
#include "md_headers.h"
#include <vector>

#define FORCECNVG false

class SpecieMdlMRU;   // FOrward Declare





extern "C" double g0h2o_(double*, double *);

extern "C" double version_();


extern "C" void bayer_(        // In all its glory
  double*,    //  TempC,
  const long*,//  InUnits,            INTEGER!
  double*,    //  Pressure,
  const long*,//  NInComp,            INTEGER!
  double*,    //  InComp,             ARRAY*11 (5)
  double*,    //  DPDATA
  const long*,      //  NOutComp,     INTEGER!
  double*,    //  Comp_molkg,         ARRAY
  double*,    //  Comp_molL,          ARRAY
  double*,    //  Comp_molL25,        ARRAY  (10)
  double*,    //  Comp_mpercent,      ARRAY
  double*,    //  Comp_gL,            ARRAY
  const long*,      //  NOC,          INTEGER!
  double*,    //  OC,                 ARRAY
  const long*,      //  NGamma,      INTEGER! (15)
  double*,    //  Gamma,             ARRAY
  const long*,      //  NSI,         INTEGER!
  double*,    //  SI,                ARRAY
  const long*,      //  NSol,        INTEGER!
  double*,    //  SolML,             ARRAY (20)
  double*,     //  Solmkg             ARRAY (21)
  const long*
  );


//#define NDPPTS 22   // Double precision data values in DPDATA

const static  long InUnits = 3;
const static  long NInComp = 9;
const static  long NOutComp = 9;
const static  long NOC = 2;
const static  long NGamma = 12;
const static  long NSI = 10;
const static  long NSol = 6;



const static  int anSODIUM = 11;





// Indices into the dpData array...
enum {
    iI_m ,
    iI_c ,
    iI_c25 ,
    iP_sat ,
    iAl2O3 ,
    iTC ,
    iTA ,
    iTempSat ,
    iBPE ,
    iCp_Liq   ,    
    iCp_H2O   ,    
    iRho_Liq  ,   
    iRho_H2O  ,   
    iCp_phi   ,           
    iV_phi    ,     
    iCp_LiqH2O, 
    iPhi      ,       
    iAw, 
    iV25,        // Specific Volume at 25 
    iWT,         // Mass Fraction Water 
    iH,          // Enthalpy
    iS,           // Entropy
    NDPPTS       // Number of double precision data points 
};
// Component indices into return arrays

enum {
    iAlumina,   
    iNaOH	   ,
    iNaCl	   ,
    iNa2CO3	   ,
    iNa2SO4	,
    iOxalate,
    iAcetate,
    iFormate,
    iNaF
};


// Component indices into solubility arrays:
enum {
  iSGibbsite, 
  iSBoemite,  
  iSSulphate,
  iSOxalate, 
  iSNaFl,	  
  iSNa3FSO4 
};



double G0H2O(double P, double T)  /// Test function 
{
  return g0h2o_(&P, &T);
}



struct {
  long iMRU;
  double T, P;
  double DPDATA[NDPPTS];
}  d[3];

  



//===========================================================================

class MurdochBayerSpMdl : public MSpModelBase, public MIBayer
  {
  protected:

    //globals...
    

    static     CArray <int, int> LclParm;

    static MArrayI NaFactor;
    static bool NaFactorOK;

#if DBG_MVECTOR
    CArray<double, double> m_dbgM;
    double                 m_dbgtotHf;
#endif

    long flags;
    bool fDoSolubility;
    // These are the Amira variables that get passed back by the function
    double Comp_molkg    [9];    
    double Comp_molL     [9];
    double Comp_molL25   [9];
    double Comp_mpercent [9];
    double Comp_gL       [9];
    double OC            [2];
    double Gamma         [12];
    double SI            [10];
    double SolML         [6 ];
    double Solmkg        [6 ];

    // This is all of the double precision results... add additional elements as needed

    double dpData[NDPPTS];
    double dpData25[NDPPTS];  // Properties at reference temperature 25C
    double dpData20[NDPPTS];  // Properties at 20C
    double dpData0[NDPPTS];   // Properties at 0C
    double dpDataX[NDPPTS];   // Special Case

  public:
    MurdochBayerSpMdl(TaggedObject *pNd);
    ~MurdochBayerSpMdl();
    void Bayer(double, double, MArray &, double * =NULL, bool = false);

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
    double getDLLVersion();
    double CausticConc(double T_);
    double AluminaConc(double T_);
    double SodaConc(double T_);
    double SodiumCarbonateConc(double T_);
    double SodiumSulphateConc(double T_);
    double SodiumOxalateConc(double T_);
    double SolidsConc(double T_);
    double TOC(double T_);
    double AtoC();
    double CtoS();
    double CltoC();
    double Na2SO4toC();
    double THAMassFlow();
    double THADens(double T_);
    double IonicStrength();
    double AluminaConcSat(double T_);
    double FreeCaustic(double T_);
    double AluminaSolubility(double T_) { return AluminaConcSat(T_); };
// Extras
    //    double  CausticConc(double T_, MArray *pMA);
    //    double FreeCaustic(double T_, MArray *pMA);

  protected:
    DDEF_Flags      FixSpEditFlags(DDEF_Flags Flags, int iSpid, bool AsParms);
    double DRatio(double Tc);
    double LiqCpCalc(double T, MArray * pMA);
    double LiqHCalc(double T, double P, MArray * pMA);
    double LiqSCalc(double T, MArray * pMA);
    double BoilPtElev(double P, MArray * pMA);
    void   InputCalcs(bool DoIt, double T_);
    void   LiquorComposition(bool);
  public:
    //Other properties
    double LVolume25();
    double SLVolume25();
    double LDensity25();
    double SLDensity25();
    double AtoCSaturation(double T_);
    double SSNRatio(double T_);
    //double Na2CO3toS();
    double OrganateConc25();
    double OxalateConc25();
    double TotalOrganics25();
    double ChlorineConc25();
    double SulphateConc25();
    double SolidsConc25();
  
    double * CheckConverged(double T=-1.0, double P=-1.0, MArray *pMA=NULL, bool = FORCECNVG);
    /// bool   TestStateValid(int i);

  protected:
#if DBG_MVECTOR
    double DumpIt(LPCTSTR Tag, double V, double x1=NAN, double x2=NAN, double x3=NAN);
#endif
  };






#endif


class SpecieMdlMRU 
{
  long iMRU;
  double T, P;
  double *m_dData;

  SpecieMdlMRU();
  ~SpecieMdlMRU();
 public:
  double get(int i)  {return m_dData[i];}
  
};

class SpecieMdlMRUList 
{
  SpecieMdlMRU* m_pMRUList;
};
