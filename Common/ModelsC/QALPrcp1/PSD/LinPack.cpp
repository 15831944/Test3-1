//$T LINPACK.c GC 1.136 01/11/02 11:22:48
// LINPACK.f -- translated by f2c (version of 23 April 1993  18:34:30).
//   You must link the resulting object file with the libraries:
//	-lf2c -lm   (in that order)
//
#include "stdafx.h"
#include "sc_defs.h"
#include "myf2c.h"
#include "linpack.h"
#include <string.h>
#include <stdio.h>
#include "errorlog.h"

//#define min(a,b) ((a) <= (b) ? (a) : (b))
//#define max(a,b) ((a) >= (b) ? (a) : (b))

// Table of constant values
static long  c__1 = 1;
static long  c__9 = 9;
static long  c__3 = 3;

// Subroutine
int CLinPack::dgefa_(double *a, long *lda, long *n, long *ipvt, long *info)
  {
  // System generated locals
  long     a_dim1, a_offset, i__1, i__2, i__3;

  // Local variables
  long     j, k, l;
  double  t;
  long         kp1, nm1;

  //     dgefa factors a double precision matrix by gaussian elimination.
  //     dgefa is usually called by dgeco, but it can be called
  //     directly with a saving in time if  rcond  is not needed.
  //     (time for dgeco) = (1 + 9/n)*(time for dgefa) .
  //     on entry
  //        a       double precision(lda, n)
  //                the matrix to be factored.
  //        lda     long
  //                the leading dimension of the array  a .
  //        n       long
  //                the order of the matrix  a .
  //     on return
  //        a       an upper triangular matrix and the multipliers
  //                which were used to obtain it.
  //                the factorization can be written  a = l*u  where
  //                l  is a product of permutation and unit lower
  //                triangular matrices and  u  is upper triangular.
  //        ipvt    long(n)
  //                an long vector of pivot indices.
  //        info    long
  //                = 0  normal value.
  //                = k  if  u(k,k) .eq. 0.0 .  this is not an error
  //                     condition for this subroutine, but it does
  //                     indicate that dgesl or dgedi will divide by zero
  //                     if called.  use  rcond  in dgeco for a reliable
  //                     indication of singularity.
  //     linpack. this version dated 08/14/78 .
  //     cleve moler, university of new mexico, argonne national lab.
  //     subroutines and functions
  //     blas daxpy,dscal,idamax
  //     internal variables
  //     gaussian elimination with partial pivoting
  // Parameter adjustments
  --ipvt;
  a_dim1 = *lda;
  a_offset = a_dim1 + 1;
  a -= a_offset;

  // Function Body
  *info = 0;
  nm1 = *n - 1;
  if(nm1 < 1)
    {
    goto L70;
    }

  i__1 = nm1;
  for(k = 1; k <= i__1; ++k)
    {
    kp1 = k + 1;

    //        find l = pivot index
    i__2 = *n - k + 1;
    l = idamax_(&i__2, &a[k + k * a_dim1], &c__1) + k - 1;
    ipvt[k] = l;

    //        zero pivot implies this column already triangularized
    if(a[l + k * a_dim1] == 0.)
      {
      goto L40;
      }

    //           interchange if necessary
    if(l == k)
      {
      goto L10;
      }

    t = a[l + k * a_dim1];
    a[l + k * a_dim1] = a[k + k * a_dim1];
    a[k + k * a_dim1] = t;
L10:
    //           compute multipliers
    t = -1. / a[k + k * a_dim1];
    i__2 = *n - k;
    dscal_(&i__2, &t, &a[k + 1 + k * a_dim1], &c__1);

    //           row elimination with column indexing
    i__2 = *n;
    for(j = kp1; j <= i__2; ++j)
      {
      t = a[l + j * a_dim1];
      if(l == k)
        {
        goto L20;
        }

      a[l + j * a_dim1] = a[k + j * a_dim1];
      a[k + j * a_dim1] = t;
L20:
      i__3 = *n - k;
      daxpy_(&i__3, &t, &a[k + 1 + k * a_dim1], &c__1, &a[k + 1 + j * a_dim1], &c__1);

      // L30:
      }

    goto L50;
L40:
    *info = k;
L50:
    // L60:
    ;
    }

L70:
  ipvt[*n] = *n;
  if(a[*n +*n * a_dim1] == 0.)
    {
    *info = *n;
    }

  return 0;
  } // dgefa_

// Subroutine
int CLinPack::dgesl_(double *a, long *lda, long *n, long *ipvt, double *b, long *job)
  {
  // System generated locals
  long                   a_dim1, a_offset, i__1, i__2;

  // Local variables
  long                   k, l;
  double                t;
  long                   kb, nm1;

  //     dgesl solves the double precision system
  //     a * x = b  or  trans(a) * x = b
  //     using the factors computed by dgeco or dgefa.
  //     on entry
  //        a       double precision(lda, n)
  //                the output from dgeco or dgefa.
  //        lda     long
  //                the leading dimension of the array  a .
  //        n       long
  //                the order of the matrix  a .
  //        ipvt    long(n)
  //                the pivot vector from dgeco or dgefa.
  //        b       double precision(n)
  //                the right hand side vector.
  //        job     long
  //                = 0         to solve  a*x = b ,
  //                = nonzero   to solve  trans(a)*x = b  where
  //                            trans(a)  is the transpose.
  //     on return
  //        b       the solution vector  x .
  //     error condition
  //        a division by zero will occur if the input factor contains a
  //        zero on the diagonal.  technically this indicates singularity
  //        but it is often caused by improper arguments or improper
  //        setting of lda .  it will not occur if the subroutines are
  //        called correctly and if dgeco has set rcond .gt. 0.0
  //        or dgefa has set info .eq. 0 .
  //     to compute  inverse(a) * c  where  c  is a matrix
  //     with  p  columns
  //           call dgeco(a,lda,n,ipvt,rcond,z)
  //           if (rcond is too mysmall) go to ...
  //           do 10 j = 1, p
  //              call dgesl(a,lda,n,ipvt,c(1,j),0)
  //        10 continue
  //     linpack. this version dated 08/14/78 .
  //     cleve moler, university of new mexico, argonne national lab.
  //     subroutines and functions
  //     blas daxpy,ddot
  //     internal variables
  // Parameter adjustments
  --b;
  --ipvt;
  a_dim1 = *lda;
  a_offset = a_dim1 + 1;
  a -= a_offset;

  // Function Body
  nm1 = *n - 1;
  if(*job != 0)
    {
    goto L50;
    }

  //        job = 0 , solve  a * x = b
  //        first solve  l*y = b
  if(nm1 < 1)
    {
    goto L30;
    }

  i__1 = nm1;
  for(k = 1; k <= i__1; ++k)
    {
    l = ipvt[k];
    t = b[l];
    if(l == k)
      {
      goto L10;
      }

    b[l] = b[k];
    b[k] = t;
L10:
    i__2 = *n - k;
    daxpy_(&i__2, &t, &a[k + 1 + k * a_dim1], &c__1, &b[k + 1], &c__1);

    // L20:
    }

L30:
  //        now solve  u*x = y
  i__1 = *n;
  for(kb = 1; kb <= i__1; ++kb)
    {
    k = *n + 1 - kb;
    b[k] /= a[k + k * a_dim1];
    t = -b[k];
    i__2 = k - 1;
    daxpy_(&i__2, &t, &a[k * a_dim1 + 1], &c__1, &b[1], &c__1);

    // L40:
    }

  goto L100;
L50:
  //        job = nonzero, solve  trans(a) * x = b
  //        first solve  trans(u)*y = b
  i__1 = *n;
  for(k = 1; k <= i__1; ++k)
    {
    i__2 = k - 1;
    t = ddot_(&i__2, &a[k * a_dim1 + 1], &c__1, &b[1], &c__1);
    b[k] = (b[k] - t) / a[k + k * a_dim1];

    // L60:
    }

  //        now solve trans(l)*x = y
  if(nm1 < 1)
    {
    goto L90;
    }

  i__1 = nm1;
  for(kb = 1; kb <= i__1; ++kb)
    {
    k = *n - kb;
    i__2 = *n - k;
    b[k] += ddot_(&i__2, &a[k + 1 + k * a_dim1], &c__1, &b[k + 1], &c__1);
    l = ipvt[k];
    if(l == k)
      {
      goto L70;
      }

    t = b[l];
    b[l] = b[k];
    b[k] = t;
L70:
    // L80:
    ;
    }

L90:
  L100 : return 0;
  } // dgesl_

// Subroutine
int CLinPack::dgbfa_
                    (
                      double  *abd,
                      long     *lda,
                      long     *n,
                      long     *ml,
                      long     *mu,
                      long     *ipvt,
                      long     *info
                    )
  {
  // System generated locals
  long     abd_dim1, abd_offset, i__1, i__2, i__3, i__4;

  // Local variables
  long     i, j, k, l, m;
  double  t;
  long         i0, j0, j1, lm, mm, ju;
  long         jz, kp1, nm1;

  //     dgbfa factors a double precision band matrix by elimination.
  //     dgbfa is usually called by dgbco, but it can be called
  //     directly with a saving in time if  rcond  is not needed.
  //     on entry
  //        abd     double precision(lda, n)
  //                contains the matrix in band storage.  the columns
  //                of the matrix are stored in the columns of  abd  and
  //                the diagonals of the matrix are stored in rows
  //                ml+1 through 2*ml+mu+1 of  abd .
  //                see the comments below for details.
  //        lda     long
  //                the leading dimension of the array  abd .
  //                lda must be .ge. 2*ml + mu + 1 .
  //        n       long
  //                the order of the original matrix.
  //        ml      long
  //                number of diagonals below the main diagonal.
  //                0 .le. ml .lt. n .
  //        mu      long
  //                number of diagonals above the main diagonal.
  //                0 .le. mu .lt. n .
  //                more efficient if  ml .le. mu .
  //     on return
  //        abd     an upper triangular matrix in band storage and
  //                the multipliers which were used to obtain it.
  //                the factorization can be written  a = l*u  where
  //                l  is a product of permutation and unit lower
  //                triangular matrices and  u  is upper triangular.
  //        ipvt    long(n)
  //                an long vector of pivot indices.
  //        info    long
  //                = 0  normal value.
  //                = k  if  u(k,k) .eq. 0.0 .  this is not an error
  //                     condition for this subroutine, but it does
  //                     indicate that dgbsl will divide by zero if
  //                     called.  use  rcond  in dgbco for a reliable
  //                     indication of singularity.
  //     band storage
  //           if  a  is a band matrix, the following program segment
  //           will set up the input.
  //                   ml = (band width below the diagonal)
  //                   mu = (band width above the diagonal)
  //                   m = ml + mu + 1
  //                   do 20 j = 1, n
  //                      i1 = max0(1, j-mu)
  //                      i2 = min0(n, j+ml)
  //                      do 10 i = i1, i2
  //                         k = i - j + m
  //                         abd(k,j) = a(i,j)
  //                10    continue
  //                20 continue
  //           this uses rows  ml+1  through  2*ml+mu+1  of  abd .
  //           in addition, the first  ml  rows in  abd  are used for
  //           elements generated during the triangularization.
  //           the total number of rows needed in  abd  is  2*ml+mu+1 .
  //           the  ml+mu by ml+mu  upper left triangle and the
  //           ml by ml  lower right triangle are not referenced.
  //     linpack. this version dated 08/14/78 .
  //     cleve moler, university of new mexico, argonne national lab.
  //     subroutines and functions
  //     blas daxpy,dscal,idamax
  //     fortran max0,min0
  //     internal variables
  // Parameter adjustments
  --ipvt;
  abd_dim1 = *lda;
  abd_offset = abd_dim1 + 1;
  abd -= abd_offset;

  // Function Body
  m = *ml +*mu + 1;
  *info = 0;

  //     zero initial fill-in columns
  j0 = *mu + 2;
  j1 = min(*n, m) - 1;
  if(j1 < j0)
    {
    goto L30;
    }

  i__1 = j1;
  for(jz = j0; jz <= i__1; ++jz)
    {
    i0 = m + 1 - jz;
    i__2 = *ml;
    for(i = i0; i <= i__2; ++i)
      {
      abd[i + jz * abd_dim1] = 0.;

      // L10:
      }

    // L20:
    }

L30:
  jz = j1;
  ju = 0;

  //     gaussian elimination with partial pivoting
  nm1 = *n - 1;
  if(nm1 < 1)
    {
    goto L130;
    }

  i__1 = nm1;
  for(k = 1; k <= i__1; ++k)
    {
    kp1 = k + 1;

    //        zero next fill-in column
    ++jz;
    if(jz > *n)
      {
      goto L50;
      }

    if(*ml < 1)
      {
      goto L50;
      }

    i__2 = *ml;
    for(i = 1; i <= i__2; ++i)
      {
      abd[i + jz * abd_dim1] = 0.;

      // L40:
      }

L50:
    //        find l = pivot index
    // Computing MIN
    i__2 = *ml, i__3 = *n - k;
    lm = min(i__2, i__3);
    i__2 = lm + 1;
    l = idamax_(&i__2, &abd[m + k * abd_dim1], &c__1) + m - 1;
    ipvt[k] = l + k - m;

    //        zero pivot implies this column already triangularized
    if(abd[l + k * abd_dim1] == 0.)
      {
      goto L100;
      }

    //           interchange if necessary
    if(l == m)
      {
      goto L60;
      }

    t = abd[l + k * abd_dim1];
    abd[l + k * abd_dim1] = abd[m + k * abd_dim1];
    abd[m + k * abd_dim1] = t;
L60:
    //           compute multipliers
    t = -1. / abd[m + k * abd_dim1];
    dscal_(&lm, &t, &abd[m + 1 + k * abd_dim1], &c__1);

    //           row elimination with column indexing
    // Computing MIN
    // Computing MAX
    i__3 = ju, i__4 = *mu + ipvt[k];
    i__2 = max(i__3, i__4);
    ju = min(i__2, *n);
    mm = m;
    if(ju < kp1)
      {
      goto L90;
      }

    i__2 = ju;
    for(j = kp1; j <= i__2; ++j)
      {
      --l;
      --mm;
      t = abd[l + j * abd_dim1];
      if(l == mm)
        {
        goto L70;
        }

      abd[l + j * abd_dim1] = abd[mm + j * abd_dim1];
      abd[mm + j * abd_dim1] = t;
L70:
      daxpy_(&lm, &t, &abd[m + 1 + k * abd_dim1], &c__1, &abd[mm + 1 + j * abd_dim1], &c__1);

      // L80:
      }

L90:
    goto L110;
L100:
    *info = k;
L110:
    // L120:
    ;
    }

L130:
  ipvt[*n] = *n;
  if(abd[m +*n * abd_dim1] == 0.)
    {
    *info = *n;
    }

  return 0;
  } // dgbfa_

