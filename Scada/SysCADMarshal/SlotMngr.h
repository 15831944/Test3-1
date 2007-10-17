// SlotManager.h: interface for the CSlotMngr class.
//
//////////////////////////////////////////////////////////////////////////
#if !defined(AFX_SLOTEXEC_H__29584858_60E8_4E6C_A516_CFE571FFC312__INCLUDED_)
#define AFX_SLOTEXEC_H__29584858_60E8_4E6C_A516_CFE571FFC312__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//UINT AFXAPI HashKey(LPSTR Key) ;
//UINT AFXAPI HashKey(LPCSTR Key);
//UINT AFXAPI CompareElements(LPSTR* E1, LPSTR* E2);
//UINT AFXAPI CompareElements(LPCSTR* E1, LPCSTR* E2);
#include "slot.h"
#include "Link.h"
#include "device.h"
#include "SysCADMarshal_i.h"
#include "ScdMarshal.h"

// =======================================================================
//
//
//
// =======================================================================

class CCfgBlock
  {
  public:
    CCfgBlock(void);
    virtual     ~CCfgBlock(void);
    CCfgBlock & operator=(const CCfgBlock & C);

    void        SetDefault(void);
    void        Read(LPCSTR DevCfgFile);
    void        Write(LPCSTR DevCfgFile);
    void        Dump(void);

    CString     m_sCfgFile;
    CString     m_sCfgPath;

    DWORD       m_nPriorityThread;
    DWORD       m_nPriorityClass;
    DWORD       m_dwDelayResolution;
    DWORD       m_dwChangeFlushCycle;
    DWORD       m_dwStatusUpdateCycle;
    DWORD       m_dwMaxChangesInCall;
    DWORD       m_dwConditioningCycle;

    // these are not yet used but probably should be
    //double      m_dFloatDeltaChange_Rd;
    double      m_dFloatDeltaChange;
    //double      m_dFloatDeltaPerc;
    double      m_dFloatDeltaDT;
    double      m_dFloatDeltaMult;

    DWORD       m_dwReConnHold;
    DWORD       m_dwReConnItemBlk;
    DWORD       m_ClsCtx;

  };

// =======================================================================
//
//
//
// =======================================================================

const byte  OpcThreadingSTA = 1;
const byte  OpcThreadingAPART = 2;
const byte  OpcThreadingMTA = 3;

//class CTouchList : public CArray<long, long>
//  {
//  public:
//    CTouchList(void)          { m_lCurrentLength = 0; };
//
//    long  Add(long i)         { SetAt(m_lCurrentLength++, i); return m_lCurrentLength; };
//    long  CurrentLength(void) { return m_lCurrentLength; };
//    void  ResetLength(void)   { m_lCurrentLength = 0; };
//  protected:
//    long  m_lCurrentLength;
//  };

enum eDoingCfg { eDoingCfgLoad, eDoingCfgReLoad, eDoingCfgStop };

class CSetValuesOptions 
  {
  public :
    enum eAction  { eDoNothing, eWrite, eRead }; 
    enum eWhich   { eAll, eFiltered, eSelected, eGroups }; 
    CSetValuesOptions ()
      {
      }
    eAction   m_eAction;
    eWhich    m_eWhich;
  };


class CSlotMngr : public CNodeXRefMngr
  {
  friend class CSlotConnect;
  public:
    CSlotMngr(void);
    virtual     ~CSlotMngr(void);

  protected:
    void        Start(void);
    void        Stop(void);
    void        CfgBegin(eDoingCfg Doing, bool OnTheFly);
    void        CfgEnd(eDoingCfg Doing);
    void        DoSetSimulateMode(boolean On);
    void        DoGetSetValues(CSetValuesOptions & Opt);
    void        Save(LPCTSTR Name);
    void        Load(LPCTSTR Name);
    void        CheckDevices(void);

    void        EditSlot(bool DeleteIfFound, CSlotCfgStrings *pCfg);
    void        AddCdBlk(CAddCdBlk *pAdd);
    long        AddLink(CSlot * pSlot, LPCSTR Tag, LPCSTR ConnectTag, LPCSTR Cnv, VARTYPE VType, bool Get, bool Set);

    void        CompressSlotList(void);
    void        ConnectUsedSlots(void);
    void        DisConnectUnusedSlots(void);

    void        RemoveConnects(void);
    void        InitialiseConnects(void);
    void        ConnectConnects(void);

    void        FindDevicesForSlots(void);
    void        SetClientHandlesInServer(void);
    void        CompressDeviceList(void);
    void        ConnectUsedDevices(void);
    void        DisConnectUnusedDevices(void);

    void        CompressLinkList(void);
    void        MarkLinksAsLocal();

    long        FindDevice(LPCSTR Tag, bool AllowCreate = false);
    long        FindSlot(LPCSTR Tag);
    long        FindCdBlk(LPCSTR Tag);
    long        FindLink(LPCSTR Tag);

    long        FlushChangeQueue(void);
    long        FlushWriteList(void);
    long        FlushHistoryQueue();
    long        FlushErrorQueue();
    bool        ApplyChange(CChangeItem *pChg, bool IsDelay);

    void        DumpSlotInfo(LPCTSTR Where);

    void        MyPostThreadMessage(DWORD m, WPARAM w, LPARAM l)
                  {
                  if(!PostThreadMessage(m_dwThreadId, m, w, l))
                    {
                    CString S;
                    S.Format("Message not Posted");
                    AfxMessageBox(S, MB_OK);
                    };
                  }
  public:
    //void        ConnectSimulator(CScdMarshal * p);
    void        StartThread(LPCSTR pCfgFile);
    void        StopThread(void);
    void        SendUpdateStatus();
    void        MessageHandler(void);
    static UINT staticMessageHandler(LPVOID pMngr);
    CString     FullFileName(LPCSTR Fn);

    void        AddDelayedChange(CChangeItem * pNew, DWORD Timer);
    void        AppendChange(eConnSrcDst Src, long SrcI, eConnSrcDst Dst,long DstI, DWORD TransID, CFullValue  &Value, CDelayBlock *pDelay, bool OverrideHold=false, bool Refresh=false);
    long        RequestQueueFlush(void);
    long        GetTransactionID(void)  { return m_dwTransactionID++; };

    void        StartConfiguration(eDoingCfg Doing) { MyPostThreadMessage(WMU_CFGBEGIN, Doing, 0); }
    void        EndConfiguration(eDoingCfg Doing)   { MyPostThreadMessage(WMU_CFGEND, Doing, 0); }
    void        ConfigureCdBlk(CAddCdBlk * pAdd)  { MyPostThreadMessage(WMU_ADDCDBLK, 0, (LPARAM) pAdd); };
    void        ConfigureSlot(bool OnTheFly, CSlotCfgStrings* pCfg)  { MyPostThreadMessage(WMU_ADDSLOT, 0, (LPARAM) pCfg); };
    void        SetSimulateMode(boolean On)  { MyPostThreadMessage(WMU_SETSIMMODE, On?1:0, 0); };


    void        EditStart()                      { MyPostThreadMessage(WMU_EDITSTART, 0, (LPARAM) 0); };
    void        EditSlot(CSlotCfgStrings* pCfg)  { MyPostThreadMessage(WMU_EDITSLOT, 0, (LPARAM) pCfg); };
    void        EditEnd()                        { MyPostThreadMessage(WMU_EDITEND, 0, (LPARAM) 0); };
    
    //    void        AttachSimulator(CScdMarshal * pSim);

    void        SetDefaultConfig(void)            { m_Cfg.SetDefault(); }
    void        DumpConfig(void)                  { m_Cfg.Dump(); }
    void        ReadConfiguration(LPCSTR DevCfgFile)     
      { 
      m_Cfg.Read(DevCfgFile); 
      for (int i=0; i<NDevices; i++)                              
        Devices[i]->ReadConfig(DevCfgFile);
      }
    void        WriteConfiguration(LPCSTR DevCfgFile)    
      {
      m_Cfg.Write(DevCfgFile); 
      for (int i=0; i<NDevices; i++)
        Devices[i]->WriteConfig(DevCfgFile);
      }

    void        SetVisibleItems(long What, bool ForceUpdate, long Count, long * pList);
    void        ApplySetVisible(long What, bool ForceUpdate, long Count, long * pList);
    void        SetHold(eConnSrcDst Src, long Item, bool On);
    void        ApplyHold(eConnSrcDst Src, long Item, bool On);

    //void        SetFind(long What, LPCTSTR Str);
    //void        ApplyFind(long What, LPCTSTR Str);

    double      CalcFloatTolerance(double Range, double TimeDiff)
      { return Range*m_Cfg.m_dFloatDeltaChange * ((TimeDiff > m_Cfg.m_dFloatDeltaDT) ? m_Cfg.m_dFloatDeltaMult : 1.0); };

    DWORD                     ThreadId(void)  { return m_dwThreadId; };

    bool        FindTagHandle(LPCSTR Tag, DWORD &dwHandle);
    VARTYPE     GetTypeFromHandle(DWORD dwHandle);
    bool        GetValueFromHandle(DWORD dwHandle, VARIANT &Value, bool ForceRead);
    bool        SetValueFromHandle(eConnSrcDst Src, DWORD dwHandle, VARIANT Value);

    long        WriteAllSlots2Devices();
    long        TrickleRefresh();
    void        SendSimValue2Link(eConnSrcDst Src, long LnkI, DWORD TransID, CFullValue &FullValue, bool OverrideHold=false, bool Refresh=false);
    void        ProcessCondBlks();

    //CNodeXrefMngr overrides
    virtual LPSTR   GetOwnerTag()    { return "Marshal";};


    CCfgBlock                 m_Cfg;

    //protected:
    bool                      m_bRunning;
    bool                      m_bRunningB4Cfg;
    bool                      m_bDoingFullCfg;
    CCdBlkArray               m_CdBlks;
    CCdBlkMap                 m_CdBlkMap;
    CSlotArray                m_Slots;
    CSlotMap                  m_SlotMap;
    CDeviceArray              m_Devices;
    CLinkArray                m_Links;
    CLinkMap                  m_LinkMap;
    CLongArray                m_SlotsSelected;
    CLongArray                m_LinksSelected;
    long                      m_nSlotsSelected;
    long                      m_nLinksSelected;

    CLongArray                m_LinksWithCondBlks;

    DWORD                     m_dwLastChangeFlushTicks;
    DWORD                     m_dwStatusSendTicks;
    DWORD                     m_dwFlushChangeCalls;
    DWORD                     m_dwLastCBProcessTime;

    bool                      m_bMonitorOn;

    long                      m_lCfgSequence;

    DWORD                     m_dwTransactionID;

    bool                      m_bRunOnMainThread;
    DWORD                     m_dwThreadId;
    CWinThread                *m_pExecThread;
    CCriticalSection          m_MsgLoopLock;
    //CSingleLock               m_Lock;
    DWORD                     m_dwLastTickCount;

    DWORD                     m_dwTotalHolds;
    DWORD                     m_dwTotalChanges;
    CKwikList<CChangeItem *>  m_ChangeList;
    CKwikList<CChangeItem *>  m_DelayChgList;
    CKwikList<CChangeItem *>  m_HistoryList;

    bool                      m_bScdRunning;

    CStatusInfoBlock          m_Stats;

    //CScdMarshal              *m_pSimulatorReqd;
    //CScdMarshal              *m_pSimulator;

    long                      getSlotCount(void) const;
    CSlot                     *getSlot(long Index) const;
    void                      putSlot(long Index, CSlot *pSlot);
    long                      getLinkCount(void) const;
    CLink                     *getLink(long Index) const;
    void                      putLink(long Index, CLink *pLink);
    long                      getDeviceCount(void) const;
    CDevice                   *getDevice(long Index) const;
    void                      putDevice(long Index, CDevice *pDevice);

    bool                      getScdRunning() const;
    void                      putScdRunning(bool Running);

    __declspec(property(get = getSlotCount))                          long  NSlots;
    __declspec(property(get = getSlot, put = putSlot))                CSlot *Slots[];
    __declspec(property(get = getLinkCount))                          long  NLinks;
    __declspec(property(get = getLink, put = putLink))                CLink *Links[];
    __declspec(property(get = getDeviceCount))                        long  NDevices;
    __declspec(property(get = getDevice, put = putDevice))            CDevice *Devices[];
    __declspec(property(get = getScdRunning, put = putScdRunning))    bool ScdRunning;
  };

// =======================================================================
//
//
//
// =======================================================================

extern CSlotMngr  gs_SlotMngr;

// =======================================================================
//
//
//
// =======================================================================

#ifdef _DEBUG
#define WITHLOCKMESSAGES 0
#else
#define WITHLOCKMESSAGES 0
#endif

class CMsgLoopLock : CSingleLock
  {
  public:
    CMsgLoopLock(LPCSTR dbg) :
      CSingleLock(&gs_SlotMngr.m_MsgLoopLock) 
      {
      #if WITHLOCKMESSAGES
      dbgpln("-----------------------------------------Lock   %08x %s ", GetCurrentThreadId(), dbg);
      #endif
      Lock();
      #if WITHLOCKMESSAGES
      dbgpln("                                         Done   %08x ", GetCurrentThreadId());
      #endif
      };
    ~CMsgLoopLock() 
      {
      #if WITHLOCKMESSAGES
      dbgpln("-----------------------------------------Unlock %08x ", GetCurrentThreadId());
      #endif
      }
  };
    
// =======================================================================
//
//
//
// =======================================================================
#endif // !defined(AFX_SLOTEXEC_H__29584858_60E8_4E6C_A516_CFE571FFC312__INCLUDED_)
