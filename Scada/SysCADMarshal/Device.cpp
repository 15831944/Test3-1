// Device.cpp: implementation of the XDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SysCADMarshal.h"
#include "Device.h"
#include "slot.h"
#include "slotmngr.h"
//#include "dbgetc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// =======================================================================
//
//
//
// =======================================================================

COPCDevice::COPCDevice(LPCSTR Name):
  m_sName(Name)
  {
  m_GroupHandle = 0;

  //transactionID = 0;
  m_dwConnection1 = 0;
  m_dwConnection2 = 0;
  m_pTestSink = new COPC_AdviseSink;   // create the advise sink for notifications
  m_pTestSink->AddRef();

  m_pCallbackCP = new OPCCallbackObject;   // create the ConnectionPoint for notifications
  m_pCallbackCP->AddRef();
  // m_pCallbackCP->SetAccessControl(&m_CS, &m_hEvent);

  m_pShutdownCP = new OPCShutdownObject;   // create the ConnectionPoint for notifications
  m_pShutdownCP->AddRef();
  m_dwShutdownConnection = 0;
  m_bUsingCP = FALSE;

  AfxOleLockApp();

  //   // Everyone can connect back to IAdviseSink
  //   HRESULT hr = CoInitializeSecurity(NULL, -1, NULL, NULL,
  //      RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IDENTIFY, NULL, EOAC_NONE, NULL);
  //   if (FAILED(hr))
  //   {
  //      TRACE(_T("CoInitializeSecurity failed, %lx"), hr);
  //   }

  };

// -----------------------------------------------------------------------

COPCDevice::~COPCDevice()
  {
  AfxOleUnlockApp();
  if( m_OpcServer.IsOk() )
    Disconnect();

  m_pTestSink->Release(); // OLE should clean this up, but may not have time!
  m_pCallbackCP->Release();
  m_pShutdownCP->Release();
  //   DeleteCriticalSection(&m_CS);
  //   CloseHandle(m_hEvent);
  Sleep( 100 );
  };

// -----------------------------------------------------------------------

const CLSID CLSID_OPCServerList = {0x13486D51,0x4821,0x11D2,{0xA4,0x94,0x3C,0xB3,0x06,0xC1,0x00,0x00}};
UINT OPCSTMFORMATDATA = RegisterClipboardFormat(_T("OPCSTMFORMATDATA"));
UINT OPCSTMFORMATDATATIME = RegisterClipboardFormat(_T("OPCSTMFORMATDATATIME"));
UINT OPCSTMFORMATWRITECOMPLETE = RegisterClipboardFormat(_T("OPCSTMFORMATWRITECOMPLETE"));


