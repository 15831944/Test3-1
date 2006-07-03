//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DESUPHT_H
#define  __DESUPHT_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif

#define BASIC1
#define HEATXCHG1
#include "models.h"

#ifdef __DESUPHT_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// ==========================================================================

DEFINE_TAGOBJ(DeSuperHeater);
class DllImportExport DeSuperHeater : public MN_Surge
  {
  public:
    double         dFinalTRqd;
    double         dTDropRqd;
    double         dAppTRqd;
    double         dFinalP;
    double         dFinalT;
    double         dSatTOut;
    double         dFlowRqd;
    double         dActualFlow;
    byte           iTempSpec;
    //byte           iCalcMode;
    //flag           bShowQFeed;
    flag           bTrackH2OFeed;

    CFT_Condenser  FTC;

    DeSuperHeater(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~DeSuperHeater();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk &VDB);
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    virtual void   ClosureInfo();

    flag           MacroMdlValidNd(int iIONo)               { return True; };
    void           MacroMdlEvaluate(eScdMacroMdlEvals Eval) {};
    CMacroMdlBase*   MacroMdlActivate()                     { return &FTC; };
    void           MacroMdlDeactivate()                     {};

    // ConditionBlk Override
    DEFINE_CI(DeSuperHeater, MN_Surge, 4);
   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
