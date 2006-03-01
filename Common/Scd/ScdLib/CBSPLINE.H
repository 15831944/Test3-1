// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  _CBSPLINE_H
#define  _CBSPLINE_H

//#ifndef __SC_DEFS_H
//  #include "sc_defs.h"
//#endif

#ifndef __VECTORS_H
  #include "vectors.h"
#endif

#ifdef _CBSPLINE_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport DllImport
#endif

//============================================================================

/* These cubic spline algorithms are taken from Numerical Recipes in Pascal 
   (Press, Flannery, Teukolsky & Vetterling) */
									
_FWDDEF(CubicSpline);
class DllImportExport CubicSpline 
  {
  public:
	static const double yp1;
	static const double ypn;

	CDVector X;
	CDVector X1;
	CDVector Y;
	CDVector dY;
	CDVector dX;
	CDMatrix YTable, dYTab; 

    CubicSpline();
  virtual ~CubicSpline();
    
  virtual void   Clear();

	virtual void Spline(rCDVector x, rCDVector y, rCDVector y2);
	virtual int  Splint(rCDVector x, rCDVector y, rCDVector y2, double xv, double *yv);
	virtual void Spline2(rCDVector x1, rCDVector x2, rCDMatrix ytab, rCDMatrix y2tab);
	virtual int  Splint2(rCDVector x1, rCDVector x2, rCDMatrix ytab, rCDMatrix y2tab, 
	                                                double xv1, double xv2, double *yv);

  virtual double Xy(double Yi);   // interpolate an X value given a Y
  virtual double Yx(double Xi);	// interpolate a Y value given an X;
  virtual double Yxx(double Xi, double Xii);	// interpolate a Y value from the table given X and X1

  virtual void GetXYDerivs();
	virtual void GetTableDerivs();
  };

// ===========================================================================

#undef DllImportExport

#endif
