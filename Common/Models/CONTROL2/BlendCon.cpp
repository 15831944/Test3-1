//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __BLENDCON_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "blender.h"
#include "tankmin\\tankmin.h"
extern "C" 
  {
  #include "glpk\\include\\glpk.h" 
  }
#include "sc\\sc.h"
#include "blendcon.h"
//#include "optoff.h"

#if !SKIPIT 

//==========================================================================

// Global AssaySumArray
extern CAssays gs_Assays;

//==========================================================================

const long FunctSubsStartIndex = 500;

//==========================================================================
//
//
//
//==========================================================================

static double Drw_BlendCon[] =
       {
       DD_Poly, -4,-4, -4,4, 4,4, 4,-4, -4,-4, 
       DD_Poly, -4,4, -1,0, -1,-4, 
       DD_Poly, 4,4, 1,0, 1,-4, 
       DD_End
       };

IMPLEMENT_MODELUNIT(CBlendCon, "BlendCon", "", Drw_BlendCon, "Control", "MBC", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,  
                    "Control:Blend",
                   "Blend Control with Multistore.")

//--------------------------------------------------------------------------

CBlendCon::CBlendCon(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach),
  mStockpileAssays      ("mStockpileCrit",       this, TOA_Embedded)
  #if WithMethod1
  ,
  mAssays               ("mAssays",              this, TOA_Embedded),
  mHardConstraints      ("mHardConstraints",     this, TOA_Embedded),
  mSoftConstraints      ("mSoftConstraints",     this, TOA_Embedded),
  mSoftRatios           ("mSoftRatios",          this, TOA_Embedded),
  mX                    ("mX",                   this, TOA_Embedded),
  mSoftConstraintCosts  ("mSoftConstraintCosts", this, TOA_Embedded),
  mSoftRatiosCosts      ("mSoftRatiosCosts",     this, TOA_Embedded)
  #endif
  {
  AttachClassInfo(nc_Control, NULL, &NullFlwGroup);
  fActiveHoldOK_Dyn=true;
  bOn = 1;
  bLogOptimiseErrors = 1;
  bLogOptimiseNotes = 0;
  bLogSetOptimiseData = 1;
  for (int i=0; i<3; i++)
    m_StateLine[i] = "";
  m_StateLine[0] = "OK";
  bValid = 0;
  }

//--------------------------------------------------------------------------

CBlendCon::~CBlendCon()
  {
  }

//--------------------------------------------------------------------------

void CBlendCon::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

//
// Maximum 16 Criteria x 1 Blend Controller
//
const short MaxCriteria=16;

//
// Maximum 100 species x 16 Defined Assay Sums
//
const short MaxAssaySum=16;
const short MaxSpeciesPerAssaySum=100; 

XID xidBCCritCnt           = 1;             // Number of Criteria in Controller
XID xidBCAssaySumCnt       = 2;             // Number of Global Assays
XID xidBCOptimise          = 3;             // Optimise Button
XID xidBCOptimiseSet       = 4;
XID xidBCNSpcs1            = 5;             // Number of Species in each Assay
XID xidBCAddSpc            = xidBCNSpcs1+MaxAssaySum;
XID xidBCRemSpc            = xidBCAddSpc+MaxAssaySum*MaxSpeciesPerAssaySum;


//--------------------------------------------------------------------------

void CBlendCon::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CBlendCon", NULL, DDB_NoPage);
  DDB.Text("");
  //DDB.Text    ("Requirements");
  DDB.CheckBoxBtn("On",           "",         DC_, "", &bOn,           this, isParmStopped, DDBYesNo);
  //DDB.Button("Check_tags", "", DC_, "", idmCheckBtn, this, isParmStopped);
  DDB.String("MultiStoreTag", "", DC_, "", &sMSTag, this, isParmStopped|isTag);
  DDB.String("StreamTag", "", DC_, "", &sPipeTag, this, isParmStopped|isTag);
  DDB.CheckBoxBtn("TrackErrors", "", DC_, "", &bLogOptimiseErrors, this, isParm, DDBYesNo);
  DDB.CheckBoxBtn("TrackNotes", "", DC_, "", &bLogOptimiseNotes, this, isParm, DDBYesNo);
  DDB.CheckBoxBtn("TrackSetData", "", DC_, "", &bLogSetOptimiseData, this, isParm, DDBYesNo);
  DDB.Text("");
  //DDB.Text    ("Results");
  DDB.String("State",        "", DC_,    "",    &m_StateLine[0],  this, isResult|noFileAtAll);
  //DDB.Text("Error:");
  DDB.String("Msg_1",        "", DC_,    "",    &m_StateLine[1],  this, isResult|noFileAtAll);
  //DDB.String("Msg_2",        "", DC_,    "",    &m_StateLine[2],  this, isResult|noFileAtAll);
  //DDB.Text("");
  //DDB.Text("----------------------------------------");


  // Global Assay Definitions
  DDB.Page("ASSY", DDB_RqdPage);
  DDB.Text("");
  const int cc = gs_Assays.GetAssaySumCount();
  Strng Tg;
  Tg.Set("AS");
  DDB.Short("AssaySumCount", "", DC_, "", xidBCAssaySumCnt , this, isParmStopped|SetOnChange);
  if (DDB.BeginArray(this, Tg(), "SPG_AssaySum", gs_Assays.GetAssaySumCount()))
    {
    for (int i=0; i<gs_Assays.GetAssaySumCount(); i++)
      {
      DDB.BeginElement(this, i);
      gs_Assays.m_Assays[i]->BuildDataDefn(this, DDB, i);
      }
    }
  DDB.EndArray();

  // Criteria Definitions
  DDB.Page("BC", DDB_RqdPage);
  DDB.Text("");
  m_BC.BuildDataDefn(this, DDB);
  DDB.Page("Crit", DDB_RqdPage);
  DDB.Text("");
  m_BC.BuildCriteriaDataDefn(this, DDB);

  // For display only
  DDB.Object(&mStockpileAssays,this,NULL,NULL,DDB_RqdPage); 

  #if WithMethod1
  // Optimisation Lib Input/Output Data
  DDB.Object(&mAssays, this, NULL, NULL, DDB_RqdPage);
  DDB.Object(&mHardConstraints, this, NULL, NULL, DDB_RqdPage);
  DDB.Object(&mSoftConstraints, this, NULL, NULL, DDB_RqdPage);
  DDB.Object(&mSoftRatios, this, NULL, NULL, DDB_RqdPage);
  DDB.Object(&mX, this, NULL, NULL, DDB_RqdPage);
  DDB.Object(&mSoftConstraintCosts, this, NULL, NULL, DDB_RqdPage);
  DDB.Object(&mSoftRatiosCosts, this, NULL, NULL, DDB_RqdPage);
  #endif

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CBlendCon::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;

  //
  // Assay Sum Count - Number of Defined Assays for Project
  //
  if (DCB.lHandle==xidBCAssaySumCnt)
    {
    if (DCB.rS)
      {
      //
      // Should move hooks into assay sum to AssaySumHelper
      //
      gs_Assays.SetAssaySumCount(Range((short)0, *DCB.rS, MaxAssaySum));
      for (int i=0;i<gs_Assays.GetAssaySumCount();i++)
        {
        pCAssaySum ptr = gs_Assays.m_Assays[i]->m_pBCrit;
        if (ptr->m_sName.Len()<1)
          ptr->m_sName.Set("Assay%d", i);
        }
      }
    DCB.S=(short)gs_Assays.GetAssaySumCount();
    return 1;
    }


  //
  // Assay Sum List Count
  //
  if (DCB.lHandle>=xidBCNSpcs1 && DCB.lHandle<xidBCNSpcs1+MaxAssaySum)
    {
    int AssySumIndex=DCB.lHandle-xidBCNSpcs1;
    if (DCB.rL)
      gs_Assays.GetAssaySum(AssySumIndex)->InitIndexList1(*DCB.rL);
    DCB.L=gs_Assays.GetAssaySum(AssySumIndex)->List1Cnt();
    return 1;
    }

  //
  // AssaySum InsertIndex List Members 
  //
  if (DCB.lHandle>=xidBCAddSpc && DCB.lHandle<xidBCAddSpc+MaxAssaySum*MaxSpeciesPerAssaySum)
    {
    if (DCB.rL)
      {
      int j=DCB.lHandle-xidBCAddSpc;
      int AssySumIndex=j/MaxSpeciesPerAssaySum;
      if (*DCB.rL>=0)
        {
        if (gs_Assays.GetAssaySum(AssySumIndex)->InsertIndexList1(*DCB.rL))
          gs_Assays.m_Assays[AssySumIndex]->m_bDDBValid = false;
        DCB.L=-1;
        return 1;
        }
      }
    return 1;
    }

  //
  // AssaySum Remove Index List Members
  //
  if (DCB.lHandle>=xidBCRemSpc && DCB.lHandle<xidBCRemSpc+MaxAssaySum*MaxSpeciesPerAssaySum)
    {
    if (DCB.rL)
      {
      int j=DCB.lHandle-xidBCRemSpc;
      int AssySumIndex=j/MaxSpeciesPerAssaySum;
      if (*DCB.rL>=0)
        {
        if (gs_Assays.GetAssaySum(AssySumIndex)->RemoveIndexList1(*DCB.rL))
          gs_Assays.m_Assays[AssySumIndex]->m_bDDBValid = false;
        DCB.L=-1;
        return 1;
        }
      }
    return 1;
    }

  //
  // Criteria Count
  //
  if (DCB.lHandle==xidBCCritCnt)
    {
    if (DCB.rS)
      m_BC.SetCriteriaCount(Range((short)0, *DCB.rS, MaxCriteria));
    DCB.S=m_BC.CriteriaCount();
    return 1;
    }

  //
  // Blend Controller Optimise Button
  //
  if (DCB.lHandle==xidBCOptimise)
    {
    //if (DCB.rB && (*DCB.rB!=0) && DCB.ForView())
    if (DCB.rB && (*DCB.rB!=0) && !DCB.ForFileSnpScn())
      {
      m_BC.Optimise(this, false);
      }
    DCB.B=0;
    return 1;
    }

  //
  // Blend Controller Optimise Button
  //
  if (DCB.lHandle==xidBCOptimiseSet)
    {
    //if (DCB.rB && (*DCB.rB!=0) && DCB.ForView())
    if (DCB.rB && (*DCB.rB!=0) && !DCB.ForFileSnpScn())
      {
      m_BC.Optimise(this, true);
      }
    DCB.B=0;
    return 1;
    }


  return False;
  }

//--------------------------------------------------------------------------

void CBlendCon::GlobalConnect()
  {
  FlwNode::GlobalConnect();
  //BCI.Connect(this);
  m_pStorage.m_sTag=sMSTag;
  bValid = m_pStorage.Connect(this, -1);
  if (bValid)
    {
    // Initialise the Blend Controller
    // Based on the Connected Multi Storage

    CMultiStorage* lp_MultiStore = (CMultiStorage*)m_pStorage;
    //const long TankCount = m_pStorage.m_Ptr->GetPileCount();
    const long TankCount = lp_MultiStore->GetPileCount();
    const long ComponentCnt = SDB.Count()/*.No()*/;
    m_BC.InitBlendControl(TankCount, ComponentCnt);
    }

  };

// -------------------------------------------------------------------------

void CBlendCon::GlobalDisConnect()
  {
  //BCI.DisConnect();
  m_pStorage.DisConnect();
  FlwNode::GlobalDisConnect();
  }

//--------------------------------------------------------------------------

flag CBlendCon::PreStartCheck()
  {
  for (int i=0; i<3; i++)
    m_StateLine[i] = "";
  m_StateLine[0] = "OK";
  return FlwNode::PreStartCheck();
  }

//--------------------------------------------------------------------------

bool CBlendCon::IsXRefListActive() { return !GetActiveHold() && bOn!=0; }

//---------------------------------------------------------------------------

int CBlendCon::UpdateXRefLists(CXRefBuildResults & Results)
  {
  if (1)//bOn)
    {
    //FnMngrClear();
    int FunctNo = 0;
    //CMultiStorage *pS1=BCI.m_pStorage;
    CMultiStorage *pS1=m_pStorage; 
    //CMultiStore   *pS2=BCI.m_pStore;
    //FnMngrTryUpdateXRefLists(Results);
    }
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void CBlendCon::UnlinkAllXRefs()
  {
  //FnMngrClear();
  //FnMngrTryUnlinkAllXRefs();

  CNodeXRefMngr::UnlinkAllXRefs();
  }

//--------------------------------------------------------------------------

void CBlendCon::EvalCtrlStrategy(eScdCtrlTasks Tasks)
  {
  if (bOn && !GetActiveHold() && ICGetTimeInc() > 0.0)
    {
    GetNearXRefValues();

    //BCI.EvalCtrlStrategy(eScdCtrlTasks Tasks);
    if (bValid)
      {
      CMultiStorage *pS = m_pStorage;
      //LogNote("XXX", 0, "Mass:%d", pS->GetPileCount());
      }

    SetNearXRefValues();
    }
  }

//--------------------------------------------------------------------------

void CBlendCon::EvalProducts(long JoinMask)
  {
  }

//--------------------------------------------------------------------------

bool BlendCon_ChangeTag(pchar pOldTag, pchar pNewTag, Strng &sTag)
  {
  if (sTag() && sTag.IsNotEmpty())
    {
    const int len = strlen(pOldTag);
    const int TLen = sTag.Len();
    if (TLen>=len)
      {
      if (_strnicmp(pOldTag, sTag(), len)==0 && (TLen==len || sTag[len]=='.' || sTag[len]==' '))
        {
        Strng s;
        s = pNewTag;
        s += sTag.Mid(len, 256);
        sTag = s();
        return true;
        }
      }
    }
  return false;
  }

int CBlendCon::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  bool DidChange = BlendCon_ChangeTag(pOldTag, pNewTag, sMSTag);
  DidChange = DidChange || BlendCon_ChangeTag(pOldTag, pNewTag, sPipeTag);

  if (DidChange)
    {
    PreStartCheck();
    }
  return EOCT_DONE;
  }

//--------------------------------------------------------------------------

bool BlendCon_DeleteTag(pchar pDelTag, Strng &sTag)
  {
  const int len = strlen(pDelTag);
  if (sTag() && sTag.IsNotEmpty())
    {
    const int TLen = sTag.Len();
    if (TLen>=len)
      {
      if (_strnicmp(pDelTag, sTag(), len)==0 && (TLen==len || sTag[len]=='.' || sTag[len]==' '))
        return true;
      }
    }
  return false;
  }

int CBlendCon::DeleteTag(pchar pDelTag)
  {
  bool bContainsTag = BlendCon_DeleteTag(pDelTag, sMSTag);
  bContainsTag = bContainsTag || BlendCon_DeleteTag(pDelTag, sPipeTag);

  //if (BCI.ContainsTag(pDelTag))
  if (bContainsTag)
    {
    LogNote(Tag(), 0, "Delete tag '%s' affects Controller model '%s'.", pDelTag, Tag());
    PreStartCheck();
    }
  return EODT_DONE;
  }

//--------------------------------------------------------------------------
                                          
dword CBlendCon::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=bOn ? FNS_On : FNS_Off;
  return Status;
  }

//--------------------------------------------------------------------------

flag CBlendCon::CIStrng(int No, pchar & pS)
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

CAssaySumHelper::CAssaySumHelper()
  {
  m_iMyIndex = -1;
  m_pBCrit = NULL;
  m_bDDBValid = false;
  }

//--------------------------------------------------------------------------

void CAssaySumHelper::SetUpDDB()
  {
  bool InAList[MaxSpeciesEx];

  if (!m_bDDBValid)
    {
    for (int s=0; s<SDB.Count(); s++)
      InAList[s]=false;
    m_DDBAdd.Empty();
    m_DDBRemove.Empty();
    m_DDBAdd.Add(-1, " - ");
    m_DDBRemove.Add(-1, " - ");

    for (int j=0; j<m_pBCrit->m_iList1Cnt; j++)
      {
      int s=m_pBCrit->m_pIndexList1[j];
      if (s>=0 && !InAList[s])
        {
        InAList[s]=true;
        m_DDBRemove.Add(s, SDB[s].SymOrTag());
        }
      }

    for (int j=0; j<SDB.Count(); j++)
      {
      if (!InAList[j])// && SDB[j].IsSpecie)
        m_DDBAdd.Add(j, SDB[j].SymOrTag());
      }
    m_bDDBValid = true;
    }
  }

//--------------------------------------------------------------------------

void CAssaySumHelper::BuildDataDefn(FlwNode* pTagObj, DataDefnBlk & DDB, int AssayIndex)
  {
  DDB.String ("Name",   "", DC_, "", &(m_pBCrit->m_sName), pTagObj, isParmStopped);
  if (DDB.ForFileSnpScn())
    {
    DDB.Long  ("NSpcs", "", DC_, "", xidBCNSpcs1+AssayIndex, pTagObj, isParm|SetOnChange);
    const int Count = m_pBCrit->m_iList1Cnt;
      {
      for (int s=0; s<m_pBCrit->m_iList1Cnt; s++)
        {
        Strng SpcTg;
        SpcTg.Set("SPC%d",s);
        DDB.Int(SpcTg(), "", DC_, "", &(m_pBCrit->m_pIndexList1[s]), pTagObj, isParmConstruct, &SDB.DDBSpList);
        }
      }
    }
  else
    {
    Strng S("Species:");
    bool NeedsComma=false;
    for (int s=0; s<m_pBCrit->m_iList1Cnt; s++)
      {
      if (NeedsComma)
        S+=",";
      const int ii = m_pBCrit->m_pIndexList1[s];
      S+=(ii>=0 ? SDB[ii].SymOrTag() : "?");
      if (S.Length()>35)
        {
        DDB.Text(S());
        S="        ";
        NeedsComma=false;
        }
      else
        NeedsComma=true;
      }
    if (S.Length()>8)
      DDB.Text(S());
    SetUpDDB();
    DDB.Long  ("Add", "", DC_, "", xidBCAddSpc+(MaxSpeciesPerAssaySum*AssayIndex),
                pTagObj, (m_DDBAdd.Length()>1?isParm:0)|SetOnChange|noFileAtAll,
                &m_DDBAdd);
    DDB.Long  ("Remove", "", DC_, "", 
                xidBCRemSpc+(MaxSpeciesPerAssaySum*AssayIndex), pTagObj, 
                (m_DDBRemove.Length()>1?isParm:0)|SetOnChange|noFileAtAll, &m_DDBRemove);
    DDB.Text("");
    }
  }

//==========================================================================

CBlendCriteriaHelper::CBlendCriteriaHelper()
  {
  m_iMyIndex = -1;
  m_pBCrit = NULL;
  m_bDDBValid = false;
  }

//--------------------------------------------------------------------------

void CBlendCriteriaHelper::SetUpDDB()
  {


  }

//--------------------------------------------------------------------------

void CBlendCriteriaHelper::BuildDataDefn(FlwNode* pTagObj, DataDefnBlk & DDB, int CriteriaIndex)
  {

  static DDBValueLst DDB0[]={
    {CBlendCriteria::eCT_Soft, "Soft"},
    {CBlendCriteria::eCT_Ratio,"Ratio"},
    {0}};

  static DDBValueLst DDB1[]={
    {CBlendCriteria::eCT_Greater, "Greater"},
    {CBlendCriteria::eCT_Lesser,  "Lesser"},
    {0}};

  DDBValueLst DDB_AssayNames[MaxAssaySum+1];

  DDB.String ("Comment", "", DC_, "", &(m_pBCrit->m_sName), pTagObj, isParm);
  DDB.Long   ("Type", "", DC_, "", (long*)&(m_pBCrit->m_eConstraintType), pTagObj, isParmStopped|SetOnChange, DDB0);
  DDB.Long   ("Comp", "", DC_, "", (long*)&(m_pBCrit->m_eCompType), pTagObj, isParmStopped|SetOnChange, DDB1);
  DDB.Double ("Weighting", "", DC_, "", &(m_pBCrit->m_dWeighting), pTagObj, isParm);
  DDB.Double ("Requirement", "", DC_Frac , "%", &(m_pBCrit->m_dRequirement), pTagObj, isParm);
  DDB.Double ("Result", "", DC_Frac, "%", &(m_pBCrit->m_dResult), pTagObj, isResult);
  DDB.Double ("Cost",   "", DC_    , "",  &(m_pBCrit->m_dCost), pTagObj, isResult);

  // Populate the Assay Sum List
  for (int i=0;i<gs_Assays.GetAssaySumCount();i++)
    {
    pCAssaySum ptr = gs_Assays.m_Assays[i]->m_pBCrit;
    DDB_AssayNames[i].m_dwFlags = 0;
    DDB_AssayNames[i].m_lVal = i;
    DDB_AssayNames[i].m_pStr = ptr->m_sName.Str();
    }
  DDB_AssayNames[i].m_lVal = 0;
  DDB_AssayNames[i].m_pStr = 0;

  if (m_pBCrit->m_eConstraintType == CBlendCriteria::eCT_Ratio)
    {
    // Need To Select Two Assay Sums for the constraint
    DDB.Long("A1", "", DC_, "", (long*)&(m_pBCrit->m_AssayIndex1), pTagObj, isParmStopped|SetOnChange, DDB_AssayNames);
    DDB.Long("A2", "", DC_, "", (long*)&(m_pBCrit->m_AssayIndex2), pTagObj, isParmStopped|SetOnChange, DDB_AssayNames);
    }
  else
    {
    // Need to select one Assay Sum for the constraint
    DDB.Long("A1", "", DC_, "", (long*)&(m_pBCrit->m_AssayIndex1), pTagObj, isParmStopped|SetOnChange, DDB_AssayNames);
    }

  DDB.Text("");

  }


