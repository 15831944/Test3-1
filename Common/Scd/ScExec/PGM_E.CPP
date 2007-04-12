//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/*#M:PGM engine. PGM code parser and executer.*/

#include "stdafx.h"
#include <math.h>
#include <stdlib.h>

#define  __PGM_E_CPP
#include "sc_defs.h"
#include "errorlog.h"
#include "pgm_e.h"
#include "pgm_i.h"
#include "gpfuncs.h"
#include "tknpars.h"
#include "dbgmngr.h"
#include "executiv.h"
#include "tagdb.h"
#include "pgm_elec.h"
//#include "optoff.h"

#include <fstream>

#define WITHVARMAP 01

#if WITHDEBUGNEW
#ifdef new
#undef new
#endif
#endif

#ifdef _RELEASE
//#define dbgGenCon      1
#define dbgGenCon      (WITHDEBUG)
#else
#define dbgGenCon      (WITHDEBUG)
#endif

#if dbgGenCon
static CDbgMngr dbgGetSets    ("PGM", "GetSets");
static CDbgMngr dbgExec       ("PGM", "Exec");
static CDbgMngr dbgLoad       ("PGM", "Load");
static CDbgMngr dbgVars       ("PGM", "Vars");
static CDbgMngr dbgTimes      ("PGM", "Times");
static CDbgMngr dbgXRefs      ("PGM", "XRefs");
static CDbgMngr dbgXRefsDbgBrk("PGM", "XRefsDbgBrk");
flag dbgMemIsFirstBlk = 0;
#endif

//#pragma warning (disable : 4756) // Const Arith overflow Warning

//===========================================================================

IMPLEMENT_DYNAMIC(GCException, CException);
//IMPLEMENT_SPARES(CGCXRefItem, 2000)

_FWDDEF(GCEOFIns)
_FWDDEF(GCEOFDef)

_FWDDEF(GCNxtBlkIns)
_FWDDEF(GCNxtBlkDef)

_FWDDEF(GCIfIns)
_FWDDEF(GCIfDef)
_FWDDEF(GCElseifIns)
_FWDDEF(GCElseifDef)
_FWDDEF(GCElseIns)
_FWDDEF(GCElseDef)
_FWDDEF(GCEndifIns)
_FWDDEF(GCEndifDef)
_FWDDEF(GCIifIns)
_FWDDEF(GCIifDef)

_FWDDEF(GCWhileIns)
_FWDDEF(GCWhileDef)
_FWDDEF(GCEndWhileIns)
_FWDDEF(GCEndWhileDef)

_FWDDEF(GCVarRefIns)
_FWDDEF(GCVarRefDef)
_FWDDEF(GCVarTagRefIns)
_FWDDEF(GCVarTagRefDef)
_FWDDEF(GCVarClassRefIns)
_FWDDEF(GCVarClassRefDef)

_FWDDEF(GCConstIns)
_FWDDEF(GCConstDef)
_FWDDEF(GCConstStrIns)
_FWDDEF(GCConstStrDef)

_FWDDEF(GCConstVarIns)
_FWDDEF(GCConstVarDef)

_FWDDEF(GCEqualIns)
_FWDDEF(GCEqualDef)
_FWDDEF(GCEqualTagIns)
_FWDDEF(GCEqualTagDef)
_FWDDEF(GCEqualClassRefIns)
_FWDDEF(GCEqualClassRefDef)

_FWDDEF(GCTagIns)
_FWDDEF(GCTagDef)
_FWDDEF(GCStrTagIns)
_FWDDEF(GCStrTagDef)

_FWDDEF(GCCommaDef)

_FWDDEF(GCCBraceDef)

_FWDDEF(GCJmpIns)
_FWDDEF(GCJmpDef)

_FWDDEF(GCCallFntIns)
_FWDDEF(GCCallFntDef)

_FWDDEF(GCRefArrayIns)
_FWDDEF(GCRefArrayDef)

_FWDDEF(GCPopIns)
_FWDDEF(GCPopDef)

_FWDDEF(GCSetTagIns)
_FWDDEF(GCSetTagDef)
_FWDDEF(GCGetTagIns)
_FWDDEF(GCGetTagDef)

_FWDDEF(GCLblIns)
_FWDDEF(GCLblDef)

//---------------------------------------------------------------------------
//======================= Data Type Definition =============================
//---------------------------------------------------------------------------
long GCTagVar::m_nXRefNumber=0;

GCTagVar::GCTagVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags) : GCVar(pFirstVar, pVarMap, pName, flags)
  {
//static
  //m_bDynTag=0;
  m_bTempDynTag=0;
  m_bOnlyAllowGet=0;
  m_bAllowSetAtInit=0;
  m_bStrTag=0;
  m_iCnvIndex=0;
  m_pXRef=NULL;
  m_pFldName=NULL;
  m_pRqdCnvTxt=NULL;
  m_iXRefNumber=m_nXRefNumber++;
  //dbgpln("GCTagVar::GCTagVar %i", m_iXRefNumber);
  };

GCTagVar::~GCTagVar()
  {
  //dbgpln("GCTagVar::~GCTagVar %i", m_iXRefNumber);
  delete m_pXRef; // remove if neccessary
  delete []m_pFldName;
  delete []m_pRqdCnvTxt;
  };

//---------------------------------------------------------------------------
//======================= Data Type Definition =============================
//---------------------------------------------------------------------------

GCDoubleDef GCDouble;
GCLongDef GCLong;
GCByteDef GCByte;
GCBitDef GCBit;
GCStrDef GCStr;

//---------------------------------------------------------------------------
//======================== 'comparison instructions' ========================
//---------------------------------------------------------------------------

CMP_CODE(CmpEqual,"==",==,20)
CMP_CODE(CmpNotEqual,"<>",!=,20)
CMP_CODE(CmpGreat,">",>,22)
CMP_CODE(CmpGreatEqual,">=",>=,22)
CMP_CODE(CmpLess,"<",<,22)
CMP_CODE(CmpLessEqual,"<=",<=,22)

//START_CODE(CmpGreat)
//MID_CODE(CmpGreat,">",DefExp,22)
//  virtual pGCIns   Construct(GCInsMngr & IB)
//    { return new(IB) GCCmpGreatIns; };
//END_CODE(CmpGreat)
//
//void GCCmpGreatIns::Exec(GCInsMngr & IB)
//  {
//  --IB.m_iCalcPos;
//  if (IB.m_dCalcStack[IB.m_iCalcPos] > IB.m_dCalcStack[IB.m_iCalcPos+1])
//    IB.m_dCalcStack[IB.m_iCalcPos] = dGCtrue;
//  else
//    IB.m_dCalcStack[IB.m_iCalcPos] = dGCfalse;
//  Advance(IB);
//  }

//---------------------------------------------------------------------------
//======================== 'Not' Your Average Operator ==============================
//---------------------------------------------------------------------------

START_CODE(Bnot)
MID_CODE(Bnot,"BNot",DefExp | DefOperator,32)
virtual pGCIns   Construct(GCInsMngr & IB)
  {
  return new(IB) GCBnotIns;
  };
END_CODE(Bnot)

//---------------------------------------------------------------------------

void GCBnotIns::Exec(GCInsMngr & IB)
  {
  IB.m_dCalcStack[IB.m_iCalcPos] = (double)~(GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos]));
  Advance(IB);
  }

//===========================================================================

START_CODE(Not)
MID_CODE(Not,"Not",DefExp | DefOperator,32)
virtual pGCIns   Construct(GCInsMngr & IB)
  {
  return new(IB) GCNotIns;
  };
END_CODE(Not)

//---------------------------------------------------------------------------

void GCNotIns::Exec(GCInsMngr & IB)
  {
  IB.m_dCalcStack[IB.m_iCalcPos] = !(IB.m_dCalcStack[IB.m_iCalcPos]);
  Advance(IB);
  }

//===========================================================================

START_CODE(Jmp);
  pGCIns          m_pJmpPos;          //points to instruction to jump to
  virtual pchar    GetDesc(pchar Buff);
MID_CODE(Jmp,"#Jmp#",0,0);
  virtual pGCIns   Construct(GCInsMngr & IB)
    { return new(IB) GCJmpIns; };
END_CODE(Jmp);

//---------------------------------------------------------------------------

void GCJmpIns::Exec(GCInsMngr & IB)
  {
  IB.m_pIPtr = m_pJmpPos;
  if (IB.m_bDoDebug)
    IB.m_pcIPtr -= sizeof(pCPGMDbg);
  }

//---------------------------------------------------------------------------

pchar GCJmpIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  char Buff2[256];
  sprintf(Buff, "%s-->(%s)", GCIns::GetDesc(Buff1), m_pJmpPos->GetDesc(Buff2));
  return Buff;
  }

//===========================================================================

START_CODE(Endif);
MID_CODE(Endif,"Endif",DefLHS,0);
virtual pGCIns   Construct(GCInsMngr & IB)
  {
  IB.m_pH->m_bBreakLine = 1;
  return new(IB) GCEndifIns;
  }
END_CODE(Endif);

//---------------------------------------------------------------------------

void GCEndifIns::Exec(GCInsMngr & IB)
  { Advance(IB); }

//===========================================================================

START_CODE(If);
pGCIns m_pFalseJmpPos;          //position to jump to if the expression is False
virtual pchar    GetDesc(pchar Buff);
MID_CODE(If,"If",DefLHS,0);
virtual pGCIns   Construct(GCInsMngr & IB);
END_CODE(If);

//---------------------------------------------------------------------------

pGCIns GCIfDef::Construct(GCInsMngr & IB)
  {
  IB.DoConstructExprssn();
  IB.m_pH->m_bBreakLine = 1;
  pGCIns pIns = new(IB) GCIfIns;
  IB.Build(&GCEndif, pIns);
  return pIns;
  }

//---------------------------------------------------------------------------

void GCIfIns::Exec(GCInsMngr & IB)
  {
  --IB.m_iCalcPos;
  if (IB.m_dCalcStack[IB.m_iCalcPos+1] == dGCfalse)
    {
    IB.m_pIPtr = m_pFalseJmpPos;
    if (IB.m_bDoDebug)
      IB.m_pcIPtr -= sizeof(pCPGMDbg);
    }
  else
    Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCIfIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  char Buff2[256];
  sprintf(Buff, "%s-->(%s)", GCIns::GetDesc(Buff1), m_pFalseJmpPos->GetDesc(Buff2));
  return Buff;
  }

//===========================================================================

class GCElseifDef : public GCDef
  {
  public:
  GCElseifDef() : GCDef("ElseIf")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

//---------------------------------------------------------------------------

pGCIns GCElseifDef::Construct(GCInsMngr & IB)
  {
  IB.DoConstructExprssn();
  return new(IB) GCIfIns;
  }

GCElseifDef GCElseif;

//===========================================================================

class GCElseDef : public GCDef
  {
  public:
  GCElseDef() : GCDef("Else")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB)
    { ASSERT(FALSE); return NULL; };
  };

GCElseDef GCElse;

//===========================================================================

START_CODE(Iif);
  pGCIns m_pFalseJmpPos;          //position to jump to if the expression is False
  virtual pchar    GetDesc(pchar Buff);
MID_CODE(Iif,"iif",DefExp,0);
  virtual pGCIns   Construct(GCInsMngr & IB);
END_CODE(Iif);

//---------------------------------------------------------------------------

pGCIns GCIifDef::Construct(GCInsMngr & IB)
  {
  IB.DoConstructParms(1, 0x0, True, False); //expression
  IB.m_pH->m_bBreakLine = 1;
  pGCIns pIns = new(IB) GCIifIns;
  IB.DoConstructParms(1, 0x0, False, False); //TruePart
  pGCJmpIns pJmpIns = (pGCJmpIns)GCJmp.Construct(IB);
  ((pGCIifIns)pIns)->m_pFalseJmpPos = IB.m_pIPtr;
  IB.DoConstructParms(1, 0x0, False, True); //FalsePart
  pJmpIns->m_pJmpPos = IB.m_pIPtr;
  return pIns;
  }

//---------------------------------------------------------------------------

void GCIifIns::Exec(GCInsMngr & IB)
  {
  --IB.m_iCalcPos;
  if (IB.m_dCalcStack[IB.m_iCalcPos+1] == dGCfalse)
    {
    IB.m_pIPtr = m_pFalseJmpPos;
    if (IB.m_bDoDebug)
      IB.m_pcIPtr -= sizeof(pCPGMDbg);
    }
  else
    Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCIifIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  char Buff2[256];
  sprintf(Buff, "%s-->(%s)", GCIns::GetDesc(Buff1), m_pFalseJmpPos->GetDesc(Buff2));
  return Buff;
  }

//===========================================================================

class GCEndDef : public GCDef
  {
  public:
  GCEndDef() : GCDef("End")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

//---------------------------------------------------------------------------

pGCIns GCEndDef::Construct(GCInsMngr & IB)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  pGCDef pDef;
  pGCVar pVar;
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (IB.m_pH->m_pTknParser->TokType() == TokAlpha)
    {
    pVar = IB.m_pVarList->Find(Tkn);
    pDef = GCDef::Find(Tkn);
    if ((pVar) || (pDef))
      IB.m_pH->m_pTknParser->SetReturnSameToken();
    }
  else
    IB.m_pH->m_pTknParser->SetReturnSameToken();
  IB.m_pH->m_bBreakLine = 1;
  return new(IB) GCEndifIns;
  }

GCEndDef GCEnd;

//===========================================================================

class GCBeginDef : public GCDef
  {
  public:
  GCBeginDef() : GCDef("Begin")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

//---------------------------------------------------------------------------

pGCIns GCBeginDef::Construct(GCInsMngr & IB)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  pGCDef pDef;
  pGCVar pVar;
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (IB.m_pH->m_pTknParser->TokType()==TokAlpha)
    {
    pVar = IB.m_pVarList->Find(Tkn);
    pDef = GCDef::Find(Tkn);
    if ((pVar) || (pDef))
      IB.m_pH->m_pTknParser->SetReturnSameToken(); // if neither a defined variable or known "reserved word" ignore
    }
  else
    IB.m_pH->m_pTknParser->SetReturnSameToken();
  IB.DoConstructExprssn();
  pGCIns pIns = new(IB) GCIfIns;
  IB.Build(&GCEnd, pIns);
  return pIns;
  }

GCBeginDef GCBegin;

//===========================================================================

class GCEndWhileDef : public GCDef
  {
  public:
  GCEndWhileDef() : GCDef("EndWhile")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

//---------------------------------------------------------------------------

pGCIns GCEndWhileDef::Construct(GCInsMngr & IB)
  {
  pGCIns pIns = new(IB) GCJmpIns;
  ((pGCJmpIns)pIns)->m_pJmpPos = IB.m_pH->m_WhileStack[IB.m_pH->m_iWhilePos--];
  IB.m_pH->m_bBreakLine = 1;
  return new(IB) GCEndifIns;
  }

GCEndWhileDef GCEndWhile;

//===========================================================================

class GCWhileDef : public GCDef
  {
  public:
  GCWhileDef() : GCDef("While")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

//---------------------------------------------------------------------------

pGCIns GCWhileDef::Construct(GCInsMngr & IB)
  {
  IB.m_pH->m_bWhileIns = 1;
  IB.DoConstructExprssn();
  IB.m_pH->m_bBreakLine = 1;
  pGCIns pIns = new(IB) GCIfIns;
  IB.Build(&GCEndWhile, pIns);
  return pIns;
  }

GCWhileDef GCWhile;

//===========================================================================

class GCEOFIns : public GCIns
  {
  public:
  GCEOFIns() {};
  virtual void Exec(GCInsMngr & IB)
    { Advance(IB); };
  virtual void Advance(GCInsMngr & IB)
    { IB.m_pIPtr = NULL; };
  virtual pGCDef Code();
  };

//---------------------------------------------------------------------------

class GCEOFDef : public GCDef
  {
  public:
  GCEOFDef() : GCDef("$")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

//---------------------------------------------------------------------------

pGCIns GCEOFDef::Construct(GCInsMngr & IB)
  {
  IB.m_pH->m_bEOF = 1;
  //IB.m_bBreakLine = 1;
  IB.m_pEndIns = new(IB) GCEOFIns;
  return IB.m_pEndIns;
  }

//---------------------------------------------------------------------------

GCEOFDef GCEOF;

pGCDef GCEOFIns::Code()
  { return & GCEOF; }

//===========================================================================

class GCEOF2Ins : public GCIns
  {
  public:
  GCEOF2Ins() {};
  virtual void Exec(GCInsMngr & IB)
    { Advance(IB); };
  virtual void Advance(GCInsMngr & IB)
    { IB.m_pIPtr = NULL; };
  virtual pGCDef Code();
  };

//---------------------------------------------------------------------------

class GCEOF2Def : public GCDef
  {
  public:
  GCEOF2Def() : GCDef("EndFile")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

//---------------------------------------------------------------------------

pGCIns GCEOF2Def::Construct(GCInsMngr & IB)
  {
  IB.m_pH->m_bEOF = 1;
  //IB.m_bBreakLine = 1;
  IB.m_pEndIns = new(IB) GCEOFIns;
  return IB.m_pEndIns;
  }

//---------------------------------------------------------------------------

GCEOF2Def GCEOF2;

pGCDef GCEOF2Ins::Code()
  { return & GCEOF2; }

//===========================================================================

class GCNxtBlkIns : public GCIns
  {
  public:
  pchar          m_pNxt;               //points to next block of instructions
  GCNxtBlkIns() {};
  virtual void Exec(GCInsMngr & IB)
    { Advance(IB); };
  virtual void Advance(GCInsMngr & IB)
    {
    IB.m_pcIPtr = m_pNxt;
    #if dbgGenCon
    if (dbgLoad())
      dbgMemIsFirstBlk = 0;
    #endif
    };
  virtual pGCDef Code();
  };

//---------------------------------------------------------------------------

class GCNxtBlkDef : public GCDef
  {
  public:
  GCNxtBlkDef() : GCDef("#...#")
    {};
  virtual pGCIns Construct(GCInsMngr & IB)
    { return new(IB) GCNxtBlkIns; };
  };

//---------------------------------------------------------------------------

GCNxtBlkDef GCNxtBlk;

pGCDef GCNxtBlkIns::Code()
  { return &GCNxtBlk; }

//===========================================================================

class GCOBraceDef : public GCDef
  {
  public:
  GCOBraceDef() : GCDef("(")
    { m_defFlags = DefExp; };
  virtual pGCIns Construct(GCInsMngr & IB)
    { ASSERT(FALSE); return NULL; };
  };

GCOBraceDef GCOBrace;

//===========================================================================

START_CODE(VarRef);
  pGCVar           m_pVar;    //points to instance of GCVar (identifier)
  virtual pchar    GetDesc(pchar Buff);
MID_CODE(VarRef,"#VarRef#",DefLHS | DefExp,0);
  virtual pGCIns   Construct(GCInsMngr & IB)
    { return new(IB) GCVarRefIns; };
END_CODE(VarRef);

//---------------------------------------------------------------------------

void GCVarRefIns::Exec(GCInsMngr & IB)
  {
  if (m_pVar->WhatAmI()==VarStr)
    {
    if (IB.m_iStrPos >= MAX_STR_STACK)
      IB.Err(ErrOtherRuntime, 6);
    IB.m_StrStack[++IB.m_iStrPos] = ((pGCStrVar)m_pVar)->m_var;
    }
  else
    {
    if (IB.m_iCalcPos >= MAX_CALC_STACK)
      IB.Err(ErrOtherRuntime, 3);
    IB.m_dCalcStack[++IB.m_iCalcPos] = m_pVar->getD();
    }
  Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCVarRefIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  sprintf(Buff, "%s(%s)", GCIns::GetDesc(Buff1), m_pVar->Name());
  return Buff;
  }

//===========================================================================

START_CODE(VarTagRef);
  GCTagVar*        m_pVar;     //points to instance of GCTagVar (external identifier)
  Strng            m_sCnvTxt;  //conversion text to be used
  virtual pchar    GetDesc(pchar Buff);
MID_CODE(VarTagRef,"#VarTagRef#",DefLHS | DefExp,0);
  virtual pGCIns   Construct(GCInsMngr & IB)
    { return new(IB) GCVarTagRefIns; };
END_CODE(VarTagRef);

//---------------------------------------------------------------------------

void GCVarTagRefIns::Exec(GCInsMngr & IB)
  {
  if (IB.m_iCalcPos >= MAX_CALC_STACK)
    IB.Err(ErrOtherRuntime, 3);
  IB.m_dCalcStack[++IB.m_iCalcPos] = Cnvs[m_pVar->m_iCnvIndex]->Human(m_pVar->getD(), m_sCnvTxt());
  Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCVarTagRefIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  sprintf(Buff, "%s(%s %s)", GCIns::GetDesc(Buff1), m_pVar->Name(), m_sCnvTxt());
  return Buff;
  }

//===========================================================================

START_CODE(VarClassRef);
  short            m_Index;   //index to var within class (the class will be on the top of the class stack)
  Strng            m_sVarName; //name of var if it is a class within a class
  virtual pchar    GetDesc(pchar Buff);
MID_CODE(VarClassRef,"#VarClassRef#",0,0);
  virtual pGCIns   Construct(GCInsMngr & IB)
    { return new(IB) GCVarClassRefIns; };
END_CODE(VarClassRef);

//---------------------------------------------------------------------------

void GCVarClassRefIns::Exec(GCInsMngr & IB)
  {
  if (IB.m_iCalcPos >= MAX_CALC_STACK)
    IB.Err(ErrOtherRuntime, 3);
  GCClassVar* pClassVar = IB.m_pClassRefStack[IB.m_iClassRefPos];
  ASSERT(m_Index>=0);
  ASSERT(pClassVar->WhatAmI()==VarClass);
if (pClassVar==NULL)
{
int xx=0; //TROUBLE!!!
}
  GCVar* pVar = pClassVar->m_pVarList->Find(m_Index);
  if (m_sVarName.Length()==0)
    {
    ASSERT(pVar);
if (pVar==NULL)
{
int xx=0; //TROUBLE!!!
pVar = pClassVar->m_pVarList->Find(m_Index);
}
    if (pVar->WhatAmI()==VarStr)
      {
      if (IB.m_iStrPos >= MAX_STR_STACK)
        IB.Err(ErrOtherRuntime, 6);
      IB.m_StrStack[++IB.m_iStrPos] = ((pGCStrVar)pVar)->m_var;
      }
    else
      {
      if (IB.m_iCalcPos >= MAX_CALC_STACK)
        IB.Err(ErrOtherRuntime, 3);
      IB.m_dCalcStack[++IB.m_iCalcPos] = pVar->getD();
      }
    }
  else
    {
    pVar = ((pGCClassVar)pVar)->m_pVarList->Find(strchr(m_sVarName(), '.')+1);
    if (pVar->WhatAmI()==VarStr)
      {
      if (IB.m_iStrPos >= MAX_STR_STACK)
        IB.Err(ErrOtherRuntime, 6);
      IB.m_StrStack[++IB.m_iStrPos] = ((pGCStrVar)pVar)->m_var;
      }
    else
      {
      if (IB.m_iCalcPos >= MAX_CALC_STACK)
        IB.Err(ErrOtherRuntime, 3);
      IB.m_dCalcStack[++IB.m_iCalcPos] = pVar->getD();
      }
    }
  Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCVarClassRefIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  if (m_sVarName.Length()>0)
    sprintf(Buff, "%s(Indx:%d %s)", GCIns::GetDesc(Buff1), m_Index, m_sVarName());
  else
    sprintf(Buff, "%s(Indx:%d)", GCIns::GetDesc(Buff1), m_Index);
  return Buff;
  }

//===========================================================================

START_CODE(Const);
  double m_Data;   //contains the numerical constant
  virtual pchar    GetDesc(pchar Buff);
//MID_CODE(Const,"#Const#",DefLHS | DefExp,0);
MID_CODE(Const,"#Const#", DefExp,0);
  virtual pGCIns   Construct(GCInsMngr & IB)
    { return new(IB) GCConstIns; };
END_CODE(Const);

//---------------------------------------------------------------------------

void GCConstIns::Exec(GCInsMngr & IB)
  {
  if (IB.m_iCalcPos >= MAX_CALC_STACK)
    IB.Err(ErrOtherRuntime, 3);
  IB.m_dCalcStack[++IB.m_iCalcPos] = m_Data;
  Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCConstIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  sprintf(Buff, "%s(%.5f)", GCIns::GetDesc(Buff1), m_Data);
  return Buff;
  }

//===========================================================================

START_CODE(ConstStr);
  Strng m_Data;   //contains the string constant
  virtual pchar    GetDesc(pchar Buff);
MID_CODE(ConstStr,"#ConstStr#", DefExp,0);
  virtual pGCIns   Construct(GCInsMngr & IB)
    { return new(IB) GCConstStrIns; };
END_CODE(ConstStr);

//---------------------------------------------------------------------------

void GCConstStrIns::Exec(GCInsMngr & IB)
  {
  if (IB.m_iStrPos >= MAX_STR_STACK)
    IB.Err(ErrOtherRuntime, 6);
  IB.m_StrStack[++IB.m_iStrPos] = m_Data;
  Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCConstStrIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  if (m_Data.Length()>0)
    sprintf(Buff, "%s(\"%s\")", GCIns::GetDesc(Buff1), m_Data());
  else
    sprintf(Buff, "%s(\"\")", GCIns::GetDesc(Buff1));
  return Buff;
  }

//===========================================================================

START_CODE(Equal);
  pGCVar           m_pVar;     //points to instance of GCVar (identifier)
  virtual pchar    GetDesc(pchar Buff);
MID_CODE(Equal,"=",Def2ndLHS,6);
  virtual pGCIns     Construct(GCInsMngr & IB);
END_CODE(Equal);

//---------------------------------------------------------------------------

pGCIns GCEqualDef::Construct(GCInsMngr & IB)
  {
  if (!IB.m_pH->m_bEqualFlag)
    {
    IB.DoConstructExprssn((flag)IB.m_pH->m_bExpectStr);
    IB.m_pH->m_bBreakLine = 1;
    }
  return new(IB) GCEqualIns;
  }

//---------------------------------------------------------------------------

void GCEqualIns::Exec(GCInsMngr & IB)
  {
  if (m_pVar->WhatAmI()==VarStr)
    ((pGCStrVar)m_pVar)->set((IB.m_StrStack[IB.m_iStrPos--])());
  else
    m_pVar->set(IB.m_dCalcStack[IB.m_iCalcPos--]);
  Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCEqualIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  sprintf(Buff, "%s(%s)", GCIns::GetDesc(Buff1), m_pVar->Name());
  return Buff;
  }

//===========================================================================

START_CODE(EqualTag);
  GCTagVar*        m_pVar;     //points to instance of GCTagVar (external identifier)
  Strng            m_sCnvTxt;  //conversion text to be used
  virtual pchar    GetDesc(pchar Buff);
MID_CODE(EqualTag,"#=Tag#",0,6);
  virtual pGCIns     Construct(GCInsMngr & IB);
END_CODE(EqualTag);

//---------------------------------------------------------------------------

pGCIns  GCEqualTagDef::Construct(GCInsMngr & IB)
  {
  if (!IB.m_pH->m_bEqualFlag)
    {
    IB.DoConstructExprssn((flag)IB.m_pH->m_bExpectStr);
    IB.m_pH->m_bBreakLine = 1;
    }
  return new(IB) GCEqualTagIns;
  }

//---------------------------------------------------------------------------

void GCEqualTagIns::Exec(GCInsMngr & IB)
  {
  m_pVar->set(Cnvs[m_pVar->m_iCnvIndex]->Normal(IB.m_dCalcStack[IB.m_iCalcPos--], m_sCnvTxt()));
  Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCEqualTagIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  sprintf(Buff, "%s(%s %s)", GCIns::GetDesc(Buff1), m_pVar->Name(), m_sCnvTxt());
  return Buff;
  }

//===========================================================================

START_CODE(EqualClassRef);
  short            m_Index;    //index to class var
  Strng            m_sVarName; //name of var if it is a class within a class
  virtual pchar    GetDesc(pchar Buff);
MID_CODE(EqualClassRef,"#=#",0,6);
  virtual pGCIns     Construct(GCInsMngr & IB);
END_CODE(EqualClassRef);

//---------------------------------------------------------------------------

pGCIns GCEqualClassRefDef::Construct(GCInsMngr & IB)
  {
  if (!IB.m_pH->m_bEqualFlag)
    {
    IB.DoConstructExprssn((flag)IB.m_pH->m_bExpectStr);
    IB.m_pH->m_bBreakLine = 1;
    }
  return new(IB) GCEqualClassRefIns;
  }

//---------------------------------------------------------------------------

void GCEqualClassRefIns::Exec(GCInsMngr & IB)
  {
  ASSERT(m_Index>=0);
  if (m_sVarName.Length()==0)
    {
    ASSERT(IB.m_pClassRefStack[IB.m_iClassRefPos]->WhatAmI()==VarClass);
    pGCVar pVar = IB.m_pClassRefStack[IB.m_iClassRefPos]->m_pVarList->Find(m_Index);
    if (pVar->WhatAmI()==VarStr)
      ((pGCStrVar)pVar)->set((IB.m_StrStack[IB.m_iStrPos--])());
    else
      pVar->set(IB.m_dCalcStack[IB.m_iCalcPos--]);
    }
  else
    {
    pGCClassVar pClassVar = (pGCClassVar)IB.m_pClassRefStack[IB.m_iClassRefPos]->m_pVarList->Find(m_Index);
    //pClassVar->m_pVarList->Find(strchr(m_sVarName(),'.')+1)->set(IB.m_dCalcStack[IB.m_iCalcPos--]);
    pGCVar pVar = pClassVar->m_pVarList->Find(strchr(m_sVarName(),'.')+1);
    if (pVar->WhatAmI()==VarStr)
      ((pGCStrVar)pVar)->set((IB.m_StrStack[IB.m_iStrPos--])());
    else
      pVar->set(IB.m_dCalcStack[IB.m_iCalcPos--]);
    }
  Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCEqualClassRefIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  sprintf(Buff, "%s(Indx:%d %s)", GCIns::GetDesc(Buff1), m_Index, m_sVarName());
  return Buff;
  }

//===========================================================================

class GCConstVarDef : public GCDef
  {
  public:
  GCConstVarDef() : GCDef("Const")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

//---------------------------------------------------------------------------

pGCIns GCConstVarDef::Construct(GCInsMngr & IB)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  pGCDef pDef = GCDef::Find(Tkn);
  if (pDef)
    {
    if (!(pDef->m_defFlags & DefDataType))
      IB.Err(ErrBuild, 100);
    }
  else
    IB.Err(ErrBuild, 100);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (IB.m_pH->m_pTknParser->TokType() != TokAlpha)
    IB.Err(ErrBuild, 101);
  pGCVar pVar = IB.m_pVarList->Find(Tkn);
  if (pVar)
    IB.Err(ErrBuild, 102);
  pVar = pDef->ConstructVar(IB.m_pVarList, &IB.m_VarMap, Tkn, VarConst);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != '=')
    IB.Err(ErrBuild, 103);
  IB.DoConstructExprssn();
  IB.m_pH->m_bBreakLine = 1;
  pGCIns pIns = new(IB) GCEqualIns;
  ((pGCEqualIns)pIns)->m_pVar = pVar;
  return pIns;
  }

GCConstVarDef GCConstVar;

//===========================================================================

START_CODE(Lbl);
  Strng m_sName;
  virtual pchar    GetDesc(pchar Buff);
MID_CODE(Lbl,"#Lbl#",0,0);
  virtual pGCIns   Construct(GCInsMngr & IB)
    { return new(IB) GCLblIns; };
END_CODE(Lbl);

//---------------------------------------------------------------------------

void GCLblIns::Exec(GCInsMngr & IB)
  {
  Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCLblIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  sprintf(Buff, "%s(%s)", GCIns::GetDesc(Buff1), m_sName());
  return Buff;
  }

//===========================================================================

class GCTagDef : public GCDef
  {
  public:
  GCTagDef() : GCDef("[")
    { m_defFlags = DefLHS | DefExp; };
  virtual pGCIns Construct(GCInsMngr & IB)
    { IB.DoConstructTag(']'); return NULL; };
  virtual pGCVar ConstructVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags)
    { return new GCTagVar(pFirstVar, pVarMap, pName, flags); }
  };
GCTagDef GCTag;

//===========================================================================

class GCStrTagDef : public GCDef
  {
  public:
  GCStrTagDef() : GCDef("S[")
    { m_defFlags = DefLHS | DefExp/* | DefRetStr*/; };
  virtual pGCIns Construct(GCInsMngr & IB)
    { IB.DoConstructTag(']'); return NULL; };
  virtual pGCVar ConstructVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags)
    { return new GCStrTagVar(pFirstVar, pVarMap, pName, flags); }
  };
GCStrTagDef GCStrTag;

//===========================================================================

START_CODE(Return);
MID_CODE(Return,"Return",DefLHS,0);
  virtual pGCIns   Construct(GCInsMngr & IB);
END_CODE(Return);

//---------------------------------------------------------------------------

pGCIns GCReturnDef::Construct(GCInsMngr & IB)
  {
  if (IB.m_pH->m_pFunctVar==NULL)
    IB.Err(ErrBuild, 154);
  IB.m_pH->m_bFunctRetFnd = 1;
  IB.DoConstructExprssn((IB.m_pH->m_pFunctVar->m_flags & VarStrFunct)!=0);
  IB.m_pH->m_bBreakLine = 1;
  return new(IB) GCReturnIns;
  }

//---------------------------------------------------------------------------

void GCReturnIns::Exec(GCInsMngr & IB)
  {
  IB.m_pIPtr = IB.m_FntStack[IB.m_FntStackPos--];
  if (IB.m_bDoDebug)
    IB.m_pcIPtr -= sizeof(pCPGMDbg);
  }

//===========================================================================

START_CODE(PushClassRef);
  short        m_Index;      //index to var within class (-1 if not used)
  GCClassVar*  m_pClassVar;  //pointer to the class
  GCArrayVar*  m_pArrayVar;  //pointer to the array (NULL if not used)
  virtual pchar    GetDesc(pchar Buff);
MID_CODE(PushClassRef,"#PushClassRef#",0,0);
  virtual pGCIns   Construct(GCInsMngr & IB)
    { IB.m_pH->m_iPushPopClassDepth++; return new(IB) GCPushClassRefIns; };
END_CODE(PushClassRef);

//---------------------------------------------------------------------------

void GCPushClassRefIns::Exec(GCInsMngr & IB)
  {
  if (IB.m_iClassRefPos+1 == MAX_CLASSREF_STACK)
    IB.Err(ErrOtherRuntime, 4);
  if (m_pArrayVar)
    {
    int i = (int)IB.m_dCalcStack[IB.m_iCalcPos--];
    if (i<0 || i>=m_pArrayVar->m_iArrayLen)
      {
      IB.Err(ErrOtherRuntime, 17);
      i = 0;
      }
    pGCVar pVar = m_pArrayVar->m_pVars[i];
    IB.m_pClassRefStack[++IB.m_iClassRefPos] = (pGCClassVar)pVar;
    }
  else if (m_Index<0 && m_pClassVar==NULL)
    {//use class allready in stack, but one further back
    IB.m_iClassRefPos++;
    IB.m_pClassRefStack[IB.m_iClassRefPos] = IB.m_pClassRefStack[IB.m_iClassRefPos-2];
    }
  else if (m_Index<0)
    IB.m_pClassRefStack[++IB.m_iClassRefPos] = m_pClassVar;
  else
    {//handles class within a class
    int pos = IB.m_iClassRefPos;
    IB.m_iClassRefPos++;
    pGCVar pVar = IB.m_pClassRefStack[pos]->m_pVarList->Find(m_Index);
    while (pos>0 && (pVar==NULL || pVar->WhatAmI()!=VarClass || _stricmp(pVar->Name(), m_pClassVar->Name())!=0))
      {
      pos--;
      pVar = IB.m_pClassRefStack[pos]->m_pVarList->Find(m_Index);
      }
    ASSERT(pVar && pVar->WhatAmI()==VarClass && _stricmp(pVar->Name(), m_pClassVar->Name())==0);
    //it is remotely possible for all the above conditions to be met, but pVar points to the wrong class?
    IB.m_pClassRefStack[IB.m_iClassRefPos] = (pGCClassVar)pVar;
    }
  Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCPushClassRefIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  if (m_pArrayVar)
    sprintf(Buff, "%s(ArrayClass %s)", GCIns::GetDesc(Buff1), m_pArrayVar->Name());
  else
    sprintf(Buff, "%s(Indx:%d %s)", GCIns::GetDesc(Buff1), m_Index, m_pClassVar ? m_pClassVar->Name() : "");
  return Buff;
  }

//===========================================================================

START_CODE(PopClassRef);
MID_CODE(PopClassRef,"#PopClassRef#",0,0);
  virtual pGCIns   Construct(GCInsMngr & IB)
    { IB.m_pH->m_iPushPopClassDepth--; return new(IB) GCPopClassRefIns; };
END_CODE(PopClassRef);

//---------------------------------------------------------------------------

void GCPopClassRefIns::Exec(GCInsMngr & IB)
  {
  ASSERT(IB.m_iClassRefPos>=0);
  IB.m_pClassRefStack[IB.m_iClassRefPos] = NULL;
  IB.m_iClassRefPos--;
  Advance(IB);
  }

//===========================================================================

START_CODE(EndFunct);
MID_CODE(EndFunct,"EndFunct",DefLHS,0);
  virtual pGCIns   Construct(GCInsMngr & IB)
    {
    if (IB.m_pH->m_pFunctVar==NULL)
      IB.Err(ErrBuild, 155);
    if (IB.m_pH->m_bFunctRetFnd==0)
      IB.Err(ErrBuild, 156);
    IB.m_pH->m_pFunctVar = NULL;
    return new(IB) GCEndFunctIns;
    };
END_CODE(EndFunct);

//---------------------------------------------------------------------------

void GCEndFunctIns::Exec(GCInsMngr & IB)
  {
  IB.Err(ErrOtherRuntime, 1);
  }

//===========================================================================

class GCClassDef : public GCDef
  {
  public:
  GCClassDef() : GCDef("Class")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  virtual pGCVar ConstructVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags)
    { return new GCClassVar(pFirstVar, pVarMap, pName, flags); }
  };

