#define SOLVE1   1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include "sr_vcs.h"



int deleteit( int L, int mm )
{
  int    nnl,k;
  double temp,t1;


  nnl = L - nc;
  if ( L != mr) 
   {    /* re-arrange data when specie  added or removed */
    dsw(wt,mr,L);
    dsw(w,mr,L);
    dsw(ff,mr,L);
    dsw(initial_chem_pot,mr,L);
    if ( si[mr] == SINGLE ) 
           dsw(fe,mr,L);
    swap2((int *)si,mr,L);
    dsw(fel,mr,L);
    swap2((int *)ic,nr,nnl);
    dsw(da,mr,L);       /* this seems unnecessary */
    swap2(ind,mr,L);
    rotate(index2, mr, L);  
    for ( k = 1; k <= nc; k++) 
       {
      t1 = sc[k][nr];
      sc[k][nr] = sc[k][nnl];
      sc[k][nnl] = t1;
    }
    for ( k = 1; k <= no_phases; k++) 
       {
      temp = d_mole[k][nr];
      d_mole[k][nr] = d_mole[k][nnl];
      d_mole[k][nnl] = temp;
    }
    for ( k = 1; k <= ne; k++) 
       {
      temp =bm[mr][k];
      bm[mr][k] = bm[L][k];
      bm[L][k] = temp;
    }
  }
  if (mm != 0) 
       return (TRUE);
  /* ------- extra procedures when removing a species --------------- */
  dsw(dg,nr,nnl);
  dsw(ds,mr,L);
  /* -------- special procedures for removing a phase --------------- */
  tmole[si[mr]] -= w[mr];
  nr--;                           /* one less reaction */
  mr--;                           /* one less reaction */
  if ( nr == 0 ) 
       return ( FALSE );
  else 
       return ( TRUE );
}

/* ----------- convergence forcer ----------------- */
void force(void)
{
  int   i;
  double  s1,s2,al;

  /* ------- calculate slope at end of step ----------- */
  s2 = 0.0;
  for ( i = 1; i <= mr; i++ )
    s2 += fe[i]*ds[i];
  if ( s2 <= 0.0 ) 
       return;
    /* -------- calculate original slope ---------------- */
  s1 = 0.0;
  for ( i = 1; i <= mr; i++ )
    s1 += fel[i]*ds[i];
  if ( s1 >= 0.0 ) 
       return;
  /* -------- fit parabola -------------- */
  al = s1 / (s1 - s2);
  if (al >= 0.95) 
       return;
  /* ------- adust mole numbers, chemical potential ------------ */
  for (i = 1; i <= mr; i++)
    if ( ds[i] != 0.0 ) 
       {
      ds[i] *= al;
      w[i] += ds[i];
    }
  for (i = 1; i <= no_phases; i++)
    tmole[i] += al*delta_mole[i];
  dfe(w,0,iti);  /* for tmole and components + all reactions/majors only */
  FORCED = TRUE;
  return;
}


