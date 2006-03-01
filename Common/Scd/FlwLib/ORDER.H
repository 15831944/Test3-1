//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __ORDER_H
#define  __ORDER_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif  

// ===========================================================================

#define MAXNODES 1000
#define MS_NOFIRSTUMCL 1 

//#define TAO_LEE 0
//#define TAO_GRENS 1

enum  tao_Methods {tao_Lee, tao_Grens };

typedef 
struct { 
    int From,
        Toward;
} TearAndOrderIn;//[MAXNODES-(1)+1];
typedef TearAndOrderIn *pTearAndOrderIn;


typedef int TearAndOrderOut;//[MAXNODES-(1)+1];
typedef TearAndOrderOut *pTearAndOrderOut;


extern int  TearAndOrder (        short    method,
                                int nostrms,
                                pTearAndOrderIn instreams,
                                pTearAndOrderOut results     );

// ===========================================================================

#endif

