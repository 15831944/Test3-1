//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __HSTMAIN_H
#define  __HSTMAIN_H

#ifndef ForSysCAD
#define ForSysCAD 1
#endif

#if ForSysCAD
#ifndef __SC_DEFS_H
#include "sc_defs.h"
#endif
#ifndef __EXECUTIV_H
#include "executiv.h"
#endif
#ifndef __VECTORS_H
#include "vectors.h"
#endif
#endif

#ifndef __HSTMSG_H
#include "hstmsg.h"
#endif
#ifndef __HSTSLOTS_H
#include "hstslots.h"
#endif
#ifndef __HSTCAT_H
#include "hstcat.h"
#endif
#ifndef __HSTBLKS_H
#include "hstblks.h"
#endif
#ifndef __HSTQUERY_H
#include "hstquery.h"
#endif

#ifdef __HSTMAIN_CPP
  #define DllImportExport DllExport
#elif !defined(SCHIST)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================

const byte MaxQueryThreads = 3;//2;

//extern DllImportExport CHistorian* pHistory; //global pointer to the historian
extern CHistorian* pHistory; //global pointer to the historian

//=========================================================================

class CQThreadInfo
  {
  DEFINE_MEMSTATS(CQThreadInfo)
  public :
    pCHistorian    pHst;
    int            ThreadNo;
    pCQueryHist    pQuery;
  };

//---------------------------------------------------------------------------

typedef CMap <char*, char*, long, long> CSlotMap;
typedef CMap <DWORD, DWORD, long, long> CSlotIDMap;


