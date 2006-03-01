//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1996

#ifndef  __CONTCTHT_H
#define  __CONTCTHT_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif

#define BASIC1
#define HEATXCHG1
#include "models.h"

#ifdef __CONTCTHT_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// ==========================================================================

enum  QST_TempSpecs { QST_TRise, QST_FinalT };
enum  QST_CalcMode { QST_FTCActive, QST_Makup, QST_General };

// ==========================================================================

DEFINE_TAGOBJ(ContactHeater);
class DllImportExport ContactHeater : public MN_Surge
  {
  public:
    byte           nAbove, nBelow;
    double         dFinalP;
    double         dSatPOut;
    double         dFinalT;
    double         dFinalTRqd;
    double         dTRiseRqd;
    double         dTRise;
    double         dVentRqd;
    double         SteamRqd;
    double         dPrevPress;
    double         dVapFracRqd;
    double         dQmVapVent;
    byte           iTempSpec;
    byte           iCalcMode;
    flag           bTrackSteamStatus;
    flag           bRemoveExcessVapour;
    flag           bVentConnected;

    CFT_Condenser  m_FTC;

    ContactHeater(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~ContactHeater();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk &VDB);
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(long JoinMask);

    flag           MacroMdlValidNd(int iIONo)               { return True; };
    void           MacroMdlEvaluate(eScdMacroMdlEvals Eval) {};
    CMacroMdlBase*   MacroMdlActivate()                     { return &m_FTC; };
    void           MacroMdlDeactivate()                     {};

    // ConditionBlk Override
    DEFINE_CI(ContactHeater, MN_Surge, 6);
   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
