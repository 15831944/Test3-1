// Device.h: interface for the XDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICE_H__7416B0A2_AD90_4F96_9B1B_A1C6090DBA82__INCLUDED_)
#define AFX_DEVICE_H__7416B0A2_AD90_4F96_9B1B_A1C6090DBA82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// =======================================================================
//
//
//
// =======================================================================
#include <atlbase.h>
//extern CComModule _Module;
#include <atlcom.h>
#include "connectionpoint.h"
#include "advisesink.h"
#include "slot.h"

// -----------------------------------------------------------------------

class COPCDevice
  {
  public:
    COPCDevice(LPCSTR Name);
	  virtual ~COPCDevice();

    CString           m_sName;
    OPCServer         m_OpcServer;      // the connected server
    OPCGroupStateMgt  m_OpcGroup;       // the single group on this server

    // OPC 1.0 IDataObject
    DWORD             m_dwConnection1;
    DWORD             m_dwConnection2;
    OPCHANDLE         m_GroupHandle;
    COPC_AdviseSink   *m_pTestSink;

    // OPC 2.0 ConnectionPoint
    COPC_Callback     *m_pCallbackCP;
    COPC_Shutdown     *m_pShutdownCP;
    DWORD             m_dwShutdownConnection;
    BOOL              m_bUsingCP;

    //BOOL              m_InitValSrcCache;
    //long              m_InitValSrcTimeout;

    //CString           lastServer;
    //CString           lastNode;

    HRESULT           Connect (LPCSTR ProgID, LPCSTR GroupName, LPCSTR Node, CLSID &Clsid,
                               DWORD RqdRate, DWORD ClsCtx = 0, BOOL InitValCache = false, long InitValTimeout = 100);
    HRESULT           Disconnect(void);
  };

// -----------------------------------------------------------------------

class CDeviceCfg 
  {
  public:
	  CDeviceCfg(LPCSTR Name);
	  ~CDeviceCfg();
    CDeviceCfg(const CDeviceCfg & V);
    CDeviceCfg & operator =(const CDeviceCfg & V);

    long            m_lDeviceNo;
    CString         m_sServerName;
    CString         m_sProgID;
    CString         m_sGroupName;      //??
    CString         m_sNode;
    DWORD           m_dwUpdateRate;
    DWORD           m_dwTrickleCount;
    DWORD           m_dwTrickleIndex;
    bool            m_bLocalOnly;
    bool            m_bSyncIO;

        // DeadBand on Write
    double          m_dDeadBandPercent;   // As Percent = 0 Disable
    long            m_lDeadBandForceCount; // Write at least every N changes  -1 = No Force, 0 = Always

  };

class CDevice : public CDeviceCfg
  {
  public:
	  CDevice(LPCSTR Name);
	  virtual ~CDevice();

    bool            m_bInUse;       // Is Used in Current Config
    CLSID           m_Clsid;
    bool            m_bConnected;

    COPCDevice      m_OPC;

 
    long            m_Indices[MaxWorkSlots]; // References to Slots
    long            m_No;                    // Number of items in grou

    bool            ReadConfig(LPCSTR DevCfgFile);
    bool            WriteConfig(LPCSTR DevCfgFile);
    bool            Connect();
    bool            DisConnect();
    bool            AddSlots(long No, long *Indices);
    bool            AddSlotsReconnect(void);  // Adds the items again after a reconnect
    bool            RemoveSlots(long No, long *Indices);
    bool            SetClientHandles(long No, OPCHANDLE *hServer, OPCHANDLE * hClient);
    bool            Connected() { return  m_OPC.m_OpcServer.IsOk()!=0; }
    bool            ServerGood(void); // Mark West testing
    bool            SetActiveState(bool On);
    long            FlushWriteList();
                  
                  
   // CWriteRqst      * AppendWriteRqst(long Slot, long hServer, VARIANT & m_vValue);
    void            AppendWriteRqst(CSlot & Slot, OPCHANDLE hServer, VARIANT & m_vValue, bool Refresh=false);
    long            ClearWriteList()       { return m_WriteList.Clear(); };
                  
    void            Save(CProfINIFile & SavePF);
    void            Load(CProfINIFile & SavePF);

    long            getSlotCount() const;
    CSlot     *     getSlot(long Index) const;
    void            putSlot(long Index, CSlot * pSlot);
    long            getDeviceCount() const;
    CDevice   *     getDevice(long Index) const;
    void            putDevice(long Index, CDevice * pDevice);

    __declspec(property(get=getSlotCount))             long NSlots;
    __declspec(property(get=getSlot,put=putSlot))      CSlot* Slots[];
    __declspec(property(get=getDeviceCount))           long NDevices;
    __declspec(property(get=getDevice,put=putDevice))  CDevice* Devices[];

    //CKwikList <CWriteRqst *> m_WriteList;
    CKwikList <CChangeItem *> m_WriteList;
  };


// =======================================================================
//
//
//
// =======================================================================
//class CDevices : public CArray <CDevice*, CDevice*> {};
class CDeviceCfgArray : public CArray <CDeviceCfg*, CDeviceCfg*> {};
class CDeviceArray : public CArray <CDevice*, CDevice*> {};

// =======================================================================
//
//
//
// =======================================================================

#endif // !defined(AFX_DEVICE_H__7416B0A2_AD90_4F96_9B1B_A1C6090DBA82__INCLUDED_)
