//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include <winioctl.h>

#define  __GPFUNCS_CPP

#include "sc_defs.h"
#include "scdver.h"
#include "gpfuncs.h"
#include "scdtempl.h"
#include "errorlog.h"
#include "FnOptDlg.h"
#include "revhist.h"
#include "scdverlic.h"
#include <locale.h>

#define NO_SHLWAPI_STRFCNS
#include "shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")

//#include "optoff.h"

extern AFX_EXTENSION_MODULE SCDLibDLL;

#define dbgStrng   0
#define dbgStrng1  0
#define dbgFnTools 0

// ========================================================================
//
//
//
// ========================================================================

#if WithOEP
static int _OEPOptions=False;
int OEPOptions()            { return _OEPOptions;};
void SetOEPOptions(int On)  { _OEPOptions=On; };
#endif

#if WithQAL
static int _QALOptions=False;
int QALOptions()            { return _QALOptions;};
void SetQALOptions(int On)  { _QALOptions=On; };
#endif

#if WithMG
static int _MGOptions=False;
int MGOptions()            { return _MGOptions;};
void SetMGOptions(int On)  { _MGOptions=On; };
#endif

//===========================================================================
#ifdef _DEBUG

dbgMemDumpBlk dbgCurrentMemDumpBlk = {0,0,0};

// copied from dbgint.h --------------------
#define nNoMansLandSize 4

typedef struct _CrtMemBlockHeader
{
        struct _CrtMemBlockHeader * pBlockHeaderNext;
        struct _CrtMemBlockHeader * pBlockHeaderPrev;
        char *                      szFileName;
        int                         nLine;
        size_t                      nDataSize;
        int                         nBlockUse;
        long                        lRequest;
        unsigned char               gap[nNoMansLandSize];
        /* followed by:
         *  unsigned char           data[nDataSize];
         *  unsigned char           anotherGap[nNoMansLandSize];
         */
} _CrtMemBlockHeader;

#define pbData(pblock) ((unsigned char *)((_CrtMemBlockHeader *)pblock + 1))
#define pHdr(pbData) (((_CrtMemBlockHeader *)pbData)-1)
// -----------------------------------------

void dbgGetMemStats(dbgMemDumpBlk & MDP)
  {
  _CrtMemCheckpoint(&MDP.ms);
//  _CrtMemCheckpoint(&dbgCurrentMemDumpBlk.ms);
//  MDP=dbgCurrentMemDumpBlk;
  }

void dbgDumpMemBlks(dbgMemDumpBlk & MDP, char * Tag)
  {
  dbgpln("");
  dbgpln("--- %7i %9i ------------------------------------------------------ %s",
    dbgCurrentMemDumpBlk.lMemUsed-MDP.lMemUsed, dbgCurrentMemDumpBlk.lMemUsed ,Tag);
  _CrtMemDumpAllObjectsSince(&MDP.ms);

//  _CrtMemState ms;
//  _CrtMemCheckpoint(&ms);
  //struct _CrtMemBlockHeader * p;
//  for (p=MDP.ms.pBlockHeader; p; p=p->pBlockHeaderNext)
  /*
  for (p=MDP.ms.pBlockHeader; p; p=p->pBlockHeaderPrev)
    {
    //if ((p->lRequest>=MDP.lMemRqstNo) && (p->nBlockUse==_NORMAL_BLOCK))
    if ((p->lRequest>=MDP.lMemRqstNo) && (p->nBlockUse!=_FREE_BLOCK))
      {
      dbgpln("    %7i  %2i  %7i %s(%i)",
        //p->nBlockUse,
        p->nDataSize,p->nBlockUse, p->lRequest, p->szFileName, p->nLine);
      }
    }
  */
  /*
  for (p=MDP.ms.pBlockHeader; p->pBlockHeaderNext; p=p->pBlockHeaderNext)
    { };
  while (p)
    {
    //if ((p->lRequest>=MDP.lMemRqstNo) && (p->nBlockUse==_NORMAL_BLOCK))
    if ((p->lRequest>=MDP.lMemRqstNo) && (p->nBlockUse!=_FREE_BLOCK))
      {
      dbgpln("    %7i %7i %s(%i)",
        //p->nBlockUse,
        p->nDataSize, p->lRequest, p->szFileName, p->nLine);
      }
    p=p->pBlockHeaderPrev;
    }
  */
  }


int dbgAllocHook( int allocType, void *userData, size_t size, int blockType,
   long requestNumber, const unsigned char *filename, int lineNumber)
  {
  if ( blockType== _CRT_BLOCK )
     return( TRUE );

  switch (allocType)
    {
    case _HOOK_ALLOC:
      {
      dbgCurrentMemDumpBlk.lMemRqstNo=requestNumber;
      dbgCurrentMemDumpBlk.lMemRqsts++;
      dbgCurrentMemDumpBlk.lMemUsed+=size;

//dbgpln("   +%10i  %10s =%10i", size,"",lMemUsed);
      static size_t lTest=176;
      static size_t lBig=64000;
      if (size>lBig)
        { int xxx=0; }
      else if (size==lTest)
        { int xxx=0; }
      }
      break;
    case _HOOK_REALLOC:
      {
      _CrtMemBlockHeader *p=pHdr(userData);
      dbgCurrentMemDumpBlk.lMemUsed-=p->nDataSize;

      dbgCurrentMemDumpBlk.lMemRqstNo=requestNumber;
      dbgCurrentMemDumpBlk.lMemRqsts++;
      dbgCurrentMemDumpBlk.lMemUsed+=size;
      break;
      }
    case _HOOK_FREE:
      {
      _CrtMemBlockHeader *p=pHdr(userData);
      dbgCurrentMemDumpBlk.lMemUsed-=p->nDataSize;
      break;
      }
    }
  return TRUE;
  };

//===========================================================================

DbgMemHelper::DbgMemHelper(bool BreakAtFirstAlloc/*=true*/, int TestSize/*=-1*/)
  {
  bBreakAtFirstAlloc = BreakAtFirstAlloc;
  iCnt = 0;
  iStopAtCnt = -1;
  iStopAtRequest = -1;
  iTestSize = TestSize;
  };

//---------------------------------------------------------------------------

bool DbgMemHelper::AllocTest(int Size, void* p, char* pMsg)
  {
  if (bBreakAtFirstAlloc)
    {//use debuger to set: iTestSize , iStopAtCnt , iStopAtRequest
    bBreakAtFirstAlloc = false;
    _CrtDbgBreak();
    }

  if (iTestSize==0 || iTestSize==Size)
    {
    iCnt++;
    long requestNumber = 0;
    int b = _CrtIsMemoryBlock((const void *)p, Size, &requestNumber, NULL, NULL);
    if (b)
      {
      _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "?", "%s  Cnt:%d  Request:{%d}  Size:%d  [%p]\n", pMsg ? pMsg : "", iCnt, requestNumber, Size, p);
      if (iCnt==iStopAtCnt)
        {
        _CrtDbgBreak();
        }
      else if (requestNumber==iStopAtRequest)
        {
        _CrtDbgBreak();
        }
      return true;
      }
    }
  return false;
  }

//---------------------------------------------------------------------------

bool DbgMemHelper::DumpTest(int Size, void* p, char* pMsg)
  {
  if (iTestSize==0 || iTestSize==Size)
    {
    long requestNumber = 0;
    int b = _CrtIsMemoryBlock((const void *)p, Size, &requestNumber, NULL, NULL);
    if (b)
      {
      _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "?", "%s  Request:{%d}  Size:%d  [%p]\n", pMsg ? pMsg : "", requestNumber, Size, p);
      if (requestNumber==iStopAtRequest)
        {
        _CrtDbgBreak();
        }
      return true;
      }
    }
  return false;
  }

#endif
//===========================================================================

void DumpOurHeap()
  {
  dbgpln("Heap Dump ===========================================");
  dbgpln("Big Blocks");
  dbgpln(" Address      Size  State");
  _HEAPINFO hinfo;
  int heapstatus;
  hinfo._pentry = NULL;
  const int MAXBLKSIZE=4097;
  int BlkUsed[MAXBLKSIZE];
  int BlkFree[MAXBLKSIZE];
  for (int i=0;i<MAXBLKSIZE; i++)
    {
    BlkUsed[i]=0;
    BlkFree[i]=0;
    }
  while( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK )
    {
    if (hinfo._size<MAXBLKSIZE)
      {
      if ( hinfo._useflag == _USEDENTRY)
        BlkUsed[hinfo._size]++;
      else
        BlkFree[hinfo._size]++;
      }
    else
      dbgpln( "%Fp  %8u  %s",
           hinfo._pentry, hinfo._size,
         ( hinfo._useflag == _USEDENTRY ? "USED" : "    " ));
    }
  dbgpln("------------------------------");
  switch( heapstatus )
    {
    case _HEAPEMPTY:
      dbgpln( "OK - empty heap" );
      break;
    case _HEAPEND:
      dbgpln( "OK - end of heap" );
      break;
    case _HEAPBADPTR:
      dbgpln( "ERROR - bad pointer to heap" );
      break;
    case _HEAPBADBEGIN:
      dbgpln( "ERROR - bad start of heap" );
      break;
    case _HEAPBADNODE:
      dbgpln( "ERROR - bad node in heap" );
      break;
    }
  dbgpln("------------------------------");
  dbgpln("Size    Used     Free");
  for (i=0;i<MAXBLKSIZE; i++)
    if (BlkUsed[i] || BlkFree[i])
      dbgpln("%4i  %6i  %6i", i, BlkUsed[i], BlkFree[i]);
  dbgpln("=====================================================");
  }

//===========================================================================

void DumpOurHeapChanges()
  {
  const int MAXBLKSIZE=0x00010000;
  static int BlkUsed[MAXBLKSIZE];
  static int BlkFree[MAXBLKSIZE];
  static int BlkUsedLast[MAXBLKSIZE];
  static int BlkFreeLast[MAXBLKSIZE];
  static int Init=1;

  dbgpln("Heap Dump Changes ===================================");
  dbgpln("Big Blocks");
  dbgpln(" Address      Size  State");
  _HEAPINFO hinfo;
  int heapstatus;
  hinfo._pentry = NULL;
  if (Init)
    {
    Init=0;
    for (int i=0;i<MAXBLKSIZE; i++)
      {
      BlkUsedLast[i]=0;
      BlkFreeLast[i]=0;
      }
    }

  for (int i=0;i<MAXBLKSIZE; i++)
    {
    BlkUsed[i]=0;
    BlkFree[i]=0;
    }
  while( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK )
    {
    if (hinfo._size<MAXBLKSIZE)
      {
      if ( hinfo._useflag == _USEDENTRY)
        BlkUsed[hinfo._size]++;
      else
        BlkFree[hinfo._size]++;
      }
    else
      dbgpln( "%Fp  %8u  %s",
           hinfo._pentry, hinfo._size,
         ( hinfo._useflag == _USEDENTRY ? "USED" : "    " ));
    }
  dbgpln("------------------------------");
  switch( heapstatus )
    {
    case _HEAPEMPTY:
      dbgpln( "OK - empty heap" );
      break;
    case _HEAPEND:
      dbgpln( "OK - end of heap" );
      break;
    case _HEAPBADPTR:
      dbgpln( "ERROR - bad pointer to heap" );
      break;
    case _HEAPBADBEGIN:
      dbgpln( "ERROR - bad start of heap" );
      break;
    case _HEAPBADNODE:
      dbgpln( "ERROR - bad node in heap" );
      break;
    }
  dbgpln("------------------------------");
  dbgpln("Size    Used     Chg     Free");
  for (i=0;i<MAXBLKSIZE; i++)
    {
    if ((BlkUsed[i] != BlkUsedLast[i]))
      dbgpln("%4i  %6i  %6i", i, BlkUsed[i], BlkUsed[i]-BlkUsedLast[i], BlkFree[i]);
    BlkUsedLast[i] = BlkUsed[i];
    BlkFreeLast[i] = BlkFree[i];
    }
  dbgpln("=====================================================");
  }

//===========================================================================

void DoAssert1(char * pMsg)
  {
  char Buff[4096];
  sprintf(Buff, "ASSERT:%s\n", pMsg);
  OutputDebugString(Buff);   // Win32 API
  AfxMessageBox(pMsg, MB_OK);
  }

//===========================================================================

#if HEAP_SPARES
SparesList * SparesList::pFirstList=NULL;

//===========================================================================
//===========================================================================
//===========================================================================

void SparesList::DumpUsage(char *Where, flag ForceIt)
  {
  #if HEAP_STATS
  flag DoHd=1;
  dbgMemoryState(Where);
  StartupLock.Lock();
  SparesList * p=SparesList::pFirstList;
  while (p && _CrtIsValidPointer(p, 1, TRUE))
    {
    if (ForceIt || p->dwAllocsList || p->dwAllocs || p->dwFreesList || p->dwFrees || p->GetCount())
      {
      dword Used=(p->dwAllocsList+p->dwAllocs)-(p->dwFreesList+p->dwFrees);
      if (ForceIt || Used!=p->dwLastUsed || p->GetCount()!=p->dwLastCount)
        {
        if (DoHd)
          {
          dbgpln("Memory State ------------------------------------------");
          dbgp("%7s","Size");
          dbgp("%12s","Total:Kb");
          dbgp("%12s","Count");
          dbgp("%12s","CountChg");
          dbgp("%12s","InUse/Lost");
          dbgp("%12s","UseChg");
          dbgp("%12s","AllocsList");
          dbgp("%12s","Allocs");
          dbgp("%12s","FreesList");
          dbgp("%12s","Frees");
          dbgp(" %s","What");
          dbgpln("");
          DoHd=0;
          }
        dbgp("%7li",p->dwSize);
        dbgp("%12.2f",(p->GetCount()*p->dwSize)/1024.0);
        dbgp("%12li",p->GetCount());
        dbgp("%12li",((int)p->GetCount())-((int)p->dwLastCount));
        dbgp("%12li",Used);
        dbgp("%12li",((int)Used)-((int)p->dwLastUsed));
        dbgp("%12lu",p->dwAllocsList);
        dbgp("%12lu",p->dwAllocs);
        dbgp("%12lu",p->dwFreesList);
        dbgp("%12lu",p->dwFrees);
        dbgp(" %s",((SparesList*)p)->pClassName);
        dbgpln("");
        p->dwLastUsed=Used;
        p->dwLastCount=p->GetCount();
        }
      }
    p=p->pNextList;
    }
  if (p && !_CrtIsValidPointer(p, 1, TRUE))
    dbgpln("BAD PTR %8x",p);

  if (!DoHd)
    dbgpln("-------------------------------------------------------");
  StartupLock.Unlock();
  #endif
  }

//---------------------------------------------------------------------------

SparesList::SparesList(char * ClassName, dword MaxCount, dword Size)
  {
  pNextList=NULL;
  StartupLock.Lock();
  SparesList *p=pFirstList, *p1=NULL;
  while (p && _stricmp(p->pClassName, ClassName)<0)
    {
    p1=p;
    p=p->pNextList;
    }
  if (p1)
    {
    pNextList=p1->pNextList;
    p1->pNextList=this;
    }
  else
    {
    pNextList=pFirstList;
    pFirstList=this;
    }
  StartupLock.Unlock();

  dwMaxCount=MaxCount;
  dwSize=Size;

  #if HEAP_STATS
  dwAllocsList=0;
  dwAllocs=0;
  dwFreesList=0;
  dwFrees=0;
  dwLastUsed=0;
  dwLastCount=0;
  #endif
  pClassName=NULL;

  pClassName=ClassName;
  InitializeCriticalSectionAndSpinCount(&CritSect, 4000);
  ListOK=1;

  #if defined (_DEBUG)
  #if HEAP_STATS
  pTstBlk=new char[dwSize];
  memset(pTstBlk, 0xDD, dwSize);
  #else
  pTstBlk=NULL;
  #endif
  #endif
  };

//---------------------------------------------------------------------------

SparesList::~SparesList()
  {
  EnterCriticalSection(&CritSect);
  ListOK=0;
  char * p;
  while (GetCount()>0)
    {
    p=(char*)RemoveHead();
    _free_dbg(p, _NORMAL_BLOCK);
    //dwFrees++;
    };

  #if HEAP_STATS
  dbgp("MemStats Lost:%12li", (dwAllocsList+dwAllocs)-(dwFreesList+dwFrees));
  dbgp("   MaxAllocs:%12lu ", dwAllocs);
  dbgp("   TotAllocs:%12lu ", (dwAllocsList+dwAllocs));
  dbgp("  %s", pClassName);
  dbgpln("");
  #endif
  #if defined (_DEBUG)
  delete pTstBlk;
  #endif
  StartupLock.Lock();
  if (pFirstList==this) // Unlink this Object
    pFirstList=pNextList;
  else
    {
    SparesList *p = pFirstList;
    while(p->pNextList!=this)
      p = p->pNextList;
    p->pNextList=pNextList;
    }
  StartupLock.Unlock();
  };

//---------------------------------------------------------------------------

void * SparesList::DoNew(size_t size)
  {
//  ASSERT(size>=sizeof(NextSpareRec *));
  void * p=NULL;
  if (ListOK)
    {
    // Check that Derived class has Spares if base class has
    if (size!=dwSize && dwMaxCount>0)
      {
      // Derived class has NO Spares defined
      DoBreak();
      }

    EnterCriticalSection(&CritSect);
    if (GetCount() > 0)
      {
      #if HEAP_STATS
      dwAllocsList++;
      #endif
      p=RemoveHead();
      #if defined (_DEBUG) && HEAP_STATS
      _ASSERTE(memcmp ((char*)p, pTstBlk, dwSize)==0);
      #endif
      }
    else
      {
      #if HEAP_STATS
      dwAllocs++;
      #endif
      p=_malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__);
      //pClassName=#Obj;
      }
    LeaveCriticalSection(&CritSect);
    return p;
    }
  #if HEAP_STATS
  dwAllocs++;
  #endif
  return _malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__);
  };

//---------------------------------------------------------------------------

void * SparesList::DoNew(size_t size, char * File, int Line)
  {
//  ASSERT(size>=sizeof(NextSpareRec *));
  void * p=NULL;
  if (ListOK)
    {
    // Check that Derived class has Spares if base class has
    if (size!=dwSize && dwMaxCount>0)
      {
      // Derived class has NO Spares defined
      DoBreak();
      }

    EnterCriticalSection(&CritSect);
    if (GetCount() > 0)
      {
      #if HEAP_STATS
      dwAllocsList++;
      #endif
      p=RemoveHead();
      #if defined (_DEBUG) && HEAP_STATS
      _ASSERTE(memcmp ((char*)p, pTstBlk, dwSize)==0);
      #endif
      }
    else
      {
      #if HEAP_STATS
      dwAllocs++;
      #endif
      p=_malloc_dbg(size, _NORMAL_BLOCK, File, Line);
      //pClassName=#Obj;
      }
    LeaveCriticalSection(&CritSect);
    return p;
    }
  #if HEAP_STATS
  dwAllocs++;
  #endif
  return _malloc_dbg(size, _NORMAL_BLOCK, File, Line);
  };

//---------------------------------------------------------------------------

void SparesList::DoDelete(void *p)
  {
  if (ListOK)
    {
    EnterCriticalSection(&CritSect);
//    if (size==dwSize && GetCount() < (int)dwMaxCount)
    if (GetCount() < (int)dwMaxCount)
      {
      #if HEAP_STATS
      dwFreesList++;
      #endif
      AddHead(p);
      #if defined (_DEBUG) && HEAP_STATS
      memset(p, 0xDD, dwSize);
      #endif
      }
    else
      {
      #if HEAP_STATS
      dwFrees++;
      #endif
      char * pc=(char*)p;
      _free_dbg(pc, _NORMAL_BLOCK);
      }
    LeaveCriticalSection(&CritSect);
    return;
    }
  #if HEAP_STATS
  dwFrees++;
  #endif
  char * pc=(char*)p;
  _free_dbg(pc, _NORMAL_BLOCK);
  };

//===========================================================================
//
//
//
//===========================================================================

StkSparesListList StkSparesList::ListOfAll;

//void StkSparesList::DumpUsage(char *Where, flag ForceIt) {};
void StkSparesList::ClearAll()
  {
  POSITION Pos=ListOfAll.GetHeadPosition();
  while (Pos)
    {
    StkSparesList*p=ListOfAll.GetNext(Pos);
    p->Clear();
    };
  }
#endif

//===========================================================================
//
//
//
//===========================================================================

static char lftoainx=0;
static char lftoabuff[10][64];

/*#F:Converts a floating point number into text.*/
pchar ftoa(double f)
  {
  pchar p=lftoabuff[lftoainx];
  lftoainx=(lftoainx+1)%10;
  sprintf(p, "%g", f); return p;
  };

/*#F:Converts a long integer into text.*/
pchar ltoa(long l)
  {
  pchar p=lftoabuff[lftoainx];
  lftoainx=(lftoainx+1)%10;
  sprintf(p, "%li", l); return p;
  };

//===========================================================================

/*#F:#R:A zero length string.*/
void StrNull(pchar s)// the string to be emptied.
  {
  s[0] = NULL;
  }

//---------------------------------------------------------------------------

/*#F:This is a case sensitive string comparison function.
#R:True ( not equal to zero ) if the strings are identical.#n
False (equal to zero ) if the strings differ.*/
flag StrEq(pchar s1, pchar s2)
  {
  return (strcmp(s1,s2)==0);
  }

//---------------------------------------------------------------------------

/*#F:This is a case insensitive string comparison function.
#R:True ( not equal to zero ) if the strings are identical.#n
False (equal to zero ) if the strings differ.*/
flag StrEqI(pchar s1, pchar s2)
  {
  return (strcmpi(s1,s2)==0);
  }

////---------------------------------------------------------------------------
//
///*#F:Removes the specified characters from the left until the leftmost character is not in the specified set.*/
//void StrLTrim(pchar s, // string to be trimmed.
//              pchar t) //Optional - specified character set, default = " ".
//  {
//  ASSERT(s && t);
//  pchar ps = s+strspn(s, t);
//  if (ps != s)
//    memmove(s, ps, strlen(ps)+1);
//  }
//
////---------------------------------------------------------------------------
//
///*#F:Removes the specified characters from the right until the rightmost character is not in the specified set.*/
//void StrRTrim(pchar s, // string to be trimmed.
//              pchar t) //Optional - specified character set, default = " ".
//  {
//  ASSERT(s && t);
//  if (s[0])
//    {
//    pchar pe = s+strlen(s)-1;
//    while (pe-s>=0 && strchr(t, pe[0]))
//      {
//      *pe=0;
//      pe--;
//      }
//    }
//  }
//
////---------------------------------------------------------------------------
//
///*#F:Removes the specified characters from the left and right until the leftmost and rightmost character is not in the specified set.*/
//void StrTrim(pchar s,  // string to be trimmed.
//             pchar t)  //Optional - specified character set, default = " ".
//  {
//  StrLTrim(s, t);
//  StrRTrim(s, t);
//  }
//
//---------------------------------------------------------------------------

/*#F:Removes the specified characters from the left until the leftmost character is not in the specified set.*/
void XStrLTrim(pchar s, // string to be trimmed.
              pchar t) //Optional - specified character set, default = " ".
  {
  ASSERT(s && t);
  pchar ps = s+strspn(s, t);
  if (ps != s)
    memmove(s, ps, strlen(ps)+1);
  }

//---------------------------------------------------------------------------

/*#F:Removes the specified characters from the right until the rightmost character is not in the specified set.*/
void XStrRTrim(pchar s, // string to be trimmed.
              pchar t) //Optional - specified character set, default = " ".
  {
  ASSERT(s && t);
  if (s[0])
    {
    pchar pe = s+strlen(s)-1;
    while (pe-s>=0 && strchr(t, pe[0]))
      {
      *pe=0;
      pe--;
      }
    }
  }

//---------------------------------------------------------------------------

/*#F:Removes the specified characters from the left and right until the leftmost and rightmost character is not in the specified set.*/
void XStrTrim(pchar s,  // string to be trimmed.
             pchar t)  //Optional - specified character set, default = " ".
  {
  XStrLTrim(s, t);
  XStrRTrim(s, t);
  }

//---------------------------------------------------------------------------

/*#F:Removes the matched pairs of quotes.*/
void StrStripMatchedQuotes(pchar s)
  {
  ASSERT(s);
  const int len=strlen(s);
  if ( (len>1) && ((s[0]=='\"' && s[len-1]=='\"') || (s[0]=='\'' && s[len-1]=='\'')) )
    {
    memmove(s, s+1, len-2);
    s[len-2]=0;
    }
  }

//---------------------------------------------------------------------------

/*#F:If the length of the string is less than the specified length, the difference in string
 length, is filled with the specified character to the left of the original characters.*/
void StrLPad(pchar s, // string to be padded.
             int l,   //the required length of the string.
             char what) //Optional - the character used to occupy the additional length, default = ' '.
  {
  ASSERT(s);
  const int i = strlen(s);
  if (l>i)
    {
    memmove(&s[l-i], s, i+1);
    memset(s, what, l-i);
    }
  }

//---------------------------------------------------------------------------

/*#F:If the length of the string is less than the specified length, the difference in string
 length, is filled with the specified character to the right of the original characters.*/
void StrRPad(pchar s, // string to be padded.
             int l,   //the required length of the string.
             char what)//Optional - the character used to occupy the additional length, default = ' '.
  {
  ASSERT(s);
  int i = strlen(s);
  while (i < l)
    {
    s[i++] = what;
    s[i] = 0;
    }
  }

//---------------------------------------------------------------------------

/*#F:If the length of the string is less than the specified length, the difference in string
 length, is filled with the specified character : half to the to the right and half to the left of the original characters.*/
void StrLRPad(pchar s, // string to be padded.
             int l,   //the required length of the string.
             char what)//Optional - the character used to occupy the additional length, default = ' '.
  {
  const int i = strlen(s);
  const int j=i+(l-i)/2;
  StrLPad(s, j, what);
  StrRPad(s, l, what);
  }

//---------------------------------------------------------------------------

/*#F:If the length of the string is less than the specified length, the difference in string
 length, is filled with the specified character to the left of the original characters.*/
void XStrLPad(pchar s, // string to be padded.
             int l,   //the required length of the string.
             char what) //Optional - the character used to occupy the additional length, default = ' '.
  {
  ASSERT(s);
  const int i = strlen(s);
  if (l>i)
    {
    memmove(&s[l-i], s, i+1);
    memset(s, what, l-i);
    }
  }

//---------------------------------------------------------------------------

/*#F:If the length of the string is less than the specified length, the difference in string
 length, is filled with the specified character to the right of the original characters.*/
void XStrRPad(pchar s, // string to be padded.
             int l,   //the required length of the string.
             char what)//Optional - the character used to occupy the additional length, default = ' '.
  {
  ASSERT(s);
  int i = strlen(s);
  while (i < l)
    {
    s[i++] = what;
    s[i] = 0;
    }
  }

//---------------------------------------------------------------------------

/*#F:If the length of the string is less than the specified length, the difference in string
 length, is filled with the specified character : half to the to the right and half to the left of the original characters.*/
void XStrLRPad(pchar s, // string to be padded.
             int l,   //the required length of the string.
             char what)//Optional - the character used to occupy the additional length, default = ' '.
  {
  const int i = strlen(s);
  const int j=i+(l-i)/2;
  StrLPad(s, j, what);
  StrRPad(s, l, what);
  }

//---------------------------------------------------------------------------

/*#F:Replaces any character from the specified set (t) in the string with the
 new specified character (what).*/
void StrReplaceChars(pchar s, //string to be changed
                     pchar t/*=" "*/, //characters to search for
                     char what/*='_'*/) //character to use for replacements
  {
  ASSERT(s && t);
  char* pe = s;
  while (pe[0])
    {
    if (strchr(t, pe[0]))
      pe[0] = what;
    pe++;
    }
  }

//---------------------------------------------------------------------------

/*#F:This is a string concatenation function
#R:The character appended to the end of the string*/
void StrCatC(pchar s, // string to be altered
             char c) //character to be appended
  {
  int i = strlen(s);
  s[i] = c;
  s[i+1] = 0;
  }

//---------------------------------------------------------------------------

/*#F:Removes matching pairs of quotes (single or double).*/
pchar StrQuotesTrim(pchar s) // string to have quotes trimmed.
  {
  int len = strlen(s);
  while (len>1 && ((s[0]=='\'' && s[len-1]=='\'') || (s[0]=='\"' && s[len-1]=='\"')))
    {
    len -= 2;
    memmove(s, &s[1], len);
    s[len] = 0;
    }
  return s;
  }

//---------------------------------------------------------------------------

/*#F:This is a string search function
#R:True if the string contains a wildcard character ie '*'.#n
False if the string does not contain an '*'.*/
flag StrHasWildCard(pchar WildCard)// string to  be searched
  {
  const int l = strcspn(WildCard, "*");
  const int tl = strlen(WildCard);
  return (l < tl);
  }

//---------------------------------------------------------------------------

/*#F:This is a string search function
#R:True if either the first or last character of the string is a wildcard character ie '*'.
#nFalse if neither the first nor last character of the string is a wildcard character ie '*'.*/
flag StrMatchWildCard(pchar WildCard, pchar p)
  {
  const int l = strcspn(WildCard, "*");
  const int tl = strlen(WildCard);
  if (l == tl)
    return (_stricmp(WildCard, p) == 0);
  else
    return (_strnicmp(WildCard, p, l) == 0);
  }

//---------------------------------------------------------------------------

/*#F:#R:The extracted token.*/
pchar StrToken(pchar &Current,// string that is to be searched for tokens
               pchar Terms, // token-delimiting characters.
               pchar WhiteSpace)// characters that are initially skipped, before the string is searched.
  {
  if (Current && *Current != 0)
    {             // 1st char of current != 0
    pchar ps = Current + strspn(Current, WhiteSpace), pe;
    char qchar = (*ps == '\'' ? '\'' : (*ps == '\"' ? '\"' : ' '));

    if (ps && (*ps == qchar))  // is a quoted token
      {
      ps++;
      pchar pe = strchr(ps, qchar);
      while (pe && *(pe+1)==qchar) //  find escaped chars
        {
        pe = strchr(pe+2, qchar);
        }

      if (pe) // if a quote was found
        {
        *pe = 0; // set quote to zero
        Current = pe+1;// set Surrent to start of quoted term
        if ((*Current!=0) && (strchr(Terms, *Current)!=NULL)) // if 1st char of current != delimit term
          Current++;
        Current += strspn(Current, WhiteSpace); // skip whitespace
        }
      else
        Current = NULL; // ps+strlen(ps);
      return ps;
      }
    else
      {
      pe=ps+strcspn(ps, Terms);
      if (*pe == 0)// no more tokens
        Current = NULL;
      else
        {
        *pe=0;
        Current = pe+1;
        Current += strspn(Current, WhiteSpace); // new
        }
      return (strlen(ps) > 0 ? ps : NULL);
      }
    }
  else
    return NULL;
  }

//===========================================================================
#ifdef _DEBUG
short Strng::TestForBigStrngSize = 8192;
#endif

#if HEAP_SPARES
flag Strng::SparesOK = 0;

//---------------------------------------------------------------------------

void Strng::StartUp()
  {
  InitializeCriticalSectionAndSpinCount(&Strng::BuffSect, 4000);
  EnterCriticalSection(&BuffSect);
  for (int i=0; i<MaxSpareStrBuffs; i++)
    SpareBuffs[i]=NULL;
  SparesOK=1; //use debugger to set to 0 if spares must not be used - for memory leaks
  LeaveCriticalSection(&BuffSect);
  //Strng * pS = new Strng("Dealloc Test String"); // ATest String which will not be deallocated?
  }

//---------------------------------------------------------------------------

void Strng::ShutDown()
  {
  if (SparesOK)
    {
    EnterCriticalSection(&BuffSect);
    Strng::SparesOK=0;
    //TRACE("============ Dealloc Test String will be left behind ============\n");
    for (int i=0; i<MaxSpareStrBuffs; i++)
      while (Strng::SpareBuffs[i])
        {
        pchar p=Strng::SpareBuffs[i];
        ppchar pp=(ppchar)p;
        Strng::SpareBuffs[i]=*pp;
        ::delete []p;
        }
    LeaveCriticalSection(&BuffSect);
    DeleteCriticalSection(&Strng::BuffSect);
    }
  }
#endif

//===========================================================================

#define TESTValidAddress  0

#if TESTValidAddress
static DWORD LookForValidAddress=0x0148f7c0;
#endif

/*#F:#R:A Strng with the specified capacity in which to store characters.*/
Strng::Strng() // the string storage capacity of the Strng
  {
  DoAlloc=True;
  pNxt = pPrv = NULL;
  Pos = NULL;
  Buff = AllocBuff(Size = ReqdSize(0)); //new char[Size = ReqdSize(InitLen)];
  strcpy(Buff, "");
  iIndex=0;
  #if dbgStrng1
  dbgpln("Strng::Strng()     [%p] %3i '%s'", Buff, Size, Buff);
  #endif
#if TESTValidAddress
  if (LookForValidAddress)
    if (_CrtIsValidPointer((void*)LookForValidAddress, 1, TRUE))
      {
      DoBreak();
      LookForValidAddress=0;
      }
#endif
  }

//---------------------------------------------------------------------------

/*#F:#R:A Strng with the specified capacity in which to store characters.*/
Strng::Strng(word InitLen) // the string storage capacity of the Strng
  {
  DoAlloc=True;
  pNxt = pPrv = NULL;
  Pos = NULL;
  Buff = AllocBuff(Size = ReqdSize(InitLen)); //new char[Size = ReqdSize(InitLen)];
  strcpy(Buff, "");
  iIndex=0;
  #if dbgStrng1
  dbgpln("Strng::Strng()     [%p] %3i '%s'", Buff, Size, Buff);
  #endif
#if TESTValidAddress
  if (LookForValidAddress)
    if (_CrtIsValidPointer((void*)LookForValidAddress, 1, TRUE))
      {
      DoBreak();
      LookForValidAddress=0;
      }
#endif
  }

//---------------------------------------------------------------------------

/*#F:#R:A Strng containing the specified string.*/
Strng::Strng(const char *Init)  // the string to be stored in the Strng.
  {
  if (Init==NULL) Init="";
  pNxt = pPrv = NULL;
  Pos = NULL;
  iIndex=0;

  DoAlloc=True;
  Buff = AllocBuff(Size = ReqdSize(Init)); //new char[Size = ReqdSize(Init)];
  strcpy(Buff, Init);
  #if dbgStrng1
  dbgpln("Strng::Strng(Init) [%p] %3i '%s'", Buff, Size, Buff);
  #endif
#if TESTValidAddress
  if (LookForValidAddress)
    if (_CrtIsValidPointer((void*)LookForValidAddress, 1, TRUE))
      {
      DoBreak();
      LookForValidAddress=0;
      }
#endif
  }

//---------------------------------------------------------------------------

Strng::Strng(const char *lpsz1, const char *lpsz2)  // the string to be stored in the Strng.
  {
  pNxt = pPrv = NULL;
  Pos = NULL;
  iIndex=0;

  DoAlloc=True;
  Buff = AllocBuff(Size = ReqdSize(0)); //new char[Size = ReqdSize(InitLen)];
  strcpy(Buff, "");

  ConcatCopy(Strng::SafeStrlen(lpsz1), lpsz1,
             Strng::SafeStrlen(lpsz2), lpsz2);

#if TESTValidAddress
  if (LookForValidAddress)
    if (_CrtIsValidPointer((void*)LookForValidAddress, 1, TRUE))
      {
      DoBreak();
      LookForValidAddress=0;
      }
#endif
  }

//---------------------------------------------------------------------------

Strng::Strng(const Strng &S1, const Strng &S2)// the string to be stored in the Strng.
  {
  pNxt = pPrv = NULL;
  Pos = NULL;
  iIndex=0;

  DoAlloc=True;
  Buff = AllocBuff(Size = ReqdSize(0)); //new char[Size = ReqdSize(InitLen)];
  strcpy(Buff, "");

  ConcatCopy(S1.Length(), S1(),
             S2.Length(), S2());

#if TESTValidAddress
  if (LookForValidAddress)
    if (_CrtIsValidPointer((void*)LookForValidAddress, 1, TRUE))
      {
      DoBreak();
      LookForValidAddress=0;
      }
#endif
  }

//---------------------------------------------------------------------------

Strng::Strng(const Strng &S1, const char * lpsz2)  // the string to be stored in the Strng.
  {
  pNxt = pPrv = NULL;
  Pos = NULL;
  iIndex=0;

  DoAlloc=True;
  Buff = AllocBuff(Size = ReqdSize(0)); //new char[Size = ReqdSize(InitLen)];
  strcpy(Buff, "");

  ConcatCopy(S1.Length(), S1(),
             Strng::SafeStrlen(lpsz2), lpsz2);

#if TESTValidAddress
  if (LookForValidAddress)
    if (_CrtIsValidPointer((void*)LookForValidAddress, 1, TRUE))
      {
      DoBreak();
      LookForValidAddress=0;
      }
#endif
  }

//---------------------------------------------------------------------------

Strng::Strng(const char * lpsz1, const Strng &S2)  // the string to be stored in the Strng.
  {
  pNxt = pPrv = NULL;
  Pos = NULL;
  iIndex=0;

  DoAlloc=True;
  Buff = AllocBuff(Size = ReqdSize(0)); //new char[Size = ReqdSize(InitLen)];
  strcpy(Buff, "");

  ConcatCopy(Strng::SafeStrlen(lpsz1), lpsz1,
             S2.Length(), S2());

#if TESTValidAddress
  if (LookForValidAddress)
    if (_CrtIsValidPointer((void*)LookForValidAddress, 1, TRUE))
      {
      DoBreak();
      LookForValidAddress=0;
      }
#endif
  }

//    Strng(const Strng &S1, const Strng &S2);
//    Strng(const Strng &S1, const char * lpsz2);
//    Strng(const char * lpsz1, const Strng &S2);

//---------------------------------------------------------------------------

/*#F:#R:A Strng containing the specified character.*/
Strng::Strng(char Init)  // the character to be stored in the Strng.
  {
  pNxt = pPrv = NULL;
  Pos = NULL;
  iIndex=0;

  DoAlloc=True;
  char Bff[2];
  Bff[0] = Init;
  Bff[1] = 0;

  Buff = AllocBuff(Size = ReqdSize(Bff)); //new char[Size = ReqdSize(Bff)];
  strcpy(Buff, Bff);
  #if dbgStrng1
  dbgpln("Strng::Strng(Init) [%p] %3i '%s'", Buff, Size, Buff);
  #endif
#if TESTValidAddress
  if (LookForValidAddress)
    if (_CrtIsValidPointer((void*)LookForValidAddress, 1, TRUE))
      {
      DoBreak();
      LookForValidAddress=0;
      }
#endif
  }

//---------------------------------------------------------------------------

/*#F:#R:A Strng containing the specified string and with either the specified capacity,
 or, if this is zero, then the capacity is set to the length of the specified string.*/
Strng::Strng(pchar Init,  // the string to be stored in the Strng.
             unsigned long Size_)  // the string storage capacity of the Strng.
  {
  if (Init==NULL) Init="";
  pNxt = pPrv = NULL;
  Pos = NULL;
  iIndex=0;
  ASSERT(Size_>0);

  DoAlloc=False;//(Size_==0);
  Size=(int)Size_;
  Buff=Init;

  #if dbgStrng1
  dbgpln("Strng::Strng(Init) [%p] %3i '%s'", Buff, Size, Buff);
  #endif
#if TESTValidAddress
  if (LookForValidAddress)
    if (_CrtIsValidPointer((void*)LookForValidAddress, 1, TRUE))
      {
      DoBreak();
      LookForValidAddress=0;
      }
#endif
  }

//---------------------------------------------------------------------------

/*#F:This is a copy constructor
#R:A Strng identical to the specified Strng*/
Strng::Strng(const Strng &S) // the Strng to be copied.
  {
  DoAlloc=True;
  pNxt = pPrv = NULL;
  Pos = NULL;
  iIndex=S.iIndex;
  Buff = AllocBuff(Size = ReqdSize(S.Buff)); //new char[Size = ReqdSize(S.Buff)];
  strcpy(Buff, S.Buff);
  #if dbgStrng1
  dbgpln("Strng::Strng(S)    [%p] %3i '%s'", Buff, Size, Buff);
  #endif
#if TESTValidAddress
  if (LookForValidAddress)
    if (_CrtIsValidPointer((void*)LookForValidAddress, 1, TRUE))
      {
      DoBreak();
      LookForValidAddress=0;
      }
#endif
  }

//---------------------------------------------------------------------------

/*#F:This empties the Strng's buffer.*/
Strng::~Strng()
  {
  #if dbgStrng1
  dbgpln("Strng::~Strng(S)   [%p] %3i", Buff, Size);
  #endif
  if (DoAlloc)
    FreeBuff(Size, Buff); //delete[] Buff;
  DoAlloc=False;
  }

//---------------------------------------------------------------------------

/*#F:This is an assignment operator.
  #R:A duplicate copy of the specified string or character.*/
Strng& Strng::operator=(const Strng &S) // Strng to be duplicated.
  {
  if (this != &S)
    {
    if (DoAlloc)
      {
      FreeBuff(Size, Buff); //delete[] Buff;
      Buff = AllocBuff(Size = ReqdSize(S.Buff)); //new char[Size = ReqdSize(S.Buff)];
      }
    else
      {
      ASSERT(Size<=ReqdSize(S.Buff));
      }
    strcpy(Buff, S.Buff);
    iIndex=S.iIndex;
    }
  #if dbgStrng1
  dbgpln("Strng::=(S)        [%p] %3i '%s'", Buff, Size, Buff);
  #endif
  Pos = NULL;
  return *this;
  }

//---------------------------------------------------------------------------

Strng& Strng::operator=(const pchar a)
  {
  pchar p=a;
  if (p==NULL)
    p="";
  const int l = ReqdSize(p);
  Expand(l);
  strcpy(Buff, p);
  #if dbgStrng1
  dbgpln("Strng::=(a)        [%p] %3i '%s'", Buff, Size, Buff);
  #endif
  Pos = NULL;
  return *this;
  }

//---------------------------------------------------------------------------

/*#F:This is a concatenation operator.
#R:The string or character appended to the end of the Strng.*/
Strng& Strng::operator+=(const Strng &S) // Strng to be appended
  {
  *this += S.Buff;
  return *this;
  }

//---------------------------------------------------------------------------

Strng& Strng::operator+=(const pchar a)
  {
  if (a==NULL)
    return *this;
  const int l = ReqdSize(strlen(Buff)+strlen(a));
  Expand(l);
  strcat(Buff, a);
  #if dbgStrng1
  dbgpln("Strng::+=(a)       [%p] %3i '%s'", Buff, Size, Buff);
  #endif
  return *this;
  }

//---------------------------------------------------------------------------

/*#F:Either increases the string storage capacity of the Strng to the specified amount,
 or if this amount is smaller than the current size, then the capacity remains the same.*/
void Strng::Expand(int l) // the required string storage capacity.
  {
  if (l > Size)
    {
    ASSERT(DoAlloc==1);
    ASSERT_ALWAYS(l<32768, "Strng Buffer Overflow"); 
    //Why is string so HUGE!!! AllocBuff fails because of conversion of l>=32768 to short!
    //ASSERT_ALWAYS(l<32768, "HUGE Strng!"); //Why is string so HUGE!!! AllocBuff fails because of conversion of l>=32768 to short!
    pchar tBuff = AllocBuff(l); //new char[l];
    strcpy(tBuff, Buff);
    FreeBuff(Size, Buff); //delete[] Buff;
    Buff = tBuff;
    Size = l;
    }
  }

//---------------------------------------------------------------------------

/*#F:*/
void Strng::SetLength(int l) // the required length
  {
  Expand(ReqdSize(l));
  Buff[l] = 0;
  }

//---------------------------------------------------------------------------

/*#F:Insert specified character at specified position.*/
pchar Strng::InsertChar(int pos, char c)
  {
  ASSERT(pos>=0);
  const int l = strlen(Buff);
  Expand(ReqdSize(l+1));
  if (pos>=l)
    {
    Buff[l] = c;
    }
  else
    {
    for (int i=l; i>pos; i--)
      Buff[i] = Buff[i-1];
    Buff[pos] = c;
    }
  Buff[l+1] = 0;
  return Buff;
  }

//---------------------------------------------------------------------------

/*#F:#R:The length of the extracted token, NULL if no token was located.
#x:Characters between quotes are returned as a token.*/
int Strng::Token(pchar T,     // location where located token can be referenced
                 int BuffSize,// maximum allowable token length
                 pchar Terms, // token delimiting terms ( if not specified, DefaultTerm is used)
                 int TokLen,
                 pchar WhiteSpace)// characters that are initially skipped, before the string is searched.
  {
  if (!Terms)
    Terms = DefaultTerm;
  if (!Pos)
    Pos = Buff;
  pchar tp = (char*)&T[0];
  pchar ps = Pos + strspn(Pos, WhiteSpace);
  if (*ps == 0)
    {
    strcpy(T, "");
    return NULL;
    }

  char qchar = (*ps == '\'' ? '\'' : (*ps == '\"' ? '\"' : ' '));

  if (ps && (*ps == qchar) && (qchar!=' '))  // is a quoted token
    {
    ps++;
    pchar pe = strchr(ps, qchar);
    while (pe && *(pe+1)==qchar) //  find escaped chars
      {
      pe = strchr(pe+2, qchar);
      }

    if (pe)
      {
      TokLen=(pe-ps);
      Pos = ps+TokLen+1;
      }
    else
      {
      TokLen=strlen(ps);
      Pos = ps+TokLen;
      }
    if ((*Pos!=0) && (strchr(Terms, *Pos)!=NULL))
      Pos++;
    }
  else
    {
    if (TokLen == 0)
      {
      TokLen = strcspn(ps, Terms);
      if (TokLen==(int)strlen(ps))
        {
        TokLen = strlen(ps);
        Pos = ps+TokLen;
        }
      else
        {
        Pos = ps+TokLen;
        }
 /*
      pchar TermAt=strpbrk(ps, Terms);
      pchar WhiteAt=strpbrk(ps, WhiteSpace);
      if (TermAt)
        TokLen=TermAt-ps+1;
      else if (WhiteAt)
        TokLen=WhiteAt-ps+1;
      else
        TokLen = strlen(ps);
*/
      }
    Pos = ps+TokLen;
    }
  Pos += strspn(Pos, WhiteSpace);

  TokLen=Min(TokLen, BuffSize-1);
  strncpy(tp, ps, TokLen);
  tp[TokLen] = 0;

  #if dbgStrng
  dbgpln("Strng::Token()     [%p] %s", Buff, tp);
  #endif
  return TokLen;
  }

//---------------------------------------------------------------------------

/*#F:#R:True (1) if a token has been located in the Strng and copied into the .
False (0) if a token was nit located in the Strng.*/
flag Strng::Token(pStrng &s, // Strng pointer which references the Strng into which the located token is copied
                  pchar Terms,// term-delimiting characters
                  int TokLen,
                  pchar WhiteSpace) // characters initially skipped before the search of the Strng occurs.
  {
  char Bff[16384];
  s = NULL;
  if (Token(Bff, sizeof(Bff), Terms, TokLen, WhiteSpace))
    if (strlen(Bff) > 0)
      s = new Strng(Bff);
  return (s!=NULL);
  }

//---------------------------------------------------------------------------

flag Strng::Token(Strng &s, pchar Terms, int TokLen, pchar WhiteSpace)
  {
  char Bff[16384];
  s="";
  if (Token(Bff, sizeof(Bff), Terms, TokLen, WhiteSpace))
    if (strlen(Bff) > 0)
      {
      s=Bff;
      return 1;
      }
  return 0;
  }

//---------------------------------------------------------------------------

/*#F:#R:True (1) if the specified term matches the token,located by a previous call to Strng::Token.
False (0) if the specified term does not match the located token.*/
flag Strng::TokenIs(pchar RqdToken)
  {
  if (!Pos)
    Pos = Buff;

  if (*Pos == 0)
    return NULL;

  #if xdbgStrng
  dbgpln("Strng::TokenIs()   [%p] %s", Buff, RqdToken);
  #endif

  pchar ps = Pos + strspn(Pos, " ");// start searched string after any initial spaces
  if (_strnicmp(ps, RqdToken, strlen(RqdToken)) == 0)
    {
    pchar pe = ps+strlen(RqdToken);
    if ((!isalnum(RqdToken[0])) || (!isalnum(pe[0])))
      {
      StrngToken Tok;
      Token(Tok, sizeof(Tok), NULL, strlen(RqdToken));
      return 1;
      }
    }
  return 0;
  }

//---------------------------------------------------------------------------

/*#F:#R:The position within the currently searched Strng,
 where the next token is to be searched for, excluding any spaces.*/
pchar Strng::NxtTokenAt()
  {
  if (!Pos)
    Pos = Buff;
  return Pos + strspn(Pos, " ");
  }

//---------------------------------------------------------------------------

/*#F:#R:True if one of the function's pchar parameters matches
the token,located by a previous call to Strng::Token. False if the located token is not matched by one of the function's pchar parameters.*/
flag Strng::TokenIs(int *TokNo,//its initialized to zero and incremented, as each pchar parameter is compared to the token. If the function is True, it will correspond to the matching parameter.
                     ...)// unspecified parameters, only the pchar parameters are considered.
  {
  va_list argptr;
  va_start(argptr, TokNo);
  pchar Tst = va_arg(argptr, char*);

  *TokNo = 0;
  while (Tst)
    {
    if (TokenIs(Tst))
      break;
    Tst = va_arg(argptr, char*);
    (*TokNo)++;
    }
  va_end(argptr);

  return (Tst != NULL);
  }

//---------------------------------------------------------------------------

/*#F:#R:The characters remaining in the Strng's buffer, after the current
token-parsing (excluding spaces) or NULL if no characters remain.*/
pchar Strng::Remainder()
  {
  pchar p = Pos + strspn(Pos, " ");
  if (strlen(p) == 0)
    return NULL;
  else
    return p;
  }

//---------------------------------------------------------------------------

/*#F:Copies the various arguments into the Strng.*/
pchar Strng::Set(pchar fmt, // ensures that the format of the copied parameters, is that of the "printf" statements.
               ...)// arguments to be copied into the Strng.
  {
  char b[16384];
  va_list argptr;
  va_start(argptr, fmt);
  VERIFY(vsprintf(b, fmt, argptr) < sizeof(b));
  va_end(argptr);
  *this = b;
  return Str();
  }

//---------------------------------------------------------------------------

/*#F:Appends the various arguments to the end of the Strng's characters.*/
pchar Strng::Append(pchar fmt,// ensures that the format of the copied parameters, is that of the "printf" statements.
                  ...)// arguments to be appended to the Strng.
  {
  char b[16384];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(b, fmt, argptr);
  va_end(argptr);
  *this += b;
  return Str();
  }

//---------------------------------------------------------------------------

/*#F:If the Strng already contains characters, a new line is initally appended, after which the various arguments are appended to the Strng.*/
pchar Strng::AppendNewLn(pchar fmt,// ensures that the format of the copied parameters, is that of the "printf" statements.
                       ...)// arguments to be appended to the Strng.
  {
  char b[16384];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(b, fmt, argptr);
  va_end(argptr);
  if (this->Length()>0)
    *this += "\n";
  *this += b;
  return Str();
  }

//---------------------------------------------------------------------------

/*#F:If the length of the Strng is less than the specified length, the difference in string
 length, is filled with the specified character to the right of the original characters.*/
void Strng::RPad(int l, //the required length of the Strng.
                 char what) //the character used to occupy the additional length.
  {
  Expand(ReqdSize(l));
  StrRPad(Buff, l, what);
  }

//---------------------------------------------------------------------------

/*#F:If the length of the Strng is less than the specified length, the difference in string
 length, is filled with the specified character to the left of the original characters.*/
void Strng::LPad(int l, //the required length of the Strng.
                 char what) //the character used to occupy the additional length.
  {
  Expand(ReqdSize(l));
  StrLPad(Buff, l, what);
  }

//---------------------------------------------------------------------------

/*#F:If the length of the Strng is less than the specified length, the difference in string
 length, is filled with the specified character : half to the to the right and half to the left of the original characters.*/
void Strng::LRPad(int l, //the required length of the Strng.
                  char what) //the character used to occupy the additional length.
  {
  Expand(ReqdSize(l));
  StrLRPad(Buff, l, what);
  }

//---------------------------------------------------------------------------

Strng Strng::Mid(int nFirst) const
  {
  return Mid(nFirst, strlen(Buff));
  }

//---------------------------------------------------------------------------

/*#F:Extracts the first (leftmost) characters from the string,
commencing at the specified character within the string (zero-based index).*/
Strng Strng::Mid(int nFirst, int nCount) const
  {
  const int l = strlen(Buff);
  if (nFirst < 0)
    nFirst = 0;
  if (nCount < 0)
    nCount = 0;
  if (nFirst + nCount > l)
    nCount = l - nFirst;
  if (nFirst > l)
    nCount = 0;

  Strng dest;
  if (nCount>0)
    {
    dest.SetLength(nCount);
    strncpy(dest.Buff, &Buff[nFirst], nCount);
    }
  return dest;
  }

//---------------------------------------------------------------------------

/*#F:Extracts the last (rightmost) characters from the string.*/
Strng Strng::Right(int nCount) const
  {
  const int l = strlen(Buff);
  if (nCount < 0)
    nCount = 0;
  else if (nCount > l)
    nCount = l;

  Strng dest;
  if (nCount>0)
    {
    dest.SetLength(nCount);
    strncpy(dest.Buff, &Buff[l-nCount], nCount);
    }
  return dest;
  }

//---------------------------------------------------------------------------

/*#F:Extracts the first (leftmost) characters from the string.*/
Strng Strng::Left(int nCount) const
  {
  const int l = strlen(Buff);
  if (nCount < 0)
    nCount = 0;
  else if (nCount > l)
    nCount = l;

  Strng dest;
  if (nCount>0)
    {
    dest.SetLength(nCount);
    strncpy(dest.Buff, Buff, nCount);
    }
  return dest;
  }

//---------------------------------------------------------------------------

/*#F:This is a Strng search function.
#R:A Strng of the extracted substring (which only contains characters in charSet),or to an empty string if Strng begins with a character not in charSet.*/
// strspn equivalent
Strng Strng::SpanIncluding(LPCTSTR lpszCharSet) const
  {
  ASSERT(AfxIsValidString(lpszCharSet, FALSE));
  return Left(_tcsspn(Buff, lpszCharSet));
  }

//---------------------------------------------------------------------------

/*#F:This is a Strng search function.
#R:A Strng of the extracted substring(which does not contain characters in charSet) or to an empty string if Strng begins with a character in charSet.*/
// strcspn equivalent
Strng Strng::SpanExcluding(LPCTSTR lpszCharSet) const
  {
  ASSERT(AfxIsValidString(lpszCharSet, FALSE));
  return Left(_tcscspn(Buff, lpszCharSet));
  }

//---------------------------------------------------------------------------

