//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#if ForSysCAD
#include "sc_defs.h"
#include "errorlog.h"
#else
#include <math.h>
#endif
#include "hstblks.h"
#include "hstslots.h"
#ifndef __AFXTEMPL_H__
  #include <afxtempl.h> // "xafxtempl.h"
#endif
#include "hstmain.h"

#define  dbgHistorian  1

#if dbgHistorian
#include "dbgmngr.h"
static CDbgMngr dbgHistTag("Historian", "HistTag");
#endif

//===========================================================================

IMPLEMENT_MEMSTATS(CFilter)
IMPLEMENT_MEMSTATS(CBoxcar)
IMPLEMENT_MEMSTATS(CSlot)

//===========================================================================

CFilter::CFilter()
  {
  memset(&Data, 0, sizeof(Data));
  iSampleNo = 0;
  iNSamples = 0;
  pList = NULL;
  iListLen = 0;
  dSigma = 1.0;
  pSlot = NULL;
  }

//---------------------------------------------------------------------------

CFilter::~CFilter()
  {
  if (pList)
    delete []pList;
  }

//---------------------------------------------------------------------------

void CFilter::Cfg(pSlotCfgInfo pSCI, pCSlot Slot)
  {
  pSlot = Slot;
  if (pSCI)
    {
    Data.dFilterTau = pSCI->mFilterTau;
    Data.dWinFltPeriod = pSCI->mWinFltPeriod;
    Data.iWinFltCount = pSCI->mWinFltCount;
    Data.iFilterAlg = pSCI->mFilterAlg;
    Data.FltDeltaVal.SetTypeDouble(pSlot->Data.VarTyp, pSCI->mFltDelta);
    }

  if ( (Data.dWinFltPeriod > 0) &&
       (Data.dFilterTau >= Data.dWinFltPeriod) )
    dAlpha = Data.dWinFltPeriod / Data.dFilterTau;
  else
    dAlpha = 1.0;
  FixList();
  }

//---------------------------------------------------------------------------

void CFilter::FixList()
  {
  Data.iWinFltCount = Range((short)0, Data.iWinFltCount, (short)100);
  if (Data.iWinFltCount != iListLen)
    {
    iListLen = Data.iWinFltCount;
    if (pList)
      delete[] pList;
    pList = NULL;
    iNSamples = 0;
    iSampleNo = 0;
    if (iListLen > 0)
      {
      pList = new DataUnion[iListLen];
      for (int i=0; i<iListLen; i++)
        pList[i].SetTypeDouble(pSlot->Data.VarTyp, 0.0);
      }
    }
  }

//---------------------------------------------------------------------------

