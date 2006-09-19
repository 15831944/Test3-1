// SlotManager.cpp: implementation of the CSlotMngr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SysCADMarshal.h"
#include "slotmngr.h"
#include "dbgetc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int dbgflushCQ = 0;
const int dbgflushHQ = 0;
const int dbgTimers  = 0;
const int dbgChanges = 0;

// =======================================================================
//
//
//
// =======================================================================

CCfgBlock::CCfgBlock()
  {
  }

// -----------------------------------------------------------------------

CCfgBlock::~CCfgBlock()
  {
  }

// -----------------------------------------------------------------------

void CCfgBlock::SetDefault()
  {
  //dbgpln("DefaultConfig");

  m_nPriorityThread = THREAD_PRIORITY_ABOVE_NORMAL;
  m_nPriorityClass  = ABOVE_NORMAL_PRIORITY_CLASS;
  m_dFloatDeltaChange=1.0/4096.0; // one count in 4096
  m_dFloatDeltaDT=60000;
  m_dFloatDeltaMult=0.01;
  m_dwReConnHold=60;
  m_dwReConnItemBlk=100;
  m_dwDelayResolution   = 50;
  m_dwChangeFlushCycle  = 1000; 
  m_dwStatusUpdateCycle  = 500; 
  m_dwMaxChangesInCall   = 10;
  m_dwConditioningCycle  = 1000;

  m_ClsCtx = CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER;

  }

// -----------------------------------------------------------------------

void CCfgBlock::Read(LPCSTR DevCfgFile)
  {
  //dbgpln("ReadConfig:%s",DevCfgFile);
  CProfINIFile PF(DevCfgFile);
  CString s;

  s = PF.RdStr("Thread", "Priority", "above_normal");
  m_dwDelayResolution   = PF.RdLong("Thread", "DelayResolution", m_dwDelayResolution);
  m_dwChangeFlushCycle  = PF.RdLong("Thread", "ChangeFlushCycle", m_dwChangeFlushCycle);
  m_dwStatusUpdateCycle = PF.RdLong("Thread", "StatusUpdateCycle", m_dwStatusUpdateCycle);
  m_dwMaxChangesInCall  = PF.RdLong("Thread", "MaxChangesInCall", m_dwMaxChangesInCall);
  m_dwConditioningCycle  = PF.RdLong("Thread", "ConditioningCycle", m_dwConditioningCycle);

  if (s.CompareNoCase("normal")==0)
    {
    m_nPriorityThread = THREAD_PRIORITY_NORMAL;
    m_nPriorityClass  = NORMAL_PRIORITY_CLASS;
    }
  else if (s.CompareNoCase("above_normal")==0)
    {
    m_nPriorityThread = THREAD_PRIORITY_ABOVE_NORMAL;
    m_nPriorityClass  = ABOVE_NORMAL_PRIORITY_CLASS;
    }
  else if (s.CompareNoCase("highest")==0)
    {
    m_nPriorityThread = THREAD_PRIORITY_HIGHEST;
    m_nPriorityClass  = HIGH_PRIORITY_CLASS;
    }
  else if (s.CompareNoCase("time_critical")==0)
    {
    m_nPriorityThread = THREAD_PRIORITY_TIME_CRITICAL;
    m_nPriorityClass  = REALTIME_PRIORITY_CLASS;
    }
  else if (s.CompareNoCase("below_normal")==0)
    {
    m_nPriorityThread = THREAD_PRIORITY_BELOW_NORMAL;
    m_nPriorityClass  = BELOW_NORMAL_PRIORITY_CLASS;
    }

  m_dFloatDeltaChange = PF.RdDouble("FloatPointFilter", "Change", m_dFloatDeltaChange);
  m_dFloatDeltaDT     = PF.RdDouble("FloatPointFilter", "DeltaTime", m_dFloatDeltaDT/1000)*1000;
  m_dFloatDeltaMult   = PF.RdDouble("FloatPointFilter", "Multiplier", m_dFloatDeltaMult);

  m_dwReConnHold=PF.RdLong("Thread", "ReconnectHold", m_dwReConnHold);
  m_dwReConnItemBlk=PF.RdLong("Thread", "ReconnectItemBlk", m_dwReConnItemBlk);


  s=PF.RdStr("DCOM", "ClassContext", "");
  if (s.GetLength()>0)
    {
    s.MakeUpper();
    if (s.Find("INPROC_SERVER")>=0)
      m_ClsCtx|=CLSCTX_INPROC_SERVER;
    if (s.Find("INPROC_HANDLER")>=0)
      m_ClsCtx|=CLSCTX_INPROC_HANDLER;
    if (s.Find("LOCAL_SERVER")>=0)
      m_ClsCtx|=CLSCTX_LOCAL_SERVER;
    if (s.Find("REMOTE_SERVER")>=0)
      m_ClsCtx|=CLSCTX_REMOTE_SERVER;
    }
  if (m_ClsCtx==0)
    m_ClsCtx = CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER;


  //  s=PF.RdStr("Configuration", "InitialValueSource", "Cache");//"Device");
  //  BOOL InitValSrcCache=(s.XStrICmp("Cache")==0);
  //  long IVTO=PF.RdLong("Configuration", "InitialValueTimeout", 5000);
  //  s=PF.RdStr("Configuration", "ThreadingModel", ThreadingModelStr(bThreadingModel));
  //  bThreadingModel = ThreadingStrModel(s());

  }

// -----------------------------------------------------------------------

void CCfgBlock::Write(LPCSTR DevCfgFile)
  {
  //dbgpln("WriteConfig:%s",DevCfgFile);

  CProfINIFile PF(DevCfgFile);
  CString s;

  switch (m_nPriorityThread)
    {
    case THREAD_PRIORITY_NORMAL       : PF.WrStr("Thread", "Priority", "Normal");        break;
    case THREAD_PRIORITY_ABOVE_NORMAL : PF.WrStr("Thread", "Priority", "Above_Normal");  break;
    case THREAD_PRIORITY_HIGHEST      : PF.WrStr("Thread", "Priority", "Highest");       break;
    case THREAD_PRIORITY_TIME_CRITICAL: PF.WrStr("Thread", "Priority", "Time_Critical"); break;
    }
  PF.WrInt   ("Thread", "DelayResolution", m_dwDelayResolution);
  PF.WrLong  ("Thread", "ReconnectHold", m_dwReConnHold);
  PF.WrLong  ("Thread", "ReconnectItemBlk", m_dwReConnItemBlk);

  PF.WrDouble("FloatPointFilter", "Change", m_dFloatDeltaChange);
  PF.WrDouble("FloatPointFilter", "DeltaTime", m_dFloatDeltaDT/1000);
  PF.WrDouble("FloatPointFilter", "Multiplier", m_dFloatDeltaMult);

  s="";
  if (m_ClsCtx&CLSCTX_LOCAL_SERVER)
    s += "LOCAL_SERVER | ";
  if (m_ClsCtx&CLSCTX_REMOTE_SERVER)
    s += "REMOTE_SERVER | ";
  if (m_ClsCtx&CLSCTX_INPROC_SERVER)
    s += "INPROC_SERVER | ";
  if (m_ClsCtx&CLSCTX_INPROC_HANDLER)
    s += "INPROC_HANDLER | ";
  PF.WrStr   ("DCOM", "ClassContext", s);

  //PF.WrStr   ("Configuration", "InitialValueSource", InitValSrcCache ? "Cache" : "Device");
  //PF.WrLong  ("Configuration", "InitialValueTimeout", IVTO);
  //  PF.WrStr   ("Configuration", "ThreadingModel", ThreadingModelStr(bThreadingModel));

  }

// -----------------------------------------------------------------------

void CCfgBlock::Dump()
  {
  CString s;

  //  dbgpln("      Databases.Slots             %s", m_SlotCfgDB);
  //  dbgpln("      Databases.CodeBlocks        %s", m_CdBlkDB);

  dbgpln("      Thread.DelayResolution      %10i", m_dwDelayResolution);
  dbgpln("      Thread.ReconnectHold        %10i", m_dwReConnHold);
  dbgpln("      Thread.ReconnectItemBlk     %10i", m_dwReConnItemBlk);
  switch (m_nPriorityThread)
    {
    case THREAD_PRIORITY_NORMAL       : dbgpln("      Thread.Priority             Normal");        break;
    case THREAD_PRIORITY_ABOVE_NORMAL : dbgpln("      Thread.Priority             Above_Normal");  break;
    case THREAD_PRIORITY_HIGHEST      : dbgpln("      Thread.Priority             Highest");       break;
    case THREAD_PRIORITY_TIME_CRITICAL: dbgpln("      Thread.Priority             Time_Critical"); break;
    }

  //dbgpln("      FloatPointFilter.Change     %10.2f", m_dFloatDeltaChange_Rd);
  dbgpln("      FloatPointFilter.Percentage %10.2f", m_dFloatDeltaChange);
  dbgpln("      FloatPointFilter.DeltaTime  %10.2f", m_dFloatDeltaDT);
  dbgpln("      FloatPointFilter.Multiplier %10.2f", m_dFloatDeltaMult);

  s="";
  if (m_ClsCtx&CLSCTX_LOCAL_SERVER)
    s += "LOCAL_SERVER | ";
  if (m_ClsCtx&CLSCTX_REMOTE_SERVER)
    s += "REMOTE_SERVER | ";
  if (m_ClsCtx&CLSCTX_INPROC_SERVER)
    s += "INPROC_SERVER | ";
  if (m_ClsCtx&CLSCTX_INPROC_HANDLER)
    s += "INPROC_HANDLER | ";
  dbgpln("      Configuration.ClassContext  %s", s);
  //PF.WrStr   ("Configuration", "InitialValueSource", InitValSrcCache ? "Cache" : "Device");
  //PF.WrLong  ("Configuration", "InitialValueTimeout", IVTO);
  //  PF.WrStr   ("Configuration", "ThreadingModel", ThreadingModelStr(bThreadingModel));

  }

// =======================================================================
//
//
//
// =======================================================================

CSlotMngr::CSlotMngr() 
//: m_Lock(&m_MsgLoopLock)
  {
  m_bRunning          =  false;
  m_bRunningB4Cfg     =  false;
  m_bDoingFullCfg     =  false;
  m_bRunOnMainThread  =  false;
  m_dwThreadId        =  0;
  m_pExecThread       =  NULL;
  m_SlotMap.InitHashTable(4999);
  m_CdBlkMap.InitHashTable(4999);
  m_LinkMap.InitHashTable(4999);

  m_nSlots2Write            = 0;
  m_nLinks2Write            = 0;

  m_lCfgSequence            =  -1;

  m_dwTransactionID         = 1;
  m_dwLastChangeFlushTicks  = GetTickCount();
  m_dwStatusSendTicks       = GetTickCount();
  m_dwFlushChangeCalls      = 0;
  m_dwLastCBProcessTime     = GetTickCount();

  m_bMonitorOn              = true;
  m_dwTotalChanges          = 0;
  m_dwTotalHolds            = 0;

  m_bScdRunning             = false;

  SetDefaultConfig();
  }

