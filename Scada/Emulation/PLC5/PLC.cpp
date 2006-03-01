// PLC5.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxtempl.h"
//#include "PLC5Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PLC.h"
#include <math.h>

FILE * dbgf=NULL;

//===========================================================================
//
//
//
//===========================================================================                           ff

long CTknFile::ReadALine(FILE *f)
  {
  fgets(m_Buff, sizeof(m_Buff), f);
  m_LineNo++;
  m_Pos=0;
  m_UsePrv=false;
  m_PrvTkn=NULL;
  m_Len=strlen(m_Buff);
  while (m_Len>0 && m_Buff[m_Len-1]<=' ') m_Len--;
  m_Buff[m_Len]=0;
  // Strip COmments
  char *p=strchr(m_Buff, '%');
  while (p)
    {
    char *q=strchr(p+1, '%');
    if (q)
      {
      int ComLen=q-p+1;
      char T[BigBuffSize];
      strcpy(T, q+1);
      strcpy(p, T);
      m_Len-=ComLen;
      }
    else
      break;
    p=strchr(m_Buff, '%');
    }
  static l=0;
  if (0 && dbgf) fprintf(dbgf,"%i) %s\n",l++,m_Buff);
  return m_Len;
  }

//---------------------------------------------------------------------------

char * CTknFile::StrToken()
  {
  if (m_PrvTkn && m_UsePrv)
    {
    m_UsePrv=false;
    return m_PrvTkn;
    }
  m_UsePrv=false;

  int i=strspn(&m_Buff[m_Pos], " ");
  m_Pos+=i;
  char * p=&m_Buff[m_Pos];
  if (!p || *p==0)
    return NULL;
  char * q=strchr(p, ' ');
  if (q)
    *q=0;
  m_Pos+=q ? q-p+1 : strlen(p);
  m_PrvTkn=p;
  //m_TknType=TT_Numeric;
  //for (q=p; *q; q++)
  //  if (isalpha(*q))
  //    m_TknType=TT_Alpha;
  //  else if  (!isdigit(*q) && (*q!='.' || *q!='+' || *q!='-'))
  //    m_TknType=TT_Other;
  char t=*p;
  if (isalpha(t) || t=='#')
    {
    m_TknType=TT_Code;
    for (q=p+1; *q; q++)
      if (!isalpha(*q))
        {
        m_TknType=TT_Alpha;
        break;
        }
    }
  else if (isdigit(t) || t=='+' || t=='-')
    m_TknType=TT_Numeric;
  else
    m_TknType=TT_Other;

  return p;
  }

//---------------------------------------------------------------------------

char * CTknFile::StrQToken()
  {
  if (m_PrvTkn && m_UsePrv)
    {
    m_UsePrv=false;
    return m_PrvTkn;
    }
  m_UsePrv=false;

  int i=strspn(&m_Buff[m_Pos], " ");
  m_Pos+=i;
  if (m_Buff[m_Pos]==0)
    return NULL;

  char QuoteChar=m_Buff[m_Pos];
  byte GotQuote=QuoteChar=='"' || QuoteChar=='\'';
  if (GotQuote)
    m_Pos++;

  char * p=&m_Buff[m_Pos];

  char * q=GotQuote ? strchr(p, QuoteChar) : strchr(p, ' ');
  if (q)
    *q=0;
  m_Pos+=q ? q-p+1 : strlen(p);

  if (GotQuote)
    m_Pos++;

  m_PrvTkn=p;
  m_TknType=TT_Other;
  return p;
  }

//---------------------------------------------------------------------------

long CTknFile::IntToken()
  {
  if (m_PrvTkn && m_UsePrv)
    {
    m_UsePrv=false;
    return atol(m_PrvTkn);
    }
  m_UsePrv=false;

  int i=strspn(&m_Buff[m_Pos], " ");
  m_Pos+=i;
  char *p=&m_Buff[m_Pos];
  char *q=strchr(&m_Buff[m_Pos], ' ');
  if (q)
    *q=0;
  m_Pos+=q ? q-p+1 : strlen(p);

  while (*p==' ')
    p++;
  m_PrvTkn=p;
  m_TknType=TT_Numeric;
  return atol(p);
  }

//---------------------------------------------------------------------------

float CTknFile::FltToken()
  {
  if (m_PrvTkn && m_UsePrv)
    {
    m_UsePrv=false;
    return (float)atof(m_PrvTkn);
    }
  m_UsePrv=false;

  int i=strspn(&m_Buff[m_Pos], " ");
  m_Pos+=i;
  char *p=&m_Buff[m_Pos];
  char *q=strchr(&m_Buff[m_Pos], ' ');
  if (q)
    *q=0;
  m_Pos+=q ? q-p+1 : strlen(p);

  while (*p==' ')
    p++;
  m_PrvTkn=p;
  m_TknType=TT_Numeric;
  return (float)atof(p);
  }

//===========================================================================
//
//
//
//===========================================================================

static char * StrToken(char* Buff, int Len=-1, int Trim=0)
  {
  char *p=(Len>=0 ? Buff+Len : strchr(Buff, ' '));
  if (p)
    *p=0;
  if (Trim)
    for (p--; p>=Buff && (*p==' '); p--)
      *p=0;
  return Buff;
  }

//---------------------------------------------------------------------------

static long IntToken(char* Buff)
  {
  int i=strspn(Buff, " ");
  Buff+=i;
  char *p=strchr(Buff, ' ');
  if (p)
    *p=0;
  p=Buff;
  while (*p==' ')
    p++;
  return atol(p);
  }

//===========================================================================
//
//
//
//===========================================================================

void CExpression::Parse(CTknFile & Tkns, byte FnMsk)
  {
  CXOperator Def;
  CXOperator opStack[1024];
  CXOperator opSeq[1024];
  int opStackPos = -1;
  int opSeqLen = 0;
  int nAddOps = 0;
  CString Tkn;
  XIF_Types infixChar = XIF_Null;
  XIF_Types PrevInfixChar;
  bool Pushed;
  bool Valid = TRUE;
  bool First = TRUE;
  Tkn=Tkns.StrToken();
  while (Valid && Tkn.GetLength()>0)
    {
    PrevInfixChar = infixChar;
    Def.m_Op = XO_Null;

    for (int i=0; XSymbols[i].m_Txt; i++)
      if (strcmp(Tkn, XSymbols[i].m_Txt)==0)
        break;
    int xx=0;
    // Not a valid Code
    if (XSymbols[i].m_Txt)
      {
      Def.m_Op=XSymbols[i].m_Op;
      Def.m_iSym=i;
      Def.m_Flt=0;
      infixChar = XSymbols[i].m_InFix;
      CheckThat((FnMsk&XSymbols[i].m_FnMsk)!=0, "Unexpected Code in expression");
      }
    else if (Tkns.TypeIs(TT_Numeric))//m_pH->m_pTknParser->TokType() == TokNumeric)
      {                                //numerical constant...
      //pDef = &GCConst;
      if (strchr(Tkn, '.'))
        {
        Def.m_Op = XO_FConst;
        Def.m_Flt=(float)atof(Tkn);
        }
      else
        {
        Def.m_Op = XO_IConst;
        Def.m_Int=atol(Tkn);
        }
      infixChar = XIF_IdConst;
      }
    else if (Tkns.TypeIs(TT_Alpha))
      {                                // variable expected...
      nAddOps++;
      Def.m_Op = XO_Address;
      Def.m_pAdd=new CAddress(m_Plc);
      Def.m_pAdd->Parse((char*)(LPCTSTR)Tkn);
      infixChar = XIF_IdVar;
      }
    else// if (Tkns.TypeIs(TT_Code))
      {
      // Code which is unknown to this expression
      //Tkns.PushToken();
      break;
      }

    bool AssumeTag = false;
    Valid = (Def.m_Op!=XO_Null);
    if (Valid)
      {
      if (First)
        {
        Valid = ( (infixChar == XIF_IdVar) ||
          (infixChar == XIF_IdConst) ||
          (infixChar == XIF_Funct) ||
          (infixChar == XIF_Not) ||
          (infixChar == XIF_OBrace) );
        }
      else
        {
        switch (PrevInfixChar)
          {
          case XIF_Funct:
            Valid = ( (infixChar == XIF_IdConst) ||
              (infixChar == XIF_IdVar));
            break;
          case XIF_IdVar :
          case XIF_IdConst :
          case XIF_CBrace :
            Valid = ( (infixChar == XIF_Operator) ||
              (infixChar == XIF_CBrace) ||
              (infixChar == XIF_Cmp) );
            break;
          case XIF_Cmp :
          case XIF_Operator :
          case XIF_OBrace :
          case XIF_Not :
            Valid = ( (infixChar == XIF_IdVar) ||
              (infixChar == XIF_IdConst) ||
              (infixChar == XIF_Funct) ||
              (infixChar == XIF_Not) ||
              (infixChar == XIF_OBrace) );
            break;
          }
        }
      }
    if ((First) && (!Valid))
      {
      CheckThat(false, "");      //Err(ErrBuild, 123);
      }
    First = FALSE;
    #if dbgGenCon
    if (dbgLoad())
      dbgpln("Tkn:%s  Valid:%d  opStackPos:%d", Tkn, Valid, opStackPos);
    #endif

    if (Valid)
      {
      switch (infixChar)
        {
        case (XIF_IdConst) :
          {
          opSeq[opSeqLen++]=Def;
          break;
          }
        case (XIF_IdVar)   :
          opSeq[opSeqLen++]=Def;
          break;
        case (XIF_Funct) :
          opStack[++opStackPos] = Def;
          break;
        case (XIF_OBrace) :
          opStack[++opStackPos].m_Op = XO_OBrace;
          break;
        case (XIF_CBrace) :
          while ((opStackPos >= 0) && (opStack[opStackPos].m_Op != XO_OBrace))
            opSeq[opSeqLen++]=opStack[opStackPos--];// ???->Construct(*this);
          if (opStackPos < 0)
            Valid = FALSE;
          else
            opStackPos--;
          break;
        default:
          Pushed = FALSE;
          while (!Pushed)
            {
            if ( (opStackPos < 0) || (Def.m_Op > opStack[opStackPos].m_Op) )
              {
              opStack[++opStackPos] = Def;
              Pushed = TRUE;
              }
            else
              opSeq[opSeqLen++]=opStack[opStackPos--];
            }
        }
      }
    if (Valid)
      {
      Extend(Tkn);
      Tkn=Tkns.StrToken();
      }
    }
  while(opStackPos >= 0)
    {
    if (opStack[opStackPos].m_Op == XO_OBrace)
      {
      CheckThat(false, "OpenBrace not Expected");
      }
    else
      opSeq[opSeqLen++]=opStack[opStackPos--];
    }

  m_nOps=opSeqLen;
  m_nAddOps=nAddOps;
  m_pOps=m_nOps>0 ? new CXOperator[m_nOps] : NULL;
  for (int i=0; i<m_nOps; i++)
    m_pOps[i]=opSeq[i];

  if (0 && dbgf)
    {
    fprintf(dbgf, "Expression: ");
    for (int i=0; i<m_nOps; i++)
      switch (m_pOps[i].m_Op)
      {
      case XO_Address:
        fprintf(dbgf, "%s ", m_pOps[i].m_pAdd->Text());
        break;
      case XO_FConst:
        fprintf(dbgf, "%g ", m_pOps[i].m_Flt);
        break;
      case XO_IConst:
        fprintf(dbgf, "%i ", m_pOps[i].m_Int);
        break;
      default:
        if (m_pOps[i].m_iSym<nXSymbols)
          fprintf(dbgf, "%s ", XSymbols[m_pOps[i].m_iSym].m_Txt);
        else
          fprintf(dbgf, "?%i? ", m_pOps[i].m_Op);
        break;
      }

    fprintf(dbgf, "\n");
    }


  Tkns.PushToken();
  };

//---------------------------------------------------------------------------

CAddress* CExpression::AddressFld(int Index)
  {
  int iAdd=0;
  for (int i=0; i<m_nOps; i++)
    {
    if (m_pOps[i].m_Op==XO_Address)
      {
      if (iAdd==Index)
        return m_pOps[i].m_pAdd;
      iAdd++;
      }
    }
  return NULL;
  }

//---------------------------------------------------------------------------

inline TOSPairFlt(CXValue * Stack, int TOS) { return Stack[TOS].IsFlt() || Stack[TOS-1].IsFlt(); };
CXValue CExpression::Evaluate()
  {
  CXValue Stack[64];
  int  iTOS=-1;
  for (int i=0; i<m_nOps; i++)
    {
    switch (m_pOps[i].m_Op)
      {
      case XO_Address:
        if (m_pOps[i].m_pAdd->IsFlt())
          Stack[++iTOS].SetF(m_pOps[i].m_pAdd->FValue());
        else
          Stack[++iTOS].SetI(m_pOps[i].m_pAdd->IValue());
        break;
      case XO_FConst:
        Stack[++iTOS].SetF(m_pOps[i].m_Flt);
        break;
      case XO_IConst:
        Stack[++iTOS].SetI(m_pOps[i].m_Int);
        break;
      case XO_SQR:  Stack[iTOS].SetF((float)sqrt(fabs(Stack[iTOS].F())));  break;
      case XO_LN:   Stack[iTOS].SetF((float)log(Stack[iTOS].F()));    break;
      case XO_LOG:  Stack[iTOS].SetF((float)log10(Stack[iTOS].F())); break;
      case XO_ACS:  Stack[iTOS].SetF((float)acos(Stack[iTOS].F()));  break;
      case XO_ASN:  Stack[iTOS].SetF((float)asin(Stack[iTOS].F()));  break;
      case XO_ATN:  Stack[iTOS].SetF((float)atan(Stack[iTOS].F()));  break;
      case XO_SIN:  Stack[iTOS].SetF((float)sin(Stack[iTOS].F()));   break;
      case XO_COS:  Stack[iTOS].SetF((float)cos(Stack[iTOS].F()));   break;
      case XO_TAN:  Stack[iTOS].SetF((float)tan(Stack[iTOS].F()));   break;
      //case XO_FRD:  break;
      //case XO_TOD:  break;
      //case XO_DEG:  break;
      //case XO_RAD:  break;
      case XO_GT:
        if (TOSPairFlt(Stack, iTOS--))
          Stack[iTOS].SetI(Stack[iTOS].F()>Stack[iTOS+1].F());
        else
          Stack[iTOS].SetI(Stack[iTOS].I()>Stack[iTOS+1].I());
        break;
      case XO_GE:
        if (TOSPairFlt(Stack, iTOS--))
          Stack[iTOS].SetI(Stack[iTOS].F()>=Stack[iTOS+1].F());
        else
          Stack[iTOS].SetI(Stack[iTOS].I()>=Stack[iTOS+1].I());
        break;
      case XO_EQ:
        if (TOSPairFlt(Stack, iTOS--))
          Stack[iTOS].SetI(Stack[iTOS].F()==Stack[iTOS+1].F());
        else
          Stack[iTOS].SetI(Stack[iTOS].I()==Stack[iTOS+1].I());
        break;
      case XO_NE:
        if (TOSPairFlt(Stack, iTOS--))
          Stack[iTOS].SetI(Stack[iTOS].F()!=Stack[iTOS+1].F());
        else
          Stack[iTOS].SetI(Stack[iTOS].I()!=Stack[iTOS+1].I());
        break;
      case XO_LE:
        if (TOSPairFlt(Stack, iTOS--))
          Stack[iTOS].SetI(Stack[iTOS].F()<=Stack[iTOS+1].F());
        else
          Stack[iTOS].SetI(Stack[iTOS].I()<=Stack[iTOS+1].I());
        break;
      case XO_LT:
        if (TOSPairFlt(Stack, iTOS--))
          Stack[iTOS].SetI(Stack[iTOS].F()<Stack[iTOS+1].F());
        else
          Stack[iTOS].SetI(Stack[iTOS].I()<Stack[iTOS+1].I());
        break;
      //case XO_Or:   break;
      //case XO_XOr:  break;
      //case XO_And:  break;
      //case XO_BOr:  break;
      //case XO_BXOr: break;
      //case XO_BAnd: break;

      case XO_Plus:
        if (TOSPairFlt(Stack, iTOS--))
          Stack[iTOS].SetF(Stack[iTOS].F()+Stack[iTOS+1].F());
        else
          Stack[iTOS].SetI(Stack[iTOS].I()+Stack[iTOS+1].I());
        break;
      case XO_Minus:
        if (TOSPairFlt(Stack, iTOS--))
          Stack[iTOS].SetF(Stack[iTOS].F()-Stack[iTOS+1].F());
        else
          Stack[iTOS].SetI(Stack[iTOS].I()-Stack[iTOS+1].I());
        break;
      case XO_Mult:
        if (TOSPairFlt(Stack, iTOS--))
          Stack[iTOS].SetF(Stack[iTOS].F()*Stack[iTOS+1].F());
        else
          Stack[iTOS].SetI(Stack[iTOS].I()*Stack[iTOS+1].I());
        break;
      case XO_Div:
        if (TOSPairFlt(Stack, iTOS--))
          Stack[iTOS].SetF(Stack[iTOS].F()/Stack[iTOS+1].F());
        else
          Stack[iTOS].SetI(Stack[iTOS].I()/Stack[iTOS+1].I());
        break;
      case XO_Exp:
        iTOS--;
        Stack[iTOS].SetF((float)pow(Stack[iTOS].F(), Stack[iTOS+1].F()));
        break;
      //case XO_Not: break;
      //case XO_BNot: break;
      default:
        CheckThat(m_pOps[i].m_Op==XO_Null, "Unimplemented Expression Operator");
        break;
      }
    }

  if (iTOS>=0)
    return Stack[iTOS];
  CXValue X;
  X.SetI(0);
  return X;
/*
enum XO_OpCodes {
  XO_Null,
  XO_Const, XO_Address,

  XO_SQR, XO_LN, XO_LOG, XO_ACS, XO_ASN, XO_ATN, XO_SIN, XO_COS, XO_TAN,

  XO_FRD, XO_TOD, XO_DEG, XO_RAD,

  XO_OBrace, XO_CBrace,

  XO_GT, XO_GE, XO_EQ, XO_NE, XO_LE, XO_LT,

  XO_Or, XO_XOr, XO_And, XO_BOr, XO_BXOr, XO_BAnd,

  XO_Plus, XO_Minus,

  XO_Mult, XO_Div,

  XO_Exp,

  XO_Not, XO_BNot,
*/
  };

//===========================================================================
//
//
//
//===========================================================================

inline CCoOrd Max(const CCoOrd A, const CCoOrd B)
  {
  CCoOrd C;
  C.m_Col=Max(A.m_Col, B.m_Col);
  C.m_Row=Max(A.m_Row, B.m_Row);
  return C;
  }

//===========================================================================
//
//
//
//===========================================================================

enum PLC5Fns {
  Fn_NULL,

  Fn_SOR, Fn_EOR, Fn_BST, Fn_NXB, Fn_BND,

  Fn_XIC, Fn_XIO, Fn_OTE, Fn_OTL, Fn_OTU,

  Fn_ADD,

  Fn_BSL, Fn_BSR, Fn_BTD, Fn_BTR, Fn_BTW,

  Fn_CMP, Fn_CPT, Fn_COP, Fn_CTD, Fn_CTU,

  Fn_DIV,

  Fn_EQU,

  Fn_FAL, Fn_FBC, Fn_FFL, Fn_FFU,

  Fn_GEQ, Fn_GRT,

  Fn_JSR, Fn_JMP,

  Fn_LBL, Fn_LEQ, Fn_LES, Fn_LIM,

  Fn_MOV, Fn_MUL,

  Fn_NEG,

  Fn_ONS,

  Fn_RES, Fn_RET,

  Fn_SBR, Fn_SQR, Fn_SUB,

  Fn_TON, Fn_TOF,

  // Special's should always be at end
  Fn_HConn, Fn_SConn, Fn_EConn, Fn_DConn, Fn_UConn,

  };

class CFnInfoItem
  {
  public:
    const char * m_Txt;
    byte  m_Fn;
    byte  m_Type;
    int   m_nParms;
    byte  m_iJust;
    const char * m_Pict;
    const char * m_Desc;

    byte  m_InComplete;
  };

const byte Typ_Other   = 0;
const byte Typ_SimpleA = 1;
const byte Typ_SimpleN = 2;
const byte Typ_Block   = 3;

const byte J_None      = 0;
const byte J_Left      = 1;
const byte J_Right     = 2;

static CFnInfoItem FnList[]=
  {
    //Txt     Fn        Type      nParm  Just     Pict       Desc
    //                                                        12345678901234567890123456
    {"???",   Fn_NULL , Typ_Other,    0, J_None , "   ",     "",                      },

    {"SOR",   Fn_SOR  , Typ_Other,    0, J_Left , "+--",     "",                      },
    {"EOR",   Fn_EOR  , Typ_Other,    0, J_Right, "--+",     "",                      },
    {"BST",   Fn_BST  , Typ_Other,    0, J_None,  "-+-",     "",                      },
    {"NXB",   Fn_NXB  , Typ_Other,    0, J_None,  " +-",     "",                      },
    {"BND",   Fn_BND  , Typ_Other,    0, J_None,  "-+-",     "",                      },

    // basics
    {"XIC",   Fn_XIC  , Typ_SimpleA,  1, J_Left , "-] [-",   "",                      },
    {"XIO",   Fn_XIO  , Typ_SimpleA,  1, J_Left , "-]/[-",   "",                      },
    {"OTE",   Fn_OTE  , Typ_SimpleA,  1, J_Right, "-( )-",   "",                      },
    {"OTL",   Fn_OTL  , Typ_SimpleA,  1, J_Right, "-(L)-",   "",                      },
    {"OTU",   Fn_OTU  , Typ_SimpleA,  1, J_Right, "-(U)-",   "",                      },

    {"ADD",   Fn_ADD  , Typ_Block,    3, J_Right, "#",       "ADD",                   },

    {"BSL",   Fn_BSL  , Typ_Block,    4, J_Right, "#",       "BIT SHIFT LEFT",        },
    {"BSR",   Fn_BSR  , Typ_Block,    4, J_Right, "#",       "BIT SHIFT RIGHT",       },
    {"BTD",   Fn_BTD  , Typ_Block,    5, J_Right, "#",       "",                      },
    {"BTR",   Fn_BTR  , Typ_Block,    7, J_Right, "#",       "BLOCK TRNSFR READ",     },
    {"BTW",   Fn_BTW  , Typ_Block,    7, J_Right, "#",       "BLOCK TRNSFR WRITE",    },

    {"CMP",   Fn_CMP  , Typ_Block,    3, J_Left , "#",       "COMPARE",               },
    {"CPT",   Fn_CPT  , Typ_Block,    0, J_Right, "#",       "COMPUTE",               },
    {"COP",   Fn_COP  , Typ_Block,    3, J_Right, "#",       "COPY FILE",             },
    {"CTD",   Fn_CTD  , Typ_Block,    3, J_Right, "#",       "COUNT DOWN",            },
    {"CTU",   Fn_CTU  , Typ_Block,    3, J_Right, "#",       "COUNT UP",              },

    {"DIV",   Fn_DIV  , Typ_Block,    3, J_Right, "#",       "DIV",                   },

    {"EQU",   Fn_EQU  , Typ_Block,    2, J_Right, "#",       "EQUAL",                 },

    {"FAL",   Fn_FAL  , Typ_Block,    5, J_Right, "#",       "FILE ARITH/LOGICAL",                      },
    {"FBC",   Fn_FBC  , Typ_Block,    9, J_Right, "#",       "FILE BIT COMPARE"       },
    {"FFL",   Fn_FFL  , Typ_Block,    5, J_Right, "#",       "FIFO LOAD",             },
    {"FFU",   Fn_FFU  , Typ_Block,    5, J_Right, "#",       "FIFO UNLOAD",           },

    {"GEQ",   Fn_GEQ  , Typ_Block,    2, J_Right, "#",       "GREATER THAN OR EQUAL", },
    {"GRT",   Fn_GRT  , Typ_Block,    2, J_Right, "#",       "GREATER THAN",          },

    {"JSR",   Fn_JSR  , Typ_Block,    0, J_Right, "#",       "JUMP TO SUBROUTINE"     },
    {"JMP",   Fn_JMP  , Typ_SimpleN,  1, J_Right, "-(JMP)-", "",                      },

    {"LBL",   Fn_LBL  , Typ_SimpleN,  1, J_Left,  "-]LBL[-", "",                      },
    {"LEQ",   Fn_LEQ  , Typ_Block,    2, J_Right, "#",       "LESS THAN OR EQUAL",    },
    {"LES",   Fn_LES  , Typ_Block,    2, J_Right, "#",       "LESS THAN",             },
    {"LIM",   Fn_LIM  , Typ_Block,    3, J_Right, "#",       "LIMIT",                 },

    {"MOV",   Fn_MOV  , Typ_Block,    2, J_Right, "#",       "MOVE",                  },
    {"MUL",   Fn_MUL  , Typ_Block,    3, J_Right, "#",       "MUL",                   },

    {"NEG",   Fn_NEG  , Typ_Block,    2, J_Right, "#",       "NEG",                   },
    {"ONS",   Fn_ONS  , Typ_SimpleA,  1, J_Left,  "-[ONS]-", "ONS",                   },

    {"RES",   Fn_RES  , Typ_SimpleA,  1, J_Right, "-(RES)-", "",                      },
    {"RET",   Fn_RET  , Typ_Block,    0, J_Right, "#",       "RETURN ()",             },

    {"SBR",   Fn_SBR  , Typ_Block,    0, J_Right, "#",       "SUBROUTINE",            },
    {"SQR",   Fn_SQR  , Typ_Block,    2, J_Right, "#",       "SQR",                   },
    {"SUB",   Fn_SUB  , Typ_Block,    3, J_Right, "#",       "SUB",                   },

    {"TON",   Fn_TON  , Typ_Block,    4, J_Right, "#",       "TIMER ON DELAY",        },
    {"TOF",   Fn_TOF  , Typ_Block,    4, J_Right, "#",       "TIMER OFF DELAY",       },

    {"---",   Fn_HConn, Typ_Other,    0, J_None , "---",     "",                      },
    {" +-",   Fn_SConn, Typ_Other,    0, J_None , " +-",     "",                      },
    {"-+ ",   Fn_EConn, Typ_Other,    0, J_None , "-+ ",     "",                      },
    {" | ",   Fn_DConn, Typ_Other,    0, J_None , " | ",     "",                      },
    {" | ",   Fn_UConn, Typ_Other,    0, J_None , " | ",     "",                      },

  };


//===========================================================================
//
//  Statics
//
//===========================================================================

//CPLC5   & CFnBase::MyPLC()               { return *m_pRung->m_pLadder->m_pPLC; };
CFnInfoItem & CFnBase::MyInfo()          { return FnList[m_Cd]; };

//===========================================================================


//===========================================================================
//
//  Timers
//
//===========================================================================

struct CFnTimer_Struct
  {
  word  m_XCnt:9,
        m_TimeBase1:1,
          _1:3,
        m_DN:1,
        m_TT:1,
        m_EN:1;
  short m_Pre, m_Acc;

  void Reset()
    {
    m_XCnt=0;
    m_Acc=0;
    m_EN=0;
    m_TT=0;
    m_DN=0;
    };
  bool SetEnabled_TON(CPLC5 * Plc, bool On)
    {
    if (On)
      {
      m_EN=1;
      m_TT=1;
      if (m_Acc<m_Pre)
        {
        long inc=m_XCnt+(Plc->m_StepMS*100L)/(m_TimeBase1 ? 1000 : 10);
        m_Acc+=short(inc/100L);
        m_XCnt=short(inc%100L);
        }
      if (m_Acc>=m_Pre)
        {
        m_Acc=m_Pre;
        m_DN=1;
        m_TT=0;
        }
      }
    else
      {
      m_EN=0;
      m_DN=0;
      m_Acc=0;
      }
    return m_EN!=0;
    }
  bool SetEnabled_TOF(CPLC5 * Plc, bool On)
    {
    if (On)
      {
      m_EN=1;
      m_TT=0;
      m_DN=1;
      m_Acc=0;
      }
    else
      {
      m_EN=0;
      //m_DN=0;
      if (m_Acc<m_Pre)
        {
        m_TT=1;
        long inc=m_XCnt+(Plc->m_StepMS*100L)/(m_TimeBase1 ? 1000 : 10);
        m_Acc+=short(inc/100L);
        m_XCnt=short(inc%100L);
        }
      if (m_Acc>=m_Pre)
        {
        m_Acc=m_Pre;
        m_DN=0;
        m_TT=0;
        }
      }
    return m_EN!=0;
    }
  };

class CFnTimer : public CFnBlk
  {
  protected:
    CAddress m_Add;
    CAddress *m_Adds[2];

  public:
    CFnTimer_Struct & Struct() { return *(CFnTimer_Struct*)m_Add.StructAddress(); };
    CFnTimer(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns),
      m_Add(Plc)
      {
      m_Add.Parse(Tkns.StrToken());
      CFnTimer_Struct & T=Struct();
      T.m_XCnt=0;
      T.m_TimeBase1=(Tkns.FltToken()>0.9);
      T.m_Pre=(short)Tkns.IntToken();
      short a=(short)Tkns.IntToken();
      T.m_Acc=(m_Plc->m_LoadWhat>=Load_All ? a : 0);

      long PreMS=T.m_Pre*(T.m_TimeBase1 ? 1000L : 10L);
      if (PreMS<m_Plc->m_MinTimerPre)
        m_Plc->m_MinTimerPre=PreMS;
      };
    virtual ~CFnTimer() { };
    virtual byte PictWide()   { return 21; };
    virtual byte PictHeight() { return 7; };
    //virtual byte PictOffset() { return 1; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Timer:%*s",  Wide-6,m_Add.Text()); return true;
        case 1: S.Format("Base (SEC):%*s", Wide-11, Struct().m_TimeBase1 ? "1.0":"0.01"); return true;
        case 2: S.Format("Preset:%*i", Wide-7, Struct().m_Pre); return true;
        case 3: S.Format("Accum:%*i", Wide-6, Struct().m_Acc); return true;
        }
      return false;
      };
    virtual byte    PictDescCount()     { return m_Add.DescCount(); };
    virtual LPCTSTR PictDescText(int i) { return m_Add.DescText(i); };

    virtual bool SetEnabled(bool On)
      {
      CFnTimer_Struct & T=Struct();
      if (m_Plc->m_bReset)
        {
        T.m_Acc=0;
        }

      if (m_Cd==Fn_TON)
        return T.SetEnabled_TON(m_Plc, On);
      else if (m_Cd==Fn_TOF)
        return T.SetEnabled_TOF(m_Plc, On);
      else
        return false;
      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return &m_Add; } ;
    
    virtual long      NAddressFlds()  { return 1; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return &m_Add; 
        } ;
      return NULL;
      }
  };

//===========================================================================
//
//  Counters
//
//===========================================================================

struct CFnCounter_Struct
  {
  word    _0:11,
        m_UN:1,
        m_OV:1,
        m_DN:1,
        m_CD:1,
        m_CU:1;
  short m_Pre, m_Acc;

  void Reset()
    {
    m_Acc=0;
    m_UN=0;
    m_OV=0;
    m_DN=0;
    m_CD=0;
    m_CU=0;
    };
  bool SetEnabled_CTU(bool On)
    {
    if (On && !m_CU)
      {
      m_CU=1;
      if (m_Acc==32767)
        m_OV=true;
      m_Acc++;
      m_DN=(m_Acc>=m_Pre);
      }
    else
      m_CU=0;
    return m_CU!=0;
    }
  bool SetEnabled_CTD(bool On)
    {
    if (On && !m_CD)
      {
      m_CD=1;
      if (m_Acc==-32768)
        m_OV=true;
      m_Acc--;
      m_DN=(m_Acc>=m_Pre);
      }
    else
      m_CD=0;
    return m_CD!=0;
    }
  };

class CFnCounter : public CFnBlk
  {
  protected:
    CAddress m_Add;
  public:

    CFnCounter_Struct & Struct() { return *(CFnCounter_Struct*)m_Add.StructAddress(); };

    CFnCounter(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns),
      m_Add(Plc)
      {
      m_Add.Parse(Tkns.StrToken());
      CFnCounter_Struct & C=Struct();
      C.m_Pre=(short)Tkns.IntToken();
      short a=(short)Tkns.IntToken();
      C.m_Acc=(Plc->m_LoadWhat>=Load_All ? a : 0);
      };
    virtual ~CFnCounter() { };
    virtual byte PictWide()   { return 21; };
    virtual byte PictHeight() { return 6; };
    //virtual byte PictOffset() { return 1; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Counter:%*s",  Wide-8,m_Add.Text()); return true;
        case 1: S.Format("Preset:%*i", Wide-7, Struct().m_Pre); return true;
        case 2: S.Format("Accum:%*i", Wide-6, Struct().m_Acc); return true;
        }
      return false;
      };
    virtual byte    PictDescCount()     { return m_Add.DescCount(); };
    virtual LPCTSTR PictDescText(int i) { return m_Add.DescText(i); };
    virtual bool SetEnabled(bool On)
      {
      CFnCounter_Struct & C=Struct();
      if (m_Plc->m_bReset)
        {
        C.m_Acc=0;
        }

      if (m_Cd==Fn_CTU)
        return C.SetEnabled_CTU(On);
      else if (m_Cd==Fn_CTD)
        return C.SetEnabled_CTD(On);
      else
        return false;
      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return &m_Add; } ;
    
    virtual long      NAddressFlds()  { return 1; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return &m_Add; 
        } ;
      return NULL;
      }

  };

//===========================================================================
//
// Test Functions
//
//===========================================================================

class CFnTest : public CFnBlk
  {
  protected:
    CAddOrImmed m_SrcA;
    CAddOrImmed m_SrcB;
  public:
    CFnTest(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns),
      m_SrcA(Plc),
      m_SrcB(Plc)
      {
      m_SrcA.Parse(Tkns.StrToken());
      m_SrcB.Parse(Tkns.StrToken());
      };
    virtual ~CFnTest() { };
    virtual byte PictWide()   { return 21+4; };
    virtual byte PictHeight() { return 5; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Source A:%*s",  Wide-9,m_SrcA.Text()); return true;
        case 1: S.Format("Source B:%*s",  Wide-9,m_SrcB.Text()); return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      if (On)
        {
        float A=m_SrcA.ValueAsFlt();
        float B=m_SrcB.ValueAsFlt();
        bool OK=false;
        switch (m_Cd)
          {
          case Fn_EQU: OK=(A==B); break;
          case Fn_GEQ: OK=(A>=B); break;
          case Fn_GRT: OK=(A>B); break;
          case Fn_LEQ: OK=(A<=B); break;
          case Fn_LES: OK=(A<B); break;
          default:
            CheckThat(FALSE, "BAD Test");
          };
        if (0 && dbgf) fprintf(dbgf, "                            Test:%s %s %s=%g  %s=%g\n",
                          MyInfo().m_Txt, OK ? "True":"    ", m_SrcA.Text(), A, m_SrcB.Text(), B);
        return OK;
        };
      return false;

      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return (cpt.y<=2) ? m_SrcA.Address() : m_SrcB.Address(); } ;
    
    virtual long      NAddressFlds()  { return 2; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return m_SrcA.Address();
        case 1: return m_SrcB.Address();
        } ;
      return NULL;
      }


  };

//===========================================================================

class CFnLIM : public CFnBlk
  {
  protected:
    CAddOrImmed m_Lo;
    CAddress    m_Tst;
    CAddOrImmed m_Hi;
  public:
    CFnLIM(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns),
      m_Lo(Plc),
      m_Tst(Plc),
      m_Hi(Plc)
      {
      m_Lo.Parse(Tkns.StrToken());
      m_Tst.Parse(Tkns.StrToken());
      m_Hi.Parse(Tkns.StrToken());
      };
    virtual ~CFnLIM() { };
    virtual byte PictWide()   { return 21+4; };
    virtual byte PictHeight() { return 6; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Low Limit:%*s",   Wide-10,m_Lo.Text()); return true;
        case 1: S.Format("Test:%*s",        Wide-5,m_Tst.Text()); return true;
        case 2: S.Format("High Limit:%*s",  Wide-11,m_Hi.Text()); return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      if (On)
        {
        float L=m_Lo.ValueAsFlt();
        float T=m_Tst.ValueAsFlt();
        float H=m_Hi.ValueAsFlt();
        bool OK=false;
        if (L<=H)
          OK=(L<=T && T<=H);
        else // L>H)
          OK=(T<L || T>H);

        if (0 && dbgf) fprintf(dbgf, "Test:%s %s %s=%g  %s=%g  %s=%g\n",
                          MyInfo().m_Txt, OK ? "OK":"  ", m_Lo.Text(), L, m_Tst.Text(), T, m_Hi.Text(), H);
        return OK;
        };
      return false;

      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return (cpt.y<=2) ? m_Lo.Address() : (cpt.y<=3) ? &m_Tst : m_Hi.Address(); } ;
    
    virtual long      NAddressFlds()  { return 3; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return m_Lo.Address(); 
        case 1: return &m_Tst; 
        case 2: return m_Hi.Address(); 
        } ;
      return NULL;
      }

  };

//===========================================================================

class CFnCMP : public CFnBlk
  {
  protected:
    //CString m_Exp;
    CExpression m_Exp;
    bool bPrevValue;
    bool bPrevInited;
  public:
    CFnCMP(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns),
      m_Exp(Plc)
      {
      bPrevInited=false;
      m_Exp.Parse(Tkns, XM_CMP);
      m_Wide=Max(21,(int)strlen(m_Exp.Text())+5);
      };
    virtual ~CFnCMP() { };
    virtual byte PictWide()   { return m_Wide; };
    virtual byte PictHeight() { return 5; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Expression:%*s", Wide-11,""); return true;
        case 1: S.Format("%*s",            Wide,m_Exp.Text()); return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      if (On)
        {
        CXValue V=m_Exp.Evaluate();
        if (!bPrevInited || (bPrevValue!=(V.I()!=0)))
          m_History.DumpChange((V.I()!=0), m_Exp, m_pRung, m_Exp.Text());
        bPrevValue=V.I()!=0;
        bPrevInited=true;
        return bPrevValue;
        };
      return false;

      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt) { return NULL;};//&m_Tst; } ;
    
    virtual long      NAddressFlds()  { return 0; };
    virtual CAddress* AddressFld(int Index)
      { 
      //switch (Index)
      //  {
      //  case 0: return m_Exp.Address(
      //  } ;
      return NULL;
      }
  };

//===========================================================================
//
// Compute
//
//===========================================================================

class CFnCpt2 : public CFnBlk
  {
  protected:
    CAddOrImmed m_A;
    CAddress    m_B;
  public:
    CFnCpt2(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns),
      m_A(Plc),
      m_B(Plc)
      {
      m_A.Parse(Tkns.StrToken());
      m_B.Parse(Tkns.StrToken());
      };
    virtual ~CFnCpt2() { };
    virtual byte PictWide()   { return 21+4; };
    virtual byte PictHeight() { return 6; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Source:%*s",      Wide-7, m_A.Text()); return true;
        case 1: S.Format("Destination:%*s", Wide-12,m_B.Text()); return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      if (On)
        {
        float A=m_A.ValueAsFlt();
        float B=0;
        switch (m_Cd)
          {
          case Fn_NEG: B=-A; break;
          default: CheckThat(FALSE, "Unexpected code");
          }

        //if (dbgf) fprintf(dbgf, "Test:%s %s %s=%g  %s=%g  %s=%g\n",
        //                  MyInfo().m_Txt, OK ? "OK":"  ", m_Lo.Text(), L, m_Tst.Text(), T, m_Hi.Text(), H);
        m_B.SetValueAsFlt(B);
        return true;
        };
      return false;
      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return (cpt.y<=2) ? m_A.Address():&m_B; } ;
    
    virtual long      NAddressFlds()  { return 2; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return m_A.Address(); 
        case 1: return &m_B; 
        } ;
      return NULL;
      }
  };

//===========================================================================

class CFnCpt3 : public CFnBlk
  {
  protected:
    CAddOrImmed m_A;
    CAddOrImmed m_B;
    CAddress    m_C;
  public:
    CFnCpt3(CPLC5 * Plc, byte Cd, CTknFile & Tkns):
      CFnBlk(Plc, Cd, Tkns),
      m_A(Plc),
      m_B(Plc),
      m_C(Plc)
      {
      m_A.Parse(Tkns.StrToken());
      m_B.Parse(Tkns.StrToken());
      m_C.Parse(Tkns.StrToken());
      };
    virtual ~CFnCpt3() { };
    virtual byte PictWide()   { return 21+4; };
    virtual byte PictHeight() { return 6; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Source A:%*s",    Wide-9, m_A.Text()); return true;
        case 1: S.Format("Source B:%*s",    Wide-9, m_B.Text()); return true;
        case 2: S.Format("Destination:%*s", Wide-12,m_C.Text()); return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      if (On)
        {
        float A=m_A.ValueAsFlt();
        float B=m_B.ValueAsFlt();
        float C=0;
        switch (m_Cd)
          {
          case Fn_ADD: C=A+B; break;
          case Fn_SUB: C=A-B; break;
          case Fn_MUL: C=A*B; break;
          case Fn_DIV: C=A/B; break;
          default: CheckThat(FALSE, "Unexpected code");
          }
        //if (dbgf) fprintf(dbgf, "Test:%s %s %s=%g  %s=%g  %s=%g\n",
        //                  MyInfo().m_Txt, OK ? "OK":"  ", m_Lo.Text(), L, m_Tst.Text(), T, m_Hi.Text(), H);
        m_C.SetValueAsFlt(C);
        return true;
        };
      return false;
      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return (cpt.y<=2) ? m_A.Address() : (cpt.y<=3) ? m_B.Address() : &m_C; } ;
//    virtual CAddress* TargetAddress(long i)  
//      { return (i==0) ? m_A.Address() : (i==1) ? m_B.Address() : &m_C; } ;
    
    virtual long      NAddressFlds()  { return 3; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return m_A.Address(); 
        case 1: return m_B.Address(); 
        case 2: return &m_C; 
        } ;
      return NULL;
      }
  };

//===========================================================================

class CFnCPT : public CFnBlk
  {
  protected:
    //CString m_Exp;
    CAddress    m_Dst;
    CExpression m_Exp;
  public:
    CFnCPT(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns),
      m_Dst(Plc),
      m_Exp(Plc)
      {
      m_Dst.Parse(Tkns.StrToken());
      m_Exp.Parse(Tkns, XM_CPT);
      m_Wide=Max(21,         (int)strlen(m_Dst.Text())+12+4);
      m_Wide=Max((int)m_Wide,(int)strlen(m_Exp.Text())+4);
      };
    virtual ~CFnCPT() { };
    virtual byte PictWide()   { return m_Wide; };
    virtual byte PictHeight() { return 6; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Destination:%*s",  Wide-12,m_Dst.Text()); return true;
        case 1: S.Format("Expression:%*s",   Wide-11,""); return true;
        case 2: S.Format("%*s",              Wide,m_Exp.Text()); return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      if (On)
        {
        CXValue X=m_Exp.Evaluate();
        if (m_Dst.IsFlt())
          m_Dst.SetFValue(X.F());
        else
          m_Dst.SetIValue(X.I());
        return true;
        };
      return false;

      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return &m_Dst; } ;
    
    virtual long      NAddressFlds()  { return 1+m_Exp.NAddressFlds(); };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return &m_Dst; 
        case 1: return m_Exp.AddressFld(Index-1); 
        } ;
      return NULL;
      }
  };

//===========================================================================
//
// FAL
//
//===========================================================================

struct CFnFAL_Struct
  {
  word  m_Mode:11,
        m_ER:1,
          _1:1,
        m_DN:1,
          _2:1,
        m_EN:1;
  short m_Len, m_Pos;
  };

class CFnFAL : public CFnBlk
  {
  protected:
    //CString m_Exp;
    CAddress    m_Ctrl;
    CAddress    m_Dst;
    CExpression m_Exp;
  public:
    CFnFAL_Struct & Struct() { return *(CFnFAL_Struct*)m_Ctrl.StructAddress(); };
    CFnFAL(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns),
      m_Ctrl(Plc),
      m_Dst(Plc),
      m_Exp(Plc)
      {

      //TRACE("Complete FAL\n");
      m_Ctrl.Parse(Tkns.StrToken());
      CFnFAL_Struct &F=Struct();
      F.m_Len=(short)Tkns.IntToken();
      F.m_Pos=(short)Tkns.IntToken();
      CString T=Tkns.StrToken();
      if (T.CompareNoCase("ALL")==0)
        F.m_Mode=0;
      else if (T.CompareNoCase("INC")==0)
        F.m_Mode=1024;
      else
        F.m_Mode=Range(1, (int)atol(T), 1000);
      CheckThat(F.m_Mode==0, "FAL Mode other than ALL not implemented");
      m_Dst.Parse(Tkns.StrToken());
      m_Exp.Parse(Tkns, XM_FAL);
      //F.m_EN=0;
      //F.m_DN=0;
      //F.m_ER=0;

      m_Wide=Max(21,    (int)strlen(m_Ctrl.Text())+8);
      m_Wide=Max((int)m_Wide,(int)strlen(m_Dst.Text())+12);
      m_Wide=Max((int)m_Wide,(int)strlen(m_Exp.Text())+5);
      };
    virtual ~CFnFAL() { };
    virtual byte PictWide()   { return m_Wide; };
    virtual byte PictHeight() { return 10; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Control:%*s",      Wide-8, m_Ctrl.Text()); return true;
        case 1: S.Format("Length:%*i",       Wide-7, Struct().m_Len); return true;
        case 2: S.Format("Position:%*i",     Wide-9, Struct().m_Pos); return true;
        case 3:
          {
          CFnFAL_Struct &F=Struct();
          if (F.m_Mode==0)
            S.Format("Mode:%*s",         Wide-5, "ALL");
          else if (F.m_Mode>1000)
            S.Format("Mode:%*s",         Wide-5, "INC");
          else
            S.Format("Mode:%*i",         Wide-5, F.m_Mode);
          return true;
          }
        case 4: S.Format("Destination:%*s",  Wide-12,m_Dst.Text()); return true;
        case 5: S.Format("Expression:%*s",   Wide-11,""); return true;
        case 6: S.Format("%*s",              Wide,m_Exp.Text()); return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      CFnFAL_Struct &F=Struct();
      if (On && !F.m_ER)
        {
        F.m_EN=1;
        // NB NB Not quite sure what to do with F.Pos
        for (int i=0; i<F.m_Len; i++)
          {
          m_Plc->SetIndexRegister(i);
          CXValue X=m_Exp.Evaluate();
          if (m_Dst.IsFlt())
            m_Dst.SetFValue(X.F());
          else
            m_Dst.SetIValue(X.I());
          }
        F.m_DN=1;
        return true;
        };
      F.m_EN=0;
      F.m_DN=0;
      return false;
      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt) { return (cpt.y<=2) ? &m_Ctrl : &m_Dst; };
    
    virtual long      NAddressFlds()  { return 2; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return &m_Ctrl; 
        case 1: return &m_Dst; 
//        case 2: return &m_Exp; 
        } ;
      return NULL;
      }
  };

//===========================================================================
//
// COP
//
//===========================================================================

class CFnCOP : public CFnBlk
  {
  protected:
    CAddress    m_Src;
    CAddress    m_Dst;
    int         m_Length;
  public:
    CFnCOP(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns),
      m_Src(Plc),
      m_Dst(Plc)
      {
      m_Src.Parse(Tkns.StrToken());
      m_Dst.Parse(Tkns.StrToken());
      m_Length=Tkns.IntToken();
      m_Wide=Max(21,    (int)strlen(m_Src.Text())+7+5);
      m_Wide=Max((int)m_Wide,(int)strlen(m_Dst.Text())+12+5);
      };
    virtual ~CFnCOP() { };
    virtual byte PictWide()   { return m_Wide; };
    virtual byte PictHeight() { return 6; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Source:%*s",      Wide-7,m_Src.Text()); return true;
        case 1: S.Format("Destination:%*s", Wide-12,m_Dst.Text()); return true;
        case 2: S.Format("Length:%*i",      Wide-7,m_Length); return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      if (On)
        {
        for (int i=0; i<m_Length; i++)
          {
          m_Plc->SetIndexRegister(i);
          for (int j=0; j<m_Src.Table()->NStructVals(); j++)
            {
            float V=m_Src.ValueAsFlt(j);
            m_Dst.SetValueAsFlt(j, V);
            }
          }
        return true;
        };
      return false;

      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return (cpt.y<=2) ? &m_Src : &m_Dst; } ;
    
    virtual long      NAddressFlds()  { return 2; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return &m_Src; 
        case 1: return &m_Dst; 
        } ;
      return NULL;
      }
  };

//===========================================================================
//
// MOV
//
//===========================================================================

class CFnMOV : public CFnBlk
  {
  protected:
    CAddOrImmed m_Src;
    CAddress    m_Dst;
  public:
    CFnMOV(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns),
      m_Src(Plc),
      m_Dst(Plc)
      {
      m_Src.Parse(Tkns.StrToken());
      m_Dst.Parse(Tkns.StrToken());
      m_Wide=Max(21,    (int)strlen(m_Src.Text())+7+5);
      m_Wide=Max((int)m_Wide,(int)strlen(m_Dst.Text())+12+5);
      };
    virtual ~CFnMOV() { };
    virtual byte PictWide()   { return m_Wide; };
    virtual byte PictHeight() { return 5; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Source:%*s",      Wide-7,m_Src.Text()); return true;
        case 1: S.Format("Destination:%*s", Wide-12,m_Dst.Text()); return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      if (On)
        {
        float V=m_Src.ValueAsFlt();
        m_Dst.SetValueAsFlt(V);
        return true;
        };
      return false;

      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return (cpt.y<=2) ? m_Src.Address() : &m_Dst;};//&m_Tst; } ;
    
    virtual long      NAddressFlds()  { return 2; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return m_Src.Address(); 
        case 1: return &m_Dst;
        } ;
      return NULL;
      }
  };

//===========================================================================
//
// FBC
//
//===========================================================================

struct CFnFBC_CmpStruct
  {
  word    _0:8,
        m_FD:1,
        m_IN:1,
          _1:1,
        m_ER:1,
          _2:1,
        m_DN:1,
          _3:1,
        m_EN:1;
  short m_Len, m_Pos;
  };

struct CFnFBC_ResStruct
  {
  word    _0:13,
        m_DN:1,
          _2:2;
  short m_Len, m_Pos;
  };

class CFnFBC : public CFnBlk
  {
  protected:
    CAddress    m_Src;
    CAddress    m_Ref;
    CAddress    m_Res;
    CAddress    m_CmpCtrl;
    CAddress    m_ResCtrl;
  public:
    CFnFBC_CmpStruct & CmpStruct() { return *(CFnFBC_CmpStruct*)m_CmpCtrl.StructAddress(); };
    CFnFBC_ResStruct & ResStruct() { return *(CFnFBC_ResStruct*)m_ResCtrl.StructAddress(); };

    CFnFBC(CPLC5 * Plc, byte Cd, CTknFile & Tkns):
      CFnBlk(Plc, Cd, Tkns),
      m_Src(Plc),
      m_Ref(Plc),
      m_Res(Plc),
      m_CmpCtrl(Plc),
      m_ResCtrl(Plc)
      {
      m_Src.Parse(Tkns.StrToken());
      m_Ref.Parse(Tkns.StrToken());
      m_Res.Parse(Tkns.StrToken());

      m_CmpCtrl.Parse(Tkns.StrToken());
      CFnFBC_CmpStruct & C=CmpStruct();
      C.m_Len=(short)Tkns.IntToken();
      C.m_Pos=(short)Tkns.IntToken();

      m_ResCtrl.Parse(Tkns.StrToken());
      CFnFBC_ResStruct & R=ResStruct();
      R.m_Len=(short)Tkns.IntToken();
      R.m_Pos=(short)Tkns.IntToken();

      m_Wide=Max(21,         (int)strlen(m_Src.Text())+7+5);
      m_Wide=Max((int)m_Wide,(int)strlen(m_Ref.Text())+10+5);
      m_Wide=Max((int)m_Wide,(int)strlen(m_Res.Text())+7+5);
      m_Wide=Max((int)m_Wide,(int)strlen(m_CmpCtrl.Text())+16+5);
      m_Wide=Max((int)m_Wide,(int)strlen(m_ResCtrl.Text())+15+5);
      };
    virtual ~CFnFBC() { };
    virtual byte PictWide()   { return m_Wide; };
    virtual byte PictHeight() { return 12; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Source:%*s",          Wide-7, m_Src.Text()); return true;
        case 1: S.Format("Reference:%*s",       Wide-10,m_Ref.Text()); return true;
        case 2: S.Format("Result:%*s",          Wide-7, m_Res.Text()); return true;
        case 3: S.Format("Compare Control:%*s", Wide-16,m_CmpCtrl.Text()); return true;
        case 4: S.Format("Length:%*i",          Wide-7, CmpStruct().m_Len); return true;
        case 5: S.Format("Position:%*i",        Wide-9, CmpStruct().m_Pos); return true;
        case 6: S.Format("Result Control:%*s",  Wide-15,m_ResCtrl.Text()); return true;
        case 7: S.Format("Length:%*i",          Wide-7, ResStruct().m_Len); return true;
        case 8: S.Format("Position:%*i",        Wide-9, ResStruct().m_Pos); return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      CFnFBC_CmpStruct & C=CmpStruct();
      CFnFBC_ResStruct & R=ResStruct();

//#pragma message ("-------------------------FIX FBC")
      if (On)
        {
        CheckThat(!C.m_IN, "FBC One Test oper scan not implemented");
        C.m_EN=1;
        C.m_ER=C.m_Len<=0 || C.m_Pos<0;
        if (!C.m_ER)
          {
          R.m_Pos=0;
          for (int i=0; i<C.m_Len; i++)
            {
            m_Plc->SetIndexRegister(i);
            bool src=m_Src.IValueAsBit();
//            if (((i%10)*RAND_MAX)/5<rand()) // Debug Test rand()
//              src=!src;
            bool ref=m_Ref.IValueAsBit();
            if (src!=ref)
              {
              m_Plc->SetIndexRegister(R.m_Pos++);
              m_Res.SetIValue(i);
              C.m_FD=1;
              R.m_DN=R.m_Pos>=R.m_Len;
              if (R.m_DN)
                break;
              }
            }
          C.m_DN=1;
          }
        //CAddress::SetIndexRegister(0);
        return true;
        };
      C.m_EN=0;
      C.m_FD=0;
      C.m_DN=0;
      R.m_DN=0;
      C.m_Pos=0;
      R.m_Pos=0;
      return false;

      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  
      {
      switch (cpt.y)
        {
        case 0:
        case 1:
        case 2: return &m_Src;
        case 3: return &m_Ref;
        case 4: return &m_Res;
        case 5:
        case 6:
        case 7: return &m_CmpCtrl;
        default:  return &m_ResCtrl;
        }
      return NULL;
      } ;
    
    virtual long      NAddressFlds()  { return 5; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return &m_Src;
        case 1: return &m_Ref;
        case 2: return &m_Res;
        case 3: return &m_CmpCtrl;
        case 4: return &m_ResCtrl;
        } ;
      return NULL;
      }
  };

//===========================================================================
//
// BTR/BTW
//
//===========================================================================

//struct CFnFBC_CmpStruct
//  {
//  word    _0:8,
//        m_FD:1,
//        m_IN:1,
//          _1:1,
//        m_ER:1,
//          _2:1,
//        m_DN:1,
//          _3:1,
//        m_EN:1;
//  short m_Len, m_Pos;
//  };

struct CFnBXfr_Struct
  {
  word  m_Slot:1,
        m_Group:3,
        m_Rack:3,
        m_RW:1,
        m_TO:1,
        m_NR:1,
        m_EW:1,
        m_CO:1,
        m_ER:1,
        m_DN:1,
        m_ST:1,
        m_EN:1;

  short m_RqstCnt;
  short m_XmitCnt;
  short m_FileTypeNo;
  short m_ElementNo;
  };

class CFnBXfr : public CFnBlk
  {
  protected:
    //byte        m_Rack;
    //byte        m_Group;
    //byte        m_Module;

    CAddress    m_Ctrl;

    CAddress    m_Data;
    long        m_State;
    //byte        m_Length;
    //byte        m_Continuous;

  public:
    CFnBXfr_Struct & CtrlStruct() { return *(CFnBXfr_Struct*)m_Ctrl.StructAddress(); };

    CFnBXfr(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns),
      m_Ctrl(Plc),
      m_Data(Plc)
      {
      int Rack=Tkns.IntToken();
      int Group=Tkns.IntToken();
      int Slot=Tkns.IntToken();

      m_Ctrl.Parse(Tkns.StrToken());

      CFnBXfr_Struct &C=CtrlStruct();
      C.m_Rack=Rack;
      C.m_Group=Group;
      C.m_Slot=Slot;

      m_Data.Parse(Tkns.StrToken());
      C.m_RqstCnt=(byte)Tkns.IntToken();
      C.m_CO=Tkns.IntToken()!=0;
      C.m_FileTypeNo=m_Data.FileNo();
      C.m_ElementNo=m_Data.WordNo(false);

      m_Wide=Max(21,         (int)strlen(m_Ctrl.Text())+14+5);
      m_Wide=Max((int)m_Wide,(int)strlen(m_Data.Text())+10+5);

      m_State=0;
      };
    virtual ~CFnBXfr() { };
    virtual byte PictWide()   { return m_Wide; };
    virtual byte PictHeight() { return 10; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Rack:%*i",          Wide-5,  CtrlStruct().m_Rack); return true;
        case 1: S.Format("Group:%*i",         Wide-6,  CtrlStruct().m_Group); return true;
        case 2: S.Format("Module:%*i",        Wide-7,  CtrlStruct().m_Slot); return true;
        case 3: S.Format("Control Block:%*s", Wide-14, m_Ctrl.Text()); return true;
        case 4: S.Format("Data File:%*s",     Wide-10, m_Data.Text()); return true;
        case 5: S.Format("Length:%*i",        Wide-7,  CtrlStruct().m_RqstCnt); return true;
        case 6: S.Format("Continuous:%*i",    Wide-11, CtrlStruct().m_CO); return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      CFnBXfr_Struct & C=CtrlStruct();
      //CFnFBC_ResStruct & R=ResStruct();
      //
      CheckThat(!C.m_CO, "BTR?BTW Continuous Mode not yet implemented");
      switch (m_State)
        {
        case 0:
          if (On)
            {
            C.m_EN=1;
            C.m_EW=0;
            C.m_ST=0;
            C.m_DN=0; 
            C.m_ER=0;
            C.m_NR=0;
            m_State++;
            }
          else // (!On)
            {
            m_State=0;
            C.m_EN=0;
            }
          break;
        case 1:
          C.m_EW=1;
          m_State++;
          break;
        case 2:
          C.m_ST=1;
          m_State++;
          break;
        case 3:
        case 4:
        case 5:
        case 6:
          m_State++;
          break;
        case 7:
          C.m_DN=1; 
          //C.m_ER=1; // no errors
          m_State++;
          break;
        case 8:
          if (!On)
            {
            m_State=0;
            C.m_EN=0;
            }
          break;
        }
      return C.m_EN;
      };

    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return (cpt.y<=4) ? &m_Ctrl : &m_Data; } ;

    virtual long      NAddressFlds()  { return 2; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return &m_Ctrl;
        case 1: return &m_Data;
        } ;
      return NULL;
      }
  };

