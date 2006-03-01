
#define DEBUG3    1     // PRINT THE EQUILIBRIUM RESULTS
//#define DEBUG2    1   // PRINT THE EQUILIBRIUM RESULTS AFTER EACH ITERATION
              // AND ADDITIONAL (ln gamma) information
//#define DEBUG1    1   // PRINT THE FLOW of the PROGRAM as it solves
              // the problem
//#define DEBUG     1 // PRINT low level matrix calculations from
              // solve3-1.c mainly
#define FALSE   0
#define TRUE    !FALSE
#define MAXSPECIE   70
#define MAXELEMENT  30
#define MAXPHASE    7    /* Check "MAX_P_STREAMS" in VCSTHERM.H - Must correspond */
#define MAXIT       200
#define DMATRIX     double**
#define DARRAY      double*

enum phase      { SINGLE = 0, GAS, LIQ, SLAG1, SLAG2, SLAG3, SLAG4 };
enum path   { L1955,L429,L6714,L430,L093,L090,L096,L441,L212,L227,L24192,
          L1366,L857,RESULT };
enum min_maj  { NMINOR = -1, MINOR, MAJOR };  /* -1, 0, 1 */

#ifndef   SOLVE
extern FILE *debug_prn_file;

extern enum phase      si[MAXSPECIE];     /* phase type of species i */
extern enum min_maj    ic[MAXSPECIE];           /* ic[reaction_no] */
extern enum path       route;

extern  double *tmole,                /* total moles for each phase before the iteration */
         *tinert,               /* inert moles for each phase */
         *delta_mole,           /* change in moles for each phase per iteration */
         *tmole1,               /* total mole for each phase after the iteration */
                    /* temp. total moles per phase */
        tempt,                /* temperature */
        press,                /* pressure */
        TEST,
         *w,                    /* moles of species */
         *wt,
         *fel,          /* previous iterations chem. pot (ff[i]*molefrac i)*/
         *fe,           /* this iterations chem. pot (ff[i]*molefrac i)*/
         *ff,           /* standard chem. pot (per mole) */
         *initial_chem_pot,   /* orginal chemical potential for activity porporses */
         *ds,           /* extent of reaction i */
        **sc,                   /* stoichiometric coef. vector for species */
                    /* and temporary matrix for inest/lin_prog */
                    /* sc[component_no][reaction_no]*/
        **bm,                 /* species formula vector */
         *dg,           /* delta g for the reaction i*/
                    /* there are less reactions than species */
                    /* dg[reaction_no] */
        **d_mole,               /* phase variable used in formula 6.4-16 in book*/
                    /* is calculated in BASOPT() */
         *ga,                 /* current elemental abundance value */
         *gai,          /* element abundance input value */
         *da,           /* starting free energy storage */
             *aw,                 /* temporary variable used by basopt() and inest() */
           *sa,                   /* temporary variable used by basopt() and elcor() */
           **sm,              /* temporary variable used by basopt() and elcor() */
           *ss;                 /* temporary variable used by basopt() and elcor() */

/* specie ordering */
/*  |<--components-->|<--reactions--->|     */
/*  (1,2,.........,nc,nc1,.........,mr)     */
/*                  mr = m if no species was deleted */
/*           nc = ne (number of elements) */
/* number of reactions => nr = mr - nc      */
/* number of orginal reactions => n = m - ne  */
/*                  = nr + no of deleted species */
/* deleted species is from mr+1 to m      */

extern  int     m,                              /* number of species in input */
        ne,                             /* number of elements */
        no_phases,                      /* number of phases */
        n,                              /* equals m - nc */
        nc1,                            /* equals nc + 1 */
        iter,                           /* iteration number */
        iti,              /* iteration variable */
        it1,              /* iteration variable */
        nr,                             /* number of starting reactions */
        no_specie_phase[MAXPHASE],      /* number of species per phase */
        nc,                             /* number of components */
        mr,                             /* number of current reactions */
        minor,                          /* number of minors */
        type,                           /* type of chemical pot */
        ir[MAXSPECIE],                  /* index to reaction species */
        ind[MAXSPECIE],         /* index to orginal specie input array */
        index2[MAXSPECIE],        /* second index to show new position */
                        /* relative to the orginal position */
        nopt,                           /* no off basis optimisation */
        IM,                             /* flag - if true all reactions are MINORS */
        LBO,                            /* flag */
        LEC,                            /* flag */
        ILT,                            /* flag - if true no conver between minors */
                        /*      - if false conver between majors (see equi_chk_min/maj) */
        FORCED,                         /* flag */
        CONV,                           /* convergence flag */
        ICONV,              /* convergence flag */
        iest,                           /* user/machine estimate */
        ITL,                            /* ds[] step too large do convergence forcing */
        sc_max_i,           /* maxinum i dimension of array sc */
        sc_max_j;           /* maxinum j dimension of array sc */

#endif
#if defined(SOLVE1) || defined(VCSTLNK)
extern jmp_buf e_buf;
#endif
extern char error[];


#ifndef TRANSFER
extern double alpha;
#endif
/* --------- routines in solve3-0.c --------- */
void solve(void);
void initial_data(void);
enum path init(void);
enum path chk_opt_basis(void);
enum path eval_init_vect(void);
enum path calculate(void);
enum path equi_chk_maj(void);
enum path equi_chk_min(void);
enum path elem_abund(void);
enum path chk_del_specie(void);
enum path calc_result(void);
void prin_immed(void);

/* --------- routines in solve3-1.c ---------- */
int deleteit(int L,int mm);
void force(void);
int  st2(void);
void elcorr(void);
void deltag(int L, int j, int kp);
void basopt(int IFIRST);
void elab(void);
void dfe(DARRAY z, int kk, int ll);
void mlequ(DMATRIX c, DMATRIX b, int m);
void swap2(int x[], int j1, int j2);
void rotate(int x[], int j1, int j2);
int amax(DARRAY x, int j, int n);
void dsw(DARRAY x, int j1, int j2);
void dsw2(DMATRIX x, int j1, int j2);
void inest(void);
void lin_prog(DARRAY psol, DMATRIX ax);


/*------------ routines in ascthlnk.c --------- */
void read_data(void);
void trans_mole_spread(void);
void get_activity_coef(void);
