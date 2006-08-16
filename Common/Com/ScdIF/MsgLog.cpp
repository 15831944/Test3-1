// MsgLog.cpp: implementation of the CMsgLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#define __MSGLOG_CPP
#include "MsgLog.h"
#include "DebugLib.h"
//#include "dbgmngr.h"

#ifndef DISTRIBUTION
static int dbgLogCallsEntry      = 0;
static int dbgLogCallsExit       = 0;
static int dbgLogCalls           = 0;
static int dbgLogCallsInternal   = 0;
//static CDbgMngr dbgLogCallsEntry          ("MsgLog",   "CallsEntry");
//static CDbgMngr dbgLogCallsExit           ("MsgLog",   "CallsExit");
//static CDbgMngr dbgLogCalls               ("MsgLog",   "Calls");
//static CDbgMngr dbgLogCallsInternal       ("MsgLog",   "CallsInternal");
#endif

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//static long nCMsgLogItemCnt=0;
CMsgLogItem::CMsgLogItem()
  {
  m_dwFlags     = 0;
  m_lIterNo     = 0;
  m_lIDNo       = 0;
  m_lCallNo     = 0;
  m_lSeqNo      = 0;
  m_hr          = 0;
  m_MsgBoxFlags = 0;
  //m_iDescStart      = 0;
  //m_Str[0]      = 0;
  //dbgpln("CMsgLogItem %i", ++nCMsgLogItemCnt);
  };
CMsgLogItem::~CMsgLogItem()
  {
  //dbgpln("~CMsgLogItem %i", nCMsgLogItemCnt--);
  };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageLog::CMessageLog()
  {
  //InitializeCriticalSection(&m_errSect);
  InitializeCriticalSectionAndSpinCount(&m_errSect, 4000);

  m_WndQ.m_pMsgs = new CMsgLogArray;
  m_ComQ.m_pMsgs = new CMsgLogArray;
  m_WndQ.m_nCount=0;
  m_ComQ.m_nCount=0;
  m_lIDNo=0;
  m_bMessageBoxSkip=false;
  m_bMessageBoxEmbedded=false;

  m_hMsgWnd=NULL;
  m_dwLogMsgID=0;
  m_dwCommonMsgFlags=0;
  //m_pRmtLogMsgs=NULL;

  m_lComCallNo=0;
  m_lComCallLevel=0;
  m_lComCallLnCnt=0;

  m_lLogDeferred=0;
  m_bLogAppStarted=false;
  
  m_dwLogOptions=0;
  
  m_hLogFile=NULL;

  m_lComIFCnt=0;

  //m_MessageLogHook=NULL;
  }

//===========================================================================

CMessageLog::~CMessageLog()
  {
  for (UINT i=0; i<m_WndQ.m_pMsgs->size(); i++)
    delete (*m_WndQ.m_pMsgs)[i];
  delete m_WndQ.m_pMsgs;
  for (i=0; i<m_ComQ.m_pMsgs->size(); i++)
    delete (*m_ComQ.m_pMsgs)[i];
  delete m_ComQ.m_pMsgs;
  DeleteCriticalSection(&m_errSect);
  }

//===========================================================================

CMessageLog gs_MsgLog;

//===========================================================================
//
//
//
//===========================================================================

CMsgLogItem & CMessageLog::GetWndQMsg(DWORD  index)
  {
  return *((*m_WndQ.m_pMsgs)[index]);
  };
CMsgLogItem & CMessageLog::GetComQMsg(DWORD  index)
  {
  return *((*m_ComQ.m_pMsgs)[index]);
  };

void CMessageLog::Lock() { EnterCriticalSection(&gs_MsgLog.m_errSect); }
void CMessageLog::Unlock() { LeaveCriticalSection(&gs_MsgLog.m_errSect); }

//===========================================================================

DWORD CMessageLog::MsgTypeWord(DWORD Flags) 
  { 
  return (Flags & (LogFlag_Fatal | LogFlag_Stop | LogFlag_Error | LogFlag_Warning | LogFlag_Note | LogFlag_Cmd | LogFlag_CondClear | LogFlag_Separator));
  };

LPCTSTR CMessageLog::MsgTypeString(DWORD iTyp)
  {
  switch (iTyp)
    {
    case LogFlag_Fatal:     return "Fatal"; 
    case LogFlag_Stop:      return "Stop";  
    case LogFlag_Error:     return "Err";   
    case LogFlag_Warning:   return "Wrn";   
    case LogFlag_CondClear: return "- Clr"; 
    case LogFlag_Note:      return "Nt";    
    case LogFlag_Cmd:       return "CMD";   
    case LogFlag_Separator: return "===";   
    }
  return NULL;
  }

DWORD CMessageLog::GetOptions()
  {
  return m_dwLogOptions;
  }

void CMessageLog::SetOptions(DWORD Opt)
  {
  m_dwLogOptions=Opt;
  }

BOOL CMessageLog::TestOption(DWORD Opt)
  {
#ifndef DISTRIBUTION
  DWORD DbgOpt=0;
  if (dbgLogCallsEntry   /*()*/)   DbgOpt |= LogOption_COMCallsEntry   ;
  if (dbgLogCallsExit    /*()*/)   DbgOpt |= LogOption_COMCallsExit    ;
  if (dbgLogCalls        /*()*/)   DbgOpt |= LogOption_COMCalls        ;
  if (dbgLogCallsInternal/*()*/)   DbgOpt |= LogOption_COMCallsInternal;
  return (((DbgOpt|m_dwLogOptions)&Opt)!=0);
#else
  return ((m_dwLogOptions&Opt)!=0);
#endif
  };

//===========================================================================

void CMessageLog::SetMsgIteration(long Value) 
  { 
  m_lMsgIteration=Value; 
  }; 
void CMessageLog::IncMsgIteration(long Value) 
  { 
  m_lMsgIteration+=Value; 
  }; 

void CMessageLog::ClrMsgBoxSkip() 
  { 
  m_bMessageBoxSkip=false; 
  };
void CMessageLog::SetMsgBoxSkip()
  { 
  m_bMessageBoxSkip=true; 
  };
bool CMessageLog::GetMsgBoxSkip() 
  { 
  return m_bMessageBoxSkip; 
  };
void CMessageLog::SetMsgBoxEmbedded() 
  { 
  m_bMessageBoxEmbedded=true; 
  };

void CMessageLog::ComCallStart() 
  { 
  if ((m_lComCallLevel++)==0) 
    {
    m_lComCallNo++;
    m_sComCallLogLn=""; 
    m_lComCallLnCnt=0;
    }
  };
void CMessageLog::ComCallEnd() 
  { 
  m_lComCallLevel--; 
  };
long CMessageLog::GetComCallLevel()
  {
  return m_lComCallLevel;
  };
int  CMessageLog::GetComCallLogLnCnt() 
  { 
  return m_lComCallLnCnt;
  };

LPCTSTR CMessageLog::GetComCallLogLn()
  {
  /*EnterCriticalSection(&m_errSect);
  RecentLogLn = LogMsgs[NLogMsgs].Str;
  LeaveCriticalSection(&m_errSect);*/
  return m_sComCallLogLn;
  }

BOOL CMessageLog::RegisterMsgWnd(HWND hWnd, DWORD LogMsgID)
  {
  m_hMsgWnd=hWnd;
  m_dwLogMsgID=LogMsgID;
  
  return true;
  };

BOOL CMessageLog::DeRegisterMsgWnd()
  {
  m_hMsgWnd=NULL;
  m_dwLogMsgID=0;
  
  return true;
  };

void CMessageLog::Entry()
  {
  //  InitializeCriticalSection(&errSect);
  }

void CMessageLog::Exit()
  {
  //  DeleteCriticalSection(&errSect);
  }

