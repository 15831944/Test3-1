//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 // SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __2D_HARIS_H
#define  __2D_HARIS_H

#ifdef __2D_HARIS_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

const long HarisNoOfParms = 5;

DEFINE_TAGOBJ(C2DHaris);
class DllImportExport C2DHaris : public C2DModel
  {
  public:
    static pchar ParmDescs[HarisNoOfParms];

  public:
    C2DHaris(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C2DHaris();
    virtual void Clear();
    virtual double Yx(double Xi);
    virtual pchar GetParmDesc(long i) { return ParmDescs[i]; };
  };

// ===========================================================================

#undef DllImportExport

#endif
