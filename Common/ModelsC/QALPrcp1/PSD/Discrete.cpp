//$T Discrete.c GC 1.136 01/11/02 11:22:47
// Discrete.f -- translated by f2c (version of 23 April 1993  18:34:30).
//   You must link the resulting object file with the libraries:
//	-lf2c -lm   (in that order)
//
#include "stdafx.h"
#include "sc_defs.h"
#include "flwnode.h"
#define  __DISCRETE_CPP

#include "myf2c.h"
#include "PrecipPSD.h"


//#define min(a,b) ((a) <= (b) ? (a) : (b))
//#define max(a,b) ((a) >= (b) ? (a) : (b))

// Table of constant values
static double c_b4 = 2.;
static long    c__3 = 3;
static long    c__0 = 0;

// cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

int CPrecipPSD::EqnsToSolve
                    (
                      double  *y,
                      double  *yin,
                      double  *g,
                      double  *nuc,
                      double  *agg,
                      double  *res_time__,
                      double  *grow_coeff__,
                      double  *aggl_coeff__,
                      long     *q,
                      long     *npts,
                      double  *yp
                    )
  {
  long                   i;
  double                DotTmp[MaxPSDGridPts];
  double                PSDDotDiff[MaxPSDGridPts];

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     rate of change of variables due to input/output to tank
  // Parameter adjustments
  CPSDStates &St  = *((CPSDStates*)y);
  CPSDStates &In  = *((CPSDStates*)yin);
  CPSDStates &Dot = *((CPSDStates*)yp);

  // Function Body
  for(i = 0; i < St.NStates(); ++i)
    {
    Dot[i] = -(St[i] - In[i]) / *res_time__;
    if (Dot[i]<0.0)
      { int xxx=0; }
    }

  // remember psd changes due to advection - CNM 
  for(i = 0; i < St.NPSD(); ++i)
    PSDDotDiff[i] = Dot.PSD(i);

  if (1)
    {
    //     rate of change of distribution due to growth and nucleation
    PureGrowth(St.PSD(), grow_coeff__, npts, DotTmp);

    for(i = 0; i < St.NPSD(); ++i)
      {
      Dot.PSD(i) += *g * 1e6 * DotTmp[i];
      if (Dot.PSD(i)<0.0)
        { int xxx=0; }
      }

    //     rate of change of distribution due to nucleation
    Dot.PSD(0) += *nuc;

    //     rate of change of distribution due to agglomeration
    if(*agg > 0.)
      {
      PureAgglom(St.PSD(), q, npts, aggl_coeff__, DotTmp);

      for(i = 0; i < St.NPSD(); ++i)
        {
        Dot.PSD(i) += *agg * DotTmp[i];
        if (Dot.PSD(i)<0.0)
          { int xxx=0; }
        }
      }
    }

  // Bound Soda
  Dot.rSoda()       -=m_DerivsPars.m_SodaRate;
  Dot.rBndSoda()    +=m_DerivsPars.m_SodaRate;
  Dot.rOrgSoda()    -=m_DerivsPars.m_OrgSodaRate;
  Dot.rBndOrgSoda() +=m_DerivsPars.m_OrgSodaRate;

  // Heat of Reaction
  if (m_DerivsPars.m_bWithHOR)
    {
    // calculate sum psd changes due to growth etc - CNM 
    for(i = 0; i < St.NPSD(); ++i)
      PSDDotDiff[i] = Dot.PSD(i) - PSDDotDiff[i];

    double mom3 = Moment(PSDDotDiff, 3, &m_GridPts.npts, m_GridPts.x) * 1e-18;  // calc 3rd moment
    //     convert basic variables to State2Conc variables
    double drh = m_ModelParms.m_RhoH * 3.141592653589793 * mom3 / 6.0; // rh in tank
 
    double dt=drh*m_DerivsPars.m_dTRate;
    Dot.Temp() += dt;
    m_DerivsPars.m_dReactionHeat = dt/3600.0*m_DerivsPars.m_totCp;
    }
  else
    m_DerivsPars.m_dReactionHeat=0;

  // ThermalLoss
  if (1)
    {
    double dt=3600.0*m_DerivsPars.m_dThermalConst*(m_DerivsPars.m_dAmbientTemp-St.Temp());
    Dot.Temp() += dt;
    m_DerivsPars.m_dThermalLoss = -dt/3600.0*m_DerivsPars.m_totCp;
    }
  else
    m_DerivsPars.m_dThermalLoss=0;
  return 0;
  } // EqnsToSolve

