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
 
#define extern
#define NOTEXTERN
#include "mtx_ordr.h"
#undef NOTEXTERN
#undef extern

#include "debugnew.h" // must follow all IMPLEMENT_DYNCREATE & IMPLEMENT_SERIAL
 
/*-----------------------------------------------------------------------------*/
 
SpM_Ordr::SpM_Ordr(dword n)
  {
  dword i;
 
  rowplacer = NULL;
  colplacer = NULL;
  freelist = NULL;
  size = n;
  rows        .SetSize(Max(rows        .GetSize(), (int)(n+BUG_EXTRA)));  /* Dynamic Arrays */
  cols        .SetSize(Max(cols        .GetSize(), (int)(n+BUG_EXTRA)));
  rowordering .SetSize(Max(rowordering .GetSize(), (int)(n+BUG_EXTRA)));
  colordering .SetSize(Max(colordering .GetSize(), (int)(n+BUG_EXTRA)));
  freelist    = new SpM_Node;

  /* Never have to check if FreeList is empty */
 
  freelist->next = NULL;
  freelist->prev = NULL;
  for (i = 1; i <= size; i++)
    {
    rows[ (i) - 1] = NULL;
    cols[ (i) - 1] = NULL;
    rowordering[ (i) - 1] = NULL;
    colordering[ (i) - 1] = NULL;
    }
  }
 
/*-----------------------------------------------------------------------------*/
 
void kill_list(SpM_Node **curr)
  {
  if (* curr != NULL)
    {
    //kill_list(&(* curr)->next);
    //delete(* curr);
    //* curr = NULL;
    SpM_Node *Del = *curr,*Next;
    while (Del)
      {
      Next = Del->next;
      //dbgpln("%p del SpM_Node", Del);
      delete Del;
      Del = Next;
      }
    (*curr)=NULL;
    }

  }
 
/*-----------------------------------------------------------------------------*/
 
