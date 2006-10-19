//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __TAGDB_CPP
#include "datacnvs.h"
#include "tagobj.h"
#include "errorlog.h"
#include "tagdb.h"
#include "kwdb.h"
#include "crack.h"
#include "share.h"
#include "dbhelper.h"
#include <oledberr.h>

#define dbgSaveObjMDB       0
#define dbgSaveObjMDBFlds   0
#define dbgLoadObjectMDB    0
#define dbgLoadObjMDB       0
#define dbgLoadObjMDBFlds   0
#define dbgLoadObjMDBPf     0
#define dbgOpenMDB          0

#define dbgShowTiming       0

typedef char BigBuffer[65536];

//#define ConnectClassName "Connects"
#define ConnObjType   "Conn"

// ==========================================================================
//
//
//
// ==========================================================================
/*Load an object from the TagReg table.*/
struct SwopClassRec { char *Old, *New, *NewNV; };
struct SwopClassRec SwopClass[] =
  {
    {"OpenTank",        "Tank-1",           "Tank"            },
    {"Tank_GP",         "Tank-1",           "Tank"            },
    {"Valve_GP",        "Valve-1",          "Valve"           },
    {"Pump_GP",         "Pump-1",           "Pump"            },
    {"Drum Filter-0",   "DrumFilt-0",       "DrumFilt"        },
    {"SpCondui",        "SpConduit",        "SpConduit"       },
    {"SpContai",        "SpContainer",      "SpContainer"     },
    {"MultiFB",         "FB_Grp",           "FB_Grp"          },
    {"GenMdl",          "GSM_Cfg",          "GSM_Cfg"         },
    {"GenMdlA",         "GSM_IOs",          "GSM_IOs"         },
    {"PIDCfg",          "PID_Cfg",          "PID_Cfg"         },
    {"ReactBlk",        "SR_Cfg",           "SR_Cfg"          },
    {"VLEBlk",          "VLE_Cfg",          "VLE_Cfg"         },
    {"Belt_Cnv",        "BeltCnv",          "BeltCnv"         },
    {"Belt_Cnv-2",      "BeltCnv-2",        "BeltCnv"         },
    {"Conveyor-1",      "BeltCnv-2",        "BeltCnv"         },

    // Temporary
    {"Isolator",        "E_Isolator",       "E_Isolator"      },
    {"ElecLink",        "E_Cable",          "E_Cable"         },
    {"ElecLd",          "E_Load",           "E_Load"          },
    {"Incomer",         "E_Incomer",        "E_Incomer"       },
    {"Transformer",     "E_Transformer",    "E_Transformer"   },
    {"Distributor",     "E_BusBar",         "E_BusBar"        },
    {"CctBrk",          "E_CctBrk",         "E_CctBrk"        },
    {"Starter",         "E_Socket",         "E_Socket"        },
    //{"",      "E_",   "E_"     },

    {NULL},

  };

//========================================================================
//
//
//
//========================================================================

static struct {byte iType; ADOX::DataTypeEnum dbType; int dbSize; } TypeMap [] =
  {
    { tt_NULL        , ADOX::adError/*-1*/,       0               },
    { tt_Bool        , ADOX::adBoolean,           0    },
    { tt_Bit         , ADOX::adBoolean,           0    },
    { tt_Byte        , ADOX::adUnsignedTinyInt,   sizeof(byte)    },
    { tt_Word        , ADOX::adInteger,           sizeof(long)    },
    { tt_DWord       , ADOX::adInteger,           sizeof(long)    },
    { tt_Char        , ADOX::adUnsignedTinyInt,   sizeof(byte)    },
    { tt_Int         , ADOX::adInteger,           sizeof(long)    },
    //{ tt_Short       , ADOX::adInteger,         sizeof(long)    },
    { tt_Short       , ADOX::adSmallInt,          sizeof(short)    },
    { tt_Long        , ADOX::adInteger,           sizeof(long)    },
    { tt_Flt16       , ADOX::adSingle,            sizeof(float)   },
    { tt_Float       , ADOX::adSingle,            sizeof(float)   },
    { tt_Double      , ADOX::adDouble,            sizeof(double)  },
    { tt_Generic     , ADOX::adError/*-1*/,       0               },
    { tt_pChar       , ADOX::adVarWChar,          255             },
    { tt_ppChar      , ADOX::adVarWChar,          255             },
    { tt_Strng       , ADOX::adVarWChar,          255             },
    { tt_RqdPage     , ADOX::adError/*-1*/,       0               },
    { tt_OptPage     , ADOX::adError/*-1*/,       0               },
    { tt_Column      , ADOX::adError/*-1*/,       0               },
    { tt_Text        , ADOX::adVarWChar,          255             },
    { tt_Struct      , ADOX::adError/*-1*/,       0               },
    { tt_Element     , ADOX::adError/*-1*/,       0               },
    { tt_Object      , ADOX::adError/*-1*/,       0               },
    { tt_Array       , ADOX::adError/*-1*/,       0               },
    { tt_Struct_E    , ADOX::adError/*-1*/,       0               },
    { tt_Element_E   , ADOX::adError/*-1*/,       0               },
    { tt_Object_E    , ADOX::adError/*-1*/,       0               },
    { tt_Array_E     , ADOX::adError/*-1*/,       0               },
    { tt_Blob        , ADOX::adError/*-1*/,       0               },
  };                                              

static const BOOL DoGlblTransactions = FALSE;

class CTagDBMDB : public CTagDBRt, public KWDatabase
  {
  //friend class CTagDBCSVRec;
  //friend class CTagDBCSVTab;
  private:
    Strng          sDBName;         //fullname of database
    Strng          sRoot;           //directory of database
    //Strng          sRootOld;        //directory of database when last loaded
    //Strng          sNameOld;        //Name when last loaded
    //pCTagDBCSVTab   pFirstTable;     //pointer to first table in linked list
    //long           Version;         //version number (not used)
    //pCTagDBCSVTab   pTagReg;         //pointer to the tag register table
    //pCTagDBCSVTab   pDocReg;         //pointer to the document register table
    //
    //Strng          sLoadLabel;      //current load Label
    //Strng          sLoadLocation;   //current load location (ExecutiveObject name)
    //Strng          sLoadObjType;    //current load objects type (Node or Conn)
    //pCTagDBCSVRec   pLoadTagRec;     //current load record
    //
    static Strng   sOldFlwSolveTag; // Used when Upgrading odl Projects
    static Strng   sNewFlwSolveTag;

    flag           m_DoingConnects;
    //
  public:
    //static Strng   sOldFlwLibTag;
    //static Strng   sOldFlwLibLocation;

    char *         XchgTag(char * Tag);
    char *         XchgClass(char * Class);

    //ADODB::_RecordsetPtr m_pLoadRS;
    ADODB::_RecordsetPtr m_pLoadRS;

    Strng_List     MissingClasses;

    long           m_iDBFmtAsOpened;

  public:
    CTagDBMDB();
    ~CTagDBMDB();
    flag            IsOpen();
    int             Open(long DBFmt, DWORD DBOpts, pchar pRoot, pchar pRootOld, pchar pOldName);
    int             Flush(flag bOnlyChanged);
    int             CloseIt();
    void            SetChanged(flag Changed); // temporary

    long            FindRevisionNo(DWORD Options);
    long            BumpRevisionNo();
    long            GetRevisionNo() { return KWDatabase::GetRevisionNo(); };
    long            SetRevisionNo(long RevNo) { return KWDatabase::SetRevisionNo(RevNo); };

    void           StartTransaction();
    void           StartSaveSequence(pchar pObjType);
    int            SaveObject(pchar pObjType, CXM_ObjectData *pObjData);
    void           EndTransaction(BOOL Commit);
    void           SetHoldLocalTransactions(BOOL Hold);

    void           StartLoadSequence(pchar pObjType);
    int            LoadObject(CXM_ObjectData *pObjData, flag CreateOnly=False);

    int            LoadObject(pchar Tag, CXM_ObjectData *pObjData);

    void           RemoveRevisions(int From, int To) { KWDatabase::RemoveRevisions(From, To); };

    BOOL           GetFieldValue(ADODB::_RecordsetPtr pRS,
      LPCTSTR  strFieldName,
      Strng &S) 
      {
      S="";
      try
        {
        COleVariant var;
        var=pRS->Fields->GetItem(strFieldName)->Value;
        if (var.vt!=VT_NULL)
          S = (LPCTSTR)_bstr_t(var);
        }
      catch (_com_error & e)
        {
        e; // ignore
        return FALSE;
        }
      return TRUE;
      }
    BOOL           GetFieldValue(ADODB::_RecordsetPtr pRS,
      LPCTSTR  strFieldName,
      long &L,
      long LDefault=-1) 
      {
      L=LDefault;
      try
        {
        COleVariant var;
        var=pRS->Fields->GetItem(strFieldName)->Value;
        if (var.vt!=VT_NULL)
          L = V_I4(&var);
        }
      catch (_com_error & e)
        {
        e; // ignore
        return FALSE;
        }
      return TRUE;
      }
    BOOL MoveFirst(ADODB::_RecordsetPtr pRS)
      {
      Strng S;
      BOOL OK=TRUE;
      try
        {
        pRS->MoveFirst();
        }
      catch (_com_error & e)
        {
        e; // ignore
        OK=FALSE;
        }
      return OK;
      }
    BOOL MoveNext(ADODB::_RecordsetPtr pRS)
      {
      Strng S;
      BOOL OK=TRUE;
      try
        {
        pRS->MoveNext();
        }
      catch (_com_error & e)
        {
        e; // ignore
        OK=FALSE;
        }
      return OK;
      }

    BOOL ErrorOccured() { return KWDatabase::ErrorOccured(); };
    void ClearErrorOccured() { KWDatabase::ClearErrorOccured(); };

    // KWDatabase overides
    void DoKWDisplayException(LPCTSTR strMsg);

  private:
    //void           Clear();
    //pCTagDBCSVTab   FindClassTable(pchar pClassId);
    int            SaveObj(pchar pObjType, pchar pObjTag, CPkDataList &List, CPkDataIter &Iter, Strng &SDOTag);
    int            SaveArr(pchar pObjType, pchar pObjTag, CPkDataList &List, CPkDataIter &Iter, Strng &SDOTag);
    int            LoadObj(flag IsConn, pchar pFullTag, pchar pTag, pchar pClass, CXM_ObjectData *pObjData, /*CPkDataItem * &pItem,*/ flag LoadArray);
  };

