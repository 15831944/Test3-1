
#pragma once
#include "linpack.h"


class CLSoda : public CLinPack
  {
  protected:

    union
      {
      struct
        {
        double  rowns[209], ccmax, el0, h, hmin, hmxi, hu, rc, tn, uround;
        long     illin, init, lyh, lewt, lacor, lsavf, lwm, liwm, mxstep, mxhnil, nhnil, ntrep, nslast, nyh, iowns[6],
          icf, ierpj, iersl, jcur, jstart, kflag, l, meth, miter, maxord, maxcor, msbp, mxncf, n, nq, nst, nfe, nje, nqu;
        } _1;
      struct
        {
        double  rowns[209], ccmax, el0, h, hmin, hmxi, hu, rc, tn, uround;
        long     iownd[14], iowns[6], icf, ierpj, iersl, jcur, jstart, kflag, l, meth, miter, maxord, maxcor, msbp,
          mxncf, n, nq, nst, nfe, nje, nqu;
        } _2;
      struct
        {
        double  conit, crate, el[13], elco[156] /* was [13][12] */, hold, rmax, tesco[36] /* was [3][12] */, ccmax, el0,
          h, hmin, hmxi, hu, rc, tn, uround;
        long     iownd[14], ialth, ipup, lmax, meo, nqnyh, nslp, icf, ierpj, iersl, jcur, jstart, kflag, l, meth, miter,
          maxord, maxcor, msbp, mxncf, n, nq, nst, nfe, nje, nqu;
        } _3;
      } ls0001_;

    union
      {
      struct
        {
        double  tsw, rowns2[20], pdnorm;
        long     insufr, insufi, ixpr, iowns2[2], jtyp, mused, mxordn, mxords;
        } _1;
      struct
        {
        double  rownd2, rowns2[20], pdnorm;
        long     iownd2[3], iowns2[2], jtyp, mused, mxordn, mxords;
        } _2;
      struct
        {
        double  rownd2, pdest, pdlast, ratio, cm1[12], cm2[5], pdnorm;
        long     iownd2[3], icount, irflag, jtyp, mused, mxordn, mxords;
        } _3;
      } lsa001_;

    struct
      {
      long mesflg;//, lunit;
      } eh0001_;

  public:


     int LSoda(/*S_fp f,*/ long *neq, double *y, double *t, double *tout, long *itol, double *rtol, double *atol, long *itask, long *istate, long *iopt, double *rwork, long *lrw, long *iwork, long *liw, /*U_fp jac,*/ long *jt);
     double bnorm_(long *n, double *a, long *nra, long *ml, long *mu, double *w);
     int cfode_(long *meth, double *elco, double *tesco);
     int ewset_(long *n, long *itol, double *rtol, double *atol, double *ycur, double *ewt);
     double fnorm_(long *n, double *a, double *w);
     int intdy_(double *t, long *k, double *yh, long *nyh, double *dky, long *iflag);
     //int prja_(long *neq, double *y, double *yh, long *nyh, double *ewt, double *ftem, double *savf, double *wm, long *iwm, S_fp f, S_fp jac);
     int stoda_(long *neq, double *y, double *yh, long *nyh, double *yh1, double *ewt, double *savf, double *acor, double *wm, long *iwm /*,S_fp f, U_fp jac, S_fp pjac, S_fp slvs*/);
     double vmnorm_(long *n, double *v, double *w);
     int xerrwv_(char *msg, long *nmes, long *nerr, long *level, long *ni, long *i1, long *i2, long *nr, double *r1, double *r2, long L);
    /* comlen ls0001_ 1900 */
    /* comlen lsa001_ 212 */
    /* comlen eh0001_ 8 */
    /*:ref: d1mach_ 7 1 4 */
    /*:ref: stoda_ 14 14 4 7 7 4 7 7 7 7 7 4 214 200 214 214 */
    /*:ref: dgefa_ 14 5 7 4 4 4 4 */
    /*:ref: dgbfa_ 14 7 7 4 4 4 4 4 4 */
    /*:ref: dgesl_ 14 6 7 4 4 4 7 4 */
    /*:ref: dgbsl_ 14 8 7 4 4 4 4 4 7 4 */
    /* Rerunning f2c -P may change prototypes or declarations. */

    virtual int LSFun(long *neq, double *t, double *y, double *yp)=0;
    virtual int LSJac(long *neq, double *t, double *y, long *ml, long *mu, double *pd, long *nrowpd)=0;
    virtual int LSPJac(long *neq, double *y, double *yh, long *nyh, double  *ewt,
                      double  *ftem, double *savf, double  *wm, long *iwm);
    int CLSoda::LSSolSy(double *wm, long *iwm, double *x, double *tem);

  };