/*#F:This is a Strng search function.
  #R:A zero based index of the first character of Item in the Strng or it returns -1 if Item is not found.*/
int Strng::Find(char item)
  {
  pchar p = (char*)strchr(Buff, item);
  return (p == NULL) ? -1 : (int)(p - Buff);
  }

//---------------------------------------------------------------------------

int Strng::Find(pchar item)
  {
  pchar p = (char*)strstr(Buff, item);
  return (p == NULL) ? -1 : (int)(p - Buff);
  }

//---------------------------------------------------------------------------

int Strng::Find(Strng &item)
  {
  pchar p = (char*)strstr(Buff, item.Buff);
  return (p == NULL) ? -1 : (int)(p - Buff);
  }

//---------------------------------------------------------------------------

int Strng::FindI(pchar item)
  {
  if (item && item[0]!=0)
    {
    const int FindLen = strlen(item);
    const int l = strlen(Buff);
    if (l<FindLen)
      return -1;
    const int MaxSearchPos = l - FindLen;
    for (int j=0; j<(isalpha(item[0]) ? 2 : 1); j++)
      {
      char c = item[0];
      if (j==1)
        c = (isupper(c) ? _tolower(c) :  _toupper(c));
      int pos = 0;
      char* p = strchr(&Buff[pos], c);
      while (p && pos<=MaxSearchPos)
        {
        pos = (int)(p - Buff);
        if (_strnicmp(p, item, FindLen)==0)
          return pos;
        p = strchr(&Buff[++pos], c);
        }
      }
    }
  return -1;
  }

//---------------------------------------------------------------------------

int Strng::FindI(Strng &item)
  {
  return FindI(item.Buff);
  }

//---------------------------------------------------------------------------

int Strng::FindFrom(int n, char item)
  {
  ASSERT(n>=0);
  if (n>=(int)strlen(Buff))
    return -1;
  pchar p = (char*)strchr(&Buff[n], item);
  return (p == NULL) ? -1 : (int)(p - Buff);
  }

//---------------------------------------------------------------------------

int Strng::FindFrom(int n, pchar item)
  {
  ASSERT(n>=0);
  if (n>=(int)strlen(Buff))
    return -1;
  pchar p = (char*)strstr(&Buff[n], item);
  return (p == NULL) ? -1 : (int)(p - Buff);
  }

//---------------------------------------------------------------------------

/*#F:This is a Strng search function.
  #R:A zero based index of the last character of Item in the Strng or it returns -1 if Item is not found.*/
int Strng::ReverseFind(char item)
  {
  pchar p = (char*)strrchr(Buff, item);
  return (p == NULL) ? -1 : (int)(p - Buff);
  }

//---------------------------------------------------------------------------

/*#F:Return true if the last character in the string is equal to the specified character.*/
flag Strng::CmpLastChar(char c)
  {
  return (Buff[0] && Buff[strlen(Buff)-1]==c);
  }

//---------------------------------------------------------------------------

/*#F:#R:Returns the drive AND directory part of the file name*/
Strng Strng::FnDrivePath(pchar FilePath)
  {
  char Drv[_MAX_DRIVE];
  char Dir[_MAX_DIR];
  _splitpath(FilePath?FilePath:Str()?Str():"", Drv, Dir, NULL, NULL);
  Set("%s%s", Drv, Dir);
  return *this;
  }

//---------------------------------------------------------------------------

/*#F:#R:Returns the filename AND extension part of the file name*/
Strng Strng::FnNameExt(pchar FilePath)
  {
  char Name[_MAX_FNAME];
  char Ext[_MAX_EXT];
  _splitpath(FilePath?FilePath:Str()?Str():"", NULL, NULL, Name, Ext);
  Set("%s%s", Name, Ext);
  return *this;
  }

//---------------------------------------------------------------------------

/*#F:#R:Returns the drive AND directory AND filename part of the file name*/
Strng Strng::FnDrivePathName(pchar FilePath)
  {
  char Drv[_MAX_DRIVE];
  char Dir[_MAX_DIR];
  char Name[_MAX_FNAME];
  _splitpath(FilePath?FilePath:Str()?Str():"", Drv, Dir, Name, NULL);
  Set("%s%s%s", Drv, Dir, Name);
  return *this;
  }

//---------------------------------------------------------------------------

/*#F:#R:Returns the directory AND filename AND extension part of the file name*/
Strng Strng::FnPathNameExt(pchar FilePath)
  {
  char Dir[_MAX_DIR];
  char Name[_MAX_FNAME];
  char Ext[_MAX_EXT];
  _splitpath(FilePath?FilePath:Str()?Str():"", NULL, Dir, Name, Ext);
  Set("%s%s%s", Dir, Name, Ext);
  return *this;
  }

//---------------------------------------------------------------------------

/*#F:#R:Converts the number supplied into a binary string.*/
char* Strng::MakeBinary(DWORD dw, int MinLen, int Grp/*=-1*/)
  {
  *this = "";
  int i = 0;
  int BitCnt = 0;
  DWORD r = dw;
  while (r)
    {
    BitCnt++;
    *this += ((r%2) ? '1' : '0');
    r = r/2;
    if (++i==Grp)
      {
      i = 0;
      *this += ' ';
      }
    }
  while (BitCnt<MinLen)
    {
    BitCnt++;
    *this += '0';
    if (++i==Grp)
      {
      i = 0;
      *this += ' ';
      }
    }
  strrev(Buff);
  return Buff;
  }

//---------------------------------------------------------------------------

/*#F:#R:Converts the number supplied into an alpha string using specified base.*/
char* Strng::MakeAlpha(long dw, int MinLen/*-1*/, int Base/*=26*/)
  {
  ASSERT(Base>1);
  *this = "";
  long c = abs(dw);
  ldiv_t div_result;
  while (c>0)
    {
    div_result = ldiv(c-1, (long)Base);
    *this += (char)(65 + div_result.rem);
    c = div_result.quot;
    }
  if (dw<0)
    *this += '-';
  int CharCnt = strlen(Buff);
  while (CharCnt<MinLen)
    {
    CharCnt++;
    *this += ' ';
    }
  strrev(Buff);
  return Buff;
  }

//---------------------------------------------------------------------------
#if HEAP_SPARES
pchar Strng::SpareBuffs[MaxSpareStrBuffs]={0};
CRITICAL_SECTION Strng::BuffSect;
#endif

//---------------------------------------------------------------------------

/*#F:#R: A string with enough memory to hold the specified number of characters.*/

static int iInStrAlloc=0;
pchar Strng::AllocBuff(short l)// size of the required string's buffer
  {
#ifdef _DEBUG
  if (l>=TestForBigStrngSize)
    {
    ASSERT_ALWAYS(false, "Why a big Strng?!?");
    }
#endif
#if HEAP_SPARES
  pchar p;
  if (SparesOK)
    {
    EnterCriticalSection(&BuffSect);
    const short n=l / StrngAllocSize;
    if ((n < MaxSpareStrBuffs) && SpareBuffs[n])
      {
      p=SpareBuffs[n];
      ppchar pp=(ppchar)p;
      SpareBuffs[n]=*pp;
      }
    else
      p= new char[l+1];
    LeaveCriticalSection(&BuffSect);
    }
  else
    p=new char[l+1];
  return p;
#endif
  return new char[l+1];
  };

//---------------------------------------------------------------------------

/*#F:#R:Frees the memory allocated to a string to store the specified number of characters.*/
void Strng::FreeBuff(short l,// size of buffer allocated to the specified string
                     pchar p) // string containing the memory which is to be freed
  {
#if HEAP_SPARES
  if (SparesOK)
    {
    EnterCriticalSection(&BuffSect);
    ASSERT(AfxIsValidAddress(p, l));
    const short n=l / StrngAllocSize;
    if (n < MaxSpareStrBuffs)
      {
      ppchar pp=(ppchar)p;
      *pp=SpareBuffs[n];
      SpareBuffs[n]=p;
      //if (n==1)
      //  {
      //  if (IsBadWritePtr(p, 1))
      //    {
      //    int xxx=0;
      //    }
      //  }
      }
    else
      delete []p;
    LeaveCriticalSection(&BuffSect);
    }
  else
#endif
    delete []p;
  }

//---------------------------------------------------------------------------

int Strng::SafeStrlen(LPCTSTR lpsz)
  { return (lpsz == NULL) ? 0 : lstrlen(lpsz); }

void Strng::Concat(LPCTSTR lpsz1, LPCTSTR lpsz2, LPCTSTR lpsz3)
  {
  ConcatCopy(SafeStrlen(lpsz1), lpsz1, SafeStrlen(lpsz2), lpsz2);
  if (lpsz3)
    (*this)+=lpsz3;
  }

//---------------------------------------------------------------------------

void Strng::ConcatCopy(int nSrc1Len, LPCTSTR lpszSrc1Data,
  int nSrc2Len, LPCTSTR lpszSrc2Data)
  {
  int nNewLen = nSrc1Len + nSrc2Len;
  const int l = ReqdSize(nNewLen);
  Expand(l);
  memcpy(Buff, lpszSrc1Data, nSrc1Len*sizeof(char));
  memcpy(Buff+nSrc1Len, lpszSrc2Data, nSrc2Len*sizeof(char));
  Buff[nSrc1Len+nSrc2Len]=0;
  }

//---------------------------------------------------------------------------

Strng operator+(const Strng& string1, const Strng& string2)
  {
  Strng s;
  s.ConcatCopy(string1.Length(), string1(), string2.Length(), string2());
  return s;
  }

//---------------------------------------------------------------------------

Strng operator+(const Strng& string, LPCTSTR lpsz)
  {
  ASSERT(lpsz == NULL || AfxIsValidString(lpsz));
  Strng s;
  s.ConcatCopy(string.Length(), string(), Strng::SafeStrlen(lpsz), lpsz);
  return s;
  }

//---------------------------------------------------------------------------

Strng operator+(LPCTSTR lpsz, const Strng& string)
  {
  ASSERT(lpsz == NULL || AfxIsValidString(lpsz));
  Strng s;
  s.ConcatCopy(Strng::SafeStrlen(lpsz), lpsz, string.Length(), string());
  return s;
  }

//==========================================================================

IMPLEMENT_MEMSTATS(Strng_Queue)

Strng_Queue::Strng_Queue()
  {
  qTail = qHead = NULL;
  len = 0;
  };

//---------------------------------------------------------------------------


Strng_Queue::~Strng_Queue()
  {
  Strng b("");
  while (qHead) Get(b);
  };

//---------------------------------------------------------------------------

/*#F:Appends an item to the end of the Strng Queue.*/
int Strng_Queue::Put(Strng &b)
  {
  pStrng bp = new Strng(b.Buff);
  bp->pNxt = qTail;
  if (qTail)
    qTail->pPrv = bp;
  qTail = bp;
  if (!qHead)
    qHead = bp;
  len++;
  return 0;
  };

//---------------------------------------------------------------------------

/*#F:Places a Strng in the Strng Queue, either immediately before the tail of the Strng Queue, or, as the tail, if a tail does not already exist.*/
int Strng_Queue::Put(pchar s)
  {
  pStrng bp = new Strng(s);
  bp->pNxt = qTail;
  if (qTail)
    qTail->pPrv = bp;
  qTail = bp;
  if (!qHead)
    qHead = bp;
  len++;
  return 0;
  };

//---------------------------------------------------------------------------

/*#F:Retrieves the Strng from the head of the Strng Queue or a zero length string if the queue is empty.*/
int Strng_Queue::Get(Strng &b)
  {
  if (qHead)
    {
    pStrng bp = qHead;
    b.pNxt = NULL;
    b.pPrv = NULL;
    b = qHead->Buff;
    qHead = qHead->pPrv;
    if (qHead)
      qHead->pNxt = NULL;
    else
      qTail = NULL;
    len--;
    delete bp;
    }
  else
    b = "";
  return 0;
  };

//---------------------------------------------------------------------------

/*#F:Removes all the Strngs from the Strng Queue.*/
int Strng_Queue::Clear()
  {
  pStrng p = qTail;
  while (p)
    {
    pStrng pn = p->pNxt;
    delete p;
    p = pn;
    }
  qTail = NULL;
  qHead = NULL;
  len = 0;
  return 0;
  };


//==========================================================================

Strng_List::Strng_List()
  {
  Head = Tail = Posn = NULL;
  len = 0;
  }

//---------------------------------------------------------------------------

Strng_List::~Strng_List()
  {
  Clear();
  }

//---------------------------------------------------------------------------
// Copy Constructor
Strng_List::Strng_List(Strng_List & L)// Strng List to be appended.
  {
  pStrng p = L.Head;
  while (p)
    {
    //L->Remove(p);
    Append(p->Str());
    p = p->pNxt;
    }
  }

//---------------------------------------------------------------------------
// Assingment
Strng_List &Strng_List::operator=(Strng_List & L)// Strng List to be appended.
  {
  Clear();
  pStrng p = L.Head;
  while (p)
    {
    //L->Remove(p);
    Append(p->Str());
    p = p->pNxt;
    }
  return *this;
  }

//---------------------------------------------------------------------------

/*#F:Removes all the Strngs from the Strng List.*/
void Strng_List::Clear()
  {
  pStrng p = Head;
  while (p)
    {
    pStrng pn = p->pNxt;
    delete p;
    p = pn;
    }
  Head = Tail = Posn = NULL;
  len = 0;
  }

//---------------------------------------------------------------------------

void Strng_List::Insert(pStrng_List L) //Strng List to be inserted.
  {
  VERIFY(0);
  }

//---------------------------------------------------------------------------

Strng * Strng_List::Insert(Strng & S)
  {
  Strng* p=Insert(S());
  p->SetIndex(S.Index());
  return p;
  }

//---------------------------------------------------------------------------

Strng* Strng_List::Insert(pStrng p) //Strng to be inserted.
  {
  if (p)
    {
    p->pNxt = Head;
    p->pPrv = NULL;
    if (Head)
      Head->pPrv = p;
    Head = p;
    if (!Tail)
      Tail = p;
    len++;
    }
  return p;
  }

//---------------------------------------------------------------------------

Strng* Strng_List::Append(Strng & S) // strng to be appended
  {
  Strng* p=Append(S());
  p->SetIndex(S.Index());
  return p;
  }

//---------------------------------------------------------------------------

Strng* Strng_List::Append(pStrng p) // strng to be appended
  {
  if (p)
    {
    p->pNxt = NULL;
    p->pPrv = Tail;
    if (Tail)
      Tail->pNxt = p;
    Tail = p;
    if (!Head)
      Head = p;
    len++;
    }
  return p;
  }

//---------------------------------------------------------------------------

/*#F:Appends, the specified item, to the end of the Strng List*/
void Strng_List::Append(pStrng_List L)// Strng List to be appended.
  {
  if (L)
    {
    pStrng p = L->Head;
    while (p)
      {
      L->Remove(p);
      Append(p);
      p = L->Head;
      }
    }
  }

//---------------------------------------------------------------------------

int Strng_List::Index(pStrng p)
  {
  if (!Head)
    return -1;
  int cnt = 0;
  pStrng pCur = Head;
  while (pCur)
    {
    if (p==pCur)
      return cnt;
    cnt++;
    pCur = pCur->pNxt;
    }
  return -1;
  }

//---------------------------------------------------------------------------

pStrng Strng_List::AtIndex(int i)
  {
  pStrng pCur = Head;
  while (i-- && pCur)
    pCur = pCur->pNxt;
  return pCur;
  }

//---------------------------------------------------------------------------

pStrng Strng_List::AtIndexVal(int IndexVal)
  {
  pStrng pCur = Head;
  while (pCur)
    {
    if (pCur->Index()==IndexVal)
      return pCur;
    pCur = pCur->pNxt;
    }
  return pCur;
  }

//---------------------------------------------------------------------------

/*#F:Removes the specified Strng from the Strng List.*/
void Strng_List::Remove(pStrng p)// Strng to be removed.
  {
  pStrng pp = NULL;
  pStrng pn = NULL;
  if (p==Head)
    {
    Head = p->pNxt;
    if (Head)
      Head->pPrv = NULL;
    }
  else
    pp = p->pPrv;

  if (p==Tail)
    {
    Tail = p->pPrv;
    if (Tail)
      Tail->pNxt = NULL;
    }
  else
    pn = p->pNxt;

  if (pn)
    pn->pPrv = pp;
  if (pp)
    pp->pNxt = pn;

  len--;
  }

//---------------------------------------------------------------------------

/*#F:#R:The Strng containing the specified string*/
pStrng Strng_List::Find(Strng & S,          // the string to be located.
                        flag CaseSensitive) // if True then the case of string,to be located, must match that of the specified string.If False the case of the strings are not taken into consideration in the search.
  {
  return Find(S(), CaseSensitive);
  }

//---------------------------------------------------------------------------

/*#F:#R:The Strng containing the specified string*/
pStrng Strng_List::Find(const char* s,   // the string to be located.
                        flag CaseSensitive) // if True then the case of string,to be located, must match that of the specified string.If False the case of the strings are not taken into consideration in the search.
  {
  pStrng f = Head;
  if (CaseSensitive)
    while (f && (strcmp(f->Str(), s)!=0)) f=f->pNxt;
  else
    while (f && (strcmpi(f->Str(), s)!=0)) f=f->pNxt;
  return f;
  }

//---------------------------------------------------------------------------

/*#F:#R:The Strng containing the specified string*/
pStrng Strng_List::FindFrom(pStrng pStart,      // starting string for search
                            const char* s,      // the string to be located.
                            flag CaseSensitive) // if True then the case of string,to be located, must match that of the specified string.If False the case of the strings are not taken into consideration in the search.
  {
  pStrng f = pStart;
  if (CaseSensitive)
    while (f && (strcmp(f->Str(), s)!=0)) f=f->pNxt;
  else
    while (f && (strcmpi(f->Str(), s)!=0)) f=f->pNxt;
  return f;
  }

//---------------------------------------------------------------------------

int Strng_List::AppendTokString(char* s, char* tok)
  {
  const int len = strlen(s);
  if (len==0)
    return 0;
  char* buf = new char[len+1];
  strcpy(buf, s);
  int cnt = 0;
  char* p = buf;
  p = strtok(p, tok);
  while (p)
    {
    Append(p);
    cnt++;
    p = strtok(NULL, tok);
    }
  delete buf;
  return cnt;
  }

//---------------------------------------------------------------------------

void Strng_List::BuildTokString(Strng& s, char* tok)
  {
  s = "";
  pStrng p = Head;
  while (p)
    {
    s += p->Str();
    s += tok;
    p = p->pNxt;
    }
  }

// =========================================================================
//
//
//
//===========================================================================

CSVector::CSVector( long NewLen,
                    const char *InitVal)
  {
  m_d = NULL;
  m_Len = 0;
  SetSize(NewLen, InitVal);
  }

// -------------------------------------------------------------------------

CSVector::CSVector()
  {
  m_d = NULL;
  m_Len = 0;
  }

// -------------------------------------------------------------------------

CSVector::CSVector(const CSVector &v)
  {
  m_d = NULL;
  m_Len = 0;
  SetSize(v.m_Len);
  for (long i=0; i<m_Len; i++)
    m_d[i] = v.m_d[i];
  }

// -------------------------------------------------------------------------

CSVector::~CSVector()
  {
  FreePts();
  }

// -------------------------------------------------------------------------

void CSVector::FreePts()
  {
  if (m_d)
    {
    delete []m_d;
    m_d = NULL;
    m_Len = 0;
    }
  }

// -------------------------------------------------------------------------

flag CSVector::SetSize( long NewLen,     //New length of the vector.
                        const char * InitVal)  //Value to initialise all new elements to. (default = 0)
  {
  ASSERT(NewLen>=0); //must have positive length
  if (NewLen==m_Len)
    return True;
  if (NewLen==0)
    {
    FreePts();
    return True;
    }
  Strng* nd = new Strng[NewLen];
  long j = Min(m_Len,NewLen);
  for (long i=0; i<j; i++)
    nd[i] = m_d[i];
  j = (m_Len<NewLen) ? NewLen : 0;
  for (; i<j; i++)
    nd[i] = InitVal;
  FreePts();
  m_d = nd;
  m_Len = NewLen;
  return True;
  }

// -------------------------------------------------------------------------

flag CSVector::Swop( long i1,  //Index value one
                     long i2)  //Index value two
  {
  ASSERT(i1>=0 && i1<m_Len && i2>=0 && i2<m_Len);  //check for valid index range
  Strng temp = m_d[i1];
  m_d[i1] = m_d[i2];
  m_d[i2] = temp;
  return True;
  }

// -------------------------------------------------------------------------

flag CSVector::Sort(flag Ascending)//True if vector is to be sorted from lowest to highest, default = True
  {
  for (long i=1; i<m_Len; i++)
    for (long j=i; j>=1 && ((m_d[j].XStrCmp(m_d[j-1])<0)==Ascending); j--)
      {
      Strng temp = m_d[j-1];
      m_d[j-1] = m_d[j];
      m_d[j] = temp;
      }
  return True;
  }

// -------------------------------------------------------------------------

flag CSVector::Reverse()
  {
  if (m_Len>1)
    {
    const long k = div(m_Len,2L).quot;
    for (long i=0; i<k; i++)
      {
      Strng temp = m_d[i];
      m_d[i] = m_d[m_Len-i-1];
      m_d[m_Len-i-1] = temp;
      }
    }
  return True;
  }

// -------------------------------------------------------------------------

flag CSVector::SetAll(const char * s)
  {
  for (long i=0; i<m_Len; i++)
    m_d[i] = s;
  return True;
  }

// -------------------------------------------------------------------------

long CSVector::Find(char* s, flag CaseSensitive, long StartIndex)
  {
  if (s)
    {
    const size_t len = strlen(s);
    for (long i=StartIndex; i<m_Len; i++)
      {
      if ((size_t)(m_d[i].Length())==len)
        {
        if (CaseSensitive)
          {
          if (strcmp(m_d[i](), s)==0)
            return i;
          }
        else
          {
          if (_stricmp(m_d[i](), s)==0)
            return i;
          }
        }
      }
    }
  return -1;
  }

// -------------------------------------------------------------------------

CSVector& CSVector::operator=(const CSVector &v)
  {
  SetSize(v.m_Len);
  for (long i=0; i<m_Len; i++)
    m_d[i] = v.m_d[i];
  return *this;
  }

// -------------------------------------------------------------------------

flag CSVector::operator==(const CSVector &v)
  {
  if (m_Len != v.m_Len)
    return False;
  for (long i=0; i<m_Len; i++)
    if (m_d[i].XStrCmp(v.m_d[i])!=0)
      return False;
  return True;
  }

// -------------------------------------------------------------------------

void CSVector::dbgDump( pchar Desc,
                        flag Horizontal)
  {
  /*
  Dbg->Ln("%s|Len:%d|", Desc, m_Len);
  if (Horizontal)
    {
    for (long i=0; i<Min(5L,m_Len); i++)
    Dbg->Ln("%8d ", m_d[i]);
  if (m_Len>5)
    Dbg->Ln("...");
    Dbg->Ln("\n");
    }
  else
    for (long i=0; i<m_Len; i++)
    Dbg->Ln("%4d: %8d\n", i, m_d[i]);
  */
  }

//===========================================================================
//
//
//
//===========================================================================

