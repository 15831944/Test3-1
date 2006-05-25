//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#if ForSysCAD
#include "sc_defs.h"
#include "errorlog.h"
#include "vectors.h"
#include "scd_wm.h"
#else
#include "hstvect.h"
#endif
#define  __HSTQUERY_CPP
#include "hstquery.h"
#include "hstqyinf.h"
//#include "optoff.h"

#define dbgHstQueryTiming 0
#if dbgHstQueryTiming
DWORD QryCnt1;
#endif
#if dbgHstQuery
#include "dbgmngr.h"
static CDbgMngr dbgHistQuery("Historian", "Query");
#endif


//===========================================================================

IMPLEMENT_DYNAMIC(CHstQueryException, CException);

//===========================================================================

IMPLEMENT_MEMSTATS(CHstQueryException)
IMPLEMENT_MEMSTATS(CQueryBlk)
IMPLEMENT_MEMSTATS(CQueryHist)
IMPLEMENT_MEMSTATS(CQuerySlice)

//===========================================================================
//Some CXM_ data blacks are VERY large, minimize the number of them allocated, 
//otherwise excessive stack space is used!

CQuerySlice::CQuerySlice(pCQueryHist Q, pCQueryBlk QB)
  {
  pQ = Q;
  pQB = QB;
  pData = NULL;
  pTS = NULL;
  pLockBlk = NULL;
  }

//---------------------------------------------------------------------------

CQuerySlice::~CQuerySlice()
  {
  if (pLockBlk)
    pLockBlk->FreeEvBlk(); //block might not get unlocked if an exception is thrown
  }

//---------------------------------------------------------------------------

void CQuerySlice::SkipTime()
  {
  switch (pData[lPos++] & 0xEF)
    {
    case TS_TIMESKIP_0 :
      dTime += *((float*)&pData[lPos]);
      break;
    case TS_TIMESKIP_1 :
      dTime += *((float*)&pData[lPos]);
      lPos += 1;
      break;
    case TS_TIMESKIP_2 :
      dTime += *((float*)&pData[lPos]);
      lPos += 2;
      break;
    default: //ASSERT(FALSE);
      {//error, assume hist data is corrupt!
      CHstQueryException* pE = new CHstQueryException();
      pE->lErrorNumber = 10L;
      THROW(pE);
      break;
      }
    }
  lPos += sizeof(float);
  lID = 0;
  lQIDCnt = 0;
  lQID = pQ->SlotIDs[0];
  }

//---------------------------------------------------------------------------

void CQuerySlice::SkipBackTime()
  {
  switch (pData[lPos++] & 0xEF)
    {
    case TS_TIMESKIP_0 :
      dTime -= *((float*)&pData[lPos]);
      lPrevDeltaTimePos = 0;
      break;
    case TS_TIMESKIP_1 :
      dTime -= *((float*)&pData[lPos]);
      lPrevDeltaTimePos -= pData[lPos+sizeof(float)];
      break;
    case TS_TIMESKIP_2 :
      dTime -= *((float*)&pData[lPos]);
      lPrevDeltaTimePos -= *((WORD*)&pData[lPos+sizeof(float)]);
      break;
    default: //ASSERT(FALSE);
      {//error, assume hist data is corrupt!
      CHstQueryException* pE = new CHstQueryException();
      pE->lErrorNumber = 11L;
      THROW(pE);
      break;
      }
    }
  lPos = lPrevDeltaTimePos;
  lID = 0;
  lQIDCnt = 0;
  lQID = pQ->SlotIDs[0];
  }

//---------------------------------------------------------------------------

void CQuerySlice::SkipSkipTime()
  {
  switch (pData[lPos++] & 0xEF)
    {
    case TS_TIMESKIP_0 :
      break;
    case TS_TIMESKIP_1 :
      lPos++;
      break;
    case TS_TIMESKIP_2 :
      lPos += 2;
      break;
    default: //ASSERT(FALSE);
      {//error, assume hist data is corrupt!
      CHstQueryException* pE = new CHstQueryException();
      pE->lErrorNumber = 12L;
      THROW(pE);
      break;
      }
    }
  lPos += sizeof(float);
  }

//---------------------------------------------------------------------------

void CQuerySlice::Get()
  {
  long APos = lPos;
  switch (pData[lPos++] & 0x0F)
    {
    case TS_DOUBLE : 
      Val.Set(*((double*)&pData[lPos]));
      lPos += sizeof(double);
      break;
    case TS_FLOAT : 
      Val.Set(*((float*)&pData[lPos]));
      lPos += sizeof(float);
      break;
    case TS_WORD : 
      Val.Set(*((unsigned short*)&pData[lPos]));
      lPos += sizeof(unsigned short);
      break;
    case TS_BIT_OFF :
      Val.Set((flag)0);
      //Val.Set(tt_Bit, 0.0);
      break;
    case TS_BIT_ON :
      Val.Set((flag)1);
      //Val.Set(tt_Bit, 1.0);
      break;
    case TS_BYTE :
      Val.Set(*((byte*)&pData[lPos]));
      lPos += sizeof(byte);
      break;
    case TS_LONG :
      Val.Set(*((long*)&pData[lPos]));
      lPos += sizeof(long);
      break;
    case TS_DWORD :
      Val.Set(*((unsigned long*)&pData[lPos]));
      lPos += sizeof(unsigned long);
      break;
    case TS_INT16 :
      Val.Set(*((short*)&pData[lPos]));
      lPos += sizeof(short);
      break;
    case TS_FLT16 :
      Val.Set(*((flt16*)&pData[lPos]));
      lPos += sizeof(flt16);
      break;
    case TS_STR :
      Val.Set((char*)&pData[lPos]);
      lPos += (strlen(Val.pChar)+1);
      break;
    default: //ASSERT(FALSE);
      {//error, assume hist data is corrupt!
      CHstQueryException* pE = new CHstQueryException();
      pE->lErrorNumber = 13L;
      THROW(pE);
      break;
      }
    }
  switch (pData[APos] & 0x60)
    {
    case TS_STATUS_VALID   : break;
    case TS_STATUS_UNDER   : Val.SetStatus(ds_UnderFlow); break;
    case TS_STATUS_OVER    : Val.SetStatus(ds_OverFlow); break;
    case TS_STATUS_INVALID : Val.SetStatus(ds_NAN); break;
    }
  lID++;
  }

//---------------------------------------------------------------------------

byte TSPosSkip[11] = { 1, 1, sizeof(double)+1, sizeof(float)+1, sizeof(long)+1, 
                       sizeof(unsigned short)+1, sizeof(byte)+1, sizeof(flt16)+1, 
                       sizeof(unsigned long)+1, sizeof(short)+1, 1 };

//---------------------------------------------------------------------------

/*#if dbgHstQuery
double DbgStartTime = 799646494.671;
long DbgCnt = 1;
#endif*/

