//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __FLWSTATS_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "pgm_e.h"
#include "flwstats.h"

#include "scd_wm.h"

#if !SKIPIT 

//==========================================================================

const long FunctSubsStartIndex = 500;

const byte FSWhatQm         = 0x01;
const byte FSWhatQv         = 0x02;
const byte FSWhatPhases     = 0x04;

//==========================================================================

FlwStatInfo::FlwStatInfo():
  m_MeasVar(BXO_Blank),
  m_QvVar(BXO_Blank),
  m_QmSVar(BXO_Blank),
  m_QmLVar(BXO_Blank)
  {
  //m_MeasVar.Init(FALSE, TRUE);
  //m_QvVar.Init(FALSE, TRUE);
  //m_QmSVar.Init(FALSE, TRUE);
  //m_QmLVar.Init(FALSE, TRUE);
  bValid = 0;
  iPriority = 0;
  iType = FSWhatQm;
  ResetStats();
  }

//--------------------------------------------------------------------------

FlwStatInfo::~FlwStatInfo()
  {
  }

//--------------------------------------------------------------------------

void FlwStatInfo::Init(int iNo)
  {
  sTagSuffix.Set("FS_%d", iNo);
  iPriority = iNo;
  //pXRM = ERH;
  }

//--------------------------------------------------------------------------

void FlwStatInfo::ResetStats()
  {
  dMeasQm = 0.0;
  dTtlTime = 0.0;
  dZeroTime = 0.0;
  dTotalMass = 0.0;
  dMaxQm = -1e99;
  dMinQm = 1e99;
  dMeasQv = 0.0;
  dTotalVol = 0.0;
  dMeasQmS = 0.0;
  dTotalMassS = 0.0;
  dMeasQmL = 0.0;
  dTotalMassL = 0.0;
  }

//--------------------------------------------------------------------------

flag FlwStatInfo::DoSetVars(char* Tag)
  {
  flag Chg = (m_MeasVar.SetVar(Tag, true, "Qm")==1);
  if (iType & FSWhatQv)
    {
    if (m_QvVar.SetVar(Tag, true, "Qv")==1)
      Chg =  true;
    }
  else
    m_QvVar.Clear();
  if (iType & FSWhatPhases)
    {
    if (m_QmSVar.SetVar(Tag, true, "Qi.SQMt")==1)
      Chg =  true;
    if (m_QmLVar.SetVar(Tag, true, "Qi.LQMt")==1)
      Chg =  true;
    }
  else
    {
    m_QmSVar.Clear();
    m_QmLVar.Clear();
    }
  return Chg;
  }

//--------------------------------------------------------------------------

void FlwStatInfo::ExecIns(double dT)
  {
  dTtlTime += dT;
  if (fabs(dMeasQm)<1e-6)
    dZeroTime += dT;
  dTotalMass += (dMeasQm*dT);
  if (dMeasQm>dMaxQm)
    dMaxQm = dMeasQm;
  if (dMeasQm<dMinQm)
    dMinQm = dMeasQm;
  if (iType & FSWhatQv)
    dTotalVol += (dMeasQv*dT);
  if (iType & FSWhatPhases)
    {
    dTotalMassS += (dMeasQmS*dT);
    dTotalMassL += (dMeasQmL*dT);
    }
  }

//==========================================================================
//
//  
//
//==========================================================================
/*#D:#T:Flow Controller(1)
#X:#h<General Description>#nThis unit is used to ...
#n#n
#G:Units
*/

static double Drw_FlwStats[] =
       {
       //DD_Poly, -1,0, -3,0, -3,-3, -3,3, 3,3, 3,-3, -3,-3, -3,0,
       //DD_Poly, 3,0, 1,0, 2,0, 2,2, -2,2, -2,-2, 2,-2, 2,0,
       DD_Poly, -3,-3, -3,3, 3,3, 3,-3, -3,-3,
       //DD_Poly, -2,-2, -2,2, 2,2, 2,-2, -2,-2,
       DD_Poly, -1,-1, -1,1, 1,1, 1,-1, -1,-1,
       DD_Poly, -3,0, -1,0,
       DD_Poly, 3,0, 1,0,
       DD_End
       };

IMPLEMENT_MODELUNIT(CFlowStats, "FlwStats", "", Drw_FlwStats, "Control", "FS", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                    "Statistics:Flow",
                   "Flow Statistics model.")

