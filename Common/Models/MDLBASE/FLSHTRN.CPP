//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include "sc_defs.h"
#define  __FLSHTRN_CPP
#include "flshtrn.h"

//==========================================================================

#define dbgModels         WITHDEBUG
#if dbgModels   
#include "dbgmngr.h"
static CDbgMngr dbgFlashTrain ("HeatXch1", "FlashTrain");
#endif

//==========================================================================
//
//
//
//==========================================================================

class VapPFinder : public MRootFinderBase
  {
  protected:
    CArrayFlsh &m_Flsh;
    static CToleranceBlock s_Tol;

  public:
    VapPFinder(CArrayFlsh & Flsh);//, SpConduit &Ql, SpConduit &Qv);
    ~VapPFinder();
    double Function(double x);
  };

//---------------------------------------------------------------------------

VapPFinder::VapPFinder(CArrayFlsh &Flsh) ://;FlashTank & FT, SpConduit &Ql, SpConduit &Qv) :
  m_Flsh(Flsh), MRootFinderBase("VapPFinder", s_Tol)//1.0e-6)
  { 
  };

CToleranceBlock VapPFinder::s_Tol(TBF_Both, "FlashTrain:VapPFinder", 0, 1.0e-6);

//---------------------------------------------------------------------------

VapPFinder::~VapPFinder()
  { 
  };

//---------------------------------------------------------------------------
  
double VapPFinder::Function(double P) 
  { 
  double Q=0.0;
  for (int i=0; i<m_Flsh.GetSize(); i++)
    {
    CFT_Flash &F=*m_Flsh[i];
    F.m_dPEval = P;
    F.m_pFNd->MacroMdlEvaluate(MME_Flash2Qv); 
    Q+=F.m_dQVapEval;
    }
  return Q;
  };

// ==========================================================================
//
//
//
// ==========================================================================

CFT_Flash::CFT_Flash(FlwNode * pNd) : \
m_pNd(pNd)
  { 
  m_dQVap=0.0;
  m_dFeedP=0.0;
  m_dPRqd=101.0;
  m_dPRqdCalc=Std_P;
  m_dPEval=0.0;

  m_dQVapRqd=0.0;
  m_dQVapEval=0.0;
  m_dQVapFrac=0.001;
  m_dQFrac=0.0;
  m_dQCool=0.0;

  m_dDQvDp=0.0;
  m_fDQvDpGood=False;

  m_dFlashP=Std_P;
  m_dFlashT=Std_T;

  m_dSuctionP=Std_P;

#if DOPBSMOOTHING  
  m_dVapLoss=0.0;
#endif
  }

// --------------------------------------------------------------------------

void CFT_Flash::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double  ("Flash_Temp",        "FlashT",   DC_T,   "C",      &m_dFlashT,   m_pNd, NAN_OK );
  DDB.Double  ("Flash_Press",       "FlashP",   DC_P,   "kPag",   &m_dFlashP,   m_pNd, NAN_OK);

  dword VFlags2=(Active() ? 0 : noView);
  DDB.Double  ("QVap",              "",         DC_Qm,  "kg/s",   &m_dQVap,     m_pNd, 0/*VFlags2*/); //steam
  DDB.Double  ("PRqd",              "",         DC_P,   "kPag",   &m_dPRqd,     m_pNd, VFlags2);
  DDB.Double  ("PRqdCalc",          "",         DC_P,   "kPag",   &m_dPRqdCalc, m_pNd, noView|isParm|VFlags2);
  DDB.Double  ("dDQvDp",            "",         DC_,    "",       &m_dDQvDp,    m_pNd, /*noView|*/VFlags2);
  if (Active())
    {
    DDB.String("FlashTrain",        "",         DC_,    "",       &m_pMNd->TagRef(),  m_pNd, noFile|noSnap|VFlags2);
    DDB.String("FlashTrainEqp",     "",         DC_,    "",       MMList(),     m_pNd, MultiLineStr|noFile|noSnap|VFlags2);
    }
  }

// --------------------------------------------------------------------------

