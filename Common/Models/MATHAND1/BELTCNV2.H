//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __BELTCNV2_H
#define  __BELTCNV2_H

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

#ifdef __BELTCNV2_CPP
  #define DllImportExport DllExport
#elif !defined(MDLLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================


//===========================================================================
//
//
//
//===========================================================================


DEFINE_TAGOBJ(BeltCnv2);

class CBeltFdPrDrive
  {
  friend class BeltCnv2;

  public:
    enum { BPC_None, BPC_FwdRev, BPC_Full};

    CBeltFdPrDrive(BeltCnv2 * pBelt);//, bool AsFeed);

    void            Advance(double TimeStep);
    double          Speed(FlwNode*pNd)
      {
      const double SpdFrac = m_IOPtSB.Speed(pNd);
      return m_dMaxSpeed*Range(-1.0, SpdFrac, 1.0);
      }

  protected:
    BeltCnv2      * m_pBelt;
    long            m_lCtrl;
    double          m_dPosnReqd;
    double          m_dPosnActual;
    double          m_dPosnStart;
    double          m_dMaxSpeed;
    double          m_dPosnMin;
    double          m_dPosnMax;
    CIO_MtrSpdBlk   m_IOPtSB;
    CPwrUser        m_Pwr;
  };

class BeltCnv2 : public MdlNode
  {
  friend class CBeltFdPrDrive;
  protected:
    typedef CArray < double, double > CWtMtrPosArray;
  
  public:
    CArray <CBeltFdPrDrive*, CBeltFdPrDrive*> m_FeedDrives; 
    CArray <CBeltFdPrDrive*, CBeltFdPrDrive*> m_ProdDrives; 

    BeltCnv2(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~BeltCnv2();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   CollectElectrics(CNodeElectricsArray & TSCA);
    virtual void   ConnectElectrics();

    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);

    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   StartSolution();
    virtual CFlange * GetFlange(int IOId);
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

    virtual flag GetModelAction(Strng & Tag, MdlActionArray & Acts);
    virtual flag SetModelAction(Strng & Tag, MdlAction & Act);
    
    // ConditionBlk Override
    DEFINE_CI(BeltCnv2, MdlNode, 4);

  protected:
    SpQueue        m_Q;
    CWtMtrPosArray &m_WtMtrPos;
    //CDVector       FdPos, PrPos;

    CIO_MtrSpdBlk  m_BeltSB;
    CPwrUser       m_BeltPwr;
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
    flag           m_TripperOn;
    double         m_NLPower;
    double         m_FLPower;
    double         m_Power;
    //Strng          m_SpillName;
    double         m_TotMassPrev;
    double         m_SpillMassPrev;

  };

//===========================================================================

#undef DllImportExport

#endif
