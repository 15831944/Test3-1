//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 // SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __2D_SPLIN_H
#define  __2D_SPLIN_H

#ifdef __2D_SPLIN_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

DEFINE_TAGOBJ(C2DSpline);
class DllImportExport C2DSpline : public C2DModel
  {
  public:
    static const double SlopeFree;
    static const double SlopeOfSegment;

    long             CurSeg;
    double           Sigma, SlpLo, SlpHi, SlpN, Slp1;
    CDVector Yp;
  public:
    C2DSpline(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C2DSpline();
    
    virtual void   CopyModel(pC2DSpline pMd); 
    virtual void   Clear();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

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
