//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __BBSURGE_H
#define  __BBSURGE_H

#include "sc_defs.h"
#include "bbbase.h"

#define MDLBASE
#include "models.h"
#include "..\..\..\SMDK\Include\md_headers.h"

#ifdef __BBSURGE_CPP
  #define DllImportExport DllExport
#elif !defined(SMDK1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_TAGOBJ(CBBSurge);
class CBBSurge : public MN_Surge, CBBBase
  {
  public:
    CBBSurge(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CBBSurge();
    virtual void SetSubClass(pchar pSubClass);

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   PreStartCheck();

    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);
    virtual Z_States SetDatums(int Pass, int IOIn, double Zi, Z_States Z_State_Src);
    virtual void   SetDatumsDone();
    virtual flag   Set_Sizes();
    
    virtual flag   InitialiseSolution();
    virtual void   StartSolution();
    virtual void   StartStep();

    virtual bool   PropagateNetInfo(eScdPropagateNetTasks Task, long IONo, long Info, bool Start);
    virtual void   OnSetUserMethod();

    virtual void   ConfigureJoins();
    virtual void   EvalPBMakeUpReqd(long JoinMask);
    virtual void   EvalPBMakeUpAvail(long JoinMask);
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalJoinFlows(int JoinNo);
    virtual CSpPropInfo* IOGetNetProps(int i, double Qm);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);


    virtual void   EvalProducts(long JoinMask);
    virtual void   EvalIntegral(long JoinMask);

    virtual void   EvalDiscrete();
    virtual void   EvalCtrlInitialise();
    virtual void   EvalCtrlActions();
    virtual void   EvalCtrlStrategy();
    virtual void   EvalCtrlTerminate();
    virtual void   EvalStatistics();
    
    virtual void   EvalPowerAvailable();
    virtual void   EvalPowerRequired();
    virtual void   EvalState();

    virtual void   SetState(eScdMdlStateActs RqdState);

    virtual void   ClosureInfo();
    virtual void   OnAppActivate(BOOL bActive);
    virtual int    FilesUsed(CFilesUsedArray & Files);
    virtual dword  ModelStatus();

    // FlashTrain
    virtual void    MacroMdlEvaluate(eScdMacroMdlEvals Eval);
    virtual flag    MacroMdlValidNd(int iIONo);
    virtual void    MacroMdlAddNd(int iIONo);
    virtual void    MacroMdlRemoveNd(int iIONo);
    virtual CMacroMdlBase* MacroMdlActivate();
    virtual void    MacroMdlDeactivate();

  protected:

  private:
    DEFINE_CI(CBBSurge, MN_Surge, FirstUserCI+MaxUserCI);
    DEFINE_CIVIRT(CBBSurge, MN_Surge);

  };

// ===========================================================================

#undef DllImportExport

#endif
