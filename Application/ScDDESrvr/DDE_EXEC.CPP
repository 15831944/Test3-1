//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#define  __DDE_EXEC_CPP
  
#include "stdafx.h"
#include "sc_defs.h"
#include "scd_wm.h"
#include "dde_exec.h"
#include "dde_srvr.h"
#include "dde_mngr.h"
#include "dde_exec.h"
#if WITHDDEREPORTS
#include "hstmsg.h"
#endif

#define dbgDde       0
   
#if dbgDde
#include "dbgmngr.h"
static CDbgMngr dbgDDE("General", "DDE");
#endif

#if WITHDDEREPORTS
const char* ReportKey = "SysCAD_Tags(";
#endif
//------------------------------------------------------------------------

//#define SZ_SERVICE_NAME "SYSCAD"        // Application
//#define SZ_CMD          "CMD"           // Topic
#define SZ_RTDB         "RTDB"          // Topic
#if WITHDDEREPORTS
#define SZ_HISTORY      "HISTORY"       // Topic
#endif
//#define SZ_ANY          "*"             // AnyItem

//------------------------------------------------------------------------

CDdeExec* pDdeExec = NULL; //global

//===========================================================================

#if WITHDDEREPORTS
CReport::CReport(ReportOption eOpt, CXM_Route* pRoute, char* FileName, char* ReportName)
  {
  Opt = eOpt;
  xRoute.ReverseRoute(*pRoute);
  sFileName = FileName;
  sReportName = ReportName;
  bAll = (sReportName.Length()==0 || sReportName=="*");
  pConv = NULL;
  }

//---------------------------------------------------------------------------

flag CReport::ParseFn(char* p)
  {
  int Quote;
  CSVColArray f;
  int nParms = ParseCSVFunction(p, f, Quote) - 1;
  if (Quote || nParms<4 || strlen(f[2])==0 || strlen(f[3])==0 || strlen(f[4])==0)
    return False;
  sName = f[1];
  bVert = (!(f[2][0]=='H' || f[2][0]=='h'));
  if (bVert)
    {
    lBLen = atol(f[3]);
    lALen = atol(f[4]);
    }
  else
    {
    lALen = atol(f[3]);
    lBLen = atol(f[4]);
    }
  if (lALen<0 || lBLen<0)
    return False;
  sNan = (nParms>4 ? f[5] : gs_pPrj->ReportInfo.sNan);
  sBlank = (nParms>5 ? f[6] : gs_pPrj->ReportInfo.sBlank);
  sIgnoreChars = (nParms>6 ? f[7] : gs_pPrj->ReportInfo.sIgnoreChars);
  ATags.SetSize(lALen);
  BTags.SetSize(lBLen);
  ATags.SetAll("");
  BTags.SetAll("");
  return True;
  }

//---------------------------------------------------------------------------

void CReport::SetATag(int index, char* p)
  {
  XStrRTrim(p, "\n\r\t ");
  ATags[index] = p;
  int len = ATags[index].Length();
  if (len>1 && ATags[index][0]=='"' && ATags[index][len-1]=='"')
    ATags[index] = ATags[index].Mid(1, len-2);
  }

//---------------------------------------------------------------------------

void CReport::SetBTag(int index, char* p)
  {
  XStrRTrim(p, "\n\r\t ");
  BTags[index] = p;
  int len = BTags[index].Length();
  if (len>1 && BTags[index][0]=='"' && BTags[index][len-1]=='"')
    BTags[index] = BTags[index].Mid(1, len-2);
  }

//---------------------------------------------------------------------------

void CReport::ReportError(long ErrNo, char * fmt, ...)
  {
  char Buff[1024];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  CXM_DDEErrorCode xb(ErrNo, Buff);
  CXMsgLst XMr;
  XMr.PackMsg(xb);
  pDdeExec->XSendMessage(XMr, xRoute);
  if (ErrNo)
    LogError("Report", 0, Buff);
  }

//---------------------------------------------------------------------------

flag CReport::Connect()
  {
  pConv = new CReportConv(&(pDdeExec->Dde));
  pConv->AddRef();
  if (pConv->ConnectTo("excel", sFileName()))
    return True;
  pConv->Release(); // and it should get deleted
  ReportError(10L, "Unable to connect to excel %s", sFileName());
  return False;
  }

//---------------------------------------------------------------------------

