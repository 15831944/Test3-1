//============ Copyright Kenwalt (Pty) Ltd (1996, 1997, ...) ================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"    
#define __DBBASE_CPP
#pragma warning( push , 3 )
#include "sc_defs.h"
#include "gpfuncs.h"
#pragma warning( pop )
//#include "general.h"
#include "dbbase.h"
//#include "optoff.h"

#pragma warning(disable: 4100)
#ifdef _DEBUG
#pragma warning(disable: 4189)
#endif

//=========================================================================

CStopWatch SW_DB;
CStopWatch SW_Txt;

//============================================================================

CTxtFile::CTxtFile( BOOL nToFile)
  {
  m_ToFile = nToFile;
  m_dbgf = NULL;
  m_FileName[0] = 0;
  }

//----------------------------------------------------------------------------

CTxtFile::~CTxtFile()
  {
  Close();
  }

//----------------------------------------------------------------------------

void CTxtFile::Open(LPCTSTR nFileName)
  {
  if (m_dbgf==NULL)
    {
    strcpy(m_FileName, nFileName);
    if (m_ToFile == TRUE)
      {
      m_dbgf = fopen(m_FileName, "wt");
      if (m_dbgf==NULL && strchr(m_FileName, '\\'))
        {//try create folder...
        char folder[256];
        int len = (int)strchr(m_FileName, '\\')-(int)m_FileName;
        strncpy(folder, m_FileName, len);
        folder[len] = 0;
        CreateDirectory(folder, NULL);
        m_dbgf = fopen(m_FileName, "wt");
        }
      }
    m_ToFile = (m_dbgf != NULL);
    }
  }

//----------------------------------------------------------------------------

void CTxtFile::Append()
  {
  if (m_ToFile && (m_dbgf==NULL))
    {
    m_dbgf = fopen(m_FileName, "at");
    m_ToFile = (m_dbgf != NULL);
    }
  }

//----------------------------------------------------------------------------

void CTxtFile::Append(LPCTSTR nFileName)
  {
  if (m_ToFile && (m_dbgf==NULL))
    {
    strcpy(m_FileName, nFileName);
    m_dbgf = fopen(m_FileName, "at");
    m_ToFile = (m_dbgf != NULL);
    }
  }

//----------------------------------------------------------------------------

void CTxtFile::Close()
  {
  if (m_ToFile && (m_dbgf!=NULL))
    {
    fclose(m_dbgf); 
    m_dbgf = NULL;
    }
  }

//----------------------------------------------------------------------------

void CTxtFile::Ln(LPTSTR fmt, ...)
  {
  if (m_ToFile && (m_dbgf!=NULL))
    {
    va_list argptr;
    va_start(argptr,fmt);
    vfprintf(m_dbgf, fmt, argptr);
    va_end(argptr);
    #ifdef _DEBUG
    fflush(m_dbgf);
    #endif
    }
  }

//----------------------------------------------------------------------------

void CTxtFile::Ln(BOOL DoNewLine, LPTSTR fmt, ...)
  {
  if (m_ToFile && (m_dbgf!=NULL))
    {
    va_list argptr;
    va_start(argptr,fmt);
    vfprintf(m_dbgf, fmt, argptr);
    va_end(argptr);
    if (DoNewLine)
      fprintf(m_dbgf, "\n");
    #ifdef _DEBUG
    fflush(m_dbgf);
    #endif
    }
  }

//============================================================================

CFileRec::CFileRec(BOOL ToTxt, BOOL ToDB, KWDatabase* pDB_/*=NULL*/)
  {
  bToTxt = ToTxt;
  bToDB = ToDB;
  m_pDB = pDB_ ? pDB_ : pTheDB;
  r = NULL;
  m_pRS = NULL;
  bIgnoreWriteErr3022 = 0;
  }

//-------------------------------------------------------------------------

CFileRec::~CFileRec()
  {
  Close();
  }

//-------------------------------------------------------------------------

void CFileRec::Open(LPCTSTR pFileName, int nOptions/*=0*/)
  {
  Close();
  if (bToTxt)
    {
    ASSERT(pFileName);
    SW_Txt.Start();
    r = new CTxtFile();
    r->Open(pFileName);
    r->Ln(1, (LPTSTR)(LPCTSTR)sTitleLine);
    SW_Txt.Stop();
    }
  if (bToDB)
    {
    SW_DB.Start();
    /*ADODB::_RecordsetPtr */m_pRS = ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
    m_pRS->Open((LPCTSTR)sTableName, _variant_t((IDispatch*)m_pDB->GetConnectionPtr()), ADODB::adOpenKeyset, ADODB::adLockOptimistic/*ReadOnly*/, ADODB::adCmdTableDirect);
    SW_DB.Stop();
    }
  }

