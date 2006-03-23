//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#define  __SQSSA_CPP
#include "sc_defs.h"
#include "sp_db.h"
#include "mdlcfg.h"
#include "sqssa.h"
#include "executiv.h"
#include "dbgmngr.h"
#include "sqszdst1.h"//"alspmdlb.h"

//#include "optoff.h"

#pragma chFIXIT(Manual size distribution)
//set this to 1 to allow user to enter manual size distribution sieve size ranges
#define UseManualSizeDefn 0

#define  dbgSzDist   1

#if dbgSzDist
static CDbgMngr dbgSzDistSplits("CSzSSA", "Splits");
#endif

XID xidSSAPartSAM         = Qual1XID(1);
XID xidSSAPartSAL         = Qual1XID(2);
XID xidSSAPartDiam        = Qual1XID(3);
XID xidSSAPartNoperSec    = Qual1XID(4);
XID xidSSAPartNoperVol    = Qual1XID(5);
XID xidSSAPartSolidsQm    = Qual1XID(6);
XID xidSSAAllowSet        = Qual1XID(7);
//XID xidSSAPartBndSoda     = Qual1XID(7);
//XID xidSSAPartBndSodaOrg  = Qual1XID(8);

//===========================================================================
//
//
//
//===========================================================================

#define MoveDistList 0

// ==========================================================================
//
//
//
// ==========================================================================
static int SSA_ID_Cnt = 0;

IMPLEMENT_SPARES(CSzSSA, 400);
IMPLEMENT_TAGOBJ(CSzSSA, "SpQuality", "SzSSA", "", "", "SSA", TOC_ALL|TOC_GRP_ALUMINA|TOC_ALUMINA, 
                 "SpecificSurfaceArea",
                 "Specie Quality to describe the Specific Surface Area");
CSzSSA::CSzSSA(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  SpQuality(pClass_, Tag_, pAttach, eAttach)//,
  {
  m_iMethod    = SSA_Independent;
  m_dSAM       = 0.07; // Surface Area 
  //m_bSAMCalcd  = false;
  m_iSolids    = -1;
  m_iInclusion = -1;
  m_bHasPSD    = false;
  //m_bAllowSet  = false;

  //if (m_iSolids<0)
  //  m_iSolids=SDB.Find("Al2O3.3H2O(s)");
  //if (m_iInclusion<0)
  //  m_iInclusion=SDB.Find("Al2O3(s)");
ID=SSA_ID_Cnt++; //temp for debug
//dbgpln("SSA:%4d Construct", ID);
  };

// --------------------------------------------------------------------------

CSzSSA::~CSzSSA()
  {
//dbgpln("SSA:%4d(%d) Delete", ID, m_bAllowSet);
  };

// --------------------------------------------------------------------------

inline CSD_Distribution* CSzSSA::GetDist()
  {
  switch (m_iMethod)
    {
    case SSA_Independent:
      break;
    case SSA_FromPSD:
      {
      SQSzDist1 *pSz=SQSzDist1::Ptr(pModel, false);
      if (pSz)
        return pSz->Distributions[pSz->iDistUsed];
      }
    }
  return NULL;
  }

// --------------------------------------------------------------------------

//Does Nothing: 
//void CSzSSA::BuildDataDefn_Lcl(DataDefnBlk & DDB)
//  {
//  }

// --------------------------------------------------------------------------

static DDBValueLst DDBMeth[] =
  {
    {SSA_Independent , "Independent"},
    {SSA_FromPSD     , "FromPSD"    },
    {NULL,NULL}
  };

void CSzSSA::BuildDataDefn(DataDefnBlk & DDB)
  {
//  DDB.CheckBox("SetSAM",        "",     DC_,          "",       &m_bSetSAM,         this,  m_bSAMCalcd ? 0 : isParm);
  if (DDB.BeginStruct(this, "SSA", 0))
    {
    DDB.Byte    ("",              "Method",     DC_,          "",       &m_iMethod,            this, 0/*isParm*/, DDBMeth);
    DDB.Visibility(SHM_All ,m_iMethod==SSA_Independent);
    if (!DDB.ForFileSnpScn())
      DDB.CheckBoxBtn("SetData",  "",           DC_,          "",       xidSSAAllowSet,        this, isParm);
    DDB.Long    ("",              "Solids",     DC_,          "",       &m_iSolids,            this, (m_bHasPSD || !DataSettable()) ? 0 : isParm, SDB.DDBSolSpListDash());
    DDB.Visibility();
    DDB.Double  ("SeedSurfAreaM", "SAM",        DC_SurfAreaM, "m^2/g",  xidSSAPartSAM,         this, (m_bHasPSD || !DataSettable()) ? 0 : isParm);
    DDB.Double  ("SeedSurfAreaL", "SAL",        DC_SurfAreaL, "m^2/L",  xidSSAPartSAL,         this, noFile);
    DDB.Double  ("#/s",           "",           DC_,          "",       xidSSAPartNoperSec,    this, noFile|InitHidden);
    DDB.Double  ("#/L",           "",           DC_,          "",       xidSSAPartNoperVol,    this, noFile|InitHidden);
    DDB.Double  ("PartDiam",      "D",          DC_L,         "um",     xidSSAPartDiam,        this, ((m_bHasPSD || !DataSettable()) ? 0 : isParm)|noFile);
    DDB.Double  ("SolidsQm",      "",           DC_Qm,        "kg/s",   xidSSAPartSolidsQm,    this, noFile|InitHidden);
    }
  DDB.EndStruct();
  }

// --------------------------------------------------------------------------

flag CSzSSA::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidSSAAllowSet:
      if (DCB.rB)
        SetSQCreator(*DCB.rB);
      DCB.B=DataSettable();
      return true;
    case xidSSAPartSAM:
      if (DCB.rD)
        m_dSAM=*DCB.rD;
      DCB.D=SpecificSurfaceAreaMass();
      return true;
    case xidSSAPartSAL:
      DCB.D=SpecificSurfaceAreaVol();
      return true;
    case xidSSAPartDiam:
      if (DCB.rD)
        SetSAMFromPartDiam(*DCB.rD);
      DCB.D=PartDiamFromSAM();
      return true;
    case xidSSAPartNoperSec:
      DCB.D=PartNumPerSec();
      return true;
    case xidSSAPartNoperVol:
      DCB.D=PartNumPerLiter();
      return true;
    case xidSSAPartSolidsQm:
      DCB.D=SolidsMassFlow();
      return true;
    //case xidSSAPartBndSoda:
    //  DCB.D=BoundSodaFrac();
    //  return true;
    //case xidSSAPartBndSodaOrg:
    //  DCB.D=BoundSodaOrgFrac();
    //  return true;
    }

  return SpQuality::DataXchg(DCB);
  };

