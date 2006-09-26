#include "stdafx.h"

#define __MAKEUPBLOCK_CPP
#include "MakeupBlock.h"
//#include "optoff.h"

//=========================================================================
//
//
//
//=========================================================================

XID xidMakeupEnable = AdjustXID(1);
XID xidMakeupMdlNm  = AdjustXID(2);

//============================================================================
//
//
//
//============================================================================

const pchar CMakeupBlock::GroupName="MakeupBlock";

IMPLEMENT_MAKEUPBLOCK(CMakeupBlock, "MB_None", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "None",  " ");

CMakeupBlock::CMakeupBlock(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
  m_pMakeupBase = NULL;
  }

//--------------------------------------------------------------------------

CMakeupBlock::~CMakeupBlock()
  {
  };

//--------------------------------------------------------------------------

void CMakeupBlock::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, NULL, NULL, DDB_NoPage))
    {
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

void CMakeupBlock::EvalProducts(SpConduit &Qf, double Po, double FinalTEst)
  {
  };

void CMakeupBlock::EvalProductsPipe(SpConduit & Qf, double Len, double Diam, double Po, double FinalTEst)
  {
  };

//============================================================================
//
//
//
//============================================================================

CMakeupBase::CMakeupBase(TaggedObject * pAttach, int Index, LPTSTR Name) : CBlockEvalBase(BEId_Makeup, Index, Name),
m_SrcIO(eDIO_Makeup, dynamic_cast<FlwNode*>(pAttach), false, false, 
        Name, IOId_Makeup2Area+Index, IOId_AreaMakeupO, "MakeupSrc", "MakeupSrc_1")
  { 
  m_pMakeupB=NULL; 
  m_pNd=pAttach; 
  m_fEnabled=false;
  m_fFixed=false; 
  m_SrcIO.UsrEnable=true;//false;
  //m_Index=Index; 
  }

CMakeupBase::~CMakeupBase()  
  { 
  delete m_pMakeupB; 
  };

//Strng CMakeupBase::Name()
//  {
//  Strng S;
//  S.Set("M%i", m_Index+1);
//  return S;
//  }

flag CMakeupBase::Open(TagObjClass * pMakeupClass, flag Fixed)
  {
  m_fEnabled=True;

  if (m_pMakeupB && (pMakeupClass==NULL || m_pMakeupB->Class()==pMakeupClass))
    return True;

  delete m_pMakeupB;
  m_fFixed=Fixed;

  if (pMakeupClass)
    m_pMakeupB=(CMakeupBlock*)pMakeupClass->Construct(NULL, "Makeup", m_pNd, TOA_Embedded);//pNd);
  else
    m_pMakeupB=(CMakeupBlock*)CMakeupBlockClass.Construct(NULL, "Makeup", m_pNd, TOA_Embedded);//pNd);
  m_pNd->StructureChanged(NULL);
  m_pMakeupB->m_pMakeupBase=this;

  m_fEnabled=m_pMakeupB!=NULL;
  return m_fEnabled;
  };

//--------------------------------------------------------------------------

void CMakeupBase::Close()
  {
  if (m_fFixed)
    return;
  m_fEnabled=False;
  }

//--------------------------------------------------------------------------

void CMakeupBase::Add_OnOff(DataDefnBlk &DDB, dword Flags, int UserInfo)
  {
  Strng S;
  S.Set("Makeup%i", m_Index+1);
  DDB.PushUserInfo(UserInfo);
  DDB.Byte(S(), "",  DC_,    "",     xidMakeupEnable,  m_pNd, Flags, GetOnOffValLst());
  DDB.PopUserInfo();
  }

//--------------------------------------------------------------------------

void CMakeupBase::BuildDataDefn(DataDefnBlk &DDB, char* pTag, char* pTagComment, DDBPages PageIs, dword UserInfo)
  {
  DDB.PushUserInfo(UserInfo);
  if (pTag==NULL)
    pTag="Xx";
  DDEF_Flags Old=DDB.GetVisibility();
  DDB.Visibility(NSHM_All, m_fEnabled);
  if (Enabled())//pHL)
    {
    DDB.Text("");
    if (DDB.BeginObject(m_pNd, Name(), "EB_Makeup", pTagComment, PageIs))
      {
      DDBValueLstMem DDB0;
      TagObjClass::GetSDescValueLst(CMakeupBlock::GroupName, DDB0);
      DDB.String  ("Model",      "",       DC_    , "",      xidMakeupMdlNm  , m_pNd,m_fFixed ? 0 : isParmStopped|SetOnChange, DDB0());

      if (m_SrcIO.Enabled)
        {
        //m_SrcIO.BuildDataDefn(DDB, NULL, DDB_NoPage, UserInfo+102, 0);//DFIO_ShowQm);
        m_SrcIO.BuildDataDefn(DDB, "DIO", DDB_NoPage, UserInfo+102, 0);//DFIO_ShowQm);
        }

      if (m_pMakeupB)
        {
        //DDB.SuppressNextBlock();
        m_pMakeupB->BuildDataDefn(DDB);
        }
      }
    DDB.EndObject();
    }
  DDB.SetVisibility(Old);
  DDB.PopUserInfo();
  }

//--------------------------------------------------------------------------

flag CMakeupBase::DataXchg(DataChangeBlk & DCB)
  {
  if (DCB.dwUserInfo%1000==102)
    {
    if (m_SrcIO.DataXchg(DCB))
      return 1;
    }

  switch (DCB.lHandle)
    {
    case xidMakeupEnable:
      if (DCB.rB)
        {
        if (*DCB.rB)
          {
          Open(*DCB.rB);
          m_pMakeupB->Tag(Name());
          }
        else
          Close();
        }
      DCB.B=OpenStatus();// (Enabled());
      return 1;
    case xidMakeupMdlNm:
      if (DCB.rpC && !m_fFixed)
        {
        int WasEnabled=m_fEnabled;
        TagObjClass * pC=CMakeupBlockClass.FindGrpShortDesc(DCB.rpC);
        if (pC)
          {
          Open(pC);
          m_pMakeupB->Tag(Name());
          }
        else
          Close();
        m_SrcIO.UsrEnable = m_pMakeupB ? m_pMakeupB->DoesSomething() && Enabled() : false;
        }
      DCB.pC = m_pMakeupB ? m_pMakeupB->ShortDesc() : "";
      return 1;
    }
  return 0;
  }

//--------------------------------------------------------------------------

int CMakeupBase::ChangeTag(char * pOldTag, char * pNewTag)
  {
  if (stricmp(m_SrcIO.Target(), pOldTag)==0)
    m_SrcIO.SetTarget(pNewTag);
  return EOCT_DONE;
  };

//--------------------------------------------------------------------------

int CMakeupBase::DeleteTag(char * pDelTag)
  {
  return EOCT_DONE;
  };

//============================================================================
//
//
//
//============================================================================

// Common
XID xidMkType    = AdjustXID(1000);
XID xidMkSelect  = AdjustXID(1001);
XID xidMkAll     = AdjustXID(1002);
XID xidMkSolids  = AdjustXID(1003);
XID xidMkLiquids = AdjustXID(1004);
XID xidMkGasses  = AdjustXID(1006);
XID xidMkSpcCnt  = AdjustXID(1007);
XID xidMkAddSpc  = AdjustXID(1008);
XID xidMkRemSpc  = AdjustXID(1009);
XID xidMkCmpCnt  = AdjustXID(1010);
XID xidMkAddCmp  = AdjustXID(1011);
XID xidMkRemCmp  = AdjustXID(1012);
XID xidMkError   = AdjustXID(1013);
XID xidMkPhase   = AdjustXID(1200);


//============================================================================
//
//
//
//============================================================================

#if 01 /*NOTDEFINED*/

//============================================================================
//
//
//
//============================================================================

#define DllImportExport /* */

class DllImportExport CMeasInfo
  {
  friend class CXBlk_MUFeed;
  public:
    enum eSelect
      {
      Slct_All,
      Slct_Occur,
      Slct_IndPhase,
      Slct_Specie,
      Slct_Component,
      };

    CMeasInfo()
      {
      m_eSelect   = Slct_All;
      m_Phases    = som_ALL;
      m_DDBSpcsOK = false;
      m_DDBCmpsOK = false;
      };

    void            SetUpDDBSpcs();
    void            SetUpDDBCmps();

    void            BuildDataDefn(DataDefnBlk& DDB, CXBlk_MUFeed &Blk, LPTSTR Tag, DWORD UserInfo);
    flag            DataXchg(DataChangeBlk & DCB, CXBlk_MUFeed &Blk);
    flag            ValidateData(ValidateDataBlk & VDB, CXBlk_MUFeed &Blk);

  protected:


    eSelect         m_eSelect;
    PhMask          m_Phases; 
    CIArray         m_Species;
    CIArray         m_Comps;
    bool            m_DDBSpcsOK;
    DDBValueLstMem  m_DDBSpcAdd;
    DDBValueLstMem  m_DDBSpcRem;
    int             m_nLastSpcStr;
    CStringArray    m_SpcStr;
    bool            m_DDBCmpsOK;
    DDBValueLstMem  m_DDBCmpAdd;
    DDBValueLstMem  m_DDBCmpRem;
    int             m_nLastCmpStr;
    Strng           m_MeasDesc;
    CStringArray    m_CmpStr;
  };

//============================================================================
//
//
//
//============================================================================

class DllImportExport CXBlk_MUFeed: public CMakeupBlock
  {
  public:
    CXBlk_MUFeed(bool AsRatio, TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CXBlk_MUFeed();

    void           ClrMeasEtc();

    virtual flag   DoesSomething() { return true; };

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void   EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

    double         GetSetPoint();
    double         GetMeasVal(SpConduit &QIn, SpConduit &QSrc, SpConduit &QPrd);
    double         GetFlowValue(CMeasInfo &MI, SpConduit &QPrd, PhMask PhRqd=0);

    DEFINE_CI(CXBlk_MUFeed, CMakeupBlock, 12);

  public:
    bool           m_AsRatio;

    enum eSource
      {
      Src_Self,
      Src_Remote,
      };

    enum eType
      { 
      //Type_None, 
      Type_Mass,
      Type_Volume,
      Type_NVolume,
      Type_Mole,
      };

    enum eLoFeed
      {
      LF_Ignore,
      LF_StopMakeUp,
      //LF_HoldMeasure,
      };

    Strng           m_MyTag;
    eSource         m_eSource;
    double          m_MUQmMin;
    double          m_MUQmMax;
    eType           m_eType;

    eLoFeed         m_eLoFeedOpt;
    double          m_LoFeedQm;

    CMeasInfo       m_Mu, m_In;

    bool            m_bHasFlow;

    double          m_QmRqd;
    double          m_QmRatio;
    double          m_QMlRqd;
    double          m_QMlRatio;
    double          m_QvRqd;
    double          m_QvRatio;
    double          m_NQvRqd;
    double          m_NQvRatio;

    //results
    double          m_dSetPoint;
    double          m_dMeas;
    double          m_dResult;
    double          m_dFeedAct;
    double          m_dProdAct;
    double          m_dMakeupAct;

    double          m_dQmMakeup;
    double          m_dQmFeed;
    double          m_dQmProd;
    double          m_dTempKFeed;
    double          m_dTempKProd;

    Strng_List      m_ErrorLst;

    static CToleranceBlock sm_QmTol;

  };

//============================================================================
//
//
//
//============================================================================

//--------------------------------------------------------------------------

void CMeasInfo::SetUpDDBSpcs()
  {

  if (!m_DDBSpcsOK || m_DDBSpcAdd.Length()+m_DDBSpcRem.Length()<SDB.Count())
    {
    m_DDBSpcAdd.Empty();
    m_DDBSpcRem.Empty();
    m_DDBSpcAdd.Add(-2, "All_Available");
    m_DDBSpcAdd.Add(-1, " - ");
    m_DDBSpcRem.Add(-2, "All_Available");
    m_DDBSpcRem.Add(-1, " - ");

    bool InList[MaxSpeciesEx];
    for (int s=0; s<SDB.Count(); s++)
      InList[s]=false;

    bool NeedsComma=false;
    m_nLastSpcStr=0;
    m_SpcStr.SetSize(Max(m_nLastSpcStr+1, m_SpcStr.GetSize()));
    m_SpcStr[m_nLastSpcStr]="Species:";
    for (int j=0; j<m_Species.GetSize(); j++)
      {
      int s=m_Species[j];
      InList[s]=true;
      m_DDBSpcRem.Add(s, SDB[s].SymOrTag());

      if (NeedsComma)
        m_SpcStr[m_nLastSpcStr]+=",";
      m_SpcStr[m_nLastSpcStr]+=SDB[s].SymOrTag();
      if (m_SpcStr[m_nLastSpcStr].GetLength()>35)
        {
        m_nLastSpcStr++;
        m_SpcStr.SetSize(Max(m_nLastSpcStr+1, m_SpcStr.GetSize()));
        m_SpcStr[m_nLastSpcStr]="        ";
        NeedsComma=false;
        }
      else
        NeedsComma=true;
      }

    for (int s=0; s<SDB.Count(); s++)
      {
      if (!InList[s])
        m_DDBSpcAdd.Add(s,SDB[s].SymOrTag());
      }
    }

  m_DDBSpcsOK = true;
  }

//--------------------------------------------------------------------------

void CMeasInfo::SetUpDDBCmps()
  {
  if (!m_DDBCmpsOK || m_DDBCmpAdd.Length()+m_DDBCmpRem.Length()<CDB.No())
    {
    m_DDBCmpAdd.Empty();
    m_DDBCmpRem.Empty();
    m_DDBCmpAdd.Add(-2, "All_Available");
    m_DDBCmpAdd.Add(-1, " - ");
    m_DDBCmpRem.Add(-2, "All_Available");
    m_DDBCmpRem.Add(-1, " - ");

    bool InList[MaxSpeciesEx];
    for (int s=0; s<CDB.No(); s++)
      InList[s]=false;

    bool NeedsComma=false;
    m_nLastCmpStr=0;
    m_CmpStr.SetSize(Max(m_nLastCmpStr+1, m_CmpStr.GetSize()));
    m_CmpStr[m_nLastCmpStr]="Comps:";
    for (int j=0; j<m_Comps.GetSize(); j++)
      {
      int s=m_Comps[j];
      InList[s]=true;
      m_DDBCmpRem.Add(s, CDB[s].SymOrTag());

      if (NeedsComma)
        m_CmpStr[m_nLastCmpStr]+=",";
      m_CmpStr[m_nLastCmpStr]+=CDB[s].SymOrTag();
      if (m_CmpStr[m_nLastCmpStr].GetLength()>35)
        {
        m_nLastCmpStr++;
        m_CmpStr.SetSize(Max(m_nLastCmpStr+1, m_CmpStr.GetSize()));
        m_CmpStr[m_nLastCmpStr]="        ";
        NeedsComma=false;
        }
      else
        NeedsComma=true;
      //for (int ss=CDB[s].
      }

    for (int s=0; s<CDB.No(); s++)
      {
      if (!InList[s])
        m_DDBCmpAdd.Add(s,CDB[s].SymOrTag());
      }
    }

  m_DDBCmpsOK = true;
  }

//--------------------------------------------------------------------------

void CMeasInfo::BuildDataDefn(DataDefnBlk& DDB, CXBlk_MUFeed &Blk, LPTSTR Tag, DWORD UserInfo)
  {
  DDB.PushUserInfo(UserInfo);
  if (DDB.BeginStruct(&Blk, Tag, NULL, DDB_NoPage))
    {
    static DDBValueLst DDBSelect[] =
      {                         
        {CMeasInfo::Slct_All,            "All"             },
        {CMeasInfo::Slct_Occur,          "Phase"           },
        {CMeasInfo::Slct_IndPhase,       "IndividualPhase" },
        {CMeasInfo::Slct_Specie,         "Specie"          },
        {CMeasInfo::Slct_Component,      "Component"       },
        {}
      };

    DDB.Visibility();
    DDB.Long       ("", "Selection",        DC_,  "", xidMkSelect,  &Blk, isParm|SetOnChange, DDBSelect);
    //DDB.Text("Selection");
    if (DDB.ForFileSnpScn())
      {
      switch (m_eSelect)
        {
        case Slct_All:
        case Slct_Occur:
        case Slct_IndPhase:
          DDB.Long("", "PhaseMask",               DC_,  "", (long*)&m_Phases,     &Blk, isParm|InitHidden);
          break;
        case Slct_Specie:
          {
          SetUpDDBSpcs();

          DDB.Long  ("NSpcs",       "", DC_,      "",   xidMkSpcCnt, &Blk, isParm);
          if (DDB.BeginArray(&Blk, "Spcs", "MU_SpIndex", m_Species.GetSize(), 0, DDB_NoPage, isParm))
            {
            for (int s=0; s<m_Species.GetSize(); s++)
              {
              DDB.BeginElement(&Blk, s);
              DDB.Int("Spc", "", DC_, "", &m_Species[s], &Blk, isParmConstruct, &SDB.DDBSpList);
              }
            };
          DDB.EndArray();
          break;
          }
        case Slct_Component:
          {
          SetUpDDBCmps();

          DDB.Long  ("NCmps",       "", DC_,      "",   xidMkCmpCnt, &Blk, isParm);
          if (DDB.BeginArray(&Blk, "Cmps", "MU_CmpIndex", m_Comps.GetSize(), 0, DDB_NoPage, isParm))
            {
            for (int s=0; s<m_Comps.GetSize(); s++)
              {
              DDB.BeginElement(&Blk, s);
              DDB.Int("Cmp", "", DC_, "", &m_Comps[s], &Blk, isParmConstruct, &CDB.DDBCompList);
              }
            };
          DDB.EndArray();
          break;
          }
        }  
      }
    else
      {
      switch (m_eSelect)
        {
        case Slct_All:
          break;
        case Slct_Occur:
          {
          DDB.CheckBoxBtn("", "Solids",           DC_,  "", xidMkSolids,  &Blk, isParm);
          DDB.CheckBoxBtn("", "Liquids",          DC_,  "", xidMkLiquids, &Blk, isParm);
          DDB.CheckBoxBtn("", "Gasses",           DC_,  "", xidMkGasses,  &Blk, isParm);
          break;
          }
        case Slct_IndPhase:
          {
          for (int o=CDB.PhaseFirst(BOT_Solid); o<=CDB.PhaseLast(BOT_Gas); o++)
            {
            CPhaseInfo & P=CDB.PhaseInfo(o);
            Strng T,S;
            T.Set("(%s)", P.m_Tag());
            S.Set("(%s)", P.m_Sym());
            DDB.CheckBoxBtn(T(), S(), DC_,  "", xidMkPhase+o,  &Blk, isParm);
            }
          break;
          }
        case Slct_Specie:
          {
          SetUpDDBSpcs();
          for (int s=0; s<=m_nLastSpcStr; s++)
            DDB.Text((LPTSTR)(LPCTSTR)m_SpcStr[s]);
          DDB.Long  ("Add",    "", DC_, "", xidMkAddSpc, &Blk, (m_DDBSpcAdd.Length()>1?isParm:0)|SetOnChange, &m_DDBSpcAdd);
          DDB.Long  ("Remove", "", DC_, "", xidMkRemSpc, &Blk, (m_DDBSpcRem.Length()>1?isParm:0)|SetOnChange, &m_DDBSpcRem);
          break;
          }
        case Slct_Component:
          {
          SetUpDDBCmps();
          for (int s=0; s<=m_nLastCmpStr; s++)
            DDB.Text((LPTSTR)(LPCTSTR)m_CmpStr[s]);
          DDB.Long  ("Add",    "", DC_, "", xidMkAddCmp, &Blk, (m_DDBCmpAdd.Length()>1?isParm:0)|SetOnChange, &m_DDBCmpAdd);
          DDB.Long  ("Remove", "", DC_, "", xidMkRemCmp, &Blk, (m_DDBCmpRem.Length()>1?isParm:0)|SetOnChange, &m_DDBCmpRem);
          break;
          }
        }  
      }

    if (Blk.m_eType==CXBlk_MUFeed::Type_Mass)
      m_MeasDesc = "Mass flow";
    else if (Blk.m_eType==CXBlk_MUFeed::Type_Volume)
      m_MeasDesc = "Volume flow";
    else if (Blk.m_eType==CXBlk_MUFeed::Type_NVolume)
      m_MeasDesc = "NormVolume flow";
    else if (Blk.m_eType==CXBlk_MUFeed::Type_Mole)
      m_MeasDesc = "Molar flow";
    else
      m_MeasDesc = "";

    switch (m_eSelect)
      {
      case Slct_All:
        m_MeasDesc += " of total stream:"; 
        break;
      case Slct_Occur:
        {
        m_MeasDesc += " of sum of phases (";
        if (m_Phases & som_Sol) m_MeasDesc += "solids ";
        if (m_Phases & som_Liq) m_MeasDesc += "liquids ";
        if (m_Phases & som_Gas) m_MeasDesc += "vapours ";
        m_MeasDesc.RTrim();
        m_MeasDesc += "):";
        break;
        }
      case Slct_IndPhase:
        {
        m_MeasDesc += " of sum of individual phases (";
        for (int o=CDB.PhaseFirst(BOT_Solid); o<=CDB.PhaseLast(BOT_Gas); o++)
          {
          CPhaseInfo & P=CDB.PhaseInfo(o);
          if (m_Phases&P.m_PhMsk)
            {
            m_MeasDesc += P.m_Sym();
            m_MeasDesc += ' ';
            }
          }
        m_MeasDesc.RTrim();
        m_MeasDesc += "):";
        break;
        }
      case Slct_Specie:
        {
        m_MeasDesc += " of sum of selected species:"; 
        break;
        }
      case Slct_Component:
        {
        m_MeasDesc += " of sum of selected components:"; 
        break;
        }
      }
    }
  DDB.EndStruct();
  DDB.PopUserInfo();
};

//--------------------------------------------------------------------------

flag CMeasInfo::DataXchg(DataChangeBlk & DCB, CXBlk_MUFeed &Blk)
  {
  switch (DCB.lHandle)
    {
    case xidMkSelect:
      if (DCB.rL && (m_eSelect!=(eSelect)*DCB.rL))
        {          
        m_eSelect   = (eSelect)*DCB.rL;
        Blk.ClrMeasEtc();
        }
      DCB.L = m_eSelect; 
      return 1;
    case xidMkAll:
      if (DCB.rB)
        {
        m_Phases = *DCB.rB ? som_ALL:0;
        Blk.ClrMeasEtc();
        }
      DCB.B = (m_Phases==som_ALL) ? 1 : (m_Phases==0) ? 0 : 2; 
      return 1;
    case xidMkSolids:
      if (DCB.rB)
        {
        m_Phases = (m_Phases&~som_Sol) | (*DCB.rB ? som_Sol:0);
        Blk.ClrMeasEtc();
        }
      DCB.B = ((m_Phases&som_Sol)==som_Sol)? 1 : ((m_Phases&som_Sol)==0) ? 0 : 2; 
      return 1;
    case xidMkLiquids:
      if (DCB.rB)
        {
        m_Phases = (m_Phases&~som_Liq) | (*DCB.rB ? som_Liq:0);
        Blk.ClrMeasEtc();
        }
      DCB.B = ((m_Phases&som_Liq)==som_Liq)? 1 : ((m_Phases&som_Liq)==0) ? 0 : 2; 
      return 1;
    case xidMkGasses:
      if (DCB.rB)
        {
        m_Phases = (m_Phases&~som_Gas) | (*DCB.rB ? som_Gas:0);
        Blk.ClrMeasEtc();
        }
      DCB.B = ((m_Phases&som_Gas)==som_Gas)? 1 : ((m_Phases&som_Gas)==0) ? 0 : 2; 
      return 1;

    case xidMkSpcCnt:
      if (DCB.rL)
        {
        m_Species.SetSize(*DCB.rL);
        m_DDBSpcsOK = false;
        Blk.ClrMeasEtc();
        }
      DCB.L=m_Species.GetSize();
      return 1;
    case xidMkAddSpc:
      if (DCB.rL)
        {
        if (*DCB.rL>=0)
          {
          for (int j=0; j<m_Species.GetSize(); j++)
            {
            if (m_Species[j]>*DCB.rL)
              {
              m_Species.InsertAt(j, *DCB.rL);
              m_DDBSpcsOK=false;
              DCB.L=-1;
              Blk.ClrMeasEtc();
              return 1;
              }
            }
          m_Species.Add(*DCB.rL);
          m_DDBSpcsOK=false;
          Blk.ClrMeasEtc();
          }
        else if (*DCB.rL==-2)
          {
          m_Species.SetSize(0);
          for (int i=0; i<SDB.Count(); i++)
            m_Species.Add(i);
          m_DDBSpcsOK=false;
          Blk.ClrMeasEtc();
          }
        }
      DCB.L=-1;
      return 1;
    case xidMkRemSpc:
      if (DCB.rL)
        {
        if (*DCB.rL>=0)
          {
          for (int j=0; j<m_Species.GetSize(); j++)
            {
            if (m_Species[j]==*DCB.rL)
              {
              m_Species.RemoveAt(j);
              m_DDBSpcsOK=false;
              Blk.ClrMeasEtc();
              break;
              }
            }
          }
        else if (*DCB.rL==-2)
          {
          m_Species.SetSize(0);
          m_DDBSpcsOK=false;
          Blk.ClrMeasEtc();
          }
        }
      DCB.L=-1;
      return 1;

    case xidMkCmpCnt:
      if (DCB.rL)
        {
        m_Comps.SetSize(*DCB.rL);
        m_DDBCmpsOK = false;
        Blk.ClrMeasEtc();
        }
      DCB.L=m_Comps.GetSize();
      return 1;
    case xidMkAddCmp:
      if (DCB.rL)
        {
        if (*DCB.rL>=0)
          {
          for (int j=0; j<m_Comps.GetSize(); j++)
            {
            if (m_Comps[j]>*DCB.rL)
              {
              m_Comps.InsertAt(j, *DCB.rL);
              m_DDBCmpsOK=false;
              DCB.L=-1;
              Blk.ClrMeasEtc();
              return 1;
              }
            }
          m_Comps.Add(*DCB.rL);
          m_DDBCmpsOK=false;
          Blk.ClrMeasEtc();
          }
        else if (*DCB.rL==-2)
          {
          m_Comps.SetSize(0);
          for (int i=0; i<CDB.No(); i++)
            m_Comps.Add(i);
          m_DDBCmpsOK=false;
          Blk.ClrMeasEtc();
          }
        }
      DCB.L=-1;
      return 1;
    case xidMkRemCmp:
      if (DCB.rL)
        {
        if (*DCB.rL>=0)
          {
          for (int j=0; j<m_Comps.GetSize(); j++)
            {
            if (m_Comps[j]==*DCB.rL)
              {
              m_Comps.RemoveAt(j);
              m_DDBCmpsOK=false;
              Blk.ClrMeasEtc();
              break;
              }
            }
          }
        else if (*DCB.rL==-2)
          {
          m_Comps.SetSize(0);
          m_DDBCmpsOK=false;
          Blk.ClrMeasEtc();
          }
        }
      DCB.L=-1;
      return 1;

    default:
      if (DCB.lHandle>=xidMkPhase && DCB.lHandle<xidMkPhase+CDB.PhaseCount())
        {
        int o=DCB.lHandle-xidMkPhase;
        CPhaseInfo & P=CDB.PhaseInfo(o);
        if (DCB.rB)
          {
          m_Phases = (m_Phases&~P.m_PhMsk) | (*DCB.rB ? P.m_PhMsk:0);
          Blk.ClrMeasEtc();
          }
        DCB.B = (m_Phases&P.m_PhMsk) ? 1 : 0; 
        return 1;
        }
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag CMeasInfo::ValidateData(ValidateDataBlk & VDB, CXBlk_MUFeed &Blk)
  {
  switch (m_eSelect)
    {
    case Slct_All:
      {
      m_Phases=som_ALL;
      break;
      }
    case Slct_Specie:
      {
      break;
      }
    case Slct_Component:
      {
      m_Species.SetSize(0);
      for (int i=0; i<m_Comps.GetCount(); i++)
        {
        for (int j=0; j<CDB[i].NSpecies(); j++)
          {
          int s=CDB[i].iSpecie(j);
          m_Species.Add(s);
          }
        }
      break;
      }
    default:
      break;
    }

  return true;
  }


//============================================================================
//
//
//
//============================================================================

class DllImportExport CXBlk_MUFixed : public CXBlk_MUFeed
  {
  public:
    CXBlk_MUFixed(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUFeed(false, pClass_, Tag_, pAttach, eAttach)
      {
      m_AsRatio=false;
      };
    virtual ~CXBlk_MUFixed() 
      {
      };
  };

class DllImportExport CXBlk_MURatio : public CXBlk_MUFeed
  {
  public:
    CXBlk_MURatio(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUFeed(true, pClass_, Tag_, pAttach, eAttach)
      {
      };
    virtual ~CXBlk_MURatio() 
      {
      };
  };


DEFINE_MAKEUPBLOCK(CXBlk_MUFixed);
DEFINE_MAKEUPBLOCK(CXBlk_MURatio);

//============================================================================
//
//
//
//============================================================================

CToleranceBlock CXBlk_MUFeed::sm_QmTol(TBF_DynSys, "Makeup:CtrlEPS", 1.0e-8, 1.0e-8, 200);

IMPLEMENT_MAKEUPBLOCK(CXBlk_MUFixed, "MB_Fixed", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Fixed Makeup",  " ");
IMPLEMENT_MAKEUPBLOCK(CXBlk_MURatio, "MB_Ratio", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Ratio Makeup",  " ");

CXBlk_MUFeed::CXBlk_MUFeed(bool AsRatio, pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
CMakeupBlock(pClass_, Tag_, pAttach, eAttach)
  {
  m_AsRatio   = AsRatio; 
  m_eSource   = Src_Remote;
  m_eType     = Type_Mass;

  m_MUQmMin   = 0.0;
  m_MUQmMax   = 10000.0;

  m_eLoFeedOpt= LF_Ignore;
  m_LoFeedQm  = 1.0;

  m_QmRqd     = 0;
  m_QMlRqd    = 0;
  m_QvRqd     = 0;
  m_NQvRqd    = 0;
  m_QmRatio   = 0;
  m_QMlRatio  = 0;
  m_QvRatio   = 0;
  m_NQvRatio  = 0;


  m_bHasFlow  = true;

  m_dSetPoint   = 0.0;
  m_dMeas       = 0.0;
  m_dResult     = 0.0;
  m_dQmMakeup   = 0.0;
  m_dQmFeed     = 0.0;
  m_dQmProd     = 0.0;
  m_dTempKFeed  = C2K(0.0);
  m_dTempKProd  = C2K(0.0);

  ClrMeasEtc();
  }

//--------------------------------------------------------------------------

CXBlk_MUFeed::~CXBlk_MUFeed()
  {
  }

//--------------------------------------------------------------------------

void CXBlk_MUFeed::BuildDataDefn(DataDefnBlk& DDB)
  {

  //if (DDB.BeginStruct(this, "MakeupQm", NULL, DDB_NoPage))
  //  {
  static DDBValueLst DDBCtrl[] =
    {                         
      {Type_Mass,           "Mass"          },
      {Type_Volume,         "Volume"        },
      {Type_NVolume,        "NVolume"       },
      {Type_Mole,           "Mole"          },
      {}
    };

  static DDBValueLst DDBSource[] =
    {                         
      {Src_Self,            "Self"            },
      {Src_Remote,          "Remote"          },
      {}
    };

  static DDBValueLst DDBLoFeed[] =
    {                         
      {LF_Ignore,           "Ignore"            },
      {LF_StopMakeUp,       "StopMakeup"        },
      //{LF_HoldMeasure,      "HoldAtLo"          },
      {}
    };

  const bool MassBasis = (m_eType==Type_Mass);
  const bool VolBasis  = (m_eType==Type_Volume);
  const bool NVolBasis = (m_eType==Type_NVolume);
  const bool MoleBasis = (m_eType==Type_Mole);

  DDB.Text(" ");
  DDB.Text("SetPoint");
  DDB.Long       ("", "Type",             DC_,  "", xidMkType,  this, isParmStopped|SetOnChange, DDBCtrl);
  if (m_AsRatio)
    {
    DDB.Visibility(NSHM_All, m_eType==Type_Mass);
    DDB.Double("", "QmRatio",    DC_Frac, "%", &m_QmRatio,  this, isParm);
    DDB.Visibility(NSHM_All, m_eType==Type_Mole);
    DDB.Double("", "QMlRatio",    DC_Frac, "%", &m_QMlRatio,  this, isParm);
    DDB.Visibility(NSHM_All, m_eType==Type_Volume);
    DDB.Double("", "QvRatio",    DC_Frac, "%", &m_QvRatio,  this, isParm);
    DDB.Visibility(NSHM_All, m_eType==Type_NVolume);
    DDB.Double("", "NQvRatio",    DC_Frac, "%", &m_NQvRatio,  this, isParm);
    }
  else
    {
    DDB.Visibility(NSHM_All, m_eType==Type_Mass);
    DDB.Double("", "QmRqd",    DC_Qm, "kg/s", &m_QmRqd,  this, isParm);
    DDB.Visibility(NSHM_All, m_eType==Type_Mole);
    DDB.Double("", "QMlRqd",   DC_QKgMl, "kmol/s", &m_QMlRqd,  this, isParm);
    DDB.Visibility(NSHM_All, m_eType==Type_Volume);
    DDB.Double("", "QvRqd",    DC_Qv, "m^3/s", &m_QvRqd,  this, isParm);
    DDB.Visibility(NSHM_All, m_eType==Type_NVolume);
    DDB.Double("", "NQvRqd",    DC_NQv, "Nm^3/s", &m_NQvRqd,  this, isParm);
    }

  DDB.Visibility();
  if (m_AsRatio)
    {
    DDB.Text("Input Measurement");
    m_In.BuildDataDefn(DDB, *this, "Feed", 1);
    }
  DDB.Text("Makeup Measurement");
  m_Mu.BuildDataDefn(DDB, *this, "Makeup", 2);

  DDB.Visibility();
  DDB.Text(" ");
  DDB.Text("MakeUp");
  DDB.Long       ("", "Source",           DC_,   "", (long*)&m_eSource,  this, isParm|SetOnChange, DDBSource);
  DDB.Double     ("QmMin", "",            DC_Qm, "kg/s", &m_MUQmMin, this, isParm);
  DDB.Double     ("QmMax", "",            DC_Qm, "kg/s", &m_MUQmMax, this, isParm);

  DDB.Long       ("LoFeed.Options", "",   DC_,   "", (long*)&m_eLoFeedOpt, this, isParm|SetOnChange, DDBLoFeed);
  DDB.Visibility (NSHM_All, m_eLoFeedOpt >LF_Ignore);
  DDB.Double     ("LoFeed.Qm", "",        DC_Qm, "kg/s", &m_LoFeedQm, this, isParm);
  DDB.Visibility ();

  DDB.Text(" ");
  DDB.Text("Results");

  DDB.Text("Simple Control:");
  DDB.String("Error",            "", DC_,   "",    xidMkError,     this, isResult);

  CCnvIndex CnvUsed;
  Strng CnvTxt;
  if (m_AsRatio)
    {
    CnvUsed=DC_Frac; 
    CnvTxt="%"; 
    }
  else
    {
    switch (m_eType)
      {                         
      case Type_Mass    : CnvUsed=DC_Qm; CnvTxt="kg/s"; break;
      case Type_Mole    : CnvUsed=DC_QKgMl; CnvTxt="kmol/s"; break;
      case Type_Volume  : CnvUsed=DC_Qv; CnvTxt="m^3/s"; break;
      case Type_NVolume : CnvUsed=DC_NQv; CnvTxt="Nm^3/s"; break;
      }
    }

  DDB.Text(" ");
  if (m_AsRatio)
    {
    DDB.Text(m_In.m_MeasDesc());
    if (MassBasis)
      {
      DDB.Double ("Meas.Qm.Feed",   "", DC_Qm,    "kg/s",     &m_dFeedAct,     this, isResult|noFileAtAll|NAN_OK);
      DDB.Double ("Meas.Qm.Prod",   "", DC_Qm,    "kg/s",     &m_dProdAct,     this, isResult|noFileAtAll|NAN_OK);
      }
    else if (VolBasis)
      {
      DDB.Double ("Meas.Qv.Feed",   "", DC_Qv,    "m^3/s",    &m_dFeedAct,     this, isResult|noFileAtAll|NAN_OK);
      DDB.Double ("Meas.Qv.Prod",   "", DC_Qv,    "m^3/s",    &m_dProdAct,     this, isResult|noFileAtAll|NAN_OK);
      }
    else if (NVolBasis)
      {
      DDB.Double ("Meas.NQv.Feed",  "", DC_NQv,   "Nm^3/s",   &m_dFeedAct,     this, isResult|noFileAtAll|NAN_OK);
      DDB.Double ("Meas.NQv.Prod",  "", DC_NQv,   "Nm^3/s",   &m_dProdAct,     this, isResult|noFileAtAll|NAN_OK);
      }
    else if (MoleBasis)
      {
      DDB.Double ("Meas.QMl.Feed",  "", DC_QKgMl, "kmol/s",   &m_dFeedAct,     this, isResult|noFileAtAll|NAN_OK);
      DDB.Double ("Meas.QMl.Prod",  "", DC_QKgMl, "kmol/s",   &m_dProdAct,     this, isResult|noFileAtAll|NAN_OK);
      }
    }

  DDB.Text(m_Mu.m_MeasDesc());
  if (MassBasis)
    DDB.Double ("Meas.Qm.Makeup",   "", DC_Qm,    "kg/s",     &m_dMakeupAct,     this, isResult|noFileAtAll|NAN_OK);
  else if (VolBasis)
    DDB.Double ("Meas.Qv.Makeup",   "", DC_Qv,    "m^3/s",    &m_dMakeupAct,     this, isResult|noFileAtAll|NAN_OK);
  else if (NVolBasis)
    DDB.Double ("Meas.NQv.Makeup",  "", DC_NQv,   "Nm^3/s",   &m_dMakeupAct,     this, isResult|noFileAtAll|NAN_OK);
  else if (MoleBasis)
    DDB.Double ("Meas.QMl.Makeup",  "", DC_QKgMl, "kmol/s",   &m_dMakeupAct,     this, isResult|noFileAtAll|NAN_OK);
 
  DDB.Text(" ");
  DDB.Text("Total mass flow:");
  DDB.Double ("Qm.Feed",            "", DC_Qm,    "kg/s",     &m_dQmFeed,     this, isResult);
  DDB.Double ("Qm.Makeup",          "", DC_Qm,    "kg/s",     &m_dQmMakeup,   this, isResult);
  DDB.Double ("Qm.Prod",            "", DC_Qm,    "kg/s",     &m_dQmProd,     this, isResult);
  if (!HeatSkipMethod())
    {
    DDB.Text("Total heat flow:");
    DDB.Double ("Temp.Feed",          "", DC_T,    "C",       &m_dTempKFeed,  this, isResult);//|noFileAtAll);
    //DDB.Double ("HeatFlow",           "", DC_Pwr,  "kW",      &m_dHeatFlow,   this, isResult);
    DDB.Double ("Temp.Prod",          "", DC_T,    "C",       &m_dTempKProd,  this, isResult);//|noFileAtAll);
    }
  };

//--------------------------------------------------------------------------

void CXBlk_MUFeed::ClrMeasEtc()
  {
  m_dMeas     = dNAN;
  m_dSetPoint = dNAN;
  m_dResult   = dNAN;
  m_dFeedAct  = dNAN;
  m_dProdAct  = dNAN;
  m_dMakeupAct= dNAN;
  }

flag CXBlk_MUFeed::DataXchg(DataChangeBlk & DCB)
  {
  if (DCB.dwUserInfo==1 && m_In.DataXchg(DCB, *this))
    return 1;
  if (DCB.dwUserInfo==2 && m_Mu.DataXchg(DCB, *this))
    return 1;

  switch (DCB.lHandle)
    {
    case xidMkType:
      if (DCB.rL && (m_eType!=(eType)*DCB.rL))
        {          
        m_eType     = (eType)*DCB.rL;
        ClrMeasEtc();
        }
      DCB.L = m_eType; 
      return 1;

    case xidMkError:
      {
      m_ErrorLst.Clear();
      ConditionBlk::GetMyCIs(m_ErrorLst,3);
      if (!SrcIO.Connected)
        {
        DCB.pC="Not Connected";
        }
      else if (!m_bHasFlow)
        {
        DCB.pC="No Flow";
        }
      else if (m_ErrorLst.Length()>0)
        {
        Strng &S=*m_ErrorLst.First();
        DCB.pC=S.XStrChr('\t')+1;
        }
      else
        DCB.pC="None";
      return 1;
      }
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag CXBlk_MUFeed::ValidateData(ValidateDataBlk & VDB)
  {
  m_In.ValidateData(VDB, *this);
  m_Mu.ValidateData(VDB, *this);

  return CMakeupBlock::ValidateData(VDB); 
  }

//--------------------------------------------------------------------------

double CXBlk_MUFeed::GetSetPoint()
  {
  if (m_AsRatio)
    {
    switch (m_eType)
      {                         
      case Type_Mass:      return m_QmRatio;
      case Type_Mole:      return m_QMlRatio;
      case Type_Volume:    return m_QvRatio;
      case Type_NVolume:   return m_NQvRatio;
      };
    }
  else
    {
    switch (m_eType)
      {                         
      case Type_Mass:       return m_QmRqd;
      case Type_Mole:       return m_QMlRqd;
      case Type_Volume:     return m_QvRqd;
      case Type_NVolume:    return m_NQvRqd;
      };
    }
  return dNAN;
  }

//--------------------------------------------------------------------------

double CXBlk_MUFeed::GetFlowValue(CMeasInfo &MI, SpConduit &QPrd, PhMask PhRqd)
  {
  switch (m_eType)
    {                         
    case Type_Mass:
      if (PhRqd)
        return QPrd.QMass(PhRqd);
      if (MI.m_eSelect>=CMeasInfo::Slct_Specie)
        return QPrd.QMass(MI.m_Species);
      return QPrd.QMass(MI.m_Phases);

    case Type_Mole:
      if (PhRqd)
        return QPrd.QMole(PhRqd);
      if (MI.m_eSelect>=CMeasInfo::Slct_Specie)
        return QPrd.QMole(MI.m_Species);
      return QPrd.QMole(MI.m_Phases);

    case Type_Volume:
      if (PhRqd)
        return QPrd.QVolume(PhRqd);
      if (MI.m_eSelect>=CMeasInfo::Slct_Specie)
        return QPrd.QVolume(MI.m_Species);
      return QPrd.QVolume(MI.m_Phases);

    case Type_NVolume:
      if (PhRqd)
        return QPrd.QNVolume(PhRqd);
      if (MI.m_eSelect>=CMeasInfo::Slct_Specie)
        return QPrd.QNVolume(MI.m_Species);
      return QPrd.QNVolume(MI.m_Phases);

    }
  return dNAN;
  }

double CXBlk_MUFeed::GetMeasVal(SpConduit &QIn, SpConduit &QSrc, SpConduit &QPrd)
  {
  double In  = GetFlowValue(m_In, QIn);
  double Src = GetFlowValue(m_Mu, QSrc);
  double Prd = GetFlowValue(m_In, QPrd);
  if (m_AsRatio)
    return Src/GTZ(In);
  else
    return Src;
  }

//-------------------------------------------------------------------------

class CFeedMkUpFnd : public MRootFinder
  {
  public:
    CFeedMkUpFnd(CXBlk_MUFeed * pMU, LPCTSTR pTag, SpConduit * pIn, SpConduit * pSrc, SpConduit * pSrcWrk, SpConduit * pPrd, /*double TRqd, double PRqd,*/ CToleranceBlock & Tol) : \
      m_pMU(pMU), m_pTag(pTag), m_In(*pIn), m_Src(*pSrc), m_SrcWrk(*pSrcWrk), m_Prd(*pPrd), /*m_TRqd(TRqd), m_PRqd(PRqd),*/ MRootFinder("MkUpFnd", Tol)
      { 
      };
    LPCTSTR ObjTag() { return m_pTag; };
    double Function(double Qm)
      {
      m_Prd.QCopy(m_In);
      m_SrcWrk.QSetM(m_Src, som_ALL, Qm);
      m_Prd.QAddF(m_SrcWrk, som_ALL, 1.0);
      dbgpln("   FeedConverge Qm:%20.6f Meas:%20.6f SetPt:%20.6f In:%20.6f Src:%20.6f", Qm, m_pMU->GetMeasVal(m_In, m_SrcWrk, m_Prd), m_pMU->GetSetPoint(), m_In.QMass(), m_Src.QMass());
      return m_pMU->GetMeasVal(m_In, m_SrcWrk, m_Prd);
      };

  protected:
    CXBlk_MUFeed  * m_pMU;
    LPCTSTR         m_pTag;
    SpConduit     & m_In;
    SpConduit     & m_Src;
    SpConduit     & m_Prd;
    SpConduit     & m_SrcWrk;
  };

//--------------------------------------------------------------------------

void CXBlk_MUFeed::EvalProducts(SpConduit &QPrd, double Po, double FinalTEst)
  {
  FlwNode *pNd=FindObjOfType((FlwNode*)NULL);
  ASSERT_ALWAYS(pNd, "Should always be part of a FlwNode");

  if (QPrd.QMass()>SmallPosFlow)
    {
    m_bHasFlow = true;

    m_dQmFeed = QPrd.QMass();

    bool StopMakeUp = (m_eLoFeedOpt>LF_Ignore) && (m_dQmFeed<m_LoFeedQm); 

    m_dTempKFeed = QPrd.Temp();
    const double HzIn = QPrd.totHz();

    StkSpConduit QIn("QIn", "MkUp", pNd);
    QIn().QCopy(QPrd);

    StkSpConduit QSrcWrk("QSrcWrk", "SrcWrk", pNd);

    SpConduit &QSrc=SrcIO.Cd;

    m_dMeas     = GetMeasVal(QIn(), QSrc, QPrd);
    m_dFeedAct  = GetFlowValue(m_In, QIn());

    bool CIsOn[10]={false,false,false,false,false,false,false,false,false};
    if (m_AsRatio)
      {
      if (m_In.m_eSelect>=CMeasInfo::Slct_Specie)
        CIsOn[6]=(m_In.m_Species.GetCount()==0);
      else 
        CIsOn[5]=(m_In.m_Phases==0);
      }
    if (m_Mu.m_eSelect>=CMeasInfo::Slct_Specie)
      CIsOn[8]=(m_In.m_Species.GetCount()==0);
    else 
      CIsOn[7]=(m_In.m_Phases==0);

    CIsOn[9]=StopMakeUp;
    if (!StopMakeUp && !CIsOn[5] && !CIsOn[6] && !CIsOn[7] && !CIsOn[8])
      {
      // Copy to Src if Self
      if (m_eSource==Src_Self)
        QSrc.QSetF(QPrd, som_ALL, 1.0);

      CFeedMkUpFnd MkUpFnd(this, BaseTag(), &QIn(), &QSrc, &QSrcWrk(), &QPrd, /*TReqd, Po,*/ sm_QmTol);
      int iRet=MkUpFnd.FindRoot(GetSetPoint(), m_MUQmMin, m_MUQmMax, m_dQmMakeup, 0.0);
      switch (iRet)
        {
        case RF_OK:         
          m_dQmMakeup = MkUpFnd.Result();
          break;
        case RF_LoLimit:    
        case RF_EstimateLoLimit:    
          m_dQmMakeup = MkUpFnd.Result();
          CIsOn[2]=true;
          break;
        case RF_HiLimit:    
        case RF_EstimateHiLimit:    
          m_dQmMakeup = MkUpFnd.Result();   
          CIsOn[3]=true;
          break;
        case RF_Independant:
          MkUpFnd.Function(0);   
          m_dQmMakeup = MkUpFnd.Result();   
          CIsOn[4]=true;
          break;

        default: 
          CIsOn[1]=true;
          SetCI(1, "E\tConverge Error [%i]", iRet);
          break;
        }

      //QSrc.QAdjustQmTo(som_ALL, m_dQmMakeup);
      QSrc.QCopy(QSrcWrk());
      }
    if (SrcIO.Enabled)
      SrcIO.Sum.Set(QSrc);
    else
      SrcIO.Sum.ZeroFlows();
  
    if (!CIsOn[1])
      ClrCI(1);

    for (int i=2; i<=9; i++)
      SetCI(i, CIsOn[i]);

    m_dSetPoint   = GetSetPoint();
    m_dResult     = GetMeasVal(QIn(), QSrc, QPrd);
    m_dQmProd     = QPrd.QMass();
    m_dQmMakeup   = m_dQmProd-m_dQmFeed;
    m_dTempKProd  = QPrd.Temp();

    m_dProdAct    = GetFlowValue(m_In, QPrd);
    m_dMakeupAct  = GetFlowValue(m_Mu, QSrcWrk());
    }
  else
    {
    m_bHasFlow    = false;
    m_dQmFeed     = QPrd.QMass();
    m_dQmMakeup   = 0;
    m_dFeedAct    = GetFlowValue(m_In, QPrd);
    m_dProdAct    = GetFlowValue(m_In, QPrd);
    m_dMakeupAct  = 0;
    m_dMeas       = dNAN;
    m_dSetPoint   = GetSetPoint();
    m_dResult     = dNAN;
    m_dQmProd     = 0;
    m_dQmMakeup   = 0;
    m_dTempKProd  = QPrd.Temp();
    ClrCI(1);
    ClrCI(2);
    ClrCI(3);
    ClrCI(4);
    SrcIO.Cd.QZero();
    SrcIO.Sum.ZeroFlows();
    }

  if (SrcIO.MyConnectedIO()>=0)
    pNd->SetIOQm_In(SrcIO.MyConnectedIO(), m_dQmMakeup);

  };

void CXBlk_MUFeed::EvalProductsPipe(SpConduit & Qf, double Len, double Diam, double Po, double FinalTEst)
  {
  EvalProducts(Qf, Po, FinalTEst);
  };

//--------------------------------------------------------------------------

flag CXBlk_MUFeed::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\tConverge Error"; return 1;
    case  2: pS="E\tRequirement not Achieved - Low Limit"; return 1;
    case  3: pS="E\tRequirement not Achieved - High Limit"; return 1;
    case  4: pS="E\tMakeup has No Effect"; return 1;
    case  5: pS="E\tNo Phase Selected for Measurement in Feed"; return 1;
    case  6: pS="E\tNo Species Selected for Measurement in Feed"; return 1;
    case  7: pS="E\tNo Phase Selected for Measurement in Makeup"; return 1;
    case  8: pS="E\tNo Species Selected for Measurement in Makeup"; return 1;
    case  9: pS="W\tLow Feed - Makeup Stopped"; return 1;
    default:
      return CXBlk_MUFeed::CIStrng(No, pS);
    }
  }

//============================================================================
//
//
//
//============================================================================

#endif

//============================================================================
//
//
//
//============================================================================

#define DllImportExport /* */

class DllImportExport CXBlk_MUSimple: public CMakeupBlock
  {
  public:
    CXBlk_MUSimple(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CXBlk_MUSimple();

    void           SetUpDDBSpcs();
    void           SetUpDDBCmps();
    void           ClrMeasEtc();

    virtual flag   DoesSomething() { return true; };

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void   EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

    double         GetSetPoint();
    double         GetRawMeas(SpConduit &QPrd, PhMask PhRqd=0);
    double         GetMeasVal(SpConduit &QIn, SpConduit &QPrd);
    double         GetFlowValue(SpConduit &QPrd, PhMask PhRqd=0);

    DEFINE_CI(CXBlk_MUSimple, CMakeupBlock, 4);
    //char *         CBTag() { m_MyTag=FullObjTag(); _MyTag+="."; m_MyTag+=Name()(); return m_MyTag(); };

  public:
    enum eSource
      {
      Src_Self,
      Src_Remote,
      };

    enum eType
      { 
      //Type_None, 
      Type_MassFrac,
      Type_MassFlow, 
      Type_MassRatio, 
      Type_MassMult, 
      Type_VolumeFrac,
      Type_VolumeFlow, 
      Type_VolumeRatio, 
      Type_VolumeMult, 
      Type_NVolumeFrac,
      Type_NVolumeFlow, 
      Type_NVolumeRatio,
      Type_NVolumeMult,
      Type_MoleFrac,
      Type_MoleFlow, 
      Type_MoleRatio, 
      Type_MoleMult, 
      Type_Conc,
      };

    enum eSelect
      {
      Slct_All,
      Slct_Occur,
      Slct_IndPhase,
      Slct_Specie,
      Slct_Component,
      };

    enum eTemp
      {
      Temp_Inlet,
      Temp_Source,
      Temp_Std,
      Temp_Const,
      Temp_Mixture,
      };

    Strng           m_MyTag;
    eSource         m_eSource;
    double          m_QmMin;
    double          m_QmMax;
    eType           m_eType;
    eSelect         m_eSelect;
    PhMask          m_Phases; 
    CIArray         m_Species;
    CIArray         m_Comps;
    bool            m_DDBSpcsOK;
    DDBValueLstMem  m_DDBSpcAdd;
    DDBValueLstMem  m_DDBSpcRem;
    int             m_nLastSpcStr;
    CStringArray    m_SpcStr;
    bool            m_DDBCmpsOK;
    DDBValueLstMem  m_DDBCmpAdd;
    DDBValueLstMem  m_DDBCmpRem;
    int             m_nLastCmpStr;
    Strng           m_MeasDesc;
    CStringArray    m_CmpStr;

    bool            m_bHasFlow;

    double          m_QmRqd;
    double          m_QmRatio;
    double          m_QmMult;
    double          m_MassFrac;
    double          m_QMlRqd;
    double          m_QMlRatio;
    double          m_QMlMult;
    double          m_MoleFrac;
    double          m_QvRqd;
    double          m_QvRatio;
    double          m_QvMult;
    double          m_VolFrac;
    double          m_NQvRqd;
    double          m_NQvRatio;
    double          m_NQvMult;
    double          m_NVolFrac;
    double          m_Conc;

    eTemp           m_eRqdTemp;
    double          m_RqdTemp;

    //results
    double          m_dSetPoint;
    double          m_dMeas;
    double          m_dResult;
    double          m_dFeedAct;
    double          m_dProdAct;

    double          m_dQmMakeup;
    double          m_dQmFeed;
    double          m_dQmProd;
    double          m_dHeatFlow;
    double          m_dTempKFeed;
    double          m_dTempKProd;

    Strng_List      m_ErrorLst;

    static CToleranceBlock sm_QmTol;

  };

DEFINE_MAKEUPBLOCK(CXBlk_MUSimple);

//============================================================================
//
//
//
//============================================================================

//XID xidMkType    = AdjustXID(1000);
//XID xidMkSelect  = AdjustXID(1001);
//XID xidMkAll     = AdjustXID(1002);
//XID xidMkSolids  = AdjustXID(1003);
//XID xidMkLiquids = AdjustXID(1004);
//XID xidMkGasses  = AdjustXID(1006);
//XID xidMkSpcCnt  = AdjustXID(1007);
//XID xidMkAddSpc  = AdjustXID(1008);
//XID xidMkRemSpc  = AdjustXID(1009);
//XID xidMkCmpCnt  = AdjustXID(1010);
//XID xidMkAddCmp  = AdjustXID(1011);
//XID xidMkRemCmp  = AdjustXID(1012);
//XID xidMkError   = AdjustXID(1013);
//XID xidMkPhase   = AdjustXID(1200);


CToleranceBlock CXBlk_MUSimple::sm_QmTol(TBF_DynSys, "Makeup:CtrlEPS", 1.0e-8, 1.0e-8, 200);

IMPLEMENT_MAKEUPBLOCK(CXBlk_MUSimple, "MB_Simple", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "SimpleControl",  " ");

CXBlk_MUSimple::CXBlk_MUSimple(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
CMakeupBlock(pClass_, Tag_, pAttach, eAttach)
  {
  m_eSource   = Src_Remote;
  m_eType     = Type_MassRatio;
  m_eSelect   = Slct_All;
  m_Phases    = som_ALL;
  m_DDBSpcsOK = false;
  m_DDBCmpsOK = false;

  m_QmMin     = 0.0;
  m_QmMax     = 10000.0;

  m_QmRqd     = 0;
  m_QMlRqd    = 0;
  m_QvRqd     = 0;
  m_NQvRqd    = 0;
  m_QmRatio   = 0;
  m_QMlRatio  = 0;
  m_QvRatio   = 0;
  m_NQvRatio  = 0;
  m_QmMult    = 1.0;
  m_QMlMult   = 1.0;
  m_QvMult    = 1.0;
  m_NQvMult   = 1.0;
  m_MassFrac  = 0;
  m_MoleFrac  = 0;
  m_VolFrac   = 0;
  m_NVolFrac  = 0;
  m_Conc      = 0;

  m_bHasFlow  = true;

  m_eRqdTemp  = Temp_Inlet;
  m_RqdTemp   = C2K(25);

  m_dSetPoint   = 0.0;
  m_dMeas       = 0.0;
  m_dResult     = 0.0;
  m_dQmMakeup   = 0.0;
  m_dQmFeed     = 0.0;
  m_dQmProd     = 0.0;
  m_dHeatFlow   = 0.0;
  m_dTempKFeed  = C2K(0.0);
  m_dTempKProd  = C2K(0.0);

  }

//--------------------------------------------------------------------------

CXBlk_MUSimple::~CXBlk_MUSimple()
  {
  }

//--------------------------------------------------------------------------

void CXBlk_MUSimple::SetUpDDBSpcs()
  {

  if (!m_DDBSpcsOK || m_DDBSpcAdd.Length()+m_DDBSpcRem.Length()<SDB.Count())
    {
    m_DDBSpcAdd.Empty();
    m_DDBSpcRem.Empty();
    m_DDBSpcAdd.Add(-2, "All_Available");
    m_DDBSpcAdd.Add(-1, " - ");
    m_DDBSpcRem.Add(-2, "All_Available");
    m_DDBSpcRem.Add(-1, " - ");

    bool InList[MaxSpeciesEx];
    for (int s=0; s<SDB.Count(); s++)
      InList[s]=false;

    bool NeedsComma=false;
    m_nLastSpcStr=0;
    m_SpcStr.SetSize(Max(m_nLastSpcStr+1, m_SpcStr.GetSize()));
    m_SpcStr[m_nLastSpcStr]="Species:";
    for (int j=0; j<m_Species.GetSize(); j++)
      {
      int s=m_Species[j];
      InList[s]=true;
      m_DDBSpcRem.Add(s, SDB[s].SymOrTag());

      if (NeedsComma)
        m_SpcStr[m_nLastSpcStr]+=",";
      m_SpcStr[m_nLastSpcStr]+=SDB[s].SymOrTag();
      if (m_SpcStr[m_nLastSpcStr].GetLength()>35)
        {
        m_nLastSpcStr++;
        m_SpcStr.SetSize(Max(m_nLastSpcStr+1, m_SpcStr.GetSize()));
        m_SpcStr[m_nLastSpcStr]="        ";
        NeedsComma=false;
        }
      else
        NeedsComma=true;
      }

    for (int s=0; s<SDB.Count(); s++)
      {
      if (!InList[s])
        m_DDBSpcAdd.Add(s,SDB[s].SymOrTag());
      }
    }

  m_DDBSpcsOK = true;
  }

//--------------------------------------------------------------------------

void CXBlk_MUSimple::SetUpDDBCmps()
  {
  if (!m_DDBCmpsOK || m_DDBCmpAdd.Length()+m_DDBCmpRem.Length()<CDB.No())
    {
    m_DDBCmpAdd.Empty();
    m_DDBCmpRem.Empty();
    m_DDBCmpAdd.Add(-2, "All_Available");
    m_DDBCmpAdd.Add(-1, " - ");
    m_DDBCmpRem.Add(-2, "All_Available");
    m_DDBCmpRem.Add(-1, " - ");

    bool InList[MaxSpeciesEx];
    for (int s=0; s<CDB.No(); s++)
      InList[s]=false;

    bool NeedsComma=false;
    m_nLastCmpStr=0;
    m_CmpStr.SetSize(Max(m_nLastCmpStr+1, m_CmpStr.GetSize()));
    m_CmpStr[m_nLastCmpStr]="Comps:";
    for (int j=0; j<m_Comps.GetSize(); j++)
      {
      int s=m_Comps[j];
      InList[s]=true;
      m_DDBCmpRem.Add(s, CDB[s].SymOrTag());

      if (NeedsComma)
        m_CmpStr[m_nLastCmpStr]+=",";
      m_CmpStr[m_nLastCmpStr]+=CDB[s].SymOrTag();
      if (m_CmpStr[m_nLastCmpStr].GetLength()>35)
        {
        m_nLastCmpStr++;
        m_CmpStr.SetSize(Max(m_nLastCmpStr+1, m_CmpStr.GetSize()));
        m_CmpStr[m_nLastCmpStr]="        ";
        NeedsComma=false;
        }
      else
        NeedsComma=true;
      //for (int ss=CDB[s].
      }

    for (int s=0; s<CDB.No(); s++)
      {
      if (!InList[s])
        m_DDBCmpAdd.Add(s,CDB[s].SymOrTag());
      }
    }

  m_DDBCmpsOK = true;
  }

//--------------------------------------------------------------------------

void CXBlk_MUSimple::BuildDataDefn(DataDefnBlk& DDB)
  {

  //if (DDB.BeginStruct(this, "MakeupQm", NULL, DDB_NoPage))
  //  {
  static DDBValueLst DDBCtrl[] =
    {                         
      {Type_MassFrac,       "MassFrac"          },
      {Type_MassFlow,       "MassFlow"          },
      {Type_MassRatio,      "MassRatio"         },
      {Type_MassMult,       "MassMultiplier"    },
      {Type_VolumeFrac,     "VolumeFrac"        },
      {Type_VolumeFlow,     "VolumeFlow"        },
      {Type_VolumeRatio,    "VolumeRatio"       },
      {Type_VolumeMult,     "VolumeMultiplier"  },
      {Type_NVolumeFrac,    "NVolumeFrac"       },
      {Type_NVolumeFlow,    "NVolumeFlow"       },
      {Type_NVolumeRatio,   "NVolumeRatio"      },
      {Type_NVolumeMult,    "NVolumeMultiplier" },
      {Type_MoleFrac,       "MoleFrac"          },
      {Type_MoleFlow,       "MoleFlow"          },
      {Type_MoleRatio,      "MoleRatio"         },
      {Type_MoleMult,       "MoleMultiplier"    },
      {Type_Conc,           "Concentration"     },
      {}
    };
  static DDBValueLst DDBSelect[] =
    {                         
      {Slct_All,            "All"             },
      {Slct_Occur,          "Phase"           },
      {Slct_IndPhase,       "IndividualPhase" },
      {Slct_Specie,         "Specie"          },
      {Slct_Component,      "Component"       },
      {}
    };
  static DDBValueLst DDBSource[] =
    {                         
      {Src_Self,            "Self"            },
      {Src_Remote,          "Remote"          },
      {}
    };

  const bool MassBasis = (m_eType==Type_MassFrac || m_eType==Type_MassFlow || m_eType==Type_MassRatio || m_eType==Type_MassMult);
  const bool VolBasis  = (m_eType==Type_VolumeFrac || m_eType==Type_VolumeFlow || m_eType==Type_VolumeRatio || m_eType==Type_VolumeMult);
  const bool NVolBasis = (m_eType==Type_NVolumeFrac || m_eType==Type_NVolumeFlow || m_eType==Type_NVolumeRatio || m_eType==Type_NVolumeMult);
  const bool MoleBasis = (m_eType==Type_MoleFrac || m_eType==Type_MoleFlow || m_eType==Type_MoleRatio || m_eType==Type_MoleMult);

  const bool FracBasis  = (m_eType==Type_MassFrac || m_eType==Type_VolumeFrac || m_eType==Type_NVolumeFrac || m_eType==Type_MoleFrac);
  const bool FlowBasis  = (m_eType==Type_MassFlow || m_eType==Type_VolumeFlow || m_eType==Type_NVolumeFlow || m_eType==Type_MoleFlow);
  const bool RatioBasis = (m_eType==Type_MassRatio || m_eType==Type_VolumeRatio || m_eType==Type_NVolumeRatio || m_eType==Type_MoleRatio);
  const bool MultBasis  = (m_eType==Type_MassMult || m_eType==Type_VolumeMult || m_eType==Type_NVolumeMult || m_eType==Type_MoleMult);

  DDB.Text(" ");
  DDB.Text("SetPoint");
  DDB.Long       ("", "Type",             DC_,  "", xidMkType,  this, isParmStopped|SetOnChange, DDBCtrl);
  DDB.Visibility(NSHM_All, m_eType==Type_MassFlow);
  DDB.Double("", "QmRqd",    DC_Qm, "kg/s", &m_QmRqd,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_MoleFlow);
  DDB.Double("", "QMlRqd",   DC_QKgMl, "kmol/s", &m_QMlRqd,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_VolumeFlow);
  DDB.Double("", "QvRqd",    DC_Qv, "m^3/s", &m_QvRqd,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_NVolumeFlow);
  DDB.Double("", "NQvRqd",    DC_NQv, "Nm^3/s", &m_NQvRqd,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_MassRatio);
  DDB.Double("", "QmRatio",    DC_Frac, "%", &m_QmRatio,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_MoleRatio);
  DDB.Double("", "QMlRatio",    DC_Frac, "%", &m_QMlRatio,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_VolumeRatio);
  DDB.Double("", "QvRatio",    DC_Frac, "%", &m_QvRatio,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_NVolumeRatio);
  DDB.Double("", "NQvRatio",    DC_Frac, "%", &m_NQvRatio,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_MassMult);
  DDB.Double("", "QmMult",    DC_Frac, "%", &m_QmMult,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_MoleMult);
  DDB.Double("", "QMlMult",    DC_Frac, "%", &m_QMlMult,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_VolumeMult);
  DDB.Double("", "QvMult",    DC_Frac, "%", &m_QvMult,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_NVolumeMult);
  DDB.Double("", "NQvMult",    DC_Frac, "%", &m_NQvMult,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_MassFrac);
  DDB.Double("", "MassFrac",    DC_Frac, "%", &m_MassFrac,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_MoleFrac);
  DDB.Double("", "MoleFrac",    DC_Frac, "%", &m_MoleFrac,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_VolumeFrac);
  DDB.Double("", "VolumeFrac",    DC_Frac, "%", &m_VolFrac,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_NVolumeFrac);
  DDB.Double("", "NVolumeFrac",    DC_Frac, "%", &m_NVolFrac,  this, isParm);
  DDB.Visibility(NSHM_All, m_eType==Type_Conc);
  DDB.Double("", "Concentration",  DC_Conc, "g/L", &m_Conc,  this, isParm);

  DDB.Visibility();
  DDB.Text("Measurement");
  DDB.Long       ("", "Selection",        DC_,  "", xidMkSelect,  this, isParm|SetOnChange, DDBSelect);
  //DDB.Text("Selection");
  if (DDB.ForFileSnpScn())
    {
    switch (m_eSelect)
      {
      case Slct_All:
      case Slct_Occur:
      case Slct_IndPhase:
        DDB.Long("", "PhaseMask",               DC_,  "", (long*)&m_Phases,     this, isParm|InitHidden);
        break;
      case Slct_Specie:
        {
        SetUpDDBSpcs();

        DDB.Long  ("NSpcs",       "", DC_,      "",   xidMkSpcCnt, this, isParm);
        if (DDB.BeginArray(this, "Spcs", "MU_SpIndex", m_Species.GetSize(), 0, DDB_NoPage, isParm))
          {
          for (int s=0; s<m_Species.GetSize(); s++)
            {
            DDB.BeginElement(this, s);
            DDB.Int("Spc", "", DC_, "", &m_Species[s], this, isParmConstruct, &SDB.DDBSpList);
            }
          };
        DDB.EndArray();
        break;
        }
      case Slct_Component:
        {
        SetUpDDBCmps();

        DDB.Long  ("NCmps",       "", DC_,      "",   xidMkCmpCnt, this, isParm);
        if (DDB.BeginArray(this, "Cmps", "MU_CmpIndex", m_Comps.GetSize(), 0, DDB_NoPage, isParm))
          {
          for (int s=0; s<m_Comps.GetSize(); s++)
            {
            DDB.BeginElement(this, s);
            DDB.Int("Cmp", "", DC_, "", &m_Comps[s], this, isParmConstruct, &CDB.DDBCompList);
            }
          };
        DDB.EndArray();
        break;
        }
      }  
    }
  else
    {
    switch (m_eSelect)
      {
      case Slct_All:
        break;
      case Slct_Occur:
        {
        DDB.CheckBoxBtn("", "Solids",           DC_,  "", xidMkSolids,  this, isParm);
        DDB.CheckBoxBtn("", "Liquids",          DC_,  "", xidMkLiquids, this, isParm);
        DDB.CheckBoxBtn("", "Gasses",           DC_,  "", xidMkGasses,  this, isParm);
        break;
        }
      case Slct_IndPhase:
        {
        for (int o=CDB.PhaseFirst(BOT_Solid); o<=CDB.PhaseLast(BOT_Gas); o++)
          {
          CPhaseInfo & P=CDB.PhaseInfo(o);
          Strng T,S;
          T.Set("(%s)", P.m_Tag());
          S.Set("(%s)", P.m_Sym());
          DDB.CheckBoxBtn(T(), S(), DC_,  "", xidMkPhase+o,  this, isParm);
          }
        break;
        }
      case Slct_Specie:
        {
        SetUpDDBSpcs();
        for (int s=0; s<=m_nLastSpcStr; s++)
          DDB.Text((LPTSTR)(LPCTSTR)m_SpcStr[s]);
        DDB.Long  ("Add",    "", DC_, "", xidMkAddSpc, this, (m_DDBSpcAdd.Length()>1?isParm:0)|SetOnChange, &m_DDBSpcAdd);
        DDB.Long  ("Remove", "", DC_, "", xidMkRemSpc, this, (m_DDBSpcRem.Length()>1?isParm:0)|SetOnChange, &m_DDBSpcRem);
        break;
        }
      case Slct_Component:
        {
        SetUpDDBCmps();
        for (int s=0; s<=m_nLastCmpStr; s++)
          DDB.Text((LPTSTR)(LPCTSTR)m_CmpStr[s]);
        DDB.Long  ("Add",    "", DC_, "", xidMkAddCmp, this, (m_DDBCmpAdd.Length()>1?isParm:0)|SetOnChange, &m_DDBCmpAdd);
        DDB.Long  ("Remove", "", DC_, "", xidMkRemCmp, this, (m_DDBCmpRem.Length()>1?isParm:0)|SetOnChange, &m_DDBCmpRem);
        break;
        }
      }  
    }

  DDB.Visibility();
  DDB.Text(" ");
  DDB.Text("MakeUp");
  DDB.Long       ("", "Source",           DC_,  "", (long*)&m_eSource,  this, isParm|SetOnChange, DDBSource);
  DDB.Double     ("QmMin", "",            DC_Qm, "kg/s", &m_QmMin, this, isParm);
  DDB.Double     ("QmMax", "",            DC_Qm, "kg/s", &m_QmMax, this, isParm);

  static DDBValueLst DDBTemp[] = 
    {
      {Temp_Inlet,   "InletTemp"},
      {Temp_Source,  "SourceTemp"},
      {Temp_Std,     "StdTemp"},
      {Temp_Const,   "Const"},
      {Temp_Mixture, "Mixture"},  // Needs work - a solve loop for volume
      {0}
    };

  if (!HeatSkipMethod())
    {
    DDB.Text(" ");
    DDB.Long  ("Temp.Final",      "",  DC_,  "", (long*)&m_eRqdTemp, this, isParm/*|DDEF_WRITEPROTECT*/, DDBTemp);
    DDB.Visibility(NSHM_All, m_eRqdTemp==Temp_Const);
    DDB.Double("Temp.Reqd",        "",  DC_T, "C", &m_RqdTemp, this, isParm/*|DDEF_WRITEPROTECT*/);
    DDB.Visibility();
    }

  DDB.Text(" ");
  DDB.Text("Results");

  DDB.Text("Simple Control:");
  DDB.String("Error",            "", DC_,   "",    xidMkError,     this, isResult);

  CCnvIndex CnvUsed;
  Strng CnvTxt;
  switch (m_eType)
    {                         
    case Type_MassFlow    : CnvUsed=DC_Qm; CnvTxt="kg/s"; break;
    case Type_MoleFlow    : CnvUsed=DC_QKgMl; CnvTxt="kmol/s"; break;
    case Type_VolumeFlow  : CnvUsed=DC_Qv; CnvTxt="m^3/s"; break;
    case Type_NVolumeFlow : CnvUsed=DC_NQv; CnvTxt="Nm^3/s"; break;
    case Type_Conc        : CnvUsed=DC_Conc; CnvTxt="g/L"; break;
    default               : CnvUsed=DC_Frac; CnvTxt="%"; break;
    }
  DDB.Visibility(NSHM_All, FracBasis || FlowBasis || m_eType==Type_Conc);
  DDB.Double ("Meas.Feed",        "", CnvUsed, CnvTxt(),  &m_dMeas,       this, isResult|noFileAtAll|NAN_OK);
  DDB.Visibility();
  DDB.Double ("Meas.SetPoint",    "", CnvUsed, CnvTxt(),  &m_dSetPoint,   this, isResult|noFileAtAll|NAN_OK);
  DDB.Double ("Meas.Prod",        "", CnvUsed, CnvTxt(),  &m_dResult,     this, isResult|noFileAtAll|NAN_OK);

  if (MassBasis)
    m_MeasDesc = "Mass flow";
  else if (VolBasis)
    m_MeasDesc = "Volume flow";
  else if (NVolBasis)
    m_MeasDesc = "NormVolume flow";
  else if (MoleBasis)
    m_MeasDesc = "Molar flow";
  else
    m_MeasDesc = "";

  switch (m_eSelect)
    {
    case Slct_All:
      m_MeasDesc += " of total stream:"; 
      break;
    case Slct_Occur:
      {
      m_MeasDesc += " of sum of phases (";
      if (m_Phases & som_Sol) m_MeasDesc += "solids ";
      if (m_Phases & som_Liq) m_MeasDesc += "liquids ";
      if (m_Phases & som_Gas) m_MeasDesc += "vapours ";
      m_MeasDesc.RTrim();
      m_MeasDesc += "):";
      break;
    case Slct_IndPhase:
      {
      m_MeasDesc += " of sum of individual phases (";
      for (int o=CDB.PhaseFirst(BOT_Solid); o<=CDB.PhaseLast(BOT_Gas); o++)
        {
        CPhaseInfo & P=CDB.PhaseInfo(o);
        if (m_Phases&P.m_PhMsk)
          {
          m_MeasDesc += P.m_Sym();
          m_MeasDesc += ' ';
          }
        }
      m_MeasDesc.RTrim();
      m_MeasDesc += "):";
      break;
      }
    case Slct_Specie:
      {
      m_MeasDesc += " of sum of selected species:"; 
      break;
      }
    case Slct_Component:
      {
      m_MeasDesc += " of sum of selected components:"; 
      break;
      }
      }  
    }

  DDB.Text(" ");
  DDB.Text(m_MeasDesc());
  if (MassBasis)
    {
    DDB.Double ("Meas.Qm.Feed",   "", DC_Qm,    "kg/s",     &m_dFeedAct,     this, isResult|noFileAtAll|NAN_OK);
    DDB.Double ("Meas.Qm.Prod",   "", DC_Qm,    "kg/s",     &m_dProdAct,     this, isResult|noFileAtAll|NAN_OK);
    }
  else if (VolBasis)
    {
    DDB.Double ("Meas.Qv.Feed",   "", DC_Qv,    "m^3/s",    &m_dFeedAct,     this, isResult|noFileAtAll|NAN_OK);
    DDB.Double ("Meas.Qv.Prod",   "", DC_Qv,    "m^3/s",    &m_dProdAct,     this, isResult|noFileAtAll|NAN_OK);
    }
  else if (NVolBasis)
    {
    DDB.Double ("Meas.NQv.Feed",  "", DC_NQv,   "Nm^3/s",   &m_dFeedAct,     this, isResult|noFileAtAll|NAN_OK);
    DDB.Double ("Meas.NQv.Prod",  "", DC_NQv,   "Nm^3/s",   &m_dProdAct,     this, isResult|noFileAtAll|NAN_OK);
    }
  else if (MoleBasis)
    {
    DDB.Double ("Meas.QMl.Feed",  "", DC_QKgMl, "kmol/s",   &m_dFeedAct,     this, isResult|noFileAtAll|NAN_OK);
    DDB.Double ("Meas.QMl.Prod",  "", DC_QKgMl, "kmol/s",   &m_dProdAct,     this, isResult|noFileAtAll|NAN_OK);
    }

  DDB.Text(" ");
  DDB.Text("Total mass flow:");
  DDB.Double ("Qm.Feed",            "", DC_Qm,    "kg/s",     &m_dQmFeed,     this, isResult);
  DDB.Double ("Qm.Makeup",          "", DC_Qm,    "kg/s",     &m_dQmMakeup,   this, isResult);
  DDB.Double ("Qm.Prod",            "", DC_Qm,    "kg/s",     &m_dQmProd,     this, isResult);
  if (!HeatSkipMethod())
    {
    DDB.Text("Total heat flow:");
    DDB.Double ("Temp.Feed",          "", DC_T,    "C",       &m_dTempKFeed,  this, isResult);//|noFileAtAll);
    DDB.Double ("HeatFlow",           "", DC_Pwr,  "kW",      &m_dHeatFlow,   this, isResult);
    DDB.Double ("Temp.Prod",          "", DC_T,    "C",       &m_dTempKProd,  this, isResult);//|noFileAtAll);
    }
  };

//--------------------------------------------------------------------------

void CXBlk_MUSimple::ClrMeasEtc()
  {
  m_dMeas     = dNAN;
  m_dSetPoint = dNAN;
  m_dResult   = dNAN;
  m_dFeedAct  = dNAN;
  m_dProdAct  = dNAN;
  }

flag CXBlk_MUSimple::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidMkType:
      if (DCB.rL && (m_eType!=(eType)*DCB.rL))
        {          
        m_eType     = (eType)*DCB.rL;
        ClrMeasEtc();
        }
      DCB.L = m_eType; 
      return 1;
    case xidMkSelect:
      if (DCB.rL && (m_eSelect!=(eSelect)*DCB.rL))
        {          
        m_eSelect   = (eSelect)*DCB.rL;
        ClrMeasEtc();
        }
      DCB.L = m_eSelect; 
      return 1;
    case xidMkAll:
      if (DCB.rB)
        {
        m_Phases = *DCB.rB ? som_ALL:0;
        ClrMeasEtc();
        }
      DCB.B = (m_Phases==som_ALL) ? 1 : (m_Phases==0) ? 0 : 2; 
      return 1;
    case xidMkSolids:
      if (DCB.rB)
        {
        m_Phases = (m_Phases&~som_Sol) | (*DCB.rB ? som_Sol:0);
        ClrMeasEtc();
        }
      DCB.B = ((m_Phases&som_Sol)==som_Sol)? 1 : ((m_Phases&som_Sol)==0) ? 0 : 2; 
      return 1;
    case xidMkLiquids:
      if (DCB.rB)
        {
        m_Phases = (m_Phases&~som_Liq) | (*DCB.rB ? som_Liq:0);
        ClrMeasEtc();
        }
      DCB.B = ((m_Phases&som_Liq)==som_Liq)? 1 : ((m_Phases&som_Liq)==0) ? 0 : 2; 
      return 1;
    case xidMkGasses:
      if (DCB.rB)
        {
        m_Phases = (m_Phases&~som_Gas) | (*DCB.rB ? som_Gas:0);
        ClrMeasEtc();
        }
      DCB.B = ((m_Phases&som_Gas)==som_Gas)? 1 : ((m_Phases&som_Gas)==0) ? 0 : 2; 
      return 1;

    case xidMkSpcCnt:
      if (DCB.rL)
        {
        m_Species.SetSize(*DCB.rL);
        m_DDBSpcsOK = false;
        ClrMeasEtc();
        }
      DCB.L=m_Species.GetSize();
      return 1;
    case xidMkAddSpc:
      if (DCB.rL)
        {
        if (*DCB.rL>=0)
          {
          for (int j=0; j<m_Species.GetSize(); j++)
            {
            if (m_Species[j]>*DCB.rL)
              {
              m_Species.InsertAt(j, *DCB.rL);
              m_DDBSpcsOK=false;
              DCB.L=-1;
              ClrMeasEtc();
              return 1;
              }
            }
          m_Species.Add(*DCB.rL);
          m_DDBSpcsOK=false;
          ClrMeasEtc();
          }
        else if (*DCB.rL==-2)
          {
          m_Species.SetSize(0);
          for (int i=0; i<SDB.Count(); i++)
            m_Species.Add(i);
          m_DDBSpcsOK=false;
          ClrMeasEtc();
          }
        }
      DCB.L=-1;
      return 1;
    case xidMkRemSpc:
      if (DCB.rL)
        {
        if (*DCB.rL>=0)
          {
          for (int j=0; j<m_Species.GetSize(); j++)
            {
            if (m_Species[j]==*DCB.rL)
              {
              m_Species.RemoveAt(j);
              m_DDBSpcsOK=false;
              ClrMeasEtc();
              break;
              }
            }
          }
        else if (*DCB.rL==-2)
          {
          m_Species.SetSize(0);
          m_DDBSpcsOK=false;
          ClrMeasEtc();
          }
        }
      DCB.L=-1;
      return 1;

    case xidMkCmpCnt:
      if (DCB.rL)
        {
        m_Comps.SetSize(*DCB.rL);
        m_DDBCmpsOK = false;
        ClrMeasEtc();
        }
      DCB.L=m_Comps.GetSize();
      return 1;
    case xidMkAddCmp:
      if (DCB.rL)
        {
        if (*DCB.rL>=0)
          {
          for (int j=0; j<m_Comps.GetSize(); j++)
            {
            if (m_Comps[j]>*DCB.rL)
              {
              m_Comps.InsertAt(j, *DCB.rL);
              m_DDBCmpsOK=false;
              DCB.L=-1;
              ClrMeasEtc();
              return 1;
              }
            }
          m_Comps.Add(*DCB.rL);
          m_DDBCmpsOK=false;
          ClrMeasEtc();
          }
        else if (*DCB.rL==-2)
          {
          m_Comps.SetSize(0);
          for (int i=0; i<CDB.No(); i++)
            m_Comps.Add(i);
          m_DDBCmpsOK=false;
          ClrMeasEtc();
          }
        }
      DCB.L=-1;
      return 1;
    case xidMkRemCmp:
      if (DCB.rL)
        {
        if (*DCB.rL>=0)
          {
          for (int j=0; j<m_Comps.GetSize(); j++)
            {
            if (m_Comps[j]==*DCB.rL)
              {
              m_Comps.RemoveAt(j);
              m_DDBCmpsOK=false;
              ClrMeasEtc();
              break;
              }
            }
          }
        else if (*DCB.rL==-2)
          {
          m_Comps.SetSize(0);
          m_DDBCmpsOK=false;
          ClrMeasEtc();
          }
        }
      DCB.L=-1;
      return 1;

    case xidMkError:
      {
      m_ErrorLst.Clear();
      ConditionBlk::GetMyCIs(m_ErrorLst,3);
      if (!SrcIO.Connected)
        {
        DCB.pC="Not Connected";
        }
      else if (!m_bHasFlow)
        {
        DCB.pC="No Flow";
        }
      else if (m_ErrorLst.Length()>0)
        {
        Strng &S=*m_ErrorLst.First();
        DCB.pC=S.XStrChr('\t')+1;
        }
      else
        DCB.pC="None";
      return 1;
      }
    default:
      if (DCB.lHandle>=xidMkPhase && DCB.lHandle<xidMkPhase+CDB.PhaseCount())
        {
        int o=DCB.lHandle-xidMkPhase;
        CPhaseInfo & P=CDB.PhaseInfo(o);
        if (DCB.rB)
          {
          m_Phases = (m_Phases&~P.m_PhMsk) | (*DCB.rB ? P.m_PhMsk:0);
          ClrMeasEtc();
          }
        DCB.B = (m_Phases&P.m_PhMsk) ? 1 : 0; 
        return 1;
        }
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag CXBlk_MUSimple::ValidateData(ValidateDataBlk & VDB)
  {
  switch (m_eSelect)
    {
    case Slct_All:
      {
      m_Phases=som_ALL;
      break;
      }
    case Slct_Specie:
      {
      break;
      }
    case Slct_Component:
      {
      m_Species.SetSize(0);
      for (int i=0; i<m_Comps.GetCount(); i++)
        {
        for (int j=0; j<CDB[i].NSpecies(); j++)
          {
          int s=CDB[i].iSpecie(j);
          m_Species.Add(s);
          }
        }
      break;
      }
    default:
      break;
    }

  if (m_Phases==som_ALL)
    {
    switch (m_eType)
      {                         
      case Type_MassFrac:
      case Type_VolumeFrac:
      case Type_NVolumeFrac:
        LogError(Tag(), 0, "Fraction Type Invalid");
        break;
      }
    }

  return CMakeupBlock::ValidateData(VDB); 
  }

//--------------------------------------------------------------------------

double CXBlk_MUSimple::GetSetPoint()
  {
  switch (m_eType)
    {                         
    case Type_MassFlow:       return m_QmRqd;
    case Type_MassRatio:      return m_QmRatio;
    case Type_MassMult:       return m_QmMult;
    case Type_MassFrac:       return m_MassFrac;
    case Type_MoleFlow:       return m_QMlRqd;
    case Type_MoleRatio:      return m_QMlRatio;
    case Type_MoleMult:       return m_QMlMult;
    case Type_MoleFrac:       return m_MoleFrac;
    case Type_VolumeFlow:     return m_QvRqd;
    case Type_VolumeRatio:    return m_QvRatio;
    case Type_VolumeMult:     return m_QvMult;
    case Type_VolumeFrac:     return m_VolFrac;
    case Type_NVolumeFlow:    return m_NQvRqd;
    case Type_NVolumeRatio:   return m_NQvRatio;
    case Type_NVolumeMult:    return m_NQvMult;
    case Type_NVolumeFrac:    return m_NVolFrac;
    case Type_Conc:           return m_Conc;
    };
  return dNAN;
  }

//--------------------------------------------------------------------------

double CXBlk_MUSimple::GetRawMeas(SpConduit &QPrd, PhMask PhRqd)
  {
  switch (m_eType)
    {                         
    case Type_MassFlow:
    case Type_MassRatio:
    case Type_MassMult: 
    case Type_MassFrac:
      if (PhRqd)
        return QPrd.QMass(PhRqd);
      if (m_eSelect>=Slct_Specie)
        return QPrd.QMass(m_Species);
      return QPrd.QMass(m_Phases);

    case Type_MoleFlow:
    case Type_MoleRatio:
    case Type_MoleMult: 
    case Type_MoleFrac:
      if (PhRqd)
        return QPrd.QMole(PhRqd);
      if (m_eSelect>=Slct_Specie)
        return QPrd.QMole(m_Species);
      return QPrd.QMole(m_Phases);

    case Type_VolumeFlow:
    case Type_VolumeRatio:
    case Type_VolumeMult: 
    case Type_VolumeFrac:
      if (PhRqd)
        return QPrd.QVolume(PhRqd);
      if (m_eSelect>=Slct_Specie)
        return QPrd.QVolume(m_Species);
      return QPrd.QVolume(m_Phases);

    case Type_NVolumeFlow:
    case Type_NVolumeRatio:
    case Type_NVolumeMult: 
    case Type_NVolumeFrac:
      if (PhRqd)
        return QPrd.QNVolume(PhRqd);
      if (m_eSelect>=Slct_Specie)
        return QPrd.QNVolume(m_Species);
      return QPrd.QNVolume(m_Phases);

    case Type_Conc:
      if (m_eSelect>=Slct_Specie)
        return QPrd.QMass(m_Species);
      return QPrd.QMass(m_Phases);
    }
  return dNAN;
  }

double CXBlk_MUSimple::GetMeasVal(SpConduit &QIn, SpConduit &QPrd)
  {
  switch (m_eType)
    {                         
    case Type_MassFlow:
    case Type_MoleFlow:
    case Type_VolumeFlow:
    case Type_NVolumeFlow:
      return GetRawMeas(QPrd);

    case Type_MassRatio:
    case Type_MoleRatio:
    case Type_VolumeRatio:
    case Type_NVolumeRatio:
      return GetRawMeas(QPrd)/GTZ(GetRawMeas(QIn))-1.0;

    case Type_MassMult: 
    case Type_MoleMult: 
    case Type_VolumeMult: 
    case Type_NVolumeMult: 
      return GetRawMeas(QPrd)/GTZ(GetRawMeas(QIn));

    case Type_MassFrac:
    case Type_MoleFrac:
    case Type_VolumeFrac:
    case Type_NVolumeFrac:
      return GetRawMeas(QPrd)/GTZ(GetRawMeas(QPrd, som_ALL));

    case Type_Conc:
      return GetRawMeas(QPrd)/GTZ(QPrd.QVolume());
    }
  return 0.0;
  }

//--------------------------------------------------------------------------

double CXBlk_MUSimple::GetFlowValue(SpConduit &QPrd, PhMask PhRqd)
  {
  switch (m_eType)
    {                         
    case Type_MassFlow:
    case Type_MassRatio:
    case Type_MassMult: 
    case Type_MassFrac:
      if (PhRqd)
        return QPrd.QMass(PhRqd);
      if (m_eSelect>=Slct_Specie)
        return QPrd.QMass(m_Species);
      return QPrd.QMass(m_Phases);

    case Type_MoleFlow:
    case Type_MoleRatio:
    case Type_MoleMult: 
    case Type_MoleFrac:
      if (PhRqd)
        return QPrd.QMole(PhRqd);
      if (m_eSelect>=Slct_Specie)
        return QPrd.QMole(m_Species);
      return QPrd.QMole(m_Phases);

    case Type_VolumeFlow:
    case Type_VolumeRatio:
    case Type_VolumeMult: 
    case Type_VolumeFrac:
      if (PhRqd)
        return QPrd.QVolume(PhRqd);
      if (m_eSelect>=Slct_Specie)
        return QPrd.QVolume(m_Species);
      return QPrd.QVolume(m_Phases);

    case Type_NVolumeFlow:
    case Type_NVolumeRatio:
    case Type_NVolumeMult: 
    case Type_NVolumeFrac:
      if (PhRqd)
        return QPrd.QNVolume(PhRqd);
      if (m_eSelect>=Slct_Specie)
        return QPrd.QNVolume(m_Species);
      return QPrd.QNVolume(m_Phases);

    case Type_Conc:
      if (m_eSelect>=Slct_Specie)
        return QPrd.QMass(m_Species);
      return QPrd.QMass(m_Phases);
    }
  return dNAN;
  }
//-------------------------------------------------------------------------

class CSimpleMkUpFnd : public MRootFinder
  {
  public:
    CSimpleMkUpFnd(CXBlk_MUSimple * pMU, LPCTSTR pTag, SpConduit * pIn, SpConduit * pSrc, SpConduit * pPrd, double TRqd, double PRqd, CToleranceBlock & Tol) : \
      m_pMU(pMU), m_pTag(pTag), m_In(*pIn), m_Src(*pSrc), m_Prd(*pPrd), m_TRqd(TRqd), m_PRqd(PRqd), MRootFinder("MkUpFnd", Tol)
      { 
      };
    LPCTSTR ObjTag() { return m_pTag; };
    double Function(double Qm)
      {
      m_Prd.QCopy(m_In);
      m_Prd.QAddM(m_Src, som_ALL, Qm);
      m_Prd.SetTempPress(m_TRqd, m_PRqd);
      dbgpln("   Converge Qm:%20.6f Meas:%20.6f SetPt:%20.6f In:%20.6f Src:%20.6f", Qm, m_pMU->GetMeasVal(m_In, m_Prd), m_pMU->GetSetPoint(), m_In.QMass(), m_Src.QMass());
      return m_pMU->GetMeasVal(m_In, m_Prd);
      };

  protected:
    CXBlk_MUSimple  * m_pMU;
    LPCTSTR         m_pTag;
    SpConduit     & m_In;
    SpConduit     & m_Src;
    SpConduit     & m_Prd;
    double          m_TRqd;
    double          m_PRqd;
  };

//--------------------------------------------------------------------------

void CXBlk_MUSimple::EvalProducts(SpConduit &QPrd, double Po, double FinalTEst)
  {
  FlwNode *pNd=FindObjOfType((FlwNode*)NULL);
  ASSERT_ALWAYS(pNd, "Should always be part of a FlwNode");

  if (QPrd.QMass()>SmallPosFlow)
    {
    m_bHasFlow = true;

    m_dQmFeed = QPrd.QMass();
    m_dTempKFeed = QPrd.Temp();
    const double HzIn = QPrd.totHz();

    StkSpConduit QIn("QIn", "MkUp", pNd);
    QIn().QCopy(QPrd);

    m_dMeas     = GetMeasVal(QIn(), QPrd);
    m_dFeedAct  = GetFlowValue(QIn());

    bool CIsOn[7]={false,false,false,false,false,false,false};
    if (m_eSelect>=Slct_Specie)
      CIsOn[6]=(m_Species.GetCount()==0);
    else 
      CIsOn[5]=(m_Phases==0);

    if (!CIsOn[5] && !CIsOn[6])
      {

      SpConduit &QSrc=SrcIO.Cd;

      // Copy to Src if Self
      if (m_eSource==Src_Self)
        QSrc.QSetF(QPrd, som_ALL, 1.0);

      double TReqd;
      switch (m_eRqdTemp)
        {
        case Temp_Inlet:
          TReqd=QPrd.Temp();
          break;
        case Temp_Source:
          TReqd=QSrc.Temp();
          break;
        case Temp_Std:
          TReqd=StdT;
          break;
        case Temp_Mixture:
          TReqd=StdT; //??????
          break;
        default:
          TReqd=QPrd.Temp();
        }

      CSimpleMkUpFnd MkUpFnd(this, BaseTag(), &QIn(), &QSrc, &QPrd, TReqd, Po, sm_QmTol);
      //int iRet=MkUpFnd.FindRootEst(GetSetPoint(), m_QmMin, m_QmMax, m_dQmMakeup, 0.0);
      int iRet=MkUpFnd.FindRoot(GetSetPoint(), m_QmMin, m_QmMax, m_dQmMakeup, 0.0);
      switch (iRet)
        {
        case RF_OK:         
          m_dQmMakeup = MkUpFnd.Result();
          break;
        case RF_LoLimit:    
        case RF_EstimateLoLimit:    
          m_dQmMakeup = MkUpFnd.Result();
          CIsOn[2]=true;
          break;
        case RF_HiLimit:    
        case RF_EstimateHiLimit:    
          m_dQmMakeup = MkUpFnd.Result();   
          CIsOn[3]=true;
          break;
        case RF_Independant:
          MkUpFnd.Function(0);   
          m_dQmMakeup = MkUpFnd.Result();   
          CIsOn[4]=true;
          break;

        default: 
          CIsOn[1]=true;
          SetCI(1, "E\tConverge Error [%i]", iRet);
          break;
        }

      QSrc.QAdjustQmTo(som_ALL, m_dQmMakeup);

      if (SrcIO.Enabled)
        SrcIO.Sum.Set(QSrc);
      else
        SrcIO.Sum.ZeroFlows();
      }

    if (!CIsOn[1])
      ClrCI(1);

    for (int i=2; i<=6; i++)
      SetCI(i, CIsOn[i]);

    m_dSetPoint   = GetSetPoint();
    m_dResult     = GetMeasVal(QIn(), QPrd);
    m_dQmProd     = QPrd.QMass();
    m_dQmMakeup   = m_dQmProd-m_dQmFeed;
    m_dTempKProd  = QPrd.Temp();
    m_dHeatFlow   = QPrd.totHz() - HzIn;

    m_dProdAct    = GetFlowValue(QPrd);
    }
  else
    {
    m_bHasFlow    = false;
    m_dQmFeed     = QPrd.QMass();
    m_dQmMakeup   = 0;
    m_dFeedAct    = GetFlowValue(QPrd);
    m_dProdAct    = GetFlowValue(QPrd);
    m_dMeas       = dNAN;
    m_dSetPoint   = GetSetPoint();
    m_dResult     = dNAN;
    m_dQmProd     = 0;
    m_dQmMakeup   = 0;
    m_dTempKProd  = QPrd.Temp();
    m_dHeatFlow   = 0;
    ClrCI(1);
    ClrCI(2);
    ClrCI(3);
    ClrCI(4);
    SrcIO.Cd.QZero();
    SrcIO.Sum.ZeroFlows();
    }

  if (SrcIO.MyConnectedIO()>=0)
    pNd->SetIOQm_In(SrcIO.MyConnectedIO(), m_dQmMakeup);

  };

void CXBlk_MUSimple::EvalProductsPipe(SpConduit & Qf, double Len, double Diam, double Po, double FinalTEst)
  {
  EvalProducts(Qf, Po, FinalTEst);
  };

//--------------------------------------------------------------------------

flag CXBlk_MUSimple::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\tConverge Error"; return 1;
    case  2: pS="E\tRequirement not Achieved - Low Limit"; return 1;
    case  3: pS="E\tRequirement not Achieved - High Limit"; return 1;
    case  4: pS="E\tMakeup has No Effect"; return 1;
    case  5: pS="E\tNo Phase Selected for Measurement"; return 1;
    case  6: pS="E\tNo Species Selected for Measurement"; return 1;
    default:
      return CXBlk_MUSimple::CIStrng(No, pS);
    }
  }

//=========================================================================
//
//
//
//=========================================================================
