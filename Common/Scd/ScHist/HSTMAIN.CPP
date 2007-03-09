//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __HSTMAIN_CPP
#include "hstmain.h"
#if ForSysCAD
#include "errorlog.h"
#include "dlgbusy.h"
#include "scd_wm.h"
#include "hstdlgs.h"
#endif
#include "hstqyinf.h"
#include "hstmsg.h"
//#include "optoff.h"

#define  dbgHistorian  WITHDEBUG

#if dbgHistorian
#include "dbgmngr.h"
static CDbgMngr dbgOpenClose("Historian", "OpenClose");
static CDbgMngr dbgStatistics("Historian", "Statistics");
static CDbgMngr dbgTime("Historian", "Time");
static CDbgMngr dbgThreads("Historian", "Threads");
static CDbgMngr dbgQueryPriority("Historian", "QueryPriority");
static CDbgMngr dbgWriteData("Historian", "WriteData");
static CDbgMngr dbgExecute("Historian", "Execute");
#endif

//===========================================================================

IMPLEMENT_MEMSTATS(CQThreadInfo)
IMPLEMENT_MEMSTATS(CHistorian)

//===========================================================================

CHistorian* pHistory = NULL; //global pointer to the historian

//===========================================================================

UINT QueryHistoryThread( LPVOID pParam )
  {
  CQThreadInfo* pInfo = (CQThreadInfo*)pParam;
  SCDTRY_THREAD

    ASSERT(pInfo != NULL);
    pInfo->pHst->ServiceHistoryQueries(pInfo->ThreadNo);

  SCDCATCH_THREAD("QueryHistoryThread")

  return 0; // thread completed successfully
  }

//===========================================================================

DWORD CHistorian::dwGranularity  = 65536;
DWORD CHistorian::dwDoubleGran   = 131072;

//---------------------------------------------------------------------------

CHistorian::CHistorian() : SlotMap(), SlotIDMap()
  {
  #if dbgHistorian
  if (dbgOpenClose())
    dbgpln("CHistorian::CHistorian");
  #endif
  Slots = NULL;
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  dwGranularity = si.dwAllocationGranularity;
  dwDoubleGran = dwGranularity + dwGranularity;
  InitializeCriticalSection(&QuerySection);
  Initialise();
  }

//---------------------------------------------------------------------------

CHistorian::~CHistorian()
  {
  #if dbgHistorian
  if (dbgOpenClose())
    dbgpln("CHistorian::~CHistorian");
  #endif
  ClearDataLists();
  if (gs_pQueryInfo)
    {
    gs_pQueryInfo->DestroyWindow();
    delete gs_pQueryInfo;
    gs_pQueryInfo = NULL;
    }
  ASSERT(pNxtBlkFile==NULL);
  DeleteCriticalSection(&QuerySection);
  }

//---------------------------------------------------------------------------

void CHistorian::Initialise()
  {
  #if dbgHistorian
  if (dbgOpenClose())
    dbgpln("CHistorian::Initialise");
  #endif
  bDoSaveAll = false;
  lScanNo = 0;
  ClearDataLists();
  pCurEvBlk = NULL;
  pNxtBlkFile = NULL;
  bDoneConfig = 0;
  bOpen = 0;
  bShowQueryInfo = 1;
  m_bWasPaused=false;
  m_StopGap=CTimeValue::MinResolution*0.1;
  m_MidTimeFactor=99.0/100.0;// CNM 59.0/60.0;
  }

//---------------------------------------------------------------------------

void CHistorian::ClearDataLists()
  {
  if (Slots)
    {
    for (long i=0; i<Data.lSlotsUsed; i++)
      delete Slots[i];
    delete []Slots;
    Slots = NULL;
    }
  lSlotsAlloc = 0;
  }

//---------------------------------------------------------------------------

void CHistorian::Configure( char* Path,
                            char* DataPath,
                            char* CatPath,
                            char* Name,
                            long LicMaxHistSizeAllowed,
                            UINT LicMaxHistFilesAllowed,
                            UINT FilesMin,
                            UINT FilesMax,
                            long BlkWaitTime, //number of seconds after which an unused block must be closed
                            long ForceCnt,    //at worst value will be saved every (ForceCnt * 2) iterations
                            flag ShowQueryInfo,
                            flag FixedDataExtn,
                            flag UseTol,
                            double ATol,
                            double RTol,
                            double StopGap,
                            double MidTimeFactor)
  {
  sPath = Path;
  if (sPath.GetLength()>0 && sPath[sPath.GetLength()-1] != '\\')
    sPath += '\\';
  sDataPath = DataPath;
  if (sDataPath.GetLength()>0 && sDataPath[sDataPath.GetLength()-1] != '\\')
    sDataPath += '\\';
  sCatPath = CatPath;
  if (sCatPath.GetLength()>0 && sCatPath[sCatPath.GetLength()-1] != '\\')
    sCatPath += '\\';

  sName = Name;
  if (sName.Find('.')>=0)
    sName = sName.Left(sName.Find('.'));
  //if (sName.GetLength()>8)
  //  sName = sName.Left(8);

  CCatItem::m_bFixedDataExtn=FixedDataExtn!=0;
  CString CatName = sCatPath;
  CatName += sName;
  CatName += ".cat";
  Cat.SetFilename((char*)(const char*)CatName);

  iFilesMax = FilesMax;
  if (iFilesMax>LicMaxHistFilesAllowed)
    {
    iFilesMax = LicMaxHistFilesAllowed;
    LogWarning("History", 0, "License does not allow more than %d data files", LicMaxHistFilesAllowed);
    }
  iFilesMin = Min(FilesMin, iFilesMax);
  iFilesMax = Max(iFilesMax, (UINT)1);
  iFilesMin = Max(iFilesMin, (UINT)1);

  lBlkWaitTime = BlkWaitTime;
  lForceCnt = ForceCnt;
  bShowQueryInfo = ShowQueryInfo;
  bUseTolerance = UseTol;
  dRecordATol = min(0.1, ATol);
  dRecordRTol = min(0.1, RTol);
  //m_StopGap = Max(0.0, StopGap);
  m_MidTimeFactor = Range(0.001, MidTimeFactor, 0.999);
  bDoneConfig = 1;
  bOpen = 0;
  m_bWasPaused=false;
  }

//---------------------------------------------------------------------------

flag CHistorian::Create()
  {
  #if dbgHistorian
  if (dbgOpenClose())
    dbgpln("CHistorian::Create");
  #endif

  Data.lReqdBlkSize = Max(DefaultBlkSize, Data.lReqdBlkSize);
  Data.iMaxDeltaTimeCnt = DefaultTimeCnt;
  Data.lMaxTimeSliceLen = DefaultSliceSize;
  if (strlen(Data.sScenName)==0)
    strncpy(Data.sScenName, "histdat", sizeof(Data.sScenName));

  Data.lSlotsUsed = 0;
  Data.iCurCatNo = 0;
  Data.lNxtSlotID = 0;
  Data.bDidClose = 1;
  Data.iVerNo = HstVerNo;

  Cat.Data.iCatItemCnt = 0;
  Cat.Data.iVerNo = HstVerNo;

  flag OK = Cat.Save();
  if (OK)
    OK = Save();
  return OK;
  }

//---------------------------------------------------------------------------

byte CHistorian::Load(bool ShowBusyDlg)
  {
#if ForSysCAD
  if (ShowBusyDlg)
    CDlgBusy::SetLine(3, "Load : Header");
#endif
  SlotMap.RemoveAll();
  SlotMap.InitHashTable(53);
  SlotIDMap.RemoveAll();
  SlotIDMap.InitHashTable(53);
  CString Filename = sPath;
  Filename += sName;
  Filename += ".hed";
  #if dbgHistorian
  if (dbgOpenClose())
    dbgpln("CHistorian::Load %s", (const char*)Filename);
  #endif
  WIN32_FIND_DATA fd;
  HANDLE H;
  if ((H=FindFirstFile((const char*)Filename, &fd))!=INVALID_HANDLE_VALUE)
    {
    FindClose(H);
    if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      {
      LogError("History", 0/*LF_Exclamation*/, "Cannot open read-only historian '%s'", (const char*)Filename);
      return HST_ERR_OPENHED;
      }
    DWORD dwFlags = CFile::shareCompat|CFile::modeRead|CFile::typeBinary;
    CFile F;
    if (F.Open((const char*)Filename, dwFlags))
      {
      //load historian header...
      TRY
        {
        F.Read((void*)&Data, sizeof(HD_Hist));
        if (Data.iVerNo != HstVerNo && Data.iVerNo==3 && HstVerNo==4)
          {
          Data.iVerNo = HstVerNo;
          LogNote("History", 0, "Historian auto upgrade version number");
          }
        if (Data.iVerNo != HstVerNo)
          {
          LogError("History", 0, "Historian version number mismatch");
          return HST_ERR_OPENHED;
          }
        #if dbgHistorian
        if (dbgOpenClose())
          Data.dbgDump("Load");
        #endif
        if (Data.lSlotsUsed>0)
          {
          lSlotsAlloc = Data.lSlotsUsed;
          //ASSERT(Slots==NULL);
          Slots = new pCSlot[lSlotsAlloc];
          EvSlotNos.SetSize(lSlotsAlloc);
          UINT PrimeNo = 23;
#if ForSysCAD
          PrimeNo = FindNextPrimeNumber((UINT)(Data.lSlotsUsed*1.18));
#endif
          SlotMap.InitHashTable(PrimeNo);
          SlotIDMap.InitHashTable(PrimeNo);
          for (long i=0; i<Data.lSlotsUsed; i++)
            {
            Slots[i] = new CSlot();
            Slots[i]->Load(F, i);
            SlotMap.SetAt(Slots[i]->Data.sTag, i);
            SlotIDMap.SetAt(Slots[i]->Data.lSlotID, i);
#if ForSysCAD
            if (ShowBusyDlg && (i%20)==0)
              CDlgBusy::SetLine(3, "Load : Slot %d of %d", i, Data.lSlotsUsed);
#endif
            #if dbgHistorian
            if (dbgOpenClose())
              Slots[i]->dbgDump("Load");
            #endif
            }
#if ForSysCAD
          if (ShowBusyDlg)
            CDlgBusy::SetLine(3, "Load : Slot %d of %d done", Data.lSlotsUsed, Data.lSlotsUsed);
#endif
          }
        F.Close();
        }
      CATCH (CException, e)
        {
        #if dbgHistorian
        if (dbgOpenClose())
          dbgpln("  Load-Exception");
        #endif
        return HST_ERR_OPENHED;
        }
      END_CATCH
      //TODO : check which cataloged EvBlks are directly available
      FixFilterList();
      return 0;
      }
    }
  else
    return HST_ERR_NOTEXISTS_HED;
  return HST_ERR_OPENHED;
  }

//---------------------------------------------------------------------------

