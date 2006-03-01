//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef  __QCOOLTOW_H
#define  __QCOOLTOW_H

#include "Models.h"

#ifdef __QCOOLTOW_CPP
  #define DllImportExport DllExport
#elif !defined(QALMdls)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================

DEFINE_TAGOBJ(QCoolingTower);
class DllImportExport QCoolingTower : public MN_Surge
  {
  public:
    double    AirWetBulbT ; //
    double    Approach;//
    long      NoCellinLine; //
    double    AvgCellAge; //
    double    TimeSinceDescale;;
    double    EvapFraction;
    double    QmWaterEvap;
     
    QCoolingTower(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~QCoolingTower() {};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   EvalSteadyState();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(long JoinMask);
    virtual void   ConvergeStates(CConvergeStateBlk &CSB);

  private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