#if ForSysCAD
//class DllImportExport CHistorian : public CExecObj
class CHistorian : public CExecObj
#else
class DllImportExport CHistorian
#endif
  {
  DEFINE_MEMSTATS(CHistorian)

  //friend class CHistView;
  friend class CEvBlk;
  friend class CSlot;
  friend class CFilter;
  friend class CBoxcar;
  friend class CHistCat;
  friend class CCatItem;
  friend class CQuerySlice;
  friend class CQueryBlk;
  friend class CQueryHist;
  friend class CHstSlotDlg;
  friend class CHstSlotInfoDlg;

  //protected:
  public:
    HD_Hist          Data;
    CString          sPath;           //file path for historian (.hed and .cat)
    CString          sDataPath;       //file path for historian event data files
    CString          sCatPath;        //file path for historian cataloged data files
    CString          sName;           //file name of historian (excluding extension)
    long             lSlotsAlloc;     //number of slots allocated in array
    CSlot **         Slots;           //pointer to array of historian slots
    CSlotMap         SlotMap;         //map of tags and index to array of slots
    CSlotIDMap       SlotIDMap;       //map of tag IDs and index to array of slots
    CEvBlk*          pCurEvBlk;       //pointer to current event block
    flag             bJustStarted;    //set to True for the first iteration after starting
    double           dTime;           //iteration current time
    double           dPrevTime;       //time of preveous iteration
    double           dMidTime;        //time between prev and cur time
    double           dTimeRunStarted; //time at which the run was started
    double           dRecordATol;     //Abs tolerance to decide to record data
    double           dRecordRTol;     //Relative tolerance to decide to record data
    double           m_StopGap;
    double           m_MidTimeFactor; //factor of timestep to use for time when recording previous unchanged value
    CHistCat         Cat;             //historian catalog
    long             lScanNo;         //counter incremented each iteration
    CLVector         EvSlotNos;       //list of slots that must be saved at end of iteration
    CLVector         FltSlotNos;      //list of slots that are filtered
    CLVector         RecSlotNos;      //list of slots that must be recorded next iteration
    long             lEvSlotNoCnt;    //number of EvSlotNos used
    flag             bDoneConfig:1,   //has configure been called
                     bOpen:1,         //has open been successfully called and is it still open
                     bQueryClose:1,   //must the query threads be closed
                     bDoSaveAll:1,    //must all values be saved next iteration
                     bDoneWrite:1,    //True if done a write this iteration
                     bSwitchedOff:1,  //True if historian switches itself off when there is no disk space
                     bShowQueryInfo:1,//True if little query info window must be displayed
                     bUseTolerance:1, //True if using tolerance test before deciding to save
                     m_bWasPaused:1; // Set if any value written during pause
    CRITICAL_SECTION QuerySection;    //
    HANDLE           hQuerySignal[MaxQueryThreads];   //
    flag             bThreadIsBusy[MaxQueryThreads];  //
    CQThreadInfo     QThreadInfo[MaxQueryThreads];    //
    CEvBlkFileInfo*  pNxtBlkFile;     //pointer to memory mapped file waiting to be used
    CQueryHist*      pQueryList;      //pointer to linked list of queries
    long             lBlkWaitTime;    //time after which an unused block can be closed
    long             lForceCnt;       //number of iterations after which unchanged values should be forced to disk
    UINT             iFilesMin;       //min no of historian files to keep
    UINT             iFilesMax;       //max no of historian files to keep, after which files are deleted
    static DWORD     dwGranularity;   //memory granularity obtained from SYSTEM_INFO
    static DWORD     dwDoubleGran;    //memory granularity obtained from SYSTEM_INFO
    DWORD            dwStatsCnt;
  public:
    CHistorian();
    ~CHistorian();
    void Configure(char* Path,
                   char* DataPath,
                   char* CatPath,
                   char* Name,
                   long LicMaxHistSizeAllowed,
                   UINT LicMaxHistFilesAllowed,
                   UINT FilesMin = 5,
                   UINT FilesMax = 5,
                   long BlkWaitTime = 300,
                   long ForceCnt = 1000,
                   flag ShowQueryInfo = 1,
                   flag FixedDataExtn = 1,
                   flag UseTol = 0,
                   double ATol = 0.001,
                   double RTol = 0.001,
                   double StopGap = 0.001,
                   double MidTimeFactor = 0.98333333333333333333);//=59/60
    flag Create();
    byte Open(char* ScenarioName,
              long ReqdBlkSize,
              long EstimatedMaxSlots,
              long LicMaxHistSizeAllowed,
              bool ShowBusyDlg = false);
    flag Restart(char* ScenarioName);
    void DeleteDataFiles();
    void DeleteHistorian();
    flag Close();
    flag ChangeScenario();
    long CfgSlot(pSlotCfgInfo pSCI);
    void ServiceHistoryQueries(byte ThreadNo);

    void ResetStats();
    void FetchStats(char* Filename);
    flag SlotInfoDlg(char* pTag = "", byte iDlgNo = 0);

#if ForSysCAD
  private:
    // ExecObj Overrides
    virtual DWORD  EO_Message(CXMsgLst &XM, CXM_Route &Route);
    virtual flag   EO_SetTime(double TimeRqd, bool Rewind);
    virtual flag   EO_QueryTime(const CXM_TimeControl &CB, double &TimeRqd, double &dTimeRqd);
    virtual flag   EO_Start(const CXM_TimeControl &CB);
    virtual void   EO_QuerySubsReqd(CXMsgLst &XM);
    virtual void   EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    virtual flag   EO_ReadSubsData(CXMsgLst &XM);
    virtual flag   EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);
    virtual flag   EO_Execute(const CXM_TimeControl &CB, CEOExecReturn &EORet);
    virtual flag   EO_BeginPause(const CXM_TimeControl &CB);
    virtual flag   EO_EndPause(const CXM_TimeControl &CB);
    virtual flag   EO_Stop(const CXM_TimeControl &CB);
    virtual flag   EO_BeginSave(FilingControlBlock &FCB);
    virtual flag   EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info);
    virtual int    EO_QueryChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    EO_ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    EO_CanClose(Strng_List & Problems);
#else
  private:
    DWORD XSendMessage(CXMsgLst &XM, CXM_Route &Route);
#endif

  protected:
    void Initialise();
    void ClearDataLists();
    byte Load(bool ShowBusyDlg);
    flag Save(long SlotNo = -1);
    inline void Lock() { EnterCriticalSection(&QuerySection); };
    inline void Release() { LeaveCriticalSection(&QuerySection); };
    byte AddEvBlk();
    byte OpenEvBlk();
    void CloseEvBlk();
    flag CreateEventBlk(pCEvBlkFileInfo pI);
    WORD FindLastCatNo();
    void Recover(WORD MaxCatNo, BOOL CheckAgainstCat);
    void DeleteExtra();
    void CopyFiles();
    flag RemoveSlot(long SNo);
    long AddSlot(DWORD SlotID);
    long GetSlot(char* Tag);
    pCSlot GetSlot(DWORD ID);
    void FixFilterList();
    void WriteSlots(flag MustDoSaveAll);
    void CancelQueries();
    void CancelAllQueries();

    inline void AddEv(long SNo)
      { EvSlotNos[lEvSlotNoCnt++] = SNo; };
    inline pCSlot GetSlotByIndex(long index)
      { return Slots[index]; };

  public:
    inline double GetLastTime()
      { ASSERT(pCurEvBlk); return pCurEvBlk->GetLastTime(); };
  };

//-------------------------------------------------------------------------

#if ForSysCAD
class DllImportExport CHistorianMngr
  {
  public:
    CHistorianMngr();

    void CreateTheHistEO();
    void DestroyTheHistEO();

    void LoadSettings(CProfINIFile & PF);
    void SaveSettings(CProfINIFile & PF);
    void SetLicensingOptions(long LicMaxHistSizeAllowed, UINT LicMaxHistFilesAllowed);
    bool DoCfgHistorian();
    bool DoOpenHistorian(bool ShowBusyDlg=false);
    flag DoClose();
    bool DoRestart();
    void DoDeleteDataFiles();
    bool DoDeleteHistorian();
    void DoFetchStats(char* Filename);
    void DoResetStats();

    bool KeepHistoryFile(char* pFilename);
    void BrowseTags();
    void QueryToFile(char* pFile, double StartTime, double Duration, double Interval);
    DWORD HistoryExists(char* pTag);

    bool LicenseExceeded(long LicMaxHistSizeAllowed, UINT LicMaxHistFilesAllowed);
    double GetLastTime();
    bool IsOn();
    bool IsOpen();
    void SetExecObj(CExecObj* pXObj) { pExecObj=pXObj; };
    flag HstOn() { return bHstOn; };
    void SetHstOn(flag On) { bHstOn=On; };
    flag HstOpen() { return bHstOpen; };
    void SetHstOpen(flag Open) { bHstOpen=Open; };

  public:
    Strng          sHistorianName;     //historian name
    Strng          sHstDataDir;        //directory for historian event block data files
    Strng          sHstCatDir;         //directory where catalog items may be found
    Strng          sHstScenName;       //current scenario name
    long           lHstEvFileSize;     //file size of historian event blocks (kbytes)
    long           lHstSlotNoEstimate; //estimate of number of slots to be recorded by the historian
    UINT           iHstFilesMin;       //min no of historian files to keep
    UINT           iHstFilesMax;       //max no of historian files to keep, after which files are deleted
    long           lHstForceCnt;       //no iterations after which an unchanged value will be stored
    double         dHstRecordATol;     //Abs tolerance to decide to record data
    double         dHstRecordRTol;     //Relative tolerance to decide to record data
    double         dHstStopGap;        //Gap left in historian during stop start
    double         dHstMidTimeFactor;  //factor of timestep to use for time when recording previous unchanged value
    flag           bHstShowQueryInfo:1,    //True if little query info window must be displayed
                   bHstFixedDataExtn:1,    //
                   bHstUseTol:1;           //true if using tolerance test before deciding to save value to historian

  protected:
    CExecObj*     pExecObj;
    byte          bHstOn:1,
                  bHstOpen:1;
    long          lLicMaxHistSizeAllowed;
    UINT          lLicMaxHistFilesAllowed;
  };

extern DllImportExport CHistorianMngr HstMngr; //the global class for historian access

#endif

//-------------------------------------------------------------------------

#undef DllImportExport

#endif


