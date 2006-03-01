//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MG_SWF_H
#define  __MG_SWF_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __FLWNODE_H
  #include "flwnode.h"
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

DEFINE_TAGOBJ(SWF_Package);

class SWF_Package : public MN_BstRes
  {
  public:
    SWF_Package(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SWF_Package (){};
//            void   Initialise();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    //virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int iJoinId, int FE);
  };

#undef DllImportExport

#endif

