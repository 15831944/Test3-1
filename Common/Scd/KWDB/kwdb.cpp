// KWDatabase.cpp : Defines the initialization routines for the DLL.
//
// NOTES:
//
//  MW:
//  I don't like using so many try and catch sections but as the DAO functions
//  throw exceptions rather than return error codes, I don't really have any
//  other choice.
//
#include "stdafx.h"
#include <afxdllx.h>

#define __KWDB_CPP
#include "kwdb.h"
#include "crack.h"
#include "dbhelper.h"
#include <oledberr.h>
//#include "optoff.h"

#include "licbase.h"

//#pragma chCHECKIT(temporary)
//#pragma warning chCHECKIT(temporary)

#pragma warning( push , 3 )

#include "sc_defs.h"              
#include "gpfuncs.h"
#pragma LIBCOMMENT("..\\..\\com\\ScdIF\\", "\\ScdIF" )
#pragma LIBCOMMENT("..\\..\\scd\\ScdLib\\", "\\ScdLib" )

#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF", "adEOF"), implementation_only
#import "c:\Program Files\Common Files\system\ado\msadox.dll" implementation_only
#import "c:\Program Files\Common Files\system\ado\msjro.dll" implementation_only

#pragma warning( pop )


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define dbgCreateFields  0
#define dbgRecSets       0
#define dbgTiming        0

KWDatabase* pTheDB = NULL;

static const char * const strProj = _T("PROJ");

#pragma warning( disable : 4100 )

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
  {
#ifdef _DEBUG
  lpReserved = lpReserved;
  hInstance = hInstance;
#endif
  if (dwReason == DLL_PROCESS_ATTACH)
    {
    TRACE1("KWDB.DLL Initializing %08x!\n", hInstance);
    }
  else if (dwReason == DLL_PROCESS_DETACH)
    {
    TRACE0("KWDB.DLL Terminating!\n");
    }
  return 1;   // ok
  }

#pragma warning( default : 4100 )


KWDatabase::KWDatabase() 
  {
  m_bWithSCDOptions=FALSE;
  m_bFullTagWithOwner=TRUE;
  m_bWithRevisionNo=FALSE;
  m_bObjsWithSeqNo=FALSE;
  m_bHoldLclTransactions=FALSE;
  m_bGlblTransactionBusy=FALSE;
  m_bErrorOccured=FALSE;

  m_strObjects = _T("$Objects");
  m_strConnections = _T("$Connections");

  m_lRevisionNo = 1;
  m_lSequenceNo = 0;

  m_strDefaultOwner = strProj;

  }

//---------------------------------------------------------------------------

KWDatabase::~KWDatabase()
  {
  CloseAllTblRecordsets();
  RemoveAllFieldDefs();
  }

//---------------------------------------------------------------------------

#pragma warning(disable: 4100)
#ifdef _DEBUG
#pragma warning(disable: 4189)
#endif

// Open an existing database
BOOL KWDatabase::OpenDB(long DBFmt, 
                        LPCTSTR lpszName, 
                        BOOL    bExclusive, 
                        BOOL    bReadOnly/*, 
                                         LPCTSTR lpszConnect*/ )
  {
  BOOL bResult = TRUE;

  try
    {
    HRESULT hr=S_OK;
    CloseDB();

    ASSERT(m_pCat==NULL);
    ASSERT(m_pCnn==NULL);
    try
      {
      m_pCnn=ADODB::_ConnectionPtr(__uuidof(ADODB::Connection));
      m_pCat=ADOX::_CatalogPtr(__uuidof (ADOX::Catalog));

      CString S;
      S.Format(DBConnectOpenString(DBFmt), lpszName);
      //m_pCat->ActiveCollection(_bstr_t(S));
      try
      {
        m_pCnn->Open(_bstr_t(S), "", "", ADODB::adConnectUnspecified);
      }
      catch(_com_error)
      {
        S.AppendFormat("Jet OLEDB:Database Password=test;");
        m_pCnn->Open(_bstr_t(S), "", "", ADODB::adConnectUnspecified);
        gs_EncryptNDemos.encryptedPGM = true;
      }
      m_pCat->PutActiveConnection(_variant_t((IDispatch*)m_pCnn));

      BuildFieldDefMap();//this can take a "long" time!
      }
    catch(_com_error &e)
      {
      KWDisplayException(e, "Open DB");
      }

    //Create(lpszName, lpszLocale, dwOptions);
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, _T("Unable to open Database\n") );
    RemoveAllFieldDefs();
    return FALSE;
    }

  return bResult;
  }

//---------------------------------------------------------------------------

void KWDatabase::BuildFieldDefMap(void)
  {
  HRESULT hr=S_OK;
  CString       strKey;

  RemoveAllFieldDefs();

  //check that default tables exist...
  if (!IsExistentClass(GetPropertiesTableName()))
    {
    try
      {
      // Create empty default tables
      CreateClassesTable();
      CreatePropertiesTable();
      if (m_bWithSCDOptions)
        {
        CreateObjectsTable();
        CreateConnectivityTable();
        }
      }
    catch(_com_error & e)
      {
      KWDisplayException(e, _T("Unable to create default tables\n"));
      }
    }

  // Open Properties for use as non-queried table
  ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
  pRS->Open(GetPropertiesTableName(), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic/*ReadOnly*/, ADODB::adCmdTableDirect);
  //   pRS->Open(dbOpenTable, GetPropertiesTableName(), dbDenyRead);

  ASSERT(pRS->Fields->GetCount() == NUM_FIELDS_IN_MODELDEFS);

  // Build map of all field definitions in 'Properties'
  while (!pRS->adEOF)
    {
    KWFieldDef * fd = new KWFieldDef();
    BuildKWFieldDefFromPropertiesRecord(pRS, fd);
    strKey.Format(_T("%s.%s"), fd->strClassName, fd->strFieldName);

    if (IsExistentClass(fd->strClassName))
      {
      ADOX::_ColumnPtr pCol;
      try 
        {
        pCol=m_pCat->Tables->GetItem((LPCTSTR)fd->strClassName)->Columns->GetItem((LPCTSTR)fd->strFieldName);
        }
      catch(_com_error & e)
        {
        TRACE("Error with field: %s\n", (const char*)strKey);

        CString S;
        S.Format("GetFieldProperties %s.%s", (LPCTSTR)fd->strClassName, (LPCTSTR)fd->strFieldName);
        KWDisplayException(e, S);
        delete fd;
        fd = NULL;
        // Only interested in 'Item not found' exception
        //if (e.m_pErrorInfo==NULL || e->m_pErrorInfo->m_lErrorCode != 3265)
        //  throw e; // re-throwing
        //else
        }

      if (fd)
        {
        //_asm int 3; // TODO
        fd->bRequired        = ((pCol->Attributes & ADODB::adFldIsNullable)==0);//fi.m_bRequired;
        //fd->bAllowZeroLength = fi.m_bAllowZeroLength;
        //fd->nOrdinalPosition = fi.m_nOrdinalPosition;

        //m_mapFieldDefs[strKe] = fd;
        fd->bInMap=TRUE;
        m_mapFieldDefs.SetAt(strKey, fd);

        }
      }
    else
      {
      TRACE("Error with field: %s\n", (const char*)strKey);

      delete fd;
      pRS->Delete(ADODB::adAffectCurrent);
      }

    pRS->MoveNext();
    }
  pRS->Close();
  }


//---------------------------------------------------------------------------
// Attempt to create a new Access Database with the given name (full path name)
BOOL KWDatabase::CreateDB(long DBFmt, LPCTSTR lpszName, /*LPCTSTR lpszLocale,*/ int dwOptions ) 
  {
  ASSERT(m_pCat==NULL);
  ASSERT(m_pCnn==NULL);
  try
    {
    m_pCnn=ADODB::_ConnectionPtr(__uuidof(ADODB::Connection));
    m_pCat=ADOX::_CatalogPtr(__uuidof(ADOX::Catalog));

    CString S;
    S.Format(DBConnectCreateString(DBFmt), lpszName);
    m_pCat->Create(_bstr_t(S));
    m_pCnn=m_pCat->GetActiveConnection();
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, _T("Unable to create Database\n") );
    return FALSE;
    }

  try
    {
    // Create empty default tables
    CreateClassesTable();
    CreatePropertiesTable();
    if (m_bWithSCDOptions)
      {
      CreateObjectsTable();
      CreateConnectivityTable();
      }
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, _T("Unable to create default tables\n"));
    return FALSE;
    }

  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL KWDatabase::VerifyDB(void)
  {
  // MW ??? TODO

  // Verify structural integrity

  // Verify entries : each obj. instance must exist in tag register
  //                  each entry in the Properties table must represent a column in a table
  // 

  return TRUE;
  }

//---------------------------------------------------------------------------
// Create a new instance (i.e. an object) of a previously defined class
// (i.e. a table). 'strObjName' is the name of the object within
// this database. A unique 'full' name is constructed by concatenating the 
// Owner name, a period '.' and Object name (e.g. OwnedByMe.AnObject)
// If 'strOwnerName' is specified as NULL, owner will default to PROJ which
// indicates it has global scope
BOOL KWDatabase::CreateObject
(
 LPCTSTR strObjName, 
 LPCTSTR strClassName,
 LPCTSTR strClassVersion,
 LPCTSTR strPrimaryCfg,
 LPCTSTR strOwnerName
 )
  {
  return( CreateObjectEx(strObjName, strClassName, strClassVersion, strPrimaryCfg, strOwnerName, NULL) );
  }

//---------------------------------------------------------------------------

