//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#if ForSysCAD
#include "sc_defs.h"
#include "errorlog.h"
#endif
#define __HSTBLKS_CPP
#include "hstblks.h"
#include "hstmain.h"

#define  dbgHistorian  0

#if dbgHistorian
#include "dbgmngr.h"
static CDbgMngr dbgHistBlkTag("Historian", "HistBlkTag");
#endif

//===========================================================================

IMPLEMENT_MEMSTATS(CEvBlkFileInfo)
IMPLEMENT_MEMSTATS(CEvBlk)

//===========================================================================

CEvBlk::CEvBlk()
  {
  Initialise();
  }

//---------------------------------------------------------------------------

void CEvBlk::Initialise()
  {
  pCatItem    = NULL;
  pInfo       = NULL;
  pEvHead     = NULL;
  pTS         = NULL;
  pFirstTS    = NULL;
  pPrevTS     = NULL;
  pPrevDeltaTime = NULL;
  pData       = NULL;
  lTtlLen     = 0;
  }

//---------------------------------------------------------------------------

flag CEvBlk::Open(WORD CatNo)
  {
  return Open(pHistory->Cat.Find(CatNo));
  }

//---------------------------------------------------------------------------

flag CEvBlk::Open(CCatItem* CatItem)
  {
  if (CatItem==NULL)
    return false;
  pCatItem = CatItem;

  if (!pHistory->pNxtBlkFile)
    return false;

  InitializeCriticalSection(&Lock);

  pInfo = pHistory->pNxtBlkFile;
  pHistory->pNxtBlkFile = NULL;
  const BOOL IsNew = (memcmp((void*)pInfo->pBlkMap, (void*)NewBlkKey, strlen(NewBlkKey)) == 0);

  pEvHead = (pHD_EvBlkHead)pInfo->pBlkMap;
  lTtlLen = sizeof(HD_EvBlkHead);

  LockEvBlk();
  if (!IsNew)
    {//old block...
    //setup all flags pointers etc so that next event will be added to end of the file...
    if (pEvHead->iVerNo < 3)
      {//ERROR!!! close block...
      LogError("History", 0, "Historian version number mismatch in %s", (const char*)(pInfo->sFilename));
      FreeEvBlk();
      DeleteCriticalSection(&Lock);
      pCatItem = NULL;
      return false;
      }
    pPrevTS = NULL;
    pFirstTS = NULL;
    lTtlLen = sizeof(HD_EvBlkHead);
    if (pEvHead->iTimeSliceCnt>0)
      {
      pFirstTS = (pHD_TimeSliceHead)&pInfo->pBlkMap[lTtlLen];
      pPrevTS = pFirstTS;
      for (long i=0; i<pEvHead->iTimeSliceCnt; i++)
        {
        pTS = (pHD_TimeSliceHead)&pInfo->pBlkMap[lTtlLen];
        lTtlLen += sizeof(HD_TimeSliceHead);
        lTtlLen += pTS->iLen;
        if (i == pEvHead->iTimeSliceCnt-1)
          pPrevTS = pTS;
        if (lTtlLen>pInfo->dwFileAllocLen)
          {//ERROR!!! close block...
          LogError("History", 0, "Corrupt data file, delete historian data files!");
          FreeEvBlk();
          DeleteCriticalSection(&Lock);
          pCatItem = NULL;
          return false;
          }
        }
      }
    pTS = NULL;
    }
  else
    {//new block...
    pEvHead->bFinished = 0;
    pEvHead->bReducedSize = 0;
    pEvHead->dStartTime = 0.0;
    pEvHead->dLastTime = 0.0;
    pEvHead->iTimeSliceCnt = 0;
    pEvHead->iCatNo = pCatItem->CatNo();
    pEvHead->iVerNo = HstVerNo;
    pEvHead->iJmpCount = 0;
    for (int i=0; i<MaxJmps; i++)
      {
      pEvHead->JmpPos[i] = 0;
      pEvHead->JmpTimes[i] = 0.0;
      }
    memset(pEvHead->sSpare, 0, sizeof(pEvHead->sSpare));
    strncpy(pEvHead->sScenName, pCatItem->Data.sScenName, sizeof(pEvHead->sScenName));
    memcpy(pEvHead->sCopyNotice, CopyNotice, sizeof(pEvHead->sCopyNotice));
    FlushViewOfFile((void*)pEvHead, 0);
    }

  dJmpDelta = pInfo->dwFileAllocLen / MaxJmps;
  FreeEvBlk();
  return true;
  }