void CFilter::DoFilter()
  {
  flag IsFirst = pSlot->bFirstValue;
  if (pSlot->Data.iPrevStatus == TS_STATUS_INVALID)
    {
    iNSamples = 0;
    iSampleNo = 0;
    IsFirst = true;
    }
  if ((Data.iFilterAlg & HSFA_MovingAve) != 0)
    {
    if (Data.iWinFltCount > 1)
      {
      switch (pSlot->TSFlag)
        {
        case TS_BIT   : break; //meaningless to filter it
        case TS_DOUBLE:
        case TS_FLOAT :
        case TS_BYTE  :
        case TS_WORD  :
        case TS_DWORD :
        case TS_INT16 :
        case TS_LONG  :
        case TS_FLT16 :
          {
          long diff = pHistory->lScanNo - pSlot->lScanOfLastChange;
          flag ForceCalc = (diff>1);
          if (ForceCalc && !IsFirst)
            {//value did not change between two consecutive iterations...
            diff = min(Data.iWinFltCount, (short)diff) - 1;
            short PrevNo = (iSampleNo + Data.iWinFltCount - 1) % Data.iWinFltCount;
            for (short i=0; i<diff; i++)
              {
              if (iNSamples < Data.iWinFltCount)
                iNSamples++;
              pList[iSampleNo] = pList[PrevNo];
              iSampleNo = (iSampleNo+1) % Data.iWinFltCount;
              }
            }
          if (iNSamples < Data.iWinFltCount)
            iNSamples++;
          if (ForceCalc || iNSamples < Data.iWinFltCount || iSampleNo==0)   // To Prevent Rounding Errors Accumulating
            {
            pList[iSampleNo] = pSlot->MeasVal;
            dSigma = 0.0;
            for (int i=0; i<iNSamples; i++)
              dSigma += pList[i].GetDouble();
            }
          else
            {
            dSigma -= pList[iSampleNo].GetDouble();
            pList[iSampleNo] = pSlot->MeasVal;
            dSigma += pList[iSampleNo].GetDouble();
            }

          iSampleNo = (iSampleNo+1) % Data.iWinFltCount;
          pSlot->MeasVal.SetTypeDouble(pSlot->Data.VarTyp, dSigma / iNSamples);
          break;
          }
        default: ASSERT(FALSE);
        }
      }
    }

  if ((Data.iFilterAlg & HSFA_Tau)!=0)
    {
    if (dAlpha < 1.0 && !IsFirst)
      {
      switch (pSlot->TSFlag)
        {
        case TS_BIT   : break; //meaningless to filter it
        case TS_DOUBLE:
        case TS_FLOAT :
        case TS_BYTE  :
        case TS_WORD  :
        case TS_DWORD :
        case TS_INT16 :
        case TS_LONG  :
        case TS_FLT16 :
          {
          double Mv = pSlot->Data.PrevMeasVal.GetDouble();
          double Nv = pSlot->MeasVal.GetDouble();
          if (fabs(Nv-Mv) < Data.FltDeltaVal.GetDouble())
            {
            Nv = dAlpha * Nv + (1.0 - dAlpha) * Mv;
            pSlot->MeasVal.SetTypeDouble(pSlot->Data.VarTyp, Nv);
            }
          }
          break;
        default : ASSERT(FALSE);
          break;
        }
      }
    }
  }

//---------------------------------------------------------------------------

flag CFilter::Save(CFile & F)
  {
  F.Write((void*)&(Data), sizeof(HD_FilterInfo));
  return true;
  }

//---------------------------------------------------------------------------

flag CFilter::Load(CFile & F)
  {
  F.Read((void*)&(Data), sizeof(HD_FilterInfo));
  return true;
  }

//===========================================================================

CBoxcar::CBoxcar()
  {
  memset(&Data, 0, sizeof(Data));
  pSlot = NULL;
  }

//---------------------------------------------------------------------------

void CBoxcar::Cfg(pSlotCfgInfo pSCI, pCSlot Slot)
  {
  pSlot = Slot;
  if (pSCI)
    {
    Data.dDeltaLo = pSCI->mDeltaLo;
    Data.dDeltaHi = Max(pSCI->mDeltaHi, Data.dDeltaLo);
    Data.dChange = pSCI->mChange;
    Data.iNoRec = pSCI->mNoRec;
    Data.iNoNotRec = pSCI->mNoNotRec;
    Data.iBoxcarAlg = pSCI->mBoxcarAlg;
    }
  AveVal = 0.0;
  AveCnt = 0L;
  NoRec = 0;
  NoNotRec = 0;
  Data.dDeltaVal = (Data.dDeltaLo + Data.dDeltaHi) / 2.0;
  }

//---------------------------------------------------------------------------

