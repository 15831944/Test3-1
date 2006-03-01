//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 
#include "stdafx.h"
#include "sc_defs.h"

#define _CBSPLINE_CPP

#include "cbspline.h"
#include "vectors.h"

// =========================================================================

const double CubicSpline::yp1=1e30;
const double CubicSpline::ypn=1e30;

 
CubicSpline::CubicSpline()
  {
  Clear();
  }
 
// -------------------------------------------------------------------------
 
CubicSpline::~CubicSpline()
  {
  Clear();
  }
 
//---------------------------------------------------------------------------
 
void CubicSpline::Clear()
  {
  dY.SetSize(0);
  dX.SetSize(0);
  dYTab.SetSize(0,0);
  }

// -------------------------------------------------------------------------
 
double CubicSpline::Xy(double Yi)
  {
  
  if ((Y.GetLen()>3)&&(X.ScanMin()>0.0)&&(Y.ScanMin()>0.0))
  	{
	if (dX.GetLen() <= 0)
	  {
	  Spline(Y, X, dX);
	  }
	double xv, yv;

	yv=Range(Y.ScanMin(), Yi, Y.ScanMax());  // can't extrapolate too well

	if (Splint(Y, X, dX, yv, &xv)==0)
	 return xv;
	else
	 return 0.0;
	}
  else
   return 0.0;
  }
 
// -------------------------------------------------------------------------
 
double CubicSpline::Yx(double Xi)
  {
  if ((X.GetLen()>3)&&(X.ScanMin()>0.0)&&(Y.ScanMin()>0.0))
	{
	if (dY.GetLen() <= 0)
	  {
      Spline(X, Y, dY);
	  }

	double yv, xv;

	xv=Range(X.ScanMin(), Xi, X.ScanMax());  // can't extrapolate too well
	
	if (Splint(X, Y, dY, xv, &yv)==0)
	 return yv;
	else
	 return 0.0;
	}
  else
   return 0.0;
  
  }

//----------------------------------------------------------

double CubicSpline::Yxx(double Xi, double Xii)
{
if ((X.GetLen()>3)&&(X1.GetLen()>3))
 {
 if ((dYTab.GetRows()<=0)||(dYTab.GetCols()<=0))	 
   Spline2(X, X1, YTable, dYTab);
 
 double xv1, xv2, yv;
 xv1=Range(X.ScanMin(), Xi, X.ScanMax());  
 xv2=Range(X1.ScanMin(), Xii, X1.ScanMax());

 if (Splint2(X, X1, YTable, dYTab, xv1, xv2, &yv)==0)
  return yv;
 else
  return 0.0;
 }
else
 return 0.0;
}

//----------------------------------------------------------

/*
  --------------------------------------------------------------------
 Given arrays x[0..n-1] and y[0..n-1] containing a tabulated function,  
 i.e. y = f(x) with x1 < x2 <....<xn, and given values yp1 and ypn  
 for the first derivative of the interpolating function at points 0 
 and n-1, respectively, this routine returns an array y2[0..n-1] that   
 contains the second derivative of the interpolating function at the 
 tabulated points xi. If yp1 and/or ypn are equal to 1e30 or larger, 
 the routine is signalled to set the corresponding boundary condition 
 for a natural spline, with zero second derivative on that boundary  
 --------------------------------------------------------------------
*/



void CubicSpline::Spline(rCDVector x, rCDVector y, rCDVector y2)
{
  long n = x.GetLen();
  if ((n>3)&&(x.ScanMin()>0.0)&&(y.ScanMin()>0.0))
	{
	y2.SetSize(n);
	//long n = NPts();
	CDVector u;
	u.SetSize(n);
    int i, k;
    double p, qn, sig, un;
    if (yp1 > 0.99e+30) 
      {					
      y2[0] = 0.0;
      u[0] = 0.0;
      }
    else  
      {
      y2[0]= - 0.5;	// check in book for error below
      u[0] = (3.0 / (x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-yp1);
	  }
    for (i=1; i<=n-2; i++) 
      {
      sig = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
      p = sig * y2[i - 1] + 2.0;
      y2[i] = (sig-1.0)/p;
      u[i] = (y[i+1]-y[i])/(x[i+1]-x[i])-(y[i]-y[i-1])/(x[i]-x[i-1]);
      u[i] = (6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
      }
    if (ypn > 0.99e+30) 
      {
      qn = 0.0;
      un = 0.0;
      }
    else  
      {
      qn = 0.5;
      un = (3.0/(x[n-1]-x[n-2]))*(ypn-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]));
      }
   y2[n-1] = (un-qn*u[n-2])/(qn*y2[n-2]+1.0);
    for (k = n-2; k >= 0; k--)
        y2[k] = y2[k]*y2[k+1]+u[k];
	}
}

