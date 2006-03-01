//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __HYDROCYC_H
#define  __HYDROCYC_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#define SIZEDST1
#include "models.h"
//#ifndef __M_XFER_H
//  #include "m_xfer.h"
//#endif  
//#ifndef __SQSZDST1_H
//  #include "sqszdst1.h"
//#endif  
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif  
//#ifndef __PL_BASIC_H
//  #include "pl_basic.h"
//#endif  

#ifdef __HYDROCYC_CPP
  #define DllImportExport DllExport
#elif !defined(MDLLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

DEFINE_TAGOBJ(HydroCyclone);
class HydroCyclone : public MdlNode
  {
  public:
    SpConduit      Feed;
    C2DFn          PartCrv;
    double         Ore2Grit, Liq2Grit, GritFlowRatio, ByePass2Grits;

  public:
    HydroCyclone(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~HydroCyclone();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    //virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   ChangeMode(dword NewMode, dword OldMode);
    //virtual flag   InitialiseSolution();
    virtual void   ConfigureJoins();
    virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts();
  private:

  };

//=========================================================================== 

#undef DllImportExport

#endif
