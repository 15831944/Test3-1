//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __SPDBE_NBS_CPP
#include "sp_db.h"
#include "surfaces.h"

//#define dbgBuildSDB    0

// ==========================================================================
//
//
//
// ===========================================================================
#if WithNBSH2OMdl
// ==========================================================================
//
//
//
// ===========================================================================

#if dbgBuildSDB
#include "dbgmngr.h"
static CDbgMngr dbgBuild("Specie_DB", "BuildNBS"); 
#endif

// ==========================================================================
//
//
//
// ===========================================================================


extern double pow(double, double);
static double pow_di(double * d, long * e) { return pow(*d, *e);};
static double pow_dd(double * d, double * e) { return pow(*d, *e);};

// forward decs
double tdpsdt_(double *t);
long ideal_(double *t);

/* Common Block Declarations */

struct aconst_1_ {
    double wm, gascon, tz, aa, zb, dzb, yb, uref, sref;
};
struct aconst_2_ {
    double wm, gascon, tz, aa, z, dz, y, uref, sref;
};
struct aconst_3_ {
    double wm, gascon, tz, a, z, dz, y, uref, sref;
};
struct aconst_4_ {
    double wm, gascon, tz, aa, zb, dzb, y, uref, sref;
};
struct aconst_5_ {
    double wm, gascon, tc, aa, z, dz, y, uref, sref;
};

#define aconst_1 (*(struct aconst_1_ *) &aconst_)
#define aconst_2 (*(struct aconst_2_ *) &aconst_)
#define aconst_3 (*(struct aconst_3_ *) &aconst_)
#define aconst_4 (*(struct aconst_4_ *) &aconst_)
#define aconst_5 (*(struct aconst_5_ *) &aconst_)

struct nconst_1_ {
    double g[40];
    long ii[40], jj[40], nc;
};
struct nconst_2_ {
    double g[40];
    long ii[40], jj[40], n;
};

#define nconst_1 (*(struct nconst_1_ *) &nconst_)
#define nconst_2 (*(struct nconst_2_ *) &nconst_)

struct ellcon_1_ {
    double g1, g2, gf, b1, b2, b1t, b2t, b1tt, b2tt;
};
struct ellcon_2_ {
    double g1, g2, gf, bb1, bb2, b1t, b2t, b1tt, b2tt;
};

#define ellcon_1 (*(struct ellcon_1_ *) &ellcon_)
#define ellcon_2 (*(struct ellcon_2_ *) &ellcon_)

struct bconst_1_ {
    double bp[10], bq[10];
};

#define bconst_1 (*(struct bconst_1_ *) &bconst_)

struct addcon_1_ {
    double atz[4], adz[4], aat[4], aad[4];
};

#define addcon_1 (*(struct addcon_1_ *) &addcon_)

struct {
    double ab, gb, sb, ub, hb, cvb, dpdtb;
} basef_;

#define basef_1 basef_

struct {
    double ar, gr, sr, ur, hr, cvr, dpdtr;
} resf_;

#define resf_1 resf_

union {
    struct {
        double q, q5;
    } _1;
    struct {
        double q0, q5;
    } _2;
    struct {
        double qp, qdp;
    } _3;
    struct {
        double q00, q11;
    } _4;
} qqqq_;

#define qqqq_1 (qqqq_._1)
#define qqqq_2 (qqqq_._2)
#define qqqq_3 (qqqq_._3)
#define qqqq_4 (qqqq_._4)

struct {
    double ai, gi, si, ui, hi, cvi, cpi;
} idf_;

#define idf_1 idf_

struct {
    double ad, gd, sd, ud, hd, cvd, cpd, dpdt, dvdt, cjtt, cjth;
} fcts_;

#define fcts_1 fcts_

struct {
    double dvap, svap, uvap, hvap, cvvap, cpvap;
} vap_;

#define vap_1 vap_

struct {
    double dliq, sliq, uliq, hliq, cvliq, cpliq;
} liq_;

#define liq_1 liq_

struct {
    long it, id, ip, ih;
    double nt, nd, np, nh, ft, fd, fp, fh;
} units_;

#define units_1 units_

/* Initialized data */

struct {
    double e_1[4];
    double fill_2[3];
    double e_3[2];
    } aconst_ = { 18.0152, .461522, 647.073, 1., {0}, -4328.455039, 7.6180802 
            };

struct {
    double e_1[40];
    long e_2[81];
    } nconst_ = { -530.62968529023, 2274.4901424408, 787.79333020687, 
            -69.830527374994, 17863.832875422, -39514.731563338, 
            33803.884280753, -13855.050202703, -256374.3661326, 
            482125.75981415, -341830.1696966, 122231.56417448, 
            1179743.3655832, -2173481.0110373, 1082995.216862, 
            -254419.98064049, -3137777.4947767, 5291191.0757704, 
            -1380257.7177877, -251099.14369001, 4656182.6115608, 
            -7275277.3275387, 417742.46148294, 1401635.8244614, 
            -3155523.1392127, 4792966.6384584, 409126.64781209, 
            -1362636.9388386, 696252.20862664, -1083490.0096447, 
            -227228.27401688, 383654.8600066, 6883.3257944332, 
            21757.245522644, -2662.794482977, -70730.418082074, -.225, -1.68, 
            .055, -93., 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 
            4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 8, 8, 8, 8, 2, 2, 0, 4, 2, 2, 2, 4, 
            2, 3, 5, 7, 2, 3, 5, 7, 2, 3, 5, 7, 2, 3, 5, 7, 2, 3, 5, 7, 2, 3, 
            5, 7, 2, 3, 5, 7, 2, 3, 5, 7, 1, 4, 4, 4, 0, 2, 0, 0, 36 };

struct {
    double e_1[3];
    double fill_2[6];
    } ellcon_ = { 11., 44.333333333333, 3.5 };

struct {
    double e_1[20];
    } bconst_ = { .7478629, -.3540782, 0., 0., .007159876, 0., -.003528426, 
            0., 0., 0., 1.1278334, 0., -.5944001, -5.010996, 0., .63684256, 
            0., 0., 0., 0. };

struct {
    double e_1[16];
    } addcon_ = { 640., 640., 641.6, 270., .319, .319, .319, 1.55, 2e4, 2e4, 
            4e4, 25., 34., 40., 30., 1050. };


/* Table of constant values */

static double c_b18 = -.6;
static double c_b32 = .325;



/*      This program is from : NBS/NRC STEAM TABLES 1984 */