BOOL KWDatabase::CreateObjectEx
(
 LPCTSTR strObjName,
 LPCTSTR strClassName,
 LPCTSTR strClassVersion,
 LPCTSTR strPrimaryCfg,
 LPCTSTR strOwnerName,
 ...
 )
  {
  ADODB::_RecordsetPtr pRS;
  CString       varFieldName;
  COleVariant   varValue;
  COleVariant   varFieldType;
  CString       FullTag;
  CString       strFind;
  CString       strTmp;
  KWFieldDef *  pkwfd;
  LPCTSTR       strFieldName;
  LPCTSTR       lpszOwn;
  va_list       marker;
  BOOL          bResult = FALSE;
  CMapStringToKWFieldDef mapFields;

  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strObjName != NULL);
  ASSERT(strClassName != NULL);

  if ( !IsExistentClass(strClassName) )
    return FALSE;

  lpszOwn=OwnerName(strOwnerName);
  FullTag=MakeFullTag(lpszOwn, strObjName);

  strFind.Format(_T("Select * From [%s] Where [%s] = '%s'"),
    strClassName,
    GetFullTagFieldName(),
    (LPCTSTR) FullTag );
  try 
    {
    // Check that an object with given name not already defined in this class
#if dbgTiming
    CStopWatch SW;
    SW.Start();
#endif
    INCOMPLETECODE(__FILE__, __LINE__)(__FILE__, __LINE__);
    ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
    pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
    //    pRS->Open(dbOpenDynaset,strFind);

    if ( pRS->adEOF )
      {
      // MW ??? : Also check Objects Table?
      // Object with given name and type does not already exist
      pRS->Close();
      // Build record set of all fields defined for this class 
      strFind.Format(_T("Select * From [%s] Where [%s] = '%s'"),
        GetPropertiesTableName(),
        GetPropertiesClassFieldName(),
        strClassName );
      pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
      // Create a local mapping of all field's type and default in this class
      while ( !pRS->adEOF )
        {
        //pRS->GetFieldValue(GetPropertiesPropertyFieldName(), varFieldName);
        varFieldName=pRS->Fields->GetItem(GetPropertiesPropertyFieldName())->Value;
        strTmp.Format(_T("%s.%s"), strClassName, (LPCTSTR)varFieldName);
        if ( m_mapFieldDefs.Lookup(strTmp, pkwfd) == FALSE )
          throw( _T("Software Error: missing field def") );
        mapFields[strTmp] = pkwfd;
        pRS->MoveNext();
        }
      pRS->Close();
      // Add new record (instance) to relevant class (table)
      //pRS->Open(dbOpenTable, strClassName);
      pRS->Open(strClassName, _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdTableDirect);
      if (LclTransactionRqd())
        m_pCnn->BeginTrans();
      pRS->AddNew();
      pRS->Fields->GetItem(GetFullTagFieldName())->Value=(LPCTSTR)FullTag;
      va_start(marker, strOwnerName);
      while ( (strFieldName = va_arg(marker, LPCTSTR)) != NULL )
        {
        strFind.Format(_T("%s.%s"), strClassName, strFieldName );

        // Convert field name to type via lookup table
        if ( ! m_mapFieldDefs.Lookup(strFind, pkwfd) )
          {
          // ObjectName.Field Not found
          break;
          }

        // Remove this field from the tmp mapping
        mapFields.RemoveKey(strFind);

        switch(pkwfd->nType)
          {
          case ADODB::adUnsignedTinyInt:
          case ADODB::adBoolean:
            // MW ??? : TODO : Hasn't been checked, and probably needs work
            varValue = va_arg(marker, BYTE);
            break;
          case ADODB::adSmallInt:
            varValue = va_arg(marker, short);
            break;
          case ADODB::adInteger:
            varValue = va_arg(marker, long);
            break;
          case ADODB::adSingle:
          case ADODB::adDouble:
            varValue = va_arg(marker, double);
            break;
          case ADODB::adVarWChar:
            varValue = COleVariant(va_arg(marker, LPCTSTR), VT_BSTRT);
            break;
          default:
            AfxMessageBox("SOFTWARE ERROR!: CreateObject() : No such type supported");
            break;
          }

        pRS->Fields->GetItem(strFieldName)->Value=varValue;
        }
      va_end(marker);
      // For each record left in the local mapping, set its default value
      POSITION pos = mapFields.GetStartPosition();
      while (pos != NULL)
        {
        mapFields.GetNextAssoc(pos, strTmp, pkwfd);
        if ( !pkwfd->strDefaultValue.IsEmpty() )
          {
          ConvertToType((LPCTSTR) pkwfd->strDefaultValue, pkwfd->nType, varValue );
          pRS->Fields->GetItem((LPCTSTR) pkwfd->strFieldName)->Value=varValue;
          }
        }

      pRS->Update();
      if (LclTransactionRqd())
        if ( strFieldName )
          m_pCnn->RollbackTrans();
        else
          m_pCnn->CommitTrans();
      pRS->Close();
      // Add entry to tag (object) register
      //SW.Lap();
      if (LclTransactionRqd())
        m_pCnn->BeginTrans();
      //pRS->Open(dbOpenTable, GetObjectsTableName());
      pRS->Open(GetObjectsTableName(), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdTableDirect);
      pRS->AddNew();
      pRS->Fields->GetItem(GetObjectsOwnerFieldName())->Value=lpszOwn;
      pRS->Fields->GetItem(GetTagFieldName())->Value=strObjName; 
      pRS->Fields->GetItem(GetObjectsClassFieldName())->Value=strClassName;
      pRS->Fields->GetItem(GetObjectsClassVerFieldName())->Value=strClassVersion;
      pRS->Fields->GetItem(GetObjectsPrimaryCfgName())->Value=strPrimaryCfg;
      pRS->Update();
      if (LclTransactionRqd())
        m_pCnn->CommitTrans();
      pRS->Close();
      bResult = TRUE;
      }
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, _T(""));
    if ( pRS!=NULL && (pRS->State & ADODB::adStateClosed))
      pRS->Close();
    }

  return bResult;
  }

//---------------------------------------------------------------------------

BOOL KWDatabase::CreateObjectExA(BOOL IsRootObject,
                                 LPCTSTR strObjName,
                                 LPCTSTR strClassName,
                                 LPCTSTR strClassVersion,
                                 LPCTSTR strPrimaryCfg,
                                 LPCTSTR strOwnerName,
                                 DWORD Options,
                                 KWFieldDef ** fieldDefs, 
                                 COleVariant * fieldData, 
                                 int nDefs)
  {
  ADODB::_RecordsetPtr pRS;
  CString       varFieldName;
  COleVariant   varValue;
  COleVariant   varFieldType;
  CString       FullTag;
  CString       strFind;
  CString       strTmp;
  KWFieldDef *  pkwfd;
  //LPCTSTR       strFieldName;
  LPCTSTR       lpszOwn;
  //va_list       marker;
  BOOL          bResult = FALSE;
  int           iDef = -1;
  CMapStringToKWFieldDef mapFields;

  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strObjName != NULL);
  ASSERT(strClassName != NULL);

  if ( !IsExistentClass(strClassName) )
    return FALSE;

  lpszOwn=OwnerName(strOwnerName);
  FullTag=MakeFullTag(lpszOwn, strObjName);

  try 
    {
    // Check that an object with given name not already defined in this class
#if dbgTiming
    CStopWatch SW;
    SW.Start();
#endif
    int Exists=0; // Assume does not exist
    if (Options & CO_CheckForExistence)
      {
      long iRetryCnt=0;
ReTry:
      try
        {
        ADODB::_RecordsetPtr pRS=GetTblRecordset(strClassName);
        bool UseIndex=ALLOWTAGINDEX && pRS->Supports(ADODB::CursorOptionEnum(ADODB::adSeek|ADODB::adIndex)); 
        if (UseIndex)
          pRS->Index=GetFullTagFieldName();
        if (m_bWithRevisionNo)
          {
          if (UseIndex)
            pRS->Seek(MyKeyArray(m_lRevisionNo, (LPCTSTR)FullTag), ADODB::adSeekFirstEQ);
          else
            pRS->Filter=(LPCTSTR)TagFilterString(m_lRevisionNo, FullTag, "=");
          }
        else
          {
          if (UseIndex)
            pRS->Seek(MyKeyArray((LPCTSTR)FullTag), ADODB::adSeekFirstEQ);
          else
            pRS->Filter=(LPCTSTR)TagFilterString(FullTag, "=");
          }
        Exists = (!pRS->adEOF);
        }
      catch (_com_error &e)
        {
        if (e.Error()==DB_E_ROWSNOTRELEASED && iRetryCnt++<2)
          {
          CloseTblRecordset(strClassName);
          goto ReTry;
          }
        throw;
        }
      }

    if (!Exists)
      {
      if (Options & CO_SetDefaultValues)
        {
        // Build record set of all fields defined for this class 
        strFind.Format(_T("Select * From [%s] Where [%s] = '%s'"),
          GetPropertiesTableName(),
          GetPropertiesClassFieldName(),
          strClassName );
        ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
        pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
        // Create a local mapping of all field's type and default in this class
        while ( !pRS->adEOF )
          {
          varFieldName=pRS->Fields->GetItem(GetPropertiesPropertyFieldName())->Value;
          strTmp.Format(_T("%s.%s"), strClassName, (LPCTSTR)varFieldName);
          if ( m_mapFieldDefs.Lookup(strTmp, pkwfd) == FALSE )
            throw( _T("Software Error: missing field def") );
          mapFields[strTmp] = pkwfd;
          pRS->MoveNext();
          }
        pRS->Close();
        }
      // Add new record (instance) to relevant class (table)
      ADODB::_RecordsetPtr pRS1=GetTblRecordset(strClassName);
      //pRS->Open(dbOpenTable, strClassName);
      if (LclTransactionRqd())
        m_pCnn->BeginTrans();
      pRS1->AddNew();

      const int DbgIt = 0;
      if (DbgIt)
        dbgpln("---------        %s = %s", GetFullTagFieldName(), (LPCTSTR)FullTag);
      pRS1->Fields->GetItem(GetFullTagFieldName())->Value=(LPCTSTR)FullTag;
      if (m_bWithRevisionNo)
        {
        if (DbgIt)
          dbgpln("                 %s = %i", GetRevisionNoFieldName(), m_lRevisionNo);
        pRS1->Fields->GetItem(GetRevisionNoFieldName())->Value=m_lRevisionNo;
        }
      for (iDef=0; iDef<nDefs; iDef++)
        {
        if (DbgIt)
          {
          USES_CONVERSION;
          COleVariant VS=fieldData[iDef];
          if (VS.vt==VT_BSTR)
            dbgpln("              %2i %s = %s", fieldData[iDef].vt, fieldDefs[iDef]->strFieldName, OLE2CT(VS.bstrVal));
          else
            dbgpln("              %2i %s", fieldData[iDef].vt, fieldDefs[iDef]->strFieldName);
          }
        pRS1->Fields->GetItem((LPCTSTR)fieldDefs[iDef]->strFieldName)->Value=fieldData[iDef];
        }
      iDef = -1;
      //          pRS->Update();

      if (Options & CO_SetDefaultValues)
        {
        if (DbgIt)
          dbgpln("                 SetDEFAULT");
        // For each record left in the local mapping, set its default value
        POSITION pos = mapFields.GetStartPosition();
        while (pos != NULL)
          {
          mapFields.GetNextAssoc(pos, strTmp, pkwfd);
          if ( !pkwfd->strDefaultValue.IsEmpty() )
            {
            ConvertToType((LPCTSTR) pkwfd->strDefaultValue, pkwfd->nType, varValue );
            pRS1->Fields->GetItem((LPCTSTR) pkwfd->strFieldName)->Value=varValue;
            }
          }
        }

      try
        {
        pRS1->Update();
        //dbgpln(" >>");
        }
      catch(_com_error &e)
        {
        KWDisplayException(e, "Update Record on insert");
        // ignore it;
        //dbgpln(" >> ---------------------------DUPICATE");
        }

      //if ( strFieldName )
      //   m_pCnn->RollbackTrans();
      //else
      if (LclTransactionRqd())
        m_pCnn->CommitTrans();
      //pRS->Close();
      // Add entry to tag (object) register
      //SW.Lap();
      if (IsRootObject)
        {
        if (LclTransactionRqd())
          m_pCnn->BeginTrans();
        ADODB::_RecordsetPtr pRS2=GetTblRecordset(GetObjectsTableName());
        //pRS->Open(dbOpenTable, GetObjectsTableName());
        pRS2->AddNew();
        pRS2->Fields->GetItem(GetObjectsOwnerFieldName())->Value=lpszOwn;
        pRS2->Fields->GetItem(GetTagFieldName())->Value=strObjName; 

        if (m_bWithRevisionNo)
          pRS2->Fields->GetItem(GetRevisionNoFieldName())->Value=m_lRevisionNo; 

        if (m_bObjsWithSeqNo)
          {
          m_lSequenceNo++;
          pRS2->Fields->GetItem(GetSequenceNoFieldName())->Value=m_lSequenceNo; 
          }

        pRS2->Fields->GetItem(GetObjectsClassFieldName())->Value=strClassName;
        pRS2->Fields->GetItem(GetObjectsClassVerFieldName())->Value=strClassVersion;
        pRS2->Fields->GetItem(GetObjectsPrimaryCfgName())->Value=strPrimaryCfg;
        pRS2->Update();
        if (LclTransactionRqd())
          m_pCnn->CommitTrans();
        //pRS2->m_pRS->Close();
        }
      bResult = TRUE;
      }
    }
  catch(_com_error & e)
    {
    // Turf Duplicates
    KWDisplayException(e, "CreateObjectEx");
    // ??
    ////if (e->m_pErrorInfo->m_lErrorCode!=3022)
    ////  {
    ////  char Buff[128];
    ////  sprintf(Buff, "CreateObjectExA(%s) %s", (LPCTSTR)FullTag, iDef>=0 ? fieldDefs[iDef]->strFieldName : "");
    ////  KWDisplayException(e, Buff);//_T(""));
    ////  }
    //if ( pRS->IsOpen() )
    //   pRS->Close();
    }

  return bResult;
  }
