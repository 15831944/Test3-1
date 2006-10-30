//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __VALVEQNS_H
#define  __VALVEQNS_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif

#if defined(__VALVEQNS_CPP)
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif
//============================================================================
//
//
//
// ===========================================================================

DEFINE_TAGOBJ(VE_Linear);
class VE_Linear : public CFlwEqn
  {
  enum ValveCharacteristics {VC_Linear, VC_EqPerc, VC_Quick};
  public:
    double         R_Fxd;
    byte           Character;

    VE_Linear(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~VE_Linear();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

//    virtual void   StartSolution(rFlwBlkBase FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(VE_CvKm);
class VE_CvKm : public CFlwEqn
  {
  enum ValveCharacteristics {VC_Linear, VC_EqPerc, VC_Quick};
  public:
    flag           fUSCvForm;
    double         Cv, Km, Area;
    byte           Character;

    VE_CvKm(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~VE_CvKm();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

//    virtual void   StartSolution(rFlwBlkBase FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(VE_Steam);
class VE_Steam : public CFlwEqn
  {
  enum ValveCharacteristics {VC_Linear, VC_EqPerc, VC_Quick};
  public:
    double         Cs, C1;
    byte           Character;

    VE_Steam(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~VE_Steam();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

//    virtual void   StartSolution(rFlwBlkBase FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(VE_VarCv);
class VE_VarCv : public CFlwEqn
  {
  enum ValveCharacteristics {VC_Linear, VC_EqPerc, VC_Quick};
  public:
    double         Cv, Km, Area;
    flag           fUSCvForm;
    C2DFn          CvCurve;

    VE_VarCv(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~VE_VarCv();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

//    virtual void   StartSolution(rFlwBlkBase FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================

#undef DllImportExport

#endif
