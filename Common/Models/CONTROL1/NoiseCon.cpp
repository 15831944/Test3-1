//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
//#include "pgm_e.h"
#include "noisecon.h"

#if !SKIPIT 

//==========================================================================

const byte NT_Gaus1      = NS_Gaus1;
const byte NT_Flat1      = NS_Flat1;
const byte NT_Pois1      = NS_Pois1;
const byte NT_Gamma1     = NS_Gamma1;
const byte NT_Weibull1   = NS_Weibull1;

const long FunctSubsStartIndex = 500;

//==========================================================================

NoiseConInfo::NoiseConInfo():
  m_OutputVar(BXO_Blank)
  {
  //m_OutputVar.Init(FALSE, TRUE);
  bOn = 1;
  bValid = 0;
  iPriority = 0;
  iExecCnt = 1;
  ResetStats();
  }

//--------------------------------------------------------------------------

NoiseConInfo::~NoiseConInfo()
  {
  }

//--------------------------------------------------------------------------

void NoiseConInfo::Init(int iNo)
  {
  sTagSuffix.Set("NC_%d", iNo);
  iPriority = iNo;
  //pXRM = ERH;
  }

//--------------------------------------------------------------------------

void NoiseConInfo::ResetStats()
  {
  iIterCnt = 0;
  }

//--------------------------------------------------------------------------

double NoiseConInfo::ExecIns()
  {
  iExecCnt = max(iExecCnt, 1L);
  if (iExecCnt==1)
    Noise.GetVal();
  else
    {
    ldiv_t div_result;
    div_result = ldiv(iIterCnt, iExecCnt);
    if (div_result.rem==0)
      Noise.GetVal();
    }
  iIterCnt++;
  return Noise.m_dOutput;
  }

//==========================================================================
//
//
//
//==========================================================================

static double Drw_NoiseCon[] =
       {
       DD_Poly, -3.2,-3.2, -3.2,3.2, 3.2,3.2, 3.2,-3.2, -3.2,-3.2,
       DD_Poly, -3.2,0.0, -2.4,1.6, -1.6,-0.8, -0.8,0.8, 0.0,-1.6, 0.8,1.6, 1.6,-2.4, 2.4,0.0, 3.2,0.0, 
       DD_TagPos, 0, -6.7,
       DD_End
       };

IMPLEMENT_MODELUNIT(CNoiseCon, "NoiseCon", "", Drw_NoiseCon, "Control", "NC", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,  
                    "Control:Noise",
                   "Noise and Random number Control model.")

flag CNoiseCon::bWithCnvComment = true;

//--------------------------------------------------------------------------

