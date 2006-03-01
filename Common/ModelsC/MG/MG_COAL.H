//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MG_COALES_H
#define  __MG_COALES_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif
#ifndef __FLWNODE_H
  #include <flwnode.h>
#endif
#ifndef __MODELS_H
  #include "models.h"
#endif
//#ifndef __M_XFER_H
//  #include "m_xfer.h"
//#endif
#include "sp_cont.h"

#ifdef __MG_COALES_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// --------------------------------------------------------------------------

DEFINE_TAGOBJ(Coalescer);
class Coalescer : public MN_Xfer
  {
  public:
    CDVector         Cd, Wt;
    SpContainer      BootC;
    SpConduit        QBootIn;
    double           CondW;
    double           WaterC;
    double           BootV;
    double           BootLvl;
    double           Split;
    double           P;
    double           T;
    flag             DoneWarnVap;
    flag             DoneWarnSol;

    Coalescer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Coalescer(){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE);
    virtual void   EvalProducts();
    virtual void   EvalDerivs();
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   ApplyDerivs(double dTime, flag DoDbg);

  private:
  };

// --------------------------------------------------------------------------

#undef DllImportExport

#endif


