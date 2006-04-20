//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define __DBHELPER_CPP

#include "sc_defs.h"
#include "dbhelper.h"
#include "errorlog.h"

#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF", "adEOF"), implementation_only
#import "c:\program files\common files\system\ado\msadox.dll" implementation_only

//===========================================================================

static CDBConnect ls_DBConnects[]=
  {
    {"Access 97",     "Provider='Microsoft.JET.OLEDB.4.0';Data source = '%s';",
                      "Provider='Microsoft.JET.OLEDB.4.0';Data source = '%s';Jet OLEDB:Engine Type=4"},
    {"Access 2000",   "Provider='Microsoft.JET.OLEDB.4.0';Data source = '%s';",
                      "Provider='Microsoft.JET.OLEDB.4.0';Data source = '%s';Jet OLEDB:Engine Type=5"},
    {"Excel",         "Provider='Microsoft.JET.OLEDB.4.0';Data source = '%s';",
                      "Provider='Microsoft.Jet.OLEDB.4.0';Data Source = '%s';Extended Properties = 'Excel 8.0';" /*HDR=No;*/},
  };

static long ls_lDBConnectDefault=DBConnect_JET2000;

CDBConnect * DBConnect(long i) 
  {
  if (i<0)
    i=ls_lDBConnectDefault;
  return (i<sizeof(ls_DBConnects)/sizeof(ls_DBConnects[0])) ? &ls_DBConnects[i]:NULL; 
  };
LPCTSTR DBConnectName(long i) 
  { 
  if (i<0)
    i=ls_lDBConnectDefault;
  return (i>=0 && i<sizeof(ls_DBConnects)/sizeof(ls_DBConnects[0])) ? ls_DBConnects[i].m_pName:NULL; 
  };

LPCTSTR DBConnectOpenString(long i) 
  { 
  if (i<0)
    i=ls_lDBConnectDefault;
  return (i>=0 && i<sizeof(ls_DBConnects)/sizeof(ls_DBConnects[0])) ? ls_DBConnects[i].m_pOpenStr:NULL;
  };
LPCTSTR DBConnectCreateString(long i) 
  { 
  if (i<0)
    i=ls_lDBConnectDefault;
  return (i>=0 && i<sizeof(ls_DBConnects)/sizeof(ls_DBConnects[0])) ? ls_DBConnects[i].m_pCreateStr:NULL;
  };
long DBConnectCount() 
  { 
  return sizeof(ls_DBConnects)/sizeof(ls_DBConnects[0]); 
  };

long DBDefaultConnect() 
  { 
  return ls_lDBConnectDefault; 
  };
void SetDBDefaultConnect(long i)
  {
  if (i>=0 && i<sizeof(ls_DBConnects)/sizeof(ls_DBConnects[0]))
    ls_lDBConnectDefault=i;
  };

//===========================================================================

CDBHelperBase::CDBHelperBase()
  {
  m_pRS = NULL;
  //m_RS1=NULL;
  bDoneCheckFields = 0;
  bHasHeadings = 0;
  bTrimStrings = 1;
  iLineNumber = 0;
  }

//===========================================================================

CDBHelperBase::CDBHelperBase(char* Dir, char* Filename, char* SheetName)
  {
  CommonConstruct(Dir, Filename, SheetName);
  }

//---------------------------------------------------------------------------

CDBHelperBase::CDBHelperBase(char* Filename, char* SheetName)
  {
  CommonConstruct("", Filename, SheetName);
  }

//---------------------------------------------------------------------------

void CDBHelperBase::Reset(char* Dir, char* Filename, char* SheetName)
  {
  if (m_pRS)
    {
    m_pRS->Close();
    m_pRS.Release();
    }
  CommonConstruct(Dir, Filename, SheetName);
  }

//---------------------------------------------------------------------------

void CDBHelperBase::Reset(char* Filename, char* SheetName)
  {
  if (m_pRS)
    {
    m_pRS->Close();
    m_pRS.Release();
    }
  CommonConstruct("", Filename, SheetName);
  }

//---------------------------------------------------------------------------

void CDBHelperBase::CommonConstruct(char* Dir, char* Filename, char* SheetName)
  {
  m_pRS = NULL;
  bDoneCheckFields = 0;
  bHasHeadings = 0;
  bTrimStrings = 1;
  iLineNumber = 0;
  sFilename = Dir;
  sFilename += Filename;
  sSheetName = SheetName;
  ASSERT(sSheetName.GetLength()>0);
//  if (sSheetName[sSheetName.GetLength()-1] != '$')
//    sSheetName += '$';
  }

//---------------------------------------------------------------------------

CDBHelperBase::~CDBHelperBase()
  {
  try
    {
    if (m_pRS)
      {
      if (m_pRS->State != ADODB::adStateClosed)
        m_pRS->Close();
      m_pRS.Release();
      }
    }
  catch(_com_error &e)
    {
    SetErrorInfo(e, "Close RecordSet");
    }
  }

//---------------------------------------------------------------------------

BOOL CDBHelperBase::Open(long DBConnectID, char* FirstFieldName/*=NULL*/, DWORD flags/*=DBH_HasHeadings|DBH_ForwardOnly|DBH_TrimStrings */)
  {
#define FLAGSET(a, b) ((a&b)!=0)
  if (m_pRS)
    {
    m_pRS->Close();
    m_pRS.Release();
    }
  bHasHeadings = (flags & DBH_HasHeadings)!=0;
  bTrimStrings = (flags & DBH_TrimStrings)!=0;
  //nRSOptions = ((flags & DBH_ReadOnly)!=0) ? dbReadOnly : 0;
  //if ((flags & DBH_ForwardOnly)!=0)
  //  nRSOptions |= dbForwardOnly;

  try
    {
    HRESULT hr=m_pRS.CreateInstance(__uuidof(ADODB::Recordset));
    if (SUCCEEDED(hr))
      {
      
      }
    else
      {
      LogError("TagHelp", 0, "ADODB::Recordset not Created");
      return false;
      }
    }
  catch (...)
    {
    LogError("TagHelp", 0, "Exception Occurred");
    int xxx=0;
    }

  CString Connect;
  Connect.Format(DBConnectOpenString(DBConnectID), (LPCTSTR)sFilename);
  m_bsConnect=Connect;
  try
    {
    CString Src;
    Src.Format("SELECT * FROM %s", (const char*)sSheetName);
    _bstr_t Source(Src);

    m_pRS->Open(Source, m_bsConnect, 
      (flags & DBH_ForwardOnly) ? ADODB::adOpenForwardOnly:ADODB::adOpenKeyset, 
      (flags & DBH_ReadOnly)    ? ADODB::adLockOptimistic/*ReadOnly*/:ADODB::adLockOptimistic, 
      -1);
    }
  catch(_com_error &e)
    {
    SetErrorInfo(e, "Open RecordSet");
    return false;
    }
  return TRUE;
  }

//---------------------------------------------------------------------------

short CDBHelperBase::GetFieldCount()
  {
  return (short)(m_pRS ? m_pRS->Fields->Count : 0);
  }

//---------------------------------------------------------------------------