/* ---------- calculates reaction adjustments (delta sigma) -- */
/* ------ dg[i] must be calulated before using this routine -- */
int st2(void)
{
  int i,j,L,k,SOLDEL;
  double s,dss,xx;

  SOLDEL = FALSE;
   for ( i = 1; i <= nr; i++) 
    {
    L = ir[i];
    if ( w[L] == 0.0 && si[L] != SINGLE ) 
       {
      if ( dg[i] >= -1e-4 ) 
              ds[L] = 0.0;
      else 
           {
        ds[L] = 1e-10;
        ic[i] = MAJOR;
      }
    }
    else if ((fabs(dg[i]) <= 1e-6) || (ic[i] <= MINOR && dg[i] >= 0.0) ) 
       {}
    else 
       {
      /* ------- this is formula 6.4-16 in VCS manual -------- */
      if ( si[L] == SINGLE ) 
              s = 0.0;
      else 
              s = 1.0/w[L];
      for ( j = 1; j <= nc; j++)
        if (si[j] != SINGLE) 
                  s += sc[j][i]*sc[j][i]/w[j];
      for (j = 1; j <= no_phases; j++)
        if (tmole[j] > 0.0) 
                  s -= d_mole[j][i]*d_mole[j][i]/tmole[j];
      if ( s != 0.0 ) 
                  ds[L] = -dg[i]/s;
      /* --------- reaction entirely among condensed phases ----- */
      /* --------- delete one solid and recompute basis --------- */
      else 
           {
        #ifdef DEBUG1
        fprintf(debug_prn_file,"\n Reaction entirely among condensed phases in 'st2()'");
        #endif
        if ( dg[i] <= 0.0) 
               {
          dss =1e10;
          for (j = 1; j <= nc; j++) 
                   {
            if ( sc[j][i] < 0.0 ) 
                       {
              xx = -w[j]/sc[j][i];
              if ( xx < dss) 
                           {
                dss = xx;
                k = j;
              }
            }
          }
        }
        else 
               {
          dss = w[L];
          k = L;
          for (j = 1; j <= nc; j++) 
                   {
             if ( sc[j][i] > 0.0 ) 
                      {
              xx = w[j]/sc[j][i];
              if ( xx < dss) 
                           {
                dss = xx;
                k = j;
              }
            }
          }
          dss = -dss;
        }
        if ( dss != 0.0 ) 
               {
          w[L] += dss;
          for (j = 1; j <= nc; j++) 
                       w[j] += dss*sc[j][i];
          w[k] = 0.0;
          if (k != L) 
                       SOLDEL = TRUE;
        }
      }
    }
  }
   if (SOLDEL) 
      return (FALSE);
  else 
      return (TRUE);
}

/* ------------ correct elemental abundances ----------------- */
void elcorr(void)
{
  int i,j,k,L,ip1;
  double par,xx,r;

  #ifdef DEBUG
  fprintf(debug_prn_file,"\nEntered into 'elcorr()'");
  #endif
  for (i = 1; i <= nc; i++) 
   {
       sa[i] = ga[i] - gai[i];
    for(j = 1; j <= nc; j++) 
          sm[j][i] = bm[i][j];  /* get a temporary transpose of bm(i,j) */
  }
  #ifdef DEBUG
  fprintf(debug_prn_file,"\nData for bm[i,j] to use in mlequ() in 'basopt(FALSE)'\n");
  for (i = 1; i <= m; i++) 
   {
    for (j = 1; j <= ne; j++)
      fprintf(debug_prn_file,"%4.2g ",bm[i][j]);
    fprintf(debug_prn_file,"\n");
  }
  fprintf(debug_prn_file,"\nData for sm[i,j] (Transformed bm[i,j])'\n");
  for (i = 1; i <= ne; i++) 
   {
    for (j = 1; j <= m; j++)
      fprintf(debug_prn_file,"%4.2g ",sm[i][j]);
      fprintf(debug_prn_file,"\n");
  }
  #endif
  /* ------- mlequ() --------- */
  for ( i = 1; i <= nc; i++) {
    if ( sm[i][i] == 0.0 ) {
      ip1 = i+1;
      for (k = ip1; k <= nc; k++)
        if (sm[k][i] != 0.0 ) goto JUMP1;
      strcpy( error, "\n No unique solution - Number of components < Number of elements.\nAborting VCS solver ....");
      longjmp( e_buf, 1 );
  JUMP1:  for (j = i; j <= nc; j++) sm[i][j] += sm[k][j];
      sa[i] += sa[k];
    }
    for (L = 1; L <= nc; L++) {
      if ( L == i || sm[L][i] == 0.0 ) {
      }
      else {
        r = sm[L][i]/sm[i][i];
        for (j = i; j <= nc; j++) sm[L][j] -= sm[i][j]*r;
        sa[L] -= sa[i]*r;
      }
    }
  }
  for (i = 1; i <= nc; i++)
      sa[i] = -sa[i]/sm[i][i];
    /* --------- end mlequ() ------------ */
  par = 0.5;
  for (i = 1; i <= nc; i++) {
    xx = -sa[i]/w[i];
    if (par < xx) par = xx;
  }
  par = 1.0/par;
  if ( par <= 1.01 && par > 0.0 ) par = .99*par;
  else par = 1.0;
  for (i = 1; i <= nc; i++)
    w[i] += par*sa[i];
  #ifdef DEBUG
  elab();
  fprintf(debug_prn_file,"\n\nElemental abundances");
  for (i =1; i <= ne; i++) fprintf(debug_prn_file,"\n           %1d    %10.5g",i,ga[i]);
  #endif
}

/* -------- calculates free energy changes for the reactions --------- */
void deltag(int L, int j, int kp)
{
    int i,k,LL;
  double sdel[MAXPHASE];

  switch ( L ) {
    /* -------- majors only --------------- */
    case -3 :
    case -2 :
    case -1 : for (k = 1; k <= nr; k++) {
            if ( ic[k] == MAJOR ) {
              LL = ir[k];
              dg[k] = fe[LL];
              for (LL = 1; LL <= nc; LL++)
                dg[k] += sc[LL][k]*fe[LL];
            }
          }
          break;
    /* ------------- all reactions from j to kp ----------- */
    case 0  : for (k = j; k <= kp; k++) {
            LL = ir[k];
            dg[k] = fe[LL];
            for (LL = 1; LL <= nc; LL++)
              dg[k] += sc[LL][k]*fe[LL];
          }
          break;
    /* ------------ minors only ( NMINOR + MINOR ) --------------- */
    case 1  : for (k =1; k <= nr; k++) {
            if ( ic[k] <= MINOR ) {
              LL = ir[k];
              dg[k] = fe[LL];
              for (LL = 1; LL <= nc; LL++)
                dg[k] += sc[LL][k]*fe[LL];
            }
          }
          break;
  }
  /* ------ multispecies phase with nt = 0 ------------- */
  for(i = 1; i <= no_phases; i++)
    sdel[i] = 0.0;
  for (i = 1; i <= nr; i++) {
    LL = ir[i];
    if ( w[LL] == 0.0 ) {
      if ( dg[i] > 50.0 ) dg[i] = 50.0;
      if ( dg[i] < -50.0 ) dg[i] = -50.0;
      for (k = 1; k <= no_phases; k++)
        if ( si[LL] == k ) sdel[k] += exp( -dg[i] );
    }
  }
  for (i = 1; i <= nr; i++) {
    LL = ir[i];
    if ( w[LL] == 0.0 ) {
      for (k = 1; k <= no_phases; k++)
        if ( si[LL] == k ) dg[i] = 1 - sdel[k];
    }
  }
}



/* -------- Chooses optimum basis, calculates stoichiometry ---------- */
void basopt(int FIRST)
{
  int k,jr,jl,i,j,L,ip1;


  double temp,r;

  CONV = FALSE;
  nopt++;
  for (i = 1; i <= mr; i++) aw[i] = w[i];
  for (jr = 1; jr <= nc; jr++) {
    do {
      k = amax(aw,jr,mr);
      if ( aw[k] == 0.0 ) {
        CONV = TRUE;  /* the biggest specie is zero */
        return;
      }
      if ( aw[k] == TEST ) {
        nc = jr - 1;
        n = m - nc;
        nr = n;
        for (i = 1; i <= m; i++) ir[i] = nc + i;
      }
      aw[k] = TEST;
      /* ----- check linear independence with previous species --- */
      /* ----- logical function lindep(bm,jr,k,nc) ------ */
      jl = jr - 1;
      sa[jr] = 0.0;
      for (j = 1; j <= nc; j++) sm[j][jr] = bm[k][j];
      if ( jl != 0 ) {
        for (j = 1; j <= jl; j++) {
          ss[j] = 0.0;
          for (i = 1; i <= nc; i++) ss[j] += sm[i][jr]*sm[i][j];
          ss[j] = ss[j]/sa[j];
        }
      for (j = 1; j <= jl; j++)
        for (L = 1; L <= nc; L++)
          sm[L][jr] -= ss[j]*sm[L][j];
      }
      for (j = 1; j <= nc; j++)
        if ( fabs(sm[j][jr]) > 1e-17 )
          sa[jr] += sm[j][jr]*sm[j][jr];
    }
    while ( sa[jr] < 1e-6);
    /* ----- rearrange data ---------- */
    dsw(w,jr,k);
    dsw(wt,jr,k);
    dsw(ff,jr,k);
    dsw(initial_chem_pot,jr,k);
    dsw(fe,jr,k);
    dsw(aw,jr,k);
    swap2((int *)si,jr,k);
    dsw(da,jr,k);
    dsw(fel,jr,k);
    swap2(ind,jr,k);
    rotate(index2, jr, k);
    for(j = 1; j <= ne; j++) {
      temp = bm[jr][j];
      bm[jr][j] = bm[k][j];
      bm[k][j] = temp;
    }
  }
  if ( FIRST ) return;
  /* ----- evaluate stoichiometric matrix, when FIRST = FALSE ------- */
  for (j = 1; j <= nc; j++) {             /* Transform bm[] into sm[] */
    for (i = 1; i <= nc; i++) sm[i][j] = bm[j][i];
  }
  #ifdef DEBUG
    fprintf(debug_prn_file,"\nData for bm[i,j] to use in mlequ() in 'basopt(FALSE)'\n");
    for (i = 1; i <= m; i++) {
      for (j = 1; j <= ne; j++)
        fprintf(debug_prn_file,"%4.2g ",bm[i][j]);
      fprintf(debug_prn_file,"\n");
    }
    fprintf(debug_prn_file,"\nData for sm[i,j] (Transformed bm[i,j])'\n");
    for (i = 1; i <= ne; i++) {
      for (j = 1; j <= m; j++)
        fprintf(debug_prn_file,"%4.2g ",sm[i][j]);
    fprintf(debug_prn_file,"\n");
    }
  #endif
  for (i = 1; i <= n; i++) {            /* Transform bm[] into stoich.coeff.matr sc[] */
    for(j = 1; j <= nc; j++) sc[j][i] = bm[ir[i]][j];
  }
  #ifdef DEBUG
    fprintf(debug_prn_file,"\nData for sc[i,j] to use in mlequ() in 'basopt(FALSE)'\n");
    for (i = 1; i <= nc; i++) {
      for (j = 1; j <= n; j++)
        fprintf(debug_prn_file,"%4.2g ",sc[i][j]);
      fprintf(debug_prn_file,"\n");
    }
  #endif
  /* ------- mlequ() --------- */
  /* ------ calculates stoichiometric matrix (reactions) -------- */
  for ( i = 1; i <= nc; i++) {
    if ( sm[i][i] == 0.0 ) {
      ip1 = i+1;
      for (k = ip1; k <= nc; k++)
        if (sm[k][i] != 0.0 ) goto JUMP;
      #ifdef DEBUG
        fprintf(debug_prn_file,"\nData for sm[i,j] of mlequ() in 'basopt(FALSE)'\n");
        for (jr = 1; jr <= ne; jr++) {
          for (j = 1; j <= m; j++)
            fprintf(debug_prn_file,"%4.2g ",sm[jr][j]);
          fprintf(debug_prn_file,"\n newline \n");
        }
      #endif
      strcpy( error, "\n No unique solution - Number of components < Number of elements.\nAborting VCS solver ....");
      longjmp( e_buf, 1 );
  JUMP:   for (j = i; j <= nc; j++) sm[i][j] += sm[k][j];
      for (j = 1; j <= n; j++) sc[i][j] += sc[k][j];
    }
    for (L = 1; L <= nc; L++) {
      if ( L == i || sm[L][i] == 0.0 ) {
      }
      else {
        r = sm[L][i]/sm[i][i];
        for (j = i; j <= nc; j++) sm[L][j] -= sm[i][j]*r;
        for (j = 1; j <= n; j++) sc[L][j] -= sc[i][j]*r;
      }
    }
  }
  for (i = 1; i <= nc; i++)
    for (j = 1; j <= n; j++)
      sc[i][j] = -sc[i][j]/sm[i][i];
  #ifdef DEBUG
    fprintf(debug_prn_file,"\nStoichiometric matrix sc[i][j]\n\n");
    for (i = 1; i <= nc; i++) {
      for (j = 1; j <= n; j++)
        fprintf(debug_prn_file,"%4.2g  ",sc[i][j]);
      fprintf(debug_prn_file,"\n");
    }
  #endif
  /* ------ end of mlequ -------- */
  for ( i = 1; i <= n; i++) {
    k = ir[i];
    for (j = 0; j <= no_phases; j++) d_mole[j][i] = 0.0;
    if (si[k] != SINGLE) d_mole[si[k]][i] = 1.0;
    for ( j = 1; j <= nc; j++) {
      if ( fabs(sc[j][i]) <= 1e-6 ) sc[j][i] = 0.0;
      if ( si[j] != SINGLE ) d_mole[si[j]][i] += sc[j][i];
    }
  }
}

