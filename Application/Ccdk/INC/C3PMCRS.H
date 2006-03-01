//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* C3PMCRS.H *********************************/
#ifndef     C3PMCRS
#define     C3PMCRS
#define     C3_PCURVE_BUFFER(A) (&((A)->buffer))
#define     C3_PCURVE_BLIST(A)  ((A)->buffer_list)
#define     C3_WALK_PCURVE(P,IT) \
for ( IT=dml_first(C3_PCURVE_BLIST(P)) ; IT!=NULL ; IT=dml_next(IT) ) 
#endif /* C3PMCRS */
