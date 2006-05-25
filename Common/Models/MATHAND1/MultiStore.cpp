//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
#include "stdafx.h"
#define  __MULTISTORE_CPP
#include "multistore.h"
#include "dbgmngr.h"
//#include "optoff.h"

#define dbgMultiStore (WITHDEBUG)
#if dbgMultiStore
static CDbgMngr dbgEvalProductsBrk  ("MultiStore",   "EvalProductsBrk");
static CDbgMngr dbgEvalProducts     ("MultiStore",   "EvalProducts");
#endif

//==========================================================================
//
//
//
//==========================================================================

const byte idFeed=0;
const byte idProd=1;

enum SplitMethods { SM_Flow, SM_Frac, SM_EqFrac, SM_PropMassFrac, SM_Store };

const short MaxStores=80;
const short MaxBlendCont=10;
const short MaxCriteria=16;
const short MaxAssaySum=16;

XID xidStoreCount          = ModelXID(1000);
XID xidTtlStoreMass        = ModelXID(1001);
XID xidBlendCount          = ModelXID(1002);
XID xidStoreRBOn           = ModelXID(1003);
XID xidStoreCommonRct      = ModelXID(1004);
XID xidStoreCommonRctBrwse = ModelXID(1005);
XID xidStoreMass           = ModelXID(1100); // leave MaxStores
XID xidStoreTemp           = ModelXID(1100+MaxStores); // leave MaxStores

XID xidIOSplitMethod       = ModelXID(10000);
XID xidIOTargetStore       = xidIOSplitMethod+MaxIOList;
XID xidIOQmEst             = xidIOTargetStore+MaxIOList;
XID xidIOQm                = xidIOQmEst+MaxIOList;
XID xidIOStkQm             = xidIOQm+MaxIOList;
XID xidIOStkQmRqd          = xidIOStkQm+MaxIOList*MaxIOList;
XID xidIOPass              = xidIOStkQmRqd+MaxStores*MaxIOList;


static IOAreaRec MultiStorageIOAreaList[] =
  {{"Feeds",        "Feed"     , idFeed, LIO_In0 ,    nc_MLnk, 0, 20, IOSetXfer|IOGRP(0)},
   {"Products",     "Prod"     , idProd, LIO_Out0,    nc_MLnk, 0, 10, IOSetXfer|IOGRP(0)},
   {NULL}}; //This lists the areas of the model where links can be attached.


static double Drw_MultiStorage[] = { 
                               DD_Poly, -10,-6, -4,2, 2,-6,   DD_Poly, -7,-5, -4,1, -1,-5, DD_Poly, -4,1, -4,-5,
                               DD_Poly, -2,  1,  0,4, 6,-4,   DD_Poly, -2,-1,  0,3,  3,-3, DD_Poly,  0,3,  0,-3,
                               DD_Poly,  2,  3,  4,6, 10,-2,  DD_Poly,  2, 1,  4,5,  7,-1, DD_Poly,  4,5,  4,-1,
                               DD_End }; //This provides the default drawing of the MultiStorage.

IMPLEMENT_MODELUNIT(CMultiStorage, "MultiStorage", "", Drw_MultiStorage, "Tank", "MS", TOC_DYNAMICFLOW|TOC_GRP_TRANSFER|TOC_STD_KENWALT,
                    "Material Transfer:Multi-Storage",
                    "Multiple Unit Storage Model");

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_SPARES(CMultiStore, 100);
IMPLEMENT_TAGOBJ(CMultiStore, "CMultiStore", "SpStore", "", "", "", TOC_SYSTEM,
                 "Specie Storage", "Specie Storage");
CSpView CMultiStore::GlblView(SVV_AsMass);

//--------------------------------------------------------------------------

CMultiStore::CMultiStore(TagObjClass *pClass_, pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) : \
  SpModelOwner(pClass_, pTag_, pAttach, eAttach, false)
  {
  pModel=(SpModel *)SpModelClass.ConstructGroup(SpContainer::DefaultModel(), NULL, SpModelDefTag, NULL, TOA_Free);
  pModel->m_pGlblView=&GlblView;
  SetView(SVV_AsMass);
  SetUsage(SPMU_Mass);

#if WITHMSSRCSNK
  m_SrcQm         = 0.0;
  m_SnkQm         = 0.0;
#endif
  m_TotProdQmRqd  = 0.0;
  m_ProdQmTotal   = 0.0;
  m_FeedQmTotal   = 0.0;
  };

//--------------------------------------------------------------------------

CMultiStore::CMultiStore(pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) : \
SpModelOwner(&SpContainerClass, pTag_, pAttach, pAttach ? (eAttach!=TOA_Unknown ? eAttach : TOA_Embedded) : TOA_Free, true)
  {
  pModel=(SpModel *)SpModelClass.ConstructGroup(SpContainer::DefaultModel(), NULL, SpModelDefTag, NULL, TOA_Free);
  pModel->m_pGlblView=&GlblView;
  SetView(SVV_AsMass);
  SetUsage(SPMU_Mass);

#if WITHMSSRCSNK
  m_SrcQm         = 0.0;
  m_SnkQm         = 0.0;
#endif
  m_TotProdQmRqd  = 0.0;
  m_ProdQmTotal   = 0.0;
  m_FeedQmTotal   = 0.0;
  };

//--------------------------------------------------------------------------

CMultiStore::~CMultiStore()
  {
  pModel->Destroy();
  };

//--------------------------------------------------------------------------

static const BDDSpecies_Blk sbStore[]= {
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
  {SVV_AsElem,         SVV_AsMass, false, false, VAMsk_SpElem     },
  {SVV_AsElemFrac,     SVV_AsMass, false, false, VAMsk_SpElem     },
  {SVV_AsElemConc,     SVV_AsMass, false, false, VAMsk_SpElem     },
  {SVV_AsElemConc25,   SVV_AsMass, false, false, VAMsk_SpElem     },
  {SVV_AsComp,         SVV_AsMass, false, false, VAMsk_SpComp     },
  {SVV_AsCompFrac,     SVV_AsMass, false, false, VAMsk_SpComp     },
  {SVV_AsCompConc,     SVV_AsMass, false, false, VAMsk_SpComp     },
  {SVV_AsCompConc25,   SVV_AsMass, false, false, VAMsk_SpComp     }
  };

void CMultiStore::BuildDataDefn(DataDefnBlk &DDB)
  {
  if (DDB.BeginStruct(this))
    {
    AddModelName(DDB);
    AddFidelity(DDB, false);
    pModel->BuildDataDefn_QualsSlct(DDB, true);
    //pModel->BuildDataDefn_UsrProps(DDB, MP_GlobalProp, false);
    pModel->BuildDataDefn_UsrProps(DDB, MP_ConfigProp, false);

    DDB.Text    (" ");
    pModel->BuildDataDefn_Vars(DDB);
    pModel->BuildDataDefn_UsrProps(DDB, MP_UserProp, true);
    DDB.Text   (" ");

    BuildDataDefn_Species(DDB, sbStore, sizeof(sbStore));

    Strng Tg(Tag()/*DDB.BlockTag()*/);
    Tg+="..";
    DDB.Page(Tg(), DDB_OptPage);
    pModel->BuildDataDefn_RqdProps(DDB);
    //pModel->BuildDataDefn_UsrProps(DDB, MP_GlobalProp);
    pModel->BuildDataDefn_UsrProps(DDB, MP_ConfigProp);
    AddFnRanges(DDB);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CMultiStore::DataXchg(DataChangeBlk & DCB)
  {
  //if (TaggedObject::DataXchg(DCB))
  //  return 1;
  switch (DCB.lHandle)
    {
    case xidCnIntTests:
      if (DCB.rB)
        pModel->SetIntegrationTests((SPMTests)*DCB.rB);
      DCB.B=(byte)pModel->IntegrationTests();
      return 1;
    case xidStoreMass:
      DCB.D=Mass();
      return 1;
    case xidStoreTemp:
      DCB.D=Temp();
      return 1;
    case xidCnPress:
      DCB.D = Press();
      return 1;
    case xidCnTemp:
      DCB.D = Temp();
      return 1;
    default:
      if (pModel->DataXchg(DCB)) 
        return 1;
    }

  return SpModelOwner::DataXchg(DCB);
  };

//---------------------------------------------------------------------------

flag CMultiStore::ValidateData(ValidateDataBlk & VDB)
  {
  return pModel->ValidateData(VDB) && SpModelOwner::ValidateData(VDB);
  };

//--------------------------------------------------------------------------

void CMultiStore::Validate()
  {
#if WITHMSSRCSNK
  m_SrcQm=GEZ(m_SrcQm);
  m_SnkQm=GEZ(m_SnkQm);
#endif
  };

//--------------------------------------------------------------------------

void CMultiStore::SetStateAction(IE_Enables E)
  {
  pModel->SetStateAction(E);
  };

//--------------------------------------------------------------------------

void CMultiStore::SetState(eScdMdlStateActs RqdState)
  {
  switch (RqdState)
    {
    case MSA_Empty:
      ZeroMass();
      ZeroDeriv();
      break;
    case MSA_PreSet:
      break;
    case MSA_ZeroFlows:
      break;
    case MSA_EmptySpillTargets:
      break;
    case MSA_SteadyState:
      break;
    case MSA_DynStatsRunInit:
      break;
    }
  };

//--------------------------------------------------------------------------

//static byte STP_LenPos=1;
static byte STP_Model=1;

flag CMultiStore::GetOtherData(FilingControlBlock &FCB)
  {
  DWORD nBytes;
  DWORD CurPos=FCB.SetFilePointer(0, FILE_CURRENT);

  FCB.WriteFile(&STP_Model, sizeof(STP_Model), &nBytes);
  //FCB.WriteFile(&m_dLength, sizeof(m_dLength), &nBytes);
  //FCB.WriteFile(&m_dPosition, sizeof(m_dPosition), &nBytes);
  pModel->GetOtherData(FCB);

  return True;
  }

// --------------------------------------------------------------------------

flag CMultiStore::PutOtherData(FilingControlBlock &FCB)
  {
  DWORD nBytes;
  byte What;

  FCB.ReadFile(&What, sizeof(What), &nBytes);
  ASSERT(What==STP_Model);
  //FCB.ReadFile(&m_dLength, sizeof(m_dLength), &nBytes);
  //FCB.ReadFile(&m_dPosition, sizeof(m_dPosition), &nBytes);
  pModel->PutOtherData(FCB);

  return True;
  };

//--------------------------------------------------------------------------

double CMultiStore::GetProduct(int ProdNo, PhMask Pass, double QmRqd, SpConduit & Cd)
  {
  //m_ProdQm[ProdNo]=QmRqd;
  double Scale=Min(1.0, pModel->Mass(Pass)/GTZ(ICGetTimeInc()*m_TotProdQmRqd));

#if WITHMSSRCSNK
  if (ProdNo==0)
    m_ProdQmTotal=Scale*m_SnkQm;
#else
  if (ProdNo==0)
    m_ProdQmTotal=0.0;
#endif
  double QmActual = Scale*QmRqd;
  m_ProdQmTotal+=QmActual;
  Cd.QAddM(*pModel, Pass, QmActual);

  const double MOut=ICGetTimeInc()*QmActual;
  const double MTot=Mass();
  pModel->ScaleMass(Pass, GEZ(MTot-MOut)/GTZ(MTot));

  return QmActual;
  };

//--------------------------------------------------------------------------

void CMultiStore::SetFeed(int FeedNo, double QmRqd, SpConduit & Cd)
  {
  if (FeedNo==0)
    {
    m_Feed.QZero();
#if WITHMSSRCSNK
    m_FeedQmTotal=m_SrcQm;
#else
    m_FeedQmTotal=0.0;
#endif
    }
  m_FeedQmTotal+=QmRqd;
  m_Feed.QAddM(Cd, som_ALL, QmRqd);
  };

//--------------------------------------------------------------------------

flag CMultiStore::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="N\t????";                           return 1;
    default:
      return SpModelOwner::CIStrng(No, pS);
    }
  };

//==========================================================================
//
//
//
//==========================================================================

CMultiStoreIO::CMultiStoreIO()
  {
  m_iSplitMethod  = SM_PropMassFrac;
  m_iSplitStoreIndex = 0;
  };

//--------------------------------------------------------------------------

CMultiStoreIO::~CMultiStoreIO()
  {
  };

//--------------------------------------------------------------------------

CMultiStoreIO::CMultiStoreIO(const CMultiStoreIO & I)
  {
  m_iSplitMethod=I.m_iSplitMethod;
  m_iSplitStoreIndex=I.m_iSplitStoreIndex;
  const int n=I.m_Split.GetSize();
  m_Split.SetSize(n);
  m_QmRqd.SetSize(n);
  m_Qm.SetSize(n);
  m_QmPass.SetSize(n);
  for (int i=0; i<n; i++)
    {
    m_Split[i]=I.m_Split[i];
    m_QmRqd[i]=I.m_QmRqd[i];
    m_Qm[i]=I.m_Qm[i];
    m_QmPass[i]=I.m_QmPass[i];
    }
  };

//--------------------------------------------------------------------------

CMultiStoreIO & CMultiStoreIO::operator = (const CMultiStoreIO & I)
  {
  m_iSplitMethod=I.m_iSplitMethod;
  m_iSplitStoreIndex=I.m_iSplitStoreIndex;
  const int n=I.m_Split.GetSize();
  m_Split.SetSize(n);
  m_QmRqd.SetSize(n);
  m_Qm.SetSize(n);
  m_QmPass.SetSize(n);
  for (int i=0; i<n; i++)
    {
    m_Split[i]=I.m_Split[i];
    m_QmRqd[i]=I.m_QmRqd[i];
    m_Qm[i]=I.m_Qm[i];
    m_QmPass[i]=I.m_QmPass[i];
    }
  return *this;
  };

//--------------------------------------------------------------------------

void CMultiStoreIO::Validate(bool FeedStream, bool StoreCentric)
  {
  if (FeedStream)
    {
    if ((m_iSplitMethod==SM_Flow || m_iSplitMethod==SM_PropMassFrac))
      m_iSplitMethod = SM_EqFrac;
    }
  else // ProdStream
    {
    if (StoreCentric) 
      m_iSplitMethod=SM_Flow;
    }
  const int n=m_Split.GetSize();
  if (n)
    {
    if (m_iSplitMethod==SM_Store)
      {
      m_iSplitStoreIndex=(short)Range(0, (int)m_iSplitStoreIndex, n-1);
      for (int j=0; j<n; j++)
        m_Split[j] = 0.0;
      m_Split[m_iSplitStoreIndex] = 1.0;
      }
    else if (m_iSplitMethod==SM_EqFrac)
      {
      for (int j=0; j<n; j++)
        m_Split[j] = 1.0/n;
      }
    else
      {
      //ValidateSplit();
      }
    }
  }

//--------------------------------------------------------------------------

void CMultiStoreIO::ValidateSplit()
  {
  const int n=m_Split.GetSize();
  if (n)
    {
    if (m_iSplitMethod!=SM_Store && m_iSplitMethod!=SM_EqFrac)
      {
      double Tot=0.0;
      for (int j=0; j<n-1; j++)
        {
        double &S=m_Split[j];
          S=Valid(S) ? Range(0.0, S, 1.0) : 0.0;
          S=Min(S, 1.0-Tot);
        Tot+=S;
        }
      m_Split[n-1] = 1.0-Tot;
      }
    }
  }

//==========================================================================
//
//
//
//==========================================================================

CMultiStorage::CMultiStorage(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  m_Src(eDIO_Src, this, false, true, "Src"),
  m_Snk(eDIO_Snk, this, false, true, "Snk"),
  m_AccIn(eDIO_Accum, this, false, true, "Accumulation"),
  m_AccOut(eDIO_Deplete, this, false, true, "Depletion"),
  m_AccCalc(this, m_AccIn, m_AccOut)
  {
  AttachClassInfo(nc_Process, NULL);
  AttachIOAreas(MultiStorageIOAreaList);

  m_bTrackFlwReq  = true;
  m_Store2View    = 0;
  m_bRBsOn        = false;
  m_bReloadReqd   = false;
  m_bStoreCentric = false;
  SetPileCount(1);
  }

// -------------------------------------------------------------------------

CMultiStorage::~CMultiStorage()
  {
  }

// -------------------------------------------------------------------------

void CMultiStorage::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  static DDBValueLst DDBIOFeedMeth[]=
    {
      {SM_Frac,           "Split"},
      {SM_EqFrac,         "Equal Split"},
      {SM_Store,          "Spec Store"},
      {0}
    };
  static DDBValueLst DDBIOProdMeth[]=
    {
      {SM_Flow,           "Flow"},
      {SM_Frac,           "Split"},
      {SM_EqFrac,         "Equal Split"},
      {SM_PropMassFrac,   "Prop Mass Split"},
      {SM_Store,          "Spec Store"},
      {0}
    };
  static DDBValueLst DDBStoreProdMeth[]=
    {
      {SM_Flow,           "Flow"},
      //{SM_Frac,           "Split"},
      //{SM_EqFrac,         "Equal Split"},
      //{SM_PropMassFrac,   "Prop Mass Split"},
      //{SM_Store,          "Spec Store"},
      {0}
    };
  DDBValueLst DDBPass[]=
    { // not static som_ETC are not initialised until later
      {som_Sol,           "Solids"},
      {som_Liq,           "Liquids"},
      {som_Gas,           "Gasses"},
      {som_Sol|som_Liq,   "Sol&Liq"},
      {som_Liq|som_Gas,   "Liq&Gas"},
      {som_Sol|som_Gas,   "Sol&Gas"},
      {som_ALL,           "All"},
      {0}
    };
  BuildDataDefnElevation(DDB);
  DDB.Text("");
  DDB.Short   ("NoStores",    "",     DC_,  "",     xidStoreCount,          this, isParmStopped|SetOnChange);
  DDB.CheckBox("Reactions",   "",     DC_,  "",     xidStoreRBOn,           this, isParm|SetOnChange);
  DDEF_Flags Vis=DDB.Visibility(SHM_All, m_bRBsOn);
  DDB.String  ("Rct.CommonRctFile", "", DC_, "",    xidStoreCommonRct,       this, isParm|SetOnChange);
  DDB.Button  ("Rct.CommonBrowse",  "", DC_, "",    xidStoreCommonRctBrwse, this, isParm);
  DDB.SetVisibility(Vis);

  DDB.CheckBox("TrackRqdFlowOut", "", DC_,  "",     &m_bTrackFlwReq,        this, isParm|SetOnChange);
  DDB.Text    ("");
  DDB.Short   ("Store2View",  "",     DC_,  "",     &m_Store2View,          this, isParm|SetOnChange);
  DDB.Text    ("");
  DDB.Double  ("TotalMass",   "",     DC_M, "kg",   xidTtlStoreMass,        this, isResult);
  DDB.Text    ("");

  //------ Storage Masses ------
  if (DDB.BeginArray(this, "Summary", "SPG_Summary", m_Store.GetCount()))
    {
    for (int p=0; p<m_Store.GetCount(); p++)
      {
      if (DDB.BeginElement(this, p))
        {
        DDB.Double  ("",          "Mass",  DC_M,  "kg",     xidStoreMass+p,            this, isResult|noFileAtAll);
        DDB.Double  ("",          "Temp",  DC_T,  "C",      xidStoreTemp+p,            this, isResult|noFileAtAll);
        }
      }
    }
  DDB.EndArray();

  BuildDataDefnShowIOs(DDB);
 
  int FeedCount = 0;
  int ProdCount = 0;
  for (int i=0; i<NoFlwIOs(); i++)
    {
    if (IOId_Self(i)==idFeed)
      FeedCount++;
    if (IOId_Self(i)==idProd)
      ProdCount++;
    }

  for (int Pass=0; Pass<2; Pass++)
    {
    LPSTR ATag;
    if (Pass==0 /*&& FeedCount>0*/)
      {
      DDB.Page("Feeds", DDB_RqdPage);
      //DDB.Long("FeedCount",   "",     DC_,  "",     &m_iFeedCount,         this, isResult|InitHidden);//todo
      ATag="Fd";
      }
    else //if (Pass==1 /*&& ProdCount>0*/)
      {
      DDB.Page("Products", DDB_RqdPage);
      //DDB.Long("ProdCount",   "",     DC_,  "",     &m_iProdCount,         this, isResult|InitHidden);//todo
      ATag="Pr";
      DDB.CheckBoxBtn("StoreCentric", "", DC_, "", &m_bStoreCentric, this, isParmStopped);
      if (m_bStoreCentric && !DDB.ForFileSnpScn())
        break; // Do Store Products below
      }
    DDB.Text("");
    if (DDB.BeginArray(this, ATag, "SPG_IO", m_Store.GetCount()))
      {
      char Buff[256];
      for (int i=0; i<NoFlwIOs(); i++)
        {
        const int id = IOId_Self(i);
        if (id==idFeed && Pass==0 || id==idProd && Pass==1)   
          {
          const int SplM = m_StoreIO[i].m_iSplitMethod;
          Strng LTg=Nd_Rmt(i)->Tag();
          sprintf(Buff, "-------- %s --------", LTg());
          DDB.Text(Buff);
          if (DDB.BeginElement(this, LTg()))
            {
            DDB.Byte    ("SplitMethod", "",            DC_,     "",      xidIOSplitMethod+i, this, isParmStopped|SetOnChange, (Pass==0 ? DDBIOFeedMeth : DDBIOProdMeth));
            DDB.Visibility(SHM_All, SplM==SM_Store);
            DDB.Short   ("TargetStore", "",            DC_,     "",      xidIOTargetStore+i, this, isParm);
            DDB.Visibility();
            DDB.Double  ("MassFlowEst", "QmEst",       DC_Qm,  "kg/s",   xidIOQmEst+i,  this, isResult);
            DDB.Double  ("MassFlow",    "Qm",          DC_Qm,  "kg/s",   xidIOQm+i,     this, isResult);
            DDB.Text("");
            if (DDB.BeginArray(this, "Store", "SPG_SIO", m_Store.GetCount()))
              {
              for (int p=0; p<m_Store.GetCount(); p++)
                {
                if (DDB.BeginElement(this, p))
                  {
                  if (m_Store[p].m_Name.Len()>0)
                    {
                    sprintf(Buff, "%s (Store%d)...", m_Store[p].m_Name(), p);
                    DDB.Text(Buff);
                    }
                  if (Pass==0)
                    { // Inputs
                    DDB.Double  ("",    "Split",  DC_Frac,  "%",    &m_StoreIO[i].m_Split[p],     this, (SplM==SM_Frac && p<m_Store.GetCount()-1) ? isParm : isResult);
                    DDB.Double  ("",    "Qm",     DC_Qm,    "kg/s", xidIOStkQm+i*MaxStores+p,     this, isResult);
                    }
                  else 
                    { // Outputs
                    if (SplM==SM_Frac || SplM==SM_EqFrac || SplM==SM_PropMassFrac || DDB.ForFileSnpScn())
                      DDB.Double("",    "Split",  DC_Frac,  "%",    &m_StoreIO[i].m_Split[p],     this, (SplM==SM_Frac && p<m_Store.GetCount()-1) ? isParm : isResult);
                    if (SplM==SM_Flow || DDB.ForFileSnpScn())
                      DDB.Double("",    "QmRqd",  DC_Qm,    "kg/s", xidIOStkQmRqd+i*MaxStores+p,  this, (SplM==SM_Flow ? isParm : isResult)|NAN_OK);
                    DDB.Double  ("",    "Qm",     DC_Qm,    "kg/s", xidIOStkQm+i*MaxStores+p,     this, isResult);
                    DDB.Long    ("",    "Pass",   DC_,      "",     xidIOPass+i*MaxStores+p,      this, isParm, DDBPass);
                    }
                  }
                }
              }
            DDB.EndArray();
            }
          DDB.Text("");
          }
        }
      }
    DDB.EndArray();
    }

  //------ Storage contents ------
  m_Store2View=(short)Range(-1, (int)m_Store2View, m_Store.GetCount()-1);

  if (DDB.BeginArray(this, "Store", "SPG_Store", m_Store.GetCount()))
    {
    for (int p=0; p<m_Store.GetCount(); p++)
      {
      //if (m_bStoreCentric && 
      //    !DDB.ForFileSnpScn() && 
      //    (DDB.ForView() && p==m_Store2View || !DDB.ForView()) 
      //  {

      //  int xxx=0;
      //  }
          
      if (DDB.ForView() && p!=m_Store2View) 
        continue;

      if (m_bStoreCentric && DDB.ForView())
        {
        DDB.Short("Store2View",  "",     DC_,    "",   &m_Store2View,    this, isParm|SetOnChange|noFileAtAll);
        DDB.Text(" ");
        }

      //sprintf(Tg, "%d", p);
      if (DDB.BeginElement(this, p))
      //if (DDB.BeginElement(this, Tg))
        {
        if (m_bStoreCentric && !DDB.ForFileSnpScn()) 
          {
          //if (DDB.ForView())
          //  {
          //  DDB.Short("Store2View",  "",     DC_,    "",   &m_Store2View,    this, isParm|SetOnChange|noFileAtAll);
          //  DDB.Text(" ");
          //  }
          char Buff[256];
          sprintf(Buff, "-------- Store%i --------", p);
          DDB.Text(Buff);
          LPTSTR ATag="Pr";
          if (DDB.BeginArray(this, ATag, "SPG_IO", m_Store.GetCount()))
            {
            for (int i=0; i<NoFlwIOs(); i++)
              {
              const int id = IOId_Self(i);
              if (id==idProd)   
                {
                const int SplM = m_StoreIO[i].m_iSplitMethod;
                Strng LTg=Nd_Rmt(i)->Tag();
                if (DDB.BeginElement(this, LTg()))
                  {
                  DDB.Byte    ("Method", "",            DC_,     "",      xidIOSplitMethod+i, this, isParmStopped|SetOnChange, DDBStoreProdMeth);
                  //DDB.Visibility(SHM_All, SplM==SM_Store);
                  //DDB.Short   ("TargetStore", "",            DC_,     "",      xidIOTargetStore+i, this, isParm);
                  //DDB.Visibility();
                  //DDB.Double  ("MassFlowEst", "QmEst",       DC_Qm,  "kg/s",   xidIOQmEst+i,  this, isResult);
                  //DDB.Double  ("MassFlow",    "Qm",          DC_Qm,  "kg/s",   xidIOQm+i,     this, isResult);
                  //DDB.Text("");
                  //if (DDB.BeginArray(this, "Store", "SPG_SIO", m_Store.GetCount()))
                  //  {
                    //for (int p=0; p<m_Store.GetCount(); p++)
                      {
                      //if (DDB.BeginElement(this, p))
                        { // Outputs
                        //if (SplM==SM_Frac || SplM==SM_EqFrac || SplM==SM_PropMassFrac || DDB.ForFileSnpScn())
                        //  DDB.Double("",    "Split",  DC_Frac,  "%",    &m_StoreIO[i].m_Split[p],     this, (SplM==SM_Frac && p<m_Store.GetCount()-1) ? isParm : isResult);
                        //if (SplM==SM_Flow || DDB.ForFileSnpScn())
                        DDB.Double("",    "QmRqd",  DC_Qm,    "kg/s", xidIOStkQmRqd+i*MaxStores+p,  this, /*(SplM==SM_Flow ? */isParm /*: isResult)*/|NAN_OK);
                        DDB.Double  ("",    "Qm",     DC_Qm,    "kg/s", xidIOStkQm+i*MaxStores+p,     this, isResult);
                        DDB.Long    ("",    "Pass",   DC_,      "",     xidIOPass+i*MaxStores+p,      this, isParm, DDBPass);
                        }
                      }
                  //  }
                  //DDB.EndArray();
                  }
                DDB.Text("");
                }
              }
            }
          DDB.EndArray();
          }

        char Tg[8];
        sprintf(Tg, "S%d", p);
        DDB.Page(Tg, DDB_RqdPage);

        if (DDB.ForView())
          {
          DDB.Short("Store2View",  "",     DC_,    "",   &m_Store2View,    this, isParm|SetOnChange|noFileAtAll);
          DDB.Text(" ");
          }
        
        DDB.String  ("Name",      "",      DC_,   "",       &m_Store[p].m_Name,        this, isParm);
        DDB.Double  ("",          "Mass",  DC_M,  "kg",     xidStoreMass+p,            this, isResult);
        DDB.Double  ("",          "Temp",  DC_T,  "C",      xidStoreTemp+p,            this, isResult);
        DDB.Double  ("FeedRate",  "",      DC_Qm, "kg/s",   &m_Store[p].m_FeedQmTotal, this, isResult);
        DDB.Double  ("ProdRate",  "",      DC_Qm, "kg/s",   &m_Store[p].m_ProdQmTotal, this, isResult);
#if WITHMSSRCSNK
        DDB.Double  ("Src",       "",      DC_Qm, "kg/s",   &m_Store[p].m_SrcQm,       this, isParm);
        DDB.Double  ("Snk",       "",      DC_Qm, "kg/s",   &m_Store[p].m_SnkQm,       this, isParm);
#endif
        DDB.Text(" ");
        DDB.Object(&m_Store[p], this, 0, 0, DDB_NoPage);
        //DDB.Object(m_Store[p].m_pContents, this, 0, 0, DDB_NoPage);
        if (p<m_StoreRB.GetCount())
          {
          char Tg[8];
          sprintf(Tg, "RB%d", p);
          DDB.Page(Tg, DDB_RqdPage);
          m_StoreRB[p].BuildDataDefn(DDB, NULL, NULL, DDB_NoPage, 1000+p);
          }
        }
      }
    }
  DDB.EndArray();

  //if (DDB.BeginArray(this, "RB", "SPG_StoreRB", m_Store.GetCount()))
  //  {
  //  for (int p=0; p<m_StoreRB.GetCount(); p++)
  //    {
  //    if (DDB.ForView() && p!=m_Store2View) 
  //      continue;

  //    //char Tg[8];
  //    //sprintf(Tg, "RB%d", p);
  //    //DDB.Page(Tg, DDB_RqdPage);
  //    //sprintf(Tg, "%d", p);
  //    if (DDB.BeginElement(this, p))
  //    //if (DDB.BeginElement(this, Tg))
  //      {
  //      //if (DDB.ForView())
  //      //  {
  //      //  DDB.Short("Store2View",  "",     DC_,    "",      &m_Store2View,    this, isParm|SetOnChange|noFileAtAll);
  //      //  DDB.Text(" ");
  //      //  }
  //      
  //      m_StoreRB[p].BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 1000+p);
  //      }
  //    }
  //  }
  //DDB.EndArray();

  if (SolveDynamicMethod())
    {
    if (m_Src.Enabled || m_Snk.Enabled || m_AccIn.Enabled || m_AccOut.Enabled)
      {
      DDB.Page("AreaIO", DDB_RqdPage);
      m_Src.BuildDataDefn(DDB, NULL, DDB_NoPage, 1);
      m_Snk.BuildDataDefn(DDB, NULL, DDB_NoPage, 2);
      m_AccIn.BuildDataDefn(DDB, NULL, DDB_NoPage, 3);
      m_AccOut.BuildDataDefn(DDB, NULL, DDB_NoPage, 4);
      }
    }

  DDB.EndStruct();
  };

