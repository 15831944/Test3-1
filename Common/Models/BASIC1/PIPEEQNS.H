//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __PIPEEQNS_H
#define  __PIPEEQNS_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __FLWEQN_H
  #include "flweqn.h"
#endif

#if defined(__PIPEEQNS_CPP)
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================
//============================================================================

DEFINE_TAGOBJ(FE_Linear);
class FE_Linear : public CFlwEqn
  {
  public:
    double         R_Fxd;

    FE_Linear(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~FE_Linear();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual void   SetPhysData(double DZ);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

//============================================================================
//============================================================================

DEFINE_TAGOBJ(FE_DPperQV);
class FE_DPperQV : public CFlwEqn
  {
  public:
    double         R_Fxd;

    FE_DPperQV(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~FE_DPperQV();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   SetPhysData(double DZ);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(FE_Darcy);
class FE_Darcy : public CFlwEqn
  {
  public:
    byte           m_iFlwRgm;
    double         m_dFricFac;
    double         m_dResMagnFac;
    double         m_dResPowrFac;
    double         m_dResRevMult;
    double         m_dAgeResFac;
    double         m_dRough;
    double         m_dLoLimVel;
    byte           m_iFlwType;
    double         m_dDP;
    double         m_dRe;

#if WithOEP
    double         m_Len_Fit;
    static double  m_dFitLenScl;
#endif

    FE_Darcy(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~FE_Darcy();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   SetPhysData(double DZ);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    double         dP_Vel_Fn(double VVolFrac, double Vel, double QmL, double QmV, double RhoM, double Visc, double Length, CFBPhysData & PhD, CFlwBlkBase &FE, flag &BadDerivs);
  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(FE_Hazen);
class FE_Hazen : public CFlwEqn
  {
  public:
    byte           FlwRgm;
    double         Res_Magn_Fac,
                   Res_Powr_Fac, Age_Res_Fac, LoLimVel;

    FE_Hazen(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~FE_Hazen();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual void   SetPhysData(double DZ);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(FE_Chmbr);
class FE_Chmbr : public CFlwEqn
  {
  public:
    byte           FlwRgm;
    double         Res_Magn_Fac,
                   Res_Powr_Fac, Age_Res_Fac;
    double         LoLimQm;

    FE_Chmbr(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~FE_Chmbr();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual void   SetPhysData(double DZ);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================

// To go back once tested
// DEFINE_TAGOBJ(FE_MR);
class FE_MR : public CFlwEqn
  {
  public:
    byte           FlwRgm;
    double         Kp, K_, np, LoLimVel;

    FE_MR(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~FE_MR();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual void   SetPhysData(double DZ);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================

// To go back once tested
// DEFINE_TAGOBJ(FE_MRCTF);
class FE_MRCTF : public CFlwEqn
  {
  public:
    byte           FlwRgm;
    double         Kp, np, LoLimVel;

    FE_MRCTF(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~FE_MRCTF();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual void   SetPhysData(double DZ);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(FE_MRDM);
class FE_MRDM : public CFlwEqn
  {
  public:
    static double  A_Const, B_Const, N_Const;
    long           MaxIters;
    byte           FlwRgm;
    double         Kp, np, LoLimVel;
    double         Tur_ff_H, Tur_ff_M, Tur_ff_L;

    FE_MRDM(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~FE_MRDM();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    double         DetermineTurbFactor(double Reynolds, double Tur_ff_Guess);
    double         CalcDP(CFBPhysData & PhD, CFlwBlkBase & FE, double Vel, double Rho, double &Tur_ff);
    virtual void   SetPhysData(double DZ);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================

// To go back once tested
// DEFINE_TAGOBJ(FE_MRHR);
class FE_MRHR : public CFlwEqn
  {
  public:
    byte           FlwRgm;
    double         Kp, np, LoLimVel;

    FE_MRHR(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~FE_MRHR();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual void   SetPhysData(double DZ);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================

DEFINE_TAGOBJ(FE_CompAir);
class FE_CompAir : public CFlwEqn
  {
  public:
    double         LoLimVel, EstRf, ManRf;
    FE_CompAir(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~FE_CompAir();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   SetPhysData(double DZ);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================
// ===========================================================================


#undef DllImportExport

#endif
