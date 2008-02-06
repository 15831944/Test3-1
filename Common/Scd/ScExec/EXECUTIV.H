//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef __EXECUTIV_H
#define __EXECUTIV_H

#ifndef __AFXTEMPL_H__
  #include <afxtempl.h> // "xafxtempl.h"
#endif
#ifndef __EXECLIB_H
  #include "execlib.h"
#endif
#ifndef __EQNSOLVE_H
  #include "eqnsolve.h"
#endif
#ifndef __CTRLSEQ_H
  #include "ctrlseq.h"
#endif
#include "tagdb.h"
#include "scdtempl.h"
#include "..\..\..\smdk\include\md_share4.h"

#ifdef __EXECUTIV_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


#define dbgKeepMsgHistory 1

/*#L:D:The #b<CExecutive> is the kernel or core class of SysCAD,
 only one instance of it should exist. Its main functions include,
 starting, stopping and executing SysCAD and managing a number of #b<CExecObj>ects
 ie serializing the data stored by these objects and managing the
 transfer of data between these objects.
 The CExecObjects , when they are created, they need to #i<register>
 with the #b<CExecutive>, where they pass their name to the CExecutive, the
 #b<CExecutive> then asks the #b<CExecObj> if it is aware about time
 ( at least one CExecObj must be aware of time ). These #b<CExecObj>ects can
 #i<deregister> at any time.
 The CExecutive then asks each exec object, if they require any information,
  and if so what kind, it then asks each CExecObject in turn if they possess
  the required information, and when its found, it passes this information
  back to the necessary CExecObject.
 When SysCAD is to be saved each CExecObj is individually saved,(BeginSave)
 where initially any data unique to the CExecObj is saved
  eg. the graphics window will save a .dxf file
  then any tagged objects and any connections contained in the
  CExecObject are saved to a central database, once the CExecObject is saved,
  it sends out an EndSave message, to inform the next CExecObj to be to save itself
  once all the CExecObj are saved, the SaveComplete message is sent to the CExecutive

Similarly - when SysCAD is to be loaded each CExecObj is individually loaded,(Beginload)
 where initially any data unique to the CExecObj is loaded
  eg. the graphics window will load a .dxf file
  then any tagged objects and any connections required by the
  CExecObject are loaded from a central database, once the CExecObject is loaded,
  it sends out an Endload message, to inform the next CExecObj to be to load itself
  once all the CExecObj are loadd, the loadComplete message is sent to the CExecutive
This load complete message is important since during the load the data belong
to SysCAD is unstable, and it is only after all the data has been completely
loaded that data-consistency checking can occur.
While running , per iteration the CExecutive allows each CExecObject to execute,
 after an CExecObject has executed, it then reads the new data from the CExecObject
  and ????
 While SysCAD is running, if any CExecObject needs to read another CExecObject's
  data it sends an Open Transation meeasge then a ReadTaggedItem(msgblock parameter object contains procedures for storing data) message to
  the CExecutive which then obtains the information, only when the data is stable,
  then a close transation message is sent. The open/close transatin messages are
  necessary since they prevent other processes from gaining access to an object's
  data when that object is currently involved in another operation, which may
  invalidate the information that is to be obtained. When data is to be set during run time then the same procedure is carried out,
  with the WriteTaggedObject function being used instead.

 The CExecObjects contain tagged objects
 The exec objects include : graph window, DDE, Dyn_graph, trends, Access Nodes, Flowsheets
*/
//=========================================================================
//
// NBNBNB This must not conflict with command ids in syscad
//
#define IDM_EXECUTIVE_CMD               4102
#define ID_EXECUTIVE_TIMER              123

//=========================================================================

const int  MaxRqdPriority = 5;
const int  MaxXObjects    = 256;       // MAXIMUM_WAIT_OBJECTS;

//_FWDDEF(CExecutive);
//_FWDDEF(CExecObj);
//_FWDDEF(CXTagBlk);
// forwards
class CExecutive;
class CExecObj;
class CXTagBlk;

//-------------------------------------------------------------------------
//executive object names...
#define pExecName_IOMarshal  "IOMarshal"
#define pExecName_DrvMngr    "Driver"
#define pExecName_ArcMngr    "Archive"
#define pExecName_MainWnd    "Main_Window"
#define pExecName_TrendDoc   "Trend"
#define pExecName_Project    "Project"
#define pExecName_Historian  "Historian"
#define pExecName_GrfWnd     "Graphics"
#define pExecName_OLE        "OLE"
#define pExecName_DDE        "DDE"
#define pExecName_ExecCmd    "Command_Scripts"
#define pExecName_Rep        "Reports"

//-------------------------------------------------------------------------
//return codes from XSendMessage (EO_Message)...
const DWORD RC_SM_HistExists     = 0x0001; //for XM_HistoryExists, XM_KeepHistory, XM_HistRecordingOn
const DWORD RC_SM_HistRecOn      = 0x0002; //for XM_HistoryExists, XM_KeepHistory, XM_HistRecordingOn
const DWORD RC_SM_HistDriverTag  = 0x0004; //for XM_HistoryExists, XM_KeepHistory, XM_HistRecordingOn
const DWORD RC_SM_ArcExists      = 0x0010; //for XM_ArchiveExists, XM_KeepArchive
const DWORD RC_SM_ArcRecOn       = 0x0020; //for XM_ArchiveExists, XM_KeepArchive
const DWORD RC_SM_ArcDriverTag   = 0x0040; //for XM_ArchiveExists, XM_KeepArchive

//-------------------------------------------------------------------------
//return codes from XReadTaggedItem (EO_ReadTaggedItem)...
const DWORD RC_RTI_Exists        = 0x0001; //tag exists
const DWORD RC_RTI_DriverTag     = 0x0002; //tag is 'owned' by driver executive object
const DWORD RC_RTI_SolverTag     = 0x0004; //tag is 'owned' by flowsheet solver executive object
const DWORD RC_RTI_ArchiveTag    = 0x0008; //tag is 'owned' by Archive solver executive object
const DWORD RC_RTI_NumData       = 0x0010; //tag data type is numerical
const DWORD RC_RTI_StrData       = 0x0020; //tag data type is string
const DWORD RC_RTI_IsParm        = 0x0040; //tag has isParm flag set
const DWORD RC_RTI_MarshalTag    = 0x0080; //tag is 'owned' by IOMarshal

//-------------------------------------------------------------------------
const byte RQ_Tags              = 1;  //request is for tags owned by exec-objects
const byte RQ_Files             = 2;  //request is for files used by exec-objects
const byte RQ_TagCount          = 3;  //request is for number of tags owned by exec-objects
const byte RQ_TagsConns         = 4;  //request is for number of tags owned by exec-objects and a text rep of their connections
const byte RQ_TagCountCategory  = 5;  //request is for number of tags matching specified category

//these two TagInfoRecs can/need to be extended to allow for different
//requests and to return more info...
class DllImportExport RequestTagInfoRec
  {
  public:
    byte        bFirst:1,   //set this to true for FIRST request only
                RQ_Type:4;  //request type
    char*       pGroup;     //non-null if specific classID objects required
    Strng       sExcludeExecObj; //name of executive object NOT to be queried
    DWORD       dwReqData;  //extra request data/info/flags

    RequestTagInfoRec() { Reset(); };
    RequestTagInfoRec(byte RequestType) { Reset(RequestType); };
    void Reset(byte RequestType = RQ_Tags) { bFirst = 1; RQ_Type = RequestType; pGroup = NULL; sExcludeExecObj = ""; dwReqData = 0; };
  };

class DllImportExport ReplyTagInfoRec
  {
  friend class CExecutive;
  public:
    Strng       sData;          //tag or filename found
    Strng       sData1;         //extra data
    DWORD       dwData;         //extra data
    CExecObj*   pExecObj;       //executive object data belongs to
    flag        bData1Valid:1,  //sData1 has valid data
                bDWDataValid:1, //dwData has valid data
                bCanEditFile:1, //text editable file returned
                bExcelFile:1,   //excel file returned
                bForceCopy:1,   //Set if a CopyFile Required
                bPreventCopy:1; //Set if a CopyFile must be prevented
  private:
    int         OInx;        //index to array of ExecObj currently used
    int         iCount;      //reply number for current ExecObj
    long        lPos;        //4byte value that can be used as required by ExecObj eg as Position

  public:
    ReplyTagInfoRec() { pExecObj=NULL; dwData=0; bData1Valid=0; bDWDataValid=0; OInx=0; iCount=0; lPos=0; };
    int Count() { return iCount; };
    long GetPos() { return lPos; };
    void SetPos(long Pos) { lPos = Pos; };
  };

//-------------------------------------------------------------------------

const byte MRG_ChangeAllTags = 0;
const byte MRG_ChangeDuplicates = 1;
const byte MRG_ChangeNone = 2;

const byte MRG_Prefix = 0;
const byte MRG_Suffix = 1;

