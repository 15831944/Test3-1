#pragma once


class CLinPack
  {
  protected:
    // for d1mach
    double equiv_4[5];
    long  sc;

  public:

    CLinPack()
      {
      equiv_4[0]=0;
      equiv_4[1]=0;
      equiv_4[2]=0;
      equiv_4[3]=0;
      equiv_4[4]=0;
      sc=0;
      };


     int dgefa_(double *a, long *lda, long *n, long *ipvt, long *info);
     int dgesl_(double *a, long *lda, long *n, long *ipvt, double *b, long *job);
     int dgbfa_(double *abd, long *lda, long *n, long *ml, long *mu, long *ipvt, long *info);
     int dgbsl_(double *abd, long *lda, long *n, long *ml, long *mu, long *ipvt, double *b, long *job);
     int daxpy_(long *n, double *da, double *dx, long *incx, double *dy, long *incy);
     int dscal_(long *n, double *da, double *dx, long *incx);
     long idamax_(long *n, double *dx, long *incx);
     double ddot_(long *n, double *dx, long *incx, double *dy, long *incy);
     double d1mach_(long *i);
  };