//===========================================================================
//
// JSR
//
//===========================================================================

inline bool IsInsMne(char *p) { return (strlen(p)==3 && isalpha(p[0]) && isalpha(p[1]) && isalpha(p[2])); };

class CFnJSR : public CFnBlk
  {
  protected:
    int  m_ProgFileNo, m_nParms, m_nReturns;
    CAddOrImmed m_Parm[MaxJSRParms];

  public:
    CFnJSR(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns)
      {
      for (int i=0; i<MaxJSRParms; i++)
        m_Parm[i].m_Plc=Plc;
      m_ProgFileNo=Tkns.IntToken();//atol(Tkns.StrPA[0]);
      m_nParms=Tkns.IntToken();//atol(PA[1]);
      m_nReturns=0;
      char * pTkn;
      for (i=0; (pTkn=Tkns.StrToken()); i++)
        {
        if (IsInsMne(pTkn))
          {
          Tkns.PushToken();
          break;
          }
        else
          m_Parm[i].Parse(pTkn);
        }
      m_nReturns=Max(0, i-m_nParms);
      };
    virtual ~CFnJSR()
      {
      };
    virtual byte PictWide()   { return 23; };
    virtual byte PictHeight() { return 4+Max(1,m_nParms)+Max(1,m_nReturns); };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      if (Row==0)
        {
        S.Format("Prog File #:%*i",  Wide-12,m_ProgFileNo);
        return true;
        }
      Row-=1;
      if (Row<m_nParms)
        {
        S.Format("Input  Par:%*s", Wide-11, m_Parm[Row].m_Txt);
        return true;
        }
      Row-=m_nParms;
      if (Row<m_nReturns)
        {
        S.Format("Return Par:%*s", Wide-11, m_Parm[m_nParms+Row].m_Txt);
        return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      if (On)
        {
        CLadder &L=MyPLC().m_Ladder[m_ProgFileNo];
        L.m_nXferParms=0;
        for (int i=0; i<m_nParms; i++)
          {
          CAddOrImmed &P=m_Parm[i];
          switch (P.m_What)
            {
            case AOC_Add: L.m_XferParms[i]=P.m_pAdd->ValueAsFlt(); break;
            case AOC_Flt: L.m_XferParms[i]=P.m_Flt; break;
            case AOC_Int: L.m_XferParms[i]=P.m_Int; break;
            default:  CheckThat(false, "Unknown JSR Parm Type");
            }
          L.m_nXferParms++;
          }

        L.Evaluate();

        CheckThat(L.m_nXferParms>=m_nReturns, "Too few return parameters");
        for (i=0; i<m_nReturns; i++)
          {
          CAddOrImmed &P=m_Parm[i+m_nParms];
          if (P.m_What==AOC_Add)
            P.m_pAdd->SetValueAsFlt(L.m_XferParms[i]);
          else
            CheckThat(false, "Must be address type");
          }
        }
      return On;
      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return NULL; };
    virtual CLadder* TargetLadder(CPoint pt, CPoint cpt)  { return &(MyPLC().m_Ladder[m_ProgFileNo]); };
    
    virtual long      NAddressFlds()  { return MaxJSRParms; };
    virtual CAddress* AddressFld(int Index)
      { 
      if (Index<MaxJSRParms)
        return m_Parm[Index].Address();
      return NULL;
      }
  };

//===========================================================================

class CFnSBR : public CFnBlk
  {
  protected:
    int  m_nParms;
    CAddOrImmed m_Parm[MaxJSRParms];

  public:
    CFnSBR(CPLC5 * Plc, byte Cd, CTknFile & Tkns): CFnBlk(Plc, Cd, Tkns)
      {
      for (int i=0; i<MaxJSRParms; i++)
        m_Parm[i].m_Plc=Plc;
      char * pTkn;
      for (i=0; (pTkn=Tkns.StrToken()); i++)
        {
        if (IsInsMne(pTkn))
          {
          Tkns.PushToken();
          break;
          }
        else
          m_Parm[i].Parse(pTkn);
        }
      m_nParms=i;
      };
    virtual ~CFnSBR()
      {
      };
    virtual byte PictWide()   { return 23; };
    virtual byte PictHeight() { return 4+Max(1,m_nParms); };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      if (Row<m_nParms)
        {
        S.Format("Input  Par:%*s", Wide-11, m_Parm[Row].m_Txt);
        return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      if (On)
        {
        CLadder & L=MyLadder();
        CheckThat(L.m_nXferParms>=m_nParms, "Too few passed parameters");
        for (int i=0; i<m_nParms; i++)
          {
          CAddOrImmed &P=m_Parm[i];
          if (P.m_What==AOC_Add)
            P.m_pAdd->SetValueAsFlt(L.m_XferParms[i]);
          else
            CheckThat(false, "Must be address type");
          }
        }
      return On;
      };
    //virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return NULL; };
    
    virtual long      NAddressFlds()  { return MaxJSRParms; };
    virtual CAddress* AddressFld(int Index)
      { 
      if (Index<MaxJSRParms)
        return m_Parm[Index].Address();
      return NULL;
      }
  };

//===========================================================================

class CFnRET : public CFnBlk
  {
  protected:
    int  m_nReturns;
    CAddOrImmed m_Return[MaxJSRParms];

  public:
    CFnRET(CPLC5 * Plc, byte Cd, CTknFile & Tkns): CFnBlk(Plc, Cd, Tkns)
      {
      for (int i=0; i<MaxJSRParms; i++)
        m_Return[i].m_Plc=Plc;
      
      char * pTkn;
      for (i=0; (pTkn=Tkns.StrToken()); i++)
        {
        if (IsInsMne(pTkn))
          {
          Tkns.PushToken();
          break;
          }
        else
          m_Return[i].Parse(pTkn);
        }
      m_nReturns=i;
      };
    virtual ~CFnRET()
      {
      };
    virtual byte PictWide()   { return 23; };
    virtual byte PictHeight() { return 3+Max(1,m_nReturns); };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      if (Row<m_nReturns)
        {
        S.Format("Return Par:%*s", Wide-11, m_Return[Row].m_Txt);
        return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      if (On)
        {
        CLadder & L=MyLadder();
        L.m_nXferParms=0;
        for (int i=0; i<m_nReturns; i++)
          {
          CAddOrImmed &P=m_Return[i];
          switch (P.m_What)
            {
            case AOC_Add: L.m_XferParms[i]=P.m_pAdd->ValueAsFlt(); break;
            case AOC_Flt: L.m_XferParms[i]=P.m_Flt; break;
            case AOC_Int: L.m_XferParms[i]=P.m_Int; break;
            default: CheckThat(false, "Unknown RET Parm Type");
            }
          L.m_nXferParms++;
          }
        }
      return On;
      };
    //virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return NULL; };
    
    virtual long      NAddressFlds()  { return MaxJSRParms; };
    virtual CAddress* AddressFld(int Index)
      { 
      if (Index<MaxJSRParms)
        return m_Return[Index].Address();
      return NULL;
      }
  };

//===========================================================================
//
// Shift Register
//
//===========================================================================

struct CFnFIFO_Struct
  {
  word    _0:12,
        m_EM:1,
        m_DN:1,
        m_EU:1,
        m_EN:1;
  short m_Len, m_Pos;

  void Reset()
    {
    m_Pos=0;
    m_Len=10;
    m_EM=0;
    m_DN=0;
    m_EU=0;
    m_EN=0;
    };
  };

class CFnFFL : public CFnBlk
  {
  protected:
    CAddress    m_Src;
    CAddress    m_FIFO;
    CAddress    m_Control;

  public:
    CFnFFL(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns),
      m_Src(Plc),
      m_FIFO(Plc),
      m_Control(Plc)
      {
      m_Src.Parse(Tkns.StrToken());
      m_FIFO.Parse(Tkns.StrToken());
      m_Control.Parse(Tkns.StrToken());

      CFnFIFO_Struct & F=*(CFnFIFO_Struct *)m_Control.StructAddress();
      F.m_Len=(short)Tkns.IntToken();
      F.m_Pos=(short)Tkns.IntToken();

      m_Wide=Max(21,(int)strlen(m_Src.Text())+7+5);
      m_Wide=Max(m_Wide, (byte)(strlen(m_FIFO.Text())+5+5));
      m_Wide=Max(m_Wide, (byte)(strlen(m_Control.Text())+8+5));
      };
    virtual ~CFnFFL() { };
    virtual byte PictWide()   { return m_Wide; };
    virtual byte PictHeight() { return 8; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("Source:%*s",   Wide-7,m_Src.Text()); return true;
        case 1: S.Format("FIFO:%*s",     Wide-5,m_FIFO.Text()); return true;
        case 2: S.Format("Control:%*s",  Wide-8,m_Control.Text()); return true;
        case 3: S.Format("Length:%*i",   Wide-7,((CFnFIFO_Struct *)m_Control.StructAddress())->m_Len); return true;
        case 4: S.Format("Position:%*i", Wide-9,((CFnFIFO_Struct *)m_Control.StructAddress())->m_Pos); return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      CFnFIFO_Struct &F= *(CFnFIFO_Struct *)m_Control.StructAddress();
      if (m_Plc->m_bReset)
        {
        F.m_Pos=0;
        }
      if (On && !F.m_EN)
        {
        F.m_EN=true;
        if (F.m_Pos<F.m_Len)
          {
          m_Plc->SetIndexRegister(F.m_Pos);
          float V=m_Src.ValueAsFlt();

          //if (0)  // Debug Test
          //  {
          //  static int x=0;
          //  V=(float)x++;
          //  }

          m_FIFO.SetValueAsFlt(V);
          F.m_Pos++;
          }
        F.m_DN=(F.m_Pos>=F.m_Len);
        }
      else if (!On)
        {
        F.m_EN=false;
        }
      return F.m_EN;
      };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return (cpt.y<=2) ? &m_Src : (cpt.y<=3) ? &m_FIFO : &m_Control; } ;
    
    virtual long      NAddressFlds()  { return 3; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return &m_Src;
        case 1: return &m_FIFO;
        case 2: return &m_Control;
        };
      return NULL;
      }
  };

class CFnFFU : public CFnBlk
  {
  protected:
    CAddress    m_FIFO;
    CAddress    m_Dst;
    CAddress    m_Control;

  public:
    CFnFIFO_Struct & Struct() { return *(CFnFIFO_Struct *)m_Control.StructAddress(); };

    CFnFFU(CPLC5 * Plc, byte Cd, CTknFile & Tkns): 
      CFnBlk(Plc, Cd, Tkns),
      m_FIFO(Plc),
      m_Dst(Plc),
      m_Control(Plc)
      {
      m_FIFO.Parse(Tkns.StrToken());
      m_Dst.Parse(Tkns.StrToken());
      m_Control.Parse(Tkns.StrToken());

      CFnFIFO_Struct & F=Struct();
      F.m_Len=(short)Tkns.IntToken();
      F.m_Pos=(short)Tkns.IntToken();

      m_Wide=Max(21,(int)strlen(m_Dst.Text())+12+5);
      m_Wide=Max(m_Wide, (byte)(strlen(m_FIFO.Text())+5+5));
      m_Wide=Max(m_Wide, (byte)(strlen(m_Control.Text())+8+5));
      };
    virtual ~CFnFFU() { };
    virtual byte PictWide()   { return m_Wide; };
    virtual byte PictHeight() { return 8; };
    virtual bool PictText(int Row, int Wide, CString &S)
      {
      switch (Row)
        {
        case 0: S.Format("FIFO:%*s",        Wide-5, m_FIFO.Text()); return true;
        case 1: S.Format("Destination:%*s", Wide-12,m_Dst.Text()); return true;
        case 2: S.Format("Control:%*s",     Wide-8, m_Control.Text()); return true;
        case 3: S.Format("Length:%*i",      Wide-7, Struct().m_Len); return true;
        case 4: S.Format("Position:%*i",    Wide-9, Struct().m_Pos); return true;
        }
      return false;
      };
    virtual bool SetEnabled(bool On)
      {
      CFnFIFO_Struct &F=Struct();
      if (On && !F.m_EU)
        {
        F.m_EU=true;
        if (F.m_Pos>0)
          {
          m_Plc->SetIndexRegister(0);//4=F.m_Pos;//float A=m_A.ValueAsFlt();
          float V=m_FIFO.ValueAsFlt();
          m_Dst.SetValueAsFlt(V);
          for (int i=0; i<F.m_Pos; i++)
            {
            m_Plc->SetIndexRegister(i+1);
            float V=m_FIFO.ValueAsFlt();
            m_Plc->SetIndexRegister(i);
            m_FIFO.SetValueAsFlt(V);
            }

          F.m_Pos--;
          }
        F.m_EM=(F.m_Pos<=0);
        }
      else if (!On)
        {
        F.m_EU=false;
        }
      return F.m_EU;
      };
    
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return (cpt.y<=2) ? &m_FIFO : (cpt.y<=3) ? &m_Dst : &m_Control; } ;
    
    virtual long      NAddressFlds()  { return 3; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return &m_FIFO;
        case 1: return &m_Dst;
        case 2: return &m_Control;
        } ;
      return NULL;
      }
  };

//===========================================================================
//
//
//
//===========================================================================

class CFnSimpleA : public CFnBase
  {
  public:
    CAddress m_Add;

    CFnSimpleA(CPLC5 * Plc, byte Cd, CTknFile & Tkns):
      CFnBase(Plc, Cd, Tkns),
      m_Add(Plc)
      {
      m_Add.Parse(Tkns.StrToken());
      }
    virtual ~CFnSimpleA()
      {
      }
    virtual byte PictWide()   { return Max(15, 2+(int)strlen(m_Add.Text())); };
    virtual byte PictHeight() { return 4; };
    virtual byte PictOffset() { return m_Add.DescCount(); };

    virtual LPCTSTR AddressText()     { return m_Add.Text(); };
    virtual int     DescCount()       { return m_Add.DescCount(); };
    virtual LPCTSTR DescText(int i)   { return m_Add.DescText(i); };

    //virtual bool Enabled();
    virtual bool SetEnabled(bool On);
    //virtual void BuildTxtImage(CRungImage & Img);
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return &m_Add; } ;
    
    virtual long      NAddressFlds()  { return 1; };
    virtual CAddress* AddressFld(int Index)
      { 
      switch (Index)
        {
        case 0: return &m_Add; 
        } ;
      return NULL;
      }
  };

//===========================================================================

//bool CFnSimpleA::Enabled()
//  {
//  switch (m_Cd)
//    {
//    case Fn_XIC: return m_Add.BValue();
//    case Fn_XIO: return !m_Add.BValue();
//    case Fn_ONS: return m_Add.BValue();
//    }
//  return false;
//  } ;

//===========================================================================

bool CFnSimpleA::SetEnabled(bool On)
  {
  bool B0=m_Add.BValue(); 
  bool Ret = false;
  switch (m_Cd)
    {
    case Fn_XIC: return On && m_Add.BValue();
    case Fn_XIO: return On && !m_Add.BValue();
    case Fn_ONS: 
      { 
      bool B=m_Add.BValue(); 
      m_Add.SetBValue(On, m_pRung); 
      Ret=!B && On; 
      break;
      }
    case Fn_OTE: 
      {
      Ret= m_Add.SetBValue(On, m_pRung);
      break;
      }
    case Fn_OTL: 
      if (On) 
        m_Add.SetBValue(true, m_pRung); 
      Ret=m_Add.BValue();
      break;
    case Fn_OTU: 
      if (On) 
        m_Add.SetBValue(false, m_pRung); 
      Ret=m_Add.BValue();
      break;
    case Fn_RES:
      {
      switch (m_Add.m_Txt[0])
        {
        case 'C':
          {
          CFnCounter_Struct *p=(CFnCounter_Struct *)m_Add.StructAddress();
          if (On)
            p->Reset();
          Ret= On;
          break;
          }
        case 'T':
          {
          CFnTimer_Struct *p=(CFnTimer_Struct *)m_Add.StructAddress();
          if (On)
            p->Reset();
          Ret= On;
          break;
          }
        }
      break;
      }
    }
  //if (Ret!=B0)
  //  { 
  //  if (dbgf) 
  //    {
  //    CAddInfo * p=m_Plc->FindAddress(m_Add.m_Txt);
  //    char buff[1000];
  //    sprintf(buff,"[%-6s:%2i:%4i] %-12s = %s : %-15s %s\n",
  //      (LPCTSTR)m_pRung->m_pLadder->m_pPLC->m_sPlcName,
  //      m_pRung->m_pLadder->m_iIndex, m_pRung->m_iRung,
  //      (LPCTSTR)m_Add.m_Txt, Ret?"1":".", 
  //      p?(LPCTSTR)p->m_sSymbol:"",
  //      p?(LPCTSTR)p->FullDescText():"");
  //    
  //    
  //    fprintf(dbgf,"%s", buff);
  //    //::AfxTrace("%s", buff);
  //    }
  //  };
  return Ret;
  };

//===========================================================================

class CFnSimpleN : public CFnBase
  {
  public:
    CString   m_Txt;
    int       m_No;

    CFnSimpleN(CPLC5 * Plc, byte Cd, CTknFile & Tkns): CFnBase(Plc, Cd, Tkns)
      {
      m_No=Tkns.IntToken();
      m_Txt.Format("%i", m_No);
      }
    virtual ~CFnSimpleN()
      {
      }
    virtual byte PictWide()   { return 15; };
    virtual byte PictHeight() { return 4; };
    virtual byte PictOffset() { return 0; };

    virtual LPCTSTR AddressText()     { return m_Txt; };

    //virtual bool Enabled();
    virtual bool SetEnabled(bool On);
    //virtual void BuildTxtImage(CRungImage & Img);
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)  { return NULL; } ;
    
    virtual long      NAddressFlds()  { return 0; };
    virtual CAddress* AddressFld(int Index)
      { 
 //     switch (Index)
 //       {
 //       case 0: return &m_Add; 
 //       } ;
      return NULL;
      }
  };

//===========================================================================

//bool CFnSimpleN::Enabled()
//  {
//  switch (m_Cd)
//    {
//    case Fn_LBL: return true;
//    case Fn_JMP: return true;
//    }
//  return false;
//  } ;

//===========================================================================

bool CFnSimpleN::SetEnabled(bool On)
  {
  switch (m_Cd)
    {
    case Fn_LBL: return On;
    case Fn_JMP: if (On) { MyLadder().SetNextRung(m_No); }; return On;
    }
  return false;
  };

//===========================================================================
//
//
//
//===========================================================================