//helper class for merging projects...
class DllImportExport CMergeProjectsInfo
  {
  public:
    Strng       sSlavePrj;           //secondary/imported project
    Strng       sSlavePrjDir;        //imported project directory
    Strng       sLabel;              //database Flowsheet Label "Default"
    Strng       sReportFile;         //merge report filename
    Strng       sMasterModelCfg;     //current project model configuration
    Strng       sSlaveModelCfg;      //import project model configuration
    Strng       sTagAppendChars;     //characters to be continuously appended to ensure unique tags
    Strng       sFlwExecName;        //solver Exec Object name
    CExecObj*   pFlwExec;            //pointer to solver executive object
    int         iMasterTagCnt;       //number of tags in current project
    int         iSlaveTagCnt;        //number of tags in imported project
    int         iMasterExtraChngCnt; //number of changed tags in current project that required extra characters to be appended to make tag unique
    int         iSlaveExtraChngCnt;  //number of changed tags in imported project that required extra characters to be appended to make tag unique
    byte        iMasterChangeType;   //
    byte        iSlaveChangeType;    //
    byte        iMasterRenameType;   //
    byte        iSlaveRenameType;    //
    Strng       sMasterPrefix;       //
    Strng       sMasterSuffix;       //
    Strng       sSlavePrefix;        //
    Strng       sSlaveSuffix;        //
#if WithNumericTags
    BOOL        bMasterNumTagsBAD;   //
#endif
    BOOL        bMasterStNumTagsBAD; //
#if WithNumericTags
    BOOL        bSlaveNumTagsBAD;    //
#endif
    BOOL        bSlaveStNumTagsBAD;  //
    CSVector    SlaveTags;           //
    CSVector    PreChangeTags;       //tags in master project requiring a change
    CSVector    PreTempTags;         //intermediate temporary unique tags for master
    CSVector    PreNewTags;          //final tags for master
    CSVector    PostChangeTags;      //
    CSVector    PostNewTags;         //
    CByteArray  PreFlags;            //
    CByteArray  PostFlags;           //
    CSVector    MasterTrends;        //filenames of all trend files in master
    CSVector    SlaveTrends;         //filenames of all trend files in slave
    CSVector    MasterGraphics;      //filenames of all graphics files in master
    CSVector    SlaveGraphics;       //filenames of all graphics files in slave
    CSVector    PostChangeTrends;    //filenames of all trend files in slave that need to be changed
    CSVector    PostNewTrends;       //new filenames of all trend files in slave that will be changed
    CSVector    PostChangeGraphics;  //filenames of all graphics files in slave that need to be changed
    CSVector    PostNewGraphics;     //new filenames of all graphics files in slave that will be changed
    CSVector    MasterRefFiles;      //filenames of all files referenced in master
    CSVector    MasterRefModels;     //models used by referenced files in master
    CSVector    SlaveRefFiles;       //filenames of all files referenced in slave
    CSVector    SlaveRefModels;      //models used by referenced files in slave
    CSVector    PostChangeRefFiles;  //filenames of all referenced files in slave that need to be changed
    CSVector    PostNewRefFiles;     //new filenames of all referenced files in slave that will be changed
    int         iProblemRefFiles;    //number of referenced files that have name clashes
    int         iRefFilesUnknownCnt;
    int         iRefFilesExistsCnt;
    BOOL        bIgnoreProbRefFiles; //do not copy ref files from slave that clash
    Strng       sError;              //last error description
    int         iErrorNo;            //last error code

    CMergeProjectsInfo() { sLabel = "Default"; pFlwExec = NULL; };
    void MakeNewTag(char* pTag, flag ForMaster, Strng& sNewTag);
    int NoPreChangeTags() { return iMasterChangeType==MRG_ChangeNone ? iMasterExtraChngCnt : PreChangeTags.GetSize(); };
    int NoPostChangeTags() { return iSlaveChangeType==MRG_ChangeNone ? iSlaveExtraChngCnt : PostChangeTags.GetSize(); };
  };

//-------------------------------------------------------------------------

_FWDDEF(ObjTagIndex);
class DllImportExport ObjTagIndex
  {
  friend class CExecutive;
  friend class CExecObj;
  public:
    int              iObjExecIndex;
    int              iTag;
    void             Clear();// {iObjExecIndex=-1;};
    void             Set(int iObj_, int iTag_);// {iObjExecIndex=iObj_; iTag=iTag_;};
    flag             Valid() {return iObjExecIndex>=0;};
    CXTagBlk        *pTag();
    ObjTagIndex()    { iObjExecIndex=-1; iTag=0; };

  };

//-------------------------------------------------------------------------

class DllImportExport CXTagBlk
  {
  public:
    Strng            sExtTag;
    TABOptions       Options;
    XIOAction        eAction;
    long             lIndex;
    dword            wAllocSize;

    flag             bClientEnd:1, bDataChgd:1, bWriting:1;
    CXTagBlk        *pNxtChg, *pPrvChg;
    CPkDataItem     *m_pDataItem;
    ObjTagIndex      Rmt;

    int              iNextInMap;

    CXTagBlk();
    ~CXTagBlk();

    flag             PutDataChgd(CPkDataItem * pItem);
    flag             Changed() { return (/*m_pDataBuff &&*/ bDataChgd); };
    void             ClearChanged() { bDataChgd=0; };

    char*            Tag() { return ExtractTag(sExtTag); };

    static void      BuildExtTag(char* pTag, XIOAction Act, flag bClient, Strng& KeyTag);
    static char*     ExtractTag(Strng &ExtTag);
  };

//===========================================================================

typedef  CArray <CXTagBlk*, CXTagBlk*> CXTagArray;
typedef CMap<char *, char *, int, int> CXTagMap;
typedef CArray <CExecObj*, CExecObj*> CExecObjArray;

//===========================================================================

DllImportExport CTimeValue GetCurrentTime_1970();

//===========================================================================
//
//
//
//===========================================================================

const byte EOExec_None      = 0x00;
const byte EOWrite_Msg      = 0x01;
const byte EOExec_Msg       = 0x02;
const byte EORead_Msg       = 0x04;
const byte EOWrite_Thread   = 0x08;
const byte EOExec_Thread    = 0x10;
const byte EORead_Thread    = 0x20;
const byte EODisplayUpdate  = 0x40;

//constants for CanClose return codes...
const int EO_CanClose_Yes  = 0; // Increasing order of priority
const int EO_CanClose_Wait = 1;
const int EO_CanClose_Save = 2;
const int EO_CanClose_No   = 3;

//constants for change tag return codes...
const int  EOCT_FOUND      = 0;
const int  EOCT_NOTFOUND   = 1;
const int  EOCT_DUPLICATE  = 2;
const int  EOCT_NOTALLOWED = 3;
const int  EOCT_DONE       = 4;
const int  EOCT_FAILED     = 5;
const int  EOCT_NULLTAG    = 6;
const int  EOCT_NOTDIFFER  = 7;
const int  EOCT_INVALIDTAG = 8;
const int  EOCT_BADTAGLEN  = 9;
const int  EOCT_DRVNOTALLOWED = 10;
const int  EOCT_ARCNOTALLOWED = 11;
const int  EOCT_RESERVED   = 12;

//constants for delete tag return codes...
const int  EODT_FOUND      = 0;
const int  EODT_NOTFOUND   = 1;
const int  EODT_NOTALLOWED = 2;
const int  EODT_INVALIDTAG = 3;
const int  EODT_NULLTAG    = 4;
const int  EODT_RESERVED   = 5;
const int  EODT_DONE       = 6;
const int  EODT_FAILED     = 7;

//constants for server connect return codes...
const int  EOSC_DONE       = 0;
const int  EOSC_NOTFOUND   = 1;
const int  EOSC_NOTALLOWED = 2;
const int  EOSC_FAILED     = 3;

static DDBValueLst SetStateStrs[]={
  {MSA_Null,            " "},
  {MSA_ZeroFlows,       "ZeroFlows"},
  {MSA_Empty,           "Empty"},
  {MSA_PreSet,          "Preset"},
  {MSA_SteadyState,     "SteadyState"},
  {MSA_PBInit,          "PBInit"},
  {MSA_DynStatsRunInit, "DynStatsRunInit"},
  {MSA_EmptySpillTargets, "EmptySpills"},
  {0}};

struct DllImportExport CEOExecReturn
  {
  CTimeValue m_Time;
  CTimeValue m_StepSizeNxt;

  CEOExecReturn()
    {
    m_Time=TimeNAN;
    m_StepSizeNxt=TimeNAN;
    }
  void Clear()
    {
    m_Time=TimeNAN;
    m_StepSizeNxt=TimeNAN;
    }
  };

const byte UDO_OnlyStatusLine = 0x01;
const byte UDO_ForceUpd       = 0x02;
const byte UDO_ColoursChgd    = 0x04;
const byte UDO_FlushMsgQ      = 0x10;

class DllImportExport CSvcExecCtrl
  {
  public:
    CSvcExecCtrl()
      {
      m_bExportBusy=false;
      }

    void SetXObjArray(CExecObj * p)  { if (p) { m_Objs.SetSize(1); m_Objs[0]=p; } else m_Objs.SetSize(0); };
    void AddXObjArray(CExecObj * p)  { if (p) m_Objs.Add(p); };
    void ClrXObjArray()              { m_Objs.SetSize(0); };
  
    bool           m_bExportBusy;
    CExecObjArray  m_Objs;
  };


