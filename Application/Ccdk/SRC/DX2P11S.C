//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 */
/****************************** DX2P11S.C *******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2ndefs.h>
#include <c2sdefs.h>
#include <dxents.h>
#include <dx2defs.h>
#include <dx2priv.h>
#ifdef __ACAD11__
#include <alsdefs.h>
#include <dmldefs.h>
#include <dx0defs.h>
#include <c2nmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx2_vertex_to_entity ( file, pt, bulge, flags ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
PT2 pt ;
REAL bulge ;
INT flags ;
{ 
    DXF_ENTITY vertex ;

    vertex = dx0_alloc_entity ( DXF_VERTEX, file ) ;
    if ( vertex == NULL ) 
        RETURN ( NULL ) ;
    C2V_COPY ( pt, DXF_VERTEX_PT(vertex) ) ;
    DXF_VERTEX_BULGE(vertex) = bulge ;
    DXF_VERTEX_FLAGS(vertex) = flags ;
    RETURN ( vertex ) ;
}


#ifdef SPLINE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx2_spline_to_entity ( file, spline, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
C2_NURB spline ;
PARM parm0, parm1 ;
{
    DXF_ENTITY pline ;
    INT i, m, n, splinesegs ;
    DXF_ENTITY vertex ;
    REAL *knot, h ;
    PARM_S parm ;
    PT2 pt ; 

    if ( ( parm0 != NULL && c2n_trimmed0 ( spline, parm0 ) ) || 
         ( parm1 != NULL && c2n_trimmed1 ( spline, parm1 ) ) ) {

        C2_NURB tspline = c2n_trim ( spline, parm0, parm1 ) ;
        pline = dx2_spline_to_entity ( file, tspline, NULL, NULL ) ;
        c2n_free_nurb ( tspline ) ;
        RETURN ( pline ) ;
    }

    pline = dx0_alloc_entity ( DXF_POLYLINE, file ) ;
    if ( C2_NURB_D(spline) == 3 ) {
        DXF_PLINE_FLAGS(pline) |= DXF_PLINE_SPL_FIT ;
        DXF_PLINE_TYPE(pline) = DXF_PLINE_QUADR ;
    }
    else if ( C2_NURB_D(spline) == 4 ) {
        DXF_PLINE_FLAGS(pline) |= DXF_PLINE_SPL_FIT ;
        DXF_PLINE_TYPE(pline) = DXF_PLINE_CUBIC ;
    }
    else if ( C2_NURB_D(spline) != 2 ) 
        RETURN ( NULL ) ;
    if ( pline == NULL ) 
        RETURN ( NULL ) ;

    n = C2_NURB_N(spline) ;
    knot = MALLOC ( n + C2_NURB_D(spline), REAL) ;
    als_clsc_uni_knots ( n, C2_NURB_D(spline), knot ) ;

    if ( c2n_closed ( spline ) ) {
        DXF_PLINE_FLAGS(pline) |= DXF_PLINE_CLSD ;
        n-- ;
    }
    DXF_PLINE_VLIST(pline) = dml_create_list();
    if ( DXF_PLINE_VLIST(pline) == NULL )
        RETURN ( NULL ) ;

    vertex = dx2_vertex_to_entity ( file, C2_NURB_CTPT(spline)[0], 0.0, 
        DXF_VERTEX_SPL_FR ) ;
    if ( vertex == NULL ) 
        RETURN ( NULL ) ;
    if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
        RETURN ( NULL ) ;

    splinesegs = dxf_get_splinesegs ( file ) ;
    h = 1.0 / (REAL)splinesegs ;
    m = splinesegs * ( n - C2_NURB_D(spline) + 1 ) ;
    PARM_J(&parm) = 0 ;

    for ( i=0, PARM_T(&parm) = 0.0 ; i <= m ; i++, PARM_T(&parm) += h ) {
        c2s_pt_tan ( C2_NURB_CTPT(spline), n, C2_NURB_D(spline), knot, 0.0, 
            &parm, pt, NULL ) ;
        vertex = dx2_vertex_to_entity ( file, pt, 0.0, DXF_VERTEX_SPL_FIT ) ;
        if ( vertex == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
            RETURN ( NULL ) ;
    }

    for ( i=1 ; i<n ; i++ ) {
        vertex = dx2_vertex_to_entity ( file, C2_NURB_CTPT(spline)[i], 0.0, 
            DXF_VERTEX_SPL_FR ) ;
        if ( vertex == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( DXF_PLINE_VLIST(pline), 
            vertex ) == NULL )
            RETURN ( NULL ) ;
    }
    FREE ( knot ) ;
    RETURN ( pline ) ;
}
#endif /*SPLINE*/
#endif /*__ACAD11__*/

