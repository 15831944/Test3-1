#include "stdafx.h"

#define __MAKEUPBLOCK_CPP
#include "MakeupBlock.h"
#include "nrecipe.h"

//#include "optoff.h"

#define dbgMakeup  0

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
  SrcIO.Cd.QZero();
  SrcIO.Sum.ZeroFlows();
  };

void CMakeupBlock::EvalProductsInline(SpConduit & Qf, double Len, double Diam, double Po, double FinalTEst)
  {
  SrcIO.Cd.QZero();
  SrcIO.Sum.ZeroFlows();
  };

//============================================================================
//
//
//
//============================================================================

CMakeupBase::CMakeupBase(TaggedObject * pAttach, int Index, LPTSTR Name) : CBlockEvalBase(BEId_Makeup, Index, Name, false),
m_SrcIO(eDIO_Makeup, dynamic_cast<FlwNode*>(pAttach), false, false,
        Name, IOId_Makeup2Area+Index, IOId_AreaMakeupO, "MakeupSrc", "")//MakeupSrc_1")
  {
  m_pMakeupB        = NULL;
  m_pNd             = dynamic_cast<FlwNode*>(pAttach);
  m_fEnabled        = false;
  m_fFixed          = false;
  m_SrcIO.UsrEnable = true;//false;
  m_ConfigChecked   = PrjFileVerNo()>=114;
  m_On              = true;
  //m_Index           = Index;
  }

CMakeupBase::~CMakeupBase()
  {
  delete m_pMakeupB;
  };

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
      if (1)//m_SrcIO.Enabled)
        {
        if (PrjFileVerNo()<107)
          {
          m_SrcIO.BuildDataDefn(DDB, tt_Object, "DIO", DDB_NoPage, UserInfo+102, 0);//DFIO_ShowQm);
          }
        else if (!DDB.ForFileSnpScn())// || PrjFileVerNo()<107))
          {
          //m_SrcIO.BuildDataDefn(DDB, NULL, DDB_NoPage, UserInfo+102, 0);//DFIO_ShowQm);
          m_SrcIO.BuildDataDefn(DDB, tt_Struct, "DIO", DDB_NoPage, UserInfo+102, 0);//DFIO_ShowQm);
          }
        }

      DDB.Text("Requirements");
      DDB.CheckBoxBtn("On",              "",       DC_    , "",      &m_On, m_pNd, isParm);
      //if (OrigPrjFileVerNo()<114)
      if (!m_ConfigChecked)
        DDB.CheckBoxBtn("ConfigChecked",   "",       DC_    , "",      &m_ConfigChecked, m_pNd, isParm);

      DDBValueLstMem DDB0;
      TagObjClass::GetSDescValueLst(CMakeupBlock::GroupName, DDB0);
      DDB.String  ("Model",      "",       DC_    , "",      xidMakeupMdlNm  , m_pNd, m_fFixed ? 0 : isParmStopped|SetOnChange, DDB0());

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
        m_pNd->DoDirectDisConnect(&m_SrcIO);
        dbgpln("------------xidMakeupEnable");
        if (*DCB.rB)
          {
          Open(*DCB.rB);
          m_pMakeupB->SetTag(Name());
          SetEnable(true);
          }
        else
          {
          Close();
          SetEnable(false);
          }
        if (!DCB.ForFiling()) // do not try connect during load
          m_pNd->DoDirectConnect(&m_SrcIO);
        }
      DCB.B=OpenStatus();// (Enabled());
      return 1;
    case xidMakeupMdlNm:
      if (DCB.rpC && !m_fFixed)
        {
        m_pNd->DoDirectDisConnect(&m_SrcIO);
        dbgpln("------------xidMakeupMdlNm");
        int WasEnabled=m_fEnabled;
        TagObjClass * pC=CMakeupBlockClass.FindGrpShortDesc(DCB.rpC);
        if (pC)
          {
          Open(pC);
          m_pMakeupB->SetTag(Name());
          }
        else
          {
          }
        m_SrcIO.UsrEnable = m_pMakeupB ? m_pMakeupB->DoesSomething() && Enabled() : false;
        if (!DCB.ForFiling()) // do not try connect during load
          m_pNd->DoDirectConnect(&m_SrcIO);
        }
      DCB.pC = m_pMakeupB ? m_pMakeupB->ShortDesc() : "";
      return 1;
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag CMakeupBase::ValidateData(ValidateDataBlk & VDB)
  {
  if (!Enabled())
    return 1;
  if (!m_ConfigChecked)
    {
    m_pNd->SetValidateFailMsg("E\tMakeup Configuration needs checking");
    return 0;
    }
  return m_pMakeupB->ValidateData(VDB);
  };

//--------------------------------------------------------------------------

int CMakeupBase::ChangeTag(char * pOldTag, char * pNewTag)
  {
  if (m_SrcIO.Target() && stricmp(m_SrcIO.Target(), pOldTag)==0)
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
XID xidMkQmMin   = AdjustXID(1001);
XID xidMkPhaseB  = AdjustXID(1002);
XID xidMkSelect  = AdjustXID(1003);
XID xidMkAll     = AdjustXID(1004);
XID xidMkSolids  = AdjustXID(1005);
XID xidMkLiquids = AdjustXID(1006);
XID xidMkGasses  = AdjustXID(1007);
XID xidMkSpcCnt  = AdjustXID(1008);
XID xidMkAddSpc  = AdjustXID(1009);
XID xidMkRemSpc  = AdjustXID(1010);
XID xidMkCmpCnt  = AdjustXID(1011);
XID xidMkAddCmp  = AdjustXID(1012);
XID xidMkRemCmp  = AdjustXID(1013);
XID xidMkElemCnt = AdjustXID(1014);
XID xidMkAddElem = AdjustXID(1015);
XID xidMkRemElem = AdjustXID(1016);
XID xidMkState   = AdjustXID(1017);
XID xidMkDesc    = AdjustXID(1018);
XID xidMkPhase   = AdjustXID(1200);

XID xidMkSpcId   = AdjustXID(1020);
XID xidMkCmpId   = AdjustXID(1040);
XID xidMkElemId  = AdjustXID(1060);

//============================================================================
//
//
//
//============================================================================


const int MaxMUItems = 10;

class CDDBItemList
  {
  public:
    CDDBItemList (CIArray & List) : \
      m_List(List)
      {
      m_OK=false;
      }

    void SetUp(bool ForceIt=false)
      {
      if (!m_OK || ForceIt)
        {
        for (int j=0; j<m_List.GetSize(); j++)
          {
          if (m_List[j]<0)
            m_List.RemoveAt(j);
          }

        if (m_List.GetCount()>1)
          {
          //HpSort(m_List.GetCount(), (void**)&m_List[0], TestOrder);
          for (int i=1; i<m_List.GetCount(); i++)
            {
            for (int j=i-1; j>=0; j--)
              {
              if (m_List[j]>m_List[j+1])
                Exchange(m_List[j], m_List[j+1]);
              else
                break;
              }
            }
          }
        int InList[MaxSpeciesEx];
        for (int s=0; s<SDB.Count(); s++)
          InList[s]=-1;

        int iList=0;
        for (int j=0; j<m_List.GetSize(); j++)
          {
          if (InList[m_List[j]]<0)
            InList[m_List[j]]=iList++;
          }

        for (int j=0; j<m_List.GetCount(); j++)
          {
          m_Values[j].Empty();
          m_Values[j].Add(-1, "-");
          for (int s=0; s<m_AllValues.Length(); s++)
            {
            if (s==m_List[j] || InList[s]<0)
              m_Values[j].Add(m_AllValues[s].m_lVal,m_AllValues[s].m_pStr);
            }
          }

        if (m_List.GetCount()<MaxMUItems)
          {
          int j=m_List.GetCount();
          m_Values[j].Empty();
          m_Values[j].Add(-1, "-");
          for (int s=0; s<m_AllValues.Length(); s++)
            {
            if (InList[s]<0)
              m_Values[j].Add(m_AllValues[s].m_lVal,m_AllValues[s].m_pStr);
            }
          }
        //m_DDBSpcLst;
        m_OK=true;
        }
      }
  public:

    //bool            m_DDBSpcsOK;
    bool            m_OK;
    DDBValueLstMem  m_Values[MaxMUItems];
    DDBValueLstMem  m_AllValues;
    CIArray       & m_List;
  };


#define DllImportExport /* */


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
  LF_AlwaysOn,
  LF_StopBelow,
  //LF_HoldMeasure,
  };


enum eSelect
  {
  Slct_All,
  Slct_Occur,
  Slct_IndPhase,
  Slct_Specie,
  Slct_Component,
  Slct_Element,
  };

enum eTemp
  {
  Temp_Inlet,
  Temp_Source,
  Temp_Std,
  Temp_Const,
  Temp_Mixture,
  };

