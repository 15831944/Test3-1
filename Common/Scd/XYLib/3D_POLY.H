//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __3D_POLY_H
#define  __3D_POLY_H

#ifdef __3D_POLY_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

const long C3DPolyNoOfParms = 9;

DEFINE_TAGOBJ(C3DPoly);
class DllImportExport C3DPoly : public C3DModel
  {
  DEFINE_SPARES(C3DPoly);
  public:
    static pchar ParmDescs[C3DPolyNoOfParms];

  public:
    C3DPoly(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C3DPoly();
    virtual void   Clear();
    virtual double Zxy(double Xi, double Yi);
    virtual double Yxz(double Xi, double Zi);
    virtual flag   ReFit();
  	virtual pchar GetParmDesc(long i) { return ParmDescs[i]; };
  };

// ===========================================================================

#undef DllImportExport

#endif
