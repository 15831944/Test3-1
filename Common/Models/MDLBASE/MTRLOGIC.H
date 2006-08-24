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
    bool           bOn;
    bool           bStart;
    bool           bStop;
    bool           bStartInv;
    bool           bStopInv;
    bool           bStarted;
    bool           bRunning;
    bool           bTripped;
    bool           bRunMemory;
    double         m_dSpeedReg;
    double         m_dSpeed;
    double         m_dSpeedReqd;
    double         m_dManualSpeed;
    bool           bLoSpeed;
    bool           bHiSpeed;
    float          dLoSpeedLimit;
    float          dHiSpeedLimit;
    bool           m_bAutoRestart;
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
    virtual bool   Running();
    virtual bool   Forward();
    virtual bool   Reverse();
    virtual void   SetDirection(bool Fwd);
    virtual bool   Tripped();
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

    bool            On() { return ((PL_Basic*)pLogic)->bOn!=0; };
    void            SetOn(int On) { ((PL_Basic*)pLogic)->bOn=On!=0; };
    void            SetManualSpeed(double P) { ((PL_Basic*)pLogic)->m_dManualSpeed=P; };
    void            SetSpeed(double P) { ((PL_Basic*)pLogic)->m_dSpeedReqd=P; }; 
    double          ManualSpeed(FlwNode* pFNode) { return ((PL_Basic*)pLogic)->m_dManualSpeed; };
    double          Speed(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL) { return ((PL_Basic*)pLogic)->Speed(pFNode, pFRB) ;};
    void            Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg) { ((PL_Basic*)pLogic)->Ctrl_ConnIDStr(i, ID, Tg); };
                    
    void            Start()               {        ((PL_Basic*)pLogic)->Start(); };
    void            Stop()                {        ((PL_Basic*)pLogic)->Stop(); };
    bool            Running()             { return ((PL_Basic*)pLogic)->Running(); };
    bool            Forward()             { return ((PL_Basic*)pLogic)->Forward(); };
    bool            Reverse()             { return ((PL_Basic*)pLogic)->Reverse(); };
    void            SetDirection(bool Fwd){        ((PL_Basic*)pLogic)->SetDirection(Fwd); };
    bool            Tripped()             { return ((PL_Basic*)pLogic)->Tripped(); };
    void            SetTrip()             {        ((PL_Basic*)pLogic)->SetTrip(); };
    void            ResetTrip()           {        ((PL_Basic*)pLogic)->ResetTrip(); };
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
    PL_FwdRev(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~PL_FwdRev();

    //virtual void   Initialise(pTaggedObject pAttach);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   EvalDiscrete(FlwNode* pFNode) {};
    virtual void   EvalCtrlActions(FlwNode* pNd);
    virtual void   EvalCtrlStrategy(FlwNode* pNd) {};
    virtual double Speed(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL);

    virtual bool   Forward();
    virtual bool   Reverse();
    virtual void   SetDirection(bool Fwd);
  public:
    bool           bFwd;

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
    PL_SoftStart(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~PL_SoftStart();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   EvalDiscrete(FlwNode* pFNode) {};
    virtual void   EvalCtrlActions(FlwNode* pNd);
    virtual void   EvalCtrlStrategy(FlwNode* pNd) {};
    virtual double Speed(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL);

  public:
    double         dStartTime;
    double         dMapLo;
    double         dMapHi;
    double         m_dMapSpeed;
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
    PL_SoftStSp(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~PL_SoftStSp();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   EvalDiscrete(FlwNode* pFNode) {};
    virtual void   EvalCtrlActions(FlwNode* pNd);
    virtual void   EvalCtrlStrategy(FlwNode* pNd) {};
    virtual double Speed(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL);

    virtual bool   Forward();
    virtual bool   Reverse();
    virtual void   SetDirection(bool Fwd);

  public:
    byte           iFwdRev;
    bool           bRunRev;
    double         dStartTime;
    double         dStopTime;
    double         dMapLo;
    double         dMapHi;
    double         m_dMapSpeed;
  };

// ===========================================================================
#undef DllImportExport

#endif