flag CBoxcar::CheckChanges()
  {
  AveVal = (AveVal * AveCnt);
  long diff = pHistory->lScanNo - pSlot->lScanOfLastChange;
  AveCnt += diff;
  AveVal = (AveVal + (pSlot->MeasVal.GetDouble() * diff)) / AveCnt;

  flag Changed = false;
  switch(Data.iBoxcarAlg)
    {
    case HSA_Boxcar:
      switch (pSlot->TSFlag)
        {
        case TS_DOUBLE:
        case TS_FLOAT :
        case TS_BYTE  :
        case TS_WORD  :
        case TS_DWORD :
        case TS_INT16 :
        case TS_LONG  :
        case TS_FLT16 :
          {
          if (fabs(pSlot->Data.PrevSaveVal.GetDouble() - AveVal) > Data.dDeltaLo)
            Changed = true;
          break;
          }
        case TS_BIT:
          if (pSlot->MeasVal.GetLong() != pSlot->Data.PrevSaveVal.GetLong())
            Changed = true;
        default: ASSERT(FALSE); //type not supported ???
        }
      break;
    case HSA_AdaptiveBoxcar:
      switch (pSlot->TSFlag)
        {
        case TS_DOUBLE:
        case TS_FLOAT :
        case TS_BYTE  :
        case TS_WORD  :
        case TS_DWORD :
        case TS_INT16 :
        case TS_LONG  :
        case TS_FLT16 :
          {
          if (fabs(pSlot->Data.PrevSaveVal.GetDouble() - AveVal) > Data.dDeltaVal)
            Changed = true;
          break;
          }
        case TS_BIT:
          if (pSlot->MeasVal.GetLong() != pSlot->Data.PrevSaveVal.GetLong())
            Changed = true;
        default: ASSERT(FALSE); //type not supported ???
        }
      if (Changed)
        {
        NoRec++;
        if (NoRec>Data.iNoRec)
          {
          Data.dDeltaVal = Min(Data.dDeltaVal+Data.dChange, Data.dDeltaHi);
          NoRec = 1;
          }
        NoNotRec = 0;
        }
      else
        {
        NoNotRec++;
        if (NoNotRec>Data.iNoNotRec)
          {
          Data.dDeltaVal = Max(Data.dDeltaVal-Data.dChange, Data.dDeltaLo);
          NoNotRec = 1;
          }
        NoRec = 0;
        }

      break;
    case HSA_None:
      break;
    }

  return Changed;
  }


//---------------------------------------------------------------------------

flag CBoxcar::Save(CFile & F)
  {
  F.Write((void*)&(Data), sizeof(HD_BoxcarInfo));
  return true;
  }

//---------------------------------------------------------------------------

flag CBoxcar::Load(CFile & F)
  {
  F.Read((void*)&(Data), sizeof(HD_BoxcarInfo));
  return true;
  }

//===========================================================================

CSlot::CSlot()
  {
  Initialise();
  }

//---------------------------------------------------------------------------

CSlot::~CSlot()
  {
  if (pFilter)
    delete pFilter;
  if (pBoxcar)
    delete pBoxcar;
  }

//---------------------------------------------------------------------------

void CSlot::Initialise()
  {
  memset(&Data, 0, sizeof(Data));
  Data.lSlotID = 0xFFFFFFFF;
  lSNo = -1;
  iStatus = 0;
  TSFlag = 0;
  bFirstValue = false;
  bIsRecording = false;
  m_bVariesWithinTol=false;
  m_bChangedDuringPause=false;
  pFilter = NULL;
  pBoxcar = NULL;
  lScanOfLastSave = pHistory->lScanNo;
  lScanOfLastChange = pHistory->lScanNo;
  dwWriteCnt = 0;
  dwChangeCnt = 0;
  iCnvNo = 0;
  }

//---------------------------------------------------------------------------

void CSlot::Configure(DWORD SlotID, long SNo)
  {//configures NEW slot
  bWriteMidTime = 1;
  bFirstValue = 1;
  m_bVariesWithinTol=false;
  m_bChangedDuringPause=false;
  Data.lSlotID = SlotID;
  lSNo = SNo;
  }

//---------------------------------------------------------------------------

void CSlot::Configure(long SNo)
  {//configures OLD slot
  bFirstValue = false;
  m_bVariesWithinTol=false;
  m_bChangedDuringPause=false;
  lSNo = SNo;
  switch (Data.VarTyp)
    {
    case tt_Bool   :
    case tt_Bit    : TSFlag = TS_BIT; break;
    case tt_Char   :
    case tt_Byte   : TSFlag = TS_BYTE; break;
    case tt_Short  : TSFlag = TS_INT16; break;
    case tt_Word   : TSFlag = TS_WORD; break;
    case tt_DWord  : TSFlag = TS_DWORD; break;
    case tt_Long   : TSFlag = TS_LONG; break;
    case tt_Float  : TSFlag = TS_FLOAT; break;
    case tt_Double : TSFlag = TS_DOUBLE; break;
    case tt_Flt16  : TSFlag = TS_FLT16; break;
    case tt_Int    : TSFlag = TS_LONG; break;
    case tt_Strng  : TSFlag = TS_STR; break;
    default: ASSERT(FALSE); //data type not supported
    }
  }

//---------------------------------------------------------------------------

void CSlot::Cfg(pSlotCfgInfo pSCI, flag NewSlot)
  {
  #if dbgHistorian && DBGMANAGER
  if (dbgHistTag())
    {
    if (dbgHistTag.Items.Length()>0 && dbgHistTag.Items.Find(Data.sTag)!=NULL)
      { //place breakpoint here to see what is happening with a specific tag specified in syscad.dmf
      dbgpln("Historian: tag %s configure", Data.sTag);
      }
    }
  #endif
  ASSERT(IsNumData(pSCI->mTyp) || (/*AllowHistStr &&*/ IsStrng(pSCI->mTyp)));
  if (NewSlot)
    {
    Data.VarTyp = pSCI->mTyp;
    Data.SetTag(pSCI->mTag);
    bFirstValue = true;
    m_bVariesWithinTol=false;
    m_bChangedDuringPause=false;
    Data.PrevMeasVal.SetTypeDouble(Data.VarTyp, 0.0);
    Data.PrevMeasVal.SetStatus(ds_NAN);
    Data.PrevSaveVal.SetTypeDouble(Data.VarTyp, 0.0);
    Data.PrevSaveVal.SetStatus(ds_NAN);
    Data.PrevActualVal.SetTypeDouble(Data.VarTyp, 0.0);
    Data.PrevActualVal.SetStatus(ds_NAN);
    Data.dPrevTime = 0.0;
    Data.iPrevStatus = TS_STATUS_INVALID;
    MeasVal.SetTypeDouble(Data.VarTyp, 0.0);
    MeasVal.SetStatus(ds_NAN);
    switch (Data.VarTyp)
      {
      case tt_Bool   :
      case tt_Bit    : TSFlag = TS_BIT; break;
      case tt_Char   :
      case tt_Byte   : TSFlag = TS_BYTE; break;
      case tt_Short  : TSFlag = TS_INT16; break;
      case tt_Word   : TSFlag = TS_WORD; break;
      case tt_DWord  : TSFlag = TS_DWORD; break;
      case tt_Long   : TSFlag = TS_LONG; break;
      case tt_Float  : TSFlag = TS_FLOAT; break;
      case tt_Double : TSFlag = TS_DOUBLE; break;
      case tt_Flt16  : TSFlag = TS_FLT16; break;
      case tt_Int    : TSFlag = TS_LONG; break;
      case tt_Strng  : TSFlag = TS_STR; break;
      default: ASSERT(FALSE); //data type not supported
      }
    }
  else
    {
    //ASSERT(Data.VarTyp==pSCI->mTyp); //cannot change data type !!!
    if (Data.VarTyp != pSCI->mTyp)
      {
      LogWarning("History", 0, "Tag '%s' changing data type!", pSCI->mTag);
      Data.VarTyp = pSCI->mTyp;
      switch (Data.VarTyp)
        {
        case tt_Bool   :
        case tt_Bit    : TSFlag = TS_BIT; break;
        case tt_Char   :
        case tt_Byte   : TSFlag = TS_BYTE; break;
        case tt_Short  : TSFlag = TS_INT16; break;
        case tt_Word   : TSFlag = TS_WORD; break;
        case tt_DWord  : TSFlag = TS_DWORD; break;
        case tt_Long   : TSFlag = TS_LONG; break;
        case tt_Float  : TSFlag = TS_FLOAT; break;
        case tt_Double : TSFlag = TS_DOUBLE; break;
        case tt_Flt16  : TSFlag = TS_FLT16; break;
        case tt_Int    : TSFlag = TS_LONG; break;
        case tt_Strng  : TSFlag = TS_STR; break;
        default: ASSERT(FALSE); //data type not supported
        }
      FixTyp();
      }
    }
  //general stuff...
  strncpy(Data.sEng, pSCI->mEng, sizeof(Data.sEng));
  strncpy(Data.sCnvFam, pSCI->mCnvFam, sizeof(Data.sCnvFam));
  strncpy(Data.sDesc, pSCI->mDesc, sizeof(Data.sDesc));
  Data.bDriver = pSCI->mDriver;
  Data.bRecordIt = pSCI->mRecordIt;

  //filter stuff...
  Data.bUseFilter = pSCI->mUseFilter;
  if (Data.bUseFilter)
    {
    if (pFilter==NULL)
      pFilter = new CFilter();
    pFilter->Cfg(pSCI, this);
    }
  else
    {
    if (pFilter)
      {
      delete pFilter;
      pFilter = NULL;
      }
    }

  //boxcar stuff...
  Data.bUseBoxcar = pSCI->mUseBoxcar;
  if (Data.bUseBoxcar)
    {
    if (pBoxcar==NULL)
      pBoxcar = new CBoxcar();
    pBoxcar->Cfg(pSCI, this);
    }
  else
    {
    if (pBoxcar)
      {
      delete pBoxcar;
      pBoxcar = NULL;
      }
    }

  MeasVal = Data.PrevMeasVal;
  if (NewSlot)
    {
    lScanOfLastChange = pHistory->lScanNo;
    lScanOfLastSave = pHistory->lScanNo;
    }
  else
    {
    lScanOfLastChange = pHistory->lScanNo - 2;
    lScanOfLastSave = pHistory->lScanNo - 2;
    }
#if ForSysCAD
  iCnvNo = gs_CnvsMngr.FindCnv(Data.sCnvFam);
#endif
  if (iCnvNo<0)
    iCnvNo = 0;
  FixTyp(); //might as well check it
  }

