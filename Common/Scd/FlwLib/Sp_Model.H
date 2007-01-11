//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SP_MODEL_H
#define  __SP_MODEL_H

#include "sc_defs.h"
#include "sp_db.h"
#include "odesolve.h"
#include "tagobj.h"
#include "vectors.h"
#include "scdarray.h"
#include "eqnsolve.h"
#include "executiv.h"
#include "odesolve.h"
#include "sp_shape.h"
#include "propertybase.h"
#include "..\..\..\SMDK\Include\md_alumina.h"
#include "..\..\..\SMDK\Include\md_vector.h"
#include "..\..\..\SMDK\Include\md_spmodel.h"

#ifdef __SP_MODEL_CPP
#define DllImportExport DllExport
#elif !defined(FLWLIB)
#define DllImportExport DllImport
#else
#define DllImportExport
#endif

// ===========================================================================

#define WITHSPMDLTIMERS WITHTIMERS
#if WITHSPMDLTIMERS
#define SPMDLTIMERLAP(x) CStopWatchLap SW##__LINE__(SpModel::x)
#define SPMDLTIMESTUFF(x) x;
//  #define SPMARRAYTIMERLAP(x) CStopWatchLap SW##__LINE__(CSysVector::x)
//  #define SPMVECTORTIMERLAP(x) CStopWatchLap SW##__LINE__(CSysVector::x)
#else
#define SPMDLTIMERLAP(x)
#define SPMDLTIMESTUFF(x)
//  #define SPMARRAYTIMERLAP(x)
//  #define SPMVECTORTIMERLAP(x)
#endif

// ===========================================================================

#define SpModelDefTag "Model"

// ===========================================================================
//
//
//
// ===========================================================================

_FWDDEF(SpMArray);            // Array Of Species - MaxSpecies
_FWDDEF(SpVector);            // Array Of Species - Based on CDVector
_FWDDEF(SpQuality);           // A Quality Of a Specie
_FWDDEF(SpModel);             // Model of Specie behaviour in Container
_FWDDEF(SpModelOwner);        // Contains Pointers to Model(s) and (optionally) a Shape
class CVLEBase;

class CCall2MSpQuals;

// ===========================================================================

enum SpMdlUsage { SPMU_Null, SPMU_Mass, SPMU_Flow, SPMU_Image };

const int MaxSpQualities = 16;

XID xidSpShape          = SpcXID(100000);
XID xidSpModel          = SpcXID(100001);
XID xidSpModelClass     = SpcXID(100002);
XID xidSpMdlFidelity    = SpcXID(100003);
XID xidSMPress          = SpcXID(100004);
XID xidSMTemp           = SpcXID(100005);
XID xidSMViewGlbl       = SpcXID(100006);
XID xidSMView           = SpcXID(100007);
XID xidSMViewL          = SpcXID(100008);
XID xidSMViewG          = SpcXID(100009);
XID xidSMViewBasisMass  = SpcXID(100010);
XID xidSMViewBasisElem  = SpcXID(100011);

XID xidSMTMass          = SpcXID(100012);
XID xidSMSMass          = SpcXID(100013);
XID xidSMLMass          = SpcXID(100014);
XID xidSMVMass          = SpcXID(100015);
XID xidSMSLMass         = SpcXID(100016);
XID xidSMDeltaMass      = SpcXID(100017);

XID xidApplyCfgTemplate = SpcXID(100018);
XID xidSaveCfgTemplate  = SpcXID(100019);

XID xidSMTVol           = SpcXID(100020);
XID xidSMSVol           = SpcXID(100021);
XID xidSMLVol           = SpcXID(100022);
XID xidSMVVol           = SpcXID(100023);
XID xidSMSLVol          = SpcXID(100024);

XID xidSMTNVol          = SpcXID(100025);
XID xidSMSNVol          = SpcXID(100026);
XID xidSMLNVol          = SpcXID(100027);
XID xidSMVNVol          = SpcXID(100028);
XID xidSMSLNVol         = SpcXID(100029);

XID xidSMRho            = SpcXID(100030);
XID xidSMSRho           = SpcXID(100031);
XID xidSMLRho           = SpcXID(100032);
XID xidSMVRho           = SpcXID(100033);
XID xidSMSLRho          = SpcXID(100034);
XID xidSMMoleWt         = SpcXID(100035);
XID xidSMCpCv           = SpcXID(100036);

XID xidSMSFrac          = SpcXID(100040);
XID xidSMLFrac          = SpcXID(100041);
XID xidSMVFrac          = SpcXID(100042);
XID xidSMMlSFrac        = SpcXID(100043);
XID xidSMMlLFrac        = SpcXID(100044);
XID xidSMMlVFrac        = SpcXID(100045);

XID xidSMPureSatTemp    = SpcXID(100050);
XID xidSMSatTemp        = SpcXID(100051);
XID xidSMSatPress       = SpcXID(100052);
XID xidSMBoilPtEle      = SpcXID(100053);
XID xidSMSatComp        = SpcXID(100054);

XID xidSMSmsH           = SpcXID(100060);
XID xidSMLmsH           = SpcXID(100061);
XID xidSMVmsH           = SpcXID(100062);
XID xidSMSLmsH          = SpcXID(100063);
XID xidSMSmsCp          = SpcXID(100064);
XID xidSMLmsCp          = SpcXID(100065);
XID xidSMVmsCp          = SpcXID(100066);
XID xidSMSLmsCp         = SpcXID(100067);

XID xidSMmsLatHtVap     = SpcXID(100070);
XID xidSMUsage          = SpcXID(100071);

XID xidSMmsH            = SpcXID(100075);
XID xidSMmsHz           = SpcXID(100076);
XID xidSMmsHf           = SpcXID(100077);
XID xidSMmsHf0          = SpcXID(100078);
XID xidSMmsCp           = SpcXID(100079);
XID xidSMmsCp0          = SpcXID(100080);
XID xidSMmsCp0T         = SpcXID(100081);
XID xidSMmsCpUt         = SpcXID(100082);
XID xidSMmsCpUtT        = SpcXID(100083);
XID xidSMtotH           = SpcXID(100084);
XID xidSMtotHz          = SpcXID(100085);
XID xidSMtotHf          = SpcXID(100086);
XID xidSMtotHf0         = SpcXID(100087);
XID xidSMtotHAtmP       = SpcXID(100088);
XID xidSMtotHSatP       = SpcXID(100089);

XID xidSMtotHUt         = SpcXID(100090);
XID xidSMtotHzUt        = SpcXID(100091);
XID xidSMtotHfUt        = SpcXID(100092);

XID xidSMmsSf           = SpcXID(100093);
XID xidSMmsSf0          = SpcXID(100094);
XID xidSMtotSf          = SpcXID(100095);
XID xidSMtotSf0         = SpcXID(100096);

XID xidSMtotHsFn        = SpcXID(100097); 
XID xidSMtotHzFn        = SpcXID(100098); 
XID xidSMtotHfFn        = SpcXID(100099); 

XID xidSSVolume         = SpcXID(100100);
XID xidSSVolumeB        = SpcXID(100101);
XID xidSSVapVolScale    = SpcXID(100102);
XID xidSSArea           = SpcXID(100103);
XID xidSSAreaB          = SpcXID(100104);
XID xidSSVPt1           = SpcXID(100105);
XID xidSSPPt1           = SpcXID(100106);
XID xidSSVPt2           = SpcXID(100107);
XID xidSSPPt2           = SpcXID(100108);

XID xidSMFlashComp      = SpcXID(100117);

XID xidEquilSwitch      = SpcXID(100118);

XID xidOvrAnyOn         = SpcXID(100119);
XID xidOvrVDens         = SpcXID(100120);
XID xidOvrLDens         = SpcXID(100121);
XID xidOvrSDens         = SpcXID(100122);
XID xidOvrVCp           = SpcXID(100123);
XID xidOvrLCp           = SpcXID(100124);
XID xidOvrSCp           = SpcXID(100125);
XID xidOvrVDynVisc      = SpcXID(100126);
XID xidOvrLDynVisc      = SpcXID(100127);
XID xidOvrVThermCd      = SpcXID(100128);
XID xidOvrLThermCd      = SpcXID(100129);
XID xidOvrSurfT         = SpcXID(100130);
XID xidOvrBPEle         = SpcXID(100131);

XID xidRqdVDens         = SpcXID(100132);
XID xidRqdLDens         = SpcXID(100133);
XID xidRqdSDens         = SpcXID(100134);
XID xidRqdVVisc         = SpcXID(100135);
XID xidRqdLVisc         = SpcXID(100136);
XID xidRqdSurfT         = SpcXID(100137);
XID xidRqdBPEle         = SpcXID(100138);

XID xidVVisc            = SpcXID(100140);
XID xidLVisc            = SpcXID(100141);
XID xidDVisc            = SpcXID(100142);
XID xidVKVisc           = SpcXID(100143);
XID xidLKVisc           = SpcXID(100144);
XID xidVThermCond       = SpcXID(100145);
XID xidLThermCond       = SpcXID(100146);
XID xidVPrandtl         = SpcXID(100147);
XID xidLPrandtl         = SpcXID(100148);
XID xidLSurfT           = SpcXID(100149);

XID xidSpUseFlgs        = SpcXID(100150);

XID xidRhoSoln1         = SpcXID(100151);
XID xidRhoSolnLast      = SpcXID(100159);

XID xidSpHeatValueHiM     = SpcXID(100160);
XID xidSpHeatValueHiMl    = SpcXID(100161);
XID xidSpHeatValueHiV     = SpcXID(100162);
XID xidSpHeatValueHiNV    = SpcXID(100163);
XID xidSpHeatValueLoM     = SpcXID(100164);
XID xidSpHeatValueLoMl    = SpcXID(100165);
XID xidSpHeatValueLoV     = SpcXID(100166);
XID xidSpHeatValueLoNV    = SpcXID(100167);
XID xidSpHeatValueHiM25   = SpcXID(100170);
XID xidSpHeatValueHiMl25  = SpcXID(100171);
XID xidSpHeatValueHiV25   = SpcXID(100172);
XID xidSpHeatValueHiNV25  = SpcXID(100173);
XID xidSpHeatValueLoM25   = SpcXID(100174);
XID xidSpHeatValueLoMl25  = SpcXID(100175);
XID xidSpHeatValueLoV25   = SpcXID(100176);
XID xidSpHeatValueLoNV25  = SpcXID(100177);

XID xidSpEqnOfState         = SpcXID(100180);
XID xidSpMixtureRule        = SpcXID(100181);
XID xidSpMixtureAlpha       = SpcXID(100182);
XID xidSpMixtureTc          = SpcXID(100183);
XID xidSpMixturePc          = SpcXID(100184);
XID xidSpMixtureVc          = SpcXID(100185);
XID xidSpMixtureAc          = SpcXID(100186);
XID xidSpdHIdeal            = SpcXID(100187);
XID xidSpdSIdeal            = SpcXID(100188);
XID xidSpdHResidual         = SpcXID(100189);
XID xidSpdSResidual         = SpcXID(100190);

XID xidAutoVLEComp          = SpcXID(100200);
XID xidAutoVLESet           = SpcXID(100201);
XID xidAutoVLESpecH         = SpcXID(100202);
XID xidAutoVLERelH          = SpcXID(100203);
XID xidAutoVLEDewPt         = SpcXID(100204);

XID xidHumiditySpecific     = SpcXID(100205);
XID xidHumidityRelative     = SpcXID(100206);
XID xidHumidityDewPt        = SpcXID(100207);

XID xidSMFnRanges           = SpcXID(100208);
XID xidSMFnRangeID          = SpcXID(100209);

XID xidSpMdlVarsAvail       = SpcXID(100300); // leave 32
XID xidSpMdlVarsAvailEnd    = SpcXID(100332); 
XID xidSpMdlSpVarsAvail     = SpcXID(100333); // leave 32
XID xidSpMdlSpVarsAvailEnd  = SpcXID(100365); 

XID xidQualSwitch           = SpcXID(200000);   //xidAttribute      + MaxAttributes;            // ditto
XID xidQualSwitch1          = xidQualSwitch     + MaxSpQualities;           // ditto
XID xidQualSwitch2          = xidQualSwitch1    + MaxSpQualities;           // ditto
XID xidSpcEndOfList         = xidQualSwitch2;                               // ditto

XID xidSpUsrProps           = SpcXID(700000);
XID xidSpUsrPropsLast       = SpcXID(799999);
XID xidSpUsrQuals           = SpcXID(800000);
XID xidSpUsrQualsLast       = SpcXID(899999);

// ===========================================================================

enum eSetMass { SetMass_Frac, SetMass_MassFlow };

// ===========================================================================
//
//
//
// ===========================================================================
// Base Class for Specie TagObjClass definitions

class DllImportExport SQTagObjClass : public TagObjClass
  {
  public:
    SQTagObjClass(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar SubClassId, pchar pVersion_, pchar pDefTag_, dword dwCat, pchar ShortDesc, pchar pDesc, DWORD SelectMask) : \
      TagObjClass(pClassName, pGroup_, pClassId_, SubClassId, pVersion_, "", pDefTag_, dwCat, ShortDesc, pDesc, SelectMask)
      {};

    virtual int NTearVariables()=0;
    virtual int DefineTearVariables(TearVarArray & TV, int n) { return NTearVariables(); };
  };

// ===========================================================================

