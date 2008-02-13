//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "queuecon.h"
//#include "optoff.h"

#if !SKIPIT 

//==========================================================================

const byte ReportToMsg = 0x01;
const byte ReportToEventLog = 0x02;

//==========================================================================
const short MaxColumns = 250;

QueueColInfo::QueueColInfo():
  m_OutputVar(BXO_Blank)
  {
  //m_OutputVar.Init(FALSE, TRUE);
  m_dOutput = 0.0;
  }

//--------------------------------------------------------------------------

QueueColInfo::~QueueColInfo()
  {
  }

//--------------------------------------------------------------------------
//==========================================================================

QueueConInfo::QueueConInfo(CQueueCon* Parent)
  {
  pParent = Parent;
  bOn = 1;
  bValid = 0;
  bLoaded = 0;
  bReloadRqd = 1;
  bWrapArround = 1;
  bUseHeadingRow = 0;
  bUseXRefs = 1;
  eLogSetTags = 0;
  iCurIndex = -1;
  iStartIndex = 0;
  sPath = PrjFiles();
  hProcess = NULL;
  dwProcessId = 0;
  ColData = NULL;
  iColCnt = 0;
  SetColCnt(1);
  }

//--------------------------------------------------------------------------

QueueConInfo::~QueueConInfo()
  {
  SetColCnt(0);
  }

//--------------------------------------------------------------------------

