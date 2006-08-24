//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __PUMP_H
#define  __PUMP_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif

#include <models.h>
//#ifndef __M_BSTRES_H
//  #include "m_bstres.h"
//#endif

#ifdef __PUMP_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

DEFINE_TAGOBJ(Pump_GP);
class DllImportExport Pump_GP : public MN_BstRes
  {
  public:
    Pump_GP (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Pump_GP(){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   GetModelAction(CMdlActionArray & Acts);
    virtual flag   SetModelAction(CMdlAction & Act);
    virtual void   ConfigureJoins();
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);

    virtual CFlwRegBlk * GetFlwRegulator() { return &m_FRB; };

    virtual double LnkFlowDefined()             { return !GetActiveHold() && m_PSB.On() ? PBQmEst : 0.0; };
    virtual bool   LnkFlowDefinedIn(int IONo)   { return m_PSB.On() && MdlNode::LnkFlowDefinedIn(IONo); };
    virtual bool   LnkFlowDefinedOut(int IONo)  { return m_PSB.On() && MdlNode::LnkFlowDefinedOut(IONo); };

    int            AutoWiring(AutoWireJob AWJ);
    virtual void   CollectElectrics(CNodeElectricsArray & TSCA);
    virtual void   ConnectElectrics();
    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng &Tg);

    virtual bool   IsXRefListActive();
    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    virtual void   UnlinkAllXRefs();
    virtual void   EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);
    //virtual void   EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All);
    virtual void   EvalPowerRequired();
    virtual dword  ModelStatus();

    DEFINE_CI(Pump_GP, MN_BstRes, 4);

  public:
    CIO_MtrSpdBlk  m_PSB;
    CFlwRegBlk     m_FRB;
    CPwrUser       m_Pwr;
    CTgFnIoVar     m_SpdVar;
  };

//===========================================================================

#undef DllImportExport

#endif


