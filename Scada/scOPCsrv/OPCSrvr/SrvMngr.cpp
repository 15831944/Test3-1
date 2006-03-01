//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#include "scdver.h"
#if WITHOPC
#include "srvCallback.h"
#include "resource.h"
#include "SrvDlgs.h"
#include "SrvMngr.h"
#include "CommsSrvrBase.h"
#include "scd_wm.h"

//#include "optoff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define dbgOPCSrvr  0

//==========================================================================

// Also put this GUID into the OPCServer.rgs file
CLSID CLSID_OPCServer0 =
  { 0x65a0cb71, 0x6ab7, 0x475c, { 0x87, 0xb, 0xd2, 0x5d, 0xc8, 0xce, 0x14, 0x33 } };
const char* psCLSID_OPCServer0 = "65A0CB71-6AB7-475C-870B-D25DC8CE1433";
CLSID CLSID_OPCServer1 =
  { 0x65a0cb71, 0x6ab7, 0x475c, { 0x87, 0xb, 0xd2, 0x5d, 0xc8, 0xce, 0x14, 0x34 } };
const char* psCLSID_OPCServer1 = "65A0CB71-6AB7-475C-870B-D25DC8CE1434";

//==========================================================================

bool CScdOPCManager::sm_bInitOK = false;
CScdOPCManager* CScdOPCManager::sm_pTheMngr = NULL;

CScdOPCManager::CScdOPCManager(CScdOPCCallBack* pCB)
  {
  pTheCB = pCB;
  pTheOPCStatsDlg = NULL;
  bShowStatsOnConnect = 1;
  bDoLogNotes = 0;
  bForceOnStart = 0;
  bForceOnStop = 0;
  iForceCnt = 0;
  
  m_bEnabled=false;
  m_bAllowInvalidTags=false;
  }

//--------------------------------------------------------------------------

CScdOPCManager::~CScdOPCManager()
  {
  }

//--------------------------------------------------------------------------

bool CScdOPCManager::Init(LPCTSTR ExeFile, bool ResetReg, int Server/*=0*/)
  {
  ASSERT(sm_bInitOK==0 && sm_pTheMngr==NULL); //assume we can handle ONLY ONE OPC Server Callback!!!
  sm_bInitOK = false;
  HINSTANCE h = AfxGetApp()->m_hInstance;

  //kga:12/08/2001:the RegisterServer() changes key values in registry based on contents
  //               of opcserver.rgs in syscad resources which thus overwrites any settings
  //               entered in DCOMcnfg.exe.

  HKEY hKey = NULL;
  char Key[256];
  if (Server==0)
    sprintf(Key, "SOFTWARE\\Classes\\AppID\\{%s}", psCLSID_OPCServer0);
  else
    sprintf(Key, "SOFTWARE\\Classes\\AppID\\{%s}", psCLSID_OPCServer1);
  bool OPC_APPID_Reg = (RegOpenKeyEx(HKEY_LOCAL_MACHINE, Key, 0, KEY_QUERY_VALUE, &hKey)==ERROR_SUCCESS);
  if (hKey)
    RegCloseKey(hKey);

  if (Server==0)
    sprintf(Key, "SOFTWARE\\Classes\\CLSID\\{%s}", psCLSID_OPCServer0);
  else
    sprintf(Key, "SOFTWARE\\Classes\\CLSID\\{%s}", psCLSID_OPCServer1);
  bool OPC_CLSID_Reg = (RegOpenKeyEx(HKEY_LOCAL_MACHINE, Key, 0, KEY_QUERY_VALUE, &hKey)==ERROR_SUCCESS);
  if (hKey)
    RegCloseKey(hKey);

  if (Server==0)
    {
    if (!StartFSServer(h, &CLSID_OPCServer0, !ResetReg))
      {
      AfxMessageBox(_T("SysCAD OPC Server Not Started"));
      return false;
      }
    }
  else
    {
    if (!StartFSServer(h, &CLSID_OPCServer1, !ResetReg))
      {
      AfxMessageBox(_T("SysCAD OPC Server Not Started"));
      return false;
      }
    }

  if (ResetReg || !OPC_APPID_Reg || !OPC_CLSID_Reg)
    {
    // OPC Registration
    HRESULT hr1 = RegisterServer(ExeFile);
    if ( FAILED(hr1) )
      {
      Strng E;
      E.GetSystemErrorStr(GetLastError());
      AfxMessageBox(_T("OPC RegisterServer Failed"));
      AfxMessageBox(E());
      return false;
      }
    }

  /*if (pValueData)
    {//restore key values...
    hKey = NULL;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, Key, 0, KEY_QUERY_VALUE|KEY_SET_VALUE, &hKey)==ERROR_SUCCESS)
      {
      DWORD index = 0; 
      while (index<cValues)
        {
        if (strlen(pValueData[index].ValueName)>0 && RegSetValueEx(hKey, pValueData[index].ValueName, NULL, pValueData[index].DataTyp, (LPBYTE)&(pValueData[index].Data)[0], pValueData[index].DataLen)!=ERROR_SUCCESS)
          {
          AfxMessageBox("Error reseting SysCAD OPC registry value");
          }
        index++;
        }
      }
    delete []pValueData;
    if (hKey)
      RegCloseKey(hKey);
    }*/

  if (!ResetReg)
    {
    CScdOPCCallBack* pOPCCallback = new CScdOPCCallBack(/*this, &m_Plc*/);
    SetCallbackObject(pOPCCallback);
    sm_pTheMngr = new CScdOPCManager(pOPCCallback);
    }
  sm_bInitOK = true;
  return sm_bInitOK;
  }

