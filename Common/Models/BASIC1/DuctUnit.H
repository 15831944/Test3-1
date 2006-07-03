//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DUCTUNIT_H
#define  __DUCTUNIT_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_XFER_H
  #include "m_xfer.h"
#endif
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif
#ifndef __GSMODEL_H
  #include "gsmodel.h"
#endif

#ifdef __DUCTUNIT_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1) 
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================

DEFINE_TAGOBJ(DuctUnit);
/*Basic Mixed DuctUnit - Multiple Inputs and Outputs*/
class DllImportExport DuctUnit : public MN_Surge
  {
  public:
    double         FlashSplit;
    double         SS_Lvl;
    byte           nAbove, nBelow;

    double         dRqdTOut, dRqddT;
    double         dRqdTOutMin;
    double         dRqdTOutMax;
    double         dTInLowest;
    double         dTInHighest;

    double         dAmbientTemp;
    double         dHeatXferCoeff;
    double         dHeatLossRate;
    double         dStructMass;
    double         dStructTemp;
    double         dStructCp;
    double         dStructUA;

    double         dRqdH2OVapFrac;

    double         dQmSink;
    double         dQmSinkMeas;
    double         dQmSinkTau;

    DuctUnit(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~DuctUnit (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    //virtual void   StartSolution();
    //virtual void   EvalSteadyState();
    //virtual void   EvalJoinPressures(long JoinMask);
    //virtual void   EvalJoinFlows(int JoinNo);
    virtual void   StartStep();
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual flag   EvalPressureSens();

   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif


