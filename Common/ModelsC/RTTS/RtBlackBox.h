//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __RTBLACKBOX_H
#define  __RTBLACKBOX_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#include "md_headers.h"

#ifdef __RTBLACKBOX_CPP
  #define DllImportExport DllExport
#elif !defined(RTTS)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_TAGOBJ(RTBlackBox);
class RTBlackBox : public MdlNode
  {
  //friend class Model400Helper;
  public:
    flag           bOnLine;
    MBaseMethod*   mm__pMethod;

    RTBlackBox(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~RTBlackBox();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   PreStartCheck();

    virtual void   EvalProducts(long JoinMask);
    virtual void   EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);
    virtual void   ConfigureJoins();

    virtual dword  ModelStatus();

    void BuildDataDefnForMethod(DataDefnBlk & DDB);


  private:
    DEFINE_CI(RTBlackBox, MdlNode, 4);

  };

// ===========================================================================

#undef DllImportExport

#endif