//==========================================================================

CBlendControlHelper::CBlendControlHelper(/*int MyIndex*/)
  {
  //m_iMyIndex = MyIndex;
  m_iStreamIndex = -1;
  m_iIOIndex = -1;
  m_iOptimiseMethod = 1;
  tankMin = NULL;
  tankOpt = NULL;
  }

//--------------------------------------------------------------------------

CBlendControlHelper::~CBlendControlHelper()
  {
  SetCriteriaCount(0);
  if (tankMin)
    {
    delete tankMin;
    tankMin = NULL;
    }
  if (tankOpt)
    {
    delete tankOpt;
    tankOpt = NULL;
    }
  }

//--------------------------------------------------------------------------

void CBlendControlHelper::InitBlendControl(long TankCount, long ComponentCnt)
  {
  InitTanks(TankCount, ComponentCnt);
  }

//--------------------------------------------------------------------------

void CBlendControlHelper::BuildDataDefn(FlwNode* pTagObj, DataDefnBlk & DDB)
  {
  static DDBValueLst DDB0[]={
    {0, "Method0"},
    {1, "Method1"},
    {0}};

  DDBValueLstMem DDBSt;
  char Buff[64];
  int ProdCount = 0;
  int i;

  DDB.Short   ("CriteriaCount", "", DC_, "", xidBCCritCnt, pTagObj, isParmStopped|SetOnChange);
  //DDB.Short   ("OptimiseMethod", "", DC_, "", &m_iOptimiseMethod, pTagObj, isParmStopped, DDB0);
  DDB.Double  ("Rqd", "", DC_M, "kg", &m_dTotalRqd, pTagObj, isParm);
  DDB.Text("");
  DDB.Button("Optimise", "", DC_, "", xidBCOptimise, pTagObj, isParm);
  DDB.Button("Optimise_Set", "", DC_, "", xidBCOptimiseSet, pTagObj, isParm);
  DDB.Text("");

  for (i=0; i<m_iTankCnt; i++)
    {
    sprintf(Buff, "Tank.[%d].FracMin", i);
    DDB.Double (Buff/*"FracMin"*/, "", DC_Frac, "%", &(m_pMin[i]), pTagObj, isParm);
    sprintf(Buff, "Tank.[%d].FracMax", i);
    DDB.Double (Buff/*"FracMax"*/, "", DC_Frac, "%", &(m_pMax[i]), pTagObj, isParm);
    sprintf(Buff, "Tank.[%d].Frac", i);
    DDB.Double (Buff/*"Frac"*/, "", DC_Frac, "%", &(m_pResultSplits[i]), pTagObj, isResult);
    }

  }

//--------------------------------------------------------------------------

void CBlendControlHelper::BuildCriteriaDataDefn(FlwNode * pTagObj, DataDefnBlk & DDB)
  {
  DDB.Text("");
  const int cc = CriteriaCount();

  //
  // Criteria Data Definition
  //

  Strng  Tg;
  //Tg.Set("CR%d", m_iMyIndex);
   Tg.Set("CR");
  if (DDB.BeginArray(pTagObj, Tg(), "SPG_BlendCriteria", CriteriaCount()))
    {
    for (int i=0; i<cc; i++)
      {
      if (DDB.BeginElement(pTagObj, i))
        {
        Strng       m_sName;
        m_BCH[i]->BuildDataDefn(pTagObj, DDB , /*m_iMyIndex*/i );
        }
      }
    }
  DDB.EndArray();
  }

//--------------------------------------------------------------------------

flag CBlendControlHelper::DataXchg(DataChangeBlk & DCB)
  {
  return 0;
  }

//--------------------------------------------------------------------------

void CBlendControlHelper::InitTanks(long TankCnt, long ComponentCnt)
  {
  if (TankCnt!=m_iTankCnt || ComponentCnt!=m_iComponentCnt)
    {
    long PrevTankCount = m_iTankCnt;
    long PrevComponentCnt = m_iComponentCnt;
    pDbl * pContent = NULL;
    double * pTotal = NULL;
    double * pMin = NULL;
    double * pMax = NULL;
    double * pResultSplits = NULL;
    if (TankCnt>0 && ComponentCnt>0)
      {
      pContent = new pDbl[TankCnt];
      for (int i=0; i<TankCnt; i++)
        pContent[i] = new double[ComponentCnt];
      pTotal = new double[TankCnt];
      pMin = new double[TankCnt];
      pMax = new double[TankCnt];
      pResultSplits = new double[TankCnt];

      for (int i=0; i<min(PrevTankCount, TankCnt); i++)
        {
        for (int j=0; j<min(PrevComponentCnt, ComponentCnt); j++)
          pContent[i][j] = m_pContent[i][j];
        for (int j=PrevComponentCnt; j<ComponentCnt; j++)
          pContent[i][j] = 0.0;
        pTotal[i] = m_pTotal[i];
        pMin[i] = m_pMin[i];
        pMax[i] = m_pMax[i];
        pResultSplits[i] = m_pResultSplits[i];
        }
      for (int i=PrevTankCount; i<TankCnt; i++)
        {
        for (int j=0; j<ComponentCnt; j++)
          pContent[i][j] = 0.0;
        pTotal[i] = 0.0;
        pMin[i] = 0.0;
        pMax[i] = 1.0;
        pResultSplits[i] = 0.0;
        }
      }
    DestroyContent();
    m_iTankCnt = TankCnt;
    m_iComponentCnt = ComponentCnt;
    if (TankCnt>0 && ComponentCnt>0)
      {
      m_pContent = pContent;
      m_pTotal = pTotal;
      m_pMin = pMin;
      m_pMax = pMax;
      m_pResultSplits = pResultSplits;
      }
    }
  }

//--------------------------------------------------------------------------

void CBlendControlHelper::SetCriteriaCount(short CriteriaCnt)
  {
  InitCriteria(CriteriaCnt);

  int PrevCnt = m_BCH.GetSize();
  if (CriteriaCnt<m_BCH.GetSize())
    {
    for (int i=CriteriaCnt; i<PrevCnt; i++)
      delete m_BCH[i];
    }
  m_BCH.SetSize(CriteriaCnt);
  for (int i=PrevCnt; i<m_BCH.GetSize(); i++)
    m_BCH[i] = new CBlendCriteriaHelper;

  for (int i=0; i<m_iCriteriaCnt; i++)
    {
    m_BCH[i]->m_iMyIndex = i;
    m_BCH[i]->m_pBCrit = GetCriteria(i);
    m_BCH[i]->m_bDDBValid = false;
    }

  }

//--------------------------------------------------------------------------

void CBlendControlHelper::InitCriteria(long CriteriaCount)
  {
  if (CriteriaCount!=m_iCriteriaCnt)
    {

    long PrevCriteriaCount = m_iCriteriaCnt;
    pCBlendCriteria * pCriteria = NULL;
    if (CriteriaCount>0)
      {
      pCriteria = new pCBlendCriteria[CriteriaCount];
      for (int i=0; i<min(PrevCriteriaCount, CriteriaCount); i++)
        {
        pCriteria[i] = m_pCriteria[i];
        }
      for (int i=PrevCriteriaCount; i<CriteriaCount; i++)
        {
        pCriteria[i] = new CBlendCriteria;
        }
      }
    for (int i=CriteriaCount; i<PrevCriteriaCount; i++)
      delete (m_pCriteria[i]);

    m_iCriteriaCnt = CriteriaCount;
    m_pCriteria = pCriteria;
    }
  }

//==========================================================================

void CBlendControlHelper::Optimise(CBlendCon* pBlender, bool SetIt)
  {
  m_iOptimiseMethod = 2;
  if (m_iOptimiseMethod==0)
    Optimise0(pBlender, SetIt);
  else if (m_iOptimiseMethod==1)
    Optimise1(pBlender, SetIt);
  else
    Optimise2(pBlender, SetIt);
  }

//--------------------------------------------------------------------------

