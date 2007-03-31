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
    DWORD           m_dwProcessId[TP_MaxTtlIncludeFiles];          //process ID for the editor
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

// Moved to md_share4.h
//enum { TIO_Int, TIO_Double, TIO_String };
//const long TIO_Get       = 0x00000001;
//const long TIO_Set       = 0x00000002;
//const long TIO_Parm      = 0x00000004;
//const long TIO_SICnv     = 0x00000008;

class DllImportExport CNodeTagIOList
  {
  public:

    class CTagItem : public CXRefStatus
      {
      public:
        CTagItem(LPCSTR Tag, LPCSTR Name, long Options);
        ~CTagItem();

        LPCSTR        Tag()       { return m_sTag; };
        LPCSTR        Cnv()       { return m_sCnv; };
        LPCSTR        FullTag()   { return m_sFullTag; };
        LPCSTR        Name()      { return m_sName; };//if (m_Name.GetLength()>0) return m_sName; CString S; S.Format("Tag%03i", m_IdNo); return S; };
        byte          DataType()  { return m_Var.m_pXRef ? m_Var.m_pXRef->m_iType : tt_NULL; };
        CCnvIndex     CnvIndex()  { return m_Var.m_pXRef ? m_Var.m_pXRef->m_iCnvIndex : 0; };
        LPCSTR        CnvText()   { return m_Var.m_pXRef ? m_Var.m_pXRef->m_sCnv() : ""; };

        //CXRefStatus Override
        bool IsXRefActive() const    { return m_bValid;  };

      public:
        CString           m_sTag;
        CString           m_sCnv;
        CString           m_sFullTag;
        CString           m_sName;
        long              m_lOptions;
        //short             m_iDataType;
        long              m_lIdNo;
        bool              m_bValid;
        bool              m_bInUse;
        bool              m_bValueValid;
        CTgFnIoVar        m_Var;

        double            m_DblValue;
      };

  public:
    CNodeTagIOList(FlwNode * pNd, long EstTagCount);
    ~CNodeTagIOList();

    void            BuildDataDefn(DataDefnBlk & DDB);
    flag            DataXchg(DataChangeBlk & DCB);
    flag            ValidateData(ValidateDataBlk & VDB);

    int             ChangeTag(pchar pOldTag, pchar pNewTag);
    int             DeleteTag(pchar pDelTag);

    long            Count()                  { return m_Items.GetCount(); };
    CTagItem &      operator[](int ID)    { return *m_Items[ID]; } 
    
    long            Add(LPCSTR ItemTag, LPCSTR Name, long Options);        // returns >=0 ID, < 0 Errors
    bool            Remove(long ID);
    bool            Remove(LPCSTR ItemTag);
    void            RemoveAll();
    long            GetCount();

    long            FindTag(LPCSTR ItemTag);
    long            FindName(LPCSTR Name);

    bool            IsActive(long ID);
    short           GetDataType(long ID);
    short           GetCnvIndex(long ID);
    long            GetOptions(long ID);
    LPCSTR          GetTag(long ID);
    LPCSTR          GetFullTag(long ID);
    LPCSTR          GetCnvText(long ID);

    double          GetDValue(long ID, bool UseCnv=false);
    void            SetDValue(long ID, double Value, bool UseCnv=false);

    bool            IsActive(LPCSTR Tag);
    short           GetDataType(LPCSTR Tag);
    short           GetCnvIndex(LPCSTR Tag);
    long            GetOptions(LPCSTR Tag);
    LPCSTR          GetFullTag(LPCSTR Tag);
    LPCSTR          GetCnvText(LPCSTR Tag);

    double          GetDValue(LPCSTR Tag, bool UseCnv=false);
    void            SetDValue(LPCSTR Tag, double Value, bool UseCnv=false);

    int             UpdateXRefLists(CXRefBuildResults & Results);
    void            UnlinkAllXRefs();
    void            EvalCtrlInitialise(eScdCtrlTasks Tasks);
//    void            EvalCtrlStrategy(eScdCtrlTasks Tasks);
    void            GetAllValues(bool CallGetNearXRefs);
    void            SetAllValues(bool CallSetNearXRefs);

  private:
    void            UpdateList();

    FlwNode       * m_pNd;
                             
    long            m_nCount;

    CArray<CTagItem*, CTagItem*> m_Items;
    CMap<LPCSTR, LPCSTR, CTagItem*, CTagItem*> m_TagMap;
    CMap<LPCSTR, LPCSTR, CTagItem*, CTagItem*> m_NameMap;

    bool            m_bShowTags;
  };

#endif

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport
