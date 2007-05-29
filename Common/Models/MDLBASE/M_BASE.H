//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MDLNODE_H
#define  __MDLNODE_H

#ifndef  __SCDVER_H
  #include "scdver.h"
#endif
#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __SP_CONT_H
  #include "sp_cont.h"
#endif
#ifndef __ODESOLVE_H
  #include "odesolve.h"
#endif
#ifndef __ERRORLOG_H
  #include "errorlog.h"
#endif
//#ifndef __MTX_SOLV_H
//  #include "mtx_solv.h"
//#endif
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif
#ifndef __CTRL_IO_H
  #include "ctrl_io.h"
#endif
#ifndef __GSMODEL_H
  #include "gsmodel.h"
#endif
#ifndef __SP_QUEUE_H
  #include "sp_queue.h"
#endif
#include "sp_react.h"
#include "blockevaluator.h"

#ifdef __M_BASE_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================
// ===========================================================================


//============================================================================
// ===========================================================================

//const byte SC_None    = 0; NOT USED
const byte SC_Level   = 1;
const byte SC_Mass    = 2;
const byte SC_Volume  = 3;

class DllImportExport CSurgeControl
  {
  public:
    CSurgeControl();
    //void DoEvalCtrl(FlwNode* pNd);
    //void CalcIOOut(FlwNode* pNd);
    void      BuildDataDefn(DataDefnBlk & DDB, LPCSTR Tag, FlwNode* pNd);
    int       StreamIndex() { return (m_bEnabled && m_bOn) ? m_iStreamIndex : -1; };
    void      StartStep();

  public:
    flag      m_bEnabled;
    flag      m_bOn;
    byte      m_iType;
    double    m_dLevelSPT;      //Frac
    double    m_dMassSPT;       //kg
    double    m_dVolumeSPT;     //m^3
    short     m_iStreamIndex;

#if WITHSURGECTRLDOUBLE 
    double    m_dQmCalc;        //kg/s

    double    m_dQmMin;         //kg/s
    double    m_dQmMax;         //kg/s
    double    m_dQmMinStep;     //kg/s
    double    m_dQmMaxStep;     //kg/s
    double    m_dQmMaxSlewUp;   //kg/s/s
    double    m_dQmMaxSlewDn;   //kg/s/s
#else
    float     m_dQmCalc;        //kg/s

    float     m_dQmMin;         //kg/s
    float     m_dQmMax;         //kg/s
    float     m_dQmMinStep;     //kg/s
    float     m_dQmMaxStep;     //kg/s
    float     m_dQmMaxSlewUp;   //kg/s/s
    float     m_dQmMaxSlewDn;   //kg/s/s
#endif
  };

//==========================================================================

class Xfer_EvalProducts_Info
  {
  friend class MdlNode;
  protected:
    int             iJoinId;
    double          dPress;
    SpConduit      *pQi;
    SpConduit      *pQo;
    CReactionBlock *pRB;
    GSModel        *pGSM;
    CEnvironHX     *pEHX;
    CRqdPressCtrl   *pPBP;

    int             Io[MaxIOList+1];

  public:

    Xfer_EvalProducts_Info(int JoinId, double Press,  SpConduit *Qi=NULL, SpConduit *Qo=NULL,
                           GSModel * GSM=NULL, CRqdPressCtrl *PBP=NULL, 
                           CReactionBlock *RB=NULL, CEnvironHX * EHX=NULL)
      {
      iJoinId = JoinId;
      dPress  = Press;
      pQi  = Qi;
      pQo  = Qo;
      pRB  = RB;
      pGSM = GSM;
      pEHX = EHX;
      pPBP = PBP;
      };
  };

//--------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------

typedef CArray   <double, double> PMUImgArray;

//--------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------

class CSetDatumsData
  {
  public:
    IDMask        m_IOIdMask;
    SpContainer * m_pCn;
    double        m_dShpBaseFromDatum;
    double        m_dShpHeight;
    double        m_dShpDatumPt;
    double        m_dLowestIOPt;
  };

class SortSurgeIOData
  {
  public:
    IDMask        m_IOIdMask;
    SpContainer * m_pCn;
    CHgtOrdInfo * m_pOrd;
  };

class MdlNode;


#define CNMTOREMOVE   0

class CSigmaIOFlows
  {
  public:
    enum TestResult { Balanced, BadMass, BadHeat, BadBalance};
    CSigmaIOFlows() { Clear(); }

    void Clear() { Mass=Heat=MaxMass=MaxHeat=0; };
    TestResult Test();

    double Mass;
    double Heat;
    double MaxMass;
    double MaxHeat;
  };

const dword  MNAudit_Qm     = 0x00000001;
const dword  MNAudit_Hz     = 0x00000002;
const dword  MNAudit_Cnts   = 0x00000004;

class DllImportExport MdlNode : public FlwNode
  {
  public:
    MdlNode(TagObjClass * pClass_, char* TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~MdlNode();

    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual long   CheckDataRanges(bool CheckIO);

    virtual void   BuildCommonEndDefn(DataDefnBlk & DDB);
    void           BuildCommonEndDefnB481(DataDefnBlk &DDB);

//    virtual void   InitChangeTrace();
//    virtual void   TraceChanges();
//    virtual CTraceArray * GetChangeTrace(long i, dword Flags);
//
//    void    ChngTrcBuildDataDefn(DataDefnBlk & DDB);

    // Performs the basic EvalProducts for Ties
    void           Xfer_EvalProductsIn_(int JoinId, double & Press, SpConduit *pQi, SpConduit *pQo, CRqdPressCtrl *pPC);
    void           Xfer_EvalProductsWork_(SpConduit *pQi, SpConduit *pQo, CReactionBlock *pRB, CEnvironHX * pEHX, CBlockEvaluator *pBlkEval = NULL);
    void           Xfer_EvalProductsOut_(int JoinId, SpConduit *pQi, SpConduit *pQo, GSModel * pGSM, CRqdPressCtrl *pPC);
    virtual void   Xfer_EvalProducts(int JoinId, double Press, SpConduit *pQi, SpConduit *pQo, GSModel * pGSM, CRqdPressCtrl *pPC, CReactionBlock *pRB, CEnvironHX * pEHX);
    virtual void   Xfer_EvalProducts(int JoinId, double Press, SpConduit *pQi, SpConduit *pQo, GSModel * pGSM, CRqdPressCtrl *pPC, CBlockEvaluator *pBlkEval);
    virtual void   Xfer_EvalProducts(Xfer_EvalProducts_Info & S1, Xfer_EvalProducts_Info & S2, CHXBase * pHX);
    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);

    // Arranges Outputs in ascending order of height
    virtual void   GetCnFlows(bool GetActualQm, CHgtOrdInfo &HO, int IONo, double &QmSLFeed, double &QvSLFeed, double &QvSLOutBelow);
    virtual double GetSVolFrac(SpContainer & Cn,
                               long IONo, double Aperture, double FHgt, double QvSLRqd,
                               double QvSLFeed, double QvSLOutBelow,
                               SpMArray *pImage);
    virtual void   DoIOGetNetProps_Surge(int i, double Qm);

    // Derivative Evaluation Utilities
    virtual void   SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpMArray &S, double Temperature, double Pressure, SpModel * pQualMdl);
    virtual void   SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, PMUImgArray &S, double Temperature, double Pressure, SpModel * pQualMdl);
    virtual void   SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpContainer & Contents, PhMask PhaseM);
    virtual void   SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpContainer & Contents, double SolFrac, double LiqFrac, double VapFrac);
    virtual void   SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpContainer & Contents, double ReqdValue);
    virtual void   SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpConduit & C, PhMask PhaseM);
    virtual void   SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpConduit & C, double SolFrac, double LiqFrac, double VapFrac);
    virtual void   SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpConduit & C, double ReqdValue);

    virtual eEvalSeqStart IOEvalSeqStart_Self(int i);

    virtual double SigmaQInPMax(SpConduit & Qi, PhMask PhaseM, IDMask IOIds, bool PRmt=false, TagObjClass * RqdMdl=NULL);
    virtual double SigmaQInPMin(SpConduit & Qi, PhMask PhaseM, IDMask IOIds, bool PRmt=false, TagObjClass * RqdMdl=NULL);
    virtual double SigmaQInPMax(SpConduit & Qi, PhMask PhaseM, int IOId1, int IOIdN, bool PRmt=false, TagObjClass * RqdMdl=NULL);
    virtual double SigmaQInPMin(SpConduit & Qi, PhMask PhaseM, int IOId1, int IOIdN, bool PRmt=false, TagObjClass * RqdMdl=NULL);

    virtual void   SigmaQOut(SpConduit & Qo, PhMask PhaseM, IDMask IOIds, bool PRmt=false);
    virtual double SigmaQv(PhMask PhaseM, IDMask IOIds);
    virtual double SigmaQvIn(PhMask PhaseM, IDMask IOIds);
    virtual double SigmaQvOut(PhMask PhaseM, IDMask IOIds);

    virtual void   EvalProductsInit_Source(EvalProductsInitTasks Task, int IoNo, double SLQm, double VQm);
    virtual void   EvalProductsInit_SurgeLevel(EvalProductsInitTasks Task, SpContainer & Contents, IDMask IOIds);
    virtual void   EvalProductsInit_Node(EvalProductsInitTasks Task, IDMask IOIds);
    virtual void   EvalProductsInit_Link(EvalProductsInitTasks Task);

    void           VentVapours(SpContainer & Contents);
    void           DoOverflowLinks(CHgtOrdInfo & Info);
    void           DoGasVentLinks(CHgtOrdInfo & Info);

    virtual void   EvalProducts_SurgeLevel(CHgtOrdInfo & HgtOrdInfo, SpConduit * pQFeed, CSurgeControl *pControl=NULL);
    virtual void   EvalProducts_SurgeLevelInline(CHgtOrdInfo & HgtOrdInfo, SpConduit * pQFeed, CSurgeControl *pControl=NULL);
    virtual void   EvalProducts_SurgeLevelDeriv(CHgtOrdInfo & HgtOrdInfo, SpConduit * pQFeed, CSurgeControl *pControl=NULL);
    //virtual void   EvalProducts_SurgeLevel(SpContainer & Contents, SpConduit & QInEst, IDMask IOIds, PMUCfgBlk * pPMU=NULL);
    //virtual void   EvalProducts_SurgeImage(SpContainer & Contents, IDMask IOIds);
    virtual void   EvalDerivs_SetQs_Xfer(SpContainer & Contents, IDMask IOIds);
    virtual void   ClosureInfo();

    void           MakeUpNodeTransferAvail(int JoinId);
    void           MakeUpNodeTransferReqd(int JoinId);
    flag           MakeUpNodeEvalProducts(int JoinId);
    flag           MakeUpNodeEvalProducts(int JoinId, SpConduit &Sd);
    void           MakeUpLinkTransferAvail();
    void           MakeUpLinkTransferReqd();

    // Physical Layout (Datums Pipelength) Calculations & Checks.
    virtual void   SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn);
    virtual flag   Set_Sizes();
//    virtual Z_States SetDatums_Node(int Pass, double ShpHgt, int IOIn, double Zi, Z_States Z_State_Src);
    virtual void   SetDatums_Node(int Pass, CFlwNodeIndexList & List, int IOIn, CSetDatumsData * Data);
    virtual void   SetDatums_Link(int Pass, CFlwNodeIndexList & List, int IOIn, double RiseRqd, double DatumChg);
    virtual void   SortSurgeIO(SortSurgeIOData * Data);

    //  Helper Functions
    void           BuildDataDefnIOOptions(DataDefnBlk & DDB);
    void           BuildDataDefnShowIOs(DataDefnBlk & DDB, flag bWithSigma=True);
    void           FillIOIndexList(int JoinId, int *I);
    void           BuildDataDefnIOOpts(DataDefnBlk & DDB);

    void           SigmaIOFlows(CSigmaIOFlows & Sigma);

    DEFINE_CI(MdlNode, FlwNode, 16);

  private:
    void SetOutputFlows(CHgtOrdInfo & HO, int IONo, double LVolFrac, double VVolFrac);

  public:
    double         FlowIn;
    GSModelBase    GSM;
    //CEnvironHXBase EHX;

    flag           bNodeMakeUpReqd; // Set If Makeup Defines splits through the node

    dword          m_dwBaseAuditOptions;

  };

// ---------------------------------------------------------------------------

#define MN(p) ((pMdlNode)p)

// ---------------------------------------------------------------------------

  XID xidRho                    = MdlBsXID(10);
  XID xidNRho                   = MdlBsXID(11);
  XID xidQmEst                  = MdlBsXID(12);
  XID xidQmEstSrc               = MdlBsXID(13);
  XID xidQmEstType              = MdlBsXID(14);
  XID xidQm                     = MdlBsXID(16);
  XID xidQi                     = MdlBsXID(17);
  XID xidQo                     = MdlBsXID(18);
  XID xidQv                     = MdlBsXID(19);
  XID xidNQv                    = MdlBsXID(20);
  XID xidLevel                  = MdlBsXID(21);
  XID xidMomentum               = MdlBsXID(22);
  XID xidNettBoost              = MdlBsXID(23);
  XID xiddP                     = MdlBsXID(24);
  XID xidPressMethod            = MdlBsXID(25);
  XID xidPoutRqd                = MdlBsXID(26);
  XID xidPBoostRqd              = MdlBsXID(27);
  XID xidPDropRqd               = MdlBsXID(28);
  XID xidPIn                    = MdlBsXID(29);
  XID xidPOut                   = MdlBsXID(30);
  XID xidPInEst                 = MdlBsXID(31);
  XID xidPOutEst                = MdlBsXID(32);
  XID xidPInRmt                 = MdlBsXID(33);
  XID xidPOutRmt                = MdlBsXID(34);
  XID xidPMean                  = MdlBsXID(35);
  XID xidPEstMean               = MdlBsXID(36);
  XID xidDPInRmt                = MdlBsXID(37);
  XID xidDPOutRmt               = MdlBsXID(38);
  XID xidDPRmt                  = MdlBsXID(39);

  XID xidSolFrac                = MdlBsXID(42);
  XID xidLiqFrac                = MdlBsXID(43);
  XID xidLiqFracIn              = MdlBsXID(44);
  XID xidLiqFracOut             = MdlBsXID(45);
  XID xidVapFrac                = MdlBsXID(46);
  XID xidVapFracIn              = MdlBsXID(47);
  XID xidVapFracOut             = MdlBsXID(48);
  XID xidMoleVapFrac            = MdlBsXID(49);

  XID xidTemp                   = MdlBsXID(50);
  XID xidTempIn                 = MdlBsXID(51);
  XID xidTempOut                = MdlBsXID(52);

  XID xidToutRqd                = MdlBsXID(57);
  XID xidDutyRqd                = MdlBsXID(58);
  XID xidDuty                   = MdlBsXID(59);

  XID xidUDFActCnt              = MdlBsXID(60);
  XID xidUDFOpenP               = MdlBsXID(61);
  XID xidUDFState               = MdlBsXID(62);

  XID xidClosed                 = MdlBsXID(63);
  XID xidVelocity               = MdlBsXID(64);
  XID xidChokeVelocity          = MdlBsXID(65);
  XID xidNetPressDamp           = MdlBsXID(66);

  XID xidInlineIntegralOn       = MdlBsXID(68);
  //XID xidLevelControl           = MdlBsXID(69);

#define WITHIOSENERGY 0

  XID xidIO_QmTotalIn           = MdlBsXID(70);
  XID xidIO_QmTotalOut          = MdlBsXID(71);
  XID xidIO_QmNett              = MdlBsXID(72);
  XID xidIO_QmBalance           = MdlBsXID(73);
  XID xidIO_QmBalanceRel        = MdlBsXID(74);
  XID xidIO_QmNettTotal         = MdlBsXID(75);
  //XID xidIO_QmNettTotalRel      = MdlBsXID(76);
  XID xidIO_QmBalanceTotal      = MdlBsXID(77);
  XID xidIO_QmBalanceTotalRel   = MdlBsXID(78);

  XID xidIO_HfTotalIn           = MdlBsXID(80);
  XID xidIO_HfTotalOut          = MdlBsXID(81);
  XID xidIO_HfNett              = MdlBsXID(82);
  XID xidIO_HfBalance           = MdlBsXID(83);
  XID xidIO_HfBalanceRel        = MdlBsXID(84);
  XID xidIO_HfNettTotal         = MdlBsXID(85);
  //XID xidIO_HfNettTotalRel      = MdlBsXID(86);
  XID xidIO_HfBalanceTotal      = MdlBsXID(87);
  XID xidIO_HfBalanceTotalRel   = MdlBsXID(88);

  XID xidIO_LnkCount            = MdlBsXID(100);
  XID xidIO_DirCount            = MdlBsXID(101);
  XID xidIO_AdtCount            = MdlBsXID(102);

