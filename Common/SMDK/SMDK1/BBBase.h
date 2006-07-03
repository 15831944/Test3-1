//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __BBBASE_H
#define  __BBBASE_H

#include "sc_defs.h"

#define MDLBASE
#include "models.h"
#include "..\..\..\SMDK\Include\md_headers.h"
#include "..\..\..\SMDK\Include\md_impl.h"

#ifdef __BBBASE_CPP
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

class DllImportExport CBBBase
  {
  public:
    CBBBase(MdlNode* pObj);
    virtual ~CBBBase();

    void      SetSubClass(LPTSTR MdlName);

    void      DoBuildDataDefn(DataDefnBlk & DDB);
    flag      DoDataXchg(DataChangeBlk & DCB);
    flag      DoValidateData(ValidateDataBlk & VDB);
    flag      DoPreStartCheck();

    void      DoPostConnect(long IONo);
    void      DoPreDisConnect(long IONo);
    void      DoSetDatums(CSetDatumsData * pSDD, long nSDD);
    void      DoSetSizes();

    void      DoInitialiseSolution();
    void      DoStartSolution();
    void      DoStartStep();
    
    bool      DoPropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo);

    bool      DoConfigureJoins();
    bool      DoEvalJoinPressures(long JoinMask);
    bool      DoEvalJoinFlows(long JoinNo);
    bool      DoEvalFlowEquations(eScdFlwEqnTasks Task,/*CSpPropInfo *pProps,*/ long IONo, long FE, long LnkNo);

    void      DoEvalPBMakeUpReqd(long JoinMask); 
    void      DoEvalPBMakeUpAvail(long JoinMask); 

    //void      DoEvalProductsInit(EvalProductsInitTasks Task);
    //void      DoEvalProductsSurge();
    //void      DoInitFlowInfo();
    //void      DoStepFlowInfo();
    //void      DoEvalFlowInfoStart();
    //void      DoEvalFlowInfoEnd();
    //flag      DoTestFlowInfo();
    //flag      DoEvalPressureSens();
    //void      DoCompletePressureSens();
    //void      DoApplyDerivs(double dTime, flag DoDbg);
      
    //void      DoEvalProductsInit(EvalProductsInitTasks Task);
    //void      DoEvalProductsSurge();
    void      DoEvalProducts(CNodeEvalIndex & NEI);
    //void    DoEvalDerivs(double StepSize);
    void      DoEvalIntegral(CNodeEvalIndex & NEI);
    
    void      DoEvalDiscrete();
    void      DoEvalCtrlInitialise(eScdCtrlTasks Tasks=CO_All);
    void      DoEvalCtrlActions(eScdCtrlTasks Tasks=CO_All);
    void      DoEvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All);
    void      DoEvalCtrlTerminate(eScdCtrlTasks Tasks=CO_All);
    void      DoEvalStatistics(eScdCtrlTasks Tasks=CO_All);

    void      DoEvalPowerAvailable();
    void      DoEvalPowerRequired();
    void      DoEvalState();
    
    void      DoOnAppActivate(BOOL bActive);
    int       DoFilesUsed(CFilesUsedArray & Files);

    void      DoClosureInfo(MClosureInfo & CI);

    void      DoSetState(eScdMdlStateActs RqdState);

    void      DoMacroMdlEvaluate(eScdMacroMdlEvals Eval);
    flag      DoMacroMdlValidNd(int iIONo);  
    void      DoMacroMdlAddNd(int iIONo);    
    void      DoMacroMdlRemoveNd(int iIONo); 
    CMacroMdlBase* DoMacroMdlActivate();     
    void      DoMacroMdlDeactivate();        

  protected:
    MdlNode*       m_pNd;
    MBaseMethod*   m_pMethod;
    unsigned char  bDoneInitIO:1;
    short          iInCnt;
    short          iOutCnt;

    virtual MContainer * getContainer(long i) { return NULL; }

    MBaseMethod &   getMethod() const     { return *m_pMethod; };
    MBaseMethodImpl &   getMethodImpl() const { return *(m_pMethod->m_pImpl); };

    __declspec(property(get=getMethod))     MBaseMethod&     Method;
    __declspec(property(get=getMethodImpl)) MBaseMethodImpl& MethodImpl;

  private:
    void InitGeneral();

  };

// ===========================================================================

#undef DllImportExport

#endif
