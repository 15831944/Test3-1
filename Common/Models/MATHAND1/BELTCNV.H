//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __BELTCNV_H
#define  __BELTCNV_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_BASE_H
  #include "m_base.h"
#endif
#ifndef __MTRLOGIC_H
  #include "mtrlogic.h"
#endif
#ifndef __SP_QUEUE_H
  #include "sp_queue.h"
#endif

#ifdef __BELTCNV_CPP
  #define DllImportExport DllExport
#elif !defined(MatHand1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================


DEFINE_TAGOBJ(BeltCnv);

class BeltCnv : public MdlNode
  {
  public:
    BeltCnv(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~BeltCnv();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   CollectElectrics(CNodeElectricsArray & TSCA);
    virtual void   ConnectElectrics();

    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);

    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   StartSolution();
    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);

    virtual void   ConfigureJoins();
    virtual void   StartStep();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);

    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    virtual void   EvalPowerRequired();
    virtual void   EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);

    virtual void   EvalDiscrete();
    virtual void   ClosureInfo();
                                                        
    virtual dword  ModelStatus();
    // ConditionBlk Override
    DEFINE_CI(BeltCnv, MdlNode, 4);

  protected:
    typedef CArray < double, double > CWtMtrPosArray;
  public:
    SpQueue        m_Q;
    CWtMtrPosArray &m_WtMtrPos;
    //CDVector       FdPos, PrPos;

    CIO_MtrSpdBlk  m_BeltSB;
    CPwrUser       m_Pwr;
    CDirectFlwIO   m_Spill;
    CDirectFlwIO   m_Vent;
    CDirectFlwIO   m_AccIn;
    CDirectFlwIO   m_AccOut;
    CDirectAccCalc m_AccCalc;

    double         m_MaxVelocity;
    double         m_MaxSpillLoading;
    double         m_MaxStallLoading;
    double         m_MinLoading;
    long           m_MaxTurnDown;
    long           m_ProfPts;
    long           m_ProfDispType;
    flag           m_fShowProfile;
    flag           m_fStartIfStalled;
    flag           m_fWasRunning;
    double         m_NLPower;
    double         m_FLPower;
    double         m_Power;
    //Strng          m_SpillName;
  };

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif
