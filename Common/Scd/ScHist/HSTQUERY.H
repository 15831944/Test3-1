//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __HSTQUERY_H
#define  __HSTQUERY_H

#if ForSysCAD
#ifndef __EXECLIB_H
#include "execlib.h"
#endif
#endif
#ifndef __HSTMSG_H
#include "hstmsg.h"
#endif

/*#if defined(__HSTQUERY_CPP)
  #define DllImportExport DllExport
#elif !defined(SCHIST)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif*/

//-------------------------------------------------------------------------
//_FWDDEF(CHistorian)
_FWDDEF(CEvBlk)
_FWDDEF(CQueryHist)
_FWDDEF(CQueryBlk)
_FWDDEF(CQuerySlice)
_FWDDEF(CCatItem)
_FWDDEF(CHistCat)

#define dbgHstQuery  0 //creates qrydump.xxx for each query

//-------------------------------------------------------------------------


//-------------------------------------------------------------------------

class CHstQueryException : public CException
  {
  DEFINE_MEMSTATS(CHstQueryException)

  public :
    DECLARE_DYNAMIC(CHstQueryException);
    long lErrorNumber;
    pCCatItem pCatItem;
    CHstQueryException() 
      { 
      lErrorNumber = 0L;
      pCatItem = NULL;
      };
  };

//-------------------------------------------------------------------------

class CQueryBlk
  {
  DEFINE_MEMSTATS(CQueryBlk)

  friend class CQueryHist;
  friend class CQuerySlice;
  private:
    flag           bAll;             //must all values regardless of time be returned
    pCQueryHist    pQ;               //pointer to the main source query
    HANDLE         hBlkFile;         //handle for event block file
    HANDLE         hBlkFileMap;      //handle for event block mem-mapped file
    byte*          pBlkMap;          //pointer to memory mapping
    DWORD          dwFileAllocLen;   //number of bytes allocated to memory mapped file
    DWORD          dwMapSize;        //number of bytes allocated to the mappping
    DWORD          dwMapPos;         //position in file at which the mapping starts
    pHD_TimeSliceHead pTS;           //pointer to current time slice
  protected:
    DWORD          dwTSPos;          //position of current time slice in file
    pCCatItem      pCatItem;         //pointer to the corresponding catalog item
    byte           iVerNo;           //copy from EvHead, historian version number
    WORD           iJmpCount;        //copy from EvHead, number of jumps used
    double         JmpTimes[MaxJmps];//copy from EvHead, array of times for spaced time slices
    DWORD          JmpPos[MaxJmps];  //copy from EvHead, array of positions for spaced time slices

  public: 
    CQueryBlk();
    ~CQueryBlk();
    flag Setup(long CatIndex);
    void GetQuery();
    flag GetLesser();
    flag GetGreater();
    inline void SetQueryMain(pCQueryHist Q) 
      { pQ = Q; };
  private:
    void CloseBlk();
    void GetQueryTS();
    void GetFirstTS();
    void GetNextTS();
    void GetPrevTS();
    void GetNearestTS();
    inline flag IsNextTS()
      { return (pTS->iNext!=0); };
    /*inline flag IsPrevTS();
      { return (pTS->iPrev); };*/
  };

//-------------------------------------------------------------------------

class CQueryHist
  {
  DEFINE_MEMSTATS(CQueryHist)

  friend class CHistorian;
  friend class CQuerySlice;
  friend class CQueryBlk;
  protected:
    double           dStartTime;          //start time of requested query
    double           dLastTime;           //end time of requested query
    short            iAllocCnt;           //memory allocated for slot list
    short            iTagCnt;             //number of slots/tags to be queried
    CLMatrix         Slots;               //array of slot numbers and trend nos for each slot
    DWORD*           SlotIDs;             //array of slot ID's
    flag*            SlotFlags;           //array of slot flags
    CQueryBlk        Q;                   //block file being queried
    DWORD            bGetGreater:1,
                     bGetLesser:1,
                     bBusyServicing:1,
                     bMustFlush:1,
                     bCancel:1,
                     bStarted:1,
                     bBackward:1,
                     bDoHeadings:1,
                     bDoneHeadings:1,
                     bGettingOuter:1,
                     bIsDData:1,
                     bIsLData:1,
                     bIsSData:1,
                     bTimeOptFull:1,
                     bPrevTimeStartStop:1,
                     bDoLastValForFirstPoint:1; //when getting AveEqualySpaced, for the first point rather get the LastEqualySpaced value

    byte             iTimeOptUnits;
    short            iFlagsFound;         //used for finding lesser/greater 
    CQueryHist*      pNext;               //pointer to next query in list
    CXM_Route        xRoute;              //route to which query results must be sent 
    CXMsgLst         XMr;                 //message being built
    long             lRqstNo;             //
    long             lSrcID;              //unique ID from source of query
    QueryDestination QueryDst;            //destination of query
    QueryOption      QueryOpt;            //
    CDVector         DData;               //array for storing double results for single time slice
    CLVector         LData;               //array for storing long results for single time slice
    //CSVector         SData;               //array for storing string results for single time slice
    CStringArray     SData;
    double           dRepTimeOffset;      //adjust all times returned by this amount
    CDVector         dPrevTime;           //time of result set
    CDVector         dPrevVal;            //only used by AveEqualySpaced and LastEqualySpaced queries
    CDVector         dSigVal;             //only used by AveEqualySpaced queries
    double           dTimeDelta;          //
    double           dWrkTim;             //
    long             lNoPts;              //
    long             iTimeSliceCnt;       //time slice count for AveEqualySpaced and LastEqualySpaced queries
    byte             iFileFormat;         //
    FILE*            fFile;               //file handle if query going directly to file
    CString          sFilename1;          //
    CString          sFilename2;          //

  public:
    CQueryHist( double StartTime,
                double LastTime,
                CXM_Route &Route,
                long RqstNo,
                long SrcID,
                flag GetGreater,
                flag GetLesser,
                short TagCnt = 0);
    CQueryHist( double StartTime,
                double LastTime,
                QueryOption Opt,
                byte QryTimeOptUnits,
                flag QryTimeOptFull,
                flag Headings,
                long NoPts,
                CXM_Route &Route,
                long SrcID,
                QueryDestination QDst,
                char* pFilename1,
                char* pFilename2,
                byte FileMode,
                double RepTimeOffset);
    ~CQueryHist();
    flag AddTag(char* Tag, long TrndNo);
    void GetQuery();
  private:
    void Initialise();
    void Send(CXM_QueryString * xb);
    void SendSlice();
    void SendSliceOLE();
    void SendSliceOther();
    void BlockUnavailable(flag &OK);
    void BlockUnavailableTrend(const char* pBlkName);
    void BlockUnavailableOther(const char* pBlkName);
    flag GetLesser(long Index);
    flag GetGreater(long Index);
    void GetQuery_PartOne();
    void GetQuery_PartTwo();
    void GetQuery_PartThree();
    void GetQuery_PartFour();
    void GetQuery_PartFive();
    void GetQuery_Error(long ErrorNumber);
  protected:
    long GetIndex(DWORD ID);
    void RetVal(DWORD ID, double Time, rDataUnion Val);

    #if dbgHstQuery
    CFile* DbgDmpFile;
    #endif
  };

//-------------------------------------------------------------------------

class CQuerySlice
  {
  DEFINE_MEMSTATS(CQuerySlice)

  private:
    pHD_TimeSliceHead pTS;         //pointer to the time slice header being queried
    byte*             pData;       //points to the timeslice data, directly after the header
    DWORD             lID;         //the current slot ID
    DWORD             lQID;        //the current slot ID to be found for the query
    long              lQIDCnt;     //index into list of slot IDs to be queried
    WORD              lPos;        //current position in timeslice data
    WORD              lPrevDeltaTimePos; //for backward queries, pos of prev delta time in timeslice
    double            dTime;       //current time
    DataUnion         Val;         //current value, obtained from timeslice data
    pCQueryHist       pQ;          //pointer to the main source query
    pCQueryBlk        pQB;         //pointer to the source query block
    flag              bAll;        //must all values regardless of time be returned
    flag              bGetClosest; //trying to find value lesser or greater than a given time
    pCEvBlk           pLockBlk;    //points to an event block that was locked

  public:
    CQuerySlice(pCQueryHist Q, pCQueryBlk QB);
    ~CQuerySlice();
    void SetSlice(pHD_TimeSliceHead TS, flag All = true, flag GetClosest = false);
    void GetQuery(flag Backward);
    void DumpCompleteSlice(flag Backward);
  private:
    void Skip();
    void SkipTime();
    void SkipBackTime();
    void SkipSkipTime();
    void Get();
  };

//-------------------------------------------------------------------------

#ifndef __HSTMAIN_H
#include "hstmain.h"
#endif

//---------------------------------------------------------------------------

inline void CQuerySlice::SetSlice(pHD_TimeSliceHead TS, flag All, flag GetClosest)
  {
  pTS = TS;
  bAll = All;
  bGetClosest = GetClosest;
  pData = (byte*)pTS + sizeof(HD_TimeSliceHead);
  lPos = 0;
  lID = 0;
  }

//---------------------------------------------------------------------------

inline void CQuerySlice::Skip()
  {
  byte b = (pData[lPos] & 0xE0);
  DWORD ToSkip = (DWORD)(pData[lPos++] & 0x1F);
  if (b == TS_SKIP_0)
    {
    }
  else if (b == TS_SKIP_1)
    {
    //ToSkip |= (_lrotl((DWORD)pData[lPos], 5));
    ToSkip |= (DWORD)pData[lPos] << 5;
    lPos ++;
    }
  else if (b == TS_SKIP_2)
    {
    //ToSkip |= (_lrotl((DWORD)*((WORD*)&pData[lPos]), 5));
    ToSkip |= (DWORD)*((WORD*)&pData[lPos]) << 5;
    lPos += 2;
    }
  else
    {
    //ToSkip |= (_lrotl((*((DWORD*)&pData[lPos-1]) & 0x00FFFFFF), 5));
    ToSkip |= (*((DWORD*)&pData[lPos-1]) & 0x00FFFFFF) << 5;
    lPos += 3;
    }
  lID += ToSkip + 1;
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

inline void CQueryBlk::GetFirstTS()
  {
  dwTSPos = sizeof(HD_EvBlkHead);
  //todo what if there are none ???
  GetQueryTS();
  }

//-------------------------------------------------------------------------

inline void CQueryBlk::GetNextTS()
  {
  if (pTS->iNext==0)
    {
    pTS = NULL;
    return;
    }
  dwTSPos += pTS->iNext;
  GetQueryTS(); 
  }

//-------------------------------------------------------------------------

inline void CQueryBlk::GetPrevTS()
  {
  if (pTS->iPrev==0)
    {
    pTS = NULL;
    return;
    }
  dwTSPos -= pTS->iPrev; 
  GetQueryTS(); 
  }

//-------------------------------------------------------------------------

#undef DllImportExport

#endif