HRESULT COPCDevice::Connect (LPCSTR ProgID, LPCSTR GroupName, LPCSTR Node, CLSID &Clsid,
                             DWORD RqdRate, DWORD ClsCtx, BOOL InitValCache, long InitValTimeout)
  {
  USES_CONVERSION;

  if (ProgID && strlen(ProgID)>0)
    {//go get CLSID from the specified ProgID...
    IOPCServerList* pServers=NULL;

    COSERVERINFO si;
    MULTI_QI  qi;

    si.dwReserved1 = 0;
    si.pwszName = NULL;
    if(strlen(Node)>0 )
      si.pwszName = T2OLE(Node);
    si.pAuthInfo = NULL;
    si.dwReserved2 = 0;

    qi.pIID = &IID_IOPCServerList;
    qi.pItf = NULL;
    qi.hr = 0;

    HRESULT hr = CoCreateInstanceEx(CLSID_OPCServerList, NULL, CLSCTX_ALL, &si, 1, &qi);
    if (FAILED(hr) || FAILED(qi.hr))
      {
      CString msg(_T("Please install the OPC 2.0 Components."));
      if( strlen(Node)>0 )
         msg.Format(_T("Please install the OPC 2.0 Components on %s."), Node);
      if( FAILED(hr) )
        ReportError(m_sName, hr , msg);
      else
        ReportError(m_sName, qi.hr, msg);
      return hr;
      }
    CString sProgID = ProgID;
    pServers = (IOPCServerList*)qi.pItf;
    hr = pServers->CLSIDFromProgID(T2OLE(sProgID.GetBuffer(0)), &Clsid);
    pServers->Release();
    if (FAILED(hr))
      {
      ReportError(m_sName, hr,
                  (strlen(Node)>0 ?"Error locating '%s' on %s.":"Error locating '%s'."),
                  (LPCTSTR)sProgID, Node);
      return hr;
      }
    }


  ASSERT( !m_OpcServer.IsOk() );
  HRESULT hr = S_OK;

  // Create a running object from the class ID
  LPUNKNOWN pUnkn = NULL;

  // (CLSCTX_ALL will allow in-proc, local and remote)
  if (ClsCtx==0)
    ClsCtx=CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER | CLSCTX_INPROC_SERVER;

  //m_InitValSrcCache=InitValCache;
  //m_InitValSrcTimeout=InitValTimeout;

  if(strlen(Node)==0)
    {
    hr = CoCreateInstance(Clsid, NULL, ClsCtx/*CLSCTX_ALL*/, IID_IUnknown, (LPVOID *)&pUnkn);
    if( FAILED(hr) || pUnkn == NULL)
      {
         CString msg;
         msg.Format("Error connecting to OPC server '%s'", m_sName );
         msg += _T("CoCreateInstance: ");
         ReportError(m_sName, hr, msg);
         return hr;
      }
    }
  else  // use the node name
    {
    COSERVERINFO si;
    MULTI_QI  qi;

    si.dwReserved1 = 0;
    si.pwszName = NULL;
    if (strlen(Node)>0)
      si.pwszName = T2OLE(Node);
    si.pAuthInfo = NULL;
    si.dwReserved2 = 0;

    qi.pIID = &IID_IOPCServer;
    qi.pItf = NULL;
    qi.hr = 0;

    hr = CoCreateInstanceEx(Clsid, NULL, ClsCtx, &si, 1, &qi);
    if (FAILED(hr))
      {
      CString msg;
      msg.Format("Error connecting to OPC server '%s'", m_sName );
      msg += _T("CoCreateInstance: ");
      ReportError(m_sName, hr, msg);
      return hr;
      }
    if (FAILED(qi.hr))
      {
      ReportError(m_sName, qi.hr, _T("MultiQI: "));
      return hr;
      }
    pUnkn = qi.pItf;
    }

  // Get the IOPCServer interface.
  hr = m_OpcServer.Attach( pUnkn );
  pUnkn->Release();  // Don't need this anymore.
  pUnkn = NULL;
  if( FAILED(hr) )
    {
    CString appName((LPCSTR)AFX_IDS_APP_TITLE);
    MessageBox(0, _T("You may not have registered the OPC Proxy dll!\n"), appName, MB_OK);
    return hr;
    }

  {
  IOPCCommon *pCom= 0;
  hr = m_OpcServer.QueryInterface(IID_IOPCCommon, (void**)&pCom);
  if( SUCCEEDED(hr) )  // This server supports 2.0
    {
    hr = pCom->SetClientName(T2W("SysCADMarshal"));
    pCom->Release();
    if( FAILED(hr) )
      {
      CString appName((LPCSTR)AFX_IDS_APP_TITLE);
      MessageBox(0, _T("You may not have registered the OPC Proxy dll!\n"), appName, MB_OK);
      return hr;
      }
    }
  }


  // OPC 2.0 Server shutdown ConnectionPoint
  {
  IConnectionPointContainer *pCPC = 0;
  hr = m_OpcServer.QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
  if( SUCCEEDED(hr) )  // This server supports 2.0
    {
    IConnectionPoint *pCallbackCP = 0;
    hr = pCPC->FindConnectionPoint(IID_IOPCShutdown, &pCallbackCP);
    pCPC->Release();
    if( SUCCEEDED(hr) )
      {
      hr = pCallbackCP->Advise(m_pShutdownCP, &m_dwShutdownConnection);
      pCallbackCP->Release();
      if( FAILED(hr) )
        {
        ReportError(m_sName, hr, _T("Advise: IID_IOPCShutdown") );
        return hr;
        }
      }
    }
  }

  // Create a single group that will contain all the items
  FLOAT deadband = 0.0;
  DWORD rate;
  BOOL GroupActive = FALSE; //what is best for this???
  hr = m_OpcServer.AddGroup( T2OLE(GroupName)/*L"Scd"*/, GroupActive, RqdRate,  // name, active, rate
                           1324, NULL, &deadband,  // handle, bias, band
                           0, &m_GroupHandle, &rate,
                           IID_IOPCGroupStateMgt,   // interface to return
                           m_OpcGroup );              // this holds the group ptr
  if( FAILED(hr) )
    {
    ReportError(m_sName, hr, _T("AddGroup: ") );
    return hr;
    }

  // OPC 2.0 ConnectionPoints
  IConnectionPointContainer *pCPC = 0;
  hr = m_OpcGroup.QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
  if( SUCCEEDED(hr) )  // This server supports 2.0
    {
    m_bUsingCP = TRUE;
    IConnectionPoint *pCallbackCP = 0;
    hr = pCPC->FindConnectionPoint(IID_IOPCDataCallback, &pCallbackCP);
    pCPC->Release();
    if( FAILED(hr) )
      {
      ReportError(m_sName, hr, _T("FindConnectionPoint: "));
      m_bUsingCP = FALSE;  // Try old style
      }

    hr = pCallbackCP->Advise(m_pCallbackCP, &m_dwConnection1);
    pCallbackCP->Release();
    if( FAILED(hr) )
      {
      ReportError(m_sName, hr, _T("Advise ConnectionPoint: "));
      m_dwConnection1 = 0;
      m_bUsingCP = FALSE;  // Try old style
      }
    }

  if( !m_bUsingCP )
    {
    // OPC 1.0 data advise format
    FORMATETC formatEtc ;

    formatEtc.tymed =  TYMED_HGLOBAL;
    formatEtc.ptd = NULL;
    formatEtc.dwAspect = DVASPECT_CONTENT;
    formatEtc.lindex = -1;

    // IAdviseSink is an interface on OUR object that is passed to
    // the server for callbacks
    IAdviseSink *pAdviseSink = NULL;
    hr = m_pTestSink->QueryInterface(IID_IAdviseSink, (LPVOID *)&pAdviseSink);
    if( FAILED(hr) )
      {
      ReportError(m_sName, hr, _T("IAdviseSink: "));
      m_OpcGroup.Detach();
      m_OpcServer.Detach();
      return hr;
      }

    // Get an IDataObject interface on the group
    DataObject dataObject;
    hr = dataObject.Attach( m_OpcGroup );
    if(FAILED(hr) || !dataObject.IsOk() )
      {
      //  some servers don't do this, so don't quit altogether
      MessageBox( 0, _T("IDataObject not supported by this server\nNo data notifications will take place"), _T("FactorySoft Client"), MB_OK );
      return hr;
      }

    // Register our IAdvise with the group
    // Need to register both formats: data change, and write complete
    formatEtc.cfFormat = OPCSTMFORMATWRITECOMPLETE ;
    hr = dataObject.DAdvise(&formatEtc,
                              ADVF_PRIMEFIRST,    // ADVF flag
                              pAdviseSink,
                              &m_dwConnection2);
    if( FAILED(hr) )
      {
      ReportError(m_sName, hr,  _T("IDataObject::DAdvise: "));
      return hr;
      }

#ifdef DATATIMEFORMAT
    formatEtc.cfFormat = OPCSTMFORMATDATATIME ;
#else
    formatEtc.cfFormat = OPCSTMFORMATDATA ;
#endif // DATATIMEFORMAT
    hr = dataObject.DAdvise(&formatEtc,
                              ADVF_PRIMEFIRST,    // ADVF flag
                              pAdviseSink,
                              &m_dwConnection1);
    pAdviseSink->Release();
    if( FAILED(hr) )
      {
      ReportError(m_sName, hr,  _T("IDataObject::DAdvise: "));
      return hr;
      }
    }
  return hr;
  };

