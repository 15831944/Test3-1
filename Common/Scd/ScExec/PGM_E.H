//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __PGM_E_H
#define  __PGM_E_H

#ifndef __SC_DEFS_H
#include <sc_defs.h>
#endif  
#ifndef __EXECLIB_H
#include "execlib.h"
#endif
#ifndef __SFE_BASE_H
#include "sfe_base.h"
#endif
#ifndef __AFXTEMPL_H__
#include <afxtempl.h> // "xafxtempl.h"
#endif
#ifndef __EQNSOLVE_H__
#include "eqnsolve.h"
#endif
#ifndef __DATAATTR_H__
#include "dataattr.h"
#endif
#ifndef __PGM_D_H__
#include "pgm_d.h"
#endif
#ifndef __TKNPARS_H
#include "tknpars.h"
#endif
#ifndef __ELECBASE_H
#include "elecbase.h"
#endif
#include "xrefs.h"

#ifdef __PGM_E_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

class CPGMDbgMngr;

//_FWDDEF(GCXRefHelper)
_FWDDEF(GCIns)
_FWDDEF(GCInsMngr)
_FWDDEF(GControl)
_FWDDEF(GCDef)
_FWDDEF(GCVar)
_FWDDEF(GCBitVar)
_FWDDEF(GCByteVar)
_FWDDEF(GCLongVar)
_FWDDEF(GCDoubleVar)
_FWDDEF(GCStrVar)
_FWDDEF(GCFunctVar)
_FWDDEF(GCClassVar)
_FWDDEF(GCTagVar)
_FWDDEF(GCStrTagVar)
_FWDDEF(GCLabelVar)
class GCArrayWatch;

//============================================================================

const short MAX_CALC_STACK = 40; //maximum depth required for internal calculation stack
const short MAX_STR_STACK = 16;  //maximum depth required for internal string stack
const int MAX_FUNCT_STACK = 1024;//maximum number of functions that can be called from within functions (ie depth)
const short MAX_FUNCT_PARMS = 32;//maximum number of parameters that can be passed to a function 
const short MAX_CLASSREF_STACK = 128; //maximum number of stored class references needed when calling class functions
const int MaxWhileStack = 64;    //maximum number of nested while loops allowed
const int MaxTagStack = 4;       //maximum number of nested SetTag/GetTags allowed (should NEVER get bigger than 2)
const int MaxTknMemSize = 512;   //maximum memory required for a token
const int MaxTknSize = 511;      //maximum number of characters for a token
const short MaxCondMsgs = 8;     //maximum number of CI condition note/error messages supported

const double dGCtrue  = 1.0; // CNM changed from -1 to +1 : in c++ comparison returns +1 for true
const double dGCfalse = 0.0;
const flag bGCtrue  = 0xff;
const flag bGCfalse = 0;
const flag bitGCtrue  = 1;
const flag bitGCfalse = 0;

const char ErrBuild        = 'B';
const char ErrMathRuntime  = 'M';
const char ErrOtherRuntime = 'R';

//variable unique id's...
const char VarDouble = 'D';
const char VarLong   = 'L';
const char VarByte   = 'Y';
const char VarBit    = 'B';
const char VarStr    = 'S';
const char VarFunct  = 'F';
const char VarTag    = 'T';
const char VarClass  = 'C';
const char VarArray  = 'A';
const char VarLabel  = 'E';

const DWORD VarConst       = 0x00000001; //variable is a constant and may not be altered by PGM code
const DWORD VarClassDefn   = 0x00000002; //is not actually a variable but contains a variable definition
const DWORD VarPgmWatch    = 0x00000004; //variable is watched ie accessable externally
const DWORD VarReadOnly    = 0x00000008; //variable is read only externally
const DWORD VarTagOut      = 0x00000010; //tag variable is set by PGM code
const DWORD VarTagIn       = 0x00000020; //tag variable is read by PGM code
const DWORD VarTagNear     = 0x00000040; //tag variable accessed by PGM code is "nearby" (ie in flowsheet)
const DWORD VarTagFar      = 0x00000080; //tag variable accessed by PGM code is "far away" (ie external eg driver)
const DWORD VarSetNotify   = 0x00000100; //if the variable is set must the PGM class be notified
const DWORD VarGetNotify   = 0x00000200; //if the variable is read must the PGM class be notified
const DWORD VarStrFunct    = 0x00000400; //if the variable is a FunctVar that returns a Str
const DWORD VarDynTag      = 0x00000800; //CNM - moved m_bDynTag
const DWORD VarDynTagOK    = 0x00001000; //is the dynamic tag OK (ie found and legal)
const DWORD VarArrayClass  = 0x00002000; //is this an array class
const DWORD VarMatrixClass = 0x00004000; //is this a matrix class
const DWORD VarClassUsed   = 0x00008000; //is this class defn used
const DWORD VarPageLabel   = 0x00010000; //access window tab page label

const WORD DefLHS       = 0x0001; //instruction/symbol is allowed as a left hand side
const WORD DefExp       = 0x0002; //symbol is allowed in expressions
const WORD Def2ndLHS    = 0x0004;
const WORD DefOperator  = 0x0008; //symbol is an operator
const WORD DefDataType  = 0x0010; //symbol is a data type (float, word etc)
const WORD DefRetStr    = 0x0020; //function returns a string
const WORD DefSolverSet = 0x0040; //reserved word is used in solver only  
const WORD DefGenConSet = 0x0080; //reserved word is used in gencon
const WORD DefFunctSet  = 0x0100; //reserved word is used in functions

enum VarLocation { VL_NotFound, VL_Funct, VL_Class, VL_Global, VL_GlobalTag };

const int MaxAccWndTabPages = 80; //Note: same as "FixedEditView::MaxFixedPages"

//=========================== 'Exception handler' ===========================

class GCException : public CException
  {
  public :
    DECLARE_DYNAMIC(GCException);
    //static enum PGMErrors {noError,Error,noFile};
    int m_ErrNo;
    GCException() 
      { m_ErrNo = 0; };
  };

//===========================================================================

class CGCXRefItem;

//===========================================================================

void DefinePGMClasses(GCInsMngr &IB); //implemented elsewhere to allow PGM Class Definitions to be added

//===========================================================================

class GCLoadHelp
  {
  public:
    GCFunctVar*    m_pFunctVar;    //pointer to function if a function is currently being built
    GCClassVar*    m_pClassVar;    //pointer to class if a class is currently being built
    WORD           m_bExpectStr:1, //nasty trick, temp flag
                   m_bEqualFlag:1, //nasty trick, temp flag
                   m_bDoingNext:1, //is the instruction constructor busy making a new instruction block
                   m_bPrevLHS:1,   //is the current parsing on the right of an =
                   m_bBreakLine:1, //next instruction can have a break point
                   m_bWhileIns:1,  //next instruction must go on the while stack
                   m_bEOF:1,       //has the $ at the end of the pgm been found
                   m_bFunctRetFnd:1;//has a return been found in a function
    int            m_iPushPopClassDepth; //how deep into push/pop classes are we during compile
    GCVar*         m_pTempVar;     //nasty trick to get latest GCfunct just created
    CTokenParser*  m_pTknParser;   //pointer to token file being read
    WORD           m_iInsSet;      //flag to indicate which set of symbols are valid
    int            m_iBreakLineNo; //line number where break must occur
    pGCIns         m_WhileStack[MaxWhileStack]; //
    int            m_iWhilePos;    //current position in while stack
    GCVar*         m_pTempTagVars[MaxTagStack]; //nasty trick to get latest GCTag just created
    Strng          m_sTempCnvTxt[MaxTagStack];  //Cnv Text to be used by a GCTagVar, temp storage
    int            m_iTempTagPos;  //current position in tag stack
    int            m_iTxtLblCnt;   //next text label count
    int            m_iPgLblCnt;   //next page label count

    GCLoadHelp();
    int GetCurLineNo();
    void AddToTagStack(GCInsMngr &IB, GCVar* pVar, char* pCnvTxt);
  };

class CGExecContext
  {
  public:
    //set
    CNodeXRefMngr* pXRM;
    CTimeValue dIC_Time;
    CTimeValue dIC_dTime;
    pchar   StartLabel;
    pchar   EndLabel;
    flag    OnInitialise;
    flag    OnStart;
    flag    OnTerminate;
    flag    OnLoaded;
    flag    OnEmpty;
    flag    OnPreset;
    long    m_NetMode;
    long    m_HeatMode;
    flag    m_HoldNearXRefGet;
    flag    m_HoldNearXRefSet;
    //returned...
    flag    DoXStop;
    flag    DoXIdle;

    CGExecContext(CNodeXRefMngr* ERH)
      {
      pXRM = ERH;
      m_NetMode = DefNetMode();
      m_HeatMode = DefHeatMode();
      dIC_Time = CTimeValue(0.0);
      dIC_dTime = CTimeValue(0.0);
      StartLabel = NULL;
      EndLabel = NULL;
      OnInitialise = False;
      OnStart = False;
      OnTerminate = False;
      OnLoaded = False;
      OnEmpty = False;
      OnPreset = False;
      m_HoldNearXRefGet=false;
      m_HoldNearXRefSet=false;
      DoXStop = False;
      DoXIdle = False;
      }
    void SetFlags(flag OnInitialise_, flag OnStart_, flag OnTerminate_, flag OnLoaded_, flag OnEmpty_, flag OnPreset_)
      {
      OnInitialise = OnInitialise_;
      OnStart = OnStart_;
      OnTerminate = OnTerminate_;
      OnLoaded = OnLoaded_;
      OnEmpty = OnEmpty_;
      OnPreset = OnPreset_;
      }
  };
    
class GCVarMap : public CMap<LPCSTR, LPCSTR, GCVar*, GCVar*> {};

class DllImportExport GCInsMngr
  {
  public :
    GCInsMngr(LPCTSTR pFnExt=NULL);
    virtual ~GCInsMngr();

    void           Reset();
    flag           Load(byte Places, char* fullFileName, CNodeXRefMngr* pXRM, WORD InsSet);
    flag           Debug(byte Places, char* fullFileName, CNodeXRefMngr* pXRM, WORD InsSet);
    flag           LoadFunct(char* ParseBuff, WORD InsSet);
    flag           SaveScenario(FilingControlBlock &FCB);
    flag           LoadScenario(FilingControlBlock &FCB);
    
    int            UpdateXRefLists(CXRefBuildResults & Build);
    CXRefArray    &XRefs(byte e)    { return m_pXRM->m_XRefsOwnedByMe[e]; };

    void           GetTermStripData(CGExecContext &ECtx);
    void           PutTermStripData(CGExecContext &ECtx);

    int            Execute(CGExecContext &ECtx, LPCTSTR TriggerName=NULL, CNodeElectrics * pTermStripPtrs=NULL);

  //protected:
    flag           ParseTerminalIO();
    void           Build( pGCDef pExitDef,
                          pGCIns pPrevIns = NULL);
    double         GetDParm();
    long           GetLParm();
    unsigned char  GetCParm(); //byte
    flag           GetBParm();
    pchar          GetSParm();
    int            SetBrkPt(int LineNo);
    void           Err(char Code, int errNo, pchar ErrMsg = "");
    void           StdCheck(pchar Tkn);
    double         ParseNumber();
    void           AddArrayWatch(pGCClassVar pClassVar, bool ReadOnly, int index0, int index1);
    void           DoConstructArray(rGCDef Def, pGCVar &pVarList, GCVarMap * pVarMap, pchar pVarName, int Cnt);
    void           DoConstructVars(rGCDef Def, pGCVar &pVarList, GCVarMap * pVarMap);
    void           DoConstructExprssn( flag ExpectStr = False);
    GCFunctVar*    DoConstructFnt(pGCVar &pVarList, GCVarMap * pVarMap);
    GCVar*         DoConstructTag(char Terminate);
    void           DoConstructParms( int ParmCount, 
                                     dword StrParmMask = 0x0,
                                     flag StartExpected = True,
                                     flag EndExpected = True);
    void           DoConstructVarCnv(GCVar* pVar);
    void           DoConstructClass();
    void           DoConstructClassArrayAccess();

    GCVar*         FindVar(pchar Tkn, short &VarIndex, VarLocation &eVarLoc);
    GCVar*         TryAddDynTag(char* Tag, bool ForGet, bool IsString);
    int            UpdateXRef(GCVar* pVar);//, CGCXRefItem * pXRef);

  protected:
    flag           BuildCode(byte Places, char* fullFileName, WORD InsSet, char* ParseBuff, flag AddingCode=False, int FirstInsBlkSize=4096, int NextInsBlkSize=2048);
    void           OptimizeCode();
    void           DoOptimizeVars(GCVar* pVar);
    void           DestroyCode();
    void           DestroyCodeFrom(pGCIns pIPtr, char* OldCurBlk);
    void           DestroyVars(pGCVar& pVarList);
  public:
    void           PushStringValue(LPSTR Value);
    void           UnlinkAllXRefs();
  protected:
    void           DestroyTempDynTagVars();
    void           SaveVars();
    void           DoSaveVars(CProfINIFile& PF, GCVar* pVar, char* pClassName);
    void           RestoreVars();
    void           DoRestoreVars(CProfINIFile& PF, GCVar* pVar, char* pClassName);
    void           ConstructConstants();
    void           SaveClass(FilingControlBlock &FCB, GCVar* pVar);
    void           LoadClass(FilingControlBlock &FCB, GCVar* pClssVar);
    void           DbgVarList();
    void           DbgCalcStack();
    void           DbgExe();

  public:
    CNodeXRefMngr *m_pXRM;              //pointer to the external ref helper, ie the code that is using the PGM
    CTimeValue     m_dIC_Time;          //IntegrationControl Time
    CTimeValue     m_dIC_dTime;         //IntegrationControl dTime
    CSVector       m_FilesUsed;         //array of all files directly used by PGM
    CArray<FILETIME, FILETIME&> m_FilesTime; //array of all files times directly used by PGM
    GCLoadHelp*    m_pH;                //
    WORD           m_bDoDebug:1,        //is pgm debug on
                   m_bSetDbgOn:1,       //switch debug on at next load
                   m_bLogBuildErrs:1,   //must log notes be given for compile errors
                   m_bLogAllBuildErrs:1,//must log all notes be given for compile errors
                   m_bDidSaveVars:1;    //done a save vars
    int            m_iRunTimeErr;       //number of runtime errors during last execute
    int            m_iMathRunTimeErr;   //number of math runtime errors during last execute
    int            m_iFirstInsBlkSize;  //size of 1st memory block to be allocated for instruction code
    int            m_iNextInsBlkSize;   //size of all succesive memory blocks to be allocated for instruction code
    CArray<GCArrayWatch*, GCArrayWatch*> m_ExtraWatchArray; //array of special watch variables
    GCVar*         m_pVarList;          //pointer to first var in list of variables
    GCVarMap       m_VarMap;            //pointer to first var in list of variables
    GCVar*         m_pOnInitialiseVar;  //pointer to Initialise Flag
    GCVar*         m_pOnStartVar;       //pointer to Start Flag
    GCVar*         m_pOnTerminateVar;   //pointer to Start Flag
    GCVar*         m_pRunMode;          //pointer to Run Mode
    GCVar*         m_pHeatMode;         //pointer to Heat Mode
    GCVar*         m_pOnLoadedVar;      //pointer to Just Loaded Flag
    GCVar*         m_pOnEmptyVar;       //pointer to OnEmpty Flag
    GCVar*         m_pOnPresetVar;      //pointer to OnPreset Flag
    GCStrVar*      m_pModelTagVar;      //pointer to tag of model using the pgm code
    GCVar*         m_pStopVar;          //pointer to StopSimulation Flag
    GCVar*         m_pPauseVar;         //pointer to PauseSimulation Flag
    Strng          m_sTempSaveFile;     //temporary file to save PGM vars
    Strng          m_sTempSaveOtherFile;//temporary file to save PGM wrapped class vars
    Strng          m_StateLine[5];      //array of string messages
    Strng          m_CINMsgs[MaxCondMsgs];//array of CI note messages
    flag           m_CINOn[MaxCondMsgs];//array of active error conditions
    Strng          m_CIEMsgs[MaxCondMsgs];//array of CI error messages
    flag           m_CIEOn[MaxCondMsgs];//array of active error conditions
    CPGMDbgMngr    m_DbgMngr;           //PGM debugger
    GCIns*         m_pEndIns;           //pointer to last instruction
    Strng          m_sFnExtn;           //The file extention to be used PGM ECB ...
  //class reference scope stack...
    GCClassVar*    m_pClassRefStack[MAX_CLASSREF_STACK];
    short          m_iClassRefPos;
  //calculation stack..
    double         m_dCalcStack[MAX_CALC_STACK];
    short          m_iCalcPos;
    Strng          m_StrStack[MAX_STR_STACK];
    short          m_iStrPos;
    Strng          m_sLastSParm;
  //instruction stack...
    char*          m_pFirstBlk;
    char*          m_pCurrentBlk;
    union
      {
      char*        m_pcIPtr;
      GCIns*       m_pIPtr;
      };
  //function call stack  
    GCIns* m_FntStack[MAX_FUNCT_STACK];
    int m_FntStackPos;

  //other

    CETermStripDefnArray  m_TermStripDefns;
    CNodeElectrics * m_pTermStripPtrs;

    Strng          m_sFldName;

    bool           m_ProcExecuted;

    static double  sm_ExecTimeout; // Execution Timeout in seconds.
  };

//============================================================================

class DllImportExport CFnMngr     : public GCInsMngr 
  {
  public:
    CFnMngr    ();

    void Reset(CNodeXRefMngr* pXRM, flag LogBuildErrors=False);
    flag AddFunct(char* Name, char* pTxt, char*FldName);
    GCVar* GetFunctVar(char* Name);
  };

//============================================================================

class GCDef
  {
  public:
    static pGCDef  m_pFirst;
    static flag    m_bDoneDefFlags;
    WORD           m_defFlags;
    char           m_prec; 
  protected:
    pGCDef         m_pNext;
    pchar          m_pName;
    GCDef(pchar Name_);
  public:
    virtual pGCIns Construct(GCInsMngr &IB) = 0;
    virtual GCVar* ConstructVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags) { ASSERT(FALSE); return NULL; };
    static  pGCDef Find(pchar pName);
    pchar          Name() { return m_pName;};
    pGCDef         Next() { return m_pNext;};
  };

//============================================================================
#if WITHDEBUGNEW 
#ifdef new
#undef new
#define NEED_new
#endif
#endif
class GCIns
  {
  public:
    GCIns() {};
    virtual ~GCIns();
    pvoid          operator new (size_t Size, GCInsMngr &IB);
#if (_MSC_VER>1100)
    // needed for version 6.0
    void           operator delete (pvoid pIns, GCInsMngr &IB);
#endif
    void           operator delete (pvoid pIns);
    virtual void   Exec(GCInsMngr &IB)    = 0;
    virtual void   Advance(GCInsMngr &IB) = 0;
    virtual pchar  GetDesc(pchar Buff) { strcpy(Buff, Code()->Name()); return Buff; };
    virtual pGCDef Code() = 0;
  };
