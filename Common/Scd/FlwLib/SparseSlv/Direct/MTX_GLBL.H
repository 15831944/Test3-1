//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MTX_GLBL_H
#define  __MTX_GLBL_H

#ifndef  __SCDARRAY_H
#include "scdarray.h"
#endif

typedef double *vector;

#define SMTX_NONE                 0
#define SMTX_SINGULAR             1
#define SMTX_INSUFFICIENT_MEMORY  2
#define SMTX_INSERT_ERROR         3
#define SMTX_VECTOR_ERROR         4
#define SMTX_FPP_ERROR            5

typedef unsigned char errorcodes;

struct Mtx_Info
  {  /* Used for stats on Analyse phase */
  dword   nonzerocount, num_fill_ins;
  double sparcity_before, sparcity_after;
  };

#define MU 1.0e-2  /* The threshold parameter for the pivoting strategy */
#define BUG_EXTRA 0 

/// #define dZERO 0.0 

/*============================================================================*/


#endif