//------------------------------------------------------------------------

CTagDBMDB::CTagDBMDB()
  {
  m_bWithSCDOptions=true;
  m_bFullTagWithOwner=false;
  m_bWithRevisionNo=true;
  m_bObjsWithSeqNo=true;
  m_DoingConnects=false;
  m_strObjects=_T("$Models");
  m_strConnections=_T("$Connects");
  m_strDefaultOwner=_T("");

  m_bHoldLclTransactions=TRUE;

  m_pLoadRS=NULL;

  m_iDBFmtAsOpened=-1;
  };

//------------------------------------------------------------------------

CTagDBMDB::~CTagDBMDB()
  {
  //try
  //  {
  //  //delete m_pLoadRS;
  //  }
  //catch (_com_error & e)
  //  {
  //  KWDisplayException(e, _T(""));
  //  }
  };

//------------------------------------------------------------------------

void CTagDBMDB::DoKWDisplayException(LPCTSTR strMsg)
  {
  m_bErrorOccured = TRUE;
  LogError("TagDB", LF_Exclamation, "%s", strMsg);
  };

//------------------------------------------------------------------------

flag CTagDBMDB::IsOpen()
  {
  return true; // ??? CNM
  };

//------------------------------------------------------------------------

int CTagDBMDB::Open(long DBFmt, DWORD DBOpts, pchar pRoot, pchar pRootOld, pchar pOldName)
  {
  MissingClasses.Clear();

  TaggedObject::SetBadFieldNameChars("'!~{} ", ".[]");
  //  TaggedObject::SetBadFieldNameChars("'!~{}:", " .[]");

  m_OpenOpts=DBOpts;
  m_iDBFmtAsOpened=-1;

  //  CStopWatchList::Clear();

#if dbgOpenMDB
  dbgpln("TagDB:Open %s -------------------------",pRoot);
#endif
  sRoot=pRoot;

  //TODO: If the database does not exist, (eg new project) then special
  //      code would be needed here to create a new database.

  sDBName.Concat(pRoot, MDBName); //build filename 
  WIN32_FIND_DATA fd;
  if (FileExists(sDBName(), fd))
    {
    if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      {
      LogError("TagDB", LF_Exclamation, "Cannot open read-only Database '%s'", sDBName());
      return 4;
      }
    ASSERT_ALWAYS(m_OpenOpts & (DBO_ForWrite|DBO_ForRead), "Must be Readble or Writable")
    if (!OpenDB(DBFmt, sDBName(), (m_OpenOpts & DBO_ForWrite), false))
      return 1;
    }
  //dbVersion30 for Access97;
  //dbVersion30 = 64??? Access2000?
  //dwOptions = 0 use "default" based on driver
  else if (!CreateDB(DBFmt, sDBName(), /*dbLangGeneral,*/ 0/*(DBOpts & DBO_MDB97)!=0 ? dbVersion30 : 0*/))
    {
    return 2;
    }

  if (!VerifyDB())
    return 3;

  m_iDBFmtAsOpened=DBFmt;

  return 0;
  };

//------------------------------------------------------------------------

int CTagDBMDB::Flush(flag bOnlyChanged)
  {
  return 0;
  };

//------------------------------------------------------------------------

int CTagDBMDB::CloseIt()
  {
  CloseAllTblRecordsets();
  CloseDB();
  if (1 && (m_OpenOpts & DBO_CompressDB))//DoCompact)
    {
    ASSERT_ALWAYS(m_OpenOpts & DBO_ForWrite, "Must be Writable for Compact")
    ULONGLONG SizeBefore=0;
    ULONGLONG SizeAfter=0;
    CFileStatus State;
    if (CFile::GetStatus(sDBName(), State))
      SizeBefore = State.m_size;
    Strng sDBNameTmp(sDBName);
    Strng sDBNameExt(sDBName);
    sDBNameTmp.FnDrivePathName();
    sDBNameExt.FnExt();
    sDBNameTmp+=".Tmp";
    sDBNameTmp+=sDBNameExt;
    DeleteFile(sDBNameTmp()); // Incase;
    //m_pCnn->CompactDatabase(sDBName(), sDBNameTmp());
    if (m_iDBFmtAsOpened<0)
      {
      LogWarning("TagDB", 0, "Compact Database DB not previously opened");
      }
    else if (CompactDB(m_iDBFmtAsOpened, sDBName(), sDBNameTmp()))
      {
      if (CopyFile(sDBNameTmp(), sDBName(), false))
        DeleteFile(sDBNameTmp());
      }
    if (CFile::GetStatus(sDBName(), State))
      SizeAfter = State.m_size;
    LogNote("TagDB", 0, "Compact Database from %I64d to %I64d kbytes", SizeBefore/1024, SizeAfter/1024);
    }
  return 0;
  };

//------------------------------------------------------------------------

void CTagDBMDB::SetChanged(flag Changed)
  {
  };

//------------------------------------------------------------------------
#if dbgSaveObjMDBFlds
static int dbgIndent=0;
#endif