#if WITHDEBUGNEW 
#ifdef NEED_new
#undef NEED_new
#define new DEBUG_NEW 
#endif
#endif

//===========================================================================

_FWDDEF(CWatchInfo)
class CWatchInfo
  {
  public:
    double dMin;   //minimum value (for trend display)
    double dMax;   //minimum value (for trend display)
    Strng sDesc;   //tag description

    CWatchInfo(double Min, double Max, pchar Desc)
      { dMin = Min; dMax = Max; sDesc = Desc; };
  };

//===========================================================================

_FWDDEF(CCnvInfo)
class CCnvInfo
  {
  public:
    CCnvIndex iCnvIndex;   //conversion family index
    Strng  sCnvTxt;     //conversion unit used

    CCnvInfo(CCnvIndex CnvIndex, pchar CnvTxt)
      { iCnvIndex = CnvIndex; sCnvTxt = CnvTxt; };
    inline double Cnv(double d, pchar ReqdCnvTxt) 
      { return Cnvs[(iCnvIndex)]->Human(Cnvs[(iCnvIndex)]->Normal(d, sCnvTxt()), ReqdCnvTxt); };
  };

//===========================================================================

class DllImportExport GCVar
  {
  friend class GCInsMngr;
  protected:
    GCVar( pGCVar &pFirstVar,
           GCVarMap * pVarMap,
           pchar pName, 
           DWORD flags);
    GCVar*         m_pNext;   //points to next var in linked list
    GCVarMap     * m_pVarMap;
    pchar          m_pName;   //points to identifier name
  public:
    virtual ~GCVar();
    DWORD          m_flags;       //flags
    CWatchInfo*    m_pWatchInfo;  //points to extra tag info, NULL if none
    CnvAttribute*  m_pCnvInfo;    //points to conversion info, NULL if none

    GCVar*         Find(pchar pName);
    GCVar*         Find(pchar pName, short &Index);
    GCVar*         Find(short Index);
    GCVar*         FindTag(pchar pName);
    GCVar*         FindTagByType(pchar pName, char TagType);
    GCClassVar*    FindClass(pchar pName);
    GCClassVar*    FindClass(pchar pName, short& ClassIndex);
    inline pchar   Name() { return m_pName; };
    inline GCVar*  Next() { return m_pNext; };
    inline void    AddWatchInfo(double Min, double Max, pchar Desc)
      { m_pWatchInfo = new CWatchInfo(Min, Max, Desc); };
    inline void    AddCnvInfo(CCnvIndex CnvIndex, pchar CnvTxt)
      { m_pCnvInfo = new CnvAttribute(CnvIndex, CnvTxt); };
    inline void    AddCnvInfo(CnvAttribute* pCnvInfo)
      { m_pCnvInfo = new CnvAttribute(pCnvInfo->Index(), pCnvInfo->Text()); };
    inline CCnvIndex CnvIndex() { return m_pCnvInfo ? m_pCnvInfo->Index() : DC_; };
    inline char*   CnvTxt() { return m_pCnvInfo ? m_pCnvInfo->Text() : ""; };
    inline flag    ReadOnly() { return ((m_flags & VarReadOnly)!=0); };
    inline flag    Watched() { return ((m_flags & VarPgmWatch)!=0); };

    //virtual        void Init() {};
    //virtual        void Done() {};
    //virtual        double CallFunct(GCInsMngr &IB, short FunctId) { ASSERT(FALSE); return 0.0; }; //
    //virtual        void OnSave(FilingControlBlock &FCB/*FILE* f*/, pvoid pSubClass) { ASSERT(FALSE); };
    //virtual        flag OnLoad(FilingControlBlock &FCB/*rCTokenFile f*/, pvoid pSubClass) { ASSERT(FALSE); return True; };

    virtual pchar  GetDesc(pchar Buff, flag Short = True);
    virtual pchar  GetStrVal(pchar Buff) = 0;
    virtual char   WhatAmI() = 0;
    virtual double getD() = 0;
    virtual long   getL() = 0;
    virtual unsigned char getC() = 0;
    virtual flag   getB() = 0;

    virtual void   set(double d) = 0;
    virtual void   set(long l) = 0;
    virtual void   set(char c) = 0;
    virtual void   setC(unsigned char c) = 0;
    virtual void   set(flag b) = 0;

    // CNM
    virtual void   OnOptimize(CNodeXRefMngr * pXRM, pvoid pClssVar, pvoid pSubClass) {};
  };

//============================================================================

inline long GetAsLong(double d) 
  { double d1=Range((double)LONG_MIN, d, (double)LONG_MAX); return (long)d1; };
//  { return (long)(Range((double)LONG_MIN, d, (double)LONG_MAX)); };

//===========================================================================


class DllImportExport GCDoubleVar : public GCVar
  {
  public:
    GCDoubleVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags) : GCVar(pFirstVar, pVarMap, pName, flags) { m_var = 0.0 ;};
    GCDoubleVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags, double d) : GCVar(pFirstVar, pVarMap, pName, flags) { m_var = d ;};
    double         m_var;
    char           WhatAmI() { return VarDouble; };                               
    pchar          GetStrVal(pchar Buff) { Valid(m_var) ? sprintf(Buff, "%.12g", m_var) : sprintf(Buff, "*"); return Buff; };

    double         getD() { return m_var;}; 
    long           getL()  
      { double d=Range((double)LONG_MIN, m_var, (double)LONG_MAX); return (long)d; }; 
    unsigned char  getC()
      { return (unsigned char)Range(0.0, m_var, (double)UCHAR_MAX);}; 
    flag           getB() { if (m_var==dGCfalse) return False; return True;}; 

    void           set(double d) { m_var = d; }; 
    void           set(long l) { m_var = l; }; 
    void           set(char c) { m_var = c; };
    void           setC(unsigned char c) { m_var = c; };
    void           set(flag b) { m_var = /*b;*/(b ? dGCtrue : dGCfalse); }; 
  };

//===========================================================================

