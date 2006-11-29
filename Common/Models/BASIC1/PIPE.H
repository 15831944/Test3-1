//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __PIPE_H
#define  __PIPE_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_LINK_H
  #include "m_link.h"
#endif

#ifdef __PIPE_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

DEFINE_TAGOBJ(Pipe);

class DllImportExport Pipe : public MN_Lnk
  {
  public:

    Pipe(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Pipe();
    virtual void   BuildDataDefn(DataDefnBlk &DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);

  };

//=========================================================================== 

#undef DllImportExport

#endif


