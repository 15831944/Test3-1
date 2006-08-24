//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __M_LINK_H
#define  __M_LINK_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __SCDVER_H
  #include "scdver.h"
#endif
#ifndef __M_BASE_H
  #include "m_base.h"
#endif
#ifndef __SP_REACT_H
  #include "sp_react.h"
#endif
#ifndef __VLEBASE_H
  #include "vlebase.h"
#endif
#ifndef __MULTIFB_H
  #include "multifb.h"
#endif
#include "m_area.h"
#include "blockevaluator.h"
#include "evapblock.h"

#ifdef __M_LINK_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//
//
// ===========================================================================

// VolflowTypes
const byte VFFX_Off      = 0;
const byte VFFX_PreMix   = 1;
const byte VFFX_PostMix  = 2;
const byte VFFX_Plugflow = 3;

// PlugFlowTypes
const byte PFT_Incompressible = 0;
const byte PFT_Compressible   = 1;
const byte PFT_Mixed          = 2;

class DllImportExport CPlugFlowInfo
  {
  public:
    CPlugFlowInfo(FlwNode * pFlwNd, byte Method=VFFX_Off, SpContainer *pContents=NULL, CSpPlugFlow *pPlugFlow=NULL, bool Owner=true) :
      m_pFlwNd(pFlwNd),
      m_bOwner(Owner),
      m_iMethod(Method),
      m_iType(PFT_Incompressible),
      m_dLength(1.0),
      m_nSections(0),
      m_pContents(pContents),
      m_pPlugFlow(pPlugFlow)
      {
      m_bInitReqd=true;
      m_pFi=NULL;
      m_pFo=NULL;
      m_dQmIn=0.0;
      m_dQmOut=0.0;
      m_dPIn=Std_P;
      m_dPOut=Std_P;
      m_dVelS=0.0;
      m_dVelE=0.0;
      }
    ~CPlugFlowInfo()
      {
      Clear();
      }

    void Allocate(bool CnRqd, bool QRqd);
    void Clear();
    void SetData(SpContainer *pContents, CSpPlugFlow *pPlugFlow, bool Owner=true);

    byte          getMethod() const         { return m_iMethod; };
    byte          getType()   const         { return m_iType; };
    long          getSectionCount() const   { return m_nSections; };

    SpContainer   &getCn()	const   		    { return *m_pContents; };
    CSpPlugFlow   &getPf()	const	    	    { return *m_pPlugFlow; };
    bool          getCnExists() const		    { return m_pContents!=NULL; };
    bool          getPfExists() const		    { return m_pPlugFlow!=NULL; };

    double        getSrcPressure() const;
    double        getDstPressure() const;

    void          SetMethod(byte Meth);
    void          SetType(byte Type);
    void          SetSectionCount(long N);
    void          InitialisePlugFlow(SpConduit & Fi, double FillFrac);
    void          InitialisePlugFlow(double Length, double XArea, double Rise, SpConduit & Fi, double FillFrac, double PIn, double POut, double VelIn, double VelOut);
    double        DoPlugFlowProducts(SpConduit & Fi, SpConduit & Fo, double QmIn, double QmOut, double PIn, double POut, double VelS, double VelE);
    void          DoPlugFlowDiscrete(double TimeInc, bool Moved);
    void          SetPlugFlowOut(SpConduit & Fo, double Qm, double P);

    void          Sample(CFlowSamplePt & Pt);
    void          MeasureProperties(CSpPropInfoArray & Props, double StartPos, double EndPos);
    //CSpPropInfo  *SegPropsS(long Index) { return &m_SegPropsS[Index]; };
    //CSpPropInfo  *SegPropsE(long Index) { return &m_SegPropsE[Index]; };

    __declspec( property( get=getMethod) )        byte        Method;
    __declspec( property( get=getType) )          byte        Type;
    __declspec( property( get=getSectionCount) )  long        SectionCount;
    __declspec( property( get=getCn) )            SpContainer &Cn;
    __declspec( property( get=getPf) )            CSpPlugFlow &Pf;
    __declspec( property( get=getCnExists) )      bool        CnExists;
    __declspec( property( get=getPfExists) )      bool        PfExists;
    __declspec( property( get=getSrcPressure) )   double      SrcPressure;
    __declspec( property( get=getDstPressure) )   double      DstPressure;

  protected:
    FlwNode       * m_pFlwNd;
    byte            m_iMethod;
    byte            m_iType;
    double          m_dLength;
    long            m_nSections;
    SpContainer   * m_pContents;
    CSpPlugFlow   * m_pPlugFlow;
    bool            m_bOwner;
    bool            m_bInitReqd;


    // LocalInfo in case of Discrete
    SpConduit     * m_pFi;
    SpConduit     * m_pFo;
    double          m_dQmIn;
    double          m_dQmOut;
    double          m_dPIn;
    double          m_dPOut;
    double          m_dVelS;
    double          m_dVelE;

  public:
    CSpPropInfoArray m_SegPropsS, m_SegPropsE;

  };

