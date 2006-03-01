//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SKOSPMDL_H
#define  __SKOSPMDL_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif  
#ifndef __SP_MODEL_H
  #include <sp_model.h>
#endif  

#define MDLBASE
#define BASIC1
#include "models.h"

#ifdef __SKOSPMDL_CPP
  #define DllImportExport DllExport
#elif !defined(SKO)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//                          SKO Specie Model
//
// ===========================================================================

DEFINE_SPMODEL(CSKOSpMdl)

// ===========================================================================

class DllImportExport CSKOSpMdl : public SpModel
  {
  DEFINE_SPARES(CSKOSpMdl)

  public:

    CSKOSpMdl(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CSKOSpMdl();

    virtual bool   IsBaseClassOf(pSpModel pOther) 
      { return (dynamic_cast<CSKOSpMdl*>(pOther)) != NULL; };

    virtual void   BuildDataDefn_Vars(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

  public:
    // ConditionBlk Override
    DEFINE_CI(CSKOSpMdl, SpModel, 6);
  };

// ==========================================================================
//
//                                  End
//
// ==========================================================================

#undef DllImportExport

#endif
