//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/***************************** DXSTRING.H **********************************/ 
#include <string.h>
#ifndef     __DX_STRING__
#define     __DX_STRING__
#include    "tststr.h"


//#define pDX_STRING_GET(s)    (s->p)
//#define pDX_STRING_LEN(s)    (strlen(s->p))
//#define pDX_STRING_SIZE(s)   (s->size)
//#define pDX_STRING_VALID(s) (1)

//#define DX_STRING_GET(s)    ((&s)->p)
//#define DX_STRING_LEN(s)    (strlen((&s)->p))
//#define DX_STRING_SIZE(s)   (((&s)->size)
//#define DX_STRING_VALID(s)  (1)


typedef     struct  {
              //void *addr_start;
              char *p;
              int   size;
              //int   indx;
              //void *addr_end;
            } DX_STRING;

#endif