void CFnBase::SBuildTxtImage(CRungImage & Img, CMtxRow &R, CMtxElement & E)
  {
  int c=E.m_X;
  int r=R.m_Yr;
  CString S;

  if (FnList[E.m_Cd].m_Type==Typ_Other)
    {
    if (E.m_Cd==Fn_NULL)
      {
      int Off=-1;
      if (E.m_LftRail)
        Off=0;
      else if (E.m_RgtRail)
        Off=/*E.m_X+*/E.m_AWd-1;
      if (Off>=0)
        {
        char * Dest=&Img.m_C[r][c];
        int T=R.m_Yt;
        int B=R.m_Yt+R.m_Ht;//-1;
        for (int y=T; y<=B; y++)
          Img.m_C[y][c+Off]='|';
        }
      }
    else
      {
      byte IsRail=E.m_Cd==Fn_SOR || E.m_Cd==Fn_EOR;
      char * Dest=&Img.m_C[r][c];
      //const char *p=FnList[E.m_Cd].m_Pict;
      //int l=strlen(p);

      char PictBuff[32];
      strcpy(PictBuff, FnList[E.m_Cd].m_Pict);
      int l=strlen(PictBuff);
      if (E.m_LftPwr && PictBuff[0]=='-')
        PictBuff[0]='=';
      if (E.m_RgtPwr && PictBuff[l-1]=='-')
        PictBuff[l-1]='=';

      int w=E.m_AWd;
      int xl=E.m_AWd-l;
      int xl0=Max(0,xl/2);
      int xl1=Max(0,xl-xl0);
      if (!IsRail)
        {
        memset(&Dest[0], PictBuff[0], xl0);
        memcpy(&Dest[xl0], PictBuff, l);
        memset(&Dest[xl0+l], PictBuff[l-1], xl1);
        Dest[xl0+l+xl1]=0;
        }

      char PwrChar=E.m_LftPwr ? '=':'-';
      int O=1;
      int T=0;
      int B=-1;
      switch (E.m_Cd)
        {
        case Fn_SOR:
          O=0;
          T=R.m_Yt;
          B=R.m_Yt+R.m_Ht-1;
          memset(&Dest[0], PwrChar, w);
          Dest[0]='+';
          break;
        case Fn_EOR:
          O=E.m_AWd-1-xl0;
          T=R.m_Yt;
          B=R.m_Yt+R.m_Ht-1;
          memset(&Dest[0], PwrChar, w);
          Dest[w-1]='+';
          break;
        case Fn_BST:
        case Fn_BND:
          T=R.m_Yr;
          B=R.m_Yt+R.m_Ht-1;
          break;
        case Fn_UConn:
        case Fn_DConn:
          T=R.m_Yt;
          B=R.m_Yt+R.m_Ht-1;
          break;
        case Fn_SConn:
          T=R.m_Yt;
          B=E.m_CdB==Fn_SConn ? R.m_Yt+R.m_Ht-1 : R.m_Yr;
          break;
        case Fn_EConn:
          T=R.m_Yt;
          B=E.m_CdB==Fn_EConn ? R.m_Yt+R.m_Ht-1 : R.m_Yr;
          break;
        }
      for (int y=T; y<=B; y++)
        {
        char * Dest=&Img.m_C[y][c];
        char C='|';
        if (Dest[xl0+O]==' ')
          Dest[xl0+O]=C;
        }
      }
    }
  else
    ASSERT(0);

  memmove(&Img.m_C[r][c], LPCTSTR(S), S.GetLength());
  Img.m_Width=Max(Img.m_Width, E.m_X+E.m_AWd);
  Img.m_Height=Max(Img.m_Height, R.m_Yt+R.m_Ht);
  };


#define SET(r,c, ch, n) memset(&Img.m_C[(r)][(c)], (ch), (n));
#define CPY(r,c, s)     memcpy(&Img.m_C[(r)][(c)], s, strlen(s));
#define CHR(r,c)        Img.m_C[(r)][(c)]


void CFnBase::VBuildTxtImage(CRungImage & Img, CMtxRow &R, CMtxElement & E)
  {
  int c=E.m_X;
  int r=R.m_Yr;
  switch (FnList[m_Cd].m_Type)
    {
    case Typ_SimpleA:
    case Typ_SimpleN:
      {
      char * Dest=&Img.m_C[r][c];
      char PictBuff[32];
      strcpy(PictBuff, FnList[m_Cd].m_Pict);
      int l=strlen(PictBuff);
      if (E.m_LftPwr && PictBuff[0]=='-')
        PictBuff[0]='=';
      if (E.m_RgtPwr && PictBuff[l-1]=='-')
        PictBuff[l-1]='=';
      int xl=E.m_AWd-l;
      int xl0=Max(0,xl/2);
      int xl1=Max(0,xl-xl0);
      memset(&Dest[0], PictBuff[0], xl0);
      memcpy(&Dest[xl0], PictBuff, l);
      memset(&Dest[xl0+l], PictBuff[l-1], xl1);

      Dest=&Img.m_C[r+1][c];
      LPCTSTR pTxt=E.m_pFn->AddressText();
      l=strlen(pTxt);
      xl=E.m_AWd-l;
      xl0=Max(0,xl/2);
      memcpy(&Dest[xl0], pTxt, l);

      CAddress *pAdd=E.Fn().AddressFld(0);
      if (pAdd && pAdd->IsBValueFrc())
        Img.m_Colour[r+1]=1;

      int n=E.m_pFn->DescCount();
      for (int i=0; i<n; i++)
        {
        Dest=&Img.m_C[r-n+i][c];
        LPCTSTR pTxt=E.m_pFn->DescText(i);
        l=strlen(pTxt);
        xl=E.m_AWd-l;
        xl0=Max(0,xl/2);
        memcpy(&Dest[xl0], pTxt, l);
        }
      //void *pVoid;
      //if (MyPLC().m_SymMap.Lookup(pTxt, pVoid))
      //  {
      //  CAddInfo *pInfo=(CAddInfo*)pVoid;
      //  Dest=&Img.m_C[r+2][c];
      //  LPCTSTR pTxt=pInfo->m_Desc[0];
      //  l=strlen(pTxt);
      //  xl=E.m_AWd-l;
      //  xl0=Max(0,xl/2);
      //  memcpy(&Dest[xl0], pTxt, l);
      //  int xx=0;
      //  }
      }
      break;
    case Typ_Block:
      {
      //const char *ptxt=FnList[m_Cd].m_Txt;
      //const char *pdesc=FnList[m_Cd].m_Desc;
      //int l=strlen(ptxt);
      int w=E.m_AWd-2;//
      c++;

      int T=r-E.m_Of;
      int B=r+E.m_Ht-E.m_Of-1;

      SET(T, c, '-', w);
      CPY(T, c+3, FnList[m_Cd].m_Txt);

      CString S;
      CPY(T+1, c+1, FnList[m_Cd].m_Desc);
      for (int i=0; E.FnBlk().PictText(i, w-2, S); i++)
        CPY(T+2+i, c+1, S);

      SET(B, c, '-', w);

      for (int y=T; y<=B; y++)
        {
        char C=(y==T || y==B ||y==r) ? '+' : '|';
        CHR(y, c)=C;
        CHR(y, c+w-1)=C;
        }
      c--;

      CHR(r, c)=E.m_LftPwr ? '=' : '-';
      CHR(r, c+w+1)=E.m_RgtPwr ? '=' : '-';
      }
      break;
    default:
      ASSERT(FALSE);
      break;
    }
  Img.m_Width=Max(Img.m_Width, E.m_X+E.m_AWd);
  Img.m_Height=Max(Img.m_Height, R.m_Yt+R.m_Ht);
  };

//===========================================================================
//
//
//
//===========================================================================

static CTableRoot s_TableRoots[]=
  {
  //  Nm  Df Lo    Hi  Octal, IsFlt, HexLoad, VarType,Val/Col, ColWide, StructVals, ReqdFmt            LoadFmt
  //                                                                                       1234567890123456
    {"O",  0, 0,    0,  true, false,   true,   VT_I2,      8,       9,          1, "                ", " %x %x %x %x %x %x %x %x",       },
    {"I",  1, 1,    1,  true, false,   true,   VT_I2,      8,       9,          1, "                ", " %x %x %x %x %x %x %x %x",       },
    {"S",  2, 2,    2, false, false,  false,   VT_I2,     10,       9,          1, "                ", " %i %i %i %i %i %i %i %i %i %i", },
    {"B",  3, 3, 1000, false, false,  false,   VT_I2,     10,       9,          1, "                ", " %i %i %i %i %i %i %i %i %i %i", },
    {"T",  4, 3, 1000, false, false,  false,   VT_I2,      1,       9,          3, "xii             ", " %x %i %i",                      },
    {"C",  5, 3, 1000, false, false,  false,   VT_I2,      1,       9,          3, "xii             ", " %x %i %i",                      },
    {"R",  6, 3, 1000, false, false,  false,   VT_I2,      1,       9,          3, "xii             ", " %x %i %i",                      },
    {"N",  7, 3, 1000, false, false,  false,   VT_I2,     10,       9,          1, "                ", " %i %i %i %i %i %i %i %i %i %i", },
    {"F",  8, 3, 1000, false,  true,  false,   VT_R4,      5,      16,          1, "                ", " %g %g %g %g %g",                },
    {NULL},
  };

inline CTableRoot *FindTableRoot(const char* Nm)
  {
  for (int i=0; s_TableRoots[i].m_sRootName; i++)
    if (strcmp( s_TableRoots[i].m_sRootName, Nm)==0)
      return &s_TableRoots[i];
  return NULL;
  }

//===========================================================================
//
//
//
//===========================================================================

struct CValidMnemonicRec
  {
  const char * m_Tbl;
  const char *m_Mne;
  bool m_IsBit;
  union { byte m_iBitNo; byte m_Offset; };
  //const char * InsList;
  };

const CValidMnemonicRec ValidMnemonics[] =
  {
    {"C",  "PRE",   false,  1, }, //"CTD,CTU",     },
    {"C",  "ACC",   false,  2, }, //"CTD,CTU",     },
    {"C",  "CU",    true,  15, }, //"CTD,CTU",     },
    {"C",  "CD",    true,  14, }, //"CTD,CTU",     },
    {"C",  "DN",    true,  13, }, //"CTD,CTU",     },
    {"C",  "OV",    true,  12, }, //"CTD,CTU",     },
    {"C",  "UN",    true,  11, }, //"CTD,CTU",     },

    {"T",  "PRE",   false,  1, }, //"TON,TOF,RTO",  },
    {"T",  "ACC",   false,  2, }, //"TON,TOF,RTO",  },
    {"T",  "EN",    true,  15, }, //"TON,TOF,RTO",  },
    {"T",  "TT",    true,  14, }, //"TON,TOF,RTO",  },
    {"T",  "DN",    true,  13, }, //"TON,TOF,RTO",  },

    {"R",  "LEN",   false,  1, }, //"FFL,FFU,FBC",  },
    {"R",  "POS",   false,  2, }, //"FFL,FFU,FBC",  },
    {"R",  "EN",    true,  15, }, //"FFL,FFU,FBC",  },
    {"R",  "EU",    true,  14, }, //"FFL,FFU",      },
    {"R",  "DN",    true,  13, }, //"FFL,FFU,FBC",  },
    {"R",  "EM",    true,  12, }, //"FFL,FFU",      },
    {"R",  "ER",    true,  11, }, //"FFL,FFU,FBC",  },
    {"R",  "IN",    true,   9, }, //"FFL,FFU,FBC",  },
    {"R",  "FD",    true,   8, }, //"FFL,FFU,FBC",  },

    {NULL},
  };

//===========================================================================
//
//
//
//===========================================================================

void CTable::Initialise(CTableRoot * pTbRt, const int TabNum, const char* Nm, const char* NmTxt, const char* NmNo, int nVals)
  {
  m_pRoot=pTbRt;
  m_iIndex=TabNum;
  m_sName=Nm;
  m_sTxt=NmTxt;
  m_sNum=NmNo;
  m_nVals=nVals;

  if (m_pRoot->m_VarType==VT_R4)
    {
    m_FVals.SetSize(m_nVals*m_pRoot->m_nStructVals);
    m_FValsPrev.SetSize(m_nVals*m_pRoot->m_nStructVals);
    }
  else
    {
    m_IVals.SetSize(m_nVals*m_pRoot->m_nStructVals);
    m_IValsPrev.SetSize(m_nVals*m_pRoot->m_nStructVals);
    m_IValsOn.SetSize(m_nVals*m_pRoot->m_nStructVals);
    m_IValsOff.SetSize(m_nVals*m_pRoot->m_nStructVals);
    }
  };

//---------------------------------------------------------------------------

void CTable::Reset()
  {
  if (m_pRoot->m_sRootName=="I")
    {
    }
  else if (m_pRoot->m_sRootName=="T" ||
           m_pRoot->m_sRootName=="C" ||
           m_pRoot->m_sRootName=="R")
    {
    for (int i=0; i<m_nVals; i++)
      {
      SetIValue(i,0,0x0fff & IValue(i,0));
      SetIValue(i,2,0); // ACC
      }
    }
  else if (m_pRoot->m_sRootName=="S")
    {
    for (int i=0; i<m_nVals; i++)
      SetIValue(i,0,0);
    }
  else if (m_pRoot->m_sRootName=="F")
    {
    for (int i=0; i<m_nVals; i++)
      SetFValue(i,0,0.0);
    }
  else
    {
    for (int i=0; i<m_nVals; i++)
      SetIValue(i,0,0);
    }
  }

//---------------------------------------------------------------------------

LPCTSTR CAddress::ParseNo(LPCTSTR pTxt, CUIndAdd & uNo, byte &IsInd, int Default, CTableRoot * pTblRt, const char * Terms)
  {
  // FileNumber
  IsInd=*pTxt=='[';
  if (IsInd)
    {
    LPCTSTR popen=pTxt+1;
    char * pclose=strchr(popen, ']');
    *pclose=0;
    uNo.m_pAdd=new CAddress(m_Plc);
    uNo.m_pAdd->Parse(popen);
    return pclose+1;
    }

  if (isdigit(*pTxt))
    {
    int i;
    int j=sscanf(pTxt, pTblRt->m_OctalAdd ? "%o" : "%i", &i);
    if (j>0)
      {
      uNo.m_iIndex = i;
      while (isdigit(*pTxt))
        pTxt++;
      }
    else
      uNo.m_iIndex = Default;
    }
  else if (isalpha(*pTxt))
    {
    uNo.m_iIndex = 0;
    m_iOffInStruct=0;
    for (int i=0; ValidMnemonics[i].m_Tbl; i++)
      {
      const CValidMnemonicRec & VM=ValidMnemonics[i];

      if (strcmp(pTblRt->m_sRootName, VM.m_Tbl)==0 && stricmp(pTxt, VM.m_Mne)==0)
        {
        if (VM.m_IsBit)
          {
          m_iOffInStruct=0;
          m_uBit.m_iIndex=VM.m_iBitNo;
          //m_BitMsk=(1<<m_uBitNo.m_iIndex);
          m_IsAddOfBit=true;
          }
        else
          {
          m_iOffInStruct=VM.m_Offset;
          m_IsAddOfBit=false;
          }
        pTxt+=strlen(VM.m_Mne);
        break;
        }
      }
    if (ValidMnemonics[i].m_Tbl==NULL)
      {
      TRACE("Unknown Mne '%s':'%s'\n",pTblRt->m_sRootName,pTxt);
      }
    }
  else
    uNo.m_iIndex = Default;

  if (*pTxt && Terms)
    CheckThat(strchr(Terms, *pTxt)!=NULL, "Terminator not valid");

  return pTxt;
  }

//---------------------------------------------------------------------------

bool CAddress::Parse(LPCTSTR Txt, bool Complain)
  {

  m_IsValid=false;
  m_Txt=Txt;

  m_bIndexed=Txt[0]=='#';
  if (m_bIndexed)
    Txt++;

  // File Type
  char sFile[4];
  for (int i=0; i<3 && isalpha(Txt[i]); i++)
    sFile[i]=Txt[i];
  sFile[i]=0;

  LPCTSTR pTerm;
  CTableRoot * pTblRt=FindTableRoot(sFile);
  if (pTblRt==NULL)
    goto Bad;

  pTerm=ParseNo(&Txt[i], m_uFile, m_bIndFileNo, pTblRt->m_TbNumDef, pTblRt, ":;/.");

  if (m_uFile.m_iIndex<0 || m_uFile.m_iIndex>=MaxTables)
    goto Bad;

  if (m_Plc->m_Table[m_uFile.m_iIndex]==NULL)
    goto Bad;

  m_IsAddOfBit=false;
  m_IsBitAddRelToWrd=false;
  switch (*pTerm)
    {
    case '/':
    case '.':
      // Only for 'B' table
      ParseNo(pTerm+1, m_uBit, m_bIndBitNo, 0, pTblRt, NULL);
      m_IsAddOfBit=true;
      break;
    case ':':
      pTerm=ParseNo(pTerm+1, m_uWord, m_bIndWordNo, 0, pTblRt, "./");
      if (pTerm && *pTerm)
        {
        if (*pTerm=='/' || *pTerm=='.')
          {
          m_IsAddOfBit=true;
          ParseNo(pTerm+1, m_uBit, m_bIndBitNo, 0, pTblRt, NULL);
          if (m_IsAddOfBit)
            m_IsBitAddRelToWrd=true;
          }
        else
          goto Bad;
        }
      break;
    }

  m_IsValid=CheckValidAddress();
  if (!m_IsBitAddRelToWrd)
    {
    }
  else
    {
    // Check - This does get called before the tables are built 
    if (m_Plc->m_Table[m_uFile.m_iIndex]!=NULL)
      if (m_uWord.m_iIndex>=m_Plc->m_Table[m_uFile.m_iIndex]->m_nVals)
        m_IsValid=false;
    }
  m_pInfo=SymbolF(m_Txt);

  if (0 && dbgf)
    {
    char dbgFBuff[32], dbgWBuff[32], dbgBBuff[32];
    sprintf(dbgFBuff, m_bIndFileNo ? "<i>":m_uFile.m_iIndex<0 ? "":"%i", m_uFile.m_iIndex);
    sprintf(dbgWBuff, m_bIndWordNo ? "<i>":m_uWord.m_iIndex<0 ? "":"%i", m_uWord.m_iIndex);
    sprintf(dbgBBuff, m_bIndBitNo ?  "<i>":m_uBit.m_iIndex<0 ? "":"%i", m_uBit.m_iIndex);
    fprintf(dbgf,"Add %s: %-12.12s: %3.3s.%4.4s.%5.5s\n","",Txt, dbgFBuff, dbgWBuff, dbgBBuff);
    }

  return m_IsValid;

Bad:
  if (dbgf)
    {
    char dbgFBuff[32], dbgWBuff[32], dbgBBuff[32];
    sprintf(dbgFBuff, m_bIndFileNo ? "<i>":"%i", m_uFile.m_iIndex);
    sprintf(dbgWBuff, m_bIndWordNo ? "<i>":"%i", m_uWord.m_iIndex);
    sprintf(dbgBBuff, m_bIndBitNo ? "<i>":"%i", m_uBit.m_iIndex);
    fprintf(dbgf,"Add %s: %-12.12s: %3.3s.%4.4s.%5.5s\n","BAD",Txt, dbgFBuff, dbgWBuff, dbgBBuff);
    }
  if (Complain)
    {
    Beep(1500, 20);
    //Sleep(20);
    //Beep(1000, 20);
    //Sleep(20);
    //Beep(1000, 20);
    //_asm int 3;
    }
  return false;
  }

//---------------------------------------------------------------------------

VARTYPE CAddress::VarType()
  {
  if (!m_IsValid)
    return VT_EMPTY;

  if (m_IsAddOfBit)
    return VT_BOOL;
  // File Type
  char sFile[4];
  for (int i=0; i<3 && isalpha(m_Txt[i]); i++)
    sFile[i]=m_Txt[i];
  sFile[i]=0;
  CTableRoot * pTblRt=FindTableRoot(sFile);
  if (pTblRt==NULL)
    return VT_EMPTY;

  return pTblRt->m_VarType;
  }

//---------------------------------------------------------------------------

