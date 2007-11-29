 //================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1996

#ifndef  __FFEVAP_H
#define  __FFEVAP_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif

#define BASIC1
#define HEATXCHG1
#include "models.h"

#ifdef __FFEVAP_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// ==========================================================================
#define WithFFEvap 1

#if WithFFEvap
class DllImportExport CHXSideHelper
  {
  public:
    double dTi;
    double dTo;
    double dPi;
    double dPo;
    double dHi;
    double dHo;
    double dVfi;
    double dVfo;
    double dSatT;
    double dSatP;
    double dQm;
    Strng sName;

    CHXSideHelper(char* pName);
    void BuildDataDefn(TaggedObject* pTagObj, DataDefnBlk & DDB);
  };

const byte FFMeth_TRise = 1;
const byte FFMeth_TBoil = 2;

DEFINE_TAGOBJ(FFEvap);
class DllImportExport FFEvap : public MN_Surge
  {
  public:
    byte           m_iMethod;
    double         m_TBoilRqd;
    double         m_TRiseRqd;
    double         dShellHeatLoss;
    double         dTubeMinPout;
    double         dDuty;
    byte           iCalcMode;
    double         dPrvVapFrac;
    double         dQmEvap;
    double         dQmVapVent;
    flag           bOnLine;
    flag           bAllowEvap;
    flag           bTrackSteamStatus;
    flag           bRemoveExcessVapour;
    flag           bShellVentConnected;

    CHXSideHelper  Shell;
    CHXSideHelper  Tubes;
    CVLEBase       VLE_Tubes;
    //CClosureHfGain m_HfShell;
    //CClosureHfGain m_HfTubes;

    //CFT_Condenser   FTC;

    FFEvap(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~FFEvap();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk &VDB);
    void           ConfigureJoins();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   SetState(eScdMdlStateActs RqdState);
    //virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    virtual void   ClosureInfo();

    //flag           MacroMdlValidNd(int iIONo) { return True; };
    //void           MacroMdlEvaluate(int Option) {};
    //CMacroMdlBase*   MacroMdlActivate() { return &FTC; };
    //void           MacroMdlDeactivate() {};

    DEFINE_CI(FFEvap, MN_Surge, 6);
   private:
  };
#endif

//=========================================================================== 

#undef DllImportExport

#endif