flag CReport::DoExcelReports()
  {
  ASSERT(gs_pPrj);
  CSVColArray c;
  char Buff[8192];
  const int ReportKeyLen = strlen(ReportKey);
  int i,j,Pos1;
  char* p;
  DWORD dwSize = 0;
  void* pData = NULL;
  char* pDat;
  i = 0;
  flag found = 0;
  flag DoMore = 1;
  flag FoundOne = 0;
  flag DoReportErr = 1;
  int jStart = 0;
  CDdeReportInfo& d = gs_pPrj->ReportInfo; 
  while (DoMore && i<d.iVSearchLen)
    {
    sprintf(Buff, "R%dC1:R%dC%d", i+1, i+1, d.iHSearchLen+1);
    pConv->Request(Buff, &pData, &dwSize); //request for a row
    if (dwSize) 
      {//got row...
      pDat = (char*)pData;
      int nToks = ParseTokens(pDat, c, '\t');
      XStrRTrim(c[nToks-1], "\r\t\n ");
      found = False;
      j = jStart;
      while (!found && j<nToks)
        {
        if (_strnicmp(c[j], ReportKey, ReportKeyLen)==0)
          {
          if (ParseFn(c[j]))
            found = (bAll || sReportName==sName);
          else
            {
            DoReportErr = 0;
            ReportError(13L, "Illegal syntax for %s....) R%dC%d", ReportKey, i+1, j+1);
            found = False;
            }
          }
        j++;
        }
      delete pData;
      
      jStart = 0;
      if (found)
        {//found, now get tags...
        iStartRow = i+1;
        iStartCol = j;
        //get the primary tags...
        if (bVert)
          sprintf(Buff, "R%dC%d:R%dC%d", iStartRow+1, iStartCol, iStartRow+lALen, iStartCol);
        else
          sprintf(Buff, "R%dC%d:R%dC%d", iStartRow, iStartCol+1, iStartRow, iStartCol+lALen);
        char c = (bVert ? '\r' : '\t');
        pConv->Request(Buff, &pData, &dwSize);
        if (dwSize) 
          {
          pDat = (char*)pData;
          Pos1 = 0;
          int index = 0;
          p = strchr(pDat, c);
          while (p)
            {
            p[0] = 0;
            SetATag(index, &pDat[Pos1]);
            Pos1 = p - pDat + 1;
            if (bVert)
              Pos1++;
            index++;
            p = strchr(&pDat[Pos1], c);
            }
          if (index<lALen)
            SetATag(index, &pDat[Pos1]);
          delete pData;
          }
        else
          {
          pConv->Release(); // and it should get deleted
          ReportError(14L, "Unable to get primary tags %s from excel", Buff);
          return False;
          }
        //get the secondary tags...
        if (bVert)
          sprintf(Buff, "R%dC%d:R%dC%d", iStartRow, iStartCol+1, iStartRow, iStartCol+lBLen);
        else
          sprintf(Buff, "R%dC%d:R%dC%d", iStartRow+1, iStartCol, iStartRow+lBLen, iStartCol);
        c = (bVert ? '\t' : '\r');
        pConv->Request(Buff, &pData, &dwSize);
        if (dwSize) 
          {
          pDat = (char*)pData;
          Pos1 = 0;
          int index = 0;
          p = strchr(pDat, c);
          while (p)
            {
            p[0] = 0;
            SetBTag(index, &pDat[Pos1]);
            Pos1 = p - pDat + 1;
            if (!bVert)
              Pos1++;
            index++;
            p = strchr(&pDat[Pos1], c);
            }
          if (index<lBLen)
            SetBTag(index, &pDat[Pos1]);
          delete pData;
          }
        else
          {
          pConv->Release(); // and it should get deleted
          ReportError(15L, "Unable to get secondary tags %s from excel", Buff);
          return False;
          }

        //have the primary and secondry tags, do the report...
        DoReport();
        FoundOne = 1;
        DoMore = bAll;
        i--;
        jStart = j;
        }
      }
    else
      {
      pConv->Release(); // and it should get deleted
      ReportError(11L, "Unable to get request %s from excel", Buff);
      return False;
      }
    i++;
    }
  if (!FoundOne)
    {
    pConv->Release(); // and it should get deleted
    if (DoReportErr)
      ReportError(12L, "Unable to find %s....)", ReportKey);
    return False;
    }
  return True;
  }

//---------------------------------------------------------------------------

flag CReport::DoReport()
  {
  ASSERT_ALWAYS(Opt==Excel, "Only Excel Report");
  char Buff[512];
  int i,j,iMax,jMax;
  Strng s, sTag, WrkTag, WrkCnvTxt, Separ;
  ASSERT(pConv);
  Separ = "\t";
  int aa,bb;
  iMax = (bVert ? lALen : lBLen);
  jMax = (bVert ? lBLen : lALen);
  DWORD dwSize = 0;
  void* pData = NULL;
  char* pDat;
  flag CheckCells = 0 /*kga temp; still TODO */|| (sIgnoreChars.Length()>0);
  
  if (CheckCells)
    {
    sprintf(Buff, "R%dC%d:R%dC%d", iStartRow+1, iStartCol+1, iStartRow+iMax, iStartCol+jMax);
    pConv->Request(Buff, &pData, &dwSize); //request for all cells
    if (dwSize) 
      {//got data...
      pDat = (char*)pData;
      //...TODO... retain special cells
      }
    else
      {
      pConv->Release(); // and it should get deleted
      ReportError(17L, "Unable to get cell contents %s", Buff);
      return False;
      }
    }

  for (i=0; i<iMax; i++)
    {
    s = "";
    for (j=0; j<jMax; j++)
      {
      if (j>0)
        s += Separ;
      aa = (bVert ? i : j);
      bb = (bVert ? j : i);
      if (ATags[aa].Length()==0 || BTags[bb].Length()==0)
        s += sBlank;
      else
        {
        sTag = ATags[aa];
        sTag += '.';
        sTag += BTags[bb];
        TaggedObject::SplitTagCnv(sTag(), WrkTag, WrkCnvTxt);
        CXM_ObjectTag ObjTag(WrkTag(), TABOpt_AllInfoOnce);
        CXM_ObjectData ObjData;
        CXM_Route Route;
        if (pDdeExec->XReadTaggedItem(ObjTag, ObjData, Route))
          {
          CPkDataItem * pItem = ObjData.FirstItem();
          if (pItem->Value()->IsNAN())
            s += sNan;
          else
            s += pItem->Value()->GetString("%i", "%g", pItem->CnvIndex(), WrkCnvTxt());
          }
        else
          s += sBlank;
        }
      }
    s += "\r\n";
    sprintf(Buff, "R%dC%d:R%dC%d", i+iStartRow+1, iStartCol+1, i+iStartRow+1, iStartCol+jMax);
    if (!pConv->Poke(CF_TEXT, Buff, (void*)s(), s.Length() + 1))
      {
      pConv->Release(); // and it should get deleted
      ReportError(16L, "Unable to send row %d", i+iStartRow+1);
      if (CheckCells)
        delete pData;
      return False;
      }
    }
  
  if (CheckCells)
    delete pData;

  ReportError(0L, "");
  return True;
  }

