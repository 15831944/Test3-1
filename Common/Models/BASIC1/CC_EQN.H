//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __CC_EQN_H
#define  __CC_EQN_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif
#ifndef __PUMPEQNS
  #include "pumpeqns.h"
#endif

#if defined(__CC_EQN_CPP)
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif
// ===========================================================================

XID xidPVConst = MdlBsXID(15998);
XID xidGamma   = MdlBsXID(15999);

class CC_EvalProducts
  {
  public:
    CC_EvalProducts (TaggedObject * pThis);
    ~CC_EvalProducts ();
    void        BuildDataDefn(TaggedObject * pThis, DataDefnBlk & DDB);
    flag        DataXchg(TaggedObject * pThis, DataChangeBlk & DCB);
    flag        ValidateData(TaggedObject * pThis, ValidateDataBlk & VDB);
    void        EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    double      ShaftPower() { return m_ShaftPower; };

  public:
    enum        { IsoThermal, Adiabatic, PolyTropic };
    byte        m_Oper;
    //DualDbl     m_PVConst;
    double      m_PVConst;
    DualDbl     m_Gamma;
    double      m_ShaftPower;
    double      m_MechEff;
    //double      m_AdiabaticEff;
  };

// ===========================================================================

DEFINE_TAGOBJ(CC_FixQm);
class CC_FixQm : public CFlwEqn, public CC_EvalProducts
  {
  public:
    CC_FixQm(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~CC_FixQm();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual double ShaftPower() { return CC_EvalProducts::ShaftPower(); };

  public:
    double         m_Reqd_Qm;

  };

// ===========================================================================

DEFINE_TAGOBJ(CC_FixQv);
class DllImportExport CC_FixQv : public CFlwEqn, public CC_EvalProducts
  {
  public:
    CC_FixQv(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~CC_FixQv();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual void   StepFlowInfo(CFlwBlkBase & FE);
    virtual double ShaftPower() { return CC_EvalProducts::ShaftPower(); };

  public:
    flag           m_bAtInlet;
    double         m_dReqd_Qv;
    double         m_dMaxPress;
    double         m_dMaxPSlew;
    double         m_dMaxNdPress;
    double         m_Press;
    double         m_Density;
    double         m_QmRqd;
    double         m_QmRqdPrv;

    enum { eNoDamping, eLclDamping, eGlblDamping };

    byte           m_WhatDamping;
    static double  sm_Damping;
    double         m_Damping;

  };

// ===========================================================================

DEFINE_TAGOBJ(CC_FixBst);
class CC_FixBst : public CFlwEqn, public CC_EvalProducts
  {
  public:
    CC_FixBst(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~CC_FixBst();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual double ShaftPower() { return CC_EvalProducts::ShaftPower(); };

  public:
    double         m_P_Boost;
  };

// ===========================================================================

DEFINE_TAGOBJ(CC_FixBstRes);
class CC_FixBstRes : public CFlwEqn, public CC_EvalProducts
  {
  public:
    CC_FixBstRes(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~CC_FixBstRes();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual double ShaftPower() { return CC_EvalProducts::ShaftPower(); };

  public:
    double         m_P_Boost;
    double         m_P_Res_factor;
    double         m_Inp_Imp;
    double         m_In_Rho;
    double         m_Dsn_VFlw;
    double         m_Dsn_MFlw;
    double         m_Dsn_PRatio;
    double         m_Act_PRatio;
    double         m_PIn;
    double         m_POut;
    double         m_P1;
    double         m_F1;
    double         m_P2;
    double         m_F2;
    double         m_FEst;
  };

// ===========================================================================

DEFINE_TAGOBJ(CC_FixHd);
class CC_FixHd : public CFlwEqn, public CC_EvalProducts
  {
  public:
    CC_FixHd(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~CC_FixHd();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual double ShaftPower() { return CC_EvalProducts::ShaftPower(); };

  public:
    double         m_P_Head;

  };

// ===========================================================================

XID xidGPmpDBSpec = MdlBsXID(16000);

#if WithPCEdt
DEFINE_TAGOBJEDT(CC_DataBase);
#else
DEFINE_TAGOBJ(CC_DataBase);
#endif
class CC_DataBase : public CFlwEqn
  {
  public:
    CC_DataBase(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~CC_DataBase();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   StepFlowInfo(CFlwBlkBase &FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual double ShaftPower() { return dShaftPower; };

  protected:
    virtual double BstAt(double Qv, rSpConduit C);

  public:
    PC_DBase       PDB;
    double         dShaftPower, dEfficiency;
    double         dBstDamping;

    double         Head;
    double         MaxBst, MaxBstQv, OpReg, PrvOpReg;

    double         PressIn; // Pressure @ Inlet;
    double         RhoIn;   // Density @ Inlet;
    double         TempIn;  // Temperature @ Inlet;
    double         GammaIn; // CpCv Ratio @ Inlet;
    double         MoleWtFIn;  // Compressor Intermediate Values @ Inlet

    DDBValueLstMem DDBPumpList;

  };

// ===========================================================================

#if WithPCEdt
_FWDDEF(CC_DataBaseEdt);
class DllImportExport CC_DataBaseEdt : public PC_DataBaseEdt
  {
  public:
    CC_DataBaseEdt(FxdEdtView *pView_, CC_DataBase *pPC_);
    virtual ~CC_DataBaseEdt();
  };
#endif

// ===========================================================================

DEFINE_TAGOBJ(CC_FixQvAGA);
class DllImportExport CC_FixQvAGA : public CFlwEqn
  {
  public:
    CC_FixQvAGA(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~CC_FixQvAGA();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual void   StepFlowInfo(CFlwBlkBase & FE);
    virtual double ShaftPower() { return m_dShaftPower; };

  public:
    flag           m_bAtInlet;
    double         m_dReqd_Qv;
//kcg
    double         m_dOpRatio;
    double         m_dSrg_Qv;
    double         m_dSrgRatio;
    double         m_dCrvDmp;
    double         m_dSrgFlwF;
    double         m_dLclEff;
    double         m_dStCnt;



//kcg
    double         m_dShaftPower;
    double         m_dEfficiency;
    double         m_dAdiabaticEff;
    double         m_dK;
    double         m_dMaxPress;
    double         m_dMaxPSlew;
    double         m_dMaxNdPress;

  };

// ===========================================================================

#undef DllImportExport

#endif
