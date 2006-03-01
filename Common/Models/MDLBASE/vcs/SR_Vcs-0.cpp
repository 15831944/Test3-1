#define SOLVE 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <conio.h>
#include "SR_vcs.h"



enum phase    si[MAXSPECIE];        /* phase type of species i */
enum min_maj    ic[MAXSPECIE];
enum path   route;


double  *tmole = NULL,             /* total moles for each phase */
    *tinert = NULL,            /* inert moles fpr each phase */
    *delta_mole = NULL,        /* delta moles for each phase */
    *tmole1 = NULL,            /* total mole1 for each phase */
    tempt,              /* temperature */
    press,        /* pressure */
    TEST,
    *w = NULL,          /* moles of species */
    *wt = NULL,
    *fel = NULL,
    *fe = NULL,
    *initial_chem_pot = NULL, /* orginal chemical potential for activity porporses */
    *ff = NULL,
        *ds = NULL,
    **sc = NULL,               /* stoichiometric coef. vector for species */
              /* and temporary matrix for inest/lin_prog */
    **bm = NULL,
    *dg = NULL,
    **d_mole = NULL,
    *ga = NULL,
    *gai = NULL,
    *da = NULL;

double  *aw = NULL,   /* temporary variable used by basopt() and inest() */
    *sa = NULL,        /* temporary variable used by basopt() and elcor() */
    **sm = NULL,    /* temporary variable used by basopt() and elcor() */
    *ss = NULL;   /* temporary variable used by basopt() and elcor() */

int     m,                /* number of species in input */
    ne,                             /* number of elements */
    no_phases,                      /* number of phases */
    n,                /* equals m - nc */
    iter,                         /* iteration number */
    iti,
    it1,
    nr,
    no_specie_phase[MAXPHASE],    /* number of species per phase */
    nc,                             /* number of components */
    mr,                             /* number of reactions species taking */
                    /* part in the system, mr to m is the */
                    /* deleted species */
    minor,                          /* number of minors */
    type,                           /* type of chemical pot */
    ir[MAXSPECIE],                  /* index to reaction species */
    ind[MAXSPECIE],         /* index to orginal specie vector */
                    /* pointing to orginal position */
    index2[MAXSPECIE],        /* second index to show new position */
                    /* relative to the orginal position */
    nopt,             /* no off basis optimisation */
    LIQUID,
    IM,                             /* flag - if true all reactions are MINORS */
        LBO,              /* flag */
        LEC,              /* flag */
    ILT,                            /* flag - if true no conver between minors */
                    /*      - if false conver between majors (see equi_chk_min/maj) */
    FORCED,             /* flag */
    CONV,             /* convergence flag */
    ICONV,              /* Signal no convergence (iterations > MAXIT) */
    iest,             /* user/machine estimate */
    ITL,              /* flag */
    sc_max_i = MAXELEMENT+2,    /* maxinum i dimension of array sc */
    sc_max_j = MAXSPECIE+MAXELEMENT;/* maxinum j dimension of array sc */




#ifdef DEBUG1
char  *debug[14] = {"L1955-init()","L429-basopt()","L6714- bad exit","L430-deltag(0,1,nr)",
          "L093-init for each iter.","L090- calculate()","L096-elem_abund()","L441-chk_opt_basis()",
          "L212-equi_chk_maj()","L227-equi_chk_min()","L24192-eval_init_vect()",
          "L1366-chk_del_specie()","L857-calc_result()","RESULT"};
#endif

