//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __HEATX1_H
#define  __HEATX1_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif
#define BASIC1
#define HEATXCHG1
#include "models.h"

#ifdef __HEATX1_CPP
  #define DllImportExport DllExport
#elif !defined(HeatXch1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
//
//  Shell & Tube Specific Solution
//
// ==========================================================================

const byte HX1_Inoperative       = 0;
const byte HX1_LiquorLiquor      = 1;
const byte HX1_LiquorGas         = 2;
const byte HX1_GasGas            = 3;
const byte HX1_FullyCondensing   = 4;
//const byte HX1_PartialCondensing = 5;


class CStructure
  {
  public:

    double         m_Ci;
    double         m_Cp;
    double         m_dTh, m_dTc;
    double         m_Th, m_Tc;
    double         m_Hi, m_Ho;
    double         m_Duty;
    double         m_Mass;

    double         m_ThF, m_TcF;

    CStructure();
  };

class DllImportExport CHXBlock1 : public CHXBlock
  {
  public:
    byte           m_iRqdOpMode;
    byte           m_iActOpMode;
    CVLEBase        m_VLE;

    double         m_dV2LQ, m_dV2LQZeroDuty, m_dV2LQVFrac, m_dV2LQLFrac;
    CFT_Condenser  m_FTC;

    CStructure     m_Structure;

    SpVector       m_SrgDM;       // rates of change of Mass in surge
    double         m_dSrgDH;      // rate of change of heat in surge
    double         m_dInlineDT;
    bool           m_bInlineIsPri;

    CHXBlock1(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CHXBlock1();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    void           FindLimitingDuties(byte OpMode, CHXSide& HotSd, CHXSide& ColdSd, const double ApproachDT=0.1);
    double         CondenseAll(flag AllowVent, flag MixVentBack, CHXSide& HotSd, CHXSide& ColdSd);
    void           SolveForDutyBrent(CHXSide &HotSd, CHXSide &ColdSd, const double ApproachDT=0.1);

    double         SolveForDutySubsFn(double UA, CHXSide &SdA, CHXSide &SdB, double DutyEst, double DutyB=0.0);
    void           SolveForDutySubs(CHXSide &HotSd, CHXSide &ColdSd, const double ApproachDT=0.1);

    double         SolveForDutySteelFn(double UA, CHXSide &Flow, CStructure &Struct, CHXSide &Other,
                                      double DutyEst, double OtherDuty,
                                      double TSi, double TSo,
                                      double TSif, double TSof,
                                      double &dTSi, double &dTSo);

    double         FindFinalTSFn(CHXSide &HotSd, CHXSide &ColdSd, double TSif, double TSof, double LMTDRqdH, double LMTDRqdC,
                                double *pErrH0=NULL, double *pErrC0=NULL);
    void           FindFinalTS(double UAH, double UAC, CHXSide &HotSd, CStructure &Struct, CHXSide &ColdSd);
    void           SolveForDutySteel(CHXSide &HotSdx, CStructure &Struct, CHXSide &ColdSd);

    void           EvalProductsDynFCSurge(FlwNode* pNd, bool PriIsHot);
    void           EvalProductsDynFCInline(FlwNode* pNd);
    virtual void   EvalProducts(FlwNode* pNd, flag MixVentBack);

    void           EvalPressureSens(FlwNode* pNd, SpContainer & Contents, int iJoin, IDMask idMask);
    void           RestoreContents(SpContainer &Contents, SpMArray & M, double H0);

    flag FlowExists(CHXSide &Flow, CStructure &Struct) { return (Flow.Ci>1.0e-10*Struct.m_Ci); };

  };

DEFINE_HXB(CHXBlock1)


// ==========================================================================
//
//  Heat Exchange Model
//
// ==========================================================================

enum HXSurgeModes { HXSM_NoSurge, HXSM_Tubes, HXSM_Shell };

DEFINE_TAGOBJ(HeatXchg1);
class DllImportExport HeatXchg1 : public MdlNode
  {
  public:
    CHXBlock1      m_HX;
    CHXJoinInfo    m_HXJ;

    flag           m_fUsePriDP, m_fUseSecDP;
    byte           m_iPriSgMd, m_iSecSgMd;
    SpConduit      *m_pPriVent;
    SpConduit      *m_pSecVent;

    SpContainer    *m_pPriSg;
    SpContainer    *m_pSecSg;
    CHgtOrdInfo     m_PriHgtOrd;
    CHgtOrdInfo     m_SecHgtOrd;

    CSpContPreset * m_pPriSgPS;
    CSpContPreset * m_pSecSgPS;

    CCnStartCnds    m_PriStart;
    CCnStartCnds    m_SecStart;

    //flag           m_fDamped;

    long            m_MyPrjFileVerNo;

    HeatXchg1(TagObjClass* pClass_, pchar TagIn, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~HeatXchg1();

    void           SetContProps(int SgMd, SpContainer *pSg, IDMask IOMask);

    void           BuildDataDefn(DataDefnBlk & DDB);
    flag           DataXchg(DataChangeBlk & DCB);
    flag           ValidateData(ValidateDataBlk & VDB);

    flag           InitialiseSolution();
    Z_States       SetDatums(int Pass, int IOIn, double Zi, Z_States Z_State_Src);
    void           SetDatumsDone();
    flag           Set_Sizes();
    void           SetState(eScdMdlStateActs RqdState);
    bool           PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo);
    void           StartSolution();
    void           PostConnect(int IONo);
    void           ConfigureJoins();
    void           StartStep();
    void           EvalJoinPressures(long JoinMask);
    CSpPropInfo*   IOGetNetProps(int i, double Qm);
    flag           EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    void           EvalProductsDyn(bool DoSurge, bool DoXferOut);
    void           EvalProductsSurge(long JoinMask);
    void           EvalProducts(long JoinMask);
    void           EvalDerivs(long JoinMask);
    //void           EvalIntegral(long JoinMask);
    void           ODEOperate(CODEDataBlock & ODB);
    void           ConvergeStates(CConvergeStateBlk &CSB);
    void           EvalDiscrete();

    flag           EvalPressureSens();

    void           ClosureInfo();

    flag           MacroMdlValidNd(int iIONo)               { return True; };
    void           MacroMdlEvaluate(eScdMacroMdlEvals Eval) {};
    CMacroMdlBase*   MacroMdlActivate()                     { return &m_HX.m_FTC; };
    void           MacroMdlDeactivate()                     {};

    dword          ModelStatus();

  private:
  };

// ===========================================================================

#undef DllImportExport


#endif


