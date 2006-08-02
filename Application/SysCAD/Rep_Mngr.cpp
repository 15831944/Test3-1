//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __REP_MNGR_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "rep_mngr.h"
#include "dbbase.h"
#include "tagdb.h"
#include "scd_wm.h"
//#include "optoff.h"

//===========================================================================
const int DBMaxTagLen = 201;
const int DBMaxTimeDescLen = 41;

class CDataHist : public CFileRec
  {
  public:
    CDataHist(KWDatabase* pDB_, char* TableName, int TagCnt, CSVector* pTgs);
    virtual ~CDataHist();
    virtual void Init();
    virtual int Read();
    virtual int Write(BOOL AddNew = TRUE);

    int TagCnt() { return iTagCnt; };
    int WriteCnt() { return iWriteCnt; };

  protected:
    static BOOL bDoneInit;
    int iTagCnt;
    CSVector Tags;
    int iWriteCnt;
  public:
    double m_dTime;
    char TimeDesc[DBMaxTimeDescLen];
    double* Val;
  };

//-------------------------------------------------------------------------

BOOL CDataHist::bDoneInit = 0;

//-------------------------------------------------------------------------

CDataHist::CDataHist(KWDatabase* pDB_, char* TableName, int TagCnt, CSVector* pTgs) : CFileRec(false, true, pDB_)
  {
  sTableName = TableName;
  iTagCnt = TagCnt;
  Tags = *pTgs;
  iWriteCnt = 0;
  Val = new double[iTagCnt];
  //if (!bDoneInit)
    Init();
  }

//-------------------------------------------------------------------------

CDataHist::~CDataHist()
  {
  delete []Val;
  }

//-------------------------------------------------------------------------

void CDataHist::Init()
  {
  if (m_pDB)
    {
    DefineClass();
    CreateVal("Time",        &m_dTime,   ADOX::adDouble, FF_Required);
    CreateStr("TimeDesc",    TimeDesc,   DBMaxTimeDescLen-1);//, FF_AllowZeroLen);
    for (int i=0; i<iTagCnt; i++)
      {
      CTagDBase::EscapeFlds(Tags[i]);
      CreateVal(Tags[i](), &(Val[i]), ADOX::adDouble, FF_None);
      }
    CreateIndex("Time");
    bDoneInit = 1;
    }
  }

//-------------------------------------------------------------------------

int CDataHist::Read()
  {
  return BaseRead();
  }

//-------------------------------------------------------------------------

int CDataHist::Write(BOOL AddNew/*=TRUE*/)
  {
  int RetCode = BaseWrite(AddNew);
  if (RetCode==0)
    iWriteCnt++;
  return RetCode;
  }

// ==========================================================================

class CRepTrend
  {
  public:
    long iTagCnt;        //number of tags to be reported
    CSVector RqdTags;     //array of requested tags
    CSVector Tags;        //array of tags for query
    CArray <short,short> TagOffsets; //offset of returned data relative to request
    CTimeValue dStartTime;    //
    CTimeValue dEndTime;      //
    CTimeValue dDuration;     //
    long iNoOfPts;
    byte iOpt;
    Strng sDBName;
    Strng sTableName;
    Strng sTagList;
    byte bStartOK:1,
         bRequestOK:1,
         bQueryTagsDone:1,
         bDoneFirst:1,
         bDoneEnd:1;
    CRepExec* pExecObj;
    CXM_Route HRoute;
    KWDatabase DB;
    CDataHist* pf;

    double* DatVals;
    long iRecCnt;
    long iMaxRecCnt;

    //CRepTrend();
    CRepTrend(CRepExec* Exec);
    ~CRepTrend();
    flag Start0(CXM_RepTrendDB* p);
    flag Start1();
    flag Start2();
    void WriteData();
    void End();
    flag ProcessMsg(CXM_QueryString* p);
    void ProcessMsg(CXM_QueryRow* p);
    void ProcessMsg(CXM_QueryRowEx* p);
  };

//===========================================================================