// -----------------------------------------------------------------------

HRESULT COPCDevice::Disconnect(void)
  {
  //   CWaitCursor wait;

  HRESULT hr = S_OK;
  if(m_OpcServer.IsOk() && m_OpcGroup.IsOk())
    {
    if(m_dwShutdownConnection)
      {
      IConnectionPointContainer *pCPC = 0;
      hr = m_OpcServer.QueryInterface(IID_IConnectionPointContainer, (void **) &pCPC);
      if(SUCCEEDED(hr))
        {
        IConnectionPoint  *pCallbackCP = 0;
        hr = pCPC->FindConnectionPoint(IID_IOPCShutdown, &pCallbackCP);
        if(SUCCEEDED(hr))
          {
          hr = pCallbackCP->Unadvise(m_dwShutdownConnection);
          //m_pCallbackCP->Release();
          }

        pCPC->Release();
        }
      }

    if(m_bUsingCP)
      {
      // OPC 2.0 ConnectionPoints
      IConnectionPointContainer *pCPC = 0;
      hr = m_OpcGroup.QueryInterface(IID_IConnectionPointContainer, (void **) &pCPC);
      if(SUCCEEDED(hr) && m_dwConnection1)  // This server supports 2.0
        {
        IConnectionPoint  *pCallbackCP = 0;
        hr = pCPC->FindConnectionPoint(IID_IOPCDataCallback, &pCallbackCP);
        if(SUCCEEDED(hr))
          {
          hr = pCallbackCP->Unadvise(m_dwConnection1);
          //m_pCallbackCP->Release();
          }

        pCPC->Release();
        }
      }
    else
    // call IDataObject::DUnadvise to turn off data notification
      {
      DataObject  dataObject;
      HRESULT     hr = dataObject.Attach(m_OpcGroup);
      if(SUCCEEDED(hr))
        {
        if(m_dwConnection1) hr = dataObject.DUnadvise(m_dwConnection1);
        if(m_dwConnection2) hr = dataObject.DUnadvise(m_dwConnection2);
        dataObject.Detach();
        }
      }

    m_OpcServer.RemoveGroup(m_GroupHandle, FALSE);
    }

  m_OpcGroup.Detach();
  m_OpcServer.Detach();

  Sleep(100);

  // now that the group is released and unadvised, no more data will
  // be sent from the server. It is safe to delete the items
  //   while( !xitems.IsEmpty() )
  //      delete xitems.RemoveTail();
  return hr;
  };

// =======================================================================
//
//
//
// =======================================================================

CDeviceCfg::CDeviceCfg(LPCSTR Name):
  m_sServerName(Name)
  {
  m_lDeviceNo=-1;

  m_dwUpdateRate=250;
  m_sProgID="ScdIODB.OPC.1";
  m_sGroupName="SysCAD";
  m_sNode="";
  m_bLocalOnly=(_stricmp(Name, "Local")==0);
  m_dwTrickleCount=0;
  m_dwTrickleIndex=0;
  m_bSyncIO=false;
  m_dDeadBandPercent=0.001;
  m_lDeadBandForceCount=-1;
  };

