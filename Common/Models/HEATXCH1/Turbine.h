//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1996

#ifndef  __TURBINE_H
#define  __TURBINE_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif

#define BASIC1
#define HEATXCHG1
#include "models.h"

#ifdef __TURBINE_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// ==========================================================================

DEFINE_TAGOBJ(CTurbine);
class DllImportExport CTurbine : public MN_Surge
  {
  public:
    byte           iExhaustMode;
    flag           bOnLine;
    flag           bTrackSteamStatus;
    double         dCalcT;
    double         dCalcVF1;
    double         dCalcVF2;
    double         dEfficiency;
    double         dMechEff;
    double         dPoutRqd;
    double         dSteamTestFrac;
    double         dPIn,dPOut;
    double         dTempIn,dTempOut;
    double         dVapFracIn,dVapFracOut;
    //double         dCpIn,dCpOut;
    double         dSatTOut;
    double         dSatPOut;
    double         dIdealWork;
    double         dIdealPower;
    //double         dHeatFlow;
    double         dQm;
    double         dH20Frac;
    double         dSgIn;
    double         dSlIn;

    //FT_Cond        FTC;

    CTurbine(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CTurbine();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk &VDB);
    //void           ConfigureJoins();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   ClosureInfo();
    virtual void   SetState(eScdMdlStateActs RqdState);
    //virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(long JoinMask);

    //flag           MacroMdlValidNd(int iIONo) { return True; };
    //void           MacroMdlEvaluate(int Option) {};
    //MMVars_Base*   MacroMdlActivate() { return &FTC; };
    //void           MacroMdlDeactivate() {};

    DEFINE_CI(CTurbine, MN_Surge, 4);
   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