// Subroutine
int CLinPack::dgbsl_
                    (
                      double  *abd,
                      long     *lda,
                      long     *n,
                      long     *ml,
                      long     *mu,
                      long     *ipvt,
                      double  *b,
                      long     *job
                    )
  {
  // System generated locals
  long                   abd_dim1, abd_offset, i__1, i__2, i__3;

  // Local variables
  long                   k, l, m;
  double                t;
  long                   kb, la, lb, lm, nm1;

  //     dgbsl solves the double precision band system
  //     a * x = b  or  trans(a) * x = b
  //     using the factors computed by dgbco or dgbfa.
  //     on entry
  //        abd     double precision(lda, n)
  //                the output from dgbco or dgbfa.
  //        lda     long
  //                the leading dimension of the array  abd .
  //        n       long
  //                the order of the original matrix.
  //        ml      long
  //                number of diagonals below the main diagonal.
  //        mu      long
  //                number of diagonals above the main diagonal.
  //        ipvt    long(n)
  //                the pivot vector from dgbco or dgbfa.
  //        b       double precision(n)
  //                the right hand side vector.
  //        job     long
  //                = 0         to solve  a*x = b ,
  //                = nonzero   to solve  trans(a)*x = b , where
  //                            trans(a)  is the transpose.
  //     on return
  //        b       the solution vector  x .
  //     error condition
  //        a division by zero will occur if the input factor contains a
  //        zero on the diagonal.  technically this indicates singularity
  //        but it is often caused by improper arguments or improper
  //        setting of lda .  it will not occur if the subroutines are
  //        called correctly and if dgbco has set rcond .gt. 0.0
  //        or dgbfa has set info .eq. 0 .
  //     to compute  inverse(a) * c  where  c  is a matrix
  //     with  p  columns
  //           call dgbco(abd,lda,n,ml,mu,ipvt,rcond,z)
  //           if (rcond is too mysmall) go to ...
  //           do 10 j = 1, p
  //              call dgbsl(abd,lda,n,ml,mu,ipvt,c(1,j),0)
  //        10 continue
  //     linpack. this version dated 08/14/78 .
  //     cleve moler, university of new mexico, argonne national lab.
  //     subroutines and functions
  //     blas daxpy,ddot
  //     fortran min0
  //     internal variables
  // Parameter adjustments
  --b;
  --ipvt;
  abd_dim1 = *lda;
  abd_offset = abd_dim1 + 1;
  abd -= abd_offset;

  // Function Body
  m = *mu +*ml + 1;
  nm1 = *n - 1;
  if(*job != 0)
    {
    goto L50;
    }

  //        job = 0 , solve  a * x = b
  //        first solve l*y = b
  if(*ml == 0)
    {
    goto L30;
    }

  if(nm1 < 1)
    {
    goto L30;
    }

  i__1 = nm1;
  for(k = 1; k <= i__1; ++k)
    {
    // Computing MIN
    i__2 = *ml, i__3 = *n - k;
    lm = min(i__2, i__3);
    l = ipvt[k];
    t = b[l];
    if(l == k)
      {
      goto L10;
      }

    b[l] = b[k];
    b[k] = t;
L10:
    daxpy_(&lm, &t, &abd[m + 1 + k * abd_dim1], &c__1, &b[k + 1], &c__1);

    // L20:
    }

L30:
  //        now solve  u*x = y
  i__1 = *n;
  for(kb = 1; kb <= i__1; ++kb)
    {
    k = *n + 1 - kb;
    b[k] /= abd[m + k * abd_dim1];
    lm = min(k, m) - 1;
    la = m - lm;
    lb = k - lm;
    t = -b[k];
    daxpy_(&lm, &t, &abd[la + k * abd_dim1], &c__1, &b[lb], &c__1);

    // L40:
    }

  goto L100;
L50:
  //        job = nonzero, solve  trans(a) * x = b
  //        first solve  trans(u)*y = b
  i__1 = *n;
  for(k = 1; k <= i__1; ++k)
    {
    lm = min(k, m) - 1;
    la = m - lm;
    lb = k - lm;
    t = ddot_(&lm, &abd[la + k * abd_dim1], &c__1, &b[lb], &c__1);
    b[k] = (b[k] - t) / abd[m + k * abd_dim1];

    // L60:
    }

  //        now solve trans(l)*x = y
  if(*ml == 0)
    {
    goto L90;
    }

  if(nm1 < 1)
    {
    goto L90;
    }

  i__1 = nm1;
  for(kb = 1; kb <= i__1; ++kb)
    {
    k = *n - kb;

    // Computing MIN
    i__2 = *ml, i__3 = *n - k;
    lm = min(i__2, i__3);
    b[k] += ddot_(&lm, &abd[m + 1 + k * abd_dim1], &c__1, &b[k + 1], &c__1);
    l = ipvt[k];
    if(l == k)
      {
      goto L70;
      }

    t = b[l];
    b[l] = b[k];
    b[k] = t;
L70:
    // L80:
    ;
    }

L90:
  L100 : return 0;
  } // dgbsl_

