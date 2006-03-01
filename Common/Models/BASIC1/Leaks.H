//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __LEAKS_H
#define  __LEAKS_H

#include "sc_defs.h"
#include "flweqn.h"
#include "m_xfer.h"

#if defined(__LEAKS_CPP)
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================
//============================================================================

DEFINE_TAGOBJ(LE_MassFlow);
class LE_MassFlow : public CFlwEqn
  {
  public:
    LE_MassFlow(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~LE_MassFlow();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);

  protected:
    double    m_PwrLaw, m_OpDP, m_OpQm;
    //double    m_dDP;
  };

//============================================================================
//============================================================================

DEFINE_TAGOBJ(LE_VolFlow);
class LE_VolFlow : public CFlwEqn
  {
  public:
    LE_VolFlow(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~LE_VolFlow();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);

  protected:
    double    m_PwrLaw, m_OpDP, m_OpVol,m_OpNVol;
    //double    m_dDP;
  };

//============================================================================
//============================================================================

//DEFINE_TAGOBJ(LE_KFact);
//class LE_KFact : public CFlwEqn
//  {
//  public:
//    LE_KFact(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
//    virtual ~LE_KFact();
//    virtual void   BuildDataDefn(DataDefnBlk & DDB);
//    virtual flag   DataXchg(DataChangeBlk & DCB);
//    virtual flag   ValidateData(ValidateDataBlk & VDB);
//    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
//
//  protected:
//    //double         m_dDP;
//  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(LE_EquivDiam);
class LE_EquivDiam : public CFlwEqn
  {
  public:
    LE_EquivDiam(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~LE_EquivDiam();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);

  public:
    double         m_dEquivDiam, m_dDP;
    double         m_dVelSonicMeas;
  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(CPipeLeak);
class DllImportExport CPipeLeak : public MN_Xfer
  {
  public:
    CPipeLeak (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CPipeLeak (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   PostConnect(int IONo);

  protected:
    //flag           m_bShowQFeed;
  };

// ===========================================================================
// ===========================================================================

#undef DllImportExport

#endif
