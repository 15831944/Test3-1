// DebugLib.cpp: implementation of the DebugFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include <math.h>
#include <float.h>

#define __DEBUGLIB_CPP
#include "DebugLib.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//#include "dbgmngr.h"
//
//static CDbgMngr  dbgTrace  ("_Debug",  "ToDebugTrace");
//
HANDLE _dbgf = NULL;
char dbgf_fn[256];
static CRITICAL_SECTION dbgSect;

class DbgInitialise
  {
  public:
    DbgInitialise()
      {
      //InitializeCriticalSection(&dbgSect);
      InitializeCriticalSectionAndSpinCount(&dbgSect, 4000);
      }
    ~DbgInitialise()
      {
      DeleteCriticalSection(&dbgSect);
      dbgfileclose();
      }
  };

static DbgInitialise gs_DbgInitialise;

//----------------------------------------------------------------------------

bool dbgtestfileopen()  { return _dbgf!=NULL; };
HANDLE dbgfile()    { return _dbgf; };
char *dbgfilename() { return dbgf_fn; };
void dbglock() { EnterCriticalSection(&dbgSect); };
void dbgunlock() { LeaveCriticalSection(&dbgSect); };

//----------------------------------------------------------------------------

bool dbgfileopen(LPCTSTR Fn)
  {
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

  CString SFn;
  SFn=Drv;
  SFn+=Path;
  SFn+=Name;
  SFn+=Ext;

  strncpy(dbgf_fn, SFn, sizeof(dbgf_fn));
  _dbgf = CreateFile(dbgf_fn, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);

  return (_dbgf!=NULL);
  };

//----------------------------------------------------------------------------

void dbgfileclose(void)
  {
  if (_dbgf)
    {
//    DeleteCriticalSection(&dbgSect);
    CloseHandle(_dbgf);
    //fclose(_dbgf);
    }
  _dbgf=NULL;
  };

//----------------------------------------------------------------------------

static void ExpandNewLine(char * buff)
  {
  char *p=strchr(buff, '\n');
  while (p)
    {
    memmove(p+1,p,strlen(p)+1);
    *p='\r';
    p=strchr(p+2, '\n');
    }
  }