class DllImportExport CExecObj
  {
  friend class ObjTagIndex;
  friend class CExecutive;
  public:
    CExecObj();
    virtual ~CExecObj();

    DWORD          ThreadId() { return pObjThread->m_nThreadID; };
    //void           SetThreadId(DWORD Id) { pThread->m_nThreadID=Id; };
    //HANDLE         ExitEvent() { return hExitEvent; };
    long           Id() { return lId; };
    pchar          Name() { return sName(); };
    flag           Active() { return bActive; };
    int            ObjExecIndex() { return iObjExecIndex; };

    int            AddXTagBlk(int Index, CXTagBlk * p);
    int            RemoveXTagBlk(int Index, CXTagBlk * p);

    void           LockXO() { EnterCriticalSection(&csObjLock); };
    void           UnlockXO() { LeaveCriticalSection(&csObjLock); };

    flag           EO_Register(const char* Name_, byte bExec_, int RqdPriority, DWORD StackSize, int HashSize=17);
    flag           EO_DeRegister();
    void           EO_MarkForDelete();
    void           EO_AllowForceWriteOnStart(BOOL On);
    void           EO_SynchronousDataIO();
    void           EO_ASynchronousDataIO();
    flag           EO_ChangeName(const char* pName);
    CTimeValue     EO_Time() { return dEOTime; };

    virtual void   EO_ThreadInitialise()                                { };
    virtual void   EO_ThreadTerminate()                                 { };

    virtual void   EO_OnAppActivate(BOOL bActive)                       { };
    virtual flag   EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info) { return False; };
    virtual int    EO_QueryChangeTag(pchar pOldTag, pchar pNewTag)      { return EOCT_NOTFOUND; };
    virtual int    EO_ChangeTag(pchar pOldTag, pchar pNewTag)           { return EOCT_NOTFOUND; };
    virtual int    EO_ChangeTagDone(pchar pOldTag, pchar pNewTag)       { return EOCT_DONE;     };
    virtual int    EO_QueryDeleteTags(Strng_List & Tags)                { return EODT_NOTFOUND; };
    virtual int    EO_DeleteTags(Strng_List & Tags)                     { return EODT_NOTFOUND; };
    virtual int    EO_DeleteTagsDone(Strng_List & Tags)                 { return EODT_DONE;     };
    virtual int    EO_CanClose(Strng_List & Problems)                   { return EO_CanClose_Yes; };


    virtual int    EO_SCInsertNodeG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR path,
                                  LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect, 
                                  float angle, const CRectangleF & tagArea, float tagAngle, bool tagVisible, COLORREF Colour, 
                                  bool mirrorX, bool mirrorY)           { return EOSC_NOTFOUND; };
    virtual int    EO_SCDeleteNodeG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR path) { return EOSC_NOTFOUND; };
    virtual int    EO_SCModifyNodeG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR path,
                                  LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect,                                           
                                  float angle, const CRectangleF & tagArea, float tagAngle, bool tagVisible, COLORREF Colour, 
                                  bool mirrorX, bool mirrorY)           { return EOSC_NOTFOUND; };

    virtual int    EO_SCInsertNodeM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR model)      { return EOSC_NOTFOUND; };
    virtual int    EO_SCDeleteNodeM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid)                    { return EOSC_NOTFOUND; };
    virtual int    EO_SCModifyNodeM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR model)      { return EOSC_NOTFOUND; };

    virtual int    EO_SCInsertLinkG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/,
                                   LPCSTR model, 
                                   LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                   LPCSTR OriginTag, LPCSTR DestinationTag, 
                                   CPointFList & ControlPoints, 
                                   const CRectangleF & tagArea, float tagAngle, bool tagVisible) { return EOSC_NOTFOUND; };               
    virtual int    EO_SCDeleteLinkG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/){ return EOSC_NOTFOUND; };
    virtual int    EO_SCModifyLinkG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/,
                                   LPCSTR model, 
                                   LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                   CPointFList & ControlPoints, 
                                   const CRectangleF & tagArea, float tagAngle, bool tagVisible) { return EOSC_NOTFOUND; };

    virtual int    EO_SCInsertLinkM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/,
                                   LPCSTR model, 
                                   LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                   LPCSTR OriginTag, LPCSTR DestinationTag, 
                                   LPCSTR OriginPort, LPCSTR DestinationPort) { return EOSC_NOTFOUND; };               
    virtual int    EO_SCDeleteLinkM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/){ return EOSC_NOTFOUND; };
    virtual int    EO_SCModifyLinkM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/,
                                   LPCSTR model, 
                                   LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                   LPCSTR OriginTag, LPCSTR DestinationTag, 
                                   LPCSTR OriginPort, LPCSTR DestinationPort) { return EOSC_NOTFOUND; };

    virtual DWORD  EO_Message(CXMsgLst &XM, CXM_Route &Route)           { return False; };

    virtual void   EO_SetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags) {};
    virtual flag   EO_GetModelAction(LPCTSTR Tag, CMdlActionArray & Acts)            { return false; };
    virtual flag   EO_SetModelAction(LPCTSTR Tag, CMdlAction & Acts)                 { return false; };

    virtual flag   EO_GetModelGraphic(LPCTSTR Tag, CMdlGraphicArray & Grf)                        { return false; };
    virtual flag   EO_OperateModelGraphic(LPCTSTR Tag, CMdlGraphicWnd & Wnd, CMdlGraphic & Grf)   { return false; };
    
    //virtual flag   EO_SolutionReqd()                                    { return False; };
    //virtual int    EO_InitSolution(EqnSlvCtrlBlk & EQCtrl)              { return -1; };
    //virtual int    EO_StepSolution(EqnSlvCtrlBlk & EQCtrl)              { return -1; };
    //virtual int    EO_TermSolution(EqnSlvCtrlBlk & EQCtrl)              { return -1; };
    virtual int    EO_GetSolutionError(EqnSlvCtrlBlk & EQCtrl, Strng & RptLine) { return -1; };

    virtual void   EO_SetSolveMode()                                     { };
    virtual flag   EO_GotoRunStart(CXM_TimeControl &CB)                  { return True;};
    virtual flag   EO_GotoRunEnd(CXM_TimeControl &CB, bool TimeChanged)  { return True;};
    virtual flag   EO_PreStart(CXM_TimeControl &CB)                      { return True;};
    virtual flag   EO_Starting(flag fBeginStarting)                      { return True;};
    virtual flag   EO_SetTime(CTimeValue TimeRqd, bool Rewind)           { return True;};
    virtual flag   EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd) {return True;}; // False return will prevent run
    virtual flag   EO_Start(CXM_TimeControl &CB)                   { return False;}; // False return will prevent run
    virtual void   EO_QueryHistoryCfg(CXMsgLst &XM)                      { XM.Clear(); };
    virtual void   EO_QueryArchiveCfg(CXMsgLst &XM)                      { XM.Clear(); };
    virtual void   EO_QuerySubsReqd(CXMsgLst &XM)                        { XM.Clear(); };
    virtual flag   EO_QuerySubsReqdDone()                                { return True; };
    virtual void   EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)      { };
    virtual flag   EO_QuerySubsAvailDone()                               { return True; };
    virtual flag   EO_TagsNotAvail(CXMsgLst &XM)                         { return True; };
    virtual flag   EO_ReadSubsData(CXMsgLst &XM)                         { return False;};
    virtual flag   EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)          { return False;};
    virtual DWORD  EO_ReadTaggedItem(CXM_ObjectTag & ObjTag, CXM_ObjectData &ObjData, CXM_Route &Route) { return False; };
    virtual int    EO_WriteTaggedItem(CXM_ObjectData &ObjData, CXM_Route &Route){ return TOData_NotFound;};
    virtual void   EO_GlblResultValidity(flag IsValid, flag IsAlwaysValid) { };
    virtual void   EO_BeginBulkChange()                                  { ;};
    virtual void   EO_EndBulkChange()                                    { ;};

    //virtual flag   EO_UpdateOutputs()                                   {return True;};
    virtual flag   EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet) { return False;};
    virtual flag   EO_StepDone(CXM_TimeControl &CB, flag StepExecuted)   { return False; };
    virtual flag   EO_Stopping(flag fBeginStopping)                      { return True;};
    virtual flag   EO_QueryStop(CXM_TimeControl &CB, bool Starting)      { return False;};
    virtual flag   EO_Stop(CXM_TimeControl &CB)                          { return True;};
    virtual flag   EO_BeginPause(CXM_TimeControl &CB)                    { return True;};
    virtual flag   EO_EndPause(CXM_TimeControl &CB)                      { return True;};
    virtual void   EO_DisplayUpdate(byte Options, const CXM_TimeControl &CB) { };

    virtual flag   EO_BeginSave(FilingControlBlock &FCB)                 { return False;};
    virtual flag   EO_SaveDefinedData(FilingControlBlock &FCB, Strng &Tag, CXMsgLst &XM) { return False;};
    virtual flag   EO_SaveOtherData(FilingControlBlock &FCB)             { return False;};
    virtual flag   EO_SaveConn(FilingControlBlock &FCB, CXMsgLst &XM)    { return False;};
    virtual flag   EO_EndSave(FilingControlBlock &FCB)                   { return False;};
    virtual flag   EO_SaveComplete(FilingControlBlock &FCB)              { return True;};
    virtual flag   EO_BeginLoad(FilingControlBlock &FCB)                 { return False;};
    virtual flag   EO_LoadDefinedData(FilingControlBlock &FCB, CXMsgLst &XM) { return False;};
    virtual flag   EO_LoadOtherData(FilingControlBlock &FCB)             { return False;};
    virtual flag   EO_LoadConn(FilingControlBlock &FCB, CXMsgLst &XM)    { return False;};
    virtual flag   EO_ConnectsDone(FilingControlBlock &FCB)                { return False;};
    virtual flag   EO_DataLoaded(FilingControlBlock &FCB)                { return False;};
    virtual flag   EO_EndLoad(FilingControlBlock &FCB)                   { return False;};
    virtual flag   EO_LoadComplete(FilingControlBlock &FCB)              { return True;};

    virtual flag   EO_FindObject(char* pObjName, CXM_Route &Route)       { return False;};
    //virtual flag   EO_SendMessage(CXMsgLst &XM, CXM_Route &Route)         {return False;};

    virtual int    XChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    XDeleteTags(Strng_List & Tags);

    flag           XFindObject(char* pName, CXM_Route &Route, flag AllowSelf=false);
    flag           XFindObject(const char* pName, CXM_Route &Route, flag AllowSelf=false);
    DWORD          XSendMessage(CXMsgLst &XM, CXM_Route &Route, flag FromExecThread=false);

    //flag           XOpenTransaction();
    flag           XBuildDataLists();
    flag           XBuildMyDataLists();
    //flag           XUpdateData();

    DWORD          XReadTaggedItem(CXM_ObjectTag & ObjTag, CXM_ObjectData &ObjData, CXM_Route &Route);
    int            XWriteTaggedItem(CXM_ObjectData &ObjData, CXM_Route &rRoute);

    void           XIdle();
    void           XStop();
    void           XGlblResultValidity(int IsValid=-1, int IsAlwaysValid=-1);
    //flag           XCloseTransaction();

    flag           XReadSyncData(flag ForceRead);
    flag           XWriteSyncData(flag ForceWrite);

    flag           XReadASyncData(CXMsgLst &XM, flag ReadAll, long &Index);
    flag           XWriteASyncData(CXMsgLst &XM);

    flag           XReadASyncDataItem(long Index, CXM_ObjectData &ObjData);

    //void           XSetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags);
    //flag           XGetModelAction(Strng & Tag, CMdlActionArray & Acts);
    //flag           XSetModelAction(Strng & Tag, CMdlAction & Acts);

    flag           Server() { return bServer; };
    void           SetServer(flag Server);

    flag           Client() { return bClient; };
    void           SetClient(flag Client);

    flag           SelfConnect() { return bSelfConnect; };
    void           SetSelfConnect(flag SelfConnect);

  private:
    void           ClearDataLists();
    flag           LoadTags(CXMsgLst &XM, flag ClientEnd, long &LastIndex);
    flag           UpdateTags(CXMsgLst &XM, flag ClientEnd, long &LastIndex);

    flag           PutData(CXTagBlk &b, CPkDataItem * pItem, flag ForceChg);
    flag           SearchForTag(pchar pTag, XIOAction Action, flag ClientEnd, rObjTagIndex OTI);
    flag           SearchForTag(pchar pExtTag, rObjTagIndex OTI);

  private:
    long           lId;
    int            iObjExecIndex;
    Strng          sName;

    byte           bExec;

    flag           bActive;
    flag           bReadAllData;
    flag           bMarkedForDelete;
    flag           bSyncDataIO;
    flag           bListUpdateReqd;
    flag           bQueryAvailExecd;
    flag           bAllowForceWriteOnStart;

    CWinThread    *pObjThread;
    CTimeValue     dEOTime;

    int            nTags;
    CXTagArray     aTags;   // a Kludge because CMap etc are not dll-interfaced
    CXTagMap       mTags;
    int            nHashSize;

    flag           bServer, bClient, bSelfConnect;

    CRITICAL_SECTION csObjLock;

    void           SetEOTime(CTimeValue Time) { dEOTime=Time; };

  };

