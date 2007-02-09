//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef  __GPFUNCS_H
#define  __GPFUNCS_H

#include "sc_defs.h"
#include "..\..\..\SMDK\Include\ScdIF.h"
#include "..\..\..\SMDK\include\md_share1.h"
#include "..\..\..\SMDK\include\md_share3.h"
#include ".\registry.h"


#ifdef __GPFUNCS_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#include <time.h>
#include <afxmt.h>
#include <afxtempl.h> // "xafxtempl.h"

// ========================================================================

extern bool DllImportExport MakeVersionOK(LPCTSTR FileName, LPCTSTR MakeName, WORD V0, WORD V1, WORD V2, WORD V3, bool DoMsg=true);

// ========================================================================
// ========================================================================
// Define allocation debugging line numbers
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

struct dbgMemDumpBlk
  {
  long lMemRqstNo;
  long lMemRqsts;
  long lMemUsed;
  _CrtMemState ms;
  };

extern DllImportExport dbgMemDumpBlk dbgCurrentMemDumpBlk;

DllImportExport void dbgGetMemStats(dbgMemDumpBlk & MDP);
DllImportExport void dbgDumpMemBlks(dbgMemDumpBlk & MDP, char * Tag);
DllImportExport int  dbgAllocHook(int allocType, void *userData, size_t size, int blockType,
                     long requestNumber, const unsigned char *filename, int lineNumber);

class DllImportExport DbgMemHelper
  {
  public:
    DbgMemHelper(bool BreakAtFirstAlloc = true, int TestSize = -1); //-1=no testing; 0=debug all; x=test specified size
    bool AllocTest(int Size, void* p, char* pMsg);
    bool DumpTest(int Size, void* p, char* pMsg);

    bool bBreakAtFirstAlloc;
    long iCnt;
    long iStopAtCnt;
    long iStopAtRequest;
    int  iTestSize;
  };
#endif

#define BreakPoint() { int xxx=0; }

// Code is kept in GPFUNCS
DllImportExport void DumpOurHeap();
DllImportExport void DumpOurHeapChanges();

#define DO_NDEBUG_ASSERT  0
#if DO_NDEBUG_ASSERT
  #ifndef _DEBUG
  #undef ASSERT
  #define ASSERT(a) { if (!(a)) { DoAssert1(); DoAssert2(); }; };
  #endif
#endif


// ========================================================================
//
// Specials
//
// ========================================================================

#define WithOEP        0

#if WithOEP
DllImportExport int OEPOptions();
DllImportExport void SetOEPOptions(int On);
#endif

#define WithQAL        01

#if WithQAL
DllImportExport int QALOptions();
DllImportExport void SetQALOptions(int On);
#endif

#define WithMG        01

#if WithMG
DllImportExport int MGOptions();
DllImportExport void SetMGOptions(int On);
#endif

//===========================================================================
//
//
//
//===========================================================================

const byte AF_PrjFiles        = 0x01;
const byte AF_CfgFiles        = 0x02;
//const byte AF_CfgHome         = 0x04;
const byte AF_BaseCfgFiles    = 0x08;
const byte AF_ProgFiles       = 0x10;
const byte AF_BackupFiles     = 0x20;
const byte AF_RmtCpyFiles     = 0x40;

const byte AF_Elsewhere       = 0x80;
const byte AF_All             = (AF_PrjFiles|AF_CfgFiles|AF_Elsewhere);

const byte AF_First           = AF_PrjFiles;
const byte AF_Last            = AF_ProgFiles;

inline byte AF_Mask(byte i) { return 0x1 << i; }
inline byte AF_Index(byte m) { byte i=0; while ((m&0x1)==0){ i++; m=m>>1;}; return i;};

DllImportExport char * AF_FilesMasked(byte m);
DllImportExport char * AF_FilesIndexed(int i);
DllImportExport char * AF_AliasIndexed(int i);

//===========================================================================
//
//                                 Spares
//
//===========================================================================

#if WITHDEBUGNEW

#ifndef _DEBUG
#pragma message(DEBUG_NEW NOT ALLOWED)
#endif
//#define new DEBUG_NEW
#define HEAP_SPARES 0
#define HEAP_STATS  0

#else

#ifdef _DEBUG
#define HEAP_SPARES 0
#define HEAP_STATS  0
#else
#define HEAP_SPARES 01
#define HEAP_STATS  01
#endif
#endif


//===========================================================================

#if HEAP_SPARES

class DllImportExport SparesList : public CList <void*, void*>
  {
  public:
    // List
    static SparesList * pFirstList;
    SparesList * pNextList;
    char *pClassName;

    // Safety
    CRITICAL_SECTION CritSect;
    flag ListOK;

    DWORD dwMaxCount;

    // for Sanity Checks & stats
    DWORD dwSize;

#if HEAP_STATS
    // Statistics
    DWORD dwAllocsList;
    DWORD dwAllocs;
    DWORD dwFreesList;
    DWORD dwFrees;
    DWORD dwLastUsed;

    DWORD dwLastCount;
#endif
#if defined (_DEBUG)
    char * pTstBlk;
#endif
    //
    SparesList(char * ClassName, dword MaxCount, dword Size);
    ~SparesList();

    void * DoNew(size_t size);
    void * DoNew(size_t size, char * File, int Line);
    void DoDelete(void *p);

    static void DumpUsage(char *Where, flag ForceIt);
  };

#define DEFINE_SPARES(Obj)                                  \
  public:                                                   \
    static SparesList QQSparesLst;                          \
    void * operator new(size_t size)                        \
      { return QQSparesLst.DoNew(size);};                   \
    void * operator new(size_t size, char * File, int Line) \
      { return QQSparesLst.DoNew(size, File, Line);};       \
    void operator delete(void *p)                           \
      { QQSparesLst.DoDelete(p); };                         \
  private:

#define IMPLEMENT_SPARES(Obj, Max)  SparesList Obj::QQSparesLst(#Obj, Max, sizeof(Obj));

#else /* !HEAP_SPARES */

#define DEFINE_SPARES(Obj)
#define IMPLEMENT_SPARES(Obj, Max)
#endif /* HEAP_SPARES */

//===========================================================================

#if HEAP_STATS
#define DEFINE_MEMSTATS(Obj)    DEFINE_SPARES(Obj)
#define IMPLEMENT_MEMSTATS(Obj) IMPLEMENT_SPARES(Obj, 0)
#else
#define DEFINE_MEMSTATS(Obj)
#define IMPLEMENT_MEMSTATS(Obj)
#endif

//===========================================================================
//
//                                 StackSpares
//
//===========================================================================

#if HEAP_SPARES

class StkSparesList;
class StkSparesListList : public CList <StkSparesList*, StkSparesList*> {};

class DllImportExport StkSparesList : public CList <void*, void*>
  {
  private:
    static StkSparesListList ListOfAll;

    CRITICAL_SECTION CritSect;
  public:
    StkSparesList()
      {
      //InitializeCriticalSection(&CritSect);
      InitializeCriticalSectionAndSpinCount(&CritSect, 4000);
      ListOfAll.AddHead(this);
      }
    virtual ~StkSparesList()
      {
      // UnLink it
      POSITION Pos=ListOfAll.Find(this);
      if (Pos)
        ListOfAll.RemoveAt(Pos);
      // Clear the List
      Clear();
      DeleteCriticalSection(&CritSect);
      }
    virtual void Clear() {};
    //static void DumpUsage(char *Where, flag ForceIt);
    void Lock() { EnterCriticalSection(&CritSect); };
    void Unlock() { LeaveCriticalSection(&CritSect); };
    static void ClearAll();
  };

#define DEFINE_STKSPARES(Obj)                                                  \
  class StkSparesList##Obj : public StkSparesList                              \
    {                                                                          \
    public:                                                                    \
      virtual void Clear();                                                    \
    };                                                                         \
  class DllImportExport Stk##Obj                                               \
    {                                                                          \
    static StkSparesList##Obj Lst;                                             \
    private:                                                                   \
      Obj* p;                                                                  \
    public:                                                                    \
      Stk##Obj()                                                               \
        {                                                                      \
        Lst.Lock();                                                            \
        if (Lst.IsEmpty())                                                     \
          {                                                                    \
          p=GetNew();                                                          \
          Lst.Unlock();                                                        \
          }                                                                    \
        else                                                                   \
          {                                                                    \
          p=(Obj*)Lst.RemoveHead();                                            \
          Lst.Unlock();                                                        \
          Reconstruct();                                                       \
          };                                                                   \
        }                                                                      \
      ~Stk##Obj()                                                              \
        {                                                                      \
        Lst.Lock();                                                            \
        Lst.AddHead(p);                                                        \
        Lst.Unlock();                                                          \
        }                                                                      \
      Obj & operator()() { return *p; };                                       \
      Obj * GetNew();                                                          \
      void Reconstruct();                                                      \
      StkSparesList & List() { return Lst; };                                  \
    };