// -------------------------------------------------------------------------
/*
{---------------------------------------------------------------------    }
{ Given the arrays xa[0..n-1] and ya[0..n-1], which tabulate a function   }
{ with the xa's in order), and given the array y2a[0..n-1], which is the  }
{ output from Spline above, and given a value of x, this routine          }
{ returns a cubic-spline interpolated value y.                            }
{--------------------------------------------------------------------    -}

*/


int CubicSpline::Splint(rCDVector x, rCDVector y, rCDVector y2,
                           double xv, double *yv)
{

long klo,khi, k, er;
double h, b, a;
long n = x.GetLen();


klo = 0;   //1;
khi = n-1; //n;
while (khi-klo > 1) 
 {
 k = (khi+klo) / 2;
 if (x[k] > xv)
   khi = k;
 else 
   klo = k;
 }
 //h = x.m_d[khi]-x.m_d[klo];
 h = x[khi]-x[klo];
 if (h==0.0) 
   {
   //printf(" pause in routine SPLINT\n");
   //printf("....bad xa input\n");
   er=1;
   }
 else
   er = 0;
 a = (x[khi]-xv)/h;
 b = (xv-x[klo])/h;
 *yv = a*y[klo]+b*y[khi]+((a*a*a-a)*y2[klo]
       +(b*b*b-b)*y2[khi])*(h*h)/6.0;

 return er;
}


//--------------------------------------------------------------------------

/* Given the tabulated independent variables  x1[0..m-1] and x2[0..n-1], and a tabulated   */
/* function ytab[0..m-1,0..n-1], this routine constructs one-dimensional natural cubic 	   */
/* splines of the rows ytab and returns the second-derivatives in the array 			   */
/* y2tab[0..m-1,0..n-1]  																   */

void CubicSpline::Spline2(rCDVector x1, rCDVector x2, rCDMatrix ytab, rCDMatrix y2tab)
{
long m, n;
CDVector ytmp, y2tmp;
m=x1.GetLen();
n=x2.GetLen();
//ytmp.SetSize(n, 0.0);
y2tmp.SetSize(n, 0.0);
y2tab.SetSize(m, n, 0);
for (long i=0; i<m; i++)
 {
 ytab.GetRow(i, ytmp);
 //for (long k=0; k<n; k++)  ytmp[k]=ytab(j,k);
 Spline(x2, ytmp, y2tmp);
 y2tab.SetRow(i, y2tmp);
 //for (k=0;k<n; k++)  y2tab(j,k)=y2tmp[k];
 }
}

// -------------------------------------------------------------------------

/*	Given x1, x2, ytab and y2tab as produced by Spline2 and given a desired interpolating    */
/*  point x1, x2; this routine returns an interpolated function value yv by bicubic spline 	 */
/*  interpolation.																			 */


int  CubicSpline::Splint2(rCDVector x1, rCDVector x2, rCDMatrix ytab, rCDMatrix y2tab, 
	                                                double xv1, double xv2, double *yv)
{
int res=0;
long m, n;
CDVector ytmp, y2tmp, yytmp;
m=x1.GetLen();
n=x2.GetLen();
yytmp.SetSize(m, 0.0);
for (long i=0; i<m; i++)
  {
  ytab.GetRow(i, ytmp);
  y2tab.GetRow(i, y2tmp);
  //for (long k=0; k<n; k++)
  // {
  // ytmp[k]=ytab(j,k);
  // y2tmp[k]=y2tab(j,k);
  // }
  if (Splint(x2, ytmp, y2tmp, xv2, &yytmp[i])!=0) res+=1;
  }
Spline(x1, yytmp, y2tmp);
if (Splint(x1, yytmp, y2tmp, xv1, yv)!=0) res+=1;
return res;
}
	                                                 
// -------------------------------------------------------------------------


void CubicSpline::GetXYDerivs()
{
Spline(X, Y, dY);
Spline(Y, X, dX);
}

//--------------------------------------------------------------------------

void CubicSpline::GetTableDerivs()
{
Spline2(X, X1, YTable, dYTab);
}

//--------------------------------------------------------------------------


// =========================================================================
