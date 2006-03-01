//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MG_CYCLONE_H
#define  __MG_CYCLONE_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif
#ifndef __FLWNODE_H
  #include <flwnode.h>
#endif
#ifndef __MODELS_H
  #include "models.h"
#endif
//#ifndef __M_XFER_H
//  #include "m_xfer.h"
//#endif
#include "sp_cont.h"

#ifdef __MG_CYCLONE_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// --------------------------------------------------------------------------

DEFINE_TAGOBJ(HCyclone);
class HCyclone : public MN_Xfer
  {
  public:
    CDVector         Cd, Wt;
    double           SepEff;

    HCyclone(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~HCyclone(){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual void   ConfigureJoins();
    virtual void   EvalJoinPressures();
    virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE);
    virtual void   EvalProducts();

  private:
  };

// --------------------------------------------------------------------------

#undef DllImportExport

#endif