//---------------------------------------------------------------------------

void CEvBlk::Close()
  {
  LockEvBlk();
  if (pTS)
    EndTimeSlice();
  UnmapViewOfFile(pInfo->pBlkMap);
  if (!CloseHandle(pInfo->hBlkFileMap))
    LogError("Hst", 0, "Close Handle C %s", FindWinError(GetLastError()));
  pInfo->hBlkFileMap = NULL;
  FreeEvBlk();

  if (!CloseHandle(pInfo->hBlkFile))
    LogError("Hst", 0, "Close Handle D %s", FindWinError(GetLastError()));
  pInfo->hBlkFile = NULL;

  DeleteCriticalSection(&Lock);

  //remove link to catalog...
  pCatItem = NULL;
  delete pInfo;
  pInfo = NULL;
  }

//---------------------------------------------------------------------------

void CEvBlk::MarkAsFinished()
  {
  LockEvBlk();
  if (pTS)
    EndTimeSlice();

  pCatItem->UpdateTime(this);
  pEvHead->bFinished = true;
  FreeEvBlk();
  }

//---------------------------------------------------------------------------

void CEvBlk::StartTimeSlice(double time)
  {
  LockEvBlk();
  pTS = (pHD_TimeSliceHead)&pInfo->pBlkMap[lTtlLen];
  pData = &pInfo->pBlkMap[lTtlLen + sizeof(HD_TimeSliceHead)]; //(pbyte)pTS + sizeof(HD_TimeSliceHead);
  pTS->dStartTime = time;
  pTS->dLastTime = time;
  pTS->iLen = 0;
  pTS->iNext = 0;
  pTS->iLastDeltaTime = 0;
  if (pPrevTS)
    {
    pTS->iPrev = (WORD)((byte*)pTS - (byte*)pPrevTS);
    pPrevTS->iNext = pTS->iPrev;
    }
  else
    pTS->iPrev = 0;
  if (pFirstTS==NULL)
    {
    pFirstTS = pTS;
    pEvHead->dStartTime = pTS->dStartTime;
    pEvHead->dLastTime = pTS->dStartTime;
    pEvHead->iTimeSliceCnt = 0;
    pCatItem->UpdateTime(this);
    }
  iDeltaTimeCnt = 0;
  pPrevDeltaTime = NULL;
  lNextID = 0;
  FreeEvBlk();
  }

//---------------------------------------------------------------------------

void CEvBlk::EndTimeSlice()
  {
  if (pTS)
    {
    LockEvBlk();
    pData[pTS->iLen++] = TS_END;
    lTtlLen += sizeof(HD_TimeSliceHead);
    lTtlLen += pTS->iLen;
    pPrevTS = pTS;
    pEvHead->dLastTime = pTS->dLastTime;
    pTS = NULL;
    pCatItem->UpdateTime(this);
    pEvHead->iTimeSliceCnt++;
    if (pEvHead->iVerNo>3)
      {
      if ((long)(lTtlLen/dJmpDelta)-1>=pEvHead->iJmpCount && pEvHead->iJmpCount<MaxJmps)
        {//add this TimeSlice to the quick lookup jump table
        pEvHead->JmpPos[pEvHead->iJmpCount] = (DWORD)((byte*)pPrevTS - (byte*)pEvHead);
        pEvHead->JmpTimes[pEvHead->iJmpCount] = pPrevTS->dStartTime;
        pEvHead->iJmpCount++;
        }
      }
    FlushViewOfFile((void*)pEvHead, 0);
    pHistory->Cat.Save(pCatItem->lRecNo);
    FreeEvBlk();
    }
  }