class DllImportExport GCLongVar : public GCVar
  {
  public:
    GCLongVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags) : GCVar(pFirstVar, pVarMap, pName, flags)  { m_var = 0; };
    GCLongVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags, long l) : GCVar(pFirstVar, pVarMap, pName, flags) { m_var = l ;};
    long           m_var;
    char           WhatAmI() { return VarLong; };
    pchar          GetStrVal(pchar Buff) { sprintf(Buff, "%d", m_var); return Buff; };

    double         getD() { return (double)m_var; };
    long           getL() { return m_var;}; 
    unsigned char  getC() 
      { return (unsigned char)Range((long)0, m_var, (long)UCHAR_MAX);}; 
    flag           getB() { if(m_var == 0) return False; return True; }; 

    void           set(double d) 
      { double d1=Range((double)LONG_MIN, d,(double)LONG_MAX); m_var = (long)d1; }; 
    void           set(long l) { m_var = l; }; 
    void           set(char c) { m_var = c; };
    void           setC(unsigned char c) { m_var = c; };
    void           set(flag b) { m_var = /*b;*/(b ? bGCtrue : bGCfalse); }; 
  };

//===========================================================================

class DllImportExport GCByteVar : public GCVar
  {
  public:
    GCByteVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags) : GCVar(pFirstVar, pVarMap, pName, flags) { m_var = 0; };
    GCByteVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags, unsigned char c) : GCVar(pFirstVar, pVarMap, pName, flags) { m_var = c ;};
    unsigned char  m_var;
    char           WhatAmI() { return VarByte; };
    pchar          GetStrVal(pchar Buff) { sprintf(Buff, "%d", m_var); return Buff; };

    double         getD() { return (double)m_var; };
    long           getL() { return (long)m_var;}; 
    unsigned char  getC() { return m_var;}; 
    flag           getB() { if(m_var == 0) return False; return True;}; 

    void           set(double d)
      { m_var = (unsigned char)Range((double)0, d, (double)UCHAR_MAX);}; 
    void           set(long l)
      { m_var = (unsigned char)Range((long)0, l, (long)UCHAR_MAX);}; 
    void           setC(unsigned char c) { m_var = c; };
    void           set(char c) { m_var = c; };
    void           set(flag b) { m_var = /*b;*/(b ? bGCtrue : bGCfalse); }; 
  };

//===========================================================================

class DllImportExport GCBitVar : public GCVar
  {
  public:
    GCBitVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags) : GCVar(pFirstVar, pVarMap, pName, flags) { m_var = 0; };
    GCBitVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags, flag b) : GCVar(pFirstVar, pVarMap, pName, flags) { m_var = b ;};
    flag           m_var;
    char           WhatAmI() { return VarBit; };
    pchar          GetStrVal(pchar Buff) { m_var ? sprintf(Buff, "True") : sprintf(Buff, "False"); return Buff; };
    double         getD() { return (m_var ? bitGCtrue : bitGCfalse); };
    long           getL() { return (long)(m_var ? bitGCtrue : bitGCfalse); };
    unsigned char  getC() { return (unsigned char)(m_var ? bitGCtrue : bitGCfalse); }; 
    flag           getB() { return (m_var ? bitGCtrue : bitGCfalse); };
    /*double         getD() { return (double)m_var; };
    long           getL() { return (long)m_var; };
    unsigned char  getC() { return (unsigned char)m_var; }; 
    flag           getB() { return m_var; };*/

    void           set(double d) { if(d == dGCfalse) m_var = bGCfalse; else m_var = bGCtrue; }; 
    void           set(long l) { m_var = (l ? bGCtrue : bGCfalse); }; 
    void           set(char c) { m_var = (c ? bGCtrue : bGCfalse); };
    void           setC(unsigned char c) { m_var = (c ? bGCtrue : bGCfalse); };
    void           set(flag b) { m_var = (b ? bGCtrue : bGCfalse); }; 
  };

//===========================================================================

class DllImportExport GCStrVar : public GCVar
  {
  public:
    GCStrVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags) : GCVar(pFirstVar, pVarMap, pName, flags) { m_var = ""; };
    GCStrVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags, pchar p) : GCVar(pFirstVar, pVarMap, pName, flags) { m_var = p; };
    Strng m_var;
    char           WhatAmI() { return VarStr; };
    pchar          GetStrVal(pchar Buff) { sprintf(Buff, "%s", m_var() ? m_var() : ""); return Buff; };
    double         getD() { return 0.0; };
    long           getL() { return 0; }; 
    unsigned char  getC() { return 0; }; 
    flag           getB() { return 0; }; 
    pchar          getS() { return (m_var() ? m_var() : ""); }; 

    void           set(double d) {}; 
    void           set(long l) {}; 
    void           set(char c) {};
    void           setC(unsigned char c) {};
    void           set(flag b) {};
    void           set(pchar p) { m_var = p; };

    pchar  GetDesc(pchar Buff, flag Short = True);
  };

//===========================================================================

//class DllImportExport CGCXRefItem : public CXRefItem
//  {
//  public:
//    CGCXRefItem(GCTagVar * pVar);
//    ~CGCXRefItem();      
//    CGCXRefItem(const CGCXRefItem &X);
//    GCTagVar        * m_pVar;
//
//    virtual bool      IsXRefActive() const             { return true; };
//
//    DEFINE_SPARES(CGCXRefItem)
//
//  };

//===========================================================================

