//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 // SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __2D_LNORM_H
#define  __2D_LNORM_H

#ifdef __2D_LNORM_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

const long LNormNoOfParms = 4;

DEFINE_TAGOBJ(C2DLNorm);
class DllImportExport C2DLNorm : public C2DModel
  {
  public:
    static double Const1;
    static pchar ParmDescs[LNormNoOfParms];

  public:
    C2DLNorm(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C2DLNorm();
    virtual void Clear();
    virtual double Yx(double Xi);
  	virtual pchar GetParmDesc(long i) { return ParmDescs[i]; };
  };

// ===========================================================================

#undef DllImportExport

#endif
