//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __RTSPMDL_H
#define  __RTSPMDL_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif  
#ifndef __SP_MODEL_H
  #include <sp_model.h>
#endif  

#define MDLBASE
#define BASIC1
#include "models.h"

#ifdef __RTSPMDL_CPP
  #define DllImportExport DllExport
#elif !defined(RTTS)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//                          Rio Tinto Specie Model
//
// ===========================================================================

DEFINE_SPMODEL(CRtSpMdl)

// ===========================================================================

class DllImportExport CRtSpMdl : public SpModel
  {
  DEFINE_SPARES(CRtSpMdl)

  public:

    CRtSpMdl(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CRtSpMdl();

    virtual bool   IsBaseClassOf(pSpModel pOther) 
      { return (dynamic_cast<CRtSpMdl*>(pOther)) != NULL; };

    virtual void   BuildDataDefn_Vars(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

  public:
    // ConditionBlk Override
    DEFINE_CI(CRtSpMdl, SpModel, 6);
  };

// ==========================================================================
//
//                                  End
//
// ==========================================================================

#undef DllImportExport

#endif