CRepTrend::CRepTrend(CRepExec* Exec)
  {
  pExecObj = Exec;
  pf = NULL;
  dEndTime = gs_Exec.TimeStopped;
  dDuration = 3600.0;
  iNoOfPts = 61;
  iOpt = 1;//AveEqualySpaced
  iTagCnt = 0;
  dStartTime = dEndTime - dDuration;
  bStartOK = 0;
  bRequestOK = 0;
  bQueryTagsDone = 0;
  bDoneFirst = 0;
  bDoneEnd = 0;
  DatVals = NULL;
  iRecCnt = 0;
  }

//---------------------------------------------------------------------------

CRepTrend::~CRepTrend()
  {
  if (pf)
    delete pf;
  if (DatVals)
    delete []DatVals;
  }

//---------------------------------------------------------------------------

flag CRepTrend::Start0(CXM_RepTrendDB* p)
  {
  dEndTime = p->dEndTime;
  dDuration = p->dDuration;
  iNoOfPts = p->iNoOfPts;
  sDBName = p->FileName();
  sTableName = p->TableName();
  iTagCnt = 0;
  RqdTags.SetSize(p->nTags);
  int Cnt=0;
  for (char* pTg = p->FirstTag(); pTg; pTg = p->NextTag(pTg))
    {
    RqdTags[Cnt++] = pTg;
    }
  ASSERT(Cnt==p->nTags);
  bStartOK = 0;
  return true;
  }

//---------------------------------------------------------------------------

flag CRepTrend::Start1()
  {
  flag GotHistorian = pExecObj->XFindObject(pExecName_Historian, HRoute);
  if (!GotHistorian)
    {
    LogError("Report", LF_Exclamation, "No historian configured, cannot generate report");
    return false;
    }
  iTagCnt = 0;
  Strng WrkTag,WrkCnvTxt;
  for (int i=0; i<RqdTags.GetLen(); i++)
    {
    flag DupErr = false;
    TaggedObject::SplitTagCnv(RqdTags[i](), WrkTag, WrkCnvTxt);
    const int tl = WrkTag.Len();
    if (tl>0)
      {
      for (int j=0; j<iTagCnt; j++)
        if (Tags[j].Len()>=tl && _strnicmp(Tags[j](), WrkTag(), tl)==0 && (Tags[j].Len()==tl || Tags[j]()[tl]==' '))
          DupErr = true;
      if (DupErr)
        {
        LogError("Report", 0, "Tag '%s' requested twice", WrkTag());
        }
      else
        {
        CXM_HistoryExists *xb=new CXM_HistoryExists (0, WrkTag());
        CXMsgLst XM;
        XM.PackMsg(xb);
        DWORD RetCode = pExecObj->XSendMessage(XM, HRoute);
        if ((RetCode & RC_SM_HistRecOn)!=0)
          {
          if (iTagCnt>=Tags.GetLen())
            Tags.SetSize(iTagCnt + 16);
          Tags[iTagCnt++] = RqdTags[i]();
          }
        else
          LogError("Report", 0, "Tag '%s' not recorded in historian", WrkTag());
        }
      }
    }
  if (iTagCnt==0)
    {
    LogError("Report", LF_Exclamation, "No valid tags requested");
    return false;
    }

  WIN32_FIND_DATA fd;
  if (FileExists(sDBName(), fd))
    {
    if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      {
      LogError("Report", LF_Exclamation, "Cannot open read-only Database '%s'", sDBName());
      return false;
      }
    if (!DB.OpenDB(DBConnect_Default, sDBName(), false, false))
      {
      LogError("Report", LF_Exclamation, "Cannot open Database '%s'", sDBName());
      return false;
      }
    }
  else if (!DB.CreateDB(DBConnect_Default, sDBName(), /*dbLangGeneral,*/ 0/*dbVersion30*//*(DBOpts & DBO_MDB97)!=0 ? dbVersion30 : 0*/))
    {
    LogError("Report", LF_Exclamation, "Cannot create Database '%s'", sDBName());
    return false;
    }
  pf = new CDataHist(&DB, sTableName(), iTagCnt, &Tags);
  pf->SetIgnoreWriteErr3022(true);
  try
    {
    pf->Open(NULL/*, dbDenyWrite*/);
    }
  catch(_com_error & e)
    {
    //KWDisplayDaoException(e, _T("Unable to create index\n"));
    DB.CloseDB();
    LogError("Report", LF_Exclamation, "Cannot open table '%s':%s", sTableName(), (LPCTSTR)e.Description());
    return false;
    }
  bStartOK = 1;
  return true;
  }