// -----------------------------------------------------------------------

CSlotMngr::~CSlotMngr()
  {
  FlushChangeQueue();
  FlushHistoryQueue();
  }

// -----------------------------------------------------------------------

long CSlotMngr::FindDevice(LPCSTR Tag, bool AllowCreate)
  {
  for (int d=0; d<NDevices; d++)
    if (m_Devices[d]->m_sServerName.CompareNoCase(Tag)==0)
      return d;
  if (AllowCreate)
    {
    d=m_Devices.GetSize();
    m_Devices.SetSize(d+1);
    CDevice *p=new CDevice(Tag);
    m_Devices[d]=p;
    return d;
    }
  return -1;
  };

// -----------------------------------------------------------------------

long CSlotMngr::FindSlot(LPCSTR Tag)
  {
  long I;
  if (m_SlotMap.Lookup(Tag, I))
    return I;
  return -1;
  };

// -----------------------------------------------------------------------

long CSlotMngr::FindCdBlk(LPCSTR Tag)
  {
  long I;
  if (m_CdBlkMap.Lookup(Tag, I))
    return I;
  return -1;
  };

// -----------------------------------------------------------------------

long CSlotMngr::FindLink(LPCSTR Tag)
  {
  long I;
  if (m_LinkMap.Lookup(Tag, I))
    return I;
  return -1;
  };


// -----------------------------------------------------------------------

void CSlotMngr::CompressSlotList()
  {
  dbgpln("CompressSlotList");
  for (int i=0; i<NSlots; i++)
    {
    if (m_Slots[i]->m_bInUse)
      {
      m_Slots[i]->m_lSlot=i;
      }
    else
      {
      delete m_Slots[i];
      m_Slots[i]=NULL;
      for (int j=i+1; j<NSlots; j++)
        m_Slots[j-1]=m_Slots[j];
      m_Slots.SetSize(m_Slots.GetSize()-1);
      i--;
      }
    }

  m_SlotMap.RemoveAll();
  for (i=0; i<NSlots; i++)
    m_SlotMap.SetAt(m_Slots[i]->m_sTag, i);
  }

// -----------------------------------------------------------------------

void CSlotMngr::ConnectUsedSlots()
  {
  dbgpln("ConnectUsedSlots");

  // Fix Device Indices
  for (int i=0; i<NSlots; i++)
    {
    CSlot &S=*m_Slots[i];
    S.m_lDevice = (S.m_bInUse /*&& !S.m_State.m_nError*/) ? FindDevice(S.m_sOPCClient) : -1;
    }

  for (int d=0; d<NDevices; d++)
    {
    CDevice &D=*m_Devices[d];
    if (D.Connected())
      {
      long ConnIndices[MaxWorkSlots];
      long NS=NSlots;
      long NI=0;
      for (long i=0 ;i<NS; i++)
        {
        CSlot &S=*m_Slots[i];
        if (S.m_lDevice==d && !S.m_bConnected)
          ConnIndices[NI++]=i;
        if (NI>=MaxWorkSlots || (NI>0 && (i+1)==NS))
          {
          D.AddSlots(NI, ConnIndices);
          NI=0;
          }
        // Check Spans etc
        if (S.m_Span.m_bValid)
          {
          if (IsSignedDataVT(S.m_wNativeType) || IsUnsignedDataVT(S.m_wNativeType))
            S.m_Span.m_dSpan = Max(1.0, S.m_Span.m_dSpan);
          else
            S.m_Span.m_dSpan = Max(1.0e-20, S.m_Span.m_dSpan);
          }
        if (S.m_Range.m_bValid)
          {
          if (IsSignedDataVT(S.m_wNativeType) || IsUnsignedDataVT(S.m_wNativeType))
            S.m_Range.m_dMax = Max(S.m_Range.m_dMin+1, S.m_Range.m_dMax);
          else
            S.m_Range.m_dMax = Max(S.m_Range.m_dMin+1.0e-20, S.m_Range.m_dMax);
          }
        }
      }
    }
  };

// -----------------------------------------------------------------------

void CSlotMngr::DisConnectUnusedSlots()
  {
  dbgpln("DisConnectUnusedSlots");

  for (int d=0; d<NDevices; d++)
    {
    CDevice &D=*m_Devices[d];
    if (D.Connected())
      {
      long ConnIndices[MaxWorkSlots];
      long NS=NSlots;
      long NI=0;
      for (long i=0 ;i<NS; i++)
        {
        CSlot &S=*m_Slots[i];
        //if (S.m_lDevice==d)
        //  dbgpln("  %4i %s %s", i, (S.m_lDevice==d) && !S.m_bInUse && S.m_bConnected ? "Remove":"Keep  ", S.m_sTag);
        if ((S.m_lDevice==d) && !S.m_bInUse && S.m_bConnected)
          ConnIndices[NI++]=i;
        if (NI>=MaxWorkSlots || (NI>0 && (i+1)==NS))
          {
          D.RemoveSlots(NI, ConnIndices);
          NI=0;
          }
        }
      }
    }
  };

// -----------------------------------------------------------------------

void CSlotMngr::FindDevicesForSlots()
  {
  dbgpln("FindDevicesForSlots");
  for (int d=0; d<NDevices; d++)
    m_Devices[d]->m_bInUse=false;
  for (int i=0; i<NSlots; i++)
    {
    CSlot &S=*m_Slots[i];
    if (S.m_bInUse && !S.m_State.m_nError)
      {
      long d=FindDevice(S.m_sOPCClient, true);
      if (d>=0)
        m_Devices[d]->m_bInUse=true;
      }
    }
  };

// -----------------------------------------------------------------------

void CSlotMngr::CompressDeviceList()
  {
  dbgpln("CompressDeviceList");
  for (int i=0; i<NDevices; i++)
    {
    if (m_Devices[i]->m_bInUse)
      {
      m_Devices[i]->m_lDeviceNo=i;
      //      m_Devices[i]->m_sServer.Format("SysCADMarshal%i", i);
      // Also Call SetClientName
      }
    else
      {
      delete m_Devices[i];
      m_Devices[i]=NULL;
      for (int j=i+1; j<NDevices; j++)
        m_Devices[j-1]=m_Devices[j];
      m_Devices.SetSize(m_Devices.GetSize()-1);
      i--;
      }
    }
  //for (i=0; i<NDevices; i++)
  //  dbgpln("  Device %4i %4i %s",i, m_Devices[i]->m_lDeviceNo, m_Devices[i]->m_sServerName);
  }

// -----------------------------------------------------------------------

void CSlotMngr::ConnectUsedDevices()
  {
  dbgpln("ConnectUsedDevices");

  for (int d=0; d<NDevices; d++)
    {
    CDevice &D=*m_Devices[d];
    if (D.m_bInUse && !D.Connected() && !D.m_bLocalOnly)
      {
      D.ReadConfig(m_Cfg.m_sCfgFile);
      D.Connect();
      }
    }
  };

// -----------------------------------------------------------------------

void CSlotMngr::DisConnectUnusedDevices()
  {
  dbgpln("DisConnectUnusedDevices");
  for (int d=0; d<NDevices; d++)
    {
    CDevice &D=*m_Devices[d];
    if (!D.m_bInUse && !D.m_bLocalOnly)
      D.DisConnect();
    }
  };

// -----------------------------------------------------------------------

void CSlotMngr::SetClientHandlesInServer()
  {
  dbgpln("SetClientHandlesInServer");
  for (int d=0; d<NDevices; d++)
    {
    CDevice &D=*m_Devices[d];
    OPCHANDLE hClient[MaxWorkSlots];
    OPCHANDLE hServer[MaxWorkSlots];
    long NS=NSlots;
    long NI=0;
    for (long i=0 ;i<NS; i++)
      {
      CSlot &S=*m_Slots[i];
      if (S.m_lDevice==d && S.m_bConnected)
        {
        hClient[NI]=i;
        hServer[NI]=S.m_hServer;
        //dbgpln("  SetClientHandle  %4i %08x", i, S.m_hServer);
        NI++;
        }
      if (NI>=MaxWorkSlots || (NI>0 && (i+1)==NS))
        {
        D.SetClientHandles(NI, hServer, hClient);
        NI=0;
        }
      }
    }
  };

// -----------------------------------------------------------------------

void CSlotMngr::RemoveConnects()
  {
  dbgpln("RemoveConnects");
  for (long i=0 ;i<m_Slots.GetSize(); i++)
    m_Slots[i]->RemoveConnects();
  for (i=0 ;i<m_Links.GetSize(); i++)
    m_Links[i]->RemoveConnects();
  for (i=0 ;i<m_CdBlks.GetSize(); i++)
    m_CdBlks[i]->RemoveConnects();
  };

// -----------------------------------------------------------------------

void CSlotMngr::InitialiseConnects()
  {
  dbgpln("InitialiseConnects");
  for (long i=0 ;i<m_Slots.GetSize(); i++)
    m_Slots[i]->InitialiseConnects();
  for (i=0 ;i<m_Links.GetSize(); i++)
    m_Links[i]->InitialiseConnects();
  for (i=0 ;i<m_CdBlks.GetSize(); i++)
    m_CdBlks[i]->InitialiseConnects();
  };

// -----------------------------------------------------------------------

void CSlotMngr::ConnectConnects()
  {
  dbgpln("ConnectConnects");
  for (long i=0 ;i<m_Slots.GetSize(); i++)
    m_Slots[i]->ConnectConnects();

  if (10)
    {
    for (long i=0 ;i<m_Slots.GetSize(); i++)
      {
      dbgpln("Slot %s", m_Slots[i]->m_sTag);
      for (long j=0; j<m_Slots[i]->m_SetConnects.GetSize(); j++)
        m_Slots[i]->m_SetConnects[j]->Dump("  Set:");
      };
    for (i=0 ;i<m_Links.GetSize(); i++)
      {
      dbgpln("Link %s", m_Links[i]->m_sTag);
      for (long j=0; j<m_Links[i]->m_ReflectedGets.GetSize(); j++)
        m_Links[i]->m_ReflectedGets[j]->Dump("  Set:");
      }
    for (i=0 ;i<m_CdBlks.GetSize(); i++)
      {
      //dbgpln("CdBlk %s", m_CdBlks[i]->m_sTag);
      for (long j=0; j<m_CdBlks[i]->m_ReflectedGets.GetSize(); j++)
        m_CdBlks[i]->m_ReflectedGets[j]->Dump("  Set:");
      }
    }
  };

// -----------------------------------------------------------------------