//---------------------------------------------------------------------------

void CSlot::GetCfgInfo(SlotCfgInfo& SCI)
  {
  SCI.mTag = Data.sTag;
  SCI.mTyp = Data.VarTyp;
  SCI.mEng = Data.sEng;
  SCI.mCnvFam = Data.sCnvFam;
  SCI.mDesc = Data.sDesc;
  SCI.mDriver = Data.bDriver;
  SCI.mRecordIt = Data.bRecordIt;
  SCI.mUseFilter = Data.bUseFilter;
  if (Data.bUseFilter)
    {
    ASSERT(pFilter);
    SCI.mFilterAlg = pFilter->Data.iFilterAlg;
    SCI.mFilterTau = pFilter->Data.dFilterTau;
    SCI.mWinFltPeriod = pFilter->Data.dWinFltPeriod;
    SCI.mWinFltCount = pFilter->Data.iWinFltCount;
    SCI.mFltDelta = pFilter->Data.FltDeltaVal.GetDouble();
    }
  else
    {
    SCI.mFilterAlg = 0;
    SCI.mFilterTau= 0.0;
    SCI.mWinFltPeriod = 0.0;
    SCI.mWinFltCount = 0;
    SCI.mFltDelta = 0.0;
    }
  SCI.mUseBoxcar = Data.bUseBoxcar;
  if (Data.bUseBoxcar)
    {
    ASSERT(pBoxcar);
    SCI.mBoxcarAlg = pBoxcar->Data.iBoxcarAlg;
    SCI.mDeltaLo = pBoxcar->Data.dDeltaLo;
    SCI.mDeltaHi = pBoxcar->Data.dDeltaHi;
    SCI.mNoRec = pBoxcar->Data.iNoRec;
    SCI.mNoNotRec = pBoxcar->Data.iNoNotRec;
    SCI.mChange = pBoxcar->Data.dChange;
    }
  else
    {
    SCI.mBoxcarAlg = 0;
    SCI.mDeltaLo = 0.0;
    SCI.mDeltaHi = 0.0;
    SCI.mNoRec = 0;
    SCI.mNoNotRec = 0;
    SCI.mChange = 0.0;
    }
  }

//---------------------------------------------------------------------------

