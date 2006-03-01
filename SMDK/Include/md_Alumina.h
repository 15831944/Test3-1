//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#if !defined(MD_ALUMINA_H__)
#define MD_ALUMINA_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------------------------------

/*class MIBayer: Interface class for Bayer Properties. This is base class with required overides. 
Aditional optional properties may be included and accessed by name/index.*/
class MIBayer
  {
  public:
    virtual double CausticConc(double T_)=0;
    virtual double AluminaConc(double T_)=0;
    virtual double SodaConc(double T_)=0;
    virtual double SodiumCarbonateConc(double T_)=0;
    virtual double SodiumSulphateConc(double T_)=0;
    virtual double SodiumOxalateConc(double T_)=0;
    virtual double SolidsConc(double T_)=0;
    virtual double TOC(double T_)=0;
    virtual double AtoC()=0;
    virtual double CtoS()=0;
    virtual double CltoC()=0;
    virtual double Na2SO4toC()=0;
    virtual double THAMassFlow()=0;
    virtual double THADens(double T_)=0;
    virtual double IonicStrength()=0;
    virtual double AluminaConcSat(double T_)=0;
    virtual double FreeCaustic(double T_)=0;
    virtual double AluminaSolubility(double T_) { return AluminaConcSat(T_); };
    virtual double OxalateSolubility(double T_) { return 0.0; };
    virtual double SulphateSolubility(double T_) { return 0.0; };
    virtual double CarbonateSolubility(double T_) { return 0.0; };
    virtual double BoehmiteSolubility(double T_) { return 0.0; };

    //remove these...?
    virtual double GrowthRate()                         { return 0.0; };
    virtual double AgglomerationRate()                  { return 0.0; };
    virtual double NucleationRate(double SpecSurface)   { return 0.0; };
    /*
    virtual double TOOC(double T_)=0;
    virtual double TOOCtoC()=0;
    virtual double OxalateEquilibrium(double T_)=0;
    */
  };

// ----------------------------------------------------------------------

#undef DllImportExport

// ----------------------------------------------------------------------

#endif // !defined(MD_ALUMINA_H__
