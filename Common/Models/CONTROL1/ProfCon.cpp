//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __PROFCON_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "profiler.h"
#include "profcon.h"

#if !SKIPIT 

//==========================================================================

const long FunctSubsStartIndex = 500;
const byte MaxColumns   = 32;

//==========================================================================

ProfColInfo::ProfColInfo():
  m_OutputVar(BXO_Blank)
  {
  //m_OutputVar.Init(FALSE, TRUE);
  m_dOutput = 0.0;
  m_dGain   = 1.0;
  m_dOffset = 0.0;
  }

//--------------------------------------------------------------------------
//==========================================================================

ProfConInfo::ProfConInfo()
  {
  bOn = 1;
  bValid = 0;
  bReloadRqd = 1;
  bLinear = 0;
  bWrapArround = 1;
  bUseHeadingRow = 0;
  bUseGainOffset = 0;
  iPriority = 0;
  sPath = PrjFiles();
  hProcess = NULL;
  dwProcessId = 0;
  ColData = NULL;
  iColCnt = 0;
  SetColCnt(1, MaxColumns);
  }

//--------------------------------------------------------------------------

ProfConInfo::~ProfConInfo()
  {
  SetColCnt(0, MaxColumns);
  }

//--------------------------------------------------------------------------

void ProfConInfo::Init(int iNo)
  {
  sTagSuffix.Set("PC_%d", iNo);
  iPriority = iNo;
  //pXRM = ERH;
  }

//--------------------------------------------------------------------------

byte ProfConInfo::SetColCnt(byte NewSize, byte MaxAllowed)
  {
  NewSize = Range((byte)0, NewSize, MaxAllowed);
  if (NewSize!=iColCnt)
    {
    ProfColInfo** NewColData = NULL;
    if (NewSize<iColCnt)
      {
      for (int i=NewSize; i<iColCnt; i++)
        delete ColData[i];
      }
    if (NewSize>0)
      {
      NewColData = new pProfColInfo[NewSize];
      for (int i=0; (i<NewSize && i<iColCnt); i++)
        NewColData[i] = ColData[i];
      for (i=iColCnt; i<NewSize; i++)
        {
        NewColData[i] = new ProfColInfo;
        //NewColData[i]->Init(this, i);
        }
      }
    if (ColData)
      delete []ColData;
    ColData = NewColData;
    iColCnt = NewSize;
    }
  return iColCnt;
  }

//--------------------------------------------------------------------------

flag ProfConInfo::DoLoad()
  {
  bReloadRqd = 0;
  Strng Fn(sPath);
  Fn.FnExpand();
  Fn.FnCheckEndBSlash();
  Fn += sFile;
  if (FileExists(Fn()))
    {
    int RetCode = Prof.Load(Fn(), bUseHeadingRow);
    switch (RetCode)
      {
      case 0: 
        {
        const double tm = Prof.FullTime(0);
        if (tm>86400.0)
          sStatus.Set("OK (%d columns; %d rows; %.0fhours)", Prof.ColCnt(), Prof.RowCnt(0), tm/3600.0);
        else if (tm>3600.0)
          sStatus.Set("OK (%d columns; %d rows; %.0fminutes)", Prof.ColCnt(), Prof.RowCnt(0), tm/60.0);
        else
          sStatus.Set("OK (%d columns; %d rows; %.0fseconds)", Prof.ColCnt(), Prof.RowCnt(0), tm);
        Prof.SetAllOptions(bLinear, bWrapArround);
        return true;
        }
      case 1: sStatus = "Cannot open or load the specified file"; break;
      case 2: sStatus = "No data rows found"; break;
      case 3: sStatus = "Expect two or more columns"; break;
      case 4: sStatus = "Time in first column is not incrementing"; break;
      case 5: sStatus = "Empty column not allowed"; break;
      case 6: sStatus = "Time in first column is Invalid(NAN)"; break;
      default: sStatus = "Profile load error"; break;
      }
    }
  else
    sStatus = "Specified file not found!";
  return false;
  }

//--------------------------------------------------------------------------

