//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __M_XFER_H
#define  __M_XFER_H

#include <sc_defs.h>
#include "m_base.h"
#include "m_area.h"
#include "flshtrn.h"
#include "sp_react.h"

#ifdef __M_XFER_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif
// ==========================================================================
// Basic Flow Object

const int IOId_XferLeak=LeakIOId(0);

class DllImportExport MN_Xfer : public MdlNode
  {
  public:
    MN_Xfer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~MN_Xfer(){};

    virtual flag DataXchg(DataChangeBlk & DCB);
    virtual flag ValidateData(ValidateDataBlk & VDB);

    virtual CFlange * GetFlange(int IOId);
    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);
    virtual Z_States SetDatums(int Pass, int IOIn, double Zi, Z_States Z_State_Src);
    virtual flag   Set_Sizes();
    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual bool   PropagateNetInfo(eScdPropagateNetTasks Task, long IONo, long Info, bool Start);
    virtual void   ConfigureJoins();
    virtual void   StartSolution();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void   EvalProductsInit(EvalProductsInitTasks Task);
    virtual void   EvalProducts(long JoinMask);

    virtual void   EvalDiscrete();

    virtual void   DumpDerivs();
    virtual flag   ChangeMode(dword NewMode, dword OldMode);

    virtual void   EvalPBMakeUpReqd(long JoinMask);
    virtual void   EvalPBMakeUpAvail(long JoinMask);

    virtual flag   MacroMdlValidNd(int iIONo);
    virtual void   MacroMdlEvaluate(eScdMacroMdlEvals Eval);
    virtual CMacroMdlBase* MacroMdlActivate();
    virtual void   MacroMdlDeactivate();
    virtual void   ClosureInfo();

    virtual void   OnAppActivate(BOOL bActive) { m_RB.OnAppActivate(bActive); };
    virtual int    FilesUsed(CFilesUsedArray & Files) { return m_RB.FilesUsed(Files); };
    virtual flag   GetOtherData(FilingControlBlock &FCB) { return m_RB.GetOtherData(FCB); };
    virtual flag   PutOtherData(FilingControlBlock &FCB) { return m_RB.PutOtherData(FCB); };

    virtual dword  ModelStatus();

  public:
    CFT_Mixer      m_FTM;
    CSpCOptionPtr <SpConduit> m_QFeed;
    CSpCOptionPtr <SpConduit> m_QProd;

    CPBPressCtrl   m_PBPCtrl0;
    CReactionBase  m_RB;
    CDirectFlwIO   m_Leak;
  };

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif


