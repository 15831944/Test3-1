//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DDE_EXEC_H
#define  __DDE_EXEC_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif
#ifndef __DDE_SRVR_H
#include "dde_srvr.h"
#endif

// ==========================================================================

#if WITHDDEREPORTS
_FWDDEF(CReportConv)

enum ReportOption { Csv, Excel };

_FWDDEF(CReport)
class CReport
  {
  public:
    ReportOption Opt;   //report type
    Strng sFileName;    //filename for report
    Strng sReportName;  //name of required report (*=all)
    Strng sNan;         //string to use for NAN
    Strng sBlank;       //string to use for invalid tag
    Strng sIgnoreChars; //ignore cells starting with any of these characters
    CXM_Route xRoute;    //executive object to notify about errors
    Strng sName;        //report name
    CSVector ATags;     //array of primary tags
    CSVector BTags;     //array of secondary tags
    flag bVert:1,       //are the primary tags vertical
         bAll:1;        //fetch all reports regardless of name
    long lALen;         //no of primary tags
    long lBLen;         //no of secondary tags
    pCReportConv pConv; //DDE conversation to excel
    int iStartRow;      //starting row of report in spreadsheet
    int iStartCol;      //starting column of report in spreadsheet

    CReport(ReportOption eOpt, CXM_Route* pRoute, char* FileName, char* ReportName);
    flag Connect();
    flag DoExcelReports();
  private:
    flag ParseFn(char* p);
    void SetATag(int index, char* p);
    void SetBTag(int index, char* p);
    void ReportError(long ErrNo, char * fmt, ...);
    flag DoReport();
  };

const DWORD MaxDDEMsgLen = 64000;

#endif

// ==========================================================================

class CRTDBItem;

// ==========================================================================

_FWDDEF(CDdeAdviseBlk)
class CDdeAdviseBlk
  {
  public:
    CDdeAdviseBlk() { pItem = NULL; };
    ~CDdeAdviseBlk() { pItem = NULL; };
    Strng            sValue;
    Strng            sCnvTxt;
    CRTDBItem*       pItem;
  };

typedef CSmartPtr<CDdeAdviseBlk> CSPDdeAdviseBlk; 
typedef CArray <CSPDdeAdviseBlk,CSPDdeAdviseBlk&> Dde_SubsList; 

// ==========================================================================

#if WITHDDEREPORTS
_FWDDEF(CHistQueryList)
class CHistQueryList
  {
  public:
    HANDLE                  hSignal;  //handle for semaphore
    pchar                   pBuff;    //pointer to DDE message string to be returned
    DWORD                   dwPos;    //position in buffer
    pCHistQueryList         pNext;    //pointer to next request in linked list
    static pCHistQueryList  pFirst;   //pointer to start of list

    CHistQueryList(pchar Buff);//UINT Fmt, HSZ szItem);
    ~CHistQueryList();
    void Add(pchar p);
    static flag Find(pCHistQueryList pQry);
    void Wait()
      { WaitForSingleObject(hSignal, INFINITE); };
    void Release()
      { ReleaseSemaphore(hSignal, 1, NULL); };
};
#endif

// ==========================================================================

class CRTDBItem : public CDDEStringItem
  {
  DECLARE_DYNCREATE(CRTDBItem);
  CCnvIndex iCnvIndex;
  protected:
    virtual void OnPoke();
  };

// ==========================================================================

class CRTDBTopic : public CDDETopic
  {
  DECLARE_DYNCREATE(CRTDBTopic);
  public:
    CRTDBTopic();
    virtual ~CRTDBTopic();
    virtual BOOL CreateNewRequestItem(const char* pszItem);
    virtual BOOL CreateNewPokeItem(const char* pszItem, void* pData, DWORD dwSize);
    virtual BOOL RemoveItem(const char* pszItem);
    virtual BOOL ValidPokeFmt(UINT wFmt) { return (wFmt==CF_TEXT); };
  };

// ==========================================================================

#if WITHDDEREPORTS
class CHistoryTopic : public CDDETopic
  {
  DECLARE_DYNCREATE(CHistoryTopic);
  public:
    CHistoryTopic();
    virtual BOOL Request(UINT wFmt, const char* pszItem,
                         void** ppData, DWORD* pdwSize);
  };

// ==========================================================================

class CReportConv : public CDDEConv
  {
  DECLARE_DYNCREATE(CReportConv);
  public:
    CReportConv();
    CReportConv(CDDEServer* pServer);
    //virtual BOOL AdviseData(UINT wFmt, const char* pszTopic, const char* pszItem, void* pData, DWORD dwSize);
  };
#endif

// ==========================================================================

class CMyServer : public CDDEServer
  {
  public:
    CMyServer();
    virtual ~CMyServer();
    virtual void Status(const char* pszFormat, ...);
    virtual BOOL OnCreate();
    virtual HDDEDATA CustomCallback(WORD wType,
                                    WORD wFmt,
                                    HCONV hConv,
                                    HSZ hsz1,
                                    HSZ hsz2,
                                    HDDEDATA hData,
                                    DWORD dwData1,
                                    DWORD dwData2);

  public:
    CRTDBTopic m_RTDBTopic;
#if WITHDDEREPORTS
    CHistoryTopic m_HistoryTopic;
#endif
  };

// ==========================================================================
const int MaxDDEStats = 4;

_FWDDEF(CDdeExec)
class CDdeExec : public CExecObj
  {
  public:
    DWORD            dwThreadId;
    Dde_SubsList     SubsData;
    CMyServer        Dde;
    int              StatsCnt[MaxDDEStats];
    HANDLE           hQuitSignal;  //handle for semaphore

    CDdeExec();
    virtual ~CDdeExec();

    int InterpretRequest( pchar szItem, 
                          byte& Opt,
                          double& StartTime,
                          double& LastTime,
                          byte& TimeOpt,
                          flag& Headings,
                          long& NoPts,
                          CStringArray& TagList);
    int FindSubsData(CDDEItem* pItem);
    void BuildTagList(CListBox* pList);
    void BuildSubsList(CListBox* pList);
    void ForceAllWrites();
    void DoWrite();
    void QuitWait() { WaitForSingleObject(hQuitSignal, INFINITE); };
    void QuitRelease() { ReleaseSemaphore(hQuitSignal, 1, NULL); };

    // CExecObj Overrides
    virtual DWORD  EO_Message(CXMsgLst &XM, CXM_Route &Route);
    virtual flag   EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd);
    virtual flag   EO_Start(CXM_TimeControl &CB);
    virtual void   EO_QuerySubsReqd(CXMsgLst &XM);
    virtual void   EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    virtual flag   EO_ReadSubsData(CXMsgLst &XM);  
    virtual flag   EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);
    virtual flag   EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet);
    virtual flag   EO_Stop(CXM_TimeControl &CB);
  };

// ==========================================================================

_FWDDEF(CDdeThread)
class CDdeThread : public CWinThread
  {
  public:
    DECLARE_DYNCREATE(CDdeThread);
    virtual BOOL InitInstance();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual int ExitInstance();
#if WITHDDEREPORTS
    flag DoReports(pCReport pReport);
#endif
  };

//=============================================================================

//extern pCDdeExec pDdeExec;

#endif


