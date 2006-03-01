//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __2D_FN_H
#define  __2D_FN_H

#ifndef __DATACNVS_H
#include "datacnvs.h"
#endif  
#ifndef __FIXEDIT_H
#include "fixedit.h"
#endif  
#ifndef __TAGOBJ_H
#include "tagobj.h"
#endif  
#ifndef __VECTORS_H
#include "vectors.h"
#endif
#ifndef __BASEMDL_H
#include "basemdl.h"
#endif

#ifdef __2D_FN_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

#define IMPLEMENT_2D_MODEL(Md, Name, Version, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJ(Md, C2DModel::GroupName, Name, Version, "2DFn", "", Cat, ShortDesc, Desc);

_FWDDEF(C2DModel)
_FWDDEF(C2DFn);
DEFINE_TAGOBJEDT(C2DFn);

const long Max2DFnPts   = 1024;
const double MinLogVal  = 1.0e-10;

//----------------------------------------------------------------------------

class DllImportExport C2DModel : public CBaseMdl//: public FxdEdtBookRef, public TaggedObject
  {
  public:
    pC2DFn        pFn;   //pointer to XY function that created and uses this model
    static const pchar GroupName; //tagged object group name

    C2DModel(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C2DModel();
    static pC2DModel Create(pC2DFn pFn_, pchar Name_);
    virtual void   CopyModel(pC2DModel pMd) { CBaseMdl::CopyModel(pMd); };
    virtual void   Clear() { CBaseMdl::Clear(); };
    virtual void   BuildDataDefn(DataDefnBlk & DDB) { CBaseMdl::BuildDataDefn(DDB); };
    virtual double Xy(double Yi) { ASSERT(0); return 0.0; }; //method not overwritten by derived model
    virtual double Yx(double Xi) = 0;
    virtual double Yx(CDVector &Xs) { return Yx(Xs[0]); };
    virtual flag ReFit();
    
    //virtual flag InValidRgn() { return True; }; //= 0;
    //virtual void GetBoundary(CDMatrix &B) { B.SetSize(0); }; //= 0;
    virtual void Build(flag bShowEqn) { BuildParmsBlk(bShowEqn); };
    virtual void Load(FxdEdtInfo &EI, Strng & Str) { LoadParmsBlk(EI, Str); };
    virtual long Parse(FxdEdtInfo &EI, Strng & Str) { return ParseParmsBlk(EI, Str, bObjModified); };
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str) { return ButtonPushedParmsBlk(EI, Str); };
    inline long GetNIV() { return 1L; };
    inline rC2DFn Fn() { return *pFn; };
    inline long NPts();
  };

//----------------------------------------------------------------------------

class DllImportExport C2DXAxis 
  {
  public:
    double         dXLoExt;    //max X Extent - Used By Fractional Calcs
    double         dXLoLim;    //max valid X value (boundry)            
    double         dXUpLim;    //min valid X value (boundry)            
    double         dXUpExt;    //min X Extent - Used By Fractional Calcs
    int            iNPts;      //number of Points in Fn
    double         dGeomRatio; //geometric Ratio between points

    C2DXAxis(double XLoExt, double XLoLim, double XUpLim, double XUpExt, int NPts, double GeomRatio);
    C2DXAxis(C2DFn & Fn);
    C2DXAxis(C2DXAxis & Ax);
  };

//----------------------------------------------------------------------------

class /*DllImportExport*/ C2DRegStuff
  {
  public:
    CDVector Parms;
    CDVector MinParms;
    CDVector MaxParms;
    CDVector StdD;
    CDVector ParmsTol;
    CDVector YC;
    long NParms;     //number of parameters describing equation
    long N;          //number of points (Y values with corresponding X's
  };

//----------------------------------------------------------------------------

typedef flag (*fn_C2DFnDataCollection)(void * Ptr);

