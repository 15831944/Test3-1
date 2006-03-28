//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SHLTUBE_H
#define  __SHLTUBE_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif  

#define BASIC1
#define HEATXCHG1
#include "models.h"

//#ifndef __M_HXBASE_H
//  #include "m_hxbase.h"
//#endif
//#ifndef __FLSHTRN_H
//  #include "flshtrn.h"
//#endif
//#ifndef __M_BASE_H
//  #include "m_base.h"
//#endif
//#ifndef __VLEBASE_H
//  #include "vlebase.h"
//#endif  

#ifdef __SHLTUBE_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// ==========================================================================
//
//  Shell & Tube Specific Solution 
//
// ==========================================================================

const byte STHX_Inoperative       = 0;
const byte STHX_LiquorLiquor      = 1;
const byte STHX_LiquorGas         = 2;
const byte STHX_GasGas            = 3;
const byte STHX_FullyCondensing   = 4;
const byte STHX_PartialCondensing = 5;

class CHXSdInfo 
  {
  public:
    // Extra info that may be needed;
  };

const byte CA_AllowVent = 0x01;
const byte CA_LimitDuty = 0x02;
const byte CA_LimitNone = 0x04;

class DllImportExport CHXShlTube : public CHXBlock 
  {
  public:
    byte           m_iRqdOpMode;
    byte           m_iActOpMode;

    double         m_dV2LQ, m_dV2LQZeroDuty;
		double				 m_dV2LQVFrac, m_dV2LQLFrac;
    double         m_dQmSteamCond;
    flag           m_bExtraCond;
    
    CHXSdInfo      PriI, SecI;

    CVLEBase        VLE;

    CFT_Condenser  m_FTC;
    
    CHXShlTube(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CHXShlTube();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    void           FindLimitingDuties(byte OpMode, CHXSide& HotSd, CHXSide& ColdSd, CHXSdInfo &HotI, CHXSdInfo &ColdI);
    double         CondenseAll(CHXSide& HotSd, CHXSide& ColdSd, CHXSdInfo &HotI, CHXSdInfo &ColdI, byte flags);// AllowVent, flag LimitDuty);
    
    virtual void   EvalProducts(pFlwNode pNd); 

    void           EvalPressureSens(pFlwNode pNd, SpContainer & Contents, int iJoin, IDMask idMask);
    void           RestoreContents(SpContainer &Contents, SpMArray & M, double H0);

  };

DEFINE_HXB(CHXShlTube)


// ==========================================================================
//
//  Heat Exchange Model
//
// ==========================================================================

DEFINE_TAGOBJ(ShellTube);
class DllImportExport ShellTube : public MdlNode
  {
	public:
    CHXBase        HX;
    CHXShlTube     *pHXST;
    CHXJoinInfo    m_HXJ;
    CFlwEqnPtr      FBP, FBS;
    //double         QmVentRqd;
    SpConduit      LastVent;
      
    SpContainer    Shell;
    SpConduit      QInEst;     //The object which manages the estimate of the input of materials to the surge unit.
    SpConduit      QSigmaOut;  //The object which manages the estimate of the input of materials to the surge unit.
    CHgtOrdInfo    ShellHgtOrd;

    CCnStartCnds   m_ShellStart;

    CSpContPreset  m_Preset;

    ShellTube(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~ShellTube(){};

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
    void           EvalProducts(long JoinMask);
    void           EvalDerivs(long JoinMask);
    void           ConvergeStates(CConvergeStateBlk &CSB);
    virtual flag   EvalPressureSens();
    void           EvalDiscrete();
 
#if DOPBSMOOTHING  
    virtual void   EvalMsAGDatas();
#endif
    void           ClosureInfo();

    flag           MacroMdlValidNd(int iIONo)                 { return True; };
    void           MacroMdlEvaluate(eScdMacroMdlEvals Eval);
    CMacroMdlBase*   MacroMdlActivate()                       { return &pHXST->m_FTC; };
    void           MacroMdlDeactivate()                       {};
    
    virtual        dword ModelStatus();

  private:
  };

// ===========================================================================

#undef DllImportExport


#endif