void CSlotMngr::CompressLinkList()
  {
  m_LinksWithCondBlks.SetSize(0, 64);
  dbgpln("CompressLinkList");
  CLongArray OldNos, NewNos;
  OldNos.SetSize(NLinks);
  NewNos.SetSize(NLinks);
  for (int i=0; i<NLinks; i++)
    {
    OldNos[i]=i;
    NewNos[i]=-1;
    }
  for (long i=0; i<NLinks; i++)
    {
    if (m_Links[i]->m_bInUse)
      {
      m_Links[i]->m_lLink=i;
      if (m_Links[i]->m_CondBlk.m_On)
        {
        //m_Links[i]->m_CondBlk.m_Active=true;
        m_LinksWithCondBlks.Add(i);
        }
      }
    else
      {
      delete m_Links[i];
      m_Links[i]=NULL;
      for (int j=i+1; j<NLinks; j++)
        {
        m_Links[j-1]=m_Links[j];
        OldNos[j-1]=OldNos[j];
        }
      m_Links.SetSize(m_Links.GetSize()-1);
      OldNos.SetSize(m_Links.GetSize());
      i--;
      }
    }

  m_LinkMap.RemoveAll();
  for (i=0; i<NLinks; i++)
    m_LinkMap.SetAt(m_Links[i]->m_sTag, i);

  // correct  all LinIndices in Slots&Connects
  for (int i=0; i<NLinks; i++)
    NewNos[OldNos[i]]=i;

  for (int s=0; s<NSlots; s++)
    {
    Slots[s]->CorrectConnects(NewNos);
    }
  }

// -----------------------------------------------------------------------

void CSlotMngr::MarkLinksAsLocal()
  {
  for (int i=0; i<NLinks; i++)
    {
    m_Links[i]->m_State.m_bSimTagValid=false;
    m_Links[i]->m_State.m_bSimTagIsParm=true;
    }
  };

// -----------------------------------------------------------------------

void CSlotMngr::Start()
  {
  dbgpln("Start");
  if (!m_bRunning)
    {
    }
  m_bRunning=true;
  }

// -----------------------------------------------------------------------

void CSlotMngr::Stop()
  {
  dbgpln("Stop");
  if (m_bRunning)
    {
    }
  m_bRunning=false;
  }

// -----------------------------------------------------------------------

void CSlotMngr::CfgBegin(eDoingCfg Doing, bool OnTheFly)
  {
  dbgpln("CfgBegin");
  m_bRunningB4Cfg=m_bRunning;
  m_bDoingFullCfg=true;
  Stop();
  m_dwTotalChanges=0;
  m_dwTotalHolds=0;

  //  m_pSimulator=m_pSimulatorReqd;

  //  m_Lock.Lock();
  //if (!IsReload)
  //  Load("LastState");

  FlushChangeQueue();
  FlushHistoryQueue();

  // Discard Writes and Touches
  for (int d=0; d<NDevices; d++)
    Devices[d]->ClearWriteList();

  DumpSlotInfo("CfgBegin 1");
  //if (Doing==eDoingCfgStop)
  //  Save("LastState");

  if (!OnTheFly)
    {
    for (int i=0; i<NSlots; i++)
      m_Slots[i]->m_bInUse=false;

    for (i=0; i<NDevices; i++)
      {
      m_Devices[i]->m_bInUse=false;
      m_Devices[i]->SetActiveState(false);
      }

    for (i=0; i<NLinks; i++)
      m_Links[i]->m_bInUse=false;
    }

  InitialiseConnects();

  m_lCfgSequence=Max(0L, m_lCfgSequence+1L);

  }

// -----------------------------------------------------------------------

void CSlotMngr::CfgEnd(eDoingCfg Doing)
  {
  dbgpln("CfgEnd");
  //if (ShuttingDown)
  //  {
  //  Save("LastState");
  //  }
  DumpSlotInfo("CfgEnd 1");
  DisConnectUnusedSlots();
  DumpSlotInfo("CfgEnd 2");

  CompressSlotList();
  SetClientHandlesInServer();
  FindDevicesForSlots();
  DisConnectUnusedDevices();
  CompressDeviceList();

  ConnectUsedDevices();
  ConnectUsedSlots();

  ConnectConnects();
  CompressLinkList();
  MarkLinksAsLocal();

  SendUpdateStatus();

  DumpSlotInfo("CfgEnd 3");

  m_bDoingFullCfg=false;

  for (int i=0; i<NDevices; i++)
    m_Devices[i]->SetActiveState(true);

  if (Doing!=eDoingCfgStop)
    {
    CSlotCfgArray *pSlotLst= new CSlotCfgArray;
    pSlotLst->SetSize(m_Slots.GetSize());
    for (i=0; i<m_Slots.GetSize(); i++)
      {
      CSlot &S=*m_Slots[i];
      S.m_lCfgSequence=m_lCfgSequence; // Mark as Valid
      if (S.m_State.m_bHold)
        m_dwTotalHolds++;
      (*pSlotLst)[i]=new CSlotCfg(*m_Slots[i]);
      dbgpln("Slot[%4i]:%-30s %s", i, S.m_sTag, S.m_sOPCTag);
      }
    theApp.PostThreadMessage(WMU_SETSLOTCFG, m_lCfgSequence, (LPARAM)pSlotLst);

    CLinkCfgArray *pLinkLst= new CLinkCfgArray;
    pLinkLst->SetSize(m_Links.GetSize());
    for (i=0; i<m_Links.GetSize(); i++)
      {
      CLink &L=*m_Links[i];
      L.m_lCfgSequence=m_lCfgSequence; // Mark as Valid
      if (L.m_State.m_bHold)
        m_dwTotalHolds++;
      (*pLinkLst)[i]=new CLinkCfg(*m_Links[i]);
      dbgpln("Link[%4i]:%-30s %s", i, L.m_sTag, L.m_sSimTag);
      }
    theApp.PostThreadMessage(WMU_SETLINKCFG, m_lCfgSequence, (LPARAM)pLinkLst);

    CCdBlkCfgArray *pCdBlkLst= new CCdBlkCfgArray;
    pCdBlkLst->SetSize(m_CdBlks.GetSize());
    for (i=0; i<m_CdBlks.GetSize(); i++)
      {
      CCdBlk &C=*m_CdBlks[i];
      C.m_lCfgSequence=m_lCfgSequence; // Mark as Valid
      (*pCdBlkLst)[i]=new CCdBlkCfg(*m_CdBlks[i]);
      }
    theApp.PostThreadMessage(WMU_SETCDBLKCFG, m_lCfgSequence, (LPARAM)pCdBlkLst);

    CDeviceCfgArray *pDeviceLst= new CDeviceCfgArray;
    pDeviceLst->SetSize(m_Devices.GetSize());
    for (i=0; i<m_Devices.GetSize(); i++)
      {
      CDevice &D=*m_Devices[i];
      //D.m_lCfgSequence=m_lCfgSequence; // Mark as Valid
      (*pDeviceLst)[i]=new CDeviceCfg(*m_Devices[i]);
      }
    theApp.PostThreadMessage(WMU_SETDEVICECFG, m_lCfgSequence, (LPARAM)pDeviceLst);

    for (i=0; i<m_Slots.GetSize(); i++)
      m_Slots[i]->SendStateValue();

    for (i=0; i<m_Links.GetSize(); i++)
      m_Links[i]->SendStateValue();

    }

  //if (Doing==eDoingCfgLoad)
  //  Load("LastState");

  if (m_bRunningB4Cfg)
    Start();

  //  m_Lock.Unlock();
  if (Doing!=eDoingCfgStop)
    {
    for (i=0; i<m_Slots.GetSize(); i++)
      m_Slots[i]->SetValueID("OnLoad", m_Slots[i]->m_vValue, eCSD_Slot, i);  // what about "OnReLoad"

    for (i=0; i<m_Links.GetSize(); i++)
      m_Links[i]->SetValueID("OnLoad", m_Links[i]->m_vValue, eCSD_Link, i);  // what about "OnReLoad"
    }

  }

// -----------------------------------------------------------------------

void CSlotMngr::DoSetSimulateMode(boolean On)
  {
  for (int i=0; i<m_Slots.GetSize(); i++)
    m_Slots[i]->SetValueID(On ? "OnSimOn": "OnSimOff", m_Slots[i]->m_vValue, eCSD_Slot, i);  // what about "OnReLoad"

  for (i=0; i<m_Links.GetSize(); i++)
    m_Links[i]->SetValueID(On ? "OnSimOn": "OnSimOff", m_Links[i]->m_vValue, eCSD_Link, i);  // what about "OnReLoad"
  }

// -----------------------------------------------------------------------

void CSlotMngr::DoSetValues(CSetValuesOptions & Opt)
  {
  m_Slots2Write.SetSize(m_Slots.GetSize());
  m_nSlots2Write=0;
  m_Links2Write.SetSize(m_Links.GetSize());
  m_nLinks2Write=0;
  int i;
  switch (Opt.m_eWhich)
    {
    case CSetValuesOptions::eAll:
      for (i=0; i<m_Slots.GetSize(); i++)
        m_Slots2Write[i]=m_nSlots2Write++;
      for (int i=0; i<m_Links.GetSize(); i++)
        m_Links2Write[i]=m_nLinks2Write++;
      break;
    case CSetValuesOptions::eFiltered:
      for (int i=0; i<m_Slots.GetSize(); i++)
        if (m_Slots[i]->m_bInFilter)
          m_Slots2Write[i]=m_nSlots2Write++;
      for (int i=0; i<m_Links.GetSize(); i++)
        if (m_Links[i]->m_bInFilter)
          m_Links2Write[i]=m_nLinks2Write++;
      break;
    case CSetValuesOptions::eSelected:
      break;
    case CSetValuesOptions::eGroups:
      break;
    }
  switch (Opt.m_eAction)
    {
    case CSetValuesOptions::eDoNothing:
      return;
    case CSetValuesOptions::eRefresh:
      for (i=0; i<m_Slots2Write.GetSize(); i++)
        {
        CSlot & S = *m_Slots[m_Slots2Write[i]];
        //if (m_Slots[i]->m_bWrite)
        m_Slots[i]->WriteCurrentValue2Device();
        }
      break;
    case CSetValuesOptions::eSet:
      break;
    }
  //else if (Opt.GrpName && strlen(Opt.GrpName)>0)
  //  {
  //  for (int i=0; i<m_Slots.GetSize(); i++)
  //    m_Slots[i]->SetValueID(GrpName, m_Slots[i]->m_vValue, eCSD_Slot, i);  // what about "OnReLoad"

  //  for (i=0; i<m_Links.GetSize(); i++)
  //    m_Links[i]->SetValueID(GrpName, m_Links[i]->m_vValue, eCSD_Link, i);  // what about "OnReLoad"
  //  }
  }

// -----------------------------------------------------------------------

void CSlotMngr::Save(LPCTSTR Name)
  {
  Strng SaveFile(m_Cfg.m_sCfgPath);
  SaveFile+="\\";
  SaveFile+=Name;
  SaveFile.FnCheckExtension("sms");
  CProfINIFile SavePF(SaveFile());
  SavePF.WrLong("Counts", "Links", NLinks);
  SavePF.WrLong("Counts", "Slots", NSlots);
  SavePF.WrLong("Counts", "Devices", NDevices);

  for (int i=0; i<NDevices; i++)
    m_Devices[i]->Save(SavePF);
  for (int i=0; i<NLinks; i++)
    m_Links[i]->Save(SavePF);
  for (int i=0; i<NSlots; i++)
    m_Slots[i]->Save(SavePF);
  }

