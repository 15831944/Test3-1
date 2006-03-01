//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __HSTMSG_H
#define  __HSTMSG_H

#if ForSysCAD
#ifndef __TAGOBJ_H
#include "tagobj.h"
#endif
#else
#ifndef __HSTDATYP_H
#include "hstdatyp.h"
#endif
#ifndef __HSTVECT_H
#include "hstvect.h"
#endif
#endif
#ifndef __AFXTEMPL_H__
#include <afxtempl.h> // "xafxtempl.h"
#endif

#ifdef __HSTMSG_CPP
  #define DllImportExport DllExport
#elif !defined(SCHIST)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


//const unsigned char HstVerNo = 3;
const unsigned char HstVerNo = 4; //changed 28/11/95
const HD_SlotSpace = 512;

//=========================================================================

const unsigned char TS_BIT            = 0x00;
const unsigned char TS_BIT_OFF        = 0x00;
const unsigned char TS_BIT_ON         = 0x01;
const unsigned char TS_DOUBLE         = 0x02; // 8 bytes  double
const unsigned char TS_FLOAT          = 0x03; // 4 bytes  float
const unsigned char TS_LONG           = 0x04; // 4 bytes  long
const unsigned char TS_WORD           = 0x05; // 2 bytes  unsigned short
const unsigned char TS_BYTE           = 0x06; // 1 bytes  unsigned char
const unsigned char TS_FLT16          = 0x07; // "special SysCAD type"
//const unsigned char TS_INT            = 0x08; // 4 bytes  int : 21 Nov 2001 changed this to dword
const unsigned char TS_DWORD          = 0x08; // 4 bytes  unsigned long
const unsigned char TS_INT16          = 0x09; // 2 bytes  short
const unsigned char TS_STR            = 0x0A; // ? bytes  NULL terminated string

const unsigned char TS_START          = 0x0B;
const unsigned char TS_STOP           = 0x0C;
const unsigned char TS_TIMESKIP       = 0x0D;
const unsigned char TS_END            = 0x0E;
const unsigned char TS_EXTENDED       = 0x0F;

const unsigned char TS_STATUS_VALID   = 0x00;
const unsigned char TS_STATUS_UNDER   = 0x20;
const unsigned char TS_STATUS_OVER    = 0x40;
const unsigned char TS_STATUS_INVALID = TS_STATUS_UNDER | TS_STATUS_OVER; //==0x60;
const unsigned char TS_SKIP           = 0x80;
const unsigned char TS_SKIP_0         = 0x80;
const unsigned char TS_SKIP_1         = 0xA0;
const unsigned char TS_SKIP_2         = 0xC0;
const unsigned char TS_SKIP_3         = 0xE0;
const unsigned char TS_TIMESKIP_0     = 0x0D;
const unsigned char TS_TIMESKIP_1     = 0x2D;
const unsigned char TS_TIMESKIP_2     = 0x4D;

const unsigned char HST_ERR_OPENHED          = 0x01;
const unsigned char HST_ERR_OPENCAT          = 0x02;
const unsigned char HST_ERR_OPENEVBLK        = 0x03;
const unsigned char HST_ERR_OPENEVBLK_CREATE = 0x04;
const unsigned char HST_ERR_NOTEXISTS_HED    = 0x05;
const unsigned char HST_ERR_NOTEXISTS_CAT    = 0x06;

const long DefaultSliceSize = 32768; // CNM changed from 4K //4K
const long DefaultBlkSize = 65536; //64K
const short DefaultTimeCnt = 100;

//historian "compresion" algorithm...
const unsigned char HSA_None           = 0;
const unsigned char HSA_Boxcar         = 1;
const unsigned char HSA_AdaptiveBoxcar = 2;

//historian filter algorithm...
const unsigned char HSFA_None          = 0x0;
const unsigned char HSFA_MovingAve     = 0x1;
const unsigned char HSFA_Tau           = 0x2;
//const unsigned char HSFA_AveAndTau     = 3;

extern const char* CopyNotice;
extern const char* NewBlkKey;

enum QueryDestination { QTrend, QFile, QDDE, QOLE, QFileDirect };
enum QueryOption { All, AveEqualySpaced, LastEqualySpaced };

//=========================================================================
//=========================================================================

_FWDDEF(HD_SlotInfo)
_FWDDEF(HD_EvBlkHead)
_FWDDEF(HD_TimeSliceHead)
_FWDDEF(HD_CatItem)
_FWDDEF(HD_CatHead)
_FWDDEF(HD_Hist)
_FWDDEF(SlotCfgInfo)

//-------------------------------------------------------------------------

class DllImportExport SlotCfgInfo
  {
  public:
    //general...
    char*     mTag;
    char      mTyp;
    char*     mEng;
    char*     mCnvFam;
    char*     mDesc;
    UCHAR     mDriver:1,      //config comes from drivers
              mRecordIt:1;    //must it be recorded
    //filter...
    unsigned char mUseFilter;
    unsigned char mFilterAlg;
    double        mFilterTau;
    double        mWinFltPeriod;
    short         mWinFltCount;
    double        mFltDelta;
    //boxcar...
    unsigned char mUseBoxcar;
    unsigned char mBoxcarAlg;
    double        mDeltaLo;
    double        mDeltaHi;
    WORD          mNoRec;
    WORD          mNoNotRec;
    double        mChange;
    
    SlotCfgInfo();
  };

//-------------------------------------------------------------------------

class HD_Hist
  {
  public:
    char          sCopyNotice[80];  //Kenwalt copyright notice
    long          lReqdBlkSize;     //size of mem-mapped event blocks to be created
    long          lSlotsUsed;       //number of slots used
    short         iMaxDeltaTimeCnt; //max no of time delta's in a time slice
    long          lMaxTimeSliceLen; //end time slice if data len exceeds this value
    WORD          iCurCatNo;        //catalog number of the current EvBlk
    DWORD         lNxtSlotID;       //ID to be given to next new slot
    char          sScenName[9];     //current scenario name
    char          sSpare[500];      //space for future stuff to allow backward compatability
    UCHAR         bWrapAround:1,
                  bSyncWithClock:1, // NBNB This is never set by anything
                  bDidClose:1;      //unsigned char set to 1 if historian closed correctly
    unsigned char iVerNo;           //historian version number

    HD_Hist();
    void dbgDump(char* p);
    };

//-------------------------------------------------------------------------

class HD_CatItem
  {
  public:
    WORD      iCatNo;        //catalog number
    char      sScenName[9];  //scenario name
    double    dStartTime;
    double    dLastTime;
    long      lCreateTime;
    char      sSpare[16];

    HD_CatItem() 
      {
      iCatNo = 0;
      dStartTime = 0.0;
      dLastTime = 0.0;
      lCreateTime = 0;
      sScenName[0] = 0;
      sScenName[sizeof(sScenName)-1] = 0;
      memset(sSpare, 0, sizeof(sSpare));
      };
  };

//-------------------------------------------------------------------------

class HD_CatHead
  {
  public:
    char          sCopyNotice[80]; //Kenwalt copyright notice
    WORD          iCatItemCnt;     //total number of cat items in catalog file
    //NOT USED kga 7/12/00  WORD      iNextCatNo;      //next CatNo to be used
    WORD          iNextCatNo_;      //next CatNo to be used
    unsigned char iVerNo;          //historian version number
    char          sSpare[200];     //space for future stuff to allow backward compatability
    
    HD_CatHead();
    void dbgDump();
  };

//-------------------------------------------------------------------------

class HD_FilterInfo
  {
  public:
    double        dFilterTau;         //
    double        dWinFltPeriod;      //   , Alpha = Period/Tau            
    short         iWinFltCount;       //number of points to store for "moving average"           
    DataUnion     FltDeltaVal;        //if diff between prev and cur value < 2 * Delta apply Alpha
    unsigned char iFilterAlg;         //type of filtering to be performed
  };

//-------------------------------------------------------------------------

