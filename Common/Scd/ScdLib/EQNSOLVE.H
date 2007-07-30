//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __EQNSOLVE_H
#define  __EQNSOLVE_H

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

#ifdef __EQNSOLVE_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// =========================================================================
//
//
//
// =========================================================================

_FWDDEF(EqnSlvCtrlBlk);
_FWDDEF(TearVarBlk);
_FWDDEF(TearPosBlk);

// =========================================================================
//
//
//
// =========================================================================

const int  MaxTearHistory = 50;

// Tear Type
const byte TT_NoTear        = 0;
const byte TT_SystemTear    = 1;
const byte TT_ManualTear    = 2;
const byte TT_Break         = 3;

// Tear Priority
const byte TP_First         = 0;
const byte TP_Normal        = 1;
const byte TP_Last          = 2;

// Tear Initial Mode
const byte TIW_OnInit       = 1;
const byte TIW_OnStart      = 2;
const byte TIW_Always       = 3;
const byte TIW_Manual       = 4;

const byte TIH_Off          = 0;
const byte TIH_ZeroNIters   = 1;
const byte TIH_HoldNIters   = 2;
const byte TIH_RampNIters   = 3;
const byte TIH_Zeroing      = 4;
const byte TIH_Holding      = 5;
const byte TIH_Ramping      = 6;

const byte TIEU_None        = 0;
const byte TIEU_FullEst     = 1;
const byte TIEU_PartEst     = 2;

const byte TVBTS_Maximum    = 0;
const byte TVBTS_Minimum    = 1;
const byte TVBTS_Priority   = 2;

const byte TVT_None         = 0x00;
const byte TVT_Tear         = 0x01;
const byte TVT_Test         = 0x02;

const byte TVU_Inputs       = 0x01;
const byte TVU_Outputs      = 0x02;
const byte TVU_Meas         = 0x04;
const byte TVU_AdvanceVars  = 0x08;
const byte TVU_TestVars     = 0x10;

//Tag Tear Types...
const byte TTT_Unknown      = 0x00;
const byte TTT_Tear         = 0x01;
const byte TTT_FlashTrain   = 0x02;
const byte TTT_PID          = 0x03;

//=========================================================================
//
//
//
//=========================================================================

class DllImportExport TearVar
  {
  friend class TearVarBlk;
  friend class TearVarBlkEdt;
  friend class EqnSolverBlk;
  public:
    // Common
    Strng          Tag;
    Strng          Sym;
    Strng          Name;

    // Tear advance
    flag           m_bTestIsValid;
    flag           m_bHoldOutput;
    byte           m_iConvergedCnt;
    double         m_DampingRqd;
    double         m_DampFactor;
    double         m_EstDampFactor;

    double         dYRat;
    CDArray        X;
    CDArray        Y;
    FmtAttribute * AdvFmt;
    CnvAttribute * AdvCnv;

    // Convergence
    double         EPS_Abs;
    double         EPS_Rel;
    double         CurTol;
    CDArray        m_Error;
    CDArray        m_Meas;

    FmtAttribute * MeasFmt;
    CnvAttribute * MeasCnv;

    TearVar();

  };

typedef CArray <TearVar, TearVar&> TearVarArray;

//=========================================================================
//
//
//
//=========================================================================

typedef CList <TearPosBlk*, TearPosBlk*> TearPosBlkList;
typedef CSCDPtrListIter<TearPosBlkList, pTearPosBlk> TearPosBlkIter;