// **********************************************************************
//     Pure_Growth: A subroutine to calculate growth discretization.
// ***********************************************************************

int CPrecipPSD::PureGrowth(double *dist, double *grow_coeff__, long *npts, double *distp)
  {
  long i__1, i__2, i__3, i__4, i__5, i__6;

  long i, j;

  // cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     Dist          particle size distribution
  //     DistP         derivative (wrt time) of psd
  //     i             counter in do loop
  //     j             counter in do loop
  //     nu            number of upper diagonals in Jacobian matrix
  //     nl            number of lower diagonals in Jacobian matrix
  //     npts          number of points in discretization of psd
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  // Parameter adjustments
  --distp;
  grow_coeff__ -= 5;
  --dist;

  // Function Body
  i__1 = *npts;
  for(i = 1; i <= i__1; ++i)
    {
    distp[i] = 0.;

    // Computing MAX
    i__2 = 1, i__3 = i + 2 -*npts;

    // Computing MIN
    i__5 = 4, i__6 = i + 1;
    i__4 = min(i__5, i__6);
    for(j = max(i__2, i__3); j <= i__4; ++j)
      {
      distp[i] += grow_coeff__[j + ((i - j + 2) << 2)] * dist[i - j + 2];
      }
    }

  return 0;
  } // PureGrowth

// **********************************************************************
//     Pure_Agglom: A subroutine to calculate agglomeration
//     discretization for a constant kernel
// ***********************************************************************

int CPrecipPSD::PureAgglom
                    (
                      double  *dist,
                      long     *q,
                      long     *npts,
                      double  *aggl_coeff__,
                      double  *distp
                    )
  {
  long i__1, i__2, i__3, i__4;

  // Builtin functions
  double pow_dd(double , double ), pow_di(double , long ), log(double);

  double  sumi, dist1[MaxPSDGridPts];
  long     i, j, k, s[20];
  double  qroot, sumup, qroot1, rq, tmp, rqq, sumdown;
  long     i1st;
  double  tmp1, tmp2;

  // cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     Dist          particle size distribution
  //     DistP         derivative (wrt time) of psd
  //     npts          number of points in discretization of psd
  //     q             parameter for grid generation
  //     s             parameter for agglomeration calculation
  //     qroot         parameter for agglomeration calculation
  //     qroot1        parameter for agglomeration calculation
  //     rq           parameter for agglomeration calculation
  //     rqq           parameter for agglomeration calculation
  //     i             counter in do loop
  //     i1st          range long in a do loop
  //     j             counter in do loop
  //     k             counter in do loop
  //     SumUp         variable for accumulating sum in a loop
  //     SumDown       variable for accumulating sum in a loop
  //     SumI          variable for accumulating sum in a loop
  //     tmp           temporary storage
  //     tmp1          temporary storage
  //     tmp2          temporary storage
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     Calculate preliminary variables
  // Parameter adjustments
  --distp;
  --aggl_coeff__;
  --dist;

  // Function Body
  rq = 1. / (double) (*q);
  qroot = pow_dd(c_b4, rq);
  qroot1 = qroot - 1.;

  //     Calculate S(i) for i=1,..q using new definition by Wynn (1996).
  // parameter for agglom calc
  i__1 = *q;
  for(i = 1; i <= i__1; ++i)
    {
    i__2 = -i;
    tmp = (double) (*q) * log(1. - pow_di(qroot, i__2)) / log(2.);
    s[i - 1] = (long) (1.0001 - tmp);

    // parameter for agglom calc
    }

  rqq = 1. / (qroot1 * pow_di(qroot, *s));

  //     Start Calculation
  // parameter for agglom calc
  i__1 = *npts;
  for(i = 1; i <= i__1; ++i)
    {
    dist1[i - 1] = aggl_coeff__[i] * dist[i];
    }

  i__1 = *q;
  for(i = 1; i <= i__1; ++i)
    {
    distp[i] = 0.;
    }

  i__1 = *npts;
  for(i = *q + 1; i <= i__1; ++i)
    {
    distp[i] = dist1[i -*q - 1] * .5 * dist1[i -*q - 1];
    }

  sumup = 0.;
  i__1 = *npts;
  for(j = *npts - s[0] + 3; j <= i__1; ++j)
    {
    sumup += dist1[j - 1];
    }

  i__1 = *npts - s[0] + 1;
  for(i = 0; i <= i__1; ++i)
    {
    sumup += dist1[*npts - s[0] + 2 - i - 1];
    distp[*npts - i] -= dist1[*npts - i - 1] * sumup;
    }

  i__1 = s[0] - 2;
  for(i = 1; i <= i__1; ++i)
    {
    distp[i] -= dist1[i - 1] * sumup;
    }

  sumdown = 0.;
  i__1 = *npts;
  for(i = s[0]; i <= i__1; ++i)
    {
    sumdown = sumdown / qroot + dist1[i - s[0]];
    tmp = dist1[i - 1] * sumdown * rqq;
    distp[i] -= tmp;
    if(i < *npts)
      {
      distp[i + 1] += tmp;
      }
    }

  i__1 = *q;
  for(k = 2; k <= i__1; ++k)
    {
    tmp1 = qroot / pow_di(qroot, k) - 1.;
    i__2 = *npts;
    for(i = k + 1; i <= i__2; ++i)
      {
      // Computing MAX
      i__3 = 1, i__4 = i - s[k - 2];
      i1st = max(i__3, i__4);
      sumi = 0.;
      i__3 = i - s[k - 1];
      for(j = i1st; j <= i__3; ++j)
        {
        i__4 = j - i + 1;
        tmp2 = pow_di(qroot, i__4);
        sumi += dist1[j - 1] * (tmp1 + tmp2);
        }

      distp[i] += dist1[i - k - 1] * sumi / qroot1;
      }
    }

  i__1 = *q - 1;
  for(k = 1; k <= i__1; ++k)
    {
    i__2 = k + 1;
    tmp1 = qroot * (1. - 1. / pow_di(qroot, i__2));
    i__2 = *npts;
    for(i = k + 1; i <= i__2; ++i)
      {
      // Computing MAX
      i__3 = 1, i__4 = i + 1 - s[k - 1];
      i1st = max(i__3, i__4);
      sumi = 0.;
      i__3 = i + 1 - s[k];
      for(j = i1st; j <= i__3; ++j)
        {
        i__4 = j - i;
        tmp2 = pow_di(qroot, i__4);
        sumi += dist1[j - 1] * (tmp1 - tmp2);
        }

      distp[i] += dist1[i - k - 1] * sumi / qroot1;
      }
    }

  return 0;
  } // PureAgglom

// ***********************************************************************
//     Calc_Growth_Coeff: A subroutine to calculate coefficients for
//     growth discretization. Indices are chosen to be compatible with
//     linpack representation of banded matrices.
// ***********************************************************************