flag CHistorian::Save(long SlotNo/*=-1*/)
  {
  #if dbgHistorian
  if (dbgOpenClose())
    dbgpln("CHistorian::Save");
  #endif
  CString Filename = sPath;
  Filename += sName;
  Filename += ".hed";
  DWORD dwFlags = CFile::shareCompat|CFile::modeWrite|CFile::typeBinary;
  CFileStatus State;
  if (CFile::GetStatus((const char*)Filename, State)==0)
    dwFlags |= CFile::modeCreate;
  CFile F;
  if (F.Open((const char*)Filename, dwFlags))
    {
    TRY
      {
      F.Write((void*)&Data, sizeof(HD_Hist));
      if (SlotNo==-1)
        {
        for (long i=0; i<Data.lSlotsUsed; i++)
          Slots[i]->Save(F, i);
        }
      else if (SlotNo>=0)
        Slots[SlotNo]->Save(F, SlotNo);

      F.Close();
      }
    CATCH (CException, e)
      {
      #if dbgHistorian
      if (dbgOpenClose())
        dbgpln("  Save-Exception");
      #endif
      return false;
      }
    END_CATCH
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------
/*
flag CHistorian::ChangeScenario()
  {
  dTime = 0.0;
  }
*/
//---------------------------------------------------------------------------

void CHistorian::CloseEvBlk()
  {
  if (pCurEvBlk)
    {
    pCurEvBlk->MarkAsFinished();
    pCurEvBlk->Close();
    delete pCurEvBlk;
    }
  pCurEvBlk = NULL;
  }

//---------------------------------------------------------------------------

byte CHistorian::AddEvBlk()
  {
  Lock();
  if (pCurEvBlk)
    CloseEvBlk();
  Data.iCurCatNo++;
  Cat.AddCatItem(Data.iCurCatNo);
  byte RetCode = OpenEvBlk();
  if (RetCode!=0)
    {//were not able to open new event block, why not ???  disk space ???
    Cat.DeleteLastCatItem();
    Data.iCurCatNo--;
    bSwitchedOff = true;
    LogError("History", 0, "Unable to open next historian file");
    LogError("History", 0, "Historian not recording, insufficient disk space");
    }
  else
    {
    Cat.Save(Data.iCurCatNo-1);
    Save(-2);
    bDoSaveAll = true;
    }
  Release();
  return RetCode;
  }

//---------------------------------------------------------------------------

byte CHistorian::OpenEvBlk()
  {
  ASSERT(pCurEvBlk==NULL);
  pCurEvBlk = new CEvBlk();

  if (pNxtBlkFile==NULL)
    {//current block is not available, create it NOW...
    CString sFilename;
    CCatItem::MakeFilename(Data.iCurCatNo, sFilename);
    pCEvBlkFileInfo pI = new CEvBlkFileInfo();
    pI->sFilename = sFilename;
    if (CreateEventBlk(pI))
      pNxtBlkFile = pI;
    else
      {
      delete pCurEvBlk;
      pCurEvBlk = NULL;
      return HST_ERR_OPENEVBLK_CREATE;
      }
    }

  if (!pCurEvBlk->Open(Data.iCurCatNo))
    {//ERROR, close event block...
    if (pCurEvBlk->pInfo)
      {
      if (!UnmapViewOfFile(pCurEvBlk->pInfo->pBlkMap))
        LogError("History", 0, "Unmap file %s", FindWinError(GetLastError()));
      if (!CloseHandle(pCurEvBlk->pInfo->hBlkFileMap))
        LogError("History", 0, "Close Handle L %s", FindWinError(GetLastError()));
      if (!CloseHandle(pCurEvBlk->pInfo->hBlkFile))
        LogError("History", 0, "Close Handle M %s", FindWinError(GetLastError()));
      delete pCurEvBlk->pInfo;
      pCurEvBlk->pInfo = NULL;
      }
    delete pCurEvBlk;
    pCurEvBlk = NULL;
    if (pNxtBlkFile)
      {
      if (!UnmapViewOfFile(pNxtBlkFile->pBlkMap))
        LogError("History", 0, "Unmap file %s", FindWinError(GetLastError()));
      if (!CloseHandle(pNxtBlkFile->hBlkFileMap))
        LogError("History", 0, "Close Handle L %s", FindWinError(GetLastError()));
      if (!CloseHandle(pNxtBlkFile->hBlkFile))
        LogError("History", 0, "Close Handle M %s", FindWinError(GetLastError()));
      delete pNxtBlkFile;
      pNxtBlkFile = NULL;
      }
    return HST_ERR_OPENEVBLK;
    }

  return 0;
  }

//---------------------------------------------------------------------------
#define TryHistFix 0

static CWinThread* pQueryHistoryThread[MaxQueryThreads];

byte CHistorian::Open(char* ScenarioName, long ReqdBlkSize, long EstimatedMaxSlots, long LicMaxHistSizeAllowed, bool ShowBusyDlg/*=false*/)
  {
  ASSERT(bDoneConfig);

  //Cat.Data.iNextCatNo = Max(Cat.Data.iNextCatNo, (WORD)1);

  //load historian...
  byte b = Load(ShowBusyDlg);
  if (b!=0)
    return b;

  #if TryHistFix
  //load catalog...
  if (ShowBusyDlg)
    CDlgBusy::SetLine(3, "Load : Catalog");
  if (!Cat.Load())
    return HST_ERR_OPENCAT;
  if (ShowBusyDlg)
    CDlgBusy::SetLine(3, "Load : Catalog %d of %d done", Cat.Data.iCatItemCnt, Cat.Data.iCatItemCnt);
  #if dbgHistorian
  if (dbgOpenClose())
    Cat.dbgDump("Load");
  #endif
  #endif

  const bool IsPermHist = (iFilesMin!=iFilesMax);
  #if TryHistFix
  WORD LastCatNo = FindLastCatNo();
  if (Data.iCurCatNo != LastCatNo)
    {
    //KGA *** June 97 *** NB NB *** I do not like this, I'm sure this is the cause of some historian problems!!!

    //try repair historian, config, catalog etc
    LogWarning("History", 0, "Attempting historian file recovery...");
    LogWarning("History", 0, "Catalog number mismatch: File:%d Expected:%d", LastCatNo, Data.iCurCatNo);
    Data.iCurCatNo = Max(Data.iCurCatNo, LastCatNo);
    //save fixes...
    //Data.bDidClose = 1;
    //Save(-2);
    //Data.bDidClose = 0;
    }

  if (!Data.bDidClose)
    {
    Recover(0, false); //try repair historian, config, catalog etc
    //if (!Load())
    //  return HST_ERR_OPENHED;
    ASSERT(Data.bDidClose);
    }
  Data.bDidClose = 0;
  #else
  bool DoDeleteExtra = false;
  WORD LastCatNo = FindLastCatNo();
  b = Cat.Load(true);
  if (b!=0)
    return b;
  if (Data.iCurCatNo != LastCatNo || Max(Data.iCurCatNo, LastCatNo)>Cat.Data.iCatItemCnt)
    {
    DoDeleteExtra = true;
    //try repair historian, config, catalog etc
    LogNote("History", 0, "Attempting historian file recovery...");
    if (IsPermHist)
      {
      LogWarning("History", 0, "Catalog number mismatch: File:%d Catalog:%d Expected:%d", LastCatNo, Cat.Data.iCatItemCnt, Data.iCurCatNo);
      Data.iCurCatNo = Max(Data.iCurCatNo, LastCatNo); //change catalog number to last file
      if (Data.iCurCatNo>Cat.Data.iCatItemCnt)
        {
        if (Cat.Data.iCatItemCnt<1)
          Recover(0, false);
        else
          Recover(Cat.Data.iCatItemCnt, false);
        }
      }
    else
      {
      Data.iCurCatNo = Min(Data.iCurCatNo, Cat.Data.iCatItemCnt);//change catalog number
      Recover(Data.iCurCatNo, false);
      }
    //save fixes...
    //Data.bDidClose = 1;
    //Save(-2);
    //Data.bDidClose = 0;
    }

  if (!Data.bDidClose)
    {
    DoDeleteExtra = true;
    if (IsPermHist)
      LogWarning("History", 0, "Attempting historian file recovery...");
    Recover(Cat.Data.iCatItemCnt, false); //try repair historian, config, catalog etc
    ASSERT(Data.bDidClose);
    }
  Data.bDidClose = 0;
  if (DoDeleteExtra)
    DeleteExtra();
  #endif

  // Default Values
  if (ReqdBlkSize>0)
    Data.lReqdBlkSize = ReqdBlkSize;
  if (EstimatedMaxSlots>0)
    Data.lMaxTimeSliceLen = (EstimatedMaxSlots*5); // cnm Estimate +/- 5 bytes per slot
  if (ScenarioName && strlen(ScenarioName)>0)
    strncpy(Data.sScenName, ScenarioName, sizeof(Data.sScenName));
  Data.sScenName[sizeof(Data.sScenName)-1] = 0;
  Data.lReqdBlkSize = Max(Data.lReqdBlkSize, DefaultBlkSize);
  if (Data.lReqdBlkSize>LicMaxHistSizeAllowed*1024L)
    {
    Data.lReqdBlkSize = LicMaxHistSizeAllowed*1024L;
    LogWarning("History", 0, "License does not allow data files larger than %d kb", LicMaxHistSizeAllowed);
    }
  Data.lMaxTimeSliceLen = Max(Data.lMaxTimeSliceLen, DefaultSliceSize); //minimum 4K

  #if !TryHistFix
  //load catalog...
#if ForSysCAD
  if (ShowBusyDlg)
    CDlgBusy::SetLine(3, "Load : Catalog");
#endif
  b = Cat.Load();
  if (b!=0)
    return b;
#if ForSysCAD
  if (ShowBusyDlg)
    CDlgBusy::SetLine(3, "Load : Catalog %d of %d done", Cat.Data.iCatItemCnt, Cat.Data.iCatItemCnt);
#endif
  #if dbgHistorian
  if (dbgOpenClose())
    Cat.dbgDump("Load");
  #endif

  //check catalog match with event blocks...
  //todo...
  WORD xLastCatNo = FindLastCatNo();
  /*if (???Data.iCurCatNo != LastCatNo)
    {
    if (Cat.Data.iCatItemCnt<1)
      Recover(0, false);
    else
      Recover(Cat.Data.iNextCatNo, false);
    DeleteExtra();
    }*/
  #endif

  //load current event block...
  if (Data.iCurCatNo==0)
    b = AddEvBlk(); //new
  else
    b = OpenEvBlk(); //old

  if (b==0)
    {
#if ForSysCAD
    EO_Register(pExecName_Historian, EOWrite_Thread | EORead_Thread | EOExec_Thread,/*Pri*/THREAD_PRIORITY_NORMAL,/*Stack*/50000, /*Hash*/ 101);
#endif

    //open query services...
    pQueryList = NULL;
    bQueryClose = 0;
    for (byte i=0; i<MaxQueryThreads; i++)
      {
      hQuerySignal[i] = CreateSemaphore((LPSECURITY_ATTRIBUTES)NULL, 0, 0x7fffffff, NULL);
      bThreadIsBusy[i] = false;
      QThreadInfo[i].ThreadNo = i;
      QThreadInfo[i].pHst = this;
      QThreadInfo[i].pQuery = NULL;
      pQueryHistoryThread[i] = AfxBeginThread(QueryHistoryThread, (LPVOID)&QThreadInfo[i], THREAD_PRIORITY_NORMAL, 65536/*+PkDataListLength*4*/);
      pQueryHistoryThread[i]->m_bAutoDelete=FALSE;
      SetThreadName(pQueryHistoryThread[i]->m_nThreadID, "HistorianQuery");
      }

    }
  else
    {
    ClearDataLists();
    }

  if (gs_pQueryInfo==NULL && bShowQueryInfo)
    gs_pQueryInfo = new CQueryInfo(AfxGetMainWnd());

  if (b==0)
    bOpen = 1;
  return b;
  }

//---------------------------------------------------------------------------

flag CHistorian::Restart(char* ScenarioName)
  {
  dTime =0.0;
  dPrevTime =0.0;
  dMidTime =0.0;
  dTimeRunStarted =0.0;
  if (Load(false)!=0)
    return false;
  if (Cat.Load()!=0)
    return false;
  Data.iCurCatNo = 0;
  Cat.Data.iCatItemCnt = 0;
  Cat.Data.iVerNo = HstVerNo;

  Cat.Save();
  Data.bDidClose = 1;
  Save();
  m_bWasPaused=false;
  return true;
  }

//---------------------------------------------------------------------------

void CHistorian::CancelQueries()
  {
  EnterCriticalSection(&QuerySection);
  pCQueryHist pQ = pQueryList;
  pCQueryHist pPrevQ = NULL;
  pCQueryHist pDeleteQ = NULL;
  while (pQ)
    {
    if (pQ->bBusyServicing)
      pQ->bCancel = 1;
    else
      {
      if (pPrevQ)
        pPrevQ->pNext = pQ->pNext;
      else
        pQueryList = pQ->pNext;
      pDeleteQ = pQ;
      }
    if (pDeleteQ==NULL)
      pPrevQ = pQ;
    pQ = pQ->pNext;
    if (pDeleteQ)
      delete pDeleteQ;
    }
  LeaveCriticalSection(&QuerySection);
  }

//---------------------------------------------------------------------------

void CHistorian::CancelAllQueries()
  {//cancel all queries...
  while (pQueryList)
    {
    CancelQueries();
    if (pQueryList)
      Sleep(100); //give query threads time to cancel
    }

  if (gs_pQueryInfo)
    {
    gs_pQueryInfo->DestroyWindow();
    delete gs_pQueryInfo;
    gs_pQueryInfo = NULL;
    }
  }

//---------------------------------------------------------------------------

flag CHistorian::Close()
  {
  CancelAllQueries();

  Cat.Save();
  Data.bDidClose = 1;
  Save();

  //close current event block...
  if (pCurEvBlk)
    {
    pCurEvBlk->Close();
    delete pCurEvBlk;
    pCurEvBlk = NULL;
    }

  //close down slots...
  ClearDataLists();

  //close query services...
  bQueryClose = 1;
  for (int i=0; i<MaxQueryThreads; i++)
    ReleaseSemaphore(hQuerySignal[i], 1, NULL);
  //have released all of them, now wait for them to finish...
  for (i=0; i<MaxQueryThreads; i++)
    {
    WaitForSingleObject(pQueryHistoryThread[i]->m_hThread, INFINITE);
    CloseHandle(hQuerySignal[i]);
    delete pQueryHistoryThread[i];
    pQueryHistoryThread[i] = NULL;
    }

  if (pNxtBlkFile)
    {
    if (!UnmapViewOfFile(pNxtBlkFile->pBlkMap))
      LogError("History", 0, "Unmap file C %s", FindWinError(GetLastError()));
    if (!CloseHandle(pNxtBlkFile->hBlkFileMap))
      LogError("History", 0, "Close Handle E %s", FindWinError(GetLastError()));
    if (!CloseHandle(pNxtBlkFile->hBlkFile))
      LogError("History", 0, "Close Handle F %s", FindWinError(GetLastError()));
    delete pNxtBlkFile;
    pNxtBlkFile = NULL;
    }

#if ForSysCAD
  EO_DeRegister();
#endif

  bOpen = 0;
  return true;
  }

//---------------------------------------------------------------------------

flag CHistorian::RemoveSlot(long SNo)
  {
  ASSERT(SNo<Data.lSlotsUsed);
  ASSERT(FALSE); //todo
  return true;
  }

//---------------------------------------------------------------------------

long CHistorian::AddSlot(DWORD SlotID)
  {
  long SNo = Data.lSlotsUsed++;
  if (SNo >= lSlotsAlloc)
    {//increase slot array...
    lSlotsAlloc += 32;
    EvSlotNos.SetSize(lSlotsAlloc);
    CSlot ** pNewSlots = new pCSlot[lSlotsAlloc];
    if (Slots)
      {
      for (long i=0; i<SNo; i++)
        pNewSlots[i] = Slots[i];
      delete []Slots;
      }
    Slots = pNewSlots;
    Data.lMaxTimeSliceLen = Max(Data.lMaxTimeSliceLen, 5L * Data.lSlotsUsed);
    }
  Slots[SNo] = new CSlot();
  Slots[SNo]->Configure(SlotID, SNo);
  return SNo;
  }

//---------------------------------------------------------------------------

long CHistorian::CfgSlot(pSlotCfgInfo pSCI)
  {//used to add or change a slot
  long SNo = GetSlot(pSCI->mTag);
  flag NewSlot = false;
  if (SNo<0)
    {
    SNo = AddSlot(Data.lNxtSlotID++);
    NewSlot = true;
    }

  flag HasFilter = Slots[SNo]->Data.bUseFilter;
  Slots[SNo]->Cfg(pSCI, NewSlot);
  if (HasFilter != Slots[SNo]->Data.bUseFilter)
    FixFilterList(); //filter added or removed
  SlotMap.SetAt(Slots[SNo]->Data.sTag, SNo);
  SlotIDMap.SetAt(Slots[SNo]->Data.lSlotID, SNo);
  return SNo;
  }

//---------------------------------------------------------------------------

CSlot* CHistorian::GetSlot(DWORD ID)
  {
  long Index;
  if (SlotIDMap.Lookup(ID, Index))
    return Slots[Index];
  return NULL;
  /*long SNo = 0;
  while (SNo<Data.lSlotsUsed)
    {
    if (Slots[SNo]->ID() == ID)
      return Slots[SNo];
    SNo++;
    }
  return NULL;*/
  }

//---------------------------------------------------------------------------

long CHistorian::GetSlot(char* Tag)
  {
  long Index;
  if (SlotMap.Lookup(Tag, Index))
    return Index;
  return -1;
  }

//---------------------------------------------------------------------------

void CHistorian::FixFilterList()
  {
  FltSlotNos.SetSize(Data.lSlotsUsed);
  long FltCnt = 0;
  for (long SNo=0; SNo<Data.lSlotsUsed; SNo++)
    if (Slots[SNo]->Data.bUseFilter)
      FltSlotNos[FltCnt++] = SNo;
  FltSlotNos.SetSize(FltCnt);
  }

//---------------------------------------------------------------------------

WORD CHistorian::FindLastCatNo()
  {
  CString s;
  s = pHistory->sDataPath;
  s += pHistory->Data.sScenName;
  s += ".*";
  WIN32_FIND_DATA fd;
  HANDLE H = FindFirstFile((const char*)s, &fd);
  WORD N=0;
  if (H!=INVALID_HANDLE_VALUE)
    {
    // Find All data Files
    for (flag AllDone=0;!AllDone; AllDone = !FindNextFile(H, &fd))
      {
      // Is this File Valid
      CString Fn;
      Fn = pHistory->sDataPath;
      Fn += fd.cFileName;
      CFile f;
      if (f.Open((const char*)Fn, CFile::shareCompat|CFile::modeRead|CFile::typeBinary))
        {
        char Buff[256];
        f.Read((void*)Buff, sizeof(Buff));
        if (memcmp((void*)Buff, (void*)NewBlkKey, strlen(NewBlkKey)) != 0)
          {
          // Does have data in it !
          char Ext[_MAX_EXT];
          if (CCatItem::FixedDataExtn())
            {
            char Name[_MAX_FNAME];
            _splitpath(fd.cFileName, NULL, NULL, Name, Ext);
            _splitpath(Name, NULL, NULL, NULL, Ext);
            }
          else
            _splitpath(fd.cFileName, NULL, NULL, NULL, Ext);
          const WORD N1=CCatItem::CatNoFromStr(&Ext[1]);
          N=Max(N, N1);
          //if (N1>N && Cat.Find(N1))
          //  {//does the catalog know about this file?
          //  N=N1;
          //  }
          }
        f.Close();
        }
      }
    FindClose(H);
    }
  return N;
  }

//---------------------------------------------------------------------------

void CHistorian::Recover(WORD MaxCatNo, BOOL CheckAgainstCat)
  {//attempt to repair historian etc
  const bool IsPermHist = (iFilesMin!=iFilesMax);
  #if TryHistFix
  ASSERT(MaxCatNo>=0);
  //check next historian block file number...
  flag Checked = False;
  while (!Checked)
    {
    CString Filename;
    CCatItem::MakeFilename(Data.iCurCatNo, Filename);
    const char* Fn = (const char*)Filename;
    CFileStatus State;
    if (CFile::GetStatus(Fn, State)==0)
      Data.iCurCatNo--;
    else
      {
      if (Data.iCurCatNo>MaxCatNo)
        {//info on file not contained in catalog, delete it!
        Data.iCurCatNo--;
        if (DeleteFile(Fn)) //do we really want to delete them ???
          LogError("History", 0, "Historian file '%s' deleted!", Fn);
        }
      else
        {
        CFile f;
        if (f.Open(Fn, CFile::shareCompat|CFile::modeRead|CFile::typeBinary))
          {
          char Buff[256];
          f.Read((void*)Buff, sizeof(Buff));
          if (memcmp((void*)Buff, (void*)NewBlkKey, strlen(NewBlkKey)) == 0)
            Data.iCurCatNo--;
          else
            Checked = True;
          f.Close();
          }
        else
          {
          Checked = True; //can't open it, do something ????
          }
        }
      }
    if (Data.iCurCatNo==0)
      Checked = True;
    }

  //save fixes...
  Data.bDidClose = 1;
  Save(-2);

  #else

  if (IsPermHist)
    LogWarning("History", 0, "Attempting historian file recovery...");
//  ClearDataLists();
//    WORD         iCurCatNo;        //catalog number of the current EvBlk
//    DWORD        lNxtSlotID;       //ID to be given to next new slot

  /*
  blah...
  blah...
  etc
  */

  //check next historian block file number...
  flag Checked = false;
  while (!Checked)
    {
    CString Filename;
    CCatItem::MakeFilename(Data.iCurCatNo, Filename);
    const char* Fn = (const char*)Filename;
    CFileStatus State;
    if (CFile::GetStatus(Fn, State)==0)
      Data.iCurCatNo--; //file doesn't exist
    //else if (Cat.Find(Data.iCurCatNo)==NULL)
    //  Data.iCurCatNo--; //catalog doesn't know about file
    else
      {
      CFile f;
      if (f.Open(Fn, CFile::shareCompat|CFile::modeRead|CFile::typeBinary))
        {
        char Buff[256];
        f.Read((void*)Buff, sizeof(Buff));
        if (memcmp((void*)Buff, (void*)NewBlkKey, strlen(NewBlkKey)) == 0)
          Data.iCurCatNo--; //file header data is bad
        else
          Checked = true;
        f.Close();
        }
      else
        Checked = true; //can't open it, do something ????
      }
    if (Data.iCurCatNo==0)
      Checked = true;
    }

  //save fixes...
  Data.bDidClose = 1;
  Save(-2);
  Cat.Data.iCatItemCnt = Data.iCurCatNo;
  Cat.Save(-2); //save header only

  #endif
  }

//---------------------------------------------------------------------------

void CHistorian::DeleteExtra()
  {
  const bool IsPermHist = (iFilesMin!=iFilesMax);
  CString s;
  s = pHistory->sDataPath;
  s += pHistory->Data.sScenName;
  s += ".*";
  WIN32_FIND_DATA fd;
  HANDLE H = FindFirstFile((const char*)s, &fd);
  flag AllDone = (H==INVALID_HANDLE_VALUE);
  while (!AllDone)
    {
    char Ext[_MAX_EXT];
    if (CCatItem::FixedDataExtn())
      {
      char Name[_MAX_FNAME];
      _splitpath(fd.cFileName, NULL, NULL, Name, Ext);
      _splitpath(Name, NULL, NULL, NULL, Ext);
      }
    else
      _splitpath(fd.cFileName, NULL, NULL, NULL, Ext);
    WORD ExtCatNo = CCatItem::CatNoFromStr(&Ext[1]);
    if (ExtCatNo>Data.iCurCatNo)
      {
      if (DeleteFile(fd.cFileName))
        {
        if (IsPermHist)
          LogError("History", 0, "Historian file '%s' deleted!", fd.cFileName);
        }
      }
    AllDone = !FindNextFile(H, &fd);
    }
  FindClose(H);
  }

//---------------------------------------------------------------------------

void CHistorian::CopyFiles()
  {
  CString Filename;
  CString CatFilename;
  WIN32_FIND_DATA fd;
  HANDLE H;
  const char* Fn;
  const char* CatFn;
  flag b;
  for (long i=Data.iCurCatNo - iFilesMax; i>(long)Data.iCurCatNo - (long)iFilesMax - 4; i--)
    if (i>0)
      {
      CCatItem::MakeFilename((WORD)i, Filename);
      Fn = (const char*)Filename;
      H = FindFirstFile(Fn, &fd);
      if (H!=INVALID_HANDLE_VALUE)
        {
        FindClose(H);
        b = DeleteFile(Fn);
        if ((b) && (iFilesMin!=iFilesMax))
          LogError("History", 0, "Historian file '%s' deleted!", Fn);
        }
      }

  if (iFilesMax<=iFilesMin)
    return;

  for (i=Data.iCurCatNo - iFilesMax + 1; i<=(long)Data.iCurCatNo - (long)iFilesMin; i++)
    if (i>0)
      {
      CCatItem::MakeFilename((WORD)i, Filename);
      CCatItem::MakeCatFilename((WORD)i, CatFilename);
      Fn = (const char*)Filename;
      CatFn = (const char*)CatFilename;
      H = FindFirstFile(Fn, &fd);
      if (H!=INVALID_HANDLE_VALUE)
        {
        FindClose(H);
        b = CopyFile(Fn, CatFn, false);
        if (b)
          DeleteFile(Fn);
        else
          {
          if (iFilesMin!=iFilesMax)
            LogError("History", 0, "Unable to copy historian file '%s' to catalog", Fn);
          }
        }
      }
  }

//---------------------------------------------------------------------------

void CHistorian::DeleteDataFiles()
  {
  Strng Fn(sDataPath),se;
  Fn.FnExpand();
  Fn.FnCheckEndBSlash();
  se=Fn;
  Fn += Data.sScenName;
  Fn += ".*";
  WIN32_FIND_DATA fd;
  HANDLE H = FindFirstFile(Fn(), &fd);
  flag AllDone = (H==INVALID_HANDLE_VALUE);
  while (!AllDone)
    {
    Fn = se;
    Fn += fd.cFileName;
    if (DeleteFile(Fn()))
      LogNote("History", 0, "Deleted '%s'", Fn());
    else
      LogError("History", 0, "Failed to delete '%s'", Fn());
    AllDone = !FindNextFile(H, &fd);
    }
  FindClose(H);
  }

//---------------------------------------------------------------------------

void CHistorian::DeleteHistorian()
  {//delete all historian related files...
  CWaitCursor Wait;
  Strng Root((const char*)sDataPath);
  Root.FnExpand();
  Root.FnCheckEndBSlash();
  Strng Fn = Root;
  Fn += (const char*)sName;
  Fn += ".hed";
  if (DeleteFile(Fn()))
    LogNote("History", 0, "Deleted '%s'", Fn());
  Fn = Root;
  Fn += (const char*)sName;
  Fn += ".cat";
  if (DeleteFile(Fn()))
    LogNote("History", 0, "Deleted '%s'", Fn());

  DeleteDataFiles();
  }

//---------------------------------------------------------------------------

void CHistorian::ServiceHistoryQueries(byte ThreadNo)
  {
  while (1)
    {
    bThreadIsBusy[ThreadNo] = false;
    #if dbgHistorian
    if (dbgThreads())
      dbgpln("QT: Enter pause on query thread %d", ThreadNo);
    #endif
    WaitForSingleObject(hQuerySignal[ThreadNo], INFINITE);
    if (bQueryClose)
      break;

    #if dbgHistorian
    if (dbgThreads())
      dbgpln("QT: Service query on thread %d", ThreadNo);
    #endif
    pCQueryHist pQuery = NULL;
    EnterCriticalSection(&QuerySection);
    bThreadIsBusy[ThreadNo] = true;
    if (pQueryList)
      {
      pQuery = pQueryList;
      while ((pQuery) && (pQuery->bBusyServicing))
        pQuery = pQuery->pNext;
      if (pQuery)
        pQuery->bBusyServicing = 1;
      }

    if (pQuery)
      {
      QThreadInfo[ThreadNo].pQuery = pQuery;
      LeaveCriticalSection(&QuerySection);
      #if dbgHistorian
      if (dbgThreads())
        dbgpln("QT: Get query on thread %d, SrcID:%d", ThreadNo, pQuery->lSrcID);
      #endif
      pQuery->GetQuery();
      EnterCriticalSection(&QuerySection);
      pQuery->bBusyServicing=0;
      if (pQuery == pQueryList)
        pQueryList = pQuery->pNext;
      else
        {
        pCQueryHist pQ = pQueryList;
        while (pQ)
          {
          if (pQ->pNext==pQuery)
            {
            pQ->pNext = pQuery->pNext;
            break;
            }
          pQ = pQ->pNext;
          }
        }
      QThreadInfo[ThreadNo].pQuery = NULL;
      bThreadIsBusy[ThreadNo] = false;
      LeaveCriticalSection(&QuerySection);
      delete pQuery;
      }
    else
      {
      QThreadInfo[ThreadNo].pQuery = NULL;
      bThreadIsBusy[ThreadNo] = false;
      LeaveCriticalSection(&QuerySection);
      }
    }

  }

//---------------------------------------------------------------------------

flag CHistorian::CreateEventBlk(pCEvBlkFileInfo pI)
  {//create event block mem-mapped file...
  const char* fn = (const char*)(pI->sFilename);
  WIN32_FIND_DATA fd;
  BOOL IsNew = true;
  HANDLE H;
  if ((H=FindFirstFile(fn, &fd))!=INVALID_HANDLE_VALUE)
    {
    FindClose(H);
    IsNew = false;
    }
  if (IsNew)
    LogNote("History", 0, "Create historian file %s", fn);
  else if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
    {
    LogError("History", 0/*LF_Exclamation*/, "Cannot open read-only Event Block file '%s'", fn);
    return false;
    }

  pI->hBlkFile = CreateFile(fn, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
  if (pI->hBlkFile==INVALID_HANDLE_VALUE)
    {
    LogError("History", 0, "Event Block File '%s' Not Created [%i]", fn, GetLastError());
    pI->hBlkFile = NULL;
    return false;
    }

  pI->dwFileAllocLen = Max(GetFileSize(pI->hBlkFile, NULL), (DWORD)pHistory->Data.lReqdBlkSize);
  //pI->dwFileAllocLen = Max(GetFileSize(pI->hBlkFile, NULL), (DWORD)10240; //force small blocks for debug...

  if (SetFilePointer(pI->hBlkFile, pI->dwFileAllocLen, 0, FILE_BEGIN) == 0xffffffff)
    {
    if (!CloseHandle(pI->hBlkFile))
      LogError("History", 0, "Close Handle G %s", FindWinError(GetLastError()));
    pI->hBlkFile = NULL;
    return false;
    }

  if (!SetEndOfFile(pI->hBlkFile))
    {
    if (!CloseHandle(pI->hBlkFile))
      LogError("History", 0, "Close Handle H %s", FindWinError(GetLastError()));
    pI->hBlkFile = NULL;
    return false;
    }

  CString MapName = fn;
  int i = MapName.ReverseFind('\\');
  if (i>=0)
    MapName = MapName.Mid(i+1);
  MapName += "_MAP";
  const char* mn = (const char*)(MapName);
  pI->hBlkFileMap = CreateFileMapping((LPVOID)pI->hBlkFile, NULL, PAGE_READWRITE, 0,0, mn);
  if (pI->hBlkFileMap==NULL)
    {
    LogError("History", 0, "Create File Mapping for '%s' [%i]", fn, GetLastError());
    if (!CloseHandle(pI->hBlkFile))
      LogError("History", 0, "Close Handle I %s", FindWinError(GetLastError()));
    pI->hBlkFile = NULL;
    return false;
    }

  pI->pBlkMap = (byte*)MapViewOfFile(pI->hBlkFileMap, FILE_MAP_WRITE, 0, 0, 0);
  if (pI->pBlkMap==NULL)
    {
    LogError("History", 0, "Map View Of File for '%s' [%i]", fn, GetLastError());
    if (!CloseHandle(pI->hBlkFileMap))
      LogError("History", 0, "Close Handle J %s", FindWinError(GetLastError()));
    pI->hBlkFileMap = NULL;
    if (!CloseHandle(pI->hBlkFile))
      LogError("History", 0, "Close Handle K %s", FindWinError(GetLastError()));
    pI->hBlkFile = NULL;
    return false;
    }

  if (IsNew)
    {
    memcpy((void*)pI->pBlkMap, (void*)NewBlkKey, strlen(NewBlkKey));
    FlushViewOfFile((void*)pI->pBlkMap, 0);
    }

  return true;
  }

#if ForSysCAD
//---------------------------------------------------------------------------
//----------- ExecObj Overrides
//---------------------------------------------------------------------------

DWORD CHistorian::EO_Message(CXMsgLst &XM, CXM_Route &Route)
  {
  #if dbgHistorian
  DWORD StartTime = GetTickCount();
  #endif
  long LastIndex=-1;
  flag KeepingHistory=0;
  DWORD RetCode = 0;
  //XM.dbgDump(0, "CHistorian::EO_Message");
  while (XM.MsgAvail())
    {
    switch (XM.MsgId())
      {
      case XM_HistoryExists:
        {
        CXM_HistoryExists* pX = XM.HistoryExists();
        char* pTag=pX->FirstTag();
        while (pTag)//strlen(pTag))
          {
          long i = GetSlot(pTag);
          RetCode=0;
          if (i>=0)
            {
            RetCode |= RC_SM_HistExists; //exists in historian
            if (Slots[i]->Data.bRecordIt)
              RetCode |= RC_SM_HistRecOn; //recording is on
            if (Slots[i]->Data.bDriver)
              RetCode |= RC_SM_HistDriverTag; //driver tag
            }
          pX->SetFlags(pTag, RetCode);
          pTag=pX->NextTag(pTag);
          }
        XM.RewindReadPosition();
        return RetCode;
        }

      case XM_KeepHistory:
        {
        CXM_KeepHistory * pX = XM.KeepHistory();
        SlotCfgInfo SCI;
        pchar p = pX->cTagEndDesc;
        SCI.mTag = p;
        if (SCI.mTag && strlen(SCI.mTag)>0)
          {
          SCI.mTyp = pX->cType;
          SCI.mFilterTau = pX->dFltTau;
          SCI.mWinFltPeriod = pX->dWinFltPeriod;
          SCI.mWinFltCount = pX->iWinFltCount;
          SCI.mFltDelta = pX->dFltDelta;
          SCI.mDeltaLo = pX->dDeltaLo;
          SCI.mDeltaHi = pX->dDeltaHi;
          SCI.mNoRec = pX->iNoRec;
          SCI.mNoNotRec = pX->iNoNotRec;
          SCI.mChange = pX->dChange;
          SCI.mFilterAlg = pX->iFilterAlg;
          SCI.mBoxcarAlg = pX->iBoxcarAlg;
          SCI.mRecordIt = pX->bRecordIt;
          SCI.mDriver = pX->bDriver;
          SCI.mUseFilter = (pX->iFilterAlg>0);
          SCI.mUseBoxcar = (pX->iBoxcarAlg>0);
          p += strlen(p)+1;
          SCI.mEng = p;
          p += strlen(p)+1;
          SCI.mCnvFam = Cnvs[(pX->iCnv)]->Fam();
          SCI.mDesc = p;
          long SlotNo = CfgSlot(&SCI); //configure slot
          if (SlotNo>=0)
            {
            RetCode |= RC_SM_HistExists;
            if (Slots[SlotNo]->Data.bRecordIt)
              RetCode |= RC_SM_HistRecOn; //recording is on
            if (Slots[SlotNo]->Data.bDriver)
              RetCode |= RC_SM_HistDriverTag; //driver tag
            if (gs_Exec.Busy()/* && Slots[SlotNo]->Data.bRecordIt*/)
              {//ensure value is recorded at next iteration...
              const int Size = RecSlotNos.GetLen();
              RecSlotNos.SetSize(Size+1);
              RecSlotNos[Size] = SlotNo;
              if (Slots[SlotNo]->Data.bRecordIt)
                {
                Slots[SlotNo]->MeasVal.Set(pX->Val);
                Slots[SlotNo]->iStatus = TS_STATUS_VALID;
                //Slots[SlotNo]->PrevMeasVal.TypeDouble(Slots[SlotNo]->Data.VarTyp, dNAN);
                Slots[SlotNo]->Data.PrevMeasVal.SetStatus(ds_NAN);
                Slots[SlotNo]->Data.iPrevStatus = TS_STATUS_INVALID;
                }
              else
                {
                Slots[SlotNo]->MeasVal.SetTypeDouble(Slots[SlotNo]->Data.VarTyp, dNAN);
                Slots[SlotNo]->MeasVal.SetStatus(ds_NAN);
                Slots[SlotNo]->iStatus = TS_STATUS_INVALID;
                Slots[SlotNo]->Data.PrevMeasVal.Set(pX->Val);
                Slots[SlotNo]->Data.iPrevStatus = TS_STATUS_VALID;
                }
              }
            }
          }
        KeepingHistory = 1;
        LastIndex = pX->lDataIndex;
        break;
        }

      case XM_KeepHistoryFile:
        {
        CXM_KeepHistoryFile * pX = XM.KeepHistoryFile();
        char* pFilename = pX->cHistTagListFile;
        int line_number = 0;
        int tags_added = 0;
//dbgpln("XM_KeepHistoryFile : %s", pFilename);
        if (FileExists(pFilename))
          {
          FILE* f=fopen(pFilename, "rt");
          if (f)
            {
            Strng WrkTag, WrkCnvTxt;
            char* c[MaxCSVCols];
            char cTag[256],cCnv[128],cFam[128],cDesc[2];
            cDesc[0] = 0;
            char Buff[4096];
            // Skip Title Line
            if (fgets(Buff, sizeof(Buff)-1, f))
              line_number++;
            while (!feof(f))
              {
              if (fgets(Buff, sizeof(Buff)-1, f))
                {
                int Quote;
                int nFlds = ParseCSVTokens(Buff, c, Quote);
                if (Quote)
                  {
                  LogError("History", 0, "Line %u: Quotes mismatched", line_number);
                  break;
                  }
                line_number++;
                if (c[0] && strlen(c[0])>0)
                  {
//dbgpln("  : %s",c[0]);
                  TaggedObject::SplitTagCnv(c[0], WrkTag, WrkCnvTxt);
                  CXM_ObjectTag ObjTag(WrkTag(), TABOpt_ValCnvsOnce);//TABOpt_AllInfoOnce);
                  CXM_ObjectData ObjData;
                  CXM_Route      Route;
                  if (XReadTaggedItem(ObjTag, ObjData, Route))
                    {
                    CPkDataItem *pItem = ObjData.FirstItem();
                    byte cType = pItem->Type();
                    if (IsNumData(cType))
                      {
                      strcpy(cTag, WrkTag());
                      CCnvIndex CnvIndx = pItem->CnvIndex();
                      const bool HasCnv = (CnvIndx>0);
                      if (HasCnv)
                        {
                        strcpy(cCnv, Cnvs[CnvIndx]->Txt());
                        strcpy(cFam, Cnvs[CnvIndx]->Fam());
                        }
                      else
                        {
                        cCnv[0] = 0;
                        cFam[0] = 0;
                        }
                      SlotCfgInfo SCI;
                      SCI.mTag = cTag;
                      SCI.mTyp = cType;
                      SCI.mEng = cCnv;
                      SCI.mCnvFam = cFam;
                      SCI.mDesc = cDesc;
                      long SlotNo = CfgSlot(&SCI); //configure slot
                      if (SlotNo>=0)
                        {
                        tags_added++;
                        if (gs_Exec.Busy())
                          {//ensure value is recorded at next iteration...
                          const int Size = RecSlotNos.GetLen();
                          RecSlotNos.SetSize(Size+1);
                          RecSlotNos[Size] = SlotNo;
                          if (Slots[SlotNo]->Data.bRecordIt)
                            {
                            Slots[SlotNo]->MeasVal.Set(*(pItem->Value()));
                            Slots[SlotNo]->iStatus = TS_STATUS_VALID;
                            //Slots[SlotNo]->PrevMeasVal.TypeDouble(Slots[SlotNo]->Data.VarTyp, dNAN);
                            Slots[SlotNo]->Data.PrevMeasVal.SetStatus(ds_NAN);
                            Slots[SlotNo]->Data.iPrevStatus = TS_STATUS_INVALID;
                            }
                          else
                            {
                            Slots[SlotNo]->MeasVal.SetTypeDouble(Slots[SlotNo]->Data.VarTyp, dNAN);
                            Slots[SlotNo]->MeasVal.SetStatus(ds_NAN);
                            Slots[SlotNo]->iStatus = TS_STATUS_INVALID;
                            Slots[SlotNo]->Data.PrevMeasVal.Set(*(pItem->Value()));
                            Slots[SlotNo]->Data.iPrevStatus = TS_STATUS_VALID;
                            }
                          }
                        }
                      KeepingHistory = 1;
                      }
                    }
                  else
                    {
                    LogWarning("History", 0, "Tag %s not added to data historian (not found : line %u)", WrkTag(), line_number-1);
                    }
                  }
                }
              }
            fclose(f);
            LogNote("History", 0, "%u out of %u tags added to data historian", tags_added, line_number-1);
            }
          else
            {
            LogError("History", 0, "Error opening file:%s", pFilename);
            }
          }
        else
          {
          LogError("History", 0, "File not found:%s", pFilename);
          }
        XM.Clear();
        return True;
        }

      case XM_QueryHistory:  // NBNB Only Handles one at a time
        {
        CXM_QueryHistory * pQH = XM.QueryHistory();
        CXM_Route * pXRoute = XM.Route();
        //pXRoute->dbgDump("QueryHistory");
        pCQueryHist pQuery = new CQueryHist(pQH->dStartTime.Seconds, pQH->dEndTime.Seconds, *pXRoute, pQH->iRqstNo, pQH->iSrcID, True, True);
        int TrndNo;
        for (char *pTag=pQH->FirstTag(TrndNo); pTag; pTag=pQH->NextTag(TrndNo, pTag))
          {
          //dbgpln("QH %4i %s", TrndNo, pTag);
          pQuery->AddTag(pTag, TrndNo);
          }

        if (pQuery->iTagCnt == 0)
          {
          delete pQuery;
          XM.Clear();
          return False;
          }
        else
          {//try find a query from the same source that has the same route...
          pQuery->Slots.SortRows(0, True, -1);
          EnterCriticalSection(&QuerySection);
          pCQueryHist pQ = pQueryList;
          CXM_Route CompareRoute;// = *pXRoute;
          CompareRoute.ReverseRoute(*pXRoute);
          while (pQ)
            {
            if (pQ->QueryDst==QTrend && pQ->lSrcID==pQuery->lSrcID && pQ->xRoute==CompareRoute)//*pXRoute)
              break; //the same
            pQ = pQ->pNext;
            }

          if (pQ)
            {//similar query was found, update old query...
            if (pQ->bBusyServicing)
              {
              pQ->bCancel = 1;
              pQ = NULL;
              }
            else
              {
              pQ->dStartTime = pQuery->dStartTime;
              pQ->dLastTime = pQuery->dLastTime;
              pQ->lRqstNo = pQuery->lRqstNo;
              }
            }
          LeaveCriticalSection(&QuerySection);

          if (pQ)
            delete pQuery;
          else
            {//add new query to list...
            EnterCriticalSection(&QuerySection);
            if (pQueryList)
              pQuery->pNext = pQueryList;
            pQueryList = pQuery;
            flag DoneRelease = False;
            //first check all busy threads to see if one of them are allready busy with a query from the same source
            byte i = 0;
            while (i<MaxQueryThreads && !DoneRelease)
              {
              if (bThreadIsBusy[i])
                {
                if (QThreadInfo[i].pQuery->lSrcID==pQuery->lSrcID)
                  {
                  DoneRelease = True;
                  ReleaseSemaphore(hQuerySignal[i], 1, NULL);
                  }
                }
              i++;
              }
            if (!DoneRelease)
              {//find any free thread...
              i = 0;
              while (i<MaxQueryThreads && !DoneRelease)
                { //maybe instead of flags for each thread, usage counts (obtained from semaphore ???) would be better ?
                if (!bThreadIsBusy[i])
                  {
                  DoneRelease = True;
                  ReleaseSemaphore(hQuerySignal[i], 1, NULL);
                  }
                i++;
                }
              }
            if (!DoneRelease)
              ReleaseSemaphore(hQuerySignal[(long)(((long)rand()*MaxQueryThreads) / (long)(RAND_MAX+1))], 1, NULL); //must release something if all threads are busy
            LeaveCriticalSection(&QuerySection);
            }
          }
        XM.Clear();
        return True;
        }

      case XM_QueryHistoryOther:  // NBNB Only Handles one at a time
        {
        CXM_QueryHistoryOther* pQH = XM.QueryHistoryOther();
        CXM_Route* pXRoute = XM.Route();
        //pXRoute->dbgDump("QueryHistoryOther");
        CQueryHist* pQuery = new CQueryHist(pQH->dStartTime, pQH->dEndTime, (QueryOption)pQH->iOpt,
                                            pQH->iTimeOptUnits, pQH->bTimeOptFull, pQH->bHeadings, pQH->iNoPts,
                                            *pXRoute, pQH->iSrcID, (QueryDestination)pQH->iQryDest, pQH->Filename1(), pQH->Filename2(), pQH->iFileMode, pQH->dRepTimeOffset);

        if (pQuery->QueryDst==QFileDirect)
          {//add ALL tags
          for (long DataIndex=0; DataIndex<Data.lSlotsUsed; DataIndex++)
            {
            if (Slots[DataIndex]->Data.bRecordIt)
              {
              pQuery->AddTag(Slots[DataIndex]->Tag(), -1);
              }
            }
          if (pQuery->iTagCnt==0)
            LogWarning("History", 0, "Request for all tags failed as no tags are recorded. (File:%s)", pQH->Filename1());
          else
            {
            char DateBuff[128];
            if (pQH->dEndTime-pQH->dStartTime>86400.0)
              LogNote("History", 0, "Process request for all %d tags to be retrieved to %s... (duration:%gd  points:%d  StartTime:%s)", pQuery->iTagCnt, pQH->Filename1(), (pQH->dEndTime-pQH->dStartTime, pQH->iNoPts)/86400.0, SecstoDateHMS(pQH->dStartTime, DateBuff, true));
            else if (pQH->dEndTime-pQH->dStartTime>3600.0)
              LogNote("History", 0, "Process request for all %d tags to be retrieved to %s... (duration:%gs  points:%d  StartTime:%s)", pQuery->iTagCnt, pQH->Filename1(), (pQH->dEndTime-pQH->dStartTime)/3600.0, pQH->iNoPts, SecstoDateHMS(pQH->dStartTime, DateBuff, true));
            else
              LogNote("History", 0, "Process request for all %d tags to be retrieved to %s... (duration:%gs  points:%d  StartTime:%s)", pQuery->iTagCnt, pQH->Filename1(), pQH->dEndTime-pQH->dStartTime, pQH->iNoPts, SecstoDateHMS(pQH->dStartTime, DateBuff, true));
            }
          }
        else
          {//add requested tags
          for (char *pTag = pQH->FirstTag(); pTag; pTag=pQH->NextTag(pTag))
            if (pTag && (strlen(pTag)>0))
              {
              //dbgpln("QHDDE %s", pTag);
              pQuery->AddTag(pTag, -1);
              }
          }

        if (pQuery->iTagCnt == 0)
          {
          CXM_QueryString *xb=new CXM_QueryString (pQuery->lSrcID, "TheEnd");
          pQuery->XMr.Clear();
          pQuery->XMr.PackMsg(xb);
          XSendMessage(pQuery->XMr, pQuery->xRoute); //send something back
          delete pQuery;
          XM.Clear();
          return False;
          }
        else
          {//try find a query for the same set of slots that has the same route...
          pQuery->Slots.SortRows(0, True, -1);
          //add new query to list...
          EnterCriticalSection(&QuerySection);
          if (pQueryList)
            pQuery->pNext = pQueryList;
          pQueryList = pQuery;
          flag DoneRelease = False;
          //find any free thread...
          int i = 0;
          while (i<MaxQueryThreads && !DoneRelease)
            { //maybe instead of flags for each thread, usage counts (obtained from semaphore ???) would be better ?
            if (!bThreadIsBusy[i])
              {
              DoneRelease = True;
              ReleaseSemaphore(hQuerySignal[i], 1, NULL);
              }
            i++;
            }
          if (!DoneRelease)
            ReleaseSemaphore(hQuerySignal[(long)(((long)rand()*MaxQueryThreads) / (long)(RAND_MAX+1))], 1, NULL); //must release something if all threads are busy
          LeaveCriticalSection(&QuerySection);
          }
        XM.Clear();
        return True;
        }

      case XM_HistorySlotDlg:
        {
        CXM_HistorySlotDlg* pX = XM.HistorySlotDlg();
        SlotInfoDlg(pX->cTag, pX->iDlg);
        XM.Clear();
        return True;
        }

      case XM_HistRecordingOn:
        {
        CXM_HistRecordingOn* pX = XM.HistRecordingOn();
        long i = GetSlot(pX->cTag);
        flag RecordingOn = pX->bRecordingOn;
        XM.Clear();
        if (i>=0)
          {
          if ((Slots[i]->Data.bRecordIt!=0) != (RecordingOn!=0))
            {
            SlotCfgInfo SCI;
            Slots[i]->GetCfg(SCI);
            SCI.mRecordIt = RecordingOn;
            Lock();
            long SlotNo = CfgSlot(&SCI);
            ASSERT(SlotNo==i);
            Save(SlotNo);
            Release();
            XBuildMyDataLists();
            }
          RetCode |= RC_SM_HistExists; //exists in historian
          if (Slots[i]->Data.bRecordIt)
            RetCode |= RC_SM_HistRecOn; //recording is on
          if (Slots[i]->Data.bDriver)
            RetCode |= RC_SM_HistDriverTag; //driver tag
          if (gs_Exec.Busy()/* && Slots[SlotNo]->Data.bRecordIt*/)
            {//ensure value is recorded at next iteration...
            CXM_ObjectTag ObjTag(Slots[i]->Tag(), TABOpt_ValCnvsOnce);//TABOpt_Parms);//TABOpt_AllInfoOnce);
            CXM_ObjectData ObjData;
            CXM_Route Route;
            if (XReadTaggedItem(ObjTag, ObjData, Route))
              {
              const int Size = RecSlotNos.GetLen();
              RecSlotNos.SetSize(Size+1);
              RecSlotNos[Size] = i;
              CPkDataItem * pItem = ObjData.FirstItem();
              if (Slots[i]->Data.bRecordIt)
                {
                Slots[i]->MeasVal.Set(*(pItem->Value()));
                Slots[i]->iStatus = TS_STATUS_VALID;
                //Slots[i]->PrevMeasVal.TypeDouble(Slots[i]->Data.VarTyp, dNAN);
                Slots[i]->Data.PrevMeasVal.SetStatus(ds_NAN);
                Slots[i]->Data.iPrevStatus = TS_STATUS_INVALID;
                }
              else
                {
                Slots[i]->MeasVal.SetTypeDouble(Slots[i]->Data.VarTyp, dNAN);
                Slots[i]->MeasVal.SetStatus(ds_NAN);
                Slots[i]->iStatus = TS_STATUS_INVALID;
                Slots[i]->Data.PrevMeasVal.Set(*(pItem->Value()));
                Slots[i]->Data.iPrevStatus = TS_STATUS_VALID;
                }
              }
            }
          }
        return RetCode;
        }

      default:
        ASSERT(0);
      }
    }
  XM.Clear();

  if (KeepingHistory)
    {
    if (RetCode)
      {
      #if dbgHistorian
      if (dbgTime())
        dbgpln("HT DoneCfgSlots:%d", GetTickCount() - StartTime);
      #endif
      Save();  // Save Configuration
      #if dbgHistorian
      if (dbgTime())
        dbgpln("HT DoneSave:%d", GetTickCount() - StartTime);
      #endif
      XBuildMyDataLists();
      }
    CXM_Long *xbLong=new CXM_Long(LastIndex);
    XM.PackMsg(xbLong);
    }
  return RetCode;
  }

//---------------------------------------------------------------------------
#define WhatWasThis 0
//const double InitTimeGap = 0.0;// CNM 0025; // 0.0005 > 0.00025 so that each start/stop advance 0.001 secs only

flag CHistorian::EO_SetTime(CTimeValue TimeRqd, bool Rewind)
  {//this can be called a number of times each iteration!
  if (TimeRqd < GetLastTime())
    {
    if (Rewind)
      {
      flag OK=Close();
      OK=OK && Restart(Data.sScenName);
      DeleteDataFiles();
      OK=OK && Open(Data.sScenName, Data.lReqdBlkSize, Data.lMaxTimeSliceLen/5, Data.lReqdBlkSize/1024L)==0;
      if (!OK)
        return False;
      }
    else if (TimeRqd < GetLastTime()-CTimeValue::MinResolution)
      {
      LogError("History", 0, "SetTime failure");
      return False;
      }
    }

#if WhatWasThis
  dTime=(TimeRqd-m_StopGap).Seconds;
  dPrevTime=(TimeRqd-m_StopGap).Seconds;
  dMidTime=dPrevTime+(m_StopGap*0.5);
#else
  if (dTime!=TimeRqd.Seconds)
    {
    dPrevTime=dTime;
    dTime=TimeRqd.Seconds;
    dMidTime = dPrevTime + ((dTime - dPrevTime) * m_MidTimeFactor);
//LogNote("EO SetTime",0,"Hist  %18.6f --> %18.6f [%+12.6f]",dPrevTime,dTime,dTime-dPrevTime);
    }
#endif
  //dTimeRunStarted=dTime; !!!????
  return True;
  };

flag CHistorian::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd)
  {
  if (!pCurEvBlk)
    {
    bSwitchedOff = False;
    AddEvBlk();
    }
// CNM why is this here
// if (bSwitchedOff)
//    return True;
  if (CB.m_bSyncWithClock)
    {
    if (CB.m_TheTime < (CTimeValue(GetLastTime()) + m_StopGap))
      {
      LogError("History", 0, "Last Time > Current Time");
      return False;
      }
    }
  else
    {
    //dbgpln("HistLastTime %23.6f", GetLastTime());
    TimeRqd = CTimeValue(GetLastTime());// CNM + 0.01;
    }
  //dbgpln("Hist QueryTime    %15.3f %s", dTime, TimeRqd.Format(TD_DaysJoined|TD_Time)); 
  return True;
  }

//---------------------------------------------------------------------------

flag CHistorian::EO_Start(CXM_TimeControl &CB)
  {
  bDoneWrite      = False;
  bJustStarted    = True;
  dTimeRunStarted = CB.m_TheTime.Seconds;
  bDoSaveAll      = True;
  bSwitchedOff    = (pCurEvBlk==NULL);
  lEvSlotNoCnt    = 0;
  RecSlotNos.SetSize(0);
  for (long i=0; i<Data.lSlotsUsed; i++)
    Slots[i]->bIsRecording = False;
  ResetStats();

  //dbgpln("Hist Start Time A %15.3f %s", dTime, gs_Exec.TheTime.Format(TD_DaysJoined|TD_Time)); 
  return True;
  }

//---------------------------------------------------------------------------

void CHistorian::EO_QuerySubsReqd(CXMsgLst &XM)
  {
  //dbgpln("SubsRqd:%13.3f   %13.3f", dPrevTime, dTime);
  CXM_ReadIndexedData * pMsg = (CXM_ReadIndexedData*)XM.MsgPtr(XM_ReadIndexedData);
  flag IsStart=pMsg->Start;
  long LastIndex=pMsg->LastIndex;
  XM.Clear();
  if (!bSwitchedOff)
    {
    long DataIndex = IsStart ? 0 : LastIndex+1;
    for ( ; DataIndex < Data.lSlotsUsed; DataIndex++)
      {
      if (Slots[DataIndex]->Data.bRecordIt)
        {
        //CXM_DataRequest DRqst(DataIndex, Slots[DataIndex]->Data.Tag(), TABOpt_AllInfoOnce, XIO_In);
        //KGA 19/11/97 : changed from above line to that seen below:
        //            : Trend windows did not refresh measured values properly!
        //            : eg had a tag in driver, historian, PGM and trend  --> DID NOT WORK !!
        //
        //   I think the problem is when a driver and solver are present, the executive_manager
        //   gets confused with data subscriptions, to fix, use XIO_InOut EVERYWHERE (gencon,
        //   driver, trend, hist, etc) instead of XIO_In or XIO_Out !!!
        CXM_DataRequest *DRqst=new CXM_DataRequest (DataIndex, Slots[DataIndex]->Data.Tag(), TABOpt_AllInfoOnce, XIO_InOut);
        if (!XM.PackMsg(DRqst))
          break;
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CHistorian::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)
  {
  }

//---------------------------------------------------------------------------

flag CHistorian::EO_ReadSubsData(CXMsgLst &XM)
  {
  return False;
  }

//---------------------------------------------------------------------------

void CHistorian::WriteSlots(flag MustDoSaveAll)
  {
  if (!bSwitchedOff)
    {
    RecSlotNos.SetSize(0);
    if (MustDoSaveAll)
      {
      for (long i=0; i<Data.lSlotsUsed; i++)
        if (Slots[i]->bIsRecording && Slots[i]->Data.bRecordIt)
          {
          Slots[i]->Changed(True);
          }
      }

    #if dbgHistorian
    long nEvents=0;
    long nEvBlks=0;
    long nBytes=0;
    #endif

    for (long i=0; i<lEvSlotNoCnt; i++)
      {
      #if dbgHistorian
      nEvents++;
      long TtlLen = pCurEvBlk->lTtlLen + (pCurEvBlk->pTS!=NULL ? pCurEvBlk->pTS->iLen : 0);
      #endif

      CSlot* pSlot = GetSlotByIndex(EvSlotNos[i]);
      #if dbgHistorian
      if (dbgWriteData())
        dbgpln("HistAddEv:%3d %15.3f  %g %s", i, dTime, pSlot->MeasVal.GetDouble(), pSlot->Tag());
      #endif
      if (!pCurEvBlk->AddEv(dTime, pSlot->lSNo, pSlot->MeasVal))
        {
        if (pCurEvBlk==NULL)
          {//were not able to open new event block, why not ???  disk space ???
          bSwitchedOff = True;
          MustDoSaveAll = False;
          i = lEvSlotNoCnt;
          LogError("History", 0, "Historian not recording, insufficient disk space");
          }
        else
          {
          pCurEvBlk->AddEv(dTime, pSlot->lSNo, pSlot->MeasVal); //try again, possibly a new event block
          #if dbgHistorian
          nEvBlks++;
          nBytes += pCurEvBlk->lTtlLen + (pCurEvBlk->pTS!=NULL ? pCurEvBlk->pTS->iLen : 0);
          #endif
          }
        }
      else
        {
        #if dbgHistorian
        nBytes += (pCurEvBlk->lTtlLen + (pCurEvBlk->pTS!=NULL ? pCurEvBlk->pTS->iLen : 0))-TtlLen;
        #endif
        }
      }

    if (MustDoSaveAll)
      {
      if (pCurEvBlk->pTS)
        pCurEvBlk->EndTimeSlice();
      bDoSaveAll = False;
      }
    lEvSlotNoCnt = 0;

    #if dbgHistorian
    if (dbgStatistics())
      {
      dbgp("Hist Stats: %s   Events:%5i", MustDoSaveAll ? "SaveAll" : "Chgs   ", nEvents);
      if (nEvents>0)
        dbgp("   Blks:%2i   Space:%7.3f kb   Spc/Ev:%6.2f bytes/Ev", nEvBlks, nBytes/1024.0, nBytes/(float)nEvents);
      dbgpln("");
      }
    #endif
    }
  }

//---------------------------------------------------------------------------

flag CHistorian::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)
  {
  //dbgpln("Hist WriteSubs    %15.3f %s", dTime, gs_Exec.TheTime.Format(TD_DaysJoined|TD_Time)); 

  //Some notes:
  // Historian must be given all tags that (will potentially) be recorded in the
  // correct sequence for this time stamp. Effectively an ordered array of indexes
  // to all relevant historian slots is built, WriteSlots() then processes the array.
  //1) historian gets told about all tags that have changed (by the Executive Manager)
  //2) historian must be told about all tags that must be recorded due to filtering
  //   conditions (even-though they have not been changed this iteration)
  //3) any tags for which recording is switched on or off must be recorded
  //   regardless of change
  //
  //Also note that all values are saved when the solver is started, stopped or every
  //n'th iteration where n is specified in the driver configuration (force count).

  if (bSwitchedOff)
    return True;

  if (gs_Exec.Paused())
    {
    while (XM.MsgAvail())
      {
      CXM_ObjectData* pX = XM.ObjectData();
      CPkDataItem* pItem = pX->FirstItem();
      long SNo = pX->Index;

      if (IsData(pItem->Type()) && SNo < Data.lSlotsUsed)
        {
        #if dbgHistorian
        if (dbgWriteData())
          dbgpln("HistPauseWrite:%3d %15.3f  %g %s", SNo, dTime, pItem->Value()->GetDouble(), Slots[SNo]->Tag());
        #endif
        Slots[SNo]->bIsRecording = True;
        Slots[SNo]->dwChangeCnt++;
        if (Slots[SNo]->Data.bRecordIt)
          {
          Slots[SNo]->MeasVal.Set(*(pItem->Value()));
          switch (pItem->Value()->Status())
            {//could do a bit left shift instead
            case ds_OK        : Slots[SNo]->iStatus = TS_STATUS_VALID; break;
            case ds_UnderFlow : Slots[SNo]->iStatus = TS_STATUS_UNDER; break;
            case ds_OverFlow  : Slots[SNo]->iStatus = TS_STATUS_OVER; break;
            case ds_NAN       : Slots[SNo]->iStatus = TS_STATUS_INVALID; break;
            default: ASSERT(FALSE); Slots[SNo]->iStatus = TS_STATUS_INVALID; //invalid status !!!
            }
          Slots[SNo]->m_bChangedDuringPause=true;
          m_bWasPaused=true;
          }
        }
      }
    return True;
    }

  flag MustDoSaveAll = bDoSaveAll;

  long FltCnt = 0;
  const long TtlFltCnt = FltSlotNos.GetLen();
  long RecCnt = 0;
  const long TtlRecCnt = RecSlotNos.GetLen();

  if (FirstBlock)
    {

    lEvSlotNoCnt = 0;
    bDoneWrite = False;
    if (TtlRecCnt>1)
      RecSlotNos.Sort();

    if (bJustStarted)
      {
      bJustStarted = False;
#if !WhatWasThis
      dMidTime = dTime - m_StopGap;
      dPrevTime = dMidTime - m_StopGap;
#endif
      //dTime = gs_Exec.TheTime.Seconds - m_StopGap;
      //pCurEvBlk->Start(dTime); //put start StopGap seconds in past
      }
#if WhatWasThis
    dPrevTime = dTime;
    dTime = gs_Exec.TheTime.Seconds;
    double Total2Add=0.0;
    while (dTime+Total2Add<=dPrevTime+m_StopGap*0.01)
      Total2Add += m_StopGap;
    if (0)
      {
      if (Total2Add>0)
        dbgpln("  Hist  Total2Add %12.6f",Total2Add);
      }

    if (Total2Add>=CTimeValue::MinResolution)
      LogWarning("History", 0, "Time advanced by %i!",int(Total2Add*1000));

    dTime += Total2Add;
    dMidTime = dPrevTime + ((dTime - dPrevTime) * m_MidTimeFactor);

    if (0)
      dbgpln("  Hist  %18.6f > %18.6f [%+12.6f]",dPrevTime,dTime,dTime-dPrevTime);
#endif
//LogNote("EO Write",0,"Hist  %18.6f --> %18.6f [%+12.6f]",dPrevTime,dTime,dTime-dPrevTime);

    if (dTime<dPrevTime)
      {
      dPrevTime = dTime;
      dMidTime = dTime + 0.001;
      dTime += 0.002;
      bDoneWrite = True; //CNM BUG 4 : received execute at earlier time, time travel backwards ????
      LogWarning("History", 0, "Received EO_Execute at earlier time!");
      return False;
      }

    if (dTime==dPrevTime)
      {
      bDoneWrite = True; //CNM BUG 3 : received execute at same time as previous execute ???
      //LogWarning("History", 0, "Received EO_Execute at same time!");
      return False;
      }

    if (bDoneWrite)
      {//CNM BUG 1 : tried to write more than once per iteration, historian gets upset,
       //caused by rebuilding/changing subscription list ???
      LogWarning("History", 0, "Multi Write!");
      return True; //do nothing
      }
    if (dMidTime>=dTime)
      {//CNM BUG 2 : this does happen sometimes !!! startup and when scan time is small??? because stopgap is small!?!?
      bDoneWrite = True;
      LogWarning("History", 0, "MidTime >= Time!");
      return True;
      dMidTime = dTime + 0.001;
      dTime += 0.002;
      }
    }
  else
    {
    if (lEvSlotNoCnt>0 && !MustDoSaveAll)
      {
      while (FltCnt < TtlFltCnt && FltSlotNos[FltCnt] < EvSlotNos[lEvSlotNoCnt-1])
        FltCnt++;
      while (RecCnt < TtlRecCnt && RecSlotNos[RecCnt] < EvSlotNos[lEvSlotNoCnt-1])
        RecCnt++;
      }
    }

  if (bDoneWrite)
    {//CNM BUG 1 : tried to write more than once per iteration, historian gets upset,
     //caused by rebuilding/changing subscription list ???
    LogWarning("History", 0, "Multi Write!");
    return True; //do nothing
    }

  //ASSERT(dMidTime<dTime);
  while (XM.MsgAvail() && !bSwitchedOff)
    {
    CXM_ObjectData* pX = XM.ObjectData();
    CPkDataItem* pItem = pX->FirstItem();
    long SNo = pX->Index;

    //TODO: method for checking FltSlotNos and RecSlotNos should be combined to ensure correct sequence!

    //check if due to filtering, a value must be sent to historian...
    if (FltCnt < TtlFltCnt && !MustDoSaveAll)
      {
      while (FltCnt < TtlFltCnt && FltSlotNos[FltCnt] < SNo)
        {
        ASSERT(FltSlotNos[FltCnt] < Data.lSlotsUsed);
        CSlot* pSlot = Slots[FltSlotNos[FltCnt++]];
        if (pSlot->Data.bRecordIt)
          {
          pSlot->MeasVal = pSlot->Data.PrevActualVal;
          pSlot->Changed();
          }
        //dbgpln("  Slot:%d  ID:%d  FILTER SNo:%d", FltSlotNos[FltCnt], Slots[FltSlotNos[FltCnt]]->ID(), SNo);
        }
      if (FltCnt < TtlFltCnt && SNo==FltSlotNos[FltCnt])
        FltCnt++;
      }

    //check if a value in RecSlotNos must be sent to historian...
    if (RecCnt < TtlRecCnt && !MustDoSaveAll)
      {
      while (RecCnt < TtlRecCnt && RecSlotNos[RecCnt] < SNo)
        {
        ASSERT(RecSlotNos[RecCnt] < Data.lSlotsUsed);
        Slots[RecSlotNos[RecCnt++]]->Changed(True);
        }
      if (RecCnt < TtlRecCnt && SNo==RecSlotNos[RecCnt])
        RecCnt++;
      }

    if (IsData(pItem->Type()) && SNo < Data.lSlotsUsed)
      {
      #if dbgHistorian
      if (dbgWriteData())
        dbgpln("HistWrite:%3d %15.3f  %g %s", SNo, dTime, pItem->Value()->GetDouble(), Slots[SNo]->Tag());
      #endif
      //double Value = pItem->Value()->GetDouble();
      Slots[SNo]->bIsRecording = True;
      Slots[SNo]->dwChangeCnt++;
      if (Slots[SNo]->Data.bRecordIt)
        {
        Slots[SNo]->MeasVal.Set(*(pItem->Value()));
        switch (pItem->Value()->Status())
          {//could do a bit left shift instead
          case ds_OK        : Slots[SNo]->iStatus = TS_STATUS_VALID; break;
          case ds_UnderFlow : Slots[SNo]->iStatus = TS_STATUS_UNDER; break;
          case ds_OverFlow  : Slots[SNo]->iStatus = TS_STATUS_OVER; break;
          case ds_NAN       : Slots[SNo]->iStatus = TS_STATUS_INVALID; break;
          default: ASSERT(FALSE); Slots[SNo]->iStatus = TS_STATUS_INVALID; //invalid status !!!
          }
        if (!MustDoSaveAll)
          Slots[SNo]->Changed();
        }
      //dbgpln("  Slot:%d  ID:%d  DATA", SNo, Slots[SNo]->ID());
      }
    }

  if (LastBlock && !bSwitchedOff)
    {
    if (!MustDoSaveAll)
      {
      //TODO: method for checking FltSlotNos and RecSlotNos should be combined to ensure correct sequence!
      while (FltCnt < TtlFltCnt)
        {
        ASSERT(FltSlotNos[FltCnt] < Data.lSlotsUsed);
        CSlot* pSlot = Slots[FltSlotNos[FltCnt++]];
        if (pSlot->Data.bRecordIt)
          {
          pSlot->MeasVal = pSlot->Data.PrevActualVal;
          pSlot->Changed();
          }
        }
      while (RecCnt < TtlRecCnt)
        {
        ASSERT(RecSlotNos[RecCnt] < Data.lSlotsUsed);
        Slots[RecSlotNos[RecCnt++]]->Changed(True);
        }
      }
    WriteSlots(MustDoSaveAll);
    bDoneWrite = True;
    }

  return True;
  }

//---------------------------------------------------------------------------

flag CHistorian::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
  const long MaxScanNo=(LONG_MAX-2)/2;

  if (m_bWasPaused)
    {
    m_bWasPaused=false;
    for (long SNo=0; SNo<Data.lSlotsUsed; SNo++)
      {
      CSlot* pSlot = Slots[SNo];
      if (pSlot->m_bChangedDuringPause)
        {
        pSlot->m_bChangedDuringPause=false;
        pSlot->Changed(true);
        }
      }
    }

  lScanNo++;

  #if dbgHistorian
  if (dbgExecute())
    dbgpln("HistExec :%3s %15.3f ", "", dTime);
  #endif

  const long lModulo=500;
  if (lScanNo % lModulo == lModulo/2)
    CopyFiles(); //copy to archive or delete old historian blocks

  if (lScanNo % lForceCnt == 0)
    bDoSaveAll = True;

  if (lScanNo > MaxScanNo)
    {
    for (long SNo=0; SNo<Data.lSlotsUsed; SNo++)
      {
      CSlot* pSlot = Slots[SNo];
      long Shft = Max(-MaxScanNo, pSlot->lScanOfLastChange - MaxScanNo);
      pSlot->lScanOfLastChange = Shft;
      Shft = Max(-MaxScanNo, pSlot->lScanOfLastSave - MaxScanNo);
      pSlot->lScanOfLastSave = Shft;
      }
    lScanNo -= MaxScanNo;
    }

  //COMBUG - ScdMainWnd()->PostMessage(WMU_HSTUPDATESTATS, 0, ++dwStatsCnt);
  return False;
  }

//---------------------------------------------------------------------------

flag CHistorian::EO_Stop(CXM_TimeControl &CB)
  {
  //dbgpln("Hist Stop Time A  %15.3f %s", dTime, gs_Exec.TheTime.Format(TD_DaysJoined|TD_Time)); 
  
  if (dTime != CB.m_TheTime.Seconds)
    {
    dPrevTime = dTime;
    dTime = CB.m_TheTime.Seconds;
    dMidTime = dPrevTime + ((dTime - dPrevTime) * m_MidTimeFactor);
    }
  if (dMidTime>=dTime || bDoneWrite)
    {
    dMidTime = dTime + m_StopGap;
    dTime = dMidTime + m_StopGap; //force last save to go .001 seconds into the future
    }

  //dbgpln("               B  %15.3f %s", dTime, gs_Exec.TheTime.Format(TD_DaysJoined|TD_Time)); 
  
  for (long i=0; i<Data.lSlotsUsed; i++)
    Slots[i]->bWriteMidTime = 0;
  lEvSlotNoCnt = 0;
  if (!bSwitchedOff)
    {
    WriteSlots(True); //force ALL values to the historian
    dMidTime = dTime + m_StopGap;
    dTime = dMidTime + m_StopGap; //force end to go another .001 seconds into the future
    pCurEvBlk->Stop(dTime);
    }
  bDoneWrite = True;
  for (i=0; i<Data.lSlotsUsed; i++)
    Slots[i]->bIsRecording = False;
  return True;
  }

//---------------------------------------------------------------------------

flag CHistorian::EO_BeginPause(CXM_TimeControl &CB)
  {
  return True;
  };

//---------------------------------------------------------------------------

flag CHistorian::EO_EndPause(CXM_TimeControl &CB)
  {
  return True;
  };

//---------------------------------------------------------------------------

flag CHistorian::EO_BeginSave(FilingControlBlock &FCB)
  {
  //if (gs_pPrj->bHstOpen) //TODO!!!
    {
    Cat.Save();
    Save();
    return True;
    }
  return False;
  }

//---------------------------------------------------------------------------

flag CHistorian::EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info)
  {
  if (bDoneConfig)
    {
    switch (Rqst.RQ_Type)
      {
      case RQ_Files:
        {
        if (Info.Count()==0)
          {
          Info.sData = sPath;
          Info.sData += sName;
          Info.sData += ".hed";
          Info.sData.FnContract();
          Info.bPreventCopy=true;
          return True;
          }
        if (Info.Count()==1)
          {
          Info.sData = sCatPath;
          Info.sData += sName;
          Info.sData += ".cat";
          Info.sData.FnContract();
          Info.bPreventCopy=true;
          return True;
          }
        if (bOpen)
          {
          Strng s((const char*)sDataPath);
          s += Data.sScenName;
          s += ".*";
          int i = 0;
          WIN32_FIND_DATA fd;
          HANDLE H = FindFirstFile(s(), &fd);
          flag AllDone = (H==INVALID_HANDLE_VALUE);
          while (!AllDone)
            {
            if (Info.Count()-2==i)
              {
              Info.sData = sDataPath;
              Info.sData += fd.cFileName;
              Info.sData.FnContract();
              Info.bPreventCopy=true;
              FindClose(H);
              return True;
              }
            i++;
            AllDone = !FindNextFile(H, &fd);
            }
          FindClose(H);
          }
        break;
        }
      }
    }
  return False;
  }

//---------------------------------------------------------------------------

int CHistorian::EO_QueryChangeTag(pchar pOldTag, pchar pNewTag)
  {//will I allow the tag to be changed...
  return EOCT_NOTFOUND;
  }

//---------------------------------------------------------------------------

int CHistorian::EO_ChangeTag(pchar pOldTag, pchar pNewTag)
  {//a tag has been changed, change historian slot...
  //Lock();
  int Err = EOCT_NOTFOUND;
  const size_t len = strlen(pOldTag);
  for (long i=0; i<Data.lSlotsUsed; i++)
    if (strlen(Slots[i]->Data.sTag)>=len && _strnicmp(pOldTag, Slots[i]->Data.sTag, len)==0)
      {
      SlotMap.RemoveKey(Slots[i]->Data.sTag);
      Strng ss = Slots[i]->Data.Tag();
      Strng s = pNewTag;
      s += ss.Mid(len, 256);
      Slots[i]->Data.SetTag(s());
      Err = EOCT_DONE;
      SlotMap.SetAt(Slots[i]->Data.sTag, i);
      }
  if (Err==EOCT_DONE)
    {
    Save(-1);
    XBuildMyDataLists();
    }

  //Release();
  return Err;
  }

//---------------------------------------------------------------------------

int CHistorian::EO_CanClose(Strng_List & Problems)
  {
  if (pQueryList)
    {
    CancelQueries();
    LogWarning("History", 0, "Historian queries have been canceled.");
    return EO_CanClose_Wait;
    }
  return EO_CanClose_Yes;
  }

//---------------------------------------------------------------------------
#else
//---------------------------------------------------------------------------

DWORD CHistorian::XSendMessage(CXMsgLst &XM, CXM_Route &Route)
  {
  //do something with data that needs to be sent...
  return 0;
  }

#endif

//---------------------------------------------------------------------------

void CHistorian::ResetStats()
  {
  dwStatsCnt = 0;
  for (long i=0; i<Data.lSlotsUsed; i++)
    {
    Slots[i]->dwWriteCnt = 0;
    Slots[i]->dwChangeCnt = 0;
    }
  }

//---------------------------------------------------------------------------

void CHistorian::FetchStats(char* Filename)
  {
  if (dwStatsCnt<1)
    return;
  CFileException e;
  CStdioFile f;
  if ( !f.Open(Filename, CFile::modeCreate | CFile::modeWrite, &e) )
    {
    dbgpln("File %s could not be opened, error:%i", Filename, e.m_cause);
    LogError("History", 0, "Statistics could not be written to '%s'", Filename);
    ResetStats();
    return;
    }
  char Buff[1024];
  sprintf(Buff, "Tag,Write,Change,Write_Change_Per,TtlCnt,Write_Ttl_Per,Change_Ttl_Per\n");
  f.WriteString(Buff);
  for (long i=0; i<Data.lSlotsUsed; i++)
    {
    double d = 0.0;
    if (Slots[i]->dwChangeCnt>0)
      d = (double)Slots[i]->dwWriteCnt / Slots[i]->dwChangeCnt * 100.0;
    sprintf(Buff, "%s,%d,%d,%6.2f,%d,%6.2f,%6.2f\n", Slots[i]->Data.sTag,
             Slots[i]->dwWriteCnt,
             Slots[i]->dwChangeCnt,
             d,
             dwStatsCnt,
             (double)Slots[i]->dwWriteCnt / dwStatsCnt * 100.0,
             (double)Slots[i]->dwChangeCnt / dwStatsCnt * 100.0);
    f.WriteString(Buff);
    }
  sprintf(Buff, "\n\nHistorian catalog summary [%d Items in %s]:\nRec No,Cat No,File,Start Time,End Time,Duration\n", Cat.iCatItemCnt, (const char*)Cat.sFilename);
  f.WriteString(Buff);
  CString s;
  for (i=0; i<Cat.iCatItemCnt; i++)
    {
    pCCatItem p = Cat.CatItems[i];
    s = Data.sScenName;
    s += '.';
    s += CCatItem::CatNoStr(p->Data.iCatNo);
    if (CCatItem::FixedDataExtn())
      s += CCatItem::m_sFilenameExtn;
    sprintf(Buff, "%d,%d,%s,%g,%g,%g\n", p->lRecNo, p->Data.iCatNo, (const char*)s, p->Data.dStartTime, p->Data.dLastTime, p->Data.dLastTime - p->Data.dStartTime);
    f.WriteString(Buff);
    }
  f.Close();
  ResetStats();
  }

//---------------------------------------------------------------------------

flag CHistorian::SlotInfoDlg(char* pTag, byte iDlgNo)
  {
#if ForSysCAD
  switch (iDlgNo)
    {
    case 0:
      {
      CHstSlotInfoDlg Dlg(pTag);
      Dlg.DoModal();
      return true;
      }
    case 1:
      {
      long SlotIndex = GetSlot(pTag);
      if (SlotIndex>=0)
        {
        CHstSlotDlg Dlg(Slots[SlotIndex]);
        Dlg.DoModal();
        return true;
        }
      break;
      }
    default: ASSERT(0);
    }
#endif
  return false;
  }

//===========================================================================
#if ForSysCAD

CHistSettings::CHistSettings()
  {
  sHistorianName      = "Hist_Cfg";
  sHstScenName        = "Hist";
  sHstDataDir         = PrjFilesAlias();
  sHstCatDir          = PrjFilesAlias();
  lHstEvFileSize      = 640;
  lHstSlotNoEstimate  = 1000;
  iHstFilesMin        = 3;
  iHstFilesMax        = 3;
  lHstForceCnt        = 600;
  dHstRecordATol      = 0.0001;
  dHstRecordRTol      = 0.0001;
  dHstStopGap         = 0.001;
  dHstMidTimeFactor   = 99.0/100;//CNM 59.0/60.0;
  bHstUseTol          = 1;
  bHstShowQueryInfo   = 1;
  bHstFixedDataExtn   = 1;
  
  m_OnRqd             = 0;
  };

//---------------------------------------------------------------------------

CHistSettings::CHistSettings(const CHistSettings & Sets)
  {
  *this=Sets;
  };

//---------------------------------------------------------------------------

CHistSettings & CHistSettings::operator=(const CHistSettings & Sets)
  {
  sHistorianName      = Sets.sHistorianName;
  sHstScenName        = Sets.sHstScenName;
  sHstDataDir         = Sets.sHstDataDir;
  sHstCatDir          = Sets.sHstCatDir;
  lHstEvFileSize      = Sets.lHstEvFileSize;
  lHstSlotNoEstimate  = Sets.lHstSlotNoEstimate;
  iHstFilesMin        = Sets.iHstFilesMin;
  iHstFilesMax        = Sets.iHstFilesMax;
  lHstForceCnt        = Sets.lHstForceCnt;
  dHstRecordATol      = Sets.dHstRecordATol;
  dHstRecordRTol      = Sets.dHstRecordRTol;
  dHstStopGap         = Sets.dHstStopGap;
  dHstMidTimeFactor   = Sets.dHstMidTimeFactor;
  bHstUseTol          = Sets.bHstUseTol;
  bHstShowQueryInfo   = Sets.bHstShowQueryInfo;
  bHstFixedDataExtn   = Sets.bHstFixedDataExtn;
  m_OnRqd             = Sets.m_OnRqd;
  return *this;
  };

//---------------------------------------------------------------------------

void CHistSettings::ReadSettings(CProfINIFile & PF)
  {
  sHistorianName      = PF.RdStr("Historian", "Name", sHistorianName());
  sHstScenName        = PF.RdStr("Historian", "ScenarioName", sHstScenName());
  sHstDataDir         = PF.RdStr("Historian", "DataDirectory", PrjFilesAlias());
  sHstCatDir          = PF.RdStr("Historian", "CatDirectory", PrjFilesAlias());
  lHstEvFileSize      = PF.RdLong("Historian", "FileSize", lHstEvFileSize);
  lHstSlotNoEstimate  = PF.RdLong("Historian", "SlotNoEstimate", lHstSlotNoEstimate);
  m_OnRqd             = PF.RdLong("Historian", "On", 1) != 0;
  iHstFilesMin        = PF.RdInt("Historian", "FilesMin", iHstFilesMin);
  iHstFilesMax        = Max((UINT)PF.RdInt("Historian", "FilesMax", iHstFilesMax), iHstFilesMin);
  lHstForceCnt        = PF.RdLong("Historian", "ForceCount", lHstForceCnt);
  bHstShowQueryInfo   = PF.RdLong("Historian", "ShowQueryInfo", bHstShowQueryInfo) != 0;
  bHstFixedDataExtn   = PF.RdLong("Historian", "FixedDataExtn", 0) != 0;
  bHstUseTol          = PF.RdLong("Historian", "UseRecordingTolerance", bHstUseTol) != 0;
  dHstRecordATol      = PF.RdDouble("Historian", "RecordingAbsTolerance", dHstRecordATol);
  dHstRecordRTol      = PF.RdDouble("Historian", "RecordingRelTolerance", dHstRecordRTol);
  dHstStopGap         = PF.RdDouble("Historian", "StopGap", dHstStopGap);
  dHstMidTimeFactor   = PF.RdDouble("Historian", "MidTimeFactor", dHstMidTimeFactor);


  //gs_HstMngr.SetHstOn(PF.RdLong("Historian", "On", 1) != 0);
  }

//---------------------------------------------------------------------------

void CHistSettings::WriteSettings(CProfINIFile & PF)
  {
  PF.WrStr("Historian", "Name", sHistorianName());
  PF.WrStr("Historian", "ScenarioName", sHstScenName());
  PF.WrStr("Historian", "DataDirectory", sHstDataDir());
  PF.WrStr("Historian", "CatDirectory", sHstCatDir());
  PF.WrLong("Historian", "FileSize", lHstEvFileSize);
  PF.WrLong("Historian", "SlotNoEstimate", lHstSlotNoEstimate);
  //PF.WrLong("Historian", "RealTime", gs_Exec.RealTime());
  PF.WrLong("Historian", "On", m_OnRqd);
  PF.WrInt("Historian", "FilesMin", iHstFilesMin);
  PF.WrInt("Historian", "FilesMax", iHstFilesMax);
  PF.WrLong("Historian", "ForceCount", lHstForceCnt);
  //PF.WrDouble("Historian", "TimeMax", gs_Exec.dTimeMax());
  PF.WrLong("Historian", "ShowQueryInfo", bHstShowQueryInfo);
  PF.WrLong("Historian", "FixedDataExtn", bHstFixedDataExtn);
  PF.WrLong("Historian", "UseRecordingTolerance", bHstUseTol);
  PF.WrDouble("Historian", "RecordingAbsTolerance", dHstRecordATol);
  PF.WrDouble("Historian", "RecordingRelTolerance", dHstRecordRTol);
  PF.WrDouble("Historian", "StopGap", dHstStopGap);
  PF.WrDouble("Historian", "MidTimeFactor", dHstMidTimeFactor);

  }

//===========================================================================

CHistorianMngr::CHistorianMngr()
  {
  pExecObj = NULL;
  //bHstOn = 0;
  bHstOpen = 0;

  lLicMaxHistSizeAllowed = 640;
  lLicMaxHistFilesAllowed = 3;
  }

//---------------------------------------------------------------------------

void CHistorianMngr::CreateTheHistEO()
  {
  ASSERT(pHistory==NULL);
  pHistory = new CHistorian;
  }

//---------------------------------------------------------------------------

void CHistorianMngr::DestroyTheHistEO()
  {
  ASSERT(pHistory);
  delete pHistory;
  pHistory = NULL;
  }

//---------------------------------------------------------------------------

void CHistorianMngr::SetLicensingOptions(long LicMaxHistSizeAllowed, UINT LicMaxHistFilesAllowed)
  {
  lLicMaxHistSizeAllowed = LicMaxHistSizeAllowed;
  lLicMaxHistFilesAllowed = LicMaxHistFilesAllowed;
  }

//---------------------------------------------------------------------------

bool CHistorianMngr::DoCfgHistorian()
  {
  Strng Drv, Dir, File;
  Drv.FnDrive(sHistorianName());
  Dir.FnDir(sHistorianName());
  File.FnName(sHistorianName());
  Strng FnD;
  FnD.FnDrivePath(sHistorianName());
  if (FnD()==NULL)
    FnD = PrjFiles();

  FnD.FnContract();

  //create historian directories if required...
  Strng Fn;
  for (int i=0; i<(iHstFilesMax>iHstFilesMin ? 3 : 2); i++)
    {
    switch (i)
      {
      case 0: Fn = FnD(); break;
      case 1: Fn = sHstDataDir(); break;
      case 2: Fn = sHstCatDir(); break;
      }
    Fn.FnExpand();
    Fn.FnClearEndBSlash();
    if (!FileExists(Fn()))
      {
      if (CreateDirectory(Fn(), NULL))
        LogNote("History", 0, "Created folder '%s'", Fn());
      else
        {
        LogError("History", LF_Exclamation, "Unable to create folder '%s'", Fn());
        if (i<2)
          return false;
        }
      }
    }

  if (DefNetProbalMode())
    {
    dHstStopGap = 0.001;
    }
  else
    {
    const double StpSz = gs_Exec.StepSizeMax.Seconds;
    //dHstStopGap = Range(StpSz*0.0001, dHstStopGap, StpSz*0.1);
    dHstStopGap = Range(/*StpSz**/0.0001, dHstStopGap, StpSz*0.1);
    }

  Strng DD(sHstDataDir);
  Strng CD(sHstCatDir);
  DD.FnExpand();
  CD.FnExpand();
  FnD.FnExpand();
  ASSERT(pHistory);
  pHistory->Configure(FnD(), DD(), CD(), File(), lLicMaxHistSizeAllowed, lLicMaxHistFilesAllowed,
                      iHstFilesMin, iHstFilesMax, 300, lHstForceCnt,
                      bHstShowQueryInfo && 0, //COMBUG
                      bHstFixedDataExtn, bHstUseTol, dHstRecordATol, dHstRecordRTol, dHstStopGap, dHstMidTimeFactor);
  return true;
  }

//---------------------------------------------------------------------------

bool CHistorianMngr::DoOpenHistorian(bool ShowBusyDlg/*=false*/)
  {
  if (sHistorianName.Length()==0 || sHstScenName.Length()==0)
    {
    LogError("History", LF_Exclamation, "Historian name and scenario name must be specified");
    return false;
    }
  if (sHstScenName.Length()>8)
    {
    LogWarning("History", 0, "Historian scenario name truncated. Should be 8 or less characters.");
    sHstScenName = sHstScenName.Left(8);
    }
  if (_stricmp(sHistorianName(), sHstScenName())==0)
    {
    LogError("History", LF_Exclamation, "Historian name and scenario name must NOT be the same");
    return false;
    }
  if (!DoCfgHistorian())
    return false;

  if (ShowBusyDlg)
    CDlgBusy::Open("\nOpening Historian");
  byte RetCode = pHistory->Open(sHstScenName(), lHstEvFileSize*1024, lHstSlotNoEstimate, lLicMaxHistSizeAllowed, ShowBusyDlg);
  bHstOpen = (RetCode==0);
  if (RetCode==HST_ERR_NOTEXISTS_HED || RetCode==HST_ERR_NOTEXISTS_CAT)
    {
    flag OK = pHistory->Create(); //create default historian
    if (OK)
      {
      RetCode = pHistory->Open(sHstScenName(), lHstEvFileSize*1024, lHstSlotNoEstimate, lLicMaxHistSizeAllowed, ShowBusyDlg);
      bHstOpen = (RetCode==0);
      }
    }
  if (ShowBusyDlg)
    CDlgBusy::Close();
  switch (RetCode)
    {
    case 0:
      //gs_Exec.SetTime(gs_HstMngr.GetLastTime());// + 0.001);// CNM 10.0);
      return true;
      break;
    case HST_ERR_OPENHED:
    case HST_ERR_NOTEXISTS_HED:
      LogError("History", LF_Exclamation, "Header File not Opened");
      break;
    case HST_ERR_OPENCAT:
    case HST_ERR_NOTEXISTS_CAT:
      LogError("History", LF_Exclamation, "Catalogue File not Opened");
      break;
    case HST_ERR_OPENEVBLK:
      LogError("History", LF_Exclamation, "Event Block not Opened");
      break;
    case HST_ERR_OPENEVBLK_CREATE:
      LogError("History", LF_Exclamation, "Event Block not created during open");
      break;
    default :
      LogError("History", LF_Exclamation, "Error %i occured while opening", RetCode);
      break;
    }
  return false;
  }

//---------------------------------------------------------------------------

flag CHistorianMngr::DoClose()
  {
  bHstOpen = 0;
  return pHistory->Close();
  }

//---------------------------------------------------------------------------

bool CHistorianMngr::DoRestart()
  {
  if (!HstOpen() && sHistorianName.Length()>0)
    {
    CWaitCursor Wait;
    //CDlgBusy::Open("\nRestart Historian");
    if (DoCfgHistorian() && pHistory->Restart(sHstScenName()))
      {
      pHistory->DeleteDataFiles();
      return true;
      }
    //CDlgBusy::Close();
    }
  return false;
  }

//---------------------------------------------------------------------------

void CHistorianMngr::DoDeleteDataFiles()
  {
  pHistory->DeleteDataFiles();
  }

//---------------------------------------------------------------------------

bool CHistorianMngr::DoDeleteHistorian()
  {
  if (!HstOpen() && sHistorianName.Length()>0)
    {
    //CDlgBusy::Open("\nDelete Historian");
    CWaitCursor Wait;
    if (DoCfgHistorian())
      {//delete all historian related files...
      pHistory->DeleteHistorian();
      return true;
      }
    //CDlgBusy::Close();
    }
  return false;
  }

//---------------------------------------------------------------------------

void CHistorianMngr::DoFetchStats(char* Filename)
  {
  if (bHstOpen && pHistory)
    pHistory->FetchStats(Filename);
  }

//---------------------------------------------------------------------------

void CHistorianMngr::DoResetStats()
  {
  if (bHstOpen && pHistory)
    pHistory->ResetStats();
  }

//---------------------------------------------------------------------------

bool CHistorianMngr::KeepHistoryFile(char* pFilename)
  {//add list of tags in specified file to list of tags to be recorded
  if (bHstOpen && pHistory && bHstOpen && pFilename)
    {
    CXM_Route HRoute;
    if (pExecObj->XFindObject(pExecName_Historian, HRoute))
      {
      CXM_KeepHistoryFile * xb = new CXM_KeepHistoryFile(pFilename);
      CXMsgLst XM;
      XM.PackMsg(xb);
      DWORD RetCode = pExecObj->XSendMessage(XM, HRoute);
      return true;
      }
    }
  return false;
  }

//---------------------------------------------------------------------------

DWORD CHistorianMngr::HistoryExists(char* pTag)
  {
  DWORD RetCode = 0;
  if (bHstOpen && pHistory && bHstOpen && pTag)
    {
    CXM_Route HRoute;
    if (pExecObj->XFindObject(pExecName_Historian, HRoute))
      {
      CXM_HistoryExists *xb=new CXM_HistoryExists (0, pTag);
      CXMsgLst XM;
      XM.PackMsg(xb);
      RetCode = pExecObj->XSendMessage(XM, HRoute);
      }
    }
  return RetCode;
  }

//---------------------------------------------------------------------------

void CHistorianMngr::BrowseTags()
  {
  if (bHstOpen && pHistory && bHstOpen)
    {
    CXM_Route HRoute;
    if (pExecObj->XFindObject(pExecName_Historian, HRoute))
      {
      CProfINIFile PF(PrjIniFile());
      Strng sPrevTag;
      sPrevTag = PF.RdStr("Historian", "PrevBrowseTag", "");
      CXM_HistorySlotDlg *xb=new CXM_HistorySlotDlg (sPrevTag() ? sPrevTag() : "", 0);
      CXMsgLst XM;
      XM.PackMsg(xb);
      pExecObj->XSendMessage(XM, HRoute);
      }
    }
  }

//---------------------------------------------------------------------------

void CHistorianMngr::QueryToFile(char* pFile, double StartTime, double Duration, double Interval)
  {
  double EndTime = StartTime + Duration;
  long SrcID = (long)pExecObj;//gs_pPrj;
  byte Opt = 1;//enum QueryOption { All, AveEqualySpaced, LastEqualySpaced };
  byte TimeOptUnits = 2;//1=Seconds;2=Minutes;3=Hours;4=Days
  flag TimeOptFull = 1;
  flag Headings = 0;
  long NoPts = (long)(Duration/Interval)+1;
  double Invalid = dNAN;
  double RepTimeOffset = 0.0;
  //todo check if pFile path is valid
  //todo strip .xxx extension if found
  Strng Filename = pFile;
  Filename += "_values.csv";
  Strng FilenameTags = pFile;
  FilenameTags += "_tags.csv";
  byte FileMode = 1;
  if (bHstOpen && pHistory && bHstOpen)
    {
    //CQueryDlg* pDlg = new CQueryDlg(pTrnd, this, ST, ET);
    CXM_Route HRoute;
    if (pExecObj->XFindObject(pExecName_Historian, HRoute))
      {
      CXM_QueryHistoryOther *xb=new CXM_QueryHistoryOther(StartTime, EndTime, SrcID, Opt, TimeOptUnits, TimeOptFull, Headings, NoPts, Invalid, 4/*FileDirect*/, Filename(), FilenameTags(), FileMode, RepTimeOffset);
      CXMsgLst XM;
      XM.PackMsg(xb);
      XM.PackMsg(new CXM_Route(HRoute));
      pExecObj->XSendMessage(XM, HRoute);
      }
    }
  }

//---------------------------------------------------------------------------

bool CHistorianMngr::LicenseExceeded(long LicMaxHistSizeAllowed, UINT LicMaxHistFilesAllowed)
  {
  return (pHistory &&
          //bHstOn &&
          bHstOpen &&
          (pHistory->iFilesMin!=pHistory->iFilesMax ||
           pHistory->iFilesMax>LicMaxHistFilesAllowed ||
           pHistory->Data.lReqdBlkSize>LicMaxHistSizeAllowed*1024L));
  }

//---------------------------------------------------------------------------

double CHistorianMngr::GetLastTime()
  {
  return pHistory->GetLastTime();
  }

//---------------------------------------------------------------------------

bool CHistorianMngr::IsOn()
  {
  return (bHstOpen && pHistory);
  }

//---------------------------------------------------------------------------

bool CHistorianMngr::IsOpen()
  {
  return (bHstOpen && pHistory && bHstOpen);
  }

//---------------------------------------------------------------------------

CHistorianMngr gs_HstMngr; //the global class for historian access

#endif
//===========================================================================
