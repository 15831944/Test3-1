//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* C3NMCRS.H *********************************/
#ifndef C3NMCRS
#define C3NMCRS
#ifdef SPLINE
#define     C3_NURB_N(B)        ((B)->nctpts)
#define     C3_NURB_D(B)        ((B)->order)
#define     C3_NURB_CTPT_PTR(B) ((B)->ctl_pt)
#define     C3_NURB_CTPT(B)     ((HPT3*)(C3_NURB_CTPT_PTR(B)))
#define     C3_NURB_KNOT_PTR(B) ((B)->knot)
#define     C3_NURB_KNOT(B)     ((REAL*)(C3_NURB_KNOT_PTR(B)))
#define     C3_NURB_W(B)        ((B)->offset)
#define     C3_NURB_NORMAL(B)   ((B)->normal)
#endif  /* SPLINE */
#endif  /* C#NMCRS */