//---------------------------------------------------------------------------

void CEvBlk::Skip(DWORD DeltaID)
  {
  DeltaID--;
  if (DeltaID<=0x1F)
    {
    pData[pTS->iLen++] = (TS_SKIP_0 | (byte)DeltaID); 
    }
  else if (DeltaID<=0x1FFF)
    {
    pData[pTS->iLen++] = (TS_SKIP_1 | ((byte)DeltaID & 0x1F)); 
    pData[pTS->iLen++] = (byte)_lrotr(DeltaID, 5);
    }
  else if (DeltaID<=0x1FFFFF)
    {
    pData[pTS->iLen++] = (TS_SKIP_2 | ((byte)DeltaID & 0x1F)); 
    *((WORD*)&pData[pTS->iLen]) = (WORD)_lrotr(DeltaID, 5);
    pTS->iLen += 2;
    }
  else if (DeltaID<=0x1FFFFFFF)
    {
    *((DWORD*)&pData[pTS->iLen]) = (DWORD)_lrotr(DeltaID, 5);
    pData[pTS->iLen++] = (TS_SKIP_3 | ((byte)DeltaID & 0x1F)); 
    pTS->iLen += 3;
    }
  else
    {
    dbgpln("DeltaID in historian %d is to large !!!!!", DeltaID);
    VERIFY(FALSE);
    }
  }

//---------------------------------------------------------------------------

void CEvBlk::SkipTime(float DeltaTime)
  {
  WORD Prev = 0;
  if (pPrevDeltaTime)
    Prev = (WORD)(&pData[pTS->iLen] - pPrevDeltaTime);
  pPrevDeltaTime = &pData[pTS->iLen];
  if (Prev==0)
    {
    pData[pTS->iLen++] = TS_TIMESKIP;
    *((float*)&pData[pTS->iLen]) = DeltaTime;
    pTS->iLen += sizeof(float);
    }
  else if (Prev<=0xFF)
    {
    pData[pTS->iLen++] = TS_TIMESKIP_1;
    *((float*)&pData[pTS->iLen]) = DeltaTime;
    pTS->iLen += sizeof(float);
    pData[pTS->iLen++] = (byte)Prev;
    }
  else if (Prev<=0xFFFF)
    {
    pData[pTS->iLen++] = TS_TIMESKIP_2;
    *((float*)&pData[pTS->iLen]) = DeltaTime;
    pTS->iLen += sizeof(float);
    *((WORD*)&pData[pTS->iLen]) = Prev;
    pTS->iLen += sizeof(WORD);
    }
  else
    {
    ASSERT(FALSE); //a delta time skip should never be so big ???
    }
  pTS->iLastDeltaTime = (WORD)((byte*)pPrevDeltaTime - (byte*)pTS);
  iDeltaTimeCnt++;
  lNextID = 0;
  }

//---------------------------------------------------------------------------