//---------------------------------------------------------------------------

flag CRepTrend::Start2()
  {
  if (iTagCnt==0)
    return false;

  iMaxRecCnt = Max(iNoOfPts, 1441L);//60*24+1
  DatVals = new double[iMaxRecCnt*(iTagCnt+1)];
  iRecCnt = 0;

  //bQueryDone = 0;
  //bQueryTagsDone = 0;
  CXMsgLst XM;
  CXM_QueryHistoryOther *xb=new CXM_QueryHistoryOther (dStartTime.Seconds, dEndTime.Seconds, (long)this, iOpt, 0, 0, 0, iNoOfPts, dNAN, 3/*OLE*/, NULL, NULL, 0, 0.0);
  for (int i=0; i<iTagCnt; i++)
    {
    TagOffsets.Add(0L);
    xb->xAddTag(Tags[i]());
    }
  XM.PackMsg(xb);

  XM.PackMsg(new CXM_Route(HRoute));

  pExecObj->XSendMessage(XM, HRoute);

  strcpy(pf->TimeDesc, CTimeValue(dEndTime).Format(TD_TimeDate));
  LogNote("Report", 0, "Start: Retrieve historical data for %d tags over %d data points (%.1f seconds) for time ending %s",
          iTagCnt, iNoOfPts, dDuration, pf->TimeDesc);

  return true;
  }

//---------------------------------------------------------------------------

void CRepTrend::WriteData()
  {
  for (int i=0; i<iRecCnt; i++)
    {
    const int offset = i*(iTagCnt+1);
    pf->m_dTime = DatVals[offset + 0];
    strcpy(pf->TimeDesc, CTimeValue(pf->m_dTime).Format(TD_TimeDate));
    //SecstoHMSDate(pf->m_dTime, pf->TimeDesc);
    for (int j=0; j<iTagCnt; j++)
      pf->Val[j] = DatVals[offset + j + 1];
    pf->Write(true);
    }
  iRecCnt = 0;
  }

//---------------------------------------------------------------------------

void CRepTrend::End()
  {
  WriteData();
  bDoneEnd = 1;
  try
    {
    pf->Close();
    DB.CloseDB();
    LogNote("Report", 0, "Completed: %d records of historical data written. (%d tags for %d points requested)",
            pf->WriteCnt(), iTagCnt, iNoOfPts);
    }
  catch(_com_error & e)
    {
    DB.KWDisplayException(e, "CloseDB");
    LogError("Report", LF_Exclamation, "Cannot close database '%s'", sDBName());
    }
  }

//---------------------------------------------------------------------------

flag CRepTrend::ProcessMsg(CXM_QueryString* p)
  {
  if (strlen(p->cValue)==6 && _stricmp(p->cValue, "TheEnd")==0)
    {
    ScdMainWnd()->PostMessage(WMU_REPORT, SUB_REPACTION_END, (LPARAM)this);
    }
  else if (strlen(p->cValue)>6 && _strnicmp(p->cValue, "TheTags", 7)==0)
    {
    sTagList = p->cValue;
    bQueryTagsDone = 1;
    }
  return false;
  }

//---------------------------------------------------------------------------

