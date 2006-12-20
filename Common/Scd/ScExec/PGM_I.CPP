//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"                 
#include "gpfuncs.h"
#define  __PGM_I_CPP
#include "pgm_e.h"
#include "vectors.h"
#include "errorlog.h"
#include "optimize.h"
#include "executiv.h"
#include "ndtree.h"
#include "pgm_i.h"

#if WITHDEBUGNEW 
#ifdef new
#undef new
#endif
#endif

//#pragma warning (disable : 4756) // Const Arith overflow Warning

//===========================================================================

_FWDDEF(GCCommaDef)
_FWDDEF(GCCBraceDef)

//---------------------------------------------------------------------------

class GCCommaDef : public GCDef
  {
  public:
  GCCommaDef() : GCDef(",")
    { };
  virtual pGCIns Construct(rGCInsMngr IB)
    { ASSERT(FALSE); return NULL; };
  };

GCCommaDef GCComma;

//===========================================================================

class GCCBraceDef : public GCDef
  {
  public:
  GCCBraceDef() : GCDef(")")
    { m_defFlags = DefExp; };
  virtual pGCIns Construct(rGCInsMngr IB)
    { ASSERT(FALSE); return NULL; };
  };

GCCBraceDef GCCBrace;

//===========================================================================

class GCETagDef : public GCDef
  {
  public:
  GCETagDef() : GCDef("]")
    { m_defFlags = DefExp; };
  virtual pGCIns Construct(rGCInsMngr IB)
    { ASSERT(FALSE); return NULL; };
  };

GCETagDef GCETag;

//---------------------------------------------------------------------------
//======================== 'Binary' Operators ==============================
//---------------------------------------------------------------------------

START_CODE(Band)
MID_CODE(Band,"BAND",DefExp | DefOperator,18)
virtual pGCIns   Construct(rGCInsMngr IB)
  {
  return new(IB) GCBandIns;
  };
END_CODE(Band)

//---------------------------------------------------------------------------

void GCBandIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  IB.m_dCalcStack[IB.m_iCalcPos] = (double)(GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos]) & GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos+1]));
  Advance(IB);
  };


//===========================================================================

START_CODE(Bxor)
MID_CODE(Bxor,"BXOR",DefExp | DefOperator,16)
virtual pGCIns   Construct(rGCInsMngr IB)
  { return new(IB) GCBxorIns; };
END_CODE(Bxor)

//---------------------------------------------------------------------------

void GCBxorIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  IB.m_dCalcStack[IB.m_iCalcPos] = (double)(GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos]) ^ GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos+1]));
  Advance(IB);
  };

//===========================================================================

START_CODE(Bor)
MID_CODE(Bor,"BOR",DefExp | DefOperator,14)
virtual pGCIns   Construct(rGCInsMngr IB)
  { return new(IB) GCBorIns; };
END_CODE(Bor)

//---------------------------------------------------------------------------

void GCBorIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  IB.m_dCalcStack[IB.m_iCalcPos] = (double)(GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos]) | GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos+1]));
  Advance(IB);
  };

//---------------------------------------------------------------------------
//======================== 'Logical' Operators ==============================
//---------------------------------------------------------------------------

START_CODE(And)
MID_CODE(And,"AND",DefExp | DefOperator,12)
virtual pGCIns   Construct(rGCInsMngr IB)
  { return new(IB) GCAndIns; };
END_CODE(And)

//---------------------------------------------------------------------------

inline void GCAndIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  IB.m_dCalcStack[IB.m_iCalcPos] = IB.m_dCalcStack[IB.m_iCalcPos] && IB.m_dCalcStack[IB.m_iCalcPos+1];
  Advance(IB);
  };

//===========================================================================

START_CODE(Xor)
MID_CODE(Xor,"XOR",DefExp | DefOperator,10)
virtual pGCIns   Construct(rGCInsMngr IB)
  { return new(IB) GCXorIns; };
END_CODE(Xor)

//---------------------------------------------------------------------------

void GCXorIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  IB.m_dCalcStack[IB.m_iCalcPos] = ( (IB.m_dCalcStack[IB.m_iCalcPos] || IB.m_dCalcStack[IB.m_iCalcPos+1])
    && (!(IB.m_dCalcStack[IB.m_iCalcPos] && IB.m_dCalcStack[IB.m_iCalcPos+1])));
  Advance(IB);
  };

//===========================================================================

START_CODE(Or)
MID_CODE(Or,"OR",DefExp | DefOperator,8)
virtual pGCIns   Construct(rGCInsMngr IB)
  { return new(IB) GCOrIns; };
END_CODE(Or)

//---------------------------------------------------------------------------

inline void GCOrIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  IB.m_dCalcStack[IB.m_iCalcPos] = IB.m_dCalcStack[IB.m_iCalcPos] || IB.m_dCalcStack[IB.m_iCalcPos+1];
  Advance(IB);
  };


//===========================================================================
//---------------------------------------------------------------------------
//======================== 'Normal' Operators ==============================
//-------------------

START_CODE(Add)
MID_CODE(Add,"+",DefExp | DefOperator,26)
virtual pGCIns   Construct(rGCInsMngr IB)
  { return new(IB) GCAddIns; };
END_CODE(Add)

//---------------------------------------------------------------------------

inline void GCAddIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  IB.m_dCalcStack[IB.m_iCalcPos] += IB.m_dCalcStack[IB.m_iCalcPos+1];
  Advance(IB);
  };

//===========================================================================

START_CODE(Sub);
MID_CODE(Sub,"-",DefExp | DefOperator,26);
virtual pGCIns   Construct(rGCInsMngr IB)
  { return new(IB) GCSubIns; };
END_CODE(Sub);

//---------------------------------------------------------------------------

inline void GCSubIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  IB.m_dCalcStack[IB.m_iCalcPos] -= IB.m_dCalcStack[IB.m_iCalcPos + 1];
  Advance(IB);
  };

//===========================================================================

START_CODE(Mult)
MID_CODE(Mult,"*",DefExp | DefOperator,28)
virtual pGCIns   Construct(rGCInsMngr IB)
  { return new(IB) GCMultIns; };
END_CODE(Mult)

//---------------------------------------------------------------------------

inline void GCMultIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  IB.m_dCalcStack[IB.m_iCalcPos] *= IB.m_dCalcStack[IB.m_iCalcPos + 1];
  Advance(IB);
  };

//===========================================================================

START_CODE(Divide)
MID_CODE(Divide,"/",DefExp | DefOperator,28)
virtual pGCIns   Construct(rGCInsMngr IB)
  { return new(IB) GCDivideIns; };
END_CODE(Divide)

//---------------------------------------------------------------------------

inline void GCDivideIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  if (IB.m_dCalcStack[IB.m_iCalcPos + 1] == 0.0)
    IB.Err(ErrMathRuntime,50);
  IB.m_dCalcStack[IB.m_iCalcPos] /= NZ(IB.m_dCalcStack[IB.m_iCalcPos + 1]);
  Advance(IB);
  };

//===========================================================================

START_CODE(Power)
MID_CODE(Power,"^",DefExp | DefOperator,30)
virtual pGCIns   Construct(rGCInsMngr IB)
  { return new(IB) GCPowerIns; };
END_CODE(Power)

//---------------------------------------------------------------------------

