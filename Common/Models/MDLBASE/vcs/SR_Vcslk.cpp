/* this module links the nodes and pipes to the VCS routine and then calls */
/* the VCS algorithm. Finally it transfers the resulting moles of the specie */
/* to the data arrays via trans_result() */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Still to cater for when Single specie phases and inerts are handeled  */
/* NOTE (1) in invoke_VCS()  and calc_sen() the inerts must also be swapped*/
/*      if the phase numbering is altered              */
/*      (2) in initial_data() it will make a phase containing one specie */
/*      a single-specie phase (this must be catered for)         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define  VCSTHLNK  1
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <conio.h>
#include <setjmp.h>
#include "sr_vcs.h"
#include "vcstherm.h"
#include "ssizes.h"
//#include "parser.h"
#include "error.h"

/* External declarations */

/* Local definitions */
double alpha, GASCONST;
jmp_buf e_buf;
enum mnemonic  { CLR, RTN, PUSH, GET, FUNC, UNA, POW, MUL, DIV, ADD, SUB };
static struct OPCODE opcode[MAX_OPCODE];    /* local copy of the opcodes */
static INDEX  VCS_specie[MAXSPECIE];      /* points to the comp_array for each compound */
static INDEX  VCS_act[MAXSPECIE];       /* points to the local opcode_array for each  */
                        /* compound that has an activity formalism */
static enum Elem  VCS_elem[MAXELEMENT];   /* contains the unique elements defined for the */
                        /* formula matrix */

/* Local function decalarations */
int   n_build_VCS( unsigned node );
int   p_build_VCS( unsigned pipe );
int   s_build_VCS( unsigned strm, enum Object type, double Temp, int add );
int s_delete_losses_VCS( INDEX strm );
int   clear_VCS_arrays(void);
void  check_phase_distr( void );
double  parse_opcode(INDEX i);
void  calc_sen(void);
void  trans_result( enum Object object, INDEX ob_num );
int   calc_coef_mat(void);
void  ludcmp(double a[][MAXELEMENT+MAXSPECIE], int n, int indx[], double *d);
void  lubksb( double a[][MAXELEMENT+MAXSPECIE], int n, int indx[], double b[]);

#define TRANSFER
#define EPSON

#ifdef EPSON
 #define CONDENSED_ON "\xf"
 #define CONDENSED_OFF  "\x12"
#else
 #define CONDENSED_ON "\x1b\x51"
 #define CONDENSED_OFF  "\x1b\x4e"
#endif

/* Calculate the compositions of pipes in simple componentes */
/* must call 'initialise_pipe_simple_compositions' once prior to calculating the
   compositions */
int initialise_pipe_simple_compositions( INDEX pipe )
{
  int   error_code = NO_ERR;

  clear_VCS_arrays();
  error_code = p_build_VCS( pipe );
  return error_code;
}

/* Now: Calculate the compositions of pipes in simple componentes */
int calc_slag_pipe_simple_compositions( enum Elem first_comp, enum Elem second_comp, double *num_moles )
{
   int i;
   int index1 = MARKER,
     index2 = MARKER;

   *num_moles = 0.0;
  //fprintf(debug_prn_file,"\n %S & %S", Elem_txt[first_comp], Elem_txt[second_comp]);
   /* first check if the components are in the stream at all*/
   if (s_uni_mat[first_comp] == 0.0 || s_uni_mat[second_comp] == 0.0)
   {
    //fprintf(debug_prn_file,"\n      Could not find = %s & %s", Elem_txt[first_comp], Elem_txt[second_comp]);
    return NO_ERR;
   }
   for ( i=1; i<=ne; i++ )
   {
     if( VCS_elem[i] == first_comp ) index1 = i;
     if( VCS_elem[i] == second_comp ) index2 = i;
   }
   //fprintf(debug_prn_file,"\nIndex1 = %d, Index2 = %d", index1, index2);

   if( index1 == MARKER || index2 == MARKER ) return !NO_ERR;
   // Add the moles where the first and second elements are simultaneously
   // not zero
   for ( i=1; i<=m; i++ )
   {
     if (!(bm[i][index1] == 0.0 || bm[i][index2] == 0.0))
     {
      *num_moles += (w[i] * bm[i][index1]);
      //fprintf(debug_prn_file,"\nSpecie = %d, Mass = %f", i, (w[i] * bm[i][index1]));
     }
   }

    return NO_ERR;
}

/* Build the VCS arrays for the node */
int n_build_VCS( INDEX node )
{
  int   i, j;
  int   error_code = NO_ERR;
  double temp;
  double  temp1;
  struct  Pipe  *p_ptr;
  struct  CTNode    *ptr;

  ptr = &node_array[node];
  temp = n_get_temp( node);
  alpha = 0.0;

#if defined(DEBUG3)
  fprintf(debug_prn_file,CONDENSED_ON);
  fprintf(debug_prn_file,"\nNode # = %d", node+1);
  fprintf(debug_prn_file,"\nInput specie and molecular mass:");
#endif
  for(i=0; i<ptr->no_of_P_in; i++)  /* build the elemental abundances */
  {
     p_ptr = &pipe_array[ptr->pipe_in[i]];
     for(j=0; j<p_ptr->no_of_streams; j++)
    if ( error_code = s_build_VCS( p_ptr->Str[j], NODE, temp, TRUE) )
      return error_code;
  }
  for(i=0; i<ptr->no_of_P_out; i++) /* build the formula matrixes */
  {
     p_ptr = &pipe_array[ptr->pipe_out[i]];
     for(j=0; j<p_ptr->no_of_streams; j++)
     {
    if ( error_code = s_build_VCS( p_ptr->Str[j], NODE, temp, FALSE) )
      return error_code;
    temp1 = s_get_alpha(p_ptr->Str[j]);
    alpha = (temp1 > alpha) ? temp1 : alpha ;  /* select the greatest alpha */
     }
  }
  /* subtract the losses around a node */
  if( n_are_there_losses( node ) )
  {
     p_ptr = &pipe_array[n_get_losses_pipe( node )];
     for(j=0; j<p_ptr->no_of_streams; j++)
     if ( error_code = s_delete_losses_VCS(  p_ptr->Str[j] ) )
      return error_code;
  }
  iest = 1;           /* set at machine estimate */
  return error_code;
}

/* Build the VCS arrays for the pipe */
int p_build_VCS( INDEX pipe )
{
  int   i;
  int   error_code = NO_ERR;
  double temp;
  double  temp1;
  struct  Pipe  *ptr;

  ptr = &pipe_array[pipe];
  temp = p_get_ntemp( pipe );
  alpha = 0.0;

#if defined(DEBUG3)
  fprintf(debug_prn_file,CONDENSED_ON);
  fprintf(debug_prn_file,"\nInput specie and molecular mass:");
#endif
  for(i=0; i<ptr->no_of_streams; i++)
  {
     if ( error_code = s_build_VCS( ptr->Str[i], PIPE, temp, FALSE) )
     break;
     temp1 = s_get_alpha(ptr->Str[i]);
     alpha = (temp1 > alpha) ? temp1 : alpha ;  /* select the greatest alpha */
  }
  iest = 0;           /* set at user estimate */
  return error_code;
}


