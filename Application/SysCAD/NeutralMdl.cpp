#include "stdafx.h"
#include ".\neutralmdl.h"
#include ".\dbhelper.h"
#include "gpfuncs.h"
#include "gpwfuncs.h"
#include "sfe_clnt.h"
#include "project.h"
//#include "optoff.h"

//===========================================================================

BOOL CNeutralMdlCommonUnits::sm_bDoneInit = 0;
CNeutralMdlCommonUnits::CNeutralMdlCommonUnits(KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  //m_bWithGroup=WithGroup;
  sTableName = "CommonUnits";
  //if (!sm_bDoneInit)
    Init();
  }

//-------------------------------------------------------------------------

void CNeutralMdlCommonUnits::Init()
  {
  if (m_pDB)
    {
    DefineClass();
    CreateStr("UnitOperation", m_sUnitOperation, sizeof(m_sUnitOperation)-1, FF_Required);
    CreateStr("ModelClass", m_sModelClass, sizeof(m_sModelClass)-1, FF_None);
    CreateIndex("UnitOperation", false, false);
    sm_bDoneInit = 1;
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

BOOL CNeutralMdlCommonNames::sm_bDoneInit = 0;
CNeutralMdlCommonNames::CNeutralMdlCommonNames(KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  //m_bWithGroup=WithGroup;
  sTableName = "CommonNames";
  //if (!sm_bDoneInit)
    Init();
  }

//-------------------------------------------------------------------------

void CNeutralMdlCommonNames::Init()
  {
  if (m_pDB)
    {
    DefineClass();
    CreateStr("CommonName", m_sCommonName, sizeof(m_sCommonName)-1, FF_Required);
    CreateStr("ValueTag", m_sValueTag, sizeof(m_sValueTag)-1, FF_None);
    CreateIndex("CommonName", false, false);
    sm_bDoneInit = 1;
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

BOOL CNeutralMdlCommonIO::sm_bDoneInit = 0;
CNeutralMdlCommonIO::CNeutralMdlCommonIO(KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  //m_bWithGroup=WithGroup;
  sTableName = "CommonIONames";
  //if (!sm_bDoneInit)
    Init();
  }

//-------------------------------------------------------------------------

void CNeutralMdlCommonIO::Init()
  {
  if (m_pDB)
    {
    DefineClass();
    CreateStr("CommonIOName", m_sCommonIOName, sizeof(m_sCommonIOName)-1, FF_Required);
    CreateStr("UnitIOTag", m_sUnitIOTag, sizeof(m_sUnitIOTag)-1, FF_None);
    CreateIndex("CommonIOName", false, false);
    sm_bDoneInit = 1;
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

BOOL CNeutralMdlSequence::sm_bDoneInit = 0;
CNeutralMdlSequence::CNeutralMdlSequence(bool WithGroup, KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  m_bWithGroup=WithGroup;
  sTableName = "ModelSequence";
  //if (!sm_bDoneInit)
    Init();
  }

//-------------------------------------------------------------------------

void CNeutralMdlSequence::Init()
  {
  if (m_pDB)
    {
    DefineClass();
    if (m_bWithGroup)
      CreateStr("Group", m_sGroup, sizeof(m_sGroup)-1, FF_None);
    else
      strcpy(m_sGroup, "");
    CreateVal("SequenceNo", &m_lSeqNo, ADOX::adInteger, FF_Required);
    CreateVal("IsLnk", &m_bIsLnk, ADOX::adBoolean, FF_None);
    CreateStr("Tag", m_sTag, sizeof(m_sTag)-1, FF_None);
    CreateIndex("SequenceNo", false, false);
    sm_bDoneInit = 1;
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

BOOL CNeutralMdlUnits::sm_bDoneInit = 0;
CNeutralMdlUnits::CNeutralMdlUnits(bool WithGroup, KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  m_bWithGroup=WithGroup;
  sTableName = "ModelUnits";
  //if (!sm_bDoneInit)
    Init();
  }

//-------------------------------------------------------------------------

void CNeutralMdlUnits::Init()
  {
  if (m_pDB)
    {
    DefineClass();
    if (m_bWithGroup)
      CreateStr("Group", m_sGroup, sizeof(m_sGroup)-1, FF_None);
    else
      strcpy(m_sGroup, "");
    CreateStr("Tag", m_sTag, sizeof(m_sTag)-1, FF_Required);
    //CreateVal("SequenceNo", &m_lSeqNo, ADOX::adInteger, FF_None);
    CreateStr("ClassID", m_sClassID, sizeof(m_sClassID)-1, FF_None);
    CreateStr("SubClassID", m_sSubClassID, sizeof(m_sSubClassID)-1, FF_None);
    CreateStr("PrimaryCfg", m_sPrimaryCfg, sizeof(m_sPrimaryCfg)-1, FF_None);
    CreateStr("EqpDesc", m_sEqpDesc, sizeof(m_sEqpDesc)-1, FF_None);
    CreateStr("EqpMemo", m_sEqpMemo, sizeof(m_sEqpMemo)-1, FF_None);
    CreateStr("EqpLocation", m_sEqpLocation, sizeof(m_sEqpLocation)-1, FF_None);
    if (!m_bWithGroup)
      {
      CreateStr("EqpIdStr", m_sEqpIdStr, sizeof(m_sEqpIdStr)-1, FF_None);
      CreateStr("EqpGUID", m_sEqpGUID, sizeof(m_sEqpGUID)-1, FF_None);
      }
    else
      {
      strcpy(m_sEqpIdStr, "");
      strcpy(m_sEqpGUID, "");
      }
    CreateIndex("Tag", false, false);
    sm_bDoneInit = 1;
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

BOOL CNeutralMdlLinks::sm_bDoneInit = 0;
CNeutralMdlLinks::CNeutralMdlLinks(bool WithGroup, KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  m_bWithGroup=WithGroup;
  sTableName = "ModelLinks";
  //if (!sm_bDoneInit)
    Init();
  }

//-------------------------------------------------------------------------

void CNeutralMdlLinks::Init()
  {
  if (m_pDB)
    {
    DefineClass();
    if (m_bWithGroup)
      CreateStr("Group", m_sGroup, sizeof(m_sGroup)-1, FF_None);
    else
      strcpy(m_sGroup, "");
    CreateStr("Tag", m_sTag, sizeof(m_sTag)-1, FF_Required);
    CreateStr("ClassID", m_sClassID, sizeof(m_sClassID)-1, FF_None);
    CreateStr("SubClassID", m_sSubClassID, sizeof(m_sSubClassID)-1, FF_None);
    CreateStr("PrimaryCfg", m_sPrimaryCfg, sizeof(m_sPrimaryCfg)-1, FF_None);
    CreateStr("EqpDesc", m_sEqpDesc, sizeof(m_sEqpDesc)-1, FF_None);
    CreateStr("EqpMemo", m_sEqpMemo, sizeof(m_sEqpMemo)-1, FF_None);
    CreateStr("EqpLocation", m_sEqpLocation, sizeof(m_sEqpLocation)-1, FF_None);
    if (!m_bWithGroup)
      {
      CreateStr("EqpIdStr", m_sEqpIdStr, sizeof(m_sEqpIdStr)-1, FF_None);
      CreateStr("EqpGUID", m_sEqpGUID, sizeof(m_sEqpGUID)-1, FF_None);
      }    
    else
      {
      strcpy(m_sEqpIdStr, "");
      strcpy(m_sEqpGUID, "");
      }
    CreateStr("SrcTag", m_sSrcTag, sizeof(m_sSrcTag)-1, FF_Required);
    CreateStr("SrcPort", m_sSrcIO, sizeof(m_sSrcIO)-1, FF_Required);
    CreateStr("SrcTermStrip", m_sSrcTS, sizeof(m_sSrcTS)-1, FF_None);
    //CreateVal("SrcIOIndex", &m_lSrcIOInx, ADOX::adInteger, FF_Required);
    CreateStr("DstTag", m_sDstTag, sizeof(m_sDstTag)-1, FF_Required);
    CreateStr("DstPort", m_sDstIO, sizeof(m_sDstIO)-1, FF_Required);
    CreateStr("DstTermStrip", m_sDstTS, sizeof(m_sDstTS)-1, FF_None);
    //CreateVal("DstIOIndex", &m_lDstIOInx, ADOX::adInteger, FF_Required);
    CreateIndex("Tag", false, false);
    sm_bDoneInit = 1;
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

BOOL CNeutralMdlConfig::sm_bDoneInit = 0;
CNeutralMdlConfig::CNeutralMdlConfig(bool WithGroup, KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  m_bWithGroup=WithGroup;
  m_lSeqNo=0;
  sTableName = "ModelConfiguration";
  //if (!sm_bDoneInit)
    Init();
  }

//-------------------------------------------------------------------------

void CNeutralMdlConfig::Init()
  {
  if (m_pDB)
    {
    DefineClass();
    if (m_bWithGroup)
      CreateStr("Group", m_sGroup, sizeof(m_sGroup)-1, FF_None);
    else
      strcpy(m_sGroup, "");
    CreateStr("NdTag", m_sNdTag, sizeof(m_sNdTag)-1, FF_Required);
    CreateVal("SequenceNo", &m_lSeqNo, ADOX::adInteger, FF_Required);
    CreateStr("FieldType",  m_sFldType, sizeof(m_sFldType)-1, FF_None);
    CreateStr("FieldTag", m_sFldTag, sizeof(m_sFldTag)-1, FF_None);

    //CreateVal("IsDbl", &m_bIsDbl, ADOX::adBoolean, FF_None);
    //CreateVal("dValue", &m_dValue, ADOX::adDouble, FF_None);
    //CreateStr("sValue", m_sValue, sizeof(m_sValue)-1, FF_None);
    CreateStr("Value", m_sValue, sizeof(m_sValue)-1, FF_None);
    CreateStr("CnvUnits", m_sCnvStr, sizeof(m_sCnvStr)-1, FF_None);
    CreateIndex("NdTag", false, false);
    sm_bDoneInit = 1;
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

BOOL CNeutralMdlWires::sm_bDoneInit = 0;
CNeutralMdlWires::CNeutralMdlWires(bool WithGroup, KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  m_bWithGroup=WithGroup;
  sTableName = "ModelWires";
  //if (!sm_bDoneInit)
    Init();
  }

//-------------------------------------------------------------------------

void CNeutralMdlWires::Init()
  {
  if (m_pDB)
    {
    DefineClass();
    if (m_bWithGroup)
      CreateStr("Group", m_sGroup, sizeof(m_sGroup)-1, FF_None);
    else
      strcpy(m_sGroup, "");
    CreateStr("CableTag", m_sCableTag, sizeof(m_sCableTag)-1, FF_Required);
    CreateVal("WireNo", &m_lWireNo, ADOX::adInteger, FF_None);

    CreateStr("WireTag", m_sWireTag, sizeof(m_sWireTag)-1, FF_None);
    CreateStr("SrcNdTag", m_sSrcNdTag, sizeof(m_sSrcNdTag)-1, FF_None);
    CreateStr("SrcTrmStrip", m_sSrcTrmStrip, sizeof(m_sSrcTrmStrip)-1, FF_None);
    CreateStr("SrcTerminal", m_sSrcTerminal, sizeof(m_sSrcTerminal)-1, FF_None);
    CreateStr("DstNdTag", m_sDstNdTag, sizeof(m_sDstNdTag)-1, FF_None);
    CreateStr("DstTrmStrip", m_sDstTrmStrip, sizeof(m_sDstTrmStrip)-1, FF_None);
    CreateStr("DstTerminal", m_sDstTerminal, sizeof(m_sDstTerminal)-1, FF_None);

    CreateIndex("CableTag", false, false);
    sm_bDoneInit = 1;
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

BOOL CNeutralErrors::sm_bDoneInit = 0;
CNeutralErrors::CNeutralErrors(bool WithGroup, KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  m_bWithGroup=WithGroup;
  sTableName = "ErrorList";
  //if (!sm_bDoneInit)
    Init();
  }

//-------------------------------------------------------------------------

void CNeutralErrors::Init()
  {
  if (m_pDB)
    {
    DefineClass();
    if (m_bWithGroup)
      CreateStr("Group", m_sGroup, sizeof(m_sGroup)-1, FF_None);
    else
      strcpy(m_sGroup, "");
    CreateStr("Source", m_sSource, sizeof(m_sSource)-1, FF_Required);
    CreateStr("Description", m_sDesc, sizeof(m_sDesc)-1, FF_None);
    //CreateVal("SequenceNo", &m_lSeqNo, ADOX::adInteger, FF_None);
    CreateIndex("Source", false, false);
    sm_bDoneInit = 1;
    }
  }

//===========================================================================

CNeutralMdlImportExport::CNeutralMdlImportExport()
  {
  m_pCU = NULL;
  m_pCN = NULL;
  m_pCI = NULL;
  m_pGI = NULL;
  m_pSq = NULL;
  m_pUn = NULL;
  m_pLk = NULL;
  m_pCf = NULL;
  m_pWr = NULL;
  m_pEr = NULL;
  //m_pOldTags=NULL;
  //m_pNewTags=NULL;
  m_bWithGroups=false;
  //m_iFixupRule=eFixup_NoChange;
  }

//---------------------------------------------------------------------------

CNeutralMdlImportExport::~CNeutralMdlImportExport()
  {
  //ASSERT_RDB(fUnits==NULL && fLinks==NULL && fLinkLines==NULL, "Should close file!");
  ASSERT_RDB(m_pCU==NULL && m_pCN==NULL && m_pCI==NULL && m_pUn==NULL && 
             m_pGI==NULL && m_pSq==NULL && m_pLk==NULL && m_pCf==NULL && m_pWr==NULL && m_pEr==NULL, "Should close file!");
  }

//---------------------------------------------------------------------------

void CNeutralMdlImportExport::SetGroup(LPCTSTR Group)
  {
  m_bWithGroups=true;
  m_sGroup=Group;
  };

//---------------------------------------------------------------------------

void CNeutralMdlImportExport::SetGroupDesc(LPCTSTR GroupDesc)
  {
  m_sGroupDesc=GroupDesc;
  };

//---------------------------------------------------------------------------

//void CNeutralMdlImportExport::SetExportTagFixups(long FixupRule, LPCTSTR Prefix, LPCTSTR Suffix, CStringArray *OldTags, CStringArray *NewTags)
//  {
//  m_iFixupRule = FixupRule;
//  m_sPrefix    = Prefix;
//  m_sSuffix    = Suffix;
//  m_pOldTags   = OldTags;
//  m_pNewTags   = NewTags;
//  };

//---------------------------------------------------------------------------

void CNeutralMdlImportExport::SetExportTagList(CStringArray *OldTags, CStringArray *NewTags)
  {
  m_iFixupRule=eFixup_ExchangeTags;
  m_pOldTags=OldTags;
  m_pNewTags=NewTags;
  };

//---------------------------------------------------------------------------

void CNeutralMdlImportExport::SetImportFilter(LPCTSTR Group)
  {
  SetGroup(Group);
  };

//---------------------------------------------------------------------------

void CNeutralMdlImportExport::SetImportTagFixups(long FixupRule, LPCTSTR String1, LPCTSTR String2, CStringArray *OldTags, CStringArray *NewTags)
  {
  ASSERT(FixupRule!=eFixup_ExchangeTags || (OldTags && NewTags));
  m_iFixupRule = FixupRule;
  m_sString1   = String1;
  m_sString2   = String2;
  m_pOldTags   = OldTags;
  m_pNewTags   = NewTags;
  };

//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::Open(LPCTSTR pDatabaseName, bool OpenForExport)
  {
  Strng sDBName=pDatabaseName;
  //sDBName = pFolder;
  //sDBName += "\\";
  //sDBName += pDatabaseName;

  WIN32_FIND_DATA fd;
  if (FileExists(sDBName(), fd))
    {
    if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      {
      LogError("ModelNDB", 0/*LF_Exclamation*/, "Cannot open read-only Database '%s'", sDBName());
      return false;
      }
    if (!m_DB.OpenDB(DBConnect_Default, sDBName(), false, false))
      {
      LogError("ModelNDB", 0/*LF_Exclamation*/, "Cannot open Database '%s'", sDBName());
      return false;
      }
    }
  else 
    {
    long UtilsDBFmt = ScdPFUser.RdInt("General", "UtilsDBFormat", DBConnect_Default);
    if (OpenForExport && !m_DB.CreateDB(UtilsDBFmt, sDBName(), 0))
      {
      LogError("ModelNDB", LF_Exclamation, "Cannot create Database '%s'", sDBName());
      return false;
      }
    else
      {
      LogError("ModelNDB", LF_Exclamation, "Cannot open Database '%s'", sDBName());
      return false;
      }
    }

  //open tables...
  if (OpenForExport  && (m_Opts & eNDB_CommonTables))
    {
    m_pCU = new CNeutralMdlCommonUnits(&m_DB);
    m_pCU->SetIgnoreWriteErr3022(true);
    m_pCN = new CNeutralMdlCommonNames(&m_DB);
    m_pCN->SetIgnoreWriteErr3022(true);
    m_pCI = new CNeutralMdlCommonIO(&m_DB);
    m_pCI->SetIgnoreWriteErr3022(true);
    }
  if (m_Opts & eNDB_GroupInserts)
    {
    if (OpenForExport)
      {
      if (m_Opts & eNDB_GroupInserts)
        {
        m_pGI = new CNeutralGroupInserts(&m_DB);
        m_pGI->SetIgnoreWriteErr3022(true);
        }
      }
    else if (m_DB.IsExistentClass("GroupInserts"))
      {
      m_pGI = new CNeutralGroupInserts(&m_DB);
      m_pGI->SetIgnoreWriteErr3022(true);
      }
    }
  
  if (OpenForExport || m_DB.IsExistentClass("ModelSequence"))
    {
    m_pSq = new CNeutralMdlSequence(m_bWithGroups, &m_DB);
    m_pSq->SetIgnoreWriteErr3022(true);
    }
  m_pUn = new CNeutralMdlUnits(m_bWithGroups, &m_DB);
  m_pUn->SetIgnoreWriteErr3022(true);
  m_pLk = new CNeutralMdlLinks(m_bWithGroups, &m_DB);
  m_pLk->SetIgnoreWriteErr3022(true);
  m_pCf = new CNeutralMdlConfig(m_bWithGroups, &m_DB);
  m_pCf->SetIgnoreWriteErr3022(true);
  m_pWr = new CNeutralMdlWires(m_bWithGroups, &m_DB);
  m_pWr->SetIgnoreWriteErr3022(true);
  m_pEr = new CNeutralErrors(m_bWithGroups, &m_DB);
  m_pEr->SetIgnoreWriteErr3022(true);

  try
    {
    if (OpenForExport)
      {
      if (m_Opts & eNDB_CommonTables)
        {
        m_pCU->Open(NULL/*, dbDenyWrite*/);
        m_pCN->Open(NULL/*, dbDenyWrite*/);
        m_pCI->Open(NULL/*, dbDenyWrite*/);
        }
      if ((m_Opts & eNDB_GroupInserts) && m_pGI)
        m_pGI->Open(NULL/*, dbDenyWrite*/);

      m_pSq->Open(NULL/*, dbDenyWrite*/);
      m_pUn->Open(NULL/*, dbDenyWrite*/);
      m_pLk->Open(NULL/*, dbDenyWrite*/);
      m_pCf->Open(NULL/*, dbDenyWrite*/);
      m_pWr->Open(NULL/*, dbDenyWrite*/);
      m_pEr->Open(NULL/*, dbDenyWrite*/);
      }
    }
  catch(_com_error & e)
    {
    if (1)//e->m_pErrorInfo)
      LogError("ModelNDB", 0/*LF_Exclamation*/, "Cannot open table\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
    else
      LogError("ModelNDB", 0/*LF_Exclamation*/, "Cannot open table");
    Close();
    return false;
    }

  //delete contents...
  try
    {
    if (OpenForExport)
      {
      if (m_Opts & eNDB_CommonTables)
        {
        m_pCU->DeleteAll();
        m_pCN->DeleteAll();
        m_pCI->DeleteAll();
        }
      //if ((m_Opts & eNDB_GroupInserts) && m_pGI)
      //  m_pGI->DeleteAll();
      if ((m_Opts & eNDB_GroupInserts) && m_pGI)
        m_pGI->DeleteAll();

      if (m_bWithGroups)
        {
        CFileRec * Tables[] = { m_pSq, m_pUn, m_pLk, m_pCf, m_pWr, NULL};
        for (int i=0; Tables[i]; i++)
          {
          CString Select;
          Select.Format("SELECT * FROM [%s] WHERE [Group] = '%s'", Tables[i]->TableName(), m_sGroup);
          Tables[i]->DeleteAll(Select);
          }
        }
      else
        {
        m_pSq->DeleteAll();
        m_pUn->DeleteAll();
        m_pLk->DeleteAll();
        m_pCf->DeleteAll();
        m_pWr->DeleteAll();
        }
      }
    m_pEr->DeleteAll();
    }
  catch(_com_error & e)
    {
    if (1)//e->m_pErrorInfo)
      LogError("ModelNDB", 0/*LF_Exclamation*/, "Cannot delete table contents\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
    else
      LogError("ModelNDB", 0/*LF_Exclamation*/, "Cannot delete table contents");
    Close();
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

void CNeutralMdlImportExport::Close()
  {
  if (m_pCU)
    {
    m_pCU->Close();
    delete m_pCU;
    m_pCU= NULL;
    }
  if (m_pCN)
    {
    m_pCN->Close();
    delete m_pCN;
    m_pCN= NULL;
    }
  if (m_pCI)
    {
    m_pCI->Close();
    delete m_pCI;
    m_pCI= NULL;
    }

  if (m_pGI)
    {
    m_pGI->Close();
    delete m_pGI;
    m_pGI= NULL;
    }
  if (m_pSq)
    {
    m_pSq->Close();
    delete m_pSq;
    m_pSq= NULL;
    }
  if (m_pUn)
    {
    m_pUn->Close();
    delete m_pUn;
    m_pUn= NULL;
    }
  if (m_pLk)
    {
    m_pLk->Close();
    delete m_pLk;
    m_pLk = NULL;
    }
  if (m_pCf)
    {
    m_pCf->Close();
    delete m_pCf;
    m_pCf = NULL;
    }
  if (m_pWr)
    {
    m_pWr->Close();
    delete m_pWr;
    m_pWr= NULL;
    }
  if (m_pEr)
    {
    m_pEr->Close();
    delete m_pEr;
    m_pEr= NULL;
    }

  m_DB.CloseDB();
  }

//---------------------------------------------------------------------------

void CNeutralMdlImportExport::WriteSequence(LPCTSTR Group, CNodeListItem & Item)
  {
  //assume string lengths are long enough!
  strcpy(m_pSq->m_sGroup, Group?Group:"");
  m_pSq->m_lSeqNo=Item.m_lSeqNo;
  m_pSq->m_bIsLnk=Item.m_bIsLnk;
  strcpy(m_pSq->m_sTag, AdjustTag(Item.m_sTag));
  m_pSq->Write();

  dbgpln("Write Seq %4i %i %s > %s", Item.m_lSeqNo, Item.m_bIsLnk, Item.m_sTag, AdjustTag(Item.m_sTag));
  }

//---------------------------------------------------------------------------

void CNeutralMdlImportExport::WriteUnit(LPCTSTR Group, CNodeListItem & Item)
  {
  //assume string lengths are long enough!
  strcpy(m_pUn->m_sGroup, Group?Group:"");
  strcpy(m_pUn->m_sTag, AdjustTag(Item.m_sTag));
  strcpy(m_pUn->m_sClassID, Item.m_sClass);
  strcpy(m_pUn->m_sSubClassID, Item.m_sSubClass.GetLength()?Item.m_sSubClass:"");
  strcpy(m_pUn->m_sPrimaryCfg, Item.m_sPrimaryCfg.GetLength()?Item.m_sPrimaryCfg:"");
  strcpy(m_pUn->m_sEqpDesc, Item.m_sEqpDesc.GetLength()?Item.m_sEqpDesc:"");
  strcpy(m_pUn->m_sEqpMemo, Item.m_sEqpMemo.GetLength()?Item.m_sEqpMemo:"");
  strcpy(m_pUn->m_sEqpLocation, Item.m_sEqpLocation.GetLength()?Item.m_sEqpLocation:"");
  if (!m_bWithGroups)
    {
    strcpy(m_pUn->m_sEqpIdStr, /*!m_bWithGroup && */Item.m_sEqpIdStr.GetLength()?Item.m_sEqpIdStr:"");
    strcpy(m_pUn->m_sEqpGUID, /*!m_bWithGroup &&*/ Item.m_sEqpGUID.GetLength()?Item.m_sEqpGUID:"");
    }
  else
    {
    strcpy(m_pUn->m_sEqpIdStr, "");
    strcpy(m_pUn->m_sEqpGUID, "");
    }
  //m_pUn->m_lSeqNo=Item.m_lSeqNo;
  m_pUn->Write();
  }

//---------------------------------------------------------------------------

void CNeutralMdlImportExport::WriteLink(LPCTSTR Group, CNodeListItem & Item)
  {
  //assume string lengths are long enough!
  strcpy(m_pLk->m_sGroup, Group?Group:"");
  strcpy(m_pLk->m_sTag, AdjustTag(Item.m_sTag));
  strcpy(m_pLk->m_sClassID, Item.m_sClass);
  strcpy(m_pLk->m_sSubClassID, Item.m_sSubClass?Item.m_sSubClass:"");
  strcpy(m_pLk->m_sPrimaryCfg, Item.m_sPrimaryCfg?Item.m_sPrimaryCfg:"");
  strcpy(m_pLk->m_sEqpDesc, Item.m_sEqpDesc.GetLength()?Item.m_sEqpDesc:"");
  strcpy(m_pLk->m_sEqpMemo, Item.m_sEqpMemo.GetLength()?Item.m_sEqpMemo:"");
  strcpy(m_pLk->m_sEqpLocation, Item.m_sEqpLocation.GetLength()?Item.m_sEqpLocation:"");
  if (!m_bWithGroups)
    {
    strcpy(m_pLk->m_sEqpIdStr, /*!m_bWithGroup && */Item.m_sEqpIdStr.GetLength()?Item.m_sEqpIdStr:"");
    strcpy(m_pLk->m_sEqpGUID, /*!m_bWithGroup &&*/ Item.m_sEqpGUID.GetLength()?Item.m_sEqpGUID:"");
    }
  else
    {
    strcpy(m_pLk->m_sEqpIdStr, "");
    strcpy(m_pLk->m_sEqpGUID, "");
    }

  strcpy(m_pLk->m_sSrcTag, Item.m_sSrcNd.GetLength()?AdjustTag(Item.m_sSrcNd):"");
  strcpy(m_pLk->m_sDstTag, Item.m_sDstNd.GetLength()?AdjustTag(Item.m_sDstNd):"");
  strcpy(m_pLk->m_sSrcIO,  Item.m_sSrcIO.GetLength()?Item.m_sSrcIO:"");
  strcpy(m_pLk->m_sDstIO,  Item.m_sDstIO.GetLength()?Item.m_sDstIO:"");
  strcpy(m_pLk->m_sSrcTS,  "");//Item.m_sSrcTS.GetLength()?Item.m_sSrcTS:"");
  strcpy(m_pLk->m_sDstTS,  "");//Item.m_sDstTS.GetLength()?Item.m_sDstTS:"");
  //m_pLk->m_lSrcIOInx = Item.m_lSrcIOInx;
  //m_pLk->m_lDstIOInx = Item.m_lDstIOInx;
  m_pLk->Write();
  }

//---------------------------------------------------------------------------

void CNeutralMdlImportExport::WriteConfig(LPCTSTR Group, long SeqNo, CNodeConfigItem & Item)
  {
  //assume string lengths are long enough!
  strcpy(m_pCf->m_sGroup, Group?Group:"");
  m_pCf->m_lSeqNo=SeqNo;
  strcpy(m_pCf->m_sNdTag, Item.m_sNdTag.GetLength()?AdjustTag(Item.m_sNdTag):"");
  strcpy(m_pCf->m_sFldType, tt_TypeString(Item.m_ttType));
  CString FTag=Item.m_sFldTag.GetLength()?AdjustTagWild(Item.m_sFldTag):"";
  strcpy(m_pCf->m_sFldTag, FTag);
  if (FTag.Compare(Item.m_sFldTag)!=0)
    strcat(m_pCf->m_sFldType, "*");

  //m_pCf->m_bIsDbl=Item.m_bIsDbl;
  //m_pCf->m_dValue=Item.m_dValue;
  strcpy(m_pCf->m_sCnvStr, Item.m_sCnvStr.GetLength()?Item.m_sCnvStr:"");
  strcpy(m_pCf->m_sValue, Item.m_sValue.GetLength()?Item.m_sValue:"");
  m_pCf->Write();
  }

//---------------------------------------------------------------------------

void CNeutralMdlImportExport::WriteWires(LPCTSTR Group, CLinkWiring & LW)
  {
  //assume string lengths are long enough!
  strcpy(m_pWr->m_sGroup, Group?Group:"");
  strcpy(m_pWr->m_sCableTag, AdjustTag(LW.m_LinkTag));
  strcpy(m_pWr->m_sWireTag, "");
  strcpy(m_pWr->m_sSrcNdTag, AdjustTag(LW.m_SrcNdTag));
  strcpy(m_pWr->m_sDstNdTag, AdjustTag(LW.m_DstNdTag));
  for (int w=0; w<LW.WireCount(); w++)
    {
    bool GotSrc=false;
    for (int w0=0; w0<LW.m_SrcTerminals.GetCount(); w0++)
      {
      if (LW.m_SrcTerminals[w0].m_iWire==w)
        {
        strcpy(m_pWr->m_sSrcTrmStrip, LW.m_SrcTerminals[w0].m_sTermStripName);
        strcpy(m_pWr->m_sSrcTerminal, LW.m_SrcTerminals[w0].m_sTerminalName);
        GotSrc=true;
        break;
        }
      }
    bool GotDst=false;
    for (int w1=0; w1<LW.m_DstTerminals.GetCount(); w1++)
      {
      if (LW.m_DstTerminals[w1].m_iWire==w)
        {
        strcpy(m_pWr->m_sDstTrmStrip, LW.m_DstTerminals[w1].m_sTermStripName);
        strcpy(m_pWr->m_sDstTerminal, LW.m_DstTerminals[w1].m_sTerminalName);
        GotDst=true;
        break;
        }
      }
    if (GotSrc && GotDst)
      {
      m_pWr->m_lWireNo=w;
      m_pWr->Write();
      }
    }
  }

//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::DoExportValidate(eScdNDBOptions Opts, CString &ErrMsg)
  {
  bool OK=true;
  CWaitMsgCursor Wait("Validate Model Export");


  m_Opts=Opts;
  if (m_Opts & eNDB_Models)
    {
    CNodeList List;

    long iRet=gs_pPrj->pFlwLib->FE_GetNodeList(List);
  
    ASSERT(m_pOldTags);
    POSITION Pos=List.GetHeadPosition();
    while (Pos && OK)
      {
      CNodeListItem &I=List.GetNext(Pos);
      if (FindOldTag(I.m_sTag)<0)
        continue;

      if (I.m_bIsLnk)
        {
        if (FindOldTag(I.m_sSrcNd)<0 || FindOldTag(I.m_sDstNd)<0)
          {
          ErrMsg.Format("Link %s excluded - Src and/or Dst not included", I.m_sTag);
          OK=false;
          }
        }
      if (m_Opts & eNDB_Configuration)
        {
        }
      }
    }
  return OK;
  }

//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::DoImportValidate(eScdNDBOptions Opts, CString &ErrMsg)
  {
  bool OK=true;
  CWaitMsgCursor Wait("Validate Model Import");


  m_Opts=Opts;
  if (m_Opts & eNDB_Models)
    {
    CNodeList List;

    long iRet=gs_pPrj->pFlwLib->FE_GetNodeList(List);
  
    ASSERT(m_pNewTags);
    POSITION Pos=List.GetHeadPosition();
    while (Pos && OK)
      {
      CNodeListItem &I=List.GetNext(Pos);
      if (FindNewTag(I.m_sTag)<0)
        continue;
      ErrMsg.Format("Duplicate Tag %s", I.m_sTag);
      OK=false;
      }
    }
  return OK;
  }

//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::DoExport(eScdNDBOptions Opts, LPCTSTR pDatabaseName)
  {
  if (0)//!gs_pPrj->AllMdlLoaded())
    {
    LogError("ModelNDB", 0/*LF_Exclamation*/, "All graphics pages are not open!");
    return false;
    }

  CWaitMsgCursor Wait("Export Model data");

  m_Opts=Opts;
  if (m_Opts & eNDB_Models)
    {
    if (Open(pDatabaseName, true))
      {
      if (m_Opts & eNDB_CommonTables)
        {
        //?????????
        }

      if ((m_Opts & eNDB_BlankTables)==0)
        {
        CNodeList List;

        long iRet=gs_pPrj->pFlwLib->FE_GetNodeList(List);

        ASSERT(m_pOldTags);
        POSITION Pos=List.GetHeadPosition();
        while (Pos)
          {
          CNodeListItem &I=List.GetNext(Pos);
          if (FindOldTag(I.m_sTag)<0)
            continue;

          WriteSequence(m_sGroup, I);
          if (I.m_bIsLnk)
            {
            WriteLink(m_sGroup, I);

            CLinkWiring LW(I.m_sTag);
            int nWires=gs_pPrj->pFlwLib->FE_GetLinkWiring(LW);
            if (nWires>0)
              WriteWires(m_sGroup, LW);
            }
          else
            WriteUnit(m_sGroup, I);
          if (m_Opts & eNDB_Configuration)
            {
            long SeqNo=0;
            CNodeConfigList CfgList;
            long iRet=gs_pPrj->pFlwLib->FE_GetNodeConfiguration((Opts & eNDB_Parameters)!=0, (Opts & eNDB_State)!=0, I.m_sTag, CfgList);
            POSITION CfgPos=CfgList.GetHeadPosition();
            while (CfgPos)
              {
              CNodeConfigItem &CfgI=CfgList.GetNext(CfgPos);
              WriteConfig(m_sGroup, SeqNo++, CfgI);
              }
            }
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

bool CNeutralMdlImportExport::ImportGroupInserts()
  {
  if (!m_pGI)
    return false;

  // (a)Templates
  CString SQL = "SELECT * FROM GroupInserts ORDER BY Page";
  m_pGI->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
  ADODB::_RecordsetPtr ndrs = m_pGI->Recordset();
  if (!ndrs->adEOF)
    ndrs->MoveFirst();
  Strng PrevPage;
  while (!ndrs->adEOF)
    {
    m_pGI->Read();
    //LogNote(m_pGI->sPage, 0, m_pGI->sTag);
    //todo check if parms are legal!!!
    CNeutralMdlImportExport MImport;
    MImport.SetGroup(m_pGI->m_sGroup);
    //GImport.SetImportOrigin(m_pGI->m_InsertX, m_pGI->m_InsertY, m_pGI->m_InsertZ);
    MImport.SetImportTagFixups(m_pGI->m_iFixupRule, m_pGI->m_sFixupString1, m_pGI->m_sFixupString2, NULL, NULL);
    if (!MImport.DoImportGroup(eScdNDBOptions(eNDB_Sequence|eNDB_Models|eNDB_Configuration|eNDB_Parameters|eNDB_State), m_pGI->m_sLibrary))
      {
      LogError("ModelNDB", 0, "Model Template not Imported %s(%s)", m_pGI->m_sLibrary, m_pGI->m_sGroup);
      };

    ndrs->MoveNext();
    }
  return true;
  }

//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::ImportBySequence()
  {
  if (!m_pSq)
    return false;

  try
    {
    //determine SequenceNo of required Nodes ...
    CString SQL;
    if (m_bWithGroups)    
      SQL.Format("SELECT * FROM ModelSequence WHERE [Group] = '%s' ORDER BY [SequenceNo]", m_sGroup);
    else
      SQL = "SELECT * FROM ModelSequence ORDER BY [SequenceNo]";

    m_pSq->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
    ADODB::_RecordsetPtr rs = m_pSq->Recordset();
    if (rs==NULL)
      return false;
    if (!rs->adEOF)
      rs->MoveFirst();
    if (rs->adEOF)
      return false;
    while (!rs->adEOF)
      {
      m_pSq->Read();
      if (m_pSq->m_bIsLnk)
        ImportLinks(m_pSq->m_sTag);
      else
        ImportUnits(m_pSq->m_sTag);
      rs->MoveNext();
      }
    }
  catch(_com_error & e)
    {
    LogError("ModelNDB", 0/*LF_Exclamation*/, "Import Sequence Failed\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
    return false;
    }
  return true;
  }

//inline operator LPTSTR(CString &S) { return (LPTSTR)(LPCTSTR)S; }
//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::ImportUnits(LPCTSTR Which)
  {
  dbgpln("Import Unit   %s", Which?Which:"ALL");
  if (!m_pUn)
    return false;

  try
    {
    //determine Sequence of required Nodes ...
    CString SQL, Where;
    if (m_bWithGroups && Which)
      SQL.Format("SELECT * FROM ModelUnits WHERE [Group] = '%s' AND [Tag] = '%s'", m_sGroup, Which);
    else if (m_bWithGroups)
      SQL.Format("SELECT * FROM ModelUnits WHERE [Group] = '%s'", m_sGroup);
    else if (Which)
      SQL.Format("SELECT * FROM ModelUnits WHERE [Tag] = '%s'", Which);
    else
      SQL.Format("SELECT * FROM ModelUnits");

    SQL+=" ORDER BY [Tag]";

    m_pUn->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
    ADODB::_RecordsetPtr rs = m_pUn->Recordset();
    if (rs==NULL)
      return false;
    if (!rs->adEOF)
      rs->MoveFirst();
    while (!rs->adEOF)
      {
      m_pUn->Read();
      CString Tg=AdjustTag(m_pUn->m_sTag);
      //dbgpln("  Insert Unit %s", Tg);
      
      long iRet=gs_pPrj->pFlwLib->FE_DoInsert(m_pUn->m_sClassID, m_pUn->m_sSubClassID, m_pUn->m_sPrimaryCfg, (LPTSTR)(LPCTSTR)Tg, NULL, NULL);
      if (iRet!=0)
        LogError("ModelNDB", 0/*LF_Exclamation*/, "Unit Insert %s Failed %s", Tg, NErr_String(iRet));
      else
        {
        iRet=gs_pPrj->pFlwLib->FE_SetCommon((LPTSTR)(LPCTSTR)Tg, m_pUn->m_sEqpDesc, m_pUn->m_sEqpMemo, m_pUn->m_sEqpIdStr, m_pUn->m_sEqpLocation, m_pUn->m_sEqpGUID);
        if (iRet!=0)
          LogError("ModelNDB", 0/*LF_Exclamation*/, "Unit SetCommon %s Failed %s", Tg, NErr_String(iRet));
        }
      rs->MoveNext();
      }
    }
  catch(_com_error & e)
    {
    LogError("ModelNDB", 0/*LF_Exclamation*/, "Import Unit Failed\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::ImportLinks(LPCTSTR Which)
  {
  dbgpln("Import Link   %s", Which?Which:"ALL");
  if (!m_pLk)
    return false;

  try
    {
    //determine Sequence of required Nodes ...
    CString SQL;
    if (m_bWithGroups && Which)
      SQL.Format("SELECT * FROM ModelLinks WHERE [Group] = '%s' AND [Tag] = '%s'", m_sGroup, Which);
    else if (m_bWithGroups)
      SQL.Format("SELECT * FROM ModelLinks WHERE [Group] = '%s'", m_sGroup);
    else if (Which)
      SQL.Format("SELECT * FROM ModelLinks WHERE [Tag] = '%s'", Which);
    else
      SQL.Format("SELECT * FROM ModelLinks");

    SQL+=" ORDER BY [Tag]";

    m_pLk->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
    ADODB::_RecordsetPtr rs = m_pLk->Recordset();
    if (rs==NULL)
      return false;
    if (!rs->adEOF)
      rs->MoveFirst();
    while (!rs->adEOF)
      {
      m_pLk->Read();
      //dbgpln("  Insert Link %-30s %s %s %s %s", m_pLk->m_sTag, m_pLk->m_sEqpDesc, m_pLk->m_sEqpMemo, m_pLk->m_sEqpIdStr, m_pLk->m_sEqpLocation, m_pLk->m_sEqpGUID);
      CString LTg=AdjustTag(m_pLk->m_sTag);

      CString Src(AdjustTag(m_pLk->m_sSrcTag));
      Src+=".";
      Src+=m_pLk->m_sSrcIO;
      CString Dst(AdjustTag(m_pLk->m_sDstTag));
      Dst+=".";
      Dst+=m_pLk->m_sDstIO;

      //FlwNode *pNd;
      long iRet=gs_pPrj->pFlwLib->FE_DoInsert(m_pLk->m_sClassID, m_pLk->m_sSubClassID, m_pLk->m_sPrimaryCfg, (LPTSTR)(LPCTSTR)LTg, (LPTSTR)(LPCTSTR)Src, (LPTSTR)(LPCTSTR)Dst);
      if (iRet!=0)
        LogError("ModelNDB", 0/*LF_Exclamation*/, "Link Insert %s Failed %s", m_pUn->m_sTag, NErr_String(iRet));
      else
        {
        iRet=gs_pPrj->pFlwLib->FE_SetCommon((LPTSTR)(LPCTSTR)LTg, m_pLk->m_sEqpDesc, m_pLk->m_sEqpMemo, m_pLk->m_sEqpIdStr, m_pLk->m_sEqpLocation, m_pLk->m_sEqpGUID);
        if (iRet!=0)
          LogError("ModelNDB", 0/*LF_Exclamation*/, "Link SetCommon %s Failed %s", LTg, NErr_String(iRet));
        ASSERT(m_pLk);
        if (strlen(m_pLk->m_sSrcTS)>0 && strlen(m_pLk->m_sDstTS)>0)
          {
          CLinkWiring LW;
          LW.m_bAutoWire=true;
          LW.m_LinkTag=m_pLk->m_sTag;
          LW.m_SrcTS=m_pLk->m_sSrcTS;
          LW.m_DstTS=m_pLk->m_sDstTS;

          if (gs_pPrj->pFlwLib->FE_SetLinkWiring(LW)<0)
            LogError("ModelNDB", 0/*LF_Exclamation*/, "Link AutoWire \n%s:%s -> %s", LW.m_LinkTag, LW.m_SrcTS, LW.m_DstTS);
          }
        }
      rs->MoveNext();
      }
    }
  catch(_com_error & e)
    {
    LogError("ModelNDB", 0/*LF_Exclamation*/, "Import Link Failed\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::ImportWires(LPCTSTR Which)
  {
  dbgpln("Import Wires  %s", Which?Which:"ALL");
  if (!m_pWr)
    return false;

  try
    {
    //determine Sequence of required Nodes ...
    CString SQL;
    //SQL.Format(Which ? "SELECT * FROM ModelWires WHERE [NdTag] = '%s'":"SELECT * FROM ModelWires", Which);
    //SQL="SELECT * FROM ModelWires";
    //if (Which)
    //  {
    //  SQL+=" WHERE [NdTag] = '";
    //  SQL+=Which;
    //  SQL+="'";
    //  }

    if (m_bWithGroups && Which)
      SQL.Format("SELECT * FROM ModelWires WHERE [Group] = '%s' AND [NdTag] = '%s'", m_sGroup, Which);
    else if (m_bWithGroups)
      SQL.Format("SELECT * FROM ModelWires WHERE [Group] = '%s'", m_sGroup);
    else if (Which)
      SQL.Format("SELECT * FROM ModelWires WHERE [NdTag] = '%s'", Which);
    else
      SQL.Format("SELECT * FROM ModelWires");

    SQL+=" ORDER BY [CableTag],[WireNo]";


    m_pWr->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
    ADODB::_RecordsetPtr rs = m_pWr->Recordset();
    if (rs==NULL)
      return false;
    if (!rs->adEOF)
      {
      rs->MoveFirst();
      m_pWr->Read();
      }
    bool NewCable=true;
    CLinkWiring LW;
    while (!rs->adEOF)
      {
      //dbgpln("  Insert Wire %s", m_pWr->m_sCableTag);

      if (NewCable)
        {
        LW.m_LinkTag=AdjustTag(m_pWr->m_sCableTag);
        LW.m_SrcNdTag=AdjustTag(m_pWr->m_sSrcNdTag);
        LW.m_DstNdTag=AdjustTag(m_pWr->m_sDstNdTag);
        LW.m_SrcTerminals.SetSize(0);
        LW.m_DstTerminals.SetSize(0);
        }

      CLinkWiring::CTerm ST(m_pWr->m_lWireNo, m_pWr->m_sSrcTrmStrip, m_pWr->m_sSrcTerminal);
      CLinkWiring::CTerm DT(m_pWr->m_lWireNo, m_pWr->m_sDstTrmStrip, m_pWr->m_sDstTerminal);

      LW.m_SrcTerminals.Add(ST);
      LW.m_DstTerminals.Add(DT);

      CString PrevCable=m_pWr->m_sCableTag;
      rs->MoveNext();
      NewCable=true;
      if (!rs->adEOF)
        {
        m_pWr->Read();
        NewCable=(PrevCable!=m_pWr->m_sCableTag);
        }
      if (NewCable)
        int nWires=gs_pPrj->pFlwLib->FE_SetLinkWiring(LW);
      }
    }
  catch(_com_error & e)
    {
    LogError("ModelNDB", 0/*LF_Exclamation*/, "Import Wires Failed\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::ImportConfigs(LPCTSTR Which)
  {
  dbgpln("Import Cfg    %s", Which?Which:"ALL");
  if (!m_pCf)
    return false;

  try
    {
    gs_pTheSFELib->FE_SetHoldGlobalLinks(true);

    //determine Sequence of required Nodes ...
    CString SQL;
//    SQL.Format(Which ? "SELECT * FROM ModelConfiguration WHERE [NdTag] = '%s'":"SELECT * FROM ModelConfiguration", Which);

    if (m_bWithGroups && Which)
      SQL.Format("SELECT * FROM ModelConfiguration WHERE [Group] = '%s' AND [NdTag] = '%s'", m_sGroup, Which);
    else if (m_bWithGroups)
      SQL.Format("SELECT * FROM ModelConfiguration WHERE [Group] = '%s'", m_sGroup);
    else if (Which)
      SQL.Format("SELECT * FROM ModelConfiguration WHERE [NdTag] = '%s'", Which);
    else
      SQL.Format("SELECT * FROM ModelConfiguration");
    
    SQL+=" ORDER BY [NdTag],[SequenceNo]";

    m_pCf->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
    ADODB::_RecordsetPtr rs = m_pCf->Recordset();
    if (rs==NULL)
      return false;
    if (!rs->adEOF)
      rs->MoveFirst();


    int Level=0;

    CXM_ObjectData ObjData(0, TABOpt_ForFile); // large objects are from filing point of view
    int ItemsInObject=0;
   
    //Strng TheTag,Value;
    Strng_List sStrList;

    Strng CurrentNdTag;
    Strng CurrentNdTagAdj;
    bool Done=rs->adEOF!=0;
    while (!Done)
      {
      Done = rs->adEOF!=0; // do it here 
      if (!Done)
        m_pCf->Read();             

      bool SimpleItem = !Done && (Level==0) && ((strlen(m_pCf->m_sFldType)==0) || strpbrk(m_pCf->m_sFldType, "{}[]<>()")==NULL);
      if (SimpleItem)
        {
        // single items;
        // just write it - assuming from access view 
        Strng FldTg, Cnv;
        TaggedObject::SplitTagCnv(m_pCf->m_sFldTag, FldTg, Cnv);
        if (strlen(m_pCf->m_sCnvStr)>0)
          Cnv=m_pCf->m_sCnvStr;
        Strng Tg;
        Tg.Set(/*Cnv.GetLength()>0 ? "%s.%s (%s)":*/"%s.%s", (LPCTSTR)m_pCf->m_sNdTag, FldTg()/*, Cnv()*/);
                                                   
        if (FldTg.XStrICmp("Regulator.Mode")==0)
          { int xxxx=0; }

        CXM_Route Route;
        Route.Clear();
        //Strng TagTag(m_pCf->m_sNdTag);
        //TagTag+=".Tag";
        CXM_ObjectTag  ObjTag(Tg(), TABOpt_ForView|TABOpt_AllInfoOnce); // Single Items 
        CXM_ObjectData RdData;
        bool Ok = (gs_Exec.XReadTaggedItem(NULL, ObjTag, RdData, Route)!=0);
        if (Ok)
          {
          CPkDataItem *pItem = RdData.FirstItem();
          int Type = pItem->Type();
          if (1)
            dbgpln("XRead %s %-7s %-25s %-8s %-30s %-20s %s", Ok?"OK":"  ", m_pCf->m_sFldType, m_pCf->m_sNdTag, m_pCf->m_sCnvStr, m_pCf->m_sFldTag, m_pCf->m_sValue, Cnv()?Cnv():"" );

          PkDataUnion DU;
          if (IsStrng(Type))
            DU.SetTypeString(Type, m_pCf->m_sValue);
          else if (IsFloatData(Type))
            DU.SetTypeDouble(Type, SafeAtoF(m_pCf->m_sValue), pItem->CnvIndex(), Cnv());
          else if (IsIntData(Type) && pItem->Contains(PDI_StrList))
            {
            pItem->GetStrList(sStrList);
            pStrng pS = sStrList.Find(m_pCf->m_sValue);
            const int Indx = (pS==NULL ? 0 : pS->Index());
            DU.SetTypeLong(Type, Indx);
            }
          else
            DU.SetTypeLong(Type, SafeAtoL(m_pCf->m_sValue));
     
          //PkDataUnion DU(pItem->Type(), m_pCf->m_sValue, pItem->CnvIndex(), Cnv());
          CXM_ObjectData LclObjData(0, TABOpt_ForView, Tg(), DU);

          Ok = (gs_Exec.XWriteTaggedItem(NULL, LclObjData, Route)!=TOData_NotFound);
          if (!Ok)
            LogError("ModelNDB", 0, "Write Failed %s = %s", Tg(), m_pCf->m_sValue);
          }
        else
          {
          LogWarning("ModelNDB", 0, "Tag %s Missing", Tg());
          }
        rs->MoveNext();
        Done = rs->adEOF!=0;
        }
      else // not simple - compound
        {
        if (0 && !Done)
          dbgpln("Read %-7s %-25s %-8s %s", m_pCf->m_sFldType, m_pCf->m_sNdTag, m_pCf->m_sCnvStr, m_pCf->m_sFldTag );
        if (Done || ((CurrentNdTag.Length()>0) && CurrentNdTag.XStrCmp(m_pCf->m_sNdTag)!=0))
          {
          if (1)
            dbgpln("CurrentConfig %s > %s", CurrentNdTag(), CurrentNdTagAdj());
          //write it 

          if (1)
            ObjData.List.dbgDump(true, "ImportCfg");

          CXM_Route Route;
          Route.Clear();
          Strng TagTag(CurrentNdTagAdj());
          TagTag+=".Tag";
          CXM_ObjectTag  ObjTag(TagTag(), TABOpt_ForFile);
          CXM_ObjectData ObjDataTag;
          bool Ok = (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjDataTag, Route)!=0);

          if (Ok)
            {
            Ok = (gs_Exec.XWriteTaggedItem(NULL, ObjData, Route)!=TOData_NotFound);
            if (!Ok)
              LogError("ModelNDB", 0, "Write Failed %s",CurrentNdTagAdj());
            }
          else
            {
            LogWarning("ModelNDB", 0, "Tag %s Missing",CurrentNdTagAdj());
            }

          CurrentNdTag="";
          }

        if (!Done)
          {
          if (CurrentNdTag.Length()==0)
            {
            CurrentNdTag=m_pCf->m_sNdTag;
            CurrentNdTagAdj=AdjustTagWild(m_pCf->m_sNdTag);
            ObjData.List.Clear();
            ItemsInObject=0;
            }

          LPTSTR pStar=strchr(m_pCf->m_sFldType, '*');
          if (pStar)
            *pStar=0;
          bool NdTagInTag=(pStar!=0);
          bool IsStructStart=strlen(m_pCf->m_sFldType)==1 && strpbrk(m_pCf->m_sFldType, "{[<(")!=NULL;
          bool IsStructEnd=strlen(m_pCf->m_sFldType)==1 && strpbrk(m_pCf->m_sFldType, "}]>)")!=NULL;

          CString FldTag;
          LPCTSTR pFldTag="";
          if (strlen(m_pCf->m_sFldTag)>0)
            {
            if (NdTagInTag) 
              FldTag=AdjustTagWild(m_pCf->m_sFldTag);
            else
              FldTag=m_pCf->m_sFldTag;
            pFldTag=FldTag;
            }

          if (IsStructStart)
            {
            Level++;
            ObjData.List.SetStructureS(tt_StringType(m_pCf->m_sFldType), (LPTSTR)pFldTag, m_pCf->m_sValue, "", "", "", NULL, NULL, 0);
            }
          else if (IsStructEnd)
            {
            ObjData.List.SetStructureE(tt_StringType(m_pCf->m_sFldType), (LPTSTR)pFldTag);
            Level--;
            }
          else
            {
            PkDataUnion DU(tt_Generic, m_pCf->m_sValue);
            ObjData.List.SetDataValue((LPTSTR)pFldTag, DU);
            }
          ItemsInObject++;
          }

        rs->MoveNext();
        }
      }

    gs_pTheSFELib->FE_SetHoldGlobalLinks(false);
    }
  catch(_com_error & e)
    {
    gs_pTheSFELib->FE_SetHoldGlobalLinks(false);

    LogError("ModelNDB", 0/*LF_Exclamation*/, "Import Configuration Failed\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::EnumerateTagsBySequence(CStringArray &Tags)
  {
  if (!m_pSq)
    return false;

  try
    {
    //determine SequenceNo of required Nodes ...
    CString SQL;
    if (m_bWithGroups)    
      SQL.Format("SELECT * FROM ModelSequence WHERE [Group] = '%s' ORDER BY [Tag]", m_sGroup);
    else
      SQL = "SELECT * FROM ModelSequence ORDER BY [Tag]";

    m_pSq->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
    ADODB::_RecordsetPtr rs = m_pSq->Recordset();
    if (rs==NULL)
      return false;
    if (!rs->adEOF)
      rs->MoveFirst();
    if (rs->adEOF)
      return false;
    while (!rs->adEOF)
      {
      m_pSq->Read();
      Tags.Add(m_pSq->m_sTag);
      rs->MoveNext();
      }
    }
  catch(_com_error & e)
    {
    LogError("ModelNDB", 0/*LF_Exclamation*/, "Enumerate Sequence Failed\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
    return false;
    }
  return true;
  }

//inline operator LPTSTR(CString &S) { return (LPTSTR)(LPCTSTR)S; }
//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::EnumerateUnitTags(CStringArray &Tags, LPCTSTR Which)
  {
  dbgpln("Enumerate Unit   %s", Which?Which:"ALL");
  if (!m_pUn)
    return false;

  try
    {
    //determine Sequence of required Nodes ...
    CString SQL, Where;
    if (m_bWithGroups && Which)
      SQL.Format("SELECT * FROM ModelUnits WHERE [Group] = '%s' AND [Tag] = '%s'", m_sGroup, Which);
    else if (m_bWithGroups)
      SQL.Format("SELECT * FROM ModelUnits WHERE [Group] = '%s'", m_sGroup);
    else if (Which)
      SQL.Format("SELECT * FROM ModelUnits WHERE [Tag] = '%s'", Which);
    else
      SQL.Format("SELECT * FROM ModelUnits");

    SQL+=" ORDER BY [Tag]";

    m_pUn->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
    ADODB::_RecordsetPtr rs = m_pUn->Recordset();
    if (rs==NULL)
      return false;
    if (!rs->adEOF)
      rs->MoveFirst();
    while (!rs->adEOF)
      {
      m_pUn->Read();
      Tags.Add(m_pUn->m_sTag);
      rs->MoveNext();
      }
    }
  catch(_com_error & e)
    {
    LogError("ModelNDB", 0/*LF_Exclamation*/, "Enumerate Unit Failed\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::EnumerateLinkTags(CStringArray &Tags, LPCTSTR Which)
  {
  dbgpln("Enumerate Link %s", Which?Which:"ALL");
  if (!m_pLk)
    return false;

  try
    {
    //determine Sequence of required Nodes ...
    CString SQL;
    if (m_bWithGroups && Which)
      SQL.Format("SELECT * FROM ModelLinks WHERE [Group] = '%s' AND [Tag] = '%s'", m_sGroup, Which);
    else if (m_bWithGroups)
      SQL.Format("SELECT * FROM ModelLinks WHERE [Group] = '%s'", m_sGroup);
    else if (Which)
      SQL.Format("SELECT * FROM ModelLinks WHERE [Tag] = '%s'", Which);
    else
      SQL.Format("SELECT * FROM ModelLinks");

    SQL+=" ORDER BY [Tag]";

    m_pLk->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
    ADODB::_RecordsetPtr rs = m_pLk->Recordset();
    if (rs==NULL)
      return false;
    if (!rs->adEOF)
      rs->MoveFirst();
    while (!rs->adEOF)
      {
      m_pLk->Read();
      //dbgpln("  Insert Link %-30s %s %s %s %s", m_pLk->m_sTag, m_pLk->m_sEqpDesc, m_pLk->m_sEqpMemo, m_pLk->m_sEqpIdStr, m_pLk->m_sEqpLocation, m_pLk->m_sEqpGUID);
      CString LTg=AdjustTag(m_pLk->m_sTag);

      CString Src(AdjustTag(m_pLk->m_sSrcTag));
      Src+=".";
      Src+=m_pLk->m_sSrcIO;
      CString Dst(AdjustTag(m_pLk->m_sDstTag));
      Dst+=".";
      Dst+=m_pLk->m_sDstIO;

      //FlwNode *pNd;
      long iRet=gs_pPrj->pFlwLib->FE_DoInsert(m_pLk->m_sClassID, m_pLk->m_sSubClassID, m_pLk->m_sPrimaryCfg, (LPTSTR)(LPCTSTR)LTg, (LPTSTR)(LPCTSTR)Src, (LPTSTR)(LPCTSTR)Dst);
      if (iRet!=0)
        LogError("ModelNDB", 0/*LF_Exclamation*/, "Link Insert %s Failed %s", m_pUn->m_sTag, NErr_String(iRet));
      else
        {
        iRet=gs_pPrj->pFlwLib->FE_SetCommon((LPTSTR)(LPCTSTR)LTg, m_pLk->m_sEqpDesc, m_pLk->m_sEqpMemo, m_pLk->m_sEqpIdStr, m_pLk->m_sEqpLocation, m_pLk->m_sEqpGUID);
        if (iRet!=0)
          LogError("ModelNDB", 0/*LF_Exclamation*/, "Link SetCommon %s Failed %s", LTg, NErr_String(iRet));
        ASSERT(m_pLk);
        if (strlen(m_pLk->m_sSrcTS)>0 && strlen(m_pLk->m_sDstTS)>0)
          {
          CLinkWiring LW;
          LW.m_bAutoWire=true;
          LW.m_LinkTag=m_pLk->m_sTag;
          LW.m_SrcTS=m_pLk->m_sSrcTS;
          LW.m_DstTS=m_pLk->m_sDstTS;

          if (gs_pPrj->pFlwLib->FE_SetLinkWiring(LW)<0)
            LogError("ModelNDB", 0/*LF_Exclamation*/, "Link AutoWire \n%s:%s -> %s", LW.m_LinkTag, LW.m_SrcTS, LW.m_DstTS);
          }
        }
      rs->MoveNext();
      }
    }
  catch(_com_error & e)
    {
    LogError("ModelNDB", 0/*LF_Exclamation*/, "Enumerate Link Failed\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::DoImportDB(eScdNDBOptions Opts, LPCTSTR pDatabaseName)
  {
  CWaitMsgCursor Wait("Import Model data");

  m_Opts=Opts;
  if (m_Opts & eNDB_Models)
    {
    if (Open(pDatabaseName, false))
      {
      if (Opts & eNDB_GroupInserts)
        ImportGroupInserts();
      if (!ImportBySequence())
        {
        ImportUnits();
        ImportLinks();
        }
      ImportWires();
      if (m_Opts & eNDB_Configuration) 
        ImportConfigs();
      Close();
      return true;
      }
    }
  return false;
  }

//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::DoImportGroup(eScdNDBOptions Opts, LPCTSTR pDatabaseName)
  {
  CWaitMsgCursor Wait("Import Model data");

  m_Opts=Opts;
  if (m_Opts & eNDB_Models)
    {
    if (Open(pDatabaseName, false))
      {
      if (!ImportBySequence())
        {
        ImportUnits();
        ImportLinks();
        }
      ImportWires();
      if (m_Opts & eNDB_Configuration) 
        ImportConfigs();

      Close();
      return true;
      }
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

long CNeutralMdlImportExport::EnumerateGroupTags(eScdNDBOptions Opts, CStringArray &Tags, LPCTSTR pDatabaseName)
  {
  CWaitMsgCursor Wait("Enumerate Group Tags");
  long Count=0;

  m_Opts=Opts;
  if (m_Opts & eNDB_Models)
    {
    if (Open(pDatabaseName, false))
      {
      if (!EnumerateTagsBySequence(Tags))
        {
        EnumerateUnitTags(Tags);
        EnumerateLinkTags(Tags);
        }
      Count=Tags.GetCount(); 
      Close();
      return Count;
      }
    return 0;
    }
  return Count;
  };

//---------------------------------------------------------------------------

bool CNeutralMdlImportExport::DeleteGroup(LPCTSTR Name, LPCTSTR pDatabaseName)
  {
  CWaitMsgCursor Wait("Delete Mdl Group");
  return false;
  };

//===========================================================================
//
//
//
//===========================================================================

//class CNeutralImportExport
//  {
//  friend class CMdlDoc;
//  public:
CNeutralImportExport::CNeutralImportExport()
  {
  };
CNeutralImportExport::~CNeutralImportExport()
  {
  };
void CNeutralImportExport::SetGroup(bool On)
  {
  m_GrfDB.SetGroup(On);
  //m_MdlDB.SetGroup(On);
  };
void CNeutralImportExport::SetGroup(LPCTSTR Group)
  {
  m_GrfDB.SetGroup(Group);
  m_MdlDB.SetGroup(Group);
  };
void CNeutralImportExport::SetGroupDesc(LPCTSTR GroupDesc)
  {
  m_GrfDB.SetGroupDesc(GroupDesc);
  m_MdlDB.SetGroupDesc(GroupDesc);
  };

void CNeutralImportExport::SetExportCoOrds(const CDblTriPt & Origin, const CDblTriPt & Lo, const CDblTriPt & Hi, const CDblTriPt & AlignLo, const CDblTriPt & AlignHi)
  {
  m_GrfDB.SetExportCoOrds(Origin, Lo, Hi, AlignLo, AlignHi);
  //m_MdlDB.SetExportOrigin(OriginX, OriginY, OriginZ);
  };

void CNeutralImportExport::SetImportOrigin(const CDblTriPt & Origin, const CDblTriPt & Scale)
  {
  m_GrfDB.SetImportOrigin(Origin, Scale);
  //m_MdlDB.SetImportOrigin(OriginX, OriginY, OriginZ);
  };

//void CNeutralImportExport::SetExportTagFixups(long FixupRule, LPCTSTR Prefix, LPCTSTR Suffix, CStringArray *OldTags, CStringArray *NewTags)
//  {
//  m_GrfDB.SetExportTagFixups(FixupRule, Prefix, Suffix, OldTags, NewTags);
//  m_MdlDB.SetExportTagFixups(FixupRule, Prefix, Suffix, OldTags, NewTags);
//  };
void CNeutralImportExport::SetExportTagList(CStringArray *OldTags, CStringArray *NewTags)
  {
  m_GrfDB.SetExportTagList(OldTags, NewTags);
  m_MdlDB.SetExportTagList(OldTags, NewTags);
  };
void CNeutralImportExport::SetImportFilter(LPCTSTR Group)
  {
  m_GrfDB.SetImportFilter(Group);
  m_MdlDB.SetImportFilter(Group);
  };
void CNeutralImportExport::SetImportTagFixups(long FixupRule, LPCTSTR String1, LPCTSTR String2, CStringArray *OldTags, CStringArray *NewTags)
  {
  m_GrfDB.SetImportTagFixups(FixupRule, String1, String2, OldTags, NewTags);
  m_MdlDB.SetImportTagFixups(FixupRule, String1, String2, OldTags, NewTags);
  };
bool CNeutralImportExport::DoExportValidate(eScdNDBOptions Opts, CString &ErrMsg)
  {
  bool Gok=m_GrfDB.DoExportValidate(Opts, ScdApp()->GraphTemplate(), ErrMsg);
  bool Mok=Gok && m_MdlDB.DoExportValidate(Opts, ErrMsg);
  return Gok&&Mok;
  }
bool CNeutralImportExport::DoImportValidate(eScdNDBOptions Opts, CString &ErrMsg)
  {
  bool Gok=m_GrfDB.DoImportValidate(Opts, ScdApp()->GraphTemplate(), ErrMsg);
  bool Mok=Gok && m_MdlDB.DoImportValidate(Opts, ErrMsg);
  return Gok&&Mok;
  }
bool CNeutralImportExport::DoExport(eScdNDBOptions Opts, LPCTSTR pGrfDatabase, LPCTSTR pMdlDatabase)
  {
  bool Gok=m_GrfDB.DoExport(Opts, ScdApp()->GraphTemplate(), pGrfDatabase);
  bool Mok=m_MdlDB.DoExport(Opts, pMdlDatabase);
  return Gok&&Mok;
  };
bool CNeutralImportExport::DoImportDB(eScdNDBOptions Opts, LPCTSTR pGrfDatabase, LPCTSTR pMdlDatabase)
  {
  bool Gok=m_GrfDB.DoImportDB(Opts, ScdApp()->GraphTemplate(), pGrfDatabase);
  bool Mok=m_MdlDB.DoImportDB(Opts, pMdlDatabase);
  return Gok&&Mok;
  };
bool CNeutralImportExport::DoImportGroup(eScdNDBOptions Opts, CGrfDoc* pDoc, LPCTSTR pGrfDatabase, LPCTSTR pMdlDatabase)
  {
  bool Gok=m_GrfDB.DoImportGroup(Opts, pDoc, pGrfDatabase);
  bool Mok=m_MdlDB.DoImportGroup(Opts, pMdlDatabase);
  return Gok&&Mok;
  };
long CNeutralImportExport::GetGroups(CNeutralGrpDescList &List, LPCTSTR pDatabaseName)
  {
  return m_GrfDB.GetGroups(List, pDatabaseName);
  };

long CNeutralImportExport::EnumerateGroupTags(eScdNDBOptions Opts, LPCTSTR Group, CStringArray &Tags, LPCTSTR pDatabaseName)
  {
  SetGroup(Group);
  return m_MdlDB.EnumerateGroupTags(Opts, Tags, pDatabaseName);
  };

bool CNeutralImportExport::DeleteGroup(LPCTSTR Name, LPCTSTR pDatabaseName)
  {
  bool Gok=m_GrfDB.DeleteGroup(Name, pDatabaseName);
  bool Mok=m_MdlDB.DeleteGroup(Name, pDatabaseName);
  return Gok&&Mok;
  }


//===========================================================================
//
//
//
//===========================================================================