/*Save actual data*/
int CTagDBMDB::SaveObj(pchar pObjType, pchar pObjTag, CPkDataList &List, CPkDataIter &Iter, rStrng SDOTag)
  {
  //extract some info from the 'message'...
  CPkDataItem * pObjItem=List.CurrentItem(Iter);
  char TheType=pObjItem->Type();
  ASSERT(TheType==tt_Object || TheType==tt_Element);
  CPkDataItem * pItem = List.NextItem(Iter);
  if (pObjTag==NULL)
    pObjTag="";
  pchar pClass=pObjItem->Class();
  pchar pPrimaryCfg=pObjItem->PrimaryCfg();
  Strng ThisObjTag, ThisObjTagMem;
  SDOTag=pObjItem->SymOrTag();

  int MustCreate=FALSE;
  int nKWFlds=0;
  KWFieldDef* KWFlds[256];
  COleVariant KWData[256];

  // Strip off ModelVersion control Stting
  Strng ClassNV(pClass);
  char *pVersion=strchr(pClass, '-');
  if (pVersion)
    {
    ClassNV.SetLength(pVersion-pClass);
    pVersion++;
    }

  //Strng SubClass(pObjItem->SubClass());

  //determine tag...
  //EscapeFlds(SDOTag);
  if (strlen(pObjTag)>0)
    {
    if (SDOTag.Length()>0 && SDOTag[0]=='[')
      {
      BOOL AllIsDigit = isdigit(SDOTag[1]);
      int i = 2;
      while (AllIsDigit && i<SDOTag.Length() && SDOTag[i]!=']')
        AllIsDigit = isdigit(SDOTag[i++]);
      if (AllIsDigit)
        {
        long iIndex=atol(&SDOTag[1]);
        ThisObjTag.Set("%s[%0*i]",pObjTag,TOElementTagDigitCnt(),iIndex);
        }
      else
        ThisObjTag.Set("%s%s",pObjTag,SDOTag());
      }
    else
      ThisObjTag.Set("%s.%s",pObjTag,SDOTag());
    }
  else
    ThisObjTag=SDOTag();
  ThisObjTagMem=ThisObjTag;
  Strng StructTag, ItemTag;
#if dbgSaveObjMDBFlds
  dbgpln("%*sObject Start :%s [%s] ",dbgIndent,"",pClass, ThisObjTag()); 
  dbgIndent+=3;  
#endif

  flag IsConn=(_stricmp(pObjType, ConnObjType)==0);
  if (IsConn)
    {
    ClassNV=GetConnectTableName();
    if (!IsExistentClass(ClassNV()))
      {
      CreateClass(ClassNV());
      CreateFullTagIndex(ClassNV());
      }
    }
  else if (!IsExistentClass(ClassNV()))
    {
    CreateClass(ClassNV());
    CreateFullTagIndex(ClassNV());
    }

  //repeat until all fields have been placed in a buffer,
  //call SaveObj and SaveArr recursively if required...
  int nFlds=1;
  flag Busy=(pItem!=NULL);
  while (Busy)
    {
    char ThisType=pItem->Type();
    if (ThisType==tt_Object || ThisType==tt_Array)
      {
      CPkDataItem * pObjItem=pItem;
#if dbgSaveObjMDBFlds
      dbgpln("%*sSave:%-15s %-20s = %s",dbgIndent,"",tt_TypeString(pObjItem->Type()), pObjItem->TagStr(), pObjItem->Class());
#endif
      flag DoObject=(pObjItem->Type()==tt_Object);
      flag DoArray=(pObjItem->Type()==tt_Array);

      Strng SDOTag;
      ItemTag=ThisObjTag();
      if(StructTag.Length()>0)
        if (ItemTag.Length()>0)
          ItemTag.Set("%s.%s",ItemTag(),StructTag());
        else
          ItemTag=StructTag();

      Strng sClass;
      if (DoObject)
        {
        sClass.Set("<%s>", pObjItem->Class());
        SaveObj(pObjType, ItemTag(), List, Iter, SDOTag);
        }
      else
        {
        sClass.Set("[%s]", pObjItem->Class());
        int n=pObjItem->NElements();
        SaveArr(pObjType, ItemTag(), List, Iter, SDOTag);
        }
      pItem=List.CurrentItem(Iter);

      if(StructTag.Length()>0)
        ItemTag.Set("%s.%s",StructTag(),SDOTag());
      else
        ItemTag=SDOTag();

      //<CNM>      TDBStrBuff::AppendCSVBuff(ItemTag(), FldBuff, sizeof(FldBuff), lFldBuff, nFlds==0);
      //<CNM>      TDBStrBuff::AppendCSVBuff(sClass(), DataBuff, sizeof(DataBuff), lDataBuff, nFlds==0);


      CTagDBase::EscapeStructs(ItemTag);

      ASSERT_ALWAYS(nKWFlds<sizeof(KWFlds)/sizeof(KWFlds[0]), "TooManyFields")
      KWFieldDef* pFldInfo;
      if (!GetKWFieldInfo(ClassNV(), ItemTag(), pFldInfo))
        {
        pFldInfo=new KWFieldDef(ClassNV(), 
          ItemTag(), 
          "", 
          ADOX::adVarWChar, 255, // Length ?
          0,
          FALSE, // Required
          TRUE// Zero Length Allowed
          );
        MustCreate=TRUE;
        }
      KWFlds[nKWFlds]=pFldInfo;
      KWData[nKWFlds]=T2BSTR(sClass()); 
      //KWData[nKWFlds].SetString(sClass(), VT_BSTRT); 

      nKWFlds++;
      nFlds++;

#if dbgSaveObjMDBFlds
      dbgpln("%*sDone:%-15s %-20s = %s",dbgIndent,"", tt_TypeString(pObjItem->Type()), ItemTag(), sClass());
#endif
      }
    else 
      {
      char ItemType=pItem->Type();
      if (ItemType)
        {
        Strng ITag(pItem->SymOrTag());
        if (!IsLayout(ItemType))
          {
          //if (ItemType!=tt_Element_E)
          CTagDBase::EscapeFlds(ITag);
          pchar p;
          switch (ItemType)
            {
            case tt_Object:
            case tt_Array:
            case tt_Array_E:
              ASSERT(0);
              break;
            case tt_Element_E:
            case tt_Object_E:
              Busy=0;
#if dbgSaveObjMDBFlds
              dbgpln("%*sEnd :%s %-20s",dbgIndent,"",tt_TypeString(ItemType), pClass);
#endif
              break;
            case tt_Struct:
              //CTagDBase::EscapeFlds(ITag);
              if(StructTag.Length()>0)
                StructTag.Set("%s.%s", StructTag(), ITag());
              else
                StructTag=ITag();
#if dbgSaveObjMDBFlds
              dbgpln("%*sStructTag>>:%s",dbgIndent,"", StructTag());
#endif
              break;
            case tt_Struct_E:
              //CTagDBase::EscapeFlds(ITag);
              p=strrchr(StructTag(), '.');
              StructTag.SetLength(p ? p-StructTag() : 0);
#if dbgSaveObjMDBFlds
              dbgpln("%*sStructTag<<:%s",dbgIndent,"", StructTag());
#endif
              break;
            case tt_NULL:
              Busy=0;
              break;
            default:
              if (IsData(ItemType))
                {
                //CTagDBase::EscapeFlds(ITag);
                if(StructTag.Length()>0)
                  ItemTag.Set("%s.%s", StructTag(), ITag());
                else
                  ItemTag=ITag();                
                char StrValue[2048];
                strcpy(StrValue, "");
                flag HasStrList=false;
                if ((IsIntData(ItemType) || IsUnSgnData(ItemType)) && pItem->Contains(PDI_StrList))
                  {
                  if (pItem->IndexedStrList())
                    {
                    //PkValueLst StrLst;
                    Strng_List StrLst;
                    pItem->GetStrList(StrLst);
                    int i=pItem->Value()->GetLong();
                    for (pStrng p= StrLst.First(); p; p=p->pNxt)
                      //if (p->Index()==i)
                      if (p->Index()==i && p->Str())
                        {
                        strncpy(StrValue, p->Str(), sizeof(StrValue));
                        break;
                        }
                      HasStrList=true;
                    }
                  } 
                if (!HasStrList && strlen(StrValue)==0)
                  strncpy(StrValue, pItem->Value()->GetString("%i", "%.15g"), sizeof(StrValue));
                StrValue[sizeof(StrValue)-1]=0;

                CTagDBase::EscapeStructs(ItemTag);

                ASSERT_ALWAYS(nKWFlds<sizeof(KWFlds)/sizeof(KWFlds[0]), "TooManyFields")
                  KWFieldDef* pFldInfo;

                ADOX::DataTypeEnum FldType=HasStrList ? ADOX::adVarWChar : TypeMap[ItemType].dbType;
                int FldSize=HasStrList ? 255      : TypeMap[ItemType].dbSize;

                if (!GetKWFieldInfo(ClassNV(), ItemTag(), pFldInfo))
                  {
                  pFldInfo=new KWFieldDef(ClassNV(), 
                    ItemTag(), 
                    pItem->CnvTxt()?pItem->CnvTxt():"", 
                    FldType, FldSize,
                    0,
                    FALSE,                           // Required
                    FldType==ADODB::adVarWChar // Zero Length Allowed
                    );
                  MustCreate=TRUE;
                  }
                KWFlds[nKWFlds]=pFldInfo;

                if (HasStrList)
                  {

                  //if (isdigit(StrValue[0]))
                  //  DoBreak();
                  //KWData[nKWFlds].SetString(StrValue, VT_BSTRT); 
                  KWData[nKWFlds]=T2BSTR(StrValue); 
                  }
                else
                  {
                  switch (ItemType)
                    {
                    case tt_Bool  :
                    case tt_Bit   :
                    case tt_Byte  :
                      KWData[nKWFlds]=(BYTE)pItem->Value()->Byte; 
                      break;
                    case tt_Char  :
                      KWData[nKWFlds]=(BYTE)pItem->Value()->Char; 
                      break;
                    case tt_Word  :
                      KWData[nKWFlds]=(long)pItem->Value()->Word; 
                      break;
                    case tt_DWord :
                      KWData[nKWFlds]=(long)pItem->Value()->DWord; 
                      break;
                    case tt_Int   :
                      KWData[nKWFlds]=(long)pItem->Value()->Int; 
                      break;
                    case tt_Short :
                      KWData[nKWFlds]=(long)pItem->Value()->Short; 
                      break;
                    case tt_Long  :
                      KWData[nKWFlds]=(long)pItem->Value()->Long; 
                      break;
                    case tt_Flt16 :
                      KWData[nKWFlds]=(float)pItem->Value()->Flt16; 
                      break;
                    case tt_Float :
                      {
                      float f=(float)pItem->Value()->Float;
                      if (Valid(f))
                        KWData[nKWFlds]=Min(f, fNANMagic_Limit); 
                      else if (IsSignalNAN(f)) 
                        KWData[nKWFlds]=fNANMagic_Signal;
                      else if (IsQuietNAN(f))
                        KWData[nKWFlds]=fNANMagic_Quiet;
                      else
                        ASSERT_ALWAYS(FALSE, "Bad ValueType for Save value for tt_Double");
                      //float f=(float)pItem->Value()->Float;
                      //if (Valid(f))
                      //  KWData[nKWFlds]=f; 
                      //else
                      //  //KWData[nKWFlds]=0.0f; 
                      //  KWData[nKWFlds].vt=VT_NULL; 
                      break;
                      }
                    case tt_Double:
                      {
                      double d=pItem->Value()->Double;
                      if (Valid(d))
                        KWData[nKWFlds]=Min(d, dNANMagic_Limit); 
                      else if (IsSignalNAN(d)) 
                        {
                        if      (HasNANFlag(d, NF_Free  )) KWData[nKWFlds]=dNANMagic_Free  ;
                        else if (HasNANFlag(d, NF_Hold  )) KWData[nKWFlds]=dNANMagic_Hold  ;
                        else if (HasNANFlag(d, NF_Pulse )) KWData[nKWFlds]=dNANMagic_Pulse ;
                        else if (HasNANFlag(d, NF_Ignore)) KWData[nKWFlds]=dNANMagic_Ignore;
                        else if (HasNANFlag(d, NF_Block )) KWData[nKWFlds]=dNANMagic_Block ;
                        else if (HasNANFlag(d, NF_Plus  )) KWData[nKWFlds]=dNANMagic_Plus  ;
                        else if (HasNANFlag(d, NF_Minus )) KWData[nKWFlds]=dNANMagic_Minus ;
                        else if (HasNANFlag(d, NF_Star  )) KWData[nKWFlds]=dNANMagic_Star  ;
                        else                               KWData[nKWFlds]=dNANMagic_Signal;
                        }
                      else if (IsQuietNAN(d))
                        KWData[nKWFlds]=dNANMagic_Quiet;
                      else
                        ASSERT_ALWAYS(FALSE, "Bad ValueType for Save value for tt_Double");
                      break;
                      }
                    case tt_pChar :
                    case tt_ppChar:
                    case tt_Strng :
                    case tt_Text  :
//                      KWData[nKWFlds].SetString(pItem->Value()->GetString(), VT_BSTRT); 
                      KWData[nKWFlds]=T2BSTR(pItem->Value()->GetString()); 
                      break;
                    };                                              
                  }

                nKWFlds++;
                nFlds++;

#if dbgSaveObjMDBFlds
                dbgpln("%*sSave:%-15s %-20s = %s",dbgIndent,"", tt_TypeString(ItemType), ItemTag(), StrValue); 
#endif
                }
            }
          }
        pItem=List.NextItem(Iter);
        if (pItem==NULL && Busy)
          {
          LogError("TagDB", 0, "Bad Database message");
          Busy=0;
          }
        }
      else
        Busy=0;
      }
    }

  if (1)//strlen(pObjTag)==0)
    {
    pchar pTagOfParent="";
    if (pObjItem->TagOfParent() && strlen(pObjItem->TagOfParent())>0)
      pTagOfParent=pObjItem->TagOfParent();
    if (MustCreate)
      CreateFields(ClassNV(), KWFlds, nKWFlds);

#if dbgSaveObjMDB
    dbgpln("%*sObject Flds OK :%s [%s]",dbgIndent,"",pClass, ThisObjTag()); 
#endif

#if dbgShowTiming
    CStopWatch SW;
    SW.Start();
#endif

    if (1)
      {
      int IsRootObj=!IsConn && (strlen(pObjTag)==0);
      CreateObjectExA(IsRootObj, ThisObjTag(), ClassNV(), pVersion ? pVersion :"", pPrimaryCfg, pTagOfParent, IsConn ? CO_CheckForExistence : 0, KWFlds, KWData, nKWFlds);
#if dbgShowTiming
      dbgpln("             >>   >> CreateObjectExA %10.1f %s %s", SW.Lap()*1e6, IsRootObj ? "Root":"    ", ThisObjTag());
#endif
      }
    else
      {
      CreateObject(ThisObjTag(), ClassNV(), pVersion ? pVersion :"", pPrimaryCfg, pTagOfParent);

      //dbgp("--"); for (iii=0; iii<nKWFlds; iii++) dbgp("%s ", KWFlds[iii]->strFieldName); dbgpln("");
#if dbgShowTiming
      dbgpln("             >>   >> CreateObject %10.1f", SW.Lap()*1e6);
#endif
      SetFieldValuesA(ThisObjTag(), KWFlds, KWData, nKWFlds);

      //dbgp("--"); for (iii=0; iii<nKWFlds; iii++) dbgp("%s ", KWFlds[iii]->strFieldName); dbgpln("");
#if dbgShowTiming
      dbgpln("             >>   >> SetFields    %10.1f", SW.Lap()*1e6);
#endif
      }
    }

#if dbgSaveObjMDB
  dbgIndent-=3;
  dbgpln("%*sObject Done  :%s [%s]",dbgIndent,"",pClass, ThisObjTag()); 
  //dbgpln("       Flds: %s ",FldBuff); 
  //dbgpln("       Data: %s ",DataBuff); 
#endif

  return 0;
  };