class DllImportExport CMeasInfo
  {
  friend class CXBlk_MUBase;
  public:

    CMeasInfo() : \
      m_DDBSpcs(m_Species),
      m_DDBCmps(m_Comps),
      m_DDBElems(m_Elements)
      {
      for (int s=0; s<SDB.Count(); s++)
        m_DDBSpcs.m_AllValues.Add(s, SDB[s].SymOrTag());
      for (int c=0; c<CDB.No(); c++)
        m_DDBCmps.m_AllValues.Add(c, CDB[c].SymOrTag());
      for (int e=0; e<EDB.Count(); e++)
        m_DDBElems.m_AllValues.Add(e, EDB[e].m_Name);

      m_UserInfo  = 0;

      m_eType     = Type_Mass;
      m_eSelect   = Slct_All;
      m_Phases    = som_ALL;
      };

    void Initialise(LPTSTR Tag, DWORD UserInfo, LPSTR Header, eType Type=Type_Mass, eSelect Select=Slct_All, PhMask Phases=som_ALL)
      {
      m_Tag       = Tag;
      m_Header    = Header;
      m_UserInfo  = UserInfo;
      m_eType     = Type;
      m_eSelect   = Select;
      m_Phases    = Phases;
      };

    void            BuildDataDefn(DataDefnBlk& DDB, CXBlk_MUBase &Blk);
    flag            DataXchg(DataChangeBlk & DCB, CXBlk_MUBase &Blk);
    flag            ValidateData(ValidateDataBlk & VDB, CXBlk_MUBase &Blk);
    LPSTR           Description(bool Hdr);

    LPSTR           CIStr(int Which)
      {
      switch (Which)
        {
        case  0: return m_CIStr.Set("E\tNo Phase Selected for %s", m_Tag());
        case  1: return m_CIStr.Set("E\tNo Species Selected for %s", m_Tag());
        case  2: return m_CIStr.Set("E\tNo Element Selected for %s", m_Tag());
        };
      return "???";
      }

  protected:

    eType           m_eType;
    eSelect         m_eSelect;
    PhMask          m_Phases;

    CIArray         m_Species;
    CIArray         m_Comps;
    CIArray         m_Elements;

    CDDBItemList    m_DDBSpcs;
    CDDBItemList    m_DDBCmps;
    CDDBItemList    m_DDBElems;

    Strng           m_sDesc;
    Strng           m_FullDesc;

    Strng           m_Tag;
    Strng           m_Header;
    DWORD           m_UserInfo;

    Strng           m_CIStr;
  };

//============================================================================
//
//
//
//============================================================================

class DllImportExport CXBlk_MUBase: public CMakeupBlock
  {
  public:
    enum eOps  { Op_MUFixed, Op_MURatio, Op_PrdFlow, Op_PrdComp, Op_PrdConc, Op_PrdTemp, Op_PrdTChg };

  public:
    CXBlk_MUBase(eOps Op, TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CXBlk_MUBase();

    void           ClrMeasEtc();

    virtual flag   DoesSomething() { return true; };

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void   EvalProductsInline(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

    virtual double  Duty() { return 0.0; };

    double         GetSetPoint();
    double         GetMeasVal(SpConduit &QFeed, SpConduit &QSrc, SpConduit &QProd);
    double         GetFlowValue(CMeasInfo &MI, SpConduit &Q, PhMask PhRqd=0);

    DEFINE_CI(CXBlk_MUBase, CMakeupBlock, 12);

  public:
    Strng           m_MyTag;
    eSource         m_eSource;
    double          m_MUQmMin;
    double          m_MUQmMax;
    //eType           m_eType;

    eOps            m_Op;

    eLoFeed         m_eLoFeedOpt;
    double          m_LoFeedQm;

    CMeasInfo       m_Meas1, m_Meas2;

    bool            m_bHasFlow;
    bool            m_bStopMakeUp;

    double          m_RqdSetPoint;

    //results
    double          m_SetPoint;
    double          m_Measured;
    double          m_Result;
    //double          m_FeedAct;
    //double          m_ProdAct;
    //double          m_MakeupAct;

    double          m_QmFeed;
    double          m_QmMkUp;
    double          m_QmProd;
    double          m_QvFeed;
    double          m_QvMkUp;
    double          m_QvProd;

    double          m_MeasFeed;
    double          m_MeasMkUp;
    double          m_MeasProd;

    eTemp           m_eRqdTemp;
    double          m_RqdTemp;

    double          m_TempFeed;
    double          m_TempMkUp;
    double          m_TempProd;
    double          m_HeatFlow;

    bool            m_ValidateOK;
    Strng/*_List*/      m_sDesc;
    Strng_List      m_ErrorLst;

    static CToleranceBlock sm_QmTol;

  };

//============================================================================
//
//
//
//============================================================================

static int TestOrder(void * p, void * q) { return *((int *)p)>*((int*)q); };

//--------------------------------------------------------------------------

void CMeasInfo::BuildDataDefn(DataDefnBlk& DDB, CXBlk_MUBase &Blk)//, LPTSTR Tag, DWORD UserInfo, LPSTR Header)
  {
  DDB.PushUserInfo(m_UserInfo);
  if (DDB.BeginStruct(&Blk, m_Tag(), NULL, DDB_NoPage))
    {
    if (m_Header())
      DDB.Text(m_Header());

    static DDBValueLst DDBSelect[] =
      {
        {Slct_All,            "All"             },
        {Slct_Occur,          "Phase"           },
        {Slct_IndPhase,       "IndividualPhase" },
        {Slct_Specie,         "Specie"          },
        {Slct_Component,      "Component"       },
        {Slct_Element,        "Element"         },
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
          //m_DDBSpcs.SetUp();

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
          //SetUpDDBCmps();

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
        case Slct_Element:
          {
          //SetUpDDBElems();

          DDB.Long  ("NElems",       "", DC_,      "",   xidMkElemCnt, &Blk, isParm);
          if (DDB.BeginArray(&Blk, "Elems", "MU_ElemIndex", m_Elements.GetSize(), 0, DDB_NoPage, isParm))
            {
            for (int s=0; s<m_Elements.GetSize(); s++)
              {
              DDB.BeginElement(&Blk, s);
              DDB.Int("Elem", "", DC_, "", &m_Elements[s], &Blk, isParmConstruct, &EDB.DDBElementList);
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
          Strng Tag;
          m_DDBSpcs.SetUp();
          for (long s=0; s<Min(MaxMUItems, m_Species.GetCount()+1); s++)
            {
            DDB.Long  (Tag.Set("Specie%02i",s+1),  "", DC_, "", xidMkSpcId+s, &Blk, isParm|SetOnChange, &m_DDBSpcs.m_Values[s]);
            m_DDBSpcs.SetUp();
            }
          break;
          }
        case Slct_Component:
          {
          Strng Tag;
          m_DDBCmps.SetUp();
          for (long s=0; s<Min(MaxMUItems, m_Comps.GetCount()+1); s++)
            {
            DDB.Long  (Tag.Set("Component%02i",s+1),  "", DC_, "", xidMkCmpId+s, &Blk, isParm|SetOnChange, &m_DDBCmps.m_Values[s]);
            m_DDBCmps.SetUp();
            }
          break;
          }
        case Slct_Element:
          {
          Strng Tag;
          m_DDBElems.SetUp();
          for (long s=0; s<Min(MaxMUItems, m_Elements.GetCount()+1); s++)
            {
            DDB.Long  (Tag.Set("Element%02i",s+1),  "", DC_, "", xidMkElemId+s, &Blk, isParm|SetOnChange, &m_DDBElems.m_Values[s]);
            m_DDBElems.SetUp();
            }
          break;
          }
        }
      }
    }
  DDB.EndStruct();
  DDB.PopUserInfo();
  };

//--------------------------------------------------------------------------

flag CMeasInfo::DataXchg(DataChangeBlk & DCB, CXBlk_MUBase &Blk)
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
        m_DDBSpcs.SetUp(true);
        Blk.ClrMeasEtc();
        }
      DCB.L=m_Species.GetSize();
      return 1;
    case xidMkCmpCnt:
      if (DCB.rL)
        {
        m_Comps.SetSize(*DCB.rL);
        m_DDBCmps.SetUp(true);
        Blk.ClrMeasEtc();
        }
      DCB.L=m_Comps.GetSize();
      return 1;

    case xidMkElemCnt:
      if (DCB.rL)
        {
        m_Elements.SetSize(*DCB.rL);
        m_DDBElems.SetUp(true);
        Blk.ClrMeasEtc();
        }
      DCB.L=m_Elements.GetSize();
      return 1;

    default:
      if (DCB.lHandle>=xidMkSpcId && DCB.lHandle<xidMkSpcId+MaxMUItems)
        {
        int i= DCB.lHandle-xidMkSpcId;
        if (DCB.rL)
          {
          m_Species.SetAtGrow(i, *DCB.rL);
          m_DDBSpcs.m_OK=false;
          //SetUpDDBSpcs(true);
          }
        DCB.L=i<m_Species.GetCount() ? m_Species[i]:-1;
        return 1;
        }
      else if (DCB.lHandle>=xidMkCmpId && DCB.lHandle<xidMkCmpId+MaxMUItems)
        {
        int i= DCB.lHandle-xidMkCmpId;
        if (DCB.rL)
          {
          m_Comps.SetAtGrow(i, *DCB.rL);
          m_DDBCmps.m_OK=false;
          }
        DCB.L=i<m_Comps.GetCount() ? m_Comps[i]:-1;
        return 1;
        }
      else if (DCB.lHandle>=xidMkElemId && DCB.lHandle<xidMkElemId+MaxMUItems)
        {
        int i= DCB.lHandle-xidMkElemId;
        if (DCB.rL)
          {
          m_Elements.SetAtGrow(i, *DCB.rL);
          m_DDBElems.m_OK=false;
          }
        DCB.L=i<m_Elements.GetCount() ? m_Elements[i]:-1;
        return 1;
        }
      else if (DCB.lHandle>=xidMkPhase && DCB.lHandle<xidMkPhase+CDB.PhaseCount())
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

flag CMeasInfo::ValidateData(ValidateDataBlk & VDB, CXBlk_MUBase &Blk)
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
        int c=m_Comps[i];
        for (int j=0; j<CDB[c].NSpecies(); j++)
          {
          int s=CDB[c].iSpecie(j);
          m_Species.Add(s);
          }
        }
      break;
      }
    case Slct_Element:
      {
      break;
      }
    default:
      break;
    }

  return true;
  }

//--------------------------------------------------------------------------

