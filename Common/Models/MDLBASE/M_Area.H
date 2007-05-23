//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __M_AREA_H
#define  __M_AREA_H

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

#ifdef __M_AREA_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

const byte CFWhenOff        = 0;
const byte CFWhenOnceOff    = 1;
const byte CFWhenContinuous = 2;

const byte CFWhatNull       = 0x00;
const byte CFWhatTemp       = 0x01;
const byte CFWhatPress      = 0x02;
const byte CFWhatFlow       = 0x04;
const byte CFWhatComp       = 0x08;

const byte CFH_OK           = 0;
const byte CFH_BadTag       = 1;
const byte CFH_ZeroFlow     = 2;

class DllImportExport CompositionFetchHelper
  {
  public:
    CompositionFetchHelper(MdlNode* pMdl);
    void   BuildDataDefn(DataDefnBlk & DDB);
    flag   DataXchg(DataChangeBlk & DCB, SpImage & Contents);
    int    ChangeTag(pchar pOldTag, pchar pNewTag);
    byte   EvalDiscrete(SpImage & Contents);
    byte   DoFetch(SpImage & Contents);

  public:
    MdlNode*       m_pMdl;
    SpConduit *    m_pCdOther;
    bool           m_bAllowFlow;
    byte           m_iCFWhen;
    byte           m_iCFWhat;
    bool           m_bCFWarnOnZero;
    Strng          m_sCFTag;
    double         m_dCFRatio;
    CArray<flag, flag&> m_CFQualsOn;
    
    static int     sm_nCFQuals;
    static CArray<Strng, Strng&> sm_CFQualsNm;
  };

// ==========================================================================

XID xidCFTag         = MdlBsXID(4950);
XID xidCFWhen        = MdlBsXID(4951);
XID xidCFFetchNow    = MdlBsXID(4952);


XID xidMakeupsInCnt           = MdlBsXID(5000);
XID xidBleedsInCnt            = MdlBsXID(5001);
XID xidSpillsInCnt            = MdlBsXID(5002);
XID xidVentsInCnt             = MdlBsXID(5003);
XID xidLeaksInCnt             = MdlBsXID(5004);
XID xidLeaksOutCnt            = MdlBsXID(5005);
XID xidSrcsInCnt              = MdlBsXID(5006);
XID xidSnksInCnt              = MdlBsXID(5007);
XID xidAccumsInCnt            = MdlBsXID(5008);

XID xidMakeupsInQm            = MdlBsXID(5021);
XID xidMakeupsInHf            = MdlBsXID(5022);
XID xidMakeupsInQmSum         = MdlBsXID(5023);
XID xidMakeupsInHfSum         = MdlBsXID(5024);

XID xidBleedsInQm             = MdlBsXID(5031);
XID xidBleedsInHf             = MdlBsXID(5032);
XID xidBleedsInQmSum          = MdlBsXID(5033);
XID xidBleedsInHfSum          = MdlBsXID(5034);
                                         
XID xidSpillsInQm             = MdlBsXID(5041);
XID xidSpillsInHf             = MdlBsXID(5042);
XID xidSpillsInQmSum          = MdlBsXID(5043);
XID xidSpillsInHfSum          = MdlBsXID(5044);
                                         
XID xidVentsInQm              = MdlBsXID(5051);
XID xidVentsInHf              = MdlBsXID(5052);
XID xidVentsInQmSum           = MdlBsXID(5053);
XID xidVentsInHfSum           = MdlBsXID(5054);
                                         
XID xidLeaksInQm              = MdlBsXID(5061);
XID xidLeaksInHf              = MdlBsXID(5062);
XID xidLeaksInQmSum           = MdlBsXID(5063);
XID xidLeaksInHfSum           = MdlBsXID(5064);
                                         
XID xidLeaksOutQm             = MdlBsXID(5071);
XID xidLeaksOutHf             = MdlBsXID(5072);
XID xidLeaksOutQmSum          = MdlBsXID(5073);
XID xidLeaksOutHfSum          = MdlBsXID(5074);
                                         
XID xidSrcsInQm               = MdlBsXID(5081);
XID xidSrcsInHf               = MdlBsXID(5082);
XID xidSrcsInQmSum            = MdlBsXID(5083);
XID xidSrcsInHfSum            = MdlBsXID(5084);
                                         
XID xidSnksInQm               = MdlBsXID(5091);
XID xidSnksInHf               = MdlBsXID(5092);
XID xidSnksInQmSum            = MdlBsXID(5093);
XID xidSnksInHfSum            = MdlBsXID(5094);
                                
XID xidAccumsInQm             = MdlBsXID(5101);
XID xidAccumsInHf             = MdlBsXID(5102);
XID xidAccumsInQmSum          = MdlBsXID(5103);
XID xidAccumsInHfSum          = MdlBsXID(5104);
                                         
XID xidAccumsOutQm            = MdlBsXID(5111);
XID xidAccumsOutHf            = MdlBsXID(5112);
XID xidAccumsOutQmSum         = MdlBsXID(5113);
XID xidAccumsOutHfSum         = MdlBsXID(5114);
                                         