//------------------------------------------------------------------------
/*Save an array.*/
int CTagDBMDB::SaveArr(pchar pObjType, pchar pObjTag, CPkDataList &List, CPkDataIter &Iter, rStrng SDOTag)
  {
  /**********/
  char TheType=List.CurrentItem(Iter)->Type();
  ASSERT(TheType==tt_Array);
  CPkDataItem * pObjItem=List.CurrentItem(Iter);
  CPkDataItem * pItem=List.NextItem(Iter);

  if (pObjTag==NULL)
    pObjTag="";

  Strng ThisObjTag;
  SDOTag=pObjItem->SymOrTag();
  //CTagDBase::EscapeFlds(SDOTag);
  if (strlen(pObjTag)>0)
    {
    if (SDOTag.Length()>0 && SDOTag[0]=='[')
      {
      DoBreak();
      ASSERT(0);
      ThisObjTag.Set("%s%s",pObjTag,SDOTag());
      }
    else
      ThisObjTag.Set("%s.%s",pObjTag,SDOTag());
    }
  else
    ThisObjTag=SDOTag();

  //pchar pKeyName="$FullTag$";

  Strng ItemTag;

#if dbgSaveObjMDBFlds
  dbgpln("%*sArray Start:%s ",dbgIndent,"",ThisObjTag()); 
  dbgIndent+=3;
#endif

  while (IsLayout(pItem->Type()))
    pItem=List.NextItem(Iter);

  int iElem=0;
  while (pItem->Type()==tt_Element)
    {
    while (IsLayout(pItem->Type()))
      pItem=List.NextItem(Iter);

    CPkDataItem * pElemItem=pItem;
#if dbgSaveObjMDBFlds
    dbgpln("%*sSave Ele %s:%s",dbgIndent,"",tt_TypeString(pElemItem->Type()), pElemItem->TagStr());
#endif

    Strng SDOTag;
    ItemTag=ThisObjTag();

    SaveObj(pObjType, ItemTag(), List, Iter, SDOTag);
    pItem=List.CurrentItem(Iter);

#if dbgSaveObjMDBFlds
    char EndType=pItem->Type();
    dbgpln("%*sEnd  Ele",dbgIndent,"");
#endif
    iElem++;
    }

#if dbgSaveObjMDBFlds
  dbgIndent-=3;
  dbgpln("%*sArray End:%s ",dbgIndent,"",ThisObjTag()); 
#endif
  ASSERT(pItem->Type()==tt_Array_E);
  pItem=List.NextItem(Iter);
  /*********/
  return 0;
  };

//------------------------------------------------------------------------

void CTagDBMDB::StartTransaction()
  {
  m_pCnn->BeginTrans();
  m_bGlblTransactionBusy=TRUE;
  };

//------------------------------------------------------------------------

void CTagDBMDB::EndTransaction(BOOL Commit)
  {
  CloseAllTblRecordsets();

  if ( Commit )
    m_pCnn->CommitTrans();
  else
    m_pCnn->RollbackTrans();
  m_bGlblTransactionBusy=FALSE;
  };

//------------------------------------------------------------------------

void CTagDBMDB::SetHoldLocalTransactions(BOOL Hold)
  {
  m_bHoldLclTransactions=Hold;
  };

//------------------------------------------------------------------------

long CTagDBMDB::FindRevisionNo(DWORD Options)
  {
  long l=KWDatabase::FindRevisionNo(GetObjectsTableName(), Options);
  SetRevisionNo(l);
  return l;
  };

//------------------------------------------------------------------------

long CTagDBMDB::BumpRevisionNo()
  {
  long L=GetRevisionNo()+1;
  SetRevisionNo(L);
  return L;
  };

//------------------------------------------------------------------------

void CTagDBMDB::StartSaveSequence(pchar pObjType)
  {
  //  const char * TabName;
  m_DoingConnects=(_stricmp(pObjType, ConnObjType)==0);
  if (!m_DoingConnects)
    {
    //IncrementRevNumber();
    }
  ResetSequenceNo();
  };

//------------------------------------------------------------------------

int CTagDBMDB::SaveObject(pchar pObjType, CXM_ObjectData *pObjData)
  {
  Strng SDOTag;
  CPkDataIter Iter;
  pObjData->FirstItem(Iter); // Rewind
  //m_lSequenceNo++;
  ClearErrorOccured();
  int Ret = SaveObj(pObjType, "", pObjData->List, Iter, SDOTag);
  if (ErrorOccured())
    LogError("TagDB", LF_Exclamation, "Save error for object %s", SDOTag());
  return Ret; 
  };

//------------------------------------------------------------------------