#if WITHIOSENERGY
  XID xidIO_QvTotalIn           = MdlBsXID(108);
  XID xidIO_QvTotalOut          = MdlBsXID(109);

  XID xidIO_HsTotalIn           = MdlBsXID(110);
  XID xidIO_HsTotalOut          = MdlBsXID(111);
  XID xidIO_HsSigma             = MdlBsXID(112);
  XID xidIO_HfTotalIn           = MdlBsXID(113);
  XID xidIO_HfTotalOut          = MdlBsXID(114);
  XID xidIO_HfSigma             = MdlBsXID(115);
  XID xidIO_HzTotalIn           = MdlBsXID(116);
  XID xidIO_HzTotalOut          = MdlBsXID(117);
  XID xidIO_HzSigma             = MdlBsXID(118);
#endif

  XID xidIO_MkUpAvail0          = MdlBsXID(120);
  XID xidIO_MkUpAvail1          = MdlBsXID(121);
  XID xidIO_MkUpReqd0           = MdlBsXID(122);
  XID xidIO_MkUpReqd1           = MdlBsXID(123);

  XID xidIO_QmAvail             = MdlBsXID(124);
  XID xidIO_QmSpace             = MdlBsXID(125);

  XID xidChgErr                 = MdlBsXID(126);
  XID xidChgChg                 = MdlBsXID(127);
  XID xidChgOsc                 = MdlBsXID(128);

  XID xidHzIn                   = MdlBsXID(140);
  XID xidHzOut                  = MdlBsXID(141);
  XID xidHzInRmt                = MdlBsXID(142);
  XID xidHzOutRmt               = MdlBsXID(143);
  XID xidtotHzIn                = MdlBsXID(144);
  XID xidtotHzOut               = MdlBsXID(145);
  XID xidtotHzInRmt             = MdlBsXID(146);
  XID xidtotHzOutRmt            = MdlBsXID(147);

  XID xidIO_JoinMassFlwIn       = MdlBsXID(150);
  XID xidIO_JoinMassGain        = MdlBsXID(151);
  XID xidIO_JoinMassFlwOut      = MdlBsXID(152);
  XID xidIO_JoinFeedCpT         = MdlBsXID(153);
  XID xidIO_JoinProdCpT         = MdlBsXID(154);
  XID xidIO_JoinHfIn            = MdlBsXID(155);
  XID xidIO_JoinHfGain          = MdlBsXID(156);
  XID xidIO_JoinHfOut           = MdlBsXID(157);
  XID xidIO_JoinHsIn            = MdlBsXID(158);
  XID xidIO_JoinHsGain          = MdlBsXID(159);
  XID xidIO_JoinHsOut           = MdlBsXID(160);
  XID xidIO_JoinPowerIn         = MdlBsXID(161);
  XID xidIO_JoinHOR_Std0        = MdlBsXID(162);
  XID xidIO_JoinHOR_StdT        = MdlBsXID(163);
  XID xidIO_JoinHOR_Mdl0        = MdlBsXID(164);
  XID xidIO_JoinHOR_MdlT        = MdlBsXID(165);
  XID xidIO_JoinHOR_Used0       = MdlBsXID(166);
  XID xidIO_JoinHOR_UsedT       = MdlBsXID(167);
  XID xidIO_JoinHOR_Diff0       = MdlBsXID(168);
  XID xidIO_JoinHOR_DiffT       = MdlBsXID(169);
  XID xidIO_JoinHsChg           = MdlBsXID(170);
  XID xidIO_JoinHfChg           = MdlBsXID(171);

  XID xidIO_RCTMassGain         = MdlBsXID(172);
  XID xidIO_RCTHfGain           = MdlBsXID(173);
  XID xidIO_RCTHsGain           = MdlBsXID(174);
  XID xidIO_RCTPowerIn          = MdlBsXID(175);
  XID xidIO_RCTHOR_Std0         = MdlBsXID(177);
  XID xidIO_RCTHOR_Mdl0         = MdlBsXID(178);
  XID xidIO_RCTHOR_Used0        = MdlBsXID(179);
  XID xidIO_RCTHOR_Diff0        = MdlBsXID(180);
  XID xidIO_VLEHsGain           = MdlBsXID(181);
  XID xidIO_EHXPowerIn          = MdlBsXID(182);
  
  XID xidIO_CopyClsCompact      = MdlBsXID(183);
  XID xidIO_CopyClsFull         = MdlBsXID(184);
    
  XID xidIO_InCnt               = MdlBsXID(190);
  XID xidIO_OutCnt              = MdlBsXID(191);
  XID xidIO_Count               = MdlBsXID(192);
  XID xidIO_ExtQm               = MdlBsXID(193);
  XID xidIO_ExtHs               = MdlBsXID(194);
  XID xidIO_ExtQmTotal          = MdlBsXID(195);
  XID xidIO_ExtHsTotal          = MdlBsXID(196);

  XID xidIO_StreamTag0          = MdlBsXID(200+0*MaxIOList); // Leave 'MaxIOList' Ids After this
  XID xidIO_IoQm0               = MdlBsXID(200+1*MaxIOList); // Leave 'MaxIOList' Ids After this
  XID xidIO_DrQm0               = MdlBsXID(200+2*MaxIOList); // Leave 'MaxIOList' Ids After this
  XID xidIO_IoHf0               = MdlBsXID(200+3*MaxIOList); // Leave 'MaxIOList' Ids After this
  XID xidIO_DrHf0               = MdlBsXID(200+4*MaxIOList); // Leave 'MaxIOList' Ids After this
  XID xidIO_IoQmTotal0          = MdlBsXID(200+5*MaxIOList); // Leave 'MaxIOList' Ids After this
  XID xidIO_DrQmTotal0          = MdlBsXID(200+6*MaxIOList); // Leave 'MaxIOList' Ids After this
  XID xidIO_IoHfTotal0          = MdlBsXID(200+7*MaxIOList); // Leave 'MaxIOList' Ids After this
  XID xidIO_DrHfTotal0          = MdlBsXID(200+8*MaxIOList); // Leave 'MaxIOList' Ids After this
  
  XID xidIO_Hgt0                = MdlBsXID(100000+0*MaxIOList);
  XID xidIO_HFrc0               = MdlBsXID(100000+1*MaxIOList);
  XID xidIO_Aperture0           = MdlBsXID(100000+2*MaxIOList);
  XID xidIO_ApertureAng0        = MdlBsXID(100000+3*MaxIOList);
  XID xidIO_EntrainRateA0       = MdlBsXID(100000+4*MaxIOList);
  XID xidIO_EntrainRateB0       = MdlBsXID(100000+5*MaxIOList);
  XID xidIO_ExitRule0           = MdlBsXID(100000+6*MaxIOList);
  XID xidIO_EstV2L0             = MdlBsXID(100000+7*MaxIOList);
  XID xidIO_EstL2V0             = MdlBsXID(100000+8*MaxIOList);

