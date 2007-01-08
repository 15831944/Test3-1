//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __AREA_H
#define  __AREA_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif
#ifndef __M_AREA_H
  #include "m_area.h"
#endif

#ifdef __AREA_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


//=========================================================================== 

DEFINE_TAGOBJ(CArea);
class DllImportExport CArea : public MN_Area
  {
  public:
    CArea (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CArea(){};
    virtual void   PrepareDefinedData();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    
  };

DEFINE_TAGOBJ(MakeupSrc);
class DllImportExport MakeupSrc : public MN_Area
  {
  public:
    MakeupSrc (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~MakeupSrc (){};
    virtual void   PrepareDefinedData();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual void   EvalDiscrete();
    DEFINE_CI(MakeupSrc, MN_Area, 2);

  protected:
    CompositionFetchHelper CF;
  };

DEFINE_TAGOBJ(BleedSnk);
class DllImportExport BleedSnk : public MN_Area
  {
  public:
    BleedSnk(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~BleedSnk(){};
    virtual void   PrepareDefinedData();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    
  };


//=========================================================================== 

#undef DllImportExport

#endif