void CTagDBMDB::StartLoadSequence(pchar pObjType)
  {
  const char * TabName;
  CString strFind;
  m_DoingConnects=(_stricmp(pObjType, ConnObjType)==0);
  if (m_DoingConnects)
    {
    TabName=GetConnectTableName();
    strFind.Format(_T("Select * From [%s] Where [%s] = %i"),
      TabName,
      GetRevisionNoFieldName(),
      GetRevisionNo());
    }
  else 
    {
    TabName=GetObjectsTableName();
    strFind.Format(_T("Select * From [%s] Where [%s] = %i Order By [%s]"),
      TabName,
      GetRevisionNoFieldName(),
      GetRevisionNo(),
      GetSequenceNoFieldName());
    }

  try
    {
    if (m_pLoadRS)
      m_pLoadRS->Close();
    else
      m_pLoadRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));

    m_pLoadRS->Open(_variant_t(strFind), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
    if (!m_pLoadRS->adEOF)
      m_pLoadRS->MoveFirst();
    }
  catch (_com_error & e)
    {
    KWDisplayException(e, _T(""));
    //DoBreak();
    }
  };

//------------------------------------------------------------------------

Strng CTagDBMDB::sOldFlwSolveTag; // Used when Upgrading odl Projects
Strng CTagDBMDB::sNewFlwSolveTag;

//------------------------------------------------------------------------

char * CTagDBMDB::XchgTag(char * Tag)
  {
  static Strng XTags[8];
  static int   XTagsCnt=-1;
  if (Tag==NULL || PrjFileVerNo()>=16 || sOldFlwSolveTag.Length()==0)
    return Tag;
  if (strlen(Tag)>(dword)sOldFlwSolveTag.Length() &&
    Tag[sOldFlwSolveTag.Length()]=='.' &&
    _strnicmp(Tag, sOldFlwSolveTag(), sOldFlwSolveTag.Length())==0)
    {
    XTagsCnt=(XTagsCnt+1)%8;
    XTags[XTagsCnt]=PlantModelTag;
    XTags[XTagsCnt]+=&Tag[sOldFlwSolveTag.Length()];
    return XTags[XTagsCnt]();
    }
  else if (_stricmp(Tag, sOldFlwSolveTag())==0)
    return sNewFlwSolveTag();

  return Tag;
  }

//------------------------------------------------------------------------

char * CTagDBMDB::XchgClass(char * pClass)
  {
  if (pClass==NULL)// || PrjFileVerNo()>=19)
    return pClass;

  if (PrjFileVerNo()==NewPrjFileVerNo())
    return pClass;

  for (int i=0; SwopClass[i].Old; i++)
    if (_stricmp(pClass, SwopClass[i].Old)==0)
      {
      LogNote("TagDB", 0, "Class changed %s -> %s", pClass, SwopClass[i].New);
      return SwopClass[i].New;
      }

    return pClass;
  }

//------------------------------------------------------------------------

int CTagDBMDB::LoadObject(CXM_ObjectData *pObjData, flag CreateOnly)
  {
  if (m_pLoadRS->adEOF)
    return -1;

  CPkDataList   & List=pObjData->List;
  List.Clear();
  //CPkDataItem   * pItem=pObjData->FirstItem();

  flag DoIt=true;
  Strng Class;
  Strng ClassNV;
  Strng ClassVer;
  Strng PrimaryCfg;
  Strng Tag;
  if (!m_DoingConnects)
    {
    COleVariant OClassNV;
    COleVariant OClassVer;
    COleVariant OPrimaryCfg;
    COleVariant OTag;
    //COleVariant Class;
    try
      {
      OClassNV=m_pLoadRS->Fields->GetItem(GetObjectsClassFieldName())->Value;
      OClassVer=m_pLoadRS->Fields->GetItem(GetObjectsClassVerFieldName())->Value;
      OTag=m_pLoadRS->Fields->GetItem(GetObjectsTagFieldName())->Value;
      try
        {
        OPrimaryCfg=m_pLoadRS->Fields->GetItem(GetObjectsPrimaryCfgName())->Value;
        }
      catch (_com_error & e)
        {
        e; // ignore
        OPrimaryCfg="";
        }
      }
    catch (_com_error & e)
      {
      KWDisplayException(e, _T(""));
      }

    if (OClassNV.vt==VT_NULL || OClassNV.vt==VT_EMPTY)
      OClassNV="";
    if (OClassVer.vt==VT_NULL || OClassVer.vt==VT_EMPTY)
      OClassVer="";
    if (OPrimaryCfg.vt==VT_NULL || OPrimaryCfg.vt==VT_EMPTY)
      OPrimaryCfg="";

    ClassNV=(LPCTSTR)_bstr_t(OClassNV);
    ClassVer=(LPCTSTR)_bstr_t(OClassVer);
    PrimaryCfg=(LPCTSTR)_bstr_t(OPrimaryCfg);
    Tag=(LPCTSTR)_bstr_t(OTag);
    if (ClassVer.Length()>0)
      Class.Set("%s-%s", ClassNV(), ClassVer());
    else
      Class=ClassNV();
    if (ClassNV.XStrICmp("FlwSolve")==0)
      {
      sOldFlwSolveTag=Tag;
      //CTagDBase::UnEscapeFlds(sOldFlwSolveTag);
      sNewFlwSolveTag=PlantModelTag;
      }
#if !WITHCHANGEOBJ
    else if (ClassNV.XStrICmp("FS_Chgs")==0)
      {
      DoIt=false;
      }
#endif

    //    DoIt= CreateOnly;
    }
  else
    {
    COleVariant OTag;
    try
      {
      OTag=m_pLoadRS->Fields->GetItem(GetFullTagFieldName())->Value;
      }
    catch (_com_error & e)
      {
      KWDisplayException(e, _T(""));
      }
    ClassNV=GetConnectTableName();
    ClassVer="";
    Tag=(LPCTSTR)_bstr_t(OTag);
    Class=ClassNV();
    }

  int Ret=-1;

  if (DoIt)
    {
#if dbgLoadObjectMDB
    static long xxxCall=0;
    dbgpln("                                                             Load  %5i %s  %s", xxxCall++, Class(), Tag());
#endif
    if (CreateOnly)
      {//object node is being created, return required info...
      //CTagDBase::UnEscapeFlds(Tag);

      Strng NewTag(XchgTag(Tag()));
      if (NewTag!=Tag)
        LogNote(NewTag(), 0, "Tag changed from %s", Tag());

      Strng NewClass(XchgClass(Class()));
      if (NewClass!=Class)
        LogNote(NewTag(), 0, "Class changed from %s to %s", Class(), NewClass());

      List.SetStructureS(/*pItem,*/ tt_Object, NewTag(), NewClass(), "SubClass", PrimaryCfg(), 0, "", NULL, NULL, 0);
      List.SetStructureE(/*pItem,*/ StructEndType(tt_Object), NewTag(), 0);
      Ret=0;
      }
    else //load actual record
      Ret=LoadObj(m_DoingConnects, Tag(), Tag(), Class(), pObjData, /*pItem,*/ False);
    }
  //pObjData->SetSize();

  MoveNext(m_pLoadRS);

  return Ret;
  }

//------------------------------------------------------------------------

int CTagDBMDB::LoadObject(pchar Tag, CXM_ObjectData *pObjData)
  {
  DoBreak();
  int Ret=-1;
  return Ret;
  }

//------------------------------------------------------------------------

//#if DotNetBUG
//static void ClearStack()
//  {
//  char Tmp[128];
//  memset(Tmp, 0, sizeof(Tmp));
//  };
//#endif

