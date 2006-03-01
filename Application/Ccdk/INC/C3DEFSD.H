//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* C3DEFSD.H *********************************/
#ifndef C3DEFSD
#define C3DEFSD
#ifdef  SPLINE
typedef     struct          c3_nurb {
            HPT3            *ctl_pt ;
            REAL            *knot ;
            REAL            offset ;
            PT3             normal ;
            INT             nctpts ;
            INT             order ;
            INT             index ;
            } C3_NURB_S ;
typedef     C3_NURB_S    *C3_NURB ;
#endif  /* SPLINE */

typedef     struct          c3_line {
            PT3             pt[2] ;
            } C3_LINE_S ;
typedef     C3_LINE_S    *C3_LINE ;

typedef     struct          c3_aseg {
            PT3             pt0 ;
            PT3             d_vec ;
            PT3             pt1 ;
            } C3_ASEG_S ;
typedef     C3_ASEG_S    *C3_ASEG ;

typedef     struct          c3_arc {
            C3_ASEG_S       arcseg ;
            BOOLEAN         zero_rad ;
            } C3_ARC_S ;
typedef     C3_ARC_S    *C3_ARC ;

typedef     struct          c3_pc_buffer {
            PT3             *a ;
            INT             n ;
            INT             a_size ;
            } C3_PC_BUFFER_S ;
typedef     C3_PC_BUFFER_S     *C3_PC_BUFFER ;

typedef     struct          c3_pcurve {
            C3_PC_BUFFER_S  buffer ;
            DML_LIST        buffer_list ;
            } C3_PCURVE_S ;
typedef     C3_PCURVE_S    *C3_PCURVE ;
#define     C3_PC_BUFFER_MAXSIZE     100

#endif
