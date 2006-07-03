//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 /*
 * Preprocessed by tbutoi version 3.43
 */
 
/*
 * Translated by tbxlator version 3.43
 */
 
#include "stdafx.h"
//#include <stddef.h>
//#include <alloc.h>
#include <stdio.h>
 
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
//#include <dos.h>
#include <math.h>
 
#include "sc_defs.h"
#include "mtx_glbl.h"
#include "mtx_ordr.h"
 
#define extern
#define NOTEXTERN
#include "mtx_mtrx.h"
#undef NOTEXTERN
#undef extern
 
#include "debugnew.h" // must follow all IMPLEMENT_DYNCREATE & IMPLEMENT_SERIAL

////TODO TO BE Removed
 
/*-----------------------------------------------------------------------------*/
 
void kill_list(node **curr)
  {
  if (* curr != NULL)
    {
    //kill_list(&(* curr)->right);
    //delete(* curr);
    //* curr = NULL;
    node *Del = *curr,*Next;
    while (Del)
      {
      Next = Del->right;
      //dbgpln("%p del node",Del);
      delete Del;
      Del = Next;
      }
    (*curr)=NULL;
    }

  }
 
/**
static void kill_list(node **curr)
  {
  if (* curr != NULL)
    {
    kill_list(&(* curr)->right);
    delete(* curr);
    * curr = NULL;
    }
  }
**/

/*-----------------------------------------------------------------------------*/
 
SpM_Mtrx::SpM_Mtrx(dword n):
   SpM_Ordr(n)
  {
  dword i;
  head init;
 
  nonzeros = 0;
  fill_ins = 0;
  errors = SMTX_NONE;
  firstsolve = 1;
  size = n;
  rowarray.SetSize(Max(rowarray.GetSize(), (int)(size+BUG_EXTRA)));
  colarray.SetSize(Max(colarray.GetSize(), (int)(size+BUG_EXTRA)));
  rowpermutations.SetSize(Max(rowpermutations.GetSize(), (int)(size+BUG_EXTRA)));
  colpermutations.SetSize(Max(colpermutations.GetSize(), (int)(size+BUG_EXTRA)));

  init.nonzerocount = 0;
  init.next = NULL;
  for (i = 1; i <= size; i++)
    {
    rowarray[ (i) - 1] = init;
    colarray[ (i) - 1] = init;
    rowpermutations[ (i) - 1] = i;
    colpermutations[ (i) - 1] = i;
    }
 
  from     = new node;                     
  too      = new node;                      
  tempb.ExpandTo(size+BUG_EXTRA);     
  tempsoln.ExpandTo(size+BUG_EXTRA);         
  }

/*-----------------------------------------------------------------------------*/
 
