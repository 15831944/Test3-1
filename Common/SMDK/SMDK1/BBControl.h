//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __BBCONTROL_H
#define  __BBCONTROL_H

#include "sc_defs.h"
#include "bbbase.h"

#define MDLBASE
#include "models.h"
#include "..\..\..\SMDK\Include\md_headers.h"

#ifdef __BBCONTROL_CPP
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

DEFINE_TAGOBJ(CBBControl);
class CBBControl : public MdlNode, CBBBase
  {
  public:
    CBBControl(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CBBControl();
    virtual void    SetSubClass(pchar pSubClass);

    virtual void    BuildDataDefn(DataDefnBlk & DDB);
    virtual flag    DataXchg(DataChangeBlk & DCB);
    virtual flag    ValidateData(ValidateDataBlk & VDB);
    virtual flag    PreStartCheck();
                    
    virtual flag    InitialiseSolution();
    virtual void    StartSolution();
    virtual void    StartStep();
                    
    //virtual bool    PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo);
    //virtual void    ConfigureJoins();
    //virtual void    EvalPBMakeUpReqd(long JoinMask);
    //virtual void    EvalPBMakeUpAvail(long JoinMask);
    //virtual void    EvalJoinPressures(long JoinMask);
    //virtual void    EvalJoinFlows(int JoinNo);

    //virtual CSpPropInfo* IOGetNetProps(int i, double Qm);
    //virtual flag    EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    //                
    //virtual void    EvalProducts(CNodeEvalIndex & NEI);
                    
    virtual void    EvalDiscrete();
    virtual void    EvalCtrlInitialise(eScdCtrlTasks Tasks=CO_All);
    virtual void    EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);
    virtual void    EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All);
    virtual void    EvalCtrlTerminate(eScdCtrlTasks Tasks=CO_All);
    virtual void    EvalStatistics(eScdCtrlTasks Tasks=CO_All);
                    
    virtual void    EvalPowerAvailable();
    virtual void    EvalPowerRequired();
    virtual void    EvalState();
                    
    virtual void    SetState(eScdMdlStateActs RqdState);
                    
    virtual void    ClosureInfo();
    virtual void    OnAppActivate(BOOL bActive);
    virtual int     FilesUsed(CFilesUsedArray & Files);
    virtual dword   ModelStatus();

    virtual flag    GetModelAction(CMdlActionArray & Acts);
    virtual flag    SetModelAction(CMdlAction & Act);

    virtual flag    GetModelGraphic(CMdlGraphicArray & Grfs);
    virtual flag    OperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf);
    //// FlashTrain
    //virtual void    MacroMdlEvaluate(eScdMacroMdlEvals Eval);
    //virtual flag    MacroMdlValidNd(int iIONo);
    //virtual void    MacroMdlAddNd(int iIONo);
    //virtual void    MacroMdlRemoveNd(int iIONo);
    //virtual CMacroMdlBase* MacroMdlActivate();
    //virtual void    MacroMdlDeactivate();

  protected:

  private:
    DEFINE_CI(CBBControl, FlwNode, FirstUserCI+MaxUserCI);
    DEFINE_CIVIRT(CBBControl, FlwNode);

  };

// ===========================================================================

#undef DllImportExport

#endif