/*void CSlot::ChangeBoxcar(byte BoxcarAlg, double DeltaLo, double DeltaHi, WORD NoRec, WORD NoNotRec, double Change)
  {
  Data.bUseBoxcar = (m_BoxcarAlg>0);
  if (Data.bUseBoxcar)
    {
    if (pBoxcar==NULL)
      pBoxcar = new CBoxcar();
    pBoxcar->Data.iBoxcarAlg = BoxcarAlg;
    pBoxcar->Data.dDeltaLo = DeltaLo;
    pBoxcar->Data.dDeltaHi = DeltaHi;
    pBoxcar->Data.dChange = Change;
    pBoxcar->Data.iNoNotRec = NoNotRec;
    pBoxcar->Data.iNoRec = NoRec;
    }
  }*/

//---------------------------------------------------------------------------

flag CSlot::Save(CFile & F, long SlotNo)
  {
  ASSERT(HD_SlotSpace >= sizeof(HD_FilterInfo) + sizeof(HD_BoxcarInfo) + sizeof(HD_SlotInfo));

  unsigned long Pos=sizeof(HD_Hist) + SlotNo * HD_SlotSpace;
  if (F.GetLength() < Pos)
    F.SetLength(Pos);
  F.Seek(Pos, CFile::begin);

  // cnm a KLUDGE the contents of these do not get saved correctly if they are strings.
  LPTSTR p0=Data.PrevActualVal.pChar;
  LPTSTR p1=Data.PrevMeasVal.pChar;
  LPTSTR p2=Data.PrevSaveVal.pChar;
  if (IsStrng(Data.VarTyp))
    { 
    Data.PrevActualVal.pChar=NULL;
    Data.PrevMeasVal.pChar=NULL;
    Data.PrevSaveVal.pChar=NULL;
    }

  F.Write((void*)&(Data), sizeof(HD_SlotInfo));

  // cnm a KLUDGE the contents of these do not get saved correctly if they are strings.
  if (IsStrng(Data.VarTyp))
    { 
    Data.PrevActualVal.pChar=p0;
    Data.PrevMeasVal.pChar=p1;
    Data.PrevSaveVal.pChar=p2;
    }

  if (Data.bUseFilter)
    pFilter->Save(F);
  if (Data.bUseBoxcar)
    pBoxcar->Save(F);
  return true;
  };

//---------------------------------------------------------------------------

flag CSlot::Load(CFile & F, long SlotNo)
  {
  unsigned long Pos=sizeof(HD_Hist) + SlotNo * HD_SlotSpace;
  if (F.GetLength() < Pos)
    {
    dbgpln("History Desc File too short");
    ASSERT(FALSE);
    return false;
    }
  F.Seek(Pos, CFile::begin);
  F.Read((void*)&(Data), sizeof(HD_SlotInfo));

  // cnm a KLUDGE the contents of these do not get saved correctly if they are strings.
  if (IsStrng(Data.VarTyp))
    { 
    Data.PrevActualVal.pChar=NULL;
    Data.PrevMeasVal.pChar=NULL;
    Data.PrevSaveVal.pChar=NULL;
    Data.PrevActualVal.Set("");
    Data.PrevMeasVal.Set("");
    Data.PrevSaveVal.Set("");
    }

  Configure(SlotNo);
  if (Data.bUseFilter)
    {
    pFilter = new CFilter();
    pFilter->Load(F);
    pFilter->Cfg(NULL, this);
    }
  if (Data.bUseBoxcar)
    {
    pBoxcar = new CBoxcar();
    pBoxcar->Load(F);
    pBoxcar->Cfg(NULL, this);
    }
#if ForSysCAD
  iCnvNo = gs_CnvsMngr.FindCnv(Data.sCnvFam);
#endif
  if (iCnvNo<0)
    iCnvNo = 0;
  return true;
  }

//---------------------------------------------------------------------------

