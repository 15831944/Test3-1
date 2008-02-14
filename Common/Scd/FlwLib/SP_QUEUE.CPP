//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __SP_QUEUE_CPP
#include "sp_queue.h"
#include "flwnode.h"

#define dbgSpQueue            WITHDEBUG
#if dbgSpQueue
#include "dbgmngr.h"
static CDbgMngr dbgDbgBrk         ("SpQueue", "DbgBrk");
static CDbgMngr dbgLoad           ("SpQueue", "Load");
static CDbgMngr dbgUnLoad         ("SpQueue", "UnLoad");
static CDbgMngr dbgBreak          ("SpQueue", "Break");
static CDbgMngr dbgSpill          ("SpQueue", "Spill");
static CDbgMngr dbgProfileBefore  ("SpQueue", "ProfileBefore");
static CDbgMngr dbgProfileAfter   ("SpQueue", "ProfileAfter");
static CDbgMngr dbgProfileInter   ("SpQueue", "ProfileInter");
static CDbgMngr dbgProfileFeed    ("SpQueue", "ProfileFeed");
static CDbgMngr dbgAdvance        ("SpQueue", "Advance");
static CDbgMngr dbgAdvSects       ("SpQueue", "AdvanceSections");
static CDbgMngr dbgSample         ("SpQueue", "Sample");
#endif

const int TurnDownLimit=100;

// ===========================================================================
//
//                                Specie Section
//
// ===========================================================================

UINT SpQSection::m_IDCurrent=0;

SpQSection::SpQSection()
  {
  m_ID=0;
  m_bTentative=false;
  m_dLength=0.0;
  m_dPosition=0.0;
  m_dDeltaPosS=0.0;
  m_dDeltaLen=0.0;
  m_dPressS=Std_P;
  m_dPressE=Std_P;
  m_dVelS=0;
  m_dVelE=0;
  m_dMassScale=1;
  m_ListPos=NULL;
  //pSpill=NULL;
  m_pMdl=(SpModel *)SpModelClass.ConstructGroup(SpContainer::DefaultModel(), NULL, SpModelDefTag, NULL, TOA_Free);
  };

// --------------------------------------------------------------------------

SpQSection::~SpQSection()
  {
  m_dLength=0.0;
  m_dPosition=0.0;
  m_pMdl->Destroy();
  };

// --------------------------------------------------------------------------

static byte SpQ_LenPos=1;
static byte SpQ_Model=2;

flag SpQSection::GetOtherData(FilingControlBlock &FCB)
  {
/*
    BOOL ReadFile(LPVOID lpBuffer,// address of buffer that receives data
                  DWORD nNumberOfBytesToRead,// number of bytes to read
                  LPDWORD lpNumberOfBytesRead);// address of number of bytes read
    BOOL WriteFile(LPCVOID lpBuffer,// address of data to write to file
                   DWORD nNumberOfBytesToWrite,// number of bytes to write
                   LPDWORD lpNumberOfBytesWritten);// address of number of bytes written
    DWORD SetFilePointer(LONG lDistanceToMove,// number of bytes to move file pointer
                         DWORD dwMoveMethod);// how to move
*/
  DWORD nBytes;
  DWORD CurPos=FCB.SetFilePointer(0, FILE_CURRENT);

  FCB.WriteFile(&SpQ_LenPos, sizeof(SpQ_LenPos), &nBytes);
  FCB.WriteFile(&m_dLength, sizeof(m_dLength), &nBytes);
  FCB.WriteFile(&m_dPosition, sizeof(m_dPosition), &nBytes);


  m_pMdl->GetOtherData(FCB);

  //CXM_ObjectData Data(0);
  //CPkDataItem * pPItem=NULL;
  //TABOptions Opts= (FCB.Type()==TOF_DBase) ? TABOpt_ForFile|TABOpt_AllInfo : TABOpt_ForSnapShot;
  //int l=m_pMdl->GetDefinedData(Data.List, Opts, pPItem);
  //Data.SetSize();

  return True;
  }

// --------------------------------------------------------------------------

flag SpQSection::PutOtherData(FilingControlBlock &FCB)
  {

  DWORD nBytes;
  byte What;

  FCB.ReadFile(&What, sizeof(What), &nBytes);
  ASSERT(What==SpQ_LenPos);
  FCB.ReadFile(&m_dLength, sizeof(m_dLength), &nBytes);
  FCB.ReadFile(&m_dPosition, sizeof(m_dPosition), &nBytes);

  m_pMdl->PutOtherData(FCB);

  return True;
  };

// --------------------------------------------------------------------------

void SpQSection::SetMakeup(SpQueue * pQ, double Frac, double QmRqd, SpConduit * pProd)
  {
  double MassInSct = m_dMassScale*m_pMdl->Mass(som_ALL);
  double AmtToGo   = Frac * MassInSct;
  double RqdQmToGo = Frac * MassInSct / pQ->m_dTimeSlice;
  double MaxQmToGo = QmRqd;
  double QmToGo    = Min(MaxQmToGo, RqdQmToGo);

  pProd->Model()->AddMassM(m_pMdl, som_ALL, QmToGo);
  m_dMassScale*=GEZ(1.0-Frac);

  #if dbgSpQueue
  if (dbgUnLoad() && (!dbgDbgBrk() || dbgDbgBrk() && pQ->DoDbgBrk()))
    dbgpln("  StMkUp%5i) %6.2f->%6.2f   L:%6.2f   %%:%6.2f   QmToGo:%8.4f   Qm:%8.4f                  T:%6.2f  P:%6.2f",
            m_ID, Start(), End(), Length(), Frac*100, QmToGo, pProd->QMass(som_ALL), K_2_C(m_pMdl->Temp()), m_pMdl->Press());
  #endif
  };

// --------------------------------------------------------------------------

void SpQSection::Load(SpQueue * pQ, double Frac, SpConduit * pFeed, CDirectFlwIO *pSpill, CDirectFlwIO *pVent)
  {
  double MaxAmtInSct = fabs(Frac*pQ->m_dMaxSpillLoading*pQ->m_dTimeSlice*pQ->m_dSpeed);
  double AmtInSct    = m_pMdl->Mass(som_SL);
  double MaxAmtToAdd = GTZ(MaxAmtInSct-AmtInSct);
  double RqdAmtToAdd = Frac * pFeed->QMass(som_SL) * pQ->m_dTimeSlice;
  double AmtToAdd    = Min(RqdAmtToAdd, MaxAmtToAdd);
  double AmtToSpill  = RqdAmtToAdd-AmtToAdd;
  double AmtToVent   = 1.0/*Frac*/ * pFeed->QMass(som_Gas) * pQ->m_dTimeSlice;
  double QmToAdd     = AmtToAdd/pQ->m_dTimeSlice;
  double QmToSpill   = AmtToSpill/pQ->m_dTimeSlice;
  double QmToVent    = AmtToVent/pQ->m_dTimeSlice;

  m_pMdl->AddMassM(pFeed->Model(), som_SL, AmtToAdd);
  if (AmtToSpill>1.0e-6)
    {
    //pSpill->AddMass2Target(*pFeed, QmToSpill, pQ->m_dTimeSlice);
    if (pQ->m_bSumLosses)
      {
      pSpill->Cd.QAddM(*pFeed, som_SL, QmToSpill);
      pQ->m_dTotSpiltM+=AmtToSpill;
      pQ->m_dTotSpiltHf+=AmtToSpill*pFeed->msHf(som_SL);
      pQ->m_dTotSpiltHs+=AmtToSpill*pFeed->msHs(som_SL);
      pQ->m_dTotSpiltHz+=AmtToSpill*pFeed->msHz(som_SL);
      }
    }
  if (AmtToVent>1.0e-6)
    {
    //pSpill->AddMass2Target(*pFeed, QmToSpill, pQ->m_dTimeSlice);
    if (pQ->m_bSumLosses)
      {
      pVent->Cd.QAddM(*pFeed, som_Gas, QmToVent);
      pQ->m_dTotVentedM+=AmtToVent;
      pQ->m_dTotVentedHf+=AmtToVent*pFeed->msHf(som_Gas);
      pQ->m_dTotVentedHs+=AmtToVent*pFeed->msHs(som_Gas);
      pQ->m_dTotVentedHz+=AmtToVent*pFeed->msHz(som_Gas);
      }
    }

#if dbgSpQueue
  if (dbgLoad() && (!dbgDbgBrk() || dbgDbgBrk() && pQ->DoDbgBrk()))
    dbgpln("  Load  %5i) %6.2f->%6.2f   L:%6.2f   %%:%6.2f  Amt2Add:%8.4f  M/m:%8.4f  Spill/m:%6.2f  T:%6.2f  P:%6.2f",
        m_ID, Start(), End(), Length(), Frac, AmtToAdd, m_pMdl->Mass(som_ALL)/(pQ->m_dTimeSlice*pQ->m_dSpeed),
        AmtToSpill/(pQ->m_dTimeSlice*pQ->m_dSpeed), K_2_C(m_pMdl->Temp()), m_pMdl->Press());
  #endif
  };

// --------------------------------------------------------------------------

void SpQSection::UnLoad(SpQueue * pQ, double Frac, double QmRqd, SpConduit * pProd, CDirectFlwIO *pSpill, CDirectFlwIO *pVent)
  {
  double MassInSct = m_pMdl->Mass(som_ALL);
  double AmtToGo   = Frac * MassInSct;
  double RqdQmToGo = Frac * MassInSct / pQ->m_dTimeSlice;
  double MaxQmToGo = QmRqd;
  double QmToGo    = Min(MaxQmToGo, RqdQmToGo);
  double QmToSpill = GTZ(RqdQmToGo-QmToGo);

  pProd->Model()->AddMassM(m_pMdl, som_ALL, QmToGo);
  if (QmToSpill>1.0e-6)
    {
    //TODO _asm int 3;
    //pSpill->AddMass2Target(*m_pMdl, QmToSpill, pQ->m_dTimeSlice);
    pSpill->Cd.QAddM(*m_pMdl, som_ALL, QmToSpill);
    pQ->m_dTotSpiltM+=QmToSpill*pQ->m_dTimeSlice;
    pQ->m_dTotSpiltHf+=QmToSpill*pQ->m_dTimeSlice*m_pMdl->msHf();
    pQ->m_dTotSpiltHs+=QmToSpill*pQ->m_dTimeSlice*m_pMdl->msHs();
    pQ->m_dTotSpiltHz+=QmToSpill*pQ->m_dTimeSlice*m_pMdl->msHz();
    }
  m_pMdl->ScaleMass(som_ALL, GEZ(1.0-Frac));

  #if dbgSpQueue
  if (dbgUnLoad() && (!dbgDbgBrk() || dbgDbgBrk() && pQ->DoDbgBrk()))
    dbgpln("  UnLoad%5i) %6.2f->%6.2f   L:%6.2f   %%:%6.2f   QmToGo:%8.4f   Qm:%8.4f                  T:%6.2f  P:%6.2f",
            m_ID, Start(), End(), Length(), Frac*100, QmToGo, pProd->QMass(som_ALL), K_2_C(m_pMdl->Temp()), m_pMdl->Press());
  #endif
  };

// --------------------------------------------------------------------------

void SpQSection::CopyMass(SpQSection * pSct, double Scl)
  {
  m_pMdl->SetMassF(pSct->m_pMdl, som_ALL, Scl, pSct->m_pMdl->Press());
  };

// --------------------------------------------------------------------------

