//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#ifndef  __M_HXBASE_H
#define  __M_HXBASE_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __SP_CONT_H
  #include "sp_cont.h"
#endif
#ifndef __SP_CONT_H
  #include "sp_cont.h"
#endif
#ifndef __SP_REACT_H
  #include "sp_react.h"
#endif
#ifndef __3D_FN_H
  #include "3d_fn.h"
#endif
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif

#include "blockevalbase.h"

#ifdef __M_HXBASE_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
//
// Environmental Heat xchg
//
// ==========================================================================

#define DEFINE_EHX(SM) DEFINE_TAGOBJ(SM)
#define IMPLEMENT_EHX(Obj, ModelId, Version, Cat, SDesc, LDesc, SelectMask) \
  IMPLEMENT_TAGOBJSLCT(Obj, CEnvironHX::GroupName, ModelId, Version, "", "", Cat, SDesc, LDesc, SelectMask)

class DllImportExport CEnvironHX : public TaggedObject
  {
  public:
    CEnvironHX(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEnvironHX();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB) { return 0; };

    virtual flag   ValidateData(ValidateDataBlk & VDB) { return 1; };
    virtual void   EvalProducts(SpConduit & Qf, double FinalTEst=dNAN);
    virtual void   EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst=dNAN);
    virtual double HeatFlow() { return dHeatFlow; };

  public:
    static const pchar GroupName;

    double       dHeatFlow;
    double       TempKFeed;
    double       TempKProd;
  };

DEFINE_EHX(CEnvironHX);

// ===========================================================================

class DllImportExport CEHX_LossPerQm: public CEnvironHX
  {
  public:
    CEHX_LossPerQm(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEHX_LossPerQm();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    virtual void   EvalProducts(SpConduit & Qf, double FinalTEst=dNAN);
    virtual void   EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst=dNAN);

  public:
    double         dHperQm;
  };

DEFINE_EHX(CEHX_LossPerQm);

// ===========================================================================

class DllImportExport CEHX_ProductT: public CEnvironHX
  {
  public:
    CEHX_ProductT(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEHX_ProductT();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    virtual void   EvalProducts(SpConduit & Qf, double FinalTEst=dNAN);
    virtual void   EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst=dNAN);

  public:
    double         dProdT;
  };

DEFINE_EHX(CEHX_ProductT);

// ===========================================================================

class DllImportExport CEHX_TDrop: public CEnvironHX
  {
  public:
    CEHX_TDrop(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEHX_TDrop();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    virtual void   EvalProducts(SpConduit & Qf, double FinalTEst=dNAN);
    virtual void   EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst=dNAN);

  public:
    double         dTDrop;
  };

DEFINE_EHX(CEHX_TDrop);

// ===========================================================================

class DllImportExport CEHX_Loss2Ambient: public CEnvironHX
  {
  public:
    CEHX_Loss2Ambient(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEHX_Loss2Ambient();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    virtual void   EvalProducts(SpConduit & Qf, double FinalTEst=dNAN);
    virtual void   EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst=dNAN);

  public:
    double dAmbTOvr;
    double dAmbT;
    double dPipe_Thick;
    double dIns_Thick;
    double dK_Pipe;
    double dK_Ins;
    double dH_2;
    double dH_4;
    double dH_Rad;
    double dUA;
    double dPwr;
    double dPipeD; //used in ProBal only!
    double dPipeL; //used in ProBal only!

  };

DEFINE_EHX(CEHX_Loss2Ambient);

// ===========================================================================

class DllImportExport CEHX_Loss2AmbientTank: public CEnvironHX
  {
  public:
    CEHX_Loss2AmbientTank(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEHX_Loss2AmbientTank();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    virtual void   EvalProducts(SpConduit & Qf, double FinalTEst=dNAN);
    virtual void   EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst=dNAN);

  public:
    double dAmbTOvr;
    double dAmbT;
    double dArea;
    double dHTC;
  };

DEFINE_EHX(CEHX_Loss2AmbientTank);

// ===========================================================================

class DllImportExport CEHX_FixedHeatFlow: public CEnvironHX
  {
  public:
    CEHX_FixedHeatFlow(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEHX_FixedHeatFlow();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    virtual void   EvalProducts(SpConduit & Qf, double FinalTEst=dNAN);
    virtual void   EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst=dNAN);

  public:
    double         dRqdHeatFlow;
  };