//---------------------------------------------------------------------------

struct EOMsgQEntry
  {
  UINT wParam;
  LONG lParam;
  LONG EO_Id;
  };

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport SolveToolBase
  {
  public:

    virtual flag   InitProblem()=0;
    virtual flag   SolveProblem()=0;
    virtual flag   StepProblemBegin()=0;
    virtual flag   StepProblemEnd()=0;
    virtual flag   TermProblem()=0;
    virtual flag   BeginPause()=0;
    virtual flag   EndPause()=0;
  };

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CCompResultLine
  {
  public:
    double   dErr;
    Strng    sErrStr;
    Strng    sTag;
    Strng    sValue1;
    Strng    sValue2;

    CCompResultLine() { dErr = 0.0; };// sTag = ""; sValue1 = ""; sValue2 = ""; };
    CCompResultLine(double Err, char* pErr, char* pTag, char* pV1, char* pV2)
      { dErr = Err; sErrStr=pErr; sTag = pTag; sValue1 = pV1; sValue2 = pV2; };
  };

typedef CArray <CCompResultLine, CCompResultLine&> CBaseCompLineArray;

class DllImportExport CCompLineArray : public CBaseCompLineArray
  {
  public:
    int FindInsertPos(double Err);
  };

class DllImportExport CCompareInfo
  {
  public:
    CCompLineArray List;
    char*       pRoot;
    Strng       sFilename1;
    Strng       sFilename2;
    Strng       sOutFilename;
    long        iMaxCount;
    double      dRelTolerance;
    double      dAbsTolerance;
    long        iNoObjsCompared;     //result count
    long        iNoObjsNotFound;     //result count
    long        iNoTagsDifferent;    //result count
    long        iNoTagsNotMatched;   //result count
    byte        bCurrent:1,          //compare file 1 with current tag values in memory
                bWindow:1,           //results to window list
                bShowStrings:1,      //show different strings
                bShowAllMissing:1,   //show all tags where tag from one set is missing
                bResetOutFile:1,     //Reset output file to empty as apposed to appending to end
                bShowAllTags:1,      //send ALL tags to file
                bShowParamsOnly:1,   //only test parameters
                bShowSortedFile:1,   //send sorted list of largwest errors to file
                bListHeaderRqd:1;    //Must still write list header
    double      dMinErr;             //used for building sorted list
    CStdioFile* pOutF;

    CCompareInfo() { pOutF = NULL; };
    void AddErr(double Err, char* pErr, char* pTag, char* pV1, char* pV2);
  };

//CNM
class DllImportExport CCompareWorkSpace
  {
  friend class CExecutive;

  protected:
    CDWordArray  m_Itms1;
    CDWordArray  m_Itms2;
    CWordArray   m_Inds1;
    CWordArray   m_Inds2;
    CStringArray m_Tags;

    int          m_nItms1;
    int          m_nItms2;
    int          m_nInds1;
    int          m_nInds2;
    int          m_nTags;

  public:

    CCompareWorkSpace()
      {
      m_Itms1.SetSize(0, 32);
      m_Itms2.SetSize(0, 32);
      m_Inds1.SetSize(0, 32);
      m_Inds2.SetSize(0, 32);
      m_Tags.SetSize(0, 32);
      }

    void Reset()

      {
      m_nItms1=0;
      m_nItms2=0;
      m_nInds1=0;
      m_nInds2=0;
      m_nTags=0;
      }

    CStringArray &TagsArray() { return m_Tags;};
    CDWordArray  &ItmsArray(int i) { return i==0 ? m_Itms1 : m_Itms2; };
    CWordArray   &IndsArray(int i) { return i==0 ? m_Inds1 : m_Inds2; };

    CString &Tag(int i) { return m_Tags[i];};
    DWORD   &Itm(int i, int j) { return i==0 ? m_Itms1[j] : m_Itms2[j]; };
    WORD    &Ind(int i, int j) { return i==0 ? m_Inds1[j] : m_Inds2[j]; };

    void TagsAdd(char * s) { m_Tags.SetAtGrow(m_nTags++, s); };
    void TagsAdd(CString & s) { m_Tags.SetAtGrow(m_nTags++, s); };
    void ItmsAdd(int i, DWORD d) { if (i==0) m_Itms1.SetAtGrow(m_nItms1++, d); else m_Itms2.SetAtGrow(m_nItms2++, d); };
    void IndsAdd(int i, WORD d) { if (i==0) m_Inds1.SetAtGrow(m_nInds1++, d); else m_Inds2.SetAtGrow(m_nInds2++, d); };

    int NItms1() { return m_nItms1; };
    int NItms2() { return m_nItms2; };
    int NInds1() { return m_nInds1; };
    int NInds2() { return m_nInds2; };
    int NTags()  { return m_nTags;  };

 };

//===========================================================================
//
//
//
//===========================================================================

#if dbgKeepMsgHistory
const long MaxMsgHistory=1024;
struct MsgHistoryRec
  {
  UINT       Cmd;
  CExecObj * pEO;
  CTimeValue Time;
  };
#endif

//===========================================================================
//
//
//
//===========================================================================

const DWORD EXF_Stopped        = 0x00000001;
const DWORD EXF_Running        = 0x00000002;
const DWORD EXF_Starting       = 0x00000004;
const DWORD EXF_Stopping       = 0x00000008;
const DWORD EXF_Building       = 0x00000010;
const DWORD EXF_Updating       = 0x00000020;
const DWORD EXF_Waiting        = 0x00000040;
const DWORD EXF_Paused         = 0x00000080;
const DWORD EXF_SolvingPB      = 0x00000100;
const DWORD EXF_Busy           = 0x00000200;
const DWORD EXF_BetweenSteps   = 0x00000400;
const DWORD EXF_StartError     = 0x00000800;
const DWORD EXF_QueryTime      = 0x00001000;
const DWORD EXF_Reading        = 0x00002000;
const DWORD EXF_Writing        = 0x00004000;
const DWORD EXF_PreStart       = 0x00008000;
const DWORD EXF_ReadingAll     = 0x00002000;
const DWORD EXF_WritingAll     = 0x00004000;

