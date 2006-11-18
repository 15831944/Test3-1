//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __M_SURGE_H
#define  __M_SURGE_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_BASE_H
  #include "m_base.h"
#endif
#ifndef __M_HXBASE_H
  #include "m_hxbase.h"
#endif
#ifndef __VLEBASE_H
  #include "vlebase.h"
#endif
#include "m_area.h"

#ifdef __M_SURGE_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif
// ==========================================================================

const int IOId_HX         = FlwIOId(80);

XID xidPBSurgeCycleIt     = MdlBsXID(30000);

// ==========================================================================
// Basic Flow Object

/*#C:Unit models that have surge derive from this class */
class DllImportExport MN_Surge : public MdlNode
  {
  public:
    MN_Surge(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~MN_Surge();

            void   AddMdlClosed(DataDefnBlk & DDB);
            void   AddMdlNetworked(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual long   CheckDataRanges(bool CheckIO);

    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);

    virtual flag   InitialiseSolution();
    virtual void   SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn);
    virtual void   SetDatumsDone();
    virtual flag   Set_Sizes();
    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual long   NodeFlwTask(NodeFlwTasks Task);
    virtual void   ConfigureJoins();
    virtual void   StartStep();
//    virtual void   EvalFlowInfo();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual CSpPropInfo *IOGetNetProps(int i, double Qm);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void   EvalState();
    virtual void   EvalProductsInit(EvalProductsInitTasks Task);
    virtual void   EvalProductsSurge(CNodeEvalIndex & NEI);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   EvalDerivs(CNodeEvalIndex & NEI);
    virtual void   ODEOperate(CODEDataBlock & ODB);
    virtual void   EvalIntegral(CNodeEvalIndex & NEI);
    virtual void   ConvergeStates(CConvergeStateBlk &CSB);
    virtual void   EvalDiscrete();
    virtual void   DumpDerivs();
    virtual void   EvalStatistics(eScdCtrlTasks Tasks=CO_All);

    //virtual void   EvalIOPAverage(int AveIter);
    virtual flag   EvalPressureSens();
    virtual void   ApplyDerivs(double dTime, flag DoDbg);

    virtual void   ClosureInfo();
    
    virtual void   OnAppActivate(BOOL bActive)            { RB.OnAppActivate(bActive); };
    virtual int    FilesUsed(CFilesUsedArray & Files)     { return RB.FilesUsed(Files); };
    virtual flag   GetOtherData(FilingControlBlock &FCB)  { return RB.GetOtherData(FCB); };
    virtual flag   PutOtherData(FilingControlBlock &FCB)  { return RB.PutOtherData(FCB); };

#if DOPBSMOOTHING
    virtual void   EvalMsAGDatas();
#endif

    virtual void   EvalPBMakeUpReqd(long JoinMask);
    virtual void   EvalPBMakeUpAvail(long JoinMask);

    void           SetCyclicStorage();
    void           SetCyclicStorageOut();

    virtual dword  ModelStatus();

    DEFINE_CI(MN_Surge, MdlNode, 4);

  protected:
//    void GetStartConditions(SpContainer &C, ContStartInfo * Start);
  public:
//    flag           fDamped;
    CCnStartCnds    m_CnStart;

    CHXBase         HX;
    CReactionBase   RB;
    CVLEBase        VLE;

    CSpCOptionPtr <SpConduit>    QFeed;
    CSpCOptionPtr <SpConduit>    QProd;

    CHgtOrdInfo     ContentHgtOrd;
    SpContainer     Contents;   //The object which contains the material within the surge unit.
    SpImage         m_PresetImg;
    CSpContPreset   m_Preset;
    CSurgeControl   m_SrgCtrl;

    double         m_QmAcc;      //measures mass accumulation of material in the surge unit
    double         m_QvAcc;      //measures volume accumulation of material in the surge unit

    double         SS_Lvl;

    double         m_PrevQmV;
    double         m_PrevQmL;

    flag           bShowQFeed;
    flag           bShowQProd;
    double         POffset;
    CRqdPressCtrl  m_PCtrl0;

    struct
      {
      flag         m_bOn;
      //flag         m_bCycleIt;
      double       m_dFracRetained;
      double       m_dCycleTime;
      double       m_dTotalMass;
      double       m_dTotalVolume;
      }            m_CyclicStorage;

    CEquipmentThermals m_EqThermals;

    CDirectFlwIO   m_Spill;
    CDirectFlwIO   m_Vent;
    CDirectFlwIO   m_AccIn;
    CDirectFlwIO   m_AccOut;
    CDirectAccCalc m_AccCalc;
  };

//===========================================================================

#undef DllImportExport

#endif


