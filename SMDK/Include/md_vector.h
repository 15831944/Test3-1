//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#pragma once

#ifndef  __MD_VECTOR_H
#define  __MD_VECTOR_H

#include "md_defn.h"
#include "md_share1.h"
//#include "md_alumina.h"

#ifdef __MD_VECTOR_CPP
#define DllImportExport __declspec(dllexport)
#elif !defined(SMDK1)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

//---------------------------------------------------------------------------
// Open up low level options
#define DBG_MVECTOR 0

//---------------------------------------------------------------------------
// Forward declares; implementation in SysCAD App;
class CSysVector;
class SpModel;
class SpQuality;
class SpConduit;
class SpContainer;
class SpImage;
class CSD_Distribution;

//---------------------------------------------------------------------------
// Forward declares
class MVector;
class MStream;
class MContainer;
class MIPSD;
class MSpQualityBase;
class MSpModelBase;
class MXSpModel;
class MXSpQuality;

//---------------------------------------------------------------------------

typedef TaggedObject * LPTAGGEDOBJECT;
typedef SpConduit * LPSPCONDUIT;
typedef SpConduit & REFSPCONDUIT;
typedef SpContainer * LPSPCONTAINER;
typedef SpContainer & REFSPCONTAINER;

//---------------------------------------------------------------------------

