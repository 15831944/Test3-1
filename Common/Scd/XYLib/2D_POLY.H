//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 // SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __2D_POLY_H
#define  __2D_POLY_H

#ifdef __2D_POLY_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

const long C2DPolyMaxOrder = 9;

//============================================================================

DEFINE_TAGOBJ(C2DPoly);
class DllImportExport C2DPoly : public C2DModel
  {
  public:
	static pchar ParmDescs[C2DPolyMaxOrder+1];
	long Order;   //n'th order of polynomial
  public:
    C2DPoly(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C2DPoly();
    
    virtual void   CopyModel(pC2DPoly pMd); 
    virtual void   Clear();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk &DCB);
    virtual flag InitParms(flag DoIt);
    virtual double Yx(double Xi);
    virtual flag ReFit();
  	virtual pchar GetParmDesc(long i) { return ParmDescs[i]; };
    // FxdEdtBookRef Overrides
    virtual void Build(flag bShowEqn);
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);

    void SetOrder(long nOrder);
  };

// ===========================================================================

#undef DllImportExport

#endif