///////////////////////////////////////////////////////////////////////////////////////////

BOOL KWDatabase::SetFieldNull(LPCTSTR strObjName, LPCTSTR strFieldName)
  {
  ADODB::_RecordsetPtr pRS;
  CString       strFind;
  CString       varObjClass;
  BOOL          bResult = FALSE;

  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strObjName != NULL);
  ASSERT(strFieldName != NULL);

  try
    {
    strFind.Format(_T("Select * From [%s] Where [%s] = '%s'"),
      GetObjectsTableName(),
      GetObjectsTagFieldName(),
      strObjName );

    pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
    pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);

    if (!pRS->adEOF)
      {
      varObjClass=pRS->Fields->GetItem(GetObjectsClassFieldName())->Value;
      pRS->Close();

      strFind.Format(_T("Select * From [%s] Where [%s] = '%s'"),
        (LPCTSTR)varObjClass,
        GetFullTagFieldName(),
        strObjName );

      pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
      if ( !pRS->adEOF )
        {
        //pRS->SetFieldValueNull(strFieldName);
        _variant_t Null;
        INCOMPLETECODE(__FILE__, __LINE__);      // check is this null ie VT_NULL
        pRS->Fields->GetItem(strFieldName)->Value=Null;
        pRS->Update();
        bResult = TRUE;
        }
      pRS->Close();
      }
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, _T(""));
    if ((pRS!=NULL) && (pRS->State&ADODB::adStateOpen))
      pRS->Close();
    }

  return bResult;
  }


///////////////////////////////////////////////////////////////////////////////////////////

BOOL KWDatabase::SetFieldValue(LPCTSTR strObjName,
                               LPCTSTR strFieldName,
                               LPCTSTR strValue)
  {
  return ( SetFieldValue(strObjName, strFieldName, COleVariant(strValue,VT_BSTRT)) );
  }

BOOL KWDatabase::SetFieldValue(LPCTSTR  strObjName,
                               LPCTSTR  strFieldName,
                               CString& strValue)
  {
  return ( SetFieldValue(strObjName, strFieldName, COleVariant(strValue)) );
  }

BOOL KWDatabase::SetFieldValue(LPCTSTR strObjName,
                               LPCTSTR strFieldName,
                               double  dValue)
  {
  return ( SetFieldValue(strObjName, strFieldName, COleVariant(dValue)) );
  }

BOOL KWDatabase::SetFieldValue(LPCTSTR strObjName,
                               LPCTSTR strFieldName,
                               short   sValue)
  {
  return ( SetFieldValue(strObjName, strFieldName, COleVariant(sValue)) );
  }

BOOL KWDatabase::SetFieldValue(LPCTSTR strObjName,
                               LPCTSTR strFieldName,
                               long    iValue)
  {
  return ( SetFieldValue(strObjName, strFieldName, COleVariant(iValue)) );
  }


BOOL KWDatabase::SetFieldValue
(
 LPCTSTR            strObjName,
 LPCTSTR            strFieldName,
 const COleVariant& varValue
 )
  {
  ADODB::_RecordsetPtr pRS;;
  CString       strFind;
  COleVariant   varObjClass;
  COleVariant   varOwner;
  BOOL          bResult = FALSE;

  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strObjName != NULL);
  ASSERT(strFieldName != NULL);

  try
    {
    strFind.Format(_T("Select * From [%s] Where [%s] = '%s'"),
      GetObjectsTableName(),
      GetObjectsTagFieldName(),
      strObjName );

    // Search for this object
    pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
    pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);

    if (!pRS->adEOF)
      {
      varObjClass=pRS->Fields->GetItem(GetObjectsClassFieldName())->Value;
      varOwner=pRS->Fields->GetItem(GetObjectsOwnerFieldName())->Value;
      pRS->Close();

      CString FullTag=MakeFullTag(V_BSTRT(&varOwner), strObjName );

      strFind.Format(_T("Select * From [%s] Where [%s] = '%s'"),
        V_BSTRT(&varObjClass),
        GetFullTagFieldName(),
        (LPCTSTR)FullTag);


      pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
      pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
      //pRS->Open(dbOpenDynaset, strFind, dbDenyWrite);
      if ( !pRS->adEOF )
        {
        //pRS->Edit();
        pRS->Fields->GetItem(strFieldName)->Value=varValue;
        pRS->Update();
        bResult = TRUE;
        }
      pRS->Close();
      }
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, _T(""));
    if ((pRS!=NULL) && (pRS->State&ADODB::adStateOpen))
      pRS->Close();
    }

  return bResult;
  }


// Set field values using pairs of params: e.g. "FieldName1", value, "FieldName2", value ...
// The type of value on the parameter stack will be interpreted via each field's
// type (queried from the database)
// Parameter list 
// This 
BOOL KWDatabase::SetFieldValues( LPCTSTR strObjName, ... )
  {
  ADODB::_RecordsetPtr pRS;
  CString       strFind;
  COleVariant   varObjClass;
  COleVariant   varOwner;
  COleVariant   varValue;
  va_list       marker;
  KWFieldDef *  pkwfd;
  LPCTSTR       strFieldName;
  BOOL          bResult = FALSE;

  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strObjName != NULL);

  try
    {
    strFind.Format(_T("Select * From [%s] Where [%s] = '%s'"),
      GetObjectsTableName(),
      GetObjectsTagFieldName(),
      strObjName );

    // Search for this object
    pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
    pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);

    if (!pRS->adEOF)
      {
      varObjClass=pRS->Fields->GetItem(GetObjectsClassFieldName())->Value;
      varOwner=pRS->Fields->GetItem(GetObjectsOwnerFieldName())->Value;
      pRS->Close();

      CString FullTag=MakeFullTag(V_BSTRT(&varOwner), strObjName );

      strFind.Format(_T("Select * From [%s] Where [%s] = '%s'"),
        V_BSTRT(&varObjClass),
        GetFullTagFieldName(),
        (LPCTSTR) FullTag);

      pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
      pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
      if ( !pRS->adEOF )
        {
        if (LclTransactionRqd())
          m_pCnn->BeginTrans();
        //pRS->Edit();
        va_start(marker, strObjName);
        while ( (strFieldName = va_arg(marker, LPCTSTR)) != NULL )
          {
          strFind.Format(_T("%s.%s"), V_BSTRT(&varObjClass), strFieldName );

          // Convert field name to type via lookup table
          if ( ! m_mapFieldDefs.Lookup(strFind, pkwfd) )
            {
            // ObjectName.Field Not found
            break;
            }

          switch(pkwfd->nType)
            {
            case ADODB::adUnsignedTinyInt:
            case ADODB::adBoolean:
              // MW ??? : Hasn't been checked, and probably needs work
              varValue = va_arg(marker, BYTE);
              break;
            case ADODB::adSmallInt:
              varValue = va_arg(marker, short);
              break;
            case ADODB::adInteger:
              varValue = va_arg(marker, long);
              break;
              //varValue = va_arg(marker, float);
              //break;
            case ADODB::adSingle:
            case ADODB::adDouble:
              varValue = va_arg(marker, double);
              break;
            case ADODB::adVarWChar:
              varValue = COleVariant(va_arg(marker, LPCTSTR), VT_BSTRT);
              break;

            default:
              AfxMessageBox("SOFTWARE ERROR!: SetFieldValues() : No such type supported");
              break;
            }

          pRS->Fields->GetItem(strFieldName)->Value=varValue;
          }
        pRS->Update();
        va_end(marker);
        if ( ! strFieldName )
          {
          if (LclTransactionRqd())
            m_pCnn->CommitTrans();
          bResult = TRUE;
          }
        else
          {
          if (LclTransactionRqd())
            m_pCnn->RollbackTrans();
          }
        pRS->Close();
        }
      }
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, _T(""));
    if ((pRS!=NULL) && (pRS->State&ADODB::adStateOpen))
      pRS->Close();
    }

  return bResult;
  }

// Set field values two arrays of params: i.e. "FieldNames", Values,
// The type of value on the parameter stack will be interpreted via each field's
// type (queried from the database)
// Parameter list 
// This 
int KWDatabase::SetFieldValuesA(LPCTSTR strObjName, KWFieldDef ** fieldDefs, COleVariant * fieldData, int nDefs)
  {
  ADODB::_RecordsetPtr pRS;
  CString       strFind;
  COleVariant   varObjClass;
  COleVariant   varOwner;
  COleVariant   varValue;
  int           iDef;
  BOOL          bResult = FALSE;

  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strObjName != NULL);

  try
    {
    strFind.Format(_T("Select * From [%s] Where [%s] = '%s'"),
      GetObjectsTableName(),
      GetObjectsTagFieldName(),
      strObjName );

    // Search for this object
    pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
    pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);

    if (!pRS->adEOF)
      {
      varObjClass=pRS->Fields->GetItem(GetObjectsClassFieldName())->Value;
      varOwner=pRS->Fields->GetItem(GetObjectsOwnerFieldName())->Value;
      pRS->Close();

      CString FullTag=MakeFullTag(V_BSTRT(&varOwner), strObjName );
      // Find the object
      strFind.Format(_T("Select * From [%s] Where [%s] = '%s'"),
        V_BSTRT(&varObjClass),
        GetFullTagFieldName(),
        (LPCTSTR)FullTag);

      pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
      pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
      if ( !pRS->adEOF )
        {
        if (LclTransactionRqd())
          m_pCnn->BeginTrans();
        //pRS->Edit();
        for (iDef=0; iDef<nDefs; iDef++)
          {
          pRS->Fields->GetItem((LPCTSTR)fieldDefs[iDef]->strFieldName)->Value=fieldData[iDef];
          }
        pRS->Update();
        if (LclTransactionRqd())
          m_pCnn->CommitTrans();
        bResult = TRUE;
        pRS->Close();
        }
      }
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, _T(""));
    if ((pRS!=NULL) && (pRS->State&ADODB::adStateOpen))
      pRS->Close();
    }

  return bResult;
  }


/////////////////////////////////////////////////////////////////////////////////

BOOL KWDatabase::GetFieldValue(LPCTSTR strObjName,
                               LPCTSTR strFieldName,
                               LPTSTR  strValue)
  {
  COleVariant var;
  BOOL        bResult;

  bResult = GetFieldValue(strObjName, strFieldName, var);
  strcpy( strValue, (LPTSTR)var.pbVal );

  return bResult;
  }

BOOL KWDatabase::GetFieldValue(LPCTSTR  strObjName,
                               LPCTSTR  strFieldName,
                               CString& strValue)
  {
  COleVariant var;
  BOOL        bResult;

  bResult = GetFieldValue(strObjName, strFieldName, var);
  strValue = var.pbVal;

  return bResult;
  }

BOOL KWDatabase::GetFieldValue(LPCTSTR  strObjName,
                               LPCTSTR  strFieldName,
                               float&   fValue )
  {
  COleVariant var;
  BOOL        bResult;

  bResult = GetFieldValue(strObjName, strFieldName, var);
  fValue = var.fltVal;

  return bResult;
  }

BOOL KWDatabase::GetFieldValue(LPCTSTR strObjName,
                               LPCTSTR strFieldName,
                               double& dValue)
  {
  COleVariant var;
  BOOL        bResult;

  bResult = GetFieldValue(strObjName, strFieldName, var);
  dValue = var.dblVal;

  return bResult;
  }

BOOL KWDatabase::GetFieldValue(LPCTSTR strObjName,
                               LPCTSTR strFieldName,
                               short&  iValue)
  {
  COleVariant var;
  BOOL        bResult;

  bResult = GetFieldValue(strObjName, strFieldName, var);
  iValue = var.iVal;

  return bResult;
  }

BOOL KWDatabase::GetFieldValue(LPCTSTR strObjName,
                               LPCTSTR strFieldName,
                               long&   lValue)
  {
  COleVariant var;
  BOOL        bResult;

  bResult = GetFieldValue(strObjName, strFieldName, var);
  lValue = var.lVal;

  return bResult;
  }


BOOL KWDatabase::GetFieldValue
(
 LPCTSTR      strObjName,
 LPCTSTR      strFieldName,
 COleVariant& var
 )
  {

  VERIFY(FALSE); //KGA

  ADODB::_RecordsetPtr pRS;
  CString       strFind;
  COleVariant   varObjClass;
  BOOL          bResult = FALSE;

  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strObjName != NULL);
  ASSERT(strFieldName != NULL);

  try
    {
    strFind.Format(_T("Select * From [%s] Where [%s] = '%s'"),
      GetObjectsTableName(),
      GetObjectsTagFieldName(),
      strObjName );

    pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
    pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);

    if (!pRS->adEOF)
      {
      varObjClass=pRS->Fields->GetItem(GetObjectsClassFieldName())->Value;
      pRS->Close();

      strFind.Format(_T("Select * From [%s] Where [%s] = '%s'"),
        V_BSTRT(&varObjClass), // Class (Table) Name
        GetFullTagFieldName(), // Unique Name Field
        strObjName );

      pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
      //pRS->Open(dbOpenDynaset, strFind);
      if ( !pRS->adEOF )
        {
        var=pRS->Fields->GetItem(strFieldName)->Value;
        bResult = TRUE;
        }
      pRS->Close();
      }
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, _T(""));
    if ((pRS!=NULL) && (pRS->State&ADODB::adStateOpen))
      pRS->Close();
    }

  return bResult;
  }

/////////////////////////////////////////////////////////////////////////////////

void KWDatabase::CreateConnectivityTable( )
  {
  try
    {
    ADOX::_TablePtr pTbl=ADOX::_TablePtr(__uuidof(ADOX::Table));
    ADOX::_IndexPtr pIdx=ADOX::_IndexPtr(__uuidof(ADOX::Index));

    pTbl->Name=GetConnectTableName();
    MyAddField(pTbl, m_pCat, GetConnectTagFieldName(),        ADOX::adVarWChar,  0);
    if (m_bWithRevisionNo)
      MyAddField(pTbl, m_pCat, GetRevisionNoFieldName(),      ADOX::adInteger,   4,  ADOX::adColNullable);
    MyAddField(pTbl, m_pCat, GetConnectSrcFieldName(),        ADOX::adVarWChar,  0,  ADOX::adColNullable);
    MyAddField(pTbl, m_pCat, GetConnectDstFieldName(),        ADOX::adVarWChar,  0,  ADOX::adColNullable);
    MyAddField(pTbl, m_pCat, GetConnectSrcIOFieldName(),      ADOX::adInteger,   0,  ADOX::adColNullable);
    MyAddField(pTbl, m_pCat, GetConnectDstIOFieldName(),      ADOX::adInteger,   0,  ADOX::adColNullable);
    MyAddField(pTbl, m_pCat, GetConnectDescFieldName(),       ADOX::adVarWChar, 50,  ADOX::adColNullable);

    m_pCat->Tables->Append(_variant_t((IDispatch*)pTbl));

    pIdx->Name=GetConnectTagFieldName();
    if (m_bWithRevisionNo)
      pIdx->Columns->Append(GetRevisionNoFieldName(),    ADOX::adVarWChar, 0);
    pIdx->Columns->Append(GetConnectTagFieldName(),      ADOX::adVarWChar, 0);
    pIdx->PutPrimaryKey(-1);
    pIdx->PutUnique(-1);

    pTbl->Indexes->Append(_variant_t((IDispatch*)pIdx));
    }
  catch(_com_error &e)
    {
    KWDisplayException(e, "CreateConnectivityTable");
    }
  }


void KWDatabase::CloseDB( void )
  {
  if ((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen))
    {
    CloseAllTblRecordsets();
    RemoveAllFieldDefs();
    m_pCnn->Close();
    m_pCnn.Release();
    m_pCat.Release();
    }
  }

BOOL KWDatabase::CompactDB(long DBFmt, 
                           LPCTSTR lpszDBNameOld, 
                           LPCTSTR lpszDBNameNew)
  {
  try
    {
    JRO::IJetEnginePtr pJet=JRO::IJetEnginePtr(__uuidof(JRO::JetEngine));

    CString SO, SN;
    try
    {
      SO.Format(DBConnectOpenString(DBFmt), lpszDBNameOld);
      SN.Format(DBConnectCreateString(DBFmt), lpszDBNameNew);
      pJet->CompactDatabase(_bstr_t(SO), _bstr_t(SN));
    }
    catch(_com_error)
    {
      SO.AppendFormat("Jet OLEDB:Database Password=test;");
      SN.AppendFormat("Jet OLEDB:Database Password=test;");
      pJet->CompactDatabase(_bstr_t(SO), _bstr_t(SN));
    }

    pJet.Release();
    return TRUE;
    }
  catch(_com_error &e)
    {
    KWDisplayException(e, "Compacting DB");
    }
  return FALSE;
  }

//---------------------------------------------------------------------------
#pragma optimize("", off)
// Does a table with the given name exist in this database?
BOOL KWDatabase::IsExistentClass(LPCTSTR strClassName)
  {
  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strClassName != NULL);

  // Attempt to retrieve info on the given table
  // This seems preferable to an open, since should be faster
  // and have less chance of failing due to security violations
  try
    {
    ADOX::_TablePtr pTbl;
    pTbl=m_pCat->Tables->GetItem(strClassName);
    }
  catch(_com_error & e)
    {
    if (SCODE_FACILITY(e.Error())!=0xA || SCODE_CODE(e.Error())!=ADODB::adErrItemNotFound)
      KWDisplayException(e, "KWDatabase::IsExistentClass");
    return FALSE;
    }

  return TRUE;
  }

#pragma optimize("", on)
//---------------------------------------------------------------------------

BOOL KWDatabase::IsExistentField(LPCTSTR strClassName, LPCTSTR strFieldName)
  {

  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strClassName != NULL);
  ASSERT(strFieldName != NULL);

  try
    {
    ADOX::_ColumnPtr pCol;
    pCol=m_pCat->Tables->GetItem(strClassName)->Columns->GetItem(strFieldName);
    }
  catch(_com_error & e)
    {
    if (SCODE_CODE(e.Error())!=ADODB::adErrItemNotFound)
      KWDisplayException(e, "KWDatabase::IsExistentField");
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

BOOL KWDatabase::IsExistentIndex(LPCTSTR strClassName, LPCTSTR strIndexName)
  {
  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strClassName != NULL);
  ASSERT(strIndexName != NULL);

  try
    {
    ADOX::_IndexPtr pIdx;
    pIdx=m_pCat->Tables->GetItem(strClassName)->Indexes->GetItem(strIndexName);
    }
  catch(_com_error & e)
    {
    if (SCODE_CODE(e.Error())!=ADODB::adErrItemNotFound)
      KWDisplayException(e, "KWDatabase::IsExistentIndex");
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------
// Create a new table (class) in this database, possibly 'derived'
// from the given database table (not necessarily in this database)
// Each column defined in derived table will be created in new table
//
// Pre-requisite: source database must contain a 'Properties' Table.

BOOL KWDatabase::CreateClass(LPCTSTR strNewClassName,
                             LPCTSTR strDerivedClassName)
  {
  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  //ASSERT(strDerivedClassName != NULL );

  BOOL result            = FALSE;
  BOOL bTableInCurrentDB = TRUE;

  try
    {
    // Check if client wants to derive properties from an existing table
    // in the current database
    if ( strDerivedClassName )
      {
      //???ptmpDB = this;
      }
    }
  catch(_com_error & e)
    {
    e;
    // Either unable to open class db
    //???delete ptmpDB;
    //return FALSE;
    }


  try
    {
    ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(_uuidof(ADODB::Recordset));

    // MW ??? : Add new class to Classes table
    // If deriving from an existing class, search for derived class's 
    // base class, strBaseClassName
    pRS->Open(GetClassesTableName(), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdTableDirect);
    //pRS->Open(dbOpenTable, GetClassesTableName(), dbDenyWrite);
    pRS->AddNew();
    pRS->Fields->GetItem(GetClassesClassName())->Value=strNewClassName;
    if ( !strDerivedClassName )
      pRS->Fields->GetItem(GetClassesBaseFieldName())->Value=strNewClassName;
    else
      pRS->Fields->GetItem(GetClassesDerivedFieldName())->Value=strDerivedClassName;

    pRS->Update();
    pRS->Close();

    // For each field defined in the derived table create a duplicate field in the new table
    if ( strDerivedClassName )
      {
      ADODB::_RecordsetPtr pRStmp=ADODB::_RecordsetPtr(_uuidof(ADODB::Recordset));

      CString       strQuery;
      strQuery.Format( _T("Select * From [%s] Where [%s] = '%s'"),
        GetPropertiesTableName(),
        GetPropertiesClassFieldName(),
        strDerivedClassName );

      // Generate set of duplicate fields for new class
      pRStmp->Open(_variant_t(strQuery), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
      while (!pRStmp->adEOF)
        {
        ADOX::_TablePtr pTbl=ADOX::_TablePtr(_uuidof(ADOX::Table));

        KWFieldDef * fd = new KWFieldDef();
        BuildKWFieldDefFromPropertiesRecord(pRStmp, fd);

        pTbl=m_pCat->Tables->GetItem(strDerivedClassName);
        ADOX::_ColumnPtr pCol=pTbl->Columns->GetItem((LPCTSTR)fd->strFieldName);

        fd->strClassName     = strNewClassName;
        fd->bRequired        = (pCol->Attributes & ADOX::adColNullable)==0;////fi.m_bRequired;
        //fd->bAllowZeroLength = fi.m_bAllowZeroLength;
        if (CreateField(fd) == FALSE)
          {
          delete fd;
          throw;
          }
        pRStmp->MoveNext();
        }
      pRStmp->Close();
      }
    else if (m_bWithSCDOptions) // CNM
      {
      CreateField(strNewClassName, GetFullTagFieldName(), "", ADOX::adVarWChar, 255, TRUE, FALSE, 
        _T(""), _T(""), _T("Tag of the Unit"));

      if (m_bWithRevisionNo)
        {
        CreateField(strNewClassName, GetRevisionNoFieldName(), "", ADOX::adInteger, 4, TRUE, FALSE, 
          _T(""), _T(""), _T("Revision Number of Unit"));
        }
      }

    result = TRUE;
    }
  catch(_com_error & e)
    {
    //#ifdef _DEBUG
    if (0)//e.Error()==3022)
      TRACE("Create new Class '%s' failed - allready exists?\n", strNewClassName);
    else
      KWDisplayException(e, _T("Create new Class failed"));
    //#endif
    }

  return result;
  }

//---------------------------------------------------------------------------

BOOL KWDatabase::CreateFullTagIndex(LPCTSTR strClassName)
  {
  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  BOOL result=FALSE;

  CloseTblRecordset(strClassName);

  try
    {
    ADOX::_IndexPtr pIdx=ADOX::_IndexPtr(__uuidof(ADOX::Index));
    pIdx->Name=GetFullTagFieldName();
    if (m_bWithRevisionNo)
      pIdx->Columns->Append(GetRevisionNoFieldName(),   ADOX::adVarWChar, 0);
    pIdx->Columns->Append(GetFullTagFieldName(),        ADOX::adVarWChar, 0);
    pIdx->PutPrimaryKey(-1);
    pIdx->PutUnique(-1);

    m_pCat->Tables->GetItem(strClassName)->Indexes->Append(_variant_t((IDispatch*)pIdx));

    result = TRUE;
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, _T("CreateFullTagIndex failed"));
    }

  return result;
  }

//---------------------------------------------------------------------------

BOOL KWDatabase::CheckFullTagIndex(LPCTSTR strClassName)
  {
  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));

  if (!IsExistentIndex(strClassName, GetFullTagFieldName()))
    return CreateFullTagIndex(strClassName);

  bool IndexIsGood=true;

  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strClassName != NULL);
  //ASSERT(strIndexName != NULL);

  try
    {
    ADOX::_IndexPtr pIdx;
    pIdx=m_pCat->Tables->GetItem(strClassName)->Indexes->GetItem(GetFullTagFieldName());

    if (pIdx->Columns->Count!=2)
      {
      IndexIsGood=false;
      }
    else
      {

      long i=0;
      if (m_bWithRevisionNo)
        {
        ADOX::_ColumnPtr pCol;
        pCol=pIdx->Columns->GetItem(_variant_t(i));

        if ((strcmp(pCol->Name, GetRevisionNoFieldName())!=0) || (pCol->SortOrder == ADOX::adSortDescending))
          IndexIsGood=false;
        i++;
        }
      if ((strcmp(pIdx->Columns->GetItem(i)->Name, GetFullTagFieldName())!=0) ||
        (pIdx->Columns->GetItem(i)->SortOrder == ADOX::adSortDescending))
        IndexIsGood=false;
      }
    if (!IndexIsGood)
      {
      m_pCat->Tables->GetItem(strClassName)->Indexes->Delete(GetFullTagFieldName());
      if (CreateFullTagIndex(strClassName))
        IndexIsGood=TRUE;
      }
    }
  catch(_com_error & e)
    {
    // Only interested in 'Item not found' exception
    //      if (e->m_pErrorInfo->m_lErrorCode == 3265)
    //        bDuplicateIndexName = FALSE;

    KWDisplayException(e, "KWDatabase::CheckFullTagIndex");
    }


  return IndexIsGood==true;
  }

//---------------------------------------------------------------------------

long KWDatabase::GetRevisionNo()
  { 
  return m_bWithRevisionNo ? m_lRevisionNo : -1; 
  };
long KWDatabase::SetRevisionNo(long RevNo) 
  { 
  long Prv=m_lRevisionNo; 
  m_lRevisionNo= m_bWithRevisionNo ? RevNo : -1; 
  return Prv; 
  };
long KWDatabase::FindRevisionNo(LPCTSTR strClassName, DWORD Options)
  {
  ADODB::_RecordsetPtr pRS;
  CString       strFind;
  COleVariant   varObjClass;
  long          lRevNo=-1;
  Options=Options; 

  if (!m_bWithRevisionNo)
    return lRevNo;

  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strClassName != NULL);

  try
    { 
    pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
    strFind.Format(_T("Select Top 1 [%s] From [%s] Order By [%s] Desc"),
      GetRevisionNoFieldName(),
      strClassName,
      GetRevisionNoFieldName() );

    pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);

    if (pRS->adEOF==FALSE)
      {
      COleVariant varRevNo;
      varRevNo=pRS->Fields->GetItem(GetRevisionNoFieldName())->Value;
      lRevNo=V_I4(&varRevNo);
      }
    pRS->Close();
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, _T(""));
    if ((pRS!=NULL) && (pRS->State&ADODB::adStateOpen))
      pRS->Close();
    }

  return lRevNo;
  };

//---------------------------------------------------------------------------

void KWDatabase::RemoveRevisions(int From, int To)
  {
  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));

  if (!m_bWithRevisionNo)
    return;

  CloseAllTblRecordsets();

  try
    {
    for (long iT=0; iT<m_pCat->Tables->Count; iT++)
      {
      CString strFind;
      ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
      ADOX::_TablePtr pTbl=m_pCat->Tables->GetItem(iT);
      try
        {
        if (IsExistentField(pTbl->Name, GetRevisionNoFieldName()))
          {
          for (;;)
            {
            strFind.Format(_T("Select [%s] From [%s] Where [%s] >= %i AND [%s] <= %i "),
              GetRevisionNoFieldName(),
              (LPCTSTR)pTbl->Name,
              GetRevisionNoFieldName(), From,
              GetRevisionNoFieldName(), To );

            pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
            int n=9000; // MaxLocksPerFile limit = 9500
            bool Done=pRS->adEOF!=0;
            while (!Done&& --n>=0)
              {
              pRS->Delete(ADODB::adAffectCurrent);
              pRS->MoveFirst();
              Done=pRS->adEOF!=0;
              }
            pRS->Close();
            if (Done)
              break;
            }
          }
        }
      catch(_com_error & e)
        {
        //if (SCODE_FACILITY(e.Error())!=0xA || SCODE_CODE(e.Error())!=ADODB::adErrItemNotFound)
        KWDisplayException(e, _T("Remove Revisions"));
        if ((pRS!=NULL) && (pRS->State&ADODB::adStateOpen))
          pRS->Close();
        }
      }
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, _T("Remove Revisions"));
    }
  };

//---------------------------------------------------------------------------
// Create a new column in a specified table, as well as adding the
// field definition to 'Properties' table.

BOOL KWDatabase::CreateField(LPCTSTR strClassName,
                             LPCTSTR strFieldName,
                             LPCTSTR strEngUnits,
                             ADOX::DataTypeEnum   nType,
                             short   nTypeSize,
                             BOOL    bRequired,
                             BOOL    bAllowZeroLength,
                             LPCTSTR strDefaultValue,
                             LPCTSTR strDispUnits,
                             LPCTSTR strDescription,
                             long    nHelpID,
                             short   nOrdinalPosition )
  {
  if (!IsValidDBType(nType))
    return FALSE;
  KWFieldDef* fd = new KWFieldDef(strClassName, strFieldName, strEngUnits, nType, nTypeSize,
    nOrdinalPosition, bRequired, bAllowZeroLength, 
    strDefaultValue, strDispUnits, strDescription, nHelpID );
  if (KWDatabase::CreateField(fd) == FALSE)
    {
    delete fd;
    return FALSE;
    }
  return TRUE;
  }


// Update a field's 'variable' parameters. e.g. cannot change the type of an existing
// field.

BOOL KWDatabase::UpdateField
(
 LPCTSTR  strClassName,          // Table name
 LPCTSTR  strFieldName,          // Name of field in above table
 LPCTSTR  strDefaultValue,       // NEW default value for this field
 LPCTSTR  strDispUnits,          // NEW display units for this field
 LPCTSTR  strDescription,        // NEW description for this field
 long     lHelpID                // NEW help topic identifier
 )
  {
  CString       strKey;
  KWFieldDef *  pkwfd;
  BOOL bResult = TRUE;

  strKey.Format(_T("%s.%s"), strClassName, strFieldName);

  // Get a reference to field def
  if ( ! m_mapFieldDefs.Lookup(strKey, pkwfd) )
    return FALSE;

  try
    {
    ADODB::_RecordsetPtr pRS;
    CString strQuery;
    strQuery.Format( _T("Select * From [%s] Where [%s] = '%s' And [%s] = '%s';"),
      GetPropertiesTableName(),
      GetPropertiesClassFieldName(),
      strClassName,
      GetPropertiesPropertyFieldName(),
      strFieldName );

    pRS->Open(_variant_t(strQuery), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);

    // Set should only contain one record
    if ( pRS->GetRecordCount() != 1L )
      {
      throw(_T("Field definition ERROR: Duplicate or not found"));
      }

    // Update map for this field
    pkwfd->strDefaultValue  = strDefaultValue;
    pkwfd->strDispUnits     = strDispUnits;
    pkwfd->strDescription   = strDescription;
    pkwfd->lHelpID          = lHelpID;

    //pRS->Edit();
    pRS->Fields->GetItem(GetPropertiesValueFieldName())->Value=(LPCTSTR)strDefaultValue; 
    pRS->Fields->GetItem(GetPropertiesDispUnitsFieldName())->Value=(LPCTSTR)strDispUnits; 
    pRS->Fields->GetItem(GetPropertiesDescFieldName())->Value=(LPCTSTR)strDescription; 
    pRS->Fields->GetItem(GetPropertiesHelpIDFieldName())->Value=COleVariant(lHelpID);
    pRS->Update();
    pRS->Close();
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, _T(""));
    bResult = FALSE;
    }

  return bResult;
  }



// If field exists, return info describing its parameters

BOOL KWDatabase::GetKWFieldInfo(LPCTSTR strClassName,
                                LPCTSTR strFieldName,
                                KWFieldDef*& fieldInfo )
  {
  CString strKey;

  ASSERT(this);
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strClassName != NULL && strFieldName != NULL);

  // "." character not allowed in table name, so no clash possible
  strKey.Format(_T("%s.%s"), strClassName, strFieldName ); 

  return m_mapFieldDefs.Lookup(strKey, fieldInfo);
  }

BOOL KWDatabase::DeleteAllClassObjects(LPCTSTR strClassName)
  {
  ASSERT((m_pCnn!=NULL) && (m_pCnn->State&ADODB::adStateOpen));
  ASSERT(strClassName != NULL);
  if (1)//IsExistentClass(strClassName))
    {
    try
      {
      ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
      pRS->Open(strClassName, _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdTableDirect);
      if (!pRS->adEOF)
        pRS->MoveFirst();
      while (!pRS->adEOF)
        {
        pRS->Delete(ADODB::adAffectCurrent);
        pRS->MoveFirst();
        }
      pRS->Close();
      }
    catch(_com_error & e)
      {
      KWDisplayException(e, _T("Deleting table content failed"));
      return FALSE;
      }
    }
  return TRUE;
  }

