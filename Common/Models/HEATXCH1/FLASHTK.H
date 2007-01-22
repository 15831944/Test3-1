//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FLASHTK_H
#define  __FLASHTK_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#define BASIC1
#define HEATXCHG1
#include "models.h"
//#define BASIC1
//#include "models.h"
//
//#ifndef __FLSHTRN_H
//  #include "flshtrn.h"
//#endif

#ifdef __FLASHTK_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// ==========================================================================

DEFINE_TAGOBJ(FlashTank);
class DllImportExport FlashTank : public MN_Surge
  {
  public:
    Strng          sVapSpModel;
    flag           fOnLine;
    flag           m_bTrackVapFdStatus;

    #if DOPBSMOOTHING  
    flag           m_fSolnVapQmValid;
    double         m_dSolnVapQm;
    #endif

    double         m_dFlashVapFracToLiq;
    double         m_dOtherVapFracToLiq;

    double         m_RmtScl[10];
    int            m_nRmtScl;
    double         dTempIn;
    double         dBPEIn;
    double         dBPE;
    double         dWaterSatT;
    
    CFT_Flash      m_FTF;
    SpConduit      m_QWrk;
    SpConduit      m_QFlash;

    FlashTank(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~FlashTank (){};

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    
    virtual void   ConfigureJoins();
    // virtual flag   InitialiseSolution();
    virtual void   EvalSteadyState();
    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   EvalJoinPressures(long JoinMask);
    //virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    virtual void   EvalPBMakeUpReqd(long JoinMask);
    virtual void   EvalPBMakeUpAvail(long JoinMask);

    virtual void   ClosureInfo();

    virtual flag   MacroMdlValidNd(int iIONo);
    virtual void   MacroMdlEvaluate(eScdMacroMdlEvals Eval);
    virtual CMacroMdlBase* MacroMdlActivate();
    virtual void   MacroMdlDeactivate();

#if DOPBSMOOTHING  
    virtual void   EvalMsAGDatas();
    virtual void   UpdateMsSoln(SparseSolver & SSMS);
#endif

    virtual flag   InitialiseSolution();

    // ConditionBlk Override
    DEFINE_CI(FlashTank, MN_Surge, 4);

   private:
  };

//==========================================================================

#undef DllImportExport

#endif
