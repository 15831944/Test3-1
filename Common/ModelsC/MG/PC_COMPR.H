//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __PC_COMPR_H
#define  __PC_COMPR_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif
#ifndef __MODELS_H
  #include "models.h"
#endif

#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif


#define DllImportExport


// ===========================================================================

DEFINE_TAGOBJ(PC_Compr);
class PC_Compr : public FlwEqn
  {
  public:
    double         P_SetpointPressure;   // Setpoint Pressure set by the user
    double         P_RecircRatio;     // The Mass flow required in a feedback circuit to prevent surge
    C2DFn          P_SurgeCondition;     // Inlet Pressure Versus volume flow function

    PC_Compr(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PC_Compr();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    //virtual void   StartSolution(rFlwEqnBlk FE);
    virtual flag   EvaluateFlwEqn(CSpPropInfo *pProps, FlwBlkBase & FE, double Regulation, FBPhysData *pPhD0, FBPhysData *pPhD1);
  };

// ===========================================================================

#undef DllImportExport

#endif