XID xidNettInQm               = MdlBsXID(5201);
XID xidNettInHf               = MdlBsXID(5202);
XID xidNettInQmSum            = MdlBsXID(5203);
XID xidNettInHfSum            = MdlBsXID(5204);

XID xidNettInQmRel            = MdlBsXID(5211);
XID xidNettInHfRel            = MdlBsXID(5212);
XID xidNettInQmSumRel         = MdlBsXID(5213);
XID xidNettInHfSumRel         = MdlBsXID(5214);

XID xidEnvTRqd                = MdlBsXID(5301);
XID xidEnvPRqd                = MdlBsXID(5302);

XID xidIO_CopyAdtAll          = MdlBsXID(5485);
XID xidIO_CopyAdtMass         = MdlBsXID(5486);
XID xidIO_CopyAdtMassTotal    = MdlBsXID(5487);
XID xidIO_CopyAdtHeat         = MdlBsXID(5488);
XID xidIO_CopyAdtHeatTotal    = MdlBsXID(5489);

XID xidMUCount                = MdlBsXID(10000);
XID xidMUFlows                = MdlBsXID(10001);
XID xidMUTotal                = MdlBsXID(20000);
XID xidMUFlange               = MdlBsXID(20001);
XID xidMUDstTag               = MdlBsXID(20002);
XID xidBLCount                = MdlBsXID(30000);
XID xidBLFlows                = MdlBsXID(30001);
XID xidBLTotal                = MdlBsXID(40000);
XID xidBLFlange               = MdlBsXID(40001);
XID xidBLSrcTag               = MdlBsXID(40002);
                                          
// ==========================================================================

enum 
  { 
  AR_Balance, 
  AR_BalanceRel, 
  AR_LnksI, 
  AR_LnksO, 
  AR_Spills, 
  AR_Vents, 
  AR_LeaksI, 
  AR_LeaksO, 
  AR_Srcs, 
  AR_Snks, 
  AR_Makeups, 
  AR_Bleeds, 
  AR_AccumI, 
  AR_AccumO, 
  AR_Size 
  };

class DllImportExport CSumResult
  {
  public:
    CSumResult() {};

    void       ZeroFlows() { for (int i=0; i<AR_Size; i++) m_Sum[i].ZeroFlows(); };
    void       ZeroTotals() { for (int i=0; i<AR_Size; i++) m_Sum[i].ZeroTotals(); };

    double FlowMass() 
      {
      return 
        +m_Sum[AR_Srcs].FlowMass-m_Sum[AR_Snks].FlowMass
        +m_Sum[AR_Makeups].FlowMass-m_Sum[AR_Bleeds].FlowMass
        -m_Sum[AR_Spills].FlowMass-m_Sum[AR_Vents].FlowMass
        +m_Sum[AR_LeaksI].FlowMass-m_Sum[AR_LeaksO].FlowMass
        +m_Sum[AR_AccumO].FlowMass-m_Sum[AR_AccumI].FlowMass;
      };
    double FlowHf() 
      {
      return 
        +m_Sum[AR_Srcs].FlowHf-m_Sum[AR_Snks].FlowHf
        +m_Sum[AR_Makeups].FlowHf-m_Sum[AR_Bleeds].FlowHf
        -m_Sum[AR_Spills].FlowHf-m_Sum[AR_Vents].FlowHf
        +m_Sum[AR_LeaksI].FlowHf-m_Sum[AR_LeaksO].FlowHf
        +m_Sum[AR_AccumO].FlowHf-m_Sum[AR_AccumI].FlowHf;
      };

    double TotalMass() 
      {
      return 
        +m_Sum[AR_Srcs].TotalMass-m_Sum[AR_Snks].TotalMass
        +m_Sum[AR_Makeups].TotalMass-m_Sum[AR_Bleeds].TotalMass
        -m_Sum[AR_Spills].TotalMass-m_Sum[AR_Vents].TotalMass
        +m_Sum[AR_LeaksI].TotalMass-m_Sum[AR_LeaksO].TotalMass
        +m_Sum[AR_AccumO].TotalMass-m_Sum[AR_AccumI].TotalMass;
      };
    double TotalHf() 
      {
      return 
        +m_Sum[AR_Srcs].TotalHf-m_Sum[AR_Snks].TotalHf
        +m_Sum[AR_Makeups].TotalHf-m_Sum[AR_Bleeds].TotalHf
        -m_Sum[AR_Spills].TotalHf-m_Sum[AR_Vents].TotalHf
        +m_Sum[AR_LeaksI].TotalHf-m_Sum[AR_LeaksO].TotalHf
        +m_Sum[AR_AccumO].TotalHf-m_Sum[AR_AccumI].TotalHf;
      };

    double FlowMassMax() 
      {
      return Max(
                  (+m_Sum[AR_Srcs].FlowMass+m_Sum[AR_Makeups].FlowMass
                   +m_Sum[AR_LeaksI].FlowMass+m_Sum[AR_AccumO].FlowMass),
                  (+m_Sum[AR_Snks].FlowMass+m_Sum[AR_Bleeds].FlowMass+m_Sum[AR_Spills].FlowMass
                   +m_Sum[AR_Vents].FlowMass+m_Sum[AR_LeaksO].FlowMass+m_Sum[AR_AccumI].FlowMass)
                 );
      };
    double FlowHfMax() 
      {
      return Max(
                  (+m_Sum[AR_Srcs].FlowHf+m_Sum[AR_Makeups].FlowHf
                   +m_Sum[AR_LeaksI].FlowHf+m_Sum[AR_AccumO].FlowHf),
                  (+m_Sum[AR_Snks].FlowHf+m_Sum[AR_Bleeds].FlowHf+m_Sum[AR_Spills].FlowHf
                   +m_Sum[AR_Vents].FlowHf+m_Sum[AR_LeaksO].FlowHf+m_Sum[AR_AccumI].FlowHf)
                 );
      };

    double TotalMassMax() 
      {
      return Max(
                  (+m_Sum[AR_Srcs].TotalMass+m_Sum[AR_Makeups].TotalMass
                   +m_Sum[AR_LeaksI].TotalMass+m_Sum[AR_AccumO].TotalMass),
                  (+m_Sum[AR_Snks].TotalMass+m_Sum[AR_Bleeds].TotalMass+m_Sum[AR_Spills].TotalMass
                   +m_Sum[AR_Vents].TotalMass+m_Sum[AR_LeaksO].TotalMass+m_Sum[AR_AccumI].TotalMass)
                 );
      };
    double TotalHfMax() 
      {
      return Max(
                  (+m_Sum[AR_Srcs].TotalHf+m_Sum[AR_Makeups].TotalHf
                   +m_Sum[AR_LeaksI].TotalHf+m_Sum[AR_AccumO].TotalHf),
                  (+m_Sum[AR_Snks].TotalHf+m_Sum[AR_Bleeds].TotalHf+m_Sum[AR_Spills].TotalHf
                   +m_Sum[AR_Vents].TotalHf+m_Sum[AR_LeaksO].TotalHf+m_Sum[AR_AccumI].TotalHf)
                 );
      };

  public:
    //CString     m_Tag;    
    CAreaSum    m_Sum[AR_Size];
    FlwNode   * m_pNd;
  };

class DllImportExport MN_Area : public MdlNode
  {
  public:
    static const dword BDDAO_Flows    = 0x00000001;
    static const dword BDDAO_Totals   = 0x00000002;
    static const dword BDDAO_Contents = 0x00000004;
    static const dword BDDAO_MULinks  = 0x00000008;
    static const dword BDDAO_BLLinks  = 0x00000010;
    static const dword BDDAO_AllVis   = 0x0000ffff;
    static const dword BDDAO_Copy     = 0x00010000;
    static const dword BDDAO_SrcSnk   = 0x00020000;

    MN_Area(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~MN_Area();
    virtual void   ResetData(flag Complete);
            void   BuildDataDefn(DataDefnBlk & DDB, dword  What);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual long   CheckDataRanges(bool CheckIO);
    virtual flag   PreStartCheck();

    virtual flag   GetModelAction(CMdlActionArray & Acts);
    virtual flag   SetModelAction(CMdlAction & Act);

    virtual long   NodeFlwTask(NodeFlwTasks Task);

    //virtual flag   GlobalConnect();
    //virtual flag   GlobalDisConnect();
    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);
    virtual flag   IsValidAuditNode()   { return true; };

    virtual void   SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn);
    //virtual flag   Set_Sizes();
    virtual void   StartSolution();
    virtual void   StartStep();
    virtual void   ConfigureJoins();
    virtual eEvalSeqStart IOEvalSeqStart_Self(int i);
    //virtual flag   MoveTearFlange(int iTearIO, pFlwNode &pNewTearNd, int &iNewTearIO);
    virtual void   EvalJoinPressures(long JoinMask);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void   EvalState();
    virtual void   ClosureInfo();

    virtual void   EvalDiscrete();
    virtual void   EvalAudit(int Pass);
    virtual void   GetBalanceReport(CNodeAuditInfo & Audit);
    virtual void   DoEvalProducts(flag Validating, double dTime);
    virtual void   EvalProductsInit(EvalProductsInitTasks Task);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   EvalDerivs(CNodeEvalIndex & NEI);
    //virtual void   PostEvalDerivs();

    virtual flag   InitialiseSolution();
    virtual flag   TerminateSolution();

    virtual void   SetState(eScdMdlStateActs RqdState);

    //virtual void   EvalPBMakeUpReqd(long JoinMask);
    //virtual void   EvalPBMakeUpAvail(long JoinMask);

    virtual double PRequired();
    virtual double TRequired();

    virtual dword  ModelStatus();

    DEFINE_CI(MN_Area, MdlNode, 4);

  protected:

    bool            m_bDoAudit;
    bool            m_bForceAudit;
    dword           m_dwAreaAuditOptions;

    SpImage         m_Content;

    CSumResult      m_OvResults;
    CArray<CSumResult*, CSumResult*> m_NdResults;

    int             m_nCounts[AR_Size];

    double          m_PFeed;
    double          m_TFeed;
      
  };

//===========================================================================

#undef DllImportExport

#endif