void solve(void)
{
  initial_data();
  route = L1955;
  #ifdef DEBUG1
  fprintf(debug_prn_file,"\n%s",debug[route]);
  #endif
  do 
   {
    switch ( route ) 
       {
      case  L1955   : init();
                  break;
      case  L429    :   /* determine basis species, evaluate sioichiomerty */
                  basopt(FALSE);
                  if ( ! CONV ) 
                                   {
                    route = L24192;
                    break;
                  }
                  #ifdef DEBUG1
                  else 
                                   {
                    route = L6714;
                    break;
                  }
                  #endif
                    /* else fall through */
      case  L6714   : strcpy( error, " Convergence to number of positive N(I) less than C. \n");
                  strcat( error, " Results to follow - check carefully \n");
                  route = L857;     /* print results */
                  break;
      case  L430    : deltag(0,1,nr);   /* evaluate all reaction free energy changes */
                  if (!LBO) { route = L212; break;}
                  LBO = FALSE;
                  if (IM) 
                                   {    /* reaction entirely between minors */
                    iti = 0;
                    route = L090;
                    break;
                  }
                  #ifdef DEBUG1
                  else 
                                   {
                    route = L093;
                    break;
                  }
                  #endif
                        /* else fall through */
      case  L093    : iti = 4*(it1/4)-it1;  /* set initial values for iteration */
                  if (iti != 0) 
                                   { 
                                      route = L090; 
                                      break;
                                   }
                  dfe(w,0,1);     /* for tmole and all components + minor reactions only*/
                  deltag(1,1,nr);     /* minors only */
                  #ifdef DEBUG1
                  fprintf(debug_prn_file,"\nCalled 'dfe()' and 'deltag()' for minors");
                  route = L090;
                  break;
                  #endif
                        /* fall through */
      case  L090    : calculate();
                  /* THIS IS A BDB MOD TO THE CODE TO STOP THE PROGRAM AFTER MAXIT
                     AS WITH ALPHA SET IT SOMEHOW MISSES 'equi_chk_maj' IF ALL THE
                     REACTIONS ARE AMONG THE MINORS */
                  if( iter > MAXIT )
                  {
                     ICONV = TRUE;
                     route = L857;
                     break;
                  }
                  break;
      case  L096    : elem_abund(); /* evaluate final/intermediate element abundances */
                  break;
      case  L441    :   chk_opt_basis();
                  break;
      case  L212    :   /* Check for equilibrium among the reactions */
                  if (IM) 
                                   {                   /* true if all the reactions are minor */
                    route = L227;   /* thus only check minors for equilib */
                    break;
                  }
                  equi_chk_maj(); /* check equil of major species */
                  break;
      case  L227    : equi_chk_min(); /* check equil of minor species */
                  break;
      case  L24192    : eval_init_vect();   /* evaluate initial major-minor vector */
                  break;
      case  L1366   : chk_del_specie(); /* re-check deleted spesies */
                  break;
      case  L857    : calc_result();    /* final delta G/RT values */
                  break;
     }
     #ifdef DEBUG1
     fprintf(debug_prn_file,"\n%s",debug[route]);
     #endif
  }while( route != RESULT );
}

/* ----------- start of iteration loop -------------- */
enum path init(void)
{
  int i;

  for (i = 1; i <= no_phases; i++)
    tmole[i] = tinert[i];
  for (i = 1; i <= mr; i++)
    tmole[si[i]] += w[i];           /* add moles of species for each phase */
  tmole[0] = 0.0;           /* total moles for Single specie phase */
                    /* is unimportant */
  dfe(w,0,0);             /* for tmole and all components and reactions. */
  if (iter > 0) return (route = L430);  /* evaluate all reaction free energy changes */
  if (iest != 0) return (route = L441); /* chk for optimum basis when machine estimate */
  return (route = L429);
}


/* ---------- evaluate initial major-minor vector ----------- */
enum path eval_init_vect(void)
{
  int L,i;

  it1 = 1;
    ILT = FALSE;
  minor = 0;
  for (i = 1; i <= nr; i++) 
   {        /* check each reaction for major / minors */
    L = ir[i];
    if (w[L] > 0.0) 
           ic[i] = MAJOR;
    else 
       {
      minor++;
      ic[i] = NMINOR;
    }
  }
  LEC = FALSE;         /* Set equilibrium check flag */
  LBO = TRUE;
  if (iter > 0) 
       return (route = L096); /* not the first iteration */
  return (route = L430);     /* evaluate all reaction free energy changes */
}


/* ----------------- This is the actual reaction solver ----------------------*/
/* --- calculates wt[L] = w[L] + par * ds[L] ---------------------------------*/
enum path calculate(void)
{
  int i,lk,               /* current stoichiometric reaction */
    L,k,xm;

  double  par,xx,c,dgg,ww,dx;

