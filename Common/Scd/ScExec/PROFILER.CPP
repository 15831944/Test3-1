//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/*#M:Profiler, used by PGM.*/

#include "stdafx.h"
#define  __PROFILER_CPP
#include "sc_defs.h"
#include "profiler.h"
#include "noise.h"

//===========================================================================

CProfileInfo::CProfileInfo()
  {
  iLen=0;
  bLinear = 0;
  bWrapArround = 0;
  bStarted = 0;
  bPaused = 0;
  bJustLoaded = 0;
  iPulseSeq = 0;
  iCurIndex = 0;
  iPrvIndex = -1;
  dStartTime = CTimeValue(0.0);
  dLastTime = CTimeValue(0.0);
  dPausedVal = 0.0;
  dFullArea = 0.0;
  dPrevVal = 0.0;
  }

//===========================================================================

CSimpleProfiler::CSimpleProfiler()
  {
  Data.SetSize(0, 0);
  bLoaded = 0;
  pInfo = NULL;
  }

//---------------------------------------------------------------------------

CSimpleProfiler::~CSimpleProfiler()
  {
  if (pInfo)
    delete []pInfo;
  }

//---------------------------------------------------------------------------

int CSimpleProfiler::Load(char* Filename, flag LoadHeadings)
  {
  if (pInfo)
    delete []pInfo;
  pInfo = NULL;
  bLoaded = 0;
  if (Filename==NULL)
    return 1; //cannot open/load the file
  CVMLoadHelper H;
  strcpy(H.FName, Filename);
  H.StartRow = 2; //ignore first row of titles
  if (!Data.Load(H, NF_Free))
    return 1; //cannot open/load the file
  long Rows = Data.GetRows();
  if (Rows==0)
    return 2; //are no rows
  long Cols = Data.GetCols();
  if (Cols<2)
    return 3; //must be at least two columns

  if (!Valid(Data[0][0]))
    return 6; //time column has a NAN in it
  for (long i=1; i<Rows; i++)
    {
    if (!Valid(Data[i][0]))
      return 6; //time column has a NAN in it
    if (Data[i-1][0]>Data[i][0])
      return 4; //time column is not incrementing
    }
  
  pInfo = new CProfileInfo[Cols];
  pInfo[0].bWrapArround = 1;
  pInfo[0].bLinear = 1;
  pInfo[0].iLen = Rows;   
  if (LoadHeadings)
    {
    FILE* f=fopen(Filename, "rt");
    if (f)
      {
      char Buff[8192];
      if (fgets(Buff, sizeof(Buff)-1, f))
        {
        char* c[MaxCSVCols];
        int Quote;
        int nFlds = ParseCSVTokens(Buff, c, Quote);
        if (Quote)
          {
          //LogError("Quotes mismatched");
          }
        else
          {
          for (int i=0; i<Min(nFlds,(int)Cols); i++)
            {
            pInfo[i].sColTagName = (c[i] ? c[i] : "");
            }
          }
        }
      fclose(f);
      }
    }
  for (long j=1; j<Cols; j++)
    {
    pInfo[j].bWrapArround = 1;
    i = 0;
    while (i<Rows && (Valid(Data[i][j]) || HasNANFlag(Data[i][j])))
      i++;
    if (i==0)
      return 5; //empty column not allowed
    pInfo[j].iLen = i;
    }
  bLoaded = 1;
  return 0;
  }

//---------------------------------------------------------------------------

void CSimpleProfiler::Options(flag Linear, flag WrapArround, byte Column/*=1*/)
  {
  if (bLoaded && Column<Data.GetCols())
    {
    pInfo[Column].bLinear = Linear;
    pInfo[Column].bWrapArround = WrapArround;
    }
  }

//---------------------------------------------------------------------------

void CSimpleProfiler::SetAllOptions(flag Linear, flag WrapArround)
  {
  if (bLoaded)
    {
    for (byte Column=1; Column<Data.GetCols(); Column++)
      {
      pInfo[Column].bLinear = Linear;
      pInfo[Column].bWrapArround = WrapArround;
      }
    }
  }

//---------------------------------------------------------------------------

void CSimpleProfiler::JustLoaded(CTimeValue Time, byte Column)
  {
  pInfo[Column].dStartTime = Time - (pInfo[Column].dLastTime - pInfo[Column].dStartTime);
  pInfo[Column].dLastTime = Time;
  pInfo[Column].bJustLoaded = 0;
  if (pInfo[Column].iCurIndex>=pInfo[Column].iLen)
    pInfo[Column].iCurIndex = 0;
  }

//---------------------------------------------------------------------------

double CSimpleProfiler::CalcArea(CTimeValue T1, CTimeValue T2, byte Column)
  {
  if (bLoaded && Column<Data.GetCols())
    {
    if (Column==0) //TEMPORARY
      return 0.0; //TEMPORARY
    if (T1==T2)
      return 0.0;
    CProfileInfo& Info = pInfo[Column];
    ASSERT(T1>=0.0 && T2<=Data[Info.iLen-1][0] && T2>T1);
    double Area = 0.0;
    CTimeValue DeltaX = 0.0;
    int i = 0;
    while (T1>Data[i][0])
      i++;

    if (Info.bLinear || (Column==0))
      {
      //TODO GetAveVal for linear profile Not implemented yet!
      ASSERT_ALWAYS(FALSE, "KGA : GetAveVal for linear profile Not implemented yet!", __FILE__, __LINE__);
      }
    else
      {
      if (i>0 && T2>Data[i][0])
        {
        if (Valid(Data[i-1][Column]))
          {
          DeltaX = CTimeValue(Data[i][0]) - T1;
          Area += (DeltaX.Seconds * Data[i-1][Column]);
          }
        }
      flag First = 1;
      while (T2>Data[i][0])
        {
        if (First)
          First = 0;
        else 
          {
          if (Valid(Data[i-1][Column]))
            {
            DeltaX = CTimeValue(Data[i][0]) - Data[i-1][0];
            Area += (DeltaX.Seconds * Data[i-1][Column]);
            }
          }
        i++;
        }
      if (i==0)
        {
        if (Valid(Data[i][Column]))
          {
          DeltaX = T2 - T1;
          Area += (DeltaX * Data[i][Column]);
          }
        }
      else 
        {
        if (Valid(Data[i-1][Column]))
          {
          DeltaX = T2 - Max(T1, Data[i-1][0]);
          Area += (DeltaX * Data[i-1][Column]);
          }
        }
      }
    return Area;
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

double CSimpleProfiler::Start(CTimeValue Time, byte Column/*=1*/)
  {
  if (bLoaded && Column<Data.GetCols())
    {
    if (pInfo[Column].bJustLoaded)
      JustLoaded(Time, Column);
    pInfo[Column].dStartTime = Time;
    pInfo[Column].bStarted = 1;
    pInfo[Column].bPaused = 0;
    pInfo[Column].iCurIndex = 0;
    pInfo[Column].iPrvIndex = -1;
    if (pInfo[Column].bLinear)
      pInfo[Column].dFullArea = 0.0;
    else
      pInfo[Column].dFullArea = CalcArea(0.0, Data[pInfo[Column].iLen-1][0], Column);
    return GetVal(Time, Column);
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void CSimpleProfiler::StartAll(CTimeValue Time)
  {
  if (bLoaded)
    {
    for (byte i=0; i<Data.GetCols(); i++)
      Start(Time, i);
    }
  }

//---------------------------------------------------------------------------


double CSimpleProfiler::GetVal(CTimeValue Time, byte Column/*=1*/)
  {
  if (bLoaded && Column<Data.GetCols())
    {
    CProfileInfo& Info = pInfo[Column];

    if (0)
      dbgpln("CSimpleProfiler::GetVal %3i %s %s %10.2f %10.2f %10.2f %s", Column,
        Info.bStarted?"S":" ", Info.bPaused?"P":" ", Info.dLastTime, 
        Info.dPrevVal, Info.dStartTime, Info.sColTagName());

    if (Info.bJustLoaded)
      JustLoaded(Time, Column);
    if (!Info.bStarted)
      return Start(Time, Column);
    if (Info.bPaused)
      return Info.dPausedVal;
    Info.dLastTime = Time;
    CTimeValue RelTime = Time - Info.dStartTime;
    CTimeValue FullTime = CTimeValue(Data[Info.iLen-1][0]);
    if (RelTime>FullTime)
      {
      if (!Info.bWrapArround)
        return Data[Info.iLen-1][Column];
      if (FullTime==0.0)
        return Data[Info.iLen-1][Column]; //last entry is at time zero, return this value
      while (RelTime>FullTime)
        {
        Info.dStartTime += FullTime;
        RelTime = Time - Info.dStartTime;
        }
      Info.iCurIndex = 0;
      }
    //RelTime -= (FullTime * floor(RelTime/FullTime));
    while (RelTime>Data[Info.iCurIndex][0])
      Info.iCurIndex++;
    double RqdValue;
    if (Info.iCurIndex==0)
      RqdValue= Data[0][Column];
    else if (Info.bLinear || (Column==0))
      {
      CTimeValue DeltaX = CTimeValue(Data[Info.iCurIndex][0]) - CTimeValue(Data[Info.iCurIndex-1][0]);
      RqdValue=Data[Info.iCurIndex][Column];
      if (Valid(RqdValue))
        {
        if (fabs(DeltaX.Seconds)>1.0e-10)
          RqdValue=Data[Info.iCurIndex-1][Column] + ((RelTime - CTimeValue(Data[Info.iCurIndex-1][0])).Seconds * 
                    ((RqdValue - Data[Info.iCurIndex-1][Column]) / DeltaX.Seconds));
        else
          RqdValue=Data[Info.iCurIndex-1][Column];
        }
      else
        {
        int xxx=0;
        }
      }
    else
      RqdValue = Data[Info.iCurIndex-1][Column];
    //dbgpln("XXX [%3i,%3i] %f", Info.iCurIndex, Column);
    if (!Valid(RqdValue))
      { // test for buried flags
      if (HasNANFlag(RqdValue, NF_Pulse))
        {
        if (Info.iCurIndex!=Info.iPrvIndex)
          Info.iPulseSeq=1;
        RqdValue = SetNANFlag(NF_Free);
        }
      else if (HasNANFlag(RqdValue, NF_Hold))
        RqdValue = Info.dPrevVal;
      else if (HasNANFlag(RqdValue, NF_Free))
        { int xxx=0; }

      switch (Info.iPulseSeq)
        {
        case 0:                                                   break;
        case 1: RqdValue = 1;                   Info.iPulseSeq++; break;
        case 2: RqdValue = 0;                   Info.iPulseSeq++; break;
        case 3: RqdValue = SetNANFlag(NF_Free); Info.iPulseSeq=0; break;
        }
      }
    Info.iPrvIndex=Info.iCurIndex;
    Info.dPrevVal = RqdValue;
    return RqdValue;
    }
  
  return 0.0;
  }

//---------------------------------------------------------------------------

double CSimpleProfiler::GetAveVal(CTimeValue Time, byte Column/*=1*/)
  {
  if (bLoaded && Column<Data.GetCols())
    {
    CProfileInfo& Info = pInfo[Column];
    if (Info.bJustLoaded)
      JustLoaded(Time, Column);
    if (!Info.bStarted)
      return Start(Time, Column);
    if (Info.bPaused)
      return Info.dPausedVal;
    CTimeValue LastTime = Info.dLastTime;
    CTimeValue TheLastTime = Info.dLastTime;
    CTimeValue StepTime = Time - LastTime;
    double Area = 0.0;
    Info.dLastTime = Time;
    CTimeValue RelTime = Time - Info.dStartTime;
    CTimeValue FullTime = Data[Info.iLen-1][0];
    if (RelTime>FullTime)
      {
      if (LastTime<Info.dStartTime+FullTime)
        Area += CalcArea(LastTime - Info.dStartTime, Data[Info.iLen-1][0], Column);
      if (!Info.bWrapArround)
        {
        Area += (Data[Info.iLen-1][Column]*(RelTime-Max(FullTime, LastTime-Info.dStartTime)).Seconds);
        return Area/StepTime.Seconds;
        }
      flag First = 1;
      LastTime = Info.dStartTime + FullTime;
      while (RelTime>FullTime)
        {
        if (First)
          First = 0;
        else
          {
          Area += Info.dFullArea;
          LastTime += FullTime;
          }
        Info.dStartTime += FullTime;
        RelTime = Time - Info.dStartTime;
        }
      Info.iCurIndex = 0;
      }
    Area += CalcArea(LastTime-Info.dStartTime, Time-Info.dStartTime, Column);
    while (RelTime>Data[Info.iCurIndex][0])
      Info.iCurIndex++;
    return Area/StepTime.Seconds;
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

double CSimpleProfiler::Pause(CTimeValue Time, byte Column/*=1*/)
  { 
  CProfileInfo& Info = pInfo[Column];
  if (bLoaded && Column<Data.GetCols()) 
    {
    Info.dPausedVal = GetVal(Time, Column);
    Info.bPaused = 1; 
    return Info.dPausedVal;
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

double CSimpleProfiler::Continue(CTimeValue Time, byte Column/*=1*/) 
  { 
  CProfileInfo& Info = pInfo[Column];
  if (bLoaded && Column<Data.GetCols())
    {
    if (Info.bPaused && Info.bStarted)
      {
      Info.dStartTime = Time - (Info.dLastTime - Info.dStartTime);
      Info.dLastTime = Time;
      Info.bPaused = 0; 
      return Info.dPausedVal;
      }
    else
      return GetVal(Time, Column);
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

/*void CSimpleProfiler::Copy(pCSimpleProfiler p)
  {
  ASSERT(p && pInfo==NULL);
  Data = p->Data;
  bLoaded = p->bLoaded;
  if (p->pInfo)
    {
    const int Cols = Data.GetCols();
    pInfo = new CProfileInfo[Cols];
    for (int i=0; i<Cols; i++)
      pInfo[i] = p->pInfo[i];
    }
  }*/

//==========================================================================

const short Idf_ProfLoad = 1;
const short Idf_ProfOptions = 2;
const short Idf_ProfAllOptions = 3;
const short Idf_ProfStart = 4;
const short Idf_ProfStartAll = 5;
const short Idf_ProfGetVal = 6;
const short Idf_ProfGetAveVal = 7;
const short Idf_ProfPause = 8;
const short Idf_ProfCont = 9;

//---------------------------------------------------------------------------

GCProfiler::GCProfiler(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, &IB.m_VarMap, "PROFILER", VarClassDefn)
  {
  AddFunct(IB, "Load", 1, True, Idf_ProfLoad); //Load(filename)
  AddFunct(IB, "SetOptions", 3, False, Idf_ProfOptions); //SetOptions(Column, WrapArround, Linear)
  AddFunct(IB, "SetAllOptions", 2, False, Idf_ProfAllOptions); //SetAllOptions(WrapArround, Linear)
  AddFunct(IB, "Start", 1, False, Idf_ProfStart); //Start(Column)
  AddFunct(IB, "StartAll", 0, False, Idf_ProfStartAll); //StartAll()
  AddFunct(IB, "GetVal", 1, False, Idf_ProfGetVal); //GetVal(Column)
  AddFunct(IB, "GetAveVal", 1, False, Idf_ProfGetAveVal); //GetAveVal(Column)
  AddFunct(IB, "Pause", 1, False, Idf_ProfPause); //Pause(Column)
  AddFunct(IB, "Continue", 1, False, Idf_ProfCont); //Continue(Column)
  }

//---------------------------------------------------------------------------

void GCProfiler::Init(pGCClassVar pClassVar)
  {
  pClassVar->m_pSubClass = (pvoid)new CSimpleProfiler();
  }
  
//---------------------------------------------------------------------------

void GCProfiler::Done(pGCClassVar pClassVar)
  {
  delete (pCSimpleProfiler)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCProfiler::CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  pCSimpleProfiler pProf = (pCSimpleProfiler)pSubClass;
  switch (FunctId)
    {
    case Idf_ProfLoad:
      {
      Strng s,ss;
      //CNM s.AdaptFilename2(s, IB.GetSParm());
      sFilename.FnSearchContract(IB.GetSParm());
      s.FnExpand(sFilename());
      //CNM s = MakeFullPath(ss, s());
//      s = MakeFullFilename(ss, s());
      return pProf->Load(s(), false);
      break;
      }
    case Idf_ProfOptions:
      {
      flag Linear = IB.GetBParm();
      flag WrapArround =  IB.GetBParm();
      pProf->Options(Linear, WrapArround, IB.GetCParm());
      break;
      }
    case Idf_ProfAllOptions:
      {
      flag Linear = IB.GetBParm();
      pProf->SetAllOptions(Linear, IB.GetBParm());
      break;
      }
    case Idf_ProfStart:
      return pProf->Start(IB.m_dIC_Time, IB.GetCParm());
      break;
    case Idf_ProfStartAll:
      return pProf->Start(IB.m_dIC_Time);
      break;
    case Idf_ProfGetVal:
      return pProf->GetVal(IB.m_dIC_Time, IB.GetCParm());
      break;
    case Idf_ProfGetAveVal:
      return pProf->GetAveVal(IB.m_dIC_Time, IB.GetCParm());
      break;
    case Idf_ProfPause:
      return pProf->Pause(IB.m_dIC_Time, IB.GetCParm());
      break;
    case Idf_ProfCont:
      return pProf->Continue(IB.m_dIC_Time, IB.GetCParm());
      break;
    default:
      ASSERT(FALSE); //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCProfiler::OnSave(FilingControlBlock &FCB, pvoid pSubClass)
  {
//  if (FCB.SaveAs())
//    FCB.CopyFile(sFilename());
  pCSimpleProfiler pProf = (pCSimpleProfiler)pSubClass;
  GCFCBAppendRec(FCB, '{', "PROFILER", NULL, 0); // Start Internal Vars
  SaveVal(FCB, "Filename", sFilename());
  SaveVal(FCB, "ColumnCnt", pProf->ColCnt());
  for (byte i=0; i<pProf->ColCnt(); i++)
    {
    SaveVal(FCB, "Linear", pProf->Linear(i));
    SaveVal(FCB, "WrapArround", pProf->WrapArround(i));
    SaveVal(FCB, "Started", pProf->Started(i));
    SaveVal(FCB, "StartTime", pProf->StartTime(i));
    SaveVal(FCB, "LastTime", pProf->LastTime(i));
    SaveVal(FCB, "PausedVal", pProf->PausedVal(i));
    SaveVal(FCB, "Paused", pProf->Paused(i));
    SaveVal(FCB, "CurIndex", pProf->CurIndex(i));
    }
  GCFCBAppendRec(FCB, '}', "PROFILER", NULL, 0); // End Internal Vars
  }

//---------------------------------------------------------------------------
    
flag GCProfiler::OnLoad(FilingControlBlock &FCB, pvoid pSubClass)
  {
  pCSimpleProfiler pProf = (pCSimpleProfiler)pSubClass;
  byte ColCnt;
  byte CurCol = 0;
  flag Flag;
  for (;;)
    {
    GCFCBBuff Buff;
    GCFCBReadBuff(FCB, Buff);

    //dbgpln("..%5i: %c %3i %s",FCB.SetFilePointer(0, FILE_CURRENT), Buff.Hd.Id, Buff.Hd.nTotalLen, Buff.Name());

    switch (Buff.Hd.Id)
      {
      case '}':
        {
        if (pProf->bLoaded)
          {
          for (byte i=0; i<pProf->Data.GetCols(); i++)
            pProf->pInfo[i].bJustLoaded = 1;
          }
        return True;
        }
      case '{':
        break;
      default:
        if (Buff.Try("Filename", sFilename)) 
          {
          Strng s;
//          MakeFullFilename(s, sFilename());
//          MakeFullPath(s, sFilename());
          sFilename.FnSearchContract(sFilename());
          s.FnExpand(sFilename());
//          s.FnExpand(sFilename());
          pProf->Load(s(), false);
          break;
          }
        if (!pProf->bLoaded) break;
        if (Buff.Try("ColumnCnt", ColCnt)) break;
        if (CurCol>=pProf->Data.GetCols()) break;
        if (Buff.Try("Linear", Flag))
          {
          pProf->pInfo[CurCol].bLinear = Flag;
          break;
          }
        if (Buff.Try("WrapArround", Flag))
          {
          pProf->pInfo[CurCol].bWrapArround = Flag;
          break;
          }
        if (Buff.Try("Started", Flag))
          {
          pProf->pInfo[CurCol].bStarted = Flag;
          break;
          }
        if (Buff.Try("StartTime", pProf->pInfo[CurCol].dStartTime)) break;
        if (Buff.Try("LastTime", pProf->pInfo[CurCol].dLastTime)) break;
        if (Buff.Try("PausedVal", pProf->pInfo[CurCol].dPausedVal)) break;
        if (Buff.Try("Paused", Flag))
          {
          pProf->pInfo[CurCol].bPaused = Flag;
          break;
          }
        long l;
        if (Buff.Try("CurIndex", l))
          {
          pProf->pInfo[CurCol].iCurIndex = (WORD)l;
          CurCol++; //move to next column
          break;
          }
        break;
      }
    }
  return True;
  }
  
//---------------------------------------------------------------------------

/*void GCProfiler::OnRestore(pvoid pOldSubClass, pvoid pNewSubClass)
  {
  pCSimpleProfiler pOldProf = (pCSimpleProfiler)pOldSubClass;
  pCSimpleProfiler pNewProf = (pCSimpleProfiler)pNewSubClass;
  pNewProf->Copy(pOldProf);
  }*/

//===========================================================================

/*
class CRandomProfiler
  {
  public:
    CBaseNoise OnProf;
    CBaseNoise OffProf;
    double dOnOutput;
    double dOffOutput;
    double dStartTime;
    byte bGausian:1;
  
    CRandomProfiler(pGCClassVar pClassVar);
    ~CRandomProfiler();
    void Init(double OffMean, double OffStdDev, double OnMean, double OnStdDev);
    double Start(double Time);
    double GetVal(double Time);
  };

//---------------------------------------------------------------------------

CRandomProfiler::CRandomProfiler(pGCClassVar pClassVar) :
  //OnProf(*(new CBaseNoise(((pGCDoubleVar)pClassVar->GetVarByName("OnMean"))->m_var, ((pGCDoubleVar)pClassVar->GetVarByName("OnStdDev"))->m_var, dOnOutput))),
  OnProf(((pGCDoubleVar)pClassVar->GetVarByName("OnMean"))->m_var, ((pGCDoubleVar)pClassVar->GetVarByName("OnStdDev"))->m_var, dOnOutput),
  OffProf(((pGCDoubleVar)pClassVar->GetVarByName("OffMean"))->m_var, ((pGCDoubleVar)pClassVar->GetVarByName("OffStdDev"))->m_var, dOffOutput)))
  {
  //pOnProf = new CBaseNoise(
  bGausian = 1;
  }

//---------------------------------------------------------------------------

CRandomProfiler::~CRandomProfiler()
  {

  }

//---------------------------------------------------------------------------

void CRandomProfiler::Init(double OffMean, double OffStdDev, double OnMean, double OnStdDev)
  {
  }

//---------------------------------------------------------------------------

double CRandomProfiler::Start(double Time)
  {
  dStartTime = Time;

  return 0.0;
  }

//---------------------------------------------------------------------------

double CRandomProfiler::GetVal(double Time)
  {
  return 0.0;
  }

//===========================================================================

const short Idf_RndProfInit = 1;
const short Idf_RndProfOptions = 2;
const short Idf_RndProfStart = 3;
const short Idf_RndProfGetVal = 4;

//---------------------------------------------------------------------------

GCRandomProfiler::GCRandomProfiler(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, "RANDOMPROFILER", VarClassDefn)
  {
  AddVar(IB, "Output",     &GCDouble, VarConst);
  AddVar(IB, "OffMean",    &GCDouble);
  AddVar(IB, "OffStdDev",  &GCDouble);
  AddVar(IB, "OnMean",     &GCDouble);
  AddVar(IB, "OnStdDev",   &GCDouble);
  AddFunct(IB, "Init", 4, False, Idf_RndProfInit); //Init(OffMean, OffStdDev, OnMean, OnStdDev)
  AddFunct(IB, "SetOptions", 2, False, Idf_RndProfOptions); //SetOptions(Gausian, OnFirst, )
  AddFunct(IB, "Start", 0, False, Idf_RndProfStart); //Start()
  AddFunct(IB, "GetVal", 0, False, Idf_RndProfGetVal); //GetVal(Column)
  }

//---------------------------------------------------------------------------

void GCRandomProfiler::Init(pGCClassVar pClassVar)
  {
  pClassVar->m_pSubClass = (pvoid)new CRandomProfiler(pClassVar);
  }
  
//---------------------------------------------------------------------------

void GCRandomProfiler::Done(pGCClassVar pClassVar)
  {
  delete (CRandomProfiler*)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCRandomProfiler::CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  CRandomProfiler* pRndProf = (CRandomProfiler*)pSubClass;
  switch (FunctId)
    {
    case Idf_RndProfInit:
      {
      double d4 = IB.GetDParm();
      double d3 = IB.GetDParm();
      double d2 = IB.GetDParm();
      pRndProf->Init(IB.GetDParm(), d2, d3, d4);
      return 0.0;
      break;
      }
    case Idf_RndProfOptions:
      {
      double d4 = IB.GetDParm();
      double d3 = IB.GetDParm();
      double d2 = IB.GetDParm();
      return 0.0;
      break;
      }
    case Idf_RndProfStart:
      return pRndProf->Start(IB.m_dIC_Time);
      break;
    case Idf_RndProfGetVal:
      return pRndProf->GetVal(IB.m_dIC_Time);
      break;
    default:
      ASSERT(FALSE); //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCRandomProfiler::OnSave(FilingControlBlock &FCB, pvoid pSubClass)
  {
  /*if (FCB.SaveAs())
    FCB.CopyFile(sFilename());
  pCSimpleProfiler pProf = (pCSimpleProfiler)pSubClass;
  GCFCBAppendRec(FCB, '{', "PROFILER", NULL, 0); // Start Internal Vars
  SaveVal(FCB, "Filename", sFilename());
  SaveVal(FCB, "ColumnCnt", pProf->ColCnt());
  for (byte i=0; i<pProf->ColCnt(); i++)
    {
    SaveVal(FCB, "Linear", pProf->Linear(i));
    SaveVal(FCB, "WrapArround", pProf->WrapArround(i));
    SaveVal(FCB, "Started", pProf->Started(i));
    SaveVal(FCB, "StartTime", pProf->StartTime(i));
    SaveVal(FCB, "LastTime", pProf->LastTime(i));
    SaveVal(FCB, "PausedVal", pProf->PausedVal(i));
    SaveVal(FCB, "Paused", pProf->Paused(i));
    SaveVal(FCB, "CurIndex", pProf->CurIndex(i));
    }
  GCFCBAppendRec(FCB, '}', "PROFILER", NULL, 0); // End Internal Vars*//*
  }

//---------------------------------------------------------------------------
    
flag GCRandomProfiler::OnLoad(FilingControlBlock &FCB, pvoid pSubClass)
  {
  /*pCSimpleProfiler pProf = (pCSimpleProfiler)pSubClass;
  byte ColCnt;
  byte CurCol = 0;
  flag Flag;
  for (;;)
    {
    GCFCBBuff Buff;
    GCFCBReadBuff(FCB, Buff);

    //dbgpln("..%5i: %c %3i %s",FCB.SetFilePointer(0, FILE_CURRENT), Buff.Hd.Id, Buff.Hd.nTotalLen, Buff.Name());

    switch (Buff.Hd.Id)
      {
      case '}':
        {
        if (pProf->bLoaded)
          {
          for (byte i=0; i<pProf->Data.GetCols(); i++)
            pProf->pInfo[i].bJustLoaded = 1;
          }
        return True;
        }
      case '{':
        break;
      default:
        if (Buff.Try("Filename", sFilename)) 
          {
          Strng s;
          MakeFullFilename(s, sFilename());
          pProf->Load(s());
          break;
          }
        if (!pProf->bLoaded) break;
        if (Buff.Try("ColumnCnt", ColCnt)) break;
        if (CurCol>=pProf->Data.GetCols()) break;
        if (Buff.Try("Linear", Flag))
          {
          pProf->pInfo[CurCol].bLinear = Flag;
          break;
          }
        if (Buff.Try("WrapArround", Flag))
          {
          pProf->pInfo[CurCol].bWrapArround = Flag;
          break;
          }
        if (Buff.Try("Started", Flag))
          {
          pProf->pInfo[CurCol].bStarted = Flag;
          break;
          }
        if (Buff.Try("StartTime", pProf->pInfo[CurCol].dStartTime)) break;
        if (Buff.Try("LastTime", pProf->pInfo[CurCol].dLastTime)) break;
        if (Buff.Try("PausedVal", pProf->pInfo[CurCol].dPausedVal)) break;
        if (Buff.Try("Paused", Flag))
          {
          pProf->pInfo[CurCol].bPaused = Flag;
          break;
          }
        long l;
        if (Buff.Try("CurIndex", l))
          {
          pProf->pInfo[CurCol].iCurIndex = (WORD)l;
          CurCol++; //move to next column
          break;
          }
        break;
      }
    }*//*
  return True;
  }
  
//---------------------------------------------------------------------------

void GCRandomProfiler::OnRestore(pvoid pOldSubClass, pvoid pNewSubClass)
  {
  /*pCSimpleProfiler pOldProf = (pCSimpleProfiler)pOldSubClass;
  pCSimpleProfiler pNewProf = (pCSimpleProfiler)pNewSubClass;
  pNewProf->Copy(pOldProf);*//*
  }
*/
//===========================================================================
 //: Fn(&XYFnClass, "xx", NULL, TOA_Free)
/*CBaseProfiler::CBaseProfiler(pGCClassVar pClassVar)
  : Fn("DataOnly", "xx", NULL, TOA_Free),
    lFullTime(((pGCWordVar)pClassVar->GetVarByName("FullTime"))->m_var)
  {
  }*/

//---------------------------------------------------------------------------

/*CBaseProfiler::CBaseProfiler(pCProfiler pProf)
  : Fn("DataOnly", "xx", NULL, TOA_Free),
    dFullTime(pProf->FullTime)
  {
  }


//---------------------------------------------------------------------------

CBaseProfiler::~CBaseProfiler()
  {
  }
*/
//---------------------------------------------------------------------------

/*int CBaseProfiler::Configure(double FullTime, pchar Filename, UCHAR Column)
  {
  dFullTime = fabs(FullTime);
  CVMLoadHelper H;
  strcpy(H.FName, Filename);
  CDMatrix M;
  if (!M.Load(H))
    return 1; //cannot open/load the file
  long Rows = M.GetRows();
  if (Rows==0)
    return 2; //are no rows
  if (Column>=M.GetCols())
    return 3; //column specified is invalid
  int Warning = 0;
  if (dFullTime<0.0001)
    dFullTime = M[0][Rows-1];
  if (M[0][0]>dFullTime)
    return 4; //first time is greater than maximum allowed
  for (long i=0; i<Rows-1; i++)
    {
    if (M[i+1][0]>FullTime)
      {
      Warning = -1;
      Rows = i+1;
      M.SetSize(Rows, M.GetCols());
      break;
      }
    if (M[i][0]>M[i+1][0])
      return 5; //time column is not incrementing
    }
  Fn.SetLength(Rows());
  Fn.Y = M.GetCol(0);
  Fn.X.SetCol(0, M.GetCol(Column));
  return Warning;
  }

//---------------------------------------------------------------------------

double CBaseProfiler::Start(double Time, double StartAt)
  {
  return 0.0;
  }

//---------------------------------------------------------------------------

double CBaseProfiler::GetVal(double Time)
  {
  return 0.0;
  }*/

//---------------------------------------------------------------------------

//===========================================================================

/*CProfiler::CProfiler() : CBaseProfiler(this)
  {
  }*/

//---------------------------------------------------------------------------
//===========================================================================

