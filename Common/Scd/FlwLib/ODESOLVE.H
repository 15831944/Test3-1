//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __ODESOLVE_H
#define  __ODESOLVE_H

#if defined(__ODESOLVE_CPP)
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#ifndef __FE_CONST_H
  #include "fe_const.h"
#endif

#include "scdtempl.h"

// ===========================================================================

#define LINT_ARGS

// ===========================================================================
//
//
//
// ===========================================================================

enum IE_Enables {IE_Disabled, IE_SaveState, IE_Integrate};

// ===========================================================================

class DllImportExport CStateSave
  {
  DEFINE_MEMSTATS(CStateSave)
  friend class CODESolver;
  friend class CODEDataBlock;
  protected:
    static long    m_Ids;         // Id Numbers
    static long    m_nTotal;      // Count

    long           m_Id;          // the Id 

    double *       m_pV;          // Current value of State
    double         m_Vs;          // value at start of step
    double         m_Vi;          // value at intermediate step
    double         m_VEP;         // value prioor to evalproducts

  public:
    CStateSave();
    virtual ~CStateSave();

    void Attach(double * V);
  };

// ===========================================================================

class DllImportExport CStateInt : public CStateSave
  {
  DEFINE_MEMSTATS(CStateInt)
  friend class CODESolver;
  friend class CODEDataBlock;
  protected:

    static long    m_nTotal;       // 

    byte           m_TolTestOn,
                   m_TolTestRqd,   // Set if tests required
                   m_TolTestFail;
                
    double *       m_pVTst;        // value of State tolerance testing 
    double         m_Vt;           // value temporary

    double *       m_pDV;          // Current derivative(wrt time) of State
    double         m_DVs;          // derivative at start of step
    double         m_DVi;          // derivative at intermediate step
    double         m_DVt;          // derivative temporary
    double         m_DVm;          // derivative at middle of step

    double         m_dFracErr;

  public:
    CStateInt();
    virtual ~CStateInt();

    void           Attach(double * V, double * DV, double * VTst);

    void           SetTestValue(double * VTst) { m_pVTst=VTst; };
  private:
  };

// ===========================================================================

typedef CSmartPtrAllocate<CStateInt> CSPAStateInt;
typedef CSmartPtrAllocate<CStateSave> CSPAStateSave;

class CStateArray 
  {
  protected: 
    CArray <CSPAStateInt, CSPAStateInt&> m_Ints; 
    CArray <CSPAStateSave, CSPAStateSave&> m_Saves; 
  public:
    CStateArray() 
      {
      m_Ints.SetSize(0,16);
      m_Saves.SetSize(0,16);
      };
    ~CStateArray()
      {
      };
    
    void RemoveAll()
      {
      m_Ints.RemoveAll();
      m_Saves.RemoveAll();
      };

    void Add(double * V_, double * dV_, double * V_Tst)
      {
      int i=m_Ints.GetSize();
      m_Ints.SetSize(i+1);
      m_Ints[i]->Attach(V_, dV_, V_Tst);
      };
    void Add(double * V_)
      {
      int i=m_Saves.GetSize();
      m_Saves.SetSize(i+1);
      m_Saves[i]->Attach(V_);
      };

    long NInts() { return m_Ints.GetSize(); };
    long NSaves() { return m_Saves.GetSize(); };

    CStateInt & Int(int i) { return *m_Ints[i]; };
    CStateSave  & Save(int i) { return *m_Saves[i]; };

  };

// ===========================================================================
//
//
//
// ===========================================================================

class CODEDataBlock;

class DllImportExport ODESolverBase
  {
  DEFINE_MEMSTATS(ODESolverBase)
  
  public:

    virtual void   ODEStartStep()=0;
    virtual void   ODEOperate(CODEDataBlock & ODB)=0;
    virtual void   ODEDerivs()=0;
 
  };


// ===========================================================================

typedef enum ODE_StateCmds  
  {
  eStateNull, 
  eStateAdvance, 
  eStateConverge, 
  eStateFixDV,
  eStateRecon, 
  eStateTest, 
  eStateSave, 
  eStateLoad, 
  eStateDiskSave, 
  eStateDiskLoad,
  };
typedef enum ODE_SSC_Type   
  {
  ODE_SSC_Fixed,
  ODE_SSC_Var_1,
  ODE_SSC_Var_2
  };

const int iMethOrder[ODE_LastMethod] = {1,2,4};

// ===========================================================================

class DllImportExport CBadStateList
  {
  public:
    int     Len;
    CStateInt * List[BadListMax];
    
    CBadStateList() {Len=0;};
  };

// ===========================================================================

class CODEDataBlock 
  {
  public:

    typedef long (*CCmdFn)(CODEDataBlock & ODB, CStateArray & States);

    // NBNBNB if members are added that get modified then They must be locked!
    ODE_StateCmds   m_Cmd;
    double          m_TimeInc;
    CCmdFn          m_pFn;

    CODESolver    * m_pSolver;

    CODEDataBlock (CODESolver * pSolver)
      {
      m_Cmd=eStateNull;
      m_TimeInc=-1;
      m_pFn=NULL;
      m_pSolver=pSolver;
      };

    void Set(ODE_StateCmds Cmd, double dTime, CCmdFn pFn, ODESolverBase *pODE=NULL)
      {
      m_Cmd=Cmd;
      m_TimeInc=dTime;
      m_pFn=pFn;
      if (pODE)
        pODE->ODEOperate(*this);
      };

    int     StepSizeControl(); 
    double  Mag_Eps_R();       
    double  Mag_Eps_A();       
    double  ErrMax();          
    void    SetErrMax(double e);
    CBadStateList & BadTolList();
      
    static long SaveStart(CODEDataBlock & ODB, CStateArray & States);
    static long LoadStart(CODEDataBlock & ODB, CStateArray & States);
    static long SaveEP(CODEDataBlock & ODB, CStateArray & States);
    static long LoadEP(CODEDataBlock & ODB, CStateArray & States);
    static long SaveIntermediate(CODEDataBlock & ODB, CStateArray & States);
    static long CalcErrors(CODEDataBlock & ODB, CStateArray & States);
    static long Correct1(CODEDataBlock & ODB, CStateArray & States);
    static long Correct2(CODEDataBlock & ODB, CStateArray & States);

    static long Converge(CODEDataBlock & ODB, CStateArray & States);

    static long EulerAdvance(CODEDataBlock & ODB, CStateArray & States);
    static long EulerFixDV(CODEDataBlock & ODB, CStateArray & States);

    static long RK2AdvanceA(CODEDataBlock & ODB, CStateArray & States);
    static long RK2FixDVA(CODEDataBlock & ODB, CStateArray & States);
    static long RK2AdvanceB(CODEDataBlock & ODB, CStateArray & States);
    static long RK2FixDVB(CODEDataBlock & ODB, CStateArray & States);

    static long RK4AdvanceA(CODEDataBlock & ODB, CStateArray & States);
    static long RK4FixDVA(CODEDataBlock & ODB, CStateArray & States);
    static long RK4AdvanceB(CODEDataBlock & ODB, CStateArray & States);
    static long RK4FixDVB(CODEDataBlock & ODB, CStateArray & States);
    static long RK4AdvanceC(CODEDataBlock & ODB, CStateArray & States);
    static long RK4FixDVC(CODEDataBlock & ODB, CStateArray & States);
    static long RK4AdvanceD(CODEDataBlock & ODB, CStateArray & States);
    static long RK4FixDVD(CODEDataBlock & ODB, CStateArray & States);
    static long RK4AdvanceE(CODEDataBlock & ODB, CStateArray & States);
  };

// ===========================================================================

#define IC_StepStart         0x01
#define IC_StepReStart       0x02
#define IC_Discrete          0x08
#define IC_StepPreStart      0x16

class DllImportExport CTimeStateBlock
  {
  friend class CODESolver;
  DEFINE_MEMSTATS(CTimeStateBlock)

  protected:
    double         m_Time;              // Current Time
    double         m_TimeInc;             // Delta Time Current
    double         m_TimeIncMxRqd;
    double         m_TimeIncMx;   
    double         m_TimeIncMnRqd;
    double         m_TimeIncMn;   
    double         m_TimeIncNext;
    double         m_TimeIncInit;
    double         m_TimeIncRestart;
    double         m_DerivEstInc;
    double         m_SettleTime;
    word           m_Flags;
    pchar          m_Desc;

  public:
    CTimeStateBlock(double Time_, double dTime_);

    void           AdvanceTime(double Advance);

    double         GetTime(void)                    { return m_Time;               };
    double         GetTimeInc(void)                   { return m_TimeInc;              };
    double         GetTimeIncMxRqd(void)              { return m_TimeIncMxRqd;         };
    double         GetTimeIncMx(void)                 { return m_TimeIncMx;            };
    double         GetTimeIncMnRqd(void)              { return m_TimeIncMnRqd;         };
    double         GetTimeIncMn(void)                 { return m_TimeIncMn;            };
    double         GetTimeIncNext(void)              { return m_TimeIncNext;         };
    double         GetTimeIncInit(void)              { return m_TimeIncInit;         };
    double         GetDerivEstInc(void)             { return m_DerivEstInc;        };
    double         GetSettleTime(void)              { return m_SettleTime;         };

    word           GetFlags(void)                   { return m_Flags;                };
    flag           StepStart()                      { return (m_Flags & IC_StepStart)!=0; };
    flag           StepReStart()                    { return (m_Flags & IC_StepReStart)!=0; };
    flag           Discrete()                       { return (m_Flags & IC_Discrete)!=0; };
    flag           StepPreStart()                   { return (m_Flags & IC_StepPreStart)!=0; };

    void           SetTime(double Time);
    void           SetTimeInc(double dTimeRqd);
    void           SetTimeIncMxRqd(double dTimeRqd);
    void           SetTimeIncMnRqd(double dTimeRqd);
    void           SetDerivEstInc(double DerivEstInc);
    void           SetSettleTime(double SettleTime);
  };

