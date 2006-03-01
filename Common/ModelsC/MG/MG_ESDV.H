//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MG_ESDV_H
#define  __MG_ESDV_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif  
#ifndef __FLWNODE_H
  #include <flwnode.h>
#endif 
#define BASIC1
#ifndef __MODELS_H
  #include "models.h"
#endif  
//#ifndef __VALVE_H
//  #include "valve.h"
//#endif  

#ifdef __MG_ESDV_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport 
#endif


DEFINE_TAGOBJ(ESDValve);

class ESDValve : public Valve_GP
  {
  public:  

    ESDValve(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~ESDValve (){};
//            void   Initialise();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

  private:
  };

#endif 