CNoiseCon::CNoiseCon(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach)
    //, m_MXRH(this)
  {
  AttachClassInfo(nc_Control, NULL, &NullFlwGroup);
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

CNoiseCon::~CNoiseCon()
  {
  SetCount(0);
  }

//--------------------------------------------------------------------------

void CNoiseCon::SetCount(long NewSize)
  {
  NewSize = max(NewSize, 0L);
  if (NewSize!=iCount)
    {
    NoiseConInfo** NewDataBlk = NULL;
    if (NewSize<iCount)
      {
      for (int i=NewSize; i<iCount; i++)
        delete DataBlk[i];
      }
    if (NewSize>0)
      {
      NewDataBlk = new pNoiseConInfo[NewSize];
      for (int i=0; (i<NewSize && i<iCount); i++)
        NewDataBlk[i] = DataBlk[i];
      for (i=iCount; i<NewSize; i++)
        {
        NewDataBlk[i] = new NoiseConInfo;
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

flag CNoiseCon::SortRqd()
  {
  for (int i=1; i<iCount; i++)
    {
    if (DataBlk[i-1]->iPriority>DataBlk[i]->iPriority)
      return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

void CNoiseCon::Sort()
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
      NoiseConInfo* p = DataBlk[i];
      DataBlk[i] = DataBlk[MinPos];
      DataBlk[MinPos] = p;
      }
    }
  StructureChanged(this);
  }

//--------------------------------------------------------------------------

void CNoiseCon::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

const word idmCount     = 1000;
const word idmCheckBtn  = 1001;
const word idmCfgTags   = 1100;

const word NoOfCfgTags = 8;

//--------------------------------------------------------------------------

void CNoiseCon::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CNoiseCon", NULL, DDB_NoPage);
  DDB.Text("");
  DDB.CheckBoxBtn("On",           "",         DC_, "", &bOn,           this, isParmStopped, DDBYesNo);
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

  static DDBValueLst DDBNoiseType[]={
      {NT_Gaus1,              "Gaussian" }, 
      {NT_Flat1,              "Flat"     },
      {NT_Pois1,              "Poisson"  },
      {NT_Gamma1,             "Gamma"    },
      {NT_Weibull1,           "Weibull"  },
      {0}};
  char Buff[128];
  Strng Tag;
  if (DDB.BeginArray(this, "Cfg", "Nois_Cfg", iCount))
    {
    for (int i=0; i<iCount; i++)
      {
      if (i>0 && (i % 4)==2)
        {
        sprintf(Buff, "Cfg_%d", i);
        DDB.Page(Buff, DDB_RqdPage);
        }
      DDB.BeginElement(this, i);
      NoiseConInfo* p = DataBlk[i];
      const NoiseTypes NT = p->Noise.NoiseType();
      Strng CnvTxt;
      if (bWithCnvComment && XRefsValid() && p->bValid)
        GetValidCnvTxt(p->m_OutputVar, CnvTxt);
      DDB.Byte  ("Type",             "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+1, this, isParm, DDBNoiseType);
      //DDB.String("Name",             "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+0, this, isParmStopped); not needed?
      DDB.String("Output_Tag",       "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+2, this, isParmStopped|isTag);
      DDB.Double("Output_Value",     "Output",    DC_,     "",      &(p->Noise.m_dOutput),  this, isResult);
      if (NT==NT_Gaus1 || NT==NT_Flat1)
        DDB.Range(p->Noise.m_dMean-(2.0*p->Noise.m_dStdDev), p->Noise.m_dMean+(2.0*p->Noise.m_dStdDev));
      if (CnvTxt.Len())
        DDB.TagComment(CnvTxt());
      DDB.CheckBoxBtn("On",          "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+5, this, isParm, DDBYesNo);
      if (NT==NT_Weibull1)
        DDB.Double("a",                "",          DC_,     "",      &(p->Noise.m_dMean),    this, isParm);
      else
        {
        DDB.Double("Mean",             "",          DC_,     "",      &(p->Noise.m_dMean),    this, isParm);
        if (CnvTxt.Len())
          DDB.TagComment(CnvTxt());
        }
      DDB.Visibility(NSHM_All, NT==NT_Gaus1 || NT==NT_Flat1 || NT==NT_Weibull1);
      if (NT==NT_Weibull1)
        DDB.Double("b",                "",          DC_,     "",      &(p->Noise.m_dStdDev),  this, isParm);
      else
        {
        DDB.Double("StdDev",           "",          DC_,     "",      &(p->Noise.m_dStdDev),  this, isParm);
        if (CnvTxt.Len())
          DDB.TagComment(CnvTxt());
        }
      DDB.Visibility();
      DDB.Long  ("ExecCnt",          "",          DC_,     "",      &(p->iExecCnt),           this, isParm);
      DDB.Long  ("IterCnt",          "",          DC_,     "",      &(p->iIterCnt),           this, isParm);
      DDB.Text("");
      DDB.Long  ("Priority",         "",          DC_,     "",      idmCfgTags+(i*NoOfCfgTags)+4, this, isParmStopped);
      DDB.Text("----------------------------------------");
      }
    }
  DDB.EndArray();
  
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CNoiseCon::DataXchg(DataChangeBlk & DCB)
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
    default:
      if (DCB.lHandle>=idmCfgTags)
        {
        int Index = (DCB.lHandle - idmCfgTags) / NoOfCfgTags;
        if (Index<iCount)
          {
          NoiseConInfo* p = DataBlk[Index];
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
                p->Noise.SetType((NoiseTypes)(*DCB.rB));
              DCB.B=(byte)p->Noise.NoiseType();
              break;
            case 2:
              {
              flag Chg = (p->m_OutputVar.DoDataXchg(DCB)==1);
              if (Chg)
                {
                MyTagsHaveChanged();
                }
              DCB.pC = p->m_OutputVar.sVar();
              break;
              }
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
              if (DCB.rB)
                p->bOn = (*DCB.rB);
              DCB.B = p->bOn;
              break;
            }
          }
        return True;
        }
    }

  return False;
  }

//--------------------------------------------------------------------------

flag CNoiseCon::ValidateData(ValidateDataBlk & VDB)
  {
  for (int i=0; i<iCount; i++)
    {
    TaggedObject::ValidateTag(DataBlk[i]->sTagSuffix);
    }
  Sort();
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

flag CNoiseCon::PreStartCheck()
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

bool CNoiseCon::TestXRefListActive() { return SetXRefListActive(!GetActiveHold() && bOn!=0); }

//---------------------------------------------------------------------------

int CNoiseCon::UpdateXRefLists(CXRefBuildResults & Results)
  {
  if (1)//bOn)
    {
    FnMngrClear();
    int FunctNo = 0;
    for (int i=0; i<iCount; i++)
      {
      NoiseConInfo* p = DataBlk[i];
      if (p->bOn)
        {
        Strng S;
        S.Set("%s.Cfg.[%i].Out", Tag(), i);
        BXReturn RetCode = p->m_OutputVar.UpdateXRef(p, 1, 0, FunctNo, this, -1, S(), S(), "NoiseCon:Output", Results);
        p->bValid = (RetCode==BXR_OK);
        }
      }
    FnMngrTryUpdateXRefLists(Results);
    }
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void CNoiseCon::UnlinkAllXRefs()
  {
  FnMngrClear();
  FnMngrTryUnlinkAllXRefs();
  for (int i=0; i<iCount; i++)
    {
    NoiseConInfo* p = DataBlk[i];
    p->m_OutputVar.UnlinkXRefs();
    }

  CNodeXRefMngr::UnlinkAllXRefs();
  };

//--------------------------------------------------------------------------

void CNoiseCon::ResetAllStats()
  {
  for (int i=0; i<iCount; i++)
    DataBlk[i]->ResetStats();
  }

//--------------------------------------------------------------------------

void CNoiseCon::EvalCtrlStrategy(eScdCtrlTasks Tasks)
  {
  if (XRefListActive() && ICGetTimeInc() > 0.0)
    {
    
    GetNearXRefValues();
    //for (int i=0; i<m_NearXRefs.GetSize(); i++)
    //  if (m_NearXRefs[i]->bMustGet)
    //    m_NearXRefs[i]->GetNearXRefValue();

    //if (bAboutToStart && bResetOnStart)
    //  ResetAllStats();
    
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
    for (int i=0; i<iCount; i++)
      {
      NoiseConInfo* p = DataBlk[i];
      if (p->bValid)
        {
        double NewVal = p->ExecIns();
        p->m_OutputVar.PutValue(NewVal);
        }
      }
    SetNearXRefValues();
    //for (i=0; i<m_NearXRefs.GetSize(); i++)
    //  m_NearXRefs[i]->SetNearXRefValue();
    }
  }

//--------------------------------------------------------------------------

void   CNoiseCon::SetState(eScdMdlStateActs RqdState)
  {
  FlwNode::SetState(RqdState);
  /*flag DoReset = false;
  switch (RqdState)
    {
    case MSA_PBInit   : if (bResetOnInit)   DoReset = true; break;
    case MSA_ZeroFlows: break;
    case MSA_Empty    : if (bResetOnEmpty)  DoReset = true; break;
    case MSA_PreSet   : if (bResetOnPreSet) DoReset = true; break;
    default: break;
    }
  if (DoReset)
    ResetAllStats();*/
  }

//--------------------------------------------------------------------------

void CNoiseCon::EvalProducts(CNodeEvalIndex & NEI)
  {
  }

//--------------------------------------------------------------------------

int CNoiseCon::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  BOOL DidChange = FALSE;
  for (int i=0; i<iCount; i++)
    {
    if (DataBlk[i]->m_OutputVar.DoChangeTag(pOldTag, pNewTag))
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

int CNoiseCon::DeleteTag(pchar pDelTag)
  {
  BOOL FoundOne = FALSE;
  for (int i=0; i<iCount; i++)
    {
    if (DataBlk[i]->m_OutputVar.ContainsTag(pDelTag))
      FoundOne = TRUE;
    }
  if (FoundOne)
    {
    LogNote(Tag(), 0, "Delete tag '%s' affects Noise Controller model '%s'.", pDelTag, Tag());
    UnlinkAllXRefs(); //PreStartCheck();
    bAboutToStart = 0;
    }
  return FlwNode::DeleteTag(pDelTag);
  }

//--------------------------------------------------------------------------
                                          
dword CNoiseCon::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=bOn ? FNS_On : FNS_Off;
  return Status;
  }

//--------------------------------------------------------------------------

flag CNoiseCon::CIStrng(int No, pchar & pS)
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