/* ------ computes elemental abundances --------- */
void elab(void)
{
  int i,j;

  for (i = 1; i <= ne; i++) ga[i] = 0.0;
  for (j = 1; j <= ne; j++)
    for (i = 1; i <= m; i++)
      ga[j] += bm[i][j]*w[i];
}

/* ------ evaluate chemical potential ----------- */
void dfe(DARRAY z, int kk, int LL)
/* kk: >0 -> do it for tmole1
   kk:=<0 -> do it for tmole

   LL: =0 -> all components + all reactions
   LL: >0 -> all components + minor reactions only
   LL: <0 -> all components + major reactions only
*/
{
  int L1,i,L;
  double x[MAXPHASE];


  if ( kk > 0 ) 
   {
    for (i = 1; i <= no_phases; i++) 
       {
      x[i] = tmole1[i];
      if ( x[i] > 0.0 ) 
              x[i] = log((double)x[i]);
    }
  }
  else 
   {
    for (i = 1; i <= no_phases; i++)
       {
      x[i] = tmole[i];
      if ( x[i] > 0.0 ) 
              x[i] = log((double)x[i]);
    }
  }
  if ( LL == 0 ) 
       L1 = mr;
  else 
       L1 = nc;
  for (i = 1; i <= L1; i++) 
   {      /* all reaction species, or components */
    if (z[i] != 0.0) 
       {
      if( si[i] != SINGLE) 
               fe[i] = ff[i] -x[si[i]] + log((double)z[i]);
    }
    else 
          fe[i] = ff[i];
  }
  if (LL < 0) 
   {
    for (i = 1; i <= nr; i++)  /* majors only */
       {     
      if ( ic[i] == MAJOR ) 
           {
        L = ir[i];
        if ( z[L] == 0.0 ) 
                    fe[L] = ff[L];
        else 
               {
          if (si[L] != SINGLE) 
                      fe[L] = ff[L] -x[si[L]] + log((double)z[L]);
        }
      }
    }
  }
  else if (LL > 0) 
   {
    for (i = 1; i <= nr; i++)     /* minors only */
       {     
      if ( ic[i] <= MINOR ) 
           {
        L = ir[i];
        if ( z[L] == 0.0 ) 
                  fe[L] = ff[L];
        else 
               {
          if (si[L] != SINGLE) 
                       fe[L] = ff[L] -x[si[L]] + log((double)z[L]);
        }
      }
    }
  }
  #ifdef DEBUG2
  fprintf(debug_prn_file,"\nCalled dfe()");
  for (i = 1; i <= m; i++)
    fprintf(debug_prn_file,"\n     fe[%1d] = %g",i,fe[i]);
  #endif
}

/* ----- the following functions re-arrange the vector elements ------ */
/* --- swap integers ------- */
void swap2(int x[],int j1,int j2)
{
  int t;

  t = x[j1];
  x[j1] = x[j2];
  x[j2] = t;
}
void rotate(int x[],int j1,int j2)
{
  x[ind[j1]] = j1;
  x[ind[j2]] = j2;
}

int amax(DARRAY x,int j,int n)
{
  int i,k;
  double big;

  k = j;
  big = x[j];
  for (i = j; i <= n; i++) {
    if (x[i] > big) {
      k = i;
      big = x[i];
    }
  }
  return (k);
}

/* ----- swap floating points ----- */
void dsw(DARRAY x, int j1, int j2)
{
  double t;

  t = x[j1];
  x[j1] = x[j2];
  x[j2] = t;
}


