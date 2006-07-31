//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __HSTSLOTS_H
#define  __HSTSLOTS_H

#ifndef __HSTMSG_H
#include "hstmsg.h"
#endif

//=========================================================================

_FWDDEF(CHistorian)
_FWDDEF(CFilter)
_FWDDEF(CBoxcar)
_FWDDEF(CSlot)

class CFilter
  {
  DEFINE_MEMSTATS(CFilter)

  public:
    HD_FilterInfo Data;
  protected:
    short         iSampleNo;   
    short         iNSamples;
    double        dSigma;
    pDataUnion    pList;
    short         iListLen;
    double        dAlpha;
    pCSlot        pSlot;

  public:
    CFilter();
    ~CFilter();
    void Cfg(pSlotCfgInfo pSCI, pCSlot Slot);
    void DoFilter();
    flag Save(CFile & F);  
    flag Load(CFile & F);
  protected:
    void FixList();
  };

//---------------------------------------------------------------------------

class CBoxcar
  {
  DEFINE_MEMSTATS(CBoxcar)

  public:
    HD_BoxcarInfo  Data;       
    double         AveVal;     //total of all values since last save, used for calculating average
    long           AveCnt;     //number of values in total
    long           NoRec;      //number of consecutive changes saved
    long           NoNotRec;   //number of consecutive changes not saved
  protected:
    pCSlot         pSlot;

  public:
    CBoxcar();
    void Cfg(pSlotCfgInfo pSCI, pCSlot Slot);
    flag CheckChanges();
    flag Save(CFile & F);  
    flag Load(CFile & F);
  };

//---------------------------------------------------------------------------

class CSlot
  {
  DEFINE_MEMSTATS(CSlot)

  public:
    HD_SlotInfo   Data;        //slot information which is stored to file
    long          lSNo;        //slot number
    byte          iStatus;     //status of value
    DataUnion     MeasVal;     //current value after filtering etc
    DataUnion     ActualVal;   //current value
    byte          TSFlag;      //TS_xxx type related to tt_xxx type
    byte          bFirstValue:1,
                  bIsRecording:1,    //will not be set True if tag is not found
                  bWriteMidTime:1,   //must write value at MidTime
                  m_bVariesWithinTol:1, // identify an 'analog' vs 'digital'  
                  m_bChangedDuringPause:1;
    long          lScanOfLastChange; //scan number of last change
    long          lScanOfLastSave;   //scan number of last save
    DWORD         dwWriteCnt;  //no of times slot value is written to file
    DWORD         dwChangeCnt; //no of times slot value changes
    int           iCnvNo;      //conversion number

    CFilter*      pFilter;     //pointer to filter
    CBoxcar*      pBoxcar;     //pointer to boxcar/compression algorithm
    
    CSlot();
    ~CSlot();
    void Initialise();
    void Configure(DWORD SlotID, long SNo);
    void Configure(long SNo);
    flag Save(CFile & F, long SlotNo);
    flag Load(CFile & F, long SlotNo);
    void Cfg(pSlotCfgInfo pSCI, flag NewSlot);
    void GetCfg(SlotCfgInfo& SCI);
    void Changed(flag Force = false);
    void SaveEvent();
    void FixTyp();
    void dbgDump(char* p);

    inline unsigned char Status() { return iStatus; };
    inline unsigned char CntrlByte() { return (iStatus | TSFlag); };
    inline DWORD ID() { return Data.lSlotID; };
    inline char* Tag() { return Data.Tag(); };
    inline char Typ() { return Data.VarTyp; };
  };

//=========================================================================

//#ifndef __HSTMAIN_H
//#include "hstmain.h"
//#endif

//-------------------------------------------------------------------------

#undef DllImportExport 

#endif

          