class DllImportExport MProperty
  {
  friend class MIPSD;
  friend class MStream;
  friend class MContainer;
  friend class MVectorDefn;
  friend class MSpecieDefn;

  public:
    MProperty(MVector * pVec, long iIndex);
    ~MProperty();

  public:
    UCHAR   getType() const;
    LPCTSTR getTag() const;
    LPCTSTR getSymbol() const;
    LPCTSTR getDescription() const;
    bool    getSettable() const;
    bool    getHardMinimum() const;
    bool    getSoftMinimum() const;
    bool    getHardMaximum() const;
    bool    getSoftMaximum() const;
    long    getIndex() const;
    long    getCnvIndex() const;
    long    LValue(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    double  DValue(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    LPCTSTR SValue(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    void    putReqdLValue(long Value);
    void    putReqdDValue(double Value);
    void    putReqdSValue(LPCTSTR Value);

  public:
    __declspec(property(get=getType))                  UCHAR   Type;
    __declspec(property(get=getTag))                   LPCTSTR Tag;
    __declspec(property(get=getSymbol))                LPCTSTR Symbol;
    __declspec(property(get=getDescription))           LPCTSTR Description;
    __declspec(property(get=getCnvIndex))              long    CnvIndex;
    __declspec(property(get=getSettable))              bool    Settable;
    __declspec(property(get=getHardMinimum))           bool    HardMinimum;
    __declspec(property(get=getSoftMinimum))           bool    SoftMinimum;
    __declspec(property(get=getHardMaximum))           bool    HardMaximum;
    __declspec(property(get=getSoftMaximum))           bool    SoftMaximum;
    __declspec(property(get=getIndex))                 long    Index;
    //__declspec(property(get=getLValue))                long    LValue[][][];
    //__declspec(property(get=getUValue))                ULONG   UValue[][][];
    //__declspec(property(get=getDValue))                double  DValue[][][];
    //__declspec(property(get=getSValue))                LPCTSTR SValue[][][];
    __declspec(property(put=putReqdLValue))            long    ReqdLValue;
    __declspec(property(put=putReqdUValue))            ULONG   ReqdUValue;
    __declspec(property(put=putReqdDValue))            double  ReqdDValue;
    __declspec(property(put=putReqdSValue))            LPCTSTR ReqdSValue;

  protected:
    MVector * m_pVec;
    long      m_iIndex;
  };

//---------------------------------------------------------------------------
/* Class MProperties : Collection of available properties (MProperty class).*/
class DllImportExport MProperties
  {
  friend class MIPSD;
  friend class MStream;
  friend class MContainer;
  friend class MVectorDefn;
  friend class MSpecieDefn;
  public:
    MProperties(MVector * pVec) { m_pVec=pVec; };
    ~MProperties() {};

  public:
    //number of properties (MProperty) defined
    long          Count();
    //return index of property of specified name, -1 if not found.
    long          Index(LPCTSTR Name);
    //return MProperty at specified index
    MProperty     operator[](int Index);
    //return MProperty of specified name (throw exception if unknown name)
    MProperty     operator[](LPCTSTR Name);

  protected:
    MVector * m_pVec;
  };

//---------------------------------------------------------------------------

class DllImportExport MArray
  {
  public:
    MArray();
    MArray(CSysVector   * p);
    MArray(MVector      * p);
    MArray(MSpModelBase * p);
    ~MArray();

    double & operator [](int i);

    double Mass(DWORD Phases=MP_All);
    double Moles(DWORD Phases=MP_All);
    double MassFrac(DWORD Phases=MP_All);
    double MoleFrac(DWORD Phases=MP_All);

    MArray & operator=(MArray & M);
    void   Normalise();

    operator CSysVector *() { return m_pSysVec; };

  private:
    bool m_bOwner;
    union
      {
      CSysVector *m_pSysVec;
      double (*dbgM)[64];         // For debugging - limited visibilty
      };
  };

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Template interface (IF) functions

template<class T> T * getIF(MVector *pVec, bool Throw=true)
  {
  T*p=dynamic_cast<T*>(pVec->GetSpModel4Cast());
  if (p)
    return p;
  p=dynamic_cast<T*>(pVec->GetMSpModelBase4Cast());
  if (p)
    return p;
  if (&typeid(T)==&typeid(MIPSD))
    return dynamic_cast<T*>(pVec->getPSD());
  long n=pVec->GetSpQualityCount4Cast();
  for (int i=0; i<n; i++)
    {
    p=dynamic_cast<T*>(pVec->GetSpQuality4Cast(i));
    if (p)
      return p;
    p=dynamic_cast<T*>(pVec->GetMSpQualityBase4Cast(i));
    if (p)
      return p;
    }
  if (Throw)
    throw MMdlException(0,"Interface not found");
  return static_cast<T*>(NULL);
  };

template<class T> T * createIF(MVector *pVec)
  {
  T * p = getIF<T>(pVec, false);
  if (p)
    return p;

  p = dynamic_cast<T*>(pVec->GetMSpQualityBase4Cast(T::GroupIdNo(), true));
  if (p)
    return p;

  throw MMdlException(0,"Interface not found");
  return static_cast<T*>(NULL);
  };

template<class T> bool IsNothing(T & p) { return &p==NULL; };

template<class T> bool IsNothing(T * p) { return p==NULL; };

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Class MVector :*/
class DllImportExport MVector
  {
  friend class MArray;
  //friend class MBayer;
  friend class MIPSD;
  friend class MProperty;
  friend class MProperties;
  friend class MStream;
  friend class MContainer;
  friend class MVectorDefn;
  friend class MSpecieDefn;
  friend class MClosureInfo;
  friend class MVLEBlk;
  private:
    MVector(); // cannot construct standalone

  public:
    MVector(SpModel * pMdl);
    MVector(MSpQualityBase * p);
    ~MVector();

    //interface:
    long          Count(DWORD Phases=MP_All);   //number of species (same as gs_MVDefn.Count())
    LPCTSTR       getTag();

    // ----------------------------- Basic State Access
    double        getT() const;
    void          putT(double T);
    double        getP() const;
    void          putP(double P);
    double        getM(long i) const;
    void          putM(long i, double M);
    double        getMl(long i) const;
    void          putMl(long i, double M);
    double        getA(long i) const;
    void          putA(long i, double A);
    void          putSuppressErrors(bool On);

    double      * getMassVector();
    void          MarkStateChanged();  // Must be called if Masses are changed using the MassVector;

    __declspec(property(get=getTag))                    LPCTSTR Tag;
    __declspec(property(get=getT, put=putT))            double T;
    __declspec(property(get=getP, put=putP))            double P;
    __declspec(property(get=getM, put=putM))            double M[];
    __declspec(property(get=getMl, put=putMl))          double Ml[];
    __declspec(property(get=getA, put=putA))            double A[];
    __declspec(property(get=getMassVector))             double *MassVector;
    __declspec(property(put=putSuppressErrors))         bool   SuppressErrors;

    // ----------------------------- Properties

    double        RefTemp() const;

                  // returns the specific Cp(kJ/K/kg) for the required Phases at T & P
    double        msCp(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
                  // returns the specific Sensible heat contained(kJ/kg) for the required Phases at T & P
                  // this equals Zero at 0C and StdP for all species.
    double        msHs(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
                  // returns the specific heat contained(kJ/kg) for the required Phases at T & P
                  // this equals Zero at 0C and StdP for all species that exist in only one phase.
                  // if a specie exists in more that one state (eg. Water and Steam) then this value
                  // is zero for the specie with the lowest energy content and the other state specie(s)
                  // include the latent heat of transformation.
    double        msHz(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
                  // returns the specific heat contained including the heat of formation (kJ/kg)
                  // for the required Phases at T & P.
    double        msHf(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    double        msSf(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;

                  // returns the total Cp(kJ/K) for the required Phases at T & P
    double        totCp(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
                  // returns the total Sensible heat contained(kJ) for the required Phases at T & P
                  // this equals Zero at 0C and StdP for all species.
    double        totHs(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
                  // returns the total heat contained(kJ) for the required Phases at T & P
                  // this equals Zero at 0C and StdP for all species that exist in only one phase.
                  // if a specie exists in more that one state (eg. Water and Steam) then this value
                  // is zero for the specie with the lowest energy content and the other state specie(s)
                  // include the latent heat of transformation.
    double        totHz(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
                  // returns the total heat contained including the heat of formation (kJ/kg)
                  // for the required Phases at T & P.
    double        totHf(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    double        totSf(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;

    double        TCritical(DWORD Phase=MP_Gas, eScdGasMixingRule Rule=eScdGMR_Default) const;
    double        PCritical(DWORD Phase=MP_Gas, eScdGasMixingRule Rule=eScdGMR_Default) const;
    double        VCritical(DWORD Phase=MP_Gas, eScdGasMixingRule Rule=eScdGMR_Default) const;
    double        AccentricFactor(DWORD Phase=MP_Gas, eScdGasMixingRule Rule=eScdGMR_Default) const;

    double        msdHIdeal(DWORD Phase, double T1, double T2) const;
    double        msdSIdeal(DWORD Phase, double T1, double T2) const;
    double        msdHResidual(DWORD Phase, double T, double P, eScdGasMixingRule Rule=eScdGMR_Default, eScdEqnOfState Eqn=eScdEOS_Default) const;
    double        msdSResidual(DWORD Phase, double T, double P, eScdGasMixingRule Rule=eScdGMR_Default, eScdEqnOfState Eqn=eScdEOS_Default) const;


    double        PartialPress(int IncIds, int ExcIds, double Temp) const;

    double        Mass(DWORD Phases=MP_All) const;
    double        Moles(DWORD Phases=MP_All) const;
    double        MoleWt(DWORD Phases=MP_All) const;

    double        Volume(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    double        NVolume(DWORD Phases=MP_All) const;

    double        Density(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    double        NDensity(DWORD Phases=MP_All) const;

    double        MassFrac(DWORD Phases=MP_All) const;
    double        MoleFrac(DWORD Phases=MP_All) const;
    double        VolFrac(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    double        SpecieConc(DWORD Phases, int iSpNo, double Temp=NAN) const;
    double        PhaseConc(DWORD Phases, DWORD Phase, double Temp=NAN) const;

    double        DynamicViscosity(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    double        KinematicViscosity(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    double        ThermalConductivity(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    double        PrandtlNo(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    double        SurfaceTension(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    double        HeatOfCombustionHi(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;
    double        HeatOfCombustionLo(DWORD Phases=MP_All, double Temp=NAN, double Pres=NAN) const;

    int           FlashCmpIndex();
    int           FlashLiqIndex();
    int           FlashVapIndex();
    LPTSTR        FlashDescription();
    double        SaturationTotalP(double Temp=NAN) const;
    double        SaturationP(double Temp=NAN) const;
    double        SaturationT(double Pres=NAN) const;
    double        PureSaturationP(double Temp=NAN) const;
    double        PureSaturationT(double Pres=NAN) const;
    double        BoilingPtElevation(double Pres=NAN) const;

#if DBG_MVECTOR
    double        msHfRaw() const;
#endif

    // ----------------------------- Operations
    void          SetTP(double Temp, double Pres);
    void          Set_msHs(double H, double Pres=NAN, MVector * pRefMdl=NULL);
    void          Set_msHz(double H, double Pres=NAN, MVector * pRefMdl=NULL);
    void          Set_msHf(double H, double Pres=NAN, MVector * pRefMdl=NULL);
    void          Set_totHs(double H, double Pres=NAN, MVector * pRefMdl=NULL);
    void          Set_totHz(double H, double Pres=NAN, MVector * pRefMdl=NULL);
    void          Set_totHf(double H, double Pres=NAN, MVector * pRefMdl=NULL);

    void          ZeroMass();
    void          ZeroDeriv();
    void          ScaleMass(DWORD Phases, double Mult);
    void          AdjustMassTo(DWORD Phases, double RqdMass);
    void          SetTraceMass();

    void          SetF(MVector &  Other, DWORD Phases, double Frac, double P=NAN);
    void          SetM(MVector &  Other, DWORD Phases, double Mass, double P=NAN);
    void          AddF(MVector &  Other, DWORD Phases, double Frac);
    void          AddM(MVector &  Other, DWORD Phases, double Mass);
    void          SubF(MVector &  Other, DWORD Phases, double Frac);
    void          SubM(MVector &  Other, DWORD Phases, double Mass);

    long          Extract(DWORD Phases, double *pT=NULL, double *pP=NULL, double *pM=NULL, long MCount=0) throw (...);
    long          Replace(DWORD Phases, double TRqd=NAN, double PRqd=NAN, double *pMRqd=NULL, long MCount=0) throw (...);

    MVector & operator=(const MVector & V);

    // ----------------------------- Properties
    MProperties   Properties;
    //MQualities    Qualities;

    // ----------------------------- Access to SpecieModel/Quality Interfaces
    MXSpModel       * GetSpModel4Cast();
    MSpModelBase    * GetMSpModelBase4Cast();
    MXSpQuality     * GetSpQuality4Cast(long i, bool Required=false);
    MSpQualityBase  * GetMSpQualityBase4Cast(long i, bool Required=false);
    long              GetSpQualityCount4Cast();
    //Particle SIze Distribution specials
    MIPSD       * CreatePSD();
    MIPSD       * getPSD(long Index=-1);
    MIPSD       * getPSD(LPCTSTR Name);

    // ---- IF (interface) class access functions ---
    template<class T> bool IFExists() { return getIF<T>(this, false)!=NULL; }; //return true if specified IF class is present
    template<class T> T * GetIF()     { return getIF<T>(this, true); };        //return pointer to specified IF class, throw exception if doesn't exist
    template<class T> T * FindIF()    { return getIF<T>(this, false); };       //return pointer to specified IF class if exists, otherwise return NULL
    template<class T> T * CreateIF()  { return createIF<T>(this); };           //return pointer to specified IF class if exists, otherwise create the class and return the pointer

    template<class T> T & IF(bool Throw=true) { return *getIF<T>(this, Throw); }; //return reference to specified IF class. Warning: this may reference NULL, need to use IsNothing to test. Rather use GetIF or FindIF.

  private:
    SpModel     * m_pSpMdl;
    MIPSD       * m_pPSD;
  public:
    virtual SpModel * getSpMdl() const
      {
      if (!m_pSpMdl)
        throw MMdlException(0,"Null Model Pointer");
      return m_pSpMdl;
      };
    __declspec(property(get=getSpMdl))    SpModel * SpMdl;
  };

//---------------------------------------------------------------------------

class DllImportExport MStream : public MVector
  {
  friend class MBaseMethod;
  friend class MFlow;
  public:
    //MStream(SpConduit *pCn=NULL);

    MStream();
    MStream(const MStream &Cn);
    MStream(MBaseMethod * Method, LPCSTR Tag, LPCSTR SpMdlId=NULL);
    MStream(MBaseMethod * Method, LPCSTR Tag, MVector &V);
    ~MStream();

    void          Allocate(LPCSTR Tag=NULL, LPCSTR SpMdlId=NULL);
    void          Allocate(LPCSTR Tag, MVector &V);
    void          Attach(const MStream &Cn);
    void          Attach(SpConduit *pCn);
    void          Detach();
    void          CheckAttached();
    void          SelectModel(MStream *V, double MassFlow);
    void          SelectModel(long NStreams, MStream *V, double *MassFlow);

    bool          getExists()   const { return m_pCd!=NULL; };
    LPCTSTR       getModelId() const;
    void          putModelId(LPCTSTR Mdl);
    void          putModelId(MVector & Mdl);

    __declspec(property(get=getExists))                     bool    Exists;
    __declspec(property(get=getModelId, put=putModelId))    LPCTSTR ModelId;

    double        MassFlow(DWORD PhMsk=MP_All) const;
    double        VolumeFlow(DWORD PhMsk=MP_All, double T=NAN, double P=NAN) const;
    double        msCp(DWORD PhMsk=MP_All, double T=NAN, double P=NAN) const;
    double        Density(DWORD PhMsk=MP_All, double T=NAN, double P=NAN) const;

    bool          HasSizeData(long iSpeciesIndex) { return( gs_MVDefn[iSpeciesIndex].HasSizeData() ); };
    LPCTSTR       Tag(long iSpeciesIndex)         { return( gs_MVDefn[iSpeciesIndex].Tag()); };
    LPCTSTR       Symbol(long iSpeciesIndex)      { return( gs_MVDefn[iSpeciesIndex].Symbol()); };
    bool          IsSolid(long iSpeciesIndex)     { return( gs_MVDefn[iSpeciesIndex].IsSolid()); };
    bool          IsGas(long iSpeciesIndex)       { return( gs_MVDefn[iSpeciesIndex].IsGas()); };
    bool          IsLiquid(long iSpeciesIndex)    { return( gs_MVDefn[iSpeciesIndex].IsLiquid()); };

    MStream     & operator*=(const double &D);
    MStream     & operator=(const MStream & S);
    void          Copy(const MStream & S, double P=NAN);

    // Access methods for use by interface library
    operator LPTAGGEDOBJECT();
    operator LPSPCONDUIT()            { return m_pCd; };
    operator REFSPCONDUIT()           { return *m_pCd; };
    REFSPCONDUIT getCd() const        { return *m_pCd; };
    __declspec(property(get=getCd))   REFSPCONDUIT Cd;

  protected:
    MStream(SpConduit *pCd);
    virtual SpModel * getSpMdl() const;

  private:
    MBaseMethod * m_pMethod;
    SpConduit   * m_pCd;
    bool          m_bOwned;
  };

class MStreams : public CArray <MStream, MStream&> {};

//---------------------------------------------------------------------------

class DllImportExport MContainer : public MVector
  {
  friend class MBaseMethod;
  public:
    MContainer();
    MContainer(const MContainer &Cn);
    MContainer(MBaseMethod * Method, LPCSTR Tag, LPCSTR SpMdlId);
    MContainer(MBaseMethod * Method, LPCSTR Tag, MVector &V);
    ~MContainer();

    void          Allocate(LPCSTR Tag=NULL, LPCSTR SpMdlId=NULL);
    void          Allocate(LPCSTR Tag, MVector &V);
    void          Attach(const MContainer &Cn);
    void          Attach(SpContainer *pCn);
    void          Detach();
    void          CheckAttached();

    bool          getExists()   const { return m_pCn!=NULL; };
    LPCTSTR       getModelId() const;
    void          putModelId(LPCTSTR Mdl);
    void          putModelId(MVector & Mdl);

    __declspec(property(get=getExists))                     bool    Exists;
    __declspec(property(get=getModelId, put=putModelId))    LPCTSTR ModelId;


    void          Allocate(LPCSTR Tag);
    void          Deallocate();
    void          CheckAllocated();

    double        Mass(DWORD PhMsk=MP_All) const;
    double        Volume(DWORD PhMsk=MP_All, double T=NAN, double P=NAN) const;
    double        msCp(DWORD PhMsk=MP_All, double T=NAN, double P=NAN) const;
    double        Density(DWORD PhMsk=MP_All, double T=NAN, double P=NAN) const;

    MContainer & operator*=(const double &D);
    MContainer & operator=(const MContainer & C);

    // Access methods for use by interface library
    operator LPTAGGEDOBJECT();
    operator LPSPCONTAINER()          { return m_pCn; };
    operator REFSPCONTAINER()         { return *m_pCn; };
    REFSPCONTAINER getCn() const      { return *m_pCn; };
    __declspec(property(get=getCn))   REFSPCONTAINER Cn;

  protected:
    MContainer(SpContainer *pCn);
    virtual SpModel * getSpMdl() const;

  private:
    MBaseMethod * m_pMethod;
    SpContainer  * m_pCn;
    bool         m_bOwned;
  };

class MContainers : public CArray <MContainer, MContainer&> {};

//---------------------------------------------------------------------------

#undef DllImportExport

#endif
