//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __PROFILER_H
#define  __PROFILER_H

#ifndef __VECTORS_H
#include "vectors.h"
#endif
#ifndef __PGM_E_H
#include "pgm_e.h"
#endif

#ifdef __PROFILER_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

class CProfileInfo
  {
  public:
    DWORD  iLen;            //number of points in profile
    byte   bLinear:1,       //must linear interpulation be applied between points
           bWrapArround:1,  //must the profile wrap arround after full time
           bStarted:1,      //has the profile been started
           bPaused:1,       //has beed paused (hold on current value)
           bJustLoaded:1;   //used to resynchronise time after a load
    byte   iPulseSeq;       //sequence defining a pulse
    WORD   iCurIndex;       //current index in array
    WORD   iPrvIndex;       //previous index in array
    CTimeValue dStartTime;      //start time of profile
    CTimeValue dLastTime;       //last time GetVal was called
    double dPausedVal;      //value at which profile was paused
    double dFullArea;       //Area for full profile
    double dPrevVal;        //value at previous output
    Strng  sColTagName;     //

    CProfileInfo();
  };

_FWDDEF(CSimpleProfiler)
class DllImportExport CSimpleProfiler
  {
  friend class GCProfiler;
  friend class CProfileCon;
  friend class CProfileCon2;
  protected:
    CDMatrix      Data;      //data containing the time in the first column and the profiles in the next columns
    flag          bLoaded;   //has the profile file been loaded successfully
    CProfileInfo* pInfo;     //pointer to array of classes with info about each profile
  public:
    CSimpleProfiler();
    ~CSimpleProfiler();
    int Load(char* Filename, flag LoadHeadings);
    void Options(flag Linear, flag WrapArround, byte Column = 1);
    void SetAllOptions(flag Linear, flag WrapArround);
    double Start(CTimeValue Time, byte Column = 1);
    void StartAll(CTimeValue Time);
    double GetVal(CTimeValue Time, byte Column = 1);
    double GetAveVal(CTimeValue Time, byte Column = 1);
    double Pause(CTimeValue Time, byte Column = 1);
    double Continue(CTimeValue Time, byte Column = 1);
//    void Copy(pCSimpleProfiler p);
    inline flag Loaded() { return bLoaded; };
    inline flag Linear(byte Column = 1) { if (bLoaded && Column<Data.GetCols()) return (flag)(pInfo[Column].bLinear); return False; };
    inline flag WrapArround(byte Column = 1) { if (bLoaded && Column<Data.GetCols()) return (flag)(pInfo[Column].bWrapArround); return True; };
    inline flag Started(byte Column = 1) { if (bLoaded && Column<Data.GetCols()) return (flag)(pInfo[Column].bStarted); return False; };
    inline flag Paused(byte Column = 1) { if (bLoaded && Column<Data.GetCols()) return (flag)(pInfo[Column].bPaused); return False; };
    inline CTimeValue StartTime(byte Column = 1) { if (bLoaded && Column<Data.GetCols()) return (pInfo[Column].dStartTime); return 0.0; };
    inline CTimeValue LastTime(byte Column = 1) { if (bLoaded && Column<Data.GetCols()) return (pInfo[Column].dLastTime); return 0.0; };
    inline double PausedVal(byte Column = 1) { if (bLoaded && Column<Data.GetCols()) return (pInfo[Column].dPausedVal); return 0.0; };
    inline byte ColCnt() { if (bLoaded) return (byte)Data.GetCols(); return 0; };
    inline long RowCnt(byte Column = 1) { if (bLoaded && Column<Data.GetCols()) return pInfo[Column].iLen; return 0; };
    inline double FullTime(byte Column = 1) { if (bLoaded && Column<Data.GetCols()) return Data[pInfo[Column].iLen-1][0]; return 0.0; };
    inline CTimeValue TimePassed(byte Column = 1) { if (bLoaded && Column<Data.GetCols() && pInfo[Column].bStarted) return pInfo[Column].dLastTime - pInfo[Column].dStartTime; return 0.0; };
    inline long CurIndex(byte Column = 1) { if (bLoaded && Column<Data.GetCols()) return (long)(pInfo[Column].iCurIndex); return 0; };
  private:
    void JustLoaded(CTimeValue Time, byte Column);
    double CalcArea(CTimeValue T1, CTimeValue T2, byte Column);
  };

//===========================================================================

class GCProfiler : public GCClassVar
  {
  public:
    Strng         sFilename; //filename of profile
    GCProfiler(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB, pvoid pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB, pvoid pSubClass);
//    virtual void OnRestore(pvoid pOldSubClass, pvoid pNewSubClass);
  };

//===========================================================================

class GCRandomProfiler : public GCClassVar
  {
  public:
    GCRandomProfiler(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB, pvoid pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB, pvoid pSubClass);
//    virtual void OnRestore(pvoid pOldSubClass, pvoid pNewSubClass);
  };

//===========================================================================

/*_FWDDEF(CBaseProfiler)
_FWDDEF(CProfiler)
//_FWDDEF(CPGMProfiler)

class DllImportExport CBaseProfiler
  {
  protected:
    XYFn  Fn;
    double  &dFullTime;
  public:
    CBaseProfiler(pCProfiler pProf);
    //CBaseProfiler(pGCClassVar pClassVar);
    ~CBaseProfiler();
    //int Configure(double FullTime, pchar Filename, UCHAR Column);
    //flag Configure(long FullTime, pchar Model);
    //double Start(double Time, double StartAt = 0);
    //double GetVal(double Time);
  };

//===========================================================================

class DllImportExport CProfiler : public CBaseProfiler
  {
  friend class CBaseProfiler;
  protected:
    double FullTime;
  public:
    CProfiler();
  };
*/
//===========================================================================
#undef DllImportExport

#endif