class DllImportExport GCTagVar : public GCVar
  {
  public:
    GCTagVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags);
    virtual ~GCTagVar();   

    char           WhatAmI() { return VarTag; };                               
    pchar          GetStrVal(pchar Buff) 
      { 
      const MTagIOValue & V=m_pXRef->TagValue();
      switch (V.TIOType) 
        {
        case MTagType_Long:    sprintf(Buff, "%i", V.Long);                           return Buff; 
        case MTagType_Double:  sprintf(Buff, Valid(V.DoubleSI)? "%.12g":"*", V.DoubleSI); return Buff; 
        case MTagType_String:  sprintf(Buff, "%s", V.String);                         return Buff; 
        }
      return strcpy(Buff, "???"); 
      }

    double         getD() { return m_pXRef->GetXRefValue();};
    long           getL() { double d = Range((double)LONG_MIN, m_pXRef->GetXRefValue(), (double)LONG_MAX); return (long)d; };
    unsigned char  getC() { return (unsigned char)Range((double)0.0, m_pXRef->GetXRefValue(), (double)UCHAR_MAX); };
    flag           getB() { return (m_pXRef->GetXRefValue()==0.0) ? False : True; }; 

    void           set(double d) { m_pXRef->SetXRefValue(d);};
    void           set(long l) { m_pXRef->SetXRefValue(l);}; 
    void           set(char c) { m_pXRef->SetXRefValue(c);};
    void           setC(unsigned char c) { m_pXRef->SetXRefValue(c);};
    void           set(flag b) { double t=(b ? dGCtrue : dGCfalse); m_pXRef->SetXRefValue(t);}; 
  
    void           SetFldName(pchar FldName)
                     {
                     delete []m_pFldName;
                     m_pFldName = new char[strlen(FldName) + 1];
                     strcpy(m_pFldName, FldName);
                     };
    void           SetRqdCnvTxt(pchar CnvTxt)
                     {
                     delete []m_pRqdCnvTxt;
                     if (CnvTxt)
                       {
                       m_pRqdCnvTxt = new char[strlen(CnvTxt) + 1];
                       strcpy(m_pRqdCnvTxt, CnvTxt);
                       }
                     else
                       m_pRqdCnvTxt=NULL;
                     };
    char *         RqdCnvTxt() { return m_pRqdCnvTxt ? m_pRqdCnvTxt : ""; }
  
  public:
    CCnvIndex      m_iCnvIndex;
    flag           //m_bDynTag:1, 
                   m_bOnlyAllowGet:1,
                   m_bAllowSetAtInit:1,
                   //m_bXrefFound:1,
                   m_bTempDynTag:1,
                   m_bStrTag:1;
    CXRefItem    * m_pXRef;
    char         * m_pFldName;   //points to Field identifier name
    char         * m_pRqdCnvTxt;    //points to Cnv Text

    static long   m_nXRefNumber;
    long          m_iXRefNumber;
  };

//===========================================================================

class DllImportExport GCLabelVar : public GCVar
  {
  public:
    GCLabelVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags) : GCVar(pFirstVar, pVarMap, pName, flags) { m_var=""; };
    GCLabelVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags, pchar p) : GCVar(pFirstVar, pVarMap, pName, flags) { m_var=p; };
    Strng m_var;

    char           WhatAmI() { return VarLabel; };
    pchar          GetStrVal(pchar Buff) { sprintf(Buff, "%s", m_var() ? m_var() : ""); return Buff; };
    double         getD() { return 0.0; };
    long           getL() { return 0; }; 
    unsigned char  getC() { return 0; }; 
    flag           getB() { return 0; }; 
    pchar          getS() { return (m_var() ? m_var() : ""); }; 

    void           set(double d) {}; 
    void           set(long l) {}; 
    void           set(char c) {};
    void           setC(unsigned char c) {};
    void           set(flag b) {};
    void           set(pchar p) { m_var = p; };

    pchar  GetDesc(pchar Buff, flag Short = True);
  };

//===========================================================================

//inline CGCXRefItem::CGCXRefItem(GCTagVar * pVar) //: CXRefItem(&pVar->m_xValue)      
//  { 
//  //dbgpln("CGCXRefItem() %08x", this);
//  m_pVar=pVar; 
//  };
//inline CGCXRefItem::~CGCXRefItem() 
//  {
//  //dbgpln("~CGCXRefItem() %08x", this);
//  };      
//inline CGCXRefItem::CGCXRefItem(const CGCXRefItem &X) //: CXRefItem(X.m_pValue)
//  { 
//  //dbgpln("CGCXRefItem(const CGCXRefItem &X) %08x %08x", this, &X);
//  //m_pVar=X.m_pVar; 
//  };

//===========================================================================

class DllImportExport GCStrTagVar : public GCTagVar
  {
  public:
    GCStrTagVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags) : GCTagVar(pFirstVar, pVarMap, pName, flags)
      {
      m_bStrTag=1;
      };
    
    char           WhatAmI() { return VarTag; };
    pchar          GetStrVal(pchar Buff) { sprintf(Buff, "%s", m_pXRef->GetXRefStrValue()); return Buff; };

    double         getD() { return m_pXRef->GetXRefValue();};
    long           getL() { double d = Range((double)LONG_MIN, m_pXRef->GetXRefValue(), (double)LONG_MAX); return (long)d; };
    unsigned char  getC() { return (unsigned char)Range((double)0.0, m_pXRef->GetXRefValue(), (double)UCHAR_MAX); };
    flag           getB() { return (m_pXRef->GetXRefValue()==0.0) ? False : True; }; 

    void           set(double d) { m_pXRef->SetXRefValue(d);};
    void           set(long l) { m_pXRef->SetXRefValue(l);}; 
    void           set(char c) { m_pXRef->SetXRefValue(c);};
    void           setC(unsigned char c) { m_pXRef->SetXRefValue(c);};
    void           set(flag b) { double t=(b ? dGCtrue : dGCfalse); m_pXRef->SetXRefValue(t);}; 

    pchar          getS() { return m_pXRef->GetXRefStrValue(); }; 
    void           set(pchar p) { m_pXRef->SetXRefStrValue(p); };
  };

//===========================================================================

//inline CCnvIndex CGCXRefItem::GetRefCnvIndex() const           { return m_pVar->CnvIndex(); };
//inline void      CGCXRefItem::SetRefCnvIndex(CCnvIndex V)      { m_pVar->m_iCnvIndex=V; };

//===========================================================================

class DllImportExport GCFunctVar : public GCVar
  {
  public:
    GCFunctVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags) : GCVar(pFirstVar, pVarMap, pName, flags) 
      { m_var = 0.0; m_pVarList = NULL; m_pFntIPos = NULL; m_dwStrParmMask = 0x0; };
    virtual ~GCFunctVar();
    double  m_var;
    short   m_Parms;      // number of parameters for fnt
    pGCIns  m_pFntIPos;  // address of the function (PGM functions)
    short   m_FunctId;    // id of function (C++ functions)
    GCVar * m_pVarList;  // variables used in function class
    GCVarMap  m_VarMap;  // map of variables used in function class
    dword   m_dwStrParmMask;   // True if all the parameters are strings
    
    char           WhatAmI() { return VarFunct; };                               
    pchar          GetStrVal(pchar Buff) { Valid(m_var) ? sprintf(Buff, "%.12g", m_var) : sprintf(Buff, "*"); return Buff; };

    double         getD() { return m_var;}; 
    long           getL()  
      { double d=Range((double)LONG_MIN, m_var, (double)LONG_MAX); return (long)d;}; 
    unsigned char  getC()
      { return (unsigned char)Range((double)0, m_var, (double)UCHAR_MAX);}; 
    flag           getB() { if(m_var == 0) return False; return True;}; 

    void           set(double d) { m_var = d; }; 
    void           set(long l) { m_var = l; }; 
    void           set(char c) { m_var = c; };
    void           setC(unsigned char c)   { m_var = c; };
    void           set(flag b) { m_var = (b ? dGCtrue : dGCfalse); }; 
  };