/* ------------ estimates equilibrium compositions ------------ */
void inest(void)
{

  double  xt1[MAXPHASE],
      xt2[MAXPHASE],
      par,xl,ikl,s,s1;

  int i,j,k,lt,L;

  ikl = 0.0;
  lt = 0;
  /* -- setup data for linear programming routine --- */
  /* -- we will use sc[][] temporaly as the linear programing matrix -- */
  for ( i = 1; i <= ne; i++) sc[i][m+ne+1] = gai[i];
  for ( j = 1; j <= m ; j++)
    for (i = 1; i <= ne; i++) sc[i][j] = bm[j][i];  /* transpose matrix */
  for (j = 1; j <= m; j++) sc[ne+2][j] = ff[j];       /* changed -ff[j] to ff[j] and do minimising */
  /* ------ insert linear prog here ---------- */
  lin_prog(aw,sc);
  for (i = 1; i <= m; i++) {
    ds[i] = 0.0;
    w[i] = aw[i];
    if (aw[i] == 0.0) w[i] = 1e-2;
  }
  for (i = 1; i <= m; i++) wt[i] = w[i];
  basopt(FALSE);
  /* ----- calculate total gaseous and liquid moles ---- */
  /* ----- chemical potentials of basis ---------------- */
  for (i = 1; i <= no_phases; i++)
    tmole1[i] = tinert[i];
  for (i = 1; i <= nc; i++) {
    if ( si[i] != SINGLE) tmole1[si[i]] += wt[i];
  }
  for (i = 1; i <= nc; i++) {
    if ( si[i] != SINGLE)  fe[i] = ff[i] + log((double)wt[i]/tmole1[si[i]]); /* check if tmole1[] could be zero */
  }
  for (i = nc+1; i <= m; i++) fe[i] = ff[i];
  deltag(0,1,n);
  /* ------ estimate reaction adjustments ---------- */
  for (i = 1; i <= no_phases; i++) {
    delta_mole[i] = 0.0;
    if ( tmole1[i] != 0.0 ) {
      xt1[i] = log((double)1e32 * tmole1[i]);
      xt2[i] = log((double)1e-32 * tmole1[i]);
    }
  }
  /* ++++++ equation 3.4 in Canadian Jour of Chemical Engineer Vol 46 August 1968 +++++ */
  for (i = 1; i <= nr; i++) 
   {
    L = ir[i];
    if ( si[L] != SINGLE ) 
       {
      if ( dg[i] < xt1[si[L]] && dg[i] > xt2[si[L]] ) 
           {
        ds[L] = tmole1[si[L]] * exp(-dg[i]);
        for (k = 1; k <= nc; k++) 
                   ds[k] += sc[k][i] * ds[L];
        delta_mole[si[L]] += d_mole[si[L]][i] * ds[L];
      }
    }
  }
  /* ------- keep basis specie positive ------------- */
  par = 0.5;
  for (i = 1; i <= nc; i++)
    if ( par < (-ds[i]/wt[i]) ) par = -ds[i]/wt[i];
  par = 1.0/par;
  if ( par <= 1.0 && par > 0.0 )
    par = 0.8*par;
  else 
       par = 1.0;
  /* ------- calculate new mole numbers ------------ */
  while(TRUE) 
   {
    for (i = 1; i <= nc; i++) 
          w[i] = wt[i] + par*ds[i];
    for (i = nc+1; i <= m; i++)
      if (ds[i] != 0.0 ) w[i] = ds[i]*par;
    for (i = 1; i <= no_phases; i++)
      tmole[i] = tmole1[i] + delta_mole[i]*par;
    if( lt > 0 ) 
          return;
    /* ------- convergence - forcing section ---------- */
    dfe(w,0,0); /* for tmole and components + all reactions */
    s = 0.0;
    for (i = 1; i <= mr; i++) 
           s += ds[i]*fe[i];
    if ( s < 0.0 && ikl <= 0.0 ) 
           return;
    else if ( s == 0.0 ) 
           return;
    if ( ikl <= 0.0 ) 
       {
      /* -- try half step size --- */
      s1 = s;
      par *= 0.5;
      ikl = 1.0;
    }
    else 
       {
      /* ---- fit parabola thro half and full steps ----- */
      xl = 0.5 * (1.0 - s/(s1-s));
      if ( xl >= 0.0 ) 
           {
        if ( xl <= 1.0 ) 
                  par *= 2.0*xl;
        else                /* too big a step, take orginal full step */
                  par *= 2.0;                
      }
      else                   /* poor direction reduce step size */
             par *= 0.2;          
      lt = 1;
    }
  }
}

/* ---------- linear programing --------------------- */
/* minimize (cc*psol) such that ax[ne+2,j]*psol = bb  */
/* -------------------------------------------------- */
/* this routine uses the Two-Phase technique from the book */
/* "Some common basic programs-p.103". See also "Operations Research - */
/* H.A.Taha - p.70" */

