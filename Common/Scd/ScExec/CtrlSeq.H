//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

//===========================================================================

// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __CTRLSEQ_H
#define  __CTRLSEQ_H

#ifndef __AFXTEMPL_H__
  #include <afxtempl.h> // "xafxtempl.h"
#endif

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __SCDARRAY_H
  #include "scdarray.h"
#endif
#ifndef __TAGOBJ_H
  #include "tagobj.h"
#endif
#ifndef __DATAATTR_H
  #include "dataattr.h"
#endif
#ifndef __EQNSOLVE_H
  #include "eqnsolve.h"
#endif

#ifdef __CTRLSEQ_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// =========================================================================
//
//
//
// =========================================================================

_FWDDEF(CtrlSequencer);
_FWDDEF(CtrlSeqBlk);

const long  SQN_Start           = 0;
const long  SQN_SettleAndTweak  = 1;
const long  SQN_SettleAndCalc   = 2;
const long  SQN_Control         = 4;

// =========================================================================
//
//
//
// =========================================================================

const byte TR_None    = 0;
const byte TR_P       = 1;
const byte TR_PI      = 2;
const byte TR_PID     = 3;
const byte TR_P20     = 4;
const byte TR_PI20    = 5;
const byte TR_PID20   = 6;


#if MultiGainValues
const int TV_NGainSegs  = 5;
#endif

//=========================================================================
//
//
//
//=========================================================================

class DllImportExport TVC_GainItem
  {
  public:
    double         m_Out;
    double         m_Gain;
    double         m_Ic;
    double         m_Dc;

    double         m_PlantDelay;
    double         m_PlantPeriod;
    double         m_PlantGain;

  };
#if MultiGainValues
typedef CArray <TVC_GainItem, TVC_GainItem&> TVC_GainArray;
#endif

//typedef CList <CtrlSeqBlk*, CtrlSeqBlk*> CtrlSeqBlkList;
//typedef CSCDPtrListIter<CtrlSeqBlkList, pCtrlSeqBlk> CtrlSeqBlkIter;
//typedef CArray <CtrlSeqBlk*, CtrlSeqBlk*> CtrlSeqBlkPtrArray;

//=========================================================================
//
//
//
//=========================================================================

const dword CSW_On        = 0x00000001;
const dword CSW_AutoMan   = 0x00000002;
const dword CSW_Active    = 0x00000004;
const dword CSW_Hold      = 0x00000008;
const dword CSW_TuneBusy1 = 0x00000010;
const dword CSW_TuneBusy2 = 0x00000020;
const dword CSW_TuneRqst  = 0x00000040;

