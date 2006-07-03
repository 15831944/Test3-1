//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __COOLTWR_H
#define  __COOLTWR_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#define BASIC1
#include "models.h"

/*#ifndef __FLSHTRN_H
  #include "flshtrn.h"
#endif*/


#ifdef __COOLTWR_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// ==========================================================================

DEFINE_TAGOBJ(CoolingTower);
class DllImportExport CoolingTower : public MN_Surge
  {
  public:
    byte           iMethod;
    byte           iMerkelCalcType;
    byte           iLossMethod;
    double         dAirWetBulbT;
    double         dApproachT;
    double         dKaVL;
    double         dLGRatio;
    double         dDriftLossFrac;
    double         dRqdLossFrac;
    double         dRqdLossQm;
    double         dEvapFactor;
    long           iCycles;
    double         dMaxEvapFrac;

    double         dTempKFeed;
    double         dFinalP;
    double         dFinalT;
    double         dTempDrop;
    double         dHeatFlow;
    double         dDuty;
    double         dQmIn;
    double         dAirQmIn;
    double         dEvapFrac;
    double         dQmWaterEvap;
    double         dEvapLossQm;
    double         dLossQm;
    double         dTotalLossQm;
    double         dDriftLossQm;
    double         dBlowdownLossQm;
    double         dAirEnthOut;
    double         dAirCp;
    double         dAirDryBulbT;
    double         dAirTOut;
    double         dAirTRise;
    double         dAirMixQm;
    double         dAirMixCp;
    double         dAirMixT;

    CoolingTower(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CoolingTower ();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    
    virtual flag   InitialiseSolution();
    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   ClosureInfo();

    // ConditionBlk Override
    DEFINE_CI(CoolingTower, MN_Surge, 8);
  private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