//------------------------------------------------------------------------
/*Load the actual data for an object based on info from a record in the 
TagReg table. Note that this function can be called recursivly if a field
in a record refers to another table. Tag names are built using a dot
notation for each level.*/
int CTagDBMDB::LoadObj(flag IsConn, pchar pFullTag, pchar pTag, pchar pClass, CXM_ObjectData *pObjData, /*CPkDataItem * &pItem,*/ flag LoadArray)
  {
  /**/
#if dbgLoadObjMDB
  static long LoadObjCall=0;
  dbgpln("LoadObj[%4i]:%s %s %s", LoadObjCall++, pFullTag, pTag, pClass);
#endif
  CPkDataList & List=pObjData->List;

  // Strip off ModelVersion control Stting
  Strng ClassNV(pClass);

  char *pVer=strchr(pClass, '-');
  ClassNV.SetLength(pVer ? (pVer-pClass): strlen(pClass));

  if (!IsExistentClass(ClassNV()))
    {
    if (!MissingClasses.Find(pClass))
      {
      MissingClasses.Append(pClass);
      //LogWarning("TagDB", 0, "%s Class Table missing", pClass); kga 20/12/04 removed this message because user can't do anything about it and has no impact?
      }
    return 3;
    }
  CheckFullTagIndex(ClassNV());

  // Retry for ADO/Jet BUG Workaround
  long iRetryCnt=0;
ReTry:

  ADODB::_RecordsetPtr pRS=GetTblRecordset(ClassNV());

  if (pRS)//pCTab)
    {//required table has been found,
    int nRecs=0; //number of records found with the same tag
    Strng InitRecTag(pFullTag);

     try
      {

      //decide how many recs to load and search for required tag in the table...
      if (LoadArray)
        {//busy loading an array of items...
        InitRecTag+="[";
        //pRS->SetCurrentIndex(GetFullTagFieldName());
        bool UseIndex=ALLOWTAGINDEX && pRS->Supports(ADODB::CursorOptionEnum(ADODB::adSeek|ADODB::adIndex)); 
        if (UseIndex)
          pRS->Index=GetFullTagFieldName();
        if (m_bWithRevisionNo)
          {
          if (UseIndex)
            pRS->Seek(MyKeyArray(m_lRevisionNo, pFullTag), ADODB::adSeekAfterEQ);
          else
            pRS->Filter=(LPCTSTR)TagFilterString(m_lRevisionNo, pFullTag, ">=");
          }
        else
          {
          if (UseIndex)
            pRS->Seek(MyKeyArray(pFullTag), ADODB::adSeekAfterEQ);
          else
            pRS->Filter=(LPCTSTR)TagFilterString(pFullTag, ">=");
          }
        //dbgpln("RecCount: %i", pRS->RecordCount);
        flag Exists = (!pRS->adEOF);
        if (Exists)
          //if (pRS->Seek(">=", &VTag, &VRev))
          {
          //COleVariant BkMk=pRS->GetBookmark();
          while (!pRS->adEOF)
            {
            if (m_bWithRevisionNo)
              {
              long lRev;
              GetFieldValue(pRS, GetRevisionNoFieldName(), lRev,-1);
              if (lRev!=m_lRevisionNo)
                break;
              }
            Strng FTag;
            GetFieldValue(pRS, GetFullTagFieldName(), FTag);
            if (FTag.XStrNICmp(InitRecTag(), InitRecTag.Length())==0) 
              {
              nRecs++;
              }
            else
              break;
            pRS->MoveNext();
            }
          //pRS->SetBookmark(BkMk); // Set Bookmark does not always work

          flag StillExists;
          if (m_bWithRevisionNo)
            {
            if (UseIndex)
              pRS->Seek(MyKeyArray(m_lRevisionNo, pFullTag), ADODB::adSeekAfterEQ);
            else
              pRS->Filter=(LPCTSTR)TagFilterString(m_lRevisionNo, pFullTag, ">=");
            }
          else
            {
            if (UseIndex)
              pRS->Seek(MyKeyArray(pFullTag), ADODB::adSeekAfterEQ);
            else
              pRS->Filter=(LPCTSTR)TagFilterString(pFullTag, ">=");
            }
          //dbgpln("RecCount: %i", pRS->RecordCount);
          StillExists=!pRS->adEOF;
          if (!StillExists)
            {
            LogError("", 0, "Bad reseek in Table '%s'", pClass);
            nRecs=0;
            }
          }
        }
      else
        {
        //pRS->CancelUpdate();
        bool UseIndex=ALLOWTAGINDEX && pRS->Supports(ADODB::CursorOptionEnum(ADODB::adSeek|ADODB::adIndex)); 
        if (UseIndex)
          pRS->Index=GetFullTagFieldName();
        if (m_bWithRevisionNo)
          {
          if(UseIndex)
            pRS->Seek(MyKeyArray(m_lRevisionNo, pFullTag), ADODB::adSeekFirstEQ);
          else
            pRS->Filter=(LPCTSTR)TagFilterString(m_lRevisionNo, pFullTag, "=");
          }
        else
          {
          if (UseIndex)
            pRS->Seek(MyKeyArray(pFullTag), ADODB::adSeekFirstEQ);
          else
            pRS->Filter=(LPCTSTR)TagFilterString(pFullTag, "=");
          }
        //dbgpln("RecCount: %i", pRS->RecordCount);
        flag Exists = (!pRS->adEOF);

        if (Exists)
          nRecs=1; //one record for a single item
        }
      }
    catch (_com_error & e)
      {
      // Retry for ADO/Jet BUG Workaround
      if (e.Error()==DB_E_ROWSNOTRELEASED && iRetryCnt++<2)
        {
        CloseTblRecordset(ClassNV());
        goto ReTry;
        }
      
      KWDisplayException(e, _T(""));
      }

    if (nRecs>0)//GotIt)
      {//records with the matching tag name were found...
      Strng PTag, ElementTag;
      Strng TagOfParent;

      //if (pRec)
      //TagOfParent=pRec->FldValue("Parent");

      // FOR SPEED : these 2 lines are only required when/if we resustitate the multi level heirarchy of models.
      //if (!IsConn)
      //  GetFieldValue(pRS, "Parent", TagOfParent);
      
      PTag=pTag;
      //CTagDBase::UnEscapeFlds(PTag);
      //ObjPTag=PTag;
      //      Strng TagOfParent(pTagOfParent);
      //      Strng NewTagOfParent(pTagOfParent);
      //load actual data into the List structure...
      List.SetStructureS(/*pItem,*/ LoadArray ? tt_Array : tt_Object, XchgTag(PTag()), XchgClass(pClass), "", "", 0, XchgTag(TagOfParent()), NULL, NULL, nRecs);

      Strng RecTag;
      int iRecReqd=0;
      for (int iRec=0; iRec<nRecs; iRec++)
        {
        flag RecordGood=true;
        if (LoadArray)
          {
          if (iRec>0)
            MoveNext(pRS);

          Strng FTag;
          GetFieldValue(pRS, GetFullTagFieldName(), FTag);
          if (FTag.XStrNICmp(InitRecTag(), InitRecTag.Length())!=0)
            {
            LogError("", 0, "Discarding bad record '%s' from %s", FTag(), pClass);
            RecordGood=false;
            }
          else
            {
            Strng Index=FTag.Right(FTag.Length()-InitRecTag.Length()+1);
            BOOL AllIsDigit = true;
            int i = 1;
            while (AllIsDigit && Index[i]!=0 && Index[i]!=']')
              AllIsDigit = isdigit(Index[i++]);
            if (AllIsDigit)
              ElementTag.Set("[%0*i]", TOElementTagDigitCnt(), atol(&Index[1]));
            else
              ElementTag=Index;
            if (pFullTag)
              RecTag.Set("%s%s",pFullTag,ElementTag());
            else
              RecTag=ElementTag;
            }
          }
        else
          RecTag.Set("%s",pFullTag);
        if (RecordGood)
          {
          if (LoadArray)
            {
            //CTagDBase::UnEscapeFlds(ElementTag);
            List.SetStructureS(/*pItem,*/ tt_Element, XchgTag(ElementTag()), pClass, "", "", 0, XchgTag(TagOfParent()), NULL, NULL, nRecs);
            }

          Strng Prefix, RqdPrefix, Tag, FullTag, Class;

          int PrevPrefixLen=0;
          long nFlds=pRS->Fields->Count;
          for (long i=0;i<nFlds; i++)
            {
//#if DotNetBUG
//            ClearStack();
//#endif
            //COleVariant TheValue;
            _variant_t TheValue;
            Strng StrValue;
            char * pStrValue="";
            CString FldName;
            ADODB::DataTypeEnum FldType;
            //CDaoFieldInfo FI;
            try
              {
              FldName=(LPCTSTR)pRS->Fields->GetItem(i)->Name;
              FldType=pRS->Fields->GetItem(i)->Type;
              //  pRS->GetFieldInfo(i, FI);
              }
            catch (_com_error & e)
              {
              Strng E;
              E.Set("%s field %i", ClassNV(), i); 
              KWDisplayException(e, E());
              continue;
              }
            //if ((pRec->FldName(i) && pRec->FldName(i)[0]!='$') || i==pRec->Data.No())
            flag LastFld=(i==pRS->Fields->Count);

            //if (FldName.CompareNoCase("Tear:HoldCount")==0)
            //  {
            //  _asm int 3;
            //  }

            if ((FldName.GetLength()>0 && FldName[0]!='$') || LastFld)
              {
              int IsTagFld=(FldName.CompareNoCase("Tag")==0);
              int IsNullValue=false;
              //flag LastFld=(i==pRec->Data.No());
              // Need to Unwind and rebuild Structure names to match this tag
              RqdPrefix=LastFld ? "" : FldName;
              CTagDBase::UnEscapeStructs(RqdPrefix);
              TheValue=pRS->Fields->GetItem((LPCTSTR)FldName)->Value;
              switch (TheValue.vt)
                {
                case VT_BSTR:
                case VT_BSTRT:
                  StrValue=(LPCTSTR)_bstr_t(TheValue);
                  break;
                case VT_NULL:
                  StrValue="";
                  IsNullValue=true;
                  continue;
                  break;
                case VT_EMPTY:// Empty Fields
                  StrValue="";
                  continue;
                  break;
                default:
                  StrValue="";
                  break;
                }
              pStrValue=StrValue();
              if (!pStrValue)
                pStrValue="";

              int StrValLen=strlen(pStrValue);
              //              #if dbgLoadObjMDB
              //              dbgpln("LdObj:%-15s = %s", RqdPrefix(), (const char *)CCrack::strVARIANT(TheValue));
              //              #endif
              pchar p=RqdPrefix() ? strrchr(RqdPrefix(), '.') : NULL;
              if (p)// && 0) // ? Temporary Fix to Fix Load of data with '.' in field names 
                {
                VERIFY((p-RqdPrefix())>=0);
                Tag=p+1;
                RqdPrefix.SetLength(p-RqdPrefix());
                }
              else
                {
                Tag=RqdPrefix();
                RqdPrefix="";
                }
              int LenP=Prefix.Length();
              int LenR=RqdPrefix.Length();
#if dbgLoadObjMDBPf
              dbgpln("  %60s   :%3i %3i %-15s %-15s","", LenP, LenR, Prefix() ? Prefix() : "", RqdPrefix());
#endif
              if ((LenP!=LenR) || ((LenP>0) && (Prefix.XStrCmp(RqdPrefix())!=0)))
                {
#if dbgLoadObjMDBPf
                dbgpln("     :%-15s != %-15s", Prefix() ? Prefix() : "", RqdPrefix());
#endif
                // find no of chars for which current prefix == reqd prefix
                int i=0,j=Min(RqdPrefix.Length(), Prefix.Length());
                for (int LenEql=0; i<j; i++)
                  if (RqdPrefix[i]!=Prefix[i])
                    break;  
                  else if (RqdPrefix[i]=='.')
                    LenEql=i;
                if (i==j)
                  {
                  char En= (i==RqdPrefix.Length() ? '.' : RqdPrefix[i]);
                  char Eo= (i==Prefix.Length() ? '.' : Prefix[i]);
                  if (En==Eo)
                    LenEql=i;
                  }

                //Close structures of current prefix down to LenEql
                while (Prefix.Length() > LenEql)
                  {
                  pchar p=strrchr(Prefix(), '.');
                  pchar q=p ? p+1 : Prefix();
#if dbgLoadObjMDBPf
                  dbgpln("   } :%-15s - %-15s [%i]", RqdPrefix(), q, LenEql);
#endif
                  List.SetStructureE(/*pItem,*/ tt_Struct_E, XchgTag(q), 0);
                  Prefix.SetLength(p ? p-Prefix(): 0);
                  }

                //Open structures of Reqd prefix from LenEql
                int iii=0;          
                while (LenEql < RqdPrefix.Length())
                  {
                  ASSERT(iii++<100);
                  pchar p=&RqdPrefix[LenEql>0 ? LenEql : 0];
                  if (*p=='.')
                    p++;
                  pchar q=strchr(p+1, '.');
                  if (q) *q=0;

#if dbgLoadObjMDBPf
                  dbgpln("   { :%-15s - %-15s [%i]", RqdPrefix(), p, LenEql);
#endif
                  if (*p=='.')
                    { int xxx=0;};

                  List.SetStructureS(/*pItem,*/ tt_Struct, XchgTag(p), "*", "", "", 0, "", NULL, NULL, 0);
                  if (q) 
                    {
                    *q='.';
                    LenEql=q-RqdPrefix()+1;
                    }
                  else
                    LenEql=RqdPrefix.Length();
                  }
                Prefix=RqdPrefix();
                }

              //if (LoadArray)
              //  Tag.Set("%s[%i]", Tag(), iRec);
              if (!IsTagFld && StrValLen>2 && 
                ((StrValue[0]=='<' && StrValue[StrValLen-1]=='>') || 
                (StrValue[0]=='[' && StrValue[StrValLen-1]==']')))
                {
                flag LoadArray=(StrValue[0]=='[');
                Class=StrValue.Mid(1, StrValLen-2);
                //Class.SetLength(Class.Length()-1);
                //PTag=Tag();
                if (RqdPrefix.Length()>0)
                  FullTag.Set("%s.%s.%s", RecTag(), RqdPrefix(), Tag());
                else
                  FullTag.Set("%s.%s", RecTag(), Tag());

#if dbgLoadObjMDB
                dbgpln("LdObj:%-15s = %s", FullTag(), Class());
#endif

                LoadObj(IsConn, FullTag(), Tag(), Class(), pObjData, /*pItem,*/ LoadArray);
                }
              else
                {
                PTag=Tag();
                CTagDBase::UnEscapeFlds(PTag);
                PkDataUnion DU;
                if (PTag.XStrICmp("Tag")==0)
                  DU.SetTypeString(tt_Generic, XchgTag(pStrValue));
                else
                  {
                  switch (FldType)
                    {
                    case ADODB::adBoolean:
                      if (TheValue.vt==VT_BOOL)
                        DU.SetTypeLong(tt_Bool, V_BOOL(&TheValue)!=0);
                      else if (TheValue.vt==VT_BSTR || TheValue.vt==VT_BSTRT)
                        DU.SetTypeString(tt_Generic, pStrValue);
                      else if (TheValue.vt==VT_NULL)
                        DU.SetTypeString(tt_Generic, "");
                      else
                        ASSERT_ALWAYS(FALSE, "Bad ValueType for ADODB::adBoolean");
                      break;

                    case ADODB::adUnsignedTinyInt://   Byte
                      if (TheValue.vt==VT_UI1 || TheValue.vt==VT_I2)
                        DU.SetTypeLong(tt_Byte, V_UI1(&TheValue));
                      else if (TheValue.vt==VT_BSTR || TheValue.vt==VT_BSTRT)
                        DU.SetTypeString(tt_Generic, pStrValue);
                      else if (TheValue.vt==VT_NULL)
                        DU.SetTypeString(tt_Generic, "");
                      else
                        ASSERT_ALWAYS(FALSE, "Bad ValueType for ADODB::adUnsignedTinyInt");
                      break;

                    case ADODB::adSmallInt://   Short
                      if (TheValue.vt==VT_I2)
                        DU.SetTypeLong(tt_Short, V_I2(&TheValue));
                      else if (TheValue.vt==VT_BSTR || TheValue.vt==VT_BSTRT)
                        DU.SetTypeString(tt_Generic, pStrValue);
                      else if (TheValue.vt==VT_NULL)
                        DU.SetTypeString(tt_Generic, "");
                      else
                        ASSERT_ALWAYS(FALSE, "Bad ValueType for ADODB::adSmallInt");
                      break;

                    case ADODB::adInteger://   Long
                      if (TheValue.vt==VT_I4)
                        DU.SetTypeLong(tt_Long, V_I4(&TheValue));
                      else if (TheValue.vt==VT_BSTR || TheValue.vt==VT_BSTRT)
                        DU.SetTypeString(tt_Generic, pStrValue);
                      else if (TheValue.vt==VT_NULL)
                        DU.SetTypeString(tt_Generic, "");
                      else
                        ASSERT_ALWAYS(FALSE, "Bad ValueType for ADODB::adInteger");
                      break;

                    case ADODB::adSingle://   Single
                      if (TheValue.vt==VT_R4)
                        {
                        float f=V_R4(&TheValue);
                        if      (f<=fNANMagic_Limit)  DU.SetTypeDouble(tt_Float, f);
                        else if (f==fNANMagic_Signal) DU.SetTypeDouble(tt_Float, fNAN);
                        else if (f==fNANMagic_Quiet ) DU.SetTypeDouble(tt_Float, fQuietNAN);
                        else if (!Finite(f))          DU.SetTypeDouble(tt_Float, fNANMagic_Limit);
                        else ASSERT_ALWAYS(FALSE, "Bad ValueType for ADODB::adSingle");
                        }
                      else if (TheValue.vt==VT_NULL)
                        DU.SetTypeDouble(tt_Float, dNAN);
                      else
                        ASSERT_ALWAYS(FALSE, "Bad ValueType for ADODB::adSingle");
                      
                      //if (TheValue.vt==VT_R4)
                      //  DU.SetTypeDouble(tt_Float, V_R4(&TheValue));
                      //else if (TheValue.vt==VT_NULL)
                      //  DU.SetTypeDouble(tt_Float, fNAN);
                      //else
                      //  ASSERT_ALWAYS(FALSE, "Bad ValueType for ADODB::adSingle");
                      break;

                    case ADODB::adDouble://   Double
                      if (TheValue.vt==VT_R8)
                        {
                        double d=V_R8(&TheValue);
                        if      (d<=dNANMagic_Limit)  
                          DU.SetTypeDouble(tt_Double, d);
                        else if (d==dNANMagic_Signal) 
                          DU.SetTypeDouble(tt_Double, dNAN);
                        else if (d==dNANMagic_Quiet ) 
                          DU.SetTypeDouble(tt_Double, dQuietNAN);
                        else if (d==dNANMagic_Free  ) 
                          DU.SetTypeDouble(tt_Double, SetNANFlag(NF_Free  ));
                        else if (d==dNANMagic_Hold  ) 
                          DU.SetTypeDouble(tt_Double, SetNANFlag(NF_Hold  ));
                        else if (d==dNANMagic_Pulse ) 
                          DU.SetTypeDouble(tt_Double, SetNANFlag(NF_Pulse ));
                        else if (d==dNANMagic_Ignore) 
                          DU.SetTypeDouble(tt_Double, SetNANFlag(NF_Ignore));
                        else if (d==dNANMagic_Block ) 
                          DU.SetTypeDouble(tt_Double, SetNANFlag(NF_Block ));
                        else if (d==dNANMagic_Plus  ) 
                          DU.SetTypeDouble(tt_Double, SetNANFlag(NF_Plus  ));
                        else if (d==dNANMagic_Minus ) 
                          DU.SetTypeDouble(tt_Double, SetNANFlag(NF_Minus ));
                        else if (d==dNANMagic_Star ) 
                          DU.SetTypeDouble(tt_Double, SetNANFlag(NF_Star  ));
                        else
                          ASSERT_ALWAYS(FALSE, "Bad ValueType for ADODB::adDouble");
                        }
                      else if (TheValue.vt==VT_NULL)
                        DU.SetTypeDouble(tt_Double, dNAN);
                      else
                        ASSERT_ALWAYS(FALSE, "Bad ValueType for ADODB::adDouble");
                      break;

                    case ADODB::adVarWChar://   Text
                      if (TheValue.vt==VT_BSTR || TheValue.vt==VT_BSTRT)
                        DU.SetTypeString(tt_Generic, pStrValue);
                      else if (TheValue.vt==VT_NULL|| TheValue.vt==VT_BSTRT)
                        DU.SetTypeString(tt_Generic, "");
                      else
                        ASSERT_ALWAYS(FALSE, "Bad ValueType for ADODB::adVarWChar");
                      break;

                    default:
                      ASSERT_ALWAYS(FALSE, "Bad DataType");
                    }
                  }
                List.SetDataValue(/*pItem,*/ PTag(), 0, DU);
#if dbgLoadObjMDBFlds
                dbgpln("  LdVal:%-15s:%-15s [%3i]= %s", ClassNV(), PTag(), TheValue.vt, DU.GetString());
#endif
                }
              }
            }

          //Close structures of current prefix down to 0 len
          while (Prefix.Length() > 0)
            {
            pchar p=strrchr(Prefix(), '.');
            pchar q=p ? p+1 : Prefix();
#if dbgLoadObjMDBPf
            dbgpln("   } :%-15s - %-15s [%i]", RqdPrefix(), q, 0);
#endif
            List.SetStructureE(/*pItem,*/ tt_Struct_E, XchgTag(q), 0);
            Prefix.SetLength(p ? p-Prefix(): 0);
            }

          if (LoadArray)
            {
            List.SetStructureE(/*pItem,*/ tt_Element_E, XchgTag(ElementTag()), 0);
            }
          }
        }

      PTag=pTag;
      //CTagDBase::UnEscapeFlds(PTag);
      List.SetStructureE(/*pItem,*/ LoadArray ? tt_Array_E : tt_Object_E, XchgTag(PTag()), 0);

      return 0; //record successfully loaded and placed into the List structure
      }
    return 1;
    }
  //LogWarning("TagDB", 0, "%s Class Table missing", pClass); kga 20/12/04 removed this message because user can't do anything about it and has no impact?

  return 2;
  }

