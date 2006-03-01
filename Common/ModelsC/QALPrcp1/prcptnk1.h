//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __QALPRCP_H
#define  __QALPRCP_H

#define SIZEDST1
#include "Models.h"
#include "flwnode.h"

#ifdef __PRCPTNK1_CPP
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

// ==========================================================================

DEFINE_TAGOBJ(QPrecipMain1);
class DllImportExport QPrecipMain1 : public MN_Surge
  {
  public:
/*    // Params
    double         TankVol;
    double         ActivationEnergy;
    double         K0;
    double         PoisonFactor;
    double         PercM45;

    double         ThermalConst;
    double         AmbientTemp;

    // Results
    double         SSA;
    double         TSA;
    double         Yield;
    double         BoundSoda;
    double         ResidenceTime;
    double         dThermalLoss;
    flag           bWithHOR;
    byte           iMethod;
    double         dReactionHeat;

    // Locals
    double         PrecipFracGuess;*/

    QPrecipMain1(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~QPrecipMain1 () {};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(long JoinMask);

    virtual void   ClosureInfo();

    // ConditionBlk Override
    DEFINE_CI(QPrecipMain1, MN_Surge, 16);
   private:
  };

//===========================================================================

#undef DllImportExport

#endif
