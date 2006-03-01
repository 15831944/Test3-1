//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DBHELPER_H
#define  __DBHELPER_H

#include <afxtempl.h> // "xafxtempl.h"
#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF", "adEOF"), no_implementation
#import "c:\program files\common files\system\ado\msadox.dll" no_implementation

#ifdef __DBHELPER_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

struct CDBConnect
  {
  LPCTSTR  m_pName;
  LPCTSTR  m_pOpenStr;
  LPCTSTR  m_pCreateStr;
  };

extern DllImportExport CDBConnect * DBConnect(long i);
extern DllImportExport LPCTSTR DBConnectName(long i);
extern DllImportExport LPCTSTR DBConnectOpenString(long i);
extern DllImportExport LPCTSTR DBConnectCreateString(long i);
extern DllImportExport long DBConnectCount();
extern DllImportExport long DBDefaultConnect();
extern DllImportExport void SetDBDefaultConnect(long DBFmt);

const long DBConnect_Find            = -2;
const long DBConnect_Default         = -1;
const long DBConnect_JET97           = 0;
const long DBConnect_JET2000         = 1;
const long DBConnect_EXCEL           = 2;
const long DBConnect_MSDE            = 3;

//---------------------------------------------------------------------------

const DWORD DBH_HasHeadings = 0x0001; // default On
//const DWORD DBH_Exclusive   = 0x0002; // default Off
const DWORD DBH_ReadOnly    = 0x0004; // default Off
const DWORD DBH_ForwardOnly = 0x0008; // default On
const DWORD DBH_TrimStrings = 0x0010; // default On

class DllImportExport CDBHelperBase
  {
  friend class CDBHelper;
  public:
    CDBHelperBase();
    CDBHelperBase(char* Filename, char* SheetName);
    CDBHelperBase(char* Dir, char* Filename, char* SheetName);
    virtual ~CDBHelperBase();
    virtual void Reset(char* Filename, char* SheetName);
    virtual void Reset(char* Dir, char* Filename, char* SheetName);
    virtual BOOL Open(long DBConnectID, char* FirstFieldName = NULL, DWORD flags=DBH_HasHeadings|DBH_ForwardOnly|DBH_TrimStrings);
    virtual short GetFieldCount();
    virtual BOOL CheckFieldNames(char ** FieldNames);
    virtual BOOL CheckOptionalFieldNames(char ** FieldNames);
    virtual BOOL IsEOF();
    virtual BOOL GetNextRow(CArray <COleVariant, COleVariant&> &Values);
    virtual BOOL GetNextRow(CStringArray& Values);
    virtual void MoveNext();
    virtual void MoveFirst();
    virtual void Close();
    virtual int GetLineNo();
    virtual int BaseDoGet(ADODB::_RecordsetPtr pRS, COleVariant &var, int Index, int Line, int &OK);
  protected:
    virtual void CommonConstruct(char* pDir, char* pFilename, char* SheetName);

    CString          sFilename;   //name of data base (Excel file)
    int              iLineNumber; //current Excel row number
    WORD             bHasHeadings:1,
                     bDoneCheckFields:1,
                     bTrimStrings:1; //must whitespace be trimmed
    CString          sSheetName;  //name of record set (Excel worksheet)
    CString          m_ErrStr;
    long             m_ErrNo;

    //int              nRSOptions;

    void             SetErrorInfo(_com_error &e, LPCTSTR strMsg);

  public:
    CArray<int, int> FieldIndexes;

    ADODB::_ConnectionPtr m_pCN;
    ADODB::_RecordsetPtr m_pRS;
    _bstr_t           m_bsConnect;
    _bstr_t           m_bsSource;

    const char *     ErrorString() { return (const char*)m_ErrStr; };
    long             ErrorNumber() { return m_ErrNo; };
  }; 

//---------------------------------------------------------------------------

class DllImportExport CExcelDBHelper : public CDBHelperBase
  {
  public:
    static const int MaxRowSearchForFieldNames;

    CExcelDBHelper(char* Filename, char* SheetName);
    CExcelDBHelper(char* Dir, char* Filename, char* SheetName);
    virtual ~CExcelDBHelper();
    virtual BOOL Open(long DBConnectID, char* FirstFieldName = NULL, DWORD flags=DBH_HasHeadings|DBH_ForwardOnly|DBH_TrimStrings);

  protected:
    virtual void CommonConstruct(char* pDir, char* pFilename, char* SheetName);

  }; 

//---------------------------------------------------------------------------
                                
class DllImportExport CDBHelper
  {

  public:
    CDBHelper();
    CDBHelper(char* Filename, char* SheetName);
    CDBHelper(char* Dir, char* Filename, char* SheetName);
    virtual ~CDBHelper();
    void Reset(char* Filename, char* SheetName);
    void Reset(char* Dir, char* Filename, char* SheetName);
    BOOL Open(long DBConnectID, char* FirstFieldName = NULL, DWORD flags=DBH_HasHeadings|DBH_ForwardOnly|DBH_TrimStrings);
    short GetFieldCount();
    BOOL CheckFieldNames(char ** FieldNames);
    BOOL CheckOptionalFieldNames(char ** FieldNames);
    BOOL IsEOF();
    BOOL GetNextRow(CArray <COleVariant, COleVariant&> &Values);
    BOOL GetNextRow(CStringArray& Values);
    void MoveNext();
    void MoveFirst();
    void Close();
    int GetLineNo();// { return iLineNumber; };
  protected:
    void CommonConstruct(char* pDir, char* pFilename, char* SheetName);

    CDBHelperBase*   pDB;

  public:
    int              FieldIndex(int i);
    int              NFieldIndexes();
    const char *     TableName() { return (const char *)pDB->sSheetName; };

    //int              RSOptions() { return pDB->nRSOptions; };
    ADODB::_RecordsetPtr RS() { return pDB->m_pRS; };

    void             SetErrorInfo(_com_error &e, LPCTSTR strMsg) { pDB->SetErrorInfo(e, strMsg); };
    const char *     ErrorString() { return pDB->ErrorString(); };
    long             ErrorNumber() { return pDB->ErrorNumber(); };
  
  }; 

extern void DllImportExport MyAddField(ADOX::_TablePtr pTbl, ADOX::_CatalogPtr pCat, LPCTSTR Name, ADOX::DataTypeEnum Type, long Size, ADOX::ColumnAttributesEnum Att=ADOX::ColumnAttributesEnum(0));

#undef DllImportExport
//---------------------------------------------------------------------------
#endif
