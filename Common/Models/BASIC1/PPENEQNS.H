//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __PPENEQNS_H
#define  __PPENEQNS_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __FLWEQN_H
  #include "flweqn.h"
#endif

#if defined(__PPENEQNS_CPP)
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================
//============================================================================

DEFINE_TAGOBJ(PE_Linear);
class PE_Linear : public CFlwEqn
  {
  public:
    double    R_Fxd;//, Diam;
    double    m_dDP;

    PE_Linear(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PE_Linear();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

//============================================================================
//============================================================================

DEFINE_TAGOBJ(PE_SimpleVel);
class PE_SimpleVel : public CFlwEqn
  {
  public:
    double    Diam, PwrLaw, OpDP, OpVel;
    double    m_dDP;

    PE_SimpleVel(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PE_SimpleVel();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

//============================================================================
//============================================================================

DEFINE_TAGOBJ(PE_VolFlow);
class PE_VolFlow : public CFlwEqn
  {
  public:
    double    PwrLaw, OpDP, OpVol,OpNVol;
    double    m_dDP;

    PE_VolFlow(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PE_VolFlow();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

//============================================================================
//============================================================================

DEFINE_TAGOBJ(PE_KFact);
class PE_KFact : public CFlwEqn
  {
  public:
    double         m_dDP;

    PE_KFact(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PE_KFact();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================

#undef DllImportExport

#endif
