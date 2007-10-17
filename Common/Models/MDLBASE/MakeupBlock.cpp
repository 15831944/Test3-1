#include "stdafx.h"

#define __MAKEUPBLOCK_CPP
#include "MakeupBlock.h"
#include "nrecipe.h"

//#include "optoff.h"

#define dbgMakeup  01

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

void CMakeupBlock::EvalProductsInline(SpConduit & Qf, double Len, double Diam, double Po, double FinalTEst)
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
  m_pNd=dynamic_cast<FlwNode*>(pAttach); 
  m_fEnabled=false;
  m_fFixed=false; 
  m_SrcIO.UsrEnable=true;//false;
  m_ReconfigureReqd=false;
  //m_Index=Index; 
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
      if (m_SrcIO.Enabled)
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
          m_ReconfigureReqd=false;
          Open(pC);
          m_pMakeupB->SetTag(Name());
          }
        else
          {
          m_ReconfigureReqd=true;
          }
        m_SrcIO.UsrEnable = m_pMakeupB ? m_pMakeupB->DoesSomething() && Enabled() : false;
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
    return 0;
  if (m_ReconfigureReqd)
    {
    m_pNd->SetValidateFailMsg("E\tMakeup needs Configuration");
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
XID xidMkPhaseB  = AdjustXID(1001);
XID xidMkSelect  = AdjustXID(1002);
XID xidMkAll     = AdjustXID(1003);
XID xidMkSolids  = AdjustXID(1004);
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
XID xidMkError   = AdjustXID(1017);
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
  friend class CXBlk_MUFeed;
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

    void            BuildDataDefn(DataDefnBlk& DDB, CXBlk_MUFeed &Blk);
    flag            DataXchg(DataChangeBlk & DCB, CXBlk_MUFeed &Blk);
    flag            ValidateData(ValidateDataBlk & VDB, CXBlk_MUFeed &Blk);

    LPSTR          CIStr(int Which)
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

    Strng           m_MeasDesc;
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

class DllImportExport CXBlk_MUFeed: public CMakeupBlock
  {
  public:
    enum eOps  { Op_MUFixed, Op_MURatio, Op_PrdFlow, Op_PrdFrac, Op_PrdConc };

  public:
    CXBlk_MUFeed(eOps Op, TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CXBlk_MUFeed();

    void           ClrMeasEtc();

    virtual flag   DoesSomething() { return true; };

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void   EvalProductsInline(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

    virtual double  Duty() { return 0.0; };

    double         GetSetPoint();
    double         GetMeasVal(SpConduit &QIn, SpConduit &QSrc, SpConduit &QPrd);
    double         GetFlowValue(CMeasInfo &MI, SpConduit &Q, PhMask PhRqd=0);

    DEFINE_CI(CXBlk_MUFeed, CMakeupBlock, 12);

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

    double          m_RqdSetPoint;

    //results
    double          m_SetPoint;
    double          m_Measured;
    double          m_Result;
    //double          m_FeedAct;
    //double          m_ProdAct;
    //double          m_MakeupAct;

    double          m_QmFeed;
    double          m_QmMakeup;
    double          m_QmProd;
    //double          m_QvFeed;
    //double          m_QvMakeup;
    //double          m_QvProd;

    eTemp           m_eRqdTemp;
    double          m_RqdTemp;

    double          m_TempFeed;
    double          m_TempMakeup;
    double          m_TempProd;
    double          m_HeatFlow;

    bool            m_ValidateOK;
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

void CMeasInfo::BuildDataDefn(DataDefnBlk& DDB, CXBlk_MUFeed &Blk)//, LPTSTR Tag, DWORD UserInfo, LPSTR Header)
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

    if (m_eType==Type_Mass)
      m_MeasDesc = "Mass flow";
    else if (m_eType==Type_Volume)
      m_MeasDesc = "Volume flow";
    else if (m_eType==Type_NVolume)
      m_MeasDesc = "NormVolume flow";
    else if (m_eType==Type_Mole)
      m_MeasDesc = "Molar flow";
    //else if (Blk.m_eType==CXBlk_MUFeed::Type_Conc)
    //  m_MeasDesc = "Concentration";
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
        char Buff[256];
        sprintf(Buff, " of sum of %d selected species:", m_Species.GetSize());
        m_MeasDesc += Buff; 
        break;
        }
      case Slct_Component:
        {
        char Buff[256];
        sprintf(Buff, " of sum of %d selected components:", m_Comps.GetSize());
        m_MeasDesc += Buff; 
        break;
        }
      case Slct_Element:
        {
        char Buff[256];
        sprintf(Buff, " of sum of %d selected elements:", m_Elements.GetSize());
        m_MeasDesc += Buff; 
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
          //SetUpDDBSpcs(true);
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
          //SetUpDDBSpcs(true);
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


//============================================================================
//
//
//
//============================================================================

class DllImportExport CXBlk_MUFixed : public CXBlk_MUFeed
  {
  public:
    CXBlk_MUFixed(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUFeed(Op_MUFixed, pClass_, Tag_, pAttach, eAttach)  
      {
      m_Meas1.Initialise("Makeup", 1, "Makeup:Measurement");
      };
    virtual ~CXBlk_MUFixed() {};
  };

class DllImportExport CXBlk_MURatio : public CXBlk_MUFeed
  {
  public:
    CXBlk_MURatio(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUFeed(Op_MURatio, pClass_, Tag_, pAttach, eAttach)  
      {
      m_Meas1.Initialise("Feed", 1, "Feed:Measurement");
      m_Meas2.Initialise("Makeup", 2, "Makeup:Measurement");
      };
    virtual ~CXBlk_MURatio() {};
  };


class DllImportExport CXBlk_PrdFlow: public CXBlk_MUFeed
  {
  public:
    CXBlk_PrdFlow(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUFeed(Op_PrdFlow, pClass_, Tag_, pAttach, eAttach)  
      {
      m_Meas1.Initialise("Product", 1, "Product:Measurement");
      };
    virtual ~CXBlk_PrdFlow() {};
  };

class DllImportExport CXBlk_PrdFrac: public CXBlk_MUFeed
  {
  public:
    CXBlk_PrdFrac(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUFeed(Op_PrdFrac, pClass_, Tag_, pAttach, eAttach)
      {
      m_Meas1.Initialise("Product", 1, "Product:Measurement", Type_Mass, Slct_Occur, som_Liq);
      };
    virtual ~CXBlk_PrdFrac() {};
  };

class DllImportExport CXBlk_PrdConc: public CXBlk_MUFeed
  {
  public:
    CXBlk_PrdConc(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) : \
      CXBlk_MUFeed(Op_PrdConc, pClass_, Tag_, pAttach, eAttach) 
      {
      m_Meas1.Initialise("ProductMass", 1, "Product:Mass Measurement", Type_Mass, Slct_Specie);
      m_Meas2.Initialise("ProductVolume", 2, "Product:Volume Measurement", Type_Volume, Slct_Occur, som_Liq);
      };
    virtual ~CXBlk_PrdConc() {};
  };

DEFINE_MAKEUPBLOCK(CXBlk_MUFixed);
DEFINE_MAKEUPBLOCK(CXBlk_MURatio);
DEFINE_MAKEUPBLOCK(CXBlk_PrdFlow);
DEFINE_MAKEUPBLOCK(CXBlk_PrdFrac);
DEFINE_MAKEUPBLOCK(CXBlk_PrdConc);

//============================================================================
//
//
//
//============================================================================

CToleranceBlock CXBlk_MUFeed::sm_QmTol(TBF_DynSys, "MUFeed:CtrlEPS", 1.0e-8, 1.0e-8, 200);

IMPLEMENT_MAKEUPBLOCK(CXBlk_MUFixed, "MB_Fixed", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Fixed Makeup",  " ");
IMPLEMENT_MAKEUPBLOCK(CXBlk_MURatio, "MB_Ratio", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Ratio Makeup",  " ");
IMPLEMENT_MAKEUPBLOCK(CXBlk_PrdFlow, "MB_PrdFlow", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Product Flow",  " ");
IMPLEMENT_MAKEUPBLOCK(CXBlk_PrdFrac, "MB_PrdFrac", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Product Fraction",  " ");
IMPLEMENT_MAKEUPBLOCK(CXBlk_PrdConc, "MB_PrdConc", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Product Concentration",  " ");

CXBlk_MUFeed::CXBlk_MUFeed(eOps Op, pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
CMakeupBlock(pClass_, Tag_, pAttach, eAttach)
  {
  m_Op        = Op; 
  m_eSource   = Src_Remote;

  m_MUQmMin   = 0.0;
  m_MUQmMax   = 100.0;

  m_eLoFeedOpt= LF_AlwaysOn;
  m_LoFeedQm  = 1.0;

  m_RqdSetPoint      = 0;

  m_bHasFlow  = true;

  m_SetPoint   = 0.0;
  m_Measured   = 0.0;
  m_Result     = 0.0;
  m_QmFeed     = 0.0;
  m_QmMakeup   = 0.0;
  m_QmProd     = 0.0;
  //m_QvFeed     = 0.0;
  //m_QvMakeup   = 0.0;
  //m_QvProd     = 0.0;

  m_eRqdTemp    = Temp_Mixture;
  m_RqdTemp     = C2K(25);

  m_TempFeed   = C2K(25);
  m_TempMakeup = C2K(25);
  m_TempProd   = C2K(25); 
  m_HeatFlow    = 0;

  m_ValidateOK  = true;

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
      {LF_StopBelow,        "StopBelowQmMin"      },
      //{LF_HoldMeasure,      "HoldAtLo"          },
      {}
    };

  DDB.Text(" ");
  DDB.Text("Requirement");
  switch (m_Op)
    {
    case Op_MURatio:
      {
      DDB.Long       ("", "Rqd.Basis",    DC_,  "", xidMkType,  this, isParmStopped|SetOnChange, DDBCtrlRatio);
      switch (m_Meas1.m_eType)
        {
        case Type_Mass:     
          DDB.Double("", "Rqd.QmRatio",    DC_Frac, ".",  &m_RqdSetPoint,  this, isParm); 
          DDB.Double("", "Meas.QmRatio",   DC_Frac, ".",  &m_Measured,  this, isResult); 
          break;
        case Type_Mole:     
          DDB.Double("", "Rqd.QMlRatio",   DC_Frac, ".",  &m_RqdSetPoint,  this, isParm); 
          DDB.Double("", "Meas.QMlRatio",  DC_Frac, ".",  &m_Measured,  this, isResult); 
          break;
        case Type_Volume:   
          DDB.Double("", "Rqd.QvRatio",    DC_Frac, ".",  &m_RqdSetPoint,  this, isParm); 
          DDB.Double("", "Meas.QvRatio",   DC_Frac, ".",  &m_Measured,  this, isResult); 
          break;
        case Type_NVolume:  
          DDB.Double("", "Rqd.NQvRatio",   DC_Frac, ".",  &m_RqdSetPoint,  this, isParm); 
          DDB.Double("", "Meas.NQvRatio",  DC_Frac, ".",  &m_Measured,  this, isResult); 
          break;
        }
      break;
      }
    case Op_MUFixed:
    case Op_PrdFlow:
      {
      DDB.Long       ("", "Rqd.Basis",             DC_,  "", xidMkType,  this, isParmStopped|SetOnChange, DDBCtrlFlw);
      switch (m_Meas1.m_eType)
        {
        case Type_Mass:    
          DDB.Double("", "Rqd.Qm",    DC_Qm,     "kg/s",   &m_RqdSetPoint,  this, isParm);  
          DDB.Double("", "Meas.Qm",   DC_Qm,     "kg/s",   &m_Measured,     this, isResult);  
          break;
        case Type_Mole:    
          DDB.Double("", "Rqd.QMl",   DC_QKgMl,  "kmol/s", &m_RqdSetPoint,  this, isParm);  
          DDB.Double("", "Meas.QMl",  DC_QKgMl,  "kmol/s", &m_Measured,     this, isResult);  
          break;
        case Type_Volume:  
          DDB.Double("", "Rqd.Qv",    DC_Qv,     "m^3/s",  &m_RqdSetPoint,  this, isParm);  
          DDB.Double("", "Meas.Qv",   DC_Qv,     "m^3/s",  &m_Measured,     this, isResult);  
          break;
        case Type_NVolume: 
          DDB.Double("", "Rqd.NQv",   DC_NQv,    "Nm^3/s", &m_RqdSetPoint,  this, isParm);  
          DDB.Double("", "Meas.NQv",  DC_NQv,    "Nm^3/s", &m_Measured,     this, isResult);  
          break;
        }
      break;
      }
    case Op_PrdFrac:
      {
      DDB.Long       ("", "Rqd.Basis",             DC_,  "", xidMkType,  this, isParmStopped|SetOnChange, DDBCtrl);
      switch (m_Meas1.m_eType)
        {
        case Type_Mass:     
          DDB.Double("", "Rqd.QmFrac",    DC_Frac, "%",  &m_RqdSetPoint,  this, isParm); 
          DDB.Double("", "Meas.QmFrac",   DC_Frac, "%",  &m_Measured,     this, isResult); 
          break;
        case Type_Mole:     
          DDB.Double("", "Rqd.QMlFrac",   DC_Frac, "%",  &m_RqdSetPoint,  this, isParm); 
          DDB.Double("", "Meas.QMlFrac",  DC_Frac, "%",  &m_Measured,     this, isResult); 
          break;
        case Type_Volume:   
          DDB.Double("", "Rqd.QvFrac",    DC_Frac, "%",  &m_RqdSetPoint,  this, isParm); 
          DDB.Double("", "Meas.QvFrac",   DC_Frac, "%",  &m_Measured,     this, isResult); 
          break;
        case Type_NVolume:  
          DDB.Double("", "Rqd.NQvFrac",   DC_Frac, "%",  &m_RqdSetPoint,  this, isParm); 
          DDB.Double("", "Meas.NQvFrac",  DC_Frac, "%",  &m_Measured,     this, isResult); 
          break;
        }
      break;
      }
    case Op_PrdConc:
      {
      DDB.Double("", "Rqd.Conc",     DC_Conc, "g/L",  &m_RqdSetPoint,     this, isParm); 
      DDB.Double("", "Meas.Conc",    DC_Conc, "g/L",  &m_Measured,        this, isResult); 
      break;
      }
    default:;
    }

  DDB.Visibility();
  DDB.Text(" ");
  m_Meas1.BuildDataDefn(DDB, *this);

  switch (m_Op)
    {
    case Op_MURatio:
    case Op_PrdConc:
      DDB.Text(" ");
      m_Meas2.BuildDataDefn(DDB, *this);
      break;
    }

  DDB.Visibility();
  DDB.Text(" ");
  DDB.Text("MakeUp:Limits");
  //DDB.Long       ("", "Source",           DC_,   "", (long*)&m_eSource,  this, isParm|SetOnChange, DDBSource);
  DDB.Double     ("Makeup.QmMin", "",            DC_Qm, "kg/s", &m_MUQmMin, this, isParm);
  DDB.Double     ("Makeup.QmMax", "",            DC_Qm, "kg/s", &m_MUQmMax, this, isParm);

  DDB.Text(" ");
  DDB.Text("MakeUp:Low Flow");
  DDB.Long       ("Feed.LoFlowRule",   "",         DC_,   "", (long*)&m_eLoFeedOpt, this, isParm|SetOnChange, DDBLoFeed);
  DDB.Visibility (NSHM_All, m_eLoFeedOpt >LF_AlwaysOn);
  DDB.Double     ("Feed.LoFlowLimit",    "",        DC_Qm, "kg/s", &m_LoFeedQm, this, isParm);
  DDB.Visibility ();

  if (!HeatSkipMethod())
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
    DDB.Text("Product:Temperature");
    DDB.Long  ("Temp.Final",      "",  DC_,  "", (long*)&m_eRqdTemp, this, isParm|SetOnChange, DDBTemp);
    DDB.Visibility(NSHM_All, m_eRqdTemp==Temp_Const);
    DDB.Double("Temp.Reqd",        "",  DC_T, "C", &m_RqdTemp, this, isParm);
    DDB.Visibility();
    }

  DDB.Text(" ");
  //DDB.Text("Results");

  switch (m_Op)
    {
    case Op_MURatio: DDB.Text("Results:Makeup Ratio:"); break;
    case Op_MUFixed: DDB.Text("Results:Makeup Fixed:"); break;
    case Op_PrdFlow: DDB.Text("Results:Product Flow:"); break;
    case Op_PrdFrac: DDB.Text("Results:Product Fraction:"); break;
    case Op_PrdConc: DDB.Text("Results:Product Concentration:"); break;
    }
  DDB.String("Error",            "", DC_,   "",    xidMkError,     this, isResult);

  //CCnvIndex CnvUsed;
  //Strng CnvTxt;
  //switch (m_Op)
  //  {
  //  case Op_MURatio: 
  //    CnvUsed=DC_Frac; 
  //    CnvTxt="%"; 
  //    break;
  //  case Op_MUFixed: 
  //  case Op_PrdFlow: 
  //    switch (m_eType)
  //      {                         
  //      case Type_Mass    : CnvUsed=DC_Qm; CnvTxt="kg/s"; break;
  //      case Type_Mole    : CnvUsed=DC_QKgMl; CnvTxt="kmol/s"; break;
  //      case Type_Volume  : CnvUsed=DC_Qv; CnvTxt="m^3/s"; break;
  //      case Type_NVolume : CnvUsed=DC_NQv; CnvTxt="Nm^3/s"; break;
  //      }
  //    break;
  //  default: _asm int 3;
  //  }

  //DDB.Text(" ");
  //switch (m_Op)
  //  {
  //  case Op_MURatio: 
  //    DDB.Text(m_Meas1.m_MeasDesc());
  //    if (MassBasis)
  //      {
  //      DDB.Double ("Meas.Qm.Feed",   "", DC_Qm,    "kg/s",     &m_FeedAct,     this, isResult|NAN_OK);
  //      DDB.Double ("Meas.Qm.Prod",   "", DC_Qm,    "kg/s",     &m_ProdAct,     this, isResult|NAN_OK);
  //      }
  //    else if (VolBasis)
  //      {
  //      DDB.Double ("Meas.Qv.Feed",   "", DC_Qv,    "m^3/s",    &m_FeedAct,     this, isResult|NAN_OK);
  //      DDB.Double ("Meas.Qv.Prod",   "", DC_Qv,    "m^3/s",    &m_ProdAct,     this, isResult|NAN_OK);
  //      }
  //    else if (NVolBasis)
  //      {
  //      DDB.Double ("Meas.NQv.Feed",  "", DC_NQv,   "Nm^3/s",   &m_FeedAct,     this, isResult|NAN_OK);
  //      DDB.Double ("Meas.NQv.Prod",  "", DC_NQv,   "Nm^3/s",   &m_ProdAct,     this, isResult|NAN_OK);
  //      }
  //    else if (MoleBasis)
  //      {
  //      DDB.Double ("Meas.QMl.Feed",  "", DC_QKgMl, "kmol/s",   &m_FeedAct,     this, isResult|NAN_OK);
  //      DDB.Double ("Meas.QMl.Prod",  "", DC_QKgMl, "kmol/s",   &m_ProdAct,     this, isResult|NAN_OK);
  //      }
  //    break;
  //  }

  //DDB.Text(m_Meas.m_MeasDesc() ? m_MkUpMeas.m_MeasDesc():"");
  //if (MassBasis)
  //  DDB.Double ("Meas.Qm.Makeup",   "", DC_Qm,    "kg/s",     &m_MakeupAct,     this, isResult|NAN_OK);
  //else if (VolBasis)
  //  DDB.Double ("Meas.Qv.Makeup",   "", DC_Qv,    "m^3/s",    &m_MakeupAct,     this, isResult|NAN_OK);
  //else if (NVolBasis)
  //  DDB.Double ("Meas.NQv.Makeup",  "", DC_NQv,   "Nm^3/s",   &m_MakeupAct,     this, isResult|NAN_OK);
  //else if (MoleBasis)
  //  DDB.Double ("Meas.QMl.Makeup",  "", DC_QKgMl, "kmol/s",   &m_MakeupAct,     this, isResult|NAN_OK);

  DDB.Text(" ");
  DDB.Text("Total mass flow:");
  DDB.Double ("Qm.Feed",            "", DC_Qm,    "kg/s",     &m_QmFeed,     this, isResult);
  DDB.Double ("Qm.Makeup",          "", DC_Qm,    "kg/s",     &m_QmMakeup,   this, isResult);
  DDB.Double ("Qm.Prod",            "", DC_Qm,    "kg/s",     &m_QmProd,     this, isResult);
  //DDB.Text("Total volume flow:");
  //DDB.Double ("Qv.Feed",            "", DC_Qv,    "m^3/s",    &m_QvFeed,     this, isResult|InitHidden);
  //DDB.Double ("Qv.Makeup",          "", DC_Qv,    "m^3/s",    &m_QvMakeup,   this, isResult);
  //DDB.Double ("Qv.Prod",            "", DC_Qv,    "m^3/s",    &m_QvProd,     this, isResult|InitHidden);
  if (!HeatSkipMethod())
    {
    DDB.Text(" ");
    DDB.Text("Total heat flow:");
    DDB.Double ("Temp.Feed",          "", DC_T,    "C",       &m_TempFeed,  this, isResult);
    DDB.Double ("Temp.Makeup",        "", DC_T,    "C",       &m_TempMakeup, this, isResult);
    DDB.Double ("Temp.Prod",          "", DC_T,    "C",       &m_TempProd,  this, isResult);
    DDB.Double ("HeatFlow",           "", DC_Pwr,  "kW",      &m_HeatFlow,   this, isResult);
    }

  }

//--------------------------------------------------------------------------

void CXBlk_MUFeed::ClrMeasEtc()
  {
  m_Measured = dNAN;
  m_SetPoint = dNAN;
  m_Result   = dNAN;
  //m_FeedAct  = dNAN;
  //m_ProdAct  = dNAN;
  //m_MakeupAct= dNAN;
  }

flag CXBlk_MUFeed::DataXchg(DataChangeBlk & DCB)
  {
  if (DCB.dwUserInfo==1 && m_Meas1.DataXchg(DCB, *this))
    return 1;
  if (DCB.dwUserInfo==2 && m_Meas2.DataXchg(DCB, *this))
    return 1;
  //if (DCB.dwUserInfo==3 && m_MkUpMeas.DataXchg(DCB, *this))
  //  return 1;

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

    case xidMkError:
      {
      m_ErrorLst.Clear();
      ConditionBlk::GetMyCIs(m_ErrorLst,3);
      if (!SrcIO.Connected)
        {
        DCB.pC="Not Connected";
        }
      else if (!m_bHasFlow && (m_Op==Op_MURatio))
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

double CXBlk_MUFeed::GetSetPoint()
  {
  return m_RqdSetPoint;
  }

//--------------------------------------------------------------------------

double CXBlk_MUFeed::GetFlowValue(CMeasInfo &MI, SpConduit &Q, PhMask PhRqd/*=0*/)
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

double CXBlk_MUFeed::GetMeasVal(SpConduit &QFeed, SpConduit &QMu, SpConduit &QProd)
  {
  switch (m_Op)
    {
    case Op_MUFixed:   return GetFlowValue(m_Meas1, QMu);
    case Op_MURatio:   return GetFlowValue(m_Meas2, QMu)/GTZ(GetFlowValue(m_Meas1, QFeed));
    case Op_PrdFlow:   return GetFlowValue(m_Meas1, QProd);
    case Op_PrdFrac:   return GetFlowValue(m_Meas1, QProd)/GTZ(GetFlowValue(m_Meas1, QProd, som_ALL));
    case Op_PrdConc:   return GetFlowValue(m_Meas1, QProd)/GTZ(GetFlowValue(m_Meas2, QProd));
    }
  _asm int 3;
  return dNAN;
  }

//-------------------------------------------------------------------------

class CFeedMkUpFnd : public MRootFinder
  {
  public:
    CFeedMkUpFnd(CXBlk_MUFeed * pMU, LPCTSTR pTag, SpConduit * pIn, SpConduit * pSrc, SpConduit * pSrcWrk, SpConduit * pPrd, double TRqd, double PRqd, CToleranceBlock & Tol) : \
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
    CXBlk_MUFeed  * m_pMU;
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

void CXBlk_MUFeed::EvalProducts(SpConduit &QPrd, double Po, double FinalTEst)
  {
  FlwNode *pNd=FindObjOfType((FlwNode*)NULL);
  ASSERT_ALWAYS(pNd!=0, "Should always be part of a FlwNode", __FILE__, __LINE__);

  m_QmFeed = QPrd.QMass();
  //m_QvFeed = QPrd.QVolume();
  m_TempFeed = QPrd.Temp();
  double HzIn = QPrd.totHz();

  m_bHasFlow = (m_QmFeed>SmallPosFlow);
  bool StopMakeUp = (m_eLoFeedOpt>LF_AlwaysOn) && (m_QmFeed<m_LoFeedQm); 

  StkSpConduit QIn("QIn", "MkUp", pNd);
  QIn().QCopy(QPrd);

  StkSpConduit QSrcWrk("QSrcWrk", "SrcWrk", pNd);
  SpConduit &QSrc=SrcIO.Cd;

  //m_Meas     = GetMeasVal(QIn(), QSrc, QPrd);
  //m_FeedAct  = dNAN;//GetFlowValue(m_FeedMeas, QIn());
  m_TempMakeup = QSrc.Temp();

  bool CIsOn[5]={false,false,false,false,false};
  if (!StopMakeUp && m_ValidateOK)
    {
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
        TReqd=dNAN;//StdT; //??????
        break;
      case Temp_Const:
        TReqd=Range(CDB.MinT, m_RqdTemp, CDB.MaxT);
        break;
      default:
        TReqd=QPrd.Temp();
      }

    CFeedMkUpFnd MkUpFnd(this, FullObjTag(), &QIn(), &QSrc, &QSrcWrk(), &QPrd, TReqd, Po, sm_QmTol);
    int iRet=MkUpFnd.FindRoot(GetSetPoint(), m_MUQmMin, m_MUQmMax, m_QmMakeup, 0.0, true);
    switch (iRet)
      {
      case RF_OK:         
        m_QmMakeup = MkUpFnd.Result();
        break;
      case RF_LoLimit:    
      case RF_EstimateLoLimit:    
        m_QmMakeup = MkUpFnd.Result();
        CIsOn[2]=true;
        break;
      case RF_HiLimit:    
      case RF_EstimateHiLimit:    
        m_QmMakeup = MkUpFnd.Result();   
        CIsOn[3]=true;
        break;
      case RF_Independant:
        MkUpFnd.Function(0);   
        m_QmMakeup = MkUpFnd.Result();   
        CIsOn[4]=true;
        break;

      default: 
        //CIsOn[1]=true;
        //SetCI(1, "E\tConverge Error [%i]", iRet);
        LogError(Tag(), 0, "Converge Error [%i]", iRet);
        break;
      }

    //QSrc.QAdjustQmTo(som_ALL, m_dQmMakeup);
    QSrc.QCopy(QSrcWrk());
    m_HeatFlow   = QPrd.totHf() - QPrd.totHf(som_ALL, MkUpFnd.TMix(), QPrd.Press());
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

  SetCI(11, StopMakeUp);

  for (int i=2; i<sizeof(CIsOn)/sizeof(CIsOn[0]); i++)
    SetCI(i, CIsOn[i]);

  m_SetPoint   = GetSetPoint();
  m_Result     = GetMeasVal(QIn(), QSrc, QPrd);
  m_QmProd     = QPrd.QMass();
  m_QmMakeup   = m_QmProd-m_QmFeed;
  //m_QvProd     = QPrd.QVolume();
  //m_QvMakeup   = QSrcWrk().QVolume();
  m_TempProd  = QPrd.Temp();

  //m_ProdAct    = dNAN;//GetFlowValue(m_ProdMeas, QPrd);
  //m_MakeupAct  = dNAN;//GetFlowValue(m_MkUpMeas, QSrcWrk());

  if (SrcIO.MyConnectedIO()>=0)
    pNd->SetIOQm_In(SrcIO.MyConnectedIO(), m_QmMakeup);

  };

void CXBlk_MUFeed::EvalProductsInline(SpConduit & Qf, double Len, double Diam, double Po, double FinalTEst)
  {
  EvalProducts(Qf, Po, FinalTEst);
  };

//--------------------------------------------------------------------------

flag CXBlk_MUFeed::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  int i=No-CBContext();
  switch (i)
    {
    //case  1: pS="E\tConverge Error"; return 1;
    case   2: pS="E\tRequirement not Achieved - Low Limit"; return 1;
    case   3: pS="E\tRequirement not Achieved - High Limit"; return 1;
    case   4: pS="E\tMakeup has No Effect"; return 1;
    case   5: 
    case   6: 
    case   7: pS = m_Meas1.CIStr(i-5); return 1;
    case   8: 
    case   9: 
    case  10: pS = m_Meas2.CIStr(i-8); return 1;
    case  11: pS="W\tLow Feed - Makeup Stopped"; return 1;
    default:
      return CXBlk_MUFeed::CIStrng(No, pS);
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