//----------------------------------------------------------------------------
/*#F:This truncates the dbg file to the specified length, default is zero*/
void dbgfiletrunc(long size )   //the required end-size of the existing file
  {
  if ( _dbgf )
    {
    struct tm* newtime;
    __time64_t aclock;
    _time64 ( &aclock );                 /* Get time in seconds */
    newtime = _localtime64 ( &aclock );  /* Convert time to struct */
    EnterCriticalSection ( &dbgSect );
    //fseek ( _dbgf, 0L, SEEK_END ); // get size of file
    long actSize = GetFileSize( _dbgf ,NULL);
    if ( size > actSize )
      size = 0;
    if (0)// size != 0 )
      {
      /*
      LPCTSTR bunker = new char [size+1];
//      fclose ( _dbgf );
      CloseHanlde(_dbgf );
      //_dbgf = fopen ( dbgf_fn, "rt" );
      _dbgf = CreateFile(dbgf_fn, GENERIC_WRITE, FILE_SHARE_READ, NULL, , "rt" );

      long location = actSize - size;
      fseek ( _dbgf, location, SEEK_SET );
      //    DWORD after = oldFile.GetPosition();
      //    oldFile.SeekToEnd (); //move pointer from end of dbg file
      //    DWORD ending = oldFile.GetPosition();
      size_t last = fread ( bunker, sizeof( char ), size, _dbgf );
      bunker[last] = '\0';// to null terminate buffer
      fclose ( _dbgf );
      _dbgf = _fsopen ( dbgf_fn, "wt" , _SH_DENYWR);
      if (_dbgf)
        {
        //char IstLine[256];
        if (newtime)
          fprintf ( _dbgf, "This file has been shortened on %s", asctime( newtime ));
        //IstLine[59] = '\0'; // to ensure that string is null terminated
        //fwrite( IstLine,  strlen(IstLine) );
        LPCTSTR cleanStart = strchr( bunker,'\n' );
        if ( cleanStart != NULL )
          fwrite( (cleanStart+1) , sizeof( char ), ( last - (cleanStart+1-bunker) ), _dbgf );
        else
          fwrite( bunker, sizeof( char ), last, _dbgf );
        //newFile.Close();
        }
      delete bunker;
      */
      }
    else
      { // size == 0
      CloseHandle( _dbgf );
      //_dbgf = _fsopen ( dbgf_fn, "wt" , _SH_DENYWR);
      _dbgf = CreateFile(dbgf_fn, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
      if (newtime)
        {
        DWORD nWrt;
        char buff[512];
        sprintf(buff, "This file has been shortened on %s\n", asctime( newtime ));
        ExpandNewLine(buff);
        WriteFile(_dbgf, buff, strlen(buff), &nWrt, NULL);
        }
      };
    //FILE * newDbgFile = _fsopen( test(), "at", _SH_DENYWR);
    //dbgopen( test() );
    //_dbgf _seek
    if (_dbgf)
      FlushFileBuffers(_dbgf);
      //FlushFileBuffers(_dbgf);
    LeaveCriticalSection ( &dbgSect );
    }
  else
    {
    //_dbgf = _fsopen (dbgf_fn, "wt", _SH_DENYWR); //deletes file
    _dbgf = CreateFile(dbgf_fn, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
    if (_dbgf)
      CloseHandle(_dbgf);
    _dbgf = NULL;
    }
  };

//----------------------------------------------------------------------------

static char AtLineStart = 1;
static byte dbgflushOn = 0;
static int DbgIndentCnt = 0;
static char *IndentStr = "                   ";
int currentdbglevel = MAX_DEBUG_LEVEL;

//----------------------------------------------------------------------------

static CStopWatch TimeSW;
static int dbgtimeopts=0; //0x01 = Time 0x02 = Diff
static double dbgstarttime=0;
static double dbglasttime=0;
static bool BusyWithTime=false;
static bool DoneTimeHdr=false;
void dbgtime(int Opt)
  {
  if (Opt!=dbgtimeopts)
    {
    dbgtimeopts =Opt;
    dbgstarttime=TimeSW.Time();
    dbglasttime=dbgstarttime;
    DoneTimeHdr=false;
    }
  };

static void dodbgtime()
  {
  if (AtLineStart && dbgtimeopts && !BusyWithTime)
    {
    BusyWithTime=true;
    double T1=TimeSW.Time();
    double T2=T1-dbgstarttime;
    double DT=T1-dbglasttime;
    dbglasttime=T1;

    if (!DoneTimeHdr)
      {
      DoneTimeHdr=true;
      if (dbgtimeopts&2)
        dbgpln("%11s %11s| ", "Time(s)", "dTime(ms)");
      else
        dbgpln("%14s| ", "Time(s)");
      }

    if (dbgtimeopts&2)
      dbgp("%11.3f %11.3f| ", T2,DT*1000);
    else
      dbgp("%14.6f| ", T2);
    BusyWithTime=false;
    }
  };
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

static char dbgbuff[64000];
static DWORD dbgWrt;

void dbgpln(char * fmt, ...)
  {
  if (_dbgf && (currentdbglevel >= MAX_DEBUG_LEVEL))
    {
    dbglock();
    //if (hDbgSema)
    //  WaitForSingleObject(hDbgSema,INFINITE);
    dodbgtime();
    if (AtLineStart && (DbgIndentCnt > 1))
      {
      sprintf(dbgbuff, "%.*s",DbgIndentCnt,IndentStr);
      WriteFile(_dbgf, dbgbuff, strlen(dbgbuff), &dbgWrt, NULL);
      }
    va_list argptr;
    va_start(argptr,fmt);
    vsprintf(dbgbuff, fmt, argptr);
    va_end(argptr);

    ExpandNewLine(dbgbuff);
    WriteFile(_dbgf, dbgbuff, strlen(dbgbuff), &dbgWrt, NULL);
    //if (dbgTrace())
    //  OutputDebugString(dbgbuff);
    //if (dbgTrace())
    //  OutputDebugString("\n");
    strcpy(dbgbuff, "\n");
    ExpandNewLine(dbgbuff);
    WriteFile(_dbgf, dbgbuff, strlen(dbgbuff), &dbgWrt, NULL);
    AtLineStart = 1;
    if (dbgflushOn)
      FlushFileBuffers(_dbgf);
    dbgunlock();
    //if (hDbgSema)
    //  ReleaseSemaphore(hDbgSema,1,NULL);
    }
  };

//----------------------------------------------------------------------------

void dbgp(char * fmt, ...)
  {
  if (_dbgf && (currentdbglevel >= MAX_DEBUG_LEVEL))
    {
    dbglock();
    //if (hDbgSema)
    //  WaitForSingleObject(hDbgSema,INFINITE);
    dodbgtime();
    if (AtLineStart && (DbgIndentCnt > 0))
      {
      sprintf(dbgbuff, "%.*s",DbgIndentCnt,IndentStr);
      WriteFile(_dbgf, dbgbuff, strlen(dbgbuff), &dbgWrt, NULL);
      }
    va_list argptr;
    va_start(argptr,fmt);
    vsprintf(dbgbuff, fmt, argptr);
    va_end(argptr);

if (1)
  {
  if (strstr(dbgbuff, "0              0               0"))
    { int xxx=0; }
  }

    ExpandNewLine(dbgbuff);
    WriteFile(_dbgf, dbgbuff, strlen(dbgbuff), &dbgWrt, NULL);
    //if (dbgTrace())
    //  OutputDebugString(dbgbuff);

    if (strcmp(&fmt[strlen(fmt)-2], "\n") == 0)
      AtLineStart = 1;
    else
      AtLineStart = 0;
    if (dbgflushOn)
      FlushFileBuffers(_dbgf);
    dbgunlock();
    //if (hDbgSema)
    //  ReleaseSemaphore(hDbgSema,1,NULL);
    }
  };

//----------------------------------------------------------------------------
/*
bool dbgat_on = 0;
int dbgatcnt = 0;
char dbgatlist[DBGATLISTLEN][DBGATLEN];

void dbgat(char * fmt, ...)
  {
  if (dbgat_on && _dbgf)
    {
    va_list argptr;
    va_start(argptr,fmt);
    vsprintf(dbgatlist[dbgatcnt], fmt, argptr);
    va_end(argptr);

//    if (1)
      {
      dbglock();
      //if (hDbgSema)
      //  WaitForSingleObject(hDbgSema,INFINITE);
      dbgnln();
      fprintf(_dbgf, "%.*s",DbgIndentCnt,IndentStr);
      fprintf(_dbgf, "@:%s\n",dbgatlist[dbgatcnt]);
      AtLineStart = 1;
      if (dbgflushOn)
        FlushFileBuffers(_dbgf);
      dbgunlock();
      //if (hDbgSema)
      //  ReleaseSemaphore(hDbgSema,1,NULL);
      }

    dbgatcnt = (dbgatcnt+1)%DBGATLISTLEN;
    }
  };
*/
//----------------------------------------------------------------------------

void dbgnln()
  {
  if (_dbgf && (currentdbglevel >= MAX_DEBUG_LEVEL))
    {
    dbglock();
    //if (hDbgSema)
    //  WaitForSingleObject(hDbgSema,INFINITE);
    dodbgtime();
    if (!AtLineStart)
      {
      //if (dbgTrace())
      //  OutputDebugString("\n");
      strcpy(dbgbuff, "\n");
      ExpandNewLine(dbgbuff);
      WriteFile(_dbgf, dbgbuff, strlen(dbgbuff), &dbgWrt, NULL);
      //fprintf(_dbgf, "\n");
      }
    AtLineStart = 1;
    if (dbgflushOn)
      FlushFileBuffers(_dbgf);
    dbgunlock();
    //if (hDbgSema)
    //  ReleaseSemaphore(hDbgSema,1,NULL);
    }
  };

//----------------------------------------------------------------------------

void dbgfileflush(bool On)
  {
  if (_dbgf)
    {
    dbglock();
    //if (hDbgSema)
    //  WaitForSingleObject(hDbgSema,INFINITE);
// very slow
//    FlushFileBuffers(_dbgf);
//    if (On)
//      { if (dbgflushOn < 255) dbgflushOn++; }
//    else
//      { if (dbgflushOn > 0) dbgflushOn--; }
    dbgunlock();
    //if (hDbgSema)
    //  ReleaseSemaphore(hDbgSema,1,NULL);
    }
 };

//----------------------------------------------------------------------------

void dbgindent(int chg, LPCTSTR where)
  {
  const int indentdbgon=0;
  if (indentdbgon && (chg>0))
    dbgpln(">> %3i %2i %s", DbgIndentCnt, chg, where?where:"");
  DbgIndentCnt += chg;
  if (indentdbgon && (chg<0))
    dbgpln("<< %3i %2i %s", DbgIndentCnt, -chg, where?where:"");
  }

int dbgindentcnt()
  {
  return DbgIndentCnt;
  }

//===========================================================================

//void cleardumpatlist(void)
//  {
//  memset(dbgatlist, 0, sizeof(dbgatlist));
//  };

//===========================================================================

//void dumpatlist(void)
//  {
//  int i = dbgatcnt;
//  while (i != dbgatcnt)
//    {
//    if (strlen(dbgatlist[i])>0)
//      dbgpln("***:%s", dbgatlist[i]);
//    i = (i+1)%DBGATLISTLEN;
//    }
//  };
//

//===========================================================================

//#define  NEW_BUFFER 16
//void* dbgnew(size_t size, LPCTSTR s)
//  {
//  DWORD sl=strlen(s);
//  LPCTSTR p= (LPCTSTR)new char[NEW_BUFFER+sizeof(word)+sl+size+NEW_BUFFER];
//  memset(p, 0x5a, NEW_BUFFER);
//  p+=NEW_BUFFER;
//  *((word*)p)=size;
//  p+=sizeof(word);
//  memcpy(p, s, sl);
//  p+=sl;
//  memset(p+size, 0x5a, NEW_BUFFER);
//  dbgpln("new   : '%s' %i %p ",s,size,p);
//  return p;
//  };
//
////---------------------------------------------------------------------------
//
//void dbgdel(void* p, LPCTSTR s)
//  {
//  char trouble=0;
//  size_t size;
//  word sl=strlen(s);
//  LPCTSTR pc=(LPCTSTR)p,ps;
//  pc-=sl;
//  ps=pc;
//  pc-=sizeof(word);
//  size=*((word*)pc);
//  pc-=NEW_BUFFER;
//  dbgp("delete: '%s' %i %p %*.*s ",s,size,p,sl,sl,ps);
//  for (int i =0; i < NEW_BUFFER; i++)
//    if (pc[i]!=0x5a)
//      {
//      trouble=1;
//      dbgp(" %i.%2x",i,pc[i]);
//      }
//  LPCTSTR pe=(LPCTSTR)p+size;
//  for (i =0; i < NEW_BUFFER; i++)
//    if (pe[i]!=0x5a)
//      {
//      trouble=1;
//      dbgp(" %i.%2x",i,pe[i]);
//      }
//  if (trouble)
//    dbgp("############# TROUBLE ############");
//  dbgpln("");
//  delete[]pc;
//  };

//---------------------------------------------------------------------------

static DWORD PrvdwAvailPhys = 0;
static DWORD PrvdwAvailPageFile = 0;
static DWORD PrvdwVirtualUsed = 0;

void dbgMemoryState(LPCTSTR Where)
  {
  MEMORYSTATUS MS;
  MS.dwLength=sizeof(MS);

  GlobalMemoryStatus(&MS);

  DWORD dwVirtualUsed = MS.dwTotalVirtual-MS.dwAvailVirtual;

  dbgpln("MemState: %3i%% FreePhys:%6i[%+5i] FreePg:%6i[%+5i] VirtUsed:%6i[%+5i] %s",
    MS.dwMemoryLoad,          // percent of memory in use
    MS.dwAvailPhys>>10,      // free physical memory bytes
    ((long)(MS.dwAvailPhys>>10)-(long)(PrvdwAvailPhys>>10)),
    MS.dwAvailPageFile>>10,  // free bytes of paging file
    ((long)(MS.dwAvailPageFile>>10)-(long)(PrvdwAvailPageFile>>10)),
    dwVirtualUsed>>10,
    ((long)(dwVirtualUsed>>10)-(long)(PrvdwVirtualUsed>>10)),
    Where ? Where : "");

  PrvdwAvailPhys=MS.dwAvailPhys;
  PrvdwAvailPageFile=MS.dwAvailPageFile;
  PrvdwVirtualUsed=dwVirtualUsed;
  /*
  dbgpln("MemState: %3i%% Phys:%9i FreePhys:%9i Pg:%9i "
         "FreePg:%9i TotVirt:%9i AvlVirt:%9i",
    MS.dwMemoryLoad,    // percent of memory in use
    MS.dwTotalPhys,     // bytes of physical memory
    MS.dwAvailPhys,     // free physical memory bytes
    MS.dwTotalPageFile, // bytes of paging file
    MS.dwAvailPageFile, // free bytes of paging file
    MS.dwTotalVirtual,  // user bytes of address space
    MS.dwAvailVirtual); // free user bytes
  */
  }

//===========================================================================

void dbgTimeStamp(LPCTSTR Where)
  {
  };

//===========================================================================

LPCTSTR CStopWatch::MinSecDesc(double Seconds, CString & s)
  {
  Seconds = floor(Seconds*1.0e6+0.5)/1.0e6; //round for lost precision on LSD
  const long lMin = ((long)(Seconds+0.5))/60;
  const long lSecs = ((long)Seconds)%60;
  s.Format("%d:%02d", lMin, lSecs);
  return s;
  }

//---------------------------------------------------------------------------

LPCTSTR CStopWatch::MinSecHunDesc(double Seconds, CString & s)
  {
  Seconds = floor(Seconds*1.0e6+0.5)/1.0e6; //round for lost precision on LSD
  const long lHSecs = (long)floor(((Seconds-floor(Seconds))+0.0005)*1000.0);
  const long lSecs  = ((long)Seconds)%60;
  const long lMin   = ((long)(Seconds))/60;
  s.Format("%d:%02d.%03d", lMin, lSecs, lHSecs);
  return s;
  }

//---------------------------------------------------------------------------

LPCTSTR CStopWatch::HourMinSecHunDesc(double Seconds, CString & s)
  {
  Seconds = floor(Seconds*1.0e6+0.5)/1.0e6; //round for lost precision on LSD
  const long lHSecs = (long)floor(((Seconds-floor(Seconds))+0.0005)*1000.0);
  const long lSecs  = ((long)Seconds)%60;
  const long lMin   = (((long)(Seconds))/60)%60;
  const long lHours = ((long)Seconds)/3600;
  s.Format("%d:%02d:%02d.%03d", lHours, lMin, lSecs, lHSecs);
  return s;
  }

//---------------------------------------------------------------------------

LPCTSTR CStopWatch::DayHourMinSecHunDesc(double Seconds, CString & s)
  {
  Seconds = floor(Seconds*1.0e6+0.5)/1.0e6; //round for lost precision on LSD
  const long lHSecs = (long)floor(((Seconds-floor(Seconds))+0.0005)*1000.0);
  const long lSecs = ((long)Seconds)%60;
  const long lMin = (((long)(Seconds))/60)%60;
  const long lHours = ((long)Seconds)/3600;
  const long lDays = ((long)Seconds)/86400;
  if (lDays>0)
    {
    s.Format("%ddays %d:%02d:%02d", lDays, lHours%24, lMin, lSecs);
    }
  else
    s.Format("%d:%02d:%02d.%03d", lHours, lMin, lSecs, lHSecs);
  return s;
  }

//===========================================================================

CGlblStopWatch::CGlblStopWatch(CGlblStopWatchList & List, LPCTSTR Group, LPCTSTR Name, int Order, int Show) : 
  m_List(List), 
    m_sGroup(Group), 
    m_sName(Name), 
    m_iOrder(Order),
    m_iShow(Show)
  {
  //m_sName+="Tm";
  m_sFullName=m_sGroup;
  m_sFullName+=".";
  m_sFullName+=m_sName;
  m_dTimeMem=0;

  if (m_List.GetSize()==0)
    m_List.SetSize(0, 128);

  for (int i=0; i<m_List.GetSize(); i++)
    {
    int iCmp=m_List[i]->m_sGroup.CompareNoCase(m_sGroup);
    if (iCmp>0)
      break;
    else if (iCmp==0)
      {
      if (m_List[i]->m_iOrder==m_iOrder)
        {
        if (m_List[i]->m_sName.CompareNoCase(m_sName)>0)
          break;
        }
      else if (m_List[i]->m_iOrder>m_iOrder)
        break;
      }
    }
  if (i<m_List.GetCount())
    m_List.InsertAt(i,this);
  else
    m_List.Add(this);
  }; 

CGlblStopWatch::~CGlblStopWatch()
  {
  for (int i=0; i<m_List.GetSize(); i++)
    if (m_List[i]==this)
      break;
  if (i<m_List.GetSize())
    m_List.RemoveAt(i);

  }; 

double CGlblStopWatch::Time()            { return Secs(); };
double CGlblStopWatch::TotalTime()       { return Secs(); };
double CGlblStopWatch::TimeFrac()        { return Secs()/m_List.StepTime(); };
double CGlblStopWatch::TotalTimeFrac()   { return Secs(); };

//===========================================================================

CGlblStopWatchList::CGlblStopWatchList()
  {
  m_dStepTime=1e6;
  }

bool CGlblStopWatchList::GroupIndices(LPCTSTR Group, int &First, int &Last)
  {
  for (int i=0; i<GetSize(); i++)
    if (_stricmp(GetAt(i)->Group(), Group)==0)
      {
      for (int j=i+1; j<GetSize(); j++)
        if (_stricmp(GetAt(j)->Group(), Group)!=0)
          break;
      First=i;
      Last=j-1;
      return true;
      }
  First=-1;
  Last=-1;
  return false;
  };

double CGlblStopWatchList::Time(int i)      { return GetAt(i)->m_dTimeMem;};
double CGlblStopWatchList::TotalTime(int i) 
  { 
  CGlblStopWatch &SW=*GetAt(i);
  double T=SW.m_dTimeMem;
  for (int j=i-1; j>=0; j--)
    {
    CGlblStopWatch &SWJ=*GetAt(j);
    if (_stricmp(SW.m_sGroup, SWJ.m_sGroup)!=0)
      break;
    T+=SWJ.m_dTimeMem;
    }
  for (int j=i+1; j<GetSize(); j++)
    {
    CGlblStopWatch &SWJ=*GetAt(j);
    if (_stricmp(SW.m_sGroup, SWJ.m_sGroup)!=0)
      break;
    T+=SWJ.m_dTimeMem;
    }
  return T;
  };

double CGlblStopWatchList::TimeFrac(int i)      { return GetAt(i)->m_dTimeMem/m_dStepTime;};
double CGlblStopWatchList::TotalTimeFrac(int i) { return TotalTime(i)/m_dStepTime;};

void CGlblStopWatchList::Start()
  {
  for (int i=0; i<GetSize(); i++)
    GetAt(i)->Clear();
  m_swStep.Clear(); 
  m_swStep.Start(); 
  };
void CGlblStopWatchList::Stop()
  {
  m_swStep.Stop(); 
  m_dStepTime=m_swStep.Secs();
  for (int i=0; i<GetSize(); i++)
    GetAt(i)->m_dTimeMem=GetAt(i)->Secs();
  }

CGlblStopWatchList gs_swFlwSolve;
CGlblStopWatchList gs_swExecute;

//===========================================================================

LPCTSTR DbgFltString(double V, int eSig, int fSig, int RqdLen) // only one call per pline
  {
  dbglock();

  static CString Ss[20];
  static int iStrng=0;
  CString &S=Ss[iStrng];
  iStrng=(iStrng+1)%20;
  CString *pS=&S;

  if (_finite(V)==0)
    S="*";
  else if (V==0)
    S.Format("%s","0");
  else 
    {
    double LargeTol=RqdLen>0?pow(10.0, RqdLen-(fSig+3)):1e10;
    if (eSig>0 && (fabs(V)>LargeTol || fabs(V)<1e-3))
      S.Format("%.*e",eSig,V);
    else
      S.Format("%.*f",fSig,V);
    }

  int Len=S.GetLength();
  if (Len<RqdLen)
    {
    pS=&Ss[iStrng];
    iStrng=(iStrng+1)%20;
    pS->Format("%*s", RqdLen, S);
    }

  dbgunlock();
  return *pS;
  }

//===========================================================================

inline double GetProcessorTime()
  {
  FILETIME tCreat, tExit, tKernel, tUser;
  GetProcessTimes(GetCurrentProcess(), &tCreat, &tExit, &tKernel, &tUser);
  LARGE_INTEGER lKernel = {tKernel.dwLowDateTime, tKernel.dwHighDateTime };
  LARGE_INTEGER lUser   = {tUser.dwLowDateTime, tUser.dwHighDateTime };

  return (lKernel.QuadPart+lUser.QuadPart)*1.0e-7; // 100's of nanosecs ticks
  }

//===========================================================================

#define AUTOOPENDEBUG 0
#if AUTOOPENDEBUG  

static bool FileExists(LPCSTR FileName, WIN32_FIND_DATA & fd)
  {
  HANDLE H;
  if ((H=FindFirstFile(FileName, &fd))!=INVALID_HANDLE_VALUE)
    {
    FindClose(H);
    return true;
    }
  return false;
  };

class CXXXX
  {
  public:
  CXXXX()
    {
    WIN32_FIND_DATA fd;
    CString Fn;
    for (int i=0; i<100; i++)
      {
      Fn.Format("d:\\temp\\dbg%03i.txt", i);
      if (!FileExists(Fn, fd))
        break;
      }
    
    dbgfileopen(Fn);
    };
  ~CXXXX()
    {
    dbgfileclose();
    };

  };

CXXXX xxxx;
#endif