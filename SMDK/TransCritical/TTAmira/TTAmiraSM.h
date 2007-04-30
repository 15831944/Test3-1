//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __AmiraSM_H_
#define __AmiraSM_H_
#include "md_headers.h"
#include <vector>


extern "C" double g0h2o_(double*, double *);

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
                   double*     //  Solmkg             ARRAY (21)
		   );


const static long InUnits = 3;
const static long NInComp = 9;

const static long NOutComp = 9;
const static long NOC = 2;
const static long NGamma = 12;
const static long NSI = 10;
const static long NSol = 6;



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
    iAw
};





double G0H2O(double P, double T) {
  return g0h2o_(&P, &T);  // Amira test: Gibbs free energy for water...
}


//===========================================================================

class AmiraBayer : public MSpModelBase, public MIBayer
  {
  protected:

    //globals...
    //static hAmiraDLL;
    

    static     CArray <int, int> LclParm;

#if DBG_MVECTOR
    CArray<double, double> m_dbgM;
    double                 m_dbgtotHf;
#endif


    // These are the Amira variables that get passed back by the function
    double Comp_molkg    [12];    
    double Comp_molL     [12];
    double Comp_molL25   [12];
    double Comp_mpercent [12];
    double Comp_gL       [12];
    double Gamma         [12];
    double OC            [2];
    double SI            [10];
    double SolML         [6 ];
    double Solmkg        [6 ];

    // This is all of the double precision results... add additional elements as needed
    double dpData[18];
    

  public:
    AmiraBayer(TaggedObject *pNd);
    ~AmiraBayer();
    void RecalcAmira();
    void Bayer(double, double, MArray &);

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
    double  CausticConc(double T_, MArray *pMA);
    double AtoC(MArray *pMA);
    double FreeCaustic(double T_, MArray *pMA);

  protected:
    DDEF_Flags      FixSpEditFlags(DDEF_Flags Flags, int iSpid, bool AsParms);
    double DRatio(double Tc);
    double BoundSodaSat(double T_);
    double LiqCpCalc(MArray & MA, double Tc);
    double LiqHCalc(MArray & MA, double Tc);
    double BoilPtElev(MArray & MA, double T);
    void   InputCalcs(bool DoIt, double T_);
    void   LiquorComposition(bool);
    double Molality(MArray & MA, double &, double &);
  public:
    //Other properties
    double LVolume25();
    double SLVolume25();
    double LDensity25();
    double SLDensity25();
    double TotalNa25();
    double AtoCSaturation(double T_);
    double SSNRatio(double T_);
    double Na2CO3toS();
    double OrganateConc25();
    double OxalateConc25();
    double TotalOrganics25();
    double ChlorineConc25();
    double SulphateConc25();
    double TOS(double T_);
    double TOS25();
    double TOC25();
    double TOStoTOC();
    double SolidsConc25();
    double TOOCtoC();
    double AluminaSSN(double T_);
    double NaOnCS();                  
  
    void            CheckConverged(MArray *pMA=NULL);

  protected:
#if DBG_MVECTOR
    double          DumpIt(LPCTSTR Tag, double V, double x1=NAN, double x2=NAN, double x3=NAN);
    bool            TestStateValid(int i);
#endif
  };

#endif