void CRepTrend::ProcessMsg(CXM_QueryRow* p)
  {
  if (bStartOK)
    {
    if (!bDoneFirst)
      {
      ASSERT_ALWAYS(bQueryTagsDone, "Should get tags BEFORE data!");
      int Cnt = 0;
      Strng s1;
      Strng s = sTagList;
      int Pos = s.Find(":\"");
      while (Pos>=0)
        {
        s = s.Mid(Pos+2, 4096);
        int Pos2 = s.Find('\"');
        if (Pos2>=0)
          s1 = s.Left(Pos2);
        else
          s1 = s;
        const int tl = s1.Len();
        for (int i=0; i<iTagCnt; i++)
          {
          if (Tags[i].Len()>=tl && _strnicmp(Tags[i](), s1(), tl)==0 && (Tags[i].Len()==tl || Tags[i]()[tl]==' '))
            {
            TagOffsets[Cnt] = i;
            i = iTagCnt;
            }
          }
        Pos = s.Find(":\"");
        Cnt++;
        }
      bDoneFirst = 1;
      }

    ASSERT_ALWAYS(!bDoneEnd, "Still data...");

    ASSERT(p->nPts==iTagCnt);
    const int offset = iRecCnt*(iTagCnt+1);
    DatVals[offset + 0] = p->dTime;
    for (int j=0; j<iTagCnt; j++)
      DatVals[offset + j + 1] = p->dValue[j];
    iRecCnt++;
    if (iRecCnt==iMaxRecCnt)
      //ScdMainWnd()->PostMessage(WMU_REPORT, SUB_REPACTION_PROCESS, (LPARAM)this);
      AfxGetMainWnd()->SendMessage(WMU_REPORT, SUB_REPACTION_PROCESS, (LPARAM)this);
    }
  }

//---------------------------------------------------------------------------

void CRepTrend::ProcessMsg(CXM_QueryRowEx* p)
  {
  if (bStartOK)
    {
    if (!bDoneFirst)
      {
      ASSERT_ALWAYS(bQueryTagsDone, "Should get tags BEFORE data!");
      int Cnt = 0;
      Strng s1;
      Strng s = sTagList;
      int Pos = s.Find(":\"");
      while (Pos>=0)
        {
        s = s.Mid(Pos+2, 4096);
        int Pos2 = s.Find('\"');
        if (Pos2>=0)
          s1 = s.Left(Pos2);
        else
          s1 = s;
        const int tl = s1.Len();
        for (int i=0; i<iTagCnt; i++)
          {
          if (Tags[i].Len()>=tl && _strnicmp(Tags[i](), s1(), tl)==0 && (Tags[i].Len()==tl || Tags[i]()[tl]==' '))
            {
            TagOffsets[Cnt] = i;
            i = iTagCnt;
            }
          }
        Pos = s.Find(":\"");
        Cnt++;
        }
      bDoneFirst = 1;
      }

    ASSERT_ALWAYS(!bDoneEnd, "Still data...");

    ASSERT(p->nPts==iTagCnt);
    const int offset = iRecCnt*(iTagCnt+1);
    DatVals[offset + 0] = p->dTime;
    //for (int j=0; j<iTagCnt; j++)
    //  DatVals[offset + j + 1] = p->dValue[j];
    int Pos=0;
    byte ValTyp = p->FirstValTyp(Pos);
    for (int j=0; j<iTagCnt; j++)
      {
      if (ValTyp==QueryRowExType_Double)
        DatVals[offset + j + 1] = p->DValue(Pos);
      else if (ValTyp==QueryRowExType_Long)
        DatVals[offset + j + 1] = p->LValue(Pos);
      //else
      //  SDatVals[offset + j + 1] = p->SValue(Pos);
      ValTyp = p->NextValTyp(Pos);
      }
    iRecCnt++;
    if (iRecCnt==iMaxRecCnt)
      //ScdMainWnd()->PostMessage(WMU_REPORT, SUB_REPACTION_PROCESS, (LPARAM)this);
      AfxGetMainWnd()->SendMessage(WMU_REPORT, SUB_REPACTION_PROCESS, (LPARAM)this);
    }
  }

//===========================================================================
//===========================================================================

//CRepExec* pRepExec = NULL;

//===========================================================================

CRepExec::CRepExec()
  {
  //pRepExec = this;
  EO_Register(pExecName_Rep, /*EOWrite_Msg|EORead_Msg|*/EOExec_Msg, /*Pri*/THREAD_PRIORITY_NORMAL, /*Stack*/10000);
  }

//---------------------------------------------------------------------------