// Subroutine
int CLinPack::daxpy_(long *n, double *da, double *dx, long *incx, double *dy, long *incy)
  {
  // System generated locals
  long i__1;

  // Local variables
  long i, m, ix, iy, mp1;

  //     constant times a vector plus a vector.
  //     uses unrolled loops for increments equal to one.
  //     jack dongarra, linpack, 3/11/78.
  //     modified 12/3/93, array(1) declarations changed to array(*)
  // Parameter adjustments
  --dy;
  --dx;

  // Function Body
  if(*n <= 0)
    {
    return 0;
    }

  if(*da == 0.)
    {
    return 0;
    }

  if(*incx == 1 && *incy == 1)
    {
    goto L20;
    }

  //        code for unequal increments or equal increments
  //          not equal to 1
  ix = 1;
  iy = 1;
  if(*incx < 0)
    {
    ix = (-(*n) + 1) **incx + 1;
    }

  if(*incy < 0)
    {
    iy = (-(*n) + 1) **incy + 1;
    }

  i__1 = *n;
  for(i = 1; i <= i__1; ++i)
    {
    dy[iy] += *da * dx[ix];
    ix += *incx;
    iy += *incy;

    // L10:
    }

  return 0;

  //        code for both increments equal to 1
  //        clean-up loop
L20:
  m = *n % 4;
  if(m == 0)
    {
    goto L40;
    }

  i__1 = m;
  for(i = 1; i <= i__1; ++i)
    {
    dy[i] += *da * dx[i];

    // L30:
    }

  if(*n < 4)
    {
    return 0;
    }

L40:
  mp1 = m + 1;
  i__1 = *n;
  for(i = mp1; i <= i__1; i += 4)
    {
    dy[i] += *da * dx[i];
    dy[i + 1] += *da * dx[i + 1];
    dy[i + 2] += *da * dx[i + 2];
    dy[i + 3] += *da * dx[i + 3];

    // L50:
    }

  return 0;
  } // daxpy_

// Subroutine
int CLinPack::dscal_(long *n, double *da, double *dx, long *incx)
  {
  // System generated locals
  long i__1, i__2;

  // Local variables
  long i, m, nincx, mp1;

  //     scales a vector by a constant.
  //     uses unrolled loops for increment equal to one.
  //     jack dongarra, linpack, 3/11/78.
  //     modified 3/93 to return if incx .le. 0.
  //     modified 12/3/93, array(1) declarations changed to array(*)
  // Parameter adjustments
  --dx;

  // Function Body
  if(*n <= 0 || *incx <= 0)
    {
    return 0;
    }

  if(*incx == 1)
    {
    goto L20;
    }

  //        code for increment not equal to 1
  nincx = *n **incx;
  i__1 = nincx;
  i__2 = *incx;
  for(i = 1; i__2 < 0 ? i >= i__1 : i <= i__1; i += i__2)
    {
    dx[i] = *da * dx[i];

    // L10:
    }

  return 0;

  //        code for increment equal to 1
  //        clean-up loop
L20:
  m = *n % 5;
  if(m == 0)
    {
    goto L40;
    }

  i__2 = m;
  for(i = 1; i <= i__2; ++i)
    {
    dx[i] = *da * dx[i];

    // L30:
    }

  if(*n < 5)
    {
    return 0;
    }

L40:
  mp1 = m + 1;
  i__2 = *n;
  for(i = mp1; i <= i__2; i += 5)
    {
    dx[i] = *da * dx[i];
    dx[i + 1] = *da * dx[i + 1];
    dx[i + 2] = *da * dx[i + 2];
    dx[i + 3] = *da * dx[i + 3];
    dx[i + 4] = *da * dx[i + 4];

    // L50:
    }

  return 0;
  } // dscal_

long CLinPack::idamax_(long *n, double *dx, long *incx)
  {
  // System generated locals
  long     ret_val, i__1;
  double  d__1;

  // Local variables
  double  dmax_;
  long     i, ix;

  //     finds the index of element having max. absolute value.
  //     jack dongarra, linpack, 3/11/78.
  //     modified 3/93 to return if incx .le. 0.
  //     modified 12/3/93, array(1) declarations changed to array(*)
  // Parameter adjustments
  --dx;

  // Function Body
  ret_val = 0;
  if(*n < 1 || *incx <= 0)
    {
    return ret_val;
    }

  ret_val = 1;
  if(*n == 1)
    {
    return ret_val;
    }

  if(*incx == 1)
    {
    goto L20;
    }

  //        code for increment not equal to 1
  ix = 1;
  dmax_ = abs(dx[1]);
  ix += *incx;
  i__1 = *n;
  for(i = 2; i <= i__1; ++i)
    {
    if((d__1 = dx[ix], abs(d__1)) <= dmax_)
      {
      goto L5;
      }

    ret_val = i;
    dmax_ = (d__1 = dx[ix], abs(d__1));
L5:
    ix += *incx;

    // L10:
    }

  return ret_val;

  //        code for increment equal to 1
L20:
  dmax_ = abs(dx[1]);
  i__1 = *n;
  for(i = 2; i <= i__1; ++i)
    {
    if((d__1 = dx[i], abs(d__1)) <= dmax_)
      {
      goto L30;
      }

    ret_val = i;
    dmax_ = (d__1 = dx[i], abs(d__1));
L30:
    ;
    }

  return ret_val;
  } // idamax_