class DllImportExport TearPosBlk //: public TaggedObject
  {
  friend class EqnSolverBlk;

  protected:
    POSITION ListPos;
  public:
    byte           m_iRqdTearType,
                   m_iTearType,     // Tear Type
                   m_iTearPriority, // Tear Priority
                   m_iTearInitWhen, // Initialise Time
                   m_iTearInitHow,  // Initialise How to Init (Zero,Hold,Ramp)
                   m_iTearInitEstUsage;  // Initialise Estimate usage
    int            m_iTearHoldRqdCnt,
                   m_iTearHoldCount;

    TearVarBlk    *m_pVarBlk;

  protected:

  public:

    static TearPosBlkList List;

    TearPosBlk(byte DefaultTearType);
    virtual        ~TearPosBlk();

    virtual TearVarBlk* CreateVarBlk(char * Tag, pTaggedObject pAttach)=0;

    void           ConnectTear(flag CreateIfReqd);
    void           DisConnectTear();
    void           ActivateTear();
    void           DeActivateTear();

    virtual void   SetRqdTearType(byte Type)    { m_iRqdTearType=Range(TT_NoTear, Type, TT_Break); };
    byte           RqdTearType()                { return m_iRqdTearType; };
    virtual void   SetTearType(byte Type)       { m_iTearType = Type; };
    byte           TearType()                   { return m_iTearType; };
    flag           IsTear()                     { return m_iTearType != TT_NoTear; };
    flag           IsSystemTear()               { return m_iTearType == TT_SystemTear; };
    byte           TearPriority()               { return m_iTearPriority; };
    virtual void   SetTearPriority(byte iPri)   { m_iTearPriority=iPri; };
    //byte           TearInitMode()               { return m_iTearInitMode; };
    byte           TearInitHow()                { return m_iTearInitHow; };
    byte           TearInitWhen()               { return m_iTearInitWhen; };
    byte           TearInitEstUsage()           { return m_iTearInitEstUsage; };
    bool           TearInitActive();
    virtual void   SetTearInitWhen(byte iMd)    { m_iTearInitWhen = iMd; };
    virtual void   SetTearInitHow(byte iMd)     { m_iTearInitHow = iMd; };
    virtual void   SetTearInitEstUsage(byte iEu) { m_iTearInitEstUsage= iEu; };
    void           SetTearInitActive(bool On, bool SetCount);
    int            TearHoldRqdCnt()             { return m_iTearHoldRqdCnt; };
    virtual void   SetTearHoldRqdCnt(int iCnt)  { m_iTearHoldRqdCnt=Max(1, iCnt); };
    int            TearHoldCount()              { return m_iTearHoldCount; };
    virtual void   SetTearHoldCount(int iCnt)   { m_iTearHoldCount=Max(0, iCnt); };

    void           SetTearInitCounters(bool AtStart);
    void           BumpTearInitCounters();
    double         EstimatePortion();

    virtual byte   TearVarType() { return TVT_None; };
    virtual void   TearInitialiseOutputs(double EstimatePortion) {};
    virtual void   TearInputs2Outputs() {};
    virtual char * TearGetTag(Strng & RootTag) { return NULL; };
    virtual int    TearGetConvergeInfo(TearVarArray & TV) { return 0; };
    virtual void   TearGetErrors(TearVarArray & TV) { };
    virtual void   TearGetInputs(TearVarArray & TV) { };
    virtual void   TearGetOutputs(TearVarArray & TV) { };
    virtual void   TearSetOutputs(TearVarArray & TV) { };
    virtual flag   TearAdvance(TearVarArray & TV, EqnSlvCtrlBlk & EqnCB) { return 0; };

  protected:

  };


//=========================================================================
//
//
//
//=========================================================================

const byte TCM_NULL         = 0;
const byte TCM_DirectSubs   = 1;
const byte TCM_AdaptSubs    = 2;
const byte TCM_Wegstein     = 3;
const byte TCM_Kalman       = 4;
const byte TCM_Last         = 5;
const byte TCM_Default      = TCM_NULL;

const byte DAG_Off          = 0;
const byte DAG_On           = 1;
const byte DAG_Default      = 2;
const byte DAG_Last = 3;

extern DllImportExport char * TearMethodStrings[];
extern DllImportExport DDBValueLst DDBTearMethod[];
extern DllImportExport char * EPSStrategyStrings[];
extern DllImportExport DDBValueLst DDBEPSStrategy[];

inline LPCTSTR TearConvergeMethodStr(int i)
  {
  return (i>=0 && i<TCM_Last) ? TearMethodStrings[i] : "";
  };
inline long FindTearConvergeMethod(LPCTSTR Str)
  {
  for (int i=0; i<TCM_Last; i++)
    {
    if (_stricmp(Str, TearMethodStrings[i])==0)
      {
      return i;
      }
    }
  return -1;
  };

extern DllImportExport char * DampAsGroupStrings[];
extern DllImportExport char * DampAsGroupStringsShort[];
extern DllImportExport DDBValueLst DDBDampAsGroup[];
extern DllImportExport DDBValueLst DDBDampAsGroupDef[];

inline LPCTSTR DampAsGroupStr(int i)
  {
  return (i>=0 && i<TCM_Last) ? DampAsGroupStrings[i] : "";
  };
