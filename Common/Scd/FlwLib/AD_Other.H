//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __AD_OTHER_H
#define  __AD_OTHER_H

#define WITHGFB
#ifndef __FLWSOLVE_H
  #include "flwsolve.h"
#endif

#include <typeinfo.h>

#if defined(__AD_OTHER_CPP) /*|| defined(__FLWNETS_CPP)*/
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
//
//
//
// ==========================================================================

//class ADGroup
//  {
//  public:
//    TagObjClass *pClass;
//    Strng       Class;
//    Strng       Name;
//   
//    CArray <CADObject*, CADObject*> Obj;
//  };
//
//// =========================================================================
//
//DEFINE_TAGOBJ(CADObject);
//
////============================================================================
//
//class DllImportExport CADObject : public TaggedObject
//  {
//  DEFINE_MEMSTATS(CADObject)
//
//  public:
//    CSArray        m_SelectedTags;
//    CFlwSolver     *pFS;
//  public:
//    CADObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
//    virtual ~CADObject();
//
//    virtual void   BuildDataDefn(DataDefnBlk & DDB) {};
//
//    virtual int    SetSelectedTags(Strng_List &SelectedTags);
//    virtual int    GetStatus(Strng & Status);
//    virtual flag   ValidModel() { return true; };
//  };
//
//typedef CSCDList <pADObject, pADObject> ADObjectList;
//typedef CSCDPtrListIter<ADObjectList, pADObject> ADObjectIter;
//typedef CSCDPtrListIterWithPos<ADObjectList, pADObject> ADObjectIterPos;
//
////===========================================================================
//
//#define ADObjectGrp "CADObject"
//#define IMPLEMENT_ADOBJECT(ClassName, Name, Version, DefTag, Cat, ShortDesc, Desc) \
//  IMPLEMENT_TAGOBJ(ClassName, ADObjectGrp, Name, Version, DefTag, Cat, ShortDesc, Desc); 
////  NodeDrawEntry DrwDef_##ClassName(Name, Version, Draw);
//
//// ==========================================================================
////
////
////
//// ==========================================================================
//

//===========================================================================
//
//
//
//===========================================================================
//






// To Move






//
DEFINE_TAGOBJ(CADThickener) 

class DllImportExport CADThickener: public CADObject
  {
  DEFINE_MEMSTATS(CADThickener)

  public:
    CADThickener(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CADThickener();

    virtual void   BuildDataDefn(DataDefnBlk & DDB) {};

    virtual flag   ValidModel() { return true; };
  };

//===========================================================================
//
//
//
//===========================================================================

DEFINE_TAGOBJ(CADSysCurve) 

class DllImportExport CADSysCurve: public CADObject
  {
  DEFINE_MEMSTATS(CADSysCurve)

  public:
    CADSysCurve(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CADSysCurve();

    virtual void   BuildDataDefn(DataDefnBlk & DDB) {};

    virtual flag   ValidModel() { return true; };
  };

//===========================================================================
//
//
//
//===========================================================================

DEFINE_TAGOBJ(CADNetFlash) 

class DllImportExport CADNetFlash: public CADObject
  {
  DEFINE_MEMSTATS(CADNetFlash)

  public:
    CADNetFlash(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CADNetFlash();

    virtual void   BuildDataDefn(DataDefnBlk & DDB) {};

    virtual flag   ValidModel() { return true; };
  };

//===========================================================================
//
//
//
//===========================================================================

DEFINE_TAGOBJ(CADPipeVelocity) 

class DllImportExport CADPipeVelocity: public CADObject
  {
  DEFINE_MEMSTATS(CADPipeVelocity)

  public:
    CADPipeVelocity(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CADPipeVelocity();

    virtual void   BuildDataDefn(DataDefnBlk & DDB) {};

    virtual flag   ValidModel() { return true; };
  };

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif


