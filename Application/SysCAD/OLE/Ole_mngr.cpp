//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
  
#define  __OLE_MNGR_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "gpfuncs.h"
#include "ole_base.h"
#include "ole_mngr.h"
#include "..\resource.h"
#include "oledlgs.h"
#include "oleexcel.h"
#include "project.h"
#include "scd_wm.h"

//===========================================================================

COleThread* pOleThread = NULL;

//===========================================================================

COleExec::COleExec()
  {
  wm_SyscadOleCommand = RegisterWindowMessage("WM_SyscadOleCommand");
  EO_Register(pExecName_OLE, /*EOWrite_Msg|EORead_Msg|*/EOExec_Msg, /*Pri*/THREAD_PRIORITY_NORMAL, /*Stack*/10000);
  COleThread* pOleThrd = (pCOleThread)AfxBeginThread(RUNTIME_CLASS(COleThread), THREAD_PRIORITY_NORMAL, 65536);
  dwThreadId = pOleThrd->m_nThreadID;
  }

//---------------------------------------------------------------------------

COleExec::~COleExec()
  {
  EO_DeRegister();
  PostThreadMessage(dwThreadId, wm_SyscadOleCommand, MAKEWPARAM(OLE_QUIT,0), 0);
  }

//---------------------------------------------------------------------------

DWORD COleExec::EO_Message(CXMsgLst &XM, CXM_Route &Route)
  {
  while (XM.MsgAvail())
    switch (XM.MsgId())
      {
      case XM_OleExcelReport:
        {
        CXM_OleExcelReport* p = XM.OleExcelReport();
        CXM_Route* pXRoute = XM.Route();
        ASSERT(pXRoute); //did you remember to pack the route into the message ???
        COleBaseAuto* pReportMngr = NULL;
        switch (p->iOpt)
          {
          case SUB_REPORT_GETVALUES   : pReportMngr = (COleBaseAuto*)new COleReportMngr(this, pXRoute, p->FileName(), p->ReportName(), true); break;
          case SUB_REPORT_TREND       : pReportMngr = (COleBaseAuto*)new COleReportTrendMngr(this, pXRoute, p->FileName(), p->ReportName());  break;
          case SUB_REPORT_SETVALUES   : pReportMngr = (COleBaseAuto*)new COleSetTagsMngr(this, pXRoute, p->FileName(), p->ReportName());      break;
          case SUB_REPORT_OPEN        : pReportMngr = (COleBaseAuto*)new COleStartExcel(this, pXRoute, p->FileName());                        break;
          case SUB_REPORT_SAVEANDCLOSE: pReportMngr = (COleBaseAuto*)new COleSaveCloseExcel(this, pXRoute, p->FileName());                    break;
          case SUB_REPORT_MACRO       : pReportMngr = (COleBaseAuto*)new COleExcelMacro(this, pXRoute, p->FileName(), p->ReportName());       break;
          case SUB_REPORT_GENERALINFO : pReportMngr = (COleBaseAuto*)new COleInfoReportMngr(this, pXRoute, p->FileName());                    break;
          default: ASSERT(FALSE); //what Excel OLE option is this ???
          }
        pReportMngr->m_pComCmd=p->ComCmdBlk();
        ((COleExcelBaseAuto*)pReportMngr)->SetOptions((byte)gs_pPrj->m_bRptExcelLock, 
                                                      (byte)gs_pPrj->m_bRptExcelCellName, 
                                                      (byte)gs_pPrj->m_bRptExcelMakeActive, 
                                                      (byte)gs_pPrj->m_bRptExcelSysCADActive, 
                                                      (byte)gs_pPrj->m_bRptExcelUpdateLinks, 
                                                      (byte)gs_pPrj->m_bRptExcelSaveOnComplete);
        pOleThread->AddQueueItem(pReportMngr);
        PostThreadMessage(dwThreadId, wm_SyscadOleCommand, MAKEWPARAM(OLE_DOAUTOITEM, (WORD)(p->FromExecutive?1:0)), (LPARAM)pReportMngr);
        break;
        }    
      case XM_QueryRow:
        {
        CXM_QueryRow* p = XM.QueryRow();
        COleReportTrendMngr* pM = (COleReportTrendMngr*)(p->iSrcID);
        pM->Lock();
        CRepTrndItem* pNewItm = new CRepTrndItem(p->dTime, p->nPts);
        if (pM->pFirst==NULL)
          pM->pFirst = pNewItm;
        else
          {
          CRepTrndItem* pItm = pM->pFirst;
          while (pItm->pNxt)
            pItm = pItm->pNxt;
          pItm->pNxt = pNewItm;
          }
        for (int i=0; i<p->nPts; i++)
          pNewItm->Values[i] = p->dValue[i];
        pM->Unlock();
        break;
        }
      case XM_QueryRowEx:
        {
        CXM_QueryRowEx* p = XM.QueryRowEx();
        COleReportTrendMngr* pM = (COleReportTrendMngr*)(p->iSrcID);
        pM->Lock();
        CRepTrndItem* pNewItm = new CRepTrndItem(p->dTime, p->nPts);
        if (pM->pFirst==NULL)
          pM->pFirst = pNewItm;
        else
          {
          CRepTrndItem* pItm = pM->pFirst;
          while (pItm->pNxt)
            pItm = pItm->pNxt;
          pItm->pNxt = pNewItm;
          }
        //for (int i=0; i<p->nPts; i++)
        //  pNewItm->Values[i] = p->dValue[i];
        int Pos=0;
        byte ValTyp = p->FirstValTyp(Pos);
        for (int j=0; j<p->nPts; j++)
          {
          if (ValTyp==QueryRowExType_Double)
            {
            pNewItm->Values[j] = p->DValue(Pos);
            }
          else if (ValTyp==QueryRowExType_Long)
            pNewItm->Values[j] = p->LValue(Pos);
          else
            pNewItm->Values[j] = p->SValue(Pos);
          ValTyp = p->NextValTyp(Pos);
          }
        pM->Unlock();
        break;
        }
      case XM_QueryString:
        {
        CXM_QueryString* p = XM.QueryString();
        if (p->cValue)
          {
          const int len = strlen(p->cValue);
          if (len==6 && _stricmp(p->cValue, "TheEnd")==0)
            {
            COleReportTrendMngr* pM = (COleReportTrendMngr*)(p->iSrcID);
            pM->Lock();
            pM->bQueryDone = 1;
            pM->Unlock();
            }
          else if (len>6 && _strnicmp(p->cValue, "TheTags", 7)==0)
            {
            COleReportTrendMngr* pM = (COleReportTrendMngr*)(p->iSrcID);
            pM->Lock();
            pM->sTagList = p->cValue;
            pM->bQueryTagsDone = 1;
            pM->Unlock();
            }
          }
        break;
        }
      default: 
        ASSERT(0);
      }
  return 1;
  }

//---------------------------------------------------------------------------

flag COleExec::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd) 
  {
  return True;
  }

//---------------------------------------------------------------------------

flag COleExec::EO_Start(CXM_TimeControl &CB)                      
  {
  return True;
  }

//---------------------------------------------------------------------------

void COleExec::EO_QuerySubsReqd(CXMsgLst &XM)                      
  {
  XM.Clear();
  }

//---------------------------------------------------------------------------

void COleExec::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)                      
  {
  //XM.Clear();
  }

//---------------------------------------------------------------------------

flag COleExec::EO_ReadSubsData(CXMsgLst &XM)                      
  {
  flag DataRead = 0;
  return DataRead;
  }

//---------------------------------------------------------------------------

flag COleExec::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)
  {
  while (XM.MsgAvail())
    {
    CXM_ObjectData* pX = XM.ObjectData();
    CPkDataItem* pPItem = pX->FirstItem();
    }
  return True;
  }

//---------------------------------------------------------------------------

flag COleExec::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet) 
  {
  return False;
  }

//---------------------------------------------------------------------------

flag COleExec::EO_Stop(CXM_TimeControl &CB)                       
  {
  return True;
  }

//---------------------------------------------------------------------------

int COleExec::EO_CanClose(Strng_List & Problems)
  {
  if (pOleThread->IsBusy())
    {
    pOleThread->CancelQueueItems();
    //should these rather be added to the problems list???
    LogWarning("SysCAD", 0, "Extra Automation requests have been canceled.");
    LogError("SysCAD", 0, "Automation requests (eg Excel reports) are still being serviced.");
    return EO_CanClose_Wait;
    }
  return EO_CanClose_Yes; 
  }

//===========================================================================

IMPLEMENT_DYNCREATE(COleThread, CWinThread);

//---------------------------------------------------------------------------

BOOL COleThread::InitInstance()
  {
  pOleThread = this;
  InitializeCriticalSection(&QueueSection);
  pQueue = NULL;
  bQuitNow = 0;
  iQueueCnt = 0;
  if (pOleInfoWnd==NULL)
    pOleInfoWnd = (CWnd*)(new COleInfoWnd(AfxGetMainWnd()));
  DWORD dwThreadId = GetCurrentThreadId();
  SCODE sc = ::OleInitialize(NULL);
  return TRUE;//Must return TRUE to begin run loop
  }

//---------------------------------------------------------------------------

int COleThread::ExitInstance()
  {
  Lock();
  bQuitNow = 1;
  Unlock();
  DeleteCriticalSection(&QueueSection);
  if (pOleInfoWnd)
    {
    pOleInfoWnd->DestroyWindow();
    delete pOleInfoWnd;
    pOleInfoWnd = NULL;
    }
  pOleThread = NULL;
  ::OleUninitialize();
  return 0;
  }

//---------------------------------------------------------------------------

void COleThread::AddQueueItem(COleBaseAuto* pNewItem)
  {
  Lock();
  //add to end of queue...
  COleAutoQueueItem* pItem = pQueue;
  while (pItem && pItem->pNxt)
    pItem = pItem->pNxt;
  if (pQueue)
    pItem->pNxt = new COleAutoQueueItem(pNewItem);
  else
    pQueue = new COleAutoQueueItem(pNewItem);
  iQueueCnt++;
  Unlock();
  COleInfoMsg* pMsg;
  if (iQueueCnt==1)
    pMsg = new COleInfoMsg(iQueueCnt, "", "");
  else
    pMsg = new COleInfoMsg(iQueueCnt);
  pOleInfoWnd->SendMessage(WMU_OLEINFOUPDATE, 0, (LPARAM)pMsg);
  }

//---------------------------------------------------------------------------

void COleThread::CancelQueueItems()
  {
  Lock();
  bQuitNow = 1;
  Unlock();
  }

//---------------------------------------------------------------------------

BOOL COleThread::PreTranslateMessage(MSG* pMsg)
  {
  if (pMsg->message==wm_SyscadOleCommand)
    {
    WORD Cmd = LOWORD(pMsg->wParam);
    WORD FromExecutive= HIWORD(pMsg->wParam);
    switch (Cmd)
      {
      case OLE_DOAUTOITEM:
        {
        if (bQuitNow)
          {
          Lock();
          while (pQueue)
            {
            iQueueCnt--;
            delete pQueue->pAutoItem;
            COleAutoQueueItem* DelItem = pQueue;
            pQueue = pQueue->pNxt;
            delete DelItem;
            }
          Unlock();
          }
        Lock();
        if (pQueue)
          {
          Unlock();
          pQueue->pAutoItem->DoAutomation();
          pQueue->pAutoItem->ActivateSysCAD();
          delete pQueue->pAutoItem;
          Lock();
          iQueueCnt--;
          COleAutoQueueItem* DelItem = pQueue;
          pQueue = pQueue->pNxt;
          delete DelItem;
          }
        Unlock();
        COleInfoMsg* pMsg = new COleInfoMsg(iQueueCnt);
        pOleInfoWnd->SendMessage(WMU_OLEINFOUPDATE, 0, (LPARAM)pMsg);
        if (FromExecutive)
          gs_Exec.OneReportComplete();
        if (iQueueCnt==0 && !bQuitNow)
          ScdMainWnd()->PostMessage(WMU_OLEINFOUPDATE, 0, 0); //send message after LAST report
        break;
        }
      case OLE_QUIT:
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
//===========================================================================
