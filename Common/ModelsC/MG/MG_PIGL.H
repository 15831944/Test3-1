//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FU_MG_H
#define  __FU_MG_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif 

#ifndef __MODELS_H
  #include "models.h"
#endif  
//#ifndef __M_XFER_H
//  #include "m_xfer.h"
//#endif 

#define DllImportExport 

DEFINE_TAGOBJ(PigLaunch);

class PigLaunch: public MN_Xfer
  {
  public:  

    PigLaunch(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~PigLaunch (){};
//            void   Initialise();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

  private:
  };

#endif 

#undef DllImportExport 