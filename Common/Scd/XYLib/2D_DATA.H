//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __2D_DATA_H
#define  __2D_DATA_H

#ifdef __2D_DATA_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

DEFINE_TAGOBJ(C2DDataOnly);
class DllImportExport C2DDataOnly : public C2DModel
  {
  DEFINE_SPARES(C2DDataOnly);
  public:
    long          CurSeg;

    C2DDataOnly(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C2DDataOnly();
    
    virtual void   CopyModel(pC2DDataOnly pMd); 
    virtual void   Clear();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual double Xy(double Yi);
    virtual double Yx(double Xi);
    virtual flag ReFit();

    // FxdEdtBookRef Overrides
    virtual void Build();
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
  };

// ===========================================================================

#undef DllImportExport

#endif
