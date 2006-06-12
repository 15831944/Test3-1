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
m_In(eDIO_Makeup, dynamic_cast<FlwNode*>(pAttach), false, true, 
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
      if (m_In.Enabled)
        m_In.BuildDataDefn(DDB, NULL, DDB_NoPage, UserInfo+102);

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
    if (m_In.DataXchg(DCB))
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
    enum eType     
      { 
      Type_None, 
      Type_MassFlow, 
      Type_MassRatio, 
      Type_VolumeFlow, 
      Type_VolumeRatio, 
      Type_NVolumeFlow, 
      Type_NVolumeRatio,
      Type_TotalMassFrac
      };

    eType           m_Type;
    PhMask          m_Phases;
    double          m_QmRqd;


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
  m_Type=Type_MassRatio;
  m_Phases=som_ALL;
  m_QmRqd=0;
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
    static DDBValueLst DDBType[] =
      {                         
        {Type_None,           "None"          },
        {Type_MassFlow,       "MassFlow"      },
        {Type_MassRatio,      "MassRatio"     },
        {Type_VolumeFlow,     "VolumeFlow"    },
        {Type_VolumeRatio,    "VolumeRatio"   },
        {Type_NVolumeFlow,    "NVolumeFlow"   },
        {Type_NVolumeRatio,   "NVolumeRatio"  },
        {Type_TotalMassFrac,  "TotalMassFrac" },
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

///    DDB.Text(" ");
    DDB.Long       ("", "Type",             DC_,  "", (long*)&m_Type,  this, isParm|SetOnChange, DDBType);
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


    DDB.Double("", "QmRqd",    DC_Qm, "kg/s", &m_QmRqd,  this, isParm);
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

void CXBlk_Makeup::EvalProducts(SpConduit &Qf, double Po, double FinalTEst)
  {
  SpConduit &QIn=In.Cd;
  double QmIn=QIn.QMass();


  double Qm=Qf.QMass();
  if (m_QmRqd>Qm)
    {
    double QmAdd=m_QmRqd-Qm;
    Qf.QAddM(QIn, som_ALL, QmAdd);
    QIn.QAdjustQmTo(som_ALL, QmAdd);
    }
  else
    QIn.QZero();
  
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