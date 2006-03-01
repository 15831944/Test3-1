//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FE_VEL_H
#define  __FE_VEL_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __FLWEQN_H
  #include "flweqn.h"
#endif

#if defined(__FE_VEL_CPP)
  #define DllImportExport DllExport
#elif !defined(MDLLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

DEFINE_TAGOBJ(FE_Hyp);
class FE_Hyp : public FlwEqn
  {
  public:
    double         Diam, Len_Rqd, Mod;

    FE_Hyp(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~FE_Hyp();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   EvaluateFlwEqn(CSpPropInfo *pProps, FlwBlkBase & FE, double Regulation, FBPhysData *pPhD0, FBPhysData *pPhD1);
  };

// ===========================================================================


#undef DllImportExport

#endif
