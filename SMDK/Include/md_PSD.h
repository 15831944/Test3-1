//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#if !defined(MD_PSD_H__)
#define MD_PSD_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "md_defn.h"

#ifdef __MD_PSD_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDK1)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------
// Forward declares; implementation in SysCAD App;
class SpMArray;
class SpModel;
class SpConduit;
class SpContainer;
class SpImage;
class SMBayerBase;
class CSD_Distribution;

//---------------------------------------------------------------------------
// Forward declares
class MVector;
class MStream;
class MContainer;
class MIPSD;
class MSpQualityBase;
class MSpModelBase;

// ----------------------------------------------------------------------

class MISSA
  {
  public:
    virtual double PartDiamFromSAM()                                  = 0;
    virtual double PartNumPerSec()                                    = 0;
    virtual double PartNumPerLiter()                                  = 0;
    virtual double SpecificSurfaceAreaVol()                           = 0;
    virtual double SpecificSurfaceAreaMass()                          = 0;
    virtual void   SetSAM(double SAM, bool Calculated)                = 0;
    virtual void   SetSAMFromFlow(double THAFlow, double NoPerSec)    = 0;
    virtual void   SetSAMFromPartDiam(double D)                       = 0;
    virtual void   SetSAMfromPSD(long npts, double x[], double psd[]) = 0;

  protected:

  };

//---------------------------------------------------------------------------
// Internal dimensions are meters for intervals with sizes increasing;
// if any other order is required then the extract replace methods should be used.

/*
Interpretation of Vectors 
------------------------------------------------------------------------
SizePassingVector, SizePassing: All sizes in Meters       Eg(Microns)
  [  0] Smallest                                           2
  [  1] Smallest+dSz[0]                                    6
  [...]                                                  ...
  [N-2] LargestSdSz[N-2]                                 194
  [N-1] Largest                                          198

FractionPassingVector, FracPassing:
  [  0] Fraction Passing     Smallest                    (-2)
  [  1] Fraction Passing     Smallest+dSz[0]             (-6)
  [...]                                                  ...
  [N-2] Fraction Passing     Largest-dSz[N-2]          (-194)
  [N-1] Fraction Larger than Largest-dSz[N-2]          (+194)

MassPassing:
  [  0] Mass(Flow) Passing     Smallest                  (-2)
  [  1] Mass(Flow) Passing     Smallest+dSz[0]           (-6)
  [...]                                                  ...
  [N-2] Mass(Flow) Passing     Largest-dSz[N-2]        (-194)
  [N-1] Mass(Flow) Larger than Largest-dSz[N-2]        (+194)

  ------------------------------------------------------------------------

SizeRetained:                   All sizes in Meters       Eg(Microns)
  [  0] Largest-dSz[N-2]                                 194
  [  1] Largest-dSz[N-3]                                 188
  [...]                                                  ...
  [N-2] Smallest                                           2
  [N-1] Smallest                                           0

FracRetained:
  [  0] Fraction Larger than Largest-dSz[N-2]          (+194)
  [  0] Fraction Larger than Largest-dSz[N-3]          (+188)
  [...]                                                  ...
  [N-2] Fraction Larger than SmallestSize                (+2)
  [N-1] Fraction Passing     SmallestSize                (-2)

MassRetained:
  [  0] Mass(Flow) Larger than Largest-dSz[N-2]        (+194)
  [  0] Mass(Flow) Larger than Largest-dSz[N-3]        (+188)
  [...]                                                  ...
  [N-2] Mass(Flow) Larger than SmallestSize              (+2)
  [N-1] Mass(Flow) Passing     SmallestSize              (-2)
------------------------------------------------------------------------
*/