void   CFT_Flash::SetState(eScdMdlStateActs RqdState)
  {
  switch (RqdState)
    {
    case MSA_PBInit:
      m_dQVap=0.0;
      m_dFeedP=0.0;
      m_dFeedVapP=10000.0;
      m_dPRqd=Std_P;
      m_dPRqdCalc=Std_P;
      m_dPEval=0.0;
      m_dDQvDp=0.0;
      m_fDQvDpGood=False;

      m_dFlashP=Std_P;
      m_dFlashT=Std_T;

      m_dSuctionP=Std_P;
      break;
    case MSA_ZeroFlows:
    case MSA_Empty:
    case MSA_PreSet:
      break;
    case MSA_SteadyState:
      break;
    }
  }

// ==========================================================================
//
//
//
// ==========================================================================

CFT_Condenser::CFT_Condenser(FlwNode * pNd) : \
m_pNd(pNd)
  {
  m_dSuctionP=Std_P;
  m_dQRqd=0.0;
  m_dQCond=0.0;
  m_dQCool=0.0;
  m_dMinSatPress=Std_P;
#if DOPBSMOOTHING  
  m_dVapLoss=0.0;
#endif
  }

void CFT_Condenser::BuildDataDefn(DataDefnBlk & DDB)
  {
  dword VFlags=(Active() ? 0 : noView);
  DDB.Double("SuctionP",      "", DC_P,    "kPa",       &m_dSuctionP,       m_pNd, isResult|VFlags);
  DDB.Double("QRqd",          "", DC_Qm,   "kg/s",      &m_dQRqd,           m_pNd, isResult|VFlags);
  DDB.Double("QCond",         "", DC_Qm,   "kg/s",      &m_dQCond,          m_pNd, isResult|VFlags);
  DDB.Double("MinSatPress",   "", DC_P,    "kPa",       &m_dMinSatPress,    m_pNd, isResult|VFlags);
  if (Active())
    {
    DDB.String("FlashTrain",    "", DC_,     "",        &m_pMNd->TagRef(),  m_pNd, noFile|noSnap|VFlags);
    DDB.String("FlashTrainEqp", "", DC_,     "",        MMList(),           m_pNd, MultiLineStr|noFile|noSnap|VFlags);
    }
  };