void CBlendControlHelper::Optimise0(CBlendCon* pBlender, bool SetIt)
  {
  #if WithMethod1
  //
  // Get a pointer to the connected Multi-Storage Unit
  //
  pBlender->m_StateLine[0] = "";
  pBlender->m_StateLine[1] = "";
  pBlender->m_StateLine[2] = "";
  CMultiStorage* lpMultiStore = (CMultiStorage*)(pBlender->m_pStorage);
  if (lpMultiStore == NULL)
    {
    pBlender->m_StateLine[0] = "Error: Multistore not connected";
    LogError(pBlender->Tag(), 0, "Optimise Failed: Multistore %s not connected!", pBlender->sMSTag());
    return;
    }

  const int Elements = 4;
  int iSoftConstraintsIndex = 0;
  int iSoftRatioIndex = 0;
  int iNumSoftConstraints = 0;
  int iNumRatioConstraints = 0;

  double dBlendTonnes = m_dTotalRqd; // This is the blend amount required

  //
  // Sanity Checks
  //
  if (dBlendTonnes <= 0.0)
    {
    pBlender->m_StateLine[0] = "Error: Target Mass is zero";
    LogError(pBlender->Tag(), 0, "Optimise Failed: Target Mass is zero");
    return;
    }

  //
  // Calculate the number of SoftConstraints and the Number of Ratio Constraints
  //

  for (int i=0; i<m_iCriteriaCnt; i++)
    {
    CBlendCriteria* pBCrit = m_BCH[i]->m_pBCrit;
    if (pBCrit->m_eConstraintType==CBlendCriteria::eCT_Ratio)
      iNumRatioConstraints++;
    else 
      iNumSoftConstraints++;
    }

  // Sum of Assays Array - Each Column represents a storage bin (tank)
  pBlender->mAssays.SetDim(gs_Assays.GetAssaySumCount(), m_iTankCnt);

  // Hard Constraints for each storage bin (minimum and maximum amounts
  // that can be taken to meet the blend requirement)
  pBlender->mHardConstraints.SetDim(2, m_iTankCnt);

  //
  // The Soft Constraint Dimension should be the same as the
  // number of Assay Sums. We leave the constraint data set at zero 
  // in the Soft Constraint Matrix if a soft constraint is not specified
  //
  pBlender->mSoftConstraints.SetDim(4,gs_Assays.GetAssaySumCount());

  //
  // The ratio constraint dimension depends on the number of ratio type criteria
  //
  if (iNumRatioConstraints > 0)
    pBlender->mSoftRatios.SetDim(iNumRatioConstraints,5);
  else
    {
    // Optimiser code requires a non empty matrix
    // Set values to 0 so they are not used
    pBlender->mSoftRatios.SetDim(1,5);
    }

  pBlender->mSoftRatiosCosts.SetDim(pBlender->mSoftRatios.RowCount(),1);

  // Results Matrix
  pBlender->mX.SetDim(m_iTankCnt, 1);

  // Initialise matrices to 0 before setting individual elements
  pBlender->mSoftConstraints *= 0.0;
  pBlender->mHardConstraints *= 0.0;
  pBlender->mSoftRatios *= 0.0;

  // First we need to extract the individual assays from the SysCAD Species.
  // Each SysCAD Species can exist as various phases and we need to add up all the
  // phases for a particular species as defined by the Blending Criteria in each stockpile.
  // For example Ti02 can exist in IL1 and IL2 etcetera. We need to add up all the TiO2 in all
  // the forms specified by the Blending Criteria

  // Set Column Names to be in the Form Si where S means stockpile
  pBlender->mAssays.SetColPrefix("S");
  // Set Row Names to be in the Form Assyi where Assy means the calculated Assay Sum
  pBlender->mAssays.SetRowPrefix("Assy");

  // for each assay sum data specification
  for (int i=0; i<gs_Assays.GetAssaySumCount(); i++)
    {
    CAssaySum* pAssaySumData = gs_Assays.m_Assays[i]->m_pBCrit;
  	pBlender->mAssays.SetRowName(i,pAssaySumData->m_sName.Str());

    //
    // Calculate the Assay Sum as specified
    //

    // for each stockpile
    for (int t=0; t<m_iTankCnt; t++)
      {
      CMultiStore & St = lpMultiStore->m_Store[t];
      double sum = 0.0;
      double Mtot = 0.0;

      Mtot = St.Mass();// kgs

      if ((Mtot>0.0) && (dBlendTonnes>0.0))
        {
        if ((Mtot/dBlendTonnes) > 0.02)
          {
          // sum all the assay data for the species specified in the AssaySumData
          for (int s=0; s<pAssaySumData->m_iList1Cnt; s++)
            {
            //sum += St.m_pContents->M[pAssaySumData->m_pIndexList1[s]];

            double M = St.VMass[pAssaySumData->m_pIndexList1[s]];
            sum += M;		 

            } // end_for each species

          //
          // Percentage of Total Mass in Stockpile
          //
          pBlender->mAssays[i][t] = sum/Mtot;
          }
        else
          {   
          // Less than 2% in the stockpile so set assays to zero
          // so they will not be included in optimisation calculations
          pBlender->mAssays[i][t] = 0.0;
          }
        }
      else
        {
        // Stockpile is empty or Specified dBlendTonnes is zero
        pBlender->mAssays[i][t] = 0.0;

        }

      } // end_for each stockpile

    } // end_for each assay sum specification

  if (pBlender->bLogOptimiseNotes)
    {
    // Print out Assay Matrix
    pBlender->mAssays.display_mode = ZMat::smOneLine;
    ZString zstr = ZString(pBlender->mAssays);
    LogNote("Assays", 0, zstr.c_str());
    }

  //
  // Calculate the Criteria for display only
  //
  iSoftRatioIndex=0;
  pBlender->mStockpileAssays.SetDim(2*m_iCriteriaCnt,m_iTankCnt);
  pBlender->mStockpileAssays.SetColPrefix("S");

  for (int i=0; i<m_iCriteriaCnt; i++)
  {
	  CBlendCriteria* pBCrit = m_BCH[i]->m_pBCrit;
	  Strng s;

	  if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
		  s = pBCrit->m_sName + " < " + ftoa(pBCrit->m_dRequirement);
	  else
		  s = pBCrit->m_sName + " > " + ftoa(pBCrit->m_dRequirement);

	  pBlender->mStockpileAssays.SetRowName(i,s.Str());

	  s = "d " + pBCrit->m_sName; // distance away from criteria
	  pBlender->mStockpileAssays.SetRowName(m_iCriteriaCnt+i,s.Str());

	  if ((pBCrit->m_eConstraintType==CBlendCriteria::eCT_Ratio))
	  {
		  // Get the index of the Assays for the Ratio
		  int Gamma1 = pBCrit->m_AssayIndex1;
		  int Gamma2 = pBCrit->m_AssayIndex2;

		  // for each stockpile
		  for (int t=0; t<m_iTankCnt; t++)
		  {
			  CMultiStore & St = lpMultiStore->m_Store[t];
			  double Assay1 = pBlender->mAssays[Gamma1][t];
			  double Assay2 = pBlender->mAssays[Gamma2][t];
			  double Ratio = 0.0;
			  if (Assay2 > 0.0)
			  {
				  Ratio = Assay1/Assay2;
			  }
			  pBlender->mStockpileAssays[i][t] = Ratio;

		  } // end_for each stockpile

		  iSoftRatioIndex++;
	  }
	  else
	  {
		  // Get the index of the Assay
		  int Gamma1 = pBCrit->m_AssayIndex1;

		  // for each stockpile
		  for (int t=0; t<m_iTankCnt; t++)
		  {
			  CMultiStore & St = lpMultiStore->m_Store[t];
			  double Assay1 = pBlender->mAssays[Gamma1][t];
			  pBlender->mStockpileAssays[i][t] = Assay1;

		  } // end_for each stockpile
	  }

  } // end_for each criteria specification

  // Check which Stockpile Meet the Critera
  for (int t=0; t<m_iTankCnt; t++)
  {
    bool bAllReqsMet = true;
	for (int i=0; i<m_iCriteriaCnt; i++)
	{
	  CBlendCriteria* pBCrit = m_BCH[i]->m_pBCrit;
	  double Error = 0.0;
	  if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
	  {
		if (pBlender->mStockpileAssays[i][t] >= pBCrit->m_dRequirement )
			bAllReqsMet = false;

		Error = pBCrit->m_dRequirement - pBlender->mStockpileAssays[i][t];
	  }
	  else
	  {
		if (pBlender->mStockpileAssays[i][t] <= pBCrit->m_dRequirement )
			bAllReqsMet = false;


		Error = pBlender->mStockpileAssays[i][t] - pBCrit->m_dRequirement;
	  }

	  // Store distance away from constraint for stockpile
	  pBlender->mStockpileAssays[m_iCriteriaCnt+i][t]=Error;

	}

	char buff[80];
	if (bAllReqsMet)
		sprintf(buff,"S%d (Y)",t);
	else
		sprintf(buff,"S%d (N)",t);

	pBlender->mStockpileAssays.SetColName(t,buff);
  }

  // for each blending criteria

  // Row Names for Display
  pBlender->mSoftConstraints.SetRowName(0,"Req >");
  pBlender->mSoftConstraints.SetRowName(1,"Req <");
  pBlender->mSoftConstraints.SetRowName(2,"Wt >");
  pBlender->mSoftConstraints.SetRowName(3,"Wt <");
  // Column Names For Display
  pBlender->mSoftConstraints.SetColPrefix("Assy");

  iSoftRatioIndex=0;
  for (int i=0; i<m_iCriteriaCnt; i++)
    {
    CBlendCriteria* pBCrit = m_BCH[i]->m_pBCrit;

    // Map The SysCAD Blending Criteria to The Optimisation Lib Format

    //    mSoftConstraints - a 4 x m matrix of constraints of the assay elements.
    //       The 1st row is a row of minima (greater-than) constraints. The 2nd row
    //       is contains the maxima (less-than) constraints. The 3rd and 4th rows
    //       contain the constraint weightings (I call alpha's) for the 1st and 2nd
    //       rows respectively. If an element of the 3rd or 4th row is 0, the
    //       corresponding constraint in the 1st or 2nd row is ignored.
    //    enum eCalcType {eCT_Sum, eCT_Ratio };
    //    enum eCompType {eCT_Equal, eCT_Greater, eCT_Lesser };


    if ((pBCrit->m_eConstraintType==CBlendCriteria::eCT_Soft) /*&& (pBCrit->m_iList1Cnt > 0)*/)
      {

      // Get the index of the Assay Sum that the Soft Constraint Applies to
      //int AssaySumIndex = pBCrit->m_pIndexList1[0];
      int AssaySumIndex = pBCrit->m_AssayIndex1;

	   pBlender->mSoftConstraints.SetColName(AssaySumIndex,pBCrit->m_sName.Str());


      if (pBCrit->m_eCompType==CBlendCriteria::eCT_Greater)
        {
        pBlender->mSoftConstraints[0][AssaySumIndex] = pBCrit->m_dRequirement;
        pBlender->mSoftConstraints[2][AssaySumIndex] = pBCrit->m_dWeighting;
        }
      else
        {
        pBlender->mSoftConstraints[0][AssaySumIndex] = 0.0;
        pBlender->mSoftConstraints[2][AssaySumIndex] = 0.0;
        }
      if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
        {
        pBlender->mSoftConstraints[1][AssaySumIndex] = pBCrit->m_dRequirement;
        pBlender->mSoftConstraints[3][AssaySumIndex] = pBCrit->m_dWeighting;
        }
      else
        {
        pBlender->mSoftConstraints[1][AssaySumIndex] = 0.0;
        pBlender->mSoftConstraints[3][AssaySumIndex] = 0.0;
        }

      }

    //    mSoftRatios - a p x 5 matrix of ratio constraints where p is the number
    //       of constraints. The 1st and 2nd columns are the indices of the
    //       dividend (numerator, I call gamma_i) and divisor (denominator, I call
    //       gamma_j) of the ratio respectively. 
    //       The 3rd column is the ratio constraint (I call r). 
    //       The 4th column is the weighting (alpha).
    //       The 5th (last) column is a boolean representing whether the ratio
    //       constraint is a greater-than (1) or less-than (0) constraint. i.e. if
    //       column 5 = 1, then the inequality constraint is:
    //             (gamma_i/gamma_j) > r

    // Column Names for Display
    pBlender->mSoftRatios.SetColName(0,"a1");
    pBlender->mSoftRatios.SetColName(1,"a2");
    pBlender->mSoftRatios.SetColName(2,"r");
    pBlender->mSoftRatios.SetColName(3,"wt");
    pBlender->mSoftRatios.SetColName(4,"1 >");


    if ((pBCrit->m_eConstraintType==CBlendCriteria::eCT_Ratio)/*&&(pBCrit->m_iList1Cnt == 2)*/)
      {
      // Row Names For Display
      char temp[80];
      sprintf(temp,"Crit%d",i);
      pBlender->mSoftRatios.SetRowName(iSoftRatioIndex,pBCrit->m_sName.Str());
      //pBlender->mSoftRatios.SetRowName(iSoftRatioIndex,temp);
      pBlender->mSoftRatiosCosts.SetRowName(iSoftRatioIndex,pBCrit->m_sName.Str());
      //pBlender->mSoftRatiosCosts.SetRowName(iSoftRatioIndex,temp);


      // Get the index of the Assay Sum that the Soft Constraint Applies to
      //int Gamma1 = pBCrit->m_pIndexList1[0];
      //int Gamma2 = pBCrit->m_pIndexList1[1];
      int Gamma1 = pBCrit->m_AssayIndex1;
      int Gamma2 = pBCrit->m_AssayIndex2;

      pBlender->mSoftRatios[iSoftRatioIndex][0] = Gamma1; // Assay 2
      pBlender->mSoftRatios[iSoftRatioIndex][1] = Gamma2; // Assay 4
      pBlender->mSoftRatios[iSoftRatioIndex][2] = pBCrit->m_dRequirement; // Assay 2/ Assay4 > 6
      pBlender->mSoftRatios[iSoftRatioIndex][3] = pBCrit->m_dWeighting; // Weighting 1
      if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
        pBlender->mSoftRatios[iSoftRatioIndex][4] = 0.0; // Less Than
      else
        pBlender->mSoftRatios[iSoftRatioIndex][4] = 1.0; // Greater Than

      iSoftRatioIndex++;
      }

    }

  if (pBlender->bLogOptimiseNotes)
    {
    // Print out Constraints Matrix
    pBlender->mSoftConstraints.display_mode = ZMat::smOneLine;
    ZString zstr = ZString(pBlender->mSoftConstraints);
    LogNote("SC", 0, zstr.c_str());
    pBlender->mSoftRatios.display_mode = ZMat::smOneLine;
    zstr = ZString(pBlender->mSoftRatios);
    LogNote("RC", 0, zstr.c_str());
    }

  //    mHardConstraints - a 2 x m matrix of constraints of the assay elements.
  //       The 1st column is the minimum amount you can take from the stockpile - 0
  //       The second column is the maximum amount you can take from the stockpile (0-1)
  //       as a ratio of the amount of the target blend
  //       
  //       The maximum amount is calculated as the maximum that can be taken from the stockpile
  //	     as a ratio of the total blend. For example if the total blend is 60 tons and you
  //       only have 40 tons in the stockpile then you can take a maximum of 40/60 = 0.66
  //       

  // Row Names for Display
  pBlender->mHardConstraints.SetRowName(0,"Min");
  pBlender->mHardConstraints.SetRowName(1,"Max");
  // Column Names For Display
  pBlender->mHardConstraints.SetColPrefix("S");


  // for each stockpile
  for (int t=0; t<m_iTankCnt; t++)
    {
    CMultiStore & St = lpMultiStore->m_Store[t];
    double M = 0.0;

    M = St.Mass();// kgs

    // Minimum Constraint - Set to configured minimum
    pBlender->mHardConstraints[0][t] = /*0.0*/max(0.0,m_pMin[t]); 

    // Maximum Constraint - Set to minimum of configured maximum or most that can
    // be physcially taken from the stockpile

    // Adam had some code to set deadband stockpile masses for this algorithm
    // which sets the mass of the stockpile to 0 if it is less than 2% of the required blend
    // tons. Need to work out a good way to do this in SysCAD

    pBlender->mHardConstraints[1][t] = min(m_pMax[t],min(1.0,M/dBlendTonnes)); 

    } // end_for each stockpile


  // Call the optimisation routine

  bool OK = Optimise1stOrder(dBlendTonnes,
    pBlender->mAssays, 
    pBlender->mHardConstraints,
    pBlender->mSoftConstraints,
    pBlender->mSoftRatios,
    pBlender->mX);

	// Set status of Optimiser on Status Lines
	if (OK)
	  {
	  }
	else
	  {
	  }

  if (OK)
    {
		ZString s = OptimiseLastStatus();
		pBlender->m_StateLine[0] = s.c_str();

    // Calculate the Costs
    double dCost;

    pBlender->mSoftConstraintCosts.SetDim(pBlender->mAssays.RowCount(),1);
    pBlender->mSoftRatiosCosts.SetDim(pBlender->mSoftRatios.RowCount(),1);
    pBlender->mSoftRatiosCosts.SetColName(0,"Cost");
    pBlender->mSoftConstraintCosts.SetColName(0,"Cost");
    pBlender->mSoftConstraintCosts.SetRowPrefix("Assy");

    OK = CostFunction(pBlender->mX,
      pBlender->mAssays,
      pBlender->mSoftConstraints,
      pBlender->mSoftRatios,
      dCost,
      pBlender->mSoftConstraintCosts,
      pBlender->mSoftRatiosCosts);

    if ( !OK )
      {
      if (pBlender->bLogOptimiseErrors)
        LogError(pBlender->Tag(), 0, "CostFunction Error!");
  		pBlender->m_StateLine[1] = "CostFunction Error!";
      }

    // Copy Optimal Blending Results
    for (int t=0; t<m_iTankCnt; t++)
      {
      m_pResultSplits[t] = pBlender->mX[t][0];
      }

    // Set Resultant Assays when tanks are blended according to
    // optimiser results
    for (int i=0; i< gs_Assays.GetAssaySumCount(); i++)
      {
      CAssaySum* pAssaySumData = gs_Assays.m_Assays[i]->m_pBCrit;

      // for each stockpile
      //pAssaySumData->m_dResult = 0.0;
      double dResult = 0.0;
      for (int t=0; t<m_iTankCnt; t++)
        {
        double d1 = pBlender->mX[t][0];
        double d2 = pBlender->mAssays[i][t];
        //
        // TO DO: - Need to store result somewhere else
        // as Assay Definition is global
        //
        //pAssaySumData->m_dResult += d1*d2;
        dResult += d1*d2;
        }
      }

    // Criteria Results
    iSoftRatioIndex = 0;
    for (int i=0; i<m_iCriteriaCnt; i++)
      {
      CBlendCriteria* pBCrit = m_BCH[i]->m_pBCrit;

      // Calculate Optimised Criteria Value
      if ((pBCrit->m_eConstraintType==CBlendCriteria::eCT_Soft) )
        {
        // Get the index of the Assay Sum that the Soft Constraint Applies to
        int AssayIndex = pBCrit->m_AssayIndex1;

        pBCrit->m_dResult = 0.0;
        for (int t=0; t<m_iTankCnt; t++)
          {
          double d1 = pBlender->mX[t][0];
          double d2 = pBlender->mAssays[AssayIndex][t];
          pBCrit->m_dResult += d1*d2;
          }

        // Final Cost For Criteria
        pBCrit->m_dCost = pBlender->mSoftConstraintCosts[AssayIndex][0];
        pBlender->mSoftConstraintCosts.SetRowName(AssayIndex,pBCrit->m_sName.Str());
        }

      if ((pBCrit->m_eConstraintType==CBlendCriteria::eCT_Ratio) )
        {
        // Get the index of the Assay Sum that the Soft Constraint Applies to
        int AssayIndexNum = pBCrit->m_AssayIndex1;
        int AssayIndexDen = pBCrit->m_AssayIndex2;
        double dnum = 0.0;
        double dden = 0.0;
        for (int t=0; t<m_iTankCnt; t++)
          {
          dnum += pBlender->mX[t][0]*pBlender->mAssays[AssayIndexNum][t];
          dden += pBlender->mX[t][0]*pBlender->mAssays[AssayIndexDen][t];
          }
        if (dden > 0.0)
          pBCrit->m_dResult = dnum/dden;
        else
          pBCrit->m_dResult = 0.0;

        // Final Cost For Criteria
        pBCrit->m_dCost = pBlender->mSoftRatiosCosts[iSoftRatioIndex][0];
        iSoftRatioIndex++;
        }

      }

    }
  else
    {
		ZString s = OptimiseLastStatus();
		pBlender->m_StateLine[0] = s.c_str();
		s = OptimiseLastErr();
		pBlender->m_StateLine[1] = s.c_str();

    if (pBlender->bLogOptimiseErrors)
      {
      s = "Optimise Failed: ";
      s += OptimiseLastErr();
      //LogError(pBlender->Tag(), 0, "Optimise1stOrder Error!");
      LogError(pBlender->Tag(), 0, s.c_str() );
      }

    if (SetIt)
      {
      //Set results to something...???
      double MtotSum = 0.0;
      for (int t=0; t<m_iTankCnt; t++)
        {
        CMultiStore & St = lpMultiStore->m_Store[t];
        const double Mtot = St.Mass();// kgs
        MtotSum += Mtot;
        }

      for (int t=0; t<m_iTankCnt; t++)
        {
        if (MtotSum>0.0)
          {
          const double Mtot = lpMultiStore->m_Store[t].Mass();// kgs
          m_pResultSplits[t] = Mtot/MtotSum;
          }
        else
          m_pResultSplits[t] = 1.0/m_iTankCnt;
        }
      }
    }

  if (SetIt && pBlender->bOn)
    {
    int StreamIndex = lpMultiStore->GetOutputStreamIndex(pBlender->sPipeTag());

    if (StreamIndex>=0)
      {
      if (lpMultiStore->IsFracForStream(StreamIndex))
        {
        for (int t=0; t<m_iTankCnt; t++)
          lpMultiStore->SetStoreFracForStream(StreamIndex, t, m_pResultSplits[t]);
        if (pBlender->bLogSetOptimiseData)
          LogNote(pBlender->Tag(), 0, "Fractions for '%s' have been set in '%s'", pBlender->sPipeTag(), pBlender->sMSTag());
        }
      else
        {
        LogError(pBlender->Tag(), 0, "Fraction split method must be specified for product stream '%s'", pBlender->sPipeTag());
        }
      }
    else
      {
      LogError(pBlender->Tag(), 0, "Invalid Stream specified '%s'", pBlender->sPipeTag());
      }
    }


  // Above we Set Matrix Row and Column Names every time Optimise is Run
  // as the Matrix sizes may change

  pBlender->mX.SetRowPrefix("S");
  pBlender->mX.SetColName(0,"Blend");
  #endif
  }

