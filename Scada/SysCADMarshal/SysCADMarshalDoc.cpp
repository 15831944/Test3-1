// SysCADMarshalDoc.cpp : implementation of the CSysCADMarshalDoc class
//
#include "stdafx.h"
#include "SysCADMarshal.h"
#include "slotmngr.h"

#include "SysCADMarshalDoc.h"
#include "mainfrm.h"
#include "tagdb.h"
#include "dbhelper.h"
#include "FindItDlg.h"
#include "SetValuesDlg.h"
#include ".\syscadmarshaldoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
//
//
//===========================================================================
// CSysCADMarshalDoc
IMPLEMENT_DYNCREATE(CSysCADMarshalDoc, CDocument)
BEGIN_MESSAGE_MAP(CSysCADMarshalDoc, CDocument)
//{{AFX_MSG_MAP(CSysCADMarshalDoc)
  ON_COMMAND(ID_VIEW_CONFIGURATON, OnViewConfiguraton)
  ON_UPDATE_COMMAND_UI(ID_VIEW_CONFIGURATON, OnUpdateViewConfiguraton)
  ON_COMMAND(ID_VIEW_STATISTICS, OnViewStatistics)
  ON_UPDATE_COMMAND_UI(ID_VIEW_STATISTICS, OnUpdateViewStatistics)
	ON_COMMAND(ID_VIEW_HEXADECIMAL, OnViewHexadecimal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HEXADECIMAL, OnUpdateViewHexadecimal)
	ON_COMMAND(ID_CONFIGURATION_LOADDB, OnConfigurationLoaddb)
	ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REFRESH, OnUpdateViewRefresh)
	ON_COMMAND(ID_CHGLOG_ALL, OnChglogAll)
	ON_UPDATE_COMMAND_UI(ID_CHGLOG_ALL, OnUpdateChglogAll)
	ON_COMMAND(ID_CHGLOG_TRK, OnChglogTrk)
	ON_UPDATE_COMMAND_UI(ID_CHGLOG_TRK, OnUpdateChglogTrk)
	ON_COMMAND(ID_MONITOR, OnMonitor)
	ON_UPDATE_COMMAND_UI(ID_MONITOR, OnUpdateMonitor)
	ON_COMMAND(ID_CONFIGURATION_SAVEDB, OnConfigurationSavedb)
	ON_COMMAND(ID_CONFIGURATION_COMMIT, OnConfigurationCommit)
	ON_UPDATE_COMMAND_UI(ID_CONFIGURATION_COMMIT, OnUpdateConfigurationCommit)
	ON_COMMAND(ID_CONFIGURATION_REFRESHDEVICES, OnConfigurationRefreshdevices)
	ON_UPDATE_COMMAND_UI(ID_CONFIGURATION_REFRESHDEVICES, OnUpdateConfigurationRefreshdevices)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateEditFind)
	//}}AFX_MSG_MAP
//  ON_COMMAND(ID_UPDATEALL, OnUpdateAll)
//  ON_COMMAND(ID_UPDATETAGVALUES, OnUpdateTagValues)
ON_COMMAND(ID_FILE_SAVESTATE, OnFileSavestate)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVESTATE, OnUpdateFileSavestate)
ON_COMMAND(ID_FILE_LOADSTATE, OnFileLoadstate)
ON_UPDATE_COMMAND_UI(ID_FILE_LOADSTATE, OnUpdateFileLoadstate)
ON_COMMAND(ID_MODE_SIMULATE, OnModeSimulate)
ON_UPDATE_COMMAND_UI(ID_MODE_SIMULATE, OnUpdateModeSimulate)
ON_COMMAND(ID_EDIT_SETVALUES, OnSetValues)
ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateSetValues)
END_MESSAGE_MAP()
BEGIN_DISPATCH_MAP(CSysCADMarshalDoc, CDocument)
//{{AFX_DISPATCH_MAP(CSysCADMarshalDoc)
// NOTE - the ClassWizard will add and remove mapping macros here.
//      DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()
// Note: we add support for IID_ISysCADMarshal to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the
//  dispinterface in the .ODL file.
// {8DD18AB7-D600-4085-8E4F-B34E51C44D26}
static const IID  IID_ISysCADMarshal =
  {
  0x8dd18ab7,
  0xd600,
  0x4085,
  { 0x8e, 0x4f, 0xb3, 0x4e, 0x51, 0xc4, 0x4d, 0x26 }
  };

BEGIN_INTERFACE_MAP(CSysCADMarshalDoc, CDocument)
INTERFACE_PART(CSysCADMarshalDoc, IID_ISysCADMarshal, Dispatch)
END_INTERFACE_MAP()
/////////////////////////////////////////////////////////////////////////////

// CSysCADMarshalDoc construction/destruction
CSysCADMarshalDoc::CSysCADMarshalDoc(void)
  {
  // TODO: add one-time construction code here
  m_pTagView = NULL;
  m_lView = ViewSlots;
  m_lDeviceSelect = -1;
  m_lMarkedItem = -1;
  m_EdtDst = eCSD_Null;
  m_EdtItem = -1;
  m_bViewCfg = true;
  m_bViewStats = true;
  m_bChgLogAll = false;
  m_bChgLogTrack = true;
  m_bSimulateMode = true;
  m_bCfgBeginEnd = true;

  EnableAutomation();

  m_lCfgSequence=-1;

  AfxOleLockApp();

  m_SlotEdits.SetSize(0,128);
  }

CSysCADMarshalDoc::~CSysCADMarshalDoc(void)
  {
  AfxOleUnlockApp();
  }

//---------------------------------------------------------------------------
//LRESULT CSysCADMarshalDoc::OnUpdateTags(void)
//  {
//  UpdateAllViews(NULL, MakeHint(HINT_UPDATE_TAGS));
//  return 0;
//  };

//---------------------------------------------------------------------------
//LRESULT CSysCADMarshalDoc::OnUpdateTagValues(void)
//  {
//  UpdateAllViews(NULL, MakeHint(HINT_UPDATE_TAG_VALUES));
//  return 0;
//  };

//---------------------------------------------------------------------------

// CSysCADMarshalDoc serialization
void CSysCADMarshalDoc::Serialize(CArchive &ar)
  {
  if(ar.IsStoring())
    {
    // TODO: add storing code here
    }
  else
    {
    // TODO: add loading code here
    }
  }

//---------------------------------------------------------------------------
#ifdef _DEBUG
void CSysCADMarshalDoc::AssertValid(void) const
  {
  CDocument::AssertValid();
  }

void CSysCADMarshalDoc::Dump(CDumpContext &dc) const
  {
  CDocument::Dump(dc);
  }
#endif //_DEBUG

//---------------------------------------------------------------------------
BOOL CSysCADMarshalDoc::OnNewDocument(void)
  {
  //dbgpln("OnNewDocument");
  if(!CDocument::OnNewDocument()) return FALSE;

  // (SDI documents will reuse this document)
  gs_SlotMngr.StopThread();
  gs_SlotMngr.StartThread(NULL);
  return TRUE;
  }

//---------------------------------------------------------------------------
void CSysCADMarshalDoc::DeleteContents(void)
  {
  //dbgpln("DeleteContents");
  gs_SlotMngr.StopThread();

  CDocument::DeleteContents();
  }

//---------------------------------------------------------------------------

BOOL CSysCADMarshalDoc::OnOpenDocument(LPCTSTR lpszPathName)
  {
  //dbgpln("OnOpenDocument %s", lpszPathName);

  gs_SlotMngr.StopThread();

  CProfINIFile  PF(lpszPathName);
  m_sCfgSlotDB   = PF.RdStr("Databases", "Slots", "");
  m_sCfgCdBlkDB  = PF.RdStr("Databases", "CodeBlocks", "");

  m_bChgLogAll    = PF.RdLong("MenuOptions", "ChgLogAll"      , m_bChgLogAll    ? 1:0)!=0;
  m_bChgLogTrack  = PF.RdLong("MenuOptions", "ChgLogTrack"    , m_bChgLogTrack  ? 1:0)!=0;
  m_bMonitor      = PF.RdLong("MenuOptions", "Monitor"        , m_bMonitor      ? 1:0)!=0;
  m_bViewCfg      = PF.RdLong("MenuOptions", "ViewCfg"        , m_bViewCfg      ? 1:0)!=0;
  m_bViewStats    = PF.RdLong("MenuOptions", "ViewStats"      , m_bViewStats    ? 1:0)!=0;
  m_bSimulateMode = PF.RdLong("MenuOptions", "SimulateMode"   , m_bSimulateMode ? 1:0)!=0;

  gs_SlotMngr.StartThread(lpszPathName);

  gs_SlotMngr.StartConfiguration(eDoingCfgLoad);

  LoadCodeBlocks(gs_SlotMngr.FullFileName(m_sCfgCdBlkDB));
  LoadSlots(gs_SlotMngr.FullFileName(m_sCfgSlotDB));

  gs_SlotMngr.EndConfiguration(eDoingCfgLoad);
  gs_SlotMngr.SetSimulateMode(!m_bSimulateMode); 
  gs_SlotMngr.SetSimulateMode(m_bSimulateMode); 

  //Sleep(10000);

  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CSysCADMarshalDoc::OnSaveDocument(LPCTSTR lpszPathName)
  {
  //dbgpln("OnSaveDocument %s", lpszPathName);

  CProfINIFile  PF(lpszPathName);
  PF.WrStr("Databases", "Slots", m_sCfgSlotDB);
  PF.WrStr("Databases", "CodeBlocks", m_sCfgCdBlkDB);

  PF.WrLong("MenuOptions", "ChgLogAll"  , m_bChgLogAll   ? 1:0);
  PF.WrLong("MenuOptions", "ChgLogTrack", m_bChgLogTrack ? 1:0);
  PF.WrLong("MenuOptions", "Monitor"    , m_bMonitor     ? 1:0);
  PF.WrLong("MenuOptions", "ViewCfg"    , m_bViewCfg     ? 1:0);
  PF.WrLong("MenuOptions", "ViewStats"  , m_bViewStats   ? 1:0);
  PF.WrLong("MenuOptions", "SimulateMode"  , m_bSimulateMode? 1:0);

  CMsgLoopLock Lk("OnSaveDoc");
  //CSingleLock Lk(gs_SlotMngr, true);
  gs_SlotMngr.WriteConfiguration(lpszPathName);

  return true;  //CDocument::OnSaveDocument(lpszPathName);
  }

//---------------------------------------------------------------------------

void CSysCADMarshalDoc::OnCloseDocument(void)
  {
  //dbgpln("OnCloseDocument");
  gs_SlotMngr.StopThread();
  CDocument::OnCloseDocument();
  }

//---------------------------------------------------------------------------

void CSysCADMarshalDoc::LoadCodeBlocks(LPCSTR CfgCdBlkDB)
  {
  flag OpenOK=false;
  Strng Ext;
  Ext.FnNameExt((LPSTR)CfgCdBlkDB);
  if (Ext.Len()==0)
    {
    //code block filename not provided
    return;
    }
  Ext.FnExt((LPSTR)CfgCdBlkDB);
  Ext.Upper();
  DWORD line_number = 0;

  if (Ext.XStrCmp(".XLS")==0 || Ext.XStrCmp(".MDB")==0)
    {
    BOOL OK = TRUE;
    CFileStatus State;
    if (CFile::GetStatus(CfgCdBlkDB, State))
      {
      LPTSTR ColNames[] = {"scdLoopTag", "scdSchedule", "scdCodeBlock", "scdDescription", NULL};
      BOOL IsOpen=false;
      CDBHelper *pDB=new CDBHelper((LPSTR)CfgCdBlkDB, "CodeBlocks");
      IsOpen=pDB->Open(DBConnect_Find, ColNames[0], DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings);
      if (!IsOpen)
        {
        delete pDB;
        pDB=new CDBHelper((LPSTR)CfgCdBlkDB, "ScdDrvr");
        IsOpen=pDB->Open(DBConnect_Find, ColNames[0], DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings);
        }
      if (IsOpen)
        {
        OpenOK=true;
        if (pDB->CheckFieldNames(ColNames))
          {
          //bIsOpen = true;
          CStringArray Values;
          while (!pDB->IsEOF())
            {
            pDB->GetNextRow(Values);

            CAddCdBlk *pAdd = new CAddCdBlk;
            pAdd->m_sTag          = Values.GetSize()>0 ? Values[0] : "";
            pAdd->m_sSchedule     = Values.GetSize()>1 ? Values[1] : "";
            pAdd->m_sCdBlk        = Values.GetSize()>2 ? Values[2] : "";
            pAdd->m_sDescription  = Values.GetSize()>3 ? Values[3] : "";

            gs_SlotMngr.ConfigureCdBlk(pAdd);
            }
          //LogNote("DrvMngr", 0, "%u out of %u CodeBlocks loaded", UsedCodeBlks, line_number);
          }
        else
          {
          ReportError("Cnvs", 0, "Correct column field names for '%s' not found.", CfgCdBlkDB);
          }
        pDB->Close();
        }
      else
        {
        ReportError("LoadCodeBlocks", 0, "%s not opened: %s", (LPSTR)CfgCdBlkDB, pDB->ErrorString());
        }
      delete pDB;
      }
    }
  else
    {
    FILE* f=fopen(CfgCdBlkDB, "rt");
    if (f)
      {
      OpenOK=true;
      PSTR pEmpty = " ";
      //bIsOpen = true;
      char Buff[4096];
       // Skip Title Line
      if (fgets(Buff, sizeof(Buff)-1, f))
        line_number++;
      while (!feof(f))
        {
        if (fgets(Buff, sizeof(Buff)-1, f))
          {
          CSVColArray c;
          int Quote;
          int nFlds = ParseCSVTokens(Buff, c, Quote);
          if (Quote)
            {
            ReportError("CodeBlks", 0, "Line %u: Quotes mismatched", line_number);
            break;
            }
          CAddCdBlk *pAdd = new CAddCdBlk;
          pAdd->m_sTag          = nFlds>0 ? c[0] : "";
          pAdd->m_sSchedule     = nFlds>1 ? c[1] : "";
          pAdd->m_sCdBlk        = nFlds>2 ? c[2] : "";
          pAdd->m_sDescription  = nFlds>3 ? c[3] : "";

          gs_SlotMngr.ConfigureCdBlk(pAdd);
          if ((++line_number%10)==0)
            {
            CString S;
            S.Format("Loading CodeBlock :%i", line_number);
            //((CMainFrame*)AfxGetMainWnd())->m_wndStatusBar.SetPaneText(0, S);
            }
          }
        }
      fclose(f);
      ReportError("CodeBlocks", 0, "%u CodeBlocks ", line_number);
      }
    else
      {
      ReportError("LoadCodeBlocks", 0, "%s not opened", (LPSTR)CfgCdBlkDB);
      }
    }
  }

//---------------------------------------------------------------------------

void CSysCADMarshalDoc::LoadSlots(LPCSTR CfgSlotDB)
  {
  Strng Ext;
  Ext.FnExt((LPSTR)CfgSlotDB);
  Ext.Upper();
  DWORD line_number = 0;
  if (Ext.XStrCmp(".XLS")==0 || Ext.XStrCmp(".MDB")==0)
    {
    BOOL OK = TRUE;
    CFileStatus State;
    if (CFile::GetStatus(CfgSlotDB, State))
      {
      LPTSTR ColNames[] = {"scdLoopTag", "scdConnection", "scdTyp", "scdDriver", "scdModifier", "scdRange", 
                           "scdFilter", "scdCompression", "scdConversion", "scdDescription", NULL};
      BOOL IsOpen=false;
      m_SlotTableName="Slots";
      CDBHelper *pDB=new CDBHelper((LPSTR)CfgSlotDB, (LPSTR)(LPCTSTR)m_SlotTableName);
      IsOpen=pDB->Open(DBConnect_Find, ColNames[0], DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings);
      if (!IsOpen)
        {
        m_SlotTableName="ScdDrvr";
        delete pDB;
        pDB=new CDBHelper((LPSTR)CfgSlotDB, (LPSTR)(LPCTSTR)m_SlotTableName);
        IsOpen=pDB->Open(DBConnect_Find, ColNames[0], DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings);
        }
      if (IsOpen)
      //CDBHelper DB((LPSTR)CfgSlotDB, "ScdDrvr");
      //if (DB.Open(DBConnect_Find, ColNames[0], DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings))
        {
        if (pDB->CheckFieldNames(ColNames))
          {
          LPTSTR OptColNames[] = {"scdSlotValues", "scdLinkValues", "scdArchive", "scdTODO", NULL };
          pDB->CheckOptionalFieldNames(OptColNames);
          long m_enSlotValues= (pDB->FieldIndex(pDB->NFieldIndexes()-4)<0 ? -1 : pDB->NFieldIndexes()-4);
          long m_enLinkValues= (pDB->FieldIndex(pDB->NFieldIndexes()-3)<0 ? -1 : pDB->NFieldIndexes()-3);
          long m_enArchiveDB= (pDB->FieldIndex(pDB->NFieldIndexes()-2)<0 ? -1 : pDB->NFieldIndexes()-2);
          long m_enTODO= (pDB->FieldIndex(pDB->NFieldIndexes()-1)<0 ? -1 : pDB->NFieldIndexes()-1);

          CStringArray Values;
          while (!pDB->IsEOF())
            {
            pDB->GetNextRow(Values);

            CSlotCfgStrings  *pAdd = new CSlotCfgStrings;

            pAdd->m_sTag      = Values.GetSize()>0 ? Values[0] : "";
            pAdd->m_sConnects = Values.GetSize()>1 ? Values[1] : "";
            pAdd->m_sType     = Values.GetSize()>2 ? Values[2] : "";
            pAdd->m_sDriver   = Values.GetSize()>3 ? Values[3] : "";
            pAdd->m_sModifier = Values.GetSize()>4 ? Values[4] : "";
            pAdd->m_sRange    = Values.GetSize()>5 ? Values[5] : "";
            if (m_enSlotValues>=0)
              pAdd->SetValueSet(Values.GetSize()>m_enSlotValues ? Values[m_enSlotValues] : "");
            if (m_enLinkValues>=0)
              pAdd->m_sLinkValues = Values.GetSize()>m_enLinkValues ? Values[m_enLinkValues] : "";
            //pAdd->m_sCnv      = Values.GetSize()>8 ? Values[8] : "";
            pAdd->m_sDesc     = Values.GetSize()>9 ? Values[9] : "";

            gs_SlotMngr.ConfigureSlot(false, pAdd);
            line_number++;
            if ((line_number%100)==0)
              gs_SlotMngr.SendUpdateStatus();
            //if ((line_number%10)==0)
            //  {
            //  CString S;
            //  S.Format("Loading Slot :%i", line_number);
            //  ((CMainFrame*)AfxGetMainWnd())->m_wndStatusBar.SetPaneText(0, S);
            //  }
            }
          }
        else
          {
          ReportError("Slots", 0, "Correct column field names for '%s' not found.", CfgSlotDB);
          }
        pDB->Close();
        }
      else
        {
        ReportError("LoadSlots", 0, "%s not Opened: %s", (LPSTR)CfgSlotDB, pDB->ErrorString());
        }
      delete pDB;
      }
    }
  else
    {
    FILE* f=fopen(CfgSlotDB, "rt");
    if (f)
      {
      PSTR pEmpty = " ";
      char Buff[4096];
       // Skip Title Line
      if (fgets(Buff, sizeof(Buff)-1, f))
        line_number++;
      while (!feof(f))
        {
        if (fgets(Buff, sizeof(Buff)-1, f))
          {
          CSVColArray c;
          int Quote;
          int nFlds = ParseCSVTokens(Buff, c, Quote);
          if (Quote)
            {
            LogError("DrvMngr", 0, "Line %u: Quotes mismatched", line_number);
            break;
            }
          // StrStripMatchedQuotes(C[j]);

          CSlotCfgStrings  *pAdd = new CSlotCfgStrings;

          pAdd->m_sTag      = nFlds>0 ? c[0] : "";
          pAdd->m_sConnects = nFlds>1 ? c[1] : "";
          pAdd->m_sType     = nFlds>2 ? c[2] : "";
          pAdd->m_sDriver   = nFlds>3 ? c[3] : "";
          pAdd->m_sModifier = nFlds>4 ? c[4] : "";
          pAdd->m_sRange    = nFlds>5 ? c[5] : "";
          //pAdd->m_sCnv      = nFlds>8 ? c[8] : "";
          pAdd->m_sDesc     = nFlds>9 ? c[9] : "";

          gs_SlotMngr.ConfigureSlot(false, pAdd);
          if ((++line_number%10)==0)
            {
            CString S;
            S.Format("Loading Slot :%i", line_number);
            //((CMainFrame*)AfxGetMainWnd())->m_wndStatusBar.SetPaneText(0, S);
            }
          }
        }
      fclose(f);
      }
    else
      {
      ReportError("LoadSlots", 0, "%s not Opened", (LPSTR)CfgSlotDB);
      }
    }
  };

//---------------------------------------------------------------------------

void CSysCADMarshalDoc::SaveSlots(LPCSTR CfgSlotDB)
  {
  Strng Ext;
  Ext.FnExt((LPSTR)CfgSlotDB);
  Ext.Upper();
  DWORD line_number = 0;
  if (Ext.XStrCmp(".XLS")==0 || Ext.XStrCmp(".MDB")==0)
    {
    BOOL OK = TRUE;
    CFileStatus State;

    if (CFile::GetStatus(CfgSlotDB, State))
      {
      LPTSTR ColNames[] = {"scdLoopTag", "scdConnection", "scdTyp", "scdDriver", "scdModifier", "scdRange", 
                           /*"scdFilter", "scdCompression", "scdConversion",*/ "scdDescription", NULL};
      CDBHelper DB((LPSTR)CfgSlotDB, (LPSTR)(LPCTSTR)m_SlotTableName);
      if (DB.Open(DBConnect_Find, ColNames[0], DBH_HasHeadings/*|DBH_ReadOnly*//*|DBH_ForwardOnly*/|DBH_TrimStrings))
        {
        if (DB.CheckFieldNames(ColNames))
          {
          //LPTSTR OptColNames[] = {"scdArchive", "scdTODO", NULL };
          //DB.CheckOptionalFieldNames(OptColNames);
          //long m_enArchiveDB= (DB.FieldIndex(DB.NFieldIndexes()-2)<0 ? -1 : DB.NFieldIndexes()-2);
          //long m_enTODO= (DB.FieldIndex(DB.NFieldIndexes()-1)<0 ? -1 : DB.NFieldIndexes()-1);

          ADODB::_RecordsetPtr pRS=DB.RS();
          //CDaoRecordset &RS=DB.RS();

          try
            {
            if (!pRS->BOF)
              {
              pRS->MoveFirst();
              while (!pRS->adEOF)
                {
                pRS->Delete(ADODB::adAffectCurrent);
                pRS->MoveNext();
                }
              }
            }
          catch(_com_error &e)
            {
            DB.SetErrorInfo(e, _T("Unable to Clear Database\n") );
            ReportError("SaveSlots", 0, "%s not Cleared: %s", (LPSTR)CfgSlotDB, DB.ErrorString());
            goto Continue;
            }

          try
            {
            for(int i=0; i<m_SlotCfgs.GetSize(); i++)
              {
              CSlotCfg & S=*m_SlotCfgs[i];
              pRS->AddNew();
              pRS->Fields->GetItem(_variant_t("scdLoopTag"))->Value     = (LPCTSTR)S.m_sTag;
              pRS->Fields->GetItem(_variant_t("scdConnection"))->Value  = (LPCTSTR)S.m_sConnects;
              pRS->Fields->GetItem(_variant_t("scdTyp"))->Value         = (LPCTSTR)S.m_sType;
              pRS->Fields->GetItem(_variant_t("scdDriver"))->Value      = (LPCTSTR)S.m_sDriver;
              pRS->Fields->GetItem(_variant_t("scdModifier"))->Value    = (LPCTSTR)S.m_sModifier;
              pRS->Fields->GetItem(_variant_t("scdRange"))->Value       = (LPCTSTR)S.m_sRange;
              pRS->Fields->GetItem(_variant_t("scdDescription"))->Value = (LPCTSTR)S.m_sDesc;
              pRS->Update();
              }
            }
          catch(_com_error &e)
            {
            DB.SetErrorInfo(e, _T("Unable to Save Database\n") );
            ReportError("SaveSlots", 0, "%s not Saved: %s", (LPSTR)CfgSlotDB, DB.ErrorString());
            goto Continue;
            }

          
//          CS}ringArray Values;
//          while (!DB.IsEOF())
//            {
//            DB.GetNextRow(Values);
//
//            CSlotCfgStrings  *pAdd = new CSlotCfgStrings;
//
//            pAdd->m_sTag      = Values.GetSize()>0 ? Values[0] : "";
//            pAdd->m_sConnects = Values.GetSize()>1 ? Values[1] : "";
//            pAdd->m_sType     = Values.GetSize()>2 ? Values[2] : "";
//            pAdd->m_sDriver   = Values.GetSize()>3 ? Values[3] : "";
//            pAdd->m_sModifier = Values.GetSize()>4 ? Values[4] : "";
//            pAdd->m_sRange    = Values.GetSize()>5 ? Values[5] : "";
//            //pAdd->m_sCnv      = Values.GetSize()>8 ? Values[8] : "";
//            pAdd->m_sDesc     = Values.GetSize()>9 ? Values[9] : "";
//
//            gs_SlotMngr.ConfigureSlot(false, pAdd);
//            if ((++line_number%10)==0)
//              {
//              CString S;
//              S.Format("Loading Slot :%i", line_number);
//              //((CMainFrame*)AfxGetMainWnd())->m_wndStatusBar.SetPaneText(0, S);
//              }
//            }
Continue: ;
          }
        else
          {
          ReportError("Slots", 0, "Correct column field names for '%s' not found.", CfgSlotDB);
          }
        DB.Close();
        }
      else
        {
        ReportError("SaveSlots", 0, "%s not Opened: %s", (LPSTR)CfgSlotDB, DB.ErrorString());
        }
      }
    }
  else
    {
    FILE* f=fopen(CfgSlotDB, "rt");
    if (f)
      {
      PSTR pEmpty = " ";
      char Buff[4096];
       // Skip Title Line
      if (fgets(Buff, sizeof(Buff)-1, f))
        line_number++;
      while (!feof(f))
        {
        if (fgets(Buff, sizeof(Buff)-1, f))
          {
          CSVColArray c;
          int Quote;
          int nFlds = ParseCSVTokens(Buff, c, Quote);
          if (Quote)
            {
            LogError("DrvMngr", 0, "Line %u: Quotes mismatched", line_number);
            break;
            }
          // StrStripMatchedQuotes(C[j]);

          CSlotCfgStrings  *pAdd = new CSlotCfgStrings;

          pAdd->m_sTag      = nFlds>0 ? c[0] : "";
          pAdd->m_sConnects = nFlds>1 ? c[1] : "";
          pAdd->m_sType     = nFlds>2 ? c[2] : "";
          pAdd->m_sDriver   = nFlds>3 ? c[3] : "";
          pAdd->m_sModifier = nFlds>4 ? c[4] : "";
          pAdd->m_sRange    = nFlds>5 ? c[5] : "";
          //pAdd->m_sCnv      = nFlds>8 ? c[8] : "";
          pAdd->m_sDesc     = nFlds>9 ? c[9] : "";

          gs_SlotMngr.ConfigureSlot(false, pAdd);
          if ((++line_number%10)==0)
            {
            CString S;
            S.Format("Loading Slot :%i", line_number);
            //((CMainFrame*)AfxGetMainWnd())->m_wndStatusBar.SetPaneText(0, S);
            }
          }
        }
      fclose(f);
      }
    else
      {
      ReportError("SaveSlots", 0, "%s not Opened", (LPSTR)CfgSlotDB);
      }
    }
  };

//===========================================================================

void CSysCADMarshalDoc::OnViewHexadecimal() 
  {
	gs_bAsHex=!gs_bAsHex;
  UpdateAllViews(NULL, MakeHint(HINT_UPDATE_TAGS));
  }

void CSysCADMarshalDoc::OnUpdateViewHexadecimal(CCmdUI* pCmdUI) 
  {
	pCmdUI->Enable(true);
  pCmdUI->SetCheck(gs_bAsHex?1:0);
  }

//===========================================================================

void CSysCADMarshalDoc::OnConfigurationLoaddb() 
  {
  gs_SlotMngr.StartConfiguration(eDoingCfgReLoad);

  LoadCodeBlocks(gs_SlotMngr.FullFileName(m_sCfgCdBlkDB));
  LoadSlots(gs_SlotMngr.FullFileName(m_sCfgSlotDB));

  gs_SlotMngr.EndConfiguration(eDoingCfgReLoad);
  gs_SlotMngr.SetSimulateMode(!m_bSimulateMode); 
  gs_SlotMngr.SetSimulateMode(m_bSimulateMode); 
  }

//===========================================================================

void CSysCADMarshalDoc::OnConfigurationSavedb() 
  {
	if (1 || IsModified())
    {
    SetModifiedFlag(false);

    SaveSlots(gs_SlotMngr.FullFileName(m_sCfgSlotDB));
    };
  }

//===========================================================================

void CSysCADMarshalDoc::SetSlotCfg(long CfgSequence, CSlotCfgArray * pSlots)
  {
  m_lCfgSequence=CfgSequence;
  for (int i=0; i<m_SlotCfgs.GetSize(); i++)
    delete m_SlotCfgs[i];
  m_SlotCfgs.SetSize(pSlots->GetSize());
  m_IndexOfSlotInList.SetSize(pSlots->GetSize());
  for (i=0; i<m_SlotCfgs.GetSize(); i++)
    {
    m_SlotCfgs[i]=(*pSlots)[i];
    m_IndexOfSlotInList[i]=-1;
    }
  }

//===========================================================================

void CSysCADMarshalDoc::SetLinkCfg(long CfgSequence, CLinkCfgArray * pLinks)
  {
  ASSERT(m_lCfgSequence==CfgSequence);
  for (int i=0; i<m_LinkCfgs.GetSize(); i++)
    delete m_LinkCfgs[i];
  m_LinkCfgs.SetSize(pLinks->GetSize());
  m_IndexOfLinkInList.SetSize(pLinks->GetSize());
  for (i=0; i<m_LinkCfgs.GetSize(); i++)
    {
    m_LinkCfgs[i]=(*pLinks)[i];
    m_IndexOfLinkInList[i]=-1;
    }
  }

//===========================================================================

void CSysCADMarshalDoc::SetCdBlkCfg(long CfgSequence, CCdBlkCfgArray * pCdBlks)
  {
  ASSERT(m_lCfgSequence==CfgSequence);
  for (int i=0; i<m_CdBlkCfgs.GetSize(); i++)
    delete m_CdBlkCfgs[i];
  m_CdBlkCfgs.SetSize(pCdBlks->GetSize());
  m_CdBlkListIndex.SetSize(pCdBlks->GetSize());
  for (i=0; i<m_CdBlkCfgs.GetSize(); i++)
    {
    m_CdBlkCfgs[i]=(*pCdBlks)[i];
    m_CdBlkListIndex[i]=-1;
    }

  }

//===========================================================================

void CSysCADMarshalDoc::SetDeviceCfg(long CfgSequence, CDeviceCfgArray * pDevices)
  {
  ASSERT(m_lCfgSequence==CfgSequence);
  for (int i=0; i<m_DeviceCfgs.GetSize(); i++)
    delete m_DeviceCfgs[i];
  m_DeviceCfgs.SetSize(pDevices->GetSize());
  //m_DeviceListIndex.SetSize(pDevices->GetSize());
  for (i=0; i<m_DeviceCfgs.GetSize(); i++)
    {
    m_DeviceCfgs[i]=(*pDevices)[i];
    //m_DeviceListIndex[i]=-1;
    }

  UpdateAllViews(NULL, MakeHint(HINT_UPDATE_CFG));
  }

//===========================================================================

void CSysCADMarshalDoc::UpdateOneSlot(long SlotNo, CSlotValue * pSlotValue)
  {
  ASSERT(SlotNo==pSlotValue->m_lSlot);
  //dbgpln("OnUpdateOneSlot: %4i", SlotNo);
  if (m_lCfgSequence!=pSlotValue->m_lCfgSequence)
    {
    ReportError("TagView", 0, "Bad CfgSequence %i != %i", m_lCfgSequence, pSlotValue->m_lCfgSequence);
    return ;
    }
  m_SlotCfgs[SlotNo]->SlotValue()=*pSlotValue;
  UpdateAllViews(NULL, MakeHint(HINT_UPDATE_ONESLOTVALUE, SlotNo));

  delete pSlotValue;
  };

//===========================================================================

void CSysCADMarshalDoc::UpdateOneLink(long LinkNo, CLinkValue * pLinkValue)
  {
  ASSERT(LinkNo==pLinkValue->m_lLink);
  //dbgpln("OnUpdateOneLink: %4i", LinkNo);
  if (m_lCfgSequence!=pLinkValue->m_lCfgSequence)
    {
    ReportError("TagView", 0, "Bad CfgSequence %i != %i", m_lCfgSequence, pLinkValue->m_lCfgSequence);
    return ;
    }
  m_LinkCfgs[LinkNo]->LinkValue()=*pLinkValue;
  UpdateAllViews(NULL, MakeHint(HINT_UPDATE_ONELINKVALUE, LinkNo));

  delete pLinkValue;
  };

//===========================================================================

void CSysCADMarshalDoc::UpdateOneSlotState(long SlotNo, long SlotState)
  {
  m_SlotCfgs[SlotNo]->StateLong()=SlotState;
  UpdateAllViews(NULL, MakeHint(HINT_UPDATE_ONESLOTVALUE, SlotNo));
  };

//===========================================================================

void CSysCADMarshalDoc::UpdateOneLinkState(long LinkNo, long LinkState)
  {
  m_LinkCfgs[LinkNo]->StateLong()=LinkState;
  UpdateAllViews(NULL, MakeHint(HINT_UPDATE_ONELINKVALUE, LinkNo));
  };

//===========================================================================

void CSysCADMarshalDoc::XferChangeBlock(long wParam, CChangeBlock*pChangeBlock)
  {
  UpdateAllViews(NULL, MakeHint(HINT_UPDATE_CHANGES, 0), (CObject*)pChangeBlock);
  };

//===========================================================================

bool CSysCADMarshalDoc::SetSelection(CSelection & Selection)
  {
  m_Selection=Selection;
  //CString sBestTag;
  ////long iWhat=wParam;
  int iBestInx=-1;
  //int iBestIsSlot=false;
  //const int LinksSize = m_LinkCfgs.GetSize();
  //const int SlotsSize = m_SlotCfgs.GetSize();
  //int n=String.GetLength();
  //if (n>0)
  //  {
  //  if (iWhat==eFindStr_LinkTag || iWhat == eFindStr_SimulatorTag)
  //    {
  //    for (int i=0; i<LinksSize; i++)
  //      {
  //      CString & Tag = (iWhat==eFindStr_LinkTag ? m_LinkCfgs[i]->m_sTag : m_LinkCfgs[i]->m_sSimTag);
  //      if (_strnicmp(Tag, String, n)==0)
  //        {
  //        if (iBestInx<0)
  //          {
  //          sBestTag = Tag;
  //          iBestInx = i;
  //          }
  //        else if (_stricmp(Tag, sBestTag)<0)
  //          {
  //          sBestTag = Tag;
  //          iBestInx = i;
  //          }
  //        }
  //      }
  //    }
  //  if (iWhat==eFindStr_SlotTag || iWhat == eFindStr_OPCTag)
  //    {
  //    for (int i=0; i<SlotsSize; i++)
  //      {
  //      CString & Tag = (iWhat==eFindStr_SlotTag ? m_SlotCfgs[i]->m_sTag : m_SlotCfgs[i]->m_sOPCTag);
  //      if (_strnicmp(Tag, String, n)==0)
  //        {
  //        if (iBestInx<0)
  //          {
  //          sBestTag = Tag;
  //          iBestInx = i;
  //          iBestIsSlot = true;
  //          }
  //        else if (_stricmp(Tag, sBestTag)<0)
  //          {
  //          sBestTag = Tag;
  //          iBestInx = i;
  //          iBestIsSlot = true;
  //          }
  //        }
  //      }
  //    }

  //  dbgpln("Find >>>> %-20s %s %5i %s", String, iBestIsSlot?"Slot":"Link", iBestInx, sBestTag.GetBuffer());
    if (1)//iBestInx>=0)
      {
      //String=sBestTag;
      if (1)//DoIt)
        {
        UpdateAllViews(NULL, MakeHint(HINT_SETSELECTION, iBestInx));
        UpdateAllViews(NULL, MakeHint(HINT_CHANGE_VIEW));
        UpdateAllViews(NULL, MakeHint(HINT_UPDATE_TAGS));
        }
      }
  //  }
  //return iBestInx>=0;
  return false;
  };

//===========================================================================
//
//
//
//===========================================================================

// CSysCADMarshalDoc commands
void CSysCADMarshalDoc::OnViewConfiguraton(void)
  {
  m_bViewCfg = !m_bViewCfg;
  UpdateAllViews(NULL, MakeHint(HINT_CHANGE_VIEW));
  UpdateAllViews(NULL, MakeHint(HINT_UPDATE_TAGS));
  }

void CSysCADMarshalDoc::OnUpdateViewConfiguraton(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(true);
  pCmdUI->SetCheck(m_bViewCfg ? 1 : 0);
  }

//---------------------------------------------------------------------------

void CSysCADMarshalDoc::OnViewStatistics(void)
  {
  m_bViewStats = !m_bViewStats;
  UpdateAllViews(NULL, MakeHint(HINT_CHANGE_VIEW));
  UpdateAllViews(NULL, MakeHint(HINT_UPDATE_TAGS));
  }

void CSysCADMarshalDoc::OnUpdateViewStatistics(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(true);
  pCmdUI->SetCheck(m_bViewStats ? 1 : 0);
  }

//---------------------------------------------------------------------------

void CSysCADMarshalDoc::OnViewRefresh() 
  {
	
  }

void CSysCADMarshalDoc::OnUpdateViewRefresh(CCmdUI* pCmdUI) 
  {
	pCmdUI->Enable(true);
  }

//---------------------------------------------------------------------------

void CSysCADMarshalDoc::OnChglogAll() 
  {
  m_bChgLogAll = !m_bChgLogAll;
  UpdateAllViews(NULL, MakeHint(HINT_UPDATE_CHGLOG));
  }

void CSysCADMarshalDoc::OnUpdateChglogAll(CCmdUI* pCmdUI) 
  {
  pCmdUI->Enable(m_lView==ViewChanges);
  pCmdUI->SetCheck(m_bChgLogAll ? 1 : 0);
  }

//---------------------------------------------------------------------------

void CSysCADMarshalDoc::OnChglogTrk() 
  {
  m_bChgLogTrack = !m_bChgLogTrack;
  UpdateAllViews(NULL, MakeHint(HINT_UPDATE_TRACKING));
  }

void CSysCADMarshalDoc::OnUpdateChglogTrk(CCmdUI* pCmdUI) 
  {
  pCmdUI->Enable(m_lView==ViewChanges);
  pCmdUI->SetCheck(m_bChgLogTrack ? 1 : 0);
  }

//---------------------------------------------------------------------------

void CSysCADMarshalDoc::OnMonitor() 
  {
  m_bMonitor = !m_bMonitor;
  PostThreadMessage(gs_SlotMngr.ThreadId(), WMU_CHANGEMONITOR, m_bMonitor!=0, 0);
  }

void CSysCADMarshalDoc::OnUpdateMonitor(CCmdUI* pCmdUI) 
  {
  pCmdUI->Enable(true);
  pCmdUI->SetCheck(m_bMonitor ? 1 : 0);
  }

void CSysCADMarshalDoc::AddEdit(CSlotCfgStrings * pCfg)
  {
  SetModifiedFlag(true);

  pCfg->m_sTag.TrimLeft();
  pCfg->m_sTag.TrimRight();
  pCfg->m_lNo=m_SlotEdits.GetSize();
  
  for (int i=0; i<m_SlotEdits.GetSize(); i++)
    if (m_SlotEdits[i]->m_sTag.CompareNoCase(pCfg->m_sTag)==0)
      {
      delete m_SlotEdits[i];
      m_SlotEdits[i]=pCfg;
      UpdateAllViews(NULL, HINT_UPDATE_EDITS);
      return;
      }

  m_SlotEdits.Add(pCfg);
    
  UpdateAllViews(NULL, HINT_UPDATE_EDITS);
  return;


  //gs_SlotMngr.ConfigureSlot(true, pCfg);
  }

void CSysCADMarshalDoc::ClearEdits()
  {
  for (int i=0; i<m_SlotEdits.GetSize(); i++)
    delete m_SlotEdits[i];

  m_SlotEdits.SetSize(0);

  UpdateAllViews(NULL, HINT_UPDATE_EDITS);
  return;
  }

void CSysCADMarshalDoc::ApplyEdits()
  {
  gs_SlotMngr.EditStart();
  for (int i=0; i<m_SlotEdits.GetSize(); i++)
    gs_SlotMngr.EditSlot(m_SlotEdits[i]);
  gs_SlotMngr.EditEnd();
  
  m_SlotEdits.SetSize(0);
  SetModifiedFlag(false);
  UpdateAllViews(NULL, HINT_UPDATE_EDITS);
  return;
  }

void CSysCADMarshalDoc::OnConfigurationCommit() 
  {
	ApplyEdits();	
  }

void CSysCADMarshalDoc::OnUpdateConfigurationCommit(CCmdUI* pCmdUI) 
  {
  pCmdUI->Enable(IsModified());
  }

void CSysCADMarshalDoc::OnConfigurationRefreshdevices() 
  {
  PostThreadMessage(gs_SlotMngr.ThreadId(), WMU_WRITEALL2DEVICES, 0, 0);
  }

void CSysCADMarshalDoc::OnUpdateConfigurationRefreshdevices(CCmdUI* pCmdUI) 
  {
  pCmdUI->Enable(true);
  }

void CSysCADMarshalDoc::OnEditFind()
  {
  CFindItDlg::OpenIt(AfxGetMainWnd(), this);
  };

void CSysCADMarshalDoc::OnUpdateEditFind(CCmdUI* pCmdUI) 
  {
  pCmdUI->Enable(true);
  }

void CSysCADMarshalDoc::OnFileSavestate()
  {
  CString *pStr=new CString("TheState");
  //CString *pStr=new CString(m_strPathName);
  //(*pStr)+="TheState";
  PostThreadMessage(gs_SlotMngr.ThreadId(), WMU_SAVESTATE, 0, (LPARAM)pStr);
  }

void CSysCADMarshalDoc::OnUpdateFileSavestate(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(true);
  }

void CSysCADMarshalDoc::OnFileLoadstate()
  {
  CString *pStr=new CString("TheState");
  //CString *pStr=new CString(m_strPathName);
  //(*pStr)+="TheState";
  PostThreadMessage(gs_SlotMngr.ThreadId(), WMU_LOADSTATE, 0, (LPARAM)pStr);
  }

void CSysCADMarshalDoc::OnUpdateFileLoadstate(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(true);
  }

//---------------------------------------------------------------------------

void CSysCADMarshalDoc::OnModeSimulate()
  {
  m_bSimulateMode = !m_bSimulateMode;
  gs_SlotMngr.SetSimulateMode(m_bSimulateMode); 
  }

void CSysCADMarshalDoc::OnUpdateModeSimulate(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(TRUE);//m_lView==ViewChanges);
  pCmdUI->SetCheck(m_bSimulateMode? 1 : 0);
  }
//---------------------------------------------------------------------------


void CSysCADMarshalDoc::OnSetValues()
  {
  CSetValuesDlg::OpenIt(AfxGetMainWnd(), this);
  }

void CSysCADMarshalDoc::OnUpdateSetValues(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(true);
  }