// ---------------------------------------------------------------------------

#define IMPLEMENT_MODELUNIT(ClassName, Name, Version, Draw, DrwGroup, TagInitialID, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJ(ClassName, FlwUnitGrp, Name, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc); \
  NodeDrawEntry DrwDef_##ClassName(Name, Version, DrwGroup, Draw);                                \
  CClassLicenseOptions ClsLic_##ClassName(ClassName##Class, true);

#define IMPLEMENT_MODELUNITHIDDEN(ClassName, Name, Version, Draw, DrwGroup, TagInitialID, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJSLCT(ClassName, FlwUnitGrp, Name, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc, 0x00000000); \
  NodeDrawEntry DrwDef_##ClassName(Name, Version, DrwGroup, Draw);                                \
  CClassLicenseOptions ClsLic_##ClassName(ClassName##Class, true);

#define IMPLEMENT_MODELLINK(ClassName, Name, Version, TagInitialID, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJ(ClassName, FlwLinkGrp, Name, Version, "Pipe", TagInitialID, Cat, ShortDesc, Desc) \
  CClassLicenseOptions ClsLic_##ClassName(ClassName##Class, true);

#define IMPLEMENT_CONTROLLINK(ClassName, Name, Version, TagInitialID, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJ(ClassName, CtrlLinkGrp, Name, Version, "Control", TagInitialID, Cat, ShortDesc, Desc) \
  CClassLicenseOptions ClsLic_##ClassName(ClassName##Class, true);

