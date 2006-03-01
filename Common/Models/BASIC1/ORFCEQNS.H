//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __ORFCEQNS_H
#define  __ORFCEQNS_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif

#if defined(__ORFCEQNS_CPP)
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif
//============================================================================
// ===========================================================================

//DEFINE_TAGOBJ(OE_Linear);
//class OE_Linear : public CFlwEqn
//  {
//  public:
//    double         m_RFxd;
//
//    OE_Linear(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
//
//    virtual ~OE_Linear();
//    virtual void   BuildDataDefn(DataDefnBlk & DDB);
//
//    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
//  };

// ===========================================================================

DEFINE_TAGOBJ(OE_QvOp);
class OE_QvOp : public CFlwEqn
  {
  public:
    double         m_dQvOp;
    double         m_dDPOp;
    double         m_dPwrLaw;
    double         m_dDP;

    OE_QvOp(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~OE_QvOp();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(OE_DiamRatio);
class OE_DiamRatio : public CFlwEqn
  {
  public:
    double         m_dDiamOrifice, m_dDiamPipe, m_dDP;

    OE_DiamRatio(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~OE_DiamRatio();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================

DEFINE_TAGOBJ(OE_InlineFilter);
class OE_InlineFilter : public CFlwEqn
  {
  public:
    double          m_dQvOp;
    double          m_dDPOp;
    double          m_dPwrLaw;
    double          m_dDP;

    byte            m_iGrowthMode;
    flag            m_bClean;
    double          m_dGrowthRate;
    double          m_dGrowthVRate;
    double          m_dGrowthMRate;
 
    double          m_dCurrentDPOp;


    OE_InlineFilter(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~OE_InlineFilter();
    virtual void    BuildDataDefn(DataDefnBlk & DDB);
    virtual flag    DataXchg(DataChangeBlk & DCB);
    virtual flag    ValidateData(ValidateDataBlk & VDB);

    virtual void    InitFlowInfo(CFlwBlkBase & FE);

    virtual flag    EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void    EvalDiscrete(CFlwBlkBase & FE);
  };

// ===========================================================================
// ===========================================================================

#undef DllImportExport

#endif