/*      OCR scanned by Chris Devadas, Comalco Research, April 1995 */
/*      Debugged and compiled by Tony Crisp, Comalco M&A, 6 May 1995 */


/* This BLOCKDATA Subroutine supplies most of the fixed parameters */
/* used in the rest of the routines. BP is the b(i) of Table 1, */
/* BQ is the B(i) of Table 1. G1,G2 and GF are the Alpha, Beta */
/* and Gamma of Eq.2, and G,II,JJ are the g(i), k(i)-1(?) and */
/* l(i)+1(?) of Eq.5. */




/* Subroutine */ long bb_(double *t)
//double *t;
{
    /* System generated locals */
    long i__1;

    /* Builtin functions */
    //double log();

    /* Local variables */
    static long i;
    static double v[10];


/* This Subroutine calculates the B'S of Eqs. 3,4 using coefficients */
/* from BLOCKDATA, calculating also the first and second derivatives */
/* w.r.to temp.  The B'S calculated here are in cm3/g. */

    v[0] = (float)1.;
    for (i = 2; i <= 10; ++i) {
/* L2: */
        v[i - 1] = v[i - 2] * aconst_2.tz / *t;
    }
    ellcon_1.b1 = bconst_1.bp[0] + bconst_1.bp[1] * log((float)1. / v[1]);
    ellcon_1.b2 = bconst_1.bq[0];
    ellcon_1.b1t = bconst_1.bp[1] * v[1] / aconst_2.tz;
    ellcon_1.b2t = (float)0.;
    ellcon_1.b1tt = (float)0.;
    ellcon_1.b2tt = (float)0.;
    for (i = 3; i <= 10; ++i) {
        ellcon_1.b1 += bconst_1.bp[i - 1] * v[i - 2];
        ellcon_1.b2 += bconst_1.bq[i - 1] * v[i - 2];
        ellcon_1.b1t -= (i - 2) * bconst_1.bp[i - 1] * v[i - 2] / *t;
        ellcon_1.b2t -= (i - 2) * bconst_1.bq[i - 1] * v[i - 2] / *t;
/* Computing 2nd power */
        i__1 = i - 2;
        ellcon_1.b1tt += bconst_1.bp[i - 1] * (i__1 * i__1) * v[i - 2] / *t / 
                *t;
/* L4: */
/* Computing 2nd power */
        i__1 = i - 2;
        ellcon_1.b2tt += bconst_1.bq[i - 1] * (i__1 * i__1) * v[i - 2] / *t / 
                *t;
    }
    ellcon_1.b1tt -= ellcon_1.b1t / *t;
    ellcon_1.b2tt -= ellcon_1.b2t / *t;
    return 0;
} /* bb_ */


double base_(double *d, double *t)
{
    /* System generated locals */
    double ret_val, d__1, d__2, d__3;

    /* Builtin functions */
    //double log();

    /* Local variables */
    static double bb2tt, x, z0, dz0;


/* This function calculates Z (=Pbase/(DRT) of Eq. 2) (called BASE), */
/* and also Abase,Gbase,Sbase,Ubase,Hbase,CVbase, AND 1/(DRT) * dP/dT */
/* for the BASE function (returns DPDTB here ?) */
/* The AB,GB,SB,UB,HB and CVB are calculated in dimensionless units: */
/* AB/RT, GB/RT, SB/R, etc. */

/* G1,G2 and GF are the Alpha  Beta and Gamma of Eq.2, which are */
/* supplied by the BLOCKDATA routine.  B1 and B2 are the "excluded */
/* volume" and "2nd Virial" (Eqs 3 and 4) supplied by the Subroutine */
/* BB(T), which also supplies the 1st and 2nd derivatives with */
/* respect to T (B1T,B2T,B1TT,B2TT). */

    aconst_3.y = ellcon_1.b1 * .25 * *d;
    x = 1. - aconst_3.y;
/* Computing 3rd power */
    d__1 = x, d__2 = d__1;
    z0 = (ellcon_1.g1 * aconst_3.y + 1. + ellcon_1.g2 * aconst_3.y * 
            aconst_3.y) / (d__2 * (d__1 * d__1));
    aconst_3.z = z0 + aconst_3.y * 4. * (ellcon_1.b2 / ellcon_1.b1 - 
            ellcon_1.gf);
/* Computing 3rd power */
    d__1 = x, d__2 = d__1;
/* Computing 4th power */
    d__3 = x, d__3 *= d__3;
    dz0 = (ellcon_1.g1 + ellcon_1.g2 * 2. * aconst_3.y) / (d__2 * (d__1 * 
            d__1)) + (ellcon_1.g1 * aconst_3.y + 1. + ellcon_1.g2 * 
            aconst_3.y * aconst_3.y) * 3. / (d__3 * d__3);
    aconst_3.dz = dz0 + (ellcon_1.b2 / ellcon_1.b1 - ellcon_1.gf) * 4.;
    basef_1.ab = -log(x) - (ellcon_1.g2 - 1.) / x + 28.16666667 / x / x + 
            aconst_3.y * 4. * (ellcon_1.b2 / ellcon_1.b1 - ellcon_1.gf) + 
            15.166666667 + log(*d * *t * aconst_3.gascon / .101325);
    basef_1.gb = basef_1.ab + aconst_3.z;
    ret_val = aconst_3.z;
    bb2tt = *t * *t * ellcon_1.b2tt;
    basef_1.ub = -(*t) * ellcon_1.b1t * (aconst_3.z - 1. - *d * ellcon_1.b2) /
             ellcon_1.b1 - *d * *t * ellcon_1.b2t;
    basef_1.hb = aconst_3.z + basef_1.ub;
/* Computing 2nd power */
    d__1 = *t * ellcon_1.b1t / ellcon_1.b1;
/* Computing 2nd power */
    d__2 = *t * ellcon_1.b1t / ellcon_1.b1;
    basef_1.cvb = basef_1.ub * 2. + (z0 - 1.) * (d__1 * d__1 - *t * *t * 
            ellcon_1.b1tt / ellcon_1.b1) - *d * (bb2tt - ellcon_1.gf * 
            ellcon_1.b1tt * *t * *t) - d__2 * d__2 * aconst_3.y * dz0;
    basef_1.dpdtb = ret_val / *t + ret_val * *d / aconst_3.z * (aconst_3.dz * 
            ellcon_1.b1t / 4. + ellcon_1.b2t - ellcon_1.b2 / ellcon_1.b1 * 
            ellcon_1.b1t);
    basef_1.sb = basef_1.ub - basef_1.ab;
    return ret_val;
} /* base_ */