// --------------------------------------------------------------------------

flag CMultiStorage::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==1 && m_AccIn.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==2 && m_AccOut.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==3 && m_AccIn.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==4 && m_AccOut.DataXchg(DCB))
    return 1;
    //flag           DataXchg(DataChangeBlk & DCB);
  if (DCB.dwUserInfo>=1000 && 
      DCB.dwUserInfo<dword(1000+m_StoreRB.GetSize()) && 
      m_StoreRB[DCB.dwUserInfo-1000].DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidStoreCount:
      if (DCB.rS)
        SetPileCount(Min(MaxStores, *DCB.rS));
      DCB.S=(short)m_Store.GetCount();
      return 1;
    case xidStoreRBOn:
      if (DCB.rB)
        SetRBsOn(*DCB.rB);
      DCB.B=m_bRBsOn;
      return 1;
    case xidStoreCommonRct:
      if (DCB.rpC)
        {
        m_sCommonRct=DCB.rpC;
        m_bReloadReqd=true;
        }
      DCB.pC=m_sCommonRct();
      return 1;
    case xidStoreCommonRctBrwse:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng Ext("*.rct");
        CSCDFileDialog Dlg(true, NULL, Ext(), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, "RCT Files (*.RCT)|*.RCT||");
        Dlg.m_ofn.lpstrInitialDir = PrjFiles();
        Dlg.m_ofn.lpstrTitle = "Reaction File";
        HWND H=::GetActiveWindow();
        if (Dlg.DoModal()==IDOK)
          m_sCommonRct=Dlg.GetPathName();
        ::SetActiveWindow(H);
        DCB.B=0;
        m_sCommonRct.FnContract();
        m_bReloadReqd=true;
        }
      return 1;

    case xidTtlStoreMass:
      {
      double Ttl=0.0;
      for (int i=0; i<m_Store.GetCount(); i++)
        Ttl += m_Store[i].Mass();
      DCB.D=Ttl;
      return 1;
      }
    default:
      if (DCB.lHandle>=xidStoreMass && DCB.lHandle<xidStoreMass+MaxStores)
        {
        int i=DCB.lHandle-xidStoreMass;
        DCB.D=m_Store[i].Mass();
        return 1;
        }
      if (DCB.lHandle>=xidStoreTemp && DCB.lHandle<xidStoreTemp+MaxStores)
        {
        int i=DCB.lHandle-xidStoreTemp;
        DCB.D=m_Store[i].Temp();
        return 1;
        }
      if (DCB.lHandle>=xidIOSplitMethod && DCB.lHandle<xidIOSplitMethod+MaxIOList)
        {
        int i=DCB.lHandle-xidIOSplitMethod;
        if (DCB.rB)
          m_StoreIO[i].m_iSplitMethod = *DCB.rB;
        DCB.B=m_StoreIO[i].m_iSplitMethod;
        return 1;
        }
      if (DCB.lHandle>=xidIOTargetStore && DCB.lHandle<xidIOTargetStore+MaxIOList)
        {
        int i=DCB.lHandle-xidIOTargetStore;
        if (DCB.rS)
          m_StoreIO[i].m_iSplitStoreIndex = Range((short)0, *DCB.rS, (short)(m_Store.GetCount()-1));
        DCB.S=m_StoreIO[i].m_iSplitStoreIndex;
        return 1;
        }
      if (DCB.lHandle>=xidIOQmEst && DCB.lHandle<xidIOQmEst+MaxIOList)
        {
        int i=DCB.lHandle-xidIOQmEst;
        DCB.D=(IOId_Self(i)==idFeed?1:-1)*IOQmEst_In(i);
        return 1;
        }
      if (DCB.lHandle>=xidIOQm && DCB.lHandle<xidIOQm+MaxIOList)
        {
        int i=DCB.lHandle-xidIOQm;
        DCB.D=(IOId_Self(i)==idFeed?1:-1)*IOQm_In(i);
        return 1;
        }
      if (DCB.lHandle>=xidIOStkQmRqd  && DCB.lHandle<xidIOStkQmRqd+MaxStores*MaxIOList)
        {
        int j=DCB.lHandle-xidIOStkQmRqd;
        int i=j/MaxStores;
        int p=j-i*MaxStores;
        if (IOId_Self(i)==idFeed)
          {
          DCB.D=IOQmEst_In(i)*m_StoreIO[i].m_Split[p];   
          }
        else
          {
          if (m_StoreIO[i].m_iSplitMethod==SM_Flow)
            {
            if (DCB.rD)
              m_StoreIO[i].m_QmRqd[p]=*DCB.rD;
            DCB.D=m_StoreIO[i].m_QmRqd[p];   
            }
          else
            DCB.D=IOOutSplitOK(i) ? IOQmEst_Out(i)*m_StoreIO[i].m_Split[p] : dNAN;   
          }
        return 1;
        }
      if (DCB.lHandle>=xidIOStkQm && DCB.lHandle<xidIOStkQm+MaxStores*MaxIOList)
        {
        int j=DCB.lHandle-xidIOStkQm;
        int i=j/MaxStores;
        int p=j-i*MaxStores;
        DCB.D=m_StoreIO[i].m_Qm[p];
        return 1;
        }
      if (DCB.lHandle>=xidIOPass  && DCB.lHandle<xidIOPass+MaxStores*MaxIOList)
        {
        int j=DCB.lHandle-xidIOPass;
        int i=j/MaxStores;
        int p=j-i*MaxStores;
        if (DCB.rL)
          m_StoreIO[i].m_QmPass[p]=*DCB.rL;
        DCB.L=m_StoreIO[i].m_QmPass[p];   
        return 1;
        }
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag CMultiStorage::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);

  for (int p=0; p<m_Store.GetSize(); p++)
    m_Store[p].Validate();

  if (m_bRBsOn)
    {
    for (int p=0; p<m_Store.GetSize(); p++)
      {
      if (m_bReloadReqd && m_sCommonRct())
        m_StoreRB[p].Load(m_sCommonRct());
      OK = OK && m_StoreRB[p].ValidateData(VDB);
      }
    m_bReloadReqd=false; 
    }

  for (int i=0; i<NoFlwIOs(); i++)
    {
    m_StoreIO[i].Validate(IOId_Self(i)==idFeed, m_bStoreCentric!=0);
    }

  return OK;
  }