flag CEvBlk::AddEv(double time, long SNo, rDataUnion Val)
  {
  CSlot* pSlot = pHistory->GetSlotByIndex(SNo);

  #if dbgHistorian && DBGMANAGER
  if (dbgHistBlkTag())
    {
    if (dbgHistBlkTag.Items.Length()>0 && dbgHistBlkTag.Items.Find(pSlot->Data.sTag)!=NULL)
      { //place breakpoint here to see what is happening with a specific tag specified in syscad.dmf
      dbgpln("Historian:%s AddEv:%12.3f; time:%15.3f", pSlot->Data.sTag, Val.GetDouble(), time);
      }
    }
  #endif

  if (pTS==NULL)
    {
    if ((pEvHead->bFinished) || ((pInfo->dwFileAllocLen - lTtlLen) < ((DWORD)pHistory->Data.lMaxTimeSliceLen * 2)))
      {
      pHistory->AddEvBlk(); //cannot add to this block, make new one
      return false;
      }
    StartTimeSlice(time);
    }
  double DeltaTime = Max(ChkZero(time - pTS->dLastTime), 0.0);
  if (DeltaTime==0.0 && lNextID==pSlot->ID()+1)
    {
    DeltaTime = 0.001; //two values for the same slot at the same time, should NEVER happen
    ASSERT(FALSE);//two values for the same slot at the same time, should NEVER happen
    }
  if (DeltaTime>0.0)
    {
    // cnm dbgpln("AddEv  :%13.3f   %13.3f", time, DeltaTime);
    pTS->dLastTime = time;
    if (iDeltaTimeCnt>=pHistory->Data.iMaxDeltaTimeCnt || pTS->iLen>=pHistory->Data.lMaxTimeSliceLen)
      {//force time slice to end !!!
      EndTimeSlice();
      if ((pInfo->dwFileAllocLen - lTtlLen) < ((DWORD)pHistory->Data.lMaxTimeSliceLen * 2))
        {
        pHistory->AddEvBlk(); //possibly not enough space for new timeslice...
        return false;
        }
      StartTimeSlice(time);
      }
    else
      {
      LockEvBlk();
      SkipTime((float)DeltaTime);
      FreeEvBlk();
      }
    }

  //ASSERT(pSlot->ID()>=lNextID); //slots expected in ID sequence OR
                                //sending values for the same time !
  if (pSlot->ID() < lNextID)
    {
    LogError("History", 0, "Slots expected in ID sequence, OR values for the same time are being sent for %s", pSlot->Tag());
    //ASSERT(FALSE); //slots expected in ID sequence OR sending values for the same time
    lNextID = pSlot->ID() + 1;
    return true;
    }

  LockEvBlk();
  DWORD DeltaID = (DWORD)(pSlot->ID() - lNextID);
  if (DeltaID>0)
    Skip(DeltaID);
  lNextID = pSlot->ID() + 1;
  
  //ASSERT(Val.Type()==pSlot->Typ()); //data type from message MUST match slot type
  if (Val.Type()!=pSlot->Typ())
    {
    LogError("History", 0, "Slot data type must match message for %s", pSlot->Tag());
    //ASSERT(FALSE);  //data type from message MUST match slot type
    ASSERT(IsNumData(pSlot->Typ())); //the slot type MUST be correct, if not the .hed file must be wrong !!!
    Val.SetTypeDouble(pSlot->Typ(), Val.GetDouble());
    pSlot->FixTyp();
    }
  
  pData[pTS->iLen++] = pSlot->CntrlByte();
  if (pSlot->TSFlag==TS_BIT)
    {
    if (Val.GetLong())
      pData[pTS->iLen-1] |= TS_BIT_ON;
    }
  else if (pSlot->TSFlag==TS_STR)
    {
    Val.CopyTo((void*)&pData[pTS->iLen]);
    pTS->iLen += (strlen(Val.pChar)+1);
    }
  else
    {
    Val.CopyTo((void*)&pData[pTS->iLen]);
    pTS->iLen += Val.TypeSize();
    }
  
  FreeEvBlk();
  return true;
  }

//---------------------------------------------------------------------------

void CEvBlk::Start(double time)
  {
  if (pTS)
    EndTimeSlice();
  StartTimeSlice(time);
  pData[pTS->iLen++] = TS_START;
  EndTimeSlice();
  }

//---------------------------------------------------------------------------

void CEvBlk::Stop(double time)
  {
  if (pTS)
    EndTimeSlice();
  StartTimeSlice(time);
  pData[pTS->iLen++] = TS_STOP;
  EndTimeSlice();
  }

//---------------------------------------------------------------------------
//===========================================================================