/* Subroutine */ long qq_(double *t, double *d)
{
    /* System generated locals */
    long i__1;
    double d__1;
    static double equiv_0[11], equiv_1[10];

    /* Builtin functions */
    //double exp(), pow_di();

    /* Local variables */
    static double dadt, dfdt, e;
    static long i, j, k, l;
    static double v, q10, q20;
    static long km;
    static double qm, qp;
#define qr (equiv_0)
#define qt (equiv_1)
    static double rt, tx, zz, d2f, q2a, ex1, ex2, q5t, del, fct, dex, ddz,
             dpt, att, tau, tex;
#define qzr (equiv_0 + 2)
#define qzt (equiv_1 + 1)


/* This routine calculates, for a given T(K) and D(g/cm3), the residual */
/* contributions to: Pressure (Q), Helmholtz Fct (AR), dP/dD (Eq.5), */
/* and also to the Gibbs function, Entropy,Internal Energy, Enthalpy, */
/* isochoric Heat Capacity and dP/dT. (EQ.5) */
/* Terms 37 thru 39 are the additional terms affecting only the */
/* immediate vicinity of the critical point, and term 40 is the */
/* additional term improving the low T, high P region. */

    rt = aconst_2.gascon * *t;
    qr[0] = 0.;
    qqqq_1.q5 = 0.;
    qqqq_1.q = 0.;
    resf_1.ar = 0.;
    dadt = 0.;
    resf_1.cvr = 0.;
    resf_1.dpdtr = 0.;
    e = exp(-aconst_2.aa * *d);
    q10 = *d * *d * e;
    q20 = 1. - e;
    qr[1] = q10;
    v = aconst_2.tz / *t;
    qt[0] = *t / aconst_2.tz;
    for (i = 2; i <= 10; ++i) {
        qr[i] = qr[i - 1] * q20;
/* L4: */
        qt[i - 1] = qt[i - 2] * v;
    }
    i__1 = nconst_2.n;
    for (i = 1; i <= i__1; ++i) {
        k = nconst_2.ii[i - 1] + 1;
        l = nconst_2.jj[i - 1];
        zz = (double) k;
        qp = nconst_2.g[i - 1] * aconst_2.aa * qzr[k - 2] * qzt[l - 1];
        qqqq_1.q += qp;
        qqqq_1.q5 += aconst_2.aa * ((float)2. / *d - aconst_2.aa * ((float)1. 
                - e * (k - 1) / q20)) * qp;
        resf_1.ar += nconst_2.g[i - 1] * qzr[k - 1] * qzt[l - 1] / q10 / zz / 
                rt;
        dfdt = pow_di(&q20, &k) * (1 - l) * qzt[l] / aconst_2.tz / k;
        d2f = l * dfdt;
        dpt = dfdt * q10 * aconst_2.aa * k / q20;
        dadt += nconst_2.g[i - 1] * dfdt;
        resf_1.dpdtr += nconst_2.g[i - 1] * dpt;
/* L10: */
        resf_1.cvr += nconst_2.g[i - 1] * d2f / aconst_2.gascon;
    }
    qp = 0.;
    q2a = 0.;

    for (j = 37; j <= 40; ++j) {
        if (nconst_2.g[j - 1] == 0.) {
            goto L20;
        }
        k = nconst_2.ii[j - 1];
        km = nconst_2.jj[j - 1];
        ddz = addcon_1.adz[j - 37];
        del = *d / ddz - 1.;
        if (fabs(del) < 1e-10) {
            del = 1e-10;
        }
/* ****     DD = DEL*DEL    ***** does not appear to get used. */
        ex1 = -addcon_1.aad[j - 37] * pow_di(&del, &k);
        dex = exp(ex1) * pow_di(&del, &km);
        att = addcon_1.aat[j - 37];
        tx = addcon_1.atz[j - 37];
        tau = *t / tx - 1.;
        ex2 = -att * tau * tau;
        tex = exp(ex2);
        q10 = dex * tex;
        i__1 = k - 1;
        qm = km / del - k * addcon_1.aad[j - 37] * pow_di(&del, &i__1);
/* Computing 2nd power */
        d__1 = *d;
        fct = qm * (d__1 * d__1) * q10 / ddz;
/* Computing 2nd power */
        d__1 = *d / ddz;
        i__1 = k - 2;
        q5t = fct * ((float)2. / *d + qm / ddz) - d__1 * d__1 * q10 * (km / 
                del / del + k * (k - 1) * addcon_1.aad[j - 37] * pow_di(&del, 
                &i__1));
        qqqq_1.q5 += q5t * nconst_2.g[j - 1];
        qp += nconst_2.g[j - 1] * fct;
        dadt -= nconst_2.g[j - 1] * 2. * att * tau * q10 / tx;
        resf_1.dpdtr -= nconst_2.g[j - 1] * 2. * att * tau * fct / tx;
        q2a += *t * nconst_2.g[j - 1] * (att * 4. * ex2 + att * 2.) * q10 / 
                tx / tx;
        resf_1.ar += q10 * nconst_2.g[j - 1] / rt;
L20:
        ;
    }
    resf_1.sr = -dadt / aconst_2.gascon;
    resf_1.ur = resf_1.ar + resf_1.sr;
    resf_1.cvr += q2a / aconst_2.gascon;
    qqqq_1.q += qp;
    return 0;
} /* qq_ */

#undef qzt
#undef qzr
#undef qt
#undef qr