//--------------------------------------------------------------------------

void CMultiStorage::SetPileCount(short N)
  {
  N=Min(N, MaxStores);
  
  int OldN=m_Store.GetSize();
  
  for (int p=N; p<OldN; p++)
    delete m_Store.GetAt(p);

  m_Store.SetSize(N);
  
  for (int p=OldN; p<N; p++)
    m_Store.SetAt(p, new CMultiStore("Content", this, TOA_Embedded));
  
  if (m_bRBsOn)
    {
    for (int p=N; p<OldN; p++)
      {
      m_StoreRB[p].Close();
      delete m_StoreRB.GetAt(p);
      }
    m_StoreRB.SetSize(N);
    for (int p=OldN; p<N; p++)
      {
      Strng Tg;
      Tg.Set("RB%i", p);
      m_StoreRB.SetAt(p, new CMultiStoreRB(this, true, Tg()));
      m_StoreRB[p].Open();
      }
    m_bReloadReqd=true;
    }

  for (int p=0; p<m_Store.GetSize(); p++)
    {
    m_Store[p].Tag("Content");
    Strng S;
    S.Set("Store.[%i]",p);
    m_Store[p].Prefix(S());
    }
  for (int i=0; i<NoFlwIOs(); i++)
    {
    m_StoreIO[i].m_Split.SetSize(N);
    m_StoreIO[i].m_QmRqd.SetSize(N);
    m_StoreIO[i].m_Qm.SetSize(N);
    m_StoreIO[i].m_QmPass.SetSize(N);
    for (int p=0; p<N; p++)
      m_StoreIO[i].m_QmPass[p]=som_ALL;
    }
  }

//--------------------------------------------------------------------------

void CMultiStorage::SetRBsOn(flag On)
  {
  if (m_bRBsOn && !On)
    {
    int N=m_StoreRB.GetSize();
    for (int p=0; p<N; p++)
      {
      m_StoreRB[p].Close();
      delete m_StoreRB.GetAt(p);
      }
    m_StoreRB.SetSize(0);
    }

  if (!m_bRBsOn && On)
    {
    int N=m_Store.GetSize();
    m_StoreRB.SetSize(N);
    for (int p=0; p<N; p++)
      {
      Strng Tg;
      Tg.Set("RB%i", p);
      m_StoreRB.SetAt(p, new CMultiStoreRB(this, true, Tg()));
      m_StoreRB[p].Open();
      }
    m_bReloadReqd=true;
    }
  m_bRBsOn = On;
  }

//--------------------------------------------------------------------------

void CMultiStorage::OnAppActivate(BOOL bActive)
  {
  if (m_bRBsOn)
    {
    int N=m_Store.GetSize();
    for (int p=0; p<N; p++)
      m_StoreRB[p].OnAppActivate(bActive); 
    }
  };

//--------------------------------------------------------------------------

int CMultiStorage::FilesUsed(CFilesUsedArray & Files)
  { 
  int Cnt=0;
  if (m_bRBsOn)
    {
    int N=m_Store.GetSize();
    m_StoreRB.SetSize(N);
    for (int p=0; p<N; p++)
      Cnt+=m_StoreRB[p].FilesUsed(Files);
    }
  return Cnt;
  };

//--------------------------------------------------------------------------

void CMultiStorage::AddIO(int n)
  {
  CMultiStoreIO IO;
  const int N = m_Store.GetSize();
  IO.m_Split.SetSize(N);
  IO.m_QmRqd.SetSize(N);
  IO.m_Qm.SetSize(N);
  IO.m_QmPass.SetSize(N);
  for (int p=0; p<N; p++)
    {
    IO.m_Split[p]=0.0;
    IO.m_QmRqd[p]=0.0;
    IO.m_Qm[p]=0.0;
    IO.m_QmPass[p]=som_ALL;
    }
  m_StoreIO.InsertAt(n, IO);
  };

//--------------------------------------------------------------------------

void CMultiStorage::RemoveIO(int n)
  {
  m_StoreIO.RemoveAt(n);
  };

//--------------------------------------------------------------------------

bool CMultiStorage::IOOutSplitOK(int i)
  {
  return (IOId_Self(i)==idProd && IOQmEst_Out(i)<0.99*MaxXfCap);
  };

//---------------------------------------------------------------------------

flag CMultiStorage::GetOtherData(FilingControlBlock &FCB)
  {
  flag OK=true;
  //for (int p=0; p<m_Store.GetCount(); p++)
  //  OK = OK && m_Store[p].GetOtherData(FCB);
  return OK;
  };

//---------------------------------------------------------------------------

flag CMultiStorage::PutOtherData(FilingControlBlock &FCB)
  {
  flag OK=true;
  //for (int p=0; p<m_Store.GetCount(); p++)
  //  OK = OK && m_Store[p].PutOtherData(FCB);
  return OK;
  };

//---------------------------------------------------------------------------

void CMultiStorage::PostConnect(int IONo)
  {
  if (!PostConnectDirect(IONo))
    {
    MdlNode::PostConnect(IONo);
    AddIO(IONo);
    }
  };

//---------------------------------------------------------------------------

void CMultiStorage::PreDisConnect(int IONo)
  {
  if (!PreDisConnectDirect(IONo))
    {
    RemoveIO(IONo);
    MdlNode::PreDisConnect(IONo);
    }
  };

//--------------------------------------------------------------------------

void CMultiStorage::SetState(eScdMdlStateActs RqdState)
  {
  MdlNode::SetState(RqdState);
  //m_Q.SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
    case MSA_Empty:
      {
      for (int p=0; p<m_Store.GetCount(); p++)
        m_Store[p].ZeroMass();
      break;
}
    case MSA_PreSet:
  //    m_Q.Empty();
  //    m_Q.SetTotalSpilt(0.0);
  //    break;
    case MSA_SteadyState:
      break;
    }
  };

//---------------------------------------------------------------------------

flag CMultiStorage::InitialiseSolution()
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      for (int p=0; p<m_Store.GetSize(); p++)
        m_Store[p].SetStateAction(IE_Disabled);
      break;
    case SM_Inline:
      for (int p=0; p<m_Store.GetSize(); p++)
        m_Store[p].SetStateAction(IE_SaveState);
      break;
    case SM_Buffered:
      for (int p=0; p<m_Store.GetSize(); p++)
        m_Store[p].SetStateAction(IE_Integrate);
      break;
    }

  return 1;
  };

//---------------------------------------------------------------------------

void CMultiStorage::StartSolution()
  {
  };

//---------------------------------------------------------------------------

void CMultiStorage::ConfigureJoins()
  {
  //int i;
  switch (NetMethod())
    {
    case NM_Probal:
      //for (i=0; (i<NoFlwIOs()); i++)
      //  SetIO_Join(i, 0);
      //break;
    case NM_Dynamic:
      for (int i = 0; i < NoFlwIOs(); i++)
        SetIO_Open(i, 0, false, ESS_Denied);
      break;
    }
  };

//---------------------------------------------------------------------------

void CMultiStorage::StartStep()
  {
  for (int p=0; p<m_Store.GetSize(); p++)
    m_AccCalc.Start(p, m_Store[p].MArray(), m_Store[p].totHz());
  };

//--------------------------------------------------------------------------

void CMultiStorage::EvalJoinPressures(long JoinMask)
  {
  for (int i = 0; i < NoFlwIOs(); i++)
    Set_IOP_Self(i, AtmosPress(IODatum_Term(i)));
  };

//--------------------------------------------------------------------------

flag CMultiStorage::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  switch (Task)
    {
    case FET_GetMode:
      {
      int Feed=0;
      for (int i=0; i<IONo; i++)
        if (IOId_Self(i)==idFeed)
          Feed++;
      if (IOId_Self(IONo)==idFeed)
        {
        IOFB(IONo,FE)->SetXferMode();
        return True;
        }

      IOFB(IONo,FE)->SetQmFree();
      break;
      }
    case FET_SetQm:
      IOFB(IONo, FE)->SetDPb(0.0, 0.0);
      IOFB(IONo, FE)->SetDPbX(0.0);
      IOFB(IONo, FE)->SetDPq(0.0, 0.0);
      break;
    case FET_CalcDP:
      break;
    }
  return True;
  };

//--------------------------------------------------------------------------

