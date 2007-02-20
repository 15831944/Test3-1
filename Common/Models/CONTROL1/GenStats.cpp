//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __GENSTATS_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "pgm_e.h"
#include "genstats.h"

#if !SKIPIT 

//==========================================================================

const long FunctSubsStartIndex = 500;

//==========================================================================

GenStatInfo::GenStatInfo():
  m_MeasVar(BXO_Blank)
  {
  //m_MeasVar.Init(FALSE, TRUE);
  bValid = 0;
  iPriority = 0;
  ResetStats();
  }

//--------------------------------------------------------------------------

GenStatInfo::~GenStatInfo()
  {
  }

//--------------------------------------------------------------------------

void GenStatInfo::Init(int iNo)
  {
  sTagSuffix.Set("FS_%d", iNo);
  iPriority = iNo;
  //pXRM = ERH;
  }

//--------------------------------------------------------------------------

void GenStatInfo::ResetStats()
  {
  dMeas = 0.0;
  dTtlTime = 0.0;
  dZeroTime = 0.0;
  dTotal = 0.0;
  dMax = -1e99;
  dMin = 1e99;
  }

//--------------------------------------------------------------------------

void GenStatInfo::ExecIns(double dT)
  {
  dTtlTime += dT;
  if (fabs(dMeas)<1e-6)
    dZeroTime += dT;
  dTotal += (dMeas*dT);
  if (dMeas>dMax)
    dMax = dMeas;
  if (dMeas<dMin)
    dMin = dMeas;
  }

//==========================================================================
//
//  Statistics
//
//==========================================================================

static double Drw_GenStats[] =
       {
       DD_Poly, -3,-3, -3,3, 3,3, 3,-3, -3,-3, 
       DD_Poly, -2,-2, -2,2, 2,2, 2,-2, -2,-2, 
       DD_End
       };

IMPLEMENT_MODELUNIT(CGenStats, "GenStats", "", Drw_GenStats, "Control", "GS", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,  
                    "Statistics:General",
                   "General Statistics model.")

flag CGenStats::bWithCnvComment = true;

//--------------------------------------------------------------------------

CGenStats::CGenStats(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach)
    //,  m_MXRH(this)
  {
  AttachClassInfo(nc_Control, NULL, &NullFlwGroup);
  fActiveHoldOK_Dyn=true;
  bOn = 1;
  bResetOnStart = 0;
  bResetOnInit = 1;
  bResetOnEmpty = 1;
  bResetOnPreSet = 1;
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

CGenStats::~CGenStats()
  {
  SetCount(0);
  }

//--------------------------------------------------------------------------

void CGenStats::SetCount(long NewSize)
  {
  NewSize = max(NewSize, 0L);
  if (NewSize!=iCount)
    {
    GenStatInfo** NewDataBlk = NULL;
    if (NewSize<iCount)
      {
      for (int i=NewSize; i<iCount; i++)
        delete DataBlk[i];
      }
    if (NewSize>0)
      {
      NewDataBlk = new pGenStatInfo[NewSize];
      for (int i=0; (i<NewSize && i<iCount); i++)
        NewDataBlk[i] = DataBlk[i];
      for (i=iCount; i<NewSize; i++)
        {
        NewDataBlk[i] = new GenStatInfo;
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

flag CGenStats::SortRqd()
  {
  for (int i=1; i<iCount; i++)
    {
    if (DataBlk[i-1]->iPriority>DataBlk[i]->iPriority)
      return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

void CGenStats::Sort()
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
      GenStatInfo* p = DataBlk[i];
      DataBlk[i] = DataBlk[MinPos];
      DataBlk[MinPos] = p;
      }
    }
  StructureChanged(this);
  }

//--------------------------------------------------------------------------

void CGenStats::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

const word idmCount     = 1000;
const word idmCheckBtn  = 1001;
const word idmResetBtn  = 1002;
const word idmCfgTags   = 1100;

const word NoOfCfgTags = 10;

//--------------------------------------------------------------------------

void CGenStats::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CGenStats", NULL, DDB_NoPage);
  DDB.Text("");
  DDB.CheckBoxBtn("On",           "",         DC_, "", &bOn,           this, isParmStopped, DDBYesNo);
  DDB.CheckBoxBtn("ShowCnv",      "",         DC_, "", &bWithCnvComment,this, isParmStopped, DDBYesNo);
  DDB.Visibility(NSHM_All, iCount>0);
  DDB.CheckBoxBtn("ResetOnStart", "",         DC_, "", &bResetOnStart, this, isParm, DDBYesNo);
  DDB.Visibility(NM_Probal|SM_All|HM_All, iCount>0);
  DDB.CheckBoxBtn("ResetOnInit",  "",         DC_, "", &bResetOnInit,  this, isParm, DDBYesNo);
  DDB.Visibility(NSHM_All, iCount>0);
  DDB.CheckBoxBtn("ResetOnEmpty", "",         DC_, "", &bResetOnEmpty, this, isParm, DDBYesNo);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All, iCount>0);
  DDB.CheckBoxBtn("ResetOnPreSet","",         DC_, "", &bResetOnPreSet,this, isParm, DDBYesNo);
  DDB.Visibility();
  DDB.Long("Number_of_Stats", "No_of_Stats",  DC_, "", idmCount,       this, isParmStopped/*|AffectsStruct*/);
  DDB.Text("");
  if (iCount>0)
    {
    DDB.Button("Check_tags"/* and functions"*/, "", DC_, "", idmCheckBtn, this, isParmStopped);
    DDB.Button("Reset_All",      "",          DC_,     "",      idmResetBtn, this, isParm);
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
  if (DDB.BeginArray(this, "Cfg", "Gen_Cfg", iCount))
    {
    for (int i=0; i<iCount; i++)
      {
      if (i>0 && (i % 4)==2)
        {
        sprintf(Buff, "Cfg_%d", i);
        DDB.Page(Buff, DDB_RqdPage);
        }
      DDB.BeginElement(this, i);
      GenStatInfo* p = DataBlk[i];
      Strng CnvTxt;
      if (bWithCnvComment && XRefsValid() && p->bValid)
        GetValidCnvTxt(p->m_MeasVar, CnvTxt);
      //DDB.String("Name",             "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+0, this, isParmStopped); not needed?
      DDB.String("Description",      "Desc",      DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+7, this, isParm);
      DDB.String("StatTag",          "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+2, this, isParmStopped|isTag);
      DDB.Button("Reset_Stats",      "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+3, this, isParm);
      DDB.Double("Time",             "Tm",        DC_Time, "s",     &(p->dTtlTime),   this, isResult);
      //DDB.Visibility(NSHM_All, (p->iType & FSWhatQm));
      DDB.Double("Meas",             "",          DC_,     "",      &(p->dMeas),      this, isResult|InitHidden|noFileAtAll);
      if (CnvTxt.Len())
        DDB.TagComment(CnvTxt());
      DDB.Double("Total",            "Ttl",       DC_,     "",      &(p->dTotal),     this, isResult);
      DDB.Double("Average",          "Ave",       DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+5, this, isResult|noFileAtAll);
      if (CnvTxt.Len())
        DDB.TagComment(CnvTxt());
      DDB.Double("ZeroTime",         "ZeroTm",    DC_Time, "s",     &(p->dZeroTime),  this, isResult);
      DDB.Double("On_Average",       "OnAve",     DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+6, this, isResult|InitHidden|noFileAtAll);
      if (CnvTxt.Len())
        DDB.TagComment(CnvTxt());
      DDB.Double("Min",              "",          DC_,     "",      &(p->dMin),       this, isResult);//|InitHidden);
      if (CnvTxt.Len())
        DDB.TagComment(CnvTxt());
      DDB.Double("Max",              "",          DC_,     "",      &(p->dMax),       this, isResult);//|InitHidden);
      if (CnvTxt.Len())
        DDB.TagComment(CnvTxt());
      DDB.Text("");
      DDB.Long  ("Priority",         "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+4, this, isParmStopped);
      DDB.Text("----------------------------------------");
      }
    }
  DDB.EndArray();
  
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CGenStats::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  switch (DCB.lHandle)
    {
    case idmCount:
      if (DCB.rL)
        {
        if (*DCB.rL!=iCount)
          StructureChanged(this);
        SetCount(Range(0L, *DCB.rL, 50L));
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
    case idmResetBtn:
      if (DCB.rB && (*DCB.rB!=0))
        ResetAllStats();
      DCB.B=0;
      return True;
    default:
      if (DCB.lHandle>=idmCfgTags)
        {
        int Index = (DCB.lHandle - idmCfgTags) / NoOfCfgTags;
        if (Index<iCount)
          {
          GenStatInfo* p = DataBlk[Index];
          switch ((DCB.lHandle - idmCfgTags) - (Index * NoOfCfgTags))
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
            case 2:
              {
              flag Chg = (p->m_MeasVar.DoDataXchg(DCB)==1);
              if (Chg)
                {
                MyTagsHaveChanged();
                }
              DCB.pC = p->m_MeasVar.sVar();
              break;
              }
            case 3:
              if (DCB.rB)
                p->ResetStats();
              DCB.B = 0;
              break;
            case 4:
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
            case 5:
              //if (DCB.rD)
              //  xxx = *DCB.rD;
              DCB.D = p->dTotal/GTZ(p->dTtlTime);
              break;
            case 6:
              //if (DCB.rD)
              //  xxx = *DCB.rD;
              DCB.D = p->dTotal/GTZ(p->dTtlTime - p->dZeroTime);
              break;
            case 7:
              {
              if (DCB.rpC)
                p->sDesc = DCB.rpC;
              DCB.pC = p->sDesc();
              break;
              }
            }
          }
        return True;
        }
    }

  return False;
  }

//--------------------------------------------------------------------------

flag CGenStats::ValidateData(ValidateDataBlk & VDB)
  {
  for (int i=0; i<iCount; i++)
    {
    TaggedObject::ValidateTag(DataBlk[i]->sTagSuffix);
    }
  Sort();
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

flag CGenStats::PreStartCheck()
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

//--------------------------------------------------------------------------

bool CGenStats::TestXRefListActive() { return SetXRefListActive(!GetActiveHold() && bOn!=0); }

//---------------------------------------------------------------------------

int CGenStats::UpdateXRefLists(CXRefBuildResults & Results)
  {
  if (1)//bOn)
    {
    FnMngrClear();
    int FunctNo = 0;
    for (int i=0; i<iCount; i++)
      {
      GenStatInfo* p = DataBlk[i];
      Strng S;
      S.Set("%s.Cfg.[%i].%s", Tag(), i, "Meas");
      int RetCode = p->m_MeasVar.UpdateXRef(p, 0, 1, FunctNo, this, -1, S(), S(), "GenStats:Meas", Results);
      p->bValid = (RetCode==BXR_OK);
      }
    FnMngrTryUpdateXRefLists(Results);
    }
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void CGenStats::UnlinkAllXRefs()
  {
  FnMngrClear();
  FnMngrTryUnlinkAllXRefs();

  for (int i=0; i<iCount; i++)
    {
    GenStatInfo* p = DataBlk[i];
    p->m_MeasVar.UnlinkXRefs();
    }

  CNodeXRefMngr::UnlinkAllXRefs();
  };

//--------------------------------------------------------------------------

void CGenStats::ResetAllStats()
  {
  for (int i=0; i<iCount; i++)
    DataBlk[i]->ResetStats();
  }

//--------------------------------------------------------------------------

void CGenStats::EvalStatistics(eScdCtrlTasks Tasks)
  {
  if (XRefListActive() && ICGetTimeInc() > 0.0)
    {
    
    GetNearXRefValues();
    //for (int i=0; i<m_NearXRefs.GetSize(); i++)
    //  if (m_NearXRefs[i]->bMustGet)
    //    m_NearXRefs[i]->GetNearXRefValue();

    if (bAboutToStart && bResetOnStart)
      ResetAllStats();
    
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

    //solve GenStats...
    for (int i=0; i<iCount; i++)
      {
      GenStatInfo* p = DataBlk[i];
      if (p->bValid)
        {
        p->m_MeasVar.GetValue(p->dMeas);
        p->ExecIns(ICGetTimeInc());
        }
      }
    }
  }

//--------------------------------------------------------------------------

void   CGenStats::SetState(eScdMdlStateActs RqdState)
  {
  FlwNode::SetState(RqdState);
  flag DoReset = false;
  switch (RqdState)
    {
    case MSA_PBInit   : if (bResetOnInit)   DoReset = true; break;
    case MSA_ZeroFlows: break;
    case MSA_Empty    : if (bResetOnEmpty)  DoReset = true; break;
    case MSA_PreSet   : if (bResetOnPreSet) DoReset = true; break;
    case MSA_DynStatsRunInit: DoReset = true; break;
    default: break;
    }
  if (DoReset)
    ResetAllStats();
  }

//--------------------------------------------------------------------------

void CGenStats::EvalProducts(CNodeEvalIndex & NEI)
  {
  }

//--------------------------------------------------------------------------

int CGenStats::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  BOOL DidChange = FALSE;
  for (int i=0; i<iCount; i++)
    {
    if (DataBlk[i]->m_MeasVar.DoChangeTag(pOldTag, pNewTag))
      DidChange = TRUE;
    }
  if (DidChange)
    {
    UnlinkAllXRefs(); //PreStartCheck();
    bAboutToStart = 0;
    }
  return EOCT_DONE;
  }

//--------------------------------------------------------------------------

int CGenStats::DeleteTag(pchar pDelTag)
  {
  BOOL FoundOne = FALSE;
  for (int i=0; i<iCount; i++)
    {
    if (DataBlk[i]->m_MeasVar.ContainsTag(pDelTag))
      FoundOne = TRUE;
    }
  if (FoundOne)
    {
    LogNote(Tag(), 0, "Delete tag '%s' affects Gen Statistics model '%s'.", pDelTag, Tag());
    UnlinkAllXRefs(); //PreStartCheck();
    bAboutToStart = 0;
    }
  return EODT_DONE;
  }

//--------------------------------------------------------------------------
                                          
dword CGenStats::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=bOn ? FNS_On : FNS_Off;
  return Status;
  }

//--------------------------------------------------------------------------

flag CGenStats::CIStrng(int No, pchar & pS)
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