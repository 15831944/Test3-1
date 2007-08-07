//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "profiler.h"
#include "profcon2.h"

#if !SKIPIT 

//==========================================================================

const long FunctSubsStartIndex = 500;
const byte MaxColumns = 230;

//==========================================================================
//
//
//
//==========================================================================

static double Drw_ProfCon2[] =
       {
       DD_Poly, -3.2,-3.2, -3.2,3.2, 3.2,3.2, 3.2,-3.2, -3.2,-3.2,
       DD_Poly, -3.2,0.0, -2.4,0.0, -2.4,1.6, -0.8,1.6, -0.8,-1.6, 0.0,-1.6, 0.0,-0.8, 1.6,-0.8, 1.6,0.0, 2.4,0.0, 2.4,2.4, 3.2,2.4, 
       DD_TagPos, 0, -6.7,
       DD_End
       };

IMPLEMENT_MODELUNIT(CProfileCon2, "Profile", "", Drw_ProfCon2, "Control", "PC", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,  
                    "Control:Profile",
                   "Profile Control model.")

flag CProfileCon2::bWithCnvComment = true;

//--------------------------------------------------------------------------

CProfileCon2::CProfileCon2(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach)
    //, m_MXRH(this)
  {
  fHasFiles=1;
  AttachClassInfo(nc_Control, NULL, NULL, &NullFlwGroup);
  fActiveHoldOK_Dyn=true;
  //bDoneXRefs = 0;
  bAboutToStart = 0;
  }

//--------------------------------------------------------------------------

CProfileCon2::~CProfileCon2()
  {
  }

//--------------------------------------------------------------------------

void CProfileCon2::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

const word idmCheckBtn  = 1000;
const word idmCfgTags   = 1100;

const word FirstColIndex = 22;

//--------------------------------------------------------------------------

