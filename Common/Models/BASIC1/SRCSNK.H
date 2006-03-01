//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SRCSNK_H
#define  __SRCSNK_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif
#ifndef __M_SRCSNK_H
  #include "m_srcsnk.h"
#endif

#ifdef __SRCSNK_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


//=========================================================================== 

DEFINE_TAGOBJ(Feed_XPG);
/*#C:
Basic Source and/or Sink of Material.
Also provides cross page connector capability.
*/
class DllImportExport Feed_XPG : public MN_SrcSnk
  {
  public:
    //CIO_RegulationBlk    PRegB;
    //CIO_RegulationBlk    TRegB;

    Feed_XPG (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Feed_XPG(){};
    virtual void   PrepareDefinedData();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    
    //virtual void   EvalJoinPressures(long JoinMask);
    //virtual flag   EvalFlowEquations(double QIn, int IONo);
  };



//=========================================================================== 

#undef DllImportExport

#endif