void ProfConInfo::ExecIns(double ICTime)
  {
  if (bReloadRqd)
    DoLoad();
  if (Prof.Loaded())
    {
    Prof.GetVal(ICTime, 0);
    //dbgpln("            Time = %10.2f Passed : %10.2f ", ICTime, Prof.TimePassed());
    for (int j=0; j<iColCnt; j++)
      {
      ProfColInfo &C=*ColData[j];
      C.m_dOutput = Prof.GetVal(ICTime, j+1);
      if (bUseGainOffset && Valid(C.m_dOutput))
        C.m_dOutput=C.m_dOffset+C.m_dOutput*C.m_dGain;
      //dbgpln("            %i) %10.2f %s", j, ColData[j]->m_dOutput, ColData[j]->m_OutputVar.sVar());
      }
    }
  }

//==========================================================================
//
//
//
//==========================================================================

static double Drw_ProfCon[] =
       {
       DD_Poly, -4,-4, -4,4, 4,4, 4,-4, -4,-4, 
       DD_Poly, -4,0, -3,0, -3,2, -1,2, -1,-2, 0,-2, 0,-1, 2,-1, 2,0, 3,0, 3,3, 4,3, 
       DD_End
       };

IMPLEMENT_MODELUNIT(CProfileCon, "ProfCon", "", Drw_ProfCon, "Control", "PC", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,  
                    "Control:Profile(old)",
                   "Profile Control model.")

flag CProfileCon::bWithCnvComment = true;

//--------------------------------------------------------------------------

CProfileCon::CProfileCon(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach)
    //, m_MXRH(this)
  {
  fHasFiles=1;
  AttachClassInfo(nc_Control, NULL, NULL, &NullFlwGroup);
  fActiveHoldOK_Dyn=true;
  bOn = 1;
  //bDoneXRefs = 0;
  bAboutToStart = 0;
  iCount = 0;
  DataBlk = NULL;
  for (int i=0; i<3; i++)
    m_StateLine[i] = "";
  m_StateLine[0] = "OK";
  SetCount(1);
  }

//--------------------------------------------------------------------------

CProfileCon::~CProfileCon()
  {
  SetCount(0);
  }

//--------------------------------------------------------------------------

void CProfileCon::SetCount(long NewSize)
  {
  NewSize = max(NewSize, 0L);
  if (NewSize!=iCount)
    {
    ProfConInfo** NewDataBlk = NULL;
    if (NewSize<iCount)
      {
      for (int i=NewSize; i<iCount; i++)
        delete DataBlk[i];
      }
    if (NewSize>0)
      {
      NewDataBlk = new pProfConInfo[NewSize];
      for (int i=0; (i<NewSize && i<iCount); i++)
        NewDataBlk[i] = DataBlk[i];
      for (i=iCount; i<NewSize; i++)
        {
        NewDataBlk[i] = new ProfConInfo;
        NewDataBlk[i]->Init(i);
        }
      }
    if (DataBlk)
      delete []DataBlk;
    DataBlk = NewDataBlk;
    iCount = NewSize;
    if (SortRqd())
      Sort();
    StructureChanged(this);
    }
  }

//--------------------------------------------------------------------------

flag CProfileCon::SortRqd()
  {
  for (int i=1; i<iCount; i++)
    {
    if (DataBlk[i-1]->iPriority>DataBlk[i]->iPriority)
      return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

void CProfileCon::Sort()
  {
  for (int i=0; i<iCount; i++)
    {
    int MinPos = -1;
    long MinVal = DataBlk[i]->iPriority;
    for (int j=i+1; j<iCount; j++)
      {
      if (DataBlk[j]->iPriority<MinVal)
        {
        MinPos = j;
        MinVal = DataBlk[j]->iPriority;
        }
      }
    if (MinPos>=0)
      {
      ProfConInfo* p = DataBlk[i];
      DataBlk[i] = DataBlk[MinPos];
      DataBlk[MinPos] = p;
      }
    }
  StructureChanged(this);
  }

//--------------------------------------------------------------------------

void CProfileCon::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

void CProfileCon::RestartAll()
  {
  for (int i=0; i<iCount; i++)
    {
    ProfConInfo* p = DataBlk[i];
    if (p->bOn)
      {
      p->Prof.StartAll(ICGetTime());
      }
    }
  }

//--------------------------------------------------------------------------

const word idmCount     = 1000;
const word idmCheckBtn  = 1001;
const word idmCfgTags   = 1100;

const word FirstColIndex = 20;
const word NoOfCfgTags = 2+FirstColIndex+MaxColumns;

//--------------------------------------------------------------------------

void CProfileCon::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CProfileCon", NULL, DDB_NoPage);
  DDB.Text("");
  DDB.CheckBoxBtn("On",           "",         DC_, "", &bOn,             this, isParmStopped, DDBYesNo);
  DDB.CheckBoxBtn("ShowCnv",      "",         DC_, "", &bWithCnvComment, this, isParmStopped, DDBYesNo);
  DDB.Long("Number_of_Blocks",    "No_of_Blks", DC_, "", idmCount,       this, isParmStopped/*|AffectsStruct*/);
  DDB.Text("");
  if (iCount>0)
    {
    DDB.Button("Check_tags"/* and functions"*/, "", DC_, "", idmCheckBtn, this, isParmStopped);
    DDB.Text("");
    DDB.String("State",        "", DC_,    "",    &m_StateLine[0],  this, noSnap|noFile);
    DDB.Text("Error:");
    DDB.String("Msg_1",        "", DC_,    "",    &m_StateLine[1],  this, noSnap|noFile);
    DDB.String("Msg_2",        "", DC_,    "",    &m_StateLine[2],  this, noSnap|noFile);
    DDB.Text("");
    }
  DDB.Text("----------------------------------------");

  char Buff[128];
  Strng Tag;
  if (DDB.BeginArray(this, "Cfg", "Prof_Cfg", iCount))
    {
    for (int i=0; i<iCount; i++)
      {
      if (i>0 && (i % 3)==1)
        {
        sprintf(Buff, "Cfg_%d", i);
        DDB.Page(Buff, DDB_RqdPage);
        }
      DDB.BeginElement(this, i);
      ProfConInfo* p = DataBlk[i];
      //DDB.String("Name",             "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+0, this, isParmStopped); not needed?
      DDB.CheckBoxBtn("On",          "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+4, this, isParm, DDBYesNo);
      DDB.String("Status",           "",          DC_,     "",      &(p->sStatus),                this, isResult|noFileAtAll);
      DDB.String("Folder",           "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+5, this, isResult);//isParm);
      DDB.String("File",             "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+6, this, isParm);
      DDB.Button("Edit",             "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+7, this, isParm);
      DDB.Button("Browse",           "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+8, this, isParm);
      //DDB.Button("Execute_Now",      "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+9, this, isParm);
      DDB.Button("Reload_Restart",   "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+10,this, isParm);
      DDB.Button("Restart",          "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+11,this, isParm);
      DDB.CheckBoxBtn("Linear",      "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+12,this, isParm, DDBYesNo);
      DDB.CheckBoxBtn("WrapArround", "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+13,this, isParm, DDBYesNo);
      DDB.Double("TimePassed",       "",          DC_Time, "s",     idmCfgTags+(i*NoOfCfgTags)+18,this, isResult|noFileAtAll);
      DDB.Byte  ("TagCount",         "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+1, this, isParmStopped);
      for (int j=0; j<p->iColCnt; j++)
        {
        Strng CnvTxt;
        if (bWithCnvComment && XRefsValid() && p->bValid)
          GetValidCnvTxt(p->ColData[j]->m_OutputVar, CnvTxt);
        sprintf(Buff, "Output_Tag%d", j+1);
        DDB.String(Buff,             "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+FirstColIndex+j, this, isParmStopped|isTag);
        sprintf(Buff, "Output%d", j+1);
        DDB.Double(Buff,             "",          DC_,     "",      &(p->ColData[j]->m_dOutput),    this, isResult);
        if (CnvTxt.Len())
          DDB.TagComment(CnvTxt());
        }
      if (p->iColCnt>0)
        {//NBNB: This assumes ALL profiles have same time basis length (ie will NOT recover properly if different columns have different lengths!!!)
        DDB.Byte  ("Started1",       "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+14,this, isResult|noView);
        DDB.Double("StartTime1",     "",          DC_Time, "s",     idmCfgTags+(i*NoOfCfgTags)+15,this, isResult|noView);
        DDB.Double("LastTime1",      "",          DC_Time, "s",     idmCfgTags+(i*NoOfCfgTags)+16,this, isResult|noView);
        DDB.Long  ("CurIndex1",      "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+17,this, isResult|noView);
        }
      DDB.Text("");
      DDB.Long  ("Priority",         "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+3, this, isParmStopped);
      //DDB.Text("");
      DDB.Text("----------------------------------------");
      }
    }
  DDB.EndArray();
  
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CProfileCon::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  switch (DCB.lHandle)
    {
    case idmCount:
      if (DCB.rL)
        {
        if (*DCB.rL!=iCount)
          {
          StructureChanged(this);
          SetCount(Range(0L, *DCB.rL, 50L));
          }
        }
      DCB.L = iCount;
      return True;
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
        int Index = (DCB.lHandle - idmCfgTags) / NoOfCfgTags;
        if (Index<iCount)
          {
          ProfConInfo* p = DataBlk[Index];
          const int SubIndex = (DCB.lHandle - idmCfgTags) - (Index * NoOfCfgTags);
          switch (SubIndex)
            {
            case 0:
              if (DCB.rpC)
                {
                if (p->sTagSuffix.IsEmpty() || p->sTagSuffix.XStrICmp(DCB.rpC)!=0)
                  {
                  StructureChanged(this);
                  //bDoneXRefs = 0;
                  }
                if (strlen(DCB.rpC)) p->sTagSuffix = DCB.rpC;
                }
              DCB.pC = p->sTagSuffix();
              break;
            case 1:
              if (DCB.rB)
                {
                byte Prev = p->iColCnt;
                p->SetColCnt(*DCB.rB, MaxColumns);
                if (p->iColCnt!=Prev)
                  {
                  MyTagsHaveChanged();
                  }
                }
              DCB.B=p->iColCnt;
              break;
            case 3:
              if (DCB.rL)
                {
                p->iPriority = *DCB.rL;
                if (SortRqd())
                  {
                  Sort();
                  }
                }
              DCB.L = p->iPriority;
              break;
            case 4:
              if (DCB.rB)
                {
                bool WasOff = (p->bOn==0); 
                p->bOn = (*DCB.rB);
                if (p->bOn && WasOff)
                  {
                  MyTagsHaveChanged();
                  }
                }
              DCB.B = p->bOn;
              break;
            case 5:
              if (DCB.rpC!=NULL)
                {
                // Dont Set if coming from snapshot ???
                p->sPath = DCB.rpC;
                p->sPath.FnContract();
                }
              if (DCB.ForFileSnpScn())
                {
                p->sPath.FnContract();//ensure it is always in contract form!
                DCB.pC = p->sPath();
                }
              else
                {
                p->sPathForView = p->sPath;
                if (SymbolicPaths())
                  p->sPathForView.FnContract();
                else
                  p->sPathForView.FnExpand();
                DCB.pC = p->sPathForView();
                }
              return 1;
            case 6:
              if (DCB.rpC)
                {
                Strng Prev(p->sFile);
                p->sFile = DCB.rpC;
                if (Prev!=p->sFile)
                  p->bReloadRqd = 1;
                }
              DCB.pC = p->sFile();
              return 1;
            case 7:
              if (DCB.rB && (*DCB.rB!=0))
                DCB.DoEditBtn(p->hProcess, p->dwProcessId, p->sPath(), p->sFile(), NULL/*"csv"*/);
              DCB.B=0;
              return 1;
            case 8:
              if (DCB.rB && (*DCB.rB!=0))
                {
                Strng Prev(p->sPath);
                Prev += p->sFile;
                DCB.DoBrowseBtn(NULL/*"csv"*/, "SysCAD Profile", p->sPath, p->sFile);
                Strng Next(p->sPath);
                Next += p->sFile;
                if (Prev!=Next)
                  p->bReloadRqd = 1;
                }
              DCB.B=0;
              return 1;
            case 10:
              if (DCB.rB && (*DCB.rB!=0))
                p->DoLoad();
              DCB.B=0;
              return 1;
            case 11: //restart
              if (DCB.rB && (*DCB.rB!=0))
                p->Prof.StartAll(ICGetTime());
              DCB.B=0;
              return 1;
            case 12:
              if (DCB.rB)
                {
                p->bLinear = (*DCB.rB);
                p->Prof.SetAllOptions(p->bLinear, p->bWrapArround);
                }
              DCB.B = p->bLinear;
              break;
            case 13:
              if (DCB.rB)
                {
                p->bWrapArround = (*DCB.rB);
                p->Prof.SetAllOptions(p->bLinear, p->bWrapArround);
                }
              DCB.B = p->bWrapArround;
              break;
            case 14:
              if (DCB.rB)
                {
                if (p->bReloadRqd)
                  p->DoLoad();
                for (byte j=0; j<p->Prof.ColCnt(); j++)
                  p->Prof.pInfo[j].bStarted = (*DCB.rB);
                }
              DCB.B = p->Prof.ColCnt() ? p->Prof.Started(0) : 0;
              break;
            case 15:
              if (DCB.rD)
                {
                if (p->bReloadRqd)
                  p->DoLoad();
                for (byte j=0; j<p->Prof.ColCnt(); j++)
                  p->Prof.pInfo[j].dStartTime = (*DCB.rD);
                }
              DCB.D = p->Prof.ColCnt() ? p->Prof.StartTime(0).Seconds : 0.0;
              break;
            case 16:
              if (DCB.rD)
                {
                if (p->bReloadRqd)
                  p->DoLoad();
                for (byte j=0; j<p->Prof.ColCnt(); j++)
                  p->Prof.pInfo[j].dLastTime = (*DCB.rD);
                }
              DCB.D = p->Prof.ColCnt() ? p->Prof.LastTime(0).Seconds : 0.0;
              break;
            case 17: //NBNB: order is important, this must be last
              if (DCB.rL)
                {
                if (p->bReloadRqd)
                  p->DoLoad();
                for (byte j=0; j<p->Prof.ColCnt(); j++)
                  p->Prof.pInfo[j].iCurIndex = (WORD)(*DCB.rL);
                if (p->Prof.bLoaded)
                  {//required to recover properly
                  for (j=0; j<p->Prof.Data.GetCols(); j++)
                    p->Prof.pInfo[j].bJustLoaded = 1;
                  }

                }
              DCB.L = p->Prof.ColCnt() ? p->Prof.CurIndex(0) : 0;
              break;
            case 18:
              //if (DCB.rD)
              //  ???
              DCB.D = p->Prof.ColCnt() ? p->Prof.TimePassed(0).Seconds : 0.0;
              break;
            default:
              if (SubIndex>=FirstColIndex)
                {
                flag Chg = (p->ColData[SubIndex-FirstColIndex]->m_OutputVar.DoDataXchg(DCB)==1);
                if (Chg)
                  {
                  MyTagsHaveChanged();
                  }
                DCB.pC = p->ColData[SubIndex-FirstColIndex]->m_OutputVar.sVar();
                }
              break;
            }
          }
        return True;
        }
    }

  return False;
  }

//--------------------------------------------------------------------------

flag CProfileCon::ValidateData(ValidateDataBlk & VDB)
  {
  for (int i=0; i<iCount; i++)
    {
    TaggedObject::ValidateTag(DataBlk[i]->sTagSuffix);
    }
  Sort();
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

flag CProfileCon::PreStartCheck()
  {
  for (int i=0; i<3; i++)
    m_StateLine[i] = "";
  m_StateLine[0] = "OK";
  if (bOn)
    {
    bAboutToStart = 1;
    }
  return FlwNode::PreStartCheck();
  }

//---------------------------------------------------------------------------

bool CProfileCon::TestXRefListActive() { return SetXRefListActive(!GetActiveHold() && bOn!=0); }

//---------------------------------------------------------------------------

int CProfileCon::UpdateXRefLists(CXRefBuildResults & Results)
  {
  if (1)//bOn)
    {
    FnMngrClear();
    int FunctNo = 0;
    for (int i=0; i<iCount; i++)
      {
      ProfConInfo* p = DataBlk[i];
      if (p->bOn)
        {
        p->bValid = 1;
        for (int j=0; j<p->iColCnt; j++)
          {
          Strng S;
          S.Set("%s.Cfg.[%i].Out", Tag(), i);
          int RetCode = p->ColData[j]->m_OutputVar.UpdateXRef(p, 1, 0, FunctNo, this, -1, S(), S(), "ProfileCon:Output", Results);
          if (RetCode!=BXR_OK)
            p->bValid = 0;
          }
        }
      }
    FnMngrTryUpdateXRefLists(Results);
    }
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void CProfileCon::UnlinkAllXRefs()
  {
  FnMngrClear();
  FnMngrTryUnlinkAllXRefs();
  for (int i=0; i<iCount; i++)
    {
    ProfConInfo* p = DataBlk[i];
    for (int j=0; j<p->iColCnt; j++)
      p->ColData[j]->m_OutputVar.UnlinkXRefs();
    }

  CNodeXRefMngr::UnlinkAllXRefs();
  };

//--------------------------------------------------------------------------

void CProfileCon::EvalCtrlInitialise(eScdCtrlTasks Tasks)
  {
  if (Tasks&CO_InitPrf)
    {
    for (int i=0; i<iCount; i++)
      //DataBlk[i]->DoLoad();
      DataBlk[i]->Prof.StartAll(ICGetTime());
    }
  };

//--------------------------------------------------------------------------

void CProfileCon::EvalCtrlStrategy(eScdCtrlTasks Tasks)
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
      ECtx.HoldNearXRefXfer=true;
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
    for (int i=0; i<iCount; i++)
      {
      ProfConInfo* p = DataBlk[i];
      if (p->bValid && p->bOn)
        {
        p->ExecIns(ICGetTime());
        for (int j=0; j<p->iColCnt; j++)
          if (Valid(p->ColData[j]->m_dOutput) || !HasNANFlag(p->ColData[j]->m_dOutput, NF_Free))
            p->ColData[j]->m_OutputVar.PutValue(p->ColData[j]->m_dOutput);
        }
      }
    SetNearXRefValues();
    //for (i=0; i<m_NearXRefs.GetSize(); i++)
    //  m_NearXRefs[i]->SetNearXRefValue();
    }
  }

//--------------------------------------------------------------------------

void   CProfileCon::SetState(eScdMdlStateActs RqdState)
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
  if (DoReset)
    RestartAll();
  }

//--------------------------------------------------------------------------

void CProfileCon::EvalProducts(CNodeEvalIndex & NEI)
  {
  }

//--------------------------------------------------------------------------

int CProfileCon::FilesUsed(CFilesUsedArray & Files)
  {
  int Cnt = 0;
  for (int i=0; i<iCount; i++)
    {
    ProfConInfo* p = DataBlk[i];
    if (p->sFile.Len()>0)
      {
      Strng Fn(p->sPath);
      Fn.FnExpand();
      Fn += p->sFile;
      Files.AddFile(Fn(), FU_CopyFile|FU_EditTxt);
      Cnt++;
      }
    }
  return Cnt;
  }

//--------------------------------------------------------------------------

int CProfileCon::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  BOOL DidChange = FALSE;
  for (int i=0; i<iCount; i++)
    {
    ProfConInfo* p = DataBlk[i];
    for (int j=0; j<p->iColCnt; j++)
      {
      if (p->ColData[j]->m_OutputVar.DoChangeTag(pOldTag, pNewTag))
        DidChange = TRUE;
      }
    }
  if (DidChange)
    {
    UnlinkAllXRefs(); //PreStartCheck();
    bAboutToStart = 0;
    }
  return EOCT_DONE;
  }

//--------------------------------------------------------------------------

int CProfileCon::DeleteTag(pchar pDelTag)
  {
  BOOL FoundOne = FALSE;
  for (int i=0; i<iCount; i++)
    {
    ProfConInfo* p = DataBlk[i];
    for (int j=0; j<p->iColCnt; j++)
      {
      if (p->ColData[j]->m_OutputVar.ContainsTag(pDelTag))
        FoundOne = TRUE;
      }
    }
  if (FoundOne)
    {
    LogNote(Tag(), 0, "Delete tag '%s' affects Profile Controller model '%s'.", pDelTag, Tag());
    UnlinkAllXRefs(); //PreStartCheck();
    bAboutToStart = 0;
    }
  return EODT_DONE;
  }

//--------------------------------------------------------------------------
                                          
dword CProfileCon::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=bOn ? FNS_On : FNS_Off;
  return Status;
  }

//--------------------------------------------------------------------------

flag CProfileCon::CIStrng(int No, pchar & pS)
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