/* Build the VCS arrays for the stream by adding its compounds into the    */
/* formula matrixes (bm) and the elemental abundance (gai)           */
/* The fuction will add both the elemental abundance and the formula matrix */
/* for the PIPE objects. For the NODE object by specifing add (TRUE) it will */
/* add the stream only to the elemental abundance ( this is for pipe going into */
/* the node) else it would only add the stream to the formula matrix (pipes */
/* going out of the node ). */
int s_build_VCS( INDEX strm, enum Object type, double Temp, int add )
{
  int   i, j, ii, k;
  int   error_code = NO_ERR;
  int   num_el, found;
  INDEX current_comp, current_opcode;
  struct Stream       *ptr;
  struct Element_array  el;

  ptr = &stream_array[strm];

  if ( (type==PIPE) || ((type==NODE) && add) )
  {
    s_reset_uni_elem( strm );  /* reset the unique element array and counter */

#if defined(DEBUG)
    /* Lets print the incoming compounds */
    fprintf(debug_prn_file,"\n  %25.25s",  s_name( strm ) );

    for(i=0; i<ptr->no_of_comp; i++)
    {
       current_comp = ptr->comp[i];     /* get the index to the compound */
       fprintf(debug_prn_file,"\n  %25.25s  %f", c_name(current_comp), c_get_mole( current_comp ) );
    }
#endif
    /* we have to determine if the streams unique elements are already specified */
    /* If they are specified, add the number of the unique element to the abundance */
    /* array. Else add the name of the unique element to the list VCS_elem and add */
    /* the number to the abundance */
    for(i=0; i<MAX_UNI_ELEM; i++)
    {
      if (s_uni_mat[i] == 0.0) continue;
      found = FALSE;
      for(j=0; j<MAXELEMENT; j++)
      {
        if (VCS_elem[j+1] == Nil) break;
        if ( VCS_elem[j+1] == i )
        {
          found = TRUE;
          gai[j+1] += s_uni_mat[i];
        }
      }
      if ( !found )
      {
        if ( j<MAXELEMENT )
        {
        VCS_elem[j+1] = i;
        gai[j+1] += s_uni_mat[i];
        ne++;   /* increase the number of elements */
        }
        else
        {
          sprintf( error, "Exceeded maximum elements for VCS in stream %s", s_name(strm) );
          return error_code = VCS_ELEM_OVFLOW;
        }
      }
    }
  }
  if ( (type==PIPE) || ((type==NODE) && !add) )
  {
    for(i=0; i<ptr->no_of_comp; i++)
    {
      /* first we have to find a empty space to add the compound */
      k=1;    /* start at one */
      while ( VCS_specie[k] != MARKER && k < MAXSPECIE) k++;
      if ( k >= MAXSPECIE )
      {
        sprintf( error, "Exceeded maximum species for VCS in stream %s", s_name(strm) );
        return error_code = VCS_SPECIE_OVFLOW;
      }
      VCS_specie[k] = current_comp = ptr->comp[i];  /* assign the compound */
      w[k] = c_get_mole( current_comp );        /* get the number of moles of the compound */
      ff[k] = c_delta_G( current_comp, Temp )/1e3;  /* get the chemical potential */
                              /* and convert to kJ/kmole */
      m++;                      /* increase the number of species */
      /* ----------- The following phase assignment (see invoke_VCS)could be a problem ---------- */
      si[k] = ptr->s_phase;             /* get the phase type for the compound */
      /* now we have to build the formula array for this compound */
      num_el = c_num_uni_elemts( current_comp );    /* get the number of unique elements for this compound */
      c_uni_elemts( current_comp, &el );        /* get the unique elements */
      for(j=0; j<num_el; j++)
      {
         ii = 1;   /* start at one */
         /* does the compound element name correspond to the already defined elemental abundance */
         while ( VCS_elem[ii] != el.element[j] && ii < MAXELEMENT ) ii++;
         if ( ii >= MAXELEMENT )
         {
           sprintf( error, "Could not find a place in VCS for element %s of compound %s in stream %s", Elem_txt[el.element[j]], c_name(current_comp), s_name(strm) );
           return error_code = FATAL_ERR;
         }
         bm[k][ii] = (double)el.no_of_element[j];   /* enter the number of the element into the formula mat */
      }
      /* now we must enter the activity formalism */
      if (ptr->s_alpha != 0.0)   /* only do this for non_ideal cases */
      {
         VCS_act[k] = current_opcode = ptr->act_c[i]; /* assign the index into the opcode_array */
         if ( current_opcode != MARKER )
         { /* this compound must have a activity formalism */
         /* now we must go through our local copy of the opcodes and adjust the references */
         while ( opcode[current_opcode++].mnem != RTN )
         {
           if ( opcode[current_opcode].mnem == GET )
           opcode[current_opcode].x.entry += (INDEX)(k - i); /* now the opcode will point to the correct specie in VCS_specie */
         }
         }
      }
    }
  }
  return error_code;
}



int s_delete_losses_VCS( INDEX strm )
{
  int   i, j;
  int   error_code = NO_ERR;
  int   found;
  //int   num_el, found;
//  INDEX current_comp;
  struct Stream       *ptr;

  ptr = &stream_array[strm];

  s_reset_uni_elem( strm );  /* reset the unique element array and counter */


#if defined(DEBUG)
  /* Lets print the incoming compounds */
  fprintf(debug_prn_file,"\n Deleting node losses for stream %25.25s",  s_name( strm ) );

  for(i=0; i<ptr->no_of_comp; i++)
  {
     current_comp = ptr->comp[i];     /* get the index to the compound */
     fprintf(debug_prn_file,"\n  %25.25s  %f", c_name(current_comp), c_get_mole( current_comp ) );
  }
#endif
  /* we have to determine if the streams unique elements are already specified */
  /* If they are specified, delete the number of the unique element to the abundance */
  /* array. Else add the name of the unique element to the list VCS_elem and D=delete */
  /* the number to the abundance */
  for(i=0; i<MAX_UNI_ELEM; i++)
  {
    if (s_uni_mat[i] == 0.0) continue;
    found = FALSE;
    for(j=0; j<MAXELEMENT; j++)
    {
      if (VCS_elem[j+1] == Nil) break;
      if ( VCS_elem[j+1] == i )
      {
        found = TRUE;
        gai[j+1] -= s_uni_mat[i];
      }
    }
    if ( !found )
    {
      if ( j<MAXELEMENT )
      {
      VCS_elem[j+1] = i;
      gai[j+1] -= s_uni_mat[i];
      ne++;   /* increase the number of elements */
      }
      else
      {
        sprintf( error, "Exceeded maximum elements for VCS in stream %s", s_name(strm) );
        return error_code = VCS_ELEM_OVFLOW;
      }
    }
  }
  return error_code;
}

int clear_VCS_arrays(void)
{
  int i, j;


  type = 1;     /* set at kJ/mole convereted below from j/mole to kj/mole */
            /* so that we can compare the results with the PASCAL version */
  for (i = 0; i < MAXPHASE; i++)
  {   /* Remember that phase 0 -> 'SINGLE SPECIE PHASE' or 'PURE CONDENSED PHASE' */
    tinert[i] = 0.0;        /* Zero the inerts for each phase */
    no_specie_phase[i] = 0;     /* Zero the phase count for each phase */
  }
  memcpy(opcode, opcode_array, sizeof(opcode)); /* make a local copy of the opcode array */
  for(i=0; i<MAXSPECIE; i++)
  {
    VCS_specie[i] = MARKER;     /* Reset the compound indexes */
    VCS_act[i] = MARKER;
    for(j=0; j<MAXELEMENT; j++)
      bm[i][j] = 0.0;       /* Reset formula matrix */
  }
  for(i=0; i<MAXELEMENT; i++)
  {
    VCS_elem[i] = Nil;        /* Reset the unique element number */
    gai[i] = 0.0;         /* Reset elemental abundances */
  }
  ne = m = 0;       /* zero the number of elements and species */
  return NO_ERR;
}

/* Read the data from the program's data arrays and initialise the solver's data */
/* Then call the solver */
int invoke_VCS(enum Object object, INDEX ob_num)
{
  int i, j;
  int test;
  int error_code;

  clear_VCS_arrays();
  switch ( object )
  {
    case PIPE:  tempt = p_get_ntemp( ob_num  ); /* get pipe temperature */
          press = p_get_npress( ob_num );/* get pipe pressure */
          if( error_code = p_build_VCS( ob_num ) ) return error_code;
          break;
    case NODE:  tempt = n_get_temp( ob_num  ); /* get node temperature */
          press = n_get_press( ob_num );/* get node pressure */
          if( error_code = n_build_VCS( ob_num ) ) return error_code;
          break;
  }
  //Universal Gas Constant R_c 8.314472
  switch (type) {
    case -1: GASCONST = 0.0019872; break; /* kCal/mole */
    case  0: GASCONST = 1.0/tempt; break; /* MU/RT */
    case  1: GASCONST = 0.0083143; break; /* kJ/mole */
    case  2: GASCONST = 8.3143;    break; /* kJ/kmole or J/mole*/
  }
  for (i = 1; i <= m; i++)
  {
    no_specie_phase[si[i]]++;   /* count the no of species in each phase */
    da[i] = ff[i];                  /* store the starting chemical potential in da[] */
  }
  no_phases = 0;
  for (i = 1; i < MAXPHASE; i++)
    if (no_specie_phase[i] != 0) no_phases++; /* Count the number of phases */
  if ( no_phases == 0 ) return VCS_PHASE_ERR;
  check_phase_distr();

  // Create a longjump incase we have some or other error.
  test = setjmp( e_buf );
  if( test == 0)
  {
    /* NOW CALL THE VCS SOLVER */
    solve();
    /* Now transfer the results to the data arrays */
    trans_result( object, ob_num );
    /*-----------------------------------------------------------------*/
    /* HARDCODED SENSITIVITY CALCULATION                 */
    /* NOW CALL THE SENSITIVITY CALCULATOR TO DETEREMINE THE NECESSARY */
    /* Mn and Si mole INCREASES TO OBTAIN 0.1% Mn & 0.02% Si in the    */
    /* metal                               */
    /* Uncomment this                            */
    /* calc_sen();                             */
    /*-----------------------------------------------------------------*/
  }
  else return VCS_NO_CONV;

  if (ICONV)
  {
     strcat( error, "Conversion criterion not statisfied - Check results" );
     strcat( error, "\nIf Alpha is set, reduce it!" );
     return VCS_NO_CONV;
  }
  else return NO_ERR;
}

/* make sure that the phase token allocation is in sequentual order */
void check_phase_distr( void )
{
  int   i, j, k;

  for(i=1; i<=no_phases; i++)
  {
    j=0;
    while(no_specie_phase[i+j] == 0) j++;
    for (k=1; k<=m; k++)
    {
    if(si[k] >= i+j) si[k] -= j;
    }
    for(k=i; k<MAXPHASE; k++)
    no_specie_phase[k] = no_specie_phase[k+j];
  }
}

/* The solver calls this routine after each iteration */
/* it could be used to transfer the intermediate data */
void trans_mole_spread()
{
}

/* If the mode was set to non-ideal calculations the solver calls this */
/* routine (0 < alpha <=1). It should calculate the activity in terms of */
/* MU/RT and this should be added to the current chemical potential */
void get_activity_coef(void)
{
  int i, L;
  double coef;

/* First calculate ln(solvent) if it is implemented */
   /* ln(solvent) implementation */
/* Now calculate the activity coefficient */
  for (i = 1; i <= mr; i++)
  {
    L = ind[i];
    if (VCS_act[L] != MARKER)
    {
      coef = parse_opcode(VCS_act[L]);
      #ifdef DEBUG2
      fprintf(debug_prn_file,"\nln gamma[%2d] = %+4e mu = %+4e sigma = %+4e",i,coef,initial_chem_pot[i],ds[i]);
      #endif
      ff[i] += alpha*(initial_chem_pot[i] + coef - ff[i]);
    }
  }
}

/*  calculate specie i's activity coeficient */
double parse_opcode(INDEX i)
{
  double stack[10];
  int    k=-1;
  double temp;

  #ifdef DEBUG3
  if(opcode[i].mnem != CLR)
  {
    strcpy( error, "OPCODE array index error. Aborting....");
    longjmp( e_buf, 1 );
  }
  #endif
  while ( opcode[i].mnem != RTN )
  {
     #ifdef DEBUG3
     if(k>=9)
     {
     strcpy( error, "Activity stack overflow. Aborting....");
     longjmp( e_buf, 1 );
     }
     #endif
     switch ( opcode[i].mnem )
     {
      case  CLR : k=-1;
          break;
      case  PUSH: stack[++k] = opcode[i].x.op_value;
          break;
      case  GET :
            /* first check if the total moles for its phase is zero */
            temp = tmole[si[index2[opcode[i].x.entry]]];
            if (temp == 0.0)
            stack[++k] = 0.0;
            else
            stack[++k] =  w[index2[opcode[i].x.entry]] / temp;
          break;
      case  ADD : stack[--k] = stack[k] + stack[k+1];
          break;
      case  SUB : stack[--k] = stack[k] - stack[k+1];
          break;
      case  MUL : stack[--k] = stack[k] * stack[k+1];
          break;
      case  DIV : if(stack[k] != 0.0)
             stack[--k] = stack[k] / stack[k+1];
            else
            {
             strcpy( error, "ERROR - divide by zero in parse_opcode. Aborting....");
             longjmp( e_buf, 1 );
            }
          break;
      case  POW : stack[--k] = pow(stack[k], stack[k+1]);
          break;
      case  UNA : stack[k] = - stack[k];
          break;
      case  FUNC:
            switch ((int)opcode[i].x.entry)
            {
               case 0:  stack[k] = fabs(stack[k]);
                   break;
               case 1:  stack[k] = acos(stack[k]);
                   break;
               case 2:  stack[k] = asin(stack[k]);
                   break;
               case 3:  stack[k] = atan(stack[k]);
                   break;
               case 4:  stack[k] = cosh(stack[k]);
                   break;
               case 5:  stack[k] = cos(stack[k]);
                   break;
               case 6:  stack[k] = exp(stack[k]);
                   break;
               case 7:  stack[k] = log10(stack[k]);
                   break;
               case 8:  stack[k] = log(stack[k]);
                   break;
               case 9:  stack[k] = pow10(stack[k]);
                   break;
               case 10: stack[k] = (int)(stack[k] + 0.5);
                   break;
               case 11: stack[k] = sinh(stack[k]);
                   break;
               case 12: stack[k] = sin(stack[k]);
                   break;
               case 13: stack[k] = sqrt(stack[k]);
                   break;
               case 14: stack[k] *= stack[k];
                   break;
               case 15: stack[k] = tanh(stack[k]);
                   break;
               case 16: stack[k] = tan(stack[k]);
                   break;
               case 17: stack[k] = (int)stack[k];
                   break;
            }
          break;
     }
     i++;
  }
  if(k!=0) 
  {
    strcpy( error, "Internal stack error. Aborting....");
    longjmp( e_buf, 1 );
  }
  return stack[0];
}

/* When the solver has completed its job it calls this function which should */
/* transfer the result to the program's data arrays */
void trans_result( enum Object object, INDEX ob_num )
{
  int             i, j;
  struct  Pipe  *p_ptr;
  struct  CTNode    *ptr;


  /* Set the compound masses */
  for (i = 1; i <= mr; i++)
    c_set_mole( VCS_specie[ind[i]], w[i] );
  if ( mr != m )
  {         /* deleted species */
    for (i = mr+1; i <= m; i++)
      c_zero_mole( VCS_specie[ind[i]] );
  }
  /* Now we have to reset the totals and fractions of the streams */
  /* within the pipes that contained them */
  switch ( object )
  {
    case PIPE:    /* get the pipe pionter */
          p_ptr = &pipe_array[ob_num];
          for(j=0; j<p_ptr->no_of_streams; j++)
            s_reset_frac_mole( p_ptr->Str[j] );
          break;


    case NODE:    /* get the node ptr */
          ptr = &node_array[ob_num];
          /* Do it for each pipe going out off the node */
          for(i=0; i<ptr->no_of_P_out; i++)
          {
             p_ptr = &pipe_array[ptr->pipe_out[i]];
             for(j=0; j<p_ptr->no_of_streams; j++)
             s_reset_frac_mole( p_ptr->Str[j] );
          }
          break;
  }

}


void initial_data(void)
{
  int i,j;

  iter = 0;
  nopt = 0;
  nc = ne;
  IM = FALSE;
  ICONV = FALSE;
  for (i = 1; i <= m; i++)
    index2[i] = ind[i] = i;
  /*++++++++ Watch out for this +++++++*/
  for (j = 1; j <= no_phases; j++) {
    if ( no_specie_phase[j] == 1 && tinert[j] == 0.0) { /* only one specie in the phase - make it SINGLE */
      no_specie_phase[0]++;   /* increase SINGLE specie phase count */
      no_specie_phase[j] = 0;
      for (i = 1; i <= m; i++) {
        if ( j == GAS && si[i] == GAS ) {
          ff[i] += GASCONST * tempt * log((double)press);
          si[i] = SINGLE;
        }
        else if ( j == si[i] )
          si[i] = SINGLE;
      }
    }
  }
  n = m - nc;
  nr = n;
  mr = m;
  TEST = -1e-10;
  if ( iest != 0 ) {
    for (i = 1; i <= mr; i++) w[i] = -ff[i];
    TEST = -1e20;
  }
  for (i = 1; i <= m; i++) {
    ir[i] = nc + i;
    ff[i] /= (GASCONST * tempt);
    if ( si[i] == GAS )
      ff[i] += log((double)press);
    if ( si[i] == SINGLE )
      fe[i] = ff[i];
    initial_chem_pot[i] = ff[i];  /* initialise orginal chemical pot (activity) */
  }
  if ( iest != 0 ) {    /* do machine starting estimate */
    inest();
    elab();
    elcorr();
  }
  else {
    #ifdef DEBUG
    fprintf(debug_prn_file,"\nCalling 'basopt(TRUE)' from initial_data()");
    #endif
    basopt(TRUE); /* only calculate optimum basis but not the stioch. matrix */
    elab();
  }
  #ifdef DEBUG3
  fprintf(debug_prn_file, CONDENSED_ON );
  fprintf(debug_prn_file,"\n\nVCS - algorithm version 1.4");
  fprintf(debug_prn_file,"\n  %3d Species %3d Elements %3d Components",m,ne,nc);
  for (i = 0; i <= no_phases; i++)
    fprintf(debug_prn_file,"\n  %3d Phase %1d Species ",no_specie_phase[i],i);
  fprintf(debug_prn_file,"\nPressure       %8.3f atm\nTemperature    %8.3f Kelvin\n",press,tempt);
  fprintf(debug_prn_file,"Alpha = %8.3f\n",alpha);
  for (i = 1; i <= no_phases; i++)
    fprintf(debug_prn_file,"Phase %1d inerts  %5.2f\n",i,tinert[i]);
  //#endif
  //#ifdef DEBUG2
  fprintf(debug_prn_file,"\n Elemental abundances           Correct          From estimate\n");
  for (i = 1; i <= ne; i++) {
    fprintf(debug_prn_file,"                       %2d %s    %+.7e     %+.7e\n",i, Elem_txt[VCS_elem[i]],gai[i],ga[i]);
  }
  if ( iest == 0 ) fprintf(debug_prn_file,"\nUser estimate of equilibrium");
  else fprintf(debug_prn_file,"\nModified linear programming estimate of equilibrium");
  switch (type) {
    case -1 :   fprintf(debug_prn_file,"\nStandard chemical potential in kcal/mole\n");
          break;
    case  0 :   fprintf(debug_prn_file,"\nStandard chemical potential in MU/RT\n");
          break;
    case  1 :   fprintf(debug_prn_file,"\nStandard chemical potential in kJ/mole\n");
          break;
    case  2 :   fprintf(debug_prn_file,"\nStandard chemical potential in kJ/kmole\n");
          break;
  }
  fprintf(debug_prn_file,"\nSpecies     Stan. Chem. Pot.    Equilibrium est.   Formula vector");
  fprintf(debug_prn_file,"\n                                               ");
  for(j = 1; j <= ne; j++) fprintf(debug_prn_file,"  %s", Elem_txt[VCS_elem[j]]);
  fprintf(debug_prn_file," Phase\n");

  for (i = 1; i <= m; i++) {
    for(j = 0; j < 10; j++) fprintf(debug_prn_file,"%c",comp_array[VCS_specie[ind[i]]].name[j]);
    fprintf(debug_prn_file,"  ");
    fprintf(debug_prn_file,"%+.7e     %+.7e   ",da[i],w[i]);
    for(j = 1; j <= ne; j++) fprintf(debug_prn_file,"%2.1f ",bm[i][j]);
    fprintf(debug_prn_file,"  %1d\n",(int)si[i]);
  }
  #endif
}


/* -----------  final delta G/RT values --------------------------- */
/* using fel[], tmole1[] & wt[] as temporary values */
enum path calc_result(void)
{
  #ifdef DEBUG3
  int i,j,k;                      /* loop counters , temp values */
  int   mr1;
  double  g;
  #endif
  #ifdef DEBUG2
  int   L;
  double  lnK;
  #endif


