//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __VALVE_H
#define  __VALVE_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_BSTRES_H
  #include "m_bstres.h"
#endif

#ifdef __VALVE_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================

DEFINE_TAGOBJ(Valve_GP)
class DllImportExport Valve_GP : public MN_BstRes
  {
  public:
    CIO_VlvPosBlk   m_VPB;
    CFlwRegBlk      m_FRB;
    CPwrUser        m_Pwr;
    CTgFnIoVar      m_PosVar;

    Valve_GP(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Valve_GP (){};
    virtual void   BuildDataDefn(DataDefnBlk &DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   GetModelAction(Strng & Tag, MdlActionArray & Acts);
    virtual flag   SetModelAction(Strng & Tag, MdlAction & Act);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg);
    virtual bool   IsXRefListActive();
    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    virtual void   UnlinkAllXRefs();
    virtual void   EvalCtrlActions();

    virtual CFlwRegBlk * GetFlwRegulator() { return &m_FRB; };
    virtual void         SetFlwRegulator() 
      { 
      double Posn=m_FRB.Ctrl();
      m_VPB.SetReqdPosition(Posn); 
      m_VPB.SetActualPosition(Posn); 
      };

    virtual dword  ModelStatus();

    virtual double LnkFlowDefined()             { return !GetActiveHold() && m_VPB.On() ? PBQmEst : 0.0; };
    virtual bool   LnkFlowDefinedIn(int IONo)   { return m_VPB.On() && MdlNode::LnkFlowDefinedIn(IONo); };
    virtual bool   LnkFlowDefinedOut(int IONo)  { return m_VPB.On() && MdlNode::LnkFlowDefinedOut(IONo); };
  };

//===========================================================================

DEFINE_TAGOBJ(Valve_NR)
class DllImportExport Valve_NR : public MN_BstRes
  {
  public:
    byte           fForceOpen;

    Valve_NR(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Valve_NR (){};
    virtual void   BuildDataDefn(DataDefnBlk &DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   GetModelAction(Strng & Tag, MdlActionArray & Acts);
    virtual flag   SetModelAction(Strng & Tag, MdlAction & Act);
    virtual void   ConfigureJoins();
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual dword  ModelStatus();

  };

//===========================================================================

#undef DllImportExport

#endif


