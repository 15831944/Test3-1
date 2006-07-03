//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __BOILER_H
#define  __BOILER_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif

#define BASIC1
#define HEATXCHG1
#include "models.h"

#ifdef __BOILER_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// ==========================================================================

DEFINE_TAGOBJ(Boiler);
class DllImportExport Boiler : public MN_Surge
  {
  public:
    double         dFinalTRqd;
    double         dFinalPRqd;
    double         dDrumPress;
    double         dBlowDownFrac;
    double         dEffFrac;
    double         dFinalP;
    double         dFinalT;
    double         dSatTOut;
    double         dHeat1;
    double         dHeat2;
    double         dDrumTemp;
    double         dBlowDownQm;
    double         dBlowDownHf;
    double         dBlowDownHs;
    flag           bOnLine;
    flag           bShowQFeed;
    flag           bTrackSteamStatus;

    CFT_Condenser  FTC;

    Boiler(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Boiler();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk &VDB);
    virtual void   ClosureInfo();
    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   ConfigureJoins();
    virtual void   EvalJoinPressures(long JoinMask);
    //virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    flag           MacroMdlValidNd(int iIONo)               { return True; };
    void           MacroMdlEvaluate(eScdMacroMdlEvals Eval) {};
    CMacroMdlBase*   MacroMdlActivate()                     { return &FTC; };
    void           MacroMdlDeactivate()                     {};

    // ConditionBlk Override
    DEFINE_CI(Boiler, MN_Surge, 4);
   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
