//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __EQNSOLVE_CPP
#include "eqnsolve.h"

//#include "optoff.h"

#define dbgEqnSolve     (0 || WITHDEBUG)
#if dbgEqnSolve
#include "dbgmngr.h"
static CDbgMngr dbgTears            ("FlwSolve", "Tears");
static CDbgMngr dbgHoldTearAdvance  ("FlwSolve", "Tears-HoldAdvance");
static CDbgMngr dbgTearSetup        ("FlwSolve", "Tears-Setup");
static CDbgMngr dbgTearSetupAll     ("FlwSolve", "Tears-SetupAll");
static CDbgMngr dbgShowAll          ("FlwSolve", "Tears-ShowAll");
static CDbgMngr dbgTearCnvgLoop     ("FlwSolve", "Tears-CnvgLoop");
#endif

XID xidNTearVars    = SysXID(100);
XID xidNTearHistory = SysXID(101);

const long  SQN_Start           = 0;
const long  SQN_SettleAndTweak  = 1;
const long  SQN_SettleAndCalc   = 2;
const long  SQN_Control         = 5;

// ========================================================================

TearVar::TearVar()
  {
  m_DampingRqd=dNAN;
  m_DampFactor=0.0;
  m_EstDampFactor=0.0;
  dYRat=0.0;
  AdvFmt=NULL;
  AdvCnv=NULL;

  EPS_Abs=dNAN;
  EPS_Rel=dNAN;
  CurTol=0.0;
  MeasFmt=NULL;
  MeasCnv=NULL;

  m_bTestIsValid=true;
  m_bHoldOutput=false;
  m_iConvergedCnt=0;
  }

// ------------------------------------------------------------------------
// ========================================================================

TearPosBlk::TearPosBlk(byte DefaultTearType)
  {
  ListPos=List.AddHead(this);
  m_iRqdTearType=TT_NoTear;
  m_iTearType=DefaultTearType;//TT_NoTear;
  m_iTearPriority=TP_Normal;
  m_iTearInitWhen=TIW_OnInit;
  m_iTearInitHow=TIH_ZeroNIters;
  m_iTearInitEstUsage=TIEU_None;
  m_iTearHoldRqdCnt=1;
  m_iTearHoldCount=0;
  m_pVarBlk=NULL;
  }

// ------------------------------------------------------------------------

TearPosBlk::~TearPosBlk()
  {
  List.RemoveAt(ListPos);
  DeActivateTear();
  };

//--------------------------------------------------------------------------

void TearPosBlk::ConnectTear(flag CreateIfReqd)
  {
  Strng Tag;
  m_pVarBlk=TearVarBlk::Find(TearGetTag(Tag));
  if (m_pVarBlk==NULL && CreateIfReqd)
    m_pVarBlk=TearVarBlk::Add(this, Tag());
  if (m_pVarBlk)
    m_pVarBlk->Connect(this);//, CreateIfReqd);

  #if dbgEqnSolve
  if (dbgTearSetupAll())
    if (m_pVarBlk)
      dbgpln("ConnectTear() %s", Tag());
  #endif
  };

//--------------------------------------------------------------------------

void TearPosBlk::DisConnectTear()
  {
  if (m_pVarBlk)
    {
    m_pVarBlk->DisConnect();
    #if dbgEqnSolve
    if (dbgTearSetupAll())
      {
      Strng Tag;
      dbgpln("DisConnectTear() %s", TearGetTag(Tag));
      }
    #endif
    }
  m_pVarBlk=NULL;
  };

//--------------------------------------------------------------------------

void TearPosBlk::ActivateTear()
  {
  #if dbgEqnSolve
  if (dbgTearSetupAll())
    {
    Strng Tag;
    dbgpln("ActivateTear() %s", TearGetTag(Tag));
    }
  #endif
  Strng Tag;
  m_pVarBlk=TearVarBlk::Find(TearGetTag(Tag));
  if (m_pVarBlk==NULL)
    m_pVarBlk=TearVarBlk::Add(this, Tag());

  m_pVarBlk->Connect(this);//, False);

  m_pVarBlk->SetActive(True);
  };

//--------------------------------------------------------------------------

void TearPosBlk::DeActivateTear()
  {
  #if dbgEqnSolve
  if (dbgTearSetupAll())
    {
    Strng Tag;
    dbgpln("DeActivateTear() %s", TearGetTag(Tag));
    }
  #endif
  if (m_pVarBlk)
    m_pVarBlk->SetActive(False);
  };

//--------------------------------------------------------------------------

void TearPosBlk::SetTearInitCounters(bool AtStart)
  {
  if (RqdTearType()<TT_ManualTear)
    return;

  switch (TearInitWhen())
    {
    case TIW_OnInit:
      if (AtStart)
        return;
      break;
    case TIW_Always:
      m_iTearHoldCount  = -1;
      return;
    }

  switch (TearInitHow())
    {
    case TIH_ZeroNIters:
    case TIH_HoldNIters:
    case TIH_RampNIters:
      m_iTearHoldCount  = m_iTearHoldRqdCnt;
      break;
    }
  };

//--------------------------------------------------------------------------

bool TearPosBlk::TearInitActive()
  {
  return m_iTearHoldCount!=0;
  };

//--------------------------------------------------------------------------

void TearPosBlk::BumpTearInitCounters()
  {
  if (m_iTearHoldCount>0)
    m_iTearHoldCount--;
  };

//--------------------------------------------------------------------------

void TearPosBlk::SetTearInitActive(bool On, bool SetCount)
  {
  if (SetCount && TearInitWhen()!=TIW_Always)
    m_iTearHoldCount=m_iTearHoldRqdCnt;
  };

//--------------------------------------------------------------------------

double TearPosBlk::EstimatePortion()
  {
  if (m_iTearHoldCount<0)
    return 1.0;

  switch (TearInitHow())
    {
    case TIH_RampNIters:
      return Max(0.0, m_iTearHoldCount/(double)Max(1, m_iTearHoldRqdCnt));
    case TIH_HoldNIters:
    default:
      return TearInitActive() ? 1.0 : 0.0;
    }
  };

// ========================================================================
//
//
//
// ========================================================================

