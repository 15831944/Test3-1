//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __M_BSTRES_H
#define  __M_BSTRES_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_BASE_H
  #include "m_base.h"
#endif
#ifndef __VLVLOGIC_H
  #include "vlvlogic.h"
#endif
#ifndef __MTRLOGIC_H
  #include "mtrlogic.h"
#endif
#ifndef __VLEBASE_H
  #include "vlebase.h"
#endif
#include "m_area.h"

#ifdef __M_BSTRES_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
// Basic Flow Object

enum BRPM_BstResPressMethod { BRPM_None, BRPM_Fixed, BRPM_Boost, BRPM_Drop, BRPM_Atmos, BRPM_SatP };

class DllImportExport MN_BstRes : public MdlNode
  {
  public:
    MN_BstRes(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~MN_BstRes(){};
    virtual void   FinaliseConstruction();
    virtual void   ResetData(flag Complete);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn);
    virtual flag   Set_Sizes();

    virtual CFlwEqnPtr * GetSystemDefnFlwEqn(int i, Strng *Tg)
      {
      if (i!=0)
        return NULL;
      if (Tg)
        *Tg="Eqn";
      return &m_FEP;
      };

    //virtual CFlange * GetFlange(int IOId);
    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);
    virtual bool   PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo);
    virtual void   ConfigureJoins();
    virtual void   StartSolution();

    virtual void   EvalJoinPressures(long JoinMask);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void   EvalProductsInit(EvalProductsInitTasks Task);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   ClosureInfo();
    //virtual void   EvalState();

    virtual void   EvalPBMakeUpReqd(long JoinMask) { MakeUpLinkTransferReqd(); };
    virtual void   EvalPBMakeUpAvail(long JoinMask) { MakeUpLinkTransferAvail(); };

    virtual void   EvalDiscrete();
    virtual dword  ModelStatus();

  public:
    CFlwEqnPtr     m_FEP;
    CFlwThermalBlk m_FTB;
    flag           bOnLine;
    flag           bIsentropic; //ProBal
    byte           iPressMethod; //ProBal
    double         dPressBoost_Rqd; //ProBal
    double         dPout_Rqd; //ProBal
    double         dTout_Rqd; //ProBal
    double         dDuty_Rqd; //ProBal
    double         dTout; //ProBal
    double         dPout; //ProBal
    double         dDuty; //ProBal
    //double         dTempDrop;
    CVLEBase       m_VLE;
    CDirectFlwIO   m_LeakI;
    CDirectFlwIO   m_LeakO;

  };

//===========================================================================

#undef DllImportExport

#endif