void CSlot::Changed(flag Force)
  {
  #if dbgHistorian && DBGMANAGER
  if (dbgHistTag())
    {
    if (dbgHistTag.Items.Length()>0 && dbgHistTag.Items.Find(Data.sTag)!=NULL)
      { //place breakpoint here to see what is happening with a specific tag specified in syscad.dmf
      dbgpln("Historian: tag %s [%8i] changed:%12.3f  time:%15.3f", Data.sTag, pHistory->lScanNo, MeasVal.GetDouble(), pHistory->dTime);
      }
    }
  #endif

  ActualVal = MeasVal; //remember value before it gets changed by filtering

  if (pFilter)
    {
    if (iStatus != TS_STATUS_INVALID)
      pFilter->DoFilter();
    }

  flag Do = (Force || bFirstValue || (Data.iPrevStatus!=iStatus)); //always save for change of status

  if (pBoxcar)
    {
    if (bFirstValue)
      {
      pBoxcar->AveVal = 0.0;
      pBoxcar->AveCnt = 0L;
      pBoxcar->NoRec = 0;
      pBoxcar->NoNotRec = 0;
      pBoxcar->Data.dDeltaVal = Max(pBoxcar->Data.dDeltaLo, pBoxcar->Data.dDeltaVal);
      pBoxcar->Data.dDeltaVal = Min(pBoxcar->Data.dDeltaHi, pBoxcar->Data.dDeltaVal);
      }
    if (!Do)
      Do = pBoxcar->CheckChanges(); //because of boxcar compression, save may not be required
    }
  else
    {
    if (!Do && pHistory->bUseTolerance)
      {
      if (IsStrng(MeasVal.Type()))
        {
      //Do = (fabs((Data.PrevSaveVal.GetDouble()-MeasVal.GetDouble())/Data.PrevSaveVal.GetDouble())>pHistory->dRecordATol);
        Do = stricmp(Data.PrevSaveVal.GetString(), MeasVal.GetString())!=0;
        }
      else
        {
        double Err;
        Do = !ConvergedVV_NANOK(Data.PrevSaveVal.GetDouble(), MeasVal.GetDouble(), pHistory->dRecordATol, pHistory->dRecordRTol, Err);
        }
      }
    else
      Do = true;
    }

  if (!Do)
    {
    // CNM Should the logic bracketted AAAA below not maybe be moved here ??
    // and instead of PrevSaveVal the Current value should be used

    }

  if (pHistory->bUseTolerance && !IsStrng(Data.VarTyp))
    {
    m_bVariesWithinTol=m_bVariesWithinTol || (Data.PrevSaveVal.EqualNANOK(MeasVal)==0);
    }

  if (Do)
    {
    SaveEvent();
    bFirstValue = false;
    m_bVariesWithinTol=false;
    }

  Data.PrevActualVal = ActualVal;
  Data.PrevMeasVal = MeasVal;
  Data.iPrevStatus = iStatus;
  Data.dPrevTime = pHistory->dTime;

  lScanOfLastChange = pHistory->lScanNo;
  }

//---------------------------------------------------------------------------

