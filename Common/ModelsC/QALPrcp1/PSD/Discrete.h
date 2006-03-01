extern int eqns_to_solve__(doublereal *y, doublereal *yin, doublereal *g, doublereal *nuc, doublereal *agg, doublereal *res_time__, doublereal *grow_coeff__, doublereal *aggl_coeff__, integer *q, integer *npts, doublereal *yp);
extern int pure_growth__(doublereal *dist, doublereal *grow_coeff__, integer *npts, doublereal *distp);
extern int pure_agglom__(doublereal *dist, integer *q, integer *npts, doublereal *aggl_coeff__, doublereal *distp);
extern int calc_growth_coeff__(doublereal *x, integer *npts, integer *grow_dis__, doublereal *grow_coeff__);
extern int calc_agglom_coeff__(doublereal *x, doublereal *akf, integer *npts, doublereal *aggl_coeff__);
extern int set_grid__(doublereal *xmin, integer *q, integer *npts, doublereal *x);
extern doublereal moment_(doublereal *dist, integer *j, integer *npts, doublereal *x);
extern int number_to_mass__(doublereal *dist, integer *npts, doublereal *x);
extern int dist_sauter__(doublereal *psd, doublereal *l32, integer *npts, doublereal *x);