BOOL CDBHelperBase::CheckFieldNames(char ** FieldNames)
  {
  bDoneCheckFields = 0;
  if (m_pRS)
    {
    const short FldCnt = GetFieldCount();

    short ExpectedFldCnt = 0;
    while (FieldNames[ExpectedFldCnt])
      ExpectedFldCnt++;
    if (FldCnt<ExpectedFldCnt)
      return FALSE; //less fields in file than expected

    FieldIndexes.SetSize(ExpectedFldCnt);
    for (short i=0; i<ExpectedFldCnt; i++)
      FieldIndexes[i] = -1;

    long FoundCount=0;
    for (short j=0; (j<FldCnt); j++)
      {
      BOOL Fnd = FALSE;
      _bstr_t nm=m_pRS->Fields->GetItem(_variant_t((long)j))->Name;
      for (short i=0; i<ExpectedFldCnt && !Fnd; i++)
        {
        if (_stricmp(FieldNames[i], nm)==0)
          {
          //if (FieldIndexes[i]>=0)
          //  return FALSE; //field name occurs twice!
          FieldIndexes[i] = j;
          Fnd = TRUE;
          FoundCount++;
          }
        }
      }
    if (FoundCount<ExpectedFldCnt)
      return FALSE; //field name not found
    bDoneCheckFields = 1;
    return TRUE;
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CDBHelperBase::CheckOptionalFieldNames(char ** FieldNames)
  {
  ASSERT(bDoneCheckFields);
  if (m_pRS)
    {
    const short FldCnt = GetFieldCount();
    short ExpectedFldCnt = 0;
    while (FieldNames[ExpectedFldCnt])
      ExpectedFldCnt++;
    int OldSize = FieldIndexes.GetSize();
    FieldIndexes.SetSize(OldSize + ExpectedFldCnt);
    for (short i=OldSize; i<OldSize + ExpectedFldCnt; i++)
      FieldIndexes[i] = -1;
    for (i=0; i<ExpectedFldCnt; i++)
      {
      for (short j=0; j<FldCnt; j++)
        {
        //m_pRS->GetFieldInfo(j, fi);
        if (_stricmp(FieldNames[i], (const char*)m_pRS->Fields->GetItem(_variant_t((long)j))->Name)==0)
          {
          FieldIndexes[OldSize + i] = j;
          break;
          }
        }
      }
    return TRUE;
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CDBHelperBase::IsEOF()
  {
  return m_pRS ? m_pRS->adEOF : TRUE;
  }

//---------------------------------------------------------------------------

void CDBHelperBase::MoveNext()
  {
  if (m_pRS)
    {
    m_pRS->MoveNext();
    iLineNumber++;
    }
  }

//---------------------------------------------------------------------------

void CDBHelperBase::MoveFirst()
  {
  if (m_pRS)
    {
    m_pRS->MoveFirst();
    iLineNumber=0;
    }
  }

//---------------------------------------------------------------------------

int CDBHelperBase::BaseDoGet(ADODB::_RecordsetPtr pRS, COleVariant &var, int Index, int Line, int &OK)
  {
  try
    {
    OK=1;
    var.vt=VT_EMPTY;
//    if (!pRS->IsFieldNull(Index))
    var=pRS->Fields->GetItem(_variant_t((long)Index))->Value;
    if ( var.vt == VT_NULL )    // do something
      OK=0;
    }
  catch(_com_error &e)
    {
    OK=0;
    // Numeric field overflow. (Error 3349)
    //if (e.ErrorInfo.m_pErrorInfo->m_lErrorCode!=3349)
    if (SCODE_CODE(e.Error())!=ADODB::adErrDataOverflow)
      {
      char Buff[256];
      sprintf(Buff, "GetField (Row:%d, Column:%d)", Line, Index);
      SetErrorInfo(e, Buff);
      return FALSE;
      }
    var.vt=VT_EMPTY;
    };
  return TRUE;
  };

//---------------------------------------------------------------------------

BOOL CDBHelperBase::GetNextRow(CArray <COleVariant, COleVariant&> &Values)
  {
  ASSERT(bDoneCheckFields);
  if (m_pRS)
    {
    const int Cnt = FieldIndexes.GetSize();
    Values.SetSize(Cnt);
    int i;
    for (i=0; i<Cnt; i++) // CNM
      Values[i].Clear();
    for (i=0; i<Cnt; i++)
      {
      if (FieldIndexes[i]>=0)
        {
        // CNM for safety
        int OK=0;
        if (!BaseDoGet(m_pRS, Values[i], FieldIndexes[i], iLineNumber, OK))
          return FALSE;

        if (!OK)
          Values[i].Clear();
        }
      }
    m_pRS->MoveNext();
    iLineNumber++;
    return TRUE;
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CDBHelperBase::GetNextRow(CStringArray& Values)
  {
  ASSERT(bDoneCheckFields);
  if (m_pRS)
    {
    char Buff[256];
    COleVariant var;
    const int Cnt = FieldIndexes.GetSize();
    Values.SetSize(Cnt);
    int i;
    for (i=0; i<Cnt; i++) // CNM
      Values[i] = "";
    for (i=0; i<Cnt; i++)
      {
      if (FieldIndexes[i]<0)
        Values[i] = "";
      else
        {
        // CNM for safety
        int OK=0;
        if (!BaseDoGet(m_pRS, var, FieldIndexes[i], iLineNumber, OK))
          return FALSE;

//        if (1) // extra block scope
//          {
//          try
//            {
//            m_pRS->GetFieldValue(FieldIndexes[i], var);
//            }
//          catch(CDaoException * e)
//            {
//            if (e->m_pErrorInfo->m_lErrorCode!=3349)
//              {
//              sprintf(Buff, "GetField (Row:%d, Column:%d)", iLineNumber+1, FieldIndexes[i]);
//              SetErrorInfo(e, Buff);
//              e->Delete();
//              return FALSE;
//              }
//            e->Delete();
//            var.vt=VT_EMPTY;
//            };
//          };

        if (!OK)
          Values[i] = "";
        else
          switch (var.vt)
            {
            case VT_EMPTY:
              {
              Values[i] = "";
              break;
              }
            case VT_R4:
              {
              sprintf(Buff, "%g", var.fltVal);
              Values[i] = Buff;
              break;
              }
            case VT_R8:
              {
              sprintf(Buff, "%g", var.dblVal);
              Values[i] = Buff;
              break;
              }
            default:
              {
              Values[i] = var;//V_BSTRT(&var);
              if (bTrimStrings)
                {
                Values[i].TrimLeft();
                Values[i].TrimRight();
                }
              break;
              }
            }
        }
      }
    m_pRS->MoveNext();
    iLineNumber++;
    return TRUE;
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

void CDBHelperBase::Close()
  {
  try
    {
    if (m_pRS)// && m_pRS->IsOpen())
      {
      m_pRS->Close();
      m_pRS.Release();
      }
    //DB.Close();
    }
  catch(_com_error &e)
    {
    SetErrorInfo(e, _T("Failure Closing Database\n") );
    }
  }

//---------------------------------------------------------------------------

int CDBHelperBase::GetLineNo()
  {
  return iLineNumber; 
  };

//===========================================================================
//
//
//
//===========================================================================

const int CExcelDBHelper::MaxRowSearchForFieldNames = 32;

CExcelDBHelper::CExcelDBHelper(char* Dir, char* Filename, char* SheetName)
  {
  CommonConstruct(Dir, Filename, SheetName);
  }

//---------------------------------------------------------------------------

CExcelDBHelper::CExcelDBHelper(char* Filename, char* SheetName)
  {
  CommonConstruct("", Filename, SheetName);
  }

//---------------------------------------------------------------------------

void CExcelDBHelper::CommonConstruct(char* Dir, char* Filename, char* SheetName)
  {
  CDBHelperBase::CommonConstruct(Dir, Filename, SheetName);
  ASSERT(sSheetName.GetLength()>0);
  if (sSheetName[sSheetName.GetLength()-1] != '$')
    sSheetName += '$';
  }

//---------------------------------------------------------------------------

CExcelDBHelper::~CExcelDBHelper()
  {
  //m_pRS.Release();
  }

//---------------------------------------------------------------------------

BOOL CExcelDBHelper::Open(long DBConnectID, char* FirstFieldName/*=NULL*/, DWORD flags/*=DBH_HasHeadings|DBH_ForwardOnly|DBH_TrimStrings */)
  {
  if (m_pRS)
    {
    m_pRS->Close();
    m_pRS.Release();
    }
  bHasHeadings = (flags & DBH_HasHeadings)!=0;
  bTrimStrings = (flags & DBH_TrimStrings)!=0;

  try
    {
    //HRESULT hr=m_pCN.CreateInstance(__uuidof(ADODB::Connection));
    HRESULT hr=m_pRS.CreateInstance(__uuidof(ADODB::Recordset));
    if (FAILED(hr))
      {
      LogError("TagHelp", 0, "ADODB::Recordset not Created");
      return false;
      }
    }
  catch (_com_error &e)
    {
    SetErrorInfo(e, "Open Excel RecordSet");
    return false;
    }

  CString Connect;
  Connect.Format("Provider = '%s';Data Source = '%s';Extended Properties = '%s';",// HDR=No';", 
                "Microsoft.Jet.OLEDB.4.0", 
                (LPCTSTR)sFilename,
                "Excel 8.0", "No");
 
  m_bsConnect=Connect;
  try
    {
    CString Src;
    Src.Format("SELECT * FROM[%sA%d:BZ65536]", (const char*)sSheetName, 1);//FieldNamesRow);
    _bstr_t Source(Src);
    m_pRS->Open(Source, m_bsConnect, 
      (flags & DBH_ForwardOnly) ? ADODB::adOpenForwardOnly:ADODB::adOpenKeyset, 
      (flags & DBH_ReadOnly)    ? ADODB::adLockOptimistic/*ReadOnly*/:ADODB::adLockOptimistic, 
      -1);
    }
  catch(_com_error &e)
    {
    SetErrorInfo(e, "Open Excel RecordSet");
    return false;
    }

  iLineNumber = 1;//FieldNamesRow;
  return TRUE;
  }

//---------------------------------------------------------------------------

void CDBHelperBase::SetErrorInfo(_com_error &e, LPCTSTR strMsg)
  {
  CString strErrCode;
  m_ErrNo=e.Error();
  m_ErrStr = strErrCode;
  m_ErrStr += (LPCTSTR)e.Description();           
  m_ErrStr += e.ErrorMessage();           
  
  //m_ErrStr += _T(": ");
  //m_ErrStr += e->m_pErrorInfo->m_strDescription;
  }

//===========================================================================
//
//
//
//===========================================================================

CDBHelper::CDBHelper()
  {
  pDB=NULL;
  }

//---------------------------------------------------------------------------

CDBHelper::CDBHelper(char* Dir, char* Filename, char* SheetName)
  {
  CommonConstruct(Dir, Filename, SheetName);
  }

//---------------------------------------------------------------------------

CDBHelper::CDBHelper(char* Filename, char* SheetName)
  {
  CommonConstruct("", Filename, SheetName);
  }

//---------------------------------------------------------------------------

void CDBHelper::Reset(char* Dir, char* Filename, char* SheetName)
  {
  delete pDB;
  CommonConstruct(Dir, Filename, SheetName);
  }

//---------------------------------------------------------------------------

void CDBHelper::Reset(char* Filename, char* SheetName)
  {
  delete pDB;
  CommonConstruct("", Filename, SheetName);
  }

//---------------------------------------------------------------------------

void CDBHelper::CommonConstruct(char* Dir, char* Filename, char* SheetName)
  {
  Strng Ext;
  Ext.FnExt(Filename);
  if (Ext.XStrICmp(".XLS")==0)
    {
    pDB=new CExcelDBHelper(Dir, Filename, SheetName);
    }
  else
    pDB=new CDBHelperBase(Dir, Filename, SheetName);
  }

//---------------------------------------------------------------------------

CDBHelper::~CDBHelper()
  {
  delete pDB;
  }

//---------------------------------------------------------------------------

BOOL CDBHelper::Open(long DBConnectID, char* FirstFieldName/*=NULL*/, DWORD flags/*=DBH_HasHeadings|DBH_ForwardOnly|DBH_TrimStrings */)
  {
  return pDB->Open(DBConnectID, FirstFieldName, flags);
  }

//---------------------------------------------------------------------------

short CDBHelper::GetFieldCount()
  {
  return pDB->GetFieldCount();
  }

//---------------------------------------------------------------------------

BOOL CDBHelper::CheckFieldNames(char ** FieldNames)
  {
  return pDB->CheckFieldNames(FieldNames);
  }

//---------------------------------------------------------------------------

BOOL CDBHelper::CheckOptionalFieldNames(char ** FieldNames)
  {
  return pDB->CheckOptionalFieldNames(FieldNames);
  }

//---------------------------------------------------------------------------

BOOL CDBHelper::IsEOF()
  {
  return pDB->IsEOF();
  }

//---------------------------------------------------------------------------

void CDBHelper::MoveNext()
  {
  pDB->MoveNext();
  }

//---------------------------------------------------------------------------

void CDBHelper::MoveFirst()
  {
  pDB->MoveFirst();
  }

//---------------------------------------------------------------------------

BOOL CDBHelper::GetNextRow(CArray <COleVariant, COleVariant&> &Values)
  {
  return pDB->GetNextRow(Values);
  };

//---------------------------------------------------------------------------

BOOL CDBHelper::GetNextRow(CStringArray& Values)
  {
  return pDB->GetNextRow(Values);
  }

//---------------------------------------------------------------------------

void CDBHelper::Close()
  {
  pDB->Close();
  }

//---------------------------------------------------------------------------

int CDBHelper::GetLineNo()
  {
  return pDB->GetLineNo();
  }

//---------------------------------------------------------------------------

int CDBHelper::FieldIndex(int i)
  {
  return pDB->FieldIndexes[i];
  }

//---------------------------------------------------------------------------

int CDBHelper::NFieldIndexes()
  {
  return pDB->FieldIndexes.GetSize();
  }

//===========================================================================

void MyAddField(ADOX::_TablePtr pTbl, ADOX::_CatalogPtr pCat, LPCTSTR Name, ADOX::DataTypeEnum Type, long Size, ADOX::ColumnAttributesEnum Att/*=ADOX::ColumnAttributesEnum(0)*/)
  {
  ADOX::_ColumnPtr pCol=ADOX::_ColumnPtr(__uuidof(ADOX::Column));
  //dbgpln("MyAddField %3i %4i %2i %s", Type, Size, Att, Name);
  pCol->Name=Name;
  pCol->Type=Type;
  pCol->DefinedSize=Size;
  if (Type==ADOX::adBoolean) // it appears that adBooleans are allways nullable ???? CNM
    Att=ADOX::ColumnAttributesEnum(Att&~ADOX::adColNullable);
  pCol->Attributes=Att;
  pTbl->Columns->Append((IDispatch*)pCol, Type, Size);
  }

//===========================================================================