CFlowStats::CFlowStats(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach)
    //, m_MXRH(this)
  {
  AttachClassInfo(nc_Control, NULL, &NullFlwGroup);
  fActiveHoldOK_Dyn=true;
  bOn = 1;
  bResetOnStart = 0;
  bResetOnInit = 1;
  bResetOnEmpty = 1;
  bResetOnPreSet = 1;

//  bDoneXRefs = 0;
  bAboutToStart = 0;
  iCount = 0;
  DataBlk = NULL;
  for (int i=1; i<3; i++)
    m_StateLine[i] = "";
  m_StateLine[0] = "OK";
  SetCount(1);
  }

//--------------------------------------------------------------------------

CFlowStats::~CFlowStats()
  {
  SetCount(0);
  }

//--------------------------------------------------------------------------

void CFlowStats::SetCount(long NewSize)
  {
  NewSize = max(NewSize, 0L);
  if (NewSize!=iCount)
    {
    FlwStatInfo** NewDataBlk = NULL;
    if (NewSize<iCount)
      {
      for (int i=NewSize; i<iCount; i++)
        delete DataBlk[i];
      }
    if (NewSize>0)
      {
      NewDataBlk = new pFlwStatInfo[NewSize];
      for (int i=0; (i<NewSize && i<iCount); i++)
        NewDataBlk[i] = DataBlk[i];
      for (i=iCount; i<NewSize; i++)
        {
        NewDataBlk[i] = new FlwStatInfo;
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

flag CFlowStats::SortRqd()
  {
  for (int i=1; i<iCount; i++)
    {
    if (DataBlk[i-1]->iPriority>DataBlk[i]->iPriority)
      return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

void CFlowStats::Sort()
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
      FlwStatInfo* p = DataBlk[i];
      DataBlk[i] = DataBlk[MinPos];
      DataBlk[MinPos] = p;
      }
    }
  StructureChanged(this);
  }

//--------------------------------------------------------------------------

void CFlowStats::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

const word idmCount     = 1000;
const word idmCheckBtn  = 1001;
const word idmResetBtn  = 1002;
const word idmCfgTags   = 1100;

const word NoOfCfgTags = 12;

//--------------------------------------------------------------------------

void CFlowStats::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CFlowStats", NULL, DDB_NoPage);
  DDB.Text("");
  DDB.CheckBoxBtn("On",           "",         DC_, "", &bOn,           this, isParmStopped, DDBYesNo);
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

  static DDBValueLst DDBFlwStatsWhat[]={
      {FSWhatQm,                                  "Qm"             },
      {FSWhatQm|FSWhatQv,                         "Qm,Qv"          },
      {FSWhatQm|FSWhatPhases,                     "Qm,Phases"      },
      {FSWhatQm|FSWhatQv|FSWhatPhases,            "Qm,Qv,Phases"   },
      {0}};
  char Buff[128];
  Strng Tag;
  if (DDB.BeginArray(this, "Cfg", "Flw_Cfg", iCount))
    {
    for (int i=0; i<iCount; i++)
      {
      if (i>0 && (i % 2)==1)
        {
        sprintf(Buff, "Cfg_%d", i);
        DDB.Page(Buff, DDB_RqdPage);
        }
      DDB.BeginElement(this, i);
      FlwStatInfo* p = DataBlk[i];
      //DDB.String("Name",             "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+0, this, isParmStopped); not needed?
      DDB.String("Description",      "Desc",      DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+10, this, isParm);
      DDB.Byte  ("Type",             "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+1, this, isParmStopped, DDBFlwStatsWhat); //type MUST be before tag, etc
      DDB.String("QmTag",            "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+2, this, isParmStopped|isTag);
      DDB.Button("Reset_Stats",      "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+3, this, isParm);
      DDB.Double("Time",             "Tm",        DC_Time, "s",     &(p->dTtlTime),   this, isResult);
      //DDB.Visibility(NSHM_All, (p->iType & FSWhatQm));
      DDB.Double("Mass_Flow",        "Qm",        DC_Qm,   "kg/s",  &(p->dMeasQm),    this, isResult|InitHidden|noFileAtAll);
      DDB.Double("TotalMass",        "Mt",        DC_M,    "kg",    &(p->dTotalMass), this, isResult);
      DDB.Double("MassFlow_Average", "QmAve",     DC_Qm,   "kg/s",  idmCfgTags+(i*NoOfCfgTags)+5, this, isResult|noFileAtAll);
      DDB.Double("ZeroFlowTime",     "ZeroTm",    DC_Time, "s",     &(p->dZeroTime),  this, isResult);
      DDB.Double("On_Average",       "OnQmAve",   DC_Qm,   "kg/s",  idmCfgTags+(i*NoOfCfgTags)+6, this, isResult|InitHidden|noFileAtAll);
      DDB.Double("Min_MassFlow",     "QmMin",     DC_Qm,   "kg/s",  &(p->dMinQm),     this, isResult|InitHidden);
      DDB.Double("Max_MassFlow",     "QmMax",     DC_Qm,   "kg/s",  &(p->dMaxQm),     this, isResult|InitHidden);
      if (p->iType & FSWhatQv)
        {
        DDB.Double("Vol_Flow",         "Qv",        DC_Qv,   "m^3/s", &(p->dMeasQv),    this, isResult|InitHidden|noFileAtAll);
        DDB.Double("TotalVol",         "Vt",        DC_Vol,  "m^3",   &(p->dTotalVol),  this, isResult);
        DDB.Double("VolFlow_Average",  "QvAve",     DC_Qv,   "m^3/s", idmCfgTags+(i*NoOfCfgTags)+7, this, isResult|noFileAtAll);
        }
      if (p->iType & FSWhatPhases)
        {
        DDB.Double("SolidMassFlow",    "SQm",       DC_Qm,   "kg/s",  &(p->dMeasQmS),   this, isResult|InitHidden|noFileAtAll);
        DDB.Double("SolidTotalMass",   "SMt",       DC_M,    "kg",    &(p->dTotalMassS),this, isResult);
        DDB.Double("SolidMassFlowAve" ,"SQmAve",    DC_Qm,   "kg/s",  idmCfgTags+(i*NoOfCfgTags)+8, this, isResult|noFileAtAll);
        DDB.Double("LiquidMassFlow",   "LQm",       DC_Qm,   "kg/s",  &(p->dMeasQmL),   this, isResult|InitHidden|noFileAtAll);
        DDB.Double("LiquidTotalMass",  "LMt",       DC_M,    "kg",    &(p->dTotalMassL),this, isResult);
        DDB.Double("LiquidMassFlowAve","LQmAve",    DC_Qm,   "kg/s",  idmCfgTags+(i*NoOfCfgTags)+9, this, isResult|noFileAtAll);
        }
      DDB.Text("");
      DDB.Long  ("Priority",         "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+4, this, isParmStopped);
      DDB.Text("----------------------------------------");
      }
    }
  DDB.EndArray();

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CFlowStats::DataXchg(DataChangeBlk & DCB)
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
          FlwStatInfo* p = DataBlk[Index];
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
            case 1:
              if (DCB.rB)
                {
                if (p->iType!=*DCB.rB)
                  {
                  p->iType=*DCB.rB;
                  p->DoSetVars(p->m_MeasVar.sVar());
                  MyTagsHaveChanged();
                  }
                }
              DCB.B = p->iType;
              break;
            case 2:
              {
              if (DCB.rpC)
                {
                if (p->DoSetVars(DCB.rpC))
                  {
                  MyTagsHaveChanged();
                  }
                }
              DCB.pC = p->m_MeasVar.sVar();
              break;
              }
            case 3:
              if (DCB.rB && (*DCB.rB!=0))
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
              DCB.D = p->dTotalMass/GTZ(p->dTtlTime);
              break;
            case 6:
              //if (DCB.rD)
              //  xxx = *DCB.rD;
              DCB.D = p->dTotalMass/GTZ(p->dTtlTime - p->dZeroTime);
              break;
            case 7:
              //if (DCB.rD)
              //  xxx = *DCB.rD;
              DCB.D = p->dTotalVol/GTZ(p->dTtlTime);
              break;
            case 8:
              //if (DCB.rD)
              //  xxx = *DCB.rD;
              DCB.D = p->dTotalMassS/GTZ(p->dTtlTime);
              break;
            case 9:
              //if (DCB.rD)
              //  xxx = *DCB.rD;
              DCB.D = p->dTotalMassL/GTZ(p->dTtlTime);
              break;
            case 10:
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

flag CFlowStats::ValidateData(ValidateDataBlk & VDB)
  {
  for (int i=0; i<iCount; i++)
    {
    TaggedObject::ValidateTag(DataBlk[i]->sTagSuffix);
    }
  Sort();
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

flag CFlowStats::PreStartCheck()
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

bool CFlowStats::TestXRefListActive() { return SetXRefListActive(!GetActiveHold() && bOn!=0); }

//---------------------------------------------------------------------------

int CFlowStats::UpdateXRefLists(CXRefBuildResults & Results)
  {
  if (1)//bOn)
    {
    FnMngrClear();
    int FunctNo = 0;
    for (int i=0; i<iCount; i++)
      {
      FlwStatInfo* p = DataBlk[i];
      Strng S;
      S.Set("%s.Cfg.[%i].%s", Tag(), i, "Qm");
      int RetCode = p->m_MeasVar.UpdateXRef(p, 0, 1, FunctNo, this, -1, S(), S(), "FlwStats:Meas", Results);
      p->bValid = (RetCode==BXR_OK);
      if (p->bValid && (p->iType & FSWhatQv))
        {
        S.Set("%s.Cfg.[%i].%s", Tag(), i, "Qv");
        p->m_QvVar.UpdateXRef(p, 0, 1, FunctNo, this, -1, S(), S(), "FlwStats:MeasQv", Results); //ignore return code
        }
      if (p->bValid && (p->iType & FSWhatPhases))
        {
        S.Set("%s.Cfg.[%i].%s", Tag(), i, "SQm");
        p->m_QmSVar.UpdateXRef(p, 0, 1, FunctNo, this, -1, S(), S(), "FlwStats:MeasQmS", Results); //ignore return code
        S.Set("%s.Cfg.[%i].%s", Tag(), i, "LQm");
        p->m_QmLVar.UpdateXRef(p, 0, 1, FunctNo, this, -1, S(), S(), "FlwStats:MeasQmL", Results); //ignore return code
        }
      }
    FnMngrTryUpdateXRefLists(Results);
    }
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void CFlowStats::UnlinkAllXRefs()
  {
  FnMngrClear();
  FnMngrTryUnlinkAllXRefs();
  for (int i=0; i<iCount; i++)
    {
    FlwStatInfo* p = DataBlk[i];
    p->m_MeasVar.UnlinkXRefs();
    p->m_QvVar.UnlinkXRefs();
    p->m_QmSVar.UnlinkXRefs();
    p->m_QmLVar.UnlinkXRefs();
    }

  CNodeXRefMngr::UnlinkAllXRefs();
  };

//--------------------------------------------------------------------------

void CFlowStats::ResetAllStats()
  {
  for (int i=0; i<iCount; i++)
    DataBlk[i]->ResetStats();
  }

//--------------------------------------------------------------------------

void CFlowStats::EvalStatistics(eScdCtrlTasks Tasks)
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

    //solve FlowStats...
    for (int i=0; i<iCount; i++)
      {
      FlwStatInfo* p = DataBlk[i];
      if (p->bValid)
        {
        p->m_MeasVar.GetValue(p->dMeasQm);
        if (p->iType & FSWhatQv)
          p->m_QvVar.GetValue(p->dMeasQv);
        if (p->iType & FSWhatPhases)
          {
          p->m_QmSVar.GetValue(p->dMeasQmS);
          p->m_QmLVar.GetValue(p->dMeasQmL);
          }

        p->ExecIns(ICGetTimeInc());
        }
      }
    }
  }

//--------------------------------------------------------------------------

void   CFlowStats::SetState(eScdMdlStateActs RqdState)
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

void CFlowStats::EvalDiscrete()
  {
  }

//--------------------------------------------------------------------------

int CFlowStats::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  BOOL DidChange = FALSE;
  for (int i=0; i<iCount; i++)
    {
    if (DataBlk[i]->m_MeasVar.DoChangeTag(pOldTag, pNewTag))
      DidChange = TRUE;
    if (DataBlk[i]->m_QvVar.DoChangeTag(pOldTag, pNewTag))
      DidChange = TRUE;
    if (DataBlk[i]->m_QmSVar.DoChangeTag(pOldTag, pNewTag))
      DidChange = TRUE;
    if (DataBlk[i]->m_QmLVar.DoChangeTag(pOldTag, pNewTag))
      DidChange = TRUE;
    }
  if (DidChange)
    {
    PreStartCheck();
    bAboutToStart = 0;
    }
  return EOCT_DONE;
  }

//--------------------------------------------------------------------------

int CFlowStats::DeleteTag(pchar pDelTag)
  {
  BOOL FoundOne = FALSE;
  for (int i=0; i<iCount; i++)
    {
    if (DataBlk[i]->m_MeasVar.ContainsTag(pDelTag))
      FoundOne = TRUE;
    if (DataBlk[i]->m_QvVar.ContainsTag(pDelTag))
      FoundOne = TRUE;
    if (DataBlk[i]->m_QmSVar.ContainsTag(pDelTag))
      FoundOne = TRUE;
    if (DataBlk[i]->m_QmLVar.ContainsTag(pDelTag))
      FoundOne = TRUE;
    }
  if (FoundOne)
    {
    LogNote(Tag(), 0, "Delete tag '%s' affects Flow Statistics model '%s'.", pDelTag, Tag());
    PreStartCheck();
    bAboutToStart = 0;
    }
  return EODT_DONE;
  }

//--------------------------------------------------------------------------

dword CFlowStats::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=bOn ? FNS_On : FNS_Off;
  return Status;
  }

//--------------------------------------------------------------------------

flag CFlowStats::CIStrng(int No, pchar & pS)
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