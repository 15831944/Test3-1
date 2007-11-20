//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __EQNSOLVE_CPP
#include "eqnsolve.h"
#include "executiv.h"

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

//const long  SQN_Start           = 0;
//const long  SQN_SettleAndTweak  = 1;
//const long  SQN_SettleAndCalc   = 2;
//const long  SQN_Control         = 5;

// ========================================================================

TearVar::TearVar()
  {
  m_DampingRqd=dNAN;
  m_DampFactor=0.0;
  m_EstDampFactor=0.0;
  m_YRat=0.0;

  m_iTearMethod=TCM_Default;
  m_iHoldOutput=THO_Default;

  m_EPS_Abs=dNAN;
  m_EPS_Rel=dNAN;
  m_CurTol=0.0;

  m_bTestIsValid=true;
  m_iConvergedCnt=0;

  m_Initial=0.0;

  m_pDiffFmt=NULL;
  m_pDiffCnv=NULL;
  m_pMeasFmt=NULL;
  m_pMeasCnv=NULL;
  }

// ------------------------------------------------------------------------
// ========================================================================

TearPosBlk::TearPosBlk(byte DefaultTearType)
  {
  m_ListPos=sm_List.AddHead(this);
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
  sm_List.RemoveAt(m_ListPos);
  DeActivateTear();
  };

//--------------------------------------------------------------------------

void TearPosBlk::ConnectTear(flag CreateIfReqd)
  {
  Strng Tag=TearGetTag();
  m_pVarBlk=TearVarBlk::Find(Tag());
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
      dbgpln("DisConnectTear() %s", TearGetTag());
      }
#endif
    }
  m_pVarBlk=NULL;
  };

//--------------------------------------------------------------------------

void TearPosBlk::ActivateTear()
  {
  Strng Tag=TearGetTag();
#if dbgEqnSolve
  if (dbgTearSetupAll())
    {
    dbgpln("ActivateTear() %s", Tag());
    }
#endif
  m_pVarBlk=TearVarBlk::Find(Tag());
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
    dbgpln("DeActivateTear() %s", TearGetTag());
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
  TearVarBlkIter TCBs(TearVarBlk::sm_List);
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

  TearVarBlkIter TCBs(TearVarBlk::sm_List);
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
  if (sm_List.IsEmpty())
    p->m_ListPos=sm_List.AddHead(p);
  else
    {
    TearVarBlkIter TCBs(TearVarBlk::sm_List);
    for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
      if (_stricmp(pT->Tag(), p->Tag())>0)
        break;
    if (pT)
      p->m_ListPos=sm_List.InsertBefore(pT->m_ListPos, p);
    else
      p->m_ListPos=sm_List.AddTail(p);
    }
  return p;
  };

// ------------------------------------------------------------------------

