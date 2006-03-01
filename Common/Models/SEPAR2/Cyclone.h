//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __Cyclone_H
#define  __Cyclone_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#define BASIC1
#define SIZEDST1
#include "models.h"
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif

#ifdef __CYCLONE_CPP
  #define DllImportExport DllExport
#elif !defined(SEPAR2)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

const int MaxCycMeths = 5;

DEFINE_TAGOBJ(Cyclone);
class CycloneMeth
  {
  protected:;
    Cyclone      * m_pCyc;
    double       m_d50;

  public:

    CycloneMeth::CycloneMeth(Cyclone * pC) { m_pCyc=pC; m_d50=0.0; };

    virtual char*  MethName()=0;
    virtual void   BuildDataDefn0(DataDefnBlk & DDB, dword Flags)=0;
    virtual void   BuildDataDefn1(DataDefnBlk & DDB, dword Flags)=0;
    virtual flag   DataXchg(DataChangeBlk & DCB) { return false;};
    virtual flag   ValidateData(ValidateDataBlk & VDB) { return true; };

    virtual void   Init(double Q, SpConduit & Fd) {};
    virtual void   Term(SpConduit & Fd, SpConduit & Os, SpConduit & Us) {};
    virtual int    NPasses() { return 1; };
    virtual flag   Converged(int Pass) { return true; };
    virtual double DP(double Q, SpConduit & Fd)=0;
    virtual double D50()=0;
    virtual double OSizeFrac(double Size, double UFLiq, double OFLiq, int Pass)=0;
    virtual double GetLiquidSplit(int Pass, SpConduit & Fd, double UFSol, double OFSol, double &UFLiq, double &OFLiq)=0;
  };

// ===========================================================================

class Cyclone : public MdlNode
  {
  public:
    byte           iMethod;
    CycloneMeth  * Meth[MaxCycMeths];
    //long           CycNo;
    double         dCycNo;
    double         OFDens, UFDens;
    double         UFSol, UFLiq, OFSol, OFLiq;
    CDArray        OTemp, Feed, UTemp, YTemp;
    SpConduit      QFd, QFine, QTemp;

    SzPartCrv1     PartCrv;

  public:
    Cyclone(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Cyclone();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   ChangeMode(dword NewMode, dword OldMode);
    virtual void   ConfigureJoins();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void   EvalProducts(long JoinMask);
    virtual dword  ModelStatus();

    DEFINE_CI(Cyclone, MdlNode, 4);

  private:
    double         PercSolids(SpConduit &QFd);
    double         SpecGravity(SpConduit &QFd);
    double         Intersection(SpConduit &QUs, SpConduit &QOs);

  };

//===========================================================================

#undef DllImportExport

#endif