void CMultiStorage::EvalProducts(long JoinMask)
  {
  #if dbgMultiStore
  int DoDbg = dbgEvalProducts() || DoDbgBrk() && dbgEvalProductsBrk();
  #endif
  switch (SolveMethod())
    {
    case SM_Direct:
      DoBreak();
      if (NJoins()>=1)
        Xfer_EvalProducts(0, Joins[0].Pressure(), NULL, NULL, NULL, NULL, NULL);
      break;
    case SM_Inline:
    case SM_Buffered:
      {
      //check if product split sum to 100%
      for (int i=0; i<NoFlwIOs(); i++)
        m_StoreIO[i].ValidateSplit();

      //determine number of feed/product streams...
      int FeedCnt=0;
      int ProdCnt=0;
      bool HasPropMassFrac = false;
      for (int i=0; i<NoFlwIOs(); i++)
        {
        if (m_StoreIO[i].m_iSplitMethod==SM_PropMassFrac)
          HasPropMassFrac = true;
        const int id = IOId_Self(i);
        if (id==idProd)
          {
          IOConduit(i)->QZero();
          ProdCnt++;
          }
        else if (id==idFeed)
          {
          FeedCnt++;
          }
        }

      #if dbgMultiStore
      if (DoDbg)
        {
        dbgpln("MultiStore At Start ---------------------------------------------- %s", FullObjTag());
        for (int p=0; p<m_Store.GetSize(); p++)
          {
          dbgpln("Store %2i %14.4f", p, m_Store[p].Mass());
          }
        }
      #endif

      //determine feed flow for each store for each feed stream...
      for (int p=0; p<m_Store.GetSize(); p++)
        {
        int Feed=0;
        for (int i=0; i<NoFlwIOs(); i++)
          {
          if (IOId_Self(i)==idFeed)
            {
            const double Qm=IOQm_In(i);
            m_StoreIO[i].m_Qm[p]=Qm*m_StoreIO[i].m_Split[p];
            m_StoreIO[i].m_QmRqd[p]=m_StoreIO[i].m_Qm[p];
            m_Store[p].SetFeed(Feed++, m_StoreIO[i].m_Qm[p], *IOConduit(i));
            }
          }
        }

      //add feed for all input streams to contents of each store...
      for (int p=0; p<m_Store.GetCount(); p++)
        {
        CMultiStore & St=m_Store[p];

#if WITHMSSRCSNK
        //add material for 'built in feed source'...
        if ((St.Mass()>10*TraceMass) && (St.m_SrcQm>1.0e-10))
          {
          const double MSrc=ICGetTimeInc()*St.m_SrcQm;
          const double MTot=St.Mass();
          St.pModel->ScaleMass(som_ALL, GEZ(MTot+MSrc)/GTZ(MTot));
          }
#endif

        //add material for Feeds...
        St.pModel->AddMassM(St.m_Feed, som_ALL, ICGetTimeInc()*St.m_Feed.QMass());
        }

      // reactions
      if (m_bRBsOn)
        {
        for (int p=0; p<m_Store.GetSize(); p++)
          {
          CConvergeStateBlk CSB(eStateAdvance, ICGetTimeInc());
          //Contents.MeanResTimeCalcsReqd(true);
          m_StoreRB[p].ConvergeStates(CSB, m_Store[p], NULL);//, Fd, Pr);
          }
        }

      //calculate percent split based on store contents
      if (HasPropMassFrac)
        {
        double Ttl=0.0;
        for (int p=0; p<m_Store.GetCount(); p++)
          Ttl += m_Store[p].Mass();

        for (int p=0; p<m_Store.GetCount(); p++)
          {
          for (int i=0; i<NoFlwIOs(); i++)
            {
            const int id = IOId_Self(i);
            if (id==idProd && m_StoreIO[i].m_iSplitMethod==SM_PropMassFrac)
              {
              m_StoreIO[i].m_Split[p] = (Ttl>TraceMass ? m_Store[p].Mass()/Ttl : 1.0/ProdCnt);
              }
            }
          }
        }

      //remove mass for all output streams from contents of each store...
      for (int p=0; p<m_Store.GetCount(); p++)
        {
        CMultiStore & St=m_Store[p];
        #if dbgMultiStore
        if (DoDbg)
          dbgpln("Store %i -----------------------------------------", p);
        #endif

        double TotMass=St.Mass();
        double StPhMass[MaxPhases];
        double StPhRqdMass[MaxPhases];
        //double StPhMassFrac[MaxPhases];
        double StPhScale[MaxPhases];
        for (int phi=0, phm=1; (phm & som_ALL)!=0; phi++, phm<<=1)
          {
          StPhMass[phi]=St.Mass(phm);
          //StPhMassFrac[phi]=StPhMass[phi]/GTZ(TotMass);
          StPhRqdMass[phi]=0;
          }

        double QmRqdTot=0.0;
        for (int i=0; i<NoFlwIOs(); i++)
          {
          const int id = IOId_Self(i);
          if (id==idProd)
            {
            CMultiStoreIO &IO=m_StoreIO[i];
            IO.m_PhFrac.SetSize(CDB.PhaseCount());
            const double QmEstOut = IOQmEst_Out(i);
            const int SplM = IO.m_iSplitMethod;
            double &Qm=IO.m_QmRqd[p];
            if (SplM==SM_Frac || SplM==SM_EqFrac || SplM==SM_PropMassFrac || SplM==SM_Store)
              Qm=QmEstOut*m_StoreIO[i].m_Split[p];
            #if dbgMultiStore
            if (DoDbg)
              dbgpln("IO %i %#06x Est:%10.2f Qm:%10.2f", i, m_StoreIO[i].m_QmPass[p], QmEstOut, Qm);
            QmRqdTot+=Qm;
            #endif

            double StTot4IO=0.0;
            for (int phi=0, phm=1; (phm & som_ALL)!=0; phi++, phm<<=1)
              {
              if (phm & IO.m_QmPass[p])
                StTot4IO+=StPhMass[phi];
              }
            #if dbgMultiStore
            if (DoDbg)
              dbgpln("  %6s %10s %10s", "Phase", "IOPhFrac", "StPhRqdM");
            #endif
            for (int phi=0, phm=1; (phm & som_ALL)!=0; phi++, phm<<=1)
              {
              if (phm & m_StoreIO[i].m_QmPass[p])
                {
                IO.m_PhFrac[phi]=StPhMass[phi]/GTZ(StTot4IO);
                StPhRqdMass[phi]+=Qm*IO.m_PhFrac[phi]*ICGetTimeInc();
                #if dbgMultiStore
                if (DoDbg)
                  dbgpln("  %#06x %10.2f %10.2f", phi, IO.m_PhFrac[phi], StPhRqdMass[phi]);
                #endif
                }
              }
            }
          }

        for (int phi=0, phm=1; (phm & som_ALL)!=0; phi++, phm<<=1)
          {
          StPhScale[phi]=Min(1.0, StPhMass[phi]/GTZ(StPhRqdMass[phi]));
          }

        #if dbgMultiStore
        if (DoDbg)
          {
          dbgpln("%6s %10s %10s %10s", "Phase", "PhMass", "PhRqdMass", "PhScale");
          for (int phi=0, phm=1; (phm & som_ALL)!=0; phi++, phm<<=1)
            {
            dbgpln("%#06x %10.2f %10.2f %10.2f ", phm, StPhMass[phi], StPhRqdMass[phi], StPhScale[phi]);
            }
          }
        #endif

        //double QmRqdTot=0.0;
        //for (int i=0; i<NoFlwIOs(); i++)
        //  {
        //  const int id = IOId_Self(i);
        //  if (id==idProd)
        //    {
        //    const double QmEstOut = IOQmEst_Out(i);
        //    const int SplM = m_StoreIO[i].m_iSplitMethod;
        //    double &Qm=m_StoreIO[i].m_QmRqd[p];
        //    if (SplM==SM_Frac || SplM==SM_EqFrac || SplM==SM_PropMassFrac || SplM==SM_Store)
        //      Qm=QmEstOut*m_StoreIO[i].m_Split[p];
        //    QmRqdTot+=Qm;
        //    for (int phi=0, phm=1; (phm & som_ALL)!=0; phi++, phm<<=1)
        //      if (phm & m_StoreIO[i].m_QmPass)
        //        StPhRqdMass[phi]+=Qm*StPhMassFrac[phi]*ICGetTimeInc();
        //    }
        //  }


#if WITHMSSRCSNK
        St.m_TotProdQmRqd = QmRqdTot+St.m_SnkQm;
        St.m_ProdQmTotal = 0.0;
#else
        St.m_TotProdQmRqd = QmRqdTot;
        St.m_ProdQmTotal = 0.0;
#endif



        #if dbgMultiStore
        if (DoDbg)
          dbgpln("%3s %6s %10s %10s %10s", "IO", "Phase", "MOut", "MTot", "Scl");
        #endif
        int Prod=0;
        for (i=0; i<NoFlwIOs(); i++)
          {
          if (IOId_Self(i)==idProd)
            {
            CMultiStoreIO &IO=m_StoreIO[i];
//---------------
            double IOQmActual=0.0;
            for (int phi=0, phm=1; (phm & som_ALL)!=0; phi++, phm<<=1)
              {
              //StPhScale[phi]=Min(1.0, StPhRqdMass[phi]);

              //m_ProdQm[ProdNo]=QmRqd;
              double Scale = StPhScale[phi];
              double QmRqd = m_StoreIO[i].m_QmRqd[p];
#if WITHMSSRCSNK
              St.m_ProdQmTotal+=Scale*St.m_SnkQm;
#else
#endif
              double PhQmActual = StPhScale[phi]*QmRqd*IO.m_PhFrac[phi];
              IOConduit(i)->QAddM(St, phm, PhQmActual);

              St.m_ProdQmTotal+=PhQmActual ;
              IOQmActual+=PhQmActual;

              const double MOut=ICGetTimeInc()*PhQmActual;
              const double MTot=St.Mass(phm);
              const double Scl=GEZ(MTot-MOut)/GTZ(MTot);
              St.ScaleMass(phm, Scl);
              #if dbgMultiStore
              if (DoDbg)
                dbgpln("%3i %#06x %10.2f %10.2f %10.6f ", i, phm, MOut, MTot, Scl);
              #endif
              }

            m_StoreIO[i].m_Qm[p] = IOQmActual;
//---------------
            //const double QmActual = St.GetProduct(Prod++, m_StoreIO[i].m_QmPass, m_StoreIO[i].m_QmRqd[p], *IOConduit(i));
            //m_StoreIO[i].m_Qm[p] = QmActual;
            }
          }

        //// Products
        //const double MOut=ICGetTimeInc()*St.m_TotProdQmRqd;
        //const double MTot=St.Mass();
        //St.pModel->ScaleMass(som_ALL, GEZ(MTot-MOut)/GTZ(MTot));
        }

      if (m_bTrackFlwReq)
        {
        bool FlwOverspecifiedError = false;
        bool FlwRequirementNotMet = false;
        for (int i=0; i<NoFlwIOs(); i++)
          {
          const int id = IOId_Self(i);
          if (id==idProd)
            {
            const double QmEstOut = IOQmEst_Out(i);
            if (QmEstOut>=0.0 && QmEstOut<0.999*MaxXfCap)
              {
              const double QmActual = IOConduit(i)->QMass();
              if (QmActual>QmEstOut+1.0e-6)
                FlwOverspecifiedError = true;
              else if (QmActual<QmEstOut-1.0e-6)
                FlwRequirementNotMet = true;
              }
            }
          }
        SetCI(1, FlwOverspecifiedError);
        SetCI(2, FlwRequirementNotMet);
        }
      else
        {
        ClrCI(1);
        ClrCI(2);
        }
      #if dbgMultiStore
      if (DoDbg)
        {
        dbgpln("MultiStore At End");
        for (int i=0; i<NoFlwIOs(); i++)
          {
          const int id = IOId_Self(i);
          if (id==idProd)
            {
            dbgpln("Out %2i %14.4f %14.4f", i, ICGetTimeInc()*IOQm_Out(i), ICGetTimeInc()*IOConduit(i)->QMass());
            }
          else if (id==idFeed)
            {
            dbgpln("In  %2i %14.4f %14.4f", i, ICGetTimeInc()*IOQm_In(i), ICGetTimeInc()*IOConduit(i)->QMass());
            }
          else
            {
            dbgpln("??? %2i", i);
            }
          }
        for (int p=0; p<m_Store.GetSize(); p++)
          {
          dbgpln("Store %2i %14.4f", p, m_Store[p].Mass());
          }
        }
      #endif
      SetIntegralDone(true);
      }
      break;
    }
  //m_Src.Cd.QSetF(*Contents.Model()->m_DC.m_pFd, som_ALL, 1.0);
  //m_Snk.Cd.QSetF(*Contents.Model()->m_DC.m_pFd, som_ALL, 1.0);
  //m_AccIn.Cd.QSetF(*Contents.Model()->m_DC.m_pFd, som_ALL, 1.0);
  //m_AccOut.Cd.QSetF(*Contents.Model()->m_DC.m_pPr, som_ALL, 1.0);

  }

//--------------------------------------------------------------------------

void CMultiStorage::ODEOperate(CODEDataBlock & ODB)
  {
  bool DoIt=true;
  switch (ODB.m_Cmd)
    {
    case eStateAdvance:
      DoIt=!IntegralDone();
    case eStateConverge:
    case eStateLoad:
    case eStateDiskLoad:
      {
      if (DoIt && !GetActiveHold())
        {
        #if dbgMSurge
        if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
          {
          dbgp("ODE:---  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f",
            m_CnStart.DTemp(Contents), Contents.Temp(),
            m_CnStart.DPress(Contents), Contents.Press(),
            m_CnStart.DMass(Contents), Contents.Mass(),
            m_CnStart.DLevel(Contents)*100, Contents.Level()*100, FullObjTag());
          if (0)
            for (int s=0; s<SVDataSize(); s++)
              dbgp(" %12.5g", Contents.MArray()[s]);
          dbgpln(" %s.Contents", FullObjTag());
          }
        #endif
        for (int p=0; p<m_Store.GetSize(); p++)
          m_Store[p].pModel->ODEOperate(ODB);
      
        #if dbgMSurge
        if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
          {
          dbgp("ODE:Int  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f",
            m_CnStart.DTemp(Contents), Contents.Temp(),
            m_CnStart.DPress(Contents), Contents.Press(),
            m_CnStart.DMass(Contents), Contents.Mass(),
            m_CnStart.DLevel(Contents)*100, Contents.Level()*100, FullObjTag());
          if (0)
            for (int s=0; s<SVDataSize(); s++)
              dbgp(" %12.5g", Contents.MArray()[s]);
          dbgpln(" %s.Contents", FullObjTag());
          }
        #endif

        ConvergeStates(CConvergeStateBlk(ODB));
        #if dbgMSurge
        if (dbgODEOperate() || dbgODEOperateDbgBrk() && fDoDbgBrk)
          {
          dbgp("ODE:Cvg  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f",
            m_CnStart.DTemp(Contents), Contents.Temp(),
            m_CnStart.DPress(Contents), Contents.Press(),
            m_CnStart.DMass(Contents), Contents.Mass(),
            m_CnStart.DLevel(Contents)*100, Contents.Level()*100);
          if (0)
            for (int s=0; s<SVDataSize(); s++)
              dbgp(" %12.5g", Contents.MArray()[s]);
          dbgpln(" %s.Contents", FullObjTag());
          }
        #endif
        }
      break;
      }
    case eStateFixDV:
    case eStateTest:
    case eStateSave:
    case eStateDiskSave:
      {
      for (int p=0; p<m_Store.GetSize(); p++)
        m_Store[p].pModel->ODEOperate(ODB);
      break;
      }
    }
  };

//---------------------------------------------------------------------------

void CMultiStorage::EvalPowerRequired()
  {
  //m_Pwr.Zero();
  //if (Valid(m_Power))
  //  m_Pwr.SetPower(m_Power);
  }

//--------------------------------------------------------------------------

void CMultiStorage::EvalCtrlActions(eScdCtrlTasks Tasks)
  {
  //m_BeltSB.EvalCtrlActions(this);
  }

//--------------------------------------------------------------------------

/*This is called once, after the iteration process and allows the discreet
manipulation of the contents contained by the surge unit.*/

void CMultiStorage::EvalDiscrete()
  {
  for (int p=0; p<m_Store.GetSize(); p++)
    m_AccCalc.Complete(p, m_Store[p].MArray(), m_Store[p].totHz(), ICGetTimeInc());
  }

//--------------------------------------------------------------------------

void CMultiStorage::ClosureInfo()
  {
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];

    }
  else if (m_Closure.DoContent())
    {
    CClosureInfo &CI=m_Closure[0];
    //pRefMdl=NULL;
    for (int p=0; p<m_Store.GetSize(); p++)
      {
      CI.m_ContentMass += m_Store[p].Mass();
      CI.m_ContentHeat += m_Store[p].totHf();
      }
    }
  }

//--------------------------------------------------------------------------

int CMultiStorage::GetOutputStreamIndex(char* pStreamTag)
  {
  if (pStreamTag && strlen(pStreamTag)>0)
    {
    for (int i=0; i<NoFlwIOs(); i++)
      if (IOId_Self(i)==idProd && _stricmp(Nd_Rmt(i)->Tag(), pStreamTag)==0)
        return i;
    }
  return -1;
  }

//--------------------------------------------------------------------------

bool CMultiStorage::IsFracForStream(int StreamIndex)
  {
  return (m_StoreIO[StreamIndex].m_iSplitMethod==SM_Frac);
  }

//--------------------------------------------------------------------------

void CMultiStorage::SetStoreFracForStream(int StreamIndex, int StoreIndex, double Value)
  {
  //m_StoreIO[StreamIndex].m_iSplitMethod = SM_Frac; //force split method to fraction
  m_StoreIO[StreamIndex].m_Split[StoreIndex] = Value;
  }

//--------------------------------------------------------------------------

dword CMultiStorage::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (GetStatusAsStopped())
    {
    Status |= FNS_UNoFlw;
    //Status |= (SolveMethod()==SM_Inline  ? FNS_UFlw : FNS_UNoFlw);
    }
  else if (NoFlwIOs())
    {
    int HasFlw=0;
    double TFlw=0.0;
    for (int i=0; i<NoFlwIOs(); i++)
      {
      TFlw+=IOQm_In(i);
      if (IOConduit(i)->QMass()>UsableMass)
        HasFlw=1;
      }
    Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
    if (TFlw>1.0e-6)
      Status |= FNS_NettQmP;
    else if (TFlw<-1.0e-6)
      Status |= FNS_NettQmN;
    }
  return Status;
  }

//--------------------------------------------------------------------------

flag CMultiStorage::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\tSum of required flows from stores exceeds pipe maximum capacity."; return 1;
    case  2: pS="W\tSum of flows does not achieve pipe capacity requirement."; return 1;
    case  3: pS="E\t";              return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }                             
  };

//==========================================================================
//
//
//
//==========================================================================