BOOL CMessageLog::LogFileOpen(LPCTSTR Fn)
  {
  EnterCriticalSection(&m_errSect);
  LogFileClose();
  char Drv[_MAX_DRIVE];
  char Path[_MAX_DIR];
  char Name[_MAX_FNAME];
  char Ext[_MAX_EXT];
  _splitpath(Fn?Fn:"", Drv, Path, Name, Ext);
  
  if (strlen(Drv)==0 && strlen(Path)==0)
    {
    char WDir[1024];
    CString LFiles;
    if (GetTempPath(sizeof(WDir), WDir)>0)
      _splitpath(WDir, Drv, Path, NULL,NULL);
    else
      _splitpath("C:\\", Drv, Path, NULL,NULL);
    strcat(Path, "SysCAD\\");
    }

  m_sLogFile=Drv;
  m_sLogFile+=Path;
  m_sLogFile+=Name;
  m_sLogFile+=Ext;

  if (m_sLogFile.GetLength()>0)
    {
    unlink(m_sLogFile);
    m_hLogFile = fopen(m_sLogFile, "wt");
    if (m_hLogFile)
      {
      fprintf(m_hLogFile, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n",
        "Cmd", "Typ", "Where", "IDNo", "IterNo", "CallNo", "SeqNo", "HResult", "Msg", "Date", "Time");
      }
    }

  LeaveCriticalSection(&m_errSect);
  return (m_hLogFile!=NULL);
  }              

void CMessageLog::LogFileClose()
  {
  EnterCriticalSection(&m_errSect);
  if (m_hLogFile) 
    fclose(m_hLogFile);
  m_hLogFile = NULL;
  LeaveCriticalSection(&m_errSect);
  }
                                                                                   
int CMessageLog::LogLn(HWND hWndOwn, DWORD Flags, LPCTSTR Where, UINT BoxFlags, LPCTSTR Msg, long SeqNo, HRESULT hr)
  {
  const int MsgBuffLen=4096;
  char FileStr[MsgBuffLen];
  char DbgStr[MsgBuffLen];
  //char AfxStr[MsgBuffLen];
  char DbgLine[MsgBuffLen+MsgBuffLen];
  char DT[64],TM[64];

  DWORD ItmFlags=BoxFlags;
  if (1)
    {
    EnterCriticalSection(&m_errSect);

    CMsgLogItem Itm;

    Itm.m_dwFlags=Flags | m_dwCommonMsgFlags;
    Itm.m_MsgBoxFlags=BoxFlags;  
    Itm.m_lIterNo=m_lMsgIteration;
    Itm.m_lIDNo=++m_lIDNo;
    Itm.m_lCallNo=m_lComCallNo;
    Itm.m_hr=m_hr;
    Itm.m_lSeqNo=SeqNo;  
    
    CString sHr(" ");
    if (m_hr)
      sHr.Format("0x%08x", m_hr); 

    _strdate(DT);
    _strtime(TM);
    sprintf(FileStr, "\"%i\",\"%i\",\"%i\",\"%i\",\"%s\",\"%s\",\"%s\",\"%s\"",
      Itm.m_lIDNo, Itm.m_lIterNo, Itm.m_lCallNo, Itm.m_lSeqNo, (LPCTSTR)sHr, Msg, DT,TM);
    sprintf(DbgStr, "%i\t%s\t%s\t%s", m_lMsgIteration, Msg, DT,TM);
    
    Itm.AddSource(Where);
    Itm.AddDescription(Msg);

    DWORD iTyp=CMessageLog::MsgTypeWord(Flags);
    LPCTSTR pTyp=CMessageLog::MsgTypeString(iTyp);

    if (m_hLogFile)
      {
      long filesize=ftell(m_hLogFile);
      if (filesize >= (1<<20))
        {
        LogFileClose();
        LogNote("SysCAD", 0, "Error File Truncated");
        };
      }
  
    if (iTyp==LogFlag_Cmd)
      {
      if (m_hLogFile)
        {
        LPCTSTR pCmd=strchr(Msg, ' ');
        if (pCmd) 
          pCmd++;
        else
          pCmd=Msg;

        fprintf(m_hLogFile, "\"%s\",\"%s\",\"%s\",%s\n", pCmd, pTyp, Where, FileStr);
        fflush(m_hLogFile);
        }
      sprintf(DbgLine, "CMD: %s\n", DbgStr);
      }
    else if (pTyp)
      {
      if (m_hLogFile)
        {
        fprintf(m_hLogFile, "\"\",\"%s\",\"%s\",%s\n", pTyp, Where, FileStr);
        fflush(m_hLogFile);
        }
      sprintf(DbgLine, "Log: %-3.3s  %-15s  %s", pTyp, Where, DbgStr);
      }
    else  
      sprintf(DbgLine, "Log:---");
  
    //Itm.m_Str[sizeof(Itm.m_Str)-1]=0;

    for (int q=0; q<(m_lComIFCnt>0 ? 2:1); q++)
      {
      CMsgLogArray &Msgs=(q==0 ? *m_WndQ.m_pMsgs:*m_ComQ.m_pMsgs);
      UINT & Cnt=(q==0 ? m_WndQ.m_nCount:m_ComQ.m_nCount);

      UINT n=Cnt+1;
      if (n>Msgs.size())
        {
        UINT no=Msgs.size();
        UINT ns=((n/256)+1)*256;
        Msgs.resize(ns);
        for (UINT i=no; i<ns; i++)
          Msgs[i]=new CMsgLogItem;
        }
      *Msgs[Cnt++]=Itm;
      }

//    if (m_pRmtLogMsgs)
//      {
//      m_pRmtLogMsgs->push_back(Itm);
//      }
    if (m_lComCallLevel)
      {
      m_lComCallLnCnt++;
      m_sComCallLogLn = Itm.Source();
      m_sComCallLogLn += " ";
      m_sComCallLogLn += Itm.Description();
      }
    LeaveCriticalSection(&m_errSect);
    }

  if (dbgtestfileopen())
    dbgpln("%s", DbgLine);

  if (m_lComCallLevel)
    return 0; //NO USER PROMPT MESSAGE BOXES !!!

  int RetVal=0;

  int MustDo=((BoxFlags&LF_NoSkip)!=0);
  BoxFlags&=~LF_NoSkip;

  if (!m_bMessageBoxEmbedded && (m_hMsgWnd==NULL || ((BoxFlags&LF_DoAfxMsgBox) && (MustDo || !GetMsgBoxSkip()))))
    {//message window has not been created yet...
//dbgpln("MsgBox >>");
    if ((BoxFlags & (MB_ABORTRETRYIGNORE|MB_OK|MB_OKCANCEL|MB_RETRYCANCEL|MB_YESNO|MB_YESNOCANCEL))==0)
      {
      CString SMsg(Msg);
      //SMsg+="\n\nIgnore other messages ?";
      SMsg+="\n\nIgnore other messages during this command?";
//      if (AfxMessageBox(SMsg(), (BoxFlags & ~LF_DoAfxMsgBox)|MB_YESNO)==IDYES)
      if (::MessageBox(hWndOwn, SMsg, "SysCAD", (BoxFlags & ~LF_DoAfxMsgBox)|MB_YESNO)==IDYES)
        SetMsgBoxSkip();
      }
    else
      //RetVal=AfxMessageBox(Msg, BoxFlags & ~LF_DoAfxMsgBox);
      RetVal=::MessageBox(hWndOwn, Msg, "SysCAD", (BoxFlags & ~LF_DoAfxMsgBox));

//dbgpln("MsgBox <<");
    }
  else if (!m_bMessageBoxEmbedded && ((BoxFlags!=0) && (MustDo || !GetMsgBoxSkip())))
    {
    //while (!PostMessage(m_hMsgWnd, m_dwLogMsgID, 0, 0))
    //  Sleep(0);
  	//AfxGetApp()->EnableModeless(FALSE);

    // remove Buttons - all in lower 4 Bits
    BoxFlags=((BoxFlags & ~0xF) | MB_OKCANCEL);
    if (SendMessage(m_hMsgWnd, m_dwLogMsgID, BoxFlags, 0))
      SetMsgBoxSkip();
  	//AfxGetApp()->EnableModeless(TRUE);

    }
  else 
    {
    if (m_lLogDeferred==0)
      {
      //on windows 95/98 without the counter this can go into "infinite" loop!
      int Cnt=0;
      while (!PostMessage(m_hMsgWnd, m_dwLogMsgID, 0, 0) && Cnt++<4)
        Sleep(0);
      }
    }
  return RetVal;
  }

