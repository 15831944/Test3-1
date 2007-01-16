//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __M_SRCSNK_H
#define  __M_SRCSNK_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_BASE_H
  #include "m_base.h"
#endif
#ifndef __RL_BASIC_H
  #include "rl_basic.h"
#endif
#ifndef __NOISE_H
  #include "noise.h"
#endif
#ifndef __VLEBASE_H
  #include "vlebase.h"
#endif
#ifndef __M_AREA_H
  #include "m_area.h"
#endif

#ifdef __M_SRCSNK_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif
// ==========================================================================

XID xidConnectTag    = MdlBsXID(4000);
XID xidConnectedTag  = MdlBsXID(4001);
XID xidQmRqd         = MdlBsXID(4002);
XID xidQvRqd         = MdlBsXID(4003);
XID xidNQvRqd        = MdlBsXID(4004);
XID xidTRqd          = MdlBsXID(4005);
XID xidPRqd          = MdlBsXID(4006);
XID xidSSVLEOnOff    = MdlBsXID(4007);
XID xidBatchAddition = MdlBsXID(4008);
XID xidSSMode        = MdlBsXID(4009);

// ==========================================================================
// Basic Flow Object
const byte  SSIO_Snk      = 0;
const byte  SSIO_Src      = 1;

const byte  SSM_Source    = 0x1;
const byte  SSM_Sink      = 0x2;
const byte  SSM_Connect   = 0x4;
const byte  SSM_ConstP    = 0x8;
const byte  SSM_ConstQm   = 0x10;
const byte  SSM_ConstQv   = 0x20;
const byte  SSM_ConstNQv  = 0x40;

enum SSQmRev { SSQmRev_Null, SSQmRev_Image, SSQmRev_InFlw };

enum SSQmRqd { SSQmRqdTot, SSQmRqdSol, SSQmRqdLiq, SSQmRqdVap };
enum VLEOpts { VLEO_Off, VLEO_SatLiqT, VLEO_SatVapT, VLEO_SatLiqP, VLEO_SatVapP, VLEO_SubLiq, VLEO_SupVap, };

_FWDDEF(MN_SrcSnk);

class DllImportExport MN_SrcSnk : public MdlNode
  {
  public:
    virtual flag   IsLnk() { return fCrossConnected && NoFlwIOs()==2; };

    MN_SrcSnk(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~MN_SrcSnk();
    virtual void   ResetData(flag Complete);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   PreStartCheck();

    virtual flag   GetModelAction(CMdlActionArray & Acts);
    virtual flag   SetModelAction(CMdlAction & Act);

    virtual long   NodeFlwTask(NodeFlwTasks Task);

    virtual void   GlobalConnect();
    virtual void   GlobalDisConnect();
    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);

//    virtual pFlwBlk FBlk(int i=0) { return i==0 ? &FB : NULL; };
    virtual void   SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn);
    virtual flag   Set_Sizes();
    virtual void   StartSolution();
    virtual void   StartStep();
    virtual bool   PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo);
    virtual void   ConfigureJoins();
    virtual flag   MoveTearFlange(int iTearIO, pFlwNode &pNewTearNd, int &iNewTearIO);
    virtual void   EvalJoinPressures(long JoinMask);
//    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int iJoinId, int FE);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void   EvalState();
    virtual void   ClosureInfo();

    virtual void   EvalDiscrete();
    virtual void   DoEvalProducts(flag Validating, double dTime);
    virtual void   EvalProductsInit(EvalProductsInitTasks Task);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   EvalDerivs(CNodeEvalIndex & NEI);

    virtual flag   InitialiseSolution();
    virtual flag   TerminateSolution();

    virtual void   EvalPBMakeUpReqd(long JoinMask);
    virtual void   EvalPBMakeUpAvail(long JoinMask);

    double OutputChg(double RqdOut, double CurOut, double Tau, double dTime)
       {
       return (RqdOut-CurOut)* (TstGTZ(Tau) ? (1.0-exp(Max(-dTime/Tau, -85.0))) : 1.0);
       }

    virtual double TRequired();
    virtual double PRequired();
    virtual double QRequired(double Rho);

    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    DeleteTag(pchar pDelTag);
    virtual int    GetConnectionTags(Strng_List & TagLst);

#if DOPBSMOOTHING
    virtual void   EvalMsAGDatas();
#endif
    virtual dword  ModelStatus();

    DEFINE_CI(MN_SrcSnk, MdlNode, 4);

  protected:
    Strng          sConnectTag;
    Strng          sConnectedTag;
    SpImage        Contents;
    SpConduit      Flows;
    flag           fCopyFlw2Contents;
    flag           fConnectionOn;
    flag           fConnectInitiator;
    flag           fCrossConnected;
    flag           fGlblConnectsInPlace;

    MN_SrcSnk     *pConnectedTo;
    int            m_iConnectedIO;
    byte           SSMode;
    byte           QmRqdMode;
    double         dTime;
    double         PFeed;
    double         dP_Std_Dev, dP_Std_Dev_C, dP_Tau, dP_Out;
    double         dQ_Std_Dev, dQ_Std_Dev_C, dQ_Tau, dQ_Out;
    double         dT_Std_Dev, dT_Std_Dev_C, dT_Tau, dT_Out;
    double         dHead;
    CNoise         P_Noise;
    CNoise         Q_Noise;
    CNoise         T_Noise;
    double         BatchAvail;
    double         dQmOut;
    int            nQmOut;
    flag           bMakeUpMode;

    CVLEBase       m_VLE;
    flag           bFlashIt;
    byte           iVLEOpt;

    CompositionFetchHelper CF;

//    flag           fOutFlwIsInFlw;
    byte           iRevFlw;

    CIO_RegulationBlk P_RegB;
    CIO_RegulationBlk T_RegB;

    byte           m_bKeepTrace;

    CDirectFlwIO   m_Src;
    CDirectFlwIO   m_Snk;

    CBlockEvaluator m_BlkEval;

  };

//===========================================================================

#undef DllImportExport

#endif