bool CAddress::CheckValidAddress()
  {
  if (!m_bIndFileNo && m_uFile.m_iIndex==2)
    {
    // Check Addresses Status Words Bits are implemented
    if (m_uWord.m_iIndex==1 && m_uBit.m_iIndex==7)
      {
      // Remote Switch
      }
    else if (m_uWord.m_iIndex==1 && m_uBit.m_iIndex==9)
      {
      // Forces Present
      }
    else if (m_uWord.m_iIndex==1 && m_uBit.m_iIndex==15)
      {
      // First Scan
      }
    else if (m_uWord.m_iIndex==10)
      {
      // Minor Fault
      }
    else if (m_uWord.m_iIndex==11)
      {
      // Major Fault
      }
//    else if (m_uWord.m_iIndex==7)
//      {
//      // Rack Fault
//      }
    else
      {
      CString S;
      S.Format("Unimplemented Status Info %s", m_Txt);
      CheckThat(FALSE, S);
      return false;
      }
    }
  return true;
  }

//---------------------------------------------------------------------------

bool    CAddress::BValue()            { return Table()->BValue(BitNo(true)); };
bool    CAddress::IValueAsBit()       { return Table()->BValue(BitNo(true));  };
short   CAddress::IValue()            { return Table()->IValue(WordNo(true), m_iOffInStruct);  };
float   CAddress::FValue()            { return Table()->FValue(WordNo(true), m_iOffInStruct);  };
bool    CAddress::SetBValue(bool On, CRung * pRung, LPCTSTR pText)  { return Table()->SetBValue(BitNo(true), On, *this, pRung, pText); };
short   CAddress::SetIValue(short S)  { return Table()->SetIValue(WordNo(true), m_iOffInStruct, S);  };
float   CAddress::SetFValue(float F)  { return Table()->SetFValue(WordNo(true), m_iOffInStruct, F);  };

short   CAddress::IValue(int iShift)              { return Table()->IValue(WordNo(true)+iShift, m_iOffInStruct);  };
float   CAddress::FValue(int iShift)              { return Table()->FValue(WordNo(true)+iShift, m_iOffInStruct);  };
short   CAddress::SetIValue(int iShift, short S)  { return Table()->SetIValue(WordNo(true)+iShift, m_iOffInStruct, S);  };
float   CAddress::SetFValue(int iShift, float F)  { return Table()->SetFValue(WordNo(true)+iShift, m_iOffInStruct, F);  };

bool    CAddress::BValueFrc()           { return Table()->BValueFrc(BitNo(true)); };
short   CAddress::IValueFrc()           { return Table()->IValueFrc(WordNo(true), m_iOffInStruct);  };
void    CAddress::SetBValueFrc(bool On) { Table()->SetBValueFrc(BitNo(true), On); };
void    CAddress::SetIValueFrc(short S) { Table()->SetIValueFrc(WordNo(true), m_iOffInStruct, S);  };
void    CAddress::ClrBValueFrc()        { Table()->ClrBValueFrc(BitNo(true)); };
void    CAddress::ClrIValueFrc()        { Table()->ClrIValueFrc(WordNo(true), m_iOffInStruct);  };
bool    CAddress::IsBValueFrc()         { return Table()->IsBValueFrc(BitNo(true)); };
bool    CAddress::IsIValueFrc()         { return Table()->IsIValueFrc(WordNo(true), m_iOffInStruct);  };

void *  CAddress::StructAddress()                   { return Table()->IValuePtr(WordNo(true), m_iOffInStruct); };
float   CAddress::ValueAsFlt()                      { return Table()->ValueAsFlt(WordNo(true), m_iOffInStruct);  };
void    CAddress::SetValueAsFlt(float Val)          { Table()->SetValueAsFlt(WordNo(true), m_iOffInStruct, Val); };
float   CAddress::ValueAsFlt(int iOff)              { return Table()->ValueAsFlt(WordNo(true), iOff);  };
void    CAddress::SetValueAsFlt(int iOff, float Val){ Table()->SetValueAsFlt(WordNo(true), iOff, Val); };

//===========================================================================
//
//
//
//===========================================================================

//===========================================================================
//
//
//
//===========================================================================

void CPLC5Array::InitTree(CTreeCtrl & m_Tree)
  {
  if (m_Tree.GetCount()>0)
    m_Tree.DeleteAllItems();

  HTREEITEM hPLC, hTable, hLadder, hSymbols, hForces;
  TV_INSERTSTRUCT IS;
  IS.hInsertAfter = TVI_SORT;
  IS.item.mask = TVIF_TEXT;// | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
  IS.item.state = 0;
  IS.item.stateMask = 0;
  IS.item.iImage = 0;
  IS.item.iSelectedImage = 0;
  IS.item.cChildren = 0;
  IS.item.lParam = 0;

  // ------------------------------------------

  for (int p=0; p<NPLCs();p++)
    {
    CPLC5 &P=PLC(p);
  
    IS.hParent = TVI_ROOT;
    IS.item.pszText = (LPTSTR)(LPCTSTR)P.m_sPlcName;
    hPLC = m_Tree.InsertItem(&IS);
    m_Tree.SetItemState(hPLC, TVIS_BOLD, TVIS_BOLD);
    m_Tree.SetItemData(hPLC, 0);//MakeItemData(Vw_Table, p, MaxTables));

    IS.hParent = hPLC;
    IS.item.pszText = "Data Tables";
    hTable = m_Tree.InsertItem(&IS);
    m_Tree.SetItemState(hTable, TVIS_BOLD, TVIS_BOLD);
    m_Tree.SetItemData(hTable, MakeItemData(Vw_Table, p, MaxTables));

    for (int t=0; t<P.m_nTables; t++)
      {

      if (P.m_Table[t])
        {
        const char * pNm=P.Table(t).m_sName;
        const char * pTxt=P.Table(t).m_sTxt;

        int iCnt=0;
        for (int tt=0; tt<P.m_nTables; tt++)
          if (P.m_Table[tt] && P.Table(tt).m_sTxt==pTxt)
            iCnt++;

        const char * pNum=P.Table(t).m_sNum;
        HTREEITEM ht=m_Tree.GetChildItem(hTable);
        int Cmp=1;
        while (ht)
          {
          CString T=m_Tree.GetItemText(ht);
          Cmp=T.Compare(pTxt);
          if (Cmp>=0)
            break;
          else
            ht=m_Tree.GetNextSiblingItem(ht);
          }
        if (iCnt==1)
          {
          IS.hParent = hTable;
          IS.item.pszText = (char*)pNm;
          IS.hInsertAfter=TVI_SORT;
          ht=m_Tree.InsertItem(&IS);
          m_Tree.SetItemData(ht, MakeItemData(Vw_Table, p, t));
          }
        else
          {
          if (Cmp>0 || ht==NULL)
            {
            IS.hParent = hTable;
            IS.item.pszText = (char*)pTxt;
            IS.hInsertAfter=TVI_SORT;
            ht=m_Tree.InsertItem(&IS);
            m_Tree.SetItemData(ht, MakeItemData(Vw_Table, p, MaxTables));
            }

          if (strlen(pNum)>0)
            {
            IS.hParent = ht;
            IS.item.pszText = (char*)pNum;
            HTREEITEM hi=m_Tree.InsertItem(&IS);
            IS.hInsertAfter=TVI_SORT;
            m_Tree.SetItemData(hi, MakeItemData(Vw_Table, p, t));
            }
          else
            m_Tree.SetItemData(ht, MakeItemData(Vw_Table, p, t));
          }

        }
      }
    //m_Tree.Expand(hTable, TVE_EXPAND);

    // ------------------------------------------
    IS.hParent = hPLC;
    IS.hInsertAfter= hTable;//TVI_SORT;
    IS.item.pszText = "Ladder";
    hLadder = m_Tree.InsertItem(&IS);
    m_Tree.SetItemState(hLadder, TVIS_BOLD, TVIS_BOLD);
    m_Tree.SetItemData(hLadder, MakeItemData(Vw_Ladder, p, P.m_nLadders));

    CString S;
    for (int l=0; l<P.m_nLadders; l++)
      if (P.m_Ladder[l].m_iIndex>=0)
        {
        IS.hParent = hLadder;
        S.Format("%i) %s", l, P.m_Ladder[l].m_sName);
        IS.item.pszText = (char*)LPCTSTR(S);
        HTREEITEM hi=m_Tree.InsertItem(&IS);
        IS.hInsertAfter=hi;
        m_Tree.SetItemData(hi, MakeItemData(Vw_Ladder, p, l));
        }

    //m_Tree.Expand(hLadder, TVE_EXPAND);

    // ------------------------------------------
    IS.hParent = hPLC;
    IS.hInsertAfter= hLadder;//TVI_SORT;
    IS.item.pszText = "Symbols";
    hSymbols = m_Tree.InsertItem(&IS);
    m_Tree.SetItemState(hSymbols, TVIS_BOLD, TVIS_BOLD);
    m_Tree.SetItemData(hSymbols, MakeItemData(Vw_Symbol, p, P.m_AddInfo.GetSize()));
    for (l=0; l<P.m_AddInfo.GetSize(); l++)
      {
      CAddInfo *pInfo=P.m_AddInfo[l];
      IS.hParent = hSymbols;
      S.Format("%s [%s]", LPCTSTR(pInfo->m_sSymbol), LPCTSTR(pInfo->m_sAddress));
      IS.item.pszText = (char*)LPCTSTR(S);
      HTREEITEM hi=m_Tree.InsertItem(&IS);
      IS.hInsertAfter=hi;
      m_Tree.SetItemData(hi, MakeItemData(Vw_Symbol, p, l));
      }
  
    // ------------------------------------------
    IS.hParent = hPLC;
    IS.hInsertAfter= hSymbols;//TVI_SORT;
    IS.item.pszText = "Forces";
    hForces = m_Tree.InsertItem(&IS);
    m_Tree.SetItemState(hForces, TVIS_BOLD, TVIS_BOLD);
    }
  // ------------------------------------------
  //m_Tree.Expand(hAtRoot, TVE_EXPAND);
  }

//===========================================================================
//
//
//
//===========================================================================

void CRung::BuildFnMatrixFn(byte Cd, int iFnNo, CCoOrd & RC, CTFnMatrix & Mtx)
  {
  Mtx.Cd(RC.m_Row, RC.m_Col)=Cd;
  CMtxElement &E=Mtx(RC.m_Row, RC.m_Col);
  if (iFnNo>=0)
    {
    CFnBase * F=m_Fns[iFnNo];
    F->m_pRung=this;
    E.m_pFn=F;
    E.m_Wd=F->PictWide();
    E.m_Ht=F->PictHeight();
    E.m_Of=F->PictOffset();
    }
  else
    {
    E.m_pFn=NULL;
    E.m_Wd=3;
    E.m_Ht=1;
    E.m_Of=0;
    }
  Mtx.m_iWide=Max(Mtx.m_iWide, RC.m_Col+1);
  Mtx.m_iHigh=Max(Mtx.m_iHigh, RC.m_Row+1);

  if (0 && dbgf)
    fprintf(dbgf,"  = %s [%2i,%2i]\n", FnList[Cd].m_Txt, RC.m_Row, RC.m_Col);
//    fprintf(dbgf,"  = %s [%2i,%2i] [%3i,%3i]\n", FnList[Cd].m_Txt, RC.m_Row, RC.m_Col, RC.m_Y, RC.m_X);
  }

//---------------------------------------------------------------------------

int CRung::BuildFnMatrixBr(int iFnNo, CCoOrd & RC, CCoOrd & RCEnd, CTFnMatrix & Mtx)
  {
  byte Cd=m_Fns[iFnNo]->m_Cd;
  ASSERT(Cd==Fn_BST);

  BuildFnMatrixFn(Cd, iFnNo, RC, Mtx);

  CCoOrd RCRoot = RC;
  CCoOrd RCLeft = RC;
  CCoOrd RCBrnch[MaxFnRows];
  RC.m_Col++;
  RCEnd  = RC;

  int BrNo=0;
  RCBrnch[BrNo]=RC;
  //if (dbgf) fprintf(dbgf,"  [ >   %3i %3i %3i\n", iFnNo, RC.m_Row, RC.m_Col);
  iFnNo++;
  for (bool BrDone=false; !BrDone && iFnNo<m_Fns.GetSize(); )
    {
    for (bool LegDone=false; !LegDone && iFnNo<m_Fns.GetSize(); )
      {
      byte Cd=m_Fns[iFnNo]->m_Cd;
      if (0 && dbgf) fprintf(dbgf,"                        * %s %2i [%2i,%2i][%2i,%2i] \n", FnList[Cd].m_Txt, iFnNo, RC.m_Row, RC.m_Col, RCEnd.m_Row, RCEnd.m_Col);
      switch (Cd)
        {
        case Fn_BST:
          {
          CCoOrd RCEndX;
          iFnNo=BuildFnMatrixBr(iFnNo, RC, RCEndX, Mtx);
          RCEnd=Max(RCEnd, RCEndX);
          RCBrnch[BrNo]=RC;
          }
          break;
        case Fn_BND:
          {
          RCEnd=Max(RCEnd, RC);

          RC.m_Col=RCEnd.m_Col;
          RC.m_Row=RCRoot.m_Row;

          for (int b=0;b<=BrNo;b++)
            {
            CCoOrd X;
            for ( ; RCBrnch[b].m_Col<RC.m_Col; RCBrnch[b].m_Col++)
              BuildFnMatrixFn(Fn_HConn, -1, RCBrnch[b], Mtx);
            BuildFnMatrixFn(Fn_EConn, -1, RCBrnch[b], Mtx);
            }
          for (b=BrNo;b>0;b--)
            {
            CCoOrd X;
            for (RCBrnch[b].m_Row-- ; RCBrnch[b].m_Row>RCBrnch[b-1].m_Row; RCBrnch[b].m_Row--)
              BuildFnMatrixFn(Fn_UConn, -1, RCBrnch[b], Mtx);
            }

          BuildFnMatrixFn(Cd, iFnNo, RC, Mtx);

          RC.m_Col++;
          LegDone=true;
          BrDone=true;
          iFnNo++;
          }
          break;
        case Fn_NXB:
          LegDone=true;
          RCEnd=Max(RCEnd, RC);

          RC.m_Col=RCRoot.m_Col;
          while (RCLeft.m_Row<RCEnd.m_Row)
            {
            RCLeft.m_Row++;
            RC.m_Row=RCLeft.m_Row;
            BuildFnMatrixFn(Fn_DConn, -1, RC, Mtx);
            }

          RCLeft.m_Row++;
          RC.m_Row=RCLeft.m_Row;
          BuildFnMatrixFn(Fn_SConn, -1, RC, Mtx);
          BrNo++;
          ASSERT(BrNo<MaxFnRows);
          RCBrnch[BrNo]=RC;

          RC.m_Col++;
          iFnNo++;
          break;
        default:;
          {
          CCoOrd RCEndX;
          iFnNo=BuildFnMatrix(iFnNo, RC, RCEndX, Mtx);
          RCEnd=Max(RCEnd, RCEndX);
          RCBrnch[BrNo]=RC;
          }
        }
      }
    }

  //if (dbgf) fprintf(dbgf,"  ] >   %3i %3i %3i\n", iFnNo, RC.m_Row, RC.m_Col);
  return iFnNo;
  };

//---------------------------------------------------------------------------

int CRung::BuildFnMatrix(int iFnNo, CCoOrd & RC, CCoOrd & RCEnd, CTFnMatrix & Mtx)
  {
  //if (dbgf) fprintf(dbgf,"    >   %3i %3i %3i\n", iFnNo, RC.m_Row, RC.m_Col);
  bool LegDone=false;
  RCEnd=Max(RCEnd, RC);
  while (!LegDone && iFnNo<m_Fns.GetSize())
    {
    byte Cd=m_Fns[iFnNo]->m_Cd;
    //if (dbgf) fprintf(dbgf,"    %s\n", FnList[Cd].m_Txt);
    if (0 && dbgf) fprintf(dbgf,"                          %s %2i [%2i,%2i][%2i,%2i] \n", FnList[Cd].m_Txt, iFnNo, RC.m_Row, RC.m_Col, RCEnd.m_Row, RCEnd.m_Col);
    switch (Cd)
      {
      case Fn_BST:
        {
        CCoOrd RCEndX;
        iFnNo=BuildFnMatrixBr(iFnNo, RC, RCEndX, Mtx);
        RCEnd=Max(RCEnd, RCEndX);
        }
        break;
      case Fn_BND:
      case Fn_NXB:
        LegDone=true;
        break;
      case Fn_EOR:
        LegDone=true;
      case Fn_SOR:
        BuildFnMatrixFn(Cd, iFnNo, RC, Mtx);
        RCEnd.m_Col=RC.m_Col++;
        iFnNo++;
        break;
      case Fn_LBL:
        m_pLadder->m_Lbl2Rung[((CFnSimpleN*)m_Fns[iFnNo])->m_No]=m_iRung;
      default:;
        BuildFnMatrixFn(Cd, iFnNo, RC, Mtx);
        RCEnd.m_Col=RC.m_Col++;
        iFnNo++;
      }
    }
  //if (dbgf) fprintf(dbgf,"    <   %3i %3i %3i\n", iFnNo, RC.m_Row, RC.m_Col);
  return iFnNo;
  };

//---------------------------------------------------------------------------

inline const char * FnText(CFnBase * E) { return E->m_Cd>0 ? FnList[E->m_Cd].m_Txt : "   "; };
inline const char * FnText(CMtxElement &E) { return E.m_Cd>0 ? FnList[E.m_Cd].m_Txt : "   "; };
inline const char * FnPict(CMtxElement &E)
  {
  const char * p;
  if (E.m_Cd>0)
    {
    p=FnList[E.m_Cd].m_Pict;
    if (*p=='#')
      p=FnList[E.m_Cd].m_Txt;

    }
  else
    p="   ";
  return p;
  };

void CLadder::Evaluate()
  {
//TRACE(">>> %3i %2i\n",m_iIndex, m_CallLevel);
  CheckThat(m_CallLevel==0, "JSR Recursion not allowed");
  m_CallLevel++;
  int i=0;
  while (i<m_Rungs.GetSize())
    {
    //if (i==113)
    //  {
    //  int xxx=0;
    //  }
//TRACE("         %3i %2i %5i\n",m_iIndex, m_CallLevel,i);
    m_Rungs[i]->Evaluate();
    if (m_NextRung>=0)
      {
      i=m_NextRung;
      m_NextRung=-1;
      }
    else
      i++;
    }

  m_CallLevel--;
//TRACE(" << %3i %2i\n",m_iIndex, m_CallLevel);
  };
//---------------------------------------------------------------------------