#define IMPLEMENT_STKSPARES(Obj)                                               \
  Obj* Stk##Obj::GetNew() { return new Obj;}                                   \
  StkSparesList##Obj Stk##Obj::Lst;                                            \
  void StkSparesList##Obj::Clear()                                             \
    {                                                                          \
    Lock();                                                                    \
    while (!IsEmpty())                                                         \
      {                                                                        \
      delete (Obj*)RemoveHead();                                               \
      };                                                                       \
    Unlock();                                                                  \
    }

//===========================================================================
#else
//===========================================================================

#define DEFINE_STKSPARES(Obj)                                                  \
  class DllImportExport Stk##Obj                                               \
    {                                                                          \
    protected:                                                                 \
      Obj * p;                                                                 \
    public:                                                                    \
      Stk##Obj() { p=GetNew(); };                                              \
      ~Stk##Obj() { FreeIt(p); };                                              \
      Obj & operator()() { return *p; };                                       \
      Obj * GetNew();                                                          \
      void Reconstruct();                                                      \
      void FreeIt(Obj * p);                                                    \
    };

#define IMPLEMENT_STKSPARES(Obj)             \
  Obj* Stk##Obj::GetNew() { return new Obj;}  \
  void Stk##Obj::FreeIt(Obj *p) { delete p;};

#endif

//===========================================================================
//
//                             Class Factory
//
//===========================================================================

#define DEFINE_CLASSFACTORY0(Cls)                                           \
  class DllImportExport Cls##Factory                                        \
    {                                                                       \
    private:                                                                \
      static Cls##Factory*  pFirstClass;                                    \
      Cls##Factory*  pNextClass;                                            \
      Strng sClassName;                                                     \
      Strng sClassId;                                                       \
    public:                                                                 \
      static Cls##Factory* FindClassName(pchar ClassName_);                 \
      static Cls##Factory* FindClassId(pchar ClassName_);                   \
      static Cls*    Construct(pchar ClassName);                            \
      pchar        ClassName() { return sClassName(); };                    \
      pchar        ClassId() { return sClassId(); };                        \
      Cls##Factory(char * ClsName, char * ClsId);                           \
      virtual ~Cls##Factory();                                              \
      static Cls##Factory*  First () { return pFirstClass;  };              \
      Cls##Factory*  Next  () { return pNextClass;  };                      \
      virtual Cls* Construct() { return NULL; };                            \
      virtual void Destroy(Cls*p) { delete p; };                            \
    };