class HD_BoxcarInfo
  {
  public:
    WORD          iNoRec;           //ABox: no of consecutive records after which DeltaVal must be increased
    WORD          iNoNotRec;        //ABox: no of consecutive records after which DeltaVal must be decreased
    double        dChange;          //ABox: amount by much DeltaVal must be changed up or down
    double        dDeltaLo;         //Box AND ABox:min change after which a value should be stored
    double        dDeltaHi;         //ABox:maximum delta
    double        dDeltaVal;        //ABox:current delta value
    unsigned char iBoxcarAlg;       //type of boxcar/compression to be performed
    char          sSpare[16];
  };

//-------------------------------------------------------------------------

class HD_SlotInfo
  {
  public:
    DWORD         lSlotID;       //unique slot ID
    char          VarTyp;        //type of variable tt_xxx
    char          sTag[128];     //tag asociated with slot
    char          sCnvFam[32];   //conversion family
    char          sEng[32];      //conversion units
    char          sDesc[128];    //tag description
    UCHAR         bUseFilter:1,  //does it use filters
                  bUseBoxcar:1,  //does it use data compression
                  bDriver:1,     //is the source a driver slot
                  bRecordIt:1;   //must the value be recorded

    DataUnion     PrevSaveVal;   //previous value (filtered etc) saved to disk
    DataUnion     PrevMeasVal;   //previous value (filtered etc)
    DataUnion     PrevActualVal; //previous value (filtered etc)
    double        dPrevTime;     //time of prev meas val
    unsigned char iPrevStatus;   //status of prev meas val
    char          sSpare[16];

    HD_SlotInfo() {};
    char* Tag();
    char* CnvFam();
    char* Eng();
    char* Desc();
    void SetTag(char* pTag);
    void dbgDump();
  };

//-------------------------------------------------------------------------

const WORD MaxJmps = 45;

class HD_EvBlkHead
  {
  public:
    char          sCopyNotice[80];  //Kenwalt copyright notice
    double        dStartTime;       //start time of first event
    double        dLastTime;        //time of last event
    WORD          iTimeSliceCnt;    //number of time slices in event block
    WORD          iCatNo;           //catalog number
    char          sScenName[8];     //scenario name
    UCHAR         bFinished:1,      //True if the event block connot be extended
                  bReducedSize:1;   //True if the event block connot be extended
    unsigned char iVerNo;           //historian version number
    WORD          iJmpCount;        //number of jumps used
    double        JmpTimes[MaxJmps];//array of times for spaced time slices (for quick lookup)
    DWORD         JmpPos[MaxJmps];  //array of positions for spaced time slices (for quick lookup)
    char          sSpare[58];       //space for future stuff to allow backward compatability

    HD_EvBlkHead()
      {
      ASSERT(FALSE);
      };
    char* ScenName()
      { 
      static char Buff[sizeof(sScenName)+1]; 
      strncpy(Buff, sScenName, sizeof(sScenName));
      Buff[sizeof(sScenName)] = 0;
      return Buff;
      };
  };

//-------------------------------------------------------------------------

class HD_TimeSliceHead
  {
  public:
    double   dStartTime;     //start time
    double   dLastTime;      //time of last event in time slice
    WORD     iLen;           //length of variable part of record
    WORD     iPrev;          //number of unsigned chars from start of time slice to start of previous time slice
    WORD     iNext;          //number of unsigned chars from start of time slice to start of next time slice
    WORD     iLastDeltaTime; //number of unsigned chars from start of time slice to last DeltaTime in time slice

    HD_TimeSliceHead()
      { 
      ASSERT(FALSE);
      };
  };

//-------------------------------------------------------------------------

class CQueryInfoStats
  {
  public:
    CQueryInfoStats()
      { lID = 0; dPer = 0.0; };
    CQueryInfoStats(long ID, double Per, flag Remote, int Increment)
      { lID = ID; dPer = Per; bRemote = Remote; iIncrement = Increment; };
    long      lID;        //unique ID matching query
    double    dPer;       //percentage complete
    flag      bRemote;    //
    int       iIncrement; //
  };

//-------------------------------------------------------------------------

//=========================================================================
#undef DllImportExport 

#endif

          
