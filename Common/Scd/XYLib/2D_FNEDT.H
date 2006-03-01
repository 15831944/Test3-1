//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef __2D_FNEDT_H
#define __2D_FNEDT_H

#ifndef __FIXEDIT_H
#include "fixedit.h"
#endif  
#ifndef __GPWNDS_H
#include "gpwnds.h"
#endif  

#ifdef __2D_FNEDT_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================

_FWDDEF(C2DFnEdt);
class DllImportExport C2DFnEdt : public FxdEdtBookRef
  {
  public :
    pC2DFn         pFn;
    RECT           GraphR;
    double         XMin;       //normalised x min (lhs of graph)
    double         XMax;       //normalised x max (rhs of graph)
    double         XFactor;    // = XPix/(XMax-XMin)
    double         YMin;       //normalised y min (bottom of graph)
    double         YMax;       //normalised y max (top of graph)
    double         YFactor;    // = YPix/(YMax-YMin)
    int            iDragPtNo;  //the index of the point being moved (-1 if none)
    int            iNameWidth; //
    CLabelWnd*     pLabels;    //
    static const int XPix;     //number of x pixels
    static const int YPix;     //number of x pixels
    static const int CrvPts;

    C2DFnEdt(pFxdEdtView pView_, pC2DFn pFn);//, rStrng Tag, rStrng Name);
    virtual ~C2DFnEdt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void ChangeLayout(rFxdEdtPage Pg, int TotalRows, int TotalCols);
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);
//    virtual flag Changed(FxdEdtInfo &EI);
//    virtual void ClearChanged(FxdEdtInfo &EI);

    virtual void OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn);
    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoMouseMove(UINT nFlags, CPoint point);

    virtual void SetPosition(rGDIBlk GB);//, CRect *CRReqd);
  private:
    void SetRanges(CDVector& YPts, CDVector& XPts);
    void PointtoLP(POINT &Pt);
    void PointtoWP(POINT Pt, double& x, double& y);
    int ClosestPt(CPoint point);
    void ToggleDigCursor(POINT &Pt);

  };

// ==========================================================================

#undef DllImportExport

#endif
