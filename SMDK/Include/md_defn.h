//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#pragma once

#ifndef  __MD_DEFN_H
#define  __MD_DEFN_H

#include <float.h>
#include "md_share2.h"
#include "md_share3.h"

#ifdef __MD_DEFN_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(DEVLIB)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#pragma warning( disable : 4290 ) // Exception Syntax

//---------------------------------------------------------------------------

extern DllImportExport double NAN;   //Not A Number (displayed as *) in Access window
extern DllImportExport double ZeroC; //Temperature zero Centigrade

//---------------------------------------------------------------------------

//Model Phase Masks MP_xxx :
extern DllImportExport long MP_All; //All phases
extern DllImportExport long MP_Sol; //solid phase
extern DllImportExport long MP_Liq; //liquid phase
extern DllImportExport long MP_Gas; //vapour phase
extern DllImportExport long MP_SL;  //solids+liquids

//---------------------------------------------------------------------------

class MVector;
class CElementD;
class CSpecie;
class CSpecieBlk;
class SpPropOveride;
class MArray;

//---------------------------------------------------------------------------
/* Class MElementDefn: Definition of an element from collection of elements used.*/
class DllImportExport MElementDefn
  {
  friend class MSpecieDefn;
  friend class MSpecieElement;
  friend class MSpecieElements;
  friend class MElementsInUse;
  friend class MPeriodicTable;
  protected:
    //MElementDefn()                    { m_pElem=NULL; };
    MElementDefn(CElementD * pElement)   { m_pElement=pElement; };

  public:
    //element symbol of specified index; eg H2O(l)
    LPCTSTR       Symbol();
    //element atomic number
    long          AtomicNumber();
    //element atomic weight
    double        AtomicWt();

    long          InUseCount();

  protected:
    CElementD   * m_pElement;
  };

//---------------------------------------------------------------------------
/* Class MSpecieElement: Definition of the amount of an element in a specie.*/
class DllImportExport MSpecieElement
  {
  friend class MSpecieDefn;
  friend class MSpecieElements;
  protected:
    MSpecieElement(CElementD * pElement, double Amount) : m_pElement(pElement), m_Amount(Amount) {};

  public:
    MElementDefn   Element();
    double         Amount();

  protected:

    CElementD   *  m_pElement;
    double         m_Amount;

  };

//---------------------------------------------------------------------------
/* Class MSpecieElements: Collection the elements in a specie.*/

class DllImportExport MSpecieElements
  {
  friend class MSpecieDefn;
  protected:
    MSpecieElements(CSpecie * pSpecie);

  public:
    long          Count();
    MSpecieElement operator[] (long i);

  protected:

    CSpecie     * m_pSpecie;
  };


//---------------------------------------------------------------------------
/* Class MSpecieDefn: Definition of a specie from collection of species used.
   Provides access to properties of individual species from the "specie database".*/
class DllImportExport MSpecieDefn
  {
  friend class MVectorDefn;
  friend class MPSDDefn;
  friend class MPSDDefns;
  public:
    MSpecieDefn()
      {
      m_pSp=NULL;
      m_iPSDDefn=-1;
      };
    ~MSpecieDefn()
      {
      };

    //specie tag of specified index; eg Water(l)
    LPCTSTR       Tag();
    //specie symbol of specified index; eg H2O(l)
    LPCTSTR       Symbol();
    //specie phase; eg MP_Gas, MP_Liq, MP_Sol
    DWORD         Phase();

    MSpecieElements Elements();

    //true if phase is MP_Liq (liquid)
    bool          IsLiquid();
    //true if phase is MP_Sol (solid)
    bool          IsSolid();
    //true if phase is MP_Gas (vapour)
    bool          IsGas();

    //specie molecular weight
    double        MolecularWt();

    //specie density at Temperature and Pressure
    double        Density(double T=StdT, double P=StdP, MVector * MVec=NULL);
    //specific volume at Temperature and Pressure
    double        msVolume(double T=StdT, double P=StdP, MVector * MVec=NULL);
    //specie normalised density
    double        NDensity(MVector * MVec=NULL);
    //specific normalised density
    double        msNVolume(MVector * MVec=NULL);

    //lower limit of temperature range for which properties are valid
    double        TempLimitLo();
    //upper limit of temperature range for which properties are valid
    double        TempLimitHi();
    double        TempLimitTestLo();
    double        TempLimitTestHi();


    double        mlHs(double T=StdT, double P=StdP);
    double        mlHz(double T=StdT, double P=StdP);
    double        mlHf(double T=StdT, double P=StdP);
    double        mlCp(double T=StdT, double P=StdP);
    double        mlSf(double T=StdT, double P=StdP);
    double        mlGf(double T=StdT, double P=StdP);

    double        msHs(double T=StdT, double P=StdP);
    double        msHz(double T=StdT, double P=StdP);
    double        msHf(double T=StdT, double P=StdP);
    double        msCp(double T=StdT, double P=StdP);
    double        msSf(double T=StdT, double P=StdP);
    double        msGf(double T=StdT, double P=StdP);

    double        VapourP(double T=StdT);
    double        VapourT(double P=StdP);

    // Critical Pressure
    double        PCritical();
    // Critical Temperature
    double        TCritical();
    // Critical Volume
    double        VCritical();
    // Acentricity
    double        Acentricity();
    // Molar Volume
    double        MolarVolume();
    // Molecular Diameter
    double        MolecularDiam();

    bool          PcOK();
    bool          TcOK();
    bool          VcOK();
    bool          AcOK();


    //specie size data used for specified index
    bool          HasSizeData();
    //pointer to PSD Definition, if present
    MPSDDefn    * getPSDDefn();

  protected:
    CSpecie      *m_pSp;
    long          m_iPSDDefn;
  private:
  };

//---------------------------------------------------------------------------
/* Class MVectorDefn: Definition of collection of species used. Provides access
   to properties of individual species from the "specie database".*/
class DllImportExport MVectorDefn
  {
  friend class MPSDDefn;
  friend class MPSDDefns;
  public:
    MVectorDefn();
    ~MVectorDefn();

    void          Initialise();

    //number of species in Phase
    long          Count(DWORD PhMask=MP_All);
    //return index of specified specie symbol or tag, throws exception if not found
    long          Lookup(LPCTSTR Symbol) throw (...);
    //return index of specified specie symbol or tag, -1 if not found
    long          LookupNX(LPCTSTR Symbol);

    //return global specie model "Fidelity"
    long          getFidelity() const;
    //set global specie model "Fidelity"
    void          putFidelity(long F);

    __declspec(property(get=getFidelity, put=putFidelity)) long Fidelity;

    //return reference to specie definition class at specified specie index
    MSpecieDefn & operator[](long Index)          { return m_Species[Index]; };
    //return reference to specie definition class by searching for Name to determine specie index
    MSpecieDefn & operator[](LPCTSTR Name)        { return m_Species[Lookup(Name)]; };

    double        msHz(DWORD Phase, double T_, double P_, SpPropOveride *Ovr, MArray * M);
    double        msCp(DWORD Phase, double T_, double P_, SpPropOveride *Ovr, MArray * M);

    MDDValueLst * DDSpList();
    MDDValueLst * DDSpListDash();
    MDDValueLst * DDSolSpList();
    MDDValueLst * DDSolSpListDash();
    MDDValueLst * DDLiqSpList();
    MDDValueLst * DDLiqSpListDash();
    MDDValueLst * DDVapSpList();
    MDDValueLst * DDVapSpListDash();

  protected:

  private:
    //void          TestInitialised() { if (m_bInitReqd) Initialise(); }
    bool          m_bInitReqd;
    CArray<MSpecieDefn, MSpecieDefn&> m_Species;
  };

//The one and only global instance of the Specie Vector Definition:
extern DllImportExport MVectorDefn gs_MVDefn;

//---------------------------------------------------------------------------
// An Array of Elements in Use
class DllImportExport MElementsInUse
  {
  public:
    MElementsInUse() {}

  public:
    long           Count();
    MElementDefn   operator[] (long i);
  };

extern DllImportExport MElementsInUse gs_ElementsInUse;