int CPrecipPSD::CalcGrowthCoeff(double *x, long *npts, long *grow_dis__, double *grow_coeff__)
  {
  long     i__1;

  double  a[4];
  long     i;
  double  w, r1, r2, r3, r4, ff[4], fs[4], sf[4], ss[4];

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     npts          number of points in discretization of psd
  //     x             midpoints of discretization
  //     Grow_Dis      Growth discretization type
  //     grow_coeff    coefficients for growth discretization
  //     a              coefficients for scheme
  //     ff             coefficients for first, first order scheme
  //     fs             coefficients for second, first order scheme
  //     i              counter in do loop
  //     r1             ratio
  //     r2             ratio^2
  //     r3             ratio^3
  //     r4             ratio^4
  //     sf             coefficients for first, second order scheme
  //     ss             coefficients for second, second order scheme
  //     w              weight
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  // Parameter adjustments
  grow_coeff__ -= 5;
  --x;

  // Function Body
  r1 = x[2] / x[1];
  r2 = r1 * r1;
  r3 = r1 * r2;
  r4 = r1 * r3;

  //     First Order Scheme - uses pts i-1, i
  //     Exact for 0th & 3rd Moments
  //     Stable but diffusive
  ff[0] = 0.;
  ff[1] = r1 * 3. / (r3 - 1.);
  ff[2] = -3. / (r3 - 1.);
  ff[3] = 0.;

  //     First Order Scheme - uses pts i, i+1
  //     Exact for 0th & 3rd Moments
  //     Unstable by itself
  fs[0] = 0.;
  fs[1] = 0.;
  fs[2] = r3 * 3. / (r3 - 1.);
  fs[3] = r2 * -3. / (r3 - 1.);

  //     Second Order Scheme - uses pts i-1, i, i+1
  //     Exact for 0th, 2nd & 3rd Moments
  sf[0] = (float) 0.;
  sf[1] = r1 * (r1 + 2.) / (-1. - r1 + r3 + r4);
  sf[2] = (r1 + 2. + r2 * 2.) / (r1 + 1. + r2);
  sf[3] = -(r2 * (r1 * 2. + 1.)) / (-1. - r1 + r3 + r4);

  //     Second Order Scheme - uses pts i-2, i-1, i
  //     Exact for 0th, 2nd & 3rd Moments
  ss[0] = -(r1 * 2. + 1.) / (-1. - r1 + r3 + r4);
  ss[1] = (r1 + 1.) * (r2 * 2. + 1.) / (r1 * (r3 - 1.));
  ss[2] = -(r1 * 4. + 3. + r2 * 2.) / (-1. - r1 + r3 + r4);
  ss[3] = 0.;

  if(*grow_dis__ <= 2)
    {
    if(*grow_dis__ == 1)
      {
      w = 0.;
      }
    else
      {
      w = 1. / (r1 + 1. + r2 + r3);
      }

    //     combine first order schemes
    for(i = 1; i <= 4; ++i)
      {
      a[i - 1] = w * fs[i - 1] + (1. - w) * ff[i - 1];
      }
    }
  else
    {
    if(*grow_dis__ == 3)
      {
      w = (r1 + 1.) / (r1 * 2. + 1.);
      }
    else if(*grow_dis__ == 4)
      {
      w = r2 / 2.;
      }
    else
      {
      w = r1 / (r2 + r1 + 1.);
      }

    //     combine second order schemes
    for(i = 1; i <= 4; ++i)
      {
      a[i - 1] = w * sf[i - 1] + (1. - w) * ss[i - 1];
      }
    }

  //     Use banded matrix structure consistent with LSODA and LINPACK
  //     First correct coefficient in first equation
  grow_coeff__[6] = -a[1] / x[2] - a[0] / x[3];

  //     Now the basic growth equation
  //     Note that there are no corrections at the end
  i__1 = *npts - 1;
  for(i = 1; i <= i__1; ++i)
    {
    grow_coeff__[((i + 1) << 2) + 1] = a[3] / x[i];
    }

  i__1 = *npts;
  for(i = 2; i <= i__1; ++i)
    {
    grow_coeff__[(i << 2) + 2] = a[2] / x[i];
    }

  i__1 = *npts;
  for(i = 2; i <= i__1; ++i)
    {
    grow_coeff__[((i - 1) << 2) + 3] = a[1] / x[i];
    }

  i__1 = *npts;
  for(i = 3; i <= i__1; ++i)
    {
    grow_coeff__[((i - 2) << 2) + 4] = a[0] / x[i];
    }

  return 0;
  } // CalcGrowthCoeff

// ***********************************************************************
//     Calc_Agglom_Coeff: A subroutine to calculate coefficients for
//     agglomeration discretization.
// ***********************************************************************

