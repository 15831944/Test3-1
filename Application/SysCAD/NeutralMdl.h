#pragma once

#include "dbbase.h"
#include "sfe_base.h"
#include "neutralcommon.h"
#include "neutralgrf.h"

//---------------------------------------------------------------------------

const int MdlDBMaxGrpLen          = 60+1;
const int MdlDBMaxTypeLen         = 10+1;
const int MdlDBMaxTagLen          = 80+1;
const int MdlDBMaxIOTagLen        = 60+1;
const int MdlDBMaxClassIDLen      = 40+1;
const int MdlDBMaxPrimaryCfgLen   = 250+1;
const int MdlDBMaxFieldTagLen     = 250+1;
const int MdlDBMaxFieldStrLen     = 250+1;
const int MdlDBMaxEqpDescLen      = 250+1;
const int MdlDBMaxEqpMemoLen      = 250+1;
const int MdlDBMaxEqpIdStrLen     = 250+1;
const int MdlDBMaxEqpLocationLen  = 250+1;
const int MdlDBMaxEqpGUIDLen      = 38+1;

class CNeutralMdlCommonUnits : public CFileRec
  {
  public:
    CNeutralMdlCommonUnits(KWDatabase* pDB_);
    virtual void  Init();
    virtual int   Read() { return BaseRead(); };
    virtual int   Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL   sm_bDoneInit;
  public:
    char          m_sUnitOperation[50];
    char          m_sModelClass[50];
  };

class CNeutralMdlCommonNames : public CFileRec
  {
  public:
    CNeutralMdlCommonNames(KWDatabase* pDB_);
    virtual void  Init();
    virtual int   Read() { return BaseRead(); };
    virtual int   Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL   sm_bDoneInit;
  public:
    char          m_sCommonName[50];
    char          m_sValueTag[50];
  };

class CNeutralMdlCommonIO : public CFileRec
  {
  public:
    CNeutralMdlCommonIO(KWDatabase* pDB_);
    virtual void  Init();
    virtual int   Read() { return BaseRead(); };
    virtual int   Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL   sm_bDoneInit;
  public:
    char          m_sCommonIOName[50];
    char          m_sUnitIOTag[50];
  };

class CNeutralMdlSequence : public CFileRec
  {
  public:
    CNeutralMdlSequence(bool WithGroup, KWDatabase* pDB_);
    virtual void  Init();
    virtual int   Read() { return BaseRead(); };
    virtual int   Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL   sm_bDoneInit;
  public:
    bool          m_bWithGroup; 
    char          m_sGroup[MdlDBMaxGrpLen ];
    long          m_lSeqNo;
    short         m_bIsLnk;
    char          m_sTag[MdlDBMaxTagLen];
  };

class CNeutralMdlUnits : public CFileRec
  {
  public:
    CNeutralMdlUnits(bool WithGroup, KWDatabase* pDB_);
    virtual void  Init();
    virtual int   Read() { return BaseRead(); };
    virtual int   Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL   sm_bDoneInit;
  public:
    bool          m_bWithGroup; 
    char          m_sGroup[MdlDBMaxGrpLen ];
    char          m_sTag[MdlDBMaxTagLen];
    char          m_sClassID[MdlDBMaxClassIDLen];
    char          m_sSubClassID[MdlDBMaxClassIDLen];
    char          m_sPrimaryCfg[MdlDBMaxPrimaryCfgLen ];
    char          m_sEqpDesc[MdlDBMaxEqpDescLen];
    char          m_sEqpMemo[MdlDBMaxEqpMemoLen];
    char          m_sEqpIdStr[MdlDBMaxEqpIdStrLen];
    char          m_sEqpLocation[MdlDBMaxEqpLocationLen];
    char          m_sEqpGUID[MdlDBMaxEqpGUIDLen];
  };