class DllImportExport CtrlSeqBlk
  {
  friend class CtrlSequencer;
  public:

//    static CtrlSeqBlkPtrArray m_CSBs;
//    static CCriticalSection   m_CSB_Sect;

    static flag           fListChgd;
  protected:
    POSITION ListPos;

    // Common
    Strng          m_Tag;
    Strng          m_Sym;
    Strng          m_Name;

    CDArray        m_ErrorA;
    CDArray        m_MeasA;
    CDArray        m_OutA;

    flag           m_CtrlActive;
    double         m_TuneOutLo;
    double         m_TuneOutHi;
    int            m_nGain;

    #if MultiGainValues
    TVC_GainArray  m_EstGain;
    #else
    TVC_GainItem   m_EstGain;
    #endif

    flag           m_InitHold;

    flag           m_Hold;
    flag           m_HoldRqst;
    flag           m_TuneRqst;
    byte           m_TuneBusy;
    long           m_ExecCnt;
    long           m_ExecCntP;

    CDArray        m_ErrHistPrior;
    CDArray        m_ErrHistPost;

    double         m_SlopePrior;

    FmtAttribute   m_InFmt, m_OutFmt;
    CnvAttribute   m_InCnv, m_OutCnv;

  public:
    virtual char * PIDTag()                 { return "P?"; };

    virtual char * MeasTag()                { return "M?"; };
    virtual char * SptTag()                 { return "S?"; };
    virtual char * OutTag()                 { return "O?"; };

    virtual FmtAttribute  & InFmt()         { return m_InFmt; };
    virtual FmtAttribute  & OutFmt()        { return m_OutFmt; };
    //
    virtual CnvAttribute  & InCnv()         { return m_InCnv; };
    virtual CnvAttribute  & OutCnv()        { return m_OutCnv; };


    CtrlSeqBlk();
    virtual ~CtrlSeqBlk();

    // Required Overides
    virtual double &SetPoint()             { return *((double*)NULL);};
    virtual double &Measure()              { return *((double*)NULL);};
    virtual double &Output()               { return *((double*)NULL); };
    virtual double &InMin()                { return *((double*)NULL);};
    virtual double &InMax()                { return *((double*)NULL);};
    virtual double &OutMin()               { return *((double*)NULL);};
    virtual double &OutMax()               { return *((double*)NULL);};
    virtual flag   &TuneRule()             { return *((flag*)NULL); }
    virtual double &TuneOut()              { return *((double*)NULL); }
    virtual flag   &RevActing()            { return *((flag*)NULL); }
    virtual flag   &AutoMan()              { return *((flag*)NULL); }
    virtual flag   &On()                   { return *((flag*)NULL); }

    virtual double &EPSAbs()               { return *((double*)NULL); };
    virtual double &EPSRel()               { return *((double*)NULL); };

    virtual double  OpGain()               { return dNAN; }
    virtual double  OpIc()                 { return dNAN; }
    virtual double  OpDc()                 { return dNAN; }
    virtual void    GetOpGain(double &G, double &I, double &D)  { DoBreak(); }
    virtual void    SetOpGain(double G, double I, double D)  { DoBreak(); }

    virtual double &DeTune()               { return *((double*)NULL); }
    virtual int     TestConverged()        { DoBreak(); return -1; };

    flag            CtrlActive()           { return m_CtrlActive; };
    void            SetCtrlActive(flag On) { m_CtrlActive=On; };
    double          Error()                { return Measure()-SetPoint(); };

    double          InSpan()               { return InMax()-InMin();};
    double          OutSpan()              { return OutMax()-OutMin();};
    double          Ks()                   { return OutSpan()/InSpan();};
    double          SetPointN()            { return (SetPoint()-InMin())/InSpan();};
    //void            SetSetPoint(double S)  { SetPoint()=Range(InMin(), InMin()+S, InMax());};
    void            SetSetPoint(double S)  { SetPoint()=Range(InMin(), S, InMax());};
    void            SetSetPointN(double S) { SetPoint()=Range(InMin(), InMin()+S*InSpan(), InMax());};
    double          MeasureN()             { return (Measure()-InMin())/InSpan();};
    double          ErrorN()               { return MeasureN()-SetPointN(); };
    double          OutputN()              { return (Output()-OutMin())/OutSpan();};
    //void            SetOutput(double O)    { Output()=Range(OutMin(), OutMin()+O, OutMax()); };
    void            SetOutput(double O)    { Output()=Range(OutMin(), O, OutMax()); };
    void            SetOutputN(double O)   { Output()=Range(OutMin(), OutMin()+O*OutSpan(), OutMax()); };

    double          TuneOutLo()            { return m_TuneOutLo; };
    void            SetTuneOutLo(double O) { m_TuneOutLo=O; };
    double          TuneOutHi()            { return m_TuneOutHi; };
    void            SetTuneOutHi(double O) { m_TuneOutHi=O; };

    void            ClearGain();
    flag            GainOK();

    #if MultiGainValues
    //flag   GetGain(double O, double &G, double &I, double &D);
    //int    SetGain(double O, double G, double I, double D, double PG, double PD, double PP);
//  //  int    GetRevActing(int &A, double &P, double &R);
    //double Gain(double O)       { double G,I,D; GetGain(O,P,R); return P; }
    //double IntTime(double O)    { double G,I,D; GetGain(O,G,I,D); return R; }
    #else
    flag            GetEstGain(double &G, double &I, double &D);
    int             SetEstGain(double G, double I, double D, double PG, double PD, double PP);
    int             SetEstGain(double G, double I, double D);
    double          EstGain()              { double G,I,D; GetEstGain(G,I,D); return G; }
    double          EstIc()                { double G,I,D; GetEstGain(G,I,D); return I; }
    double          EstDc()                { double G,I,D; GetEstGain(G,I,D); return D; }
    #endif

    #if MultiGainValues
    flag            GetPlant(double O, double &G, double &D, double &P);
    double          PlantGain(double O)    { double G,D,P; GetPlant(O,G,D,P); return G; };
    double          PlantDelay(double O)   { double G,D,P; GetPlant(O,G,D,P); return D; };
    double          PlantPeriod(double O)  { double G,D,P; GetPlant(O,G,D,P); return P; };
    #else
    flag            GetPlant(double &G, double &D, double &P);
    double          PlantGain()            { double G,D,P; GetPlant(G,D,P); return G; };
    double          PlantDelay()           { double G,D,P; GetPlant(G,D,P); return D; };
    double          PlantPeriod()          { double G,D,P; GetPlant(G,D,P); return P; };
    #endif

    flag            Hold()                 { return m_Hold; };
    void            SetHold(CtrlSequencer * Seq, flag On);
    void            UpdateHold(CtrlSequencer * Seq);
    flag            InitTuneRqd();
    flag            TuneRqd(double Tol);
    flag            TuneRqst()             { return m_TuneRqst; };
    void            SetTuneRqst(flag On)   { m_TuneRqst=On; };
    byte            TuneBusy()             { return m_TuneBusy; };
    void            SetTuneBusy(byte s)    { m_TuneBusy=s; };
    long            ExecCnt()              { return m_ExecCnt; };
    void            SetExecCnt(long C)     { m_ExecCnt=C; };
    long            ExecCntP()             { return m_ExecCntP; };
    void            SetExecCntP(long C)    { m_ExecCntP=C; };

    dword           StatusWord();
    void            SetStatusWord(dword Values, dword Mask);

  };

//=========================================================================
//
//
//
//=========================================================================

class CtrlSeqBlkInfo
  {
  friend class CtrlSeqBlk;
  friend class CtrlSequencer;
  public:

    CtrlSeqBlk *   pBlk;

    double         m_SetPoint;
    double         m_Measure;
    double         m_Output;
    CDArray        m_Alpha;

    double         m_Integral;

    double         m_ErrorChg;
    double         m_ErrorChgPos;
    double         m_ErrorChgNeg;
    double         m_ErrorChgAvg;
    double         m_Error;
    double         m_PrevError;
    int            m_SettleIters;
    int            m_DelayIters;
    double         m_OutAtTune;
    double         m_OutMemory;
    int            m_AlphaLinear;

    CtrlSeqBlkInfo()
      {
      pBlk=NULL;
      m_SetPoint=0.0;
      m_Measure=0.0;
      m_Output=0.0;
      //CDArray        m_Alpha;

      m_Integral=0.0;

      m_ErrorChg=0.0;
      m_ErrorChgPos=0.0;
      m_ErrorChgNeg=0.0;
      m_ErrorChgAvg=0.0;
      m_Error=0.0;
      m_PrevError=0.0;
      m_SettleIters=0;
      m_DelayIters=0;
      m_OutAtTune=0.0;
      m_OutMemory=0.0;
      m_AlphaLinear=0;
      };

    CtrlSeqBlk * operator->() { return pBlk; };
    CtrlSeqBlk * Blk() { return pBlk; };
  };