void CQuerySlice::GetQuery(flag Backward)
  {//this function must be efficient as possible !!!
  #if dbgHstQuery
  //if (pTS->dStartTime==DbgStartTime && pQ->iTagCnt==DbgCnt)
  //  DumpCompleteSlice(Backward);
  if (dbgHistQuery())
    if (pQ->DbgDmpFile)
      {
      char Buff[256];
      sprintf(Buff, "---QueryTimeSlice: All:%d (%12.3f..%12.3f)  Closest:%d\r\n", bAll, pTS->dStartTime, pTS->dLastTime, bGetClosest);
      pQ->DbgDmpFile->Write(Buff, strlen(Buff));
      }
  #endif

  if (pHistory->pCurEvBlk && pHistory->pCurEvBlk->pCatItem==pQB->pCatItem)
    {
    if (pQB->dwTSPos==pHistory->pCurEvBlk->GetTSPos())
      {//if the slice being queried is the current one, LOCK it ...
      pLockBlk = pHistory->pCurEvBlk;
      pLockBlk->LockEvBlk();
      }
    }

  WORD TSLen = pTS->iLen;
  short TagCnt = pQ->iTagCnt;
  
  if (TSLen==2)
    {//may be start or stop...
    if ( ((pData[0] & TS_START) == TS_START) ||
         ((pData[0] & TS_STOP) == TS_STOP) )
      {
      Val.SetStatus(ds_NAN);
      for (long i=0; i<TagCnt; i++)
        {
        if (bGetClosest)
          {
          if (!(pQ->SlotFlags[i]))
            if (pTS->dStartTime < pQ->dStartTime)
              {
              pQ->RetVal(pQ->SlotIDs[i], pTS->dStartTime, Val);
              pQ->SlotFlags[i] = true;
              pQ->iFlagsFound++;
              }
          }
        else
          {
          //TO CONSIDER: if query is not of type "SysCAD Trend", do we really want to return a NAN !!!??? (I think not)
          if (IsIntData((byte)(pQ->Slots[3][i])))
            Val.Set(LONG_MAX);
          else
            Val.Set(dNAN);
          Val.SetStatus(ds_NAN);//set status after value!
          pQ->RetVal(pQ->SlotIDs[i], pTS->dStartTime, Val);
          }
        }
      return;
      }
    }

  if (Backward)
    {
    lID = 0;
    if (pTS->iLastDeltaTime > 0)
      lPos = pTS->iLastDeltaTime - sizeof(HD_TimeSliceHead);
    else
      lPos = 0; //there are no delta times
    lQIDCnt = 0;
    lQID = pQ->SlotIDs[0];
    lPrevDeltaTimePos = lPos;
    if (lPos>0)
      {
      ASSERT((pData[lPos] & TS_TIMESKIP) == TS_TIMESKIP);
      SkipSkipTime(); //get past last DeltaTime
      }
    dTime = pTS->dLastTime;
    flag Valid = true;
    while (Valid)
      {
      if (pData[lPos] & TS_SKIP)
        Skip();
      if ( (lQIDCnt >= TagCnt) ||
           ((pData[lPos] & TS_END) == TS_END) ||
           ((pData[lPos] & TS_TIMESKIP) == TS_TIMESKIP) ||
           (lPos >= TSLen) )
        {
        lPos = lPrevDeltaTimePos;
        if (lPos==0)
          Valid = false;
        else
          {
          SkipBackTime();
          if (lPos>0)
            SkipSkipTime();
          }
        }
      else
        {
        if (lID > lQID)
          {
          while (lQIDCnt < TagCnt && lQID < lID)
            {
            lQIDCnt++;
            if (lQIDCnt < TagCnt)
              lQID = pQ->SlotIDs[lQIDCnt];
            else
              lQID = 0;
            }
          }
        if (lID == lQID)
          {
          Get();
          if (bGetClosest)
            {
            if (!(pQ->SlotFlags[lQIDCnt]))
              if (dTime < pQ->dStartTime)
                {
                pQ->RetVal(lQID, dTime, Val);
                pQ->SlotFlags[lQIDCnt] = true;
                pQ->iFlagsFound++;
                }
            }
          else
            {
            if ((bAll) || (dTime>=pQ->dStartTime && dTime<=pQ->dLastTime))
              pQ->RetVal(lQID, dTime, Val);
            }
          lQIDCnt++;
          if (lQIDCnt < TagCnt)
            lQID = pQ->SlotIDs[lQIDCnt];
          else
            lQID = 0;
          }
        else
          {
          if ((pData[lPos] & 0x0F)==TS_STR)
            lPos += (strlen((const char*)&pData[lPos+1]) + 2);
          else
            lPos += TSPosSkip[pData[lPos] & 0x0F];
          lID++;
          }
        }
      }
    }
  else
    {
    lID = 0;
    lPos = 0;
    lQIDCnt = 0;
    lQID = pQ->SlotIDs[0];
    dTime = pTS->dStartTime;
    flag Valid = true;
    while (Valid)
      {
      if (lPos >= TSLen)
        Valid = false;
      else if (pData[lPos] & TS_SKIP)
        Skip();
      if ( ((pData[lPos] & TS_END) == TS_END) || 
           (lPos >= TSLen) )
        Valid = false;
      else if ((pData[lPos] & TS_TIMESKIP) == TS_TIMESKIP)
        SkipTime();
      else
        {
        if (lID > lQID)
          {
          while (lQIDCnt < TagCnt && lQID < lID)
            {
            lQIDCnt++;
            if (lQIDCnt < TagCnt)
              lQID = pQ->SlotIDs[lQIDCnt];
            else
              lQID = 0;
            }
          }
        if (lID == lQID)
          {
          Get();
          if (bGetClosest)
            {
            if (!(pQ->SlotFlags[lQIDCnt]))
              if (dTime > pQ->dLastTime)
                {
                pQ->RetVal(lQID, dTime, Val);
                pQ->SlotFlags[lQIDCnt] = true;
                pQ->iFlagsFound++;
                }
            }
          else
            {
            if ((bAll) || (dTime>=pQ->dStartTime && dTime<=pQ->dLastTime))
              pQ->RetVal(lQID, dTime, Val);
            }
          lQIDCnt++;
          if (lQIDCnt < TagCnt)
            lQID = pQ->SlotIDs[lQIDCnt];
          else
            lQID = 0;
          }
        else
          {
          if ((pData[lPos] & 0x0F)==TS_STR)
            lPos += (strlen((const char*)&pData[lPos+1]) + 2);
          else
            lPos += TSPosSkip[pData[lPos] & 0x0F];
          lID++;
          if (lID > lQID)
            {
            while (lQIDCnt < TagCnt && lQID < lID)
              {
              lQIDCnt++;
              if (lQIDCnt < TagCnt)
                lQID = pQ->SlotIDs[lQIDCnt];
              else
                lQID = 0;
              }
            }
          }
        }
      }
    }
  if (pLockBlk)
    {
    pLockBlk->FreeEvBlk();
    pLockBlk = NULL;
    }
  }

//---------------------------------------------------------------------------

void CQuerySlice::DumpCompleteSlice(flag Backward)
  {
  /*
  #if dbgHstQuery
  dbgpln("============================================================================");
  dbgpln("TimeSlice dump:%15.3f..%15.3f", pTS->dStartTime, pTS->dLastTime);
  dbgpln("TimeSlice len :%d", pTS->iLen);

  if (pEvBlk->pTS==pTS)
    {//if the slice being queried is the current one, LOCK it ...
    pLockBlk = pEvBlk;
    pLockBlk->LockEvBlk();
    }

  if (pTS->iLen==2)
    {//may be start or stop...
    if ( ((pData[0] & TS_START) == TS_START) ||
         ((pData[0] & TS_STOP) == TS_STOP) )
      {
      dbgpln("IS START OR STOP");
      return;
      }
    }

  if (Backward)
    {
    lID = 0;
    if (pTS->iLastDeltaTime > 0)
      lPos = pTS->iLastDeltaTime - sizeof(HD_TimeSliceHead);
    else
      lPos = 0; //there are no delta times
    lQIDCnt = 0;
    lQID = pQ->SlotIDs[0];
    lPrevDeltaTimePos = lPos;
    if (lPos>0)
      {
      ASSERT((pData[lPos] & TS_TIMESKIP) == TS_TIMESKIP);
      SkipSkipTime(); //get past last DeltaTime
      }
    dTime = pTS->dLastTime;
    dbgpln("Start pos:%d", lPos);
    dbgpln("last time:%15.3f", dTime);
    flag Valid = True;
    dbgpln("__Pos  ___ID  TYPE  value");
    while (Valid)
      {
      if (pData[lPos] & TS_SKIP)
        {
        Skip();
        dbgpln("%5d  %5d  skip  ", lPos, lID);
        }
      if (// (lQIDCnt >= pQ->iTagCnt) || 
           ((pData[lPos] & TS_END) == TS_END) ||
           ((pData[lPos] & TS_TIMESKIP) == TS_TIMESKIP) ||
           (lPos >= pTS->iLen) )
        {
        dbgpln("%5d  %5d  end   ", lPos, lID);
        lPos = lPrevDeltaTimePos;
        dbgpln("%5d  %5d  end   ", lPos, lID);
        if (lPos==0)
          Valid = False;
        else
          {
          SkipBackTime();
          if (lPos>0)
            SkipSkipTime();
          }
        dbgpln("%5d  %5d  end   time:%15.3f", lPos, lID, dTime);
        }
      else
        {
        if (lID > lQID)
          {
          while (lQIDCnt < pQ->iTagCnt && lQID < lID)
            {
            lQIDCnt++;
            if (lQIDCnt < pQ->iTagCnt)
              lQID = pQ->SlotIDs[lQIDCnt];
            else
              lQID = 0;
            }
          }
        if (lID == lQID)
          {
          Get();
          double dd = Val.GetDouble();
          dbgpln("%5d  %5d  get   %13.3f", lPos, lID, dd);
          lQIDCnt++;
          if (lQIDCnt < pQ->iTagCnt)
            lQID = pQ->SlotIDs[lQIDCnt];
          else
            lQID = 0;
          }
        else
          {
          //long Index = GetIndex(lID);
          CSlot* pSlot = pHistory->GetSlot(lID);
          Get();
          double dd = Val.GetDouble();
          dbgpln("%5d  %5d  sg    %13.3f  %s", lPos, lID, dd, pSlot->Data.sTag);
          }
        }
      }
    }
  else
    {
    //todo
    }
  if (pLockBlk)
    {
    pLockBlk->FreeEvBlk();
    pLockBlk = NULL;
    }
  #endif*/
  }

//===========================================================================

CQueryBlk::CQueryBlk()
  {
  pQ = NULL;
  pCatItem = NULL;
  pTS = NULL;
  hBlkFile = NULL;
  hBlkFileMap = NULL;
  pBlkMap = NULL;
  }