//========================================================================
//
//
//
//========================================================================

CTagDBase::CTagDBase()
  {
  pDB=NULL;
  }

//------------------------------------------------------------------------

CTagDBase::~CTagDBase()
  {
  ASSERT(pDB==NULL); 
  }

//------------------------------------------------------------------------

flag  CTagDBase::IsOpen()  
  { 
  return pDB->IsOpen(); 
  }

//------------------------------------------------------------------------

int CTagDBase::Open(long DBFmt, DWORD DBOpts, pchar pRoot, pchar pRootOld, pchar pOldName)
  {
#if DoDDBChecks
  DataDefnBlk::ClearAllChecks();
#endif

  //AFX_MODULE_STATE * p=AfxGetModuleState();
  //p->m_dwVersion = 0x0601;

  m_OpenOpts=DBOpts; 
  ASSERT(pDB==NULL);
  if (DBFmt == DBConnect_Find)
    {
    Strng Fn;
    Fn.Set("%s%s", pRoot, MDBName); //build filename 
    if (FileExists(Fn()))
      DBFmt=DBConnect_JET2000; // Maybe get clever
    }
  switch (DBFmt)
    {
    case DBConnect_JET97: 
    case DBConnect_JET2000: 
      if (pRootOld && (m_OpenOpts & DBO_CopyOldDBFiles)) 
        {
        Strng Fn, FnO;
        Fn.Set("%s%s", pRoot, MDBName);
        FnO.Set("%s%s", pRootOld, MDBName);
        if (FileExists(FnO()) && !CopyFile(FnO(), Fn(), true))
          {
          Strng E;
          E.GetSystemErrorStr(GetLastError());
          LogError("SysCAD", LF_Exclamation,
            "Database file %s\nnot copied to %s:\n %s", FnO(), Fn(), E());
          }
        }

      pDB= new CTagDBMDB; 
      break;
    }

  m_nObjectsSaved=0;
  if (pDB && pDB->Open(DBFmt, DBOpts|DBO_ForRead, pRoot, pRootOld, pOldName)==0)
    return DBFmt;

  return -1;
  };
int CTagDBase::Flush(flag bOnlyChanged) { return pDB->Flush(bOnlyChanged); };
int CTagDBase::Close() 
  { 
  if (pDB==NULL)
    return 0;
  if (DoGlblTransactions && m_nObjectsSaved)
    {
    pDB->EndTransaction(true);
    m_nObjectsSaved=0;
    }
  if ((m_OpenOpts & DBO_RemovePrevRevs)) 
    {
    pDB->RemoveRevisions(-1, pDB->GetRevisionNo()-1);
    }
  int i=pDB->CloseIt();
  delete pDB;
  pDB=NULL;

#if DoDDBChecks
  if (0)
    DataDefnBlk::DumpAllChecks();
#endif
  return i;
  };

//------------------------------------------------------------------------

void CTagDBase::SetChanged(flag Changed) 
  { pDB->SetChanged(Changed); };

//------------------------------------------------------------------------

long CTagDBase::FindRevisionNo(DWORD Options)
  { return pDB->FindRevisionNo(Options); };

//------------------------------------------------------------------------

long CTagDBase::BumpRevisionNo()
  { return pDB->BumpRevisionNo(); };

//------------------------------------------------------------------------

void CTagDBase::StartSaveSequence(pchar pObjType)
  { pDB->StartSaveSequence(pObjType); };

//------------------------------------------------------------------------

int CTagDBase::SaveObject(pchar pObjType, CXM_ObjectData *pObjData)
  { 
  //pDB->SetHoldLocalTransactions(TRUE);

  // Transactions create Error 3211 'Table Locked by user or process' (sometimes!)
  if (DoGlblTransactions && m_nObjectsSaved==0)
    pDB->StartTransaction();

  int Ret=pDB->SaveObject(pObjType, pObjData); 

  if (DoGlblTransactions && ++m_nObjectsSaved>=50)
    {
    pDB->EndTransaction(true);
    m_nObjectsSaved=0;
    }
  //pDB->SetHoldLocalTransactions(FALSE);

  return Ret;
  };

//------------------------------------------------------------------------

void CTagDBase::StartLoadSequence(pchar pObjType)
  { pDB->StartLoadSequence(pObjType); };

//------------------------------------------------------------------------

int CTagDBase::LoadObject(CXM_ObjectData *pObjData, flag CreateOnly)
  { return pDB->LoadObject(pObjData, CreateOnly); };

//------------------------------------------------------------------------

int CTagDBase::LoadObject(pchar Tag, CXM_ObjectData *pObjData)
  { return pDB->LoadObject(Tag, pObjData); };

//------------------------------------------------------------------------

void CTagDBase::EscapeFlds(Strng &Tag)
  {
  static int AssertDone=0;
  if (Tag.XStrPBrk("{}")!=NULL && !AssertDone)
    {
    AssertDone=1;
    ASSERT_ALWAYS(FALSE, "Bad Tag Name - Contains '{}'");
    }
  //ASSERT(Tag.StrPBrk("{}")==NULL);
  char* p=Tag();
  if (!p)
    return;
  while ((p=strchr(p, '['))!=NULL)
    *p='{';
  p=Tag();
  while ((p=strchr(p, ']'))!=NULL)
    *p='}';
  p=Tag();
  while ((p=strchr(p, '.'))!=NULL)
    *p='~';
  };

//------------------------------------------------------------------------

void CTagDBase::UnEscapeFlds(Strng &Tag)
  {
  char* p=Tag();
  if (!p)
    return;
  while ((p=strchr(p, '{'))!=NULL)
    *p='[';
  p=Tag();
  while ((p=strchr(p, '}'))!=NULL)
    *p=']';
  p=Tag();
  while ((p=strchr(p, '~'))!=NULL)
    *p='.';
  };

//------------------------------------------------------------------------

void CTagDBase::EscapeStructs(Strng &Tag)
  {
  char* p=Tag();
  if (!p)
    return;
  while ((p=strchr(p, '.'))!=NULL)
    *p=':';
  };

//------------------------------------------------------------------------

void CTagDBase::UnEscapeStructs(Strng &Tag)
  {
  char* p=Tag();
  if (!p)
    return;
  while ((p=strchr(p, ':'))!=NULL)
    *p='.';
  };

//------------------------------------------------------------------------
//========================================================================
//
//
//
//========================================================================
//