void CFT_Condenser::SetState(eScdMdlStateActs RqdState)
  {
  switch (RqdState)
    {
    case MSA_PBInit:
      m_dSuctionP=Std_P;
      m_dQRqd=0.0;
      m_dQCond=0.0;
      m_dQCool=0.0;
      m_dMinSatPress=Std_P;
      break;
    case MSA_ZeroFlows:
    case MSA_Empty:
    case MSA_PreSet:
      break;
    case MSA_SteadyState:
      //LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  }

// ==========================================================================
//
//
//
// ==========================================================================

CFT_Mixer::CFT_Mixer(FlwNode * pNd) : \
m_pNd(pNd)
  {
  m_bPartOfFlashTrain=False;
  }

void CFT_Mixer::BuildDataDefn(DataDefnBlk & DDB)
  {
  };

void CFT_Mixer::Activate(CMacroMdl* pMMNd, pFlwNode pNd)
  {
  CMacroMdlBase::Activate(pMMNd, pNd);
  m_bPartOfFlashTrain=True;
  }

void CFT_Mixer::Deactivate()
  {
  CMacroMdlBase::Deactivate();
  m_bPartOfFlashTrain=False;
  }

//==========================================================================
//
//
//
//==========================================================================

#if KeepOldTearVarBlkEdit
IMPLEMENT_TAGOBJEDTDERIVED(FlshTrnTearVarBlk, "FlshTrnTearVarBlk", "TbFlshTrn", "", "TBFT", TOC_SYSTEM, 
                           TearVarBlkEdt, 
                           "Flash Train TearVarBlk", "Flash Train Tear Variable Calculation Blk",
                           TearVarBlkClassDef);
#else
IMPLEMENT_TAGOBJDERIVED(FlshTrnTearVarBlk, "FlshTrnTearVarBlk", "TbFlshTrn", "", "TBFT", TOC_SYSTEM, 
                           "Flash Train TearVarBlk", "Flash Train Tear Variable Calculation Blk",
                           TearVarBlkClassDef);
#endif

FlshTrnTearVarBlk::FlshTrnTearVarBlk(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TearVarBlk(pClass_, Tag_, pAttach, eAttach)
    {
    DoConstruct();
    };

// --------------------------------------------------------------------------

FlshTrnTearVarBlk::FlshTrnTearVarBlk(pchar Tag_, pTaggedObject pAttach) :
  TearVarBlk(&FlshTrnTearVarBlkClass, Tag_, pAttach, TOA_Free)
    {
    DoConstruct();
    };

// --------------------------------------------------------------------------

FlshTrnTearVarBlk::~FlshTrnTearVarBlk() 
  {
  };

// --------------------------------------------------------------------------

void FlshTrnTearVarBlk::DoConstruct()
  {
  PCnv.Set(DC_P, "kPa");
  PFmt.Set("", 0, 2, 'f');
  sGroupTag="FlashTrain";
  iTearTagTyp=TTT_FlashTrain;
//  if (1) // (!MassSmoothing())
//    {
  SetNVariables(1, TVU_Inputs|TVU_Outputs|TVU_Meas|TVU_AdvanceVars|TVU_TestVars);//, TVT_Tear);
  int n=0;
  TV[n].Tag.Set("RequiredP");
  TV[n].Sym.Set("RqdP");
  TV[n].AdvCnv=&PCnv;
  TV[n].AdvFmt=&PFmt;
  TV[n].MeasCnv=&PCnv;
  TV[n].MeasFmt=&PFmt;
//    }
//  else
//    SetNVariables(0, TVT_Tear);
  };

// --------------------------------------------------------------------------

void FlshTrnTearVarBlk::OnEditConstruct() 
  {
  ObjectAttribute *pAttr=ObjAttributes.FindObject("FlshTrnTearEdt");
  if (pAttr)
    {
    pAttr->FieldFmtCnvs("RequiredP", PFmt, PCnv);
    //pAttr->FieldRange("RequiredP", dXDispMin, dXDispMax);
    }
  };

// --------------------------------------------------------------------------

void FlshTrnTearVarBlk::OnEditDestroy() 
  {
  ObjectAttribute *pAttr=ObjAttributes.FindObject("FlshTrnTearEdt");
  if (pAttr)
    {
    pAttr->SetFieldFmt("RequiredP", PFmt);
    pAttr->SetFieldCnvs("RequiredP", PCnv);
    //pAttr->SetFieldMin("RequiredP", dXDispMin);
    //pAttr->SetFieldMax("RequiredP", dXDispMax);
    }
  };

//==========================================================================
//
//
//
//==========================================================================

CMacroMdlIOArray CMMFlashTrain::MMIOs;

IMPLEMENT_TAGOBJ(CMMFlashTrain, "CMacroMdl", "FlshTrn", "", "Tank", "FT", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT, "CMMFlashTrain", "CMMFlashTrain") 

//==========================================================================
//
//
//
//==========================================================================

CMMFlashTrain::CMMFlashTrain(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CMacroMdl(pClass_, TagIn, pAttach, eAttach),
  TearPosBlk(TT_SystemTear)
  {
//  if (!CMacroMdl::RegisterNode(MMIOs, "Tie-1", 0, mmio_CONNECT, NULL))
//    LogError("FlashTrain", 0, "Unable to register 'Tie' as a connector"); 
//  if (!CMacroMdl::RegisterNode(MMIOs, "Pipe-1", 0, mmio_CONNECT, NULL))
//    LogError("FlashTrain", 0, "Unable to register 'Pipe' as a connector"); 
//  if (!CMacroMdl::RegisterNode(MMIOs, "Pipe-1", 1, mmio_CONNECT, NULL))
//    LogError("FlashTrain", 0, "Unable to register 'Pipe' as a connector"); 
  };

CMMFlashTrain::~CMMFlashTrain() 
  {
  };


CMacroMdlIO * CMMFlashTrain::ValidNd(flag fIsModel, pFlwNode pNd, int iIONo) 
  {
  return CMacroMdl::ValidNd(fIsModel, pNd, iIONo, MMIOs); 
  };

//--------------------------------------------------------------------------

flag CMMFlashTrain::ValidModel()
  {
  int NFlash=0, NCond=0;
  for (int i=0; i<Nds.GetSize(); i++)
    if (Nds[i].m_pNdVarTypeId)
      if (*Nds[i].m_pNdVarTypeId==typeid(CFT_Flash))
        NFlash++;
      else if (*Nds[i].m_pNdVarTypeId==typeid(CFT_Condenser))
        NCond++;
  return (NFlash>0 && NCond>0);
  }

//--------------------------------------------------------------------------

flag CMMFlashTrain::PBInitialise()
  {
  PMean=Std_P;
  // Find Tag of Flashtrain (Prioritised)
  iIter=0;
  Strng T1, T2;
  int TagLvl=-1;
  for (int iNd=0; iNd<Nds.GetSize(); iNd++)
    {
    CMacroMdlNd &Nd=Nds[iNd];
    FlwNode *pFNd=Nd.m_pFNd;

    pFNd->fActiveInMacroMdl=true;

    if (iNd==0)
      T1=pFNd->Tag();

    if (Nd.m_pNdVarTypeId)
      if (*Nd.m_pNdVarTypeId==typeid(CFT_Flash))
        {
        if (TagLvl<2 && _stricmp(pFNd->Tag(), T1())<0)
          {
          T1=pFNd->Tag();
          TagLvl=2;
          }
        Nd.m_pVars=pFNd->MacroMdlActivate();
        if (Nd.m_pVars&& !Flsh.Exists((CFT_Flash*)Nd.m_pVars))
          {
          ASSERT(typeid(*Nd.m_pVars)==typeid(CFT_Flash));
          Flsh.Add((CFT_Flash*)Nd.m_pVars);
          Nd.m_pVars->Activate(this, pFNd);//, iNd, Nds);
          }
        }
      else if (*Nd.m_pNdVarTypeId==typeid(CFT_Condenser))
        {
        if (TagLvl<1 && _stricmp(pFNd->Tag(), T1())<0)
          {
          T1=pFNd->Tag();
          TagLvl=1;
          }
        Nd.m_pVars=pFNd->MacroMdlActivate();
        if (Nd.m_pVars && !Cond.Exists((CFT_Condenser*)Nd.m_pVars))
          {
          ASSERT(typeid(*Nd.m_pVars)==typeid(CFT_Condenser));
          Cond.Add((CFT_Condenser*)Nd.m_pVars);
          Nd.m_pVars->Activate(this, pFNd);//, iNd, Nds);
          }
        }
      else if (*Nd.m_pNdVarTypeId==typeid(CFT_Mixer))
        {
        if (TagLvl<0 && _stricmp(pFNd->Tag(), T1())<0)
          {
          T1=pFNd->Tag();
          TagLvl=0;
          }
        Nd.m_pVars=pFNd->MacroMdlActivate();
        if (Nd.m_pVars)
          {
          ASSERT(typeid(*Nd.m_pVars)==typeid(CFT_Mixer));
          Nd.m_pVars->Activate(this, pFNd);//, iNd, Nds);
          }
        }
      else
        {
        int xxx=0;
        }

    }

  T2.Set("%s_FT", T1());
  Tag(T2());

  #if dbgModels   
  if (dbgFlashTrain())
    {
    dbgpln(" -- PBInitialise %s", Tag());
    for (int i=0; i<Nds.GetSize(); i++)
      dbgpln(" %s %s(%i) -> %s",Nds[i].m_pNdVarTypeId ? Nds[i].m_pNdVarTypeId->name() : "      ... ", 
              Nds[i].m_pFNd->FullObjTag(), Nds[i].m_iIONo, Nds[i].m_pFNd->Nd_Rmt(Nds[i].m_iIONo)->Tag());
    }
  #endif
  
  for (int i=0; i<Flsh.GetSize(); i++)
    {
    CFT_Flash &F=*Flsh[i];
    F.m_Iter=0;
    F.m_dPRqdCalc=F.m_dPRqd;

    }
  
  for (i=0; i<Cond.GetSize(); i++)
    Cond[i]->m_Iter=0;
  
  return True;
  };

//--------------------------------------------------------------------------

void CMMFlashTrain::PBTerminate()
  {
  Flsh.SetSize(0);
  Cond.SetSize(0);
  for (int iNd=0; iNd<Nds.GetSize(); iNd++)
    {
    CMacroMdlNd &Nd=Nds[iNd];
    FlwNode *pFNd=Nd.m_pFNd;

    pFNd->fActiveInMacroMdl=false;

    if (Nd.m_pNdVarTypeId)
      if (*Nd.m_pNdVarTypeId==typeid(CFT_Flash))
        {
        pFNd->MacroMdlDeactivate();
        if (Nd.m_pVars && Nd.m_pVars->Active())
          Nd.m_pVars->Deactivate();
        }
      else if (*Nd.m_pNdVarTypeId==typeid(CFT_Condenser))
        {
        pFNd->MacroMdlDeactivate();
        if (Nd.m_pVars && Nd.m_pVars->Active())
          Nd.m_pVars->Deactivate();
        }
      else if (*Nd.m_pNdVarTypeId==typeid(CFT_Mixer))
        {
        pFNd->MacroMdlDeactivate();
        if (Nd.m_pVars && Nd.m_pVars->Active())
          Nd.m_pVars->Deactivate();
        }
    }
  }

//--------------------------------------------------------------------------

void CMMFlashTrain::PBPreJoinPressures()
  {
  PMean=0.0;
  double QFlsh=0.0;
  for (int i=0; i<Flsh.GetSize(); i++)
    {
    CFT_Flash &F=*Flsh[i];
    double Q=GTZ(F.m_dQVap);
    QFlsh+=Q;
    PMean+=F.m_dFlashP*Q;
    }
  PMean/=QFlsh;

  double MeanSuctionPAsSet=0.0;
  PMean=Range(MinFlshTrainP, PMean, 6000.0);
  for (i=0; i<Cond.GetSize(); i++)
    {
    CFT_Condenser &C=*Cond[i];
    MeanSuctionPAsSet+=C.m_dSuctionP;
    C.m_dSuctionP=0.5*(PMean+C.m_dSuctionP);
    }

  MeanSuctionPAsSet/=Max(1,Cond.GetSize());
  for (i=0; i<Flsh.GetSize(); i++)
    Flsh[i]->m_dSuctionP=MeanSuctionPAsSet;
  
#if dbgModels   
  if (dbgFlashTrain())
    dbgpln("Tr: QFlsh:PMean %12.6f %s", PMean, Tag());
  #endif
  };

//--------------------------------------------------------------------------

void CMMFlashTrain::PBPreEvalProducts() 
  {
  // Force All Flash Tanks in this Module o be at the same pressure
  double PMin=1.0e5;
  for (int i=0; i<Flsh.GetSize(); i++)
    if (Flsh[i]->m_dQVap>1.0e-10) // ? Is it Flashing
      PMin=Min(PMin, Flsh[i]->m_dFlashP);
  if (PMin==1.0e5)
    for (int i=0; i<Flsh.GetSize(); i++)
      PMin=Min(PMin, Flsh[i]->m_dFlashP);
  for (i=0; i<Flsh.GetSize(); i++)
    Flsh[i]->m_dFlashP=PMin;
  };

//--------------------------------------------------------------------------

void CMMFlashTrain::PBPostEvalProducts()
  {
  PMean=0.0;
  double QFlsh=0.0;
  for (int i=0; i<Flsh.GetSize(); i++)
    {
    CFT_Flash &F=*Flsh[i];
    double Q=GTZ(F.m_dQVap);
    QFlsh+=Q;
    PMean+=F.m_dFlashP*Q;
    }
  PMean/=QFlsh;

  double MaxFeedP=MinFlshTrainP;
  double MinFeedP=10000.0;
  double MinPRqdCalc=10000.0;

  double MaxdDQvDp=0.0;
  double MinFeedVapP=10000.0;
  for (i=0; i<Flsh.GetSize(); i++)
    {
    double P0;
    CFT_Flash &F=*Flsh[i];
    P0          = Min(F.m_dFlashP, F.m_dFeedP);//-0.1;
    MaxFeedP    = Max(F.m_dFeedP, MaxFeedP);
    MinFeedP    = Max(F.m_dFeedP, MinFeedP);

    MinPRqdCalc = Min(F.m_dPRqdCalc, MinPRqdCalc);

    MinFeedVapP = Min(MinFeedVapP, F.m_dFeedVapP);
    F.m_dPEval    = P0;
    F.m_pFNd->MacroMdlEvaluate(MME_Flash2P); 
    if (F.m_fDQvDpGood)
      MaxdDQvDp=Sign(F.m_dDQvDp)*Max(fabs(F.m_dDQvDp), fabs(MaxdDQvDp));
    }

  double MinFlashPInit=10000.0;
  double MinFlashP=MinFlashPInit;
  double QCond=0.0;
  double CQRqdTotal=0.0;

  for (i=0; i<Cond.GetSize(); i++)
    {
    CFT_Condenser &C=*Cond[i];
    QCond+=C.m_dQCond;
    CQRqdTotal+=C.m_dQRqd;
    MinFlashP=Min(MinFlashP, C.m_dMinSatPress);
    }

  double VapLoss = QFlsh-QCond;
  if (fabs(VapLoss)>0.1)
    {
    int xxx=0;
    }

  #if DOPBSMOOTHING  
  double QCool=0.0;
  CArray <CMSIO, CMSIO&> CoolLnks;
  //double VapLoss=0.0;
  for (i=0; i<Cond.GetSize(); i++)
    {
    CFT_Condenser &C=*Cond[i];
    QCool+=C.m_dQCool;
    C.pFNd->MacroMdlEvaluate(0); 
    for (int j=0; j<C.CoolLnks.GetSize(); j++)
      CoolLnks.Add(C.CoolLnks[j]);
    //VapLoss+=C.dVapLoss;
    }
  for (i=0; i<Flsh.GetSize(); i++)
    {
    CFT_Flash &F=*Flsh[i];
    int n=0;
    for (int j=0; j<CoolLnks.GetSize(); j++)
      F.CoolLnks.SetAtGrow(n++, CoolLnks[j]);
    F.CoolLnks.SetSize(n);
    F.dVapLoss=VapLoss;
    }
  #endif

  #if dbgModels   
  if (dbgFlashTrain())
    dbgpln("Tr:CQRqdTot QFlsh QCond : %12.6f %12.6f %12.6f : PMnQ %12.6f %s", 
            CQRqdTotal, QFlsh, QCond, PMean, Tag());
  #endif

  double dPRqd;
  if (fabs(MaxdDQvDp)>1.0e-20)
    {
    double DP=/*0.8**/(CQRqdTotal-QCond)/MaxdDQvDp;
    DP=Range(-PMean*0.2, DP, PMean*0.2);
    // Prevent PRqd going all the way to adjoining Pressures in one Shot
//    if (MinFlashP<MinFlashPInit)
//      DP=Range(LEZ(MinFlashP-PMean)*0.75, DP, 0.75*GEZ(MaxFeedP-PMean));
//    else
//      DP=Min(DP, 0.75*GEZ(MaxFeedP-PMean));
    if (MinFlashP<MinFlashPInit)
      DP=Max(LEZ(MinFlashP-PMean)*0.75, DP);
//    else
//      DP=Min(DP, 0.75*GEZ(MaxFeedP-PMean));
    dPRqd=PMean+DP;
    }
  else
    dPRqd=MinPRqdCalc-1.0;
//    dPRqd=Min(MaxFeedP, MinPRqdCalc-1.0);
  dPRqd=Range(MinFlshTrainP, dPRqd, MinFeedVapP);

  for (i=0; i<Flsh.GetSize(); i++)
    {
    CFT_Flash &F=*Flsh[i];
    F.m_Iter++;

    if (!MassSmoothing())
      //if (MinFlashP<MinFlashPInit)
      //  F.dPRqdCalc=Range(MinFlashP, dPRqd, F.dFeedP);
      //else
      //  F.dPRqdCalc=Min(dPRqd, F.dFeedP);
      if (MinFlashP<MinFlashPInit)
        F.m_dPRqdCalc=Max(MinFlashP, dPRqd);

//    if (MassSmoothing())
//      F.dPRqd=F.dPRqdCalc;
    #if dbgModels   
    if (dbgFlashTrain())
      dbgpln("  QAct %12.6f : dp:%12.6f P:%12.6f QvAtP %12.6f", 
        F.m_dQVap, (F.m_dFlashP-PMean), F.m_dFlashP, F.m_dDQvDp);
    #endif
    }
  double P=dPRqd;

  //CStopWatch SW;
  //SW.Start();
  if (MassSmoothing())
    {
    if (Flsh.GetSize()>1)
      {
      if (0)
        {
        //double TotFrac=0.0;
        //for (i=0; i<Flsh.GetSize(); i++)
        //  TotFrac+=Flsh[i]->dQFrac;
        //double Pm=0.0;
        //for (i=0; i<Flsh.GetSize(); i++)
        //  {
        //  CFT_Flash &F=*Flsh[i];
        //  if (TotFrac<0.1)
        //    F.dQFrac=1.0/Flsh.GetSize();
        //  else
        //    F.dQFrac/=TotFrac;
        //  F.dQVapEval=F.dQFrac*CQRqdTotal;
        //  F.pFNd->MacroMdlEvaluate(2);       
        //  Pm+=F.dQVapEval*F.dPEval;
        //  F.pFNd->MacroMdlEvaluate(0);       
        //  }
        //Pm/=GTZ(CQRqdTotal);
        //dPRqd=Pm;
        //double Qm=0.0;
        //for (i=0; i<Flsh.GetSize(); i++)
        //  {
        //  CFT_Flash &F=*Flsh[i];
        //  double Dq=0.8*F.dDQvDp*(F.dPEval-Pm);
        //  F.dQVapEval=Range(0.5*F.dQVapEval, F.dQVapEval-Dq, 1.5*F.dQVapEval);
        //  Qm+=F.dQVapEval;
        //  }
        //for (i=0; i<Flsh.GetSize(); i++)
        //  {
        //  CFT_Flash &F=*Flsh[i];
        //  F.dQFrac = 0.9*F.dQFrac+0.1*(F.dQVapEval / GTZ(Qm));
        //  }
        //
        //for (i=0; i<Flsh.GetSize(); i++)
        //  {
        //  CFT_Flash &F=*Flsh[i];
        //  F.dQRqdTotal = CQRqdTotal;
        //  F.m_dQCool = QCool;
        //  dbgpln("FlshTrn %10.6f  %10.6f  %10.6f %s", F.dQFrac, F.dQVapRqd, F.dQCool, sTag());
        //  }
        }
      else
        {
        // Old ...............
        VapPFinder VPF(Flsh);
        VPF.SetTarget(CQRqdTotal);
        VPF.SetLimits(1.0, dPRqd*10.0);
        if (VPF.SolveFromEst(dPRqd-0.1, -1.0, 5.0)==RF_OK)
          dPRqd=VPF.Result(); 
        else
          LogError("FlashTank", 0, "VapourP Finder not converged"); 
        double FracTot=0.0;
        for (i=0; i<Flsh.GetSize(); i++)
          {
          CFT_Flash &F=*Flsh[i];
          F.m_dQVapRqd = F.m_dQVapEval;
          F.m_dQFrac = F.m_dQVapEval / GTZ(CQRqdTotal);
          FracTot+=F.m_dQFrac;
          //if (MassSmoothing())
          //  {
          //  F.dPRqd=dPRqd;
          //  F.dPRqdCalc=dPRqd;
          //  }
          }
        for (i=0; i<Flsh.GetSize(); i++)
          {
          CFT_Flash &F=*Flsh[i];
          F.m_dQRqdTotal = CQRqdTotal;
          F.m_dQFrac /= GTZ(FracTot);
#if DOPBSMOOTHING  
          F.m_dQCool = QCool;
#endif
          #if dbgModels   
          if (dbgFlashTrain())
            dbgpln("FlshTrn %10.6f  %10.6f  %10.6f %s", F.m_dQFrac, F.m_dQVapRqd, F.m_dQCool, Tag());
          #endif
          }
        }
      }
    else 
      {
      CFT_Flash &F=*Flsh[0];
      F.m_dQVapEval=CQRqdTotal;
      F.m_pFNd->MacroMdlEvaluate(MME_Flash2QvSens);
      dPRqd=F.m_dPEval;
      F.m_dQVapRqd = F.m_dQVapEval;
      F.m_dQFrac = 1.0;
      F.m_dQRqdTotal = CQRqdTotal;
#if DOPBSMOOTHING  
      F.m_dQCool = QCool;
#endif
      #if dbgModels   
      if (dbgFlashTrain())
        dbgpln("FlshTrn %10.6f  %10.6f  %10.6f %s", F.m_dQFrac, F.m_dQVapRqd, F.m_dQCool, Tag());
      #endif
      }
    }

  //SW.Stop();
  //double xxx=SW.Secs()*1000;

//  double FracTot=0.0;
//  for (i=0; i<Flsh.GetSize(); i++)
//    {
//    CFT_Flash &F=*Flsh[i];
//    F.dQVapRqd = F.dQVapEval;
//    F.dQFrac = F.dQVapEval / GTZ(CQRqdTotal);
//    FracTot+=F.dQFrac;
//    }
//  for (i=0; i<Flsh.GetSize(); i++)
//    {
//    CFT_Flash &F=*Flsh[i];
//    F.dQRqdTotal = CQRqdTotal;
//    F.dQFrac /= GTZ(FracTot);
//    F.m_dQCool = QCool;
//dbgpln("FlshTrn %10.6f  %10.6f  %10.6f %s", F.dQFrac, F.dQVapRqd, F.dQCool, sTag());
//    }
  
  for (i=0; i<Cond.GetSize(); i++)
    Cond[i]->m_Iter++;
  };

// --------------------------------------------------------------------------

TearVarBlk* CMMFlashTrain::CreateVarBlk(char * Tag, pTaggedObject pAttach)
  {
  return new FlshTrnTearVarBlk(Tag, pAttach);
  };

//--------------------------------------------------------------------------

byte CMMFlashTrain::TearVarType() 
  { 
  return (Flsh.GetSize()>0 && !MassSmoothing()) ? TVT_Tear|TVT_Test : TVT_None; 
  };

//flag CMMFlashTrain::TearAdvanceReqd() 
//  { 
//  return Flsh.GetSize()>0 && !MassSmoothing(); 
//  };
//
////--------------------------------------------------------------------------
//
//flag CMMFlashTrain::TearCnvTestReqd() 
//  { 
//  return Flsh.GetSize()>0 && !MassSmoothing(); 
//  };

//--------------------------------------------------------------------------

void CMMFlashTrain::TearInitialiseOutputs(double EstimatePortion) 
  {
  };

//--------------------------------------------------------------------------

void CMMFlashTrain::TearInputs2Outputs() 
  {
  if (!MassSmoothing())
    for (int i=0; i<Flsh.GetSize(); i++)
      Flsh[i]->m_dPRqd=Flsh[i]->m_dPRqdCalc;
  };

//--------------------------------------------------------------------------
    
char * CMMFlashTrain::TearGetTag(Strng & RootTag) 
  { 
  RootTag.Set("%s#", Tag());
  return RootTag(); 
  };

//--------------------------------------------------------------------------

void CMMFlashTrain::TearGetInputs(TearVarArray & TV) 
  { 
  if (!MassSmoothing())
    TV[0].Y[0]=Flsh[0]->m_dPRqdCalc;
  };

//--------------------------------------------------------------------------

void CMMFlashTrain::TearGetOutputs(TearVarArray & TV) 
  { 
  if (!MassSmoothing())
    TV[0].X[0]=Flsh[0]->m_dPRqd;
  };

//--------------------------------------------------------------------------

void CMMFlashTrain::TearSetOutputs(TearVarArray & TV) 
  { 
  if (!MassSmoothing())
    for (int i=0; i<Flsh.GetSize(); i++)
      Flsh[i]->m_dPRqd=TV[0].X[0];
  };

//==========================================================================
//
//
//
//==========================================================================