DEFINE_EHX(CEHX_FixedHeatFlow);

// ===========================================================================

XID xidEnvHXEnable = MdlBsXID(32001);
XID xidEnvHXEqnNm  = MdlBsXID(32002);

// ===========================================================================

class DllImportExport CEnvironHXBase : public CBlockEvalBase
  {
  public:
    CEnvironHXBase(FlwNode* pAttach, dword SelectMask, LPTSTR Name="EHX");
    ~CEnvironHXBase();

    flag           Open(TagObjClass * pEHXClass=NULL, flag Fixed=False);
    flag           Open(byte OnOffSeq, TagObjClass * pEHXClass=NULL, flag Fixed=False);
    byte           OpenStatus();
    void           Close();

    CEnvironHX *   operator() ()                                            { return fEnabled ? pHL : NULL; };
    flag           Enabled()                                                { return fEnabled && (pHL!=NULL); };
    void           Add_OnOff(DataDefnBlk& DDB, dword Flags=isParmStopped);
    void           BuildDataDefn(DataDefnBlk& DDB, char* pTag="EHX", char* pTagComment=NULL, DDBPages PageIs=DDB_RqdPage, dword UserInfo=0);
    flag           DataXchg(DataChangeBlk & DCB);

    flag           ValidateData(ValidateDataBlk & VDB)                      { return Enabled() ? pHL->ValidateData(VDB) : 0; };
    void           EvalProducts(SpConduit & Qf, double FinalTEst=dNAN)  { if (Enabled()) pHL->EvalProducts(Qf, FinalTEst); };
    void           EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst=dNAN) { if (Enabled()) pHL->EvalProductsInline(Qf, Len, Diam, FinalTEst); };

    double         HeatFlow()                                               { return Enabled() ? pHL->dHeatFlow : 0.0; };

  protected:
    flag            fFixed;
    flag            fEnabled;
    CEnvironHX *    pHL;
    FlwNode*        pNd;
    dword           m_dwSelectMask;
  };

//===========================================================================
//
//  Heat Exchange Side
//
//===========================================================================

#define DEFINE_HXB(SM) DEFINE_TAGOBJ(SM)
#define IMPLEMENT_HXB(Obj, ModelId, Version, Cat, SDesc, LDesc) \
        IMPLEMENT_TAGOBJ(Obj, CHXBlock::GroupName, ModelId, Version, "", "", Cat, SDesc, LDesc)

#define HX_MEASURE_LIQVAP  0

DEFINE_HXB(CHXBlock)

class DllImportExport CHXSide
  {
  public:
    CHXBlock      *pHX;

    byte           iMode;        // Mode of Operation
    double         Ci;
    double         Cp;
    double         Qm; //total Qm
#if HX_MEASURE_LIQVAP
    double         m_LiqQm;
    double         m_VapQm;
#endif
    double         Ti;
    double         To;
    double         Sgn;
    double         Hi, Ho, dHEnv;
    double         FlFrac;
    double         m_SatT;
    double         m_SatP;
    double         m_SatPP;
    double         m_PPFrac;

    flag           fConnected;
    bool           fPreReact;

    SpVector       MassImg;

    SpConduit     *m_pCdIn;
    SpConduit     *m_pCd;
    SpConduit     *m_pByPass;
    SpContainer   *m_pCn;
    FlwNode       *m_pNd;
    CEnvironHX    *m_pEHX;
    CReactionBlock *m_pRB;

    double         m_Pi;
    double         m_Po;
    
    double         m_VfIn;

    double         Duty;
    double         MaxFlDuty;
    double         MaxNfDuty;

    SpConduit     *m_pWrkCd;
    SpConduit     *m_pMemCd;

    double         dFinalTEst;


    CHXSide(CHXBlock * pHX_);
    ~CHXSide();
    //void           Copy(CHXSide* p);

    void           BuildDataDefn(char*Tag, DataDefnBlk & DDB, TaggedObject* pTagObj, flag Conduit, flag Connected, flag PriSide);
    flag           DataXchg(DataChangeBlk & DCB);
    void           MeasureHXDataCd(SpConduit * pCd);
    void           MeasureHXDataCn(SpContainer * pCn);
    void           SetInput(SpConduit *pCdIn, SpContainer& rCn, PhMask CnPhase, double CnFraction, double PIn, SpConduit * pFeed, CEnvironHX * EHX, CReactionBlock * RB, double FinalTEst);
    void           SetInput(SpConduit *pCdIn, SpConduit& rCd, double PIn, CEnvironHX * EHX, CReactionBlock * RB, double FinalTEst, bool PreReact);
    void           ClrInput();
    void           NoJoinInput();

    void           EvalProducts(double FinalTEst=dNAN);
    double         KeepByPassLiquor(double Press);
    void           AddByPassLiquor();
    double         SendGasToVent(bool FullyCondensing);
    double         AddGasToVent(double VentFlow, bool MixVentBack);

    double         FlashPressOut() { return m_Po*m_PPFrac; };

  };

