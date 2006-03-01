//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FE_HYD_H
#define  __FE_HYD_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif
#ifndef __TSPLINE_H
  #include "tspline.h"
#endif

#if defined(__FE_HYD_CPP)
  #define DllImportExport DllExport
#elif !defined(MG)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif
// ===========================================================================

DEFINE_TAGOBJ(FE_Hyd);
class FE_Hyd : public FlwEqn
  {
  public:
    byte           Hydrate;
    double         Block, Force, Resistance;
    double         Diam, Len_Rqd, Len_Fit, Res_Magn_Fac, Res_Powr_Fac,
                   Age_Res_Fac, Rough, Viscosity, LoLimVel;
    C2DFn          HCurve;
    TSpline        HSpline;

    double         dFreezeRate; // Rate at which hydrates form
    double         dMeltRate;   // Rate at which hydrates melt
    double         dChangeRate; // Rate at which hydrate changes (Melt or freeze or stay the same)

    FE_Hyd(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~FE_Hyd();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

//    virtual void   GetPhysData(FBPhysData &D, double DZ);

    //virtual double GetActualLength(double DZ);
    //virtual double GetExtraLength();
    //virtual double GetActualArea();

    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   EvaluateFlwEqn(CSpPropInfo *pProps, FlwBlkBase & FE, double Regulation, FBPhysData *pPhD0, FBPhysData *pPhD1);
    //virtual void   EvalDiscrete(rFlwBlkBase FE);
    virtual void   StepFlowInfo(FlwBlkBase & FE);
    double         dP_Vel_Fn(double Vel, double Rho, double Length);
    double         Inhibitor(double dT);
  };

// ===========================================================================

#undef DllImportExport

#endif
