//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __XFER_H
#define  __XFER_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_XFER_H
  #include "m_xfer.h"
#endif
#ifndef __GSMODEL_H
  #include "gsmodel.h"
#endif

#ifdef __EJECTOR_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================

DEFINE_TAGOBJ(Ejector);
/*#C:
Basic Ejector- See Perry 10-56
*/

class DllImportExport Ejector : public MN_Xfer
  {
  public:

    double dAreaRatio;
    double daKFact;
    double dbKFact;
    double daNzDiam;
    double dbNzDiam;
    double d3NzDiam;
    double dQa;
    double dQb;
    double dWr;
    double dPsr;
    double dPcr;
    double dPSuct;
    //double dPSuctDmp;
    double dPa;
    double dPb;
    double dP3;

//    double dP3Use; // ... TOGO ?????

    Ejector(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Ejector (){};
    virtual void  BuildDataDefn(DataDefnBlk & DDB);
    virtual flag  DataXchg(DataChangeBlk & DCB);
    virtual flag  ValidateData(ValidateDataBlk & VDB);
    virtual void  ConfigureJoins();
    virtual flag  EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void  EvalJoinFlows(int JoinNo);
    virtual void  EvalDiscrete();
    virtual dword ModelStatus();

  private:
  };

//===========================================================================

#undef DllImportExport

#endif