//==========================================================================

pCHistQueryList CHistQueryList::pFirst = NULL;

//---------------------------------------------------------------------------

CHistQueryList::CHistQueryList(pchar Buff)
  {
  hSignal = CreateSemaphore((LPSECURITY_ATTRIBUTES)NULL, 0, 0x7fffffff, NULL);
  pBuff = Buff;
  dwPos = 0;
  if (pFirst)
    pNext = pFirst;
  else
    pNext = NULL;
  pFirst = this;
  }

//---------------------------------------------------------------------------

CHistQueryList::~CHistQueryList()
  {
  CloseHandle(hSignal);
  if (pFirst==this)
    pFirst = pNext;
  else
    {  
    pCHistQueryList p = pFirst;
    while (p->pNext!=this)
      {
      p = p->pNext;
      ASSERT(p); //item should always exist in list !!!!
      }
    p->pNext = pNext;
    }
  }

//---------------------------------------------------------------------------

void CHistQueryList::Add(pchar p)
  {
  if (p)
    {
    int l = strlen(p);
    if (dwPos + l + 2 < MaxDDEMsgLen)
      {
      strcpy(&pBuff[dwPos], p);
      dwPos += l;
      pBuff[dwPos++] = '\r';
      pBuff[dwPos++] = '\n';
      }
    }
  }

//---------------------------------------------------------------------------

flag CHistQueryList::Find(pCHistQueryList pQry)
  {
  pCHistQueryList p = pFirst;
  while (p)
    {
    if (p==pQry)
      return True;
    p = p->pNext;
    }
  return False;
  }
#endif

//---------------------------------------------------------------------------
//==========================================================================

//---------------------------------------------------------------------------

static UINT wm_SyscadDdeCommand = 0;

const UINT DDE_QUIT          = 1;
const UINT DDE_POSTADVISE    = 2;
const UINT DDE_DELETEITEM    = 3;
#if WITHDDEREPORTS
const UINT DDE_REPORT        = 4;
#endif

//===========================================================================

IMPLEMENT_DYNCREATE(CDdeThread, CWinThread);

//---------------------------------------------------------------------------

BOOL CDdeThread::InitInstance()
  {
  if (!pDdeExec->Dde.Create(AfxGetAppName())) 
    {
    char buf[128];
    sprintf(buf, "DDE Service failed to start. Error: %d", pDdeExec->Dde.GetLastError());
    AfxMessageBox(buf);
    }
  //DWORD ThreadId = GetCurrentThreadId();
  return TRUE;//Must return TRUE to begin run loop
  }

//---------------------------------------------------------------------------

