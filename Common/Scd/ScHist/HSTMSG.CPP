//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"

#ifndef  __HSTMSG_CPP
#define  __HSTMSG_CPP

#if ForSysCAD
#include "sc_defs.h"
#endif
#include "hstmsg.h"

//========================================================================
//NB Do NOT change CopyNotice!
const char* CopyNotice = "SysCAD 7.1 - Historian file - Copyright (C) Kenwalt 1995                      \r\n";
                        //012345678901234567890123456789012345678901234567890123456789012345678901234567890
                        //          1         2         3         4         5         6         7         8
const char* NewBlkKey = "NEW_BLK_NOT_INITIALISED!!!@#$%^&*xXxX";

//===========================================================================
 
SlotCfgInfo::SlotCfgInfo()
  {
  mDriver=false;
  mRecordIt=true;
  mTag=NULL;
  mTyp=tt_NULL;
  mUseFilter=false;
  mFilterAlg=HSFA_None;
  mFilterTau=0.0;
  mWinFltPeriod=0.0;
  mWinFltCount=0;
  mFltDelta=0.0;
  mUseBoxcar=false;
  mBoxcarAlg=HSA_None;
  mDeltaLo=0.0;
  mDeltaHi=0.0;
  mNoRec=0;
  mNoNotRec=0;
  mChange=0.5;
  mEng=NULL;
  mDesc=NULL;
  };

//========================================================================

HD_Hist::HD_Hist()
  {
  lReqdBlkSize = DefaultBlkSize;
  lSlotsUsed = 0;
  bWrapAround = false;
  iMaxDeltaTimeCnt = DefaultTimeCnt;  
  lMaxTimeSliceLen = DefaultSliceSize;
  iCurCatNo = 0;
  strncpy(sScenName, "scn1", strlen("scn1")+1);//sizeof(sScenName));
  memcpy(sCopyNotice, CopyNotice, sizeof(sCopyNotice));
  memset(sSpare, 0, sizeof(sSpare));
  }

//---------------------------------------------------------------------------

void HD_Hist::dbgDump(char* p)
  {
  dbgpln("Historian header:[%s]", p);
  dbgpln("Slots used        :%d", lSlotsUsed);
  dbgpln("Current Catalog No:%d", iCurCatNo);
  dbgpln("Next slot ID      :%d", lNxtSlotID);
  dbgpln("Scenario name     :%s", sScenName);
  dbgpln("Is SyncWithClock  :%d", bSyncWithClock);
  dbgpln("Did close         :%d", bDidClose);
  }

//========================================================================

char* HD_SlotInfo::Tag()
  {
  static char Buff[sizeof(sTag)+1];
  strncpy(Buff, sTag, sizeof(sTag));
  Buff[sizeof(sTag)] = 0;
  return Buff;
  }

//---------------------------------------------------------------------------

char* HD_SlotInfo::Eng()
  {
  static char Buff[sizeof(sEng)+1];
  strncpy(Buff, sEng, sizeof(sEng));
  Buff[sizeof(sEng)] = 0;
  return Buff;
  }

//---------------------------------------------------------------------------

char* HD_SlotInfo::CnvFam()
  {
  static char Buff[sizeof(sCnvFam)+1];
  strncpy(Buff, sCnvFam, sizeof(sCnvFam));
  Buff[sizeof(sCnvFam)] = 0;
  return Buff;
  }

//---------------------------------------------------------------------------

char* HD_SlotInfo::Desc()
  {
  static char Buff[sizeof(sDesc)+1];
  strncpy(Buff, sDesc, sizeof(sDesc));
  Buff[sizeof(sDesc)] = 0;
  return Buff;
  }

//---------------------------------------------------------------------------

void HD_SlotInfo::SetTag(char* pTag)
  {
  strncpy(sTag, pTag, sizeof(sTag));
  sTag[sizeof(sTag)-1] = 0;
  }

//---------------------------------------------------------------------------