CDeviceCfg::~CDeviceCfg()
  {
  };

CDeviceCfg::CDeviceCfg(const CDeviceCfg & V)
  {
  *this=V;
  };
CDeviceCfg & CDeviceCfg::operator =(const CDeviceCfg & V)
  {
  m_lDeviceNo       = V.m_lDeviceNo;    
  m_sServerName     = V.m_sServerName;  
  m_sProgID         = V.m_sProgID;      
  m_sGroupName      = V.m_sGroupName;   
  m_sNode           = V.m_sNode;        
  m_dwUpdateRate    = V.m_dwUpdateRate; 
  m_bLocalOnly      = V.m_bLocalOnly;
  m_dwTrickleCount  = V.m_dwTrickleCount;
  m_dwTrickleIndex  = V.m_dwTrickleIndex;
  m_bSyncIO         = V.m_bSyncIO;
  m_dDeadBandPercent  = V.m_dDeadBandPercent;
  m_lDeadBandForceCount= V.m_lDeadBandForceCount;
  return *this;
  };

// =======================================================================
//
//
//
// =======================================================================

CDevice::CDevice(LPCSTR Name):
  CDeviceCfg(Name), m_OPC(Name)
  {
  m_bInUse=false;

  m_bConnected=false;
  m_No = 0;
  };

// -----------------------------------------------------------------------

CDevice::~CDevice()
  {
  };

// -----------------------------------------------------------------------

bool CDevice::ReadConfig(LPCSTR DevCfgFile)
  {
  CProfINIFile PF(DevCfgFile);

  m_sProgID         = PF.RdStr(m_sServerName,   "ProgID",       "ScdIODB.OPC.1");
  m_sNode           = PF.RdStr(m_sServerName,   "Node",         "");
  m_sGroupName      = PF.RdStr(m_sServerName,   "GroupName",    "SysCAD");
  m_dwUpdateRate    = PF.RdLong(m_sServerName,  "UpdateRate",   250);
  m_dwTrickleCount  = PF.RdLong(m_sServerName,  "TrickleCount", 0);
  m_bSyncIO         = PF.RdLong(m_sServerName,  "SyncIO",       0)!=0;
  m_dDeadBandPercent    = PF.RdDouble (m_sServerName,  "DeadBandPercent",      0.1)*0.01;
  m_lDeadBandForceCount = PF.RdLong   (m_sServerName,  "DeadBandForceCount", -1);
  m_dwTrickleIndex  = 0;

  if (m_sGroupName.GetLength()==0)
    {
    m_sGroupName="Scd_";
    m_sGroupName+=m_sServerName;
    }

  return true;
  }

// -----------------------------------------------------------------------

bool CDevice::WriteConfig(LPCSTR DevCfgFile)
  {
  CProfINIFile PF(DevCfgFile);

  PF.WrStr(m_sServerName, "ProgID",      m_sProgID);
  PF.WrStr(m_sServerName, "Node",        m_sNode);
  PF.WrStr(m_sServerName, "GroupName",   m_sGroupName);
  PF.WrLong(m_sServerName, "UpdateRate", m_dwUpdateRate);
  PF.WrLong(m_sServerName, "TrickleCount", m_dwTrickleCount);
  PF.WrLong(m_sServerName, "SyncIO",       m_bSyncIO?1:0);
  PF.WrDouble (m_sServerName, "DeadBandPercent",  m_dDeadBandPercent*100.0);
  PF.WrLong   (m_sServerName, "DeadBandForceCount", m_lDeadBandForceCount);

  if (m_sGroupName.GetLength()==0)
    {
    m_sGroupName="Scd_";
    m_sGroupName+=m_sServerName;
    }
  return true;
  }

// -----------------------------------------------------------------------

bool CDevice::Connect()
  {
  HRESULT hr=m_OPC.Connect (m_sProgID, m_sGroupName, m_sNode, m_Clsid, m_dwUpdateRate);
  return (m_bConnected=SUCCEEDED(hr));
  };

// -----------------------------------------------------------------------

bool CDevice::DisConnect()
  {
  if (!m_bConnected)
    return true;
  HRESULT hr=m_OPC.Disconnect();
  return SUCCEEDED(hr);
  };

// -----------------------------------------------------------------------

