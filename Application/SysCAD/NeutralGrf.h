#pragma once

#ifndef __DBBASE_H
#include "dbbase.h"
#endif
#include "grfdoc.h"
#include "neutralcommon.h"

//---------------------------------------------------------------------------
const int GrfDBMaxTagLen = 81;
const int GrfDBMaxPageLen = 51;
const int GrfDBMaxSymbolLen = 101;
const int GrfDBMaxClassIDLen = 41;
//const int GrfDBMaxSymSrcLen = 100;

class CNeutralGrfInserts : public CFileRec
  {
  public:
    CNeutralGrfInserts(bool WithGroup, KWDatabase* pDB_);
    virtual void    Init();
    virtual int     Read() { return BaseRead(); };
    virtual int     Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL     sm_bDoneInit;
  public:
    bool            m_bWithGroup; 
    char            m_sGroup[GrfDBMaxTagLen];
    char            m_sTag[GrfDBMaxTagLen];
    char            m_sPage[GrfDBMaxPageLen];
    char            m_sSymbol[GrfDBMaxSymbolLen];
    CDblTriPt       m_Insert;
    CDblTriPt       m_Scale;
    double          m_dRotation;
  };

class CNeutralGrfUnits : public CFileRec
  {
  public:
    CNeutralGrfUnits(bool WithGroup, KWDatabase* pDB_);
    virtual void    Init();
    virtual int     Read() { return BaseRead(); };
    virtual int     Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL     sm_bDoneInit;
  public:
    bool            m_bWithGroup; 
    char            m_sGroup[GrfDBMaxTagLen];
    char            m_sTag[GrfDBMaxTagLen];
    char            m_sPage[GrfDBMaxPageLen];
    char            m_sSymbol[GrfDBMaxSymbolLen];
    char            m_sClassID[GrfDBMaxClassIDLen];
    CDblTriPt       m_Insert;
    CDblTriPt       m_Scale;
    double          m_dRotation;
  };

class CNeutralGrfLinks : public CFileRec
  {
  public:
    CNeutralGrfLinks(bool WithGroup, KWDatabase* pDB_);
    virtual void    Init();
    virtual int     Read() { return BaseRead(); };
    virtual int     Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL     sm_bDoneInit;
  public:
    bool            m_bWithGroup; 
    char            m_sGroup[GrfDBMaxTagLen];
    char            m_sTag[GrfDBMaxTagLen];
    char            m_sPage[GrfDBMaxPageLen];
    char            m_sClassID[GrfDBMaxClassIDLen];
    CDblTriPt       m_Insert;
  };

class CNeutralGrfLinklines : public CFileRec
  {
  public:
    CNeutralGrfLinklines(bool WithGroup, KWDatabase* pDB_);
    virtual void    Init();
    virtual int     Read() { return BaseRead(); };
    virtual int     Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL     sm_bDoneInit;
  public:
    bool            m_bWithGroup; 
    char            m_sGroup[GrfDBMaxTagLen];
    char            m_sTag[GrfDBMaxTagLen];
    char            m_sPage[GrfDBMaxPageLen];
    long            m_lSeqNo;
    CDblTriPt       m_Vertex;
  };

class CNeutralGrfSymbols : public CFileRec
  {
  public:
    CNeutralGrfSymbols(bool WithGroup, KWDatabase* pDB_);
    virtual void    Init();
    virtual int     Read() { return BaseRead(); };
    virtual int     Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL     sm_bDoneInit;
  public:
    bool            m_bWithGroup; 
    char            m_sName[GrfDBMaxSymbolLen];
    char        *   m_pSymbolData;
    long            m_SymbolLen;
  };

//---------------------------------------------------------------------------