short QueueConInfo::SetColCnt(short NewSize)
  {
  NewSize = Range((short)0, NewSize, MaxColumns);
  if (NewSize!=iColCnt)
    {
    QueueColInfo** NewColData = NULL;
    if (NewSize<iColCnt)
      {
      for (int i=NewSize; i<iColCnt; i++)
        delete ColData[i];
      }
    if (NewSize>0)
      {
      NewColData = new pQueueColInfo[NewSize];
      for (int i=0; (i<NewSize && i<iColCnt); i++)
        NewColData[i] = ColData[i];
      for (i=iColCnt; i<NewSize; i++)
        {
        NewColData[i] = new QueueColInfo;
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

int QueueConInfo::DoLoad()
  {
  bReloadRqd = 0;
  bLoaded = 0;
  Strng Fn(sPath);
  Fn.FnExpand();
  Fn.FnCheckEndBSlash();
  Fn += sFile;
  if (FileExists(Fn()))
    {
    iColCnt = Range((short)1, iColCnt, MaxColumns);
    SetColCnt(iColCnt);
    CVMLoadHelper H;
    strcpy(H.FName, Fn());
    H.StartRow = 2; //ignore first row of titles
    H.Cols = iColCnt; //required number of columns
    if (!Data.Load(H, NF_Free))
      {
      sStatus = "Cannot open or load the specified file";
      return 1; //cannot open/load the file
      }
    long Rows = Data.GetRows();
    if (Rows==0)
      {
      sStatus = "No data rows found";
      return 2; //are no rows
      }
    long Cols = Data.GetCols();
    if (Cols<1)
      {
      sStatus = "Expect one or more columns";
      return 3; //must be at least one column
      }
    //long tt = Data.GetCols();
    //SetColCnt(iColCnt);
    //long Test = Min((long)iColCnt, Cols);
    //SetColCnt((short)Cols);

    for (long j=0; j<Cols; j++)
      {
      int i = 0;
      while (i<Rows && (Valid(Data[i][j]) || HasNANFlag(Data[i][j])))
        i++;
      if (i==0)
        {
        sStatus = "Empty column not allowed";
        return 5; //empty column not allowed
        }
      //ColData[j]->m_iLen = i;
      }

    //Data.dbgDump();
    if (bUseHeadingRow)
      {
      for (int i=0; i<iColCnt; i++)
        ColData[i]->m_sColTagName = "";
      FILE* f=fopen(Fn(), "rt");
      if (f)
        {
        char Buff[8192];
        if (fgets(Buff, sizeof(Buff)-1, f))
          {
          char* c[MaxCSVCols];
          int Quote;
          int nFlds = ParseCSVTokens(Buff, c, Quote);
          if (Quote)
            {
            //LogError("Quotes mismatched");
            }
          else
            {
            for (int i=0; i<Min(nFlds,(int)Cols); i++)
              {
              ColData[i]->m_sColTagName = (c[i] ? c[i] : "");
              ColData[i]->m_sColTagName.Trim();
              }
            }
          }
        fclose(f);
        }
      }
    sStatus.Set("OK (%d columns; %d rows)", Data.GetCols(), Data.GetRows());
    bLoaded = 1;
    }
  else
    sStatus = "Specified file not found!";

  return 0;
  }

//--------------------------------------------------------------------------

void QueueConInfo::SetTagsDirectly()
  {
  if (!bUseXRefs && iCurIndex>=0)
    {
    const long Cols = Data.GetCols();
    for (int j=0; j<Cols; j++)
      {
      Strng Tag = ColData[j]->m_OutputVar.sVar();
      if (Tag.Len())
        {
        const double d = ColData[j]->m_dOutput;
        int Ret = TryWriteTag(pParent, pParent->FamilyHead(), Tag(), d);
        if (Ret==FXR_Found)
          {
          if (eLogSetTags)
            {
            char Msg[1024];
            sprintf(Msg, "Row %d: Set '%s' to %g", iCurIndex, Tag(), d);
            if (eLogSetTags & ReportToMsg)
              LogNote(pParent->Tag(), 0, Msg);
            if (eLogSetTags & ReportToEventLog)
              gs_EventLog.LogEvent(pParent->Tag(), Msg);
            }
          }
        else
          {
          LogWarning(pParent->Tag(), 0, "Row %d: Failed to set tag '%s'!", iCurIndex, Tag());
          }
        }
      }
    }
  }

//--------------------------------------------------------------------------

void QueueConInfo::CheckTags()
  {
  if (!bUseXRefs)
    {
    bool FoundErr = false;
    const long Cols = Data.GetCols();
    for (int j=0; j<Cols; j++)
      {
      Strng Tag = ColData[j]->m_OutputVar.sVar();
      if (Tag.Len())
        {
        int Ret = TryTestTag(pParent, pParent->FamilyHead(), Tag());
        if (Ret!=FXR_Found)
          {
          LogError(pParent->Tag(), 0, "Col %d: Tag '%s' not found or not allowed!", j+1, Tag());
          FoundErr = true;
          }
        }
      else
        LogWarning(pParent->Tag(), 0, "Col %d: Tag not specified.", j+1);
      }
    if (!FoundErr)
      LogNote(pParent->Tag(), 0, "No bad tags references.");
    }
  }

//--------------------------------------------------------------------------

void QueueConInfo::SetIndex(short RqdIndex)
  {
  if (bReloadRqd)
    DoLoad();
  iCurIndex = Range((short)-1, RqdIndex, (short)(Data.GetRows()-1));
  if (iCurIndex>=0)
    {
    long SetCnt = 0;
    const long Cols = Data.GetCols();
    for (int j=0; j<Cols; j++)
      {
      ColData[j]->m_dOutput = Data[iCurIndex][j];
      }
    while (j<iColCnt)
      {
      ColData[j]->m_dOutput = 0.0;
      j++;
      }
    }
  }

//--------------------------------------------------------------------------

void QueueConInfo::AdvanceIndex()
  {
  if (bReloadRqd)
    DoLoad();
  const int rows = Data.GetRows();
  short PrevIndex = iCurIndex;
  if (rows>0)
    {
    iCurIndex++;
    if (iCurIndex==rows)
      {
      iCurIndex = (bWrapArround ? 0 : rows-1);
      }
    }
  else
    iCurIndex = -1;
  if (iCurIndex!=PrevIndex)
    SetIndex(iCurIndex);
  }

//==========================================================================
//
//
//
//==========================================================================

static double Drw_QueueCon[] =
       {
       DD_Poly, -3.2,-3.2, -3.2,3.2, 3.2,3.2, 3.2,-3.2, -3.2,-3.2,
       DD_Poly, -2.4,0, -1.6,0,
       DD_Poly, -0.8,0, 0.0,0,
       DD_Poly, 0.8,0, 1.6,0,
       DD_Poly, 2.4,0, 3.2,0,
       DD_TagPos, 0, -6.7,
       DD_End
       };

IMPLEMENT_MODELUNIT(CQueueCon, "QueueProfile", "", Drw_QueueCon, "Control", "QC", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,  
                    "Control:Queue Profile",
                   "Queue Profile Control model.")

flag CQueueCon::bWithCnvComment = true;

//--------------------------------------------------------------------------

CQueueCon::CQueueCon(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach),
  QCI(this)
  {
  fHasFiles=1;
  AttachClassInfo(nc_Control, NULL, NULL, &NullFlwGroup);
  SetActiveOptions(true, true);
  bDirectSetRqd = 0;
  bAboutToStart = 0;
  }

//--------------------------------------------------------------------------

CQueueCon::~CQueueCon()
  {
  }

//--------------------------------------------------------------------------

void CQueueCon::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

const word idmCheckBtn      = 1000;
const word idmOn            = 1100;
const word idmTagsInHeading = 1101;
const word idmSetTagsAlways = 1102;
const word idmFolder        = 1103;
const word idmFile          = 1104;
const word idmEdit          = 1105;
const word idmBrowse        = 1106;
const word idmReload        = 1107;
const word idmRestart       = 1108;
const word idmReloadRestart = 1109;
const word idmTagCount      = 1110;
const word idmCurIndex      = 1111;
const word idmAdvanceRowBtn = 1112;
const word idmRowCount      = 1113;

const word idmFirstTag      = 1120;

//--------------------------------------------------------------------------

void CQueueCon::BuildDataDefn(DataDefnBlk & DDB)
  {
  static DDBValueLst DDBLogSetTags[]={
    {0, "None"},
    {ReportToMsg, "Messages"},
    {ReportToEventLog, "EventLog"},
    {ReportToMsg|ReportToEventLog,   "Messages and EventLog"},
    {0}};

  DDB.BeginStruct(this);
  DDB.Text("");

  Strng Tag;
  DDB.CheckBoxBtn("On",          "",          DC_,     "",      idmOn,              this, isParm, DDBYesNo);
  DDB.CheckBoxBtn("TagsInHeading","",         DC_,     "",      idmTagsInHeading,   this, isParmStopped, DDBYesNo);
  DDB.CheckBoxBtn("SetAlways",   "",         DC_,     "",       idmSetTagsAlways,   this, isParmStopped, DDBYesNo);
  DDB.Visibility(NSHM_All, !QCI.bUseXRefs);
  DDB.Byte("LogTagSets",         "",          DC_,      "",     &QCI.eLogSetTags,   this, isParm, DDBLogSetTags);
  DDB.Visibility();
  DDB.String("Folder",           "",          DC_,     "",      idmFolder,          this, isResult);//isParm);
  DDB.String("File",             "",          DC_,     "",      idmFile,            this, isParmStopped);
  DDB.String("Status",           "",          DC_,     "",      &(QCI.sStatus),     this, isResult|noFileAtAll);
  DDB.Button("Edit",             "",          DC_,     "",      idmEdit,            this, isParm);
  DDB.Button("Browse",           "",          DC_,     "",      idmBrowse,          this, isParmStopped);
  DDB.Text("");
  DDB.Button("Reload",           "",          DC_,     "",      idmReload,          this, isParm);
  DDB.Button("Restart",          "",          DC_,     "",      idmRestart,         this, isParm);
  DDB.Button("Reload_Restart",   "",          DC_,     "",      idmReloadRestart,   this, isParm);
  DDB.CheckBoxBtn("ShowCnv",     "",          DC_,      "",     &bWithCnvComment,   this, isParmStopped, DDBYesNo);
  DDB.Button("Check_tags",        "",         DC_,      "",     idmCheckBtn,        this, isParmStopped);
  DDB.Text("");
  DDB.Short ("TagCount",         "",          DC_,     "",      idmTagCount,        this, isParmStopped);
  DDB.Text("");
  DDB.CheckBoxBtn("WrapArround", "",          DC_,     "",      &(QCI.bWrapArround),this, isParm, DDBYesNo);
  DDB.Short ("StartRow",         "",          DC_,     "",      &(QCI.iStartIndex), this, isParm);
  DDB.Short ("CurRow",           "",          DC_,     "",      idmCurIndex,        this, isParm);
  DDB.Button("Advance_Row",      "",          DC_,      "",     idmAdvanceRowBtn,   this, isParm);
  DDB.Short ("RowCount",         "",          DC_,     "",      idmRowCount,        this, isResult|noFileAtAll);
  DDB.Text("");
  if (QCI.iColCnt>0)
    DDB.Page("Q0", DDB_RqdPage);
  if (DDB.BeginArray(this, "Item", "ProfileTags", QCI.iColCnt))
    {
    for (int j=0; j<QCI.iColCnt; j++)
      {
      if ((j-10)%20==0)
        {
        char Pg[16];
        sprintf(Pg, "Q%d", j);
        DDB.Page(Pg, DDB_RqdPage);
        }
      DDB.BeginElement(this, j);
      Strng CnvTxt;
      if (bWithCnvComment && XRefsValid() && QCI.bValid)
        GetValidCnvTxt(QCI.ColData[j]->m_OutputVar, CnvTxt);
      DDB.String("Output_Tag",        "",          DC_,     "",      idmFirstTag+j, this, (QCI.bUseHeadingRow ? isResult : isParmStopped)|isTag);
      DDB.Double("Value",             "",          DC_,     "",      &(QCI.ColData[j]->m_dOutput), this, isResult|NAN_OK);
      if (CnvTxt.Len())
        DDB.TagComment(CnvTxt());
      }
    }
  DDB.EndArray();
  
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CQueueCon::DataXchg(DataChangeBlk & DCB)
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
          {
          if (QCI.bUseXRefs)
            LogNote(Tag(), 0, "No bad external tag references or function errors");
          }
        if (!QCI.bUseXRefs)
          QCI.CheckTags();
        bAboutToStart = 0;
        }
      DCB.B=0;
      return 1;
    case idmOn:
      if (DCB.rB)
        {
        bool WasOff = (QCI.bOn==0); 
        QCI.bOn = (*DCB.rB);
        if (QCI.bOn && WasOff)
          {
          MyTagsHaveChanged();
          }
        }
      DCB.B = QCI.bOn;
      return 1;
    case idmTagsInHeading:
      if (DCB.rB)
        {
        flag Prev = QCI.bUseHeadingRow;
        QCI.bUseHeadingRow = (*DCB.rB);
        if (Prev!=QCI.bUseHeadingRow && QCI.bUseHeadingRow)
          {
          QCI.bReloadRqd = 1;
          }
        }
      DCB.B = QCI.bUseHeadingRow;
      return 1;
    case idmSetTagsAlways:
      if (DCB.rB)
        {
        //flag Prev = QCI.bUseXRefs;
        QCI.bUseXRefs = (*DCB.rB);
        //if (Prev!=QCI.bUseXRefs && !QCI.bUseXRefs)
        //  UnlinkAllXRefs();
        }
      DCB.B = QCI.bUseXRefs;
      return 1;
    case idmFolder:
      if (DCB.rpC!=NULL)
        {
        // Dont Set if coming from snapshot ???
        QCI.sPath = DCB.rpC;
        QCI.sPath.FnContract();
        }
      if (DCB.ForFileSnpScn())
        {
        QCI.sPath.FnContract();//ensure it is always in contract form!
        DCB.pC = QCI.sPath();
        }
      else
        {
        QCI.sPathForView = QCI.sPath;
        if (SymbolicPaths())
          QCI.sPathForView.FnContract();
        else
          QCI.sPathForView.FnExpand();
        DCB.pC = QCI.sPathForView();
        }
      return 1;
    case idmFile:
      if (DCB.rpC)
        {
        Strng Prev(QCI.sFile);
        QCI.sFile = DCB.rpC;
        if (Prev!=QCI.sFile)
          QCI.bReloadRqd = 1;
        }
      DCB.pC = QCI.sFile();
      return 1;
    case idmEdit:
      if (DCB.rB && (*DCB.rB!=0))
        DCB.DoEditBtn(QCI.hProcess, QCI.dwProcessId, QCI.sPath(), QCI.sFile(), NULL/*"csv"*/);
      DCB.B=0;
      return 1;
    case idmBrowse:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng Prev(QCI.sPath);
        Prev += QCI.sFile;
        DCB.DoBrowseBtn(NULL/*"csv"*/, "SysCAD Profile", QCI.sPath, QCI.sFile);
        Strng Next(QCI.sPath);
        Next += QCI.sFile;
        if (Prev!=Next)
          QCI.bReloadRqd = 1;
        }
      DCB.B=0;
      return 1;
    case idmReload:
      if (DCB.rB && (*DCB.rB!=0))
        {
        short Prev = QCI.iColCnt;
        DoLoad();
        QCI.SetIndex(QCI.iCurIndex);
        if (QCI.iCurIndex!=Prev)
          QCI.SetTagsDirectly();
        }
      DCB.B=0;
      return 1;
    case idmRestart:
      if (DCB.rB && (*DCB.rB!=0))
        {
        QCI.SetIndex(QCI.iStartIndex);
        QCI.SetTagsDirectly();
        }
      DCB.B=0;
      return 1;
    case idmReloadRestart:
      if (DCB.rB && (*DCB.rB!=0))
        {
        DoLoad();
        QCI.SetIndex(QCI.iStartIndex);
        QCI.SetTagsDirectly();
        }
      DCB.B=0;
      return 1;
    case idmTagCount:
      if (DCB.rS)
        {
        short Prev = QCI.iColCnt;
        QCI.SetColCnt(*DCB.rS);
        if (QCI.bReloadRqd && QCI.iColCnt!=Prev)
          {
          DoLoad();
          MyTagsHaveChanged();
          }
        }
      DCB.S=QCI.iColCnt;
      return 1;
    case idmCurIndex:
      if (DCB.rS)
        {
        short Prev = QCI.iCurIndex;
        if (QCI.bReloadRqd)
          DoLoad();
        QCI.SetIndex(*DCB.rS);
        if (DCB.ForFileSnpScn())
          bDirectSetRqd = 1;
        else
          {
          if (QCI.iCurIndex!=Prev)
            QCI.SetTagsDirectly();
          }
        }
      DCB.S=QCI.iCurIndex;
      return 1;
    case idmAdvanceRowBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        //dbgp("[QCI.AdvanceIndex]");
        QCI.AdvanceIndex();
        QCI.SetTagsDirectly();
        }
      DCB.B=0;
      return 1;
    case idmRowCount:
      DCB.S=(short)QCI.Data.GetRows();
      return 1;
    default:
      if (DCB.lHandle>=idmFirstTag)
        {
        const int Index = DCB.lHandle - idmFirstTag;
        if (DCB.rpC && !QCI.bUseHeadingRow)
          {
          flag Chg = (QCI.ColData[Index]->m_OutputVar.DoDataXchg(DCB)==1);
          if (Chg)
            {
            MyTagsHaveChanged();
            }
          }
        DCB.pC = QCI.ColData[Index]->m_OutputVar.sVar();
        }
      return 1;
    return True;
    }//end switch

  return False;
  }

