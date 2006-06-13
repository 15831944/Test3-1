#include "stdafx.h"

#define __MAKEUPBLOCK_CPP
#include "MakeupBlock.h"

#if WITHBLOCKEVALUATOR

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

CMakeupBase::CMakeupBase(TaggedObject * pAttach, int Index) : CBlockEvalBase(BEId_Makeup, Index),
m_SrcIO(eDIO_Makeup, dynamic_cast<FlwNode*>(pAttach), false, true, 
     "Makeup", IOId_Makeup2Area+Index, IOId_AreaMakeupO, "MakeupSrc", "MakeupSrc_1")
  { 
  m_pMakeupB=NULL; 
  m_pNd=pAttach; 
  m_fEnabled=false;
  m_fFixed=false; 
  //m_Index=Index; 
  };
CMakeupBase::~CMakeupBase()  
  { 
  delete m_pMakeupB; 
  };

Strng CMakeupBase::Name()
  {
  Strng S;
  S.Set("M%i", m_Index+1);
  return S;
  }

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
  };
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
  DDB.Visibility(SHM_All, m_fEnabled);
  if (Enabled())//pHL)
    {
    DDB.Text("");
    if (DDB.BeginObject(m_pNd, Name()(), "EB_Makeup", pTagComment, PageIs))
      {
      if (m_SrcIO.Enabled)
        m_SrcIO.BuildDataDefn(DDB, NULL, DDB_NoPage, UserInfo+102, 0);//DFIO_ShowQm);

      DDBValueLstMem DDB0;
      TagObjClass::GetSDescValueLst(CMakeupBlock::GroupName, DDB0);
      DDB.String  ("Model",      "",       DC_    , "",      xidMakeupMdlNm  , m_pNd,m_fFixed ? 0 : isParm|SetOnChange, DDB0());

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
  };

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
          Open(*DCB.rB);
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
          Open(pC);
        else
          Close();
        }
      DCB.pC = m_pMakeupB ? m_pMakeupB->ShortDesc() : "";
      return 1;
    }
  return 0;
  }

//============================================================================
//
//
//
//============================================================================

#define DllImportExport /* */

//const unsigned long NF_Inlet    = NF_No1;
//const unsigned long NF_Source   = NF_No2;
//const unsigned long NF_Std      = NF_No3;
//const unsigned long NF_Mixture  = NF_No4;

class DllImportExport CXBlk_Makeup: public CMakeupBlock
  {
  public:
    //enum eWhat
    //  { 
    //  MW_All, 
    //  MM_Phase,
    //  MM_Specie,
    //  MM_VolumeFlow,
    //  MM_VolumeRatio,
    //  MM_NVolumeFlow,
    //  MM_NVolumeRatio
    //  };


    CXBlk_Makeup(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CXBlk_Makeup();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void   EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

  public:
    enum eSource
      {
      Src_Self,
      Src_Remote,
      };

    enum eType
      { 
      //Type_None, 
      Type_MassFlow, 
      Type_MassRatio, 
      Type_VolumeFlow, 
      Type_VolumeRatio, 
      Type_NVolumeFlow, 
      Type_NVolumeRatio,
      Type_MassFrac,
      Type_VolumeFrac,
      Type_NVolumeFrac,
      };
 
    enum eSelect
      {
      Slct_Phase,
      Slct_Specie,
      };

    enum eTemp
      {
      Temp_Inlet,
      Temp_Source,
      Temp_Std,
      Temp_Const,
      Temp_Mixture,
      };

    //bool IsByPhase()  {  return m_eType>=Type_MassFlowPh && m_eType<= Type_NVolumeFracPh;  };
    //bool IsBySpecie() {  return m_eType>=Type_MassFlowSp && m_eType<= Type_NVolumeFracSp;  };
    //bool TypeIn(eType X1, eType X2=Type_None, eType X3=Type_None, eType X4=Type_None, eType X5=Type_None, eType X6=Type_None)    
    //                  {  return m_eType==X1 || m_eType==X2 || m_eType==X3 || m_eType==X4 || m_eType==X5 || m_eType==X6; };


    eSource         m_eSource;
    eType           m_eType;
    eSelect         m_eSelect;
    PhMask          m_Phases;
    double          m_QmRqd;
    double          m_QvRqd;
    double          m_NQvRqd;
    double          m_QmRatio;
    double          m_QvRatio;
    double          m_NQvRatio;
    double          m_MassFrac;
    double          m_VolFrac;
    double          m_NVolFrac;

    eTemp           m_eRqdTemp;
    double          m_RqdTemp;
  };

DEFINE_MAKEUPBLOCK(CXBlk_Makeup);

//============================================================================
//
//
//
//============================================================================

XID xidMkAll     = AdjustXID(1000);
XID xidMkSolids  = AdjustXID(1001);
XID xidMkLiquids = AdjustXID(1002);
XID xidMkGasses  = AdjustXID(1003);
XID xidMkPhase   = AdjustXID(1004);

IMPLEMENT_MAKEUPBLOCK(CXBlk_Makeup, "MB_Simple", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Simple",  " ");

CXBlk_Makeup::CXBlk_Makeup(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
CMakeupBlock(pClass_, Tag_, pAttach, eAttach)
  {
  m_eSource    = Src_Remote;
  m_eType      = Type_MassRatio;
  m_eSelect    = Slct_Phase;
  m_Phases    = som_ALL;
  m_QmRqd     = 0;
  m_QvRqd     = 0;
  m_NQvRqd    = 0;
  m_QmRatio   = 0;
  m_QvRatio   = 0;
  m_NQvRatio  = 0;
  m_MassFrac  = 0;
  m_VolFrac   = 0;
  m_NVolFrac  = 0;
                       
  m_eRqdTemp  = Temp_Inlet;
  m_RqdTemp   = C2K(25);
  }

//--------------------------------------------------------------------------

CXBlk_Makeup::~CXBlk_Makeup()
  {
  }

//--------------------------------------------------------------------------

void CXBlk_Makeup::BuildDataDefn(DataDefnBlk& DDB)
  {

  //if (DDB.BeginStruct(this, "MakeupQm", NULL, DDB_NoPage))
  //  {
    static DDBValueLst DDBCtrl[] =
      {                         
        {Type_MassFlow,       "MassFlow"      },
        {Type_MassRatio,      "MassRatio"     },
        {Type_VolumeFlow,     "VolumeFlow"    },
        {Type_VolumeRatio,    "VolumeRatio"   },
        {Type_NVolumeFlow,    "NVolumeFlow"   },
        {Type_NVolumeRatio,   "NVolumeRatio"  },
        {Type_MassFrac,       "MassFrac"      },
        {Type_VolumeFrac,     "VolumeFrac"    },
        {Type_NVolumeFrac,    "NVolumeFrac"   },
        {}
      };
    static DDBValueLst DDBSelect[] =
      {                         
        {Slct_Phase,          "Phase"         },
        {Slct_Specie,         "Specie"        },
        {}
      };
    static DDBValueLst DDBSource[] =
      {                         
        {Src_Self,            "Self"          },
        {Src_Remote,          "Remote"        },
        {}
      };
    //static DDBValueLstMem DDBPhases;
    //if (DDBPhases.Length()==0)
    //  {
    //  DDBPhases.Add(som_ALL,       "All");
    //  DDBPhases.Add(som_Sol,       "Solids");
    //  DDBPhases.Add(som_Liq,       "Liquid");
    //  DDBPhases.Add(som_Gas,       "Gas");
    //  DDBPhases.Add(som_SL,        "Slurry");
    //                for (int p=0; p<CDB.PhaseCount(); p++)
    //                  {
    //                  CPhaseInfo &P=CDB.PhaseInfo(p);
    //                  if (P.m_eOcc==c)
    //                    DDB.Double(P.m_Tag(), P.m_Sym(),  DC_Frac, "%",  &A[j]->m_PhSplt[p],  this,
    //                    (j<NPri-1 ? isParm : 0)|(m_iPhMethSpec&GSPM_Total?NAN_OK:0)|ChildLevel(1));
    //                  }
    //  
    //  DDBPhases.Add();
    //    
    //    
    //    
    //    
    //    
    //  };

    //                  static const LPSTR Nm[]={"Solids","Liquids",/*"Aqueous",*/"Gasses"};
    //              if (m_iPhMethSpec&GSPM_Total)
    //                DDB.Double(Nm[c], "",  DC_Frac, "%",  &A[j]->m_Splt[c],  this, j<NPri-1 ? isParm : 0);
    //              if (m_iPhMethSpec&GSPM_Individual)
    //                {
    //                for (int p=0; p<CDB.PhaseCount(); p++)
    //                  {
    //                  CPhaseInfo &P=CDB.PhaseInfo(p);
    //                  if (P.m_eOcc==c)
    //                    DDB.Double(P.m_Tag(), P.m_Sym(),  DC_Frac, "%",  &A[j]->m_PhSplt[p],  this,
    //                    (j<NPri-1 ? isParm : 0)|(m_iPhMethSpec&GSPM_Total?NAN_OK:0)|ChildLevel(1));
    //                  }
    //                }

    DDB.Text(" ");
    DDB.Long       ("", "Type",             DC_,  "", (long*)&m_eType,  this, isParm|SetOnChange, DDBCtrl);
    //DDB.Text("Requirement");
    DDB.Visibility(SHM_All, m_eType==Type_MassFlow);
    DDB.Double("", "QmRqd",    DC_Qm, "kg/s", &m_QmRqd,  this, isParm);

    DDB.Visibility(SHM_All, m_eType==Type_VolumeFlow);
    DDB.Double("", "QvRqd",    DC_Qv, "m^3/s", &m_QvRqd,  this, isParm);

    DDB.Visibility(SHM_All, m_eType==Type_NVolumeFlow);
    DDB.Double("", "NQvRqd",    DC_NQv, "Nm^3/s", &m_NQvRqd,  this, isParm);

    DDB.Visibility(SHM_All, m_eType==Type_MassRatio);
    DDB.Double("", "QmRatio",    DC_Frac, "%", &m_QmRatio,  this, isParm);

    DDB.Visibility(SHM_All, m_eType==Type_VolumeRatio);
    DDB.Double("", "QvRatio",    DC_Frac, "%", &m_QvRatio,  this, isParm);

    DDB.Visibility(SHM_All, m_eType==Type_NVolumeRatio);
    DDB.Double("", "NQvRatio",    DC_Frac, "%", &m_NQvRatio,  this, isParm);

    DDB.Visibility(SHM_All, m_eType==Type_MassFrac);
    DDB.Double("", "MassFrac",    DC_Frac, "%", &m_MassFrac,  this, isParm);

    DDB.Visibility(SHM_All, m_eType==Type_VolumeFrac);
    DDB.Double("", "VolumeFrac",    DC_Frac, "%", &m_VolFrac,  this, isParm);

    DDB.Visibility(SHM_All, m_eType==Type_NVolumeFrac);
    DDB.Double("", "NVolumeFrac",    DC_Frac, "%", &m_NVolFrac,  this, isParm);

    DDB.Visibility();
    DDB.Text(" ");
    DDB.Long       ("", "Selection",        DC_,  "", (long*)&m_eSelect,  this, isParm|SetOnChange, DDBSelect);
    //DDB.Text("Selection");
    if (m_eSelect==Slct_Phase)
      {
      if (!DDB.ForFileSnpScn())
        {
        DDB.CheckBoxBtn("", "All",              DC_,  "", xidMkAll,     this, isParm);

        DDB.CheckBoxBtn("", "Solids",           DC_,  "", xidMkSolids,  this, isParm);
        if (CDB.PhaseCount(BOT_Solid)>1)
          {
          for (int o=CDB.PhaseFirst(BOT_Solid); o<=CDB.PhaseLast(BOT_Solid); o++)
            {
            CPhaseInfo & P=CDB.PhaseInfo(o);
            Strng T,S;
            T.Set("(%s)", P.m_Tag());
            S.Set("(%s)", P.m_Sym());
            DDB.CheckBoxBtn(T(), S(), DC_,  "", xidMkPhase+o,  this, isParm);
            //DDB.CheckBoxBtn(P.m_Tag(), P.m_Sym(), DC_,  "", xidMkPhase+o,  this, isParm);
            }
          }

        DDB.CheckBoxBtn("", "Liquids",          DC_,  "", xidMkLiquids, this, isParm);
        if (CDB.PhaseCount(BOT_Liquid)>1)
          {
          for (int o=CDB.PhaseFirst(BOT_Liquid); o<=CDB.PhaseLast(BOT_Liquid); o++)
            {
            CPhaseInfo & P=CDB.PhaseInfo(o);
            Strng T,S;
            T.Set("(%s)", P.m_Tag());
            S.Set("(%s)", P.m_Sym());
            DDB.CheckBoxBtn(T(), S(), DC_,  "", xidMkPhase+o,  this, isParm);
            //DDB.CheckBoxBtn(P.m_Tag(), P.m_Sym(), DC_,  "", xidMkPhase+o,  this, isParm);
            }
          }

        DDB.CheckBoxBtn("", "Gasses",           DC_,  "", xidMkGasses,  this, isParm);
        if (CDB.PhaseCount(BOT_Gas)>1)
          {
          for (int o=CDB.PhaseFirst(BOT_Gas); o<=CDB.PhaseLast(BOT_Gas); o++)
            {
            CPhaseInfo & P=CDB.PhaseInfo(o);
            Strng T,S;
            T.Set("(%s)", P.m_Tag());
            S.Set("(%s)", P.m_Sym());
            DDB.CheckBoxBtn(T(), S(), DC_,  "", xidMkPhase+o,  this, isParm);
            //DDB.CheckBoxBtn(P.m_Tag(), P.m_Sym(), DC_,  "", xidMkPhase+o,  this, isParm);
            }
          }
        }  
      if (1)//DDB.ForFileSnpScn())
        {
        DDB.Long("", "PhaseMask",               DC_,  "", (long*)&m_Phases,     this, isParm|InitHidden);
        }
      }

    DDB.Visibility();
    DDB.Text(" ");
    DDB.Long       ("", "Source",           DC_,  "", (long*)&m_eSource,  this, isParm|SetOnChange, DDBSource);

    static DDBValueLst DDBTemp[] = 
      {
        {Temp_Inlet,   "InletTemp"},
        {Temp_Source,  "SourceTemp"},
        {Temp_Std,     "StdTemp"},
        {Temp_Const,   "Const"},
        {Temp_Mixture, "Mixture"},  // Needs work - a solve loop for volume
        {0}
      };

    DDB.Text(" ");
    DDB.Long  ("FinalTemp",      "",  DC_,  "", (long*)&m_eRqdTemp, this, isParm/*|DDEF_WRITEPROTECT*/, DDBTemp);
    DDB.Visibility(SHM_All, m_eRqdTemp==Temp_Const);
    DDB.Double("RqdTemp",        "",  DC_T, "C", &m_RqdTemp, this, isParm/*|DDEF_WRITEPROTECT*/);
    DDB.Visibility();

    //#if VER1
//    if (DDB.BeginArray(this, "Comp", "EVB_Comps", m_Components.GetSize()))
//      {
//      for (int i=0; i<m_Components.GetSize(); i++)
//        {
//        LPTSTR Tg=gs_CDB[m_Components[i].m_CIndex].SymOrTag();
//        if (DDB.BeginElement(this, Tg, NULL, i))
//          {
//          DDB.Byte  ("", "Destination", DC_,     "",  &m_Components[i].m_Dest,      this, isParm, DDBDestinations);
//          DDB.Double("", "Fraction",    DC_Frac, "%", &m_Components[i].m_Fraction,  this, isParm);
//          }
//        }
//      }
//    DDB.EndArray();
//#else
//    DDB.Int("CompCount",     "",   DC_, "", xidCompCount,    this, isParm);
//    if (DDB.BeginArray(this, "Comp", "EVB_Comps", m_Components.GetSize()))
//      {
//      for (int i=0; i<m_Components.GetSize(); i++)
//        {
//        if (DDB.BeginElement(this, i, NULL, i))
//          {
//          DDB.Long  ("", "Component", DC_,     "",  &m_Components[i].m_CIndex,    this, isParmStopped, &gs_CDB.DDBCompListDashVapLiq);
//          DDB.Double("", "Fraction",  DC_Frac, "%", &m_Components[i].m_Fraction,  this, isParm);
//          }
//        }
//      }
//    DDB.EndArray();
//#endif
  //  }
  //DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CXBlk_Makeup::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidMkAll:
      if (DCB.rB)
        m_Phases = *DCB.rB ? som_ALL:0;
      DCB.B = (m_Phases==som_ALL) ? 1 : (m_Phases==0) ? 0 : 2; 
      return 1;
    case xidMkSolids:
      if (DCB.rB)
        m_Phases = (m_Phases&~som_Sol) | (*DCB.rB ? som_Sol:0);
      DCB.B = ((m_Phases&som_Sol)==som_Sol)? 1 : ((m_Phases&som_Sol)==0) ? 0 : 2; 
      return 1;
    case xidMkLiquids:
      if (DCB.rB)
        m_Phases = (m_Phases&~som_Liq) | (*DCB.rB ? som_Liq:0);
      DCB.B = ((m_Phases&som_Liq)==som_Liq)? 1 : ((m_Phases&som_Liq)==0) ? 0 : 2; 
      return 1;
    case xidMkGasses:
      if (DCB.rB)
        m_Phases = (m_Phases&~som_Gas) | (*DCB.rB ? som_Gas:0);
      DCB.B = ((m_Phases&som_Gas)==som_Gas)? 1 : ((m_Phases&som_Gas)==0) ? 0 : 2; 
      return 1;
    default:
      if (DCB.lHandle>=xidMkPhase && DCB.lHandle<xidMkPhase+CDB.PhaseCount())
        {
        int o=DCB.lHandle-xidMkPhase;
        CPhaseInfo & P=CDB.PhaseInfo(o);
        if (DCB.rB)
          m_Phases = (m_Phases&~P.m_PhMsk) | (*DCB.rB ? P.m_PhMsk:0);
        DCB.B = ((m_Phases&P.m_PhMsk)==P.m_PhMsk)? 1 : ((m_Phases&P.m_PhMsk)==0) ? 0 : 2; 
        return 1;
        }
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag CXBlk_Makeup::ValidateData(ValidateDataBlk & VDB)
  {
//#if VER1
//#else
//  for (int i=0; i<m_Components.GetSize(); i++)
//    {
//    int iComp=m_Components[i].m_CIndex;
//    if (iComp>=0)
//      {
//      for (int j=i+1; j<m_Components.GetSize(); j++)
//        {
//        if (iComp==m_Components[j].m_CIndex)
//          {
//          m_Components.RemoveAt(j);
//          j--;
//          }
//        }
//      }
//    }
//#endif
//
//  for (int i=0; i<m_Components.GetSize(); i++)
//    m_Components[i].m_Fraction=Range(0.0, m_Components[i].m_Fraction, 1.0);

  return CMakeupBlock::ValidateData(VDB); 
  }

//--------------------------------------------------------------------------

void CXBlk_Makeup::EvalProducts(SpConduit &QPrd, double Po, double FinalTEst)
  {
  SpConduit &QSrc=SrcIO.Cd;
  double QmAdd=0.0;

  // Copy to Src if Self
  if (m_eSource==Src_Self)
    QSrc.QSetF(QPrd, som_ALL, 1.0);

  double TSrc=QSrc.Temp();
  double TPrd=QPrd.Temp();
  double TReqd=TPrd;

  switch (m_eRqdTemp)
    {
    case Temp_Inlet:
      TReqd=TPrd;
      break;
    case Temp_Source:
      TReqd=TSrc;
      break;
    case Temp_Std:
      TReqd=StdT;
      break;
    case Temp_Mixture:
      TReqd=StdT; //??????
      break;
    }

  switch (m_eType)
    {                         
    case Type_MassFlow:
      {
      double QmPrd=QPrd.QMass(m_Phases);
      if (m_QmRqd>QmPrd)
        QmAdd=m_QmRqd-QmPrd;
      break;
      }
    case Type_MassRatio:
      {
      QmAdd=QPrd.QMass(m_Phases)*m_QmRatio;
      break;
      }
    case Type_VolumeFlow:
      {
      double QvPrd=QPrd.QVolume(m_Phases);
      if (m_QvRqd>QvPrd)
        {
        double QvAdd=m_QvRqd-QvPrd;
        QmAdd=QvAdd*QPrd.Rho(m_Phases, QPrd.Temp(), Po);
        }
      break;
      }
    case Type_VolumeRatio:
      {
      QmAdd=QPrd.QVolume(m_Phases)*m_QvRatio*QPrd.Rho(m_Phases, QPrd.Temp(), Po);
      break;
      }
      break;
    case Type_NVolumeFlow:
      {
      double QvPrd=QPrd.QNVolume(m_Phases);
      if (m_NQvRqd>QvPrd)
        {
        double NQvAdd=m_NQvRqd-QvPrd;
        QmAdd=NQvAdd*QPrd.NRho(m_Phases);
        }
      break;
      }
    case Type_NVolumeRatio:
      {
      QmAdd=QPrd.QNVolume(m_Phases)*m_QvRatio*QPrd.NRho(m_Phases);
      break;
      }
    case Type_MassFrac:
      {
      double QmPh    = QPrd.QMass(m_Phases);
      double QmTot   = QPrd.QMass();
      double SrcFrac = QSrc.QMass(m_Phases)/GTZ(QSrc.QMass());
      QmAdd   = GEZ((QmPh - m_MassFrac*QmTot) / NZ(m_MassFrac - SrcFrac));
      break;
      }
      break;
    case Type_VolumeFrac:
      {
      double QvPh    = QPrd.QVolume(m_Phases);
      double QvTot   = QPrd.QVolume();
      double SrcFrac = QSrc.QVolume(m_Phases)/GTZ(QSrc.QVolume());
      double QvAdd   = GEZ((QvPh - m_VolFrac) / NZ(m_VolFrac - SrcFrac));
      QmAdd   = QvAdd*QPrd.Rho(m_Phases);
      break;
      }
    case Type_NVolumeFrac:
      {
      double QvPh    = QPrd.QNVolume(m_Phases);
      double QvTot   = QPrd.QNVolume();
      double SrcFrac = QSrc.QNVolume(m_Phases)/GTZ(QSrc.QNVolume());
      double QvAdd   = GEZ((QvPh - m_NVolFrac) / NZ(m_NVolFrac - SrcFrac));
      QmAdd   = QvAdd*QPrd.NRho(m_Phases);
      break;
      }
    };

  if (QmAdd>0)
    {
    QPrd.QAddM(QSrc, som_ALL, QmAdd);
    QSrc.QAdjustQmTo(som_ALL, QmAdd);
    }
  else
    QSrc.QZero();

  QPrd.SetTempPress(TReqd, Po);


  //SpConduit & Discard = m_pMakeupBase->DiscardCd();
  //Discard.QZero();
  //SpMArray OldVap;

  //Qf.SetPress(Po);
  //double H=Qf.totHf();
  //for (int i=0; i<m_Components.GetSize(); i++)
  //  {
  //  CMakeupComp  & EC = m_Components[i];
  //  CComponent & C  = gs_CDB[EC.m_CIndex];
  //  int iLiq=C.LiqPhInx();
  //  int iVap=C.VapPhInx();
  //  double Liq=Qf.VMass[iLiq];
  //  double Vap=Qf.VMass[iVap];
  //  double D=Liq*EC.m_Fraction;

  //  Qf.SetVValue(iLiq, Liq-D);
  //  Qf.SetVValue(iVap, Vap+D);

  //  Discard.SetVValue(iVap, D);
  //  OldVap.SetVValue(iVap, Vap);
  //  }
  //Qf.Set_totHf(H);

  //double T=Qf.Temp();
  //double P=Qf.Press();
  //for (int i=0; i<m_Components.GetSize(); i++)
  //  {
  //  CMakeupComp  & EC = m_Components[i];
  //  int iVap = gs_CDB[EC.m_CIndex].VapPhInx();
  //  switch (EC.m_Dest)
  //    {
  //    case Dest_Mix:
  //      // Qf is at correct conditions
  //      Discard.SetVValue(iVap, 0.0);
  //      break;
  //    case Dest_Discard:
  //      Qf.SetVValue(iVap, OldVap[iVap]); // reset to original vapour mass
  //      break;
  //    }
  //  }
  //Qf.SetTempPress(T, P);
  //Discard.SetTempPress(T, P);
  };

void CXBlk_Makeup::EvalProductsPipe(SpConduit & Qf, double Len, double Diam, double Po, double FinalTEst)
  {
  EvalProducts(Qf, Po, FinalTEst);
  };

//=========================================================================
//
//
//
//=========================================================================

#endif