//============================================================================

class DllImportExport GCClassVar : public GCVar
  {
  public:
    GCClassVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags) : GCVar(pFirstVar, pVarMap, pName, flags) 
      { m_pVarList = NULL; m_pRefClass = NULL; m_pSubClass = NULL; m_pParentClass = NULL; };
    GCClassVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags, pGCClassVar pRefClass);
    virtual ~GCClassVar();
    void           CopyClass();
    void           DestroySubClass();
    GCVar*         m_pVarList;// variables (and functions) in class
    GCVarMap       m_VarMap;  // map of variables used in function class
    GCClassVar*    m_pRefClass;//if this is not used as a class defenition, then this is the instance of the class and the variable points to the instance of the class defenition
    pvoid          m_pSubClass;//if this class refers to a PGM coded class then this is null, otherwise points to the instance of the c++ class
    GCClassVar*    m_pParentClass;//null if at top level, pointer to parent class if this class is a data member of a class

    //stuff for C++ classes implemented in the PGM
    virtual        void Init(pGCClassVar pClassVar) {};
    virtual        void Done(pGCClassVar pClassVar) {};
    virtual        double CallFunct(GCInsMngr &IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar)
       { ASSERT(FALSE); return 0.0; }; //Functions MUST be implemented in derived class !!!
    virtual        void OnSave(FilingControlBlock &FCB, pvoid pSubClass) {};
    virtual        flag OnLoad(FilingControlBlock &FCB, pvoid pSubClass) { return True; };
//    virtual        void OnRestore(pvoid pOldSubClass, pvoid pNewSubClass) {};
//    virtual        flag EvalPBConvergence(EqnSlvCtrlBlk & EQCtrl, pGCClassVar pClassVar) { return True; };
//    virtual        BOOL InitPBConvergence(pGCClassVar pClassVar) { return True; };
//    virtual        void InitIteration(pGCClassVar pClassVar) {}; //kkk

    //SaveVal is used for saving non PGM variables
    void SaveVal(FilingControlBlock &FCB,
                        pchar pValName,
                        double d);
    void SaveVal(FilingControlBlock &FCB,
                        pchar pValName,
                        CTimeValue d);
    void SaveVal(FilingControlBlock &FCB,
                        pchar pValName,
                        int i);
    void SaveVal(FilingControlBlock &FCB,
                        pchar pValName,
                        long i);
    void SaveVal(FilingControlBlock &FCB,
                        pchar pValName,
                        flag b);
    void SaveVal(FilingControlBlock &FCB,
                        pchar pValName,
                        pchar s);
    
    char           WhatAmI() { return VarClass; };                               
    pchar          GetStrVal(pchar Buff) { sprintf(Buff, ""); return Buff; };
    double         getD() { ASSERT(FALSE); return 0;}; 
    long           getL() { ASSERT(FALSE); return 0;}; 
    unsigned char  getC() { ASSERT(FALSE); return 0;}; 
    flag           getB() { ASSERT(FALSE); return 0;}; 

    void           set(double d) { ASSERT(FALSE); }; 
    void           set(long l)   { ASSERT(FALSE); }; 
    void           set(char c)   { ASSERT(FALSE); };
    void           setC(unsigned char c)   { ASSERT(FALSE); };
    void           set(flag b)   { ASSERT(FALSE); }; 

    pchar          GetDesc(pchar Buff, flag Short = True);
    inline flag    IsArrayClass() { return ((m_flags & VarArrayClass)!=0); };
    inline flag    IsMatrixClass() { return ((m_flags & VarMatrixClass)!=0); };
    void           SetArrayData(int index0, int index1, double d);
    double         GetArrayData(int index0, int index1);
    char*          GetFullInstanceName(Strng & s);

    GCVar*         GetVarByName(pchar pName);
    GCVar*         GetVarByIndex(int index);
    GCVar*         AddVar( GCInsMngr &IB,
                           pchar pName,
                           pGCDef pDef,
                           DWORD flags = 0);
    GCVar*         AddFunct( GCInsMngr &IB,
                             pchar pName,
                             short TtlParmCount,
                             dword StrParmMask,
                             short FunctId,
                             flag StrRet=false);
    int            GetVarCount();
  };

//===========================================================================

class DllImportExport GCArrayVar : public GCVar
  {
  public:
    GCArrayVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags) : GCVar(pFirstVar, pVarMap, pName, flags) 
      { m_pClassRefVar = NULL; m_iArrayLen = 0; };
    GCArrayVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags, GCClassVar* pClassRefVar, int ArrayLen);
    virtual ~GCArrayVar();
    GCClassVar*    m_pClassRefVar; // pointer to reference class for the array
    int            m_iArrayLen;    // array length
    CArray<GCVar*, GCVar*> m_pVars;// array of vars
    
    char           WhatAmI() { return VarArray; };                               
    pchar          GetStrVal(pchar Buff) { sprintf(Buff, ""); return Buff; };

    double         getD() { return 0.0; }; 
    long           getL() { return 0; }; 
    unsigned char  getC() { return 0; }; 
    flag           getB() { return False; }; 

    void           set(double d) {}; 
    void           set(long l) {}; 
    void           set(char c) {};
    void           setC(unsigned char c) {};
    void           set(flag b) {}; 
  };

//===========================================================================

class DllImportExport GCArrayWatch
  {
  friend class GCInsMngr;
  protected:
    GCArrayWatch(pGCClassVar pClassVar, bool ReadOnly, int index0, int index1 = -1); 
  public:
    virtual ~GCArrayWatch();
    GCClassVar*    m_pClassVar; //points to class instance
    int            m_iIndex0;   //array index
    int            m_iIndex1;   //second index for matrix
    bool           m_bReadOnly; //read only
    Strng          m_sTg;       //the var tag
    //CnvAttribute*  m_pCnvInfo;    //points to conversion info, NULL if none

    double         getD(); 
    void           set(double d);
  };