//--------------------------------------------------------------------------

flag CQueueCon::ValidateData(ValidateDataBlk & VDB)
  {
  TaggedObject::ValidateTag(QCI.sTagSuffix);
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

void CQueueCon::DoLoad()
  {
  QCI.DoLoad();
  if (QCI.bUseHeadingRow)
    {
    bool AnyChg = false;
    for (int j=0; j<QCI.iColCnt; j++)
      {
      QueueColInfo * pCI = QCI.ColData[j];
      Strng s = pCI->m_sColTagName();
      bool Chg = (pCI->m_OutputVar.SetVar(s() ? s() : "")==1);
      AnyChg = AnyChg || Chg;
      }
    if (AnyChg)
      {
      MyTagsHaveChanged();
      }
    }
  }

//--------------------------------------------------------------------------

flag CQueueCon::PreStartCheck()
  {
  bAboutToStart = 1;
  QCI.iStartIndex = Max((short)-1, QCI.iStartIndex);
  QCI.iCurIndex = Max((short)-1, QCI.iCurIndex);
  if (QCI.bReloadRqd)
    {
    DoLoad();
    QCI.SetIndex(QCI.iCurIndex);
    }
  if (bDirectSetRqd)
    {
    QCI.SetTagsDirectly();
    bDirectSetRqd = 0;
    }
  return FlwNode::PreStartCheck();
  }

//--------------------------------------------------------------------------

bool CQueueCon::TestXRefListActive() 
  { 
  if (QCI.bUseXRefs)
    return SetXRefListActive((!GetActiveHold()) && (QCI.bOn!=0) && (QCI.iCurIndex>=0));
  return false;
  }

//---------------------------------------------------------------------------

int CQueueCon::UpdateXRefLists(CXRefBuildResults & Results)
  {
  FnMngrClear();
  int FunctNo = 0;
  if (QCI.bOn)
    {
    QCI.bValid = 1;
    if (QCI.bUseXRefs)
      {
      for (int j=0; j<QCI.iColCnt; j++)
        {
        Strng S;
        S.Set("%s.Out%i", Tag(), j);
        int RetCode = QCI.ColData[j]->m_OutputVar.UpdateXRef(&QCI, 1, 0, FunctNo, this, -1, S(), S(), "ProfileQueueCon:Output", Results);
        if (RetCode!=BXR_OK)
          QCI.bValid = 0;
        }
      }
    }
  if (QCI.bUseXRefs)
    FnMngrTryUpdateXRefLists(Results);
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void CQueueCon::UnlinkAllXRefs()
  {
  FnMngrClear();
  FnMngrTryUnlinkAllXRefs();
  for (int j=0; j<QCI.iColCnt; j++)
    QCI.ColData[j]->m_OutputVar.UnlinkXRefs();
  CNodeXRefMngr::UnlinkAllXRefs();
  }

//--------------------------------------------------------------------------

void CQueueCon::EvalCtrlStrategy(eScdCtrlTasks Tasks)
  {
  if (!QCI.bUseXRefs)
    return;

  if (XRefListActive() && QCI.iCurIndex>=0 && ICGetTimeInc()>0.0)
    {
    GetNearXRefValues();

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
    if (QCI.bValid && QCI.bOn)
      {
      for (int j=0; j<QCI.iColCnt; j++)
        if (Valid(QCI.ColData[j]->m_dOutput) || !HasNANFlag(QCI.ColData[j]->m_dOutput, NF_Free))
          QCI.ColData[j]->m_OutputVar.PutValue(QCI.ColData[j]->m_dOutput);
      }
    SetNearXRefValues();
    }
  }

//--------------------------------------------------------------------------

void CQueueCon::SetState(eScdMdlStateActs RqdState)
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
  if (DoReset && QCI.bOn)
    {
    QCI.SetIndex(QCI.iStartIndex);
    QCI.SetTagsDirectly();
    }

  }

//--------------------------------------------------------------------------

void CQueueCon::EvalProducts(CNodeEvalIndex & NEI)
  {
  }

//--------------------------------------------------------------------------

int CQueueCon::FilesUsed(CFilesUsedArray & Files)
  {
  int Cnt = 0;
  if (QCI.sFile.Len()>0)
    {
    Strng Fn(QCI.sPath);
    Fn.FnExpand();
    Fn += QCI.sFile;
    Files.AddFile(Fn(), FU_CopyFile|FU_EditTxt);
    Cnt++;
    }
  FlwNode::FilesUsed(Files);
  return Files.GetCount();
  //return Cnt;
  }

//--------------------------------------------------------------------------

int CQueueCon::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  BOOL DidChange = FALSE;
  for (int j=0; j<QCI.iColCnt; j++)
    {
    if (QCI.ColData[j]->m_OutputVar.DoChangeTag(pOldTag, pNewTag))
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

int CQueueCon::DeleteTag(pchar pDelTag)
  {
  BOOL FoundOne = FALSE;
  for (int j=0; j<QCI.iColCnt; j++)
    {
    if (QCI.ColData[j]->m_OutputVar.ContainsTag(pDelTag))
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
                                          
dword CQueueCon::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=(QCI.bOn ? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag CQueueCon::CIStrng(int No, pchar & pS)
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