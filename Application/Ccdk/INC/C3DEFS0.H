//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* C3DEFS0.H *********************************/
/*************** Include file for 3d definitions and macros ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include    <qgldefs.h>
#include    <dmdefs.h>
#include    <fndefs.h>

#define     C3_DISPLAY_BUF_SIZE     50

typedef     REAL            PT3[3];     /* 3d point */
typedef     void            (*PF_PROP3) __(( PT3*, REAL* )) ; 
typedef     void            (*PF_EVAL3) __(( ANY, PT3 )) ; 
                /* pointer to an evaluation function */
typedef     REAL            HPT3[4];    /* 3d homogeneous point */
#ifdef SPLINE
#define C3_TAN_OPTIONS_ENUM { C3_NDEF_TAN, C3_DEF_TAN_MAGN, C3_DEF_TAN } 
#define C3_KNOT_OPTIONS_ENUM { C3_DEFAULT, C3_UNIFORM, C3_CLSC_UNI, \
            C3_NONUNI } 
#ifdef __cplusplus_cnm
enum C3_TAN_OPTIONS_ENUM ;
typedef char C3_TAN_OPTIONS ;
enum C3_KNOT_OPTIONS_ENUM ;
typedef char C3_KNOT_OPTIONS ;
#else
typedef enum C3_TAN_OPTIONS_ENUM C3_TAN_OPTIONS ;
typedef enum C3_KNOT_OPTIONS_ENUM C3_KNOT_OPTIONS ;
#endif 
#endif /* SPLINE */

#define     PT3_X(P)        ((P)[0]) 
#define     PT3_Y(P)        ((P)[1]) 
#define     PT3_Z(P)        ((P)[2]) 
#define     HPT3_X(P)       ((P)[0]/(P)[3]) 
#define     HPT3_Y(P)       ((P)[1]/(P)[3]) 
#define     HPT3_Z(P)       ((P)[2]/(P)[3]) 
#define     HPT3_W(P)       ((P)[3]) 
#define     HPT3_INF(P)     ((P)[3]<=BBS_ZERO) 

typedef     REAL            C3_XFORM[3][4] ;
#define     C3_TRANSFORM    C3_XFORM

#define     C3_EXTR_REC_S   FN_EXTR_REC_S
#define     C3_EXTR_REC     FN_EXTR_REC

/*********** Access macros ********/

#define C3_EXTR_REC_PARM    FN_EXTR_REC_PARM
#define C3_EXTR_REC_T       FN_EXTR_REC_T
#define C3_EXTR_REC_J       FN_EXTR_REC_J
#define C3_EXTR_REC_F       FN_EXTR_REC_F
#define C3_EXTR_REC_TYPE    FN_EXTR_REC_TYPE

typedef     struct          c3_bndbox {
            PT3             minpt ;
            PT3             maxpt ;
            } C3_BOX_S ;
typedef     C3_BOX_S    *C3_BOX ;

#define     C3_MIN_PT(B)    ((B)->minpt)
#define     C3_MAX_PT(B)    ((B)->maxpt)
#define     C3_MIN_X(B)     ((C3_MIN_PT(B))[0])
#define     C3_MIN_Y(B)     ((C3_MIN_PT(B))[1])
#define     C3_MIN_Z(B)     ((C3_MIN_PT(B))[2])
#define     C3_MAX_X(B)     ((C3_MAX_PT(B))[0])
#define     C3_MAX_Y(B)     ((C3_MAX_PT(B))[1])
#define     C3_MAX_Z(B)     ((C3_MAX_PT(B))[2])

#define C3_CURVE_TYPE_ENUM { C3_LINE_TYPE, C3_ARC_TYPE, C3_PCURVE_TYPE, \
        C3_NURB_TYPE, C3_ELLIPSE_TYPE, C3_BEZIER_TYPE, C3_UNKNOWN_TYPE } 
#define C3_PLANE_STATUS_ENUM { C3_PLANE_DET, C3_PLANE_UNDER_DET, \
        C3_PLANE_OVER_DET } 
#ifdef __cplusplus_cnm
enum C3_CURVE_TYPE_ENUM ;
typedef char C3_CURVE_TYPE_DEF ;
enum C3_PLANE_STATUS_ENUM ;
typedef char C3_PLANE_STATUS ;
#else
typedef enum C3_CURVE_TYPE_ENUM C3_CURVE_TYPE_DEF ;
typedef enum C3_PLANE_STATUS_ENUM C3_PLANE_STATUS ;
#endif /*c++*/

#include    <c3defsd.h>
#include    <dxstring.h>


typedef     struct          c3_curve {

    				INT           m_Color;		  /* mhm addition */
    				REAL          m_Thickness;	/* cnm addition */
            DX_STRING     layer;		    /* mhm addition */

            C3_CURVE_TYPE_DEF   curve_type ;
            union {
#ifdef  SPLINE
                C3_NURB         nurb ;
#endif  /* SPLINE */
                C3_LINE         line ;
                C3_ARC          arc ;
                C3_PCURVE       pcurve ;
            } curve_geom ;
            C3_BOX_S        box ;   /* ?, cone - ? */ 
            PARM_S          parm0 ;
            PARM_S          parm1 ;
            } C3_CURVE_S ;
typedef     C3_CURVE_S    *C3_CURVE ;

#define     C3_CURVE_PARM0(C)   (&((C)->parm0))
#define     C3_CURVE_PARM1(C)   (&((C)->parm1))
#define     C3_CURVE_T0(C)      (PARM_T(C3_CURVE_PARM0(C)))
#define     C3_CURVE_J0(C)      (PARM_J(C3_CURVE_PARM0(C)))
#define     C3_CURVE_T1(C)      (PARM_T(C3_CURVE_PARM1(C)))
#define     C3_CURVE_J1(C)      (PARM_J(C3_CURVE_PARM1(C)))

#define     C3_CURVE_TYPE(C)    ((C)->curve_type)
#define     C3_CURVE_IS_LINE(C) (C3_CURVE_TYPE(C)==C3_LINE_TYPE)
#define     C3_CURVE_IS_ARC(C)  (C3_CURVE_TYPE(C)==C3_ARC_TYPE)
#define     C3_CURVE_IS_EXT_ARC(C)   \
        ((BOOLEAN)(C3_CURVE_IS_ARC(C)&&C3_CURVE_T1(C)>=1.0+BBS_ZERO))
#define     C3_CURVE_IS_PCURVE(C) (C3_CURVE_TYPE(C)==C3_PCURVE_TYPE)
#ifdef  SPLINE
#define     C3_CURVE_IS_ELLIPSE(C) (C3_CURVE_TYPE(C)==C3_ELLIPSE_TYPE)
#define     C3_CURVE_IS_SPLINE(C)  (C3_CURVE_TYPE(C)==C3_NURB_TYPE)
#define     C3_CURVE_IS_BEZIER(C)  (C3_CURVE_TYPE(C)==C3_BEZIER_TYPE)
#endif  /* SPLINE */

#define     C3_CURVE_BOX(C)     (&((C)->box))

#define     C3_CURVE_X_MIN(C)   (C3_MIN_X(C3_CURVE_BOX(C)))
#define     C3_CURVE_X_MAX(C)   (C3_MAX_X(C3_CURVE_BOX(C)))
#define     C3_CURVE_Y_MIN(C)   (C3_MIN_Y(C3_CURVE_BOX(C)))
#define     C3_CURVE_Y_MAX(C)   (C3_MAX_Y(C3_CURVE_BOX(C)))
#define     C3_CURVE_Z_MIN(C)   (C3_MIN_Z(C3_CURVE_BOX(C)))
#define     C3_CURVE_Z_MAX(C)   (C3_MAX_Z(C3_CURVE_BOX(C)))

#include <c3defsm.h>