#define IMPLEMENT_ELECLINK(ClassName, Name, Version, TagInitialID, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJ(ClassName, ElecLinkGrp, Name, Version, "Electrical", TagInitialID, Cat, ShortDesc, Desc) \
  CClassLicenseOptions ClsLic_##ClassName(ClassName##Class, true);

#define IMPLEMENT_AIRLINK(ClassName, Name, Version, TagInitialID, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJ(ClassName, AirLinkGrp, Name, Version, "Air", TagInitialID, Cat, ShortDesc, Desc) \
  CClassLicenseOptions ClsLic_##ClassName(ClassName##Class, true);

//#define IMPLEMENT_INODE(ClassName, Name, Version, TagInitialID, Cat, ShortDesc, Desc) \
//  IMPLEMENT_TAGOBJ(ClassName, "INode", Name, Version, "INode", TagInitialID, Cat, ShortDesc, Desc)

// ===========================================================================

class DllImportExport CCnStartCnds
  {
  public:
    CCnStartCnds()
      {
      m_bStepStartDone=false;
      m_dTemp=Std_T;
      m_dPress=Std_P;
      m_dLevel=0.0;
      m_dMass=0.0;
      };

    void AtStart(SpContainer & Cn)
      {
      m_bStepStartDone=true;
      m_dTemp=Cn.Temp();
      m_dPress=Cn.Press();
      m_dLevel=Cn.Level();
      m_dMass=Cn.Mass();
      }

    double DTemp(SpContainer & Cn) { return m_bStepStartDone ? Cn.Temp()-m_dTemp : 0; };
    double DPress(SpContainer & Cn) { return m_bStepStartDone ? Cn.Press()-m_dPress : 0; };
    double DLevel(SpContainer & Cn) { return m_bStepStartDone ? Cn.Level()-m_dLevel : 0; };
    double DMass(SpContainer & Cn) { return m_bStepStartDone ? Cn.Mass()-m_dMass : 0; };

  public:
    bool           m_bStepStartDone;
    double         m_dTemp;
    double         m_dPress;
    double         m_dLevel;
    double         m_dMass;

  };

// ===========================================================================

XID xidEqThermTau1     = MdlBsXID(20100);
XID xidEqThermTau2     = MdlBsXID(20101);
XID xidEqThermAmbT     = MdlBsXID(20102);

class DllImportExport CEquipmentThermals
  {
  public:

    TaggedObject * m_pObj;

    flag           m_bActive;
    flag           m_bConfigured;
    SpContainer  * m_pCn;
    double         m_dMass;
    double         m_dCp;
    double         m_dTemp;
    double         m_dTempDT;
    DualDbl        m_dAmbient;
    double         m_dHTC1;
    double         m_dHTC2;
    double         m_dPwr1;
    double         m_dPwr2;

    CStateArray    m_States;                // Array of Integrators

    CEquipmentThermals(TaggedObject * pObj, SpContainer * pCn);
    ~CEquipmentThermals();

    void BuildDataDefn(DataDefnBlk & DDB, char* pTag_=NULL, char* pTagComment=NULL, DDBPages PageIs = DDB_OptPage, dword UserInfo=0);
    flag DataXchg(DataChangeBlk & DCB);

    flag ValidateData(ValidateDataBlk & VDB);
    void EvalDerivs();
    void ConvergeStates(CConvergeStateBlk &CSB);

    void SetStateAction(IE_Enables E);
    void ODEOperate(CODEDataBlock & ODB);

  };

// ===========================================================================

#undef DllImportExport

#endif