// -----------------------------------------------------------------------

void CSlotMngr::Load(LPCTSTR Name)
  {
  Strng LoadFile(m_Cfg.m_sCfgPath);
  LoadFile+="\\";
  LoadFile+=Name;
  LoadFile.FnCheckExtension("sms");
  CProfINIFile LoadPF(LoadFile());
  //NLinks = LoadPF.RdLong("Counts", "Links", 0);
  //NSlots = LoadPF.RdLong("Counts", "Slots", 0);
  //NDevices = LoadPF.RdLong("Counts", "Devices", 0);

  for (int i=0; i<NDevices; i++)
    m_Devices[i]->Load(LoadPF);
  for (int i=0; i<NLinks; i++)
    m_Links[i]->Load(LoadPF);
  for (int i=0; i<NSlots; i++)
    m_Slots[i]->Load(LoadPF);
  RequestQueueFlush();
  }

// -----------------------------------------------------------------------

void CSlotMngr::CheckDevices(void)
  {

  bool WriteAll = false;
  for (long d=0; d<NDevices; d++)
    {
    CDevice &D=*Devices[d];
    if (!D.m_bLocalOnly && !D.ServerGood())
      {
      D.m_OPC.m_dwShutdownConnection = true;
      //D.m_OPC.Disconnect();//DisConnect(); IsOK seems to return OK in Disconnect
      //method? Call m_OPC.Disconnect directly
      D.DisConnect();
      //
      // Reconnect to the OPC Server if possible
      //
      if ( D.Connect() )
        {

        // Add all the device items to the group
        // NB: When originally configured use AddSlots passing
        // the necessary slot data. Subsequent reconnect can
        // use the information that the Device has a local copy of
        D.AddSlotsReconnect();

        // Set Device OPC Group to Active
        Devices[d]->SetActiveState(true);

        // Should we resend the write data now?
        WriteAll = true;
        }

      }

    if (WriteAll) 
      WriteAllSlots2Devices();

    }
  }

// -----------------------------------------------------------------------

void CSlotMngr::AddCdBlk(CAddCdBlk * pAdd)
  {
  bool WasRunning=m_bRunning;
  if (WasRunning)
    Stop();

  //dbgpln(">>AddCdBlk %4i %4i        %s",m_CdBlks.GetSize(), m_CdBlkMap.GetCount(), pAdd->m_sTag);
  //  if (nFlds>=3 && c[0] && c[2])
  //    {
  //    Strng BlkName=c[0];
  //    Strng BlkOpts=c[1]?c[1]:"";
  long index;
  CString CdFile=gs_SlotMngr.m_Cfg.m_sCfgPath;
  CdFile+=pAdd->m_sCdBlk;

  CCdBlk *pOrig=NULL;
  if (!m_CdBlkMap.Lookup(pAdd->m_sTag, index))
    {
    index=m_CdBlks.GetSize();
    m_CdBlks.SetSize(index+1,1024);
    }
  else
    {
    // Must first remove before delete
    if (!m_CdBlkMap.RemoveKey(pAdd->m_sTag))
      {
      INCOMPLETECODE();
      }
    pOrig=m_CdBlks[index];
    }

  CCdBlk *pNew= new CCdBlk ;
  if (!pNew->Parse(pAdd, CdFile))
    {
    if (!pNew->m_State.m_nError)
      pNew->SetError(SErr_NotLoaded); // Just in Case
    }
  else if (pOrig)
    {
    delete pOrig;
    }

  pNew->m_lCdBlk=index;
  m_CdBlks[index]=pNew;
  m_CdBlkMap.SetAt(pNew->m_sTag, index);
  //
  //    CDrvCodeBlk * p=new CDrvCodeBlk;
  //
  }


// -----------------------------------------------------------------------

long CSlotMngr::AddLink(CSlot * pSlot, LPCSTR Tag, LPCSTR ConnectTag, LPCSTR Cnv, VARTYPE VType, bool Get, bool Set)
  {
  long index;
  bool AddIt=true;
  if (m_LinkMap.Lookup(Tag, index))
    {
    if (ConnectTag==NULL) ConnectTag="";
    if (Cnv==NULL) Cnv="";
    CLink & L=*m_Links[index];
    if ((L.m_sSimTag.CompareNoCase(ConnectTag)==0) &&
      (L.m_sCnvTxt.CompareNoCase(Cnv)==0) && 
      (L.m_vValue.vt==VType) &&
      (L.m_bGet==Get) &&
      (L.m_bSet==Set))
      {
      m_Links[index]->m_bInUse=true;
      AddIt=false;
      }
    }

  if (AddIt)
    {
    index=m_Links.GetSize();
    m_Links.SetSize(index+1,1024);
    CLink *pNew= new CLink(pSlot, Tag, ConnectTag, Cnv, pSlot->m_sLinkValues, pSlot->m_sDesc, VType, Get, Set) ;
    m_Links[index]=pNew;
    m_LinkMap.SetAt(pNew->m_sTag, index);
    pNew->m_bInUse=true;
    }

  return index;
  };

// -----------------------------------------------------------------------

void CSlotMngr::EditSlot(bool DeleteIfFound, CSlotCfgStrings * pCfg)
  {
  bool WasRunning=m_bRunning;
  if (pCfg->m_sTag.GetLength()>0)
    {
    if (WasRunning)
      Stop();


    //dbgpln(">>AddSlot %4i %4i        %s",m_Slots.GetSize(), m_SlotMap.GetCount(), pAdd->m_sTag);

    CSlot *pOrig=NULL;
    long index;
    if (DeleteIfFound)
      {
      if (m_SlotMap.Lookup(pCfg->m_sTag, index))
        {
        VERIFY(m_SlotMap.RemoveKey(pCfg->m_sTag));
        delete m_Slots[index];
        for ( ;index+1<m_Slots.GetSize(); index++)
          m_Slots[index]=m_Slots[index+1];
        m_Slots.SetSize(m_Slots.GetSize()-1);
        return;
        }
      }
    else if (!m_SlotMap.Lookup(pCfg->m_sTag, index))
      {
      index=m_Slots.GetSize();
      m_Slots.SetSize(index+1,1024);
      }
    else
      {
      // Must first remove before delete
      if (!m_SlotMap.RemoveKey(pCfg->m_sTag))
        {
        INCOMPLETECODE();
        }
      pOrig=m_Slots[index];
      }

    CSlot *pNew= new CSlot;
    if (!pNew->Parse(pCfg))
      {
      if (!pNew->m_State.m_nError)
        pNew->SetError(SErr_NotLoaded); // Just in Case
      }
    else if (pOrig)
      {
      if (!pOrig->MustReConnect(pNew))
        {
        // Must retain all flags etc including m_bConnected
        // BUT must update other configs.
        pOrig->TransferConfiguration(pNew);
        delete pNew;
        pNew=pOrig;
        }
      else
        delete pOrig;
      }

    pNew->m_lSlot=index;
    m_Slots[index]=pNew;
    m_SlotMap.SetAt(pNew->m_sTag, index);

    //dbgpln("  AddSlot %4i %4i [%4i] %s",m_Slots.GetSize(), m_SlotMap.GetCount(), index, pAdd->m_sTag);

    pNew->m_bInUse=true;

    if (WasRunning)
      Start();
    }

  delete pCfg;
  }

// -----------------------------------------------------------------------

void CSlotMngr::AddDelayedChange(CChangeItem * pNew, DWORD Timer)
  {
  if (Timer!=InfiniteDelay)
    {
    pNew->m_dwDelayTimer = Timer;
    if (dbgTimers || dbgChanges)
      {
      CString S, S1;
      dbgp("AddDelayedChg[     %10i] %13s %20s to %-9s",
        pNew->m_dwDelayTimer, "", "", SrcDstString(pNew->m_eDst));
      if (pNew->m_eDst==eCSD_SlotConn)
        dbgp(" [%4i][%i]", GetSlotIndex(pNew->m_lDstInx), GetConnIndex(pNew->m_lDstInx));
      else
        dbgp(" [%4i]   ", pNew->m_lDstInx);
      dbgpln(" %-10s %15s",
        TypeToString(pNew->Type()), VariantToString(pNew->m_vValue, S, false));
      }

    if (Timer==0)
      { // No Delay
      m_ChangeList.AddTail(pNew);
      }
    else
      {
      pNew->m_dwDelayTimer=Max(pNew->m_dwDelayTimer, m_Cfg.m_dwDelayResolution); // enforce at least 1 interval delay
      DWORD T1=pNew->m_dwDelayTimer;
      // Add in order
      CChangeItem * pChg=m_DelayChgList.Head();
      CChangeItem * pPrev=NULL;
      while (pChg && pChg->m_dwDelayTimer<=T1)
        {
        pPrev=pChg;
        pChg=pChg->m_pNext;
        }
      if (pChg)
        {
        if (pPrev)
          m_DelayChgList.AddAfter(pPrev, pNew);
        else
          m_DelayChgList.AddHead(pNew);
        }
      else
        m_DelayChgList.AddTail(pNew);
      }
    }
  else
    {
    // This Change to be ignored
    delete pNew;
    }
  }

// -----------------------------------------------------------------------