#define DEFINE_QUALITYPTR(Obj, Tg)                                                  \
  static p##Obj Ptr(SpModel * p, flag NewIt=true)                                   \
    {                                                                               \
    int i=Obj##Class.GroupIdNo();                                                   \
    VERIFY(i<MaxSpQualities);                                                       \
    if (!p->Quals[i].Exists() && NewIt)                                             \
      {                                                                             \
      p->Quals[i].m_pQ=(p##Obj)Obj##Class.Construct(NULL, Tg, p, TOA_Embedded);     \
      p->Quals[i].m_pQP=p->GetQualityPersonality(p->Quals[i].m_pQ);                 \
      p->nQuals=Max(p->nQuals, (byte)(i+1));                                        \
      };                                                                            \
    return (p##Obj)(p->Quals[i].m_pQ);                                              \
    };                                                                              \
  static p##Obj Kill(SpModel * p)                                                   \
    {                                                                               \
    int i=Obj##Class.GroupIdNo();                                                   \
    VERIFY(i<MaxSpQualities);                                                       \
    if (p->Quals[i].Exists())                                                       \
      {                                                                             \
      delete p->Quals[i].m_pQ;                                                      \
      p->Quals[i].m_pQ=NULL;                                                        \
      delete p->Quals[i].m_pQP;                                                     \
      p->Quals[i].m_pQP=NULL;                                                       \
      if (p->nQuals==(byte)(i+1))                                                   \
        p->nQuals--;                                                                \
      };                                                                            \
    return (p##Obj)(p->Quals[i].m_pQ);                                              \
    };                                                                              \
  static p##Obj FindQual(SpModel * p) { return Ptr(p, false); };                    \
  static p##Obj CreateQual(SpModel * p) { return Ptr(p, true); };                   \
  static p##Obj KillQual(SpModel * p) { return Kill(p); };

class MSpQualityBase; // forward;

class DllImportExport SpQuality : public TaggedObject, public CPropertyBase, public MXSpQuality
  {
  public:
    SpQuality(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SpQuality();

    SpMdlUsage Usage();
    flag SQEditable();
    flag SQSource();
    flag SQTearSet();
    flag RemoveMe();
    void SetRemoveMe(flag On);
    flag HoldCopy();
    void SetHoldCopy(flag On);

    flag SQCreator();
    void SetSQCreator(flag On);

    virtual MSpQualityBase  * GetMSpQualityBase4Cast()  { return NULL; };

    virtual void   BuildDataDefn(DataDefnBlk & DDB)=0;
    //Does Nothing: virtual void   BuildDataDefn_Lcl(DataDefnBlk & DDB) {};
    virtual flag   DataXchg(DataChangeBlk & DCB);

    virtual flag   GetOtherData(FilingControlBlock &FCB){ return false;};
    virtual flag   PutOtherData(FilingControlBlock &FCB){ return false;};

    virtual void   ExchangeSpecies(SpQuality* Other);

    virtual flag   EquilRqd4HEval() { return false; };
    virtual void   Equilibrate();

    virtual void   ZeroMass()=0;
    virtual void   ZeroDeriv()=0;
    virtual void   ScaleMass(PhMask Phase, double Mult)=0;
    virtual void   ScaleMass(CIArray & SpIds, double Mult)=0;

    virtual void   SetMassF(CSysVector & M1, SpQuality* pQual2)=0;
    virtual void   AddMassF(CSysVector & M1, SpQuality* pQual2, CSysVector & M2)=0;
    virtual void   SubMassF(CSysVector & M1, SpQuality* pQual2, CSysVector & M2)=0;

    virtual void   Copy(SpModel * pMdl2, SpQuality* pQual2)=0;
    virtual void   AddDeriv(SpModel * pMdl2, SpQuality* pQual2, double Sgn_)=0;
    virtual void   AddDiscrete(SpModel * pMdl2, SpQuality* pQual2, double Sgn_)=0;
    virtual void   ODEOperate(CODEDataBlock & ODB)=0;

    // Extras for Tear Management;
    virtual void   TearGetInputs(TearVarArray & TV, int n) {};
    virtual void   TearGetOutputs(TearVarArray & TV, int n) {};
    virtual void   TearSetOutputs(TearVarArray & TV, int n) {};

    // Debug Stuff
    virtual void   Dump() {};
    virtual void   SetDebug(flag DbgOn_);

    // CPropertyBase override
    virtual LPCTSTR DefinedPropertyMapName()                              { return ClassId(); };
    virtual long    DefinedPropertyCount();
    virtual long    DefinedPropertyInfo(long Index, MPropertyInfo & Info);

    virtual DWORD   GetPropertyVisibility(long Index) throw(...);
    virtual void    GetPropertyValue(long Index, DWORD Phase, double T, double P, MPropertyValue &Value) throw(...);
    virtual void    PutPropertyValue(long Index, MPropertyValue &Value) throw(...);

  public:
    static const pchar GroupName;
    flag              m_bRemoveMe;
    flag              m_bHoldCopy;
    int               m_iQualIndex;
    SpModel         * pModel;

  };

inline void SpQuality::BuildDataDefn(DataDefnBlk & DDB) { };
inline flag SpQuality::DataXchg(DataChangeBlk & DCB) {return TaggedObject::DataXchg(DCB); };
inline void SpQuality::ExchangeSpecies(SpQuality* Other) {};
inline void SpQuality::Equilibrate() {};
inline void SpQuality::ZeroMass() {};
inline void SpQuality::ZeroDeriv() {};
inline void SpQuality::ScaleMass(PhMask Phase, double Mult) {};
inline void SpQuality::ScaleMass(CIArray & SpIds, double Mult) {};
inline void SpQuality::SetDebug(flag DbgOn_) {};

// ===========================================================================
//
//
//
// ===========================================================================

enum QPF_Modes { QPF_Error, QPF_Sensible, QPF_Condensing, QPF_Boiling };

// Specie Validity Flags

const DWORD SMVF_UserModel     = 0x000000ff;
const DWORD SMVF_UserQuality   = 0x0000ff00;

const DWORD SMVF_M_OK          = 0x00010000;
const DWORD SMVF_H_OK          = 0x00020000;
const DWORD SMVF_UnModified    = 0x00040000;

const DWORD SMVF_EquilOK       = 0x00400000;

// ===========================================================================

enum SPMTests {SPMT_Individual, SPMT_Total};
enum SPMEnthStrat {SPMES_BestTemp, SPMES_ConserveH };
enum SPMMixStrat {SPMMS_PreBld35, SPMMS_Bld35 };

// ===========================================================================

struct CSpMdlDerivCtrlFlw
  {
  SpModel * m_pMdl;
  int       m_iSgn;
  CSpMdlDerivCtrlFlw()
    {
    m_pMdl=NULL;
    m_iSgn=0;
    };
  CSpMdlDerivCtrlFlw(SpModel * pMdl, int Sgn)
    {
    m_pMdl=pMdl;
    m_iSgn=Sgn;
    };
  };

class DllImportExport CSpMdlDerivCtrl
  {
  public:
    flag         m_fStarted;
    flag         m_fMassOK;
    SpModel   *  m_pFd;
    SpModel   *  m_pPr;
    SpModel   *  m_pCn;
    SpVector  *  m_pDM;
    double       m_dDuty;

    long         m_nFlws;
    CArray<CSpMdlDerivCtrlFlw, CSpMdlDerivCtrlFlw> m_Flws;

    SPMTests     m_eIntTests;

    double       m_V_TestValue;       // Values Used to Test Integration;
    double       m_SL_TestValue;

    CSpMdlDerivCtrl();
    ~CSpMdlDerivCtrl();
    void Clear();
    void Start(SpModel * pMdl);
    void Finalise(SpModel * pMdl, double dTime);

    void Save(SpModel * pMdl);
    void Restore(SpModel * pMdl);
    void Estimate(SpModel * pMdl, double dTime);
  };

// ===========================================================================

class CSpView
  {
  friend class SpModel;
  protected:
    flag         m_bOn;
    SV_View      m_iView;
    SV_ViewBasis m_iViewBasis;

  public:
    CSpView(SV_View View=SVV_AsMass)
      {
      m_bOn=false;
      m_iView=View;//SVV_AsMass;
      m_iViewBasis=SVVB_All;
      };
  };

// ===========================================================================
// Used to setup BuildDataDefn_Species
struct BDDSpecies_Blk
  {
  SV_View   m_iView;
  SV_View   m_iFileView;
  flag      m_bAsParm;
  flag      m_bIsRoot;
  long      m_VAMsk;
  };

// ===========================================================================

enum SQActions { SQA_Xfer=0, SQA_Create=1, SQA_Remove=2 };

const byte SQF_Editable    = 0x01;
const byte SQF_Source      = 0x02;
const byte SQF_TearSet     = 0x04;
const byte SQF_CreateOK    = 0x10;
const byte SQF_RemoveOK    = 0x20;
const byte SQF_CreateDsp   = 0x40;
const byte SQF_RemoveDsp   = 0x80;

class DllImportExport SpQPersonality
  {
  protected:
    SpModel     *m_pModel;
  public:
    SpQPersonality(SpModel *pModel)  { m_pModel = pModel; };
    virtual void   BuildDataDefn(DataDefnBlk & DDB)=0;
    virtual flag   DataXchg(DataChangeBlk & DCB) { return 0; };
  };

class SpQualInfo
  {  
  protected:
  public:
    SpQuality       *m_pQ;
    SpQPersonality  *m_pQP;
    byte             m_iAct;
    SpQualInfo()
      {
      m_pQ=NULL;
      m_pQP=NULL;
      m_iAct=SQA_Xfer;
      };
    flag CopyReqd(SpModel * pMdl);
    bool Exists() { return m_pQ!=NULL; };
  };

// ===========================================================================

const int CSpMdlSlctMax = 16;
class DllImportExport CSpMdlSlct
  {
  friend class SpModel;
  friend class SpModelOwner;
  protected:
    int       N;
    int       iQMax;
    SpModel * Mdls[CSpMdlSlctMax];
    double    Qms[CSpMdlSlctMax];

  public :
    CSpMdlSlct() { N=0; iQMax=-1; };
    void          Add(SpModel * Mdl, double Qm);
    TagObjClass*  RequiredMdlClass();
    SpModel *     RequiredMdl();
  };

// ===========================================================================

#define QUALITY_TYPECAST()  template <class C> operator C*() { return C::Ptr(this, false); };

// ===========================================================================

const long FUS_Null           = 0;
const long FUS_Largest        = 1;
const long FUS_BottomUp       = 2;
const long xFUS_TopDown       = 3;
const long xFUS_BelowCurrent  = 4;

const byte AVLESet_None       = 0;
const byte AVLESet_SpecH      = 1;
const byte AVLESet_RelH       = 2;
const byte AVLESet_DewPt      = 3;
const byte AVLESet_Saturated  = 4;
const byte AVLESet_SuperSat   = 5;

// ===========================================================================

class CSpUseArray : public CArray<bool,bool> {} ;

struct DllImportExport CSpMCfgImg
  {
  double   m_Value;
  //PhMask   m_PhMask;
  };
struct DllImportExport CVDS_WorkSpace
  {
  bool        m_bKeepNormal;
  bool        m_bAdjustScale;
  byte        m_Op;
  SpMArray    m_SclX, m_MTmp;
  bool        m_IsAmount[MaxSpecies + MaxAttributes];
  int         m_MTmpPh0, m_MTmpPhN;
  int         m_CalcPh0, m_CalcPhN;
  double      m_TheTemp;
  double      m_ThePress;
  };

#define DEFINE_SPMODEL(SM) DEFINE_TAGOBJ(SM)

#define IMPLEMENT_SPMODEL(Obj, ModelId, Version, Cat, SDesc, LDesc) \
  IMPLEMENT_TAGOBJ(Obj, SpModel::GroupName, ModelId, Version, "", "", Cat, SDesc, LDesc)

DEFINE_SPMODEL(SpModel)
class DllImportExport SpModel : public TaggedObject, public CPropertyBase, public MXSpModel
  {
  friend class SpMArray;
  friend class SpVector;
  friend class SpQuality;
  friend class SpContainer;
  friend class SpConduit;
  friend class SpImage;
  friend class SpShape;
  friend class CSpMdlDerivCtrl;
  friend class CSpContPreset;

  public:
    SpModel(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SpModel();

    virtual bool   IsBaseClassOf(SpModel * pOther) { return (SpModel::iSpMdlMixStrategy==SPMMS_PreBld35) ? false : ((dynamic_cast<SpModel*>(pOther)) != NULL); };
    virtual SpModel * PreferredModel(SpModel * pOther) { return this ; };

    void           SetUsage(SpMdlUsage As)  { m_iUsage=As; };
    SpMdlUsage     Usage()                  { return m_iUsage; };
    flag           UseAsFlow()              { return m_iUsage==SPMU_Flow; };
    flag           UseAsMass()              { return m_iUsage==SPMU_Mass; };
    flag           UseAsImage()             { return m_iUsage==SPMU_Image; };

    void           SetSQFlags(byte Flags, flag On) { bSQFlags = On ? bSQFlags|Flags : bSQFlags &(~Flags); };
    flag           SQFlagsSet(byte Flags)   { return (bSQFlags & Flags) !=0; };

    void           ShapeChanged()           { ClrStatesOK(); };

    static long    Fidelity()               { return m_iFidelity; };
    static void    SetFidelity(long F)      { m_iFidelity=F; };

    static long    FixupStrategy()          { return sm_iFixupStrategy; };
    static void    SetFixupStrategy(long F) { sm_iFixupStrategy=F; };

    // Quality Methods
    void           Scan4EquilRqd4HEval();
    void           SetQualityPresent(int q, flag Present);

    SpQuality*     QualityPtr(int No, flag NewIt=true);
    SpQuality*     QualityPtr(char* RqdQuality, flag NewIt=true);
    void           QualityRemove(int No);
    virtual SpQPersonality *GetQualityPersonality(SpQuality* pQ) { return NULL; };  // ask Spmodel for the personality

    SpQuality*     FindQuality(int No) { return QualityPtr(No, false);};
    SpQuality*     CreateQuality(int No) { return QualityPtr(No, true);};
    void           KillQuality(int No) { QualityRemove(No);};

    void           QualitiesSetState(eScdMdlStateActs RqdState);
    void           CopyQualities(SpModel * SrcModel, int q0=-1, int q1=-1);
    void           SetQualities(SpModel * SrcModel);
    void           AddQualities(SpModel * SrcModel, CSysVector & Cm);
    void           DumpQualities();
    void           EquilibrateAll(double RqdT=dNAN, double RqdP=dNAN);

    QUALITY_TYPECAST();

    // ---------
    static flag    BuildDataDefnGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataDefnBlk & DDB);
    static flag    DataXchgGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataChangeBlk & DCB);

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual void   BuildDataDefn_Vars(DataDefnBlk & DDB);
    virtual void   BuildDataDefn_UsrProps(DataDefnBlk & DDB, ULONG WhichProps=MP_UserProp, flag AsParms=true);
    virtual void   BuildDataDefn_RqdProps(DataDefnBlk & DDB);

    void           AddSpView(DataDefnBlk & DDB, const BDDSpecies_Blk * Blks, int nBlk, /*byte RqdVw,*/ DDEF_Flags Flags);
    void           AddSpViewBasis(DataDefnBlk & DDB, const BDDSpecies_Blk * Blks, int nBlk, SV_View Vw, DDEF_Flags Flags);
    virtual void   BuildDataDefn_AvailVars(DataDefnBlk & DDB);
    virtual void   BuildDataDefn_Species(DataDefnBlk & DDB, const BDDSpecies_Blk * Blks, size_t BlkSize);
    virtual DDEF_Flags FixSpEditFlags(DDEF_Flags Flags, int iSpid, flag AsParms) { return (Flags&!(isParm|isResult)) | AsParms ? isParm : isResult; };
    flag           DataXchg_Species(DataChangeBlk & DCB);
    flag           ValidateData_Species(ValidateDataBlk & VDB);

    virtual flag   Has_Quals();
    virtual void   BuildDataDefn_Quals(DataDefnBlk & DDB, flag AsParms);
    virtual void   BuildDataDefn_QualsSlct(DataDefnBlk & DDB, flag AsParms);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    void           SetCfgImage(SV_View SetView, SV_ViewBasis ViewBasis, int Index, double Value);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual long   CheckDataRanges();

    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);
    void           CopyPutOtherData(SpModel & Src);

    void           FindTFromHAtP(double ThePress);
    virtual void   EvalMStates();
    virtual void   EvalHStates();

    void           SetHoldEquilibrate()                   { m_iHoldEquilibrate++;};
    void           ClrHoldEquilibrate(bool DoEquil=true)  { m_iHoldEquilibrate--; if (DoEquil) EquilibrateAll(); };
    void           SetHoldVentExpand() { m_iHoldVentExpand++;};
    void           ClrHoldVentExpand() { m_iHoldVentExpand--;};
    void           SetVentExpandOK()   { m_bVentExpandOK=true;};
    void           ClrVentExpandOK()   { m_bVentExpandOK=false;};

    flag           IsEmpty() { return !m_bUsableMass; };

#define FlagsBad(x) ((m_dwValidFlags ^ (x)) & (x))
    void           TestMStatesOK() { SPMDLTIMESTUFF(sm_lEvalMCalls++); if (FlagsBad(SMVF_M_OK)) EvalMStates(); };
    void           TestHStatesOK() { SPMDLTIMESTUFF(sm_lEvalHCalls++); if (FlagsBad(SMVF_M_OK|SMVF_H_OK)) EvalHStates(); };
#undef  FlagsBad
    void           EvalDerivedStates() { TestHStatesOK(); };

    void           ClrMStatesOK() { m_dwValidFlags &= ~(SMVF_M_OK|SMVF_UserModel|SMVF_UserQuality|SMVF_EquilOK); m_dwMassVersionNo++; };
    void           ClrHStatesOK() { m_dwValidFlags &= ~(SMVF_H_OK|SMVF_UserModel|SMVF_UserQuality|SMVF_EquilOK); };
    void           ClrStatesOK()  { m_dwValidFlags = 0; m_dwMassVersionNo++; };
    bool           Modified()     { return (m_dwValidFlags & SMVF_UnModified)==0; };
    void           ClrModified()  { TestHStatesOK(); m_dwValidFlags |= SMVF_UnModified; };

    void           SetMaxLevel(double MaxLevel);
    double         MaxLevel() { return m_dMaxLevel; };
    void           SetMaxPressure(double Press);
    double         MaxPressure() { return m_dMaxPress; };
    void           SetMassScale(double Scl, LPCTSTR DbgStr="");
    double         MassScale() { return m_dMassScale; };
    void           SetDataSign(int Sgn);
    signed char    DataSign() { return m_iDataSgn; };
    void           SetNegDataOK(bool OK) { m_bNegDataOK=OK; };
    bool           NegDataOK() { return m_bNegDataOK; };

    virtual void   SetMixed(byte Mixed) { m_bMixed=Mixed;};
    virtual byte   Mixed() { return m_bMixed;};

    virtual void   SetClosed(byte Closed) { ClrHStatesOK(); m_bClosed=Closed; };
    virtual byte   Closed() { return m_bClosed;};

    virtual void   SetPressAsSet(byte PressAsSet) { ClrHStatesOK(); m_bPressAsSet=PressAsSet; };
    virtual byte   PressAsSet() { return m_bPressAsSet;};

    virtual SPMTests IntegrationTests() { return m_DC.m_eIntTests; };
    virtual void   SetIntegrationTests(SPMTests T);

    virtual void   SetStateAction(IE_Enables E);
    virtual void   ExchangeSpecies(SpModel * Other);

    void SetBadTempOK(flag OK) { if (OK) m_bBadTempOK++; else m_bBadTempOK--; };
    flag BadTempOK()           { return m_bBadTempOK>0; };

    // User definable Properties

    virtual double RefTemp() { return C_2_K(0.0); };

    virtual double CpCv(PhMask Phase=som_ALL)       { return m_M.CpCv(m_iFidelity, Phase); };

    virtual MSMFnRanges * SMFnRanges() { return NULL; };
    virtual double msCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);
            double msCp(PhMask Phase=som_ALL)       { return msCp(Phase, Temp(), Press()); };
    virtual double msHs(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);
            double msHs(PhMask Phase=som_ALL)     { return msHs(Phase, Temp(), Press());};
    virtual double msHz(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);
            double msHz(PhMask Phase=som_ALL)     { return msHz(Phase, Temp(), Press());};
    virtual double msHf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);
            double msHf(PhMask Phase=som_ALL)     { return msHf(Phase, Temp(), Press()); };
    virtual double msSf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);
            double msSf(PhMask Phase=som_ALL)     { return msSf(Phase, Temp(), Press()); };
            double msHfb0(PhMask Phase, CSysVector * pMA=NULL);
            double msH_DHfb0(PhMask Phase, CSysVector * pMA=NULL);

    virtual double totCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
            double totCp(PhMask Phase=som_ALL)    { return totCp(Phase, Temp(), Press()); };
    virtual double totHs(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
            double totHs(PhMask Phase=som_ALL)    { return totHs(Phase, Temp(), Press());};
    virtual double totHz(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
            double totHz(PhMask Phase=som_ALL)    { return totHz(Phase, Temp(), Press());};
    virtual double totHf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
            double totHf(PhMask Phase=som_ALL)    { return totHf(Phase, Temp(), Press()); };
    virtual double totSf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
            double totSf(PhMask Phase=som_ALL)    { return totSf(Phase, Temp(), Press()); };
            double totHfb0(PhMask Phase, CSysVector * pMA=NULL);
            double totH_DHfb0(PhMask Phase, CSysVector * pMA=NULL);

    virtual double msLatentHeatVap(double T, double P, CSysVector * pMA=NULL);

    void   KeepTemp();
    void   SetFileTemp(double T); // for filing
    void   SetFilePress(double P); // for filing

    virtual double Temp();
    virtual double dTemp();
    virtual double Press();
    virtual double PartialPress(SpecieIter & Ids, double Temp);
    virtual double PartialPress(int Id, double Temp);
    virtual double PartialPressFrac(SpecieIter & Ids, double Temp);
    virtual double PartialPressFrac(int Id, double Temp);

    // Properties
    double          getVValue(int s)                              { return m_M.VValue[s]; };
    double          getVMass(int s)                               { return m_M.VMass[s]; };
    double          getVMole(int s)                               { ASSERT(s>=0 && s<SVSpcCount()); return m_M.VMass[s]/SDB[s].MoleWt(); };
    double          getVAttr(int s)                               { return m_M.VAttr[s]; };
    double          getVCalc(int s)                               { return m_M.VCalc[s]; };
    double        * getVPtr(int s)                                { return m_M.VPtr[s]; };

    void            SetVValue(int i, double Val)                  { KeepTemp(); m_M.SetVValue(i, Val); ClrStatesOK(); };
    void            SetVMass(int i, CSysVector &Vec, double Val)  { KeepTemp(); m_M.SetVMass(i, Vec, Val);  ClrStatesOK(); };
    void            AddVMass(int i, CSysVector &Vec, double Val)  { KeepTemp(); m_M.AddVMass(i, Vec, Val); ClrStatesOK(); };
    void            SclVMass(int i, double Scl)                   { KeepTemp(); m_M.SclVMass(i, Scl); ClrStatesOK(); };
    void            ClrVMass(int i)                               { SclVMass(i,0); };
    void            SetVAttr(int i, double Val)                   { m_M.SetVAttr(i, Val); };
    void            SetSpcScalar(double V)                        { m_M.SetSpcScalar(V); ClrStatesOK(); };

    __declspec(property(get=getVValue,put=SetVValue))             double VValue[];
    __declspec(property(get=getVMass))                            double VMass[];
    __declspec(property(get=getVMole))                            double VMole[];
    __declspec(property(get=getVAttr,put=SetVAttr))               double VAttr[];
    __declspec(property(get=getVCalc))                            double VCalc[];
    __declspec(property(get=getVPtr))                             double *VPtr[];

    virtual double Mass(PhMask Phase=som_ALL);
    virtual double Mass(CIArray & SpIds)                          { return m_M.Mass(SpIds); };
    virtual double Mass(CSysVector & Scale)                       { return m_M.Mass(Scale); };
    virtual double Moles(PhMask Phase=som_ALL)                    { return m_M.Moles(Phase); };
    virtual double Moles(CIArray & SpIds)                         { return m_M.Moles(SpIds); };
    virtual double Moles(CSysVector & Scale)                      { return m_M.Moles(Scale); };
    virtual double MoleWt(PhMask Phase=som_ALL)                   { return m_M.Mass(Phase)/GTZ(m_M.Moles(Phase)); };
    virtual double MoleWt(CIArray & SpIds)                        { return m_M.Mass(SpIds)/GTZ(m_M.Moles(SpIds)); };
    virtual double MoleWt(CSysVector & Scale)                     { return m_M.Mass(Scale)/GTZ(m_M.Moles(Scale)); };

    double ElementMass(CIArray & ElementIds, PhMask Phase=som_ALL) { return m_M.ElementMass(ElementIds, Phase); };
    //virtual double Volume(CIArray & SpIds);
    //virtual double NVolume(CIArray & SpIds);

    double Volume(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
    double Volume(CIArray & SpIds, double T_, double P_, CSysVector * pMA=NULL);
    double Volume(CSysVector & Scale, double T_, double P_, CSysVector * pMA=NULL);
    double Volume(PhMask Phase=som_ALL)                                             { return Volume(Phase, Temp(), Press()); };
    double Volume(CIArray & SpIds)                                                  { return Volume(SpIds, Temp(), Press()); };
    double Volume(CSysVector & Scale)                                               { return Volume(Scale, Temp(), Press()); };
    double NVolume(PhMask Phase=som_ALL, CSysVector * pMA=NULL)                     { return Volume(Phase, Norm_T, Norm_P); };
    double NVolume(CIArray & SpIds, CSysVector * pMA=NULL)                          { return Volume(SpIds, Norm_T, Norm_P); };
    double NVolume(CSysVector & Scale, CSysVector * pMA=NULL)                       { return Volume(Scale, Norm_T, Norm_P); };

    virtual double Rho(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)         { SPMDLTIMERLAP(sm_swDens); return SDB.Density(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData()); };
    virtual double Rho(CIArray & SpIds, double T_, double P_, CSysVector * pMA=NULL)      { SPMDLTIMERLAP(sm_swDens); return SDB.Density(m_iFidelity, SpIds, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData()); };
    virtual double Rho(CSysVector & Scale, double T_, double P_, CSysVector * pMA=NULL)   { SPMDLTIMERLAP(sm_swDens); return SDB.Density(m_iFidelity, Scale, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData()); };
    virtual double NRho(PhMask Phase=som_ALL, CSysVector * pMA=NULL)                      { SPMDLTIMERLAP(sm_swDens); return Rho(Phase, Norm_T, Norm_P, pMA); };
    virtual double NRho(CIArray & SpIds, CSysVector * pMA=NULL)                           { SPMDLTIMERLAP(sm_swDens); return Rho(SpIds, Norm_T, Norm_P, pMA); };
    virtual double NRho(CSysVector & Scale, CSysVector * pMA=NULL)                        { SPMDLTIMERLAP(sm_swDens); return Rho(Scale, Norm_T, Norm_P, pMA); };
    double Rho(PhMask Phase=som_ALL)                                                      { return Rho(Phase, Temp(), Press()); };
    double Rho(CIArray & SpIds)                                                           { return Rho(SpIds, Temp(), Press()); };
    double Rho(CSysVector & Scale)                                                        { return Rho(Scale, Temp(), Press()); };

    virtual double DynamicViscosity(PhMask Phase=som_ALL);
    virtual double DynamicViscosity(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
    virtual double KinematicViscosity(PhMask PhaseM=som_ALL);
    virtual double KinematicViscosity(PhMask PhaseM, double T_, double P_, CSysVector* S=NULL);
    virtual double ThermalConductivity(PhMask PhaseM=som_ALL);
    virtual double ThermalConductivity(PhMask PhaseM, double T_, double P_, CSysVector* S=NULL);
    virtual double PrandtlNo(PhMask PhaseM=som_ALL);
    virtual double PrandtlNo(PhMask PhaseM, double T_, double P_, CSysVector* S=NULL);
    virtual double SurfaceTension(PhMask Phase=som_ALL);
    virtual double SurfaceTension(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
    virtual double HeatOfCombustionHi(PhMask Phase=som_ALL);
    virtual double HeatOfCombustionHi(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
    virtual double HeatOfCombustionLo(PhMask Phase=som_ALL);
    virtual double HeatOfCombustionLo(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);

    // new Stuff for Trancritical
    virtual double TCritical(PhMask Phase=som_Gas, eScdGasMixingRule Rule=eScdGMR_Default);
    virtual double PCritical(PhMask Phase=som_Gas, eScdGasMixingRule Rule=eScdGMR_Default);
    virtual double VCritical(PhMask Phase=som_Gas, eScdGasMixingRule Rule=eScdGMR_Default);
    virtual double AccentricFactor(PhMask Phase=som_Gas, eScdGasMixingRule Rule=eScdGMR_Default);

    virtual double msdHIdeal(PhMask Phase, double T1, double T2);    // Mass Weight sum ....
    virtual double msdSIdeal(PhMask Phase, double T1, double T2);

    virtual double msdHResidual(PhMask Phase, double T, double P, eScdGasMixingRule Rule=eScdGMR_Default, eScdEqnOfState Eqn=eScdEOS_Default); // NB This is a funtion of Eqn Of State
    virtual double msdSResidual(PhMask Phase, double T, double P, eScdGasMixingRule Rule=eScdGMR_Default, eScdEqnOfState Eqn=eScdEOS_Default); // NB This is a funtion of Eqn Of State


    // Operations
    virtual void   SetTempPress(double T_, double P_);
    virtual void   SetPress(double P_);
    virtual void   SetTemp(double T_);
    virtual void   Set_msHs(double H_, double P=dNAN, SpModel * pRefMdl=NULL);
    virtual void   Set_msHz(double H_, double P=dNAN, SpModel * pRefMdl=NULL);
    virtual void   Set_msHf(double H_, double P=dNAN, SpModel * pRefMdl=NULL);
    virtual void   Set_totHs(double H_, double P=dNAN, SpModel * pRefMdl=NULL);
    virtual void   Set_totHz(double H_, double P=dNAN, SpModel * pRefMdl=NULL);
    virtual void   Set_totHf(double H_, double P=dNAN, SpModel * pRefMdl=NULL);

    virtual void   ZeroMass();
    virtual void   ZeroDeriv();
    virtual void   ScaleMass(PhMask Phase, double Mult);
    virtual void   ScaleMass(CIArray & SpIds, double Mult);
    virtual void   AdjustMassTo(PhMask Phase, double RqdMass);
    virtual void   AdjustMassTo(CIArray & SpIds, double RqdMass);
    virtual void   SetTraceMass();

    virtual void   SaveMass(SpVector & V);
    virtual void   SaveMass(CSysVector & A);
    virtual void   RestoreMass(SpVector & V);
    virtual void   RestoreMass(CSysVector & A);

    void           SetLoMassLimit(double LoMassLimit=0.0) { m_LoMassLimit=LoMassLimit; };
    virtual void   SetMassF(SpModel * Other, PhMask Phase, double Frac, double Press);
    virtual void   SetMassF(SpModel * Other, eSetMass How, double SF__, double LF__, double VF__, double Press);
    virtual void   SetMassF(SpModel * Other, CSysVector & Filter, double Frac, double Press);
    virtual void   SetMassM(SpModel * Other, PhMask Phase, double Mass__, double Press);
    virtual void   SetMassM(SpModel * Other, eSetMass How, double SM__, double LM__, double VM__, double Press);
    virtual void   SetMassM(SpModel * Other, CSysVector & Filter, double Mass__, double Press);

    virtual void   AddMassF(SpModel * Other, PhMask Phase, double Frac);
    virtual void   AddMassF(SpModel * Other, eSetMass How, double SF__, double LF__, double VF__);
    virtual void   AddMassF(SpModel * Other, CSysVector & Filter, double Frac);
    virtual void   AddMassM(SpModel * Other, PhMask Phase, double Mass__);
    virtual void   AddMassM(SpModel * Other, eSetMass How, double SM__, double LM__, double VM__);
    virtual void   AddMassM(SpModel * Other, CSysVector & Filter, double Mass__);

    virtual void   SubMassF(SpModel * Other, PhMask Phase, double Frac);
    virtual void   SubMassF(SpModel * Other, eSetMass How, double SF__, double LF__, double VF__);
    virtual void   SubMassF(SpModel * Other, CSysVector & Filter, double Frac);
    virtual void   SubMassM(SpModel * Other, PhMask Phase, double Mass__);
    virtual void   SubMassM(SpModel * Other, eSetMass How, double SM__, double LM__, double VM__);
    virtual void   SubMassM(SpModel * Other, CSysVector & Filter, double Mass__);

    virtual void   Copy(SpModel * Other);
    virtual void   AddDeriv(SpModel * Flw, double Sgn_);//, double FluxDamping);
    virtual void   AddInternalDerivs(CSysVector * pdMdt, double dHdt);
    virtual void   AddDiscrete(SpModel * Flw, double Sgn_);
    virtual void   ApplyDerivs(double dTime, flag DoDbg);
    virtual void   ODEOperate(CODEDataBlock & ODB);

    double         Duty()                                                         { return m_DC.m_dDuty; };
    void           SetDuty(double D)                                              { m_DC.m_dDuty=D; };

    virtual double MassFrac(PhMask Phase=som_ALL)                                 { return m_M.MassFrac(Phase); };
    virtual double MoleFrac(PhMask Phase=som_ALL)                                 { return m_M.MoleFrac(Phase); };
    virtual double VolFrac(PhMask Phase, double Temp, double Press)               { SPMDLTIMERLAP(sm_swDens); return SDB.VolFrac(m_iFidelity, Phase, Temp, Press, &m_Ovr, m_M.SVData()); };
    virtual double SpecieConc(double T_, int iSpNo, PhMask Phase=som_ALL);
    virtual double PhaseConc(double T_, PhMask Ph, PhMask Phase=som_ALL);

    virtual void   SetDebug(flag DbgOn_);

    virtual int    FlashCmpIndex();
    virtual int    FlashLiqIndex();
    virtual int    FlashVapIndex();
    virtual LPTSTR FlashDescription();
    virtual double SaturationP(double T, CSysVector * pMA=NULL);
    virtual double SaturationT(double P, CSysVector * pMA=NULL);
    virtual double PureSaturationP(double T, CSysVector * pMA=NULL)               { return SpModel::SaturationP(T, pMA);};
    virtual double PureSaturationT(double P, CSysVector * pMA=NULL)               { return SpModel::SaturationT(P, pMA);};
    virtual double BoilingPtElevation(double P_, CSysVector * pMA=NULL);

    //virtual long   InitialisePropertyMap(CPropertyMap * pParentMap);

    // CPropertyBase override
    virtual LPCTSTR DefinedPropertyMapName()						  								        { return ClassId(); };
    virtual long		DefinedPropertyCount();
    virtual long  	DefinedPropertyInfo(long Index, MPropertyInfo & Info);

    virtual DWORD   GetPropertyVisibility(long Index) throw(...);
    virtual void    GetPropertyValue(long Index, DWORD Phase, double T, double P, MPropertyValue &Value) throw(...);
    virtual void    PutPropertyValue(long Index, MPropertyValue &Value) throw(...);

    MPropertyValue  m_WorkPropValue;

    // SMDK Connectivity
    virtual MXSpModel       * GetSpModel4Cast()                                      { return this; };
    virtual MSpModelBase    * GetMSpModelBase4Cast()                                 { return NULL; };

    virtual long              GetSpQualityCount4Cast()                               { return nQuals; };
    virtual MXSpQuality     * GetSpQuality4Cast(long i, bool Required=false)         { return QualityPtr(i, Required); };// Quals[i].m_pQ; };
    virtual MSpQualityBase  * GetMSpQualityBase4Cast(long i, bool Required=false)    { SpQuality * p=QualityPtr(i, Required); return p ? p->GetMSpQualityBase4Cast() : NULL; };
    virtual MIBayer         * GetMIBayer()                                           { return dynamic_cast<MIBayer*>(this) ; };

#ifndef _RELEASE
    static int     SetDoTestEstTP(int OnCnt) { int Was=CSpecieDataBase::gs_nDoTestEstTP; CSpecieDataBase::gs_nDoTestEstTP=OnCnt; return Was; };
    static bool    m_bTestMassNotNeg;
    void           DoTestMassNotNeg();
#endif

    enum eSVForce {SVForce_None, SVForce_Lcl, SVForce_Glbl};

    CSpView       & WhichView(eSVForce Force);
    void            SetView(SV_View View, eSVForce Force=SVForce_None);
    SV_View         GetView(eSVForce Force=SVForce_None)                               { return WhichView(Force).m_iView; };
    void            SetViewBasis(SV_ViewBasis ViewBasis, eSVForce Force=SVForce_None);
    SV_ViewBasis    GetViewBasis(eSVForce Force=SVForce_None)                          { return WhichView(Force).m_iViewBasis; };
    long            GenVarsAvail() { return sm_VarsAvail|m_VarsAvail; };
    long            SpVarsAvail() { return sm_SpVarsAvail|m_SpVarsAvail; };

    CSysVector    * pMArray()     { return &m_M;};
    CSysVector    * pdMArray()    { return &m_dM;};
    CSysVector    & MArray()      { return m_M;};
    CSysVector    & dMArray()     { return m_dM;};

    operator        CSysVector*() { return &m_M; };
    operator        CSysVector&() { return m_M; };
    operator        CSVData()     { return m_M.SVData(); };
    CSVData         SVData()      { return m_M.SVData(); };

    bool            MassInUseOK();
    void            SetMassInUseOK(bool On);
    bool            MassInUse(int i);
    void            SetMassInUse(int i, bool InUse);
    void            SetMassInUse(bool InUse);
    void            ClrMassInUse();

    static void     InitialiseHumidity();       
    double          SpecificHumidity();       
    double          RelativeHumidity();       
    double          HumidityDewPt();       

    DEFINE_CI(SpModel, TaggedObject, 16);
    DEFINE_SPARES(SpModel);

  protected:
    void           VDS_UpdateAttribures();
    void           VDS_FindOptions(CVDS_WorkSpace & WS, bool doDbg);
    void           VDS_GetMTmpForAmount(CVDS_WorkSpace & WS, PhMask PhM, bool doDbg);
    void           VDS_GetMTmpForFrac(CVDS_WorkSpace & WS, PhMask PhM, bool doDbg);
    void           VDS_UpdateFractions(CVDS_WorkSpace & WS, bool doDbg);

  public:
    DWORD          m_dwValidFlags;
    DWORD          m_dwMassVersionNo; // everytime that SMVF_M_OK is cleared then this is incremented

    //protected:
    double         m_CurrentP;                   // Pressure - if no Shape
    double         m_CurrentT;
    double         m_PreviousT;

    double         m_RqdTemp;
    double         m_KeptTemp;
    double         m_FileTemp;
    double         m_FilePres;

    SpVector       m_M;                 // Array of Masses
    SpVector       m_dM;                // Array of Mass derivatives
    double         m_PhMass[MaxPhases];

    bool           m_bMassInUseOK;
    CSpUseArray    m_MassInUse;

    static long    m_iFidelity; // currently just a static - must improve
    flag           m_bMixed;
    flag           m_bClosed;
    flag           m_bPressAsSet;        // Dont calc press
    flag           m_bUsableMass;
    flag           m_bTraceMass;
    //flag           m_bSQEditable;
    flag           m_bVentExpandOK;
    flag           m_bEquilRqd4HEval;
    flag           m_bScan4EquilRqd4HEval;

    SpMdlUsage     m_iUsage;

    byte           m_bBadTempOK;
    byte           m_iHoldVentExpand;
    byte           m_iHoldEquilibrate;
    byte           m_iEquilibrateBusy;
    byte           m_iEHBusy;

    double         m_dMaxLevel;
    double         m_dMaxPress;
    double         m_dMassScale;          // Used to display Small totals while retaining reasonable amount;
    bool           m_bKeepNormalEnabled;  // keep normalised;

    signed char    m_iDataSgn;
    bool           m_bNegDataOK;

    Strng          m_sMainTemplate;
    SV_View        m_iSetView;
    SV_ViewBasis   m_iViewBasis;
    bool           m_bCfgImgSet;
    CSpMCfgImg    *m_pSetCfgImg;
    double         m_dSetCfgImgT;
    double         m_dSetCfgImgP;
    bool           m_bSpeciesAdjusted;

    static eScdEqnOfState    sm_iEqnOfState;
    static eScdGasMixingRule sm_iGasMixingRule;
    double         m_dGasMixAlpha;

    static long    sm_iFixupStrategy;

  public:
    IE_Enables     m_iEnable;             // Integrators On/Off

    CStateArray    m_States;                // Array of Integrators
    // Energy Data
    double         m_msHf;               // Enthalpy
    double         m_msHfDeriv;

    CSpMdlDerivCtrl m_DC;

    bool           m_fConvergeBusy;

    CSpView        m_Vw;
    CSpView *      m_pGlblView;

    double         m_dPrvSatT;         // Previous Estimate of VapourTemp

  public:

    CVLEBase     * m_pVLE;

    flag           m_bEquilOn;
    byte           m_iEquilAct;

    void           ClearAutoVLE()   { m_AutoVLE.m_iCmp=m_AutoVLE.m_iVap=m_AutoVLE.m_iLiq=-1; };
    bool           AutoVLEOK()      { return CDB.m_Flashable.GetCount()>0; };//m_AutoVLE.m_iCmp>=0; };
    flag           AutoVLEOn()      { return m_AutoVLE.m_iCmp>=0 && m_bEquilOn; };// && m_AutoVLE.m_bOn; };

    struct 
      {
      //flag         m_bOn;
      int          m_iCmp;
      int          m_iLiq;
      int          m_iVap;
      byte         m_iSet;    
      double       m_dSpecH;    
      double       m_dRelH;    
      double       m_dDewPt;    
      }            m_AutoVLE;

    byte           nQuals;
    SpQualInfo     *Quals;
    byte           bSQFlags;

    SpPropOveride  m_Ovr;

    double         m_LoMassLimit;

    static flag   sm_VarsChange;
    static long   sm_VarsAvail;
    long          m_VarsAvail;
    static flag   sm_SpVarsChange;
    static long   sm_SpVarsAvail;
    long          m_SpVarsAvail;

    static SPMEnthStrat iEnthalpyStrategy;
    static SPMMixStrat  iSpMdlMixStrategy;
    static double       dUsrDefT;
    static double       dUsrDefP;
    static double       dTestToleranceMassFrac;
    static bool         m_RangeErrorsOn;
    static bool         m_RangeExtErrorsOn;

#if WITHSPMDLTIMERS
    static CGlblStopWatch   sm_swEvalM;
    static CGlblStopWatch   sm_swEvalT;
    static CGlblStopWatch   sm_swEvalH;
    static CGlblStopWatch   sm_swEvalHC;
    static CGlblStopWatch   sm_swDens;
    static CGlblStopWatch   sm_swHeat;
    static long         sm_lEvalMCalls;
    static long         sm_lEvalMEvals;
    static long         sm_lEvalHCalls;
    static long         sm_lEvalHEvals;
#endif

  public:
    static const pchar GroupName;

  };

#ifdef _RELEASE
#define TESTMASSNOTNEG() {};
#define TESTMASSNOTNEGC(a) {};
#else
#define TESTMASSNOTNEG() { if(m_bTestMassNotNeg) DoTestMassNotNeg(); };
#define TESTMASSNOTNEGC(a) { if ((a)->m_bTestMassNotNeg) (a)->DoTestMassNotNeg(); };
#endif

// ===========================================================================
//
//
//
// ===========================================================================

union CSpHandle2
  {
  long Hnd;
  struct { long Ph:8, Index:24; };
  CSpHandle2(long H) : Hnd(H) {} ;
  CSpHandle2(long P, long I) : Ph(P), Index(I) {} ;
  };

inline long SpHandle2(int Phase, int Index) { CSpHandle2 X(Phase, Index); return X.Hnd; };
inline long SpIndex2(long Handle2) { CSpHandle2 X(Handle2); return X.Index; };
inline long SpPhase2(long Handle2) { CSpHandle2 X(Handle2); return X.Ph; };

// ===========================================================================
//
//
//
// ===========================================================================

typedef CArray <bool, bool> SpDHFlags;

class DllImportExport SpModelEx : public SpModel
  {
  friend class SpMArray;
  friend class SpVector;
  friend class SpQuality;
  friend class SpContainer;
  friend class SpConduit;
  friend class SpImage;

  //  protected:
  //    SpDHFlags  IncDH;

  public:
    SpModelEx(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SpModelEx();

    // Must supply the next 3

    virtual MSMFnRanges * SMFnRanges() { return &m_SMFnRanges; };
    virtual double msCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL)=0;
    virtual double msHm(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL)=0;

    virtual double msHs(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);
    virtual double msHz(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);
    virtual double msHf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);
    virtual double msSf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);

    virtual double totCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
    virtual double totHs(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
    virtual double totHz(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
    virtual double totHf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
    virtual double totSf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);

  protected:
    MSMFnRanges  m_SMFnRanges;
  };

// ===========================================================================
//
//
//
// ===========================================================================

//inline double SpModel::msCp(PhMask Phase, double T_, double P_, CSysVector * pMA)   { SPMDLTIMERLAP(sm_swHeat); return SDB.msCp(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? &(*pMA)[0] : &M[0]); };
//inline double SpModel::msHs(PhMask Phase, double T_, double P_, CSysVector * pMA)   { SPMDLTIMERLAP(sm_swHeat); return SDB.msHs(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? &(*pMA)[0] : &M[0]); };
//inline double SpModel::msHz(PhMask Phase, double T_, double P_, CSysVector * pMA)   { SPMDLTIMERLAP(sm_swHeat); return SDB.msHz(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? &(*pMA)[0] : &M[0]); };
//inline double SpModel::msHf(PhMask Phase, double T_, double P_, CSysVector * pMA)   { SPMDLTIMERLAP(sm_swHeat); return SDB.msHf(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? &(*pMA)[0] : &M[0]); };
//inline double SpModel::msSf(PhMask Phase, double T_, double P_, CSysVector * pMA)   { SPMDLTIMERLAP(sm_swHeat); return SDB.msSf(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? &(*pMA)[0] : &M[0]); };
//inline double SpModel::totCp(PhMask Phase, double T_, double P_, CSysVector * pMA)  { SPMDLTIMERLAP(sm_swHeat); return SDB.totCp(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? &(*pMA)[0] : &M[0]); };
//inline double SpModel::totHs(PhMask Phase, double T_, double P_, CSysVector * pMA)  { SPMDLTIMERLAP(sm_swHeat); return SDB.totHs(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? &(*pMA)[0] : &M[0]); };
//inline double SpModel::totHz(PhMask Phase, double T_, double P_, CSysVector * pMA)  { SPMDLTIMERLAP(sm_swHeat); return SDB.totHz(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? &(*pMA)[0] : &M[0]); };
//inline double SpModel::totHf(PhMask Phase, double T_, double P_, CSysVector * pMA)  { SPMDLTIMERLAP(sm_swHeat); return SDB.totHf(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? &(*pMA)[0] : &M[0]); };
//inline double SpModel::totSf(PhMask Phase, double T_, double P_, CSysVector * pMA)  { SPMDLTIMERLAP(sm_swHeat); return SDB.totSf(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? &(*pMA)[0] : &M[0]); };

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport SpModelOwner : public TaggedObject, public CPropertyBase
  {
  public :
    SpModelOwner(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach, flag HasShape);
    virtual ~SpModelOwner();

    SpModel       *Model()          { return pModel; };
    CSysVector    &MArray()         { return pModel->MArray(); };
    CSysVector    *pMArray()        { return pModel->pMArray(); };
    SpPropOveride *Ovr()            { return pModel ? &pModel->m_Ovr : NULL; };
    CSVData        SVData()         { return pModel->SVData(); };
    //CSysVector    &dMArray()        { return pModel->dMArray(); };

    operator       SpModel*()       { return pModel; };
    operator       SpModel&()       { return *pModel; };
    operator       CSysVector*()    { return pModel->pMArray(); };
    operator       CSysVector&()    { return pModel->MArray(); };
    operator       SpPropOveride*() { return pModel ? &pModel->m_Ovr : NULL; };

    void           AddShapeName(DataDefnBlk &DDB);
    void           AddModelName(DataDefnBlk &DDB);
    void           AddFidelity(DataDefnBlk &DDB, bool Editable);
    void           AddFnRanges(DataDefnBlk &DDB);
    flag           DataXchg(DataChangeBlk & DCB);

    flag           ModelPermanent() { return fPermModel; };
    flag           SetModelPermanent(flag Perm) { fPermModel=Perm; return fPermModel; };
    flag           ModelSelectable() { return m_fModelSelectable; };
    flag           SetModelSelectable(flag ModelSelectable) { m_fModelSelectable=ModelSelectable; return m_fModelSelectable; };
    void           SelectModel(SpModel * &pModel, SpModel * pOther, flag ForceIt);
    void           SelectModel(SpModel * &pModel, CSpMdlSlct & Slct);
    void           CopyModel(SpModel * &pModel, SpModel * RqdModel) { ChangeModel(pModel, RqdModel->Class()); };
    void           CopyQualities(SpModel * pModel, SpModel * SrcModel, int q0=-1, int q1=-1);
    void           CopyShape(pSpShape &pShape, SpModel * RqdShape) { ChangeShape(pShape, RqdShape->Class()); };
    void           ChangeShape(pSpShape &pShape, pchar ReqdShapeDesc);
    void           ChangeShape(pSpShape &pShape, pTagObjClass pRqdShapeClass_);

    virtual SpShape * Shape() { return NULL; };

    void           ChangeModel(SpModel * &pModel, pchar ReqdModelDesc, bool UseAsClassId=false);
    void           ChangeModel(SpModel * &pModel, pTagObjClass pRqdModelClass_);

    void           ChangeModel(pchar ReqdModelDesc, bool UseAsClassId=false) { ChangeModel(pModel, ReqdModelDesc, UseAsClassId); };
    void           ChangeModel(pTagObjClass pRqdModelClass_) { ChangeModel(pModel, pRqdModelClass_); };

    void           SetModel(SpModel * &pModel, pchar ReqdModelDesc, bool UseAsClassId) { fPermModel=false; ChangeModel(pModel, ReqdModelDesc, UseAsClassId); fPermModel=true; };
    void           SetModel(SpModel * &pModel, pTagObjClass pRqdModelClass_) { fPermModel=false; ChangeModel(pModel, pRqdModelClass_); fPermModel=true; };

    void           SetHoldEquilibrate()                   { pModel->SetHoldEquilibrate(); };
    void           ClrHoldEquilibrate(bool DoEquil=true)  { pModel->ClrHoldEquilibrate(DoEquil); };

    virtual void   SetState(eScdMdlStateActs RqdState)=0;

    //    virtual double MCB_GetVolume() { return 0.0; };
    //    virtual double MCB_SetVolume(double V) { return 0.0; };
    virtual void   MCB_GetOriginalTP(double &OrigT, double &OrigP) { OrigT=dNAN; OrigP=dNAN; };
    virtual void   MCB_OnSetVolume(flag KeepLPT, double OrigT, double OrigP, double OrigVol, double NewVol) {};
    virtual void   MCB_OnSetVapVolScl(double OrigScl, double NewScl) {};

    // CPropertyBase override
    virtual LPCTSTR DefinedPropertyMapName()						  								{ return pModel->DefinedPropertyMapName(); };
    virtual long		DefinedPropertyCount()						  								  { return pModel->DefinedPropertyCount(); };
    virtual long  	DefinedPropertyInfo(long Index, MPropertyInfo & Info)	{ return pModel->DefinedPropertyInfo(Index, Info); };

    virtual DWORD   GetPropertyVisibility(long Index) throw(...)                                                              { return pModel->GetPropertyVisibility(Index); };
    virtual void    GetPropertyValue(long Index, DWORD Phase, double T, double P, MPropertyValue &Value) throw(...) { pModel->GetPropertyValue(Index, Phase, T, P, Value); };
    virtual void    PutPropertyValue(long Index, MPropertyValue &Value) throw(...)						  								              { pModel->PutPropertyValue(Index, Value); };

    bool            SMFnsInRange() { MSMFnRanges *p=pModel->SMFnRanges(); return p ? p->AllInRange() : true; };

    CVLEBase      * VLEBlk() { return pModel->m_pVLE; };
#ifndef _RELEASE
    void           SanityCheck();
#else
    void           SanityCheck() {}; //DO NOTHING in Release
#endif
  public :
    flag          fPermModel;
    flag          m_fModelSelectable;
    SpModel *     pModel;
    Strng         sReqdModel;
    Strng         m_sSMFnRanges;

    static Strng  DefaultShape;
    static Strng  DefaultModel;

  };

// ==========================================================================
//
//                               Specie Vector
//
// ==========================================================================

inline SpMdlUsage SpQuality::Usage()        { return pModel->Usage();};
inline flag SpQuality::SQEditable()         { return pModel->SQFlagsSet(SQF_Editable);};
inline flag SpQuality::SQSource()           { return pModel->SQFlagsSet(SQF_Source);};
inline flag SpQuality::SQTearSet()          { return pModel->SQFlagsSet(SQF_TearSet);};
inline flag SpQuality::RemoveMe()           { return m_bRemoveMe;};
inline void SpQuality::SetRemoveMe(flag On) { m_bRemoveMe=On;};
inline flag SpQuality::HoldCopy()           { return m_bHoldCopy;};
inline void SpQuality::SetHoldCopy(flag On) { m_bHoldCopy=On;};

// ===========================================================================
//
//
//
// ===========================================================================

#undef DllImportExport

#endif