BOOL CDdeThread::PreTranslateMessage(MSG* pMsg)
  {
  if (pMsg->message==wm_SyscadDdeCommand)
    {
    WORD Cmd = LOWORD(pMsg->wParam);
    switch (Cmd)
      {
      case DDE_POSTADVISE:
        {
        CDdeAdviseBlk* p = (CDdeAdviseBlk*)(pMsg->lParam);
        //with a bit of luck the pointer to the DdeAdviseBlk is still valid
        WORD index = HIWORD(pMsg->wParam);
        pDdeExec->Dde.Lock();
        if (index<pDdeExec->SubsData.GetSize() && pDdeExec->SubsData[index]==p && p->pItem)//try check if it is valid
          {
          ((CRTDBItem*)(p->pItem))->SetData(p->sValue());
          }
        pDdeExec->Dde.Release();
        break;
        }
      case DDE_DELETEITEM:
        {
        pDdeExec->Dde.Lock();
        CDDEItem* pItem = (CDDEItem*)(pMsg->lParam);
        delete pItem;
        pDdeExec->Dde.Release();
        break;
        }
#if WITHDDEREPORTS
      case DDE_REPORT:
        {//unlikely to call this code, Excel DDE reports have been replaced by OLE automation reports
        pCReport pReport = (pCReport)(pMsg->lParam);
        DoReports(pReport);
        delete pReport;
        break;
        }
#endif
      case DDE_QUIT:
        //pDdeExec->Dde.Shutdown();
        //AfxEndThread(0);
        ::PostQuitMessage(0); //Do NOT use AfxEndThread for a user interface thread
        break;
      default:
        ASSERT(FALSE); //where did this message come from ???
      }
    return 1; //message processed here
    }
  else
    return CWinThread::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

int CDdeThread::ExitInstance()
  {
  pDdeExec->QuitRelease();
  return CWinThread::ExitInstance();
  }

//---------------------------------------------------------------------------

#if WITHDDEREPORTS
flag CDdeThread::DoReports(pCReport pReport)
  {
  CReport& R = *pReport;
  ASSERT(gs_pPrj);
  switch (R.Opt)
    {
    case Excel:
      {
      if (!R.Connect())
        return False;
      if (!R.DoExcelReports())
        return False;
      R.pConv->Release(); // and it should get deleted
      break;
      }
    case Csv:
      ASSERT(FALSE); //NOT implemented
      break;
    default:
      ASSERT(0);
    }
  return True;
  }
#endif

//===========================================================================

CMyServer::CMyServer()
  {
  }

//---------------------------------------------------------------------------

CMyServer::~CMyServer()
  {
  }

//---------------------------------------------------------------------------

void CMyServer::Status(const char* pszFormat, ...)
  {
  #if dbgDde
  if (dbgDDE())
    {
    char buf[2048];
    va_list arglist;
    va_start(arglist, pszFormat);
    vsprintf(buf, pszFormat, arglist);
    va_end(arglist);
    dbgpln("DDE:%s", buf);
    }
  #endif
  }

//---------------------------------------------------------------------------

BOOL CMyServer::OnCreate()
  {
  // Add our own topics and items
  m_RTDBTopic.Create(SZ_RTDB);
  AddTopic(&m_RTDBTopic);
#if WITHDDEREPORTS
  m_HistoryTopic.Create(SZ_HISTORY);
  AddTopic(&m_HistoryTopic);
#endif

  /*CRTDBItem* pItem = new CRTDBItem();
  pItem->Create("xxx");
  m_RTDBTopic.AddItem(pItem);
  pItem->SetData("3.0");*/

  //m_StringItem1.Create("String1");
  //m_DataTopic.AddItem(&m_StringItem1);

  /* DDE tests...
  CMyConv* m_pConversation;
  m_pConversation = new CMyConv(this);
  ASSERT(m_pConversation);
  m_pConversation->AddRef();
  if (!m_pConversation->ConnectTo("excel", "x2.xls"))
    {
    m_pConversation->Release(); // and it should get deleted
    m_pConversation = NULL;
    return TRUE;
    }

  DWORD dwSize = 0;
  void* pData = NULL;
  m_pConversation->Request("R2C2:R2C11", &pData, &dwSize);
  if (dwSize) 
    {
    TRACE("%s\n", (char*)pData);
    delete pData;
    //m_pConversation->Advise(m_strItem);
    }
  else
    {
    TRACE("Failed to get data");
    }
  
  char Buff[2048];
  char Itm[256];
  int i,j;

  CStopWatch W3;
  W3.Start();
  for (i=2; i<12; i++)
    {
    for (j=2; j<102; j++)
      {
      sprintf(Itm, "R%dC%d", j, i);
      m_pConversation->Request(Itm, &pData, &dwSize);
      if (dwSize) 
        {
        delete pData;
        //m_pConversation->Advise(m_strItem);
        }
      else
        {
        TRACE("Failed to get data");
        }
      }
    }
  W3.Stop();

  CStopWatch W1;
  W1.Start();
  for (i=2; i<12; i++)
    {
    for (j=2; j<102; j++)
      {
      sprintf(Itm, "R%dC%d", j, i);
      sprintf(Buff, "%d", j*1000+i);
      if (!m_pConversation->Poke(CF_TEXT, Itm,
                          (void*)Buff, strlen(Buff) + 1))
        {
        TRACE("Poke failed");
        }
      }
    }
  W1.Stop();

  CStopWatch W2;
  W2.Start();
  for (j=2; j<102; j++)
    {
    for (i=2; i<12; i++)
      {
      sprintf(Itm, "R%dC%d", j, i);
      sprintf(Buff, "%d", j*1000+i);
      if (!m_pConversation->Poke(CF_TEXT, Itm,
                          (void*)Buff, strlen(Buff) + 1))
        {
        TRACE("Poke failed");
        }
      }
    }
  W2.Stop();

  CStopWatch W4;
  W4.Start();
  for (j=2; j<102; j++)
    {
    for (i=2; i<12; i++)
      {
      if (i==2)
        sprintf(Buff, "%d", j*1000+i);
      else
        sprintf(Buff, "%s\t%d", Buff, j*1000+i);
      }
    sprintf(Buff, "%s\r\n", Buff);
    sprintf(Itm, "R%dC2:R%dC11", j, j);
    if (!m_pConversation->Poke(CF_TEXT, Itm,
                          (void*)Buff, strlen(Buff) + 1))
      {
      TRACE("Poke failed");
      }
    }
  W4.Stop();
  
  TRACE("Watch1:%g\n", W1.Secs());
  TRACE("Watch2:%g\n", W2.Secs());
  TRACE("Watch3:%g\n", W3.Secs());
  TRACE("Watch4:%g\n", W4.Secs());

  //if (!m_pConversation->Exec("GOTO($B$1)"))
  //  {
  //  TRACE("Exec failed!");
  //  }
  */

  return TRUE;
  }

//---------------------------------------------------------------------------

HDDEDATA CMyServer::CustomCallback(WORD wType,
                                  WORD wFmt,
                                  HCONV hConv,
                                  HSZ hsz1,
                                  HSZ hsz2,
                                  HDDEDATA hData,
                                  DWORD dwData1,
                                  DWORD dwData2)
  {
  switch (wType)
    {
    case XTYP_ADVSTOP:
      {
      CString strTopic = StringFromHsz(hsz1);
      CDDETopic* pTopic = FindTopic(strTopic);
      if (pTopic)
        {
        pDdeExec->Dde.Lock();
        CString strItem = StringFromHsz(hsz2);
        CDDEItem* pItem = pTopic->FindItem((const char*)strItem);
        if (pItem)
          {
          pItem->RemoveConv(hConv);
          if (pItem->ConvCount()==0)
            if (pTopic->RemoveItem((const char*)strItem))
              {
              int i = pDdeExec->FindSubsData(pItem);
              if (i>=0)
                {
                pDdeExec->SubsData[i]->pItem = NULL;
                pDdeExec->XBuildMyDataLists();
//dbgpln("SubsData[%i]:NULL", i);
                }
              }
          }
        pDdeExec->Dde.Release();
        }
      break;
      }
    }
  return NULL;
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CRTDBItem, CDDEStringItem);

void CRTDBItem::OnPoke()
  {
  Strng Tag, Cnv;
  TaggedObject::SplitTagCnv((char*)(const char*)m_strName, Tag, Cnv);
  
  CXM_ObjectTag  ObjTag(Tag(), TABOpt_AllInfoOnce);
  CXM_ObjectData ObjData;
  CXM_Route      Route;

  DWORD RetCode = pDdeExec->XReadTaggedItem(ObjTag, ObjData, Route);

  //LogNote(Tag(), 0, "DDE Poke: '%s' %s", pszItem, RetCode ? "Found" : "Not Found");
  if (RetCode==0)
    LogError(Tag(), 0, "DDE: Tag poke for '%s' not found", (const char*)m_strName);
  pDdeExec->StatsCnt[RetCode==0 ? 3 : 2]++;
  
  if (RetCode)
    {
    CPkDataItem * pPItem = ObjData.FirstItem();
    CCnvIndex iCnv = 0;
    if (pPItem->Contains(PDI_CnvInfo))
      iCnv = pPItem->CnvIndex();

    PkDataUnion DU;
    DU.SetTypeString(pPItem->Type(), (char*)(const char*)m_strData, iCnv, Cnv());
    CXM_ObjectData ObjData(0, 0, Tag(), 0, DU);
    flag Ok = (pDdeExec->XWriteTaggedItem(ObjData, Route)==TOData_OK);
    if (Ok)
      gs_Exec.m_Seq.NoteManSet(CExecSequence::VSS_DDE, Tag(), m_strData, Cnv());
    }
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CRTDBTopic, CDDETopic);

CRTDBTopic::CRTDBTopic()
  {
  }

//---------------------------------------------------------------------------

CRTDBTopic::~CRTDBTopic()
  {
  POSITION pos = m_ItemList.GetHeadPosition();
  while (pos) 
    {
    CDDEItem* pItem = m_ItemList.GetNext(pos);
    if (pItem->IsKindOf(RUNTIME_CLASS(CRTDBItem)))
      delete pItem;
    }
  }

//---------------------------------------------------------------------------

BOOL CRTDBTopic::CreateNewRequestItem(const char* pszItem)
  {
  Strng Tag, Cnv;
  TaggedObject::SplitTagCnv((char*)pszItem, Tag, Cnv);

  CXM_Route  ObjRoute; 
  CXM_ObjectTag ObjTag(Tag(), TABOpt_AllInfo);
  CXM_ObjectData ObjData(0);
 
  DWORD RetCode = pDdeExec->XReadTaggedItem(ObjTag, ObjData, ObjRoute);
  
  //LogNote(Tag(), 0, "DDE: '%s' %s", pszItem, RetCode ? "Found" : "Not Found");
  if (RetCode==0)
    LogError(Tag(), 0, "DDE: Tag request for '%s' not found", pszItem);
  pDdeExec->StatsCnt[RetCode==0 ? 3 : 1]++;

  if (RetCode) //This is a configured point so go and add the item
    {
    m_pServer->Lock();
    CPkDataItem * pPItem = ObjData.FirstItem();
    CRTDBItem* pItem = new CRTDBItem();
    pItem->iCnvIndex = pPItem->CnvIndex();
    pItem->Create(pszItem);
    AddItem(pItem);
    m_pServer->Release();
    pItem->SetDataOnly(pPItem->Value()->GetString("%i", "%g", pItem->iCnvIndex, Cnv()));
    pDdeExec->XBuildMyDataLists();
    return TRUE;
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CRTDBTopic::CreateNewPokeItem(const char* pszItem, void* pData, DWORD dwSize)
  {
  ASSERT(pData);
  //if (!isascii((int)(((char*)pData)[0])) || (int)(((char*)pData)[0])<0x20)
  //  return FALSE; //only want text data...
  Strng s((char*)pData);
  s.Trim(" \r\n");
  if (s.Length()>0)
    {
    Strng Tag, Cnv;
    TaggedObject::SplitTagCnv((char*)pszItem, Tag, Cnv);

    CXM_Route  ObjRoute; 
    CXM_ObjectTag ObjTag(Tag(), TABOpt_AllInfo);
    CXM_ObjectData ObjData(0);
 
    DWORD RetCode = pDdeExec->XReadTaggedItem(ObjTag, ObjData, ObjRoute);
    flag Ok = (RetCode!=0);
  
    //LogNote(Tag(), 0, "DDE Poke: '%s' %s", pszItem, RetCode ? "Found" : "Not Found");
    if (RetCode==0)
      LogError(Tag(), 0, "DDE: Tag poke for '%s' not found", pszItem);
    pDdeExec->StatsCnt[RetCode==0 ? 3 : 2]++;

    if (Ok) //This is a configured point so change it's value
      {
      CPkDataItem * pPItem = ObjData.FirstItem();
      CCnvIndex iCnv = pPItem->CnvIndex();
      PkDataUnion DU;
      DU.SetTypeString(pPItem->Type(), s(), iCnv, Cnv());
      CXM_ObjectData ObjData(0, 0, Tag(), 0, DU);
      Ok = (pDdeExec->XWriteTaggedItem(ObjData, ObjRoute)==TOData_OK);
      }
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CRTDBTopic::RemoveItem(const char* pszItem) 
  {
  POSITION pos = m_ItemList.GetHeadPosition();
  POSITION PrevPos = pos;
  CDDEItem* pItem = NULL;
  BOOL Found = FALSE;
  while (pos) 
    {
    PrevPos = pos;
    pItem = m_ItemList.GetNext(pos);
    if (pItem->m_strName.CompareNoCase(pszItem) == 0) 
      {
      Found = TRUE;
      break;
      }
    }
  if (!Found)
    return FALSE;

 if (pItem)
   {
   //dbgpln("DDE:RmvItem %s", (const char *)pItem->m_strName);
   
   m_pServer->Lock();
   m_ItemList.RemoveAt(PrevPos);
   delete pItem;
   m_pServer->Release();
   return TRUE;
   }
 return FALSE; 
 }

//===========================================================================

#if WITHDDEREPORTS
IMPLEMENT_DYNCREATE(CHistoryTopic, CDDETopic);

CHistoryTopic::CHistoryTopic()
  {
  }

//---------------------------------------------------------------------------

BOOL CHistoryTopic::Request(UINT wFmt, const char* pszItem,
                            void** ppData, DWORD* pdwSize)
  {
  if (wFmt != CF_TEXT)
    return FALSE;
  CXM_Route HRoute;
  flag GotHistorian = pDdeExec->XFindObject(pExecName_Historian, HRoute);
  char sz[MaxDDEMsgLen];
  char szItem[1024];
  szItem[0] = 0;
  szItem[sizeof(szItem)-1] = 0;
  sz[0] = 0;
  flag OK = 0;
  pCHistQueryList pQry = NULL;
  if (GotHistorian)
    {
    byte Opt;
    double StartTime;
    double LastTime;
    byte TimeOpt;
    flag Headings;
    long NoPts;
    CStringArray TagList;
    strncpy(szItem, pszItem, sizeof(szItem)-1);
    int ValidFormat = pDdeExec->InterpretRequest(szItem, Opt, StartTime, LastTime, TimeOpt, Headings, NoPts, TagList);
    int TagCnt = TagList.GetSize();
    if (TagCnt>0 && ValidFormat==0)
      OK = 1;
    if (OK)
      {
      CString Txt;
      pQry = new CHistQueryList(sz);
      //HRoute.dbgDump("Historian Route ");
      CXMsgLst XM;
      CXM_QueryHistoryOther xb(StartTime, LastTime, (long)pQry, Opt, TimeOpt, Headings, NoPts, dNAN, DDE);
      for (int i=0; i<TagCnt; i++)
        {
        Txt = TagList.GetAt(i);
        xb.AddTag((pchar)(const char*)Txt);
        }
      XM.PackMsg(xb);
      XM.PackMsg(HRoute);
      pDdeExec->XSendMessage(XM, HRoute);
      pQry->Wait();
      }
    else
      {
      if (ValidFormat!=0)
        wsprintf(sz, "Cannot retrieve query, invalid format\r\n\r\n");
      else if (TagCnt==0)
        wsprintf(sz, "Cannot retrieve query, no tags were specified\r\n\r\n");
      else
        wsprintf(sz, "InValid\r\n\r\n");
      }
    }
  else
    wsprintf(sz, "No historian configured, cannot retrieve query\r\n\r\n");

  //if (OK)
    {
    if (sz[0]==0)
      wsprintf(sz, "No Valid Tags\r\n\r\n");
    CDDEStringItem* pItem = new CDDEStringItem();
    pItem->Create(pszItem);
    pItem->m_pTopic = this; //do this instead of calling AddItem
    pItem->SetDataOnly(sz);
    BOOL b = pItem->Request(wFmt, ppData, pdwSize);
    delete pQry;
    PostThreadMessage(pDdeExec->dwThreadId, wm_SyscadDdeCommand, MAKEWPARAM(DDE_DELETEITEM, (WORD)0), (LPARAM)pItem);
    return b;
    /*UINT wCBformat = RegisterClipboardFormat((LPSTR)"XlTable");
    return DdeCreateDataHandle(dwDDEInstance(),
      (LPBYTE) sz,
      28,
      0,
      hszItem,
      wCBformat,//XlTable,//CF_TEXT,
      NULL);*/
    }
  return FALSE;
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CReportConv, CDDEConv);
    
CReportConv::CReportConv()
  {
  }

//---------------------------------------------------------------------------

CReportConv::CReportConv(CDDEServer* pServer)
: CDDEConv(pServer)
  {
  }
#endif

//===========================================================================

CDdeExec::CDdeExec()
  {
  pDdeExec = this; //global
  hQuitSignal = CreateSemaphore((LPSECURITY_ATTRIBUTES)NULL, 0, 0x7fffffff, NULL);
  wm_SyscadDdeCommand = RegisterWindowMessage("WM_SyscadDdeCommand");
  EO_Register(pExecName_DDE, EOWrite_Msg|EORead_Msg|EOExec_Msg, /*Pri*/THREAD_PRIORITY_NORMAL, /*Stack*/10000);
  CDdeThread* pDdeThrd = (CDdeThread*)AfxBeginThread(RUNTIME_CLASS(CDdeThread), THREAD_PRIORITY_NORMAL, 65536);
  dwThreadId = pDdeThrd->m_nThreadID;
  for (int i=0; i<MaxDDEStats; i++)
    StatsCnt[i] = 0;
  }

//---------------------------------------------------------------------------

CDdeExec::~CDdeExec()
  {
  EO_DeRegister();
  PostThreadMessage(dwThreadId, wm_SyscadDdeCommand, MAKEWPARAM(DDE_QUIT,0), 0);
  Sleep(500); //give PostThreadMessage a chance...
  QuitWait(); //now wait for release...
  CloseHandle(hQuitSignal);
  pDdeExec = NULL; //global
  }

//---------------------------------------------------------------------------

#if WITHDDEREPORTS
int CDdeExec::InterpretRequest( pchar szItem, 
                                byte& Opt,
                                double& StartTime,
                                double& LastTime,
                                byte& TimeOpt,
                                flag& Headings,
                                long& NoPts,
                                CStringArray& TagList)
  {                                  
  CSVColArray f;
  int Quote;
  int nFlds = ParseCSVFunction(szItem, f, Quote);
  if (Quote || nFlds<6)
    return 2; //Quotes mismatched or too few fields
  if (_stricmp(f[0], "AllPoints")==0)
    Opt = All;
  else if (_stricmp(f[0], "AvePoints")==0)
    Opt = AveEqualySpaced;
  else
    return 3; //function unknown
  if (!HMSDatetoSecs(f[1], StartTime))
    return 4;
  if (!HMSDatetoSecs(f[2], LastTime))
    return 5;
  if (_stricmp("Full", f[3])==0)
    TimeOpt = Full;
  else if (_stricmp("Secs", f[3])==0)
    TimeOpt = Seconds;
  else if (_stricmp("Both", f[3])==0)
    TimeOpt = Both;
  else if (_stricmp("None", f[3])==0)
    TimeOpt = None;
  else
    return 6;
  if (f[4] && (f[4][0]=='y' || f[4][0]=='Y'))
    Headings = True;
  else 
    Headings = False;
  CString Tag;
  int i = 5;
  if (Opt==AveEqualySpaced)
    {
    NoPts = Max(2L, atol(f[i++]));
    if (nFlds<7)
      return 2; //Quotes mismatched or 
    }
  while (i<nFlds)
    {
    Tag = f[i++];
    TagList.Add(Tag);
    }
  return 0;
  }
#endif

//---------------------------------------------------------------------------

void CDdeExec::BuildTagList(CListBox* pList)
  {
  CDDETopic* pTopic = Dde.FindTopic(SZ_RTDB);
  ASSERT(pTopic);
  int Cnt = 0;
  Dde.Lock();
  POSITION pos = pTopic->m_ItemList.GetHeadPosition();
  while (pos) 
    {
    CDDEItem* pItem = pTopic->m_ItemList.GetNext(pos);
    ASSERT(pItem);
    if (pItem->IsKindOf(RUNTIME_CLASS(CRTDBItem)))
      {
      pList->AddString((const char*)pItem->m_strName);
      Cnt++;
      }
    }
  Dde.Release();
  char Buff[64];
  sprintf(Buff, "Tag request count : %d", Cnt);
  pList->InsertString(0, Buff);
  }

//---------------------------------------------------------------------------

void CDdeExec::BuildSubsList(CListBox* pList)
  {
  int Index = 0;
  int Cnt = 0;
  char Buff[1024];
  Dde.Lock();
  while (Index<SubsData.GetSize())
    {
    CDdeAdviseBlk *p = SubsData[Index];
    if (p->pItem)
      sprintf(Buff, "%s  (%s)", (const char*)p->pItem->m_strName, p->sValue);
    else
      sprintf(Buff, "???  (%s)", p->sValue);
    pList->AddString(Buff);
    Cnt++;
    Index++;
    }
  Dde.Release();
  //sprintf(Buff, "Subscription count : %d", Cnt);
  //pList->InsertString(0, Buff);
  }

//---------------------------------------------------------------------------

void CDdeExec::ForceAllWrites()
  {
  int Index = 0;
  while (Index<SubsData.GetSize())
    {
    Dde.Lock();
    CDdeAdviseBlk *p = SubsData[Index];
    if (p->pItem)
      {
      StatsCnt[0]++;
      PostThreadMessage(dwThreadId, wm_SyscadDdeCommand, MAKEWPARAM(DDE_POSTADVISE, (WORD)Index), (LPARAM)p);
      }
    Dde.Release();
    Index++;
    }
  }

//---------------------------------------------------------------------------

int CDdeExec::FindSubsData(CDDEItem* pItem)
  {
  for (int i=0; i<SubsData.GetSize(); i++)
    {
    if (SubsData[i]->pItem==pItem)
      return i;
    }
  return -1;
  }

//---------------------------------------------------------------------------

DWORD CDdeExec::EO_Message(CXMsgLst &XM, CXM_Route &Route)
  {
  DWORD RetCode = 0;
#if WITHDDEREPORTS
  while (XM.MsgAvail())
    switch (XM.MsgId())
      {
      case XM_QueryString:
        {
        pXB_QueryString p = XM.QueryString();
        pCHistQueryList pQry = (pCHistQueryList)p->iSrcID;
        if (CHistQueryList::Find(pQry))
          {
          if (strcmp(p->String(), "TheEnd")==0)
            pQry->Release();
          else
            pQry->Add(p->String());
          }
        RetCode = 1;
        break;
        }    
      case XM_DDEReport:
        {
        pXB_DDEReport p = XM.DDEReport();
        CXM_Route * pXRoute = XM.Route();
        ASSERT(pXRoute); //did you remember to pack the route into the message ???
        pCReport pReport = new CReport((ReportOption)(p->iOpt), pXRoute, p->FileName(), p->ReportName());
        PostThreadMessage(dwThreadId, wm_SyscadDdeCommand, MAKEWPARAM(DDE_REPORT, (WORD)0), (LPARAM)pReport);
        RetCode = 1;
        break;
        }    
      default: 
        ASSERT(0);
      }
#endif
  return RetCode;
  }

//---------------------------------------------------------------------------

flag CDdeExec::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd) 
  {
  return True;
  }

//---------------------------------------------------------------------------

flag CDdeExec::EO_Start(CXM_TimeControl &CB)                      
  {
  CScdDDEManager::pTheMngr->UpdateStatusWnd((WPARAM)SUB_UPDATE_EOSTART);
  return True;
  }

//---------------------------------------------------------------------------

void CDdeExec::EO_QuerySubsReqd(CXMsgLst &XM)                      
  {
  CXM_ReadIndexedData * pMsg = (CXM_ReadIndexedData *)XM.MsgPtr(XM_ReadIndexedData);
  flag ReadAll = pMsg->ReadAll;
  long DataIndex;
  if (pMsg->Start)
    {
    DataIndex = 0;
    SubsData.RemoveAll();
    CScdDDEManager::pTheMngr->UpdateStatusWnd((WPARAM)SUB_UPDATE_SUBSCHANGED);
//dbgpln("SubsData[]:RemoveAll");
    }
  else
    DataIndex = pMsg->LastIndex+1;
  XM.Clear();
  Strng Tag, Cnv;
  
  int i=0;
  CDDETopic* pTopic = Dde.FindTopic(SZ_RTDB);
  ASSERT(pTopic);
  flag BuffFull = False; 
  Dde.Lock();
  POSITION pos = pTopic->m_ItemList.GetHeadPosition();
  while (pos && !BuffFull) 
    {
    CDDEItem* pItem = pTopic->m_ItemList.GetNext(pos);
    ASSERT(pItem);
    if (pItem->IsKindOf(RUNTIME_CLASS(CRTDBItem)))
      {
      if (i>=DataIndex)
        {//has not been done !!!
        TaggedObject::SplitTagCnv((char*)(const char*)pItem->m_strName, Tag, Cnv);
        CXM_DataRequest *pDRqst=new CXM_DataRequest(i, Tag(), TABOpt_AllInfoOnce, XIO_In);
        if (XM.PackMsg(pDRqst)) //if it can fit ...
          {
          CSPDdeAdviseBlk p;
          p = new CDdeAdviseBlk();
          p->sValue = ".";
          p->sCnvTxt = Cnv();
          //p->iCnv = 0;
          p->pItem = (CRTDBItem*)pItem;
          int AnIndex = SubsData.Add(p);// CArray of pDdeAdviseBlk
//dbgpln("SubsData[%i]:Add %08x %s", AnIndex, p, (const char*)pItem->m_strName);
          VERIFY(AnIndex==i);
          #if dbgDde
          if (dbgDDE())
            dbgpln("XDde:Subscribe for Item %8x %s", pItem, (const char*)pItem->m_strName);
          #endif
          }
        else  //if it cannot fit - EXIT
          {
          BuffFull = True; 
          delete pDRqst;
          }
        }
      i++;
      }
    }
//dbgpln("EO_QuerySubsReqd Done");

  Dde.Release();
  }

//---------------------------------------------------------------------------

void CDdeExec::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)                      
  {
  //XM.Clear();
  }

//---------------------------------------------------------------------------

flag CDdeExec::EO_ReadSubsData(CXMsgLst &XM)                      
  {
  flag DataRead = 0;
  return DataRead;
  }

//---------------------------------------------------------------------------

flag CDdeExec::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)
  {
//dbgp("SubsData:Wrt ");
  while (XM.MsgAvail())
    {
    CXM_ObjectData *pX = XM.ObjectData();
    CPkDataItem * pPItem = pX->FirstItem();
    int iIndex = pX->Index;
    Dde.Lock();
    if (iIndex<SubsData.GetSize())
      {
      CDdeAdviseBlk *p = SubsData[iIndex];
//dbgp(" %i", iIndex);
      //if (p->pItem && pPItem->Value() && IsData(pPItem->Value()->Type()))
      if (p->pItem && pPItem->Contains(PDI_Value) && IsData(pPItem->Value()->Type()))
        {
//dbgp(" %s", (LPCTSTR)p->pItem->m_strName);
        // Fetch On First Pass
        //if (pPItem->Contains(PDI_CnvInfo) && p->sCnvTxt.Length() > 0)
        CCnvIndex iCnv = p->pItem->iCnvIndex;
        if (p->sCnvTxt.Length()>0 && (iCnv)==0)
          p->sValue.Set("#Conversion not expected");
        else
          {
          if (p->sCnvTxt() && Cnvs[iCnv]->Find(p->sCnvTxt())==NULL)
            p->sValue.Set("#Unknown conversion '%s's", p->sCnvTxt());
          else
            p->sValue = pPItem->Value()->GetString("%i", "%g", iCnv, p->sCnvTxt());
          }
        StatsCnt[0]++;

        PostThreadMessage(dwThreadId, wm_SyscadDdeCommand, MAKEWPARAM(DDE_POSTADVISE, (WORD)iIndex), (LPARAM)p);
        }
      else
        {
        dbgp("Skipping DDE Data subscription change");
        if (p->pItem)
          dbgp(" %s", (LPCTSTR)p->pItem->m_strName);
        dbgpln("");
        }
      }
    Dde.Release();
    }
//dbgpln("");
  CScdDDEManager::pTheMngr->UpdateStatusWnd((WPARAM)SUB_UPDATE_EOWRITE);
  return True;
  }

//---------------------------------------------------------------------------

flag CDdeExec::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet) 
  {
  //CScdDDEManager::pTheMngr->UpdateStatusWnd((WPARAM)SUB_UPDATE_EOEXEC);
  return False;
  }

//---------------------------------------------------------------------------

flag CDdeExec::EO_Stop(CXM_TimeControl &CB)                       
  {
  CScdDDEManager::pTheMngr->UpdateStatusWnd((WPARAM)SUB_UPDATE_EOSTOP);
  return True;
  }

//===========================================================================
