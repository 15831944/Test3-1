//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef __3D_FNEDT_H
#define __3D_FNEDT_H

#ifndef __FIXEDIT_H
#include "fixedit.h"
#endif
#ifndef __GPWNDS_H
#include "gpwnds.h"
#endif

#ifdef __3D_FNEDT_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================

_FWDDEF(C3DFnEdt);
class DllImportExport C3DFnEdt : public FxdEdtBookRef
  {
  public :
    pC3DFn         pFn;
    flag           bBusy;      //True while contour line is being built
    int            GrfRows, GrfCols;
    RECT           GraphR;
    int            CrsRows,CrsCols;
    RECT           CrsR;
    double         XMin;       //normalised x min (lhs of graph)
    double         XMax;       //normalised x max (rhs of graph)
    double         XFactor;    // = XPix/(XMax-XMin)
    double         YMin;       //normalised y min (bottom of graph)
    double         YMax;       //normalised y max (top of graph)
    double         YFactor;    // = YPix/(YMax-YMin)
    double         ZMin;
    double         ZMax;
    double         ZFactor;
    int            iDragPtNo;  //the index of the point being moved (-1 if none)
    int            iNameWidth; //
    CLabelWnd*     pLabels;    //
    static const int XPix;
    static const int YPix; 
    static const int CrvPts;

    C3DFnEdt(pFxdEdtView pView_, pC3DFn pFn);
    virtual ~C3DFnEdt();

    virtual void StartBuild();
    virtual void Build();
    virtual void ChangeLayout(rFxdEdtPage Pg, int TotalRows, int TotalCols);
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

    virtual void OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn);
    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoMouseMove(UINT nFlags, CPoint point);

    void SetPosition(rGDIBlk GB);
    void PointtoLP(POINT &Pt);
    void PointtoWP(POINT Pt, double& x, double& y);
    void PointtoCrsP(POINT Pt, double& d, double& z);
    int ClosestPt(CPoint point);
    void ToggleDigCursor(POINT &Pt);

    void SimpleContour(CDC &DC);
    void DoTriangle(CDC &DC,double *a,double *b,double *c,double dz);
  };

// ==========================================================================

#undef DllImportExport

#endif