class DllImportExport C2DFn : public FxdEdtBookRef, public TaggedObject
  {
  friend class C2DModel;
  friend class C2DXAxis;
  protected:
    pC2DModel     pM;         //pointer to Model used
    CDVector      X;          //vector of X values (independent variable)
    CDVector      Y;          //vector of Y values
    CDVector     *pYFrac;     //A Fraction representation of Y Size = Size(Y)+1
    double        dXDispMax;  //max X value for display
    double        dXDispMin;  //min X value for display
    double        dYDispMax;  //max Y value for display
    double        dYDispMin;  //min Y value for display
    double        dXGRatio;   //scale type used for x axis (min 0.0001)
    double        dYGRatio;   //scale type used for y axis (min 0.0001)
    double        dXUpExt;    //max X Extent - Used By Fractional Calcs
    double        dXLoExt;    //min X Extent - Used By Fractional Calcs
    double        dXUpLim;    //max valid X value (boundry)
    double        dXLoLim;    //min valid X value (boundry)
    double        dYUpLim;    //max valid Y value (boundry)
    double        dYLoLim;    //min valid Y value (boundry)
    long          iRqdLength; //required number of points when points are added or removed
    long          iNoElimIter;//number of iterations to eliminate points
    long          iGraphWidth;//width of graphics graph window
    long          iGraphHeight;//height of graphics graph window
    flag          bShowEqn;   //must the equation info be shown
    flag          bShowOther; //must other config info be shown
    flag          bShowData;  //must the x & y points be shown
    flag          bXLog;      //is the X axis a log scale
    flag          bYLog;      //is the Y axis a log scale
    flag          bRedist;    //must points be redistributed after each math operation
    flag          bAutoScale; //must the display ranges automatically be calculated
    flag          bYCumulative; //must the display of the Y Scale be Cumulative
    flag          bReDoFrac;  //
    Strng         PtsFName;   //filename of file from which points are loaded
    Strng         sXName;     //name of x axis
    Strng         sYName;     //name of y axis

    flag          bAllowCumulative;
    flag          bAllowRedist;
    flag          bAllowYtoCurve;
    flag          bAllowRefit;

    fn_C2DFnDataCollection pCollectDataFn;
    void *        pCollectDataPtr;

  public:
    C2DFn(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    C2DFn(pchar pModelId, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    C2DFn(pchar pModelId, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach, pchar pXName, pchar pYName);
    C2DFn(C2DFn& Fn);
    virtual ~C2DFn();
    void Clear();
    void CopyFunction(C2DFn& Fn);
    C2DFn & operator=(C2DFn& Fn) { CopyFunction(Fn); return *this; };

    //static void Get2DModelDesc(pTagObjClass pC, rStrng S);
    void SetLength(long n);
    pchar ModelName(pchar pReqdModel = NULL);
  protected:
    pchar FindModelByClassId(pchar pReqdModelClassId);
    pchar FindModelByDesc(pchar pReqdModelDesc);
    flag ChangeModel(pchar pReqdModelClassId);
  public:
    flag ReFit();
    flag LoadPts(char* FName, CCnvIndex  XCnvDC=0, CCnvIndex  YCnvDC=0);
    flag SavePts(char* FName, char * XLabel="X", char * YLabel="Y");
    void MovePt(long i, double x, double y);
    flag GeoDistribute(long NReqd, double GeometricRatio=1.0);
    flag DoRedistribute();
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
    void Sort(flag Ascending = TRUE);
    void SetXPts(double *RqdXs, int NXs);
    void SetXPts(CDVector &RqdXs);
    flag SetXPts(C2DFn &Fn);
    void CombineXs(C2DFn &Fn);
    flag InsertXPts(C2DFn &Fn);
    flag Add(double Offset);
    flag Add(C2DFn &Fn, double Scalar = 1.0, double Offset = 0.0);
    flag Multiply(double Scalar);
    flag Multiply(C2DFn &Fn, double Scalar = 1.0, double Offset = 0.0);
    void ReCalcFrac();
    //flag ToFractional();
    double ToFractional(double Area=1.0);
    //flag ToCumulative(double ConstInteg);
    double ToCumulative(double ConstInteg=0.0, double Total=1.0);
    double SetCumulativeArea(double ConstInteg, double RqdArea);
    double ScaleEndPoint(double RqdEndPt);
    flag Convolve(C2DFn &Fn, double Frac = 1.0, double Offset = 0.0);
    flag GetYFrac(C2DFn &Fn);
    flag LBEst(CDVector &DeltaParms, double &Sa, long &Iter, const long ItMax);
  protected:
    double RegSumSSQ(C2DRegStuff &R, CDVector &Parms);
    double RegEvalDeriv(C2DRegStuff &R, long i, long k);
    void RegConstructMatrix(C2DRegStuff &R, CDMatrix &Z, CDVector &C);
  public:
    //double Integrate(double IntConst);
    //flag Differentiate();
    //double NormaliseCumulative(double IntConst);
    void dbgDump( pchar Desc = "", flag Horizontal = False);

    void SetDataCollection(fn_C2DFnDataCollection pCollectDataFn_, void * pCollectDataPtr_) { pCollectDataFn=pCollectDataFn_; pCollectDataPtr=pCollectDataPtr_; };

    virtual void   BuildDataDefn(DataDefnBlk &DDB);
    virtual flag   DataXchg(DataChangeBlk &DCB);
    virtual void   Build();
    virtual void   Load(FxdEdtInfo &EI, Strng & Str);
    virtual long   Parse(FxdEdtInfo &EI, Strng & Str);
    virtual long   ButtonPushed(FxdEdtInfo &EI, Strng & Str);

    inline double Yx(double x) { return pM->Yx(x); };
    inline long Length(long* RqdLen = NULL) { if (RqdLen) SetLength(*RqdLen); return Y.GetLen(); };
    inline long GetLen() { return Y.GetLen(); };
    inline long GetNIV() { return 1L; };
    inline double Xy(double y) { return pM->Xy(y); };
    inline double XDispMax() { return dXDispMax; };
    inline double XDispMin() { return dXDispMin; };
    inline double YDispMax() { return dYDispMax; };
    inline double YDispMin() { return dYDispMin; };
    inline void SetXDispMax(double x) { dXDispMax = x; };
    inline void SetXDispMin(double x) { dXDispMin = x; };
    inline void SetYDispMax(double y) { dYDispMax = y; };
    inline void SetYDispMin(double y) { dYDispMin = y; };
    inline double XUpExtent() { return dXUpExt; };
    inline double XLoExtent() { return dXLoExt; };
    inline double XUpLimit() { return dXUpLim; };
    inline double XLoLimit() { return dXLoLim; };
    inline double YUpLimit() { return dYUpLim; };
    inline double YLoLimit() { return dYLoLim; };
    inline void SetXLimits(double XLoExt, double XLoLim, double XUpLim, double XUpExt) { dXLoExt=XLoExt; dXLoLim=XLoLim; dXUpLim=XUpLim; dXUpExt=XUpExt; };
    inline void SetXLimits(C2DXAxis & Lims) { dXLoExt=Lims.dXLoExt; dXLoLim=Lims.dXLoLim; dXUpLim=Lims.dXUpLim; dXUpExt=Lims.dXUpExt; GeoDistribute(Lims.iNPts, Lims.dGeomRatio); };
    inline double Xp(long i) { return X[i]; };
    inline double Yp(long i) { return Y[i]; };
    inline flag XLog() { return bXLog; };
    inline flag YLog() { return bYLog; };
    inline flag YCumulative() { return bYCumulative; };
    inline double ScanXMin() { return X.ScanMin(); };
    inline double ScanXMax() { return X.ScanMax(); };
    inline double ScanYMin() { return Y.ScanMin(); };
    inline double ScanYMax() { return Y.ScanMax(); };
    inline rCDVector Xs() { return X; };
    inline rCDVector Ys() { return Y; };
    inline void SetPt(long i, double x, double y) { X[i] = x; Y[i] = y; bReDoFrac = True; };
    inline void SetXPt(long i, double x) { X[i] = x; bReDoFrac = True; };
    inline void SetYPt(long i, double y) { Y[i] = y; bReDoFrac = True; };
    inline flag InRegion(double x) { return (x>=dXLoLim && x<=dXUpLim); };
    inline void SetAxisNames(pchar pXName, pchar pYName) { sXName = pXName; sYName = pYName; };
    inline pchar XName() { return sXName.Length()==0 ? "" : sXName(); };
    inline pchar YName() { return sYName.Length()==0 ? "" : sYName(); };
    inline int XNameLen() { return sXName.Length(); };
    inline int YNameLen() { return sYName.Length(); };
    inline flag AutoScale() { return bAutoScale; };
    inline double YpFrac(long i) { if (bReDoFrac) ReCalcFrac(); return (*pYFrac)[i]; };
    inline double XpFrac(long i) { if (i<0) return dXLoLim; int N=X.GetLen(); return i<N ? X[i] : Max(X[N-1], dXUpLim); };
    inline GraphWidth() { return iGraphWidth; };
    inline GraphHeight() { return iGraphHeight; };
  };

// ===========================================================================

inline long C2DModel::NPts() { return pFn->Y.GetLen(); };
inline C2DXAxis::C2DXAxis(double XLoExt, double XLoLim, double XUpLim, double XUpExt, int NPts, double GeomRatio)
  {
  dXLoExt=XLoExt;
  dXUpExt=XUpExt;
  dXLoLim=XLoLim;
  dXUpLim=XUpLim;
  iNPts=NPts;  
  dGeomRatio=GeomRatio;
  }
inline C2DXAxis::C2DXAxis(C2DFn & Fn)
  {
  dXLoExt=Fn.dXLoExt;
  dXUpExt=Fn.dXUpExt;
  dXLoLim=Fn.dXLoLim;
  dXUpLim=Fn.dXUpLim;
  iNPts=Fn.Length();  
  dGeomRatio=Fn.dXGRatio;
  }
inline C2DXAxis::C2DXAxis(C2DXAxis & Ax)
  {
  dXLoExt=Ax.dXLoExt;
  dXUpExt=Ax.dXUpExt;
  dXLoLim=Ax.dXLoLim;
  dXUpLim=Ax.dXUpLim;
  iNPts=Ax.iNPts;  
  dGeomRatio=Ax.dGeomRatio;
  }

// ===========================================================================

#undef DllImportExport

#endif