double CLinPack::ddot_(long *n, double *dx, long *incx, double *dy, long *incy)
  {
  // System generated locals
  long     i__1;
  double  ret_val;

  // Local variables
  long     i, m;
  double  dtemp;
  long     ix, iy, mp1;

  //     forms the dot product of two vectors.
  //     uses unrolled loops for increments equal to one.
  //     jack dongarra, linpack, 3/11/78.
  //     modified 12/3/93, array(1) declarations changed to array(*)
  // Parameter adjustments
  --dy;
  --dx;

  // Function Body
  ret_val = 0.;
  dtemp = 0.;
  if(*n <= 0)
    {
    return ret_val;
    }

  if(*incx == 1 && *incy == 1)
    {
    goto L20;
    }

  //        code for unequal increments or equal increments
  //          not equal to 1
  ix = 1;
  iy = 1;
  if(*incx < 0)
    {
    ix = (-(*n) + 1) **incx + 1;
    }

  if(*incy < 0)
    {
    iy = (-(*n) + 1) **incy + 1;
    }

  i__1 = *n;
  for(i = 1; i <= i__1; ++i)
    {
    dtemp += dx[ix] * dy[iy];
    ix += *incx;
    iy += *incy;

    // L10:
    }

  ret_val = dtemp;
  return ret_val;

  //        code for both increments equal to 1
  //        clean-up loop
L20:
  m = *n % 5;
  if(m == 0)
    {
    goto L40;
    }

  i__1 = m;
  for(i = 1; i <= i__1; ++i)
    {
    dtemp += dx[i] * dy[i];

    // L30:
    }

  if(*n < 5)
    {
    goto L60;
    }

L40:
  mp1 = m + 1;
  i__1 = *n;
  for(i = mp1; i <= i__1; i += 5)
    {
    dtemp = dtemp +
      dx[i] *
      dy[i] +
      dx[i + 1] *
      dy[i + 1] +
      dx[i + 2] *
      dy[i + 2] +
      dx[i + 3] *
      dy[i + 3] +
      dx[i + 4] *
      dy[i + 4];

    // L50:
    }

L60:
  ret_val = dtemp;
  return ret_val;
  } // ddot_

double CLinPack::d1mach_(long *i)
  {
  // Format strings
//Org   static char       fmt_9000[] = "(/\002 Adjust D1MACH by uncommenting data stat\
//Org ements\002/\002 appropriate for your machine.\002)";

  // System generated locals
  double        ret_val;
//  /*static*/ double equiv_4[5];
//#pragma message("These 2 STATIC's should be restored and a CRITICALSECTION implemented ")

  // Local variables
#define log10 ((long *) equiv_4 + 8)
#define dmach (equiv_4)
#define large ((long *) equiv_4 + 2)
#define mysmall ((long *) equiv_4)
#define diver ((long *) equiv_4 + 6)
#define right ((long *) equiv_4 + 4)
//  /*static*/ long  sc=0;

  // Fortran I/O blocks

  //  DOUBLE-PRECISION MACHINE CONSTANTS
  //  D1MACH( 1) = B**(EMIN-1), THE SMALLEST POSITIVE MAGNITUDE.
  //  D1MACH( 2) = B**EMAX*(1 - B**(-T)), THE LARGEST MAGNITUDE.
  //  D1MACH( 3) = B**(-T), THE SMALLEST RELATIVE SPACING.
  //  D1MACH( 4) = B**(1-T), THE LARGEST RELATIVE SPACING.
  //  D1MACH( 5) = LOG10(B)
  //  TO ALTER THIS FUNCTION FOR A PARTICULAR ENVIRONMENT,
  //  THE DESIRED SET OF DATA STATEMENTS SHOULD BE ACTIVATED BY
  //  REMOVING THE C FROM COLUMN 1.
  //  ON RARE MACHINES A STATIC STATEMENT MAY NEED TO BE ADDED.
  //  (BUT PROBABLY MORE SYSTEMS PROHIBIT IT THAN REQUIRE IT.)
  //  FOR IEEE-ARITHMETIC MACHINES (BINARY STANDARD), ONE OF THE FIRST
  //  TWO SETS OF CONSTANTS BELOW SHOULD BE APPROPRIATE.  IF YOU DO NOT
  //  KNOW WHICH SET TO USE, TRY BOTH AND SEE WHICH GIVES PLAUSIBLE
  //  VALUES.
  //  WHERE POSSIBLE, DECIMAL, OCTAL OR HEXADECIMAL CONSTANTS ARE USED
  //  TO SPECIFY THE CONSTANTS EXACTLY.  SOMETIMES THIS REQUIRES USING
  //  EQUIVALENT INTEGER ARRAYS.  IF YOUR COMPILER USES HALF-WORD
  //  INTEGERS BY DEFAULT (SOMETIMES CALLED INTEGER*2), YOU MAY NEED TO
  //  CHANGE INTEGER TO INTEGER*4 OR OTHERWISE INSTRUCT YOUR COMPILER
  //  TO USE FULL-WORD INTEGERS IN THE NEXT 5 DECLARATIONS.
  //  COMMENTS JUST BEFORE THE END STATEMENT (LINES STARTING WITH *)
  //  GIVE C SOURCE FOR D1MACH.
  // /6S
  // /7S
  // /
  //     MACHINE CONSTANTS FOR BIG-ENDIAN IEEE ARITHMETIC (BINARY FORMAT)
  //     MACHINES IN WHICH THE MOST SIGNIFICANT BYTE IS STORED FIRST,
  //     SUCH AS THE AT&T 3B SERIES, MOTOROLA 68000 BASED MACHINES (E.G.
  //     SUN 3), AND MACHINES THAT USE SPARC, HP, OR IBM RISC CHIPS.
  //      DATA SMALL(1),SMALL(2) /    1048576,          0 /
  //      DATA LARGE(1),LARGE(2) / 2146435071,         -1 /
  //      DATA RIGHT(1),RIGHT(2) / 1017118720,          0 /
  //      DATA DIVER(1),DIVER(2) / 1018167296,          0 /
  //      DATA LOG10(1),LOG10(2) / 1070810131, 1352628735 /, SC/987/
  //     MACHINE CONSTANTS FOR LITTLE-ENDIAN (BINARY) IEEE ARITHMETIC
  //     MACHINES IN WHICH THE LEAST SIGNIFICANT BYTE IS STORED FIRST,
  //     E.G. IBM PCS AND OTHER MACHINES THAT USE INTEL 80X87 OR DEC
  //     ALPHA CHIPS.
  //      DATA SMALL(1),SMALL(2) /          0,    1048576 /
  //      DATA LARGE(1),LARGE(2) /         -1, 2146435071 /
  //      DATA RIGHT(1),RIGHT(2) /          0, 1017118720 /
  //      DATA DIVER(1),DIVER(2) /          0, 1018167296 /
  //      DATA LOG10(1),LOG10(2) / 1352628735, 1070810131 /, SC/987/
  //     MACHINE CONSTANTS FOR AMDAHL MACHINES.
  //      DATA SMALL(1),SMALL(2) /    1048576,          0 /
  //      DATA LARGE(1),LARGE(2) / 2147483647,         -1 /
  //      DATA RIGHT(1),RIGHT(2) /  856686592,          0 /
  //      DATA DIVER(1),DIVER(2) /  873463808,          0 /
  //      DATA LOG10(1),LOG10(2) / 1091781651, 1352628735 /, SC/987/
  //     MACHINE CONSTANTS FOR THE BURROUGHS 1700 SYSTEM.
  //      DATA SMALL(1) / ZC00800000 /
  //      DATA SMALL(2) / Z000000000 /
  //      DATA LARGE(1) / ZDFFFFFFFF /
  //      DATA LARGE(2) / ZFFFFFFFFF /
  //      DATA RIGHT(1) / ZCC5800000 /
  //      DATA RIGHT(2) / Z000000000 /
  //      DATA DIVER(1) / ZCC6800000 /
  //      DATA DIVER(2) / Z000000000 /
  //      DATA LOG10(1) / ZD00E730E7 /
  //      DATA LOG10(2) / ZC77800DC0 /, SC/987/
  //     MACHINE CONSTANTS FOR THE BURROUGHS 5700 SYSTEM.
  //      DATA SMALL(1) / O1771000000000000 /
  //      DATA SMALL(2) / O0000000000000000 /
  //      DATA LARGE(1) / O0777777777777777 /
  //      DATA LARGE(2) / O0007777777777777 /
  //      DATA RIGHT(1) / O1461000000000000 /
  //      DATA RIGHT(2) / O0000000000000000 /
  //      DATA DIVER(1) / O1451000000000000 /
  //      DATA DIVER(2) / O0000000000000000 /
  //      DATA LOG10(1) / O1157163034761674 /
  //      DATA LOG10(2) / O0006677466732724 /, SC/987/
  //     MACHINE CONSTANTS FOR THE BURROUGHS 6700/7700 SYSTEMS.
  //      DATA SMALL(1) / O1771000000000000 /
  //      DATA SMALL(2) / O7770000000000000 /
  //      DATA LARGE(1) / O0777777777777777 /
  //      DATA LARGE(2) / O7777777777777777 /
  //      DATA RIGHT(1) / O1461000000000000 /
  //      DATA RIGHT(2) / O0000000000000000 /
  //      DATA DIVER(1) / O1451000000000000 /
  //      DATA DIVER(2) / O0000000000000000 /
  //      DATA LOG10(1) / O1157163034761674 /
  //      DATA LOG10(2) / O0006677466732724 /, SC/987/
  //     MACHINE CONSTANTS FOR FTN4 ON THE CDC 6000/7000 SERIES.
  //      DATA SMALL(1) / 00564000000000000000B /
  //      DATA SMALL(2) / 00000000000000000000B /
  //      DATA LARGE(1) / 37757777777777777777B /
  //      DATA LARGE(2) / 37157777777777777774B /
  //      DATA RIGHT(1) / 15624000000000000000B /
  //      DATA RIGHT(2) / 00000000000000000000B /
  //      DATA DIVER(1) / 15634000000000000000B /
  //      DATA DIVER(2) / 00000000000000000000B /
  //      DATA LOG10(1) / 17164642023241175717B /
  //      DATA LOG10(2) / 16367571421742254654B /, SC/987/
  //     MACHINE CONSTANTS FOR FTN5 ON THE CDC 6000/7000 SERIES.
  //      DATA SMALL(1) / O"00564000000000000000" /
  //      DATA SMALL(2) / O"00000000000000000000" /
  //      DATA LARGE(1) / O"37757777777777777777" /
  //      DATA LARGE(2) / O"37157777777777777774" /
  //      DATA RIGHT(1) / O"15624000000000000000" /
  //      DATA RIGHT(2) / O"00000000000000000000" /
  //      DATA DIVER(1) / O"15634000000000000000" /
  //      DATA DIVER(2) / O"00000000000000000000" /
  //      DATA LOG10(1) / O"17164642023241175717" /
  //      DATA LOG10(2) / O"16367571421742254654" /, SC/987/
  //     MACHINE CONSTANTS FOR CONVEX C-1
  //      DATA SMALL(1),SMALL(2) / '00100000'X, '00000000'X /
  //      DATA LARGE(1),LARGE(2) / '7FFFFFFF'X, 'FFFFFFFF'X /
  //      DATA RIGHT(1),RIGHT(2) / '3CC00000'X, '00000000'X /
  //      DATA DIVER(1),DIVER(2) / '3CD00000'X, '00000000'X /
  //      DATA LOG10(1),LOG10(2) / '3FF34413'X, '509F79FF'X /, SC/987/
  //     MACHINE CONSTANTS FOR THE CRAY 1, XMP, 2, AND 3.
  //      DATA SMALL(1) / 201354000000000000000B /
  //      DATA SMALL(2) / 000000000000000000000B /
  //      DATA LARGE(1) / 577767777777777777777B /
  //      DATA LARGE(2) / 000007777777777777776B /
  //      DATA RIGHT(1) / 376434000000000000000B /
  //      DATA RIGHT(2) / 000000000000000000000B /
  //      DATA DIVER(1) / 376444000000000000000B /
  //      DATA DIVER(2) / 000000000000000000000B /
  //      DATA LOG10(1) / 377774642023241175717B /
  //      DATA LOG10(2) / 000007571421742254654B /, SC/987/
  //     MACHINE CONSTANTS FOR THE DATA GENERAL ECLIPSE S/200
  //     SMALL, LARGE, RIGHT, DIVER, LOG10 SHOULD BE DECLARED
  //     INTEGER SMALL(4), LARGE(4), RIGHT(4), DIVER(4), LOG10(4)
  //     NOTE - IT MAY BE APPROPRIATE TO INCLUDE THE FOLLOWING LINE -
  //     STATIC DMACH(5)
  //      DATA SMALL/20K,3*0/,LARGE/77777K,3*177777K/
  //      DATA RIGHT/31420K,3*0/,DIVER/32020K,3*0/
  //      DATA LOG10/40423K,42023K,50237K,74776K/, SC/987/
  //     MACHINE CONSTANTS FOR THE HARRIS SLASH 6 AND SLASH 7
  //      DATA SMALL(1),SMALL(2) / '20000000, '00000201 /
  //      DATA LARGE(1),LARGE(2) / '37777777, '37777577 /
  //      DATA RIGHT(1),RIGHT(2) / '20000000, '00000333 /
  //      DATA DIVER(1),DIVER(2) / '20000000, '00000334 /
  //      DATA LOG10(1),LOG10(2) / '23210115, '10237777 /, SC/987/
  //     MACHINE CONSTANTS FOR THE HONEYWELL DPS 8/70 SERIES.
  //      DATA SMALL(1),SMALL(2) / O402400000000, O000000000000 /
  //      DATA LARGE(1),LARGE(2) / O376777777777, O777777777777 /
  //      DATA RIGHT(1),RIGHT(2) / O604400000000, O000000000000 /
  //      DATA DIVER(1),DIVER(2) / O606400000000, O000000000000 /
  //      DATA LOG10(1),LOG10(2) / O776464202324, O117571775714 /, SC/987/
  //
  //     MACHINE CONSTANTS FOR THE IBM 360/370 SERIES,
  //     THE XEROX SIGMA 5/7/9 AND THE SEL SYSTEMS 85/86.
  //      DATA SMALL(1),SMALL(2) / Z00100000, Z00000000 /
  //      DATA LARGE(1),LARGE(2) / Z7FFFFFFF, ZFFFFFFFF /
  //      DATA RIGHT(1),RIGHT(2) / Z33100000, Z00000000 /
  //      DATA DIVER(1),DIVER(2) / Z34100000, Z00000000 /
  //      DATA LOG10(1),LOG10(2) / Z41134413, Z509F79FF /, SC/987/
  //     MACHINE CONSTANTS FOR THE INTERDATA 8/32
  //     WITH THE UNIX SYSTEM FORTRAN 77 COMPILER.
  //     FOR THE INTERDATA FORTRAN VII COMPILER REPLACE
  //     THE Z'S SPECIFYING HEX CONSTANTS WITH Y'S.
  //      DATA SMALL(1),SMALL(2) / Z'00100000', Z'00000000' /
  //      DATA LARGE(1),LARGE(2) / Z'7EFFFFFF', Z'FFFFFFFF' /
  //      DATA RIGHT(1),RIGHT(2) / Z'33100000', Z'00000000' /
  //      DATA DIVER(1),DIVER(2) / Z'34100000', Z'00000000' /
  //      DATA LOG10(1),LOG10(2) / Z'41134413', Z'509F79FF' /, SC/987/
  //     MACHINE CONSTANTS FOR THE PDP-10 (KA PROCESSOR).
  //      DATA SMALL(1),SMALL(2) / "033400000000, "000000000000 /
  //      DATA LARGE(1),LARGE(2) / "377777777777, "344777777777 /
  //      DATA RIGHT(1),RIGHT(2) / "113400000000, "000000000000 /
  //      DATA DIVER(1),DIVER(2) / "114400000000, "000000000000 /
  //      DATA LOG10(1),LOG10(2) / "177464202324, "144117571776 /, SC/987/
  //
  //     MACHINE CONSTANTS FOR THE PDP-10 (KI PROCESSOR).
  //      DATA SMALL(1),SMALL(2) / "000400000000, "000000000000 /
  //      DATA LARGE(1),LARGE(2) / "377777777777, "377777777777 /
  //      DATA RIGHT(1),RIGHT(2) / "103400000000, "000000000000 /
  //      DATA DIVER(1),DIVER(2) / "104400000000, "000000000000 /
  //      DATA LOG10(1),LOG10(2) / "177464202324, "047674776746 /, SC/987/
  //
  //     MACHINE CONSTANTS FOR PDP-11 FORTRANS SUPPORTING
  //     32-BIT INTEGERS (EXPRESSED IN INTEGER AND OCTAL).
  //      DATA SMALL(1),SMALL(2) /    8388608,           0 /
  //      DATA LARGE(1),LARGE(2) / 2147483647,          -1 /
  //      DATA RIGHT(1),RIGHT(2) /  612368384,           0 /
  //      DATA DIVER(1),DIVER(2) /  620756992,           0 /
  //      DATA LOG10(1),LOG10(2) / 1067065498, -2063872008 /, SC/987/
  //      DATA SMALL(1),SMALL(2) / O00040000000, O00000000000 /
  //      DATA LARGE(1),LARGE(2) / O17777777777, O37777777777 /
  //      DATA RIGHT(1),RIGHT(2) / O04440000000, O00000000000 /
  //      DATA DIVER(1),DIVER(2) / O04500000000, O00000000000 /
  //      DATA LOG10(1),LOG10(2) / O07746420232, O20476747770 /, SC/987/
  //     MACHINE CONSTANTS FOR PDP-11 FORTRANS SUPPORTING
  //     16-BIT INTEGERS (EXPRESSED IN INTEGER AND OCTAL).
  //     SMALL, LARGE, RIGHT, DIVER, LOG10 SHOULD BE DECLARED
  //     INTEGER SMALL(4), LARGE(4), RIGHT(4), DIVER(4), LOG10(4)
  //      DATA SMALL(1),SMALL(2) /    128,      0 /
  //      DATA SMALL(3),SMALL(4) /      0,      0 /
  //      DATA LARGE(1),LARGE(2) /  32767,     -1 /
  //      DATA LARGE(3),LARGE(4) /     -1,     -1 /
  //      DATA RIGHT(1),RIGHT(2) /   9344,      0 /
  //      DATA RIGHT(3),RIGHT(4) /      0,      0 /
  //      DATA DIVER(1),DIVER(2) /   9472,      0 /
  //      DATA DIVER(3),DIVER(4) /      0,      0 /
  //      DATA LOG10(1),LOG10(2) /  16282,   8346 /
  //      DATA LOG10(3),LOG10(4) / -31493, -12296 /, SC/987/
  //      DATA SMALL(1),SMALL(2) / O000200, O000000 /
  //      DATA SMALL(3),SMALL(4) / O000000, O000000 /
  //      DATA LARGE(1),LARGE(2) / O077777, O177777 /
  //      DATA LARGE(3),LARGE(4) / O177777, O177777 /
  //      DATA RIGHT(1),RIGHT(2) / O022200, O000000 /
  //      DATA RIGHT(3),RIGHT(4) / O000000, O000000 /
  //      DATA DIVER(1),DIVER(2) / O022400, O000000 /
  //      DATA DIVER(3),DIVER(4) / O000000, O000000 /
  //      DATA LOG10(1),LOG10(2) / O037632, O020232 /
  //      DATA LOG10(3),LOG10(4) / O102373, O147770 /, SC/987/
  //     MACHINE CONSTANTS FOR THE PRIME 50 SERIES SYSTEMS
  //     WITH 32-BIT INTEGERS AND 64V MODE INSTRUCTIONS,
  //     SUPPLIED BY IGOR BRAY.
  //      DATA SMALL(1),SMALL(2) / :10000000000, :00000100001 /
  //      DATA LARGE(1),LARGE(2) / :17777777777, :37777677775 /
  //      DATA RIGHT(1),RIGHT(2) / :10000000000, :00000000122 /
  //      DATA DIVER(1),DIVER(2) / :10000000000, :00000000123 /
  //      DATA LOG10(1),LOG10(2) / :11504046501, :07674600177 /, SC/987/
  //     MACHINE CONSTANTS FOR THE SEQUENT BALANCE 8000
  //      DATA SMALL(1),SMALL(2) / $00000000,  $00100000 /
  //      DATA LARGE(1),LARGE(2) / $FFFFFFFF,  $7FEFFFFF /
  //      DATA RIGHT(1),RIGHT(2) / $00000000,  $3CA00000 /
  //      DATA DIVER(1),DIVER(2) / $00000000,  $3CB00000 /
  //      DATA LOG10(1),LOG10(2) / $509F79FF,  $3FD34413 /, SC/987/
  //     MACHINE CONSTANTS FOR THE UNIVAC 1100 SERIES.
  //      DATA SMALL(1),SMALL(2) / O000040000000, O000000000000 /
  //      DATA LARGE(1),LARGE(2) / O377777777777, O777777777777 /
  //      DATA RIGHT(1),RIGHT(2) / O170540000000, O000000000000 /
  //      DATA DIVER(1),DIVER(2) / O170640000000, O000000000000 /
  //      DATA LOG10(1),LOG10(2) / O177746420232, O411757177572 /, SC/987/
  //
  //     MACHINE CONSTANTS FOR THE VAX UNIX F77 COMPILER
  //      DATA SMALL(1),SMALL(2) /        128,           0 /
  //      DATA LARGE(1),LARGE(2) /     -32769,          -1 /
  //      DATA RIGHT(1),RIGHT(2) /       9344,           0 /
  //      DATA DIVER(1),DIVER(2) /       9472,           0 /
  //      DATA LOG10(1),LOG10(2) /  546979738,  -805796613 /, SC/987/
  //     MACHINE CONSTANTS FOR THE VAX-11 WITH
  //     FORTRAN IV-PLUS COMPILER
  //      DATA SMALL(1),SMALL(2) / Z00000080, Z00000000 /
  //      DATA LARGE(1),LARGE(2) / ZFFFF7FFF, ZFFFFFFFF /
  //      DATA RIGHT(1),RIGHT(2) / Z00002480, Z00000000 /
  //      DATA DIVER(1),DIVER(2) / Z00002500, Z00000000 /
  //      DATA LOG10(1),LOG10(2) / Z209A3F9A, ZCFF884FB /, SC/987/
  //     MACHINE CONSTANTS FOR VAX/VMS VERSION 2.2
  //      DATA SMALL(1),SMALL(2) /       '80'X,        '0'X /
  //      DATA LARGE(1),LARGE(2) / 'FFFF7FFF'X, 'FFFFFFFF'X /
  //      DATA RIGHT(1),RIGHT(2) /     '2480'X,        '0'X /
  //      DATA DIVER(1),DIVER(2) /     '2500'X,        '0'X /
  //      DATA LOG10(1),LOG10(2) / '209A3F9A'X, 'CFF884FB'X /, SC/987/
  //  ***  ISSUE STOP 779 IF ALL DATA STATEMENTS ARE COMMENTED...
  if(sc != 987)
    {
    dmach[0] = 1e13;
    if(mysmall[0] == 1117925532 && mysmall[1] == -448790528)
      {
      //           *** IEEE BIG ENDIAN ***
      mysmall[0] = 1048576;
      mysmall[1] = 0;
      large[0] = 2146435071;
      large[1] = -1;
      right[0] = 1017118720;
      right[1] = 0;
      diver[0] = 1018167296;
      diver[1] = 0;
      log10[0] = 1070810131;
      log10[1] = 1352628735;
      }
    else if(mysmall[1] == 1117925532 && mysmall[0] == -448790528)
      {
      //           *** IEEE LITTLE ENDIAN ***
      mysmall[1] = 1048576;
      mysmall[0] = 0;
      large[1] = 2146435071;
      large[0] = -1;
      right[1] = 1017118720;
      right[0] = 0;
      diver[1] = 1018167296;
      diver[0] = 0;
      log10[1] = 1070810131;
      log10[0] = 1352628735;
      }
    else if(mysmall[0] == -2065213935 && mysmall[1] == 10752)
      {
      //               *** VAX WITH D_FLOATING ***
      mysmall[0] = 128;
      mysmall[1] = 0;
      large[0] = -32769;
      large[1] = -1;
      right[0] = 9344;
      right[1] = 0;
      diver[0] = 9472;
      diver[1] = 0;
      log10[0] = 546979738;
      log10[1] = -805796613;
      }
    else if(mysmall[0] == 1267827943 && mysmall[1] == 704643072)
      {
      //               *** IBM MAINFRAME ***
      mysmall[0] = 1048576;
      mysmall[1] = 0;
      large[0] = 2147483647;
      large[1] = -1;
      right[0] = 856686592;
      right[1] = 0;
      diver[0] = 873463808;
      diver[1] = 0;
      log10[0] = 1091781651;
      log10[1] = 1352628735;
      }
    else
      {
      throw new CScdException(0,"LinPack 779");
      }

    sc = 987;
    }

  //  ***  ISSUE STOP 778 IF ALL DATA STATEMENTS ARE OBVIOUSLY WRONG...
  if(dmach[3] >= 1.)
    throw new CScdException(0,"LinPack 778");

  // C/6S
  // C     IF (I .LT. 1  .OR.  I .GT. 5)
  // C    1   CALL SETERR(24HD1MACH - I OUT OF BOUNDS,24,1,2)
  // C/7S
  //      IF (I .LT. 1  .OR.  I .GT. 5)
  //     1   CALL SETERR('D1MACH - I OUT OF BOUNDS',24,1,2)
  // C/
  if(*i < 1 || *i > 5)
    {
    char buff[1024];
    sprintf(buff, "LinPack : D1MACH(I): I =%i  is out of bounds.", i);
    throw new CScdException(0,buff);
    }

  ret_val = dmach[*i - 1];
  return ret_val;

  // /+ C source for D1MACH -- remove the * in column 1 +/
  // #include <stdio.h>
  // #include <float.h>
  // #include <math.h>
  // double d1mach_(long *i)
  // {
  // 	switch(*i){
  // 	  case 1: return DBL_MIN;
  // 	  case 2: return DBL_MAX;
  // 	  case 3: return DBL_EPSILON/FLT_RADIX;
  // 	  case 4: return DBL_EPSILON;
  // 	  case 5: return log10(FLT_RADIX);
  // 	  }
  // 	fprintf(stderr, "invalid argument: d1mach(%ld)\n", *i);
  // 	exit(1);
  // 	return 0; /+ for compilers that complain of missing return values +/
  //
  // 	}
  } // d1mach_

#undef right
#undef diver
#undef mysmall
#undef large
#undef dmach
#undef log10
