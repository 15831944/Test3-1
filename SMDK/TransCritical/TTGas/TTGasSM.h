//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __TTGasSM_H_
#define __TTGasSM_H_
#include "md_headers.h"
#include <vector>

class TTGasSM; // forward declare
int cubic(double a1, double a2, double a3, double x[]);

//===========================================================================


class TTGasSM : public MSpModelBase
  {
  protected:

    //globals...
    




    static byte   sm_iGasModel;
    bool fDoCalc;
    double dRqdMWT;
    double m_dHRes;  // Save these when doing basic compressibility calc
    double m_dSRes;
    
   

    static     CArray <int, int> LclParm;
  
  public:
    TTGasSM(TaggedObject *pNd);
    ~TTGasSM();

    bool            ValidateDataFields();

    // Standard Methods
    bool            get_IsBaseClassOf(LPCTSTR OtherProgID);
    LPCTSTR         get_PreferredModelProgID();
    double          get_Density(long Phases, double T, double P, MArray * pMA);
    double          get_msEnthalpy(long Phases, double T, double P, MArray * pMA);
    double          get_msEntropy(long Phases, double T, double P, MArray * pMA);
    double          get_msCp(long Phases, double T, double P, MArray * pMA);
    double          get_DynamicViscosity(long Phases, double T, double P, MArray * pMA);
    double          get_ThermalConductivity(long Phases, double T, double P, MArray * pMA);
    double          get_SaturationT(double P, MArray *pMA);
    
    double get_MWT(MArray *pMA);
    double idealCp(double T);
    double SRKn(double T, double P, MArray *pMA=NULL);

  
    double HRes(double T, double P);
    double GRes(double T, double P);
    double SRes(double T, double P);
    double Tc();
    double Pc();
    
    double GasZ(double, double);
    double GasCpCalc(double T, double P);
    double GasEnthalpy(double T, double P);
    double GasEntropy(double T, double P);


    // Define accessable "properties"
    long            DefinedPropertyCount();
    long            DefinedPropertyInfo(long Index, MPropertyInfo & Info);
    DWORD           GetPropertyVisibility(long Index);
    void            GetPropertyValue(long Index, ULONG Phase, double T, double P, MPropertyValue & Value);
    void            PutPropertyValue(long Index, MPropertyValue & Value);


  protected:
    DDEF_Flags      FixSpEditFlags(DDEF_Flags Flags, int iSpid, bool AsParms);

  public:
    //Other properties
    double GasDensity(double T, double P) {return P*get_MWT(NULL)/(R_c*GTZ(T)*GasZ(T,P));}
    double NormalDensity() {return 101.325/8.3143*get_MWT(NULL)/273.15;} // Density at atmospheric pressure and 0C
  };

#endif