//--------------------------------------------------------------------------

bool CScdOPCManager::Stop()
  {
  if (sm_pTheMngr)
    sm_pTheMngr->CloseStatusWnd();
  bool ClientsConnected = (FSServerInUse()!=0);
  if (ClientsConnected)
    LogWarning("OPC", 0, "OPC Clients still connected when closing SysCAD OPC Server!");
  StopFSServer(); //stop the OPC server regardless of client connections
  delete sm_pTheMngr;
  sm_pTheMngr = NULL;
  sm_bInitOK = false;
  if (ClientsConnected)
    Sleep(50);
  return true;
  }

//--------------------------------------------------------------------------

void CScdOPCManager::LoadOrSaveOptions(CProfINIFile & PF, bool Loading)
  {
  if (Loading)
    {
    bShowStatsOnConnect = (PF.RdLong("OPCManager", "ShowStatsOnConnect", bShowStatsOnConnect) != 0);
    bDoLogNotes = (PF.RdLong("OPCManager", "DoLogNotes", bDoLogNotes) != 0);
    bForceOnStart = (PF.RdLong("OPCManager", "ForceOnStart", bForceOnStart) != 0);
    bForceOnStop = (PF.RdLong("OPCManager", "ForceOnStop", bForceOnStop) != 0);
    iForceCnt = PF.RdLong("OPCManager", "ForceCnt", iForceCnt);
    }
  else
    {
    PF.WrLong("OPCManager", "ShowStatsOnConnect", bShowStatsOnConnect);
    PF.WrLong("OPCManager", "DoLogNotes", bDoLogNotes);
    PF.WrLong("OPCManager", "ForceOnStart", bForceOnStart);
    PF.WrLong("OPCManager", "ForceOnStop", bForceOnStop);
    PF.WrLong("OPCManager", "ForceCnt", iForceCnt);
    }
  }

//--------------------------------------------------------------------------

bool CScdOPCManager::OpenStatusWnd()
  {
  if (pTheOPCStatsDlg==NULL)
    pTheOPCStatsDlg = new COPCSrvrStatsDlg(this);
  return true;
  }
  
//--------------------------------------------------------------------------

bool CScdOPCManager::CloseStatusWnd()
  {
  if (pTheOPCStatsDlg)
    {
    pTheOPCStatsDlg->DestroyWindow();
    pTheOPCStatsDlg = NULL;
    }
  return true;
  }

//--------------------------------------------------------------------------

void CScdOPCManager::UpdateStatusWnd(WPARAM wParam/*=0*/, LPARAM lParam/*=0*/)
  {
  if (pTheOPCStatsDlg)
    {
    if (wParam & SUB_UPDATE_SENDMSG)
      pTheOPCStatsDlg->SendMessage(WMU_OPCSRVRUPDATESTATS, wParam, lParam);
    else
      pTheOPCStatsDlg->PostMessage(WMU_OPCSRVRUPDATESTATS, wParam, lParam);
    }
  }

//==========================================================================

bool CScdOPCManager::SetEnable(bool On)
  {
  m_bEnabled=On;
  SetOPCServerAllowed(On);
  pTheCB->SetEnable(On);
  if (On)
    {
    }
  else
    sm_pTheMngr->CloseStatusWnd();
  return m_bEnabled;
  }

//==========================================================================
#endif