TearVarBlk *TearVarBlk::Find(char * Tag)
  {
  TearVarBlkIter TCBs(TearVarBlk::List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    if (TaggedObject::TagCmp(pT->Tag(), Tag))
      return pT;

  return NULL;
  };

//--------------------------------------------------------------------------

TearVarBlk *TearVarBlk::staticFindObjTag(pchar pSrchTag, flag SrchAll, int &ObjTagLen, int MinTagLen)
  {

  ObjTagLen=0;
  int P1=0,L=0;

  TearVarBlkIter TCBs(TearVarBlk::List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    if ((L=TaggedObject::TagCmp(pT->Tag(), &pSrchTag[P1], MinTagLen))>0)
      {
      ObjTagLen+=L;//=P1;
      P1+=L;
      return pT;
      }

  return NULL;
  };

// ------------------------------------------------------------------------

TearVarBlk *TearVarBlk::Add(TearPosBlk *Pos, char *Tag)
  {
  TearVarBlk *p=Pos->CreateVarBlk(Tag, NULL);//new TearVarBlk(Tag, NULL);
  if (List.IsEmpty())
    p->ListPos=List.AddHead(p);
  else
    {
    TearVarBlkIter TCBs(TearVarBlk::List);
    for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
      if (_stricmp(pT->Tag(), p->Tag())>0)
        break;
    if (pT)
      p->ListPos=List.InsertBefore(pT->ListPos, p);
    else
      p->ListPos=List.AddTail(p);
    }
  return p;
  };

// ------------------------------------------------------------------------

TearVarBlk *TearVarBlk::Add(TearVarBlk *Blk)
  {
  TearVarBlk *p=static_cast<TearVarBlk *>(Blk);//new TearVarBlk(Tag, NULL);
  if (List.IsEmpty())
    Blk->ListPos=List.AddHead(Blk);
  else
    {
    TearVarBlkIter TCBs(TearVarBlk::List);
    for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
      if (_stricmp(pT->Tag(), Blk->Tag())>0)
        break;
    if (pT)
      Blk->ListPos=List.InsertBefore(pT->ListPos, Blk);
    else
      Blk->ListPos=List.AddTail(Blk);
    }
  //p->Connect(p, False);
  return Blk;
  };

// ------------------------------------------------------------------------

void TearVarBlk::Delete(TearVarBlk *Var)
  {
  delete Var;
  };

// ========================================================================
//
//
//
// ========================================================================

#if KeepOldTearVarBlkEdit
IMPLEMENT_TAGOBJEDT(TearVarBlk, "TearBlk", "TbBase", "", "TBase", TOC_SYSTEM,
                   TearVarBlkEdt,
                   "TearBlock", "Tear Variable Calculation Block");
#else
IMPLEMENT_TAGOBJ(TearVarBlk, "TearBlk", "TbBase", "", "", "TBase", TOC_SYSTEM,
                   "TearBlock", "Tear Variable Calculation Block");
#endif

// ------------------------------------------------------------------------

TearVarBlk::TearVarBlk(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, Tag_, pAttach, TOA_Free)
  {
  CommonConstruct();
  }

// ------------------------------------------------------------------------

TearVarBlk::TearVarBlk(pchar Tag_, pTaggedObject pAttach) :
  TaggedObject(&TearVarBlkClass, Tag_, pAttach, TOA_Free)
  {
  CommonConstruct();
  }

// ------------------------------------------------------------------------

void TearVarBlk::CommonConstruct()
  {
  ListPos=NULL;
  m_dDampingRqd=dNAN;
  m_bHoldDamping=false;
  m_iEPSStrategy=TVBTS_Maximum;
  dEPS_Rel=dNAN;
  dEPS_Abs=dNAN;
  m_iDampAsGroup=DAG_Default;
  GrpDampFactor=0.0;
  fGroupBlk=False;
  pGroupInfo=NULL;
  bUsedByBlk=0;
  m_iTearMethod=TCM_NULL;
  iTearTagTyp=TTT_Unknown;

  pPosBlk=NULL;
  nVariables=0;
  nHistory=4;
  SetNVariables(0, 0);
  fInUse=False;
  m_lStageCnt=-2;
  m_lSeqConvergedCnt=0;
  m_lConvergeLoopCnt=0;
  #if dbgEqnSolve
  if (dbgTearSetupAll())
    dbgpln("Construct %s", FullObjTag());
  #endif
  }

// ------------------------------------------------------------------------

TearVarBlk::~TearVarBlk()
  {
  if (ListPos)
    List.RemoveAt(ListPos);
  #if dbgEqnSolve
  if (dbgTearSetupAll())
    dbgpln("Destroy %s", FullObjTag());
  #endif
  };

//--------------------------------------------------------------------------

char * TearMethodStrings[]={"Default", "DirectSubs", "AdaptSubs", "Wegstein", /*"Kalman",*/ 0};

DDBValueLst DDBTearMethod[]=
  {
    {TCM_NULL      ,  TearMethodStrings[TCM_NULL      ]},
    {TCM_DirectSubs,  TearMethodStrings[TCM_DirectSubs]},
    {TCM_AdaptSubs ,  TearMethodStrings[TCM_AdaptSubs ]},
    {TCM_Wegstein  ,  TearMethodStrings[TCM_Wegstein  ]},
   // {TCM_Kalman    ,  TearMethodStrings[TCM_Kalman    ]},
    {0}
  };

char * EPSStrategyStrings[]={"Maximum", "Minimum", "Priority", 0};

DDBValueLst DDBEPSStrategy[]=
  {
    {TVBTS_Maximum,  EPSStrategyStrings[TVBTS_Maximum]},
    {TVBTS_Minimum,  EPSStrategyStrings[TVBTS_Minimum]},
    {TVBTS_Priority, EPSStrategyStrings[TVBTS_Priority]},
    {0}
  };

char * DampAsGroupStrings[]={"-", "Yes", "Default", 0};
char * DampAsGroupStringsShort[]={"-", "Yes", "Def", 0};

DDBValueLst DDBDampAsGroup[]=
  {
    {DAG_Off,     DampAsGroupStrings[DAG_Off]      },
    {DAG_On,      DampAsGroupStrings[DAG_On]       },
    {0}
  };
DDBValueLst DDBDampAsGroupDef[]=
  {
    {DAG_Default, DampAsGroupStrings[DAG_Default]  },
    {DAG_Off,     DampAsGroupStrings[DAG_Off]      },
    {DAG_On,      DampAsGroupStrings[DAG_On]       },
    {0}
  };

void TearVarBlk::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this))
    {
    DDB.String("GroupTag",    "",    DC_,     "",     &sGroupTag,                 this, 0);//isParmConstruct);//0isParm);
    DDB.Long  ("NVars",       "",    DC_,     "",     xidNTearVars,               this, 0);//isParmConstruct);//0isParm);
    DDB.Byte  ("TearMethod",  "",    DC_,     "",     &m_iTearMethod,             this, isParm, DDBTearMethod);
    DDB.Long  ("NHistory",    "",    DC_,     "",     xidNTearHistory,            this, isParmStopped);
    DDB.Byte  ("DampAsGroup", "",    DC_,     "",     &m_iDampAsGroup,            this, isParm, DDBDampAsGroupDef);
    DDB.Double("DampingRqd",  "",    DC_Frac, "%",    &m_dDampingRqd,             this, isParm|NAN_OK);
    DDB.Byte  ("EPS_Strategy","",    DC_,     "",     &m_iEPSStrategy,            this, isParm, DDBEPSStrategy);
    DDB.Double("EPS_Abs",     "",    DC_Frac, "%",    &dEPS_Abs,                  this, isParm|NAN_OK);
    DDB.Double("EPS_Rel",     "",    DC_Frac, "%",    &dEPS_Rel,                  this, isParm|NAN_OK);
    DDB.Long  ("ConvergedCnt","",    DC_,     "",     &m_lSeqConvergedCnt,        this, isParmConstruct|InitHidden);//noFileAtAll);
    DDB.Long  ("ConvergeLoopCnt","", DC_,     "",     &m_lConvergeLoopCnt,        this, 0);//noFileAtAll);
    DDB.Long  ("Cnt",         "",    DC_,     "",     &m_lStageCnt,               this, isParmConstruct|InitHidden);//noFileAtAll);

    if (DDB.BeginArray(this, "V", "TearVars", nVariables))
      {
      for (int iVar=0; iVar<nVariables; iVar++)
        {
        TearVar & Ti=TV[iVar];
        if (DDB.BeginElement(this, SymOrTag(iVar)()))
          {
          DDB.String("Tag",             "",   DC_,         "",     &Ti.Tag,               this, noFileAtAll|InitHidden);
          DDB.String("Sym",             "",   DC_,         "",     &Ti.Sym,               this, noFileAtAll|InitHidden);
          DDB.String("Name",            "",   DC_,         "",     &Ti.Name,              this, InitHidden);
          DDB.Double("DampingRqd",      "",   DC_Frac,     "%",    &Ti.m_DampingRqd,        this, isParm|NAN_OK);
          DDB.Double("EPS_Abs",         "",   DC_Frac,     "%",    &Ti.EPS_Abs,           this, isParm|NAN_OK);
          DDB.Double("EPS_Rel",         "",   DC_Frac,     "%",    &Ti.EPS_Rel,           this, isParm|NAN_OK);
          DDB.Double("DampFactor",      "",   DC_,         "",     &Ti.m_DampFactor,        this, isParmConstruct|DDEF_NOCOMPARE);
          DDB.Double("EstDampFactor",   "",   DC_,         "",     &Ti.m_EstDampFactor,     this, isParmConstruct|InitHidden|DDEF_NOCOMPARE);
          DDB.Double("dYRat",           "",   DC_,         "",     &Ti.dYRat,             this, isParmConstruct|InitHidden|DDEF_NOCOMPARE);
          DDB.Bool  ("TestIsValid",     "",   DC_,         "",     &Ti.m_bTestIsValid,      this, isParmConstruct|InitHidden);
          DDB.Bool  ("HoldOutput",      "",   DC_,         "",     &Ti.m_bHoldOutput,       this, isParmConstruct|InitHidden);
          DDB.Byte  ("ConvergedCnt",    "",   DC_,         "",     &Ti.m_iConvergedCnt,     this, isParmConstruct|InitHidden);
          //DDB.Byte  ("Cnt",             "",   DC_,         "",     &Ti.iCnt,              this, isParmConstruct|InitHidden);

          if (bUsedByBlk & TVU_Inputs)
            {
            CnvAttribute &C=*Ti.AdvCnv;
            FmtAttribute &F=*Ti.AdvFmt;
            DDB.Double("Y0",             "",   C.Index(), C.Text(), &Ti.Y[0],       this, isParmConstruct|DDEF_NOCOMPARE);
            DDB.Double("Y1",             "",   C.Index(), C.Text(), &Ti.Y[1],       this, isParmConstruct|InitHidden|NAN_OK|DDEF_NOCOMPARE);
            DDB.Double("Y2",             "",   C.Index(), C.Text(), &Ti.Y[2],       this, isParmConstruct|InitHidden|NAN_OK|DDEF_NOCOMPARE);
            DDB.Double("Y3",             "",   C.Index(), C.Text(), &Ti.Y[3],       this, isParmConstruct|InitHidden|NAN_OK|DDEF_NOCOMPARE);
            }
          if (bUsedByBlk & TVU_Outputs)
            {
            CnvAttribute &C=*Ti.AdvCnv;
            FmtAttribute &F=*Ti.AdvFmt;
            DDB.Double("X0",             "",   C.Index(), C.Text(), &Ti.X[0],       this, isParmConstruct|DDEF_NOCOMPARE);//isParmConstruct);
            DDB.Double("X1",             "",   C.Index(), C.Text(), &Ti.X[1],       this, isParmConstruct/*isParmConstruct*/|InitHidden|NAN_OK|DDEF_NOCOMPARE);
            DDB.Double("X2",             "",   C.Index(), C.Text(), &Ti.X[2],       this, isParmConstruct|InitHidden|NAN_OK|DDEF_NOCOMPARE);
            DDB.Double("X3",             "",   C.Index(), C.Text(), &Ti.X[3],       this, isParmConstruct|InitHidden|NAN_OK|DDEF_NOCOMPARE);
            }

          if (bUsedByBlk & TVU_Meas)
            {
            CnvAttribute &C=*Ti.MeasCnv;
            FmtAttribute &F=*Ti.MeasFmt;
            DDB.Double("CurTol",         "",   DC_Frac,     "%",    &Ti.CurTol,    this, isParmConstruct|InitHidden);
            for (int iH=0; iH<Ti.m_Error.GetSize(); iH++)
              {
              Strng Tg1;
              Tg1.Set("Err%i",iH);
              DDB.Double(Tg1(),          "",   DC_Frac,     "%",    &Ti.m_Error[iH],  this, isParmConstruct|InitHidden);
              }
            for (iH=0; iH<Ti.m_Error.GetSize(); iH++)
              {
              Strng Tg1;
              Tg1.Set("Meas%i",iH);
              DDB.Double(Tg1(),          "",   C.Index(), C.Text(), &Ti.m_Meas[iH],   this, isParmConstruct|InitHidden);
              }
            }
          }
        }
      }
    DDB.EndArray();
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag TearVarBlk::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidNTearVars:
      {
//      if (DCB.rL)
//        SetNVariables(*DCB.rL);
      DCB.L=NVariables();
      return 1;
      }
    case xidNTearHistory:
      {
      if (DCB.rL)
        SetNHistory(*DCB.rL);
      DCB.L=NHistory();
      return 1;
      }
    }

  return 0;
  };

//--------------------------------------------------------------------------

flag TearVarBlk::ValidateData(ValidateDataBlk & VDB)
  {
  #if dbgEqnSolve
  if (dbgTearSetupAll())
    {
    dbgpln("%s=======================", FullObjTag());
    dbgpln("GroupName   %s", sGroupTag());
    dbgpln("NVars       %i", nVariables);
    dbgpln("NHistory    %i", nHistory);
    dbgpln("DampAsGroup %i", m_iDampAsGroup);
    dbgpln("DampingRqd  %g", m_dDampingRqd);
    dbgpln("TolStrategy %i", m_iEPSStrategy);
    dbgpln("EPS_Abs     %g", dEPS_Abs);
    dbgpln("EPS_Rel     %g", dEPS_Rel);

    for (int iVar=0; iVar<nVariables; iVar++)
      {
      if (Valid(TV[iVar].m_DampingRqd)) dbgp("%8.2f",  TV[iVar].m_DampingRqd); else dbgp("%8s",  "*");
      if (Valid(TV[iVar].EPS_Abs))    dbgp(" %8.2f", TV[iVar].EPS_Abs);    else dbgp(" %8s", "*");
      if (Valid(TV[iVar].EPS_Rel))    dbgp(" %8.2f", TV[iVar].EPS_Rel);    else dbgp(" %8s", "*");
      dbgp(" %8.2f", TV[iVar].m_DampFactor);
      dbgp(" %8.2f", TV[iVar].m_EstDampFactor);
      if (1)//fDoAdvance)
        {
        dbgp(" %8.2f", TV[iVar].X[0]);
        dbgp(" %8.2f", TV[iVar].Y[0]);
        }
      dbgp(" %s", SymOrTag(iVar));
      dbgpln("");
      }
    }
  #endif
  return True;
  };

// ------------------------------------------------------------------------

void TearVarBlk::SetActive(int On)
  {
  TaggedObject::SetActive(On);
  };

// ------------------------------------------------------------------------

char* TearVarBlk::LHSTag(Strng & Tg)
  {
  Tg=Tag();
  switch (iTearTagTyp)
    {
    case TTT_Unknown: Tg = ""; break;
    case TTT_Tear:
      {
      int index = Tg.Find("//");
      if (index>0)
        Tg = Tg.Left(index);
      break;
      }
    case TTT_FlashTrain:
      if (Tg.Len()>4)
        Tg = Tg.Left(Tg.Len()-4);
      break;
    case TTT_PID: Tg = "todo"; break;
    default: Tg = "?"; break;
    }
  return Tg();
  }

// ------------------------------------------------------------------------

char* TearVarBlk::RHSTag(Strng & Tg)
  {
  Tg=Tag();
  switch (iTearTagTyp)
    {
    case TTT_Tear:
      {
      int index = Tg.Find("//");
      if (index>0)
        Tg = Tg.Mid(index+2, Tg.Len()-index-3);
      else
        Tg = Tg.Left(Tg.Len()-1);
      break;
      }
    case TTT_Unknown:
    case TTT_FlashTrain:
    case TTT_PID: Tg = ""; break;
    default: Tg = "?"; break;
    }
  return Tg();
  }

// ------------------------------------------------------------------------