  for (i = 1; i <= mr; i++) 
         fel[i] = fe[i];
  if ( !(ILT || IM) )    /* IM => True if all reactions are minors */
   {
    #ifdef DEBUG1
    fprintf(debug_prn_file,"\nCalling 'st2(SOLDEL) from 'calculate()'");
    #endif
    if (FALSE == st2())                   /* calculates ds[] - delta sigma */
       {    
      #ifdef DEBUG1
      fprintf(debug_prn_file,"\nNOT FOLLOWING CONVENTIONAL ROUTE - exiting 'calculate()'");
      #endif
      return (route = L429);            /* do basis optimisation again */
    }
  }
  ITL = FALSE;                              /* Set equilibrium check flag */
  LEC = FALSE;
  for (i = 1; i <= no_phases; i++)
    delta_mole[i] = 0.0;                  /* clear the mole differences per phase */
  for (i = 1; i <= nc; i++) ds[i] = 0.0;    /* clear the extents of the components */
  i = 1;
  do 
   {                                         /* do for each reaction i */
    L = ir[i];
    switch( ic[i] ) 
       {
      case MAJOR  : if ( fabs( dg[i] ) <= 1e-6 ) 
                           {                           /* reaction in equilibrium */
                wt[L] = w[L];     /* cancel adjustments */
                ds[L] = 0.0;
                break;
              }
              else 
                           {                           /* reaction not in equilibrium */
                dx = ds[L];
                wt[L] = w[L] + dx;      /* adjust reaction by extent */
                if ( wt[L] > 0.0 )      /* check for positive major species */
                               {   
                    /* adjust component moles of the reaction accordingly */
                  for ( k = 1; k <= nc; k++) 
                                       ds[k] += sc[k][i]*dx;
                  for ( k = 1; k <= no_phases; k++)
                    delta_mole[k] += d_mole[k][i] * dx;
                  break;
                }
                else 
                               {            /* non positive major species */
                  if ( si[L] != SINGLE ) 
                                   {
                    ds[L] = dx = -0.9*w[L]; /* cut reaction adustment to 1/3 */
                    wt[L] = 0.1*w[L];
                                        /* adjust component moles of the reaction accordingly */
                    for ( k = 1; k <= nc; k++) 
                                           ds[k] += sc[k][i]*dx;
                    for ( k = 1; k <= no_phases; k++)
                      delta_mole[k] += d_mole[k][i] * dx;
                                       /* is reaction greater than smallest component/100 */
                    if ( w[L] >= 0.01*w[nc] )
                                       {
                      break;  /* do next reaction */
                    }
                    else 
                                       {
                                           ic[i] = MINOR;     /* change major to minor */
                                           #ifdef DEBUG1
                      fprintf(debug_prn_file,"\nChanged major %3d to minor",i);
                      #endif
                      /* fall through to handel minors */
                    }
                  }
                  else 
                                   {                  /* set single-specie phase to zero */
                    dx = -w[L];
                    /* calculate extent so that the components stay positive */
                    for (k = 1; k <= nc; k++) 
                                       {
                      wt[k] = w[k] + sc[k][i]*dx;
                      if (wt[k] <= 0.0)       /* reset loop counter and do it again */
                                           { 
                                              k = 1; 
                                              dx = dx/2; 
                                           } 
                    }
                    /* set final moles immediately */
                                       w[L] += dx;
                    for (k = 1; k <= nc; k++)  
                                           w[k] = wt[k];
                    for ( k = 1; k <= no_phases; k++)
                      tmole[k] += d_mole[k][i] * dx;
                    wt[L] = w[L];
                    ds[L] = 0.0;
                    if ( w[L] > 0.0 )           /* if posetive mass exit */
                                           break;
                    ic[i] = NMINOR;             /* negative -> change major to neg. minor */
                    #ifdef DEBUG1
                    fprintf(debug_prn_file,"\nChanged major %3d to negative minor",i);
                    #endif
                    dfe(w,0,iti);   /* for tmole and components + all reactions/majors only */
                    deltag(iti,1,nr);   /* majors or all reactions */
                    for (k = 1; k <= mr; k++) 
                                           fel[k] = fe[k];
                  }
                                   /* if we got here we changed a major to a minor specie */
                  minor++;          /* correct total minors */
                  IM = (minor == nr);
                  if ( IM && (iti != 0) )   /* all reactions are minors */
                    return (route = L227);  /* exit & do equilib check for MINOR */
                  break;
                }
              }
              /* a MINOR reaction is a specie smaller 
                              than smallest component/100
                              which is part of a multi specie phase */
      case MINOR  : if (iti == 0 )  /* we do this only every fourth iteration */
                           {
                dgg = dg[i];
                ww = w[L];
                if ( dgg < 82.0 ) 
                               {
                  if ( fabs(dgg) <= 1e-5 ) 
                                   {                          /* reactions in equilibrium */
                    wt[L] = w[L];     /* delete adjustments */
                    ds[L] = 0.0;
                    break;
                  }
                  else 
                                   {
                    c = log((double)ww)-dgg;
                    if ( c > - 73.6) 
                                       {  /* limit wt[L] to a minimum of e-32 moles */
                      wt[L] = exp(c); /* equation 9.5-1 in book */
                      if ( wt[L] > w[1] )
                        wt[L] = ww * 1e6; /* new minor moles greater */
                                  /* than biggest component */
                      ds[L] = dx = wt[L] - ww;
                      if ( ww <= (-dx) ) 
                                               ITL = TRUE;      /* step too large - force */
                      /* adjust component moles of the reaction accordingly */
                      for ( k = 1; k <= nc; k++) ds[k] += sc[k][i]*dx;
                      for ( k = 1; k <= no_phases; k++)
                        delta_mole[k] += d_mole[k][i] * dx;
                      break;
                    }
                  }
                }
                /* thus dg[i] >= 82 or (dg[i] < 82 and c < -73.6) */
                if (ww >= 1e-26) 
                               {
                  wt[L] = ww * 1e-6;
                  ds[L] = dx = -0.999999*w[L];
                  /* adjust component moles of the reaction accordingly */
                  for ( k = 1; k <= nc; k++) ds[k] += sc[k][i]*dx;
                  for ( k = 1; k <= no_phases; k++)
                    delta_mole[k] += d_mole[k][i] * dx;
                  break;
                }
                else 
                               {  /* delete reaction specie because < 1e-26 */
                  #ifdef DEBUG1
                  fprintf(debug_prn_file,"\nCalling 'delete(L,0)' from 'calculate'");
                  #endif
                  if (!deleteit(L,0)) 
                                   {
                    /* cannot delete because it was the last reaction */
                    #ifdef DEBUG1
                    fprintf(debug_prn_file,"\n   EXITING 'calculate()' to re-check deleted specie");
                    #endif
                    return (route = L1366); /* exit & re-check deleted species */
                  }
                  #ifdef DEBUG1
                  fprintf(debug_prn_file,"\nReturned from 'delete(L,0)' to 'calculate'");
                  #endif
                  minor--;
                  break;
                }
              }
              else ds[L] = 0.0 ; /* only calculate MINORS every 4 th iteration */
              break;
              /* NMINOR is a zero mole single-specie phase */
      case NMINOR :   if  (dg[i] < 0.0) 
                           {  /* Check if we can make it a major */
                minor--;
                ic[i] = MAJOR;
                IM = FALSE;
                ILT = FALSE;
                #ifdef DEBUG1
                fprintf(debug_prn_file,"\nChanged nminor %3d to major",i);
                #endif
              }
              wt[L] = w[L];   /* delete any adjustments */
              ds[L] = 0.0;
              break;
       }  /* end of switch() */
    i++;
  } while ( i <= nr);
  /* -- limit reduction of components to 99 % of its original moles - */
  /* -- thus calculate the step size parameter such that the components -*/
  /* -- stay positive -- */
  par = 0.5;
  for (i = 1; i <= nc; i++) 
   {
    xx = -ds[i]/w[i];
    if (par < xx) par = xx;
  }
  par = 1.0/par;              
  if ( par <= 1.01 && par > 0.0 )         /* reduction in basis too large */
   {                                    /* par was greater than 1 */
    par = 0.99*par;                     /* reduce overall step size */
    ITL = FALSE;            /* step too large -> force */
    for (i = 1; i <= mr; i++)
       ds[i] *= par;
    for (i = 1; i <= no_phases; i++)
      delta_mole[i] *= par;
    lk = mr;
  }
  else 
   {
    lk = nc;
  }
   /* At last adjust the reaction specie mass by the calculated extent */
  for (i = 1; i <= lk; i++)
    wt[i] = w[i] + ds[i];
  for (i = 1; i <= no_phases; i++)            /* also do it for the phases */
    tmole1[i] = tmole[i] + delta_mole[i]; /* new total moles */
  /* ------- temporary chemical potentials -------- */
  dfe(wt,1,iti);  /* for tmole1 and components + all reactions/majors only */
  FORCED = FALSE;
  if ( !(IM || ILT || ITL) ) 
   {
    #ifdef DEBUG1
    fprintf(debug_prn_file,"\nCalling 'force()' from 'calculate()'");
    #endif
    force();  /* convergence forcer */
  }
  /* --------- reset values at end of iteration --------------------- */
  /* --------- calculate new reaction free energy changes ----------- */
  if (!FORCED) 
   {
    #ifdef DEBUG1
    fprintf(debug_prn_file,"\nCONVERGENCE WAS NOT FORCED!!!!!!!");
    #endif
    for (i = 1; i <= no_phases; i++)
      tmole[i] = tmole1[i];
    for ( k = 1; k <= mr; k++ ) 
      if (ds[k] != 0.0) 
               w[k] = wt[k];
  }
  #ifdef DEBUG2
  prin_immed(); /* print intermediate results */
  fprintf(debug_prn_file,"\nTransfering data to spreadsheet");
  #endif
  trans_mole_spread();
  if (alpha != 0.0) 
   {
    get_activity_coef(); /* getting new chem. pot. in ff[] */
    dfe(w,0,iti); /* for tmole and components + all reactions/majors only */
  }