LPSTR CMeasInfo::Description(bool Hdr)
  {
  if (Hdr)
    {
    switch (m_eType)
      {
      case Type_Mass:     m_sDesc = "Mass";         break;
      case Type_Volume:   m_sDesc = "Volume";       break;
      case Type_NVolume:  m_sDesc = "NVolume";   break;
      case Type_Mole:     m_sDesc = "Molar";        break;
      default:            m_sDesc = "";                  break;
      }
    }
  else
    m_sDesc = "";

  switch (m_eSelect)
    {
    case Slct_All:
      if (m_sDesc())
        m_sDesc += ":";
      m_sDesc += "All";
      break;
    case Slct_Occur:
      {
      m_sDesc += "[";
      if (m_Phases & som_Sol) m_sDesc += "Sol";
      if (m_Phases & som_Liq) m_sDesc += (m_Phases & som_Sol) ? ",Liq":"Liq";
      if (m_Phases & som_Gas) m_sDesc += (m_Phases & som_SL) ? ",Vap":"Vap";
      m_sDesc.RTrim();
      m_sDesc += "]";
      break;
      }
    case Slct_IndPhase:
      {
      bool GotOne = false;
      m_sDesc += "[";
      for (int o=CDB.PhaseFirst(BOT_Solid); o<=CDB.PhaseLast(BOT_Gas); o++)
        {
        CPhaseInfo & P=CDB.PhaseInfo(o);
        if (m_Phases&P.m_PhMsk)
          {
          if (GotOne)
            m_sDesc+=",";
          m_sDesc += P.m_Sym();
          GotOne = true;
          }
        }
      m_sDesc.RTrim();
      m_sDesc += "]";
      break;
      }
    case Slct_Specie:
      {
      m_sDesc.Append("[");
      for (int i=0; i<m_Species.GetSize(); i++)
        {
        if (i>0)
          m_sDesc+=",";
        m_sDesc+=SDB[m_Species[i]].SymOrTag();
        }
      m_sDesc.RTrim();
      m_sDesc += "]";
      break;
      }
    case Slct_Component:
      m_sDesc.Append("[");
      for (int i=0; i<m_Comps.GetSize(); i++)
        {
        if (i>0)
          m_sDesc+=",";
        m_sDesc+=CDB[m_Comps[i]].SymOrTag();
        }
      m_sDesc.RTrim();
      m_sDesc += "]";
      break;
    case Slct_Element:
      m_sDesc.Append("[");
      for (int i=0; i<m_Elements.GetSize(); i++)
        {
        if (i>0)
          m_sDesc+=",";
        m_sDesc+=EDB[m_Elements[i]].m_Name;
        }
      m_sDesc.RTrim();
      m_sDesc += "]";
      break;
    }
  return m_sDesc();
  }

//============================================================================
//
//
//
//============================================================================