inline LPCTSTR DampAsGroupStrShort(int i)
  {
  return (i>=0 && i<TCM_Last) ? DampAsGroupStringsShort[i] : "";
  };
inline long FindDampAsGroup(LPCTSTR Str)
  {
  for (int i=0; i<DAG_Last; i++)
    {
    if (_stricmp(Str, DampAsGroupStrings[i])==0)
      {
      return i;
      }
    }
  for (int i=0; i<DAG_Last; i++)
    {
    if (_stricmp(Str, DampAsGroupStringsShort[i])==0)
      {
      return i;
      }
    }
  return -1;
  };

//=========================================================================
//
//
//
//=========================================================================

#define KeepOldTearVarBlkEdit 1

#if KeepOldTearVarBlkEdit
DEFINE_TAGOBJEDT(TearVarBlk);
#else
DEFINE_TAGOBJ(TearVarBlk);
#endif

typedef CList <TearVarBlk*, TearVarBlk*> TearVarBlkList;
typedef CSCDPtrListIter<TearVarBlkList, TearVarBlk*> TearVarBlkIter;

class DllImportExport TearVarBlk : public TaggedObject
  {
#if KeepOldTearVarBlkEdit
  friend class TearVarBlkEdt;
#endif
  friend class EqnSolverBlk;
  friend class TearObjectEdt;

  protected:
    POSITION ListPos;
  public:

  protected:
    byte           bUsedByBlk;

    byte           m_iTearMethod;
    flag           fInUse;
    byte           m_iEPSStrategy;
    byte           m_iDampAsGroup;
    flag           fGroupBlk;

    TearPosBlk    *pPosBlk;
    long           nVariables;
    long           nHistory;
    double         m_dDampingRqd;
    double         dEPS_Abs;
    double         dEPS_Rel;
    flag           m_bHoldDamping;

    byte           iTearTagTyp;
    Strng          sGroupTag;
    TearVarBlk *   pGroupInfo;

    double         GrpDampFactor;
    long           m_lStageCnt;
    Strng          RootTag;

    TearVarArray   TV;

    long           m_lSeqConvergedCnt;
    long           m_lConvergeLoopCnt;

  public:
    static TearVarBlkList List;
    static TearVarBlk *Find(char * Tag);
    static TearVarBlk *staticFindObjTag(pchar pSrchTag, flag SrchAll, int &ObjTagLen, int MinTagLen);
    static TearVarBlk *Add(TearPosBlk *Pos, char *Tag);
    static TearVarBlk *Add(TearVarBlk *Blk);
    static void Delete(TearVarBlk *Var);
    static void CleanUp();

    TearVarBlk(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    TearVarBlk(pchar Tag_, pTaggedObject pAttach);
    virtual ~TearVarBlk();
    void           CommonConstruct();

    virtual void   OnEditConstruct() {};
    virtual void   OnEditDestroy() {};

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual pchar  TagOfParent() { return PlantModelTag; };

    void           Connect(TearPosBlk *PosBlk);//, flag CreateIfReqd);
    void           DisConnect();
    int            Active() { return TaggedObject::GetActive() && (pPosBlk !=NULL); };
    int            InUse() { return fInUse; };
    void           SetInUse(int InUse) { fInUse=InUse; };
    void           SetActive(int On);

    byte           ConvergeMethod() { return m_iTearMethod; };
    LPCTSTR        ConvergeMethodStr()
      {
      return TearConvergeMethodStr(m_iTearMethod);
      }
    void           SetConvergeMethod(byte i) { m_iTearMethod=i; };
    bool           SetConvergeMethod(LPCTSTR Str)
      {
      int i=FindTearConvergeMethod(Str);
      if (i>=0)
        {
        m_iTearMethod=i;
        return true;
        }
      return false;
      };
    byte           EPSStrategy() { return m_iEPSStrategy; };
    LPCTSTR        EPSStrategyStr() { return (m_iEPSStrategy>=0 && m_iEPSStrategy<TCM_Last) ? EPSStrategyStrings[m_iEPSStrategy] : ""; };
    void           SetEPSStrategy(byte i) { m_iEPSStrategy=i; };
    bool           SetEPSStrategy(LPCTSTR Str)
      {
      for (int i=0; i<TCM_Last; i++)
        if (_stricmp(Str, EPSStrategyStrings[i])==0)
          {
          m_iEPSStrategy=i;
          return true;
          }
      return false;
      };

    void           SetNVariables(int Variables, byte Used);

    int            NVariables() { return nVariables; };
    void           SetNHistory(int History);
    int            NHistory() { return nHistory; };
    long           SeqConvergedCnt() { return m_lSeqConvergedCnt; };
    long           ConvergeLoopCnt() { return m_lConvergeLoopCnt; };
    byte           TearTagTyp() { return iTearTagTyp; };
    char*          LHSTag(Strng & Tg);
    char*          RHSTag(Strng & Tg);

    TearPosBlk    *PosBlk() { return pPosBlk; };

    void           SetRqdTearType(byte Type)  { pPosBlk->SetRqdTearType(Type); };
    byte           RqdTearType()              { return pPosBlk->RqdTearType(); };
    void           SetTearType(byte Type)     { pPosBlk->SetTearType(Type); };
    byte           TearType()                 { return pPosBlk->TearType(); };
    flag           IsTear()                   { return pPosBlk->IsTear(); };
    flag           IsSystemTear()             { return pPosBlk->IsSystemTear(); };
    byte           TearPriority()             { return pPosBlk->TearPriority(); };
    void           SetTearPriority(byte iPri) { pPosBlk->SetTearPriority(iPri); };
    //byte           TearInitMode()             { return pPosBlk->TearInitMode(); };
    byte           TearInitWhen()             { return pPosBlk->TearInitWhen(); };
    byte           TearInitHow()              { return pPosBlk->TearInitHow(); };
    byte           TearInitEstUsage()         { return pPosBlk->TearInitEstUsage(); };
    bool           TearInitActive()           { return pPosBlk->TearInitActive(); };
    //void           SetTearInitMode(byte iMd)  { pPosBlk->SetTearInitMode(iMd); };
    void           SetTearInitWhen(byte iMd)  { pPosBlk->SetTearInitWhen(iMd); };
    void           SetTearInitHow(byte iMd)   { pPosBlk->SetTearInitHow(iMd); };
    void           SetTearInitEstUsage(byte iEu) { pPosBlk->SetTearInitEstUsage(iEu); };
    void           SetTearInitActive(bool On, bool SetCount) { pPosBlk->SetTearInitActive(On, SetCount); };
    void           SetTearInitCounters(bool AtStart) { pPosBlk->SetTearInitCounters(AtStart); };
    void           BumpTearInitCounters()     { pPosBlk->BumpTearInitCounters(); };
    double         EstimatePortion()          { return pPosBlk->EstimatePortion(); };

    bool           TearAdvanceReqd()          { return pPosBlk && (pPosBlk->TearVarType() & TVT_Tear) !=0; };
    bool           TearTestReqd()             { return pPosBlk && (pPosBlk->TearVarType() & (TVT_Test))!=0; };

    void           TearInitialiseOutputs(double EstimatePortion) { pPosBlk->TearInitialiseOutputs(EstimatePortion); };
    void           TearInputs2Outputs()       { pPosBlk->TearInputs2Outputs(); };
    char *         TearGetTag(Strng & RootTag) { return pPosBlk->TearGetTag(RootTag); };
    int            TearGetConvergeInfo()      { return pPosBlk->TearGetConvergeInfo(TV); };
    void           TearGetErrors()            { pPosBlk->TearGetErrors(TV); };
    void           TearGetInputs()            { pPosBlk->TearGetInputs(TV); };
    void           TearGetOutputs()           { pPosBlk->TearGetOutputs(TV); };
    void           TearSetOutputs()           { pPosBlk->TearSetOutputs(TV); };
    flag           TearAdvance(EqnSlvCtrlBlk & EqnCB) { return pPosBlk->TearAdvance(TV, EqnCB); };

    int            IsGroupBlk() { return fGroupBlk; };

    double         DampingRqd(EqnSlvCtrlBlk & EqnCB, int iVar);
    double         EPS_Abs(EqnSlvCtrlBlk & EqnCB, int iVar);
    double         EPS_Rel(EqnSlvCtrlBlk & EqnCB, int iVar);

    byte           DampAsGroupCfg() { return m_iDampAsGroup; };
    double         DampingRqdCfg() { return m_dDampingRqd; };
    double         EPS_AbsCfg() { return dEPS_Abs; };
    double         EPS_RelCfg() { return dEPS_Rel; };
    void           SetDampAsGroupCfg(byte On) { m_iDampAsGroup=On; };
    void           SetDampingRqdCfg(double Val) { m_dDampingRqd=Val; };
    void           SetEPS_AbsCfg(double Val) { dEPS_Abs=Val; };
    void           SetEPS_RelCfg(double Val) { dEPS_Rel=Val; };

    void           SetHoldDamping(bool On)   { m_bHoldDamping=On; };
    flag           HoldDamping()             { return m_bHoldDamping; };

  protected:
    void           Initialise(bool ForceIt);
    void           BeginStep(EqnSlvCtrlBlk & TCB);
    void           EndStep(EqnSlvCtrlBlk & TCB);
    flag           TestConverged(EqnSlvCtrlBlk & TCB, double &Error, Strng &ErrTag);
    void           RotateInputs();
    void           RotateErrors();
    void           RotateOutputs();

    void           AdvWegstein(EqnSlvCtrlBlk & TCB);
    void           AdvDirect(EqnSlvCtrlBlk & TCB, double Damping);
    void           AdvAdaptDirect(EqnSlvCtrlBlk & TCB, double Damping);
    void           Advance(EqnSlvCtrlBlk & TCB);

    Strng &        SymOrTag(int i) { return TV[i].Sym.Length()>0 ? TV[i].Sym : TV[i].Sym; };

    void           GetGroupGlblTag(Strng & Tag) { Tag.Set("%s_%s", sGroupTag(), "Tear"); };

  public:
  };

//=========================================================================
//
//
//
//=========================================================================

#if KeepOldTearVarBlkEdit
class DllImportExport TearVarBlkEdt : public FxdEdtBookRef
  {
  public :
    TearVarBlk &   rTVB;
    CRect          GraphR;
    int            iDragPtNo;  //the index of the point being moved (-1 if none)
    int            iNameWidth; //
    int            iPg1;
    CnvAttribute   DampCnv, MeasCnv, DiffCnv, EPSCnv;//, *pWrkCnv;
    FmtAttribute   DampFmt, MeasFmt, DiffFmt, EPSFmt;//, *pWrkFmt;

    TearVarBlkEdt(FxdEdtView *pView_, TearVarBlk *pTVB_);//, rStrng Tag, rStrng Name);
    virtual ~TearVarBlkEdt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoMouseMove(UINT nFlags, CPoint point);

    virtual flag DoAccCnv(UINT Id);
    virtual flag DoAccFmt(UINT Id);
    virtual flag DoAccRptTagLists();

  private:

  };
#endif

//=========================================================================
//
//
//
//=========================================================================

const int MAX_EQNSLV_WORST = 5;
const int MAX_EQNSLV_MSGLEN = 96;
class DllImportExport WorstEqn
  {
  public:
    double         dErr;
    byte           iTearTagTyp;
    char           cStr[MAX_EQNSLV_MSGLEN];

    WorstEqn()     { dErr=0.0; cStr[0]=0; iTearTagTyp=TTT_Unknown; };
  };

//=========================================================================
//
//  EqnSlv Control .. Seperate because it is needed across dll interface
//
//=========================================================================
#define ALLOWMASSSMOOTH 0
//NB: MassSmoothing in ProBal solver matrix does not work correctly. Until fixed disable this!!!

class DllImportExport EqnSlvCfgBlk
  {
  public:
    byte           iConvergeMeth;
    flag           fMassSmooth;
    flag           fForceConverged:1,
                   fIdleWhenDone:1;
    short          iRqdCnvrgdIters;
    long           iMaxIters;
    short          iWA_Delay;
    double         dEps_Rel;
    double         dEps_Abs;
    double         m_dDamping;
    double         dWA_Bound;
    double         dWA_Clamping;
    double         dDampFctGrowth;
    double         dDampFctDecay;
    byte           m_iDampAsGroup;

    EqnSlvCfgBlk() { InitProBal(); };

    void           InitProBal();
    void           InitDynamic();
  };

//=========================================================================
//
//
//
//=========================================================================

const byte ESR_Snks    = 0x01;
const byte ESR_Srcs    = 0x02;
const byte ESR_Errors  = 0x04;
const byte ESR_Species = 0x08;

class DllImportExport EqnSlvCtrlBlk
  {
  public:
    EqnSlvCfgBlk  Cfg;

  protected:
    long           nIters;
    long           nConvergedIters;
    long           nBadError;
    long           nBadLimit;
    flag           fConfigOK;
    flag           fConverged;
    //ag           fSolutionOK;

    flag           bHoldTearUpdate;
    long           nUnknowns;
    byte           iCollectWorst;
    byte           iReport;
    double         dReportTol;
    double         dWorstErr;
    double         dWorstOtherErr;
    double         dTheWorstErr;

  public:
    WorstEqn       Worst[MAX_EQNSLV_WORST];
    WorstEqn       WorstOther;

    EqnSlvCtrlBlk();
    void   Init();
    void   BeginStep();
    void   EndStep();

    flag   NoteBadEqnError(double Err, double RelTol);
    void   CollectBadEqnError(double Err, double Val, double AbsTol, double RelTol, double Damping, char * Tag, byte TearTagTyp);
    flag   NoteBadEqnLimit();
    void   CollectBadEqnLimit(double Val, char * Tag, byte TearTagTyp);
    flag   NoteWorstError(double Err, double RelTol);
    void   CollectWorstError(double Err, double Val, double AbsTol, double RelTol, double Damping, char * Tag, byte TearTagTyp);
    void   SetCollectWorst(byte C);

    long   NIters() const { return nIters; };
    long   NConvergedIters() const { return nConvergedIters; };
    long   NBadError() const { return nBadError; };
    long   NBadLimit() const { return nBadLimit; };
    flag   ConfigOK() const { return fConfigOK; };
    flag   Converged() const { return fConverged; };

    flag   HoldTearUpdate() const { return bHoldTearUpdate; };
    long   NUnknowns() const { return nUnknowns; };
    byte   Report() const { return iReport; };
    double ReportTol() const { return dReportTol; };
    double WorstErr() const { return dWorstErr; };
    double WorstOtherErr() const { return dWorstOtherErr; };
    double TheWorstErr() const { return dTheWorstErr; };
    byte   CollectWorst() const { return iCollectWorst; };

    void   BumpNIters() { nIters++; if (fConverged) nConvergedIters++; };
    void   BumpNBadError() { nBadError++; };
    void   BumpNBadLimit() { nBadLimit++; };
    void   BumpNUnknowns() { nUnknowns++; };

    void   SetNBadError(long E) { nBadError=E; };
    void   SetNBadLimit(long L) { nBadLimit=L; };
    void   SetNIters(long I) { nIters=I; };
    void   SetConfigOK(flag C) { fConfigOK=C; };
    void   SetConverged(flag C) { fConverged=C; if (!fConverged) nConvergedIters=0; };
    void   SetHoldTearUpdate(flag H) { bHoldTearUpdate=H; };
    void   SetNUnknowns(long U) { nUnknowns=U; };
    void   SetReport(byte R) { iReport=R; };
    void   SetReportTol(double T) { dReportTol=T; };

  };
typedef CArray <TearVarBlk*, TearVarBlk*> TearVarPtrArray;

//=========================================================================
//
//
//
//=========================================================================

class DllImportExport EqnSolverBlk
  {
  public:

    EqnSolverBlk();
    ~EqnSolverBlk();


    void           SetTearsInUse(int InUse);
    void           ActivateAllReqdTears();
    void           DeActivateAllTears();
    void           ConnectAllTears(flag CreateIfReqd);
    void           DisConnectAllTears();
    void           FindAllTearsInUse();
    void           RemoveAllTears();
    void           RemoveUnusedTears();
    void           RemoveInActiveTears();
    void           InitialiseActiveTears(EqnSlvCtrlBlk & EqnCB);
    void           RestartAllActiveTears(EqnSlvCtrlBlk & EqnCB);

    //void           InitialiseAllTearOutputs(EqnSlvCtrlBlk & EqnCB);
    void           TransferAllTears(EqnSlvCtrlBlk & EqnCB);
    flag           TestTearConvergence(long ConvergeLoopCnt, EqnSlvCtrlBlk & EqnCB, Strng_List &BadTearTags, Strng_List & BadTearInfo);

    void           BeginStep(EqnSlvCtrlBlk & TCB);
    void           EndStep(EqnSlvCtrlBlk & TCB);

  protected:

  };

//=========================================================================
//
//
//
//=========================================================================

#undef DllImportExport

#endif