  #ifdef DEBUG1
  fprintf(debug_prn_file,"\nTransfering data to spreadsheet");
  #endif
  #ifdef DEBUG3
  j = mr + 1;
  k = nr + 1;
  for (i = 1; i <= nr; i++) {
    j--;k--;
    dg[j] = dg[k];
  }
  for (i = 1; i < MAXPHASE; i++)
    tmole1[i] = 0.0;
  /* first calculate total mass */
  /* we will use fel as the temp variable for the mass% */
  /* and tmole1 as the total mass for the phase */
  for ( i = 1; i <= mr; i++ )
  {
    fel[i] = c_mole_mass(VCS_specie[ind[i]])*w[i];
    tmole1[si[i]] += fel[i];
  }
  /* calculate mass% and mole fractions */
  for (i = 1; i <= mr; i++)
  {
    if ( si[i] == SINGLE )
    {
      if ( w[i] == 0.0)
      {
        wt[i] = 0.0;          /* single-species phase */
        fel[i] = 0.0;     /* wt% */
      }
      else if ( w[i] > 0.0 )
      {
        wt[i] = 1.0;          /* mole fraction */
        fel[i] = 100.0;       /* wt% */
      }
    }
    else
    {
      if (tmole1[si[i]] != 0.0)
      {
        wt[i] = w[i]/tmole[si[i]];    /* all other phases */
        fel[i] = fel[i]/tmole1[si[i]]*100.0;
      }
      else fel[i] = 0.0;
    }
  }
  /* --------------- print out the results -------------------- */
  /* --------- to be included ------------*/
  fprintf(debug_prn_file, CONDENSED_ON);
  if (ICONV) fprintf(debug_prn_file,"\n Convergence criterion not statisfied.");
  fprintf(debug_prn_file,"\n Number of iterations = %3d",iter);
  fprintf(debug_prn_file,"\n Number of times a new stoichiometry matrix was evaluated = %3d\n",nopt);
  fprintf(debug_prn_file,"Species       Equilibrium moles  Mole Fraction  DG/RT Reaction  Mass Perc  ln gamma\n");
  /* All the components */
  for (i = 1; i <= nc; i++) {
    for(j = 0; j < 10; j++) fprintf(debug_prn_file,"%c",comp_array[VCS_specie[ind[i]]].name[j]);
    fprintf(debug_prn_file,"    ");
    fprintf(debug_prn_file,"%+.7e  %+.7e                    %8.4f", w[i], wt[i], fel[i] );
    if (VCS_act[ind[i]] != MARKER)
      fprintf(debug_prn_file,"   %8.4f", parse_opcode(VCS_act[ind[i]]));
    fprintf(debug_prn_file,"\n");
  }
  /* Now all the reactions */
  for (i = nc+1;i <= mr; i++) {
    for(j = 0; j < 10; j++) fprintf(debug_prn_file,"%c",comp_array[VCS_specie[ind[i]]].name[j]);
    fprintf(debug_prn_file,"    ");
    fprintf(debug_prn_file,"%+.7e  %+.7e  %+.7e    %8.4f", w[i], wt[i], dg[i], fel[i]);
    if (VCS_act[ind[i]] != MARKER)
      fprintf(debug_prn_file,"   %8.4f", parse_opcode(VCS_act[ind[i]]));
    fprintf(debug_prn_file,"\n");
  }
  /* print the deleted species */
  if ( mr != m ) {
    fprintf(debug_prn_file,"\n    LESS THAN 1.E-32 MOLES \n");
    mr1 = mr + 1;
    for (i = mr1; i <= m; i++) {
      for(j = 0; j <= 10; j++) fprintf(debug_prn_file,"%c",comp_array[VCS_specie[ind[i]]].name[j]);
      fprintf(debug_prn_file,"\n");
    }
  }
  g = 0.0;
  for (i = 1; i <= no_phases; i++)
    if ( tmole[i] > 0.0 && tinert[i] > 0.0 ) g += tinert[i]*log((double)tinert[i]/tmole[i]);
  for (i =1; i <= mr; i++) g += w[i]*fe[i];
  fprintf(debug_prn_file,"\nDG/RT = %+.7e",g);
  for (i = 1; i <= no_phases; i++)
    fprintf(debug_prn_file,"\nTotal phase %1d moles = %+.7e mass = %+.7e kg",i,tmole[i], tmole1[i]/1000.0);
  fprintf(debug_prn_file,"\n\nElemental abundances");
  for (i =1; i <= ne; i++) fprintf(debug_prn_file,"\n           %2d %s   %+.7e",i,Elem_txt[VCS_elem[i]], ga[i]);
  #endif
  #ifdef DEBUG2
  fprintf(debug_prn_file,"\n\nStoichiometric matrix sc[i][j]\n\n            ");
  for (i = 1; i <= nc; i++) {
    for(j = 0; j < 8; j++) fprintf(debug_prn_file,"%c",comp_array[VCS_specie[ind[i]]].name[j]);
    fprintf(debug_prn_file,"  ");
  }
  /* now lets calculate ln K for the non-ideal case */
  fprintf(debug_prn_file," Delta Gø  ln K(non-ideal)\n");
  for (i = 1; i <= nr; i++) {
    L = ir[i];
    lnK = ff[L];    /* - ln K */
    g = da[L];    /* delta G */
    for (j = 1; j <= nc; j++)
    {
     g += sc[j][i]*da[j];
     lnK += sc[j][i]*ff[j];
    }
    fprintf(debug_prn_file," 0 = ");
    for (j = 0; j < 7; j++) fprintf(debug_prn_file,"%c",comp_array[VCS_specie[ind[L]]].name[j]);
    fprintf(debug_prn_file," ");
    for (j = 1; j <= nc; j++)
      fprintf(debug_prn_file,"%4.2g     ",sc[j][i]);
    fprintf(debug_prn_file,"%-8.6f  %-8.6f\n", g, -lnK);
  }
  fprintf(debug_prn_file,CONDENSED_OFF);
  #endif
  return ( route = RESULT );
}


#ifdef DEBUG2
void prin_immed(void)
{
  int i,j;
  double g;

  fprintf(debug_prn_file,"\n\nImmediate results, after forcer........");
  fprintf(debug_prn_file,"\n\nIteration %3d    Iterations since last evaluation of STOICH. %3d",iter,it1-1);
  fprintf(debug_prn_file,"\nNumber of basis optimisations = %3d  Immediate counter iti = %3d",nopt,iti);
  fprintf(debug_prn_file,"\nSpecies  Initial moles      Final moles         MU/RT         delta G/RT\n");
  for (i = 1; i <= nc; i++) {
    for(j = 0; j < 7; j++) fprintf(debug_prn_file,"%c",comp_array[VCS_specie[ind[i]]].name[j]);
    fprintf(debug_prn_file,"      ");
    fprintf(debug_prn_file,"%+.7e  %+.7e  %+.7e\n",wt[i],w[i],fe[i]);
  }
  for (i = nc+1; i <= mr; i++) {
    for(j = 0; j < 7; j++) fprintf(debug_prn_file,"%c",comp_array[VCS_specie[ind[i]]].name[j]);
    fprintf(debug_prn_file,"      ");
    fprintf(debug_prn_file,"%+.7e  %+.7e  %+.7e  %+.7e\n",wt[i],w[i],fe[i],dg[i-nc]);
  }
  g = 0.0;
  for (i = 1; i <= no_phases; i++)
    if ( tmole[i] > 0.0 && tinert[i] > 0.0 ) g += tinert[i]*log((double)tinert[i]/tmole[i]);
  for (i =1; i <= mr; i++) g += w[i] * fe[i];
  fprintf(debug_prn_file,"\nDG/RT              = %+.7e",g);
  elab();
  fprintf(debug_prn_file,"\n\nElemental abundances");
  for (i =1; i <= ne; i++) fprintf(debug_prn_file,"\n           %1d    %+.7e",i,ga[i]);
}
#endif