void HD_SlotInfo::dbgDump()
  {
  dbgpln("Unique Slot ID:%d", lSlotID);
  dbgpln("tt_ var type  :%d", VarTyp);
  dbgpln("tag           :%s", Tag());
  dbgpln("description   :%s", sDesc);
  dbgpln("Use filter    :%d", bUseFilter);
  dbgpln("Use boxcar    :%d", bUseBoxcar);
  //char         sEng[32];
  //DataUnion    MinValue;
  //DataUnion    MaxValue;
  //DataUnion    PrevSaveVal;   //previous value (filtered etc) saved to disk
  //DataUnion    PrevMeasVal;   //previous value (filtered etc)
  //DataUnion    PrevActualVal; //previous value (filtered etc)
  //double       dPrevTime;     //time of prev meas val
  //byte         iPrevStatus;   //status of prev meas val
  }

//========================================================================

HD_CatHead::HD_CatHead()
  { 
  iCatItemCnt = 0; 
  iNextCatNo_ = 0;
  memcpy(sCopyNotice, CopyNotice, sizeof(sCopyNotice));
  memset(sSpare, 0, sizeof(sSpare));
  }

//---------------------------------------------------------------------------

void HD_CatHead::dbgDump()
  {
  dbgpln("Ttl no cat items :%d", iCatItemCnt);
  //dbgpln("Next cat no      :%d", iNextCatNo_);
  //  char      sCopyNotice[80]; //Kenwalt copyright notice
  //  byte      iVerNo;          //historian version number
  //  char      sSpare[200];     //space for future stuff to allow backward compatability
  }

//========================================================================
//
//
//
//===========================================================================

// This converts a time in seconds into the corresponding date ( day/month/year ) and time ( hour:minute:second. hundredth of a second).
pchar SecstoDateHMS(double time_in, // the time in seconds.
                    pchar buff,  // the buffer into which the result is to be stored.
                    flag WithDate)// if True it returns the date followed by the time, if False only the time is returned.
  {
  if (!WithDate)
    return SecstoHMS(time_in, buff);
  __time64_t tt=(__time64_t)(time_in);
  tm* t=_localtime64(&tt);
  int hsec=(int)((time_in-tt)*100.0);
  if (t)
    sprintf(buff, "%02i/%02i/%04i  %02i:%02i:%02i.%02i",t->tm_mday,t->tm_mon+1,t->tm_year+1900,t->tm_hour,t->tm_min,t->tm_sec,hsec);
  else
    strcpy(buff, "?");
  return buff;
  };

//---------------------------------------------------------------------------
// This converts a time in seconds into the corresponding time ( hour:minute:second. hundredth of a second).
pchar SecstoHMS(double time_in,// the time in seconds
                pchar buff,  // the buffer in which the result is to be stored.
                flag TruncDate, //must the date be ignored ie max hours=23
                flag TruncMilliSecs)
  {
  long t=(long)time_in;
  long hsecs=(long)((time_in-t)*100.0);
  long secs=t%60;
  long mins=(t/60)%60;
  long hours=t/3600; //does not work ??? TIME ZONES !!!
  if (TruncDate)
    hours=hours%24;

  if (TruncMilliSecs)
    {
    sprintf(buff, "%02i:%02i:%02i", hours, mins, secs);
    }
  else
    {
    long hsecs=(long)((time_in-t)*100.0);
    sprintf(buff, "%02i:%02i:%02i.%02i", hours, mins, secs, hsecs);
    }
  return buff;
  };

//---------------------------------------------------------------------------
// This converts a time in seconds into the corresponding time ( hour:minute:second. hundredth of a second) and date ( day/month/year ).
pchar SecstoHMSDate(double time_in, // the time in seconds.
                    pchar buff,  // the buffer in which the result is to be stored.
                    flag WithDate)// if True it returns the date followed by the time, if False only the time is returned.
  {
  //time_in = floor(time_in*1.0e6+0.5)/1.0e6; //round for lost precision on LSD
  if (!WithDate)
    return SecstoHMS(time_in, buff);
  __time64_t tt=(__time64_t)(time_in);
  tm* t=_localtime64(&tt);
  int hsec=(int)((time_in-tt)*100.0);
  if (t)
    sprintf(buff, "%02i:%02i:%02i.%02i  %02i/%02i/%04i",t->tm_hour,t->tm_min,t->tm_sec,hsec,t->tm_mday,t->tm_mon+1,t->tm_year+1900);
  else
    strcpy(buff, "?");
  //TRACE2("Secs>> %15.2g %s",time_in, buff);
  return buff;
  };

//========================================================================
//
//
//
//===========================================================================

#endif
