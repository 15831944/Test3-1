//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* C3QMCRS.H *********************************/
#ifndef C3QMCRS
#define C3QMCRS

#define     C3_PC_BUFFER_N(A)       ((A)->n)
#define     C3_PC_BUFFER_SIZE(A)    ((A)->a_size)
#define     C3_PC_BUFFER_A(A)       ((A)->a)
#define     C3_PC_BUFFER_ARC(A,I)   ((C3_ASEG)(C3_PC_BUFFER_A(A)+2*(I)))
#define     C3_PC_BUFFER_ARC0(A)    (C3_PC_BUFFER_ARC(A,0))
#define     C3_PC_BUFFER_ARC1(A)    (C3_PC_BUFFER_ARC(A,C3_PC_BUFFER_N(A)-2))
#define     C3_PC_BUFFER_PT0(A)     (C3_PC_BUFFER_A(A)[0])
#define     C3_PC_BUFFER_PT1(A)     (C3_PC_BUFFER_A(A)[2*C3_PC_BUFFER_N(A)-2])
#define     C3_PC_BUFFER_PT(A,I)    (C3_PC_BUFFER_A(A)[2*(I)])
#define     C3_PC_BUFFER_D_VEC(A,I) (C3_PC_BUFFER_A(A)[2*(I)+1])
#endif /* C3QMCRS */

