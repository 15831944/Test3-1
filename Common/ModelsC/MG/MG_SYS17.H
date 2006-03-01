//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
#ifndef  __MG_SYS17_H
#define  __MG_SYS17_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif  
#ifndef __MODELS_H
  #include <models.h>
#endif 
#ifndef __FLWNODE_H
  #include <flwnode.h>
#endif 

#define DllImportExport 

DEFINE_TAGOBJ(Sys17);
class Sys17 : public MdlNode
  {
  public:
    struct 
      {
      double       Bst;
      flag         bAuto;
      long         iOperate;
      }            PK1702;
    struct 
      {
      double       P;
      }            PIA17002;

    //Sys17(pchar NameIn, pchar TagIn, short NdIdIn, pchar DataSheetIn);
    //virtual ~Sys17(){};
//    //        void   Initialise();
    //virtual void   PackedData(rPackBuffer B);
    //virtual flag   FindData(dword Page, DataInfo &DInfo);
 
    Sys17(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Sys17(){};
//            void   Initialise();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual void   EvalDerivs();
    virtual void   EvalDiscrete(); 
  private:
  };

#undef DllImportExport 

#endif