//---------------------------------------------------------------------------

CQueryBlk::~CQueryBlk()
  {
  if (pCatItem)
    CloseBlk();
  }

//---------------------------------------------------------------------------

flag CQueryBlk::Setup(long CatIndex)
  {
  pCCatItem pNewCatItem = pHistory->Cat.GetCatItemByIndex(CatIndex);
  ASSERT(pNewCatItem); //cat item should exist
  if (pCatItem)
    {
    if (pCatItem==pNewCatItem)
      return true;
    CloseBlk();
    }
  pCatItem = pNewCatItem;
  CString sFilename;
  CString sMapName;
  pCatItem->MakeFilename(sFilename);
  const char* fn = (const char*)sFilename;
  CFileStatus State;
  if (CFile::GetStatus(fn, State) == 0)
    {
    pCatItem->MakeCatFilename(sFilename);
    fn = (const char*)sFilename;
    if (CFile::GetStatus(fn, State) == 0)
      return false; //file does not exist on data directory or catalog directory
    }
  pCatItem->MakeMapName(sMapName);
  const char* mn = (const char*)sMapName;
  hBlkFile = CreateFile(fn, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
  if (hBlkFile==INVALID_HANDLE_VALUE)
    {
    LogError("History", 0, "Event Block File '%s' Not Opened [%i]", fn, GetLastError());
    return false;
    }
    
  dwFileAllocLen = GetFileSize(hBlkFile, NULL);

  hBlkFileMap = CreateFileMapping((LPVOID)hBlkFile, NULL, PAGE_READONLY, 0,0, mn);
  if (hBlkFileMap==NULL)
    {
    LogError("History", 0, "Create File Mapping for '%s' [%i]", fn, GetLastError());
    if (!CloseHandle(hBlkFile))
      LogError("History", 0, "Close Handle A %s", FindWinError(GetLastError()));
    return false;
    }

  dwMapPos = 0;
  dwMapSize = Min(CHistorian::dwDoubleGran, dwFileAllocLen);

  pBlkMap = (byte*)MapViewOfFile(hBlkFileMap, FILE_MAP_READ, 0, dwMapPos, dwMapSize);

  if (pBlkMap==NULL)
    {
    LogError("History", 0, "Map View Of File for '%s' [%i]", fn, GetLastError());
    CloseHandle(hBlkFileMap);
    if (!CloseHandle(hBlkFile))
      LogError("History", 0, "Close Handle B %s", FindWinError(GetLastError()));
    return false;
    }

  pHD_EvBlkHead pEvHead = (pHD_EvBlkHead)pBlkMap;
  if (pEvHead->iVerNo!=HstVerNo && pEvHead->iVerNo!=3 && HstVerNo!=4)
    {
    UnmapViewOfFile(pBlkMap);
    CloseHandle(hBlkFileMap);
    CloseHandle(hBlkFile);
    LogError("History", 0, "Historian version number mismatch in %s", fn);
    return false;
    }
  //store TimeSlice quick lookup jump table from EvHead...
  iVerNo = pEvHead->iVerNo;
  iJmpCount = pEvHead->iJmpCount;
  ASSERT(iJmpCount<MaxJmps);
  for (int i=0; i<MaxJmps; i++)
    {
    JmpPos[i] = pEvHead->JmpPos[i];
    JmpTimes[i] = pEvHead->JmpTimes[i];
    }
  return true;
  }

//---------------------------------------------------------------------------

void CQueryBlk::CloseBlk()
  {
  if (pCatItem)
    {
    if (!UnmapViewOfFile(pBlkMap))
      LogError("History", 0, "Unmap file A %s", FindWinError(GetLastError()));
    if (!CloseHandle(hBlkFileMap))
      LogError("History", 0, "Close Handle C %s", FindWinError(GetLastError()));
    if (!CloseHandle(hBlkFile))
      LogError("History", 0, "Close Handle D %s", FindWinError(GetLastError()));
    pCatItem = NULL;
    }
  }

//---------------------------------------------------------------------------

void CQueryBlk::GetNearestTS()
  {
  if (iVerNo>3)
    {
    if (bAll)
      {
      if (pQ->bBackward)
        {
        if (iJmpCount>0)
          {
          dwTSPos = JmpPos[iJmpCount-1];
          GetQueryTS();
          }
        else
          GetFirstTS();
        if (pTS)
          {
          while (IsNextTS())
            GetNextTS();
          }
        }
      else
        GetFirstTS();
      }
    else
      {
      if (pQ->bBackward)
        {
        if (iJmpCount>0)
          {
          int i = iJmpCount-1;
          while (i>0 && JmpTimes[i]>=pQ->dLastTime)
            i--;
          dwTSPos = JmpPos[i];
          GetQueryTS();
          }
        else
          GetFirstTS();
        while (pTS->dStartTime<pQ->dLastTime && IsNextTS())
          GetNextTS();
        }
      else
        {
        if (iJmpCount>0)
          {
          int i = 0;
          while (i<iJmpCount-1 && JmpTimes[i]<=pQ->dStartTime)
            i++;
          if (i>0)
            {
            dwTSPos = JmpPos[i-1];
            GetQueryTS();
            }
          else
            GetFirstTS();
          }
        else
          GetFirstTS();
        }
      }
    }
  else
    {
    GetFirstTS();
    if (pQ->bBackward)
      {
      if (pTS)
        {
        if (bAll)
          {
          while (IsNextTS())
            GetNextTS();
          }
        else
          {
          while (pTS->dStartTime<pQ->dLastTime && IsNextTS())
            GetNextTS();
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CQueryBlk::GetQuery()
  {
  bAll = false;
  if (pQ->dStartTime<=pCatItem->StartTime() && pQ->dLastTime>=pCatItem->LastTime())
    bAll = true;

  #if dbgHstQuery
  if (dbgHistQuery())
    if (pQ->DbgDmpFile)
      {
      char Buff[256];
      sprintf(Buff, "---QueryBlk:%s All:%d (%12.3f..%12.3f)\r\n", pCatItem->CatNoStr(), bAll, pCatItem->StartTime(), pCatItem->LastTime());
      pQ->DbgDmpFile->Write(Buff, strlen(Buff));
      }
  #endif

  GetNearestTS();
  CQuerySlice QS(pQ, this);
  if (pQ->bBackward)
    {
    while (pTS)
      {
      if (bAll)
        {
        QS.SetSlice(pTS);
        QS.GetQuery((flag)pQ->bBackward);
        }
      else
        {
        if (pTS->dLastTime>=pQ->dStartTime && pTS->dStartTime<=pQ->dLastTime)
          {
          if (pTS->dStartTime>=pQ->dStartTime && pTS->dLastTime<=pQ->dLastTime)
            QS.SetSlice(pTS, true);
          else
            QS.SetSlice(pTS, false);
          QS.GetQuery((flag)pQ->bBackward);
          }
        }
      GetPrevTS();
      }
    }
  else
    {
    while (pTS)
      {
      if (bAll)
        {
        QS.SetSlice(pTS);
        QS.GetQuery((flag)pQ->bBackward);
        }
      else
        {
        if (pTS->dLastTime>=pQ->dStartTime && pTS->dStartTime<=pQ->dLastTime)
          {
          if (pTS->dStartTime>=pQ->dStartTime && pTS->dLastTime<=pQ->dLastTime)
            QS.SetSlice(pTS, true);
          else
            QS.SetSlice(pTS, false);
          QS.GetQuery((flag)pQ->bBackward);
          }
        }
      GetNextTS();
      }
    }
  }

//---------------------------------------------------------------------------

flag CQueryBlk::GetLesser()
  {
  flag AllFound = false;
  if (iVerNo>3)
    {
    if (iJmpCount>0)
      {
      int i = iJmpCount-1;
      while (i>0 && JmpTimes[i]>=pQ->dStartTime)
        i--;
      dwTSPos = JmpPos[i];
      GetQueryTS();
      }
    else
      GetFirstTS();
    }
  else
    GetFirstTS();
  CQuerySlice QS(pQ, this);
  if (pTS)
    while (pTS->dLastTime < pQ->dStartTime && IsNextTS())
      GetNextTS();
  while (pTS && !AllFound)
    {
    if (pTS->dStartTime < pQ->dStartTime)
      {
      QS.SetSlice(pTS, false, true);
      QS.GetQuery(true);
      if (pQ->iFlagsFound == pQ->iTagCnt)
        AllFound = true;
      }
    GetPrevTS();
    }
  return AllFound;
  }

//---------------------------------------------------------------------------

flag CQueryBlk::GetGreater()
  {
  flag AllFound = false;
  if (iVerNo>3)
    {
    if (iJmpCount>0)
      {
      int i = 0;
      while (i<iJmpCount-1 && JmpTimes[i]<=pQ->dLastTime)
        i++;
      if (i>0)
        {
        dwTSPos = JmpPos[i-1];
        GetQueryTS();
        }
      else
        GetFirstTS();
      }
    else
      GetFirstTS();
    }
  else
    GetFirstTS();
  CQuerySlice QS(pQ, this);
  while (pTS && !AllFound)
    {
    if (pTS->dLastTime > pQ->dLastTime)
      {
      QS.SetSlice(pTS, false, true);
      QS.GetQuery(false);
      if (pQ->iFlagsFound == pQ->iTagCnt)
        AllFound = true;
      }
    GetNextTS();
    }
  return AllFound;
  }

//---------------------------------------------------------------------------

void CQueryBlk::GetQueryTS()
  {
  if ( (dwTSPos >= dwMapPos) && 
       (dwTSPos <= dwMapPos + CHistorian::dwGranularity) )
    {
    pTS = (pHD_TimeSliceHead)&(pBlkMap[dwTSPos - dwMapPos]);
    return;
    }
  if ( (dwTSPos > dwMapPos) &&
       (dwMapPos + dwMapSize >= dwFileAllocLen) )
    {
    pTS = (pHD_TimeSliceHead)&(pBlkMap[dwTSPos - dwMapPos]);
    return;
    }
  //need to change mapping...
  dwMapPos = (dwTSPos / CHistorian::dwGranularity) * CHistorian::dwGranularity;
  dwMapSize = CHistorian::dwDoubleGran;
  if (dwMapPos + dwMapSize > dwFileAllocLen)
    dwMapSize = dwFileAllocLen - dwMapPos;
  if (!UnmapViewOfFile(pBlkMap))
    LogError("History", 0, "Unmap file B %s", FindWinError(GetLastError()));
  pBlkMap = (byte*)MapViewOfFile(hBlkFileMap, FILE_MAP_READ, 0, dwMapPos, dwMapSize);
  if (pBlkMap==NULL)
    {
    LogError("History", 0, "Map View Of File [%i]", GetLastError());
    ASSERT(FALSE);
    pTS = NULL;
    return;
    }
  pTS = (pHD_TimeSliceHead)&(pBlkMap[dwTSPos - dwMapPos]);
  }

//---------------------------------------------------------------------------
//===========================================================================

CQueryHist::CQueryHist( double StartTime,
                        double LastTime,
                        CXM_Route &Route,
                        long RqstNo,
                        long SrcID,
                        flag GetGreater,
                        flag GetLesser,
                        short TagCnt)
  {
  iAllocCnt = TagCnt;
  Initialise();
  dStartTime = StartTime;
  dLastTime = LastTime;
  bGetGreater = GetGreater;
  bGetLesser = GetLesser;
  lSrcID = SrcID;
  xRoute.ReverseRoute(Route);
  XMr.Clear();
  QueryDst = QTrend;
  bBackward = 1;
  lRqstNo = RqstNo;
  //Route.dbgDump("QH-In");
  //xRoute.dbgDump("QH-Rev");
  if (gs_pQueryInfo)
    {
    CQueryInfoStats QI((long)this, 0.0, (xRoute.NoNodes()>3), 1);
    gs_pQueryInfo->SendMessage(WMU_HSTQRYUPDATE, 0, (LPARAM)&QI);
    }
  }

//---------------------------------------------------------------------------

CQueryHist::CQueryHist( double StartTime,
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
                        double RepTimeOffset)
  {
  iAllocCnt = 0;
  Initialise();
  dStartTime = Max(0.0, StartTime);
  dLastTime = Max(0.0, LastTime);
  dRepTimeOffset = RepTimeOffset;
  QueryOpt = Opt;
  iTimeOptUnits = QryTimeOptUnits;
  bTimeOptFull = QryTimeOptFull;
  bDoHeadings = Headings;
  lNoPts = Max(NoPts, 2L);
  lSrcID = SrcID;
  bGetGreater = true;
  bGetLesser = true;
  xRoute.ReverseRoute(Route);
  XMr.Clear();
  QueryDst = QDst;
  bBackward = 0;
  iFileFormat = FileMode;
  if (pFilename1)
    sFilename1 = pFilename1;
  if (pFilename2)
    sFilename2 = pFilename2;
  if (gs_pQueryInfo)
    {
    CQueryInfoStats QI((long)this, 0.0, (xRoute.NoNodes()>3), 1);
    gs_pQueryInfo->SendMessage(WMU_HSTQRYUPDATE, 0, (LPARAM)&QI);
    }
  }

//---------------------------------------------------------------------------

void CQueryHist::Initialise()
  {
  Slots.SetSize(4, iAllocCnt, LONG_MAX);
  bGetGreater = 0;
  bGetLesser = 0;
  bBusyServicing = 0;
  bCancel = 0;
  bStarted = 0;
  bDoHeadings = 1;
  bIsDData = 0;
  bIsLData = 0;
  bIsSData = 0;
  pNext = NULL;
  iTagCnt = 0;
  SlotIDs = NULL;
  SlotFlags = NULL;
  lRqstNo = 0;
  QueryOpt = All;
  iTimeOptUnits = 1;
  bTimeOptFull = 1;
  bPrevTimeStartStop = 0;
  iTimeSliceCnt = 0;
  fFile = NULL;
  dRepTimeOffset = 0.0;
  bDoLastValForFirstPoint = 1; //For now force this to true, this should be a user option!!!
                               //when getting AveEqualySpaced, for the first point rather get the LastEqualySpaced value
  Q.SetQueryMain(this);
  }

//---------------------------------------------------------------------------

CQueryHist::~CQueryHist()
  {
  if (gs_pQueryInfo)
    {
    CQueryInfoStats QI((long)this, 100.01, (xRoute.NoNodes()>3), -1);
    gs_pQueryInfo->SendMessage(WMU_HSTQRYUPDATE, 0, (LPARAM)&QI);
    }
  for (long i=0; i<iTagCnt && i<Slots.GetCols(); i++)
    {
    if (Slots[2][i]!=0) //pointer to cnv string
      {
      char* p = (char*)Slots[2][i];
      delete []p;
      }
    }
  Slots.SetSize(4, 0);
  }

//---------------------------------------------------------------------------

flag CQueryHist::AddTag(char* pTag, long TrndNo)
  {
#if ForSysCAD
  Strng Tag,Cnv;
  TaggedObject::SplitTagCnv(pTag, Tag, Cnv);

  long SNo = pHistory->GetSlot(Tag());
#else
  long SNo = pHistory->GetSlot(pTag);
#endif
  if (SNo>=0)
    {
    CSlot* pSlot = pHistory->GetSlotByIndex(SNo);
    if (iTagCnt>=iAllocCnt)
      {
      iAllocCnt += 16;
      Slots.SetSize(4, iAllocCnt, LONG_MAX);
      }
    Slots[0][iTagCnt] = SNo;      //slot number
    Slots[1][iTagCnt] = TrndNo;   //trend number
    Slots[3][iTagCnt] = pSlot->Data.VarTyp; //slot tt_xxx type
    if (IsFloatData(pSlot->Data.VarTyp))
      bIsDData = 1;
    else if (IsIntData(pSlot->Data.VarTyp))
      bIsLData = 1;
    else
      bIsSData = 1;
#if ForSysCAD
    if (Cnv.Length()>0 && pSlot->iCnvNo>0 && IsFloatData(pSlot->Data.VarTyp)) //only support cnvs for floating point data
      {
      Slots[2][iTagCnt] = (long)(new char[Cnv.Length()+1]);
      strcpy((pchar)(Slots[2][iTagCnt]), Cnv()); //pointer to cnv string
      }
    else
#endif
      Slots[2][iTagCnt] = 0; //pointer to cnv string
    iTagCnt++;
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

flag CQueryHist::GetLesser(long Index)
  {
  bGettingOuter = 1;
  for (long i=0; i<iTagCnt; i++)
    SlotFlags[i] = false;
  iFlagsFound = 0;
  ASSERT(Index>=0);
  /*if (Index<0)
    Index = pHistory->Cat.GetCatItemByStartTime(dStartTime);*/
  if (!Q.Setup(Index))
    return false;
  if (!Q.GetLesser())
    {//if slots arn't found in this block, try one more
    Index--;
    if (Index<0)
      return true;
    if (!Q.Setup(Index))
      return false;
    Q.GetLesser();
    }
  return true;
  }

//---------------------------------------------------------------------------

flag CQueryHist::GetGreater(long Index)
  {
  ASSERT(QueryOpt!=AveEqualySpaced && QueryOpt!=LastEqualySpaced);
  bGettingOuter = 1;
  for (long i=0; i<iTagCnt; i++)
    SlotFlags[i] = false;
  iFlagsFound = 0;
  ASSERT(Index>=0);
  /*if (Index<0)
    Index = pHistory->Cat.GetCatItemByLastTime(dLastTime);*/
  if (!Q.Setup(Index))
    return false;
  if (!Q.GetGreater())
    {//if slots arn't found in this block, try one more
    Index++;
    if (Index>=pHistory->Cat.Data.iCatItemCnt)
      return true;
    if (!Q.Setup(Index))
      return false;
    Q.GetGreater();
    }
  return true;
  }

//---------------------------------------------------------------------------

void CQueryHist::Send(CXM_QueryString * xb)
  {
  if (XMr.PackMsg(xb))
    bMustFlush = true;
  else
    {
    pHistory->XSendMessage(XMr, xRoute);
    XMr.Clear();
    bMustFlush = XMr.PackMsg(xb);
    }
  }

//---------------------------------------------------------------------------

void CQueryHist::SendSlice()
  {//send result set
  ASSERT(QueryDst==QFile || QueryDst==QFileDirect || QueryDst==QDDE || QueryDst==QOLE);
  if (QueryDst==QOLE)
    SendSliceOLE();
  else
    SendSliceOther();
  }

//---------------------------------------------------------------------------

void CQueryHist::SendSliceOLE()
  {
  if (QueryOpt==AveEqualySpaced || QueryOpt==LastEqualySpaced)
    dStartTime += dTimeDelta;
  char Buff[1024];
  char Buff1[256];
  if (!bDoneHeadings)
    {
    bDoneHeadings = 1;
    if (QueryOpt!=AveEqualySpaced && QueryOpt!=LastEqualySpaced)
      {
      if (bBackward)
        dPrevTime[0] = dLastTime;
      else
        dPrevTime[0] = dStartTime;
      }
    if (bDoHeadings)
      {//write headings...
      char Buff2[256];
      if (QueryOpt==AveEqualySpaced || QueryOpt==LastEqualySpaced)
        SecstoHMSDate(dStartTime+dTimeDelta, Buff1);
      else
        SecstoHMSDate(dStartTime, Buff1);
      SecstoHMSDate(dLastTime, Buff2);
      CXM_QueryString *pQry= new CXM_QueryString (lSrcID);
      pQry->Set("Query SysCAD historian from %s to %s", Buff1, Buff2);
      Send(pQry); // it has gone

      pQry= new CXM_QueryString(lSrcID);
      if (bTimeOptFull)
        sprintf(Buff, "FullTime,");
      else
        sprintf(Buff, "");
      switch (iTimeOptUnits)
        {
        case 1: sprintf(Buff, "%sSeconds,", Buff); break;
        case 2: sprintf(Buff, "%sMinutes,", Buff); break;
        case 3: sprintf(Buff, "%sHours,", Buff); break;
        case 4: sprintf(Buff, "%sDays,", Buff); break;
        }
      for (int i=0; i<iTagCnt; i++)
        {
        CSlot* pSlot = pHistory->GetSlot(SlotIDs[i]);
        if (i==0)
          sprintf(Buff, "%s\"%s\"", Buff, pSlot->Data.sTag);
        else
          sprintf(Buff, "%s,\"%s\"", Buff, pSlot->Data.sTag);
        }
      pQry->Set(Buff);
      Send(pQry);
      }    
    }
  //write data...
  double dTime;
  switch (QueryOpt)
    {
    case All: dTime = dPrevTime[0]; break;
    case AveEqualySpaced: dTime = dWrkTim; break;
    case LastEqualySpaced: dTime = dWrkTim; break;
    }
  CXM_QueryRowEx *pRowQry=new CXM_QueryRowEx(lSrcID, dTime);
  for (int i=0; i<iTagCnt; i++)
    {
    if (Slots[2][i]!=0) //pointer to cnv string
      {
      CSlot* pSlot = pHistory->GetSlot(SlotIDs[i]);
#if ForSysCAD
      pRowQry->AddValue(Cnvs[pSlot->iCnvNo]->Human(DData[i], (pchar)(Slots[2][i])));
#else
      pRowQry->AddValue(DData[i]);
#endif
      }
    else
      {
      if (IsFloatData((byte)Slots[3][i]))
        pRowQry->AddValue(DData[i]);
      else if (IsIntData((byte)Slots[3][i]))
        pRowQry->AddValue(LData[i]);
      else
        //pRowQry->AddValue(SData[i]());
        pRowQry->AddValue((char*)(const char*)SData[i]);
      }
    }
  if (XMr.PackMsg(pRowQry))
    bMustFlush = true;
  else
    {
    pHistory->XSendMessage(XMr, xRoute);
    XMr.Clear();
    bMustFlush = XMr.PackMsg(pRowQry);
    }
  if (QueryOpt==AveEqualySpaced || QueryOpt==LastEqualySpaced)
    dStartTime -= dTimeDelta;
  }

//---------------------------------------------------------------------------

void CQueryHist::SendSliceOther()
  {
  char Buff1[512];
  if (QueryOpt==AveEqualySpaced || QueryOpt==LastEqualySpaced)
    dStartTime += dTimeDelta;
  char Buff[4096];
  if (!bDoneHeadings)
    {
    bDoneHeadings = 1;
    if (QueryOpt!=AveEqualySpaced && QueryOpt!=LastEqualySpaced)
      {
      if (bBackward)
        dPrevTime[0] = dLastTime;
      else
        dPrevTime[0] = dStartTime;
      }
    if (bDoHeadings)
      {//write headings...
      char Buff2[256];
      if (QueryOpt==AveEqualySpaced || QueryOpt==LastEqualySpaced)
        SecstoHMSDate(dStartTime+dTimeDelta, Buff1);
      else
        SecstoHMSDate(dStartTime, Buff1);
      SecstoHMSDate(dLastTime, Buff2);
      sprintf(Buff, "Query SysCAD historian from %s to %s", Buff1, Buff2);
      if (QueryDst==QFileDirect)
        {
        if (iFileFormat==0)
          {
          strcat(Buff, "\n");
          fwrite(Buff, sizeof(char), strlen(Buff), fFile);
          }
        }
      else
        {
        CXM_QueryString *xb=new CXM_QueryString (lSrcID);
        xb->Set(Buff);
        Send(xb); // It has gone
        }

      if (bTimeOptFull)
        sprintf(Buff, "FullTime,");
      else
        Buff[0] = 0;
      switch (iTimeOptUnits)
        {
        case 1: sprintf(Buff, "%sSeconds,", Buff); break;
        case 2: sprintf(Buff, "%sMinutes,", Buff); break;
        case 3: sprintf(Buff, "%sHours,", Buff); break;
        case 4: sprintf(Buff, "%sDays,", Buff); break;
        }
      for (int i=0; i<iTagCnt; i++)
        {
        CSlot* pSlot = pHistory->GetSlot(SlotIDs[i]);
        if (i==0)
          sprintf(Buff, "%s\"%s\"", Buff, pSlot->Data.sTag);
        else
          sprintf(Buff, "%s,\"%s\"", Buff, pSlot->Data.sTag);
        if (QueryDst==QFileDirect && i%25==24)
          {
          if (iFileFormat==0)
            {
            fwrite(Buff, sizeof(char), strlen(Buff), fFile);
            Buff[0] = 0;
            }
          }
        }

      if (QueryDst==QFileDirect)
        {
        if (iFileFormat==0)
          {
          strcat(Buff, "\n");
          fwrite(Buff, sizeof(char), strlen(Buff), fFile);
          }
        else if (iFileFormat==1)
          {
          sprintf(Buff, "TagId,TimeIntervalNo,Value\n");
          fwrite(Buff, sizeof(char), strlen(Buff), fFile);
//LogNote("History Debug", 0, "Write:%s", Buff);
          }
        }
      else
        {
        CXM_QueryString *xb=new CXM_QueryString (lSrcID);
        xb->Set(Buff);
        Send(xb);
        }
      }    
    }

  //write data...
  if (QueryDst==QFileDirect && iFileFormat==1)
    Buff[0] = 0;
  else
    {
    double dTime;
    switch (QueryOpt)
      {
      case All: dTime = dPrevTime[0]; break;
      case AveEqualySpaced: dTime = dWrkTim; break;
      case LastEqualySpaced: dTime = dWrkTim; break;
      }
    if (bTimeOptFull)
      {
      SecstoHMSDate(dTime, Buff1);
      sprintf(Buff, "%s,", Buff1);
      }
    else
      Buff[0] = 0;
    switch (iTimeOptUnits)
      {
      case 1: sprintf(Buff, "%s%.4f,", Buff, dTime - dStartTime); break;
      case 2: sprintf(Buff, "%s%.3f,", Buff, (dTime - dStartTime)/60.0); break;
      case 3: sprintf(Buff, "%s%.3f,", Buff, (dTime - dStartTime)/3600.0); break;
      case 4: sprintf(Buff, "%s%.3f,", Buff, (dTime - dStartTime)/86400.0); break;
      }
    }

  for (int i=0; i<iTagCnt; i++)
    {
    if (IsFloatData((byte)Slots[3][i]))
      {
      if (Slots[2][i]!=0) //pointer to cnv string
        {
        CSlot* pSlot = pHistory->GetSlot(SlotIDs[i]);
  #if ForSysCAD
        sprintf(Buff1, "%g", Cnvs[pSlot->iCnvNo]->Human(DData[i], (pchar)(Slots[2][i])));
  #else
        sprintf(Buff1, "%g", DData[i]);
  #endif
        }
      else
        {
        sprintf(Buff1, "%g", DData[i]);
        }
      }
    else if (IsIntData((byte)Slots[3][i]))
      {
      sprintf(Buff1, "%d", LData[i]);
      }
    else
      {
      sprintf(Buff1, "%s", (char*)(const char*)SData[i]);
      }
    if (QueryDst==QFileDirect && iFileFormat==1)
      {
      sprintf(Buff, "%s%d,%d,%s\n", Buff, i, iTimeSliceCnt, Buff1);
      if (i%90==89)
        {
        fwrite(Buff, sizeof(char), strlen(Buff), fFile);
        Buff[0] = 0;
        }
      }
    else
      {
      if (i==0)
        strcat(Buff, Buff1);
      else
        sprintf(Buff, "%s,%s", Buff, Buff1);
      if (QueryDst==QFileDirect && i%120==119)
        {
        if (iFileFormat==0)
          {
          fwrite(Buff, sizeof(char), strlen(Buff), fFile);
          Buff[0] = 0;
          }
        }
      }
    }
  if (QueryDst==QFileDirect)
    {
    if (iFileFormat==0)
      {
      strcat(Buff, "\n");
      fwrite(Buff, sizeof(char), strlen(Buff), fFile);
      }
    else if (iFileFormat==1)
      {
      if (strlen(Buff)>0)
        fwrite(Buff, sizeof(char), strlen(Buff), fFile);
      }
    }
  else
    {
    CXM_QueryString *xb=new CXM_QueryString (lSrcID);
    xb->Set(Buff);
    Send(xb);
    }
  if (QueryOpt==AveEqualySpaced || QueryOpt==LastEqualySpaced)
    dStartTime -= dTimeDelta;
  iTimeSliceCnt++;
  }

//---------------------------------------------------------------------------

void CQueryHist::BlockUnavailable(flag &OK)
  {//query data block file could not be opened
  OK = true;
  CString s;
  Q.pCatItem->MakeFilename(s);
  if (s.ReverseFind('\\')>=0)
    s = s.Mid(s.ReverseFind('\\')+1);
  switch (QueryDst)
    {
    case QTrend:
      {
      if (!bGettingOuter)
        {
        BlockUnavailableTrend((const char*)s);
        OK = false;
        }
      break;
      }
    case QFile:
    case QFileDirect:
    case QDDE:
    case QOLE:
      {
      BlockUnavailableOther((const char*)s);
      break;
      }
    }
  Q.pCatItem = NULL; //must not try to close the block since it was never opened
  }

//---------------------------------------------------------------------------

void CQueryHist::BlockUnavailableTrend(const char* pBlkName)
  {
  if (bMustFlush)
    pHistory->XSendMessage(XMr, xRoute);
  bMustFlush = 0;
  XMr.Clear();
  VERIFY(XMr.PackMsg(new CXM_HistoryDataError(1L, lRqstNo, 0.0, (char*)pBlkName)));
  pHistory->XSendMessage(XMr, xRoute);
  XMr.Clear();
  }

//---------------------------------------------------------------------------

void CQueryHist::BlockUnavailableOther(const char* pBlkName)
  {
  char Buff1[256];
  char Buff2[256];
  SecstoHMSDate(Q.pCatItem->Data.dStartTime, Buff1);
  SecstoHMSDate(Q.pCatItem->Data.dLastTime, Buff2);
  CXM_QueryString *xb=new CXM_QueryString (lSrcID);
  xb->Set("%s could not be opened. Range %s (%18.3f) to %s (%18.3f)", pBlkName, Buff1, Q.pCatItem->Data.dStartTime, Buff2, Q.pCatItem->Data.dLastTime);
  Send(xb);
  }

//---------------------------------------------------------------------------

void CQueryHist::GetQuery_PartOne()
  {
  //check validity of query...
  ASSERT(QueryDst==QFile || QueryDst==QFileDirect || QueryDst==QDDE || QueryDst==QOLE);
  bool Error = false;
  if (iTagCnt==0 || dStartTime >= dLastTime)
    {
    CXM_QueryString *xb=new CXM_QueryString(lSrcID);
    if (iTagCnt==0)
      {
      //LogWarning("History", 0, "No valid tags were queried");
      xb->Set("No valid tags were queried");
      }
    else
      {
      //LogWarning("History", 0, "Query start time must be less than the query end time");
      xb->Set("Query start time must be less than the query end time");
      }
    Send(xb);
    Error = true;
    }

  if (!Error && QueryDst==QFileDirect)
    {
    if (!Error)
      {
      fFile = fopen((const char*)sFilename1, "wt");
      if (!fFile)
        {
        LogWarning("History", 0, "Cannot open output file %s", (const char*)sFilename1);
        CXM_QueryString *xb=new CXM_QueryString(lSrcID);
        xb->Set("Cannot open query result file");
        Send(xb);
        Error = true;
        }
//else
//LogNote("History Debug", 0, "Open %s", (const char*)sFilename1);

      }
    }

  if (Error)
    {
    CXM_QueryString *xb=new CXM_QueryString(lSrcID);
    xb->Set("TheEnd");
    Send(xb);
    if (bMustFlush)
      {
      pHistory->XSendMessage(XMr, xRoute);
      bMustFlush = 0;
      XMr.Clear();
      }
    }
  }

//---------------------------------------------------------------------------

void CQueryHist::GetQuery_PartTwo()
  {
  //setup query...
  ASSERT(QueryDst==QFile || QueryDst==QFileDirect || QueryDst==QDDE || QueryDst==QOLE);
  bDoneHeadings = 0;
  //if (bIsDData)
  DData.SetSize(iTagCnt);
  DData.SetAll(dNAN);
  if (bIsLData)
    LData.SetSize(iTagCnt);
  if (bIsSData)
    SData.SetSize(iTagCnt);
  dPrevTime.SetSize(iTagCnt);
  dPrevTime.SetAll(-1.0);
  if (QueryOpt==AveEqualySpaced || QueryOpt==LastEqualySpaced)
    {
    ASSERT(!bBackward); //CANNOT go backward
    bGetGreater = 0;
    dTimeDelta = (dLastTime - dStartTime) / (lNoPts - 1);
    dSigVal.SetSize(iTagCnt);
    dPrevVal.SetSize(iTagCnt);
    dSigVal.SetAll(0.0);
    dPrevVal.SetAll(0.0);
    dWrkTim = dStartTime;
    dStartTime -= dTimeDelta;
    dPrevTime.SetAll(dStartTime);
    }
  }

//---------------------------------------------------------------------------

void CQueryHist::GetQuery_PartThree()
  {
  ASSERT(QueryDst==QFile || QueryDst==QFileDirect || QueryDst==QDDE || QueryDst==QOLE);
  CXM_QueryString *xb=new CXM_QueryString (lSrcID, "TheEnd");
  Send(xb);
  if (bMustFlush)
    {
    pHistory->XSendMessage(XMr, xRoute);
    bMustFlush = 0;
    XMr.Clear();
    }
  }

//---------------------------------------------------------------------------

void CQueryHist::GetQuery_PartFour()
  {
  ASSERT(QueryDst==QFile || QueryDst==QFileDirect || QueryDst==QDDE || QueryDst==QOLE);
  char Buff[8192];
  if (QueryDst==QFileDirect)
    {
    if (iFileFormat==0)
      {
      strcpy(Buff, "TheTags");
      for (int i=0; i<iTagCnt; i++)
        {
        CSlot* pSlot = pHistory->GetSlot(SlotIDs[i]);
        strcat(Buff, ":\"");
        strcat(Buff, pSlot->Data.sTag);
        strcat(Buff, "\"");
        if (i%25==24)
          {
          fwrite(Buff, sizeof(char), strlen(Buff), fFile);
          Buff[0] = 0;
          }
        }
      strcat(Buff, "\n");
      fwrite(Buff, sizeof(char), strlen(Buff), fFile);
      }
    else if (iFileFormat==1)
      {
      FILE* f = fopen((const char*)sFilename2, "wt");
      if (!f)
        {
        LogWarning("History", 0, "Cannot open output file %s", (const char*)sFilename2);
        //CXM_QueryString *xb=new CXM_QueryString(lSrcID);
        //xb->Set("Cannot open query result tags file");
        //Send(xb);
        }
      else
        {
        if (bDoHeadings)
          {
          sprintf(Buff, "TagID,Tag\n");
          fwrite(Buff, sizeof(char), strlen(Buff), f);
          }
        Buff[0] = 0;
        for (int i=0; i<iTagCnt; i++)
          {
          CSlot* pSlot = pHistory->GetSlot(SlotIDs[i]);
          sprintf(Buff, "%s%d,%s\n", Buff, i, pSlot->Data.sTag);
          if (i%25==24)
            {
            fwrite(Buff, sizeof(char), strlen(Buff), f);
            Buff[0] = 0;
            }
          }
        if (strlen(Buff)>0)
          fwrite(Buff, sizeof(char), strlen(Buff), f);
        fclose(f);
        }
      }
    }
  else
    {
    strcpy(Buff, "TheTags");
    for (int i=0; i<iTagCnt; i++)
      {
      CSlot* pSlot = pHistory->GetSlot(SlotIDs[i]);
      strcat(Buff, ":\"");
      strcat(Buff, pSlot->Data.sTag);
      strcat(Buff, "\"");
      if (i>100 && strlen(Buff)>sizeof(Buff)-100)
        {//Buff too small!!!
        strcat(Buff, ":\"Too Many Tags...\"");
        i = iTagCnt;//exit
        }
      }
    CXM_QueryString *xb=new CXM_QueryString(lSrcID, Buff);
    Send(xb);
    if (bMustFlush)
      {
      pHistory->XSendMessage(XMr, xRoute);
      bMustFlush = 0;
      XMr.Clear();
      }
    }
  }

//---------------------------------------------------------------------------

void CQueryHist::GetQuery_PartFive()
  {
  ASSERT(QueryDst==QFile || QueryDst==QFileDirect || QueryDst==QDDE || QueryDst==QOLE);
  if (QueryDst==QFileDirect)
    {
//todo: put in a stopwatch to calc and display query time...
    LogNote("History", 0, "Generated historian data file %s", (const char*)sFilename1);
    fclose(fFile);
    fFile = NULL;
    }
  CXM_QueryString *xb=new CXM_QueryString (lSrcID, "TheEnd");
  Send(xb);
  if (bMustFlush)
    {
    pHistory->XSendMessage(XMr, xRoute);
    bMustFlush = 0;
    XMr.Clear();
    }
  }

//---------------------------------------------------------------------------

void CQueryHist::GetQuery_Error(long ErrorNumber)
  {
  XMr.Clear();
  CXM_HistoryDataError *xb=new CXM_HistoryDataError (ErrorNumber, lRqstNo, 0.0, "");
  VERIFY(XMr.PackMsg(xb));
  pHistory->XSendMessage(XMr, xRoute);
  }

//---------------------------------------------------------------------------

void CQueryHist::GetQuery()
  {
  if (QueryDst!=QTrend)
    GetQuery_PartOne();

  if (iTagCnt==0 || dStartTime>=dLastTime || (QueryDst==QFileDirect && fFile==NULL))
    return;

  if (QueryDst!=QTrend)
    GetQuery_PartTwo();

  //determine range of cataloged historian data files required...
  long StartIndex = pHistory->Cat.GetCatItemByStartTime(dStartTime);
  long LastIndex = pHistory->Cat.GetCatItemByLastTime(dLastTime);

  //safty check: end query for illegal values...
  if (StartIndex>LastIndex)
    {
    ASSERT(FALSE); //should never get here, can continue...
    if (QueryDst!=QTrend)
      GetQuery_PartThree();
    return;
    }

  //debug dump...
  #if dbgHstQuery
  DbgDmpFile = NULL;
  if (dbgHistQuery())
    {
    char FnTmp[128];
    for (int ii=0; ii<64; ii++)
      {
      sprintf(FnTmp, "QRYDUMP.%03i", ii);
      WIN32_FIND_DATA fd;
      HANDLE FndHnd = FindFirstFile(FnTmp, &fd);
      if (FndHnd==INVALID_HANDLE_VALUE)
        break;
      else
        FindClose(FndHnd);
      }
    if (ii<64)
      {
      DbgDmpFile = new CFile();
      if (DbgDmpFile->Open(FnTmp, CFile::modeCreate|CFile::shareCompat|CFile::modeWrite))
        {
        char Buff[256];
        sprintf(Buff, "  Start time       :%14.3f\r\n", dStartTime);
        DbgDmpFile->Write(Buff,strlen(Buff));
        sprintf(Buff, "  Last time        :%14.3f\r\n", dLastTime);
        DbgDmpFile->Write(Buff,strlen(Buff));
        sprintf(Buff, "  Start index      :%d\r\n", StartIndex);
        DbgDmpFile->Write(Buff,strlen(Buff));
        sprintf(Buff, "  Last index       :%d\r\n", LastIndex);
        DbgDmpFile->Write(Buff,strlen(Buff));
        sprintf(Buff, "  Tag Count        :%d\r\n", iTagCnt);
        DbgDmpFile->Write(Buff,strlen(Buff));
        }
      else
        {
        delete DbgDmpFile;
        DbgDmpFile = NULL;
        }
      }
    }
  #endif

  //setup more for the query...
  XMr.Clear();
  bMustFlush = false;
  SlotIDs = new DWORD[iTagCnt];
  SlotFlags = new flag[iTagCnt];
  for (long i=0; i<iTagCnt; i++)
    SlotIDs[i] = pHistory->GetSlotByIndex(Slots[0][i])->ID();
  if (QueryDst!=QTrend)
    GetQuery_PartFour();

  //debug for timeing queries...
  #if dbgHstQueryTiming
  dbgpln("Query Start %li",GetTickCount());      
  QryCnt1=GetTickCount()*-1;
  #endif

  //get all the query data...
  flag OK = true;
  TRY
    {
    if (bBackward)
      {
      if (bGetGreater)
        OK = GetGreater(LastIndex);
      if (!OK)
        BlockUnavailable(OK);
      bGettingOuter = 0;
      for (long i=LastIndex; i>=StartIndex && OK; i--)
        {
        OK = Q.Setup(i);
        if (!OK)
          BlockUnavailable(OK);
        else
          Q.GetQuery();
        }
      if (OK && bGetLesser)
        {
        if (bMustFlush)
          {
          pHistory->XSendMessage(XMr, xRoute);
          if (gs_pQueryInfo)
            {
            CQueryInfoStats QI((long)this, 99.5, (xRoute.NoNodes()>3), 0);
            gs_pQueryInfo->SendMessage(WMU_HSTQRYUPDATE, 0, (LPARAM)&QI);
            }
          bMustFlush = 0;
          XMr.Clear();
          }
        OK = GetLesser(StartIndex);
        if (!OK)
          BlockUnavailable(OK);
        }
      }
    else
      {
      if (bGetLesser)
        OK = GetLesser(StartIndex);
      if (!OK)
        BlockUnavailable(OK);
      bGettingOuter = 0;
      for (long i=StartIndex; i<=LastIndex && OK; i++)
        {
        OK = Q.Setup(i);
        if (!OK)
          BlockUnavailable(OK);
        else
          Q.GetQuery();
        }
      if (OK && bGetGreater)
        {
        if (QueryDst==QOLE || QueryDst==QDDE || QueryDst==QFile || QueryDst==QFileDirect)
          SendSlice();
        if (bMustFlush)
          {
          pHistory->XSendMessage(XMr, xRoute);
          if (gs_pQueryInfo)
            {
            CQueryInfoStats QI((long)this, 99.5, (xRoute.NoNodes()>3), 0);
            gs_pQueryInfo->SendMessage(WMU_HSTQRYUPDATE, 0, (LPARAM)&QI);
            }
          bMustFlush = 0;
          XMr.Clear();
          }
        OK = GetGreater(LastIndex);
        if (!OK)
          BlockUnavailable(OK);
        }
      }

    //finish off the query...
    switch (QueryDst)
      {
      case QTrend:
        break;
      case QFile:
      case QFileDirect:
      case QDDE:
      case QOLE:
        {
        switch (QueryOpt)
          {
          case All:
            if (bBackward)
              dPrevTime[0] = dStartTime;
            else
              dPrevTime[0] = dLastTime;
            SendSlice();
            break;
          case AveEqualySpaced:
          case LastEqualySpaced:
            {
            DataUnion Dummy(0.0);
            RetVal(SlotIDs[0], dLastTime+1.0, Dummy); //forces last points out
            break;
            }
          }
        break;
        }
      }
    if (bMustFlush)
      {
      pHistory->XSendMessage(XMr, xRoute);
      if (gs_pQueryInfo)
        {
        CQueryInfoStats QI((long)this, 99.9, (xRoute.NoNodes()>3), 0);
        gs_pQueryInfo->SendMessage(WMU_HSTQRYUPDATE, 0, (LPARAM)&QI);
        }
      bMustFlush = 0;
      XMr.Clear();
      }
    }
  CATCH(CHstQueryException, pE)
    {
    switch (pE->lErrorNumber)
      {
      case 1: //query data block file could not be loaded
        {
        ASSERT(FALSE);//exception no longer used
        break;
        }
      case 2: //query is cancelled, exit
        {
        switch (QueryDst)
          {
          case QTrend:
            GetQuery_Error(pE->lErrorNumber);
            break;
          case QFile:
          case QFileDirect:
          case QOLE:
            if (bMustFlush)
              pHistory->XSendMessage(XMr, xRoute);//send what we have
          case QDDE:
            XMr.Clear();
            bMustFlush = 0;
            break;
          default: 
            ASSERT(FALSE);
          }
        break;
        }
      case 10: //error parsing data, assume corrupt data
      case 11:
      case 12:
      case 13:
        {
        CString sFilename;
        if (Q.pCatItem)
          Q.pCatItem->MakeFilename(sFilename);
        LogError("History", 0, "Data corrupt? Error %d %s", pE->lErrorNumber, sFilename.GetLength()>0 ? (const char*)sFilename : "");
        switch (QueryDst)
          {
          case QTrend:
            GetQuery_Error(pE->lErrorNumber);
            break;
          case QFile:
          case QFileDirect:
          case QOLE:
            if (bMustFlush)
              pHistory->XSendMessage(XMr, xRoute);//send what we have
          case QDDE:
            XMr.Clear();
            bMustFlush = 0;
            break;
          default: 
            ASSERT(FALSE);
          }
        break;
        }
      default:
        ASSERT(FALSE); //unknown error no
      }
    }
  END_CATCH

  if (QueryDst!=QTrend)
    GetQuery_PartFive();

  //free memory, tidy up...
  delete []SlotIDs;
  SlotIDs = NULL;
  delete []SlotFlags;
  SlotFlags = NULL;
  for (i=0; i<iTagCnt && i<Slots.GetCols(); i++)
    {
    if (Slots[2][i]!=0) //pointer to cnv string
      {
      char* p = (char*)(Slots[2][i]);
      delete []p;
      }
    }
  Slots.SetSize(4, 0);

  //debug stuff...
  
  #if dbgHstQueryTiming
  QryCnt1+=GetTickCount();
  dbgpln("Query End   %li",GetTickCount());      
  dbgpln("      Total %9.3f",QryCnt1/1000.0);      
  #endif
  #if dbgHstQuery
  if (dbgHistQuery())
    if (DbgDmpFile)
      {
      DbgDmpFile->Close();
      delete DbgDmpFile;
      DbgDmpFile = NULL;
      }
  #endif
  }

//---------------------------------------------------------------------------

long CQueryHist::GetIndex(DWORD ID)
  {
  for (long i=0; i<iTagCnt; i++)
    if (SlotIDs[i] == ID)
      return i;
  return -1;
  }

//---------------------------------------------------------------------------

inline long RoundLong(double d)
  {
	if (d-floor(d)<0.5)
	  return (long)(floor(d));
	else
	  return (long)(ceil(d));
  }

void CQueryHist::RetVal(DWORD ID, double Time, rDataUnion Val)
  {
  #if dbgHstQuery
  if (dbgHistQuery())
    if (DbgDmpFile)
      {
      char Buff[256];
      CSlot* pSlot = pHistory->GetSlot(ID);
      sprintf(Buff, "%8d:%16.4f|%14s  %s\r\n", ID, Time, Val.GetString("%12i", "%16.4f"), pSlot->Data.sTag);
      DbgDmpFile->Write(Buff, strlen(Buff));
      }
  #endif
  
  Time -= dRepTimeOffset;

  if (bCancel)
    {
    CHstQueryException* pE = new CHstQueryException();
    pE->lErrorNumber = 2L;
    THROW(pE);
    }

  if (!bStarted)
    {
    bStarted = 1;
    //send message to say data is about to arrive...
    //XMr.Clear();
    //CXM_HistoryDataError xb(???, lRqstNo, 0.0, "");
    //VERIFY(XMr.PackMsg(xb));
    //pHistory->XSendMessage(XMr, xRoute);
    }

  long Index = GetIndex(ID);
  switch (QueryDst)
    {
    case QTrend:
      {
      CXM_HistoryData *xb=new CXM_HistoryData(Slots[1][Index], lRqstNo, Time, Val.GetDouble(), Val.Status());
      if (XMr.PackMsg(xb))
        bMustFlush = true;
      else
        {
        pHistory->XSendMessage(XMr, xRoute);
        double per = dLastTime - dStartTime;
        if (bBackward)
          per = (dLastTime - Time) / per * 100.0;
        else
          per = (Time - dStartTime) / per * 100.0;
        if (gs_pQueryInfo)
          {
          CQueryInfoStats QI((long)this, per, (xRoute.NoNodes()>3), 0);
          gs_pQueryInfo->SendMessage(WMU_HSTQRYUPDATE, 0, (LPARAM)&QI);
          }
        XMr.Clear();
        bMustFlush = XMr.PackMsg(xb);
        }
      break;
      }
    case QFile:
    case QFileDirect:
    case QDDE:
    case QOLE:
      {
      switch (QueryOpt)
        {
        case All:
          {
          if (dPrevTime[0]!=Time && !bGettingOuter && !bPrevTimeStartStop)
            SendSlice();
          if (Val.Status()==ds_NAN)
            {//solver start or stop
            bPrevTimeStartStop = 1;
            }
          else
            {
            bPrevTimeStartStop = 0;
            }
          dPrevTime[0] = Time;
          if (!bPrevTimeStartStop)
            {
            if (IsFloatData((byte)Slots[3][Index]))
              DData[Index] = Val.GetDouble();
            else if (IsIntData((byte)Slots[3][Index]))
              LData[Index] = Val.GetLong();
            else
              SData[Index] = Val.GetString();
            }
          break;
          }
        case AveEqualySpaced:
        case LastEqualySpaced:
          {
          if (!bGettingOuter)
            {
            if (QueryOpt==LastEqualySpaced || (!bDoneHeadings && bDoLastValForFirstPoint))
              {
              while (Time>dWrkTim)
                {
                for (int i=0; i<iTagCnt; i++)
                  {
                  if (IsNumData((byte)Slots[3][i]))
                    {
                    if (IsFloatData((byte)Slots[3][i]))
                      DData[i] = dPrevVal[i];
                    else if (IsIntData((byte)Slots[3][i]))
                      LData[i] = RoundLong(dPrevVal[i]);
                    }
                  dPrevTime[i] = dWrkTim;
                  }
                SendSlice();
                dPrevTime.SetAll(dWrkTim);
                dWrkTim += dTimeDelta;
                }
              dPrevTime[Index] = Time;
              }
            else
              {//AveEqualySpaced
              double dT;
              if (Time>dWrkTim)
                {
                while (Time>dWrkTim)
                  {
                  for (int i=0; i<iTagCnt; i++)
                    {
                    if (IsNumData((byte)Slots[3][i]))
                      {
                      dT = dWrkTim - dPrevTime[i];
                      dSigVal[i] += (dPrevVal[i] * dT);
                      if (IsFloatData((byte)Slots[3][i]))
                        DData[i] = dSigVal[i]/dTimeDelta;
                      else if (IsIntData((byte)Slots[3][i]))
                        LData[i] = RoundLong(dSigVal[i]/dTimeDelta);
                      }
                    dPrevTime[i] = dWrkTim;
                    }
                  SendSlice();
                  dSigVal.SetAll(0.0);
                  dPrevTime.SetAll(dWrkTim);
                  dT = Time - dWrkTim;
                  dWrkTim += dTimeDelta;
                  }
                if (IsNumData((byte)Slots[3][Index]))
                  dSigVal[Index] = dPrevVal[Index] * dT;
                }
              else
                {
                if (IsNumData((byte)Slots[3][Index]))
                  {
                  dT = Time - dPrevTime[Index];
                  dSigVal[Index] += (dPrevVal[Index] * dT);
                  }
                }
              dPrevTime[Index] = Time;
              }
            }
          if (Val.Status()==ds_NAN)
            {//solver start or stop
            //assume previous value is OK
            }
          else if (Val.Status()!=ds_OK)
            {
            dPrevVal[Index] = 0.0; //assume 0
            }
          else
            {
            if (IsFloatData((byte)Slots[3][Index]))
              dPrevVal[Index] = Val.GetDouble();
            else if (IsIntData((byte)Slots[3][Index]))
              dPrevVal[Index] = Val.GetLong();
            else
              SData[Index] = Val.GetString();
            }
          break;
          }
        default:
          ASSERT(FALSE);
        }
      break;
      }
    default:
      ASSERT(FALSE);
    }
  }

//===========================================================================
