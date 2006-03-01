//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 // SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __2D_PART_H
#define  __2D_PART_H

#ifdef __2D_PART_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


//============================================================================

const long PartNoOfParms = 3;

DEFINE_TAGOBJ(C2DPart);
class DllImportExport C2DPart : public C2DModel
  {
  public:
    static pchar ParmDescs[PartNoOfParms];
    flag           bInvert;

  public:
    C2DPart(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C2DPart();
    
    virtual void   CopyModel(pC2DPart pMd);
    virtual void   Clear();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual double Xy(double Yi);
    virtual double Yx(double Xi);

    virtual flag   ReFit();

    virtual pchar GetParmDesc(long i) { return ParmDescs[i]; };
    // FxdEdtBookRef Overrides
    virtual void   Build(flag bShowEqn);
    virtual void   Load(FxdEdtInfo &EI, Strng & Str);
    virtual long   Parse(FxdEdtInfo &EI, Strng & Str);
    virtual long   ButtonPushed(FxdEdtInfo &EI, Strng & Str);
  };

// ===========================================================================

#undef DllImportExport

#endif
