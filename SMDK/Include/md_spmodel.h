//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#pragma once

#ifndef  __MD_SPMODEL_H
#define  __MD_SPMODEL_H

#include "md_defn.h"
#include "md_share1.h"
#include "md_share2.h"
#include "md_share3.h"
#include "md_data.h"

#ifdef __MD_SPMODEL_CPP
#define DllImportExport __declspec(dllexport)
#elif !defined(SMDK1)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

//---------------------------------------------------------------------------
// Forward declares; implementation in SysCAD;
class SpModelEx;
class SpQuality;
class SpPropOveride;
class TagObjClass;

//---------------------------------------------------------------------------
// Forward declares
class MArray;
class MSpModelBase;

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

class DllImportExport MSpModelDefBase : public MSubConstructBaseDef
  {
  public:
    MSpModelDefBase(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR Desc, unsigned long Flgs, LPCTSTR DLL);
    virtual ~MSpModelDefBase();
    virtual MSubConstructBase* Construct(TaggedObject * pNd) = 0;
    LPCSTR ClassName() { return (LPCSTR)sClassName; };
    unsigned long SpMdlFlags() { return iSpMdlFlags; };

    operator TagObjClass*&()  { return m_pClassDef; };

  protected:
    CString           sClassName;    // Class name - for DataDefn
    unsigned long     iSpMdlFlags;   //SpMdl group flags

    TagObjClass     * m_pClassDef;
    CString           m_sDllName;
  };

//---------------------------------------------------------------------------

#define DEFINE_SPECIEMODEL(Obj) \
class Obj##SpMdlDef :  public MSpModelDefBase \
  { \
  public: \
  Obj##SpMdlDef(LPCTSTR pName, LPCTSTR pClassId, LPCTSTR pShortDesc, LPCTSTR pDesc, unsigned long Flgs, LPCTSTR pDLL); \
  virtual MSpModelBase * Construct(TaggedObject * pNd); \
  }; \
  extern Obj##SpMdlDef Obj##_D; \
  extern Obj * gs_p##Obj;  

#define IMPLEMENT_SPECIEMODEL(Obj, Class, ShortDesc, Desc, DLL) \
  Obj##SpMdlDef::Obj##SpMdlDef(LPCTSTR pName, LPCTSTR pClassId, LPCTSTR pShortDesc, LPCTSTR pDesc, unsigned long Flgs, LPCTSTR pDLL) : \
  MSpModelDefBase(pName, pClassId, pShortDesc, pDesc, Flgs, pDLL) { }; \
  MSpModelBase * Obj##SpMdlDef::Construct(TaggedObject * pNd) { return new Obj(pNd); }; \
  Obj##SpMdlDef Obj##_D(DLL##"*"##Class, NULL, ShortDesc, Desc, 0, DLL); \
  Obj * gs_p##Obj=NULL;

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------


