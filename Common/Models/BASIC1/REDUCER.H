//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __REDUCER_H
#define  __REDUCER_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_BSTRES_H
  #include "m_bstres.h"
#endif

#ifdef __REDUCER_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================

DEFINE_TAGOBJ(Reducer)
class DllImportExport Reducer : public MN_BstRes
  {
  public:
//    CIO_VlvPosBlk   VPB;
    Reducer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Reducer (){};
    virtual void   BuildDataDefn(DataDefnBlk &DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
                   
  };

//=========================================================================== 

#undef DllImportExport

#endif


