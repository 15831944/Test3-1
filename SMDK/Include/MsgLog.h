// MsgLog.h: interface for the CMsgLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGLOG_H__607FD7F9_7904_4A04_87DC_E107B94375A9__INCLUDED_)
#define AFX_MSGLOG_H__607FD7F9_7904_4A04_87DC_E107B94375A9__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#ifdef __MSGLOG_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SCDIF)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

// ========================================================================

const DWORD LogFlag_CondClear     = 0x00000001;
const DWORD LogFlag_Cond          = 0x00000002;
const DWORD LogFlag_Fatal         = 0x00000004;
const DWORD LogFlag_Stop          = 0x00000008;
const DWORD LogFlag_Error         = 0x00000010;
const DWORD LogFlag_Warning       = 0x00000020;
const DWORD LogFlag_Note          = 0x00000040;
const DWORD LogFlag_Cmd           = 0x00000080;
const DWORD LogFlag_Separator     = 0x00000100;

const DWORD LogFlag_AutoClear     = 0x00001000;
const DWORD LogFlag_ClearErrList  = 0x00002000;
const DWORD LogFlag_ClearCondList = 0x00004000;
const DWORD LogFlag_FromPGM       = 0x00008000;
const DWORD LogFlag_FromCOM       = 0x00010000;
const DWORD LogFlag_FromBlackBox  = 0x00020000;

const DWORD LogOption_COMCallsEntry    = 0x00000001;
const DWORD LogOption_COMCallsExit     = 0x00000002;
const DWORD LogOption_COMCalls         = 0x00000004;
const DWORD LogOption_COMCallsInternal = 0x00000008;

//const int MsgBuffLen = 4096;

const UINT  LF_Exclamation = MB_ICONEXCLAMATION;
const UINT  LF_Hand        = MB_ICONHAND;
const UINT  LF_DoAfxMsgBox = MB_NOFOCUS;
const UINT  LF_NoSkip      = MB_DEFAULT_DESKTOP_ONLY;

const DWORD LogItem_Time         = 0x00000001;
const DWORD LogItem_ElapsedHMS   = 0x00000002;
const DWORD LogItem_ElapsedDays  = 0x00000004;
const DWORD LogItem_ElapsedHrs   = 0x00000008;
const DWORD LogItem_ElapsedMins  = 0x00000010;
const DWORD LogItem_ElapsedSecs  = 0x00000020;
const DWORD LogItem_ElapsedDbl   = LogItem_ElapsedDays | LogItem_ElapsedHrs | LogItem_ElapsedMins | LogItem_ElapsedSecs;
const DWORD LogItem_Command      = 0x00000040;
const DWORD LogItem_Type         = 0x00000080;
const DWORD LogItem_Tag          = 0x00000100;
const DWORD LogItem_Source       = 0x00000200;
const DWORD LogItem_IDNo         = 0x00000400;
const DWORD LogItem_IterNo       = 0x00000800;
const DWORD LogItem_CallNo       = 0x00001000;
const DWORD LogItem_SeqNo        = 0x00002000;
const DWORD LogItem_Message	     = 0x00004000;
const DWORD LogItem_MessageLogOn = 0x00008000;
const DWORD LogItem_EventLogOn   = 0x00010000;
                                          
// ========================================================================

class DllImportExport  CMsgLogItem
  {
  public:
    DWORD   m_dwFlags;
    long    m_lIterNo;
    long    m_lIDNo;
    long    m_lCallNo;
    long    m_lSeqNo;
    HRESULT m_hr;
    UINT    m_MsgBoxFlags;
    //long    m_iDescStart;
    //char    m_Str[MsgBuffLen];
    CString m_Source;
    CString m_Desc;

    CMsgLogItem();
    ~CMsgLogItem();

    LPCTSTR Source()                { return m_Source; };
    LPCTSTR Description()           { return m_Desc; };
    void AddSource(LPCTSTR S)       { m_Source=S; };
    void AddDescription(LPCTSTR S)  { m_Desc = S; };

    //LPCTSTR Source() { return m_Str; };
    //LPCTSTR Description() { return &m_Str[m_iDescStart]; };
    //void AddSource(LPCTSTR S) { strcpy(m_Str, S); m_iDescStart=strlen(m_Str); };
    //void AddDescription(LPCTSTR S) { m_iDescStart = strlen(m_Str)+1; strcpy(&m_Str[m_iDescStart], S); };
  };

// ========================================================================

#ifdef  __MSGLOG_CPP
#include <vector>
class CMsgLogArray : public std::vector <CMsgLogItem*> {}; 
#endif

class DllImportExport  CMessageLogQ
  {
  public:
    UINT              m_nCount;
#ifdef  __MSGLOG_CPP
    CMsgLogArray    * m_pMsgs;
#else
    void            * m_pMsgs;
#endif
  }; 

//typedef void (*MessageLogHookFn)(DWORD Flags, LPCTSTR Where, LPCTSTR Msg, long SeqNo);