BOOL KWDatabase::BuildKWFieldDefFromPropertiesRecord( ADODB::_RecordsetPtr pRS, KWFieldDef * fd )
  {
  _variant_t var;

  try
    {
    var=pRS->Fields->GetItem(GetPropertiesClassFieldName())->Value;
    fd->strClassName = var;

    var=pRS->Fields->GetItem(GetPropertiesPropertyFieldName())->Value;
    fd->strFieldName = var;

    var=pRS->Fields->GetItem(GetPropertiesEngUnitFieldName())->Value;
    fd->strEngUnits = var;

    var=pRS->Fields->GetItem(GetPropertiesTypeFieldName())->Value;
    fd->nType = ConvertStringTypeToDBType(var);

    var=pRS->Fields->GetItem(GetPropertiesSizeFieldName())->Value;
    fd->nTypeSize = var.iVal;

    var=pRS->Fields->GetItem(GetPropertiesValueFieldName())->Value;
    fd->strDefaultValue = var;

    var=pRS->Fields->GetItem(GetPropertiesDescFieldName())->Value;
    fd->strDescription = var;

    var=pRS->Fields->GetItem(GetPropertiesDispUnitsFieldName())->Value;
    fd->strDispUnits = var;

    var=pRS->Fields->GetItem(GetPropertiesHelpIDFieldName())->Value;
    fd->lHelpID = var.lVal;
    }
  catch(_com_error & e)
    {
    KWDisplayException(e, "BuildKWFieldDefFromPropertiesRecord");
    return FALSE;
    }

  return TRUE;
  }


int KWDatabase::CreateFields(LPCTSTR strClassName, KWFieldDef ** fds, int nDefs)
  {
  HRESULT hr=S_OK;
  ADOX::_TablePtr pTbl;

  ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
  KWFieldDef *  pkwfd;
  CString       strKey;
  BOOL          tdOpenDone=FALSE;           
  BOOL          rsOpenDone=FALSE;           
  int           iDef=0;

  ASSERT(fds != NULL);

#if dbgTiming
  CStopWatch SW;
  SW.Start();
#endif

  CloseTblRecordset(strClassName);

  BOOL ExistingClass = IsExistentClass(strClassName);
  for ( ; iDef<nDefs; iDef++)
    {
    KWFieldDef * fd = fds[iDef];
    strKey.Format(_T("%s.%s"), fd->strClassName, fd->strFieldName);
    BOOL ExistingField = IsExistentField((LPCTSTR)fd->strClassName, (LPCTSTR)fd->strFieldName);
    BOOL ExistingFieldDef = m_mapFieldDefs.Lookup(strKey, pkwfd);

    if (strpbrk((LPCTSTR)fd->strFieldName, ",'[]"))
      DoBreak()
#if dbgCreateFields
      dbgp("CF:%3s %3s %+15.15s:%-15.15s",
      ExistingField ? "Fld":"", ExistingFieldDef ? "Def":"",
      (LPCTSTR)fd->strClassName,(LPCTSTR)fd->strFieldName);
#endif

    // Check that this is not a duplicate property (field) name
    // This logic is here so that this code may be used in a similar fashion
    // to the database usage in SysCad. i.e. calls to CreateField succeed whether
    // or not the field already exists, PROVIDED that the properties match.
    if (ExistingField && ExistingFieldDef)
      {
      // Check that default properties of given and defined fields match
      if ( pkwfd->nType != fd->nType ||
        (pkwfd->nType == ADODB::adVarWChar && pkwfd->nTypeSize != fd->nTypeSize) ||
        pkwfd->strEngUnits != fd->strEngUnits )
        return iDef;

      // We are about to return TRUE to calling function, the newly created
      // field definition will NOT be added to the map, so we must delete it.
      //delete fd;
      //fds[iDef]=pkwfd;
      //fd=pkwfd;
      if (fd->bInMap)
        {
        // implies duplicate field name
#if dbgCreateFields
        dbgpln(" Duplicate Field name");
#endif
        //ASSERT(FALSE);
        };
      //dbgp(" 1"); for (int iii=0; iii<nDefs; iii++) dbgp("%s ", fds[iii]->strFieldName); dbgpln("");
      //return TRUE;
      continue;
      }

    if (!ExistingField)
      {
      // Create a field of type specified by given parameters
      if (!tdOpenDone)
        {
        try
          {

          if (ExistingClass)
            {
            pTbl=m_pCat->Tables->GetItem((LPCTSTR)fd->strClassName);
            }
          else
            {
            pTbl=ADOX::_TablePtr(__uuidof(ADOX::Table));
            pTbl->Name=(LPCTSTR)fd->strClassName;
            m_pCat->Tables->Append(_variant_t((IDispatch*)pTbl));
            }
          tdOpenDone=TRUE;
#if dbgCreateFields
          dbgp(" TDOpn");
#endif
          }
        catch(_com_error & e)
          {
          // locked or not ready
          // if (e->m_pErrorInfo->m_lErrorCode!=3211)
          KWDisplayException(e, _T(strKey));
          goto Done;
          }
        }

      try
        {
        MyAddField(pTbl, m_pCat, (LPCTSTR)fd->strFieldName, fd->nType, fd->nTypeSize, fd->bRequired?ADOX::ColumnAttributesEnum(0):ADOX::adColNullable);

#if dbgCreateFields
        dbgp(" Write");
#endif
        }
      catch(_com_error & e)
        {
        // locked or not ready
        // if (e->m_pErrorInfo->m_lErrorCode!=3211)
        KWDisplayException(e, _T("CreateFields"));
        goto Done;
        }
      }

    if (!ExistingFieldDef)
      {
      try
        {

        // For each record in specified table, set new field to its default value
        // or leave all empty if no value specified.
        //SetFieldDefaultValueInAllRecords(fd);

        // Add field definition to 'Properties' Table
        CString strFind;
        strFind.Format(_T("Select * From [%s] Where [%s] = '%s' And [%s] = '%s'"),
          GetPropertiesTableName(),
          GetPropertiesClassFieldName(),
          fd->strClassName,
          GetPropertiesPropertyFieldName(),
          fd->strFieldName);

        if (!rsOpenDone)
          {
          rsOpenDone=TRUE;
          pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
#if dbgCreateFields
          dbgp(" RSOpn");
#endif
          }
        if (pRS->adEOF)
          pRS->AddNew();
        else
          {
          //pRS->Edit(); //it is possible that the field allready exists!!!
          }
        pRS->Fields->GetItem( GetPropertiesClassFieldName())->Value=(LPCTSTR)fd->strClassName; 
        pRS->Fields->GetItem( GetPropertiesPropertyFieldName())->Value=(LPCTSTR)fd->strFieldName; 
        pRS->Fields->GetItem( GetPropertiesTypeFieldName())->Value=(LPCTSTR)CCrack::strFieldType(fd->nType); 
        pRS->Fields->GetItem( GetPropertiesSizeFieldName())->Value=COleVariant(fd->nTypeSize, VT_I2); 
        // NOTE: Entirely up to client to ensure this property's EngUnits are in conversion table
        pRS->Fields->GetItem( GetPropertiesEngUnitFieldName())->Value=(LPCTSTR)fd->strEngUnits; 
        pRS->Fields->GetItem( GetPropertiesValueFieldName())->Value=(LPCTSTR)fd->strDefaultValue; 
        pRS->Fields->GetItem( GetPropertiesDispUnitsFieldName())->Value=(LPCTSTR)fd->strDispUnits; 
        pRS->Fields->GetItem( GetPropertiesDescFieldName())->Value=(LPCTSTR)fd->strDescription; 
        pRS->Fields->GetItem( GetPropertiesHelpIDFieldName())->Value=COleVariant(fd->lHelpID, VT_I4);
        pRS->Update();

        //pRS->Close();

        // Add field definition to internal map
        //m_mapFieldDefs[strKey] = fd;
        ASSERT(!fd->bInMap);
        fd->bInMap=TRUE;
        m_mapFieldDefs.SetAt(strKey, fd);
#if dbgCreateFields
        dbgp(" WriteDef");
#endif
        //dbgp(" 3"); for (int iii=0; iii<nDefs; iii++) dbgp("%s ", fds[iii]->strFieldName); dbgpln("");
        }
      catch(_com_error & e)
        {
        KWDisplayException(e, _T(strKey));
        return iDef;
        }
      }
#if dbgCreateFields
    dbgpln("");
#endif
    }

Done:
  if (rsOpenDone)
    {
    pRS->Close();
#if dbgCreateFields
    dbgpln(" RSCls");
#endif
    }

  if (tdOpenDone)
    {
    pTbl.Release();
#if dbgCreateFields
    dbgpln(" TDCls");
#endif
    }

  //KGA TODO !!!
  //if (ExistingField && !ExistingFieldDef)
  //{
  // Check that default properties of given and defined fields match
  // KGA TODO !!!
  //if ( pkwfd->nType != fd->nType ||
  //     (pkwfd->nType == ADODB::adVarWChar && pkwfd->nTypeSize != fd->nTypeSize) ||
  //     pkwfd->strEngUnits != fd->strEngUnits )
  //    return FALSE;

  // We are about to return TRUE to calling function, the newly created
  // field definition will NOT be added to the map, so we must delete it.
  //delete fd;

  //return TRUE;
  //}

  return iDef;
  }

/////////////////////////////////////////////////////////////////////////////////////
// Private implementation


