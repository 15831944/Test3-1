//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* C3MEM.H ***********************************/
/************ Include file for 3d alloc definitions and macros *************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C3MEM
#define     C3MEM

#ifdef  SPLINE
#define     C3_ALLOC_NURB    ((C3_NURB) MALLOC (1, C3_NURB_S ) )
#define     C3_FREE_NURB(N)  FREE ( (N) )
#endif  /* SPLINE */
#define     C3_ALLOC_LINE    ((C3_LINE) MALLOC (1, C3_LINE_S ) )
#define     C3_FREE_LINE(L)  FREE((L))

#define     C3_ALLOC_ARC     ((C3_ARC) MALLOC (1, C3_ARC_S ) )
#define     C3_FREE_ARC(A)   FREE(A)

#define     C3_ALLOC_CURVE  ((C3_CURVE) MALLOC (1, C3_CURVE_S ) )
#define     C3_FREE_CURVE(C) FREE ( (C) )

/*
#define     C3_ALLOC_INT_REC    ((C3_INT_REC) MALLOC(1, C3_INT_REC_S ))
#define     C3_FREE_INT_REC(L)  FREE((L))
*/
#endif /* C3MEM */


