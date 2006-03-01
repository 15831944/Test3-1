//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 // SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __2D_ROSIN_H
#define  __2D_ROSIN_H

#ifdef __2D_ROSIN_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

const long RosinNoOfParms = 4;

DEFINE_TAGOBJ(C2DRosin);
class DllImportExport C2DRosin : public C2DModel
  {
  public:
    static pchar ParmDescs[RosinNoOfParms];

  public:
    C2DRosin(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C2DRosin();
    virtual void Clear();
    virtual double Yx(double Xi);
    virtual pchar GetParmDesc(long i) { return ParmDescs[i]; };
  };

// ===========================================================================

#undef DllImportExport

#endif