/* Subroutine */ long dfind_(double *dout, double*p, double*d, double*t, double*dpd)
{
    /* System generated locals */
    double d__1;

    /* Local variables */
    //extern double base_();
    static double dpdx, zdum;
    static long l;
    static double x, dd, dp, pp;
    //extern /* Subroutine */ long qq_();
    static double rt;


/* Routine to find Density corresponding to input Pressure P(MPa), and */
/* Temperature T(K), using initial guess Density D(g/cm3). The output */
/* density is in g/cm3. Also as a byproduct, dP/dD is calculated */
/* ("DPD", MPa.cm3/g) */

    dd = *d;
    rt = aconst_2.gascon * *t;
    l = 0;
L9:
    ++l;
/* L11: */
    if (dd <= 0.) {
        dd = 1e-8;
    }
    if (dd > 1.9) {
        dd = 1.9;
    }
    qq_(t, &dd);
    zdum = base_(&dd, t);
    pp = rt * dd * zdum + qqqq_2.q0;
    *dpd = rt * (aconst_2.z + aconst_2.y * aconst_2.dz) + qqqq_2.q5;

/* The following 3 lines check for negative dP/dD, and if so assume */
/* guess to be in 2-phase region, and adjust guess accordingly. */

    if (*dpd > 0.) {
        goto L13;
    }
    if (*d >= .2967) {
        dd *= 1.02;
    }
    if (*d < .2967) {
        dd *= .98;
    }
    if (l <= 10) {
        goto L9;
    }
L13:
    dpdx = *dpd * 1.1;
    if (dpdx < .1) {
        dpdx = .1;
    }
    dp = (d__1 = 1. - pp / *p, fabs(d__1));
    if (dp < 1e-8) {
        goto L20;
    }
    if (*d > .3 && dp < 1e-7) {
        goto L20;
    }
    if (*d > .7 && dp < 1e-6) {
        goto L20;
    }
    x = (*p - pp) / dpdx;
    if (fabs(x) > .1) {
        x = x * .1 / fabs(x);
    }
    dd += x;
    if (dd <= 0.) {
        dd = 1e-8;
    }
/* L19: */
    if (l <= 30) {
        goto L9;
    }
L20:
    *dout = dd;
    return 0;
} /* dfind_ */


/* Subroutine */ long therm_(double *d, double *t)
{
    /* System generated locals */
    double d__1;

    /* Local variables */
    static double dpdd;
    //extern /* Subroutine */ long ideal_();
    static double z, rt;


/* This subroutine calculates the thermodynamic functions in */
/* dimensionless units (AD=A/RT, GD=G/RT, SD=S/R, UD=U/RT, */
/* HD=H/RT, CvD=Cv/R, and CpD=Cp/R) */

    ideal_(t);
    rt = aconst_4.gascon * *t;
    z = aconst_4.zb + qqqq_3.qp / rt / *d;
    dpdd = rt * (aconst_4.zb + aconst_4.y * aconst_4.dzb) + qqqq_3.qdp;
    fcts_1.ad = basef_1.ab + resf_1.ar + idf_1.ai - aconst_4.uref / *t + 
            aconst_4.sref;
    fcts_1.gd = fcts_1.ad + z;
    fcts_1.ud = basef_1.ub + resf_1.ur + idf_1.ui - aconst_4.uref / *t;
    fcts_1.dpdt = rt * *d * basef_1.dpdtb + resf_1.dpdtr;
    fcts_1.cvd = basef_1.cvb + resf_1.cvr + idf_1.cvi;
/* Computing 2nd power */
    d__1 = fcts_1.dpdt;
    fcts_1.cpd = fcts_1.cvd + *t * (d__1 * d__1) / (*d * *d * dpdd * 
            aconst_4.gascon);
    fcts_1.hd = fcts_1.ud + z;
    fcts_1.sd = basef_1.sb + resf_1.sr + idf_1.si - aconst_4.sref;
    fcts_1.dvdt = fcts_1.dpdt / dpdd / *d / *d;
    fcts_1.cjtt = (float)1. / *d - *t * fcts_1.dvdt;
    fcts_1.cjth = -fcts_1.cjtt / fcts_1.cpd / aconst_4.gascon;
    return 0;
} /* therm_ */


double ps_(double *t)
{
    /* Initialized data */

    static double a[8] = { -7.8889166,2.5514255,-6.716169,33.239495,
            -105.38479,174.35319,-148.39348,48.631602 };

    /* System generated locals */
    double ret_val, d__1;

    /* Builtin functions */
    //double pow_dd(), exp();

    /* Local variables */
    static double b;
    static long i;
    static double q, v, w, z, pl;


/* This function calculates an approximation to the sat.vapour pressure, 
*/
/* PS, as a function of the input temperature. The vapor pressure */
/* calculated agrees with the vapor pressure predicted by the surface */
/* to within .02% a degree or so of the critical temperature, */
/* and can serve as an initial guess for further refinement by */
/* imposing the condition that Gl=Gv. */

    if (*t > 314.) {
        goto L2;
    }
    pl = 6.3573118 - 8858.843 / *t + pow_dd(t, &c_b18) * 607.56335;
    ret_val = exp(pl) * .1;
    return ret_val;
L2:
    v = *t / 647.25;
    w = (d__1 = 1. - v, fabs(d__1));
    b = 0.;
    for (i = 1; i <= 8; ++i) {
        z = (double) i;
/* L4: */
        d__1 = (z + 1.) / 2.;
        b += a[i - 1] * pow_dd(&w, &d__1);
    }
    q = b / v;
    ret_val = exp(q) * 22.093;
    return ret_val;
} /* ps_ */


double tsat_(double *p)
{
    /* System generated locals */
    double ret_val, d__1;

    /* Builtin functions */
    //double log();

    /* Local variables */
    static long k;
    static double dp;
    static double tg;
    static double pl;
    static double pp;
    //extern double ps_(), tdpsdt_();


/* This function calculates the saturation temperature for a given */
/* pressure, by an iterative process using PS and TDPSDT. */

    ret_val = (float)0.;
    if (*p > 22.05) {
        return ret_val;
    }
    k = 0;
    pl = log(*p) + 2.302585;

/* PL+LOGE(10)+LOGE(P) to convert equation from bars to MPa */

    tg = pl * (pl * (pl * (pl * (float).0193855 + (float).24834) + (float)
            2.3819) + (float)27.7589) + (float)372.83;
L1:
    if (tg < 273.15) {
        tg = 273.15;
    }
    if (tg > 647.) {
        tg = 647.;
    }
    if (k < 8) {
        goto L2;
    }
/*       WRITE(6,*) K,P,PP,TG */
    goto L8;
L2:
    ++k;
    pp = ps_(&tg);
    dp = tdpsdt_(&tg);
    if ((d__1 = (float)1. - pp / *p, fabs(d__1)) < 1e-5) {
        goto L8;
    }
    tg *= (*p - pp) / dp + (float)1.;
    goto L1;
L8:
    ret_val = tg;
    return ret_val;
} /* tsat_ */