#define IMPLEMENT_CLASSFACTORY0(Cls)                                        \
  Cls##Factory::Cls##Factory(char * ClsName, char * ClsId)                  \
    {                                                                       \
    pNextClass=pFirstClass;pFirstClass=this;                                \
    sClassName=ClsName;                                                     \
    sClassId=ClsId;                                                         \
    }                                                                       \
  Cls##Factory::~Cls##Factory()                                             \
    {                                                                       \
    if (pFirstClass==this)                                                  \
      pFirstClass=pNextClass;                                               \
    else                                                                    \
      {                                                                     \
      Cls##Factory *p=pFirstClass;                                          \
      while (p->pNextClass!=this)                                           \
        p=p->pNextClass;                                                    \
      if (p)                                                                \
        p->pNextClass=pNextClass;                                           \
      }                                                                     \
    }                                                                       \
  Cls##Factory* Cls##Factory::pFirstClass=NULL;                             \
  /*Cls##Factory* Cls##Factory::FindClassName(pchar ClassName_);*/              \
  Cls##Factory* Cls##Factory::FindClassId(pchar ClassId_)                   \
    { for (Cls##Factory* p=pFirstClass; p && p->sClassId.XStrICmp(ClassId_)!=0; p=p->pNextClass) {}; return p; }; \
  Cls*          Cls##Factory::Construct(pchar ClassId_)                     \
    { Cls##Factory* p=FindClassId(ClassId_); return p ? (Cls*)p->Construct() : NULL; };

#define DEFINE_CLASSBUILDER0(Cls, BaseCls)                                  \
  class DllImportExport Cls##Builder : BaseCls##Factory                     \
    {                                                                       \
    public:                                                                 \
      virtual BaseCls* Construct();                                         \
      virtual void Destroy(Cls*p);                                          \
      Cls##Builder();                                                       \
    };                                                                      \
  extern DllImportExport Cls##Builder Cls##ClassDef;

#define IMPLEMENT_CLASSBUILDER0(Cls, BaseCls, ClsId)                        \
  BaseCls* Cls##Builder::Construct()                                        \
    { return new Cls; };                                                    \
  void Cls##Builder::Destroy(Cls* p)                                        \
    { delete p; };                                                          \
  Cls##Builder::Cls##Builder() : BaseCls##Factory(#Cls, ClsId) {};          \
  Cls##Builder Cls##ClassDef;

//===========================================================================

#define DEFINE_CLASSFACTORY1(Cls, ParmType)                                 \
  class DllImportExport Cls##Factory                                        \
    {                                                                       \
    private:                                                                \
      static Cls##Factory*  pFirstClass;                                    \
      Cls##Factory*  pNextClass;                                            \
      Strng sClassName;                                                     \
      Strng sClassId;                                                       \
    public:                                                                 \
      static Cls##Factory* FindClassName(pchar ClassName_);                 \
      static Cls##Factory* FindClassId(pchar ClassName_);                   \
      static Cls*    Construct(pchar ClassName, ParmType A);                \
      pchar        ClassName() { return sClassName(); };                    \
      pchar        ClassId() { return sClassId(); };                        \
      Cls##Factory(char * ClsName, char * ClsId);                           \
      virtual ~Cls##Factory();                                              \
      static Cls##Factory*  First () { return pFirstClass;  };              \
      Cls##Factory*  Next  () { return pNextClass;  };                      \
      virtual Cls* Construct(ParmType) { return NULL; };                    \
      virtual void Destroy(Cls*p) { delete p; };                            \
    };

#define IMPLEMENT_CLASSFACTORY1(Cls, ParmType)                              \
  Cls##Factory::Cls##Factory(char * ClsName, char * ClsId)                  \
    {                                                                       \
    pNextClass=pFirstClass;pFirstClass=this;                                \
    sClassName=ClsName;                                                     \
    sClassId=ClsId;                                                         \
    }                                                                       \
  Cls##Factory::~Cls##Factory()                                             \
    {                                                                       \
    if (pFirstClass==this)                                                  \
      pFirstClass=pNextClass;                                               \
    else                                                                    \
      {                                                                     \
      Cls##Factory *p=pFirstClass;                                          \
      while (p->pNextClass!=this)                                           \
        p=p->pNextClass;                                                    \
      if (p)                                                                \
        p->pNextClass=pNextClass;                                           \
      }                                                                     \
    }                                                                       \
  Cls##Factory* Cls##Factory::pFirstClass=NULL;                             \
  /*Cls##Factory* Cls##Factory::FindClassName(pchar ClassName_);*/              \
  Cls##Factory* Cls##Factory::FindClassId(pchar ClassId_)                   \
    { for (Cls##Factory* p=pFirstClass; p && p->sClassId.XStrICmp(ClassId_)!=0; p=p->pNextClass) {}; return p; }; \
  Cls*          Cls##Factory::Construct(pchar ClassId_, ParmType A)         \
    { Cls##Factory* p=FindClassId(ClassId_); return p ? (Cls*)p->Construct(A) : NULL; };

#define DEFINE_CLASSBUILDER1(Cls, BaseCls, ParmType)                        \
  class DllImportExport Cls##Builder : public BaseCls##Factory              \
    {                                                                       \
    public:                                                                 \
      virtual BaseCls* Construct(ParmType A);                               \
      virtual void Destroy(Cls*p);                                          \
      Cls##Builder();                                                       \
    };                                                                      \
  extern DllImportExport Cls##Builder Cls##ClassDef;

#define IMPLEMENT_CLASSBUILDER1(Cls, BaseCls, ClsId, ParmType)              \
  BaseCls* Cls##Builder::Construct(ParmType A)                              \
    { return new Cls(this, A); };                                           \
  void Cls##Builder::Destroy(Cls* p)                                        \
    { delete p; };                                                          \
  Cls##Builder::Cls##Builder() : BaseCls##Factory(#Cls, ClsId) {};          \
  Cls##Builder Cls##ClassDef;

// -------------------------------------------------------------------------

DllImportExport void   StrNull(pchar s);
DllImportExport flag   StrEq(pchar s1, pchar s2);
DllImportExport flag   StrEqI(pchar s1, pchar s2);
DllImportExport void   StrCatC(pchar s, char c);
//DllImportExport void   StrLTrim(pchar s, pchar t=" ");
//DllImportExport void   StrRTrim(pchar s, pchar t=" ");
//DllImportExport void   XStrTrim(pchar s, pchar t=" ");
DllImportExport void   XStrLTrim(pchar s, pchar t=" ");
DllImportExport void   XStrRTrim(pchar s, pchar t=" ");
DllImportExport void   XStrTrim(pchar s, pchar t=" ");
DllImportExport void   StrStripMatchedQuotes(pchar s);
//DllImportExport void   StrLPad(pchar s, int l, char what=' ');
//DllImportExport void   StrRPad(pchar s, int l, char what=' ');
//DllImportExport void   StrLRPad(pchar s, int l, char what=' ');
DllImportExport void   XStrLPad(pchar s, int l, char what=' ');
DllImportExport void   XStrRPad(pchar s, int l, char what=' ');
DllImportExport void   XStrLRPad(pchar s, int l, char what=' ');
DllImportExport void   StrReplaceChars(pchar s, pchar t=" ", char what='_');
DllImportExport pchar  StrQuotesTrim(pchar s);
DllImportExport flag   StrHasWildCard(pchar WildCard);
DllImportExport flag   StrMatchWildCard(pchar WildCard, pchar p);
DllImportExport pchar  StrToken(pchar &Current, pchar Terms=" ", pchar WhiteSpace=" ");
DllImportExport double FractionToDouble(char * p);

//===========================================================================
//
//                               Strings
//
//===========================================================================

DllImportExport pchar ftoa(double f);
DllImportExport pchar ltoa(long l);

const short StrngAllocSize   = 8;
const pchar DefaultTerm      = " []{}()";
const short StrngTokenSize   = 256;
#if HEAP_SPARES
const short MaxSpareStrBuffs = 512;
#endif
#if MOVEDTO_MD_SHARE_3
const short MaxCSVCols = 256;
typedef char * CSVColArray[MaxCSVCols];
#endif
typedef char StrngToken[StrngTokenSize];

_FWDDEF(Strng)
/*#C:Strng a string handler class similar to CString without the (const char *) hassles */
class DllImportExport Strng
  {
  friend class Strng_List;
  friend class Strng_Queue;
#if HEAP_SPARES
  friend class StrngEntryExit;
#endif

  public:
    Strng();
    Strng(word InitLen);
    Strng(const char *Init);
    Strng(const char *lpsz1, const char *lpsz2);
    Strng(char Init);
    Strng(pchar Init, unsigned long Size_);
    Strng(const Strng &S);             // copy constructor
    Strng(const Strng &S1, const Strng &S2);
    Strng(const Strng &S1, const char * lpsz2);
    Strng(const char * lpsz1, const Strng &S2);
    ~Strng();

    Strng&   operator=(const Strng &S);  // assignment
    Strng&   operator=(const pchar a);
    Strng&   operator=(const char a) { char Bff[2]; Bff[0] = a; Bff[1] = 0; return operator=(&Bff[0]); };
    Strng&   operator=(const char * a) { return operator=((pchar)a); };
    Strng&   operator+=(const Strng &S);
    Strng&   operator+=(const pchar a);
    Strng&   operator+=(const char a) { char Bff[2]; Bff[0] = a; Bff[1] = 0; return operator+=(&Bff[0]); };
    Strng&   operator+=(const char * a) { return operator+=((pchar)a); };
    /*#F:This is a case sensitive Strng comparison operator.
      #R: 1 if strings are identical.#n 0 if strings differ.  */
    int      operator==(const Strng &String) { return (strcmp(Buff, String.Buff)==0); };
    int      operator==(const pchar String) { return (strcmp(Buff, String)==0); };
    int      operator==(const char * String) { return (strcmp(Buff, (pchar)String)==0); };
    /*#F:This is a case sensitive Strng comparison operator.
      #R: 0 if strings are identical.#n 1 if strings differ.  */
    int      operator!=(const Strng &String) { return (strcmp(Buff, String.Buff)!=0); };
    int      operator!=(const pchar String) { return (strcmp(Buff, String)!=0); };
    int      operator!=(const char * String) { return (strcmp(Buff, (pchar)String)!=0); };
    /*#F:This is character retrieval operator. This can only be used to get characters from a Strng and NOT to set a character's value.
    #R:The character stored at the specified Index*/
    rchar    operator [](int Index) { ASSERT(Index<Size); return Buff[Index]; };
    void     Expand(int l);
    void     SetLength(int l);
    /*#F:Ensures that sufficient memory is allocated for the specified string.*/
    int      ReqdSize(const char *s) const { return ((strlen(s) / StrngAllocSize) + 1)*StrngAllocSize; };
    /*#F:Ensures that sufficient memory is allocated for the specified string length.*/
    int      ReqdSize(int len) const { return ((len / StrngAllocSize) + 1)*StrngAllocSize; };
    /*#F:#R:The contents of the Strng.*/
    pchar    Buffer() const { return Buff; };
    /*#F:#R:True if the Strng is empty (zero length).*/
    flag     IsEmpty() const { return (Buff[0]==0); };
    /*#F:#R:True if the Strng is not empty (length>0).*/
    flag     IsNotEmpty() const { return (Buff[0]!=0); };
    /*#F:#R:The contents of the Strng or NULL if the Strng is empty.*/
    pchar    Str() const { return ((strlen(Buff)>0) ? Buff : NULL); };
    LPCTSTR  StrC() const { return ((strlen(Buff)>0) ? (LPCTSTR)Buff : NULL); };
    /*#F:#R:The contents of the Strng or NULL if the Strng is empty.*/
    pchar    operator()() const { return ((strlen(Buff)>0) ? Buff : NULL);};
    /*#F:#R:The length of the contents of Strng*/
    int      Length() const { return strlen(Buff); };
    /*#F:#R:The length of the contents of Strng*/
    int      GetLength() const { return strlen(Buff); };
    /*#F:#R:The length of the contents of Strng*/
    int      Len() const { return strlen(Buff); };
    pchar    InsertChar(int pos, char c);
    int      Token(pchar T, pchar Term = NULL, int TokLen = 0, pchar WhiteSpace=" ")
               { return Token(T, sizeof(StrngToken), Term, TokLen, WhiteSpace); };
    int      Token(pchar T, int BuffSize, pchar Term = NULL, int TokLen = 0, pchar WhiteSpace=" ");
    flag     Token(pStrng &s, pchar Term= NULL, int TokLen = 0, pchar WhiteSpace=" ");
    flag     Token(Strng &s, pchar Term= NULL, int TokLen = 0, pchar WhiteSpace=" ");
    pchar    NxtTokenAt();
    flag     TokenIs(pchar RqdToken);
    flag     TokenIs(int *TokNo, ...);
    pchar    SkipToken(pchar RqdToken);
    /*#F:#R:The current position of the token-parsing operation within the Strng's buffer.*/
    int      CurrentPosn() const { return (Pos-Buff); };
    /*#F:Expicitly sets the position within the Strng's buffer, where from tokens may be extracted.*/
    void     TokensFrom(int RqdPosn) { Pos = &Buff[RqdPosn]; };
    pchar    Remainder();
    pchar    Set(pchar fmt, ...);
    pchar    Append(pchar fmt, ...);
    pchar    AppendNewLn(pchar fmt, ...);
    /*F:Removes the spaces from the left and right until the leftmost and rightmost character is not in the specified set.*/
    void     Trim(pchar t=" ") { XStrTrim(Buff, t); };
    void     LTrim(pchar t=" ") { XStrLTrim(Buff, t); };
    void     RTrim(pchar t=" ") { XStrRTrim(Buff, t); };
    void     LRTrim(pchar t=" ") { XStrTrim(Buff, t); };
    void     StripMatchedQuotes() { StrStripMatchedQuotes(Buff); };
    void     LPad(int l, char what=' ');
    void     RPad(int l, char what=' ');
    void     LRPad(int l, char what=' ');
    void     ReplaceChars(pchar t=" ", char what='_') { StrReplaceChars(Buff, t, what); };
    /*#F:Converts all characters in Strng to uppercase.*/
    pchar    Upper() { return _strupr(Buff); };
    /*#F:Converts all characters in Strng to lowercase.*/
    pchar    Lower() { return _strlwr(Buff); };
    /*#F:Reverses the order of the characters in a string.
         #R:A pointer to the altered string. */
    pchar    Reverse() { return strrev(Buff); };
    Strng    Mid(int nFirst) const;
    Strng    Mid(int nFirst, int nCount) const;
    Strng    Right(int nCount) const;
    Strng    Left(int nCount) const;
    /*#F:This is a Strng search function.
    #R: The index of the first character in the Strng,not belonging to charSet ( equivalent to length of the substring consisting only of characters in charSet) or Zero if first character in Strng does not occur in charSet. */
    int      XStrSpn(Strng &charSet) { return strspn(Buff, charSet.Buff); };
    int      XStrSpn(pchar charSet) { return strspn(Buff, charSet); };
    int      XStrSpn(const char * charSet) { return strspn(Buff, charSet); };
    /*#F:This is a Strng search function.
    #R: The index of the first character in the Strng, belonging to charSet ( equivalent to length of the substring not consisting of characters in charSet). or Zero if first character in Strng occurs in charSet. */
    int      XStrCSpn(Strng &charSet) { return strcspn(Buff, charSet.Buff); };
    int      XStrCSpn(pchar charSet) { return strcspn(Buff, charSet); };
    int      XStrCSpn(const char * charSet) { return strcspn(Buff,charSet); };

    Strng    SpanIncluding(LPCTSTR lpszCharSet) const;
    Strng    SpanIncluding(Strng &charSet) const { return SpanIncluding((LPCTSTR)charSet.Buff); };
    Strng    SpanIncluding(char* charSet) const { return SpanIncluding((LPCTSTR)charSet); };
    Strng    SpanExcluding(LPCTSTR lpszCharSet) const;
    Strng    SpanExcluding(Strng &charSet) const { return SpanExcluding((LPCTSTR)charSet.Buff); };
    Strng    SpanExcluding(char* charSet) const { return SpanExcluding((LPCTSTR)charSet); };
    int      Find(char Item);
    int      Find(Strng &Item);
    int      Find(pchar Item);
    int      Find(const char * Item) { return Find((pchar)Item); };
    int      FindI(Strng &Item); //case insensitive
    int      FindI(pchar Item); //case insensitive
    int      FindI(const char * Item) { return FindI((pchar)Item); }; //case insensitive
    int      FindFrom(int n, char Item);
    int      FindFrom(int n, char* Item);
    int      FindFrom(int n, const char* Item)  { return FindFrom(n, (pchar)Item); };
    int      ReverseFind(char Item);
    /*#F:This is a Strng search function.
    #R: A pointer to the first occurrence of String in Strng, or a NULL pointer if the search is unsuccessfull.*/
    pchar    XStrStr(Strng & String) { return strstr(Buff, String.Buff); };
    pchar    XStrStr(pchar & String) { return strstr(Buff, String); };
    pchar    XStrStr(const char * String) { return strstr(Buff, String); };
    /*#F:This is a Strng search function.
    #R:A pointer to the first occurrence of c in the Strng, or NULL if c is not found, the NULL terminator is included in the search.*/
    pchar    XStrChr(int c) { return strchr(Buff, c); };
    /*#F:This is a Strng search function.
    #R:A pointer to the last occurrence c in the Strng, or it returns NULL if c is not found, the NULL terminator is included in the search.*/
    pchar    XStrRChr(int c) { return strrchr(Buff, c); };
    flag     CmpLastChar(char c);
    /*#F:This is a case sensitive Strng comparison function.
    #n#n#b<Note:> All string comparison is lexicographic
     (ie according to the order in which words are arranged in a dictionary)
        #R:An integer value indicating the result of the comparison :#n
         value < 0    Strng less than String.#n
         value = 0    Strng identical to String.#n
         value > 0    Strng greater than String.*/
    int      XStrCmp(Strng & String) { return strcmp(Buff, String.Buff); };
    int      XStrCmp(pchar String) { return strcmp(Buff, String); };
    int      XStrCmp(const char * String) { return strcmp(Buff, String); };
    /*#F:XStrICmp case insensitive Strng comparison function.
         #R:An integer value indicating the result of the comparison :#n
         value < 0    Strng less than String.#n
         value = 0    Strng identical to String.#n
         value > 0    Strng greater than String.*/
    int      XStrICmp(Strng & String) { return _stricmp(Buff, String.Buff); };
    int      XStrICmp(pchar String) { return _stricmp(Buff, String); };
    int      XStrICmp(const char * String) { return _stricmp(Buff, String); };
    /*#F:This is a case sensitive Strng comparison function.
        #R:An integer value indicating the result of the comparison :#n
         value < 0    Strng less than String.#n
         value = 0    Strng identical to String.#n
         value > 0    Strng greater than String.*/
    int      XStrNCmp(Strng & String, int number) { return strncmp(Buff, String.Buff, number); };
    int      XStrNCmp(pchar String, int number) { return strncmp(Buff, String,number); };
    int      XStrNCmp(const char * String, int number) { return strncmp(Buff, String,number); };
    /*#F:This is a case insensitive Strng comparison function.
         #R:An integer value indicating the result of the comparison :#n
         value < 0    Strng less than String.#n
         value = 0    Strng identical to String.#n
         value > 0    Strng greater than String.*/
    int      XStrNICmp(Strng & String, int Number) { return _strnicmp(Buff, String.Buff, Number); };
    int      XStrNICmp(pchar String, int Number) { return _strnicmp(Buff, String, Number); };
    int      XStrNICmp(const char * String, int Number) { return _strnicmp(Buff, String, Number); };
    /*#F:This is a Strng search function..
        #R: A pointer to the first occurrence of any character from charSet in Strng, or a NULL pointer if the search is unsuccessfull.*/
    pchar    XStrPBrk(Strng charSet) { return strpbrk(Buff, charSet.Buff); };
    pchar    XStrPBrk(pchar charSet) { return strpbrk(Buff, charSet); };
    pchar    XStrPBrk(const char * charSet) { return strpbrk(Buff, charSet); };
    /*#F:This is a Strng tokenizing function. #N
     A token is a substring of Strng not containing any delimitingChars.
    #R: When Start is True : A pointer to the first token.#n
    When Start is False (default) : A pointer to the next token or a NULL pointer if the search is unsuccessfull.*/
    pchar    StrTok(Strng delimitingChars, flag Start=False) //determines whether the Strng is to be searched for the first time or not.
               { return(Start==True ? strtok(Buff, delimitingChars.Buff) : strtok(NULL, delimitingChars.Buff)); };
    pchar    StrTok(pchar delimitingChars, flag Start=False)
               { return(Start==True ? strtok(Buff, delimitingChars) : strtok(NULL, delimitingChars)); };
    pchar    StrTok(const char * delimitingChars, flag Start=False)
               { return(Start==True ? strtok(Buff, delimitingChars) : strtok(NULL, delimitingChars)); };
    /*#F:#R:Set the zero based of the Strng to the specified integer*/
    void     SetIndex(int i) { iIndex=i; };
    /*#F:#R:Returns the current value of the zero based index of the Strng*/
    int      Index() { return iIndex; };

//pkh#if !_MANAGED
    double   SafeAtoF(double Default=0.0) { return ::SafeAtoF(Buff, Default); };
    long     SafeAtoL(long Default=0) { return ::SafeAtoL(Buff, Default); };
    char*    MakeBinary(DWORD dw, int MinLen, int Grp = -1);
    char*    MakeAlpha(long dw, int MinLen = -1, int Base = 26);
//pkh#endif

  //protected:
    pchar    Buff;//stores the contents of the Strng.
    pchar    Pos; //used in token operations to store the current position within the parsed string.
    int      Size; // stores the size of the current Strng.
    int      iIndex;//stores the current index value of the Strng.
    flag     DoAlloc;// determines whether storage space has been allocated for the Strng.
    pStrng   pNxt; //refers to the next Strng in the list.
    pStrng   pPrv; //refers to the previous Strng in the list.

    DEFINE_SPARES(Strng)
  public:

#if HEAP_SPARES
    static flag SparesOK;
#endif
#ifdef _DEBUG
    static short TestForBigStrngSize;
#endif

    /*#F:#R:Returns the Drive part of the file name*/
    Strng    FnDrive(char* FilePath=NULL) { char Drv[_MAX_DRIVE]; _splitpath(FilePath?FilePath:Str(), Drv, NULL, NULL, NULL); *this = Drv; return *this; };
    /*#F:#R:Returns the directory part of the file name*/
    Strng    FnDir(char* FilePath=NULL) { char Dir[_MAX_DIR]; _splitpath(FilePath?FilePath:Str(), NULL, Dir, NULL, NULL); *this = Dir; return *this; };
    Strng    FnPath(char* FilePath=NULL) { char Dir[_MAX_DIR]; _splitpath(FilePath?FilePath:Str(), NULL, Dir, NULL, NULL); *this = Dir; return *this; };
    /*#F:#R:Returns the filename part of the file name*/
    Strng    FnName(char* FilePath=NULL) { char Name[_MAX_FNAME]; _splitpath(FilePath?FilePath:Str(), NULL, NULL, Name, NULL); *this = Name; return *this; };
    /*#F:#R:Returns the extension part of the file name*/
    Strng    FnExt(char* FilePath=NULL) { char Ext[_MAX_EXT]; _splitpath(FilePath?FilePath:Str(), NULL, NULL, NULL, Ext); *this = Ext; return *this; };
    Strng    FnDrivePath(char* FilePath=NULL);
    Strng    FnNameExt(char* FilePath=NULL);
    Strng    FnDrivePathName(char* FilePath=NULL);
    Strng    FnPathNameExt(char* FilePath=NULL);

    flag     FnCreateTime(FILETIME &Ft);
    flag     FnModifyTime(FILETIME &Ft);
    flag     FnAccessTime(FILETIME &Ft);

    void     FnCheckEndBSlash();
    void     FnClearEndBSlash();
    void     FnCheckExtension(char * E);
    void     FnExpand(char * pFn=NULL);
    void     FnContract(char * pFn=NULL, flag Complete=false);
    void     FnContractDO(char * pFn=NULL);
    void     FnSearchExpand(char* pFn=NULL, byte Places=AF_All, byte PlaceIfMissing=0);
    void     FnSearchContract(char* pFn=NULL, byte Places=AF_All, flag Complete=false);
    flag     FnExpandQueryReload(char * pFn, const FILETIME & Ft);
    void     FnMakeDataFolder(LPTSTR F);

    char*    FnEscapeDotDotPath();
    char*    FnUnEscapeDotDotPath();
    char*    FnRemoveDotDirs();
    char*    FnMakeFileRelative(char * RefPath, flag ExpContract=false);
    char*    FnMakePrjFileRelative(flag ExpContract=false);
    char*    FnMakePathInLocal();

    void     FnCompactPath(long MaxLen);

    void     GetSystemErrorStr(DWORD E);

#if HEAP_SPARES
    static   pchar SpareBuffs[MaxSpareStrBuffs];//An array of unused string buffers.
    static   CRITICAL_SECTION BuffSect;//An array of unused string buffers.
#endif
  public:
#if HEAP_SPARES
    static void StartUp();
    static void ShutDown();
#endif
    pchar    AllocBuff(short l);
    void     FreeBuff(short l, pchar p);

    pStrng   Next() { return pNxt; }; //refers to the next Strng in the list.
    pStrng   Prev() { return pPrv; }; //refers to the previous Strng in the list.

    static int SafeStrlen(LPCTSTR lpsz);
    void Concat(LPCTSTR lpsz1, LPCTSTR lpsz2, LPCTSTR lpsz3=NULL);
    void ConcatCopy(int nSrc1Len, LPCTSTR lpszSrc1Data, int nSrc2Len, LPCTSTR lpszSrc2Data);

  };

DllImportExport Strng operator+(const Strng& string1, const Strng& string2);
DllImportExport Strng operator+(const Strng& string, LPCTSTR lpsz);
DllImportExport Strng operator+(LPCTSTR lpsz, const Strng& string);

// -------------------------------------------------------------------------

#if !_MANAGED

inline double SafeAtoF(Strng & Str, double Default=0.0)
  { return SafeAtoF(Str(), Default); };

inline long SafeAtoL(Strng & Str, long Default=0)
  { return SafeAtoL(Str(), Default); };

#endif

// -------------------------------------------------------------------------

_FWDDEF(Strng_Queue)
/*#C: The class creates a queue or stack of Strngs and provides member functions which allow one to manipulate the Strngs with the Strng queue.*/
class DllImportExport Strng_Queue
  {
  DEFINE_MEMSTATS(Strng_Queue)
  public:
    Strng_Queue();
    ~Strng_Queue();
    int      Get(Strng &b);
    int      Put(Strng &b);
    int      Put (pchar s);
    int      Clear();
/*#F:#R:The number of Strngs in the the Strng Queue.*/
    int      Length()
               { return len;};
/*#F:#R:The size of the string stored by the Head of the Strng Queue.*/
    int      LengthOfHead()
               { return (qHead ? qHead->Length() : 0); };

    int      len;  //the number of Strngs within the queue.
    pStrng   qHead;//refers to the head of the Strng Queue.
    pStrng   qTail;//refers to the tail of the Strng Queue.

  };

// -------------------------------------------------------------------------

_FWDDEF(Strng_List)
/*#C: The class creates a linked list of Strngs and provides member functions which allow one to manipulate Strngs within the Strng List.*/
class DllImportExport Strng_List
  {
  DEFINE_SPARES(Strng_List)
  public:
    Strng_List();
    ~Strng_List();
    Strng_List(Strng_List & L);
    Strng_List &operator=(Strng_List & L);

    Strng*        Insert(Strng & S);
    Strng*        Insert(pStrng p);
    void          Insert(pStrng_List L);
    inline Strng* Insert(char* s) { return Insert(new Strng(s)); };
    inline Strng* Insert(const char* s) { return Insert(new Strng(s)); };
    Strng*        Append(Strng & S);
    Strng*        Append(pStrng p);
    void          Append(pStrng_List L);
    inline Strng* Append(char* s) { return Append(new Strng(s)); };
    inline Strng* Append(const char* s) { return Append(new Strng(s)); };
    void          Clear();
    void          Remove(pStrng p);
    int           Index(pStrng p);                    // Finds the index on a list of a string
    pStrng        AtIndex(int i);                     // Finds the string at the specified index (0 based index counter from start of list)
    pStrng        AtIndexVal(int IndexVal);           // Finds the string of the specified index value (index value is contained in CStrng)
    pStrng        Find(Strng &S, flag CaseSensitive = 0);
    pStrng        Find(const char* s, flag CaseSensitive = 0);
    inline pStrng Find(pStrng p, flag CaseSensitive = 0) { return Find(p->Str(), CaseSensitive); };
    pStrng        FindFrom(pStrng pStart, const char* s, flag CaseSensitive = 0);
    inline pStrng FindFrom(pStrng pStart, pStrng p, flag CaseSensitive = 0) { return FindFrom(pStart, p->Str(), CaseSensitive); };
    int           AppendTokString(char* s, char* tok);    // mhm builds a string list out of tok seperated string
    void          BuildTokString(Strng& s, char* tok);    // mhm builds a token seperated strng from a strng_list
    inline int    Length() { return len; };
    inline pStrng First() { if (Head) Posn = Head->pNxt; return Head; };
    inline pStrng Last() { if (Tail) Posn = Tail->pPrv; return Tail; };
    inline pStrng Next() { pStrng p = Posn; if (Posn) Posn = Posn->pNxt; return p; };
    inline pStrng Prev() { pStrng p = Posn; if (Posn) Posn = Posn->pPrv; return p; };

    int      len; // the number of Strngs within the linked list.
    pStrng   Head;// refers to the Strng at the head of the list.
    pStrng   Tail;// refers to the Strng at the tail of the list.
    pStrng   Posn;// refers to the Strng currently being accessed in the list.
  };

// -------------------------------------------------------------------------
/*#C:This class is used for a vector of Strng classes. ie a variable length array of
strngs that has a number of useful manipulation methods. The internal management
is made as fast as possible at the cost of using more memory. In the debug version
index ranges and valid operations are checked. In the release version invalid
operations will cause fatal errors.#S:CDVector*/
class DllImportExport CSVector
  {
  public:
    CSVector( long NewLen,
              const char * InitVal = "");
    CSVector();
    CSVector(const CSVector &v);
    ~CSVector();
    flag SetSize( long NewLen,
                  const char * InitVal = "");
    flag Swop(long i1, long i2);
    flag Sort(flag Ascending = True);
    flag Reverse();
    flag SetAll(const char * s = "");
    long Find(char* s, flag CaseSensitive = False, long StartIndex = 0);
    flag operator==(const CSVector &v);
    CSVector& operator=(const CSVector &v);
    void dbgDump( pchar Desc = "",
                  flag Horizontal = True);

    //inline methods...
    Strng GetAt(long i) const { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    void SetAt(long i, LPCTSTR s) { ASSERT(i>=0 && i<m_Len); m_d[i] = s; };
    Strng& operator[](long i) { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    Strng operator[](long i) const { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    long Find(const char* s, flag CaseSensitive = False, long StartIndex = 0)
      { return Find((char*)s, CaseSensitive, StartIndex); };
    long GetLen() const { return m_Len; };
    long GetSize() const { return m_Len; };
    void RemoveAll() { SetSize(0); };
    int Add(LPCTSTR s) { SetSize(m_Len+1); m_d[m_Len-1] = s; return m_Len-1; };

  protected:
    void FreePts();
    pStrng m_d;          //Pointer to array of Strngs
    long m_Len;          //Current length of array
  };

const byte LCF_Same       = 0x00;
const byte LCF_RmtChanged = 0x01;
const byte LCF_LclChanged = 0x02;
const byte LCF_RmtMissing = 0x04;
const byte LCF_LclMissing = 0x08;

//---------------------------------------------------------------------------

//DllImportExport pchar ftoa(double f, pchar buff);
//DllImportExport pchar  SecstoDateHMS(double t, pchar buff, flag WithDate=1);
//DllImportExport flag   DateHMStoSecs(pchar buff, double &t);
//DllImportExport pchar  SecstoHMSDate(double t, pchar buff, flag WithDate=1);
//DllImportExport flag   HMSDatetoSecs(pchar buff, double &t);
//DllImportExport pchar  SecstoHMS(double t, pchar buff, flag TruncDate=0, flag TruncMilliSecs=0);
//DllImportExport pchar  SecstoDate(double t, pchar buff);
//DllImportExport flag   HMStoSecs(pchar buff, double &t);
//DllImportExport pchar  SecstoHMSCnv(double time_in, pchar buff, flag TruncDate=0, flag TruncMilliSecs=0);
//DllImportExport pchar  SecstoDHMSCnv(double time_in, pchar buff, flag TruncMilliSecs=0);
DllImportExport bool   SecsToDate1900Var(double Secs1970, VARIANT * pVal);
DllImportExport bool   Date1900VarToSecs(VARIANT Val, double * pSecs1970);

//DllImportExport pchar  SecstoDateHMS(CTimeValue t, pchar buff, flag WithDate=1);
//DllImportExport flag   DateHMStoSecs(pchar buff, CTimeValue &t);
//DllImportExport pchar  SecstoHMSDate(CTimeValue t, pchar buff, flag WithDate=1);
//DllImportExport flag   HMSDatetoSecs(pchar buff, CTimeValue &t);
//DllImportExport pchar  SecstoHMS(CTimeValue t, pchar buff, flag TruncDate=0, flag TruncMilliSecs=0);
//DllImportExport pchar  SecstoDate(CTimeValue t, pchar buff);
//DllImportExport flag   HMStoSecs(pchar buff, CTimeValue &t);
//DllImportExport pchar  SecstoHMSCnv(CTimeValue time_in, pchar buff, flag TruncDate=0, flag TruncMilliSecs=0);
//DllImportExport pchar  SecstoDHMSCnv(CTimeValue time_in, pchar buff, flag TruncMilliSecs=0);
DllImportExport bool   SecsToDate1900Var(CTimeValue Secs1970, VARIANT * pVal);
DllImportExport bool   Date1900VarToSecs(VARIANT Val, CTimeValue * pSecs1970);

DllImportExport char*  CurDateTime(Strng& s);
DllImportExport char*  CurDateTime(CString& s);

DllImportExport char*  TxtEditor(); //default = "notepad.exe"
DllImportExport char*  SetTxtEditor(char* TE);
DllImportExport Strng  TxtEditCmdString(char* Fn);
//---------------------------------------------------------------------------

DllImportExport char*  PrjIniFileName();
DllImportExport char*  PrjAccFmtsIniFileName();
DllImportExport char*  BackupFilesFolderName();
DllImportExport char*  FlwSymFolderName();
DllImportExport char*  FontsFolderName();

DllImportExport char*  BCfgFolderName();
DllImportExport char*  BCfgDBFileName();

DllImportExport char*  DefCfgFolderName();
DllImportExport char*  CfgDBFileName();

DllImportExport flag   FnCreateTime(LPCTSTR pFn, FILETIME &Ft);
DllImportExport flag   FnModifyTime(LPCTSTR pFn, FILETIME &Ft);
DllImportExport flag   FnAccessTime(LPCTSTR pFn, FILETIME &Ft);

DllImportExport char*  FnCheckEndBSlash(Strng & D);
DllImportExport char*  FnClearEndBSlash(Strng & D);
DllImportExport char*  FnCheckExtension(Strng & D, char * E);
DllImportExport LPCTSTR FnCheckExtension(CString & D, LPCTSTR E);
//DllImportExport char*  FnAdjust(Strng & D, char * pFn=NULL);

DllImportExport char*  FnExpand(Strng & D, char * pFn=NULL);
DllImportExport char*  FnContract(Strng & D, char* pFn=NULL, flag Complete=false);
DllImportExport char*  FnContractDO(Strng & D, char* pFn=NULL);
DllImportExport void   FnSetTestLocalFiles(flag On);
DllImportExport bool   FnLocalCompareFiles(LPCTSTR SymFullFileRmt, LPCTSTR FullFileRmt, LPCTSTR FullFileLcl);
DllImportExport long   FnLocalFilesDifferentCount();
DllImportExport void   FnLocalFilesDifferentClear();
DllImportExport Strng_List & FnLocalFilesDifferentList();

DllImportExport char*  FnContractPath(Strng & Fn, char *pFn, char * pPath, char * pAlias, flag Complete);
DllImportExport char*  FnExpandPath(Strng & Fn, char * pFn, char * pPath, char * pAlias);

DllImportExport void   FnSetSearchDlg(byte Where=AF_All);
DllImportExport void   FnSkipSearchDlg(flag DoSkip);
DllImportExport int    FnSearchDlg(Strng &F1, Strng &F2, char * D1=NULL, char* D2=NULL);

DllImportExport char*  FnSearchExpand(Strng & D, char* pFn=NULL, byte Places=AF_All, byte PlaceIfMissing=0);
DllImportExport char*  FnSearchContract(Strng & D, char* pFn=NULL, byte Places=AF_All, flag Complete=false);
DllImportExport flag   FnExpandQueryReload(Strng &D, char * pFn, const FILETIME & Ft);

DllImportExport flag   FnCreatePath(char * pPath, Strng &E);
DllImportExport flag   FnCreateDirectory(char * pPath, flag Compress);

DllImportExport char*  FnMakeDataFolder(LPTSTR F, Strng & D);

DllImportExport char*  FnCompactPath(long MaxLen, Strng & D);

DllImportExport char*  FnEscapeDotDotPath(Strng &D);
DllImportExport char*  FnUnEscapeDotDotPath(Strng &D);
DllImportExport char*  FnRemoveDotDirs(Strng &Fn);
DllImportExport char*  FnMakeFileRelative(char * RefPath, Strng &Fn, flag ExpContract=false);
DllImportExport char*  FnMakePrjFileRelative(Strng &Fn, flag ExpContract=false);
DllImportExport char*  FnMakePathInLocal(Strng & Fn);

DllImportExport char*  SetStartupDirectory(char* PD=NULL);
DllImportExport char*  StartupDirectory();
DllImportExport char*  StartupDrive();

DllImportExport char*  SetProgFiles(char* PD=NULL);
DllImportExport char*  ProgFiles();
DllImportExport char*  SetExeFile(char* PD=NULL);
DllImportExport char*  ExeFile();

DllImportExport Strng FnDefaultControlsFolder();
DllImportExport Strng FnDefaultReactionsFolder();

DllImportExport char*  SetDllFilesPath(char* FP);
DllImportExport char*  ClrDllFilesPath();
DllImportExport char*  AddDllFilesPath(char* FP);
DllImportExport char*  DllFilesPath();

DllImportExport int    DllFilesFolder(int N, char * DllPath, Strng &Folder);
DllImportExport HINSTANCE AFXAPI MyAfxLoadLibrary( LPCTSTR lpszModuleName, char * DllPath);

DllImportExport char*  SetTemporaryFiles(char* PD=NULL);
#if MOVEDTO_MD_SHARE_3
DllImportExport char*  TemporaryFiles();
#endif

DllImportExport char*  SetBaseCfgFiles(char* PD=NULL);
DllImportExport int    TestBaseCfgFiles();
DllImportExport char*  BaseCfgFiles();
DllImportExport char*  BaseCfgFilesAlias();

DllImportExport flag   SetUsePreviousCfgAlias(flag On);
DllImportExport flag   SetUsePreviousPrjAlias(flag On);

DllImportExport char*  SetCfgFile(char* PD=NULL);
#if MOVEDTO_MD_SHARE_3
DllImportExport char*  CfgFile();
#endif
DllImportExport char*  SetCfgFiles(char* PD=NULL);
DllImportExport int    TestCfgFiles();
DllImportExport int    CheckCfgFilesName();
#if MOVEDTO_MD_SHARE_3
DllImportExport char*  CfgFiles();
#endif
DllImportExport char*  CfgFilesAlias();
#if MOVEDTO_MD_SHARE_3
DllImportExport char*  CfgName();
#endif
DllImportExport char*  CfgNameAlias();

DllImportExport char*  SetPrjFiles(char* PD=NULL, char* PF=NULL);
#if MOVEDTO_MD_SHARE_3
DllImportExport char*  PrjFile();
DllImportExport char*  PrjFiles();
#endif
DllImportExport char*  PrjFilesAlias();
DllImportExport char*  SetPrjRmtCpyFiles(char * F);
DllImportExport char*  PrjRmtCpyFiles();
#if MOVEDTO_MD_SHARE_3
DllImportExport char*  PrjName();
DllImportExport char*  PrjNameAlias();
#endif
DllImportExport char*  PrjIniFile();
DllImportExport char*  PrjAccFmtsIniFile();

DllImportExport char*  SetGrfSymbolImportFiles(char* FullPath);
DllImportExport char*  GrfSymbolImportFiles();
DllImportExport char*  GrfSymbolFiles();
DllImportExport char*  GrfFontFiles();
DllImportExport char*  BaseGrfSymbolFiles();
DllImportExport char*  BaseGrfFontFiles();
                                             
#if MOVEDTO_MD_SHARE_3
DllImportExport BOOL   FileExists(LPCTSTR FileName, WIN32_FIND_DATA & fd);
DllImportExport BOOL   FileExists(LPCTSTR FileName);
DllImportExport BOOL   FileAttributes(LPCTSTR FileName, DWORD & Attributes);
DllImportExport BOOL   FileWriteTime(LPCTSTR FileName, FILETIME & FileTime);
#endif
DllImportExport BOOL   CnmVersion();

DllImportExport int    SelectFile(Strng & FilePath, char* Name1, char* Name2, char* Name3);

DllImportExport BOOL   Copy_File(char* pFN, char* pDestDir);
DllImportExport BOOL   Move_FileEx(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, DWORD dwFlags, LPTSTR pMsgBuff=NULL, DWORD MsgBuffLen=0);
DllImportExport BOOL   ClearFolder(LPCTSTR DirName, bool DeleteIt);
DllImportExport BOOL   MoveCfgFile(Strng & From, LPTSTR To);
DllImportExport void   SwingCfgFileIfReqd(Strng & Fn);


DllImportExport BOOL   FindDLLOrHlp(char * pFn, char * DllPath, Strng & Result);

DllImportExport BOOL   CopyTextToClipboard(CWnd* pOwnerWnd, LPCTSTR buff);
DllImportExport BOOL   AddTextToClipboard(CWnd* pOwnerWnd, LPCTSTR buff, LPCTSTR Separator=NULL);

DllImportExport char*  GetSystemErrorStr(Strng &S, DWORD E);

DllImportExport void   StripSingleQuoteComment(char * Buff);
DllImportExport int    ParseTokenList(LPTSTR Buff, char* C[], char* WhiteSpace=" \t", int MaxColumns=MaxCSVCols, char Separator=0, bool Test4Braces=0);
#if MOVEDTO_MD_SHARE_3
DllImportExport int    ParseCSVTokens(LPTSTR Buff, CSVColArray &C, int& Quote, LPCTSTR MoreSepars=NULL, bool TabsAsWhitespace=false);
DllImportExport int    ParseCSVFunction(LPTSTR Buff, CSVColArray &C, int& Quote);
DllImportExport int    ParseCSVTokens(LPTSTR Buff, CSVColArray &C, LPCTSTR MoreSepars=NULL, bool TabsAsWhitespace=false);
DllImportExport int    ParseCSVFunction(LPTSTR Buff, CSVColArray &C);
DllImportExport int    ParseTokens(LPTSTR Buff, CSVColArray &C, char Token);
DllImportExport int    ParseTabTokens(LPTSTR Buff, CSVColArray &C);
#endif
DllImportExport DWORD  FreeVirtualMemory();

// -------------------------------------------------------------------------
// SysCAD Wrapper class - used to ensure that the LOCALE remains "C"
//pkh#if !_MANAGED

class DllImportExport CSCDFileDialog : public CFileDialog
  {
  protected:
    BOOL    m_ForOpen;
  public:
    BOOL    m_DoLCTests;
    BOOL    m_DoingPrjOpen;

    CSCDFileDialog( BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL );
    ~CSCDFileDialog();
    virtual int DoModal( );
    virtual BOOL OnFileNameOK( );
  };

//pkh#endif


BOOL DllImportExport MyPostMessage(CWnd* pWnd, UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

#ifndef _DEBUG
inline BOOL MyPostMessage(CWnd* pWnd, UINT message, WPARAM wParam/*=0*/, LPARAM lParam/*=0*/)
  { return pWnd->PostMessage(message, wParam, lParam); };
#endif

// -------------------------------------------------------------------------

class DllImportExport CSaveRestResHandle
  {
  protected:
    HINSTANCE hInstOld;
  public:
    CSaveRestResHandle(HINSTANCE hNew);
    ~CSaveRestResHandle();
  };

#if MOVEDTO_MD_SHARE_3
// -------------------------------------------------------------------------
/*#C:This class is used to access INI type files. It provides more than the
standard "Set/Get Profile" options.*/
class DllImportExport CProfINIFile : protected CRegistry
  {
  public:
    bool  m_bUseRegistry;
    Strng sFilename;
    Strng sTmpRdStr;

    CProfINIFile(LPCSTR pFilename = "");
    CProfINIFile(LPCSTR pDir, LPCSTR pFilename);
    ~CProfINIFile();
    __int64 RdInt64(LPCTSTR lpszSection, LPCTSTR lpszEntry, __int64 iDefault);
    void WrInt64(LPCTSTR lpszSection, LPCTSTR lpszEntry, __int64 iValue);
    double RdDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dDefault);
    void WrDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dValue);
    long RdLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nDefault);
    void WrLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nValue);
    int RdInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
    void WrInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
    LPCSTR RdStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCSTR pDefault="");
    void WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCSTR pValue);
    DWORD RdSection(LPCTSTR lpszSection, LPSTR pBuff, int BuffLen);
    void WrSection(LPCTSTR lpszSection, LPSTR pBuff);
    DWORD RdSectionNames(LPSTR pBuff, int BuffLen);
    void Flush();
    static void GetProfItem(LPSTR &pBuff, LPSTR &pName, LPSTR &pExp);
    inline void SetUseRegistry(bool On, HKEY hKey, LPCTSTR lpszRegistryKey, LPCTSTR lpszProfile)
      {
      m_bUseRegistry=On;
      if (m_bUseRegistry)
        SetRegistryKey(hKey, lpszRegistryKey, lpszProfile);
      };
    inline void SetUseRegistry(const CProfINIFile & Other)
      {
      m_bUseRegistry=Other.m_bUseRegistry;
      if (m_bUseRegistry)
        SetRegistryKey(Other.m_hKey, Other.m_pszRegistryKey, Other.m_pszProfileName);
      };
    inline void SetProfFilename(LPCSTR pFilename) { sFilename = pFilename; };
    inline char* Filename() { return sFilename(); }
    inline void WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, Strng& sValue) { WrStr(lpszSection, lpszEntry, sValue()); };
    inline void WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, CString& sValue) { WrStr(lpszSection, lpszEntry, (char*)(const char*)sValue); };
    //inline void WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, const char* pValue) { WrStr(lpszSection, lpszEntry, (char*)pValue); };
  };


extern DllImportExport CProfINIFile ScdPFUser;
extern DllImportExport CProfINIFile ScdPFMachine;

#endif
// ==========================================================================
//
//
//
// ==========================================================================

#if !_MANAGED

class DllImportExport CStopWatchItem : public CStopWatch
  {
  public:
    Strng Title;

    CStopWatchItem(char * pTitle);
    ~CStopWatchItem();
  };

class DllImportExport CStopWatchList : public CList<CStopWatchItem*, CStopWatchItem *>
  {
  public:
    static void  Clear();
    static void  Dump();
  };

#define STATICSW(x) static CStopWatchItem SSWItm##x(#x);
#define STATICSWSTART(x) SSWItm##x.Start();
#define STATICSWSTOP(x) SSWItm##x.Stop();

#endif

// ==========================================================================
//
//
//
// ==========================================================================

DllImportExport BOOL ActivateApp(DWORD dwProcessId);
DllImportExport void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName);

//===========================================================================

DllImportExport void SetPrjFileVerNo(int PrjFileVerNo);
DllImportExport int PrjFileVerNo();
DllImportExport void SetNewPrjFileVerNo(int NewPrjFileVerNo);
DllImportExport int NewPrjFileVerNo();
DllImportExport void SetOrigPrjFileVerNo(int OrigPrjFileVerNo);
DllImportExport int OrigPrjFileVerNo();
DllImportExport void SetPrjFileVerNoAsLoaded(int PrjFileVerNo);
DllImportExport int PrjFileVerNoAsLoaded();
DllImportExport void SetPrjFileFormat(int PrjFileFormat);
DllImportExport int PrjFileFormat();
DllImportExport void SetSCDFileFormat(int SCDFileFormat);
DllImportExport int SCDFileFormat();

DllImportExport flag SetUsingPrjLclFiles(flag On);
DllImportExport flag UsingPrjLclFiles();

DllImportExport flag SetSymbolicPaths(flag On);
DllImportExport flag SymbolicPaths();

//===========================================================================
extern DllImportExport BOOL IsWinNT; //set to false if not WindowsNT

DllImportExport char* CopyrightNotice(); //"Copyright © 1995-96"
DllImportExport char* FullCopyrightNotice(); //"Copyright © Kenwalt 1995-96"
DllImportExport char* Company(); //"Kenwalt"
DllImportExport char* FullCompany(); //"Kenwalt Pty Ltd"
DllImportExport char* Company2(); //"KWA Kenwalt"
DllImportExport char* FullCompany2(); //"KWA Kenwalt Australia Pty Ltd"
DllImportExport char* BuildDate(); //"1 December 1998"

extern DllImportExport Strng MajorVersionNo;       //eg "8.0"
extern DllImportExport Strng VersionNo;            //eg "8.0 (Build 35)"
extern DllImportExport Strng FullMajorVersion;     //eg "SysCAD 8.0" or "SysCAD 8.0 Release Debug"
extern DllImportExport Strng FullVersion;          //eg "SysCAD 8.0 (Build 35)" or "SysCAD (Build 35) Full Debug"
extern DllImportExport Strng FullVersionAndUpdate; //eg "SysCAD 8.2 (Build 57)   Update 2  (1 September 2001)"
extern DllImportExport Strng UpdateString;         //eg "Update 2  (1 September 2001)"

DllImportExport void InitVersionStuff();

//===========================================================================

DllImportExport flag SCDLibMakeCompatible(char * MakeName);
DllImportExport flag SCDLibVersionCompatible(word V0, word V1, word V2, word V3);


//inline BOOL MakeVersionOK(char * FileName, char * MakeName, word V0, word V1, word V2, word V3, flag DoMsg=True)
//  {
//  if (!SCDLibMakeCompatible(MakeName))
//    {
//    if (DoMsg)
//      {
//      char buff[256];
//      sprintf(buff, "%s Terminating - Different Build Types !", FileName);
//      ::MessageBox(NULL, buff, "SysCAD", MB_OK|MB_ICONEXCLAMATION|MB_SETFOREGROUND|MB_TASKMODAL);
//      strcat(buff, "\n");
//      TRACE(buff);
//      }
//    return False;
//    }
//  if (!SCDLibVersionCompatible(V0, V1, V2, V3))
//    {
//    if (DoMsg)
//      {
//      char buff[256];
//      sprintf(buff, "%s Terminating - Incompatible Version !", FileName);
//      ::MessageBox(NULL, buff, "SysCAD", MB_OK|MB_ICONEXCLAMATION|MB_SETFOREGROUND|MB_TASKMODAL);
//      strcat(buff, "\n");
//      TRACE(buff);
//      }
//    return False;
//    }
//  return True;
//  };

//===========================================================================
//
//
//
//===========================================================================

inline flag  Strng::FnCreateTime(FILETIME &Ft)    { return ::FnCreateTime(Str(), Ft); };
inline flag  Strng::FnModifyTime(FILETIME &Ft)    { return ::FnModifyTime(Str(), Ft); };
inline flag  Strng::FnAccessTime(FILETIME &Ft)    { return ::FnAccessTime(Str(), Ft); };

inline void  Strng::FnCheckEndBSlash()            { ::FnCheckEndBSlash(*this);    };
inline void  Strng::FnClearEndBSlash()            { ::FnClearEndBSlash(*this);    };
inline void  Strng::FnCheckExtension(char * E)    { ::FnCheckExtension(*this, E); };
inline void  Strng::FnContract(char * pFn, flag Complete) { ::FnContract(*this, pFn, Complete); };
inline void  Strng::FnContractDO(char * pFn)      { ::FnContractDO(*this, pFn); };
inline void  Strng::FnExpand(char * pFn)  { ::FnExpand(*this, pFn);       };
inline void  Strng::FnSearchExpand(char* pFn, byte Places, byte PlaceIfMissing) { ::FnSearchExpand(*this, pFn, Places, PlaceIfMissing); };
inline void  Strng::FnSearchContract(char* pFn, byte Places, flag Complete) { ::FnSearchContract(*this, pFn, Places, Complete); };
inline flag  Strng::FnExpandQueryReload(char * pFn, const FILETIME & Ft) { return ::FnExpandQueryReload(*this, pFn, Ft); };

inline void  Strng::FnMakeDataFolder(LPTSTR F)    { ::FnMakeDataFolder(F, *this) ;};
inline char* Strng::FnEscapeDotDotPath()          { return ::FnEscapeDotDotPath(*this) ;};
inline char* Strng::FnUnEscapeDotDotPath()        { return ::FnUnEscapeDotDotPath(*this) ;};
inline char* Strng::FnRemoveDotDirs()             { return ::FnRemoveDotDirs(*this) ;};
inline char* Strng::FnMakeFileRelative(char * RefPath, flag ExpContract) { return ::FnMakeFileRelative(RefPath, *this, ExpContract); };
inline char* Strng::FnMakePrjFileRelative(flag ExpContract) { return ::FnMakePrjFileRelative(*this, ExpContract) ;};

inline char* Strng::FnMakePathInLocal()           { return ::FnMakePathInLocal(*this); };

inline void  Strng::FnCompactPath(long MaxLen)    { ::FnCompactPath(MaxLen, *this);};

inline void  Strng::GetSystemErrorStr(DWORD E)    { ::GetSystemErrorStr(*this, E); };

//===========================================================================
//
//
//
//===========================================================================

class CAutoIncDec
  {
  public:
    CAutoIncDec(int & Flag) : m_Flag(Flag) { m_Flag++; };
    ~CAutoIncDec()                         { m_Flag--; };
  protected:
    int & m_Flag;
  };

//===========================================================================
//
//
//
//===========================================================================

enum UGC_Ops { UGC_Delete, UGC_Copy, UGC_Move, UGC_MoveOld, UGC_CDir, UGC_CDirComp };
enum UGC_Outcome { UGC_Success, UGC_Fail };

class DllImportExport CUpGrdCompletionJob
  {
  public:
    UGC_Ops  m_What;
    Strng    m_Src;
    Strng    m_Dst;
  };
typedef  CArray<CUpGrdCompletionJob, CUpGrdCompletionJob&> UGC_JobList;

class DllImportExport CUpGrdCompletion
  {
  protected:
    Strng m_Old;
    UGC_JobList m_SuccessJobs;
    UGC_JobList m_FailJobs;

  public:
    CUpGrdCompletion(char * Old);
    int Add(UGC_Outcome Outcome, UGC_Ops  What, char*Src, char*Dst=NULL);
    int Complete(flag OK);
  };

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CScdCmdParms
  {
  public:
    CString   m_sParms[3];
    BOOL      m_bParms[3];

  };

//===========================================================================
//
// Machine Info
//
// ========================================================================

DllImportExport CString GetMacAddressStr(LPCTSTR sNetBiosName);
DllImportExport INT64 GetMacAddress(LPCTSTR sNetBiosName);
DllImportExport DWORD GetDiskSerialNumber(LPCTSTR FileName);

//===========================================================================
//
//
//
// ========================================================================
// AfxGetMainWnd() replacement

DllImportExport void SetScdMainWnd(CWnd * p);
DllImportExport CWnd * ScdMainWnd();


//===========================================================================
//
//
//
//===========================================================================

// Lock to be used on Startup & shutdown
extern CCriticalSection StartupLock;

//===========================================================================
//
//
//
//===========================================================================
// clear this to prevent libs from being freed
extern DllImportExport long lDoFreeLibraries;

#undef DllImportExport

#endif
