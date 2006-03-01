//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __CONVEYOR_H
#define  __CONVEYOR_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __M_BASE_H
  #include "m_base.h"
#endif  
#ifndef __MTRLOGIC_H
  #include "mtrlogic.h"
#endif  
#ifndef __SP_QUEUE_H
  #include "sp_queue.h"
#endif  
#ifndef __BELTCNV_H
  #include "beltcnv.h"
#endif  

#ifdef __CONVEYOR_CPP
  #define DllImportExport DllExport
#elif !defined(Mathand1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

//typedef CArray < double, double > CWtMtrPosArray;
//
DEFINE_TAGOBJ(Conveyor);

class Conveyor : public BeltCnv
  {
  public:

    Conveyor(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Conveyor();
    virtual dword  ModelStatus() { return BeltCnv::ModelStatus(); };
  
  };

//=========================================================================== 

#undef DllImportExport

#endif