//==========================================================================
//
//
//
//==========================================================================

#define WITHPBDPSTUFF 1
#if WITHPBDPSTUFF
// Probal Pressure drop Modes
const byte LDP_Fixed_Drop   = 0;
const byte LDP_Fixed_Boost  = 1;
const byte LDP_Linear_DP    = 2;
const byte LDP_SquareLaw_DP = 3;
const byte LDP_Const_P      = 4;
const byte LDP_Darcy_DP     = 5;
const byte LDP_BRC_DP       = 6;
#endif

// Link Flw Directions
const byte LFD_Default      = 0;
const byte LFD_Forward      = 1;
const byte LFD_Reverse      = 2;

// Link block action sequence of RB,EHX,VLE,etc
const byte BS_RB_EHX_VLE    = 0;
const byte BS_EHX_RB_VLE    = 1;

//===========================================================================

XID xidLnkMode            = MdlBsXID(9000);
XID xidSrcHgt             = MdlBsXID(9001);
XID xidSrcHFrc            = MdlBsXID(9002);
XID xidSrcAperture        = MdlBsXID(9003);
XID xidDstHgt             = MdlBsXID(9004);
XID xidDstHFrc            = MdlBsXID(9005);
XID xidDstAperture        = MdlBsXID(9006);
XID xidFlashIt            = MdlBsXID(9007);
XID xidCalcRes            = MdlBsXID(9008);
XID xidWithOP             = MdlBsXID(9009);
//XID xidTwoPhase           = MdlBsXID(9010)
XID xidVolFlwFX           = MdlBsXID(9010);
XID xidPlugFlowType       = MdlBsXID(9011);
XID xidPlugFlowSects      = MdlBsXID(9012);
XID xidVolPrsFX           = MdlBsXID(9013);
XID xidKeepSrcSnk         = MdlBsXID(9014);
XID xidCopyCurrent        = MdlBsXID(9015);
XID xidPropertySelect     = MdlBsXID(9016);

XID xidFlwPhaseRqd        = MdlBsXID(9018);
XID xidFlwPhaseAct        = MdlBsXID(9019);

XID xidTearPriority       = MdlBsXID(9020);
XID xidTearType           = MdlBsXID(9021);
XID xidTearInitWhen       = MdlBsXID(9022);
XID xidTearInitHow        = MdlBsXID(9023);
XID xidTearInitEstUsage   = MdlBsXID(9024);
XID xidTearInitActive     = MdlBsXID(9025);
XID xidTearInitActivate   = MdlBsXID(9026);
XID xidTearCntRqd         = MdlBsXID(9027);
XID xidTearCntAct         = MdlBsXID(9028);
XID xidTearState          = MdlBsXID(9029);

XID xidTearQmMode         = MdlBsXID(9030);
XID xidTearQmRqd          = MdlBsXID(9031);
XID xidTearQvRqd          = MdlBsXID(9032);
XID xidTearNQvRqd         = MdlBsXID(9033);
XID xidTearPRqd           = MdlBsXID(9034);
XID xidTearTRqd           = MdlBsXID(9035);

XID xidSI_On              = MdlBsXID(9040);
XID xidSI_Type            = MdlBsXID(9041);
XID xidSI_ResetStats      = MdlBsXID(9042);
XID xidSI_QmAve           = MdlBsXID(9043);
XID xidSI_QmTime          = MdlBsXID(9044);
XID xidSI_OnQmAve         = MdlBsXID(9045);
XID xidSI_CapQmAve        = MdlBsXID(9046);
XID xidSI_CapUtil         = MdlBsXID(9047);
XID xidSI_QvAve           = MdlBsXID(9048);
XID xidSI_SQmAve          = MdlBsXID(9049);
XID xidSI_LQmAve          = MdlBsXID(9050);

XID xidSamplePtCount      = MdlBsXID(9060);
XID xidSamplePtPos        = MdlBsXID(9061);

XID xidOldQmReqd          = MdlBsXID(9071);
XID xidOldQmRqdCapacity   = MdlBsXID(9072);
XID xidOldQmMinCapacity   = MdlBsXID(9073);
XID xidOldQmMaxCapacity   = MdlBsXID(9074);

//XID xidSpillDiscard       = MdlBsXID(9070);
//XID xidSpillTarget        = MdlBsXID(9071);
//XID xidSpillSettle        = MdlBsXID(9072);
//XID xidSpillRate          = MdlBsXID(9074);
//XID xidSpillTotal         = MdlBsXID(9075);

