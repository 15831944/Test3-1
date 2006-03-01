//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 // SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __2D_GTGAU_H
#define  __2D_GTGAU_H

#ifdef __2D_GTGAU_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

const long GtGauNoOfParms = 4;

DEFINE_TAGOBJ(C2DGtGau);
class DllImportExport C2DGtGau : public C2DModel
  {
  public:
    static pchar ParmDescs[GtGauNoOfParms];
  public:
    C2DGtGau(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C2DGtGau();
    virtual void Clear();
    virtual double Yx(double Xi);
  	virtual pchar GetParmDesc(long i) { return ParmDescs[i]; };
  };

// ===========================================================================

#undef DllImportExport

#endif