///*#F:This converts a time in seconds into the corresponding date ( day/month/year ) and time ( hour:minute:second. hundredth of a second).*/
//pchar SecstoDateHMS(double time_in, // the time in seconds.
//                    pchar buff,  // the buffer into which the result is to be stored.
//                    flag WithDate)// if True it returns the date followed by the time, if False only the time is returned.
//  {
//  if (!WithDate)
//    return SecstoHMS(time_in, buff);
//  __time64_t tt=(__time64_t)(time_in);
//  tm* t=_localtime64(&tt);
//  int hsec=(int)((time_in-tt)*100.0);
//  if (t)
//    sprintf(buff, "%02i/%02i/%04i  %02i:%02i:%02i.%02i",t->tm_mday,t->tm_mon+1,t->tm_year+1900,t->tm_hour,t->tm_min,t->tm_sec,hsec);
//  else
//    strcpy(buff, "?");
//  return buff;
//  };
//
////---------------------------------------------------------------------------
//
///*#F:This converts a date ( day/month/year ) and time ( hour:minute:second. hundredth of a second) into the corresponding time in seconds.*/
//flag DateHMStoSecs(pchar buff, //the buffer in which the date and time are stored,
//                   double &time_out) //the resultant time in seconds.
//  {
//  tm t;
//  memset(&t, 0, sizeof(t));
//  long tm_hsec=0;
//  char b[256], *p=b;
//  strcpy(b, buff);
//  int e=strlen(b)-1;
//  if (e>=0 && (b[e]==':' || b[e]=='.'))
//    strcat(b, "0");
//  else if (e>=0 && (b[e]=='/'))
//    strcat(b, "96");
//  strcat(b, " ");
//  int BusyTime=1, TOK=0, DOK=0;//, done=0;
//  flag fTime=1, fDate=0;
//  while (1)//!done)
//    {
//    pchar d=strpbrk(p, " :/.");
//    int   l=d-p;
//
//    if (d && isdigit(*p))
//      {
//      if (*d=='/')
//        {
//        fTime=0;
//        fDate=1;
//        }
//      else if (*d==':')
//        {
//        fTime=1;
//        fDate=0;
//        }
//
//      if (fTime)
//        {
//        BusyTime=1;
//        t.tm_hour=t.tm_min;
//        t.tm_min=t.tm_sec;
//        t.tm_sec=atol(p);
//        if (*d=='.')
//          {
//          p=d+1;
//          for (int n=0; isdigit(*(p+n)); n++) {};
//          tm_hsec=Min(99L, (long)((strlen(d)>0) ? atol(p) : 0));
//          if (n==1)
//            tm_hsec/=10;
//          p+=n;
//          fTime=0;
//          }
//        else
//          {
//          tm_hsec=0;
//          p=d+1;
//          }
//        TOK=1;
//        }
//      else
//        {
//        BusyTime=0;
//        t.tm_year=t.tm_mon+1-1900;
//        t.tm_mon=t.tm_mday-1;
//        t.tm_mday=atol(p);
//        DOK=1;
//        p=d+1;
//        }
//      }
//    else
//      break;
//    }
//  if (!DOK)
//    time_out= t.tm_hour*3600.0+t.tm_min*60.0+t.tm_sec+tm_hsec/100.0;
//  else
//    time_out=mktime(&t)+tm_hsec/100.0;
//  //TRACE2(">>Secs %15.2g %s\n",time_out, buff);
//
//  return TOK||DOK;
//  };
//
////===========================================================================
//
///*#F:This converts a time in seconds into the corresponding time ( hour:minute:second. hundredth of a second) and date ( day/month/year ).*/
//pchar SecstoHMSDate(double time_in, // the time in seconds.
//                    pchar buff,  // the buffer in which the result is to be stored.
//                    flag WithDate)// if True it returns the date followed by the time, if False only the time is returned.
//  {
//  //time_in = floor(time_in*1.0e6+0.5)/1.0e6; //round for lost precision on LSD
//  if (!WithDate)
//    return SecstoHMS(time_in, buff);
//  __time64_t tt=(__time64_t)(time_in);
//  tm* t=_localtime64(&tt);
//  int hsec=(int)((time_in-tt)*100.0);
//  if (t)
//    sprintf(buff, "%02i:%02i:%02i.%02i  %02i/%02i/%04i",t->tm_hour,t->tm_min,t->tm_sec,hsec,t->tm_mday,t->tm_mon+1,t->tm_year+1900);
//  else
//    strcpy(buff, "?");
//  //TRACE2("Secs>> %15.2g %s",time_in, buff);
//  return buff;
//  };
//
////---------------------------------------------------------------------------
//
///*#F:This converts a date ( day/month/year ) and time ( hour:minute:second. hundredth of a second) into the corresponding time in seconds.*/
//flag HMSDatetoSecs(pchar buff, //the buffer in which the date and time are stored.
//                   double &time_out) //the resultant time in seconds.
//  {
//  tm t;
//  memset(&t, 0, sizeof(t));
//  long tm_hsec=0;
//  char b[256], *p=b;
//  strcpy(b, buff);
//
//  int BusyTime=1, TOK=0, DOK=0, done=0, DoneSpace=0, DoneDot=0;
//  while (!done)
//    {
//    pchar d=strpbrk(p, " :/.");
//    int   l=d-p;
//    if (d==NULL)
//      {
//      d=" ";
//      done=1;
//      }
//    if ((*d==':' || *d=='.' || *d==' ') && (!DoneSpace))
//      {
//      if (*d==' ')
//        DoneSpace=1;
//      if (*d=='.')
//        DoneDot=1;
//
//      BusyTime=1;
//      t.tm_hour=t.tm_min;
//      t.tm_min=t.tm_sec;
//      t.tm_sec=tm_hsec;
//      tm_hsec=atol(p);
//      TOK=1;
//      }
//    else if (*d=='/' || *d==' ')
//      {
//      BusyTime=0;
//      t.tm_mday=t.tm_mon;
//      t.tm_mon=t.tm_year;
//      t.tm_year=atol(p);
//      DOK=1;
//      }
//    p=d+1;
//    }
//  if (!DoneDot)
//    {
//    t.tm_hour=t.tm_min;
//    t.tm_min=t.tm_sec;
//    t.tm_sec=tm_hsec;
//    tm_hsec=0;
//    }
//  if (!DOK)
//    time_out= t.tm_hour*3600.0+t.tm_min*60.0+t.tm_sec+tm_hsec/100.0;
//  else
//    {
//    if (t.tm_year>1900)
//      t.tm_year -= 1900;
//    t.tm_mon--;
//    time_out=(double)mktime(&t)+tm_hsec/100.0;
//    }
//  return TOK||DOK;
//  };
//
////---------------------------------------------------------------------------
///*#F:This converts a time in seconds into the corresponding time ( hour:minute:second. hundredth of a second).*/
//pchar SecstoHMS(double time_in,// the time in seconds
//                pchar buff,  // the buffer in which the result is to be stored.
//                flag TruncDate/*=0*/, //must the date be ignored ie max hours=23
//                flag TruncMilliSecs/*=0*/)
//  {
//  long t=(long)time_in;
//  long hsecs=(long)((time_in-t)*100.0);
//  long secs=t%60;
//  long mins=(t/60)%60;
//  long hours=t/3600; //does not work ??? TIME ZONES !!!
//  if (TruncDate)
//    hours=hours%24;
//
//  if (TruncMilliSecs)
//    {
//    sprintf(buff, "%02i:%02i:%02i", hours, mins, secs);
//    }
//  else
//    {
//    long hsecs=(long)((time_in-t)*100.0);
//    sprintf(buff, "%02i:%02i:%02i.%02i", hours, mins, secs, hsecs);
//    }
//  return buff;
//  };
//
////---------------------------------------------------------------------------
///*#F:This converts a time in seconds into the corresponding date ( day/month/year ).*/
//pchar SecstoDate(double time_in,// the time in seconds
//                    pchar buff)  // the buffer in which the result is to be stored.
//  {
//  __time64_t tt=(__time64_t)(time_in);
//  tm* t=_localtime64(&tt);
//  if (t)
//    sprintf(buff, "%02i/%02i/%04i", t->tm_mday, t->tm_mon+1, t->tm_year+1900);
//  else
//    strcpy(buff, "?");
//  return buff;
//  };
//
////---------------------------------------------------------------------------
///*#F:This converts a time ( hour:minute:second. hundredth of a second) into the corresponding time in seconds.*/
//flag HMStoSecs(pchar buff,//the buffer in which the time is stored.
//                   double &time_out) //the resultant time in seconds
//  {
//  return DateHMStoSecs(buff, time_out);
//  };
//
////---------------------------------------------------------------------------
///*#F:This converts a time in seconds into the corresponding time ( hour:minute:second. hundredth of a second).*/
//pchar SecstoHMSCnv(double time_in,// the time in seconds
//                   pchar buff,  // the buffer in which the result is to be stored.
//                   flag TruncDate/*=0*/, //must the date be ignored ie max hours=23
//                   flag TruncMilliSecs/*=0*/)
//  {
//  long t=(long)time_in;
//  long secs=t%60;
//  long mins=(t/60)%60;
//  long hours=t/3600;
//  if (TruncDate)
//    hours=hours%24;
//
//  if (TruncMilliSecs)
//    {
//    sprintf(buff, "%02i:%02i:%02i", hours, mins, secs);
//    }
//  else
//    {
//    long hsecs=(long)((time_in-t)*100.0);
//    sprintf(buff, "%02i:%02i:%02i.%02i", hours, mins, secs, hsecs);
//    }
//  return buff;
//  };
//
////---------------------------------------------------------------------------
///*#F:This converts a time in seconds into the corresponding time ( hour:minute:second. hundredth of a second).*/
//pchar SecstoDHMSCnv(double time_in,// the time in seconds
//                    pchar buff,  // the buffer in which the result is to be stored.
//                    flag TruncMilliSecs/*=0*/)
//  {
//  long t=(long)time_in;
//  const long secs=t%60;
//  const long mins=(t/60)%60;
//  const long days=t/86400;
//  const long hours=(days>1 ? (t/3600)%24 : t/3600);
//  if (TruncMilliSecs)
//    {
//    if (days>1)
//      sprintf(buff, "%ddays %02i:%02i:%02i", days, hours, mins, secs);
//    else
//      sprintf(buff, "%02i:%02i:%02i", hours, mins, secs);
//    }
//  else
//    {
//    long hsecs=(long)((time_in-t)*100.0);
//    if (days>1)
//      sprintf(buff, "%ddays %02i:%02i:%02i.%02i", days, hours, mins, secs, hsecs);
//    else
//      sprintf(buff, "%02i:%02i:%02i.%02i", hours, mins, secs, hsecs);
//    }
//  return buff;
//  };
//
//---------------------------------------------------------------------------

