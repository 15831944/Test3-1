//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __PIPETERM_H
#define  __PIPETERM_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_BASE_H
  #include "m_base.h"
#endif
#ifndef __VLVLOGIC_H
  #include "vlvlogic.h"
#endif

#ifdef __PIPETERM_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
// Basic Flow Object

const byte  PT_Source    = 0x1;
const byte  PT_Sink      = 0x2;
const byte  PT_Connect   = 0x4;
const byte  PT_ConstP    = 0x8;
const byte  PT_ConstQm   = 0x10;
const byte  PT_ConstQv   = 0x20;
const byte  PT_ConstNQv  = 0x40;

DEFINE_TAGOBJ(CPipeTerm);

class DllImportExport CPipeTerm : public MdlNode
  {
  protected:
    SpConduit      m_Flows;

    double         m_dHead;
    double         m_dPOut;

    CIO_VlvPosBlk  m_VPB;
    CFlwRegBlk     m_FRB;

  public:
    virtual flag   IsLnk() { return false; };

    CPipeTerm(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CPipeTerm();
    virtual void   ResetData(flag Complete);
    virtual void   BuildDataDefn(DataDefnBlk &DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   PreStartCheck();

    virtual flag   GetModelAction(Strng & Tag, MdlActionArray & Acts);
    virtual flag   SetModelAction(Strng & Tag, MdlAction & Act);

    virtual long   NodeFlwTask(NodeFlwTasks Task);

    virtual void   PostConnect(int IONo);

//    virtual pFlwBlk FBlk(int i=0) { return i==0 ? &FB : NULL; };
    virtual void   SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn);
    virtual flag   Set_Sizes();
    virtual void   StartSolution();
    virtual void   StartStep();
    virtual void   ConfigureJoins();
    //virtual flag   MoveTearFlange(int iTearIO, pFlwNode &pNewTearNd, int &iNewTearIO);
    virtual void   EvalJoinPressures(long JoinMask);
//    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int iJoinId, int FE);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);

    virtual CFlwRegBlk * GetFlwRegulator()  { return &m_FRB; };

    virtual void   EvalState();
    virtual void   ClosureInfo();
    virtual void   EvalDiscrete();
    //virtual void   DoEvalProducts(flag Validating, double dTime);
    virtual void   EvalProductsInit(EvalProductsInitTasks Task);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   EvalDerivs(CNodeEvalIndex & NEI);

    //virtual flag   InitialiseSolution();
    //virtual flag   TerminateSolution();
    //
    //virtual void   DoFetch();

    //virtual void   EvalPBMakeUpReqd(long JoinMask);
    //virtual void   EvalPBMakeUpAvail(long JoinMask);
    virtual void   EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);

    //double OutputChg(double RqdOut, double CurOut, double Tau, double dTime)
    //   {
    //   return (RqdOut-CurOut)* (TstGTZ(Tau) ? (1.0-exp(Max(-dTime/Tau, -85.0))) : 1.0);
    //   }

//    virtual double TRequired();
//    virtual double PRequired();
//    virtual double QRequired(double Rho);
//
//    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
//    virtual int    GetConnectionTags(Strng_List & TagLst);
//
//#if DOPBSMOOTHING
//    virtual void   EvalMsAGDatas();
//#endif
    virtual dword  ModelStatus();

    DEFINE_CI(CPipeTerm, MdlNode, 4);

  };

//===========================================================================

#undef DllImportExport

#endif