class DllImportExport CtrlSeqInfoArray : public CArray <CtrlSeqBlkInfo, CtrlSeqBlkInfo&> {};

class DllImportExport CtrlSequencer
  {
  public:

    CtrlSequencer();
    ~CtrlSequencer();


    void           Initialise();//EqnSlvCtrlBlk & EqnCB);
    void           BeginStep();
    void           EndStep();

    static dword   dwExecCount;
    static flag    m_fAllowAutoRetune;

    flag           m_fSequencerActive;
    flag           m_fSequencerWasActive;
    flag           m_fHoldAll;
    flag           m_fTuneWithHold;

    long           m_nSolveVars;
//    CDArray        m_SetPoint;
//    CDArray        m_Measure;
//    CDArray        m_Output;
//    CDMatrx        m_Alpha;
//
//    CDArray        m_Integral;

    double         m_TestOutChg;
    double         m_TestOutput;
//    CDArray        m_ErrorChg;
//    CDArray        m_ErrorChgPos;
//    CDArray        m_ErrorChgNeg;
//    CDArray        m_ErrorChgAvg;
//    CDArray        m_Error;
//    CDArray        m_PrevError;
//    CIArray        m_SettleIters;
//    CIArray        m_DelayIters;
//    CDArray        m_OutAtTune;
//    CDArray        m_OutMemory;
//    CIArray        m_AlphaLinear;

    long           m_iTestErrChgCnt;
    long           m_iSequence;
    long           m_iCount;
    long           m_iVar;

    long           m_MinSettleCount;
    long           m_MaxSettleCount;
    double         m_TestDelta;
    double         m_MaxPBDerate;
    double         m_RetuneOutDelta;
    double         m_SlopePrior;
    double         m_ScaleLimit;

    static dword   ExecCount() { return dwExecCount; };
    static dword   BumpExecCount() { return dwExecCount++; };

    flag           TestSettledPrior(long iVar);
    flag           TestSettledPost(long iVar);
    void           SetOutput(long iVar);
    void           TweakOutput(long iVar);
    void           RestoreOutput(long iVar);
    void           ResetHistory();
    void           CollectHistoryPrior();
    void           CollectHistoryPost();
    int            AnalyseHistory(long iVar, TVC_GainItem & GI);
    void           ClearErrors();
    void           MonitorErrors(long iVar);
    void           CalcAlpha(long iVar);
    void           CalcGains(long iVar);
    void           DumpAlpha();
    long           DoControl();

    flag           SequencerActive() { return m_fSequencerActive; };
    void           SetSequencerActive(flag On) { m_fSequencerActive=On; };
    flag           SequencerTuning() { return m_iSequence != SQN_Control; };

    flag           HoldAll() { return m_fHoldAll; };
    void           SetHoldAll(flag On);// { m_fHoldAll=On; };

    flag           TuneWithHold() { return m_fTuneWithHold; };
    void           SetTuneWithHold(flag On) { m_fTuneWithHold=On; };

    flag           TestConverged(EqnSlvCtrlBlk & TCB);
    int            StepSequencer();

    static CtrlSeqInfoArray & CSBs();
    static int     NCSBs();
    static CtrlSeqBlk & CSBlk(int i);
    static void    LockCSBs();
    static void    UnlockCSBs();

  protected:

  };

// --------------------------------------------------------------------------

inline void  CtrlSeqBlk::SetHold(CtrlSequencer * Seq, flag On)
  {
  m_HoldRqst=On;
  m_Hold=m_HoldRqst || Seq->m_fHoldAll;
  };

// --------------------------------------------------------------------------

inline void  CtrlSeqBlk::UpdateHold(CtrlSequencer * Seq)
  {
  m_Hold=m_HoldRqst || Seq->m_fHoldAll;
  };

// --------------------------------------------------------------------------

extern DllImportExport DDBValueLst DDBTuneRules[];

// --------------------------------------------------------------------------

#undef DllImportExport

#endif