class DllImportExport CXBlk_MUFixed : public CXBlk_MUBase
  {
  public:
    CXBlk_MUFixed(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUBase(Op_MUFixed, pClass_, Tag_, pAttach, eAttach)
      {
      m_Meas1.Initialise("MkUp", 1, "MakeUp Measurement");
      };
    virtual ~CXBlk_MUFixed() {};
  };

class DllImportExport CXBlk_MURatio : public CXBlk_MUBase
  {
  public:
    CXBlk_MURatio(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUBase(Op_MURatio, pClass_, Tag_, pAttach, eAttach)
      {
      m_Meas1.Initialise("Feed", 1, "Feed Measurement");
      m_Meas2.Initialise("MkUp", 2, "MakeUp Measurement");
      };
    virtual ~CXBlk_MURatio() {};
  };


class DllImportExport CXBlk_PrdFlow: public CXBlk_MUBase
  {
  public:
    CXBlk_PrdFlow(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUBase(Op_PrdFlow, pClass_, Tag_, pAttach, eAttach)
      {
      m_Meas1.Initialise("Product", 1, "Product Measurement");
      };
    virtual ~CXBlk_PrdFlow() {};
  };

class DllImportExport CXBlk_PrdFrac: public CXBlk_MUBase
  {
  public:
    CXBlk_PrdFrac(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUBase(Op_PrdComp, pClass_, Tag_, pAttach, eAttach)
      {
      m_Meas1.Initialise("Product", 1, "Product Measurement", Type_Mass, Slct_Occur, som_Liq);
      };
    virtual ~CXBlk_PrdFrac() {};
  };

class DllImportExport CXBlk_PrdConc: public CXBlk_MUBase
  {
  public:
    CXBlk_PrdConc(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUBase(Op_PrdConc, pClass_, Tag_, pAttach, eAttach)
      {
      m_Meas1.Initialise("ProductMass", 1, "Product Mass Measurement", Type_Mass, Slct_Specie);
      m_Meas2.Initialise("ProductVolume", 2, "Product Volume Measurement", Type_Volume, Slct_Occur, som_Liq);
      };
    virtual ~CXBlk_PrdConc() {};
  };

class DllImportExport CXBlk_PrdTemp: public CXBlk_MUBase
  {
  public:
    CXBlk_PrdTemp(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUBase(Op_PrdTemp, pClass_, Tag_, pAttach, eAttach)
      {
      m_RqdSetPoint = StdT;
      m_MeasFeed = StdT;
      m_MeasMkUp = StdT;
      m_MeasProd = StdT;
      };
    virtual ~CXBlk_PrdTemp() {};
  };

class DllImportExport CXBlk_PrdTChg: public CXBlk_MUBase
  {
  public:
    CXBlk_PrdTChg(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUBase(Op_PrdTChg, pClass_, Tag_, pAttach, eAttach)
      {
      m_RqdSetPoint = 0;
      m_MeasFeed = StdT;
      m_MeasMkUp = StdT;
      m_MeasProd = StdT;
      };
    virtual ~CXBlk_PrdTChg() {};
  };

DEFINE_MAKEUPBLOCK(CXBlk_MUFixed);
DEFINE_MAKEUPBLOCK(CXBlk_MURatio);
DEFINE_MAKEUPBLOCK(CXBlk_PrdFlow);
DEFINE_MAKEUPBLOCK(CXBlk_PrdFrac);
DEFINE_MAKEUPBLOCK(CXBlk_PrdConc);
DEFINE_MAKEUPBLOCK(CXBlk_PrdTemp);
DEFINE_MAKEUPBLOCK(CXBlk_PrdTChg);

//============================================================================
//
//
//
//============================================================================

CToleranceBlock CXBlk_MUBase::sm_QmTol(TBF_DynSys, "MUFeed:CtrlEPS", 1.0e-8, 1.0e-8, 200);

IMPLEMENT_MAKEUPBLOCK(CXBlk_MUFixed, "MB_Fixed",   "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Fixed Makeup",           " ");
IMPLEMENT_MAKEUPBLOCK(CXBlk_MURatio, "MB_Ratio",   "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Ratio Makeup",           " ");
IMPLEMENT_MAKEUPBLOCK(CXBlk_PrdFlow, "MB_PrdFlow", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Product Flow",           " ");
IMPLEMENT_MAKEUPBLOCK(CXBlk_PrdFrac, "MB_PrdFrac", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Product Composition",    " ");
IMPLEMENT_MAKEUPBLOCK(CXBlk_PrdConc, "MB_PrdConc", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Product Concentration",  " ");
IMPLEMENT_MAKEUPBLOCK(CXBlk_PrdTemp, "MB_PrdTemp", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Product Temperature",    " ");
IMPLEMENT_MAKEUPBLOCK(CXBlk_PrdTChg, "MB_PrdTChg", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Temperature Change",    " ");

CXBlk_MUBase::CXBlk_MUBase(eOps Op, pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
CMakeupBlock(pClass_, Tag_, pAttach, eAttach)
  {
  m_Op        = Op;
  m_eSource   = Src_Remote;

  m_MUQmMin   = 0.0;
  m_MUQmMax   = 100.0;

  m_eLoFeedOpt= LF_AlwaysOn;
  m_LoFeedQm  = 1.0;

  m_RqdSetPoint = 0;

  m_bHasFlow   = true;
  m_bStopMakeUp= false;

  m_SetPoint   = 0.0;
  m_Measured   = 0.0;
  m_Result     = 0.0;
  m_QmFeed     = 0.0;
  m_QmMkUp     = 0.0;
  m_QmProd     = 0.0;
  m_QvFeed     = 0.0;
  m_QvMkUp     = 0.0;
  m_QvProd     = 0.0;

  m_MeasFeed   = 0.0;
  m_MeasMkUp   = 0.0;
  m_MeasProd   = 0.0;

  m_eRqdTemp    = Temp_Mixture;
  m_RqdTemp     = C2K(25);

  m_TempFeed   = C2K(25);
  m_TempMkUp   = C2K(25);
  m_TempProd   = C2K(25);
  m_HeatFlow    = 0;

  m_ValidateOK  = true;

  ClrMeasEtc();
  }

//--------------------------------------------------------------------------

CXBlk_MUBase::~CXBlk_MUBase()
  {
  }

//--------------------------------------------------------------------------

void CXBlk_MUBase::BuildDataDefn(DataDefnBlk& DDB)
  {
  static DDBValueLst DDBCtrl[] =
    {
      {Type_Mass,           "Mass"          },
      {Type_Volume,         "Volume"        },
      {Type_NVolume,        "NVolume"       },
      {Type_Mole,           "Mole"          },
      {}
    };

  static DDBValueLst DDBCtrlFlw[] =
    {
      {Type_Mass,           "MassFlow"          },
      {Type_Volume,         "VolumeFlow"        },
      {Type_NVolume,        "NVolumeFlow"       },
      {Type_Mole,           "MoleFlow"          },
      {}
    };

  static DDBValueLst DDBCtrlRatio[] =
    {
      {Type_Mass,           "MassRatio"          },
      {Type_Volume,         "VolumeRatio"        },
      {Type_NVolume,        "NVolumeRatio"       },
      {Type_Mole,           "MoleRatio"          },
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
      {LF_AlwaysOn,         "AlwaysOn"            },
      {LF_StopBelow,        "StopBelowQmLimit"    },
      //{LF_HoldMeasure,      "HoldAtLo"          },
      {}
    };

  //DDB.Text(" ");
  //DDB.Text("Requirement");
  switch (m_Op)
    {
    case Op_MUFixed:
      {
      DDB.Long       ("", "Rqd.Basis",             DC_,  "", xidMkType,  this, isParmStopped|SetOnChange, DDBCtrlFlw);
      break;
      }
    case Op_MURatio:
      {
      DDB.Long       ("", "Rqd.Basis",    DC_,  "", xidMkType,  this, isParmStopped|SetOnChange, DDBCtrlRatio);
      break;
      }
    case Op_PrdFlow:
      {
      DDB.Long       ("", "Rqd.Basis",             DC_,  "", xidMkType,  this, isParmStopped|SetOnChange, DDBCtrlFlw);
      break;
      }
    case Op_PrdComp:
      {
      DDB.Long       ("", "Rqd.Basis",             DC_,  "", xidMkType,  this, isParmStopped|SetOnChange, DDBCtrl);
      break;
      }
    case Op_PrdConc:
    case Op_PrdTemp:
    case Op_PrdTChg:
      break;
    default:;
    }

  DDB.Visibility();
  switch (m_Op)
    {
    case Op_PrdTemp:
    case Op_PrdTChg:
      break;
    default:
      //DDB.Text(" ");
      m_Meas1.BuildDataDefn(DDB, *this);
      break;
    }

  switch (m_Op)
    {
    case Op_MURatio:
    case Op_PrdConc:
      //DDB.Text(" ");
      m_Meas2.BuildDataDefn(DDB, *this);
      break;
    }

  DDB.Text("----------------------------------------");
  switch (m_Op)
    {
    case Op_MUFixed:
      {
      switch (m_Meas1.m_eType)
        {
        case Type_Mass:
          {
          Strng S("Required Mass flow ", m_Meas1.Description(false));
          DDB.Text(S());
          DDB.Double("", "Rqd.MakeUp.Qm",     DC_Qm,     "kg/s",   &m_RqdSetPoint,  this, isParm);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.Feed.Qm",    DC_Qm,     "kg/s",   &m_MeasFeed,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.MakeUp.Qm",  DC_Qm,     "kg/s",   &m_MeasMkUp,     this, isResult);
          //DDB.Double("", "Meas.Prod.Qm",    DC_Qm,     "kg/s",   &m_MeasProd,     this, isResult|InitHidden);
          break;
          }
        case Type_Mole:
          {
          Strng S("Required Molar flow ", m_Meas1.Description(false));
          DDB.Text(S());
          DDB.Double("", "Rqd.MakeUp.QMl",    DC_QKgMl,  "kmol/s", &m_RqdSetPoint,  this, isParm);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.Feed.QMl",   DC_QKgMl,  "kmol/s", &m_MeasFeed,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.MakeUp.QMl", DC_QKgMl,  "kmol/s", &m_MeasMkUp,     this, isResult);
          //DDB.Double("", "Meas.Prod.QMl",   DC_QKgMl,  "kmol/s", &m_MeasProd,     this, isResult|InitHidden);
          break;
          }
        case Type_Volume:
          {
          Strng S("Required Volume flow ", m_Meas1.Description(false));
          DDB.Text(S());
          DDB.Double("", "Rqd.MakeUp.Qv",     DC_Qv,     "L/s",    &m_RqdSetPoint,  this, isParm);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.Feed.Qv",    DC_Qv,     "L/s",    &m_MeasFeed,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.MakeUp.Qv",  DC_Qv,     "L/s",    &m_MeasMkUp,     this, isResult);
          //DDB.Double("", "Meas.Prod.Qv",    DC_Qv,     "L/s",    &m_MeasProd,     this, isResult|InitHidden);
          break;
          }
        case Type_NVolume:
          {
          Strng S("Required NVolume flow ", m_Meas1.Description(false));
          DDB.Text(S());
          DDB.Double("", "Rqd.MakeUp.NQv",    DC_NQv,    "NL/s",   &m_RqdSetPoint,  this, isParm);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.Feed.NQv",   DC_NQv,    "NL/s",   &m_MeasFeed,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.MakeUp.NQv", DC_NQv,    "NL/s",   &m_MeasMkUp,     this, isResult);
          //DDB.Double("", "Meas.Prod.NQv",   DC_NQv,    "NL/s",   &m_MeasProd,     this, isResult|InitHidden);
          break;
          }
        }
      break;
      }
    case Op_MURatio:
      {
      switch (m_Meas1.m_eType)
        {
        case Type_Mass:
          {
          Strng S("Required Mass ratio");
          DDB.Text(S());
          DDB.Double("", "Rqd.QmRatio",    DC_Frac,   "",         &m_RqdSetPoint,    this, isParm);
          //DDB.Double("", "Meas.QmRatio",   DC_Frac,   "",         &m_Measured,       this, isResult);
          //DDB.Double("", "Meas.Feed.Qm",   DC_Qm,     "kg/s",     &m_MeasFeed,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.MakeUp.Qm", DC_Qm,     "kg/s",     &m_MeasMkUp,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas2.Description(false));
          break;
          }
        case Type_Mole:
          {
          Strng S("Required Molar ratio");
          DDB.Text(S());
          DDB.Double("", "Rqd.QMlRatio",   DC_Frac,   "",         &m_RqdSetPoint,    this, isParm);
          //DDB.Double("", "Meas.QMlRatio",  DC_Frac,   "",         &m_Measured,       this, isResult);
          //DDB.Double("", "Meas.Feed.QMl",  DC_QKgMl,  "kmol/s",   &m_MeasFeed,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.MakeUp.QMl", DC_QKgMl,  "kmol/s",   &m_MeasMkUp,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas2.Description(false));
          break;
          }
        case Type_Volume:
          {
          Strng S("Required Volume ratio");
          DDB.Text(S());
          DDB.Double("", "Rqd.QvRatio",    DC_Frac,   "",         &m_RqdSetPoint,    this, isParm);
          //DDB.Double("", "Meas.QvRatio",   DC_Frac,   "",         &m_Measured,       this, isResult);
          //DDB.Double("", "Meas.Feed.Qv",   DC_Qv,     "L/s",      &m_MeasFeed,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.MakeUp.Qv", DC_Qv,     "L/s",      &m_MeasMkUp,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas2.Description(false));
          break;
          }
        case Type_NVolume:
          {
          Strng S("Required NVolume ratio");
          DDB.Text(S());
          DDB.Double("", "Rqd.NQvRatio",   DC_Frac,   "",         &m_RqdSetPoint,    this, isParm);
          //DDB.Double("", "Meas.NQvRatio",  DC_Frac,   "",         &m_Measured,       this, isResult);
          //DDB.Double("", "Meas.Feed.NQv",  DC_NQv,    "NL/s",     &m_MeasFeed,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.MakeUp.NQv", DC_NQv,    "NL/s",     &m_MeasMkUp,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas2.Description(false));
          break;
          }
        }
      break;
      }
    case Op_PrdFlow:
      {
      switch (m_Meas1.m_eType)
        {
        case Type_Mass:
          {
          Strng S("Required Mass flow ", m_Meas1.Description(false));
          DDB.Text(S());
          DDB.Double("", "Rqd.Prod.Qm",     DC_Qm,     "kg/s",   &m_RqdSetPoint,  this, isParm);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.Feed.Qm",    DC_Qm,     "kg/s",   &m_MeasFeed,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.MakeUp.Qm",  DC_Qm,     "kg/s",   &m_MeasMkUp,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.Prod.Qm",    DC_Qm,     "kg/s",   &m_MeasProd,     this, isResult);
          break;
          }
        case Type_Mole:
          {
          Strng S("Required Molar flow ", m_Meas1.Description(false));
          DDB.Text(S());
          DDB.Double("", "Rqd.Prod.QMl",    DC_QKgMl,  "kmol/s", &m_RqdSetPoint,  this, isParm);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.Feed.QMl",   DC_QKgMl,  "kmol/s", &m_MeasFeed,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.MakeUp.QMl", DC_QKgMl,  "kmol/s", &m_MeasMkUp,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.Prod.QMl",   DC_QKgMl,  "kmol/s", &m_MeasProd,     this, isResult);
          break;
          }
        case Type_Volume:
          {
          Strng S("Required Volume flow ", m_Meas1.Description(false));
          DDB.Text(S());
          DDB.Double("", "Rqd.Prod.Qv",     DC_Qv,     "L/s",   &m_RqdSetPoint,  this, isParm);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.Feed.Qv",    DC_Qv,     "L/s",   &m_MeasFeed,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.MakeUp.Qv",  DC_Qv,     "L/s",   &m_MeasMkUp,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.Prod.Qv",    DC_Qv,     "L/s",   &m_MeasProd,     this, isResult);
          break;
          }
        case Type_NVolume:
          {
          Strng S("Required NVolume flow ", m_Meas1.Description(false));
          DDB.Text(S());
          DDB.Double("", "Rqd.Prod.NQv",    DC_NQv,    "NL/s",  &m_RqdSetPoint,  this, isParm);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.Feed.NQv",   DC_NQv,    "NL/s",  &m_MeasFeed,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.MakeUp.NQv", DC_NQv,    "NL/s",  &m_MeasMkUp,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.Prod.NQv",   DC_NQv,    "NL/s",  &m_MeasProd,     this, isResult);
          break;
          }
        }
      break;
      }
    case Op_PrdComp:
      {
      switch (m_Meas1.m_eType)
        {
        case Type_Mass:
          {
          Strng S("Required Mass fraction ", m_Meas1.Description(false));
          DDB.Text(S());
          DDB.Double("", "Rqd.Prod.QmFrac",     DC_Frac, "%",  &m_RqdSetPoint,  this, isParm);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.Feed.QmFrac",    DC_Frac, "%",  &m_MeasFeed,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.MakeUp.QmFrac",  DC_Frac, "%",  &m_MeasMkUp,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.Prod.QmFrac",    DC_Frac, "%",  &m_MeasProd,     this, isResult);
          break;
          }
        case Type_Mole:
          {
          Strng S("Required Molar fraction ", m_Meas1.Description(false));
          DDB.Text(S());
          DDB.Double("", "Rqd.Prod.QMlFrac",    DC_Frac, "%",  &m_RqdSetPoint,  this, isParm);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.Feed.QMlFrac",   DC_Frac, "%",  &m_MeasFeed,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.MakeUp.QMlFrac", DC_Frac, "%",  &m_MeasMkUp,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.Prod.QMlFrac",   DC_Frac, "%",  &m_MeasProd,     this, isResult);
          break;
          }
        case Type_Volume:
          {
          Strng S("Required Volume fraction ", m_Meas1.Description(false));
          DDB.Text(S());
          DDB.Double("", "Rqd.Prod.QvFrac",     DC_Frac, "%",  &m_RqdSetPoint,  this, isParm);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.Feed.QvFrac",    DC_Frac, "%",  &m_MeasFeed,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.MakeUp.QvFrac",  DC_Frac, "%",  &m_MeasMkUp,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.Prod.QvFrac",    DC_Frac, "%",  &m_MeasProd,     this, isResult);
          break;
          }
        case Type_NVolume:
          {
          Strng S("Required NVolume fraction ", m_Meas1.Description(false));
          DDB.Text(S());
          DDB.Double("", "Rqd.Prod.NQvFrac",    DC_Frac, "%",  &m_RqdSetPoint,  this, isParm);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Double("", "Meas.Feed.NQvFrac",   DC_Frac, "%",  &m_MeasFeed,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.MakeUp.NQvFrac", DC_Frac, "%",  &m_MeasMkUp,     this, isResult|InitHidden);
          //DDB.Double("", "Meas.Prod.NQvFrac",   DC_Frac, "%",  &m_MeasProd,     this, isResult);
          break;
          }
        }
      break;
      }
    case Op_PrdConc:
      {
      //Strng S("Concentration:", m_Meas1.Description(true), "/");
      //S+=m_Meas2.Description(true);
      //DDB.Text(S());
      Strng S("Required Concentration ", m_Meas1.Description(false));
      DDB.Text(S());
      DDB.Double("", "Rqd.Prod.Conc",     DC_Conc, "g/L",  &m_RqdSetPoint,     this, isParm);
      //DDB.Double("", "Meas.Feed.Conc",    DC_Conc, "g/L",  &m_MeasFeed,        this, isResult|InitHidden);
      //DDB.Double("", "Meas.MakeUp.Conc",  DC_Conc, "g/L",  &m_MeasMkUp,        this, isResult|InitHidden);
      //DDB.Double("", "Meas.Prod.Conc",    DC_Conc, "g/L",  &m_MeasProd,        this, isResult);
      break;
      }
    case Op_PrdTemp:
      {
      Strng S("Required Temperature");
      DDB.Text(S());
      DDB.Double("", "Rqd.Prod.Temp",     DC_T, "C",  &m_RqdSetPoint,     this, isParm);
      //DDB.Double("", "Meas.Feed.Temp",    DC_T, "C",  &m_MeasFeed,        this, isResult|InitHidden);
      //DDB.Double("", "Meas.MakeUp.Temp",  DC_T, "C",  &m_MeasMkUp,        this, isResult|InitHidden);
      //DDB.Double("", "Meas.Prod.Temp",    DC_T, "C",  &m_MeasProd,        this, isResult);
      break;
      }
    case Op_PrdTChg:
      {
      Strng S("Required Temperature change");
      DDB.Text(S());
      DDB.Double("", "Rqd.Prod.TChg",     DC_dT, "C",  &m_RqdSetPoint,     this, isParm);
      //DDB.Double("", "Meas.Prod.TChg",    DC_dT, "C",  &m_Measured,        this, isResult);
      //DDB.Double("", "Meas.Feed.Temp",    DC_T, "C",   &m_MeasFeed,        this, isResult|InitHidden);
      //DDB.Double("", "Meas.MakeUp.Temp",  DC_T, "C",   &m_MeasMkUp,        this, isResult|InitHidden);
      //DDB.Double("", "Meas.Prod.Temp",    DC_T, "C",   &m_MeasProd,        this, isResult);
      break;
      }
    default:;
    }

  DDB.Visibility();
  DDB.Text(" ");
  DDB.Text("MakeUp Limits");
  //DDB.Long       ("", "Source",           DC_,   "", (long*)&m_eSource,  this, isParm|SetOnChange, DDBSource);
  DDB.Double     ("MakeUp.QmMin", "",            DC_Qm, "kg/s", xidMkQmMin, this, isParm);
  DDB.Double     ("MakeUp.QmMax", "",            DC_Qm, "kg/s", &m_MUQmMax, this, isParm);

  DDB.Text(" ");
  DDB.Text("Makeup Rule for Low Feed Flow");
  DDB.Long       ("LowQmRule",   "",         DC_,   "", (long*)&m_eLoFeedOpt, this, isParm|SetOnChange, DDBLoFeed);
  DDB.Visibility (NSHM_All, m_eLoFeedOpt >LF_AlwaysOn);
  DDB.Double     ("LowQmLimit",    "",       DC_Qm, "kg/s", &m_LoFeedQm, this, isParm);
  DDB.Visibility ();

  if (!HeatSkipMethod())
    {
    switch (m_Op)
      {
      case Op_PrdTemp:
      case Op_PrdTChg:
        break;
      default:
        {
        static DDBValueLst DDBTemp[] =
          {
            {Temp_Inlet,   "InletTemp"},
            {Temp_Source,  "SourceTemp"},
            {Temp_Std,     "StdTemp"},
            {Temp_Const,   "Const"},
            {Temp_Mixture, "Mixture"},
            {0}
          };
        DDB.Text(" ");
        DDB.Text("Product Temperature");
        DDB.Long  ("Temp.Final",      "",  DC_,  "", (long*)&m_eRqdTemp, this, isParm|SetOnChange, DDBTemp);
        DDB.Visibility(NSHM_All, m_eRqdTemp==Temp_Const);
        DDB.Double("Temp.Reqd",        "",  DC_T, "C", &m_RqdTemp, this, isParm);
        DDB.Visibility();
        break;
        }
      }
    }

  DDB.Text(" ");
  DDB.Text("Results");
  DDB.String("Description",      "", DC_,   "",    xidMkDesc,     this, isResult);
  DDB.String("State",            "", DC_,   "",    xidMkState,     this, isResult);

  DDB.Text(" ");
  switch (m_Op)
    {
    case Op_MUFixed:
      {
      switch (m_Meas1.m_eType)
        {
        case Type_Mass:
          {
          Strng S("Mass flow ", m_Meas1.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          DDB.Double("", "Rqd.MakeUp.Qm",   DC_Qm,     "kg/s",   &m_RqdSetPoint,  this, isResult);
          //DDB.TagComment(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.Qm",    DC_Qm,     "kg/s",   &m_MeasFeed,     this, isResult|InitHidden);
          DDB.Double("", "Meas.MakeUp.Qm",  DC_Qm,     "kg/s",   &m_MeasMkUp,     this, isResult);
          DDB.Double("", "Meas.Prod.Qm",    DC_Qm,     "kg/s",   &m_MeasProd,     this, isResult|InitHidden);
          break;
          }
        case Type_Mole:
          {
          Strng S("Molar flow ", m_Meas1.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          DDB.Double("", "Rqd.MakeUp.QMl",  DC_QKgMl,  "kmol/s", &m_RqdSetPoint,  this, isResult);
          //DDB.TagComment(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.QMl",   DC_QKgMl,  "kmol/s", &m_MeasFeed,     this, isResult|InitHidden);
          DDB.Double("", "Meas.MakeUp.QMl", DC_QKgMl,  "kmol/s", &m_MeasMkUp,     this, isResult);
          DDB.Double("", "Meas.Prod.QMl",   DC_QKgMl,  "kmol/s", &m_MeasProd,     this, isResult|InitHidden);
          break;
          }
        case Type_Volume:
          {
          Strng S("Volume flow ", m_Meas1.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          DDB.Double("", "Rqd.MakeUp.Qv",   DC_Qv,     "L/s",    &m_RqdSetPoint,  this, isResult);
          //DDB.TagComment(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.Qv",    DC_Qv,     "L/s",    &m_MeasFeed,     this, isResult|InitHidden);
          DDB.Double("", "Meas.MakeUp.Qv",  DC_Qv,     "L/s",    &m_MeasMkUp,     this, isResult);
          DDB.Double("", "Meas.Prod.Qv",    DC_Qv,     "L/s",    &m_MeasProd,     this, isResult|InitHidden);
          break;
          }
        case Type_NVolume:
          {
          Strng S("NVolume flow ", m_Meas1.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          DDB.Double("", "Rqd.MakeUp.NQv",  DC_NQv,    "NL/s",   &m_RqdSetPoint,  this, isResult);
          //DDB.TagComment(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.NQv",   DC_NQv,    "NL/s",   &m_MeasFeed,     this, isResult|InitHidden);
          DDB.Double("", "Meas.MakeUp.NQv", DC_NQv,    "NL/s",   &m_MeasMkUp,     this, isResult);
          DDB.Double("", "Meas.Prod.NQv",   DC_NQv,    "NL/s",   &m_MeasProd,     this, isResult|InitHidden);
          break;
          }
        }
      break;
      }
    case Op_MURatio:
      {
      switch (m_Meas1.m_eType)
        {
        case Type_Mass:
          {
          /*CNM */ DDB.Double("", "Rqd.QmRatio",    DC_Frac,   "",         &m_RqdSetPoint,    this, isResult|DupHandlesOk);
          DDB.Double("", "Meas.QmRatio",   DC_Frac,   "",         &m_Measured,       this, isResult);
          Strng S;
          S.Set("Mass flow ratio of MakeUp%s to Feed%s", m_Meas1.Description(false), m_Meas2.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.Qm",   DC_Qm,     "kg/s",     &m_MeasFeed,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Text(m_Meas2.Description(false));
          DDB.Double("", "Meas.MakeUp.Qm", DC_Qm,     "kg/s",     &m_MeasMkUp,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas2.Description(false));
          break;
          }
        case Type_Mole:
          {
          /*CNM */ DDB.Double("", "Rqd.QMlRatio",   DC_Frac,   "",         &m_RqdSetPoint,    this, isResult|DupHandlesOk);
          DDB.Double("", "Meas.QMlRatio",  DC_Frac,   "",         &m_Measured,       this, isResult);
          Strng S;
          S.Set("Molar flow ratio of MakeUp%s to Feed%s", m_Meas1.Description(false), m_Meas2.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.QMl",  DC_QKgMl,  "kmol/s",   &m_MeasFeed,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Text(m_Meas1.Description(false));
          DDB.Double("", "Meas.MakeUp.QMl", DC_QKgMl,  "kmol/s",   &m_MeasMkUp,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas2.Description(false));
          break;
          }
        case Type_Volume:
          {
          /*CNM */ DDB.Double("", "Rqd.QvRatio",    DC_Frac,   "",         &m_RqdSetPoint,    this, isResult|DupHandlesOk);
          DDB.Double("", "Meas.QvRatio",   DC_Frac,   "",         &m_Measured,       this, isResult);
          Strng S;
          S.Set("Volume flow ratio of MakeUp%s to Feed%s", m_Meas1.Description(false), m_Meas2.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.Qv",   DC_Qv,     "L/s",      &m_MeasFeed,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Text(m_Meas1.Description(false));
          DDB.Double("", "Meas.MakeUp.Qv", DC_Qv,     "L/s",      &m_MeasMkUp,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas2.Description(false));
          break;
          }
        case Type_NVolume:
          {
          /*CNM */ DDB.Double("", "Rqd.NQvRatio",   DC_Frac,   "",         &m_RqdSetPoint,    this, isResult|DupHandlesOk);
          DDB.Double("", "Meas.NQvRatio",  DC_Frac,   "",         &m_Measured,       this, isResult);
          Strng S;
          S.Set("NVolume flow ratio of MakeUp%s to Feed%s", m_Meas1.Description(false), m_Meas2.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.NQv",  DC_NQv,    "NL/s",     &m_MeasFeed,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas1.Description(false));
          //DDB.Text(m_Meas1.Description(false));
          DDB.Double("", "Meas.MakeUp.NQv", DC_NQv,    "NL/s",     &m_MeasMkUp,       this, isResult|InitHidden);
          //DDB.TagComment(m_Meas2.Description(false));
          break;
          }
        }
      break;
      }
    case Op_PrdFlow:
      {
      switch (m_Meas1.m_eType)
        {
        case Type_Mass:
          {
          Strng S("Mass flow ", m_Meas1.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          /*CNM */ DDB.Double("", "Rqd.Prod.Qm",     DC_Qm,     "kg/s",   &m_RqdSetPoint,  this, isResult|DupHandlesOk);
          //DDB.TagComment(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.Qm",    DC_Qm,     "kg/s",   &m_MeasFeed,     this, isResult|InitHidden);
          DDB.Double("", "Meas.MakeUp.Qm",  DC_Qm,     "kg/s",   &m_MeasMkUp,     this, isResult|InitHidden);
          DDB.Double("", "Meas.Prod.Qm",    DC_Qm,     "kg/s",   &m_MeasProd,     this, isResult);
          break;
          }
        case Type_Mole:
          {
          Strng S("Molar flow ", m_Meas1.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          /*CNM */ DDB.Double("", "Rqd.Prod.QMl",    DC_QKgMl,  "kmol/s", &m_RqdSetPoint,  this, isResult|DupHandlesOk);
          //DDB.TagComment(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.QMl",   DC_QKgMl,  "kmol/s", &m_MeasFeed,     this, isResult|InitHidden);
          DDB.Double("", "Meas.MakeUp.QMl", DC_QKgMl,  "kmol/s", &m_MeasMkUp,     this, isResult|InitHidden);
          DDB.Double("", "Meas.Prod.QMl",   DC_QKgMl,  "kmol/s", &m_MeasProd,     this, isResult);
          break;
          }
        case Type_Volume:
          {
          Strng S("Volume flow ", m_Meas1.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          /*CNM */ DDB.Double("", "Rqd.Prod.Qv",     DC_Qv,     "L/s",   &m_RqdSetPoint,  this, isResult|DupHandlesOk);
          //DDB.TagComment(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.Qv",    DC_Qv,     "L/s",   &m_MeasFeed,     this, isResult|InitHidden);
          DDB.Double("", "Meas.MakeUp.Qv",  DC_Qv,     "L/s",   &m_MeasMkUp,     this, isResult|InitHidden);
          DDB.Double("", "Meas.Prod.Qv",    DC_Qv,     "L/s",   &m_MeasProd,     this, isResult);
          break;
          }
        case Type_NVolume:
          {
          Strng S("NVolume flow ", m_Meas1.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          /*CNM */ DDB.Double("", "Rqd.Prod.NQv",    DC_NQv,    "NL/s",  &m_RqdSetPoint,  this, isResult|DupHandlesOk);
          //DDB.TagComment(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.NQv",   DC_NQv,    "NL/s",  &m_MeasFeed,     this, isResult|InitHidden);
          DDB.Double("", "Meas.MakeUp.NQv", DC_NQv,    "NL/s",  &m_MeasMkUp,     this, isResult|InitHidden);
          DDB.Double("", "Meas.Prod.NQv",   DC_NQv,    "NL/s",  &m_MeasProd,     this, isResult);
          break;
          }
        }
      break;
      }
    case Op_PrdComp:
      {
      switch (m_Meas1.m_eType)
        {
        case Type_Mass:
          {
          Strng S("Mass fraction ", m_Meas1.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          /*CNM */ DDB.Double("", "Rqd.Prod.QmFrac",     DC_Frac, "%",  &m_RqdSetPoint,  this, isResult|DupHandlesOk);
          //DDB.TagComment(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.QmFrac",    DC_Frac, "%",  &m_MeasFeed,     this, isResult|InitHidden);
          DDB.Double("", "Meas.MakeUp.QmFrac",  DC_Frac, "%",  &m_MeasMkUp,     this, isResult|InitHidden);
          DDB.Double("", "Meas.Prod.QmFrac",    DC_Frac, "%",  &m_MeasProd,     this, isResult);
          break;
          }
        case Type_Mole:
          {
          Strng S("Molar fraction ", m_Meas1.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          /*CNM */ DDB.Double("", "Rqd.Prod.QMlFrac",    DC_Frac, "%",  &m_RqdSetPoint,  this, isResult|DupHandlesOk);
          //DDB.TagComment(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.QMlFrac",   DC_Frac, "%",  &m_MeasFeed,     this, isResult|InitHidden);
          DDB.Double("", "Meas.MakeUp.QMlFrac", DC_Frac, "%",  &m_MeasMkUp,     this, isResult|InitHidden);
          DDB.Double("", "Meas.Prod.QMlFrac",   DC_Frac, "%",  &m_MeasProd,     this, isResult);
          break;
          }
        case Type_Volume:
          {
          Strng S("Volume fraction ", m_Meas1.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          /*CNM */ DDB.Double("", "Rqd.Prod.QvFrac",     DC_Frac, "%",  &m_RqdSetPoint,  this, isResult|DupHandlesOk);
          //DDB.TagComment(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.QvFrac",    DC_Frac, "%",  &m_MeasFeed,     this, isResult|InitHidden);
          DDB.Double("", "Meas.MakeUp.QvFrac",  DC_Frac, "%",  &m_MeasMkUp,     this, isResult|InitHidden);
          DDB.Double("", "Meas.Prod.QvFrac",    DC_Frac, "%",  &m_MeasProd,     this, isResult);
          break;
          }
        case Type_NVolume:
          {
          Strng S("NVolume fraction ", m_Meas1.Description(false));
          DDB.Text(S());
          //DDB.Text(m_Meas1.Description(false));
          /*CNM */ DDB.Double("", "Rqd.Prod.NQvFrac",    DC_Frac, "%",  &m_RqdSetPoint,  this, isResult|DupHandlesOk);
          //DDB.TagComment(m_Meas1.Description(false));
          DDB.Double("", "Meas.Feed.NQvFrac",   DC_Frac, "%",  &m_MeasFeed,     this, isResult|InitHidden);
          DDB.Double("", "Meas.MakeUp.NQvFrac", DC_Frac, "%",  &m_MeasMkUp,     this, isResult|InitHidden);
          DDB.Double("", "Meas.Prod.NQvFrac",   DC_Frac, "%",  &m_MeasProd,     this, isResult);
          break;
          }
        }
      break;
      }
    case Op_PrdConc:
      {
      Strng S("Concentration:", m_Meas1.Description(true), "/");
      S+=m_Meas2.Description(true);
      DDB.Text(S());
      /*CNM */ DDB.Double("", "Rqd.Prod.Conc",     DC_Conc, "g/L",  &m_RqdSetPoint,     this, isResult|DupHandlesOk);
      DDB.Double("", "Meas.Feed.Conc",    DC_Conc, "g/L",  &m_MeasFeed,        this, isResult|InitHidden);
      DDB.Double("", "Meas.MakeUp.Conc",  DC_Conc, "g/L",  &m_MeasMkUp,        this, isResult|InitHidden);
      DDB.Double("", "Meas.Prod.Conc",    DC_Conc, "g/L",  &m_MeasProd,        this, isResult);
      break;
      }
    case Op_PrdTemp:
      {
      /*CNM */ DDB.Double("", "Rqd.Prod.Temp",     DC_T, "C",  &m_RqdSetPoint,     this, isResult|DupHandlesOk);
      DDB.Double("", "Meas.Feed.Temp",    DC_T, "C",  &m_MeasFeed,        this, isResult|InitHidden);
      DDB.Double("", "Meas.MakeUp.Temp",  DC_T, "C",  &m_MeasMkUp,        this, isResult|InitHidden);
      DDB.Double("", "Meas.Prod.Temp",    DC_T, "C",  &m_MeasProd,        this, isResult);
      break;
      }
    case Op_PrdTChg:
      {
      /*CNM */ DDB.Double("", "Rqd.Prod.TChg",     DC_dT, "C",  &m_RqdSetPoint,     this, isResult|DupHandlesOk);
      DDB.Double("", "Meas.Prod.TChg",    DC_dT, "C",  &m_Measured,        this, isResult);
      DDB.Double("", "Meas.Feed.Temp",    DC_T, "C",   &m_MeasFeed,        this, isResult|InitHidden);
      DDB.Double("", "Meas.MakeUp.Temp",  DC_T, "C",   &m_MeasMkUp,        this, isResult|InitHidden);
      DDB.Double("", "Meas.Prod.Temp",    DC_T, "C",   &m_MeasProd,        this, isResult);
      break;
      }
    default:;
    }

  DDB.Text(" ");
  DDB.Text("Total mass flow:");
  DDB.Double ("Qm.Feed",            "", DC_Qm,    "kg/s",     &m_QmFeed,   this, isResult);
  DDB.Double ("Qm.MakeUp",            "", DC_Qm,    "kg/s",     &m_QmMkUp,   this, isResult);
  DDB.Double ("Qm.Prod",            "", DC_Qm,    "kg/s",     &m_QmProd,   this, isResult);

  DDB.Text(" ");
  DDB.Text("Total volume flow:");
  DDB.Double ("Qv.Feed",            "", DC_Qv,    "L/s",    &m_QvFeed,     this, isResult|InitHidden);
  DDB.Double ("Qv.MakeUp",            "", DC_Qv,    "L/s",    &m_QvMkUp,     this, isResult);
  DDB.Double ("Qv.Prod",            "", DC_Qv,    "L/s",    &m_QvProd,     this, isResult|InitHidden);

  if (!HeatSkipMethod())
    {
    DDB.Text(" ");
    DDB.Text("Total heat flow:");
    DDB.Double ("Temp.Feed",          "", DC_T,    "C",       &m_TempFeed,  this, isResult);
    DDB.Double ("Temp.MakeUp",          "", DC_T,    "C",       &m_TempMkUp,  this, isResult);
    DDB.Double ("Temp.Prod",          "", DC_T,    "C",       &m_TempProd,  this, isResult);
    DDB.Double ("HeatFlow",           "", DC_Pwr,  "kW",      &m_HeatFlow,   this, isResult);
    }
  }

//--------------------------------------------------------------------------

void CXBlk_MUBase::ClrMeasEtc()
  {
  m_Measured = dNAN;
  m_SetPoint = dNAN;
  m_Result   = dNAN;
  m_MeasFeed = 0.0;
  m_MeasMkUp = 0.0;
  m_MeasProd = 0.0;
  }

flag CXBlk_MUBase::DataXchg(DataChangeBlk & DCB)
  {
  if (DCB.dwUserInfo==1 && m_Meas1.DataXchg(DCB, *this))
    return 1;
  if (DCB.dwUserInfo==2 && m_Meas2.DataXchg(DCB, *this))
    return 1;

  switch (DCB.lHandle)
    {
    case xidMkType:
      if (DCB.rL && (m_Meas1.m_eType!=(eType)*DCB.rL))
        {
        m_Meas1.m_eType     = (eType)*DCB.rL;
        m_Meas2.m_eType     = (eType)*DCB.rL;
        ClrMeasEtc();
        }
      DCB.L = m_Meas1.m_eType;
      return 1;
    case xidMkQmMin:
      if (DCB.rD)
        m_MUQmMin = Max(0.0, *DCB.rD);
      DCB.D = m_MUQmMin;
      return 1;
    case xidMkDesc:
      {
      switch (m_Op)
        {
        case Op_MUFixed:
          m_sDesc="Fixed Makeup:";
          m_sDesc+=m_Meas1.Description(true);
          break;
        case Op_MURatio:
          m_sDesc="Ratio Makeup:";
          m_sDesc+=m_Meas1.Description(true);
          m_sDesc+=" to Feed:";
          m_sDesc+=m_Meas2.Description(true);
          break;
        case Op_PrdFlow:
          m_sDesc="Product Flow:";
          m_sDesc+=m_Meas1.Description(true);
          break;
        case Op_PrdComp:
          m_sDesc="Product Comp:";
          m_sDesc+=m_Meas1.Description(true);
          break;
        case Op_PrdConc:
          m_sDesc="Product Conc:";
          m_sDesc+=m_Meas1.Description(true);
          m_sDesc+="/";
          m_sDesc+=m_Meas2.Description(true);
          break;
        case Op_PrdTemp:
          m_sDesc="Product Temperature";
        break;
        case Op_PrdTChg:
          m_sDesc="Temperature Change";
        break;
        default:
          m_sDesc="???";
          break;
        }
      DCB.pC=m_sDesc();
      return 1;
      }
    case xidMkState:
      {
      m_ErrorLst.Clear();
      ConditionBlk::GetMyCIs(m_ErrorLst,3);
      if (!m_pMakeupBase->On())
        {
        DCB.pC="Off";
        }
      else if (!SrcIO.Connected)
        {
        DCB.pC="Not Connected";
        }
      else if (!m_bHasFlow && (m_Op==Op_MURatio))
        {
        DCB.pC="No Flow";
        }
      else if (m_bStopMakeUp)
        {
        DCB.pC="Makeup stopped due to low feed rule";
        }
      else if (m_ErrorLst.Length()>0)
        {
        Strng &S=*m_ErrorLst.First();
        DCB.pC=S.XStrChr('\t')+1;
        }
      else
        DCB.pC="OK";
      return 1;
      }
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag CXBlk_MUBase::ValidateData(ValidateDataBlk & VDB)
  {
  m_Meas1.ValidateData(VDB, *this);
  m_Meas2.ValidateData(VDB, *this);

  bool CIsOn[12]={false,false,false,false,false,false,false,false,false,false,false,false};

  if (m_Meas1.m_eSelect==Slct_Element)
    CIsOn[7]=(m_Meas1.m_Elements.GetCount()==0);
  else if (m_Meas1.m_eSelect>=Slct_Specie)
    CIsOn[6]=(m_Meas1.m_Species.GetCount()==0);
  else
    CIsOn[5]=(m_Meas1.m_Phases==0);

  switch (m_Op)
    {
    case Op_MURatio:
    case Op_PrdConc:
      if (m_Meas2.m_eSelect==Slct_Element)
        CIsOn[10]=(m_Meas2.m_Elements.GetCount()==0);
      else if (m_Meas2.m_eSelect>=Slct_Specie)
        CIsOn[9]=(m_Meas2.m_Species.GetCount()==0);
      else
        CIsOn[8]=(m_Meas2.m_Phases==0);
      break;
    }

  m_ValidateOK=true;
  for (int i=5; i<=10; i++)
    {
    SetCI(i, CIsOn[i]);
    if (CIsOn[i])
      m_ValidateOK=false;
    }

  return CMakeupBlock::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

double CXBlk_MUBase::GetSetPoint()
  {
  return m_RqdSetPoint;
  }

//--------------------------------------------------------------------------

double CXBlk_MUBase::GetFlowValue(CMeasInfo &MI, SpConduit &Q, PhMask PhRqd/*=0*/)
  {
  switch (MI.m_eType)
    {
    case Type_Mass:
      if (MI.m_eSelect==Slct_Element)
        return Q.QElementMass(MI.m_Elements, MI.m_Phases);
      if (PhRqd)
        return Q.QMass(PhRqd);
      if (MI.m_eSelect>=Slct_Specie)
        return Q.QMass(MI.m_Species);
      return Q.QMass(MI.m_Phases);

    case Type_Mole:
      if (MI.m_eSelect==Slct_Element)
        return Q.QElementMoles(MI.m_Elements, MI.m_Phases);
      if (PhRqd)
        return Q.QMole(PhRqd);
      if (MI.m_eSelect>=Slct_Specie)
        return Q.QMole(MI.m_Species);
      return Q.QMole(MI.m_Phases);

    case Type_Volume:
      if (MI.m_eSelect==Slct_Element)
        return Q.QElementVolume(MI.m_Elements, MI.m_Phases);
      if (PhRqd)
        return Q.QVolume(PhRqd);
      if (MI.m_eSelect>=Slct_Specie)
        return Q.QVolume(MI.m_Species);
      return Q.QVolume(MI.m_Phases);

    case Type_NVolume:
      if (MI.m_eSelect==Slct_Element)
        return Q.QElementNVolume(MI.m_Elements, MI.m_Phases);
      if (PhRqd)
        return Q.QNVolume(PhRqd);
      if (MI.m_eSelect>=Slct_Specie)
        return Q.QNVolume(MI.m_Species);
      return Q.QNVolume(MI.m_Phases);

    }
  return dNAN;
  }

double CXBlk_MUBase::GetMeasVal(SpConduit &QFeed, SpConduit &QMkUp, SpConduit &QProd)
  {
  switch (m_Op)
    {
    case Op_MUFixed:   return GetFlowValue(m_Meas1, QMkUp);
    case Op_MURatio:   return GetFlowValue(m_Meas2, QMkUp)/GTZ(GetFlowValue(m_Meas1, QFeed));
    case Op_PrdFlow:   return GetFlowValue(m_Meas1, QProd);
    case Op_PrdComp:   return GetFlowValue(m_Meas1, QProd)/GTZ(GetFlowValue(m_Meas1, QProd, som_ALL));
    case Op_PrdConc:   return GetFlowValue(m_Meas1, QProd)/GTZ(GetFlowValue(m_Meas2, QProd));
    case Op_PrdTemp:   return QProd.Temp();
    case Op_PrdTChg:   return QProd.Temp()-m_TempFeed;
    }
  _asm int 3;
  return dNAN;
  }

//-------------------------------------------------------------------------

class CFeedMkUpFnd : public MRootFinder
  {
  public:
    CFeedMkUpFnd(CXBlk_MUBase * pMU, LPCTSTR pTag, SpConduit * pIn, SpConduit * pSrc, SpConduit * pSrcWrk, SpConduit * pPrd, double TRqd, double PRqd, CToleranceBlock & Tol) : \
      m_pMU(pMU), m_pTag(pTag), m_MeasInfo(*pIn), m_Src(*pSrc), m_SrcWrk(*pSrcWrk), m_Prd(*pPrd), m_TRqd(TRqd), m_PRqd(PRqd), MRootFinder("MkUpFnd", Tol)
      {
      if (dbgMakeup)
        {
        dbgpln(" FeedSetPt:%20.6f %-25s ========================================================================",
          m_pMU->GetSetPoint(), m_pTag);
        }
      };
    LPCTSTR ObjTag() { return m_pTag; };
    double Function(double Qm)
      {
      m_Prd.QCopy(m_MeasInfo);
      m_SrcWrk.QSetM(m_Src, som_ALL, Qm);
      m_Prd.QAddF(m_SrcWrk, som_ALL, 1.0);

      m_TMix = m_Prd.Temp();
      if (Valid(m_TRqd))
        m_Prd.SetTempPress(m_TRqd, m_PRqd);

      m_Measured = m_pMU->GetMeasVal(m_MeasInfo, m_SrcWrk, m_Prd);

      if (dbgMakeup)
        {
        dbgpln("      Meas:%20.6f Qm:%20.6f In:%20.6f Src:%20.6e %s",
          m_Measured, Qm, m_MeasInfo.QMass(), m_Src.QMass(), m_pTag);
        }

      return m_Measured;
      };

    double TMix()       { return m_TMix; }
    double Measured()   { return m_Measured; };

  protected:
    CXBlk_MUBase  * m_pMU;
    LPCTSTR         m_pTag;
    double          m_TRqd;
    double          m_PRqd;
    double          m_TMix;
    double          m_Measured;
    SpConduit     & m_MeasInfo;
    SpConduit     & m_Src;
    SpConduit     & m_Prd;
    SpConduit     & m_SrcWrk;
  };

//--------------------------------------------------------------------------

void CXBlk_MUBase::EvalProducts(SpConduit &QProd, double Po, double FinalTEst)
  {
  FlwNode *pNd=FindObjOfType((FlwNode*)NULL);
  ASSERT_ALWAYS(pNd!=0, "Should always be part of a FlwNode", __FILE__, __LINE__);

  switch (m_Op)
    {
    case Op_PrdComp:
      m_MeasFeed = GetFlowValue(m_Meas1, QProd)/GTZ(GetFlowValue(m_Meas1, QProd, som_ALL));
      break;
    case Op_PrdConc:
      m_MeasFeed = GetFlowValue(m_Meas1, QProd)/GTZ(GetFlowValue(m_Meas2, QProd));
      break;
    case Op_PrdTemp:
    case Op_PrdTChg:
      m_MeasFeed = QProd.Temp();
      break;
    default:
      m_MeasFeed = GetFlowValue(m_Meas1, QProd);
      break;
    };

  m_QmFeed = QProd.QMass();
  m_QvFeed = QProd.QVolume();
  m_TempFeed = QProd.Temp();
  double HzIn = QProd.totHz();

  StkSpConduit QIn("QIn", "MkUp", pNd);
  QIn().QCopy(QProd);

  m_bHasFlow = (m_QmFeed>SmallPosFlow);
  m_bStopMakeUp = m_pMakeupBase->On() && (m_eLoFeedOpt>LF_AlwaysOn) && (m_QmFeed<m_LoFeedQm);

  StkSpConduit QMkUp("QMkUp", "SrcWrk", pNd);
  SpConduit &QSrc=SrcIO.Cd;

  m_TempMkUp = QSrc.Temp();

  bool CI12Set=false;
  switch (m_Op)
    {
    case Op_PrdComp:
      m_MeasMkUp = GetFlowValue(m_Meas1, QSrc)/GTZ(GetFlowValue(m_Meas1, QSrc, som_ALL));
      CI12Set = m_MeasMkUp<1e-6 && m_MeasFeed<1e-6;
      break;
    case Op_PrdConc:
      m_MeasMkUp = GetFlowValue(m_Meas1, QSrc)/GTZ(GetFlowValue(m_Meas2, QSrc));
      CI12Set = m_MeasMkUp<1e-6 && m_MeasFeed<1e-6;
      break;
    };

  SetCI(12, CI12Set);

  bool CIsOn[5]={false,false,false,false,false};
  if (m_pMakeupBase->On() && !CI12Set && !m_bStopMakeUp && m_ValidateOK)
    {
    // Copy to Src if Self
    if (m_eSource==Src_Self)
      QSrc.QSetF(QProd, som_ALL, 1.0);

    double TReqd;
    switch (m_eRqdTemp)
      {
      case Temp_Inlet:
        TReqd=QProd.Temp();
        break;
      case Temp_Source:
        TReqd=QSrc.Temp();
        break;
      case Temp_Std:
        TReqd=StdT;
        break;
      case Temp_Mixture:
        TReqd=dNAN;//StdT; //??????
        break;
      case Temp_Const:
        TReqd=Range(CDB.MinT, m_RqdTemp, CDB.MaxT);
        break;
      default:
        TReqd=QProd.Temp();
      }

    CFeedMkUpFnd MkUpFnd(this, FullObjTag(), &QIn(), &QSrc, &QMkUp(), &QProd, TReqd, Po, sm_QmTol);
    const double Estimate = Range(m_MUQmMin, m_QmMkUp, m_MUQmMax);
    int iRet=MkUpFnd.FindRoot(GetSetPoint(), m_MUQmMin, m_MUQmMax, Estimate, 0.0, true);
    switch (iRet)
      {
      case RF_OK:
        m_QmMkUp = MkUpFnd.Result();
        break;
      case RF_LoLimit:
      case RF_EstimateLoLimit:
        m_QmMkUp = MkUpFnd.Result();
        CIsOn[2]=true;
        break;
      case RF_HiLimit:
      case RF_EstimateHiLimit:
        m_QmMkUp = MkUpFnd.Result();
        CIsOn[3]=true;
        break;
      case RF_Independant:
        MkUpFnd.Function(0);
        m_QmMkUp = MkUpFnd.Result();
        CIsOn[4]=true;
        break;

      default:
        //CIsOn[1]=true;
        //SetCI(1, "E\tConverge Error [%i]", iRet);
        LogError(Tag(), 0, "Converge Error [%i]", iRet);
        break;
      }

    //QSrc.QAdjustQmTo(som_ALL, m_dQmMakeup);
    QSrc.QCopy(QMkUp());
    m_HeatFlow   = QProd.totHf() - QProd.totHf(som_ALL, MkUpFnd.TMix(), QProd.Press());
    m_Measured   = MkUpFnd.Measured();

    if (SrcIO.Enabled)
      SrcIO.Sum.Set(QSrc);
    else
      SrcIO.Sum.ZeroFlows();

    }
  else
    {
    m_HeatFlow   = 0;
    SrcIO.Cd.QZero();
    SrcIO.Sum.ZeroFlows();
    m_Measured   = dNAN;
    }

  //SetCI(11, m_bStopMakeUp);

  for (int i=2; i<sizeof(CIsOn)/sizeof(CIsOn[0]); i++)
    SetCI(i, CIsOn[i]);

  m_SetPoint   = GetSetPoint();
  m_Result     = GetMeasVal(QIn(), QSrc, QProd);
  m_QmProd     = QProd.QMass();
  m_QmMkUp     = m_QmProd-m_QmFeed;
  m_QvProd     = QProd.QVolume();
  m_QvMkUp     = QMkUp().QVolume();
  m_TempProd   = QProd.Temp();

  switch (m_Op)
    {
    case Op_MUFixed:
      m_MeasMkUp = GetFlowValue(m_Meas1, QMkUp());
      m_MeasProd = GetFlowValue(m_Meas1, QProd);
      break;
    case Op_MURatio:
      m_MeasMkUp = GetFlowValue(m_Meas2, QMkUp());
      m_MeasProd = GetFlowValue(m_Meas1, QProd);
      break;
    case Op_PrdFlow:
      m_MeasMkUp = GetFlowValue(m_Meas1, QMkUp());
      m_MeasProd = GetFlowValue(m_Meas1, QProd);
      break;
    case Op_PrdComp:
      m_MeasMkUp = GetFlowValue(m_Meas1, QMkUp())/GTZ(GetFlowValue(m_Meas1, QMkUp(), som_ALL));
      m_MeasProd = GetFlowValue(m_Meas1, QProd)/GTZ(GetFlowValue(m_Meas1, QProd, som_ALL));
      break;
    case Op_PrdConc:
      m_MeasMkUp = GetFlowValue(m_Meas1, QMkUp())/GTZ(GetFlowValue(m_Meas2, QMkUp()));
      m_MeasProd = GetFlowValue(m_Meas1, QProd)/GTZ(GetFlowValue(m_Meas2, QProd));
      break;
    case Op_PrdTemp:
    case Op_PrdTChg:
      m_MeasMkUp = QMkUp().Temp();
      m_MeasProd = QProd.Temp();
      break;
    };

  if (SrcIO.MyConnectedIO()>=0)
    pNd->SetIOQm_In(SrcIO.MyConnectedIO(), m_QmMkUp);
  };

void CXBlk_MUBase::EvalProductsInline(SpConduit & Qf, double Len, double Diam, double Po, double FinalTEst)
  {
  EvalProducts(Qf, Po, FinalTEst);
  };

//--------------------------------------------------------------------------

flag CXBlk_MUBase::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  int i=No-CBContext();
  switch (i)
    {
    //case  1: pS="E\tConverge Error"; return 1;
    case   2: pS="W\tRequirement not Achieved - Low Limit"; return 1;
    case   3: pS="W\tRequirement not Achieved - High Limit"; return 1;
    case   4: pS="E\tMakeup has No Effect"; return 1;
    case   5:
    case   6:
    case   7: pS = m_Meas1.CIStr(i-5); return 1;
    case   8:
    case   9:
    case  10: pS = m_Meas2.CIStr(i-8); return 1;
    case  11: pS="N\tLow Feed - Makeup Stopped"; return 1;
    case  12: pS="W\tMakeup appears poorly configured"; return 1;
    default:
      return CMakeupBlock::CIStrng(No, pS);
    }
  }

//============================================================================
//
//
//
//============================================================================

#define DllImportExport /* */

//=========================================================================
//
//
//
//=========================================================================
