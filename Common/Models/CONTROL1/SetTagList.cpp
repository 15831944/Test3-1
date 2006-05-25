//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __SETTAGLIST_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "settaglist.h"

#if !SKIPIT 

//==========================================================================
//
//
//
//==========================================================================

TagListFileHelper::TagListFileHelper(CTagListCon* Parent)
  {
  pParent = Parent;
  bValid = 0;
  bLogSetTags = 0;
  sPath = PrjFiles();
  hProcess = NULL;
  dwProcessId = 0;
  }

//--------------------------------------------------------------------------

TagListFileHelper::~TagListFileHelper()
  {
  }

//--------------------------------------------------------------------------

bool TagListFileHelper::LoadAndSetTags(bool TestParamStopped)
  {
  Strng Fn(sPath);
  Fn.FnExpand();
  Fn.FnCheckEndBSlash();
  Fn += sFile;
  if (FileExists(Fn()))
    {
    bool FileError = false;
    char Buff[1024];
    int ValidRowCnt = 0;
    FILE* f=fopen(Fn(), "rt");
    if (f)
      {
      int line_number = 0;
      char* c[MaxCSVCols];
      fgets(Buff, sizeof(Buff)-1, f);// Skip Title Line
      line_number++;
      while (!feof(f))
        {
        if (fgets(Buff, sizeof(Buff)-1, f))
          {
          line_number++;
          int Quote;
          int nFlds = ParseCSVTokens(Buff, c, Quote);
          if (Quote)
            {
            LogWarning(pParent->Tag(), 0, "Line %u: Quotes mismatched", line_number);
            break;
            }
          const int len = (nFlds>0 ? strlen(c[0]) : 0);
          if (len==0)
            {
            break; //blank line
            }
          if (nFlds<2)
            {
            LogWarning(pParent->Tag(), 0, "Line %u: Expect Tag and Value", line_number);
            break;
            }
          if (c[0] && c[1] && len>0/* && strlen(c[1])>0*/)
            {
            Strng Tag = c[0];
            int Ret = TryWriteTag(pParent->FamilyHead(), c[0], c[1], TestParamStopped);
            if (Ret==FXR_Found)
              {
              ValidRowCnt++;
              if (bLogSetTags)
                LogNote(pParent->Tag(), 0, "Set '%s' to %s", c[0], c[1]);
              }
            else
              {
              LogWarning(pParent->Tag(), 0, "Failed to set tag '%s'!", c[0]);
              }
            }
          else
            {
            }
          }
        }
      fclose(f);
      sStatus.Set("OK (%d tags set out of %d rows)", ValidRowCnt, line_number-1);
      if (bLogSetTags)
        LogNote(pParent->Tag(), 0, "%d tags set out of %d rows", ValidRowCnt, line_number-1);
      return true;
      }
    else
      {
      if (!FileError)
        sStatus = "No rows with Tags and Value have been specified!";
      }
    }
  else
    sStatus = "Specified file not found!";
  return false;
  }

//--------------------------------------------------------------------------
//==========================================================================

static double Drw_TagListCon[] =
       {
       DD_Poly, -4,-4, -4,4, 4,4, 4,-4, -4,-4, 
       DD_Poly, -2,2, 0,2,
       DD_Poly, -1,2, -1,-2,
       DD_Poly, 1,2, 1,-2, 3,-2,
       DD_End
       };

IMPLEMENT_MODELUNIT(CTagListCon, "TagListCon", "", Drw_TagListCon, "Control", "STL", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT,  
                    "Control:Set Tag List",
                   "Events Control model.")

//--------------------------------------------------------------------------

CTagListCon::CTagListCon(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach),
  TLH(this)
  {
  fHasFiles=1;
  AttachClassInfo(nc_Control, NULL, NULL, &NullFlwGroup);
  fActiveHoldOK_Dyn=true;
  bOn = 1;
  bSetAtStart = 0;
  bSetBeforeStart = 0;
  bAboutToStart = 0;
  }

//--------------------------------------------------------------------------

CTagListCon::~CTagListCon()
  {
  }

//--------------------------------------------------------------------------

void CTagListCon::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

const word idmCheckBtn    = 1000;
const word idmPath        = 1001;
const word idmFile        = 1002;
const word idmEditBtn     = 1003;
const word idmBrowseBtn   = 1004;
const word idmSetTagsBtn  = 1005;

//--------------------------------------------------------------------------

void CTagListCon::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  //DDB.Text("");
  //DDB.Button("Check_tags"/* and functions"*/, "", DC_, "", idmCheckBtn, this, isParmStopped);
  DDB.Text("");

  Strng Tag;
  DDB.CheckBoxBtn("On",          "",          DC_,     "",      &bOn, this, isParm, DDBYesNo);
  DDB.String("Status",           "",          DC_,     "",      &(TLH.sStatus), this, isResult|noFileAtAll);
  DDB.String("Folder",           "",          DC_,     "",      idmPath, this, isResult);//isParm);
  DDB.String("File",             "",          DC_,     "",      idmFile, this, isParm);
  DDB.Text("");
  DDB.Button("Edit",             "",          DC_,     "",      idmEditBtn, this, isParm);
  DDB.Button("Browse",           "",          DC_,     "",      idmBrowseBtn, this, isParm);
  DDB.CheckBoxBtn("LogTagSets",  "",          DC_,     "",      &TLH.bLogSetTags, this, isParm, DDBYesNo);
  DDB.Text("");
  DDB.Button("SetTags",          "",          DC_,     "",      idmSetTagsBtn, this, isParm);
  DDB.Text("");
  DDB.CheckBoxBtn("SetBeforeStart", "",       DC_,     "",      &bSetBeforeStart, this, isParm, DDBYesNo);
  DDB.CheckBoxBtn("SetAtStart",  "",          DC_,     "",      &bSetAtStart, this, isParm, DDBYesNo);
  
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CTagListCon::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  switch (DCB.lHandle)
    {
    case idmCheckBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        //AccNodeWnd::RefreshData(TRUE); ???
        //TODO: need to accept current data before processing button push!
        if (PreStartCheck())
          LogNote(Tag(), 0, "No bad external tag references or function errors");
//        bAboutToStart = 0;
        }
      DCB.B=0;
      return 1;
    case idmPath:
      if (DCB.rpC!=NULL)
        {
        // Dont Set if coming from snapshot ???
        TLH.sPath = DCB.rpC;
        TLH.sPath.FnContract();
        }
      if (DCB.ForFileSnpScn())
        {
        TLH.sPath.FnContract();//ensure it is always in contract form!
        DCB.pC = TLH.sPath();
        }
      else
        {
        TLH.sPathForView = TLH.sPath;
        if (SymbolicPaths())
          TLH.sPathForView.FnContract();
        else
          TLH.sPathForView.FnExpand();
        DCB.pC = TLH.sPathForView();
        }
      return 1;
    case idmFile:
      if (DCB.rpC)
        {
        Strng Prev(TLH.sFile);
        TLH.sFile = DCB.rpC;
        //if (Prev!=TLH.sFile)
        //  TLH.bReloadRqd = 1;
        }
      DCB.pC = TLH.sFile();
      return 1;
    case idmEditBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng FullFilename;
        int RetCode = DCB.DoEditBtn(TLH.hProcess, TLH.dwProcessId, TLH.sPath(), TLH.sFile(), NULL, &FullFilename, true);
        if (RetCode==-1)
          {
          FILE* f = fopen(FullFilename(), "wt");
          if (f)
            {
            char Buff[2048];
            strcpy(Buff, "Tag,Value\n");
            fwrite(Buff, sizeof(char), strlen(Buff), f);
            fclose(f);
            }
          RetCode = DCB.DoEditBtn(TLH.hProcess, TLH.dwProcessId, TLH.sPath(), TLH.sFile(), NULL);
          }
        }
      DCB.B=0;
      return 1;
    case idmBrowseBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng Prev(TLH.sPath);
        Prev += TLH.sFile;
        DCB.DoBrowseBtn(NULL/*"csv"*/, "SysCAD Events", TLH.sPath, TLH.sFile);
        Strng Next(TLH.sPath);
        Next += TLH.sFile;
        //if (Prev!=Next)
        //  TLH.bReloadRqd = 1;
        }
      DCB.B=0;
      return 1;
    case idmSetTagsBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        if (bOn)
          TLH.LoadAndSetTags(!gs_Exec.Stopped());
        }
      DCB.B=0;
      return 1;
    }//end switch
  return False;
  }

//--------------------------------------------------------------------------

flag CTagListCon::ValidateData(ValidateDataBlk & VDB)
  {
  if (bSetBeforeStart)
    bSetAtStart = 0;
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

flag CTagListCon::PreStartCheck()
  {
  bAboutToStart = 1;
  return FlwNode::PreStartCheck();
  }

//--------------------------------------------------------------------------

void CTagListCon::EvalCtrlInitialise(eScdCtrlTasks Tasks)
  {
  if (bOn && bSetBeforeStart && !GetActiveHold())
    {
    TLH.LoadAndSetTags(false);
    }
  }

//--------------------------------------------------------------------------

void CTagListCon::EvalCtrlStrategy(eScdCtrlTasks Tasks)
  {
  if (bAboutToStart)
    {
    if (bOn && bSetAtStart && !GetActiveHold() && ICGetTimeInc() > 0.0)
      {
      TLH.LoadAndSetTags(true);
      }
    bAboutToStart = 0;
    }
  }

//--------------------------------------------------------------------------

void CTagListCon::SetState(eScdMdlStateActs RqdState)
  {
  FlwNode::SetState(RqdState);
  }

//--------------------------------------------------------------------------

void CTagListCon::EvalProducts(long JoinMask)
  {
  }

//--------------------------------------------------------------------------

int CTagListCon::FilesUsed(CFilesUsedArray & Files)
  {
  int Cnt = 0;
  if (TLH.sFile.Len()>0)
    {
    Strng Fn(TLH.sPath);
    Fn.FnExpand();
    Fn += TLH.sFile;
    Files.AddFile(Fn(), FU_CopyFile|FU_EditTxt);
    Cnt++;
    }
  return Cnt;
  }

//--------------------------------------------------------------------------

int CTagListCon::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  //todo: perhaps load the file and check all tags...
  return EOCT_DONE;
  }

//--------------------------------------------------------------------------

int CTagListCon::DeleteTag(pchar pDelTag)
  {
  //todo: perhaps load the file and check all tags...
  return EODT_DONE;
  }

//--------------------------------------------------------------------------
                                          
dword CTagListCon::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=(bOn ? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag CTagListCon::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\t?????????"; return 1;
    default:                                               
      return FlwNode::CIStrng(No, pS);
    }
  }

//==========================================================================

#endif