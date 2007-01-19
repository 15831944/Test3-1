//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __XFER_H
#define  __XFER_H

#include "sc_defs.h"
#include "m_xfer.h"
#include "gsmodel.h"

#ifdef __XFER_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================

DEFINE_TAGOBJ(X_Tie);
class DllImportExport X_Tie : public MN_Xfer
  {
  public:
    X_Tie(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~X_Tie (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

  protected:
    flag           m_bShowQFeed;
    flag           m_bShowQProd;
  };

//=========================================================================== 

#undef DllImportExport

#endif


