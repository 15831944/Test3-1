//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __3D_FN_H
#define  __3D_FN_H

#ifndef __FIXEDIT_H
#include "fixedit.h"
#endif  
#ifndef __TAGOBJ_H
#include "tagobj.h"
#endif  
#ifndef __VECTORS_H
#include "vectors.h"
#endif
#ifndef __TRINET_H
#include "trinet.h"
#endif
#ifndef __BASEMDL_H
#include "basemdl.h"
#endif

#ifdef __3D_FN_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

#define IMPLEMENT_3D_MODEL(Md, Name, Version, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJ(Md, C3DModel::GroupName, Name, Version, "3DFn", "", Cat, ShortDesc, Desc);

_FWDDEF(C3DModel)
_FWDDEF(C3DFn);
DEFINE_TAGOBJEDT(C3DFn);

const long Max3DFnPts      = 1024;
const long MaxContourIntervals = 25;

//----------------------------------------------------------------------------

class DllImportExport C3DModel : public CBaseMdl
  {
  public:
    pC3DFn        pFn;   //pointer to XY function that created and uses this model
    static const pchar GroupName; //tagged object group name

    C3DModel(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C3DModel();
    static pC3DModel Create(pC3DFn pFn_, pchar Name_);
    virtual void   CopyModel(pC3DModel pMd) { CBaseMdl::CopyModel(pMd); };
    virtual void   Clear() { CBaseMdl::Clear(); };
    virtual void   BuildDataDefn(DataDefnBlk & DDB) { CBaseMdl::BuildDataDefn(DDB); };
    virtual double Xyz(double Yi, double Zi) { ASSERT(0); return 0.0; }; //method not overwritten by derived model
    virtual double Yxz(double Xi, double Zi) = 0;//{ ASSERT(0); return 0.0; }; //method not overwritten by derived model
    virtual double Zxy(double Xi, double Yi) = 0;
    virtual flag ReFit() = 0;
    
    //virtual flag InValidRgn() { return True; }; //= 0;
    //virtual void GetBoundary(CDMatrix &B) { B.SetSize(0); }; //= 0;
    virtual void Build() { BuildParmsBlk(True); };
    virtual void Load(FxdEdtInfo &EI, Strng & Str) { LoadParmsBlk(EI, Str); };
    virtual long Parse(FxdEdtInfo &EI, Strng & Str) { flag b; return ParseParmsBlk(EI, Str, b); };
    inline long GetNIV() { return 2L; };
    inline rC3DFn Fn() { return *pFn; };
    inline long NPts();
  };

//----------------------------------------------------------------------------

class DllImportExport C3DFn : public FxdEdtBookRef, public TaggedObject
  {
  friend class C3DModel;
  friend class C3DFnEdt;
  public:
    TriNet        Trinet;     //triangular network
  protected:
    pC3DModel     pM;         //pointer to Model used
    CDVector      X;          //vector of X values (independent variable)
    CDVector      Y;          //vector of Y values (independent variable)
    CDVector      Z;          //vector of Z values
    double        dXDispMax;  //max X value for display
    double        dXDispMin;  //min X value for display
    double        dYDispMax;  //max Y value for display
    double        dYDispMin;  //min Y value for display
    double        dZDispMax;  //max Y value for display
    double        dZDispMin;  //min Y value for display
    double        dXGRatio;   //scale type used for x axis
    double        dYGRatio;   //scale type used for y axis
    double        dZGRatio;   //scale type used for y axis
    double        dXUpLim;    //max valid X value (boundry)
    double        dXLoLim;    //min valid X value (boundry)
    double        dYUpLim;    //max valid Y value (boundry)
    double        dYLoLim;    //min valid Y value (boundry)
    double        dZUpLim;    //max valid Z value (boundry)
    double        dZLoLim;    //min valid Z value (boundry)
    long          iRqdLength; //required number of points when points are added or removed
    long          iNoElimIter;//number of iterations to eliminate points
    flag          bShowData;  //must the x & y points be shown
	  flag          bRedist;    //must points be redistributed after each math operation
    Strng         PtsFName;   //filename of file from which points are loaded
    Strng         sXName;     //name of x axis
    Strng         sYName;     //name of y axis
    Strng         sZName;     //name of z axis
    flag          bIsBuilt;   //has the trinet been built
    CDMatrix      CrsSection; //limited to 10 points
    long          iNoCrsPts;  //number of points in the cross section
    long          iXDivs;     //number of x divisions
    long          iYDivs;     //number of y divisions
    long          iContourIntervals; //number of contour divisions in full range

  public:
    C3DFn(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    C3DFn(pchar pModelId, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    C3DFn(pchar pModelId, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach, pchar pXName, pchar pYName, pchar pZName);
    C3DFn(C3DFn& Fn);
    virtual ~C3DFn();
    void Clear();
    void CopyFunction(C3DFn& Fn);
 
    //static void Get3DModelDesc(pTagObjClass pC, rStrng S);
    void SetLength(long n);
    pchar ModelName(pchar pReqdModelDesc = NULL);
    flag ChangeModel(pchar pReqdModelDesc);
    flag ReFit();
    flag LoadPts(char* FName, CCnvIndex XCnvDC=0, CCnvIndex YCnvDC=0, CCnvIndex ZCnvDC=0);
    flag SavePts(char* FName, char * XLabel="X", char * YLabel="Y", char * ZLabel="X");
    flag DoRedistribute();
    flag EliminateEdgePts(long lNoPtsRqd,CLMatrix &Edge);
    flag EliminatePts(long lNoPtsRqd,long NoIterations);
    flag CreatePts(long lNoPtsRqd);
    
    //math and manipulation methods...
    double GetNormX(double x);
    void GetNormX(CDVector &Xs);
    double GetActualX(double x);
    void GetActualX(CDVector &Xs);
    double GetNormY(double y);
    void GetNormY(CDVector &Ys);
    double GetActualY(double y);
    void GetActualY(CDVector &Ys);
    double GetNormZ(double z);
    void GetNormZ(CDVector &Zs);
    double GetActualZ(double z);
    void GetActualZ(CDVector &Zs);
    void SortByX(flag Ascending = True);
    void SortByY(flag Ascending = True);
    void CombineSurfaces(C3DFn& Fn);
    flag Add(double Offset);
    flag Add(C3DFn& Fn);
    flag Multiply(double Scalar);
    flag Multiply(C3DFn& Fn);

    virtual void BuildDataDefn(DataDefnBlk &DDB);
    virtual flag DataXchg(DataChangeBlk &DCB);
    // FxdEdtBookRef Overrides
    virtual void   Build();
    virtual void   Load(FxdEdtInfo &EI, Strng & Str);
    virtual long   Parse(FxdEdtInfo &EI, Strng & Str);
    virtual long   ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    
    inline double Xyz(double Yi,double Zi) { return pM->Xyz(Yi, Zi); };
    inline double Yxz(double Xi,double Zi) { return pM->Yxz(Xi, Zi); };
    inline double Zxy(double Xi,double Yi) { return pM->Zxy(Xi, Yi); };
    inline long Length(long* RqdLen = NULL) { if (RqdLen) SetLength(*RqdLen); return Z.GetLen(); };
    inline long GetLen() { return Z.GetLen(); };
    inline long GetNIV() { return 2L; };
    inline void SetIsBuilt(flag IsBuilt) { bIsBuilt = IsBuilt; };
    inline flag IsBuilt() { return bIsBuilt; };
    inline long GetNoCrsPts() { return iNoCrsPts; };
    inline double GetCrsSection(long i,long j) { return ((CrsSection.GetRows()==0) ? 0 : CrsSection(i,j)); };
    inline double XDispMax() { return dXDispMax; };
    inline double XDispMin() { return dXDispMin; };
    inline double YDispMax() { return dYDispMax; };
    inline double YDispMin() { return dYDispMin; };
    inline double ZDispMax() { return dZDispMax; };
    inline double ZDispMin() { return dZDispMin; };
    inline void SetXDispMax(double x) { dXDispMax = x; };
    inline void SetXDispMin(double x) { dXDispMin = x; };
    inline void SetYDispMax(double y) { dYDispMax = y; };
    inline void SetYDispMin(double y) { dYDispMin = y; };
    inline void SetZDispMax(double z) { dZDispMax = z; };
    inline void SetZDispMin(double z) { dZDispMin = z; };
    inline double XUpLimit() { return dXUpLim; };
    inline double XLoLimit() { return dXLoLim; };
    inline double YUpLimit() { return dYUpLim; };
    inline double YLoLimit() { return dYLoLim; };
    inline double ZUpLimit() { return dYUpLim; };
    inline double ZLoLimit() { return dYLoLim; };
    inline double Xp(long i) { return X[i]; };
    inline double Yp(long i) { return Y[i]; };
    inline double Zp(long i) { return Y[i]; };
    inline double ScanXMin() { return X.ScanMin(); };
    inline double ScanXMax() { return X.ScanMax(); };
    inline double ScanYMin() { return Y.ScanMin(); };
    inline double ScanYMax() { return Y.ScanMax(); };
    inline double ScanZMin() { return Z.ScanMin(); };
    inline double ScanZMax() { return Z.ScanMax(); };
    inline rCDVector Xs() { return X; };
    inline rCDVector Ys() { return Y; };
    inline rCDVector Zs() { return Y; };
    inline void SetPt(long i, double x, double y, double z) { X[i] = x; Y[i] = y; Z[i] = z; };
    inline flag InRegion(double x, double y) { return (x>=dXLoLim && x<=dXUpLim && y>=dYLoLim && y<=dYUpLim); };
    inline long XDivs() { return iXDivs; };
    inline long YDivs() { return iYDivs; };
    inline long ContourIntervals() { return Max(1L, iContourIntervals); };
    inline long NoCrsPts() { return iNoCrsPts; };
    inline void SetAxisNames(pchar pXName, pchar pYName, pchar pZName) { sXName = pXName; sYName = pYName; sZName = pZName; };
    inline pchar XName() { return (sXName.Length()>0 ? sXName() : ""); };
    inline pchar YName() { return (sYName.Length()>0 ? sYName() : ""); };
    inline pchar ZName() { return (sZName.Length()>0 ? sZName() : ""); };
    inline int XNameLen() { return sXName.Length(); };
    inline int YNameLen() { return sYName.Length(); };
    inline int ZNameLen() { return sZName.Length(); };
    };

// ===========================================================================

inline long C3DModel::NPts() { return pFn->Z.GetLen(); };

// ===========================================================================

#undef DllImportExport

#endif