class CNeutralGrfImportExport : public CNeutralGroupTagEditor
  {
  friend class CGrfDoc;
  public:
    CNeutralGrfImportExport();
    ~CNeutralGrfImportExport();

    void            SetGroup(bool On);
    void            SetGroup(LPCTSTR Group);
    void            SetGroupDesc(LPCTSTR GroupDesc);
    //void            SetExportTagFixups(long FixupRule, LPCTSTR Prefix, LPCTSTR Suffix, CStringArray *OldTags, CStringArray *NewTags);
    void            SetExportTagList(CStringArray *OldTags, CStringArray *NewTags);
    void            SetExportCoOrds(const CDblTriPt & Origin, const CDblTriPt & Lo, const CDblTriPt & Hi, const CDblTriPt & AlignLo, const CDblTriPt & AlignHi);
    void            SetImportFilter(LPCTSTR Group);
    void            SetImportTagFixups(long FixupRule, LPCTSTR Prefix, LPCTSTR Suffix, CStringArray *OldTags, CStringArray *NewTags);
    void            SetImportOrigin(const CDblTriPt & Origin, const CDblTriPt & Scale);

    bool            DoExportValidate(eScdNDBOptions Opts, CDocTemplate & Template, CString &ErrMsg);
    bool            DoImportValidate(eScdNDBOptions Opts, CDocTemplate & Template, CString &ErrMsg);
    void            DoExport(eScdNDBOptions Opts, CGrfDoc* pDoc);
    bool            DoExport(eScdNDBOptions Opts, CDocTemplate & Template, LPCTSTR pDatabaseName);
    bool            DoImportDB(eScdNDBOptions Opts, CDocTemplate & Template, LPCTSTR pDatabaseName);
    bool            DoImportGroup(eScdNDBOptions Opts, CGrfDoc* pDoc, LPCTSTR pDatabaseName);
    long            GetGroups(CNeutralGrpDescList &List, LPCTSTR pDatabaseName);
    bool            DeleteGroup(LPCTSTR Name, LPCTSTR pDatabaseName);

  protected:
    bool            Open(LPCTSTR pDatabaseName, bool OpenForExport);
    void            Close();
    void            WriteInsert(LPCTSTR Group, CGrfTagInfo & GTI, CDblTriPt Org, LPCTSTR pPage, LPCTSTR pDocGUID);
    void            WriteUnit(LPCTSTR Group, CGrfTagInfo & GTI, CDblTriPt Org, LPCTSTR pPage, LPCTSTR pDocGUID);
    void            WriteLink(LPCTSTR Group, CGrfTagInfo & GTI, CDblTriPt Org, LinePointsArray & LPA, char* pPage, LPCTSTR pDocGUID);
    void            WriteGroupInfo(LPCTSTR Group, LPCTSTR Desc, CDblTriPt Org, CDblTriPt Lo, CDblTriPt Hi, CDblTriPt AlignLo, CDblTriPt AlignHi);

    bool            OpenSymbolExport();
    bool            ExportSymbol(CGrfTagInfo & GTI, CDblTriPt Org, LPCTSTR pDocGUID);
    void            CloseSymbolExport();
    bool            OpenSymbolImport();
    bool            ImportSymbol(LPCTSTR Name, bool MustExist);
    void            CloseSymbolImport();

  protected:
    KWDatabase              m_DB;
    eScdNDBOptions          m_Opts;

    CNeutralGrfInserts    * m_pIn;
    CNeutralGrfUnits      * m_pUn;
    CNeutralGrfLinks      * m_pLk;
    CNeutralGrfLinklines  * m_pLl;
    CNeutralGrfSymbols    * m_pSy;
    CNeutralGroups        * m_pGr;
    CNeutralGroupInserts  * m_pGI;

    bool                    m_bWithGroups;
    CString                 m_sGroup;
    CString                 m_sGroupDesc;

    CDblTriPt               m_Origin;
    CDblTriPt               m_Scale;
    CDblTriPt               m_Move;
    CDblTriPt               m_Lo;
    CDblTriPt               m_Hi;
    CDblTriPt               m_AlignLo;
    CDblTriPt               m_AlignHi;
    
    bool                    m_bExportingSymbols;
    CString                 m_SymbolExportFolder;
    bool                    m_bImportingSymbols;
    CString                 m_SymbolImportFolder;


  };