  /* exit  if escape key was pressed */
/*  if (kbhit() && (getch() == 27)) 
   {
    ICONV = TRUE;
    return (route = L857);
  }
*/
  deltag(iti,1,nr); /* iti = 0 ->all reactions from 1 to nr*/
            /* else for major reactions only */
  iter++;
  it1++;
  /* ----------- set  microscopic multispecies phase (tmole < 1e-10) to zero ------ */
  xm = 0;
  for (i = 1; i <= no_phases; i++) 
   {
    if( ((tmole[i]-tinert[i]) < 1e-10) && no_specie_phase[i] != 0 && tmole[i] != 0.0 ) 
       {
      #ifdef DEBUG1
      fprintf(debug_prn_file,"\n DELETING PHASE %3d - total moles less than 1e-10",i);
      #endif
      for (k = 1; k <= mr; k++) 
           {
        if ( si[k] == i ) 
               {
          w[k] = 0.0;
          wt[k] = 0.0;
          ds[k] = 0.0;
        }
      }
      tmole[i] = 0.0;
      tmole1[i] = 0.0;
      delta_mole[i] = 0.0;
      xm = 1;
    }
  }
  if (xm == 0) 
       return (route = L441);      /* no phase deleted */
                  /* exit and check for optimum basis, convergence */
  /* a phase was deleted, check the reactions */
  for (k = 1; k <= nr; k++)
    if (w[(ir[k])] == 0.0) 
       {
      ic[k] = NMINOR;
      #ifdef DEBUG1
      fprintf(debug_prn_file,"\nChanged major %3d to negative minor",k);
      #endif
    }
  basopt(FALSE);  /* do basis optimisation and calculate stioch. matrix */
  dfe(w,0,0);     /* for tmole and components + all reactions */
  deltag(0,1,nr); /* all reactions from 1 to nr */
  if( CONV ) 
      return ( route = L6714 ); /* the biggest specie is zero */
                    /* - no optimum basis in basopt */
  else 
      return( route = L441);       /* exit and check for optimum basis */
}