/* ---- allocate a two dimensional array of doubles in a special way so ----*/
/* ---- that we can pass a two dimensional array to functions without   ----*/
/* ---- specifying the second dimension in those functions argument list ---*/
DMATRIX alloc_d_matrix( int size_x, int size_y, int start_index_x, int start_index_y )
{
  int i;
  double **d_arr, **d_tmp;

  if( !(d_arr = (double **)malloc( size_x * sizeof( void *) )) ) return NULL;
  d_tmp = d_arr;
  for(i=0; i<size_x; i++)
  {
    if(!(*d_tmp = (double *)malloc( size_y * sizeof( double ) ) - start_index_y) ) return NULL;
    d_tmp++;
  }

  return( d_arr - start_index_x );
}

void free_d_matrix( DMATRIX array, int size_x, int size_y, int start_index_x, int start_index_y )
{
  int i;

  array = array + start_index_x;

  for(i=0; i<size_x; i++)
  {
    free( *(array+i) + start_index_y );
  }
  free( array );
}

DARRAY alloc_d_array( int size_x, int start_index_x )
{
  double *d_arr;

  d_arr = malloc( size_x * sizeof( double ) );

  return( d_arr - start_index_x );
}

void free_d_array( DARRAY array, int start_index_x )
{
    free( array + start_index_x );
}

int alloc_vcs_memory( void )
{
    if (!tmole &&
        !(tmole = alloc_d_array( MAXPHASE, 0 )) ) return NO_MEMORY;    /* start at 0 */
    if (!tinert &&
       !(tinert = alloc_d_array( MAXPHASE, 0 )) ) return NO_MEMORY;    /* start at 0 */
    if (!delta_mole &&
       !(delta_mole = alloc_d_array( MAXPHASE, 0 )) ) return NO_MEMORY;/* start at 0 */
    if (!tmole1 &&
       !(tmole1 = alloc_d_array( MAXPHASE, 0 )) ) return NO_MEMORY;    /* start at 0 */
    if (!w &&
       !(w = alloc_d_array( MAXSPECIE, 0 )) ) return NO_MEMORY;
    if (!wt &&
       !(wt = alloc_d_array( MAXSPECIE, 0 )) ) return NO_MEMORY;
    if (!fel &&
       !(fel = alloc_d_array( MAXSPECIE, 0 )) ) return NO_MEMORY;
    if (!fe &&
       !(fe = alloc_d_array( MAXSPECIE, 0 )) ) return NO_MEMORY;
    if (!initial_chem_pot &&
       !(initial_chem_pot = alloc_d_array( MAXSPECIE, 0 )) ) return NO_MEMORY;
    if (!ff &&
       !(ff = alloc_d_array( MAXSPECIE, 0 )) ) return NO_MEMORY;
    if (!ds &&
       !(ds = alloc_d_array( MAXSPECIE, 0 )) ) return NO_MEMORY;
    if (!dg &&
       !(dg = alloc_d_array( MAXSPECIE, 0 )) ) return NO_MEMORY;
    if (!da &&
       !(da = alloc_d_array( MAXSPECIE, 0 )) ) return NO_MEMORY;
    if (!aw &&
       !(aw = alloc_d_array( MAXSPECIE, 0 )) ) return NO_MEMORY;
    if (!ga &&
       !(ga = alloc_d_array( MAXELEMENT, 0 )) ) return NO_MEMORY;
    if (!gai &&
       !(gai = alloc_d_array( MAXELEMENT, 0 )) ) return NO_MEMORY;
    if (!sa &&
       !(sa = alloc_d_array( MAXELEMENT, 0 )) ) return NO_MEMORY;
    if (!ss &&
       !(ss = alloc_d_array( MAXELEMENT, 0 )) ) return NO_MEMORY;
  // sc_max_i and sc_max_j are defined in SOLVE3-0.C
    if (!sc &&
     !(sc = alloc_d_matrix( sc_max_i, sc_max_j, 0, 0 )) ) return NO_MEMORY;
    if (!bm &&
       !(bm = alloc_d_matrix( MAXSPECIE, MAXELEMENT, 0, 0 )) ) return NO_MEMORY;
    if (!d_mole &&
       !(d_mole = alloc_d_matrix( MAXPHASE, MAXSPECIE, 0, 0 )) ) return NO_MEMORY;       /* start at 0 for x */
    if (!sm &&
       !(sm = alloc_d_matrix( MAXELEMENT, MAXSPECIE, 0, 0 )) ) return NO_MEMORY;
    return NO_ERR;
}

void free_vcs_memory( void )
{
  if(tmole){free_d_array(tmole, 0); tmole = NULL;};
  if(tinert){free_d_array(tinert, 0); tinert = NULL;}
  if(delta_mole){free_d_array(delta_mole, 0); delta_mole = NULL;}
  if(tmole1){free_d_array(tmole1, 0); tmole1 = NULL;}
  if(w){free_d_array(w, 0); w = NULL;}
  if(wt){free_d_array(wt, 0); wt = NULL;}
  if(fel){free_d_array(fel, 0); fel = NULL;}
  if(fe){free_d_array(fe, 0); fe = NULL;}
  if(initial_chem_pot){free_d_array(initial_chem_pot, 0); initial_chem_pot = NULL;}
  if(ff){free_d_array(ff, 0); ff = NULL;}
  if(ds){free_d_array(ds, 0); ds = NULL;}
  if(dg){free_d_array(dg, 0); dg = NULL;}
  if(da){free_d_array(da, 0); da = NULL;}
  if(aw){free_d_array(aw, 0); aw = NULL;}
  if(ga){free_d_array(ga, 0); ga = NULL;}
  if(gai){free_d_array(gai, 0); gai = NULL;}
  if(sa){free_d_array(sa, 0); sa = NULL;}
  if(ss){free_d_array(ss, 0); ss = NULL;}
  if(sc){free_d_matrix(sc, MAXELEMENT+2, MAXSPECIE+MAXELEMENT, 0, 0 ); sc = NULL;}
  if(bm){free_d_matrix(bm, MAXSPECIE, MAXELEMENT, 0, 0 ); bm = NULL;}
  if(d_mole){free_d_matrix(d_mole, MAXPHASE, MAXSPECIE, 0, 0 ); d_mole = NULL;}
  if(sm){free_d_matrix( sm, MAXELEMENT, MAXSPECIE, 0, 0 ); sm = NULL;}
}