void CLadder::BuildFnMatrix()
  {
  m_MaxWidth=10;
  for (int r=0; r<m_Rungs.GetSize(); r++)
    {
    CRung &R=*m_Rungs[r];
    R.m_pLadder=this;
    R.m_iRung=r;
    if (0 && dbgf)
      {
      fprintf(dbgf,"---------- Ladder:%2i Rung:%i ----------\n", this->m_iIndex, r);
      for (int f=0; f<R.m_Fns.GetSize(); f++)
        fprintf(dbgf, " %s",FnText(R.m_Fns[f]));
      fprintf(dbgf, "\n");
      }
    CCoOrd RC(0,0,0,0);
    CCoOrd RCEnd(0,0,0,0);

    CTFnMatrix TMtx;
    R.BuildFnMatrix(0, RC, RCEnd, TMtx);
    R.m_Matrix=TMtx;
    R.m_Matrix.CalculateSpacing(m_MaxWidth);

    }
  if (m_Rungs.GetSize())
    if (0 && dbgf) fprintf(dbgf,"------------------- MaxWidth : %i\n", m_MaxWidth);

  for (r=0; r<m_Rungs.GetSize(); r++)
    {
    CRung &R=*m_Rungs[r];
    R.m_Matrix.ReCalculateSpacing(m_MaxWidth);

    if (0 && dbgf)
      {
      fprintf(dbgf,"\n");
      for (int r=0; r<R.m_Matrix.m_iHigh; r++)
        {
        for (int c=0; c<R.m_Matrix.m_iWide; c++)
          {
          fprintf(dbgf,"%s", FnPict(R.m_Matrix(r,c)));
          }
        fprintf(dbgf,"\n");
        }
      fprintf(dbgf," Yt  Yr  Ht\n");
      for (r=0; r<R.m_Matrix.m_iHigh; r++)
        {
        CMtxRow &Rw=R.m_Matrix.m_Rows[r];
        fprintf(dbgf,"%3i %3i %3i|", Rw.m_Yt, Rw.m_Yr, Rw.m_Ht);

        for (int c=0; c<R.m_Matrix.m_iWide; c++)
          {
          CMtxElement &E=R.m_Matrix.Mtx(r,c);
          //if (E.m_Cd>0)
          //  fprintf(dbgf,"<%2i,%3i>%s  ", E.m_Y, E.m_X, FnPict(E));
          //else
          //  fprintf(dbgf," %2s %3s %s  ", "", "", "   ");
          if (E.m_Cd>0)
            fprintf(dbgf,"<%3i,%3i>%s  ", E.m_X, E.m_AWd, FnPict(E));
          else if (E.m_LftRail || E.m_RgtRail)
            fprintf(dbgf,"<%3i,%3i>%3s  ", E.m_X, E.m_AWd, "!  ");
          else
            fprintf(dbgf,"<%3i,%3i>%s  ", E.m_X, E.m_AWd, "   ");
          }
        fprintf(dbgf,"\n");
        }
      fprintf(dbgf,"\n");
      }
    }

  };

long CLadder::FindRungWithAddress(LPCTSTR pAdd)
  {
  long l=0; 
  long r=Max(0L,m_FindRung);
  long f=m_FindFnct;
  long rstop=-1;
  long fstop=-1;
  for (;;)
    {
    l++;
    f++;
    if (f>=NFns(r))
      {
      r++;
      f=0;
      }
    if (r>=NRungs())
      {
      r=0;
      f=0;
      }
    if (r==rstop && f==fstop)
      break;
    
    CFnBase &Fn=Function(r,f);
    for (int i=0; i<Fn.NAddressFlds(); i++)
      {
      CAddress * pA=Fn.AddressFld(i);
      if (pA&& stricmp(pA->Text(), pAdd)==0)
        {
        m_FindRung=r;
        m_FindFnct=f;
        return l;
        }
      }
    
    if (rstop<0)
      {
      rstop=r;
      fstop=f;
      }
    }

  int xxx=0;
  return -1;
  };

//---------------------------------------------------------------------------

CFnMatrix & CFnMatrix::operator=(const CTFnMatrix & M)
  {
  m_iWide=M.m_iWide;
  m_iHigh=M.m_iHigh;
  m_Rows.SetSize(m_iHigh);
  for (int r=0; r<m_iHigh; r++)
    {
    m_Rows[r].m_Yt=M.m_Yt[r];
    m_Rows[r].m_Yr=M.m_Yr[r];
    m_Rows[r].m_Ht=M.m_Ht[r];
    m_Rows[r].m_Cols.SetSize(m_iWide);
    for (int c=0; c<m_iWide; c++)
      m_Rows[r].m_Cols[c]=M.m_Elements[r][c];
    };

  return *this;
  };

//===========================================================================
//
//
//
//===========================================================================

void CFnMatrix::CalculateSpacing(int &MaxWidth)
  {
  // first column always SOR
  int c,r;
  int Left=0;
  int Right=0;

  for (c=0; c<m_iWide; c++)
    for (r=0; r<m_iHigh; r++)
      Mtx(r, c).m_AWd=Mtx(r, c).m_Wd;

  for (c=1; c<m_iWide; c++)
    {
    for (r=0; r<m_iHigh; r++)
      Mtx(r, c).m_X=Mtx(r, c-1).m_X+Mtx(r, c-1).m_Wd;
    for (r=0; r<m_iHigh; r++)
      {
      CMtxElement & E=Mtx(r, c);
      CMtxElement & El=Mtx(r, c-1);
      //E.m_AWd=E.m_Wd;

      switch (E.m_Cd)
        {
        case Fn_BST:
          Left=E.m_X;
          El.m_AWd=E.m_X-El.m_X;
          break;
        case Fn_BND:
          {
          Right=E.m_X;
          for (int r1=r+1; r1<m_iHigh; r1++)
            {
            CMtxElement & Ex=Mtx(r1, c);
            if (Ex.m_Cd==Fn_UConn || Ex.m_Cd==Fn_EConn)
              {
              Right=Max(Right, Ex.m_X);
              }
            else
              break;
            }
          E.m_X=Right;
          El.m_AWd=E.m_X-El.m_X;
          }
          break;
        case Fn_SConn:
        case Fn_DConn:
          E.m_X=Left;
          El.m_AWd=E.m_X-El.m_X;
          break;
        case Fn_EConn:
        case Fn_UConn:
          E.m_X=Right;
          El.m_AWd=E.m_X-El.m_X;
          break;
        case Fn_NULL:
          if (r>0)
            {
            CMtxElement & Eu=Mtx(r-1, c);
            E.m_X=Eu.m_X;
            E.m_AWd=Eu.m_AWd;
            }
        default:
          El.m_AWd=E.m_X-El.m_X;
          break;
        }
      if (0 && E.m_Cd>0 && dbgf)
        {
        fprintf(dbgf,"%2i,%2i) <%3i> %2i >> <%3i,%2i> \n",r,c,El.m_X,El.m_Wd,E.m_X,E.m_AWd);
//        fprintf(dbgf,"%2i,%2i) <%2i,%3i> %2i >> <%2i,%3i> \n",r,c,El.m_Y,El.m_X,El.m_Wd,E.m_Y,E.m_X);
        }
      }
    }

  //MaxWide=0;
  c=m_iWide-1;
  for (r=0; r<m_iHigh; r++)
    {
    CMtxElement & E=Mtx(r, c);
    MaxWidth=Max(MaxWidth, E.m_X+E.m_AWd);
    }

  for (r=0; r<m_iHigh; r++)
    {
    int T=0;
    int B=0;
    for (c=0; c<m_iWide; c++)
      {
      CMtxElement & E=Mtx(r, c);
      T=Min(T, (int)-E.m_Of);
      B=Max(B, (int)E.m_Ht-E.m_Of);
      E.m_LftRail=0;
      E.m_RgtRail=0;
      E.m_LftPwr=0;
      E.m_RgtPwr=0;
      E.m_CdA=Fn_NULL;
      E.m_CdB=Fn_NULL;
      E.m_CdL=Fn_NULL;
      E.m_CdR=Fn_NULL;;
      }
    CMtxRow &R=m_Rows[r];
    if (r==0)
      {
      R.m_Yt=0;
      R.m_Yr=-T;
      }
    else
      {
      CMtxRow &R1=m_Rows[r-1];
      R.m_Yt=R1.m_Yt+R1.m_Ht;
      R.m_Yr=R.m_Yt-T;
      }
    R.m_Ht=B-T;
    }


  for (r=1; r<m_iHigh; r++)
    for (c=0; c<m_iWide; c++)
      Mtx(r, c).m_CdA=Mtx(r-1, c).m_Cd;

  for (r=0; r<m_iHigh-1; r++)
    for (c=0; c<m_iWide; c++)
      Mtx(r, c).m_CdB=Mtx(r+1, c).m_Cd;

  for (r=0; r<m_iHigh; r++)
    for (c=1; c<m_iWide; c++)
      Mtx(r, c).m_CdL=Mtx(r, c-1).m_Cd;

  for (r=0; r<m_iHigh; r++)
    {
    Mtx(r, 0).m_LftRail=1;
    Mtx(r, m_iWide-1).m_RgtRail=1;
    for (c=0; c<m_iWide-1; c++)
      Mtx(r, c).m_CdR=Mtx(r, c+1).m_Cd;
    }
  };

//---------------------------------------------------------------------------

void CFnMatrix::ReCalculateSpacing(int MaxWidth)
  {
  // first column always SOR
  int c,r;
  int ColOneRow=-1;
  int ColRight=-1;
  int Done=false;
  for (c=0; !Done && c<m_iWide; c++)
    {
    int CountT=0;
    int Count1=0;
    for (r=0; !Done && r<m_iHigh; r++)
      {
      byte Cd=Mtx(r, c).m_Cd;
      if (Cd!=Fn_NULL)
        Count1++;
      if (FnList[Cd].m_Type!=Typ_Other)//.m_iJust;Cd!=Fn_NULL)
        CountT++;
      byte J=FnList[Cd].m_iJust;
      if (J==J_Right)
        Done=true;
      }
    if (Count1==1)
      ColOneRow=c;
    }

  c=m_iWide-1;
  for (r=0; r<m_iHigh; r++)
    {
    int X=Mtx(r, c).m_X;
    int XE=X+Mtx(r, c).m_AWd-1;
    int Add=MaxWidth-XE;
    Mtx(r, c).m_AWd+=Add;
    }
  };

//---------------------------------------------------------------------------

void CFnMatrix::Evaluate()
  {
  // first column always SOR

  int c,r,r1;
  int ColOneRow=-1;
  int ColRight=-1;
  int Done=false;
  for (c=0; !Done && c<m_iWide; c++)
    {
    for (r=0; !Done && r<m_iHigh; r++)
      {
      CMtxElement & E=Mtx(r, c);
      switch (E.m_Cd)
        {
        case Fn_SOR:
          E.m_LftPwr=true;
          E.m_RgtPwr=true;
          break;
        case Fn_XIC:
        case Fn_XIO:
        case Fn_ONS:
          //E.m_RgtPwr=(E.Fn().Enabled()) ? E.m_LftPwr : false;
          //break;
        case Fn_OTE:
        case Fn_OTL:
        case Fn_OTU:
        case Fn_RES:
          E.m_RgtPwr=E.Fn().SetEnabled(E.m_LftPwr);
          break;
        case Fn_JMP:
          E.m_RgtPwr=E.Fn().SetEnabled(E.m_LftPwr);
          break;
        case Fn_BST:
        case Fn_HConn:
          E.m_RgtPwr=E.m_LftPwr;
          break;
        case Fn_SConn:
          E.m_LftPwr=Mtx(r-1,c).m_LftPwr;
          E.m_RgtPwr=E.m_LftPwr;
          break;
        case Fn_BND:
          {
          bool On=false;
          for (r1=r; r1<m_iHigh; r1++)
            {
            CMtxElement & Ex=Mtx(r1, c);
            if (r1==r || Ex.m_Cd==Fn_EConn)
              {
              if (Ex.m_LftPwr)
                {
                On=true;
                break;
                }
              }
            else
              break;
            }
          E.m_RgtPwr=On;
          }
          break;
        default:
          if (E.HasFn())
            E.m_RgtPwr=E.Fn().SetEnabled(E.m_LftPwr);
          break;
        }

      if (c<m_iWide-1)
        Mtx(r, c+1).m_LftPwr=E.m_RgtPwr;
      }
    }
  };

//===========================================================================
//
//
//
//===========================================================================

int CRung::BuildTxtImage(CRungImage & Img, CLadderDisplayList & Lst, int LnRungStart)
  {
  if (0 && dbgf) fprintf(dbgf,"  Img   >>>\n");
  int Len=1;

  for (int r=0; r<m_Matrix.m_iHigh; r++)
    {
    CMtxRow &Rw=m_Matrix.m_Rows[r];
    //fprintf(dbgf,"%3i %3i %3i|", Rw.m_Yt, Rw.m_Yr, Rw.m_Ht);

    Img.m_Colour[r]=0;
    for (int c=0; c<m_Matrix.m_iWide; c++)
      {
      CMtxElement &E=m_Matrix.Mtx(r,c);
      if (E.m_pFn && FnList[E.m_Cd].m_Type!=Typ_Other)
        E.m_pFn->VBuildTxtImage(Img, Rw, E);
      else
        CFnBase::SBuildTxtImage(Img, Rw, E);//, m_Matrix(r-1,c).m_Cd);
      int i=Lst.m_ActLen++;
      Lst[i].m_Pos.SetRect(E.m_X, LnRungStart, E.m_X+E.m_AWd, LnRungStart+E.m_Ht);
      Lst[i].m_pE=&m_Matrix.Mtx(r,c);

      }
    LnRungStart+=Rw.m_Ht;
    }
  return Len;
  };

//===========================================================================
//
//
//
//===========================================================================

//===========================================================================
//
//
//
//===========================================================================

BOOL CPLC5::LoadSymbols(LPCSTR FnSym, LPCSTR FnSymOut, LPCTSTR FnOptProfile)
  {
  BOOL OK=TRUE;
  FILE *f=fopen(FnSym, "rt");
  FILE *fo=FnSymOut && strlen(FnSymOut) ? fopen(FnSymOut, "wt") : NULL;

  int nSymbols=0;
  if (f)
    {
    char Buff[4096];

    m_SymbolMap.InitHashTable(2001);
    m_AddressMap.InitHashTable(2001);
    // Measure File Size
    long l=0;
    while (!feof(f))
      {
      fgets(Buff, sizeof(Buff), f);
      //ADDRESS................................ SYMFIL SYMBOL......... DES 1.......... DES 2.......... DES 3.......... DES 4.......... DES 5.......... DEV.. ABV...... BLW......
      //012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567
      //          1         2         3         4         5         6         7         8         9         0         1         2         3         4         5         6
      //           <39>
      for (int i=0; i<168; i++)
        if (Buff[i]<' ')
          Buff[i]=' ';

      if (l==0)
        {
        if (strncmp(Buff, "ADDRESS.", 8)!=0)
          {
          OK=false;
          break;
          }
        }
      else
        {
        CAddInfo *pInfo=new CAddInfo;
        pInfo->m_sAddress = StrToken(&Buff[0]);
        pInfo->m_lSymFile = IntToken(&Buff[40]);
        pInfo->m_sSymbol  = StrToken(&Buff[47]);
        pInfo->m_sDesc[0] = StrToken(&Buff[63],15,1);
        pInfo->m_sDesc[1] = StrToken(&Buff[79],15,1);
        pInfo->m_sDesc[2] = StrToken(&Buff[95],15,1);
        pInfo->m_sDesc[3] = StrToken(&Buff[111],15,1);
        pInfo->m_sDesc[4] = StrToken(&Buff[127],15,1);
        pInfo->m_lDev     = IntToken(&Buff[143]);
        pInfo->m_sAbv     = StrToken(&Buff[149],9);
        pInfo->m_sBlw     = StrToken(&Buff[159],9);
        pInfo->Prepare();
        
        m_SymbolMap.SetAt(pInfo->m_sSymbol, pInfo);
        CString S=pInfo->m_sAddress;
        StripAddressZeros(S);
        m_AddressMap.SetAt(S, pInfo);
        m_AddInfoList.AddTail(pInfo);

        if (0 && dbgf)
          {
          if (nSymbols==0)
            fprintf(dbgf,"-----------------------------------------------------------------------------\n");
          //fprintf(dbgf,"%-40s  %3i %-15s %-15s\n",pInfo->m_sAddress, pInfo->m_lSymFile, pInfo->m_sSymbol, pInfo->m_sDesc[0]);
          //fprintf(dbgf,"%-40s  %3s %-15s %-15s\n","","","", pInfo->m_sDesc[1]);
          //fprintf(dbgf,"%-40s  %3s %-15s %-15s\n","","","", pInfo->m_sDesc[2]);
          //fprintf(dbgf,"%-40s  %3s %-15s %-15s\n","","","", pInfo->m_sDesc[3]);
          //fprintf(dbgf,"%-40s  %3s %-15s %-15s\n","","","", pInfo->m_sDesc[4]);
          //fprintf(dbgf,"-----------------------------------------------------------------------------\n");
          
          fprintf(dbgf,"%-40s  %3i %-15s %-15s %-15s %-15s %-15s %-15s\n",
                       pInfo->m_sAddress, pInfo->m_lSymFile, pInfo->m_sSymbol, 
                       pInfo->m_sDesc[0], pInfo->m_sDesc[1], pInfo->m_sDesc[2], pInfo->m_sDesc[3], pInfo->m_sDesc[4]);
          }

        if (fo)
          fprintf(fo, "%s\t%s\t%s\n", pInfo->m_sAddress, pInfo->m_sSymbol, pInfo->m_sFullDesc);
        
        nSymbols++;
        //Symbols.Add();
        }
      l++;
      }

    m_AddInfo.SetSize(m_AddInfoList.GetCount());
    POSITION Pos=m_AddInfoList.GetHeadPosition();
    for (l=0; Pos; l++)
      {
      CAddInfo * AI=m_AddInfoList.GetNext(Pos);
      m_AddInfo[l]=AI;
      AI->m_AddInfoInx=l;
      AI->m_pAddress=new CAddress(this);
      AI->m_pAddress->Parse((char*)(LPCTSTR)AI->m_sAddress, false);
      }

    fclose(f);
    if (fo)
      fclose(fo);
    }
  return OK;
  };

//===========================================================================
//
//
//
//===========================================================================

void CPLC5::LoadATable(CTknFile & Tkns, FILE *f, LPCTSTR FnOptProfile)
  {

  CString T=StrToken(Tkns(5));
  int NmLen=T.Find(':');
  CString Nm=T.Left(NmLen);
  CString Last=T.Right(T.GetLength()-NmLen-1);

  CString NmTxt=Nm.SpanExcluding("0123456789");
  CString NmNo=Nm.Right(Nm.GetLength()-NmTxt.GetLength());

  CTableRoot *pTbRt=FindTableRoot(NmTxt);
  long iTabNum=-1;
  if (NmNo.GetLength()>0)
    iTabNum=Range(pTbRt->m_TbNumLo,(short)atol(NmNo),pTbRt->m_TbNumHi);
  else
    iTabNum=pTbRt->m_TbNumDef;

  m_nTables=Max(m_nTables, iTabNum+1L);
  m_Table[iTabNum]=new CTable;
  CTable &Tab=*m_Table[iTabNum];

  int iLast;
  int LoadData=(m_LoadWhat==Load_All);
  if (Nm=="O" || Nm=="I")
    {
    sscanf(Last, "%o", &iLast);
    LoadData=(m_LoadWhat>= (Nm=="I" ? Load_Input : Load_All));
    }
  else
    sscanf(Last, "%i", &iLast);


  UINT iSize=Max((UINT)iLast+1, GetPrivateProfileInt(m_sPlcName, Nm, 1, FnOptProfile));

  Tab.Initialise(pTbRt, iTabNum, Nm, NmTxt, NmNo, iSize);

  int BuffLen=0;
  for (int i=0; i<=iLast; )
    {
    BuffLen=Tkns.ReadALine(f);
    if (Tab.IsFlt())
      {
      float Val[10];
      int nVals=sscanf(Tkns(0), Tab.LoadFmt(), &Val[0], &Val[1], &Val[2], &Val[3], &Val[4], &Val[5], &Val[6], &Val[7], &Val[8], &Val[9]);
      int NewVals=nVals/ Tab.NStructVals();
      int iVal=0;
      for (int j=0; j<NewVals; j++, i++)
        for (int k=0; k<Tab.NStructVals(); k++)
          {
          Tab.SetFValue(i, k, LoadData ? Val[iVal] : 0);
          iVal++;
          }
      }
    else
      {
      int Val[10];
      int nVals=sscanf(Tkns(0),  Tab.LoadFmt(), &Val[0], &Val[1], &Val[2], &Val[3], &Val[4], &Val[5], &Val[6], &Val[7], &Val[8], &Val[9]);
      int NewVals=nVals/Tab.NStructVals();
      int iVal=0;
      for (int j=0; j<NewVals; j++, i++)
        for (int k=0; k<Tab.NStructVals(); k++)
          {
          Tab.SetIValue(i, k, LoadData ? Val[iVal] : 0);
          iVal++;
          }
      }
    }
  };

//===========================================================================

void CPLC5::LoadARung(CTknFile & Tkns, FILE *f)
  {
  int LadNum=m_pWkLadder->m_iIndex;
  int RungNo=m_pWkLadder->m_Rungs.GetSize();
  m_pWkLadder->m_Rungs.SetSize(RungNo+1);
#if DotNetAfxTempl
  m_pWkRung=m_pWkLadder->m_Rungs[RungNo].Ptr();
#else
  m_pWkRung=m_pWkLadder->m_Rungs[RungNo];
#endif

  if (0 && dbgf) fprintf(dbgf,"Rung [%i:%i] -------- %s\n",LadNum, RungNo, Tkns());
  //int Pos=0;
  char *pTkn=Tkns.StrToken();
  if (strcmp(pTkn, "SOR")==0)
    {
    byte RungDone=false;
    //byte GotTkn=true;
    Tkns.PushToken();
    while (!RungDone)
      {
      //if (!GotTkn)
      pTkn=Tkns.StrToken();
      while (!pTkn)
        {
        Tkns.ReadALine(f);
        pTkn=Tkns.StrToken();
        }

      //GotTkn=false;
      int n=sizeof(FnList)/sizeof(FnList[0]);
      int Cd=-1;
      //CParmArray PA;
      CFnInfoItem *pIT=NULL;
      int nParms=0;
      for (int i=0; i<n; i++)
        {
        if (strcmp(pTkn, FnList[i].m_Txt)==0)
          {
          pIT=&FnList[i];
          Cd=pIT->m_Fn;
          nParms=0;
          CheckThat(Cd==i, "Cd must match index");
          break;
          }
        }
      if (Cd>0)
        {
        CFnBase *pFn=NULL;
        //PA[nParms]=NULL;
        switch (FnList[Cd].m_Type)
          {
          case Typ_SimpleA: pFn=new CFnSimpleA(this, Cd, Tkns); break;
          case Typ_SimpleN: pFn=new CFnSimpleN(this, Cd, Tkns); break;
          case Typ_Block:
            {
            switch (Cd)
              {
              case Fn_TON:
              case Fn_TOF: pFn=new CFnTimer(this, Cd, Tkns);  break;
              case Fn_CTU:
              case Fn_CTD: pFn=new CFnCounter(this, Cd, Tkns);  break;
              case Fn_JSR: pFn=new CFnJSR(this, Cd, Tkns);    break;
              case Fn_SBR: pFn=new CFnSBR(this, Cd, Tkns);    break;
              case Fn_RET: pFn=new CFnRET(this, Cd, Tkns);    break;
              case Fn_EQU:
              case Fn_GEQ:
              case Fn_GRT:
              case Fn_LEQ:
              case Fn_LES: pFn=new CFnTest(this, Cd, Tkns);   break;
              case Fn_LIM: pFn=new CFnLIM(this, Cd, Tkns);    break;
              case Fn_CMP: pFn=new CFnCMP(this, Cd, Tkns);    break;
              case Fn_NEG: pFn=new CFnCpt2(this, Cd, Tkns);    break;
              case Fn_ADD:
              case Fn_SUB:
              case Fn_MUL:
              case Fn_DIV: pFn=new CFnCpt3(this, Cd, Tkns);    break;
              case Fn_CPT: pFn=new CFnCPT(this, Cd, Tkns);    break;
              case Fn_COP: pFn=new CFnCOP(this, Cd, Tkns);    break;
              case Fn_MOV: pFn=new CFnMOV(this, Cd, Tkns);    break;
              case Fn_FBC: pFn=new CFnFBC(this, Cd, Tkns);    break;
              case Fn_BTW: pFn=new CFnBXfr(this, Cd, Tkns);    break;
              case Fn_BTR: pFn=new CFnBXfr(this, Cd, Tkns);    break;
              case Fn_FAL: pFn=new CFnFAL(this, Cd, Tkns);    break;
              case Fn_FFL: pFn=new CFnFFL(this, Cd, Tkns);    break;
              case Fn_FFU: pFn=new CFnFFU(this, Cd, Tkns);    break;
              default:
                if (!FnList[Cd].m_InComplete)
                  {
                  TRACE("Blk Function Incomplete %s\n", pTkn);
                  FnList[Cd].m_InComplete=1;
                  m_nInCompOpCd++;
                  }
                pFn=new CFnBlk(this, Cd, Tkns);
                for (int j=0; j<FnList[Cd].m_nParms; j++)
                  Tkns.StrToken();
                break;
              }
            }
            break;
          case Typ_Other:
          default:         pFn=new CFnBase(this, Cd, Tkns);   break;
          }

        m_pWkRung->m_Fns.Add(pFn);

        if (Cd==Fn_EOR)
          RungDone=true;

        }
      else
        {
        m_nUnImpOpCd++;
        TRACE("[%i] UNIMPLEMENTED OPCODE %s\n", m_nUnImpOpCd,pTkn);

        if (dbgf)
          fprintf(dbgf,"[%i] UNIMPLEMENTED OPCODE %s",m_nUnImpOpCd,pTkn);

        char * pTkn;
        for (int i=0; (pTkn=Tkns.StrToken()); i++)
          {
          if (IsInsMne(pTkn))
            {
            Tkns.PushToken();
            break;
            }
          else if (dbgf)
            fprintf(dbgf," %s",pTkn);
          }
        //CParmArray PA;
        //int n=ScanParms(PA, Tkns, pTkn);

        if (dbgf)
          fprintf(dbgf,"\n");
        }
      }
    }
  else
    {
    CheckThat(FALSE, "Bad Start of Rung");
    }
  }

//===========================================================================

void CPLC5::LoadALadder(CTknFile & Tkns, FILE *f, LPCTSTR FnOptProfile)
  {
  m_pWkLadder=NULL;
  m_pWkRung=NULL;
  m_MinTimerPre=0x7fffffff; // 10 secs Real Long

  long iLadNum=-1;
  byte inRungTitle=0;
  for ( ; ; )
    {
    if (inRungTitle)
      {
      if (strncmp(Tkns(), "%", 1)==0)
        inRungTitle=false;
      }
    else if (strncmp(Tkns(), "% TITLE/RUNG", 12)==0)
      inRungTitle=true;
    else if (strncmp(Tkns(), "LADDER", 6)==0)
      {
      iLadNum=IntToken(Tkns(6));
      m_nLadders=Max(m_nLadders, iLadNum+1L);
      m_pWkLadder=&m_Ladder[iLadNum];
      m_pWkRung=NULL;

      m_pWkLadder->m_iIndex=iLadNum;
      }
    else if (strncmp(Tkns(), "SOR", 3)==0)
      {
      LoadARung(Tkns, f);
      }
    else if (Tkns.Length()==0)
      {
      // continue a blank line
      }
    else
      break;
    Tkns.ReadALine(f);
    }
  };

//---------------------------------------------------------------------------

void CPLC5::GetProjectInfo(CTknFile & Tkns, FILE *f)
  {
  int Pos=7;
  m_sPrjName=Tkns.StrQToken();
  m_sPrjName.TrimLeft();
  m_sPrjName.TrimRight();
  //int NmLen=T.Find(':');
  //CString Nm=T.Left(NmLen);
  //CString Last=T.Right(T.GetLength()-NmLen-1);
  //const char *pFmt;
  int Len;
  while ((Len=Tkns.ReadALine(f))>0)
    {
    int Pos=0;
    int n=Tkns.IntToken();
    if (n>=0 && n<MaxLadders)
      m_Ladder[n].m_sName=Tkns.StrQToken();
    }
  };

//---------------------------------------------------------------------------
#if WithTestXCode
void CPLC5::LoadTextXs(CTknFile & Tkns, FILE *f)
  {
  int iX=0;
  int Len;
  while ((Len=Tkns.ReadALine(f))>0)
    {
    if (iX<10)
      m_TstXs[iX++].Parse(this, Tkns, XM_CPT);
    //int Pos=0;
    //int n=Tkns.IntToken();
    //if (n>=0 && n<MaxLadders)
    //  m_Ladder[n].m_sName=Tkns.StrQToken();

    }
  }
#endif
//---------------------------------------------------------------------------

BOOL CPLC5::LoadLadder(LPCSTR Fn, LPCTSTR FnOptProfile)
  {
  BOOL OK=FALSE;
  FILE *f=fopen(Fn, "rt");

  m_nUnImpOpCd=0;
  m_nInCompOpCd=0;

  if (f)
    {
    CTknFile Tkns;

    // Load File
    int l=0;
    while (!feof(f))
      {
      long Len=Tkns.ReadALine(f);

      if (strncmp(Tkns(), "DATA", 4)==0)
        {
        if (dbgf) fprintf(dbgf,"Table -------- %s\n",Tkns());
        LoadATable(Tkns, f, FnOptProfile);
        }
      else if (strncmp(Tkns(), "PROJECT", 7)==0)
        {
        if (dbgf) fprintf(dbgf,"Project -------- %s\n",Tkns());
        GetProjectInfo(Tkns, f);
        }
      else if (strncmp(Tkns(), "LADDER", 6)==0)
        {
        if (dbgf) fprintf(dbgf,"Ladder -------- %s\n",Tkns());
        LoadALadder(Tkns, f, FnOptProfile);
        OK=true;
        }
#if WithTestXCode
      else if (strncmp(Tkns(), "TESTX", 6)==0)
        {
        if (dbgf) fprintf(dbgf,"TestXpression -------- %s\n",Tkns());
        LoadTextXs(Tkns, f);
        }
#endif
      }
    fclose(f);
    }

  return OK;
  };

BOOL CPLC5::Load(LPCTSTR PlcName, LPCSTR FnLad, LPCTSTR FnSym, LPCTSTR FnSymOut, LPCTSTR FnOptProfile)
  {
  BOOL OK=TRUE;

  m_sPlcName=PlcName;
  m_LoadWhat=Load_All;

  m_nLadders=0;
  for (int l=0; l<MaxLadders; l++)
    m_Ladder[l].m_sName.Format("#%i",l);

  m_LadLoaded=LoadLadder(FnLad, FnOptProfile);
  if (!m_LadLoaded)
    {
    AfxMessageBox("Ladder not Parsed");
    return FALSE;
    }

  m_SymLoaded=LoadSymbols(FnSym, FnSymOut, FnOptProfile);

  if (!m_SymLoaded)
    {
    AfxMessageBox("Symbols Not Loaded");
    return FALSE;
    }

  for (l=0; l<m_nLadders; l++)
    {
    m_Ladder[l].m_pPLC=this;
    m_Ladder[l].BuildFnMatrix();
    }

  if (dbgf) fflush(dbgf);
  //fclose(dbgf);
  //dbgf=NULL;

  m_Status_ForcesPresent.Parse("S:1/9");
  m_Status_Remote.Parse("S:1/7");
  m_Status_ScanOne.Parse("S:1/15");
  m_Status_MinorFault.Parse("S:10");
  m_Status_MajorFault.Parse("S:11");

  if (m_Table[2]==NULL)
    return FALSE;

  m_pS24=&(m_Table[2]->m_IVals[24]);

  return OK;
  };

//===========================================================================

void CPLC5::SaveData()
  {
  for (int i=0; i<m_nTables; i++)
    if (m_Table[i])
      {
      CTable &T=*m_Table[i];

      for (int j=0; j<T.m_IVals.GetSize(); j++)
        T.m_IValsPrev[j]=T.m_IVals[j];

      for (j=0; j<T.m_FVals.GetSize(); j++)
        T.m_FValsPrev[j]=T.m_FVals[j];
      }
  }

//---------------------------------------------------------------------------

void CPLC5::Reset()
  {
  m_IterNo=0;
  // Clear Memory
  for (int t=0; t<m_nTables; t++)
    if (m_Table[t])
      m_Table[t]->Reset();
  }

//---------------------------------------------------------------------------

void CPLC5::Evaluate(int StepMS, bool DoReset, bool InRemote)
  {
  m_StepMS=StepMS;
  m_bReset=DoReset;
  if (DoReset)
    Reset();
  //else
  //  {

  // Must Set ForcePresent ????
  //m_Status_ForcesPresent.SetBValue(0);
  m_Status_Remote.SetBValue(InRemote);
  //m_Status_ScanOne.SetBValue(m_IterNo==0);
  //m_Status_MinorFault.SetIValue(0);
  //m_Status_MajorFault.SetIValue(0);

#if WithTestXCode
  for (int iX=0; iX<sizeof(m_TstXs)/sizeof(m_TstXs[0]); iX++)
    m_TstXs[iX].Evaluate();
#endif

  int m_MainLd=2;
  CLadder &L=m_Ladder[m_MainLd];
  L.m_nXferParms=0;
  L.Evaluate();

  m_IterNo++;

  //  }
  };

//===========================================================================

int CPLC5::SaveSnapshot(LPCTSTR Fn)
  {
  char cDrv[_MAX_DRIVE];
  char cPath[_MAX_PATH];
  char cFile[_MAX_FNAME];
  char cExt[_MAX_EXT];
  _splitpath(Fn, cDrv, cPath, cFile, cExt);
  
  CString FnX(Fn);
  if (strlen(cExt)==0)
    FnX+=".txt";
      
  FILE *h=fopen(FnX, "wt");
  
  if (h)
    {
    for (int t=0; t<m_nTables; t++)
      {
      if (m_Table[t])
        {
        CTable &T=*m_Table[t];

        fprintf(h, "T %2s %4i %4i %4i %2i : ", 
          (LPCTSTR)T.m_sTxt, t, 
          T.m_IVals.GetSize(), T.m_FVals.GetSize(), T.NStructVals());
        for (int i=0; i<T.m_IVals.GetSize(); i++)
          fprintf(h, "%i ", T.m_IVals[i]);
        for (int f=0; f<T.m_FVals.GetSize(); f++)
          fprintf(h, "%.12g ", T.m_FVals[f]);
        fprintf(h, ";\n");
        }
      }
    fclose(h);
    return 0;
    }
  return 2;
  };

//---------------------------------------------------------------------------

int CPLC5::LoadSnapshot(LPCTSTR Fn)
  {
  char cDrv[_MAX_DRIVE];
  char cPath[_MAX_PATH];
  char cFile[_MAX_FNAME];
  char cExt[_MAX_EXT];
  _splitpath(Fn, cDrv, cPath, cFile, cExt);
  
  CString FnX(Fn);
  if (strlen(cExt)==0)
    FnX+=".txt";
      
  FILE *h=fopen(FnX, "rt");
  
  if (h)
    {
    while (!feof(h))
      {
      char Flag;
      char ID[32];
      int TNo;
      int NIVal;
      int NFVal;
      int NStruct;
      char Separ;
      fscanf(h, "%c %s %i %i %i %i %c ", &Flag, ID, &TNo, &NIVal, &NFVal, &NStruct, &Separ);

      int ReAlign=false;
      if (Flag=='T' && Separ==':' && TNo>=0 && TNo<MaxTables && m_Table[TNo]!=NULL)
        {
        // got a table
        CTable &T=*m_Table[TNo];

        int ii=0;
        for (int i=0; i<NIVal; i++)
          {
          int iTemp;
          fscanf(h, "%i ", &iTemp);
          if (ii<T.m_IVals.GetSize())
            T.m_IVals[ii++]=iTemp;
          }
        while (ii<T.m_IVals.GetSize())
          T.m_IVals[ii++]=0;

        int ff=0;
        for (int f=0; f<NFVal; f++)
          {
          float fTemp;
          fscanf(h, "%g ", &fTemp);
          if (ff<T.m_FVals.GetSize())
            T.m_FVals[ff++]=fTemp;
          }
        while (ff<T.m_FVals.GetSize())
          T.m_FVals[ff++]=0;

        char Term;
        fscanf(h, "%c ", &Term);
        if (Term!=';')
          ReAlign=true;
        }
      else
        ReAlign=true;

      if (ReAlign)
        {
        char BigBuff[1024];
        //get next line
        while (!feof(h) && fgets(BigBuff, sizeof(BigBuff), h))
          {
          int n=strlen(BigBuff);
          if (n>0 && BigBuff[n-1]=='\n')
            break;
          }
        }
      }
    fclose(h);
    return 0;
    }
  return 0;
  };

//---------------------------------------------------------------------------

int CPLC5::SaveBacktrack(LPCTSTR Fn)
  {
  // Temporaray ??? may have to improve
  return SaveSnapshot(Fn);
  };

//---------------------------------------------------------------------------

int CPLC5::LoadBacktrack(LPCTSTR Fn)
  {
  // Temporaray ??? may have to improve
  return LoadSnapshot(Fn);
  };

//===========================================================================
//
//
//
//===========================================================================

const long MaxChgHistory=1000;

CChgHistory::CChgHistory()
  {
  m_Length=0;
  m_Position=-1;
  m_Strings.SetSize(MaxChgHistory);
  m_nIterNo=-1;
  m_nIterNoOut=-2;
  }

long CChgHistory::Length()
  {
  return m_Length;
  }
void CChgHistory::AddIt(LPCTSTR Str)
  {
  m_Position++;
  if (m_Position>=MaxChgHistory)
    m_Position=0;
  if (m_Length<MaxChgHistory)
    m_Length++;
  m_Strings[m_Position]=Str;
  if (dbgf)
    fprintf(dbgf, "%s\n",Str);
  };
void CChgHistory::Add(LPCTSTR Str)
  {
  if (m_nIterNo!=m_nIterNoOut)
    {
    m_nIterNoOut=m_nIterNo;
    char Buff[128];
    sprintf(Buff, "----------- %i",m_nIterNo);
    AddIt(Buff);
    }
  AddIt(Str);
  };
LPCTSTR CChgHistory::Value(long Pos)
  {
  if (Pos>=m_Length)
    return NULL;
  Pos = Pos + m_Position + 1;
  if (Pos>=m_Length)
    Pos-=m_Length;
  return m_Strings[Pos];
  };

void CChgHistory::DumpChange(bool Val, CAddress & Addr, CRung * pRung, LPCTSTR pText)
  {
  CAddInfo * p=Addr.m_Plc->FindAddress(Addr.m_Txt);
  if (p)
  {
  if (p->m_bDumpChanges)
    {
    char buff1[1000];
    char buff2[1000];
    if (pRung)
      sprintf(buff1,"%-6s:%2i:%4i",
        (LPCTSTR)pRung->m_pLadder->m_pPLC->m_sPlcName,
          pRung->m_pLadder->m_iIndex, 
          pRung->m_iRung);
  
    sprintf(buff2,"[%-14.14s %-10.10s] %-16s = %s : %-15s %s",
      pRung ? buff1 : pText, pRung==NULL ? pText : "",
      (LPCTSTR)Addr.m_Txt, Val?"1":".",
      p?(LPCTSTR)p->m_sSymbol:"",
      p?(LPCTSTR)p->FullDescText():"");

    Add(buff2);
    }
  }
  }

void CChgHistory::DumpChange(bool Val, CExpression & Exp, CRung * pRung, LPCTSTR pText)
  {
  if (true)
    {
    char buff1[1000];
    char buff2[1000];
    if (pRung)
      sprintf(buff1,"%-6s:%2i:%4i",
        (LPCTSTR)pRung->m_pLadder->m_pPLC->m_sPlcName,
          pRung->m_pLadder->m_iIndex, 
          pRung->m_iRung);
  
    sprintf(buff2,"[%-14.14s %-10.10s] %-16s = %s",
      pRung ? buff1 : pText, pRung==NULL ? pText : "",
      (LPCTSTR)Exp.m_sTxt, Val?"1":".");

    Add(buff2);
    }
  }

CChgHistory m_History;

//===========================================================================
//
//
//
//===========================================================================