bool CDevice::AddSlots(long No, long *Indices)
  {
  if (!m_bConnected)
    return false;

  USES_CONVERSION;

  HRESULT hr=S_OK;

  // get an interface pointer
  OPCItemMgt itemMgt;
  hr = itemMgt.Attach(m_OPC.m_OpcGroup );
  if( FAILED(hr) )
    {
    ReportError(m_sServerName, hr,  _T("AddItemsAttach: "));
    return false;
    }

  LPOLESTR NullStr=T2OLE("");
  // fill out its definition
  ASSERT(No<=MaxWorkSlots);
  OPCITEMDEF Defs[MaxWorkSlots]; //Make a part of device for reconnecting MRW Testing
  m_No = No;

  for (int i=0; i<No; i++)
    {
    m_Indices[i] = Indices[i];
    CSlot &S=*(Slots[Indices[i]]);
    Defs[i].szItemID = T2OLE((LPCSTR)S.m_sOPCTag.GetBuffer(0));
    Defs[i].dwBlobSize = 0;
    Defs[i].pBlob = NULL;
    Defs[i].bActive = TRUE;
    Defs[i].hClient = (OPCHANDLE)Indices[i];  // pointer to item is its "handle"
    Defs[i].szAccessPath = NullStr;
    Defs[i].vtRequestedDataType = S.Type();
    //dbgpln("  AddItem %4i %4i %s", i, Indices[i], S.m_sTag);
    }

  // add the item
  OPCITEMRESULT * pResults;
  HRESULT *pErrors;

  hr = itemMgt.AddItems(No, Defs, &pResults, &pErrors);
  if( FAILED( hr ) )   // if the call failed, get out
    {
    ReportError(m_sServerName, hr,  _T("AddItems: "));
    //delete item;
    return false;
    }

  for (i=0; i<No; i++)
    {
    CSlot &S=*(Slots[Indices[i]]);
    S.m_hServer = pResults[i].hServer;  // save the server handle
    S.m_wNativeType = pResults[i].vtCanonicalDataType;
    //.m_hrResult = pErrors[i];          // and the item's result

    //dbgpln("  Item = %4i %4i %08x %s", Indices[i], S.m_lSlot, S.m_hServer, S.m_sTag);

    if (FAILED(pErrors[i]))
      {
      S.SetError(SErr_AddItem, pErrors[i], "");
      //ReportError(m_sServerName, _T("AddItems: "), itemResult );
      }
    else
      S.m_bConnected=true;
    if( pResults[i].pBlob != NULL )
      CoTaskMemFree( pResults[i].pBlob );
    }

  // If the call was successful, memory must be cleaned up
  CoTaskMemFree( pResults );
  CoTaskMemFree( pErrors );

  //this is only needed if not set ACTIVE above
  // Enable
  if (1)
    {
    OPCHANDLE SrvHandles[MaxWorkSlots];
    for (i=0; i<No; i++)
      SrvHandles[i]=Slots[Indices[i]]->m_hServer;
    hr = itemMgt.SetActiveState( No, SrvHandles, TRUE, &pErrors);

    if( SUCCEEDED(hr) )
      CoTaskMemFree( pErrors );
    else
      ReportError(m_sServerName, hr,  _T("SetActiveState: "));
    }


  if (0)
    { // Refresh Slots

    OPCAsyncIO2 ASyncIO;
    HRESULT hr = ASyncIO.Attach(m_OPC.m_OpcGroup );
    if( FAILED(hr) )
      {
      ReportError(m_sServerName, hr, _T("ASyncIO2Attach: "));
      return false;
      }

    DWORD CanID;
    hr=ASyncIO.Refresh2(OPC_DS_DEVICE/*CACHE*/, gs_SlotMngr.GetTransactionID(), &CanID);
    if( FAILED( hr ) )   // if the call failed, get out
      {
      ReportError(m_sServerName, hr,  _T("ASyncIO.Refresh2: "));
      //delete item;
      return false;
      }

    CoTaskMemFree( pErrors );
    }

  return SUCCEEDED(hr);
  };

// -----------------------------------------------------------------------

bool CDevice::ServerGood(void)

  {
  bool lServerRunning = false;
  OPCSERVERSTATUS *ServerStatus;
  HRESULT hrr;

  if (!m_bConnected) return(false);

  hrr = m_OPC.m_OpcServer.GetStatus(&ServerStatus);
  if( FAILED(hrr) )
    {
    ReportError(m_sServerName, hrr,  _T("DoGetStatus: "));
    m_bConnected = false;
    return false;
    }
  else
    {

      // Successful Retrieval Status
      // We are meant to free szVendorInfo and ServerStatus
      // using the IMalloc interface - use CoTaskMemFree to do this

      if ( ServerStatus->dwServerState == OPC_STATUS_RUNNING )
        {
          lServerRunning = true;
        }
 
      // TO DO - Free status memory
      CoTaskMemFree(ServerStatus->szVendorInfo);
      CoTaskMemFree(ServerStatus);

    }
    
    if (!lServerRunning)
      return(false);


    return(true);
  }