TearVarBlk *TearVarBlk::Add(TearVarBlk *Blk)
  {
  TearVarBlk *p=static_cast<TearVarBlk *>(Blk);//new TearVarBlk(Tag, NULL);
  if (sm_List.IsEmpty())
    Blk->m_ListPos=sm_List.AddHead(Blk);
  else
    {
    TearVarBlkIter TCBs(TearVarBlk::sm_List);
    for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
      if (_stricmp(pT->Tag(), Blk->Tag())>0)
        break;
    if (pT)
      Blk->m_ListPos=sm_List.InsertBefore(pT->m_ListPos, Blk);
    else
      Blk->m_ListPos=sm_List.AddTail(Blk);
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

#if WithTearVarBlkEdit
IMPLEMENT_TAGOBJEDT(TearVarBlk, "TearBlk", "TbBase", "", "TBase", TOC_SYSTEM,
                    TearVarBlkEdt,
                    "TearBlock", "Tear Variable Calculation Block");
#else
IMPLEMENT_TAGOBJ(TearVarBlk, "TearBlk", "TbBase", "", "", "TBase", TOC_SYSTEM,
                 "TearBlock", "Tear Variable Calculation Block");
#endif

// ------------------------------------------------------------------------

TearVarBlk::TearVarBlk(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) : \
TaggedObject(pClass_, Tag_, pAttach, TOA_Free)
  {
  CommonConstruct();
  }

// ------------------------------------------------------------------------

TearVarBlk::TearVarBlk(pchar Tag_, pTaggedObject pAttach) : \
TaggedObject(&TearVarBlkClass, Tag_, pAttach, TOA_Free)
  {
  CommonConstruct();
  }

// ------------------------------------------------------------------------

void TearVarBlk::CommonConstruct()
  {
  m_ListPos=NULL;
  m_DampingRqd=dNAN;
  m_iDampAsGroup=DAG_Default;
  m_GrpDampFactor=0.0;
  m_bHoldDamping=false;
  fGroupBlk=False;
  pGroupInfo=NULL;
  bUsedByBlk=0;

  m_iTearMethod=TCM_Default;
  m_iHoldOutput=THO_Default;
  iTearTagTyp=TTT_Unknown;

  m_iEPSStrategy=TVBTS_Default;
  m_EPS_Rel=dNAN;
  m_EPS_Abs=dNAN;

  m_iTagWidth=-1;
  m_iSymWidth=-1;

  m_pPosBlk=NULL;
  m_nVariables=0;
  m_nHistory=4;
  SetNVariables(0, 0);
  m_bInUse=False;
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
  if (m_ListPos)
    sm_List.RemoveAt(m_ListPos);
#if dbgEqnSolve
  if (dbgTearSetupAll())
    dbgpln("Destroy %s", FullObjTag());
#endif
  };

//--------------------------------------------------------------------------

LPSTR TearMethodStrings[]={"<Default>", "Direct", "Adaptive", "Wegstein", /*"Kalman",*/ 0};
LPSTR TearMethodDefStrings[]={"??Default??", "<Direct>", "<Adaptive>", "<Wegstein>", /*"Kalman",*/ 0};

DDBValueLst DDBTearMethod[]=
  {
  //{TCM_Default        ,  TearMethodStrings[TCM_Default        ]},
    {TCM_DirectSubs,  TearMethodStrings[TCM_DirectSubs]},
    {TCM_AdaptSubs ,  TearMethodStrings[TCM_AdaptSubs ]},
    {TCM_Wegstein  ,  TearMethodStrings[TCM_Wegstein  ]},
    // {TCM_Kalman    ,  TearMethodStrings[TCM_Kalman    ]},
    {0}
  };
DDBValueLst DDBTearMethodDef[]=
  {
    {TCM_Default   ,  TearMethodStrings[TCM_Default]},
    {TCM_DirectSubs,  TearMethodStrings[TCM_DirectSubs]},
    {TCM_AdaptSubs ,  TearMethodStrings[TCM_AdaptSubs ]},
    {TCM_Wegstein  ,  TearMethodStrings[TCM_Wegstein  ]},
    // {TCM_Kalman    ,  TearMethodStrings[TCM_Kalman    ]},
    {0}
  };

//--------------------------------------------------------------------------

LPSTR DampAsGroupStrings[]={"<Def>", "no", "Yes", 0};
LPSTR DampAsGroupDefStrings[]={"<Def>", "<no>", "<Yes>", 0};

LPSTR DampAsGroupStringsShort[]={"<Def>", "no", "Yes", 0};
LPSTR DampAsGroupDefStringsShort[]={"<Def>", "<no>", "<Yes>", 0};

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

//--------------------------------------------------------------------------

LPSTR HoldOutputStrings[]={"<Def>", "no", "Yes", 0};
LPSTR HoldOutputDefStrings[]={"<Def>", "<no>", "<Yes>", 0};

LPSTR HoldOutputStringsShort[]={"<Def>", "no", "Yes", 0};
LPSTR HoldOutputDefStringsShort[]={"<Def>", "<no>", "<Yes>", 0};

DDBValueLst DDBHoldOutput[]=
  {
    {THO_Off,     HoldOutputStrings[THO_Off]      },
    {THO_On,      HoldOutputStrings[THO_On]       },
    {0}
  };
DDBValueLst DDBHoldOutputDef[]=
  {
    {THO_Default, HoldOutputStrings[THO_Default]  },
    {THO_Off,     HoldOutputStrings[THO_Off]      },
    {THO_On,      HoldOutputStrings[THO_On]       },
    {0}
  };

//--------------------------------------------------------------------------

LPSTR EPSStrategyStrings[]    = {"<Default>", "Maximum", "Minimum", "Priority", 0};
LPSTR EPSStrategyDefStrings[] = {"<Default>", "<Maximum>", "<Minimum>", "<Priority>", 0};

DDBValueLst DDBEPSStrategy[]=
  {
    {TVBTS_Maximum,  EPSStrategyStrings[TVBTS_Maximum]},
    {TVBTS_Minimum,  EPSStrategyStrings[TVBTS_Minimum]},
    {TVBTS_Priority, EPSStrategyStrings[TVBTS_Priority]},
    {0}
  };

DDBValueLst DDBEPSStrategyDef[]=
  {
    {TVBTS_Default,  EPSStrategyStrings[TVBTS_Default]},
    {TVBTS_Maximum,  EPSStrategyStrings[TVBTS_Maximum]},
    {TVBTS_Minimum,  EPSStrategyStrings[TVBTS_Minimum]},
    {TVBTS_Priority, EPSStrategyStrings[TVBTS_Priority]},
    {0}
  };

//--------------------------------------------------------------------------

LPSTR GetStrFlags(int j, int k, LPSTR *Strings, LPSTR *DefStrings, DDEF_Flags * Flags)  
  {
  if (k<0)
    return Strings[j];
  if (Flags)
    {
    *Flags |= DDEF_SHOWASDEFAULT;
    return Strings[j];
    }
  return DefStrings[j];
  };

//--------------------------------------------------------------------------

int TearConvergeMethod(int i, int j, int k)
  {
  if (k>TCM_Default) 
    return k;
  if (j>TCM_Default) 
    return j;
  return i;
  };

LPSTR TearConvergeMethodStr(int i, int j, int k, DDEF_Flags * Flags)
  {
  if (k>TCM_Default) 
    return TearMethodStrings[k];
  if (j>TCM_Default) 
    return GetStrFlags(j, k, TearMethodStrings, TearMethodDefStrings, Flags);
  return GetStrFlags(i, j, TearMethodStrings, TearMethodDefStrings, Flags);
  };

int FindTearConvergeMethod(LPCSTR Str)
  {
  for (int i=0; TearMethodStrings[i]!=NULL; i++)
    {
    if (_stricmp(Str, TearMethodStrings[i])==0)
      return i;
    }
  return -1;
  };

//--------------------------------------------------------------------------

int HoldOutput(int i, int j, int k)
  {
  if (k>THO_Default) 
    return k;
  if (j>THO_Default) 
    return j;
  return i;
  }

LPSTR HoldOutputStr(int i, int j, int k, DDEF_Flags * Flags)
  {
  if (k>THO_Default) 
    return HoldOutputStrings[k];
  if (j>THO_Default) 
    return GetStrFlags(j, k, HoldOutputStrings, HoldOutputDefStrings, Flags);
  return GetStrFlags(i, j, HoldOutputStrings, HoldOutputDefStrings, Flags);
  };

int FindHoldOutput(LPCSTR Str)
  {
  for (int i=0; HoldOutputStrings[i]!=NULL; i++)
    {
    if (_stricmp(Str, HoldOutputStrings[i])==0)
      return i;
    }
  return -1;
  };

//--------------------------------------------------------------------------

int DampAsGroup(int i, int j, int k)
  {
  if (k>DAG_Default) 
    return k;
  if (j>DAG_Default) 
    return j;
  return i;
  }

LPSTR DampAsGroupStr(int i, int j, int k, DDEF_Flags * Flags)
  {
  if (k>DAG_Default) 
    return TearMethodStrings[k];
  if (j>DAG_Default) 
    return GetStrFlags(j, k, DampAsGroupStrings, DampAsGroupDefStrings, Flags);
  return GetStrFlags(i, j, DampAsGroupStrings, DampAsGroupDefStrings, Flags);
  };
LPSTR DampAsGroupStrShort(int i, int j, int k)
  {
  if (k>DAG_Default) 
    return TearMethodStrings[k];
  if (j>DAG_Default) 
    return k<0 ? DampAsGroupStringsShort[j] : DampAsGroupDefStringsShort[j];
  return j<0 ? DampAsGroupStringsShort[i] : DampAsGroupDefStringsShort[i];
  };
int FindDampAsGroup(LPCSTR Str)
  {
  for (int i=0; DampAsGroupStrings[i]!=NULL; i++)
    {
    if (_stricmp(Str, DampAsGroupStrings[i])==0)
      return i;
    }
  //for (int i=0; i<DAG_Last; i++)
  //  {
  //  if (_stricmp(Str, DampAsGroupStringsShort[i])==0)
  //    {
  //    return i;
  //    }
  //  }
  return -1;
  };

//--------------------------------------------------------------------------

int EPSStrategy(int i, int j, int k)
  {
  if (k>TVBTS_Default) 
    return k;
  if (j>TVBTS_Default) 
    return j;
  return i;
  };

LPSTR EPSStrategyStr(int i, int j, int k, DDEF_Flags * Flags)
  {
  if (k>TVBTS_Default) 
    return EPSStrategyStrings[k];
  if (j>TVBTS_Default) 
    return GetStrFlags(j, k, EPSStrategyStrings, EPSStrategyDefStrings, Flags);
  return GetStrFlags(i, j, EPSStrategyStrings, EPSStrategyDefStrings, Flags);
  };

int FindEPSStrategy(LPCSTR Str)
  {
  for (int i=0; EPSStrategyStrings[i]!=NULL; i++)
    {
    if (_stricmp(Str, EPSStrategyStrings[i])==0)
      return i;
    }
  return -1;
  };

//--------------------------------------------------------------------------

// --------------------------------------------------------------------------

XID xidGlblTearMeth           = MdlBsXID(5510);
XID xidGlblTearMaxIters       = MdlBsXID(5511);
XID xidGlblTearHold           = MdlBsXID(5512);
XID xidGlblTearDampGroup      = MdlBsXID(5513);
XID xidGlblTearDampReqd       = MdlBsXID(5514);
XID xidGlblTearDampGrowth     = MdlBsXID(5515);
XID xidGlblTearDampDecay      = MdlBsXID(5516);
XID xidGlblTearWegDelay       = MdlBsXID(5517);
XID xidGlblTearWegBound       = MdlBsXID(5518);
XID xidGlblTearWegClamp       = MdlBsXID(5519);
XID xidGlblTearTolStrat       = MdlBsXID(5520);
XID xidGlblTearTolAbs         = MdlBsXID(5521);
XID xidGlblTearTolRel         = MdlBsXID(5522);

XID xidFlngTearMeth           = MdlBsXID(5530);
XID xidFlngTearCnvIters       = MdlBsXID(5531);
XID xidFlngTearNVar           = MdlBsXID(5532);
XID xidFlngTearHold           = MdlBsXID(5533);
XID xidFlngTearDampGroup      = MdlBsXID(5534);
XID xidFlngTearDampReqd       = MdlBsXID(5535);
XID xidFlngTearDampGrowth     = MdlBsXID(5536);
XID xidFlngTearDampDecay      = MdlBsXID(5537);
XID xidFlngTearWegDelay       = MdlBsXID(5538);
XID xidFlngTearWegBound       = MdlBsXID(5539);
XID xidFlngTearWegClamp       = MdlBsXID(5540);
XID xidFlngTearTolStrat       = MdlBsXID(5541);
XID xidFlngTearTolAbs         = MdlBsXID(5542);
XID xidFlngTearTolRel         = MdlBsXID(5543);

XID xidTearPriority       = MdlBsXID(9020);
XID xidTearType           = MdlBsXID(9021);
XID xidTearInitWhen       = MdlBsXID(9022);
XID xidTearInitHow        = MdlBsXID(9023);
XID xidTearInitEstUsage   = MdlBsXID(9024);
XID xidTearInitActive     = MdlBsXID(9025);
XID xidTearInitActivate   = MdlBsXID(9026);
XID xidTearCntRqd         = MdlBsXID(9027);
XID xidTearCntAct         = MdlBsXID(9028);
XID xidTearState          = MdlBsXID(9029);

XID xidTearQmMode         = MdlBsXID(9030);
XID xidTearQmRqd          = MdlBsXID(9031);
XID xidTearQvRqd          = MdlBsXID(9032);
XID xidTearNQvRqd         = MdlBsXID(9033);
XID xidTearPRqd           = MdlBsXID(9034);
XID xidTearTRqd           = MdlBsXID(9035);

//XID xidFlngTearMeth           = MdlBsXID(5530);
//XID xidFlngTearCnvIters       = MdlBsXID(5531);
//XID xidFlngTearNVar           = MdlBsXID(5532);
//XID xidFlngTearDampGrp        = MdlBsXID(5533);
//XID xidFlngTearDampReqd       = MdlBsXID(5534);
//xidFlngTearWegDelay,  
//xidFlngTearWegBound,  
//xidFlngTearWegClamp,  
//
//XID xidFlngTearTolStrat       = MdlBsXID(5535);
//XID xidFlngTearTolAbs         = MdlBsXID(5536);
//XID xidFlngTearTolRel         = MdlBsXID(5537);
XID xidFlngTearValues         = MdlBsXID(5550);

XID xidFlngTearBlock          = MdlBsXID(5552); // temporary
XID xidFlngTearSummary         = MdlBsXID(5553); // temporary

enum 
  {
  FTV_Input,
  FTV_Output,
  FTV_Error,
  FTV_Initial,
  FTV_Hold,
  FTV_Cvgd,
  FTV_Iters,
  FTV_EPS_Abs,
  FTV_EPS_Rel,
  FTV_Method,
  FTV_EPStrat,

  // max value 15 - 4 bits
  };

DDBValueLst DDBTearPri[]=
  {
    {TP_First,  "First"},
    {TP_Normal, "Normal"},
    {TP_Last,   "Last"},
    {0}
  };
DDBValueLst DDBTearTypeRqd[]=
  {
    {TT_NoTear,     "NoTear"},
    //{TT_SystemTear, "SystemTear"}, // Cannot Select this
    {TT_ManualTear, "ManualTear"},
    {TT_Break,      "Break"},
    {0}
  };
DDBValueLst DDBTearState[]=
  {
    {TT_NoTear,     "NoTear"},
    {TT_SystemTear, "SystemTear"},
    {TT_ManualTear, "ManualTear"},
    {TT_Break,      "Break"},
    {0}
  };


static DDBValueLst DDBHow[]=
  {
    {TIH_ZeroNIters,  "Zero"                       },
    {TIH_HoldNIters,  "Hold"                       },
    {TIH_RampNIters,  "Ramp"                       },
    {0}
  };
static DDBValueLst DDBWhen[]=
  {
    {TIW_OnInit,      "OnInitialise", MDD_Default  },
    {TIW_OnStart,     "OnStart"                    },
    //{TIW_Always,      "Always"                     },
    {TIW_Manual,      "OnDemand"                 },
    {0}
  };
//static DDBValueLst DDBInitQmMode[] =
//  {
//  //{ SPI_QModeNone, "None" },
//    { SPI_QModeQm,   "Mass" },
//    { SPI_QModeQv,   "Volume" },
//    { SPI_QModeNQv,  "NVolume" },
//  };
static DDBValueLst DDBInitEstUsage[]=
  {
  //{TIEU_None,     "None"},
    {TIEU_FullEst,  "Full"},
    {TIEU_PartEst,  "Partial"},
    {0},
  };
static DDBValueLst DDBInitState[]=                             
  {                                                            
    {TIH_Off,        "Off"                      },                       
    {TIH_ZeroNIters, "Zeroing"                  },                       
    {TIH_HoldNIters, "Holding"                  },                       
    {TIH_RampNIters, "Ramping"                  },                       

    //{TIH_ZeroNIters, "WillZero"                 },                       
    //{TIH_HoldNIters, "WillHold"                 },                       
    //{TIH_RampNIters, "WillRamp"                 },                       
    //{TIH_Zeroing,    "Zeroing"                  },
    //{TIH_Holding,    "Holding"                  },
    //{TIH_Ramping,    "Ramping"                  },
    {0},
  };

// --------------------------------------------------------------------------

void TearVarBlk::BuildDataDefnGlblTears(DataDefnBlk & DDB, TaggedObject * pOwner)
  {
  DDB.Text(StdSepUnderbar);
  DDB.Text("GLOBAL Tear Defaults");
  DDB.Text("Control:");
  DDB.String  ("GlblTear.Method",         "",       DC_,      "",   xidGlblTearMeth,           pOwner, isParm|SetOnChange, DDBTearMethod);
  DDB.Long    ("GlblTear.MaxIters",       "",       DC_,      "",   xidGlblTearMaxIters,       pOwner, isParm);
  DDB.String  ("GlblTear.Hold",           "",       DC_,      "",   xidGlblTearHold,           pOwner, isParm, DDBHoldOutput);
  DDB.Text("Damping:");
  DDB.String  ("GlblTear.Damp.Group",     "",       DC_,      "",   xidGlblTearDampGroup,      pOwner, isParm|SetOnChange, DDBDampAsGroup);
  DDB.Double  ("GlblTear.Damp.Reqd",      "",       DC_Frac,  "%",  xidGlblTearDampReqd,       pOwner, isParm);
  DDB.Double  ("GlblTear.Damp.Growth",    "",       DC_Frac,  "%",  xidGlblTearDampGrowth,     pOwner, isParm);
  DDB.Double  ("GlblTear.Damp.Decay",     "",       DC_Frac,  "%",  xidGlblTearDampDecay,      pOwner, isParm);
  DDB.Text("Wegstein:");
  DDB.Long    ("GlblTear.Wegs.Delay",     "",       DC_,      "",   xidGlblTearWegDelay,       pOwner, isParm);
  DDB.Double  ("GlblTear.Wegs.Bound",     "",       DC_,      "",   xidGlblTearWegBound,       pOwner, isParm);
  DDB.Double  ("GlblTear.Wegs.Clamp",     "",       DC_,      "",   xidGlblTearWegClamp,       pOwner, isParm);
  DDB.Text("Cnv Tolerance:");
  DDB.String  ("GlblTear.Tol.Strat",      "",       DC_,      "",   xidGlblTearTolStrat,       pOwner, (/*m_pVarBlk*/1?isParm:0)|SetOnChange, DDBEPSStrategy);
  DDB.Double  ("GlblTear.Tol.Abs",        "",       DC_Frac,  "%",  xidGlblTearTolAbs,         pOwner, isParm|DDEF_FLOATFMT);
  DDB.Double  ("GlblTear.Tol.Rel",        "",       DC_Frac,  "%",  xidGlblTearTolRel,         pOwner, isParm|DDEF_FLOATFMT);
  DDB.Text("");
  }

flag TearVarBlk::DataXchgGlblTears(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidGlblTearMeth:
      if (DCB.rpC)
        gs_EqnCB.Cfg.m_iConvergeMeth=FindTearConvergeMethod(DCB.rpC);
      DCB.pC = (LPSTR)TearConvergeMethodStr(gs_EqnCB.Cfg.m_iConvergeMeth);
      return 1;
    case xidGlblTearMaxIters:
      if (DCB.rL)
        gs_EqnCB.Cfg.m_iMaxIters=*DCB.rL;
      DCB.L=gs_EqnCB.Cfg.m_iMaxIters;
      return 1;
    case xidGlblTearHold:
      if (DCB.rpC)
        gs_EqnCB.Cfg.m_iHoldOutput=FindHoldOutput(DCB.rpC);
      DCB.pC=HoldOutputStr(gs_EqnCB.Cfg.m_iHoldOutput);
      return 1;
    case xidGlblTearDampGroup:
      if (DCB.rpC)
        gs_EqnCB.Cfg.m_iDampAsGroup=FindDampAsGroup(DCB.rpC);
      DCB.pC=(LPSTR)DampAsGroupStr(gs_EqnCB.Cfg.m_iDampAsGroup);
      return 1;
    case xidGlblTearDampReqd:
      if (DCB.rD)
        gs_EqnCB.Cfg.m_DampingRqd = *DCB.rD;
      DCB.D=gs_EqnCB.Cfg.m_DampingRqd;
      return 1;
    case xidGlblTearDampGrowth:
      if (DCB.rD)
        gs_EqnCB.Cfg.m_DampFctGrowth = *DCB.rD;
      DCB.D=gs_EqnCB.Cfg.m_DampFctGrowth;
      return 1;
    case xidGlblTearDampDecay:
      if (DCB.rD)
        gs_EqnCB.Cfg.m_DampFctDecay = *DCB.rD;
      DCB.D=gs_EqnCB.Cfg.m_DampFctDecay;
      return 1;
    case xidGlblTearWegDelay:
      if (DCB.rL)
        gs_EqnCB.Cfg.m_iWA_Delay = (short)*DCB.rL;
      DCB.L=gs_EqnCB.Cfg.m_iWA_Delay;
      return 1;
    case xidGlblTearWegBound:
      if (DCB.rD)
        gs_EqnCB.Cfg.m_WA_Bound = *DCB.rD;
      DCB.D=gs_EqnCB.Cfg.m_WA_Bound;
      return 1;
    case xidGlblTearWegClamp:
      if (DCB.rD)
        gs_EqnCB.Cfg.m_WA_Clamping = *DCB.rD;
      DCB.D=gs_EqnCB.Cfg.m_WA_Clamping;
      return 1;
    case xidGlblTearTolStrat:
      if (DCB.rpC)
        gs_EqnCB.Cfg.m_iEPSStrategy=FindEPSStrategy(DCB.rpC);
      DCB.pC=(LPSTR)::EPSStrategyStr(gs_EqnCB.Cfg.m_iEPSStrategy);
      return 1;
    case xidGlblTearTolAbs:
      if (DCB.rD)
        gs_EqnCB.Cfg.m_EPS_Abs = *DCB.rD;
      DCB.D=gs_EqnCB.Cfg.m_EPS_Abs;
      return 1;
    case xidGlblTearTolRel:
      if (DCB.rD)
        gs_EqnCB.Cfg.m_EPS_Rel = *DCB.rD;
      DCB.D=gs_EqnCB.Cfg.m_EPS_Rel;
      return 1;
    }
  return 0;
  };

void TearVarBlk::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.ForFileSnpScn())
    {
    BuildDataDefnFile(DDB);
    return;
    }

  if (DDB.BeginStruct(this))
    {
    //if ((m_iNameWidth<0) && /*m_pVarBlk && (m_pVarBlk->*/NVariables()>0))
    if ((m_iSymWidth<0) && (NVariables()>0))
      {
      int N=NVariables();
      m_iSymWidth=8;
      m_iTagWidth=8;
      for (int i=0; i<N; i++)
        {
        m_iSymWidth=Max(m_iSymWidth, TV[i].m_Sym.GetLength()+1);
        m_iTagWidth=Max(m_iTagWidth, TV[i].m_Tag.GetLength()+1);
        }
      }

    //if (!DDB.ForFileSnpScn())
    //  {
    //DDB.Text(StdSepUnderbar);
    //DDB.Text("This flange has a tear in place");
    //DDB.String  ("TearBlock",               "",       DC_,      "",   xidFlngTearBlock,           this, isTag);//isParm);
    //DDB.String  ("TearBlocks",              "",       DC_,      "",   xidFlngTearSummary,           this, isTag);//isParm);

    DDB.String  ("TearSummary",               "",       DC_,      "",   xidFlngTearSummary,           this, isTag|noFileAtAll);//isParm);
    //  DDB.String  ("TearBlock",                "",       DC_,      "",   xidFlngTearBlock,           this, isTag|noFileAtAll);//isParm);

    DDB.Text("");

    DDB.Byte        ("Priority",      "",   DC_,    "",       xidTearPriority,     this, isParm|SetOnChange, DDBTearPri);
    DDB.Byte        ("RqdType",       "",   DC_,    "",       xidTearType,         this, isParm|SetOnChange, DDBTearTypeRqd);
    DDB.Byte        ("State",         "",   DC_,    "",       xidTearState,        this, 0, DDBTearState);

    DDB.Text("");

    BuildDataDefnGlblTears(DDB, this);

    DDB.Text(StdSepUnderbar);
    DDB.Text("Tear Defaults for THIS tear");
    DDB.Text("Control:");
    DDB.String  ("Tear.Method",             "",       DC_,      "",   xidFlngTearMeth,        this, (/*m_pVarBlk*/1?isParm:0)|SetOnChange, DDBTearMethodDef);
    DDB.Long    ("Tear.CnvIters",           "",       DC_,      "",   xidFlngTearCnvIters,    this, 0);//isParm);
    DDB.Long    ("Tear.VarCount",           "",       DC_,      "",   xidFlngTearNVar,        this, 0);//isParm);
    DDB.String  ("Tear.Hold",               "",       DC_,      "",   xidFlngTearHold,        this, (/*m_pVarBlk*/1?isParm:0), DDBHoldOutputDef);
    DDB.Text("Damping:");                                                                               
    DDB.String  ("Tear.Damp.Group",         "",       DC_,      "",   xidFlngTearDampGroup,      this, (/*m_pVarBlk*/1?isParm:0)|SetOnChange, DDBDampAsGroupDef);
    DDB.Double  ("Tear.Damp.Reqd",          "",       DC_Frac,  "%",  xidFlngTearDampReqd,       this, (/*m_pVarBlk*/1?isParm:0)|NAN_OK);
    DDB.Double  ("Tear.Damp.Growth",        "",       DC_Frac,  "%",  xidFlngTearDampGrowth,     this, (/*m_pVarBlk*/1?isParm:0)|NAN_OK);
    DDB.Double  ("Tear.Damp.Decay",         "",       DC_Frac,  "%",  xidFlngTearDampDecay,      this, (/*m_pVarBlk*/1?isParm:0)|NAN_OK);
    //DDB.Text("Wegstein:");
    //DDB.Long    ("Tear.Wegs.Delay",         "",       DC_,      "",   xidFlngTearWegDelay,       this, isParm);
    //DDB.Double  ("Tear.Wegs.Bound",         "",       DC_,      "",   xidFlngTearWegBound,       this, isParm);
    //DDB.Double  ("Tear.Wegs.Clamp",         "",       DC_,      "",   xidFlngTearWegClamp,       this, isParm);
    DDB.Text("Cnv Tolerance:");
    DDB.String  ("Tear.Tol.Strat",          "",       DC_,      "",   xidFlngTearTolStrat,       this, (/*m_pVarBlk*/1?isParm:0)|SetOnChange, DDBEPSStrategyDef);
    DDB.Double  ("Tear.Tol.Abs",            "",       DC_Frac,  "%",  xidFlngTearTolAbs,         this, (/*m_pVarBlk*/1?isParm:0)|NAN_OK|DDEF_FLOATFMT);
    DDB.Double  ("Tear.Tol.Rel",            "",       DC_Frac,  "%",  xidFlngTearTolRel,         this, (/*m_pVarBlk*/1?isParm:0)|NAN_OK|DDEF_FLOATFMT);
    DDB.Text("");

    if (m_pPosBlk)
      {

      DDB.Text(StdSepUnderbar);
      DDB.Text("Tear Initialisation");

      bool TearVis=true && (RqdTearType()>=TT_ManualTear);
      DDB.Visibility(NSHM_All, TearVis);
      //DDB.Page("Initial", DDB_RqdPage);

      DDB.Byte        ("Tear.Init.How",      "",   DC_,    "",       xidTearInitHow,      this, isParmStopped|SetOnChange, DDBHow);
      DDB.Byte        ("Tear.Init.When",     "",   DC_,    "",       xidTearInitWhen,     this, isParmStopped|SetOnChange, DDBWhen);
      static DDBValueLst DDBStart[] = {{0, "Start"}, {1, "Busy"}, {0}};
      DDB.Button      ("Tear.Init.Start",   "",   DC_,    "",       xidTearInitActivate, this, isParm|SetOnChange);//, DDBStart);
      bool HoldCntVis=true;
      DDB.Visibility(NSHM_All, TearVis && HoldCntVis);
      DDB.Byte        ("Tear.Init.Count",    "",   DC_,    "",       xidTearCntRqd,       this, isParmStopped);
      DDB.Byte        ("Tear.Init.Iters",    "",   DC_,    "",       xidTearCntAct,       this, InitHidden);
      DDB.Visibility(NSHM_All, TearVis);

      DDB.Byte        ("Tear.Init.State",    "",   DC_,    "",       xidTearInitActive,   this, 0, DDBInitState);

      //SpImgMode Md=SPI_QModeNone;
      //DDBValueLstMem DDBInitQmModeX;
      ////DDBInitQmModeX.Add(SPI_QModeNone, "None");
      //DDBInitQmModeX.Add(SPI_QModeQm,   "Mass");

      //if (NoProcessIOs()>0 && IOFlange(0) && IOFlange(0)->TearImage())
      //  {
      //  Md=IOFlange(0)->TearImage()->QMode();

      //  SpImage &Img=*IOFlange(0)->TearImage();
      //  if (Img.SpVarsAvail() & VAMsk_SpVol)
      //    DDBInitQmModeX.Add(SPI_QModeQv,   "Volume");
      //  if (Img.SpVarsAvail() & VAMsk_SpNVol)
      //    DDBInitQmModeX.Add(SPI_QModeNQv,  "NVolume");
      //  }

      //DDB.Visibility(NSHM_All, TearVis);
      //DDB.Double      ("T_Rqd",         "",   DC_T,   "C",      xidTearTRqd,         this, isParm       |NAN_OK, DDBNAN_NotSpecd);
      //DDB.Double      ("P_Rqd",         "",   DC_P,   "kPag",   xidTearPRqd,         this, isParm       |NAN_OK, DDBNAN_NotSpecd);
      //DDB.Byte        ("Estimate.Usage","",   DC_,    "",       xidTearInitEstUsage, this, isParmStopped|SetOnChange, DDBInitEstUsage);
      //DDB.Byte        ("Flow.Mode",     "",   DC_,    "",       xidTearQmMode,       this, isParmStopped|SetOnChange, DDBInitQmModeX());
      //DDB.Visibility(NSHM_All, TearVis && (Md!=SPI_QModeNone));
      //DDB.Double      ("Qm_Rqd",        "",   DC_Qm,  "kg/s",   xidTearQmRqd,        this, (Md==SPI_QModeQm?isParm:isResult)|NAN_OK, DDBNAN_NotSpecd);
      //DDB.Double      ("Qv_Rqd",        "",   DC_Qv,  "L/s",    xidTearQvRqd,        this, (Md==SPI_QModeQv?isParm:isResult)|NAN_OK, DDBNAN_NotSpecd);
      //DDB.Double      ("NQv_Rqd",       "",   DC_NQv, "NL/s",   xidTearNQvRqd,       this, (Md==SPI_QModeNQv?isParm:isResult)|NAN_OK, DDBNAN_NotSpecd);
      DDB.Visibility();
      }

    }
  DDB.EndStruct();    

  BuildDataDefnTearValues(DDB);
  BuildDataDefnTearMethod(DDB);
  //BuildDataDefnInitialVars(DDB);

  };

// --------------------------------------------------------------------------

void TearVarBlk::BuildDataDefnTearValues(DataDefnBlk & DDB)
  {
  int N=NVariables();

  if (N>0)
    {
    DDB.Page("Values", 1/*N>5*/ ? DDB_RqdPage:DDB_NoPage);
    DDB.BeginGrid("ABC", 6, N, 9, 1);

    DDB.ColumnHeader("Variable",   GetDisplayTagsOnly()? m_iTagWidth:m_iSymWidth, 1, -1);
    DDB.ColumnHeader("Input",     12, 0, 1);
    DDB.ColumnHeader("Error",     12, 1, 1);
    DDB.ColumnHeader("Cvgd",       4, 1, 1);
    DDB.ColumnHeader("Count",      5, 1, 1);
    DDB.ColumnHeader("EPS_Abs",   10, 0, 1);
    DDB.ColumnHeader("EPS_Rel",   10, 0, 1);

    if (DDB.BeginArray(this, "Tear.Values", NULL, DDB_NoPage))
      {
      for (int i=0; i<N; i++)
        {
        TearVar & V = TV[i];
        LPSTR Txt = GetDisplayTagsOnly() ? V.m_Tag() : V.m_Sym();
        CnvAttribute * pCnv = V.m_pMeasCnv;
        CnvAttribute * pDCnv = V.m_pDiffCnv;
        DDB.RowHeader(Txt);
        DDB.RowStart();

        DDB.BeginElement(this, Txt);
        DDB.Double  ("Input",   "", pCnv->Index(),      pCnv->Text(),  xidFlngTearValues, (i<<4) | FTV_Input,   this, 0);
        DDB.Double  ("Error",   "", pDCnv->Index(),     pDCnv->Text(), xidFlngTearValues, (i<<4) | FTV_Error,   this, 0|DDEF_FLOATFMT);
        DDB.Long    ("Cvgd",    "", DC_,                "",            xidFlngTearValues, (i<<4) | FTV_Cvgd,    this, 0);
        DDB.Long    ("Iters",   "", DC_,                "",            xidFlngTearValues, (i<<4) | FTV_Iters,   this, 0);
        DDB.Double  ("EPS_Abs", "", pDCnv->Index(),     pDCnv->Text(), xidFlngTearValues, (i<<4) | FTV_EPS_Abs, this, isParm|NAN_OK|DDEF_FLOATFMT);
        DDB.Double  ("EPS_Rel", "", DC_Frac,            "%",           xidFlngTearValues, (i<<4) | FTV_EPS_Rel, this, isParm|NAN_OK|DDEF_FLOATFMT);
        }
      }
    DDB.EndArray();
    DDB.EndGrid();
    }
  };

// --------------------------------------------------------------------------