//============================================================================
//============================================================================

#define START_CODE(Name)                                    \
class GC##Name##Ins : public GCIns                          \
  {                                                         \
  public:                                                   \
    virtual pGCDef Code();                                  \
    virtual void   Advance(GCInsMngr &IB)                   \
      { IB.m_pIPtr=(pGCIns)(((char*)this)+sizeof(*this)); } \
    virtual void   Exec(GCInsMngr &IB);                     

    // User Methods & Members

#define MID_CODE(Name,Symbol,defFlgs,Precedence)            \
  };                                                        \
                                                            \
class GC##Name##Def : public GCDef                          \
  {                                                         \
  public:                                                   \
     GC##Name##Def() : GCDef(Symbol)                        \
       {m_defFlags = defFlgs; m_prec = Precedence;};                             

    // User Methods & Members

#define END_CODE(Name)                                      \
   };                                                       \
                                                            \
GC##Name##Def GC##Name;                                     \
pGCDef GC##Name##Ins::Code() { return &GC##Name; };

//===========================================================================

#define VAR_CODE(Name)                                        \
class GC##Name##Def : public GCDef                            \
  {                                                           \
  public:                                                     \
    GC##Name##Def() : GCDef(#Name)                            \
      {m_defFlags = DefLHS | DefDataType;  };                 \
    virtual pGCIns Construct(GCInsMngr &IB)                   \
      { if (IB.m_pH->m_pFunctVar)                             \
          IB.DoConstructVars(*this, IB.m_pH->m_pFunctVar->m_pVarList, &IB.m_pH->m_pFunctVar->m_VarMap);  \
        else                                                  \
          IB.DoConstructVars(*this, IB.m_pVarList, &IB.m_VarMap);           \
        return NULL; };                                       \
    virtual GCVar* ConstructVar(GCVar* &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags)    \
      { return new GC##Name##Var(pFirstVar, pVarMap, pName, flags); }  \
   };                                                         \
                                                              \
extern GC##Name##Def GC##Name;

//===========================================================================

#define CMP_CODE(Name,Symbol,CPPSymbol,Precedence)          \
START_CODE(Name)                                            \
MID_CODE(Name,Symbol,DefExp,Precedence)                     \
  virtual pGCIns   Construct(GCInsMngr &IB)                 \
    { return new(IB) GC##Name##Ins; };                      \
END_CODE(Name)                                              \
                                                            \
void GC##Name##Ins::Exec(GCInsMngr &IB)                     \
  {                                                         \
  --IB.m_iCalcPos;                                          \
  ASSERT(IB.m_iCalcPos >= 0);                               \
  if (IB.m_dCalcStack[IB.m_iCalcPos] CPPSymbol IB.m_dCalcStack[IB.m_iCalcPos+1]) \
    IB.m_dCalcStack[IB.m_iCalcPos] = dGCtrue;               \
  else                                                      \
    IB.m_dCalcStack[IB.m_iCalcPos] = dGCfalse;              \
  Advance(IB);                                              \
  };

//===========================================================================

VAR_CODE(Double);
VAR_CODE(Long);
VAR_CODE(Byte);
VAR_CODE(Bit);
VAR_CODE(Str);

//===========================================================================
//
//
//
//===========================================================================

#pragma pack(push, 1)  
struct GCFCBHead
  {
  char Id;
  word nTotalLen;
  };

class GCFCBBuff
  {
  public:
    GCFCBHead Hd;
    char Buff[1024];

    inline char* Name() { return Buff; };
    inline flag  NameIs(char* Nm) { return (_stricmp(Buff, Nm)==0); };
    inline void* Data() { return &Buff[strlen(Buff)+1]; };
    inline flag  HasName() { return (Hd.nTotalLen-sizeof(Hd)>0); };

    inline double D() { return *((double*)&Buff[strlen(Buff)+1]); };
    inline long   L() { return *((long*)&Buff[strlen(Buff)+1]); };
    inline int    I() { return *((int*)&Buff[strlen(Buff)+1]); };
    inline unsigned char C() { return *((unsigned char *)&Buff[strlen(Buff)+1]); };
    inline flag   B() { return *((flag*)&Buff[strlen(Buff)+1]); };
    inline char*  S() { return &Buff[strlen(Buff)+1]; };

    inline flag Try(char *Nm, double &D) { if (!NameIs(Nm)) return False; D=*((double*)&Buff[strlen(Buff)+1]); return True; };
    inline flag Try(char *Nm, CTimeValue &D) 
      { 
      __debugbreak();
      if (!NameIs(Nm)) return False; D=*((double*)&Buff[strlen(Buff)+1]); return True; 
      };
    inline flag Try(char *Nm, long   &L) { if (!NameIs(Nm)) return False; L=*((long*)&Buff[strlen(Buff)+1]); return True; };
    inline flag Try(char *Nm, int    &I) { if (!NameIs(Nm)) return False; I=*((int*)&Buff[strlen(Buff)+1]); return True; };
    inline flag Try(char *Nm, flag   &B) { if (!NameIs(Nm)) return False; B=*((flag*)&Buff[strlen(Buff)+1]); return True; };
    inline flag Try(char *Nm, Strng  &S) { if (!NameIs(Nm)) return False; S=&Buff[strlen(Buff)+1]; return True; };
  };

#pragma pack(pop)  

extern void GCFCBAppendRec(FilingControlBlock &FCB,
                      char Id, char * Name, PVOID pData, dword DataSize);

extern void GCFCBAppendValue(FilingControlBlock &FCB, GCVar* pVar, char * Name);

extern void GCFCBReadBuff(FilingControlBlock &FCB, GCFCBBuff & Buff);

//===========================================================================
//
//
//
//===========================================================================

class GCArray : public GCClassVar
  {
  public:
    GCArray(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, void* pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB/*FILE* f*/, void* pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB/*rCTokenFile f*/, void* pSubClass);
    virtual void OnRestore(void* pOldSubClass, void* pNewSubClass);
  };

//---------------------------------------------------------------------------

class GCMatrix : public GCClassVar
  {
  public:
    GCMatrix(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, void* pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB/*FILE* f*/, void* pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB/*rCTokenFile f*/, void* pSubClass);
    virtual void OnRestore(void* pOldSubClass, void* pNewSubClass);
  };

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif
