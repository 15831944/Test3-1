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

#define HEATXCH1
#ifndef __MODELS_H
  #include "models.h"
#endif 

#define DllImportExport 

DEFINE_TAGOBJ(WHRUP_Package);

class WHRUP_Package : public HeatXchg
  {
  public:  
    double         DamperPos,Eff;

    WHRUP_Package(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~WHRUP_Package (){};
//            void   Initialise();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual void   EvalDerivs();
  private:
  };

#undef DllImportExport 
#endif 


/* old defs
_FWDDEF(WHRUPackage);
class WHRUPackage : public HeatXchg
  {
  public:
    double         DamperPos;
    WHRUPackage(pchar NameIn, pchar TagIn, short NdIdIn, pchar DataSheetIn);
    virtual ~WHRUPackage(){};
            void   Initialise();
    virtual void   PackedData(rPackBuffer B);
    virtual flag   FindData(dword Page, DataInfo &DInfo);
    virtual void   EvalDerivs();
  private:
  };
*/


