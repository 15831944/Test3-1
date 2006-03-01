//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MG_STRAIN_H
#define  __MG_STRAIN_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif
#ifndef __FLWNODE_H
  #include <flwnode.h>
#endif
#ifndef __MODELS_H
  #include "models.h"
#endif
//#ifndef __M_BSTRES_H
//  #include "m_bstres.h"
//#endif
//#ifndef __VALVE_GP_H
//  #include "valve_gp.h"
//#endif

#define DllImportExport

DEFINE_TAGOBJ(Strainer);

class Strainer : public MN_BstRes
  {
  public:
    double R_Fxd;
    Strainer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Strainer (){};
//            void   Initialise();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE);
  };

#undef DllImportExport

#endif