void lin_prog(DARRAY psol, DMATRIX ax)
{
  int b[MAXELEMENT],
    leaving,enter,
    m1,m2,i,j,i1,j1,
    c1,c2,x;


  double  pivot, temp;

  /* -- initialise variables ----- */
  m1 = ne + 1;
  m2 = ne + 2;
  c1 = m + ne + 1;
  c2 = m;
  /* Check this dynamically */
  if( m2 >= sc_max_i || c1 >= sc_max_j )
  {
    strcpy( error, "Exceeded allocated scratch pad memory. Aborting....");
    longjmp( e_buf, 1 );
  }

  for (i = 1; i <= c1; i++)
    ax[m1][i] = 0.0;
  // Zero the part which gave me endless problems as Floating point errors
  for (i = m+1; i <= c1; i++)
    ax[m2][i] = 0.0;
  for (i = 1; i <= ne; i++)
   {
    for (j = 1; j <= m; j++) ax[m1][j] -= ax[i][j];
    for (j = m+1; j <= (m+ne); j++) ax[i][j]=0.0;
    ax[i][m+i] = 1.0;             /* artificial variables */
    b[i] = m + i;
  }
  x = m1;
  while ( x <= m2 ) {
    do  {
      /* ---- price out columns (find entering variable ) ----- */
      temp = -1e-5;
      for (j = 1; j <= m; j++) {
        if ( ax[x][j] < temp ) {
          enter = j;              /* enter = biggest negative value */
          temp = ax[x][j];
        }
      }
      if ( temp != -1e-5 ) {
        /* --- find leaving variable ---- */
        temp = 1e38;
        /* ----- choose smallest positive ratio -------- */
        for (i = 1; i <= ne; i++) {
          if ( ax[i][enter] > 1e-5 && ax[i][c1]/ax[i][enter] < temp ) {
            leaving = i;
            temp = ax[i][c1]/ax[i][enter];
          }
        }
        if ( temp == 1e38 ) {
          strcpy( error, "The LP-solution is unbounded !!!!\nAborting VCS solver ....");
          longjmp( e_buf, 1 );
        }
        else {
          /* ----- preform pivoting ----- */
          pivot = ax[leaving][enter];
          for (j = 1; j <= c1; j++) ax[leaving][j] /= pivot;
          for (i = 1; i <= m2; i++) {
            if ( i != leaving) {
              for (j = 1;  j <= c1; j++) {
                if ( j != enter) {
                  ax[i][j] -= ax[i][enter]*ax[leaving][j];
                  if ( fabs(ax[i][j]) < 1e-5 )
                      ax[i][j] = 0.0;
                }
              }
            }
          }
          for (i = 1; i <= m2; i++)  ax[i][enter] = 0.0;
          ax[leaving][enter] = 1.0;
          b[leaving] = enter;
        }
      }
    }
    while ( temp != -1e-5 );
    if ( x == m1 ) {
      for (i1 = 1; i1 <= ne; i1++) {
        if ( b[i1] > c2 ) {
          if ( ax[i1][c1] > 1e-5 ) {
            strcpy( error, "The LP-problem has no feasible solution.\nAborting VCS solver ....");
            longjmp( e_buf, 1 );
          }
          else {
            for (j1 =1 ; j1 <= c2 ; j1++) {
              if ( fabs(ax[i1][j1]) > 1e-5 ) {
                leaving = i1;
                enter = j1;
                /* ----- preform pivoting ----- */
                pivot = ax[leaving][enter];
                for (j = 1; j <= c1; j++) ax[leaving][j] /= pivot;
                for (i = 1; i <= m2; i++) {
                  if ( i != leaving) {
                    for (j = 1;  j <= c1; j++) {
                      if ( j != enter) {
                        ax[i][j] -= ax[i][enter]*ax[leaving][j];
                        if ( fabs(ax[i][j]) < 1e-5 )
                           ax[i][j] = 0.0;
                      }
                    }
                  }
                }
                for (i = 1; i <= m2; i++)  ax[i][enter] = 0.0;
                ax[leaving][enter] = 1.0;
                b[leaving] = enter;
                j1 = c2;
              }
            }
          }
        }
      }
    }
    x++;
  }
  for (i = 1; i <= m; i++) psol[i] = 0.0;
  for (i = 1; i <= ne; i++)
    psol[b[i]] = ax[i][c1];
}