const long MaxMvAvgPts=100;

enum ExecCoupling { XC_None, XC_ASync, XC_Sync };

enum ExecFiling { EF_Null, EF_SaveDBReqd, EF_LoadDBReqd, EF_SaveSnapReqd, EF_LoadSnapReqd, EF_SaveScenarioReqd, EF_LoadScenarioReqd };

class DllImportExport CXStopWatches
  {
  protected:
    CExecutive &X;   
    bool   m_bStepStarted;
    dword  m_nNoOfProcessors;
    double m_dProcTimePrv;
  public:
    CStopWatch  m_swStart,
                m_swQuery,
                m_swBuild,
                m_swExec,
                m_swExecDn,
                m_swRead,
                m_swWrite,
                m_swStop,
                m_swStep,
                m_swRun;
    
    CTimeValue m_dSimulatedTime;
    double m_dStartingTime;

    double m_dStepTimeI[MaxMvAvgPts];
    double m_dStepTimeTtl;

    double m_dMultiplierI[MaxMvAvgPts];
    double m_dMultiplierTtl;

    double m_dProcUseI[MaxMvAvgPts];
    double m_dProcUseTtl;
    
    long   m_nStepCnt;
    long   m_nMvAvgCnt;
    long   m_nMvAvgPts;

    double m_dProcTime;
    
    bool   m_bLastStep;

    CXStopWatches(CExecutive * pX);

    CXStopWatches & operator=(CXStopWatches & SW);

    void      Clear(bool All);
    double    TotalSecs();
    void      StepBegin();
    void      StepEnd();

    double StepTimeInst();
    double StepTimeAvg();
    double StepTimeMvAvg();
    double MultiplierInst();
    double MultiplierAvg();
    double MultiplierMvAvg();
    double ProcUseInst();
    double ProcUseAvg();
    double ProcUseMvAvg();

  };

//===========================================================================
//-------------------------------------------------------------------------

//class to retrieve general "report" info from model DLL's...
class DllImportExport CMdlGeneralInfo
  {
  public:
    int iGroupIndex;
    int iItemIndex;
    WORD bInitRequest:1,
         bStringData:1;
    CArray <Strng, Strng&> Headings;
    Strng sItem;
    double dItem;

  public:
    CMdlGeneralInfo();
    virtual ~CMdlGeneralInfo();
    virtual bool GetInfo() = 0;
    void Init();
    static CMdlGeneralInfo* First() { return pFirst; };
    CMdlGeneralInfo* Next() { return pNext; };

  private:
    static CMdlGeneralInfo* pFirst;
    CMdlGeneralInfo* pNext;

  };

//===========================================================================

class DllImportExport CExecSeqItem
  {
  public:
    long        m_lSeqNo;
    Strng       m_sTag;
    COleVariant m_vtValue;
    Strng       m_sCnvTxt;
  };

 
class DllImportExport CExecSequence
  {
  protected:
    CExecutive         &m_Exec;
    long                m_lSeqNo;
    CCriticalSection    m_Sect;
    bool                m_HoldPlay;            
    
    CList <CExecSeqItem, CExecSeqItem&> m_ReplayList;
    
  public:
    enum {VSS_Access, VSS_Trend, VSS_COM, VSS_DDE, VSS_Script, VSS_SnapFix, VSS_Replay};
    enum {VSC_ClearTo, VSC_ClearAll, VSC_PlayTo, VSC_PlayAll, VSC_SetHold};
    
    CExecSequence(CExecutive * pExec);
    
    long SequenceNo() { return m_lSeqNo; };
    void SetSequenceNo(long SeqNo);
    long IncSequenceNo(long Inc=1);

    void PlayTheList(long Up2SeqNo);
    void ClearTheList(long Up2SeqNo); 

    void VariableSetReplayItem(long SeqNo, LPCTSTR Tag, const VARIANT Value, LPCTSTR CnvTxt);
    void VariableSetReplayCtrl(long Cmd, long Param);

    void NoteManSet(long Source, LPCTSTR Tag, double Value, LPCTSTR CnvTxt="");
    void NoteManSet(long Source, LPCTSTR Tag, long Value, LPCTSTR CnvTxt="");
    void NoteManSet(long Source, LPCTSTR Tag, LPCTSTR Value, LPCTSTR CnvTxt="");

  };

//===========================================================================

typedef union
  {
  long lParam;
  struct{
    long bHoldAdv:1,
         lWaitForNextMS:16;
    };
  } CDoOneStepInfo;

class CLoadDBInfo
  {
  public:
    CLoadDBInfo(long DBFmt, DWORD DBOpts, LPTSTR pRoot) :
      m_FCB(pRoot, DefaultCfgLabel, TOF_DBase)
      {
      m_pRoot      = pRoot;
      m_RtFolder   = pRoot;
      m_RtFolder.FnClearEndBSlash();
      m_RtFolder.FnCompactPath(50);

      m_DBFmt     = DBFmt;
      m_DBOpts    = DBOpts;
      m_AllLoaded = false;
      m_Cancelled = false;
      m_Failed    = false;
      };

  public:
    LPTSTR                m_pRoot;
    long                  m_DBFmt;
    DWORD                 m_DBOpts;

    CTagDBase             m_TagDB;
    FilingControlBlock    m_FCB;

    CStopWatch            m_LoadTime;
    Strng                 m_RtFolder;
    flag                  m_AllLoaded;
    flag                  m_Cancelled;
    flag                  m_Failed;
    CArray <flag, flag>   m_LoadIt;
  };

enum eLoadDBPasses {eLdDB_Open, eLdDB_Create, eLdDB_Connect, eLdDB_Load, eLdDB_LoadOther, eLdDB_Close };