SpM_Mtrx::~SpM_Mtrx()
  {
  dword i;

  delete too;
  delete from;
  
  for (i = 1; i <= size; i++)
    {
    kill_list(& rowarray[(i) - 1].next);
    }
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::Clear(dword n)
  {
  dword i;
  delete too;
  delete from;
  
  for (i = 1; i <= size; i++)
    kill_list(& rowarray[(i) - 1].next);

  SpM_Ordr::Clear(n);

  head init;
 
  nonzeros = 0;
  fill_ins = 0;
  errors = SMTX_NONE;
  firstsolve = 1;
  size = n;
  rowarray.SetSize(Max(rowarray.GetSize(), (int)(size+BUG_EXTRA)));
  colarray.SetSize(Max(colarray.GetSize(), (int)(size+BUG_EXTRA)));
  rowpermutations.SetSize(Max(rowpermutations.GetSize(), (int)(size+BUG_EXTRA)));
  colpermutations.SetSize(Max(colpermutations.GetSize(), (int)(size+BUG_EXTRA)));

  init.nonzerocount = 0;
  init.next = NULL;
  for (i = 1; i <= size; i++)
    {
    rowarray[ (i) - 1] = init;
    colarray[ (i) - 1] = init;
    rowpermutations[ (i) - 1] = i;
    colpermutations[ (i) - 1] = i;
    }
 
  from     = new node;                     
  too      = new node;                      
  tempb.ExpandTo(size+BUG_EXTRA);     
  tempsoln.ExpandTo(size+BUG_EXTRA);         
 
  };

/*-----------------------------------------------------------------------------*/
 
flag  SpM_Mtrx::Singular()
  {
  dword k;
  flag Flag;
  flag function;
 
  k = 1;
  Flag = FALSE;
 
  while ((! Flag) && (k <= size))
    {
    if ((rowarray[ (k) - 1].next == NULL) || (colarray[ (k) - 1].next == NULL))
      Flag = TRUE;
    k++;
    }
 
  function = Flag;
 
  return (function);
  }
 
/*-----------------------------------------------------------------------------*/
 
dword SpM_Mtrx::rownonzerocount(dword row)
  {
  dword function;
  function = rowarray[ (row) - 1].nonzerocount;
  return (function);
  }
 
/*-----------------------------------------------------------------------------*/
 
dword SpM_Mtrx::colnonzerocount(dword col)
  {
  dword function;
  function = colarray[ (col) - 1].nonzerocount;
  return (function);
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::createordering()
  {
  dword i;
 
  for (i = size; i >= 1; i--)
    {
    SpM_Ordr::rowinsert(i, rowarray[ (i) - 1].nonzerocount);
    SpM_Ordr::colinsert(i, colarray[ (i) - 1].nonzerocount);
    }
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::colinsert(dword colpos,
             dword rowpos,
             node *newnode)
  {
  /* NBB!! - After a call to this procedure you must perform the following */
 
 
  /* statement if a NEW element is to be placed in the matrix :            */
  /* INC( ColArray[ColPos].NonZeroCount ); - This has not been placed in   */
  /* this procedure so that this same procedure can be used in Procedure   */
  /* SwopRow.                                                              */
 
  node *prev,
               * curr;
  prev = colarray[ (colpos) - 1].next;
  if ((prev == NULL) || (prev->row > rowpos))
    {
    colarray[ (colpos) - 1].next = newnode;
    newnode->down = prev;   /* Beginning of list */
    }
  else
    {                       /* Middle or end of entries */
    curr = prev->down;
    while ((curr != NULL) && (curr->row < rowpos))
      {
      prev = curr;
      curr = curr->down;   /* Find the correct position */
      }
    newnode->down = curr;  /* perform the insert. */
    prev->down = newnode;
    }
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::colremove(dword colpos,
             dword rowpos)
  {
  /* Pre : SpM_Mtrx::Singular = FALSE and the element does exist. */
 
  node *prev,
               * curr;
  prev = colarray[ (colpos) - 1].next;
  if (prev->row == rowpos)
    colarray[ (colpos) - 1].next = prev->down; 
  /* Beginning of list */
  else
    {
    /* Middle or end of entries */
    curr = prev->down;
    while (curr && curr->row != rowpos)
      {
      prev = curr;
      curr = curr->down;  /* Find the element */
      }
    prev->down = curr->down;
    }
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::insertelement1(dword rowpos,
             dword colpos,
             double value)
  {
  node *curr, * prev, * newnode;
 
  curr = rowarray[ (rowpos) - 1].next;
  if ((curr == NULL) || (curr->col > colpos))
    {
    /* Beginning of list */
//MallocSW.Start();
    newnode = new node;
//MallocSW.Stop();
    //dbgpln("%p new node1",newnode);
 
    /* Create and fill a new node */
 
    newnode->value = value;
    newnode->row = rowpos;
    newnode->col = colpos;
    newnode->right = curr;
    rowarray[ (rowpos) - 1].next = newnode;
    colinsert(colpos, rowpos, newnode);
    colarray[ (colpos) - 1].nonzerocount++; /* another nonzero in this col */
    rowarray[ (rowpos) - 1].nonzerocount++; /* Another nonzero in this row */
    nonzeros++;                             /* Global non-zero count */
    }
  else
    {
    /* Middle or end of list */
    prev = curr;
    curr = prev->right;
    while ((curr != NULL) && (curr->col <= colpos))
      {
      prev = curr;
      curr = curr->right;    /* Find the position */
      }
    if (prev->col == colpos)  /* Already inserted into this position */
      prev->value = value;    /* Only needed for consistancy with Insert2 */
 
    else
      {
//MallocSW.Start();
      newnode = new node;     
//MallocSW.Stop();
      //dbgpln("%p new node2",newnode);
      newnode->value = value;
      newnode->row = rowpos;
      newnode->col = colpos;
      newnode->right = curr;
      prev->right = newnode;   /* Insert in the row list */
      colinsert(colpos, rowpos, newnode);  /* Insert in the col list */
      colarray[ (colpos) - 1].nonzerocount++;  /* another nonzero in this col */
      rowarray[ (rowpos) - 1].nonzerocount++;  /* Another nonzero in this row */
      nonzeros++;                              /* Global non-zero count */
      }
    }
  }
/*-----------------------------------------------------------------------------*/
 
double SpM_Mtrx::getelement1(dword rowpos,dword colpos)
  {
  node *curr, * prev;
 
  curr = rowarray[ (rowpos) - 1].next;
  if ((curr == NULL) || (curr->col > colpos))
    {
    /* Beginning of list */
    return 0.0;
    }
  else
    {
    /* Middle or end of list */
    prev = curr;
    curr = prev->right;
    while ((curr != NULL) && (curr->col <= colpos))
      {
      prev = curr;
      curr = curr->right;    /* Find the position */
      }
    if (prev->col == colpos)  /* Already inserted into this position */
      return prev->value;    /* Only needed for consistancy with Insert2 */
    }
  return 0.0;
  }

/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::insertelement2(dword rowpos,
             dword colpos,
             double value)
  {
  dword newcol,
  newrow;
  node *curr;
  newcol = inversecolperm[ (colpos) - 1];  /* Place value in the */
  newrow = inverserowperm[ (rowpos) - 1];  /* permuted matrix.   */
  curr = rowarray[ (newrow) - 1].next;
  while ((curr != NULL) && (curr->col != newcol))  /* Find the node */
    curr = curr->right;
  if (curr == NULL)    /* Node not found */
    {
    errors = SMTX_INSERT_ERROR;
    return;
    }
  curr->value = value;
 
  }
/*-----------------------------------------------------------------------------*/
 
double SpM_Mtrx::getelement2(dword rowpos, dword colpos)
  {
  dword col, row;
  node *curr;
  col = inversecolperm[ (colpos) - 1];  /* Place value in the */
  row = inverserowperm[ (rowpos) - 1];  /* permuted matrix.   */
  curr = rowarray[ (row) - 1].next;
  while ((curr != NULL) && (curr->col != col))  /* Find the node */
    curr = curr->right;
  if (curr == NULL)    /* Node not found */
    return 0.0;//errors = SMTX_INSERT_ERROR;
  return curr->value;
  }

/*-----------------------------------------------------------------------------*/

void SpM_Mtrx::insertelement(dword rowpos,
      dword colpos,
      double value)
  {
  //(* performinsert) (rowpos, colpos, value);
  if (firstsolve)
    insertelement1(rowpos, colpos, value);
  else  
    insertelement2(rowpos, colpos, value);
  }
 
/*-----------------------------------------------------------------------------*/

double SpM_Mtrx::getelement(dword rowpos, dword colpos)
  {
  //(* performinsert) (rowpos, colpos, value);
  if (firstsolve)
    return getelement1(rowpos, colpos);
  else  
    return getelement2(rowpos, colpos);
  }
 
/*-----------------------------------------------------------------------------*/
 
 
 
void SpM_Mtrx::swoprow(dword fromrow,
      dword torow)
  {
  node *currf,
               * currt,
               * prevf,
               * prevt,
               * temp;
  dword tempcount1,
  tempcount2;
  double tempvalue;
 
  prevf = from;
  prevt = too;
  prevf->right = rowarray[ (fromrow) - 1].next;
  /* Copy in phase */
 
  prevt->right = rowarray[ (torow) - 1].next;
  currf = prevf->right;
  currt = prevt->right;
  tempcount1 = rowarray[ (torow) - 1].nonzerocount;
 
  /* Swop non zero count */
 
  tempcount2 = rowarray[ (fromrow) - 1].nonzerocount;
  SpM_Ordr::rowchangeordering(fromrow, tempcount2, tempcount1);
  SpM_Ordr::rowchangeordering(torow, tempcount1, tempcount2);
  rowarray[ (torow) - 1].nonzerocount = tempcount2;
  rowarray[ (fromrow) - 1].nonzerocount = tempcount1;
  tempcount1 = rowpermutations[ (fromrow) - 1];                    /* Record the   */
  rowpermutations[ (fromrow) - 1] = rowpermutations[ (torow) - 1]; /* permutation. */
  rowpermutations[ (torow) - 1] = tempcount1;                      
  /*    WHILE (CurrF <> NULL) OR (CurrT <> NULL) DO BEGIN  WWS */
 
  while ((currf != NULL) && (currt != NULL))
    {
    if (currf->col == currt->col)
      {
      /* Swop Values */
      tempvalue = currf->value;
      /* No pointers to change */
 
      currf->value = currt->value;
      currt->value = tempvalue;
      prevf = currf;
      currf = currf->right;
      /* Advance pointers */
 
      prevt = currt;
      currt = currt->right;
      }
    else
      {
      if (currf->col > currt->col)
        {
        temp = currt;
        prevt->right = currt->right;
        colremove(temp->col, temp->row);
        prevf->right = temp;
        temp->right = currf;
        temp->row = fromrow;
        colinsert(temp->col, temp->row, temp);
        prevf = temp;
        currt = prevt->right;
        }
      else
        {
        /* CurrF^.Col < CurrT^.Col */
 
        temp = currf;
        prevf->right = currf->right;
        colremove(temp->col, temp->row);
        prevt->right = temp;
        temp->right = currt;
        temp->row = torow;
        colinsert(temp->col, temp->row, temp);
        prevt = temp;
        currf = prevf->right;
        }
      }
    }
 
  if (currt == NULL)
    {
    /*Append the rest of the row*/
    prevt->right = currf;
    prevf->right = NULL;
    currt = currf;
    while (currt != NULL)
      {
      colremove(currt->col, currt->row);
      currt->row = torow;
      colinsert(currt->col, torow, currt);
      currt = currt->right;
      /* Restore the downward increasing */
      }
    /* column order after a row swop . */
    }
  if (currf == NULL)
    {
    /* Append the rest of the row */
    prevf->right = currt;
    prevt->right = NULL;
    currf = currt;
    while (currf != NULL)
      {
      colremove(currf->col, currf->row);
      currf->row = fromrow;
      colinsert(currf->col, fromrow, currf);
      currf = currf->right;
      }
    }
  rowarray[ (fromrow) - 1].next = from->right;  /* Copy out phase */
  rowarray[ (torow) - 1].next = too->right;
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::rowremove(dword colpos,
             dword rowpos)
  {
  /* Pre : SpM_Mtrx::Singular = FALSE and the element does exist. */
 
  node *prev,
               * curr;
 
  prev = rowarray[ (rowpos) - 1].next;
  if (prev->col == colpos)
    rowarray[ (rowpos) - 1].next = prev->right;   /* Beginning of list */
  else
    {
    /* Middle or end of entries */
    curr = prev->right;
    while (curr && curr->col != colpos)
      {
      prev = curr;
      curr = curr->right;   /* Find the element */
      }
    prev->right = curr->right;
    }
  }
 
/*-----------------------------------------------------------------------------*/
 
 
 
void SpM_Mtrx::rowinsert(dword colpos,
             dword rowpos,
             node *newnode)
  {
  node *prev,
               * curr;
 
  prev = rowarray[ (rowpos) - 1].next;
  if ((prev == NULL) || (prev->col > colpos))
    {
    rowarray[ (rowpos) - 1].next = newnode;
    newnode->right = prev;  /* Beginning of list */
    }
  else
    {
    /* Middle or end of entries */
 
    curr = prev->right;
    while ((curr != NULL) && (curr->col < colpos))
      {
      prev = curr;
      curr = curr->right;  /* Find the correct position */
      }
    newnode->right = curr;
    /* perform the insert. */
 
    prev->right = newnode;
    }
  }
 
/*-----------------------------------------------------------------------------*/
 
 
 
void SpM_Mtrx::swopcol(dword fromcol,
      dword tocol)
  {
  node *currf,
               * currt,
               * prevf,
               * prevt,
               * temp;
  dword tempcount1,
  tempcount2;
  double tempvalue;
 
  prevf = from;
  prevt = too;
  prevf->down = colarray[ (fromcol) - 1].next;
  /* Copy in phase */
 
 
  prevt->down = colarray[ (tocol) - 1].next;
  currf = prevf->down;
  currt = prevt->down;
  tempcount1 = colarray[ (tocol) - 1].nonzerocount;
 
  /* Swop non zero count*/
 
  tempcount2 = colarray[ (fromcol) - 1].nonzerocount;
  SpM_Ordr::colchangeordering(fromcol, tempcount2, tempcount1);
  SpM_Ordr::colchangeordering(tocol, tempcount1, tempcount2);
  colarray[ (tocol) - 1].nonzerocount = tempcount2;
  colarray[ (fromcol) - 1].nonzerocount = tempcount1;
  tempcount1 = colpermutations[ (fromcol) - 1];
  /* Record the   */
 
 
  colpermutations[ (fromcol) - 1] = colpermutations[ (tocol) - 1];
 
  /* permutation. */
 
  colpermutations[ (tocol) - 1] = tempcount1;
 
  /*   WHILE (CurrF <> NULL) OR (CurrT <> NULL) DO BEGIN  WWS */
 
  while ((currf != NULL) && (currt != NULL))
    {
    if (currf->row == currt->row)
      {
      /* Swop Values */
 
 
      tempvalue = currf->value;
      /* No pointers to change */
 
 
      currf->value = currt->value;
      currt->value = tempvalue;
      prevf = currf;
      currf = currf->down;
      /* Advance pointers */
 
 
      prevt = currt;
      currt = currt->down;
      }
    else
      {
      if (currf->row > currt->row)
        {
        temp = currt;
        prevt->down = currt->down;
        rowremove(temp->col, temp->row);
        prevf->down = temp;
        temp->down = currf;
        temp->col = fromcol;
        rowinsert(temp->col, temp->row, temp);
        prevf = temp;
        currt = prevt->down;
        }
      else
        {
        /* CurrF^.Row < CurrT^.Row */
 
 
        temp = currf;
        prevf->down = currf->down;
        rowremove(temp->col, temp->row);
        prevt->down = temp;
        temp->down = currt;
        temp->col = tocol;
        rowinsert(temp->col, temp->row, temp);
        prevt = temp;
        currf = prevf->down;
        }
      /*If*/
 
 
      }
    /*Outer If*/
 
 
    }
  /*While*/
 
 
 
  if (currt == NULL)
    {
    /*Append the rest of the row*/
 
 
    prevt->down = currf;
    prevf->down = NULL;
    currt = currf;
    while (currt != NULL)
      {
      rowremove(currt->col, currt->row);
      currt->col = tocol;
      rowinsert(tocol, currt->row, currt);
      currt = currt->down;
      }
    }
  if (currf == NULL)
    {
    prevf->down = currt;
    prevt->down = NULL;
    currf = currt;
    while (currf != NULL)
      {
      rowremove(currf->col, currf->row);
      currf->col = fromcol;
      rowinsert(fromcol, currf->row, currf);
      currf = currf->down;
      }
    }
  colarray[ (fromcol) - 1].next = from->down;
  /* Copy out phase */
 
 
  colarray[ (tocol) - 1].next = too->down;
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::addrows1(dword x,
             dword y,
             double * mult)
  {
  /* X:= X + alpha*Y                                                   */
 
  /* Pre : alpha = value of the pivot element. X[diag]#0 and Y[diag]#0 */
  /*       Diagonal entry is in position  A[Y,Y]. ie. Y = Diag.        */
  node *currx,
               * curry,
               * prevx,
               * prevy,
               * newnode,
               * xdiag;
  dword ri,
  cj,
  diag;
 
  diag = y;
  ri = rowarray[ (x) - 1].nonzerocount - 1; /* Leading entry in row X is */
  prevx = rowarray[ (x) - 1].next;          /* removed in adding the two rows */
  currx = prevx;
  prevy = rowarray[ (y) - 1].next;
  curry = prevy;
  while (curry->col < diag)
    {                 /* go to the diagonal elements of */
 
    prevy = curry;    /* the rows */
    curry = curry->right;
    }
  while (currx->col < diag)
    {
    prevx = currx;
    currx = currx->right;
    }
  if (curry->value == 0.0)
    {                 /* Singular matrix */
    errors = SMTX_SINGULAR;
    return;
    }
  xdiag = currx;
  * mult = - currx->value / curry->value;
  while (curry != NULL)
    {
    if ((currx == NULL) || (currx->col > curry->col))
      {               /* fill-in */
      fill_ins++;     /* Global fill-in count */
 
//MallocSW.Start();
      newnode = new node;    
//MallocSW.Stop();
      //dbgpln("%p new node3",newnode);
      newnode->value = curry->value * * mult;
      newnode->col = curry->col;      /* Fill the node */
      newnode->row = x;
      newnode->right = currx;
      colinsert(newnode->col, x, newnode);
 
      /* One more node in column CurrY^.Col */
 
      cj = colarray[ (curry->col) - 1].nonzerocount;
      colarray[ (curry->col) - 1].nonzerocount++;
 
      /* Alter column and row orderings: */
 
      ri++;
      SpM_Ordr::colchangeordering(curry->col, cj, cj + 1);
 
      /* Advance pointers */
 
      prevx->right = newnode;
      prevx = newnode;
      prevy = curry;
      curry = curry->right;
      }
    else
      {
      /* No fill-in cases */
 
 
      if (currx->col == curry->col)
        {
        currx->value = currx->value + * mult * curry->value;
        prevx = currx;
        currx = currx->right;
        prevy = curry;
        curry = curry->right;
        /* Add adjacent entries */
 
 
        }
      else
        {
 
        /*The column entry in row X does not correspond to a non zero */
        /* entry in row Y.ie. CurrY^.Col > CurrX^.Col */
 
        prevx = currx;
        currx = currx->right;
        }
      /*If*/
 
 
      }
    /*outer if*/
 
 
    }
  /* While */
 
 
  SpM_Ordr::rowchangeordering(x, rowarray[ (x) - 1].nonzerocount, ri);
  rowarray[ (x) - 1].nonzerocount = ri;
  xdiag->value = 0.0; /* Remove this statement and this */
                       /* program will become a virus.   */
 
  }
/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::addrows2(dword x,
             dword y,
             double * mult)
  {
  /* Pre: Adding two rows does not produce any fill-ins. */
 
  node *currx,
               * curry,
               * prevx,
               * prevy,
               * xdiag;
  dword diag;
  diag = y;
  prevx = rowarray[ (x) - 1].next;
  currx = prevx;
  prevy = rowarray[ (y) - 1].next;
  curry = prevy;
  while (curry->col < diag)
    {
    /* go to the diagonal elements of */
 
 
    prevy = curry;
    /* the rows */
 
 
    curry = curry->right;
    }
  while (currx->col < diag)
    {
    prevx = currx;
    currx = currx->right;
    }
  if (curry->value == 0.0)
    {   /* Singular matrix */
    errors = SMTX_SINGULAR;
    return;
    }
  xdiag = currx;
  * mult = - currx->value / curry->value;
  /*   While (CurrY<>NULL) Do Begin  */
 
  // cnm while ((curry != NULL) && (currx->col <= curry->col))
  while ((curry != NULL) && (currx != NULL) && (currx->col <= curry->col))
    {
 
    /* No fill-in case exists now */
 
    if (currx->col == curry->col)
      {
      currx->value = currx->value + * mult * curry->value;
      prevx = currx;
      currx = currx->right;
      prevy = curry;
      curry = curry->right;
      /* Add adjacent entries */
 
 
      }
    else
      {
 
      /*The column entry in row X does not correspond to a non zero */
      /* entry in row Y.ie. CurrY^.Col > CurrX^.Col */
 
      prevx = currx;
      currx = currx->right;
      }
    /*If*/
 
 
    }
  /* While */
 
 
  xdiag->value = 0.0;
  }
/*-----------------------------------------------------------------------------*/
/* X:= X + alpha*Y */
 
void SpM_Mtrx::addrows(dword x,
      dword y,
      double * mult)
  {
  // (* performaddrows) (x, y, mult);
  if (firstsolve)
    addrows1(x, y, mult);
  else  
    addrows2(x, y, mult);
  }
 
/*-----------------------------------------------------------------------------*/
 
 
 
void SpM_Mtrx::searchrow(dword rownum,
      dword * colnum,
      dword * max_fill_ins,
      double * value)
  {
  double max;
  node *curr;
  dword bestcanget,
  temp,
  ri,
  cj;
 
  curr = rowarray[ (rownum) - 1].next;
  max = 0.0;
  while (curr != NULL)
    {
    /* Find largest element */
 
 
    if (fabs(curr->value) > max)
      max = fabs(curr->value);
    curr = curr->right;
    }
  * max_fill_ins = size * size;
  /*N*N*/
 
 
  ri = rowarray[ (rownum) - 1].nonzerocount;
  bestcanget = (ri - 1) * (ri - 1);
  curr = rowarray[ (rownum) - 1].next;
  while ((curr != NULL) && (* max_fill_ins > bestcanget))
    {
    cj = colarray[ (curr->col) - 1].nonzerocount;
    temp = (ri - 1) * (cj - 1);
    if ((temp < * max_fill_ins) && (fabs(curr->value) >= MU * max))
      {
      * max_fill_ins = temp;
      * colnum = curr->col;
      * value = curr->value;
      }
    curr = curr->right;
    }
  /*While*/
 
  }
 
/*-----------------------------------------------------------------------------*/
 
 
 
void SpM_Mtrx::searchcol(dword colnum,
      dword fromrow,
      dword * rownum,
      dword * max_fill_ins,
      double * value,
      flag * valid)
  {
  node *curr;
  dword bestcanget,
  temp,
  ri,
  cj;
  flag found;
 
  * valid = FALSE;
  curr = colarray[ (colnum) - 1].next;
  while ((curr->row < fromrow))
    curr = curr->down;
  /* Only search active */
 
 
  cj = colarray[ (colnum) - 1].nonzerocount;
  /* part of the column */
 
 
  bestcanget = (cj - 1) * cj;
  * max_fill_ins = size * size;
  /*N*N*/
 
 
  while ((curr != NULL) && (* max_fill_ins > bestcanget))
    {
    comparerow(curr->row, colnum, curr->value, & found);
    ri = rowarray[ (curr->row) - 1].nonzerocount;
    temp = (cj - 1) * (ri - 1);
    if (found && (temp < * max_fill_ins))
      {
      * rownum = curr->row;
      * value = curr->value;
      * max_fill_ins = temp;
      * valid = TRUE;
      }
    curr = curr->down;
    }
  }
 
/*-----------------------------------------------------------------------------*/
 
 
 
void SpM_Mtrx::getfirstcol(dword rownum,
      dword * colnum,
      double * value)
  {
  node *curr;
 
 
 
  /* When the entries below the pivot column are made zero during reduction, */
  /* these zeros are not counted as non zeros but are stored in the matrix   */
  /* to restore the entry position when new entries are to be placed in the  */
  /* matrix. Thus, the first non zero entry in a row is not the first node   */
  /* for a row but rather the first non zero entry in that row .If this is   */
  /* not done, then a pivot with the value of zero can be chosen causing     */
  /* divide overflow: see AddRows Pre-condition for entry.                   */
  curr = rowarray[ (rownum) - 1].next;
  while ((curr != NULL) && (curr->value == 0.0))
    curr = curr->right;
  if (curr!=NULL)
    {
    * value = curr->value;
    * colnum = curr->col;
    }
  else
    {
    dbgpln("IsSingular");
    errors= SMTX_SINGULAR;        
    }
 
  }
 
/*-----------------------------------------------------------------------------*/
 
 
 
void SpM_Mtrx::getfirstrow(dword colnum,
      dword * rownum,
      double * value)
  {
  node *curr;
 
  curr = colarray[ (colnum) - 1].next;
  while ((curr != NULL) && (curr->value == 0.0))
    curr = curr->down;
  if (curr!=NULL)
    {
    * value = curr->value;
    * rownum = curr->row;
    }
  else
    {
    errors= SMTX_SINGULAR;        
    dbgpln("IsSingular");
    }
 
  }
 
/*-----------------------------------------------------------------------------*/
 
 
 
void SpM_Mtrx::comparerow(dword rownum,
      dword colnum,
      double value,
      flag * valid)
  {
  node *curr;
  double max;
 
  curr = rowarray[ (rownum) - 1].next;
  max = 0.0;
  while (curr != NULL)
    {
    /* Find largest element */
 
 
    if (fabs(curr->value) > max)
      max = fabs(curr->value);
    curr = curr->right;
    }
  * valid = fabs(value) >= MU * max;
  }
 
/*-----------------------------------------------------------------------------*/
 
 
 
void SpM_Mtrx::getfirstactiverow(dword colnum,
      dword * rownum,
      flag * exists)
  {
 
  rowplacer = colarray[ (colnum) - 1].next;
 
//cnm  while (((rowplacer != NULL) && (rowplacer->row <= colnum)) ||
//cnm      (rowplacer->value == 0.0))
  while ((rowplacer != NULL) && ((rowplacer->row <= colnum) || (rowplacer->value == 0.0)))
    rowplacer = rowplacer->down;
  * exists = rowplacer != NULL;
  if (* exists)
    * rownum = rowplacer->row;
  }
 
/*-----------------------------------------------------------------------------*/
 
 
 
void SpM_Mtrx::getnextactiverow(dword * rownum,
      flag * exists)
  {
  rowplacer = rowplacer->down;
  while ((rowplacer != NULL) && (rowplacer->value == 0.0))
    rowplacer = rowplacer->down;
  * exists = rowplacer != NULL;
  if (* exists)
    * rownum = rowplacer->row;
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::solve2(CDArray & solution, CDArray & b)
  {
  dword diag,
  i,
  rownum;
  double mult,
         value,
         sum;
  node *curr;
  flag exists;
 
  /* Permute B to the internal order */
  for (i = 1; i <= size; i++)
    tempb[ (i) - 1] = b[ (rowpermutations[ (i) - 1]) - 1];
 
  /* Reduce matrix */
 
  for (diag = 1; diag <= size - 1; diag++)
    {
    getfirstactiverow(diag, & rownum, & exists);
    while (exists)
      {
      addrows2(rownum, diag, & mult);
      tempb[ (rownum) - 1] = tempb[ (rownum) - 1] + mult *
      tempb[ (diag) - 1];
      getnextactiverow(& rownum, & exists);
      }
    }
  /*For loop*/
  /* A back substitute algorithm. */
  for (i = size; i >= 1; i--)
    {
    curr = rowarray[ (i) - 1].next;
    while (curr->col != i)
      curr = curr->right;
    /* Find diag element */
 
    value = curr->value;
    curr = curr->right;
    sum = 0.0;
    while (curr != NULL)
      {
      sum = sum + tempsoln[ (curr->col) - 1] * curr->value;
      curr = curr->right;
      }
    if (value != 0.0)
      tempsoln[ (i) - 1] = (tempb[ (i) - 1] - sum) / value;
    else
      tempsoln[ (i) - 1] = tempb[ (i) - 1]; /*Singular !*/
    }
  /* Re-permute solution to correct order */
 
  for (i = 1; i <= size; i++)
    solution[ (colpermutations[ (i) - 1]) - 1] = tempsoln[ (i) - 1];
 
  }
/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::solve1(CDArray & solution,
                      CDArray & b)
  {
  dword i;                             /* A back substitute algorithm.        */
  node *curr;
  double value,
         sum;
 
  for (i = size; i >= 1; i--)
    {
    curr = rowarray[ (i) - 1].next;
    while (curr->col != i)
      curr = curr->right;
    /* Find diag element */
 
 
    value = curr->value;
    curr = curr->right;
    sum = 0.0;
    while (curr != NULL)
      {
      sum = sum + tempsoln[ (curr->col) - 1] * curr->value;
      curr = curr->right;
      }
    tempsoln[ (i) - 1] = (b[ (i) - 1] - sum) / value;
    }
  /* Re-permute to correct order */
 
 
  for (i = 1; i <= size; i++)
    {
    solution[ (colpermutations[ (i) - 1]) - 1] = tempsoln[ (i) - 1];
    }
  /* Convert unit for subsequent problems */
 
 
  firstsolve = 0;
  
  // performsolve = solve2;               //(solution, &b);
  // performaddrows = addrows2;           //(&x,&y,mult);
  // performinsert = insertelement2;      //(&rowpos,&colpos,&value);
  
//MallocSW.Start();
  inverserowperm.SetSize(Max(inverserowperm.GetSize(), (int)(size+BUG_EXTRA)));
  inversecolperm.SetSize(Max(inversecolperm.GetSize(), (int)(size+BUG_EXTRA)));
//  inverserowperm = new dword[size+BUG_EXTRA];// * sizeof (dword)];
//  inversecolperm = new dword[size+BUG_EXTRA];// * sizeof (dword)];
//MallocSW.Stop();
  //dbgpln("%p new inverserowperm",inverserowperm);
  //dbgpln("%p new inversecolperm",inversecolperm);
  for (i = 1; i <= size; i++)
    {
    inverserowperm[ (rowpermutations[ (i) - 1]) - 1] = i;  /* Construct the inverse */
    inversecolperm[ (colpermutations[ (i) - 1]) - 1] = i;  /* row/col permutations. */
    }
  /* This is needed for    */
  /* InsertElement2.       */
 
  }
/*-----------------------------------------------------------------------------*/

void SpM_Mtrx::solve(CDArray & solution,
                     CDArray & b)
  {
  dword i;
  node *curr;
 
  // (* performsolve) (solution, b);
  if (firstsolve)
    solve1(solution, b);
  else  
    solve2(solution, b);
 
  /* Fill the created matrix with zeros before */
  /* using it again for subsequent solves.     */
 
  for (i = 1; i <= size; i++)
    {
    curr = rowarray[ (i) - 1].next;
    while (curr != NULL)
      {
      curr->value = 0.0;
      curr = curr->right;
      }
    }
  errors = SMTX_NONE;
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::report_stats(Mtx_Info *table)
  {
  table->nonzerocount = nonzeros;
  table->num_fill_ins = fill_ins;
  table->sparcity_before = (1.0 - (nonzeros / (double)(size * size))) * 100.0;
  table->sparcity_after = (1.0 - ((nonzeros + fill_ins) / (double)(size * size))) * 100.0;
  }
 
/*-----------------------------------------------------------------------------*/
 
/* There are two versions of Rowprint. One prints the values of the matrix, */
/* which is suitable for a system of order 10 or less. The other procedure  */
/* prints only the positions of the non zero entries represented as '1' and */
/* zero entries are represented by '0'. This is suitable for a system of    */
/* order 79 or less. Rowprint prints out the matrix using the row links.    */
 
void SpM_Mtrx::rowprint(CDArray & b,char Symbolic, int Wide, int Prec)
  {
  /* Ver.1 */
  dword i,
  j,
  k,
  kk, MaxRowLen=0;
  node *row;
  dbglock();
  if (Symbolic)
    {
    dbgpln("Matrix:-----------------------");
    kk = (size >= 1000) ? 1000 : (size >= 100) ? 100 : (size >= 10) ? 10 : 1;

    //if (size >= 1000) kk = 1000
    //else if (size >= 100) kk = 100
    //else if (size >= 10) kk = 10
    //else kk = 1
    for (; kk >= 1; kk /= 10)
      {
      dbgp("     ");
      for (k = 1; k <= size; k++)
        if (k%10==0 || kk==1)
          dbgp("%i",(k/kk)%10);
        else
          dbgp(" ");
      dbgpln("");
      }

    for (i = 1; i <= size; i++)
      {
      row = rowarray[ (i) - 1].next;
      j = 1;
      dword len=0;
      while (row != NULL)
        {
        if (row->col == j)
          {
          if (row->value != 0.0)
            len++;
          row = row->right;
          }
        j++;
        }
      MaxRowLen=Max(len, MaxRowLen);
      }
    }


  /*    FOR i:= 1 TO Size DO Write(Colarray^[i].NonZeroCount:7); Writeln; */
  //char * buff = new char[size];
  Strng Buff;
  Buff.SetLength(size+1);
  for (i = 1; i <= size; i++)
    {
    /*       WriteLn; */
    row = rowarray[ (i) - 1].next;
    /*       Write(RowArray^[i].NonZeroCount,'       '); */
    j = 1;
    if (Symbolic)
      {
      dbgp("%4i:",i);
      int bi=0;
      while (row != NULL)
        {
        if (row->col == j)
          {
          if (row->value > 0.0)
            Buff[bi++]='+';//dbgp("+");
          else if (row->value < 0.0)
            Buff[bi++]='-';//dbgp("-");
          else
            Buff[bi++]='0';//dbgp("0");
          row = row->right;
          }
        else
          Buff[bi++]=(j%2)==1 ? '.' : ' ';//dbgp((j%2)==1 ? "." : " ");
        j++;
        }
      for (k = j; k <= size; k++)
        Buff[bi++]=(k%2)==1 ? '.' : ' ';//dbgp((k%2)==1 ? "." : " ");

      Buff[bi++]=0;
      dbgp("%s", Buff());
      dbgp(":",b[(i)-1]);
      
      row = rowarray[ (i) - 1].next;
      j=1;
      dword len=0;
      while (row != NULL)
        {
        if (row->col == j)
          {
          if (fabs(row->value)>0.0)
            {
            dbgp(" %#*.*lg",Wide,Prec,row->value);
            len++;
            }
          row = row->right;
          }
        j++;
        }
      while (len++<MaxRowLen)
        dbgp(" %*s",Wide,"");
      dbgp(" : %#*.*lg",Wide,Prec,b[(i)-1]);

      }  
    else  
      {
      while (row != NULL)
        {
        if (row->col == j)
          {
          dbgp("%#*.*lg ",Wide,Prec, row->value);
          row = row->right;
          }
        else
          dbgp("%#*.*lg ",Wide,Prec, 0.0);
        j++;
        }
      for (k = j; k <= size; k++)
        dbgp("%#*.*lg ",Wide,Prec, 0.0);
      }  
    dbgpln("");
    }
  //delete [] buff;
  dbgunlock();
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::colprint()
  {
  /* Colprint prints out the matrix using the column links.*/
  dword i,
  j,
  k;
  node *col;
  printf("\r\n");
  for (i = 1; i <= size; i++)
    {
    printf("\r\n");
    col = colarray[ (i) - 1].next;
    j = 1;
    while (col != NULL)
      {
      if (col->row == j)
        {
        printf("%6.0f", col->value);
        col = col->down;
        }
      else
        printf("     0");
      j++;
      }
    for (k = j; k <= size; k++)
      printf("     0");
    }
  printf("\r\n");
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Mtrx::coltable()
  {
  /* This procedure prints out a table which shows the number of non zeros */
  /* for each column of the matrix as recorded by the matrix structure. It */
  /* then checks if the ordering structure agrees with the number of non-  */
  /* zeros for each column.                                                */
  dword i,
  z;
 
  for (i = 1; i <= size; i++)
    {
    if ((i % 25) == 0)
      {
      char dummy[512];
      gets(dummy);
      }
    z = colarray[ (i) - 1].nonzerocount;
    printf("%5u%5u%5u\n", i, z, SpM_Ordr::colfound(i, z));
    }
  }
  
//#pragma warning (disable : 4073) // initializers put in library initialization area
//#pragma init_seg(lib) // Force Early Construction and Late Destruction
IMPLEMENT_SPARES(node, 40000)
IMPLEMENT_SPARES(head, 10000)
IMPLEMENT_SPARES(SpM_Mtrx, 50)

/*=============================================================================*/