class DllImportExport CMessageLog //public CMsgLogArray 
  {
  public:
    CMessageLog();
    ~CMessageLog();

    void              Entry();
    void              Exit();

    CMsgLogItem &     GetWndQMsg(DWORD index);
    CMsgLogItem &     GetComQMsg(DWORD  index);
    //CMsgLogItem &     operator[](long index);
    long              NextIDNo() { return m_lIDNo; };
    long              NextComCallNo() { return m_lComCallNo; };
    DWORD             WndQMsgCount() { return m_WndQ.m_nCount; };
    DWORD             ComQMsgCount() { return m_ComQ.m_nCount; };

    static DWORD      MsgTypeWord(DWORD Flags); 
    static LPCTSTR    MsgTypeString(DWORD iTyp);

    void              SetMsgIteration(long Value); 
    void              IncMsgIteration(long Value=1); 
                            
    void              ClrMsgBoxSkip();
    void              SetMsgBoxSkip();
    bool              GetMsgBoxSkip();
    void              SetMsgBoxEmbedded();
                            
    void              ComCallStart();
    void              ComCallEnd();
    long              GetComCallLevel();
    int               GetComCallLogLnCnt();
    LPCTSTR           GetComCallLogLn();

    BOOL              LogFileOpen(LPCTSTR Fn);
    void              LogFileClose();
                              
    int               LogLn(HWND hWndOwn, DWORD Flags, LPCTSTR Where, UINT BoxFlags, LPCTSTR Msg, long SeqNo=0, HRESULT hr=S_OK);
    int               LogLn(DWORD Flags, LPCTSTR Where, UINT BoxFlags, LPCTSTR Msg, long SeqNo=0, HRESULT hr=S_OK);
    int               LogSeparator(LPCTSTR Where, UINT BoxFlags);

    void              Clear();
    void              ClearErrList();
    void              ClearCondList();
    BOOL              AutoClear(BOOL On);

    DWORD             GetOptions();
    void              SetOptions(DWORD Opt);
    BOOL              TestOption(DWORD Opt);
                              
    void              SetAppStarted();
    void              SetDefer(BOOL On);

    void              FlushWndQ();
    void              FlushComQ(DWORD  NewLength);
    BOOL              GetWndQMsg(DWORD  N, CMsgLogItem & Msg);
    BOOL              GetComQMsg(DWORD  N, CMsgLogItem & Msg);

    BOOL              RegisterMsgWnd(HWND hWnd, DWORD LogMsgID);
    BOOL              DeRegisterMsgWnd();

    void              IncrementComIFCnt();
    void              DecrementComIFCnt();

    void              Lock();
    void              Unlock();

    //void              SetHook(MessageLogHookFn Hook);

  public:
    long              m_lIDNo;
    long              m_lMsgIteration;

    HRESULT           m_hr;

    bool              m_bMessageBoxSkip;
    bool              m_bMessageBoxEmbedded;

    CRITICAL_SECTION  m_errSect;
    HWND              m_hMsgWnd;
    DWORD             m_dwLogMsgID;
    DWORD             m_dwCommonMsgFlags;
    //CMsgLogArray    * m_pRmtLogMsgs;
                  
    CMessageLogQ      m_WndQ, m_ComQ;

    long              m_lComCallNo;
    CString           m_sComCallLogLn;
    long              m_lComCallLevel;
    long              m_lComCallLnCnt;
                      
    long              m_lLogDeferred;
    bool              m_bLogAppStarted;  

    DWORD             m_dwLogOptions;

    FILE             *m_hLogFile;
    CString           m_sLogFile;

    long              m_lComIFCnt;

    //MessageLogHookFn  m_MessageLogHook;

  };

extern DllImportExport CMessageLog gs_MsgLog;


DllImportExport int LogLn(HWND hWndOwn, DWORD Flags, LPCTSTR Where, UINT BoxFlags, LPCTSTR Msg, long SeqNo=0, HRESULT hr=S_OK);
DllImportExport int LogLn(DWORD Flags, LPCTSTR Where, UINT BoxFlags, LPCTSTR Msg, long SeqNo=0, HRESULT hr=S_OK);

DllImportExport int LogSeparator(LPCTSTR Where, UINT BoxFlags);

DllImportExport int LogFatal(HWND hWndOwn, LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int LogStop(HWND hWndOwn, LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int LogError(HWND hWndOwn, LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int LogWarning(HWND hWndOwn, LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int LogNote(HWND hWndOwn, LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int PgmLogError(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int PgmLogWarning(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int PgmLogNote(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int ComLogError(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int ComLogWarning(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int ComLogNote(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);

DllImportExport int LogFatal(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int LogStop(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int LogError(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int LogWarning(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int LogNote(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int LogCmd(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
DllImportExport int LogCondClear(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);

DllImportExport LPCSTR GetHResultString(CString &Str, HRESULT hr);

// ========================================================================


#undef DllImportExport

#endif // !defined(AFX_MSGLOG_H__607FD7F9_7904_4A04_87DC_E107B94375A9__INCLUDED_)