//-------------------------------------------------------------------------

void CFileRec::OpenQuery(LPCTSTR pFileName, LPCTSTR pFind, int nOptions/*=0*/)
  {
  Close();
  if (bToTxt)
    {
    ASSERT(pFileName);                                  
    SW_Txt.Start();
    r = new CTxtFile();
    r->Open(pFileName);
    r->Ln(1, (LPTSTR)(LPCTSTR)sTitleLine);
    SW_Txt.Stop();
    }
  if (bToDB)
    {
    SW_DB.Start();
    /*ADODB::_RecordsetPtr */m_pRS = ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
    m_pRS->Open(_variant_t(pFind), _variant_t((IDispatch*)m_pDB->GetConnectionPtr()), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
    SW_DB.Stop();
    }
  }

//-------------------------------------------------------------------------

void CFileRec::Close()
  {
  if (r)
    {
    SW_Txt.Start();
    r->Close();
    delete r;
    r = NULL;
    SW_Txt.Stop();
    }
  if (m_pRS)
    {
    SW_DB.Start();
    try
      {
      if (m_pRS->State!=ADODB::adStateClosed)
        m_pRS->Close();
      }
    catch(_com_error & e)
      {
      CString S;
      S.Format("Unable to close DB %s\n", TableName());
      m_pDB->KWDisplayException(e, S);
      }
    m_pRS.Release();
    SW_DB.Stop();
    }
  }

//-------------------------------------------------------------------------

void CFileRec::CreateIndex(LPTSTR IndexName, BOOL Unique/*=TRUE*/, BOOL Primary/*=TRUE*/)
  {
  if (m_pDB)
    {
    LPCTSTR p = (LPCTSTR)sTableName;
    if (!m_pDB->IsExistentIndex(p, IndexName))
      {
      // Create Primary Index ...
      try
        {
        ADOX::_IndexPtr pIdx=ADOX::_IndexPtr(__uuidof(ADOX::Index));
        pIdx->Name=IndexName;
        pIdx->Columns->Append(IndexName,    ADOX::adVarWChar, 0);
        pIdx->PutPrimaryKey(Primary?-1:0);
        pIdx->PutUnique(Unique?-1:0);

        ADOX::_TablePtr pTb=m_pDB->GetCatalogPtr()->Tables->GetItem((LPCTSTR)sTableName);
        pTb->Indexes->Append(_variant_t((IDispatch*)pIdx));
        }
      catch(_com_error & e)
        {
        CString S;
        S.Format("Unable to create index %s\n", TableName());
        m_pDB->KWDisplayException(e, S);
        }
      }
    }
  }

//-------------------------------------------------------------------------

bool CFileRec::SetCurrentIndex(LPTSTR IndexName)
  {
  if (m_pDB && (m_pRS!=NULL) && m_pDB->IsExistentIndex((LPCTSTR)sTableName, IndexName))
    {
    m_pRS->Index=IndexName;
    //m_pRS->SetCurrentIndex(IndexName);
    return true;
    }
  #ifdef _DEBUG
  if (m_pDB && (m_pRS!=NULL))
    {
    char Buff[1024];
    sprintf(Buff, "Index '%s' doesn't exist in '%s'.", IndexName, (LPCTSTR)sTableName);
    AfxMessageBox(Buff);
    }
  #endif
  return false;
  }

//-------------------------------------------------------------------------

bool CFileRec::SeekEqualStr(LPTSTR What)
  {
  if (m_pDB && (m_pRS!=NULL))
    {
    INCOMPLETECODE(); // FIX below
    COleVariant Val(What, VT_BSTRT);
    m_pRS->Seek(&Val, ADODB::adSeekFirstEQ);
    return !m_pRS->adEOF;
    }
  return false;
  }

//-------------------------------------------------------------------------

void CFileRec::DeleteAll(LPCTSTR Select)
  {
  LPCTSTR p = Select ? Select : (LPCTSTR)sTableName;
  ASSERT(p && strlen(p)>0);
  m_pDB->DeleteAllClassObjects(p);
  }

//-------------------------------------------------------------------------

int CFileRec::DefineClass(BOOL DeleteAll/*=FALSE*/)
  {
  LPCTSTR p = (LPCTSTR)sTableName;
  ASSERT(p && strlen(p)>0);
  if (!m_pDB->IsExistentClass(p))
    m_pDB->CreateClass(p);
  Defs.RemoveAll();
  if (DeleteAll)
    m_pDB->DeleteAllClassObjects(p);
  return 0;
  }

//-------------------------------------------------------------------------

int CFileRec::CreateStr(  LPCTSTR strFieldName, 
                          LPTSTR pData,
                          short nTypeSize,
                          WORD FieldFlags/*=0*/,
                          void* pEmptyVal/*=NULL*/)
  {
  LPCTSTR p = (LPCTSTR)sTableName;
  KWFieldDef* pDef = NULL;
  if (!m_pDB->IsExistentField(sTableName, strFieldName))
    m_pDB->CreateField(p, strFieldName, "", ADOX::adVarWChar, nTypeSize, (FieldFlags & FF_Required), (FieldFlags & FF_AllowZeroLen));
  m_pDB->GetKWFieldInfo(p, strFieldName, pDef);
  CFieldDef Fld(pDef, (void*)pData, pEmptyVal);
  Defs.Add(Fld);
  return 0;
  }

//-------------------------------------------------------------------------

int CFileRec::CreateVal(  LPCTSTR strFieldName, 
                          void* pData,
                          ADOX::DataTypeEnum nType,
                          WORD FieldFlags/*=0*/,
                          void* pEmptyVal/*=NULL*/)
  {
  LPCTSTR p = (LPCTSTR)sTableName;
  KWFieldDef* pDef = NULL;
  if (!m_pDB->IsExistentField(sTableName, strFieldName))
    m_pDB->CreateField(p, strFieldName, "", nType, 0, (FieldFlags & FF_Required));
  m_pDB->GetKWFieldInfo(p, strFieldName, pDef);
  CFieldDef Fld(pDef, pData, pEmptyVal);
  Defs.Add(Fld);
  return 0;
  }

//-------------------------------------------------------------------------

int CFileRec::CreateLongVal(  LPCTSTR strFieldName, 
                              char ** pData,
                              long* pLen,
                              ADOX::DataTypeEnum nType,
                              WORD FieldFlags/*=0*/,
                              void* pEmptyVal/*=NULL*/)
  {
  LPCTSTR p = (LPCTSTR)sTableName;
  KWFieldDef* pDef = NULL;
  if (!m_pDB->IsExistentField(sTableName, strFieldName))
    m_pDB->CreateField(p, strFieldName, "", nType, 0, (FieldFlags & FF_Required));
  m_pDB->GetKWFieldInfo(p, strFieldName, pDef);
  CFieldDef Fld(pDef, pData, pLen, pEmptyVal);
  Defs.Add(Fld);
  return 0;
  }

//-------------------------------------------------------------------------

int CFileRec::BaseRead()
  {
  int RetCode=0;
  USES_CONVERSION;
  ASSERT((m_pRS!=NULL) && bToDB);
  SW_DB.Start();
  COleVariant Val;
  const int Cnt = Defs.GetSize();
  for (int i=0; i<Cnt; i++)
    {
    CFieldDef& Def = Defs[i];
    switch (Def.pDef->nType)
      {
      case ADODB::adLongVarBinary:
      case ADODB::adLongVarWChar:
        {
        try
          {
          long &Len=*Defs[i].pLen;
          char * &pData=*((char**)Defs[i].pData);

          Len=m_pRS->Fields->GetItem((LPCTSTR)Def.pDef->strFieldName)->ActualSize;
          pData=new char[Len];

          COleVariant varChunk;
          varChunk=m_pRS->Fields->GetItem((LPCTSTR)Def.pDef->strFieldName)->GetChunk(Len);

          UINT ElemSize=SafeArrayGetElemsize(varChunk.parray);
          if (ElemSize!=sizeof(char))
            RetCode=1;
          else
            {
            for (long i=0; i<Len; i++)
              {
              HRESULT hr;
              hr=SafeArrayGetElement(varChunk.parray, &i, &pData[i]);
              if (FAILED(hr))
                {
                RetCode=2;
                break;
                }
              }
            }
          }
        catch(_com_error &e)
          {
          char Buff[1024];
          sprintf(Buff, "Table:'%s'  Field(%d):'%s'  Error for SetFieldValue(...)", (LPCTSTR)sTableName, i, Defs[i].pDef->strFieldName);
          m_pDB->KWDisplayException(e, Buff);
          }

        break;
        }
      default:
        {
        //KWFieldDef* pDef = Def.pDef;
        Val=m_pRS->Fields->GetItem((LPCTSTR)Def.pDef->strFieldName)->Value;
        if (Def.pEmptyVal && Val.vt==VT_NULL)
          {
          switch (Def.pDef->nType)
            {
            //case ADODB::adUnsignedTinyInt: break;
            case ADODB::adBoolean:   *((BOOL*)Def.pData)   = *((BOOL*)Def.pEmptyVal);   break;
            case ADODB::adSmallInt:  *((short*)Def.pData)  = *((short*)Def.pEmptyVal);  break;
            case ADODB::adInteger:   *((long*)Def.pData)   = *((long*)Def.pEmptyVal);   break;
            case ADODB::adSingle:    *((float*)Def.pData)  = *((float*)Def.pEmptyVal);  break;
            case ADODB::adDouble:    *((double*)Def.pData) = *((double*)Def.pEmptyVal); break;
            case ADODB::adVarWChar:      
              strncpy((LPTSTR)Def.pData, (LPTSTR)Def.pEmptyVal, Def.pDef->nTypeSize);
              ((LPTSTR)(Def.pData))[Def.pDef->nTypeSize-1] = 0;
              break;
            default:
              ASSERT(FALSE); //unhandled db Type !!!
            }
          }
        else if (Val.vt==VT_NULL)
          {
          switch (Def.pDef->nType)
            {
            //case ADODB::adUnsignedTinyInt: break;
            case ADODB::adBoolean:   *((BOOL*)Def.pData)   = 0;   break;
            case ADODB::adSmallInt:  *((short*)Def.pData)  = 0;  break;
            case ADODB::adInteger:   *((long*)Def.pData)   = 0;   break;
            case ADODB::adSingle:    *((float*)Def.pData)  = fNAN;  break;
            case ADODB::adDouble:    *((double*)Def.pData) = dNAN; break;
            case ADODB::adVarWChar:      
              memset((LPTSTR)Def.pData, 0, Def.pDef->nTypeSize);
              ((LPTSTR)(Def.pData))[Def.pDef->nTypeSize-1] = 0;
              break;
            default:
              ASSERT(FALSE); //unhandled db Type !!!
            }
          }
        else
          {
          switch (Def.pDef->nType)
            {
            //case ADODB::adUnsignedTinyInt: break;
            case ADODB::adBoolean:   *((BOOL*)Def.pData)   = Val.boolVal; break;
            case ADODB::adSmallInt:  *((short*)Def.pData)  = Val.iVal;    break;
            case ADODB::adInteger:   *((long*)Def.pData)   = Val.lVal;    break;
            case ADODB::adSingle:    *((float*)Def.pData)  = Val.fltVal;  break;
            case ADODB::adDouble:    *((double*)Def.pData) = Val.dblVal;  break;
            case ADODB::adVarWChar:      
              if (Val.vt==VT_NULL)
                ((LPTSTR)(Def.pData))[0] = 0;
              else
                //            strncpy((LPTSTR)Def.pData, (LPTSTR)(Val.bstrVal), Def.pDef->nTypeSize);
                strncpy((LPTSTR)Def.pData, W2CT(Val.bstrVal), Def.pDef->nTypeSize);
              ((LPTSTR)(Def.pData))[Def.pDef->nTypeSize] = 0;
              break;
            default:
              ASSERT(FALSE); //unhandled db Type !!!
            }
          }
        }
      }
    }
  SW_DB.Stop();
  return RetCode;
  }

//-------------------------------------------------------------------------

int CFileRec::BaseWrite(BOOL DoAddNew)
  {
  ASSERT((m_pRS!=NULL) && bToDB);
  COleVariant Val;
  const int Cnt = Defs.GetSize();
  if (Cnt==0)
    return -1;
  SW_DB.Start();
  m_pDB->GetConnectionPtr()->BeginTrans();
  if (DoAddNew)
    m_pRS->AddNew();
  else
    {
    //m_pRS->Edit();
    }
  int RetCode = 0;
  int i=0;
  try
    {
    for (i=0; i<Cnt; i++)
      {
      KWFieldDef* pDef = Defs[i].pDef;
      if (pDef)
        {
        switch (pDef->nType)
          {
          //case ADODB::adUnsignedTinyInt: break;
          case ADODB::adBoolean:   m_pRS->Fields->GetItem((LPCTSTR)pDef->strFieldName)->Value = COleVariant((short)*((BOOL*)Defs[i].pData), VT_BOOL);  break;
          case ADODB::adSmallInt:  m_pRS->Fields->GetItem((LPCTSTR)pDef->strFieldName)->Value = COleVariant(*((short*)Defs[i].pData));                 break;
          case ADODB::adInteger:   m_pRS->Fields->GetItem((LPCTSTR)pDef->strFieldName)->Value = COleVariant(*((long*)Defs[i].pData));                  break;
          case ADODB::adSingle:    m_pRS->Fields->GetItem((LPCTSTR)pDef->strFieldName)->Value = COleVariant(*((float*)Defs[i].pData));                 break;
          case ADODB::adDouble:    m_pRS->Fields->GetItem((LPCTSTR)pDef->strFieldName)->Value = COleVariant(*((double*)Defs[i].pData));                break;
          //case ADODB::adVarWChar:  m_pRS->Fields->GetItem((LPCTSTR)pDef->strFieldName)->Value = COleVariant((LPTSTR)Defs[i].pData, VT_BSTRT);          break;
          case ADODB::adVarWChar:  m_pRS->Fields->GetItem((LPCTSTR)pDef->strFieldName)->Value = (LPTSTR)Defs[i].pData;                                 break;
          case ADODB::adLongVarBinary:
          case ADODB::adLongVarWChar:
            {
            SAFEARRAY FAR *psa;
            SAFEARRAYBOUND rgsabound[1];
            rgsabound[0].lLbound = 0;

            try
              {
              //Create a safe array to store the array of BYTES  
              rgsabound[0].cElements = *Defs[i].pLen;
              psa = SafeArrayCreate(VT_UI1,1,rgsabound);

              int Len=*Defs[i].pLen;
              char * &pData=*((char**)Defs[i].pData);

              //Copy the data only upto the Actual Size of Field.  
              for(long index=0; index<Len; index++)
                {
                HRESULT hr = SafeArrayPutElement(psa,&index,&pData[index]); 
                if (FAILED(hr))
                  {
                  RetCode=-5;
                  break;
                  }
                }

              //Assign the Safe array  to a variant. 
              COleVariant varChunk;
              varChunk.vt = VT_ARRAY|VT_UI1;
              varChunk.parray = psa;
              m_pRS->Fields->GetItem((LPCTSTR)pDef->strFieldName)->AppendChunk(varChunk); 

              // delete the buffer
              delete [] pData;
              pData=NULL;
              }
            catch(_com_error &e)
              {
              char Buff[1024];
              sprintf(Buff, "Table:'%s'  Field(%d):'%s'  Error for SetFieldValue(...)", (LPCTSTR)sTableName, i, Defs[i].pDef->strFieldName);
              m_pDB->KWDisplayException(e, Buff);
              }

            break;
            }
          default:
            ASSERT(FALSE); //unhandled db Type !!!
          }
        }
      }
    }
  catch(_com_error & e)
    {
    char Buff[1024];
    sprintf(Buff, "Table:'%s'  Field(%d):'%s'  Error for SetFieldValue(...)", (LPCTSTR)sTableName, i, Defs[i].pDef->strFieldName);
    m_pDB->KWDisplayException(e, Buff);
    RetCode = -2;
    }
  try
    {
    m_pRS->Update();
    }
  catch(_com_error & e)
    {
    m_pRS->CancelUpdate();
    if (!bIgnoreWriteErr3022 || e.ErrorInfo()==NULL)//??? || e->m_pErrorInfo->m_lErrorCode!=3022)
      {
      char Buff[1024];
      sprintf(Buff, "Table:'%s'  Error for Update%s", (LPCTSTR)sTableName, DoAddNew ? " add new" : "");
      m_pDB->KWDisplayException(e, Buff);
      RetCode = -3;
      }
    else
      RetCode = -4;
    }
  if (RetCode==0)
    m_pDB->GetConnectionPtr()->CommitTrans();
  else
    m_pDB->GetConnectionPtr()->RollbackTrans();
  SW_DB.Stop();
  return RetCode;
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//===========================================================================