bool SecsToDate1900Var(double Secs1970, VARIANT * pVal)
  {
  if (pVal)
    {
    __time64_t t = (__time64_t)Max(0.0, Secs1970);
    struct tm *pT=_localtime64(&t);
    SYSTEMTIME ST;
    ST.wYear=pT->tm_year+1900;
    ST.wMonth=pT->tm_mon+1;
    ST.wDay=pT->tm_mday;
    ST.wHour=pT->tm_hour;
    ST.wMinute=pT->tm_min;
    ST.wSecond=pT->tm_sec;
    ST.wMilliseconds=0;
    pVal->vt=VT_DATE;
    SystemTimeToVariantTime(&ST, &pVal->dblVal);
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

bool Date1900VarToSecs(VARIANT Val, double * pSecs1970)
  {
  if (pSecs1970)
    {
    if (Val.vt==VT_DATE)
      {
      SYSTEMTIME ST;
      VariantTimeToSystemTime(Val.dblVal, &ST);
      tm t;
      t.tm_year   =      ST.wYear-1900;
      t.tm_mon    =      ST.wMonth-1;
      t.tm_mday   =      ST.wDay;
      t.tm_hour   =      ST.wHour;
      t.tm_min    =      ST.wMinute;
      t.tm_sec    =      ST.wSecond;
      (*pSecs1970) =     (double)_mktime64(&t);
      return true;
      }
    }
  return false;
  }

//===========================================================================
//
//
//
//===========================================================================

///*#F:This converts a time in seconds into the corresponding date ( day/month/year ) and time ( hour:minute:second. hundredth of a second).*/
//pchar SecstoDateHMS(CTimeValue time_in, // the time in seconds.
//                    pchar buff,  // the buffer into which the result is to be stored.
//                    flag WithDate)// if True it returns the date followed by the time, if False only the time is returned.
//  {
//  __debugbreak();
//  if (!WithDate)
//    return SecstoHMS(time_in, buff);
//  __time64_t tt=(__time64_t)time_in.SecondsI64;
//  tm* t=_localtime64(&tt);
//  int hsec=(int)((time_in.Seconds-tt)*100.0);
//  if (t)
//    sprintf(buff, "%02i/%02i/%04i  %02i:%02i:%02i.%02i",t->tm_mday,t->tm_mon+1,t->tm_year+1900,t->tm_hour,t->tm_min,t->tm_sec,hsec);
//  else
//    strcpy(buff, "?");
//  return buff;
//  };
//
////---------------------------------------------------------------------------
//
///*#F:This converts a date ( day/month/year ) and time ( hour:minute:second. hundredth of a second) into the corresponding time in seconds.*/
//flag DateHMStoSecs(pchar buff, //the buffer in which the date and time are stored,
//                   CTimeValue &time_out) //the resultant time in seconds.
//  {
//  tm t;
//  memset(&t, 0, sizeof(t));
//  long tm_hsec=0;
//  char b[256], *p=b;
//  strcpy(b, buff);
//  int e=strlen(b)-1;
//  if (e>=0 && (b[e]==':' || b[e]=='.'))
//    strcat(b, "0");
//  else if (e>=0 && (b[e]=='/'))
//    strcat(b, "96");
//  strcat(b, " ");
//  int BusyTime=1, TOK=0, DOK=0;//, done=0;
//  flag fTime=1, fDate=0;
//  while (1)//!done)
//    {
//    pchar d=strpbrk(p, " :/.");
//    int   l=d-p;
//
//    if (d && isdigit(*p))
//      {
//      if (*d=='/')
//        {
//        fTime=0;
//        fDate=1;
//        }
//      else if (*d==':')
//        {
//        fTime=1;
//        fDate=0;
//        }
//
//      if (fTime)
//        {
//        BusyTime=1;
//        t.tm_hour=t.tm_min;
//        t.tm_min=t.tm_sec;
//        t.tm_sec=atol(p);
//        if (*d=='.')
//          {
//          p=d+1;
//          for (int n=0; isdigit(*(p+n)); n++) {};
//          tm_hsec=Min(99L, (long)((strlen(d)>0) ? atol(p) : 0));
//          if (n==1)
//            tm_hsec/=10;
//          p+=n;
//          fTime=0;
//          }
//        else
//          {
//          tm_hsec=0;
//          p=d+1;
//          }
//        TOK=1;
//        }
//      else
//        {
//        BusyTime=0;
//        t.tm_year=t.tm_mon+1-1900;
//        t.tm_mon=t.tm_mday-1;
//        t.tm_mday=atol(p);
//        DOK=1;
//        p=d+1;
//        }
//      }
//    else
//      break;
//    }
//  if (!DOK)
//    time_out= t.tm_hour*3600.0+t.tm_min*60.0+t.tm_sec+tm_hsec/100.0;
//  else
//    time_out=mktime(&t)+tm_hsec/100.0;
//  //TRACE2(">>Secs %15.2g %s\n",time_out, buff);
//
//  return TOK||DOK;
//  };
//
////===========================================================================
//
///*#F:This converts a time in seconds into the corresponding time ( hour:minute:second. hundredth of a second) and date ( day/month/year ).*/
//pchar SecstoHMSDate(CTimeValue time_in, // the time in seconds.
//                    pchar buff,  // the buffer in which the result is to be stored.
//                    flag WithDate)// if True it returns the date followed by the time, if False only the time is returned.
//  {
//  //time_in = floor(time_in*1.0e6+0.5)/1.0e6; //round for lost precision on LSD
//  if (!WithDate)
//    return SecstoHMS(time_in, buff);
//  __time64_t tt=(__time64_t)time_in.SecondsI64;
//  tm* t=_localtime64(&tt);
//  int hsec=(int)((time_in.Seconds-tt)*100.0);
//  if (t)
//    sprintf(buff, "%02i:%02i:%02i.%02i  %02i/%02i/%04i",t->tm_hour,t->tm_min,t->tm_sec,hsec,t->tm_mday,t->tm_mon+1,t->tm_year+1900);
//  else
//    strcpy(buff, "?");
//  //TRACE2("Secs>> %15.2g %s",time_in, buff);
//  return buff;
//  };
//
////---------------------------------------------------------------------------
//
///*#F:This converts a date ( day/month/year ) and time ( hour:minute:second. hundredth of a second) into the corresponding time in seconds.*/
//flag HMSDatetoSecs(pchar buff, //the buffer in which the date and time are stored.
//                   CTimeValue &time_out) //the resultant time in seconds.
//  {
//  tm t;
//  memset(&t, 0, sizeof(t));
//  long tm_hsec=0;
//  char b[256], *p=b;
//  strcpy(b, buff);
//
//  int BusyTime=1, TOK=0, DOK=0, done=0, DoneSpace=0, DoneDot=0;
//  while (!done)
//    {
//    pchar d=strpbrk(p, " :/.");
//    int   l=d-p;
//    if (d==NULL)
//      {
//      d=" ";
//      done=1;
//      }
//    if ((*d==':' || *d=='.' || *d==' ') && (!DoneSpace))
//      {
//      if (*d==' ')
//        DoneSpace=1;
//      if (*d=='.')
//        DoneDot=1;
//
//      BusyTime=1;
//      t.tm_hour=t.tm_min;
//      t.tm_min=t.tm_sec;
//      t.tm_sec=tm_hsec;
//      tm_hsec=atol(p);
//      TOK=1;
//      }
//    else if (*d=='/' || *d==' ')
//      {
//      BusyTime=0;
//      t.tm_mday=t.tm_mon;
//      t.tm_mon=t.tm_year;
//      t.tm_year=atol(p);
//      DOK=1;
//      }
//    p=d+1;
//    }
//  if (!DoneDot)
//    {
//    t.tm_hour=t.tm_min;
//    t.tm_min=t.tm_sec;
//    t.tm_sec=tm_hsec;
//    tm_hsec=0;
//    }
//  if (!DOK)
//    time_out= t.tm_hour*3600.0+t.tm_min*60.0+t.tm_sec+tm_hsec/100.0;
//  else
//    {
//    if (t.tm_year>1900)
//      t.tm_year -= 1900;
//    t.tm_mon--;
//    time_out=CTimeValue(mktime(&t))+tm_hsec/100.0;
//    }
//  return TOK||DOK;
//  };
//
////---------------------------------------------------------------------------
///*#F:This converts a time in seconds into the corresponding time ( hour:minute:second. hundredth of a second).*/
//pchar SecstoHMS(CTimeValue time_in,// the time in seconds
//                pchar buff,  // the buffer in which the result is to be stored.
//                flag TruncDate/*=0*/, //must the date be ignored ie max hours=23
//                flag TruncMilliSecs/*=0*/)
//  {
//  long t=(long)time_in.SecondsI64;
//  long hsecs=(long)((time_in.Seconds-t)*100.0);
//  long secs=t%60;
//  long mins=(t/60)%60;
//  long hours=t/3600; //does not work ??? TIME ZONES !!!
//  if (TruncDate)
//    hours=hours%24;
//
//  if (TruncMilliSecs)
//    {
//    sprintf(buff, "%02i:%02i:%02i", hours, mins, secs);
//    }
//  else
//    {
//    long hsecs=(long)((time_in.Seconds-t)*100.0);
//    sprintf(buff, "%02i:%02i:%02i.%02i", hours, mins, secs, hsecs);
//    }
//  return buff;
//  };
//
////---------------------------------------------------------------------------
///*#F:This converts a time in seconds into the corresponding date ( day/month/year ).*/
//pchar SecstoDate(CTimeValue time_in,// the time in seconds
//                    pchar buff)  // the buffer in which the result is to be stored.
//  {
//  __time64_t tt=(__time64_t)time_in.SecondsI64;
//  tm* t=_localtime64(&tt);
//  if (t)
//    sprintf(buff, "%02i/%02i/%04i", t->tm_mday, t->tm_mon+1, t->tm_year+1900);
//  else
//    strcpy(buff, "?");
//  return buff;
//  };
//
////---------------------------------------------------------------------------
///*#F:This converts a time ( hour:minute:second. hundredth of a second) into the corresponding time in seconds.*/
//flag HMStoSecs(pchar buff,//the buffer in which the time is stored.
//                   CTimeValue &time_out) //the resultant time in seconds
//  {
//  return DateHMStoSecs(buff, time_out);
//  };
//
////---------------------------------------------------------------------------
///*#F:This converts a time in seconds into the corresponding time ( hour:minute:second. hundredth of a second).*/
//pchar SecstoHMSCnv(CTimeValue time_in,// the time in seconds
//                   pchar buff,  // the buffer in which the result is to be stored.
//                   flag TruncDate/*=0*/, //must the date be ignored ie max hours=23
//                   flag TruncMilliSecs/*=0*/)
//  {
//  long t=(long)time_in.Seconds;
//  long secs=t%60;
//  long mins=(t/60)%60;
//  long hours=t/3600;
//  if (TruncDate)
//    hours=hours%24;
//
//  if (TruncMilliSecs)
//    {
//    sprintf(buff, "%02i:%02i:%02i", hours, mins, secs);
//    }
//  else
//    {
//    long hsecs=(long)((time_in.Seconds-t)*100.0);
//    sprintf(buff, "%02i:%02i:%02i.%02i", hours, mins, secs, hsecs);
//    }
//  return buff;
//  };
//
////---------------------------------------------------------------------------
///*#F:This converts a time in seconds into the corresponding time ( hour:minute:second. hundredth of a second).*/
//pchar SecstoDHMSCnv(CTimeValue time_in,// the time in seconds
//                    pchar buff,  // the buffer in which the result is to be stored.
//                    flag TruncMilliSecs/*=0*/)
//  {
//  long t=(long)time_in.Seconds;
//  const long secs=t%60;
//  const long mins=(t/60)%60;
//  const long days=t/86400;
//  const long hours=(days>1 ? (t/3600)%24 : t/3600);
//  if (TruncMilliSecs)
//    {
//    if (days>1)
//      sprintf(buff, "%ddays %02i:%02i:%02i", days, hours, mins, secs);
//    else
//      sprintf(buff, "%02i:%02i:%02i", hours, mins, secs);
//    }
//  else
//    {
//    long hsecs=(long)((time_in.Seconds-t)*100.0);
//    if (days>1)
//      sprintf(buff, "%ddays %02i:%02i:%02i.%02i", days, hours, mins, secs, hsecs);
//    else
//      sprintf(buff, "%02i:%02i:%02i.%02i", hours, mins, secs, hsecs);
//    }
//  return buff;
//  };

//---------------------------------------------------------------------------

bool SecsToDate1900Var(CTimeValue Secs1970, VARIANT * pVal)
  {
  if (pVal)
    {
    __time64_t t = (__time64_t)Max(0.0, Secs1970.Seconds);
    struct tm *pT=_localtime64(&t);
    SYSTEMTIME ST;
    ST.wYear=pT->tm_year+1900;
    ST.wMonth=pT->tm_mon+1;
    ST.wDay=pT->tm_mday;
    ST.wHour=pT->tm_hour;
    ST.wMinute=pT->tm_min;
    ST.wSecond=pT->tm_sec;
    ST.wMilliseconds=0;
    pVal->vt=VT_DATE;
    SystemTimeToVariantTime(&ST, &pVal->dblVal);
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

bool Date1900VarToSecs(VARIANT Val, CTimeValue * pSecs1970)
  {
  if (pSecs1970)
    {
    if (Val.vt==VT_DATE)
      {
      SYSTEMTIME ST;
      VariantTimeToSystemTime(Val.dblVal, &ST);
      tm t;
      t.tm_year   =      ST.wYear-1900;
      t.tm_mon    =      ST.wMonth-1;
      t.tm_mday   =      ST.wDay;
      t.tm_hour   =      ST.wHour;
      t.tm_min    =      ST.wMinute;
      t.tm_sec    =      ST.wSecond;
      (*pSecs1970) = mktime(&t);
      return true;
      }
    }
  return false;
  }

//---------------------------------------------------------------------------

char* CurDateTime(Strng& s)
  {
  char DT[128],TM[128];
  _strdate(DT);
  _strtime(TM);
  s.Set("%s %s", DT, TM);
  return s();
  }

//---------------------------------------------------------------------------

char* CurDateTime(CString& s)
  {
  char DT[128],TM[128];
  _strdate(DT);
  _strtime(TM);
  s.Format("%s %s", DT, TM);
  return (LPTSTR)(LPCTSTR)s;
  }

///===========================================================================

double FractionToDouble(char * p)
  {
  double d=atof(p);
  while (isdigit(*p) || *p=='.') p++;
  if (*p=='/') // ie a Fraction
    {
    p++;
    d/=Max(0.0001, atof(p));
    };
  return d;
  }

//===========================================================================

static char *_PrjIniFileName        = "Scd_Prj.ini";
static char *_PrjAccFmtsIniFileName = "Scd_PrjAccessFmts.ini";
static char *_BackupFilesFolder     = "$Lc";
static char *_FlwSymFolder          = "Symbols";//"FlwSym"; changed in Build113
static char *_FontsFolder           = "Fonts";

//static char *_OldCfgDataFolder      = "$CfgFiles";
static char *_DefCfgDataFolder      = "CfgFiles";
//static char *_OldCfgDBFile          = "$SysCAD.MDB";
static char *_CfgDBFile             = "SysCAD.MDB";

static char *_OldBCfgDataFolder     = "$CfgFiles";
static char *_BCfgDataFolder        = "BaseFiles";
//static char *_OldBCfgDBFile         = "$SysCAD.MDB";
static char *_BCfgDBFile            = "Default.MDB";

static Strng _TxtEditor             = "notepad.exe";

static Strng _StartupDirectory      ;
static Strng _StartupDrive          ;
static Strng _ProgFiles             ;
static Strng _ExeFile               ;
static Strng _DllFilesPath          ;
static Strng _TemporaryFiles        ;
static Strng _BaseCfgFiles          ;

static flag  _UsePrevCfgAlias = false;
static flag  _UsePrevPrjAlias = false;
static Strng _CfgName               ;
static Strng _CfgFile               ;
static Strng _CfgFiles              ;
static Strng _PrjName               ;
static Strng _PrjFile               ;
static Strng _PrjFiles              ;
static Strng _PrjRmtCpyFiles        ;
static Strng _PrjIniFile            ;
static Strng _PrjAccFmtsIniFile     ;

static Strng _PrevCfgName;
static Strng _PrevCfgFile;
static Strng _PrevCfgFiles;
static Strng _PrevPrjName;
static Strng _PrevPrjFile;
static Strng _PrevPrjFiles;
static Strng _PrevPrjIniFile;

static Strng _GrfSymbolImportFiles;
static Strng _GrfSymbolFiles;
static Strng _GrfFontFiles;
static Strng _BaseGrfSymbolFiles;
static Strng _BaseGrfFontFiles;

pchar TxtEditor()
  {
  return _TxtEditor();
  }

pchar SetTxtEditor(pchar TE)
  {
  _TxtEditor=TE;
  return _TxtEditor();
  }

Strng TxtEditCmdString(char* Fn)
  {
  Strng X;
  X.Set("\"%s\" \"%s\"", TxtEditor(), Fn);
  return X;
  };

//---------------------------------------------------------------------------

char *PrjIniFileName()            { return _PrjIniFileName; };
char *PrjAccFmtsIniFileName()     { return _PrjAccFmtsIniFileName; };
char *BackupFilesFolderName()     { return _BackupFilesFolder; };
char *FlwSymFolderName()          { return _FlwSymFolder; };
char *FontsFolderName()           { return _FontsFolder; };

char *OldBCfgFolderName()         { return _OldBCfgDataFolder; };
char *BCfgFolderName()            { return _BCfgDataFolder; };
//char *OldBCfgDBFileName()         { return _OldBCfgDBFile; };
char *BCfgDBFileName()            { return _BCfgDBFile; };

//char *OldCfgFolderName()          { return _OldCfgDataFolder; };
char *DefCfgFolderName()          { return _DefCfgDataFolder; };
//char *OldCfgDBFileName()          { return _OldCfgDBFile; };
char *CfgDBFileName()             { return _CfgDBFile; };

//---------------------------------------------------------------------------

LPTSTR   PrjFile()                { return _UsePrevPrjAlias ? _PrevPrjFile() : _PrjFile(); };
LPTSTR   PrjFiles()               { return _UsePrevPrjAlias ? _PrevPrjFiles() : _PrjFiles(); };
char*    PrjFilesAlias()          { return "$Prj\\"; };
char*    PrjFilesAliasOld()       { return "$PrjFiles\\"; };
char*    SetPrjRmtCpyFiles(char * F)
  {
  _PrjRmtCpyFiles     = F;
  _PrjRmtCpyFiles.FnCheckEndBSlash();
  return _PrjRmtCpyFiles();
  };
char*    PrjRmtCpyFiles()         { return _PrjRmtCpyFiles(); };
char*    PrjRmtCpyFilesAlias()    { return "$RCFiles\\"; };
LPTSTR   PrjName()                { return _UsePrevPrjAlias ? _PrevPrjName() : _PrjName(); };
LPTSTR   PrjNameAlias()           { return "$PrjName"; };
char*    PrjIniFile()             { return _UsePrevPrjAlias ? _PrevPrjIniFile() : _PrjIniFile(); };
char*    PrjAccFmtsIniFile()      { return _PrjAccFmtsIniFile(); };

//---------------------------------------------------------------------------

char*  SetGrfSymbolImportFiles(char* FullPath) 
  { 
  _GrfSymbolImportFiles=FullPath; 
  _GrfSymbolImportFiles.FnCheckEndBSlash();
  return _GrfSymbolImportFiles(); 
  };
char*  GrfSymbolImportFiles() { return _GrfSymbolImportFiles(); };
char*  GrfSymbolFiles() { return _GrfSymbolFiles(); };
char*  GrfFontFiles() { return _GrfFontFiles(); };
char*  BaseGrfSymbolFiles() { return _BaseGrfSymbolFiles(); };
char*  BaseGrfFontFiles() { return _BaseGrfFontFiles(); };

//---------------------------------------------------------------------------

flag FnCreateTime(LPCTSTR pFn, FILETIME &Ft)
  {
  WIN32_FIND_DATA findFileData;
  HANDLE hFind = FindFirstFile((LPTSTR)pFn, &findFileData);
  if (hFind == INVALID_HANDLE_VALUE)
    return false;
  VERIFY(FindClose(hFind));

  Ft=findFileData.ftCreationTime;
  return true;
  };
flag FnModifyTime(LPCTSTR pFn, FILETIME &Ft)
  {
  WIN32_FIND_DATA findFileData;
  HANDLE hFind = FindFirstFile((LPTSTR)pFn, &findFileData);
  if (hFind == INVALID_HANDLE_VALUE)
    return false;
  VERIFY(FindClose(hFind));

  Ft=findFileData.ftLastWriteTime;
  return true;
  };
flag FnAccessTime(LPCTSTR pFn, FILETIME &Ft)
  {
  WIN32_FIND_DATA findFileData;
  HANDLE hFind = FindFirstFile((LPTSTR)pFn, &findFileData);
  if (hFind == INVALID_HANDLE_VALUE)
    return false;
  VERIFY(FindClose(hFind));

  Ft=findFileData.ftLastAccessTime;
  return true;
  };

//---------------------------------------------------------------------------

char* FnCheckEndBSlash(Strng & D)
  {
  if (D.GetLength()>0)
    if (D[D.GetLength()-1]!='\\')
      D+="\\";
  return D();
  };
char* FnClearEndBSlash(Strng & D)
  {
  if (D.GetLength()>0)
    if (D[D.GetLength()-1]=='\\')
      D.SetLength(D.GetLength()-1);
  return D();
  };
char* FnCheckExtension(Strng & D, char * E)
  {
  Strng Ext, RqdExt(E);
  Ext.FnExt(D());
  if (E[0]!='.')
    {
    RqdExt=".";
    RqdExt+=E;
    }
  else
    RqdExt=E;

  if (Ext.XStrICmp(RqdExt)!=0)
    D+=RqdExt;
  return D();
  };
LPCTSTR FnCheckExtension(CString & D, LPCTSTR E)
  {
  char Drv[_MAX_DRIVE];
  char Dir[_MAX_DIR];
  char Name[_MAX_FNAME];
  char Ext[_MAX_EXT];
  _splitpath(D.GetLength()?D:"", Drv, Dir, Name, Ext);
  if (int(*E)!='.')
    {
    strcpy(Ext, ".");
    strcat(Ext, E);
    }
  else
    strcpy(Ext, E);
  D=Drv;
  D+=Dir;
  D+=Name;
  D+=Ext;
  return D;
  };

//---------------------------------------------------------------------------

char* FnMakeDataFolder(LPTSTR F, Strng & D)
  {
  Strng Temp(D);
  Temp.FnClearEndBSlash();
  Temp.FnName(Temp());
  if (Temp.XStrICmp(F)==0)
    return  D();
  D.FnCheckEndBSlash();
  D+=F;
  D.FnCheckEndBSlash();
  return D();
  };

//---------------------------------------------------------------------------

char* FnCompactPath(long MaxLen, Strng & D)
  {
  char X[256];
  ASSERT(MaxLen<sizeof(X));
  PathCompactPathEx(X, D(), MaxLen, '\\');
  D=X;
  return D();
  };

//---------------------------------------------------------------------------

char* FnEscapeDotDotPath(Strng &D)
  {
  if (D())// && (_PrjRmtCpyFiles.Length()==0 || D.StrNICmp(_PrjRmtCpyFiles(), _PrjRmtCpyFiles.Length())!=0))
    {
    char F[_MAX_PATH*2];
    strcpy(F, D());
    char *p=F;
    while (p=strstr(p, "..\\"))
      {
      *(p+0)='~';
      if (0 && (strchr(p+3, '\\')))
        {
        // more folders
        *(p+1)=',';
        memmove(p+2, p+3, strlen(p+3)+1);
        }
      else
        {
        // no more folders
        memmove(p+1, p+2, strlen(p+2)+1);
        }
      }
    D=F;
    }
  return D();
  }

//---------------------------------------------------------------------------

char* FnUnEscapeDotDotPath(Strng &D)
  {
  if (D() && (_PrjRmtCpyFiles.Length()==0 || D.XStrNICmp(_PrjRmtCpyFiles(), _PrjRmtCpyFiles.Length())!=0))
  //if (D() && D.StrNICmp(_PrjRmtCpyFiles(), _PrjRmtCpyFiles.Length())!=0)
    {
    char F[_MAX_PATH*2];
    strcpy(F, D());
    char *p=F;
    while (p=strstr(p, "~,"))
      {
      *(p+0)='.';
      *(p+1)='.';
      memmove(p+3, p+2, strlen(p+2)+1);
      *(p+2)='\\';
      }
    p=F;
    while (p=strstr(p, "~\\"))
      {
      memmove(p+2, p+1, strlen(p+1)+1);
      *(p+0)='.';
      *(p+1)='.';
      }
    D=F;
    }
  return D();
  }

//---------------------------------------------------------------------------

char * FnRemoveDotDirs(Strng &Fn)
  {
  if (Fn() && (_PrjRmtCpyFiles.Length()==0 || Fn.XStrNICmp(_PrjRmtCpyFiles(), _PrjRmtCpyFiles.Length())!=0))
    {
    Strng Tmp(Fn);
    if (Fn[0]=='.' && Fn[1]=='\\')
      {
      //Relative Dir
      Fn=PrjFiles();
      while (Tmp.XStrNICmp(".\\", 2)==0)
        Tmp=Tmp.Right(Tmp.Length()-2); // Remove 2 leftmost chars
      Fn+=Tmp;
      }
    //else if (Fn[0]=='\\')
    else if (Fn[0]=='\\' && Fn[1]!='\\') //kga 05/2004 don't add drive for UNC path
      {
      // Was Same Drive - Change Drive
      Fn.FnDrive(PrjFiles());
      Fn+=Tmp;
      }
    char *p;
    while ((p=Fn.XStrStr("\\..\\"))!=NULL) // "\..\"
      {
      int L=4;
      int P=p-Fn();
      Tmp=Fn.Left(P);
      Tmp.FnDrivePath(); // Remove Prev Directory
      Tmp.FnCheckEndBSlash();
      Tmp+=Fn.Right(Fn.Length()-L-P);
      Fn=Tmp;
      }
    while ((p=Fn.XStrStr("\\.\\"))!=NULL) // "\.\"
      {
      int L=3;
      int P=p-Fn();
      Tmp=Fn.Left(P);
      Tmp.FnCheckEndBSlash();
      Tmp+=Fn.Right(Fn.Length()-L-P);
      Fn=Tmp;
      }
    }
  return Fn();
  }

//---------------------------------------------------------------------------


char* FnMakePrjFileRelative(Strng &Fn, flag ExpContract)
  {
  return FnMakeFileRelative(PrjFiles(), Fn, ExpContract);
  }

char* FnMakeFileRelative(char * RefPath, Strng &Fn, flag ExpContract)
  {
  if (!RefPath || !Fn())
    return Fn();
  if (ExpContract)
    Fn.FnExpand();
  Strng FP, FN;
  FP.FnDrivePath(Fn());
  FN.FnNameExt(Fn());
  char *p=RefPath;
  char *f=FP() ? FP() : "";
  int l=0;
  while (*p && *f && (tolower(*p)==tolower(*f)))
    { p++; f++; l++; };
  // *p & *f are different
  //kga 9/2/2001 : BUG this did not work when p=d:\syscad\adm\xxxxx and f=d:\syscad reports\xxxxx
  if (l>0 && *p=='\\' && *f!='\\') //kga 9/2/2001
    { p--; f--; l--; }; //kga 9/2/2001
  while (*p!='\\' && l>0)
    { p--; f--; l--; };
  // l is length of equal part
  if (l>0)
    {
    Strng N;
    N=RefPath;

    int ll=0;
    for (p++; *p; p++, ll++)
      {
      if (*p=='\\')
        {
        if (ll==2 && (_strnicmp(p, ".\\", 2)==0))
          {
          }
        else
          {
          N+="..\\";
          }
        ll=0;
        }

      };
    f++;
    if (strlen(f)>0)
      N+=f;
    N+=FN;
    Fn=N;
    }
  if (ExpContract)
    Fn.FnContract();
  return Fn();
  }

//---------------------------------------------------------------------------

char * FnMakePathInLocal(Strng & Fn)
  {
  if (Fn())
    {
    FnMakePrjFileRelative(Fn);

    int lFn=Fn.Length();
    int lPFAlias=strlen(PrjFilesAlias());
    int lRCAlias=strlen(PrjRmtCpyFilesAlias());

    if (_stricmp(Fn(), PrjFilesAlias())==0 ||
        _strnicmp(Fn(), PrjFilesAlias(), lPFAlias)==0 &&
        ((lFn>=lPFAlias+3 && _strnicmp(&Fn[lPFAlias], "..\\", 3)!=0) &&
         (lFn>=lPFAlias+2 && _strnicmp(&Fn[lPFAlias], "~\\", 2)!=0)))
      {
      // resides in the original project
      Strng D;//, Path;
      D=PrjFiles();
      D+=&Fn[lPFAlias];
      D.FnEscapeDotDotPath();
      Fn=D;
      }
    else if (Fn[0]=='$') // Is Contracted
      { // in RmtCpy
      Strng D;
      D=PrjRmtCpyFiles();
      if (D()==NULL || _strnicmp(Fn(), D(), D.Length())==0)
        D=Fn();// Already in RmtCpy
      else if (_strnicmp(Fn(), PrjRmtCpyFilesAlias(), lRCAlias)==0)
        D+=&Fn[lRCAlias];
      else if (_strnicmp(Fn(), PrjFilesAlias(), lPFAlias)==0)
        D+=&Fn[lPFAlias];
      else
        D+=Fn;
      D.FnEscapeDotDotPath();
      Fn=D;
      }
    }
  return Fn();
  }

//---------------------------------------------------------------------------

char*  SetStartupDirectory(char* PD)
  {
  _StartupDirectory=PD;
  _StartupDirectory.FnCheckEndBSlash();
  _StartupDrive.FnDrive(_StartupDirectory());
  _StartupDrive.FnCheckEndBSlash();
  return _StartupDirectory();
  };
char*  StartupDirectory() { return _StartupDirectory(); };
char*  StartupDrive() { return _StartupDrive(); };

//---------------------------------------------------------------------------

char*  SetBaseCfgFiles(char* PD)
  {
  _BaseCfgFiles=PD;
  _BaseCfgFiles.FnCheckEndBSlash();

  _BaseGrfSymbolFiles=_BaseCfgFiles;
  _BaseGrfSymbolFiles+=_FlwSymFolder;
  _BaseGrfSymbolFiles.FnCheckEndBSlash();
  _BaseGrfFontFiles=_BaseCfgFiles;
  _BaseGrfFontFiles+=_FontsFolder;
  _BaseGrfFontFiles.FnCheckEndBSlash();

  return _BaseCfgFiles();
  };
int TestBaseCfgFiles()
  {
  Strng T(BaseCfgFiles());
  T.FnClearEndBSlash();
  if (!FileExists(T()))
    return -1;
  return 0;
  };
char*  BaseCfgFiles() { return _BaseCfgFiles(); };
char*  BaseCfgFilesAlias() { return "$BaseCfgFiles\\"; };

//---------------------------------------------------------------------------

char*  SetProgFiles(char* PD)
  {
  _ProgFiles=PD;
  _ProgFiles.FnCheckEndBSlash();
  _DllFilesPath="";
  //_DllFilesPath=_ProgFiles();
  //_DllFilesPath+="Secure";
  //_DllFilesPath.FnCheckEndBSlash();
  return _ProgFiles();
  };
char*  ProgFiles() { return _ProgFiles(); };
char*  ProgFilesAlias() { return "$ProgFiles\\"; };

char*  SetExeFile(char* PD)
  {
  _ExeFile=PD;
  return _ExeFile();
  };
char*  ExeFile() { return _ExeFile(); };

char*  SetDllFilesPath(char* FP)
  {
  _DllFilesPath=FP;
  _DllFilesPath.ReplaceChars(",", ';');
  return _DllFilesPath();
  };
char*  ClrDllFilesPath()
  {
  _DllFilesPath="";
  return _DllFilesPath();
  };
char*  AddDllFilesPath(char* FP)
  {
  if (_DllFilesPath.Length()>0)
    _DllFilesPath+=";";
  _DllFilesPath+=FP;
  _DllFilesPath.ReplaceChars(",", ';');
  return _DllFilesPath();
  };
char*  DllFilesPath() { return _DllFilesPath(); };
int DllFilesFolder(int N, char * DllPath, Strng &Folder)
  {
  if (N==0)
    {
    Folder=ProgFiles();
    return 1;
    }

  int iP=1;
  int iStart=0;
  int L=DllPath ? strlen(DllPath) : 0;
  for (int i=0; i<L; i++)
    {
    flag AtEnd=(i==L-1);
    if (DllPath[i]==';' || AtEnd)
      {
      if (iP==N)
        {
        Folder=DllPath;
        Folder=Folder.Mid(iStart, DllPath[i]==';' ? i-iStart : i-iStart+1);
        if (Folder.Length()>0)
          {
          Folder.FnCheckEndBSlash();
          return 1;
          }
        else
          return 0;
        }
      iP++;
      iStart=i;
      }
    }

  if (N==iP)
    {
    Folder=ProgFiles();
    Folder+="Secure\\";
    return 1;
    }

  return -1;
  };

//---------------------------------------------------------------------------

HINSTANCE AFXAPI MyAfxLoadLibrary( LPCTSTR lpszModuleName, char * DllPath)
  {
  HINSTANCE hLib=NULL;

  //Strng T, P;
  Strng Dll(lpszModuleName);
  if (Dll[Dll.Length()-1]!='.')
    Dll.FnCheckExtension(".dll");

  Strng P(Dll);//lpszModuleName; // Strip off DirName
  P.FnDrivePath();
  if (P.Length()>0)
    return AfxLoadLibrary(Dll());

  Strng Secure(ProgFiles());
  Secure+="Secure\\";

  char OrgPathBuff[16384];
  char NewPathBuff[16384];
  OrgPathBuff[0]=0;
  NewPathBuff[0]=0;
  if (GetEnvironmentVariable("PATH", OrgPathBuff, sizeof(OrgPathBuff))<sizeof(OrgPathBuff))
    {
    if (DllFilesPath())
      {
      strcat(NewPathBuff, DllFilesPath());
      strcat(NewPathBuff, ";");
      }

    strcat(NewPathBuff, Secure());
    strcat(NewPathBuff, ";");

    strcat(NewPathBuff, OrgPathBuff);

    SetEnvironmentVariable("PATH", NewPathBuff);
    }

  //sMdlDLLPath.FnCheckEndBSlash();
  hLib=AfxLoadLibrary(Dll());
  //if (DllFilesPath() && OrgPathBuff[0])
  SetEnvironmentVariable("PATH", OrgPathBuff[0] ? OrgPathBuff : "");

//  Strng Folder;
//  for (int i=0; ; i++)
//    {
//    int ret=DllFilesFolder(i, DllPath, Folder);
//    if (ret==1)
//      {
//      Folder+=Dll;
//      hLib=AfxLoadLibrary(Folder());
//      if (hLib!=NULL)
//        goto Done;
//      }
//    else if (ret==0)
//      {
//      // Blank Folder
//      }
//    else if (ret==-1)
//      break;
//    }
//
//Done:
  return hLib;
  }

//---------------------------------------------------------------------------

char*  SetTemporaryFiles(char* PD)
  {
  _TemporaryFiles=PD;
  _TemporaryFiles.FnCheckEndBSlash();
  return _TemporaryFiles();
  };
char*  TemporaryFiles() { return _TemporaryFiles(); };

//---------------------------------------------------------------------------

flag SetUsePreviousCfgAlias(flag On) { flag Was=_UsePrevCfgAlias ; _UsePrevCfgAlias = On; return Was; };
flag SetUsePreviousPrjAlias(flag On) { flag Was=_UsePrevPrjAlias ; _UsePrevPrjAlias = On; return Was; };

//---------------------------------------------------------------------------

char*  SetCfgFile(char* PD)
  {
  _PrevCfgName  = _CfgName;
  _PrevCfgFile  = _CfgFile;
  if (PD==NULL)
    {
    _CfgFile="";
    _CfgName="";
    return "";
    }
  _CfgFile=PD;
  _CfgName.FnName(_CfgFile());

  return _CfgFile();
  };

//---------------------------------------------------------------------------

char*  SetCfgFiles(char* PD)
  {
  _PrevCfgFiles = _CfgFiles;

  if (PD==NULL)
    {
    _CfgFiles="";
    return "";
    }
  _CfgFiles=PD;
  _CfgFiles.FnClearEndBSlash();
  _CfgFiles.FnCheckEndBSlash();

  _GrfSymbolFiles=_CfgFiles;
  _GrfSymbolFiles+=_FlwSymFolder;
  _GrfSymbolFiles.FnCheckEndBSlash();
  _GrfSymbolImportFiles="";
  _GrfFontFiles=_CfgFiles;
  _GrfFontFiles+=_FontsFolder;
  _GrfFontFiles.FnCheckEndBSlash();

  return _CfgFiles();
  };

//---------------------------------------------------------------------------

int CheckCfgFilesName()
  {
  Strng Fn(CfgFiles());
  Fn.FnClearEndBSlash();
  if (!FileExists(Fn()))
    {
    Strng Fp(Fn());
    Fn.FnName();
    if (Fn.XStrICmp("$CfgFiles")==0)
      {
      Strng FpO;
      FpO.FnDrivePath(Fp());
      FpO+="_CfgData";
      if (FileExists(FpO()) && !MoveFile(FpO(), Fp()))
        {
        LogError("Project",0, "%s not renamed to %s",FpO(), Fp());
        return false;
        }
      }
    }
  return true;
  }

int TestCfgFiles()
  {
  int Ret=0;

  Strng BT(BaseCfgFiles());
  BT.FnClearEndBSlash();
  if (!FileExists(BT()))
    Ret|=0x1;

  Strng T(CfgFiles());
  T.FnClearEndBSlash();
  if (!FileExists(T()))
    Ret|=0x2;

  return Ret;
  };

char*  CfgFile()  { return _UsePrevCfgAlias ? _PrevCfgFile() : _CfgFile(); };
char*  CfgFiles() { return _UsePrevCfgAlias ? _PrevCfgFiles() : _CfgFiles(); };
char*  CfgFilesAlias() { return "$CfgFiles\\"; };
char*  CfgName()  { return _UsePrevCfgAlias ? _PrevCfgName() : _CfgName(); };
char*  CfgNameAlias() { return "$CfgName"; };

//---------------------------------------------------------------------------

char*  SetPrjFiles(char* PD, char* PF)
  {
  _PrevPrjName            = _PrjName;
  _PrevPrjFile            = _PrjFile;
  _PrevPrjFiles           = _PrjFiles;
  _PrevPrjIniFile         = _PrjIniFile;

  if (PD==NULL)
    {
    _PrjFile="";
    _PrjFiles="";
    _PrjName="";
    if (!SetCurrentDirectory(StartupDirectory()))
      {
      DWORD E=GetLastError();
      LogError("SysCAD", 0, "Cannot SetCurrentFolder %s", StartupDirectory());
      }
    _PrjIniFile="";
    _PrjAccFmtsIniFile="";
    return "";
    }

  _PrjFile=PF;
  _PrjFiles=PD;
  _PrjFiles.FnClearEndBSlash();
  _PrjName.FnName(_PrjFiles());
  Strng Dir(_PrjFiles);
  Dir+="\\.";
  if (!FileExists(Dir()) && !CreateDirectory(_PrjFiles(), NULL))
    {
    DWORD E=GetLastError();
    LogError("SysCAD", 0, "Cannot Create %s", _PrjFiles());
    }

  if (!SetCurrentDirectory(_PrjFiles()))
    {
    DWORD E=GetLastError();
    LogError("SysCAD", 0, "Cannot SetCurrentFolder %s", _PrjFiles());
    //DoBreak();
    }

  _PrjFiles.FnCheckEndBSlash();

  _PrjIniFile=_PrjFiles;
  _PrjIniFile+=_PrjIniFileName;

  _PrjAccFmtsIniFile=_PrjFiles;
  _PrjAccFmtsIniFile+=_PrjAccFmtsIniFileName;

  return _PrjFiles();
  };

//---------------------------------------------------------------------------

char * AF_FilesMasked(byte m)
  {
  switch (m)
    {
    case AF_PrjFiles:        return PrjFiles      ();
    case AF_CfgFiles:        return CfgFiles      ();
    case AF_BaseCfgFiles:    return BaseCfgFiles  ();
    case AF_ProgFiles:       return ProgFiles     ();
    case AF_RmtCpyFiles:     return PrjRmtCpyFiles();
    default:                 return NULL;
    };
  }

//---------------------------------------------------------------------------

char * AF_FilesIndexed(int i)
  {
  switch (AF_Mask(i))
    {
    case AF_PrjFiles:        return PrjFiles      ();
    case AF_CfgFiles:        return CfgFiles      ();
    case AF_BaseCfgFiles:    return BaseCfgFiles  ();
    case AF_ProgFiles:       return ProgFiles     ();
    case AF_RmtCpyFiles:     return PrjRmtCpyFiles();
    default:                 return NULL;
    };
  }

//---------------------------------------------------------------------------

char * AF_AliasIndexed(int i)
  {
  switch (AF_Mask(i))
    {
    case AF_PrjFiles:        return PrjFilesAlias      ();
    case AF_CfgFiles:        return CfgFilesAlias      ();
    case AF_BaseCfgFiles:    return BaseCfgFilesAlias  ();
    case AF_ProgFiles:       return ProgFilesAlias     ();
    case AF_Elsewhere:       return "Elsewhere";
    default:                 return NULL;
    };
  }

//---------------------------------------------------------------------------

struct sElem { int Where; char * Find; char * Rep; };

static byte DoFndRep(Strng &Fn, sElem * AA)
  {
  //dbgpln("DRP               :%s",Fn());
  int iMatch=-1;
  int MatchLen=0;
  for (int i=0; AA[i].Where; i++)
    if (AA[i].Find)
      {
      char*p=AA[i].Find;
      int Len=strlen(p);
      if (Fn.XStrNICmp(p, Len)==0)
        if (Len>MatchLen)
          {
          iMatch=i;
          MatchLen=Len;
          }
      }

  if (iMatch>=0)
    {
    char*p=AA[iMatch].Find;
    int Len=strlen(p);
    Strng T=AA[iMatch].Rep;
    T+=Fn.Right(Fn.Length()-Len);
    Fn=T;
    //dbgpln("                %02x:%s",AA[iMatch].Where,Fn());
    return AA[iMatch].Where;
    }
  //dbgpln("                %02x:%s",0,Fn());
  return AF_Elsewhere;
  }

//---------------------------------------------------------------------------

byte FnWhereIsFile(Strng &Fn, char *pFn/*=NULL*/, flag TestExisting/*=1*/)
  {
  if (pFn)
    Fn=pFn;
  byte Where=0;
  int MatchLen=0;
  for (int i=0; ;i++)
    {
    char * pA=AF_FilesIndexed(i);
    if (!pA)
      break;
    int Len=strlen(pA);
    if (Fn.XStrNICmp(pA, Len)==0)
      if (Len>MatchLen)
        {
        MatchLen=Len;
        Where=AF_Mask(i);
        }
    }
  if (Where && TestExisting)
    {
    if (!FileExists(Fn()))
      Where=0;
    }
  else
    {
    if (FileExists(Fn()))
      Where=AF_Elsewhere;
    }

  return Where;
  }

//---------------------------------------------------------------------------
// Will try to work out where the file is and return
// a reduced path to it
char* FnContract(Strng & Fn, char *pFn, flag Complete)
  {
  if (pFn)
    Fn=pFn;
  if (Fn.Len()==0)
    return Fn();

  byte W;
  if (PrjFileVerNo()>=29)
    {
    FnMakePrjFileRelative(Fn);
    FnEscapeDotDotPath(Fn);
    sElem AA1[]=
      {
        {AF_PrjFiles,       PrjFiles(),           PrjFilesAlias()       },
        {AF_RmtCpyFiles,    PrjRmtCpyFiles(),     PrjRmtCpyFilesAlias() },
        {0}
      };
    W=DoFndRep(Fn, AA1);
    }
  else
    {
    sElem AA0[]=
      {
      // Dont contract back to old Paths
        //{AF_BackupFiles,    PrjBackupFiles(),     ""},//PrjBackupFilesAlias() },
        {AF_PrjFiles,       PrjFiles(),           PrjFilesAlias()       },
        //{AF_CfgFiles,       CfgFiles(),           CfgFilesAlias()       },
        //{AF_BaseCfgFiles,   BaseCfgFiles(),       BaseCfgFilesAlias()   },
        //{AF_ProgFiles,      ProgFiles(),          ProgFilesAlias()      },
        {0}
      };
    W=DoFndRep(Fn, AA0);
    }

  if (Complete && Fn.Len()>=2 && Fn[1]==':')
    {
    Fn[1]=Fn[0];
    Fn[0]='$';
    }
#if dbgFnTools
  dbgpln("                   FnContract         : %s", pFn ? pFn:Fn() , Fn());
  dbgpln("                                      > %s", Fn());
#endif
  return Fn();
  }

//---------------------------------------------------------------------------
// Will try to work out where the file is and return
// a reduced path to it
char* FnContractPath(Strng & Fn, char *pFn, char * pPath, char * pAlias, flag Complete)
  {
  if (pFn)
    Fn=pFn;
  if (Fn.Len()==0)
    return Fn();

  byte W;
  FnMakeFileRelative(pPath, Fn);
  FnEscapeDotDotPath(Fn);
  sElem AA1[]=
    {
      {1,       pPath,           pAlias},
      {0}
    };
  W=DoFndRep(Fn, AA1);

  if (Complete && Fn.Len()>=2 && Fn[1]==':')
    {
    Fn[1]=Fn[0];
    Fn[0]='$';
    }
#if dbgFnTools
  dbgpln("                   FnContractPath     : %s", pFn ? pFn:Fn() , Fn());
  dbgpln("                                      > %s", Fn());
#endif
  return Fn();
  }

//---------------------------------------------------------------------------

char* FnContractDO(Strng & Fn, char *pFn)
  {
  if (pFn)
    Fn=pFn;
  if (Fn.Length()>=2 && Fn[1]==':')
    {
    Fn[1]=Fn[0];
    Fn[0]='$';
    }
#if dbgFnTools
  dbgpln("                   FnContractDO       : %s", pFn ? pFn:Fn() , Fn());
  dbgpln("                                      > %s", Fn());
#endif
  return Fn();
  }

//---------------------------------------------------------------------------

char* FnExpand(Strng & Fn, char * pFn)
  {
  if (pFn)
    Fn=pFn;

  if (UsingPrjLclFiles() && (PrjFileVerNo()>=29))
    {
    FnUnEscapeDotDotPath(Fn);
    FnMakePathInLocal(Fn);
    }
  else
    {
    sElem AA[]=
      {
        //{AF_BackupFiles,    PrjBackupFilesAlias(),  PrjBackupFiles() },
        {AF_PrjFiles,       PrjFilesAlias(),        PrjFiles()          },
        {AF_RmtCpyFiles,    PrjRmtCpyFilesAlias(),  PrjRmtCpyFiles()    },
        {AF_PrjFiles,       PrjFilesAliasOld(),     PrjFiles()          },
        {AF_CfgFiles,       CfgFilesAlias(),        CfgFiles()          },
        {AF_BaseCfgFiles,   BaseCfgFilesAlias(),    BaseCfgFiles()      },
        {AF_ProgFiles,      ProgFilesAlias(),       ProgFiles()         },
        {0}
      };
    byte W=DoFndRep(Fn, AA);
    FnUnEscapeDotDotPath(Fn);
    FnRemoveDotDirs(Fn);
    if (Fn.Length()>=3 && Fn[0]=='$' && isalpha(Fn[1]) && Fn[2]=='\\')
      {
      Fn[0]=Fn[1];
      Fn[1]=':';
      }
    }
#if dbgFnTools
  dbgpln("                   FnExpand           : %s", pFn ? pFn:Fn() , Fn());
  dbgpln("                                      > %s", Fn());
#endif
  return Fn();
  }

//---------------------------------------------------------------------------

char* FnExpandPath(Strng & Fn, char * pFn, char * pPath, char * pAlias)
  {
  if (pFn)
    Fn=pFn;

  sElem AA[]=

    {
      {1,        pAlias,       pPath},
      {0}
    };
  byte W=DoFndRep(Fn, AA);
  FnUnEscapeDotDotPath(Fn);
  FnRemoveDotDirs(Fn);
  if (Fn.Length()>=3 && Fn[0]=='$' && isalpha(Fn[1]) && Fn[2]=='\\')
    {
    Fn[0]=Fn[1];
    Fn[1]=':';
    }
#if dbgFnTools
  dbgpln("                   FnExpandPath       : %s", pFn ? pFn:Fn() , Fn());
  dbgpln("                                      > %s", Fn());
#endif
  return Fn();
  }

//---------------------------------------------------------------------------

//try find the file in the specified places. (if found return full path and filename)
static byte FnSearch(Strng & D, char* pFn, byte Places)
  {
#if dbgFnTools
  dbgpln("                   FnSearch           : %s", pFn ? pFn:D() , D());
#endif
  Strng Fn(pFn ? pFn : D());
  if (!Fn())
    {
    D="";
#if dbgFnTools
    dbgpln("                                    0 > %s", D());
#endif
    return 0;
    }


  if (Fn())
    {
    byte LCMask= Places & AF_BackupFiles;
    for (byte m=AF_First; m<=AF_Last; m=m<<1)
      if (m & Places)
        {
        D= AF_FilesIndexed(AF_Index(m));
        D+= Fn();
        if (LCMask)
          {
          Strng FnLcl;
          FnLcl.FnContract(D(), true);
          D=PrjRmtCpyFiles();
          D+=FnLcl;
          }

        if (FileExists(D()))
          {
#if dbgFnTools
          dbgpln("                                    1 > %s", D());
#endif
          return m|LCMask;
          }
        }

    D=Fn;
    if (Places & AF_Elsewhere)
      {
      if (LCMask)
        {
        Strng FnLcl;
        FnLcl.FnContract(D(), true);
        D=PrjRmtCpyFiles();
        D+=FnLcl;
        }
      if (FileExists(D()))
        {
#if dbgFnTools
        dbgpln("                                    2 > %s", D());
#endif
        return AF_Elsewhere|LCMask;
        }
      }
    }
  else
    D=Fn;
#if dbgFnTools
  dbgpln("                                    3 > %s", D());
#endif
  return 0; //not found
  }

//---------------------------------------------------------------------------

//try find the file in the specified places. (if found return full path and filename)
static byte FnSearch(Strng & D, char* pFn, byte *Places)
  {
  Strng Fn(pFn ? pFn : D());
  if (!Fn())
    {
    D="";
    return 0;
    }
//  if (Fn())
//    {
  for (int i=0; Places[i]; i++)
    {
    byte LCMask= Places[i] & AF_BackupFiles;
    for (byte m=AF_First; m<=AF_Last; m=m<<1)
      if (m & Places[i])
        {
        D= AF_FilesIndexed(AF_Index(m));
        D+= Fn();
        if (LCMask)
          {
          Strng FnLcl;
          FnLcl.FnContract(D(), true);
          D=PrjRmtCpyFiles();
          D+=FnLcl;
          }
        if (FileExists(D()))
          return m|LCMask;
        }

    D=Fn;
    if (Places[i] & AF_Elsewhere)
      {
      if (LCMask)
        {
        Strng FnLcl;
        FnLcl.FnContract(D(), true);
        D=PrjRmtCpyFiles();
        D+=FnLcl;
        }
      if (FileExists(D()))
        return AF_Elsewhere|LCMask;
      }

    }
//    }
//  else
//    D=Fn;
  return 0; //not found
  }

//---------------------------------------------------------------------------

char* FnSearchContract(Strng & D, char * pFn, byte Places, flag Complete)
  {
  Strng Fn(pFn ? pFn : D());
  byte Where=FnSearch(D, Fn(), Places);
  if (Where)
    D.FnContract(D(), Complete);
  else
    D=Fn;
#if dbgFnTools
  dbgpln("                   FnSearchContract   : %s", pFn ? pFn:Fn() , Fn());
  dbgpln("                                      > %s", Fn());
#endif
  return D();
  };

//---------------------------------------------------------------------------
static byte _FnTestLocalFilesOn=0;
static Strng_List _FnLocalFilesDifferent;

void FnSetTestLocalFiles(flag On)         { _FnTestLocalFilesOn=On; };
long FnLocalFilesDifferentCount()         { return _FnLocalFilesDifferent.Length(); };
void FnLocalFilesDifferentClear()         { _FnLocalFilesDifferent.Clear(); }
Strng_List & FnLocalFilesDifferentList()  { return _FnLocalFilesDifferent; };

bool FnLocalCompareFiles(LPCTSTR SymFullFileRmt, LPCTSTR FullFileRmt, LPCTSTR FullFileLcl)
  {
  if (_stricmp(FullFileRmt, FullFileLcl)==0)
    return true; //Rmt and Lcl files are the same

  Strng RCFolder=PrjRmtCpyFiles();
  RCFolder.FnExpand();
  WIN32_FIND_DATA  FindFileData;
  HANDLE H=FindFirstFile(RCFolder(), &FindFileData);
  if (H!=INVALID_HANDLE_VALUE)
    FindClose(H);
  else
    return true; //$LC folder not found, assume all is well and continue...

  FILETIME FtRmt, FtLcl;
  BOOL OkRmt, OkLcl;
  if ((OkRmt=FnModifyTime(FullFileRmt, FtRmt)))
    {
    //CTime time(FtRmt);
    //m_TimeRmt=time.Format("%#c");
    }
  if ((OkLcl=FnModifyTime(FullFileLcl, FtLcl)))
    {
    //CTime time(FtLcl);
    //m_TimeLcl=time.Format("%#c");
    }

  //Strng m_Note="";
  byte iIndex=LCF_Same;
  bool Same=false;
  //bool OldRmt=false;
  //bool OldLcl=false;
  Strng TxtRmt, TxtLcl;
  if (OkRmt && OkLcl)
    {
    //OldRmt=(CompareFileTime(&FtRmt, &FtLcl)<0);
    //OldLcl=(CompareFileTime(&FtLcl, &FtRmt)<0);
    bool NewLcl=(CompareFileTime(&FtRmt, &FtLcl)<0);
    bool NewRmt=(CompareFileTime(&FtLcl, &FtRmt)<0);
    Same = !NewRmt && !NewLcl;
    if (Same)
      return true;
    else if (NewRmt)
      iIndex=LCF_RmtChanged;
    else
      iIndex=LCF_LclChanged;
    }
  else
    {
    if (!OkRmt)
      iIndex|=LCF_RmtMissing;
    if (!OkLcl)
      iIndex|=LCF_LclMissing;
    }

  Strng X=FullFileRmt;
  if (_FnLocalFilesDifferent.Find(X)==NULL)
    {
    X.SetIndex(iIndex);
    _FnLocalFilesDifferent.Append(X);
    }
  return false;
  }

//---------------------------------------------------------------------------
//#endif
//---------------------------------------------------------------------------

static byte _FnSetSearchDlgWhere=0;

void FnSetSearchDlg(byte Where)
  {
  //_FnSetSearchDlg=On;
  _FnSetSearchDlgWhere=Where;
  if (Where)
    CFnOptDlg::m_AllOK=false;
  };

void FnSkipSearchDlg(flag DoSkip)
  {
  CFnOptDlg::m_SkipDlg=DoSkip;
  };

int FnSearchDlg(Strng &F1, Strng &F2, char * D1, char* D2)
  {
  if (FileExists(F2()))
    {
    CFnOptDlg FODlg(AfxGetMainWnd());
    FODlg.m_FileRmt=D1 ? D1 : F1();
    FODlg.m_FullFileRmt=F1();
    FODlg.m_FileLcl=D2 ? D2 : F2();
    FODlg.m_FullFileLcl=F2();

    if (FODlg.DoModal()==IDOK)
      {
      F1=(char*)(const char*)FODlg.m_FullFileUse;
      return 1;
      }
    else
      return -1;
    }
  return 0;
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

char* FnSearchExpand(Strng & D, char * pFn/*=NULL*/, byte Places/*=AF_All*/, byte PlaceIfMissing/*=0*/)
  {
  byte Where=0;
  Strng Fn(pFn ? pFn : D());
  Strng Path, Name;
  Path.FnDrivePath(Fn());
  Name.FnNameExt(Fn());
  if (Path()==NULL)
    FnSearch(D, Name(), Places & ~AF_BackupFiles);
  else
    FnExpand(D, Fn());

  if (!FileExists(D()) && (Places & AF_BackupFiles))
    {
    Strng T1(BackupFilesFolderName());
    Strng T2(D());
    T1.Upper();
    T2.Upper();
    if (T2.XStrStr(T1())==NULL)
      {
      Strng DD;
      FnContractDO(DD, Fn());
      D= AF_FilesIndexed(AF_Index(AF_BackupFiles));
      D+=DD;
      }
    }

  if (_FnTestLocalFilesOn && (Places & AF_BackupFiles))
    {
    Strng DLcl;
    if (Path()==NULL)
      {
      }
    else
      {
      Strng FnLcl, X;
      if (PrjFileVerNo()>=29)
        {
        FnLcl.FnContract(Fn(), true);
        DLcl=FnLcl();
        FnMakePathInLocal(DLcl);

        if (FileExists(DLcl()))
          FnLocalCompareFiles(FnLcl(), D(), DLcl());

        }
      else
        {
        FnLcl.FnContract(Fn(), true);
        DLcl=PrjRmtCpyFiles();
        DLcl+=FnLcl;

        if (FileExists(DLcl()))
          {
          CFnOptDlg FODlg(AfxGetMainWnd());
          FODlg.m_FileRmt=D();
          FODlg.m_FullFileRmt=D();
          FODlg.m_FileLcl=FnLcl();
          FODlg.m_FullFileLcl=DLcl();

          if (FODlg.DoModal()==IDOK)
            D=(char*)(const char*)FODlg.m_FullFileUse;
          }
        }
      }
    }

  Where=FnWhereIsFile(D, NULL, true);
  if ((Where==0) && PlaceIfMissing)
    {
    Strng DD;
    DD.FnNameExt(Fn());
    D = AF_FilesIndexed(AF_Index(PlaceIfMissing));
    D+=DD;
    Where=PlaceIfMissing;
    }
  else if (Where && (Where & Places)==0)
    Where=0;
  if (Where)
    D.FnExpand();
  else
    D=Fn;

  return D();
  };

//---------------------------------------------------------------------------
// From Sample Compress

HANDLE OpenFileForCompress( IN      PTCHAR      ptcFile )
/*++

Routine Description:

    This routine jumps through the hoops necessary to open the file
    for READ_DATA|WRITE_DATA even if the file has the READONLY
    attribute set.

Arguments:

    ptcFile     - Specifies the file that should be opened.

Return Value:

    A handle open on the file if successfull, INVALID_HANDLE_VALUE
    otherwise, in which case the caller may use GetLastError() for more
    info.

--*/
{
    BY_HANDLE_FILE_INFORMATION fi;
    HANDLE hRet;
    HANDLE h;
    INT err;

    hRet = CreateFile(
                ptcFile,
                FILE_READ_DATA | FILE_WRITE_DATA,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                FILE_FLAG_BACKUP_SEMANTICS,
                NULL
                );

    if (INVALID_HANDLE_VALUE != hRet) {
        return hRet;
    }

    if (ERROR_ACCESS_DENIED != GetLastError()) {
        return INVALID_HANDLE_VALUE;
    }

    err = GetLastError();

    h = CreateFile(
            ptcFile,
            FILE_READ_ATTRIBUTES | FILE_WRITE_ATTRIBUTES,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL
            );

    if (INVALID_HANDLE_VALUE == h) {
        return INVALID_HANDLE_VALUE;
    }

    if (!GetFileInformationByHandle(h, &fi)) {
        CloseHandle(h);
        return INVALID_HANDLE_VALUE;
    }

    if ((fi.dwFileAttributes & FILE_ATTRIBUTE_READONLY) == 0) {

        // If we couldn't open the file for some reason other than that
        // the readonly attribute was set, fail.

        SetLastError(err);
        CloseHandle(h);
        return INVALID_HANDLE_VALUE;
    }

    fi.dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;

    if (!SetFileAttributes(ptcFile, fi.dwFileAttributes)) {
        CloseHandle(h);
        return INVALID_HANDLE_VALUE;
    }

    hRet = CreateFile(
            ptcFile,
            FILE_READ_DATA | FILE_WRITE_DATA,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL
            );

    CloseHandle(h);

    if (INVALID_HANDLE_VALUE == hRet) {
        return INVALID_HANDLE_VALUE;
    }

    fi.dwFileAttributes |= FILE_ATTRIBUTE_READONLY;

    if (!SetFileAttributes(ptcFile, fi.dwFileAttributes)) {
        CloseHandle(hRet);
        return INVALID_HANDLE_VALUE;
    }

    return hRet;
}
//---------------------------------------------------------------------------

flag FnCreatePath(char * pPath, Strng &E)
  {
  Strng Path(pPath), Tmp;
  Strng_List Paths;
  Path.FnClearEndBSlash();
  while (Path() && !FileExists(Path()))
    {
    Paths.Append(Path());
    Path.FnDrivePath();
    Path.FnClearEndBSlash();
    }

  if (!Path())
    return false;
  for (pStrng p=Paths.Last(); p; p=Paths.Prev())
    if (CreateDirectory(p->Str(), NULL))
      {
      Tmp.FnName(p->Str());
      if (Tmp.XStrICmp(BackupFilesFolderName())==0)
        {
        Tmp=p->Str();
        Tmp+="\\.";

        HANDLE FileHandle;
        USHORT State = 1;
        ULONG Length;

        FileHandle = OpenFileForCompress(Tmp());

        if (INVALID_HANDLE_VALUE != FileHandle)
          {
          if (DeviceIoControl(FileHandle, FSCTL_SET_COMPRESSION, &State,
              sizeof(USHORT), NULL, 0, &Length, FALSE ))
            {
            int xxxx=0;
            }

          CloseHandle( FileHandle );
          }
        }
      }
    else
      {
      Strng E;
      E.GetSystemErrorStr(GetLastError());
//      LogError("SysCAD", LF_Exclamation,
//          "Folder not created :\n%s\n%s",D(),E());
      return false;
      }
  return true;
  }

//---------------------------------------------------------------------------

flag FnCreateDirectory(char * pPath, flag Compress)
  {
  Strng Fn(pPath);
  Fn.FnClearEndBSlash();
  Strng FnD(Fn);
  FnD+="\\.";
  if (FileExists(FnD()))
    return true;

  //Check Recursively that Parents Exist
  Strng FnParent(Fn);
  FnParent.FnDrivePath();
  FnParent.FnClearEndBSlash();
  FnParent+="\\.";
  if (!FileExists(FnParent()))
    {
    FnParent.FnDrivePath();
    if (!FnCreateDirectory(FnParent(), Compress))
      return false;
    }



  if (CreateDirectory(Fn(), NULL))
    {
    //Tmp.FnName(p->Str());
    if (Compress)
      {
      //Tmp=p->Str();
      Fn+="\\.";

      HANDLE FileHandle;
      USHORT State = 1;
      ULONG Length;

      FileHandle = OpenFileForCompress(Fn());

      if (INVALID_HANDLE_VALUE != FileHandle)
        {
        if (DeviceIoControl(FileHandle, FSCTL_SET_COMPRESSION, &State,
            sizeof(USHORT), NULL, 0, &Length, FALSE ))
          {
          int xxxx=0;
          }

        CloseHandle( FileHandle );
        }
      }
    return true;
    }
  return false;
  };

//---------------------------------------------------------------------------

flag FnExpandQueryReload(Strng &D, char * pFn, const FILETIME & Ft)
  {
  bool OK=false;
  FnExpand(D, pFn);
  FILETIME Tm;
  if (FnModifyTime(D(), Tm))
    {
    if (CompareFileTime(&Tm, &Ft)>0)
      OK=true;
    }
#if dbgFnTools
  dbgpln("                   FnExpandQueryReload: %s", pFn ? pFn:D() , D());
  dbgpln("                                      > %s", D());
#endif
  return OK;
  };

//---------------------------------------------------------------------------

int SelectFile(Strng &FilePath, char* Name1, char* Name2, char* Name3)
  {
  Strng FullFile(FilePath);
  FullFile.FnCheckEndBSlash();
  Strng FullFileT(FullFile);
  if (Name1)
    {
    FullFile=FullFileT;
    FullFile+=Name1;
    if (FileExists(FullFile()))
      {
      FilePath=FullFile;
      return 1;
      }
    }
  if (Name2)
    {
    FullFile=FullFileT;
    FullFile+=Name2;
    if (FileExists(FullFile()))
      {
      FilePath=FullFile;
      return 2;
      }
    }
  if (Name3)
    {
    FullFile=FullFileT;
    FullFile+=Name3;
    if (FileExists(FullFile()))
      {
      FilePath=FullFile;
      return 3;
      }
    }
  return 0;
  };

//---------------------------------------------------------------------------

BOOL FileExists(LPCTSTR FileName, WIN32_FIND_DATA & fd)
  {
  HANDLE H;
  if ((H=FindFirstFile(FileName, &fd))!=INVALID_HANDLE_VALUE)
    {
    FindClose(H);
    return true;
    }
  return false;
  };

//---------------------------------------------------------------------------

BOOL FileExists(LPCTSTR FileName)
  {
  WIN32_FIND_DATA fd;
  return FileExists(FileName, fd);
  };

//---------------------------------------------------------------------------

//FILE_ATTRIBUTE_ARCHIVE The file or directory is an archive file or directory. 
//Applications use this attribute to mark files for backup or removal.
// 
//FILE_ATTRIBUTE_COMPRESSED The file or directory is compressed. 
//For a file, this means that all of the data in the file is compressed.
//
//For a directory, this means that compression is the default for newly created files and subdirectories.
// 
//FILE_ATTRIBUTE_DIRECTORY The handle identifies a directory. 
//FILE_ATTRIBUTE_ENCRYPTED The file or directory is encrypted. 
//For a file, this means that all data in the file is encrypted.
//
//For a directory, this means that encryption is the default for newly created files and subdirectories.
// 
//FILE_ATTRIBUTE_HIDDEN The file or directory is hidden. 
//It is not included in an ordinary directory listing.
// 
//FILE_ATTRIBUTE_NORMAL The file or directory does not have another attributes set. 
//This attribute is valid only if used alone.
// 
//FILE_ATTRIBUTE_OFFLINE The file data is not immediately available. 
//This attribute indicates that the file data has been physically moved to offline storage.
//
//This attribute is used by Remote Storage, the hierarchical storage management software.
//
//Applications should not arbitrarily change this attribute.
// 
//FILE_ATTRIBUTE_READONLY The file or directory is read-only. 
//Applications can read the file, but cannot write to it or delete it.
//
//For a directory, applications cannot delete it.
// 
//FILE_ATTRIBUTE_REPARSE_POINT The file or directory has an associated reparse point. 
//FILE_ATTRIBUTE_SPARSE_FILE The file is a sparse file. 
//FILE_ATTRIBUTE_SYSTEM The file or directory is part of the operating system, or is used exclusively by the operating system. 
//FILE_ATTRIBUTE_TEMPORARY 

BOOL FileAttributes(LPCTSTR FileName, DWORD & Attributes)
  {
  WIN32_FIND_DATA fd;
  HANDLE H;
  if ((H=FindFirstFile(FileName, &fd))!=INVALID_HANDLE_VALUE)
    {
    Attributes=fd.dwFileAttributes;
    FindClose(H);
    return true;
    }
  return false;
  };

BOOL FileWriteTime(LPCTSTR FileName, FILETIME & FileTime)
  {
  WIN32_FIND_DATA fd;
  HANDLE H;
  if ((H=FindFirstFile(FileName, &fd))!=INVALID_HANDLE_VALUE)
    {
    FileTime=fd.ftLastWriteTime;
    FindClose(H);
    return true;
    }
  return false;
  };

//---------------------------------------------------------------------------

BOOL CnmVersion()
  {
#if ALLOWCNMVERSION       
  Strng CnmVersionFile(TemporaryFiles());
  CnmVersionFile+="CNMVERSION.TXT";
  return FileExists(CnmVersionFile());
#else
  return false;
#endif
  }

//---------------------------------------------------------------------------

BOOL Copy_File(char* pFN, char* pDestDir)
  {
  Strng s(pDestDir), s1;
  s.FnCheckEndBSlash();
  s1.FnNameExt(pFN);
  s += s1;
  return CopyFile(pFN, s(), false);
  }

//---------------------------------------------------------------------------

BOOL Move_FileEx(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, DWORD dwFlags, LPTSTR pMsgBuff, DWORD MsgBuffLen)
  {
  if (IsWinNT)
    {
    BOOL Ret=::MoveFileEx(lpExistingFileName, lpNewFileName, dwFlags);
    if (pMsgBuff)
      {
      FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, 
                     NULL, 
                     GetLastError(), 
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), 
                     pMsgBuff, MsgBuffLen, NULL ); 
      }
    return Ret;
    }
  BOOL bFailIfExists = (dwFlags & MOVEFILE_REPLACE_EXISTING) ? false : true;
  BOOL Ok = CopyFile(lpExistingFileName, lpNewFileName, (dwFlags & MOVEFILE_REPLACE_EXISTING) ? false : true);
  if (Ok)
    Ok = DeleteFile(lpExistingFileName);
  return Ok;
  }

//---------------------------------------------------------------------------

BOOL MoveCfgFile(Strng & From, LPTSTR To)
  {
  TCHAR ErrBuff[256];
  if (!Move_FileEx(From(), To, MOVEFILE_COPY_ALLOWED/*|MOVEFILE_REPLACE_EXISTING*/|MOVEFILE_WRITE_THROUGH, ErrBuff, sizeof(ErrBuff))) 
    {
    Strng Msg;
    Msg.Set("Move Configuration File Failed:\n\n%s", ErrBuff);
    AfxMessageBox(Msg(), MB_ICONEXCLAMATION|MB_OK);
    return false;
    }

  // Create a Moved Statement
  From+=".Moved";
  CProfINIFile MovedPF(From());
  MovedPF.WrStr("MovedTo", "Folder", To);

  From=To;
  return true;
  }

//---------------------------------------------------------------------------

void SwingCfgFileIfReqd(Strng & Fn)
  {
  if (!FileExists(Fn()))
    {
    // Has it Moved?
    Strng sCfgFile(Fn);
    sCfgFile+=".Moved";
    CProfINIFile MovedPF(sCfgFile());
    sCfgFile=MovedPF.RdStr("MovedTo", "Folder", "??????");
    if (sCfgFile.Len()>0 && sCfgFile.XStrICmp("??????")!=0)
      {
      LogNote("SysCAD", 0, "Configuration file %s has been moved to %s", Fn(), sCfgFile()); 
      Fn=sCfgFile; // Swing it
      }
    }
  }

//---------------------------------------------------------------------------

BOOL ClearFolder(LPCTSTR DirName, bool DeleteIt)
  {
  Strng D(DirName), T;
  D.FnCheckEndBSlash();
  T=D;
  T+="*.*";
  WIN32_FIND_DATA FD;
  HANDLE H=FindFirstFile(T(), &FD);
  flag AllDone = (H==INVALID_HANDLE_VALUE);
  if (AllDone)
    return true;
  while (!AllDone)
    {
    if (_stricmp(FD.cFileName, ".")!=0 && _stricmp(FD.cFileName, "..")!=0)
      {
      T=D;
      T+=FD.cFileName;
      // Get and Clear Readonly
      DWORD OrigFileAtts=::GetFileAttributes(T());
      flag AttsOK=OrigFileAtts!=0xFFFFFFFF;
      if (AttsOK)
        {
        if (OrigFileAtts & FILE_ATTRIBUTE_READONLY)
          ::SetFileAttributes(T(), OrigFileAtts & ~FILE_ATTRIBUTE_READONLY);
        if (OrigFileAtts & FILE_ATTRIBUTE_DIRECTORY)
          ClearFolder(T(), true);
        else
          {
          if (!DeleteFile(T()))
            {
            Strng E;
            E.GetSystemErrorStr(GetLastError());
            LogError("SysCAD", LF_Exclamation, "File not deleted :\n%s\n%s", T(), E());
            }
          }
        }
      }
    AllDone=!FindNextFile(H, &FD);
    }
  FindClose(H);
  if (DeleteIt)
    {
    D.FnClearEndBSlash();
    if (!RemoveDirectory(D()))
      {
      Strng E;
      E.GetSystemErrorStr(GetLastError());
      LogError("SysCAD", LF_Exclamation, "Folder not removed :\n%s\n%s", D(), E());
      return false;
      }
    }
  return true;
  };

//---------------------------------------------------------------------------

BOOL CopyTextToClipboard(CWnd* pOwnerWnd, LPCTSTR buff)
  {
  if (buff && pOwnerWnd->OpenClipboard())
    {
    if (EmptyClipboard())
      {
      const int len = strlen(buff);
      HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len+1);
      char *p = (char *)GlobalLock(hData);
      strcpy(p, buff);
      GlobalUnlock(hData);
      if (SetClipboardData(CF_TEXT, hData)==NULL)
        GlobalFree(hData);
      /*//need to supply text in UNICODE; notepad etc look for CF_UNICODETEXT before CF_TEXT!
      HGLOBAL hUData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, 2 * (len+1));
      wchar_t* pU = (wchar_t*)GlobalLock(hUData);
      for (int i=0; i<len; i++)
        pU[i] = buff[i];
      pU[len] = 0;
      GlobalUnlock(hUData);
      if (SetClipboardData(CF_UNICODETEXT, hUData)==NULL)
        GlobalFree(hUData);*/
      }
    CloseClipboard();
    return true;
    }
  return false;
  }

BOOL AddTextToClipboard(CWnd* pOwnerWnd, LPCTSTR buff, LPCTSTR Separator/*=NULL*/)
  {
  if (buff && pOwnerWnd->OpenClipboard())
    {
    const int len = strlen(buff);
    const int SeparLen = (Separator ? strlen(Separator) : 0);
    char * p0 = NULL;
    HANDLE hData0 = GetClipboardData(CF_TEXT);
    if (hData0!=NULL)
      {
      p0 = (char *)GlobalLock(hData0);
      }
    int RqdLen = (p0 ? strlen(p0)+SeparLen : 0);
    RqdLen += (len+1);
    HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, RqdLen);
    char *p = (char *)GlobalLock(hData);
    if (p0)
      {
      strcpy(p, p0);
      GlobalUnlock(hData0);
      if (SeparLen>0)
        strcat(p, Separator);
      }
    else
      p[0] = 0;
    strcat(p, buff);
    GlobalUnlock(hData);

    if (EmptyClipboard())
      {
      if (SetClipboardData(CF_TEXT, hData)==NULL)
        GlobalFree(hData);
      /*//need to supply text in UNICODE; notepad etc look for CF_UNICODETEXT before CF_TEXT!
      HGLOBAL hUData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, 2 * (len+1));
      wchar_t* pU = (wchar_t*)GlobalLock(hUData);
      for (int i=0; i<len; i++)
        pU[i] = buff[i];
      pU[len] = 0;
      GlobalUnlock(hUData);
      if (SetClipboardData(CF_UNICODETEXT, hUData)==NULL)
        GlobalFree(hUData);*/
      }
    CloseClipboard();
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

BOOL CopyTextToClipboard(LPCTSTR buff)                    { return CopyTextToClipboard(ScdMainWnd(), buff);           };
BOOL AddTextToClipboard(LPCTSTR buff, LPCTSTR Separator)  { return AddTextToClipboard(ScdMainWnd(), buff, Separator); }

//---------------------------------------------------------------------------

BOOL FindDLLOrHlp(char * pFn, char * DllPath, Strng & Result)
  {
  Strng T;//, P;
  T.FnDrivePath(ProgFiles());
  T.FnClearEndBSlash();
  T.FnName();
  //P=pCfgFiles; // Strip off DirName
  //P.FnCheckEndBSlash();
  //P.FnDrivePath();
  //P+=T();
  //P.FnCheckEndBSlash();
  //
  //Result.Set("%s%s", P(), pFn);
  //if (FileExists(Result()))
  //  return true;
  //
  for (int i=0; ; i++)
    {
    Strng Folder;
    int ret=DllFilesFolder(i, DllPath, Folder);
    if (ret==1)
      {
      Result.Set("%s%s", Folder(), pFn);
      if (FileExists(Result()))
        return true;
      }
    else if (ret==0)
      {
      // No Valid Folder
      }
    else if (ret==-1)
      break;
    }

  //Result.Set("%s%s", SecureProgFiles(), pFn);
  //if (FileExists(Result()))
  //  return true;

//  Result.Set("%s%s", ProgFiles(), pFn);
//  if (FileExists(Result()))
//    return true;

  return false;
  }

//---------------------------------------------------------------------------

char* GetSystemErrorStr(Strng &S, DWORD E)
  {
  LPVOID lpMsgBuf;
  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,    0,    NULL );// Display the string.
  S=(char*)lpMsgBuf;
  // Free the buffer.
  LocalFree( lpMsgBuf );
  return S();
  }

//===========================================================================

/*kga feb2005 deleted this see new version below 
int ParseTokenList(char* Buff, char* C[], char* WhiteSpace, int MaxColumns, char Separator)
  {
  C[0]=&Buff[0];
  for (int j=1,i=0; j<MaxColumns; j++)
    C[j]=NULL;
  char *p=strchr(Buff, '\n');
  if (p!=NULL)
    *p=0;
  j=strlen(Buff);
  while (j>0 && strchr(WhiteSpace, Buff[--j]))
    {};
  if (j==0)
    return 0;

  int Quote=0;
  int InToken=false;
  int PrevWasSep=false;
  for (i=0,j=0;Buff[i]!=0 && j<MaxColumns;i++)
    {
    int IsSep=Buff[i]==Separator;
    if ((Quote==0 && (Buff[i]=='\"' || Buff[i]=='\'')) ||
      (Quote==Buff[i]))
      {
      Quote = (Quote == 0) ? Buff[i] : (Quote == Buff[i]) ? 0 : Quote;
      if (Quote)
        {
        C[j]=&Buff[i+1];
        InToken=true;
        }
      else
        {
        j++;
        Buff[i]=0;
        InToken=false;
        }
      }
    else if (Quote==0)
      {
      if (IsSep)
        {
        if (PrevWasSep)
          {
          InToken=false;
          C[j]=&Buff[i];
          Buff[i]=0;
          j++;
          C[j]=&Buff[i+1];
          }
        else
          {
          if (InToken)
            j++;
          InToken=false;
          Buff[i]=0;
          C[j]=&Buff[i+1];
          }
        }
      else if (strchr(WhiteSpace, Buff[i]))
        {
        if (PrevWasSep)
          {
          InToken=false;
          C[j]=&Buff[i];
          Buff[i]=0;
          j++;
          C[j]=&Buff[i+1];
          }
        else
          {
          if (InToken)
            j++;
          InToken=false;
          Buff[i]=0;
          C[j]=&Buff[i+1];
          }
        }
      else
        InToken=true;
      }
    PrevWasSep=IsSep;
    }
  if (PrevWasSep)
    {
    C[j]=&Buff[i];
    Buff[i]=0;
    j++;
    }

  if (Quote) // Mismatched quotes
    return -1;

  for (j=0;C[j];j++)
    XStrTrim(C[j]);
  return j;
  }
*/

int ParseTokenList(char* Buff, char* C[], char* WhiteSpace, int MaxColumns, char Separator, bool Test4Braces)
  {
  C[0]=&Buff[0];
  for (int j=1,i=0; j<MaxColumns; j++)
    C[j]=NULL;
  char *p=strchr(Buff, '\n');
  if (p!=NULL)
    *p=0;
  j=strlen(Buff);
  while (j>0 && strchr(WhiteSpace, Buff[--j]))
    {};
  if (j==0)
    return 0;

  int Quote=0;
  int Brace=0;
  int Paren=0;
  int InToken=false;
  int PrevWasSep=false;
  for (i=0,j=0;Buff[i]!=0 && j<MaxColumns;i++)
    {
    if (Test4Braces && (Buff[i]=='[' || Buff[i]==']' || Buff[i]=='(' || Buff[i]==')'))
      {
      switch (Buff[i])
        {
        case '[': Brace++; break;
        case ']': Brace--; break;
        case '(': Paren++; break;
        case ')': Paren--; break;
        }
      }
    int IsSep=(Buff[i]==Separator) && (Paren==0) && (Brace==0);
      
    if ((Quote==0 && (Buff[i]=='\"' || Buff[i]=='\'')) ||
      (Quote==Buff[i]))
      {
      Quote = (Quote == 0) ? Buff[i] : (Quote == Buff[i]) ? 0 : Quote;
      if (Quote)
        {
        C[j]=&Buff[i+1];
        InToken=true;
        }
      else
        {
        j++;
        Buff[i]=0;
        InToken=false;
        }
      }
    else if (Quote==0 && Brace==0 && Paren==0)
      {
      if (IsSep)
        {
        if (PrevWasSep)
          {
          InToken=false;
          C[j]=&Buff[i];
          Buff[i]=0;
          j++;
          C[j]=&Buff[i+1];
          }
        else
          {
          if (InToken)
            j++;
          InToken=false;
          Buff[i]=0;
          C[j]=&Buff[i+1];
          }
        }
      else if (strchr(WhiteSpace, Buff[i]))
        {
        if (PrevWasSep)
          {
          InToken=false;
          //C[j]=&Buff[i];
          Buff[i]=0;
          //j++; //incrementing j if there is whitespace immediately following a separator causes a problem!!!!!!!
          C[j]=&Buff[i+1];
          IsSep=true;//the last valid entry was a separator!
          }
        else
          {
          if (InToken)
            j++;
          InToken=false;
          Buff[i]=0;
          C[j]=&Buff[i+1];
          }
        }
      else
        InToken=true;
      }
    PrevWasSep=IsSep;
    }
  if (PrevWasSep)
    {
    C[j]=&Buff[i];
    Buff[i]=0;
    j++;
    }

  if (Quote) // Mismatched quotes
    return -1;

  for (j=0;C[j];j++)
    XStrTrim(C[j]); //shouldn't this be: XStrTrim(C[j], WhiteSpace);
  return j;
  }

//===========================================================================

void StripSingleQuoteComment(char * Buff)
  {
  if (Buff==NULL)
    return;

  int iComment=-1;
  int iQuote=0;
  int iSemi=-1;
  for (int i=0; Buff[i]!=0;i++)
    {
    if (Buff[i]=='\"') 
      iQuote=(iQuote+1)%2;
    if (Buff[i]==';' && iQuote==0) 
      iSemi=i;
    if (Buff[i]=='\'')
      {
      if (iComment>=0)
        iComment=-1;
      else
        iComment=i;
      }
    }
  if (iComment>=0) // Odd # of '
    Buff[iComment]=0; // Kill Comment
  if (iSemi>=0)
    Buff[iSemi]=0; // Kill Comment
  }

//===========================================================================

int ParseCSVTokens(char* Buff, CSVColArray &C, int& Quote, LPCTSTR MoreSepars/*=NULL*/, bool TabsAsWhitespace/*=false*/)
  {
  C[0]=&Buff[0];
  for (int j=1,i=0; j<MaxCSVCols; j++)
    C[j]=NULL;
  char *p=strchr(Buff, '\n');
  if (p!=NULL)
    *p=0;
  if (TabsAsWhitespace)
    {
    // strip out tabs and convert to spaces
    //CNM ?? why was this here while ((p=strchr(Buff, '\t'))!=NULL)
    //CNM ?? why was this here   *p=' ';
    //KGA because sometimes tabs are part of whitespace and NOT separators
    while ((p=strchr(Buff, '\t'))!=NULL)
      *p=' ';
    }
  Quote=0;
  for (i=0,j=0;Buff[i]!=0;i++)
    if (Buff[i]=='\"')
      Quote = (Quote == 0) ? 1 : (Quote == 1) ? 0 : Quote;
    else if (Buff[i]=='\'')
      Quote = (Quote == 0) ? 2 : (Quote == 2) ? 0 : Quote;
    else if (Quote==0)
      if (Buff[i]==',' || Buff[i]=='\t' || (MoreSepars && strchr(MoreSepars, Buff[i])!=NULL))
        {
        j++;
        ASSERT(j<MaxCSVCols); //too many tokens
        C[j]=&Buff[i+1];
        Buff[i]=0;
        }
  //if (Quote)
  //  LogError("SpecieDB", 0, "Bad Token - Mismatched Quotes");

  for (j=0;C[j];j++)
    {
    XStrTrim(C[j]);
    StrStripMatchedQuotes(C[j]);
    }
  return j;
  }

//---------------------------------------------------------------------------

int ParseCSVTokens(char* Buff, CSVColArray &C, LPCTSTR MoreSepars/*=NULL*/, bool TabsAsWhitespace/*=false*/)
  {
  int Quote=0;
  int n=ParseCSVTokens(Buff, C, Quote, MoreSepars, TabsAsWhitespace);
  if (Quote)
    return -1;
  return n;
  }

//---------------------------------------------------------------------------

int ParseCSVFunction(char* Buff, CSVColArray &C, int& Quote)
  {
  XStrTrim(Buff, " \"");
  for (int j=0; j<MaxCSVCols; j++)
    C[j]=NULL;
  char *p=strchr(Buff, '\n');
  if (p!=NULL)
    *p=0;
  //CNM ?? why was this here // strip out tabs convert to spaces;
  //CNM ?? why was this here while ((p=strchr(Buff, '\t'))!=NULL)
  //CNM ?? why was this here   *p=' ';
  Quote=0;
  int i=0;
  while (Buff[i]!=0 && Buff[i]!='(')
    i++;
  if (Buff[i]!='(')
    {//function name followed by open brace not found
    Quote=-1;
    return 0;
    }
  if (Buff[strlen(Buff)-1]!=')')
    {//close brace not found
    Quote=-2;
    return 0;
    }
  j=0;
  Buff[strlen(Buff)-1]=0;
  C[j++]=&Buff[0];
  Buff[i++]=0;
  if (Buff[i])
    C[j++]=&Buff[i];
  for (;Buff[i]!=0;i++)
    if (Buff[i]=='\"')
      Quote = (Quote == 0) ? 1 : (Quote == 1) ? 0 : Quote;
    else if (Buff[i]=='\'')
      Quote = (Quote == 0) ? 2 : (Quote == 2) ? 0 : Quote;
    else if (Quote==0)
      if (Buff[i]==',' || Buff[i]=='\t')
        {
        ASSERT(j<MaxCSVCols); //too many tokens
        C[j++]=&Buff[i+1];
        Buff[i]=0;
        }
  //if (Quote)
  //  LogError("SpecieDB", 0, "Bad Token - Mismatched Quotes");

  for (j=0;C[j];j++)
    {
    XStrTrim(C[j]);
    StrStripMatchedQuotes(C[j]);
    }

  return j;
  }

//---------------------------------------------------------------------------

int ParseCSVFunction(char* Buff, CSVColArray &C)
  {
  int Quote=0;
  int n=ParseCSVFunction(Buff, C, Quote);
  if (Quote)
    return -1;
  return n;
  }

//---------------------------------------------------------------------------

int ParseTokens(char* Buff, CSVColArray &C, char Token)
  {
  C[0] = &Buff[0];
  int i = 1;
  char* p = strchr(Buff, Token);
  while (p && i<MaxCSVCols)
    {
    C[i] = &p[1];
    p[0] = 0;
    p = strchr(C[i++], Token);
    }
  ASSERT(i<MaxCSVCols); //too many tokens
  for (int j=i; j<MaxCSVCols; j++)
    C[j] = NULL;
  for (j=0;C[j];j++)
    XStrTrim(C[j]);
  return i;
  }

//---------------------------------------------------------------------------

int ParseTabTokens(char* Buff, CSVColArray &C)
  {
  C[0]=&Buff[0];
  for (int j=1,i=0; j<MaxCSVCols; j++)
    C[j]=NULL;
  char *p=strchr(Buff, '\n');
  if (p!=NULL)
    *p=0;
  //CNM ?? why was this here // strip out tabs convert to spaces;
  //CNM ?? why was this here while ((p=strchr(Buff, '\t'))!=NULL)
  //CNM ?? why was this here   *p=' ';
  for (i=0,j=0;Buff[i]!=0;i++)
    if (Buff[i]=='\t')
      {
      j++;
      ASSERT(j<MaxCSVCols); //too many tokens
      C[j]=&Buff[i+1];
      Buff[i]=0;
      }

  for (j=0;C[j];j++)
    XStrTrim(C[j]);
  return j;
  }

//===========================================================================

CProfINIFile::CProfINIFile(LPCSTR pFilename)
  {
  m_bUseRegistry=false;
  m_sFilename = pFilename;
  }

//---------------------------------------------------------------------------

CProfINIFile::CProfINIFile(LPCSTR pDir, LPCSTR pFilename)
  {
  m_bUseRegistry=false;
  m_sFilename = pDir;
  m_sFilename += pFilename;
  }

//---------------------------------------------------------------------------

CProfINIFile::~CProfINIFile()
  {
  if (!IsWinNT && m_sFilename.GetLength()>0)
    Flush();
  }

//---------------------------------------------------------------------------

__int64 CProfINIFile::RdInt64(LPCTSTR lpszSection, LPCTSTR lpszEntry, __int64 iDefault)
  {
  DWORD Len;
  char Buff[256];
  if (m_bUseRegistry)
    {
    CString S=/*AfxGetApp()->*/GetProfileString(lpszSection, lpszEntry, "X");
    strcpy(Buff, S);
    Len=strlen(S);
    }
  else
    Len = GetPrivateProfileString(lpszSection, lpszEntry, "", Buff, sizeof(Buff), Filename());
  if (Len==0)
    return iDefault;
  return _atoi64(Buff);
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrInt64(LPCTSTR lpszSection, LPCTSTR lpszEntry, __int64 iValue)
  {
  char Buff[256];
#if (_MSC_VER>=1400)
  _i64toa_s(iValue, Buff, sizeof(Buff)-1, 10);
#else
  _i64toa(iValue, Buff, 10);
#endif
  BOOL b;
  if (m_bUseRegistry)
    b = /*AfxGetApp()->*/WriteProfileString(lpszSection, lpszEntry, Buff);
  else
    b = WritePrivateProfileString(lpszSection, lpszEntry, Buff, Filename());
  #ifndef _RELEASE
  if (!b)
    {
    DWORD Err=GetLastError();
    Strng E;
    E.GetSystemErrorStr(Err);
    ASSERT_ALWAYS(false, "WritePrivateProfileString error!");
    }
  #endif
  }
//---------------------------------------------------------------------------

double CProfINIFile::RdDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dDefault)
  {
  DWORD Len;
  char Buff[256];
  if (m_bUseRegistry)
    {
    CString S=/*AfxGetApp()->*/GetProfileString(lpszSection, lpszEntry, "X");
    strcpy(Buff, S);
    Len=strlen(S);
    }
  else
    Len = GetPrivateProfileString(lpszSection, lpszEntry, "", Buff, sizeof(Buff), Filename());
  if (Len==0)
    return dDefault;
  return SafeAtoF(Buff, dDefault);
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dValue)
  {
  if (Valid(dValue))
    {
    char Buff[256];
    sprintf(Buff, "%.20g", dValue);
    BOOL b;
    if (m_bUseRegistry)
      b = /*AfxGetApp()->*/WriteProfileString(lpszSection, lpszEntry, Buff);
    else
      b = WritePrivateProfileString(lpszSection, lpszEntry, Buff, Filename());
    #ifndef _RELEASE
    if (!b)
      {
      DWORD Err=GetLastError();
      Strng E;
      E.GetSystemErrorStr(Err);
      ASSERT_ALWAYS(false, "WritePrivateProfileString error!");
      }
    #endif
    }
  else
    {
    BOOL b;
    if (m_bUseRegistry)
      b = /*AfxGetApp()->*/WriteProfileString(lpszSection, lpszEntry, "*");
    else
      b = WritePrivateProfileString(lpszSection, lpszEntry, "*", Filename());
    #ifndef _RELEASE
    if (!b)
      {
      DWORD Err=GetLastError();
      Strng E;
      E.GetSystemErrorStr(Err);
      ASSERT_ALWAYS(false, "WritePrivateProfileString error!");
      }
    #endif
    }
  }

//---------------------------------------------------------------------------

long CProfINIFile::RdLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nDefault)
  {
  if (m_bUseRegistry)
    return /*AfxGetApp()->*/GetProfileInt(lpszSection, lpszEntry, nDefault);

  char Buff[256];
  DWORD Len = GetPrivateProfileString(lpszSection, lpszEntry, "", Buff, sizeof(Buff), Filename());
  if (Len==0)
    return nDefault;
  return atol(Buff);
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nValue)
  {
  char Buff[256];
  sprintf(Buff, "%li", nValue);
  BOOL b;
  if (m_bUseRegistry)
    b = /*AfxGetApp()->*/WriteProfileString(lpszSection, lpszEntry, Buff);
  else
    b = WritePrivateProfileString(lpszSection, lpszEntry, Buff, Filename());
  #ifndef _RELEASE
  if (!b)
    {
    DWORD Err=GetLastError();
    Strng E;
    E.GetSystemErrorStr(Err);
    ASSERT_ALWAYS(false, "WritePrivateProfileString error!");
    }
  #endif
  }

//---------------------------------------------------------------------------

int CProfINIFile::RdInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
  {
  if (m_bUseRegistry)
    return /*AfxGetApp()->*/GetProfileInt(lpszSection, lpszEntry, nDefault);

  char Buff[256];
  DWORD Len = GetPrivateProfileString(lpszSection, lpszEntry, "", Buff, sizeof(Buff), Filename());
  if (Len==0)
    return nDefault;
  return atoi(Buff);
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
  {
  BOOL b;
  if (m_bUseRegistry)
    b = /*AfxGetApp()->*/WriteProfileInt(lpszSection, lpszEntry, nValue);
  else
    {
    char Buff[256];
    sprintf(Buff, "%i", nValue);
    b = WritePrivateProfileString(lpszSection, lpszEntry, Buff, Filename());
    }
  #ifndef _RELEASE
  if (!b)
    {
    DWORD Err=GetLastError();
    Strng E;
    E.GetSystemErrorStr(Err);
    ASSERT_ALWAYS(false, "WritePrivateProfileString error!");
    }
  #endif
  }

//---------------------------------------------------------------------------

LPCSTR CProfINIFile::RdStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCSTR pDefault)
  {
  DWORD Len;
  char Buff[1024];
  if (m_bUseRegistry)
    {
    CString S=/*AfxGetApp()->*/GetProfileString(lpszSection, lpszEntry, pDefault);
    strcpy(Buff, S);
    Len=strlen(S);
    }
  else
    Len = GetPrivateProfileString(lpszSection, lpszEntry, "", Buff, sizeof(Buff), Filename());
  if (Len==0)
    m_sTmpRdStr = pDefault;
  else
    m_sTmpRdStr = Buff;
  return m_sTmpRdStr;
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCSTR pValue)
  {
  BOOL b;
  if (m_bUseRegistry)
    b = /*AfxGetApp()->*/WriteProfileString(lpszSection, lpszEntry, pValue);
  else
    b = WritePrivateProfileString(lpszSection, lpszEntry, pValue, Filename());
  #ifndef _RELEASE
  if (!b)
    {
    DWORD Err=GetLastError();
    Strng E;
    E.GetSystemErrorStr(Err);
    ASSERT_ALWAYS(false, "WritePrivateProfileString error!");
    }
  #endif
  }

//---------------------------------------------------------------------------

DWORD CProfINIFile::RdSection(LPCTSTR lpszSection, LPSTR pBuff, int BuffLen)
  {
  if (m_bUseRegistry)
    INCOMPLETECODE();

  DWORD Len = GetPrivateProfileSection(lpszSection, pBuff, BuffLen, Filename());
  return Len;
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrSection(LPCTSTR lpszSection, LPSTR pBuff)
  {
  if (m_bUseRegistry)
    INCOMPLETECODE();

  BOOL b = WritePrivateProfileSection(lpszSection, pBuff, Filename());
  #ifndef _RELEASE
  if (!b)
    {
    DWORD Err=GetLastError();
    Strng E;
    E.GetSystemErrorStr(Err);
    ASSERT_ALWAYS(false, "WritePrivateProfileSection error!");
    }
  #endif
  }

//---------------------------------------------------------------------------

DWORD CProfINIFile::RdSectionNames(LPSTR pBuff, int BuffLen)
  {
  if (m_bUseRegistry)
    INCOMPLETECODE();

  DWORD Len = GetPrivateProfileSectionNames(pBuff, BuffLen, Filename());
  return Len;
  }

//---------------------------------------------------------------------------

void CProfINIFile::Flush()
  {
  if (m_bUseRegistry)
    return;

  if (!IsWinNT)
    WritePrivateProfileString(NULL, NULL, NULL, Filename());
  }

//---------------------------------------------------------------------------

void CProfINIFile::GetProfItem(char* &pBuff, char* &pName, char* &pExp)
  {
  //if (m_bUseRegistry)
  //  INCOMPLETECODE();

  pName=pBuff;
  pExp=strchr(pBuff, '=');
  pBuff+=strlen(pBuff)+1;
  if(pExp)
    {
    *pExp=0;
    pExp++;
    }
  else
    pExp="";
  }

CProfINIFile ScdPFUser;
CProfINIFile ScdPFMachine;

//===========================================================================
//
//
//
//===========================================================================

typedef CSCDPtrListIter<CStopWatchList, CStopWatchItem*>CStopWatchIter;

static CStopWatchList * StopWatchList=NULL;

inline CStopWatchItem::CStopWatchItem(char * pTitle)
  {
  Title=pTitle;
  if (!StopWatchList)
    StopWatchList=new CStopWatchList;
  StopWatchList->AddTail(this);
  }

inline CStopWatchItem::~CStopWatchItem()
  {
  POSITION Pos=StopWatchList->Find(this);
  ASSERT(Pos);
  StopWatchList->RemoveAt(Pos);
  }

//---------------------------------------------------------------------------

inline void CStopWatchList ::Clear()
  {
  if (StopWatchList)
    {
    CStopWatchIter x(*StopWatchList);

    for (CStopWatchItem* p=x.First(); p; p=x.Next())
      p->Clear();
    }
  };

inline void CStopWatchList ::Dump()
  {
  if (StopWatchList)
    {
    CStopWatchIter x(*StopWatchList);
    for (CStopWatchItem* p=x.First(); p; p=x.Next())
      dbgpln("SSW : %-15.15s [%6i] %10.6f", p->Title(), p->Laps(), p->Secs());
    }
  };

//===========================================================================
//
//
//
//===========================================================================

BOOL ActivateApp(DWORD dwProcessId)
  {
  HWND DT = ::GetDesktopWindow();
  HWND W = ::GetTopWindow(DT);
  DWORD dwProcId; // process identifier
  while (W!=NULL)
    {
    GetWindowThreadProcessId(W, &dwProcId);
    if (dwProcId==dwProcessId)
      {
      if (IsIconic(W))
        ShowWindow(W, SW_RESTORE);
      SetForegroundWindow(W);
      return TRUE;
      }
    else
      W = ::GetNextWindow(W, GW_HWNDNEXT);
    }
  return FALSE;
  }

//===========================================================================
//
// Usage: SetThreadName (-1, "MainThread");
//
typedef struct tagTHREADNAME_INFO
{
  DWORD dwType; // must be 0x1000
  LPCSTR szName; // pointer to name (in user addr space)
  DWORD dwThreadID; // thread ID (-1=caller thread)
  DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;

void SetThreadName( DWORD dwThreadID, LPCSTR szThreadName)
{
  THREADNAME_INFO info;
  {
    info.dwType = 0x1000;
    info.szName = szThreadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;
  }
  __try
  {
    RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
  }
  __except (EXCEPTION_CONTINUE_EXECUTION)
  {
  }
}

//===========================================================================
//
//
//
//===========================================================================
// Hyper Threading Detection

//see https://shale.intel.com/softwarecollege/CourseDetails.asp?courseID=155

#define HT_BIT      0x10000000  // EDX[28] - Bit 28 set indicates 
// Hyper-Threading Technology is supported 
// in hardware.
#define FAMILY_ID   0x0f00      // EAX[11:8] - Bit 11 thru 8 contains family 
// processor id
#define EXT_FAMILY_ID 0x0f00000 // EAX[23:20] - Bit 23 thru 20 contains 
// extended family  processor id
#define PENTIUM4_ID   0x0f00      // Pentium 4 family processor id
// Returns non-zero if Hyper-Threading Technology is supported on 
// the processors and zero if not.  This does not mean that 
// Hyper-Threading Technology is necessarily enabled.
bool HTSupported(void)
  {
  unsigned int reg_eax = 0;
  unsigned int reg_edx = 0;
  unsigned int vendor_id[3] = {0, 0, 0};
  __try 
    {            // verify cpuid instruction is supported
    __asm 
      {
      xor   eax, eax    // call cpuid with eax = 0
        cpuid        // get vendor id string
        mov   vendor_id, ebx
        mov   vendor_id + 4, edx
        mov   vendor_id + 8, ecx
        mov   eax, 1      // call cpuid with eax = 1
        cpuid
        mov   reg_eax, eax // eax contains cpu family type info
        mov   reg_edx, edx  // edx has info whether Hyper-Threading
        // Technology is available
      }
    }
  __except (EXCEPTION_EXECUTE_HANDLER ) 
    {
    return 0;   // CPUID is not supported and so Hyper-Threading Technology
    // is not supported
    }

  //  Check to see if this is a Pentium 4 or later processor
  if (((reg_eax & FAMILY_ID) ==  PENTIUM4_ID) || (reg_eax & EXT_FAMILY_ID))
    if (vendor_id[0] == 'uneG') 
      if (vendor_id[1] == 'Ieni')
        if (vendor_id[2] == 'letn')
          return (reg_edx & HT_BIT)!=0;  // Genuine Intel Processor with 
  // Hyper-Threading Technology
  return 0;  // This is not a genuine Intel processor.
  }


//     	
//Code Example
//
//Although this example shows a separate chunk of code, the appropriate lines from this example should be included when the processor type and Hyper-Threading Technology support is determined.
//
//#define NUM_LOGICAL_BITS	0x00FF0000 // EBX[23:16] indicate number of
//                                           // logical processors per package
//// Returns the number of logical processors per physical processors.
//unsigned char LogicalProcessorsPerPackage(void)
//{
//    unsigned int reg_ebx = 0;
//    if (!HTSupported()) return (unsigned char) 1;
//    __asm {
//      mov	 eax, 1			// call cpuid with eax = 1
//      cpuid
//      mov	 reg_ebx, ebx           // Has info on number of logical processors	
//    }
//  return (unsigned char) ((reg_ebx & NUM_LOGICAL_BITS) >> 16);
//}
//
//
//* Legal Information and Privacy Policy © 2002 Intel Corporation
//	
  

//  	
//The Importance of Logical:Physical Associations
//
//Although your platform might support Hyper-Threading Technology and your code has determined the number of logical processors in a physical package, there could still be limitations to running your multi-threaded code on your desktop:
//
//    * the BIOS might not support Hyper-Threading Technology
//    * the OS might not utilize all the processors (logical and physical)
//    * code licensing might prevent the use of multiple processors
//    * an application might limit processor affinity to fewer processors than the number available. 
//
//These implementations (BIOS, OS, or application) might affect an application's ability to use Hyper-Threading Technology. In order for your code to exploit the benefits of Hyper-Threading Technology, it will need to identify the APIC IDs of all processors in the system, then associate them with the appropriate logical and physical processors.
//
//To determine the association between physical and logical processors your code will first need to extract and interpret processor APIC IDs. Your code might also need to verify that the operating system is running multiple processors and that an application can use multiple processors.
//
//This lesson cannot show you all the details your code will need to cover, such as building a table of APIC IDs and the appropriate affinity settings. This lesson will illustrate how you can add to your code to check the processor IDs and the processor affinity and print out a report. A code example is included.
//
//* Legal Information and Privacy Policy © 2002 Intel Corporation
//	
//     	
//APIC IDs
//
//The OS and the application are unaware whether or not the processors support Hyper-Threading Technology. Your code has to determine that. Logical to Physical associations help the code determine what processors are available and whether or not Hyper-Threading Technology is available.
//
//The hardware identifies each processor by an identification number. At hardware reset, the Advanced Programmable Interrupt Controller establishes APIC IDs for devices in the system, including available processors. If the BIOS supports Hyper-Threading Technology, APIC IDs are established for all logical processors (logical IDs). If the BIOS does not establish APIC IDs for all logical processors, Hyper-Threading Technology might not be available to an application.
//
//When the OS is booted, supporting boot arguments determine how many processors the OS will utilize. If the OS boots with less than the available number of logical and physical processors (known by their APIC IDs) actually in the system, Hyper-Threading Technology might not be available to an application.
//
//The operating system stores the number of processors it uses. An application can use all processors the OS uses, or it can limit how many processors are available to itself. To use Hyper-Threading Technology fully, the number of processors that the OS uses must equal the number of processors available to the application. Your code needs to check that these values are equal, and then it can check all APIC IDs and associate them with physical processors.
//
//* Legal Information and Privacy Policy © 2002 Intel Corporation
//	
//  
//     	
//APIC IDs = Logical and Physical IDs
//
//Each processor stores its APIC ID in bits 24 through 31of its onboard register ebx. When a physical processor has more than one logical processor (Hyper-Threading Technology enabled), the least significant bit(s) distinguish the APIC ID of each processor. If a physical processor contains only two logical processors, bit 24 distinguishes the APIC ID of each processor; if there are four logical processors, bits 24 and 25 uniquely identify each processor. Knowing the number of logical processors in a physical processor indicates how many LSBs will be needed to uniquely identify each logical processor in the physical processor.
//ebx Bit 	31 	30 	29 	28 	27 	26 	25 	24
//APIC ID 	P 	P 	P 	P 	P 	P 	P/L 	P/L
//1 Processor 	X 	X 	X 	X 	X 	X 	X 	X
//2 Logical Processors 	X 	X 	X 	X 	X 	X 	X 	L0/1
//4 Logical Processors 	X 	X 	X 	X 	X 	X 	L3/4 	L3/4
//
//In retrieving each processor APIC ID, you will be able to identify each logical processor in a physical processor, because the two logical processors will contain the same physical ID value. In a Hyper-Threading Technology processor with two logical processors, bits 25 through 31 will have the same value. For example,
//ebx Bit 	31 	30 	29 	28 	27 	26 	25 	24
//Processor L0 	0 	0 	0 	1 	1 	0 	1 	0
//Processor L1 	0 	0 	0 	1 	1 	0 	1 	1
//
//* Legal Information and Privacy Policy © 2002 Intel Corporation
	
  

    	
//Code Example: Retrieving the Processor APIC ID
//
//Your code must retrieve the APIC ID from all processors, so you can build a list of all APIC IDs. You read the APIC ID using the CPUID instruction.
//
//The following code example shows how to use the CPUID instruction to retrieve the APIC ID for the processor the code is running on. This small chunk of code would have to be run on each logical processor in the system to retrieve the APIC ID from each processor. Note that this code returns a value to the variable GetAPIC_ID.
//
//#define INITIAL_APIC_ID_BITS	0xFF000000 // EBX[31:24] unique APIC ID
//// Returns the 8-bit unique Initial APIC ID for the processor this
//// code is actually running on. The default value returned is 0xFF if
//// Hyper-Threading Technology is not supported.
//unsigned char GetAPIC_ID (void)
//{
//  unsigned int reg_ebx = 0;
//  if (!HTSupported()) return (unsigned char) -1;
//  __asm {
//    mov	 eax, 1          // call cpuid with eax = 1
//    cpuid
//    mov	 reg_ebx, ebx    // Has APIC ID info
//  }
//  return (unsigned char) ((reg_ebx & INITIAL_APIC_ID_BITS) >> 24);
//}


// FIND Out if HT Enabled

//
//
//#include &lt;windows.h&gt;
//#include &lt;stdio.h&gt;
//
//// Be sure to include the routines previously documented in this 
//// application note here.
//
//void main (void) {
//  // Check to see if Hyper-Threading Technology is available 
//  if (HTSupported()) {  // Bit 28 set indicated Hyper-Threading Technology
//    unsigned char HT_Enabled = 0;
//    unsigned char Logical_Per_Package; 
//    printf (&quote;Hyper-Threading Technology is available.\n&quote;);
//    Logical_Per_Package = LogicalProcessorsPerPackage();
//    printf (&quote;Logical Processors Per Package: %d\n&quote;, Logical_Per_Package);
//
//    // Just because logical processors is &gt; 1 
//    // does not mean that Hyper-Threading Technology is enabled.
//    if (Logical_Per_Package &gt; 1) {
//      HANDLE hCurrentProcessHandle;
//      DWORD dwProcessAffinity;
//      DWORD dwSystemAffinity;
//      DWORD dwAffinityMask;
//
//      // Physical processor ID and Logical processor IDs are derived
//      // from the APIC ID.  We'll calculate the appropriate shift and
//      // mask values knowing the number of logical processors per
//      // physical processor package.
//      unsigned char i = 1;
//      unsigned char PHY_ID_MASK = 0xFF;
//      unsigned char PHY_ID_SHIFT = 0;
//
//      while (i &lt; Logical_Per_Package){
//        i *= 2;
//        PHY_ID_MASK &lt;&lt;= 1;
//        PHY_ID_SHIFT++;
//      }
//
//      // The OS may limit the processors that this process may run on.
//      hCurrentProcessHandle = GetCurrentProcess();
//      GetProcessAffinityMask(hCurrentProcessHandle, &dwProcessAffinity,
//                                       &dwSystemAffinity);
//
//      // If our available process affinity mask does not equal the 
//      // available system affinity mask, then we may not be able to 
//      // determine if Hyper-Threading Technology is enabled.
//      if (dwProcessAffinity != dwSystemAffinity)
//        printf (&quote;This process can not utilize all processors.\n&quote;); 
//
//      dwAffinityMask = 1;
//      while (dwAffinityMask != 0 && dwAffinityMask &lt;= dwProcessAffinity) {
//        // Check to make sure we can utilize this processor first.
//        if (dwAffinityMask & dwProcessAffinity) {
//          if (SetProcessAffinityMask(hCurrentProcessHandle, 
//                                      dwAffinityMask)) {
//            unsigned char APIC_ID;
//            unsigned char LOG_ID;
//            unsigned char PHY_ID;
//          
//            Sleep(0);  // We may not be running on the cpu that we
//                      // just set the affinity to.  Sleep gives the OS
//                      // a chance to switch us to the desired cpu.
//
//            APIC_ID = GetAPIC_ID();
//            LOG_ID = APIC_ID & ~PHY_ID_MASK;
//            PHY_ID = APIC_ID &gt;&gt; PHY_ID_SHIFT;
//
//            // Print out table of processor IDs
//            printf (&quote;OS Affinity ID: 0x%.8x, APIC ID: %d PHY ID: %d, LOG ID: %d\n&quote;,
//                dwAffinityMask, APIC_ID, PHY_ID, LOG_ID);
//            if (LOG_ID != 0) HT_Enabled = 1;
//          }
//          else {
//            // This shouldn't happen since we check to make sure we
//            // can utilize this processor before trying to set 
//            // affinity mask.
//            printf (&quote;Set Affinity Mask Error Code: %d\n&quote;, 
//                                        GetLastError());
//          }
//        }
//        dwAffinityMask = dwAffinityMask &lt;&lt; 1;
//      }
//      // Don't forget to reset the processor affinity if you use this code
//      // in an application.
//      SetProcessAffinityMask(hCurrentProcessHandle, dwProcessAffinity);
//      if (HT_Enabled) 
//        printf (&quote;Processors with Hyper-Threading Technology enabled was detected.\n&quote;);
//      else
//        printf (&quote;Processors with Hyper-Threading Technology enabled was not detected.\n&quote;);
//    }
//    else
//      printf (&quote;Processors with Hyper-Threading Technology is not enabled.\n&quote;);
//  }
//  else {
//    printf (&quote;Hyper-Threading Technology Processors are not detected.\n&quote;);
//  }
//}


	
 
 
//===========================================================================
//
//
//
//===========================================================================

//see project.cpp for setting of project load version numbers
int iPrjFileVerNo = -1;     //project version number of spj currently being loaded
int iNewPrjFileVerNo = -1;  //newest project version number that would be used for spj saves
int iOrigPrjFileVerNo = -1; //project version number of spj when created or -1 for VerNos < 27
int iPrjFileVerNoAsLoaded = -1;  // project version number as loaded
int iPrjFileFormat = 0;     // 0 = use CSV and original spj format
                            // 1 = use MDB and INI file spj format
int iSCDFileFormat = 0;     // 0 = use CSV and original spj format
                            // 1 = use MDB and INI file spj format

flag iUsingPrjLclFiles = false;
flag iSymbolicPaths = 1;

void SetPrjFileVerNo(int PrjFileVerNo)          { iPrjFileVerNo = PrjFileVerNo; }
int PrjFileVerNo()                              { return iPrjFileVerNo; }
void SetNewPrjFileVerNo(int NewPrjFileVerNo)    { iNewPrjFileVerNo = NewPrjFileVerNo; }
int NewPrjFileVerNo()                           { return iNewPrjFileVerNo; }
void SetOrigPrjFileVerNo(int OrigPrjFileVerNo)  { iOrigPrjFileVerNo = OrigPrjFileVerNo; }
int OrigPrjFileVerNo()                          { return iOrigPrjFileVerNo; }
void SetPrjFileVerNoAsLoaded(int PrjFileVerNo)  { iPrjFileVerNoAsLoaded=PrjFileVerNo; };
int PrjFileVerNoAsLoaded()                      { return iPrjFileVerNoAsLoaded; };
void SetPrjFileFormat(int PrjFileFormat)        { iPrjFileFormat = Min(iSCDFileFormat, PrjFileFormat); }
int PrjFileFormat()                             { return iPrjFileFormat; }
void SetSCDFileFormat(int SCDFileFormat)        { iSCDFileFormat = SCDFileFormat; }
int SCDFileFormat()                             { return iSCDFileFormat; }

flag SetUsingPrjLclFiles(flag On)               { flag i=iUsingPrjLclFiles; iUsingPrjLclFiles=On; return i; };
flag UsingPrjLclFiles()                         { return iUsingPrjLclFiles; };
flag SetSymbolicPaths(flag On)                  { flag i=iSymbolicPaths; iSymbolicPaths=On; return i; };
flag SymbolicPaths()                            { return iSymbolicPaths; };

//===========================================================================

BOOL IsWinNT = true;

Strng MajorVersionNo;       //eg "9.1"
Strng VersionNo;            //eg "9.1 Build 122.1020"
Strng FullMajorVersion;     //eg "SysCAD 9.1" or "SysCAD 9.1 (Release Debug)"
Strng FullVersion;          //eg "SysCAD 9.1 Build 122.1020" or "SysCAD 9.1 Build 122.1020 (Full Debug)"
Strng FullVersionAndUpdate; //eg "SysCAD 9.1 Build 122.1020   (20 November 2006)"
Strng UpdateString;         //eg "Update 2  (1 September 2001)"

//---------------------------------------------------------------------------

char* CopyrightNotice()     { return SCD_VERINFO_Copyright; }
char* FullCopyrightNotice() { return SCD_VERINFO_CopyrightFull; }
char* Company()             { return SCD_VERINFO_Company; }
char* FullCompany()         { return SCD_VERINFO_CompanyFull; }
char* Company2()            { return SCD_VERINFO_Company2; }
char* FullCompany2()        { return SCD_VERINFO_Company2Full; }
char* FirstBuildDate()      { return SCD_BUILD_DATE; }
char* BuildDate()           { return SCD_PATCHDATE; }

//---------------------------------------------------------------------------

void InitVersionStuff()
  {
  MajorVersionNo.Set("%d.%d", SCD_VERINFO_V0, SCD_VERINFO_V1);
  FullMajorVersion.Set("SysCAD %s", MajorVersionNo());
  VersionNo.Set("%s Build:%d.%d", MajorVersionNo(), SCD_BUILD_NUM, SCD_PATCH_SVN);
  FullVersion.Set("SysCAD %s", VersionNo());
  #ifdef _DEBUG
  FullMajorVersion += " Debug";
  FullVersion += " Debug";
  #elif  _RELEASE
  #else
  FullMajorVersion += " Rls Debug";
  FullVersion += " Rls Debug";
  #endif
  FullVersionAndUpdate = FullVersion;
  UpdateString.Set("(%s)", SCD_PATCHDATE);
  FullVersionAndUpdate.Append("   %s", UpdateString());
  }

///===========================================================================

// Not documented as an instance of this class is declared below.
//class StrngEntryExit
//  {
//  public:
//    StrngEntryExit();
//    ~StrngEntryExit();
//
//  pStrng  pS; // ATest String which will not be deallocated?
//  };

//StrngEntryExit::StrngEntryExit()
//  {
//  InitializeCriticalSection(&Strng::BuffSect);
//  for (int i=0; i<MaxSpareStrBuffs; i++)
//    Strng::SpareBuffs[i]=NULL;
//  Strng::SparesOK=0; //use debugger to set to 0 if spares must not be used - for memory leaks
//  pS = new Strng("Dealloc Test String"); // ATest String which will not be deallocated?
//  };
//
//StrngEntryExit::~StrngEntryExit()
//  {
//  Strng::SparesOK=0;
//  TRACE("============ Dealloc Test String will be left behind ============\n");
//  for (int i=0; i<MaxSpareStrBuffs; i++)
//    while (Strng::SpareBuffs[i])
//      {
//      char* p=Strng::SpareBuffs[i];
//      ppchar pp=(ppchar)p;
//      Strng::SpareBuffs[i]=*pp;
//      ::delete []p;
//      };
//  //DeleteCriticalSection(&Strng::BuffSect);
//  };

//==========================================================================

DWORD FreeVirtualMemory()
  {
  MEMORYSTATUS MS;
  MS.dwLength=sizeof(MS);
  GlobalMemoryStatus(&MS);
  return(MS.dwAvailVirtual);
  }

//==========================================================================

CSCDFileDialog::CSCDFileDialog( BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
  CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
    {
    m_ForOpen=bOpenFileDialog;
    m_DoLCTests=true;
    m_DoingPrjOpen=false;
    };

CSCDFileDialog::~CSCDFileDialog()
  {
  setlocale(LC_ALL, "C");
  };

int CSCDFileDialog::DoModal( )
  {
  int Ret=CFileDialog::DoModal( );
  setlocale(LC_ALL, "C");
  return Ret;
  };

BOOL CSCDFileDialog::OnFileNameOK( )
  {
  if (m_DoLCTests)
    {
    //TODO This project file checking could perhaps be simplified/reviewed...
    Strng Tst;
    Strng Fn=GetPathName();
    Strng FnP=GetPathName();
    Fn.Lower();
    FnP.FnDrivePath();

    Tst=FnP+"*.spj";
    flag PrjFileEx=FileExists(Tst());

    Tst.Set("\\%s\\", BackupFilesFolderName());
    Tst.Lower();
    flag BkUpInPath=(Fn.XStrStr(Tst())!=NULL);

    Tst=PrjFiles();
    flag InPrjFiles=Tst.Length()>0 && (Fn.XStrNICmp(Tst(), Tst.Length())==0);

    Tst=PrjRmtCpyFiles();
    flag InPrjRmtCpyFiles=Tst.Length()>0 && (Fn.XStrNICmp(Tst(), Tst.Length())==0);

    if (m_ForOpen)
      {
      Strng Ext=GetFileExt();
      if (m_DoingPrjOpen && Ext.XStrICmp("spj")==0)
        {
        if (BkUpInPath)
          {
          LogNote("SysCAD", LF_Exclamation|LF_NoSkip, "May not Open File within %s", Fn());
          return 1;
          }
        }
      else if (UsingPrjLclFiles())
        {
        if (!InPrjFiles)
          {
          LogNote("SysCAD", LF_Exclamation|LF_NoSkip, "Must select file within %s", PrjFiles());
          return 1;
          }
        }
      else
        {
        if (InPrjRmtCpyFiles || BkUpInPath)
          {
          LogNote("SysCAD", LF_Exclamation|LF_NoSkip, "May not select file within %s", PrjRmtCpyFiles());
          return 1;
          }
        }
      }
    else // SaveAs
      {
      if (m_DoingPrjOpen)
        {
        if (PrjFileEx)
          {
          LogNote("SysCAD", LF_Exclamation|LF_NoSkip, "A project already exists in selected folder!  May not create %s.spf", Fn());
          return 1;
          }
        if (BkUpInPath)
          {
          LogNote("SysCAD", LF_Exclamation|LF_NoSkip, "Cannot create project in selected folder!", Fn());
          return 1;
          }
        }
      }
    }
  return 0;
  };

//==========================================================================

#ifdef _DEBUG
BOOL DoPostMsgBreak = 1;
BOOL MyPostMessage(CWnd* pWnd, UINT message, WPARAM wParam/*=0*/, LPARAM lParam/*=0*/)
  {
  BOOL b = pWnd->PostMessage(message, wParam, lParam);
  /*if (!b)
    {
    DWORD Err = GetLastError();
    Sleep(0);
    b = pWnd->PostMessage(message, wParam, lParam);
    }*/
  if (!b)
    {
    DWORD Err = GetLastError();
    dbgpln("PostMessage(%d,%d,%d) failed!  Err:%d", message, wParam, lParam, Err);
    if (DoPostMsgBreak)
      {
      DoBreak();
      }
    }
  return b;
  }
#endif

//==========================================================================

CSaveRestResHandle::CSaveRestResHandle(HINSTANCE hNew)
  {
  // As per Sample DLLHUSK
//  hInstOld = AfxGetResourceHandle();
//  AfxSetResourceHandle(hNew);
//  dbgpln("------- %08x > %08x", hInstOld, hNew);
  }

//---------------------------------------------------------------------------

CSaveRestResHandle::~CSaveRestResHandle()
  {
//  dbgpln("------- %08x <",hInstOld);
//  AfxSetResourceHandle(hInstOld);
  };

//==========================================================================
//
//
//
//==========================================================================

CUpGrdCompletion::CUpGrdCompletion(char * Old)
  {
  m_Old=Old;
  };

//---------------------------------------------------------------------------

int CUpGrdCompletion::Add(UGC_Outcome Outcome, UGC_Ops  What, char*Src, char*Dst)
  {
  UGC_JobList & Jobs = (Outcome==UGC_Success) ? m_SuccessJobs : m_FailJobs;
  Jobs.SetSize(Jobs.GetSize()+1);
  int i=Jobs.GetUpperBound();
  Jobs[i].m_What=What;
  Jobs[i].m_Src=Src;
  Jobs[i].m_Dst=Dst;
  return i+1;
  };

//---------------------------------------------------------------------------

int CUpGrdCompletion::Complete(flag OK)
  {
  UGC_JobList & Jobs = (OK) ? m_SuccessJobs : m_FailJobs;
  int Err=0;
  OK=true;
  int i=-1;
  if (m_Old())
    OK=FnCreateDirectory(m_Old(), true);
  for (i=0; i<Jobs.GetSize(); i++)
    {
    switch (Jobs[i].m_What)
      {
      case UGC_Delete:
        OK=::DeleteFile(Jobs[i].m_Src());
        if (OK)
          LogNote("Upgrade", 0, "File '%s' deleted", Jobs[i].m_Src());
        break;
      case UGC_Copy:
        OK=::CopyFile(Jobs[i].m_Src(), Jobs[i].m_Dst(), false);
        if (OK)
          LogNote("Upgrade", 0, "File '%s' copied to '%s'", Jobs[i].m_Src(), Jobs[i].m_Dst());
        break;
      case UGC_Move:
        OK=Move_FileEx(Jobs[i].m_Src(), Jobs[i].m_Dst(), MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
        if (OK)
          LogNote("Upgrade", 0, "File '%s' moved to '%s'", Jobs[i].m_Src(), Jobs[i].m_Dst());
        break;
      case UGC_MoveOld:
        {
        Strng Fn, Path(m_Old);
        Fn.FnNameExt(Jobs[i].m_Src());
        Path.FnCheckEndBSlash();
        Path+=Fn;
        OK=Move_FileEx(Jobs[i].m_Src(), Path(), MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
        if (OK)
          LogNote("Upgrade", 0, "File '%s' moved to '%s'", Jobs[i].m_Src(), Path());
        break;
        }
      case UGC_CDir:
        OK=FnCreateDirectory(Jobs[i].m_Src(), false);
        if (OK)
          LogNote("Upgrade", 0, "Folder '%s' created", Jobs[i].m_Src());
        break;
      case UGC_CDirComp:
        OK=FnCreateDirectory(Jobs[i].m_Src(), true);
        if (OK)
          LogNote("Upgrade", 0, "Compressed Folder '%s' created", Jobs[i].m_Src());
        break;
      };
    if (!OK)
      break;
    }

  if (!OK)
    {
    DWORD Err=::GetLastError();
    Strng E;
    E.GetSystemErrorStr(Err);
    LogError("Upgrade", LF_Exclamation, "'%s' : %s",
      i<0 ? m_Old() : Jobs[i].m_Src(), E());
    }

  return OK;
  };

//==========================================================================
//
//
//
//==========================================================================

//==========================================================================
//
//
//
//==========================================================================

//==========================================================================
// These must not be inline so as to create the reference MakeName

flag SCDLibMakeCompatible(LPCTSTR MakeName)
  {
  flag OK;
  if (_stricmp(_MAKENAME, "Debug")==0)
    {
    OK=(_stricmp(MakeName, _MAKENAME)==0);
    }
  else
    {
    OK=(_stricmp(MakeName, "Release")==0) || (_stricmp(MakeName, "Dbg_Release")==0);
    }
  return OK;
  };

flag SCDLibVersionCompatible(WORD V0, WORD V1, WORD V2, WORD V3)
  {
  return (SCD_VERINFO_V0==V0 && SCD_VERINFO_V1==V1 && SCD_VERINFO_V2==V2);
  };

bool MakeVersionOK(LPCTSTR FileName, LPCTSTR MakeName, WORD V0, WORD V1, WORD V2, WORD V3, bool DoMsg)
  {
  if (!SCDLibMakeCompatible(MakeName))
    {
    if (DoMsg)
      {
      char buff[256];
      sprintf(buff, "%s Terminating - Different Build Types !", FileName);
      ::MessageBox(NULL, buff, "SysCAD", MB_OK|MB_ICONEXCLAMATION|MB_SETFOREGROUND|MB_TASKMODAL);
      strcat(buff, "\n");
      TRACE(buff);
      }
    return False;
    }
  if (!SCDLibVersionCompatible(V0, V1, V2, V3))
    {
    if (DoMsg)
      {
      char buff[256];
      sprintf(buff, "%s Terminating - Incompatible Version !", FileName);
      ::MessageBox(NULL, buff, "SysCAD", MB_OK|MB_ICONEXCLAMATION|MB_SETFOREGROUND|MB_TASKMODAL);
      strcat(buff, "\n");
      TRACE(buff);
      }
    return False;
    }
  return True;
  };

//==========================================================================
//
//
//
// ========================================================================

#include <nb30.h>
#pragma comment(lib, "netapi32.lib")

typedef struct _ASTAT_
  {
  ADAPTER_STATUS adapt;
  NAME_BUFFER    NameBuff[30];
  } ASTAT, * PASTAT;

INT64 GetMacAddress(LPCTSTR sNetBiosNameIn)
  {
  CString sNetBiosName(sNetBiosNameIn);
  ASTAT Adapter;

  NCB ncb;
  UCHAR uRetCode;

  memset(&ncb, 0, sizeof(ncb));
  ncb.ncb_command = NCBRESET;
  ncb.ncb_lana_num = 0;

  uRetCode = Netbios(&ncb);

  memset(&ncb, 0, sizeof(ncb));
  ncb.ncb_command = NCBASTAT;
  ncb.ncb_lana_num = 0;

  sNetBiosName.MakeUpper();

  FillMemory(ncb.ncb_callname, NCBNAMSZ - 1, 0x20);

  strcpy((char *)ncb.ncb_callname, (LPCTSTR) sNetBiosName);

  ncb.ncb_callname[sNetBiosName.GetLength()] = 0x20;
  ncb.ncb_callname[NCBNAMSZ] = 0x0;

  ncb.ncb_buffer = (unsigned char *) &Adapter;
  ncb.ncb_length = sizeof(Adapter);

  uRetCode = Netbios(&ncb);

  if (uRetCode == 0)
    {
    INT64 Mac=0;
    for (int i=0; i<6; i++)
      Mac = (Mac << 8) | Adapter.adapt.adapter_address[i];
    return Mac;
    }
  return 0;
  }

CString GetMacAddressStr(LPCTSTR sNetBiosNameIn)
  {
  INT64 Mac=GetMacAddress(sNetBiosNameIn);

  ASTAT Adapter;
  memmove(Adapter.adapt.adapter_address, &Mac, 6);

  for (int i=5; i>=0; i--)
    {
    Adapter.adapt.adapter_address[i] = (UCHAR)(Mac &0xff);
    Mac = (Mac >> 8);
    }

  CString sMacAddress;
  sMacAddress.Format(_T("%02x.%02x.%02x.%02x.%02x.%02x"),
    Adapter.adapt.adapter_address[0],
    Adapter.adapt.adapter_address[1],
    Adapter.adapt.adapter_address[2],
    Adapter.adapt.adapter_address[3],
    Adapter.adapt.adapter_address[4],
    Adapter.adapt.adapter_address[5]);
  return sMacAddress;
  }

//===========================================================================

DWORD GetDiskSerialNumber(LPCTSTR FileName)
  {
  Strng Fn(FileName);
  Fn.FnDrive();
  Fn.FnCheckEndBSlash();

  char VolumeName[1024];
  char FSNameBuff[1024];
  DWORD dwVolSerialNo, dwMaxCompLen, dwFileSysFlags;
  if (GetVolumeInformation(Fn(),
    VolumeName, sizeof(VolumeName)-1,
    &dwVolSerialNo, &dwMaxCompLen, &dwFileSysFlags,
    FSNameBuff, sizeof(FSNameBuff)-1))
    return dwVolSerialNo;
  return 0;
  }

//==========================================================================
//
//
//
// ========================================================================
// AfxGetMainWnd() replacement

static CWnd * s_pMainWnd = NULL;

void SetScdMainWnd(CWnd * p)
  {
  s_pMainWnd=p;
  };

CWnd * ScdMainWnd()
  {
  if (s_pMainWnd)
    return s_pMainWnd;
  return AfxGetMainWnd();
  };

//==========================================================================
//
//
//
//==========================================================================

#pragma warning (disable : 4073) // initializers put in library initialization area
#pragma init_seg(lib) // Force Early Construction and Late Destruction

//==========================================================================
// Lock to be used on Startup & shutdown
// Initialise Early

CCriticalSection StartupLock;

//==========================================================================

long lDoFreeLibraries=true;

//==========================================================================
// Initialise Early

//StrngEntryExit StrngEntryExitInstance; // destructor will get called at exit;

IMPLEMENT_SPARES(Strng, 1000);
IMPLEMENT_SPARES(Strng_List, 100);

//==========================================================================
