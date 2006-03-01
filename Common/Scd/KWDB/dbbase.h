//============ Copyright Kenwalt (Pty) Ltd (1996, 1997, ...) ================
// $Nokeywords: $
//===========================================================================
#ifndef __DBBASE_H
#define __DBBASE_H

#ifndef __KWDB_H
#include "kwdb.h"
#endif

#ifdef __DBBASE_CPP
  #define DllImportExport DllExport
#elif !defined(KWDB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//-------------------------------------------------------------------------

extern DllImportExport CStopWatch SW_DB;
extern DllImportExport CStopWatch SW_Txt;

//----------------------------------------------------------------------------

class DllImportExport CTxtFile
  {
  public:
    CTxtFile(BOOL nToFile = TRUE);
    ~CTxtFile();
    void Open(LPCTSTR nFileName);
    inline void Open(LPTSTR nFileName) { Open((LPCTSTR)nFileName); };
    void Append(LPCTSTR nFileName); 
    void Append(); //file must have been opened and then closed once!
    void Close();
    void Ln(LPTSTR fmt, ...);
    void Ln(BOOL DoNewLine, LPTSTR fmt, ...);
  private:
    FILE*   m_dbgf;
    char    m_FileName[_MAX_PATH];
    BOOL    m_ToFile;
  };

//----------------------------------------------------------------------------

//data field flags...
const WORD FF_None          = 0x00;
const WORD FF_Required      = 0x01;
const WORD FF_AllowZeroLen  = 0x02;

//-------------------------------------------------------------------------

class DllImportExport CFieldDef
  {
  public:
    CFieldDef() { pDef = NULL; pData = NULL; pLen=NULL; pEmptyVal = NULL; };
    CFieldDef(KWFieldDef* Def, void* Data, void* EmptyVal) { pDef = Def; pData = Data; pLen=NULL; pEmptyVal = EmptyVal; };
    CFieldDef(KWFieldDef* Def, char ** Data, long * Len, void* EmptyVal) { pDef = Def; pData = Data; pLen=Len; pEmptyVal = EmptyVal; };
    KWFieldDef* pDef;
    void* pData;
    long* pLen;
    void* pEmptyVal;
  };

//-------------------------------------------------------------------------

class DllImportExport CFileRec
  {
  public:
    CFileRec(BOOL ToTxt, BOOL ToDB, KWDatabase* pDB_ = NULL);
    virtual ~CFileRec();
    virtual void Init() = 0;
    virtual int Write(BOOL AddNew = TRUE) = 0;
    virtual int Read() { return 0; };
    int WriteUpdate() { return Write(false); };
    int WriteNew() { return Write(true); };
    void Open(LPCTSTR pFileName, int nOptions = 0);
    void OpenQuery(LPCTSTR pFileName, LPCTSTR pFind, int nOptions = 0);
    void Close();
    void DeleteAll(LPCTSTR Select=NULL);
    void CreateIndex(LPTSTR IndexName, BOOL Unique = TRUE, BOOL Primary = TRUE);
    bool SetCurrentIndex(LPTSTR IndexName);
    bool SeekEqualStr(LPTSTR What);
    inline ADODB::_RecordsetPtr Recordset() { return m_pRS; };
    inline BOOL IsEOF() { return m_pRS->adEOF; };
    inline void SetTableName(LPCTSTR pTableName) { sTableName = pTableName; };
    inline LPCTSTR TableName() { return (LPCTSTR)sTableName; };
    inline void SetIgnoreWriteErr3022(bool b) { bIgnoreWriteErr3022 = b; };

    int DefineClass(BOOL DeleteAll = FALSE);
    int CreateStr(  LPCTSTR strFieldName, 
                    LPTSTR pData,
                    short nTypeSize,
                    WORD FieldFlags = 0,
                    void* pEmptyVal = NULL);
    int CreateVal(  LPCTSTR strFieldName, 
                    void* pData,
                    ADOX::DataTypeEnum/*short*/ nType,
                    WORD FieldFlags = 0,
                    void* pEmptyVal = NULL);
    int CreateLongVal(  LPCTSTR strFieldName, 
                        char** pData,
                        long*  pLen,
                        ADOX::DataTypeEnum nType,
                        WORD FieldFlags=0,
                        void* pEmptyVal=NULL);
    int BaseRead();
    int BaseWrite(BOOL DoAddNew);

  protected:
    KWDatabase* m_pDB;
    CTxtFile* r;
    ADODB::_RecordsetPtr m_pRS;
    CString sTitleLine;
    CString sTableName;
    CArray <CFieldDef, CFieldDef&> Defs;
    WORD bToTxt:1,
         bToDB:1,
         bIgnoreWriteErr3022:1;
  };

//-------------------------------------------------------------------------


#undef DllImportExport

#endif
