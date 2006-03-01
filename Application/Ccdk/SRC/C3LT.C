//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3LT.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3ldefs.h>
#include <c3vdefs.h>
#include <c3tdefs.h>
#include <c3lmcrs.h>
#include <c3vmcrs.h>

/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3l_translate ( line0, shift, line1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_LINE line0 ;
PT3 shift ;
C3_LINE line1 ;
{
    C3V_ADD ( C3_LINE_PT0(line0), shift, C3_LINE_PT0(line1) ) ;
    C3V_ADD ( C3_LINE_PT1(line0), shift, C3_LINE_PT1(line1) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3l_scale ( line0, a, factor, line1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_LINE line0 ;
PT3 a ;
REAL factor ;
C3_LINE line1 ;
{
    C3V_ADDT ( a, C3_LINE_PT0(line0), factor, C3_LINE_PT0(line1) ) ;
    C3V_ADDT ( a, C3_LINE_PT1(line0), factor, C3_LINE_PT1(line1) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3l_mirror ( line0, n, d, line1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_LINE line0 ;
PT3 n ;
REAL d ;
C3_LINE line1 ;
{
    c3v_mirror_pt ( C3_LINE_PT0(line0), n, d, C3_LINE_PT0(line1) ) ;
    c3v_mirror_pt ( C3_LINE_PT1(line0), n, d, C3_LINE_PT1(line1) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3l_rotate_cs ( line0, origin, axis, c, s, line1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_LINE line0 ;
PT3 origin, axis ;
REAL c, s ;
C3_LINE line1 ;
{
    c3v_rotate_pt_cs ( C3_LINE_PT0(line0), origin, axis, 
        c, s, C3_LINE_PT0(line1) ) ;
    c3v_rotate_pt_cs ( C3_LINE_PT1(line0), origin, axis, 
        c, s, C3_LINE_PT1(line1) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3l_project_oblique ( line0, normal, dist, 
            direction, line1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_LINE line0 ;
PT3 normal, direction ;
REAL dist ;
C3_LINE line1 ;
{
    RETURN ( c3v_project_pt_oblique ( C3_LINE_PT0(line0), 
            normal, dist, direction, C3_LINE_PT0(line1) ) &&
            c3v_project_pt_oblique ( C3_LINE_PT1(line0), 
            normal, dist, direction, C3_LINE_PT1(line1) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3l_transform ( line0, t, line1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_LINE line0 ;
C3_TRANSFORM t ;
C3_LINE line1 ;
{
    c3t_eval_pt ( C3_LINE_PT0(line0), t, C3_LINE_PT0(line1) ) ;
    c3t_eval_pt ( C3_LINE_PT1(line0), t, C3_LINE_PT1(line1) ) ;
}

