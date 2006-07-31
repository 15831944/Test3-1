// ScdMdlWrap.cpp: implementation of the CScdSlvWrap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ScdSlvWrap.h"
#include "licbase.h"

#include "Sp_DB.h"
#include "tagdb.h"
#include "flwsolve.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//CScdSlvWrapMap g_ScdMdlWraps;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScdSlvWrap::CScdSlvWrap()
  {
  gs_MsgLog.SetMsgBoxEmbedded();
  }

CScdSlvWrap::~CScdSlvWrap()
  {

  }

HRESULT CScdSlvWrap::FinalConstruct()
  {
	FC_TESTLICENSE(gs_License.AllowCOMSlv());
  AtlTrace("CScdSolver : TID %i ==================================\n", GetCurrentThreadId());
  AtlTrace("CScdSolver : Done ==================================\n");
  return S_OK;
  };

void CScdSlvWrap::FinalRelease()
  {
  gs_CDB.Terminate();
  SVI.Term();
  gs_CnvsMngr.Reset();
  }

bool CScdSlvWrap::InitialiseCfg(BSTR CfgFileName)
  {
  USES_CONVERSION;
      //==============================================
  char PDir[1024];
  
  Strng m_CfgFile(OLE2T(CfgFileName));


  SetSCDFileFormat(1);
  SetPrjFileFormat(1);

  SetCfgFiles();
 // SetPrjFiles();
  
  if (GetModuleFileName(GetModuleHandle("ScdSlv.dll"), PDir, sizeof(PDir))>0)
    {
    Strng ProgPath;
    ProgPath.FnDrivePath(PDir);
    SetProgFiles(ProgPath());

    if (1)
      {
      // Extend Path
      Strng XPath;
      XPath=ProgFiles(); // this if Server is begin run fomr VB diirectly
      XPath+=";";
      XPath+=ProgFiles();
      XPath+="Secure;";
      XPath+=ProgFiles();
      XPath+="Depends;";

      char OrgPathBuff[16384];
      char NewPathBuff[16384];
      OrgPathBuff[0]=0;
      NewPathBuff[0]=0;
      if (GetEnvironmentVariable("PATH", OrgPathBuff, sizeof(OrgPathBuff))<sizeof(OrgPathBuff))
        {
        strcpy(NewPathBuff, XPath());
        strcat(NewPathBuff, OrgPathBuff);
        SetEnvironmentVariable("PATH", NewPathBuff);
        }
      }
    }

	Strng BCfg(ProgFiles());//, OBCfg;
  BCfg.FnClearEndBSlash();
  BCfg.FnDrivePath(BCfg());
  BCfg.FnClearEndBSlash();
  //OBCfg=BCfg;
  BCfg.FnMakeDataFolder(BCfgFolderName());
  //OBCfg.FnMakeDataFolder(OldBCfgFolderName());
  SetBaseCfgFiles(BCfg());

  //if (!FileExists(BaseCfgFiles()))
  //  {
  //  OBCfg.FnClearEndBSlash();
  //  if (FileExists(OBCfg()) && !MoveFile(OBCfg(), BCfg()))
  //    LogError("Project",0, "%s not renamed to %s",OBCfg(), BCfg());
  //  }

  //long CnvsLoadedCnt = 0;
  Strng CnvsFilename=BaseCfgFiles();
  CnvsFilename+=BCfgDBFileName();
  /*CnvsLoadedCnt =*/ gs_CnvsMngr.LoadCnvsADO(CnvsFilename(), false);
  SetPrjFileFormat(ScdPFUser.RdInt("General", "PrjFileFormat", 1));

  bool OK=true;
  CModelInfo Info;
  byte CfgRetCode=Info.GetCfgInfo(m_CfgFile(), false, false);//, flag WhingeLoudly)
  if (CfgRetCode<2)
    OK=false;
  //if (!AbortLoad && FmtOK && !OK)
  //  OK=Info.GetSetName(PrjFile(), false, PrjFileVerNo(), gs_License.LicCatagories());
  
  // Ignore the DLL List State
  //if (CfgRetCode==2)
  //  OK=false;

  if (OK)
    {
    SetCfgFile(Info.m_sCfgFile());
    SetCfgFiles(Info.m_sCfgFiles());

    //CheckCfgFilesName();
    //OK=InfoU.CheckDBFileFormat(CfgFiles());
    //if (OK)
    //  Info.EnsureCfgIsInList();
  
    Strng FlwLibTag, EO_LocationTag;
    FlwLibTag=PlantModelTag;
    EO_LocationTag=PlantModelTag;

    //pFlwLib=NULL;
    if (MdlLibs.LoadFlwDLLs(Info.m_sDLLList, FlwLibTag(), EO_LocationTag(), &m_FlwLib))
      {
      //pFlwLib=&m_FlwLib;
      }
//                     OK = LoadFlwDLLs(Info.DLLList, FlwLibTag(), EO_LocationTag());
    
    }

  //CScdSlvWrap::sm_pSfeSrvr=m_FlwLib.

  Strng m_PrjSDBName;
  Strng m_PrjCnvsDBName;
  
  m_PrjSDBName=CfgFiles();
  m_PrjSDBName+=CfgDBFileName();
  m_PrjCnvsDBName=m_PrjSDBName; //cnvs DB same as specie DB
  
  m_PrjCnvsDBName.FnExpand();
  m_PrjSDBName.FnExpand();
  
  //load...
  //CnvsLoadedCnt = 0;
  if (PrjFileVerNo()<51)
    m_PrjCnvsDBName.FnSearchExpand(NULL, AF_CfgFiles|AF_BackupFiles);
  if (FileExists(m_PrjCnvsDBName()))
		/*CnvsLoadedCnt =*/ gs_CnvsMngr.LoadCnvsADO(m_PrjCnvsDBName(), true);
  else
    m_PrjCnvsDBName="";
  //if (CnvsLoadedCnt>0)
  //  LogNote("Sfe_Server", 0, "%d new conversions loaded from %s", CnvsLoadedCnt, m_PrjCnvsDBName());
  m_PrjCnvsDBName.FnMakePrjFileRelative();
  m_PrjCnvsDBName.FnContract();

  if (PrjFileVerNo()<51)
    m_PrjSDBName.FnSearchExpand(NULL, AF_CfgFiles|AF_BackupFiles);
  if (!FileExists(m_PrjSDBName()))
    {
    LogError("Sfe_Server", LF_Exclamation, "File not found: %s", m_PrjSDBName());
    //m_PrjSDBName="";
    }
  
  if (!CFlwSolver::LoadSpecieData(CfgFile(), m_PrjSDBName(), false))
    {
    LogError("Sfe_Server", LF_Exclamation, "Specie Data no Loaded : %s", CfgFile());
    return false;
    }

  //CSpecieBlk::AttachSelectableFns();
  //TagObjClass::UpdateSelectConditions();
  //CSpecieBlk::DetachSelectableFns();
  CSpecieBlk::TestInitialiseOK();

  return true;
  }


//HRESULT CScdSlvWrap::GetSpecieDB(IScdSpecieDB **pVal)
//  {
//  HRESULT hr=S_OK;
////  if (!m_spDB.GetInterfacePtr())
////    {
////    hr=m_spDB.CreateInstance(__uuidof(IScdSpecieDB));
////   // m_spDB.AddRef();
////    }
////  if (SUCCEEDED(hr))
////    {
////    m_spDB.AddRef();
////    *pVal=m_spDB;
////    }
//
//  return hr;
//  }