class CNeutralMdlLinks : public CFileRec
  {
  public:
    CNeutralMdlLinks(bool WithGroup, KWDatabase* pDB_);
    virtual void  Init();
    virtual int   Read() { return BaseRead(); };
    virtual int   Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL   sm_bDoneInit;
  public:
    bool          m_bWithGroup; 
    char          m_sGroup[MdlDBMaxGrpLen ];
    char          m_sTag[MdlDBMaxTagLen];
    char          m_sClassID[MdlDBMaxClassIDLen];
    char          m_sSubClassID[MdlDBMaxClassIDLen];
    char          m_sPrimaryCfg[MdlDBMaxPrimaryCfgLen ];
    char          m_sEqpDesc[MdlDBMaxEqpDescLen];
    char          m_sEqpMemo[MdlDBMaxEqpMemoLen];
    char          m_sEqpIdStr[MdlDBMaxEqpIdStrLen];
    char          m_sEqpLocation[MdlDBMaxEqpLocationLen];
    char          m_sEqpGUID[MdlDBMaxEqpGUIDLen];

    char          m_sSrcTag[MdlDBMaxTagLen];
    char          m_sSrcIO[MdlDBMaxIOTagLen];
    char          m_sSrcTS[MdlDBMaxIOTagLen];
    char          m_sDstTag[MdlDBMaxTagLen];
    char          m_sDstIO[MdlDBMaxIOTagLen];
    char          m_sDstTS[MdlDBMaxIOTagLen];
  };

class CNeutralMdlConfig : public CFileRec
  {
  public:
    CNeutralMdlConfig(bool WithGroup, KWDatabase* pDB_);
    //virtual ~CNeutralMdlConfig();
    virtual void  Init();
    virtual int   Read() { return BaseRead(); };
    virtual int   Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL   sm_bDoneInit;
  public:
    bool          m_bWithGroup; 
    char          m_sGroup[MdlDBMaxGrpLen ];
    long          m_lSeqNo;
    char          m_sNdTag[MdlDBMaxTagLen];
    char          m_sFldType[MdlDBMaxTypeLen];
    char          m_sFldTag[MdlDBMaxFieldTagLen];
    char          m_sCnvStr[MdlDBMaxFieldTagLen];
    char          m_sValue[MdlDBMaxFieldStrLen];
    //short         m_bIsDbl;
    //double        m_dValue;
  };

class CNeutralMdlWires : public CFileRec
  {
  public:
    CNeutralMdlWires (bool WithGroup, KWDatabase* pDB_);
    virtual void  Init();
    virtual int   Read() { return BaseRead(); };
    virtual int   Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL   sm_bDoneInit;
  public:
    bool          m_bWithGroup; 
    char          m_sGroup[MdlDBMaxGrpLen ];
    char          m_sCableTag[MdlDBMaxTagLen];
    long          m_lWireNo;
    char          m_sWireTag[MdlDBMaxTagLen];
    char          m_sSrcNdTag[MdlDBMaxTagLen];
    char          m_sSrcTrmStrip[MdlDBMaxTagLen];
    char          m_sSrcTerminal[MdlDBMaxTagLen];
    char          m_sDstNdTag[MdlDBMaxTagLen];
    char          m_sDstTrmStrip[MdlDBMaxTagLen];
    char          m_sDstTerminal[MdlDBMaxTagLen];
  };

class CNeutralErrors : public CFileRec
  {
  public:
    CNeutralErrors (bool WithGroup, KWDatabase* pDB_);
    //virtual ~CNeutralErrors ();
    virtual void  Init();
    virtual int   Read() { return BaseRead(); };
    virtual int   Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL   sm_bDoneInit;
  public:
    bool          m_bWithGroup; 
    char          m_sGroup[MdlDBMaxGrpLen ];
    char          m_sSource[MdlDBMaxTagLen];
    char          m_sDesc[MdlDBMaxTagLen];
  };

//---------------------------------------------------------------------------