GCClassDef GCClass;

//---------------------------------------------------------------------------

pGCIns GCClassDef::Construct(GCInsMngr & IB)
  {
  IB.DoConstructClass();
  return NULL;
  }

//===========================================================================

class GCEndClassDef : public GCDef
  {
  public:
  GCEndClassDef() : GCDef("EndClass") {};
  virtual pGCIns Construct(GCInsMngr & IB)
    { return NULL; };
  };

GCEndClassDef GCEndClass;

//===========================================================================

class GCClassDecDef : public GCDef
  {
  public:
  GCClassDecDef() : GCDef("#ClassDec#") { };
  virtual pGCIns Construct(GCInsMngr & IB) { return NULL; };
  virtual pGCVar ConstructVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags)
    { return new GCClassVar(pFirstVar, pVarMap, pName, flags, m_pClassVar); };
  GCClassVar* m_pClassVar;
  };

GCClassDecDef GCClassDec;

//===========================================================================

class GCWatchDef : public GCDef
  {
  public:
  GCWatchDef() : GCDef("Watch")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

GCWatchDef GCWatch;

//---------------------------------------------------------------------------

pGCIns GCWatchDef::Construct(GCInsMngr & IB)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  bool Valid = true;
  while (Valid)
    {
    strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
    GCDef* pDef = GCDef::Find(Tkn);
    if (pDef)
      IB.Err(ErrBuild, 121);
    GCVar* pVar = IB.m_pVarList->Find(Tkn);
    if (pVar == NULL)
      IB.Err(ErrBuild, 121);
    GCClassVar* pClassVar = NULL;
    GCArrayVar* pArrayVar = NULL;
    switch (pVar->WhatAmI())
      {
      case VarClass: pClassVar = (GCClassVar*)pVar; pVar = NULL; break;
      case VarArray: pArrayVar = (GCArrayVar*)pVar; pVar = NULL; break;
      case VarFunct:
      case VarTag:
        IB.Err(ErrBuild, 124);
        break;
      }
    bool ReadOnly = false;
    strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if (strcmp(Tkn,"@") == 0)
      {
      ReadOnly = true;
      strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
      }
    if (pArrayVar)
      {
      if (strcmp(Tkn,"[") != 0)
        IB.Err(ErrBuild, 161);
      strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
      if (IB.m_pH->m_pTknParser->TokType() != TokNumeric)
        IB.Err(ErrBuild, 160);
      int index = atoi(Tkn);
      if (index<0 || index>=pArrayVar->m_iArrayLen)
        IB.Err(ErrBuild, 167);
      strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
      if (strcmp(Tkn,"]") != 0)
        IB.Err(ErrBuild, 162);
      strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
      if (pArrayVar->m_pVars[index]->WhatAmI()==VarClass)
        pClassVar = (GCClassVar*)(pArrayVar->m_pVars[index]);
      //else
      //  pVar = pArrayVar->m_pVars[index];
      }
    if (pVar)
      pVar->m_flags |= (ReadOnly ? (VarPgmWatch|VarReadOnly) : VarPgmWatch);
    if (pClassVar)
      {
      if (pClassVar->IsArrayClass() || pClassVar->IsMatrixClass())
        {//found array class, determine index for watch
        if (strcmp(Tkn,"[") != 0)
          IB.Err(ErrBuild, 161);
        strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
        /*if (strcmp(Tkn,"]") == 0)
          {
          IB.AddArrayWatch(pClassVar, ReadOnly, -1, -1);//watch all
          strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
          if (strcmp(Tkn,"@") == 0)
            {
            ReadOnly = true;
            strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
            }
          }
        else*/
          {
          int index0 = -1;
          int index1 = -1;
          bool DoAll = false;
          if (_stricmp(Tkn,"All") == 0)
            {
            DoAll = true;
            strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
            if (strcmp(Tkn,",") != 0)
              IB.Err(ErrBuild, 163);
            strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
            }
          if (IB.m_pH->m_pTknParser->TokType() != TokNumeric)
            IB.Err(ErrBuild, 160);
          index0 = Max(DoAll ? 1 : 0, atoi(Tkn));
          if (pClassVar->IsMatrixClass())
            {
            strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
            if (strcmp(Tkn,",") != 0)
              IB.Err(ErrBuild, 163);
            strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
            if (IB.m_pH->m_pTknParser->TokType() != TokNumeric)
              IB.Err(ErrBuild, 160);
            index1 = Max(DoAll ? 1 : 0, atoi(Tkn));
            }
          strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
          if (strcmp(Tkn,"]") != 0)
            IB.Err(ErrBuild, 162);
          strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
          if (strcmp(Tkn,"@") == 0)
            {
            ReadOnly = true;
            strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
            }
          if (DoAll)
            {
            for (int i=0; i<index0; i++)
              {
              for (int j=0; j<(index1<0 ? 1 : index1); j++)
                IB.AddArrayWatch(pClassVar, ReadOnly, i, index1<0 ? -1 : j);
              }
            }
          else
            IB.AddArrayWatch(pClassVar, ReadOnly, index0, index1);
          }
        }
      else
        {
        int VarCount = pClassVar->GetVarCount();
        for (int i=0; i<VarCount && pVar==NULL; i++)
          {
          pGCVar pAVar = pClassVar->GetVarByIndex(i);
          if (pAVar->WhatAmI() != VarFunct && pAVar->WhatAmI() != VarClass)
            pAVar->m_flags |= (ReadOnly ? (VarPgmWatch|VarReadOnly) : VarPgmWatch);
          }
        }
      }
    if (strcmp(Tkn,",") != 0)
      Valid = false;
    }
  IB.m_pH->m_pTknParser->SetReturnSameToken();
  return NULL;
  }

//===========================================================================

class GCExcludeWatchDef : public GCDef
  {
  public:
  GCExcludeWatchDef() : GCDef("ExcludeWatch")
    { m_defFlags = DefLHS; };
  void ExcludeClassVars(GCInsMngr & IB, GCClassVar* pClassVar);
  virtual pGCIns Construct(GCInsMngr & IB);
  };

GCExcludeWatchDef GCExcludeWatch;

//---------------------------------------------------------------------------

void GCExcludeWatchDef::ExcludeClassVars(GCInsMngr & IB, GCClassVar* pClassVar)
  {
  if (pClassVar->IsArrayClass() || pClassVar->IsMatrixClass())
    {//found array class, determine index for watch
    IB.Err(ErrBuild, 168);
    }
  else
    {
    int VarCount = pClassVar->GetVarCount();
    for (int i=0; i<VarCount; i++)
      {
      pGCVar pAVar = pClassVar->GetVarByIndex(i);
      if (pAVar->WhatAmI() == VarClass)
        {
        GCClassVar* pClassV = (GCClassVar*)pAVar;
        if (!pClassV->IsArrayClass() && !pClassV->IsMatrixClass())
          ExcludeClassVars(IB, pClassV);
        }
      else if (pAVar->WhatAmI() != VarFunct && pAVar->WhatAmI() != VarTag)
        pAVar->m_flags &= ~(VarPgmWatch);
      }
    }
  }

//---------------------------------------------------------------------------

pGCIns GCExcludeWatchDef::Construct(GCInsMngr & IB)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  bool Valid = true;
  while (Valid)
    {
    strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
    GCDef* pDef = GCDef::Find(Tkn);
    if (pDef)
      IB.Err(ErrBuild, 121);
    GCVar* pVar = IB.m_pVarList->Find(Tkn);
    if (pVar == NULL)
      IB.Err(ErrBuild, 121);
    GCClassVar* pClassVar = NULL;
    GCArrayVar* pArrayVar = NULL;
    switch (pVar->WhatAmI())
      {
      case VarClass: pClassVar = (GCClassVar*)pVar; pVar=NULL; break;
      case VarArray: pArrayVar = (GCArrayVar*)pVar; pVar=NULL; break;
      case VarFunct:
      case VarTag:
        IB.Err(ErrBuild, 168);
        break;
      }
    strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if (pArrayVar)
      {
      if (strcmp(Tkn,"[") != 0)
        IB.Err(ErrBuild, 161);
      strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
      if (IB.m_pH->m_pTknParser->TokType() != TokNumeric)
        IB.Err(ErrBuild, 160);
      int index = atoi(Tkn);
      if (index<0 || index>=pArrayVar->m_iArrayLen)
        IB.Err(ErrBuild, 167);
      strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
      if (strcmp(Tkn,"]") != 0)
        IB.Err(ErrBuild, 162);
      strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
      if (pArrayVar->m_pVars[index]->WhatAmI()==VarClass)
        pClassVar = (GCClassVar*)(pArrayVar->m_pVars[index]);
      //else
      //  pVar = pArrayVar->m_pVars[index];
      }
    if (pVar)
      pVar->m_flags &= ~(VarPgmWatch);
    if (pClassVar)
      ExcludeClassVars(IB, pClassVar);
    if (strcmp(Tkn,",") != 0)
      Valid = false;
    }
  IB.m_pH->m_pTknParser->SetReturnSameToken();
  return NULL;
  }

//===========================================================================

class GCWatchInfoDef : public GCDef
  {
  public:
  GCWatchInfoDef() : GCDef("WatchInfo")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

GCWatchInfoDef GCWatchInfo;

//---------------------------------------------------------------------------

pGCIns GCWatchInfoDef::Construct(GCInsMngr & IB)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0]!='(')
    IB.Err(ErrBuild, 104);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  pGCDef pDef = GCDef::Find(Tkn);
  if (pDef)
    IB.Err(ErrBuild, 121);
  pGCVar pVar = IB.m_pVarList->Find(Tkn);
  if (pVar == NULL)
    IB.Err(ErrBuild, 121);
  switch (pVar->WhatAmI())
    {
    case VarClass:
    case VarFunct:
    case VarArray:
    case VarTag:
    case VarStr:
      IB.Err(ErrBuild, 136);
      break;
    default:
      pVar->m_flags |= VarPgmWatch;
    }
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0]!=',')
    IB.Err(ErrBuild, 106);
  double mn = IB.ParseNumber();
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0]!=',')
    IB.Err(ErrBuild, 106);
  double mx = Max(mn, IB.ParseNumber());
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0]!=',')
    IB.Err(ErrBuild, 106);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  Strng Desc;
  if (Tkn[0]!=')')
    {
    Desc = Tkn;
    strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
    }
  if (Tkn[0]!=')')
    IB.Err(ErrBuild, 107);
  switch (pVar->WhatAmI())
    {
    case VarDouble :
      break;
    case VarLong :
    case VarByte :
      mn = Range((double)LONG_MIN, mn, (double)LONG_MAX);
      mx = Max(mn, Range((double)LONG_MIN, mx, (double)LONG_MAX));
      break;
    case VarBit :
      mn = Range(0.0, mn, (double)UCHAR_MAX);
      mx = Max(mn, Range(0.0, mx, (double)UCHAR_MAX));
      break;
    default:
      ASSERT(0);
      break;
    }
  pVar->AddWatchInfo(mn, mx, Desc());
  return NULL;
  }

//===========================================================================

class GCTextLabelDef : public GCDef
  {
  public:
  GCTextLabelDef() : GCDef("TextLabel")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

GCTextLabelDef GCTextLabel;

//---------------------------------------------------------------------------

pGCIns GCTextLabelDef::Construct(GCInsMngr & IB)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (strcmp(Tkn,"(")!=0)
    IB.Err(ErrBuild, 104);
  char Lbl[32];
  bool Done = false;
  while (!Done)
    {
    sprintf(Lbl, "Label_%d", IB.m_pH->m_iTxtLblCnt++);
    strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if ((strcmp(Tkn,")")==0) || (strcmp(Tkn,",") == 0))
      {//blank line
      if (IB.m_pH->m_pClassVar)
        GCVar* pLabel = new GCLabelVar(IB.m_pH->m_pClassVar->m_pVarList, &IB.m_pH->m_pClassVar->m_VarMap, Lbl, 0);
      else
        GCVar* pLabel = new GCLabelVar(IB.m_pVarList, &IB.m_VarMap, Lbl, VarPgmWatch);
	    }
    else
      {
      if (IB.m_pH->m_pClassVar)
        GCVar* pLabel = new GCLabelVar(IB.m_pH->m_pClassVar->m_pVarList, &IB.m_pH->m_pClassVar->m_VarMap, Lbl, 0, Tkn);
      else
        GCVar* pLabel = new GCLabelVar(IB.m_pVarList, &IB.m_VarMap, Lbl, VarPgmWatch, Tkn);
      strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
      if ((strcmp(Tkn,")")!=0) && (strcmp(Tkn,",")!=0))
        IB.Err(ErrBuild, 107);
    	}
    if (strcmp(Tkn,")")==0)
      Done = true;
    }
  return NULL;
  }

//===========================================================================

class GCPageLabelDef : public GCDef
  {
  public:
  GCPageLabelDef() : GCDef("PageLabel")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

GCPageLabelDef GCPageLabel;

//---------------------------------------------------------------------------

pGCIns GCPageLabelDef::Construct(GCInsMngr & IB)
  {
  if (IB.m_pH->m_iPgLblCnt>=MaxAccWndTabPages)
    IB.Err(ErrBuild, 169);
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (strcmp(Tkn,"(")!=0)
    IB.Err(ErrBuild, 104);
  char Lbl[32];
  sprintf(Lbl, "Page_%d", IB.m_pH->m_iPgLblCnt++);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (strcmp(Tkn,")")==0)
    {//blank line
    GCLabelVar* pLabel = new GCLabelVar(IB.m_pVarList, &IB.m_VarMap, Lbl, VarPgmWatch|VarPageLabel, "..");
    }
  else
    {
    if (strlen(Tkn)==0)
      sprintf(Tkn, "..");
    else if (strlen(Tkn)>60)
      Tkn[60] = 0;
    GCLabelVar* pLabel = new GCLabelVar(IB.m_pVarList, &IB.m_VarMap, Lbl, VarPgmWatch|VarPageLabel, Tkn);
    strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if (strcmp(Tkn,")")!=0)
      IB.Err(ErrBuild, 107);
  	}
  return NULL;
  }

//===========================================================================

class GCDynamicTagsDef : public GCDef
  {
  public:
  GCDynamicTagsDef() : GCDef("DynamicTags")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

GCDynamicTagsDef GCDynamicTags;

//---------------------------------------------------------------------------

pGCIns GCDynamicTagsDef::Construct(GCInsMngr & IB)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  flag Valid = TRUE;
  while (Valid)
    {
    pGCVar pVar = IB.DoConstructTag('"');
    pVar->m_flags |= (VarTagIn | VarTagOut); //don't know if it is in or out so set both
    pVar->m_flags |= VarDynTag;
    strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if (strcmp(Tkn, ",") != 0)
      Valid = FALSE;
    IB.m_pH->m_iTempTagPos--;
    }
  IB.m_pH->m_pTknParser->SetReturnSameToken();
  return NULL;
  }

//===========================================================================

START_CODE(EnterFnt); //function needed by debugger to get break points right
MID_CODE(EnterFnt,"#EnterFunct#",DefExp | DefLHS,0);
  virtual pGCIns   Construct(GCInsMngr & IB);
END_CODE(EnterFnt);

//---------------------------------------------------------------------------

pGCIns GCEnterFntDef::Construct(GCInsMngr & IB)
  {
  IB.m_pH->m_bBreakLine = 1;
  return new(IB) GCEnterFntIns;
  }

//---------------------------------------------------------------------------

void GCEnterFntIns::Exec(GCInsMngr & IB)
  {
  Advance(IB);
  }

//===========================================================================

class GCFunctDef : public GCDef
  {
  public:
  GCFunctDef() : GCDef("Function")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  virtual pGCVar ConstructVar(pGCVar &pFirstVar, GCVarMap *pVarMap, pchar pName, DWORD flags)
    { return new GCFunctVar(pFirstVar, pVarMap, pName, flags); }
  };

GCFunctDef GCFunct;

//---------------------------------------------------------------------------

pGCIns GCFunctDef::Construct(GCInsMngr & IB)
  {//NB: same as GCStrFunctDef
  pGCIns pIns = new(IB) GCJmpIns;
  IB.m_pH->m_bFunctRetFnd = 0;
  pGCIns pTempIns;
  if (IB.m_bDoDebug)
    pTempIns = (pGCIns)(IB.m_pcIPtr + sizeof(pCPGMDbg));
  else
    pTempIns = IB.m_pIPtr;
  pGCFunctVar pFunctVar;
  if (IB.m_pH->m_pClassVar)
    pFunctVar = (pGCFunctVar)IB.DoConstructFnt(IB.m_pH->m_pClassVar->m_pVarList, &IB.m_pH->m_pClassVar->m_VarMap);
  else
    pFunctVar = (pGCFunctVar)IB.DoConstructFnt(IB.m_pVarList, &IB.m_VarMap);
  pFunctVar->m_pFntIPos = pTempIns;
  if (IB.m_bDoDebug)
    GCEnterFnt.Construct(IB);

  IB.Build(&GCEndFunct);
  if (IB.m_bDoDebug)
    ((pGCJmpIns)pIns)->m_pJmpPos = (pGCIns)(IB.m_pcIPtr + sizeof(pCPGMDbg));
  else
    ((pGCJmpIns)pIns)->m_pJmpPos = IB.m_pIPtr;
  return pIns;
  }

//===========================================================================

class GCStrFunctDef : public GCDef
  {
  public:
  GCStrFunctDef() : GCDef("StrFunction") { m_defFlags = (DefLHS | DefRetStr); };
  virtual pGCIns Construct(GCInsMngr & IB);
  virtual pGCVar ConstructVar(pGCVar &pFirstVar, GCVarMap *pVarMap, pchar pName, DWORD flags)
    { return new GCFunctVar(pFirstVar, pVarMap, pName, flags); };
  };

GCStrFunctDef GCStrFunct;

//---------------------------------------------------------------------------

pGCIns GCStrFunctDef::Construct(GCInsMngr & IB)
  {//NB: same as GCFunctDef
  pGCIns pIns = new(IB) GCJmpIns;
  IB.m_pH->m_bFunctRetFnd = 0;
  pGCIns pTempIns;
  if (IB.m_bDoDebug)
    pTempIns = (pGCIns)(IB.m_pcIPtr + sizeof(pCPGMDbg));
  else
    pTempIns = IB.m_pIPtr;
  pGCFunctVar pFunctVar;
  if (IB.m_pH->m_pClassVar)
    pFunctVar = (pGCFunctVar)IB.DoConstructFnt(IB.m_pH->m_pClassVar->m_pVarList, &IB.m_pH->m_pClassVar->m_VarMap);
  else
    pFunctVar = (pGCFunctVar)IB.DoConstructFnt(IB.m_pVarList, &IB.m_VarMap);
  pFunctVar->m_flags |= VarStrFunct; //NB
  pFunctVar->m_pFntIPos = pTempIns;
  if (IB.m_bDoDebug)
    GCEnterFnt.Construct(IB);

  IB.Build(&GCEndFunct);
  if (IB.m_bDoDebug)
    ((pGCJmpIns)pIns)->m_pJmpPos = (pGCIns)(IB.m_pcIPtr + sizeof(pCPGMDbg));
  else
    ((pGCJmpIns)pIns)->m_pJmpPos = IB.m_pIPtr;
  return pIns;
  }

//===========================================================================

START_CODE(RefArray);
  long index;
  pGCClassVar m_pClassVar;
  virtual pchar    GetDesc(pchar Buff);
MID_CODE(RefArray,"#RefArray#",DefExp | DefLHS,0);
  virtual pGCIns   Construct(GCInsMngr & IB);
END_CODE(RefArray);

//---------------------------------------------------------------------------

pGCIns GCRefArrayDef::Construct(GCInsMngr & IB)
  {
/*  pGCClassVar pClassVar = (pGCClassVar)IB.m_pH->m_pTempVar;
  IB.DoConstructParms(1, 0x0, true, true);
  pGCIns pIns = new(IB) GCRefArrayIns;
  ((pGCRefArrayIns)pIns)->m_pClassVar = pClassVar;
  ((pGCRefArrayIns)pIns)->index = -1;
  return pIns;*/
  return NULL;
  }

//---------------------------------------------------------------------------

void GCRefArrayIns::Exec(GCInsMngr & IB)
  {
/*  if (IB.m_FntStackPos >= MAX_FUNCT_STACK)
    IB.Err(ErrOtherRuntime, 2);
  if (m_pFntVar->m_pFntIPos)
    {
    if (IB.m_bDoDebug)
      {
      ASSERT(false); //TODO
      //IB.m_pcIPtr += sizeof(pCPGMDbg);
      Advance(IB);                         //look ahead
      IB.m_FntStack[++IB.m_FntStackPos] = (pGCIns)(IB.m_pcIPtr + sizeof(pCPGMDbg));
      IB.m_pIPtr = m_pFntVar->m_pFntIPos;
      IB.m_pcIPtr -= sizeof(pCPGMDbg);
      }
    else
      {
      Advance(IB);                         //look ahead
      IB.m_FntStack[++IB.m_FntStackPos] = IB.m_pIPtr;
      IB.m_pIPtr = m_pFntVar->m_pFntIPos;
      }
    }
  else
    {//MUST use Temp or-else CalcStack index gets confused !!!
    double Temp = IB.m_pClassRefStack[IB.m_iClassRefPos]->m_pRefClass->CallFunct(IB, IB.m_pClassRefStack[IB.m_iClassRefPos]->m_pSubClass, m_pFntVar->m_FunctId, IB.m_pClassRefStack[IB.m_iClassRefPos]);
    IB.m_dCalcStack[++IB.m_iCalcPos] = Temp;
    Advance(IB);
    }*/
  }

//---------------------------------------------------------------------------

pchar GCRefArrayIns::GetDesc(pchar Buff)
  {
  /*char Buff1[256];
  char Buff2[256];
  if (m_pFntVar->m_pFntIPos)
    sprintf(Buff, "%s-->(%s | %s)", GCIns::GetDesc(Buff1), m_pFntVar->Name(), m_pFntVar->m_pFntIPos->GetDesc(Buff2));
  else
    sprintf(Buff, "%s-->(%s)", GCIns::GetDesc(Buff1), m_pFntVar->Name());*/
  return Buff;
  }

//===========================================================================

START_CODE(CallFnt);
  GCFunctVar * m_pFntVar;
  virtual pchar    GetDesc(pchar Buff);
MID_CODE(CallFnt,"#CallFunct#",DefExp | DefLHS,0);
  virtual pGCIns   Construct(GCInsMngr & IB);
END_CODE(CallFnt);

//---------------------------------------------------------------------------

pGCIns GCCallFntDef::Construct(GCInsMngr & IB)
  {
  pGCFunctVar pFntVar = (pGCFunctVar)IB.m_pH->m_pTempVar;
  if (pFntVar->m_Parms==0 || pFntVar->m_dwStrParmMask || pFntVar->m_pVarList==NULL)
    IB.DoConstructParms(pFntVar->m_Parms, pFntVar->m_dwStrParmMask);
  else
    {
    pGCVar pVar = pFntVar->m_pVarList;
    for (int i=0; i<pFntVar->m_Parms; i++)
      {
      IB.DoConstructParms(1, (pVar->WhatAmI()==VarStr)?0x1:0x0, (i==0), (i==pFntVar->m_Parms-1));
      pVar = pVar->Next();
      }
    }
  pGCIns pIns = new(IB) GCCallFntIns;
  ((pGCCallFntIns)pIns)->m_pFntVar = pFntVar;
  return pIns;
  }

//---------------------------------------------------------------------------

void GCCallFntIns::Exec(GCInsMngr & IB)
  {
  if (IB.m_FntStackPos >= MAX_FUNCT_STACK)
    IB.Err(ErrOtherRuntime, 2);
  if (m_pFntVar->m_pFntIPos)
    {
    if (IB.m_bDoDebug)
      {
      //IB.m_pcIPtr += sizeof(pCPGMDbg);
      Advance(IB);                         //look ahead
      IB.m_FntStack[++IB.m_FntStackPos] = (pGCIns)(IB.m_pcIPtr + sizeof(pCPGMDbg));
      IB.m_pIPtr = m_pFntVar->m_pFntIPos;
      IB.m_pcIPtr -= sizeof(pCPGMDbg);
      }
    else
      {
      Advance(IB);                         //look ahead
      IB.m_FntStack[++IB.m_FntStackPos] = IB.m_pIPtr;
      IB.m_pIPtr = m_pFntVar->m_pFntIPos;
      }
    }
  else
    {//MUST use Temp or-else CalcStack index gets confused !!!
    GCClassVar * pClass = IB.m_pClassRefStack[IB.m_iClassRefPos];
    if ((m_pFntVar->m_flags & VarStrFunct)!=0)// m_bStrRet)
      {
      pClass->m_pRefClass->CallFunct(IB, pClass->m_pSubClass, m_pFntVar->m_FunctId, pClass);
      // The Function will push the String Value using PushStringValue(LPSTR Value);
      //IB.m_StrStack[++IB.m_iStrPos] = m_pFntVar->m_sStrRet;
      }
    else
      {
      double Temp = pClass->m_pRefClass->CallFunct(IB, pClass->m_pSubClass, m_pFntVar->m_FunctId, pClass);
      IB.m_dCalcStack[++IB.m_iCalcPos] = Temp;
      }
    Advance(IB);
    }
  }

//---------------------------------------------------------------------------

pchar GCCallFntIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  char Buff2[256];
  if (m_pFntVar->m_pFntIPos)
    sprintf(Buff, "%s-->(%s | %s)", GCIns::GetDesc(Buff1), m_pFntVar->Name(), m_pFntVar->m_pFntIPos->GetDesc(Buff2));
  else
    sprintf(Buff, "%s-->(%s)", GCIns::GetDesc(Buff1), m_pFntVar->Name());
  return Buff;
  }

//===========================================================================

START_CODE(Pop);
MID_CODE(Pop,"#Pop#",0,0);
virtual pGCIns   Construct(GCInsMngr & IB)
  {
  IB.m_pH->m_bBreakLine = 1;
  return new(IB) GCPopIns;
  }
END_CODE(Pop);

//---------------------------------------------------------------------------

void GCPopIns::Exec(GCInsMngr & IB)
  {
  --IB.m_iCalcPos;
  Advance(IB);                         //look ahead
  }

//===========================================================================

START_CODE(SetTag)
  GCTagVar*        m_pVar;     //points to instance of GCTagVar (external identifier)
  Strng            m_sCnvTxt;  //conversion text to be used
MID_CODE(SetTag,"SetTag",DefLHS,0)
virtual pGCIns Construct(GCInsMngr & IB);
END_CODE(SetTag)

//---------------------------------------------------------------------------

pGCIns GCSetTagDef::Construct(GCInsMngr & IB)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != '(')
    IB.Err(ErrBuild, 104);
  GCVar *pVar = IB.DoConstructTag('"');
  pVar->m_flags |= VarTagOut;
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != ',')
    IB.Err(ErrBuild, 106);
  IB.DoConstructParms(1, 0x0, False);
  pGCIns pIns = new(IB) GCSetTagIns;
  ((GCSetTagIns*)pIns)->m_pVar = (GCTagVar*)pVar;
  ((GCSetTagIns*)pIns)->m_sCnvTxt = IB.m_pH->m_sTempCnvTxt[IB.m_pH->m_iTempTagPos--];
  ((GCTagVar*)pVar)->SetRqdCnvTxt(((GCSetTagIns*)pIns)->m_sCnvTxt());
  return pIns;
  }

//---------------------------------------------------------------------------

void GCSetTagIns::Exec(GCInsMngr & IB)
  {
#if dbgGenCon
  if (dbgGetSets())
    dbgpln("SetTag %-35s %5i %-10s %15.6f", m_pVar->Name(), m_pVar->m_iCnvIndex, m_sCnvTxt(), IB.m_dCalcStack[IB.m_iCalcPos]);
#endif
  m_pVar->set(Cnvs[m_pVar->m_iCnvIndex]->Normal(IB.m_dCalcStack[IB.m_iCalcPos--], m_sCnvTxt()));
  Advance(IB);
  }

//===========================================================================

START_CODE(GetTag)
  GCTagVar*        m_pVar;     //points to instance of GCTagVar (external identifier)
  Strng            m_sCnvTxt;  //conversion text to be used
  //virtual ~GCGetTagIns();
MID_CODE(GetTag,"GetTag",DefExp,0)
virtual pGCIns Construct(GCInsMngr & IB);
END_CODE(GetTag)

//---------------------------------------------------------------------------

/*GCGetTagIns::~GCGetTagIns()
  {//call destructor for the Strng...
  }*/

//---------------------------------------------------------------------------

pGCIns GCGetTagDef::Construct(GCInsMngr & IB)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != '(')
    IB.Err(ErrBuild, 104);
  pGCVar pVar = IB.DoConstructTag('"');
  pVar->m_flags |= VarTagIn;
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != ')')
    IB.Err(ErrBuild, 107);
  pGCIns pIns = new(IB) GCGetTagIns;
  ((GCGetTagIns*)pIns)->m_pVar = (GCTagVar*)pVar;
  ((GCGetTagIns*)pIns)->m_sCnvTxt = IB.m_pH->m_sTempCnvTxt[IB.m_pH->m_iTempTagPos--];
  ((GCTagVar*)pVar)->SetRqdCnvTxt(((GCGetTagIns*)pIns)->m_sCnvTxt());
  return pIns;
  }

//---------------------------------------------------------------------------

void GCGetTagIns::Exec(GCInsMngr & IB)
  {
  if (IB.m_iCalcPos >= MAX_CALC_STACK)
    IB.Err(ErrOtherRuntime, 3);
  IB.m_dCalcStack[++IB.m_iCalcPos] = Cnvs[m_pVar->m_iCnvIndex]->Human(m_pVar->getD(), m_sCnvTxt());
#if dbgGenCon
  if (dbgGetSets())
    dbgpln("GetTag    %-35s %5i %-10s %15.6f", m_pVar->Name(), m_pVar->m_iCnvIndex, m_sCnvTxt(), IB.m_dCalcStack[IB.m_iCalcPos]);
#endif
  Advance(IB);
  }

//---------------------------------------------------------------------------
//===========================================================================

START_CODE(SetDynTag)
  int       m_iLineNo;     //line number for errors
MID_CODE(SetDynTag,"SetDynTag",DefLHS,0)
virtual pGCIns Construct(GCInsMngr & IB);
END_CODE(SetDynTag)

//---------------------------------------------------------------------------

pGCIns GCSetDynTagDef::Construct(GCInsMngr &IB)
  {
  IB.DoConstructParms(1, 0x1, True, False); //tag
  IB.DoConstructParms(1, 0x0, False); //value
  pGCIns pIns = new(IB) GCSetDynTagIns;
  ((GCSetDynTagIns*)pIns)->m_iLineNo = IB.m_pH->GetCurLineNo();
  return pIns;
  }

//---------------------------------------------------------------------------

void GCSetDynTagIns::Exec(GCInsMngr & IB)
  {
  Strng Tag, WrkTag, WrkCnvTxt;
  Tag = IB.m_StrStack[IB.m_iStrPos--];
  if (Tag.Length()==0)
    {
    IB.m_iCalcPos--;
    Strng s;
    s.Set("Line:%d", m_iLineNo);
    IB.Err(ErrOtherRuntime, 9, s());
    }
  else
    {
    TaggedObject::SplitTagCnv(Tag(), WrkTag, WrkCnvTxt);
    pGCVar pVar = IB.m_pVarList->FindTagByType(WrkTag(), VarTag);
    if (pVar == NULL)
      {//try find new dyn tag and get its value
      pVar = IB.TryAddDynTag(WrkTag(), false, false);
      }
    else
      {
      if ((pVar->m_flags & VarTagOut)==0)
        {
        pVar->m_flags |= VarTagOut; // Ensure that Direction flags set in case it is bidirectional
        IB.m_pXRM->RequestUpdateXRefLists();
        }
      }
    if (pVar == NULL)
      {
      IB.m_iCalcPos--;
      Strng s;
      s.Set("Line:%d  Unknown external tag: '%s'", m_iLineNo, WrkTag());
      IB.Err(ErrOtherRuntime, 7, s());
      }
    else
      {
      bool Settable=(!((GCTagVar*)pVar)->m_bOnlyAllowGet ||
          (((GCTagVar*)pVar)->m_bAllowSetAtInit && (IB.m_pOnInitialiseVar->getB() || IB.m_pOnTerminateVar->getB())));
      if (!Settable)
        {
        Strng s;
        s.Set("Line:%d  Tag %s cannot be set.", m_iLineNo, pVar->Name());
        IB.Err(ErrOtherRuntime, 13, s());
        }
      if (WrkCnvTxt.Length()>0)
        {//check engineering units...
        GCTagVar* pTagVar = (GCTagVar*)pVar;
        if (pTagVar->m_iCnvIndex==DC_)
          {
          Strng s;
          s.Set("Line:%d  %s has no defined engineering units.", m_iLineNo, pTagVar->Name());
          IB.Err(ErrOtherRuntime, 11, s());
          }
        else if (gs_CnvsMngr.FindSecCnv(pTagVar->m_iCnvIndex, WrkCnvTxt())==NULL)
          {
          Strng s;
          s.Set("Line:%d  Tag:%s  (%s) is not a valid unit for %s.", m_iLineNo, pTagVar->Name(), WrkCnvTxt(), Cnvs[pTagVar->m_iCnvIndex]->Fam());
          IB.Err(ErrOtherRuntime, 11, s());
          }
        pVar->set(Cnvs[pTagVar->m_iCnvIndex]->Normal(IB.m_dCalcStack[IB.m_iCalcPos--], WrkCnvTxt()));
#if dbgGenCon
        if (dbgGetSets())
          dbgpln("SetDynTag %-35s %5i %-10s %15.6f", pVar->Name(), pTagVar->m_iCnvIndex, WrkCnvTxt(), IB.m_dCalcStack[IB.m_iCalcPos]);
#endif
        }
      else
        pVar->set(IB.m_dCalcStack[IB.m_iCalcPos--]);
      }
    }
  Advance(IB);
  }

//===========================================================================

START_CODE(GetDynTag)
  int       m_iLineNo;     //line number for errors
MID_CODE(GetDynTag,"GetDynTag",DefExp,0)
virtual pGCIns Construct(GCInsMngr & IB);
END_CODE(GetDynTag)

//---------------------------------------------------------------------------

pGCIns GCGetDynTagDef::Construct(GCInsMngr & IB)
  {
  IB.DoConstructParms(1, 0x1);
  pGCIns pIns = new(IB) GCGetDynTagIns;
  ((GCGetDynTagIns*)pIns)->m_iLineNo = IB.m_pH->GetCurLineNo();
  return pIns;
  }

//---------------------------------------------------------------------------

void GCGetDynTagIns::Exec(GCInsMngr & IB)
  {
  if (IB.m_iCalcPos >= MAX_CALC_STACK)
    IB.Err(ErrOtherRuntime, 3);
  Strng Tag, WrkTag, WrkCnvTxt;
  Tag = IB.m_StrStack[IB.m_iStrPos--];
  if (Tag.Length()==0)
    {
    IB.m_dCalcStack[++IB.m_iCalcPos] = 0.0;
    Strng s;
    s.Set("Line:%d", m_iLineNo);
    IB.Err(ErrOtherRuntime, 10, s());
    }
  else
    {
    TaggedObject::SplitTagCnv(Tag(), WrkTag, WrkCnvTxt);
    pGCVar pVar = IB.m_pVarList->FindTagByType(WrkTag(), VarTag);
    if (pVar == NULL)
      {//try find new dyn tag and get its value
      pVar = IB.TryAddDynTag(WrkTag(), true, false);
      }
    else
      {
      if ((pVar->m_flags & VarTagIn)==0)
        {
        pVar->m_flags |= VarTagIn; // Ensure that Direction flags set in case it is bidirectional
        IB.m_pXRM->RequestUpdateXRefLists();
        }
      }
    if (pVar == NULL)
      {
      IB.m_dCalcStack[++IB.m_iCalcPos] = 0.0;
      Strng s;
      s.Set("Line:%d  Unknown external tag: '%s'", m_iLineNo, WrkTag());
      IB.Err(ErrOtherRuntime, 8, s());
      }
    else
      {
      if (WrkCnvTxt.Length()>0)
        {//check engineering units...
        GCTagVar* pTagVar = (GCTagVar*)pVar;
        if (pTagVar->m_iCnvIndex==DC_)
          {
          Strng s;
          s.Set("%s has no defined engineering units.  Line:%d", pTagVar->Name(), m_iLineNo);
          IB.Err(ErrOtherRuntime, 12, s());
          }
        else if (gs_CnvsMngr.FindSecCnv(pTagVar->m_iCnvIndex, WrkCnvTxt())==NULL)
          {
          Strng s;
          s.Set("Tag:%s  (%s) is not a valid unit for %s.  Line:%d", pTagVar->Name(), WrkCnvTxt(), Cnvs[pTagVar->m_iCnvIndex]->Fam(), m_iLineNo);
          IB.Err(ErrOtherRuntime, 12, s());
          }
        IB.m_dCalcStack[++IB.m_iCalcPos] = Cnvs[pTagVar->m_iCnvIndex]->Human(pVar->getD(), WrkCnvTxt());
#if dbgGenCon
        if (dbgGetSets())
          dbgpln("GetDynTag %-35s %5i %-10s %15.6f", pVar->Name(), pTagVar->m_iCnvIndex, WrkCnvTxt(), IB.m_dCalcStack[IB.m_iCalcPos]);
#endif
        }
      else
        {
        IB.m_dCalcStack[++IB.m_iCalcPos] = pVar->getD();
#if dbgGenCon
        if (dbgGetSets())
          dbgpln("GetDynTag %-35s %5i %-10s %15.6f", pVar->Name(), -1, "", IB.m_dCalcStack[IB.m_iCalcPos]);
#endif
        }
      }
    }
  Advance(IB);
  }

//===========================================================================

START_CODE(SetDynStrTag)
  int       m_iLineNo;     //line number for errors
MID_CODE(SetDynStrTag,"SetDynStrTag",DefLHS,0)
virtual pGCIns Construct(GCInsMngr & IB);
END_CODE(SetDynStrTag)

//---------------------------------------------------------------------------

pGCIns GCSetDynStrTagDef::Construct(GCInsMngr &IB)
  {
  IB.DoConstructParms(2, 0xff); //tag,value
  pGCIns pIns = new(IB) GCSetDynStrTagIns;
  ((GCSetDynStrTagIns*)pIns)->m_iLineNo = IB.m_pH->GetCurLineNo();
  return pIns;
  }

//---------------------------------------------------------------------------

void GCSetDynStrTagIns::Exec(GCInsMngr & IB)
  {
  Strng Tag, WrkTag, WrkCnvTxt, sVal;
  sVal = IB.m_StrStack[IB.m_iStrPos--];
  Tag = IB.m_StrStack[IB.m_iStrPos--];
  if (Tag.Length()==0)
    {
    Strng s;
    s.Set("Line:%d", m_iLineNo);
    IB.Err(ErrOtherRuntime, 9, s());
    }
  else
    {
    TaggedObject::SplitTagCnv(Tag(), WrkTag, WrkCnvTxt);
    pGCVar pVar = IB.m_pVarList->FindTagByType(WrkTag(), VarTag);
    if (pVar == NULL)
      {//try find new dyn tag and get its value
      pVar = IB.TryAddDynTag(WrkTag(), false, true);
      }
    else
      {
      if ((pVar->m_flags & VarTagOut)==0)
        {
        pVar->m_flags |= VarTagOut; // Ensure that Direction flags set in case it is bidirectional
        IB.m_pXRM->RequestUpdateXRefLists();
        }
      }
    if (pVar == NULL)
      {
      Strng s;
      s.Set("Line:%d  Unknown external tag: '%s'", m_iLineNo, WrkTag());
      IB.Err(ErrOtherRuntime, 7, s());
      }
    else
      {
      bool Settable=(!((GCTagVar*)pVar)->m_bOnlyAllowGet ||
          (((GCTagVar*)pVar)->m_bAllowSetAtInit && (IB.m_pOnInitialiseVar->getB() || IB.m_pOnTerminateVar->getB())));
      if (!Settable)
        {
        Strng s;
        s.Set("Line:%d  Tag %s cannot be set.", m_iLineNo, pVar->Name());
        IB.Err(ErrOtherRuntime, 13, s());
        }
      ASSERT(pVar->WhatAmI()==VarTag);
      GCTagVar* pTagVar=((GCTagVar*)pVar);
      if (pTagVar->m_bStrTag)
        {
        ((GCStrTagVar*)pVar)->set(sVal() ? sVal() : "");
        }
      else
        {
        Strng s;
        s.Set("Line:%d  Tag is not a string: '%s'", m_iLineNo, WrkTag());
        IB.Err(ErrOtherRuntime, 14, s());
        }
      }
    }
  Advance(IB);
  }

//===========================================================================

START_CODE(GetDynStrTag)
  int       m_iLineNo;     //line number for errors
MID_CODE(GetDynStrTag,"GetDynStrTag",DefExp | DefRetStr,0)
virtual pGCIns Construct(GCInsMngr & IB);
END_CODE(GetDynStrTag)

//---------------------------------------------------------------------------

pGCIns GCGetDynStrTagDef::Construct(GCInsMngr & IB)
  {
  IB.DoConstructParms(1, 0xff);
  pGCIns pIns = new(IB) GCGetDynStrTagIns;
  ((GCGetDynStrTagIns*)pIns)->m_iLineNo = IB.m_pH->GetCurLineNo();
  return pIns;
  }

//---------------------------------------------------------------------------

void GCGetDynStrTagIns::Exec(GCInsMngr & IB)
  {
  if (IB.m_iStrPos >= MAX_STR_STACK)
    IB.Err(ErrOtherRuntime, 3);
  Strng Tag, WrkTag, WrkCnvTxt;
  Tag = IB.m_StrStack[IB.m_iStrPos--];
  if (Tag.Length()==0)
    {
    IB.m_StrStack[++IB.m_iStrPos] = "";
    Strng s;
    s.Set("Line:%d", m_iLineNo);
    IB.Err(ErrOtherRuntime, 10, s());
    }
  else
    {
    TaggedObject::SplitTagCnv(Tag(), WrkTag, WrkCnvTxt);
    pGCVar pVar = IB.m_pVarList->FindTagByType(WrkTag(), VarTag);
    if (pVar == NULL)
      {//try find new dyn tag and get its value
      pVar = IB.TryAddDynTag(WrkTag(), true, true);
      }
    else
      {
      if ((pVar->m_flags & VarTagIn)==0)
        {
        pVar->m_flags |= VarTagIn; // Ensure that Direction flags set in case it is bidirectional
        IB.m_pXRM->RequestUpdateXRefLists();
        }
      }
    if (pVar == NULL)
      {
      IB.m_StrStack[++IB.m_iStrPos] = "";
      Strng s;
      s.Set("Line:%d  Unknown external tag: '%s'", m_iLineNo, WrkTag());
      IB.Err(ErrOtherRuntime, 8, s());
      }
    else
      {
      ASSERT(pVar->WhatAmI()==VarTag);
      GCTagVar* pTagVar=((GCTagVar*)pVar);
      if (pTagVar->m_bStrTag && IsStrng(pTagVar->m_pXRef->m_iType))
        {
        IB.m_StrStack[++IB.m_iStrPos] = ((GCStrTagVar*)pVar)->getS();
        }
      else
        {
        IB.m_StrStack[++IB.m_iStrPos] = "";
        Strng s;
        s.Set("Line:%d  Tag is not a string: '%s'", m_iLineNo, WrkTag());
        IB.Err(ErrOtherRuntime, 15, s());
        }
      }
    }
  Advance(IB);
  }

//===========================================================================

START_CODE(UpdateTagSubscriptions)
MID_CODE(UpdateTagSubscriptions,"UpdateTagSubscriptions",DefLHS,0)
virtual pGCIns Construct(GCInsMngr & IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCUpdateTagSubscriptionsIns;
  };
END_CODE(UpdateTagSubscriptions)

//---------------------------------------------------------------------------

void GCUpdateTagSubscriptionsIns::Exec(GCInsMngr & IB)
  {
  // Return the same value
  gs_Exec.XUpdateTagData((long)IB.m_dCalcStack[IB.m_iCalcPos]);
  Advance(IB);
  }

//===========================================================================

START_CODE(CnvVal)
  CCnvIndex     m_iCnvIndex;    //Cnv family used
  Strng         m_sCnvTxt1;     //conversion text of supplied number
  Strng         m_sCnvTxt2;     //conversion text of result
  virtual pchar GetDesc(pchar Buff);
MID_CODE(CnvVal,"CnvVal",DefExp,0)
virtual pGCIns Construct(GCInsMngr & IB);
END_CODE(CnvVal)

//---------------------------------------------------------------------------

pGCIns GCCnvValDef::Construct(GCInsMngr & IB)
  {
  IB.DoConstructParms(1, 0x0, True, False);
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  CCnvIndex CnvIndex = gs_CnvsMngr.FindCnv(Tkn);
  if ((CnvIndex)<0)
    IB.Err(ErrBuild, 146);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != ',')
    IB.Err(ErrBuild, 106);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  CDataCnv* pCnv1 = Cnvs[CnvIndex]->Find(Tkn);
  if (pCnv1==NULL)
    {
    char ErrBuff[256];
    sprintf(ErrBuff, "Conversion family : '%s'", Cnvs[CnvIndex]->Fam());
    IB.Err(ErrBuild, 147, ErrBuff);
    }
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != ',')
    IB.Err(ErrBuild, 106);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  CDataCnv* pCnv2 = Cnvs[CnvIndex]->Find(Tkn);
  if (pCnv2==NULL)
    {
    char ErrBuff[256];
    sprintf(ErrBuff, "Conversion family : '%s'", Cnvs[CnvIndex]->Fam());
    IB.Err(ErrBuild, 147, ErrBuff);
    }
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != ')')
    IB.Err(ErrBuild, 107);
  pGCIns pIns = new(IB) GCCnvValIns;
  ((GCCnvValIns*)pIns)->m_iCnvIndex = CnvIndex;
  ((GCCnvValIns*)pIns)->m_sCnvTxt1 = pCnv1->Txt();
  ((GCCnvValIns*)pIns)->m_sCnvTxt2 = pCnv2->Txt();
  return pIns;
  }

//---------------------------------------------------------------------------

void GCCnvValIns::Exec(GCInsMngr & IB)
  {
  IB.m_dCalcStack[IB.m_iCalcPos] = Cnvs[m_iCnvIndex]->Normal(IB.m_dCalcStack[IB.m_iCalcPos], m_sCnvTxt1());
  IB.m_dCalcStack[IB.m_iCalcPos] = Cnvs[m_iCnvIndex]->Human(IB.m_dCalcStack[IB.m_iCalcPos], m_sCnvTxt2());
  Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCCnvValIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  sprintf(Buff, "%s(x,%s,%s,%s)", GCIns::GetDesc(Buff1), Cnvs[m_iCnvIndex]->Fam(), m_sCnvTxt1(), m_sCnvTxt2());
  return Buff;
  }

//===========================================================================

START_CODE(Cnv)
  pGCVar        m_pVar;        //variable supplied with conversion info
  Strng         m_sCnvTxt;     //conversion text of result
  virtual pchar GetDesc(pchar Buff);
MID_CODE(Cnv,"Cnv",DefExp,0)
virtual pGCIns Construct(GCInsMngr & IB);
END_CODE(Cnv)

//---------------------------------------------------------------------------

pGCIns GCCnvDef::Construct(GCInsMngr & IB)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != '(')
    IB.Err(ErrBuild, 104);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  pGCDef pDef = GCDef::Find(Tkn);
  if (pDef)
    IB.Err(ErrBuild, 121);
  pGCVar pVar = IB.m_pVarList->Find(Tkn);
  if (pVar == NULL)
    IB.Err(ErrBuild, 121);
  if (pVar->m_pCnvInfo==NULL)
    IB.Err(ErrBuild, 150);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != ',')
    IB.Err(ErrBuild, 106);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  CDataCnv* pCnv = Cnvs[pVar->m_pCnvInfo->Index()]->Find(Tkn);
  if (pCnv==NULL)
    {
    char ErrBuff[256];
    sprintf(ErrBuff, "Conversion family : '%s'", Cnvs[pVar->m_pCnvInfo->Index()]->Fam());
    IB.Err(ErrBuild, 147, ErrBuff);
    }
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != ')')
    IB.Err(ErrBuild, 107);
  pGCIns pIns = new(IB) GCCnvIns;
  ((GCCnvIns*)pIns)->m_pVar = pVar;
  ((GCCnvIns*)pIns)->m_sCnvTxt = pCnv->Txt();
  return pIns;
  }

//---------------------------------------------------------------------------

void GCCnvIns::Exec(GCInsMngr & IB)
  {
  IB.m_dCalcStack[++IB.m_iCalcPos] = m_pVar->m_pCnvInfo->Cnv(m_pVar->getD(), m_sCnvTxt());
  Advance(IB);
  }

//---------------------------------------------------------------------------

pchar GCCnvIns::GetDesc(pchar Buff)
  {
  char Buff1[256];
  sprintf(Buff, "%s(%s,%s)", GCIns::GetDesc(Buff1), m_pVar->Name(), m_sCnvTxt());
  return Buff;
  }

//===========================================================================

class GCCnvInfoDef : public GCDef
  {
  public:
  GCCnvInfoDef() : GCDef("CnvInfo")
    { m_defFlags = DefLHS; };
  virtual pGCIns Construct(GCInsMngr & IB);
  };

GCCnvInfoDef GCCnvInfo;

//---------------------------------------------------------------------------

pGCIns GCCnvInfoDef::Construct(GCInsMngr & IB)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0]!='(')
    IB.Err(ErrBuild, 104);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  pGCDef pDef = GCDef::Find(Tkn);
  if (pDef)
    IB.Err(ErrBuild, 121);
  pGCVar pVar = IB.m_pVarList->Find(Tkn);
  if (pVar == NULL)
    IB.Err(ErrBuild, 121);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != ',')
    IB.Err(ErrBuild, 106);
  IB.DoConstructVarCnv(pVar);
  return NULL;
  }

//---------------------------------------------------------------------------
//========================= GCVar xxx methods ===============================
//---------------------------------------------------------------------------

GCVar::GCVar( pGCVar &pFirstVar,
              GCVarMap * pVarMap,
              pchar pName, 
              DWORD flags)
  {
  m_flags = flags;
  m_pName = new char[strlen(pName) + 1];
  strcpy(m_pName, pName);
  m_pNext = NULL;
  m_pWatchInfo = NULL;
  m_pCnvInfo = NULL;

  if (pFirstVar)
    {
    pGCVar pVar = pFirstVar;
    while (pVar->m_pNext)
      pVar = pVar->m_pNext;
    pVar->m_pNext = this;
    }
  else
    pFirstVar = this;
  m_pVarMap=pVarMap;
  if (m_pVarMap)
    m_pVarMap->SetAt(m_pName, this);
  }

//---------------------------------------------------------------------------

GCVar::~GCVar()
  {
  if (m_pVarMap)
    m_pVarMap->RemoveKey(m_pName);
  delete []m_pName;
  if (m_pWatchInfo)
    delete m_pWatchInfo;
  if (m_pCnvInfo)
    delete m_pCnvInfo;
  }

//---------------------------------------------------------------------------

pchar GCVar::GetDesc(pchar Buff, flag Short)
  {
  sprintf(Buff, "[%c]%s=", WhatAmI(), Name());
  double d = getD();
  if (Valid(d))
    {
    if (Short)
      sprintf(Buff, "%s%.5f", Buff, d);
    else
      sprintf(Buff, "%s%.12g", Buff, d);
    }
  else
    strcat(Buff, "*");
  return Buff;
  }

//---------------------------------------------------------------------------

pGCVar GCVar::Find(pchar pName)
  {
  pGCVar pVar = this;
  pchar pDot = strchr(pName, '.');
  size_t n = pDot ? pDot-pName : strlen(pName);
  while (pVar && (strlen(pVar->m_pName)!=n || _strnicmp(pName,pVar->m_pName,n) != 0))
    pVar = pVar->m_pNext;
  if (pVar)
    {
    if (pDot && pVar->WhatAmI() != VarClass)
      return NULL;
    return (pDot && pVar->WhatAmI()==VarClass) ? ((pGCClassVar)pVar)->m_pVarList->Find(pDot+1) : pVar;
    }
  return NULL;
  }

//---------------------------------------------------------------------------

pGCVar GCVar::Find(pchar pName, short &Index)
  {
  pGCVar pVar = this;
  Index = 0;
  if (strchr(pName, '.'))
    {
    pchar pDot = strchr(pName, '.');
    size_t n = pDot-pName;
    while (pVar && (strlen(pVar->m_pName)!=n || _strnicmp(pName,pVar->m_pName,n) != 0))
      {
      Index++;
      pVar = pVar->m_pNext;
      }
    if (pVar)
      {
      if (pVar->WhatAmI() != VarClass)
        pVar = NULL;
      else
        pVar = ((pGCClassVar)pVar)->m_pVarList->Find(pDot+1, Index);
      }
    }
  else
    {
    size_t n = strlen(pName);
    while (pVar && (strlen(pVar->m_pName)!=n || _strnicmp(pName,pVar->m_pName,n) != 0))
      {
      Index++;
      pVar = pVar->m_pNext;
      }
    }
  if (pVar==NULL)
    Index = -1;
  return pVar;
  }

//---------------------------------------------------------------------------

pGCVar GCVar::Find(short Index)
  {
  pGCVar pVar = this;
  short i = 0;
  while (pVar && i<Index)
    {
    i++;
    pVar = pVar->m_pNext;
    }
  return pVar;
  }

//---------------------------------------------------------------------------

pGCVar GCVar::FindTag(pchar pName)
  {
  pGCVar pVar = this;
  size_t n = strlen(pName);
  while (pVar && (strlen(pVar->m_pName)!=n || _strnicmp(pName,pVar->m_pName,n) != 0))
    pVar = pVar->m_pNext;
  return pVar;
  }

//---------------------------------------------------------------------------

pGCVar GCVar::FindTagByType(pchar pName, char TagType)
  {
  //CNM/KGA the VarList should also be mapped
  pGCVar pVar = this;
#if WITHVARMAP 
  if (m_pVarMap->Lookup(pName, pVar) && pVar->WhatAmI()==TagType)
    return pVar;
#endif
  //try the length limited
  size_t n = strlen(pName);
  while (pVar && (pVar->WhatAmI()!=TagType || strlen(pVar->m_pName)!=n || _strnicmp(pName,pVar->m_pName,n) != 0))
    pVar = pVar->m_pNext;
  return pVar;
  }

//---------------------------------------------------------------------------

pGCClassVar GCVar::FindClass(pchar pName)
  {
  pGCVar pVar = this;
  pchar pDot = strchr(pName, '.');
  size_t n = pDot ? pDot-pName : strlen(pName);
  while (pVar && (strlen(pVar->m_pName)!=n || _strnicmp(pName,pVar->m_pName,n) != 0))
    pVar = pVar->m_pNext;
  if (pVar)
    {
    if (pVar->WhatAmI()==VarClass && !(pVar->m_flags & VarClassDefn))
      {
      if (strchr(pDot+1, '.'))
        return ((pGCClassVar)pVar)->m_pVarList->FindClass(pDot+1);
      return (pGCClassVar)pVar;
      }
    }
  return NULL;
  }

//---------------------------------------------------------------------------

pGCClassVar GCVar::FindClass(pchar pName, short& ClassIndex)
  {
  pGCVar pVar = this;
  pchar pDot = strchr(pName, '.');
  size_t n = pDot ? pDot-pName : strlen(pName);
  ClassIndex = 0;
  while (pVar && (strlen(pVar->m_pName)!=n || _strnicmp(pName,pVar->m_pName,n) != 0))
    {
    pVar = pVar->m_pNext;
    ClassIndex++;
    }
  if (pVar)
    {
    if (pVar->WhatAmI()==VarClass && !(pVar->m_flags & VarClassDefn))
      {
      if (strchr(pDot+1, '.'))
        return ((pGCClassVar)pVar)->m_pVarList->FindClass(pDot+1);
      return (pGCClassVar)pVar;
      }
    }
  ClassIndex = -1;
  return NULL;
  }

//===========================================================================

pchar GCStrVar::GetDesc(pchar Buff, flag Short)
  {
  if (m_var.Length()>0)
    sprintf(Buff, "[%c]%s=\"%s\"", WhatAmI(), Name(), m_var());
  else
    sprintf(Buff, "[%c]%s=\"\"", WhatAmI(), Name());
  return Buff;
  }

//===========================================================================

pchar GCLabelVar::GetDesc(pchar Buff, flag Short)
  {
  if (m_var.Length()>0)
    sprintf(Buff, "[%c]%s=\"%s\"", WhatAmI(), Name(), m_var());
  else
    sprintf(Buff, "[%c]%s=\"\"", WhatAmI(), Name());
  return Buff;
  }

//===========================================================================

GCFunctVar::~GCFunctVar()
  {
  //destruct all vars in the function...
  m_VarMap.RemoveAll();
  pGCVar pVar = m_pVarList;
  while (pVar)
    {
    pGCVar Nextp = pVar->Next();
    delete pVar;
    pVar = Nextp;
    }
  m_pVarList = NULL;

  }

//===========================================================================

GCClassVar::GCClassVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags, pGCClassVar pRefClass)
  : GCVar(pFirstVar, pVarMap, pName, flags)
  {
  m_pSubClass = NULL;
  m_pVarList = NULL;
  m_VarMap.InitHashTable(51);
  m_pRefClass = pRefClass;
  m_pParentClass = NULL;
  m_pRefClass->m_flags |= VarClassUsed;
  CopyClass();
  }

//---------------------------------------------------------------------------

GCClassVar::~GCClassVar()
  {
  //destruct all vars in the class...
  m_VarMap.RemoveAll();
  pGCVar pVar = m_pVarList;
  while (pVar)
    {
    pGCVar Nextp = pVar->Next();
    delete pVar;
    pVar = Nextp;
    }
  m_pVarList = NULL;
  ASSERT(m_pSubClass==NULL); //should have allready been deleted
  }

//---------------------------------------------------------------------------

void GCClassVar::CopyClass()
  {//makes a set of vars for the PGM instance of the class, ie makes a copy based on the reference class
  int TxtLblCnt = 0;
  pGCVar pVar = m_pRefClass->m_pVarList;
  while (pVar)
    {
    pGCVar pV = NULL;
    switch (pVar->WhatAmI())
      {
      case VarDouble:
        pV = GCDouble.ConstructVar(m_pVarList, &m_VarMap, pVar->Name(), 0);
        break;
      case VarLong:
        pV = GCLong.ConstructVar(m_pVarList, &m_VarMap, pVar->Name(), 0);
        break;
      case VarByte:
        pV = GCByte.ConstructVar(m_pVarList, &m_VarMap, pVar->Name(), 0);
        break;
      case VarBit:
        pV = GCBit.ConstructVar(m_pVarList, &m_VarMap, pVar->Name(), 0);
        break;
      case VarStr:
        pV = GCStr.ConstructVar(m_pVarList, &m_VarMap, pVar->Name(), 0);
        break;
      case VarFunct:
        {
        pGCFunctVar pFntVar = (pGCFunctVar)GCFunct.ConstructVar(m_pVarList, &m_VarMap, pVar->Name(), 0);
        pFntVar->m_Parms = ((pGCFunctVar)pVar)->m_Parms;
        pFntVar->m_pFntIPos = ((pGCFunctVar)pVar)->m_pFntIPos;
        pFntVar->m_pVarList = NULL;
        pFntVar->m_VarMap.InitHashTable(51);
        pGCVar pTmpVar = ((pGCFunctVar)pVar)->m_pVarList;
        while (pTmpVar)
          {
          switch (pTmpVar->WhatAmI())
            {
            case VarDouble:
              GCDouble.ConstructVar(pFntVar->m_pVarList, &pFntVar->m_VarMap, pTmpVar->Name(), 0);
              break;
            case VarLong:
              GCLong.ConstructVar(pFntVar->m_pVarList, &pFntVar->m_VarMap, pTmpVar->Name(), 0);
              break;
            case VarByte:
              GCByte.ConstructVar(pFntVar->m_pVarList, &pFntVar->m_VarMap, pTmpVar->Name(), 0);
              break;
            case VarBit:
              GCBit.ConstructVar(pFntVar->m_pVarList, &pFntVar->m_VarMap, pTmpVar->Name(), 0);
              break;
            case VarStr:
              GCStr.ConstructVar(pFntVar->m_pVarList, &pFntVar->m_VarMap, pTmpVar->Name(), 0);
              break;
            default:
              VERIFY(FALSE);
              break;
            }
          pTmpVar = pTmpVar->Next();
          }
        //pFntVar->m_pVarList = ((pGCFunctVar)pVar)->m_pVarList; //this caused a MAJOR subtle bug for functions in class instances !!!
        pFntVar->m_FunctId = ((pGCFunctVar)pVar)->m_FunctId;
        pFntVar->m_dwStrParmMask = ((pGCFunctVar)pVar)->m_dwStrParmMask;
        pFntVar->m_flags = pVar->m_flags;
        break;
        }
      case VarClass:
        {
        pGCClassVar pClassVar = (pGCClassVar)GCClass.ConstructVar(m_pVarList, &m_VarMap, pVar->Name(), 0);
        pClassVar->m_pRefClass = ((pGCClassVar)pVar)->m_pRefClass;
        pClassVar->m_pVarList = NULL;
        pClassVar->m_VarMap.InitHashTable(51);
        pClassVar->m_pSubClass = NULL;
        pClassVar->m_pParentClass = this;
        pClassVar->CopyClass();
        break;
        }
      case VarLabel:
        {
        char Buff[512];
        char Lbl[32];
        sprintf(Lbl, "%s_%d", pVar->Name(), TxtLblCnt++);
        GCVar* pLabel = new GCLabelVar(m_pVarList, &m_VarMap, Lbl, VarPgmWatch, pVar->GetStrVal(Buff));
        break;
        }
      case VarArray:
        {
        ASSERT_RDB(FALSE, "Not implemented!", __FILE__, __LINE__);
        break;
        }
      default:
        ASSERT(FALSE);
        break;
      }
    if (pV)
      {
      if (pVar->ReadOnly())
        pV->m_flags |= VarReadOnly;
      if (pVar->Watched())
        pV->m_flags |= VarPgmWatch;
      if (pVar->m_pCnvInfo)
        pV->AddCnvInfo(pVar->m_pCnvInfo);
      }
    pVar = pVar->Next();
    }
  m_pRefClass->Init(this); //initialises sub class
  }

//---------------------------------------------------------------------------

void GCClassVar::DestroySubClass()
  {
  //destruct all sub classes contained in the classes var list...
  pGCVar pVar = m_pVarList;
  while (pVar)
    {
    if (pVar->WhatAmI()==VarClass)
      ((pGCClassVar)pVar)->DestroySubClass();
    pVar = pVar->Next();
    }

  if (m_pSubClass)
    {
    m_pRefClass->Done(this); //destructs sub class
    m_pSubClass = NULL;
    }
  }

//---------------------------------------------------------------------------

void GCClassVar::SetArrayData(int index0, int index1, double d)
  {
  if (m_pSubClass==NULL || m_pRefClass==NULL)
    return;

  GCArray* pA = dynamic_cast<GCArray*>(m_pRefClass);
  if (pA)
    {
    CDVector* pV = (CDVector*)m_pSubClass;
    if (pV)
      {
      if (index0<pV->GetLen())
        pV->m_d[index0] = d;
      }
    return;
    }

  GCMatrix* pM = dynamic_cast<GCMatrix*>(m_pRefClass);
  if (pM)
    {
    CDMatrix* pMt = (CDMatrix*)m_pSubClass;
    if (pMt)
      {
      if (index0<pMt->GetRows() && index1<pMt->GetCols())
        pMt->SetAt(index0, index1, d);
      }
    return;
    }
  }

//---------------------------------------------------------------------------

double GCClassVar::GetArrayData(int index0, int index1)
  {
  if (m_pSubClass==NULL || m_pRefClass==NULL)
    return dNAN;

  double d = dNAN;
  GCArray* pA = dynamic_cast<GCArray*>(m_pRefClass);
  if (pA)
    {
    CDVector* pV = (CDVector*)m_pSubClass;
    if (pV)
      {
      if (index0<pV->GetLen())
        d = pV->m_d[index0];
      }
    return d;
    }

  GCMatrix* pM = dynamic_cast<GCMatrix*>(m_pRefClass);
  if (pM)
    {
    CDMatrix* pMt = (CDMatrix*)m_pSubClass;
    if (pMt)
      {
      if (index0<pMt->GetRows() && index1<pMt->GetCols())
        d = pMt->GetAt(index0, index1);
      }
    return d;
    }
  return d;
  }

//---------------------------------------------------------------------------

char* GCClassVar::GetFullInstanceName(Strng & s)
  {
  s = Name();
  if (m_pParentClass)
    {
    char Buff[256];
    pGCClassVar pClassVar = m_pParentClass;
    while (pClassVar)
      {
      sprintf(Buff, "%s.%s", pClassVar->Name(), s());
      s = Buff;
      pClassVar = pClassVar->m_pParentClass;
      }
    }
  return s();
  }

//---------------------------------------------------------------------------

pGCVar GCClassVar::GetVarByName(pchar pName)
  {
  if (m_pVarList == NULL)
    return NULL;
  return m_pVarList->Find(pName);
  }

//---------------------------------------------------------------------------

pGCVar GCClassVar::GetVarByIndex(int index)
  {
  ASSERT((index>=0) && (m_pVarList != NULL));
  int i = 0;
  pGCVar pVar = m_pVarList;
  while (i<index)
    {
    pVar = pVar->Next();
    i++;
    ASSERT(pVar != NULL);
    };
  return pVar;
  }

//---------------------------------------------------------------------------

pGCVar GCClassVar::AddVar( GCInsMngr & IB,
                           pchar pName,
                           pGCDef pDef,
                           DWORD flags)
  {
  pGCVar pVar = GetVarByName(pName);
  if (pVar)
    IB.Err(ErrBuild, 102);
  pVar = pDef->ConstructVar(m_pVarList, &m_VarMap, pName, 0);
  return pVar;
  }

//---------------------------------------------------------------------------

pGCVar GCClassVar::AddFunct( GCInsMngr & IB,
                             pchar pName,
                             short TtlParmCount,
                             dword StrParmMask,
                             short FunctId,
                             flag StrRet)
  {
  pGCVar pVar = GetVarByName(pName);
  if (pVar)
    IB.Err(ErrBuild, 102);
  pGCFunctVar pFunctVar = (pGCFunctVar)GCFunct.ConstructVar(m_pVarList, &m_VarMap, pName, StrRet ? VarStrFunct:0);
  pFunctVar->m_Parms = TtlParmCount;
  pFunctVar->m_dwStrParmMask = StrParmMask;
  pFunctVar->m_FunctId = FunctId;
  return (pGCVar)pFunctVar;
  }

//---------------------------------------------------------------------------

pchar GCClassVar::GetDesc(pchar Buff, flag Short)
  {
  sprintf(Buff, "[%c]%s=...", WhatAmI(), Name());
  return Buff;
  }

//---------------------------------------------------------------------------

int GCClassVar::GetVarCount()
  {
  int i = 0;
  pGCVar pVar = m_pVarList;
  while (pVar)
    {
    pVar = pVar->Next();
    i++;
    }
  return i;
  }

//---------------------------------------------------------------------------

void GCClassVar::SaveVal(FilingControlBlock &FCB,
                        pchar pValName,
                        double d)
  {
  GCFCBAppendRec(FCB, 'd', pValName, &d, sizeof(d));
  }

//---------------------------------------------------------------------------

void GCClassVar::SaveVal(FilingControlBlock &FCB,
                        pchar pValName,
                        CTimeValue d)
  {
  __debugbreak();
  GCFCBAppendRec(FCB, 't', pValName, &d, sizeof(d));
  }

//---------------------------------------------------------------------------

void GCClassVar::SaveVal(FilingControlBlock &FCB,
                        pchar pValName,
                        int i)
  {
  GCFCBAppendRec(FCB, 'i', pValName, &i, sizeof(i));
  }

//---------------------------------------------------------------------------

void GCClassVar::SaveVal(FilingControlBlock &FCB,
                        pchar pValName,
                        long i)
  {
  GCFCBAppendRec(FCB, 'l', pValName, &i, sizeof(i));
  }

//---------------------------------------------------------------------------

void GCClassVar::SaveVal(FilingControlBlock &FCB,
                        pchar pValName,
                        flag b)
  {
  GCFCBAppendRec(FCB, 'b', pValName, &b, sizeof(b));
  }

//---------------------------------------------------------------------------

void GCClassVar::SaveVal(FilingControlBlock &FCB,
                         pchar pValName,
                         pchar s)
  {
  if (s)
    GCFCBAppendRec(FCB, 's', pValName, s, strlen(s)+1);
  else
    {
    char Buff[1];
    Buff[0] = 0;
    GCFCBAppendRec(FCB, 's', pValName, Buff, 1);
    }
  }

//===========================================================================

GCArrayVar::GCArrayVar(pGCVar &pFirstVar, GCVarMap * pVarMap, pchar pName, DWORD flags, GCClassVar* pClassRefVar, int ArrayLen) : GCVar(pFirstVar, pVarMap, pName, flags)
  {
  m_pClassRefVar = pClassRefVar;
  m_iArrayLen = ArrayLen;
  m_pVars.SetSize(m_iArrayLen);
  }

//---------------------------------------------------------------------------

GCArrayVar::~GCArrayVar()
  {
  }

//===========================================================================

GCArrayWatch::GCArrayWatch(pGCClassVar pClassVar, bool ReadOnly, int index0, int index1/*=-1*/)
  {
  m_pClassVar = pClassVar;
  m_iIndex0 = index0;
  m_iIndex1 = index1;
  m_bReadOnly = ReadOnly;
  Strng s;
  if (m_iIndex1<0)
    m_sTg.Set("%s[%i]", m_pClassVar->GetFullInstanceName(s), m_iIndex0);
  else
    m_sTg.Set("%s[%i][%i]",m_pClassVar->GetFullInstanceName(s), m_iIndex0, m_iIndex1);
  }

//---------------------------------------------------------------------------

GCArrayWatch::~GCArrayWatch()
  {
  }
//---------------------------------------------------------------------------

double GCArrayWatch::getD()
  {
  return m_pClassVar->GetArrayData(m_iIndex0, m_iIndex1);
  }

//---------------------------------------------------------------------------

void GCArrayWatch::set(double d)
  {
  m_pClassVar->SetArrayData(m_iIndex0, m_iIndex1, d);
  }

//============================ GCLoadHelp ====================================
//info used by GCInsMngr while loading

GCLoadHelp::GCLoadHelp()
  {
  m_bEqualFlag   = 0;
  m_bExpectStr   = 0;
  m_bDoingNext   = 0;
  m_bPrevLHS     = 0;
  m_bBreakLine   = 0;
  m_bEOF         = 0;
  m_bFunctRetFnd = 0;
  m_iPushPopClassDepth = 0;
  m_pFunctVar    = NULL;
  m_pClassVar    = NULL;
  m_pTknParser   = NULL;
  m_iWhilePos    = -1;
  m_iTempTagPos  = -1;
  m_iTxtLblCnt   = 0;
  m_iPgLblCnt    = 0;
  }

//---------------------------------------------------------------------------

int GCLoadHelp::GetCurLineNo()
  {
  if (m_pTknParser==NULL)
    return -1;
  if (m_pTknParser->ParsingBuffer())
    return m_pTknParser->GetLineNo();
  if (m_pTknParser->GetFileIndex())
    return m_pTknParser->GetFileLineNo(m_pTknParser->GetFileIndex());
  return m_pTknParser->GetFileLineNo();
  }

//---------------------------------------------------------------------------

void GCLoadHelp::AddToTagStack(GCInsMngr & IB, pGCVar pVar, char* pCnvTxt)
  {
  m_iTempTagPos++;
  if (m_iTempTagPos>=MaxTagStack)
    IB.Err(ErrBuild, 144);
  m_pTempTagVars[m_iTempTagPos] = pVar;
  m_sTempCnvTxt[m_iTempTagPos] = pCnvTxt;
  }

//===========================================================================

class CTmpFileListHelper
  {
  public:
    CStringArray FileNames;
    int Add(char* pFileName) { return FileNames.Add(pFileName); };
    ~CTmpFileListHelper()
      {//destructor deletes any tmp files that may still remain...
      for (int i=0; i<FileNames.GetSize(); i++)
        DeleteFile((const char*)(FileNames[i]));
      }
  };

CTmpFileListHelper TmpFileList;

//============================ GCInsMngr ====================================
//---------------------------------------------------------------------------

GCInsMngr::GCInsMngr(LPCTSTR pFnExt)
  {
  m_sFnExtn        = pFnExt?pFnExt:"pgm";
  //m_pFlwNode       = pFlwNode;
  m_pH             = NULL;
  m_bDoDebug       = 0;
  m_bSetDbgOn      = 0;
  m_bLogBuildErrs  = 1;
  m_bLogAllBuildErrs = 0;
  m_iRunTimeErr    = 0;
  m_iMathRunTimeErr= 0;
  m_bDidSaveVars   = 0;
  m_iStrPos        = -1;
  m_iCalcPos       = -1;
  m_iClassRefPos   = -1;
  m_FntStackPos    = -1;
  m_pFirstBlk      = NULL;
  m_pCurrentBlk    = NULL;
  m_pIPtr          = NULL;
  m_FntStackPos    = -1;
  m_pVarList       = NULL;
  m_VarMap.InitHashTable(501);
  m_pXRM           = NULL;
  m_pOnInitialiseVar = NULL;
  m_pOnStartVar    = NULL;
  m_pRunMode       = NULL;
  m_pHeatMode      = NULL;
  m_pOnLoadedVar   = NULL;
  m_pOnEmptyVar    = NULL;
  m_pOnPresetVar   = NULL;
  m_pModelTagVar   = NULL;
  m_pStopVar       = NULL;
  m_pPauseVar      = NULL;
  for (int i=0; i<MaxCondMsgs; i++)
    {
    m_CINOn[i] = 0;
    m_CIEOn[i] = 0;
    }

  m_pTermStripPtrs=NULL;
  
  m_ProcExecuted = false;

  //ConstructConstants();
  //srand( (unsigned)time( NULL ) );
  }

//---------------------------------------------------------------------------

GCInsMngr::~GCInsMngr()
  {
  //delete m_pNearGCXRefs;
  //delete m_pFarGCXRefs;

  DestroyCode();
  DestroyVars(m_pVarList);
  if (m_bDidSaveVars)
    {
    DeleteFile(m_sTempSaveFile());
    DeleteFile(m_sTempSaveOtherFile());
    }
  }

//---------------------------------------------------------------------------

void GCInsMngr::DestroyCode()
  {//destruct all instructions (code)...
  pchar pThisBlk = m_pFirstBlk;
  m_pcIPtr       = m_pFirstBlk;
  while (m_pIPtr)
    {
    if (m_bDoDebug)
      {
      pCPGMDbg pDbg = *((CPGMDbg**)m_pcIPtr);
      delete pDbg;
      m_pcIPtr += sizeof(pCPGMDbg);
      }
    if (m_pIPtr->Code()==&GCNxtBlk ||
        m_pIPtr->Code()==&GCEOF ||
        m_pIPtr->Code()==&GCEOF2)
      {
      m_pIPtr->Advance(*this);
      delete []pThisBlk;
      pThisBlk = m_pcIPtr;
      }
    else
      {
      pGCIns pPrevIns = m_pIPtr;
      m_pIPtr->Advance(*this);
      delete pPrevIns; //allthough the GCIns class itself is not destroyed, the destructors for all the objects it contains MUST be called
      }
    }
  m_pFirstBlk = NULL;
  }

//---------------------------------------------------------------------------

void GCInsMngr::DestroyCodeFrom(pGCIns pIPtr, char* OldCurBlk)
  {//destruct instructions (code) from specified point...
  pchar pThisBlk = OldCurBlk;
  pGCIns pOldIPtr = m_pIPtr;
  m_pIPtr = pIPtr;
  while (m_pIPtr)
    {
    if (m_bDoDebug)
      {
      pCPGMDbg pDbg = *((CPGMDbg**)m_pcIPtr);
      delete pDbg;
      m_pcIPtr += sizeof(pCPGMDbg);
      }
    if (m_pIPtr->Code()==&GCNxtBlk ||
        m_pIPtr->Code()==&GCEOF ||
        m_pIPtr->Code()==&GCEOF2)
      {
      m_pIPtr->Advance(*this);
      if (OldCurBlk!=pThisBlk)
        delete []pThisBlk;
      pThisBlk = m_pcIPtr;
      }
    else
      {
      pGCIns pPrevIns = m_pIPtr;
      m_pIPtr->Advance(*this);
      delete pPrevIns; //allthough the GCIns class itself is not destroyed, the destructors for all the objects it contains MUST be called
      }
    }
  m_pIPtr = pOldIPtr;
  m_pCurrentBlk = OldCurBlk;
  if (OldCurBlk==NULL)
    {
    delete []m_pFirstBlk;
    m_pFirstBlk = NULL;
    }
  else
    {
    const int CurBlkSize = (m_pCurrentBlk==m_pFirstBlk ? m_iFirstInsBlkSize : m_iNextInsBlkSize);
    memset((char*)pIPtr, 0, CurBlkSize - (int)((char*)pIPtr-m_pCurrentBlk));
    }
  }

//---------------------------------------------------------------------------

void GCInsMngr::DestroyVars(pGCVar& pVarList)
  {//destruct all PGM variables...
  UnlinkAllXRefs();

  //destroy all array watch variables
  int PrevSz = m_ExtraWatchArray.GetSize();
  for (int i=0; i<m_ExtraWatchArray.GetSize(); i++)
    delete m_ExtraWatchArray.GetAt(i);
  m_ExtraWatchArray.SetSize(0, PrevSz>0 ? PrevSz : -1);

  //MUST first destroy all c++ sub-class instances... (needs PGM class defn var to destroy it)
  pGCVar pVar = pVarList;
  while (pVar)
    {
    if (pVar->WhatAmI()==VarClass)
      ((pGCClassVar)pVar)->DestroySubClass();
    pVar = pVar->Next();
    }

  //destroy PGM linked list of vars...
  pVar = pVarList;
  while (pVar)
    {
    pGCVar Nextp = pVar->Next();
    delete pVar;
    pVar = Nextp;
    }
  pVarList = NULL;

  }

//---------------------------------------------------------------------------

void GCInsMngr::PushStringValue(LPSTR Value)
  {
  m_StrStack[++m_iStrPos] = Value;
  };

//---------------------------------------------------------------------------

void GCInsMngr::UnlinkAllXRefs()
  {
  // this must not be called because the XRefs are owned etc by GCTagVar's

  pGCVar pVar = m_pVarList;
  while (pVar)
    {
    if (pVar->WhatAmI()==VarTag)
      {
      GCTagVar * p= (GCTagVar*)pVar;
// NOT Owned by ME      delete p->m_pXRef;
      p->m_pXRef=NULL;
      }
    pVar = pVar->Next();
    }

  //m_pXRM->UnlinkAllXRefs();
  }

//---------------------------------------------------------------------------

void GCInsMngr::DestroyTempDynTagVars()
  {//this tidy up of temporary dynamic tag vars probably not really needed
  //ASSERT(m_bXRefsValid==0);
  // CNM ????
  return;


  bool DidDelete = false;
  pGCVar pVarPrev = NULL;
  pGCVar pVar = m_pVarList;
  while (pVar)
    {
    if (pVarPrev &&
        pVar->WhatAmI()==VarTag &&
        (pVar->m_flags & VarDynTag) &&
        ((GCTagVar*)pVar)->m_bTempDynTag)
      {
      DidDelete = true;
      pVarPrev->m_pNext = pVar->Next();
      delete pVar;
      pVar = pVarPrev->Next();
      }
    else
      {
      pVarPrev = pVar;
      pVar = pVar->Next();
      }
    }
  if (DidDelete)
    UnlinkAllXRefs();
  }

//---------------------------------------------------------------------------

void GCInsMngr::ConstructConstants()
  {
  //Hard Coded Constants -----
  //new GCLongVar(m_pVarList, "PGM_SIZE", 0, 0);
  new GCDoubleVar(m_pVarList, &m_VarMap, "PI", VarConst, 3.141592653589793238462643);
  new GCDoubleVar(m_pVarList, &m_VarMap, "NAN", VarConst, dNAN);
  new GCDoubleVar(m_pVarList, &m_VarMap, "LargeResistance", VarConst, LargeElecRes);
  new GCBitVar(m_pVarList, &m_VarMap, "TRUE", VarConst, bGCtrue);
  new GCBitVar(m_pVarList, &m_VarMap, "FALSE", VarConst, bGCfalse);
  m_pOnInitialiseVar = new GCBitVar(m_pVarList, &m_VarMap, "OnInitialise", VarConst, bGCfalse);
  m_pOnStartVar = new GCBitVar(m_pVarList, &m_VarMap, "OnStart", VarConst, bGCfalse);
  m_pOnTerminateVar = new GCBitVar(m_pVarList, &m_VarMap, "OnTerminate", VarConst, bGCfalse);
  m_pRunMode = new GCLongVar/*ByteVar*/(m_pVarList, &m_VarMap, "RunMode", VarConst, 0);
  m_pHeatMode = new GCLongVar/*ByteVar*/(m_pVarList, &m_VarMap, "HeatMode", VarConst, 0);
  m_pOnLoadedVar = new GCBitVar(m_pVarList, &m_VarMap, "OnLoaded", VarConst, bGCfalse);
  m_pOnEmptyVar = new GCBitVar(m_pVarList, &m_VarMap, "OnEmpty", VarConst, bGCfalse);
  m_pOnPresetVar = new GCBitVar(m_pVarList, &m_VarMap, "OnPreset", VarConst, bGCfalse);
  m_pModelTagVar = new GCStrVar(m_pVarList, &m_VarMap, "ModelTag", VarConst, "?");
  m_pStopVar = new GCBitVar(m_pVarList, &m_VarMap, "StopSimulation", 0, 0);
  m_pPauseVar = new GCBitVar(m_pVarList, &m_VarMap, "PauseSimulation", 0, 0);
  //Hard coded classes -----
  DefinePGMClasses(*this);

  //this should be done better...
  if (!GCDef::m_bDoneDefFlags)
    {//set flags for all defs
    GCDef::m_bDoneDefFlags = True;
    GCGetTag.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//GetTag
    GCSetTag.m_defFlags |= (DefSolverSet | DefGenConSet);//SetTag
    GCFunct.m_defFlags |= (DefSolverSet | DefGenConSet);//Function
    GCStrFunct.m_defFlags |= (DefSolverSet | DefGenConSet);//StrFunction
    GCEndFunct.m_defFlags |= (DefSolverSet | DefGenConSet);//EndFunct
    GCReturn.m_defFlags |= (DefSolverSet | DefGenConSet);//return
    GCWatch.m_defFlags |= (DefGenConSet);//Watch
    GCExcludeWatch.m_defFlags |= (DefGenConSet);//ExcludeWatch
    GCWatchInfo.m_defFlags |= (DefGenConSet);//WatchInfo
    GCTextLabel.m_defFlags |= (DefGenConSet);//TextLabel
    GCPageLabel.m_defFlags |= (DefGenConSet);//PageLabel
    GCClass.m_defFlags |= (DefSolverSet | DefGenConSet);//Class
    GCEndClass.m_defFlags |= (DefSolverSet | DefGenConSet);//EndClass
    GCTag.m_defFlags |= (DefSolverSet | DefGenConSet);//[
    GCConst.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//const numeric
    GCConstStr.m_defFlags |= (DefSolverSet | DefGenConSet);//const string
    GCConstVar.m_defFlags |= (DefSolverSet | DefGenConSet);//const
    GCEqual.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//=
    GCEqualTag.m_defFlags |= (DefSolverSet | DefGenConSet);//=
    GCEqualClassRef.m_defFlags |= (DefSolverSet | DefGenConSet);//=
    GCOBrace.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//(
    GCEOF.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//$
    GCEOF2.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//EndFile
    GCIf.m_defFlags |= (DefSolverSet | DefGenConSet);//if
    GCElseif.m_defFlags |= (DefSolverSet | DefGenConSet);//elseif
    GCElse.m_defFlags |= (DefSolverSet | DefGenConSet);//else
    GCEndif.m_defFlags |= (DefSolverSet | DefGenConSet);//endif
    GCIif.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//iif
    GCBegin.m_defFlags |= (DefGenConSet);//begin
    GCEnd.m_defFlags |= (DefGenConSet);//end
    GCWhile.m_defFlags |= (DefGenConSet);//while
    GCEndWhile.m_defFlags |= (DefGenConSet);//endwhile
    GCDef::Find("Not")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//NOT
    GCDef::Find("Or")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//OR
    GCDef::Find("Xor")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//XOR
    GCDef::Find("And")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//AND
    GCDef::Find("Bor")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//BOR
    GCDef::Find("Bxor")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//BXOR
    GCDef::Find("Band")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//BAND
    GCDef::Find("Bnot")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//BNOT
    GCDef::Find("/")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);///
    GCDef::Find("*")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//*
    GCDef::Find("-")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//-
    GCDef::Find("+")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//+
    GCDef::Find("^")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);//^
    GCCmpGreat.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCCmpLessEqual.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCCmpLess.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCCmpGreatEqual.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCCmpNotEqual.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCCmpEqual.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCStr.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCBit.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCByte.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCLong.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDouble.m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("]")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find(")")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find(",")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("LogNote")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("LogError")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("ConditionNote")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("ConditionError")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("LogEvent")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("MsgBox")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("FltStr")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("IntStr")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("FltToStr")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("IntToStr")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("StrToFlt")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("StrToInt")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("AlphaToNum")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("IsAlpha")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("STRLWR")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("STRUPR")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("STRLEN")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("MID")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("RIGHT")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("LEFT")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("STRICMP")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("STRCMP")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("STRSTR")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("STRCAT")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("STRCAT2")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("STRCAT3")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("STRCAT4")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("STRCAT5")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("BEEP")->m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDef::Find("DELTATIME")->m_defFlags |= (DefGenConSet);
    GCDef::Find("TIME")->m_defFlags |= (DefGenConSet | DefFunctSet);
    //GCDef::Find("AN_SCALE")->m_defFlags |= (DefGenConSet);
    GCDef::Find("ISNAN")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("RANDOM")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("ROUND")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("ROUNDUP")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("TRUNC")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("RANGE")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("MIN")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("MAX")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("LOG")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("LN")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("EXP")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("SQRT")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("ABS")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("POW")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("ATAN2")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("TAN")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("COS")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("SIN")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("DIV")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCDef::Find("MOD")->m_defFlags |= (DefSolverSet | DefGenConSet | DefFunctSet);
    GCVarRef.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCVarTagRef.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCVarClassRef.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCLbl.m_defFlags |= (DefSolverSet);
    GCCallFnt.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCDynamicTags.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCSetDynTag.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCGetDynTag.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCSetDynStrTag.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCGetDynStrTag.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCUpdateTagSubscriptions.m_defFlags |= (DefGenConSet);
    GCCnvVal.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCCnv.m_defFlags |= (DefSolverSet | DefGenConSet);
    GCCnvInfo.m_defFlags |= (DefGenConSet);
    }
  }

//---------------------------------------------------------------------------

pGCVar GCInsMngr::FindVar(pchar Tkn, short &VarIndex, VarLocation &eVarLoc)
  {
  VarIndex = -1;
  pGCVar pVar = NULL;
  if (m_pH->m_pFunctVar)
    {
    pVar = m_pH->m_pFunctVar->m_pVarList->Find(Tkn); //look in function scope
    eVarLoc = VL_Funct;
    }
  if (pVar==NULL && m_pH->m_pClassVar)
    {
    pVar = m_pH->m_pClassVar->m_pVarList->Find(Tkn, VarIndex); //look in class scope
    eVarLoc = VL_Class;
    }
  if (pVar==NULL)
    {
    pVar = m_pVarList->Find(Tkn); //look in global scope
    eVarLoc = VL_Global;
    }
  if (pVar==NULL)
    {
    pVar = m_pVarList->FindTag(Tkn); //look in global scope for a tag
    eVarLoc = VL_GlobalTag;
    }
  if (pVar==NULL)
    eVarLoc = VL_NotFound;
  return pVar;
  }

//---------------------------------------------------------------------------