void CProfileCon2::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  DDB.Text("");

  Strng Tag;
  DDB.CheckBoxBtn("On",          "",          DC_,     "",      idmCfgTags+4, this, isParm, DDBYesNo);
  DDB.CheckBoxBtn("TagsInHeading","",         DC_,     "",      idmCfgTags+19,this, isParmStopped, DDBYesNo);
  DDB.String("Folder",           "",          DC_,     "",      idmCfgTags+5, this, isResult);//isParm);
  DDB.String("File",             "",          DC_,     "",      idmCfgTags+6, this, isParmStopped);
  DDB.String("Status",           "",          DC_,     "",      &(PCI.sStatus), this, isResult|noFileAtAll);
  DDB.Button("Edit",             "",          DC_,     "",      idmCfgTags+7, this, isParm);
  DDB.Button("Browse",           "",          DC_,     "",      idmCfgTags+8, this, isParmStopped);
  //DDB.Button("Execute_Now",      "",          DC_,     "",      idmCfgTags+9, this, isParm);
  DDB.Button("Reload_Restart",   "",          DC_,     "",      idmCfgTags+10,this, isParm);
  DDB.Button("Restart",          "",          DC_,     "",      idmCfgTags+11,this, isParm);
  DDB.Text("");
  DDB.CheckBoxBtn("ShowCnv",      "",         DC_, "", &bWithCnvComment, this, isParmStopped, DDBYesNo);
  DDB.Button("Check_tags"/* and functions"*/, "", DC_, "", idmCheckBtn, this, isParmStopped);
  DDB.Text("");
  DDB.CheckBoxBtn("WithGainOffset","",        DC_,     "",      idmCfgTags+20,this, isParmStopped, DDBYesNo);
  DDB.CheckBoxBtn("Linear",      "",          DC_,     "",      idmCfgTags+12,this, isParm, DDBYesNo);
  DDB.CheckBoxBtn("WrapArround", "",          DC_,     "",      idmCfgTags+13,this, isParm, DDBYesNo);
  DDB.Double("TimePassed",       "",          DC_Time, "s",     idmCfgTags+18,this, isResult|noFileAtAll);
  DDB.Byte  ("TagCount",         "",          DC_,     "",      idmCfgTags+1, this, isParmStopped);
  DDB.Text("");
  if (PCI.iColCnt>0)
    DDB.Page("P0", DDB_RqdPage);
  if (DDB.BeginArray(this, "Item", "ProfileTags", PCI.iColCnt))
    {
    for (int j=0; j<PCI.iColCnt; j++)
      {
      if ((j-10)%20==0)
        {
        char Pg[16];
        sprintf(Pg, "P%d", j);
        DDB.Page(Pg, DDB_RqdPage);
        }
      DDB.BeginElement(this, j);
      Strng CnvTxt;
      if (bWithCnvComment && XRefsValid() && PCI.bValid)
        GetValidCnvTxt(PCI.ColData[j]->m_OutputVar, CnvTxt);
      //sprintf(Buff, "Tag%d", j+1);
      DDB.String("Output_Tag",             "",          DC_,     "",      idmCfgTags+FirstColIndex+j, this, (PCI.bUseHeadingRow ? isResult : isParmStopped)|isTag);
      //sprintf(Buff, "Val%d", j+1);
      if (PCI.bUseGainOffset)
        {
        DDB.Double("Gain",             "",          DC_,     "",      &(PCI.ColData[j]->m_dGain),      this, isParm);
        DDB.Double("Offset",           "",          DC_,     "",      &(PCI.ColData[j]->m_dOffset),    this, isParm);
        }
      DDB.Double("Value",            "",          DC_,     "",      &(PCI.ColData[j]->m_dOutput),    this, isResult);
      if (CnvTxt.Len())
        DDB.TagComment(CnvTxt());
      }
    }
  DDB.EndArray();
  if (PCI.iColCnt>0)
    {//NBNB: This assumes ALL profiles have same time basis length (ie will NOT recover properly if different columns have different lengths!!!)
    DDB.Byte  ("Started1",       "",          DC_,     "",      idmCfgTags+14,this, isResult|noView);
    DDB.Double("StartTime1",     "",          DC_Time, "s",     idmCfgTags+15,this, isResult|noView);
    DDB.Double("LastTime1",      "",          DC_Time, "s",     idmCfgTags+16,this, isResult|noView);
    DDB.Long  ("CurIndex1",      "",          DC_,     "",      idmCfgTags+17,this, isResult|noView);
    }
  DDB.Text("");
  
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CProfileCon2::DataXchg(DataChangeBlk & DCB)
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
        bAboutToStart = 0;
        }
      DCB.B=0;
      return True;
    default:
      if (DCB.lHandle>=idmCfgTags)
        {
        const int SubIndex = DCB.lHandle - idmCfgTags;
        switch (SubIndex)
          {
          case 0:
            if (DCB.rpC)
              {
              if (PCI.sTagSuffix.IsEmpty() || PCI.sTagSuffix.XStrICmp(DCB.rpC)!=0)
                {
                StructureChanged(this);
                //bDoneXRefs = 0;
                }
              if (strlen(DCB.rpC)) PCI.sTagSuffix = DCB.rpC;
              }
            DCB.pC = PCI.sTagSuffix();
            break;
          case 1:
            if (DCB.rB)
              {
              byte Prev = PCI.iColCnt;
              PCI.SetColCnt(*DCB.rB, MaxColumns);
              if (PCI.iColCnt!=Prev)
                {
                MyTagsHaveChanged();
                }
              }
            DCB.B=PCI.iColCnt;
            break;
          case 4:
            if (DCB.rB)
              {
              bool WasOff = (PCI.bOn==0); 
              PCI.bOn = (*DCB.rB);
              if (PCI.bOn && WasOff)
                {
                MyTagsHaveChanged();
                }
              }
            DCB.B = PCI.bOn;
            break;
          case 5:
            if (DCB.rpC!=NULL)
              {
              // Dont Set if coming from snapshot ???
              PCI.sPath = DCB.rpC;
              PCI.sPath.FnContract();
              }
            if (DCB.ForFileSnpScn())
              {
              PCI.sPath.FnContract();//ensure it is always in contract form!
              DCB.pC = PCI.sPath();
              }
            else
              {
              PCI.sPathForView = PCI.sPath;
              if (SymbolicPaths())
                PCI.sPathForView.FnContract();
              else
                PCI.sPathForView.FnExpand();
              DCB.pC = PCI.sPathForView();
              }
            return 1;
          case 6:
            if (DCB.rpC)
              {
              Strng Prev(PCI.sFile);
              PCI.sFile = DCB.rpC;
              if (Prev!=PCI.sFile)
                PCI.bReloadRqd = 1;
              }
            DCB.pC = PCI.sFile();
            return 1;
          case 7:
            if (DCB.rB && (*DCB.rB!=0))
              DCB.DoEditBtn(PCI.hProcess, PCI.dwProcessId, PCI.sPath(), PCI.sFile(), NULL/*"csv"*/);
            DCB.B=0;
            return 1;
          case 8:
            if (DCB.rB && (*DCB.rB!=0))
              {
              Strng Prev(PCI.sPath);
              Prev += PCI.sFile;
              DCB.DoBrowseBtn(NULL/*"csv"*/, "SysCAD Profile", PCI.sPath, PCI.sFile);
              Strng Next(PCI.sPath);
              Next += PCI.sFile;
              if (Prev!=Next)
                PCI.bReloadRqd = 1;
              }
            DCB.B=0;
            return 1;
          case 10:
            if (DCB.rB && (*DCB.rB!=0))
              DoLoad();
            DCB.B=0;
            return 1;
          case 11: //restart
            if (DCB.rB && (*DCB.rB!=0))
              PCI.Prof.StartAll(ICGetTime());
            DCB.B=0;
            return 1;
          case 12:
            if (DCB.rB)
              {
              PCI.bLinear = (*DCB.rB);
              PCI.Prof.SetAllOptions(PCI.bLinear, PCI.bWrapArround);
              }
            DCB.B = PCI.bLinear;
            break;
          case 13:
            if (DCB.rB)
              {
              PCI.bWrapArround = (*DCB.rB);
              PCI.Prof.SetAllOptions(PCI.bLinear, PCI.bWrapArround);
              }
            DCB.B = PCI.bWrapArround;
            break;
          case 14:
            if (DCB.rB)
              {
              if (PCI.bReloadRqd)
                DoLoad();
              for (byte j=0; j<PCI.Prof.ColCnt(); j++)
                PCI.Prof.pInfo[j].bStarted = (*DCB.rB);
              }
            DCB.B = PCI.Prof.ColCnt() ? PCI.Prof.Started(0) : 0;
            break;
          case 15:
            if (DCB.rD)
              {
              if (PCI.bReloadRqd)
                DoLoad();
              for (byte j=0; j<PCI.Prof.ColCnt(); j++)
                PCI.Prof.pInfo[j].dStartTime = (*DCB.rD);
              }
            DCB.D = PCI.Prof.ColCnt() ? PCI.Prof.StartTime(0).Seconds : 0.0;
            break;
          case 16:
            if (DCB.rD)
              {
              if (PCI.bReloadRqd)
                DoLoad();
              for (byte j=0; j<PCI.Prof.ColCnt(); j++)
                PCI.Prof.pInfo[j].dLastTime = (*DCB.rD);
              }
            DCB.D = PCI.Prof.ColCnt() ? PCI.Prof.LastTime(0).Seconds : 0.0;
            break;
          case 17: //NBNB: order is important, this must be last
            if (DCB.rL)
              {
              if (PCI.bReloadRqd)
                DoLoad();
              for (byte j=0; j<PCI.Prof.ColCnt(); j++)
                PCI.Prof.pInfo[j].iCurIndex = (WORD)(*DCB.rL);
              if (PCI.Prof.bLoaded)
                {//required to recover properly
                for (j=0; j<PCI.Prof.Data.GetCols(); j++)
                  PCI.Prof.pInfo[j].bJustLoaded = 1;
                }

              }
            DCB.L = PCI.Prof.ColCnt() ? PCI.Prof.CurIndex(0) : 0;
            break;
          case 18:
            //if (DCB.rD)
            //  ???
            DCB.D = PCI.Prof.ColCnt() ? PCI.Prof.TimePassed(0).Seconds : 0.0;
            break;
          case 19:
            if (DCB.rB)
              {
              flag Prev = PCI.bUseHeadingRow;
              PCI.bUseHeadingRow = (*DCB.rB);
              if (Prev != PCI.bUseHeadingRow && PCI.bUseHeadingRow)
                {
                PCI.bReloadRqd = 1;
                }
              }
            DCB.B = PCI.bUseHeadingRow;
            break;
          case 20:
            if (DCB.rB)
              {
              PCI.bUseGainOffset = (*DCB.rB);
              }
            DCB.B = PCI.bUseGainOffset;
            break;
          default:
            if (SubIndex>=FirstColIndex)
              {
              if (DCB.rpC && !PCI.bUseHeadingRow)
                {
                flag Chg = (PCI.ColData[SubIndex-FirstColIndex]->m_OutputVar.DoDataXchg(DCB)==1);
                if (Chg)
                  {
                  MyTagsHaveChanged();
                  }
                }
              DCB.pC = PCI.ColData[SubIndex-FirstColIndex]->m_OutputVar.sVar();
              }
            break;
          }//end switch
        }
    return True;
    }//end switch

  return False;
  }

//--------------------------------------------------------------------------

flag CProfileCon2::ValidateData(ValidateDataBlk & VDB)
  {
  TaggedObject::ValidateTag(PCI.sTagSuffix);
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

void CProfileCon2::DoLoad()
  {
  PCI.DoLoad();

  if (PCI.bUseHeadingRow)
    {
    bool AnyChg = false;
    for (int j=0; j<PCI.iColCnt; j++)
      {
      if (j+1<PCI.Prof.ColCnt())
        {
        Strng s = PCI.Prof.pInfo[j+1].sColTagName();
        flag Chg = (PCI.ColData[j]->m_OutputVar.SetVar(s() ? s() : "")==1);
        AnyChg = AnyChg || Chg;
        }
      }
    if (AnyChg)
      {
      MyTagsHaveChanged();
      }

    }
  }

//--------------------------------------------------------------------------

flag CProfileCon2::PreStartCheck()
  {
  bAboutToStart = 1;
  return FlwNode::PreStartCheck();
  }

//--------------------------------------------------------------------------

bool CProfileCon2::TestXRefListActive() { return SetXRefListActive(!GetActiveHold() && PCI.bOn!=0); }

//---------------------------------------------------------------------------

int CProfileCon2::UpdateXRefLists(CXRefBuildResults & Results)
  {
  FnMngrClear();
  int FunctNo = 0;
  if (PCI.bOn)
    {
    PCI.bValid = 1;
    for (int j=0; j<PCI.iColCnt; j++)
      {
      Strng S;
      S.Set("%s.Out%i", Tag(), j);
      int RetCode = PCI.ColData[j]->m_OutputVar.UpdateXRef(&PCI, 1, 0, FunctNo, this, -1, S(), S(), "ProfileCon:Output", Results);
      if (RetCode!=BXR_OK)
        PCI.bValid = 0;
      }
    }
  FnMngrTryUpdateXRefLists(Results);
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void CProfileCon2::UnlinkAllXRefs()
  {
  FnMngrClear();
  FnMngrTryUnlinkAllXRefs();
  for (int j=0; j<PCI.iColCnt; j++)
    PCI.ColData[j]->m_OutputVar.UnlinkXRefs();
  CNodeXRefMngr::UnlinkAllXRefs();
  }

//--------------------------------------------------------------------------

void CProfileCon2::EvalCtrlInitialise(eScdCtrlTasks Tasks)
  {
  if (Tasks&CO_InitPrf)
    {
    //DoLoad();
    PCI.Prof.StartAll(ICGetTime());
    }
  };

//--------------------------------------------------------------------------

void CProfileCon2::EvalCtrlStrategy(eScdCtrlTasks Tasks)
  {
  if (XRefListActive() && ICGetTimeInc() > 0.0)
    {
    GetNearXRefValues();
    //for (int i=0; i<m_NearXRefs.GetSize(); i++)
    //  if (m_NearXRefs[i]->bMustGet)
    //    m_NearXRefs[i]->GetNearXRefValue();

    //if (bAboutToStart && bResetOnStart)
    //  ResetAllStats();
    //dbgpln("ProfConInfo::ExecIns %s", Tag());    
    if (FnMngrPresent())
      {
      //solve pgm functions...
      CGExecContext ECtx(this);
      ECtx.dIC_Time = ICGetTime();
      ECtx.dIC_dTime = ICGetTimeInc();
      ECtx.OnStart = bAboutToStart;
      ECtx.m_HoldNearXRefGet=true;
      ECtx.m_HoldNearXRefSet=true;
      FnMngr().Execute(ECtx);
      bAboutToStart = 0;
      if (ECtx.DoXStop)
        {
        LogError(Tag(), 0, "SysCAD stopped by function");
        ExecObj()->XStop();
        }
      if (ECtx.DoXIdle)
        {
        LogError(Tag(), 0, "SysCAD paused by function");
        ExecObj()->XIdle();
        }
      }

    //solve DataBlks...
    if (PCI.bValid && PCI.bOn)
      {
      PCI.ExecIns(ICGetTime());
      for (int j=0; j<PCI.iColCnt; j++)
        if (Valid(PCI.ColData[j]->m_dOutput) || !HasNANFlag(PCI.ColData[j]->m_dOutput, NF_Free))
          PCI.ColData[j]->m_OutputVar.PutValue(PCI.ColData[j]->m_dOutput);
      }
    SetNearXRefValues();
    //for (i=0; i<m_NearXRefs.GetSize(); i++)
    //  m_NearXRefs[i]->SetNearXRefValue();
    }
  }

//--------------------------------------------------------------------------

void CProfileCon2::SetState(eScdMdlStateActs RqdState)
  {
  FlwNode::SetState(RqdState);
  flag DoReset = false;
  switch (RqdState)
    {
    case MSA_PBInit   : 
    case MSA_ZeroFlows: 
    case MSA_Empty    : 
    case MSA_PreSet   : break;
    case MSA_DynStatsRunInit: DoReset = true; break;
    default: break;
    }
  if (DoReset && PCI.bOn)
    {
    PCI.Prof.StartAll(ICGetTime());
    }

  }

//--------------------------------------------------------------------------

void CProfileCon2::EvalProducts(CNodeEvalIndex & NEI)
  {
  }

//--------------------------------------------------------------------------

int CProfileCon2::FilesUsed(CFilesUsedArray & Files)
  {
  int Cnt = 0;
  if (PCI.sFile.Len()>0)
    {
    Strng Fn(PCI.sPath);
    Fn.FnExpand();
    Fn += PCI.sFile;
    Files.AddFile(Fn(), FU_CopyFile|FU_EditTxt);
    Cnt++;
    }
  FlwNode::FilesUsed(Files);
  return Files.GetCount();
  //return Cnt;
  }

//--------------------------------------------------------------------------

int CProfileCon2::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  BOOL DidChange = FALSE;
  for (int j=0; j<PCI.iColCnt; j++)
    {
    if (PCI.ColData[j]->m_OutputVar.DoChangeTag(pOldTag, pNewTag))
      DidChange = TRUE;
    }
  if (DidChange)
    {
    UnlinkAllXRefs(); //PreStartCheck();
    bAboutToStart = 0;
    }
  return FlwNode::ChangeTag(pOldTag, pNewTag);
  }

//--------------------------------------------------------------------------

int CProfileCon2::DeleteTag(pchar pDelTag)
  {
  BOOL FoundOne = FALSE;
  for (int j=0; j<PCI.iColCnt; j++)
    {
    if (PCI.ColData[j]->m_OutputVar.ContainsTag(pDelTag))
      FoundOne = TRUE;
    }
  if (FoundOne)
    {
    LogNote(Tag(), 0, "Delete tag '%s' affects Profile Controller model '%s'.", pDelTag, Tag());
    UnlinkAllXRefs(); //PreStartCheck();
    bAboutToStart = 0;
    }
  return FlwNode::DeleteTag(pDelTag);
  }

//--------------------------------------------------------------------------
                                          
dword CProfileCon2::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=(PCI.bOn ? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag CProfileCon2::CIStrng(int No, pchar & pS)
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