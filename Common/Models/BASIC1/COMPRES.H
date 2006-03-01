//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __COMPRES_H
#define  __COMPRES_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_BSTRES_H
  #include "m_bstres.h"
#endif

#ifdef __COMPRES_CPP
  #define DllImportExport DllExport
#elif !defined(Compres1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

DEFINE_TAGOBJ(GasPump_GP);
class DllImportExport GasPump_GP : public MN_BstRes
  {
  public:
    GasPump_GP (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~GasPump_GP(){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   GetModelAction(Strng & Tag, MdlActionArray & Acts);
    virtual flag   SetModelAction(Strng & Tag, MdlAction & Act);
    virtual void   ConfigureJoins();
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);

    virtual CFlwRegBlk * GetFlwRegulator()  { return &m_FRB; };
    virtual void   SetFlwRegulator()        { }

    virtual double LnkFlowDefined()             { return !GetActiveHold() && m_PSB.On() ? PBQmEst : 0.0; };
    virtual bool   LnkFlowDefinedIn(int IONo)   { return m_PSB.On() && MdlNode::LnkFlowDefinedIn(IONo); };
    virtual bool   LnkFlowDefinedOut(int IONo)  { return m_PSB.On() && MdlNode::LnkFlowDefinedOut(IONo); };

    int            AutoWiring(AutoWireJob AWJ);
    virtual void   CollectElectrics(CNodeElectricsArray & TSCA);
    virtual void   ConnectElectrics();
    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng &Tg);

    //virtual void   EvalProducts(long JoinMask);
    
    virtual bool   IsXRefListActive();
    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    virtual void   UnlinkAllXRefs();
    virtual void   EvalCtrlActions();
    virtual void   EvalPowerRequired();
    virtual dword  ModelStatus();

  public:
    CIO_MtrSpdBlk  m_PSB;
    CFlwRegBlk     m_FRB;
    CPwrUser       m_Pwr;
    CTgFnIoVar     m_SpdVar;
  };

//===========================================================================

#undef DllImportExport

#endif