/* ----------- check for optimum basis --------------------- */
/* ----------- then check for convergence ------------------ */
enum path chk_opt_basis(void)
{
  int i,j,k,jj,         /* counters */
    L;

  k = 2;
  while ( (w[k-1] >= w[k]) && (k <= nc)) k++;
  if ( (k > nc) && (nc != 1) ) 
   {
    for ( k = 1; k <= nr; k++ ) 
       {                               /* check if reactions mass is larger than */
      L = ir[k];                  /* component mass */
      j = nc;
      while ( (w[L] > w[j]) && (j > 0) ) 
           {
        if ( sc[j][k] != 0.0 )              /* no optimum basis */
          return (route = L429);      /* recalc basis */
        j--;
      }
    }
  }
  else 
   {
    for ( j = 1; j <= nr; j++ ) 
       {                               /* check if components are sorted in decreasing */
      L = ir[j];                  /* masses */
      for (jj = 1; jj <= nc; jj++) 
           {
        if ( (w[L] > w[jj]) && (sc[jj][j] != 0.0) )  /* no optimum basis */
          return (route = L429);            /* recalc basis */
      }
    }
  }
  if (iter == 0) 
       return ( route = L24192);                /* this will only occur when user estimate is used */
                          /* so evaluate the major-minor vector */
  if (iti != 0) 
        return (route = L212);                  /* check for equilibrium among major reactions */
  /* ------------- re-evaluate major-minor vector when minors are evaluated ------------- */
  minor = 0;
  for ( jj = 1; jj <= nc; jj++) 
         ds[jj] = 0.01*w[jj]; /* set extent of reaction for all components to 1/100*w[i] */
  for (i = 1; i <= nr; i++) 
   {        /* check each reaction */
    L = ir[i];
    if ( w[L] <= 0.0 ) 
       {                          /* negative moles */
      if ( dg[i] > 0.0) 
           {                      /* but posetive delta G ?? */
        ic[i] = NMINOR;
        #ifdef DEBUG1
        fprintf(debug_prn_file,"\nChanged %3d  to nminor in 'chk_opt_basis'",i);
        #endif
        minor++;
      }
      else 
           {             /* chemical potential for new major is negative */
        ic[i] = MAJOR;
        #ifdef DEBUG1
        fprintf(debug_prn_file,"\nChanged %3d  to major in 'chk_opt_basis'",i);
        #endif
      }
    }
    else 
       {                   /* positive moles */
      if ( si[L] != SINGLE ) 
           {
        for (jj = 1; jj <= nc; jj++)
          if ( (si[jj] != SINGLE && sc[jj][i] != 0.0 && w[L] >= ds[jj]) && (jj <= nc) )
            goto jump;
        if  ( w[L] < 0.1*tmole[si[L]] ) 
               {
              ic[i] = MINOR;
              #ifdef DEBUG1
              fprintf(debug_prn_file,"\nChanged %3d  to minor in 'chk_opt_basis'",i);
              #endif
              minor++;
        }
        else 
               {
  jump:               ic[i] = MAJOR;
            #ifdef DEBUG1
            fprintf(debug_prn_file,"\nChanged %3d  to major in 'chk_opt_basis'",i);
            #endif
        }
      }
      else 
           {                     /* for single - specie phases */
        ic[i] = MAJOR;
        #ifdef DEBUG1
        fprintf(debug_prn_file,"\nChanged %3d  to major in 'chk_opt_basis'",i);
        #endif
      }
    }
    }
  IM = (minor == nr);    /* Set IM true if all the reactions are minor */
  return ( route = L212 ); /* equilibrium check for majors */
}