// ==========================================================================
//
//
//==========================================================================
#define WITHSRCSNKDIFFS 0

class CSrcSnk
  {
  public:
    CSrcSnk();
    ~CSrcSnk();
    void          CreateImage(TaggedObject * pObj);
    void          DeleteImage();

  public:
    double         dPress;
    double         dTemp;
    flag           bViewFrac;
    flag           bKeepImage;

    SpImage        *pImage;
#if WITHSRCSNKDIFFS
    double         dQmDiff;
    double         dTempDiff;
    SpImage        *pSpDiff;
#endif
  };

// ==========================================================================
//
//
//==========================================================================
class MN_Lnk;

const word SIWhatOff       = 0x0000;
const word SIWhatAll       = 0xffff;
const word SIWhatQm        = 0x0001;
const word SIWhatQv        = 0x0002;
const word SIWhatQmLiq     = 0x0004;
const word SIWhatQmSol     = 0x0008;
const word SIWhatQmCap     = 0x0010;

class DllImportExport CLinkStatInfo
  {
  public:
    CLinkStatInfo();
    virtual ~CLinkStatInfo();
    void ResetStats();
    void BuildDataDefn(DataDefnBlk & DDB, TaggedObject* pObj);
    flag DataXchg(DataChangeBlk & DCB, TaggedObject* pObj);
    void ExecIns(MN_Lnk* pPipe);

  public:
    word       iType;

    double     dTtlTime;
    double     dZeroTime;
    double     dBelowCapTime;
    double     dTotalMass;
    double     dTotalCapMass;
    double     dMaxQm;
    double     dMinQm;
    double     dTotalVol;
    double     dTotalMassS;
    double     dTotalMassL;
  };

// ==========================================================================
// Basic Flow Object
//
//
//==========================================================================

const byte XCO_Spill  = 0;
const byte XCO_Accept = 1;

//const byte FP_NULL    = 0;
//const byte FP_Liquids = 1;
//const byte FP_Gasses  = 2;
//const byte FP_Single  = 3;
//const byte FP_Either  = 4;

#define WITHLINKBLEED 0

class DllImportExport MN_Lnk : public MdlNode
  {
  friend CLinkStatInfo;
  public:
    MN_Lnk(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach, double LengthIn_ = 1.0);
    virtual ~MN_Lnk();
    virtual void   ResetData(flag Complete);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual CFlwEqnPtr * GetSystemDefnFlwEqn(int i, Strng *Tg)
      {
      if (i!=0)
        return NULL;
      if (Tg)
        *Tg="Eqn";
      return &m_FEP;
      };

    void           SetPFMethod(byte Meth, flag VolPrsFX);
    void           SetPFType(byte Type);
    void           SetPFSectionCount(long N);

    void           UpdateSQFlags();

    virtual void   GlobalConnect();
    virtual CFlange * GetFlange(int IOId);
    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual long   CheckDataRanges(bool CheckIO);
    virtual flag   GetModelAction(CMdlActionArray & Acts);
    virtual flag   SetModelAction(CMdlAction & Act);
    virtual flag   PreStartCheck();
    virtual void   SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn);
    virtual flag   Set_Sizes();

    virtual double LnkFlowDefined()
      {
      if (GetActiveHold() || !cioOn())
        return 0.0;
      else if (m_iLF_Direction==LFD_Reverse)
        return -PBQmEst;
      else
        return PBQmEst;
      };

    virtual bool   LnkFlowDefinedIn(int IONo)   
      { 
      if (GetActiveHold() || !cioOn())
        return false;
      return (m_iLF_Direction==LFD_Reverse ? MdlNode::LnkFlowDefinedOut(IONo) : MdlNode::LnkFlowDefinedIn(IONo));
      };
    virtual bool   LnkFlowDefinedOut(int IONo)  
      {
      if (GetActiveHold() || !cioOn())
        return false;
      return (m_iLF_Direction==LFD_Reverse ? MdlNode::LnkFlowDefinedIn(IONo) : MdlNode::LnkFlowDefinedOut(IONo));
      };

    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual flag   InitialiseSolution();
    virtual void   StartSolution();
    virtual bool   PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo);
    virtual void   ConfigureJoins();
    virtual void   StartStep();

    virtual void   InitFlowInfo();
    virtual void   StepFlowInfo();
    virtual void   EvalFlowInfoStart();
    virtual void   EvalFlowInfoEnd();
    virtual flag   TestFlowInfo();

    virtual void   EvalJoinPressures(long JoinMask);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void   EvalProductsInit(EvalProductsInitTasks Task);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   EvalDerivs(CNodeEvalIndex & NEI);
    virtual void   ConvergeStates(CConvergeStateBlk &CSB);
    virtual void   EvalDiscrete();
    virtual void   EvalStatistics(eScdCtrlTasks Tasks=CO_All);

    //virtual void   EvalIOPAverage(int AveIter);
    virtual flag   EvalPressureSens();

    virtual void   EvalPBMakeUpReqd(long JoinMask);
    virtual void   EvalPBMakeUpAvail(long JoinMask);

    virtual void   OnAppActivate(BOOL bActive) { m_RB.OnAppActivate(bActive); };
    virtual int    FilesUsed(CFilesUsedArray & Files) { return m_RB.FilesUsed(Files); };
    virtual flag   GetOtherData(FilingControlBlock &FCB) { return m_RB.GetOtherData(FCB); };
    virtual flag   PutOtherData(FilingControlBlock &FCB) { return m_RB.PutOtherData(FCB); };


  protected:
    CFlwThermalBlk m_FTB;
    CFlwEqnPtr     m_FEP;
    CMultiFlwBlk   m_MFB;
    bool           m_bWithOP;

    double         m_RiseRqd;
    double         m_DatumChg;

    byte           m_iLF_Direction;
    byte           m_iBlockSeq; //sequence of RB,EHX,VLE,etc
    //byte           m_iLnkFlwModeRqd;