//==========================================================================

void CBlendControlHelper::Optimise1(CBlendCon* pBlender, bool SetIt)
  {
  pBlender->m_StateLine[0] = "";
  pBlender->m_StateLine[1] = "";
  pBlender->m_StateLine[2] = "";
  CMultiStorage* lpMultiStore = (CMultiStorage*)(pBlender->m_pStorage);
  if (lpMultiStore == NULL)
    {
    pBlender->m_StateLine[0] = "Error: Multistore not connected";
    LogError(pBlender->Tag(), 0, "Optimise Failed: Multistore %s not connected!", pBlender->sMSTag());
    return;
    }

  double dBlendTonnes = m_dTotalRqd; // This is the blend amount required
  if (dBlendTonnes <= 0.0)
    {
    pBlender->m_StateLine[0] = "Error: Target Mass is zero";
    LogError(pBlender->Tag(), 0, "Optimise Failed: Target Mass is zero");
    return;
    }

  const long AssayCnt = gs_Assays.GetAssaySumCount();

  if (tankMin==NULL)
    tankMin = new TankMin;
  tankMin->SetSize(m_iTankCnt, AssayCnt);

  int EmptyTankCnt = 0;
  for (int t=0; t<m_iTankCnt; t++)
    {
    CMultiStore & St = lpMultiStore->m_Store[t];
    const double Mtot = St.Mass();// kgs

    tankMin->lowTank[t] = 0.0; // lowest preferred quantity from each tank
    tankMin->weightLowTank[t] = 0.0;
    tankMin->highTank[t] = 1.0; // higheset preferred quantity from each tank
    tankMin->weightHighTank[t] = 1.0;
    tankMin->maxTank[t] = Mtot/dBlendTonnes; // maximum possible quantity from each tank
    tankMin->weightMaxTank[t] = 10000000.0;
    if (Mtot<1.0e-9)
      {
      tankMin->emptyTank[t] = true;
      EmptyTankCnt++;
      }
    else
      tankMin->emptyTank[t] = false;
    }

  if (pBlender->bLogOptimiseNotes)
    {
    LogNote(pBlender->Tag(), 0, "Blend amount required:%.3f (%d of %d 'tanks' have material available)", dBlendTonnes, m_iTankCnt-EmptyTankCnt, m_iTankCnt);
    }

  // for each assay sum data specification
  for (int i=0; i<AssayCnt; i++)
    {
    CAssaySum* pAssaySumData = gs_Assays.m_Assays[i]->m_pBCrit;
    for (int t=0; t<m_iTankCnt; t++)
      {
      CMultiStore & St = lpMultiStore->m_Store[t];
      const double Mtot = St.Mass();// kgs
      if (Mtot<1.0e-9)
        {
        tankMin->tankAssay[i][t] = 0.0; // amount of each assay in each tank
        }
      else
        {
        double sum = 0.0;
        // sum all the assay data for the species specified in the AssaySumData
        for (int s=0; s<pAssaySumData->m_iList1Cnt; s++)
          {
          double M = St.VMass[pAssaySumData->m_pIndexList1[s]];
          sum += M;
          }
        tankMin->tankAssay[i][t] = sum/Mtot; // amount of each assay in each tank
        }
      }

    //for all potential criteria, set NO limits
    tankMin->lowAssay[i] = 0.0; // lowest preferred quantity of each compound
    tankMin->weightLowAssay[i] = 1.0;
    tankMin->highAssay[i] = 1.0; // highest preferred quantity of each compound
    tankMin->weightHighAssay[i] = 1.0;

    for (int j=0; j<AssayCnt; j++)
      {
      tankMin->ratioBoolAssay[i][j] = false;
      tankMin->lowRatioAssay[i][j] = 0.0;
      tankMin->weightLowRatioAssay[i][j] = 1.0;
      tankMin->highRatioAssay[i][j] = 1.0;
      tankMin->weightHighRatioAssay[i][j] = 1.0;
      }

    if (pBlender->bLogOptimiseNotes)
      {// Print out Assay Matrix
      char Buff[2048];
      char Buff1[64];
      sprintf(Buff, "Assay%2d:", i);
      for (int t=0; t<m_iTankCnt; t++)
        {
        sprintf(Buff1, "%9.4g, ", tankMin->tankAssay[i][t]);
        strcat(Buff, Buff1);
        }
      LogNote(pBlender->Tag(), 0, Buff);
      }
    }


  // Calculate the Criteria for display only
  pBlender->mStockpileAssays.SetDim(m_iCriteriaCnt, m_iTankCnt);
  pBlender->mStockpileAssays.SetColPrefix("S");
  for (int i=0; i<m_iCriteriaCnt; i++)
    {
	  CBlendCriteria* pBCrit = m_BCH[i]->m_pBCrit;
	  Strng s;
	  if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
		  s = pBCrit->m_sName + " < " + ftoa(pBCrit->m_dRequirement);
	  else
		  s = pBCrit->m_sName + " > " + ftoa(pBCrit->m_dRequirement);
	  pBlender->mStockpileAssays.SetRowName(i,s.Str());

    if ((pBCrit->m_eConstraintType==CBlendCriteria::eCT_Ratio))
	    {
		  // Get the index of the Assays for the Ratio
		  const int Index1 = pBCrit->m_AssayIndex1;
		  const int Index2 = pBCrit->m_AssayIndex2;
		  for (int t=0; t<m_iTankCnt; t++)
		    {
        const double Assay1 = tankMin->tankAssay[Index1][t];
			  const double Assay2 = tankMin->tankAssay[Index2][t];
        const double Ratio = (Assay2>0.0 ? Assay1/Assay2 : 0.0);
			  pBlender->mStockpileAssays[i][t] = Ratio;
  		  }
  	  }
	  else
	    {
		  const int Index1 = pBCrit->m_AssayIndex1;
		  for (int t=0; t<m_iTankCnt; t++)
		    {
			  const double Assay1 = tankMin->tankAssay[Index1][t];
			  pBlender->mStockpileAssays[i][t] = Assay1;
  		  }
	    }
    }

  //Based on criteria, set assay and ratio limits...
  for (int i=0; i<m_iCriteriaCnt; i++)
    {
	  CBlendCriteria* pBCrit = m_BCH[i]->m_pBCrit;
	  if ((pBCrit->m_eConstraintType==CBlendCriteria::eCT_Ratio))
	    {
		  const int Index1 = pBCrit->m_AssayIndex1;
		  const int Index2 = pBCrit->m_AssayIndex2;
      tankMin->ratioBoolAssay[Index1][Index2] = true;
  	  if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
        tankMin->highRatioAssay[Index1][Index2] = pBCrit->m_dRequirement;
      else
        tankMin->lowRatioAssay[Index1][Index2] = pBCrit->m_dRequirement;
      }
	  else
	    {
		  const int Index1 = pBCrit->m_AssayIndex1;
      if (0)
        {
  	    if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
          tankMin->highAssay[Index1] = pBCrit->m_dRequirement;
        else
          tankMin->lowAssay[Index1] = pBCrit->m_dRequirement;
        }
      else
        {
  	    if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
          {
          tankMin->lowAssay[Index1] = pBCrit->m_dRequirement*0.98;
          tankMin->highAssay[Index1] = pBCrit->m_dRequirement;
          tankMin->weightLowAssay[Index1] = 0.2;
          tankMin->weightHighAssay[Index1] = 2.0;
          }
        else
          {
          tankMin->lowAssay[Index1] = pBCrit->m_dRequirement;
          tankMin->highAssay[Index1] = pBCrit->m_dRequirement*1.02;
          tankMin->weightLowAssay[Index1] = 2.0;
          tankMin->weightHighAssay[Index1] = 0.2;
          }
        }
      }
    }

  //now solve...
  tankMin->InitSolution(); //TODO... rather solve from previous iteration?
  tankMin->Go();

  // Split Results
  for (int t=0; t<m_iTankCnt; t++)
    {
    m_pResultSplits[t] = tankMin->bestTank[t];
    }

  // Criteria Results
  for (int i=0; i<m_iCriteriaCnt; i++)
    {
    CBlendCriteria* pBCrit = m_BCH[i]->m_pBCrit;
    pBCrit->m_dResult = 0.0;
    pBCrit->m_dCost = 0.0;

    // Calculate Optimised Criteria Value
	  if ((pBCrit->m_eConstraintType==CBlendCriteria::eCT_Ratio))
      {
      const int AssayIndexNum = pBCrit->m_AssayIndex1;
      const int AssayIndexDen = pBCrit->m_AssayIndex2;
      double dnum = 0.0;
      double dden = 0.0;
      for (int t=0; t<m_iTankCnt; t++)
        {
        dnum += tankMin->bestTank[t]*tankMin->tankAssay[AssayIndexNum][t];
        dden += tankMin->bestTank[t]*tankMin->tankAssay[AssayIndexDen][t];
        }
      if (dden > 0.0)
        pBCrit->m_dResult = dnum/dden;
      }
    else if ((pBCrit->m_eConstraintType==CBlendCriteria::eCT_Soft) )
      {
      const int AssayIndex = pBCrit->m_AssayIndex1;
      for (int t=0; t<m_iTankCnt; t++)
        {
        const double d1 = tankMin->bestTank[t];
        const double d2 = tankMin->tankAssay[AssayIndex][t];
        pBCrit->m_dResult += d1*d2;
        }
      }
    if (pBlender->bLogOptimiseNotes)
      {// Print out criteria rsults
      LogNote(pBlender->Tag(), 0, "Criteria %d: %f %s %f (%s)", i, pBCrit->m_dResult, (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser ? "<" : ">"), pBCrit->m_dRequirement, pBCrit->m_sName.Str());
      }
    }

  if (SetIt && pBlender->bOn)
    {
    const int StreamIndex = lpMultiStore->GetOutputStreamIndex(pBlender->sPipeTag());

    if (StreamIndex>=0)
      {
      if (lpMultiStore->IsFracForStream(StreamIndex))
        {
        for (int t=0; t<m_iTankCnt; t++)
          lpMultiStore->SetStoreFracForStream(StreamIndex, t, m_pResultSplits[t]);
        if (pBlender->bLogSetOptimiseData)
          LogNote(pBlender->Tag(), 0, "Fractions for '%s' have been set in '%s'", pBlender->sPipeTag(), pBlender->sMSTag());
        }
      else
        {
        LogError(pBlender->Tag(), 0, "Fraction split method must be specified for product stream '%s'", pBlender->sPipeTag());
        }
      }
    else
      {
      LogError(pBlender->Tag(), 0, "Invalid Stream specified '%s'", pBlender->sPipeTag());
      }
    }
  }

//==========================================================================

void CBlendControlHelper::Optimise2(CBlendCon* pBlender, bool SetIt)
  {
  pBlender->m_StateLine[0] = "";
  pBlender->m_StateLine[1] = "";
  pBlender->m_StateLine[2] = "";
  CMultiStorage* lpMultiStore = (CMultiStorage*)(pBlender->m_pStorage);
  if (lpMultiStore == NULL)
    {
    pBlender->m_StateLine[0] = "Error: Multistore not connected";
    LogError(pBlender->Tag(), 0, "Optimise Failed: Multistore %s not connected!", pBlender->sMSTag());
    return;
    }

  if (m_iCriteriaCnt<1)
    {
    pBlender->m_StateLine[0] = "Error: Criteria not defined";
    LogError(pBlender->Tag(), 0, "Optimise Failed: Criteria not defined");
    return;
    }

  double dBlendRqd = m_dTotalRqd; // This is the blend amount required
  if (dBlendRqd <= 0.0)
    {
    pBlender->m_StateLine[0] = "Error: Target Mass is zero";
    LogWarning(pBlender->Tag(), 0, "Optimise Failed: Target Mass is zero");
    return;
    }

  const double EmptyAmnt = 1.0e-12;
  int EmptyTankCnt = 0;
  double TtlMassAvail = 0.0;
  for (int t=0; t<m_iTankCnt; t++)
    {
    CMultiStore & St = lpMultiStore->m_Store[t];
    const double Mtot = St.Mass();// kgs
    if (Mtot<EmptyAmnt)
      EmptyTankCnt++;
    else
      TtlMassAvail += Mtot;
    }
  if (TtlMassAvail<EmptyAmnt)
    {
    pBlender->m_StateLine[0] = "Error: Available mass is zero";
    LogWarning(pBlender->Tag(), 0, "Optimise Failed: Available mass is zero");
    return;
    }

  if (dBlendRqd>TtlMassAvail)
    {
    LogWarning(pBlender->Tag(), 0, "Required mass < mass available!  Optimisation will use mass available.");
    dBlendRqd = TtlMassAvail;
    }

  const long AssayCnt = gs_Assays.GetAssaySumCount();
  CArray <bool, bool> AssayUsed;
  CArray <int, int> AssayLookup;
  AssayUsed.SetSize(AssayCnt);
  AssayLookup.SetSize(AssayCnt);
  for (int i=0; i<AssayCnt; i++)
    {
    AssayUsed[i] = false;
    AssayLookup[i] = -1;
    }
  for (int i=0; i<m_iCriteriaCnt; i++)
    {
	  CBlendCriteria* pBCrit = m_BCH[i]->m_pBCrit;
    const int Index1 = pBCrit->m_AssayIndex1;
    AssayUsed[Index1] = true;
    if (pBCrit->m_eConstraintType==CBlendCriteria::eCT_Ratio)
	    {
		  const int Index2 = pBCrit->m_AssayIndex2;
      AssayUsed[Index2] = true;
      }
    }
  long ActAssayCnt = 0;
  for (int i=0; i<AssayCnt; i++)
    {
    if (AssayUsed[i])
      {
      AssayLookup[i] = ActAssayCnt;
      ActAssayCnt++;
      }
    }

  if (tankOpt==NULL)
    tankOpt = new TankBlendOptimiser;
  tankOpt->SetSize(m_iTankCnt, ActAssayCnt);
  //tankOpt->constraint = 1;

  for (int t=0; t<m_iTankCnt; t++)
    {
    CMultiStore & St = lpMultiStore->m_Store[t];
    const double Mtot = St.Mass();// kgs

    if (Mtot<EmptyAmnt)
      tankOpt->tankMax[t] = 0.0;
    else
      tankOpt->tankMax[t] = Mtot/dBlendRqd; // maximum possible quantity from each tank
    }

  if (pBlender->bLogOptimiseNotes)
    {
    LogNote(pBlender->Tag(), 0, "Blend amount required:%.3f (%d of %d 'tanks' have material available)", dBlendRqd, m_iTankCnt-EmptyTankCnt, m_iTankCnt);
    }

  // for each assay sum data specification
  for (int i=0; i<AssayCnt; i++)
    {
    if (AssayUsed[i])
      {
      const int k = AssayLookup[i];
      CAssaySum* pAssaySumData = gs_Assays.m_Assays[i]->m_pBCrit;
      for (int t=0; t<m_iTankCnt; t++)
        {
        CMultiStore & St = lpMultiStore->m_Store[t];
        const double Mtot = St.Mass();// kgs
        if (Mtot<1.0e-12)
          {
          tankOpt->assayConc[k][t] = 0.0; // amount of each assay in each tank
          }
        else
          {
          double sum = 0.0;
          // sum all the assay data for the species specified in the AssaySumData
          for (int s=0; s<pAssaySumData->m_iList1Cnt; s++)
            {
            double M = St.VMass[pAssaySumData->m_pIndexList1[s]];
            sum += M;
            }
          tankOpt->assayConc[k][t] = sum/Mtot; // amount of each assay in each tank
          }
        }

      //for all potential criteria, set NO limits
      tankOpt->assayLow[k] = 0.0; // lowest preferred quantity of each compound
      tankOpt->assayHigh[k] = 1.0; // highest preferred quantity of each compound
      tankOpt->assayLowPenalty[k] = 1.0;
      tankOpt->assayHighPenalty[k] = 1.0;

      for (int j=0; j<ActAssayCnt; j++)
        {
        tankOpt->assayRatioLowEnabled[k][j] = false;
        tankOpt->assayRatioHighEnabled[k][j] = false;
        tankOpt->assayRatioLow[k][j] = 0.0;
        tankOpt->assayRatioHigh[k][j] = 1.0;
        tankOpt->assayRatioLowPenalty[k][j] = 1.0;
        tankOpt->assayRatioHighPenalty[k][j] = 1.0;
        }

      if (pBlender->bLogOptimiseNotes)
        {// Print out Assay Matrix
        char Buff[2048];
        char Buff1[64];
        sprintf(Buff, "Assay%2d:", i);
        for (int t=0; t<m_iTankCnt; t++)
          {
          sprintf(Buff1, "%9.4g, ", tankOpt->assayConc[k][t]);
          strcat(Buff, Buff1);
          }
        LogNote(pBlender->Tag(), 0, Buff);
        }
      }
    }

  // Calculate the Criteria for display only
  pBlender->mStockpileAssays.SetDim(m_iCriteriaCnt, m_iTankCnt);
  pBlender->mStockpileAssays.SetColPrefix("S");
  for (int i=0; i<m_iCriteriaCnt; i++)
    {
	  CBlendCriteria* pBCrit = m_BCH[i]->m_pBCrit;
	  Strng s;
	  if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
		  s = pBCrit->m_sName + " < " + ftoa(pBCrit->m_dRequirement);
	  else
		  s = pBCrit->m_sName + " > " + ftoa(pBCrit->m_dRequirement);
	  pBlender->mStockpileAssays.SetRowName(i,s.Str());

    if ((pBCrit->m_eConstraintType==CBlendCriteria::eCT_Ratio))
	    {
		  // Get the index of the Assays for the Ratio
		  const int Index1 = AssayLookup[pBCrit->m_AssayIndex1];
		  const int Index2 = AssayLookup[pBCrit->m_AssayIndex2];
		  for (int t=0; t<m_iTankCnt; t++)
		    {
        const double Assay1 = tankOpt->assayConc[Index1][t];
			  const double Assay2 = tankOpt->assayConc[Index2][t];
        const double Ratio = (Assay2>0.0 ? Assay1/Assay2 : 0.0);
			  pBlender->mStockpileAssays[i][t] = Ratio;
  		  }
  	  }
	  else
	    {
		  const int Index1 = AssayLookup[pBCrit->m_AssayIndex1];
		  for (int t=0; t<m_iTankCnt; t++)
		    {
			  const double Assay1 = tankOpt->assayConc[Index1][t];
			  pBlender->mStockpileAssays[i][t] = Assay1;
  		  }
	    }
    }

  //Based on criteria, set assay and ratio limits...
  for (int i=0; i<m_iCriteriaCnt; i++)
    {
	  CBlendCriteria* pBCrit = m_BCH[i]->m_pBCrit;
	  if ((pBCrit->m_eConstraintType==CBlendCriteria::eCT_Ratio))
	    {
		  const int Index1 = AssayLookup[pBCrit->m_AssayIndex1];
		  const int Index2 = AssayLookup[pBCrit->m_AssayIndex2];
      tankOpt->assayRatioLow[Index1][Index2] = pBCrit->m_dRequirement;
      tankOpt->assayRatioHigh[Index1][Index2] = pBCrit->m_dRequirement;
      /*if (0)
        {
    	  if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
          tankOpt->assayRatioLowEnabled[Index1][Index2] = true;
        else
          tankOpt->assayRatioHighEnabled[Index1][Index2] = true;
        }
      else*/
        {
        tankOpt->assayRatioLowEnabled[Index1][Index2] = true;
        tankOpt->assayRatioHighEnabled[Index1][Index2] = true;
        if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
          tankOpt->assayRatioLowPenalty[Index1][Index2] = 1.0e-5;
        else
          tankOpt->assayRatioHighPenalty[Index1][Index2] = 1.0e-5;
        }
      }
	  else
	    {
		  const int Index1 = AssayLookup[pBCrit->m_AssayIndex1];
	    /*if (0)
        {
        if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
          tankOpt->assayHigh[Index1] = pBCrit->m_dRequirement;
        else
          tankOpt->assayLow[Index1] = pBCrit->m_dRequirement;
        }
      else if (0)
        {
  	    if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
          {
          tankOpt->assayLow[Index1] = pBCrit->m_dRequirement*0.98;
          tankOpt->assayHigh[Index1] = pBCrit->m_dRequirement;
          }
        else
          {
          tankOpt->assayLow[Index1] = pBCrit->m_dRequirement;
          tankOpt->assayHigh[Index1] = pBCrit->m_dRequirement*1.02;
          }
        }
      else*/
        {
        tankOpt->assayLow[Index1] = pBCrit->m_dRequirement;
        tankOpt->assayHigh[Index1] = pBCrit->m_dRequirement;
  	    if (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser)
          tankOpt->assayLowPenalty[Index1] = 1.0e-5;
        else
          tankOpt->assayHighPenalty[Index1] = 1.0e-5;
        }
      }
    }

  if (0)
    {
    dbgpln("-------------------- Blend Problem --------------------");
    dbgpln("Tanks:%d  Assays:%d", tankOpt->tanks, tankOpt->assays);
    dbgpln("Concentration Values");
    for (int i=0; i<tankOpt->assays; i++)
      {
      for (int j=0; j<tankOpt->tanks; j++)
        dbgp("%g; ", tankOpt->assayConc[i][j]);
      dbgpln("");
      }
    dbgpln("");

    dbgpln("Tank Values");
    dbgp("Tank Max:  ");
    for (int i=0; i<tankOpt->tanks; i++)    dbgp("%g; ", tankOpt->tankMax[i]);
    dbgpln("");
    dbgp("Tank High: ");
    for (int i=0; i<tankOpt->tanks; i++)    dbgp("%g; ", tankOpt->tankHigh[i]);
    dbgpln("");
    //dbgp("**** Tank: ");
    //for (int i=0; i<tankOpt->tanks; i++)    dbgp("%g; ", tankOpt->tank[i]);
    //dbgpln("");
    dbgp("Tank Low:  ");
    for (int i=0; i<tankOpt->tanks; i++)    dbgp("%g; ", tankOpt->tankLow[i]);
    dbgpln("");

    dbgpln("Assay Values");
    dbgp("Assay High: ");
    for (int i=0; i<tankOpt->assays; i++)    dbgp("%g; ", tankOpt->assayHigh[i]);
    dbgpln("");
    //dbgp("**** Assay: ");
    //for (int i=0; i<tankOpt->assays; i++)    dbgp("%g; ", tankOpt->assay[i]);
    //dbgpln("");
    dbgp("Assay Low:  ");
    for (int i=0; i<tankOpt->assays; i++)    dbgp("%g; ", tankOpt->assayLow[i]);
    dbgpln("");
    dbgpln("");

    dbgpln("Assay Ratio Low Values");
    dbgp("Assay Ratio Num/Den: ");
    for (int i=0; i<tankOpt->assays; i++)
      for (int j=0; j<tankOpt->assays; j++)
        if (tankOpt->assayRatioLowEnabled[i][j])
          dbgp("%d/%d; ", i, j);
    dbgpln("");
    /*printf("**** Assay Ratio: ");
    for (int i=0; i<assays; i++)
      for (int j=0; j<assays; j++)
        if (assayRatioLowEnabled[i][j])
        {
          double num = 0.0;
          double den = 0.0;
          for (int k=0; k<tanks; k++)
          {
            num += lpx_get_col_prim(lp, 1+k)*assayConc[i][k];
            den += lpx_get_col_prim(lp, 1+k)*assayConc[j][k];
          }
          printf("%g; ", num/den);
        }
    printf("\n");*/
    dbgp("Assay Ratio Low:  ");
    for (int i=0; i<tankOpt->assays; i++)
      for (int j=0; j<tankOpt->assays; j++)
        if (tankOpt->assayRatioLowEnabled[i][j])
          dbgp("%g; ", tankOpt->assayRatioLow[i][j]);
    dbgpln("");
    dbgpln("");

    dbgpln("Assay Ratio High Values");
    dbgp("Assay Ratio High Num/Den: ");
    for (int i=0; i<tankOpt->assays; i++)
      for (int j=0; j<tankOpt->assays; j++)
        if (tankOpt->assayRatioHighEnabled[i][j])
          dbgp("%d/%d; ", i, j);
    dbgpln("");
    dbgp("Assay Ratio High: ");
    for (int i=0; i<tankOpt->assays; i++)
      for (int j=0; j<tankOpt->assays; j++)
        if (tankOpt->assayRatioHighEnabled[i][j])
          dbgp("%g; ", tankOpt->assayRatioHigh[i][j]);
    dbgpln("");
    /*printf("**** Assay Ratio: ");
    for (int i=0; i<assays; i++)
      for (int j=0; j<assays; j++)
        if (assayRatioHighEnabled[i][j])
        {
          double num = 0.0;
          double den = 0.0;
          for (int k=0; k<tanks; k++)
          {
            num += lpx_get_col_prim(lp, 1+k)*assayConc[i][k];
            den += lpx_get_col_prim(lp, 1+k)*assayConc[j][k];
          }
          printf("%g; ", num/den);
        }
    dbgpln("");*/
    dbgpln("");

    /*printf("Penalty Values\n");
    printf("Penalty Tank Low:   ");
    for (int i=0; i<tanks; i++)    printf("%g; ", lpx_get_col_prim(lp, 1+i+tanks));
    printf("\n");
    printf("Penalty Tank High:  ");
    for (int i=0; i<tanks; i++)    printf("%g; ", lpx_get_col_prim(lp, 1+i+2*tanks));
    printf("\n");
    printf("Penalty Assay Low:  ");
    for (int i=0; i<assays; i++)    printf("%g; ", lpx_get_col_prim(lp, 1+i+3*tanks));
    printf("\n");
    printf("Penalty Assay High: ");
    for (int i=0; i<assays; i++)    printf("%g; ", lpx_get_col_prim(lp, 1+i+3*tanks+assays));
    printf("\n");
    printf("Penalty Assay Ratio Low:  ");
    for (int i=0; i<assays; i++)
      for (int j=0; j<assays; j++)
        if (assayRatioLowEnabled[i][j])
          printf("%g; ", lpx_get_col_prim(lp, 1+i*assays+j+3*tanks+2*assays));
    printf("\n");
    printf("Penalty Assay Ratio High: ");
    for (int i=0; i<assays; i++)
      for (int j=0; j<assays; j++)
        if (assayRatioHighEnabled[i][j])
          printf("%g; ", lpx_get_col_prim(lp, 1+i*assays+j+3*tanks+2*assays+assays*assays));
    printf("\n\n");*/

    /*printf("Weight Values\n");
    printf("Weight for tank[i]: ");
    for (int i=0; i<tanks; i++)
      printf("%g; ", lpx_get_obj_coef(lp,  1+i));
    printf(" (unused, should be zero)\n");

    printf("Weight Tank Low: ");
    for (int i=0; i<tanks; i++)
      printf("%g; ", lpx_get_obj_coef(lp,  1+i+tanks));
    printf("\n");

    printf("Weight Tank High: ");
    for (int i=0; i<tanks; i++)
      printf("%g; ", lpx_get_obj_coef(lp,  1+i+2*tanks));
    printf("\n");

    printf("Weight Assay Low: ");
    for (int i=0; i<assays; i++)
      printf("%g; ", lpx_get_obj_coef(lp,  1+i+3*tanks));
    printf("\n");

    printf("Weight Assay High: ");
    for (int i=0; i<assays; i++)
      printf("%g; ", lpx_get_obj_coef(lp,  1+i+3*tanks+assays));
    printf("\n");

    printf("Weight Assay Ratio Low:\n");
    for (int i=0; i<assays; i++)
    {
      printf("                        ");
      for (int j=0; j<assays; j++)
        printf("%g; ", lpx_get_obj_coef(lp, 1+i*assays+j+3*tanks+2*assays));
      printf("\n");
    }

    printf("Weight Assay Ratio High:\n");
    for (int i=0; i<assays; i++)
    {
      printf("                         ");
      for (int j=0; j<assays; j++)
        printf("%g; ", lpx_get_obj_coef(lp, 1+i*assays+j+3*tanks+2*assays+assays*assays));
      printf("\n");
    }
    printf("\n");*/

    //printf("Total Weighted Penalty: %g\n", lpx_get_obj_val(lp));
    dbgpln("================");
    }

  //now solve...
  tankOpt->InitSolution(); //TODO... rather solve from previous iteration?
  int ExitCode = tankOpt->go();
  if (ExitCode==LPX_E_NOPFS)
    {
    LogWarning(pBlender->Tag(), 0, "Required amount too large relative to sum of contents of tanks!");
    }
  else if (ExitCode!=LPX_E_OK)
    {
    pBlender->m_StateLine[0] = "Error: Optimisation failed!";
    LogError(pBlender->Tag(), 0, "Critical blend optimisation error! Error number:%d", ExitCode);
    }

  // Split Results
  for (int t=0; t<m_iTankCnt; t++)
    {
    m_pResultSplits[t] = tankOpt->tank[t];
  }

  // Criteria Results
  for (int i=0; i<m_iCriteriaCnt; i++)
    {
    CBlendCriteria* pBCrit = m_BCH[i]->m_pBCrit;
    pBCrit->m_dResult = 0.0;
    pBCrit->m_dCost = 0.0;

    // Calculate Optimised Criteria Value
	  if ((pBCrit->m_eConstraintType==CBlendCriteria::eCT_Ratio))
      {
      const int AssayIndexNum = AssayLookup[pBCrit->m_AssayIndex1];
      const int AssayIndexDen = AssayLookup[pBCrit->m_AssayIndex2];
      double dnum = 0.0;
      double dden = 0.0;
      for (int t=0; t<m_iTankCnt; t++)
        {
        dnum += tankOpt->tank[t]*tankOpt->assayConc[AssayIndexNum][t];
        dden += tankOpt->tank[t]*tankOpt->assayConc[AssayIndexDen][t];
        }
      if (fabs(dden) > 0.0)
        pBCrit->m_dResult = dnum/dden;
      }
    else if ((pBCrit->m_eConstraintType==CBlendCriteria::eCT_Soft) )
      {
      const int AssayIndex = AssayLookup[pBCrit->m_AssayIndex1];
      for (int t=0; t<m_iTankCnt; t++)
        {
        const double d1 = tankOpt->tank[t];
        const double d2 = tankOpt->assayConc[AssayIndex][t];
        pBCrit->m_dResult += d1*d2;
        }
      }
    if (pBlender->bLogOptimiseNotes)
      {// Print out criteria results
      bool ReqMet = (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser ? (pBCrit->m_dResult<=pBCrit->m_dRequirement) : (pBCrit->m_dResult>=pBCrit->m_dRequirement));
      LogNote(pBlender->Tag(), 0, "Criteria %d:(%s) %f %s %f (%s)", i, ReqMet ? "Yes" : "No", pBCrit->m_dResult, (pBCrit->m_eCompType==CBlendCriteria::eCT_Lesser ? "<=" : ">="), pBCrit->m_dRequirement, pBCrit->m_sName.Str());
      }
    }

  if (SetIt && pBlender->bOn)
    {
    const int StreamIndex = lpMultiStore->GetOutputStreamIndex(pBlender->sPipeTag());

    if (StreamIndex>=0)
      {
      if (lpMultiStore->IsFracForStream(StreamIndex))
        {
        for (int t=0; t<m_iTankCnt; t++)
          lpMultiStore->SetStoreFracForStream(StreamIndex, t, m_pResultSplits[t]);
        if (pBlender->bLogSetOptimiseData)
          LogNote(pBlender->Tag(), 0, "Fractions for '%s' have been set in '%s'", pBlender->sPipeTag(), pBlender->sMSTag());
        }
      else
        {
        LogError(pBlender->Tag(), 0, "Fraction split method must be specified for product stream '%s'", pBlender->sPipeTag());
        }
      }
    else
      {
      LogError(pBlender->Tag(), 0, "Invalid Stream specified '%s'", pBlender->sPipeTag());
      }
    }
  }

//==========================================================================
#endif