pGCVar GCInsMngr::TryAddDynTag(char* Tag, bool ForGet, bool IsString)
  {
  pGCVar pDummyVarList = NULL;
  GCVarMap * pDummyVarMap= NULL;
  pGCVar pVar = (IsString ? GCStrTag.ConstructVar(pDummyVarList, pDummyVarMap, Tag, 0) : GCTag.ConstructVar(pDummyVarList, pDummyVarMap, Tag, 0));
  //pVar->m_flags |= ForGet ? VarTagIn : VarTagOut;//(VarTagIn | VarTagOut); //don't know if it is in or out so set both
  //kga 12/12/2002 : line above doesn't work if there is a getdyntag and setdyntag for the same variable in the same pgm

  //pVar->m_flags |= (VarTagIn | VarTagOut); //don't know if it is in or out so set both

  pVar->m_flags |= ForGet ? VarTagIn : VarTagOut;//(VarTagIn | VarTagOut); //don't know if it is in or out so set both

  GCTagVar*pTagVar=((GCTagVar*)pVar);
  //CGCXRefItem &XRef=((GCTagVar*)pVar)->m_XRef;
  pVar->m_flags |= VarDynTag;
  pTagVar->m_bTempDynTag = 1;

  int RetCode = UpdateXRef(pVar);//, pXRef);
  if ((RetCode & FXR_Found) || (RetCode & FXR_DynTag))// && (pVar->m_flags & VarTagFar)==0)
    {
    //add var to var list...
    if (m_pVarList)
      {
      pGCVar pV = m_pVarList;
      while (pV->m_pNext)
        pV = pV->m_pNext;
      pV->m_pNext = pVar;
      }
    else
      m_pVarList = pVar;
    pVar->m_pVarMap=&m_VarMap;
    m_VarMap.SetAt(pVar->m_pName, pVar);

    //dbgpln("TryAddDynTag : RequestUpdateXRefLists %s", Tag);

    //force rebuild of ExtRef subscriptions next execute
    m_pXRM->RequestUpdateXRefLists();

    //retrieve current value
    if ((pVar->m_flags & VarTagNear)!=0)
      pTagVar->m_pXRef->GetNearXRefValue();
    }
  else
    {
    delete pVar;
    pVar = NULL;
    }
  return pVar;
  }

//---------------------------------------------------------------------------

void GCInsMngr::SaveVars()
  {
  if (!m_bDidSaveVars)
    {
    BOOL Err = FALSE;
    char File[MAX_PATH];
    //CNM if (GetTempFileName(PrjFiles(), m_sFnExtn(), 0, File))
    if (GetTempFileName(TemporaryFiles(), m_sFnExtn(), 0, File))
      {
      m_sTempSaveFile = File;
      //CNM if (GetTempFileName(PrjFiles(), m_sFnExtn(), 0, File))
      if (GetTempFileName(TemporaryFiles(), m_sFnExtn(), 0, File))
        {
        m_bDidSaveVars = 1;
        m_sTempSaveOtherFile = File;
        TmpFileList.Add(m_sTempSaveFile());
        TmpFileList.Add(m_sTempSaveOtherFile());
        }
      else
        Err = TRUE;
      }
    else
      Err = TRUE;
    if (Err)
      {
      LogError(m_pXRM ? m_pXRM->GetOwnerTag() : m_sFnExtn(), 0, "Unable to create temporary file.");
      return;
      }
    }
  CProfINIFile PF(m_sTempSaveFile());
  DoSaveVars(PF, m_pVarList, "");
  #if dbgGenCon
  if (dbgLoad())
    {
    dbgpln("--------- Vars before reload -------------...");
    DbgVarList();
    }
  #endif
  FilingControlBlock FCB(PrjFiles(), DefaultCfgLabel, TOF_DBase);
  HANDLE H = CreateFile(m_sTempSaveOtherFile(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
  if (H!=INVALID_HANDLE_VALUE)
    {
    FCB.InitialiseForWrite(H, 0);
    SaveScenario(FCB);
    CloseHandle(H);
    }
  }

//---------------------------------------------------------------------------

void GCInsMngr::DoSaveVars(CProfINIFile& PF, pGCVar pVar, char* pClassName)
  {
  while (pVar)
    {
    if (pVar->WhatAmI()==VarClass)
      {
      if ((pVar->m_flags & VarClassDefn)==0)
        {
        pGCClassVar pClassVar = (pGCClassVar)pVar;
        Strng s;
        if (strlen(pClassName)==0)
          s = pVar->Name();
        else
          s.Set("%s.%s", pClassName, pVar->Name());
        DoSaveVars(PF, pClassVar->m_pVarList, s());
        }
      }
    else
      {
      if (((pVar->m_flags & VarConst)==0) && (pVar->WhatAmI()!=VarFunct) && (pVar->WhatAmI()!=VarTag))
        {
        char Buff[512];
        Strng s;
        if (strlen(pClassName)==0)
          s = pVar->Name();
        else
          s.Set("%s.%s", pClassName, pVar->Name());
        Strng Section(pVar->WhatAmI());
        switch (pVar->WhatAmI())
          {
          case VarDouble : PF.WrDouble(Section(), s(), pVar->getD()); break;
          case VarLong   : PF.WrLong(Section(), s(), pVar->getL()); break;
          case VarByte   : PF.WrInt(Section(), s(), pVar->getC()); break;
          case VarBit    : PF.WrInt(Section(), s(), pVar->getB()); break;
          case VarStr    : PF.WrStr(Section(), s(), pVar->GetStrVal(Buff)); break;
          default: ASSERT(FALSE);
          }
        }
      }
    pVar = pVar->Next();
    }
  }

//---------------------------------------------------------------------------

void GCInsMngr::RestoreVars()
  {
  ASSERT(m_bDidSaveVars);
  CProfINIFile PF(m_sTempSaveFile());
  DoRestoreVars(PF, m_pVarList, "");
  #if dbgGenCon
  if (dbgLoad())
    {
    dbgpln("--------- Vars after reload -------------...");
    DbgVarList();
    }
  #endif
  FilingControlBlock FCB(PrjFiles(), DefaultCfgLabel, TOF_DBase);
  HANDLE H = CreateFile(m_sTempSaveOtherFile(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
  if (H!=INVALID_HANDLE_VALUE)
    {
    DWORD dwSize = GetFileSize(H, NULL);
    if (dwSize>0 && dwSize!=0xFFFFFFFF)
      {
      FCB.InitialiseForRead(H, 0, dwSize);
      //SetFilePointer(H, 0, NULL, FILE_BEGIN);
      LoadScenario(FCB);
      }
    CloseHandle(H);
    }
  }

//---------------------------------------------------------------------------

void GCInsMngr::DoRestoreVars(CProfINIFile& PF, pGCVar pVar, char* pClassName)
  {
  while (pVar)
    {
    if (pVar->WhatAmI()==VarClass)
      {
      if ((pVar->m_flags & VarClassDefn)==0)
        {
        pGCClassVar pClassVar = (pGCClassVar)pVar;
        Strng s;
        if (strlen(pClassName)==0)
          s = pVar->Name();
        else
          s.Set("%s.%s", pClassName, pVar->Name());
        DoRestoreVars(PF, pClassVar->m_pVarList, s());
        }
      }
    else
      {
      if (((pVar->m_flags & VarConst)==0) && (pVar->WhatAmI()!=VarFunct) && (pVar->WhatAmI()!=VarTag))
        {
        Strng s;
        if (strlen(pClassName)==0)
          s = pVar->Name();
        else
          s.Set("%s.%s", pClassName, pVar->Name());
        Strng Section(pVar->WhatAmI());
        switch (pVar->WhatAmI())
          {
          case VarDouble : pVar->set(PF.RdDouble(Section(), s(), pVar->getD())); break;
          case VarLong   : pVar->set(PF.RdLong(Section(), s(), pVar->getL())); break;
          case VarByte   : pVar->setC((char)PF.RdInt(Section(), s(), pVar->getC())); break;
          case VarBit    : pVar->set((flag)(PF.RdInt(Section(), s(), pVar->getB())!=0)); break;
          case VarStr    :
            {
            char Buff[512];
            Strng ss;
            ss = PF.RdStr(Section(), s(), pVar->GetStrVal(Buff));
            ((GCStrVar*)pVar)->set(ss());
            break;
            }
          default: ASSERT(FALSE);
          }
        }
      }
    pVar = pVar->Next();
    }
  }

//---------------------------------------------------------------------------

void PGMParseFailure(char* pTxt, DWORD ErrNo, void* p)
  {
  pGCInsMngr pInsMngr = (pGCInsMngr)p;
  switch (ErrNo)
    {
    case 1: LogError(pInsMngr->m_pXRM->GetOwnerTag(), 0, "Include file not specified."); break;
    case 2: LogError(pInsMngr->m_pXRM->GetOwnerTag(), 0, "Cannot open include file '%s'.", pTxt); break;
    case 3: LogError(pInsMngr->m_pXRM->GetOwnerTag(), 0, "Too many nested include files '%s'.", pTxt); break;
    }
  pInsMngr->Err(ErrBuild, 135);
  }

//---------------------------------------------------------------------------

bool fileExists(const std::string& fileName)
{
  std::fstream fin;
  fin.open(fileName.c_str(),std::ios::in);
  if( fin.is_open() )
  {
    fin.close();
    return true;
  }
  fin.close();
  return false;
}


flag GCInsMngr::BuildCode(byte Places, char* fullFileName, WORD InsSet, char* ParseBuff, flag AddingCode/*=False*/, int FirstInsBlkSize/*=4096*/, int NextInsBlkSize/*=2048*/)
  {
  ASSERT(GCDef::m_pFirst != NULL);
  ASSERT(!(fullFileName && ParseBuff));//cannot specify both!
  m_iFirstInsBlkSize = Max(FirstInsBlkSize, 128);//256);
  m_iNextInsBlkSize = Max(NextInsBlkSize, 128);//256);
  m_iRunTimeErr = 0;
  m_iMathRunTimeErr = 0;
  if (m_bSetDbgOn)
    {
    m_bSetDbgOn = 0;
    m_bDoDebug = 1;
    }
  m_FilesUsed.SetSize(0);
  m_FilesTime.SetSize(0);
  m_DbgMngr.sPgmName = "";
  m_StateLine[0] = "OK";
  m_StateLine[1] = "";
  m_StateLine[2] = "";
  m_StateLine[3] = "";
  m_StateLine[4] = "";

  m_TermStripDefns.SetSize(0); //

  if ((fullFileName==NULL || strlen(fullFileName)==0) && ParseBuff==NULL)
    {
    m_StateLine[0] = "No File";
    return true;// CNM False;
    }
  m_pH = new GCLoadHelp;
  m_pH->m_iInsSet = InsSet;
  flag Loaded = True;

  #if dbgGenCon
  if (dbgLoad())
    {
    dbgpln("--------------------------------------------------------------------\nAll PGM Symbol definitions:");
    pGCDef ptr = GCDef::m_pFirst;
    while (ptr != NULL)
      {
      dbgpln("  %s", ptr->Name());
      ptr = ptr->Next();
      }
    dbgpln("--------------------------------------------------------------------");
    }
  #endif

  char Fn[512];
  strcpy(Fn, "");
  if (ParseBuff)
    {
    m_DbgMngr.sPgmName = "Buffer";
    m_pH->m_pTknParser = new CTokenParser(True, True);
    m_pH->m_pTknParser->SetFarBuffer(ParseBuff);
    m_pH->m_pTknParser->SetLineEnd("\n"); // otherwise netline returns entire buffer.
    }
  else
    {
    if (strpbrk(fullFileName, ":\\")==NULL)
      strcpy(Fn, PrjFiles());
    strcat(Fn, fullFileName);

    Strng realFn,EncryptedFn;
    realFn.FnExpand(Fn);
    EncryptedFn = realFn;
    EncryptedFn += ".x";

    if (fileExists(EncryptedFn()))
      {//FIRST look for encrypted file
      LogNote(m_pXRM ? m_pXRM->GetOwnerTag() : "???", 0, "Load:%s", EncryptedFn());
      std::fstream fin;
      fin.open(EncryptedFn(), std::ios::in|std::ios::binary);
      if (fin.is_open())
        {
        fin.seekg(0, std::ios_base::end);
        int length = fin.tellg();
        fin.seekg(0, std::ios_base::beg);

        ParseBuff = new char[length+1];
        fin.read(ParseBuff, length);
        fin.close();

        int count = fin.gcount();
        ParseBuff[count] = 71;

        for (int i=count-1; i>=0; i--) // decrypt the buffer.
          {
          ParseBuff[i] ^= ParseBuff[i+1];
          }

        ParseBuff[count] = 0;
        m_DbgMngr.sPgmName = "Buffer";
        m_pH->m_pTknParser = new CTokenParser(True, True);
        m_pH->m_pTknParser->SetFarBuffer(ParseBuff);
        m_pH->m_pTknParser->SetLineEnd("\n"); // otherwise netline returns entire buffer.
        }
      else
        {
        // What error response should go here?
        m_StateLine[0] = "Error opening encrypted file";
        return true;//False;
        }
      }
    else
      {//assume normal pgm
      m_DbgMngr.sPgmName = Fn;
      while (m_DbgMngr.sPgmName.Find('\\')>=0)
        m_DbgMngr.sPgmName = m_DbgMngr.sPgmName.Mid(m_DbgMngr.sPgmName.Find('\\') + 1, 1024);
      m_pH->m_pTknParser = new CTokenParser(Places, Fn, True, True, True);
      }
    }

  m_pH->m_pTknParser->SetSeperators("+-*/^;,= \t()<>!\"[]{}$#@\v\f");
  m_pH->m_pTknParser->SetWhiteSpace(" \t\v\f");
  m_pH->m_pTknParser->SetFailFn(&PGMParseFailure, (void*)this);

  int LastLine = 0;
  if (m_pH->m_pTknParser->Start())
    {
    char* OldCurBlk = m_pCurrentBlk;
    pGCIns OldEndIns = m_pEndIns;
    TRY
      {
      m_pH->m_bBreakLine = 1;
      Build(&GCEOF);
      }
    CATCH(GCException, thePGMException)
      {
      if (AddingCode)
        {//return PGM to previous state...
        DestroyCodeFrom(OldEndIns, OldCurBlk);
        m_pIPtr = OldEndIns;
        if (m_pIPtr)
          GCEOF.Construct(*this); //replace EOF ins that was overwritten !!!
        }
      else
        Reset();
      Loaded = False;
      dbgpln("** File Reset Exeption During PGM Compile **");
      }
    END_CATCH
    LastLine = m_pH->m_pTknParser->GetLineNo();
    m_pH->m_pTknParser->End();
    }
  else
    {
    Loaded = False;
    m_StateLine[0].Set("NO PROGRAM");
    if (ParseBuff==NULL && strlen(Fn)>0)
      {
      LogError("GenCon", 0, "%s file not opened.", Fn);
      m_StateLine[1].Set("%s file not opened.", Fn);
      }
    }

  if (ParseBuff==NULL)
    {
    m_FilesUsed.SetSize(m_pH->m_pTknParser->GetFilesUsedCount());
    m_FilesTime.SetSize(m_pH->m_pTknParser->GetFilesUsedCount());
    for (int i=0; i<m_pH->m_pTknParser->GetFilesUsedCount(); i++)
      {
      m_FilesUsed[i] = m_pH->m_pTknParser->GetIncFileName(i);
      m_FilesTime[i] = m_pH->m_pTknParser->GetIncFileTime(i);
      }
    }
  delete m_pH->m_pTknParser;
  m_pH->m_pTknParser = NULL;
  if (Loaded)
    {
    if (!m_pH->m_bEOF)
      GCEOF.Construct(*this);
    if (m_bDoDebug)
      {//setup pgm debugger...
      if (ParseBuff)
        {
        m_pH->m_pTknParser = new CTokenParser(True, True);
        m_pH->m_pTknParser->SetFarBuffer(ParseBuff);
        }
      else
        m_pH->m_pTknParser = new CTokenParser(Places, Fn, True, True, True);
      m_pH->m_pTknParser->SetSeperators("+-*/^;,= \t()<>!\"[]{}$#@\v\f");
      m_pH->m_pTknParser->SetWhiteSpace(" \t\v\f");
      //m_pH->m_pTknParser->SetFailFn(&PGMParseFailure, (void*)this);
      if (m_pH->m_pTknParser->Start())
        {
        m_DbgMngr.InitInfo(*this, LastLine);
        short LineCnt = 0;
        while (!m_pH->m_pTknParser->AtEnd() && LineCnt<LastLine)
          m_DbgMngr.AddLine(LineCnt++, m_pH->m_pTknParser->GetNextLine());
        m_pH->m_pTknParser->End();
        while (LineCnt<LastLine)
          m_DbgMngr.AddLine(LineCnt++, "");
        }
      delete m_pH->m_pTknParser;
      m_pH->m_pTknParser = NULL;
      }

    OptimizeCode();

    if (m_bDidSaveVars)
      RestoreVars();
    }

  #if dbgGenCon
  if (dbgLoad())
    {
    DbgVarList();
    DbgExe();
    dbgpln("*Done PGM Load*");
    }
  #endif
  delete m_pH;
  m_pH = NULL;
  return Loaded;
  }

//---------------------------------------------------------------------------

flag GCInsMngr::Load(byte Places, char* fullFileName, CNodeXRefMngr* pXRM, WORD InsSet)
  {
  m_pXRM = pXRM;
  ASSERT(InsSet==DefGenConSet || InsSet==DefSolverSet); //a valid PGM symbol set must be chosen
  if (m_pVarList)
    {
    SaveVars();
    DestroyVars(m_pVarList);
    }
  Reset();
  m_DbgMngr.Reset();
  flag OK = BuildCode(Places, fullFileName, InsSet, NULL);
  return OK;
  }

//---------------------------------------------------------------------------

flag GCInsMngr::Debug(byte Places, pchar fullFileName, CNodeXRefMngr* pXRM, WORD InsSet)
  {
  ASSERT(InsSet==DefGenConSet || InsSet==DefSolverSet); //a valid PGM symbol set must be chosen
  if (m_pVarList)
    {
    SaveVars();
    DestroyVars(m_pVarList);
    }
  Reset();
  m_DbgMngr.Reset();
  m_bSetDbgOn = 1;
  m_DbgMngr.bRunTillBreak = 0;
  m_pXRM = pXRM;
  flag OK = BuildCode(Places, fullFileName, InsSet, NULL);
  return OK;
  }

//---------------------------------------------------------------------------

flag GCInsMngr::LoadFunct(char* ParseBuff, WORD InsSet)
  {
  ASSERT_ALWAYS(m_bDoDebug==0, "NOT Done", __FILE__, __LINE__); //NOT DONE !!!

  //ASSERT(m_pFirstBlk      == NULL);
  //ASSERT(m_pCurrentBlk    == NULL);
  //ASSERT(m_pIPtr          == NULL);
  ////ASSERT(m_pVarList       == NULL);

  if (m_pH)
    {
    m_pH->m_bEqualFlag   = 0;
    m_pH->m_bExpectStr   = 0;
    m_pH->m_bDoingNext   = 0;
    m_pH->m_bWhileIns    = 0;
    m_pH->m_iPushPopClassDepth = 0;
    m_pH->m_pFunctVar    = NULL;
    m_pH->m_pClassVar    = NULL;
    m_pH->m_pTempVar     = NULL;
    m_pH->m_iWhilePos    = -1;
    m_pH->m_iTxtLblCnt   = 0;
    m_pH->m_iPgLblCnt    = 0;
    }
  m_iStrPos      = -1;
  m_iCalcPos     = -1;
  m_iClassRefPos = -1;
  m_FntStackPos  = -1;
  m_pIPtr = m_pEndIns;
  //m_DbgMngr.Reset();
  flag OK = BuildCode(0, NULL, InsSet, ParseBuff, True, 128, 128);
  return OK;
  }

//---------------------------------------------------------------------------

void GCInsMngr::OptimizeCode()
  {
  //optimize jumps...
  m_pcIPtr = m_pFirstBlk;
  while (m_pIPtr)
    {
    if (m_bDoDebug)
      {
      //pCPGMDbg pDbg = *((CPGMDbg**)m_pcIPtr);
      m_pcIPtr += sizeof(pCPGMDbg);
      }
    if (m_pIPtr->Code()==&GCJmp)
      {
      GCIns *pIns = ((pGCJmpIns)m_pIPtr)->m_pJmpPos;
      while (pIns->Code()==&GCJmp)
        {
        pIns = ((pGCJmpIns)pIns)->m_pJmpPos;
        }
      ((pGCJmpIns)m_pIPtr)->m_pJmpPos = pIns;
      }
    m_pIPtr->Advance(*this);
    }

  //remove unused reference class variables...
  pGCVar pVar = m_pVarList;
  pGCVar pPrevVar = NULL;
  while (pVar)
    {
    pGCVar pDelVar = NULL;
    if (pVar->WhatAmI()==VarClass && (pVar->m_flags & VarClassDefn)!=0 && (pVar->m_flags & VarClassUsed)==0)
      {
      if (pPrevVar)
        pPrevVar->m_pNext = pVar->Next();
      else
        m_pVarList = pVar->Next();
      pDelVar = pVar;
      }
    if (pDelVar==NULL)
      pPrevVar=pVar;
    pVar = pVar->Next();
    if (pDelVar)
      delete pDelVar;
    }

  DoOptimizeVars(m_pVarList);
  }

//---------------------------------------------------------------------------

void GCInsMngr::DoOptimizeVars(pGCVar pVar)
  {
  while (pVar)
    {
    if (pVar->WhatAmI()==VarClass/* && (pVar->m_flags & VarClassDefn)==0*/)
      {
      pGCClassVar pClassVar = (pGCClassVar)pVar;
      if (pClassVar->m_pRefClass)
        pClassVar->m_pRefClass->OnOptimize(m_pXRM, pClassVar, pClassVar->m_pSubClass);
      DoOptimizeVars(pClassVar->m_pVarList);
      }
    pVar = pVar->Next();
    }
  }

//---------------------------------------------------------------------------

const word PGMSaveVer=0x0100;

void GCInsMngr::SaveClass(FilingControlBlock &FCB, pGCVar pVar)
  {
  if (pVar->m_flags & VarClassDefn)
    return;
  GCFCBAppendRec(FCB, '<', pVar->Name(), NULL, 0);  // Start of Class;
  pGCClassVar pClassVar = (pGCClassVar)pVar;
  pClassVar->m_pRefClass->OnSave(FCB, pClassVar->m_pSubClass);
  pGCVar pVar2 = pClassVar->m_pVarList;
  while (pVar2)
    {
    switch (pVar2->WhatAmI())
      {
      case VarFunct:
      case VarArray:
      case VarTag:
        break;
      case VarDouble:
      case VarLong  :
      case VarByte  :
      case VarBit   :
      case VarStr   :
        GCFCBAppendValue(FCB, pVar2, pVar2->Name());
        break;
      case VarClass:
        SaveClass(FCB, pVar2);
        break;
      default:
        ASSERT(FALSE);
      }
    pVar2 = pVar2->Next();
    }
  GCFCBAppendRec(FCB, '>', pVar->Name(), NULL, 0);  // End of Class;
  }

//---------------------------------------------------------------------------

flag GCInsMngr::SaveScenario(FilingControlBlock &FCB)
  {
  CTime t = CTime::GetCurrentTime();
  word Ver=PGMSaveVer;
  GCFCBAppendRec(FCB, 'v', "", &Ver, sizeof(Ver));
  pGCVar pVar = m_pVarList;
  while (pVar)
    {
    if ((pVar->m_flags & VarConst)==0)//do not need to save constants
      {
      switch (pVar->WhatAmI())
        {
        case VarFunct:
        case VarArray:
        case VarTag:
          break;
        case VarDouble:
        case VarLong  :
        case VarByte  :
        case VarBit   :
        case VarStr   :
          GCFCBAppendValue(FCB, pVar, pVar->Name());
          break;
        case VarClass:
          SaveClass(FCB, pVar);
          break;
        default:
          ASSERT(FALSE);
        }
      }
    pVar = pVar->Next();
    }
  return True;
  }

//---------------------------------------------------------------------------

void GCInsMngr::LoadClass(FilingControlBlock &FCB, pGCVar pClssVar)
  {
  flag BadClass = False;
  pGCClassVar pClassVar = NULL;
  if (pClssVar)
    {
    if (pClssVar->WhatAmI()!=VarClass)
      BadClass = True;
    else
      {
      pClassVar = (pGCClassVar)pClssVar;
      if (pClassVar->m_pRefClass)
        {
        if (!pClassVar->m_pRefClass->OnLoad(FCB, pClassVar->m_pSubClass))
          {
          pClassVar = NULL;
          BadClass = True;
          dbgpln("***Error: loading internal protected data members for PGM class %s", pClassVar->Name());
          }
        }
      else
        {
        pClassVar = NULL;
        BadClass = True;
        }
      }
    }
  else
    BadClass = True;

  pGCVar pNextClassVar = NULL;
  pGCVar pVar = NULL;
  while (!FCB.Eof())
    {
    GCFCBBuff Buff;
    GCFCBReadBuff(FCB, Buff);

    switch (Buff.Hd.Id)
      {
      case VarDouble:
      case VarLong  :
      case VarByte  :
      case VarBit   :
      case VarStr   :
        {
        if (BadClass)
          break;
        pGCVar pVarList = (pClassVar ? pClassVar->m_pVarList : m_pVarList);
        pVar = pVarList->Find(Buff.Name());
        if (pVar)
          {
          switch (Buff.Hd.Id)
            {
            case VarDouble: pVar->set(Buff.D()); break;
            case VarLong  : pVar->set(Buff.L()); break;
            case VarByte  : pVar->setC(Buff.C()); break;
            case VarBit   : pVar->set(Buff.B()); break;
            case VarStr   : ((pGCStrVar)pVar)->set(Buff.S()); break;
            }
          }
        }
        break;
      case '<': //class start
        {
        pGCVar pVarList = (pClassVar ? pClassVar->m_pVarList : m_pVarList);
        pNextClassVar = pVarList->Find(Buff.Name());
        LoadClass(FCB, pNextClassVar);
        break;
        }
      case '>': //class end
        return;
        break;
      default:
        //ASSERT(0);
        break;
      }
    }
  }

//---------------------------------------------------------------------------

flag GCInsMngr::LoadScenario(FilingControlBlock &FCB)
  {
  pGCVar pVar = NULL;
  flag BadClass=0;
  while (!FCB.Eof())
    {
    GCFCBBuff Buff;
    GCFCBReadBuff(FCB, Buff);
    switch (Buff.Hd.Id)
      {
      case 'v' :
        if (*((word*)Buff.Data())!=PGMSaveVer)
          LogError(m_pXRM->GetOwnerTag(), 0, "Bad Data file Version");
        break;
      case VarDouble:
      case VarLong  :
      case VarByte  :
      case VarBit   :
      case VarStr   :
        {
        pGCVar pVar = m_pVarList->Find(Buff.Name());
        if (pVar)
          {
          switch (Buff.Hd.Id)
            {
            case VarDouble: pVar->set(Buff.D()); break;
            case VarLong  : pVar->set(Buff.L()); break;
            case VarByte  : pVar->setC(Buff.C()); break;
            case VarBit   : pVar->set(Buff.B()); break;
            //case VarStr   : /*ASSERT(0);CNM*/ dbgpln("PGM_E:ToDo"); break; //todo
            case VarStr   :
              if (pVar->WhatAmI()==VarStr)
                ((GCStrVar*)pVar)->set(Buff.S());
              break;
            }
          }
        break;
        }
      case '<':
        {
        pGCVar pClassVar = m_pVarList->Find(Buff.Name());
        LoadClass(FCB, pClassVar);
        break;
        }
      case '>':
        ASSERT(0); //expect class end in LoadClass function
        break;
      default:
        //ASSERT(0);
        break;
      }
    }
  return True;
  }

flag GCInsMngr::ParseTerminalIO()
  {
  char Tkn[MaxTknMemSize];
  Tkn[sizeof(Tkn)-1] = 0;

  strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
  bool IsIn=_stricmp(Tkn, "In")==0;
  bool IsOut=_stricmp(Tkn, "Out")==0;
  if (IsIn || IsOut)
    {
    // Get Name
    strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if (m_pH->m_pTknParser->GetCurTokenType()!=TokAlpha)
      Err(ErrBuild, 158);
    else
      {
      CString TermStripName=Tkn;
      CString WireName[100],S;
      long N=0;
      strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
      if (_stricmp(Tkn, "[")==0)
        {
        strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
        while (m_pH->m_pTknParser->GetCurTokenType()==TokAlpha ||
               m_pH->m_pTknParser->GetCurTokenType()==TokNumeric)
          {
          //WireName[N]=TermStripName;
          //WireName[N]+=":";
          //WireName[N]+=Tkn;
          WireName[N]=Tkn;
          N++;
          strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);

          if (_stricmp(Tkn, ",")==0)
            strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
          };
        if (_stricmp(Tkn, "]")!=0)
          Err(ErrBuild, 159);
        }
      else
        Err(ErrBuild, 159);

      // Build Definition
      CETermStripDefn CIO;
      CIO.m_sName=TermStripName;
      CIO.m_bIn=IsIn;
      CIO.m_bReqd=false;
      CIO.m_Terms.SetSize(0);
      for (long i=0; i<N; i++)
        CIO.m_Terms.Add(CETermDefn(WireName[i]));
      m_TermStripDefns.Add(CIO);

      // Add tokens to 'token stream'
      m_pH->m_pTknParser->AddToken2Queue("TerminalStrip"); // this must be first to prevent recursion
      m_pH->m_pTknParser->AddToken2Queue(TermStripName);
      for (i=0; i<N; i++)
        {
        m_pH->m_pTknParser->AddToken2Queue("Terminal");
        S.Format("%s:%s", (LPCTSTR)TermStripName, (LPCTSTR)WireName[i]);
        m_pH->m_pTknParser->AddToken2Queue(S);
        }
      }
    }
  else
    Err(ErrBuild, 157);
  return true;
  }

//---------------------------------------------------------------------------

void GCInsMngr::Build( pGCDef pExitDef,
                       pGCIns pPrevIns)
  {
  ASSERT(pPrevIns==NULL || pPrevIns->Code()==&GCIf);
  Strng PrevVarName;
  char Tkn[MaxTknMemSize];
  Tkn[sizeof(Tkn)-1] = 0;
  GCIns* pIns = NULL;
  GCDef* pDef = NULL;
  GCVar* pVar = NULL;
  GCIns* jmpStack[1024];
  short VarIndex = -1;
  VarLocation eVarLoc = VL_NotFound;
  int stackPos = -1;
  flag FoundElse = False;
  flag FoundIf = False;
  flag ClassArrayBusy = False;
  if (pPrevIns)//(pExitDef == &GCEndif) || (pExitDef == &GCEndWhile) || (pExitDef == &GCEnd))
    {
    jmpStack[0] = pPrevIns;
    stackPos = 0;
    FoundIf = True;
    }
  flag Exit = False;
  m_pH->m_bPrevLHS = 0;

  while (!m_pH->m_pTknParser->AtEnd() && (pDef != pExitDef))
    {
    strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if (_stricmp(Tkn, "TerminalStrip")==0)
      {
      ParseTerminalIO();
      strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
      // else fall through with error
      }

    pDef = GCDef::Find(Tkn);
    if (pDef==&GCEOF2)
      pDef = &GCEOF;
    m_pH->m_iBreakLineNo = m_pH->m_pTknParser->GetLineNo();

    if (pDef)
      {//valid definition instruction found
      if (!(pDef->m_defFlags & m_pH->m_iInsSet))
        Err(ErrBuild, 130);
      if (pDef==&GCEOF && pExitDef!=&GCEOF)
        {
        if (pExitDef==&GCEndif)
          Err(ErrBuild, 140);
        else if (pExitDef==&GCEndWhile)
          Err(ErrBuild, 141);
        else
          Err(ErrBuild, 110);
        }
      if ((!m_pH->m_bPrevLHS) && (pDef->m_defFlags & DefLHS))
        {
        if (pDef == &GCTag)
          {
          pDef->Construct(*this);
          pVar = m_pH->m_pTempTagVars[m_pH->m_iTempTagPos];
          pVar->m_flags |= VarTagOut;
          m_pH->m_bPrevLHS = 1;
          }
        else if ((pDef == &GCIf) || (pDef == &GCWhile) || (pDef == &GCBegin))
          {
          if (stackPos<0)
            jmpStack[++stackPos] = pDef->Construct(*this); //first if in build
          else
            pDef->Construct(*this); //if nested within if
          }
        else if (pDef == &GCElseif)
          {
          if (!FoundIf || pExitDef!=&GCEndif)
            Err(ErrBuild, 111);
          if (FoundElse)
            Err(ErrBuild, 112);
          jmpStack[++stackPos] = GCJmp.Construct(*this);
          jmpStack[++stackPos] = pDef->Construct(*this);
          }
        else if (pDef == &GCElse)
          {
          if (!FoundIf || pExitDef!=&GCEndif)
            Err(ErrBuild, 113);
          jmpStack[++stackPos] = GCJmp.Construct(*this);
          FoundElse = True;
          }
        else if ((pDef == &GCEndif) || (pDef == &GCEndWhile) || (pDef == &GCEnd))
          {
          if (!FoundIf)
            {
            if (pDef == &GCEndif)
              Err(ErrBuild, 114);
            if (pDef == &GCEnd)
              Err(ErrBuild, 115);
            if (pDef == &GCEndWhile)
              Err(ErrBuild, 116);
            }
          pIns = pDef->Construct(*this);
          for (int i=0; i<=stackPos; i++)
            {
            ASSERT(jmpStack[i]->Code()==&GCIf || jmpStack[i]->Code()==&GCJmp);
            if (jmpStack[i]->Code()==&GCIf)
              {
              if (i==stackPos)
                ((pGCIfIns)jmpStack[i])->m_pFalseJmpPos = pIns;
              else
                {
                pGCIns pTempIns = m_pIPtr;
                m_pIPtr = jmpStack[i+1];
                m_pIPtr->Advance(*this);
                if (m_bDoDebug)
                  m_pcIPtr += sizeof(pCPGMDbg);
                ((pGCIfIns)jmpStack[i])->m_pFalseJmpPos = m_pIPtr;
                m_pIPtr = pTempIns;
                }
              }
            else
              ((pGCJmpIns)jmpStack[i])->m_pJmpPos = pIns;
              //((pGCIfIns)jmpStack[i])->m_pFalseJmpPos = pIns;
            }
          }
        else
          pIns = pDef->Construct(*this);
        }
      else if ((m_pH->m_bPrevLHS) && (pDef->m_defFlags & Def2ndLHS))
        {
        ASSERT(pDef == &GCEqual);      //must be GCEqual
        if (VarIndex>=0)
          {//in (a) class member function, or (b) class array; assign to class data member
          if (pVar->WhatAmI()==VarStr)
            m_pH->m_bExpectStr = True;
          pIns = GCEqualClassRef.Construct(*this);
          m_pH->m_bExpectStr = False;
          if (ClassArrayBusy || strchr(PrevVarName(),'.')==NULL)
            {
            ((pGCEqualClassRefIns)pIns)->m_Index = VarIndex;
            ((pGCEqualClassRefIns)pIns)->m_sVarName = "";
            if (ClassArrayBusy)
              GCPopClassRef.Construct(*this);
            }
          else
            {
            Strng ClassName(PrevVarName());
            ClassName = ClassName.Left(ClassName.Find('.'));
            m_pH->m_pClassVar->m_pVarList->Find(ClassName(), VarIndex); //look in class scope
            ((pGCEqualClassRefIns)pIns)->m_Index = VarIndex;
            ((pGCEqualClassRefIns)pIns)->m_sVarName = PrevVarName();
            }
          }
        else if (pVar->WhatAmI()==VarTag)
          {
          pIns = GCEqualTag.Construct(*this);
          ((pGCEqualTagIns)pIns)->m_pVar = (GCTagVar*)pVar;
          ((pGCEqualTagIns)pIns)->m_sCnvTxt = m_pH->m_sTempCnvTxt[m_pH->m_iTempTagPos--];
          }
        else
          {
          if (pVar->WhatAmI()==VarStr)
            m_pH->m_bExpectStr = True;
          pIns = pDef->Construct(*this);
          ((pGCEqualIns)pIns)->m_pVar = pVar;
          m_pH->m_bExpectStr = False;
          }
        m_pH->m_bPrevLHS = 0;
        }
      else
        Err(ErrBuild, 105);
      }
    else
      {
      if (m_pH->m_bPrevLHS)
        Err(ErrBuild, 105);
      else
        {
        ClassArrayBusy = False;
        pVar = FindVar(Tkn, VarIndex, eVarLoc);
        PrevVarName = Tkn;
        if (pVar==NULL && Tkn[strlen(Tkn)-1]==':') // ie a Label CNM
          {
          if (!(GCLbl.m_defFlags & m_pH->m_iInsSet))
            Err(ErrBuild, 130);
          pIns = GCLbl.Construct(*this);
          ((pGCLblIns)pIns)->m_sName = Tkn;
          m_pH->m_bPrevLHS = 0;
          }
        else
          {
          if (pVar==NULL)
            {
            if (m_pH->m_iInsSet==DefSolverSet)
              {//assume it is an external tag
              pVar = GCTag.ConstructVar(m_pVarList, &m_VarMap, Tkn, 0);
              pVar->m_flags |= VarTagOut;
              m_pH->AddToTagStack(*this, pVar, "");
              }
            else
              Err(ErrBuild, 109);
            }
          if (pVar->WhatAmI() == VarFunct)
            {
            if (strchr(Tkn, '.'))
              {
              short ClassIndex = -1;
              pGCClassVar pClassVar = NULL;
              if (m_pH->m_pFunctVar)
                pClassVar = m_pH->m_pFunctVar->m_pVarList->FindClass(Tkn); //look in function scope
              if (pClassVar==NULL && m_pH->m_pClassVar)
                pClassVar = m_pH->m_pClassVar->m_pVarList->FindClass(Tkn, ClassIndex); //look in class scope
              if (pClassVar==NULL)
                pClassVar = m_pVarList->FindClass(Tkn); //look in global scope
              if (pClassVar==NULL)
                Err(ErrBuild, 133);
              GCPushClassRefIns* pPushIns = (GCPushClassRefIns*)GCPushClassRef.Construct(*this);
              pPushIns->m_pClassVar = pClassVar;
              pPushIns->m_Index = ClassIndex;
              pPushIns->m_pArrayVar = NULL;
              }
            m_pH->m_pTempVar = pVar; //must remember pointer to funct for callfnt construct
            pIns = GCCallFnt.Construct(*this);
            if (strchr(Tkn, '.'))
              GCPopClassRef.Construct(*this);
            GCPop.Construct(*this);
            }
          else if (pVar->WhatAmI() == VarArray)
            {
            DoConstructClassArrayAccess();
            GCArrayVar* pArrayVar = (GCArrayVar*)pVar;
            strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
            pVar = pArrayVar->m_pClassRefVar->m_pVarList->Find(&Tkn[1], VarIndex); //get the right VarIndex
            if (pVar==NULL)
              Err(ErrBuild, 118);
            GCPushClassRefIns* pPushIns = (GCPushClassRefIns*)GCPushClassRef.Construct(*this);
            pPushIns->m_pClassVar = NULL;
            pPushIns->m_Index = VarIndex;
            pPushIns->m_pArrayVar = pArrayVar;
            if (pVar->WhatAmI() == VarFunct)
              {
              m_pH->m_pTempVar = pVar; //must remember pointer to funct for callfnt construct
              pIns = GCCallFnt.Construct(*this);
              GCPopClassRef.Construct(*this);
              GCPop.Construct(*this);
              }
            else
              {
              ClassArrayBusy = True;
              m_pH->m_bPrevLHS = 1;
              }
            }
          else if ((pVar->WhatAmI() == VarClass) && (pVar->m_flags & VarClassDefn))
            {//construct instances of classes
            GCClassDec.m_pClassVar = (pGCClassVar)pVar;
            if (m_pH->m_pFunctVar)
              DoConstructVars(GCClassDec, m_pH->m_pFunctVar->m_pVarList, &m_pH->m_pFunctVar->m_VarMap); //in function scope
            else
              DoConstructVars(GCClassDec, m_pVarList, &m_VarMap); //in global scope
            }
          else
            {
            if (pVar->WhatAmI() == VarClass)
              Err(ErrBuild, 118);
            if (pVar->m_flags & VarConst)
              Err(ErrBuild, 119);
            m_pH->m_bPrevLHS = 1;
            }
          }
        }
      }
    }

  //m_TermStrips=m_TermStripDefns;
  //long N=m_TermStripDefns.Count();
  //m_TermStripIOIds.SetSize(N);
  //for (int t=0; t<N; t++)
  //  m_TermStripIOIds[t]=-1;
  }

//---------------------------------------------------------------------------

int GCInsMngr::UpdateXRef(pGCVar pVar)
  {
  ASSERT(pVar->WhatAmI()==VarTag);
  ASSERT(pVar->m_flags & (VarTagIn | VarTagOut));
  pVar->m_flags &= ~(VarTagNear | VarTagFar);

  GCTagVar* pTagVar= (GCTagVar*)pVar;

  pTagVar->m_pXRef=new CXRefItem(m_pXRM->NodeXRefsLocked());//(pTagVar);
  CXRefItem *pXRef=pTagVar->m_pXRef;

  pXRef->m_bDynTag = (pTagVar->m_flags & VarDynTag)!=0;
  pXRef->m_bMustSet = ((pVar->m_flags & VarTagOut)!=0);
  pXRef->m_bMustGet = ((pVar->m_flags & VarTagIn)!=0);
  pXRef->m_iType = pTagVar->m_bStrTag ? tt_Strng : tt_Double; // assume double

  pXRef->m_sRefTag = pTagVar->Name();
  pXRef->m_sRefID  = pXRef->m_sRefTag;
  pXRef->m_sCnv    = pTagVar->RqdCnvTxt();

  Strng RTg;
  if (pTagVar->m_pFldName)
    {
    RTg=pTagVar->m_pFldName;
    pXRef->m_Lcl.m_bIsFnTerm = true;
    }
  else
    RTg.Set("%s [%s]", m_pXRM->GetOwnerTag(), pXRef->m_sRefTag);
  pXRef->m_sRmtTag = RTg;
  pXRef->m_sRmtID  = pXRef->m_sRmtTag;

  //dbgpln("  ---> UpdateXRef %s %s %s ", pXRef->m_bMustSet?"Set":"   ", pXRef->m_bMustGet?"Get":"   ", pXRef->m_sRefTag());

  //pXRef->SetRefTagOK(1);
  //dbgpln("FNXR %s", pXRef->pName);
#pragma chCHECKIT("MustBeParm")
  int RetCode = m_pXRM->ResolveNearXRef(pXRef, false);
  if (pXRef->m_bDynTag)
    RetCode |= FXR_DynTag;
  if (!(RetCode & FXR_NotFound))
    {//near tags are in the flowsheet (graphics model engine)
    if (!(RetCode & FXR_BadValue))
      {
      pTagVar->m_iCnvIndex = pXRef->m_iCnvIndex;
      pTagVar->m_bOnlyAllowGet = ((RetCode & FXR_ParamStopped) || (RetCode & FXR_NotParam));
      pTagVar->m_bAllowSetAtInit = ((RetCode & FXR_ParamStopped)!=0);
      if ( (RetCode & FXR_Found) ||
          (pXRef->m_bDynTag && ((RetCode & FXR_ParamStopped) || (RetCode & FXR_NotParam))) ||
          (!pXRef->m_bMustSet && ((RetCode & FXR_ParamStopped) || (RetCode & FXR_NotParam))) )
        {
        RetCode |= FXR_Found;
        pVar->m_flags |= VarTagNear;
        if (pXRef->m_bDynTag)
          pVar->m_flags |= VarDynTagOK;
        }
      
      if (pXRef->m_TAB.Flags()&DDEF_NOFILE)
        {
        int xxx=0;
        }
      }
    }
  else
    {
    RetCode = m_pXRM->ResolveFarXRef(pXRef);
    if (!(RetCode & FXR_NotFound))
      {
      if (!(RetCode & FXR_BadValue))
        {//far tags are external to the flowsheet (example driver tags)
        pTagVar->m_iCnvIndex = pXRef->m_iCnvIndex;
        pTagVar->m_bOnlyAllowGet = ((RetCode & FXR_ParamStopped) || (RetCode & FXR_NotParam));
        pTagVar->m_bAllowSetAtInit = ((RetCode & FXR_ParamStopped)!=0);
        if ( (RetCode & FXR_Found) ||
            (pXRef->m_bDynTag && ((RetCode & FXR_ParamStopped) || (RetCode & FXR_NotParam))) ||
            (!pXRef->m_bMustSet && ((RetCode & FXR_ParamStopped) || (RetCode & FXR_NotParam))) )
          {
          RetCode |= FXR_Found;
          pVar->m_flags |= VarTagFar;
          if (pXRef->m_bDynTag)
            pVar->m_flags |= VarDynTagOK;
          }
        }
      }
    }

  if ((RetCode & FXR_Found) && !(RetCode & FXR_BadValue))
    {
    if ((pXRef->m_iCnvIndex)>0 && pXRef->m_sCnv.IsEmpty())
      {
      pXRef->m_sCnv = (Cnvs[pXRef->m_iCnvIndex]->SICnv())->Txt();
      //sVar.Set("%s (%s)", pXRef->m_sTag(), pXRef->m_sCnv());
      }
    else if (((pXRef->m_iCnvIndex==0) && !pXRef->m_sCnv.IsEmpty()) ||
             ((pXRef->m_iCnvIndex>0) && (Cnvs[pXRef->m_iCnvIndex]->Find(pXRef->m_sCnv())==NULL)))
      RetCode |= FXR_BadCnv;

    pXRef->CopyToLcl();

    if ((pTagVar->m_flags & VarTagNear)!=0)
      pTagVar->m_pXRef=m_pXRM->AddNearXRef(pTagVar->m_pXRef);
    else if ((pVar->m_flags & VarTagFar)!=0)
      pTagVar->m_pXRef=m_pXRM->AddFarXRef(pTagVar->m_pXRef);
    #if dbgGenCon
    if (dbgXRefs() || dbgXRefsDbgBrk() && m_pXRM->GetOwnerDbgBrk())
      {
      dbgpln("XR: %-7.7s %s%s %s %s",
            tt_TypeString(pTagVar->m_pXRef->m_iType),
            pTagVar->m_pXRef->m_bMustGet ? "I":" ", pTagVar->m_pXRef->m_bMustSet ? "O":" ",
            (pTagVar->m_flags & VarTagNear)!=0 ? "Near" : (pTagVar->m_flags & VarTagFar)!=0 ? "Far ": "?   ",
            pTagVar->m_pXRef->m_sRefTag());
      }
    #endif
    }

  return RetCode;
  }

//---------------------------------------------------------------------------

int GCInsMngr::UpdateXRefLists(CXRefBuildResults & Results)
  {
  #if dbgGenCon
  if (dbgXRefs() || dbgXRefsDbgBrk() && m_pXRM->GetOwnerDbgBrk())
    {
    DbgVarList();
    dbgpln("-------------------------------------------------------------------\nBuild External Refernce List... %s", m_pXRM->GetOwnerTag());
    }
  #endif

  pGCVar pVar = m_pVarList;
  while (pVar)
    {
    if (pVar->m_flags & (VarTagIn | VarTagOut | VarDynTag))
      {
      GCTagVar* pTagVar=(GCTagVar*)pVar;
      int RetCode = UpdateXRef(pTagVar);
      if (!(RetCode & FXR_Found) && (RetCode & FXR_DynTag))
        {
        LPCTSTR Tag=((GCTagVar*)pVar)->Name();
        if (RetCode & FXR_DynTag)
          {
          if (pVar->m_flags & VarDynTagOK)
            {
            Results.m_nBadDynTagsCurrent++;
            Results.m_BadTags.AddTail(CXRefBuildResult(Tag, FXR_DynTagMissing));
            }
          else
            {
            Results.m_nBadDynTagsAlways++;
            Results.m_BadTags.AddTail(CXRefBuildResult(Tag, FXR_DynTagNeverFound));
            }
          }
        }
      else if ((RetCode & FXR_ParamStopped) && (RetCode & FXR_DynTag))
        {//It is OK for a SetDynTag to set a ParamStopped in OnInitialise!
        }
      else if (RetCode & (FXR_NotFound|
                          FXR_BadValue|
                          FXR_NotParam|
                          FXR_ParamStopped|
                          FXR_BadCnv))
        {
        LPCTSTR Tag=((GCTagVar*)pVar)->Name();
        Strng Msg;
        if (RetCode & FXR_NotFound)
          {
          Msg.Set("'%s' not found", Tag);
          LogError(m_pXRM->GetOwnerTag(), 0, Msg());
          Results.m_BadTags.AddTail(CXRefBuildResult(Tag, RetCode));
          }
        if (RetCode & FXR_BadValue)
          {
          Msg.Set("'%s' not allowed (tag should be a single value)", Tag);
          LogError(m_pXRM->GetOwnerTag(), 0, Msg());
          Results.m_BadTags.AddTail(CXRefBuildResult(Tag, RetCode));
          }
        if (RetCode & FXR_NotParam)
          {
          Msg.Set("'%s' not allowed (tag cannot be set)", Tag);
          LogError(m_pXRM->GetOwnerTag(), 0, Msg());
          Results.m_BadTags.AddTail(CXRefBuildResult(Tag, RetCode));
          }
        if (RetCode & FXR_ParamStopped)
          {
          Msg.Set("'%s' not allowed (tag cannot be set while solving)", Tag);
          LogError(m_pXRM->GetOwnerTag(), 0, Msg());
          Results.m_BadTags.AddTail(CXRefBuildResult(Tag, RetCode));
          }
        if (RetCode & FXR_BadCnv)
          {
          Msg.Set("'%s' has a bad conversion", Tag);
          LogError(m_pXRM->GetOwnerTag(), 0, Msg());
          Results.m_BadTags.AddTail(CXRefBuildResult(Tag, RetCode));
          }
        //else
        //  Msg="???";
        //LogError(m_pXRM->GetOwnerTag(), 0, Msg());
        Results.m_nBadXRefs++;
        }
      }
    else
      {
      #if dbgGenCon
      if (dbgXRefs() || dbgXRefsDbgBrk() && m_pXRM->GetOwnerDbgBrk())
        {
        dbgpln("??: %s", ((GCTagVar*)pVar)->Name());
        }
      #endif
      }
    pVar = pVar->Next();
    }
  if (Results.TotalErrors()>0)
    m_StateLine[0].Set("%d bad external tag reference%s", Results.TotalErrors(), (Results.TotalErrors()>1 ? "s" : ""));
  else if (m_pIPtr)
    m_StateLine[0] = "OK";
  return Results.TotalErrors();
  }

//---------------------------------------------------------------------------

//int GCInsMngr::CheckCnvs()
//  {//must call this after UpdateXRefLists
//  int nErrors = 0;
//  m_pcIPtr = m_pFirstBlk;
//  Strng Msg;
//  while (m_pIPtr)
//    {
//    if (m_bDoDebug)
//      m_pcIPtr += sizeof(pCPGMDbg);
//    GCTagVar* pTagVar = NULL;
//    char* pCnvTxt = NULL;
//    if (m_pIPtr->Code()==&GCGetTag)
//      {
//      pTagVar = ((GCGetTagIns*)m_pIPtr)->m_pVar;
//      pCnvTxt = ((GCGetTagIns*)m_pIPtr)->m_sCnvTxt();
//      }
//    else if (m_pIPtr->Code()==&GCSetTag)
//      {
//      pTagVar = ((GCSetTagIns*)m_pIPtr)->m_pVar;
//      pCnvTxt = ((GCSetTagIns*)m_pIPtr)->m_sCnvTxt();
//      }
//    else if (m_pIPtr->Code()==&GCVarTagRef)
//      {
//      pTagVar = ((pGCVarTagRefIns)m_pIPtr)->m_pVar;
//      pCnvTxt = ((pGCVarTagRefIns)m_pIPtr)->m_sCnvTxt();
//      }
//    else if (m_pIPtr->Code()==&GCEqualTag)
//      {
//      pTagVar = ((pGCEqualTagIns)m_pIPtr)->m_pVar;
//      pCnvTxt = ((pGCEqualTagIns)m_pIPtr)->m_sCnvTxt();
//      }
//    if (pTagVar && pCnvTxt && pTagVar->m_bTagOK && strlen(pCnvTxt)>0)
//      {//check engineering units...
//      if (pTagVar->m_iCnvIndex==DC_)
//        {
//        LogWarning(m_pXRM->GetOwnerTag(), 0, "%s has no defined engineering units.  %s", pTagVar->Name(), m_pIPtr->Code()->Name());
//        nErrors++;
//        }
//      else if (gs_CnvsMngr.FindSecCnv(pTagVar->m_iCnvIndex, pCnvTxt)==NULL)
//        {
//        LogWarning(m_pXRM->GetOwnerTag(), 0, "Tag:%s  (%s) is not a valid unit for %s.  %s", pTagVar->Name(), pCnvTxt, Cnvs[pTagVar->m_iCnvIndex]->Fam(), m_pIPtr->Code()->Name());
//        nErrors++;
//        }
//      }
//    m_pIPtr->Advance(*this);
//    }
//  return nErrors;
//  }

//---------------------------------------------------------------------------

void GCInsMngr::GetTermStripData(CGExecContext &ECtx)
  {

  for (GCVar * pV=m_pVarList; pV; pV=pV->m_pNext)
    {
    GCClassVar * pC=dynamic_cast<GCClassVar*>(pV);
    if (pC)
      {
      GCETerminal * pE=dynamic_cast<GCETerminal*>(pC->m_pRefClass);
      if (pE)
        {
        GCETerminalBase * pEB=(GCETerminalBase*)pC->m_pSubClass;
        if (pEB)
          {
          //if (ECtx.OnInitialise)
            {
            CETerminal * pTrm=NULL;
            pEB->ClrTerminal();
            CString IONm=pC->Name();
            int i=IONm.FindOneOf(":");
            if (i>=0)
              {
              CString TSNm(IONm.Left(i));
              CString TrmNm(IONm.Right(IONm.GetLength()-i-1));
              pTrm=m_pTermStripPtrs->Terminal(TSNm, TrmNm);
              pEB->SetTerminal(pTrm);
              }
            if (!pTrm)
              {
//              _asm int 3;
              int xxx=0;
              }
            }
          if (pEB->Terminal())
            {
            pEB->m_V = pEB->Terminal()->m_V;
            //pEB->m_I = pEB->Terminal()->m_I;
            }
          }
        }
      else
        {
        GCTermStrip * pTS=dynamic_cast<GCTermStrip*>(pC->m_pRefClass);
        if (pTS)
          {
          GCTermStripBase * pTSB=(GCTermStripBase *)pC->m_pSubClass;
          if (pTSB)
            {
            //if (ECtx.OnInitialise)
              {
              CString TSNm=pC->Name();
              CETermStrip *p=(*m_pTermStripPtrs)[TSNm];
              pTSB->SetTermStrip(p);
              if (!p)
                {
               // _asm int 3;
                int xxx=0;
                }
              }
            }
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

void GCInsMngr::PutTermStripData(CGExecContext &ECtx)
  {
  for (GCVar * pV=m_pVarList; pV; pV=pV->m_pNext)
    {
    GCClassVar * pC=dynamic_cast<GCClassVar*>(pV);
    if (pC)
      {
      GCETerminal * pE=dynamic_cast<GCETerminal*>(pC->m_pRefClass);
      if (pE)
        {
        GCETerminalBase * pEB=(GCETerminalBase*)pC->m_pSubClass;
        if (pEB)
          {
          if (pEB->Terminal())
            {
            //CETerminal & T=(*(*m_pTermStripPtrs)[pEB->TSIndex()])[pEB->TIndex()];
            pEB->Terminal()->m_V     = pEB->m_V;
            //pEB->Terminal()->m_I     = pEB->m_I;
            //T.m_dPhi = pEB->m_dPhase;
            }
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

#define WITHPGMTIMERS  (0 && WITHTIMERS)

#if WITHPGMTIMERS
static CGlblStopWatch s_swPGMExec(gs_swFlwSolve, "PGM", "Execute");
#define PGMTIMERLAP(x) CStopWatchLap SW##__LINE__(x)
#else
#define PGMTIMERLAP(x)
#endif


double GCInsMngr::sm_ExecTimeout=10;
int GCInsMngr::Execute(CGExecContext &ECtx, LPCTSTR TriggerName, CNodeElectrics * pTermStripPtrs/*=NULL*/)
  {
  PGMTIMERLAP(s_swPGMExec);

  m_pTermStripPtrs=pTermStripPtrs;
  m_ProcExecuted = false;
  
  ECtx.DoXStop = 0;
  ECtx.DoXIdle = 0;
  if (ECtx.pXRM) // incase is null - then use previous value
    m_pXRM = ECtx.pXRM;
  if (m_bDoDebug)
    m_DbgMngr.ConnectWindow();

  #if dbgGenCon
  if (dbgExec() || dbgTimes())
    dbgpln("-------------------------------------------------------------------\nExecute PGM code...");
  CStopWatch SW;
  SW.Start();

  #endif
//??  dword Ret = 0;
//??  if (ECtx.OnStart)
//??    DestroyTempDynTagVars();
//??  if (!m_bXRefsValid)
//??    Ret= m_pXRM->UpdateXRefLists();
//??  if (Ret&(XR_Changed))
//??    return 4;

  if (!ECtx.m_HoldNearXRefGet)
    m_pXRM->GetNearXRefValues();//*m_pNearGCXRefs);

  if (m_pTermStripPtrs)
    {
    //dbgpln("GCInsMngr::Execute With TermStrip %s ----------------------------------", m_pXRM->GetOwnerTag());
    GetTermStripData(ECtx);
    }


  #if dbgGenCon
  if (dbgTimes())
    dbgpln("=========GET %16.3f", SW.Lap()*1e3);
  if (dbgExec())
    {
    DbgVarList();
    dbgpln("----------------------");
    }
  #endif

  DWORD CW = FPP_DisableExceptions();
  int ret = 0;
  m_dIC_Time = ECtx.dIC_Time;
  m_dIC_dTime = ECtx.dIC_dTime;
  if (m_pOnInitialiseVar)
    {
    m_pOnInitialiseVar->set(ECtx.OnInitialise);
    m_pOnStartVar->set(ECtx.OnStart);
    m_pOnTerminateVar->set(ECtx.OnTerminate);
    m_pOnLoadedVar->set(ECtx.OnLoaded);
    m_pOnEmptyVar->set(ECtx.OnEmpty);
    m_pOnPresetVar->set(ECtx.OnPreset);
    m_pRunMode->set(ECtx.m_NetMode);
    m_pHeatMode->set(ECtx.m_HeatMode);
    m_pModelTagVar->set(m_pXRM->GetOwnerTag());
    m_pStopVar->set((flag)0);
    m_pPauseVar->set((flag)0);
    }
  m_iRunTimeErr = 0;
  m_iMathRunTimeErr = 0;
  if (ECtx.OnStart || ECtx.OnInitialise)
    {
    for (int i=0; i<MaxCondMsgs; i++)
      {
      m_CINOn[i] = 0;
      m_CIEOn[i] = 0;
      }
    }

  TRY
    {
    for (m_iClassRefPos=0; m_iClassRefPos<MAX_CLASSREF_STACK; m_iClassRefPos++)
      m_pClassRefStack[m_iClassRefPos] = NULL;
    m_iCalcPos = -1;
    m_iClassRefPos = -1;
    m_pcIPtr = m_pFirstBlk;

    if (ECtx.StartLabel || ECtx.EndLabel)
      {
      VERIFY(!m_bDoDebug); //debugging with labels NOT supported
#if dbgGenCon
      if (dbgExec())
        dbgpln("Exec %s >>> %s", ECtx.StartLabel, ECtx.EndLabel);
#endif
      if (ECtx.StartLabel)
        {
        flag FoundStartLbl=0;
        while (m_pIPtr && !FoundStartLbl)
          {
          if (m_pIPtr->Code()==&GCLbl)
            if (((pGCLblIns)m_pIPtr)->m_sName.XStrICmp(ECtx.StartLabel)==0)
              FoundStartLbl = 1;
          m_pIPtr->Advance(*this);
          }
        }

      if (m_pIPtr==NULL && ECtx.StartLabel)
        ret = 1;

      while (m_pIPtr)
        {
        if (ECtx.EndLabel)
          if (m_pIPtr->Code()==&GCLbl)
            if (((pGCLblIns)m_pIPtr)->m_sName.XStrICmp(ECtx.EndLabel)==0)
              break;
#if dbgGenCon
        if (dbgExec())
          {
          DbgCalcStack();
          char Buff[1024];
          dbgpln("Exec:%#010x %s", m_pIPtr, m_pIPtr->GetDesc(Buff));
          }
#endif
        m_pIPtr->Exec(*this);
        }
      if (m_pIPtr==NULL && ECtx.EndLabel)
        ret = 2;
      }
    else
      {
      pCPGMDbg pDbg = NULL;
      if (TriggerName)
        {
        for (GCVar * pFn=m_pVarList; pFn; pFn=pFn->Next())
          {
          if (_stricmp(pFn->Name(), TriggerName)==0)
            {
            m_ProcExecuted = true;
            // create a call
            char LclMem[256];
            m_pIPtr = (GCIns*)LclMem;
            GCCallFntIns *pInsF = (GCCallFntIns *)m_pIPtr;
            pInsF->GCCallFntIns::GCCallFntIns(); // call the constructor
            pInsF->m_pFntVar = (GCFunctVar*)pFn;
            pInsF->Advance(*this);

            GCPopIns *pInsP = (GCPopIns *)m_pIPtr;
            pInsP->GCPopIns::GCPopIns(); // call the constructor
            pInsP->Advance(*this);

            GCEOFIns *pInsE = (GCEOFIns*)m_pIPtr;
            pInsE->GCEOFIns::GCEOFIns(); // call the constructor
            pInsE->Advance(*this);

            // execute the call
            m_pIPtr=pInsF;
            while (m_pIPtr)
              m_pIPtr->Exec(*this);

            // cleanup
            break;
            }
          }
        }
      else
        {
        DWORD LoopCnt=0;
        __int64 TickCnt=GetTickCount();
        while (m_pIPtr)
          {
          if (LoopCnt++>1000)
            {
            LoopCnt=0;
            __int64 T=GetTickCount();
            if (T<TickCnt)
              TickCnt-=0xffffffff;
            if (T-TickCnt>sm_ExecTimeout*1000)
              {
              Err(ErrOtherRuntime, 16);
              break;
              }
            }
          if (m_bDoDebug)
            {
            pDbg = *((CPGMDbg**)m_pcIPtr);
            //m_DbgMngr.Advance(pDbg, pDbg, *this);
            m_pcIPtr += sizeof(pCPGMDbg);
            }
#if dbgGenCon
          if (dbgExec())
            {
            DbgCalcStack();
            char Buff[768];
            dbgpln("Exec:%#010x %s", m_pIPtr, m_pIPtr->GetDesc(Buff));
            }
#endif
          m_pIPtr->Exec(*this);

          if (m_bDoDebug)
            {
            pCPGMDbg pNxtDbg;
            if (m_pIPtr)
              {
              pNxtDbg = *((CPGMDbg**)m_pcIPtr);
              pchar pIPtr = m_pcIPtr + sizeof(pCPGMDbg);
              if (((pGCIns)pIPtr)->Code()==&GCJmp)
                {
                //pGCIns pNxtIns = ((pGCJmpIns)pIPtr)->m_pJmpPos;
                pNxtDbg = *((CPGMDbg**)(((pchar)((pGCJmpIns)pIPtr)->m_pJmpPos)-sizeof(pCPGMDbg)));
                }
              }
            else
              pNxtDbg = pDbg;

            m_DbgMngr.Advance(pDbg, pNxtDbg, *this);
            }
          }
        }
      }
    }
  CATCH(GCException, thePGMException)
    {
    ret = 3;
    //Reset(); //KGA: WHAT MUST BE DONE???
    dbgpln("** KGA TODO !!! - Exception during Execute **");
    }
  END_CATCH

  #if dbgGenCon
  if (dbgExec())
    DbgVarList();
  if (dbgTimes())
    dbgpln("=========EXE %16.3f", SW.Lap()*1e3);
  #endif

  if (m_pXRM->UpdateXRefListsReqd())
    {
    //must rebuild this here because we may need to set some newly added XRefs!
    long Ret = m_pXRM->UpdateXRefLists(true);
    }

  if (m_pTermStripPtrs)
    PutTermStripData(ECtx);

  if (!ECtx.m_HoldNearXRefSet)
    m_pXRM->SetNearXRefValues();

  if (m_pStopVar)
    ECtx.DoXStop = m_pStopVar->getB();
  if (m_pPauseVar)
    ECtx.DoXIdle = m_pPauseVar->getB();
  if (m_bDoDebug)
    m_DbgMngr.DisConnectWindow();
  #if dbgGenCon
  if (dbgTimes())
    dbgpln("=========PUT %16.3f", SW.Lap()*1e3);
  if (dbgExec())
    dbgpln("*Done Execute* [%i]\n-------------------------------------------------------------------", ret);
  #endif
  FPP_RestoreExceptions(CW);
  m_pTermStripPtrs=NULL;
  return ret;
  }

//---------------------------------------------------------------------------

//flag GCInsMngr::EvalPBConvergence(EqnSlvCtrlBlk & EQCtrl)
//  {
//  flag OK=True;
////  pGCVar pVar = m_pVarList;
////  while (pVar)
////    {
////    if (pVar->WhatAmI()==VarClass && (pVar->m_flags & VarClassDefn)==0)
////      if (!((pGCClassVar)pVar)->m_pRefClass->EvalPBConvergence(EQCtrl, (pGCClassVar)pVar))
////        OK=False;
////    pVar = pVar->Next();
////    }
//  return OK;
//  }

//---------------------------------------------------------------------------

//BOOL GCInsMngr::InitPBConvergence()
//  {
//  BOOL OK = 1;
//  pGCVar pVar = m_pVarList;
//  while (pVar)
//    {
//    if (pVar->WhatAmI()==VarClass && (pVar->m_flags & VarClassDefn)==0)
//      if (!((pGCClassVar)pVar)->m_pRefClass->InitPBConvergence((pGCClassVar)pVar))
//        OK=False;
//    pVar = pVar->Next();
//    }
//  return OK;
//  }

//---------------------------------------------------------------------------

double GCInsMngr::GetDParm()
  {
  ASSERT(m_iCalcPos >= 0); //tried to fetch too many parameters
  return m_dCalcStack[m_iCalcPos--];
  }

//---------------------------------------------------------------------------

long GCInsMngr::GetLParm()
  {
  ASSERT(m_iCalcPos >= 0); //tried to fetch too many parameters
  return GetAsLong(m_dCalcStack[m_iCalcPos--]);
  }

//---------------------------------------------------------------------------

unsigned char GCInsMngr::GetCParm()
  {
  ASSERT(m_iCalcPos >= 0); //tried to fetch too many parameters
  return (unsigned char)Range((double)0, m_dCalcStack[m_iCalcPos--], (double)UCHAR_MAX);
  }

//---------------------------------------------------------------------------

flag GCInsMngr::GetBParm()
  {
  ASSERT(m_iCalcPos >= 0); //tried to fetch too many parameters
  if (m_dCalcStack[m_iCalcPos--] == 0)
    return False;
  return True;
  }

//---------------------------------------------------------------------------

pchar GCInsMngr::GetSParm()
  {
  ASSERT(m_iStrPos >= 0); //tried to fetch too many parameters
  m_sLastSParm = m_StrStack[m_iStrPos--];
  return m_sLastSParm();
  //if (m_sLastSParm.Len()>0)
  //  return m_sLastSParm();
  //return "";
  //return (m_StrStack[m_iStrPos--])();
  }

//---------------------------------------------------------------------------

void GCInsMngr::Reset()
  {
  DestroyCode();
  DestroyVars(m_pVarList);
  if (m_pH)
    {
    m_pH->m_bEqualFlag   = 0;
    m_pH->m_bExpectStr   = 0;
    m_pH->m_bDoingNext   = 0;
    m_pH->m_bWhileIns    = 0;
    m_pH->m_iPushPopClassDepth = 0;
    m_pH->m_pFunctVar    = NULL;
    m_pH->m_pClassVar    = NULL;
    m_pH->m_pTempVar     = NULL;
    m_pH->m_iWhilePos    = -1;
    m_pH->m_iTxtLblCnt   = 0;
    m_pH->m_iPgLblCnt    = 0;
    }
  m_iStrPos      = -1;
  m_iCalcPos     = -1;
  m_iClassRefPos = -1;
  m_FntStackPos  = -1;
  m_pFirstBlk    = NULL;
  m_pCurrentBlk  = NULL;
  m_pIPtr        = NULL;
  m_pEndIns      = NULL;
  ConstructConstants();
  }

//---------------------------------------------------------------------------

int GCInsMngr::SetBrkPt(int LineNo)
  {
  ASSERT(m_bDoDebug);
  if (!m_bDoDebug)
    return 1;
  pchar CurIPtr = m_pcIPtr;
  m_pcIPtr = m_pFirstBlk;
  flag FoundIt = False;
  pCPGMDbg pDbgFnd = NULL;
  while (m_pIPtr && !FoundIt)
    {
    pCPGMDbg pDbg = *((CPGMDbg**)m_pcIPtr);
    if (pDbg->bBreakLine)
      {
      pDbgFnd = pDbg;
      if (pDbg->iLineNo >= LineNo)
        {
        pDbg->bBreakPt = !pDbg->bBreakPt;
        FoundIt = True;
        }
      }
    m_pcIPtr += sizeof(pCPGMDbg);
    m_pIPtr->Advance(*this);
    }
  m_pcIPtr = CurIPtr;
  return (pDbgFnd ? pDbgFnd->iLineNo : 1);
  }

//---------------------------------------------------------------------------

void GCInsMngr::DbgCalcStack()
  {
  #if dbgGenCon
  dbgp("_ Calc  ");
  if (m_iCalcPos < 0)
    dbgpln("           -) Calculation stack is empty");
  else
    for (int i=0; i<=m_iCalcPos; i++)
    dbgpln("%*d) %.5f", (i==0 ? 12 : 20), i, m_dCalcStack[i]);
  dbgp("_ String");
  if (m_iStrPos < 0)
    dbgpln("           -) String stack is empty");
  else
    for (int i=0; i<=m_iStrPos; i++)
      dbgpln("%*d) \"%s\"", (i==0 ? 12 : 20), i, (m_StrStack[i])());
  dbgp("_ Class ");
  if (m_iClassRefPos < 0)
    dbgpln("           -) ClassRef stack is empty");
  else
    for (int i=0; i<=m_iClassRefPos; i++)
      dbgpln("%*d) %s", (i==0 ? 12 : 20), i, (m_pClassRefStack[i])->Name());
  #endif
  }

//---------------------------------------------------------------------------

void GCInsMngr::DbgExe()
  {//list of all ins...
  #if dbgGenCon
  dbgpln("--------------------------------------------------------------------\nPGM Instruction list:");
  dbgMemIsFirstBlk = 1;
  m_pcIPtr = m_pFirstBlk;
  char Buff[1024];
  while (m_pIPtr)
    {
    if (m_bDoDebug)
      {
      pCPGMDbg pDbg = *((CPGMDbg**)m_pcIPtr);
      dbgp("%c%3d)", (pDbg->bBreakLine ? 'B' : ' '), pDbg->iLineNo);
      m_pcIPtr += sizeof(pCPGMDbg);
      }
    if (dbgMemIsFirstBlk)
      dbgp("  %6d", (int)((char*)m_pIPtr-(char*)m_pFirstBlk));
    else
      dbgp("        ");
    dbgpln("  %s", m_pIPtr->GetDesc(Buff));
    m_pIPtr->Advance(*this);
    }
  #endif
  }

//---------------------------------------------------------------------------

void GCInsMngr::DbgVarList()
  {//list of all vars...
  #if dbgGenCon
  if (dbgVars())
    {
    dbgpln("----------------------\nPGM Variable list: %s", m_pXRM->GetOwnerTag());
    pGCVar pVar = m_pVarList;
    char Buff[768];
    while (pVar)
      {
      if (pVar->WhatAmI() != VarClass || (pVar->m_flags & VarClassDefn)==0)
        {
        dbgpln(pVar->GetDesc(Buff));
        if (pVar->WhatAmI() == VarClass)
          {
          pGCVar pVar2 = ((pGCClassVar)pVar)->m_pVarList;
          while (pVar2)
            {
            dbgpln("  %s.%s", pVar->Name(), pVar2->GetDesc(Buff));
            pVar2 = pVar2->Next();
            }
          }
        }
      pVar = pVar->Next();
      }
    }
  #endif
  }

//---------------------------------------------------------------------------

void GCInsMngr::Err(char Code, int errNo, pchar ErrMsg)
  {
  Strng errDesc;
  switch (errNo)
    {
    // execution errors... 1  to 49
    case 1  : errDesc = "Not all control paths in function have a valid RETURN."; break;
    case 2  : errDesc = "Function stack overflow. (function called recursively?)"; break;
    case 3  : errDesc = "Calculation stack overflow."; break;
    case 4  : errDesc = "Function reference stack overflow."; break;
    case 5  : errDesc = "PGM class execution error."; break;
    case 6  : errDesc = "String stack overflow."; break;
    case 7  : errDesc = "SetDynTag references unknown tag."; break;
    case 8  : errDesc = "GetDynTag references unknown tag."; break;
    case 9  : errDesc = "Tag not specified for SetDynTag."; break;
    case 10 : errDesc = "Tag not specified for GetDynTag."; break;
    case 11 : errDesc = "Conversion unit invalid for SetDynTag."; break;
    case 12 : errDesc = "Conversion unit invalid for GetDynTag."; break;
    case 13 : errDesc = "Tag cannot be set for SetDynTag."; break;
    case 14 : errDesc = "Tag for SetDynStrTag must be a string."; break;
    case 15 : errDesc = "Tag for GetDynStrTag must be a string."; break;
    case 16 : errDesc = "Execution Timeout : Possible end-less loop."; break;
    case 17 : errDesc = "Invalid index in array."; break;
    // math errors... 50  to 99
    case 50 : errDesc = "Division by zero."; break;
    case 51 : errDesc = "Cannot find square root of a negative number."; break;
    case 52 : errDesc = "Cannot raise a negative number to a non-integer power."; break;
    case 53 : errDesc = "Cannot find the log or ln of zero or a negative number."; break;
    case 54 : errDesc = "Cannot raise zero to a negative power."; break;
    //load errors 100 to 200
    case 100 : errDesc = "Valid datatype expected."; break;
    case 101 : errDesc = "Alphanumeric expected."; break;
    case 102 : errDesc = "Unique identifier expected."; break;
    case 103 : errDesc = "'=' expected."; break;
    case 104 : errDesc = "Function open parenthesis expected '('."; break;
    case 105 : errDesc = "Unexpected identifier."; break;
    case 106 : errDesc = "Function parameter delimiter expected ','."; break;
    case 107 : errDesc = "Function close parenthesis expected ')'."; break;
    case 108 : errDesc = "Function returning a string, Str or string expected."; break;
    case 109 : errDesc = "Invalid symbol"; break;
    case 110 : errDesc = "End of file '$' not expected."; break;
    case 111 : errDesc = "'If' expected before 'Elseif'."; break;
    case 112 : errDesc = "Cannot have 'Elseif' after 'Else'."; break;
    case 113 : errDesc = "'If' expected before 'Else'."; break;
    case 114 : errDesc = "'If' expected before 'Endif'."; break;
    case 115 : errDesc = "'Begin' expected before 'End'."; break;
    case 116 : errDesc = "'While' expected before 'EndWhile'."; break;
    case 117 : errDesc = "Valid LHS identifier expected."; break;
    case 118 : errDesc = "Must reference a valid variable or function of a class."; break;
    case 119 : errDesc = "Cannot assign value to a constant."; break;
    case 120 : errDesc = "Function returning a string not allowed"; break;
    case 121 : errDesc = "Variable identifier undefined."; break;
    case 122 : errDesc = "Valid expression expected."; break;
    case 123 : errDesc = "Invalid symbol at start of expression."; break;
    case 124 : errDesc = "Cannot watch this variable."; break;
    case 125 : errDesc = "Exceeded maximum number of function parameters."; break;
    case 126 : errDesc = "Cannot define local functions."; break;
    case 127 : errDesc = "Cannot define local classes."; break;
    case 128 : errDesc = "Tag enclosed in quotes (\") expected."; break;
    case 129 : errDesc = "Reserved symbol not expected."; break;
    case 130 : errDesc = "Reserved symbol not allowed in this context."; break;
    case 131 : errDesc = "Cannot follow a class instance with *,@ or (."; break;
    case 132 : errDesc = "EndClass, class type or data type expected."; break;
    case 133 : errDesc = "Must refer to a valid class instance."; break;
    case 134 : errDesc = "Valid variable or symbol expected."; break;
    case 135 : errDesc = "File error with include file (Cannot open?)."; break;
    case 136 : errDesc = "Cannot set WatchInfo for this variable type."; break;
    case 137 : errDesc = "Numerical value expected (egs 3.2e-5.0; -2.7; 0.2E4)."; break;
    case 138 : errDesc = "Comparison operator '==' expected."; break;
    case 139 : errDesc = "Comparison operator '!=' expected."; break;
    case 140 : errDesc = "'EndIf' expected."; break;
    case 141 : errDesc = "'EndWhile' expected."; break;
    case 142 : errDesc = "Str variable not expected."; break;
    case 143 : errDesc = "Function returning a Str variable not expected."; break;
    case 144 : errDesc = "Too many nested GetTag/SetTag functions."; break;
    case 145 : errDesc = "Tag expected, not a 'blank'."; break;
    case 146 : errDesc = "Valid conversion family name expected."; break;
    case 147 : errDesc = "Valid conversion unit expected."; break;
    case 148 : errDesc = "Cannot set CnvInfo for this variable type."; break;
    case 149 : errDesc = "CnvInfo already set for this variable."; break;
    case 150 : errDesc = "Variable with conversion information expected."; break;
    case 151 : errDesc = "Cannot set this variable as read-only."; break;
    case 152 : errDesc = "Cannot reference tag directly, use GetTag(...)."; break;
    case 153 : errDesc = "Cannot find closing symbol for tag."; break;
    case 154 : errDesc = "'Return' is only valid in a function."; break;
    case 155 : errDesc = "'EndFunct' is only valid in a function."; break;
    case 156 : errDesc = "'Return' expected before 'EndFunct'."; break;
    case 157 : errDesc = "Bad Cable Definition. - In/Out missing"; break;
    case 158 : errDesc = "Bad Cable Definition. - Bad Cable Name"; break;
    case 159 : errDesc = "Bad Cable Definition. - Bad Format"; break;
    case 160 : errDesc = "Positive integer value expected"; break;
    case 161 : errDesc = "'[' expected"; break;
    case 162 : errDesc = "']' expected"; break;
    case 163 : errDesc = "',' expected"; break;
    case 164 : errDesc = "Cannot follow an array declaration with *,@ or (."; break;
    case 165 : errDesc = "Cannot declare an array of this data type."; break;
    case 166 : errDesc = "Cannot declare an array here."; break;
    case 167 : errDesc = "Array index is not in range."; break;
    case 168 : errDesc = "Cannot exclude watch of this variable."; break;
    case 169 : errDesc = "Exceeded maximum number of PageLabels allowed."; break;
    default: errDesc = "Unknown Error Number";
    };

  char Buff0[1024];
  switch (Code)
    {
    case ErrBuild: sprintf(Buff0, "Build error %d", errNo); break;
    case ErrMathRuntime: sprintf(Buff0, "Math runtime error %d", errNo); break;
    case ErrOtherRuntime: sprintf(Buff0, "Runtime error %d", errNo); break;
    default: m_StateLine[0] = "Error"; break;
    }
  m_StateLine[0] = Buff0;
  m_StateLine[1] = errDesc;
  m_StateLine[2] = "";
  m_StateLine[3] = "";
  m_StateLine[4] = "";
  int Line = 2;
  if (ErrMsg && strlen(ErrMsg)>0)
    m_StateLine[Line++] = ErrMsg;
  if (Code == ErrBuild)
    {
    if (m_pH->m_pTknParser)
      {
      if (m_pH->m_pTknParser->ParsingFile() && m_pH->m_pTknParser->GetFileIndex())
        {
        Strng s1,s2;
        s2 = s1.FnName((char*)(m_pH->m_pTknParser->GetFileName(m_pH->m_pTknParser->GetFileIndex())));
        s2 += s1.FnExt((char*)(m_pH->m_pTknParser->GetFileName(m_pH->m_pTknParser->GetFileIndex())));
        sprintf(Buff0, "Line:%d  File:%s  Symbol:%s", m_pH->GetCurLineNo(), s2(), m_pH->m_pTknParser->GetCurToken());
        }
      else
        sprintf(Buff0, "Line:%d  Symbol:%s", m_pH->GetCurLineNo(), m_pH->m_pTknParser->GetCurToken());
      m_StateLine[Line++] = Buff0;
      sprintf(Buff0, "%s", m_pH->m_pTknParser->GetWholeLine());
      m_StateLine[Line++] = Buff0;
      }
    //compile error: FORCE EOF ins on code stack
    GCEOF.Construct(*this); //cannot navigate ins code without it !!!
    }

  dbgpln("PGM ERROR %c %s", Code, m_StateLine[1]);

  switch (Code)
    {
    case ErrMathRuntime: //KGA:TODO what to do for runtime errors ????
      m_iMathRunTimeErr++;
      break;
    case ErrOtherRuntime: //KGA:TODO what to do for runtime errors ????
      if (errNo<5 || errNo==6)
        {
        sprintf(Buff0, "%s : %s\n%s\n%s\nWill attempt to continue, SysCAD may CRASH !", m_pXRM->GetOwnerTag(), m_StateLine[0](), m_StateLine[1](), (m_StateLine[2]() ? m_StateLine[2]() : ""));
        //LogError(m_pXRM->GetOwnerTag(), 0, Buff0);
        AfxMessageBox(Buff0);
        }
      else
        {
        if (m_sFnExtn())
          LogError(m_pXRM->GetOwnerTag(), 0, "%s  (See model using %s code for more details)", m_StateLine[0](), m_sFnExtn());
        else
          LogError(m_pXRM->GetOwnerTag(), 0, "%s", m_StateLine[0]());
        LogNote(m_pXRM->GetOwnerTag(), 0, "%s", m_StateLine[1]());
        if (m_StateLine[2]())
          LogNote(m_pXRM->GetOwnerTag(), 0, "%s", m_StateLine[2]());
        }
      m_iRunTimeErr++;
      break;
    case ErrBuild:
      {
      if (m_bLogBuildErrs)
        {
        //LogError(m_pXRM->GetOwnerTag(), 0, "%s", m_StateLine[0]);
        LogError(m_pXRM->GetOwnerTag()?m_pXRM->GetOwnerTag():"?", 0, "%s  (See model using %s code for more details)", m_StateLine[0], m_sFnExtn());
        //LogNote(m_pXRM->GetOwnerTag(), 0, "%s", m_StateLine[1]());
        for (int i=1; i<(m_bLogAllBuildErrs ? 5 : 2); i++)
          if (m_StateLine[i]())
          LogNote(m_pXRM->GetOwnerTag(), 0, "%s", m_StateLine[i]());
        }

      GCException* theE = new GCException;
      theE->m_ErrNo = errNo;
      THROW(theE);
      break;
      }
    default:
      ASSERT(FALSE);//should never get here
      break;
    }
  }

//---------------------------------------------------------------------------

void GCInsMngr::StdCheck(pchar Tkn)
  {
  pGCDef pDef = GCDef::Find(Tkn);
  if (pDef)
    Err(ErrBuild, 129);
  //if (!(pDef->m_defFlags & m_iInsSet))
  //  Err(ErrBuild, 130);
  }

//---------------------------------------------------------------------------

double GCInsMngr::ParseNumber()
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
  double sign = 1.0;
  if (Tkn[0]=='-')
    {
    sign = -1.0;
    strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    }
  if (m_pH->m_pTknParser->TokType() != TokNumeric)
    Err(ErrBuild, 137);
  if (toupper(Tkn[strlen(Tkn)-1])=='E')
    {
    char Tkn2[MaxTknMemSize];
    Tkn2[MaxTknSize] = 0;
    strncpy(Tkn2, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if (Tkn2[0]!='-')
      Err(ErrBuild, 137);
    strcat(Tkn, Tkn2);
    strncpy(Tkn2, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if (m_pH->m_pTknParser->TokType() != TokNumeric)
      Err(ErrBuild, 137);
    strcat(Tkn, Tkn2);
    }
  if (strspn(Tkn, "01234567890.-eE") != strlen(Tkn))
    Err(ErrBuild, 137);
  char* pEPos = strchr(Tkn, 'e');
  if (pEPos==NULL)
    pEPos = strchr(Tkn, 'E');
  if (pEPos && strspn(&pEPos[1], "01234567890-") != strlen(&pEPos[1]))
    Err(ErrBuild, 137);

  double d = atof(Tkn);
  d *= sign;
  return d;
  }

//---------------------------------------------------------------------------

void GCInsMngr::AddArrayWatch(pGCClassVar pClassVar, bool ReadOnly, int index0, int index1)
  {
  const long Len = m_ExtraWatchArray.GetSize();
  GCArrayWatch* pAW = NULL;
  for (int i=0; i<Len && pAW==NULL; i++)
    {
    GCArrayWatch* p = m_ExtraWatchArray[i];
    if (p->m_pClassVar==pClassVar && p->m_iIndex0==index0 && p->m_iIndex1==index1)
      {//already have this listed
      pAW = p;
      }
    }
  if (pAW==NULL)
    {//add it
    m_ExtraWatchArray.SetSize(Len+1, 32);
    pAW = new GCArrayWatch(pClassVar, ReadOnly, index0, index1);
    m_ExtraWatchArray[Len] = pAW;
    }
  }

//---------------------------------------------------------------------------

void GCInsMngr::DoConstructArray(rGCDef Def, pGCVar &pVarList, GCVarMap * pVarMap, pchar pVarName, int Cnt)
  {
  GCClassVar* pClassVar = NULL;
  if (&Def==&GCClassDec)
    pClassVar = GCClassDec.m_pClassVar;//(GCClassDec*)(&Def)->m_pClassVar;
  GCArrayVar* pArrayVar = new GCArrayVar(pVarList, pVarMap, pVarName, 0, pClassVar, Cnt);
  Strng Tg;
  for (int i=0; i<Cnt; i++)
    {
    Tg.Set("%s[%d]", pVarName, i);
    GCVar* pVar = Def.ConstructVar(pVarList, pVarMap, Tg(), 0);
    pArrayVar->m_pVars[i] = pVar;
    }
  }

//---------------------------------------------------------------------------

void GCInsMngr::DoConstructVars(rGCDef Def, pGCVar &pVarList, GCVarMap * pVarMap)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  GCVar* pVar;
  flag Valid = TRUE;
  while (Valid)
    {
    strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if (m_pH->m_pTknParser->TokType() != TokAlpha)
      Err(ErrBuild, 101);
    pVar = pVarList ? pVarList->Find(Tkn) : NULL;
    if (pVar)
      Err(ErrBuild, 102);
    StdCheck(Tkn);
    Strng VarName(Tkn);
    strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    flag IsArray = (strcmp(Tkn,"[")==0);
    if (IsArray)
      {
      if (&Def!=&GCClassDec)
        Err(ErrBuild, 165);
      if (pVarList!=m_pVarList)
        Err(ErrBuild, 166);
      strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
      if (m_pH->m_pTknParser->TokType() != TokNumeric)
        Err(ErrBuild, 160);
      int Cnt = Max(1, atoi(Tkn));
      strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
      if (strcmp(Tkn,"]") != 0)
        Err(ErrBuild, 162);
      DoConstructArray(Def, pVarList, pVarMap, VarName(), Cnt);
      pVar = NULL;
      strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
      }
    else
      pVar = Def.ConstructVar(pVarList, pVarMap, VarName(), 0);
    flag SpecTkn = ((strcmp(Tkn,"*")==0) || (strcmp(Tkn,"@")==0) || (strcmp(Tkn,"(")==0));
    if (IsArray && SpecTkn)
      Err(ErrBuild, 164);
    if ((strcmp(Tkn,",")!=0) && !(Def.m_defFlags & DefDataType) && SpecTkn)
      Err(ErrBuild, 131);
    while (SpecTkn)
      {
      switch (Tkn[0])
        {
        case '*':
          if (/*m_pH->m_pClassVar || */m_pH->m_pFunctVar)
            Err(ErrBuild, 124);
          pVar->m_flags |= VarPgmWatch;
          break;
        case '@':
          if (/*m_pH->m_pClassVar || */m_pH->m_pFunctVar)
            Err(ErrBuild, 151);
          pVar->m_flags |= (VarPgmWatch|VarReadOnly);
          break;
        case '(':
          if (/*m_pH->m_pClassVar || */m_pH->m_pFunctVar)
            Err(ErrBuild, 148);
          if (pVar->WhatAmI()!=VarDouble && pVar->WhatAmI()!=VarLong)
            Err(ErrBuild, 148);
          DoConstructVarCnv(pVar);
          break;
        }
      strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
      SpecTkn = ((strcmp(Tkn,"*")==0) || (strcmp(Tkn,"@")==0) || (strcmp(Tkn,"(")==0));
      }
    if (strcmp(Tkn,",") != 0)
      Valid = FALSE;
    }
  m_pH->m_pTknParser->SetReturnSameToken();
  }

//---------------------------------------------------------------------------

pGCFunctVar GCInsMngr::DoConstructFnt(pGCVar &pVarList, GCVarMap * pVarMap)
  {
  if (m_pH->m_pFunctVar)
    Err(ErrBuild, 126);
  pGCVar ParmList[MAX_FUNCT_PARMS];
  //flag ByRefList[MAX_FUNCT_PARMS];
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  //get function name...
  strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (m_pH->m_pTknParser->TokType() != TokAlpha)
    Err(ErrBuild, 101);
  pGCVar pVar = pVarList->Find(Tkn);
  if (pVar)
    Err(ErrBuild, 102);
  StdCheck(Tkn);
  pGCFunctVar pFntVar = (pGCFunctVar)GCFunct.ConstructVar(pVarList, pVarMap, Tkn, 0);
  m_pH->m_pFunctVar = pFntVar;
  //m_pTempVar = (pGCVar)pFntVar; //??????
  //get parameters...
  strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != '(')
    Err(ErrBuild, 104);
  int parms = 0;
  flag Valid = True;
  while (Valid)
    {
    strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if (Tkn[0] == ')')
      Valid = FALSE;
    else
      {
      if (parms>=MAX_FUNCT_PARMS)
        Err(ErrBuild, 125);
      pGCDef pDef = GCDef::Find(Tkn);
      if (pDef == NULL)
        Err(ErrBuild, 100);
      if (!(pDef->m_defFlags & DefDataType))
        Err(ErrBuild, 100);
      strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
      /*flag ByRef = False;
      if (Tkn[0] == '&')
        {
        ByRef = True;
        strncpy(Tkn, m_pTknParser->NextToken(), MaxTknSize);
        }*/
      if (m_pH->m_pTknParser->TokType() != TokAlpha)
        Err(ErrBuild, 101);
      pGCVar pVar = pFntVar->m_pVarList->Find(Tkn);
      if (pVar)
        Err(ErrBuild, 102);
      StdCheck(Tkn);
      pVar = pDef->ConstructVar(pFntVar->m_pVarList, &pFntVar->m_VarMap, Tkn, 0);
      ParmList[parms] = pVar;
      //ByRefList[parms] = ByRef;
      parms++;
      strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
      if (strcmp(Tkn, ",") != 0)
        Valid = FALSE;
      }
    }
  ((pGCFunctVar)pFntVar)->m_Parms = parms;
  m_pH->m_bEqualFlag = True; //flag set to prevent construct expression from being called
  for (int i=parms-1; i>=0; i--)
    {
    pGCIns pIns = GCEqual.Construct(*this);
    ((pGCEqualIns)pIns)->m_pVar = ParmList[i];
    }
  m_pH->m_bEqualFlag = False;
  return pFntVar;
  }

//---------------------------------------------------------------------------

void GCInsMngr::DoConstructExprssn(flag ExpectStr)
  {
  short VarIndex = -1;
  VarLocation eVarLoc = VL_NotFound;
  flag ClassArrayBusy = False;
  GCVar* pVar;
  GCIns* pIns;
  GCDef* pDef;
  char Tkn[512];
  Tkn[511] = 0;
  if (ExpectStr)
    {
    strncpy(Tkn, m_pH->m_pTknParser->NextToken(), 511);
    if (m_pH->m_pTknParser->TokType() == TokString)
      {
      pIns = GCConstStr.Construct(*this);
      ((pGCConstStrIns)pIns)->m_Data = Tkn;
      }
    else
      {
      pDef = GCDef::Find(Tkn);
      if (pDef)
        {
        if (pDef->m_defFlags & DefRetStr)
          pDef->Construct(*this);
        else
          Err(ErrBuild, 108);
        }
      else
        {
        pVar = FindVar(Tkn, VarIndex, eVarLoc);
        if (pVar && (pVar->WhatAmI()==VarStr || (pVar->m_flags & VarStrFunct)))
          {
          if (pVar->WhatAmI()==VarStr)
            {
            if (VarIndex>=0)
              {//in class member function assign to class data member
              BOOL HasDot = (strchr(Tkn, '.')!=NULL);
              BOOL DoPushPopClass = (m_pH->m_iPushPopClassDepth>0 && !HasDot) || (m_pH->m_iPushPopClassDepth>0 && eVarLoc==VL_Class && HasDot);
              if (DoPushPopClass)
                {
                GCPushClassRefIns* pPushIns = (GCPushClassRefIns*)GCPushClassRef.Construct(*this);
                pPushIns->m_pClassVar = NULL;
                pPushIns->m_Index = -1;
                pPushIns->m_pArrayVar = NULL;
                }
              pIns = GCVarClassRef.Construct(*this);
              if (HasDot)
                {
                Strng ClassName(Tkn);
                ClassName = ClassName.Left(ClassName.Find('.'));
                m_pH->m_pClassVar->m_pVarList->Find(ClassName(), VarIndex); //look in class scope
                ((pGCVarClassRefIns)pIns)->m_sVarName = Tkn;
                ((pGCVarClassRefIns)pIns)->m_Index = VarIndex;
                }
              else
                {
                ((pGCVarClassRefIns)pIns)->m_sVarName = "";
                ((pGCVarClassRefIns)pIns)->m_Index = VarIndex;
                }
              if (DoPushPopClass)
                GCPopClassRef.Construct(*this);
              }
            else
              {
              pIns = GCVarRef.Construct(*this);
              ((pGCVarRefIns)pIns)->m_pVar = pVar;
              }
            }
          else
            {
            m_pH->m_pTempVar = pVar; //must remember pointer to funct for callfnt construct
            pDef = &GCCallFnt;
            if (strchr(Tkn,'.'))
              {
              short ClassIndex = -1;
              pGCClassVar pClassVar = NULL;
              if (m_pH->m_pFunctVar)
                pClassVar = m_pH->m_pFunctVar->m_pVarList->FindClass(Tkn); //look in function scope
              if (pClassVar==NULL && m_pH->m_pClassVar)
                pClassVar = m_pH->m_pClassVar->m_pVarList->FindClass(Tkn, ClassIndex); //look in class scope
              if (pClassVar==NULL)
                pClassVar = m_pVarList->FindClass(Tkn); //look in global scope
              if (pClassVar==NULL)
                Err(ErrBuild, 133);
              GCPushClassRefIns* pPushIns = (GCPushClassRefIns*)GCPushClassRef.Construct(*this);
              pPushIns->m_pClassVar = pClassVar;
              pPushIns->m_Index = ClassIndex;
              pPushIns->m_pArrayVar = NULL;
              }
            pIns = pDef->Construct(*this);
            if (strchr(Tkn,'.'))
              GCPopClassRef.Construct(*this);
            }
          }
        else if (pVar && pVar->WhatAmI() == VarArray)
          {
          DoConstructClassArrayAccess();
          GCArrayVar* pArrayVar = (GCArrayVar*)pVar;
          strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
          pVar = pArrayVar->m_pClassRefVar->m_pVarList->Find(&Tkn[1], VarIndex); //get the right VarIndex
          if (pVar==NULL)
            Err(ErrBuild, 118);
          if (pVar->WhatAmI()==VarStr || (pVar->m_flags & VarStrFunct))
            {
            GCPushClassRefIns* pPushIns = (GCPushClassRefIns*)GCPushClassRef.Construct(*this);
            pPushIns->m_pClassVar = NULL;
            pPushIns->m_Index = VarIndex;
            pPushIns->m_pArrayVar = pArrayVar;
            if (pVar->WhatAmI() == VarFunct)
              {
              m_pH->m_pTempVar = pVar; //must remember pointer to funct for callfnt construct
              pIns = pDef->Construct(*this);
              }
            else
              {
              pIns = GCVarClassRef.Construct(*this);
              ((pGCVarClassRefIns)pIns)->m_sVarName = "";
              ((pGCVarClassRefIns)pIns)->m_Index = VarIndex;
              }
            GCPopClassRef.Construct(*this);
            }
          else
            Err(ErrBuild, 108);
          }
        else
          Err(ErrBuild, 108);
        }
      }
    return;
    }

  enum infixType
    { Null, IDconst, IDvar, OBrace, CBrace, Operator, Funct, Cmp, Not };
  pGCDef opStack[1024];
  int opStackPos = -1;
  char Tkn2[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  Tkn2[MaxTknSize] = 0;
  infixType infixChar = Null;
  infixType PrevInfixChar;
  flag Pushed;
  flag Valid = TRUE;
  flag First = TRUE;
  strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
  while (Valid)
    {
    PrevInfixChar = infixChar;
    ClassArrayBusy = False;
    pDef = NULL;
    if (m_pH->m_pTknParser->TokType() == TokNumeric)
      {                                //numerical constant...
      pDef = &GCConst;
      infixChar = IDconst;
      }
    else if (m_pH->m_pTknParser->TokType() == TokAlpha)
      {                                // variable or funct expected...
      pVar = FindVar(Tkn, VarIndex, eVarLoc);
      if (pVar)
        {                              // variable or user-funct found
        if (pVar->WhatAmI() == VarTag)
          {
          Err(ErrBuild, 152);
          }
        else if (pVar->WhatAmI() == VarFunct)
          {
          if (pVar->m_flags & VarStrFunct)
            Err(ErrBuild, 143);
          m_pH->m_pTempVar = pVar; //must remember pointer to funct for callfnt construct
          pDef = &GCCallFnt;
          infixChar = Funct;
          }
        else if (pVar->WhatAmI() == VarArray)
          {
          pDef = &GCVarClassRef;
          infixChar = IDvar;
          ClassArrayBusy = True;
          }
        else
          {
          if (pVar->WhatAmI() == VarClass)
            {
            if (pVar->m_flags & VarClassDefn)
              pDef = &GCClass;//will end expression
            else
              Err(ErrBuild, 118);
            }
          else
            pDef = &GCVarRef;
          infixChar = IDvar;
          }
        }
      }
    else
      {
      switch (Tkn[0])
        {
        case '=' :
          strncpy(Tkn2, m_pH->m_pTknParser->NextToken(), MaxTknSize);
          if (Tkn2[0]!='=')
            Err(ErrBuild, 138);
          pDef = &GCCmpEqual;
          infixChar = Cmp;
          break;
        case '>' :
          infixChar = Cmp;
          strncpy(Tkn2, m_pH->m_pTknParser->NextToken(), MaxTknSize);
          if (Tkn2[0] == '=')
            {
            pDef = &GCCmpGreatEqual;
            Tkn[1] = Tkn2[0];
            Tkn[2] = 0;
            }
          else
            {
            pDef = &GCCmpGreat;
            m_pH->m_pTknParser->SetReturnSameToken();
            }
          break;
        case '<' :
          infixChar = Cmp;
          strncpy(Tkn2, m_pH->m_pTknParser->NextToken(), MaxTknSize);
          if (Tkn2[0] == '=')
            {
            pDef = &GCCmpLessEqual;
            Tkn[1] = Tkn2[0];
            Tkn[2] = 0;
            }
          else if (Tkn2[0] == '>')
            {
            pDef = &GCCmpNotEqual;
            Tkn[1] = Tkn2[0];
            Tkn[2] = 0;
            }
          else
            {
            pDef = &GCCmpLess;
            m_pH->m_pTknParser->SetReturnSameToken();
            }
          break;
        case '!' :
          strncpy(Tkn2, m_pH->m_pTknParser->NextToken(), MaxTknSize);
          if (Tkn2[0]!='=')
            Err(ErrBuild, 139);
          infixChar = Cmp;
          pDef = &GCCmpNotEqual;
          Tkn[1] = '>';
          Tkn[2] = 0;
          break;
        case '[' :
          pDef = &GCTag;
          infixChar = IDvar;
          break;
        default:
          break;
        }
      }
    if (pDef == NULL)
      {
      pDef = GCDef::Find(Tkn);
      if (pDef)
        {                              //valid Def found...
        if (!(pDef->m_defFlags & m_pH->m_iInsSet))
          Err(ErrBuild, 130);
        if (pDef->m_defFlags & DefRetStr)
          Err(ErrBuild, 120);
        switch(Tkn[0])
          {
          case '+' :
          case '-' :
            if ( (PrevInfixChar == Operator) ||
                (PrevInfixChar == OBrace) ||
                (PrevInfixChar == Not) ||
                (PrevInfixChar == Cmp) ||
                (First) )
              {
              strncpy(&Tkn[1], m_pH->m_pTknParser->NextToken(), 254);
              if (m_pH->m_pTknParser->TokType() == TokNumeric)
                {                      //numerical constant...
                pDef = &GCConst;
                infixChar = IDconst;
                }
              else
                pDef = NULL;           //forces error
              }
            else
              infixChar = Operator;
            break;
          case '(' :
            infixChar = OBrace;
            break;
          case ')' :
            infixChar = CBrace;
            break;
          default  :                   //valid function expected...
            if (pDef->m_defFlags & DefOperator)
              {
              if ((pDef == &GCBnot) || (pDef == &GCNot))
                infixChar = Not;
              else
                infixChar = Operator;
              }
            else
              infixChar = Funct;
            break;
          }
        }
      }

    flag AssumeTag = False;
    if (pDef==NULL)
      {
      if (Tkn[strlen(Tkn)-1]!=':')
        {
        if (m_pH->m_iInsSet==DefSolverSet)
          {//assume it is an external tag
          pDef = &GCTag;
          infixChar = IDvar;
          AssumeTag = True;
          }
        else
          Err(ErrBuild, 134);
        }
      /*
      dbgpln("Problem: Tkn:%s", Tkn);
      char Buff[222];
      sprintf(Buff, "\"%s\" : Valid Expression expected", Tkn);
      Err(ErrBuild, 122);*/
      }
    if (pDef && (!(pDef->m_defFlags & m_pH->m_iInsSet)))
      Err(ErrBuild, 130);
    Valid = (pDef && pDef->m_defFlags & DefExp) || (ClassArrayBusy);
    if (Valid)
      {
      if (First)
        {
        Valid = ( (infixChar == IDvar) ||
          (infixChar == IDconst) ||
          (infixChar == Funct) ||
          (infixChar == Not) ||
          (infixChar == OBrace) );
        }
      else
        {
        switch (PrevInfixChar)
          {
          case IDvar :
          case IDconst :
          case Funct:
          case CBrace :
            Valid = ( (infixChar == Operator) ||
              (infixChar == CBrace) ||
              (infixChar == Cmp) );
            break;
          case Cmp :
          case Operator :
          case OBrace :
          case Not :
            Valid = ( (infixChar == IDvar) ||
              (infixChar == IDconst) ||
              (infixChar == Funct) ||
              (infixChar == Not) ||
              (infixChar == OBrace) );
            break;
          }
        }
      }
    if ((First) && (!Valid))
      Err(ErrBuild, 123);
    First = FALSE;
    #if dbgGenCon
    if (dbgLoad())
      dbgpln("Tkn:%-35s  Valid:%d  opStackPos:%4d", Tkn, Valid, opStackPos);
    #endif

    if (Valid)
      {
      if (ClassArrayBusy)
        {
        DoConstructClassArrayAccess();
        GCArrayVar* pArrayVar = (GCArrayVar*)pVar;
        strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
        pVar = pArrayVar->m_pClassRefVar->m_pVarList->Find(&Tkn[1], VarIndex); //get the right VarIndex
        if (pVar==NULL)
          Err(ErrBuild, 118);
        if (pVar->WhatAmI() == VarFunct)
          {
          if (pVar->m_flags & VarStrFunct)
            Err(ErrBuild, 143);
          m_pH->m_pTempVar = pVar; //must remember pointer to funct for callfnt construct
          pDef = &GCCallFnt;
          infixChar = Funct;
          }
        GCPushClassRefIns* pPushIns = (GCPushClassRefIns*)GCPushClassRef.Construct(*this);
        pPushIns->m_pClassVar = NULL;
        pPushIns->m_Index = VarIndex;
        pPushIns->m_pArrayVar = pArrayVar;
        }

      switch (infixChar)
        {
        case (IDconst) :
          {
          pIns = GCConst.Construct(*this);
          if (toupper(Tkn[strlen(Tkn)-1])=='E')
            {
            strncpy(Tkn2, m_pH->m_pTknParser->NextToken(), MaxTknSize);
            if (Tkn2[0]!='-')
              Err(ErrBuild, 137);
            strcat(Tkn, Tkn2);
            strncpy(Tkn2, m_pH->m_pTknParser->NextToken(), MaxTknSize);
            if (m_pH->m_pTknParser->TokType() != TokNumeric)
              Err(ErrBuild, 137);
            strcat(Tkn, Tkn2);
            }
          if (strspn(Tkn, "01234567890.-eE") != strlen(Tkn))
            Err(ErrBuild, 137);
          char* pEPos = strchr(Tkn, 'e');
          if (pEPos==NULL)
            pEPos = strchr(Tkn, 'E');
          if (pEPos && strspn(&pEPos[1], "01234567890-") != strlen(&pEPos[1]))
            Err(ErrBuild, 137);
          ((pGCConstIns)pIns)->m_Data = atof(Tkn);
          break;
          }
        case (IDvar)   :
          if (pDef == &GCTag)
            {
            if (AssumeTag)
              {
              pVar = m_pVarList->FindTagByType(Tkn, VarTag);
              if (pVar == NULL)
                pVar = GCTag.ConstructVar(m_pVarList, &m_VarMap, Tkn, 0);
              m_pH->AddToTagStack(*this, pVar, "");
              }
            else
              pVar = DoConstructTag(']');
            pVar->m_flags |= VarTagIn;
            }
          if (pVar->WhatAmI()==VarTag)
            {
            pIns = GCVarTagRef.Construct(*this);
            ((pGCVarTagRefIns)pIns)->m_pVar = (GCTagVar*)pVar;
            ((pGCVarTagRefIns)pIns)->m_sCnvTxt = m_pH->m_sTempCnvTxt[m_pH->m_iTempTagPos--];
            }
          else if (VarIndex>=0)
            {//in class member function assign to class data member
            if (pVar->WhatAmI()==VarStr)
              Err(ErrBuild, 142);
            if (ClassArrayBusy)
              {
              pIns = GCVarClassRef.Construct(*this);
              ((pGCVarClassRefIns)pIns)->m_sVarName = "";
              ((pGCVarClassRefIns)pIns)->m_Index = VarIndex;
              GCPopClassRef.Construct(*this);
              }
            else
              {
              BOOL HasDot = (strchr(Tkn, '.')!=NULL);
              BOOL DoPushPopClass = (m_pH->m_iPushPopClassDepth>0 && !HasDot) || (m_pH->m_iPushPopClassDepth>0 && eVarLoc==VL_Class && HasDot);
              if (DoPushPopClass)
                {
                GCPushClassRefIns* pPushIns = (GCPushClassRefIns*)GCPushClassRef.Construct(*this);
                pPushIns->m_pClassVar = NULL;
                pPushIns->m_Index = -1;
                pPushIns->m_pArrayVar = NULL;
                }
              pIns = GCVarClassRef.Construct(*this);
              if (HasDot)
                {
                Strng ClassName(Tkn);
                ClassName = ClassName.Left(ClassName.Find('.'));
                m_pH->m_pClassVar->m_pVarList->Find(ClassName(), VarIndex); //look in class scope
                ((pGCVarClassRefIns)pIns)->m_sVarName = Tkn;
                ((pGCVarClassRefIns)pIns)->m_Index = VarIndex;
                }
              else
                {
                ((pGCVarClassRefIns)pIns)->m_sVarName = "";
                ((pGCVarClassRefIns)pIns)->m_Index = VarIndex;
                }
              if (DoPushPopClass)
                GCPopClassRef.Construct(*this);
              }
            }
          else
            {
            if (pVar->WhatAmI()==VarStr)
              Err(ErrBuild, 142);
            pIns = GCVarRef.Construct(*this);
            ((pGCVarRefIns)pIns)->m_pVar = pVar;
            }
          break;
        case (Funct) :
          if (ClassArrayBusy)
            {
            pIns = pDef->Construct(*this);
            GCPopClassRef.Construct(*this);
            }
          else
            {
            if (strchr(Tkn, '.'))
              {
              short ClassIndex = -1;
              GCClassVar* pClassVar = NULL;
              if (m_pH->m_pFunctVar)
                pClassVar = m_pH->m_pFunctVar->m_pVarList->FindClass(Tkn); //look in function scope
              if (pClassVar==NULL && m_pH->m_pClassVar)
                pClassVar = m_pH->m_pClassVar->m_pVarList->FindClass(Tkn, ClassIndex); //look in class scope
              if (pClassVar==NULL)
                pClassVar = m_pVarList->FindClass(Tkn); //look in global scope
              if (pClassVar==NULL)
                Err(ErrBuild, 133);
              GCPushClassRefIns* pPushIns = (GCPushClassRefIns*)GCPushClassRef.Construct(*this);
              pPushIns->m_pClassVar = pClassVar;
              pPushIns->m_Index = ClassIndex;
              pPushIns->m_pArrayVar = NULL;
              }
            pIns = pDef->Construct(*this);
            if (strchr(Tkn, '.'))
              GCPopClassRef.Construct(*this);
            }
          break;
        case (OBrace) :
          opStack[++opStackPos] = &GCOBrace;
          break;
        case (CBrace) :
          while ((opStackPos >= 0) && (opStack[opStackPos] != &GCOBrace))
            opStack[opStackPos--]->Construct(*this);
          if (opStackPos < 0)
            Valid = FALSE;
          else
            opStackPos--;
          break;
        default:
          Pushed = FALSE;
          while (!Pushed)
            {
            if ( (opStackPos < 0) ||
                (pDef->m_prec > opStack[opStackPos]->m_prec) )
              {
              opStack[++opStackPos] = pDef;
              Pushed = TRUE;
              }
            else
              opStack[opStackPos--]->Construct(*this);
            }
        }
      }
    if (Valid)
      strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    }
  while(opStackPos >= 0)
    {
    if (opStack[opStackPos] == &GCOBrace )
      Err(ErrBuild, 122);
    else
      opStack[opStackPos--]->Construct(*this);
    }
  m_pH->m_pTknParser->SetReturnSameToken();
  }

//---------------------------------------------------------------------------

pGCVar GCInsMngr::DoConstructTag(char Terminate)
  {
  char Tag[MaxTknMemSize];
  Tag[MaxTknSize] = 0;
  if (Terminate=='"')
    strncpy(Tag, m_pH->m_pTknParser->NextToken(), MaxTknSize);
  else
    {
    const int lineno = m_pH->GetCurLineNo();
    char Tkn[MaxTknMemSize];
    Tkn[MaxTknSize] = 0;
    Tag[0] = 0;
    strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    char OldWhiteSpaceSet[256];
    strcpy(OldWhiteSpaceSet, m_pH->m_pTknParser->GetWhiteSpaceChars());
    m_pH->m_pTknParser->SetWhiteSpace("");
    int len = strlen(Tag);
    while (Tkn[0] != Terminate && len<sizeof(Tag)-1)
      {
      strncpy(&Tag[len], Tkn, sizeof(Tag) - len - 1);
      strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
      len = strlen(Tag);
      }
    m_pH->m_pTknParser->SetWhiteSpace(OldWhiteSpaceSet);
    if (Tkn[0] != Terminate)
      {
      char ErrBuff[256];
      sprintf(ErrBuff, "Expecting '%c' on line %d", Terminate, lineno);
      Err(ErrBuild, 153, ErrBuff);
      }
    }

  Strng WrkTag, WrkCnvTxt;
  TaggedObject::SplitTagCnv(Tag, WrkTag, WrkCnvTxt);

  if (WrkTag.Length()==0)
    Err(ErrBuild, 145);
  pGCVar pVar = m_pVarList->FindTagByType(WrkTag(), VarTag);
  if (pVar == NULL)
    pVar = GCTag.ConstructVar(m_pVarList, &m_VarMap, WrkTag(), 0);
  ASSERT(pVar);
  if (m_sFldName.Length()>0)
    ((GCTagVar*)pVar)->SetFldName(m_sFldName());

  m_pH->AddToTagStack(*this, pVar, WrkCnvTxt()); //MUST remember
  return pVar;
  }

//---------------------------------------------------------------------------

void GCInsMngr::DoConstructParms( int ParmCount,
                                  dword StrParmMask,
                                  flag StartExpected,
                                  flag EndExpected)
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  if (StartExpected)
    {
    strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if (Tkn[0] != '(')
      Err(ErrBuild, 104);
    }
  for (int i=1; i<=ParmCount; i++)
    {
    #if dbgGenCon
    if (dbgLoad())
      dbgpln("Parm:%d", i);
    #endif
    DoConstructExprssn((StrParmMask&0x0001)!=0);
    StrParmMask = StrParmMask>>1;
    if (i < ParmCount || !EndExpected)
      {
      strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
      if (Tkn[0] != ',')
        Err(ErrBuild, 106);
      }
    }
  if (EndExpected)
    {
    strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if (Tkn[0] != ')')
      Err(ErrBuild, 107);
    }
  }

//---------------------------------------------------------------------------

void GCInsMngr::DoConstructVarCnv(pGCVar pVar)
  {
  switch (pVar->WhatAmI())
    {
    case VarDouble:
    case VarLong:
      break; //these are the only variable types supporting conversions
    default:
      Err(ErrBuild, 148);
      break;
    }
  if (pVar->m_pCnvInfo)
    {
    char ErrBuff[256];
    sprintf(ErrBuff, "Variable %s : Conversion family '%s' ,unit '%s'", pVar->Name(), Cnvs[pVar->m_pCnvInfo->Index()]->Fam(), pVar->m_pCnvInfo->Text());
    Err(ErrBuild, 149, ErrBuff);
    }
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
  XStrTrim(Tkn, " ");
  CCnvIndex CnvIndex = gs_CnvsMngr.FindCnv(Tkn);
  if ((CnvIndex)<0)
    Err(ErrBuild, 146);
  CDataCnv* pCnv = NULL;
  strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] == ')')
    {
    pCnv = Cnvs[CnvIndex]->SICnv();
    }
  else
    {
    if (Tkn[0] != ',')
      Err(ErrBuild, 106);
    strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    XStrTrim(Tkn, " ");
    pCnv = Tkn[0] ? Cnvs[CnvIndex]->Find(Tkn) : Cnvs[CnvIndex]->SICnv();
    if (pCnv==NULL)
      {
      char ErrBuff[256];
      sprintf(ErrBuff, "Conversion family : '%s'", Cnvs[CnvIndex]->Fam());
      Err(ErrBuild, 147, ErrBuff);
      }
    strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    if (Tkn[0] != ')')
      Err(ErrBuild, 107);
    }
  if (pCnv==Cnvs[DC_Frac]->SICnv())
    {//Kludge: Special case because of cnv SI cnv "" for fraction.
    pCnv = Cnvs[DC_Frac]->Find("Frac");
    if (pCnv==NULL)
      Cnvs[DC_Frac]->SICnv();
    }
  pVar->AddCnvInfo(CnvIndex, pCnv->Txt());
  }

//---------------------------------------------------------------------------

void GCInsMngr::DoConstructClass()
  {
  if (m_pH->m_pClassVar)
    Err(ErrBuild, 127);
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
  pGCVar pVar = m_pVarList->Find(Tkn);
  if (pVar)
    Err(ErrBuild, 102);
  pGCDef pDef = NULL;
  StdCheck(Tkn);
  GCClassVar* pClassVar = new GCClassVar(m_pVarList, &m_VarMap, Tkn, VarClassDefn);
  m_pH->m_pClassVar = pClassVar; //all functions for this user defined class will know their 'class scope'
  while (pDef != &GCEndClass)
    {
    strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
    pDef = GCDef::Find(Tkn);
    if (pDef==NULL)
      {
      pVar = m_pVarList->Find(Tkn);
      if (pVar &&
          (pVar->WhatAmI() == VarClass) &&
          (pVar->m_flags & VarClassDefn) &&
          (_stricmp(m_pH->m_pClassVar->Name(),pVar->Name())!=0))
        {
        GCClassDec.m_pClassVar = (pGCClassVar)pVar;
        DoConstructVars(GCClassDec, pClassVar->m_pVarList, &pClassVar->m_VarMap); //construct instances of classes
        }
      else
        Err(ErrBuild, 132);
      }
    else
      {
      if (pDef == &GCEndClass)
        break;
      if (pDef->m_defFlags & DefDataType)
        DoConstructVars((*pDef), pClassVar->m_pVarList, &pClassVar->m_VarMap);
      else if (pDef == &GCFunct || pDef == &GCStrFunct)
        pDef->Construct(*this);
      else if (pDef == &GCTextLabel)
        pDef->Construct(*this);
      else
        Err(ErrBuild, 132);
      }
    }
  m_pH->m_pClassVar = NULL;
  }

//---------------------------------------------------------------------------

void GCInsMngr::DoConstructClassArrayAccess()
  {
  char Tkn[MaxTknMemSize];
  Tkn[MaxTknSize] = 0;
  strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (strcmp(Tkn,"[") != 0)
    Err(ErrBuild, 161);
  DoConstructExprssn();
  strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (strcmp(Tkn,"]") != 0)
    Err(ErrBuild, 162);
  strncpy(Tkn, m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (strlen(Tkn)<2 || Tkn[0]!='.')
    Err(ErrBuild, 118);
  m_pH->m_pTknParser->SetReturnSameToken();
  }


//---------------------------------------------------------------------------
//============================ CFnMngr     ==================================
//---------------------------------------------------------------------------

CFnMngr::CFnMngr()
  {
  }

//---------------------------------------------------------------------------

void CFnMngr::Reset(CNodeXRefMngr * pXRM, flag LogBuildErrors/*=False*/)
  {
  m_bLogBuildErrs = LogBuildErrors;
  m_pXRM = pXRM;
  GCInsMngr::Reset();
  }

//---------------------------------------------------------------------------

flag CFnMngr::AddFunct(char* pName, char* pTxt, char*FldName)
  {
  ASSERT(m_pXRM); //Reset should have been called
  if (pTxt==NULL || pName==NULL)
    return False;
  const int len = strlen(pTxt);
  const int nameLen = strlen(pName);
  if (len==0)
    return False;
  char* pBigBuff = new char[len+11+nameLen+nameLen];
  m_sFldName = FldName ? FldName : "";
  sprintf(pBigBuff, "double %s %s%s $", pName, pName, pTxt);
  flag b = LoadFunct(pBigBuff, DefFunctSet);
  delete []pBigBuff;
  m_sFldName = "";
  return b;
  }

//---------------------------------------------------------------------------

GCVar* CFnMngr::GetFunctVar(char* pName)
  {
  //return IM.FindVar(pName, );
  return m_pVarList->Find(pName);
  }

//---------------------------------------------------------------------------
//=============================== Others =====================================
//---------------------------------------------------------------------------

pvoid GCIns::operator new (size_t Size, GCInsMngr & IB)
  {
  ASSERT(IB.m_iFirstInsBlkSize > (int)(sizeof(pCPGMDbg)+Size+sizeof(GCNxtBlkIns)));
  ASSERT(IB.m_iNextInsBlkSize > (int)(sizeof(pCPGMDbg)+Size+sizeof(GCNxtBlkIns)));
  static int BlkBugFix = 0;//128;  //fix BUG by adding 128 ???

  if (IB.m_pCurrentBlk==NULL)
    {//create first instruction block...
    #if dbgGenCon
    if (dbgLoad())
      dbgpln("Construct first PGM Ins block; Size=%d", IB.m_iFirstInsBlkSize + BlkBugFix);
    #endif
    IB.m_pCurrentBlk = new char[IB.m_iFirstInsBlkSize + BlkBugFix];
    memset(IB.m_pCurrentBlk, 0, IB.m_iFirstInsBlkSize + BlkBugFix);
    IB.m_pFirstBlk = IB.m_pCurrentBlk;
    IB.m_pcIPtr = IB.m_pCurrentBlk;
    }

  if (!IB.m_pH->m_bDoingNext)
    {
    const int CurBlkSize = (IB.m_pCurrentBlk==IB.m_pFirstBlk ? IB.m_iFirstInsBlkSize : IB.m_iNextInsBlkSize);
    if ((int)(Max(Size, sizeof(GCNxtBlkIns))+sizeof(pCPGMDbg)) >= (CurBlkSize - ((int)(IB.m_pcIPtr - IB.m_pCurrentBlk))))
      {//create another instruction block to link to the end...
      //#if dbgGenCon
      //dbgpln("MEM: Construct next PGM Ins block; Size=%d", IB.m_InsBlkSize + BlkBugFix);
      //#endif
      pchar pNxt = new char[IB.m_iNextInsBlkSize + BlkBugFix];
      memset(pNxt, 0, IB.m_iNextInsBlkSize + BlkBugFix);
      IB.m_pH->m_bDoingNext = 1;
      pvoid p = new (IB) GCNxtBlkIns();
      ((pGCNxtBlkIns)p)->m_pNxt = pNxt;

      IB.m_pH->m_bDoingNext = 0;
      IB.m_pCurrentBlk = pNxt;
      IB.m_pcIPtr = pNxt;
      }
    }

  if (IB.m_bDoDebug)
    {//insert pointer to debug information...
    pCPGMDbg pDbg = new CPGMDbg;
    *((CPGMDbg**)IB.m_pcIPtr) = pDbg;
    pDbg->iLineNo = IB.m_pH->m_iBreakLineNo;
    if (IB.m_pH->m_bBreakLine)
      {
      IB.m_pH->m_bBreakLine = 0;
      pDbg->bBreakLine = 1;
      }
    IB.m_pcIPtr += sizeof(pCPGMDbg);
    }

  if (IB.m_pH->m_bWhileIns)
    {
    VERIFY(IB.m_pH->m_iWhilePos+1<MaxWhileStack); //reached max no of nested while loops
    IB.m_pH->m_WhileStack[++IB.m_pH->m_iWhilePos] = IB.m_pIPtr;
    IB.m_pH->m_bWhileIns = 0;
    }

  //advance the pointer...
  pvoid p = IB.m_pcIPtr;
  IB.m_pcIPtr += Size;
  return p;
  }

//---------------------------------------------------------------------------

GCIns::~GCIns()
  {
  }

//---------------------------------------------------------------------------

#if (_MSC_VER>1100)
void GCIns::operator delete (pvoid pIns, GCInsMngr & IB)
  {//do nothing
  }
#endif

//---------------------------------------------------------------------------

void GCIns::operator delete (pvoid pIns)
  {//do nothing
  }

//===========================================================================

GCDef::GCDef(pchar Name_)
  {
  m_pName  = Name_;
  m_pNext  = m_pFirst;
  m_defFlags = 0;
  m_prec = 0;
  m_pFirst = this;
  }

//---------------------------------------------------------------------------

pGCDef GCDef::Find(pchar pName)
  {
  pGCDef ptr = GCDef::m_pFirst;
  while ((_stricmp(pName,ptr->m_pName) != 0) && (ptr->m_pNext != NULL))
    ptr = ptr->m_pNext;
  if (_stricmp(pName,ptr->m_pName) == 0)
    return ptr;
  return NULL;
  }

pGCDef GCDef::m_pFirst = NULL;
flag GCDef::m_bDoneDefFlags = False;

//==========================================================================
/*flag GCFCBBuff::Try(char *Nm, Strng  &S)
  {
  if (!NameIs(Nm))
    return False;
  S = Buff[strlen(Buff)+1];
  return True;
  };*/

void GCFCBAppendRec(FilingControlBlock &FCB,
                      char Id, char * Name, PVOID pData, dword DataSize)
  {
  dword nBytesWritten;
  GCFCBHead Hd;
  Hd.Id=Id;
  Hd.nTotalLen=word(sizeof(Hd)+strlen(Name)+1+DataSize);
  FCB.WriteFile(&Hd, sizeof(Hd), &nBytesWritten);
  FCB.WriteFile(Name, strlen(Name)+1, &nBytesWritten);
  if (pData && DataSize>0)
    FCB.WriteFile(pData, DataSize, &nBytesWritten);
  };

//==========================================================================

void GCFCBAppendValue(FilingControlBlock &FCB, pGCVar pVar, char* Name)
  {
  switch (pVar->WhatAmI())
    {
    case VarDouble:
      {
      double d=pVar->getD();
      GCFCBAppendRec(FCB, pVar->WhatAmI(), Name, &d, sizeof(d));
      break;
      }
    case VarLong :
      {
      long l=pVar->getL();
      GCFCBAppendRec(FCB, pVar->WhatAmI(), Name, &l, sizeof(l));
      break;
      }
    case VarByte :
      {
      unsigned char b=pVar->getC();
      GCFCBAppendRec(FCB, pVar->WhatAmI(), Name, &b, sizeof(b));
      break;
      }
    case VarBit  :
      {
      flag b=pVar->getB();
      GCFCBAppendRec(FCB, pVar->WhatAmI(), Name, &b, sizeof(b));
      break;
      }
    case VarStr  :
      {
      Strng s=((pGCStrVar)pVar)->getS();
      if (s.Length()>0)
        GCFCBAppendRec(FCB, pVar->WhatAmI(), Name, s(), s.Length()+1);
      else
        {
        char Buff[1];
        Buff[0] = 0;
        GCFCBAppendRec(FCB, pVar->WhatAmI(), Name, Buff, 1);
        }
      break;
      }
    }
  }

extern void GCFCBReadBuff(FilingControlBlock &FCB, GCFCBBuff & Buff)
  {
  dword nBytesRead;
  FCB.ReadFile(&Buff.Hd, sizeof(Buff.Hd), &nBytesRead);
  if (Buff.Hd.nTotalLen-sizeof(Buff.Hd)>0)
    {
    FCB.ReadFile(&Buff.Buff, Buff.Hd.nTotalLen-sizeof(Buff.Hd), &nBytesRead);
    //pName=Buff;
    //pData=&Buff[strlen(pName)+1];
    }
  else
    {
    //pName="";
    //pData=NULL;
    }

  };

//==========================================================================