class CCall2MSpProps;
class DataDefnBlk;
class DataChangeBlk;
class ValidateDataBlk;
class DllImportExport MSpModelBase : public MBaseDataCommon, public MSubConstructBase
  {
  friend class CCall2MSpProps;
  friend class MArray;
  friend class MVector;
  protected:
  public:
    MSpModelBase(); 
    virtual         ~MSpModelBase();   //destructor
    void            Init(SpModelEx * pSpMdl);

    virtual void    BuildDataFields() {};                    // virtual method for defining data fields
    virtual bool    ExchangeDataFields() { return false; };  // virtual method for manipulating data
    virtual bool    ValidateDataFields() { return true; };   // virtual method for validating data

  public:
    // Standard Methods
    virtual double  get_Density(long Phases, double T, double P, MArray * pMA);
    virtual double  get_msEnthalpy(long Phases, double T, double P, MArray * pMA);
    virtual double  get_msEntropy(long Phases, double T, double P, MArray * pMA);
    virtual double  get_msCp(long Phases, double T, double P, MArray * pMA);
    virtual double  get_SaturationT(double P, MArray * pMA);
    virtual double  get_SaturationP(double T, MArray * pMA);
    virtual double  get_DynamicViscosity(long Phases, double T, double P, MArray * pMA);
    virtual double  get_ThermalConductivity(long Phases, double T, double P, MArray * pMA);

    virtual bool    get_IsBaseClassOf(LPCTSTR OtherProgID);
    virtual LPCTSTR get_PreferredModelProgID();

    // ----------------------------- Utilities             
    double          DensityMix(double FSol, double Ds, double FLiq, double Dl, double FVap, double Dv, double T_, double P_, MArray & M);
    double          msEnthalpyMix(double FSol, double Hs, double FLiq, double Hl, double FVap, double Hv, double T_, double P_, MArray & M);
    double          msCpMix(double FSol, double Hs, double FLiq, double Hl, double FVap, double Hv, double T_, double P_, MArray & M);

    double          RefTemp() const;

    int             getFidelity();
    SpPropOveride * getPropOverides();
    LPCTSTR         getTag();
    MVector         getVector();
    bool            TestStateValid(int i);
    void            SetStateValid(int i, bool On=true);
    bool            StateUpdateReqd(int i) 
      { 
      if (TestStateValid(i))
        return false;
      SetStateValid(i, true);
      return true;
      }

    SV_View         getView();
    SV_ViewBasis    getViewBasis();

    // ----------------------------- Basic State Access
    double          getM(long i) const;
    void            putM(long i, double M) const;
    double          getMl(long i) const;
    void            putMl(long i, double M) const;

    MArray          getMassArray();                
    double          getPressure();                 
    double          getTemperature();              

    // ----------------------------- 
    MSMFnRanges   & getSMFnRanges();

    double          getBoilingPtElevation(double P, MArray * pMA);

    double          Mass(DWORD Phases=MP_All) const;
    double          Moles(DWORD Phases=MP_All) const;
    double          MoleWt(DWORD Phases=MP_All) const;

    double          MassFrac(DWORD Phases=MP_All) const;
    double          MoleFrac(DWORD Phases=MP_All) const;

    void            ScaleMass(DWORD Phases, double Scl);

    // ----------------------------- Properties
    //virtual LPCTSTR DefinedPropertyMapName() { return ClassId(); }
    virtual long    DefinedPropertyCount() { return 0; };
    virtual long    DefinedPropertyInfo(long Index, MPropertyInfo & Info) { return -1; };

    virtual DWORD   GetPropertyVisibility(long Index) { return ePVis_All; };;
    virtual void    GetPropertyValue(long Index, ULONG Phase, double T, double P, MPropertyValue & Value) {};
    virtual void    PutPropertyValue(long Index, MPropertyValue & Value) {};
    static  void    GetPropertyValueGlobal(long Index, ULONG Phase, double T, double P, MPropertyValue & Value) {};
    static  void    PutPropertyValueGlobal(long Index, MPropertyValue & Value) {};

    // ----------------------------- Helper Classes
  public:
    MLog            Log;
    MDebug          Dbg;
    MDataDefn       DD;
    MDataChange     DX;
    MDataValidate   DV;

    // ----------------------------- 
    __declspec(property(get=getM,put=putM))       double        M[];
    __declspec(property(get=getMl,put=putMl))     double        Ml[];
    __declspec(property(get=getMassVector))       double      * MassVector;
    //__declspec(property(get=getFidelity))         int     Fidelity;
    __declspec(property(get=getPropOverides))     SpPropOveride *PropOverides;
    __declspec(property(get=getMassArray))        MArray        MassArray;
    __declspec(property(get=getPressure))         double        Pressure;
    __declspec(property(get=getTemperature))      double        Temperature;
    __declspec(property(get=getSMFnRanges))       MSMFnRanges  & SMFnRanges;
    __declspec(property(get=getTag))              LPCTSTR       Tag;
    __declspec(property(get=getVector))           MVector       Vector;

    __declspec(property(get=getView))             SV_View      View;
    __declspec(property(get=getViewBasis))        SV_ViewBasis ViewBasis;

  private:
    SpModelEx     * m_pSpMdlX;
    static long     sm_lCount;
    static long     sm_lNo;
    long            m_lNo;

  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

class MSpQualityBase; // forward

/* Class MSpQualityDefBase: */
class DllImportExport MSpQualityDefBase : public MSubConstructBaseDef
  {
  public:
    MSpQualityDefBase(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR Desc, unsigned long Flgs, LPCTSTR DLL, int *pGroupIdNo);
    virtual ~MSpQualityDefBase();
    virtual MSubConstructBase* Construct(TaggedObject * pNd) = 0;
    LPCSTR ClassName() { return (LPCSTR)sClassName; };
    unsigned long SpQualFlags() { return iSpQualFlags; };
    TagObjClass     * ClassDef() { return m_pClassDef; };

  protected:
    CString           sClassName;    // Class name - for DataDefn
    unsigned long     iSpQualFlags;   //SpQual group flags

    TagObjClass     * m_pClassDef;
    CString           m_sDllName;
  };

//---------------------------------------------------------------------------
//Macro expanded for "SpQual Definition Class Factory"
#define DEFINE_SPECIEQUALITY(Obj) \
class Obj##SpQualDef :  public MSpQualityDefBase \
  { \
  public: \
  Obj##SpQualDef(LPCTSTR pName, LPCTSTR pClassId, LPCTSTR pShortDesc, LPCTSTR pDesc, unsigned long Flgs, LPCTSTR pDLL); \
  virtual MSpQualityBase* Construct(TaggedObject * pNd); \
  static MSpQualityBase* ConstructX(TaggedObject * pNd); \
  }; \
  extern Obj##SpQualDef Obj##_D; 
//extern Obj * gs_p##Obj;  

#define ATTACH_SPECIEQUALITY(Obj) \
public: \
  static Obj##SpQualDef * sm_pQualDefn; \
  static int sm_GroupIdNo; \
  static int GroupIdNo()   { return sm_GroupIdNo; };

#define IMPLEMENT_SPECIEQUALITY(Obj, Class, ShortDesc, Desc, DLL) \
  Obj##SpQualDef::Obj##SpQualDef(LPCTSTR pName, LPCTSTR pClassId, LPCTSTR pShortDesc, LPCTSTR pDesc, unsigned long Flgs, LPCTSTR pDLL) : \
  MSpQualityDefBase(pName, pClassId, pShortDesc, pDesc, Flgs, pDLL, &Obj::sm_GroupIdNo) \
  {   /*Obj::sm_GroupIdNo = Obj##_D.m_pClassDef->GroupIdNo();*/ }; \
  MSpQualityBase* Obj##SpQualDef::Construct(TaggedObject * pNd) { return new Obj(pNd); }; \
  Obj##SpQualDef Obj##_D(DLL##"*"##Class, NULL, ShortDesc, Desc, 0, DLL); \
  Obj##SpQualDef * Obj::sm_pQualDefn=&Obj##_D; \
  int Obj::sm_GroupIdNo=-1;
//Obj * gs_p##Obj;  

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

class MArray;
class MPropertyInfo; 
class MPropertyValue; 
class SpQuality;     
class DataDefnBlk;
class DataChangeBlk;
class ValidateDataBlk;

class DllImportExport MSpQualityBase : public MBaseDataCommon, public MSubConstructBase
  {
  friend class CCall2MSpQuals;
  friend class MVector;
  public:
    MSpQualityBase();
    virtual        ~MSpQualityBase();
    void           Init(SpQuality * pSpQual);

    LPCTSTR        getTag();
    MVector        getVector();
    bool           TestStateValid();
    void           SetStateValid(bool On=true);


    virtual void   BuildDataFields() = 0;                   // pure virtual method for defining data fields
    virtual bool   ExchangeDataFields() { return false; };  // virtual method for manipulating data
    virtual bool   ValidateDataFields() { return true; };   // virtual method for validating data

    virtual void   ExchangeSpecies(MSpQualityBase * Other) =0;

    virtual bool   EquilRqd4HEval() { return false; };
    virtual void   Equilibrate() {};

    virtual void   ZeroMass()=0;
    virtual void   ZeroDeriv()=0;
    virtual void   ScaleMass(long Phase, double Mult)=0;
    virtual void   ScaleMass(CArray<int,int> &SpIds, double Mult)=0;

    virtual void   SetMassF(MSpQualityBase * QualSet, MArray & MSet)=0;
    virtual void   AddMassF(MSpQualityBase * QualAdd, MArray & MAdd)=0;
    virtual void   SubMassF(MSpQualityBase * QualSub, MArray & MSub)=0;

    virtual void   Copy(MVector & V, MSpQualityBase * QualCopy)=0;
    virtual void   AddDeriv(MVector & V, MSpQualityBase * Qual2, double Sgn_, double DeltaTime)=0;
    virtual void   AddDiscrete(MVector & V, MSpQualityBase * Qual2, double Sgn_, double DeltaTime)=0;

    virtual long   DefinedPropertyCount()                                          { return 0; };
    virtual long   DefinedPropertyInfo(long Index, MPropertyInfo & Info)           { return -1; };

    virtual DWORD  GetPropertyVisibility(long Index)                               { return ePVis_All; };
    virtual void   GetPropertyValue(long Index, ULONG Phase, double T, double P, MPropertyValue & Value) {};
    virtual void   PutPropertyValue(long Index, MPropertyValue & Value)            {};

    __declspec(property(get=getTag))              LPCTSTR       Tag;
    __declspec(property(get=getVector))           MVector       Vector;

    MLog           Log;
    MDebug         Dbg;
    MDataDefn      DD;
    MDataChange    DX;
    MDataValidate  DV;

    //MVector      * m_pVector;
  private:
    SpQuality     * m_pSpQual;
    static long     sm_lCount;
    static long     sm_lNo;
    long            m_lNo;

  };


#undef DllImportExport

#endif
