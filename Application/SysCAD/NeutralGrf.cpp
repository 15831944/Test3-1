#include "stdafx.h"
#include ".\neutralgrf.h"
#include ".\dbhelper.h"
#include "msgwnd.h"

//#include "optoff.h"

#define KEEPTEMPFOLDERS 0

static double DefValZero=0.0;
static double DefValOne=1.0;
static double DefValNAN=dNAN;

//===========================================================================

BOOL CNeutralGrfInserts::sm_bDoneInit = 0;
CNeutralGrfInserts::CNeutralGrfInserts(bool WithGroup, KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  m_bWithGroup=WithGroup;
  sTableName = "GraphicsInserts";
  Init();
  }

//-------------------------------------------------------------------------

void CNeutralGrfInserts::Init()
  {
  if (m_pDB)
    {
    DefineClass();

    strcpy(m_sGroup, "");
    strcpy(m_sTag, "");
    strcpy(m_sPage, "");
    strcpy(m_sSymbol, "");
    m_Insert.m_X    = 0.0;
    m_Insert.m_Y    = 0.0;
    m_Insert.m_Z    = 0.0;
    m_Scale.m_X     = 1.0;
    m_Scale.m_Y     = 1.0;
    m_Scale.m_Z     = 1.0;
    m_Rotation      = 0.0;

    m_TagPt.m_X     = 0.0;
    m_TagPt.m_Y     = 0.0;
    m_TagPt.m_Z     = 0.0;
    m_TagScale.m_X  = 1.0;
    m_TagScale.m_Y  = 1.0;
    m_TagScale.m_Z  = 1.0;
    m_TagRotation   = 0.0;
    m_TagVisible    = 0;

    if (m_bWithGroup)
      CreateStr("Group", m_sGroup, sizeof(m_sGroup)-1, FF_None);
    CreateStr("Tag", m_sTag, sizeof(m_sTag)-1, FF_None);
    CreateStr("Page", m_sPage, sizeof(m_sPage)-1, FF_None);
    CreateStr("Symbol", m_sSymbol, sizeof(m_sSymbol)-1, FF_None);
    CreateVal("InsertX", &m_Insert.m_X, ADOX::adDouble, FF_Required);
    CreateVal("InsertY", &m_Insert.m_Y, ADOX::adDouble, FF_Required);
    CreateVal("InsertZ", &m_Insert.m_Z, ADOX::adDouble, FF_Required);
    CreateVal("ScaleX", &m_Scale.m_X, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("ScaleY", &m_Scale.m_Y, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("ScaleZ", &m_Scale.m_Z, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("Rotation", &m_Rotation, ADOX::adDouble, FF_None, &DefValZero);

    CreateVal("TagX", &m_TagPt.m_X, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("TagY", &m_TagPt.m_Y, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("TagZ", &m_TagPt.m_Z, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("TagScaleX", &m_TagScale.m_X, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("TagScaleY", &m_TagScale.m_Y, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("TagScaleZ", &m_TagScale.m_Z, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("TagRotation", &m_TagRotation, ADOX::adDouble, FF_None, &DefValZero);
    CreateVal("TagVisible", &m_TagVisible, ADOX::adBoolean, FF_None);

    CreateIndex("Tag", false, false);
    sm_bDoneInit = 1;
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------


BOOL CNeutralGrfUnits::sm_bDoneInit = 0;
CNeutralGrfUnits::CNeutralGrfUnits(bool WithGroup, KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  m_bWithGroup=WithGroup;
  sTableName = "GraphicsUnits";
  Init();
  }

//-------------------------------------------------------------------------

void CNeutralGrfUnits::Init()
  {
  if (m_pDB)
    {
    DefineClass();

    strcpy(m_sGroup, "");
    strcpy(m_sTag, "");
    strcpy(m_sPage, "");
    strcpy(m_sSymbol, "");
    strcpy(m_sClassID, "");
    m_Insert.m_X    = 0.0;
    m_Insert.m_Y    = 0.0;
    m_Insert.m_Z    = 0.0;
    m_Scale.m_X     = 1.0;
    m_Scale.m_Y     = 1.0;
    m_Scale.m_Z     = 1.0;
    m_Rotation      = 0.0;

    m_TagPt.m_X     = 0.0;
    m_TagPt.m_Y     = 0.0;
    m_TagPt.m_Z     = 0.0;
    m_TagScale.m_X  = 1.0;
    m_TagScale.m_Y  = 1.0;
    m_TagScale.m_Z  = 1.0;
    m_TagRotation   = 0.0;
    m_TagVisible    = 0;

    if (m_bWithGroup)
      CreateStr("Group", m_sGroup, sizeof(m_sGroup)-1, FF_None);
    CreateStr("Tag", m_sTag, sizeof(m_sTag)-1, FF_None);
    CreateStr("Page", m_sPage, sizeof(m_sPage)-1, FF_None);
    CreateStr("Symbol", m_sSymbol, sizeof(m_sSymbol)-1, FF_None);
    CreateStr("ClassID", m_sClassID, sizeof(m_sClassID)-1, FF_None);
    CreateVal("InsertX", &m_Insert.m_X, ADOX::adDouble, FF_Required);
    CreateVal("InsertY", &m_Insert.m_Y, ADOX::adDouble, FF_Required);
    CreateVal("InsertZ", &m_Insert.m_Z, ADOX::adDouble, FF_Required);
    CreateVal("ScaleX", &m_Scale.m_X, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("ScaleY", &m_Scale.m_Y, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("ScaleZ", &m_Scale.m_Z, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("Rotation", &m_Rotation, ADOX::adDouble, FF_None, &DefValZero);

    CreateVal("TagX", &m_TagPt.m_X, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("TagY", &m_TagPt.m_Y, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("TagZ", &m_TagPt.m_Z, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("TagScaleX", &m_TagScale.m_X, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("TagScaleY", &m_TagScale.m_Y, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("TagScaleZ", &m_TagScale.m_Z, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("TagRotation", &m_TagRotation, ADOX::adDouble, FF_None, &DefValZero);
    CreateVal("TagVisible", &m_TagVisible, ADOX::adBoolean, FF_None);

    CreateVal("LoBndX", &m_LoBnd.m_X, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("LoBndY", &m_LoBnd.m_Y, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("LoBndZ", &m_LoBnd.m_Z, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("HiBndX", &m_HiBnd.m_X, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("HiBndY", &m_HiBnd.m_Y, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("HiBndZ", &m_HiBnd.m_Z, ADOX::adDouble, FF_None, &DefValNAN);

    CreateIndex("Tag", false, false);
    sm_bDoneInit = 1;
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

BOOL CNeutralGrfLinks::sm_bDoneInit = 0;
CNeutralGrfLinks::CNeutralGrfLinks(bool WithGroup, KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  m_bWithGroup=WithGroup;
  sTableName = "GraphicsLinks";
  Init();
  }

//-------------------------------------------------------------------------

void CNeutralGrfLinks::Init()
  {
  if (m_pDB)
    {
    DefineClass();

    strcpy(m_sGroup, "");
    strcpy(m_sTag, "");
    strcpy(m_sPage, "");
    strcpy(m_sClassID, "");
    m_Insert.m_X    = 0.0;
    m_Insert.m_Y    = 0.0;
    m_Insert.m_Z    = 0.0;

    m_TagPt.m_X     = 0.0;
    m_TagPt.m_Y     = 0.0;
    m_TagPt.m_Z     = 0.0;
    m_TagScale.m_X  = 1.0;
    m_TagScale.m_Y  = 1.0;
    m_TagScale.m_Z  = 1.0;
    m_TagRotation   = 0.0;
    m_TagVisible    = 0;

    if (m_bWithGroup)
      CreateStr("Group", m_sGroup, sizeof(m_sGroup)-1, FF_None);
    CreateStr("Tag", m_sTag, sizeof(m_sTag)-1, FF_None);
    CreateStr("Page", m_sPage, sizeof(m_sPage)-1, FF_None);
    CreateStr("ClassID", m_sClassID, sizeof(m_sClassID)-1, FF_None);
    CreateVal("InsertX", &m_Insert.m_X, ADOX::adDouble, FF_Required);
    CreateVal("InsertY", &m_Insert.m_Y, ADOX::adDouble, FF_Required);
    CreateVal("InsertZ", &m_Insert.m_Z, ADOX::adDouble, FF_Required);

    CreateVal("TagX", &m_TagPt.m_X, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("TagY", &m_TagPt.m_Y, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("TagZ", &m_TagPt.m_Z, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("TagScaleX", &m_TagScale.m_X, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("TagScaleY", &m_TagScale.m_Y, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("TagScaleZ", &m_TagScale.m_Z, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("TagRotation", &m_TagRotation, ADOX::adDouble, FF_None, &DefValZero);
    CreateVal("TagVisible", &m_TagVisible, ADOX::adBoolean, FF_None);

    CreateVal("LoBndX", &m_LoBnd.m_X, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("LoBndY", &m_LoBnd.m_Y, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("LoBndZ", &m_LoBnd.m_Z, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("HiBndX", &m_HiBnd.m_X, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("HiBndY", &m_HiBnd.m_Y, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("HiBndZ", &m_HiBnd.m_Z, ADOX::adDouble, FF_None, &DefValNAN);

    CreateIndex("Tag", false, false);
    sm_bDoneInit = 1;
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

BOOL CNeutralGrfLinklines::sm_bDoneInit = 0;
CNeutralGrfLinklines::CNeutralGrfLinklines(bool WithGroup, KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  m_bWithGroup=WithGroup;
  sTableName = "GraphicsLinkLines";
  Init();
  }

//-------------------------------------------------------------------------

void CNeutralGrfLinklines::Init()
  {
  if (m_pDB)
    {
    DefineClass();

    strcpy(m_sGroup, "");
    strcpy(m_sTag, "");
    strcpy(m_sPage, "");
    m_lSeqNo        = 0;
    m_Vertex.m_X    = 0.0;
    m_Vertex.m_Y    = 0.0;
    m_Vertex.m_Z    = 0.0;

    if (m_bWithGroup)
      CreateStr("Group", m_sGroup, sizeof(m_sGroup)-1, FF_None);
    CreateStr("Tag", m_sTag, sizeof(m_sTag)-1, FF_None);
    CreateStr("Page", m_sPage, sizeof(m_sPage)-1, FF_None);
    CreateVal("SequenceNo", &m_lSeqNo, ADOX::adInteger, FF_Required);
    CreateVal("VertexX", &m_Vertex.m_X, ADOX::adDouble, FF_Required);
    CreateVal("VertexY", &m_Vertex.m_Y, ADOX::adDouble, FF_Required);
    CreateVal("VertexZ", &m_Vertex.m_Z, ADOX::adDouble, FF_Required);
    CreateIndex("Tag", false, false);
    sm_bDoneInit = 1;
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

BOOL CNeutralGrfSymbols::sm_bDoneInit = 0;
CNeutralGrfSymbols::CNeutralGrfSymbols(bool WithGroup, KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  m_bWithGroup=WithGroup;
  sTableName = "GraphicsSymbols";
  Init();
  }

//-------------------------------------------------------------------------

void CNeutralGrfSymbols::Init()
  {
  if (m_pDB)
    {
    DefineClass();

    strcpy(m_sName, "");
    m_pSymbolData   = 0;

    CreateStr("Name", m_sName, sizeof(m_sName)-1, FF_None);
    CreateLongVal("Symbol", &m_pSymbolData, &m_SymbolLen, ADOX::adLongVarBinary, FF_None);
    CreateIndex("Name", true, true);
    sm_bDoneInit = 1;
    }
  }

//===========================================================================

CNeutralGrfImportExport::CNeutralGrfImportExport() :  \
m_Origin(0,0,0), m_Scale(1,1,1)
  {
  m_pIn = NULL;
  m_pUn = NULL;
  m_pLk = NULL;
  m_pLl = NULL;
  m_pSy = NULL;
  m_pGr = NULL;
  m_pGI = NULL;

  m_pOldTags=NULL;
  m_pNewTags=NULL;

  m_bWithGroups=false;

  m_iFixupRule=eFixup_NoChange;

  m_bExportingSymbols=false;
  m_bImportingSymbols=false;

  }

  //---------------------------------------------------------------------------

CNeutralGrfImportExport::~CNeutralGrfImportExport()
  {
  ASSERT_RDB(m_pIn==NULL && m_pUn==NULL && m_pLk==NULL && m_pLl==NULL && m_pSy==NULL && m_pGr==NULL && m_pGI==NULL, 
    "Incomplete Import Export !", __FILE__, __LINE__);
  }

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::SetGroup(bool On)
  {
  m_bWithGroups=On;
  };

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::SetGroup(LPCTSTR Group)
  {
  m_bWithGroups=true;
  m_sGroup=Group;
  };

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::SetGroupDesc(LPCTSTR GroupDesc)
  {
  m_sGroupDesc=GroupDesc;
  };

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::SetExportTagList(CStringArray *OldTags, CStringArray *NewTags)
  {
  m_iFixupRule=eFixup_ExchangeTags;
  m_pOldTags=OldTags;
  m_pNewTags=NewTags;
  };

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::SetExportCoOrds(const CDblTriPt & Origin, const CDblTriPt & Lo, const CDblTriPt & Hi, const CDblTriPt & AlignLo, const CDblTriPt & AlignHi)
  {
  m_Origin=Origin;
  m_Lo=Lo;
  m_Hi=Hi;
  m_AlignLo=AlignLo;
  m_AlignHi=AlignHi;
  };

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::SetImportFilter(LPCTSTR Group)
  {
  SetGroup(Group);
  };

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::SetImportTagFixups(long FixupRule, LPCTSTR String1, LPCTSTR String2, CStringArray *OldTags, CStringArray *NewTags)
  {
  ASSERT(FixupRule!=eFixup_ExchangeTags || (OldTags && NewTags));
  m_iFixupRule = FixupRule;
  m_sString1   = String1;
  m_sString2   = String2;
  m_pOldTags   = OldTags;
  m_pNewTags   = NewTags;
  };

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::SetImportOrigin(const CDblTriPt & Origin, const CDblTriPt & Scale)
  {
  m_Origin=Origin;
  m_Scale=Scale;
  };

//---------------------------------------------------------------------------

bool CNeutralGrfImportExport::Open(LPCTSTR pDatabaseName, bool OpenForExport)
  {
  Strng sDBName=pDatabaseName;

  WIN32_FIND_DATA fd;
  if (FileExists(sDBName(), fd))
    {
    if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      {
      LogError("GraphicslNDB", LF_Exclamation, "Cannot open read-only Database '%s'", sDBName());
      return false;
      }
    if (!m_DB.OpenDB(DBConnect_Default, EscapeQuote(sDBName()), false, false))
      {
      LogError("GraphicslNDB", LF_Exclamation, "Cannot open Database '%s'", sDBName());
      return false;
      }
    }
  else 
    {
    if (OpenForExport)
      {
      long UtilsDBFmt = ScdPFUser.RdInt("General", "UtilsDBFormat", DBConnect_Default);
      if (!m_DB.CreateDB(UtilsDBFmt, sDBName(), 0))
        {
        LogError("GraphicslNDB", LF_Exclamation, "Cannot create Database '%s'", sDBName());
        return false;
        }
      }
    else 
      {
      //CMsgWindow::Show(true, false);
      LogError("GraphicslNDB", LF_Exclamation, "Cannot open Database '%s'", sDBName());
      return false;
      }
    }

  //open tables...
  m_pIn = new CNeutralGrfInserts(m_bWithGroups, &m_DB);
  m_pIn->SetIgnoreWriteErr3022(true);
  m_pUn = new CNeutralGrfUnits(m_bWithGroups, &m_DB);
  m_pUn->SetIgnoreWriteErr3022(true);
  m_pLk = new CNeutralGrfLinks(m_bWithGroups, &m_DB);
  m_pLk->SetIgnoreWriteErr3022(true);
  m_pLl = new CNeutralGrfLinklines(m_bWithGroups, &m_DB);
  m_pLl->SetIgnoreWriteErr3022(true);

  if (OpenForExport)
    {
    m_pSy = new CNeutralGrfSymbols(m_bWithGroups, &m_DB);
    m_pSy->SetIgnoreWriteErr3022(true);
    }
  else if (m_DB.IsExistentClass("GraphicsSymbols"))
    {
    m_pSy = new CNeutralGrfSymbols(m_bWithGroups, &m_DB);
    m_pSy->SetIgnoreWriteErr3022(true);
    }

  if (m_bWithGroups)
    {
    m_pGr = new CNeutralGroups(&m_DB);
    m_pGr->SetIgnoreWriteErr3022(true);
    }

  m_pGI=NULL;
  if (m_Opts & eNDB_GroupInserts)
    {
    if (OpenForExport)
      {
      m_pGI = new CNeutralGroupInserts(&m_DB);
      m_pGI->SetIgnoreWriteErr3022(true);
      }
    else if (m_DB.IsExistentClass("GroupInserts"))
      {
      m_pGI = new CNeutralGroupInserts(&m_DB);
      m_pGI->SetIgnoreWriteErr3022(true);
      }
    }

  try
    {
    if (OpenForExport)
      {
      m_pIn->Open(NULL/*, dbDenyWrite*/);
      m_pUn->Open(NULL/*, dbDenyWrite*/);
      m_pLk->Open(NULL/*, dbDenyWrite*/);
      m_pLl->Open(NULL/*, dbDenyWrite*/);
      if (m_pSy)
        m_pSy->Open(NULL/*, dbDenyWrite*/);
      if (m_pGr)
        m_pGr->Open(NULL/*, dbDenyWrite*/);
      if (m_pGI)
        m_pGI->Open(NULL/*, dbDenyWrite*/);
      }
    }
  catch(_com_error & e)
    {
    LogError("GraphicslNDB", LF_Exclamation, "Cannot open table\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
    Close();
    return false;
    }

  //delete contents...
  try
    {
    if (OpenForExport)
      {
      if (m_bWithGroups)
        {
        CFileRec * Tables[] = {m_pIn, m_pUn, m_pLk, m_pLl, m_pGr, m_pGI, NULL};
        for (int i=0; Tables[i]; i++)
          {
          CString Select;
          Select.Format("SELECT * FROM [%s] WHERE [Group] = '%s'", Tables[i]->TableName(), m_sGroup);
          Tables[i]->DeleteAll(Select);
          }
        }
      else
        {
        m_pIn->DeleteAll();
        m_pUn->DeleteAll();
        m_pLk->DeleteAll();
        m_pLl->DeleteAll();
        // ??? m_pSy
        }
      }
    }
  catch(_com_error & e)
    {
    LogError("GraphicslNDB", LF_Exclamation, "Cannot delete table contents\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
    Close();
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::Close()
  {
  if (m_pIn)
    {
    m_pIn->Close();
    delete m_pIn;
    m_pIn = NULL;
    }
  if (m_pUn)
    {
    m_pUn->Close();
    delete m_pUn;
    m_pUn = NULL;
    }
  if (m_pLk)
    {
    m_pLk->Close();
    delete m_pLk;
    m_pLk = NULL;
    }
  if (m_pLl)
    {
    m_pLl->Close();
    delete m_pLl;
    m_pLl = NULL;
    }
  if (m_pSy)
    {
    m_pSy->Close();
    delete m_pSy;
    m_pSy = NULL;
    }
  if (m_pGr)
    {
    m_pGr->Close();
    delete m_pGr;
    m_pGr = NULL;
    }
  if (m_pGI)
    {
    m_pGI->Close();
    delete m_pGI;
    m_pGI = NULL;
    }

  m_DB.CloseDB();
  }

//---------------------------------------------------------------------------

void CheckSymbol(char* DrwGrp, Strng & sym)
  {
  int sl = sym.Len();
  const int guidLen = 38;
  if (sl>guidLen && sym[sl-1]=='}' && sym[sl-guidLen]=='{')
    {
    sym = sym.Left(sl-guidLen);//remove old GUID
    sl = sym.Len();
    }
  const int gl = strlen(DrwGrp);
  if (sl>gl && sym[gl]=='$' && _strnicmp(sym(), DrwGrp, gl)==0)
    sym = sym.Right(sl-gl-1);
  }

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::WriteInsert(LPCTSTR Group, CGrfTagInfo & GTI, CDblTriPt Org, LPCTSTR pPage, LPCTSTR pDocGUID)
  {
  //assume string lengths are long enough!
  strcpy(m_pIn->m_sGroup, Group?Group:"");
  strcpy(m_pIn->m_sTag, GTI.m_sTag());
  strcpy(m_pIn->m_sPage, pPage);

  Strng sym(GTI.m_sSymbol());
  CheckSymbol(GTI.m_sDrwGroup(), sym);
  strcpy(m_pIn->m_sSymbol, GTI.m_sDrwGroup());
  strcat(m_pIn->m_sSymbol, ":");
  strcat(m_pIn->m_sSymbol, sym());
  strcat(m_pIn->m_sSymbol, pDocGUID);
  ASSERT_RDB(strlen(m_pIn->m_sSymbol)<GrfDBMaxSymbolLen-1, "Symbol string length!!!", __FILE__, __LINE__);

  m_pIn->m_Insert.m_X   = GTI.m_Node.m_X-Org.m_X;
  m_pIn->m_Insert.m_Y   = GTI.m_Node.m_Y-Org.m_Y;
  m_pIn->m_Insert.m_Z   = GTI.m_Node.m_Z-Org.m_Z;
  m_pIn->m_Scale.m_X    = GTI.m_Node.m_XScale;
  m_pIn->m_Scale.m_Y    = GTI.m_Node.m_YScale;
  m_pIn->m_Scale.m_Z    = GTI.m_Node.m_ZScale;
  m_pIn->m_Rotation     = GTI.m_Node.m_Rotation;

  m_pIn->m_TagPt.m_X    = GTI.m_Tag.m_X-m_pIn->m_Insert.m_X;//Org.m_X;
  m_pIn->m_TagPt.m_Y    = GTI.m_Tag.m_Y-m_pIn->m_Insert.m_Y;//Org.m_Y;
  m_pIn->m_TagPt.m_Z    = GTI.m_Tag.m_Z-m_pIn->m_Insert.m_Z;//Org.m_Z;
  m_pIn->m_TagScale.m_X = GTI.m_Tag.m_XScale;
  m_pIn->m_TagScale.m_Y = GTI.m_Tag.m_YScale;
  m_pIn->m_TagScale.m_Z = GTI.m_Tag.m_ZScale;
  m_pIn->m_TagRotation  = GTI.m_Tag.m_Rotation;
  m_pIn->m_TagVisible   = GTI.m_Tag.m_Visible;
  
  m_pIn->Write();
  }

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::WriteUnit(LPCTSTR Group, CGrfTagInfo & GTI, CDblTriPt Org, LPCTSTR pPage, LPCTSTR pDocGUID)
  {
  //assume string lengths are long enough!
  strcpy(m_pUn->m_sGroup, Group?Group:"");
  strcpy(m_pUn->m_sTag, AdjustTag(GTI.m_sTag()));
  strcpy(m_pUn->m_sPage, pPage);

  Strng sym(GTI.m_sSymbol());
  CheckSymbol(GTI.m_sDrwGroup(), sym);
  strcpy(m_pUn->m_sSymbol, GTI.m_sDrwGroup());
  strcat(m_pUn->m_sSymbol, ":");
  strcat(m_pUn->m_sSymbol, sym());
  strcat(m_pUn->m_sSymbol, pDocGUID);
  ASSERT_RDB(strlen(m_pUn->m_sSymbol)<GrfDBMaxSymbolLen-1, "Symbol string length!!!", __FILE__, __LINE__);

  //strcpy(m_pUn->m_sSymbol, MakeValidBlockName(m_pUn->m_sSymbol));

  strcpy(m_pUn->m_sClassID, GTI.m_sClass());

  m_pUn->m_Insert.m_X   = GTI.m_Node.m_X-Org.m_X;
  m_pUn->m_Insert.m_Y   = GTI.m_Node.m_Y-Org.m_Y;
  m_pUn->m_Insert.m_Z   = GTI.m_Node.m_Z-Org.m_Z;
  m_pUn->m_Scale.m_X    = GTI.m_Node.m_XScale;
  m_pUn->m_Scale.m_Y    = GTI.m_Node.m_YScale;
  m_pUn->m_Scale.m_Z    = GTI.m_Node.m_ZScale;
  m_pUn->m_Rotation     = GTI.m_Node.m_Rotation;

  m_pUn->m_TagPt.m_X    = GTI.m_Tag.m_X-GTI.m_Node.m_X;// m_pUn->m_Insert.m_X;// Org.m_X;
  m_pUn->m_TagPt.m_Y    = GTI.m_Tag.m_Y-GTI.m_Node.m_Y;// m_pUn->m_Insert.m_Y;// Org.m_Y;
  m_pUn->m_TagPt.m_Z    = GTI.m_Tag.m_Z-GTI.m_Node.m_Z;// m_pUn->m_Insert.m_Z;// Org.m_Z;
  m_pUn->m_TagScale.m_X = GTI.m_Tag.m_XScale;
  m_pUn->m_TagScale.m_Y = GTI.m_Tag.m_YScale;
  m_pUn->m_TagScale.m_Z = GTI.m_Tag.m_ZScale;
  m_pUn->m_TagRotation  = GTI.m_Tag.m_Rotation;
  m_pUn->m_TagVisible   = GTI.m_Tag.m_Visible;

  m_pUn->m_LoBnd.m_X    = GTI.m_LoBnd.m_X;
  m_pUn->m_LoBnd.m_Y    = GTI.m_LoBnd.m_Y;
  m_pUn->m_LoBnd.m_Z    = GTI.m_LoBnd.m_Z;
  m_pUn->m_HiBnd.m_X    = GTI.m_HiBnd.m_X;
  m_pUn->m_HiBnd.m_Y    = GTI.m_HiBnd.m_Y;
  m_pUn->m_HiBnd.m_Z    = GTI.m_HiBnd.m_Z;

  m_pUn->Write();
  }

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::WriteLink(LPCTSTR Group, CGrfTagInfo & GTI, CDblTriPt Org, CLinePointsArray & LPA, char* pPage, LPCTSTR pDocGUID)
  {
  //assume string lengths are long enough!
  strcpy(m_pLk->m_sGroup, Group?Group:"");
  strcpy(m_pLk->m_sTag, AdjustTag(GTI.m_sTag()));
  strcpy(m_pLk->m_sPage, pPage);
  strcpy(m_pLk->m_sClassID, GTI.m_sClass());
  m_pLk->m_Insert.m_X   = GTI.m_Node.m_X-Org.m_X;
  m_pLk->m_Insert.m_Y   = GTI.m_Node.m_Y-Org.m_Y;
  m_pLk->m_Insert.m_Z   = GTI.m_Node.m_Z-Org.m_Z;

  m_pLk->m_TagPt.m_X    = GTI.m_Tag.m_X-GTI.m_Node.m_X;//m_pLk->m_Insert.m_X;// Org.m_X;
  m_pLk->m_TagPt.m_Y    = GTI.m_Tag.m_Y-GTI.m_Node.m_Y;//m_pLk->m_Insert.m_Y;// Org.m_Y;
  m_pLk->m_TagPt.m_Z    = GTI.m_Tag.m_Z-GTI.m_Node.m_Z;//m_pLk->m_Insert.m_Z;// Org.m_Z;
  m_pLk->m_TagScale.m_X = GTI.m_Tag.m_XScale;
  m_pLk->m_TagScale.m_Y = GTI.m_Tag.m_YScale;
  m_pLk->m_TagScale.m_Z = GTI.m_Tag.m_ZScale;
  m_pLk->m_TagRotation  = GTI.m_Tag.m_Rotation;
  m_pLk->m_TagVisible   = GTI.m_Tag.m_Visible;

  m_pLk->m_LoBnd.m_X    = GTI.m_LoBnd.m_X;
  m_pLk->m_LoBnd.m_Y    = GTI.m_LoBnd.m_Y;
  m_pLk->m_LoBnd.m_Z    = GTI.m_LoBnd.m_Z;
  m_pLk->m_HiBnd.m_X    = GTI.m_HiBnd.m_X;
  m_pLk->m_HiBnd.m_Y    = GTI.m_HiBnd.m_Y;
  m_pLk->m_HiBnd.m_Z    = GTI.m_HiBnd.m_Z;

  //m_pLk->lLinePoints = GTI.PointsCount;
  m_pLk->Write();

  strcpy(m_pLl->m_sGroup, Group?Group:"");
  strcpy(m_pLl->m_sTag, AdjustTag(GTI.m_sTag()));
  strcpy(m_pLl->m_sPage, pPage);
  for (int i=0; i<GTI.m_PointsCount; i++)
    {
    m_pLl->m_lSeqNo = LPA[i].seq;
    m_pLl->m_Vertex.m_X = LPA[i].x-Org.m_X;
    m_pLl->m_Vertex.m_Y = LPA[i].y-Org.m_Y;
    m_pLl->m_Vertex.m_Z = LPA[i].z-Org.m_Z;
    m_pLl->Write();
    }
  }

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::WriteGroupInfo(LPCTSTR Group, LPCTSTR Desc, CDblTriPt Org, CDblTriPt Lo, CDblTriPt Hi, CDblTriPt AlignLo, CDblTriPt AlignHi)
  {
  //assume string lengths are long enough!
  strcpy(m_pGr->m_sGroup, Group?Group:"");
  strcpy(m_pGr->m_sDescription, Desc?Desc:"");
  m_pGr->m_Lo       = Lo-Org;
  m_pGr->m_Hi       = Hi-Org;
  m_pGr->m_AlignLo  = AlignLo-Org;
  m_pGr->m_AlignHi  = AlignHi-Org;
  m_pGr->Write();
  }

//---------------------------------------------------------------------------

bool CNeutralGrfImportExport::DoExportValidate(eScdNDBOptions Opts, CDocTemplate & Template, CString &ErrMsg)
  {
  if (!gs_pPrj->AllGrfLoaded())
    {
    LogError("GraphicslNDB", LF_Exclamation, "All graphics pages are not open!");
    return false;
    }

  return true;
  }

//---------------------------------------------------------------------------

bool CNeutralGrfImportExport::DoImportValidate(eScdNDBOptions Opts, CDocTemplate & Template, CString &ErrMsg)
  {
  if (!gs_pPrj->AllGrfLoaded())
    {
    LogError("GraphicslNDB", LF_Exclamation, "All graphics pages are not open!");
    return false;
    }

  return true;
  }

//---------------------------------------------------------------------------

bool CNeutralGrfImportExport::DoExport(eScdNDBOptions Opts, CDocTemplate & Template, LPCTSTR pDatabaseName)
  {
  if (!gs_pPrj->AllGrfLoaded())
    {
    LogError("GraphicslNDB", LF_Exclamation, "All graphics pages are not open!");
    return false;
    }

  CWaitMsgCursor Wait("Export graphics data");

  m_Opts=Opts;
  if (m_Opts & eNDB_Graphics)
    {
    if (Open(pDatabaseName, true))
      {
      if ((m_Opts & eNDB_BlankTables)==0)
        {
        POSITION Pos = Template.GetFirstDocPosition();
        while (Pos)
          {
          CGrfDoc* pGDoc = (CGrfDoc*)(Template.GetNextDoc(Pos));
          //pGDoc->ExportGraphicsData(*this);
          DoExport(Opts, pGDoc);
          }
        }
      Close();
      return true;
      }
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::DoExport(eScdNDBOptions Opts, CGrfDoc * pDoc) 
  {
  m_Opts=Opts;
  CGrfTagInfoArray GTIA;
  
  if (OpenSymbolExport())
    pDoc->GCB.pDrw->SaveSymbols(m_SymbolExportFolder.GetBuffer(), pDoc->m_sDocGUID());

  bool DoAllInserts=true;
  const int Cnt = pDoc->GetTagListInfo(DoAllInserts, GTIA);
  if (Cnt>0)
    {
    long InsertCnt=0;
    bool DoneOne = false;
    Strng Page = pDoc->GetTitle();
    if (Page.Len()>4)
      Page = Page.Left(Page.Len()-4);//remove .scg
    POSITION pos = pDoc->GetFirstViewPosition();
    CView* pFirstView = pDoc->GetNextView( pos );
    CGrfWnd* pGWnd = (CGrfWnd*)pFirstView;
    for (int i=0; i<Cnt; i++)
      {
      CGrfTagInfo & I = GTIA[i];
      if (I.m_bHasTag && FindOldTag(I.m_sTag())<0)
        continue;

      int TagTyp = -1; //error
      if (I.m_bHasTag)
        {
        CXM_ObjectTag ObjTag(I.m_sTag(), TABOpt_Parms);
        CXM_ObjectData ObjData;
        CXM_Route Route;
        TagObjClass * pTagObjC = NULL;
        if (pGWnd->XReadTaggedItem(ObjTag, ObjData, Route))
          {
          I.m_sClass = ObjData.FirstItem()->Class();
          pTagObjC = TagObjClass::FindClassId(I.m_sClass());
          if (pTagObjC==NULL)
            pTagObjC = TagObjClass::FindClassName(I.m_sClass());
          if (pTagObjC && (_stricmp(FlwLinkGrp, pTagObjC->Group())==0 || 
            _stricmp(CtrlLinkGrp, pTagObjC->Group())==0 || 
            _stricmp(ElecLinkGrp, pTagObjC->Group())==0 || 
            _stricmp(AirLinkGrp, pTagObjC->Group())==0))
            TagTyp = 2; //link
          else
            TagTyp = 1; //unit/node
          if (pTagObjC)
            {
            I.m_sDrwGroup=pTagObjC->DrwGroup();
            }
          }
        else
          {
          TagTyp=3;
          I.m_sClass = "";
          }

        if (TagTyp==2)
          {
          DoneOne = true;
          CLinePointsArray LPA;
          pDoc->GCB.pDrw->CollectLinkInfo(I, LPA);
          WriteLink(m_sGroup, I, m_Origin, LPA, Page(), pDoc->m_sDocGUID());
          }
        else if (TagTyp==1)// || TagTyp==0)
          {
          DoneOne = true;
          WriteUnit(m_sGroup, I, m_Origin, Page(), pDoc->m_sDocGUID());
          ExportSymbol(I, m_Origin, pDoc->m_sDocGUID());
          }
        }
      if (TagTyp==3)
        {
        I.m_sTag.Set("$%i$", ++InsertCnt);
        I.m_sClass="Insert";
        I.m_sDrwGroup="Inserts";

        DoneOne=true;
        WriteInsert(m_sGroup, I, m_Origin, Page(), pDoc->m_sDocGUID());
        ExportSymbol(I, m_Origin, pDoc->m_sDocGUID());
        }
      }
    if (m_bWithGroups && DoneOne)
      WriteGroupInfo(m_sGroup, m_sGroupDesc, m_Origin, m_Lo, m_Hi, m_AlignLo, m_AlignHi);
    }

  CloseSymbolExport();
  }

//---------------------------------------------------------------------------

long CNeutralGrfImportExport::GetGroups(CNeutralGrpDescList &List, LPCTSTR pDatabaseName)
  {
  CWaitMsgCursor Wait("Get List");
  List.RemoveAll();
  if (Open(pDatabaseName, false))
    {
    //determine list of required graphics pages...
    Strng Page;
    CString SQL = "SELECT Group,Description FROM Groups ";
    if (0 && m_sGroup.GetLength()>0)
      {
      SQL += " WHERE Group LIKE '";
      //SQL += m_sGroup;
      SQL += "%'";
      }
    CArray <Strng, Strng&> GrfPages;
    try
      {
      ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));

      pRS->Open(_variant_t(SQL), _variant_t((IDispatch*)m_DB.GetConnectionPtr()), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
      if (!pRS->adEOF)
        pRS->MoveFirst();
      while (!pRS->adEOF)
        {
        CNeutralGrpDescItem Grp;
        Grp.m_sGroup=pRS->Fields->GetItem("Group")->Value;
        Grp.m_Desc=pRS->Fields->GetItem("Description")->Value;
        List.AddTail(Grp);
        pRS->MoveNext();
        }
      pRS->Close();
      }
    catch(_com_error & e)
      {
      LogError("GraphicslNDB", LF_Exclamation, "Cannot open and read query\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
      Close();
      return 0;
      }

    Close();
    }
  return List.GetCount();
  }

//---------------------------------------------------------------------------

bool CNeutralGrfImportExport::DeleteGroup(LPCTSTR Name, LPCTSTR pDatabaseName)
  {
  CWaitMsgCursor Wait("Delete Grf Group");
  //List.RemoveAll();
  //if (Open(pDatabaseName, false))
  //  {
  //  //determine list of required graphics pages...
  //  Strng Page;
  //  CString SQL = "SELECT Group,Description FROM Groups ";
  //  if (0 && m_sGroup.GetLength()>0)
  //    {
  //    SQL += " WHERE Group LIKE '";
  //    //SQL += m_sGroup;
  //    SQL += "%'";
  //    }
  //  CArray <Strng, Strng&> GrfPages;
  //  try
  //    {
  //    ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));

  //    pRS->Open(_variant_t(SQL), _variant_t((IDispatch*)m_DB.GetConnectionPtr()), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
  //    if (!pRS->adEOF)
  //      pRS->MoveFirst();
  //    while (!pRS->adEOF)
  //      {
  //      CNeutralGrpDescItem Grp;
  //      Grp.m_sGroup=pRS->Fields->GetItem("Group")->Value;
  //      Grp.m_Desc=pRS->Fields->GetItem("Description")->Value;
  //      List.AddTail(Grp);
  //      pRS->MoveNext();
  //      }
  //    pRS->Close();
  //    }
  //  catch(_com_error & e)
  //    {
  //    LogError("GraphicslNDB", LF_Exclamation, "Cannot open and read query\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
  //    Close();
  //    return 0;
  //    }

  //  Close();
  //  }
  //return List.GetCount();
  return false;
  }

//---------------------------------------------------------------------------

bool CNeutralGrfImportExport::OpenSymbolExport()
  {
  m_bExportingSymbols=false;

  if (m_pSy==NULL)
    return m_bExportingSymbols;

  m_SymbolExportFolder=TemporaryFiles();
  m_SymbolExportFolder+="SymbolsExport";//pDoc->m_sDocGUID();
  m_SymbolExportFolder+="\\";
  Strng E;
  if (FnCreatePath(m_SymbolExportFolder.GetBuffer(), E))
    {
    m_bExportingSymbols=true;
    }
  else
    LogError("ExportSymbols", 0, "Cannot create %s : %s", m_SymbolExportFolder, E());

  return m_bExportingSymbols;
  }

//---------------------------------------------------------------------------

bool CNeutralGrfImportExport::ExportSymbol(CGrfTagInfo & GTI, CDblTriPt m_Origin, LPCTSTR pDocGUID)
  {
  if (!m_bExportingSymbols)
    return false;

  //assume string lengths are long enough!
  Strng sym(GTI.m_sSymbol());
  CheckSymbol(GTI.m_sDrwGroup(), sym);
  strcpy(m_pSy->m_sName, GTI.m_sDrwGroup());
  strcat(m_pSy->m_sName, ":");
  strcat(m_pSy->m_sName, sym());
  strcat(m_pSy->m_sName, pDocGUID);
  ASSERT_RDB(strlen(m_pSy->m_sName)<GrfDBMaxSymbolLen-1, "Symbol string length!!!", __FILE__, __LINE__);

  //strcpy(m_pSy->m_sName, MakeValidBlockName(m_pUn->m_sSymbol));

  Strng Fn(m_SymbolExportFolder);
  Strng Nm(GTI.m_sSymbol());
  Nm+=pDocGUID;
  Fn+=MakeValidBlockName(Nm());
  Fn.FnCheckExtension(".dxf");
 
  FILE * h=fopen(Fn(), "rb");
  if (h)
    {
    if (fseek(h, 0, SEEK_END)!=0)
      goto End;
    int iLen=ftell(h);
    m_pSy->m_pSymbolData = new char[iLen];
    m_pSy->m_SymbolLen=iLen;
    if (fseek(h, 0, SEEK_SET)!=0)
      goto End;
    if (fread(m_pSy->m_pSymbolData, sizeof(char), iLen, h)!=iLen)
      LogError("ExportSymbol", 0, "File read error %s", Fn());
    fclose(h);
    m_pSy->Write();
    //delete m_pSy->m_pSymbolData;
    return true;
    }
End:
  LogError("ExportSymbol", 0, "File open error %s", Fn());
  return false;
  }

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::CloseSymbolExport()
  {
  if (!KEEPTEMPFOLDERS && m_bExportingSymbols)
    ClearFolder(m_SymbolExportFolder, true);
  m_bExportingSymbols=false;
  }

//---------------------------------------------------------------------------

bool CNeutralGrfImportExport::OpenSymbolImport()
  {
  m_bImportingSymbols=false;

  if (m_pSy==NULL)
    return m_bImportingSymbols;

  m_SymbolImportFolder=TemporaryFiles();
  m_SymbolImportFolder+="SymbolsImport";//pDoc->m_sDocGUID();
  m_SymbolImportFolder+="\\";
  Strng E;
  if (FnCreatePath(m_SymbolImportFolder.GetBuffer(), E))
    {
    m_bImportingSymbols=true;
    SetGrfSymbolImportFiles(m_SymbolImportFolder.GetBuffer());
    }
  else
    LogError("ImportSymbols", 0, "Cannot create %s : %s", m_SymbolExportFolder, E());

  return m_bImportingSymbols;
  }

//---------------------------------------------------------------------------

bool CNeutralGrfImportExport::ImportSymbol(LPCTSTR Name, bool MustExist)
  {
  if (m_bImportingSymbols)
    {
    try
      {
      //ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
      CString SQL;
      SQL.Format("SELECT * FROM GraphicsSymbols WHERE [Name] = '%s'", Name);
      m_pSy->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
      ADODB::_RecordsetPtr syrs = m_pSy->Recordset();
      if (!syrs->adEOF)
        syrs->MoveFirst();
      Strng PrevPage;
      CGrfDoc* pGDoc = NULL;
      if (syrs->adEOF)
        {
        if (MustExist)
          LogError("GraphicslNDB", LF_Exclamation, "Cannot find graphics symbol %s", Name);
        }
      else
        {
        m_pSy->Read();

        CString NmNoGrp(Name);
        int iColon=NmNoGrp.Find(':');
        CString Group;
        if (iColon>0)
          {
          Group=NmNoGrp.Left(iColon);
          NmNoGrp=NmNoGrp.Right(NmNoGrp.GetLength()-(1+iColon));
          }
        int iStar=NmNoGrp.Find('*');
        if (iStar>0)
          NmNoGrp.SetAt(iStar, '$');

        Strng Fn(m_SymbolImportFolder);
        if (Group.GetLength()>0)
          {
          Fn+=Group;
          Fn+="\\";
          Strng E;
          if (!FnCreatePath(Fn(), E))
            LogError("ImportSymbol", 0, "Path %s not created :%s", Fn(), E());
          }
        Fn+=NmNoGrp;
        Fn.FnCheckExtension(".dxf");
        FILE *h=fopen(Fn(), "wb");
        if (h)
          {
          if (fwrite(m_pSy->m_pSymbolData, sizeof(char), m_pSy->m_SymbolLen, h)!=m_pSy->m_SymbolLen)
            LogError("ImportSymbol", 0, "File write error %s", Fn());
          fclose(h);
          }
        else
          LogError("ImportSymbol", 0, "File open error %s", Fn());

        delete []m_pSy->m_pSymbolData;
        m_pSy->m_pSymbolData=NULL;
        m_pSy->m_SymbolLen=0;
        }
      //pRS->Close();
      }
    catch(_com_error & e)
      {
      LogError("GraphicslNDB", LF_Exclamation, "Cannot open and read query\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
      Close();
      return false;
      }
    }
  return true;
  }

//---------------------------------------------------------------------------

void CNeutralGrfImportExport::CloseSymbolImport()
  {
  SetGrfSymbolImportFiles("");
  if (!KEEPTEMPFOLDERS && m_bImportingSymbols)
    ClearFolder(m_SymbolImportFolder, true);
  m_bImportingSymbols=false;
  }

//---------------------------------------------------------------------------

static CString FixSymbol(LPCTSTR m_sSymbol, LPCTSTR m_sClassID)
  {
  // Unit Symbols
  CString Sym=m_sSymbol;
  Sym=Sym.Trim();
  if (Sym=="" || Sym.CompareNoCase("default")==0)
    Sym=m_sClassID;
  if ((Sym.GetLength()>0) && (Sym.Find(':')<0) && (strlen(m_sClassID)>0))
    {
    TagObjClass * pTagObjC = TagObjClass::FindClassId((LPTSTR)m_sClassID);
    if (pTagObjC==NULL)
      pTagObjC = TagObjClass::FindClassName((LPTSTR)m_sClassID);
    if (pTagObjC)
      {
      CString S(pTagObjC->DrwGroup());
      S+=":";
      Sym=S+Sym;
      }
    }
  return Sym;
  }

//---------------------------------------------------------------------------
#define UseOldCode 0

bool CNeutralGrfImportExport::DoImportDB(eScdNDBOptions Opts, CDocTemplate & Template, LPCTSTR pDatabaseName)
  {
  m_Opts=Opts;
  CWaitMsgCursor Wait("Import graphics data");
  if (m_Opts & eNDB_Graphics)
    {
    if (Open(pDatabaseName, false))
      {
      //determine list of required graphics pages...
      CString SQL;
      Strng Page;
      CArray <Strng, Strng&> GrfPages;
      if (m_pGI)
        {
        try
          {
          ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
          SQL = "SELECT DISTINCT Page FROM GroupInserts";
          pRS->Open(_variant_t(SQL), _variant_t((IDispatch*)m_DB.GetConnectionPtr()), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
          if (!pRS->adEOF)
            pRS->MoveFirst();
          while (!pRS->adEOF)
            {
            COleVariant Val;
            Val=pRS->Fields->GetItem("Page")->Value;
            if (Val.vt==VT_NULL)
              {
              Page="";
              LogError("GraphicslNDB", 0, "Insert Page not specified!");
              }
            else
              {
              _bstr_t pg(Val.bstrVal);
              Page = (LPTSTR)(LPCTSTR)pg;
              Page.FnCheckExtension(".scg");
              GrfPages.Add(Page);
              }

            pRS->MoveNext();
            }
          pRS->Close();
          }
        catch(_com_error & e)
          {
          LogError("GraphicslNDB", LF_Exclamation, "Cannot open and read query\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
          Close();
          return false;
          }
        }

      try
        {
        ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
        CString SQL = "SELECT DISTINCT Page FROM GraphicsUnits";
        pRS->Open(_variant_t(SQL), _variant_t((IDispatch*)m_DB.GetConnectionPtr()), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
        if (!pRS->adEOF)
          pRS->MoveFirst();
        while (!pRS->adEOF)
          {
          COleVariant Val;
          Val=pRS->Fields->GetItem("Page")->Value;
          if (Val.vt==VT_NULL)
            {
            Page="";
            LogError("GraphicslNDB", 0, "Page not specified!");
            }
          else
            {
            _bstr_t pg(Val.bstrVal);
            Page = (LPTSTR)(LPCTSTR)pg;
            Page.FnCheckExtension(".scg");
            GrfPages.Add(Page);
            }

          pRS->MoveNext();
          }
        pRS->Close();
        }
      catch(_com_error & e)
        {
        LogError("GraphicslNDB", LF_Exclamation, "Cannot open and read query\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
        Close();
        return false;
        }
      //todo should check links table to determine if there are any other pages?!?!


      //create required graphics pages, or delete contents of specified graphics pages...
      CArray <CGrfDoc*, CGrfDoc*> GrfDocs;
      GrfDocs.SetSize(GrfPages.GetSize());
      for (int i=0; i<GrfDocs.GetSize(); i++)
        {
        CGrfDoc* pGDoc = (CGrfDoc*)(CWindowLists::GetGrfWndByName(GrfPages[i]()));
        GrfDocs[i] = pGDoc;
        if (pGDoc)
          pGDoc->GCB.DeleteDrawing();
        else
          {
          Close();
          return false;
          }
        }


      //create graphics...
      // (a)Templates
      if (m_pGI)
        {
        SQL = "SELECT * FROM GroupInserts ORDER BY Page";
        m_pGI->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
        ADODB::_RecordsetPtr ndrs = m_pGI->Recordset();
        if (!ndrs->adEOF)
          ndrs->MoveFirst();
        Strng PrevPage;
        CGrfDoc* pGDoc = NULL;
        while (!ndrs->adEOF)
          {
          m_pGI->Read();
          //LogNote(m_pGI->m_sPage, 0, m_pGI->m_sTag);
          if (pGDoc==NULL || _stricmp(m_pGI->m_sInsertPg, PrevPage())!=0)
            {
            PrevPage = m_pGI->m_sInsertPg;
            Page = m_pGI->m_sInsertPg;
            Page.FnCheckExtension(".scg");
            pGDoc = (CGrfDoc*)(CWindowLists::GetGrfWndByName(Page()));
            }
          if (pGDoc)
            {
            //todo check if parms are legal!!!
            CNeutralGrfImportExport GImport;
            GImport.SetGroup(m_pGI->m_sGroup);
            GImport.SetImportOrigin(m_pGI->m_Insert, m_pGI->m_Scale);
            GImport.SetImportTagFixups(m_pGI->m_iFixupRule, m_pGI->m_sFixupString1, m_pGI->m_sFixupString2, NULL, NULL);
            Strng LibNm;
            LibNm.FnExpand(m_pGI->m_sLibrary);
            if (!GImport.DoImportGroup(eScdNDBOptions(eNDB_Sequence|eNDB_Graphics), pGDoc, LibNm()))
              {
              LogError("GraphicslNDB", 0, "Graphics Template not Imported %s(%s)", m_pGI->m_sLibrary, m_pGI->m_sGroup);
              };
            }

          ndrs->MoveNext();
          }
        }

      // Unit Symbols
      OpenSymbolImport();

      //(b) Units...
      SQL = "SELECT * FROM GraphicsUnits ORDER BY Page";
      m_pUn->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
      ADODB::_RecordsetPtr ndrs = m_pUn->Recordset();
      if (!ndrs->adEOF)
        ndrs->MoveFirst();
      Strng PrevPage;
      CGrfDoc* pGDoc = NULL;
      while (!ndrs->adEOF)
        {
        m_pUn->Read();
        //LogNote(m_pUn->m_sPage, 0, m_pUn->m_sTag);
        if (pGDoc==NULL || _stricmp(m_pUn->m_sPage, PrevPage())!=0)
          {
          PrevPage = m_pUn->m_sPage;
          Page = m_pUn->m_sPage;
          Page.FnCheckExtension(".scg");
          pGDoc = (CGrfDoc*)(CWindowLists::GetGrfWndByName(Page()));
          }
        if (pGDoc)
          {
          //todo check if parms are legal!!!
          CInsertBlk CB; 
          CB.ATag=m_pUn->m_sTag;
          CB.AClass=m_pUn->m_sClassID;
          CB.Pt.World.X=m_pUn->m_Insert.m_X;
          CB.Pt.World.Y=m_pUn->m_Insert.m_Y;
          CB.Pt.World.Z=m_pUn->m_Insert.m_Z;
          CB.NdScl.X=m_pUn->m_Scale.m_X;
          CB.NdScl.Y=m_pUn->m_Scale.m_Y;
          CB.NdScl.Z=m_pUn->m_Scale.m_Z;
          CB.Rotate=m_pUn->m_Rotation;

          // Unit Symbols
          CB.ASymbol=FixSymbol(m_pUn->m_sSymbol, m_pUn->m_sClassID);
          ImportSymbol(CB.ASymbol(), false);
          Attr_Settings &ASet=pGDoc->GCB.Tag_Attr_Set;
          Attr_Settings SetMem=ASet;
          
          if (!Valid(m_pUn->m_TagPt.m_X))
            {
            m_pUn->m_TagPt.m_X=m_pUn->m_Insert.m_X;
            m_pUn->m_TagPt.m_Y=m_pUn->m_Insert.m_Y;
            m_pUn->m_TagPt.m_Z=m_pUn->m_Insert.m_Z;
            }
          pGDoc->GCB.Tag_InsertPt.X=m_pUn->m_TagPt.m_X;
          pGDoc->GCB.Tag_InsertPt.Y=m_pUn->m_TagPt.m_Y;
          pGDoc->GCB.Tag_InsertPt.Z=m_pUn->m_TagPt.m_Z;
          pGDoc->GCB.HideTag=!m_pUn->m_TagVisible;
          ASet.XScl=m_pUn->m_TagScale.m_X;
          ASet.Rot=m_pUn->m_TagRotation;

          pGDoc->GCB.DoInsertNodeGrf(&CB, true);
          ASet=SetMem;
          }

        ndrs->MoveNext();
        }
      
      // Unit Symbols
      CloseSymbolImport();

      //(c) links...
      /*SQL = "SELECT GraphicsLinks.Page, GraphicsLinks.Tag, GraphicsLinks.ClassID, GraphicsLinkLines.SequenceNo, GraphicsLinkLines.x, GraphicsLinkLines.y, GraphicsLinkLines.z ";
      SQL += "FROM GraphicsLinks INNER JOIN GraphicsLinkLines ON (GraphicsLinks.Page = GraphicsLinkLines.Page) AND (GraphicsLinks.Tag = GraphicsLinkLines.Tag) ";
      SQL += "ORDER BY GraphicsLinks.Page, GraphicsLinks.Tag, GraphicsLinkLines.SequenceNo";*/
      SQL = "SELECT * FROM GraphicsLinkLines ORDER BY Page, Tag, SequenceNo";
      m_pLl->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
      ADODB::_RecordsetPtr llrs = m_pLl->Recordset();
      if (!llrs->adEOF)
        llrs->MoveFirst();
      Strng PrevTag;
      pGDoc = NULL;
#if UseOldCode
      CConnectBlk CB;
      while (!llrs->adEOF)
        {
        m_pLl->Read();
        if (pGDoc==NULL || _stricmp(m_pLl->m_sPage, PrevPage())!=0 || _stricmp(m_pLl->m_sTag, PrevTag())!=0)
          {
          if (pGDoc!=NULL)
            {
            pGDoc->GCB.DoInsertLinkGrf(&CB, true);
            }
          if (pGDoc==NULL || _stricmp(m_pLl->m_sPage, PrevPage())!=0)
            {
            PrevPage = m_pLl->m_sPage;
            Page = m_pLl->m_sPage;
            Page.FnCheckExtension(".scg");
            pGDoc = (CGrfDoc*)(CWindowLists::GetGrfWndByName(Page()));
            }
          PrevTag = m_pLl->m_sTag;
          CB.iLineMethod=0;
          CB.ATag = m_pLl->m_sTag;
          CB.NVerts = 0;
          }

        CB.Verts[CB.NVerts].World.X = m_pLl->m_Vertex.m_X;
        CB.Verts[CB.NVerts].World.Y = m_pLl->m_Vertex.m_Y;
        CB.Verts[CB.NVerts].World.Z = m_pLl->m_Vertex.m_Z;
        CB.NVerts++;

        llrs->MoveNext();
        }
      if (pGDoc!=NULL)
        {
        //todo check if parms are legal!!!
        pGDoc->GCB.DoInsertLinkGrf(&CB, true);
        }
#else
      int PrevSeqNo;
      Pt_3f Pt;
      CLineDrawHelper LDH;
      LDH.SetLineMethod(LDM_Direct); //todo user option
      LDH.SetArrowRule(ARM_EndDirnAndLast); //todo user option
      LDH.SetArrowScale(2.5); //todo user option
      LDH.SetShowTag(false); //todo user option
      while (!llrs->adEOF)
        {
        m_pLl->Read();
        //LogNote(m_pLl->m_sPage, 0, m_pLl->m_sTag);
        if (pGDoc==NULL || _stricmp(m_pLl->m_sPage, PrevPage())!=0 || _stricmp(m_pLl->m_sTag, PrevTag())!=0)
          {
          if (pGDoc!=NULL)
            {
            //todo check if parms are legal!!!
            pGDoc->GCB.DoInsertLinkGrf(LDH, NULL, NULL, true);
            }
          if (pGDoc==NULL || _stricmp(m_pLl->m_sPage, PrevPage())!=0)
            {
            PrevPage = m_pLl->m_sPage;
            Page = m_pLl->m_sPage;
            Page.FnCheckExtension(".scg");
            pGDoc = (CGrfDoc*)(CWindowLists::GetGrfWndByName(Page()));
            }
          PrevTag = m_pLl->m_sTag;
          PrevSeqNo = m_pLl->m_lSeqNo;
          LDH.InitPoints();

          CString LSQL;
          LSQL.Format("SELECT * FROM GraphicsLinks WHERE [Tag] = '%s'", m_pLl->m_sTag);
          m_pLk->OpenQuery(NULL, LSQL, 0/*dbReadOnly*/);
          ADODB::_RecordsetPtr llks = m_pLk->Recordset();
          if (!llks->adEOF)
            llks->MoveFirst();
          if (!llks->adEOF)
            {
            m_pLk->Read();
            if (!Valid(m_pLk->m_TagPt.m_X))
              {
              m_pLk->m_TagPt.m_X=m_pLk->m_Insert.m_X;
              m_pLk->m_TagPt.m_Y=m_pLk->m_Insert.m_Y;
              m_pLk->m_TagPt.m_Z=m_pLk->m_Insert.m_Z;
              }

            LDH.SetShowTag(m_pLk->m_TagVisible!=0);
            LDH.SetTag(m_pLk->m_sTag, m_pLk->m_TagPt.m_X, m_pLk->m_TagPt.m_Y, m_pLk->m_TagPt.m_Z);
            LDH.SetTagProps(m_pLk->m_TagScale.m_X, m_pLk->m_TagRotation);
            }
          else
            {
            LDH.SetShowTag(FALSE);
            LDH.SetTag(m_pLl->m_sTag);
            }
          }

        Pt.Set(m_pLl->m_Vertex.m_X, m_pLl->m_Vertex.m_Y, m_pLl->m_Vertex.m_Z);
        LDH.AddPoint(Pt, m_pLl->m_lSeqNo-PrevSeqNo>1 ? LPT_Break : LPT_Line);
        PrevSeqNo = m_pLl->m_lSeqNo;
        llrs->MoveNext();
        }
      if (pGDoc!=NULL)
        {
        //todo check if parms are legal!!!
        pGDoc->GCB.DoInsertLinkGrf(LDH, NULL, NULL, true);
        }
#endif

      for (i=0; i<GrfDocs.GetSize(); i++)
        {
        CGrfDoc* pGDoc = GrfDocs[i];
        if (pGDoc)
          {
          pGDoc->GCB.pDsp->Open();
          pGDoc->GCB.pDsp->SetZoom(Zoom_All);
          pGDoc->GCB.pDrw->EntityInvalidate();
          pGDoc->GCB.pWnd->Invalidate(True);
          pGDoc->GCB.pDsp->Close();
          }
        }

      Close();
      return true;

      }
    return false;
    }
  return true;
  };

//---------------------------------------------------------------------------

bool CNeutralGrfImportExport::DoImportGroup(eScdNDBOptions Opts, CGrfDoc * pDoc, LPCTSTR pDatabaseName)
  {
  m_Opts=Opts;
  CWaitMsgCursor Wait("Import graphics data");
  if (m_Opts & eNDB_Graphics)
    {
    if (Open(pDatabaseName, false))
      {
      //determine list of required graphics pages...

      //create graphics...
      //(a) Units...

      m_Move.m_X=0;
      m_Move.m_Y=0;
      m_Move.m_Z=0;

      CString SQL;

      try
        { 
        SQL.Format("SELECT * FROM [Groups] WHERE [Group] = '%s'", m_sGroup);

        m_pGr->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
        ADODB::_RecordsetPtr ndrs = m_pGr->Recordset();
        if (!ndrs->adEOF)
          ndrs->MoveFirst();
        Strng PrevPage;
        while (!ndrs->adEOF)
          {
          m_pGr->Read();
          m_Move.m_X=m_Origin.m_X;//-m_pGr->xm_Origin.m_X;
          m_Move.m_Y=m_Origin.m_Y;//-m_pGr->xm_Origin.m_Y;
          m_Move.m_Z=m_Origin.m_Z;//-m_pGr->xm_Origin.m_Z;
          ndrs->MoveNext();
          }
        ASSERT(ndrs->adEOF);
        }
      catch(_com_error & e)
        {
        LogError("GraphicslNDB", LF_Exclamation, "Cannot open and read query while reading Groups\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
        Close();
        return 0;
        }

      OpenSymbolImport();

      // Insert Symbols
      try
        { 
        SQL.Format("SELECT * FROM [GraphicsInserts] WHERE [Group] = '%s'", m_sGroup);

        m_pIn->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
        ADODB::_RecordsetPtr ndrs = m_pIn->Recordset();
        if (!ndrs->adEOF)
          ndrs->MoveFirst();
        Strng PrevPage;
        while (!ndrs->adEOF)
          {
          m_pIn->Read();
          //LogNote(m_pIn->m_sPage, 0, m_pIn->m_sTag);
          if (pDoc)
            {
            //todo check if parms are legal!!!
            CInsertBlk CB; 
            CB.ATag=AdjustTag(m_pIn->m_sTag);
            CB.AClass="Inserts";//m_pIn->m_sClassID;
            CB.Pt.World.X=m_pIn->m_Insert.m_X*m_Scale.m_X+m_Move.m_X;
            CB.Pt.World.Y=m_pIn->m_Insert.m_Y*m_Scale.m_Y+m_Move.m_Y;
            CB.Pt.World.Z=m_pIn->m_Insert.m_Z*m_Scale.m_Z+m_Move.m_Z;
            CB.GrpScl.X=m_pIn->m_Scale.m_X*m_Scale.m_X;
            CB.GrpScl.Y=m_pIn->m_Scale.m_Y*m_Scale.m_Y;
            CB.GrpScl.Z=m_pIn->m_Scale.m_Z*m_Scale.m_Z;
            CB.Rotate=m_pIn->m_Rotation;
            //CB.ATagBase=MInfo.DefTag();

            // Unit Symbols
            CB.ASymbol=FixSymbol(m_pIn->m_sSymbol, CB.AClass());
            ImportSymbol(CB.ASymbol(), false);

            Attr_Settings &rASet=pDoc->GCB.Tag_Attr_Set;
            Attr_Settings ASet=rASet;
            

            if (!Valid(m_pIn->m_TagPt.m_X))
              {
              m_pIn->m_TagPt.m_X=CB.Pt.World.X;
              m_pIn->m_TagPt.m_Y=CB.Pt.World.Y;
              m_pIn->m_TagPt.m_Z=CB.Pt.World.Z;
              }

            pDoc->GCB.Tag_InsertPt.X=m_pIn->m_TagPt.m_X;
            pDoc->GCB.Tag_InsertPt.Y=m_pIn->m_TagPt.m_Y;
            pDoc->GCB.Tag_InsertPt.Z=m_pIn->m_TagPt.m_Z;
            pDoc->GCB.HideTag=!m_pIn->m_TagVisible;
            rASet.XScl=m_pIn->m_TagScale.m_X;
            rASet.Rot=m_pIn->m_TagRotation;
  
            ImportSymbol(CB.ASymbol(), false);
            pDoc->GCB.DoInsertNodeGrf(&CB, true);
            rASet=ASet;
            }

          ndrs->MoveNext();
          }
        }
      catch(_com_error & e)
        {
        LogError("GraphicslNDB", LF_Exclamation, "Cannot open and read query while reading GraphicsUnits\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
        Close();
        return 0;
        }

      // Unit Symbols
      try
        { 
        SQL.Format("SELECT * FROM [GraphicsUnits] WHERE [Group] = '%s'", m_sGroup);

        m_pUn->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
        ADODB::_RecordsetPtr ndrs = m_pUn->Recordset();
        if (!ndrs->adEOF)
          ndrs->MoveFirst();
        Strng PrevPage;
        while (!ndrs->adEOF)
          {
          m_pUn->Read();
          //LogNote(m_pUn->m_sPage, 0, m_pUn->m_sTag);
          if (pDoc)
            {
            //todo check if parms are legal!!!
            CInsertBlk CB; 
            CB.ATag=AdjustTag(m_pUn->m_sTag);
            CB.AClass=m_pUn->m_sClassID;
            CB.Pt.World.X=m_pUn->m_Insert.m_X*m_Scale.m_X+m_Move.m_X;
            CB.Pt.World.Y=m_pUn->m_Insert.m_Y*m_Scale.m_Y+m_Move.m_Y;
            CB.Pt.World.Z=m_pUn->m_Insert.m_Z*m_Scale.m_Z+m_Move.m_Z;
            CB.NdScl.X=m_pUn->m_Scale.m_X*m_Scale.m_X;
            CB.NdScl.Y=m_pUn->m_Scale.m_Y*m_Scale.m_Y;
            CB.NdScl.Z=m_pUn->m_Scale.m_Z*m_Scale.m_Z;
            CB.Rotate=m_pUn->m_Rotation;

            // Unit Symbols

  
            // Unit Symbols
            CB.ASymbol=FixSymbol(m_pUn->m_sSymbol, m_pUn->m_sClassID);
            ImportSymbol(CB.ASymbol(), false);

            Attr_Settings &ASet=pDoc->GCB.Tag_Attr_Set;
            Attr_Settings SetMem=ASet;
            
            if (!Valid(m_pUn->m_TagPt.m_X))
              {
              m_pUn->m_TagPt.m_X=CB.Pt.World.X;
              m_pUn->m_TagPt.m_Y=CB.Pt.World.Y;
              m_pUn->m_TagPt.m_Z=CB.Pt.World.Z;
              }

            pDoc->GCB.Tag_InsertPt.X=m_pUn->m_TagPt.m_X;
            pDoc->GCB.Tag_InsertPt.Y=m_pUn->m_TagPt.m_Y;
            pDoc->GCB.Tag_InsertPt.Z=m_pUn->m_TagPt.m_Z;
            pDoc->GCB.HideTag=!m_pUn->m_TagVisible;
            ASet.XScl=m_pUn->m_TagScale.m_X;
            ASet.Rot=m_pUn->m_TagRotation;

            pDoc->GCB.DoInsertNodeGrf(&CB, true);

            ASet=SetMem;
            }

          ndrs->MoveNext();
          }
        }
      catch(_com_error & e)
        {
        LogError("GraphicslNDB", LF_Exclamation, "Cannot open and read query while reading GraphicsUnits\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
        Close();
        return 0;
        }

      // Unit Symbols
      CloseSymbolImport();

      //(b) links...
      //SQL = "SELECT * FROM GraphicsLinkLines ORDER BY Page, Tag, SequenceNo";
      try
        { 
        SQL.Format("SELECT * FROM [GraphicsLinkLines] WHERE [Group] = '%s' ORDER BY [Tag], [SequenceNo]", m_sGroup);

        m_pLl->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
        ADODB::_RecordsetPtr llrs = m_pLl->Recordset();
        if (!llrs->adEOF)
          llrs->MoveFirst();
        Strng PrevTag;
#if UseOldCode
        CConnectBlk CB;
        CB.iLineMethod=0;
        CB.ATag = "";//m_pLl->m_sTag;
        CB.NVerts = 0;
        while (!llrs->adEOF)
          {
          m_pLl->Read();
          if (PrevTag.GetLength()>0 && _stricmp(m_pLl->m_sTag, PrevTag())!=0)
            {
            pDoc->GCB.DoInsertLinkGrf(&CB, true);
            CB.iLineMethod=0;
            CB.NVerts = 0;
            }

          PrevTag = m_pLl->m_sTag;
          if (CB.NVerts==0)
            CB.ATag = AdjustTag(m_pLl->m_sTag);
          CB.Verts[CB.NVerts].World.X = m_pLl->m_Vertex.m_X*m_Scale.m_X+m_Move.m_X;
          CB.Verts[CB.NVerts].World.Y = m_pLl->m_Vertex.m_Y*m_Scale.m_Y+m_Move.m_Y;
          CB.Verts[CB.NVerts].World.Z = m_pLl->m_Vertex.m_Z*m_Scale.m_Z+m_Move.m_Z;
          CB.NVerts++;

          llrs->MoveNext();
          }
        pDoc->GCB.DoInsertLinkGrf(&CB, true);
#else
        int PrevSeqNo;
        Pt_3f Pt;
        CLineDrawHelper LDH;
        LDH.SetLineMethod(LDM_Direct); //todo user option
        LDH.SetArrowRule(ARM_EndDirnAndLast); //todo user option
        LDH.SetArrowScale(2.5*(m_Scale.m_X+m_Scale.m_Y+m_Scale.m_Z)/3); //todo user option
        LDH.SetShowTag(false); //todo user option
        while (!llrs->adEOF)
          {
          m_pLl->Read();
          if (PrevTag.GetLength()>0 && _stricmp(m_pLl->m_sTag, PrevTag())!=0)
            {
            pDoc->GCB.DoInsertLinkGrf(LDH, NULL, NULL, true);
            PrevTag="";
            }
          if (PrevTag.GetLength()==0)
            {
            PrevSeqNo = m_pLl->m_lSeqNo;
            LDH.InitPoints();
            PrevTag = m_pLl->m_sTag;
            CString Tg = AdjustTag(m_pLl->m_sTag);
            //LDH.SetTag((char*)(LPCSTR)Tg);

            CString LSQL;                                                           
            LSQL.Format("SELECT * FROM [GraphicsLinks] WHERE [Tag] = '%s' AND [Group] = '%s'", m_pLl->m_sTag, m_sGroup);
            m_pLk->OpenQuery(NULL, LSQL, 0/*dbReadOnly*/);
            ADODB::_RecordsetPtr llks = m_pLk->Recordset();
            if (!llks->adEOF)
              llks->MoveFirst();
            if (!llks->adEOF)
              {
              m_pLk->Read();

              if (!Valid(m_pLk->m_TagPt.m_X))
                {
                m_pLk->m_TagPt.m_X=m_pLk->m_Insert.m_X;
                m_pLk->m_TagPt.m_Y=m_pLk->m_Insert.m_Y;
                m_pLk->m_TagPt.m_Z=m_pLk->m_Insert.m_Z;
                }

              LDH.SetShowTag(m_pLk->m_TagVisible!=0);
              LDH.SetTag(Tg, m_pLk->m_TagPt.m_X, m_pLk->m_TagPt.m_Y, m_pLk->m_TagPt.m_Z);
              LDH.SetTagProps(m_pLk->m_TagScale.m_X, m_pLk->m_TagRotation);
              }
            else
              {
              LDH.SetShowTag(FALSE);
              LDH.SetTag(Tg);
              }
            }

          Pt.Set(m_pLl->m_Vertex.m_X*m_Scale.m_X+m_Move.m_X, 
                 m_pLl->m_Vertex.m_Y*m_Scale.m_Y+m_Move.m_Y, 
                 m_pLl->m_Vertex.m_Z*m_Scale.m_Z+m_Move.m_Z);
          LDH.AddPoint(Pt, m_pLl->m_lSeqNo-PrevSeqNo>1 ? LPT_Break : LPT_Line);
          PrevSeqNo = m_pLl->m_lSeqNo;
          llrs->MoveNext();
          }
        pDoc->GCB.DoInsertLinkGrf(LDH, NULL, NULL, true);
#endif

        Close();
        }
      catch(_com_error & e)
        {
        LogError("GraphicslNDB", LF_Exclamation, "Cannot open and read query while reading GraphicsLinks\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
        Close();
        return 0;
        }
      Close();
      return true;
      }
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------