/* THE FOLLOWING FUNCTIONS CAN STILL BE OPTIMISED */
/* ---------------- CHECK FOR CONVERGENCE -----------------------*/
/* -----------equilibrium check for major species --------------- */
enum path equi_chk_maj(void)
{
  int i;              /* loop counter */

  if ( iter >= MAXIT) 
   {
    ICONV = TRUE;
    return ( route = L857 );  /* print results */
  }
  for (i = 1; i <= nr; i++) 
   {
    if ( (ic[i] > MINOR && fabs(dg[i]) > 1e-6) ) 
       {
      ILT = FALSE;
      return ( route = L093 );  /* pre-set and calculate again */
    }
  }
  /* ----- equilibrium among majors => no check for minors -------- */
  if (minor == 0) 
       return ( route = L096 ); /* check element abundances and maybe exit */
  else 
       return (route = L227);   /* equilibrium check for minor species */
}

/* -----------equilibrium check for minor species --------------- */
enum path equi_chk_min(void)
{
  int i;              /* loop counter */

  if (iti == 0) 
       deltag(1,1,nr);   /* minors only */
  else 
   {
    dfe(w,0,1); /* for tmole and components + minor reactions only */
    deltag(1,1,nr);         /* minors only */
  }
  if ( iter >= MAXIT ) 
   {
    ICONV = TRUE;
    return ( route = L857 );  /* print results */
  }
  for (i = 1; i <= nr; i++) 
   {
    if ( (ic[i] == MINOR && fabs(dg[i]) > 1e-5) ) 
       {
      ILT = TRUE;         /* convergence among majors */
                    /* but not among minors */
      iti = 0;
      return (route = L090);  /* calculate again */
    }
  }
  /* ------ equilibrium among minors as well --------- */
  return ( route = L096 );  /* check element abundances */
}