void TearVarBlk::SetNVariables(int Variables, byte Used)
  {
  bUsedByBlk = Used;

  if (nVariables!=Variables)
    {
    nVariables=Variables;
    nHistory=Max(nHistory, 4L);

    TV.SetSize(nVariables);
    for (int i=0; i<nVariables; i++)
      {

      TV[i].m_DampFactor=1.0;
      TV[i].m_EstDampFactor=0.0;
      TV[i].m_DampingRqd=dNAN;
      TV[i].EPS_Rel=dNAN;
      TV[i].EPS_Abs=dNAN;

      if (bUsedByBlk & (TVU_Inputs|TVU_Outputs))
        {
        TV[i].X.SetSize(nHistory);
        TV[i].Y.SetSize(nHistory);

        TV[i].X.SetAll(0.0);
        TV[i].Y.SetAll(0.0);
        TV[i].dYRat=0.0;
        }
      else
        {
        TV[i].X.SetSize(0);
        TV[i].Y.SetSize(0);
        }

      if (bUsedByBlk & (TVU_Meas))
        {
        TV[i].m_Meas.SetSize(nHistory);
        TV[i].m_Error.SetSize(nHistory);
        TV[i].m_Meas.SetAll(0.0);
        TV[i].m_Error.SetAll(0.0);
        }
      else
        {
        TV[i].m_Meas.SetSize(0);
        TV[i].m_Error.SetSize(0);
        }

      }

    GrpDampFactor=0.0;
    }
  };

// ------------------------------------------------------------------------

void TearVarBlk::SetNHistory(int History)
  {
  if (nHistory!=History)
    {
    int Orig=nHistory;
    nHistory=Max(History, 4);
    for (int i=0; i<nVariables; i++)
      {

      if (bUsedByBlk & (TVU_Inputs|TVU_Outputs))//TearAdvanceReqd())
        {
        TV[i].X.SetSize(nHistory);
        TV[i].Y.SetSize(nHistory);

        if (Orig>0)
          {
          for (int j=Orig; j<nHistory; j++)
            {
            TV[i].X[j]=TV[i].X[j-1];
            TV[i].Y[j]=TV[i].Y[j-1];
            }
          }
        }
      else
        {
        TV[i].X.SetSize(0);
        TV[i].Y.SetSize(0);
        }

      if (bUsedByBlk & (TVU_Meas))//TearConvergeReqd())
        {
        TV[i].m_Meas.SetSize(nHistory);
        TV[i].m_Error.SetSize(nHistory);
        if (Orig>0)
          {
          for (int j=Orig; j<nHistory; j++)
            {
            TV[i].m_Meas[j]=TV[i].m_Meas[j-1];
            TV[i].m_Error[j]=TV[i].m_Error[j-1];
            }
          }
        }
      else
        {
        TV[i].m_Meas.SetSize(0);
        TV[i].m_Error.SetSize(0);
        }
      }
    }
  };

//--------------------------------------------------------------------------

void TearVarBlk::Initialise(bool ForceIt)
  {
  if (m_lStageCnt<-1 || ForceIt)
    {
    for (int iVar=0; iVar<nVariables; iVar++)
      TV[iVar].m_iConvergedCnt=0;
    m_lStageCnt=-1;
    m_lSeqConvergedCnt=0;
    }
  }

//--------------------------------------------------------------------------

void TearVarBlk::Connect(pTearPosBlk PosBlk)
  {
  pPosBlk=PosBlk;
  Initialise(false);
  for (int i=0; i<nVariables; i++)
    TV[i].Name=GetDisplayTag(TV[i].Tag, TV[i].Sym);

  if (fGroupBlk)
    {
    pGroupInfo=NULL;
    }
  else
    {
    Strng GrpTag;
    GetGroupGlblTag(GrpTag);
    pGroupInfo=TearVarBlk::Find(GrpTag());
    if (pGroupInfo==NULL)
      pGroupInfo=TearVarBlk::Add(PosBlk, GrpTag());

    pGroupInfo->fGroupBlk=True;
    pGroupInfo->Connect(NULL/*PosBlk*/);
    }

  }

//--------------------------------------------------------------------------

void TearVarBlk::DisConnect()
  {
  pPosBlk=NULL;
  pGroupInfo=NULL;
  }

//--------------------------------------------------------------------------

double TearVarBlk::DampingRqd(EqnSlvCtrlBlk & EqnCB, int iVar)
  {
  double R=0.0;
  if (Valid(TV[iVar].m_DampingRqd))
    R=Max(R, TV[iVar].m_DampingRqd);
  if (pGroupInfo && Valid(pGroupInfo->TV[iVar].m_DampingRqd))
    R=Max(R, pGroupInfo->TV[iVar].m_DampingRqd);
  if (Valid(m_dDampingRqd))
    R=Max(R, m_dDampingRqd);
  R=Max(R, EqnCB.Cfg.m_dDamping);
  return Range(0.0, R, 1.0);
  };

//--------------------------------------------------------------------------

double TearVarBlk::EPS_Abs(EqnSlvCtrlBlk & EqnCB, int iVar)
  {
  double R=0.0;
  switch (m_iEPSStrategy)
    {
    case TVBTS_Priority:
      if (Valid(TV[iVar].EPS_Abs))
        R=TV[iVar].EPS_Abs;
      else if (pGroupInfo && Valid(pGroupInfo->TV[iVar].EPS_Abs))
        R=pGroupInfo->TV[iVar].EPS_Abs;
      else if (Valid(dEPS_Abs))
        R=dEPS_Abs;
      else
        R=EqnCB.Cfg.dEps_Abs;
      break;
    case TVBTS_Minimum:
      R=1.0;
      if (Valid(TV[iVar].EPS_Abs))
        R=Min(R, TV[iVar].EPS_Abs);
      if (pGroupInfo && Valid(pGroupInfo->TV[iVar].EPS_Abs))
        R=Min(R, pGroupInfo->TV[iVar].EPS_Abs);
      if (Valid(dEPS_Abs))
        R=Min(R, dEPS_Abs);
      R=Min(R, EqnCB.Cfg.dEps_Abs);
      break;
    case TVBTS_Maximum:
    default:
      R=0.0;
      if (Valid(TV[iVar].EPS_Abs))
        R=Max(R, TV[iVar].EPS_Abs);
      if (pGroupInfo && Valid(pGroupInfo->TV[iVar].EPS_Abs))
        R=Max(R, pGroupInfo->TV[iVar].EPS_Abs);
      if (Valid(dEPS_Abs))
        R=Max(R, dEPS_Abs);
      R=Max(R, EqnCB.Cfg.dEps_Abs);
      break;
    }
  return Range(0.0, R, 1.0);
  };

//--------------------------------------------------------------------------

double TearVarBlk::EPS_Rel(EqnSlvCtrlBlk & EqnCB, int iVar)
  {
  double R=0.0;
  switch (m_iEPSStrategy)
    {
    case TVBTS_Priority:
      if (Valid(TV[iVar].EPS_Rel))
        R=TV[iVar].EPS_Rel;
      else if (pGroupInfo && Valid(pGroupInfo->TV[iVar].EPS_Rel))
        R=pGroupInfo->TV[iVar].EPS_Rel;
      else if (Valid(dEPS_Rel))
        R=dEPS_Rel;
      else
        R=EqnCB.Cfg.dEps_Rel;
      break;
    case TVBTS_Minimum:
      R=1.0;
      if (Valid(TV[iVar].EPS_Rel))
        R=Min(R, TV[iVar].EPS_Rel);
      if (pGroupInfo && Valid(pGroupInfo->TV[iVar].EPS_Rel))
        R=Min(R, pGroupInfo->TV[iVar].EPS_Rel);
      if (Valid(dEPS_Rel))
        R=Min(R, dEPS_Rel);
      R=Min(R, EqnCB.Cfg.dEps_Rel);
      break;
    case TVBTS_Maximum:
    default:
      R=0.0;
      if (Valid(TV[iVar].EPS_Rel))
        R=Max(R, TV[iVar].EPS_Rel);
      if (pGroupInfo && Valid(pGroupInfo->TV[iVar].EPS_Rel))
        R=Max(R, pGroupInfo->TV[iVar].EPS_Rel);
      if (Valid(dEPS_Rel))
        R=Max(R, dEPS_Rel);
      R=Max(R, EqnCB.Cfg.dEps_Rel);
      break;
    }
  return Range(0.0, R, 1.0);
  };

//--------------------------------------------------------------------------

void TearVarBlk::BeginStep(EqnSlvCtrlBlk & EqnCB)
  {
  };

//--------------------------------------------------------------------------

void TearVarBlk::EndStep(EqnSlvCtrlBlk & EqnCB)
  {
  };

//--------------------------------------------------------------------------

flag TearVarBlk::TestConverged(EqnSlvCtrlBlk & EqnCB, double &Error, Strng &ErrTag)
  {
  Error=0.0;

  int iVar;
  Strng Tag;
  switch (TearGetConvergeInfo(/*TV*/))
    {
    case -2:// consider the Var Blk to be NOT converged
      EqnCB.SetConverged(False);
      return false;
    case -1:// consider the Var Blk to be converged
      return true;
    case 0: // Errors Not supplied
      for (iVar=0; iVar<nVariables; iVar++)
        {
        TV[iVar].m_Error[0] = (TV[iVar].Y[0]-TV[iVar].X[0]);
        TV[iVar].m_Meas[0]  = TV[iVar].Y[0];
        }
      break;
    case 1: // Data Supplied
      break;
    }

  flag TearConverged=!TearInitActive();
  for (iVar=0; iVar<nVariables; iVar++)
    {
    EqnCB.BumpNUnknowns();
    double Err=0.0;
    double RelTol=EPS_Rel(EqnCB, iVar);
    double AbsTol=EPS_Abs(EqnCB, iVar);
    TearVar &TVi=TV[iVar];
    flag Converged=ConvergedDV(TVi.m_Error[0], TVi.m_Meas[0], AbsTol, RelTol, Err);
    TVi.m_iConvergedCnt=(Converged ? (byte)Min(255, TVi.m_iConvergedCnt+1) : 0);
    if (TVi.m_bTestIsValid && !Converged)
    //if (TVi.m_bTestIsValid && !ConvergedDV(TVi.m_Error[0], TVi.m_Meas[0], AbsTol, RelTol, Err))
      {
      TearConverged=False;
      if (Err>Error)
        {
        Error=Err;
        ErrTag=TVi.Sym;
        if (ErrTag.GetLength()==0)
          ErrTag=TVi.Tag;
        }
      if (EqnCB.NoteBadEqnError(Err, RelTol))
        {
        Tag.Set("%s.%s", FullObjTag(), SymOrTag(iVar));
        EqnCB.CollectBadEqnError(Err, TVi.m_Meas[0], AbsTol, RelTol, TearAdvanceReqd() ? TVi.m_DampFactor : -1, Tag(), iTearTagTyp);
        }
      }
    else if (EqnCB.NoteWorstError(Err, RelTol))
      {
      Tag.Set("%s.%s", FullObjTag(), SymOrTag(iVar));
      EqnCB.CollectWorstError(Err, TVi.m_Meas[0], AbsTol, RelTol, TearAdvanceReqd() ? TVi.m_DampFactor : -1, Tag(), iTearTagTyp);
      }

    TVi.m_Error[0]=Err;//*RelTol;
    TVi.CurTol=RelTol;
    }

  if (!TearConverged)
    EqnCB.SetConverged(False);
  m_lSeqConvergedCnt=(TearConverged ? m_lSeqConvergedCnt+1 : 0);
  //if (lConvergedCnt<EqnCB.Cfg.nRqdCnvrgdIters)
  //  EqnCB.SetConverged(False);
  return TearConverged;
  }

//--------------------------------------------------------------------------

void TearVarBlk::AdvDirect(EqnSlvCtrlBlk & EqnCB, double Damping)
  {
  EqnSlvCfgBlk &Cfg=EqnCB.Cfg;

  for (int iVar=0; iVar<nVariables; iVar++)
    {
    TV[iVar].m_DampFactor=Max(Damping, DampingRqd(EqnCB, iVar));
    double d=m_bHoldDamping ? 0 : TV[iVar].m_DampFactor;
    if (!TV[iVar].m_bHoldOutput)
      TV[iVar].X[0]=(d)*TV[iVar].X[1]+(1.0-d)*TV[iVar].Y[0];

    #if dbgEqnSolve
    double Err=0.0;
    if (dbgTearCnvgLoop())
      if (dbgShowAll() || /*lCnt==0 ||*/ !ConvergedDV(TV[iVar].Y[0]-TV[iVar].X[1], TV[iVar].Y[0], EqnCB.Cfg.dEps_Abs, EqnCB.Cfg.dEps_Rel, Err))
        {
        Strng Tag;
        Tag.Set("%s.%s", FullObjTag(), SymOrTag(iVar));
        dbgpln("%s %3i %8.3f%% DirctSubs %6.2f%% I:%14.6g  O:%14.6g >> %14.6g : %s",
               iVar==0 ? "---" : "   ",
               iVar, Range(-999.999, 100.0*(TV[iVar].X[0]-TV[iVar].X[1])/NZ(TV[iVar].X[1]), 999.999),
		           TV[iVar].m_DampFactor*100.0,TV[iVar].Y[0], TV[iVar].X[1], TV[iVar].X[0], Tag());
        }
    #endif
    }
  m_lStageCnt++;
  }

//--------------------------------------------------------------------------
#ifndef _RELEASE
Strng TrackTag;
bool DoneHeader=false;
bool DoTrack=false;
bool AllowDoTrack=false;
#endif

void TearVarBlk::AdvAdaptDirect(EqnSlvCtrlBlk & EqnCB, double Damping)
  {
  EqnSlvCfgBlk &Cfg=EqnCB.Cfg;

#ifndef _RELEASE
  DoTrack=(AllowDoTrack && TrackTag.Len()>0);
  if (AllowDoTrack && TrackTag.Len()==0 || !DoTrack)
    {
    TrackTag = "A653R03//A653SPL19#.V[Na2CO3(l)]";
    DoneHeader=false;
    }
#endif
  GrpDampFactor=0.0;
  bool DampGrp=((m_iDampAsGroup==DAG_Default) ? EqnCB.Cfg.m_iDampAsGroup : m_iDampAsGroup)==DAG_On;
  for (int iVar=0; iVar<nVariables; iVar++)
    {
#ifndef _RELEASE
    if (DoTrack && _stricmp(TV[iVar].Tag(), TrackTag())==0)
      {
      Strng ss;
      if (!DoneHeader)
        {
        DoneHeader=true;
        ss.Set("TrackTag:%s   DampFctGrowth:%18.10g   DampFctDecay:%18.10g", TrackTag(), EqnCB.Cfg.dDampFctGrowth, EqnCB.Cfg.dDampFctDecay);
        LogNote(TrackTag(), 0, ss());
        dbgpln(ss());
        ss="Iter,B/A,Y2,Y1,Y0,X2,X1,X0,YRat,EstDampFactor,DampFactor";
        LogNote(TrackTag(), 0, ss());
        dbgpln(ss());
        }
      ss.Set("%d,Before,%g,%g,%g,%g,%g,%g,%g,%g,%g", m_lStageCnt, TV[iVar].Y[2], TV[iVar].Y[1], TV[iVar].Y[0],
            TV[iVar].X[2], TV[iVar].X[1], TV[iVar].X[0], TV[iVar].dYRat, TV[iVar].m_EstDampFactor, TV[iVar].m_DampFactor);
      LogNote(TrackTag(), 0, ss());
      dbgpln(ss());
      }
#endif
    if (TstNZ(TV[iVar].Y[1]-TV[iVar].Y[2]))
      {
      TV[iVar].dYRat=(TV[iVar].Y[0]-TV[iVar].Y[1])/NZ(TV[iVar].Y[1]-TV[iVar].Y[2]);
      if (TV[iVar].dYRat<-0.3) // Oscillatory Damp More
        TV[iVar].m_EstDampFactor=Min(0.99, TV[iVar].m_EstDampFactor+(1.0-TV[iVar].m_EstDampFactor)*EqnCB.Cfg.dDampFctGrowth);
      else if (TV[iVar].dYRat>0.3)// Damp Less
        TV[iVar].m_EstDampFactor=TV[iVar].m_EstDampFactor/(1.0+EqnCB.Cfg.dDampFctDecay);
      else // Dont Change Damping
        {};
      }
    else
      {
      TV[iVar].dYRat=0.0;
      TV[iVar].m_EstDampFactor=TV[iVar].m_EstDampFactor/(1.0+EqnCB.Cfg.dDampFctDecay);
      }

    double DFMin=Max(DampingRqd(EqnCB, iVar), Damping);
    TV[iVar].m_EstDampFactor=Range(DFMin, TV[iVar].m_EstDampFactor, 1.0);

    if (DampGrp && (fabs(TV[iVar].Y[0])>1.0e-10))
      GrpDampFactor=Max(GrpDampFactor, TV[iVar].m_EstDampFactor);
    }

  for (iVar=0; iVar<nVariables; iVar++)
    {
    if (DampGrp)
      TV[iVar].m_DampFactor=GrpDampFactor;
    else
      TV[iVar].m_DampFactor=TV[iVar].m_EstDampFactor;
    TV[iVar].m_DampFactor=Max(Damping, TV[iVar].m_DampFactor);

    double d=m_bHoldDamping ? 0 : TV[iVar].m_DampFactor;
    if (!TV[iVar].m_bHoldOutput)
      TV[iVar].X[0]=(d)*TV[iVar].X[1] + (1.0-d)*TV[iVar].Y[0];

    #if dbgEqnSolve
    double Err=0.0;
    if (dbgTearCnvgLoop())
      if (dbgShowAll() || !ConvergedDV(TV[iVar].Y[0]-TV[iVar].X[1], TV[iVar].Y[0], EqnCB.Cfg.dEps_Abs, EqnCB.Cfg.dEps_Rel, Err))
        {
        Strng Tag;
        Tag.Set("%s.%s", FullObjTag(), SymOrTag(iVar));
        dbgpln("%s %3i %8.3f%% AdptDSubs e:%6.2f%% d:%6.2f%% dI2:%18.10g I:%18.10g  O:%18.10g >> %18.10g : %s",
          iVar==0 ? "---" : "   ",
          iVar, Range(-999.999, 100.0*(TV[iVar].X[0]-TV[iVar].X[1])/NZ(TV[iVar].X[1]), 999.999),
          TV[iVar].m_EstDampFactor*100.0,TV[iVar].m_DampFactor*100.0, TV[iVar].dYRat,TV[iVar].Y[0],
          TV[iVar].X[1], TV[iVar].X[0], Tag());
        }
    #endif

#ifndef _RELEASE
    if (DoTrack && _stricmp(TV[iVar].Tag(), TrackTag())==0)
      {
      Strng ss;
      ss.Set("%d,After,%g,%g,%g,%g,%g,%g,%g,%g,%g", m_lStageCnt, TV[iVar].Y[2], TV[iVar].Y[1], TV[iVar].Y[0],
            TV[iVar].X[2], TV[iVar].X[1], TV[iVar].X[0], TV[iVar].dYRat, TV[iVar].m_EstDampFactor, TV[iVar].m_DampFactor);
      LogNote(TrackTag(), 0, ss());
      dbgpln(ss());
      /*Strng Tag;
      Tag.Set("%s.%s", FullObjTag(), SymOrTag(iVar));
      Strng ss;
      ss.Set("%4i %8.3f%% AdptDSubs e:%6.2f%% d:%5.2f%% dI2:%18.10g I:%18.10g  O:%18.10g >> %18.10g : %s",
        lCnt, Range(-999.999, 100.0*(TV[iVar].X[0]-TV[iVar].X[1])/NZ(TV[iVar].X[1]), 999.999),
        TV[iVar].m_EstDampFactor*100.0, TV[iVar].m_DampFactor*100.0, TV[iVar].dYRat, TV[iVar].Y[0],
        TV[iVar].X[1], TV[iVar].X[0], Tag());
      LogNote("Solver", 0, ss());
      dbgpln(ss());*/
      }
#endif
    }
  m_lStageCnt++;
  }

//--------------------------------------------------------------------------

void TearVarBlk::AdvWegstein(EqnSlvCtrlBlk & EqnCB)
  {
  EqnSlvCfgBlk &Cfg=EqnCB.Cfg;

  for (int iVar=0; iVar<nVariables; iVar++)
    {
    double S=(TV[iVar].X[1]-TV[iVar].X[2])/NZ(TV[iVar].X[2]-TV[iVar].X[3]);
    //double NewQ=Range(Cfg.dWA_Bound, S/NZ(S-1), 0.9);
    //if (Q[iVar] > 0.0) // Interpolation
    //  Q[iVar]=Max(0.0, Max(NewQ, Q[iVar])); // Keep As Interpolation
    //else
    //  Q[iVar]=NewQ;
    double Q=Range(Cfg.dWA_Bound, S/NZ(S-1), 0.0);

    TV[iVar].m_DampFactor=0.0;
    if (m_bHoldDamping)
      Q=0;
    if (!TV[iVar].m_bHoldOutput)
      TV[iVar].X[0]=Q*TV[iVar].X[1] + (1.0-Q)*TV[iVar].Y[0];

    #if dbgEqnSolve
    double Err=0.0;
    if (dbgTearCnvgLoop())
      {
      if (dbgShowAll() || m_lStageCnt==0 || !ConvergedDV(TV[iVar].Y[0]-TV[iVar].X[1], TV[iVar].Y[0], EqnCB.Cfg.dEps_Abs, EqnCB.Cfg.dEps_Rel, Err))
        {
        Strng Tag;
        Tag.Set("%s.%s", FullObjTag(), SymOrTag(iVar));
        dbgpln("%s %3i %8.3f%% Wegstein  %6.2f%% I:%14.6g  O:%14.6g >> %14.6g  S:%14.6g : %s",
             iVar==0 ? "---" : "   ",
             iVar, Range(-999.999, 100.0*(TV[iVar].X[0]-TV[iVar].X[1])/NZ(TV[iVar].X[1]), 999.999),(1.0-Q)*100.0,TV[iVar].Y[0], TV[iVar].X[1], TV[iVar].X[0], S, Tag());
        //if (fabs(TV[iVar].X[0]-TV[iVar].X[1]) > fabs(TV[iVar].X[0])*0.01)
        //  dbgpln("     BIGDELTA X3:%14.6g X2:%14.6g X1:%14.6g e1:%14.6g e0:%14.6g : %s",
        //         X3,X2,X1,e1,e0, Tags[iVar].Str());
        }
      }
    #endif
    }
  m_lStageCnt=0;
  }

//--------------------------------------------------------------------------

void TearVarBlk::RotateInputs()
  {
  // Rotate Inputs
  for (int iVar=0; iVar<nVariables; iVar++)
    {
    CDArray &Y=TV[iVar].Y;
    double T=Y[nHistory-1];
    for (int i=nHistory-1; i>0; i--)
      Y[i]=Y[i-1];
    Y[0]=T;
    }
  };

//--------------------------------------------------------------------------

void TearVarBlk::RotateErrors()
  {
  // Rotate Errors
  for (int iVar=0; iVar<nVariables; iVar++)
    {
    CDArray &Error=TV[iVar].m_Error;
    double T=Error[nHistory-1];
    for (int i=nHistory-1; i>0; i--)
      Error[i]=Error[i-1];
    Error[0]=T;
    CDArray &Meas=TV[iVar].m_Meas;
    T=Meas[nHistory-1];
    for (i=nHistory-1; i>0; i--)
      Meas[i]=Meas[i-1];
    Meas[0]=T;
    }
  };

//--------------------------------------------------------------------------

void TearVarBlk::RotateOutputs()
  {
  // Rotate Outputs
  for (int iVar=0; iVar<nVariables; iVar++)
    if (!TV[iVar].m_bHoldOutput)
      {
      CDArray &X=TV[iVar].X;
      double T=X[nHistory-1];
      for (int i=nHistory-1; i>0; i--)
        X[i]=X[i-1];
      X[0]=T;
      }
  };

//--------------------------------------------------------------------------

void TearVarBlk::Advance(EqnSlvCtrlBlk & EqnCB)
  {
  if (TearAdvance(/*TV, */ EqnCB))
    return;

  EqnSlvCfgBlk &Cfg=EqnCB.Cfg;
  if (m_lStageCnt<0) // First Pass
    {
    m_lStageCnt=0;
    for (int iVar=0; iVar<nVariables; iVar++)
	    {
      CDArray &X=TV[iVar].X;
      CDArray &Y=TV[iVar].Y;
      if (!Valid(X[1])) X[1]=X[0];
      if (!Valid(X[2])) X[2]=X[0];
      if (!Valid(X[3])) X[3]=X[0];
      if (!Valid(Y[1])) Y[1]=Y[0];
      if (!Valid(Y[2])) Y[2]=Y[0];
      if (!Valid(Y[3])) Y[3]=Y[0];
	    }
    }
  byte Meth=m_iTearMethod;
  if (Meth==TCM_NULL)
    Meth=Cfg.iConvergeMeth;

  switch (Meth)
    {
    case TCM_Wegstein:
      if ((m_lStageCnt < Cfg.iWA_Delay) || (m_lStageCnt<2))
        AdvDirect(EqnCB, 0.0);
      else
        AdvWegstein(EqnCB);
      break;
    case TCM_AdaptSubs:
      AdvAdaptDirect(EqnCB, EqnCB.Cfg.m_dDamping);
      break;
    case TCM_DirectSubs:
    default:
      AdvDirect(EqnCB, EqnCB.Cfg.m_dDamping);
      break;
    }
  m_bHoldDamping=false;
  }

//===========================================================================
//
//
//
//===========================================================================

TearVarBlkList TearVarBlk::List;
TearPosBlkList TearPosBlk::List;

//==========================================================================
//
//
//
//==========================================================================

#if KeepOldTearVarBlkEdit

const int Id_DampAsGroup     =  1;
const int Id_BlkDamp         =  2;
const int Id_BlkEPSStrat     =  3;
const int Id_BlkEPSRel       =  4;
const int Id_BlkEPSAbs       =  5;
const int Id_TearMeth        =  6;
const int Id_GlblDamp        = 10;
const int Id_GlblEPSRel      = 11;
const int Id_GlblEPSAbs      = 12;
const int Id_EPSRel          = 13;
const int Id_EPSAbs          = 14;
const int Id_CurTol          = 15;
const int Id_RqdDamp         = 16;
const int Id_CurDamp         = 17;
const int Id_Inputs          = 18;
const int Id_Outputs         = 19;
const int Id_Diff            = 20;
const int Id_Meas            = 21;
const int Id_ConvergedCnt    = 22;

//==========================================================================

TearVarBlkEdt::TearVarBlkEdt(FxdEdtView *pView_, TearVarBlk *pTVB_) :
  FxdEdtBookRef(pView_),
  rTVB(*pTVB_)
  {
  DampCnv.Set(DC_Frac, "%");
  MeasCnv.Set(DC_, "");
  DiffCnv.Set(DC_Frac, "%");
  EPSCnv.Set(DC_Frac, "%");
  DampFmt.Set("", 0, 2, 'f');
  MeasFmt.Set("", 0, 2, 'f');
  DiffFmt.Set("", 0, 2, 'f');
  EPSFmt.Set("", 0, 2, 'f');

  iDragPtNo = -1;
  iNameWidth=5;

  iPg1=0;

  //CProfINIFile PF(PrjIniFile());

  ObjectAttribute *pAttr=ObjAttributes.FindObject("TearEdt");
  if (pAttr)
    {
    pAttr->FieldFmtCnvs("Damp", DampFmt, DampCnv);
    pAttr->FieldFmtCnvs("Meas", MeasFmt, MeasCnv);
    pAttr->FieldFmtCnvs("Diff", DiffFmt, DiffCnv);
    //pAttr->FieldRange("Temp", dXDispMin, dXDispMax);
    pAttr->FieldFmtCnvs("EPS", EPSFmt, EPSCnv);
    }

  rTVB.OnEditConstruct();

  }

//---------------------------------------------------------------------------

TearVarBlkEdt::~TearVarBlkEdt()
  {
  rTVB.OnEditDestroy();

  ObjectAttribute *pAttr=ObjAttributes.FindObject("TearEdt");
  if (pAttr)
    {
    pAttr->SetFieldFmt("Damp", DampFmt);
    pAttr->SetFieldCnvs("Damp", DampCnv);
    //pAttr->SetFieldMin("Damp", dXDispMin);
    //pAttr->SetFieldMax("Damp", dXDispMax);
    pAttr->SetFieldFmt("Meas", MeasFmt);
    pAttr->SetFieldCnvs("Meas", MeasCnv);
    pAttr->SetFieldFmt("Diff", DiffFmt);
    pAttr->SetFieldCnvs("Diff", DiffCnv);
    //pAttr->SetFieldMin("Diff", dXDispMin);
    //pAttr->SetFieldMax("Diff", dXDispMax);
    pAttr->SetFieldFmt("EPS", EPSFmt);
    pAttr->SetFieldCnvs("EPS", EPSCnv);
    }
  }

//---------------------------------------------------------------------------

void TearVarBlkEdt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void TearVarBlkEdt::PutDataDone()
  {
  }

//---------------------------------------------------------------------------

void TearVarBlkEdt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void TearVarBlkEdt::GetDataDone()
  {
  }

//---------------------------------------------------------------------------

void TearVarBlkEdt::StartBuild()
  {

  }

//---------------------------------------------------------------------------

void TearVarBlkEdt::Build()
  {
  Strng S;
  Strng GrpTag;
  rTVB.GetGroupGlblTag(GrpTag);

  TearVarBlk * pGroupInfo=TearVarBlk::Find(GrpTag());

  TearVarArray &TV = rTVB.TV;

  iPg1=pView->Pages;

  int N=rTVB.NVariables();
  iNameWidth=8;
  for (int i=0; i<N; i++)
    iNameWidth=Max(iNameWidth, (int)(strlen(GetDisplayTag(TV[i].Tag, TV[i].Sym))+1));

  flag TearAdvRqd = rTVB.bUsedByBlk & TVU_AdvanceVars; // Page 0
  flag TearTstRqd = rTVB.bUsedByBlk & TVU_TestVars; // Page 0

  Strng Tag;

  if (TearAdvRqd) // Page 0
    {
    StartPage("Damping");//rTVB.Tag());//DD().Name());//(strlen(p)>0) ? p : "Fn");
    StartBlk(N>1 ? 4 : 1, 0, NULL);
    int L=0;
    SetSpace(L, 1);
    L++;
    if (N>1)
      {
      SetDParm(L, "Tear Method",    15, "", Id_TearMeth,     10, 2, " ");
      for (i=0; TearMethodStrings[i]!=NULL; i++)
        FldHasFixedStrValue(i, TearMethodStrings[i]);
      Tag.Set("%s.Method", rTVB.FullObjTag());
      SetTag(Tag());//, DampCnv.Text());
      //SetDesc(L, DampCnv.Text(),  -1, 8, 0, "");
      L++;
      SetDParm(L, "Damp As Group", 15, "", Id_DampAsGroup, 10, 2, "");
      FldHasFixedStrValue(0, "No");
      FldHasFixedStrValue(1, "Yes");
      L++;
      SetDParm(L, "Block Damp",    15, "", Id_BlkDamp,     10, 2, " ");
      Tag.Set("%s.DampingRqd", rTVB.FullObjTag());
      SetTag(Tag(), DampCnv.Text());

      SetDesc(L, DampCnv.Text(),  -1, 8, 0, "");
      L++;
      }

    StartBlk(N, 2, NULL);
    L=0;
    SetDesc(L, "Variable",  -1, iNameWidth, 0, "");
    SetDesc(L, "Global",    -1,    8, 2, "");
    SetDesc(L, "Local",     -1,    8, 2, "");
    SetDesc(L, "Current",   -1,    8, 2, " ");
    SetDesc(L, "Value",     -1,   10, 2, "");
    L++;

    SetDesc(L, "",          -1, iNameWidth, 2, "");
    SetDesc(L, DampCnv.Text(),  -1, 8,  2, "");
    SetDesc(L, DampCnv.Text(),  -1, 8,  2, "");
    SetDesc(L, DampCnv.Text(),  -1, 8,  2, " ");
    L++;

    for (i=0; i<N; i++)
      {
      SetDesc(L, GetDisplayTag(TV[i].Tag, TV[i].Sym),    -1, iNameWidth, 0, "");
      SetParm(L, "", Id_GlblDamp,  8, 2, "");
      if (pGroupInfo)
        {
        Tag.Set("%s.V.[%s].%s", pGroupInfo->FullObjTag(), pGroupInfo->SymOrTag(i)(), "DampingRqd");
        SetTag(Tag());
        }

      SetParm(L, "", Id_RqdDamp,   8, 2, "");
      Tag.Set("%s.V.[%s].%s", rTVB.FullObjTag(), rTVB.SymOrTag(i)(), "DampingRqd");
      SetTag(Tag(), DampCnv.Text());

      SetParm(L, "", Id_CurDamp,   8, 2, " ");
      Tag.Set("%s.V.[%s].%s", rTVB.FullObjTag(), rTVB.SymOrTag(i)(), "DampFactor");
      SetTag(Tag(), DampCnv.Text());

      SetParm(L, "", Id_Inputs ,   10, 2, " ");
      Tag.Set("%s.V.[%s].%s", rTVB.FullObjTag(), rTVB.SymOrTag(i)(), "Y0");
      SetTag(Tag());

      SetDesc(L, TV[i].AdvCnv->Text(),  -1, 8,  0, " ");
      L++;
      }
    }

  if (TearTstRqd) // Page 1
    {
    StartPage("Tolerance");
    int DoBlk=True;
    StartBlk(DoBlk ? 5 : 1, 0, NULL);
    int L=0;
    SetSpace(L, 1);
    L++;
    if (DoBlk)
      {
      SetDParm(L, "Block Strategy      ",  22, "", Id_BlkEPSStrat, 10, 0, " ");
      for (i=0; EPSStrategyStrings[i]!=NULL; i++)
        FldHasFixedStrValue(i, EPSStrategyStrings[i]);
      Tag.Set("%s.EPS_Strategy", rTVB.FullObjTag());
      SetTag(Tag());
//      SetDesc(L, EPSCnv.Text(),  -1, 10, 0, "");
      L++;

      SetDParm(L, "Block Tolerance (Rel)",  22, "", Id_BlkEPSRel,   10, 2, " ");
      Tag.Set("%s.EPS_Rel", rTVB.FullObjTag());
      SetTag(Tag());
      SetDesc(L, EPSCnv.Text(),  -1, 10, 0, "");
      L++;

      SetDParm(L, "Block Tolerance (Abs)",  22, "", Id_BlkEPSAbs,   10, 2, " ");
      Tag.Set("%s.EPS_Abs", rTVB.FullObjTag());
      SetTag(Tag());
      SetDesc(L, EPSCnv.Text(),  -1, 10, 0, "");
      L++;
      }

    StartBlk(N, 2, NULL);
    L=0;
    SetDesc(L, "Variable",  -1, iNameWidth, 0, "");
    SetDesc(L, "Global",    -1,     8, 2, "");
    SetDesc(L, "Local",     -1,     8, 2, "");
    SetDesc(L, "Current",   -1,     8, 2, " ");
    SetDesc(L, "Error",     -1,    10, 2, " ");
    SetDesc(L, "Cnt",       -1,     3, 0, " ");
    SetDesc(L, "Value",     -1,    10, 2, "");
    L++;

    SetDesc(L, "",          -1, iNameWidth, 2, "");
    SetDesc(L, EPSCnv.Text(),  -1,  8,  2, "");
    SetDesc(L, EPSCnv.Text(),  -1,  8,  2, "");
    SetDesc(L, EPSCnv.Text(),  -1,  8,  2, " ");
    SetDesc(L, DiffCnv.Text(), -1, 10,  2, " ");
    L++;

    for (i=0; i<N; i++)
      {
      SetDesc(L, GetDisplayTag(TV[i].Tag, TV[i].Sym),    -1, iNameWidth, 0, "");
      SetParm(L, "", Id_GlblEPSRel,  8, 2, "");
      if (pGroupInfo)
        {
        Tag.Set("%s.V.[%s].%s", pGroupInfo->FullObjTag(), pGroupInfo->SymOrTag(i)(), "EPS_Rel");
        SetTag(Tag());
        }

      SetParm(L, "", Id_EPSRel,   8, 2, "");
      Tag.Set("%s.V.[%s].%s", rTVB.FullObjTag(), rTVB.SymOrTag(i)(), "EPS_Rel");
      SetTag(Tag());

      SetParm(L, "", Id_CurTol,   8, 2, " ");
      Tag.Set("%s.V.[%s].%s", rTVB.FullObjTag(), rTVB.SymOrTag(i)(), "CurTol");
      SetTag(Tag());

      SetParm(L, "", Id_Diff,    10, 2, " ");
      Tag.Set("%s.V.[%s].%s", rTVB.FullObjTag(), rTVB.SymOrTag(i)(), "Error");
      SetTag(Tag());

      SetParm(L, "", Id_ConvergedCnt,    3, 0, " ");
      Tag.Set("%s.V.[%s].%s", rTVB.FullObjTag(), rTVB.SymOrTag(i)(), "ConvergedCnt");
      SetTag(Tag());

      SetParm(L, "", Id_Meas,    10, 2, " ");
      Tag.Set("%s.V.[%s].%s", rTVB.FullObjTag(), rTVB.SymOrTag(i)(), "Meas");
      SetTag(Tag());

      SetDesc(L, TV[i].MeasCnv->Text(),  -1, 8,  0, "");
      L++;
      }
    }
  }

//---------------------------------------------------------------------------

void TearVarBlkEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  Strng GrpTag;
  rTVB.GetGroupGlblTag(GrpTag);

  TearVarBlk * pGroupInfo=TearVarBlk::Find(GrpTag());
  Str="??";
  if (CurrentBlk(EI))
    {//header
    const flag IsAdaptSubs = (rTVB.m_iTearMethod==TCM_AdaptSubs || (rTVB.m_iTearMethod==TCM_NULL/* && PBEqnCB().Cfg.iConvergeMeth==TCM_AdaptSubs*/)); //todo: need access to global default iConvergeMeth !!
    switch (EI.FieldId)
      {
      case Id_DampAsGroup:
        Str=DampAsGroupStrings[rTVB.m_iDampAsGroup];
        EI.Fld->fEditable=(IsAdaptSubs);
        break;
      case Id_BlkDamp:
        DampFmt.FormatFloat(DampCnv.Human(rTVB.m_dDampingRqd), Str);
        EI.Fld->fEditable=(IsAdaptSubs);
        break;
      case Id_BlkEPSStrat:
        Str=EPSStrategyStrings[rTVB.m_iEPSStrategy];
        break;
      case Id_BlkEPSRel:
        EPSFmt.FormatFloat(EPSCnv.Human(rTVB.dEPS_Rel), Str);
        break;
      case Id_BlkEPSAbs:
        EPSFmt.FormatFloat(EPSCnv.Human(rTVB.dEPS_Abs), Str);
        break;
      case Id_TearMeth:
        Str=TearMethodStrings[rTVB.m_iTearMethod];
        break;
      }
    }

  if (CurrentBlk(EI))
    {//data
    long p=EI.PageNo;
    long i=(EI.BlkRowNo-EI.Index);
    TearVar &TVi = rTVB.TV[i];
    switch (EI.FieldId)
      {
      case Id_CurDamp:
        DampFmt.FormatFloat(DampCnv.Human(TVi.m_DampFactor), Str);
        EI.Fld->fEditable=False;
        break;
      case Id_RqdDamp:
        DampFmt.FormatFloat(DampCnv.Human(TVi.m_DampingRqd), Str);
        //EI.Fld->fEditable=(rTVB.m_iTearMethod==TCM_AdaptSubs || (rTVB.m_iTearMethod==TCM_NULL && PBEqnCB().Cfg.iConvergeMeth==TCM_AdaptSubs));  //todo: need access to global default iConvergeMeth
        EI.Fld->fEditable=(rTVB.m_iTearMethod==TCM_AdaptSubs || (rTVB.m_iTearMethod==TCM_NULL));// && PBEqnCB().Cfg.iConvergeMeth==TCM_AdaptSubs));
        break;
      case Id_EPSRel:
        EPSFmt.FormatFloat(EPSCnv.Human(TVi.EPS_Rel), Str);
        break;
      case Id_EPSAbs:
        EPSFmt.FormatFloat(EPSCnv.Human(TVi.EPS_Abs), Str);
        break;

      case Id_GlblDamp:
        if (pGroupInfo)
          DampFmt.FormatFloat(DampCnv.Human(pGroupInfo->TV[i].m_DampingRqd), Str);
        else
          Str="?";
        //EI.Fld->fEditable=(rTVB.m_iTearMethod==TCM_AdaptSubs || PBEqnCB().Cfg.iConvergeMeth==TCM_AdaptSubs);  //todo: need access to global default iConvergeMeth
        EI.Fld->fEditable=(rTVB.m_iTearMethod==TCM_AdaptSubs || rTVB.m_iTearMethod==TCM_NULL);
        break;
      case Id_GlblEPSRel:
        if (pGroupInfo)
          DampFmt.FormatFloat(DampCnv.Human(pGroupInfo->TV[i].EPS_Rel), Str);
        else
          Str="?";
        break;
      case Id_GlblEPSAbs:
        if (pGroupInfo)
          DampFmt.FormatFloat(DampCnv.Human(pGroupInfo->TV[i].EPS_Abs), Str);
        else
          Str="?";
        break;

      case Id_Inputs:
        TVi.AdvFmt->FormatFloat(TVi.AdvCnv->Human(TVi.Y[0]), Str);
        EI.Fld->fEditable=False;
        break;
      case Id_Outputs:
        TVi.AdvFmt->FormatFloat(TVi.AdvCnv->Human(TVi.X[0]), Str);
        EI.Fld->fEditable=False;
        break;
      case Id_CurTol:
        EPSFmt.FormatFloat(EPSCnv.Human(TVi.CurTol), Str);
        EI.Fld->fEditable=False;
        break;
      case Id_Diff:
        DiffFmt.FormatFloat(DiffCnv.Human(TVi.m_Error[0]), Str);
        EI.Fld->fEditable=False;
        break;
      case Id_ConvergedCnt:
        Str.Set("%3d", TVi.m_iConvergedCnt);
        EI.Fld->fEditable=False;
        break;
      case Id_Meas:
        TVi.MeasFmt->FormatFloat(TVi.MeasCnv->Human(TVi.m_Meas[0]), Str);
        EI.Fld->fEditable=False;
        break;
      }
    }
  }

//---------------------------------------------------------------------------

long TearVarBlkEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  Strng GrpTag;
  rTVB.GetGroupGlblTag(GrpTag);
  TearVarBlk * pGroupInfo=TearVarBlk::Find(GrpTag());

  long Fix=0;
  if (CurrentBlk(EI))
    {//header
    switch (EI.FieldId)
      {
      case Id_DampAsGroup:
        {
        for (int i=0; DampAsGroupStrings[i]; i++)
          if (Str.XStrICmp(DampAsGroupStrings[i])==0)
            break;
        rTVB.m_iDampAsGroup=(DampAsGroupStrings[i] ? i : DAG_Off);
        //rTVB.m_fDampAsGroup = (strpbrk(Str(), "Yy")!=0);
        break;
        }
      case Id_BlkDamp:
        rTVB.m_dDampingRqd = DampCnv.Normal(SafeAtoF(Str));
        break;
      case Id_BlkEPSStrat:
        {
        for (int i=0; EPSStrategyStrings[i]; i++)
          if (Str.XStrICmp(EPSStrategyStrings[i])==0)
            break;
        rTVB.m_iEPSStrategy=(EPSStrategyStrings[i] ? i : TVBTS_Maximum);
        }
        break;
      case Id_BlkEPSRel:
        rTVB.dEPS_Rel = DampCnv.Normal(SafeAtoF(Str));
        break;
      case Id_BlkEPSAbs:
        rTVB.dEPS_Abs = DampCnv.Normal(SafeAtoF(Str));
        break;
      case Id_TearMeth:
        {
        for (int i=0; TearMethodStrings[i]; i++)
          if (Str.XStrICmp(TearMethodStrings[i])==0)
            break;
        rTVB.m_iTearMethod=(TearMethodStrings[i] ? i : TCM_NULL);
        }
        break;
      }
    }

  if (CurrentBlk(EI))
    {//data
    long  p=EI.PageNo;
    long  i=EI.BlkRowNo-EI.Index;
    TearVar &TVi = rTVB.TV[i];
    switch (EI.FieldId)
      {
      case Id_CurDamp:
        TVi.m_DampFactor=DampCnv.Normal(SafeAtoF(Str));
        break;
      case Id_RqdDamp:
        TVi.m_DampingRqd=DampCnv.Normal(SafeAtoF(Str));
        break;
      case Id_EPSRel:
        TVi.EPS_Rel=EPSCnv.Normal(SafeAtoF(Str));
        break;
      case Id_EPSAbs:
        TVi.EPS_Abs=EPSCnv.Normal(SafeAtoF(Str));
        break;

      case Id_GlblDamp:
        if (pGroupInfo)
          pGroupInfo->TV[i].m_DampingRqd=DampCnv.Normal(SafeAtoF(Str));
        break;
      case Id_GlblEPSRel:
        if (pGroupInfo)
          pGroupInfo->TV[i].EPS_Rel=EPSCnv.Normal(SafeAtoF(Str));
        break;
      case Id_GlblEPSAbs:
        if (pGroupInfo)
          pGroupInfo->TV[i].EPS_Abs=EPSCnv.Normal(SafeAtoF(Str));
        break;
      }
    }

  return Fix;
  }

//---------------------------------------------------------------------------

long TearVarBlkEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  if (CurrentBlk(EI))
    {//header
    long p=EI.PageNo;
    long i=EI.BlkRowNo-EI.Index;
    }

  if (CurrentBlk(EI))
    {//data
    long  p=EI.PageNo;
    long  i=(long )(EI.BlkRowNo-EI.Index);
    }
  return Fix;
  }

//---------------------------------------------------------------------------

flag TearVarBlkEdt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TearVarBlkEdt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TearVarBlkEdt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TearVarBlkEdt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TearVarBlkEdt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  FxdEdtView &Vw=View();
  flag ret=false;//FxdEdtView::DoRButtonUp(nFlags, point);
  if (Vw.CPgNo>=0)
    {
    FxdEdtInfo EI;
    if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
      {
      ret=true;
      //if (EI.FieldId<0 && EI.Fld)
      if (EI.Fld)
        {
        int IsDampValue=(
          EI.FieldId==Id_BlkDamp ||
          EI.FieldId==Id_CurDamp ||
          EI.FieldId==Id_RqdDamp ||
          EI.FieldId==Id_GlblDamp );
        int IsEPSValue= (
          EI.FieldId==Id_CurTol ||
          EI.FieldId==Id_BlkEPSRel ||
          EI.FieldId==Id_BlkEPSAbs ||
          EI.FieldId==Id_EPSRel  ||
          EI.FieldId==Id_EPSAbs  ||
          EI.FieldId==Id_GlblEPSRel ||
          EI.FieldId==Id_GlblEPSAbs );
        int IsValValue= (
          EI.FieldId==Id_Inputs||
          EI.FieldId==Id_Outputs);
        int IsMeasValue= (
          EI.FieldId==Id_Meas);
        int IsDiffValue= (
          EI.FieldId==Id_Diff);
        int IsOtherValue= (
          EI.FieldId==Id_BlkEPSStrat ||
          EI.FieldId==Id_TearMeth ||
          EI.FieldId==Id_ConvergedCnt);
        if (IsDampValue || IsEPSValue || IsValValue || IsMeasValue || IsDiffValue || IsOtherValue)
          {
          CRect WRect;
          Vw.GetWindowRect(&WRect);
          CPoint  RBPoint;
          RBPoint.x = WRect.left+point.x;
          RBPoint.y = WRect.top+point.y;
          if (IsDampValue)
            {
            WrkIB.Set(EI.Fld->Tag, &DampCnv, &DampFmt);
            }
          else if (IsEPSValue)
            {
            WrkIB.Set(EI.Fld->Tag, &EPSCnv, &EPSFmt);
            }
          else if (IsValValue)
            {
            int i=EI.BlkRowNo-EI.Index;
            TearVar &TVi = rTVB.TV[i];
            WrkIB.Set(EI.Fld->Tag, TVi.AdvCnv, TVi.AdvFmt);
            }
          else if (IsMeasValue)
            {
            int i=EI.BlkRowNo-EI.Index;
            TearVar &TVi = rTVB.TV[i];
            WrkIB.Set(EI.Fld->Tag, TVi.MeasCnv, TVi.MeasFmt);
            }
          else if (IsDiffValue)
            {
            WrkIB.Set(EI.Fld->Tag, &DiffCnv, &DiffFmt);
            }
          else if (IsOtherValue)
            {
            WrkIB.Set(EI.Fld->Tag);
            }
          else
            {
            ASSERT(0);
            WrkIB.Clear();
            }

          CMenu Menu;
          Menu.CreatePopupMenu();
          CMenu CnvMenu;
          CnvMenu.CreatePopupMenu();
          Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
          CMenu FormatMenu;
          FormatMenu.CreatePopupMenu();
          Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu.m_hMenu, "&Format");
          Menu.AppendMenu(MF_SEPARATOR);

          if (WrkIB.FmtOK())
            {
            WrkIB.Cnv().AddToMenu(CnvMenu);
            if ((WrkIB.Cnv().Index())<=0)// || !IsFloatData(d.iType))
              Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);
            WrkIB.Fmt().AddToMenu(FormatMenu);
            }
          else
            {
            Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);
            Menu.EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED);
            }

          Menu.AppendMenu(MF_STRING, pView->iCmdCopyTag, "Copy Full &Tag");
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyRHSTag, "Copy Tag");
          if (EI.Fld->Tag==NULL)
            {
            Menu.EnableMenuItem(pView->iCmdCopyTag, MF_BYCOMMAND|MF_GRAYED);
            Menu.EnableMenuItem(pView->iCmdCopyRHSTag, MF_BYCOMMAND|MF_GRAYED);
            }
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyVal, "Copy &Value");

          Menu.AppendMenu(MF_STRING, ID_EDIT_COPY, "Copy");
          Menu.AppendMenu(MF_STRING, ID_EDIT_PASTE, "Paste");

          Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, /*(CWnd*)*/&View());
          Menu.DestroyMenu();
          }
        }
      }
    }
  return ret;
  }

//---------------------------------------------------------------------------

flag TearVarBlkEdt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TearVarBlkEdt::DoMouseMove(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TearVarBlkEdt::DoAccCnv(UINT Id)
  {
  if (WrkIB.CnvOK())
    {
    pCDataCnv pC=Cnvs[(WrkIB.Cnv().Index())];
    for (int i=Id; i>0; i--)
      pC=pC->Next();

    if (pC)
      WrkIB.Cnv().SetText(pC->Txt());
    View().DoRebuild();
    WrkIB.Set((CnvAttribute*)NULL);
    }
  return True;
  };

//---------------------------------------------------------------------------

flag TearVarBlkEdt::DoAccFmt(UINT Id)
  {
  if (WrkIB.FmtOK())
    {
    for (UINT i=0; i<(UINT)DefinedFmts.GetSize(); i++)
      if (i==Id)
        break;

    if (i<(UINT)DefinedFmts.GetSize())
      {
      WrkIB.Fmt()=DefinedFmts[i];
      View().DoRebuild();
      }
    WrkIB.Set((FmtAttribute*)NULL);
    }
  return True;
  };

//---------------------------------------------------------------------------

flag TearVarBlkEdt::DoAccRptTagLists()
  {
  return FxdEdtBookRef::DoAccRptTagLists();
  };
#endif

//===========================================================================
//
//
//
//===========================================================================

void EqnSlvCfgBlk::InitProBal()
  {
  iConvergeMeth = TCM_AdaptSubs;//TCM_Wegstein;
  fMassSmooth=0;
  fForceConverged=0;
  fIdleWhenDone=0;
  dEps_Rel=1.0e-5;
  dEps_Abs=1.0e-6;
  m_dDamping=0.0;
  iMaxIters=1000;
  iRqdCnvrgdIters=3;
  dWA_Bound=-3.0;
  iWA_Delay=5;
  dWA_Clamping=0.5;
  m_iDampAsGroup=DAG_Off;

  dDampFctGrowth=0.25;
  dDampFctDecay=0.05;
  }

//--------------------------------------------------------------------------

void EqnSlvCfgBlk::InitDynamic()
  {
  iConvergeMeth = TCM_DirectSubs;
  fMassSmooth=0;
  fForceConverged=1;
  fIdleWhenDone=0;
  dEps_Rel=1.0e-3;
  dEps_Abs=1.0e-4;
  m_dDamping=0.0;
  iMaxIters=25;
  iRqdCnvrgdIters=3;
  dWA_Bound=-5.0;
  iWA_Delay=2;
  dWA_Clamping=0.5;
  m_iDampAsGroup=DAG_Off;

  dDampFctGrowth=0.25;
  dDampFctDecay=0.05;
  }

//==========================================================================
//
//
//
//==========================================================================

EqnSlvCtrlBlk::EqnSlvCtrlBlk()
  {
  nIters=nConvergedIters=nBadError=nBadLimit=nUnknowns=0;
  fConfigOK=fConverged=False;
  bHoldTearUpdate=0;
  iReport=ESR_Errors;
  dReportTol=1.0e-6;
  dWorstErr=0.0;
  dWorstOtherErr=0.0;
  dTheWorstErr=0.0;
  iCollectWorst=0;
  };

//--------------------------------------------------------------------------

void EqnSlvCtrlBlk::Init()
  {
  nIters=0;
  nConvergedIters=0;
  BeginStep();
  }

//--------------------------------------------------------------------------

void EqnSlvCtrlBlk::BeginStep()
  {
  nBadError=0;
  nBadLimit=0;
  nUnknowns=0;
  fConfigOK=True;
  fConverged=True;
  for (int i=0; i<MAX_EQNSLV_WORST; i++)
    {
    Worst[i].dErr=0.0;
    Worst[i].cStr[0]=0;
    }
  dWorstErr=0.0;
  WorstOther.dErr=0.0;
  WorstOther.cStr[0]=0;
  dWorstOtherErr=0.0;
  }

//--------------------------------------------------------------------------

void EqnSlvCtrlBlk::EndStep()
  {
  dTheWorstErr=Max(dWorstErr, dWorstOtherErr);
  }

//--------------------------------------------------------------------------

const double BigError=1.0e30;

flag EqnSlvCtrlBlk::NoteBadEqnError(double Err, double RelTol)
  {
  BumpNBadError();
  const double RErr=Err*RelTol;
  if (RErr>dWorstErr)
    dWorstErr=Min(RErr, BigError);
  return (iCollectWorst && RErr>Worst[MAX_EQNSLV_WORST-1].dErr);
  }

//--------------------------------------------------------------------------

void EqnSlvCtrlBlk::CollectBadEqnError(double Err, double Val, double AbsTol, double RelTol, double Damping, char * Tag, byte TearTagTyp)
  {
  const double RErr=Err*RelTol;
  ASSERT(iCollectWorst && RErr>Worst[MAX_EQNSLV_WORST-1].dErr);

  for (int i=0; i<MAX_EQNSLV_WORST; i++)
    if (RErr>Worst[i].dErr)
      break;
  ASSERT(i<MAX_EQNSLV_WORST);
  for (int j=MAX_EQNSLV_WORST-1; j>i; j--)
    Worst[j]=Worst[j-1];
  Worst[i].dErr=Min(RErr, BigError);
  Worst[i].iTearTagTyp=TearTagTyp;

  char TolStr[32], ValStr[32], DmpStr[32];
  //sprintf(TolStr, "%10.6f", Err);
  sprintf(TolStr, "%10.1f", Err);
  sprintf(ValStr, "%14.6g", Val);
  sprintf(DmpStr, Damping>0.0 ? "%6.2f%%" : "", Damping*100.0);
  XStrTrim(TolStr);
  XStrTrim(ValStr);
  XStrTrim(DmpStr);
  //sprintf(Worst[i].cStr, "%s\t%s\t%s\t%s", TolStr, ValStr, DmpStr, Tag);
  //ASSERT(strlen(Worst[i].cStr)<MAX_EQNSLV_MSGLEN);
  const int len = sprintf(Worst[i].cStr, "%s\t%s\t%s\t", TolStr, ValStr, DmpStr);
  strncpy(Worst[i].cStr + len, Tag, MAX_EQNSLV_MSGLEN - len);
  Worst[i].cStr[MAX_EQNSLV_MSGLEN-1]=0;
  }

//--------------------------------------------------------------------------

flag EqnSlvCtrlBlk::NoteBadEqnLimit()
  {
  BumpNBadLimit();
  const double Err=BigError*10.0;
  if (Err>dWorstErr)
    dWorstErr=Err;
  return (iCollectWorst && Err>Worst[MAX_EQNSLV_WORST-1].dErr);
  }

//--------------------------------------------------------------------------

void EqnSlvCtrlBlk::CollectBadEqnLimit(double Val, char * Tag, byte TearTagTyp)
  {
  const double Err=BigError*10.0;
  ASSERT(iCollectWorst && Err>Worst[MAX_EQNSLV_WORST-1].dErr);

  for (int i=0; i<MAX_EQNSLV_WORST; i++)
    if (Err>Worst[i].dErr)
      break;
  ASSERT(i<MAX_EQNSLV_WORST);
  for (int j=MAX_EQNSLV_WORST-1; j>i; j--)
    Worst[j]=Worst[j-1];
  Worst[i].dErr=Err;
  Worst[i].iTearTagTyp=TearTagTyp;

  char ValStr[32];
  sprintf(ValStr, "%14.6g", Val);
  XStrTrim(ValStr);
  const int len = sprintf(Worst[i].cStr, "Limit\t%s\t\t", ValStr);
  strncpy(Worst[i].cStr + len, Tag, MAX_EQNSLV_MSGLEN - len);
  Worst[i].cStr[MAX_EQNSLV_MSGLEN-1]=0;
  }

//--------------------------------------------------------------------------

flag EqnSlvCtrlBlk::NoteWorstError(double Err, double RelTol)
  {
  const double RErr=Err*RelTol;
  if (RErr>dWorstOtherErr)
    dWorstOtherErr=Min(RErr, BigError);
  return (iCollectWorst && RErr>WorstOther.dErr);
  }

//--------------------------------------------------------------------------

void EqnSlvCtrlBlk::CollectWorstError(double Err, double Val, double AbsTol, double RelTol, double Damping, char * Tag, byte TearTagTyp)
  {
  const double RErr=Err*RelTol;
  ASSERT(iCollectWorst && RErr>WorstOther.dErr);

  WorstOther.dErr=dWorstOtherErr;
  WorstOther.iTearTagTyp=TearTagTyp;

  char TolStr[32], ValStr[32], DmpStr[32];
  sprintf(TolStr, "%10.3f", Err);
  sprintf(ValStr, "%14.6g", Val);
  sprintf(DmpStr, Damping>0.0 ? "%6.2f%%" : "", Damping*100.0);
  XStrTrim(TolStr);
  XStrTrim(ValStr);
  XStrTrim(DmpStr);
  const int len = sprintf(WorstOther.cStr, "%s\t%s\t%s\t", TolStr, ValStr, DmpStr);
  strncpy(WorstOther.cStr + len, Tag, MAX_EQNSLV_MSGLEN - len);
  WorstOther.cStr[MAX_EQNSLV_MSGLEN-1]=0;
  }

//--------------------------------------------------------------------------

void EqnSlvCtrlBlk::SetCollectWorst(byte C)
  {
  /*if (iCollectWorst==0 && C)
    {
    Worst[0].dErr=dWorstErr;
    strcpy(Worst[0].cStr, "...");
    for (int i=1; i<MAX_EQNSLV_WORST; i++)
      {
      Worst[i].dErr=0.0;
      Worst[i].cStr[0]=0;
      }
    WorstOther.dErr=dWorstOtherErr;
    strcpy(WorstOther.cStr, "...");
    }*/
  iCollectWorst=C;
  }

//==========================================================================
//
//
//
//==========================================================================

EqnSolverBlk::EqnSolverBlk()
  {
  };

//--------------------------------------------------------------------------

EqnSolverBlk::~EqnSolverBlk()
  {
  };

//--------------------------------------------------------------------------

void EqnSolverBlk::BeginStep(EqnSlvCtrlBlk & EqnCB)
  {
  EqnCB.BeginStep();
  TearVarBlkIter TCBs(TearVarBlk::List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    if (pT->Active())
      pT->BeginStep(EqnCB);
  };

//--------------------------------------------------------------------------

void EqnSolverBlk::EndStep(EqnSlvCtrlBlk & EqnCB)
  {
  EqnCB.EndStep();
  TearVarBlkIter TCBs(TearVarBlk::List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    if (pT->Active())
      pT->EndStep(EqnCB);
  };

// ------------------------------------------------------------------------

void EqnSolverBlk::SetTearsInUse(int InUse)
  {
  TearVarBlkIter TCBs(TearVarBlk::List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    pT->SetInUse(InUse);
  }

// ------------------------------------------------------------------------

void EqnSolverBlk::FindAllTearsInUse()
  {
  SetTearsInUse(False);
  TearPosBlkIter TPBs(TearPosBlk::List);
  for (TearPosBlk* pP=TPBs.First(); pP; pP=TPBs.Next())
    {
    Strng Tag;
    TearVarBlk* pVarBlk=TearVarBlk::Find(pP->TearGetTag(Tag));
    if (pVarBlk)
      pVarBlk->SetInUse(True);
    }
  };

// ------------------------------------------------------------------------

void EqnSolverBlk::RemoveUnusedTears()
  {
  FindAllTearsInUse();
  TearVarBlkIter TCBs(TearVarBlk::List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    if (!pT->InUse() && !pT->fGroupBlk)
      TearVarBlk::Delete(pT);
  };

// ------------------------------------------------------------------------

void EqnSolverBlk::RemoveInActiveTears()
  {
  FindAllTearsInUse();
  TearVarBlkIter TCBs(TearVarBlk::List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    //if (!pT->InUse() && !pT->fGroupBlk)
    if (!pT->Active() && !pT->fGroupBlk)
      TearVarBlk::Delete(pT);
  };

// ------------------------------------------------------------------------

void EqnSolverBlk::RemoveAllTears()
  {
  TearVarBlkIter TCBs(TearVarBlk::List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    TearVarBlk::Delete(pT);
  };

//--------------------------------------------------------------------------

void EqnSolverBlk::ActivateAllReqdTears()
  {
  TearPosBlkIter TPBs(TearPosBlk::List);
  for (TearPosBlk* pT=TPBs.First(); pT; pT=TPBs.Next())
    if (pT->TearVarType() != TVT_None)
      pT->ActivateTear();
  }

//--------------------------------------------------------------------------

void EqnSolverBlk::DeActivateAllTears()
  {
  TearPosBlkIter TPBs(TearPosBlk::List);
  for (TearPosBlk* pT=TPBs.First(); pT; pT=TPBs.Next())
    pT->DeActivateTear();
  }

//--------------------------------------------------------------------------

void EqnSolverBlk::ConnectAllTears(flag CreateIfReqd)
  {
  TearPosBlkIter TPBs(TearPosBlk::List);
  for (TearPosBlk* pT=TPBs.First(); pT; pT=TPBs.Next())
    pT->ConnectTear(CreateIfReqd);
  }

//--------------------------------------------------------------------------

void EqnSolverBlk::DisConnectAllTears()
  {
  TearPosBlkIter TPBs(TearPosBlk::List);
  for (TearPosBlk* pT=TPBs.First(); pT; pT=TPBs.Next())
    pT->DisConnectTear();
  }

//--------------------------------------------------------------------------

void EqnSolverBlk::InitialiseActiveTears(EqnSlvCtrlBlk & EqnCB)
  {
  EqnCB.SetNUnknowns(0);
  EqnCB.Cfg.iRqdCnvrgdIters=Range((short)1, EqnCB.Cfg.iRqdCnvrgdIters, (short)9999);

  TearVarBlkIter TCBs(TearVarBlk::List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    if (pT->Active())
      {
      pT->m_bHoldDamping=false;
      pT->SetTearInitCounters(true);
      if (pT->TearAdvanceReqd())
        {
        if (pT->TearType()>=TT_ManualTear && pT->TearInitActive())
          {
          switch (pT->TearInitHow())
            {
            case TIH_ZeroNIters:
              pT->TearInitialiseOutputs(-1);
              break;
            case TIH_HoldNIters:
            case TIH_RampNIters:
              pT->TearInitialiseOutputs(pT->EstimatePortion());
              break;
            default:
              break;
            }
          }
        //pT->BumpTearInitCounters();
        pT->TearGetOutputs();
        }
      }
  }

//--------------------------------------------------------------------------

void EqnSolverBlk::RestartAllActiveTears(EqnSlvCtrlBlk & EqnCB)
  {
  //DoBreak();
  TearVarBlkIter TCBs(TearVarBlk::List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    if (pT->Active())
      {
      if (pT->m_lStageCnt>0)
        pT->m_lStageCnt=0;
      }
  }

//--------------------------------------------------------------------------

void EqnSolverBlk::TransferAllTears(EqnSlvCtrlBlk & EqnCB)
  {
  TearVarBlkIter TCBs(TearVarBlk::List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    if (pT->Active() && pT->TearAdvanceReqd())
      pT->TearInputs2Outputs();
  }

//--------------------------------------------------------------------------

flag EqnSolverBlk::TestTearConvergence(long ConvergeLoopCnt, EqnSlvCtrlBlk & EqnCB, Strng_List &BadTearTags, Strng_List & BadTearInfo)
  {
  TearVarBlkIter TCBs(TearVarBlk::List);
  TearVarBlk *pT=NULL;

  BadTearTags.Clear();
  BadTearInfo.Clear();
  double MaxError=0;
  Strng MaxTag;
  TearVarBlk *pTMax=NULL;

  #if dbgEqnSolve
  //if (dbgTearCnvgLoop())
  //  dbgpln(" TestTearConvergence=");
  if (dbgHoldTearAdvance())
    goto DoTest;
  #endif

  for (pT=TCBs.First(); pT; pT=TCBs.Next())
    {
    if (pT->Active())
      {
      pT->m_lConvergeLoopCnt=ConvergeLoopCnt;
      if (pT->TearAdvanceReqd())
        {
        pT->RotateInputs();
        pT->TearGetInputs();
        }
      if (pT->TearTestReqd())
        {
        pT->RotateErrors();
        pT->TearGetErrors();
        double Err;
        Strng ErrTag;
        if (!pT->TestConverged(EqnCB, Err, ErrTag))
          {
          if (Err>MaxError)
            {
            MaxError=Err;
            pTMax=pT;
            MaxTag=ErrTag;
            }
          }
        }
      }
    else
      pT->m_lConvergeLoopCnt=-1;
    }

#if dbgEqnSolve
DoTest:
#endif

  int OK=EqnCB.Converged();
  if (!OK && pTMax)
    {
    Strng S, Tg;
    S.Set("%s(%s) %.2f", pTMax->Tag(), MaxTag(), MaxError);
    pTMax->LHSTag(Tg);
    BadTearTags.Append(Tg);
    BadTearInfo.Append(S);
    }

#if dbgEqnSolve
  if (dbgHoldTearAdvance())
    goto SetIt;
#endif

  //if (!OK)
    {
    TearVarBlkIter TCBs(TearVarBlk::List);
    for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
      {
      if (pT->Active() && pT->TearAdvanceReqd())
        {
        pT->RotateOutputs();
        pT->Advance(EqnCB);
        if (!EqnCB.HoldTearUpdate())
          {
          pT->TearSetOutputs();
          pT->BumpTearInitCounters();
          if (pT->TearType()>=TT_ManualTear && pT->TearInitActive())
            {
            switch (pT->TearInitHow())
              {
              case TIH_ZeroNIters:
                pT->TearInitialiseOutputs(-1);
                break;
              case TIH_HoldNIters:
              case TIH_RampNIters:
                pT->TearInitialiseOutputs(pT->EstimatePortion());
                break;
              default:
                break;
              }
            }
          }
        }
      }
    }

#if dbgEqnSolve
SetIt:
#endif

  EqnCB.SetConverged(OK);
  return OK;
  }


//===========================================================================
//
//
//
//===========================================================================