int CMessageLog::LogLn(DWORD Flags, LPCTSTR Where, UINT BoxFlags, LPCTSTR Msg, long SeqNo, HRESULT hr)
  { 
  return LogLn((AfxGetMainWnd() ? AfxGetMainWnd()->m_hWnd : NULL), Flags, Where?Where:"???", BoxFlags, Msg, SeqNo, hr); 
  }
                                                   
void CMessageLog::SetAppStarted()
  {
  m_bLogAppStarted=true;   
  };

void CMessageLog::SetDefer(BOOL On)
  {
  if (On)
    m_lLogDeferred++;
  else
    m_lLogDeferred--;
  };

void CMessageLog::FlushWndQ()
  {
  if (m_lLogDeferred==0 && m_WndQ.m_nCount>0)
    {
    //on windows 95/98 without the counter this can go into "infinite" loop!
    int Cnt=0;
    while (!PostMessage(m_hMsgWnd, m_dwLogMsgID, 0, 0) && Cnt++<4)
      Sleep(0);
    }
  };

BOOL CMessageLog::GetWndQMsg(DWORD N, CMsgLogItem & Msg)
  {
  bool OK=false;
  EnterCriticalSection(&m_errSect);
  if (N<m_WndQ.m_nCount)
    {
    Msg=(*(*m_WndQ.m_pMsgs)[N]);
    OK=true;
    }
  else
    m_WndQ.m_nCount=0;
  LeaveCriticalSection(&m_errSect);
  return OK;
  };

void CMessageLog::FlushComQ(DWORD NewLength)
  {
  EnterCriticalSection(&m_errSect);
  if (NewLength>0)
    {
    static CMsgLogArray TmpLog;
    CMsgLogArray &Log=(*m_ComQ.m_pMsgs);
    DWORD size=Log.size();
    if (TmpLog.size()<size)
      TmpLog.resize(size);
    for (DWORD i=0; i<size; i++)
      TmpLog[i]=Log[i];
    // Move Those Required
    long Start=m_ComQ.m_nCount-NewLength; 
    for (i=0; i<NewLength; i++)
      Log[i]=TmpLog[Start+i];
    long j=0;
    while (i<m_ComQ.m_nCount)
      Log[i++]=TmpLog[j++];
    }
  m_ComQ.m_nCount=NewLength;
  
  LeaveCriticalSection(&m_errSect);
  };

BOOL CMessageLog::GetComQMsg(DWORD N, CMsgLogItem & Msg)
  {
  bool OK=false;
  EnterCriticalSection(&m_errSect);
  if (N<m_ComQ.m_nCount)
    {
    Msg=(*(*m_ComQ.m_pMsgs)[N]);
    OK=true;
    }
//  else
//    m_ComQ.m_nCount=0;
  LeaveCriticalSection(&m_errSect);
  return OK;
  };

int CMessageLog::LogSeparator(LPCTSTR Where, UINT BoxFlags)
  {
  char S[4096]; 
  char *pSep="------------------------------------------------";
  int L=strlen(Where);
  int N=50;
  int NS=(N-(L+2));
  int S0=NS/2>=1 ? NS/2:1;
  int S1=NS-S0>=1 ? NS-S0:1;
  
  S0=50;
  sprintf(S, "%.*s %s.", S0, pSep, Where);
  return LogLn(LogFlag_Separator, "", BoxFlags, S);
  }

void CMessageLog::Clear()
  {
  LogLn(LogFlag_ClearErrList | LogFlag_ClearCondList, "", 0, "");
  }

void CMessageLog::ClearErrList()
  {
  LogLn(LogFlag_ClearErrList, "", 0, "");
  }

void CMessageLog::ClearCondList()
  {
  LogLn(LogFlag_ClearCondList, "", 0, "");
  }

BOOL CMessageLog::AutoClear(BOOL On)
  {
  bool WasOn=(m_dwCommonMsgFlags & LogFlag_AutoClear)!=0;
  if (On)
    m_dwCommonMsgFlags = m_dwCommonMsgFlags | LogFlag_AutoClear;
  else
    m_dwCommonMsgFlags = m_dwCommonMsgFlags & ~LogFlag_AutoClear;
  return WasOn;
  }

void CMessageLog::IncrementComIFCnt()
  {
  InterlockedIncrement(&m_lComIFCnt);
  };
void CMessageLog::DecrementComIFCnt()
  {
  InterlockedDecrement(&m_lComIFCnt);
  };

//void CMessageLog::SetHook(MessageLogHookFn Hook)
//  {
//  m_MessageLogHook = Hook;
//  };

//===================================================================
//
//
//
//===================================================================

int LogLn(HWND hWndOwn, DWORD Flags, LPCTSTR Where, UINT BoxFlags, LPCTSTR Msg, long SeqNo, HRESULT hr)
  {
  return gs_MsgLog.LogLn(hWndOwn, Flags, Where, BoxFlags, Msg, SeqNo, hr);
  }

int LogLn(DWORD Flags, LPCTSTR Where, UINT BoxFlags, LPCTSTR Msg, long SeqNo, HRESULT hr)
  {
  return gs_MsgLog.LogLn(Flags, Where, BoxFlags, Msg, SeqNo, hr);
  }

//===================================================================
//
//
//
//===================================================================

#define DOVARARGS(fmt)      \
  char S[4096];       \
  va_list argptr;           \
  va_start(argptr,fmt);     \
  vsprintf(S, fmt, argptr); \
  va_end(argptr);

//===================================================================
//
//
//
//===================================================================


//DWORD LogMsgTypeWord(DWORD Flags)               { return gs_MsgLog.LogMsgTypeWord(Flags);           }
//LPCTSTR  LogMsgTypeString(DWORD iTyp)           { return gs_MsgLog.LogMsgTypeString(iTyp);          }
//                                                                                                    
//void SetLogMsgIteration(long Value)             { gs_MsgLog.SetLogMsgIteration(Value);              }
//void IncLogMsgIteration(long Value)             { gs_MsgLog.IncLogMsgIteration(Value);              }
                                                                                                        
//void ClrMsgBoxSkip()                            { gs_MsgLog.ClrMsgBoxSkip();                        }
//void SetMsgBoxSkip()                            { gs_MsgLog.SetMsgBoxSkip();                        }
//bool GetMsgBoxSkip()                            { return gs_MsgLog.GetMsgBoxSkip();                 }
//void SetMsgBoxEmbedded()                        { gs_MsgLog.SetMsgBoxEmbedded();                    }
//                                                                                                        
//void ComCallStart()                             { gs_MsgLog.ComCallStart();                         }
//void ComCallEnd()                               { gs_MsgLog.ComCallEnd();                           }
//bool GetComCallLevel()                          { return gs_MsgLog.GetComCallLevel();               }
//int  GetComCallLogLnCnt()                       { return gs_MsgLog.GetComCallLogLnCnt();            }
//LPCTSTR GetComCallLogLn()                       { return gs_MsgLog.GetComCallLogLn();               }
                                                                                                        
//void LogEntry()                                 { gs_MsgLog.Entry();                                }
//void LogExit()                                  { gs_MsgLog.Exit();                                 }
//                                                                                                        
//BOOL LogFileOpen(LPCTSTR Fn)                    { return gs_MsgLog.LogFileOpen(Fn);                 } 
//void LogFileClose()                             { gs_MsgLog.LogFileClose();                         } 
                                                                                                    
