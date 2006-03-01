//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* C3DEFSM.H *********************************/
#ifndef C3DEFSM
#define C3DEFSM

#ifdef  SPLINE
#define     C3_CURVE_NURB(C)    ((C)->curve_geom.nurb)
#endif  /* SPLINE */
#define     C3_CURVE_ARC(C)     ((C)->curve_geom.arc)
#define     C3_CURVE_LINE(C)    ((C)->curve_geom.line)
#define     C3_CURVE_PCURVE(C)  ((C)->curve_geom.pcurve)

#endif  

