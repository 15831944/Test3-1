//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SP_SHAPE_H
#define  __SP_SHAPE_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __SP_DB_H
  #include "sp_db.h"
#endif
#ifndef __ODESOLVE_H
  #include "odesolve.h"
#endif
#ifndef __TAGOBJ_H
  #include "tagobj.h"
#endif
#ifndef __VECTORS_H
  #include "vectors.h"
#endif
#ifndef __SCDARRAY_H
  #include "scdarray.h"
#endif
#ifdef __SP_SHAPE_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

#define SpShapeDefTag "Shape"

const byte VVT_FixedV = 0;
const byte VVT_FunctP = 1;
const byte VVT_FixedP = 2;

// ===========================================================================

_FWDDEF(SpShape);             // Shape of Container

// ===========================================================================

#define DEFINE_SPSHAPE(SS) DEFINE_TAGOBJ(SS)

#define IMPLEMENT_SPSHAPE(Obj, ModelId, Version, Cat, SDesc, LDesc) \
        IMPLEMENT_TAGOBJ(Obj, SpShape::GroupName, ModelId, Version, "", "", Cat, SDesc, LDesc)

// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_SPSHAPE(SpShape)
class DllImportExport SpShape : public TaggedObject
  {
  DEFINE_SPARES(SpShape)
  public:
    static const pchar GroupName;

    double         m_dVolume;
    double         m_dHeight;
    double         m_dVapVolScl;
    double         m_dLowestIOPt;
    flag           m_bKeepLPTOnVChange;

    SpShape(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SpShape();

    double         CurrentP();
    double         CurrentT();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);

    virtual void   CopyOnChange(SpShape * pShape);
    virtual void   SetActualVolume(double V_)             { m_dVolume=V_; };;
    virtual void   SetHeight(double H_)                   { m_dHeight=H_; };;
    virtual void   SetVapVolScale(double s_);   
    virtual void   SetApparentVolume(double V_);

    virtual double VapVolScale()                          { return m_dVapVolScl; };
    virtual double ApparentVolume()                       { return ActualVolume()/m_dVapVolScl; };

    virtual byte   VolumeType()                           { return VVT_FixedV; };
    virtual double ActualVolume(double T, double P)       { return ActualVolume(); };
    virtual double ActualVolume();
    virtual double DatumPt();
    virtual double Level(double SLVolume);
    virtual double Height();
    virtual double RLevel(double SLVolume);

    virtual void   SetLowestIOPt(double L) { m_dLowestIOPt=Range(0.0, L, 0.1); }
    virtual double LowestIOPt() { return m_dLowestIOPt; }

  };

// ===========================================================================
//
//                              Pipe Shape
//
// ===========================================================================

DEFINE_SPSHAPE(SpShPipe)
class DllImportExport SpShPipe : public SpShape
  {
  DEFINE_SPARES(SpShPipe)
  public:

    SpShPipe(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SpShPipe();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
  };

// ===========================================================================
//
//                            Plug Flow Shape
//
// ===========================================================================

DEFINE_SPSHAPE(SpShPlugFlw)
class DllImportExport SpShPlugFlw : public SpShape
  {
  DEFINE_SPARES(SpShPlugFlw)
  public:

    SpShPlugFlw(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SpShPlugFlw();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
  };

// ===========================================================================
//
//
//
// ===========================================================================


#undef DllImportExport
#endif