void SpQSection::Spill(SpQueue * pQ, double Frac, CDirectFlwIO *pSpill, CDirectFlwIO *pVent)
  {
  double MassInSct = m_pMdl->Mass(som_ALL);
  //double AmtToGo   = Frac * MassInSct;
  double QmToSpill = Frac * MassInSct / pQ->m_dTimeSlice;
  if (QmToSpill>1.0e-6)
    {
    //TODO _asm int 3;
    //pSpill->AddMass2Target(*m_pMdl, QmToSpill, pQ->m_dTimeSlice);
    pSpill->Cd.QAddM(*m_pMdl, som_ALL, QmToSpill);
    pQ->m_dTotSpiltM+=QmToSpill*pQ->m_dTimeSlice;
    pQ->m_dTotSpiltHf+=QmToSpill*pQ->m_dTimeSlice*m_pMdl->msHf();
    pQ->m_dTotSpiltHs+=QmToSpill*pQ->m_dTimeSlice*m_pMdl->msHs();
    pQ->m_dTotSpiltHz+=QmToSpill*pQ->m_dTimeSlice*m_pMdl->msHz();
    }
  // Do this After Spilling
  m_pMdl->ScaleMass(som_ALL, GEZ(1.0-Frac));

  #if dbgSpQueue
  if (dbgSpill() && (!dbgDbgBrk() || dbgDbgBrk() && pQ->DoDbgBrk()))
    dbgpln("  Spill %5i) %6.2f->%6.2f   L:%6.2f   %%:%6.2f   Qm2Spl:%8.4f   TotSpilt:%8.4f  t:%6.2f",
            m_ID, Start(), End(), Length(), Frac*100, QmToSpill, pQ->m_dTotSpiltM, K_2_C(m_pMdl->Temp()));
  #endif
  };

// --------------------------------------------------------------------------

void SpQSection::Spill(CSpPlugFlow* pQ, double Frac)
  {
  double MassInSct = m_pMdl->Mass(som_ALL);
  double QmToSpill = Frac * MassInSct / pQ->m_dTimeSlice;
  m_pMdl->ScaleMass(som_ALL, GEZ(1.0-Frac));

  #if dbgSpQueue
  if (dbgSpill() && (!dbgDbgBrk() || dbgDbgBrk() && pQ->DoDbgBrk()))
    dbgpln("  Spill %5i) %6.2f->%6.2f   L:%6.2f   %%:%6.2f   Qm2Spl:%8.4f                             T:%6.2f",
            m_ID, Start(), End(), Length(), Frac, QmToSpill, K_2_C(m_pMdl->Temp()));
  #endif
  };

// --------------------------------------------------------------------------

void SpQSection::Load(CSpPlugFlow * pPF, double Frac, SpConduit * pFeed)
  {
  double RqdAmtToAdd = Frac * pFeed->QMass(som_ALL) * pPF->m_dTimeSlice;
  double AmtToAdd    = /*Min(*/RqdAmtToAdd/*, MaxAmtToAdd)*/;
  double QmToAdd     = AmtToAdd/pPF->m_dTimeSlice;

  m_pMdl->AddMassM(pFeed->Model(), som_ALL, AmtToAdd);
  m_pMdl->SetTempPress(pFeed->Temp(), pFeed->Press());
  m_dPressS=pFeed->Press();
  m_dPressE=pFeed->Press();

  #if dbgSpQueue
  if (dbgLoad() && (!dbgDbgBrk() || dbgDbgBrk() && pPF->DoDbgBrk()))
    dbgpln("  Load  %5i) %6.2f->%6.2f   L:%6.2f   %%:%6.2f  Amt2Add:%8.4f                             T:%6.2f  P:%6.2f  M:%8.4f",
      m_ID, Start(), End(), Length(), Frac, AmtToAdd, K_2_C(m_pMdl->Temp()), m_pMdl->Press(), m_pMdl->Mass());
  #endif
  };

// --------------------------------------------------------------------------

void SpQSection::UnLoad(CSpPlugFlow * pPF, double Frac, double QmRqd, SpConduit * pProd)
  {
  double MassInSct = m_pMdl->Mass(som_ALL);
  double AmtToGo   = Frac * MassInSct;
  double RqdQmToGo = Frac * MassInSct / pPF->m_dTimeSlice;
  double MaxQmToGo = 1e100;//QmRqd;
  double QmToGo    = Min(MaxQmToGo, RqdQmToGo);
  double QmToSpill = GTZ(RqdQmToGo-QmToGo);

  pProd->Model()->AddMassM(m_pMdl, som_ALL, QmToGo);
  pProd->SetTempPress(m_pMdl->Temp(), m_pMdl->Press());
  if (QmToSpill>1.0e-6)
    {
    //pSpill->AddMass2Target(*m_pMdl, QmToSpill, pPF->m_dTimeSlice);
    //pPF->m_dTotSpilt+=QmToSpill*pPF->m_dTimeSlice;
    }
  m_pMdl->ScaleMass(som_ALL, GEZ(1.0-Frac));

  #if dbgSpQueue
  if (dbgUnLoad() && (!dbgDbgBrk() || dbgDbgBrk() && pPF->DoDbgBrk()))
    dbgpln("  Unload%5i) %6.2f->%6.2f   L:%6.2f   %%:%6.2f   QmToGo:%8.4f   Qm:%8.4f                  T:%6.2f",
            m_ID, Start(), End(), Length(), Frac, QmToGo, pProd->QMass(som_ALL), K_2_C(m_pMdl->Temp()));
  #endif
  };

// --------------------------------------------------------------------------

void SpQSection::Fill(CSpPlugFlow * pPF, double Frac, SpConduit * pFeed, double PressS, double PressE, double VelS, double VelE)
  {
  double Dens = pFeed->Rho();
  double Vol=pPF->Volume()*Length()/pPF->Length();
  double RqdAmtToAdd = Frac * Dens*Vol;
  double AmtToAdd    = /*Min(*/RqdAmtToAdd/*, MaxAmtToAdd)*/;

  m_dPressS=PressS;
  m_dPressE=PressE;
  m_dVelS=VelS;
  m_dVelE=VelE;

  m_pMdl->SetMassM(pFeed->Model(), som_ALL, AmtToAdd, 0.5*(PressS+PressE));

  #if dbgSpQueue
  if (dbgLoad() && (!dbgDbgBrk() || dbgDbgBrk() && pPF->DoDbgBrk()))
    dbgpln("  Fill  %5i) %6.2f->%6.2f   L:%6.2f   %%:%6.2f   Amt2Ad:%8.4f                             T:%6.2f  PrS:%6.2f  PrE:%6.2f",
      m_ID, Start(), End(), Length(), Frac, AmtToAdd, K_2_C(m_pMdl->Temp()), m_dPressS, m_dPressE);
  #endif
  };

// ===========================================================================
//
//                            Specie Q Section List
//
// ===========================================================================

SpQSection * SpQSectionList::Add(double Ss, double Se, byte Where, POSITION AtPos, POSITION &RetPos, SpQSectionList & m_Spares)
  {
  SpQSection * pSct=m_Spares.GetCount() > 0 ? m_Spares.RemoveHead() : new SpQSection;

  pSct->m_ID=SpQSection::m_IDCurrent++;
  pSct->m_dPosition = Ss;
  pSct->m_dLength   = Se-Ss;
  pSct->m_bTentative=false;
  pSct->m_dMassScale = 1;
  pSct->m_pMdl->ZeroMass();

  switch (Where)
    {
    case ADDHEAD   : RetPos=AddHead(pSct); break;
    case ADDTAIL   : RetPos=AddTail(pSct); break;
    case ADDBEFORE : RetPos=InsertBefore(AtPos, pSct); break;
    case ADDAFTER  : RetPos=InsertAfter (AtPos, pSct); break;
    default: ASSERT(0);
    }

  pSct->m_ListPos=RetPos;
  return pSct;
  }

// --------------------------------------------------------------------------

bool SpQSectionList::Break(double P, POSITION AtPos, POSITION &LastPos, SpQSectionList & m_Spares)
  {
  m_pQ->DumpProfile(2, "Inter Break Begin", 4);

  SpQSection * pSct1=GetAt(AtPos);
  //if ((pSct1->End()-P)<m_pQ->MinSectLength() || (P-pSct1->Start())<m_pQ->MinSectLength())
  //  {
  //#if dbgSpQueue
  //if (dbgBreak() && (!dbgDbgBrk() || dbgDbgBrk() && m_pQ->DoDbgBrk()))
  //  dbgpln("   -- Brk %6.2f->%6.2f   L:%6.2f   @:%6.2f        %-8s        %8s %s",
  //             pSct1->Start(), pSct1->End(), pSct1->Length(), P, "Skip", "", m_pQ->FullObjTag());
  //#endif
  //  LastPos=NULL;
  //  return false;
  //  }

  SpQSection * pSct2=Add(P, pSct1->End(), ADDAFTER, AtPos, LastPos, m_Spares);

  double Scl1=(P-pSct1->Start())/pSct1->Length();
  double Scl2=1.0-Scl1;
  ASSERT(Scl1>=0.0 && Scl1<=1.0);
  ASSERT(Scl2>=0.0 && Scl2<=1.0);

  #if dbgSpQueue
  if (dbgBreak() && (!dbgDbgBrk() || dbgDbgBrk() && m_pQ->DoDbgBrk()))
    dbgpln("   -- Brk %6.2f->%6.2f   L:%6.2f   @:%6.2f   Scl1:%8.4f   Scl2:%8.4f %s",
               pSct1->Start(), pSct1->End(), pSct1->Length(), P, Scl1, Scl2, m_pQ->FullObjTag());
  #endif

  pSct2->m_dMassScale = pSct1->m_dMassScale;
  pSct1->m_dLength *= Scl1;
  pSct2->CopyMass(pSct1, Scl2);
  pSct1->ScaleMass(Scl1);

  #if dbgSpQueue
  if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && m_pQ->DoDbgBrk()))
    {
    dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct1->m_pMdl->Temp()), pSct1->m_pMdl->Mass());
    dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct2->m_pMdl->Temp()), pSct2->m_pMdl->Mass());
    }
  #endif

  m_pQ->DumpProfile(2, "Inter Break End", 4);
  return true;
  };

// ===========================================================================
//
//                            Specie PF Section List
//
// ===========================================================================

SpQSection * CSpPFSectionList::Add(double Ss, double Se, byte Where, POSITION AtPos, POSITION &RetPos, CSpPFSectionList & m_Spares)
  {
  SpQSection * pSct=m_Spares.GetCount() > 0 ? m_Spares.RemoveHead() : new SpQSection;

  pSct->m_ID=SpQSection::m_IDCurrent++;
  pSct->m_dPosition = Ss;
  pSct->m_dLength   = Se-Ss;
  pSct->m_bTentative=false;
  pSct->m_dMassScale = 1;
  pSct->m_pMdl->ZeroMass();

  switch (Where)
    {
    case ADDHEAD   : RetPos=AddHead(pSct); break;
    case ADDTAIL   : RetPos=AddTail(pSct); break;
    case ADDBEFORE : RetPos=InsertBefore(AtPos, pSct); break;
    case ADDAFTER  : RetPos=InsertAfter (AtPos, pSct); break;
    default: ASSERT(0);
    }

  pSct->m_ListPos=RetPos;

  return pSct;
  }

// --------------------------------------------------------------------------

bool CSpPFSectionList::Break(double P, POSITION AtPos, POSITION &LastPos, CSpPFSectionList & m_Spares)
  {
  //m_pQ->DumpProfile(2, "Inter Break Begin", 4);

  SpQSection * pSct1=GetAt(AtPos);
  SpQSection * pSct2=Add(P, pSct1->End(), ADDAFTER, AtPos, LastPos, m_Spares);

  double Scl1=(P-pSct1->Start())/pSct1->Length();
  double Scl2=1.0-Scl1;
  ASSERT(Scl1>=0.0 && Scl1<=1.0);
  ASSERT(Scl2>=0.0 && Scl2<=1.0);

  #if dbgSpQueue
  if ((dbgBreak()||dbgAdvSects()) && (!dbgDbgBrk() || dbgDbgBrk() && m_pPF->DoDbgBrk()))
    dbgpln("   -- Brk %6.2f->%6.2f   L:%6.2f   @:%6.2f   Scl1:%8.4f   Scl2:%8.4f %s",
               pSct1->Start(), pSct1->End(), pSct1->Length(), P, Scl1, Scl2, m_pPF->FullObjTag());
  #endif

  pSct1->m_dLength *= Scl1;
  pSct2->CopyMass(pSct1, Scl2);
  pSct1->ScaleMass(Scl1);

  #if dbgSpQueue
  if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && m_pPF->DoDbgBrk()))
    {
    dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct1->m_pMdl->Temp()), pSct1->m_pMdl->Mass());
    dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct2->m_pMdl->Temp()), pSct2->m_pMdl->Mass());
    }
  #endif

  //m_pPF->DumpProfile(2, "Inter Break End", 4);
  return true;
  };

// ===========================================================================
//
//                              Specie QIO
//
// ===========================================================================

SpQIO::SpQIO()
  {
  m_pSpill        = NULL;
  m_pVent         = NULL;
  m_dPosition     = 0;
  m_dQmEst        = 0;
  m_dQmAct        = 0;
  m_dLoss         = 0.0;

  m_dPosMin       = 0;
  m_dPosMax       = 0;
  m_dIOSpd        = 0;
  m_bIOMoving     = false;
  m_dBeltStart    = 0;
  m_dBeltEnd      = 0;

  m_bUseBinSizes  = false;
  m_dBinWidth     = 0;
  m_dSpillMargin  = 0;

  m_dRemoval      = 1.0;
  m_pQ            = NULL;
  m_pCond         = new SpConduit("", NULL, TOA_Free);
  m_fFeedLimited  = False;
  m_dFeedCapFrac  = 0.8;
  m_eType         = QIO_Fixed;
  m_lIndex        = 0;
  }

SpQIO::~SpQIO()
  {
  if (m_pCond)
    m_pCond->Destroy();
  };

void SpQIO::SetUseBinSizes(bool On)//, double BinWide, double SpillMargin)
  {
  m_bUseBinSizes=On;
  };

void SpQIO::SetIOMoving(bool On, double PosActual, double PosReqd, double Spd, double TimeSlice, double MinPosn, double MaxPosn) 
  { 
  if (On)
    {
    if (m_dPosition!=PosActual) 
      {
      m_pQ->m_bPosChg=1;
      m_dPosition=PosActual;
      }
    if (!Valid(PosReqd))
      Spd=0.0;
    else if (PosReqd>PosActual)
      Spd=Min(Spd, (PosReqd-PosActual)/TimeSlice);
    else if (PosReqd<PosActual)
      Spd=Max(Spd, (PosReqd-PosActual)/TimeSlice);
    else
      Spd=0.0;
    }

  m_bIOMoving=On;
  m_dIOSpd=Spd;
  m_dPosMin=MinPosn;
  m_dPosMax=MaxPosn;

  #if dbgSpQueue
  if (dbgUnLoad())// && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
    {
    dbglock();
    dbgp("SpQIO::SetIOMoving[%i] Pos:%6.2f %s", Index(), m_dPosition, On?"Moving":"      ");
    if (On)
      dbgp(" Spd:%6.2f RqdPos:%6.2f MinPos:%6.2f MaxPos:%6.2f", Spd, PosReqd, MinPosn, MaxPosn);
    dbgpln("");
    dbgunlock();
    }
  #endif
  };

void SpQIO::SetPosition(double PosActual) 
  { 
  if (m_dPosition!=PosActual) 
    {
    m_pQ->m_bPosChg=1;
    m_dPosition=PosActual;
    }; 

  #if dbgSpQueue
  if (dbgUnLoad())// && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
    dbgpln("SpQIO::SetPosition[%i] Pos:%6.2f", Index(), m_dPosition);
  #endif
  };

void SpQIO::SetSpill(char * SpillTag)
  {
  Spill.SetTarget(SpillTag);
  };

// ===========================================================================
//
//                              Specie Queue
//
// ===========================================================================

flag DataCollection(void * Ptr)
  {
  SpQueue * p=(SpQueue *)Ptr;
  return p->UpdateProfile();
  }

// ===========================================================================


IMPLEMENT_SPARES(SpQueue, 100);
IMPLEMENT_TAGOBJ(SpQueue, "SpQueue", "SpQueue", "", "", "", TOC_SYSTEM,
                 "Specie Queue", "Specie Queue");

SpQueue::SpQueue(pTagObjClass pClass_, pchar pTag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  SpModelOwner(pClass_, pTag_, pAttach, eAttach, False),
  m_FnProfile(&C2DFnClass, "Profile", this, TOA_Embedded),
  m_Sections(this),
  m_FeedSections(this),
  m_Spares(this)
  {
  FlwNode * pNd=dynamic_cast<FlwNode *>(pAttach);
  ASSERT(pNd!=NULL);
  m_pSpill=NULL;
  m_pVent=NULL;

  m_dSpeed=1.0;
  m_dMaxSpeed=1.0;
  m_dMinFracSectLen=0.0; //should this default be 0.0? perhaps 0.001?
  m_dTempTol=1.0e-6;
  m_dLoadTol=1.0e-6;
  m_dMFracTol=1.0e-6;

  m_dLength=10.0;
  m_dMaxSpillLoading=100.0;
  m_dMinLoading=0.001;
  m_dTimeSlice=0.0;
  
  m_bSumLosses=false;
  m_dTotSpiltM=0.0;
  m_dTotSpiltHf=0.0;
  m_dTotSpiltHs=0.0;
  m_dTotSpiltHz=0.0;
  m_dTotVentedM=0.0;
  m_dTotVentedHf=0.0;
  m_dTotVentedHs=0.0;
  m_dTotVentedHz=0.0;

  m_nMaxTurnDown=50;
  m_bPosChg=0;
  //pSpill=NULL;

  m_fProfileOK=0;
  m_eProfileType=QPT_None;
  m_FnProfile.SetDataCollection(DataCollection, this);

  m_TripperOn     = false;
  m_dTripSpeed    = 0.1;
  m_dTripPosition = 0.0;
  m_dTripPosMin   = 0.0;
  m_dTripPosMax   = 1e6;
  };

// --------------------------------------------------------------------------

SpQueue::SpQueue(pchar pTag_, TaggedObject* pAttach, TagObjAttachment eAttach, CDirectFlwIO * pSpill, CDirectFlwIO * pVent, CDirectFlwIO * pAccIn, CDirectFlwIO * pAccOut) :
  SpModelOwner(&SpQueueClass, pTag_, pAttach, eAttach, False),
  m_FnProfile(&C2DFnClass, "Profile", this, TOA_Embedded),
  m_Sections(this),
  m_FeedSections(this),
  m_Spares(this)
  {
  ASSERT(NULL!=dynamic_cast<FlwNode *>(pAttach));
  m_pSpill            = pSpill;
  m_pVent             = pVent;
  m_pAccIn            = pAccIn;
  m_pAccOut           = pAccOut;
  m_dSpeed            = 1.0;
  m_dMinFracSectLen   = 0.0;
  m_dTempTol          = 1.0e-6;
  m_dLoadTol          = 1.0e-6;
  m_dMFracTol         = 1.0e-6;

  m_dMaxSpeed         = 1.0;
  m_dLength           = 10.0;
  m_dMaxSpillLoading  = 100.0;
  m_dMinLoading       = 0.001;
  m_dTimeSlice        = 0.0;

  m_bSumLosses        = false;
  m_dTotSpiltM        = 0.0;
  m_dTotSpiltHf       = 0.0;
  m_dTotSpiltHs       = 0.0;
  m_dTotSpiltHz       = 0.0;
  m_dTotVentedM       = 0.0;
  m_dTotVentedHf      = 0.0;
  m_dTotVentedHs      = 0.0;
  m_dTotVentedHz      = 0.0;

  m_nMaxTurnDown      = 50;
  m_bPosChg           = 0;
  //pSpill              = NULL;

  m_fProfileOK        = 0;
  m_eProfileType      = QPT_None;
  m_FnProfile.SetDataCollection(DataCollection, this);

  m_TripperOn     = false;
  m_dTripSpeed    = 0.1;
  m_dTripPosition = 0.0;
  m_dTripPosMin   = 0.0;
  m_dTripPosMax   = 1e6;
  };

// --------------------------------------------------------------------------

SpQueue::~SpQueue()
  {
  m_FnProfile.SetLength(0);
  while (m_Sections.GetCount()>0)
    m_Spares.AddTail(m_Sections.RemoveHead());
  while (m_Spares.GetCount()>0)
    {
    SpQSection * pSct=m_Spares.RemoveHead();
    delete pSct;
    }
  };

// --------------------------------------------------------------------------

void SpQueue::BuildDataDefn(DataDefnBlk &DDB)
  {
  if (DDB.BeginStruct(this))
    {
    DDB.Text(" ");
    DDB.Text("");
    }
  DDB.EndStruct();
  };

// --------------------------------------------------------------------------

flag SpQueue::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;
  return 0;
  };

// --------------------------------------------------------------------------

static byte SpQ_Sct=0x48;
static byte SpQ_End=0x49;
flag SpQueue::GetOtherData(FilingControlBlock &FCB)
  {
  DWORD nBytes;
  //byte What;
  SpQSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    {
    FCB.WriteFile(&SpQ_Sct, sizeof(SpQ_Sct), &nBytes);
    pSct->GetOtherData(FCB);
    }

  FCB.WriteFile(&SpQ_End, sizeof(SpQ_End), &nBytes);

  return True;
  };

// --------------------------------------------------------------------------

flag SpQueue::PutOtherData(FilingControlBlock &FCB)
  {
  m_Sections.RemoveAll();

  byte What;
  DWORD nRead;
  FCB.ReadFile(&What, sizeof(What), &nRead);
  while (What==SpQ_Sct && nRead==sizeof(What))
    {
    SpQSection * pSct=new SpQSection;
    pSct->m_ID=SpQSection::m_IDCurrent++;
    pSct->m_ListPos=m_Sections.AddTail(pSct);
    pSct->PutOtherData(FCB);

    FCB.ReadFile(&What, sizeof(What), &nRead);
    }
  ASSERT(What==SpQ_End);

  return True;
  };

//--------------------------------------------------------------------------

void   SpQueue::SetState(eScdMdlStateActs RqdState)
  {
  switch (RqdState)
    {
    case MSA_Empty:
      Empty();
      SetTotalSpilt(0.0, 0.0, 0.0, 0.0);
      SetTotalVented(0.0, 0.0, 0.0, 0.0);
      break;
    case MSA_PreSet:
      break;
    case MSA_ZeroFlows:
      Empty();
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    case MSA_DynStatsRunInit:
      SetTotalSpilt(0.0, 0.0, 0.0, 0.0);
      SetTotalVented(0.0, 0.0, 0.0, 0.0);
      break;
    }
  };

// --------------------------------------------------------------------------

void SpQueue::SetNFeeds(int NFeeds)
  {
  ASSERT(NFeeds>=0);
  if (m_Feed.GetSize()!=NFeeds)
    {
    m_bPosChg = 1;
    int n=m_Feed.GetSize();
    m_Feed.SetSize(NFeeds);
    for (int i=n; i<NFeeds; i++)
      {
      m_Feed[i].m_pSpill=m_pSpill;
      m_Feed[i].m_pVent=m_pVent;
      m_Feed[i].m_pQ=this;
      };
    }
  };

// --------------------------------------------------------------------------

void SpQueue::SetNProds(int NProds)
  {
  ASSERT(NProds>=0);
  if (m_Prod.GetSize()!=NProds)
    {
    m_bPosChg=1;
    int n=m_Prod.GetSize();
    m_Prod.SetSize(NProds);
    for (int i=0; i<NProds; i++)
      {
      m_Prod[i].m_pSpill=m_pSpill;
      m_Prod[i].m_pVent=m_pVent;
      m_Prod[i].m_pQ=this;
      }
    }
  };

// --------------------------------------------------------------------------

void SpQueue::EvalProducts()
  {

  DumpProfile(2, "Before EvalProd", 0);
  int NF=m_Feed.GetSize();
  int NP=m_Prod.GetSize();

  m_bSumLosses=false;

  Spill.Cd.QZero();
  for (int i=0; i<NF; i++)
    m_Feed[i].Spill.Cd.QZero();
  for (int i=0; i<NP; i++)
    m_Prod[i].Spill.Cd.QZero();

  if (m_bPosChg || m_FeedOrder.GetLen()!=NF || m_ProdOrder.GetLen()!=NP)
    {
    // Sort Feed & Prods into order of increasing position
    m_bPosChg=0;
    int i,j;
    m_FeedOrder.SetSize(NF);
    for (i=0; i<NF; i++)
      m_FeedOrder[i]=i;
    for (i=1; i<NF; i++)
      for (j=i; j>=1 && m_Feed[m_FeedOrder[j]].Position() < m_Feed[m_FeedOrder[j-1]].Position() ; j--)
        Exchange(m_FeedOrder[j],m_FeedOrder[j-1]);

    m_ProdOrder.SetSize(NP);
    for (i=0; i<NP; i++)
      m_ProdOrder[i]=i;
    for (i=1; i<NP; i++)
      for (j=i; j>=1 && m_Prod[m_ProdOrder[j]].Position() < m_Prod[m_ProdOrder[j-1]].Position() ; j--)
        Exchange(m_ProdOrder[j],m_ProdOrder[j-1]);
    }

  while (!m_FeedSections.IsEmpty())
    m_Spares.AddTail(m_FeedSections.RemoveHead());

  SpQSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    pSct->m_dMassScale=1;

  if (m_dSpeed>=0.0)
    {
    int iF=0, iP=0;
    while (iF<NF || iP<NP)
      {
      double FPos=iF<NF ? m_Feed[m_FeedOrder[iF]].Position() : 1.0e300;
      double PPos=iP<NP ? m_Prod[m_ProdOrder[iP]].Position() : 1.0e300;
      if (FPos<PPos)
        EstFeed(m_FeedOrder[iF++]);
      else
        EstProd(m_ProdOrder[iP++]);
      }
    }
  else
    {
    int iF=NF-1, iP=NP-1;
    while (iF>=0 || iP>=0)
      {
      double FPos=iF>=0 ? m_Feed[m_FeedOrder[iF]].Position() : -1.0;
      double PPos=iP>=0 ? m_Prod[m_ProdOrder[iP]].Position() : -1.0;
      if (FPos>PPos)
        EstFeed(m_FeedOrder[iF--]);
      else
        EstProd(m_ProdOrder[iP--]);
      }
    }

  if (DoDbgBrk())
    {int xxx=0;};

  DumpProfile(2, "After EvalProd", 0);
  }

// --------------------------------------------------------------------------

void SpQueue::Add2Sections(int FeedNo, SpQSectionList & Sections)
  {
  SpQIO &F=m_Feed[FeedNo];
  SpConduit * pFeed=F.m_pCond;

  double MinFeedRate = m_dMinLoading*m_dTimeSlice*m_dSpeed;
  if (pFeed->QMass()<MinFeedRate)//1.0e-10)
    return;

  double Ps=F.m_dBeltStart;
  double Pe=F.m_dBeltEnd;

#if dbgSpQueue
  if (dbgLoad() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
    dbgpln("SpQ:AddS  [%i]  %6.2f->%6.2f   QmEst:%8.4f   QmAct:%8.4f  T:%6.2f  M:%8.4f %s",
           FeedNo, Ps, Pe, Range(-999.9999,F.m_dQmEst,999.9999), F.m_dQmAct, K2C(pFeed->Temp()), pFeed->QMass(som_ALL), FullObjTag());
  #endif
  POSITION SctPos;
  SpQSectionIter SCT(Sections);
  SpQSection * pSct=SCT.First();
  if (pSct==NULL)
    { // No Sections Add One
    pSct=Sections.Add(Ps, Pe, ADDHEAD, NULL, SctPos, m_Spares);
    pSct->Load(this, 1.0, pFeed, F.Spill.Connected ? &F.Spill : &Spill, F.Vent.Connected ? &F.Vent: &Vent);
    }
  else
    {
    SpQSection * pSctPrv=NULL;
    while (pSct && (pSct->End() < Ps))
      {
      pSctPrv=pSct;
      pSct=SCT.Next();
      }
    if (pSct==NULL)
      { // Must Add A Section - No Section at end
      ////  PrvExists  and              IsShort             and              NoGap
      //if (pSctPrv && (pSctPrv->Length() < MinSectLength()) && (Ps-LenTol() <= pSctPrv->End()))
      //  {
      //  // expand section behind forwards
      //  pSctPrv->m_dLength=Pe-pSctPrv->m_dPosition;
      //  }
      //else
        {
        pSct=Sections.Add(Ps, Pe, ADDTAIL, NULL, SctPos, m_Spares);
        SCT.MakeCurrent(SctPos);
        }
      }
    else
      {
      if (pSct->Start() > Ps)
        { // - Gap in Sections at Start
        ////               IsShort             and              NoGap
        //if ((pSct->Length() < MinSectLength()) && (Pe+LenTol() >= pSct->Start()))
        //  {
        //  // expand section ahead backwards
        //  pSct->m_dPosition-=Pe-Ps;
        //  pSct->m_dLength+=Pe-Ps;
        //  }
        //else
          {
          // Must Add A Section at start
          pSct=Sections.Add(Ps, Min(Pe, pSct->Start()), ADDBEFORE, SCT.Pos(), SctPos, m_Spares);
          SCT.MakeCurrent(SctPos);
          }
        }
      }

    while (1)
      {
      if (pSct->Start()<Ps-LenTol() && pSct->End()>Ps+LenTol())
        {
        POSITION PosOfEnd;
        if (Sections.Break(Ps, SCT.Pos(), PosOfEnd, m_Spares))
          pSct=SCT.MakeCurrent(PosOfEnd);
        }
      if (pSct->Start()<Pe-LenTol() && pSct->End()>Pe+LenTol())
        {
        POSITION PosOfEnd;
        if (Sections.Break(Pe, SCT.Pos(), PosOfEnd, m_Spares))
          pSct=SCT.MakeCurrent(SCT.Pos());
        }

      double Scl=(Min(pSct->End(), Pe)-Max(pSct->Start(), Ps))/GTZ(Pe-Ps);
      pSct->Load(this, Scl, pFeed, F.Spill.Connected ? &F.Spill : &Spill, F.Vent.Connected ? &F.Vent: &Vent);
      //#if dbgSpQueue
      //if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
      //  dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Mass());
      //#endif
      SpQSection * pSctPrv=pSct;
      if (pSct->End() >= Pe)
        break;

      POSITION PosPrv=SCT.Pos();
      pSct=SCT.Next();
      if (pSct==NULL)
        { // Must Add A Section - No Section at end
        ////  PrvExists  and              IsShort             and              NoGap
        //if (pSctPrv && (pSctPrv->Length() < MinSectLength()) && (Ps-LenTol() <= pSctPrv->End()))
        //  {
        //  // expand section behind forwards
        //  pSctPrv->m_dLength=Pe-pSctPrv->m_dPosition;
        //  pSct=SCT.MakeCurrent(PosPrv);
        //  }
        //else
          {
          pSct=Sections.Add(pSctPrv->End(), Pe, ADDTAIL, NULL, SctPos, m_Spares);
          SCT.MakeCurrent(SctPos);
          }
        }
      else
        {
        double GapLen=pSct->Start()-pSctPrv->End();
        if (GapLen > LenTol())
          {
          pSct=Sections.Add(pSctPrv->End(), pSct->Start(), ADDBEFORE, SCT.Pos(), SctPos, m_Spares);
          SCT.MakeCurrent(SctPos);
          }
        }
      }
    }
  }

// --------------------------------------------------------------------------

bool SpQueue::MergeSections(bool One2Two, SpQSection * pSctOne, SpQSection * pSctTwo)
  {
  SpQSection &S1=*pSctOne;
  SpQSection &S2=*pSctTwo;

  if (S1.Length()+S2.Length()>MinSectLength()) // combined length ?
    return false;
  if (S1.Start()-S2.End()>LenTol())            // Contiguous ?
    return false;

  SpModel &M1=*S1.m_pMdl;
  SpModel &M2=*S2.m_pMdl;

  double Ld1=S1.Mass()/GTZ(S1.Length());
  double Ld2=S2.Mass()/GTZ(S2.Length());

  if (!ConvergedVV(Ld1, Ld2, 1.0e-3, m_dLoadTol))             // Load the Same
    return false;
  if (!ConvergedVV(M1.Temp(), M2.Temp(), 0.01, m_dTempTol))   // Temp The Same
    return false;

  double MT1=GTZ(M1.Mass());
  double MT2=GTZ(M2.Mass());
  for (int s=0; s<SVSpcCount(); s++)
    if (!ConvergedVV(M1.m_M[s]/MT1, M2.m_M[s]/MT2, 1.0e-6, m_dMFracTol))
      return false;

  //CHECK What about Attributes && Qualities differences on Merge of Sections ?

  if (One2Two)
    {
    M2.AddMassF(&M1, som_ALL, 1.0);
    S2.m_dLength=S1.Length()+S2.Length();
    S2.m_dPosition=S1.Start();
    m_Sections.RemoveAt(S1.m_ListPos);
    m_Spares.AddTail(pSctOne);
    }
  else
    {
    M1.AddMassF(&M2, som_ALL, 1.0);
    S1.m_dLength=S1.Length()+S2.Length();
    m_Sections.RemoveAt(S2.m_ListPos);
    m_Spares.AddTail(pSctTwo);
    }

  return true;
  }

// --------------------------------------------------------------------------

void SpQueue::EstFeed(int FeedNo)
  {
  SpQIO &F=m_Feed[FeedNo];
  SpConduit * pFeed=F.m_pCond;

  // NBNB The Queue will have moved
  double Pa, Pb;
  if (F.m_bIOMoving)
    {
    double AppIOSpd=(Range(F.m_dPosMin, F.m_dPosition+m_dTimeSlice*F.m_dIOSpd, F.m_dPosMax)-F.m_dPosition)/GTZ(m_dTimeSlice);
    Pa=F.m_dPosition;
    Pb=F.m_dPosition+m_dTimeSlice*(m_dSpeed-AppIOSpd);
    }
  else
    {
    Pa=F.m_dPosition;
    Pb=F.m_dPosition+m_dTimeSlice*m_dSpeed;
    }
  double Ps=Min(Pa, Pb);
  double Pe=Max(Pa, Pb);
  F.m_dBeltStart=Ps;
  F.m_dBeltEnd=Pe;

  Add2Sections(FeedNo, m_FeedSections);

  }

// --------------------------------------------------------------------------

void SpQueue::LoadFeed(int FeedNo)
  {
  SpQIO &F=m_Feed[FeedNo];
  SpConduit * pFeed=F.m_pCond;

  Add2Sections(FeedNo, m_Sections);
  }

// --------------------------------------------------------------------------

void SpQueue::EstProd(int ProdNo)//, int Feed0, int FeedN)
  {
  SpQIO &P=m_Prod[ProdNo];
  SpConduit * pProd=P.m_pCond;
  pProd->QZero();

  // NBNB The Queue will NOT have moved yet
  double Pa, Pb;
  double Bs=0;
  double Be=0;

  double GateS=-m_dLength;
  double GateE=2*m_dLength;

  double AppIOSpd=0;
  if (m_TripperOn)
    {
    Pa=m_dTripPosition;
    Pb=m_dTripPosition+m_dTimeSlice*(m_dTripSpeed-m_dSpeed);
    Bs=Min(Pa, Pb);
    Be=Max(Pa, Pb);
    ASSERT_ALWAYS(P.m_bUseBinSizes, "P.m_bUseBinSizes", __FILE__, __LINE__);

    double S=P.m_dPosition-P.m_dBinWidth;
    double E=P.m_dPosition+P.m_dBinWidth;
    Bs=Max(S, Bs);
    Be=Min(Be, E);
    }
  else if (P.m_bIOMoving)
    {
    AppIOSpd=(Range(P.m_dPosMin, P.m_dPosition+m_dTimeSlice*P.m_dIOSpd, P.m_dPosMax)-P.m_dPosition)/GTZ(m_dTimeSlice);
    Pa=P.m_dPosition-m_dTimeSlice*(m_dSpeed-AppIOSpd);
    Pb=P.m_dPosition;
    Bs=Min(Pa, Pb);
    Be=Max(Pa, Pb);
    }
  else
    {
    Pa=P.m_dPosition-m_dTimeSlice*m_dSpeed;
    Pb=P.m_dPosition;
    Bs=Min(Pa, Pb);
    Be=Max(Pa, Pb);
    }

  P.m_dBeltStart=Bs;
  P.m_dBeltEnd=Be;

  #if dbgSpQueue
  if (dbgUnLoad() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
    {
    dbgpln("SpQ:UnLoad[%i]  [S/E%6.2f->%6.2f]   QmEst:%8.4f   QmAct:%8.4f  T:%6.2f  M:%8.4f %s",
           ProdNo, P.m_dBeltStart, P.m_dBeltEnd, Range(-999.9999, P.m_dQmEst, 999.9999), P.m_dQmAct, K2C(pProd->Temp()), pProd->QMass(som_ALL), FullObjTag());
    if (!m_TripperOn && P.m_bIOMoving)
      {
      dbgpln("                Pos:%6.2f  IOSpd:%6.2f  PosMin:%6.2f  PosMax:%6.2f", P.m_dPosition, P.m_dIOSpd, P.m_dPosMin, P.m_dPosMax);
      dbgpln("                Tm: %6.2f  BlSpd:%6.2f  AppIOSpd:%6.2f", m_dTimeSlice, m_dSpeed, AppIOSpd);
      }
    }
  #endif

  if (P.m_dBeltStart>=P.m_dBeltEnd)
    {
    P.m_dBeltEnd=P.m_dBeltStart;
    return;
    }

  for (int iPass=0; iPass<2; iPass++)
    {
    if (iPass>0)
      { // Adjust apparent Belt Positions for the Feed Queue
      Bs+=m_dTimeSlice*m_dSpeed;
      Be+=m_dTimeSlice*m_dSpeed;
      }

    SpQSectionList & Sections=(iPass==0 ? m_Sections:m_FeedSections);
    SpQSectionIter SCT(Sections);
    for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
      {
      #if dbgSpQueue
      if (dbgUnLoad() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
        dbgpln("  Seg %i %5i) [S/E%6.2f->%6.2f]   %6.2f->%6.2f   L:%6.2f", iPass,
          pSct->m_ID, Bs,Be,pSct->Start(), pSct->End(), pSct->Length());
      #endif
      if (pSct->End() <= Bs)
        continue;
      if (pSct->Start() >= Be)
        break;

      if (pSct->Start()<Bs-LenTol() && pSct->End()>Bs+LenTol())
        {
        POSITION PosOfEnd;
        if (Sections.Break(Bs, SCT.Pos(), PosOfEnd, m_Spares))
          pSct=SCT.MakeCurrent(PosOfEnd);
        }
      if (pSct->Start()<Be-LenTol() && pSct->End()>Be+LenTol())
        {
        POSITION PosOfEnd;
        if (Sections.Break(Be, SCT.Pos(), PosOfEnd, m_Spares))
          pSct=SCT.MakeCurrent(SCT.Pos());
        }

      double Bee=Min(pSct->End(), Be);
      double Bss=Max(pSct->Start(), Bs);
      double Scl=(Bee-Bss)/GTZ(pSct->Length());
      pSct->SetMakeup(this, Scl*P.Removal(), P.m_dQmEst, pProd);
      }
    }
  }

// --------------------------------------------------------------------------

void SpQueue::UnLoadProd(int ProdNo)
  {

  SpQIO &P=m_Prod[ProdNo];
  SpConduit * pProd=P.m_pCond;
  pProd->QZero();

  double Ps=P.m_dBeltStart;
  double Pe=P.m_dBeltEnd;

  #if dbgSpQueue
  if (dbgUnLoad() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
    dbgpln("SpQ:UnLoad[%i]  %6.2f->%6.2f   QmEst:%8.4f   QmAct:%8.4f  T:%6.2f  M:%8.4f %s",
           ProdNo, Ps, Pe, Range(-999.9999, P.m_dQmEst, 999.9999), P.m_dQmAct, K2C(pProd->Temp()), pProd->QMass(som_ALL), FullObjTag());
  #endif

  SpQSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    {
    if (pSct->End() <= Ps)
      continue;
    if (pSct->Start() >= Pe)
      break;

    if (pSct->Start()<Ps-LenTol() && pSct->End()>Ps+LenTol())
      {
      POSITION PosOfEnd;
      if (m_Sections.Break(Ps, SCT.Pos(), PosOfEnd, m_Spares))
        pSct=SCT.MakeCurrent(PosOfEnd);
      }
    if (pSct->Start()<Pe-LenTol() && pSct->End()>Pe+LenTol())
      {
      POSITION PosOfEnd;
      if (m_Sections.Break(Pe, SCT.Pos(), PosOfEnd, m_Spares))
        pSct=SCT.MakeCurrent(SCT.Pos());
      }

    double Scl=(Min(pSct->End(), Pe)-Max(pSct->Start(), Ps))/GTZ(pSct->Length());
    pSct->UnLoad(this, Scl*P.Removal(), P.m_dQmEst, pProd, P.Spill.Connected ? &P.Spill : &Spill, P.Vent.Connected ? &P.Vent: &Vent);

    //#if dbgSpQueue
    //if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
    //  dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Mass());
    //#endif
    }
  }

// --------------------------------------------------------------------------

void SpQueue::SetStepInfo(double dTime, double MaxSpeed, double Speed, int MaxTurnDown)
  {
  m_dMaxSpeed=MaxSpeed;
  m_nMaxTurnDown=Min(MaxTurnDown, TurnDownLimit);
  if (fabs(Speed) < m_dMaxSpeed/TurnDownLimit)
    m_dSpeed=0.0;
  else
    m_dSpeed=Speed;

  m_dTimeSlice=dTime;

  if (m_dTimeSlice <= 1.0e-10)
    {
    }

  }

// --------------------------------------------------------------------------

void SpQueue::Advance(double dTime, flag Moved)//, double MaxSpeed, double Speed, int MaxTurnDown/*=20*/)
  {
  m_bSumLosses=true;

  m_dTimeSlice=dTime;
  m_dMinFracSectLen = Range(0.0,    m_dMinFracSectLen, 0.25);
  m_dTempTol        = Range(1.0e-9, m_dTempTol       , 0.25);
  m_dLoadTol        = Range(1.0e-9, m_dLoadTol       , 0.25);
  m_dMFracTol       = Range(1.0e-9, m_dMFracTol      , 0.25);

  // Remove Feed Sections - they are only used during EvalProducts .
  while (!m_FeedSections.IsEmpty())
    m_Spares.AddTail(m_FeedSections.RemoveHead());

  if (m_dTimeSlice <=1.0e-10)
    return;

  double AdvDist=Moved ? m_dTimeSlice*m_dSpeed :  0.0;
  if (fabs(m_dSpeed) < 0.001*m_dMaxSpeed/TurnDownLimit)
    {
    ClrCI(1);
    ClrCI(2);
    AdvDist=0.0;
    }
  else if (fabs(m_dSpeed) < m_dMaxSpeed/TurnDownLimit)
    {
    ClrCI(1);
    SetCI(2);
    AdvDist=0.0;
    }
  else if (fabs(m_dSpeed) < m_dMaxSpeed/m_nMaxTurnDown)
    {
    SetCI(1);
    ClrCI(2);
    }
  else
    {
    ClrCI(1);
    ClrCI(2);
    }

  #if dbgSpQueue
  if (dbgAdvance() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
    dbgpln("SpQ:Advance:%g %s",AdvDist, FullObjTag());
  #endif

  DumpProfile(0, "Before EvalDiscrete", 0);

  m_fProfileOK=0;

  if (DoDbgBrk())
    {int xxx=0;};

  SpQSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    pSct->m_dPosition+=AdvDist;

  DumpProfile(0, "After Advance", 0);

  int NF=m_Feed.GetSize();
  int NP=m_Prod.GetSize();

  //for (int i=0; i<NF; i++)
  //  {
  //  m_Feed[i].m_dBeltStart+=AdvDist;
  //  m_Feed[i].m_dBeltEnd+=AdvDist;
  //  }
  for (int i=0; i<NP; i++)
    {
    m_Prod[i].m_dBeltStart+=AdvDist;
    m_Prod[i].m_dBeltEnd+=AdvDist;
    }

  if (m_bPosChg || m_FeedOrder.GetLen()!=NF || m_ProdOrder.GetLen()!=NP)
    {
    // Sort Feed & Prods into order of increasing position
    m_bPosChg=0;
    int i,j;
    m_FeedOrder.SetSize(NF);
    for (i=0; i<NF; i++)
      m_FeedOrder[i]=i;
    for (i=1; i<NF; i++)
      for (j=i; j>=1 && m_Feed[m_FeedOrder[j]].Position() < m_Feed[m_FeedOrder[j-1]].Position() ; j--)
        Exchange(m_FeedOrder[j],m_FeedOrder[j-1]);

    m_ProdOrder.SetSize(NP);
    for (i=0; i<NP; i++)
      m_ProdOrder[i]=i;
    for (i=1; i<NP; i++)
      for (j=i; j>=1 && m_Prod[m_ProdOrder[j]].Position() < m_Prod[m_ProdOrder[j-1]].Position() ; j--)
        Exchange(m_ProdOrder[j],m_ProdOrder[j-1]);
    }

  m_pSpill->Cd.QZero();
  m_pVent->Cd.QZero();
  if (m_dSpeed>=0.0)
    {
    int iF=0, iP=0;
    while (iF<NF || iP<NP)
      {
      double FPos=iF<NF ? m_Feed[m_FeedOrder[iF]].Position() : 1.0e300;
      double PPos=iP<NP ? m_Prod[m_ProdOrder[iP]].Position() : 1.0e300;
      if (FPos<PPos)
        {
        LoadFeed(m_FeedOrder[iF++]);
        DumpProfile(2, "Inter Feed", 4);
        }
      else
        {
        UnLoadProd(m_ProdOrder[iP++]);
        DumpProfile(2, "Inter Prod", 4);
        }
      }
    }
  else
    {
    int iF=NF-1, iP=NP-1;
    while (iF>=0 || iP>=0)
      {
      double FPos=iF>=0 ? m_Feed[m_FeedOrder[iF]].Position() : -1.0;
      double PPos=iP>=0 ? m_Prod[m_ProdOrder[iP]].Position() : -1.0;
      if (FPos>PPos)
        {
        LoadFeed(m_FeedOrder[iF--]);
        DumpProfile(2, "Inter Feed", 4);
        }
      else
        {
        UnLoadProd(m_ProdOrder[iP--]);
        DumpProfile(2, "Inter Prod", 4);
        }
      }
    }

  if (DoDbgBrk())
    {int xxx=0;};

  // Trim Start - End of Belt - Spill if neccessary
  for (pSct=SCT.First(); pSct; pSct=SCT.Next())
    {
    double Ss=pSct->Start();      // Start Of Section
    double Se=pSct->End();        // End Of Section
    double T=pSct->m_pMdl->Temp();  // For Debugging
    if (Se>m_dLength)
      {
      if (Ss>m_dLength)
        {  // Remove
        pSct->Spill(this, 1.0, m_pSpill, m_pVent);
        m_Sections.RemoveAt(SCT.Pos());
        m_Spares.AddTail(pSct);
        #if dbgSpQueue
        if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
          dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Mass());
        #endif

        }
      else
        { // Chop off end - Scale Mass
        double Scl=(m_dLength-Ss)/GTZ(Se-Ss);
        pSct->Spill(this, 1.0-Scl, m_pSpill, m_pVent);
        //pSct->ScaleMass(Scl);
        pSct->SetLength(m_dLength-Ss);
        #if dbgSpQueue
        if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
          dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Mass());
        #endif
        }
      }
    else if (Ss<0.0)
      {
      if (Se<0.0)
        { // Remove
        pSct->Spill(this, 1.0, m_pSpill, m_pVent);
        #if dbgSpQueue
        if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
          dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Mass());
        #endif
        m_Sections.RemoveAt(SCT.Pos());
        m_Spares.AddTail(pSct);
        }
      else
        { // Chop off start - Scale Mass
        double Scl=(Se-0.0)/GTZ(Se-Ss);
        pSct->Spill(this, 1.0-Scl, m_pSpill, m_pVent);
        //pSct->ScaleMass(Scl);
        pSct->SetStart(0.0);
        pSct->SetLength(Se-0.0);
        #if dbgSpQueue
        if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
          dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Mass());
        #endif
        };
      }
    }

  DumpProfile(2, "Before Merge", 0);

  // Merge Short Sections
  if (m_dMinFracSectLen>0.0)
    {
    SpQSectionIter SCT(m_Sections);
    if (m_dSpeed>0)
      {
      SpQSection * pSct1=SCT.First();
      while (pSct1)// && pSct2)
        {
        if (pSct1->Loading()<1.0e-6)
          {
          m_Sections.RemoveAt(pSct1->m_ListPos);
          m_Spares.AddTail(pSct1);
          pSct1=SCT.Next();
          }
        else
          {
          SpQSection * pSct2=SCT.Next();
          if (pSct2)
            MergeSections(true, pSct1, pSct2);
          pSct1=pSct2;
          }
        }
      }
    else
      {
      SpQSection * pSct2=SCT.Last();
      while (pSct2)
        {
        if (pSct2->Loading()<1.0e-6)
          {
          m_Sections.RemoveAt(pSct2->m_ListPos);
          m_Spares.AddTail(pSct2);
          pSct2=SCT.Prev();
          }
        else
          {
          SpQSection * pSct1=SCT.Prev();
          if (pSct1)
            MergeSections(false, pSct1, pSct2);
          pSct2=pSct1;
          }
        }
      }
    }

  DumpProfile(1, "After EvalDiscrete", 0);
  }

// --------------------------------------------------------------------------

void SpQueue::Empty()
  {
  SpQSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    pSct->ScaleMass(0.0); // Clear it

  int NP=m_Prod.GetSize();
  for (int i=0; i<NP; i++)
    {
    m_Prod[i].m_dQmEst=0.0;
    m_Prod[i].m_dQmAct=0.0;
    m_Prod[i].m_dLoss=0.0;
    m_Prod[i].m_pCond->QZero();
    }

  if (m_eProfileType!=QPT_None)
    {
    m_FnProfile.SetLength(0);
    }
  }

// --------------------------------------------------------------------------

flag SpQueue::FeedLimited(int FeedNo)
  {
  return m_Feed[FeedNo].m_fFeedLimited;
  };

// --------------------------------------------------------------------------

double SpQueue::FeedCapacity(int FeedNo)
  {
  if (m_Feed[FeedNo].m_fFeedLimited)
    {
    return Range(0.0, m_Feed[FeedNo].m_dFeedCapFrac, 1.0)*m_dSpeed*m_dMaxSpillLoading;
    }
  return dNAN;
  };

// --------------------------------------------------------------------------

void SpQueue::SetFeed(int FeedNo, double Qm, SpConduit * pFeed)
  {
  Qm=Max(0.0, Qm);
  SpQIO &F=m_Feed[FeedNo];
  F.m_dQmEst=Qm;
  F.m_dQmAct=Min(Qm, pFeed->QMass(som_ALL));
  F.m_pCond->QSetM(*pFeed, som_ALL, F.m_dQmAct, Std_P);
  }

// --------------------------------------------------------------------------

void SpQueue::SetProductQmEst(int ProdNo, double QmEst)
  {
  QmEst=Max(0.0, QmEst);
  SpQIO &P=m_Prod[ProdNo];
  P.m_dQmEst=QmEst;
  }

// --------------------------------------------------------------------------

void SpQueue::GetProduct(int ProdNo, /*double Qm, */SpConduit * pProd)
  {
  SpQIO &P=m_Prod[ProdNo];
  //P.m_dQmEst=Qm;
  P.m_dQmAct=Min(P.m_dQmEst, P.m_pCond->QMass(som_ALL));
  pProd->QSetM(*P.m_pCond, som_ALL, P.m_dQmAct, Std_P);
  }

// --------------------------------------------------------------------------

void SpQueue::SetTripper(bool On, double Speed, double Position, double PosnMin, double PosnMax)
  {
  m_TripperOn     = On;
  if (On)
    {
    m_dTripSpeed    = Speed;
    m_dTripPosition = Position;
    m_dTripPosMin   = PosnMin;
    m_dTripPosMax   = PosnMax;
    //dbgpln("SetTripper Spd%10.4f Pos%10.4f Min%10.4f Max%10.4f", m_dTripSpeed, m_dTripPosition, m_dTripPosMin, m_dTripPosMax);
    }
  };

// --------------------------------------------------------------------------

flag SpQueue::UpdateProfile()
  {
  if (!m_fProfileOK && m_eProfileType!=QPT_None)
    {
    switch (m_eProfileType)
      {
      case QPT_AllSections:
        {
        const int nSections = m_Sections.GetCount();
        m_FnProfile.SetLength(nSections*2);
        int SectCnt=0;
        SpQSectionIter SCT(m_Sections);
        for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
          {
          const double SctLoading=pSct->Mass()/GTZ(pSct->Length());
          m_FnProfile.SetPt((SectCnt*2), pSct->Start(), SctLoading);
          m_FnProfile.SetPt((SectCnt*2)+1, pSct->End(), SctLoading);
          SectCnt++;
          }
        break;
        }
      case QPT_MinSections:
        {
        const int nSections = m_Sections.GetCount();
        if (nSections>0)
          {
          int nPts = 0;
          SpQSectionIter SCT(m_Sections);
          SpQSection * pSct=SCT.First();
          double PrevLoading = pSct->Mass()/GTZ(pSct->Length());
          for (pSct=SCT.Next(); pSct; pSct=SCT.Next())
            {
            const double SctLoading=pSct->Mass()/GTZ(pSct->Length());
            if (fabs(PrevLoading-SctLoading)>1.0e-9)
              {
              nPts+=2;
              PrevLoading=SctLoading;
              }
            }
          m_FnProfile.SetLength(nPts+2);

          nPts = 0;
          pSct=SCT.First();
          PrevLoading = pSct->Mass()/GTZ(pSct->Length());
          m_FnProfile.SetPt(nPts++, pSct->Start(), PrevLoading);
          SpQSection * pPrevSct = pSct;
          for (pSct=SCT.Next(); pSct; pSct=SCT.Next())
            {
            const double SctLoading=pSct->Mass()/GTZ(pSct->Length());
            if (fabs(PrevLoading-SctLoading)>1.0e-9)
              {
              m_FnProfile.SetPt(nPts++, pPrevSct->End(), PrevLoading);
              m_FnProfile.SetPt(nPts++, pSct->Start(), SctLoading);
              PrevLoading=SctLoading;
              }
            pPrevSct = pSct;
            }
          m_FnProfile.SetPt(nPts++, pPrevSct->End(), PrevLoading);
          }
        else
          m_FnProfile.SetLength(0);
        break;
        }
      case QPT_FixedPts:
        {
        int nPrfPts=m_FnProfile.GetLen()/2;
        if (nPrfPts>0)
          {
          m_FnProfile.SetLength(Range(0, 2*nPrfPts, 200));
          for (int j=0; j<nPrfPts*2; j+=2)
            {
            m_FnProfile.SetPt(j,   (j/2*Length())/nPrfPts, 0.0);
            m_FnProfile.SetPt(j+1, ((j/2+1)*Length())/nPrfPts, 0.0);
            }

          CDVector & X=m_FnProfile.Xs();
          CDVector & Y=m_FnProfile.Ys();

          double XLen=1.0/nPrfPts;
          SpQSectionIter SCT(m_Sections);
          for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
            {
            double SctLoading=pSct->Mass()/GTZ(pSct->Length());
            double ps=pSct->Start()/ Length();
            double pe=pSct->End()  / Length();
            int is= Range(0, (int)floor(ps*nPrfPts), nPrfPts-1);
            int ie= Range(0, (int)floor(pe*nPrfPts), nPrfPts-1);

            double p0=ps;
            for (int i=is; i<=ie; i++)
              {
              double p1=Min(((double)(i+1))/nPrfPts, pe);
              Y[i*2] += SctLoading * (p1-p0)/XLen;
              p0=p1;
              }

            /*
            Y[i0*2] += SctLoading * (((float)(i0+1))/nPrfPts - p0)/XLen;
            for (int i=i0+1; i<i1; i++)
              Y[i*2] += SctLoading;
            if (i1>i0)
              Y[i1*2] += SctLoading * (p1-((float)i1/nPrfPts))/XLen;
            */
            }
          for (j=0; j<nPrfPts*2; j+=2)
            Y[j+1]=Y[j];
          }
        break;
        }
      }
    m_fProfileOK=1;
    return True;
    }
  return False;
  }

// --------------------------------------------------------------------------

void SpQueue::DumpProfile(int Where, LPCTSTR Comment, long Indent)
  {
  #if dbgSpQueue
  if (((Where==0 && dbgProfileBefore()) ||
       (Where==1 && dbgProfileAfter()) ||
       (Where==2 && dbgProfileInter())) &&
       (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
    {
    dbgpln("%*sSpQ:Profile %s %s", Indent," ", Comment, FullObjTag());
    SpQSectionIter SCT(m_Sections);
    for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
      dbgpln("%*s%5i]  %6.2f->%6.2f   L:%6.2f   M:%10.5f M/m:%10.5f  T:%6.2f",Indent,"", pSct->m_ID, pSct->Start(), pSct->End(), pSct->Length(), pSct->Mass(), pSct->Mass()/GTZ(pSct->Length()), K_2_C(pSct->Temp()));
    if (dbgProfileFeed())
      {
      SpQSectionIter SCTF(m_FeedSections);
      for (pSct=SCTF.First(); pSct; pSct=SCTF.Next())
        dbgpln("%*s%5i]F %6.2f->%6.2f   L:%6.2f   M:%10.5f M/m:%10.5f  T:%6.2f",Indent,"", pSct->m_ID, pSct->Start(), pSct->End(), pSct->Length(), pSct->Mass(), pSct->Mass()/GTZ(pSct->Length()), K_2_C(pSct->Temp()));
      }
    }
  #endif
  }

// --------------------------------------------------------------------------

double SpQueue::TotalMass()
  {
  double M=0.0;
  SpQSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    M+=pSct->Mass();
  return M;
  }

// --------------------------------------------------------------------------

double SpQueue::TotalHf()
  {
  double H=0.0;
  SpQSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    H+=pSct->totHf();
  return H;
  }

// --------------------------------------------------------------------------

double SpQueue::TotalHs()
  {
  double H=0.0;
  SpQSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    H+=pSct->totHs();
  return H;
  }

// --------------------------------------------------------------------------

double SpQueue::TotalHz()
  {
  double H=0.0;
  SpQSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    H+=pSct->totHz();
  return H;
  }

// --------------------------------------------------------------------------

void SpQueue::AddTotals(double &Mass, double &Hz, CSysVector * pSysVec)
  {
  //double M=0.0;
  //double H=0.0;
  //if (pSysVec)
  //  pSysVec->Zero();
  SpQSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    {
    Mass+=pSct->Mass();
    Hz+=pSct->m_pMdl->totHz();
    if (pSysVec)
      pSysVec->Add(*pSct->m_pMdl, som_ALL);
    }
  }

// --------------------------------------------------------------------------

double SpQueue::AverageLoading()
  {
  double M=0.0;
  double L=0.0;
  SpQSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    {
    M+=pSct->Mass();
    L+=pSct->Length();
    }
  return M/GTZ(L);
  }

// --------------------------------------------------------------------------

double SpQueue::Loading(double Dist)
  {
  if (Dist>=0.0)
    {
    SpQSectionIter SCT(m_Sections);
    for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
      if (pSct->End()>=Dist)
        return pSct->Mass()/GTZ(pSct->Length());
    }
  return 0.0;
  }

//--------------------------------------------------------------------------

flag SpQueue::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tSlow Speed";             return 1;
    case  2: pS="W\tSlow Speed - Stopped";   return 1;
    case  3: pS="W\tSpill Occurred";         return 1;
    default:
      return SpModelOwner::CIStrng(No, pS);
    }
  };

// ===========================================================================
//
//                              Specie PFIO
//
// ===========================================================================

CSpPFIO::CSpPFIO()
  {
  m_dPosition=0;
  m_dQmEst=0;
  m_dQmAct=0;
  m_pPF=NULL;
  m_pCond=new SpConduit("", NULL, TOA_Free);
  m_lIndex=0;
  }

CSpPFIO::~CSpPFIO()
  {
  if (m_pCond)
    m_pCond->Destroy();
  };

// ===========================================================================
//
//                              Specie PlugFlow
//
// ===========================================================================

IMPLEMENT_SPARES(CSpPlugFlow, 100);
IMPLEMENT_TAGOBJ(CSpPlugFlow, "CSpPlugFlow", "CSpPlugFlow", "", "", "", TOC_SYSTEM,
                             "Specie PlugFlow", "Specie PlugFlow");

CSpPlugFlow::CSpPlugFlow(pTagObjClass pClass_, pchar pTag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  SpModelOwner(pClass_, pTag_, pAttach, eAttach, False),
  m_Sections(this),
  m_Spares(this)
  {
  m_dLength=100.0;
  m_dVolume=1.0;
  m_dTimeSlice=0.0;
  };

// --------------------------------------------------------------------------

CSpPlugFlow::CSpPlugFlow(pchar pTag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  SpModelOwner(&CSpPlugFlowClass, pTag_, pAttach, eAttach, False),
  m_Sections(this),
  m_Spares(this)
  {
  m_dLength=100.0;
  m_dVolume=1.0;
  m_dTimeSlice=0.0;
  };

// --------------------------------------------------------------------------

CSpPlugFlow::~CSpPlugFlow()
  {
  //m_FnProfile.SetLength(0);
  while (m_Sections.GetCount()>0)
    m_Spares.AddTail(m_Sections.RemoveHead());
  while (m_Spares.GetCount()>0)
    {
    SpQSection * pSct=m_Spares.RemoveHead();
    delete pSct;
    }
  };

// --------------------------------------------------------------------------

void CSpPlugFlow::BuildDataDefn(DataDefnBlk &DDB)
  {
  if (DDB.BeginStruct(this))
    {
    DDB.Text(" ");
    DDB.Text("");
    }
  DDB.EndStruct();
  };

// --------------------------------------------------------------------------

flag CSpPlugFlow::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;
  return 0;
  };

// --------------------------------------------------------------------------

flag CSpPlugFlow::GetOtherData(FilingControlBlock &FCB)
  {
  DWORD nBytes;
  SpPFSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    {
    FCB.WriteFile(&SpQ_Sct, sizeof(SpQ_Sct), &nBytes);
    pSct->GetOtherData(FCB);
    }

  FCB.WriteFile(&SpQ_End, sizeof(SpQ_End), &nBytes);

  return True;
  };

/*
    BOOL ReadFile(LPVOID lpBuffer,// address of buffer that receives data
                  DWORD nNumberOfBytesToRead,// number of bytes to read
                  LPDWORD lpNumberOfBytesRead);// address of number of bytes read
    BOOL WriteFile(LPCVOID lpBuffer,// address of data to write to file
                   DWORD nNumberOfBytesToWrite,// number of bytes to write
                   LPDWORD lpNumberOfBytesWritten);// address of number of bytes written
    DWORD SetFilePointer(LONG lDistanceToMove,// number of bytes to move file pointer
                         DWORD dwMoveMethod);// how to move
    BOOL Eof();
*/

// --------------------------------------------------------------------------

flag CSpPlugFlow::PutOtherData(FilingControlBlock &FCB)
  {
  m_Sections.RemoveAll();

  byte What;
  DWORD nRead;
  FCB.ReadFile(&What, sizeof(What), &nRead);
  while (What==SpQ_Sct && nRead==sizeof(What))
    {
    SpQSection * pSct=new SpQSection;
    pSct->m_ID=SpQSection::m_IDCurrent++;
    m_Sections.AddTail(pSct);
    pSct->PutOtherData(FCB);

    FCB.ReadFile(&What, sizeof(What), &nRead);
    }
  ASSERT(What==SpQ_End);

/*
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    pSct->GetOtherData(FCB);

  for (i=0; i<m_Feed.GetSize(); i++)
    m_Feed[i]->GetOtherData(FCB);

  for (i=0; i<m_Prod.GetSize(); i++)
    m_Prod[i]->GetOtherData(FCB);
*/
  return True;
  };

//--------------------------------------------------------------------------

void CSpPlugFlow::SetState(eScdMdlStateActs RqdState)
  {
  switch (RqdState)
    {
    case MSA_Empty:
      Empty();
      break;
    case MSA_PreSet:
      break;
    case MSA_ZeroFlows:
      Empty();
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  };

// --------------------------------------------------------------------------

void CSpPlugFlow::EvalProducts()
  {
  }

// --------------------------------------------------------------------------

void CSpPlugFlow::LoadIO(int FeedNo, double dTime, double VelStart, double VelEnd)
  {

  CSpPFIO& F=m_IO[FeedNo];
  SpConduit * pFeed=F.m_pCond;

  double Pa, Pb;
  Pa=F.m_dPosition;
  Pb=F.m_dPosition+dTime*(VelStart+(VelEnd-VelStart)*F.m_dPosition/Length());
  double Ps=Min(Pa, Pb);
  double Pe=Max(Pa, Pb);

  #if dbgSpQueue
  if (dbgLoad() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
    dbgpln("SpQ :Load  [%i] %6.2f->%6.2f   QmEst:%8.4f   QmAct:%8.4f  T:%6.2f  P:%6.2f  M:%8.4f %s",
           FeedNo, Ps, Pe, Range(-999.9999, F.m_dQmEst, 999.9999), F.m_dQmAct, K2C(pFeed->Temp()), pFeed->Press(), pFeed->QMass(som_ALL), FullObjTag());
  #endif
  POSITION SctPos;
  SpPFSectionIter SCT(m_Sections);
  SpQSection * pSct=SCT.First();
  if (pSct==NULL)
    { // No m_Sections Add One
    pSct=m_Sections.Add(Ps, Pe, ADDHEAD, NULL, SctPos, m_Spares);
    pSct->Load(this, 1.0, pFeed);
    //#if dbgSpQueue
    //if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
    //  dbgpln("     --> T:%6.2f  P:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Press(), pSct->m_pMdl->Mass());
    //#endif
    }
  else
    {
    while (pSct && (pSct->End() < Ps)) // Fix a memory gobbler
      pSct=SCT.Next();

    //double Ss, Se;
    if (pSct==NULL)
      { // Must Add A Section - Gap in m_Sections at Start
      pSct=m_Sections.Add(Ps, Pe, ADDTAIL, NULL, SctPos, m_Spares);
      SCT.MakeCurrent(SctPos);
      }
    else if (pSct->Start() > Ps)
      { // Must Add A Section - Gap in m_Sections at Start
      pSct=m_Sections.Add(Ps, Min(Pe, pSct->Start()), ADDBEFORE, SCT.Pos(), SctPos, m_Spares);
      SCT.MakeCurrent(SctPos);
      }

    while (1)//pSct && (pSct->Start() <= Pe))
      {

      if (pSct->Start()<Ps-LenTol() && pSct->End()>Ps+LenTol())
        {
        POSITION PosOfEnd;
        if (m_Sections.Break(Ps, SCT.Pos(), PosOfEnd, m_Spares))
          pSct=SCT.MakeCurrent(PosOfEnd);
        }
      if (pSct->Start()<Pe-LenTol() && pSct->End()>Pe+LenTol())
        {
        POSITION PosOfEnd;
        if (m_Sections.Break(Pe, SCT.Pos(), PosOfEnd, m_Spares))
          pSct=SCT.MakeCurrent(SCT.Pos());
        }

      double Scl=(Min(pSct->End(), Pe)-Max(pSct->Start(), Ps))/GTZ(Pe-Ps);
      pSct->Load(this, Scl, pFeed);
      //#if dbgSpQueue
      //if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
      //  dbgpln("     --> T:%6.2f  P:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Press(), pSct->m_pMdl->Mass());
      //#endif
      SpQSection * pSctPrv=pSct;
      if (pSct->End() >= Pe)
        break;

      pSct=SCT.Next();
      if (pSct==NULL)
        { // Must Add A Section - Gap in m_Sections At End
        pSct=m_Sections.Add(pSctPrv->End(), Pe, ADDTAIL, NULL, SctPos, m_Spares);
        SCT.MakeCurrent(SctPos);
        }
      else
        {
        double GapLen=pSct->Start()-pSctPrv->End();
        if (GapLen > LenTol())
          {
          pSct=m_Sections.Add(pSctPrv->End(), pSct->Start(), ADDBEFORE, SCT.Pos(), SctPos, m_Spares);
          SCT.MakeCurrent(SctPos);
          }
        }
      }
    }

  }

// --------------------------------------------------------------------------

void CSpPlugFlow::UnLoadIO(int ProdNo, double dTime, double VelStart, double VelEnd, double PRqd)
  {
  CSpPFIO &P=m_IO[ProdNo];
  SpConduit * pProd=P.m_pCond;
  pProd->QZero();

  double Pa, Pb;
  Pa=P.m_dPosition;
  Pb=P.m_dPosition+dTime*(VelStart+(VelEnd-VelStart)*P.m_dPosition/Length());
  double Ps=Min(Pa, Pb);
  double Pe=Max(Pa, Pb);

  #if dbgSpQueue
  if (dbgUnLoad() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
    dbgpln("SpQ :UnLoad[%i] %6.2f->%6.2f   QmEst:%8.4f   QmAct:%8.4f  T:%6.2f  P:%6.2f  M:%8.4f %s",
           ProdNo, Ps, Pe, Range(-999.9999, P.m_dQmEst, 999.9999), P.m_dQmAct, K2C(pProd->Temp()), pProd->Press(), pProd->QMass(som_ALL), FullObjTag());
  #endif

  SpPFSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    {
    if (pSct->End() <= Ps)
      continue;
    if (pSct->Start() >= Pe)
      break;

    if (pSct->Start()<Ps-LenTol() && pSct->End()>Ps+LenTol())
      {
      POSITION PosOfEnd;
      if (m_Sections.Break(Ps, SCT.Pos(), PosOfEnd, m_Spares))
        pSct=SCT.MakeCurrent(PosOfEnd);
      }
    if (pSct->Start()<Pe-LenTol() && pSct->End()>Pe+LenTol())
      {
      POSITION PosOfEnd;
      if (m_Sections.Break(Pe, SCT.Pos(), PosOfEnd, m_Spares))
        pSct=SCT.MakeCurrent(SCT.Pos());
      }

    double Scl=(Min(pSct->End(), Pe)-Max(pSct->Start(), Ps))/GTZ(pSct->Length());
    pSct->UnLoad(this, Scl, P.m_dQmEst, pProd);//, P.SpillTarget()->Configured() ? P.SpillTarget() : SpillTarget());
    pProd->SetPress(PRqd);
    //#if dbgSpQueue
    //if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
    //  dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Mass());
    //#endif
    }
  }


// --------------------------------------------------------------------------

void CSpPlugFlow::Advance(double dTime, double VelStart, double VelEnd, bool LinearPressProfile, double PressStart, double PressEnd, flag Moved)//, double MaxSpeed, double Speed, int MaxTurnDown/*=20*/)
  {
  m_dTimeSlice=dTime;

  if (m_dTimeSlice <=1.0e-10)
    return;

  if (DoDbgBrk())
    {int xxx=0;};

  SpPFSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    {
    //pSct->dPositionStart=pSct->m_dPosition;
    pSct->m_dVelS=(VelStart+(VelEnd-VelStart)*pSct->m_dPosition/Length());
    pSct->m_dVelE=(VelStart+(VelEnd-VelStart)*(pSct->m_dPosition+pSct->m_dLength)/Length());
    pSct->m_dDeltaPosS=dTime*pSct->m_dVelS;
    double dPosE=dTime*pSct->m_dVelE;
    pSct->m_dDeltaLen=dPosE-pSct->m_dDeltaPosS;
    pSct->m_dPosition+=pSct->m_dDeltaPosS;
    pSct->m_dLength+=pSct->m_dDeltaLen;
    }

  if (LinearPressProfile)
    {
    long i=0;
    for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
      {
      pSct->m_dPressS=(PressStart+(PressEnd-PressStart)*pSct->m_dPosition/Length());
      pSct->m_dPressE=(PressStart+(PressEnd-PressStart)*(pSct->m_dPosition+pSct->m_dLength)/Length());
      pSct->m_pMdl->SetPress(0.5*(pSct->m_dPressS+pSct->m_dPressE));
      pSct->m_dVelS=(VelStart+(VelEnd-VelStart)*pSct->m_dPosition/Length());
      pSct->m_dVelE=(VelStart+(VelEnd-VelStart)*(pSct->m_dPosition+pSct->m_dLength)/Length());
      #if dbgSpQueue
      if (dbgAdvance() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
        dbgpln("SpPF:Advance: %3i - %5i) Pos:%10.2f%+10.4f Len:%10.2f%+10.4f"
                " Vs:%6.3f Ve:%6.3f   Ps:%6.2f Pe:%6.2f  T:%6.2f P:%6.2f M:%6.2f %s",
                i, pSct->m_ID,
                pSct->m_dPosition, pSct->m_dDeltaPosS, pSct->m_dLength, pSct->m_dDeltaLen,
                pSct->m_dVelS, pSct->m_dVelE,
                pSct->m_dPressS, pSct->m_dPressE, K2C(pSct->m_pMdl->Temp()),
                pSct->m_pMdl->Press(), pSct->m_pMdl->Mass(), FullObjTag());
      #endif
      }
    }

  if (VelStart>=0.0)
    LoadIO(0, dTime, VelStart, VelEnd);
  else
    UnLoadIO(0, dTime, VelStart, VelEnd, PressStart);
  if (VelEnd>=0.0)
    UnLoadIO(1, dTime, VelStart, VelEnd, PressEnd);
  else
    LoadIO(1, dTime, VelStart, VelEnd);

  if (DoDbgBrk())
    {int xxx=0;};

  // Trim Start - End - Spill if neccessary
  for (pSct=SCT.First(); pSct; pSct=SCT.Next())
    {
    double Ss=pSct->Start();      // Start Of Section
    double Se=pSct->End();        // End Of Section
    double T=pSct->m_pMdl->Temp();  // For Debugging
    if (Se>m_dLength)
      {
      if (Ss>m_dLength)
        {  // Remove
        pSct->Spill(this, 1.0);
        m_Sections.RemoveAt(SCT.Pos());
        m_Spares.AddTail(pSct);
        //#if dbgSpQueue
        //if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
        //  dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Mass());
        //#endif
        }
      else
        { // Chop off end - Scale Mass
        double Scl=(m_dLength-Ss)/GTZ(Se-Ss);
        pSct->Spill(this, 1.0-Scl);
        pSct->ScaleMass(Scl);
        pSct->SetLength(m_dLength-Ss);
        pSct->m_dPressS=(PressStart+(PressEnd-PressStart)*pSct->m_dPosition/Length());
        pSct->m_dPressE=(PressStart+(PressEnd-PressStart)*(pSct->m_dPosition+pSct->m_dLength)/Length());
        pSct->m_pMdl->SetPress(0.5*(pSct->m_dPressS+pSct->m_dPressE));
        pSct->m_dVelS=(VelStart+(VelEnd-VelStart)*pSct->m_dPosition/Length());
        pSct->m_dVelE=(VelStart+(VelEnd-VelStart)*(pSct->m_dPosition+pSct->m_dLength)/Length());
        //#if dbgSpQueue
        //if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
        //  dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Mass());
        //#endif
        }
      }
    else if (Ss<0.0)
      {
      if (Se<0.0)
        { // Remove
        pSct->Spill(this, 1.0);
        //#if dbgSpQueue
        //if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
        //  dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Mass());
        //#endif
        m_Sections.RemoveAt(SCT.Pos());
        m_Spares.AddTail(pSct);
        }
      else
        { // Chop off start - Scale Mass
        double Scl=(Se-0.0)/GTZ(Se-Ss);
        pSct->Spill(this, 1.0-Scl);
        pSct->ScaleMass(Scl);
        pSct->SetStart(0.0);
        pSct->SetLength(Se-0.0);
        pSct->m_dPressS=(PressStart+(PressEnd-PressStart)*pSct->m_dPosition/Length());
        pSct->m_dPressE=(PressStart+(PressEnd-PressStart)*(pSct->m_dPosition+pSct->m_dLength)/Length());
        pSct->m_pMdl->SetPress(0.5*(pSct->m_dPressS+pSct->m_dPressE));
        pSct->m_dVelS=(VelStart+(VelEnd-VelStart)*pSct->m_dPosition/Length());
        pSct->m_dVelE=(VelStart+(VelEnd-VelStart)*(pSct->m_dPosition+pSct->m_dLength)/Length());
        //#if dbgSpQueue
        //if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
        //  dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Mass());
        //#endif
        };
      }
    }

  if (0 && LinearPressProfile)
    {
    for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
      {
      pSct->m_dPressS=(PressStart+(PressEnd-PressStart)*pSct->m_dPosition/Length());
      pSct->m_dPressE=(PressStart+(PressEnd-PressStart)*(pSct->m_dPosition+pSct->m_dLength)/Length());
      pSct->m_pMdl->SetPress(0.5*(pSct->m_dPressS+pSct->m_dPressE));
      pSct->m_dVelS=(VelStart+(VelEnd-VelStart)*pSct->m_dPosition/Length());
      pSct->m_dVelE=(VelStart+(VelEnd-VelStart)*(pSct->m_dPosition+pSct->m_dLength)/Length());
      }
    }
  }

// --------------------------------------------------------------------------

void CSpPlugFlow::Empty()
  {
  SpPFSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    pSct->ScaleMass(0.0); // Clear it

  int NP=2;//m_Prod.GetSize();
  for (int i=0; i<NP; i++)
    {
    m_IO[i].m_dQmEst=0.0;
    m_IO[i].m_dQmAct=0.0;
    //m_IO[i].m_dLoss=0.0;
    m_IO[i].m_pCond->QZero();
    }
  }

// --------------------------------------------------------------------------

void CSpPlugFlow::Fill(SpConduit & C, PhMask PhaseM, double FillFrac, double PIn, double POut, double VelIn, double VelOut)
  {
  // Remove All Scts;
  POSITION SctPos;
  SpQSection * pSct;
  SpPFSectionIter SCT(m_Sections);
  for (pSct=SCT.First(); pSct; pSct=SCT.Next())
    {
    m_Sections.RemoveAt(SCT.Pos());
    m_Spares.AddTail(pSct);
    }

  pSct=SCT.First();
  long NSegs=10;
  for (int iSeg=0; iSeg<NSegs; iSeg++)
    {
    double Ps=double(iSeg)/NSegs*m_dLength;
    double Pe=double(iSeg+1)/NSegs*m_dLength;

    if (pSct==NULL)
      pSct=m_Sections.Add(Ps, Pe, ADDHEAD, NULL, SctPos, m_Spares);
    else
      pSct=m_Sections.Add(Ps, Pe, ADDTAIL, NULL, SctPos, m_Spares);
    double PX=(POut-PIn)/Length();
    double VX=(VelIn-VelOut)/Length();
    pSct->Fill(this, FillFrac, &C, PIn+Ps*PX, PIn+Pe*PX, VelIn+Ps*VX, VelIn+Pe*VX);
    SCT.MakeCurrent(SctPos);
    //#if dbgSpQueue
    //if (dbgAdvSects() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
    //  dbgpln("     --> T:%6.2f  M:%8.4f", K2C(pSct->m_pMdl->Temp()), pSct->m_pMdl->Mass());
    //#endif
    }
  };

// --------------------------------------------------------------------------

void CSpPlugFlow::Sample(/*double FracPos,*/ CFlowSamplePt & Pt)
  {
  Pt.m_pCnd->QZero();
  double P=Pt.m_dFracPos*Length();
  Pt.m_dAbsPos=P;
  SpPFSectionIter SCT(m_Sections);
  for (SpQSection * pSct=SCT.First(); pSct; pSct=SCT.Next())
    {
    if (pSct->Start() <= P && pSct->End() >= P)
      {
      double Vel=pSct->m_dVelS+(pSct->m_dVelE-pSct->m_dVelS)*(P-pSct->Start())/pSct->Length();
      double Frac=Min(1.0, Vel*1/*Sec*//GTZ(pSct->Length()));
      double MnPress=pSct->m_dPressS+(pSct->m_dPressE-pSct->m_dPressS)*(P-pSct->Start())/pSct->Length();
      Pt.m_pCnd->QAddF(*pSct->m_pMdl, som_ALL, Frac);
      Pt.m_pCnd->SetPress(MnPress);
      Pt.m_dVel=Vel;
      #if dbgSpQueue
      if (dbgSample() && (!dbgDbgBrk() || dbgDbgBrk() && DoDbgBrk()))
        dbgpln("SpPF:Sample @%10.2f  Pos:%10.2f Len:%10.2f"
                " Vs:%6.3f Ve:%6.3f   Ps:%6.2f Pe:%6.2f  T:%6.2f P:%6.2f M:%6.2f %s",
                P,
                pSct->m_dPosition, pSct->m_dLength,
                pSct->m_dVelS, pSct->m_dVelE,
                pSct->m_dPressS, pSct->m_dPressE, K2C(pSct->m_pMdl->Temp()),
                pSct->m_pMdl->Press(), pSct->m_pMdl->Mass(), FullObjTag());
      #endif
      }
    }
  };

// --------------------------------------------------------------------------

void CSpPlugFlow::MeasureProperties(CSpPropInfoArray & Props, double StartPos, double EndPos)
  {
  double L=GTZ(EndPos-StartPos);
  long nSegs=0;
  SpPFSectionIter SCT(m_Sections);
  SpQSection * pSct;
  for (pSct=SCT.First(); pSct; pSct=SCT.Next())
    {
    if (!(pSct->Start() >= EndPos || pSct->End() <= StartPos))
      nSegs++;
    }

  Props.SetSize(nSegs);

  long iSeg=0;
  for (pSct=SCT.First(); pSct; pSct=SCT.Next())
    {
    if (!(pSct->Start() >= EndPos || pSct->End() <= StartPos))
      {
      CSpPropInfo & Prop=Props[iSeg++];
      Prop.m_bLastSegment=(iSeg==nSegs);
      Prop.m_dLenFraction=(Min(pSct->End(), EndPos)-Max(pSct->Start(), StartPos))/L;

      SpModel &M=*pSct->m_pMdl;
      double T=M.Temp();
      double P=M.Press();
      Prop.SetPropInfoVL(M, M.VolFrac(som_Gas, T, P), M.VolFrac(som_SL, T, P), NULL);
      }
    }
  };

// --------------------------------------------------------------------------

void CSpPlugFlow::GetInput(int No, double Qm, SpConduit * pFeed)
  {
  Qm=Max(0.0, Qm);
  CSpPFIO &F=m_IO[No];
  F.m_dQmEst=Qm;
  F.m_dQmAct=Min(Qm, pFeed->QMass(som_ALL));
  F.m_pCond->QSetM(*pFeed, som_ALL, F.m_dQmAct, pFeed->Press());
  }

// --------------------------------------------------------------------------

void CSpPlugFlow::SetOutput(int No, double Qm, SpConduit * pProd)
  {
  Qm=Max(0.0, Qm);
  CSpPFIO &P=m_IO[No];
  P.m_dQmEst=Qm;
  P.m_dQmAct=Min(Qm, P.m_pCond->QMass(som_ALL));
  pProd->QSetM(*P.m_pCond, som_ALL, P.m_dQmAct, P.m_pCond->Press());
  pProd->SetTempPress(P.m_pCond->Temp(), P.m_pCond->Press());
  }

// --------------------------------------------------------------------------

flag CSpPlugFlow::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tSlow Speed";             return 1;
    case  2: pS="W\tSlow Speed - Stopped";   return 1;
    case  3: pS="W\tSpill Attempted";        return 1;
    default:
      return SpModelOwner::CIStrng(No, pS);
    }
  };

// ===========================================================================
//
//
//
// ===========================================================================