#if 0
/* ------------------------------------------------------------------------*/
/*  Cut this out in the final version - included to test the concept       */
/*  See SENSTIV.C                                */
/* ------------------------------------------------------------------------*/
/* we are using ir[], sc[][], dg[] & fel[] here temporaly */
void calc_sen(void)
{
  int i, j, k, count, solved = FALSE;
  double   d,
      target_Mn,
      target_Si,
      initial_Mn,
      initial_Si;

  /* we must first build the coefficient matrix for the linear */
  /* equations 14 and 15 in Smith(1969) */
  /* We will not include the effect of Single-specie-phases ( third term */
  /* on lefthand side of equation 14 */
  /* lets first delete any zero phases and SINGLE specie phases */
  k = mr;
  for (i = 1; i <= k; i++)  /* only upto the non-deleted specie (mr) */
  {
    if ( /* (si[i] == SINGLE) || */ (tmole[si[i]] == 0.0) )
    {
      delete(i, TRUE);  /* TRUE=> only rearrange arrays */
      mr--;
      no_specie_phase[si[i]] = 0;
    }
  }
  no_phases = 0;
  for (i = 1; i < MAXPHASE; i++)
    if (no_specie_phase[i] != 0) no_phases++; /* Count the number of phases */
  check_phase_distr();  /* redistribute phase no allocation */
  /* now generate the coefficient matrix a[][] */
  k = calc_coef_mat();
  #ifdef DEBUG2
  fprintf(debug_prn_file,CONDENSED_ON);
  fprintf(debug_prn_file,"\nSpecies     Equilibrium est.   Formula vector\n");
  for (i = 1; i <= mr; i++) {
    for(j = 0; j < 7; j++) fprintf(debug_prn_file,"%c",comp_array[VCS_specie[ind[i]]].name[j]);
    fprintf(debug_prn_file,"  ");
    fprintf(debug_prn_file,"%+.7e     ",w[i]);
    for(j = 1; j <= ne; j++) fprintf(debug_prn_file,"%2.1f ",bm[i][j]);
    fprintf(debug_prn_file,"   %1d\n",(int)si[i]);
  }
  #endif
  #ifdef DEBUG2
  fprintf(debug_prn_file,"\n\nSensitivity coefficient matrix\n");
  for (i = 1; i <= k; i++)
  {
    for (j = 1; j <= k; j++)
      fprintf(debug_prn_file,"%9.3f ",sc[i][j]);
    fprintf(debug_prn_file,"\n");
  }
  #endif
  /* lets decompose the matrix into LU form */
  ludcmp(sc, k, ir, &d);
  #ifdef DEBUG2
  fprintf(debug_prn_file,"\n\nDecomposed coefficient matrix\n");
  for (i = 1; i <= k; i++)
  {
    for (j = 1; j <= k; j++)
      fprintf(debug_prn_file,"%9.3f ",sc[i][j]);
    fprintf(debug_prn_file,"\n");
  }
  #endif
  /* lets solve the linear equations */
  /* first case - elemental abundance sensitivity */
  /* we must first build the b matrix for the LU decomposed coef matrix */
  /* we will use fel[] as the 'b[] matrix'  */
  for(i=1; i<=ne; i++)
    fel[i] = 0.0; /* second term on righthand side of equation 14 */
  for(i=ne+1; i<=ne+no_phases; i++)
    fel[i] = tinert[i-ne];  /* righthand side of equation 15 for mixed phase  */
  for(i=ne+no_phases+1; i<=k; i++)
    fel[i] = 0.0; /* righthand side of equation 15 for single-specie phases  */
  /* now solve it */
  /* we want to find the new elemental abundance such that the Mn and Si */
  /* in the metal is 10% higher than what it is now, we will iteratively */
  /* solve the problem until the error between the target and actual predicted */
  /* value is less than 1% */
  fprintf(debug_prn_file, "\nTargeted Mn in the metal  = 0.1 perc");
  fprintf(debug_prn_file, "\nTargeted Si in the metal  = 0.02 perc");
  /* first add the mass change to the total mass */
  target_Mn =  (0.1-0.0769)*tmole1[2]/100;
  target_Si =  (0.02-0.0057)*tmole1[2]/100;
  tmole1[2] += target_Mn + target_Si;
  /* now calculate the mole change */
  target_Mn =  (0.1-0.0769)*tmole1[2]/100/c_mole_mass(VCS_specie[ind[7]]);
  target_Si =  (0.02-0.0057)*tmole1[2]/100/c_mole_mass(VCS_specie[ind[9]]);
  initial_Mn = fel[6] = target_Mn;
  initial_Si = fel[4] = target_Si;
  #ifdef DEBUG3
  fprintf(debug_prn_file, "\nTargeted Mn increase in the metal = %f", target_Mn);
  fprintf(debug_prn_file, "\nTargeted Si increase in the metal = %f", target_Si);
  #endif
  count = 0;
  while ( !solved )
  {
    count++;
    lubksb( sc, k, ir, fel);
    #ifdef DEBUG2
    fprintf(debug_prn_file,"\n\nSolved b[] matrix for the combined influence of increasing Si and Mn (10 perc of metal phase)\n");
    for (i = 1; i <= k; i++)
      fprintf(debug_prn_file,"%g  ",fel[i]);
    fprintf(debug_prn_file,CONDENSED_OFF);
    #endif
    /* now lets stuff the results into equation 16 Smith(1969) */
    for(i=1; i<=mr; i++)
    {
      dg[i] = 0.0;
      for(j=1; j<=ne; j++)
      dg[i] += bm[i][j]*fel[j];
      dg[i] += fel[ne+si[i]];   /* difference induced by the change */
      dg[i] *= w[i];          /* now multiply with the original mass */
    }
     #ifdef DEBUG2
     fprintf(debug_prn_file,"\n\nDelta for each specie\n");
     for (i = 1; i <= mr; i++)
     {
      for(j = 0; j < 7; j++) fprintf(debug_prn_file,"%c",comp_array[VCS_specie[ind[i]]].name[j]);
      fprintf(debug_prn_file,"  ");
      fprintf(debug_prn_file,"%g\n",dg[i]);
     }
     #endif
     if ( (fabs(target_Mn-dg[7]) <= 0.01*target_Mn) &&
      (fabs(target_Si-dg[9]) <= 0.01*target_Si) )
     {
      solved = TRUE;
     }
     else
     {
       initial_Mn += target_Mn-dg[7];
       initial_Si += target_Si-dg[9];
       /* we will use fel[] as the 'b[] matrix'  */
       for(i=1; i<=ne; i++)
         fel[i] = 0.0;  /* second term on righthand side of equation 14 */
       for(i=ne+1; i<=ne+no_phases; i++)
         fel[i] = tinert[i-ne]; /* righthand side of equation 15 for mixed phase  */
       for(i=ne+no_phases+1; i<=k; i++)
         fel[i] = 0.0;  /* righthand side of equation 15 for single-specie phases  */
       fel[6] = initial_Mn;
       fel[4] = initial_Si;
     }
  }
  fprintf(debug_prn_file, "\nNew elemental abundance additions for Mn = %f Si = %f\n", initial_Mn, initial_Si);
  fprintf(debug_prn_file, "\nPredicted results .......\n");

  /* add the predicted changes and print the result */
  ga[6] += initial_Mn;
  ga[4] += initial_Si;
  for(i=1; i<=mr; i++)
  {
    w[i] += dg[i];
  }
  /* now print the results */
  for (i = 1; i < MAXPHASE; i++)
    tmole1[i] = 0.0;
  /* first calculate total mass */
  /* we will use fel as the temp variable for the mass% */
  /* and tmole1 as the total mass for the phase */
  for ( i = 1; i <= mr; i++ )
  {
    fel[i] = c_mole_mass(VCS_specie[ind[i]])*w[i];
    tmole1[si[i]] += fel[i];
  }
  /* calculate mass% */
  for ( i = 1; i <= mr; i++ )
  {

    if (tmole1[si[i]] != 0.0) fel[i] = fel[i]/tmole1[si[i]]*100.0;
    else fel[i] = 0.0;
  }
  for (i = 1; i <= mr; i++) {     /* calc final mole fractions */
    if ( si[i] == SINGLE ) {
      if ( w[i] == 0.0) wt[i] = 0.0;          /* single-species phase */
      else if ( w[i] > 0.0 ) wt[i] = 1.0;
    }
    else {
      if ( tmole[si[i]] != 0.0) wt[i] = w[i]/tmole[si[i]];    /* all other phases */
    }
  }
  /* --------------- print out the results -------------------- */
  fprintf(debug_prn_file, "\nIterations to obtain target = %d\n", count);
  fprintf(debug_prn_file,"Species       Equilibrium moles  Mole Fraction  DG/RT Reaction  Mass Perc  ln gamma\n");
  for (i = 1; i <= mr; i++) {
    for(j = 0; j < 7; j++) fprintf(debug_prn_file,"%c",comp_array[VCS_specie[ind[i]]].name[j]);
    fprintf(debug_prn_file,"    ");
    fprintf(debug_prn_file,"%+.7e  %+.7e                    %8.4f", w[i], wt[i], fel[i] );
    fprintf(debug_prn_file,"\n");
  }
  fprintf(debug_prn_file,"\n\nElemental abundances");
  for (i =1; i <= ne; i++) fprintf(debug_prn_file,"\n           %1d    %+.7e",i,ga[i]);
  fprintf(debug_prn_file,"\n");
}

