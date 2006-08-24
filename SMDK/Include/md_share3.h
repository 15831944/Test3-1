//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#if !defined(MD_SHARE3_H)
#define MD_SHARE3_H

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000


#undef DllImportExport
#if defined(__MD_SHARE3_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

// ======================================================================
//
//
//
// ======================================================================

class MSpModelBase;  // Forward define for SMDK - used in SpModel

// These two shared classes allow dynamic_cast<> to work to find a SpModel or SpQuality
class MXSpModel
  {
  public:
    MXSpModel() {};
    virtual ~MXSpModel() {};
  };

class MXSpQuality
  {
  public:
    MXSpQuality() {};
    virtual ~MXSpQuality() {};
  };

// ======================================================================

// SpModel SMFnRanges Checks
enum eSpFns { eSpFn_Cp, eSpFn_H, eSpFn_S, eSpFn_Rho, eSpFn_SatP, eSpFn_SatT, eSpFn_BPE, eSpFn_DynVisc, eSpFn_ThermCond, eFn_Last };
const LPCTSTR eSpFnsNames[] = {"Cp", "H", "S", "Rho", "SatP", "SatT", "BPE", "DynVisc", "ThermCond" };

class DllImportExport MSMFnRanges
  {
  friend class SpModel;
  friend class SpModelOwner;

  public:
    MSMFnRanges();
    void Clear();
    void SetInRange(eSpFns FnInx, bool OK);
    bool InRange(eSpFns FnInx);
    bool Valid(eSpFns FnInx);
    bool AllInRange();
    static void BumpGlblIter();

  protected:
    long          m_Flags;
    long          m_Masks;
    long          m_Iter;
    static long   sm_Iter;
  };

// ======================================================================
//
//
//
// ======================================================================

class TaggedObject; // fwd
class MSubConstructBase
  {
  public:
    MSubConstructBase() {};
    virtual ~MSubConstructBase() {};
  };
class MSubConstructBaseDef
  {
  public:
    MSubConstructBaseDef() {};
    virtual ~MSubConstructBaseDef() {};
    virtual MSubConstructBase * Construct(TaggedObject * pNd) = 0;
  protected:
  };

// ======================================================================
//
//
//
// ======================================================================

//Constants for basic drawing primatives for defining a unit graphics symbol...
const long MDrw_Poly   = 0x80000001; //poly-line: list of x,y co-ordinates
const long MDrw_Arc    = 0x80000002; //circle: x,y centre co-ordinates and radius
const long MDrw_Arc3   = 0x80000003; //curve: set of 3 x,y co-ordinates
const long MDrw_End    = 0x80000004; //end: end of graphics symbol definition

/*A drawing symbol is defined arround the origin 0,0.
Examples:
long Drw_SimpleHeater[] = { MDrw_Arc, 0,0, 5,
MDrw_Poly, -5,0, -3,0, -3,-2, 3,2, 3,0, 5,0,
MDrw_Poly, -5,1, -6,1, -6,0, -5,0, -5,1,
MDrw_Poly, 5,-1, 6,-1, 6,0, 5,0, 5,-1,
MDrw_End };
long Drw_FlashTank[] = { MDrw_Poly, -5,10, -5,-5, 0,-10, 5,-5, 5,10,
MDrw_Poly, -5,-2, 5,-2,
MDrw_Arc3, -5,10, 0,13, 5,10,
MDrw_End }; 
*/
// ======================================================================
//
//
//
// ======================================================================

class TagObjClass;   // Forward define for SMDK - used in SpModel

class MInitialiseTest
  {
  public:
    MInitialiseTest(TagObjClass * C)  { m_pObjClass=C; };
    MInitialiseTest(LPCTSTR Name)     { m_pObjClass=NULL; m_sName=Name; };
    ~MInitialiseTest()                { };

    TagObjClass * ObjClass()          { return m_pObjClass; };
    LPCTSTR       Name()              { return m_sName; };

  protected :
    TagObjClass * m_pObjClass;
    CString       m_sName;
  };

// ======================================================================
//
//
//
// ======================================================================

class MArray;
class MPropertyInfo; 
class MPropertyValue; 
class SpQuality;     
class DllImportExport MSpQualityBase : public MSubConstructBase
  {
  friend class CCall2MSpQuals;
  friend class MVector;
  public:
    MSpQualityBase() { m_pSpQual=NULL; };
    virtual ~MSpQualityBase() {};

    virtual void   ExchangeSpecies(MSpQualityBase * Other) =0;

    virtual bool   EquilRqd4HEval() { return false; };
    virtual void   Equilibrate() {};

    virtual void   ZeroMass()=0;
    virtual void   ZeroDeriv()=0;
    virtual void   ScaleMass(long Phase, double Mult)=0;
    virtual void   ScaleMass(CArray<int,int> &SpIds, double Mult)=0;

    virtual void   SetMassF(MSpQualityBase * QualSet)=0;
    virtual void   AddMassF(MSpQualityBase * QualAdd, MArray & MAdd)=0;
    virtual void   SubMassF(MSpQualityBase * QualSub, MArray & MSub)=0;

    virtual void   Copy(MSpQualityBase * QualCopy)=0;
    virtual void   AddDeriv(MSpQualityBase * Qual2, double Sgn_)=0;
    virtual void   AddDiscrete(MSpQualityBase * Qual2, double Sgn_)=0;

    virtual long    DefinedPropertyCount()                                          { return 0; };
    virtual long    DefinedPropertyInfo(long Index, MPropertyInfo & Info)           { return -1; };

    virtual DWORD   GetPropertyVisibility(long Index)                               { return ePVis_All; };
    virtual void    GetPropertyValue(long Index, ULONG Phase, double T, double P, MPropertyValue & Value) {};
    virtual void    PutPropertyValue(long Index, MPropertyValue & Value)            {};

  private:
    SpQuality     * m_pSpQual;

  };

// ======================================================================
//
//
//
// ======================================================================


#undef DllImportExport

#endif // !defined(MD_SHARE3_H)
