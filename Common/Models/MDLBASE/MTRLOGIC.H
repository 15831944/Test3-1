//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MTRLOGIC_H
#define  __MTRLOGIC_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __CTRL_IO_H
  #include "ctrl_io.h"
#endif
#ifndef __ELECTRICS_H
  #include "electrics.h"
#endif

#ifdef __MTRLOGIC_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//
//
// ===========================================================================

const byte PLBFail_None    = 0;
const byte PLBFail_NoStart = 0x01;
const byte PLBFail_NoStop  = 0x02;

DEFINE_TAGOBJ(PL_Basic)
class PL_Basic : public CIO_Logic
  {
  public:
    byte           iMode;
    flag           bOn;
    flag           bStart;
    flag           bStop;
    flag           bStartInv;
    flag           bStopInv;
    flag           bStarted;
    flag           bRunning;
    flag           bTripped;
    flag           bRunMemory;
    double         m_dSpeedReg;
    double         m_dSpeed;
    double         m_dSpeedReqd;
    double         m_dManualSpeed;
    flag           bLoSpeed;
    flag           bHiSpeed;
    float          dLoSpeedLimit;
    float          dHiSpeedLimit;
    flag           m_bAutoRestart;
    byte           iFailMode;

    PL_Basic(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~PL_Basic();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   CopyFrom(CIO_Logic * pOther);
    virtual void   EvalDiscrete(FlwNode* pFNode) {};
    virtual flag   DoRunning();
    virtual void   EvalCtrlActions(FlwNode* pNd);
    virtual void   EvalCtrlStrategy(FlwNode* pNd) {};
    virtual double Speed(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL);
    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg);


    virtual void   Start();
    virtual void   Stop();
    virtual flag   Running();
    virtual flag   Forward();
    virtual flag   Reverse();
    virtual void   SetDirection(flag Fwd);
    virtual flag   Tripped();
    virtual void   SetTrip();
    virtual void   ResetTrip();

    DEFINE_CI(PL_Basic, CIO_Logic, 4);

  };

// ---------------------------------------------------------------------------

#define IMPLEMENT_PMPSPDLOGIC(Md, Name, Version, Cat, ShortDesc, Desc)  \
  IMPLEMENT_TAGOBJ(Md, "MtrSpd", Name, Version, "", "", Cat, ShortDesc, Desc);

// ---------------------------------------------------------------------------

_FWDDEF(CIO_MtrSpdBlk)
class DllImportExport CIO_MtrSpdBlk : public CIO_Blk
  {
  public:
    CIO_MtrSpdBlk(pchar Name, pTaggedObject pAttach, CPwrUser * pPwr):
      CIO_Blk("MtrSpd", Name, pAttach, pPwr){ };
    virtual ~CIO_MtrSpdBlk() {};

    flag            On() { return ((PL_Basic*)pLogic)->bOn; };
    void            SetOn(int On) { ((PL_Basic*)pLogic)->bOn=On!=0; };
    void            SetManualSpeed(double P) { ((PL_Basic*)pLogic)->m_dManualSpeed=P; };
    void            SetSpeed(double P) { ((PL_Basic*)pLogic)->m_dSpeedReqd=P; }; 
    double          ManualSpeed(FlwNode* pFNode) { return ((PL_Basic*)pLogic)->m_dManualSpeed; };
    double          Speed(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL) { return ((PL_Basic*)pLogic)->Speed(pFNode, pFRB) ;};
    void            Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg) { ((PL_Basic*)pLogic)->Ctrl_ConnIDStr(i, ID, Tg); };
                    
    void            Start()           {        ((PL_Basic*)pLogic)->Start(); };
    void            Stop()            {        ((PL_Basic*)pLogic)->Stop(); };
    flag            Running()         { return ((PL_Basic*)pLogic)->Running(); };
    flag            Forward()         { return ((PL_Basic*)pLogic)->Forward(); };
    flag            Reverse()         { return ((PL_Basic*)pLogic)->Reverse(); };
    void            SetDirection(flag Fwd) {        ((PL_Basic*)pLogic)->SetDirection(Fwd); };
    flag            Tripped()         { return ((PL_Basic*)pLogic)->Tripped(); };
    void            SetTrip()         {        ((PL_Basic*)pLogic)->SetTrip(); };
    void            ResetTrip()       {        ((PL_Basic*)pLogic)->ResetTrip(); };
  };


// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_TAGOBJ(PL_FwdRev)
class PL_FwdRev : public PL_Basic
  {
  public:
    flag           bFwd;

    PL_FwdRev(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~PL_FwdRev();

    //virtual void   Initialise(pTaggedObject pAttach);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   EvalDiscrete(FlwNode* pFNode) {};
    virtual void   EvalCtrlActions(FlwNode* pNd);
    virtual void   EvalCtrlStrategy(FlwNode* pNd) {};
    virtual double Speed(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL);

    virtual flag   Forward();
    virtual flag   Reverse();
    virtual void   SetDirection(flag Fwd);
  };

// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_TAGOBJ(PL_SoftStart)
class PL_SoftStart : public PL_Basic
  {
  public:
    double         dStartTime;
    double         dMapLo;
    double         dMapHi;
    double         m_dMapSpeed;

    PL_SoftStart(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~PL_SoftStart();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   EvalDiscrete(FlwNode* pFNode) {};
    virtual void   EvalCtrlActions(FlwNode* pNd);
    virtual void   EvalCtrlStrategy(FlwNode* pNd) {};
    virtual double Speed(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL);
  };

// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_TAGOBJ(PL_SoftStSp)
class PL_SoftStSp : public PL_Basic
  {
  public:
    byte           iFwdRev;
    flag           bRunRev;
    double         dStartTime;
    double         dStopTime;
    double         dMapLo;
    double         dMapHi;
    double         m_dMapSpeed;

    PL_SoftStSp(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~PL_SoftStSp();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   EvalDiscrete(FlwNode* pFNode) {};
    virtual void   EvalCtrlActions(FlwNode* pNd);
    virtual void   EvalCtrlStrategy(FlwNode* pNd) {};
    virtual double Speed(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL);

    virtual flag   Forward();
    virtual flag   Reverse();
    virtual void   SetDirection(flag Fwd);
  };

// ===========================================================================
#undef DllImportExport

#endif