void CSlotMngr::AppendChange(eConnSrcDst Src, long SrcI, eConnSrcDst Dst, long DstI, DWORD TransID, CFullValue &FullValue, CDelayBlock *pDelay, bool OverrideHold, bool Refresh)
  {
  if (dbgChanges)
    {
    if (Src==eCSD_Manual)
      dbgpln("--------------------------------------------------------------------------");
    CString S, S1;
    dbgp("AppendChange %24s %6i from %-9s[%4i] to %-9s", 
      "", CChangeItem::sm_dwNumber,  SrcDstString(Src), SrcI, SrcDstString(Dst));
    if (Dst==eCSD_SlotConn)
      dbgp(" [%4i][%i]", GetSlotIndex(DstI), GetConnIndex(DstI));
    else
      dbgp(" [%4i]   ", DstI);
    dbgpln(" %-10s %15s @%s", 
      TypeToString(FullValue.Type()), VariantToString(FullValue, S, false),
      TimeStampToString(FullValue.m_ftTimeStamp, S1, true, NULL));
    }

  if (!m_bDoingFullCfg)
    {
    CChangeItem * pNew=new CChangeItem(Src, SrcI, Dst, DstI, -1, TransID, FullValue, OverrideHold, Refresh);
    if (!pDelay || !pDelay->Configured())
      {
      m_ChangeList.AddTail(pNew);
      }
    else
      {
      //CFullValue CurrentV;
      int Direction=0;
      switch (pNew->m_eSrc)
        {
        case eCSD_Link:
          {
          int I=pNew->m_lSrcInx;
          if (I>=0 && I<m_Links.GetSize())
            //CurrentV=m_Links[I]->FullValue();
            Direction=m_Links[I]->m_iLastChgDirn;
          else
            ReportError("AppendChange", 0, "Change %i has Bad Link Index %i <0 or >%i ", pNew->m_dwNumber, I, m_Links.GetSize()-1);
          break;
          };
        case eCSD_Slot:
          {
          int I=pNew->m_lSrcInx;
          if (I>=0 && I<m_Slots.GetSize())
            //CurrentV=m_Slots[I]->FullValue();
            Direction=m_Slots[I]->m_iLastChgDirn;
          else
            ReportError("AppendChange", 0, "Change %i has Bad Slot Index %i <0 or >%i ", pNew->m_dwNumber, I, m_Slots.GetSize()-1);
          break;
          }
        default:
          ReportError("AppendChange", 0, "Invalid Source %i ", pNew->m_eSrc);
        }

      if (0)
        dbgpln("=== %s ", Direction<0?"Falling":Direction>0?"Rising":"STEADY");

      if (pDelay->m_UseValues)
        {
        if (Direction!=0)
          {
          CArray<CDelayBlockItem, CDelayBlockItem&> & DBA = Direction<0? pDelay->m_OnFall : pDelay->m_OnRise;
          int n=DBA.GetCount();
          for (int i=0; i<n; i++)
            {

            CDelayBlockItem &DB = DBA[i];
            CChangeItem * pChg=new CChangeItem(Src, SrcI, Dst, DstI, -1, TransID, FullValue, OverrideHold, Refresh);
            pChg->m_vValue=DB.m_Value;
            AddDelayedChange(pChg, DB.m_dwTime);
            }
          }
        else
          {
          // Ignore;
          }
        delete pNew; // done with this
        }
      else
        {
        if (Direction!=0)
          {
          DWORD Timer;
          if (Direction<0)// && pDelay->m_dwTimeFall)
            Timer = pDelay->m_OnFall[0].m_dwTime; 
          else
            Timer = pDelay->m_OnRise[0].m_dwTime; 

          AddDelayedChange(pNew, Timer);
          }
        else
          {
          // Ignore;
          }
        }
      }
    }
  };

// -----------------------------------------------------------------------

long CSlotMngr::RequestQueueFlush()
  {
  PostThreadMessage(m_dwThreadId, WMU_FLUSHQ, 0, 0);
  return m_ChangeList.Count();
  };

// -----------------------------------------------------------------------

bool CSlotMngr::ApplyChange(CChangeItem * pChg, bool IsDelay)
  {
  m_dwTotalChanges++;

  if (dbgChanges)
    {
    CString S, S1;
    //dbgpln("Apply Change %24s        from %-9s[%4i] to %-9s[%4i] %-10s %15s @%s %6i", 
    //  "", SrcDstString(pChg->m_eSrc), pChg->m_lSrcInx, SrcDstString(pChg->m_eDst), pChg->m_lDstInx,
    //  TypeToString(pChg->Type()), VariantToString(pChg->m_vValue, S, false),
    //  TimeStampToString(pChg->m_ftTimeStamp, S1, true, NULL),
    //  pChg->m_dwDelayTimer
    //  );

    dbgp("Apply Change %24s        from %-9s[%4i] to %-9s", 
      "", SrcDstString(pChg->m_eSrc), pChg->m_lSrcInx, SrcDstString(pChg->m_eDst));
    if (pChg->m_eDst==eCSD_SlotConn)
      dbgp(" [%4i][%i]", GetSlotIndex(pChg->m_lDstInx), GetConnIndex(pChg->m_lDstInx));
    else
      dbgp(" [%4i]   ", pChg->m_lDstInx);
    dbgpln(" %-10s %15s @%s", 
      TypeToString(pChg->Type()), VariantToString(pChg->m_vValue, S, false),
      TimeStampToString(pChg->m_ftTimeStamp, S1, true, NULL));

    }

  LPCSTR Msg= IsDelay ? "FlushDelayChgQueue":"FlushChangeQueue";
  long I;
  switch (pChg->m_eDst)
    {
    case eCSD_Link:
      {
      I=pChg->m_lDstInx;
      if (I>=0 && I<m_Links.GetSize())
        {
        if (m_Links[I]->SetValue(pChg))
          {
          m_HistoryList.AddTail(pChg);
          pChg=NULL;
          }
        //else
        //  ReportError(Msg, 0, "Link Value Not Set %s", m_Slots[I]->m_sTag);
        }
      else
        ReportError(Msg, 0, "Change %i has Bad Link Index %i <0 or >%i ", pChg->m_dwNumber, I, m_Links.GetSize()-1);
      //if (I>=0 && I<m_Slots.GetSize())
      // m_Links[I]->SetValue(pChg));
      }
      break;
    case eCSD_Slot:
      I=pChg->m_lDstInx;
      if (I>=0 && I<m_Slots.GetSize())
        {
        bool DoSet=true;
        CSlot &S=*m_Slots[I];
        if (m_dwFlushChangeCalls==S.m_dwFlushChangeCalls)
          {
          S.m_dwChangesInCall++;
          if (S.m_dwChangesInCall>m_Cfg.m_dwMaxChangesInCall)
            {
            S.SetError(SErr_PotentialRecursion, 0, "");
            DoSet=false;
            };
          }
        else
          {
          S.m_dwFlushChangeCalls=m_dwFlushChangeCalls;
          S.m_dwChangesInCall=0;
          }

        if (DoSet && m_Slots[I]->SetValue(pChg))
          {
          m_HistoryList.AddTail(pChg);
          pChg=NULL;
          }
        //else
        //  ReportError(Msg, 0, "Slot Value Not Set %s", m_Slots[I]->m_sTag);
        }
      else
        ReportError(Msg, 0, "Change %i has Bad Slot Index %i <0 or >%i ", pChg->m_dwNumber, I, m_Slots.GetSize()-1);
      break;
    case eCSD_SlotConn:
      I=GetSlotIndex(pChg->m_lDstInx);
      if (I>=0 && I<m_Slots.GetSize())
        {
        int C=GetConnIndex(pChg->m_lDstInx);
        CSlot &S=*m_Slots[I];
        S.m_SetConnects[C]->Process(pChg->m_eSrc, pChg->m_lSrcInx, eCSD_SlotConn, pChg->m_lDstInx, (*pChg), 0);
        }
      else
        ReportError(Msg, 0, "Change %i has Bad Slot Index %i <0 or >%i ", pChg->m_dwNumber, I, m_Slots.GetSize()-1);
      break;
    default:
      ReportError(Msg, 0, "Invalid Destination %s ", SrcDstString(pChg->m_eDst));
    }
  bool RetOK=(pChg!=NULL);
  delete pChg;
  return RetOK;
  }

// -----------------------------------------------------------------------

long CSlotMngr::FlushChangeQueue()
  {
  long dbgon=m_ChangeList.Count()>0;
  if (dbgflushCQ && dbgon) dbgpln(">>FlushChangeQueue %i", m_ChangeList.Count());
  long n=0;

  m_dwFlushChangeCalls++;

  // Should improve the Wraparound Handling
  DWORD Ticks=GetTickCount();
  DWORD DT= (Ticks>m_dwLastTickCount) ? Ticks-m_dwLastTickCount: 0;
  DWORD DTTst= (Ticks>m_dwLastTickCount) ? Ticks-m_dwLastTickCount: 0;
  m_dwLastTickCount=Ticks;

  CChangeItem * pChg;
  if (/*1 ||*/ DT>0)
    {
    // Remove expired delays and add to ChangeList to be acted on 
    for (;;)
      {
      m_DelayChgList.Lock();
      pChg=m_DelayChgList.Head();
      if (pChg && (pChg->m_dwDelayTimer<=DT))
        {
        pChg=m_DelayChgList.RemoveHead();
        m_DelayChgList.UnLock();
        n++;

        // Update TimeStamp;
        CoFileTimeNow(&pChg->m_ftTimeStamp);
        //ApplyChange(pChg, true);
        m_ChangeList.AddTail(pChg);
        }
      else
        {
        m_DelayChgList.UnLock();
        break;
        }
      }

    m_DelayChgList.Lock();
    pChg=m_DelayChgList.Head();
    while (pChg!=NULL)
      {
      pChg->Advance(DT);
      pChg=pChg->m_pNext;
      }
    m_DelayChgList.UnLock();
    }

  while ((pChg=m_ChangeList.RemoveHead())!=NULL)
    {
    n++;
    ApplyChange(pChg, false);
    }
  if (dbgflushCQ && dbgon) dbgpln("<<FlushChangeQueue %i", m_ChangeList.Count());
  return n;
  }

// -----------------------------------------------------------------------

long CSlotMngr::FlushHistoryQueue()
  {
  long dbgon=m_HistoryList.Count()>0;
  long n=0;
  if (dbgflushHQ && dbgon) dbgpln(">>FlushHistoryQueue %i", m_HistoryList.Count());
  CChangeItem *pChg=m_HistoryList.RemoveHead();
  CChangeBlock *pBlock=NULL;
  while (pChg)
    {
    CChangeItem *pNxt=m_HistoryList.RemoveHead();
    if (m_bMonitorOn)
      {

      if (!pBlock) 
        pBlock=new CChangeBlock;
      pBlock->Add(pChg);
      if (!pNxt || !pBlock->HasSpace())
        {
        theApp.PostThreadMessage(WMU_XFERCHANGEBLOCK, 0, (LPARAM)pBlock);
        pBlock=NULL;
        }
      n++;
      }
    else
      delete pChg;
    pChg=pNxt;
    }
  if (dbgflushHQ && dbgon) dbgpln("<<FlushHistoryQueue %i", m_HistoryList.Count());
  return n;
  }

// -----------------------------------------------------------------------

long CSlotMngr::FlushWriteList()
  {
  long n=0;
  for (long d=0; d<NDevices; d++)
    if (Devices[d]->Connected())
      n+=Devices[d]->FlushWriteList();

  return n;
  }

// -----------------------------------------------------------------------

long CSlotMngr::WriteAllSlots2Devices()
  {
  long n=0;
  for (long s=0; s<NSlots; s++)
    {
    if (Slots[s]->WriteCurrentValue2Device())
      n++;
    }
  return n;
  };

// -----------------------------------------------------------------------

long CSlotMngr::TrickleRefresh()
  {
  //static long s = 0;
  long DoneCount=0;

  if ( NSlots > 0 )
    {
    for (long d=0; d<NDevices; d++)
      {
      CDevice &D=*Devices[d];
      DWORD NSlots=D.NSlots;
      DWORD Count=D.m_dwTrickleCount;
      DWORD &s=D.m_dwTrickleIndex;
      //
      // Write N Slots at a time
      //
      if (Count>0 && NSlots>0)
        {
        DWORD Start=Range(DWORD(0), s, NSlots);
        DWORD NoWritten=0;
        while (NoWritten<Count)
          {
          //if (D.Slots[I]->m_CCfg.m_pGetConnect->m_bL
          if (D.Slots[s]->WriteCurrentValue2Device())
            NoWritten++;
          s++;
          if (s>=NSlots)
            s=0;
          if (s==Start)
            break; // Quit if we have wrapped around (Number of writable slots < 100)
          DoneCount++;
          }
        }
      }

    }

  return DoneCount;
  };