//---------------------------------------------------------------------------
// An Array of All Defined Elements 
class DllImportExport MPeriodicTable
  {
  public:
    MPeriodicTable() {}

  public:
    long           Count();
    MElementDefn   operator[] (long i);

    MElementDefn   ByAtmNo(long i)          { return (*this)[i-1]; };
  };

extern DllImportExport MPeriodicTable gs_PeriodicTable;

//---------------------------------------------------------------------------
/* Class MPSDDefn: Definition of sieve series data. An array of size intervals from smallest to largest.*/
class DllImportExport MPSDDefn
  {
  friend class MPSDDefns;
  public:
    MPSDDefn();
    ~MPSDDefn();

  public:
	//name of series
	char* Name() const;
    //number of size intervals
    long          getSizeCount() const;
    //size at interval iSz
    double        getSize(long iSz) const throw(...);
    //smallest possible size (always less than Size[0])
    double        getBottomSize() const;
    //Geometrical mean between iSz and iSz-1 size interval (index of 0 is illegal)
    double        getGeometricMean(long iSz) const throw(...);

    //number of PSD (size data) vectors
    long          getPSDVectorCount() const;
    //number of species using PSD with index iPSDVec (usually one)
    long          getSpecieCount(long iPSDVec) const;
    //species index using PSD with index iPSDVec
    long          getSpecieIndex(long iPSDVec, long iSpecie=0) const;

  public:
    __declspec(property(get=getSizeCount))          long   SizeCount;
    __declspec(property(get=getSize))               double Size[/*iSz*/];
    __declspec(property(get=getBottomSize))         double BottomSize;
    __declspec(property(get=getGeometricMean))      double GeometricMean[/*iSz*/];
    __declspec(property(get=getPSDVectorCount))     long   PSDVectorCount;
    __declspec(property(get=getSpecieCount))        long   SpecieCount[/*iPSDVec*/];
    __declspec(property(get=getSpecieIndex))        long   SpecieIndex[/*iPSDVec*/][/*iSpecie*/];

  private:
    void          Initialise(long iDist);
    long          m_iDist;
  };

//---------------------------------------------------------------------------
/* Class MPSDDefns: Collection of Sieve Series definitions (MPSDDefn).*/
class DllImportExport MPSDDefns
  {
  friend class MPSDDefn;
  public:
    MPSDDefns();
    ~MPSDDefns();
    void          Initialise();

    //number of Sieve Series definitions
    long          Count();
    //return reference to MPSDDefn of specified index
    MPSDDefn &    operator[](long index);

  private:
    bool m_bInitReqd;
    CArray <MPSDDefn, MPSDDefn&> m_Defs;

  };

//The one and only global instance of the list of Specie Sieve Series Definitions:
extern DllImportExport MPSDDefns gs_PSDDefns;

extern DllImportExport bool MInitialise();

// ==========================================================================
//
//
//
// ==========================================================================
/* Class MSpeciePtr: Helper class for direct access to a named specie.*/
class DllImportExport MSpeciePtr
  {
  protected:
    CSpecieBlk    * m_pSB;
    MInitialiseTest & m_InitTest;
  public:
    MSpeciePtr(MInitialiseTest & Inittest, LPCSTR Name, bool Optional, LPCTSTR FileName=__FILE__, LPCTSTR DllName="SOMEDLL");//DLL_GroupName);
    //MSpeciePtr(MSpModelDefBase & DefBase, LPCSTR Name, bool Optional);
    //MSpeciePtr(MSurgeUnitDefBase & DefBase, LPCSTR Name, bool Optional);

    ~MSpeciePtr();
    MSpecieDefn   * operator->();
    operator        int();
    int             get_Index();
    double          get_MW();

    // Properties
    __declspec(property(get=get_MW))     double MW;
    __declspec(property(get=get_Index))  int    Index;

  protected:

    void            ChkIndex();
  };

//===========================================================================

DllImportExport void SetGlobalOther(char* pReferenceName, void* pGlobal);

//return a pointer based on the ReferenceName provided
DllImportExport void* GetGlobalOther(char* pReferenceName);

//===========================================================================
//
//
//
//===========================================================================

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
