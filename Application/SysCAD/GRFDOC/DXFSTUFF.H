//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
    
#ifndef  __DXFSTUFF_H
#define  __DXFSTUFF_H

#pragma pack(1)   // Repeat of pack(1) in QGLDEFS.H 

extern "C" 
  {
  #include "dxents.h"
  #include "dxtbls.h"
  #include "grdefs.h"
  #include "c3defs.h"
  #include "c3cdefs.h"
  #include "c3ddefs.h"
  #include "dx3defs.h"
  
//  #include "dxdefs.h"
  #include "qgldefs.h"
  #include "dmldefs.h"
  #include "dxfdefs.h"
  
  #include "grrdefs.h"
  #include "w3ddefs.h"
  #include "wmidefs.h"
  #include "wm3defs.h"
  #include "vp3defs.h"
  #include "vpidefs.h"
  #include "c3vdefs.h"
  #include "c3tdefs.h"                                                        
  #include "c3adefs.h"
  #include "v3ddefs.h"

  #include "dx0defs.h"
  #include "dxetmpls.h"


// new temporay defs by mhm start
#include "lpoint.h"
extern int    size_buffer_entries();
extern int    size_buffer_points();
extern POINT *addr_buffer_points();
extern int   *addr_buffer_entries();
extern short *addr_buffer_type();
extern void   free_buffer_points();
extern void   free_buffer_entries();
extern void   free_buffer_type();
extern void   free_mypolypoly_buffer();
extern int    color_buffer_entries();
extern int    ini_vector_buffer();
extern int    set_buffer_type(int type);
extern int    get_buffer_offset();
extern int    ini_insert_vbflag(int lvl);
extern int    tst_insert_vbflag();
extern int    clr_insert_vbflag();
extern int    fix_and_test_buffer();
extern void   set_color_temperature(REAL t);
// mhm end

  }

typedef INT         LOGICAL[2];

#pragma pack()    // Rest Packing rules to default

#endif