CRepExec::~CRepExec()
  {
  EO_DeRegister();
  //pRepExec = NULL;
  }

//---------------------------------------------------------------------------

void CRepExec::Start1(void* p)
  {
  CRepTrend* pRep = (CRepTrend*)p;
  pRep->Start1();
  //ScdMainWnd()->PostMessage(WMU_REPORT, SUB_REPACTION_START, (LPARAM)pRep);
  }

//---------------------------------------------------------------------------

void CRepExec::Start2(void* p)
  {
  CRepTrend* pRep = (CRepTrend*)p;
  pRep->Start2();
  }

//---------------------------------------------------------------------------

void CRepExec::WriteData(void* p)
  {
  CRepTrend* pRep = (CRepTrend*)p;
  pRep->WriteData();
  }

//---------------------------------------------------------------------------

void CRepExec::End(void* p)
  {
  CRepTrend* pRep = (CRepTrend*)p;
  pRep->End();
  delete pRep;
  }

//---------------------------------------------------------------------------

DWORD CRepExec::EO_Message(CXMsgLst &XM, CXM_Route &Route)
  {
  while (XM.MsgAvail())
    switch (XM.MsgId())
      {
      case XM_RepTrendDB:
        {
        CXM_RepTrendDB* p = XM.RepTrendDB();
        CRepTrend* pRep = new CRepTrend(this);
        pRep->Start0(p);
        ScdMainWnd()->PostMessage(WMU_REPORT, SUB_REPACTION_START, (LPARAM)pRep);
        break;
        }
      case XM_QueryRow:
        {
        CXM_QueryRow* p = XM.QueryRow();
        CRepTrend* pRep = (CRepTrend*)(p->iSrcID);
        pRep->ProcessMsg(p);
        break;
        }
      case XM_QueryRowEx:
        {
        CXM_QueryRowEx* p = XM.QueryRowEx();
        CRepTrend* pRep = (CRepTrend*)(p->iSrcID);
        pRep->ProcessMsg(p);
        break;
        }
      case XM_QueryString:
        {
        CXM_QueryString* p = XM.QueryString();
        if (p->cValue)
          {
          CRepTrend* pRep = (CRepTrend*)(p->iSrcID);
          flag AllDone = pRep->ProcessMsg(p);
          if (AllDone)
            delete pRep;
          }
        break;
        }
      default:
        ASSERT(0);
      }
  return 1;
  }

//---------------------------------------------------------------------------

flag CRepExec::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd)
  {
  return True;
  }

//---------------------------------------------------------------------------

flag CRepExec::EO_Start(CXM_TimeControl &CB)
  {
  return True;
  }

//---------------------------------------------------------------------------

void CRepExec::EO_QuerySubsReqd(CXMsgLst &XM)
  {
  XM.Clear();
  }

//---------------------------------------------------------------------------

void CRepExec::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)
  {
  //XM.Clear();
  }

//---------------------------------------------------------------------------

flag CRepExec::EO_ReadSubsData(CXMsgLst &XM)
  {
  flag DataRead = 0;
  return DataRead;
  }

//---------------------------------------------------------------------------

flag CRepExec::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)
  {
  while (XM.MsgAvail())
    {
    CXM_ObjectData* pX = XM.ObjectData();
    CPkDataItem* pPItem = pX->FirstItem();
    }
  return True;
  }

//---------------------------------------------------------------------------

flag CRepExec::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
  return False;
  }

//---------------------------------------------------------------------------

flag CRepExec::EO_Stop(CXM_TimeControl &CB)
  {
  return True;
  }

//---------------------------------------------------------------------------

int CRepExec::EO_CanClose(Strng_List & Problems)
  {
  /*if (pOleThread->IsBusy())
    {
    pOleThread->CancelQueueItems();
    //should these rather be added to the problems list???
    LogWarning("SysCAD", 0, "Extra Automation requests have been canceled.");
    LogError("SysCAD", 0, "Automation requests (eg Excel reports) are still being serviced.");
    return EO_CanClose_Wait;
    }*/
  return EO_CanClose_Yes;
  }

//---------------------------------------------------------------------------
//===========================================================================