class DllImportExport CExecutive : public ConditionBlk  
  {
  friend class ObjTagIndex;
  friend class CExecObj;
  friend class CExecSequence;
  friend class CMdlRunManager;
  friend class CAppRunManager;
  friend class FlwSlvDyn;

  public:

    enum RqdRunMode { RRM_Current, RRM_PBMd, RRM_DynMd/*, RRM_DynMdFlow, RRM_DynMdFull*/ };
    enum UpdateOps  { UO_Reset, UO_Start, UO_StartDone, UO_Step, UO_Stop, UO_StopAgain  };

  public:
    CExecutive();
    ~CExecutive();

    void           Initialise();
    void           Terminate();
    bool           Initialised() { return pExecThread!=NULL; };

    void           Initialise4Probal();
    void           Initialise4Dynamic();

    flag           Register(CExecObj *pEO, flag FixingRegistrations=0);
    flag           DeRegister(CExecObj *pEO, flag FixingRegistrations=0);
    flag           FixRegistrations();

    int            ChangeTag(pchar pOldTag, pchar pNewTag);
    int            DeleteTags(Strng_List & Tags);
#if WithNumericTags
    int            CheckAllTags(flag NewNumTagsBad, flag NewNumStartingBad, flag Prompt=1);
#else
    int            CheckAllTags(flag NewNumStartingBad, flag Prompt=1, flag ForceTest=0);
#endif
    int            SCInsertNodeG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR path,
                               LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect, 
                               float angle, const CRectangleF & tagArea, float tagAngle, bool tagVisible, COLORREF Colour, 
                               bool mirrorX, bool mirrorY);
    int            SCDeleteNodeG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR path);
    int            SCModifyNodeG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR path,
                               LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect, 
                               float angle, const CRectangleF & tagArea, float tagAngle, bool tagVisible, COLORREF Colour, 
                               bool mirrorX, bool mirrorY);
    int            SCInsertNodeM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR model);
    int            SCDeleteNodeM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid);
    int            SCModifyNodeM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR model);


    int            SCInsertLinkG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/,
                                LPCSTR model, 
                                LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                LPCSTR OriginTag, LPCSTR DestinationTag, 
                                CPointFList & ControlPoints, 
                                const CRectangleF & tagArea, float tagAngle, bool tagVisible);
    int            SCDeleteLinkG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/);
    int            SCModifyLinkG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/,
                                LPCSTR model, 
                                LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                CPointFList & ControlPoints, 
                                const CRectangleF & tagArea, float tagAngle, bool tagVisible);
    int            SCInsertLinkM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/,
                                LPCSTR model, 
                                LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                LPCSTR OriginTag, LPCSTR DestinationTag, 
                                LPCSTR OriginPort, LPCSTR DestinationPort);
    int            SCDeleteLinkM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/);
    int            SCModifyLinkM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/,
                                LPCSTR model, 
                                LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                LPCSTR OriginTag, LPCSTR DestinationTag, 
                                LPCSTR OriginPort, LPCSTR DestinationPort);

    void           ClearThread() { LockExec();  pExecThread=NULL; UnlockExec(); };
    DWORD          ExecThreadId() { return pExecThread->m_nThreadID; };

    flag           StartThreads(byte ThrdFlag, UINT Cmd, word Extra=0);
    LONG           PostCmd(CExecObj *pSrcEO, word Cmd, LONG lParam=0, CExecObj *pEO=NULL, word Extra=0);
    void           UpdateDisplays(byte Options=0);
    void           UpdateStateDisplay(DWORD StateOn, DWORD StateOff)
      {
      dwState|=StateOn;
      dwState&=~StateOff;
      UpdateDisplays(UDO_OnlyStatusLine);
      }
    void           SetStopTimeEtc(bool ForceIt);
    void           UpdateState(UpdateOps Op, eScnState RqdManState, eScnState RqdScnState);
    void           DoOneExecMessage(UINT wParam, LONG lParam);
    void           LowerPriority();

    void           DoTimer();
    void           SetAppIdleDone();
    void           SetStopSleep();

    ExecCoupling   Coupling();
    ExecCoupling   SetCoupling(ExecCoupling XC);

    void           SetDefNetMode(long Mode);
    void           SetDefNetMode(LPCTSTR Mode);

    flag           Stopped();
    flag           Starting();
    flag           StartError();
    flag           Running();
    flag           Stopping();
    flag           Busy();
    flag           SolvingPB();
    flag           Waiting();
    flag           Paused();
    flag           BetweenSteps();
    flag           AutomationBusy();
    flag           Filing();
    flag           SyncWithClock();
    flag           RealTime();
    double         RealTimeMult();
    //long           MaxRunSteps();
    void           SetSyncWithClock(flag SyncWithClock_);
    void           SetRealTime(flag RealTime_);
    void           SetRealTimeMult(double M);
    flag           SetTheTime(CTimeValue Time_, LPCTSTR Where, bool Rewind=false);
    DWORD          LoopCount();

    flag           ResetRunState(bool Reset, LPARAM lParam);

    LPTSTR         getHstTaglistFolder()                      { return m_sHstTaglistFolder(); };
    void           putHstTaglistFolder(LPTSTR Str)            { m_sHstTaglistFolder=Str; };
    LPTSTR         getHstTaglistFile()                        { return m_sHstTaglistFilename(); };
    void           putHstTaglistFile(LPTSTR Str)              { m_sHstTaglistFilename=Str; };
    
    flag           getResetTimeOnStart()                      { return m_ResetTimeOnStart; };
    void           putResetTimeOnStart(flag O)                { m_ResetTimeOnStart=O; };
    flag           getRestartHstOnStart()                     { return m_RestartHstOnStart; };
    void           putRestartHstOnStart(flag O)               { m_RestartHstOnStart=O; };
    flag           getSetHstTaglistOnStart()                  { return m_SetHstTaglistOnStart; };
    void           putSetHstTaglistOnStart(flag O)            { m_SetHstTaglistOnStart=O; };
    flag           getRestartArchiveOnStart()                 { return m_RestartArchiveOnStart; };
    void           putRestartArchiveOnStart(flag O)           { m_RestartArchiveOnStart=O; };

    flag           getRestartPGMOnStart()                     { return m_RestartPGMOnStart; };
    void           putRestartPGMOnStart(flag O)               { m_RestartPGMOnStart=O; };
    flag           getRestartPrfOnStart()                     { return m_RestartPrfOnStart; };
    void           putRestartPrfOnStart(flag O)               { m_RestartPrfOnStart=O; };

    eScnState      getScnState()                              { return m_TmCB.m_ScnState; };
    void           putScnState(eScnState On)                  { m_TmCB.m_ScnState=On; };
    eScnContRest   getManualScnReStart()                      { return m_TmCB.m_ManualScnReStart; };
    void           putManualScnReStart(eScnContRest On)       { m_TmCB.m_ManualScnReStart=On; };
    eScnContRest   getReStartIfScnBusy()                      { return m_TmCB.m_ReStartIfScnBusy; };
    void           putReStartIfScnBusy(eScnContRest On)       { m_TmCB.m_ReStartIfScnBusy=On; };
    eScnBlkRest    getReStartIfScnComplete()                  { return m_TmCB.m_ReStartIfScnComplete; };
    void           putReStartIfScnComplete(eScnBlkRest On)    { m_TmCB.m_ReStartIfScnComplete=On; };

    eScnTypes      getScnType()                               { return m_TmCB.m_ScnType; };
    void           SetScnType(eScnTypes T, bool ResetState);
    eScnTypes      getScnTypeLast()                           { return m_ScnTypeLast; };
    void           putScnTypeLast(eScnTypes T)                { m_ScnTypeLast=T; };
    eScnTermActions getScnTermAction()                        { return m_TmCB.m_TermAction; };
    void           putScnTermAction(eScnTermActions A)        { m_TmCB.m_TermAction=A; };

    CTimeValue     getTheTime()                               { return m_TmCB.m_TheTime;};
    //void           putTheTime(const CTimeValue Value);
    CTimeValue     getTimeStarted()                           { return ValidTime(m_TimeStarted) ? m_TimeStarted : m_TmCB.m_TheTime;};
    void           putTimeStarted(const CTimeValue Value)     { m_TimeStarted = Value;} 
    CTimeValue     getTimeStopped()                           { return ValidTime(m_TimeStopped) ? m_TimeStopped : m_TmCB.m_TheTime;};
    void           putTimeStopped(const CTimeValue Value)     { m_TimeStopped = Value;} 
    CTimeValue     getStepSize()                              { return m_TmCB.m_StepSize;};
    void           putStepSize(const CTimeValue Value)        { m_TmCB.m_StepSize=Value;}
    CTimeValue     getStepSizeNxt()                           { return m_TmCB.m_StepSizeNxt;};
    void           putStepSizeNxt(const CTimeValue Value)     { m_TmCB.m_StepSizeNxt=Value;}
    CTimeValue     getStepSizeMax()                           { return m_TmCB.m_StepSizeMax;};
    void           putStepSizeMax(const CTimeValue Value)     { m_TmCB.m_StepSizeMax=Value;}
    CTimeValue     getTimeAtStart()                           { return m_dTimeAtStart; };
    void           putTimeAtStart(CTimeValue TimeAtStart)     { m_dTimeAtStart=TimeAtStart; };
    CTimeValue     getTimeToStop()                            { return m_TmCB.m_TimeToStop; };
    void           putTimeToStop(CTimeValue T)                { m_TmCB.m_TimeToStop=T; };
    CTimeValue     getScnDuration()                           { return m_TmCB.m_ScnDuration; };
    void           putScnDuration(CTimeValue Duration)        { m_TmCB.m_ScnDuration=Duration; };
    CTimeValue     getScnStopTime()                           { return m_TmCB.m_ScnStopTime; };
    void           putScnStopTime(CTimeValue StopTime)        { m_TmCB.m_ScnStopTime=StopTime; };

    long           getStepCount()                             { return m_TmCB.m_StepCount; };
    void           putStepCount(long N)                       { m_TmCB.m_StepCount=N; };
    long           getStepCountMax()                          { return m_TmCB.m_StepCountMax; };
    void           putStepCountMax(long N)                    { m_TmCB.m_StepCountMax=N; };
    CTimeValue     getScnTime()                               { return m_TmCB.m_ScnTime; };
    void           putScnTime(CTimeValue T)                   { m_TmCB.m_ScnTime=T; };
    CTimeValue     getScnTimeMax()                            { return m_TmCB.m_ScnTimeMax; };
    void           putScnTimeMax(CTimeValue T)                { m_TmCB.m_ScnTimeMax=T; };


    double         getDynProgress()                           { return m_DynProgress; };
    void           putDynProgress(double D)                   { m_DynProgress=D; };
    double         getClock()                                 { return m_Clock.Secs(); };
    double         getClockEstToGo()                          { return m_ClockEstToGo; };
    double         getClockEstTotal()                         { return m_ClockEstTotal; };

    __declspec(property(get=getScnState,              put=putScnState))               eScnState     ScnState;
    __declspec(property(get=getManualScnReStart,      put=putManualScnReStart))       eScnContRest  ManualScnReStart;
    __declspec(property(get=getReStartIfScnBusy,      put=putReStartIfScnBusy))       eScnContRest  ReStartIfScnBusy;
    __declspec(property(get=getReStartIfScnComplete,  put=putReStartIfScnComplete))   eScnBlkRest   ReStartIfScnComplete;

    __declspec(property(get=getScnType))                             eScnTypes       ScnType;
    __declspec(property(get=getScnTypeLast,   put=putScnTypeLast))   eScnTypes       ScnTypeLast;
    __declspec(property(get=getScnTermAction, put=putScnTermAction)) eScnTermActions ScnTermAction;
    __declspec(property(get=getScnDuration,   put=putScnDuration))   CTimeValue      ScnDuration;
    __declspec(property(get=getScnStopTime,   put=putScnStopTime))   CTimeValue      ScnStopTime;
    __declspec(property(get=getTheTime))                             CTimeValue      TheTime;
    __declspec(property(get=getTimeToStop,    put=putTimeToStop))    CTimeValue      TimeToStop;
    __declspec(property(get=getScnTime,       put=putScnTime))       CTimeValue      ScnTime;
    __declspec(property(get=getScnTimeMax,    put=putScnTimeMax))    CTimeValue      ScnTimeMax;
    __declspec(property(get=getTimeStarted,   put=putTimeStarted))   CTimeValue      TimeStarted;
    __declspec(property(get=getTimeStopped,   put=putTimeStopped))   CTimeValue      TimeStopped;
    __declspec(property(get=getStepSize,      put=putStepSize))      CTimeValue      StepSize;
    __declspec(property(get=getStepSizeNxt,   put=putStepSizeNxt))   CTimeValue      StepSizeNxt;
    __declspec(property(get=getStepSizeMax,   put=putStepSizeMax))   CTimeValue      StepSizeMax;
    __declspec(property(get=getTimeAtStart,   put=putTimeAtStart))   CTimeValue      TimeAtStart;
    __declspec(property(get=getStepCount,     put=putStepCount))     long            StepCount;
    __declspec(property(get=getStepCountMax,  put=putStepCountMax))  long            StepCountMax;

    __declspec(property(get=getResetTimeOnStart,put=putResetTimeOnStart))         flag     ResetTimeOnStart;
    __declspec(property(get=getHstTaglistFolder,put=putHstTaglistFolder))         LPTSTR   HstTaglistFolder;
    __declspec(property(get=getHstTaglistFile,put=putHstTaglistFile))             LPTSTR   HstTaglistFile;
    __declspec(property(get=getRestartHstOnStart,put=putRestartHstOnStart))       flag     RestartHstOnStart;
    __declspec(property(get=getSetHstTaglistOnStart,put=putSetHstTaglistOnStart)) flag     SetHstTaglistOnStart;
    __declspec(property(get=getRestartArchiveOnStart,put=putRestartArchiveOnStart)) flag   RestartArchiveOnStart;

    __declspec(property(get=getDynProgress,put=putDynProgress))     double      DynProgress;
    __declspec(property(get=getClock))                              double      Clock;
    __declspec(property(get=getClockEstToGo))                       double      ClockEstToGo;
    __declspec(property(get=getClockEstTotal))                      double      ClockEstTotal;

    long           SequenceNo()                               { return m_Seq.SequenceNo(); };
    void           SetSequenceNo(long SeqNo)                  { m_Seq.SetSequenceNo(SeqNo); };

    CTimeValue     QueryCurrentTime();


    flag           TestRunning();

    flag           ScnStartupRequired();
    flag           ScnStepAllowed();

    void           GotoEditMode();
    void           GotoRunMode(RqdRunMode Md, int Action);
    void           GotoIdleMode();
    void           DoOneStep(CDoOneStepInfo Info);
    void           SetSolveTool(SolveToolBase * SolveTool);
    //void           SwapToProBal();
    //void           ReturnFromProBal();
    void           RequestReadWriteAll();
    void           BeginBulkChange();
    void           EndBulkChange();

    void           OpenSolver();

    int            CanClose(Strng_List & Problems);

    void           LockExec()
      { 
      InterlockedIncrement(&m_nExecLockWaiting); 
      EnterCriticalSection(&csExecLock); 
      InterlockedDecrement(&m_nExecLockWaiting); 
      };
    BOOL           LockExecWaiting()
      { 
      return InterlockedCompareExchange(&m_nExecLockWaiting, 0,0)>0; 
      };
    void           UnlockExec()
      { 
      LeaveCriticalSection(&csExecLock); 
      };

    int            SaveDBase(long DBFmt, DWORD DBOpts, char* pRoot, char* pOldPrjFiles);
    int            LoadDBase(eLoadDBPasses Pass, CLoadDBInfo & LDBI);
    int            SaveSnapShot(char * pRoot, char * pSnapShotName, CExecObjArray *ObjList=NULL);
    int            LoadSnapShot(char * pRoot, char * pSnapShotName);
    int            SaveScenario(char * pRoot, char * pScenarioName, CExecObjArray *ObjList=NULL);
    int            LoadScenario(char * pRoot, char * pScenarioName);
    int            CompareScenarios(CCompareInfo& Info);
    int            CheckMerge(CMergeProjectsInfo& MPI);
    int            PrepareMerge(CMergeProjectsInfo& MPI);
    int            DoMergeA(CMergeProjectsInfo& MPI); //stage one
    int            DoMergeB(CMergeProjectsInfo& MPI); //stage two
    void           DoAppActivate();
    void           OnAppActivate(BOOL bActive);
    flag           RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info);

    void           DoSetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags);
    void           SetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags);
    flag           GetModelAction(LPCTSTR Tag, CMdlActionArray & Acts);
    flag           SetModelAction(LPCTSTR Tag, CMdlAction & Act);

    flag           GetModelGraphic(LPCTSTR Tag, CMdlGraphicArray & Grfs);
    flag           OperateModelGraphic(LPCTSTR Tag, CMdlGraphicWnd & Wnd, CMdlGraphic & Grf);

    flag           SteadyStateRqd() { return m_SteadyStateRqd; };

    void           ConfigureArchive()    { DoConfigureArchive(); };
    void           ConfigureHistorian()  { DoConfigureHistorian(); };
    bool           KeepHistoryFile(char* pFilename);
    
    UINT           TimerId() { return iTimerId; };
    flag           DoingPB() { return m_fDoingPB; };
    DWORD          State() { return dwState; };
    int            StateIndex();
    char*          StateDesc(int index);
    char*          StateDesc();

  private:
    flag           QueryStartExec();
    void           ClearDataLists();
    void           DoConfigureArchive();
    void           DoConfigureHistorian();
    void           BuildDataLists();
    void           UpdateDataLists();

    int            DoSaveDBase(long DBFmt, DWORD DBOpts, char* pRoot, char* pOldPrjFiles);
    int            DoLoadDBaseOpen(CLoadDBInfo & LDBI);
    int            DoLoadDBaseCreate(CLoadDBInfo & LDBI);
    int            DoLoadDBaseConnect(CLoadDBInfo & LDBI);
    int            DoLoadDBaseLoad(CLoadDBInfo & LDBI);
    int            DoLoadDBaseLoadOther(CLoadDBInfo & LDBI);
    int            DoLoadDBaseClose(CLoadDBInfo & LDBI);
    int            DoSaveSnapShotScenario(flag DoSnapShot, char* pRoot, char* pSnapShotName, CExecObjArray *ObjList=NULL);
    int            DoLoadSnapShotScenario(flag DoSnapShot, char* pRoot, char* pSnapShotName);
    int            CompareData(CCompareInfo& Info, CCompareWorkSpace &WS, char* pTag, CXMsgLst& XM1, CXMsgLst& XM2);
    int            IndexMessage(CPkDataList* pDList, int MsgNo, CCompareWorkSpace &WS);//CDWordArray& Itms, CWordArray& Ind, CStringArray& Tags);

    void           PrepareMergeChangeTags(CMergeProjectsInfo& MPI);
  public:
    flag           XFindObject(CExecObj *pEO, char* pName, CXM_Route &Route, flag AllowSelf=false);
    inline flag    XFindObject(CExecObj *pEO, const char* pName, CXM_Route &Route, flag AllowSelf=false) { return XFindObject(pEO, (char*)pName, Route, AllowSelf); };
    DWORD          XSendMessage(CExecObj *pEO, CXMsgLst &XM, CXM_Route &Route, flag FromExecThread=false);

    flag           XBuildDataLists(CExecObj *pEO);
    flag           XBuildMyDataLists(CExecObj *pEO);
    DWORD          XReadTaggedItem(CExecObj *pEO, CXM_ObjectTag & ObjTag, CXM_ObjectData &ObjData, CXM_Route &Route, bool StopSleep=true);
    int            XWriteTaggedItem(CExecObj *pEO, CXM_ObjectData &ObjData, CXM_Route &rRoute);

    void           XGlblResultValidity(int IsValid=-1, int IsAlwaysValid=-1);
    void           DoGlblResultValidity();


    flag           SelfConnect() { return bSelfConnect; };
    void           SetSelfConnect(CExecObj *pClnt=NULL, CExecObj *pSrvr=NULL);

    void           XUpdateTagData(long IterCount=1) { lUpdateTagDataIterCount=IterCount+1; };

    int            SetHoldValidateData(flag On, flag WithLocks, flag MessagesOff/*=false*/);

    void           SetDBFormat(int Fmt) { m_DBFormat=Fmt; };
    int            DBFormat() { return m_DBFormat; };


    void           AddReportRequest(CXM_OleExcelReport* pRpt); 
    void           OneReportComplete();
    int            ProcessReportRequests();


  public:
    CEOExecReturn  ThrdRet[MaxXObjects];

    DEFINE_CI(CExecutive, ConditionBlk, 2);
    char *         CBTag() { return "Executive"; };

    CObjectCountBlk m_ObjCounts;

#if dbgKeepMsgHistory
    MsgHistoryRec MsgHistory[MaxMsgHistory];
    long lMsgHistory;
    void DumpMsgHistory();
#endif
  protected:
    CRITICAL_SECTION csExecLock;
    long             m_nExecLockWaiting;

    long           Ids;
    long           NObjs_Reg;            // No of Registered Objects

    CExecObjArray  Objs_Reg;             // Registered Objects
    CExecObjArray  Objs_Exec;            // Objects in Execution Order
    CExecObjArray  Objs_Solve;           // Objects in Solve Order
    CExecObjArray  Objs_Search;          // Objects in Search Order

    CExecObjArray  Objs_ToReg;           // Objects To register during next execution gap
    CExecObjArray  Objs_ToDeReg;         // Objects To Deregister during next execution gap
    
    CExecObj     * m_pSolver;            // A Shortcut to the solver at runtime

    DWORD          dwState;

    flag           bActive;
    flag           bStopReqd;
    flag           bStepReqd;
    flag           bPauseReqd;
    flag           bListRebuildReqd;
    flag           bListUpdateReqd;
    flag           bRegistrationListtoChange;
    flag           bSolveListRebuildReqd;
    flag           bHoldAdv;
    flag           bAutomationBusy;
    flag           m_bSendIdleEvent;
    flag           m_bSendStopEvent;
    flag           m_ReadWriteAllReqd;
    UINT           m_GotoCmdOnceStopped;
    flag           m_bStepExecuted;
    flag           m_SteadyStateRqd;

    flag           bSelfConnect;
    HANDLE         hPauseEvent;

    CWinThread    *pExecThread;

    long           lExecThreadsBusy;
    long           lExecThreadsDone;

    long           LastLowerPriority;

    CRITICAL_SECTION csTimerLock;
    flag           bTimerExpired, bWorkDone;
    UINT           iTimerId;
    HANDLE         m_hAppIdleDone;
    HANDLE         m_hStopSleep;
    DWORD          m_dwLoopCount;
    flag           m_fDoingPB;

    bool           m_bDidPauseWork;
    long           m_lPauseUpdateCount;
    long           m_lWaitCompleteCount;
    long           m_lWaitCompleteTime;

    SolveToolBase *pSolveTool;

    long           lUpdateTagDataIterCount;

    long           lLastTickCount;

    ExecCoupling   eCoupling;

    ExecFiling     eFileWhat;
    Strng          SaveLoadRoot;
    Strng          SaveLoadLabel;
    Strng          SaveLoadOldPrjFiles;
    Strng          SaveLoadSnapShotScenarioName;

    // Allow deferred activation - prevent lockup
    byte           bDoAppActivate;

    // Allow deferred SetModelState
    eScdMdlStateActs eRqdState;
    Strng_List     sRqdTags;

    // Allow deferred GlblResultValidity - prevent lockup
    flag           bDoGlblResultValidity;

    int            m_DBFormat;

    //CLDBStuff      m_LDB;

    Strng          m_sHstTaglistFolder;
    Strng          m_sHstTaglistFilename;
    flag           m_ResetTimeOnStart;
    flag           m_RestartHstOnStart;
    flag           m_SetHstTaglistOnStart;
    flag           m_RestartArchiveOnStart;
    flag           m_RestartPGMOnStart;
    flag           m_RestartPrfOnStart;
    flag           m_RestartEmptySpills;
    flag           m_RestartEmptyAll;
    flag           m_RestartResetStats;
    CTimeValue     m_dTimeAtStart;

    CTimeValue     m_TimeStarted; // Model Time Started
    CTimeValue     m_TimeStopped; // Model Time Stopped
    eScnTypes      m_ScnTypeLast;

    double         m_DynProgress;

    CStopWatch     m_Clock;
    double         m_ClockEstToGo;   // RealTime ToGo
    double         m_ClockEstTotal;  // RealTime Ttl

    CList<CXM_OleExcelReport*, CXM_OleExcelReport* > m_ReportList;
    long           m_nReportsToDo;
    HANDLE         m_hReportsDone;

  public:

    CXM_TimeControl m_TmCB;
    //CXM_TimeControl DynTmCB;
    //CXM_TimeControl PBTmCB;
    //CXM_TimeControl &gs_Exec.m_TmCB { return fDoingPB ? PBTmCB : DynTmCB; };

    EqnSolverBlk   m_EqnSlv;
    CtrlSequencer  m_CtrlSeq;
    CExecSequence  m_Seq;

    CXStopWatches  XSW, XSWDsp;


  };