#if WITHPBDPSTUFF
    byte           iDP_Mode;
    byte           iCalc_K;
    byte           iCalc_FFac;
#endif
    //byte           iFillMode;

    CPlugFlowInfo  m_PFI;
    flag           fVolPrsFX;
    flag           fVolIsSmall;
    flag           fVapLocked;
    flag           fTwoPhase;

#if WITHPBDPSTUFF
    double         LnkP_S, FxddP;
    double         PipeD, PipeL, FitPipeL;
    double         PipeDeltaZ;
    double         FricFac; //FricFac=Darcy Friction Factor;  note DarcyFricFact = 4 * FanningFricFact
    double         PipeScale, Vel;
    double         Visc, Rough, K;
    double         KMinorLoss;
    //double         SQmCap, SQmdP;
    //double         VQmCap, VQmdP;
    double         Re, PB_dp;
#endif
    byte           m_iFlwPhaseRqd;

    double         dMkUpRqd;

    double         DeltaZ;
    double         TotalArea;
    double         TotalLength;
    double         ActualLength;
    double         FlatLength;
    double         VertLength;
    double         FillLength;
    double         FlatFrac;
    double         ChgOvrLen;
    double         ChgOvrFrac;
    double         RhoL;
    double         RhoG;

    //double         dPolyTropicN;

#if WithOEP
    double         FixedVf;
#endif

    flag           fHasSlope;

    CLinkStatInfo *pStats;

    CIO_Digital    cioOn;
    //double         xReqd_Qm;
    //double         xRqd_QmCap, xMin_QmCap, xMax_QmCap;
    flag           fAppRhoH, fIsDegFree, fFindRhoH;

    CSrcSnk        SrcSnk;
    flag           m_fFwdOnly;
    flag           m_bAdjustToEstFlow;

  public:
    CReactionBase  m_RB;
    CVLEBase       m_VLE;
    CEvapBase      m_Evap;
#if (WITHBLOCKEVALUATOR)    
    CBlockEvaluator m_BlkEval;
#endif

    // Plugflow Data
    CArray <CFlowSamplePt, CFlowSamplePt&> m_SamplePt;
    void            SetSamplePtCount(long Count);
    long            GetSamplePtCount() { return m_SamplePt.GetSize(); };
    void            UpdateSamples();

    // Dynamic recycle init
    SpConduit     * m_pLastFlw;

    DEFINE_CI(MN_Lnk, MdlNode, 12);

#if DOPBSMOOTHING
    virtual void   EvalMsAGDatas();
#endif

    virtual dword  ModelStatus();
    virtual void   ClosureInfo();



    double         m_QmSpilt;
    byte           m_iXferCapOption;
    CDirectFlwIO   m_Spill;          
    CDirectFlwIO   m_Src;
    CDirectFlwIO   m_Snk;
#if WITHLINKBLEED 
    CDirectFlwIO   m_Bleed;
#endif

    bool           SpillFlowing()       { return m_Spill.Cd.QMass(som_ALL)>1.0e-12; };
    void           SpillClear()         { m_Spill.Cd.QZero(); };//.QSetTraceMass(); };

  protected:
    void InitLocals();
  };

//===========================================================================
//
//
//
// ===========================================================================

#undef DllImportExport

#endif


