//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __QALPRCP_H
#define  __QALPRCP_H

#define SIZEDST1
#include "Models.h"
#include "flwnode.h"
#include "precippsd.h"

#ifdef __PRCPTNK2_CPP
  #define DllImportExport DllExport
#elif !defined(QALPRCP1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_TAGOBJ(QPrecipMain2);
class DllImportExport QPrecipMain2 : public MN_Surge
  {
  public:
    CPrecipPSD     PSD;
    flag           bOnLine;
 

    QPrecipMain2(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~QPrecipMain2 () {};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    virtual void   ClosureInfo();

    // ConditionBlk Override
    DEFINE_CI(QPrecipMain2, MN_Surge, 16);
   private:
  };

//===========================================================================

#undef DllImportExport

#endif