int CPrecipPSD::CalcAgglomCoeff(double *x, double *akf, long *npts, double *aggl_coeff__)
  {
  long     i__1;
  double  d__1;

  // Builtin functions
  double exp(double), pow_dd(double , double );

  double  xend;
  long     i;
  double  d1, d2;

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     i              counter in do loop
  //     akf            agglomeration parameter
  //     d1             agglomeration parameter
  //     d2             agglomeration parameter
  //     xend           agglomeration parameter
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  // Parameter adjustments
  --aggl_coeff__;
  --akf;
  --x;

  // Function Body
  d1 = akf[1];
  d2 = akf[2];
  xend = akf[3]*1e6;
  i__1 = *npts;
  for(i = 1; i <= i__1; ++i)
    {
    if(x[i] <= xend)
      {
      aggl_coeff__[i] = 1.;
      }
    else
      {
      aggl_coeff__[i] = 0.;
      }

    if(d1 != 0.)
      {
      aggl_coeff__[i] *= Exps(-x[i] / d1);
      }

    if(d2 != 0.)
      {
      d__1 = x[i] / d2;
      aggl_coeff__[i] *= Exps(-pow_dd(d__1, c_b4));
      }
    }

  return 0;
  } // CalcAgglomCoeff

// **********************************************************************
//     Set_Grid: Subroutine to calculate midpoints of a geometrically
//     spaced grid
// **********************************************************************

int CPrecipPSD::SetGrid(double *xmin, long *q, long *npts, double *x)
  {
  long     i__1;
  double  d__1;

  // Builtin functions
  double      pow_dd(double , double );

  long     i;
  double  ratio;

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     x             midpoints of discretization
  //     xmin          minimum particle size midpoint
  //     npts          number of points in discretization of psd
  //     ratio         ratio of successive gridpoints
  //     i             counter in do loop
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  // Parameter adjustments
  --x;

  // Function Body
  d__1 = 1. / ((double) (*q) * 3.);
  ratio = pow_dd(c_b4, d__1);

  // grid space ratio
  x[1] = *xmin;
  i__1 = *npts - 1;
  for(i = 1; i <= i__1; ++i)
    {
    x[i + 1] = ratio * x[i]; // calc midpoints in geom grid
    }

  return 0;
  } // SetGrid

// **********************************************************************
//     Moment: Function to calculate moment of psd

// **********************************************************************
double CPrecipPSD::Moment(double *dist, long j, long *npts, double *x)
  {
  // Builtin functions
  double      pow_di(double , long );
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     npts          number of points in discretization of psd
  //     x             midpoints of discretization
  //     Dist          particle size distribution
  //     i             counter in do loop
  //     j             index of moment
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

  // Parameter adjustments
  --x;
  --dist;

  // Function Body
  double ret_val = 0.0;
  for(long i = 1; i <= *npts; ++i)
    {
    ret_val += dist[i] * pow_di(x[i], j); // note x is midpoint
    }

  return ret_val;
  } // Moment

// **********************************************************************
//     Moment: Function to calculate moment of psd

// **********************************************************************
double CPrecipPSD::SpecificSurfArea(double THAdens, double *dist, long *npts, double *x)
  {
  // Builtin functions
  double      pow_di(double , long );
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     npts          number of points in discretization of psd
  //     x             midpoints of discretization
  //     Dist          number particle size distribution in 
  //     i             counter in do loop
  //     THAdens       hydrate density
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

  // Parameter adjustments
  //--x;
  //--dist;

  //double mdist[MaxPSDGridPts];
  //for (int i=0; i < *npts; i++)
  //  mdist[i]=dist[i];

  double mom2 = Moment(dist, 2, &m_GridPts.npts, m_GridPts.x) * 1e-12;  // calc 2nd moment
  double mom3 = Moment(dist, 3, &m_GridPts.npts, m_GridPts.x) * 1e-18;  // calc 3rd moment
  double surf = mom2 * 3.141592653589793;  // Surface area

  //Mass of alumina solids / kg slurry
  double rh = m_ModelParms.m_RhoH * 3.141592653589793 * mom3 / 6.0; // rh in tank
  double SSA=surf/(rh*1000);

  return SSA;
  //NumberToMass(mdist, npts, x);
  //
  //double TotMass=0.0;
  //double TotArea=0.0;
  //
  //for (i=0; i < *npts; i++)
  //  {
  //  double D=x[i]*1.0e-6;
  //  double SAMAtD=3.0/GTZ(500.*THAdens*D);
  //  TotMass+=mdist[i];
  //  TotArea+=SAMAtD*mdist[i];
  //  //dbgpln("%12.6f %12.6f %12.6f", x[i], mdist[i], SAMAtD);
  //  }
  //double dSAM=TotArea/GTZ(TotMass);
  ////dbgpln("%12s %12.6f %12.6f %12.6f", "", TotMass, TotArea, dSAM);
  //
  //return dSAM;
  } // Moment

