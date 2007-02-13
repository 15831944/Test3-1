//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __M_XFER_H
#define  __M_XFER_H

#include <sc_defs.h>
#include "m_base.h"
#include "m_area.h"
#include "flshtrn.h"
#include "sp_react.h"

#ifdef __M_XFER_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif
// ==========================================================================
// Basic Flow Object

const int IOId_XferLeak = LeakIOId(0);

XID xidNetworkIsolator  = MdlBsXID(10000);

class DllImportExport MN_Xfer : public MdlNode
  {
  public:
    MN_Xfer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~MN_Xfer();

    virtual flag DataXchg(DataChangeBlk & DCB);
    virtual flag ValidateData(ValidateDataBlk & VDB);

    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);
    virtual flag   InitialiseSolution();
    virtual void   SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn);
    virtual flag   Set_Sizes();
    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual bool   PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo);
    virtual void   ConfigureJoins();
    virtual void   StartSolution();
    virtual void   StartStep();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void   EvalProductsInit(EvalProductsInitTasks Task);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    virtual void   EvalIntegral(CNodeEvalIndex & NEI);
    virtual void   ConvergeStates(CConvergeStateBlk &CSB);

    virtual void   EvalDiscrete();

    virtual void   DumpDerivs();
    virtual flag   ChangeMode(dword NewMode, dword OldMode);

    virtual void   EvalPBMakeUpReqd(long JoinMask);
    virtual void   EvalPBMakeUpAvail(long JoinMask);

    virtual flag   MacroMdlValidNd(int iIONo);
    virtual void   MacroMdlEvaluate(eScdMacroMdlEvals Eval);
    virtual CMacroMdlBase* MacroMdlActivate();
    virtual void   MacroMdlDeactivate();
    virtual void   ClosureInfo();

    virtual void   OnAppActivate(BOOL bActive) { m_RB.OnAppActivate(bActive); };
    virtual int    FilesUsed(CFilesUsedArray & Files) { return m_RB.FilesUsed(Files); };
    virtual flag   GetOtherData(FilingControlBlock &FCB) { return m_RB.GetOtherData(FCB); };
    virtual flag   PutOtherData(FilingControlBlock &FCB) { return m_RB.PutOtherData(FCB); };

    virtual int    ChangeTag(char * pOldTag, char * pNewTag);
    virtual int    DeleteTag(char * pDelTag);

    virtual dword  ModelStatus();

    DEFINE_CI(MN_Xfer, MdlNode, 4);

  public:
    CFT_Mixer      m_FTM;
    CSpCOptionPtr <SpConduit> m_QFeed;
    CSpCOptionPtr <SpConduit> m_QProd;

    CRqdPressCtrl  m_PCtrl0;
    CDirectFlwIO   m_Leak;


    CEnvironHXBase m_EHX;
    CReactionBase  m_RB;
    CVLEBase       m_VLE;
    CEvapBase      m_Evap;
    CBlockEvaluator m_BlkEval;

    bool           m_NetworkIsolator;

    SpContainer    * m_Accumulator;
    CDirectFlwIO   * m_AccIn;
    CDirectFlwIO   * m_AccOut;
    CDirectAccCalc * m_AccCalc;

  };

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif


