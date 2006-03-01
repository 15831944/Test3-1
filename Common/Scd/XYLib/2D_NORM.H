//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 // SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __2D_NORM_H
#define  __2D_NORM_H

#ifdef __2D_NORM_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

const long NormNoOfParms = 4;

DEFINE_TAGOBJ(C2DNorm);
class DllImportExport C2DNorm : public C2DModel
  {
  public:
    static double Const1;
    static pchar ParmDescs[NormNoOfParms];

  public:
    C2DNorm(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C2DNorm();
    virtual void Clear();
    virtual double Yx(double Xi);
    virtual flag InitParms(flag DoIt);
    virtual pchar GetParmDesc(long i) { return ParmDescs[i]; };
  };

// ===========================================================================

#undef DllImportExport

#endif