// **********************************************************************
//     Number_to_Mass: Subroutine to convert number fraction psd to
//     mass (or volume) fraction psd
// **********************************************************************

int CPrecipPSD::NumberToMass(double *dist, long *npts, double *x)
  {
  long           i__1;
  double        d__1, d__2;

  double        const_;
  long           ii;
  double        mom3;

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     npts          number of points in discretization of psd
  //     x             midpoints of discretization
  //     const         normalizing factor
  //     dist          particle size distribution
  //     ii            counter in do loop
  //     mom3          3rd moment of psd
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //    Functions Used
  //    Moment         Calculates moments from psd
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  // Parameter adjustments
  --x;
  --dist;

  // Function Body
  mom3 = Moment(&dist[1], 3, npts, &x[1]);
  if(mom3 >= 1e-20)
    {
    const_ = 1. / mom3;
    }
  else
    {
    const_ = 0.;
    }

  i__1 = *npts;
  for(ii = 1; ii <= i__1; ++ii)
    {
    // Computing 3rd power
    d__1 = x[ii], d__2 = d__1;
    dist[ii] = const_ * (d__2 * (d__1 * d__1)) * dist[ii];
    if (dist[ii]<0)
      {
      int xxx=0;
      }
    }

  return 0;
  } // NumberToMass

int CPrecipPSD::MassToNumber(double *dist, long *npts, double *x)
  {
  long           i__1;
  double        d__1;

  double pow_dd(double , double );

  long           ii;
  double        sum;
  double        scl;

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     npts          number of points in discretization of psd
  //     x             midpoints of discretization
  //     const         normalizing factor
  //     dist          particle size distribution
  //     ii            counter in do loop
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //    Functions Used
  //    Moment         Calculates moments from psd
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  // Parameter adjustments
  --x;
  --dist;

  sum=0;
  i__1 = *npts;
  for(ii = 1; ii <= i__1; ++ii)
    {
    // Computing 3rd power
    d__1 = x[ii];
    dist[ii] = dist[ii]/pow_dd(d__1, 3.0);
    if (dist[ii]<0)
      {
      int xxx=0;
      }
    sum+=dist[ii];
    }

  if (sum >= 1e-20)
    scl= 1/sum;
  else
    scl=0.0;

  for(ii = 1; ii <= i__1; ++ii)
    dist[ii] *= scl;

  return 0;
  } // NumberToMass

// **********************************************************************
//     Dist_Sauter: Subroutine calculates bin numbers for exponential
//     number psd given the Sauter mean diameter
// **********************************************************************

int CPrecipPSD::DistSauter(double *psd, double *l32, long *npts, double *x)
  {
  long           i__1;

  // Builtin functions
  double            exp(double);

  long           i;
  double        mom0;

  // cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     npts          number of points in discretization of psd
  //     x             midpoints of discretization
  //     i            counter in do loop
  //     L32          Sauter mean diameter
  //     mom0         0th moment
  //     psd          particle size distribution
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //    Functions Used
  //    Moment         Calculates moments from psd
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  // Parameter adjustments
  --x;
  --psd;

  // Function Body
  i__1 = *npts;
  for(i = 1; i <= i__1; ++i)
    {
    psd[i] = x[i] * Exps(x[i] * -3. / *l32);

    // note interval prop to x
    }

  mom0 = Moment(&psd[1], 0, npts, &x[1]);
  i__1 = *npts;
  for(i = 1; i <= i__1; ++i)
    {
    psd[i] /= mom0;

    // normalize
    }

  return 0;
  } // DistSauter