// ===========================================================================

class DllImportExport CODESolver
  {
  DEFINE_MEMSTATS(CODESolver)

  friend class CStateInt;
  friend class CStateSave;
  friend class CODEDataBlock;

  protected:
    ODESolverBase *pODEBase;

    byte           m_iMethod;            // Integration Method
    byte           m_iStepSizeControl;   // set if variable step size reqd

    CTimeStateBlock &rIC;

    double         dTimeWrk;          // dTime Local copy which gets halved etc
    double         Time_s;            // Time at start of step
    double         Time_i;            // Time at intermediate step
    double         Time_Stop;         // Required Time To Stop
    double         SafetyScale;       //

    //double         m_Mag_Eps_A;         // Convergence Criterion
    //double         m_Mag_Eps_R;         // Convergence Criterion
    CToleranceBlock  m_MagEps;         // Convergence Criterion

    flag           fStepSizeTooSmall;  //
    long           m_nGoodIters;      // Number of Good Steps
    long           m_nBadIters;       // Number of Bad Steps
    long           m_nIters4Step;     // Number of Iters for last Step
    long           m_nMaxIters;       // Maximum Number Of Steps During One Integration

    double         m_ErrMax;

    double         Grow;
    double         GrowLimit;
    double         OKShrink;
    double         OKShrinkLimit;
    double         BADShrink;

    double         dEstMaxDT;
    flag           fStepTooLarge;
    Strng          sTooLargeTag;

    CBadStateList  m_BadTolList;
    CBadStateList  m_BadTolCopy;
    CBadStateList  m_BadLim;

public:
    CODESolver(CTimeStateBlock *pIC, double dTime_, double dTimeMax_, int MaxIters, ODE_Methods Method_, ODE_SSC_Type StepSizeControl_);
    ~CODESolver();

    byte           Method() { return m_iMethod; };
    int            GetOrder() { return iMethOrder[m_iMethod]; };
    byte           StepSizeControl() { return m_iStepSizeControl; };

    void           SetMethod(byte Method_);
    void           SetStepSizeControl(byte StepSizeControl_);

    void           SetMagConvergence(double Eps_R__, double Eps_A__);

    double         GetMagEPSAbs();
    double         GetMagEPSRel();
    void           SetMagEPSAbs(double Eps);
    void           SetMagEPSRel(double Eps);

    double         GetTime()               { return rIC.GetTime(); };
    void           SetTime(double T)       { rIC.SetTime(T); };
    double         GetTimeInc()              { return rIC.GetTimeInc(); };
    void           SetTimeInc(double T)      { rIC.SetTimeInc(T); };
    double         GetTimeIncMxRqd()         { return rIC.GetTimeIncMxRqd(); };
    void           SetTimeIncMxRqd(double T) { rIC.SetTimeIncMxRqd(T); };
    double         GetTimeIncMnRqd()         { return rIC.GetTimeIncMnRqd(); };
    void           SetTimeIncMnRqd(double T) { rIC.SetTimeIncMnRqd(T); };
    double         GetTimeStepStart()      { return Time_s; };            // Time at start of step

    double         GetDerivEstInc(void)    { return rIC.GetDerivEstInc(); };
    void           SetDerivEstInc(double T) { rIC.SetDerivEstInc(T); };
    double         GetSettleTime(void)     { return rIC.GetSettleTime(); };
    void           SetSettleTime(double T) { rIC.SetSettleTime(T); };

    void           SetSafetyScale(double S)    { SafetyScale=S;};
    double         GetSafetyScale()            { return SafetyScale; };
    void           SetGrowLimit(double L)      { GrowLimit=L;};
    double         GetGrowLimit()              { return GrowLimit; };
    void           SetGrow(double G)           { Grow=G;};
    double         GetGrow()                   { return Grow; };
    void           SetOKShrinkLimit(double L)  { OKShrinkLimit=L;};
    double         GetOKShrinkLimit()          { return OKShrinkLimit; };
    void           SetOKShrink(double S)       { OKShrink=S;};
    double         GetOKShrink()               { return OKShrink; };
    void           SetBADShrink(double S)      { BADShrink=S;};
    double         GetBADShrink()              { return BADShrink; };
    long           GetGoodIters()              { return m_nGoodIters; };
    long           GetBadIters()               { return m_nBadIters; };
    long           GetIters4Step()             { return m_nIters4Step; };
    void           ClearIters4Step()           { m_nIters4Step=0; };

    long           GetNStatesIntegrate()       { return CStateInt::m_nTotal; };
    long           GetNStatesSaveState()       { return CStateSave::m_nTotal-CStateInt::m_nTotal;};

    void           Initialise();
    void           StartUp(ODESolverBase *ODEBase);
    void           Integrate(ODESolverBase *ODEBase, double TimeToStop, double Max_dTime, flag OneIteration, flag HoldAdvance);
//    void           AdvanceDiscrete(ODESolverBase *ODEBase);
    void           DumpIntegrators(FILE * f, double Time_, double dTime_, pchar Desc);
    int            BadTolInfo(int i, Strng &Tag, double &Tol);

    void           SetStepTooLarge(double EstMaxDT, char *Tag);

    void           SaveEvalProducts(ODESolverBase *ODEBase);
    void           RestoreEvalProducts(ODESolverBase *ODEBase);

  protected:
    void           LoadIC(word Flags_, pchar Desc_);

  private:
    void           SaveStart();
    void           SetLimits();
    void           ReStart();
    void           SaveIntermediate();
    flag           CalculateErrors();
    flag           StepSizeOK_1();
    flag           StepSizeOK_2();
    int            IntOneStep();

  };

