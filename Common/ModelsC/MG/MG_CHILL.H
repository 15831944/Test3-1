//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FU_MG_H
#define  __FU_MG_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif  
#ifndef __FLWNODE_H
  #include <flwnode.h>
#endif 
#define HEATXCH1
#ifndef __MODELS_H
  #include "models.h"
#endif  

#define DllImportExport 

DEFINE_TAGOBJ(GasChill);

class GasChill : public HeatXchg1
  {
  public:  

    GasChill(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~GasChill (){};
//            void   Initialise();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

  private:
  };

#undef DllImportExport 
#endif 