//void LogClear()                                 { gs_MsgLog.LogClear();                             }
//void LogClearErrList()                          { gs_MsgLog.LogClearErrList();                      }
//void LogClearCondList()                         { gs_MsgLog.LogClearCondList();                     }
//BOOL LogAutoClear(BOOL On)                      { return gs_MsgLog.LogAutoClear(On);                }
                                                                                                    
//DWORD GetLogOptions()                           { return gs_MsgLog.GetLogOptions();                 }
//void SetLogOptions(DWORD Opt)                   { gs_MsgLog.SetLogOptions(Opt);                     }
//BOOL TestLogOption(DWORD Opt)                   { return gs_MsgLog.TestLogOption(Opt);              }
                                                                                                    
//void LogSetAppStarted()                         { gs_MsgLog.LogSetAppStarted();                     }
//void LogSetDefer(BOOL On)                       { gs_MsgLog.LogSetDefer(On);                        }
//void LogFlush2Wnd()                             { gs_MsgLog.LogFlush2Wnd();                         }
//BOOL GetLogMsg4Wnd(int N, CMsgLogItem & Msg)    { return gs_MsgLog.GetLogMsg4Wnd(N, Msg);           }
                                                                                                    
//BOOL RegisterMsgWnd(HWND hWnd, DWORD LogMsgID)  { return gs_MsgLog.RegisterMsgWnd(hWnd, LogMsgID);  }
//BOOL DeRegisterMsgWnd()                         { return gs_MsgLog.DeRegisterMsgWnd();              }

int LogSeparator(LPCTSTR Where, UINT BoxFlags)  { return gs_MsgLog.LogSeparator(Where, BoxFlags);   }


int LogFatal(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(LogFlag_Fatal, Where, BoxFlags, S);
  }

int LogFatal(HWND hWndOwn, LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(hWndOwn, LogFlag_Fatal, Where, BoxFlags, S);
  }

int LogStop(HWND hWndOwn, LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(hWndOwn, LogFlag_Stop, Where, BoxFlags, S);
  }

int LogStop(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(LogFlag_Stop, Where, BoxFlags, S);
  }

int LogError(HWND hWndOwn, LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(hWndOwn, LogFlag_Error, Where, BoxFlags, S);
  }

int LogError(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  //char x=0;
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(LogFlag_Error, Where, BoxFlags, S);
  }

int LogWarning(HWND hWndOwn, LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(hWndOwn, LogFlag_Warning, Where, BoxFlags, S);
  }

int LogWarning(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(LogFlag_Warning, Where, BoxFlags, S);
  }

int LogNote(HWND hWndOwn, LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(hWndOwn, LogFlag_Note, Where, BoxFlags, S);
  }                                               

int LogNote(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(LogFlag_Note, Where, BoxFlags, S);
  }

int LogCmd(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(LogFlag_Cmd, Where, BoxFlags, S);
  }

int LogCondClear(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(LogFlag_CondClear, Where, BoxFlags, S);
  }

int PgmLogError(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(LogFlag_FromPGM|LogFlag_Error, Where, BoxFlags, S);
  }

int PgmLogWarning(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(LogFlag_FromPGM|LogFlag_Warning, Where, BoxFlags, S);
  }

int PgmLogNote(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(LogFlag_FromPGM|LogFlag_Note, Where, BoxFlags, S);
  }

int ComLogError(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(LogFlag_FromCOM|LogFlag_Error, Where, BoxFlags, S);
  }

int ComLogWarning(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(LogFlag_FromCOM|LogFlag_Warning, Where, BoxFlags, S);
  }

int ComLogNote(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  DOVARARGS(fmt);
  return gs_MsgLog.LogLn(LogFlag_FromCOM|LogFlag_Note, Where, BoxFlags, S);
  }

LPCSTR GetHResultString(CString &Str, HRESULT hr)
  {
  if(FAILED(hr))
    {
    char  msg[2048];
    ::FormatMessage(
      FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      hr,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
      msg,
      MAX_PATH,
      NULL);
    Str=msg;
    }
  else
    Str="";
  return Str;
  }

// ========================================================================
