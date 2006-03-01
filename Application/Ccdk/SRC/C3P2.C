//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3P2.C *********************************/
/******************** Routines for processing pcurves **********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2pdefs.h>
#include <c2qdefs.h>
#include <c3pdefs.h>
#include <c3qdefs.h>
#include <c2pmcrs.h>
#include <c3pmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_PCURVE c3p_conv_3d2d ( pcurve_3d, c, u, v, pcurve_2d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve_3d ;
PT3 c, u, v ;
C2_PCURVE pcurve_2d ;
{
    DML_ITEM item ;
    C2_PC_BUFFER buffer_2d ;
    C3_PC_BUFFER buffer_3d ;


    if ( pcurve_2d == NULL ) {
        buffer_3d = C3_PCURVE_BUFFER(pcurve_3d) ;
        pcurve_2d = c2p_create ( c3q_n ( buffer_3d ), c3q_n ( buffer_3d ) ) ;
        if ( pcurve_2d == NULL ) 
            RETURN ( NULL ) ;
    }
    c3q_conv_3d2d ( C3_PCURVE_BUFFER(pcurve_3d), c, u, v, 
        C2_PCURVE_BUFFER(pcurve_2d) ) ;
    C3_WALK_PCURVE ( pcurve_3d, item ) {
        buffer_2d = c3q_conv_3d2d ( C3_PCURVE_BUFFER(pcurve_3d), 
            c, u, v, NULL ) ;
        if ( buffer_2d != NULL ) {
            if ( C2_PCURVE_BLIST(pcurve_2d) == NULL ) 
                C2_PCURVE_BLIST(pcurve_2d) = dml_create_list () ;
            dml_append_data ( C2_PCURVE_BLIST(pcurve_2d), buffer_2d ) ;
        }
    }
    RETURN ( pcurve_2d ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PCURVE c3p_conv_2d3d ( pcurve_2d, c, u, v, pcurve_3d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PCURVE pcurve_2d ;
PT3 c, u, v ;
C3_PCURVE pcurve_3d ;
{
    DML_ITEM item ;
    C2_PC_BUFFER buffer_2d ;
    C3_PC_BUFFER buffer_3d ;

    if ( pcurve_3d == NULL ) {
        buffer_2d = C2_PCURVE_BUFFER(pcurve_2d) ;
        pcurve_3d = c3p_create ( c2q_n ( buffer_2d ), c2q_n ( buffer_2d ) ) ;
        if ( pcurve_3d == NULL ) 
            RETURN ( NULL ) ;
    }

    c3q_conv_2d3d ( C2_PCURVE_BUFFER(pcurve_2d), c, u, v, 
        C3_PCURVE_BUFFER(pcurve_3d) ) ;
    C2_WALK_PCURVE ( pcurve_2d, item ) {
        buffer_3d = c3q_conv_2d3d ( C2_PCURVE_BUFFER(pcurve_2d), 
            c, u, v, NULL ) ;
        if ( buffer_3d != NULL ) {
            if ( C3_PCURVE_BLIST(pcurve_3d) == NULL ) 
                C3_PCURVE_BLIST(pcurve_3d) = dml_create_list () ;
            dml_append_data ( C3_PCURVE_BLIST(pcurve_3d), buffer_3d ) ;
        }
    }
    RETURN ( pcurve_3d ) ;
}