/* this routine will determine the coefficient matrix for SMITH(1969) */
/* we will use the stoichiometric matrix sc[][] as the storage space  */
/* for the  'coeficient matrix a[][]' in the article. sc[][] however is not correctly */
/* dimensioned( ne+no_phases+no_single_specie_phases must not be greater*/
/* than MAXELEMENT)                           */
int calc_coef_mat(void)
{
  int i, j, k, p, s;


  /* initialise sc[][] */
  for (i=0; i<MAXELEMENT; i++)
    for (j=0; j<=MAXELEMENT; j++)
    sc[i][j] = 0.0;
  /* lets do equation 14 which was expanded( see note attached to article)*/
  for (j=1; j<=ne; j++)
  {
    /* first term on lefthand side */
    for (k=1; k<=ne; k++)
    {
    for (p=1; p<=no_phases; p++)
    {
     for(i=1; i<=mr; i++)
       if (si[i]==p) sc[j][k] += bm[i][j]*bm[i][k]*w[i];
    }
    }
    /* second term on lefthand side */
    for (p=1; p<=no_phases; p++)
    {
     for(i=1; i<=mr; i++)
     if (si[i]==p) sc[j][ne+p] += bm[i][j]*w[i];
    }
    /* third term on lefthand side */
    /* for the pure condensed phases */
    s = ne+no_phases+1;
    for(i=1; i<=mr; i++)
    {
     if (si[i]==SINGLE)
     {
       sc[j][s] = bm[s][j]*w[s];
       s++;
     }
    }
  }
  /* now for equation 15 in article */
  /* first do upto the number of mixed phases */
  j = ne+1;
  for (p=1; p<=no_phases; p++)
  {
    /* first term on lefthand side */
    for(k=1; k<=ne; k++)
    {
      for(i=1; i<=mr; i++)
      if (si[i]==p) sc[j][k] += bm[i][k]*w[i];
    }
    /* second term on lefthand side */
    /* the inerts per phase */
    sc[j][ne+p] -= tinert[p];
    j++;    /* next row */
  }
  /* now for the pure condensed (single-specie phases) of equation 15 */
  /* first term on lefthand side */
  for(i=1; i<=mr; i++)
  {
    if (si[i]==SINGLE)
    {
    for(k=1; k<=ne; k++)
      sc[j][k] = bm[i][k]*w[i];
    j++;    /* next row */
    }
  }
  /* now check if the indexes are correct */
  if ( (j != s) || (j-1 >= MAXELEMENT) )
  {
    strcpy( error, "\nIncorrect number of equations j= %d s= %d Max= %d", j-1, s-1, MAXELEMENT);
    longjmp( e_buf, 1 );
  }
  return j-1;
}



#define TINY 1.0e-20


/* This decomposes a matrix a[][] into the LU form */
/* From NUMERICAL RECIPES IN C */
void ludcmp(double a[][MAXELEMENT+MAXSPECIE], int n, int indx[], double *d)
{
  int   i, imax, j, k;
  double big, dum, sum, temp;
  double *vv;


  if ( NULL == (vv = (double *)malloc( (unsigned) (n)*sizeof(double)) ) )
  {
    printf("\nNot enough memory in LU decompose!");
    exit(1);
  }
  vv--;       /* subtract 1 becuase we start incrementing from 1 - gipo the pointer*/
  *d = 1.0;
  for(i=1; i<=n; i++)
  {
    big = 0.0;
    for(j=1; j<=n; j++)
     if ( (temp=fabs(a[i][j])) > big ) big = temp;
    if ( big == 0.0 )
    {
      printf("\nSingular matrix in routine LUDCMP");
      exit(1);
    }
    vv[i] = 1.0/big;
  }
  for (j=1; j<=n; j++)
  {
    for (i=1; i<j; i++)
    {
    sum = a[i][j];
    for (k=1; k<i; k++) sum -= a[i][k]*a[k][j];
    a[i][j]=sum;
    }
    big = 0.0;
    for (i=j; i<=n; i++)
    {
    sum = a[i][j];
    for (k=1; k<j; k++) sum -= a[i][k]*a[k][j];
    a[i][j]=sum;
    if ( (dum = vv[i]*fabs(sum)) >= big )
    {
      big = dum;
      imax = i;
    }
    }
    if ( j != imax )
    {
    for (k=1; k<=n; k++)
    {
      dum = a[imax][k];
      a[imax][k] = a[j][k];
      a[j][k] = dum;
    }
    *d = -(*d);
    vv[imax] = vv[j];
    }
    indx[j] = imax;
    if (a[j][j] == 0.0) a[j][j] = TINY;
    if (j != n)
    {
    dum = 1.0/(a[j][j]);
    for (i=j+1; i<=n; i++) a[i][j] *= dum;
    }
  }
      /* add 1 before we free */
  free( (char *) (vv+1));
}

/* This solves the LU decomposed matrix a[][] based on b[] */
/* From NUMERICAL RECIPES IN C */
void lubksb( double a[][MAXELEMENT+MAXSPECIE], int n, int indx[], double b[])
{
  int i, ii=0, ip, j;
  double sum;

  for (i=1; i<=n; i++)
  {
    ip = indx[i];
    sum = b[ip];
    b[ip] = b[i];
    if (ii)
      for (j=ii; j<=i-1; j++) sum -= a[i][j]*b[j];
    else if (sum) ii = i;
    b[i] = sum;
  }
  for (i=n; i>=1; i--)
  {
    sum = b[i];
    for (j=i+1; j<=n; j++) sum -= a[i][j]*b[j];
    b[i] = sum/a[i][i];
  }
}
#endif