// --------------------------------------------------------------------------

flag CSzSSA::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=true;

  SQSzDist1 *pSz=SQSzDist1::Ptr(pModel, false);
  m_iMethod=pSz ? SSA_FromPSD : SSA_Independent;
  //if (m_iMethod==SSA_FromPSD)
  //  {
  //  m_bAllowSet=false;
  //  }

  CSD_Distribution *pDst=GetDist();
  if (pDst)
    {
    m_bHasPSD=pDst!=NULL;
    m_iSolids=pDst->PriSzId(0); 
    }
  else
    m_bHasPSD=false;
 
  SetCI(1, DataSettable() && m_iSolids<0);

  if (OK)
    Equilibrate();

  return OK;
  }

// --------------------------------------------------------------------------

void CSzSSA::ExchangeSpecies(SpQuality * pQual2)
  {
  OnDataChange();
  CSzSSA *p=(CSzSSA*)pQual2;
  m_dSAM=p->m_dSAM; // will be ignored is PSD is present
  };

// --------------------------------------------------------------------------

void CSzSSA::Equilibrate()
  {
  OnDataChange();
  };

// --------------------------------------------------------------------------

void CSzSSA::ZeroMass()
  {
//dbgpln("SSA:%4d(%d) ZeroMass", ID, m_bAllowSet);
  OnDataChange();
  m_dSAM=0; // will be ignored is PSD is present
  }

// --------------------------------------------------------------------------

void CSzSSA::ZeroDeriv()
  {
  }

// --------------------------------------------------------------------------

void CSzSSA::ScaleMass(PhMask PhaseM__, double Mult)
  {
  }

// --------------------------------------------------------------------------

void CSzSSA::ScaleMass(CIArray & SpIds, double Mult)
  {
  }

// --------------------------------------------------------------------------

void CSzSSA::SetMassF(CSysVector &M1, SpQuality * pQual2)
  {
//dbgpln("SSA:%4d(%d) SetMassF from %d", ID, m_bAllowSet, ((CSzSSA*)pQual2)->ID);
  OnDataChange();
  CSzSSA *p=(CSzSSA*)pQual2;
  if (p->m_iMethod==SSA_FromPSD)
    m_iMethod=p->m_iMethod;
  if (!DataSettable())
    {
    m_dSAM=p->m_dSAM;
    m_iSolids=p->m_iSolids;
    }
  else if (m_iSolids<0)
    m_iSolids=p->m_iSolids;
  };

// --------------------------------------------------------------------------