// ===========================================================================

inline int    CODEDataBlock::StepSizeControl() { return m_pSolver->m_iStepSizeControl; };
inline double CODEDataBlock::Mag_Eps_R()       { return m_pSolver->m_MagEps.GetAbs(); };         
inline double CODEDataBlock::Mag_Eps_A()       { return m_pSolver->m_MagEps.GetRel(); };         
inline double CODEDataBlock::ErrMax()          { return m_pSolver->m_ErrMax; };            
inline void   CODEDataBlock::SetErrMax(double e) { m_pSolver->m_ErrMax=e; };
inline CBadStateList & CODEDataBlock::BadTolList() { return m_pSolver->m_BadTolList; };

// ===========================================================================
// Messy
extern DllImportExport CODESolver * g_pODE;
extern DllImportExport CTimeStateBlock * g_pPBIntCB;
extern DllImportExport CTimeStateBlock * g_pDynIntCB;

extern DllImportExport CODESolver & ODE();
extern DllImportExport CTimeStateBlock & IntCB(); 

inline void   ICAdvanceTime(double Advance)    { IntCB().AdvanceTime(Advance); };
inline double ICGetTime(void)                  { return IntCB().GetTime(); };
inline double ICGetTimeInc(void)                 { return IntCB().GetTimeInc(); };
inline double ICGetTimeIncMxRqd(void)            { return IntCB().GetTimeIncMxRqd(); };
inline double ICGetTimeIncMx(void)               { return IntCB().GetTimeIncMx(); };
inline double ICGetTimeIncMnRqd(void)            { return IntCB().GetTimeIncMnRqd(); };
inline double ICGetTimeIncMn(void)               { return IntCB().GetTimeIncMn(); };
inline double ICGetTimeIncNext(void)            { return IntCB().GetTimeIncNext(); };
inline double ICGetTimeIncInit(void)            { return IntCB().GetTimeIncInit(); };
inline word   ICGetFlags(void)                 { return IntCB().GetFlags(); };
inline double ICGetDerivEstInc(void)           { return IntCB().GetDerivEstInc(); };
inline double ICGetSettleTime(void)            { return IntCB().GetSettleTime(); };
inline flag   ICStepStart()                    { return IntCB().StepStart(); };
inline flag   ICStepReStart()                  { return IntCB().StepReStart(); };
inline flag   ICDiscrete()                     { return IntCB().Discrete(); };
inline flag   ICStepPreStart()                 { return IntCB().StepPreStart(); };
inline void   ICSetTime(double Time)           { IntCB().SetTime(Time); };
inline void   ICSetTimeInc(double dTimeRqd)      { IntCB().SetTimeInc(dTimeRqd); };
inline void   ICSetTimeIncMxRqd(double dTimeRqd) { IntCB().SetTimeIncMxRqd(dTimeRqd); };
inline void   ICSetTimeIncMnRqd(double dTimeRqd) { IntCB().SetTimeIncMnRqd(dTimeRqd); };
inline void   ICSetDerivEstInc(double DerivEstInc) { IntCB().SetDerivEstInc(DerivEstInc); };
inline void   ICSetSettleTime(double SettleTime) { IntCB().SetSettleTime(SettleTime); };

// ===========================================================================

#undef DllImportExport

#endif
