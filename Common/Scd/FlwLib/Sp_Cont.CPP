//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"

#define  __SP_CONT_CPP
#include "sp_cont.h"
#include "errorlog.h"
#include "executiv.h"
#include "dbgmngr.h"
#include "flwnode.h"
#include "vlebase.h"

//#include "optoff.h"

#define dbgConstruction  0
#define dbgProblem       0

#define dbgSpCont WITHDEBUG

#if dbgSpCont
static CDbgMngr dbgResTime           ("SpContainer", "ResTime");
static CDbgMngr dbgValidate          ("SpImage", "Validate");
static CDbgMngr dbgValidateBrk       ("SpImage", "ValidateBrk");
#endif

static byte SpCd_Start=0x51;
static byte SpCd_Model=0x52;
static byte SpCd_End  =0x53;
static byte SpCn_Start=0x54;
static byte SpCn_Shape=0x55;
static byte SpCn_Model=0x56;
static byte SpCn_End  =0x57;
static byte SpIm_Start=0x58;
static byte SpIm_End  =0x59;

// ===========================================================================
//
//                                SpContAddress
//
// ===========================================================================

//SpTargetTag::SpTargetTag(int LclIOId, int RmtIOId)
//  {
//#if dbgProblem
//  dbgpln("  Add SpTargetTag @%8x &pTrg=%8x", this, &pTarget);
//#endif
//  pTarget=NULL;
//  bMustLook=1;
//  TargetTags.AddAddress(this);
//  m_LclIOId=LclIOId;
//  m_RmtIOId=RmtIOId;
//  };
//
//// ---------------------------------------------------------------------------
//
//SpTargetTag::~SpTargetTag()
//  {
//#if dbgProblem
//  dbgpln("  Del SpTargetTag @%8x &pTrg=%8x", this, &pTarget);
//#endif
//  TargetTags.DelAddress(this);
//  };
//
//// ---------------------------------------------------------------------------
//
//void SpTargetTag::SetTag(char * Tag)
//  {
//  bMustLook=1;
//  sTargetTag=Tag;
//  pTarget=NULL;
//#if dbgProblem
//  dbgpln("SpTargetTag::SetTargetTag() @%8x &pTrg %8x = %8x", this, &pTarget, pTarget);
//#endif
//  }
//
//// ---------------------------------------------------------------------------
//
//char *SpTargetTag::Tag()
//  {
//  return sTargetTag();
//  }
//
//// ---------------------------------------------------------------------------
//
//flag SpTargetTag::Valid()
//  {
//  if (1 || bMustLook)
//    {
//    pTarget=TargetTags.Find(sTargetTag());
//    bMustLook=0;
//#if dbgProblem
//    dbgpln("SpTargetTag::Valid() @%8x &pTrg %8x = %8x", this, &pTarget, pTarget);
//#endif
//    }
//  return pTarget!=NULL;
//  }
//
//// ---------------------------------------------------------------------------
//
//SpContainer &SpTargetTag::Container()
//  {
//  Valid();
//  return *pTarget;
//  }
//
//// ---------------------------------------------------------------------------
//
//void SpTargetTag::Reset()
//  {
//#if dbgProblem
//  dbgpln("SpTargetTag::Reset() @%8x &pTrg %8x = %8x", this, &pTarget, pTarget);
//#endif
//  bMustLook=1;
//  pTarget=NULL;
//  };
//
//// ---------------------------------------------------------------------------
//
//void SpTargetTag::AddMass2Target(SpConduit &Q, double QMass, double DeltaTime)
//  {
//  if (Valid())
//    {
//    // Spills
//    _asm int 3;
//    //Container().Model()->AddMassM(Q.Model(), som_ALL, QMass*DeltaTime);
//    //Container().InFlow().QAddM(Q, som_ALL, QMass);
//    }
//  };
//
//
//// ---------------------------------------------------------------------------
//
//void SpTargetTag::AddMass2Target(SpModel &M, double QMass, double DeltaTime)
//  {
//  if (Valid())
//    {
//    // Spills
//    _asm int 3;
//    //Container().Model()->AddMassM(&M, som_ALL, QMass*DeltaTime);
//    //Container().InFlow().QAddM(M, som_ALL, QMass);
//    }
//  };
//
//// ===========================================================================
////
////                                 Spill OverFlows
////
//// ===========================================================================
//
//SpTargetTags::SpTargetTags(): Targets(*(new SpContainerList)),
//Tags(*(new SpTargetTagList)),
//MissingTargets(*(new Strng_List))
//  {
//  pOwningObj=NULL;
//  sDefaultAreaName="Floor.Content";
//  };
//
//// ---------------------------------------------------------------------------
//
//SpTargetTags::~SpTargetTags()
//  {
//  pOwningObj=NULL;
//  delete &Targets;
//  delete &Tags;
//  delete &MissingTargets;
//  };
//
//// ---------------------------------------------------------------------------
//
//void SpTargetTags::Init(pTaggedObject OwningObj)
//  {
//  //ASSERT(pOwningObj==NULL);
//  pOwningObj=OwningObj;
//  };
//
//// ---------------------------------------------------------------------------
//
//void SpTargetTags::Term()
//  {
//  Targets.RemoveAll();
//
//  pOwningObj=NULL;
//  };
//
//// ---------------------------------------------------------------------------
//
//void SpTargetTags::AddAddress(SpTargetTag * pAdd)
//  {
//  Tags.AddTail(pAdd);
//  };
//
//// ---------------------------------------------------------------------------
//
//void SpTargetTags::DelAddress(SpTargetTag * pAdd)
//  {
//  POSITION Pos=Tags.Find(pAdd);
//  if (Pos)
//    Tags.RemoveAt(Pos);
//  };
//
//// ---------------------------------------------------------------------------
//
//SpContainer *SpTargetTags::Find(char * AreaName)
//  {
//  if (AreaName==NULL || strlen(AreaName)==0)
//    AreaName=sDefaultAreaName();
//
//#if dbgProblem
//  dbgpln("SpTargetTags::Find() %s", AreaName);
//#endif
//  int ObjTagLen=0;
//  TaggedObject *p=pOwningObj->FindObjTag(AreaName, true, ObjTagLen, 0);
//  if (p)
//    {
//#if dbgProblem
//    dbgpln("SpTargetTags::Find() -AAA");
//#endif
//    if (p->Class()!=&SpContainerClass)
//      {
//      pTaggedObject p1=p->pAttachments;
//      //Pos+=strlen(p->Tag())+1;
//#if dbgProblem
//      dbgpln("SpTargetTags::Find() -BBB");
//#endif
//      while (p1 && (_stricmp(p1->Tag(), &AreaName[ObjTagLen+1])!=0))
//        {
//        p1=p1->pNxtAttachment;
//#if dbgProblem
//        dbgpln("SpTargetTags::Find() -CCC");
//#endif
//        }
//      if (p1)
//        p=p1;
//      }
//#if dbgProblem
//    dbgpln("SpTargetTags::Find() -AAAAAA");
//#endif
//
//    if (p->Class()==&SpContainerClass)
//      return (SpContainer*)p;
//    }
//#if dbgProblem
//  dbgpln("SpTargetTags::Find() -DDD");
//#endif
//
//  if (MissingTargets.Find(AreaName)==NULL)
//    MissingTargets.Append(AreaName);
//#if dbgProblem
//  dbgpln("SpTargetTags::Find() -EEE");
//#endif
//
//  return NULL;
//  };
//
//// ---------------------------------------------------------------------------
//
//flag SpTargetTags::InitialiseSolution()
//  {
//  Targets.RemoveAll();
//  MissingTargets.Clear();
//  SpTargetTagIter T(Tags);
//  for (pSpTargetTag p=T.First(); p; p=T.Next())
//    p->Reset();
//
//  return 1;
//  };
//
//// ---------------------------------------------------------------------------
//
//flag SpTargetTags::TerminateSolution()
//  {
//  return 0;
//  };
//
//// ---------------------------------------------------------------------------
//
//char * SpTargetTags::DefaultArea(char * DefaultAreaName /* =NULL */)
//  {
//  if (DefaultAreaName)
//    sDefaultAreaName=DefaultAreaName;
//
//  if (sDefaultAreaName.Length()==0)
//    {
//    if (OrigPrjFileVerNo()<90)
//      sDefaultAreaName="Floor.Content";
//    else
//      sDefaultAreaName="Lost.Content";
//    }
//
//  return sDefaultAreaName();
//  };
//
//// ===========================================================================
//
//SpTargetTags TargetTags;

// ===========================================================================
//
//                              Specie Conduit
//
// ===========================================================================

CSpView SpConduit::GlblView(SVV_AsMassFrac);

IMPLEMENT_SPARES(SpConduit, 100);
IMPLEMENT_TAGOBJ(SpConduit, "SpConduit", "SpConduit", "", "", "", TOC_SYSTEM,
                 "Specie Conduit", "Specie Conduit");

SpConduit::SpConduit(pTagObjClass pClass_, pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
SpModelOwner(pClass_, pTag_, pAttach, eAttach, false)
  {
  pModel=(SpModel*)SpModelClass.ConstructGroup(DefaultModel(), NULL, SpModelDefTag, this, TOA_Embedded|TOA_Hidden);
  pModel->m_pGlblView=&GlblView;
  SetView(SVV_AsMassFlow);
  SetUsage(SPMU_Flow);
  SetNegDataOK(true);
  };

// --------------------------------------------------------------------------

SpConduit::SpConduit(pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) : \
SpModelOwner(&SpConduitClass, pTag_, pAttach, pAttach ? (eAttach!=TOA_Unknown ? eAttach : TOA_Embedded) : TOA_Free, false)
  {
  pModel=(SpModel*)SpModelClass.ConstructGroup(DefaultModel(), NULL, SpModelDefTag, this, TOA_Embedded|TOA_Hidden);
  pModel->m_pGlblView=&GlblView;
  SetView(SVV_AsMassFlow);
  SetUsage(SPMU_Flow);
  SetNegDataOK(true);
  };

// --------------------------------------------------------------------------

SpConduit::SpConduit() : \
SpModelOwner(&SpConduitClass, "NoTag", NULL, TOA_Free, false)
  {
  pModel=(SpModel*)SpModelClass.ConstructGroup(DefaultModel(), NULL, SpModelDefTag, this, TOA_Embedded|TOA_Hidden);
  pModel->m_pGlblView=&GlblView;
  SetView(SVV_AsMassFlow);
  SetUsage(SPMU_Flow);
  };

// --------------------------------------------------------------------------

SpConduit::~SpConduit()
  {
  pModel->Destroy();
  };

// --------------------------------------------------------------------------

void SpConduit::SetStateAction(IE_Enables E)
  {
  pModel->SetStateAction(E);

  //m_States.RemoveAll();
  //switch (E)
  //  {
  //  case IE_Integrate:
  //    m_States.Add(&dTState, &dTDeriv, &dTState);
  //    break;
  //  case IE_SaveState:
  //    m_States.Add(&dTState);
  //    break;
  //  default:
  //    break;
  //  }
  };
//void SpConduit::Enable(IE_Enables E)
//  {
//  pModel->Enable(E);
//  };
//
//// --------------------------------------------------------------------------
//
//void SpConduit::Disable()
//  {
//  pModel->Disable();
//  };

// --------------------------------------------------------------------------

static const BDDSpecies_Blk sbConduit[] = {
  {SVV_AsMassFlow,     SVV_AsMassFlow,  false, true , VAMsk_SpMass      },
  {SVV_AsMassFrac,     SVV_AsMassFlow,  false, false, VAMsk_SpMass      },
  {SVV_AsMoleFlow,     SVV_AsMassFlow,  false, false, VAMsk_SpMole      },
  {SVV_AsMoleFrac,     SVV_AsMassFlow,  false, false, VAMsk_SpMole      },
  {SVV_AsVolFlow,      SVV_AsMassFlow,  false, false, VAMsk_SpVol       },
  {SVV_AsVolFrac,      SVV_AsMassFlow,  false, false, VAMsk_SpVol       },
  {SVV_AsPartialP,     SVV_AsMassFlow,  false, false, VAMsk_SpPartialP  },
  {SVV_AsPartialPFrac, SVV_AsMassFlow,  false, false, VAMsk_SpPartialP  },
  {SVV_AsNVolFlow,     SVV_AsMassFlow,  false, false, VAMsk_SpNVol      },
  {SVV_AsNVolFrac,     SVV_AsMassFlow,  false, false, VAMsk_SpNVol      },
  {SVV_AsSpcConc,      SVV_AsMassFlow,  false, false, VAMsk_SpConc      },
  {SVV_AsElemFlow,     SVV_AsMassFlow,  false, false, VAMsk_SpElem      },
  {SVV_AsElemFrac,     SVV_AsMassFlow,  false, false, VAMsk_SpElem      },
  {SVV_AsElemConc,     SVV_AsMassFlow,  false, false, VAMsk_SpElem      },
  {SVV_AsElemConc25,   SVV_AsMassFlow,  false, false, VAMsk_SpElem      },
  {SVV_AsCompFlow,     SVV_AsMassFlow,  false, false, VAMsk_SpComp      },
  {SVV_AsCompFrac,     SVV_AsMassFlow,  false, false, VAMsk_SpComp      },
  {SVV_AsCompConc,     SVV_AsMassFlow,  false, false, VAMsk_SpComp      },
  {SVV_AsCompConc25,   SVV_AsMassFlow,  false, false, VAMsk_SpComp      }
  };

void SpConduit::BuildDataDefn(DataDefnBlk &DDB)
  {
  if (DDB.BeginStruct(this))
    {
    AddModelName(DDB);
    AddFidelity(DDB, false);
    pModel->BuildDataDefn_AvailVars(DDB);
    pModel->BuildDataDefn_QualsSlct(DDB, false);
    Strng Tg(Tag()/*DDB.BlockTag()*/);
    Tg+="..";

    pModel->BuildDataDefn_RqdProps(DDB);
    //pModel->BuildDataDefn_UsrProps(DDB, MP_GlobalProp, false);
    pModel->BuildDataDefn_UsrProps(DDB, MP_ConfigProp, false);

    DDB.Double  ("Temperature",     "T",     DC_T,    "C",      xidSMTemp,     this, isResult|0|isParmConstruct);//(!AsFlow ? isParm : 0));
    DDB.Double  ("Pressure",        "P",     DC_P,    "kPag",   xidSMPress,    this, isResult|0|isParmConstruct);//(!AsFlow ? isParm : 0));

    DDB.Text    ("Mass Flow");
    DDB.Double  ("TotalMass",       "QMt",   DC_Qm,   "kg/s",   xidSMTMass,    this, isResult|noFile|noSnap);
    DDB.Double  ("SolidMass",       "SQMt",  DC_Qm,   "kg/s",   xidSMSMass,    this, isResult|noFile|noSnap); //|InitHidden); // hss 14/2/98
    DDB.Double  ("LiquidMass",      "LQMt",  DC_Qm,   "kg/s",   xidSMLMass,    this, isResult|noFile|noSnap); //|InitHidden);
    DDB.Double  ("VapourMass",      "VQMt",  DC_Qm,   "kg/s",   xidSMVMass,    this, isResult|noFile|noSnap); //|InitHidden);
    DDB.Double  ("SlurryMass",      "SLQMt", DC_Qm,   "kg/s",   xidSMSLMass,   this, isResult|noFile|noSnap); //|InitHidden);

    DDB.Text    ("Volume Flow");
    DDB.Double  ("TotalVol",        "QVt",   DC_Qv,   "m^3/s",   xidSMTVol,    this, isResult|noFile|noSnap);
    DDB.Double  ("SolidVol",        "SQVt",  DC_Qv,   "m^3/s",   xidSMSVol,    this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("LiquidVol",       "LQVt",  DC_Qv,   "m^3/s",   xidSMLVol,    this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("VapourVol",       "VQVt",  DC_Qv,   "m^3/s",   xidSMVVol,    this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("SlurryVol",       "SLQVt", DC_Qv,   "m^3/s",   xidSMSLVol,   this, isResult|noFile|noSnap|InitHidden);

    DDB.Text    ("Normal Volume Flow");
    DDB.Double  ("TotalNormVol",     "NQVt",   DC_NQv,   "NL/s",   xidSMTNVol,    this, isResult|noFile|noSnap);
    DDB.Double  ("SolidNormVol",     "SNQVt",  DC_NQv,   "NL/s",   xidSMSNVol,    this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("LiquidNormVol",    "LNQVt",  DC_NQv,   "NL/s",   xidSMLNVol,    this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("VapourNormVol",    "VNQVt",  DC_NQv,   "NL/s",   xidSMVNVol,    this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("SlurryNormVol",    "SLNQVt", DC_NQv,   "NL/s",   xidSMSLNVol,   this, isResult|noFile|noSnap|InitHidden);


    pModel->BuildDataDefn_Vars(DDB);
    pModel->BuildDataDefn_UsrProps(DDB, MP_UserProp, true);

    DDB.Text    ("");
    BuildDataDefn_Species(DDB, sbConduit, sizeof(sbConduit));

    if (pModel->Has_Quals())
      {
      DDB.Text("");
      DDB.Page(Tg(), DDB_OptPage);
      pModel->BuildDataDefn_Quals(DDB, false);
      DDB.Text("");
      }
    }
  DDB.EndStruct();
  };

// --------------------------------------------------------------------------

flag SpConduit::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;
  if (pModel->DataXchg(DCB))
    return 1;
  return SpModelOwner::DataXchg(DCB);
  };

//---------------------------------------------------------------------------

flag SpConduit::ValidateData(ValidateDataBlk & VDB)
  {

  return true;
  };

// --------------------------------------------------------------------------

flag SpConduit::GetOtherData(FilingControlBlock &FCB)
  {
  return false;
  };

// --------------------------------------------------------------------------

flag SpConduit::PutOtherData(FilingControlBlock &FCB)
  {
  return false;
  };

//--------------------------------------------------------------------------

void SpConduit::SetState(eScdMdlStateActs RqdState)
  {
  switch (RqdState)
    {
    case MSA_Empty:
      QRestoreMass(gs_Environment);
      SetTempPress(AmbientTemp(), AtmosPress());
      QSetTraceMass();
      break;
    case MSA_PreSet:
      break;
    case MSA_ZeroFlows:
      QRestoreMass(gs_Environment);
      SetTempPress(AmbientTemp(), AtmosPress());
      QSetTraceMass();
      break;
    case MSA_EmptySpillTargets:
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  };

//===========================================================================

void SpConduit::dbgDump(char *Hdr, flag Vert/*=1*/)
  {
  if (Vert)
    {
    dbgpln("SpCd : T:%14g P:%14g  %s", pModel->Temp(), pModel->Press(), Hdr);
    for(int s=0; s<SVValueCount(); s++)
      dbgpln("%-20s %14g", SVImg(s).SymOrTag(), VValue[s]);
    }
  else
    {
    static int ncnt=10000;
    if (ncnt>10)
      {
      dbgp("SpCd :   %14s   %14s ", "","");
      for(int s=0; s<SVValueCount(); s++)
        dbgp(" %14s", SVImg(s).SymOrTag());
      ncnt=0;
      }
    ncnt++;
    dbgp("SpCd : T:%14g P:%14g ", pModel->Temp(), pModel->Press());
    for(int s=0; s<SVValueCount(); s++)
      dbgp(" %14g", VValue[s]);
    dbgpln(" %s", Hdr);
    }
  }

// ===========================================================================
//
//                           Stack Specie Conduit
//
// ===========================================================================

//IMPLEMENT_STKSPARES(SpConduit);
//
//void StkSpConduit::Reconstruct()
//  {
//  p->QZero();
//  }
//
// ===========================================================================
//
//                              Specie Direct Conduit
//
// ===========================================================================

//IMPLEMENT_SPARES(SpDirectCd, 100);
//IMPLEMENT_TAGOBJ(SpDirectCd, "SpDirectCd", "SpDrctCd", "", "", "", TOC_SYSTEM,
//                 "Specie Directed Conduit", "Specie Conduit");

//SpDirectCd::SpDirectCd() : 
//  //pTagObjClass pClass_, pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
////SpConduit(pClass_, pTag_, pAttach, eAttach),
//m_Trg(0,0)
//  {
//  //_asm int 3;
//  //m_fPermanent=false;   // Set if this should always be present
//  m_fZeroReqd=true;
//  m_fDiscard=false;
//  m_dTotal=0.0;
//  m_pConduit=NULL;
//  };
//
//// --------------------------------------------------------------------------
//
////SpDirectCd::SpDirectCd(pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) : \
////SpConduit(&SpDirectCdClass, pTag_, pAttach, eAttach),
////m_Trg(0,0)
////  {
////  //_asm int 3;
////  //m_fPermanent=false;   // Set if this should always be present
////  m_fZeroReqd=true;
////  m_fDiscard=false;
////  m_dTotal=0.0;
////  };
//
//// --------------------------------------------------------------------------
//
//SpDirectCd::~SpDirectCd()
//  {
//  };

// ==========================================================================
//
//                              SpContainer
//
// ==========================================================================
/**
IMPLEMENT_SPARES(SpTrgConduit, 100);
IMPLEMENT_TAGOBJ(SpTrgConduit, "SpTrgConduit", "SpTrgCnd", "", "", TOC_SYSTEM,
"Specie Target Conduit", "Specie Target Conduit");

SpTrgConduit::SpTrgConduit(pTagObjClass pClass_, pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
SpConduit(pClass_, pTag_, pAttach, eAttach)
{
dbgpln("Const SpTrgConduit : %s",pTag_);
};

// ---------------------------------------------------------------------------

SpTrgConduit::SpTrgConduit(pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
SpConduit(pTag_, pAttach, eAttach)
{
dbgpln("Const SpTrgConduit : %s",pTag_);
};

// ---------------------------------------------------------------------------

SpTrgConduit::~SpTrgConduit()
{
};
*/
// ---------------------------------------------------------------------------
/*
void SpTrgConduit::BuildDataDefn(DataDefnBlk &DDB)
{
DDB.BeginStruct(this);
DDB.SuppressNextBlock();
SpConduit::BuildDataDefn(DDB);
DDB.EndStruct();
};
*/
//// --------------------------------------------------------------------------
//
//flag SpTrgConduit::DataXchg(DataChangeBlk & DCB)
//  {
//  if (SpConduit::DataXchg(DCB))
//    return 1;
//  switch (DCB.lHandle)
//    {
//    case xidTCdTarget:
//      if (DCB.rpC)
//        SetTargetTag(DCB.rpC);
//      DCB.pC=TargetTag();
//      return 1;
//    }
//  return 0;
//  };
//
////---------------------------------------------------------------------------
//
//flag SpTrgConduit::ValidateData(ValidateDataBlk & VDB)
//  {
//  return SpConduit::ValidateData(VDB);
//  };
//
// ==========================================================================
//
//                              SpContainer
//
// ==========================================================================

double CSpContPreset::dConvergeTol=1.0e-6;

CSpContPreset::CSpContPreset()
  {
  dTRqd=Std_T;
  dPRqd=Std_P;
  dLRqd=0.1;
  bTAdj=true;
  bDoVLE=false;
  bDoImg=false;
  pImg=NULL;
  //iSetupGas=SDB.No(); // Mixture
  //iSetupLiq=SDB.No(); // Mixture
  };

CSpContPreset::~CSpContPreset()
  {
  };

void CSpContPreset::BuildDataDefn(DataDefnBlk &DDB, SpContainer & Cn)
  {
  if (DDB.BeginStruct(&Cn, "Preset", "", DDB_NoPage,-1,MarkerClosed))
    {
    if (Cn.Closed())
      DDB.Double("Required_Press", "P_Rqd",      DC_P,     "kPag",   &dPRqd,           &Cn, isParm);//|NAN_OK);
    DDB.Double("Required_Temp",    "T_Rqd",      DC_T,     "C",      &dTRqd,           &Cn, bTAdj ? isParm : 0);//|NAN_OK);
    DDB.Double("Required_Level",   "L_Rqd",      DC_Frac,  "%",      &dLRqd,           &Cn, isParm);//|NAN_OK);

    //    if (Cn.Closed())
    //      DDB.Short("Required_Gas",      "Gas_Rqd",    DC_,      "",     &iSetupGas,       &Cn, isParm, SDB.DDBVapSpListMix());
    //    DDB.Short("Required_Liquid",   "Liq_Rqd",    DC_,      "",       &iSetupLiq,       &Cn, isParm, SDB.DDBLiqSpListMix());
    DDB.CheckBoxBtn("UseImage",    "",           DC_, "",           &bDoImg,       &Cn, pImg ? isParm : 0);
    DDB.CheckBoxBtn("UseVLE",      "",           DC_, "",           &bDoVLE,       &Cn, isParm);
    }
  DDB.EndStruct();
  };

flag CSpContPreset::ValidateData(ValidateDataBlk & VDB, SpContainer & Cn)
  {
  double T=Cn.Temp();
  double P=Cn.Press();
  double L=Cn.Level();

  bTAdj=!bDoVLE || !Cn.Closed();
  if (!bTAdj && Cn.VLEBlk())
    bTAdj = !Cn.VLEBlk()->Enabled() || Cn.VLEBlk()->HasTPEnvelope();

  if (!Valid(dTRqd)) dTRqd=T;
  if (!Valid(dPRqd)) dPRqd=P;
  if (!Valid(dLRqd)) dLRqd=L;


  if (dTRqd<1.0) dTRqd=Std_T;
  if (dPRqd<1.0) dPRqd=Std_P;

  if (bDoVLE)
    {
    dTRqd = Cn.SaturationT(dPRqd);
    }

  dTRqd=Cn.ValidateRange(VDB, "RqdT", CDB.MinT, dTRqd, CDB.MaxT);
  dPRqd=Cn.ValidateRange(VDB, "RqdP", CDB.MinP, dPRqd, CDB.MaxP);
  dLRqd=Cn.ValidateRange(VDB, "RqdL", 0.0, dLRqd, 1.0);
  return true;
  }

flag CSpContPreset::AdjustConditions(SpContainer & Cn)
  {
  if (bDoVLE && Cn.VLEBlk())
    {
    Cn.VLEBlk()->VFlash(Cn, 0.0, VLEF_Null);
    return Cn.VLEBlk()->Enabled();
    }
  return false;
  }

//==========================================================================
//
//
//==========================================================================

CContStatInfo::CContStatInfo()
  {
  //iType = SIWhatAll;
  ResetStats();
  }

//--------------------------------------------------------------------------

CContStatInfo::~CContStatInfo()
  {
  }

//--------------------------------------------------------------------------

void CContStatInfo::ResetStats()
  {
  dTtlTime = 0.0;
  dOverflowTime = 0.0;
  dMaxLvl = -1e99;
  dMinLvl = 1e99;
  dTotalLvl = 0.0;
  }

//--------------------------------------------------------------------------

void CContStatInfo::BuildDataDefn(DataDefnBlk & DDB, TaggedObject* pObj)
  {
  //DDB.BeginStruct(pObj, "Stats", NULL, DDB_OptPage);
  //DDB.Text("----------------------------------------");
  //DDB.Byte  ("Type",             "",          DC_,     "",      xidCnStatsType,   pObj, isParmStopped, DDBFlwStatsWhat); //type MUST be before tag, etc
  DDB.Button("Reset_Stats",      "",          DC_,     "",      xidCnStatsReset,  pObj, isParm);
  DDB.Double("Time",             "Tm",        DC_Time, "s",     &dTtlTime,        pObj, isResult);
  DDB.Double("Ave_Level",        "LvlAve",    DC_Frac, "%",     xidCnStatsLvlAve, pObj, isResult|noFileAtAll);
  DDB.Double("Min_Level",        "LvlMin",    DC_Frac, "%",     &dMinLvl,         pObj, isResult);
  DDB.Double("Max_Level",        "LvlMax",    DC_Frac, "%",     &dMaxLvl,         pObj, isResult);
  DDB.Double("OverflowTime",     "OvrFlwTm",  DC_Time, "s",     &dOverflowTime,   pObj, isResult|InitHidden);
  /*if (iType & SIWhatQm)
  {
  todo
  DDB.Double("",                 "QmInAve",   DC_Qm,    "kg",    &dTotalMass,    pObj, isResult);
  DDB.Double("",                 "QmOutAve",  DC_Qm,   "kg/s",  xidSI_QmAve,    pObj, isResult|noFileAtAll);
  DDB.Double("ZeroFlowTime",     "ZeroTm",    DC_Time, "s",     &dZeroTime,     pObj, isResult);
  }*/
  //DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CContStatInfo::DataXchg(DataChangeBlk & DCB, TaggedObject* pObj)
  {
  switch (DCB.lHandle)
    {
    //case xidSI_Type:
    //  todo
    //  pObj->StructureChanged(pObj);
    //  return True;
    case xidCnStatsReset:
      if (DCB.rB && (*DCB.rB!=0))
        ResetStats();
      DCB.B=0;
      return True;
    case xidCnStatsLvlAve:
      DCB.D = dTotalLvl/GTZ(dTtlTime);
      return True;
    }
  return false;
  }

//--------------------------------------------------------------------------

void CContStatInfo::ExecIns(SpContainer* pContainer)
  {
  const double dT = ICGetTimeInc();
  if (dT > 0.0)
    {
    dTtlTime += dT;
    const double dMeasLvl = pContainer->Level();
    dTotalLvl += (dMeasLvl*dT);
    if (dMeasLvl>dMaxLvl)
      dMaxLvl = dMeasLvl;
    if (dMeasLvl<dMinLvl)
      dMinLvl = dMeasLvl;
    }
  }

//--------------------------------------------------------------------------
// ==========================================================================
//
//                              SpContainer
//
// ==========================================================================

CSpView SpContainer::GlblView(SVV_AsMass);

IMPLEMENT_SPARES(SpContainer, 100);
IMPLEMENT_TAGOBJ(SpContainer, "SpContainer", "SpContainer", "", "", "", TOC_SYSTEM,
                 "Specie Container", "Specie Container");

long SpContainer::TotalNo=0;

SpContainer::SpContainer(pTagObjClass pClass_, pchar Tag__, pTaggedObject pAttach, TagObjAttachment eAttach) :
SpModelOwner(pClass_, Tag__, pAttach, eAttach, true),
m_MixSet(this)
 {
#if dbgConstruction
  dbgpln("SpContainer():%li",TotalNo+1);
#endif

  MyNo=++TotalNo;

  pShape=(SpShape*)SpShapeClass.ConstructGroup(DefaultShape(), NULL, SpShapeDefTag, this, TOA_Embedded|TOA_Hidden);
  pModel=(SpModel*)SpModelClass.ConstructGroup(DefaultModel(), NULL, SpModelDefTag, this, TOA_Embedded|TOA_Hidden);

  //dFluxDamping=2.0;
  //dLevelSettle=0.0;
  //dOverFlowSettle=0.0;
  m_pQOutEst=NULL;
  //pInFlow=NULL;
  //pOverFlow=NULL;
  //pGasVent=NULL;
  m_pSpill=NULL;
  m_pVent=NULL;

  m_dVapEntrained     = 0.0;
  m_dVapEntrainRate   = 0.0;
  m_dVapEscapeTau     = 60.0;
  m_dVapLiberationTau = 0.0;

  m_eVapEntrainCtrl   = VEC_Off;
  m_dVapEntrainVolFrac = 0.05;
  
  pStats = NULL;
  dInitMt = 0.0;

  pModel->m_pGlblView=&GlblView;
  SetView(SVV_AsMass);
  SetUsage(SPMU_Mass);

  m_States.Add(&m_dVapEntrained, &m_dVapEntrainRate, &m_dVapEntrained);
  //iVapEntrained=new CStateInt(/*S(),*/ dVapEntrained, dVapEntrainRate, dVapEntrained);//, dwVapEntValid, dwVapEntCmd, 0);
  dwDisplay=SPDF_All;

  pPreset=NULL;

  bMRTRqd=false;

  m_bKeepLPTOnVChange=false;
  m_NetPressDamp=false;
  m_TopPRqd     = dNAN;
  m_UseRhoH     = true;

  m_HiLvlLimit  = 1.02;
  m_LoLvlLimit  = dNAN;
  m_LvlHi       = 0;
  m_LvlLo       = 0; 
  m_LmtAsStatus = 1;

  m_HighestSLFrac  = 1.02;
  m_HighestVapFrac = 1.10;
  };

// --------------------------------------------------------------------------

SpContainer::SpContainer(pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) : \
SpModelOwner(&SpContainerClass, pTag_, pAttach, pAttach ? (eAttach!=TOA_Unknown ? eAttach : TOA_Embedded) : TOA_Free, true),
m_MixSet(this)
  {
#if dbgConstruction
  dbgpln("SpContainer():%li",TotalNo+1);
#endif

  MyNo=++TotalNo;

  pShape=(SpShape*)SpShapeClass.ConstructGroup(DefaultShape(), NULL, SpShapeDefTag, this, TOA_Embedded|TOA_Hidden);
  pModel=(SpModel*)SpModelClass.ConstructGroup(DefaultModel(), NULL, SpModelDefTag, this, TOA_Embedded|TOA_Hidden);

  m_pQOutEst=NULL;
  m_pSpill=NULL;
  m_pVent=NULL;

  m_dVapEntrained     = 0.0;
  m_dVapEntrainRate   = 0.0;
  m_dVapEscapeTau     = 60.0;
  m_dVapLiberationTau = 0.0;

  m_eVapEntrainCtrl   = VEC_Off;
  m_dVapEntrainVolFrac = 0.05;
  
  pStats = NULL;
  dInitMt = 0.0;

  pModel->m_pGlblView=&GlblView;
  SetView(SVV_AsMass);
  SetUsage(SPMU_Mass);

  //Strng S(FullObjTag(), "VapEnt");
  //iVapEntrained=new CStateInt(/*S(),*/ dVapEntrained, dVapEntrainRate, dVapEntrained);//, dwVapEntValid, dwVapEntCmd, 0);
  m_States.Add(&m_dVapEntrained, &m_dVapEntrainRate, &m_dVapEntrained);
  dwDisplay=SPDF_All;

  pPreset=NULL;
  bMRTRqd=false;

  m_bKeepLPTOnVChange=false;
  m_NetPressDamp=false;
  m_TopPRqd     = dNAN;
  m_UseRhoH     = true;

  m_HiLvlLimit  = 1.02;
  m_LoLvlLimit  = dNAN;
  m_LvlHi       = 0;
  m_LvlLo       = 0; 
  m_LmtAsStatus = 1;

  m_HighestSLFrac  = 1.02;
  m_HighestVapFrac = 1.10;
  };

// --------------------------------------------------------------------------

SpContainer::~SpContainer()
  {
  pShape->Destroy();
  pModel->Destroy();

  delete m_pQOutEst;
  //delete pInFlow;
  //delete pOverFlow;
  //delete pGasVent;
  delete pStats;

  //delete iVapEntrained;

#if dbgConstruction
  dbgpln("~SpContainer():%li",QQSpMassNo);
#endif

  };

// --------------------------------------------------------------------------

void SpContainer::SetStateAction(IE_Enables E)
  {
  pModel->SetStateAction(E);

  m_States.RemoveAll();
  switch (E)
    {
    case IE_Integrate:
      m_States.Add(&m_dVapEntrained, &m_dVapEntrainRate, &m_dVapEntrained);
      break;
    case IE_SaveState:
      m_States.Add(&m_dVapEntrained);
      break;
    default:
      break;
    }
  };
//void SpContainer::Enable(IE_Enables E)
//  {
//  pModel->Enable(E);
//
//  iVapEntrained->Tag().Concat(FullObjTag(), "VapEnt");
//  iVapEntrained->Enable(E);
//  };
//
//// --------------------------------------------------------------------------
//
//void SpContainer::Disable()
//  {
//  pModel->Disable();
//  iVapEntrained->Disable();
//  };

// --------------------------------------------------------------------------

static const BDDSpecies_Blk sbContainer[]= {
  {SVV_AsMass,         SVV_AsMass, true , true , VAMsk_SpMass     },
  {SVV_AsMassFrac,     SVV_AsMass, false, false, VAMsk_SpMass     },
  {SVV_AsMole,         SVV_AsMass, false, false, VAMsk_SpMole     },
  {SVV_AsMoleFrac,     SVV_AsMass, false, false, VAMsk_SpMole     },
  {SVV_AsVol,          SVV_AsMass, false, false, VAMsk_SpVol      },
  {SVV_AsVolFrac,      SVV_AsMass, false, false, VAMsk_SpVol      },
  {SVV_AsPartialP,     SVV_AsMass, false, false, VAMsk_SpPartialP },
  {SVV_AsPartialPFrac, SVV_AsMass, false, false, VAMsk_SpPartialP },
  {SVV_AsNVol,         SVV_AsMass, false, false, VAMsk_SpNVol     },
  {SVV_AsNVolFrac,     SVV_AsMass, false, false, VAMsk_SpNVol     },
  {SVV_AsSpcConc,      SVV_AsMass, false, false, VAMsk_SpConc     },
  {SVV_AsElem,         SVV_AsMass, false, false, VAMsk_SpElem     },
  {SVV_AsElemFrac,     SVV_AsMass, false, false, VAMsk_SpElem     },
  {SVV_AsElemConc,     SVV_AsMass, false, false, VAMsk_SpElem     },
  {SVV_AsElemConc25,   SVV_AsMass, false, false, VAMsk_SpElem     },
  {SVV_AsComp,         SVV_AsMass, false, false, VAMsk_SpComp     },
  {SVV_AsCompFrac,     SVV_AsMass, false, false, VAMsk_SpComp     },
  {SVV_AsCompConc,     SVV_AsMass, false, false, VAMsk_SpComp     },
  {SVV_AsCompConc25,   SVV_AsMass, false, false, VAMsk_SpComp     }
  };


void SpContainer::BuildDataDefn(DataDefnBlk &DDB)
  {
  if (DDB.BeginStruct(this))
    {
    DDEF_Flags WhichModes =0;
    if (dwDisplay & SPDF_DynMode)
      WhichModes |= SM_DynBoth;
    if (dwDisplay & SPDF_PBMode)
      WhichModes |= SM_Direct;

    DDEF_Flags Vis=DDB.GetVisibility();
    DDB.Visibility(WhichModes);  // hss 8/6/99
    if (dwDisplay & (SPDF_ShapeName|SPDF_ShapeNameDisplay))
      {
      flag DO=DDB.SetDisplayOnly((dwDisplay & SPDF_ShapeNameDisplay)!=0);
      AddShapeName(DDB);
      DDB.SetDisplayOnly(DO);
      }
    if (dwDisplay & (SPDF_ShapeData|SPDF_ShapeDataDisplay))
      {
      flag DO=DDB.SetDisplayOnly((dwDisplay & SPDF_ShapeDataDisplay)!=0);
      pShape->BuildDataDefn(DDB);
      DDB.SetDisplayOnly(DO);
      }
    DDB.Text   (" ");
    DDB.Visibility(WhichModes, !gs_Environment.HasGas() || DefNetDynamicMode());
    DDB.CheckBoxBtn("UseRhoH",        "",    DC_, "",           xidCnUseRhoH,      this, isParm);
    if (Closed() && dwDisplay & SPDF_ShapeClosed)
      DDB.Double("TopPressure",       "",    DC_P, "kPag",      xidCnTopPRqd,      this, isParm|NAN_OK, DDBNAN_AtmosP);
    DDB.Visibility(WhichModes, gs_Environment.HasGas());
    if (dwDisplay & SPDF_ShapeClosed)
      DDB.CheckBoxBtn("Closed",       "",    DC_, "",           xidCnClosed,       this, isParmStopped);
    if (dwDisplay & SPDF_ShapeNetworked)
      DDB.CheckBoxBtn("Networked",    "",    DC_, "",           xidCnNetworked,    this, isParmStopped);
    if (dwDisplay & SPDF_ShapeMixed)
      DDB.CheckBoxBtn("Mixed",        "",    DC_, "",           xidCnMixed,        this, isParmStopped);
    if (dwDisplay & SPDF_ShapeMnRes)
      DDB.CheckBoxBtn("MenResTimes",  "",    DC_, "",           &bMRTRqd,          this, isParmStopped|InitHidden);

    DDB.Visibility(Vis);
    DDB.Text   (" ");
    AddModelName(DDB);
    AddFidelity(DDB, false);
    pModel->BuildDataDefn_AvailVars(DDB);
    pModel->BuildDataDefn_QualsSlct(DDB, true);
    //pModel->BuildDataDefn_UsrProps(DDB, MP_GlobalProp, false);
    pModel->BuildDataDefn_UsrProps(DDB, MP_ConfigProp, false);

    DDB.Visibility(WhichModes);  // hss 8/6/99

    if (dwDisplay & SPDF_NewPage1)
      {
      Strng Tg(Tag()/*DDB.BlockTag()*/);
      Tg+="..";
      DDB.Page    (Tg(), DDB_OptPage);//RqdPage);
      //DDB.Text    (" ");
      }
    DDB.Text    ("Properties");
    DDB.Double  ("Temperature",  "T",    DC_T,    "C",      DDB.ForFileSnpScn() ? xidSMTemp : xidCnTemp, this, isParm|0);
    DDB.Double  ("Pressure",     "P",    DC_P,    "kPag",   DDB.ForFileSnpScn() ? xidSMPress:xidCnPress, this, isParm|0);
    DDB.Double  ("AbsLevel",     "ALvl", DC_Frac, "%",      xidCnALevel,       this, isResult|noFile|noSnap);
    DDB.Double  ("Level",        "Lvl",  DC_Frac, "%",      xidCnRLevel,       this, isParm|noFile|noSnap);
    DDB.Double  ("HiLevelLimit", "HiLmt", DC_Frac, "%",     &m_HiLvlLimit,     this, isParm|NAN_OK|InitHidden);
    DDB.Double  ("LoLevelLimit", "LoLmt", DC_Frac, "%",     &m_LoLvlLimit,     this, isParm|NAN_OK|InitHidden);
    DDB.Bool    ("LevelHi",      "LvlHi", DC_,     "",      &m_LvlHi,          this, isResult|InitHidden);
    DDB.Bool    ("LevelLo",      "LvlLo", DC_,     "",      &m_LvlLo,          this, isResult|InitHidden);
    DDB.CheckBoxBtn("LimitAsStatus", "",  DC_,     "",      &m_LmtAsStatus,    this, isParm|InitHidden);

    DDB.Text    ("Mass");
    DDB.Double  ("TotalMass",    "Mt",   DC_M,     "kg",    xidSMTMass,       this, isResult|noFile|noSnap);
    DDB.Double  ("SolidMass",    "SMt",  DC_M,     "kg",    xidSMSMass,       this, isResult|noFile|noSnap); //|InitHidden); //hss 14/2/98
    DDB.Double  ("LiquidMass",   "LMt",  DC_M,     "kg",    xidSMLMass,       this, isResult|noFile|noSnap); //|InitHidden);
    DDB.Double  ("VapourMass",   "VMt",  DC_M,     "kg",    xidSMVMass,       this, isResult|noFile|noSnap); //|InitHidden);
    DDB.Double  ("SlurryMass",   "SQMt", DC_M,     "kg",    xidSMSLMass,      this, isResult|noFile|noSnap); //|InitHidden);
    DDB.Double  ("InitTtlMass",  "InitMt", DC_M,   "kg",    &dInitMt,         this, isResult|InitHidden);
    DDB.Double  ("DeltaTtlMass", "DeltaMt", DC_M,  "kg",    xidSMDeltaMass,   this, isResult|noFile|noSnap|InitHidden);

    DDB.Text    ("Volume");
    DDB.Double  ("TotalVol",     "Vt",   DC_Vol,   "m^3",   xidSMTVol,        this, isResult|noFile|noSnap); //|InitHidden);
    DDB.Double  ("SolidVol",     "SVt",  DC_Vol,   "m^3",   xidSMSVol,        this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("LiquidVol",    "LVt",  DC_Vol,   "m^3",   xidSMLVol,        this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("VapourVol",    "VVt",  DC_Vol,   "m^3",   xidSMVVol,        this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("SlurryVol",    "SQVt", DC_Vol,   "m^3",   xidSMSLVol,       this, isResult|noFile|noSnap|InitHidden);

    DDB.Text    ("Normal Volume");
    DDB.Double  ("TotalNormVol",  "NVt",   DC_NVol, "Nm^3",   xidSMTNVol,    this, isResult|noFile|noSnap);
    DDB.Double  ("SolidNormVol",  "SNVt",  DC_NVol, "Nm^3",   xidSMSNVol,    this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("LiquidNormVol", "LNVt",  DC_NVol, "Nm^3",   xidSMLNVol,    this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("VapourNormVol", "VNVt",  DC_NVol, "Nm^3",   xidSMVNVol,    this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("SlurryNormVol", "SLNVt", DC_NVol, "Nm^3",   xidSMSLNVol,   this, isResult|noFile|noSnap|InitHidden);

    DDB.Text    ("Entrainment");
    DDB.Double  ("EntVapourVolume",   "EntVapVol",     DC_Vol, "m^3",   xidCnEntVapVol,  this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("EntVapourFraction", "EntVapFrac",    DC_Frac, "%",    xidCnVFracMM,    this, isResult|noFile|noSnap);
    DDB.Double  ("EntVapourVolFrac",  "EntVapFracVV",  DC_Frac, "%",    xidCnVFracVV,    this, isResult|noFile|noSnap);
    //DDB.Double  ("EntSlurryVolFrac",  "EntSLFracVV",  DC_Frac, "%",    xidCnSLFracVV,   this, isResult|noFile|noSnap|InitHidden);


    pModel->BuildDataDefn_Vars(DDB);
    pModel->BuildDataDefn_UsrProps(DDB, MP_UserProp, true);

    DDB.Text   (" ");
    //DDB.Visibility(NM_Dynamic|SM_All|HM_All);  // hss 8/6/99

    BuildDataDefn_Species(DDB, sbContainer, sizeof(sbContainer));

    if (1)
      {
      Strng Tg(Tag()/*DDB.BlockTag()*/);
      Tg+="..";
      DDB.Page(Tg(), DDB_RqdPage);
      pModel->BuildDataDefn_RqdProps(DDB);
      //pModel->BuildDataDefn_UsrProps(DDB, MP_GlobalProp);
      pModel->BuildDataDefn_UsrProps(DDB, MP_ConfigProp);

      DDB.Page(Tg(), DDB_OptPage);
      DDB.Text(" ");
      DDB.Text("ProductIO");
      DDB.Double ("HighestSlurryIO", "", DC_Frac, "%", &m_HighestSLFrac,  this, isParm);
      DDB.Double ("HighestVapourIO", "", DC_Frac, "%", &m_HighestVapFrac, this, 0);
      if (pPreset)
        pPreset->BuildDataDefn(DDB, *this);

//PERM
      //static DDBValueLst DDB0[]={
      //  {0, "As_Required"},
      //  {1, "Exists"},
      //  {2, "Permanent"},
      //  {0}
      //  };

      //if (dwDisplay & SPDF_Inflow)
      //  {
      //  int DoIt;
      //  if (PrjFileVerNo()>=45)
      //    DoIt=DDB.BeginObject(this, "InFlow", "SpContDirect", NULL, DDB_NoPage, -1, MarkerClosed);
      //  else
      //    DoIt=DDB.BeginStruct(this, "InFlow", "", DDB_NoPage,-1,MarkerClosed);
      //  if (DoIt)
      //    DDB.Double("Rate",    "",    DC_Qm, "kg/s",     xidCnInFlowRate,   this, isResult|0);
      //  if (PrjFileVerNo()>=45)
      //    DDB.EndObject();
      //  else
      //    DDB.EndStruct();
      //  }

      if ((dwDisplay & SPDF_Overflow) && SpillExists())
        {
        int DoIt;
        if (PrjFileVerNo()>=45)
          DoIt=DDB.BeginObject(this, "Spill", "SpContDirect", NULL, DDB_NoPage, -1, MarkerClosed);
        else
          DoIt=DDB.BeginStruct(this, "OverFlow", "", DDB_NoPage,-1,MarkerClosed);
        if (DoIt)
          {
          DDB.Double("FracHgt",       "FHgt", DC_Frac, "%",      xidCnSpillFracHgt,   this, isParmStopped);
          DDB.String("Target",        "",     DC_, "",           xidCnSpillTarget, this, isParmStopped|isTag|InitHidden);
          DDB.Double("Rate",          "",     DC_Qm,   "kg/s",   xidCnSpillRate,   this, isResult);
          FlwNode *p=FindObjOfType((FlwNode*)NULL);
          if (p && p->AuditOption()>=Audit_Totals)
            DDB.Double("Total",         "",    DC_M, "kg",        xidCnSpillTotal,  this, 0/*isParm*/);
          }
        if (PrjFileVerNo()>=45)
          DDB.EndObject();
        else
          DDB.EndStruct();
        }

      if ((dwDisplay & SPDF_GasVent) && VentExists())
        {
        int DoIt;
        if (PrjFileVerNo()>=45)
          DoIt=DDB.BeginObject(this, "Vent", "SpContDirect", NULL, DDB_NoPage, -1, MarkerClosed);
        else
          DoIt=DDB.BeginStruct(this, "GasVent", "", DDB_NoPage,-1,MarkerClosed);
        if (DoIt)
          {
          //DDB.Byte  ("Type",          "",    DC_, "",           xidCnVentType,   this, isParm|InitHidden, DDB0);
          //DDB.CheckBoxBtn("Discard",  "",    DC_, "",           xidCnVentDiscard, this, isParm);
          DDB.String("Target",        "",    DC_, "",           xidCnVentTarget, this, isParmStopped|isTag|InitHidden);
          //DDB.Double("Tau",           "",    DC_Time, "s",      xidCnVentTau,    this, isParm|InitHidden);
          DDB.Double("Rate",          "",    DC_Qm, "kg/s",     xidCnVentRate,   this, isResult|0);

          FlwNode *p=FindObjOfType((FlwNode*)NULL);
          if (p && p->AuditOption()>=Audit_Totals)
            DDB.Double("Total",         "",    DC_M, "kg",        xidCnVentTotal,  this, 0/*isParm*/);
          }
        if (PrjFileVerNo()>=45)
          DDB.EndObject();
        else
          DDB.EndStruct();
        }

      if (dwDisplay & SPDF_Safety)
        {
        if (DDB.BeginStruct(this, "Safety", "", DDB_NoPage,-1,MarkerClosed))
          {
          DDB.Visibility(Vis, pModel->Closed());
          DDB.Double("VolScale",      "",    DC_Frac, "%",        xidSSVapVolScale,    this, isParm);

          DDB.Double("VentPress",     "",    DC_P,    "kPag",     xidCnVentPress,      this, isParm);
          LPTSTR Nm1=PrjFileVerNo()<58 ? "MaxLevel":"VentLevel";
          DDB.Double(Nm1,             "",    DC_Frac, "%",        xidCnVentLevel,      this, isParm);

          DDB.Visibility(Vis, true);
          LPTSTR Nm2=PrjFileVerNo()<58 ? "MinLevel":"LowestIOPt";
          DDB.Double(Nm2,             "",    DC_Frac, "%",        xidCnLowestIOPt,       this, isParmStopped);
          }
        DDB.EndStruct();
        }

      DDB.Visibility(WhichModes);
      if (dwDisplay & SPDF_Vapour)
        {
        if (DDB.BeginStruct(this, "Vapour", "", DDB_NoPage,-1,MarkerClosed))
          {

          static DDBValueLst DDBVEC[]=
            {
              {VEC_Off,       "Off"     },
              {VEC_VolFrac,   "VolumeFrac" },
              {0}
            };

          DDB.Long("EntrainCtrl",            "",    DC_,     "",    (long*)&m_eVapEntrainCtrl,     this, isParm|SetOnChange, DDBVEC);
          DDB.Visibility(NSHM_All, m_eVapEntrainCtrl==VEC_VolFrac);
          DDB.Double("EntrainedVolFrac",     "",    DC_Frac, "%",   &m_dVapEntrainVolFrac,        this, isParm);

          DDB.Visibility();
          DDB.Double("EscapeTau",            "",    DC_Time, "s",   &m_dVapEscapeTau,     this, isParm);
          DDB.TagComment("ToAtmosphere");
          DDB.Double("LiberationTau",        "",    DC_Time, "s",   &m_dVapLiberationTau, this, isParm);
          DDB.TagComment("FromSubsurface");
          DDB.Double("Entrained",            "",    DC_M,    "kg",  &m_dVapEntrained,     this, isParm);
          }
        DDB.EndStruct();
        }
      if (dwDisplay & SPDF_MixSet)
        {
        m_MixSet.BuildDataDefn(DDB);
        }

      DDB.Visibility(WhichModes);
      if (dwDisplay & SPDF_DynStats)
        {
        if (DDB.BeginStruct(this, "DynStats", "", DDB_NoPage,-1,MarkerClosed))
          {
          DDB.CheckBoxBtn("CollectStats",  "",  DC_, "",  xidCnStatsOn,   this, isParmStopped);//|SetOnChange);
          if (pStats && NetDynamicMethod())
            {
            pStats->BuildDataDefn(DDB, this);
            }
          }
        DDB.EndStruct();
        }

      DDB.Visibility(WhichModes);
      static DDBValueLst DDB4[]= {
        {SPMT_Individual, "Individual"},
        {SPMT_Total,      "Total"},
        {0} };
        DDB.Byte  ("Int_Tests",       "",    DC_, "",           xidCnIntTests,       this, isParmStopped|InitHidden, DDB4);

        DDB.Visibility();

        if (pModel->Has_Quals())
          {
          DDB.Text   (" ");
          pModel->BuildDataDefn_Quals(DDB, true);
          DDB.Text("");
          }


        //_asm int 3;
        //if (InFlowExists())
        //  DDB.Object(&InFlow(), this, NULL, NULL, DDB_RqdPage);
        //if (VentExists())
        //  DDB.Object(&GasVent(), this, NULL, NULL, DDB_RqdPage);
        //if (SpillExists())
        //  DDB.Object(&OverFlow, this, NULL, NULL, DDB_RqdPage);
        if (bMRTRqd)
          {
          if (DDB.BeginObject(this, "RTs", "Sp_RT",NULL,DDB_RqdPage,-1))
            {
            for (int s=0; s<SVSpcCount(); s++)
              DDB.Double(SDB[s].Tag(),  SDB[s].Sym(), DC_Time, "s", SpMRT.VPtr[s], this, 0);
            }
          DDB.EndObject();
          }
      }
    DDB.Visibility(Vis);  // hss 8/6/99
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag SpContainer::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;
  switch (DCB.lHandle)
    {
    case xidCnIntTests:
      if (DCB.rB)
        pModel->SetIntegrationTests((SPMTests)*DCB.rB);
      DCB.B=(byte)pModel->IntegrationTests();
      return 1;
    case xidCnPress:
      if (DCB.rD) 
        {
        if (Closed()) 
          {
          CSpContPreset PS;
          PS.SetLRqd(RLevel());
          PS.SetTRqd(Temp());
          PS.SetPRqd(*DCB.rD);
          PS.SetImg(NULL);
          SetRequiredConditions(&PS);
          }
        }
      DCB.D = Press();
      return 1;
    case xidCnTemp:
      if (DCB.rD)
        {
        if (Closed()) 
          {
          CSpContPreset PS;
          PS.SetLRqd(RLevel() );
          PS.SetTRqd(*DCB.rD);
          PS.SetPRqd(Press());
          PS.SetImg(NULL);
          SetRequiredConditions(&PS);
          }
        else
          {
          SetTempPress(*DCB.rD, Press());
          }
        }
      DCB.D = Temp();
      return 1;
    case xidCnALevel:
      DCB.D = Level();
      return 1;
    case xidCnRLevel:
      if (DCB.rD) 
        {
        CSpContPreset PS;
        PS.SetLRqd(*DCB.rD);
        PS.SetTRqd(Temp());
        PS.SetPRqd(Press());
        PS.SetImg(NULL);
        SetRequiredConditions(&PS);
        }
      DCB.D = RLevel();
      return 1;
    case xidCnSpillFracHgt:
      if (DCB.rD)
        Spill.FracHgt=*DCB.rD;
      DCB.D=Spill.FracHgt;
      return 1;
    case xidCnSpillTarget:
      if (DCB.rpC!=NULL && (strlen(DCB.rpC)>0))
        Spill.SetTarget(DCB.rpC);
      if (SpillExists())
        DCB.pC=(LPTSTR)Spill.TargetStr(DCB.ForFileSnpScn());
      else
        DCB.pC="";
      return 1;
    case xidCnSpillTotal:
      if (SpillExists())
        {
        //if (DCB.rD)
        //  Spill.Sum.TotalMass=*DCB.rD;
        DCB.D=Spill.Sum.TotalMass;
        }
      else
        DCB.D=0.0;
      return 1;
    case xidCnSpillRate:
      if (SpillExists())
        DCB.D=Spill.Cd.QMass();
      else
        DCB.D=0.0;
      return 1;
    case xidCnVentTarget:
      if (DCB.rpC!=NULL && (strlen(DCB.rpC)>0))
        {
        Vent.SetTarget(DCB.rpC);
        }
      if (VentExists())
        DCB.pC=(LPTSTR)Vent.TargetStr(DCB.ForFileSnpScn());
      else
        DCB.pC="";
      return 1;
    case xidCnVentTotal:
      if (VentExists())
        {
        //if (DCB.rD)
        //  Vent.SetTotal(*DCB.rD);
        DCB.D=Vent.Sum.TotalMass;
        }
      else
        DCB.D=0.0;
      return 1;
    case xidCnVentRate:
      if (VentExists())
        DCB.D=Vent.Cd.QMass();
      else
        DCB.D=0.0;
      return 1;
    case xidCnVentLevel:
      if (DCB.rD!=NULL)
        {
        double dVentLevel=Range(0.5, *DCB.rD, 0.99);
        pModel->SetMaxLevel(dVentLevel/pShape->VapVolScale());
        pModel->ClrStatesOK();
        }
      DCB.D = pModel->MaxLevel()*pShape->VapVolScale();
      return 1;
    case xidCnLowestIOPt:
      if (DCB.rD!=NULL)
        {
        pShape->SetLowestIOPt(*DCB.rD);///pShape->VapVolScale());
        pModel->ClrStatesOK();
        }
      DCB.D = pShape->LowestIOPt();//*pShape->VapVolScale();
      return 1;
    case xidCnVentPress:
      if (DCB.rD!=NULL)
        {
        double dVentPress=Range(Std_P+100.0, *DCB.rD, Std_P+20000.0);
        pModel->SetMaxPressure(dVentPress);
        pModel->ClrStatesOK();
        }
      DCB.D = pModel->MaxPressure();
      return 1;

    case xidCnEntVapVol:
      DCB.D = Range(0.0, m_dVapEntrained, pModel->Mass(som_Gas))/GTZ(pModel->Rho(som_Gas));
      return 1;
    case xidCnVFracMM:
      {
      double T=pModel->Mass(som_Gas);
      double E = Range(0.0, m_dVapEntrained, T);
      DCB.D = E/GTZ(T);
      return 1;
      }
    case xidCnVFracVV:
      {
      double GVol = Range(0.0, m_dVapEntrained, pModel->Mass(som_Gas))/GTZ(pModel->Rho(som_Gas));
      double SVol = pModel->Volume(som_SL);
      DCB.D = GVol/GTZ(GVol+SVol);
      return 1;
      }
    case xidCnSLFracVV:
      {
      double GVol = Range(0.0, m_dVapEntrained, pModel->Mass(som_Gas))/GTZ(pModel->Rho(som_Gas));
      double SVol = pModel->Volume(som_SL);
      DCB.D = SVol/GTZ(GVol+SVol);
      return 1;
      }

    case xidCnTopPRqd:
      if (DCB.rD)
        {
        if (Valid(*DCB.rD))
          m_TopPRqd=Range(gs_CDB.MinP, *DCB.rD, gs_CDB.MaxP);
        else
          m_TopPRqd=dNAN;
        }
      DCB.D=m_TopPRqd;
      return 1;
    case xidCnUseRhoH:
      if (DCB.rB)
        m_UseRhoH=*DCB.rB;
      DCB.B=m_UseRhoH;
      return 1;
    case xidCnClosed:
      if (DCB.rB)
        SetClosed(*DCB.rB, DCB.ForView());
      DCB.B=Closed();
      return 1;
    case xidCnMixed:
      if (DCB.rB)
        SetMixed(*DCB.rB);
      DCB.B=Mixed();
      return 1;
    case xidCnNetworked:
      if (DCB.rB)
        SetNetPressDamp(*DCB.rB);
      DCB.B=this->NetPressDamp();
      return 1;
    case xidCnStatsOn:
      if (DCB.rB)
        {
        if (*DCB.rB && pStats==NULL)
          pStats = new CContStatInfo;
        if (*DCB.rB==0 && pStats)
          {
          delete pStats;
          pStats = NULL;
          }
        }
      DCB.B=(pStats!=NULL);
      return 1;
    case xidSpShape:
      ChangeShape(DCB.rpC);
      DCB.pC=pShape->ShortDesc();
      pModel->ShapeChanged();
      return 1;
    case xidSSVapVolScale:
      if (DCB.rD!=NULL)
        SetVapVolScale(*DCB.rD, !DCB.ForFiling());
      DCB.D=pShape->VapVolScale();
      return 1;
    case xidSMDeltaMass:
      DCB.D=pModel->Mass(som_ALL)-dInitMt;
      return 1;
    default:
      if (pModel->DataXchg(DCB)) return 1;
      if (pShape->DataXchg(DCB)) return 1;
    }

  if (pStats)
    {
    if (pStats->DataXchg(DCB, this))
      return 1;
    }

  if (m_MixSet.DataXchg(DCB))
    return 1;

  return SpModelOwner::DataXchg(DCB);
  };

//---------------------------------------------------------------------------

flag SpContainer::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=true;

  m_HighestVapFrac = Range(1.02, m_HighestVapFrac, 1.2);
  m_HighestSLFrac = Range(1.001, m_HighestSLFrac, m_HighestVapFrac);

  if (OrigPrjFileVerNo()<90)
    {
    if (SpillExists() && _stricmp(Spill.AreaTag, "Floor.Content")==0)
      Spill.AreaTag="";
    if (VentExists() && _stricmp(Vent.AreaTag, "VentGas.Content")==0)
      Vent.AreaTag="";
    }

  if (pPreset)
    OK=OK && pPreset->ValidateData(VDB, *this);

  OK = OK && m_MixSet.ValidateData(VDB);

  if (m_HiLvlLimit==1.01 && PrjFileVerNo()<109)
    m_HiLvlLimit=1.02;

  return OK;
  };

// --------------------------------------------------------------------------

flag SpContainer::GetOtherData(FilingControlBlock &FCB)
  {
  DWORD nBytes;
  DWORD CurPos=FCB.SetFilePointer(0, FILE_CURRENT);

  FCB.WriteFile(&SpCn_Start, sizeof(SpCn_Start), &nBytes);

  FCB.WriteFile(&SpCn_Shape, sizeof(SpCn_Shape), &nBytes);
  pShape->GetOtherData(FCB);

  FCB.WriteFile(&SpCn_Model, sizeof(SpCn_Model), &nBytes);
  pModel->GetOtherData(FCB);

  FCB.WriteFile(&SpCn_End, sizeof(SpCn_End), &nBytes);
  return true;
  };

// --------------------------------------------------------------------------

flag SpContainer::PutOtherData(FilingControlBlock &FCB)
  {
  DWORD StartPos=FCB.SetFilePointer(0, FILE_CURRENT);
  DWORD nBytes;
  byte What;
  FCB.ReadFile(&What, sizeof(What), &nBytes);
  if (What!=SpCn_Start)
    {
    FCB.SetFilePointer(StartPos, FILE_BEGIN);
    return false;
    }

  FCB.ReadFile(&What, sizeof(What), &nBytes);
  ASSERT(What==SpCn_Shape);
  pShape->PutOtherData(FCB);

  FCB.ReadFile(&What, sizeof(What), &nBytes);
  ASSERT(What==SpCn_Model);
  pModel->PutOtherData(FCB);

  FCB.ReadFile(&What, sizeof(What), &nBytes);
  ASSERT(What==SpCn_End);

  return true;
  };

//---------------------------------------------------------------------------

void SpContainer::dbgDump(char *Hdr, flag Vert/*=1*/)
  {
  if (Vert)
    {
    dbgpln("SpCn : T:%14g P:%14g  %s", pModel->Temp(), pModel->Press(), Hdr);
    for(int s=0; s<SVValueCount(); s++)
      dbgpln("%-20s %14g", SVImg(s).SymOrTag(), VValue[s]);
    }
  else
    {
    static int ncnt=10000;
    if (ncnt>10)
      {
      dbgp("SpCn :   %14s   %14s ", "","");
      for(int s=0; s<SVValueCount(); s++)
        dbgp(" %14s", SVImg(s).SymOrTag());
      ncnt=0;
      dbgpln("");
      }
    ncnt++;
    dbgp("SpCn : T:%14g P:%14g ", pModel->Temp(), pModel->Press());
    for(int s=0; s<SVValueCount(); s++)
      dbgp(" %14g", VValue[s]);
    dbgpln(" %s", Hdr);
    }
  }

//---------------------------------------------------------------------------

void SpContainer::SetClosed(byte RqdClosed, byte AdjustContents)
  {
  int bWasClosed = Closed();
  if (RqdClosed && !gs_Environment.HasGas())
    {
    LogError(FullObjTag(), 0, "No atmosphere defined");
    pModel->SetClosed(false);
    }
  else if (!bWasClosed && RqdClosed)
    {
    pModel->SetClosed(RqdClosed);
    if (AdjustContents)
      {
      if (Mass(som_SL)>1.0e-6)
        SetRequiredPress(Temp(), AtmosPress(), &gs_Environment);
      else
        SetRequiredPress(AmbientTemp(), AtmosPress(), &gs_Environment);
      }
    }
  else if (bWasClosed && !RqdClosed)
    {
    pModel->SetClosed(RqdClosed);
    if (AdjustContents)
      {
      double t=Temp();
      double p=Press();
      pModel->MArray().ZeroPhase(som_Gas);
      SetTempPress(t, p);
      }
    }
  else
    pModel->SetClosed(RqdClosed);

  if (AdjustContents && !Closed())
    SetPress(AtmosPress());
  };

//---------------------------------------------------------------------------

void SpContainer::SetVolume(double V)
  {
  pShape->SetApparentVolume(V);
  };

//--------------------------------------------------------------------------

void SpContainer::MCB_GetOriginalTP(double &OrigT, double &OrigP)
  {
  OrigT=Temp();
  OrigP=Press();
  }

//--------------------------------------------------------------------------

void SpContainer::MCB_OnSetVolume(flag KeepLPT, double OrigT, double OrigP, double OrigVol, double NewVol)
  {
  if (m_bKeepLPTOnVChange || KeepLPT)
    {
    pModel->ScaleMass(som_ALL, NewVol/Max(1.0e-3, OrigVol));
    pModel->SetTempPress(OrigT, OrigP);
    }
  };

//---------------------------------------------------------------------------

void SpContainer::SetVapVolScale(double S, bool DoAdjustments)
  {
  S=Range(1.0, S, 1.0e6);
  if (pModel->Closed())
    {
    double MaxLvl=pModel->MaxLevel();
    double OldVol=pShape->ActualVolume();
    double OldVVS=pShape->VapVolScale();
    pShape->SetVapVolScale(S);
    pModel->SetMaxLevel(MaxLvl*OldVVS/S);

    if (DoAdjustments)
      {
      double T=pModel->Temp();
      double P=pModel->Press();
      double VSol=pModel->Volume(som_SL);

      double NewVol=OldVol*S/OldVVS;

      pModel->ScaleMass(som_Gas, (NewVol-VSol)/(OldVol-Min(0.999*OldVol, VSol)));
      pModel->SetTempPress(T, P);
      }
    else
      {
      //    double NewVol=OldVol*S/OldVVS;
      }
    }
  else
    pShape->SetVapVolScale(S);
  }

//---------------------------------------------------------------------------

void SpContainer::MCB_OnSetVapVolScl(double OrigScl, double NewScl)
  {
  };

//---------------------------------------------------------------------------

void SpContainer::SetRequiredLevel(double TempRqd, double PressRqd, double LvlRqd, CSysVector *pImg)
  {
  SetHoldVentExpand();

  if (LvlRqd<1.0e-10)
    {
    SetMass(*this, som_SL, 0.0);
    ClrHoldVentExpand();
    return;
    }

  if (pImg)// && (Sp2Adjust>=SDB.No()))
    { // Mixture
    double ImgMass=pImg->Mass(som_SL);

    // If Empty and Preset Exists
    if (ImgMass<=1.0e-6 && pPreset && pPreset->DoImg())
      pImg->Set(*pPreset->Img(), som_SL);
    ImgMass=pImg->Mass(som_SL);
 
    if (ImgMass>1.0e-6)
      {

      //double Rho= //pImg->Rho(som_SL, TempRqd, Std_P);
      double Dens= Rho(som_SL, TempRqd, PressRqd, pImg);
      double RqdMass=Dens*LvlRqd*pShape->ApparentVolume();
      double ImgMass=pImg->Mass(som_SL);
      for (int s=0; s<SVSpcCount(); s++)
        if (SVImg(s).PhaseIs(som_SL))
          SetVMass(s, *pImg, RqdMass*(*pImg)[s]/GTZ(ImgMass));
      SetTempPress(TempRqd, PressRqd);
      //SetTemp(TempRqd);
      ClrHoldVentExpand();
      return;
      }

    //for (Sp2Adjust=0; Sp2Adjust<SDB.No(); Sp2Adjust++)
    //  if (SDB[Sp2Adjust].OccIs(som_Liq))
    //    break;
    LogWarning(FullObjTag(), 0, "No Mixture to Set Level");
    //LogWarning(FullObjTag(), 0, "No Mixture to Set Level : Using first liquid specie");
    }

  //  if (Sp2Adjust>=0 && Sp2Adjust<SDB.No())
  //    {
  //#pragma chCHECKIT(This probably needs to be Converged due to non linear Density(RelSGs))
  //    double Rho= SDB[Sp2Adjust].Density(TempRqd, Std_P, MArray());
  //    double RqdMass=Rho*LvlRqd*pShape->ApparentVolume();
  //    SetSpMass(Sp2Adjust, RqdMass);
  //    }

  SetTempPress(TempRqd, PressRqd);
  ClrHoldVentExpand();
  };

//---------------------------------------------------------------------------

void SpContainer::SetRequiredPress(double TempRqd, double PressRqd, CSysVector *pImg)
  {
  SetHoldVentExpand();
  double GasSpace=pShape->ActualVolume()-Volume(som_SL);
  if (pImg)// && (Sp2Adjust>=SDB.No()))
    { // Mixture
    double ImgMass=pImg->Mass(som_Gas);

    // If Empty and Preset Exists
    if (ImgMass<=1.0e-6 && pPreset && pPreset->DoImg())
      pImg->Set(*pPreset->Img(), som_Gas);
    ImgMass=pImg->Mass(som_Gas);

    if (ImgMass>1.0e-6)
      {
      double Rho= pImg->Rho(Model()->m_iFidelity, Model()->DensityMethod(), som_Gas, TempRqd, PressRqd, &Model()->m_Ovr);
      double RqdMass=Rho*GasSpace;//*PressRqd/Std_P*Std_T/TempRqd;
      double ImgMass=pImg->Mass(som_Gas);
      for (int s=0; s<SVSpcCount(); s++)
        if (SDB[s].PhaseIs(som_Gas))
          SetVMass(s, *pImg, RqdMass*(*pImg)[s]/GTZ(ImgMass));
      SetTempPress(TempRqd, PressRqd);
      // Correct
      double P0=Press();
      if (P0>1.0)
        {
        for (int Iter=0; Iter<20 && fabs(PressRqd-P0)>CSpContPreset::ConvergeTol()*P0; Iter++)
          {
          double Scl=PressRqd/P0;
          ScaleMass(som_Gas, Scl);
          SetTempPress(TempRqd, PressRqd);
          P0=Press();
          }
        }
      ClrHoldVentExpand();
      return;
      }

    //for (Sp2Adjust=0; Sp2Adjust<SDB.No(); Sp2Adjust++)
    //  if (SDB[Sp2Adjust].OccIs(som_Gas))
    //    break;
    //LogWarning(FullObjTag(), 0, "No Mixture to set Pressure : Using first Gas specie");
    LogWarning(FullObjTag(), 0, "No Mixture to set Pressure");
    }

  //  if (Sp2Adjust>=0 && Sp2Adjust<SDB.No())
  //    {
  //#pragma chCHECKIT(This probably needs to be Converged due to non linear Density(RelSGs))
  //    double Rho= SDB[Sp2Adjust].Density(Std_T, Std_P, MArray());
  //    double RqdMass=Rho*GasSpace*PressRqd/Std_P*Std_T/TempRqd;
  //    SetSpMass(Sp2Adjust, RqdMass);
  //    SetTemp(TempRqd);
  //      // Correct
  //    double P0=Press();
  //
  //    if (P0>1.0)
  //      for (int Iter=0; Iter<20 && fabs(PressRqd-P0)>CSpContPreset::ConvergeTol()*P0; Iter++)
  //        {
  //        double Scl=PressRqd/P0;
  //        SetSpMass(Sp2Adjust, SpMass(Sp2Adjust)*Scl);
  //
  //        SetTemp(TempRqd);
  //        P0=Press();
  //
  //        double x0=Mass(som_SL);
  //        double x1=Mass(som_Gas);
  //        int xxx=0;
  //        }
  //    }

  SetTempPress(TempRqd, PressRqd);
  ClrHoldVentExpand();
  };

//---------------------------------------------------------------------------

void SpContainer::SetRequiredConditions(CSpContPreset * PS)
  {
  if (PS==NULL)
    PS=pPreset;
  if (PS)
    {
    SetHoldVentExpand();
    CSysVector  &MA=pModel->MArray();
    SpMArray Img(PS->DoImg() ? (*PS->ImgM()) : MA);
    //SpMArray Img(*PS->ImgM());
    //SpMArray Img(MA);
    if (Closed())
      {
      PS->SetLRqd(Min(PS->LRqd(), 0.99));
      int Iter=0;
      // Pass 0 Get a Good Estimate of Makeup because Large Liquid Mass makes
      // adjustment sluggish
      double LoLvl=0.01;
      int Pass=1;//(PS->LRqd()<LoLvl) ? 1 : 0;

      double L=(Pass==0) ? LoLvl : PS->LRqd();
      ZeroMass();
      //PS->DoImg() ? PS->ImgM() : &Old
      //SetRequiredLevel(PS->TRqd(), L, &Img);
      SetRequiredLevel(PS->TRqd(), PS->PRqd(), L, &Img);
      SetRequiredPress(PS->TRqd(), PS->PRqd(), &Img);

      double MeasP=Press();
      double MeasT=Temp();
      double MeasL=Level();
      if (!PS->TAdj() && !PS->DoVLE())
        PS->SetTRqd(MeasT);

      PS->AdjustConditions(*this);
      MeasP=Press();
      MeasT=Temp();
      MeasL=Level();

      flag PBad;
      flag LBad;
      flag TBad=false;

      for ( ; Pass<2; Pass++)
        {
        //flag PBad=(fabs(MeasP-PS->PRqd())>PS->ConvergeTol()*10.0);
        //flag LBad=(fabs(MeasL-PS->LRqd())>PS->ConvergeTol()*10.0);
        int noChgCnt=0;
        for (Iter=1000; Iter ; Iter--)
          {
          PBad=(fabs(MeasP-PS->PRqd())>PS->ConvergeTol()*10.0);
          LBad=(fabs(MeasL-L)>PS->ConvergeTol()*10.0);
          if (!PBad && !LBad)
            break;
          if (PS->TAdj())
            TBad=(fabs(MeasT-PS->TRqd())>PS->ConvergeTol()*10.0);
          else if (!PS->DoVLE())
            PS->SetTRqd(MeasT);

          //ZeroMass();
          dbgpln("\n");
          dbgpln(" : %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f", PS->TRqd(), PS->PRqd(), Temp(), Press(), Level()*100, MA.Mass(som_SL), MA.Mass(som_Gas), totHf());

          SetRequiredLevel(PS->TRqd(), PS->PRqd(), L, &Img);

          dbgpln("L: %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f", PS->TRqd(), PS->PRqd(), Temp(), Press(), Level()*100, MA.Mass(som_SL), MA.Mass(som_Gas), totHf());

          SetRequiredPress(PS->TRqd(), PS->PRqd(), &Img);

          dbgpln("P: %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f", PS->TRqd(), PS->PRqd(), Temp(), Press(), Level()*100, MA.Mass(som_SL), MA.Mass(som_Gas), totHf(),
            Volume(som_SL, Temp(), Press()),
            Volume(som_Gas, Temp(), Press())            
            );

          if (PS->AdjustConditions(*this))
            {
            // if flashing or similar - update Temporary Image
            Img=MA;
            }
          dbgpln("C: %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f", PS->TRqd(), PS->PRqd(), Temp(), Press(), Level()*100, MA.Mass(som_SL), MA.Mass(som_Gas), totHf());

          double PrevMeasT=MeasT;
          double PrevMeasL=MeasL;
          double PrevMeasP=MeasP;
          MeasP=Press();
          MeasT=Temp();
          MeasL=Level();

          // No Change in Measurements - Stop
          if (fabs(MeasT-PrevMeasT)<1.0e-6*MeasT &&
            fabs(MeasL-PrevMeasL)<1.0e-6*MeasL &&
            fabs(MeasP-PrevMeasP)<1.0e-6*MeasP)
            {
            if (noChgCnt++>3)
              break;
            }
          else
            noChgCnt=0;
          }

        if (Pass==0)
          {
          //Adjust For Actual Level
          MA.Mult(som_Gas, (1-PS->LRqd())/(1-L));
          MA.Mult(som_SL, PS->LRqd()/L);
          pModel->ClrStatesOK();
          SetTempPress(PS->TRqd(), PS->PRqd());
          MeasP=Press();
          MeasT=Temp();
          L=PS->LRqd();
          }
        }

      if (TBad || PBad || TBad)
        LogError(FullObjTag(), 0, "SetRequiredConditions not Converged");
      }
    else
      {
      ZeroMass();
      SetRequiredLevel(PS->TRqd(), PS->PRqd(), PS->LRqd(), &Img);
      }
    ClrHoldVentExpand();
    }
  };

//---------------------------------------------------------------------------

double SpContainer::ResidenceTime(double SigmaQvIn, double SigmaQvOut, PhMask PhaseM)
  {
  return Volume(PhaseM)/GTZ(Min(SigmaQvIn, SigmaQvOut));
  }

//--------------------------------------------------------------------------

//void SpContainer::CreateInFlow()
//  {
//  pInFlow=new SpDirectCd("IFlw", this, TOA_Free);
//  StructureChanged(this);
//  };

//--------------------------------------------------------------------------

//void SpContainer::AddInFlowDeriv()
//  {
//  if (InFlowExists())
//    {
//    SpDirectCd &I=InFlow();
//    if (I.ZeroReqd())
//      {
//      //dbgpln("AddInFlowDeriv -         %14s %s", "Zero", FullObjTag());
//      I.ClrZeroReqd();
//      I.QZero();
//      }
//    else
//      {
//      if (I.QMass()>0)
//        dbgpln("AddInFlowDeriv -         %14.3f %s", I.QMass(), FullObjTag());
//      AddDeriv(I, 1.0);
//      }
//    I.SetZeroReqd();
//    }
//  else
//    {
//    //dbgpln("AddInFlowDeriv -         %14s %s", "Missing", FullObjTag());
//    }
//  }

//--------------------------------------------------------------------------

//void SpContainer::CreateOverFlow()
//  {
//  pOverFlow=new SpDirectCd("OFlw", this, TOA_Free);
//  pOverFlow->Target().SetTag("Floor.Content");
//  StructureChanged(this);
//  };

//--------------------------------------------------------------------------

//flag SpContainer::SpillExists()
//  {
//  return (pOverFlow!=NULL);
//  };
//bool SpContainer::SpillFlowing()
//  {
//  return m_Spill.Cd.QMass(som_ALL)>1.0e-12;
//  }
//SpDirectCd &SpContainer::getxOverFlow
//  {
//  //if (pOverFlow==NULL)
//  //  CreateOverFlow();
//  return m_OverFlow;
//  };
//void SpContainer::SpillClear()
//  {
//  m_Spill.Cd.QSetTraceMass();
//  };

//--------------------------------------------------------------------------

//void SpContainer::CreateGasVent()
//  {
//  pGasVent=new SpDirectCd("GVnt", this, TOA_Free);
//  pGasVent->Target().SetTag("VentGas.Content");
//  StructureChanged(this);
//  };

//--------------------------------------------------------------------------

//flag SpContainer::VentExists()  { return (pGasVent!=NULL);  };
//flag SpContainer::VentFlowing() 
//  { 
//  return m_Vent.Cd.QMass(som_ALL)>1.0e-12; 
//  }
//SpDirectCd &SpContainer::GasVent()
//  {
//  //if (pGasVent==NULL)
//  //  CreateGasVent();
//  return m_GasVent;
//  };
//void SpContainer::GasVentClear()
//  {
//  m_Vent.Cd.QSetTraceMass();
//  };
//
//--------------------------------------------------------------------------

class CPrfPtSlv : public MRootFinderBase
  {
  public:
    static CToleranceBlock s_Tol;
    CCnMixSetEqn * m_pEqn;
    double m_dIFLevel, m_dLevel;

    CPrfPtSlv (CCnMixSetEqn *pEqn, double IFLevel, double Level) : \
      MRootFinderBase("PrfPtSlv", s_Tol), m_pEqn(pEqn), m_dIFLevel(IFLevel), m_dLevel(Level)
      {
      };

    double Function(double x)
      {
      return m_pEqn->GetValue(x, m_dIFLevel, m_dLevel);
      };
  };

CToleranceBlock CPrfPtSlv ::s_Tol(TBF_Both, "CCnMixSetEqn:PrfPtSlv", 0.0, 1.0e-8);

//--------------------------------------------------------------------------

double CCnMixSetEqn::SolveForPrfPt(double PrfPt, double IFLevel, double Level)
  {
  CPrfPtSlv Slv(this, IFLevel, Level);
  Slv.SetTarget(PrfPt);

  int iStrtErr=Slv.Start(0.0, /*1.0*/Level);
  switch (iStrtErr)
    {
    case RF_OK:
      {
      int iRet=Slv.Solve_Brent();
      switch (iRet)
        {
        case RF_OK:
          return Slv.Result();
        default: 
          LogError(Name(), 0, "CCnMixSetEqn Finder not converged"); 
          break;
        }
      break;
      }
    case RF_LoLimit:
      return Slv.Result();
    case RF_HiLimit:
      return Slv.Result();
    }

  return 0;
  };

//--------------------------------------------------------------------------

class CSimpleMixerSettlerEqn : public CCnMixSetEqn
  {
  public:
    double m_dSeparFactor;

    CSimpleMixerSettlerEqn(CCnMixSet *pMS) : CCnMixSetEqn(pMS)
      {
      m_dSeparFactor=10;
      }

    virtual void      BuildDataDefn(DataDefnBlk & DDB)
      {
      DDB.Double("SepFactor", "", DC_, "", &m_dSeparFactor, Cn(), isParm);
      //DDB.Double("HeavyFrac", "", DC_, "", &m_dHeavyFrac,   Cn(), isResult);
      };

    virtual LPCTSTR Name() { return "Simple"; };
    virtual double GetValue(double FracHgt, double IFLevel, double Level)
      {
      double HDiff=FracHgt-IFLevel;
      double Frac;
      if (HDiff<0)
        Frac=IFLevel-IFLevel*(1-exp(HDiff/IFLevel*m_dSeparFactor));
      else
        Frac=IFLevel+(1-IFLevel)*(1-exp(-HDiff/(1-IFLevel)*m_dSeparFactor));
      return Frac;//*Level;
      };
  };

//--------------------------------------------------------------------------

CIArray CCnMixSet::sm_AvailSpcs;

CCnMixSet::CCnMixSet(SpContainer *pCn)
  {
  m_pCn=pCn;
  m_nGrps=2;
  m_iEqn=-1;
  m_DDBGrpsValid=false;
  m_bVolBased=true;

  m_bDisplay=PD_FracPrf|PD_LvlOfFrac;
  m_nHgtPrfPtsRqd=0;
  SetPrfPts(5);
  m_nFracPrfPtsRqd=5;
  for (int i=0; i<MaxMixSetGrps; i++)
    {
    m_dIFLevel[i]=0;
    m_dLtFracPoint[i]=0.95;
    }
  };

CCnMixSet::~CCnMixSet()
  {
  for (int i=0; i<m_Eqns.GetSize(); i++)
    delete m_Eqns[i];
  }

void CCnMixSet::GlblInitialise()
  {
  sm_AvailSpcs.SetSize(0);
  };

void CCnMixSet::GlblTerminate()
  {
  sm_AvailSpcs.SetSize(0);
  };

void CCnMixSet::Initialise()
  {
  ASSERT(m_Eqns.GetSize()==0);

  m_Eqns.Add(new CSimpleMixerSettlerEqn(this));

  if (sm_AvailSpcs.GetSize()==0)
    {
    for (long s=0; s<SDB.Count(); s++)
      {
      if (!SDB[s].IsGas())
        sm_AvailSpcs.Add(s);
      }
    }

  if (sm_AvailSpcs.GetSize()>0)
    SetUpDDBGrps();
  };

//--------------------------------------------------------------------------

void CCnMixSet::SetUpDDBGrps()
  {
  bool InAList[MaxSpeciesEx];
  for (int s=0; s<SDB.Count(); s++)
    InAList[s]=false;

  for (int i=0; i<m_nGrps; i++)
    {
    if (!m_DDBGrpsValid || m_DDBGrpsAdd[i].Length()+m_DDBGrpsRemove[i].Length()<sm_AvailSpcs.GetSize())
      {
      m_DDBGrpsAdd[i].Empty();
      m_DDBGrpsRemove[i].Empty();
      m_DDBGrpsAdd[i].Add(-1, " - ");
      if (i>0)
        m_DDBGrpsAdd[i].Add(-2, "All_Available");
      m_DDBGrpsRemove[i].Add(-1, " - ");

      CIArray &GI=m_GrpIndex[i];

      for (int j=0; j<GI.GetSize(); j++)
        {
        int s=GI[j];
        if (!InAList[s])
          {
          InAList[s]=true;
          m_DDBGrpsRemove[i].Add(s, SDB[s].SymOrTag());
          }
        else
          GI.RemoveAt(j); // allready in a previous list
        }

      for (int j=0; j<sm_AvailSpcs.GetSize(); j++)
        {
        int s=sm_AvailSpcs[j];
        if (!InAList[s])
          m_DDBGrpsAdd[i].Add(s,SDB[s].SymOrTag());
        }
      }
    }
  m_DDBGrpsValid = true;
  }

//--------------------------------------------------------------------------

void CCnMixSet::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginObject(m_pCn, "MixSettler", "SpMixSettle", NULL, DDB_OptPage, -1, MarkerClosed))
    {
    DDBValueLstMem DDBEqns;
    if (m_Eqns.GetSize()>0)
      {
      DDBEqns.Add(-1, "Off");
      for (int i=0; i<m_Eqns.GetSize(); i++)
        DDBEqns.Add(i, (LPSTR)m_Eqns[i]->Name());;
      }
    else
      {
      DDBEqns.Add(-1, "Off");
      DDBEqns.Add(0, "On");
      }

    DDB.Long ("Eqn",     "",     DC_,    "", xidCnMSEqn,  m_pCn, isParm|SetOnChange, &DDBEqns);


    if (On())
      {
      DDB.CheckBoxBtn("VolumeBased", "", DC_, "", &m_bVolBased, m_pCn, isParm);
      Strng S;
      m_Eqns[m_iEqn]->BuildDataDefn(DDB);

      SetUpDDBGrps();
      static DDBValueLst DDBLtFracPrf[]=
        {
          { PD_HgtPrf,                          "HgtPrf"            },
          { PD_FracPrf,                         "FracPrf"           },
          { PD_LvlOfFrac,                       "Interface"         },
          { PD_LvlOfFrac|PD_HgtPrf,             "Interface_HgtPrf"  },
          { PD_LvlOfFrac|PD_FracPrf,            "Interface_FracPrf" },
          { PD_LvlOfFrac|PD_HgtPrf|PD_FracPrf,  "All"               },
          { 0}
        };

      DDB.Byte("PrfDisplay",      "", DC_,  "", &m_bDisplay, m_pCn, isParm|SetOnChange, DDBLtFracPrf);
      DDB.Long("NHgtPrfPtsRqd",   "", DC_,  "", xidCnMSSetPrfCnt, m_pCn, isParm|(m_bDisplay&PD_HgtPrf ? 0:noView)|SetOnChange);
      DDB.Long("NFracPrfPtsRqd",  "", DC_,  "", &m_nFracPrfPtsRqd, m_pCn, isParm|(m_bDisplay&PD_FracPrf ? 0:noView)|SetOnChange);


      for (int i=0; i<m_nGrps; i++)
        {
        Strng GrpTg;
        GrpTg.Set("Grp%i", i+1);
        CIArray &GI=m_GrpIndex[i];
        if (DDB.BeginStruct(m_pCn, GrpTg()))
          {
          if (DDB.ForFileSnpScn())
            {
            DDB.Long  ("NSpcs",       "", DC_,      "",   xidCnMSSetSpcCnt+i, m_pCn, isParm);
            if (DDB.BeginArray(m_pCn, "Spcs", "SpIndex", GI.GetSize(), 0, DDB_NoPage, isParm))
              {
              for (int s=0; s<GI.GetSize(); s++)
                {
                DDB.BeginElement(m_pCn, s);
                DDB.Int("Spc", "", DC_, "", &GI[s], m_pCn, isParmConstruct, &SDB.DDBSpList);
                }
              };
            DDB.EndArray();
            }
          else
            {
            Strng S("Species:");
            bool NeedsComma=false;
            for (int s=0; s<GI.GetSize(); s++)
              {
              if (NeedsComma)
                S+=",";
              S+=SDB[GI[s]].SymOrTag();
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
            DDB.Long  ("Add",       "", DC_,      "",       xidCnMSAddSpc+i, m_pCn, (m_DDBGrpsAdd[i].Length()>1?isParm:0)|SetOnChange, &m_DDBGrpsAdd[i]);
            DDB.Long  ("Remove",    "", DC_,      "",       xidCnMSRemSpc+i, m_pCn, (m_DDBGrpsRemove[i].Length()>1?isParm:0)|SetOnChange, &m_DDBGrpsRemove[i]);
            }
          //DDB.Text  ("");
          DDB.Double("Density",   "", DC_Rho,   "kg/m^3", xidCnMSDensity+i, m_pCn, 0);
          DDB.Double("MassFrac",  "", DC_Frac,  "%",      xidCnMSMassFrac+i, m_pCn, InitHidden);
          DDB.Double("Mass",      "", DC_M,     "kg",     xidCnMSMassTotal+i, m_pCn, InitHidden);
          DDB.Double("VolFrac",   "", DC_Frac,  "%",      xidCnMSVolumeFrac+i, m_pCn, 0);
          DDB.Double("Volume",    "", DC_Vol,   "m^3",    xidCnMSVolumeTotal+i, m_pCn, 0);
          //if (i<m_nGrps-1)
          //  DDB.Double("IFLevel",  "", DC_Frac,  "%",      xidCnMSIFLevel+i, m_pCn, 0);

          if (i<m_nGrps-1)
            {
            DDEF_Flags Flags=DDB.GetVisibility();

            DDB.Visibility(NM_Dynamic|SM_All|HM_All, (m_bDisplay&PD_LvlOfFrac)!=0);
            DDB.Text("Interface");
            DDB.Double("MonitorPnt",        "", DC_Frac, "%", &m_dLtFracPoint[i],     m_pCn, isParm);
            DDB.Double("MonitorLvl",        "", DC_Frac, "%", xidCnMSLtFracLvlPt+i,   m_pCn, isResult);
            DDB.Double("IfPtOfInflection",  "", DC_Frac, "%", &m_dIFLevel[i],         m_pCn, 0|InitHidden);

            DDB.Visibility(NM_Dynamic|SM_All|HM_All, (m_bDisplay&(PD_HgtPrf|PD_FracPrf))!=0);
            if (m_bDisplay&PD_HgtPrf && !DDB.ForFileSnpScn())
              {
              DDB.Text("HgtProfile");
              long iTop=m_Prf.GetSize()-1;
              for (int j=iTop; j>=0; j--)
                {
                S.Set("FracPt%i", j+1);
                DDB.Double(S(),   "", DC_Frac, "%", xidCnMSIOLtFrac+j*MaxMixSetGrps+i,  m_pCn, isResult|noFileAtAll);

                switch (m_Prf[j].m_iType)
                  {
                  case CCnMixSetPrfPt::eSurface:  S.Set("@%.1f%% Surface", m_Prf[j].m_dHeight*100);     break;
                  case CCnMixSetPrfPt::ePt:       S.Set("@%.1f%%", m_Prf[j].m_dHeight*100);     break;
                  case CCnMixSetPrfPt::eIO:       S.Set("@%.1f%% IO", m_Prf[j].m_dHeight*100);  break;
                  }
                DDB.TagComment(S());
                }
              }
            if (m_bDisplay&PD_FracPrf && !DDB.ForFileSnpScn())
              {
              DDB.Text("LtFracProfile");
              long nPts=Max(2L,m_nFracPrfPtsRqd);
              for (int j=nPts-1; j>=0; j--)
                {
                S.Set("LvlPt%i", j+1);
                DDB.Double(S(),   "", DC_Frac, "%", xidCnMSLtFracLvl+j*MaxMixSetGrps+i,  m_pCn, isResult);
                S.Set("Of%.1f%%",  j*100.0/(nPts-1));     
                DDB.TagComment(S());
                }
              }
            DDB.SetVisibility(Flags);
            }
          }
        DDB.EndStruct();
        }
      }
    }
  DDB.EndObject();
  };

flag CCnMixSet::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidCnMSEqn:
      if (DCB.rL)
        SelectEqn(*DCB.rL);
      DCB.L=m_iEqn;
      return 1;
    case xidCnMSSetPrfCnt:
      if (DCB.rL)
        SetPrfPts(*DCB.rL);
      DCB.L=m_nHgtPrfPtsRqd;
      return 1;
    default:
      if (DCB.lHandle>=xidCnMSSetSpcCnt && DCB.lHandle<xidCnMSSetSpcCnt+MaxMixSetGrps)
        {
        CIArray &GI=m_GrpIndex[DCB.lHandle-xidCnMSSetSpcCnt];
        if (DCB.rL)
          {
          GI.SetSize(*DCB.rL);
          m_DDBGrpsValid = false;
          }
        DCB.L=GI.GetSize();
        return 1;
        }
      else if (DCB.lHandle>=xidCnMSAddSpc && DCB.lHandle<xidCnMSAddSpc+MaxMixSetGrps)
        {
        CIArray &GI=m_GrpIndex[DCB.lHandle-xidCnMSAddSpc];
        if (DCB.rL)
          {
          if (*DCB.rL>=0)
            {
            for (int j=0; j<GI.GetSize(); j++)
              {
              if (GI[j]>*DCB.rL)
                {
                GI.InsertAt(j, *DCB.rL);
                m_DDBGrpsValid=false;
                DCB.L=-1;
                return 1;
                }
              }
            GI.Add(*DCB.rL);
            m_DDBGrpsValid=false;
            DCB.L=-1;
            return 1;
            }
          else if (*DCB.rL==-2)
            {
            int iGI=DCB.lHandle-xidCnMSAddSpc;
            bool InList[MaxSpecies];
            for (int i=0; i<SDB.Count(); i++)
              InList[i]=false;
            for (int i=0; i<iGI; i++)
              {
              CIArray &GIx=m_GrpIndex[i];
              for (int j=0; j<GIx.GetSize(); j++)
                InList[GIx[j]]=true;
              }

            GI.SetSize(0);
            for (int i=0; i<sm_AvailSpcs.GetSize(); i++)
              if (!InList[sm_AvailSpcs[i]])
                GI.Add(sm_AvailSpcs[i]);
            m_DDBGrpsValid=false;
            DCB.L=-1;
            return 1;
            }
          }
        DCB.L=-1;
        return 1;
        }
      else if (DCB.lHandle>=xidCnMSRemSpc && DCB.lHandle<xidCnMSRemSpc+MaxMixSetGrps)
        {
        CIArray &GI=m_GrpIndex[DCB.lHandle-xidCnMSRemSpc];
        if (DCB.rL)
          {
          if (*DCB.rL>=0)
            {
            for (int j=0; j<GI.GetSize(); j++)
              {
              if (GI[j]==*DCB.rL)
                {
                GI.RemoveAt(j);
                m_DDBGrpsValid=false;
                DCB.L=-1;
                return 1;
                }
              }
            DCB.L=-1;
            return 1;
            }
          }
        DCB.L=-1;
        return 1;
        }
      else if (DCB.lHandle>=xidCnMSLtFracLvlPt&& DCB.lHandle<xidCnMSLtFracLvlPt+MaxMixSetGrps)
        {
        DCB.D=GrpPrfPtLevel(DCB.lHandle-xidCnMSLtFracLvlPt);
        return 1;
        }
      else if (DCB.lHandle>=xidCnMSDensity && DCB.lHandle<xidCnMSDensity+MaxMixSetGrps)
        {
        DCB.D=GrpDensity(DCB.lHandle-xidCnMSDensity);
        return 1;
        }
      else if (DCB.lHandle>=xidCnMSMassFrac && DCB.lHandle<xidCnMSMassFrac+MaxMixSetGrps)
        {
        DCB.D=GrpMassFrac(DCB.lHandle-xidCnMSMassFrac);
        return 1;
        }
      else if (DCB.lHandle>=xidCnMSMassTotal && DCB.lHandle<xidCnMSMassTotal+MaxMixSetGrps)
        {
        DCB.D=GrpMassTotal(DCB.lHandle-xidCnMSMassTotal);
        return 1;
        }
      else if (DCB.lHandle>=xidCnMSVolumeFrac && DCB.lHandle<xidCnMSVolumeFrac+MaxMixSetGrps)
        {
        DCB.D=GrpVolumeFrac(DCB.lHandle-xidCnMSVolumeFrac);
        return 1;
        }
      else if (DCB.lHandle>=xidCnMSVolumeTotal && DCB.lHandle<xidCnMSVolumeTotal+MaxMixSetGrps)
        {
        DCB.D=GrpVolumeTotal(DCB.lHandle-xidCnMSVolumeTotal);
        return 1;
        }
      else if (DCB.lHandle>=xidCnMSIFLevel && DCB.lHandle<xidCnMSIFLevel+MaxMixSetGrps)
        {
        DCB.D=GrpIFLevel(DCB.lHandle-xidCnMSIFLevel);
        return 1;
        }
      else if (DCB.lHandle>=xidCnMSIOHgt && DCB.lHandle<xidCnMSIOHgt+MaxMixSetGrps*MaxMixSetPrfPts)
        {
        DCB.D=m_Prf[DCB.lHandle>=xidCnMSIOHgt].m_dHeight;
        return 1;
        }
      else if (DCB.lHandle>=xidCnMSIOLtFrac && DCB.lHandle<xidCnMSIOLtFrac+MaxMixSetGrps*MaxMixSetPrfPts) 
        {
        int i=DCB.lHandle-xidCnMSIOLtFrac;
        DCB.D=PrfLtFrac(i%MaxMixSetGrps, i/MaxMixSetGrps);
        return 1;
        }
      else if (DCB.lHandle>=xidCnMSLtFracLvl && DCB.lHandle<xidCnMSLtFracLvl+MaxMixSetGrps*MaxMixSetPrfPts) 
        {
        int i=DCB.lHandle-xidCnMSLtFracLvl;
        DCB.D=GrpPrfPtLevel(i%MaxMixSetGrps, i/MaxMixSetGrps);
        return 1;
        }

    }

  if (On())
    {
    m_Eqns[m_iEqn]->DataXchg(DCB);
    }

  return 0;
  };

flag CCnMixSet::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=1;
  if (On())
    {
    OK=m_Eqns[m_iEqn]->ValidateData(VDB);
    }
  return OK;
  };

//--------------------------------------------------------------------------

void CCnMixSet::SelectEqn(long i)
  {
  if (m_Eqns.GetSize()==0)
    Initialise();

  m_iEqn=Range(-1L,i,m_Eqns.GetSize()-1L);
  }

//--------------------------------------------------------------------------

void CCnMixSet::StartStep()
  {
  }

//--------------------------------------------------------------------------

flag CCnMixSet::On()
  {
  return m_iEqn>=0;
  }

//--------------------------------------------------------------------------

flag CCnMixSet::GetImage(double FracHgt, CSysVector &Img)
  {
  ASSERT(m_iEqn>=0 && m_iEqn<m_Eqns.GetSize());
  CCnMixSetEqn * pEqn=m_Eqns[m_iEqn];

  double   GrpDens[MaxMixSetGrps];
  double   GrpVol[MaxMixSetGrps];
  double   GrpMass[MaxMixSetGrps];
  //double   GrpFrac[MaxMixSetGrps];
  //SpMArray Ms[MaxMixSetGrps];

  Img = m_pCn->MArray();
  Img.Normalise();

  for (int iSwap=0; iSwap<10; iSwap++)
    {
    for (int i=0; i<m_nGrps; i++)
      {
      CIArray &GI=m_GrpIndex[i];
      GrpMass[i]=Img.Mass(GI);
      GrpVol[i]=Img.Volume(m_pCn->Fidelity(), m_pCn->DensityMethod(), GI, m_pCn->Temp(), m_pCn->Press(), NULL);
      GrpDens[i]=GrpMass[i]/GTZ(GrpVol[i]);
      }

    bool Swapped=false;
    for (int i=1; i<m_nGrps; i++)
      {
      if (GrpDens[i]<GrpDens[i-1])
        {
        CIArray GI=m_GrpIndex[i];
        m_GrpIndex[i]=m_GrpIndex[i-1];
        m_GrpIndex[i-1]=GI;
        Swapped=true;
        }
      }
    if (!Swapped)
      break;
    }

  double Level=m_pCn->Level();
  double TotalVolume=m_pCn->Volume();
  double MSVolume=0;
  for (int i=0; i<m_nGrps; i++)
    MSVolume+=GrpVol[i];
  double OtherVolume=TotalVolume-MSVolume;

  // Start At Bottom

  double IFBelow=0;
  for (int i=m_nGrps-2; i>=0; i--)
    {
    int iLight=i;
    int iHeavy=i+1;

    m_dIFLevel[i]=IFBelow+GrpVol[iHeavy]/GTZ(GrpVol[iHeavy]+GrpVol[iLight])*Level;
    IFBelow=m_dIFLevel[i];

    double *GrpFrac = m_bVolBased ? GrpVol : GrpMass;

    double TotalFrac = GrpFrac[iHeavy]+GrpFrac[iLight];

    double LightFrac = pEqn->GetValue(FracHgt, m_dIFLevel[i], Level);
    double ReqdLightPortion=TotalFrac*LightFrac;
    double ReqdHeavyPortion=TotalFrac-ReqdLightPortion;

    //dbgpln("FracHgt   Ht:%10.4f IF:%10.4f ", FracHgt*100, m_dIFLevel*100);
    //dbgpln("          Lt:%10.4f Hv:%10.4f Rp%10.4f Cp%10.4f", Frac[iLight]*100, Frac[iHeavy]*100, ReqdLightPortion*100, ReqdHeavyPortion*100);

    for (int j=0; j<m_GrpIndex[iLight].GetSize(); j++)
      {
      int s=m_GrpIndex[iLight][j];
      Img.SetVMass(s, Img, Img[s]*ReqdLightPortion/GTZ(GrpFrac[iLight]));
      }
    for (int j=0; j<m_GrpIndex[iHeavy].GetSize(); j++)
      {
      int s=m_GrpIndex[iHeavy][j];
      Img.SetVMass(s, Img, Img[s]*ReqdHeavyPortion/GTZ(GrpFrac[iHeavy]));
      }
    }

  SetPrfPtAt(CCnMixSetPrfPt::eIO, FracHgt);
  SetPrfPtAt(CCnMixSetPrfPt::eSurface, Level);
  return 1;
  };

//--------------------------------------------------------------------------

double CCnMixSet::GrpMassFrac(long Index)
  {
  //CStopWatch SW;
  //SW.Start();

  CSysVector &Img = m_pCn->MArray();
  double M=Img.Mass(m_GrpIndex[Index]), Mt=0;
  for (int i=0; i<m_nGrps; i++)
    Mt+=Img.Mass(m_GrpIndex[i]);
  return M/GTZ(Mt);
  }

//--------------------------------------------------------------------------

double CCnMixSet::GrpMassTotal(long Index)
  {
  //CStopWatch SW;
  //SW.Start();
  CSysVector &Img = m_pCn->MArray();
  double M=Img.Mass(m_GrpIndex[Index]), Mt=0;
  return M;
  }

//--------------------------------------------------------------------------

double CCnMixSet::GrpVolumeFrac(long Index)
  {
  //CStopWatch SW;
  //SW.Start();

  CSysVector &Img = m_pCn->MArray();
  SpModel & Mdl=*m_pCn->pModel;
  double V=Img.Volume(Mdl.Fidelity(), Mdl.DensityMethod(), m_GrpIndex[Index], Mdl.Temp(), Mdl.Press(), NULL);
  double Vt=0;
  for (int i=0; i<m_nGrps; i++)
    Vt+=Img.Volume(Mdl.Fidelity(), Mdl.DensityMethod(), m_GrpIndex[i], Mdl.Temp(), Mdl.Press(), NULL);
  return V/GTZ(Vt);
  }

//--------------------------------------------------------------------------

double CCnMixSet::GrpVolumeTotal(long Index)
  {
  //CStopWatch SW;
  //SW.Start();

  CSysVector &Img = m_pCn->MArray();
  SpModel & Mdl=*m_pCn->pModel;
  double V=Img.Volume(Mdl.Fidelity(), Mdl.DensityMethod(), m_GrpIndex[Index], Mdl.Temp(), Mdl.Press(), NULL);
  return V;
  }

//--------------------------------------------------------------------------

double CCnMixSet::GrpDensity(long Index)
  {
  CIArray &GI=m_GrpIndex[Index];
  double   Vol;
  SpMArray Ms;
  CSysVector &Img = m_pCn->MArray();
  for (int j=0; j<GI.GetSize(); j++)
    {
    int s=GI[j];
    Ms.SetVMass(s, Img, Img[s]);
    }
  Vol=m_pCn->Volume(som_SL, m_pCn->Temp(), m_pCn->Press(), &Ms);
  return Ms.Mass()/GTZ(Vol);
  }

//--------------------------------------------------------------------------

double CCnMixSet::GrpIFLevel(long Index)
  {
  CIArray &GI=m_GrpIndex[Index];
  double   M=0,Mt=0;
  CSysVector  &Img = m_pCn->MArray();
  for (int i=0; i<m_nGrps; i++)
    {
    if (i<=Index)
      {
      for (int j=0; j<GI.GetSize(); j++)
        M+=Img[(int)GI[j]];
      }
    CIArray &GIx=m_GrpIndex[i];
    for (int j=0; j<GIx.GetSize(); j++)
      Mt+=Img[(int)GIx[j]];
    }
  return M/GTZ(Mt)*m_pCn->Level();
  }

//--------------------------------------------------------------------------

double CCnMixSet::GrpPrfPtLevel(long Index)
  {
  ASSERT(m_iEqn>=0 && m_iEqn<m_Eqns.GetSize());
  CCnMixSetEqn * pEqn=m_Eqns[m_iEqn];
  return pEqn->SolveForPrfPt(m_dLtFracPoint[Index], m_dIFLevel[Index], m_pCn->Level());
  }

//--------------------------------------------------------------------------

double CCnMixSet::GrpPrfPtLevel(long GrpIndex, long PrfIndex)
  {
  ASSERT(m_iEqn>=0 && m_iEqn<m_Eqns.GetSize());
  CCnMixSetEqn * pEqn=m_Eqns[m_iEqn];
  return pEqn->SolveForPrfPt(((double)PrfIndex)/(m_nFracPrfPtsRqd-1), m_dIFLevel[GrpIndex], m_pCn->Level());
  };

//--------------------------------------------------------------------------

double CCnMixSet::PrfLtFrac(long GrpIndex, long PrfIndex)
  {
  ASSERT(m_iEqn>=0 && m_iEqn<m_Eqns.GetSize());
  CCnMixSetEqn * pEqn=m_Eqns[m_iEqn];
  return pEqn->GetValue(m_Prf[PrfIndex].m_dHeight, m_dIFLevel[GrpIndex], m_pCn->Level());
  };

//--------------------------------------------------------------------------

void CCnMixSet::SetPrfPtAt(CCnMixSetPrfPt::ePtType Type, double H)
  {
  int N=m_Prf.GetSize(), i;

  switch (Type)
    {
    case CCnMixSetPrfPt::eSurface:
      {
      for (i=0; i<N; i++)
        if (m_Prf[i].m_iType==CCnMixSetPrfPt::eSurface)
          {
          m_Prf[i].m_dHeight=H;
          break;
          }
        if (i==N)
          m_Prf.Add(CCnMixSetPrfPt(Type, H));
      }
    break;
    case CCnMixSetPrfPt::eIO:
    case CCnMixSetPrfPt::ePt:
      for (i=0; i<N; i++)
        if ((m_Prf[i].m_iType==CCnMixSetPrfPt::eIO || m_Prf[i].m_iType==CCnMixSetPrfPt::ePt) && fabs(m_Prf[i].m_dHeight-H)<0.0001)
          {
          m_Prf[i].m_iType=Type;
          m_Prf[i].m_dHeight=H;
          break;
          }
        if (i==N)
          m_Prf.Add(CCnMixSetPrfPt(Type, H));
        break;
    }

  //sort
  for (int i=1; i<N; i++)
    {
    for (int j=i; j>0; j--)
      {
      if (m_Prf[j].m_dHeight<m_Prf[j-1].m_dHeight)
        {
        CCnMixSetPrfPt P=m_Prf[j];
        m_Prf[j]=m_Prf[j-1];
        m_Prf[j-1]=P;
        }
      else
        break;
      }
    }
  };

//--------------------------------------------------------------------------

void CCnMixSet::SetPrfPts(long N)
  {
  m_Prf.SetSize(0);
  N=Max(N,2L);
  m_nHgtPrfPtsRqd=N;
  for (int i=0; i<N; i++)
    SetPrfPtAt(CCnMixSetPrfPt::ePt, i/(N-1.0));
  };

//--------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------

void SpContainer::SetState(eScdMdlStateActs RqdState)
  {
  switch (RqdState)
    {
    case MSA_Empty:
      ZeroMass();
      ZeroDeriv();
      if (SpillExists())
        {
        Spill.Cd.QZero();
        Spill.Sum.ZeroTotals();
        }
      if (VentExists())
        {
        Vent.Cd.QZero();
        Vent.Sum.ZeroTotals();
        }
      if (Closed())
        SetRequiredPress(AmbientTemp(), AtmosPress(), &gs_Environment);
       else
         SetTempPress(AmbientTemp(), AtmosPress());
      break;
    case MSA_PreSet:
      SetRequiredConditions();
      if (SpillExists())
        Spill.Sum.ZeroTotals();
      if (VentExists())
        Vent.Sum.ZeroTotals();
      break;
    case MSA_ZeroFlows:
      if (SpillExists())
        Spill.Cd.QZero();
      if (VentExists())
        Vent.Cd.QZero();
      break;
    case MSA_EmptySpillTargets:
      if (SpillExists())
        {
        Spill.Cd.QZero();
        Spill.Sum.ZeroTotals();
        }
      if (VentExists())
        {
        Vent.Cd.QZero();
        Vent.Sum.ZeroTotals();
        }
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    case MSA_DynStatsRunInit:
      if (pStats)
        pStats->ResetStats();
      dInitMt = pModel->Mass(som_ALL);
      /*if (SpillExists())
        Spill.Sum.ZeroTotals();
      if (VentExists())
        Vent.Sum.ZeroTotals();*/
      break;
    }
  };

//--------------------------------------------------------------------------

void SpContainer::ZeroMass()
  {
  pModel->ZeroMass();
  m_dVapEntrained=0.0;
  };

//--------------------------------------------------------------------------

void SpContainer::ZeroDeriv()
  {
  pModel->ZeroDeriv();
  m_dVapEntrainRate=0.0;
  };

//--------------------------------------------------------------------------

void SpContainer::Copy(SpContainer& C)
  {
  CopyModel(C.pModel);
  pModel->Copy(C.pModel);

  m_dVapEntrained     = C.m_dVapEntrained;

  m_dVapEntrainRate   = C.m_dVapEntrainRate;
  m_dVapEscapeTau     = C.m_dVapEscapeTau;
  m_dVapLiberationTau = C.m_dVapLiberationTau;

  m_eVapEntrainCtrl    = C.m_eVapEntrainCtrl;
  m_dVapEntrainVolFrac = C.m_dVapEntrainVolFrac;

  }

//--------------------------------------------------------------------------

void SpContainer::AddDeriv(SpConduit & Flw, double Sgn_, double EntrainRate)
  {
  double d=Flw.QMass();
  pModel->AddDeriv(Flw.Model(), Sgn_);
  //if (Sgn_>0)
  m_dVapEntrainRate+=/*Flw.QMass()*/EntrainRate;
  };

//--------------------------------------------------------------------------

void SpContainer::AddDiscrete(SpConduit & Flw, double Sgn_)
  {
  pModel->AddDiscrete(Flw.Model(), Sgn_);
  };

//--------------------------------------------------------------------------

void SpContainer::ApplyDerivs(double dTime, flag DoDbg)
  {
  pModel->ApplyDerivs(dTime, DoDbg);

  // Derivative     dVapEntrainRate;
  m_dVapEntrained+=m_dVapEntrainRate*dTime;
  switch (m_eVapEntrainCtrl)
    {
    case VEC_VolFrac:
      {
      double GasVol=m_dVapEntrained/GTZ(pModel->Rho(som_Gas));
      double SLVol=pModel->Volume(som_SL);
      double GasVolFrac=GasVol/GTZ(GasVol+SLVol);
      double Diff=(GasVolFrac-m_dVapEntrainVolFrac);
      m_dVapEntrained-=Diff*m_dVapEntrained;//*pModel->Rho(som_Gas);//*(1.0-exp(-dTime/Max(0.001,m_dVapLiberationTau)));
      break;
      }
    default:
      m_dVapEntrained-=m_dVapEntrained*(1.0-exp(-dTime/Max(0.001,m_dVapLiberationTau)));
      break;
    };
  // Limit to 80 %
  m_dVapEntrained=Range(0.0, m_dVapEntrained, 0.9999999999/*0.8*/*Mass(som_Gas));
  };

//--------------------------------------------------------------------------

void SpContainer::ODEOperate(CODEDataBlock & ODB)
  {
  pModel->ODEOperate(ODB);
  ODB.m_pFn(ODB, m_States);
  #if dbgDumpSpcInfo
  if (0)
    {
    switch (ODB.m_Cmd)
      {
  //eStateNull, 
  //eStateAdvance, 
  //eStateConverge, 
  //eStateFixDV,
  //eStateRecon, 
  //eStateTest, 
  //eStateSave, 
  //eStateLoad, 
  //eStateDiskSave, 
  //eStateDiskLoad,
      case eStateSave: dbgCnState("Saved",  FullObjTag(), *this, 1); break;
      case eStateLoad: dbgCnState("Loaded", FullObjTag(), *this, 1); break;
      }
    }
  #endif
  };

//--------------------------------------------------------------------------

void SpContainer::EvalStatistics(eScdCtrlTasks Tasks)
  {
  if (pStats)
    pStats->ExecIns(this);
  };

//--------------------------------------------------------------------------

//double SpContainer::MCB_GetVolume() { return ShapeVolume(); };
//double SpContainer::MCB_SetVolume(double V) { SetVolume(V); return ShapeVolume(); };

//--------------------------------------------------------------------------

void SpContainer::InitMeanResTimeCalcs()
  {
  SpMRTWrk=MArray();
  };

//--------------------------------------------------------------------------

void SpContainer::CompleteMeanResTimeCalcs(SpConduit &Fd, SpConduit &Pr)
  {
  SpVector &Mass0=SpMRTWrk;
  CSysVector &Mass1=MArray();

  double DT=ICGetTimeInc();
  for (int s=0; s<SDB.Count(); s++)
    {
    double M0=Mass0[s]/SDB[s].MoleWt();
    double M1=Mass1[s]/SDB[s].MoleWt();
    double Miq=Fd.VMole[s]*DT;
    double Moq=Pr.VMole[s]*DT;
    double Mi=Miq;
    double Mo=Moq;
    double dMq=Miq-Moq;
    double dMr;
    if (dMq<0)
      {
      dMr=GEZ(M1-M0-dMq);
      Mi+=dMr;
      }
    else
      {
      dMr=LEZ(M1-M0+dMq);
      Mo+=-dMr;
      }
    double M=Max(M0, M1);
    double XX=SpMRT[s];
    SpMRT.VValue[s] = Max(0.01, GEZ((M-Mo)*(SpMRT[s]+DT)+Mi*DT)/GTZ(M-Mo+Mi));

#if dbgSpCont
    if (dbgResTime())
      dbgpln("ResTime: %-15.15s M:%14.6g,%14.6g si:%14.6g,%14.6g  so:%14.6g,%14.6g  d:%+14.6g,%+14.6g  m:%14.6g  rt:%14.6g->%14.6g",
      SDB[s].SymOrTag(), M0,M1,Miq,Mi,Moq,Mo,dMr,Mi-Mo,M,XX,SpMRT[s]);
#endif
    }
  };

//--------------------------------------------------------------------------

int SpContainer::TestLevelLimits()
  {
  m_LvlHi = (Valid(m_HiLvlLimit) && (Level()>m_HiLvlLimit));
  m_LvlLo = (Valid(m_LoLvlLimit) && (Level()<m_LoLvlLimit));
  SetCI(8,m_LmtAsStatus && m_LvlHi);
  SetCI(9,m_LmtAsStatus && m_LvlLo);
  if (m_LvlHi) return 1;
  if (m_LvlLo) return -1;
  return 0;
  };

//--------------------------------------------------------------------------

flag SpContainer::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="N\tSpilling";                           return 1;
    case 2: pS="E\tSpill Targeted to Self";             return 1;
    case 3: pS="E\tSpill Target xxx not Found";         return 1;
    case 4: pS="N\tVenting";                            return 1;
    case 5: pS="E\tVent Targeted to Self";              return 1;
    case 6: pS="E\tVent Target xxx not Found";          return 1;
    case 7: pS="E\tStream for level control not set";   return 1;
    case 8: pS="W\tHigh Level";                         return 1;
    case 9: pS="W\tLow Level";                          return 1;
    default:
      return SpModelOwner::CIStrng(No, pS);
    }
  };

// ===========================================================================
//
//                              Specie Image
//
// ===========================================================================

CSpView SpImage::GlblView(SVV_AsMassFrac);

IMPLEMENT_SPARES(SpImage, 100);
IMPLEMENT_TAGOBJ(SpImage, "SpImage", "SpImage", "", "", "", TOC_SYSTEM,
                 "Specie Image", "Specie Image");

SpImage::SpImage(pTagObjClass pClass_, pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
SpModelOwner(pClass_, pTag_, pAttach, eAttach, false)
  {
  m_Qm_Rqd=0.0;
  m_Qv_Rqd=0.0;
  m_NQv_Rqd=0.0;
  m_P_Rqd=Std_P;
  m_T_Rqd=Std_T;
  m_iQRqdMode=SPI_QModeNone;
  m_iQRqdSet=SPI_QModeNone;

  pModel=(SpModel*)SpModelClass.ConstructGroup(DefaultModel(), NULL, SpModelDefTag, this, TOA_Embedded|TOA_Hidden);
  pModel->m_pGlblView=&GlblView;
  SetView(SVV_AsMassFrac);
  SetUsage(SPMU_Image);
  pViewBlk=NULL;
  iViewBlkSize=0;

  bSpeciesOnly=false;
  pModel->m_bKeepNormalEnabled=true;

  SetQmRqdTo(0.0, true, true);
  };

// --------------------------------------------------------------------------

SpImage::SpImage(pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) : \
SpModelOwner(&SpImageClass, pTag_, pAttach, eAttach, false)
  {
  m_Qm_Rqd=0.0;
  m_Qv_Rqd=0.0;
  m_NQv_Rqd=0.0;
  m_P_Rqd=Std_P;
  m_T_Rqd=Std_T;
  m_iQRqdMode=SPI_QModeNone;
  m_iQRqdSet=SPI_QModeNone;

  pModel=(SpModel*)SpModelClass.ConstructGroup(DefaultModel(), NULL, SpModelDefTag, this, TOA_Embedded|TOA_Hidden);
  pModel->m_pGlblView=&GlblView;
  SetView(SVV_AsMassFrac);
  SetUsage(SPMU_Image);
  pViewBlk=NULL;
  iViewBlkSize=0;
  
  bSpeciesOnly=false;
  pModel->m_bKeepNormalEnabled=true;

  SetQmRqdTo(0.0, true, true);
  };

// --------------------------------------------------------------------------

SpImage::~SpImage()
  {
  pModel->Destroy();
  };

void SpImage::SetTRqdTo(double TRqd) 
  { 
  m_T_Rqd=TRqd; 
  };
void SpImage::SetPRqdTo(double PRqd)
  { 
  m_P_Rqd=PRqd; 
  };
void SpImage::SetQMode(SpImgMode What)    
  { 
  m_iQRqdMode=What; 
  };

void SpImage::SetQmRqdTo(double QmRqd, flag SetSet, flag SetMode)
  {
  //dbgpln("SetQmRqdTo=%s",DbgFltString(QmRqd));
  if (Valid(QmRqd))
    {
    m_Qm_Rqd=QmRqd;
    if (SetSet)
      m_iQRqdSet=SPI_QModeQm;  
    if (SetMode) 
      m_iQRqdMode=SPI_QModeQm;  
    }
  else
    {
    m_iQRqdSet=SPI_QModeNone;
    m_iQRqdMode=SPI_QModeNone;
    }
  };

void SpImage::SetQvRqdTo(double QvRqd, flag SetSet, flag SetMode)
  {
  if (Valid(QvRqd))
    {
    m_Qv_Rqd=QvRqd;  
    if (SetSet)
      m_iQRqdSet=SPI_QModeQv;
    if (SetMode)
      m_iQRqdMode=SPI_QModeQv;  
    }
  else
    {
    m_iQRqdSet=SPI_QModeNone;
    m_iQRqdMode=SPI_QModeNone;
    }
  };

void SpImage::SetNQvRqdTo(double QvRqd, flag SetSet, flag SetMode)
  {
  if (Valid(QvRqd))
    {
    m_NQv_Rqd=QvRqd;
    if (SetSet)
      m_iQRqdSet=SPI_QModeNQv;
    if (SetMode) 
      m_iQRqdMode=SPI_QModeNQv;
    }
  else
    {
    m_iQRqdSet=SPI_QModeNone;
    m_iQRqdMode=SPI_QModeNone;
    }
  };

// --------------------------------------------------------------------------

static const BDDSpecies_Blk sbImage[]=
  {
    {SVV_AsMassFlow,    SVV_AsMassFrac,  true,  false, VAMsk_SpMass },
    {SVV_AsMassFrac,    SVV_AsMassFrac,  true,  true , VAMsk_SpMass },
    {SVV_AsMoleFlow,    SVV_AsMassFrac,  true,  false, VAMsk_SpMole },
    {SVV_AsMoleFrac,    SVV_AsMassFrac,  true,  false, VAMsk_SpMole },
    {SVV_AsVolFlow,     SVV_AsMassFrac,  true,  false, VAMsk_SpVol  },
    {SVV_AsVolFrac,     SVV_AsMassFrac,  true,  false, VAMsk_SpVol  },
    {SVV_AsNVolFlow,    SVV_AsMassFrac,  true,  false, VAMsk_SpNVol },
    {SVV_AsNVolFrac,    SVV_AsMassFrac,  true,  false, VAMsk_SpNVol },
    {SVV_AsRawFrac,     SVV_AsMassFrac,  true,  false, VAMsk_SpRaw  },
    //{SVV_AsRawMassFlow, SVV_AsRawMassFlow,  true,  false, true}
  };                      

void SpImage::BuildDataDefn(DataDefnBlk &DDB)
  {
  if (DDB.BeginStruct(this))
    {
    Strng Tg(Tag()/*DDB.BlockTag()*/);
    Tg+="..";
    if (!bSpeciesOnly)
      {
      AddModelName(DDB);
      AddFidelity(DDB, true);
      pModel->BuildDataDefn_AvailVars(DDB);
      pModel->BuildDataDefn_QualsSlct(DDB, true);
      pModel->BuildDataDefn_RqdProps(DDB);
      //pModel->BuildDataDefn_UsrProps(DDB, MP_GlobalProp);
      pModel->BuildDataDefn_UsrProps(DDB, MP_ConfigProp);

      DDB.Double  ("Temperature",  "T",    DC_T,    "C",      xidSMTemp,     this, 0|isParmConstruct);//(!AsFlow ? isParm : 0));
      DDB.Double  ("Pressure",     "P",    DC_P,    "kPag",   xidSMPress,    this, 0|isParmConstruct);//(!AsFlow ? isParm : 0));

      pModel->BuildDataDefn_Vars(DDB);
      pModel->BuildDataDefn_UsrProps(DDB, MP_UserProp, true);

      DDB.Text   (" ");
      }

    if (pViewBlk)
      BuildDataDefn_Species(DDB, pViewBlk, iViewBlkSize);
    else
      BuildDataDefn_Species(DDB, sbImage, sizeof(sbImage));

    if (!bSpeciesOnly)
      {
      if (pModel->Has_Quals())
        {
        DDB.Text("");
        pModel->BuildDataDefn_Quals(DDB, true);
        DDB.Text("");
        }
      }
    }
  DDB.EndStruct();
  };

// --------------------------------------------------------------------------

flag SpImage::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;
  if (pModel->DataXchg(DCB))
    return 1;
  return SpModelOwner::DataXchg(DCB);
  };

//---------------------------------------------------------------------------

flag SpImage::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=true;
  #if dbgSpCont
  const int doDbg=0||dbgValidate() || dbgValidateBrk() && DoDbgBrk();
  if (doDbg)
    {
    dbgpln("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    dbgpln("SpImage::ValidateData %s", FullObjTag());
    for (int s=0; s<SVSpcCount(); s++)
      dbgpln("%-50s %14s", SVImg(s).SymOrTag(), DbgFltString(VValue[s]));
    }
  #endif

  SpModel & Mdl=*Model();

  bool ApplyScale=true;
  switch (GetView())
    {
    case SVV_AsRawFrac:
    case SVV_AsRawMass:
    case SVV_AsRawMassFlow:
      ApplyScale=false;
      Mdl.SetMassScale(1, "Image 1");
      break;
    }

  if (/*!TaggedObject::GetXWritesBusy() &&*/ Mdl.Mass(som_ALL)</*TraceMass*/1.0e-6)
    {
    switch (GetView())
      {
      case SVV_AsMassFrac:
      case SVV_AsMoleFrac:
      case SVV_AsVolFrac:
      case SVV_AsNVolFrac:
        {
        // Try Find Liquid Water else First specie
        int iSn=Max(0, SDB.Find("H2O(l)"));
        if (iSn>=0)
          {
          VValue[iSn]=1.0;
          Mdl.SetMassScale(1.0, "Image Init");
          }
        }
      break;
      default:
        Mdl.ZeroMass();
        break;
      }
    }

  for (int s=0; s<SVSpcCount(); s++)
    if (!Finite(VValue[s]))
      VValue[s]=0.0;

  if (Valid(m_T_Rqd) && Valid(m_P_Rqd))
    SetTempPress(m_T_Rqd, m_P_Rqd);
  else
    {
    if (Valid(m_T_Rqd))
      SetTemp(m_T_Rqd);
    if (Valid(m_P_Rqd))
      SetPress(m_P_Rqd);
    }

  switch (m_iQRqdSet)
    {
    case SPI_QModeQm: // Qm_Rqd has been set
      m_Qv_Rqd=m_Qm_Rqd/GTZ(Rho());
      m_NQv_Rqd=m_Qm_Rqd/GTZ(NRho());
      break;
    case SPI_QModeQv: // Qv_Rqd has been set
      m_Qm_Rqd=m_Qv_Rqd*Rho();
      m_NQv_Rqd=m_Qm_Rqd/GTZ(NRho());
      break;
    case SPI_QModeNQv: // NQv_Rqd has been set
      m_Qm_Rqd=m_NQv_Rqd*NRho();
      m_Qv_Rqd=m_Qm_Rqd/GTZ(Rho());
      break;
    default:
      {
      bool CalcRqd = Mdl.m_bSpeciesAdjusted && !Mdl.m_bCfgImgSetByFile;
      switch (m_iQRqdMode) // Individual Species have been set
        {
        case SPI_QModeQm: 
          ApplyScale = ApplyScale && !Mdl.m_bSpeciesAdjusted;
          if (CalcRqd)
            m_Qm_Rqd=Mdl.Mass(som_ALL)*Mdl.MassScale();
          m_Qv_Rqd=m_Qm_Rqd/GTZ(Rho());
          m_NQv_Rqd=m_Qm_Rqd/GTZ(NRho());
          break;
        case SPI_QModeQv: 
          ApplyScale = ApplyScale && !Mdl.m_bSpeciesAdjusted;
          if (CalcRqd)
            m_Qv_Rqd=Mdl.Mass(som_ALL)*Mdl.MassScale()/GTZ(Mdl.Rho());
          m_Qm_Rqd=m_Qv_Rqd*Mdl.Rho();
          m_NQv_Rqd=m_Qm_Rqd/GTZ(Mdl.NRho());
          break;
        case SPI_QModeNQv: 
          ApplyScale = ApplyScale && !Mdl.m_bSpeciesAdjusted;
          if (CalcRqd)
            m_NQv_Rqd=Mdl.Mass(som_ALL)*Mdl.MassScale()/GTZ(Mdl.NRho());
          m_Qm_Rqd=m_NQv_Rqd*Mdl.NRho();
          m_Qv_Rqd=m_Qm_Rqd/GTZ(Mdl.Rho());
          break;
        }
      break;
      }
    }

  if (ApplyScale)                
    {
    double MT0=Mdl.Mass(som_ALL);
    double Scl=1.0/GTZ(MT0);
    if (TstGTZ(MT0))
      Mdl.ScaleMass(som_ALL, Scl);
    double MT1=Mdl.Mass(som_ALL);

    Mdl.SetMassScale(TstGTZ(MT1) ? m_Qm_Rqd/MT1:1.0, "Image Qm");
    }

  m_iQRqdSet=SPI_QModeNone;
  Mdl.m_bSpeciesAdjusted=false;
  Mdl.m_bCfgImgSetByFile=false;
  
  #if dbgSpCont
  if (doDbg)
    {
    for (int s=0; s<SVSpcCount(); s++)
      dbgpln("%-50s %14s", SVImg(s).SymOrTag(), DbgFltString(VValue[s]));
    dbgpln("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    }
  #endif

  //!!!!!!!!!!!!!!!!!!!!!
  return OK;
  };

// --------------------------------------------------------------------------

flag SpImage::GetOtherData(FilingControlBlock &FCB)
  {
  return false;
  };

// --------------------------------------------------------------------------

flag SpImage::PutOtherData(FilingControlBlock &FCB)
  {
  return false;
  };

//===========================================================================
//
//
//
//===========================================================================

void SpContainerDumpTest()
  {

  };

// ===========================================================================
//
//                              Specie VectorObj
//
// ===========================================================================

//IMPLEMENT_SPARES(SpVectorObj, 100);
IMPLEMENT_TAGOBJ(SpVectorObj, "SpVect", "SpVect", "", "", "", TOC_SYSTEM,
                 "Specie VectorObj", "Specie VectorObj");

SpVectorObj::SpVectorObj(pTagObjClass pClass_, pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
TaggedObject(pClass_, pTag_, pAttach, eAttach)
  {
  iView=SVV_AsMassFrac;
  iViewBasis=SVVB_All;
  };

// --------------------------------------------------------------------------

SpVectorObj::SpVectorObj(pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) : \
TaggedObject(&SpVectorObjClass, pTag_, pAttach, eAttach)
  {
  iView=SVV_AsMass;
  iViewBasis=SVVB_All;
  };

// --------------------------------------------------------------------------

SpVectorObj::~SpVectorObj()
  {
  };

// --------------------------------------------------------------------------

void SpVectorObj::BuildDataDefn(DataDefnBlk &DDB)
  {
  if (DDB.BeginStruct(this))
    {
    const CSV_ViewInfo & Info=gs_ViewInfo[iView];
    const CSV_ViewCnv  * Cnv=Info.Cnvs[0];
    for (int i=0; i<SVCfgCount(); i++)
      {
      CSysVecItem &I=*SVI.Cfg(i);
      bool AsPerRqdVw=I.CnvIndex()<0;
      int MyiCnv = AsPerRqdVw ? *Cnv->m_pInx : I.CnvIndex();
      LPSTR MysCnv = AsPerRqdVw ? (LPSTR)(LPCSTR)Cnv->m_Txt: I.CnvString();
      switch (I.Type())
        {
        case SVT_Specie:
          {
          int iSp=I.SVIndex();
          DDB.Double(I.FullTag(), I.FullSym(), MyiCnv, MysCnv, Info.xid/*+iSp*/, SpHandle2(0, iSp), this, isParm|NAN_OK|DDEF_ISSPECIE);
          break;
          }
        case SVT_Attribute:
          {
          int iAt=I.SVIndex();
          int Handle;
          if (AsPerRqdVw)
            Handle=xidAttrViews+(Info.xid-xidSpFirst);
          else
            Handle=xidAttrSimple;
          DDB.Double(I.FullTag(), I.FullSym(), MyiCnv, MysCnv, Handle, SpHandle2(0, iAt), this, isParm|NAN_OK|DDEF_ISSPECIEATT);
          break;
          }
        case SVT_Calculation:
          {
          break;
          }
        case SVT_Annotation:
          {
          switch (I.Annotation())
            {
            case SVA_Text:    DDB.Text(I.FullTag());              break;
            case SVA_Marker:  DDB.Text(I.FullTag(), Marker);      break;
            case SVA_OptPage: DDB.Page(I.FullTag(), DDB_OptPage); break;
            case SVA_RqdPage: DDB.Page(I.FullTag(), DDB_RqdPage); break;
            }
          break;
          }
        }
      }
    }
  DDB.EndStruct();
  };

// --------------------------------------------------------------------------

flag SpVectorObj::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidSvView:
      if (DCB.rB)
        SetView(eScdSVView(*DCB.rB));
      DCB.B=iView;
      return 1;
    case xidSvViewBasis:
      if (DCB.rB)
        SetViewBasis(eScdSVViewBasis(*DCB.rB));
      DCB.B=iViewBasis;
      return 1;
    case xidSpMass:
      {
      int s=SpIndex2(DCB.lHandle2);
      if (DCB.rD)
        M.VValue[s]=*DCB.rD;
      DCB.D=M[s];
      return 1;
      }
    case xidSpRawFrac:
    case xidSpRawMass:
    case xidSpRawMassFlow:
      {
      int s=SpIndex2(DCB.lHandle2);
      if (DCB.rD)
        M.VValue[s]=*DCB.rD;
      DCB.D=M[s];
      return 1;
      }
    default:
      //if (DCB.lHandle>=xidSpMass && DCB.lHandle<xidSpMass+MaxSpecies)
      //  {
      //  int s=DCB.lHandle-xidSpMass;
      //  if (DCB.rD)
      //    M[s]=*DCB.rD;
      //  DCB.D=M[s];
      //  return 1;
      //  }
      //if (DCB.lHandle>=xidSpRawFrac && DCB.lHandle<xidSpRawFrac+MaxSpecies)
      //  {
      //  int s=DCB.lHandle-xidSpRawFrac;
      //  if (DCB.rD)
      //    M[s]=*DCB.rD;
      //  DCB.D=M[s];
      //  return 1;
      //  }
      ;
    }

  return 0;
  };

//---------------------------------------------------------------------------

flag SpVectorObj::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=1;

  return OK;
  };

// --------------------------------------------------------------------------

flag SpVectorObj::GetOtherData(FilingControlBlock &FCB)
  {
  return false;
  };

// --------------------------------------------------------------------------

flag SpVectorObj::PutOtherData(FilingControlBlock &FCB)
  {
  return false;
  };

//===========================================================================
//
//
//
//===========================================================================

CEnvironment gs_Environment;

int CEnvironment::Validate()
  {
  if (m_State==eNoGas || Mass(som_Gas)<1.0e-6)
    {
    m_State=eNoGas;//fOK=true;//false;

    int iSnAir=SDB.Find("Air(g)");
    int iSnO=SDB.Find("O2(g)");
    int iSnN=SDB.Find("N2(g)");
    int iSnC=SDB.Find("CO2(g)");

    if (iSnAir>=0)
      {
      m_d[iSnAir]=1.0;
      m_State=eHasInert;
      }
    else if (iSnO>=0 && iSnN>=0 && iSnC>=0)
      {
      m_d[iSnO]=0.2;
      m_d[iSnN]=0.79;
      m_d[iSnC]=0.01;
      m_State=eHasInert;
      }
    else if (iSnO>=0 && iSnN>=0)
      {
      m_d[iSnO]=0.2;
      m_d[iSnN]=0.8;
      m_State=eHasInert;
      }
    else if (iSnN>=0)
      {
      m_d[iSnN]=1;
      m_State=eHasInert;
      }
    else if (iSnO>=0)
      {
      m_d[iSnO]=1;
      m_State=eHasGas;
      }
    else
      {
      for (int s=0; s<SDB.Count(); s++)
        if (SDB[s].PhaseIs(som_Gas))
          {
          m_d[s]=1.0;
          m_State=eHasGas;
          break;
          }
      }
    }
  Normalise();

  m_bDoValidate=false;

  return 1; //don't complain here if no gases
  //return TaggedObject::GlblDynFlowMode() ? 1 : StateOK();
  //return StateOK();
  };

// --------------------------------------------------------------------------

void CEnvironment::BuildDataDefn(pTaggedObject pOwn, char * Tag, DataDefnBlk &DDB)
  {
  if (PrjFileVerNo()>=71)
    {
    if (DDB.BeginObject(pOwn, Tag, "Environment"))
      {
      DDB.Double("Elevation",         "",  DC_L,    "m",        xidBaseElevation,     pOwn, isParm);//(!AsFlow ? isParm : 0));
      DDB.Double("AmbientTemp",       "T", DC_T,    "C",        xidAmbTemp,       pOwn, isParm);//(!AsFlow ? isParm : 0));
      DDB.Double("AirPressure",       "P", DC_P,    "kPa",      xidAtmPress,      pOwn, 0);//(!AsFlow ? isParm : 0));
      DDB.Double("AirDensity",        "",  DC_Rho,  "kg/m^3",   xidAtmDens,       pOwn, 0);//(!AsFlow ? isParm : 0));
      DDB.Double("WindSpeed",         "",  DC_Ldt,  "m/s",      xidWindSpeed,     pOwn, isParm);//(!AsFlow ? isParm : 0));
      DDB.Double("WindDirection",     "",  DC_Ang,  "deg",      xidWindDirn,      pOwn, isParm);//(!AsFlow ? isParm : 0));
      DDB.Double("RelativeHumidity",  "",  DC_Frac, "%",        xidRelHumid,      pOwn, isParm);//(!AsFlow ? isParm : 0));
      if (DDB.BeginStruct(pOwn, "Air"))
        {
        static DDBValueLst DDB4[]= {
          {eNoGas,    "NoGas"},
          {eHasInert, "HasInert"},
          {eHasGas,   "HasGas"},
          {0} };

          DDB.Long  ("State",           "",  DC_,     "",         (long*)&m_State,  pOwn, 0, DDB4);//(!AsFlow ? isParm : 0));
          for (int i=0; i<SDB.Count(); i++)
            if (SDB[i].PhaseIs(som_Gas))
              {
              DDB.Double(SDB[i].Tag(),SDB.Sym(i), DC_Frac, "%", &m_M[i], pOwn, isParm|noSnap|noScenario);
              if(DDB.DoingPutData())
                m_bDoValidate=true;
              }
        }
      DDB.EndStruct();
      DDB.Text  (" ");
      }
    DDB.EndObject();
    }
  else
    {
    if (DDB.BeginObject(pOwn, Tag, "Atmos"))
      {
      DDB.Double("Pressure",        "P", DC_P,    "kPa",      xidAtmPress,      pOwn, 0);//(!AsFlow ? isParm : 0));

      static DDBValueLst DDB4[]= {
        {eNoGas,    "NoGas"},
        {eHasInert, "HasInert"},
        {eHasGas,   "HasGas"},
        {0} };

        DDB.Long  ("State",           "",  DC_,     "",         (long*)&m_State,  pOwn, 0, DDB4);//(!AsFlow ? isParm : 0));
        for (int i=0; i<SDB.Count(); i++)
          if (SDB[i].PhaseIs(som_Gas))
            {
            DDB.Double(SDB[i].Tag(),SDB.Sym(i), DC_Frac, "%", &m_M[i], pOwn, isParm|noSnap|noScenario);
            if(DDB.DoingPutData())
              m_bDoValidate=true;
            }
      }
    DDB.EndObject();
    }
  }

// --------------------------------------------------------------------------

flag CEnvironment::DataXchg(pTaggedObject pOwn, DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidBaseElevation: 
      if (DCB.rD)
        SetBaseElevation(*DCB.rD);
      DCB.D=BaseElevation();
      return 1;
    case xidAtmPress:
      DCB.D=AtmosPress();
      return 1;
    case xidAmbTemp:
      if (DCB.rD)
        SetAmbientTemp(*DCB.rD);//, _AtmT_C1, _AtmT_C2);
      DCB.D=AmbientTemp();
      return 1;
    case xidAtmDens: 
      DCB.D=AtmosDensity(0);
      return 1;
    case xidWindSpeed:
      if (DCB.rD)
        SetWindSpeed(*DCB.rD);
      DCB.D=WindSpeed();
      return 1;
    case xidWindDirn: 
      if (DCB.rD)
        SetWindDirection(*DCB.rD);
      DCB.D=WindDirection();
      return 1;
    case xidRelHumid: 
      if (DCB.rD)
        SetRelHumidity(*DCB.rD);
      DCB.D=RelHumidity();
      return 1;
    }

  return 0;
  }

// --------------------------------------------------------------------------

flag CEnvironment::ValidateData(ValidateDataBlk & VDB)
  {

  return Validate();
  };

//===========================================================================
//
//
//
//===========================================================================

#if dbgDumpSpcInfo
static flag dbgSpDerivsTags=1;
void dbgSpDeriv(pchar t, pchar Tag, SpContainer &Sp)
  {
  if (dbgSpDerivsTags)
    {
    dbgp("TQm %-15s.%-15s [%12s]","","","Total");
    for (int s=0; s < SVValueCount(); s++)
      dbgp(" %12s",SVImg(s).SymOrTag());
    dbgpln("");
    dbgSpDerivsTags=0;
    }

  double SigdQ=0.0;
  for (int s=0; s < SVValueCount(); s++)
    SigdQ += Sp.VValueDot[s];

  dbgp("dQm %-15s.%-15s [%12.4e]",t,Tag, SigdQ);
  for (s=0; s < SVValueCount(); s++)
    dbgp(" %12.4e",Sp.VValueDot[s]);
  dbgpln("");
  }

//--------------------------------------------------------------------------

void dbgSpFlows(pchar t, pchar Tag, SpConduit &Sp, flag Tags)
  {
  //#if dbgFlwNode
  //if (dbgEvalDerivs())
  //  {
  //  static CDbgMngr dbgOn("Derivs", "SpFlows");
  //  if (dbgOn(Tag))
  //    {
  //    if (dbgSpDerivsTags || Tags)
  //      {
  //      dbgp("TQm %-15s.%-15s ","","");
  //      for (int s=0; s < SVSpcCount(); s++)
  //        dbgp(" %12s",SVImg(s).SymOrTag());
  //      dbgpln("");
  //      dbgSpDerivsTags=0;
  //      }

  //    dbgp(" Qm %-15s.%-15s ",t,Tag);
  //    for (int s=0; s < SVSpcCount(); s++)
  //      dbgp(" %12.4e",Sp.VMass[s]);
  //    dbgpln("");
  //    }
  //  }
  //#endif
  }

//--------------------------------------------------------------------------

void dbgSpMasses(pchar t, pchar Tag, SpContainer &Sp, flag Tags)
  {
  if (dbgSpDerivsTags || Tags)
    {
    dbgp("TQm %-15s.%-15s ","","");
    for (int s=0; s < SVSpcCount(); s++)
      dbgp(" %12s",SVImg(s).SymOrTag());
    dbgpln("");
    dbgSpDerivsTags=0;
    }

  dbgp(" Qm %-15s.%-15s ",t,Tag);
  for (int s=0; s < SVSpcCount(); s++)
    dbgp(" %12.4e",Sp.VMass[s]);
  dbgpln("");
  }
//--------------------------------------------------------------------------

void dbgCnState(pchar t, pchar Tag, SpContainer &Cn, flag Tags)
  {
  //if (dbgSpDerivsTags || Tags)
  //  {
  //  dbgp("TQm %-15s.%-15s ","","");
  //  for (int s=0; s < SVSpcCount(); s++)
  //    dbgp(" %12s",SVImg(s).SymOrTag());
  //  dbgpln("");
  //  dbgSpDerivsTags=0;
  //  }

  dbgpln("%-15s ===== T:%7.2f P:%9.2f Ms:%9.2f msHz:%9.2f %s", t,  K_2_C(Cn.Temp()), Cn.Press(), Cn.Mass(), Cn.msHz(), Cn.FullObjTag());
  //dbgpln("TempKept : %i", Cn.pModel->m_bTempKept);
  //dbgpln("PresKept : %i", Cn.pModel->m_bPresKept);
  //dbgpln("RqdTemp  : %14.3f(c)", K2C(Cn.pModel->m_RqdTemp));
  }
#endif

//===========================================================================
//
//
//
//===========================================================================