void CSzSSA::AddMassF(CSysVector &M1, SpQuality * pQual2, CSysVector &M2)
  {
  OnDataChange();
  CSzSSA *p=(CSzSSA*)pQual2;

  if (p)
    {
    if (p->m_iMethod==SSA_FromPSD)
      m_iMethod=p->m_iMethod;
    if (m_iSolids<0)
      m_iSolids=p->m_iSolids;

    double N1=PartNumPerSec();
    double N2=p->PartNumPerSec();
    double M1=SolidsMassFlow();
    double M2=p->SolidsMassFlow();

    SetSAMFromFlow((M1+M2), N1+N2);
    };
  }

// --------------------------------------------------------------------------

void CSzSSA::SubMassF(CSysVector &M1, SpQuality * pQual2, CSysVector &M2)
  {
  OnDataChange();
  };

// --------------------------------------------------------------------------

void CSzSSA::Copy(pSpModel pMdl2, SpQuality * pQual2)
  {
//dbgpln("SSA:%4d(%d) Copy from %d", ID, m_bAllowSet, ((CSzSSA*)pQual2)->ID);
  OnDataChange();
  CSzSSA *p=(CSzSSA*)pQual2;
  m_iMethod=p->m_iMethod;
  if (!DataSettable())
    {
    m_dSAM=p->m_dSAM;  // will be ignored is PSD is present
    m_iSolids=p->m_iSolids;
    }
  else if (m_iSolids<0)
    m_iSolids=p->m_iSolids;
  };

// --------------------------------------------------------------------------

void CSzSSA::AddDeriv(pSpModel pMdl2, SpQuality * pQual2, double Sgn_)
  {
  // There should not be any code here 
  // Qualities are added in Discrete
  };

// --------------------------------------------------------------------------

void CSzSSA::AddDiscrete(pSpModel pMdl2, SpQuality * pQual2, double Sgn_)
  {
  OnDataChange();
  };

// --------------------------------------------------------------------------

void CSzSSA::ODEOperate(CODEDataBlock & ODB)
  {
  switch (ODB.m_Cmd)
    {
    case eStateAdvance:
    case eStateConverge:
    case eStateLoad:
    case eStateDiskLoad:
      OnDataChange();
      break;
    case eStateFixDV:
    case eStateTest:
    case eStateSave:
    case eStateDiskSave:
      break;
    }
  };

// --------------------------------------------------------------------------

double CSzSSA::PartDiamFromSAM()
  { 
  double D = 3.0/GTZ(500.0*SolidsDens()*SpecificSurfaceAreaMass());
  return Range(1.0e-9, D, 5.0e-3); 
  }

// --------------------------------------------------------------------------

double CSzSSA::PartNumPerSec()   
  { 
  double D=PartDiamFromSAM(); 
  return 6.0*SolidsMassFlow()/GTZ(PI*SolidsDens()*D*D*D); 
  }

// --------------------------------------------------------------------------

double CSzSSA::PartNumPerLiter()  
  { 
  double D=PartDiamFromSAM(); 
  return 6.0*0.001*SolidsMassFlow()/GTZ(PI*SolidsDens()*D*D*D*LiquorVolume()); 
  }

// --------------------------------------------------------------------------

double CSzSSA::SpecificSurfaceAreaVol()            
  { 
  if (m_iMethod==SSA_FromPSD)
    {
    CSD_Distribution* pSz=GetDist();
    if (pSz)
      return pSz->SpecificSurfaceAreaV(pModel->Temp(), pModel->Press(), &pModel->m_Ovr, pModel->SVData(), pModel->Volume(som_Liq));
    }
  double D=PartDiamFromSAM(); 
  return PartNumPerLiter()*PI*D*D; 
  }

//---------------------------------------------------------------------------

double CSzSSA::SpecificSurfaceAreaMass()
  {
  if (m_iMethod==SSA_FromPSD)
    {
    CSD_Distribution* pSz=GetDist();
    if (pSz)
      return pSz->SpecificSurfaceAreaM(pModel->Temp(), pModel->Press(), &pModel->m_Ovr, pModel->SVData());
    }
  return m_dSAM;
  };

// --------------------------------------------------------------------------

void CSzSSA::SetSAM(double SAM, bool Calcd)
  {
  m_dSAM=SAM;
  }

// --------------------------------------------------------------------------

void CSzSSA::SetSAMFromFlow(double THAFlow, double NoPerSec)
  {
  if (THAFlow>1.0e-6)
    {
    double Dens=SolidsDens();
    double D=Pow(6.*THAFlow/GTZ(PI*Dens*NoPerSec), 1./3.);
    m_dSAM=3.0/GTZ(500.*Dens*D);
    double xD=PartDiamFromSAM();
    }
  else
    m_dSAM=0.0;
  }

// --------------------------------------------------------------------------

void CSzSSA::SetSAMFromPartDiam(double D)
  {
//dbgpln("SSA:%4d(%d) SetSAMFromPartDiam(%g)", ID, m_bAllowSet, D);
  if (D>1.0e-12)
    {
    double Dens=SolidsDens();
    m_dSAM=3.0/GTZ(500.*Dens*D);
    }
  else
    m_dSAM=0.0;
  };

// --------------------------------------------------------------------------

void CSzSSA::SetSAMfromPSD(long npts, double x[], double psd[])
  {
  double Dens=SolidsDens();
  double TotMass=0.0;
  double TotArea=0.0;

  for (int i=0; i<npts; i++)
    {
    double D=x[i]*1.0e-6;
    double SAMAtD=3.0/GTZ(500.*Dens*D);
    TotMass+=psd[i];
    TotArea+=SAMAtD*psd[i];
    }
  m_dSAM=TotArea/GTZ(TotMass);
  }

// --------------------------------------------------------------------------

void CSzSSA::TearGetInputs(TearVarArray & TV, int n)
  {
//  double TotMass=pModel->Mass();
//  for (int d=0; d<SD_Defn.NDistributions(); d++)
//    if (DistExists(d))
//      {
//      CSD_Distribution &D=Dist(d);
//      for (int id=0; id<D.NPriIds(); id++)
//        {
//        double MF=pModel->SpMass(D.SzId(id, 0))/GTZ(TotMass);
//        flag MFOK=MF>1.0e-8;
//        for (int i=0; i<D.NIntervals(); i++)
//          {
//          TV[n].Y[0]=D.PriSp[id]->FracPass[i];
////          DoBreak();
//          TV[n].bTestIsValid=MFOK;
//          TV[n].bHoldOutput=false;//!MFOK;
//          n++;
//          }
//        }
//      }
  };

//---------------------------------------------------------------------------

void CSzSSA::TearGetOutputs(TearVarArray & TV, int n)
  {
  //for (int d=0; d<SD_Defn.NDistributions(); d++)
  //  if (DistExists(d))
  //    {
  //    CSD_Distribution &D=Dist(d);
  //    for (int id=0; id<D.NPriIds(); id++)
  //      for (int i=0; i<D.NIntervals(); i++)
  //        TV[n++].X[0]=D.PriSp[id]->FracPass[i];
  //    }
  };

//---------------------------------------------------------------------------

void CSzSSA::TearSetOutputs(TearVarArray & TV, int n)
  {
  //for (int d=0; d<SD_Defn.NDistributions(); d++)
  //  if (DistExists(d))
  //    {
  //    CSD_Distribution &D=Dist(d);
  //    for (int id=0; id<D.NPriIds(); id++)
  //      {
  //      for (int i=0; i<D.NIntervals(); i++)
  //        D.PriSp[id]->FracPass[i]=TV[n++].X[0];
  //        //if (!TV[n].bHoldOutput)
  //        //  {
  //        //  D.PriSp[id]->FracPass[i]=TV[n].X[0];
  //        //  n++;
  //        //  }
  //      D.PriSp[id]->FracPass.Normalise();
  //      }
  //    }
  };

//---------------------------------------------------------------------------

void CSzSSA::Dump()
  {
  //for (int d=0; d<SD_Defn.NDistributions(); d++)
  //  if (DistExists(d))
  //    {
  //    dbgpln("Dist%02i", d);
  //    CSD_Distribution &D=Dist(d);
  //    for (int id=0; id<D.NPriIds(); id++)
  //      {
  //      dbgp("  ");
  //      for (int i=0; i<D.NIntervals(); i++)
  //        dbgp(" %12.8f", D.PriSp[id]->FracPass[i]);
  //      dbgpln("");
  //      }
  //    }
  };

//---------------------------------------------------------------------------

double CSzSSA::SolidsMassFlow()
  { 
  CSD_Distribution* pSz=GetDist();
  if (pSz)
    return pSz->SolidsMass(pModel->Temp(), pModel->Press(), &pModel->m_Ovr, pModel->SVData());
  return m_iSolids>=0 ?  pModel->VMass[m_iSolids] : 0;
  };

//---------------------------------------------------------------------------

double CSzSSA::SolidsDens()
  {
  //CSD_Distribution* pSz=GetDist();
  //if (pSz)
  //  return pSz->SolidsDensity(pModel->Temp(), pModel->Press(), &pModel->m_Ovr, pModel->SVData());
  //  //return 2420.0;
  return m_iSolids>=0 ?  SDB[m_iSolids].Density(pModel->Fidelity(), pModel->Temp(), pModel->Press(), NULL, NULL) : 1000;
  };

//---------------------------------------------------------------------------

double CSzSSA::LiquorVolume()
  { 
  return pModel->Volume(som_Liq);
  };

//---------------------------------------------------------------------------

flag CSzSSA::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tSSA Solids Specie not selected"; return 1;
    default:
      return SpQuality::CIStrng(No, pS);
    }
  };

// ==========================================================================
//
//
//
// ==========================================================================