void CSlot::SaveEvent()
  {
  dwWriteCnt++;
  long diff = pHistory->lScanNo - lScanOfLastSave;

  if (pHistory->bUseTolerance && m_bVariesWithinTol)
    {
    if (diff>1 && !bFirstValue && bWriteMidTime && TSFlag!=TS_STR)
      {//value has remained "unchanged" for some time, previous value (except for strings) must be stored first
      // also what does 'some time' mean ?  CNM
      byte TempStatus = iStatus;
      iStatus = Data.iPrevStatus;
      if (!pHistory->pCurEvBlk->AddEv(pHistory->dMidTime, lSNo, Data.PrevMeasVal))
        {
//        if (pHistory->bSwitchedOff)
//          return;
        if (!pHistory->pCurEvBlk->AddEv(pHistory->dMidTime, lSNo, Data.PrevMeasVal)) //try again
          { int xxx=0; };//return;
        }
      iStatus = TempStatus;
      #if dbgHistorian && DBGMANAGER
      if (dbgHistTag())
        {
        if (dbgHistTag.Items.Length()>0 && dbgHistTag.Items.Find(Data.sTag)!=NULL)
          { //place breakpoint here to see what is happening with a specific tag specified in syscad.dmf
          dbgpln("Historian: tag %s [%8i] saved:%12.3f", Data.sTag, pHistory->lScanNo, Data.PrevMeasVal.GetDouble());
          }
        }
      #endif
      }
    }
  
  
//  bool AllowSavePrev=true;
//  if ((pHistory->bUseTolerance || pBoxcar) && m_bVariesWithinTol)
//    AllowSavePrev=false;
//
//  //AAAA<<<<<<<<<<<<<<<<<<<<<<<<<
//  //note, trend has steps (eg a level) instead of being smooth if tolerance is used so that not all points are saved
//  // CNM the addition of 'pHistory->bUseTolerance' removes steps in the trends
//  if (AllowSavePrev && diff>1 && !bFirstValue && bWriteMidTime && TSFlag!=TS_STR)
//    {//value has remained "unchanged" for some time, previous value (except for strings) must be stored first
//    // also what does 'some time' mean ?  CNM
//    byte TempStatus = iStatus;
//    iStatus = Data.iPrevStatus;
//    if (!pHistory->pCurEvBlk->AddEv(pHistory->dMidTime, lSNo, Data.PrevSaveVal))
//      {
//      if (pHistory->bSwitchedOff)
//        return;
//      if (!pHistory->pCurEvBlk->AddEv(pHistory->dMidTime, lSNo, Data.PrevSaveVal)) //try again
//        return;
//      }
//    iStatus = TempStatus;
//    }
//  //AAAA<<<<<<<<<<<<<<<<<<<<<<

  bWriteMidTime = 1;
  pHistory->AddEv(lSNo);
  Data.PrevSaveVal = MeasVal;
  lScanOfLastSave = pHistory->lScanNo;
  if (pBoxcar)
    {
    pBoxcar->AveVal = MeasVal.GetDouble();
    pBoxcar->AveCnt = 1L;
    }

  #if dbgHistorian && DBGMANAGER
  if (dbgHistTag())
    {
    if (dbgHistTag.Items.Length()>0 && dbgHistTag.Items.Find(Data.sTag)!=NULL)
      { //place breakpoint here to see what is happening with a specific tag specified in syscad.dmf
      dbgpln("Historian: tag %s [%8i] saved:%12.3f", Data.sTag, pHistory->lScanNo, MeasVal.GetDouble());
      }
    }
  #endif
  }

//---------------------------------------------------------------------------

void CSlot::FixTyp()
  {
  if (IsNumData(Data.VarTyp))
    {
    if (MeasVal.Type() != Data.VarTyp)
      MeasVal.SetTypeDouble(Data.VarTyp, (IsNumData(MeasVal.Type()) ? MeasVal.GetDouble() : 0.0));
    if (ActualVal.Type() != Data.VarTyp)
      ActualVal.SetTypeDouble(Data.VarTyp, (IsNumData(ActualVal.Type()) ? ActualVal.GetDouble() : 0.0));
    if (Data.PrevSaveVal.Type() != Data.VarTyp)
      Data.PrevSaveVal.SetTypeDouble(Data.VarTyp, (IsNumData(Data.PrevSaveVal.Type()) ? Data.PrevSaveVal.GetDouble() : 0.0));
    if (Data.PrevMeasVal.Type() != Data.VarTyp)
      Data.PrevMeasVal.SetTypeDouble(Data.VarTyp, (IsNumData(Data.PrevMeasVal.Type()) ? Data.PrevMeasVal.GetDouble() : 0.0));
    if (Data.PrevActualVal.Type() != Data.VarTyp)
      Data.PrevActualVal.SetTypeDouble(Data.VarTyp, (IsNumData(Data.PrevActualVal.Type()) ? Data.PrevActualVal.GetDouble() : 0.0));
    }
  }

//---------------------------------------------------------------------------

void CSlot::dbgDump(char* p)
  {
  dbgpln("Historian slot:[%s]", p);
  dbgpln("Slot number   :%d", lSNo);
  dbgpln("TS flag       :%d", TSFlag);
  Data.dbgDump();
  }

//===========================================================================
