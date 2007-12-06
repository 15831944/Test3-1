// DebugLib.h: interface for the DebugFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEBUGLIB_H__6780B3E2_46E4_43F1_AB44_10E816489653__INCLUDED_)
#define AFX_DEBUGLIB_H__6780B3E2_46E4_43F1_AB44_10E816489653__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#include <stdio.h>

#ifdef __DEBUGLIB_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SCDIF)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//=========================================================================

#define MAX_DEBUG_LEVEL 9

const int DEBUGARRAYLEN = 250;

DllImportExport HANDLE dbgfile();
DllImportExport char *dbgfilename();
DllImportExport bool dbgtestfileopen();
DllImportExport void dbglock();
DllImportExport void dbgunlock();
DllImportExport void dbgtime(int Opt);
DllImportExport void dbgpln(char * fmt, ...);
DllImportExport void dbgp(char * fmt, ...);
DllImportExport void dbgnln();
DllImportExport bool dbgfileopen(LPCTSTR Fn);
DllImportExport void dbgfileclose(void);
DllImportExport void dbgfiletrunc(long size=0);
DllImportExport void dbgfileflush(bool On);
DllImportExport void dbgindent(int chg, LPCTSTR where=NULL);
DllImportExport int  dbgindentcnt();
//DllImportExport void Dbg_Entry();
//DllImportExport void Dbg_Exit();
DllImportExport LPCTSTR DbgFltString(double V, int eSig=3, int fSig=6, int RqdLen=0);


#define DBGATON          1
#define DBGATLEN         80
#define DBGATLISTLEN     256

DllImportExport void dbgMemoryState(LPCTSTR Where);
DllImportExport void dbgTimeStamp(LPCTSTR Where);

#define  DBG_NEW(s)


//=========================================================================

class DllImportExport CStopWatch
  {
  protected:
    LARGE_INTEGER Freq;
    bool bRunning;
    DWORD dwLaps;
    double dCurrent;
    double dAccum;
    double dLapTime;

  public:
    double       Time()      { if (Freq.QuadPart==0) return 0; LARGE_INTEGER Tmp; QueryPerformanceCounter(&Tmp); return ((double)(Tmp.QuadPart))/Freq.QuadPart; }

    void         Start()     { double Tm=Time(); dLapTime=-Tm; dCurrent -= Tm; dwLaps++; bRunning=1; };
    void         Stop()      { double Tm=Time(); dLapTime+=Tm; dCurrent += Tm; bRunning=0; };
    double       Secs()      { return dCurrent + (bRunning ? Time() : 0.0); };
    double       LapTime()   { return bRunning ? dLapTime+Time() : dLapTime; };
    void         ReStart()   { dAccum=0.0; Start(); };
    double       Lap()       { double d1=Secs(); double d2=d1-dAccum; dAccum=d1; dwLaps++; return d2; };
    void         Clear()     { dCurrent=dAccum=dLapTime=0.0; dwLaps=0; bRunning=0; };
    DWORD        Laps()      { return dwLaps; };
    bool         Running()   { return bRunning; };

    static LPCTSTR MinSecDesc(double Seconds, CString & s);
    static LPCTSTR MinSecHunDesc(double Seconds, CString & s);
    static LPCTSTR HourMinSecHunDesc(double Seconds, CString & s);
    static LPCTSTR DayHourMinSecHunDesc(double Seconds, CString & s);
    LPCTSTR      MinSecDesc(CString & s) { return MinSecDesc(Secs(), s); };
    LPCTSTR      MinSecHunDesc(CString & s) { return MinSecHunDesc(Secs(), s); };
    LPCTSTR      HourMinSecHunDesc(CString & s) { return HourMinSecHunDesc(Secs(), s); };
    LPCTSTR      DayHourMinSecHunDesc(CString & s) { return DayHourMinSecHunDesc(Secs(), s); };

    CStopWatch() { QueryPerformanceFrequency(&Freq); Clear(); }
    CStopWatch(bool DoStart) { QueryPerformanceFrequency(&Freq); Clear(); if (DoStart) Start(); }
  };

//=========================================================================

class DllImportExport CStopWatchLap
  {
  protected:
    CStopWatch & m_rSW;

  public:
    CStopWatchLap(CStopWatch & SW):
      m_rSW(SW)
      {
      m_rSW.Start();
      };
    ~CStopWatchLap()
      {
      m_rSW.Stop();
      }
  };

// ==========================================================================

class CGlblStopWatchList;
class DllImportExport CGlblStopWatch : public CStopWatch
  {
  friend class CGlblStopWatchList;
  public:
    enum eShowWhat { eShowDbg=1, eShowAccess=2, eShowTotal=4};

    CGlblStopWatch(CGlblStopWatchList & List, LPCTSTR Group, LPCTSTR Name, int Order=0, int Show=(eShowDbg|eShowAccess)); 
    ~CGlblStopWatch(); 

    LPCTSTR         Group()                   { return m_sGroup; }; 
    LPCTSTR         Name()                    { return m_sName; }; 
    LPCTSTR         FullName()                { return m_sFullName; }; 
    int             Show()                    { return m_iShow; };

    double          Time();
    double          TotalTime();
    double          TimeFrac();
    double          TotalTimeFrac();

  protected:
    CGlblStopWatchList &m_List;
    CString         m_sGroup;
    CString         m_sName;
    CString         m_sFullName;
    int             m_iOrder; 
    int             m_iShow; 
    double          m_dTimeMem;
  };

class DllImportExport CGlblStopWatchList : public CArray<CGlblStopWatch*, CGlblStopWatch*> 
  {
  public:
    CGlblStopWatchList(); 

    bool            GroupIndices(LPCTSTR Group, int &First, int &Last);

    void            Start();
    void            Stop();

    double          StepTime()                { return m_dStepTime; };

    double          Time(int i);
    double          TotalTime(int i);
    double          TimeFrac(int i);
    double          TotalTimeFrac(int i);

  protected:
    double          m_dStepTime;
    CStopWatch      m_swStep;
  };

extern DllImportExport CGlblStopWatchList gs_swFlwSolve;
extern DllImportExport CGlblStopWatchList gs_swExecute;

//===========================================================================

extern DllImportExport double GetProcessorTime();

// ==========================================================================

#undef DllImportExport

// ==========================================================================

#endif // !defined(AFX_DEBUGFILE_H__6780B3E2_46E4_43F1_AB44_10E816489653__INCLUDED_)
