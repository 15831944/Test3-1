//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __PPFTEQNS_H
#define  __PPFTEQNS_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif

#if defined(__PPFTEQNS_CPP)
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif
// ===========================================================================

DEFINE_TAGOBJ(PF_Linear);
class PF_Linear : public CFlwEqn
  {
  public:
    double         R_Fxd;

    PF_Linear(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PF_Linear();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

//    virtual void   StartSolution(rFlwBlkBase FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(PF_ReducerCv);
class PF_ReducerCv : public CFlwEqn
  {
  public:
    double         R_Fxd,Cv,In_Diam;

    PF_ReducerCv(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PF_ReducerCv();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

//    virtual void   StartSolution(rFlwBlkBase FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================

#undef DllImportExport

#endif