// ==========================================================================
//
//  Heat Exchange Block
//
// ==========================================================================

const byte HXF_CounterCurrent = 0;
const byte HXF_CoCurrent      = 1;

const byte HXM_ApproachTemp   = 0;
const byte HXM_Effectiveness  = 1;
const byte HXM_LMTD           = 2;

const byte HXC_ParFlw_1Pass   = 0;
const byte HXC_CntrFlw_1Pass  = 1;
const byte HXC_ShellTube      = 2;
const byte HXC_nShellTube     = 3;
const byte HXC_Curve          = 4;

class DllImportExport CHXBlock : public TaggedObject
  {
  public:
    enum LvlCharacters { HXLC_RectXSect, HXLC_CircXSect };
    static const pchar GroupName;
    static DDBValueLst DDBEffect[];
    static DDBValueLst DDBFlwTypes[];
    static DDBValueLst DDBXSect[];

    //CHXSide        Side[2];
    CHXSide        m_Pri;
    CHXSide        m_Sec;

    double         m_QmVent;         // Actual total Vent Flow
    double         m_QmVentRqd;      // Reqd steam Vent Flow
    double         m_dNonCondVentFrac;// Reqd fraction of non-condensables to be vented
    double         m_VentPress;      // 
    SpConduit     *m_pVent;          // Ptr to vent

    double         m_dMaxDuty,       // Theoretical Maximum Duty
                   m_dMaxNfDuty,     // Maximum Duty if no flashing takes place
                   m_dMaxFlDuty;     // Maximum Duty if flashing does take place

    byte           m_bLvlEffOn;
    byte           m_iXSect;

    double         m_dHTC,           // HeatTransfer Coeff
                   m_dArea,          // Area of Heat Exchanger
                   m_dApproachDT,    // 
                   //dUA,            // Heat Transfer / K
                   m_dMxEffLvl,      // S&T Lvl below which dEffLo applies
                   m_dMnEffLvl,      // S&T Lvl above which dEffhi applies
                   m_dMxEff,         // S&T Effective area % when Lvl below dLvlLO usually 1.0
                   m_dMnEff,         // S&T Effective area % when Lvl above dLvlLO usually snall (0.05)
                   m_dLvl,           // Current Lvl
                   m_dDuty,          // CHeatXBlock Actual Duty
                   m_dDutySens,      // CHeatXBlock Duty @ PressSens
                   m_dTheoryDuty,    // CHeatXBlock Theoretical Duty
                   m_dLMTD,          // Log Mean Temperature Difference
                   m_dEffect,        // Achieved Effectiveness
                   m_dLMTDFactor,    // LMTD Correction Factor
                   m_dCondensableFrac;   // Condensable Fraction

    byte           m_iSolveMode,     // Solution Mode
                   m_iConfig,        // Configuration
                   m_fOn;            // if clear switch heatexchange off

    flag           m_fPriIsHot;
    flag           m_fPreReact;      // if applicable, must heat exchange occur before RB or after
    C3DFn          m_EffectCurve;    // Effectiveness As a 3-d Curve

    CHXBlock(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CHXBlock();

    //virtual void Copy(CHXBlock * p);

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   SetLevel(double Level) { m_dLvl=Level; };
    virtual double Level() { return m_dLvl; };
    virtual void   SetCondensableFrac(double CF) { m_dCondensableFrac=Valid(CF) ? Range(0.0, CF, 1.0) : 1.0; };
    virtual double CondensableFrac() { return m_dCondensableFrac; };
    virtual double NonCondensableFrac() { return 1.0-m_dCondensableFrac; };
    virtual double EffArea();
    virtual double EffUA() { return m_dHTC*EffArea();};

    static double  LMTD(flag CoCurrent, double PTi, double PTo, double STi, double STo);
    static double  LMTD1(flag CoCurrent, double PTi, double PTo, double STi, double STo);
    static double  LMTDSat(double Ti, double To, double Ts);
    static double  LMTDSat1(double Ti, double To, double Ts);
    double         Effectiveness(double NTU, double C);

    void           SetVent(SpConduit& rCd, /*double QVentRqd,*/ double VentP);
    void           ClrVent();

    void           SetPrimary(SpConduit *pCdIn, SpConduit & Qp, double PIn, CEnvironHX* pEHX=NULL, CReactionBlock *pRB=NULL, double FinalTEst=dNAN, bool PreReact=false)
      { m_Pri.SetInput(pCdIn, Qp, PIn, pEHX, pRB, FinalTEst, PreReact); };
    void           SetPrimary(SpConduit *pCdIn, SpContainer & Cn, PhMask CnPhase, double CnFraction, double PIn, SpConduit * pFeed=NULL, CEnvironHX* pEHX=NULL, CReactionBlock *pRB=NULL, double FinalTEst=dNAN)
      { m_Pri.SetInput(pCdIn, Cn, CnPhase, CnFraction, PIn, pFeed, pEHX, pRB, FinalTEst); };
    void           SetSecondary(SpConduit *pCdIn, SpConduit & Qp, double PIn, CEnvironHX* pEHX=NULL, CReactionBlock *pRB=NULL, double FinalTEst=dNAN, bool PreReact=false)
      { m_Sec.SetInput(pCdIn, Qp, PIn, pEHX, pRB, FinalTEst, PreReact); };
    void           SetSecondary(SpConduit *pCdIn, SpContainer & Cn, PhMask CnPhase, double CnFraction, double PIn, SpConduit * pFeed=NULL, CEnvironHX* pEHX=NULL, CReactionBlock *pRB=NULL, double FinalTEst=dNAN)
      { m_Sec.SetInput(pCdIn, Cn, CnPhase, CnFraction, PIn, pFeed, pEHX, pRB, FinalTEst); };

    void           ClrPrimary()
      { m_Pri.ClrInput(); };
    void           ClrSecondary()
      { m_Sec.ClrInput(); };
    void           NoJoinPrimary()
      { m_Pri.NoJoinInput(); };
    void           NoJoinSecondary()
      { m_Sec.NoJoinInput(); };
    flag           PreReact() 
      { return m_fPreReact; };

    virtual void   EvalProducts(FlwNode* pNd);
    virtual void   EvalDerivs(FlwNode* pNd);

    // ConditionBlk Override
    DEFINE_CI(CHXBlock, TaggedObject, 10);

  };

// ===========================================================================

XID xidHXEnable           = MdlBsXID(32003);
XID xidHXLevel            = MdlBsXID(32004);
XID xidHXNonCondenseFrac  = MdlBsXID(32005);
XID xidHXEffArea          = MdlBsXID(32006);
XID xidHXEffUA            = MdlBsXID(32007);
XID xidHXAreaUsed         = MdlBsXID(32008);

class DllImportExport CHXBase : public CBlockEvalBase
  {
  protected:
    TagObjClass    *pRqdHXClass;

    flag            fFixed;
    flag            fEnabled;
    CHXBlock       *pHX;
    FlwNode        *pNd;

  public:
    CHXBase(FlwNode* pAttach, TagObjClass * RqdHXClass, flag Fixed=false, LPTSTR Name="HX");
    ~CHXBase();

    void           SetRqdHXClass(TagObjClass * pHXClass);
    flag           Open(TagObjClass * pHXClass=NULL, flag Fixed=false);
    flag           Open(byte OnOffSeq, TagObjClass * pHXClass=NULL, flag Fixed=False) { CBlockEvalBase::Open(OnOffSeq); return Open(pHXClass, Fixed); }
    byte           OpenStatus() { return CBlockEvalBase::OpenStatus(Enabled()); }
    void           Close();

    CHXBlock      *operator() () { return pHX; };
    flag           Enabled() { return fEnabled && (pHX!=NULL); };
    flag           PreReact() { return Enabled() ? pHX->PreReact() : false; };

    void           Add_OnOff(DataDefnBlk& DDB, dword Flags=isParmStopped);
    void           BuildDataDefn(DataDefnBlk& DDB, pchar pTag_=NULL, pchar pTagComment=NULL, DDBPages PageIs = DDB_OptPage, dword UserInfo=0);
    flag           DataXchg(DataChangeBlk & DCB);

    flag           ValidateData(ValidateDataBlk & VDB) { return pHX ? pHX->ValidateData(VDB) : 0; };

    void           SetVent(SpConduit & Cd, /*double QVentRqd,*/ double VentP)
      { if (Enabled()) pHX->SetVent(Cd, /*QVentRqd,*/ VentP); };
    void           ClrVent()
      { if (Enabled()) pHX->ClrVent(); };

    void           SetPrimary(SpConduit *pCdIn, SpConduit & Qp, double PIn, CEnvironHX* pEHX=NULL, CReactionBlock *pRB=NULL, double FinalTEst=dNAN, bool PreReact=false)
      { if (Enabled()) pHX->m_Pri.SetInput(pCdIn, Qp, PIn, pEHX, pRB, FinalTEst, PreReact); };
    void           SetPrimary(SpConduit *pCdIn, SpContainer & Cn, PhMask CnPhase, double CnFraction, double PIn, SpConduit * pFeed=NULL, CEnvironHX* pEHX=NULL, CReactionBlock *pRB=NULL, double FinalTEst=dNAN)
      { if (Enabled()) pHX->m_Pri.SetInput(pCdIn, Cn, CnPhase, CnFraction, PIn, pFeed, pEHX, pRB, FinalTEst); };
    //void           SetPrimaryVent(SpConduit & Cd, double QVentRqd, double VentP)
    //  { if (Enabled()) pHX->m_Pri.SetVent(Cd, QVentRqd, VentP); };
    void           SetSecondary(SpConduit *pCdIn, SpConduit & Qp, double PIn, CEnvironHX* pEHX=NULL, CReactionBlock *pRB=NULL, double FinalTEst=dNAN, bool PreReact=false)
      { if (Enabled()) pHX->m_Sec.SetInput(pCdIn, Qp, PIn, pEHX, pRB, FinalTEst, PreReact); };
    void           SetSecondary(SpConduit *pCdIn, SpContainer & Cn, PhMask CnPhase, double CnFraction, double PIn, SpConduit * pFeed=NULL, CEnvironHX* pEHX=NULL, CReactionBlock *pRB=NULL, double FinalTEst=dNAN)
      { if (Enabled()) pHX->m_Sec.SetInput(pCdIn, Cn, CnPhase, CnFraction, PIn, pFeed, pEHX, pRB, FinalTEst); };
    //void           SetSecondaryVent(SpConduit & Cd, double QVentRqd, double VentP)
    //  { if (Enabled()) pHX->m_Sec.SetVent(Cd, QVentRqd, VentP); };

    void ClrPrimary()      { if (Enabled()) pHX->m_Pri.ClrInput(); };
    void ClrSecondary()    { if (Enabled()) pHX->m_Sec.ClrInput(); };
    void NoJoinPrimary()   { if (Enabled()) pHX->m_Pri.NoJoinInput(); };
    void NoJoinSecondary() { if (Enabled()) pHX->m_Sec.NoJoinInput(); };

    void EvalProducts() { if (Enabled()) pHX->EvalProducts(pNd); };
    void EvalDerivs()   { if (Enabled()) pHX->EvalDerivs(pNd); };
//    void TransferMakeup(FlwNode* pNd, int IoId, SpConduit* pVent) { if (Enabled()) pHX->TransferMakeup(pNd, IoId, pVent); };

  private:
  };

// ==========================================================================
//
//  Heat Exchange Duty Solver
//
// ==========================================================================

class DllImportExport CHXDutyFinder : public MRootFinderBase
  {
  protected:
    CHXBlock & m_Blk;
    CHXSide &  m_Pri;
    CHXSide &  m_Sec;
    //flag       fTPFlash;
    byte       m_iFlwMode;
    byte       m_iCalcMeth;
    
    static CToleranceBlock s_Tol;

  public:

    CHXDutyFinder(CHXBlock * Blk_, byte FlwMode, byte CalcMeth) :
      m_Blk(*Blk_), m_Pri(Blk_->m_Pri), m_Sec(Blk_->m_Sec), MRootFinderBase("DutyFinder", s_Tol)//1.0e-8)
        {
        m_iFlwMode=FlwMode;
        m_iCalcMeth=CalcMeth;
        };
    LPCTSTR ObjTag()            { return (LPCTSTR)m_Blk.FullObjTag(); };
    byte   GetFlwMode()         { return m_iFlwMode; };
    void   SetFlwMode(byte FM)  { m_iFlwMode=FM; };
    byte   GetCalcMeth()        { return m_iCalcMeth; };
    void   SetCalcMeth(byte CM) { m_iCalcMeth=CM; };

    double TheoryDuty();
    double Function(double x);
  };

// ==========================================================================
//
//  Heat Exchange Flow Drop Model
//
// ==========================================================================

extern DllImportExport CFlwEqnGrp HeatXDPGroup;

DEFINE_TAGOBJ(HX_LinearEqn);
class HX_LinearEqn : public CFlwEqn
  {
  public:
    double         R_Fxd;

    HX_LinearEqn(TagObjClass* pClass_, pchar pTag, TaggedObject* pAttach, TagObjAttachment eAttach);

    virtual ~HX_LinearEqn();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
//    virtual void   StartSolution(rFlwBlkBase FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ==========================================================================
//
//  Heat Exchange Flow Drop Model
//
// ==========================================================================

DEFINE_TAGOBJ(HX_OtherEqn);
class HX_OtherEqn : public CFlwEqn
  {
  public:
    double         NoUnits, m_dP;
    C2DFn          PresFlowCurve;

    HX_OtherEqn(TagObjClass* pClass_, pchar pTag, TaggedObject* pAttach, TagObjAttachment eAttach);

    virtual ~HX_OtherEqn();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
//    virtual void   StartSolution(rFlwBlkBase FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

//============================================================================
//============================================================================

DEFINE_TAGOBJ(HX_VolFlow);
class HX_VolFlow : public CFlwEqn
  {
  public:
    double         PwrLaw, OpDP, OpVol, OpNVol, m_DP;

    HX_VolFlow(TagObjClass* pClass_, pchar pTag, TaggedObject* pAttach, TagObjAttachment eAttach);

    virtual ~HX_VolFlow();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
//    virtual void   StartSolution(rFlwBlkBase FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ==========================================================================
//
//  HeatX Utilities
//
// ==========================================================================

class DllImportExport CHXSdJoinInfo
  {
  friend class CHXJoinInfo;
  protected:
    IDMask        idMask;
    FlwNode*      pNd;
    int           iJoinId;
    flag          fOpen;
    int           ioJoin[21], ioFlwIn[20], ioFlwOut;
    SpConduit     m_Qi;
    SpConduit     m_Qo;

  public:
    CHXSdJoinInfo(FlwNode* Nd, int IdIn, int IdOut);
    flag        TstConnected();
    flag        IsConnected()   { return ioFlwOut>=0; };
    int         JoinId()        { return iJoinId;};
    int         IOJoin(int i)   { return ioJoin[i]; };
    int         IOFlwIn(int i)  { return ioFlwIn[i]; };
    int         IOFlwOut()      { return ioFlwOut; };
    IDMask      IdMask()        { return idMask; };
    SpConduit & Qi()            { return m_Qi; };
    SpConduit & Qo()            { return m_Qo; };

  };

class DllImportExport CHXJoinInfo
  {
  public:
    CHXSdJoinInfo P, S;
  protected:
    FlwNode *pNd;

  public:
    CHXJoinInfo(FlwNode* Nd, int IdPIn, int IdPOut, int IdSIn, int IdSOut) :
      P(Nd, IdPIn, IdPOut),
      S(Nd, IdSIn, IdSOut)
      {
      pNd=Nd;
      };
    void SetJoinIds(int JoinId0, int PriDirectConn=True, int SecDirectConn=True);
    flag PriIsJoined() { return P.JoinId()>=0; };
    flag SecIsJoined() { return S.JoinId()>=0; };
    flag PriIO(int i) { return ((pNd->IOIdMask_Self(i)&P.idMask)!=0); };
    flag SecIO(int i) { return ((pNd->IOIdMask_Self(i)&S.idMask)!=0); };
    flag PriConnected() { return P.IsConnected(); };
    flag SecConnected() { return S.IsConnected(); };
    int  PriJoinId() { return P.JoinId(); };
    int  SecJoinId() { return S.JoinId(); };
    int  PriOpen() { return P.fOpen; };
    int  SecOpen() { return S.fOpen; };
  };


// ==========================================================================
//
//
//
// ==========================================================================

#undef DllImportExport

#endif


