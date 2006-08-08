//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef __EXECLIB_H
#define __EXECLIB_H

#ifndef __TAGOBJ_H
#include "tagobj.h"
#endif
#ifndef __EQNSOLVE_H
#include "eqnsolve.h"
#endif
#include "..\..\Com\ScdIF\scdcomevts.h"


#ifdef __EXECLIB_CPP
#define DllImportExport DllExport
#elif !defined(SCEXEC)
#define DllImportExport DllImport
#else
#define DllImportExport
#endif

//=========================================================================

typedef char DQTag[MaxTABTagLen];

enum XIOAction { XIO_Null, XIO_In, XIO_Out, XIO_InOut };

//const char * ActionStrings[] = { "", "I", "O", "IO"};

inline XIOAction EO_ReverseAction(XIOAction A) {return (XIOAction)((A & XIO_In ? XIO_Out : 0 )|(A & XIO_Out ? XIO_In : 0)) ;};

//=========================================================================

const byte XM_Null              = 0;
const byte XM_ObjectTag         = 1;
const byte XM_ObjectData        = 2;
const byte XM_Route             = 3;
const byte XM_ReadIndexedData   = 4;
const byte XM_DataRequest       = 5;
const byte XM_DataAvailable     = 6;
const byte XM_TagNotAvail       = 7;
const byte XM_TimeControl       = 8;
const byte XM_QueryTime         = 9;
const byte XM_Execute           = 10;

const byte XM_HistoryExists     = 11;  // historian messages
const byte XM_KeepHistory       = 12;
const byte XM_QueryRow          = 13;
const byte XM_QueryHistory      = 14;
const byte XM_HistorySlotDlg    = 15;
const byte XM_HistoryData       = 16;
const byte XM_HistoryDataError  = 17;
const byte XM_QueryHistoryOther = 18;
const byte XM_QueryString       = 19;
const byte XM_KeepHistoryFile   = 37;
const byte XM_QueryRowEx        = 38;
const byte XM_HistRecordingOn   = 39;

const byte XM_Long              = 20; // generic messages
const byte XM_Boolean           = 21;
const byte XM_Double            = 22;
const byte XM_String            = 23;

#if WITHDDEREPORTS
const byte XM_DDEReport         = 30; // DDE messages
const byte XM_DDEErrorCode      = 31;
#endif

const byte XM_DrvShowTagInfo    = 35; // Driver messages

const byte XM_OleExcelReport    = 40; // OLE messages
const byte XM_OleErrorCode      = 41;

const byte XM_RepTrendDB        = 45; // Report messages

const byte XM_ArchiveExists     = 50; // Archiver messages
const byte XM_KeepArchive       = 51;
const byte XM_ArcShowTagInfo    = 52;

//=========================================================================
//
//  NOW USE COMMON COMMAND IN ScdCOMCallBack.h
//
//=========================================================================
// All the Packed structures must have packing of 1 byte
#pragma pack(push, 1)
//=========================================================================

class DllImportExport CXM_Header
  {
  public:
    byte          Id;
    dword         m_dwLength;

    CXM_Header(byte MsgId=XM_Null);

    CXM_Header(const CXM_Header& Cpy);
    virtual ~CXM_Header();
    CXM_Header& operator=(const CXM_Header& Cpy);

    dword          Length() const { return m_dwLength; };
  };


//#define DEFINE_SPARES(Obj)                                  \
//  protected:                                                 \
//    static SparesList QQSparesLst;                          \
//    void * operator new(size_t size)                        \
//      { return QQSparesLst.DoNew(size);};                   \
//    void * operator new(size_t size, char * File, int Line) \
//      { return QQSparesLst.DoNew(size, File, Line);};       \
//  public:                                                   \
//    void operator delete(void *p)                           \
//      { QQSparesLst.DoDelete(p); };                         \
//  private:
//
//#define IMPLEMENT_SPARES(Obj, Max)  SparesList Obj::QQSparesLst(#Obj, Max, sizeof(Obj));

//-------------------------------------------------------------------------

class DllImportExport CXM_DataRequest : public CXM_Header
  {
  public:
    long           Index;
    TABOptions     Options;
    XIOAction      Action;
    char           Tag[MaxTABTagLen];

    CXM_DataRequest(long Inx, char * pTag, TABOptions Opts, XIOAction Act);

    void           Set(long Inx, char * pTag, TABOptions Opts, XIOAction Act);

    DEFINE_SPARES(CXM_DataRequest)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_DataAvailable : public CXM_Header
  {
  public:
    long           Index;
    XIOAction      Action;
    char           Tag[MaxTABTagLen];

    CXM_DataAvailable(long Inx, char * pTag, XIOAction Act);

    DEFINE_SPARES(CXM_DataAvailable)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_TagNotAvail : public CXM_Header
  {
  public:
    long           Index;
    char           Tag[MaxTABTagLen];

    CXM_TagNotAvail(long Inx, char * pTag);

    void           Set(long Inx, char * pTag);

    DEFINE_SPARES(CXM_TagNotAvail)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_ObjectTag : public CXM_Header
  {
  public:
    TABOptions     Options;
    char           Tag[MaxTABTagLen];

    CXM_ObjectTag(char * pTag, TABOptions Opt);

    DEFINE_SPARES(CXM_ObjectTag)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_ObjectData : public CXM_Header
  {
  public:
    long           Index;
    dword          dwOptions;
    CPkDataList    m_List;

    CXM_ObjectData(long Inx=0, dword Options=0);
    CXM_ObjectData(long Inx, dword Options, CPkDataItem * pItem);
    CXM_ObjectData(long Inx, dword Options, char *pTag, PkDataUnion &Data);
    CXM_ObjectData(long Inx, dword Options, char *pTag, DDEF_Flags iFlags, PkDataUnion &Data);
    virtual ~CXM_ObjectData();
    //void          SetSize();

    // Helper Methods
    CPkDataItem   * FirstItem() { return m_List.FirstItem(); };
    CPkDataItem   * FirstItem(CPkDataIter &Iter) { return m_List.FirstItem(Iter); };
    CPkDataItem   * NextItem(CPkDataIter &Iter) { return m_List.NextItem(Iter); };
    CPkDataItem   * CurrentItem(CPkDataIter & Iter) { return m_List.CurrentItem(Iter); };

    CPkDataList & getList() { return m_List; };
    __declspec(property(get=getList)) CPkDataList & List; 

    void          Clear() 
      { 
      List.Clear(); 
      //SetSize(); 
      }
    DEFINE_SPARES(CXM_ObjectData)

#if _DEBUG
    long           m_AllocNo;
    static long    m_nAllocs;
    static long    m_LRange1;
    static long    m_LRange2;
#endif
  };

//-------------------------------------------------------------------------

const int CXM_RouteBuffMax=1024;
typedef char NodeNameBuff[MAX_COMPUTERNAME_LENGTH + 1 + 100];

class DllImportExport CXM_Route : public CXM_Header
  {
  private:
    word           wEnd;
    flag           bAcrossNetwork;
    //word           wCurrentPos;
    byte           nNodes;
    char           cBuffer[CXM_RouteBuffMax];

  public:
    CXM_Route();
    CXM_Route(const CXM_Route & X);
    virtual ~CXM_Route();

    void           Clear();
    flag           AddNodeFwd(long NodeObjId, char* NodeName);
    flag           AddNodeRev(long NodeObjId, char* NodeName);
    flag           RemoveNode(int n);
    long           NodeObjId(int n);
    char*          NodeName(int n);
    long           NoNodes();
    //flag           SameRoute(CXM_Route &Other);
    flag           operator==(CXM_Route& Other);
    CXM_Route&      operator=(CXM_Route& Other);

    flag           AcrossNetwork() { return bAcrossNetwork; };
    void           SetAcrossNetwork(flag On) { bAcrossNetwork=On; };

    void           ReverseRoute(CXM_Route &Other);
    static char*   ComputerName();
    static char*   MakeNodeName(char * Node, char * Where);
    void           dbgDump(char *Where="");

    DEFINE_SPARES(CXM_Route)
  };

// --------------------------------------------------------------------------

class DllImportExport CXM_ReadIndexedData : public CXM_Header
  {
  public:
    flag           Start;
    flag           ReadAll;
    long           LastIndex;

    CXM_ReadIndexedData(flag Strt, flag RdAll, long LastInx);

    DEFINE_SPARES(CXM_ReadIndexedData)
  };

//-------------------------------------------------------------------------

//temporary should be bigger
const long MaxCXMsgLstSize=10000;//2048;//sizeof(CXM_ObjectData);

//-------------------------------------------------------------------------
//
//         Historian Messages
//
//-------------------------------------------------------------------------

class DllImportExport CXM_HistoryExists : public CXM_Header
  {
  public:
    char          cTag[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/];

    CXM_HistoryExists(long Index, char * Tag);
    CXM_HistoryExists();//(char * Tag);
    flag          xAddTag(long Index, char * pNewTag);//, char * pPosition);
    char        * FirstTag();
    char        * NextTag(char * pPrevTag);
    long          GetIndex(char * pTag);
    void          SetFlags(char * pTag, dword Flags);
    dword         GetFlags(char * pTag);
    flag          Empty();

    DEFINE_SPARES(CXM_HistoryExists)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_KeepHistory : public CXM_Header
  {
  public:
    //boxcar/data compression...
    double         dDeltaLo, dDeltaHi, dChange;
    int            iNoRec, iNoNotRec, iBoxcarAlg;
    //filter...
    double         dFltTau, dWinFltPeriod, dFltDelta;
    int            iWinFltCount, iFilterAlg;

    //general...
    flag           bRecordIt, bDriver;
    long           lDataIndex;
    char           cType;
    CCnvIndex      iCnv;
    int            iCharLen;
    DataUnion      Val;
    char           cTagEndDesc[MaxCXMsgLstSize-sizeof(CXM_Header)-220 /*Space for data members*/]; // Must Be Last;

    CXM_KeepHistory(DataUnion* pVal, long DataIndex,
      char tt_Type, char * Tag, CCnvIndex CnvNo, char * CnvTxt, char * Desc,
      double FilterTau, double WinFltPeriod, int WinFltCount, double FltDelta,
      double Change, double DeltaLo, double DeltaHi,
      int NoRec, int NoNotRec, int FilterAlg, int BoxcarAlg,
      flag RecordIt, flag Driver);

    DEFINE_SPARES(CXM_KeepHistory)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_KeepHistoryFile : public CXM_Header
  {
  public:
    char           cHistTagListFile[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/]; // Must Be Last;

    CXM_KeepHistoryFile(char * Filename)
      {
      Id=XM_KeepHistoryFile;
      m_dwLength=&cHistTagListFile[0]-(char*)this;
      if (Filename)
        {
        strcpy(cHistTagListFile, Filename);
        m_dwLength+=strlen(cHistTagListFile)+1;
        }
      };

    DEFINE_SPARES(CXM_KeepHistoryFile)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_QueryHistory : public CXM_Header
  {
  public:
    CTimeValue    dStartTime, dEndTime;
    long          iRqstNo;
    long          iSrcID;
    int           nTags;
    char          cTags[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/];

    //CXM_QueryHistoryStart() {iRqstNo=-1;};
    CXM_QueryHistory(double StartTime, double EndTime, long RqstNo, long SrcID);

    flag          xAddTag(int iTrndNo, char* pNewTag);
    char        * FirstTag(int &iTrndNo);
    char        * NextTag(int &iTrndNo, char* pPrevTag);
    flag          Empty();

    DEFINE_SPARES(CXM_QueryHistory)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_QueryHistoryOther : public CXM_Header
  {
  public:
    double        dStartTime;
    double        dEndTime;
    double        dRepTimeOffset;
    long          iSrcID;
    byte          iOpt;
    byte          iTimeOptUnits;
    byte          iQryDest;
    flag          bHeadings;
    flag          bTimeOptFull;
    int           nTags;
    long          iNoPts;
    double        dInvalid;
    //char          cInvalid[21];
    byte          iFileMode;
    char          cFilename1[256];
    char          cFilename2[256];
    char          cTags[MaxCXMsgLstSize-sizeof(CXM_Header)-180-256-256 /*Space for data members*/];

    CXM_QueryHistoryOther(double StartTime, double EndTime, long SrcID, byte Opt, byte TimeOptUnits, flag TimeOptFull, flag Headings, long NoPts, double Invalid, byte QryDest, char* Filename1, char* Filename2, byte QryFileMode, double RepTimeOffset);

    flag          xAddTag(char* pNewTag);
    char        * FirstTag();
    char        * NextTag(char* pPrevTag);
    flag          Empty();
    char*         Filename1() { return cFilename1; };
    char*         Filename2() { return cFilename2; };

    DEFINE_SPARES(CXM_QueryHistoryOther)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_QueryString : public CXM_Header
  {
  public:
    long           iSrcID;
    char           cValue[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/];

    CXM_QueryString(long SrcID, char* Value = NULL)
      {
      Id=XM_QueryString;
      iSrcID=SrcID;
      m_dwLength=&cValue[0]-(char*)this;
      if (Value)
        {
        const int len = strlen(Value)+1;
        strncpy(cValue, Value, len);
        m_dwLength+=len;
        }
      };
    void Set(char* Value, ...)
      {
      m_dwLength=&cValue[0]-(char*)this;
      if (Value==NULL)
        return;
      va_list argptr;
      va_start(argptr, Value);
      VERIFY(vsprintf(cValue, Value, argptr) < sizeof(cValue));
      va_end(argptr);
      m_dwLength+=strlen(cValue)+1;
      };
    char * String() { return cValue; };

    DEFINE_SPARES(CXM_QueryString)
  };

//-------------------------------------------------------------------------

const int MaxQueryRowValues = (MaxCXMsgLstSize-sizeof(CXM_Header)-100) / sizeof(double);

class DllImportExport CXM_QueryRow : public CXM_Header
  {
  public:
    long           iSrcID;
    double         dTime;
    int            nPts;
    double         dValue[MaxQueryRowValues];

    CXM_QueryRow(long SrcID, double Time)
      {
      Id=XM_QueryRow;
      iSrcID=SrcID;
      dTime=Time;
      nPts=0;
      m_dwLength=(char*)(&dValue[0])-(char*)this;
      };
    flag AddValue(double Value, ...)
      {
      if (nPts>=MaxQueryRowValues)
        return False;
      dValue[nPts++] = Value;
      m_dwLength += sizeof(double);
      return True;
      };
    double         Value(int i) { ASSERT(i<nPts); return dValue[i]; };

    DEFINE_SPARES(CXM_QueryRow)
  };

//-------------------------------------------------------------------------

const byte QueryRowExType_Null   = 0;
const byte QueryRowExType_Double = 1;
const byte QueryRowExType_Long   = 2;
const byte QueryRowExType_Str    = 3;
const int QueryRowExDataLen = MaxCXMsgLstSize-sizeof(CXM_Header)-100; //Space for data members

class DllImportExport CXM_QueryRowEx : public CXM_Header
  {
  public:
    long           iSrcID;
    double         dTime;
    int            nPts;
    char           cData[QueryRowExDataLen];

    CXM_QueryRowEx(long SrcID, double Time)
      {
      Id=XM_QueryRowEx;
      iSrcID=SrcID;
      dTime=Time;
      nPts=0;
      m_dwLength=(char*)(&cData[0])-(char*)this;
      };
    flag           AddValue(double Value);
    flag           AddValue(long Value);
    flag           AddValue(char* Value);
    byte           FirstValTyp(int & Pos);
    byte           NextValTyp(int & Pos);
    double         DValue(int & Pos);
    long           LValue(int & Pos);
    char*          SValue(int & Pos);

    DEFINE_SPARES(CXM_QueryRowEx)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_HistorySlotDlg : public CXM_Header
  {
  public:
    byte           iDlg;
    char           cTag[MaxCXMsgLstSize-sizeof(CXM_Header)-120 /*Space for data members*/]; // Must Be Last;

    CXM_HistorySlotDlg(char * Tag, byte WhichDlg);

    DEFINE_SPARES(CXM_HistorySlotDlg)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_HistoryData : public CXM_Header
  {
  public:
    double         dTime;
    double         dVal;
    byte           iStatus;
    int            iTrndNo;
    long           iRqstNo;

#if _DEBUG
    long           m_AllocNo;
    static long    m_nAllocs;
#endif

    CXM_HistoryData(int TrndNo, long RqstNo, double Time, double Val, byte iStatus);
    virtual ~CXM_HistoryData();
    long Length() { return sizeof(*this);};

    DEFINE_SPARES(CXM_HistoryData)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_HistoryDataError : public CXM_Header
  {
  public:
    long           lErrorNumber;
    double         dTimeMissingData;
    long           iRqstNo;
    char           cFileName[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/];

    CXM_HistoryDataError(long ErrorNumber, long RqstNo, double TimeMissingData, char *ReqdFileName);

    DEFINE_SPARES(CXM_HistoryDataError)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_HistRecordingOn : public CXM_Header
  {
  public:
    flag           bRecordingOn;
    char           cTag[MaxCXMsgLstSize-sizeof(CXM_Header)-120 /*Space for data members*/]; // Must Be Last;

    CXM_HistRecordingOn(char * Tag, flag RecordingOn);

    DEFINE_SPARES(CXM_HistRecordingOn)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_QueryTime : public CXM_Header
  {
  public:
    CTimeValue     TimeRqd;
    CTimeValue     dTimeRqd;

    CXM_QueryTime();
    CXM_QueryTime(CTimeValue TimeRqd_, CTimeValue dTimeRqd_);

    DEFINE_SPARES(CXM_QueryTime)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_Execute : public CXM_Header
  {
  public:
    CTimeValue    Time;
    CTimeValue    dTimeNext;

    CXM_Execute();
    CXM_Execute(CTimeValue Time_, CTimeValue dTimeNext_);

    DEFINE_SPARES(CXM_Execute)
  };

//-------------------------------------------------------------------------

enum eScnTypes       { ScnTyp_Manual, ScnTyp_Duration, ScnTyp_StopTime, ScnTyp_Steps, RTT_SteadyState};
enum eScnTermActions { ScnTA_None, ScnTA_Edit, ScnTA_Idle };
enum eScnState       { Scn_Off, Scn_Start, Scn_Busy, Scn_Complete, Scn_Ignore };
enum eScnContRest    { Scr_Continue, Scr_ReStart };
enum eScnBlkRest     { Sbr_Block, Sbr_ReStart };

class DllImportExport CXM_TimeControl : public CXM_Header
  {
  public:
    flag              m_bRealTime;
    flag              m_bSyncWithClock;
    flag              m_bHoldAdv;
    CTimeValue        m_TheTime;
    CTimeValue        m_StepSize;
    CTimeValue        m_StepSizeMax;
    CTimeValue        m_StepSizeNxt;
    CTimeValue        m_TimeToStop;
    CTimeValue        m_ScnTime;
    CTimeValue        m_ScnTimeMax;
    double            m_RealTimeMult;
    long              m_StepCount;
    long              m_StepCountMax;
                      
    EqnSlvCtrlBlk     m_EqnCB;
                      
    eScnState         m_ScnState;

    eScnContRest      m_ManualScnReStart;
    eScnContRest      m_ReStartIfScnBusy;
    eScnBlkRest       m_ReStartIfScnComplete;
                      
    eScnTypes         m_ScnType;
    eScnTermActions   m_TermAction;
    CTimeValue        m_ScnDuration;
    CTimeValue        m_ScnStopTime;

    flag            & m_RestartPGMOnStart;
    flag            & m_RestartPrfOnStart;

    CXM_TimeControl(flag & RestartPGMOnStart, flag & m_RestartPrfOnStart);
    //CXM_TimeControl(CXM_TimeControl &TC);

    long             StepSizeMaxMS() { return (long)(m_StepSizeMax.mSeconds); };

    DEFINE_SPARES(CXM_TimeControl)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_Long : public CXM_Header
  {
  public:
    long           lValue;

    CXM_Long(long Value) {lValue=Value; Id=XM_Long; m_dwLength=sizeof(*this); };

    DEFINE_SPARES(CXM_Long)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_Boolean : public CXM_Header
  {
  public:
    flag           bValue;

    CXM_Boolean(flag Value) { bValue=Value; Id=XM_Boolean; m_dwLength=sizeof(*this); };

    DEFINE_SPARES(CXM_Boolean)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_Double : public CXM_Header
  {
  public:
    double         dValue;

    CXM_Double(double Value) { dValue=Value; Id=XM_Double; m_dwLength=sizeof(*this); };

    DEFINE_SPARES(CXM_Double)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_String : public CXM_Header
  {
  public:
    char           cValue[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/];

    CXM_String(char* Value = NULL)
      {
      Id=XM_String;
      m_dwLength=&cValue[0]-(char*)this;
      if (Value)
        {
        strcpy(cValue, Value);
        m_dwLength+=strlen(cValue)+1;
        }
      };
    void Set(char* Value, ...)
      {
      m_dwLength=&cValue[0]-(char*)this;
      if (Value==NULL)
        return;
      va_list argptr;
      va_start(argptr, Value);
      VERIFY(vsprintf(cValue, Value, argptr) < sizeof(cValue));
      va_end(argptr);
      m_dwLength+=strlen(cValue)+1;
      };
    char * String() { return cValue; };

    DEFINE_SPARES(CXM_String)
  };

//-------------------------------------------------------------------------

#if WITHDDEREPORTS
class DllImportExport CXM_DDEReport : public CXM_Header
  {
  public:
    byte           iOpt;
    short          iReportNamePos;
    char           cBuff[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/];

    CXM_DDEReport(byte Opt, char* FileName, char* ReportName);
    char* FileName() { return cBuff; };
    char* ReportName() { return &cBuff[iReportNamePos]; };

    DEFINE_SPARES(CXM_DDEReport)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_DDEErrorCode : public CXM_Header
  {
  public:
    long           lErrorNumber;
    char           cMsg[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/];

    CXM_DDEErrorCode(long ErrorNumber, char *Msg);

    DEFINE_SPARES(CXM_DDEErrorCode)
  };
#endif

//-------------------------------------------------------------------------

class DllImportExport CXM_DrvShowTagInfo : public CXM_Header
  {
  public:
    WORD            iDrvOptMask;
    char            cTag[MaxCXMsgLstSize-sizeof(CXM_Header)-120 /*Space for data members*/]; // Must Be Last;

    CXM_DrvShowTagInfo(char * Tag, WORD DrvOptMask);

    DEFINE_SPARES(CXM_DrvShowTagInfo)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_OleExcelReport : public CXM_Header
  {
  public:
    short          iOpt;
    short          iReportNamePos;
    CScdCOCmdBlk *pComCmdBlk; //CNM
    char           cBuff[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/];

    CXM_OleExcelReport(CScdCOCmdBlk *ComCmdBlk, char* FileName, char* ReportName, short Opt);
    char* FileName() { return cBuff; };
    char* ReportName() { return &cBuff[iReportNamePos]; };
    CScdCOCmdBlk *ComCmdBlk() { return pComCmdBlk; };; // CNM

    DEFINE_SPARES(CXM_OleExcelReport)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_OleErrorCode : public CXM_Header
  {
  public:
    long           lErrorNumber;
    char           cMsg[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/];

    CXM_OleErrorCode(long ErrorNumber, char *Msg);

    DEFINE_SPARES(CXM_OleErrorCode)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_RepTrendDB : public CXM_Header
  {
  public:
    CTimeValue      dEndTime;
    CTimeValue      dDuration;
    long            iNoOfPts;
    long            iTableNamePos;
    long            iTagsPos;
    int             nTags;
    char            cBuff[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/];

    CXM_RepTrendDB(char* FileName, char* TableName, CTimeValue EndTime, CTimeValue Duration, long NoOfPts);
    char          * FileName() { return cBuff; };
    char          * TableName() { return &cBuff[iTableNamePos]; };
    flag            xAddTag(char* pNewTag);
    char          * FirstTag();
    char          * NextTag(char* pPrevTag);
    flag            Empty();

    DEFINE_SPARES(CXM_RepTrendDB)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_ArchiveExists : public CXM_Header
  {
  public:
    char            cTag[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/];

    CXM_ArchiveExists(char * Tag);
    CXM_ArchiveExists();//(char * Tag);
    flag            xAddTag(char * NewTag);//, char * pPosition);
    char          * FirstTag();
    char          * NextTag(char * pPrevTag);
    void            SetFlags(char * pTag, dword Flags);
    dword           GetFlags(char * pTag);
    flag            Empty();

    DEFINE_SPARES(CXM_ArchiveExists)
  };

//-------------------------------------------------------------------------

class DllImportExport CXM_ArcShowTagInfo : public CXM_Header
  {
  public:
    WORD            iArcOptMask;
    char            cTag[MaxCXMsgLstSize-sizeof(CXM_Header)-120 /*Space for data members*/]; // Must Be Last;

    CXM_ArcShowTagInfo(char * Tag, WORD ArcOptMask);

    DEFINE_SPARES(CXM_ArcShowTagInfo)
  };

//-------------------------------------------------------------------------

const byte ADBFn_Null       = 0;
const byte ADBFn_Instant    = 1;
const byte ADBFn_Minimum    = 2;
const byte ADBFn_Maximum    = 3;
const byte ADBFn_PeriodAvg  = 4;
const byte ADBFn_RunningAvg = 5;
const byte ADBFn_Count      = 6;

const byte ADBMeas_Null     = 0;
const byte ADBMeas_StartPt  = 1;
const byte ADBMeas_MidPt    = 2;
const byte ADBMeas_EndPt    = 3;

const byte ADBFirst_Null    = 0;
const byte ADBFirst_Use     = 1;
const byte ADBFirst_Ignore  = 2;

const byte ADBTbFmt_Null    = 0;
const byte ADBTbFmt_DB      = 1;
const byte ADBTbFmt_CSV     = 2;
const byte ADBTbFmt_TXT     = 3;

extern DllImportExport LPCTSTR ADBFuncTags[];
extern DllImportExport LPCTSTR ADBMeasTags[];
extern DllImportExport LPCTSTR ADBFirstTags[];
extern DllImportExport LPCTSTR ADBTbFmtTags[];

class DllImportExport CXM_KeepArchive : public CXM_Header
  {
  public:
    long           lDataIndex;
    char           cType;
    CCnvIndex      iCnv;
    int            iCharLen;
    DataUnion      Val;
    byte           m_iFn;
    byte           m_iMeas;
    byte           m_iFirst;
    short          iCnvPos;
    short          iTablePos;
    short          iDescPos;
    short          iFldNamePos;
    char           cBuff[MaxCXMsgLstSize-sizeof(CXM_Header)-120 /*Space for data members*/]; // Must Be Last;

    CXM_KeepArchive(DataUnion* pVal, long DataIndex,
      char tt_Type, char * Tag, CCnvIndex CnvNo, char * CnvTxt,
      char * Table, char * FldName, byte Fn, byte Meas, byte First, char * Description);
    LPCTSTR        TableName();
    LPCTSTR        Tag();
    LPCTSTR        FldName();
    LPCTSTR        CnvTxt();
    LPCTSTR        Description();

    DEFINE_SPARES(CXM_KeepArchive)
  };

//-------------------------------------------------------------------------

class DllImportExport CXMsgLst : public CList<CXM_Header*, CXM_Header*> 
  {
  protected:
    bool          m_bOnHeap;
    bool          m_bEOF;
    POSITION      m_RdPos;

  public :
    CXMsgLst();
    ~CXMsgLst();
    void  Clear();

    long          NoMsgs() { return GetCount(); };

    flag          PackMsg(CXM_Header * pMsg);//, flag KeepSpaceForRoute=True);
    CXM_Header *   MsgPtr(byte RqdMsgId=XM_Null);
    flag          MsgAvail(byte RqdMsgId=XM_Null);
    byte          MsgId();
    flag          MsgIsSkip(byte RqdMsgId);

    //dword          Length() { return (char*)&Data[WrtPos]-(char*)this; };

    void          RewindReadPosition() { m_RdPos=NULL; m_bEOF=(GetCount()==0); };
    //void           SetReadPosition(long ReadPos) { RdPos=ReadPos; };
    //long           GetReadPosition() { return RdPos; };

    void          SetOnHeap(bool On) { m_bOnHeap=On; };
    void          dbgDump(flag Full, char* Hd1=NULL, char* Hd2=NULL, char* Hd3=NULL);

    // Helper Methods
    CXM_ObjectTag *        ObjectTag        () { return (CXM_ObjectTag *       )MsgPtr(XM_ObjectTag       ); };
    CXM_ObjectData *       ObjectData       () { return (CXM_ObjectData *      )MsgPtr(XM_ObjectData      ); };
    CXM_Route *            Route            () { return (CXM_Route *           )MsgPtr(XM_Route           ); };
    CXM_ReadIndexedData *  ReadIndexedData  () { return (CXM_ReadIndexedData * )MsgPtr(XM_ReadIndexedData ); };
    CXM_DataRequest *      DataRequest      () { return (CXM_DataRequest *     )MsgPtr(XM_DataRequest     ); };
    CXM_DataAvailable *    DataAvailable    () { return (CXM_DataAvailable *   )MsgPtr(XM_DataAvailable   ); };
    CXM_TagNotAvail *      TagNotAvail      () { return (CXM_TagNotAvail *     )MsgPtr(XM_TagNotAvail     ); };
    CXM_TimeControl *      TimeControl      () { return (CXM_TimeControl *     )MsgPtr(XM_TimeControl     ); };
    CXM_QueryTime *        QueryTime        () { return (CXM_QueryTime *       )MsgPtr(XM_QueryTime       ); };
    CXM_Execute *          Execute          () { return (CXM_Execute *         )MsgPtr(XM_Execute         ); };

    CXM_HistoryExists *    HistoryExists    () { return (CXM_HistoryExists *   )MsgPtr(XM_HistoryExists   ); };
    CXM_KeepHistory *      KeepHistory      () { return (CXM_KeepHistory *     )MsgPtr(XM_KeepHistory     ); };
    CXM_KeepHistoryFile *  KeepHistoryFile  () { return (CXM_KeepHistoryFile * )MsgPtr(XM_KeepHistoryFile ); };
    CXM_HistorySlotDlg *   HistorySlotDlg   () { return (CXM_HistorySlotDlg *  )MsgPtr(XM_HistorySlotDlg  ); };
    CXM_HistoryData *      HistoryData      () { return (CXM_HistoryData *     )MsgPtr(XM_HistoryData     ); };
    CXM_HistoryDataError * HistoryDataError () { return (CXM_HistoryDataError *)MsgPtr(XM_HistoryDataError); };
    CXM_QueryHistory *     QueryHistory     () { return (CXM_QueryHistory *    )MsgPtr(XM_QueryHistory    ); };
    CXM_QueryHistoryOther *QueryHistoryOther() { return (CXM_QueryHistoryOther*)MsgPtr(XM_QueryHistoryOther); };
    CXM_QueryString *      QueryString      () { return (CXM_QueryString *     )MsgPtr(XM_QueryString     ); };
    CXM_QueryRow *         QueryRow         () { return (CXM_QueryRow *        )MsgPtr(XM_QueryRow        ); };
    CXM_QueryRowEx *       QueryRowEx       () { return (CXM_QueryRowEx *      )MsgPtr(XM_QueryRowEx      ); };

    CXM_HistRecordingOn *  HistRecordingOn  () { return (CXM_HistRecordingOn * )MsgPtr(XM_HistRecordingOn ); };

    CXM_Long *             Long             () { return (CXM_Long *            )MsgPtr(XM_Long            ); };
    CXM_Boolean *          Boolean          () { return (CXM_Boolean *         )MsgPtr(XM_Boolean         ); };
    CXM_Double *           Double           () { return (CXM_Double *          )MsgPtr(XM_Double          ); };
    CXM_String *           String           () { return (CXM_String *          )MsgPtr(XM_String          ); };

#if WITHDDEREPORTS
    CXM_DDEReport *        DDEReport        () { return (CXM_DDEReport *       )MsgPtr(XM_DDEReport       ); };
    CXM_DDEErrorCode *     DDEErrorCode     () { return (CXM_DDEErrorCode *    )MsgPtr(XM_DDEErrorCode    ); };
#endif

    CXM_DrvShowTagInfo *   DrvShowTagInfo   () { return (CXM_DrvShowTagInfo *  )MsgPtr(XM_DrvShowTagInfo  ); };

    CXM_OleExcelReport *   OleExcelReport   () { return (CXM_OleExcelReport *  )MsgPtr(XM_OleExcelReport  ); };
    CXM_OleErrorCode *     OleErrorCode     () { return (CXM_OleErrorCode *    )MsgPtr(XM_OleErrorCode    ); };

    CXM_RepTrendDB *       RepTrendDB       () { return (CXM_RepTrendDB *      )MsgPtr(XM_RepTrendDB      ); };

    CXM_ArchiveExists *    ArchiveExists    () { return (CXM_ArchiveExists *   )MsgPtr(XM_ArchiveExists   ); };
    CXM_KeepArchive *      KeepArchive      () { return (CXM_KeepArchive *     )MsgPtr(XM_KeepArchive     ); };
    CXM_ArcShowTagInfo *   ArcShowTagInfo   () { return (CXM_ArcShowTagInfo *  )MsgPtr(XM_ArcShowTagInfo  ); };

    DEFINE_SPARES(CXMsgLst) 

#if _DEBUG
    long           m_AllocNo;
    static long    m_nAllocs;
    static long    m_LRange1;
    static long    m_LRange2;
#endif
  };

//=========================================================================
// All the Packed structures must have packing of 1 byte
#pragma pack(pop)

// --------------------------------------------------------------------------

//inline flag CXMsgLst::PackMsg(CXM_Header *pMsg)//, flag KeepSpaceForRoute/*=True*/)
//  {
//  ASSERTif (abs(pMsg-this)<
//  AddTail(pMsg);
//  m_bEOF=false;
//  return True;
//  };

//---------------------------------------------------------------------------

inline CXM_Header * CXMsgLst::MsgPtr(byte RqdMsgId/*=XM_Null*/)
  {
  if (!m_bEOF)
    {
    if (m_RdPos==NULL) 
      m_RdPos=GetHeadPosition();
    CXM_Header * p=GetNext(m_RdPos);
    m_bEOF=(m_RdPos==NULL);
    if (RqdMsgId!=XM_Null)
      VERIFY(RqdMsgId==p->Id);
    return p;
    }
  return NULL;
  };

//---------------------------------------------------------------------------

inline flag CXMsgLst::MsgAvail(byte RqdMsgId/*=XM_Null*/)
  {
  if (!m_bEOF)
    {
    if (RqdMsgId!=XM_Null)
      {
      CXM_Header * p=m_RdPos ? GetAt(m_RdPos) : GetHead();
      return (RqdMsgId==p->Id);
      }
    return True;
    }
  return False;
  };

//---------------------------------------------------------------------------

inline byte CXMsgLst::MsgId()
  {
  if (!m_bEOF)
    {
    CXM_Header * p=m_RdPos ? GetAt(m_RdPos) : GetHead();
    return (p->Id);
    }
  return XM_Null;
  };

//---------------------------------------------------------------------------

inline flag CXMsgLst::MsgIsSkip(byte RqdMsgId)
  {
  if (!m_bEOF)
    {
    CXM_Header * p=m_RdPos ? GetAt(m_RdPos) : GetHead();
    if (RqdMsgId==p->Id)
      {
      GetNext(m_RdPos);
      return True;
      }
    }
  return False;
  };

//=========================================================================

//class DllImportExport XStkMsgBlk : public CXMsgLst  
//  {
//  public:
//    XStkMsgBlk() { m_bOnHeap=false; }
//  };
//
//class DllImportExport XHeapMsgBlk : public CXMsgLst  
//  {
//  public:
//    XHeapMsgBlk() { m_bOnHeap=true; }
//  };

//=========================================================================


#undef DllImportExport

#endif