SpM_Ordr::~SpM_Ordr()
  {
  dword i;
 
  for (i = 1; i <= size; i++)
    {
    kill_list(& rowordering[ (i) - 1]);
    kill_list(& colordering[ (i) - 1]);
    }

  kill_list(& freelist);
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Ordr::Clear(dword n)
  {
  dword i;
 
  for (i = 1; i <= size; i++)
    {
    kill_list(& rowordering[ (i) - 1]);
    kill_list(& colordering[ (i) - 1]);
    }

  kill_list(& freelist);

  // -----------

  rowplacer = NULL;
  colplacer = NULL;
  freelist = NULL;
  size = n;
  rows        .SetSize(Max(rows        .GetSize(), (int)(n+BUG_EXTRA)));  /* Dynamic Arrays */
  cols        .SetSize(Max(cols        .GetSize(), (int)(n+BUG_EXTRA)));
  rowordering .SetSize(Max(rowordering .GetSize(), (int)(n+BUG_EXTRA)));
  colordering .SetSize(Max(colordering .GetSize(), (int)(n+BUG_EXTRA)));
  freelist    = new SpM_Node;
 
  /* Never have to check if FreeList is empty */
 
  freelist->next = NULL;
  freelist->prev = NULL;
  for (i = 1; i <= size; i++)
    {
    rows[ (i) - 1] = NULL;
    cols[ (i) - 1] = NULL;
    rowordering[ (i) - 1] = NULL;
    colordering[ (i) - 1] = NULL;
    }

  }

/*-----------------------------------------------------------------------------*/
 
 
 
void SpM_Ordr::rowinsert(dword rownum,
       dword numnonzeros)
  {
  SpM_Node *temp;
 
  temp = rowordering[ (numnonzeros) - 1];
  if (temp != NULL)
    {                 /* Insert at front of list */
    temp->prev = new SpM_Node;             
    //dbgpln("%p new SpM_Node",temp->prev);
    rowordering[ (numnonzeros) - 1] = temp->prev;
    rowordering[ (numnonzeros) - 1]->value = rownum;
    rowordering[ (numnonzeros) - 1]->prev = NULL;
    rowordering[ (numnonzeros) - 1]->next = temp;
    rows[ (rownum) - 1] = temp->prev;
    }
  else
    {
    /* Insert a into new list */
    temp = new SpM_Node;                   
    //dbgpln("%p new SpM_Node",temp);
    temp->value = rownum;
    temp->next = NULL;
    temp->prev = NULL;
    rowordering[ (numnonzeros) - 1] = temp;
    rows[ (rownum) - 1] = temp;
    }
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Ordr::colinsert(dword colnum,
       dword numnonzeros)
  {
  SpM_Node *temp;
 
  temp = colordering[ (numnonzeros) - 1];
  if (temp != NULL)
    {
    temp->prev = new SpM_Node;             //(pSpM_Node) malloc (sizeof(struct node));
    //dbgpln("%p new SpM_Node",temp->prev);
    colordering[ (numnonzeros) - 1] = temp->prev;
    colordering[ (numnonzeros) - 1]->value = colnum;
    colordering[ (numnonzeros) - 1]->prev = NULL;
    colordering[ (numnonzeros) - 1]->next = temp;
    cols[ (colnum) - 1] = temp->prev;
    }
  else
    {
    temp = new SpM_Node;                   //(pSpM_Node) malloc (sizeof(struct node));
    //dbgpln("%p new SpM_Node",temp);
    temp->value = colnum;
    temp->next = NULL;
    temp->prev = NULL;
    colordering[ (numnonzeros) - 1] = temp;
    cols[ (colnum) - 1] = temp;
    }
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Ordr::getfirstrow(dword * rownum,
       dword numnonzeros,
       flag * exists)
  {
  rowplacer = rowordering[ (numnonzeros) - 1]; /* Get the first row number     */
  * exists = rowplacer != NULL;                /* with 'NumNonZeros' non zeros */
  if (* exists)
    * rownum = rowplacer->value;
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Ordr::getfirstcol(dword * colnum,
       dword numnonzeros,
       flag * exists)
  {
  colplacer = colordering[ (numnonzeros) - 1];
  * exists = colplacer != NULL;
  if (* exists)
    * colnum = colplacer->value;
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Ordr::getnextrow(dword * rownum,
       flag * exists)
  {
  rowplacer = rowplacer->next;   /* Get the next row number after    */
  * exists = rowplacer != NULL;  /* this sequence has been initiated */
  if (* exists)
    * rownum = rowplacer->value; /* by GetFirstRow. The number of    */
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Ordr::getnextcol(dword * colnum,
       flag * exists)
  {
  colplacer = colplacer->next;
  * exists = colplacer != NULL;
  if (* exists)
    * colnum = colplacer->value;
  }
 
/*-----------------------------------------------------------------------------*/
 
 
 
void SpM_Ordr::rowchangeordering(dword rownum,
       dword oldnonzeros,
       dword newnonzeros)
  {
  SpM_Node *temp,
           * temp2;
  SpM_Ordr::rowremove(rownum, oldnonzeros); /* Now the node is at the front of  */
  temp = rowordering[ (newnonzeros) - 1];   /* the FreeList. Use a variation of */
  temp2 = freelist;                         /* row insert proc. */
 
  freelist = freelist->next;
  if (temp != NULL)
    {
    temp->prev = temp2;
    }
  temp2->next = temp;
  temp2->prev = NULL;
  rowordering[ (newnonzeros) - 1] = temp2;
  rows[ (rownum) - 1] = temp2;
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Ordr::colchangeordering(dword colnum,
       dword oldnonzeros,
       dword newnonzeros)
  {
  SpM_Node *temp, * temp2;
  SpM_Ordr::colremove(colnum, oldnonzeros);
  temp = colordering[ (newnonzeros) - 1];
  temp2 = freelist;
  freelist = freelist->next;
  if (temp != NULL)
    {
    temp->prev = temp2;
    }
  temp2->next = temp;
  temp2->prev = NULL;
  colordering[ (newnonzeros) - 1] = temp2;
  cols[ (colnum) - 1] = temp2;
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Ordr::rowremove(dword rownum,
       dword numnonzeros)
  {
  SpM_Node *temp;
  temp = rows[ (rownum) - 1];  /* Constant search time */
  if (temp->prev == NULL)
    {                          /* Remove head of list */
    rowordering[ (numnonzeros) - 1] = temp->next;
    if (temp->next != NULL)
      {
      temp->next->prev = NULL; /* List longer than 1 node */
      }
    }
  else
    {
    temp->prev->next = temp->next;
    if (temp->next != NULL)
      {                                /*.. = NULL => End of list */
      temp->next->prev = temp->prev;   /*Middle of list */
      }
    }
  temp->next = freelist;  /* Place deleted node on the free list */
  freelist = temp;        /* for later deallocation.(saves time) */
 
  rows[ (rownum) - 1] = NULL;
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Ordr::colremove(dword colnum,
       dword numnonzeros)
  {
  SpM_Node *temp;
  temp = cols[ (colnum) - 1];  /* Constant search time */
  if (temp->prev == NULL)    
    {                            /* Remove head of list */
    colordering[ (numnonzeros) - 1] = temp->next;
    if (temp->next != NULL)
      {
      temp->next->prev = NULL;  /* List longer than 1 node */
      }
    }
  else
    {
    temp->prev->next = temp->next;
    if (temp->next != NULL)
      {                               /*.. = NULL => End of list */
      temp->next->prev = temp->prev;  /*Middle of list */
      }
    }
  temp->next = freelist;  /* Place deleted node on the free list */
  freelist = temp;        /* for later deallocation.(saves time) */
 
 
  cols[ (colnum) - 1] = NULL;
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Ordr::rowprint()
  {
  dword i;
  SpM_Node *curr;
  for (i = 1; i <= size; i++)
    {
    if (i == 22)
      return;
    curr = rowordering[ (i) - 1];
    printf("%u  ", i);
    while ((curr != NULL))
      {
      printf("%u  ", curr->value);
      curr = curr->next;
      }
    printf("\r\n");
    }
  for (i = 1; i <= size; i++)
    {
    printf("%u", rows[(i) - 1]->value);
    }
  printf("\r\n");
  }
 
/*-----------------------------------------------------------------------------*/
 
void SpM_Ordr::colprint()
  {
  dword i;
  SpM_Node *curr;
 
  for (i = 1; i <= size; i++)
    {
    if (i == 22)
      return;
    curr = colordering[ (i) - 1];
    printf("%u  ", i);
    while (curr != NULL)
      {
      printf("%u  ", curr->value);
      curr = curr->next;
      }
    printf("\r\n");
    }
  for (i = 1; i <= size; i++)
    {
    printf("%u", cols[(i) - 1]->value);
    }
  printf("\r\n");
  }
 
/*-----------------------------------------------------------------------------*/
/*  Dynamic Storage (Data Structure): 36*N , where Dimention(Matrix) = N x N. */
/*  Static Storage                  : 30 Bytes (globals) + code.              */
/*----------------------------------------------------------------------------*/
 
dword SpM_Ordr::colfound(dword colnum,
        dword nonzeros)
  {
  SpM_Node *curr;
  dword function;
 
  curr = colordering[ (nonzeros) - 1];
  while ((curr != NULL) && (curr->value != colnum))
    curr = curr->next;
  if (curr == NULL)
    function = 0;
  else
    function = 1;
  return (function);
  }

/*=============================================================================*/
 
//#pragma warning (disable : 4073) // initializers put in library initialization area
//#pragma init_seg(lib) // Force Early Construction and Late Destruction
IMPLEMENT_SPARES(SpM_Node, 30000)
IMPLEMENT_SPARES(SpM_Ordr, 30000)

/*=============================================================================*/
 