BOOL KWDatabase::CreateField(KWFieldDef * fd)
  {
  HRESULT hr=S_OK;
  ADOX::_TablePtr pTbl;
  KWFieldDef *  pkwfd;
  CString       strKey;

  ASSERT(fd != NULL);

  // Check that class (table) exists
  if (0 && m_bWithSCDOptions)
    if (! IsExistentClass((LPCTSTR)fd->strClassName))
      return FALSE;
  strKey.Format(_T("%s.%s"), fd->strClassName, fd->strFieldName);
  BOOL ExistingClass = IsExistentClass((LPCTSTR)fd->strClassName);
  BOOL ExistingField = IsExistentField((LPCTSTR)fd->strClassName, (LPCTSTR)fd->strFieldName);
  BOOL ExistingFieldDef = m_mapFieldDefs.Lookup(strKey, pkwfd);

  // Check that this is not a duplicate property (field) name
  // This logic is here so that this code may be used in a similar fashion
  // to the database usage in SysCad. i.e. calls to CreateField succeed whether
  // or not the field already exists, PROVIDED that the properties match.
  if (ExistingField && ExistingFieldDef)
    {
    // Check that default properties of given and defined fields match
    if ( pkwfd->nType != fd->nType ||
      (pkwfd->nType == ADODB::adVarWChar && pkwfd->nTypeSize != fd->nTypeSize) ||
      pkwfd->strEngUnits != fd->strEngUnits )
      return FALSE;

    // We are about to return TRUE to calling function, the newly created
    // field definition will NOT be added to the map, so we must delete it.
    delete fd;

    return TRUE;
    }

  if (!ExistingField)
    {
    // Create a field of type specified by given parameters
    try
      {
      if (ExistingClass)
        {
        //td.Open(fd->strClassName);
        pTbl=m_pCat->Tables->GetItem((LPCTSTR)fd->strClassName); 
        MyAddField(pTbl, m_pCat, (LPCTSTR)fd->strFieldName, fd->nType, fd->nTypeSize, fd->bRequired?ADOX::ColumnAttributesEnum(0):ADOX::adColNullable);
        }
      else
        {
        pTbl=ADOX::_TablePtr(__uuidof(ADOX::Table));
        pTbl->Name=(LPCTSTR)fd->strClassName;
        MyAddField(pTbl, m_pCat, (LPCTSTR)fd->strFieldName, fd->nType, fd->nTypeSize, fd->bRequired?ADOX::ColumnAttributesEnum(0):ADOX::adColNullable);
        m_pCat->Tables->Append(_variant_t((IDispatch*)pTbl));
        }

      pTbl.Release();
      }
    catch(_com_error & e)
      {
      KWDisplayException(e, _T(""));
      return FALSE;
      }
    }

  if (!ExistingFieldDef)
    {
    try
      {
      // For each record in specified table, set new field to its default value
      // or leave all empty if no value specified.
      SetFieldDefaultValueInAllRecords(fd);

      // Add field definition to 'Properties' Table
      ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
      CString strFind;
      strFind.Format(_T("Select * From [%s] Where [%s] = '%s' And [%s] = '%s'"),
        GetPropertiesTableName(),
        GetPropertiesClassFieldName(),
        fd->strClassName,
        GetPropertiesPropertyFieldName(),
        fd->strFieldName);

      pRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
      if (pRS->adEOF)
        pRS->AddNew();
      else
        {
        //pRS->Edit(); //it is possible that the field allready exists!!!
        }
      pRS->Fields->GetItem( GetPropertiesClassFieldName())->Value     = (LPCTSTR)fd->strClassName; 
      pRS->Fields->GetItem( GetPropertiesPropertyFieldName())->Value  = (LPCTSTR)fd->strFieldName; 
      pRS->Fields->GetItem( GetPropertiesTypeFieldName())->Value      = (LPCTSTR)CCrack::strFieldType(fd->nType); 
      pRS->Fields->GetItem( GetPropertiesSizeFieldName())->Value      = COleVariant(fd->nTypeSize, VT_I2); 
      // NOTE: Entirely up to client to ensure this property's EngUnits are in conversion table
      pRS->Fields->GetItem( GetPropertiesEngUnitFieldName())->Value   = (LPCTSTR)fd->strEngUnits; 
      pRS->Fields->GetItem( GetPropertiesValueFieldName())->Value     = (LPCTSTR)fd->strDefaultValue; 
      pRS->Fields->GetItem( GetPropertiesDispUnitsFieldName())->Value = (LPCTSTR)fd->strDispUnits; 
      pRS->Fields->GetItem( GetPropertiesDescFieldName())->Value      = (LPCTSTR)fd->strDescription; 
      pRS->Fields->GetItem( GetPropertiesHelpIDFieldName())->Value    = COleVariant(fd->lHelpID, VT_I4);
      pRS->Update();
      pRS->Close();

      // Add field definition to internal map
      //m_mapFieldDefs[strKey] = fd;
      fd->bInMap=TRUE;
      m_mapFieldDefs.SetAt(strKey, fd);
      }
    catch(_com_error & e)
      {
      KWDisplayException(e, _T(""));
      return FALSE;
      }
    }

  //KGA TODO !!!
  //if (ExistingField && !ExistingFieldDef)
  //{
  // Check that default properties of given and defined fields match
  // KGA TODO !!!
  //if ( pkwfd->nType != fd->nType ||
  //     (pkwfd->nType == ADODB::adVarWChar && pkwfd->nTypeSize != fd->nTypeSize) ||
  //     pkwfd->strEngUnits != fd->strEngUnits )
  //    return FALSE;

  // We are about to return TRUE to calling function, the newly created
  // field definition will NOT be added to the map, so we must delete it.
  //delete fd;

  //return TRUE;
  //}

  return TRUE;
  }

void KWDatabase::CreateClassesTable()
  {
  try
    {
    ADOX::_TablePtr pTbl=ADOX::_TablePtr(__uuidof(ADOX::Table));
    ADOX::_IndexPtr pIdx=ADOX::_IndexPtr(__uuidof(ADOX::Index));;

    pTbl->Name=GetClassesTableName();
    MyAddField(pTbl, m_pCat, GetClassesClassName(),        ADOX::adVarWChar, 50);
    MyAddField(pTbl, m_pCat, GetClassesDerivedFieldName(), ADOX::adVarWChar, 50,   ADOX::adColNullable);
    MyAddField(pTbl, m_pCat, GetClassesBaseFieldName(),    ADOX::adVarWChar, 50,   ADOX::adColNullable);
    MyAddField(pTbl, m_pCat, GetClassesLocFieldName(),     ADOX::adVarWChar, 255,  ADOX::adColNullable);

    m_pCat->Tables->Append(_variant_t((IDispatch*)pTbl));

    pIdx->Name="PrimaryKey";
    pIdx->Columns->Append(GetClassesClassName(),            ADOX::adVarWChar, 0);
    pIdx->PutPrimaryKey(-1);
    pIdx->PutUnique(-1);

    pTbl->Indexes->Append(_variant_t((IDispatch*)pIdx));
    }
  catch(_com_error &e)
    {
    KWDisplayException(e, "CreateClassTable");
    }
  }


void KWDatabase::CreatePropertiesTable()
  {
  try
    {
    ADOX::_TablePtr pTbl=ADOX::_TablePtr(__uuidof(ADOX::Table));
    ADOX::_IndexPtr pIdx=ADOX::_IndexPtr(__uuidof(ADOX::Index));;

    pTbl->Name=GetPropertiesTableName();
    MyAddField(pTbl, m_pCat, GetPropertiesClassFieldName(),        ADOX::adVarWChar,   50);
    MyAddField(pTbl, m_pCat, GetPropertiesPropertyFieldName(),     ADOX::adVarWChar,   50);
    MyAddField(pTbl, m_pCat, GetPropertiesTypeFieldName(),         ADOX::adVarWChar,   20);
    MyAddField(pTbl, m_pCat, GetPropertiesSizeFieldName(),         ADOX::adSmallInt,    2, ADOX::adColFixed);//|ADODB::adFldUpdatable);
    MyAddField(pTbl, m_pCat, GetPropertiesEngUnitFieldName(),      ADOX::adVarWChar,   35, ADOX::adColNullable); ////|ADODB::adFldUpdatable
    MyAddField(pTbl, m_pCat, GetPropertiesValueFieldName(),        ADOX::adVarWChar,  196, ADOX::adColNullable);
    MyAddField(pTbl, m_pCat, GetPropertiesDispUnitsFieldName(),    ADOX::adVarWChar,   10, ADOX::adColNullable);
    MyAddField(pTbl, m_pCat, GetPropertiesDescFieldName(),         ADOX::adVarWChar,   50, ADOX::adColNullable);
    MyAddField(pTbl, m_pCat, GetPropertiesHelpIDFieldName(),       ADOX::adInteger,     4);

    m_pCat->Tables->Append(_variant_t((IDispatch*)pTbl));

    pIdx->Name="PrimaryKey";
    pIdx->Columns->Append(GetPropertiesClassFieldName(),       ADOX::adVarWChar, 0);
    pIdx->Columns->Append(GetPropertiesPropertyFieldName(),    ADOX::adVarWChar, 0);
    pIdx->PutPrimaryKey(-1);
    pIdx->PutUnique(-1);

    pTbl->Indexes->Append(_variant_t((IDispatch*)pIdx));
    }
  catch(_com_error &e)
    {
    KWDisplayException(e, "CreatePropertiesTable");
    }
  }


void KWDatabase::CreateObjectsTable()
  {
  try
    {
    ADOX::_TablePtr pTbl=ADOX::_TablePtr(__uuidof(ADOX::Table));
    ADOX::_IndexPtr pIdx=ADOX::_IndexPtr(__uuidof(ADOX::Index));;

    pTbl->Name=GetObjectsTableName();
    MyAddField(pTbl, m_pCat, GetObjectsOwnerFieldName(),        ADOX::adVarWChar,   205);
    MyAddField(pTbl, m_pCat, GetObjectsTagFieldName(),          ADOX::adVarWChar,   200);
    if (m_bWithRevisionNo)
      MyAddField(pTbl, m_pCat, GetRevisionNoFieldName(),        ADOX::adInteger,      4);
    if (m_bObjsWithSeqNo)
      MyAddField(pTbl, m_pCat, GetSequenceNoFieldName(),        ADOX::adInteger,      4, ADOX::adColFixed);//|ADODB::adFldUpdatable);
    MyAddField(pTbl, m_pCat, GetObjectsClassFieldName(),        ADOX::adVarWChar,    50, ADOX::adColNullable); ////|ADODB::adFldUpdatable
    MyAddField(pTbl, m_pCat, GetObjectsClassVerFieldName(),     ADOX::adVarWChar,    20, ADOX::adColNullable);
    MyAddField(pTbl, m_pCat, GetObjectsPrimaryCfgName(),        ADOX::adVarWChar,   255, ADOX::adColNullable);

    m_pCat->Tables->Append(_variant_t((IDispatch*)pTbl));

    pIdx->PutName("PrimaryKey");
    if (m_bWithRevisionNo)
      pIdx->Columns->Append(GetRevisionNoFieldName(),  ADOX::adInteger,  0);
    pIdx->Columns->Append(GetObjectsOwnerFieldName(),  ADOX::adVarWChar, 0);
    pIdx->Columns->Append(GetObjectsTagFieldName(),    ADOX::adVarWChar, 0);
    pIdx->PutPrimaryKey(-1);
    pIdx->PutUnique(-1);

    pTbl->Indexes->Append(_variant_t((IDispatch*)pIdx));
    }
  catch(_com_error &e)
    {
    KWDisplayException(e, "CreateObjectsTable");
    }
  }

void KWDatabase::SetFieldDefaultValueInAllRecords(KWFieldDef * fd)
  {
  ADODB::_RecordsetPtr pRS;
  try
    {
    if (!fd->strDefaultValue.IsEmpty())
      {
      COleVariant var;
      ConvertToType((LPCTSTR) fd->strDefaultValue, fd->nType, var);

      pRS->Open((LPCTSTR)fd->strClassName, _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic/*ReadOnly*/, ADODB::adCmdTableDirect);
      if (LclTransactionRqd())
        m_pCnn->BeginTrans();
      while (!pRS->adEOF)
        {
        //pRS->Edit();
        pRS->Fields->GetItem((LPCTSTR)fd->strFieldName)->Value=var;
        pRS->Update();
        pRS->MoveNext();
        }
      if (LclTransactionRqd())
        m_pCnn->CommitTrans();
      pRS->Close();
      }
    }
  catch (_com_error & e)
    {
    if (LclTransactionRqd())
      m_pCnn->RollbackTrans();
    if ((pRS!=NULL) && (pRS->State&ADODB::adStateOpen))
      pRS->Close();
    KWDisplayException(e, _T(""));
    }
  }


void KWDatabase::ConvertToType( LPCTSTR strValue, ADOX::DataTypeEnum nType, COleVariant& var )
  {
  switch(nType)
    {
    case ADODB::adBoolean:
      var.ChangeType(VT_BOOL);
      if (_stricmp(strValue, _T("YES"))==0 || _stricmp(strValue, _T("TRUE"))==0 ||
        strcmp(strValue, _T("1"))==0 )
        var = static_cast<VARIANT_BOOL>(TRUE);
      else
        var = static_cast<VARIANT_BOOL>(FALSE);
      break;
    case ADODB::adUnsignedTinyInt:
      var = (BYTE) atoi(strValue);
      break;
    case ADODB::adSmallInt:
      var = (short)atoi(strValue);
      break;
    case ADODB::adInteger:
      var = (long)atol(strValue);
      break;
    case ADODB::adSingle:
      var = (float)atof(strValue);
      break;
    case ADODB::adDouble:
      var = (double)atof(strValue);
      break;
    case ADODB::adVarWChar:
      var = COleVariant(strValue, VT_BSTRT);
      break;
    case ADODB::adCurrency:
    case ADODB::adDate:
    case ADODB::adLongVarBinary:
    case ADODB::adLongVarWChar:
    case ADODB::adGUID:
    default:
      var = strValue;
      break;
    }
  }


void KWDatabase::RemoveAllFieldDefs(void)
  {
  KWFieldDef * pFieldDef;
  CString     str;

  // Remove the dynamically created KWFieldDef's
  POSITION pos = m_mapFieldDefs.GetStartPosition();
  while (pos != NULL)
    {
    m_mapFieldDefs.GetNextAssoc(pos, str, pFieldDef);
    delete pFieldDef;
    }
  m_mapFieldDefs.RemoveAll();
  }


/////////////////////////////////////////////////////////////////////////////////////
// Static function implementations

short KWDatabase::SizeofDBType(ADOX::DataTypeEnum  iDBType)
  {
  switch(iDBType)
    {
    case ADODB::adUnsignedTinyInt:
    case ADODB::adBoolean:
      return 1;
    case ADODB::adSmallInt:
      return 2;
    case ADODB::adSingle:
    case ADODB::adInteger:
      return 4;
    case ADODB::adDouble:
    case ADODB::adDate:
    case ADODB::adCurrency:
      return 8;
    case ADODB::adGUID:
      return 16;

      // Can't know size of these
    case ADODB::adVarWChar:
    case ADODB::adLongVarBinary:
    case ADODB::adLongVarWChar:
    default:
      return 0;
    }
  }


short KWDatabase::IsValidDBType(ADOX::DataTypeEnum  iDBType)
  {
  switch(iDBType)
    {
    case ADODB::adUnsignedTinyInt:
    case ADODB::adBoolean:
    case ADODB::adSmallInt:
    case ADODB::adSingle:
    case ADODB::adVarWChar:
    case ADODB::adInteger:
    case ADODB::adDouble:
    case ADODB::adDate:
    case ADODB::adCurrency:
    case ADODB::adGUID:
    case ADODB::adLongVarBinary:
    case ADODB::adLongVarWChar:
      return TRUE;
    default:
      return FALSE;
    }
  }

ADODB::_RecordsetPtr KWDatabase::GetTblRecordset(LPCTSTR strTable)
  {
  HRESULT hr=S_OK;
  ADODB::_RecordsetPtr pRS;
#if USEMAPPEDRECORDSETS
  if (!m_mapTblRecsets.Lookup(strTable, pRS))
#endif
    {
    try
      {
      pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
      pRS->Open(strTable, _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic/*ReadOnly*/, ADODB::adCmdTableDirect);
      m_mapTblRecsets.SetAt(strTable, pRS);

#if dbgRecSets
      dbgpln("OPEN  RECSET %s", strTable);
#endif
      }
    catch (_com_error & e)
      {
      pRS.Release();
      KWDisplayException(e, _T("GetTblRecordset"));
      }
    }
#if USEMAPPEDRECORDSETS
  else
    {
#if dbgRecSets
    dbgpln("FOUND RECSET %s", strTable);
#endif
    }
#endif
  return pRS;
  };
void KWDatabase::CloseTblRecordset(LPCTSTR strTable)
  {
  ADODB::_RecordsetPtr pRecSet;
  if (m_mapTblRecsets.Lookup(strTable, pRecSet))
    {
    try
      {
      m_mapTblRecsets.RemoveKey(strTable);
      pRecSet->Close();
      pRecSet.Release();
#if dbgRecSets
      dbgpln("CLOSE RECSET %s", strTable);
#endif
      }
    catch (_com_error & e)
      {
      pRecSet.Release();
      KWDisplayException(e, _T(""));
      }
    }
  else
    {
#if dbgRecSets
    dbgpln("NTFND RECSET %s", strTable);
#endif
    }
  };
void KWDatabase::CloseAllTblRecordsets()
  {
  ADODB::_RecordsetPtr pRecSet;
  CString     str;

  // Remove the dynamically created ADODB::_Recordset's
  POSITION pos = m_mapTblRecsets.GetStartPosition();
  while (pos != NULL)
    {
    m_mapTblRecsets.GetNextAssoc(pos, str, pRecSet);
#if dbgRecSets
    dbgpln("CLOSE RECSET %08x %s", pRecSet, str);
#endif
    pRecSet.Release();
    }
  m_mapTblRecsets.RemoveAll();
  };

//---------------------------------------------------------------------------

CString KWDatabase::MakeFullTag(LPCTSTR lpszOwner, LPCTSTR lpszTag)
  {
  CString FullTag;
  if (m_bFullTagWithOwner)
    FullTag.Format(_T("%s.%s"), lpszOwner, lpszTag);
  else
    FullTag=lpszTag;
  return FullTag;
  }

//---------------------------------------------------------------------------

LPCTSTR KWDatabase::OwnerName(LPCTSTR lpszOwner)
  {
  if ( lpszOwner && lpszOwner[0] != '\0' )
    return lpszOwner;
  else
    return m_strDefaultOwner;
  }

//---------------------------------------------------------------------------

//void KWDatabase::KWDisplayException(_com_error & e, LPCTSTR strMsg)
//  {
//  CString message;
//  if (1)//e)
//    {
//    message=strMsg;
//    CString strErrCode;
//    if (1)//e.m_pErrorInfo)
//      {
//      strErrCode.Format("\nCADOException: Error Code = %08x\n", e.Error());
//      message += strErrCode;
//      message += (LPCTSTR)e.Source();
//      message += _T(": ");
//      message += (LPCTSTR)e.Description();
//      }
//    //else
//    //  {
//    //  strErrCode.Format("\nCDaoException: SCODE=%ld  AfxDaoError=%ld\n", e.Error(), e->m_nAfxDaoError);
//    //  message += strErrCode;
//    //  }
//    //AfxMessageBox(message);
//    }
//  else
//    message="CDaoException e==NULL ????";
//  //AfxMessageBox("CDaoException e==NULL ????");
//
//  DoKWDisplayException(message);
//  }

//---------------------------------------------------------------------------

void KWDatabase::KWDisplayException(_com_error & e, LPCTSTR strMsg)
  {

  CString EC;
  switch (e.Error())
    {
    case DB_E_BADBOOKMARK : EC= "BADBOOKMARK"; break;
    case DB_E_BADCOLUMNID : EC= "BADCOLUMNID"; break;
    case DB_E_CANTCONVERTVALUE : EC= "CANTCONVERTVALUE"; break;
    case DB_E_CANTFETCHBACKWARDS : EC= "CANTFETCHBACKWARDS"; break;
    //case DB_E_DATAFIELD_OVERFLOW : EC= "DATAFIELD_OVERFLOW"; break;
    case DB_E_DATAOVERFLOW : EC= "DATAOVERFLOW"; break;
    case DB_E_DELETEDROW : EC= "DELETEDROW"; break;
    case DB_E_DUPLICATECOLUMNID : EC= "DUPLICATECOLUMNID"; break;
    case DB_E_DUPLICATEINDEXID : EC= "DUPLICATEINDEXID"; break;
    case DB_E_DUPLICATETABLEID : EC= "DUPLICATETABLEID"; break;
    case DB_E_ERRORSINCOMMAND : EC= "ERRORSINCOMMAND"; break;
    //case DB_E_FIELDDIFFERENT : EC= "FIELDDIFFERENT"; break;
    //case DB_E_FIELDMAXEXCEED : EC= "FIELDMAXEXCEED"; break;
    //case DB_E_JOINNEEDINDEX : EC= "JOINNEEDINDEX"; break;
    case DB_E_NOCOMMAND : EC= "NOCOMMAND"; break;
    case DB_E_NOINDEX : EC= "NOINDEX"; break;
    case DB_E_NOTABLE : EC= "NOTABLE"; break;
    case E_OUTOFMEMORY : EC= "OFMEMORY"; break;

    }

  CString message;
  message=strMsg;
  CString strErrCode;
  strErrCode.FormatMessage("\nException: Error Code = %s %08x\n", EC, e.Error());
  message += strErrCode;
  message += (LPCTSTR)e.Source();
  message += _T(": ");
  message += (LPCTSTR)e.Description();

  //for (long ie=0; ie<m_pCnn->Errors->Count; ie++)
  //  {
  //  message += "\n";
  //  ADODB::ErrorPtr pE=m_pCnn->Errors->GetItem(ie);
  //  message += (LPCTSTR)pE->Description;
  //  }

  DoKWDisplayException(message);
  }

//---------------------------------------------------------------------------

void KWDatabase::DoKWDisplayException(LPCTSTR strMsg)
  {
  AfxMessageBox(strMsg);
  }

//------------------------------------------------------------------------

_variant_t KWDatabase::MyKeyArray(_variant_t V1)
  {
  HRESULT hr;
  SAFEARRAY* pMyVector = SafeArrayCreateVector(VT_VARIANT, 0, 1); 
  VARIANT * pVectorData;
  hr = SafeArrayAccessData(pMyVector, (void**)&pVectorData);
  if (FAILED(hr))
    DoBreak();
  pVectorData[0]=V1;
  hr = ::SafeArrayUnaccessData(pMyVector);
  if (FAILED(hr))
    DoBreak();

  _variant_t v;
  V_VT(&v) = VT_ARRAY|VT_VARIANT;
  V_ARRAY(&v) = pMyVector;

  return v;
  }

//------------------------------------------------------------------------

_variant_t KWDatabase::MyKeyArray(_variant_t V1, _variant_t V2)
  {
  HRESULT hr;
  SAFEARRAY* pMyVector = SafeArrayCreateVector(VT_VARIANT, 0, 2); 
  VARIANT * pVectorData;
  hr = SafeArrayAccessData(pMyVector, (void**)&pVectorData);
  if (FAILED(hr))
    DoBreak();
  pVectorData[0]=V1;
  pVectorData[1]=V2;
  hr = ::SafeArrayUnaccessData(pMyVector);
  if (FAILED(hr))
    DoBreak();

  _variant_t v;
  V_VT(&v) = VT_ARRAY|VT_VARIANT;
  V_ARRAY(&v) = pMyVector;

  return v;
  }

//---------------------------------------------------------------------------

CString KWDatabase::TagFilterString(long RevNo, LPCTSTR Tag, LPCTSTR Op)
  {
  CString Tg(Tag);
  if (Tg.Find('\'')>=0)
    {
    for (int i=0; i<Tg.GetLength(); i++)
      {
      if (Tg[i]=='\'')
        Tg.Insert(i++, '\'');
      }
    }
  Tag= Tg;

  CString S;
  S.Format("[%s] %s %i AND [%s] %s '%s'",  GetRevisionNoFieldName(), Op, RevNo, GetFullTagFieldName(), Op, Tag);
  //dbgpln("TagFilterString: %s", (LPCTSTR)S);
  return S;
  }
CString KWDatabase::TagFilterString(LPCTSTR Tag, LPCTSTR Op)
  {
  CString S;
  S.Format("[%s] %s '%s'",  GetFullTagFieldName(), Op, Tag);
  //dbgpln("TagFilterString: %s", (LPCTSTR)S);
  return S;
  }

//---------------------------------------------------------------------------