double tdpsdt_(double *t)
{
    /* Initialized data */

    static double a[8] = { -7.8889166,2.5514255,-6.716169,33.239495,
            -105.38479,174.35319,-148.39348,48.631602 };

    /* System generated locals */
    double ret_val, d__1;

    /* Builtin functions */
    //double pow_dd(), exp();

    /* Local variables */
    static double b, c;
    static long i;
    static double q, v, w, y, z;


/* This function calculates T*(dPs/dT), and is used by the function TSAT. 
*/

    v = *t / (float)647.25;
    w = (float)1. - v;
    b = (float)0.;
    c = (float)0.;
    for (i = 1; i <= 8; ++i) {
        z = (double) i;
        d__1 = (z + (float)1.) / (float)2.;
        y = a[i - 1] * pow_dd(&w, &d__1);
        c += y / w * ((float).5 - z * (float).5 - (float)1. / v);
/* L4: */
        b += y;
    }
    q = b / v;
    ret_val = exp(q) * 22.093 * c;
    return ret_val;
} /* tdpsdt_ */

/* Subroutine */ long ideal_(double *t)
{
    /* Initialized data */

    static double c[18] = { 19.730271018,20.9662681977,-.483429455355,
            6.05743189245,22.56023885,-9.87532442,-4.3135538513,.458155781,
            -.047754901883,.0041238460633,-2.7929052852e-4,1.4481695261e-5,
            -5.6473658748e-7,1.6200446e-8,-3.303822796e-10,4.51916067368e-12,
            -3.70734122708e-14,1.37546068238e-16 };

    /* System generated locals */
    long i__1;

    /* Builtin functions */
    //double log(), pow_di();

    /* Local variables */
    static long i;
    static double tl, tt;


/* This subroutine calculates the thermodynamic properties for */
/* water in the Ideal Gas State from function of H.W. Woolley */

    tt = *t / 100.;
    tl = log(tt);
    idf_1.gi = -(c[0] / tt + c[1]) * tl;
    idf_1.hi = c[1] + c[0] * (1. - tl) / tt;
    idf_1.cpi = c[1] - c[0] / tt;
    for (i = 3; i <= 18; ++i) {
        i__1 = i - 6;
        idf_1.gi -= c[i - 1] * pow_di(&tt, &i__1);
        i__1 = i - 6;
        idf_1.hi += c[i - 1] * (i - 6) * pow_di(&tt, &i__1);
/* L8: */
        i__1 = i - 6;
        idf_1.cpi += c[i - 1] * (i - 6) * (i - 5) * pow_di(&tt, &i__1);
    }
    idf_1.ai = idf_1.gi - 1.;
    idf_1.ui = idf_1.hi - 1.;
    idf_1.cvi = idf_1.cpi - 1.;
    idf_1.si = idf_1.ui - idf_1.ai;
    return 0;
} /* ideal_ */


/* Subroutine */ long secvir_(double *t, double *vir)
{
    /* System generated locals */
    long i__1, i__2;

    /* Builtin functions */
    //double pow_di();

    /* Local variables */
    static long j, l;
    static double v;
    //extern /* Subroutine */ long bb_();


/* This subroutine calculates the second Virial in cm3/g */
/* at temperature T øK. */
/* This subroutine is for convenience only, and not used by program. */

    bb_(t);
    v = aconst_5.tc / *t;
    *vir = ellcon_2.bb2;
    i__1 = nconst_1.nc;
    for (j = 1; j <= i__1; ++j) {
        if (nconst_1.ii[j - 1] != 0) {
            goto L3;
        }
        l = nconst_1.jj[j - 1];
        i__2 = l - 1;
        *vir += nconst_1.g[j - 1] * pow_di(&v, &i__2) / *t / aconst_5.gascon;
L3:
        ;
    }
    return 0;
} /* secvir_ */


/* Subroutine */ long corr_(double *t, double *p, double *dl, double *dv, double *delg)
{
    /* System generated locals */
    double d__1;

    /* Builtin functions */
    //double pow_dd();

    /* Local variables */
    //extern double base_();
    static double zdum;
    //extern /* Subroutine */ long dfind_(), therm_(), bb_();
    static double gl, dq, gv;
    //extern /* Subroutine */ long qq_();
    static double rt, tau;


/* Subroutine CORR will calculate, for an input T and P at or near the */
/* sat. vap. pressure, the corresponding liquid and vapour densities and 
*/
/* also DELG = (Gl-Gv)/RT for use in calculating the correction to the */
/* vapour pressure for DELG = O. */

    rt = aconst_1.gascon * *t;
    if (*t > 646.3) {
        goto L101;
    }
    liq_1.dliq = *dl;
    if (*dl <= 0.) {
        liq_1.dliq = (float)1.11 - *t * (float)4e-4;
    }
    bb_(t);
    dfind_(dl, p, &liq_1.dliq, t, &dq);
    therm_(dl, t);
    gl = fcts_1.gd;
    liq_1.dliq = *dl;
    liq_1.sliq = fcts_1.sd;
    liq_1.uliq = fcts_1.ud;
    liq_1.hliq = fcts_1.hd;
    liq_1.cvliq = fcts_1.cvd;
    liq_1.cpliq = fcts_1.cpd;
    vap_1.dvap = *dv;
    if (*dv <= 0.) {
        vap_1.dvap = *p / rt;
    }
    dfind_(dv, p, &vap_1.dvap, t, &dq);
    if (*dv < 5e-7) {
        *dv = 5e-7;
    }
    therm_(dv, t);
    gv = fcts_1.gd;
    *delg = gl - gv;
    vap_1.dvap = *dv;
    vap_1.svap = fcts_1.sd;
    vap_1.uvap = fcts_1.ud;
    vap_1.hvap = fcts_1.hd;
    vap_1.cvvap = fcts_1.cvd;
    vap_1.cpvap = fcts_1.cpd;
    return 0;
L101:
    *p = (float)0.;
    if (*t > 647.126) {
        return 0;
    }
    *delg = (float)0.;
    bb_(t);
    d__1 = (float)1. - *t / 647.126;
    tau = pow_dd(&d__1, &c_b32) * .657128;
    *dl = tau + .322;
    *dv = .322 - tau;
    zdum = base_(dv, t);
    qq_(t, dv);
    *p = rt * *dv * zdum + qqqq_4.q00;
    return 0;
} /* corr_ */


/* Subroutine */ long pcorr_(double *t, double *p, double *dl, double *dv)
{
    static double delg;
    //extern /* Subroutine */ long corr_();
    static double dp;
    //extern double ps_();


/* Subroutine PCORR will calculate the sat vap pressure P and the liq and 
*/
/* vapour densities corresponding to the input T, corrected such that */
/* Gl-Gv=O.  The function PS is required which will give a reasonably */
/* good approximation to the vapour pressure to be used as the starting */
/* point for the iteration. */

    *p = ps_(t);
L2:
    corr_(t, p, dl, dv, &delg);
    dp = delg * aconst_1.gascon * *t / ((float)1. / *dv - (float)1. / *dl);
    *p += dp;
    if (fabs(delg) < 1e-5) {
        return 0;
    }
    goto L2;
} /* pcorr_ */


/* Subroutine */ long tcorr_(double *t, double *p, double *dl, double *dv)
{
    static double delg;
    //extern /* Subroutine */ long corr_();
    //extern double tsat_();
    static double dp;
    //extern double tdpsdt_();


/* Subroutine TCORR is similar to "PCORR" except that the sat temperature
*/
/* corresponding to the input vapour pressure is found. Functions called 
*/
/* are TSAT and TDPSDT which give an approximation to T(sat) as a */
/* function of T*dP(sat)/dT. */

    *t = tsat_(p);
    if (*t == 0.) {
        return 0;
    }
L2:
    corr_(t, p, dl, dv, &delg);
    dp = delg * aconst_1.gascon * *t / ((float)1. / *dv - (float)1. / *dl);
    *t *= (float)1. - dp / tdpsdt_(t);
    if (fabs(delg) < 1e-5) {
        return 0;
    }
    goto L2;
} /* tcorr_ */

//===========================================================================
//
//
//
//===========================================================================

// the above code is NON_REENTRANT
// we must lock access due to the multi threaded nature of SysCAD
static CCriticalSection m_NBSLock;

//===========================================================================
// The units expected by these routines are :
// T in K
// P in MPa
//===========================================================================

const double TCrit=647.073;

double StmVP(double T)
  {
  CSingleLock Lock(&m_NBSLock, true);
  double p, dl=0, dv=0;
  T=Range(ZeroCinK, T, TCrit);
  pcorr_(&T, &p, &dl, &dv);
  return p*1000;
  }
double StmVT(double P)
  {
  CSingleLock Lock(&m_NBSLock, true);
  double t, dl=0, dv=0;
  P*=0.001;
  //T=Range(ZeroCinK, T, TCrit);
  tcorr_(&t, &P, &dl, &dv);
  return t;
  }

//===========================================================================

static void NBSCalcs(double TIn, double PIn, double *pD=NULL)
  {
  PIn*=0.001;
/*
  // ??
  double Tt=TIn;
  double Pt=PIn;
  double DL, DV;
  tcorr_(&Tt, &Pt, &DL, &DV);
  double TSatD=Tt;

  // ??
  Tt=TIn;
  Pt=PIn;
  pcorr_(&Tt, &Pt, &DL, &DV);
  double PSatD=Pt;
*/  
  // ..
  bb_(&TIn);
  double DGSS=PIn/TIn/0.4;
  double PSAT=2.0e4;
  double DLL=0.;
  double DVV=0.;
  if(TIn<aconst_1.tz) 
    pcorr_(&TIn,&PSAT,&DLL,&DVV);
  if (PIn>PSAT)
    DGSS=DLL;
  double D,DQ;
  dfind_(&D,&PIn,&DGSS,&TIn,&DQ);
  therm_(&D,&TIn);
  if (pD)
    *pD=D;
  }

//---------------------------------------------------------------------------

double StmCp(double T, double P)
  {
  CSingleLock Lock(&m_NBSLock, true);
  T=Max(ZeroCinK, T);
  double Vt=StmVT(P);
  if (T<Vt)
    {
    double dVtA=Min(1.0, 0.01*Vt);
    double dVtB=0.1*dVtA;
    double T1=Vt+(dVtA+dVtB);
    double T2=Vt+dVtB;
    NBSCalcs(T1, P);
    double C1=fcts_1.cpd*aconst_1.gascon;
    NBSCalcs(T2, P);
    double C2=fcts_1.cpd*aconst_1.gascon;
    return C2+(C1-C2)*(T-T2)/dVtA;
    }
  NBSCalcs(T, P);
  return fcts_1.cpd*aconst_1.gascon;
  }

//---------------------------------------------------------------------------

double StmH(double T, double P)
  {
  CStopWatch SW;
  SW.ReStart();
  CSingleLock Lock(&m_NBSLock, true);
  T=Max(ZeroCinK, T);
  double Vt=StmVT(P);
  if (T<Vt)
    {
    double dVtA=Min(1.0, 0.01*Vt);
    double dVtB=0.1*dVtA;
    double T1=Vt+(dVtA+dVtB);
    double T2=Vt+dVtB;
    NBSCalcs(T1, P);
    double H1=fcts_1.hd*aconst_1.gascon*T1;
    NBSCalcs(T2, P);
    double H2=fcts_1.hd*aconst_1.gascon*T2;
    SW.Stop();
    return H2+(H1-H2)*(T-T2)/dVtA;
    }
  NBSCalcs(T, P);
  SW.Stop();
  return fcts_1.hd*aconst_1.gascon*T;
  }

//---------------------------------------------------------------------------

double StmS(double T, double P)
  {
  CSingleLock Lock(&m_NBSLock, true);
  T=Max(ZeroCinK, T);
  double Vt=StmVT(P);
  if (T<Vt)
    {
    double dVtA=Min(1.0, 0.01*Vt);
    double dVtB=0.1*dVtA;
    double T1=Vt+(dVtA+dVtB);
    double T2=Vt+dVtB;
    NBSCalcs(T1, P);
    double S1=fcts_1.sd*aconst_1.gascon;
    NBSCalcs(T2, P);
    double S2=fcts_1.sd*aconst_1.gascon;
    return S2+(S1-S2)*(T-T2)/dVtA;
    }
  NBSCalcs(T, P);
  return fcts_1.sd*aconst_1.gascon;
  }

//---------------------------------------------------------------------------

double StmD(double T, double P)
  {
  CSingleLock Lock(&m_NBSLock, true); 
  T=Max(ZeroCinK, T);
  double Vt=StmVT(P);
  if (T<Vt)
    {
    double D1,D2;
    double dVtA=Min(1.0, 0.01*Vt);
    double dVtB=0.1*dVtA;
    double T1=Vt+(dVtA+dVtB);
    double T2=Vt+dVtB;
    NBSCalcs(T1, P, &D1);
    NBSCalcs(T2, P, &D2);
    return (D2+(D1-D2)*(T-T2)/dVtA)*1000;
    }
  double D;
  NBSCalcs(T, P, &D);
  return D*1000; 
  }

//---------------------------------------------------------------------------

double WtrCp(double T, double P)  
  { 
  CSingleLock Lock(&m_NBSLock, true);

  T=Max(ZeroCinK, T);
  double Vt=StmVT(P);
  if (T>Vt)
    {
    double dVtA=Min(1.0, 0.01*Vt);
    double dVtB=0.1*dVtA;
    double T1=Vt-(dVtA+dVtB);
    double T2=Vt-dVtB;
    NBSCalcs(T1, P);
    double C1=fcts_1.cpd*aconst_1.gascon;
    NBSCalcs(T2, P);
    double C2=fcts_1.cpd*aconst_1.gascon;
    return C2-(C1-C2)*(T-T2)/dVtA;
    }
  NBSCalcs(T, P);
  return fcts_1.cpd*aconst_1.gascon;
  }

//---------------------------------------------------------------------------

double WtrH(double T, double P)  
  { 
  CSingleLock Lock(&m_NBSLock, true);

  T=Max(ZeroCinK, T);
  double Vt=StmVT(P);
  if (T>Vt)
    {
    double dVtA=Min(1.0, 0.01*Vt);
    double dVtB=0.1*dVtA;
    double T1=Vt-(dVtA+dVtB);
    double T2=Vt-dVtB;
    NBSCalcs(T1, P);
    double H1=fcts_1.hd*aconst_1.gascon*T1;
    NBSCalcs(T2, P);
    double H2=fcts_1.hd*aconst_1.gascon*T2;
    return H2-(H1-H2)*(T-T2)/dVtA;
    }
  NBSCalcs(T, P);
  return fcts_1.hd*aconst_1.gascon*T;
  }

//---------------------------------------------------------------------------

double WtrS(double T, double P)
  { 
  CSingleLock Lock(&m_NBSLock, true);
  T=Max(ZeroCinK, T);
  double Vt=StmVT(P);
  if (T>Vt)
    {
    double dVtA=Min(1.0, 0.01*Vt);
    double dVtB=0.1*dVtA;
    double T1=Vt-(dVtA+dVtB);
    double T2=Vt-dVtB;
    NBSCalcs(T1, P);
    double S1=fcts_1.sd*aconst_1.gascon;
    NBSCalcs(T2, P);
    double S2=fcts_1.sd*aconst_1.gascon;
    return S2-(S1-S2)*(T-T2)/dVtA;
    }
  NBSCalcs(T, P);
  return fcts_1.sd*aconst_1.gascon;
  }

//---------------------------------------------------------------------------

double WtrD(double T, double P)
  {
  CSingleLock Lock(&m_NBSLock, true); 
  
  T=Max(ZeroCinK, T);
  double Vt=StmVT(P);
  if (T>Vt)
    {
    double dVtA=Min(1.0, 0.01*Vt);
    double dVtB=0.1*dVtA;
    double T1=Vt-(dVtA+dVtB);
    double T2=Vt-dVtB;
    double D1,D2;
    NBSCalcs(T1, P, &D1);
    NBSCalcs(T2, P, &D2);
    return (D2-(D1-D2)*(T-T2)/dVtA)*1000;
    }
  double D;
  NBSCalcs(T, P, &D);
  return D*1000;
  }

//===========================================================================

static CSurface2D SfVP("NBSWaterVapourPressure", 
                         273.16,  623.0, 1000, NULL, NULL, 
                         StmVP, SfXFLogIn, SfXFLogOut);
static CSurface3D SfStmCp("NBSSteamCp", 
                         273.16, 1073.15, 200, NULL, NULL,
                         0.01, 221.0, 200, SfXFLogIn, SfXFLogOut, 
                         StmCp, NULL, NULL);
static CSurface3D SfStmH("NBSSteamEnthalpy", 
                         273.16, 1073.15, 200, NULL, NULL,
                         0.01, 221.0, 200, SfXFLogIn, SfXFLogOut, 
                         StmH, NULL, NULL);
static CSurface3D SfStmS("NBSSteamEntropy", 
                         273.16, 1073.15, 200, NULL, NULL,
                         0.01, 221.0, 200, SfXFLogIn, SfXFLogOut, 
                         StmS, NULL, NULL);
static CSurface3D SfStmD("NBSSteamDensity", 
                         273.16, 1073.15, 200, NULL, NULL,
                         0.01, 221.0, 200, SfXFLogIn, SfXFLogOut, 
                         StmD, NULL, NULL);
static CSurface3D SfWtrCp("NBSWaterCp", 
                         273.16, 623.15, 200, NULL, NULL,
                         0.01, 1000.0, 200, SfXFLogIn, SfXFLogOut, 
                         WtrCp, NULL, NULL);
static CSurface3D SfWtrH("NBSWaterEnthalpy", 
                         273.16, 623.15, 200, NULL, NULL,
                         0.01, 1000.0, 200, SfXFLogIn, SfXFLogOut, 
                         WtrH, NULL, NULL);
static CSurface3D SfWtrS("NBSWaterEntropy", 
                         273.16, 623.15, 200, NULL, NULL,
                         0.01, 1000.0, 200, SfXFLogIn, SfXFLogOut, 
                         WtrS, NULL, NULL);
static CSurface3D SfWtrD("NBSWaterDensity", 
                         273.16, 623.15, 200, NULL, NULL,
                         0.01, 1000.0, 200, SfXFLogIn, SfXFLogOut, 
                         WtrD, NULL, NULL);

// ==========================================================================
//
//
//
// ===========================================================================

const double H2OWoleMt=18.01528;

H2OEQNBEGIN(NBS_WaterHeatCapacity, "NBSWaterCp")
virtual double AtTP(double Temp, double Press) 
  { 
  return H2OWoleMt*SfWtrCp(Temp, Press);
  };
H2OEQNEND

H2OEQNBEGIN(NBS_SteamHeatCapacity, "NBSSteamCp")
virtual double AtTP(double Temp, double Press)
  { 
  return H2OWoleMt*SfStmCp(Temp, Press);
  }
H2OEQNEND

H2OEQNBEGIN(NBS_WaterEnthalpy, "NBSWaterH")
virtual double AtTP(double Temp, double Press)
  {
  return H2OWoleMt*SfWtrH(Temp, Press);
  }
H2OEQNEND