/* ---------- evaluate final/intermediate element abundances --------------- */
enum path elem_abund(void)
{
  int i;          /*loop counter */

  elab();
  #ifdef DEBUG1
  fprintf(debug_prn_file,"\n\nElemental abundances");
  for (i =1; i <= ne; i++) 
       fprintf(debug_prn_file,"\n           %1d    %+.7e",i,ga[i]);
  #endif
  if (LEC) 
   {      /* if the bottom loop was already done */       
    for ( i = 1; i <= nc; i++) 
       {
      if ( gai[i] == 0.0) 
           {               /* zero elements inputed */
        if (fabs(ga[i]) > 1e-10) 
               {
          elcorr();           /* correct element abundances */
          return ( route = L1955 );    /* go to beginning of iteration cycle */
        }
      }
      else 
           {
              /*---- changed 0.5e-8 to 0.5e-6 ---- */ 
              /* -- C floats are 7 digit precision */
        if ( fabs(ga[i] - gai[i]) > 0.5e-8*gai[i]) 
               {
          elcorr();           /* correct element abundances */
          return ( route = L1955 );    /* go to beginning of iteration cycle */
        }
      }
    }
    if(mr == m) 
          return ( route = L857 );  /* no deleted species -> print results */
/* THIS IS THE END OF THE EQUILIBRIUM CALCULATIONS */
    else 
          return ( route = L1366 );     /* re-check deleted species */
  }
  else 
   {
    LEC = TRUE;           /* must go through this loop before the above loop */
    elcorr();           /* correct element abundances */
    return ( route = L1955 );    /* go to beginning of iteration cycle */
  }
}

/* ---------- re-check deleted species ------------------------- */
enum path chk_del_specie(void)
{
  int i,k,        /* loop counter */
    mr1,nr1,npb,
    L;          /* index */

  double xt[MAXPHASE];

  mr1 = mr + 1;
  nr1 = nr + 1;
  for (i = mr1; i <= m; i++) /* from the last reaction to the last defined specie */
    fe[i] = ff[i];
  deltag(0,nr1,n);        /* all reactions */
  npb = 0;
  for (k = 1; k <= no_phases; k++)
    if ( tmole[k] > 0.0 ) xt[k] = log((double)1e32 * tmole[k]);
  for ( i = nr1; i <= n; i++) 
   {
       L = ir[i];
        k = si[i];
     if ( (tmole[k] > 0.0 && dg[i] < xt[k]) || (tmole[k] == 0.0 && dg[i] < 0.0) ) 
        {
      w[L] = 1e-20;
      tmole[k] += w[mr];
      ic[i] = MINOR;
      minor++;
      nr++;
      mr++;
      npb = 1;
      if (deleteit(L,1) == FALSE) 
              return (route = L1366); /* possibillity of an unlimited loop */
    }
  }
  if (npb == 0) 
      return (route = L857);    /* print results */
  else 
   {
    ILT = FALSE;
    dfe(w,0,1); /* for tmole and components + minor reactions only */
    deltag(0,nr1,nr);   /* all reactions */
    iti = 0;
    return ( route = L090 );
  }
}

