//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __CTank_H
#define  __CTank_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif

#ifdef __CTank_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 
DEFINE_TAGOBJ(CTank);

//=========================================================================== 

/*Basic Mixed CTank - Multiple Inputs and Outputs*/
class DllImportExport CTank : public MN_Surge
  {
  public:
    CTank(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CTank (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   EvalProducts(long JoinMask);
    virtual void   EvalDerivs(long JoinMask);

  private:
  };

//=========================================================================== 

#undef DllImportExport

#endif