H2OEQNBEGIN(NBS_WaterEntropy, "NBSWaterS")
virtual double AtTP(double Temp, double Press)
  {
  return H2OWoleMt*SfWtrS(Temp, Press);
  }
H2OEQNEND

H2OEQNBEGIN(NBS_SteamEnthalpy, "NBSSteamH")
virtual double AtTP(double Temp, double Press)
  {
  return H2OWoleMt*SfStmH(Temp, Press);
  }
H2OEQNEND

H2OEQNBEGIN(NBS_SteamEntropy, "NBSSteamS")
virtual double AtTP(double Temp, double Press)
  {
  return H2OWoleMt*SfStmS(Temp, Press);
  }
H2OEQNEND

H2OEQNBEGIN(NBS_WaterDensity, "NBSWaterRho")
virtual double AtTP(double Temp, double Press)
  {
  return SfWtrD(Temp, Press);
  }
H2OEQNEND

H2OEQNBEGIN(NBS_SteamDensity, "NBSSteamRho")
virtual double AtTP(double Temp, double Press)
  {
  return SfStmD(Temp, Press);
  }
H2OEQNEND

H2OEQNBEGIN(NBS_WaterVapourPress, "NBSWaterVp")
virtual double AtT(double Temp) 
  {
  return SfVP(Temp);
  }
H2OEQNEND

// ==========================================================================
//
//
//
// ==========================================================================

bool TryConstructNBS(pSpecieD pSpD, LPCTSTR Str, LPCTSTR &pn, pSpDBEqn & p)
  {
  if (stricmp(Str, NBS_WaterDensity::Name())==0) 
    {
    pn=NBS_WaterDensity::Name();
    p=new NBS_WaterDensity(pSpD);
    }
  else if (stricmp(Str, NBS_SteamDensity::Name())==0) 
    {
    pn=NBS_SteamDensity::Name();
    p=new NBS_SteamDensity(pSpD);
    }
  else if (stricmp(Str, NBS_WaterHeatCapacity::Name())==0) 
    {
    pn=NBS_WaterHeatCapacity::Name();
    p=new NBS_WaterHeatCapacity(pSpD);
    }
  else if (stricmp(Str, NBS_SteamHeatCapacity::Name())==0) 
    {
    pn=NBS_SteamHeatCapacity::Name();
    p=new NBS_SteamHeatCapacity(pSpD);
    }
  else if (stricmp(Str, NBS_WaterEnthalpy::Name())==0) 
    {
    pn=NBS_WaterEnthalpy::Name();
    p=new NBS_WaterEnthalpy(pSpD);
    }
  else if (stricmp(Str, NBS_SteamEnthalpy::Name())==0) 
    {
    pn=NBS_SteamEnthalpy::Name();
    p=new NBS_SteamEnthalpy(pSpD);
    }
  else if (stricmp(Str, NBS_WaterEntropy::Name())==0) 
    {
    pn=NBS_WaterEntropy::Name();
    p=new NBS_WaterEntropy(pSpD);
    }
  else if (stricmp(Str, NBS_SteamEntropy::Name())==0) 
    {
    pn=NBS_SteamEntropy::Name();
    p=new NBS_SteamEntropy(pSpD);
    }
  else if (stricmp(Str, NBS_WaterVapourPress::Name())==0) 
    {
    pn=NBS_WaterVapourPress::Name();
    p=new NBS_WaterVapourPress(pSpD);
    }
  else
    return false;

  return true;
  }

// ==========================================================================
//
//
//
// ===========================================================================


// ==========================================================================
//
//
//
// ===========================================================================


#define DllImportExport

//--------------------------------------------------------------------------


DEFINE_TAGOBJ(NBSH2OProps);
class NBSH2OProps : public H2OPropertiesBlock
  {
  public:

    NBSH2OProps(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    
    virtual ~NBSH2OProps();
    virtual void   BuildDataDefn(DataDefnBlk & DDB)     { };
    virtual flag   DataXchg(DataChangeBlk & DCB)        { return 0; };
    virtual flag   ValidateData(ValidateDataBlk & VDB)  { return 0; };

  };

//--------------------------------------------------------------------------

IMPLEMENT_TAGOBJ(NBSH2OProps, H2OPropsGroup, NBSH2OName, "", "", TOC_SYSTEM, 
                 "NBS Props",
                 "NBS Props for Water & Steam");

NBSH2OProps::NBSH2OProps(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  H2OPropertiesBlock(pClass_, pTag, pAttach, eAttach)
  {
  m_Liq.pRho[0] = new NBS_WaterDensity(NULL);
  m_Liq.pCp[0]  = new NBS_WaterHeatCapacity(NULL);
  m_Liq.pH[0]   = new NBS_WaterEnthalpy(NULL);
  m_Liq.pS[0]   = new NBS_WaterEntropy(NULL);
  m_Liq.pVp[0]  = new NBS_WaterVapourPress(NULL);
  m_Vap.pRho[0] = new NBS_SteamDensity(NULL);
  m_Vap.pCp[0]  = new NBS_SteamHeatCapacity(NULL);
  m_Vap.pH[0]   = new NBS_SteamEnthalpy(NULL);
  m_Vap.pS[0]   = new NBS_SteamEntropy(NULL);
  m_Vap.pVp[0]  = new NBS_WaterVapourPress(NULL);

  m_Vap.Ts[0]      = 273.16;
  m_Vap.Te[0]      = 1373.16;
#pragma chCHECKIT(This Temp range for NBS Steam needs to be checked)

  }  
    
//--------------------------------------------------------------------------

NBSH2OProps::~NBSH2OProps()
  {
  delete m_Liq.pRho[0];
  delete m_Liq.pCp[0];
  delete m_Liq.pH[0];
  delete m_Liq.pVp[0];
  delete m_Vap.pRho[0];
  delete m_Vap.pCp[0];
  delete m_Vap.pH[0];
  delete m_Vap.pVp[0];

  m_Liq.pRho[0]=NULL;
  m_Liq.pCp[0]=NULL;
  m_Liq.pH[0]=NULL;
  m_Liq.pS[0]=NULL;
  m_Liq.pVp[0]=NULL;
  m_Vap.pRho[0]=NULL;
  m_Vap.pCp[0]=NULL;
  m_Vap.pH[0]=NULL;
  m_Vap.pS[0]=NULL;
  m_Vap.pVp[0]=NULL;
  }

//--------------------------------------------------------------------------

// ==========================================================================
//
//
//
// ===========================================================================
#endif
// ==========================================================================
//
//
//
// ===========================================================================
