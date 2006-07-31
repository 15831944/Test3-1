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

// ==========================================================================

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
XID xidMakeupsInHz            = MdlBsXID(5022);
XID xidMakeupsInQmSum         = MdlBsXID(5023);
XID xidMakeupsInHzSum         = MdlBsXID(5024);

XID xidBleedsInQm             = MdlBsXID(5031);
XID xidBleedsInHz             = MdlBsXID(5032);
XID xidBleedsInQmSum          = MdlBsXID(5033);
XID xidBleedsInHzSum          = MdlBsXID(5034);
                                         
XID xidSpillsInQm             = MdlBsXID(5041);
XID xidSpillsInHz             = MdlBsXID(5042);
XID xidSpillsInQmSum          = MdlBsXID(5043);
XID xidSpillsInHzSum          = MdlBsXID(5044);
                                         
XID xidVentsInQm              = MdlBsXID(5051);
XID xidVentsInHz              = MdlBsXID(5052);
XID xidVentsInQmSum           = MdlBsXID(5053);
XID xidVentsInHzSum           = MdlBsXID(5054);
                                         
XID xidLeaksInQm              = MdlBsXID(5061);
XID xidLeaksInHz              = MdlBsXID(5062);
XID xidLeaksInQmSum           = MdlBsXID(5063);
XID xidLeaksInHzSum           = MdlBsXID(5064);
                                         
XID xidLeaksOutQm             = MdlBsXID(5071);
XID xidLeaksOutHz             = MdlBsXID(5072);
XID xidLeaksOutQmSum          = MdlBsXID(5073);
XID xidLeaksOutHzSum          = MdlBsXID(5074);
                                         
XID xidSrcsInQm               = MdlBsXID(5081);
XID xidSrcsInHz               = MdlBsXID(5082);
XID xidSrcsInQmSum            = MdlBsXID(5083);
XID xidSrcsInHzSum            = MdlBsXID(5084);
                                         
XID xidSnksInQm               = MdlBsXID(5091);
XID xidSnksInHz               = MdlBsXID(5092);
XID xidSnksInQmSum            = MdlBsXID(5093);
XID xidSnksInHzSum            = MdlBsXID(5094);
                                
XID xidAccumsInQm             = MdlBsXID(5101);
XID xidAccumsInHz             = MdlBsXID(5102);
XID xidAccumsInQmSum          = MdlBsXID(5103);
XID xidAccumsInHzSum          = MdlBsXID(5104);
                                         
XID xidAccumsOutQm            = MdlBsXID(5111);
XID xidAccumsOutHz            = MdlBsXID(5112);
XID xidAccumsOutQmSum         = MdlBsXID(5113);
XID xidAccumsOutHzSum         = MdlBsXID(5114);
                                         
XID xidNettInQm               = MdlBsXID(5201);
XID xidNettInHz               = MdlBsXID(5202);
XID xidNettInQmSum            = MdlBsXID(5203);
XID xidNettInHzSum            = MdlBsXID(5204);

XID xidNettInQmRel            = MdlBsXID(5211);
XID xidNettInHzRel            = MdlBsXID(5212);
XID xidNettInQmSumRel         = MdlBsXID(5213);
XID xidNettInHzSumRel         = MdlBsXID(5214);

XID xidEnvTRqd                = MdlBsXID(5301);
XID xidEnvPRqd                = MdlBsXID(5302);

XID xidIO_CopyAdtAll          = MdlBsXID(5485);
XID xidIO_CopyAdtMass         = MdlBsXID(5486);
XID xidIO_CopyAdtMassTotal    = MdlBsXID(5487);
XID xidIO_CopyAdtHeat         = MdlBsXID(5488);
XID xidIO_CopyAdtHeatTotal    = MdlBsXID(5489);


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
    double FlowHz() 
      {
      return 
        +m_Sum[AR_Srcs].FlowHz-m_Sum[AR_Snks].FlowHz
        +m_Sum[AR_Makeups].FlowHz-m_Sum[AR_Bleeds].FlowHz
        -m_Sum[AR_Spills].FlowHz-m_Sum[AR_Vents].FlowHz
        +m_Sum[AR_LeaksI].FlowHz-m_Sum[AR_LeaksO].FlowHz
        +m_Sum[AR_AccumO].FlowHz-m_Sum[AR_AccumI].FlowHz;
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
    double TotalHz() 
      {
      return 
        +m_Sum[AR_Srcs].TotalHz-m_Sum[AR_Snks].TotalHz
        +m_Sum[AR_Makeups].TotalHz-m_Sum[AR_Bleeds].TotalHz
        -m_Sum[AR_Spills].TotalHz-m_Sum[AR_Vents].TotalHz
        +m_Sum[AR_LeaksI].TotalHz-m_Sum[AR_LeaksO].TotalHz
        +m_Sum[AR_AccumO].TotalHz-m_Sum[AR_AccumI].TotalHz;
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
    double FlowHzMax() 
      {
      return Max(
                  (+m_Sum[AR_Srcs].FlowHz+m_Sum[AR_Makeups].FlowHz
                   +m_Sum[AR_LeaksI].FlowHz+m_Sum[AR_AccumO].FlowHz),
                  (+m_Sum[AR_Snks].FlowHz+m_Sum[AR_Bleeds].FlowHz+m_Sum[AR_Spills].FlowHz
                   +m_Sum[AR_Vents].FlowHz+m_Sum[AR_LeaksO].FlowHz+m_Sum[AR_AccumI].FlowHz)
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
    double TotalHzMax() 
      {
      return Max(
                  (+m_Sum[AR_Srcs].TotalHz+m_Sum[AR_Makeups].TotalHz
                   +m_Sum[AR_LeaksI].TotalHz+m_Sum[AR_AccumO].TotalHz),
                  (+m_Sum[AR_Snks].TotalHz+m_Sum[AR_Bleeds].TotalHz+m_Sum[AR_Spills].TotalHz
                   +m_Sum[AR_Vents].TotalHz+m_Sum[AR_LeaksO].TotalHz+m_Sum[AR_AccumI].TotalHz)
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
    static const byte BDDAO_Flows    = 0x01;
    static const byte BDDAO_Totals   = 0x02;
    static const byte BDDAO_Contents = 0x04;
    static const byte BDDAO_AllVis   = 0x0f;
    static const byte BDDAO_Copy     = 0x10;
    static const byte BDDAO_SrcSnk   = 0x20;

    MN_Area(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~MN_Area();
    virtual void   ResetData(flag Complete);
            void   BuildDataDefn(DataDefnBlk & DDB, byte What);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual long   CheckDataRanges(bool CheckIO);
    virtual flag   PreStartCheck();

    virtual flag   GetModelAction(Strng & Tag, MdlActionArray & Acts);
    virtual flag   SetModelAction(Strng & Tag, MdlAction & Act);

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

    //virtual void   DoFetch();
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