bool CDevice::AddSlotsReconnect(void)
  {

  USES_CONVERSION;

  HRESULT hr=S_OK;

  if (!m_bConnected)
    return false;

  // get an interface pointer
  OPCItemMgt itemMgt;
  hr = itemMgt.Attach(m_OPC.m_OpcGroup );
  if( FAILED(hr) )
    {
    ReportError(m_sServerName, hr,  _T("AddItemsAttach: "));
    return false;
    }

  LPOLESTR NullStr=T2OLE("");
  // fill out its definition
  ASSERT(m_No<=MaxWorkSlots);

  OPCITEMDEF Defs[MaxWorkSlots];

  for (int i=0; i<m_No; i++)
    {
    CSlot &S=*(Slots[m_Indices[i]]);
    Defs[i].szItemID = T2OLE((LPCSTR)S.m_sOPCTag.GetBuffer(0));
    Defs[i].dwBlobSize = 0;
    Defs[i].pBlob = NULL;
    Defs[i].bActive = TRUE;
    Defs[i].hClient = (OPCHANDLE)m_Indices[i];  // pointer to item is its "handle"
    Defs[i].szAccessPath = NullStr;
    Defs[i].vtRequestedDataType = S.Type();
    //dbgpln("  AddItem %4i %4i %s", i, m_Indices[i], S.m_sTag);
    }

  // add the item
  OPCITEMRESULT * pResults;
  HRESULT *pErrors;

  hr = itemMgt.AddItems(m_No, Defs, &pResults, &pErrors);
  if( FAILED( hr ) )   // if the call failed, get out
    {
    ReportError(m_sServerName, hr,  _T("AddItems: "));
    //delete item;
    return false;
    }

  for (int i=0; i<m_No; i++)
    {
    CSlot &S=*(Slots[m_Indices[i]]);
    S.m_hServer = pResults[i].hServer;  // save the server handle
    S.m_wNativeType = pResults[i].vtCanonicalDataType;
    //.m_hrResult = pErrors[i];          // and the item's result

    dbgpln("  Item = %4i %4i %08x %s", m_Indices[i], S.m_lSlot, S.m_hServer, S.m_sTag);

    if (FAILED(pErrors[i]))
      {
      S.SetError(SErr_AddItem, pErrors[i], "");
      //ReportError(m_sServerName, _T("AddItems: "), itemResult );
      }
    else
      S.m_bConnected=true;
    if( pResults[i].pBlob != NULL )
      CoTaskMemFree( pResults[i].pBlob );
    }

  // If the call was successful, memory must be cleaned up
  CoTaskMemFree( pResults );
  CoTaskMemFree( pErrors );

  //this is only needed if not set ACTIVE above
  // Enable
  if (1)
    {
    OPCHANDLE SrvHandles[MaxWorkSlots];
    for (i=0; i<m_No; i++)
      SrvHandles[i]=Slots[m_Indices[i]]->m_hServer;
    hr = itemMgt.SetActiveState( m_No, SrvHandles, TRUE, &pErrors);

    if( SUCCEEDED(hr) )
      CoTaskMemFree( pErrors );
    else
      ReportError(m_sServerName, hr,  _T("SetActiveState: "));
    }


  if (0)
    { // Refresh Slots

    OPCAsyncIO2 ASyncIO;
    HRESULT hr = ASyncIO.Attach(m_OPC.m_OpcGroup );
    if( FAILED(hr) )
      {
      ReportError(m_sServerName, hr, _T("ASyncIO2Attach: "));
      return false;
      }

    DWORD CanID;
    hr=ASyncIO.Refresh2(OPC_DS_DEVICE/*CACHE*/, gs_SlotMngr.GetTransactionID(), &CanID);
    if( FAILED( hr ) )   // if the call failed, get out
      {
      ReportError(m_sServerName, hr,  _T("ASyncIO.Refresh2: "));
      //delete item;
      return false;
      }

    CoTaskMemFree( pErrors );
    }

  return SUCCEEDED(hr);
  };

// -----------------------------------------------------------------------

bool CDevice::RemoveSlots(long No, long *Indices)
  {
  if (!m_bConnected)
    return false;

  USES_CONVERSION;

  HRESULT hr=S_OK;

  // get an interface pointer
  OPCItemMgt itemMgt;
  hr = itemMgt.Attach(m_OPC.m_OpcGroup );
  if( FAILED(hr) )
    {
    ReportError(m_sServerName, hr,  _T("RemoveItemsAttach: "));
    return false;
    }

  LPOLESTR NullStr=T2OLE("");
  // fill out its definition
  OPCHANDLE Handles[MaxWorkSlots];
  for (int i=0; i<No; i++)
    {
    Handles[i]=Slots[Indices[i]]->m_hServer;
    //dbgpln("  RemoveItem %4i %8x ",Indices[i],Handles[i]);
    }

  HRESULT *pErrors;
  hr = itemMgt.RemoveItems(No, Handles, &pErrors);
  if( FAILED( hr ) )   // if the call failed, get out
    {
    ReportError(m_sServerName, hr, _T("RemoveItems: "));
    return false;
    }

  for (i=0; i<No; i++)
    {
    CSlot &S=*(Slots[Indices[i]]);
    if (FAILED(pErrors[i]))
      S.SetError(SErr_RemoveItem, pErrors[i], "RemoveItem ???");

    S.m_bConnected=false;
    }

  // If the call was successful, memory must be cleaned up
  CoTaskMemFree( pErrors );

  //this is only needed if not set ACTIVE above
  //// Enable
  //hr = itemMgt.SetActiveState( 1, &item->hServerHandle, TRUE, &pErrors);
  //if( SUCCEEDED(hr) )
  //  CoTaskMemFree( pErrors );
  //else
  //  {
  //  ReportError(m_sServerName, _T("SetActiveState: "), hr );
  //  }

  return SUCCEEDED(hr);
  };

// -----------------------------------------------------------------------

bool CDevice::SetClientHandles(long No, OPCHANDLE *hServer, OPCHANDLE * hClient)
  {
  if (!m_bConnected)
    return false;

  USES_CONVERSION;

  HRESULT hr=S_OK;

  // get an interface pointer
  OPCItemMgt itemMgt;
  hr = itemMgt.Attach(m_OPC.m_OpcGroup );
  if( FAILED(hr) )
    {
    ReportError(m_sServerName, hr,  _T("AddItemsAttach: "));
    return false;
    }

  HRESULT *pErrors;
  hr = itemMgt.SetClientHandles(No, hServer, hClient, &pErrors);
  if( FAILED( hr ) )   // if the call failed, get out
    {
    ReportError(m_sServerName, hr,  _T("SetClientHandles: "));
    return false;
    }

  for (int i=0; i<No; i++)
    {
    CSlot &S=*(Slots[hClient[i]]);
    if (FAILED(pErrors[i]))
      S.SetError(SErr_SetClientHandles, pErrors[i], "SetClientHandles");
    }

  // If the call was successful, memory must be cleaned up
  CoTaskMemFree( pErrors );

  //this is only needed if not set ACTIVE above
  //// Enable
  //hr = itemMgt.SetActiveState( 1, &item->hServerHandle, TRUE, &pErrors);
  //if( SUCCEEDED(hr) )
  //  CoTaskMemFree( pErrors );
  //else
  //  {
  //  ReportError(m_sServerName, _T("SetActiveState: "), hr );
  //  }

  return SUCCEEDED(hr);
  };

// -----------------------------------------------------------------------

bool CDevice::SetActiveState(bool On)
  {
  if (!m_bConnected)
    return false;

  //dbgpln("  SetActiveState = %s",On?"TRUE":"False");
  BOOL Act=On;
  DWORD ARate;
  HRESULT hr=m_OPC.m_OpcGroup.SetState(NULL, &ARate, &Act, NULL, NULL, NULL, NULL);
  return SUCCEEDED(hr);
  }

// -----------------------------------------------------------------------

void CDevice::AppendWriteRqst(CSlot & Slot, OPCHANDLE hServer, VARIANT & vValue, bool Refresh)
  {
  if (m_bConnected)
    {
    bool WriteIt=true;
    if (!Refresh && Slot.m_Range.m_bValid && (Slot.Type() == VT_R4 || Slot.Type() == VT_R8))
      {
      double WrtVal=0;
      switch (Slot.Type())
        {
        case VT_R4: WrtVal= vValue.fltVal; break;
        case VT_R8: WrtVal= vValue.dblVal; break;
        }

      double D=fabs(Slot.m_LastValueWritten - WrtVal)/Slot.m_Range.Range();
      WriteIt=(D>=m_dDeadBandPercent) || (Slot.m_nValuesToSkip==0);
      if (WriteIt)
        {
        Slot.m_LastValueWritten = WrtVal;
        Slot.m_nValuesToSkip    = m_lDeadBandForceCount;
        }
      else if (Slot.m_nValuesToSkip>=0)
        Slot.m_nValuesToSkip--;
      
      if (!WriteIt)
        gs_SlotMngr.m_Stats.m_nFltWritesSkip++;
      }

    if (WriteIt)
      {
    CFullValue FV;
    FV.m_vValue=vValue;
      CChangeItem * pWrt=new CChangeItem(eCSD_Slot, Slot.m_lSlot, eCSD_Device, -1, hServer, gs_SlotMngr.GetTransactionID(), FV, false, Refresh);
    m_WriteList.AddTail(pWrt);
    }
    }
  };

// -----------------------------------------------------------------------

long CDevice::FlushWriteList()
  {
#ifdef NEVER
  bool lServerRunning = false;

/*** Mark West Testing ****/

if (!m_bConnected)
  {

    // Try and reconnect

    Connect();
    if (m_bConnected)
      AddSlotsReconnect();

    return(false);
  }

OPCSERVERSTATUS *ServerStatus;
HRESULT hrr;
hrr = m_OPC.m_OpcServer.GetStatus(&ServerStatus);
  if( FAILED(hrr) )
    {
    ReportError(m_sServerName, hrr,  _T("DoGetStatus: "));
    m_bConnected = false;
    return false;
    }
  else
    {

      // Successful Retrieval Status
      // We are meant to free szVendorInfo and ServerStatus
      // using the IMalloc interface - use CoTaskMemFree to do this

      if ( ServerStatus->dwServerState == OPC_STATUS_RUNNING )
        {
          lServerRunning = true;
        }
 
      // TO DO - Free status memory
      CoTaskMemFree(ServerStatus->szVendorInfo);
      CoTaskMemFree(ServerStatus);

    }
    
    if (!lServerRunning)
      return(false);

/*** Mark West Testing ****/
#endif

  if (!m_bConnected)
    return false;

  long Total=0;

  HRESULT hr=0;
  OPCSyncIO   SyncIO;
  OPCAsyncIO2 ASyncIO;
  if (m_bSyncIO)
    {
    hr = SyncIO.Attach(m_OPC.m_OpcGroup );
    if( FAILED(hr) )
      {
      ReportError(m_sServerName, hr,  _T("SyncIO::Attach: "));
      return false;
      }
    }
  else
    {
    hr = ASyncIO.Attach(m_OPC.m_OpcGroup );
  if( FAILED(hr) )
    {
      ReportError(m_sServerName, hr,  _T("ASyncIO2::Attach: "));
    return false;
    }
    }

  long      WrkSlots[MaxWorkSlots];
  OPCHANDLE WrkHandles[MaxWorkSlots];
  VARIANT   WrkValues[MaxWorkSlots];
  CChangeItem * Wrts[MaxWorkSlots];
  for (int i=0; i<MaxWorkSlots; i++)
    VariantInit(&WrkValues[i]);
  long No=0;
  long NoInts=0;
  long NoFlts=0;
  //CWriteRqst * pWrt=m_WriteList.RemoveHead();
  CChangeItem * pWrt=m_WriteList.RemoveHead();
  while (pWrt)
    {
    Total++;
    WrkSlots[No]=pWrt->m_lSrcInx;
    WrkHandles[No]=pWrt->m_hServer;
    WrkValues[No]=pWrt->m_vValue;
    Wrts[No]=pWrt;
    No++;
    if (pWrt->m_vValue.vt==VT_R4 || pWrt->m_vValue.vt==VT_R8)
      NoFlts++;
    else
      NoInts++;

    gs_SlotMngr.m_HistoryList.AddTail(pWrt);
    //delete pWrt;
    pWrt=m_WriteList.RemoveHead();

    if ((No>=MaxWorkSlots) || (No>0 && (pWrt==NULL)))
      {
      gs_SlotMngr.m_Stats.m_nIntWritesBusy+=NoInts;
      gs_SlotMngr.m_Stats.m_nFltWritesBusy+=NoFlts;
      gs_SlotMngr.SendUpdateStatus();

      HRESULT *pErrors=NULL, hr;
      if (m_bSyncIO)
        {
        hr=SyncIO.Write(No, WrkHandles, WrkValues, &pErrors);
        }
      else
        {
      DWORD CanID=0;
        hr=ASyncIO.Write(No, WrkHandles, WrkValues, gs_SlotMngr.GetTransactionID(), &CanID, &pErrors);
        }
      gs_SlotMngr.m_Stats.m_nDeviceChgsOutInt+=NoInts;
      gs_SlotMngr.m_Stats.m_nDeviceChgsOutFlt+=NoFlts;
      gs_SlotMngr.m_Stats.m_nIntWritesBusy-=NoInts;
      gs_SlotMngr.m_Stats.m_nFltWritesBusy-=NoFlts;
      gs_SlotMngr.SendUpdateStatus();
      //
      // Think we need to check all the return error codes here
      //

      LPCTSTR Msg1=m_bSyncIO  ? _T("SyncIO.Write: ") : _T("ASyncIO.Write: ");
      LPCTSTR Msg2=m_bSyncIO  ? _T("SyncIO.Write: Item") : _T("ASyncIO.Write: Item");
      int     SErr=m_bSyncIO  ? SErr_SyncIOWrite : SErr_ASyncIOWrite;
      if( FAILED( hr ) )   // if the call failed, get out
        {
        ReportError(m_sServerName, hr,  Msg1);
        //delete item;
        return false;
        }

      for (i=0; i<No; i++)
        {
        CSlot &S=*(Slots[WrkSlots[i]]);
        if (FAILED(pErrors[i]))
          S.SetError(SErr , pErrors[i], Msg2);
        }

      CoTaskMemFree( pErrors );

      for (int i=0; i<No; i++)
        VariantClear(&WrkValues[i]);
      No=0;
      NoInts=0;
      NoFlts=0;
      }
    }

  //theApp.PostThreadMessage(WMU_UPDATETAGVALUES, 0, 0);

  return Total;
  }

// -----------------------------------------------------------------------

void CDevice::Save(CProfINIFile & SavePF)
  {
  };

// -----------------------------------------------------------------------

void CDevice::Load(CProfINIFile & SavePF)
  {
  };

// =======================================================================
//
//
//
// =======================================================================

long        CDevice::getSlotCount() const               { return gs_SlotMngr.m_Slots.GetSize(); };
CSlot     * CDevice::getSlot(long Index) const          { return gs_SlotMngr.m_Slots[Index]; };
void        CDevice::putSlot(long Index, CSlot * pSlot) { gs_SlotMngr.m_Slots[Index] = pSlot; };
long        CDevice::getDeviceCount() const                   { return gs_SlotMngr.m_Devices.GetSize(); };
CDevice   * CDevice::getDevice(long Index) const              { return gs_SlotMngr.m_Devices[Index]; };
void        CDevice::putDevice(long Index, CDevice * pDevice) { gs_SlotMngr.m_Devices[Index] = pDevice; };

// =======================================================================
//
//
//
// =======================================================================