void TearVarBlk::BuildDataDefnTearMethod(DataDefnBlk & DDB)
  {
  int N=NVariables();

  if (N>0)
    {
    bool InitVis=(RqdTearType()>=TT_ManualTear);
    if (N>10)
      DDB.Page("Method", N>10 ? DDB_RqdPage:DDB_NoPage);
    DDB.BeginGrid("AB", InitVis?5:4, N, 9, 1);

    DDB.ColumnHeader("Variable",   GetDisplayTagsOnly()? m_iTagWidth:m_iSymWidth, 1, -1);
    DDB.ColumnHeader("Input",     12, 0, 1);
    DDB.ColumnHeader("Output",    12, 1, 1);
    DDB.ColumnHeader("Hold",       5, 2, -1);
    if (InitVis)
      DDB.ColumnHeader("InitValue",  12, 2, -1);
    DDB.ColumnHeader("Method",    12, 2, -1);

    if (DDB.BeginArray(this, "Tear.Methods", NULL, DDB_NoPage))
      {
      for (int i=0; i<N; i++)
        {
        TearVar & V = TV[i];
        LPSTR Txt = GetDisplayTagsOnly() ? V.m_Tag() : V.m_Sym();
        CnvAttribute * pCnv = V.m_pMeasCnv;
        DDB.RowHeader(Txt);
        DDB.RowStart();

        DDB.BeginElement(this, Txt);
        DDB.Double  ("Input",   "", pCnv->Index(),      pCnv->Text(),  xidFlngTearValues, (i<<4) | FTV_Input,   this, 0);
        DDB.Double  ("Output",  "", pCnv->Index(),      pCnv->Text(),  xidFlngTearValues, (i<<4) | FTV_Output,  this, V.HoldOutput()?isParm:0);
        DDB.String  ("Hold",    "", DC_,                "",            xidFlngTearValues, (i<<4) | FTV_Hold,    this, isParm/*|SetOnChange*/, DDBHoldOutputDef);
        if (InitVis)
          DDB.Double("Initial", "", pCnv->Index(),      pCnv->Text(),  xidFlngTearValues, (i<<4) | FTV_Initial,  this, isParm);
        DDB.String  ("Method",  "", DC_,                "",            xidFlngTearValues, (i<<4) | FTV_Method,  this, isParm, DDBTearMethodDef);
        }
      }
    DDB.EndArray();
    DDB.EndGrid();
    }
  };

// --------------------------------------------------------------------------


void TearVarBlk::BuildDataDefnInitialVars(DataDefnBlk & DDB)
  {
  int N=NVariables();

  if (N>0)
    {
    if (N>10)
      DDB.Page("Initial", N>10 ? DDB_RqdPage:DDB_NoPage);
    DDB.Text(" ");
    DDB.BeginGrid("AB", 1, N, 9, 1);

    DDB.ColumnHeader("Variable",   GetDisplayTagsOnly()? m_iTagWidth:m_iSymWidth, 1, -1);
    //DDB.ColumnHeader("Value",     10, 0, 1);
    DDB.ColumnHeader("Output",    10, 1, 1);
    ////DDB.ColumnHeader("Error",     10, 1, 1);
    //DDB.ColumnHeader("Hold",       5, 2, -1);
    //DDB.ColumnHeader("Method",    12, 2, -1);
    //DDB.ColumnHeader("EPS_Abs",    8, 0, 1);
    //DDB.ColumnHeader("EPS_Rel",    8, 0, 1);
    //DDB.ColumnHeader("Value",      9, 0, 1);

    if (DDB.BeginArray(this, "Tear.Initial", NULL, DDB_NoPage))
      {
      for (int i=0; i<N; i++)
        {
        TearVar & V = TV[i];
        LPSTR Txt = GetDisplayTagsOnly() ? V.m_Tag() : V.m_Sym();
        CnvAttribute * pCnv = V.m_pMeasCnv;
        //CnvAttribute * pDCnv = V.m_pDiffCnv;
        //LPSTR Txt=TV[i].m_Sym();
        DDB.RowHeader(Txt);
        DDB.RowStart();

        DDB.BeginElement(this, Txt);
        //DDB.Double  ("Input",   "", pCnv->Index(),      pCnv->Text(),  xidFlngTearValues, (i<<4) | FTV_Input,   this, 0);
        DDB.Double  ("Output",  "", pCnv->Index(),      pCnv->Text(),  xidFlngTearValues, (i<<4) | FTV_Output,  this, V.HoldOutput()?isParm:0);
        //DDB.String  ("Hold",    "", DC_,                "",            xidFlngTearValues, (i<<4) | FTV_Hold,    this, isParm/*|SetOnChange*/, DDBHoldOutputDef);
        //DDB.String  ("Method",  "", DC_,                "",            xidFlngTearValues, (i<<4) | FTV_Method,  this, isParm, DDBTearMethodDef);
        //DDB.Double  ("Method",  "", DC_,                "",            xidFlngTearValues, (i<<4) | FTV_Method,  this, isParm);
        //DDB.RowEnd();
        }
      }
    DDB.EndArray();
    DDB.EndGrid();
    }
  //if (m_pPosBlk)
  //  {

  //  bool TearVis=true;
  //  bool InitVis=(RqdTearType()>=TT_ManualTear);
  //  DDB.Visibility(NSHM_All, TearVis);
  //  DDB.Page("Initial", DDB_RqdPage);

  //  if (DDB.BeginStruct(this, "Tear.Init", NULL, DDB_NoPage))
  //    {
  //    DDB.Byte        ("How",      "",   DC_,    "",       xidTearInitHow,      this, isParmStopped|SetOnChange, DDBHow);
  //    DDB.Byte        ("When",     "",   DC_,    "",       xidTearInitWhen,     this, isParmStopped|SetOnChange, DDBWhen);
  //    static DDBValueLst DDBStart[] = {{0, "Start"}, {1, "Busy"}, {0}};
  //    DDB.Button      ("Start",   "",   DC_,    "",       xidTearInitActivate, this, isParm|SetOnChange);//, DDBStart);
  //    bool HoldCntVis=true;
  //    DDB.Visibility(NSHM_All, TearVis && HoldCntVis);
  //    DDB.Byte        ("Count",    "",   DC_,    "",       xidTearCntRqd,       this, isParmStopped);
  //    DDB.Byte        ("Iters",    "",   DC_,    "",       xidTearCntAct,       this, InitHidden);
  //    DDB.Visibility(NSHM_All, TearVis);

  //    DDB.Byte        ("State",    "",   DC_,    "",       xidTearInitActive,   this, 0, DDBInitState);

  //    //SpImgMode Md=SPI_QModeNone;
  //    //DDBValueLstMem DDBInitQmModeX;
  //    ////DDBInitQmModeX.Add(SPI_QModeNone, "None");
  //    //DDBInitQmModeX.Add(SPI_QModeQm,   "Mass");

  //    //if (NoProcessIOs()>0 && IOFlange(0) && IOFlange(0)->TearImage())
  //    //  {
  //    //  Md=IOFlange(0)->TearImage()->QMode();

  //    //  SpImage &Img=*IOFlange(0)->TearImage();
  //    //  if (Img.SpVarsAvail() & VAMsk_SpVol)
  //    //    DDBInitQmModeX.Add(SPI_QModeQv,   "Volume");
  //    //  if (Img.SpVarsAvail() & VAMsk_SpNVol)
  //    //    DDBInitQmModeX.Add(SPI_QModeNQv,  "NVolume");
  //    //  }

  //    DDB.Visibility(NSHM_All, TearVis);
  //    DDB.Double      ("T_Rqd",         "",   DC_T,   "C",      xidTearTRqd,         this, isParm       |NAN_OK, DDBNAN_NotSpecd);
  //    DDB.Double      ("P_Rqd",         "",   DC_P,   "kPag",   xidTearPRqd,         this, isParm       |NAN_OK, DDBNAN_NotSpecd);
  //    DDB.Byte        ("Estimate.Usage","",   DC_,    "",       xidTearInitEstUsage, this, isParmStopped|SetOnChange, DDBInitEstUsage);
  //    //DDB.Byte        ("Flow.Mode",     "",   DC_,    "",       xidTearQmMode,       this, isParmStopped|SetOnChange, DDBInitQmModeX());
  //    //DDB.Visibility(NSHM_All, TearVis && (Md!=SPI_QModeNone));
  //    //DDB.Double      ("Qm_Rqd",        "",   DC_Qm,  "kg/s",   xidTearQmRqd,        this, (Md==SPI_QModeQm?isParm:isResult)|NAN_OK, DDBNAN_NotSpecd);
  //    //DDB.Double      ("Qv_Rqd",        "",   DC_Qv,  "L/s",    xidTearQvRqd,        this, (Md==SPI_QModeQv?isParm:isResult)|NAN_OK, DDBNAN_NotSpecd);
  //    //DDB.Double      ("NQv_Rqd",       "",   DC_NQv, "NL/s",   xidTearNQvRqd,       this, (Md==SPI_QModeNQv?isParm:isResult)|NAN_OK, DDBNAN_NotSpecd);
  //    }
  //  DDB.EndStruct();
  //  }
  }

// --------------------------------------------------------------------------

void TearVarBlk::BuildDataDefnFile(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this))
    {
    DDB.String("GroupTag",        "",    DC_,     "",     &sGroupTag,                 this, 0);//isParmConstruct);//0isParm);
    DDB.Long  ("NVars",           "",    DC_,     "",     xidNTearVars,               this, 0);//isParmConstruct);//0isParm);
    DDB.Long  ("NHistory",        "",    DC_,     "",     xidNTearHistory,            this, isParmStopped);
    DDB.Byte  ("TearMethod",      "",    DC_,     "",     &m_iTearMethod,             this, isParm, DDBTearMethodDef);
    DDB.Byte  ("Hold",            "",    DC_,     "",     &m_iHoldOutput,             this, isParm, DDBHoldOutputDef);
    DDB.Byte  ("DampTogether",    "",    DC_,     "",     &m_iDampAsGroup,            this, isParm, DDBDampAsGroupDef);
    DDB.Double("DampingRqd",      "",    DC_Frac, "%",    &m_DampingRqd,              this, isParm|NAN_OK);
    DDB.Double("DampFctDecay",    "",    DC_Frac, "%",    &m_DampFctDecay,            this, isParm|NAN_OK);
    DDB.Double("DampFctGrowth",   "",    DC_Frac, "%",    &m_DampFctGrowth,           this, isParm|NAN_OK);
    DDB.Byte  ("EPS_Strategy",    "",    DC_,     "",     &m_iEPSStrategy,            this, isParm, DDBEPSStrategy);
    DDB.Double("EPS_Abs",         "",    DC_Frac, "%",    &m_EPS_Abs,                 this, isParm|NAN_OK);
    DDB.Double("EPS_Rel",         "",    DC_Frac, "%",    &m_EPS_Rel,                 this, isParm|NAN_OK);
    DDB.Long  ("ConvergedCnt",    "",    DC_,     "",     &m_lSeqConvergedCnt,        this, isParmConstruct|InitHidden);//noFileAtAll);
    DDB.Long  ("ConvergeLoopCnt", "",    DC_,     "",     &m_lConvergeLoopCnt,        this, 0);//noFileAtAll);
    DDB.Long  ("Cnt",             "",    DC_,     "",     &m_lStageCnt,               this, isParmConstruct|InitHidden);//noFileAtAll);

    if (DDB.BeginArray(this, "V", "TearVars", m_nVariables))
      {
      for (int iVar=0; iVar<m_nVariables; iVar++)
        {
        TearVar & Ti=TV[iVar];
        if (DDB.BeginElement(this, SymOrTag(iVar)))
          {
          //DDB.String("Tag",             "",   DC_,         "",     &Ti.m_Tag,               this, noFileAtAll|InitHidden);
          //DDB.String("Sym",             "",   DC_,         "",     &Ti.m_Sym,               this, noFileAtAll|InitHidden);
          //DDB.String("Name",            "",   DC_,         "",     &Ti.m_Name,              this, InitHidden);
          DDB.Byte  ("TearMethod",      "",   DC_,         "",     &Ti.m_iTearMethod,       this, isParm, DDBTearMethodDef);
          DDB.Byte  ("Hold",            "",   DC_,         "",     &Ti.m_iHoldOutput,       this, isParm, DDBHoldOutputDef);
          DDB.Double("DampingRqd",      "",   DC_Frac,     "%",    &Ti.m_DampingRqd,        this, isParm|NAN_OK);
          DDB.Double("DampFactor",      "",   DC_,         "",     &Ti.m_DampFactor,        this, isParmConstruct|DDEF_NOCOMPARE);
          DDB.Double("EstDampFactor",   "",   DC_,         "",     &Ti.m_EstDampFactor,     this, isParmConstruct|InitHidden|DDEF_NOCOMPARE);
          DDB.Double("dYRat",           "",   DC_,         "",     &Ti.m_YRat,             this, isParmConstruct|InitHidden|DDEF_NOCOMPARE);
          DDB.Bool  ("TestIsValid",     "",   DC_,         "",     &Ti.m_bTestIsValid,      this, isParmConstruct|InitHidden);
          DDB.Double("EPS_Abs",         "",   DC_Frac,     "%",    &Ti.m_EPS_Abs,           this, isParm|NAN_OK);
          DDB.Double("EPS_Rel",         "",   DC_Frac,     "%",    &Ti.m_EPS_Rel,           this, isParm|NAN_OK);
          //DDB.Bool  ("HoldOutput",      "",   DC_,         "",     &Ti.m_bHoldOutput,       this, isParmConstruct|InitHidden);
          DDB.Byte  ("ConvergedCnt",    "",   DC_,         "",     &Ti.m_iConvergedCnt,     this, isParmConstruct|InitHidden);
          //DDB.Byte  ("Cnt",             "",   DC_,         "",     &Ti.iCnt,              this, isParmConstruct|InitHidden);

          if (bUsedByBlk & TVU_Inputs)
            {
            CnvAttribute &C=*Ti.m_pMeasCnv;
            FmtAttribute &F=*Ti.m_pMeasFmt;
            DDB.Double("Y0",             "",   C.Index(), C.Text(), &Ti.m_Y[0],       this, isParmConstruct|DDEF_NOCOMPARE);
            DDB.Double("Y1",             "",   C.Index(), C.Text(), &Ti.m_Y[1],       this, isParmConstruct|InitHidden|NAN_OK|DDEF_NOCOMPARE);
            DDB.Double("Y2",             "",   C.Index(), C.Text(), &Ti.m_Y[2],       this, isParmConstruct|InitHidden|NAN_OK|DDEF_NOCOMPARE);
            DDB.Double("Y3",             "",   C.Index(), C.Text(), &Ti.m_Y[3],       this, isParmConstruct|InitHidden|NAN_OK|DDEF_NOCOMPARE);
            }
          if (bUsedByBlk & TVU_Outputs)
            {
            CnvAttribute &C=*Ti.m_pMeasCnv;
            FmtAttribute &F=*Ti.m_pMeasFmt;
            DDB.Double("X0",             "",   C.Index(), C.Text(), &Ti.m_X[0],       this, isParmConstruct|DDEF_NOCOMPARE);//isParmConstruct);
            DDB.Double("X1",             "",   C.Index(), C.Text(), &Ti.m_X[1],       this, isParmConstruct|InitHidden|NAN_OK|DDEF_NOCOMPARE);
            DDB.Double("X2",             "",   C.Index(), C.Text(), &Ti.m_X[2],       this, isParmConstruct|InitHidden|NAN_OK|DDEF_NOCOMPARE);
            DDB.Double("X3",             "",   C.Index(), C.Text(), &Ti.m_X[3],       this, isParmConstruct|InitHidden|NAN_OK|DDEF_NOCOMPARE);
            }

          if (bUsedByBlk & TVU_Meas)
            {
            CnvAttribute &C=*Ti.m_pMeasCnv;
            FmtAttribute &F=*Ti.m_pMeasFmt;
            DDB.Double("CurTol",         "",   DC_Frac,     "%",    &Ti.m_CurTol,    this, isParmConstruct|InitHidden);
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

// --------------------------------------------------------------------------

flag TearVarBlk::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;

  if (DataXchgGlblTears(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidNTearVars:
      DCB.L=NVariables();
      return 1;

    case xidNTearHistory:
      if (DCB.rL)
        SetNHistory(*DCB.rL);
      DCB.L=NHistory();
      return 1;

    case xidTearType:
      {
      if (DCB.rB)
        SetRqdTearType(*DCB.rB);
      //if (RqdTearType()>=TT_ManualTear)
      //  SetTearImage(this);
      //else
      //  ClrTearImage(this);
      //UpdateSQFlags();
      //DCB.B=Max(RqdTearType(), IOFlange(1)->RqdTearType());
      DCB.B=RqdTearType();
      return 1;
      }
    case xidTearState:
      {
      if (DCB.rB)// && NoProcLnkIOs()>=2)
        {
        SetTearType(*DCB.rB);//, this);
        //if (TearType()!=TT_NoTear)
        //  SetTearImage(this);
        }
      //if (NoProcLnkIOs()>=2)
      //DCB.B=Max(TearType(), IOFlange(1)->TearType());
      DCB.B=TearType();
      //else
      //  DCB.B=TT_NoTear;
      return 1;
      }
    case xidTearPriority:
      {
      int iT=0;
      if (DCB.rB)// && NoProcLnkIOs()>=2)
        SetTearPriority(*DCB.rB);
      //if (NoProcLnkIOs()>=2)
      //  DCB.B=Min(TearPriority(), IOFlange(1)->TearPriority());
      DCB.B=TearPriority();
      //else
      //  DCB.B=TP_Normal;
      return 1;
      }





    case xidFlngTearBlock:
      DCB.pC=TearGetTag();
      return 1;
    case xidFlngTearSummary:
      DCB.pC="$Tear";
      return 1;

    case xidFlngTearMeth:
      if (DCB.rpC)
        SetConvergeMethod(DCB.rpC);
      DCB.pC = TearConvergeMethodStr(gs_EqnCB.Cfg.m_iConvergeMeth, ConvergeMethod(), -1, DCB.ForView()?&DCB.dwRetFlags:NULL);
      return 1;
    case xidFlngTearNVar:
      DCB.L = NVariables();
      return 1;
    case xidFlngTearHold:
      if (DCB.rpC)
        m_iHoldOutput=FindHoldOutput(DCB.rpC);
      DCB.pC = HoldOutputStr(gs_EqnCB.Cfg.m_iHoldOutput, m_iHoldOutput, -1, DCB.ForView()?&DCB.dwRetFlags:NULL);
      return 1;
    case xidFlngTearCnvIters:
      DCB.L = m_lConvergeLoopCnt;
      return 1;
    case xidFlngTearDampGroup:
      if (DCB.rpC)
        m_iDampAsGroup=FindDampAsGroup(DCB.rpC);
      DCB.pC = DampAsGroupStr(gs_EqnCB.Cfg.m_iDampAsGroup, DampAsGroupCfg(), -1, DCB.ForView()?&DCB.dwRetFlags:NULL);
      return 1;
    case xidFlngTearDampReqd:
      if (DCB.rD)
        m_DampingRqd=*DCB.rD;
      if (Valid(m_DampingRqd))
        DCB.D = m_DampingRqd;
      else
        {
        DCB.D = gs_EqnCB.Cfg.m_DampingRqd;
        DCB.dwRetFlags=DDEF_SHOWASDEFAULT;
        }
      return 1;

    case xidFlngTearDampGrowth:
      if (DCB.rD)
        m_DampFctGrowth=*DCB.rD;
      if (Valid(m_DampFctGrowth))
        DCB.D = m_DampFctGrowth;
      else
        {
        DCB.D = gs_EqnCB.Cfg.m_DampFctGrowth;
        DCB.dwRetFlags=DDEF_SHOWASDEFAULT;
        }
      return 1;

    case xidFlngTearDampDecay:
      if (DCB.rD)
        m_DampFctDecay=*DCB.rD;
      if (Valid(m_DampFctDecay))
        DCB.D = m_DampFctDecay;
      else
        {
        DCB.D = gs_EqnCB.Cfg.m_DampFctDecay;
        DCB.dwRetFlags=DDEF_SHOWASDEFAULT;
        }
      return 1;

    case xidFlngTearTolStrat:
      if (DCB.rpC)
        m_iEPSStrategy=FindEPSStrategy(DCB.rpC);
      DCB.pC = ::EPSStrategyStr(gs_EqnCB.Cfg.m_iEPSStrategy, m_iEPSStrategy, -1, DCB.ForView()?&DCB.dwRetFlags:NULL);
      return 1;

    case xidFlngTearTolAbs:
      if (DCB.rD)
        m_EPS_Abs=*DCB.rD;
      if (Valid(m_EPS_Abs))
        DCB.D = m_EPS_Abs;
      else
        {
        DCB.D = gs_EqnCB.Cfg.m_EPS_Abs;
        DCB.dwRetFlags=DDEF_SHOWASDEFAULT;
        }
      return 1;

    case xidFlngTearTolRel:
      if (DCB.rD)
        m_EPS_Rel=*DCB.rD;
      if (Valid(m_EPS_Rel))
        DCB.D = m_EPS_Rel;
      else
        {
        DCB.D = gs_EqnCB.Cfg.m_EPS_Rel;
        DCB.dwRetFlags=DDEF_SHOWASDEFAULT;
        }
      return 1;

    case xidFlngTearValues:
      {
      int i=DCB.lHandle2 >> 4;
      switch (DCB.lHandle2 & 0xf)
        {
        case FTV_Hold:
          if (DCB.rpC)
            TV[i].m_iHoldOutput=FindHoldOutput(DCB.rpC);
          DCB.pC=TV[i].HoldOutputStr(DCB.ForView()?&DCB.dwRetFlags:NULL);
          return 1;

        case FTV_Input:      
          DCB.D = TV[i].m_Meas[0];
          return 1;

        case FTV_Output:
          if (DCB.rD)
            TV[i].m_X[0]=*DCB.rD;
          DCB.D = TV[i].m_X[0];
          return 1;

        case FTV_Error:
          DCB.D = TV[i].m_Error[0];
          return 1;

        case FTV_Initial:
          DCB.D = TV[i].m_Initial;
          return 1;

        case FTV_Cvgd:
          DCB.L = TV[i].m_iConvergedCnt>0;
          return 1;

        case FTV_Iters:
          DCB.L = TV[i].m_iConvergedCnt;
          return 1;

        case FTV_EPS_Abs:
          if (DCB.rD)
            TV[i].m_EPS_Abs=*DCB.rD;
          DCB.D = TV[i].m_EPS_Abs;
          if (!Valid(DCB.D))
            {
            DCB.D = EPS_Abs(gs_EqnCB, i);
            DCB.dwRetFlags=DDEF_SHOWASDEFAULT;
            }
          return 1;

        case FTV_EPS_Rel:
          if (DCB.rD)
            TV[i].m_EPS_Rel=*DCB.rD;
          DCB.D = TV[i].m_EPS_Rel;
          if (!Valid(DCB.D))
            {
            DCB.D = EPS_Rel(gs_EqnCB, i);
            DCB.dwRetFlags=DDEF_SHOWASDEFAULT;
            }
          return 1;

        case FTV_Method:
          if (DCB.rpC)
            TV[i].m_iTearMethod = FindTearConvergeMethod(DCB.rpC);
          DCB.pC=TV[i].TearMethodStr(DCB.ForView()?&DCB.dwRetFlags:NULL);
          return 1;

        case FTV_EPStrat:
          //if (DCB.rpC)
          //  TV[i].m_iTearMethod = FindTearConvergeMethod(DCB.rpC);
          DCB.pC=TV[i].TearMethodStr(DCB.ForView()?&DCB.dwRetFlags:NULL);
          return 1;

        default:
          DCB.D=dNAN;
          return 1;
        }
      break;
      }
    case xidTearInitWhen:
      {
      int iT=0;
      //if (NoProcLnkIOs()>=2)
      //  {
      if (DCB.rB)
        SetTearInitWhen(*DCB.rB);
      DCB.B=TearInitWhen();
      DCB.dwRetFlags=(RqdTearType()==TT_ManualTear) ? isParm : 0;
      //  }
      //else
      //  DCB.B=TIW_OnInit;
      return 1;
      }
    case xidTearInitHow:
      {
      int iT=0;
      //if (NoProcLnkIOs()>=2)
      //  {
      if (DCB.rB)
        SetTearInitHow(*DCB.rB);
      DCB.B = TearInitHow();
      DCB.dwRetFlags=(RqdTearType()==TT_ManualTear) ? isParm : 0;
      //  }
      //else
      //  DCB.B=TIH_ZeroNIters;
      return 1;
      }
    case xidTearInitEstUsage:
      {
      int iT=0;
      //if (NoProcLnkIOs()>=2)
      //  {
      if (DCB.rB)
        SetTearInitEstUsage(*DCB.rB);
      DCB.B=TearInitEstUsage();
      //DCB.dwRetFlags=(IOFlange(iT)->RqdTearType()==TT_ManualTear) ? isParm : 0;
      //  }
      //else
      //  DCB.B=TIEU_None;
      return 1;
      }
    case xidTearInitActive:
      {
      if (TearInitActive())
        {
        DCB.B=TearInitHow();
        //switch (F.TearInitHow())
        //  {
        //  case TIH_ZeroNIters:
        //    DCB.B=F.TearHoldCount()<F.TearHoldRqdCnt() ? TIH_Zeroing : TIH_ZeroNIters;
        //    break;
        //  case TIH_HoldNIters:
        //    DCB.B=F.TearHoldCount()<F.TearHoldRqdCnt() ? TIH_Holding : TIH_HoldNIters;
        //    break;
        //  case TIH_RampNIters:
        //    DCB.B=F.TearHoldCount()<F.TearHoldRqdCnt() ? TIH_Ramping : TIH_RampNIters;
        //    break;
        //  }

        //DCB.dwRetFlags=(IOFlange(iT)->RqdTearType()==TT_ManualTear) ? isParm : 0;
        }
      else DCB.B=TIH_Off;
      return 1;
      }
    case xidTearInitActivate:
      {
      if (DCB.rB)
        SetTearInitActive(*DCB.rB!=0, !DCB.ForFileSnpScn());
      //DCB.B=IOFlange(iT)->TearInitActive();
      //DCB.dwRetFlags=(IOFlange(iT)->RqdTearType()==TT_ManualTear) ? isParm : 0;
      return 1;
      }
    case xidTearCntRqd:
      {
      if (DCB.rB)
        m_pPosBlk->SetTearHoldRqdCnt(*DCB.rB);
      DCB.B=m_pPosBlk->TearHoldRqdCnt();
      DCB.dwRetFlags=(RqdTearType()>=TT_ManualTear) ? isParm : 0;
      return 1;
      }
    case xidTearCntAct:
      {
      if (DCB.rB)
        SetTearHoldCount(*DCB.rB);
      DCB.B=TearHoldCount();
      //DCB.dwRetFlags=(IOFlange(iT)->RqdTearType()>=TT_ManualTear) ? isParm : 0;
      return 1;
      }
    case xidTearQmMode:
      {
      //if (TearImageExists())
      //  {
      //  SpImage &I=*IOFlange(iT)->TearImage();
      //  if (DCB.rB)
      //    I.SetQMode(SpImgMode(*DCB.rB));
      //  DCB.B=I.QMode();
      //  }
      //else
      //  DCB.B=SPI_QModeNone;
      return 1;
      }
    case xidTearQmRqd:
      {
      //int iT=0;
      //if (NoProcLnkIOs()>1 && IOFlange(iT)->TearImageExists())
      //  {
      //  SpImage &I=*IOFlange(iT)->TearImage();
      //  if (DCB.rD)
      //    I.SetQmRqdTo(*DCB.rD, true, !DCB.ForFileSnpScn());
      //  DCB.D=I.QmRqd();
      //  }
      //else
      DCB.D=dNAN;
      return 1;
      }
    case xidTearQvRqd:
      {
      //int iT=0;
      //if (NoProcLnkIOs()>1 && IOFlange(iT)->TearImageExists())
      //  {
      //  SpImage &I=*IOFlange(iT)->TearImage();
      //  if (DCB.rD)
      //    I.SetQvRqdTo(*DCB.rD, true, !DCB.ForFileSnpScn());
      //  DCB.D=I.QvRqd();
      //  }
      //else
      DCB.D=dNAN;
      return 1;
      }
    case xidTearNQvRqd:
      {
      //int iT=0;
      //if (NoProcLnkIOs()>1 && IOFlange(iT)->TearImageExists())
      //  {
      //  SpImage &I=*IOFlange(iT)->TearImage();
      //  if (DCB.rD)
      //    I.SetNQvRqdTo(*DCB.rD, true, !DCB.ForFileSnpScn());
      //  DCB.D=I.NQvRqd();
      //  }
      //else
      DCB.D=dNAN;
      return 1;
      }
    case xidTearTRqd:
      {
      //int iT=0;
      //if (NoProcLnkIOs()>1 && IOFlange(iT)->TearImageExists())
      //  {
      //  if (DCB.rD)
      //    IOFlange(iT)->TearImage()->SetTRqdTo(*DCB.rD);
      //  DCB.D=IOFlange(iT)->TearImage()->TRqd();
      //  }
      //else
      DCB.D=dNAN;
      return 1;
      }
    case xidTearPRqd:
      {
      //int iT=0;
      //if (NoProcLnkIOs()>1 && IOFlange(iT)->TearImageExists())
      //  {
      //  if (DCB.rD)
      //    IOFlange(iT)->TearImage()->SetPRqdTo(*DCB.rD);
      //  DCB.D=IOFlange(iT)->TearImage()->PRqd();
      //  }
      //else
      DCB.D=dNAN;
      return 1;
      }
    }

  return 0;
  };


////--------------------------------------------------------------------------
//
//flag TearVarBlk::DataXchg(DataChangeBlk & DCB)
//  {
//  if (TaggedObject::DataXchg(DCB))
//    return 1;
//
//  switch (DCB.lHandle)
//    {
//    case xidNTearVars:
//      {
//      //      if (DCB.rL)
//      //        SetNVariables(*DCB.rL);
//      DCB.L=NVariables();
//      return 1;
//      }
//    case xidNTearHistory:
//      {
//      if (DCB.rL)
//        SetNHistory(*DCB.rL);
//      DCB.L=NHistory();
//      return 1;
//      }
//    }
//
//  return 0;
//  };
//
//--------------------------------------------------------------------------

flag TearVarBlk::ValidateData(ValidateDataBlk & VDB)
  {
#if dbgEqnSolve
  if (dbgTearSetupAll())
    {
    dbgpln("%s=======================", FullObjTag());
    dbgpln("GroupName   %s", sGroupTag());
    dbgpln("NVars       %i", m_nVariables);
    dbgpln("NHistory    %i", m_nHistory);
    dbgpln("DampTogether%i", m_iDampAsGroup);
    dbgpln("DampingRqd  %g", m_DampingRqd);
    dbgpln("TolStrategy %i", m_iEPSStrategy);
    dbgpln("EPS_Abs     %g", m_EPS_Abs);
    dbgpln("EPS_Rel     %g", m_EPS_Rel);

    for (int iVar=0; iVar<m_nVariables; iVar++)
      {
      if (Valid(TV[iVar].m_DampingRqd)) dbgp("%8.2f",  TV[iVar].m_DampingRqd); else dbgp("%8s",  "*");
      if (Valid(TV[iVar].m_EPS_Abs))    dbgp(" %8.2f", TV[iVar].m_EPS_Abs);    else dbgp(" %8s", "*");
      if (Valid(TV[iVar].m_EPS_Rel))    dbgp(" %8.2f", TV[iVar].m_EPS_Rel);    else dbgp(" %8s", "*");
      dbgp(" %8.2f", TV[iVar].m_DampFactor);
      dbgp(" %8.2f", TV[iVar].m_EstDampFactor);
      if (1)//fDoAdvance)
        {
        dbgp(" %8.2f", TV[iVar].m_X[0]);
        dbgp(" %8.2f", TV[iVar].m_Y[0]);
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

  if (m_nVariables!=Variables)
    {
    m_nVariables=Variables;
    m_nHistory=Max(m_nHistory, 4L);

    TV.SetSize(m_nVariables);
    for (int i=0; i<m_nVariables; i++)
      {
      TV[i].m_pBlk=this;

      TV[i].m_DampFactor=1.0;
      TV[i].m_EstDampFactor=0.0;
      TV[i].m_DampingRqd=dNAN;
      TV[i].m_EPS_Rel=dNAN;
      TV[i].m_EPS_Abs=dNAN;

      if (bUsedByBlk & (TVU_Inputs|TVU_Outputs))
        {
        TV[i].m_X.SetSize(m_nHistory);
        TV[i].m_Y.SetSize(m_nHistory);

        TV[i].m_X.SetAll(0.0);
        TV[i].m_Y.SetAll(0.0);
        TV[i].m_YRat=0.0;
        }
      else
        {
        TV[i].m_X.SetSize(0);
        TV[i].m_Y.SetSize(0);
        }

      if (bUsedByBlk & (TVU_Meas))
        {
        TV[i].m_Meas.SetSize(m_nHistory);
        TV[i].m_Error.SetSize(m_nHistory);
        TV[i].m_Meas.SetAll(0.0);
        TV[i].m_Error.SetAll(0.0);
        }
      else
        {
        TV[i].m_Meas.SetSize(0);
        TV[i].m_Error.SetSize(0);
        }

      }

    m_GrpDampFactor=0.0;
    }
  };

// ------------------------------------------------------------------------

void TearVarBlk::SetNHistory(int History)
  {
  if (m_nHistory!=History)
    {
    int Orig=m_nHistory;
    m_nHistory=Max(History, 4);
    for (int i=0; i<m_nVariables; i++)
      {

      if (bUsedByBlk & (TVU_Inputs|TVU_Outputs))//TearAdvanceReqd())
        {
        TV[i].m_X.SetSize(m_nHistory);
        TV[i].m_Y.SetSize(m_nHistory);

        if (Orig>0)
          {
          for (int j=Orig; j<m_nHistory; j++)
            {
            TV[i].m_X[j]=TV[i].m_X[j-1];
            TV[i].m_Y[j]=TV[i].m_Y[j-1];
            }
          }
        }
      else
        {
        TV[i].m_X.SetSize(0);
        TV[i].m_Y.SetSize(0);
        }

      if (bUsedByBlk & (TVU_Meas))//TearConvergeReqd())
        {
        TV[i].m_Meas.SetSize(m_nHistory);
        TV[i].m_Error.SetSize(m_nHistory);
        if (Orig>0)
          {
          for (int j=Orig; j<m_nHistory; j++)
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
    for (int iVar=0; iVar<m_nVariables; iVar++)
      TV[iVar].m_iConvergedCnt=0;
    m_lStageCnt=-1;
    m_lSeqConvergedCnt=0;
    }
  }

//--------------------------------------------------------------------------

void TearVarBlk::Connect(pTearPosBlk PosBlk)
  {
  m_pPosBlk=PosBlk;
  Initialise(true);//false);
  //for (int i=0; i<m_nVariables; i++)
  //  TV[i].m_Name=GetDisplayTag(TV[i].m_Tag, TV[i].m_Sym);

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
  m_pPosBlk=NULL;
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
  if (Valid(m_DampingRqd))
    R=Max(R, m_DampingRqd);
  R=Max(R, EqnCB.Cfg.m_DampingRqd);
  return Range(0.0, R, 1.0);
  };

//--------------------------------------------------------------------------

double TearVarBlk::EPS_Abs(EqnSlvCtrlBlk & EqnCB, int iVar)
  {
  double R=0.0;
  switch (m_iEPSStrategy)
    {
    case TVBTS_Priority:
      if (Valid(TV[iVar].m_EPS_Abs))
        R=TV[iVar].m_EPS_Abs;
      else if (pGroupInfo && Valid(pGroupInfo->TV[iVar].m_EPS_Abs))
        R=pGroupInfo->TV[iVar].m_EPS_Abs;
      else if (Valid(m_EPS_Abs))
        R=m_EPS_Abs;
      else
        R=EqnCB.Cfg.m_EPS_Abs;
      break;
    case TVBTS_Minimum:
      R=1.0;
      if (Valid(TV[iVar].m_EPS_Abs))
        R=Min(R, TV[iVar].m_EPS_Abs);
      if (pGroupInfo && Valid(pGroupInfo->TV[iVar].m_EPS_Abs))
        R=Min(R, pGroupInfo->TV[iVar].m_EPS_Abs);
      if (Valid(m_EPS_Abs))
        R=Min(R, m_EPS_Abs);
      R=Min(R, EqnCB.Cfg.m_EPS_Abs);
      break;
    case TVBTS_Maximum:
    default:
      R=0.0;
      if (Valid(TV[iVar].m_EPS_Abs))
        R=Max(R, TV[iVar].m_EPS_Abs);
      if (pGroupInfo && Valid(pGroupInfo->TV[iVar].m_EPS_Abs))
        R=Max(R, pGroupInfo->TV[iVar].m_EPS_Abs);
      if (Valid(m_EPS_Abs))
        R=Max(R, m_EPS_Abs);
      R=Max(R, EqnCB.Cfg.m_EPS_Abs);
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
      if (Valid(TV[iVar].m_EPS_Rel))
        R=TV[iVar].m_EPS_Rel;
      else if (pGroupInfo && Valid(pGroupInfo->TV[iVar].m_EPS_Rel))
        R=pGroupInfo->TV[iVar].m_EPS_Rel;
      else if (Valid(m_EPS_Rel))
        R=m_EPS_Rel;
      else
        R=EqnCB.Cfg.m_EPS_Rel;
      break;
    case TVBTS_Minimum:
      R=1.0;
      if (Valid(TV[iVar].m_EPS_Rel))
        R=Min(R, TV[iVar].m_EPS_Rel);
      if (pGroupInfo && Valid(pGroupInfo->TV[iVar].m_EPS_Rel))
        R=Min(R, pGroupInfo->TV[iVar].m_EPS_Rel);
      if (Valid(m_EPS_Rel))
        R=Min(R, m_EPS_Rel);
      R=Min(R, EqnCB.Cfg.m_EPS_Rel);
      break;
    case TVBTS_Maximum:
    default:
      R=0.0;
      if (Valid(TV[iVar].m_EPS_Rel))
        R=Max(R, TV[iVar].m_EPS_Rel);
      if (pGroupInfo && Valid(pGroupInfo->TV[iVar].m_EPS_Rel))
        R=Max(R, pGroupInfo->TV[iVar].m_EPS_Rel);
      if (Valid(m_EPS_Rel))
        R=Max(R, m_EPS_Rel);
      R=Max(R, EqnCB.Cfg.m_EPS_Rel);
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
      for (iVar=0; iVar<m_nVariables; iVar++)
        {
        TV[iVar].m_Error[0] = (TV[iVar].m_Y[0]-TV[iVar].m_X[0]);
        TV[iVar].m_Meas[0]  = TV[iVar].m_Y[0];
        }
      break;
    case 1: // Data Supplied
      break;
    }

  flag TearConverged=!TearInitActive();
  for (iVar=0; iVar<m_nVariables; iVar++)
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
        ErrTag=TVi.m_Sym;
        if (ErrTag.GetLength()==0)
          ErrTag=TVi.m_Tag;
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
    TVi.m_CurTol=RelTol;
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

  for (int iVar=0; iVar<m_nVariables; iVar++)
    {
    TearVar & V = TV[iVar];
    V.m_DampFactor=Max(Damping, DampingRqd(EqnCB, iVar));
    double d=m_bHoldDamping ? 0 : V.m_DampFactor;
    if (!V.HoldOutput())
      V.m_X[0]=(d)*V.m_X[1]+(1.0-d)*V.m_Y[0];

#if dbgEqnSolve
    double Err=0.0;
    if (dbgTearCnvgLoop())
      if (dbgShowAll() || /*lCnt==0 ||*/ !ConvergedDV(V.m_Y[0]-V.m_X[1], V.m_Y[0], EqnCB.Cfg.m_EPS_Abs, EqnCB.Cfg.m_EPS_Rel, Err))
        {
        Strng Tag;
        Tag.Set("%s.%s", FullObjTag(), SymOrTag(iVar));
        dbgpln("%s %3i %8.3f%% DirctSubs %6.2f%% I:%14.6g  O:%14.6g >> %14.6g : %s",
          iVar==0 ? "---" : "   ",
          iVar, Range(-999.999, 100.0*(V.m_X[0]-V.m_X[1])/NZ(V.m_X[1]), 999.999),
          V.m_DampFactor*100.0,V.m_Y[0], V.m_X[1], V.m_X[0], Tag());
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
  m_GrpDampFactor=0.0;
  bool DampGrp=((m_iDampAsGroup==DAG_Default) ? EqnCB.Cfg.m_iDampAsGroup : m_iDampAsGroup)==DAG_On;
  for (int iVar=0; iVar<m_nVariables; iVar++)
    {
    TearVar & V = TV[iVar];
#ifndef _RELEASE
    if (DoTrack && _stricmp(V.m_Tag(), TrackTag())==0)
      {
      Strng ss;
      if (!DoneHeader)
        {
        DoneHeader=true;
        ss.Set("TrackTag:%s   DampFctGrowth:%18.10g   DampFctDecay:%18.10g", TrackTag(), EqnCB.Cfg.m_DampFctGrowth, EqnCB.Cfg.m_DampFctDecay);
        LogNote(TrackTag(), 0, ss());
        dbgpln(ss());
        ss="Iter,B/A,Y2,Y1,Y0,X2,X1,X0,YRat,EstDampFactor,DampFactor";
        LogNote(TrackTag(), 0, ss());
        dbgpln(ss());
        }
      ss.Set("%d,Before,%g,%g,%g,%g,%g,%g,%g,%g,%g", m_lStageCnt, V.m_Y[2], V.m_Y[1], V.m_Y[0],
        V.m_X[2], V.m_X[1], V.m_X[0], V.m_YRat, V.m_EstDampFactor, V.m_DampFactor);
      LogNote(TrackTag(), 0, ss());
      dbgpln(ss());
      }
#endif
    if (TstNZ(V.m_Y[1]-V.m_Y[2]))
      {
      V.m_YRat=(V.m_Y[0]-V.m_Y[1])/NZ(V.m_Y[1]-V.m_Y[2]);
      if (V.m_YRat<-0.3) // Oscillatory Damp More
        V.m_EstDampFactor=Min(0.99, V.m_EstDampFactor+(1.0-V.m_EstDampFactor)*EqnCB.Cfg.m_DampFctGrowth);
      else if (V.m_YRat>0.3)// Damp Less
        V.m_EstDampFactor=V.m_EstDampFactor/(1.0+EqnCB.Cfg.m_DampFctDecay);
      else // Dont Change Damping
        {};
      }
    else
      {
      V.m_YRat=0.0;
      V.m_EstDampFactor=V.m_EstDampFactor/(1.0+EqnCB.Cfg.m_DampFctDecay);
      }

    double DFMin=Max(DampingRqd(EqnCB, iVar), Damping);
    V.m_EstDampFactor=Range(DFMin, V.m_EstDampFactor, 1.0);

    if (DampGrp && (fabs(V.m_Y[0])>1.0e-10))
      m_GrpDampFactor=Max(m_GrpDampFactor, V.m_EstDampFactor);
    }

  for (iVar=0; iVar<m_nVariables; iVar++)
    {
    TearVar & V = TV[iVar];
    if (DampGrp)
      V.m_DampFactor=m_GrpDampFactor;
    else
      V.m_DampFactor=V.m_EstDampFactor;
    V.m_DampFactor=Max(Damping, V.m_DampFactor);

    double d=m_bHoldDamping ? 0 : V.m_DampFactor;
    //if (!V.m_bHoldOutput)
    if (!V.HoldOutput())
      V.m_X[0]=(d)*V.m_X[1] + (1.0-d)*V.m_Y[0];

#if dbgEqnSolve
    double Err=0.0;
    if (dbgTearCnvgLoop())
      if (dbgShowAll() || !ConvergedDV(V.m_Y[0]-V.m_X[1], V.m_Y[0], EqnCB.Cfg.m_EPS_Abs, EqnCB.Cfg.m_EPS_Rel, Err))
        {
        Strng Tag;
        Tag.Set("%s.%s", FullObjTag(), SymOrTag(iVar));
        dbgpln("%s %3i %8.3f%% AdptDSubs e:%6.2f%% d:%6.2f%% dI2:%18.10g I:%18.10g  O:%18.10g >> %18.10g : %s",
          iVar==0 ? "---" : "   ",
          iVar, Range(-999.999, 100.0*(V.m_X[0]-V.m_X[1])/NZ(V.m_X[1]), 999.999),
          V.m_EstDampFactor*100.0,V.m_DampFactor*100.0, V.m_YRat,V.m_Y[0],
          V.m_X[1], V.m_X[0], Tag());
        }
#endif

#ifndef _RELEASE
      if (DoTrack && _stricmp(V.m_Tag(), TrackTag())==0)
        {
        Strng ss;
        ss.Set("%d,After,%g,%g,%g,%g,%g,%g,%g,%g,%g", m_lStageCnt, V.m_Y[2], V.m_Y[1], V.m_Y[0],
          V.m_X[2], V.m_X[1], V.m_X[0], V.m_YRat, V.m_EstDampFactor, V.m_DampFactor);
        LogNote(TrackTag(), 0, ss());
        dbgpln(ss());
        /*Strng Tag;
        Tag.Set("%s.%s", FullObjTag(), SymOrTag(iVar));
        Strng ss;
        ss.Set("%4i %8.3f%% AdptDSubs e:%6.2f%% d:%5.2f%% dI2:%18.10g I:%18.10g  O:%18.10g >> %18.10g : %s",
        lCnt, Range(-999.999, 100.0*(V.m_X[0]-V.m_X[1])/NZ(V.m_X[1]), 999.999),
        V.m_EstDampFactor*100.0, V.m_DampFactor*100.0, V.dYRat, V.m_Y[0],
        V.m_X[1], V.m_X[0], Tag());
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

  for (int iVar=0; iVar<m_nVariables; iVar++)
    {
    TearVar & V = TV[iVar];
    double S=(V.m_X[1]-V.m_X[2])/NZ(V.m_X[2]-V.m_X[3]);
    //double NewQ=Range(Cfg.m_WA_Bound, S/NZ(S-1), 0.9);
    //if (Q[iVar] > 0.0) // Interpolation
    //  Q[iVar]=Max(0.0, Max(NewQ, Q[iVar])); // Keep As Interpolation
    //else
    //  Q[iVar]=NewQ;
    double Q=Range(Cfg.m_WA_Bound, S/NZ(S-1), 0.0);

    V.m_DampFactor=0.0;
    if (m_bHoldDamping)
      Q=0;
    if (!V.HoldOutput())
      V.m_X[0]=Q*V.m_X[1] + (1.0-Q)*V.m_Y[0];

#if dbgEqnSolve
    double Err=0.0;
    if (dbgTearCnvgLoop())
      {
      if (dbgShowAll() || m_lStageCnt==0 || !ConvergedDV(V.m_Y[0]-V.m_X[1], V.m_Y[0], EqnCB.Cfg.m_EPS_Abs, EqnCB.Cfg.m_EPS_Rel, Err))
        {
        Strng Tag;
        Tag.Set("%s.%s", FullObjTag(), SymOrTag(iVar));
        dbgpln("%s %3i %8.3f%% Wegstein  %6.2f%% I:%14.6g  O:%14.6g >> %14.6g  S:%14.6g : %s",
          iVar==0 ? "---" : "   ",
          iVar, Range(-999.999, 100.0*(V.m_X[0]-V.m_X[1])/NZ(V.m_X[1]), 999.999),(1.0-Q)*100.0,V.m_Y[0], V.m_X[1], V.m_X[0], S, Tag());
        //if (fabs(V.m_X[0]-V.m_X[1]) > fabs(V.m_X[0])*0.01)
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
  for (int iVar=0; iVar<m_nVariables; iVar++)
    {
    CDArray &Y=TV[iVar].m_Y;
    double T=Y[m_nHistory-1];
    for (int i=m_nHistory-1; i>0; i--)
      Y[i]=Y[i-1];
    Y[0]=T;
    }
  };

//--------------------------------------------------------------------------

void TearVarBlk::RotateErrors()
  {
  // Rotate Errors
  for (int iVar=0; iVar<m_nVariables; iVar++)
    {
    CDArray &Error=TV[iVar].m_Error;
    double T=Error[m_nHistory-1];
    for (int i=m_nHistory-1; i>0; i--)
      Error[i]=Error[i-1];
    Error[0]=T;
    CDArray &Meas=TV[iVar].m_Meas;
    T=Meas[m_nHistory-1];
    for (i=m_nHistory-1; i>0; i--)
      Meas[i]=Meas[i-1];
    Meas[0]=T;
    }
  };

//--------------------------------------------------------------------------

void TearVarBlk::RotateOutputs()
  {
  // Rotate Outputs
  for (int iVar=0; iVar<m_nVariables; iVar++)
    if (!TV[iVar].HoldOutput())
      //if (!TV[iVar].m_bHoldOutput)
      {
      CDArray &X=TV[iVar].m_X;
      double T=X[m_nHistory-1];
      for (int i=m_nHistory-1; i>0; i--)
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
    for (int iVar=0; iVar<m_nVariables; iVar++)
      {
      CDArray &X=TV[iVar].m_X;
      CDArray &Y=TV[iVar].m_Y;
      if (!Valid(X[1])) X[1]=X[0];
      if (!Valid(X[2])) X[2]=X[0];
      if (!Valid(X[3])) X[3]=X[0];
      if (!Valid(Y[1])) Y[1]=Y[0];
      if (!Valid(Y[2])) Y[2]=Y[0];
      if (!Valid(Y[3])) Y[3]=Y[0];
      }
    }
  byte Meth=TearConvergeMethod(Cfg.m_iConvergeMeth, m_iTearMethod);
  //if (Meth==TCM_Default)
  //  Meth=Cfg.iConvergeMeth;

  switch (Meth)
    {
    case TCM_Wegstein:
      if ((m_lStageCnt < Cfg.m_iWA_Delay) || (m_lStageCnt<2))
        AdvDirect(EqnCB, 0.0);
      else
        AdvWegstein(EqnCB);
      break;
    case TCM_AdaptSubs:
      AdvAdaptDirect(EqnCB, EqnCB.Cfg.m_DampingRqd);
      break;
    case TCM_DirectSubs:
    default:
      AdvDirect(EqnCB, EqnCB.Cfg.m_DampingRqd);
      break;
    }
  m_bHoldDamping=false;
  }

//===========================================================================
//
//
//
//===========================================================================

TearVarBlkList TearVarBlk::sm_List;
TearPosBlkList TearPosBlk::sm_List;

//==========================================================================
//
//
//
//==========================================================================

#if WithTearVarBlkEdit

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
    iNameWidth=Max(iNameWidth, (int)(strlen(GetDisplayTag(TV[i].m_Tag, TV[i].m_Sym))+1));

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
      SetDesc(L, GetDisplayTag(TV[i].m_Tag, TV[i].m_Sym),    -1, iNameWidth, 0, "");
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

      SetDesc(L, TV[i].m_pMeasCnv->Text(),  -1, 8,  0, " ");
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
      SetDesc(L, GetDisplayTag(TV[i].m_Tag, TV[i].m_Sym),    -1, iNameWidth, 0, "");
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

      SetDesc(L, TV[i].m_pMeasCnv->Text(),  -1, 8,  0, "");
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
    const flag IsAdaptSubs = (rTVB.m_iTearMethod==TCM_AdaptSubs || (rTVB.m_iTearMethod==TCM_Default  /* && PBEqnCB().Cfg.iConvergeMeth==TCM_AdaptSubs*/)); //todo: need access to global default iConvergeMeth !!
    switch (EI.FieldId)
      {
      case Id_DampAsGroup:
        Str=DampAsGroupStrings[rTVB.m_iDampAsGroup];
        EI.Fld->fEditable=(IsAdaptSubs);
        break;
      case Id_BlkDamp:
        DampFmt.FormatFloat(DampCnv.Human(rTVB.m_DampingRqd), Str);
        EI.Fld->fEditable=(IsAdaptSubs);
        break;
      case Id_BlkEPSStrat:
        Str=EPSStrategyStrings[rTVB.m_iEPSStrategy];
        break;
      case Id_BlkEPSRel:
        EPSFmt.FormatFloat(EPSCnv.Human(rTVB.m_EPS_Rel), Str);
        break;
      case Id_BlkEPSAbs:
        EPSFmt.FormatFloat(EPSCnv.Human(rTVB.m_EPS_Abs), Str);
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
        //EI.Fld->fEditable=(rTVB.m_iTearMethod==TCM_AdaptSubs || (rTVB.m_iTearMethod==TCM_Default   && PBEqnCB().Cfg.iConvergeMeth==TCM_AdaptSubs));  //todo: need access to global default iConvergeMeth
        EI.Fld->fEditable=(rTVB.m_iTearMethod==TCM_AdaptSubs || (rTVB.m_iTearMethod==TCM_Default  ));// && PBEqnCB().Cfg.iConvergeMeth==TCM_AdaptSubs));
        break;
      case Id_EPSRel:
        EPSFmt.FormatFloat(EPSCnv.Human(TVi.m_EPS_Rel), Str);
        break;
      case Id_EPSAbs:
        EPSFmt.FormatFloat(EPSCnv.Human(TVi.m_EPS_Abs), Str);
        break;

      case Id_GlblDamp:
        if (pGroupInfo)
          DampFmt.FormatFloat(DampCnv.Human(pGroupInfo->TV[i].m_DampingRqd), Str);
        else
          Str="?";
        //EI.Fld->fEditable=(rTVB.m_iTearMethod==TCM_AdaptSubs || PBEqnCB().Cfg.iConvergeMeth==TCM_AdaptSubs);  //todo: need access to global default iConvergeMeth
        EI.Fld->fEditable=(rTVB.m_iTearMethod==TCM_AdaptSubs || rTVB.m_iTearMethod==TCM_Default  );
        break;
      case Id_GlblEPSRel:
        if (pGroupInfo)
          DampFmt.FormatFloat(DampCnv.Human(pGroupInfo->TV[i].m_EPS_Rel), Str);
        else
          Str="?";
        break;
      case Id_GlblEPSAbs:
        if (pGroupInfo)
          DampFmt.FormatFloat(DampCnv.Human(pGroupInfo->TV[i].m_EPS_Abs), Str);
        else
          Str="?";
        break;

      case Id_Inputs:
        TVi.m_pMeasFmt->FormatFloat(TVi.m_pMeasCnv->Human(TVi.m_Y[0]), Str);
        EI.Fld->fEditable=False;
        break;
      case Id_Outputs:
        TVi.m_pMeasFmt->FormatFloat(TVi.m_pMeasCnv->Human(TVi.m_X[0]), Str);
        EI.Fld->fEditable=False;
        break;
      case Id_CurTol:
        EPSFmt.FormatFloat(EPSCnv.Human(TVi.m_CurTol), Str);
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
        TVi.m_pMeasFmt->FormatFloat(TVi.m_pMeasCnv->Human(TVi.m_Meas[0]), Str);
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
        rTVB.m_DampingRqd = DampCnv.Normal(SafeAtoF(Str));
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
        rTVB.m_EPS_Rel = DampCnv.Normal(SafeAtoF(Str));
        break;
      case Id_BlkEPSAbs:
        rTVB.m_EPS_Abs = DampCnv.Normal(SafeAtoF(Str));
        break;
      case Id_TearMeth:
        {
        for (int i=0; TearMethodStrings[i]; i++)
          if (Str.XStrICmp(TearMethodStrings[i])==0)
            break;
        rTVB.m_iTearMethod=(TearMethodStrings[i] ? i : TCM_Default  );
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
        TVi.m_EPS_Rel=EPSCnv.Normal(SafeAtoF(Str));
        break;
      case Id_EPSAbs:
        TVi.m_EPS_Abs=EPSCnv.Normal(SafeAtoF(Str));
        break;

      case Id_GlblDamp:
        if (pGroupInfo)
          pGroupInfo->TV[i].m_DampingRqd=DampCnv.Normal(SafeAtoF(Str));
        break;
      case Id_GlblEPSRel:
        if (pGroupInfo)
          pGroupInfo->TV[i].m_EPS_Rel=EPSCnv.Normal(SafeAtoF(Str));
        break;
      case Id_GlblEPSAbs:
        if (pGroupInfo)
          pGroupInfo->TV[i].m_EPS_Abs=EPSCnv.Normal(SafeAtoF(Str));
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
            WrkIB.Set(EI.Fld->Tag, TVi.m_pMeasCnv, TVi.m_pMeasFmt);
            }
          else if (IsMeasValue)
            {
            int i=EI.BlkRowNo-EI.Index;
            TearVar &TVi = rTVB.TV[i];
            WrkIB.Set(EI.Fld->Tag, TVi.m_pMeasCnv, TVi.m_pMeasFmt);
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
  m_bMassSmooth       = 0;
  m_bForceConverged   = 0;
  m_bIdleWhenDone     = 0;
  m_iMaxIters         = 1000;
  m_iRqdCnvrgdIters   = 3;

  m_iConvergeMeth     = TCM_AdaptSubs;//TCM_Wegstein;
  m_iHoldOutput       = THO_Off;

  m_iEPSStrategy      = TVBTS_Maximum;
  m_EPS_Rel           = 1.0e-5;
  m_EPS_Abs           = 1.0e-6;

  m_iDampAsGroup      = DAG_On;//DAG_Off; maybe change default to off?
  m_DampingRqd        = 0.0;
  m_DampFctGrowth     = 0.25;
  m_DampFctDecay      = 0.05;

  m_iWA_Delay         = 5;
  m_WA_Bound          = -3.0;
  m_WA_Clamping       = 0.5;
  }

//--------------------------------------------------------------------------

void EqnSlvCfgBlk::InitDynamic()
  {
  m_bMassSmooth       = 0;
  m_bForceConverged   = 1;
  m_bIdleWhenDone     = 0;
  m_iMaxIters         = 26;
  m_iRqdCnvrgdIters   = 3;

  m_iConvergeMeth     = TCM_DirectSubs;
  m_iHoldOutput       = THO_Off;

  m_iEPSStrategy      = TVBTS_Maximum;
  m_EPS_Rel           = 1.0e-3;
  m_EPS_Abs           = 1.0e-4;

  m_iDampAsGroup      = DAG_On;//DAG_Off; maybe change default to off?
  m_DampingRqd        = 0.0;
  m_DampFctGrowth     = 0.25;
  m_DampFctDecay      = 0.05;

  m_iWA_Delay         = 2;
  m_WA_Bound          = -5.0;
  m_WA_Clamping       = 0.5;
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
  TearVarBlkIter TCBs(TearVarBlk::sm_List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    if (pT->Active())
      pT->BeginStep(EqnCB);
  };

//--------------------------------------------------------------------------

void EqnSolverBlk::EndStep(EqnSlvCtrlBlk & EqnCB)
  {
  EqnCB.EndStep();
  TearVarBlkIter TCBs(TearVarBlk::sm_List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    if (pT->Active())
      pT->EndStep(EqnCB);
  };

// ------------------------------------------------------------------------

void EqnSolverBlk::SetTearsInUse(int InUse)
  {
  TearVarBlkIter TCBs(TearVarBlk::sm_List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    pT->SetInUse(InUse);
  }

// ------------------------------------------------------------------------

void EqnSolverBlk::FindAllTearsInUse()
  {
  SetTearsInUse(False);
  TearPosBlkIter TPBs(TearPosBlk::sm_List);
  for (TearPosBlk* pP=TPBs.First(); pP; pP=TPBs.Next())
    {
    TearVarBlk* pVarBlk=TearVarBlk::Find(pP->TearGetTag());
    if (pVarBlk)
      pVarBlk->SetInUse(True);
    }
  };

// ------------------------------------------------------------------------

void EqnSolverBlk::RemoveUnusedTears()
  {
  FindAllTearsInUse();
  TearVarBlkIter TCBs(TearVarBlk::sm_List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    if (!pT->InUse() && !pT->fGroupBlk)
      TearVarBlk::Delete(pT);
  };

// ------------------------------------------------------------------------

void EqnSolverBlk::RemoveInActiveTears()
  {
  FindAllTearsInUse();
  TearVarBlkIter TCBs(TearVarBlk::sm_List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    //if (!pT->InUse() && !pT->fGroupBlk)
    if (!pT->Active() && !pT->fGroupBlk)
      TearVarBlk::Delete(pT);
  };

// ------------------------------------------------------------------------

void EqnSolverBlk::RemoveAllTears()
  {
  TearVarBlkIter TCBs(TearVarBlk::sm_List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    TearVarBlk::Delete(pT);
  };

//--------------------------------------------------------------------------

void EqnSolverBlk::ActivateAllReqdTears()
  {
  TearPosBlkIter TPBs(TearPosBlk::sm_List);
  for (TearPosBlk* pT=TPBs.First(); pT; pT=TPBs.Next())
    if (pT->TearVarType() != TVT_None)
      pT->ActivateTear();
  }

//--------------------------------------------------------------------------

void EqnSolverBlk::DeActivateAllTears()
  {
  TearPosBlkIter TPBs(TearPosBlk::sm_List);
  for (TearPosBlk* pT=TPBs.First(); pT; pT=TPBs.Next())
    pT->DeActivateTear();
  }

//--------------------------------------------------------------------------

void EqnSolverBlk::ConnectAllTears(flag CreateIfReqd)
  {
  TearPosBlkIter TPBs(TearPosBlk::sm_List);
  for (TearPosBlk* pT=TPBs.First(); pT; pT=TPBs.Next())
    pT->ConnectTear(CreateIfReqd);
  }

//--------------------------------------------------------------------------

void EqnSolverBlk::DisConnectAllTears()
  {
  TearPosBlkIter TPBs(TearPosBlk::sm_List);
  for (TearPosBlk* pT=TPBs.First(); pT; pT=TPBs.Next())
    pT->DisConnectTear();
  }

//--------------------------------------------------------------------------

void EqnSolverBlk::InitialiseActiveTears(EqnSlvCtrlBlk & EqnCB)
  {
  EqnCB.SetNUnknowns(0);
  EqnCB.Cfg.m_iRqdCnvrgdIters=Range((short)1, EqnCB.Cfg.m_iRqdCnvrgdIters, (short)9999);

  TearVarBlkIter TCBs(TearVarBlk::sm_List);
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
  TearVarBlkIter TCBs(TearVarBlk::sm_List);
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
  TearVarBlkIter TCBs(TearVarBlk::sm_List);
  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    if (pT->Active() && pT->TearAdvanceReqd())
      pT->TearInputs2Outputs();
  }

//--------------------------------------------------------------------------

flag EqnSolverBlk::TestTearConvergence(long ConvergeLoopCnt, EqnSlvCtrlBlk & EqnCB, Strng_List &BadTearTags, Strng_List & BadTearInfo)
  {
  TearVarBlkIter TCBs(TearVarBlk::sm_List);
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
    TearVarBlkIter TCBs(TearVarBlk::sm_List);
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


int   TearVar::TearMethod() { return ::TearConvergeMethod(gs_EqnCB.Cfg.m_iConvergeMeth, m_pBlk->m_iTearMethod, m_iTearMethod); }
LPSTR TearVar::TearMethodStr(DDEF_Flags * Flags) { return ::TearConvergeMethodStr(gs_EqnCB.Cfg.m_iConvergeMeth, m_pBlk->m_iTearMethod, m_iTearMethod, Flags); }

bool  TearVar::HoldOutput() { return ::HoldOutput(gs_EqnCB.Cfg.m_iHoldOutput, m_pBlk->m_iHoldOutput, m_iHoldOutput)==THO_On; }
LPSTR TearVar::HoldOutputStr(DDEF_Flags * Flags) { return ::HoldOutputStr(gs_EqnCB.Cfg.m_iHoldOutput, m_pBlk->m_iHoldOutput, m_iHoldOutput, Flags); }


//===========================================================================
//
//
//
//===========================================================================