// -----------------------------------------------------------------------

void CSlotMngr::SendSimValue2Link(eConnSrcDst Src, long LnkIndex, DWORD TransID, CFullValue &FullValue, bool OverrideHold, bool Refresh)
  {
  CLinkCfg & Cfg=*m_Links[LnkIndex];
  if (Cfg.m_CondBlk.m_On && IsFloatDataVT(FullValue.Type()))
    {
    CFullValue FV(FullValue, OPC_QUALITY_GOOD);
    HRESULT hr=VariantChangeType(&FV.m_vValue, &FV.m_vValue, 0, VT_R8);
    Cfg.m_CondBlk.m_BaseValue       = FAILED(hr) ? 0.0 : FV.m_vValue.dblVal; 
    Cfg.m_CondBlk.m_BaseValueValid  = true;
    Cfg.m_CondBlk.m_ChgCount        = 0;
    Cfg.m_CondBlk.m_BaseChg         = true; 
    // let cond blk update the actual values
    }
  else
    gs_SlotMngr.AppendChange(Src, -1, eCSD_Link, LnkIndex, TransID, FullValue, NULL, OverrideHold, Refresh);
  }

// -----------------------------------------------------------------------

void CSlotMngr::ProcessCondBlks()
  {
  DWORD Ticks=GetTickCount();
  if (Ticks-m_dwLastCBProcessTime<m_Cfg.m_dwConditioningCycle)
    return;
  double DeltaTime=(Ticks-m_dwLastCBProcessTime)*0.001;

  for (long i=0; i<m_LinksWithCondBlks.GetSize(); i++)
    {
    long Index=m_LinksWithCondBlks[i];
    CLink & L = *m_Links[Index];
    CLinkCondBlk &CB=L.m_CondBlk;
    if (CB.m_On && CB.m_BaseValueValid)
      {
      double d=CB.m_BaseValue;

      CSlotConnArray &RGs=L.m_ReflectedGets;
      for (int rg=0; rg<RGs.GetSize(); rg++)
        {
        CSlotConnOpArray &Ops=RGs[rg]->m_Ops;
        for (int op=0; op<Ops.GetSize(); op++)
          d=Ops[op]->DoConditioning(d, DeltaTime);
        }

      COleVariant T(d);
      CFullValue FV(OPC_QUALITY_GOOD);
      HRESULT hr=VariantChangeType(&FV.m_vValue, &T, 0, L.Type());
      CB.m_ChgCount++;
      gs_SlotMngr.AppendChange(eCSD_Simulator, -1, eCSD_Link, Index, 0, FV, NULL);
      }
    }
  m_dwLastCBProcessTime=Ticks;
  }

// -----------------------------------------------------------------------

void CSlotMngr::StartThread(LPCSTR CfgFile)
  {
  //dbgpln("==== StartThread >>");
  if (gs_SlotMngr.m_bRunOnMainThread)
    {
    }
  else
    {
    m_pExecThread = AfxBeginThread(CSlotMngr::staticMessageHandler, (LPVOID)&gs_SlotMngr,
      m_Cfg.m_nPriorityThread, 1000*1024L, CREATE_SUSPENDED);
    if (m_pExecThread==NULL)
      {
      MessageBox(GetFocus(),"Error Creating COPCWork Thread!","",MB_OK);
      VERIFY(0);
      //return 1;
      }
    else
      m_pExecThread->ResumeThread();
    m_dwThreadId=m_pExecThread->m_nThreadID;

    m_Cfg.SetDefault();
    if (CfgFile)
      {
      m_Cfg.m_sCfgFile=CfgFile? CfgFile:"";
      char Drv[_MAX_DRIVE], Dir[_MAX_DIR], FNm[_MAX_FNAME], Ext[_MAX_EXT];
      _splitpath(m_Cfg.m_sCfgFile, Drv, Dir, FNm, Ext);
      m_Cfg.m_sCfgPath=Drv;
      m_Cfg.m_sCfgPath+=Dir;
      m_Cfg.Read(m_Cfg.m_sCfgFile);
      }

    SetPriorityClass(GetCurrentProcess(), m_Cfg.m_nPriorityClass);                  // Set App PriorityClass
    m_pExecThread->SetThreadPriority(m_Cfg.m_nPriorityThread);  // Set Mngr Thread Priority

    }
  //dbgpln("==== << StartThread");
  }

// -----------------------------------------------------------------------

void CSlotMngr::StopThread()
  {
  if (m_pExecThread)
    {
    //dbgpln("==== StopThread >>");
    if (m_bRunOnMainThread)
      {
      }
    else
      {
      HANDLE hThread=m_pExecThread->m_hThread;
      // create a config with no slots ie Disconnect all

      PostThreadMessage(ThreadId(), WMU_CFGBEGIN, eDoingCfgStop, 0);
      PostThreadMessage(ThreadId(), WMU_CFGEND, eDoingCfgStop, 0);
      // then quit
      PostThreadMessage(ThreadId(), WM_QUIT, 0, 0);
      WaitForSingleObject(hThread, INFINITE);
      }
    //dbgpln("==== <<StopThread");
    }
  else
    {
    //dbgpln("Thread Stopped");
    }
  }

// -----------------------------------------------------------------------

CString CSlotMngr::FullFileName(LPCSTR Fn)
  {
  char Drv[_MAX_DRIVE], Dir[_MAX_DIR], FNm[_MAX_FNAME], Ext[_MAX_EXT];
  _splitpath(Fn, Drv, Dir, FNm, Ext);
  CString S;      
  if (strlen(Drv)==0 && strlen(Dir)==0)
    S=m_Cfg.m_sCfgPath+Fn;
  else
    S=Fn;   
  return S;
  }

// -----------------------------------------------------------------------

void CSlotMngr::SendUpdateStatus()
  {
  CStatusInfoBlock *pCB=new CStatusInfoBlock;
  pCB->m_dwSlots=NSlots;
  pCB->m_dwLinks=NLinks;
  pCB->m_dwChanges=m_ChangeList.Count();
  pCB->m_dwDelays=m_DelayChgList.Count();
  pCB->m_dwTotChgs=m_dwTotalChanges;
  pCB->m_dwHolds=m_dwTotalHolds;
  BOOL OK=theApp.PostThreadMessage(WMU_UPDATESTATUS, 0, (long)pCB); 
  }

// -----------------------------------------------------------------------

const int dbgThreadMsgs = 0;

void CSlotMngr::MessageHandler()
  {
  //dbgpln("----------------------MessageHandler Start");
  HRESULT hr = 0;
  switch (OpcThreadingMTA)
    {
    case OpcThreadingSTA:
      hr = CoInitialize(NULL);
      break;
    case OpcThreadingAPART:
      hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
      break;
    default:
    case OpcThreadingMTA:
      hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
      break;
    }
  if (FAILED(hr))
    {
    ReportErrorBox(hr, "CoInitialize failed, %lx", hr);
    }
  ASSERT(SUCCEEDED(hr));

  SendUpdateStatus();

  SetTimer(NULL, 0, m_Cfg.m_dwDelayResolution, NULL);//TimerProc);
  m_dwLastTickCount=GetTickCount();

  MSG Msg;
  bool Done=false;
  long NCount = 0;
  bool bEditMode=false;
  bool bShuttingDown=false;
  while (!Done && GetMessage(&Msg, NULL, 0,0))
    {
    CMsgLoopLock Lk("Message");
    switch (Msg.message)
      {
      case WMU_START:
        if (dbgThreadMsgs ) dbgpln("WMU_START");
        if (!bEditMode)
          Start();
        break;
      case WMU_STOP:
        if (dbgThreadMsgs ) dbgpln("WMU_STOP");
        if (!bEditMode)
          Stop();
        break;
      case WMU_CFGBEGIN:
        {
        if (dbgThreadMsgs ) dbgpln("WMU_CFGBEGIN");
        if (!bEditMode)
          {
          KillTimer(NULL, 0);
          CfgBegin((eDoingCfg)Msg.wParam, false);
          }
        break;
        }
      case WMU_CFGEND:
        if (dbgThreadMsgs ) dbgpln("WMU_CFGEND");
        if (!bEditMode)
          {
          bShuttingDown=(Msg.wParam==eDoingCfgStop);
          CfgEnd((eDoingCfg)Msg.wParam);
          SetTimer(NULL, 0, m_Cfg.m_dwDelayResolution, NULL);//TimerProc);
          }
        break;
      case WMU_SETSIMMODE:
        if (dbgThreadMsgs ) dbgpln("WMU_SETSIMMODE");
        DoSetSimulateMode(Msg.wParam!=0);
        break;

      case WMU_SETVALUES:
        DoSetValues(*((CSetValuesOptions*)Msg.lParam));
        delete ((CSetValuesOptions*)Msg.lParam);
        break;

      case WMU_ADDCDBLK:
        if (dbgThreadMsgs ) dbgpln("WMU_ADDCDBLK");
        if (!bEditMode)
          AddCdBlk((CAddCdBlk*)Msg.lParam);
        break;
      case WMU_ADDSLOT:
        if (!bEditMode)
          EditSlot(false, (CSlotCfgStrings*)Msg.lParam);
        break;

      case WMU_EDITSTART:
        if (dbgThreadMsgs ) dbgpln("WMU_EDITSTART");
        bEditMode=true;
        KillTimer(NULL, 0);
        CfgBegin(eDoingCfgReLoad, true);
        break;
      case WMU_EDITSLOT:
        if (dbgThreadMsgs ) dbgpln("WMU_EDITSLOT");
        if (bEditMode)
          {
          CSlotCfgStrings* pCfg=(CSlotCfgStrings*)Msg.lParam;
          EditSlot(pCfg->m_eDoWhat==SlotCfgDelete, pCfg);
          }
        break;
      case WMU_EDITEND:
        if (dbgThreadMsgs ) dbgpln("WMU_EDITEND");
        CfgEnd(eDoingCfgReLoad);
        bEditMode=false;
        SetTimer(NULL, 0, m_Cfg.m_dwDelayResolution, NULL);//TimerProc);
        break;

      case WMU_FLUSHQ:
        if (dbgThreadMsgs ) dbgpln("WMU_FLUSHQ");
        if (!bEditMode && ! bShuttingDown)
          {
          NCount=FlushChangeQueue();
          FlushWriteList();
          }
        break;
      case WMU_SETVISIBLE:
        if (!bEditMode && ! bShuttingDown)
          {
          long *pList=(long*)Msg.lParam;
          ApplySetVisible(
            (Msg.wParam&0x0f000000)>>24,
            (Msg.wParam&0x10000000)!=0, 
            (signed char)(Msg.wParam&0x00ffffff), 
            pList);
          delete pList;
          }
        break;
      case WMU_SETHOLD:
        if (!bEditMode && ! bShuttingDown)
          ApplyHold((eConnSrcDst)(Msg.wParam&0x0fffffff), Msg.lParam, Msg.wParam&0x10000000?1:0);
        break;
        //case WMU_FINDSTR:
        //  {
        //  CString * p=(CString*)Msg.lParam;
        //  ApplyFind(Msg.wParam, p->GetBuffer());
        //  delete p;
        //  }
        break;
      case WMU_CHANGEMONITOR:               
        if (!bEditMode && ! bShuttingDown)
          {
          m_bMonitorOn=Msg.wParam!=0;
          if (m_bMonitorOn)
            {
            for (int i=0; i<m_Links.GetSize(); i++)
              if (m_Links[i]->m_bVisible)
                m_Links[i]->SendDisplayValue();
            for (i=0; i<m_Slots.GetSize(); i++)
              if (m_Slots[i]->m_bVisible)
                m_Slots[i]->SendDisplayValue();
            }
          }
        break;
      case WM_TIMER:

        if (!bEditMode && ! bShuttingDown)
          {
          if (!m_bDoingFullCfg)
            {

            // Check all devices to see if OPC
            // server is healthy and re-establish connections
            // if they have dropped out for whatever reason
            CheckDevices();

            ProcessCondBlks();

            if (dbgThreadMsgs ) dbgpln("WM_TIMER");
            NCount=FlushChangeQueue();
            FlushWriteList();
            DWORD Ticks=GetTickCount();
            if (Ticks-m_dwLastChangeFlushTicks>m_Cfg.m_dwChangeFlushCycle)
              {
              m_dwLastChangeFlushTicks=Ticks;
              FlushHistoryQueue();


              //
              // Trying to cope with RSLinx issue where it appears to
              // fail writing data to the PLC but does not let Marshall know
              // And even if it did let marshall know we dont do anything with the
              // advice as yet. This routine will put 100 slots every timer interval
              // into the queue. Large app is around 7500 slots which means it would
              // take about 75 seconds to get back in sync at a 1 second update rate.
              // Mainly an issue for one of changes. e.g. Digitals.
              TrickleRefresh();

              }

            if (Ticks-m_dwStatusSendTicks>m_Cfg.m_dwStatusUpdateCycle)
              {
              m_dwStatusSendTicks=Ticks;
              SendUpdateStatus();
              }
            }
          }
        break;
      case WMU_WRITEALL2DEVICES:
        WriteAllSlots2Devices();
        break;

      case WMU_SAVESTATE:
        Save(*((CString*)Msg.lParam));
        delete (CString*)Msg.lParam;
        break;

      case WMU_LOADSTATE:
        Load(*((CString*)Msg.lParam));
        delete (CString*)Msg.lParam;
        break;

      default:
        ReportError("SlotMngr", 0, "Unknown Message %i", Msg.message);
      }
    //m_MsgLoopLock.Unlock();
    }

  KillTimer(NULL, 0);
  //dbgpln("----------------------MessageHandler Done");
  AfxEndThread(0);
  //dbgpln("----------------------MessageHandler Done - Will not see this");
  }

// -----------------------------------------------------------------------

UINT CSlotMngr::staticMessageHandler(LPVOID pMngr)
  {
  ((CSlotMngr*)pMngr)->MessageHandler();
  return 0;
  }

// -----------------------------------------------------------------------

void CSlotMngr:: DumpSlotInfo(LPCTSTR Where)
  {
  if (0)
    {
    dbgpln("DumpSlotInfo %s ===================", Where);
    for (int i=0; i<NSlots; i++)
      {
      CSlot &S=*m_Slots[i];
      dbgpln("  Slot %4i S:%4i D:%4i %s %s %s %s Err:%3i %-20s %-15.15s %-15.15s %s",
        i, S.m_lSlot, S.m_lDevice,
        S.m_bInUse?"Used":"    ",
        S.m_bConnected?"Conn":"    ",
        S.m_bRead?"R":" ",
        S.m_bWrite?"W":" ",
        S.m_State.m_nError,
        S.m_sTag, 
        S.m_sRange,
        S.m_sModifier,
        S.m_sConnects
        );
      }
    for (int i=0; i<NLinks; i++)
      {
      CLink &S=*m_Links[i];
      dbgpln("  Link %4i S:%4i %6s %s %s %s %s Err:%3i %-20s %s",
        i, S.m_lLink, "" ,
        S.m_bInUse?"Used":"    ", "    "," "," ",
        S.m_State.m_nError,
        S.m_sTag, S.m_sSimTag );
      }
    }
  }

// -----------------------------------------------------------------------

void CSlotMngr::SetVisibleItems(long What, bool ForceUpdate, long Count, long * pList)
  {
  PostThreadMessage(m_dwThreadId, WMU_SETVISIBLE, 
    (ForceUpdate?0x10000000:0)|
    (0x0f000000&(What<<24))|
    (0x00ffffff&Count),
    (LPARAM)pList);
  };

// -----------------------------------------------------------------------

void CSlotMngr::ApplySetVisible(long What, bool ForceUpdate, long Count, long * pList)
  {
  //dbgpln("    ApplySetVisible Top,Cnt:%4i,%2i", Start, Count);
  long n=0;
  switch (What)
    {
    case ViewLinks:
      {
      const int LinksSize = m_Links.GetSize();
      const int SlotsSize = m_Slots.GetSize();
      for (int i=0; i<SlotsSize; i++)
        m_Slots[i]->m_bVisible=false;
      for (i=0; i<LinksSize; i++)
        {
        m_Links[i]->m_bTemp=m_Links[i]->m_bVisible;
        m_Links[i]->m_bVisible=false;
        }
      for (i=0; i<Count && i<LinksSize; i++)
        {
        int j=pList[i];
        if (j<LinksSize)
          {
          m_Links[j]->m_bVisible=true;
          if (ForceUpdate || !m_Links[j]->m_bTemp)
            m_Links[j]->SendDisplayValue();
          }
        }
      break;
      }
    case ViewSlots:
      {
      const int LinksSize = m_Links.GetSize();
      const int SlotsSize = m_Slots.GetSize();
      for (int i=0; i<LinksSize; i++)
        m_Links[i]->m_bVisible=false;
      for (i=0; i<SlotsSize; i++)
        {
        m_Slots[i]->m_bTemp=m_Slots[i]->m_bVisible;
        m_Slots[i]->m_bVisible=false;
        }
      for (i=0; i<Count && i<SlotsSize; i++)
        {
        int j=pList[i];
        if (j<SlotsSize)
          {
          m_Slots[j]->m_bVisible=true;
          if (ForceUpdate || !m_Slots[j]->m_bTemp)
            m_Slots[j]->SendDisplayValue();
          }
        }
      break;
      }
    }
  }

// -----------------------------------------------------------------------

void CSlotMngr::SetHold(eConnSrcDst Src, long Item, bool On)
  {
  PostThreadMessage(m_dwThreadId, WMU_SETHOLD, (Src)| (On?0x10000000:0x00000000), Item);
  };

// -----------------------------------------------------------------------

void CSlotMngr::ApplyHold(eConnSrcDst Src, long Item, bool On)
  {
  switch (Src)
    {
    case eCSD_Slot:
      if (m_Slots[Item]->SetHold(On))
        m_dwTotalHolds+=(On?+1:-1);
      break;
    case eCSD_Link:
      if (m_Links[Item]->SetHold(On))
        m_dwTotalHolds+=(On?+1:-1);
      break;
    }
  };

//// -----------------------------------------------------------------------
//
//void CSlotMngr::SetFind(long What, LPCTSTR pStr)
//  {
//  CString * p = new CString (pStr);
//  PostThreadMessage(m_dwThreadId, WMU_FINDSTR, What, (long)p);
//  };
//
//// -----------------------------------------------------------------------
//
//void CSlotMngr::ApplyFind(long What, LPCTSTR pStr)
//  {
//  CString sBestTag;
//  int iBestInx=-1;
//  int iBestIsSlot=false;
//  const int LinksSize = m_Links.GetSize();
//  const int SlotsSize = m_Slots.GetSize();
//  int n=strlen(pStr);
//  if (n>0)
//    {
//    if (What!=ViewSlots)
//      {
//      for (int i=0; i<LinksSize; i++)
//        {
//        if (_strnicmp(m_Links[i]->m_sTag, pStr, n)==0)
//          {
//          if (iBestInx<0)
//            {
//            sBestTag = m_Links[i]->m_sTag;
//            iBestInx = i;
//            }
//          else if (_stricmp(m_Slots[i]->m_sTag, sBestTag)<0)
//            {
//            sBestTag = m_Links[i]->m_sTag;
//            iBestInx = i;
//            }
//          }
//        }
//      }
//    if (What!=ViewLinks)
//      {
//      for (int i=0; i<SlotsSize; i++)
//        {
//        if (_strnicmp(m_Slots[i]->m_sTag, pStr, n)==0)
//          {
//          if (iBestInx<0)
//            {
//            sBestTag = m_Slots[i]->m_sTag;
//            iBestInx = i;
//            iBestIsSlot = true;
//            }
//          else if (_stricmp(m_Slots[i]->m_sTag, sBestTag)<0)
//            {
//            sBestTag = m_Slots[i]->m_sTag;
//            iBestInx = i;
//            iBestIsSlot = true;
//            }
//          }
//        }
//      }
//
//    dbgpln("Find >>>> %-20s %s %5i %s", pStr, iBestIsSlot?"Slot":"Link", iBestInx, sBestTag.GetBuffer());
//    if (iBestInx>=0)
//      {
//      if (!iBestIsSlot)
//        {
//        //SetToLinks .............
//
//void CSlotMngr::ApplySetVisible(ViewLnks, true, Count, pList)
//
//          const int LinksSize = m_Links.GetSize();
//          const int SlotsSize = m_Slots.GetSize();
//          for (int i=0; i<SlotsSize; i++)
//            m_Slots[i]->m_bVisible=false;
//          for (i=0; i<LinksSize; i++)
//            {
//            m_Links[i]->m_bTemp=m_Links[i]->m_bVisible;
//            m_Links[i]->m_bVisible=false;
//            }
//      for (i=0; i<Count && i<LinksSize; i++)
//        {
//        int j=pList[i];
//        if (j<LinksSize)
//          {
//          m_Links[j]->m_bVisible=true;
//          if (ForceUpdate || !m_Links[j]->m_bTemp)
//            m_Links[j]->SendDisplayValue();
//          }
//        }
//      break;
//      }
//    case ViewSlots:
//      {
//      const int LinksSize = m_Links.GetSize();
//      const int SlotsSize = m_Slots.GetSize();
//      for (int i=0; i<LinksSize; i++)
//        m_Links[i]->m_bVisible=false;
//      for (i=0; i<SlotsSize; i++)
//        {
//        m_Slots[i]->m_bTemp=m_Slots[i]->m_bVisible;
//        m_Slots[i]->m_bVisible=false;
//        }
//      for (i=0; i<Count && i<SlotsSize; i++)
//        {
//        int j=pList[i];
//        if (j<SlotsSize)
//          {
//          m_Slots[j]->m_bVisible=true;
//          if (ForceUpdate || !m_Slots[j]->m_bTemp)
//            m_Slots[j]->SendDisplayValue();
//          }
//        }
//      break;
//      }
//    
//    }
//  }

// -----------------------------------------------------------------------

const struct {long Id; LPCSTR Str;} IOMLs[]=
  {
    { IOML_Link  , "Link"},
    { IOML_Slot  , "Slot"},
    //{ IOML_CdBlk , "CdBlk"}, // OTHE Code to be completed
    //{ IOML_Device, "Device"},
    { -1}
  };

const struct {long Id; LPCSTR Str; DWORD ChgFlag; } IOMSIs[]=
  {
    { IOMSI_Value    , "Value",     IOMChgFlag_Value    },  
    { IOMSI_HoldValue, "HoldValue", IOMChgFlag_HoldValue},
    { IOMSI_Hold     , "Hold",      IOMChgFlag_Hold     },
    { IOMSI_Type     , "Type",      IOMChgFlag_Type     },
    { IOMSI_Changes  , "Changes",   IOMChgFlag_Changes  },
    { IOMSI_Ignored  , "Ignored",   IOMChgFlag_Ignored  },
    { -1}       
  };

// -----------------------------------------------------------------------

bool CSlotMngr::FindTagHandle(LPCSTR Tag, DWORD &dwHandle)
  {
  CMsgLoopLock Lk("FindTagH");

  CString Tmp(Tag);
  LPSTR Tg=Tmp.GetBuffer(0);

  LPSTR ListStr=NULL;
  LPSTR Dot2ndLast=NULL, DotLast=NULL;
  LPSTR p=Tg;
  while ((p=strchr(p, '.'))!=NULL)
    {
    Dot2ndLast=DotLast;
    DotLast=p++;
    }

  long iList=-1;
  long iSubItem=-1;
  long iItem=-1;
  if (DotLast)
    {
    *(DotLast++)=0;
    for (long i=0; iSubItem<0 && IOMSIs[i].Id>=0; i++)
      if (_stricmp(DotLast, IOMSIs[i].Str)==0)
        iSubItem=IOMSIs[i].Id;
    }
  if (iSubItem<0)
    {
    ListStr=DotLast;
    iSubItem=IOMSI_Value; // default 
    }
  else
    {
    if (Dot2ndLast)
      *(Dot2ndLast++)=0;
    ListStr=Dot2ndLast;
    }

  if (ListStr)
    {
    for (long i=0; iList<0 && IOMLs[i].Id>=0; i++)
      if (_stricmp(ListStr, IOMLs[i].Str)==0)
        iList=IOMLs[i].Id;
    // Now Look for Atual Tag
    if (iList>=0 && iSubItem>=0)
      {
      bool GotIt=false;
      switch (iList)
        {
        case IOML_Slot:
          GotIt=m_SlotMap.Lookup(Tg, iItem)!=0;
          break;
        case IOML_Link:
          GotIt=m_LinkMap.Lookup(Tg, iItem)!=0;
          break;
        }

      if (GotIt)
        {
        dwHandle=MakeIOMHandle(iList, 0, iItem, iSubItem);
        return true;
        }
      }
    }

  return false;
  }

// -----------------------------------------------------------------------

VARTYPE CSlotMngr::GetTypeFromHandle(DWORD dwHandle)
  {
  CMsgLoopLock Lk("GetTypeFromH");
  VARTYPE vt=VT_NULL;
  switch (ExtractIOMSubItem(dwHandle))
    {
    case IOMSI_Hold:      return VT_BOOL;
    case IOMSI_Type:      return VT_I4;
    case IOMSI_Changes:   
    case IOMSI_Ignored:   return VT_I4;

    case IOMSI_Value:
    case IOMSI_HoldValue:
      {
      long iItem    = ExtractIOMItem(dwHandle);
      switch (ExtractIOMList(dwHandle))
        {
        case IOML_Slot:     return m_Slots[iItem]->Type();
        case IOML_Link:     return m_Links[iItem]->Type();
          //case IOML_CdBlk:    return m_Slots[iItem]->Type();
          //case IOML_Device:   return m_Slots[iItem]->Type();
        }
      }
    }

  return vt;
  }

// -----------------------------------------------------------------------

bool CSlotMngr::GetValueFromHandle(DWORD dwHandle, VARIANT &Value, bool ForceRead)
  {
  CMsgLoopLock Lk("GetValueFromH");
  long iItem=ExtractIOMItem(dwHandle);
  long iSubItem=ExtractIOMSubItem(dwHandle);
  switch (ExtractIOMList(dwHandle))
    {
    case IOML_Slot:     
      {
      CSlot & Slot=*m_Slots[iItem];
      if (!ForceRead)
        { 
        ASSERT(IOMSIs[iSubItem].Id==iSubItem);
        DWORD ChgFlag=IOMSIs[iSubItem].ChgFlag;
        if ((Slot.m_ChgFlags & ChgFlag)==0)
          return false;
        Slot.m_ChgFlags&=~ChgFlag;      
        }
      switch (ExtractIOMSubItem(dwHandle))
        {
        case IOMSI_Hold:      
          Value.vt=VT_BOOL;
          Value.boolVal=Slot.m_State.m_bHold;
          return true;
        case IOMSI_Type:      ;
          Value.vt=VT_I4;
          Value.lVal=Slot.m_vValue.vt;
          return true;
        case IOMSI_Changes:   
          Value.vt=VT_I4;
          Value.lVal=Slot.m_lValueChanges;
          return true;
        case IOMSI_Ignored:   
          Value.vt=VT_I4;
          Value.lVal=Slot.m_lValuesIgnored;
          return true;
        case IOMSI_Value:
          VariantCopy(&Value, &Slot.m_vValue);
          return true;
        case IOMSI_HoldValue:
          VariantCopy(&Value, &Slot.m_vValue);
          return true;
        }
      }
      break;
    case IOML_Link:     
      {
      CLink & Link=*m_Links[iItem];
      if (!ForceRead)
        { 
        ASSERT(IOMSIs[iSubItem].Id==iSubItem);
        DWORD ChgFlag=IOMSIs[iSubItem].ChgFlag;
        if ((Link.m_ChgFlags & ChgFlag)==0)
          return false;
        Link.m_ChgFlags&=~ChgFlag;      
        }
      switch (ExtractIOMSubItem(dwHandle))
        {
        case IOMSI_Hold:      
          Value.vt=VT_BOOL;
          Value.boolVal=Link.m_State.m_bHold;
          return true;
        case IOMSI_Type:      ;
          Value.vt=VT_I4;
          Value.lVal=Link.m_vValue.vt;
          return true;
        case IOMSI_Changes:   
          Value.vt=VT_I4;
          Value.lVal=Link.m_lValueChanges;
          return true;
        case IOMSI_Ignored:   
          Value.vt=VT_I4;
          Value.lVal=Link.m_lValuesIgnored;
          return true;
        case IOMSI_Value:
          VariantCopy(&Value, &Link.m_vValue);
          return true;
        case IOMSI_HoldValue:
          VariantCopy(&Value, &Link.m_vValue);
          return true;
        }
      }
    }

  return false;
  };

// -----------------------------------------------------------------------

bool CSlotMngr::SetValueFromHandle(eConnSrcDst Src, DWORD dwHandle, VARIANT Value)
  {
  CMsgLoopLock Lk("SetValueFromH");
  long iItem=ExtractIOMItem(dwHandle);
  switch (ExtractIOMList(dwHandle))
    {
    case IOML_Slot:     
      {
      CSlot & Slot=*m_Slots[iItem];
      switch (ExtractIOMSubItem(dwHandle))
        {
        case IOMSI_Hold:      
          VariantChangeType(&Value, NULL, 0, VT_BOOL);
          Slot.SetHold(Value.boolVal!=0);
          return true;
        case IOMSI_Value:
          {
          CFullValue FV(Value, OPC_QUALITY_GOOD);
          gs_SlotMngr.AppendChange(Src, -1, eCSD_Slot, iItem, GetTransactionID(), FV, NULL);
          return true;
          }
        case IOMSI_HoldValue:
          {
          CFullValue FV(Value, OPC_QUALITY_GOOD);
          gs_SlotMngr.AppendChange(Src, -1, eCSD_Slot, iItem, GetTransactionID(), FV, NULL, true);
          return true;
          }
        }
      }
      break;
    case IOML_Link:     
      {
      CLink & Link=*m_Links[iItem];
      switch (ExtractIOMSubItem(dwHandle))
        {
        case IOMSI_Hold:      
          VariantChangeType(&Value, NULL, 0, VT_BOOL);
          Link.SetHold(Value.boolVal!=1);
          return true;
        case IOMSI_Value:
          {
          CFullValue FV(Value, OPC_QUALITY_GOOD);
          gs_SlotMngr.AppendChange(Src, -1, eCSD_Link, iItem, GetTransactionID(), FV, NULL);
          return true;
          }
        case IOMSI_HoldValue:
          {
          CFullValue FV(Value, OPC_QUALITY_GOOD);
          gs_SlotMngr.AppendChange(Src, -1, eCSD_Link, iItem, GetTransactionID(), FV, NULL, true);
          return true;
          }
        }
      }
    }

  return false;
  };

// -----------------------------------------------------------------------

CSlotMngr gs_SlotMngr;

// -----------------------------------------------------------------------

long        CSlotMngr::getSlotCount() const               { return m_Slots.GetSize(); };
CSlot     * CSlotMngr::getSlot(long Index) const          { return m_Slots[Index]; };
void        CSlotMngr::putSlot(long Index, CSlot * pSlot) { m_Slots[Index] = pSlot; };
long        CSlotMngr::getLinkCount() const               { return m_Links.GetSize(); };
CLink     * CSlotMngr::getLink(long Index) const          { return m_Links[Index]; };
void        CSlotMngr::putLink(long Index, CLink * pLink) { m_Links[Index] = pLink; };
long        CSlotMngr::getDeviceCount() const                   { return m_Devices.GetSize(); };
CDevice   * CSlotMngr::getDevice(long Index) const              { return m_Devices[Index]; };
void        CSlotMngr::putDevice(long Index, CDevice * pDevice) { m_Devices[Index] = pDevice; };

bool        CSlotMngr::getScdRunning() const              { return m_bScdRunning; };
void        CSlotMngr::putScdRunning(bool Running)        { m_bScdRunning =  Running; };

// =======================================================================
//
//
//
// =======================================================================
