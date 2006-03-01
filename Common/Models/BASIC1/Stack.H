//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __STACK_H
#define  __STACK_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif

#include <models.h>

#ifdef __STACK_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

DEFINE_TAGOBJ(Stack);
/*#C:
Chimney Stack
*/
class DllImportExport Stack : public MN_SrcSnk
  {
  public:
    double    AmbientT;
    double    MeanT;
    double    StackHgt;
    double    StackDiam;
    double    StackDraught;
    double    FlowLoss;
    double    FricFact;
    double    TDecayTau;

    flag      fOn;
    double    dDampFact;

    Stack(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Stack(){};
    virtual void   PrepareDefinedData();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void   EvalDiscrete();
  };

//===========================================================================

#undef DllImportExport

#endif


