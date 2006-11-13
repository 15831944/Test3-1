//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __TTGasSM_H_
#define __TTGasSM_H_
#include "md_headers.h"
#include <vector>

class TTGasSM; // forward declare

//===========================================================================


class TTGasSM : public MSpModelBase
  {
  protected:

    //globals...
    




    static byte   sm_iGasModel;
    bool fDoCalc;
    double dRqdMWT;
    
   

    static     CArray <int, int> LclParm;
  
  public:
    TTGasSM(TaggedObject *pNd);
    ~TTGasSM();

    bool            ValidateData();

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
    
    double  get_MWT(MArray *pMA);
    double idealCp(double T);
    double dHRes(double T, double P);
    double dGRes(double T, double P);
    double dSRes(double T, double P);
    
      

    double GasCpCalc(double T, double P);
    double GasHCalc(double T, double P);

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
    double NormalDensity() {return 101.325/8.3143*get_MWT(NULL)/273.15;}
  };

#endif
