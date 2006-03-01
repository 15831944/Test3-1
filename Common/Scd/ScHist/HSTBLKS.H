//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __HSTBLKS_H
#define  __HSTBLKS_H

#ifndef __HSTMSG_H
#include "hstmsg.h"
#endif

#ifdef __HSTBLKS_CPP
  #define DllImportExport DllExport
#elif !defined(SCHIST)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


//-------------------------------------------------------------------------

_FWDDEF(CHistorian)
_FWDDEF(CSlot)
_FWDDEF(CCatItem)

//-------------------------------------------------------------------------

_FWDDEF(CEvBlkFileInfo)
class CEvBlkFileInfo
  {
  DEFINE_MEMSTATS(CEvBlkFileInfo)
  public:
    CString           sFilename;       //filename of event block = ScenarioName.CatNo
    HANDLE            hBlkFile;        //handle for event block file
    HANDLE            hBlkFileMap;     //handle for event block mem-mapped file
    unsigned char*    pBlkMap;         //pointer to start of memory mapped file
    DWORD             dwFileAllocLen;  //number of bytes allocated to memory mapped file
  };

//-------------------------------------------------------------------------

_FWDDEF(CEvBlk)
class CEvBlk
  {
  DEFINE_MEMSTATS(CEvBlk)
  friend class CHistorian;
  friend class CQueryBlk;
  friend class CQuerySlice;
  friend class CLockBlk;
  public:
    pCEvBlkFileInfo   pInfo;          //info related to mem mapped file
    CRITICAL_SECTION  Lock;           //critical section used for locking
    DWORD             lTtlLen;        //no of bytes written to mem-mapped file
    pHD_EvBlkHead     pEvHead;        //pointer to event block header in mem-mapped file
    pHD_TimeSliceHead pTS;            //pointer to current time slice header in mem-mapped file
    pHD_TimeSliceHead pPrevTS;        //pointer to the previous time slice header in mem-mapped file
    pHD_TimeSliceHead pFirstTS;       //pointer to the first time slice header in mem-mapped file
    unsigned char*    pPrevDeltaTime; //pointer to the previous delta time in current time slice           
    unsigned char*    pData;          //pointer to start of data for the current timeslice in mem-mapped file
    DWORD             lNextID;        //slot ID of next expected event
    pCCatItem         pCatItem;       //pointer to catalog item
    WORD              iDeltaTimeCnt;  //number of DeltaTime's in time slice
    double            dJmpDelta;      //number of bytes per jump block

    CEvBlk();
    void Initialise();
    unsigned char Open(WORD CatNo);
    unsigned char Open(pCCatItem CatItem);
    void Close();
    void MarkAsFinished();
  protected:
    void StartTimeSlice(double time);
    void EndTimeSlice();
    void Skip(DWORD DeltaID);
    void SkipTime(float DeltaTime);
    inline double GetLastTime() { return (pTS ? pTS->dLastTime : pEvHead->dLastTime); };
  public:
    unsigned char AddEv(double time, long SNo, rDataUnion Val);
    void Start(double time);
    void Stop(double time);
  protected:
    inline void LockEvBlk() { EnterCriticalSection(&Lock); };
    inline void FreeEvBlk() { LeaveCriticalSection(&Lock); };
    inline DWORD GetTSPos() { return (pTS==NULL ? 0 : GetIndex((unsigned char*)pTS)); };  
    inline DWORD GetIndex(unsigned char* p) { /*ASSERT(p>pInfo->pBlkMap);*/ return (DWORD)(p - pInfo->pBlkMap); };
    inline unsigned char* GetPtr(DWORD index) { return &pInfo->pBlkMap[index]; };
  };

//-------------------------------------------------------------------------

//#ifndef __HSTMAIN_H
//#include "hstmain.h"
//#endif

//-------------------------------------------------------------------------

#undef DllImportExport 

#endif

          