class DllImportExport MIPSD
  {
  friend class MVector;

  public:
    MIPSD(MVector * pVector, long iDist);
    virtual ~MIPSD();
    MIPSD &operator=(const MIPSD &V);

  public: //this group of functions provide direct access to the PSD Defn class
    //return reference to the PSD Defn class used
    MPSDDefn &    getDefn() const;
    //number of PSD (size data) vectors
    long          getPSDVectorCount() const;                                    
    //number of size intervals
    long          getSizeCount() const;
    //size at interval iSz
    double        getSize(long iSz) const throw(...);
    //smallest possible size (always less than Size[0])
    double        getBottomSize() const;
    //Geometrical mean between iSz and iSz-1 size interval
    double        getGeometricMean(long iSz) const throw(...);
    //number of species using PSD with index iPSDVec (usually one)
    long          getSpecieCount(long iPSDVec) const;
    //species index using PSD with index iPSDVec
    long          getSpecieIndex(long iPSDVec, long iSpecie=0) const;

  public:
    //return size at which the specified Frac amount is passing when only 1 PSD is used
    double        getSizePassingFraction(double Frac) const;
    //return size at which the specified Frac amount is passing for PSD with index iPSDVec 
    double        getSizePassingFraction(long iPSDVec, double Frac) const;
    //return fraction amount passing for the specified Size when only 1 PSD is used
    double        getFractionPassingSize(double Size) const;
    //return fraction amount passing for the specified Size for PSD with index iPSDVec
    double        getFractionPassingSize(long iPSDVec, double Size) const;

  public: //this group of functions provide access to the size data
    //fraction at size interval iSz when only 1 PSD is used
    double        getFrac(long iSz) const throw(...);
    //fraction at size interval iSz for PSD with index iPSDVec 
    double        getFrac(long iPSDVec, long iSz) const throw(...);
    //set fraction at size interval iSz when only 1 PSD is used
    void          putFrac(long iSz, double F) throw(...);
    //set fraction at size interval iSz for PSD with index iPSDVec 
    void          putFrac(long iPSDVec, long iSz, double F) throw(...);
   
    //return mass at size iSz when only 1 PSD is used
    double        getMass(long iSz) const throw(...); 
    //return mass at size iSz using PSD with index iPSDVec
    double        getMass(long iPSDVec, long iSz) const throw(...);
 
    //Normalise the fractions to ensure sum is 1.0 (set iPSDVec=-1 to normalise all PSD vectors)
    void Normalise(long iPSDVec=-1);

    // Extract a copy of the Size Intervals into double array Intervals
    // Memory must be able to Hold 1 double vector of 'SizeCount' elements
    void ExtractSizes(double * Intervals, double Multiplier=1.0) const;


    // Extract a copy of all the PSD (Size Fraction) vectors the Indices are [nPSDVecs][nSz]
    // NB: Memory must be able to Hold 'PSDVectorCount' double vectors of 'SizeCount' elements
    void ExtractFracMatrix(double * F) const; 
    // Replace all the PSD (Size Fraction) vectors the Indices are [nPSDVecs][nSz]
    void ReplaceFracMatrix(double * F); 

    // Extract a copy of the PSD (Size Fractions) vector with index iPSDVec
    // NB: Memory must be able to Hold 1 double vector of 'SizeCount' elements
    void ExtractFracVector(double *F, int iPSDVec) const;

    // Replace PSD (Size Fractions) vector with index iPSDVec
    void ReplaceFracVector(double *F, int iPSDVec);

    // Extract a vector with the PSD individual masses for each size interval
    // NB: Memory must be able to Hold 1 double vector of 'SizeCount' elements
    void ExtractMassVector(double *M, int iPSDVec) const;
    //CDVector ExtractMassVector(int iPSDVec, bool AsSizeRetained=false) const;     //returns vector of Mass data for item index

    // Replace the PSD vector with the specified individual masses
    void ReplaceMassVector(double *M, int iPSDVec);
    //void ReplaceMassVector(CDVector M, int iPSDVec, bool AsSizeRetained=false);   //Set Mass data from vector of mass data for item index

    // Re-calculate underlying MassFractions
    // This should be called by user or SysCAD if user has operated directly on size fractions
    // by obtaining a reference via getFracPassingVector (not the recommended way)
    //void Normalise(void);

  public:
    __declspec(property(get=getPSDVectorCount))                   long          PSDVectorCount;  
    __declspec(property(get=getSizeCount))                        long          SizeCount;  
    __declspec(property(get=getSize))                             double        Size[/*iSz*/];  
    __declspec(property(get=getFrac,put=putFrac))                 double        Frac[/*iSz*/];  

  public:
    //return pointer to array of size fractions for PSD with index iPSDVec
    double      * getFracVector(long iPSDVec=0) const throw(...);

  protected:
    MVector   * m_pVector;
    long        m_iDist;                                // iDist >=0 : Dist # Reqd
                                                        //       <0  : Whichever exist
  protected: 
    CSD_Distribution & GetDist(MVector * pVec, long iDist) const;
  };

// ----------------------------------------------------------------------

#undef DllImportExport

// ----------------------------------------------------------------------

#endif // !defined(MD_PSD_H__