class CNeutralMdlImportExport : public CNeutralGroupTagEditor
  {
  friend class CMdlDoc;
  public:
    CNeutralMdlImportExport();
    ~CNeutralMdlImportExport();
    
    void        SetGroup(LPCTSTR Group);
    void        SetGroupDesc(LPCTSTR GroupDesc);
    //void        SetExportTagFixups(long FixupRule, LPCTSTR Prefix, LPCTSTR Suffix, CStringArray *OldTags, CStringArray *NewTags);
    void        SetExportTagList(CStringArray *OldTags, CStringArray *NewTags);
    void        SetImportFilter(LPCTSTR Group);
    void        SetImportTagFixups(long FixupRule, LPCTSTR Prefix, LPCTSTR Suffix, CStringArray *OldTags, CStringArray *NewTags);
    long        EnumerateGroupTags(eScdNDBOptions Opts, CStringArray &Tags, LPCTSTR pDatabaseName);
    bool        DeleteGroup(LPCTSTR Name, LPCTSTR pDatabaseName);

    bool        DoExportValidate(eScdNDBOptions Opts, CString &ErrMsg);
    bool        DoImportValidate(eScdNDBOptions Opts, CString &ErrMsg);
    bool        DoExport(eScdNDBOptions Opts, LPCTSTR pDatabaseName);
    bool        DoImportDB(eScdNDBOptions Opts, LPCTSTR pDatabaseName);
    bool        DoImportGroup(eScdNDBOptions Opts, LPCTSTR pDatabaseName);

  protected:
    bool        ImportGroupInserts();
    bool        ImportBySequence();
    bool        ImportUnits(LPCTSTR Which=NULL);
    bool        ImportLinks(LPCTSTR Which=NULL);
    bool        ImportConfigs(LPCTSTR Which=NULL);
    bool        ImportWires(LPCTSTR Which=NULL);
    bool        EnumerateTagsBySequence(CStringArray &Tags);
    bool        EnumerateUnitTags(CStringArray &Tags, LPCTSTR Which=NULL);
    bool        EnumerateLinkTags(CStringArray &Tags, LPCTSTR Which=NULL);

  protected:
    bool        Open(LPCTSTR pDatabaseName, bool OpenForExport);
    void        Close();
    void        WriteSequence(LPCTSTR Group, CNodeListItem & Item);
    void        WriteUnit(LPCTSTR Group, CNodeListItem & Item);
    void        WriteLink(LPCTSTR Group, CNodeListItem & Item);
    void        WriteConfig(LPCTSTR Group, long SeqNo, CNodeConfigItem & Item);
    void        WriteError(LPCTSTR Group, CNodeListItem & Item);
    void        WriteWires(LPCTSTR Group, CLinkWiring & LW);

  protected:
    KWDatabase                  m_DB;
    eScdNDBOptions              m_Opts; 

    CNeutralMdlCommonUnits    * m_pCU;
    CNeutralMdlCommonNames    * m_pCN;
    CNeutralMdlCommonIO       * m_pCI;

    CNeutralGroupInserts      * m_pGI;
    CNeutralMdlSequence       * m_pSq;
    CNeutralMdlUnits          * m_pUn;
    CNeutralMdlLinks          * m_pLk;
    CNeutralMdlConfig         * m_pCf;
    CNeutralMdlWires          * m_pWr;
    CNeutralErrors            * m_pEr;

    bool                        m_bWithGroups;
    CString                     m_sGroup;
    CString                     m_sGroupDesc;
  };

//---------------------------------------------------------------------------

class CNeutralImportExport
  {
  friend class CMdlDoc;
  public:
    CNeutralImportExport();
    ~CNeutralImportExport();
    
    void        SetGroup(bool On);
    void        SetGroup(LPCTSTR Group);
    void        SetGroupDesc(LPCTSTR GroupDesc);
    //void        SetExportTagFixups(long FixupRule, LPCTSTR Prefix, LPCTSTR Suffix, CStringArray *OldTags, CStringArray *NewTags);
    void        SetExportTagList(CStringArray *OldTags, CStringArray *NewTags);
    void        SetImportFilter(LPCTSTR Group);
    void        SetImportTagFixups(long FixupRule, LPCTSTR Prefix, LPCTSTR Suffix, CStringArray *OldTags, CStringArray *NewTags);
    void        SetExportCoOrds(const CDblTriPt & Origin, const CDblTriPt & Lo, const CDblTriPt & Hi, const CDblTriPt & AlignLo, const CDblTriPt & AlignHi);
    void        SetImportOrigin(const CDblTriPt & Origin, const CDblTriPt & Scale);
    bool        DoExportValidate(eScdNDBOptions Opts, CString &ErrMsg);
    bool        DoImportValidate(eScdNDBOptions Opts, CString &ErrMsg);
    bool        DoExport(eScdNDBOptions Opts, LPCTSTR pGrfDatabase, LPCTSTR pMdlDatabase);
    bool        DoImportDB(eScdNDBOptions Opts, LPCTSTR pGrfDatabase, LPCTSTR pMdlDatabase);
    bool        DoImportGroup(eScdNDBOptions Opts, CGrfDoc* pDoc, LPCTSTR pGrfDatabase, LPCTSTR pMdlDatabase);
    long        GetGroups(CNeutralGrpDescList &List, LPCTSTR pDatabaseName);
    long        EnumerateGroupTags(eScdNDBOptions Opts, LPCTSTR Group, CStringArray &Tags, LPCTSTR pDatabaseName);
    bool        DeleteGroup(LPCTSTR Name, LPCTSTR pDatabaseName);

  protected:

    CNeutralGrfImportExport m_GrfDB;
    CNeutralMdlImportExport m_MdlDB;
  };

//---------------------------------------------------------------------------
