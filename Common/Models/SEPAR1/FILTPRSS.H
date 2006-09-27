//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FILTPRSS_H
#define  __FILTPRSS_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_XFER_H
  #include "m_xfer.h"
#endif
#ifndef __MTRLOGIC_H
  #include "mtrlogic.h"
#endif

#ifdef __FILTPRSS_CPP
  #define DllImportExport DllExport
#elif !defined(Separ1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

DEFINE_TAGOBJ(FilterPress);
class FilterPress : public MdlNode
  {
  public:
    FilterPress(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~FilterPress();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ChangeMode(dword NewMode, dword OldMode);
    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual CFlange * GetFlange(int IOId);
    virtual void   ConfigureJoins();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void   EvalDerivs(CNodeEvalIndex & NEI);
//    virtual flag   CIStrng(int No, pchar & pS);
    
    void DoEvalProducts();

    virtual dword  ModelStatus();
    DEFINE_CI(FilterPress, MdlNode, 8);

  public:
    byte           iRqdFiltMethod;
    double         RqdFiltSolConc;//, ActFiltSolConcT;
    double         RqdSolidsToFiltrate;//SolidLoss
    double         RqdCakeMoist;
    double         RqdWashEff, ActWashEff;
    double         SolidsToFiltUsed; //fraction of total feed solids reporting to filtrate
    double         ActCakeSolids, ActFiltSolids;
    double         ActCakeSolConc, ActFiltSolConc;
    //byte           iScanEffSpecie; //eg caustic
    //double         ActScandrettEff;//, ActScandrettEff25;
    byte           iWashEffSpecie; //eg caustic
    double         dSpWashEff;
    double         FilterDP;
    flag           fDischarge;
    flag           fFilt2Washings;
    flag           bTrackStatus;

    double         MembDP, LiqVisc, SpCakeRes, FiltArea;

    SpContainer    Cake;
    SpConduit      Fd, ToCake, FromCake;//,Wash;
    SpConduit      Sd;

    //CDirectFlwIO   m_Spill;
    CDirectFlwIO   m_Vent;

  };

//===========================================================================

#undef DllImportExport

#endif