//=========================================================================

inline void   CExecutive::SetAppIdleDone()              { ::SetEvent(m_hAppIdleDone); };
inline void   CExecutive::SetStopSleep()                { ::SetEvent(m_hStopSleep); };

inline flag   CExecutive::Starting()                    { return (dwState & EXF_Starting)!=0;};
inline flag   CExecutive::StartError()                  { return (dwState & EXF_StartError)!=0;};
inline flag   CExecutive::Running()                     { return (dwState & EXF_Running)!=0;};
inline flag   CExecutive::Stopping()                    { return (dwState & EXF_Stopping)!=0;};
inline flag   CExecutive::Busy()                        { return (dwState & EXF_Busy)!=0;}; //starting OR running OR stopping OR paused
inline flag   CExecutive::SolvingPB()                   { return (dwState & EXF_SolvingPB)!=0;}; //in ProBal mode
inline flag   CExecutive::Waiting()                     { return (dwState & EXF_Waiting)!=0;};
inline flag   CExecutive::Paused()                      { return (dwState & EXF_Paused)!=0;};
inline flag   CExecutive::BetweenSteps()                { return (dwState & EXF_BetweenSteps)!=0;};
inline flag   CExecutive::AutomationBusy()              { return bAutomationBusy;};
inline flag   CExecutive::Filing()                      { return (eFileWhat!=EF_Null);};
inline flag   CExecutive::SyncWithClock()               { return m_TmCB.m_bSyncWithClock;};
inline flag   CExecutive::RealTime()                    { return m_TmCB.m_bRealTime;};
inline double CExecutive::RealTimeMult()                { return m_TmCB.m_RealTimeMult;};
//inline long   CExecutive::MaxRunSteps()                 { return gs_Exec.m_TmCB.m_nMaxSteps;};
inline DWORD  CExecutive::LoopCount()                   { return m_dwLoopCount; };

inline flag   CExecutive::Stopped()                     { return (dwState & EXF_Busy)==0;}; //starting OR running OR stopping OR paused

//=========================================================================

extern DllImportExport CExecutive gs_Exec;

//inline CXM_TimeControl & PBTmCB()  { return gs_Exec.PBTmCB; };
//inline CXM_TimeControl & DynTmCB() { return gs_Exec.DynTmCB; };
//inline CXM_TimeControl & TimeCB()  { return gs_Exec.TimeCB(); };

//inline EqnSlvCtrlBlk & PBEqnCB()  { return gs_Exec.m_TmCB.m_EqnCB; };
//inline EqnSlvCtrlBlk & DynEqnCB() { return gs_Exec.DynTmCB.m_EqnCB; };
//inline EqnSlvCtrlBlk & EqnCB()    { return gs_Exec.m_TmCB.m_EqnCB; };

extern DllImportExport CXM_TimeControl & gs_TimeCB;
extern DllImportExport EqnSlvCtrlBlk   & gs_EqnCB;
extern DllImportExport EqnSolverBlk    & gs_EqnSlv;
extern DllImportExport CtrlSequencer   & gs_CtrlSeq;

//#define gs_TimeCB   gs_Exec.m_TmCB
//#define gs_EqnCB    gs_Exec.m_TmCB.m_EqnCB
//#define gs_EqnSlv   gs_Exec.m_EqnSlv
//#define gs_CtrlSeq  gs_Exec.m_CtrlSeq

//inline EqnSolverBlk &  EqnSlv()   { return gs_Exec.m_EqnSlv; };
//inline CtrlSequencer &  CtrlSeq() { return gs_Exec.m_CtrlSeq; };
#if ALLOWMASSSMOOTH
inline flag MassSmoothing()       { return EqnCB().Cfg.fMassSmooth; };
inline flag DoMassSmoothing()     { return MassSmoothing() && (EqnCB().NIters()>2); };
#else
inline flag MassSmoothing()       { return false; };
inline flag DoMassSmoothing()     { return false; };
#endif
inline flag XStarting()           { return gs_Exec.Starting();     };
inline flag XRunning()            { return gs_Exec.Running();      };
inline flag XStopping()           { return gs_Exec.Stopping();     };
inline flag XBusy()               { return gs_Exec.Busy();         };  //starting OR running OR stopping OR paused
inline flag XStopped()            { return gs_Exec.Stopped();      };  //starting OR running OR stopping OR paused
inline flag XSolvingPB()          { return gs_Exec.SolvingPB();    };  //in ProBal mode
inline flag XWaiting()            { return gs_Exec.Waiting();      };
inline flag XPaused()             { return gs_Exec.Paused();       };
inline flag XBetweenSteps()       { return gs_Exec.BetweenSteps(); };
inline flag XFiling()             { return gs_Exec.Filing();       };
inline flag XDoingPB()            { return gs_Exec.DoingPB();      };

inline void XGotoEditMode()       { gs_Exec.GotoEditMode();        };
inline void XGotoRunMode(CExecutive::RqdRunMode Md, int Action) { gs_Exec.GotoRunMode(Md, Action); };
inline void XGotoIdleMode()       { gs_Exec.GotoIdleMode();        };
inline void XDoOneStep(CDoOneStepInfo Info) { gs_Exec.DoOneStep(Info); };
inline void XSetSolveTool(SolveToolBase * SolveTool) { gs_Exec.SetSolveTool(SolveTool); };
inline void XRequestReadWriteAll() { gs_Exec.RequestReadWriteAll(); };

inline void XSetDBFormat(int Fmt) { gs_Exec.SetDBFormat(Fmt);      };
inline int  XDBFormat()           { return gs_Exec.DBFormat();     };

inline CXStopWatches  & XSW(bool ForDisp=true) { return ForDisp ? gs_Exec.XSWDsp : gs_Exec.XSW; };

inline void XUpdateDisplays(byte Options=0) { gs_Exec.UpdateDisplays(Options); };
//inline void XSetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags) { gs_Exec.SetModelState(RqdState, RqdTags); };

//=========================================================================

extern DllImportExport void XFlushAppMsgQ();
extern DllImportExport void XDoAppIdle();

//=========================================================================

typedef void (*EventLogHookFn)(LPCTSTR Tag, LPCTSTR Msg);

class DllImportExport CEventLog
  {
  friend class FlwSlvDyn;
  public:
    CEventLog();
    ~CEventLog();
    void SetHook(EventLogHookFn Hook);
    void LogEvent(LPCTSTR Tag, LPCTSTR Msg);
  private:
      
    EventLogHookFn m_EventLogHook;

  };

extern DllImportExport CEventLog gs_EventLog;

//=========================================================================

#undef DllImportExport

#endif