inline void GCPowerIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  if (IB.m_dCalcStack[IB.m_iCalcPos]<0.0 && IB.m_dCalcStack[IB.m_iCalcPos+1]-(double)((long)(IB.m_dCalcStack[IB.m_iCalcPos+1]))!=0.0)
    IB.Err(ErrMathRuntime,52);
  if (IB.m_dCalcStack[IB.m_iCalcPos]==0.0 && IB.m_dCalcStack[IB.m_iCalcPos+1]<0.0)
    IB.Err(ErrMathRuntime,54);
  IB.m_dCalcStack[IB.m_iCalcPos] = pow(IB.m_dCalcStack[IB.m_iCalcPos], IB.m_dCalcStack[IB.m_iCalcPos+1]);
  Advance(IB);
  };

//===========================================================================

START_CODE(Sin)
MID_CODE(Sin,"SIN",DefExp,0)
virtual pGCIns   Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCSinIns;
  };
END_CODE(Sin)

//---------------------------------------------------------------------------

void GCSinIns::Exec(rGCInsMngr IB)
  {
  IB.m_dCalcStack[IB.m_iCalcPos] = sin(IB.m_dCalcStack[IB.m_iCalcPos]);
  //todo trap errors, ranges etc
  Advance(IB);
  };

//===========================================================================

START_CODE(Cos)
MID_CODE(Cos,"COS",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCCosIns;
  };
END_CODE(Cos)

//---------------------------------------------------------------------------

void GCCosIns::Exec(rGCInsMngr IB)
  {
  IB.m_dCalcStack[IB.m_iCalcPos] = cos(IB.m_dCalcStack[IB.m_iCalcPos]);
  //todo trap errors, ranges etc
  Advance(IB);
  };

//===========================================================================

START_CODE(Tan)
MID_CODE(Tan,"TAN",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCTanIns;
  };
END_CODE(Tan)

//---------------------------------------------------------------------------

void GCTanIns::Exec(rGCInsMngr IB)
  {
  IB.m_dCalcStack[IB.m_iCalcPos] = tan(IB.m_dCalcStack[IB.m_iCalcPos]);
  //todo trap errors, ranges etc
  Advance(IB);
  };

//===========================================================================

START_CODE(aTan2)
MID_CODE(aTan2,"ATAN2",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(4);
  return new(IB) GCaTan2Ins;
  };
END_CODE(aTan2)

//---------------------------------------------------------------------------

void GCaTan2Ins::Exec(rGCInsMngr IB)
  {
  IB.m_iCalcPos -= 3;
  IB.m_dCalcStack[IB.m_iCalcPos] = atan2( (IB.m_dCalcStack[IB.m_iCalcPos+1] - IB.m_dCalcStack[IB.m_iCalcPos]),
    (IB.m_dCalcStack[IB.m_iCalcPos+3] - IB.m_dCalcStack[IB.m_iCalcPos+2]) );
  //todo trap errors, ranges etc
  Advance(IB);
  };

//===========================================================================

START_CODE(Pow)
MID_CODE(Pow,"POW",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(2);
  return new(IB) GCPowIns;
  };
END_CODE(Pow)

//---------------------------------------------------------------------------

void GCPowIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  if (IB.m_dCalcStack[IB.m_iCalcPos]<0.0 && IB.m_dCalcStack[IB.m_iCalcPos+1]-(double)((long)(IB.m_dCalcStack[IB.m_iCalcPos+1]))!=0.0)
    IB.Err(ErrMathRuntime,52);
  if (IB.m_dCalcStack[IB.m_iCalcPos]==0.0 && IB.m_dCalcStack[IB.m_iCalcPos+1]<0.0)
    IB.Err(ErrMathRuntime,54);
  IB.m_dCalcStack[IB.m_iCalcPos] = pow(IB.m_dCalcStack[IB.m_iCalcPos], IB.m_dCalcStack[IB.m_iCalcPos+1]);
  Advance(IB);
  }

//===========================================================================

START_CODE(Abs)
MID_CODE(Abs,"ABS",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCAbsIns;
  };
END_CODE(Abs)

//---------------------------------------------------------------------------

void GCAbsIns::Exec(rGCInsMngr IB)
  {
  IB.m_dCalcStack[IB.m_iCalcPos] = fabs(IB.m_dCalcStack[IB.m_iCalcPos]);
  Advance(IB);
  }

//===========================================================================

START_CODE(Sqrt)
MID_CODE(Sqrt,"SQRT",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCSqrtIns;
  };
END_CODE(Sqrt)

//---------------------------------------------------------------------------

void GCSqrtIns::Exec(rGCInsMngr IB)
  {
  if (IB.m_dCalcStack[IB.m_iCalcPos] < 0.0)
    IB.Err(ErrMathRuntime,51);
  IB.m_dCalcStack[IB.m_iCalcPos] = sqrt(Max(0.0, IB.m_dCalcStack[IB.m_iCalcPos]));
  Advance(IB);
  }

//===========================================================================

START_CODE(Exp)
MID_CODE(Exp,"EXP",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCExpIns;
  };
END_CODE(Exp)

//---------------------------------------------------------------------------

void GCExpIns::Exec(rGCInsMngr IB)
  {
  IB.m_dCalcStack[IB.m_iCalcPos] = exp(IB.m_dCalcStack[IB.m_iCalcPos]);
  Advance(IB);
  }

//===========================================================================

START_CODE(Ln)
MID_CODE(Ln,"LN",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCLnIns;
  };
END_CODE(Ln)

//---------------------------------------------------------------------------

void GCLnIns::Exec(rGCInsMngr IB)
  {
  if (IB.m_dCalcStack[IB.m_iCalcPos]<=0.0)
    IB.Err(ErrMathRuntime,53);
  IB.m_dCalcStack[IB.m_iCalcPos] = log(GTZ(IB.m_dCalcStack[IB.m_iCalcPos]));
  Advance(IB);
  }

//===========================================================================

START_CODE(Log)
MID_CODE(Log,"LOG",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCLogIns;
  };
END_CODE(Log)

//---------------------------------------------------------------------------

void GCLogIns::Exec(rGCInsMngr IB)
  {
  if (IB.m_dCalcStack[IB.m_iCalcPos]<=0.0)
    IB.Err(ErrMathRuntime,53);
  IB.m_dCalcStack[IB.m_iCalcPos] = log10(GTZ(IB.m_dCalcStack[IB.m_iCalcPos]));
  Advance(IB);
  }

//===========================================================================

START_CODE(Mod)
MID_CODE(Mod,"MOD",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(2);
  return new(IB) GCModIns;
  };
END_CODE(Mod)

//---------------------------------------------------------------------------

void GCModIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;   
  /*//remainder = a - (floor(a/b)*b)
  double a = fabs(IB.m_dCalcStack[IB.m_iCalcPos]);
  double b = fabs(IB.m_dCalcStack[IB.m_iCalcPos+1]);
  if (b == 0.0)
    IB.Err(ErrMathRuntime,50);
  IB.m_dCalcStack[IB.m_iCalcPos] = a - (floor(a/b)*b);*/
  if (IB.m_dCalcStack[IB.m_iCalcPos+1] == 0.0)
    IB.Err(ErrMathRuntime,50);
  IB.m_dCalcStack[IB.m_iCalcPos] = fmod(IB.m_dCalcStack[IB.m_iCalcPos], NZ(IB.m_dCalcStack[IB.m_iCalcPos+1]));
  Advance(IB);
  }

//===========================================================================

START_CODE(Div)
MID_CODE(Div,"DIV",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(2);
  return new(IB) GCDivIns;
  };
END_CODE(Div)

//---------------------------------------------------------------------------

void GCDivIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;   //quot = floor(a/b)                
  double a = fabs(IB.m_dCalcStack[IB.m_iCalcPos]);
  double b = fabs(IB.m_dCalcStack[IB.m_iCalcPos+1]);
  if (b == 0.0)
    IB.Err(ErrMathRuntime,50);
  IB.m_dCalcStack[IB.m_iCalcPos] = floor(a/NZ(b));
  Advance(IB);
  };

//===========================================================================

START_CODE(Max)
MID_CODE(Max,"MAX",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(2);
  return new(IB) GCMaxIns;
  };
END_CODE(Max)

//---------------------------------------------------------------------------

void GCMaxIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  IB.m_dCalcStack[IB.m_iCalcPos] = Max(IB.m_dCalcStack[IB.m_iCalcPos],IB.m_dCalcStack[IB.m_iCalcPos+1]);
  //todo trap errors, ranges etc
  Advance(IB);
  };

//===========================================================================

START_CODE(Min)
MID_CODE(Min,"MIN",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(2);
  return new(IB) GCMinIns;
  };
END_CODE(Min)

//---------------------------------------------------------------------------

void GCMinIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  IB.m_dCalcStack[IB.m_iCalcPos] = Min(IB.m_dCalcStack[IB.m_iCalcPos],IB.m_dCalcStack[IB.m_iCalcPos+1]);
  //todo trap errors, ranges etc
  Advance(IB);
  };

//===========================================================================

START_CODE(Range)
MID_CODE(Range,"RANGE",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(3);
  return new(IB) GCRangeIns;
  };
END_CODE(Range)

//---------------------------------------------------------------------------

void GCRangeIns::Exec(rGCInsMngr IB)
  {
  IB.m_iCalcPos = IB.m_iCalcPos - 2;
  IB.m_dCalcStack[IB.m_iCalcPos] = Range(IB.m_dCalcStack[IB.m_iCalcPos],IB.m_dCalcStack[IB.m_iCalcPos+1],IB.m_dCalcStack[IB.m_iCalcPos+2]);
  //todo trap errors, ranges etc
  Advance(IB);
  };

//===========================================================================

START_CODE(Trunc)
MID_CODE(Trunc,"TRUNC",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCTruncIns;
  };
END_CODE(Trunc)

//---------------------------------------------------------------------------

void GCTruncIns::Exec(rGCInsMngr IB)
  {
  IB.m_dCalcStack[IB.m_iCalcPos] = (long)(IB.m_dCalcStack[IB.m_iCalcPos]);
  Advance(IB);
  }

//===========================================================================
//note this is "arathmetic round" not "banking round"
START_CODE(Round)
MID_CODE(Round,"ROUND",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCRoundIns;
  };
END_CODE(Round)

//---------------------------------------------------------------------------

void GCRoundIns::Exec(rGCInsMngr IB)
  {
  if (IB.m_dCalcStack[IB.m_iCalcPos]>0)
    IB.m_dCalcStack[IB.m_iCalcPos] = (long)(IB.m_dCalcStack[IB.m_iCalcPos]+0.5);
  else
    IB.m_dCalcStack[IB.m_iCalcPos] = (long)(IB.m_dCalcStack[IB.m_iCalcPos]-0.5);
  Advance(IB);
  }

//===========================================================================

START_CODE(RoundUp)
MID_CODE(RoundUp,"ROUNDUP",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(2);
  return new(IB) GCRoundUpIns;
  };
END_CODE(RoundUp)

//---------------------------------------------------------------------------

void GCRoundUpIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  const long decimals = (long)(IB.m_dCalcStack[IB.m_iCalcPos+1]);
  //IB.m_dCalcStack[IB.m_iCalcPos] = Math::Round(IB.m_dCalcStack[IB.m_iCalcPos], decimals);
  //there MUST be a better way of implementing this...
  if (decimals<16)
    {//do nothing
    Advance(IB);
    return;
    }

  double d;
  switch (decimals)
    {
    case -5: d = 0.00001; break;
    case -4: d = 0.0001; break;
    case -3: d = 0.001; break;
    case -2: d = 0.01; break;
    case -1: d = 0.1; break;
    case  0: d = 1.0; break;
    case  1: d = 10.0; break;
    case  2: d = 100.0; break;
    case  3: d = 1000.0; break;
    case  4: d = 10000.0; break;
    case  5: d = 100000.0; break;
    case  6: d = 1000000.0; break;
    case  7: d = 10000000.0; break;
    case  8: d = 100000000.0; break;
    default: d = Pow(10, decimals); break;
    }
  const double v = IB.m_dCalcStack[IB.m_iCalcPos]*d;
  if (v<0.0)
    {
    const double f = ceil(v);
    if (v-f<-1.0e-12)
      IB.m_dCalcStack[IB.m_iCalcPos] = (f-1)/d;
    else
      IB.m_dCalcStack[IB.m_iCalcPos] = f/d;
    }
  else
    {
    const double f = floor(v);
    if (v>f)
      IB.m_dCalcStack[IB.m_iCalcPos] = (f+1)/d;
    else
      IB.m_dCalcStack[IB.m_iCalcPos] = f/d;
    }
  Advance(IB);
  }

//===========================================================================

START_CODE(Random)
MID_CODE(Random,"RANDOM",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCRandomIns;
  };
END_CODE(Random)

//---------------------------------------------------------------------------

void GCRandomIns::Exec(rGCInsMngr IB)
  {
  IB.m_dCalcStack[IB.m_iCalcPos] = rand()*(IB.m_dCalcStack[IB.m_iCalcPos]/RAND_MAX);
  Advance(IB);
  }

//===========================================================================

START_CODE(IsNAN)
MID_CODE(IsNAN,"ISNAN",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCIsNANIns;
  };
END_CODE(IsNAN)

//---------------------------------------------------------------------------

void GCIsNANIns::Exec(rGCInsMngr IB)
  {
  IB.m_dCalcStack[IB.m_iCalcPos] = (_isnan(IB.m_dCalcStack[IB.m_iCalcPos]) ? dGCtrue : dGCfalse);
  Advance(IB);
  }

//===========================================================================
/*
START_CODE(An_Scale)
MID_CODE(An_Scale,"AN_SCALE",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCAn_ScaleIns;
  };
END_CODE(An_Scale)

//---------------------------------------------------------------------------

void GCAn_ScaleIns::Exec(rGCInsMngr IB)
  {
  --IB.m_iCalcPos;
  //todo
  // do nothing , only remove value off stack
  Advance(IB);
  };
*/
//===========================================================================

START_CODE(Time)
MID_CODE(Time,"TIME",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(0);
  return new(IB) GCTimeIns;
  };
END_CODE(Time)

//---------------------------------------------------------------------------

void GCTimeIns::Exec(rGCInsMngr IB)
  {
  IB.m_dCalcStack[++IB.m_iCalcPos] = IB.m_dIC_Time.Seconds;
  Advance(IB);
  };

//===========================================================================

START_CODE(Deltatime)
MID_CODE(Deltatime,"DELTATIME",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(0);
  return new(IB) GCDeltatimeIns;
  };
END_CODE(Deltatime)

//---------------------------------------------------------------------------

void GCDeltatimeIns::Exec(rGCInsMngr IB)
  {
  IB.m_dCalcStack[++IB.m_iCalcPos] = IB.m_dIC_dTime.Seconds;
  Advance(IB);
  };

//===========================================================================

START_CODE(Beep)
MID_CODE(Beep,"BEEP",DefLHS,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1);
  return new(IB) GCBeepIns;
  };
END_CODE(Beep)

//---------------------------------------------------------------------------

void GCBeepIns::Exec(rGCInsMngr IB)
  {
  IB.m_iCalcPos -= 1;
  for (int i = 0; i <= IB.m_dCalcStack[IB.m_iCalcPos+1]; i++)
    MessageBeep(0xFFFFFFFF);
    //Beep((DWORD)IB.m_dCalcStack[IB.m_iCalcPos+2], (DWORD)IB.m_dCalcStack[IB.m_iCalcPos+3]); // 16356 Hz , each beep lasts for 10ms
  Advance(IB);
  };

//===========================================================================

/*START_CODE(Beep2)
MID_CODE(Beep2,"BEEP2",DefLHS,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(3);
  return new(IB) GCBeep2Ins;
  };
END_CODE(Beep2)

//---------------------------------------------------------------------------

void GCBeep2Ins::Exec(rGCInsMngr IB)
  {
  IB.m_iCalcPos -= 3;
  for (int i = 0; i <= IB.m_dCalcStack[IB.m_iCalcPos+1]; i++)
    Beep((DWORD)IB.m_dCalcStack[IB.m_iCalcPos+2], (DWORD)IB.m_dCalcStack[IB.m_iCalcPos+3]); // 16356 Hz , each beep lasts for 10ms
  Advance(IB);
  };*/

//===========================================================================
// string manipulation functions ...
//===========================================================================

START_CODE(StrCat)
MID_CODE(StrCat,"STRCAT",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(2, True);
  return new(IB) GCStrCatIns;
  };
END_CODE(StrCat)

//---------------------------------------------------------------------------

void GCStrCatIns::Exec(rGCInsMngr IB)
  {
  IB.m_iStrPos--;
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+1];
  Advance(IB);
  };

//===========================================================================

START_CODE(StrCat2)
MID_CODE(StrCat2,"STRCAT2",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(3, True);
  return new(IB) GCStrCat2Ins;
  };
END_CODE(StrCat2)

//---------------------------------------------------------------------------

void GCStrCat2Ins::Exec(rGCInsMngr IB)
  {
  IB.m_iStrPos -= 2;
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+1];
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+2];
  Advance(IB);
  };

//===========================================================================

START_CODE(StrCat3)
MID_CODE(StrCat3,"STRCAT3",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(4, True);
  return new(IB) GCStrCat3Ins;
  };
END_CODE(StrCat3)

//---------------------------------------------------------------------------

void GCStrCat3Ins::Exec(rGCInsMngr IB)
  {
  IB.m_iStrPos -= 3;
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+1];
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+2];
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+3];
  Advance(IB);
  };

//===========================================================================

START_CODE(StrCat4)
MID_CODE(StrCat4,"STRCAT4",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(5, True);
  return new(IB) GCStrCat4Ins;
  };
END_CODE(StrCat4)

//---------------------------------------------------------------------------

void GCStrCat4Ins::Exec(rGCInsMngr IB)
  {
  IB.m_iStrPos -= 4;
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+1];
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+2];
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+3];
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+4];
  Advance(IB);
  };

//===========================================================================

START_CODE(StrCat5)
MID_CODE(StrCat5,"STRCAT5",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(6, True);
  return new(IB) GCStrCat5Ins;
  };
END_CODE(StrCat5)

//---------------------------------------------------------------------------

void GCStrCat5Ins::Exec(rGCInsMngr IB)
  {
  IB.m_iStrPos -= 5;
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+1];
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+2];
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+3];
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+4];
  IB.m_StrStack[IB.m_iStrPos] += IB.m_StrStack[IB.m_iStrPos+5];
  Advance(IB);
  };

//===========================================================================

START_CODE(StrStr)
MID_CODE(StrStr,"STRSTR",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(2, True);
  return new(IB) GCStrStrIns;
  };
END_CODE(StrStr)

//---------------------------------------------------------------------------

void GCStrStrIns::Exec(rGCInsMngr IB)
  {
  IB.m_iStrPos -= 2;
  IB.m_dCalcStack[++IB.m_iCalcPos] = IB.m_StrStack[IB.m_iStrPos+1].Find(IB.m_StrStack[IB.m_iStrPos+2]);
//  IB.m_StrStack[IB.m_iStrPos+1].Find(IB.m_StrStack[IB.m_iStrPos]);
  Advance(IB);
  };

//===========================================================================

START_CODE(StrCmp)
MID_CODE(StrCmp,"STRCMP",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(2, True);
  return new(IB) GCStrCmpIns;
  };
END_CODE(StrCmp)

//---------------------------------------------------------------------------

void GCStrCmpIns::Exec(rGCInsMngr IB)
  {
  IB.m_iStrPos -= 2;
  IB.m_dCalcStack[++IB.m_iCalcPos] = IB.m_StrStack[IB.m_iStrPos+1].XStrCmp(IB.m_StrStack[IB.m_iStrPos+2]);
  Advance(IB);
  }

//===========================================================================

START_CODE(StriCmp)
MID_CODE(StriCmp,"STRICMP",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(2, True);
  return new(IB) GCStriCmpIns;
  };
END_CODE(StriCmp)

//---------------------------------------------------------------------------

void GCStriCmpIns::Exec(rGCInsMngr IB)
  {
  IB.m_iStrPos -= 2;
  IB.m_dCalcStack[++IB.m_iCalcPos] = IB.m_StrStack[IB.m_iStrPos+1].XStrICmp(IB.m_StrStack[IB.m_iStrPos+2]);
  Advance(IB);
  }

//===========================================================================

START_CODE(Left)
MID_CODE(Left,"LEFT",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True, True, False);
  IB.DoConstructParms(1, False, False, True);
  return new(IB) GCLeftIns;
  };
END_CODE(Left)

//---------------------------------------------------------------------------

void GCLeftIns::Exec(rGCInsMngr IB)
  {
  long i = GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos--]);
  IB.m_StrStack[IB.m_iStrPos] = IB.m_StrStack[IB.m_iStrPos].Left(i);
  Advance(IB);
  }

//===========================================================================

START_CODE(Right)
MID_CODE(Right,"RIGHT",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True, True, False);
  IB.DoConstructParms(1, False, False, True);
  return new(IB) GCRightIns;
  };
END_CODE(Right)

//---------------------------------------------------------------------------

void GCRightIns::Exec(rGCInsMngr IB)
  {
  long i = GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos--]);
  IB.m_StrStack[IB.m_iStrPos] = IB.m_StrStack[IB.m_iStrPos].Right(i);
  Advance(IB);
  }

//===========================================================================

START_CODE(Mid)
MID_CODE(Mid,"MID",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True, True, False);
  IB.DoConstructParms(2, False, False, True);
  return new(IB) GCMidIns;
  };
END_CODE(Mid)

//---------------------------------------------------------------------------

void GCMidIns::Exec(rGCInsMngr IB)
  {
  long length = GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos--]);
  long startPosn = GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos--]);
  IB.m_StrStack[IB.m_iStrPos] = IB.m_StrStack[IB.m_iStrPos].Mid(startPosn,length);
  Advance(IB);
  }

//===========================================================================

START_CODE(StrLen)
MID_CODE(StrLen,"STRLEN",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True);
  return new(IB) GCStrLenIns;
  };
END_CODE(StrLen)

//---------------------------------------------------------------------------

void GCStrLenIns::Exec(rGCInsMngr IB)
  {
  IB.m_dCalcStack[++IB.m_iCalcPos] = IB.m_StrStack[IB.m_iStrPos--].Length();
  Advance(IB);
  }

//===========================================================================

START_CODE(StrUpr)
MID_CODE(StrUpr,"STRUPR",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True);
  return new(IB) GCStrUprIns;
  };
END_CODE(StrUpr)

//---------------------------------------------------------------------------

void GCStrUprIns::Exec(rGCInsMngr IB)
  {
//  IB.m_StrStack[IB.m_iStrPos] = IB.m_StrStack[IB.m_iStrPos].Upper();
  IB.m_StrStack[IB.m_iStrPos].Upper();
  Advance(IB);
  }

//===========================================================================

START_CODE(StrLwr)
MID_CODE(StrLwr,"STRLWR",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True);
  return new(IB) GCStrLwrIns;
  };
END_CODE(StrLwr)

//---------------------------------------------------------------------------

void GCStrLwrIns::Exec(rGCInsMngr IB)
  {
  //IB.m_StrStack[IB.m_iStrPos] = IB.m_StrStack[IB.m_iStrPos].Lower();
  IB.m_StrStack[IB.m_iStrPos].Lower();
  Advance(IB);
  }

//===========================================================================

START_CODE(IntStr)
MID_CODE(IntStr,"IntStr",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(2, False);
  return new(IB) GCIntStrIns;
  };
END_CODE(IntStr)

//---------------------------------------------------------------------------

void GCIntStrIns::Exec(rGCInsMngr IB)
  {
  const long length = GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos--]);
  const long integer = GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos--]);
  IB.m_StrStack[++IB.m_iStrPos].Set("%*d",length,integer);
  Advance(IB);
  }

//===========================================================================

START_CODE(IntToStr)
MID_CODE(IntToStr,"IntToStr",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, False);
  return new(IB) GCIntToStrIns;
  };
END_CODE(IntToStr)

//---------------------------------------------------------------------------

void GCIntToStrIns::Exec(rGCInsMngr IB)
  {
  const long integer = GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos--]);
  IB.m_StrStack[++IB.m_iStrPos].Set("%d",integer);
  Advance(IB);
  }

//===========================================================================

START_CODE(StrToInt)
MID_CODE(StrToInt,"StrToInt",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True);
  return new(IB) GCStrToIntIns;
  };
END_CODE(StrToInt)

//---------------------------------------------------------------------------

void GCStrToIntIns::Exec(rGCInsMngr IB)
  {
  Strng s = (IB.m_StrStack[IB.m_iStrPos--]);
  s.Trim(" \t\n");
  IB.m_dCalcStack[++IB.m_iCalcPos] = s.Len()>0 ? atol(s()) : 0.0;
  Advance(IB);
  }

//===========================================================================

START_CODE(FltStr)
MID_CODE(FltStr,"FltStr",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(3, False);
  return new(IB) GCFltStrIns;
  };
END_CODE(FltStr)

//---------------------------------------------------------------------------

void GCFltStrIns::Exec(rGCInsMngr IB)
  {
  const long length = GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos--]);
  const long decimal = GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos--]);
  const double floater = IB.m_dCalcStack[IB.m_iCalcPos--];
  IB.m_StrStack[++IB.m_iStrPos].Set("%*.*f",length,decimal,floater);
  Advance(IB);
  }

//===========================================================================

START_CODE(FltToStr)
MID_CODE(FltToStr,"FltToStr",DefExp | DefRetStr,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, False);
  return new(IB) GCFltToStrIns;
  };
END_CODE(FltToStr)

//---------------------------------------------------------------------------

void GCFltToStrIns::Exec(rGCInsMngr IB)
  {
  const double floater = IB.m_dCalcStack[IB.m_iCalcPos--];
  IB.m_StrStack[++IB.m_iStrPos].Set("%f",floater);
  Advance(IB);
  }

//===========================================================================

START_CODE(StrToFlt)
MID_CODE(StrToFlt,"StrToFlt",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True);
  return new(IB) GCStrToFltIns;
  };
END_CODE(StrToFlt)

//---------------------------------------------------------------------------

void GCStrToFltIns::Exec(rGCInsMngr IB)
  {
  Strng s = (IB.m_StrStack[IB.m_iStrPos--]);
  s.Trim(" \t\n");
  IB.m_dCalcStack[++IB.m_iCalcPos] = s.Len()>0 ? atof(s()) : 0.0;
  Advance(IB);
  }

//===========================================================================

START_CODE(AlphaToNum)
MID_CODE(AlphaToNum,"AlphaToNum",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True);
  return new(IB) GCAlphaToNumIns;
  };
END_CODE(AlphaToNum)

//---------------------------------------------------------------------------

void GCAlphaToNumIns::Exec(rGCInsMngr IB)
  {
  Strng s = (IB.m_StrStack[IB.m_iStrPos--]);
  if (s.Len()>0 && isalpha(s[0]))
    {
    IB.m_dCalcStack[++IB.m_iCalcPos] = (char)toupper(s[0])-(char)'A'+1;
    }
  else
    IB.m_dCalcStack[++IB.m_iCalcPos] = 0.0;
  Advance(IB);
  }

//===========================================================================

START_CODE(IsAlpha)
MID_CODE(IsAlpha,"IsAlpha",DefExp,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True);
  return new(IB) GCIsAlphaIns;
  };
END_CODE(IsAlpha)

//---------------------------------------------------------------------------

void GCIsAlphaIns::Exec(rGCInsMngr IB)
  {
  Strng s = (IB.m_StrStack[IB.m_iStrPos--]);
  const int len = s.Len();
  for (int i=0; i<len; i++)
    {
    if (!isalpha(s[i]))
      break;
    }
  IB.m_dCalcStack[++IB.m_iCalcPos] = (i==len && len>0) ? dGCtrue : dGCfalse;
  Advance(IB);
  }

//===========================================================================

START_CODE(LogNote)
MID_CODE(LogNote,"LogNote",DefLHS,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True);
  return new(IB) GCLogNoteIns;
  };
END_CODE(LogNote)

//---------------------------------------------------------------------------

void GCLogNoteIns::Exec(rGCInsMngr IB)
  {
  char* p = (IB.m_StrStack[IB.m_iStrPos--])();
  PgmLogNote(IB.m_pXRM->GetOwnerTag(), 0, p ? p : "");
  Advance(IB);
  }

//===========================================================================

START_CODE(LogError)
MID_CODE(LogError,"LogError",DefLHS,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True);
  return new(IB) GCLogErrorIns;
  };
END_CODE(LogError)

//---------------------------------------------------------------------------

void GCLogErrorIns::Exec(rGCInsMngr IB)
  {
  char* p = (IB.m_StrStack[IB.m_iStrPos--])();
  PgmLogError(IB.m_pXRM->GetOwnerTag(), 0, p ? p : "");
  Advance(IB);
  }

//===========================================================================

START_CODE(ConditionNote)
MID_CODE(ConditionNote,"ConditionNote",DefLHS,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(2, False, True, False);
  IB.DoConstructParms(1, True, False, True);
  return new(IB) GCConditionNoteIns;
  };
END_CODE(ConditionNote)

//---------------------------------------------------------------------------

void GCConditionNoteIns::Exec(rGCInsMngr IB)
  {
  int CIindex = Range(0L, GetAsLong(IB.m_dCalcStack[--IB.m_iCalcPos]), (long)MaxCondMsgs-1);
  IB.m_CINOn[CIindex] = (GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos+1])!=0);
  --IB.m_iCalcPos;
  IB.m_CINMsgs[CIindex] = (IB.m_StrStack[IB.m_iStrPos--])();
  Advance(IB);
  }

//===========================================================================

START_CODE(ConditionError)
MID_CODE(ConditionError,"ConditionError",DefLHS,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(2, False, True, False);
  IB.DoConstructParms(1, True, False, True);
  return new(IB) GCConditionErrorIns;
  };
END_CODE(ConditionError)

//---------------------------------------------------------------------------

void GCConditionErrorIns::Exec(rGCInsMngr IB)
  {
  int CIindex = Range(0L, GetAsLong(IB.m_dCalcStack[--IB.m_iCalcPos]), (long)MaxCondMsgs-1);
  IB.m_CIEOn[CIindex] = (GetAsLong(IB.m_dCalcStack[IB.m_iCalcPos+1])!=0);
  --IB.m_iCalcPos;
  IB.m_CIEMsgs[CIindex] = (IB.m_StrStack[IB.m_iStrPos--])();
  Advance(IB);
  }

//===========================================================================

START_CODE(LogEvent)
MID_CODE(LogEvent,"LogEvent",DefLHS,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True);
  return new(IB) GCLogEventIns;
  };
END_CODE(LogEvent)

//---------------------------------------------------------------------------

void GCLogEventIns::Exec(rGCInsMngr IB)
  {
  char* p = (IB.m_StrStack[IB.m_iStrPos--])();

  CTNode *pNd=dynamic_cast<CTNode*>(IB.m_pXRM);
  gs_EventLog.LogEvent((pNd ? pNd->FullObjTag():""), (p ? p : ""));

  Advance(IB);
  }

//===========================================================================

START_CODE(MsgBox)
MID_CODE(MsgBox,"MsgBox",DefLHS/* | DefExp*/,0)
virtual pGCIns Construct(rGCInsMngr IB)
  {
  IB.DoConstructParms(1, True);
  return new(IB) GCMsgBoxIns;
  };
END_CODE(MsgBox)

//---------------------------------------------------------------------------

void GCMsgBoxIns::Exec(rGCInsMngr IB)
  {
  //IB.m_dCalcStack[++IB.m_iCalcPos] = AfxMessageBox((IB.m_StrStack[IB.m_iStrPos--])(), MB_OKCANCEL);
  //todo: Put message box back. Watch for thread locks, etc!!!
  PgmLogNote(IB.m_pXRM->GetOwnerTag(), 0, (IB.m_StrStack[IB.m_iStrPos--])());
  Advance(IB);
  }

//===========================================================================
// Some PGM classes used by PMC optimization equations...
//===========================================================================
#ifdef PMC

const short Idf_QuadEqnAddMeas = 1;

//---------------------------------------------------------------------------

GCQuadEqn::GCQuadEqn(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, "QuadEqn", VarClassDefn)
  {
  AddVar(IB, "Output",      &GCDouble, VarConst);
  AddVar(IB, "Scale",       &GCDouble);
  AddVar(IB, "Offset",      &GCDouble);
  AddVar(IB, "Samples",     &GCLong);
  AddVar(IB, "SampleDelay", &GCLong);
  AddVar(IB, "AdjustSO",    &GCLong);
  AddVar(IB, "UseYMeas",    &GCBit);
  AddVar(IB, "MinYValid",   &GCDouble);
  AddVar(IB, "MaxYValid",   &GCDouble);
  AddVar(IB, "A",           &GCDouble);
  AddVar(IB, "B",           &GCDouble);
  AddFunct(IB, "AddMeas", 2, False, Idf_QuadEqnAddMeas);
  }

//---------------------------------------------------------------------------

void GCQuadEqn::Init(pGCClassVar pClassVar)
  {
  pCOptimizer pOpt = new COptimizer();
  pClassVar->m_pSubClass = (pvoid)pOpt;
  pOpt->ChangeModel("Quadratic");
  pClassVar->GetVarByName("Output")->set(pOpt->Output);
  pClassVar->GetVarByName("Scale")->set(pOpt->Scale);
  pClassVar->GetVarByName("Offset")->set(pOpt->Offset);
  pClassVar->GetVarByName("Samples")->set(pOpt->NoSamples);
  pClassVar->GetVarByName("SampleDelay")->set(pOpt->SampleDelay);
  pClassVar->GetVarByName("AdjustSO")->set(pOpt->AdjSO);
  pClassVar->GetVarByName("UseYMeas")->set(pOpt->UseYMeas);
  pClassVar->GetVarByName("MinYValid")->set(pOpt->MinYValid);
  pClassVar->GetVarByName("MaxYValid")->set(pOpt->MaxYValid);
  pClassVar->GetVarByName("A")->set(1.0);
  pClassVar->GetVarByName("B")->set(1.0);
  }
  
//---------------------------------------------------------------------------

void GCQuadEqn::Done(pGCClassVar pClassVar)
  {
  delete (pCOptimizer)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCQuadEqn::CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  pCOptimizer pOpt = (pCOptimizer)pSubClass;
  switch (FunctId)
    {
    case Idf_QuadEqnAddMeas:
      {
      pOpt->Scale = pClassVar->GetVarByName("Scale")->getD();
      pOpt->Offset = pClassVar->GetVarByName("Offset")->getD();
      pOpt->NoSamples = pClassVar->GetVarByName("Samples")->getL();
      pOpt->SampleDelay = pClassVar->GetVarByName("SampleDelay")->getL();
      pOpt->AdjSO = pClassVar->GetVarByName("AdjustSO")->getL();
      pOpt->UseYMeas = pClassVar->GetVarByName("UseYMeas")->getB();
      pOpt->MinYValid = pClassVar->GetVarByName("MinYValid")->getD();
      pOpt->MaxYValid = pClassVar->GetVarByName("MaxYValid")->getD();
      pOpt->Parm[0] = pClassVar->GetVarByName("A")->getD();
      pOpt->Parm[1] = pClassVar->GetVarByName("B")->getD();
      pOpt->SetYMeas(IB.GetDParm());
      pOpt->SetXMeas(IB.GetDParm(), 0);
      pOpt->Estimate();
      pClassVar->GetVarByName("Output")->set(pOpt->Output);
      pClassVar->GetVarByName("Offset")->set(pOpt->Offset);
      pClassVar->GetVarByName("SampleDelay")->set(pOpt->SampleDelay);
      return pOpt->Output;
      }
      break;
    default:
      ASSERT(FALSE);  //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCQuadEqn::OnSave(FilingControlBlock &FCB/*FILE* f*/, pvoid pSubClass)
  {
  }

//---------------------------------------------------------------------------
    
flag GCQuadEqn::OnLoad(FilingControlBlock &FCB/*rCTokenFile f*/, pvoid pSubClass)
  {
  //pCOptimizer pOpt = (pCOptimizer)pSubClass;
  //pOpt->ChangeModel("Quadratic");
  return True;
  }
  
//==========================================================================

const short Idf_CircEqnAddMeas = 1;

//---------------------------------------------------------------------------

GCCircEqn::GCCircEqn(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, "CircLoadEqn", VarClassDefn)
  {
  AddVar(IB, "Output",      &GCDouble, VarConst);
  AddVar(IB, "Scale",       &GCDouble);
  AddVar(IB, "Offset",      &GCDouble);
  AddVar(IB, "Samples",     &GCLong);
  AddVar(IB, "SampleDelay", &GCLong);
  AddVar(IB, "AdjustSO",    &GCLong);
  AddVar(IB, "UseYMeas",    &GCBit);
  AddVar(IB, "MinYValid",   &GCDouble);
  AddVar(IB, "MaxYValid",   &GCDouble);
  AddVar(IB, "Alpha",       &GCDouble);
  AddVar(IB, "Beta",        &GCDouble);
  AddFunct(IB, "AddMeas", 4, False, Idf_CircEqnAddMeas);
  }

//---------------------------------------------------------------------------

void GCCircEqn::Init(pGCClassVar pClassVar)
  {
  pCOptimizer pOpt = new COptimizer();
  pClassVar->m_pSubClass = (pvoid)pOpt;
  pOpt->ChangeModel("CircLoad");
  pClassVar->GetVarByName("Output")->set(pOpt->Output);
  pClassVar->GetVarByName("Scale")->set(pOpt->Scale);
  pClassVar->GetVarByName("Offset")->set(pOpt->Offset);
  pClassVar->GetVarByName("Samples")->set(pOpt->NoSamples);
  pClassVar->GetVarByName("SampleDelay")->set(pOpt->SampleDelay);
  pClassVar->GetVarByName("AdjustSO")->set(pOpt->AdjSO);
  pClassVar->GetVarByName("UseYMeas")->set(pOpt->UseYMeas);
  pClassVar->GetVarByName("MinYValid")->set(pOpt->MinYValid);
  pClassVar->GetVarByName("MaxYValid")->set(pOpt->MaxYValid);
  pClassVar->GetVarByName("Alpha")->set(1.0);
  pClassVar->GetVarByName("Beta")->set(1.0);
  }
  
//---------------------------------------------------------------------------

void GCCircEqn::Done(pGCClassVar pClassVar)
  {
  delete (pCOptimizer)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCCircEqn::CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  pCOptimizer pOpt = (pCOptimizer)pSubClass;
  switch (FunctId)
    {
    case Idf_CircEqnAddMeas:
      {
      pOpt->Scale = pClassVar->GetVarByName("Scale")->getD();
      pOpt->Offset = pClassVar->GetVarByName("Offset")->getD();
      pOpt->NoSamples = pClassVar->GetVarByName("Samples")->getL();
      pOpt->SampleDelay = pClassVar->GetVarByName("SampleDelay")->getL();
      pOpt->AdjSO = pClassVar->GetVarByName("AdjustSO")->getL();
      pOpt->UseYMeas = pClassVar->GetVarByName("UseYMeas")->getB();
      pOpt->MinYValid = pClassVar->GetVarByName("MinYValid")->getD();
      pOpt->MaxYValid = pClassVar->GetVarByName("MaxYValid")->getD();
      pOpt->Parm[0] = pClassVar->GetVarByName("Alpha")->getD();
      pOpt->Parm[1] = pClassVar->GetVarByName("Beta")->getD();
      pOpt->SetYMeas(IB.GetDParm());
      pOpt->SetXMeas(IB.GetDParm(), 2);
      pOpt->SetXMeas(IB.GetDParm(), 1);
      pOpt->SetXMeas(IB.GetDParm(), 0);
      pOpt->Estimate();
      pClassVar->GetVarByName("Output")->set(pOpt->Output);
      pClassVar->GetVarByName("Offset")->set(pOpt->Offset);
      pClassVar->GetVarByName("SampleDelay")->set(pOpt->SampleDelay);
      return pOpt->Output;
      }
      break;
    default:
      ASSERT(FALSE);  //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCCircEqn::OnSave(FilingControlBlock &FCB/*FILE* f*/, pvoid pSubClass)
  {
  }

//---------------------------------------------------------------------------
    
flag GCCircEqn::OnLoad(FilingControlBlock &FCB/*rCTokenFile f*/, pvoid pSubClass)
  {
  //pCOptimizer pOpt = (pCOptimizer)pSubClass;
  //pOpt->ChangeModel("CircLoad");
  return True;
  }
  
//==========================================================================

const short Idf_PartEqnAddMeas = 1;

//---------------------------------------------------------------------------

GCPartEqn::GCPartEqn(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, "PartSizeEqn", VarClassDefn)
  {
  AddVar(IB, "Output",      &GCDouble, VarConst);
  AddVar(IB, "Scale",       &GCDouble);
  AddVar(IB, "Offset",      &GCDouble);
  AddVar(IB, "Samples",     &GCLong);
  AddVar(IB, "SampleDelay", &GCLong);
  AddVar(IB, "AdjustSO",    &GCLong);
  AddVar(IB, "UseYMeas",    &GCBit);
  AddVar(IB, "MinYValid",   &GCDouble);
  AddVar(IB, "MaxYValid",   &GCDouble);
  AddVar(IB, "GnFlw",       &GCDouble);
  AddVar(IB, "PwFlw",       &GCDouble);
  AddVar(IB, "GnPres",      &GCDouble);
  AddVar(IB, "PwPres",      &GCDouble);
  AddVar(IB, "GnRho",       &GCDouble);
  AddVar(IB, "PwRho",       &GCDouble);
  AddVar(IB, "GnTot",       &GCDouble);
  AddFunct(IB, "AddMeas", 4, False, Idf_PartEqnAddMeas);
  }

//---------------------------------------------------------------------------

void GCPartEqn::Init(pGCClassVar pClassVar)
  {
  pCOptimizer pOpt = new COptimizer();
  pClassVar->m_pSubClass = (pvoid)pOpt;
  pOpt->ChangeModel("PartSize");
  pClassVar->GetVarByName("Output")->set(pOpt->Output);
  pClassVar->GetVarByName("Scale")->set(pOpt->Scale);
  pClassVar->GetVarByName("Offset")->set(pOpt->Offset);
  pClassVar->GetVarByName("Samples")->set(pOpt->NoSamples);
  pClassVar->GetVarByName("SampleDelay")->set(pOpt->SampleDelay);
  pClassVar->GetVarByName("AdjustSO")->set(pOpt->AdjSO);
  pClassVar->GetVarByName("UseYMeas")->set(pOpt->UseYMeas);
  pClassVar->GetVarByName("MinYValid")->set(pOpt->MinYValid);
  pClassVar->GetVarByName("MaxYValid")->set(pOpt->MaxYValid);
  pClassVar->GetVarByName("GnFlw")->set(1.0);
  pClassVar->GetVarByName("PwFlw")->set(1.0);
  pClassVar->GetVarByName("GnPres")->set(1.0);
  pClassVar->GetVarByName("PwPres")->set(1.0);
  pClassVar->GetVarByName("GnRho")->set(1.0);
  pClassVar->GetVarByName("PwRho")->set(1.0);
  pClassVar->GetVarByName("GnTot")->set(1.0);
  }
  
//---------------------------------------------------------------------------

void GCPartEqn::Done(pGCClassVar pClassVar)
  {
  delete (pCOptimizer)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCPartEqn::CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  pCOptimizer pOpt = (pCOptimizer)pSubClass;
  switch (FunctId)
    {
    case Idf_PartEqnAddMeas:
      {
      pOpt->Scale = pClassVar->GetVarByName("Scale")->getD();
      pOpt->Offset = pClassVar->GetVarByName("Offset")->getD();
      pOpt->NoSamples = pClassVar->GetVarByName("Samples")->getL();
      pOpt->SampleDelay = pClassVar->GetVarByName("SampleDelay")->getL();
      pOpt->AdjSO = pClassVar->GetVarByName("AdjustSO")->getL();
      pOpt->UseYMeas = pClassVar->GetVarByName("UseYMeas")->getB();
      pOpt->MinYValid = pClassVar->GetVarByName("MinYValid")->getD();
      pOpt->MaxYValid = pClassVar->GetVarByName("MaxYValid")->getD();
      pOpt->Parm[0] = pClassVar->GetVarByName("GnFlw")->getD();
      pOpt->Parm[1] = pClassVar->GetVarByName("PwFlw")->getD();
      pOpt->Parm[2] = pClassVar->GetVarByName("GnPres")->getD();
      pOpt->Parm[3] = pClassVar->GetVarByName("PwPres")->getD();
      pOpt->Parm[4] = pClassVar->GetVarByName("GnRho")->getD();
      if (fabs(pOpt->Parm[4])<1.0e-9)
        {
        pOpt->Parm[4] = 1.0;
        pClassVar->GetVarByName("GnRho")->set(pOpt->Parm[4]);
        }
      pOpt->Parm[5] = pClassVar->GetVarByName("PwRho")->getD();
      pOpt->Parm[6] = pClassVar->GetVarByName("GnTot")->getD();
      pOpt->SetYMeas(IB.GetDParm());
      pOpt->SetXMeas(IB.GetDParm(), 2);
      pOpt->SetXMeas(IB.GetDParm(), 1);
      pOpt->SetXMeas(IB.GetDParm(), 0);
      pOpt->Estimate();
      pClassVar->GetVarByName("Output")->set(pOpt->Output);
      pClassVar->GetVarByName("Offset")->set(pOpt->Offset);
      pClassVar->GetVarByName("SampleDelay")->set(pOpt->SampleDelay);
      return pOpt->Output;
      break;
      }
    default:
      ASSERT(FALSE);  //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCPartEqn::OnSave(FilingControlBlock &FCB/*FILE* f*/, pvoid pSubClass)
  {
  }

//---------------------------------------------------------------------------
    
flag GCPartEqn::OnLoad(FilingControlBlock &FCB/*rCTokenFile f*/, pvoid pSubClass)
  {
  //pCOptimizer pOpt = (pCOptimizer)pSubClass;
  //pOpt->ChangeModel("PartSize");
  return True;
  }
  
//==========================================================================

const short Idf_AveEqnAddMeas = 1;

//---------------------------------------------------------------------------

GCAveEqn::GCAveEqn(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, "AveSizeEqn", VarClassDefn)
  {
  AddVar(IB, "Output",      &GCDouble, VarConst);
  AddVar(IB, "Scale",       &GCDouble);
  AddVar(IB, "Offset",      &GCDouble);
  AddVar(IB, "Samples",     &GCLong);
  AddVar(IB, "SampleDelay", &GCLong);
  AddVar(IB, "AdjustSO",    &GCLong);
  AddVar(IB, "UseYMeas",    &GCBit);
  AddVar(IB, "MinYValid",   &GCDouble);
  AddVar(IB, "MaxYValid",   &GCDouble);
  AddFunct(IB, "AddMeas", 7, False, Idf_AveEqnAddMeas);
  }

//---------------------------------------------------------------------------

void GCAveEqn::Init(pGCClassVar pClassVar)
  {
  pCOptimizer pOpt = new COptimizer();
  pClassVar->m_pSubClass = (pvoid)pOpt;
  pOpt->ChangeModel("AveSize");
  pClassVar->GetVarByName("Output")->set(pOpt->Output);
  pClassVar->GetVarByName("Scale")->set(pOpt->Scale);
  pClassVar->GetVarByName("Offset")->set(pOpt->Offset);
  pClassVar->GetVarByName("Samples")->set(pOpt->NoSamples);
  pClassVar->GetVarByName("SampleDelay")->set(pOpt->SampleDelay);
  pClassVar->GetVarByName("AdjustSO")->set(pOpt->AdjSO);
  pClassVar->GetVarByName("UseYMeas")->set(pOpt->UseYMeas);
  pClassVar->GetVarByName("MinYValid")->set(pOpt->MinYValid);
  pClassVar->GetVarByName("MaxYValid")->set(pOpt->MaxYValid);
  }
  
//---------------------------------------------------------------------------

void GCAveEqn::Done(pGCClassVar pClassVar)
  {
  delete (pCOptimizer)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCAveEqn::CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  pCOptimizer pOpt = (pCOptimizer)pSubClass;
  switch (FunctId)
    {
    case Idf_AveEqnAddMeas:
      {
      pOpt->Scale = pClassVar->GetVarByName("Scale")->getD();
      pOpt->Offset = pClassVar->GetVarByName("Offset")->getD();
      pOpt->NoSamples = pClassVar->GetVarByName("Samples")->getL();
      pOpt->SampleDelay = pClassVar->GetVarByName("SampleDelay")->getL();
      pOpt->AdjSO = pClassVar->GetVarByName("AdjustSO")->getL();
      pOpt->UseYMeas = pClassVar->GetVarByName("UseYMeas")->getB();
      pOpt->MinYValid = pClassVar->GetVarByName("MinYValid")->getD();
      pOpt->MaxYValid = pClassVar->GetVarByName("MaxYValid")->getD();
      pOpt->SetYMeas(IB.GetDParm());
      pOpt->SetXMeas(IB.GetDParm(), 5);
      pOpt->SetXMeas(IB.GetDParm(), 4);
      pOpt->SetXMeas(IB.GetDParm(), 3);
      pOpt->SetXMeas(IB.GetDParm(), 2);
      pOpt->SetXMeas(IB.GetDParm(), 1);
      pOpt->SetXMeas(IB.GetDParm(), 0);
      pOpt->Estimate();
      pClassVar->GetVarByName("Output")->set(pOpt->Output);
      pClassVar->GetVarByName("Offset")->set(pOpt->Offset);
      pClassVar->GetVarByName("SampleDelay")->set(pOpt->SampleDelay);
      return pOpt->Output;
      break;
      }
    default:
      ASSERT(FALSE);  //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCAveEqn::OnSave(FilingControlBlock &FCB/*FILE* f*/, pvoid pSubClass)
  {
  }

//---------------------------------------------------------------------------
    
flag GCAveEqn::OnLoad(FilingControlBlock &FCB/*rCTokenFile f*/, pvoid pSubClass)
  {
  //pCOptimizer pOpt = (pCOptimizer)pSubClass;
  //pOpt->ChangeModel("PartSize");
  return True;
  }

#endif //PMC
//==========================================================================
