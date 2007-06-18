#pragma once
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "sc_defs.h"
#include "pgm_e.h"
//#include "flwnode.h"

#ifdef __NDXREFS_CPP
#define DllImportExport DllExport
#elif !defined(FLWLIB)
#define DllImportExport DllImport
#else
#define DllImportExport
#endif

//===========================================================================
//
//
//
//===========================================================================

//files used flags...
const byte FU_CopyFile  = 0x01;
const byte FU_EditTxt   = 0x02;
const byte FU_EditExcel = 0x04;
const byte FU_EditMDB   = 0x08;

class FilesUsedItem
  {
  public:
    CString        Name;
    byte           FUFlags;

    FilesUsedItem()
      {
      FUFlags=0;
      };
    FilesUsedItem(char *pName, byte FilesUsedFlags)
      {
      Name=pName;
      FUFlags=FilesUsedFlags;
      };
    inline flag CopyFile()  { return (FUFlags & FU_CopyFile)!=0; };
    inline flag EditTxt()   { return (FUFlags & FU_EditTxt)!=0; };
    inline flag EditExcel() { return (FUFlags & FU_EditExcel)!=0; };
    inline flag EditMDB()   { return (FUFlags & FU_EditMDB)!=0; };
  };

class CFilesUsedArray : public CArray <FilesUsedItem, FilesUsedItem &>
  {
  public:
    void AddFile(char *pName, byte FilesUsedFlags=FU_CopyFile)
      {
      FilesUsedItem Item(pName, FilesUsedFlags);
      Add(Item);
      }
  };


//===========================================================================
//
//
//
//===========================================================================

class FlwNode;

//===========================================================================
//
//
//
//===========================================================================


const long NPM_InitialiseSolution    = 0x00000001; 
const long NPM_TerminateSolution     = 0x00000002; 
const long NPM_StartStep             = 0x00000004; 
const long NPM_BeforeEvalProducts    = 0x00000008;  
const long NPM_AfterEvalProducts     = 0x00000010;  

const long NPM_Any                   = -1;  

#if WITHNODEPROCS

class DllImportExport CNodeProcedures
  {
  public:
    CNodeProcedures(FlwNode * pNd);
    ~CNodeProcedures();

  //protected:

    //class GControl : public FlwNode//, GCXRefHelper
    //  {
    //  public:
    //    GControl(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    //    virtual ~GControl();
    void            ResetData(flag Complete);
    //
    //  private:
    void            SetMaxVarData(long NewSize);
    void            DDBAddWatchedVars(DataDefnBlk& DDB, char* pHdTag, pGCVar VarList, int Lvl, int &nPg, int &nOnPg, flag ForceNewPage, int MaxPerPage);
    void            DDBAddArrayWatchedVars(DataDefnBlk & DDB, int &nPg, int MaxPerPage);
    void            CountWatchedVars(pGCVar VarList, int & Count);
    flag            LoadProc(char* pProcName, flag WithDebug);
    //
    //  public:
    void            BuildDataDefn(DataDefnBlk & DDB);
    flag            DataXchg(DataChangeBlk & DCB);
    flag            ValidateData(ValidateDataBlk & VDB);
    flag            PreStartCheck();
    //    virtual void   EvalCtrlInitialise(eScdCtrlTasks Tasks=CO_All);
    //    virtual void   EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All); 
    void            DoEvalProc(long ProcIDMask, bool DoGet, bool DoSet); 
    //    void           EvalCtrlTerminate(eScdCtrlTasks Tasks=CO_All);
    //    virtual void   SetState(eScdMdlStateActs RqdState);
    void		        DoTextFileChangeTag(Strng fn, pchar pOldTag, pchar pNewTag, int& TagChangeCount, bool lookInComments, bool listChanges);
    //
    flag            GetOtherData(FilingControlBlock &FCB);
    flag            PutOtherData(FilingControlBlock &FCB);
    
    void            OnAppActivate(BOOL bActive);
    int             FilesUsed(CFilesUsedArray & Files);
    int             ChangeTag(pchar pOldTag, pchar pNewTag);
    int             DeleteTag(pchar pDelTag);
    //
    //    // CNodeXRefMngr Overides
    //    virtual bool   TestXRefListActive();
    //    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    //    virtual void   UnlinkAllXRefs();
    //
    //    virtual dword  ModelStatus();
    //
    //    DEFINE_CI(GControl, FlwNode, 6+MaxCondMsgs+MaxCondMsgs);
//
    //
    //
    //


    flag            On() { return m_bOn; } 


    // Temporary
  //protected:
    FlwNode       * m_pNd;

    Strng           m_sProcName;             //name of pgm file
    Strng           m_sProcFile;             //temp name of pgm file
    Strng           m_sProcPath;             //temp path of pgm file
    GCInsMngr       m_ProcMngr;              //Proc manager

    GCVar**         m_VarData;              //pointer to array of data vars (external refrences)
    int             m_nVarData;             //number of data vars
    long            m_nMaxVarData;          //current max length of array of data vars
    CCriticalSection m_VarDataSect;

    HANDLE          m_hProcess[TP_MaxTtlIncludeFiles];             //process handle for the editor
    DWORD           m_dwProcessId[TP_MaxTtlIncludeFiles];          //process Id for the editor
    FILETIME        m_EditTime[TP_MaxTtlIncludeFiles];             //time editor scheduled

    flag            m_bOn;                  //must the pgm be executed etc
    flag            m_bAutoReload;          // AutoReload ?
    flag            m_bReloadReqd;          //
    flag            m_bWithDBG;             //
    int             m_iIterCnt;             //iteration counter
    bool            m_bMustInit;          //flag set to indicate that Initialise must happen
    bool            m_bIterOne;           //flag set for first iteration when run is pressed
    bool            m_bMustTerm;          //flag set to indicate that Terminate must happen
    bool            m_bJustLoaded;          //flag set True just after Proc is loaded
    bool            m_bEmpty;               //flag set True for Empty SetState option
    bool            m_bPreset;              //flag set True for Preset SetState option

    Strng           m_sEditFileNameNew;

    long            m_lProcsChecked;
    long            m_lProcsFound;
  
  };

#endif

//===========================================================================
//
//
//
//===========================================================================

#if WITHNODETAGIO

class CNodeTagIOList; 
class DllImportExport CNodeTagIOItem : public CXRefStatus
  {
  friend class CNodeTagIOList;
  friend class MTagIOItem;
  friend class MTagIO;
  public:
    CNodeTagIOItem(FlwNode * pNd, bool Subscribed, LPCSTR Tag="", LPCSTR Name="", long IOFlags=0);
    ~CNodeTagIOItem();

    LPCSTR            TagOnly()       { return m_sTagOnly; };
    void              SetTag(LPCSTR NewTag);

    long              Configure(long UserHandle, LPCSTR ItemTag, LPCSTR ItemName, long IOFlags);
    
    MTagIOResult      CheckTag();
    MTagIOResult      ReadValue();
    MTagIOResult      WriteValue();

    LPCSTR            Name()        { return m_sName; };
    long              IdNo()        { return m_lIdNo; };
    long            & UserHandle()  { return m_lUserHandle; };
    byte              DataType()    { return m_iTypeRead; };
    CCnvIndex         CnvIndex()    { return m_iCnvRead; };
    LPCSTR            CnvText()     { return m_sCnv; };
    MCnv              Cnv()         { return MCnv(CnvIndex(), CnvText()); }
    bool              IsActive()    { return m_bInUse && IsXRefActive(); };
    long              IOFlags()     { return m_IOFlags; };
    DDEF_Flags        TagFlags()    { return m_TagFlags;  };
    LPCSTR            FullTag()     { return m_sFullTag; };

    //CXRefStatus Override
    bool              IsXRefActive() const { return m_bValid;  };

  protected:
    FlwNode         * m_pNd;
    CNodeTagIOList  * m_pTagIO;
    POSITION          m_MyPos;

    CString           m_sFullTag;
    CString           m_sTagOnly;
    CString           m_sCnv;
    byte              m_iTypeRead; 
    CCnvIndex         m_iCnvRead; 
    CString           m_sName;
    long              m_IOFlags;
    DDEF_Flags        m_TagFlags;
    long              m_lIdNo;
    long              m_lUserHandle;
    bool              m_bValid;
    bool              m_bInUse;
    bool              m_bTagChanged;
    bool              m_bValueValid;
    bool              m_bSubscribed;
    CTgFnIoVar        m_Var;

    MTagIOValue       m_Value;
  };

class DllImportExport CNodeTagIOList
  {
  friend class CNodeTagIOItem;
  friend class MTagIOItem;
  friend class MTagIO;

  public:
    CNodeTagIOList(FlwNode * pNd);
    ~CNodeTagIOList();

    void            BuildDataDefn(DataDefnBlk & DDB);
    flag            DataXchg(DataChangeBlk & DCB);
    flag            ValidateData(ValidateDataBlk & VDB);
    bool            ValidateReqd();
    bool            StartValidate();
    bool            EndValidate();

    int             ChangeTag(pchar pOldTag, pchar pNewTag);
    int             DeleteTag(pchar pDelTag);

    //void            SetSize(long TagCount);
    long            Count()                  { return m_Items.GetCount(); };
    
    //CNodeTagIOItem *operator[](int Id)       { return m_Items[Id]; } 
    //CNodeTagIOItem *operator[](LPCSTR IDTag) { int i=FindTag(IDTag); return i>=0 ? m_Items[i] : NULL; } 
    
    void            Add(CNodeTagIOItem * pItem);        
    void            Remove(CNodeTagIOItem * pItem);        
    //long            ConfigurSet(long Id, LPCSTR ItemTag, LPCSTR Name, long IOFlags);        // returns >=0 Id, < 0 Errors
    //bool            Remove(long Id);
    //bool            Remove(LPCSTR ItemTag);
    void            RemoveAll();
    //long            GetCount();

    //long            FindTag(LPCSTR ItemTag);
    //long            FindName(LPCSTR Name);

    int             UpdateXRefLists(CXRefBuildResults & Results);
    void            UnlinkAllXRefs();
    void            EvalCtrlInitialise(eScdCtrlTasks Tasks);
//    void            EvalCtrlStrategy(eScdCtrlTasks Tasks);
    void            GetAllValues(bool CallGetNearXRefs);
    void            SetAllValues(bool CallSetNearXRefs);

    static LPCSTR   ResultString(MTagIOResult Res);

  protected:
    void            UpdateList();

    FlwNode       * m_pNd;
                             
    long            m_nCount;
    long            m_lIdCount;

    //long            m_nBaseCount;
    //CArray<CNodeTagIOItem*, CNodeTagIOItem*> m_Items;
    CList<CNodeTagIOItem*, CNodeTagIOItem*> m_Items;
    //CMap<CNodeTagIOItem*, CNodeTagIOItem*, POSITION, POSITION> m_PosMap;
    //CMap<LPCSTR, LPCSTR, CNodeTagIOItem*, CNodeTagIOItem*> m_TagMap;
    //CMap<LPCSTR, LPCSTR, CNodeTagIOItem*, CNodeTagIOItem*> m_NameMap;

    bool            m_bValidateReqd;
    bool            m_bShowTags;
  };

#endif

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport
