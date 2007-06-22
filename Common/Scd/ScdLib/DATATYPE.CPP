//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DATATYPE_H
#define  __DATATYPE_H

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "sc_defs.h"
#define  __DATATYPE_CPP
#include "datacnvs.h"
#include "datatype.h"
//#include "errorlog.h"
#include "dbgmngr.h"
//#include "optoff.h"

//========================================================================
//
//
//
//========================================================================
 
#define DoFlags   01 
#define DoStrings 01 
#define DoCharArrays 01

//========================================================================
//
//
//
//========================================================================

const int ExpOff=6;
const int ExpMax=15;
const int ExpMin=0;

flt16 flt16::operator=(double v) 
  {
  if (_isnan(v))
    v = 0.0;
  Sgn=(v<0.0);
  v=fabs(v);
  int e;
  double m = frexp(v, &e);
  if (e%2!=0)
    {
    m/=2.0;
    e++;
    }
  e=e/2+ExpOff;
  if (e > 15)
    {
    e=15;
    m=1.0;
    //iStatus=ds_OverFlow;
    }
  else if (e < 0)
    {
    e=0;
    m=0.25;
    //iStatus=ds_UnderFlow;
    }
  //else
    //iStatus=ds_OK;
  Exp=e;
  Mant=((word)floor(m*2048.0));
  return *this;
  };

//------------------------------------------------------------------------

flt16::operator double() const
  { 
  int e=Exp-ExpOff; 
  double v=(Mant+0.5)/2048.0; 
  if (e>0) 
    v*=(1 << 2*e); 
  else if (e<0) 
    v/=(1 << 2*abs(e)); 
  if (Sgn) 
    return -v;
  return v;
  };

//========================================================================

void flt16::Test() 
  {
  flt16 f;//DataUnionCom DU;
  int kk;
  /*
  double x[]= {1.0, 1.1111111, 2.0, 2.222222, 3.0, 3.3333333, 4, 4.44444, 5, 5.555555, 6, 6.6666666, 7, 7.777777, 8, 8.88888888, 9, 9.9999999};

  for (double ii=-7; ii<6; ii++)
    for (int jj=0; jj<sizeof(x)/sizeof(x[0]); jj++)
      for (int kk=0; kk<2; kk++)
        {
        double X=x[jj]* (ii==-7 ? 0.0 : pow(10.0, ii)) * (kk==0 ? 1.0 : -1.0); 
        DU.SetFlt16(X); 
        dbgp("%14g > %14g  ", X, DU.GetFlt16());
        switch (DU.Status())
          {
          case ds_OK       : dbgp(" %8.4f", 100.0*(DU.GetFlt16()-X)/NZ(X)); break;
          case ds_OverFlow : dbgp("                 Hi"); break;
          case ds_UnderFlow: dbgp("              Lo   "); break;
          case ds_NAN      : dbgp("               NAN "); break;
          }
        dbgpln("");
        }

  for (int kk=0; kk<2000; kk++)
    {
    double X=0.9+(0.1*kk/1000); 
    DU.SetFlt16(X); 
    dbgp("%14g > %14g  ", X, DU.GetFlt16());
    switch (DU.Status())
      {
      case ds_OK        : dbgp(" %8.4f", 100.0*(DU.GetFlt16()-X)/NZ(X)); break;
      case ds_OverFlow  : dbgp("                 Hi"); break;
      case ds_UnderFlow : dbgp("              Lo   "); break;
      case ds_NAN       : dbgp("               NAN "); break;
      }
    dbgpln("");
    }
  */
  double MeanError=0.0;
  double MeanAbsError=0.0;
  double MaxError=0.0, MaxX=1.0;
  const int nn=100000;
  for (kk=0; kk<nn; kk++)
    {
    double X=1.0+(0.1*kk/1000); 
//    flt16 f; 
    f=X;
    //DU.Set(f); 
    double e=(double(f)-X)/X;
    MeanError+=e;
    MeanAbsError+=fabs(e);
    if (e>MaxError)
      {
      MaxError=e;
      MaxX=X;
      }
    if (fabs(100.0*(double(f)-X)/NZ(X)) > 0.1)
      {
      dbgp("%14g > %14g  ", X, f);
      dbgp(" %8.4f", 100.0*(f-X)/NZ(X));
      dbgpln(" <<<< ");
      }
  
    /*
    dbgp("%14g > %14g  ", X, DU.GetFlt16());
    switch (DU.Status())
      {
      case ds_OK        : dbgp(" %8.4f", 100.0*(DU.GetFlt16()-X)/NZ(X)); break;
      case ds_OverFlow  : dbgp("                 Hi"); break;
      case ds_UnderFlow : dbgp("              Lo   "); break;
      case ds_NAN       : dbgp("               NAN "); break;
      }
    dbgpln("");
    */
    }
  dbgpln("Mean     Error over %i values = %g%%", nn, 100.0*MeanError/nn);
  dbgpln("Mean Abs Error over %i values = %g%%", nn, 100.0*MeanAbsError/nn);
  f=MaxX; 
  dbgpln("Max      Error @ %14g > %14g  %14g%%", MaxX, f, 100.0*(f-MaxX)/NZ(MaxX));
  }

//========================================================================
//
//
//
//========================================================================

//===========================================================================
//
//
//
//===========================================================================

DDBValueLst DDBOnOff[]        = {{False, "Off", MDD_Default}, {True, "On"},  {0}};
DDBValueLst DDBOnBlank[]      = {{False, " "               }, {True, "On"},  {0}};
DDBValueLst DDBYesNo[]        = {{False, "No" , MDD_Default}, {True, "Yes"}, {0}};
DDBValueLst DDBYesBlank[]     = {{False, " "               }, {True, "Yes"}, {0}};

DDBValueLst DDBNAN_NotSpecd[] = {{0, "Undefined"},    {1, "0.0 : A Value"},   {0}};
DDBValueLst DDBNAN_Floating[] = {{0, "Floating"},     {1, "0.0 : A Value"},   {0}};
DDBValueLst DDBNAN_Ignore[]   = {{0, "Ignore"},       {1, "0.0 : A Value"},   {0}};
DDBValueLst DDBNAN_Off[]      = {{0, "Off"},          {1, "0.0 : A Value"},   {0}};
DDBValueLst DDBNAN_AtmosP[]   = {{0, "Atmospheric"},  {1, "101.35 : A Value"},{0}};
DDBValueLst DDBNAN_BlkPass[]  = {{0, "Block"},        {1, "Pass"            },{0}};

int DDBAltArrayAddress=0;

//===========================================================================

DDBValueLstMem & DDBValueLstMem::operator=(const DDBValueLstMem & VL)
  {
  Str.Clear();
  Lst.SetSize(VL.Lst.GetSize());
  for (int i=0; i<VL.Lst.GetSize(); i++)
    {
    Lst[i]=VL.Lst[i];
    if (VL.Lst[i].m_pStr)
      {
      Str.Append(VL.Lst[i].m_pStr);
      Lst[i].m_pStr=Str.Last()->Str();
      }
    }
  Len=VL.Len;
  return *this;
  };

DDBValueLstMem & DDBValueLstMem::operator=(const DDBValueLst & VL)
  {
  const DDBValueLst *pVL=&VL;
  int Len;
  for (Len=0; pVL[Len].m_pStr; Len++)
    { };
//  Len--;

  Str.Clear();
  Lst.SetSize(Len+1);
  for (int i=0; i<Len; i++)
    {
    Lst[i]=pVL[i];
    if (pVL[i].m_pStr)
      {
      Str.Append(pVL[i].m_pStr);
      Lst[i].m_pStr=Str.Last()->Str();
      }
    }

  return *this;
  };

void DDBValueLstMem::LoadFromCSV(Strng &S)
  {
  Strng Buff(S);
  CSVColArray f;
  int Quote,nParms;
  nParms = ParseCSVTokens(Buff(), f, Quote);
  int Index=0;
  for (int i=0; i<nParms; i++)
    {
    if (f[i] && strlen(f[i])>0)
      {
      char * p=strchr(f[i], '=');
      if (p)
        {
        *p=0;
        Index=SafeAtoL(p+1, Index);
        Add(Index, f[i]);
        }
      else
        {
        Add(Index, f[i]);
        }
      }
    else
      Add(Index, "");

    Index++;
    }
  }; 
void DDBValueLstMem::SaveToCSV(Strng &S, bool QuoteBlanks)
  {; 
  char Buff[2048];
  for (int v=0; v<Length(); v++)
    {
    char * pS=Item(v)->m_pStr;
    if (pS==NULL || strlen(pS)<1)
      pS="''";
    sprintf(Buff, v>0?", %s=%i":"%s=%i", pS, Item(v)->m_lVal);
    S+= Buff;
    }
  
  };
//===========================================================================
//
//
//
//===========================================================================

word DataUnionCom::Size() const
  {
  switch (iType)
    {
    case tt_NULL     : return            0+sizeof(byte);
    case tt_Char     : return sizeof(char)+sizeof(byte);
    case tt_Bool     :     
    case tt_Bit      : 
    case tt_Byte     : return sizeof(byte)+sizeof(byte);
    case tt_Word     : return sizeof(word)+sizeof(byte);
    case tt_DWord    : return sizeof(dword)+sizeof(byte);
    case tt_Int      : return sizeof(int)+sizeof(byte);
    case tt_Short    : return sizeof(short)+sizeof(byte);
    case tt_Long     : return sizeof(long)+sizeof(byte);
    case tt_Flt16    : return sizeof(flt16)+sizeof(byte);
    case tt_Float    : return sizeof(float)+sizeof(byte);
    case tt_Double   : return sizeof(double)+sizeof(byte);
    default          : break;
    };
  VERIFY(0);
  return 0;
  };

//------------------------------------------------------------------------

word DataUnionCom::TypeSize() const
  {
  switch (iType)
    {
    case tt_NULL     : return 0;
    case tt_Bool     :     
    case tt_Bit      : 
    case tt_Byte     : return sizeof(byte);
    case tt_Word     : return sizeof(word);
    case tt_DWord    : return sizeof(dword);
    case tt_Int      : return sizeof(int);
    case tt_Char     : return sizeof(char);
    case tt_Short    : return sizeof(short);
    case tt_Long     : return sizeof(long);
    case tt_Flt16    : return sizeof(flt16);
    case tt_Float    : return sizeof(float);
    case tt_Double   : return sizeof(double);
    };
  //VERIFY(0);
  return 0;
  };

//------------------------------------------------------------------------

flag DataUnionCom::IsNAN() const
  {
  switch (iType)
    {
    case tt_Flt16    : return (Status()==ds_NAN);
    case tt_Float    : return !Valid(Float);
    case tt_Double   : return !Valid(Double);
    default          : return 0;
    }
  return 0;
  }

//------------------------------------------------------------------------

void DataUnionCom::Set(DataUnionCom &V)
  {
  switch (V.iType)
    {
    case tt_NULL:     /* Do Nothing */  break;
    case tt_Char:     Char   =V.Char;   break;
    case tt_Bool:     
    case tt_Bit:     
    case tt_Byte:     Byte   =V.Byte;   break;
    case tt_Word:     Word   =V.Word;   break;
    case tt_DWord:    DWord  =V.DWord;  break;
    case tt_Int:      Int    =V.Int;    break;
    case tt_Short:    Short  =V.Short;  break;
    case tt_Long:     Long   =V.Long;   break;
    case tt_Flt16:    Flt16  =V.Flt16;  break;
    case tt_Float:    Float  =V.Float;  break;
    case tt_Double:   Double =V.Double; break;
    default:          VERIFY(0);        break;
    }
  iType=V.iType;
  iStatus=V.iStatus;
  //lt16Neg=V.bFlt16Neg;
  };

//------------------------------------------------------------------------

flag DataUnionCom::IsZero() const
  {
  switch (iType)
    {
    case tt_NULL:     return true;
    case tt_Char:     return Char   == 0; 
    case tt_Bool:     
    case tt_Bit:     
    case tt_Byte:     return Byte   == 0; 
    case tt_Word:     return Word   == 0; 
    case tt_DWord:    return DWord  == 0; 
    case tt_Int:      return Int    == 0; 
    case tt_Short:    return Short  == 0; 
    case tt_Long:     return Long   == 0; 
    case tt_Flt16:    return Flt16  == 0; 
    case tt_Float:    return Valid(Float) ? (Float == 0.0f) : false; 
    case tt_Double:   return Valid(Double) ? (Double  == 0.0) : false; 
    default:          return false;
    }
  };

////------------------------------------------------------------------------
//
//void DataUnionCom::Set(PkDataUnion &V)
//  {
//  switch (V.iType)
//    {
//    case tt_Char:     Char   =V.Char;   break;
//    case tt_Bool:     
//    case tt_Bit:     
//    case tt_Byte:     Byte   =V.Byte;   break;
//    case tt_Word:     Word   =V.Word;   break;
//    case tt_DWord:    DWord  =V.DWord;  break;
//    case tt_Int:      Int    =V.Int;    break;
//    case tt_Short:    Short  =V.Short;  break;
//    case tt_Long:     Long   =V.Long;   break;
//    case tt_Flt16:    Flt16  =V.Flt16;  break;
//    case tt_Float:    Float  =V.Float;  break;
//    case tt_Double:   Double =V.Double; break;
//    default:          VERIFY(0);        break;
//    }
//  iType=V.iType;
//  iStatus=V.iStatus;
//  //lt16Neg=V.bFlt16Neg;
//  };

//------------------------------------------------------------------------

flag DataUnionCom::Equal(DataUnionCom &V)
  {
  if (iType!=V.iType)
    return false;
  switch (iType)
    {
    case tt_NULL   : return true;
    case tt_Char   : return Char==V.Char;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : return Byte==V.Byte;
    case tt_Word   : return Word==V.Word;
    case tt_DWord  : return DWord==V.DWord;
    case tt_Int    : return Int==V.Int;
    case tt_Short  : return Short==V.Short;
    case tt_Long   : return Long==V.Long;
    case tt_Flt16  : return Flt16==V.Flt16;
    case tt_Float  : return Float==V.Float;
    case tt_Double : return Double==V.Double;
    default :        VERIFY(0); return 0;
    }
  return false;
  }

//------------------------------------------------------------------------

flag DataUnionCom::EqualNANOK(DataUnionCom &V)
  {
  if (iType!=V.iType)
    return false;
  switch (iType)
    {
    case tt_NULL   : return true;
    case tt_Char   : return Char==V.Char;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : return Byte==V.Byte;
    case tt_Word   : return Word==V.Word;
    case tt_DWord  : return DWord==V.DWord;
    case tt_Int    : return Int==V.Int;
    case tt_Short  : return Short==V.Short;
    case tt_Long   : return Long==V.Long;
    case tt_Flt16  : return Flt16==V.Flt16;
    case tt_Float  : return Valid(Float)&&Valid(V.Float) ? Float==V.Float : Valid(Float)==Valid(V.Float);
    case tt_Double : return Valid(Double)&&Valid(V.Double) ? Double==V.Double : Valid(Double)==Valid(V.Double);
    default :        VERIFY(0); return 0;
    }
  return false;
  }

//------------------------------------------------------------------------

long DataUnionCom::GetLong() const
  {
  switch (iType)
    {
    case tt_NULL   : return 0;
    case tt_Char   : return Char;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : return Byte;
    case tt_Word   : return Word;
    case tt_DWord  : return DWord;
    case tt_Int    : return Int;
    case tt_Short  : return Short;
    case tt_Long   : return Long;
    //case tt_Flt16  : return (long)(iCnv==0 ? Flt16 : Cnvs[iCnv]->Human(Flt16, pCnvTxt));
    case tt_Flt16  : return (long)Flt16;
    case tt_Float  : return (long)Float;
    case tt_Double : return (long)Double;
    default :        VERIFY(0); return 0;
    }
  }

//------------------------------------------------------------------------

long DataUnionCom::GetLong(CCnvIndex iCnv, char*pCnvTxt) const
  {
  switch (iType)
    {
    case tt_NULL   : return 0;
    case tt_Char   : return Char;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : return Byte;
    case tt_Word   : return Word;
    case tt_DWord  : return DWord;
    case tt_Int    : return Int;
    case tt_Short  : return Short;
    case tt_Long   : return Long;
    //case tt_Flt16  : return (long)(iCnv==0 ? Flt16 : Cnvs[iCnv]->Human(Flt16, pCnvTxt));
    case tt_Flt16  : return (long)Cnvs[iCnv]->Human(Flt16, pCnvTxt);
    case tt_Float  : return (long)Cnvs[iCnv]->Human(Float, pCnvTxt);
    case tt_Double : return (long)Cnvs[iCnv]->Human(Double, pCnvTxt);
    default :        VERIFY(0); return 0;
    }
  }

//------------------------------------------------------------------------

void DataUnionCom::PutLong(long L)
  {
  iStatus=ds_OK;
  switch (iType)
    {
    case tt_NULL   : break;
    case tt_Char   : Char=(char)L; break;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : Byte=(byte)L; break;
    case tt_Word   : Word=(word)L; break;
    case tt_DWord  : DWord=(dword)L; break;
    case tt_Int    : Int=(int)L; break;
    case tt_Short  : Short=(short)L; break;
    case tt_Long   : Long=(long)L; break;
    case tt_Flt16  : Flt16=(double)L; break;
    case tt_Float  : Float=(float)L; break;
    case tt_Double : Double=(double)L; break;
    default :        VERIFY(0); break;
    }
  };

//------------------------------------------------------------------------

void DataUnionCom::PutLong(long L, CCnvIndex iCnv, char*pCnvTxt)
  {
  iStatus=ds_OK;
  switch (iType)
    {
    case tt_NULL   : break;
    case tt_Char   : Char=(char)L; break;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : Byte=(byte)L; break;
    case tt_Word   : Word=(word)L; break;
    case tt_DWord  : DWord=(dword)L; break;
    case tt_Int    : Int=(int)L; break;
    case tt_Short  : Short=(short)L; break;
    case tt_Long   : Long=(long)L; break;
    case tt_Flt16  : Flt16=Cnvs[iCnv]->Normal(L, pCnvTxt); break;
    case tt_Float  : Float=(float)Cnvs[iCnv]->Normal(L, pCnvTxt); break;
    case tt_Double : Double=Cnvs[iCnv]->Normal(L, pCnvTxt); break;
    default :        VERIFY(0); break;
    }
  };

//------------------------------------------------------------------------

double DataUnionCom::GetDouble() const
  {
  switch (iType)
    {
    case tt_NULL   : return 0;
    case tt_Char   : return Char;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : return Byte;
    case tt_Word   : return Word;
    case tt_DWord  : return DWord;
    case tt_Int    : return Int;
    case tt_Short  : return Short;
    case tt_Long   : return Long;
    case tt_Flt16  : return Flt16;
    case tt_Float  : return Float;
    case tt_Double : return Double;
    default :        VERIFY(0); return dNAN;
    }
  }

//------------------------------------------------------------------------

double DataUnionCom::GetDouble(CCnvIndex iCnv, char*pCnvTxt) const
  {
  switch (iType)
    {
    case tt_NULL   : return 0;
    case tt_Char   : return Char;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : return Byte;
    case tt_Word   : return Word;
    case tt_DWord  : return DWord;
    case tt_Int    : return Int;
    case tt_Short  : return Short;
    case tt_Long   : return Long;
    case tt_Flt16  : return Cnvs[iCnv]->Human(Flt16, pCnvTxt);
    case tt_Float  : return Cnvs[iCnv]->Human(Float, pCnvTxt);
    case tt_Double : return Cnvs[iCnv]->Human(Double, pCnvTxt);
    default :        VERIFY(0); return dNAN;
    }
  }

//------------------------------------------------------------------------

void DataUnionCom::PutDouble(double D)
  {
  iStatus=ds_OK;
  switch (iType)
    {
    case tt_NULL   :             ; break;
    case tt_Char   : Char=(char)D; break;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : Byte=(byte)D; break;
    case tt_Word   : Word=(word)D; break;
    case tt_DWord  : DWord=(dword)D; break;
    case tt_Int    : Int=(int)D; break;
    case tt_Short  : Short=(short)D; break;
    case tt_Long   : Long=(long)D; break;
    case tt_Flt16  : Flt16=D; break;
    case tt_Float  : Float=(float)D; break;
    case tt_Double : Double=D; break;
    default :        VERIFY(0); break;
    }
  };

//------------------------------------------------------------------------

void DataUnionCom::PutDouble(double D, CCnvIndex iCnv, char*pCnvTxt)
  {
  iStatus=ds_OK;
  switch (iType)
    {
    case tt_NULL   :             ; break;
    case tt_Char   : Char=(char)D; break;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : Byte=(byte)D; break;
    case tt_Word   : Word=(word)D; break;
    case tt_DWord  : DWord=(dword)D; break;
    case tt_Int    : Int=(int)D; break;
    case tt_Short  : Short=(short)D; break;
    case tt_Long   : Long=(long)D; break;
    case tt_Flt16  : Flt16=(iCnv ? Cnvs[iCnv]->Normal(D, pCnvTxt) : D); break;
    case tt_Float  : Float=(float)(iCnv ? Cnvs[iCnv]->Normal(D, pCnvTxt) : D); break;
    case tt_Double : Double=(iCnv ? Cnvs[iCnv]->Normal(D, pCnvTxt) : D); break;
    default :        VERIFY(0); break;
    }
  };

//------------------------------------------------------------------------

char GetStringBuffer[2048];
int GetStringBufferPos=0;

char* DataUnionCom::GetString(char*IFmt/*="%i"*/, char*FFmt/*="%g"*/, CCnvIndex iCnv/*=0*/, char*pCnvTxt/*=NULL*/) const
  {
  char*p = &GetStringBuffer[GetStringBufferPos];
  switch (iType)
    {
    case tt_NULL   : sprintf(p, "");          break;
    case tt_Char   : sprintf(p, IFmt, Char);  break;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : sprintf(p, IFmt, Byte);  break;
    case tt_Word   : sprintf(p, IFmt, Word);  break;
    case tt_DWord  : sprintf(p, IFmt, DWord);  break;
    case tt_Int    : sprintf(p, IFmt, Int);   break;
    case tt_Short  : sprintf(p, IFmt, Short); break;
    case tt_Long   : sprintf(p, IFmt, Long);  break;
    case tt_Flt16  : if (Status()!=ds_NAN) 
                       sprintf(p, FFmt, Cnvs[iCnv]->Human(Flt16, pCnvTxt)); 
                     else 
                       strcpy(p, "*"); 
                     break;
    case tt_Float  : if (Valid(Float)) 
                       sprintf(p, FFmt, Cnvs[iCnv]->Human(Float, pCnvTxt)); 
                     else 
                       strcpy(p, "*"); 
                     break;
    case tt_Double : if (Valid(Double)) 
                       sprintf(p, FFmt, Cnvs[iCnv]->Human(Double, pCnvTxt)); 
                     else if (HasNANFlag(Double, NF_Free))
                       strcpy(p, "*Free"); 
                     else if (HasNANFlag(Double, NF_Hold))
                       strcpy(p, "*Hold"); 
                     else if (HasNANFlag(Double, NF_Pulse))
                       strcpy(p, "*Pulse"); 
                     else if (HasNANFlag(Double, NF_Ignore))
                       strcpy(p, "*Ignore"); 
                     else if (HasNANFlag(Double, NF_Block))
                       strcpy(p, "*Block"); 
                     else if (HasNANFlag(Double, NF_Plus))
                       strcpy(p, "*+"); 
                     else if (HasNANFlag(Double, NF_Minus))
                       strcpy(p, "*-"); 
                     else if (HasNANFlag(Double, NF_Star))
                       strcpy(p, "**"); 
                     else if (HasNANFlag(Double, NF_Pass))
                       strcpy(p, "*Pass"); 
                     else
                       strcpy(p, "*"); 
                     break;
    default : 
      VERIFY(0); 
      break;
    }
  
  GetStringBufferPos+=strlen(p)+1;
  if (GetStringBufferPos>(2048-512))
    GetStringBufferPos=0;
  return p;
  }

//------------------------------------------------------------------------

flag DataUnionCom::SetTypeString(byte iTypeRqd, char*pStrData, CCnvIndex iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  iStatus=ds_OK;
  switch (iTypeRqd)
    {
    case tt_NULL   :                             break;
    case tt_Char   : Char=(char)atol(pStrData);  break;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : Byte=(byte)atol(pStrData);  break;
    case tt_Word   : Word=(word)atol(pStrData);  break;
    case tt_DWord  : DWord=(dword)atol(pStrData);  break;
    case tt_Int    : Int=(int)atol(pStrData);   break;
    case tt_Short  : Short=(short)atol(pStrData); break;
    case tt_Long   : Long=(long)atol(pStrData);  break;
    case tt_Flt16  : 
      while (*pStrData==' ') pStrData++;
      if (*pStrData=='*') 
        SetStatus(ds_NAN);
      else
        Flt16=Cnvs[iCnv]->Normal(atof(pStrData), pCnvTxt); 
      break;
    case tt_Float  : 
      while (*pStrData==' ') pStrData++;
      if (*pStrData=='*') 
        Float=fNAN;
      else
        Float=(float)Cnvs[iCnv]->Normal(atof(pStrData), pCnvTxt); 
      break;
    case tt_Double : 
      while (*pStrData==' ') pStrData++;
      if (*pStrData=='*') 
        Double =ParseNANFlag(pStrData);
      else
        Double=Cnvs[iCnv]->Normal(atof(pStrData), pCnvTxt); 
      break;
    default        : VERIFY(0); break;
    }
  iType=iTypeRqd;
  return True;
  }

//========================================================================
//
//
//
//========================================================================

void DataUnion::CopyTo(void* p)
  {
  switch (iType)
    {
    case tt_NULL   : break; 
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : *((byte*)p) = Byte; break;
    case tt_Word   : *((word*)p) = Word; break;
    case tt_DWord  : *((dword*)p) = DWord; break;
    case tt_Int    : *((int*)p) = Int; break;
    case tt_Char   : *((char*)p) = Char; break;
    case tt_Short  : *((short*)p) =  Short; break;
    case tt_Long   : *((long*)p) =  Long; break;
    case tt_Flt16  : *((flt16*)p) = Flt16; break;
    case tt_Float  : *((float*)p) = Float; break;
    case tt_Double : *((double*)p) = Double; break;
    case tt_pChar  : 
    case tt_Strng  : strcpy((char*)p, pChar); break;
    default : ASSERT(0);
    }
  };

//========================================================================
//
//
//
//========================================================================

#pragma pack(push, 1)  

class CPkDataItem8    : public CPkDataItem { char m_BuffExt[8   ];  public:CPkDataItem8   () { m_wMaxSize=8   ;}; virtual ~CPkDataItem8   () {}; DEFINE_SPARES(CPkDataBuff8    ); };
class CPkDataItem16   : public CPkDataItem { char m_BuffExt[16  ];  public:CPkDataItem16  () { m_wMaxSize=16  ;}; virtual ~CPkDataItem16  () {}; DEFINE_SPARES(CPkDataBuff16   ); };
class CPkDataItem24   : public CPkDataItem { char m_BuffExt[24  ];  public:CPkDataItem24  () { m_wMaxSize=24  ;}; virtual ~CPkDataItem24  () {}; DEFINE_SPARES(CPkDataBuff24   ); };
class CPkDataItem32   : public CPkDataItem { char m_BuffExt[32  ];  public:CPkDataItem32  () { m_wMaxSize=32  ;}; virtual ~CPkDataItem32  () {}; DEFINE_SPARES(CPkDataBuff32   ); };
class CPkDataItem40   : public CPkDataItem { char m_BuffExt[40  ];  public:CPkDataItem40  () { m_wMaxSize=40  ;}; virtual ~CPkDataItem40  () {}; DEFINE_SPARES(CPkDataBuff40   ); };
class CPkDataItem48   : public CPkDataItem { char m_BuffExt[48  ];  public:CPkDataItem48  () { m_wMaxSize=48  ;}; virtual ~CPkDataItem48  () {}; DEFINE_SPARES(CPkDataBuff48   ); };
class CPkDataItem56   : public CPkDataItem { char m_BuffExt[56  ];  public:CPkDataItem56  () { m_wMaxSize=56  ;}; virtual ~CPkDataItem56  () {}; DEFINE_SPARES(CPkDataBuff56   ); };
class CPkDataItem64   : public CPkDataItem { char m_BuffExt[64  ];  public:CPkDataItem64  () { m_wMaxSize=64  ;}; virtual ~CPkDataItem64  () {}; DEFINE_SPARES(CPkDataBuff64   ); };
class CPkDataItem96   : public CPkDataItem { char m_BuffExt[96  ];  public:CPkDataItem96  () { m_wMaxSize=96  ;}; virtual ~CPkDataItem96  () {}; DEFINE_SPARES(CPkDataBuff96   ); };
class CPkDataItem128  : public CPkDataItem { char m_BuffExt[128 ];  public:CPkDataItem128 () { m_wMaxSize=128 ;}; virtual ~CPkDataItem128 () {}; DEFINE_SPARES(CPkDataBuff128  ); };
class CPkDataItem160  : public CPkDataItem { char m_BuffExt[160 ];  public:CPkDataItem160 () { m_wMaxSize=160 ;}; virtual ~CPkDataItem160 () {}; DEFINE_SPARES(CPkDataBuff160  ); };
class CPkDataItem192  : public CPkDataItem { char m_BuffExt[192 ];  public:CPkDataItem192 () { m_wMaxSize=192 ;}; virtual ~CPkDataItem192 () {}; DEFINE_SPARES(CPkDataBuff192  ); };
class CPkDataItem224  : public CPkDataItem { char m_BuffExt[224 ];  public:CPkDataItem224 () { m_wMaxSize=224 ;}; virtual ~CPkDataItem224 () {}; DEFINE_SPARES(CPkDataBuff224  ); };
class CPkDataItem256  : public CPkDataItem { char m_BuffExt[256 ];  public:CPkDataItem256 () { m_wMaxSize=256 ;}; virtual ~CPkDataItem256 () {}; DEFINE_SPARES(CPkDataBuff256  ); };
class CPkDataItem384  : public CPkDataItem { char m_BuffExt[384 ];  public:CPkDataItem384 () { m_wMaxSize=384 ;}; virtual ~CPkDataItem384 () {}; DEFINE_SPARES(CPkDataBuff384  ); };
class CPkDataItem512  : public CPkDataItem { char m_BuffExt[512 ];  public:CPkDataItem512 () { m_wMaxSize=512 ;}; virtual ~CPkDataItem512 () {}; DEFINE_SPARES(CPkDataBuff512  ); };
class CPkDataItem640  : public CPkDataItem { char m_BuffExt[640 ];  public:CPkDataItem640 () { m_wMaxSize=640 ;}; virtual ~CPkDataItem640 () {}; DEFINE_SPARES(CPkDataBuff640  ); };
class CPkDataItem768  : public CPkDataItem { char m_BuffExt[768 ];  public:CPkDataItem768 () { m_wMaxSize=768 ;}; virtual ~CPkDataItem768 () {}; DEFINE_SPARES(CPkDataBuff768  ); };
class CPkDataItem896  : public CPkDataItem { char m_BuffExt[896 ];  public:CPkDataItem896 () { m_wMaxSize=896 ;}; virtual ~CPkDataItem896 () {}; DEFINE_SPARES(CPkDataBuff896  ); };
class CPkDataItem1024 : public CPkDataItem { char m_BuffExt[1024];  public:CPkDataItem1024() { m_wMaxSize=1024;}; virtual ~CPkDataItem1024() {}; DEFINE_SPARES(CPkDataBuff1024 ); };
class CPkDataItem1152 : public CPkDataItem { char m_BuffExt[1152];  public:CPkDataItem1152() { m_wMaxSize=1152;}; virtual ~CPkDataItem1152() {}; DEFINE_SPARES(CPkDataBuff1152 ); };
class CPkDataItem1280 : public CPkDataItem { char m_BuffExt[1280];  public:CPkDataItem1280() { m_wMaxSize=1280;}; virtual ~CPkDataItem1280() {}; DEFINE_SPARES(CPkDataBuff1280 ); };
class CPkDataItem1408 : public CPkDataItem { char m_BuffExt[1408];  public:CPkDataItem1408() { m_wMaxSize=1408;}; virtual ~CPkDataItem1408() {}; DEFINE_SPARES(CPkDataBuff1408 ); };
class CPkDataItem1536 : public CPkDataItem { char m_BuffExt[1536];  public:CPkDataItem1536() { m_wMaxSize=1536;}; virtual ~CPkDataItem1536() {}; DEFINE_SPARES(CPkDataBuff1536 ); };
class CPkDataItem1664 : public CPkDataItem { char m_BuffExt[1664];  public:CPkDataItem1664() { m_wMaxSize=1664;}; virtual ~CPkDataItem1664() {}; DEFINE_SPARES(CPkDataBuff1664 ); };
class CPkDataItem1792 : public CPkDataItem { char m_BuffExt[1792];  public:CPkDataItem1792() { m_wMaxSize=1792;}; virtual ~CPkDataItem1792() {}; DEFINE_SPARES(CPkDataBuff1792 ); };
class CPkDataItem1920 : public CPkDataItem { char m_BuffExt[1920];  public:CPkDataItem1920() { m_wMaxSize=1920;}; virtual ~CPkDataItem1920() {}; DEFINE_SPARES(CPkDataBuff1920 ); };
class CPkDataItem2048 : public CPkDataItem { char m_BuffExt[2048];  public:CPkDataItem2048() { m_wMaxSize=2048;}; virtual ~CPkDataItem2048() {}; DEFINE_SPARES(CPkDataBuff2048 ); };
class CPkDataItem2176 : public CPkDataItem { char m_BuffExt[2176];  public:CPkDataItem2176() { m_wMaxSize=2176;}; virtual ~CPkDataItem2176() {}; DEFINE_SPARES(CPkDataBuff2176 ); };
class CPkDataItem2304 : public CPkDataItem { char m_BuffExt[2304];  public:CPkDataItem2304() { m_wMaxSize=2304;}; virtual ~CPkDataItem2304() {}; DEFINE_SPARES(CPkDataBuff2304 ); };
class CPkDataItem2432 : public CPkDataItem { char m_BuffExt[2432];  public:CPkDataItem2432() { m_wMaxSize=2432;}; virtual ~CPkDataItem2432() {}; DEFINE_SPARES(CPkDataBuff2432 ); };
class CPkDataItem2560 : public CPkDataItem { char m_BuffExt[2560];  public:CPkDataItem2560() { m_wMaxSize=2560;}; virtual ~CPkDataItem2560() {}; DEFINE_SPARES(CPkDataBuff2560 ); };
class CPkDataItem2688 : public CPkDataItem { char m_BuffExt[2688];  public:CPkDataItem2688() { m_wMaxSize=2688;}; virtual ~CPkDataItem2688() {}; DEFINE_SPARES(CPkDataBuff2688 ); };
class CPkDataItem2816 : public CPkDataItem { char m_BuffExt[2816];  public:CPkDataItem2816() { m_wMaxSize=2816;}; virtual ~CPkDataItem2816() {}; DEFINE_SPARES(CPkDataBuff2816 ); };
class CPkDataItem2944 : public CPkDataItem { char m_BuffExt[2944];  public:CPkDataItem2944() { m_wMaxSize=2944;}; virtual ~CPkDataItem2944() {}; DEFINE_SPARES(CPkDataBuff2944 ); };
class CPkDataItem3072 : public CPkDataItem { char m_BuffExt[3072];  public:CPkDataItem3072() { m_wMaxSize=3072;}; virtual ~CPkDataItem3072() {}; DEFINE_SPARES(CPkDataBuff3072 ); };
class CPkDataItem3200 : public CPkDataItem { char m_BuffExt[3200];  public:CPkDataItem3200() { m_wMaxSize=3200;}; virtual ~CPkDataItem3200() {}; DEFINE_SPARES(CPkDataBuff3200 ); };
class CPkDataItem3328 : public CPkDataItem { char m_BuffExt[3328];  public:CPkDataItem3328() { m_wMaxSize=3328;}; virtual ~CPkDataItem3328() {}; DEFINE_SPARES(CPkDataBuff3328 ); };
class CPkDataItem3456 : public CPkDataItem { char m_BuffExt[3456];  public:CPkDataItem3456() { m_wMaxSize=3456;}; virtual ~CPkDataItem3456() {}; DEFINE_SPARES(CPkDataBuff3456 ); };
class CPkDataItem3584 : public CPkDataItem { char m_BuffExt[3584];  public:CPkDataItem3584() { m_wMaxSize=3584;}; virtual ~CPkDataItem3584() {}; DEFINE_SPARES(CPkDataBuff3584 ); };
class CPkDataItem3712 : public CPkDataItem { char m_BuffExt[3712];  public:CPkDataItem3712() { m_wMaxSize=3712;}; virtual ~CPkDataItem3712() {}; DEFINE_SPARES(CPkDataBuff3712 ); };
class CPkDataItem3840 : public CPkDataItem { char m_BuffExt[3840];  public:CPkDataItem3840() { m_wMaxSize=3840;}; virtual ~CPkDataItem3840() {}; DEFINE_SPARES(CPkDataBuff3840 ); };
class CPkDataItem3968 : public CPkDataItem { char m_BuffExt[3968];  public:CPkDataItem3968() { m_wMaxSize=3968;}; virtual ~CPkDataItem3968() {}; DEFINE_SPARES(CPkDataBuff3968 ); };
class CPkDataItem4096 : public CPkDataItem { char m_BuffExt[4096];  public:CPkDataItem4096() { m_wMaxSize=4096;}; virtual ~CPkDataItem4096() {}; DEFINE_SPARES(CPkDataBuff4096 ); };
class CPkDataItem4224 : public CPkDataItem { char m_BuffExt[4224];  public:CPkDataItem4224() { m_wMaxSize=4224;}; virtual ~CPkDataItem4224() {}; DEFINE_SPARES(CPkDataBuff4224 ); };
const long BiggestBuffer=4224;

//------------------------------------------------------------------------

IMPLEMENT_SPARES(CPkDataItem8    ,10000);
IMPLEMENT_SPARES(CPkDataItem16   ,10000);
IMPLEMENT_SPARES(CPkDataItem24   ,1000);
IMPLEMENT_SPARES(CPkDataItem32   ,1000);
IMPLEMENT_SPARES(CPkDataItem40   ,1000);
IMPLEMENT_SPARES(CPkDataItem48   ,1000);
IMPLEMENT_SPARES(CPkDataItem56   ,1000);
IMPLEMENT_SPARES(CPkDataItem64   ,500);
IMPLEMENT_SPARES(CPkDataItem96   ,500);
IMPLEMENT_SPARES(CPkDataItem128  ,500);
IMPLEMENT_SPARES(CPkDataItem160  ,100);
IMPLEMENT_SPARES(CPkDataItem192  ,100);
IMPLEMENT_SPARES(CPkDataItem224  ,100);
IMPLEMENT_SPARES(CPkDataItem256  ,100);
IMPLEMENT_SPARES(CPkDataItem384  ,100);
IMPLEMENT_SPARES(CPkDataItem512  ,100);
IMPLEMENT_SPARES(CPkDataItem640  ,100);
IMPLEMENT_SPARES(CPkDataItem768  ,100);
IMPLEMENT_SPARES(CPkDataItem896  ,100);
IMPLEMENT_SPARES(CPkDataItem1024 ,10);
IMPLEMENT_SPARES(CPkDataItem1152 ,10);
IMPLEMENT_SPARES(CPkDataItem1280 ,10);
IMPLEMENT_SPARES(CPkDataItem1408 ,10);
IMPLEMENT_SPARES(CPkDataItem1536 ,10);
IMPLEMENT_SPARES(CPkDataItem1664 ,10);
IMPLEMENT_SPARES(CPkDataItem1792 ,10);
IMPLEMENT_SPARES(CPkDataItem1920 ,10);
IMPLEMENT_SPARES(CPkDataItem2048 ,10);
IMPLEMENT_SPARES(CPkDataItem2176 ,10);
IMPLEMENT_SPARES(CPkDataItem2304 ,10);
IMPLEMENT_SPARES(CPkDataItem2432 ,10);
IMPLEMENT_SPARES(CPkDataItem2560 ,10);
IMPLEMENT_SPARES(CPkDataItem2688 ,10);
IMPLEMENT_SPARES(CPkDataItem2816 ,10);
IMPLEMENT_SPARES(CPkDataItem2944 ,10);
IMPLEMENT_SPARES(CPkDataItem3072 ,10);
IMPLEMENT_SPARES(CPkDataItem3200 ,10);
IMPLEMENT_SPARES(CPkDataItem3328 ,10);
IMPLEMENT_SPARES(CPkDataItem3456 ,10);
IMPLEMENT_SPARES(CPkDataItem3584 ,10);
IMPLEMENT_SPARES(CPkDataItem3712 ,10);
IMPLEMENT_SPARES(CPkDataItem3840 ,10);
IMPLEMENT_SPARES(CPkDataItem3968 ,10);
IMPLEMENT_SPARES(CPkDataItem4096 ,10);
IMPLEMENT_SPARES(CPkDataItem4224 ,10);

#pragma pack(pop)  

//========================================================================
//
//
//
//========================================================================
               
CPkDataItem::CPkDataItem() 
  { 
  //m_pData=NULL;
  Clear();//NULL); 
  };

//------------------------------------------------------------------------
//
CPkDataItem * CPkDataItem::Create(long Sz)
  {
  CPkDataItem *p=NULL;

  Sz=Sz-1;
  Sz=Max(0L, Sz);

  switch (Sz/8)
    {
    case 0: p = new CPkDataItem8; break;
    case 1: p = new CPkDataItem16; break;
    case 2: p = new CPkDataItem24; break;
    case 3: p = new CPkDataItem32; break;
    case 4: p = new CPkDataItem40; break;
    case 5: p = new CPkDataItem48; break;
    case 6: p = new CPkDataItem56; break;
    case 7: p = new CPkDataItem64; break;
    default:
      switch (Sz/32)
        {
        case 0: p = new CPkDataItem32; break; // wont happen
        case 1: p = new CPkDataItem64; break; // wont happen
        case 2: p = new CPkDataItem96; break;
        case 3: p = new CPkDataItem128; break;
        case 4: p = new CPkDataItem160; break;
        case 5: p = new CPkDataItem192; break;
        case 6: p = new CPkDataItem224; break;
        case 7: p = new CPkDataItem256; break;
        default:
          switch (Sz/128)
            {
            case 0 : p = new CPkDataItem128; break; // wont happen
            case 1 : p = new CPkDataItem256; break; // wont happen
            case 2 : p = new CPkDataItem384; break;
            case 3 : p = new CPkDataItem512; break;
            case 4 : p = new CPkDataItem640; break;
            case 5 : p = new CPkDataItem768; break;
            case 6 : p = new CPkDataItem896; break;
            case 7 : p = new CPkDataItem1024; break;
            case 8 : p = new CPkDataItem1152; break;
            case 9 : p = new CPkDataItem1280; break;
            case 10: p = new CPkDataItem1408; break;
            case 11: p = new CPkDataItem1536; break;
            case 12: p = new CPkDataItem1664; break;
            case 13: p = new CPkDataItem1792; break;
            case 14: p = new CPkDataItem1920; break;
            case 15: p = new CPkDataItem2048; break;
            case 16: p = new CPkDataItem2176; break;
            case 17: p = new CPkDataItem2304; break;
            case 18: p = new CPkDataItem2432; break;
            case 19: p = new CPkDataItem2560; break;
            case 20: p = new CPkDataItem2688; break;
            case 21: p = new CPkDataItem2816; break;
            case 22: p = new CPkDataItem2944; break;
            case 23: p = new CPkDataItem3072; break;
            case 24: p = new CPkDataItem3200; break;
            case 25: p = new CPkDataItem3328; break;
            case 26: p = new CPkDataItem3456; break;
            case 27: p = new CPkDataItem3584; break;
            case 28: p = new CPkDataItem3712; break;
            case 29: p = new CPkDataItem3840; break;
            case 30: p = new CPkDataItem3968; break;
            case 31: p = new CPkDataItem4096; break;
            case 32: p = new CPkDataItem4224; break;
            default: 
              ASSERT_ALWAYS(FALSE, "CPkDataItem Buffer overflow", __FILE__, __LINE__);
            }
        }
    }
  ASSERT(p->MaxDataSize()>=Sz);

  p->Clear();

  return p;
  };

//------------------------------------------------------------------------
//
CPkDataItem * CPkDataItem::Create(const CPkDataItem &C)
  {
  CPkDataItem *p=NULL;

  long Sz=C.DataSize()-1;
  Sz=Max(0L, Sz);

  switch (Sz/8)
    {
    case 0: p = new CPkDataItem8; break;
    case 1: p = new CPkDataItem16; break;
    case 2: p = new CPkDataItem24; break;
    case 3: p = new CPkDataItem32; break;
    case 4: p = new CPkDataItem40; break;
    case 5: p = new CPkDataItem48; break;
    case 6: p = new CPkDataItem56; break;
    case 7: p = new CPkDataItem64; break;
    default:
      switch (Sz/32)
        {
        case 0: p = new CPkDataItem32; break; // wont happen
        case 1: p = new CPkDataItem64; break; // wont happen
        case 2: p = new CPkDataItem96; break;
        case 3: p = new CPkDataItem128; break;
        case 4: p = new CPkDataItem160; break;
        case 5: p = new CPkDataItem192; break;
        case 6: p = new CPkDataItem224; break;
        case 7: p = new CPkDataItem256; break;
        default:
          switch (Sz/128)
            {
            case 0 : p = new CPkDataItem128; break; // wont happen
            case 1 : p = new CPkDataItem256; break; // wont happen
            case 2 : p = new CPkDataItem384; break;
            case 3 : p = new CPkDataItem512; break;
            case 4 : p = new CPkDataItem640; break;
            case 5 : p = new CPkDataItem768; break;
            case 6 : p = new CPkDataItem896; break;
            case 7 : p = new CPkDataItem1024; break;
            case 8 : p = new CPkDataItem1152; break;
            case 9 : p = new CPkDataItem1280; break;
            case 10: p = new CPkDataItem1408; break;
            case 11: p = new CPkDataItem1536; break;
            case 12: p = new CPkDataItem1664; break;
            case 13: p = new CPkDataItem1792; break;
            case 14: p = new CPkDataItem1920; break;
            case 15: p = new CPkDataItem2048; break;
            case 16: p = new CPkDataItem2176; break;
            case 17: p = new CPkDataItem2304; break;
            case 18: p = new CPkDataItem2432; break;
            case 19: p = new CPkDataItem2560; break;
            case 20: p = new CPkDataItem2688; break;
            case 21: p = new CPkDataItem2816; break;
            case 22: p = new CPkDataItem2944; break;
            case 23: p = new CPkDataItem3072; break;
            case 24: p = new CPkDataItem3200; break;
            case 25: p = new CPkDataItem3328; break;
            case 26: p = new CPkDataItem3456; break;
            case 27: p = new CPkDataItem3584; break;
            case 28: p = new CPkDataItem3712; break;
            case 29: p = new CPkDataItem3840; break;
            case 30: p = new CPkDataItem3968; break;
            case 31: p = new CPkDataItem4096; break;
            case 32: p = new CPkDataItem4224; break;
            default: 
              ASSERT_ALWAYS(FALSE, "CPkDataItem Buffer overflow", __FILE__, __LINE__);
            }
        }
    }
  ASSERT(p->MaxDataSize()>=C.DataSize());

  p->m_wContents=C.m_wContents;
  p->m_wSize=C.m_wSize;
  memcpy(p->Buffer(), C.Buffer(), C.DataSize());

  return p;
  };

//------------------------------------------------------------------------
//
CPkDataItem * CPkDataItem::Create(const CPkDataItemOld &C)
  {
  CPkDataItem *p=NULL;

  long Sz=C.m_wSize-1;
  Sz=Max(0L, Sz);

  switch (Sz/8)
    {
    case 0: p = new CPkDataItem8; break;
    case 1: p = new CPkDataItem16; break;
    case 2: p = new CPkDataItem24; break;
    case 3: p = new CPkDataItem32; break;
    case 4: p = new CPkDataItem40; break;
    case 5: p = new CPkDataItem48; break;
    case 6: p = new CPkDataItem56; break;
    case 7: p = new CPkDataItem64; break;
    default:
      switch (Sz/32)
        {
        case 0: p = new CPkDataItem32; break; // wont happen
        case 1: p = new CPkDataItem64; break; // wont happen
        case 2: p = new CPkDataItem96; break;
        case 3: p = new CPkDataItem128; break;
        case 4: p = new CPkDataItem160; break;
        case 5: p = new CPkDataItem192; break;
        case 6: p = new CPkDataItem224; break;
        case 7: p = new CPkDataItem256; break;
        default:
          switch (Sz/128)
            {
            case 0 : p = new CPkDataItem128; break; // wont happen
            case 1 : p = new CPkDataItem256; break; // wont happen
            case 2 : p = new CPkDataItem384; break;
            case 3 : p = new CPkDataItem512; break;
            case 4 : p = new CPkDataItem640; break;
            case 5 : p = new CPkDataItem768; break;
            case 6 : p = new CPkDataItem896; break;
            case 7 : p = new CPkDataItem1024; break;
            case 8 : p = new CPkDataItem1152; break;
            case 9 : p = new CPkDataItem1280; break;
            case 10: p = new CPkDataItem1408; break;
            case 11: p = new CPkDataItem1536; break;
            case 12: p = new CPkDataItem1664; break;
            case 13: p = new CPkDataItem1792; break;
            case 14: p = new CPkDataItem1920; break;
            case 15: p = new CPkDataItem2048; break;
            case 16: p = new CPkDataItem2176; break;
            case 17: p = new CPkDataItem2304; break;
            case 18: p = new CPkDataItem2432; break;
            case 19: p = new CPkDataItem2560; break;
            case 20: p = new CPkDataItem2688; break;
            case 21: p = new CPkDataItem2816; break;
            case 22: p = new CPkDataItem2944; break;
            case 23: p = new CPkDataItem3072; break;
            case 24: p = new CPkDataItem3200; break;
            case 25: p = new CPkDataItem3328; break;
            case 26: p = new CPkDataItem3456; break;
            case 27: p = new CPkDataItem3584; break;
            case 28: p = new CPkDataItem3712; break;
            case 29: p = new CPkDataItem3840; break;
            case 30: p = new CPkDataItem3968; break;
            case 31: p = new CPkDataItem4096; break;
            case 32: p = new CPkDataItem4224; break;
            default: 
              ASSERT_ALWAYS(FALSE, "CPkDataItem Buffer overflow", __FILE__, __LINE__);
            }
        }
    }


  p->m_wContents=C.m_wContents;
  p->m_wSize=C.m_wSize;
  memcpy(p->Buffer(), &C.m_cData[0], C.m_wSize);

  return p;
  };

//------------------------------------------------------------------------

CPkDataItem::~CPkDataItem() 
  {
  };

//------------------------------------------------------------------------

void CPkDataItem::Clear()
  { 
  m_wContents=0; 
  m_wSize=0;
  };

//------------------------------------------------------------------------

bool CPkDataItem::operator==(const CPkDataItem &C) 
  { 
  return (C.m_wContents==m_wContents) && (C.m_wSize==m_wSize) && (memcmp(&C.m_Data[0], &m_Data[0], m_wSize)==0); 
  };
bool CPkDataItem::operator!=(const CPkDataItem &C)
  { 
  return (C.m_wContents!=m_wContents) || (C.m_wSize!=m_wSize) || (memcmp(&C.m_Data[0], &m_Data[0], m_wSize)!=0); 
  };
CPkDataItem & CPkDataItem::operator=(const CPkDataItem &C) 
  {
  m_wContents=C.m_wContents;
  m_wSize=C.m_wSize;
  memcpy(&m_Data[0], &C.m_Data[0], m_wSize); 
  return *this; 
  };

//------------------------------------------------------------------------

inline int CPkDataItem::CompactAStr(LPSTR pStr)
  {
    {
    strcpy(CurrentPtr(), pStr);
    //if (pInfo)
    //  pInfo->AddStr(CurrentPtr());
    m_wSize+=strlen(pStr)+1;
    return 0;
    }
  }

//------------------------------------------------------------------------

LPSTR CPkDataItem::SkipAStr(LPSTR p)
  {
  p+=strlen(p)+1; 
  return p;
  };

//------------------------------------------------------------------------

LPSTR CPkDataItem::ExtractAStr(LPSTR p)
  {
  return p;
  };

//------------------------------------------------------------------------

static flag TstContentsClear(dword m_wContents, dword i)
  {
  dword tst=m_wContents;
  while (i<0x8000)
    {
    if ((i & tst)!=0)
      return 0;
    i=i<<1;
    }
  return 1;
  }

#ifdef _RELEASE
#define TSTCONTENTSCLEAR(a,b)
#else
#define TSTCONTENTSCLEAR(a,b) {if (!TstContentsClear(a,b)) DoBreak();} 
#endif

//------------------------------------------------------------------------

void CPkDataItem::AppendValue(DDEF_Flags iFlags, PkDataUnion &PData) 
  { 
  TSTCONTENTSCLEAR(m_wContents, PDI_Value);
  if (iFlags)
    {
    m_wContents|=PDI_Flags|PDI_Flags64;
    memcpy(CurrentPtr(), &iFlags, sizeof(iFlags));
    m_wSize+=sizeof(iFlags);
    }

  m_wContents|=PDI_Value;
  char * pStart=CurrentPtr();
  word sz=PData.Size();
  VERIFY(sz>0);
  memcpy(CurrentPtr(), (char*)&PData, sz);
  m_wSize+=sz;
  char * pEnd=CurrentPtr();
  }

//------------------------------------------------------------------------

void CPkDataItem::AppendTag(pchar pTag, pchar pSym)
  {
  TSTCONTENTSCLEAR(m_wContents, PDI_Tag);
  m_wContents|=PDI_Tag;
  CompactAStr(pSym);
  CompactAStr(pTag);
  };

//------------------------------------------------------------------------

void CPkDataItem::AppendTagOfParent(char * pTag)
  {
  if (pTag && (strlen(pTag)>0))
    {
    TSTCONTENTSCLEAR(m_wContents, PDI_TagOfParent);
    m_wContents|=PDI_TagOfParent;
    strcpy(CurrentPtr(), pTag);
    m_wSize+=strlen(pTag)+1;
    }
  };

//------------------------------------------------------------------------

void CPkDataItem::AppendDescription(char * pDesc)
  {
  if (pDesc && (strlen(pDesc)>0))
    {
    TSTCONTENTSCLEAR(m_wContents, PDI_Description);
    m_wContents|=PDI_Description;
    CompactAStr(pDesc);
    //strcpy(CurrentPtr(), pDesc);
    //m_wSize+=strlen(pDesc)+1;
    }
  };

//------------------------------------------------------------------------

void CPkDataItem::AppendTagComment(char * pTagComment)
  {
  if (pTagComment && (strlen(pTagComment)>0))
    {
    TSTCONTENTSCLEAR(m_wContents, PDI_TagComment);
    m_wContents|=PDI_TagComment;
    CompactAStr(pTagComment);
    //m_wSize+=strlen(pTagComment)+1;
    }
  };

//------------------------------------------------------------------------

void CPkDataItem::AppendSubClass(char * pSubClass)
  {
  if (pSubClass && (strlen(pSubClass)>0))
    {
    TSTCONTENTSCLEAR(m_wContents, PDI_SubClass);
    m_wContents|=PDI_SubClass;
    strcpy(CurrentPtr(), pSubClass);
    m_wSize+=strlen(pSubClass)+1;
    }
  };

//------------------------------------------------------------------------

void CPkDataItem::AppendPrimaryCfg(char * pPrimaryCfg)
  {
  if (pPrimaryCfg && (strlen(pPrimaryCfg)>0))
    {
    TSTCONTENTSCLEAR(m_wContents, PDI_PrimaryCfg);
    m_wContents|=PDI_PrimaryCfg;
    strcpy(CurrentPtr(), pPrimaryCfg);
    m_wSize+=strlen(pPrimaryCfg)+1;
    }
  };

//------------------------------------------------------------------------

void CPkDataItem::AppendNElements(DDEF_NElements nElems)
  {
  TSTCONTENTSCLEAR(m_wContents, PDI_NElements);
  m_wContents|=PDI_NElements;
  memcpy(CurrentPtr(), &nElems, sizeof(nElems));
  m_wSize+=sizeof(nElems);
  };

//------------------------------------------------------------------------

#if WITHEQUIPSPECS
void CPkDataItem::AppendItemStatus(CTagRefStatus S, CTagValueStatus V)
#else
void CPkDataItem::AppendItemStatus(CTagRefStatus S)
#endif
  {
  TSTCONTENTSCLEAR(m_wContents, PDI_XRefStatus);
  m_wContents|=PDI_XRefStatus;
  memcpy(CurrentPtr(), &S, sizeof(S));
  m_wSize+=sizeof(S);
#if WITHEQUIPSPECS
  memcpy(CurrentPtr(), &V, sizeof(V));
  m_wSize+=sizeof(V);
#endif
  };

//------------------------------------------------------------------------

void CPkDataItem::AppendRange(DataUnion &DMin, DataUnion &DMax)
  {
  PkDataUnion PMin, PMax;
  TSTCONTENTSCLEAR(m_wContents, PDI_Range);
  m_wContents|=PDI_Range;

  char * pStart=CurrentPtr();

  PMin.Set(DMin);
  memcpy(CurrentPtr(), &PMin, PMin.Size());
  m_wSize+=PMin.Size();
  PMax.Set(DMax);
  memcpy(CurrentPtr(), &PMax, PMax.Size());
  m_wSize+=PMax.Size();
  };

//------------------------------------------------------------------------

void CPkDataItem::AppendCnvInfo(CCnvIndex iCnv, char * pCnvTxt, char * pCnvFam)
  {
  TSTCONTENTSCLEAR(m_wContents, PDI_CnvInfo);
  m_wContents|=PDI_CnvInfo;
  char * pStart=CurrentPtr();
  *((CCnvIndex*)(CurrentPtr()/*+m_wSize*/))=iCnv;
  strcpy(CurrentPtr()+sizeof(CCnvIndex), pCnvTxt);
  m_wSize+=sizeof(CCnvIndex)+strlen(pCnvTxt)+1;
  
  strcpy(CurrentPtr(), pCnvFam);
  m_wSize+=strlen(pCnvFam)+1;

  };

//------------------------------------------------------------------------

void CPkDataItem::AppendStrListItem(char * pStr, long Index, flag Indexed, pword & pStrLstLen)
  {
  char*p=CurrentPtr();
  if (pStrLstLen==NULL)
    {
    TSTCONTENTSCLEAR(m_wContents, PDI_StrList);
    m_wContents|=PDI_StrList;
    word n=1;// One Item

    pStrLstLen=(word*)p;

    *pStrLstLen=n;
    m_wSize+=sizeof(n);
    p+=sizeof(n);
  
    *((flag*)p)=Indexed;
    m_wSize+=sizeof(Indexed);
    p+=sizeof(Indexed);
    }
  else
    *pStrLstLen=*pStrLstLen+1;
  
  if (Indexed)
    {
    *((long*)p)=Index;
    m_wSize+=sizeof(long);
    p+=sizeof(long);
    }
  strcpy(p, pStr);
  int Len=strlen(pStr);
  p+=Len+1;
  m_wSize+=Len+1;
  };

//------------------------------------------------------------------------

//void CPkDataItem::AppendNextStrListItem(char *pStr, long Index, flag Indexed, pword pStrLstLen)
//  {
//  //TCONTENTSCLEAR(m_wContents, PDI_StrList);
//  //ontents|=PDI_StrList;
//
//  *pStrLstLen=*pStrLstLen+1;
//  char*p=CurrentPtr();
//  
//  if (Indexed)
//    {
//    *((long*)p)=Index;
//    wSize+=sizeof(long);
//    p+=sizeof(long);
//    }
//  strcpy(p, pStr);
//  int Len=strlen(pStr);
//  p+=Len+1;
//  wSize+=Len+1;
//  };

//------------------------------------------------------------------------

void* CPkDataItem::AddressOf(word what)
  {
  // NBNB 
  // this arithmetic must  be in the order of the PDI_ constants 
  char* p=Buffer();//m_pData->Start();
  if (what == PDI_Flags)            return (char*)p;
  if (m_wContents & PDI_Flags)        p += (m_wContents & PDI_Flags64) ? sizeof(DDEF_Flags64) : sizeof(DDEF_Flags32); 

  if (what == PDI_Value)            return (char*)p;
  if (m_wContents & PDI_Value)        p+=((PkDataUnion*)p)->Size();

  if (what == PDI_Tag)              return (char*)p;
  if (m_wContents & PDI_Tag)          
    {                               // Skip Sym
                                    p=SkipAStr(p);
                                    // Skip Tag
                                    p=SkipAStr(p);
    }
  
  if (what == PDI_TagOfParent)      return (char*)p;
  if (m_wContents & PDI_TagOfParent)  p+=strlen(p)+1;

  if (what == PDI_CnvInfo)          {
                                    return (char*)p;
                                    }
  if (m_wContents & PDI_CnvInfo)    { 
                                    p+=sizeof(CCnvIndex); // Skip CnvIndex
                                    p+=strlen(p)+1;       // Skip CnvStr
                                    p+=strlen(p)+1;       // Skip CnvFam
                                    }

  if (what == PDI_NElements)        return (char*)p;
  if (m_wContents & PDI_NElements)    p+=sizeof(DDEF_NElements);
  
  if (what == PDI_XRefStatus)       return (char*)p;
  if (m_wContents & PDI_XRefStatus) {
                                    p+=sizeof(CTagRefStatus);
#if WITHEQUIPSPECS
                                    p+=sizeof(CTagValueStatus);
#endif
                                    } 
  if (what == PDI_Range)            return (char*)p;
  if (m_wContents & PDI_Range)      { 
                                    PkDataUnion* pUn=(PkDataUnion*)p;
                                    p+=pUn->Size();
                                    PkDataUnion* pUx=(PkDataUnion*)p;
                                    p+=pUx->Size();
                                    }
  if (what == PDI_StrList)          return (char*)p;
  if (m_wContents & PDI_StrList)      { 
                                    word n=*((word*)p);
                                    p+=sizeof(word);
                                    flag Indexed=*((flag*)p);
                                    p+=sizeof(flag);
                                    for(word i=0; i<n; i++)
                                      {
                                      if (Indexed)
                                        p+=sizeof(long);
                                      p+=strlen(p)+1;
                                      }
                                    }

  if (what == PDI_Description)      return (char*)p;
  if (m_wContents & PDI_Description)  p=SkipAStr(p);
  
  if (what == PDI_TagComment)       return (char*)p;
  if (m_wContents & PDI_TagComment)   p=SkipAStr(p);
   
  if (what == PDI_SubClass)           return (char*)p;
  if (m_wContents & PDI_SubClass)       p+=strlen(p)+1;

  if (what == PDI_PrimaryCfg)           return (char*)p;
  if (m_wContents & PDI_PrimaryCfg)       p+=strlen(p)+1;

  //if (what == PDI_Blob) return (char*)p;
  //if (m_wContents & PDI_Blob)   { ASSERT(0); word l=*((word*)p); p+=sizeof(word)+l+1; }
  
  VERIFY(0);
  return NULL;
  }

//------------------------------------------------------------------------
/*
void* CPkDataItem::AddressOf(word what)
  {
  // NBNB 
  // this arithmetic must  be in the order of the PDI_ constants 
#if WITHCOMPACT
  char* p=(m_wContents & PDI_Compact) ? m_cDataCompact : cData;
#else
  char* p=cData;
#endif
  if (what == PDI_Flags)            return (char*)p;
  if (m_wContents & PDI_Flags)        
    {
                                    if (m_wContents & PDI_Flags64)
                                      {   
#if WITHCOMPACT
                                      if (m_FlgRL && (m_wContents & PDI_Compact))
                                        p+=m_FlgRL;//1+*p;
                                      else
#endif
                                        p+=sizeof(DDEF_Flags64);
                                      }
                                    else
                                      p+=sizeof(DDEF_Flags32); 
    };
  if (what == PDI_Value)            {
#if WITHCOMPACT
                                    if (m_ValRL && (m_wContents & PDI_Compact))
                                      {
                                      long i=0;
                                      memcpy(&i, p, m_ValRL);
                                      p-=i;
                                      }
#endif
                                    return (char*)p;
                                    }
  if (m_wContents & PDI_Value)        { 
#if WITHCOMPACT
                                    if (m_ValRL && (m_wContents & PDI_Compact))
                                      p+=m_ValRL;//1+*p;
                                    else
#endif
                                      p+=((PkDataUnion*)p)->Size();
                                    }

  if (what == PDI_Tag)              return (char*)p;
  if (m_wContents & PDI_Tag)          
    {                               // Skip Sym
                                    p=SkipAStr(p, m_SymRL);
                                    // Skip Tag
                                    p=SkipAStr(p, m_TagRL);
    }
  
  if (what == PDI_TagOfParent)      return (char*)p;
  if (m_wContents & PDI_TagOfParent)  p+=strlen(p)+1;

  if (what == PDI_CnvInfo)          {
#if WITHCOMPACT
                                    if (m_CnvRL && (m_wContents & PDI_Compact))
                                      {
                                      long i=0;
                                      memcpy(&i, p, m_CnvRL);
                                      p-=i;
                                      }
#endif
                                    return (char*)p;
                                    }
  if (m_wContents & PDI_CnvInfo)      { 
#if WITHCOMPACT
                                    if (m_CnvRL && (m_wContents & PDI_Compact))
                                      p+=m_CnvRL;//1+*p;
                                    else
#endif
                                      {
                                      p+=sizeof(CCnvIndex); // Skip CnvIndex
                                      p+=strlen(p)+1;       // Skip CnvStr
                                      p+=strlen(p)+1;       // Skip CnvFam
                                      }
                                    }

  if (what == PDI_NElements)        return (char*)p;
  if (m_wContents & PDI_NElements)    p+=sizeof(DDEF_NElements);
  
  if (what == PDI_XRefStatus)       return (char*)p;
  if (m_wContents & PDI_XRefStatus)   p+=sizeof(CTagRefStatusWord);
  
  if (what == PDI_Range)            {
#if WITHCOMPACT
                                    if (m_RngRL && (m_wContents & PDI_Compact))
                                      {
                                      long i=0;
                                      memcpy(&i, p, m_RngRL);
                                      p-=i;
                                      }
#endif
                                    return (char*)p;
                                    }
  if (m_wContents & PDI_Range)        { 
#if WITHCOMPACT
                                    if (m_RngRL && (m_wContents & PDI_Compact))
                                      p+=m_RngRL;//1+*p;
                                    else
#endif
                                      {
                                      PkDataUnion* pUn=(PkDataUnion*)p;
                                      p+=pUn->Size();
                                      PkDataUnion* pUx=(PkDataUnion*)p;
                                      p+=pUx->Size();
                                      }
                                    }
  if (what == PDI_StrList)          {
#if WITHCOMPACT
                                    if (m_LstRL && (m_wContents & PDI_Compact))
                                      {
                                      long i=0;
                                      memcpy(&i, p, m_LstRL);
                                      p-=i;
                                      }
#endif
                                    return (char*)p;
                                    }
  if (m_wContents & PDI_StrList)      { 
#if WITHCOMPACT
                                    if (m_LstRL && (m_wContents & PDI_Compact))
                                      p+=m_LstRL;//1+*p;
                                    else
#endif
                                      {
                                      word n=*((word*)p);
                                      p+=sizeof(word);
                                      flag Indexed=*((flag*)p);
                                      p+=sizeof(flag);
                                      for(word i=0; i<n; i++)
                                        {
                                        if (Indexed)
                                          p+=sizeof(long);
                                        p+=strlen(p)+1;
                                        }
                                      }
                                    }

  if (what == PDI_Description)      return (char*)p;
  if (m_wContents & PDI_Description)  p=SkipAStr(p, m_DscRL);
  
  if (what == PDI_TagComment)       return (char*)p;
  if (m_wContents & PDI_TagComment)   p=SkipAStr(p, m_ComRL);
   
  if (what == PDI_SubClass)           return (char*)p;
  if (m_wContents & PDI_SubClass)       p+=strlen(p)+1;

  if (what == PDI_PrimaryCfg)           return (char*)p;
  if (m_wContents & PDI_PrimaryCfg)       p+=strlen(p)+1;

  //if (what == PDI_Blob) return (char*)p;
  //if (m_wContents & PDI_Blob)   { ASSERT(0); word l=*((word*)p); p+=sizeof(word)+l+1; }
  
  VERIFY(0);
  return NULL;
  }
**/
//------------------------------------------------------------------------

byte CPkDataItem::Type()
  {
  if ((m_wContents & PDI_Value)==0) return tt_NULL;
  PkDataUnion *p=(PkDataUnion *)AddressOf(PDI_Value);
  return p->Type();
  };

//------------------------------------------------------------------------

PkDataUnion *CPkDataItem::Value()
  {
  if ((m_wContents & PDI_Value)==0) return NULL;
  PkDataUnion *p=(PkDataUnion *)AddressOf(PDI_Value);
  return p;
  };

//------------------------------------------------------------------------

char *CPkDataItem::Class()
  {
  if ((m_wContents & PDI_Value)==0) return NULL;
  PkDataUnion *p=(PkDataUnion *)AddressOf(PDI_Value);
  return p->String();
  };

//------------------------------------------------------------------------

char* CPkDataItem::SymStr()
  {
  if ((m_wContents & PDI_Tag)==0) return "";
  char*p=(char*)AddressOf(PDI_Tag);
  return ExtractAStr(p);
  //long i=0;
  //if (*p>=1 && *p<=4)
  //  {
  //  memcpy(&i, p+1, *p);
  //  p-=i;
  //  //dbgpln("SymStr='%s'", p);
  //  }
  //return p;
  };

//------------------------------------------------------------------------

char* CPkDataItem::TagStr()
  {
  if ((m_wContents & PDI_Tag)==0) return "";
  char*p=(char*)AddressOf(PDI_Tag);
  p=SkipAStr(p);
  return ExtractAStr(p);
  };

//------------------------------------------------------------------------

char* CPkDataItem::SymOrTag()
  {
  if ((m_wContents & PDI_Tag)==0) return "";


  LPSTR p=(LPSTR)AddressOf(PDI_Tag);
  if (*p==0)
    p+=strlen(p)+1; // Skip Zero len Sym
  return p;
  };

//------------------------------------------------------------------------

DDEF_Flags CPkDataItem::Flags()
  {
  if ((m_wContents & PDI_Flags)==0)
    return 0;
  if (m_wContents & PDI_Flags64) 
    {
    return *((DDEF_Flags64*)AddressOf(PDI_Flags));
    }
  else
    return *((DDEF_Flags32*)AddressOf(PDI_Flags));
  };

//------------------------------------------------------------------------

char* CPkDataItem::TagOfParent()
  {
  if ((m_wContents & PDI_TagOfParent)==0) return "";
  char*p=(char*)AddressOf(PDI_TagOfParent);
  return p;
  };

//------------------------------------------------------------------------

char* CPkDataItem::Description()
  {
  if ((m_wContents & PDI_Description)==0) return "";
  char*p=(char*)AddressOf(PDI_Description);
  return ExtractAStr(p);
  };

//------------------------------------------------------------------------

char* CPkDataItem::TagComment()
  {
  if ((m_wContents & PDI_TagComment)==0) return "";
  char*p=(char*)AddressOf(PDI_TagComment);
  return ExtractAStr(p);
  };

//------------------------------------------------------------------------

char* CPkDataItem::SubClass()
  {
  if ((m_wContents & PDI_SubClass)==0) return "";
  char*p=(char*)AddressOf(PDI_SubClass);
  return p;
  };

//------------------------------------------------------------------------

char* CPkDataItem::PrimaryCfg()
  {
  if ((m_wContents & PDI_PrimaryCfg)==0) return "";
  char*p=(char*)AddressOf(PDI_PrimaryCfg);
  return p;
  };

//------------------------------------------------------------------------

DDEF_NElements CPkDataItem::NElements()
  {
  if ((m_wContents & PDI_NElements)==0) return 0;
  DDEF_NElements*p=(DDEF_NElements*)AddressOf(PDI_NElements);
  return *p;
  };

//------------------------------------------------------------------------

CTagRefStatus CPkDataItem::GetTagRefStatus()
  {
  if ((m_wContents & PDI_XRefStatus)==0) 
    return 0;
  
  char*p=(char*)AddressOf(PDI_XRefStatus);
  CTagRefStatus Status=*((CTagRefStatus*)p);

  return Status;
  };

//------------------------------------------------------------------------

#if WITHEQUIPSPECS

CTagValueStatus CPkDataItem::GetTagValueStatus()
  {
  if ((m_wContents & PDI_XRefStatus)==0) 
    return 0;
  
  char*p=(char*)AddressOf(PDI_XRefStatus);
  p+=sizeof(CTagRefStatus);

  CTagValueStatus Status=*((CTagValueStatus*)p);

  return Status;
  };
#endif

//------------------------------------------------------------------------

flag CPkDataItem::GetRange(DataUnion &DMin, DataUnion &DMax)
  {
  if ((m_wContents & PDI_Range)==0) return 0;
  
  pPkDataUnion pDU;
  char*p=(char*)AddressOf(PDI_Range);
  pDU=(pPkDataUnion)p;
  DMin.Set(*pDU);

  p+=pDU->Size();
  pDU=(pPkDataUnion)p;
  DMax.Set(*pDU);

  return 1;
  };

//------------------------------------------------------------------------

flag CPkDataItem::GetRange(pPkDataUnion &pDMin, pPkDataUnion &pDMax)
  {
  if ((m_wContents & PDI_Range)==0) 
    {
    pDMin=NULL;
    pDMax=NULL;
    return 0;
    }
  
  char*p=(char*)AddressOf(PDI_Range);
  pDMin=(pPkDataUnion)p;
  p+=pDMin->Size();
  pDMax=(pPkDataUnion)p;

  return 1;
  };

//------------------------------------------------------------------------

CCnvIndex CPkDataItem::CnvIndex()
  {
  if ((m_wContents & PDI_CnvInfo)==0) return 0;
  CCnvIndex* p=(CCnvIndex*)AddressOf(PDI_CnvInfo);
  return  *p;
  };

//------------------------------------------------------------------------

char* CPkDataItem::CnvTxt()
  {
  if ((m_wContents & PDI_CnvInfo)==0) return 0;
  char*p=(((char*)AddressOf(PDI_CnvInfo))+sizeof(CCnvIndex));
  return p;
  };

//------------------------------------------------------------------------

char* CPkDataItem::CnvFam()
  {
  if ((m_wContents & PDI_CnvInfo)==0) return 0;
  char*p=(((char*)AddressOf(PDI_CnvInfo))+sizeof(CCnvIndex));
  p+=strlen(p)+1;
  return p;
  };

//------------------------------------------------------------------------

flag CPkDataItem::IndexedStrList()
  {
  if ((m_wContents & PDI_StrList)==0) return False;
  char*p=(char*)AddressOf(PDI_StrList);
  word n=*((word*)p);
  p+=sizeof(word);
  return *((flag*)p);
  };

//------------------------------------------------------------------------

flag CPkDataItem::GetStrList(Strng_List & Values, flag & Indexed)
  {
  Values.Clear();
  if ((m_wContents & PDI_StrList)==0) return False;
  char*p=(char*)AddressOf(PDI_StrList);
  word n=*((word*)p);
  p+=sizeof(word);
  Indexed=*((flag*)p);
  p+=sizeof(flag);
  for(word i=0; i<n; i++)
    {
    long Inx=i;
    if (Indexed)
      {
      Inx=*((long*)p);
      p+=sizeof(long);
      }
    pStrng pS=new Strng(p);
    pS->SetIndex(Inx);
    Values.Append(pS);
    p+=pS->Length()+1;
    }
  return True;
  };

//------------------------------------------------------------------------

void CPkDataItem::AppendLayout(byte iType, pchar pTag, DDEF_Flags iFlags)
  {
  Clear();//pInfo);
  PkDataUnion PData;
  PData.iType=iType;
  PData.StringSet(pTag);
  AppendValue(iFlags, PData);
  //pendDefFlags(iFlags);
  };

//------------------------------------------------------------------------

void CPkDataItem::AppendStructureS(byte iType, pchar pTag, pchar pClass, pchar pSubClass, pchar pPrimaryCfg, DDEF_Flags iFlags, pchar pTagOfParent, pchar pDescription, pchar pTagComment, int nElem)
  {
  Clear();//pInfo);
  PkDataUnion PData;
  PData.iType=iType;
  PData.StringSet(pClass);
  AppendValue(iFlags, PData);
  AppendTag(pTag, "");
  //AppendDefFlags(iFlags);
  if (pTagOfParent && (strlen(pTagOfParent)>0))
    AppendTagOfParent(pTagOfParent);
  if (iType==tt_Array)
    AppendNElements(nElem);
  if (pDescription && (strlen(pDescription)>0))
    AppendDescription(pDescription);
  if (pTagComment && (strlen(pTagComment)>0))
    AppendTagComment(pTagComment);
  if (pSubClass && (strlen(pSubClass)>0))
    AppendSubClass(pSubClass);
  if (pPrimaryCfg && (strlen(pPrimaryCfg)>0))
    AppendPrimaryCfg(pPrimaryCfg);
  };

//------------------------------------------------------------------------

void CPkDataItem::AppendStructureE(byte iType, pchar pTag, DDEF_Flags iFlags)
  {
  Clear();//pInfo);;
  PkDataUnion PData;
  PData.iType=iType;
  PData.StringSet("");
  AppendValue(iFlags, PData);
  if (pTag && (strlen(pTag)>0))
    AppendTag(pTag, "");
  };

//------------------------------------------------------------------------

void CPkDataItem::AppendDataValue(TABOptions Opts, pchar pTag, pchar pSym, DDEF_Flags iFlags, PkDataUnion &PData, CCnvIndex iCnv, char *pCnvTxt, char *pCnvFam)
  {
  Clear();//pInfo);;
  AppendValue(iFlags, PData);
  if ((Opts & (TABOpt_ForScenario|TABOpt_ForSnapShot|TABOpt_AllInfo)) && 
      ((pTag!=NULL && pTag[0]>0) || (pSym!=NULL && pSym[0]>0)))
    AppendTag(pTag, pSym);
  //AppendDefFlags(iFlags);
  if ((Opts & (TABOpt_ValCnvs|TABOpt_AllInfo)) && ((iCnv)>0) && pCnvTxt && (strlen(pCnvTxt)>0))
    AppendCnvInfo(iCnv, pCnvTxt, pCnvFam);
  }; 

//------------------------------------------------------------------------

void CPkDataItem::AppendBlob(pchar pTag, void * pBlob, short iBlobLen)
  {
  Clear();//pInfo);;
  //AppendTag(pTag);
  PkDataUnion PData;
  PData.iType=tt_Blob;
  PData.BlobSet(pBlob, iBlobLen);
  AppendValue(0, PData);
  AppendTag(pTag, "");
  }; 

//------------------------------------------------------------------------

void CPkDataItem::AppendValueLst(DDBValueLst *pValueLst, flag Indexed)
  {
  if (pValueLst && pValueLst->m_pStr)
    {
    char * pStart=CurrentPtr();
    pword pStrLstLen=NULL;
    for (  ; pValueLst->m_pStr; pValueLst++)
      if ((pValueLst->m_dwFlags & MDD_Hidden)==0)
        AppendStrListItem(pValueLst->m_pStr, pValueLst->m_lVal, Indexed, pStrLstLen);
    
    }
  }

//========================================================================
//
//
//
//========================================================================

CPkDataList::CPkDataList() : 
  m_WorkItem(*(new CPkDataItem4224))  
  { 
  Clear(); 
  };

CPkDataList::~CPkDataList()
  { 
  delete &m_WorkItem; 
  };

inline void CPkDataList::AdvanceWrite()//CPkDataItem* &pPItem) 
  { 
  //Do New etc
  ASSERT(m_WorkItem.ItemSize()<BiggestBuffer);
  CPkDataItem* pItem=CPkDataItem::Create(m_WorkItem);
  AddTail(pItem);
  };

//------------------------------------------------------------------------

void CPkDataList::SetStructureS(byte iType, pchar pTag, pchar pClass, pchar pSubClass, pchar pPrimaryCfg, DDEF_Flags iFlags, pchar pTagOfParent, pchar pDescription, pchar pTagComment, int nElem)
  {
  Item.Clear();
  PkDataUnion PData;
  PData.iType=iType;
  PData.StringSet(pClass);
  Item.AppendValue(iFlags, PData);
  Item.AppendTag(pTag, "");
  if (pTagOfParent && (strlen(pTagOfParent)>0))
    Item.AppendTagOfParent(pTagOfParent);
  if (iType==tt_Array)
    Item.AppendNElements(nElem);
  if (pDescription && (strlen(pDescription)>0))
    Item.AppendDescription(pDescription);
  if (pTagComment && (strlen(pTagComment)>0))
    Item.AppendTagComment(pTagComment);
  if (pSubClass && (strlen(pSubClass)>0))
    Item.AppendSubClass(pSubClass);
  if (pPrimaryCfg && (strlen(pPrimaryCfg)>0))
    Item.AppendPrimaryCfg(pPrimaryCfg);
  AdvanceWrite();//pPItem);
  };

//------------------------------------------------------------------------

void CPkDataList::SetStructureE(byte iType, pchar pTag, DDEF_Flags iFlags)
  {
  Item.Clear();
  PkDataUnion PData;
  PData.iType=iType;
  PData.StringSet("");
  Item.AppendValue(iFlags, PData);
  if (pTag && (strlen(pTag)>0))
    Item.AppendTag(pTag, "");
  AdvanceWrite();
  };

//------------------------------------------------------------------------

void CPkDataList::SetDataValue(pchar pTag, DDEF_Flags iFlags, PkDataUnion &PData)
  {
  Item.Clear();
  Item.AppendValue(iFlags, PData);
  if (pTag!=NULL && pTag[0]>0)
    Item.AppendTag(pTag, "");
  //AppendDefFlags(iFlags);
  AdvanceWrite();
  }; 

//------------------------------------------------------------------------

void CPkDataList::SetDataValueAll(pchar pTag, pchar pTagComment, PkDataUnion &PData, DDEF_Flags iFlags,
                                   DataUnion &DataMin, DataUnion &DataMax, CCnvIndex iCnv, pchar pCnvTxt, pchar pCnvFam, 
                                   pStrng_List pValueList, flag bIndexedStr, pchar pDesc)
  {
  Item.Clear();
  Item.AppendValue(iFlags, PData);
  Item.AppendTag(pTag, "");
  if ((iCnv)>0 && strlen(pCnvTxt) > 0)
    Item.AppendCnvInfo(iCnv, pCnvTxt, pCnvFam);
  if (DataMin.iType!=tt_NULL)
    Item.AppendRange(DataMin, DataMax);
  if (pValueList)
    {
    word *pStrLstLen=NULL;
    pStrng pS=pValueList->First();
    for (int i=0; pS; i++, pS=pS->Next())
      Item.AppendStrListItem(pS->Str(), pS->Index(), bIndexedStr, pStrLstLen);
    }
  if (pDesc && (strlen(pDesc)>0))
    Item.AppendDescription(pDesc);
  if (pTagComment && (strlen(pTagComment)>0))
    Item.AppendTagComment(pTagComment);
  AdvanceWrite();
  }; 

//------------------------------------------------------------------------

void CPkDataList::dbgDump(flag Full, char* pWhere)
  {
  dbgp("CPkDataList[] : %s", /*Size()*//*wSize*/ pWhere);
  flag MultiLine=0;
  
  CPkDataIter Iter;
  CPkDataItem * pFirstItem=FirstItem(Iter);
  if (pFirstItem)
    {
    CPkDataItem* pPItem=pFirstItem; 
    MultiLine=(!IsData(pPItem->Type()) && Full);
    pPItem=NextItem(Iter);//AdvanceReadItem();
    if (!pPItem)
      MultiLine=0;
    }
  if (MultiLine) 
    dbgpln("");
  int n=0;
  int iItem=0;
  int CompactStart=0;
  int CompactStartZ=0;
  int Size=0;
  for (CPkDataItem* pPItem=FirstItem(Iter); pPItem && (Full || n++<10); pPItem=NextItem(Iter)) //AdvanceReadItem())
    {
    if (Full)
      {
      char * pC=(char*)pPItem;
      word C=0, Cm=1;
      for (int ii=0; ii<16; ii++, Cm=Cm<<1)
        if (pPItem->Contains(Cm))
          C|=Cm; 
      dbgp("%5i)", iItem++);
      dbgp("%3i:", pPItem->DataSize());
      //dbgp("%5i[%3i]:", (char*)pPItem-(char*)pFirstItem, pPItem->Size());
      dbgp("%04x ", C);

      dbgp("%08x ", pPItem->Buffer());

      if (0)
        {
        dbgp("%5i", CompactStart);
        dbgp("|%5i", CompactStartZ);
        dbgp(pPItem->Contains(PDI_Value) ?"(%3i) ":"(   )",  pPItem->Value()->Size());

        // Contents Data XRef Index

        int DataSz=0;
        bool IsZero=false;
        if (pPItem->Contains(PDI_Value      ))
          {
          DataSz=pPItem->Value()->Size();
          switch (pPItem->Value()->Type())
            {
            case tt_Double: IsZero = pPItem->Value()->GetDouble()==0; break;
            }
          }
        dbgp(IsZero?"=0 ":"   ");
        CompactStart+=2+DataSz+1+4;
        CompactStartZ+=2+(IsZero?0:DataSz)+1+4;
        }

      dbgp("'", C);

      dbgp("%c", pPItem->Contains(PDI_Flags      ) ? 'F':' ');
      dbgp("%c", pPItem->Contains(PDI_Value      ) ? 'V':' ');
      dbgp("%c", pPItem->Contains(PDI_Tag        ) ? 'T':' ');
      dbgp("%c", pPItem->Contains(PDI_TagOfParent) ? 'P':' ');
      dbgp("%c", pPItem->Contains(PDI_CnvInfo    ) ? 'C':' ');
      dbgp("%c", pPItem->Contains(PDI_NElements  ) ? 'N':' ');
      dbgp("%c", pPItem->Contains(PDI_CnvInfo2   ) ? '2':' ');
      dbgp("%c", pPItem->Contains(PDI_XRefStatus ) ? 'X':' ');
      dbgp("%c", pPItem->Contains(PDI_Range      ) ? 'R':' ');
      dbgp("%c", pPItem->Contains(PDI_StrList    ) ? 'L':' ');
      dbgp("%c", pPItem->Contains(PDI_Description) ? 'D':' ');
      dbgp("%c", pPItem->Contains(PDI_TagComment ) ? 'c':' ');
      dbgp("%c", pPItem->Contains(PDI_SubClass   ) ? 'S':' ');
      dbgp("%c", pPItem->Contains(PDI_PrimaryCfg ) ? 'g':' ');
      //dbgp("%c", pPItem->Contains(PDI_Flags64    ) ? '6':' ');
      
      dbgp("' %4i%4i:", pPItem->DataSize(), pPItem->Value()->TypeSize());

      }
    
    if (pPItem->Empty())
      {
      if (Full)
        dbgpln("");
      break;
      }

    if (!MultiLine && pPItem!=pFirstItem) 
      dbgp(", ");
    dbgp(Full ? "  %-8.8s:" : "%s",tt_TypeString(pPItem->Type()));

    if (pPItem->Contains(PDI_Flags))
      {
      DDEF_Flags F=pPItem->Flags();
      /**/
      Strng S;
    
      if ((F&DDEF_PARAM           )!=0) S+="P";

      if ((F&DDEF_NOFILE          )==0) S+="f";
      if ((F&DDEF_NOVIEW          )==0) S+="v";
      if ((F&DDEF_NOSNAPSHOT      )==0) S+="s";
      if ((F&DDEF_NOSCENARIO      )==0) S+="c";
                           
      if ((F&DDEF_WRITEPROTECT    )!=0) S+="W";
      if ((F&DDEF_HIDDEN          )!=0) S+="H";
      if ((F&DDEF_AFFECTSSTRUCT   )!=0) S+="S";
      if ((F&DDEF_KEEPHISTORY     )!=0) S+="K";
      if ((F&DDEF_TREENODE        )!=0) S+="n";
      if ((F&DDEF_NAN_OK          )!=0) S+="N";
      if ((F&DDEF_BUTTON          )!=0) S+="B";
      if ((F&DDEF_CHECKBOX        )!=0) S+="C";
      if ((F&DDEF_DUPHANDLES_OK   )!=0) S+="x";
      if ((F&DDEF_MULTILINESTR    )!=0) S+="M";
      if ((F&DDEF_PARAMSTOPPED    )!=0) S+="p";
      if ((F&DDEF_INERROR         )!=0) S+="E";
      if ((F&DDEF_DRIVERTAG       )!=0) S+="D";
      if ((F&DDEF_HIDEIFZERO      )!=0) S+="Z";
      if ((F&DDEF_CHILDMASK       )!=0) S+="+";
      //if ((F&DDEF_STARTMATRIX     )!=0) S+="#";
      //if ((F&DDEF_STARTROW        )!=0) S+="[";
      //if ((F&DDEF_ENDMATRIX       )!=0) S+="]";

      dbgp(" <%-8s>",S.Buffer());
      /**/
     
      //dbgp(" <%8x>",F);
      }
    else
      dbgp(" <        >");

    dbgp(pPItem->Value()->IsZero() ? "Z":" ");

#if WITHEQUIPSPECS
    if (pPItem->Contains(PDI_XRefStatus))
      dbgp("{%02x.%02x}", pPItem->GetTagRefStatus(), pPItem->GetTagValueStatus());
    else
      dbgp("       ");
#else
    if (pPItem->Contains(PDI_XRefStatus))
      dbgp("{%02x}", pPItem->GetTagRefStatus());
    else
      dbgp("    ");
#endif

    if (pPItem->Contains(PDI_Tag))
      if (strlen(pPItem->SymStr())>0)
        dbgp(" '%s' ('%s')",pPItem->SymOrTag(), pPItem->TagStr());
      else
        dbgp(" '%s'",pPItem->TagStr());

    if (pPItem->Contains(PDI_TagOfParent))
      dbgp(" Parent:%s",pPItem->TagOfParent());

    if (pPItem->Contains(PDI_Value))
      if (pPItem->Type()!=tt_Blob)
        dbgp(" = '%s'", pPItem->Value()->GetString());

    if (pPItem->Contains(PDI_SubClass))
      dbgp(" [%s]",pPItem->SubClass());

    if (pPItem->Contains(PDI_PrimaryCfg))
      dbgp(" /%s/",pPItem->PrimaryCfg());

    if (pPItem->Contains(PDI_CnvInfo))
      {
      dbgp(" Cnv:'%s",pPItem->CnvTxt());
      dbgp(pPItem->CnvFam() && strlen(pPItem->CnvFam())>0 ? "(%s)":"",pPItem->CnvFam());
      dbgp("'[%i]",pPItem->CnvIndex());
      }

    if (pPItem->Contains(PDI_NElements))
      dbgp("  [%i]",pPItem->NElements());


    if (pPItem->Contains(PDI_Range))
      dbgp(" Range");

    if (pPItem->Contains(PDI_StrList))
      {
      flag Inxd=pPItem->IndexedStrList();
      dbgp(" StrList[");
      //PkValueLst ValueList;
      //pPItem->GetStrList(ValueList);
      //for (pStrng pS=ValueList.Values.First(); pS; pS=pS->Next())
      //  {
      //  if (pS!=ValueList.Values.First())
      //    dbgp(",");
      //  if (Inxd)
      //    dbgp(" %i %s",pS->Index(), pS->Str());
      //  else
      //    dbgp(" %s",pS->Str());
      //  }
      Strng_List ValueList;
      pPItem->GetStrList(ValueList);
      for (pStrng pS=ValueList.First(); pS; pS=pS->Next())
        {
        if (pS!=ValueList.First())
          dbgp(",");
        if (Inxd)
          dbgp(" %i %s",pS->Index(), pS->Str());
        else
          dbgp(" %s",pS->Str());
        }
      dbgp("]");
      }

    if (pPItem->Contains(PDI_Description))
      dbgp(" \"%s\"",pPItem->Description());

    if (pPItem->Contains(PDI_TagComment))
      dbgp(" (%s)",pPItem->TagComment());
      //dbgp(" TagComment:%s",pPItem->TagComment());

    if (MultiLine) 
      dbgpln("");
    }
  if (!MultiLine) 
    dbgpln("");
  };

//=========================================================================
//
//
//
//=========================================================================

MTagIOValue::MTagIOValue()                 
  { 
  m_TIOType = MTagType_Null;   
  };
MTagIOValue::MTagIOValue(long L)
  { 
  m_TIOType = MTagType_Long;    
  m_Long = L; 
  };
MTagIOValue::MTagIOValue(double D)
  { 
  m_TIOType = MTagType_Double;  
  m_Double = D; 
  };
MTagIOValue::MTagIOValue(LPCSTR S)
  { 
  m_TIOType = MTagType_String;  
  m_String = S; 
  };

MTagIOValue::MTagIOValue(const MTagIOValue & V)
  {
  m_TIOType  = V.m_TIOType;
  switch (m_TIOType)
    {
    case MTagType_Long: m_Long      = V.m_Long;   break;
    case MTagType_Double: m_Double  = V.m_Double; break;
    case MTagType_String: m_String  = V.m_String; break;
    }
  };

MTagIOValue::~MTagIOValue()                 { };

MTagIOValue & MTagIOValue::operator=(const MTagIOValue & V)
  {
  m_TIOType  = V.m_TIOType;
  switch (m_TIOType)
    {
    case MTagType_Long:   m_Long    = V.m_Long;   break;
    case MTagType_Double: m_Double  = V.m_Double; break;
    case MTagType_String: m_String  = V.m_String; break;
    }
  return *this;
  };
MTagIOValue & MTagIOValue::operator=(const PkDataUnion & V)
  {
  m_TIOType  = V.TagType();
  switch (m_TIOType)
    {
    case MTagType_Long:   m_Long    = V.GetLong();   break;
    case MTagType_Double: m_Double  = V.GetDouble(); break;
    case MTagType_String: m_String  = V.GetString(); break;
    }
  return *this;
  };
bool MTagIOValue::operator==(const MTagIOValue & V)
  {
  if (m_TIOType  != V.m_TIOType)
    return false;
  switch (m_TIOType)
    {
    case MTagType_Long:   return m_Long   == V.m_Long;   
    case MTagType_Double: 
      {
      bool Valid1=Valid(m_Double);
      bool Valid2=Valid(V.m_Double);
      return (Valid1 && Valid2 && (m_Double==V.m_Double)) || (Valid1==Valid2);
      }
    case MTagType_String: return m_String == V.m_String; 
    }
  return false;
  };
bool MTagIOValue::operator!=(const MTagIOValue & V)
  {
  if (m_TIOType  != V.m_TIOType)
    return true;
  switch (m_TIOType)
    {
    case MTagType_Long:   return m_Long   != V.m_Long;   
    case MTagType_Double:
      {
      bool Valid1=Valid(m_Double);
      bool Valid2=Valid(V.m_Double);
      return (Valid1 && Valid2 && (m_Double!=V.m_Double)) || (Valid1!=Valid2);
      }
    case MTagType_String: return m_String != V.m_String; 
    }
  return true;
  };

MTagIOType   MTagIOValue::getTIOType() const        { return m_TIOType; };
void         MTagIOValue::putTIOType(MTagIOType T)  { m_TIOType=T; };
long         MTagIOValue::getLong() const           
  { 
  switch (m_TIOType)
    {
    case MTagType_Long: return m_Long; 
    case MTagType_Double: return (long)Range(double(LONG_MIN), m_Double, double(LONG_MAX)); 
    case MTagType_String: return SafeAtoL(m_String, 0); 
    };
  return 0;
  }
void         MTagIOValue::putLong(long L)
  {
  m_TIOType=MTagType_Long;
  m_Long=L;
  };
double       MTagIOValue::getDoubleSI() const 
  { 
  double d=0.0;
  switch (m_TIOType)
    {
    case MTagType_Long:   d=m_Long;                break;
    case MTagType_Double: d=m_Double;              break;
    case MTagType_String: d=SafeAtoF(m_String, 0); break;
    };
  return d;
  }
void         MTagIOValue::putDoubleSI(double D)       
  { 
  m_TIOType=MTagType_Double; 
  m_Double=D; 
  };
double       MTagIOValue::getDoubleCnv(const MCnv * pCnv) const 
  { 
  double d=0.0;
  switch (m_TIOType)
    {
    case MTagType_Long:   d=m_Long;                break;
    case MTagType_Double: d=m_Double;              break;
    case MTagType_String: d=SafeAtoF(m_String, 0); break;
    };
  
  if (pCnv)
    d= Cnvs[pCnv->Index]->Human(d, (LPSTR)pCnv->Text); 
  
  return d;
  }
void         MTagIOValue::putDoubleCnv(const MCnv * pCnv, double D)       
  { 
  if (pCnv)
    D= Cnvs[pCnv->Index]->Normal(D, (LPSTR)pCnv->Text); 
  
  m_TIOType=MTagType_Double; 
  m_Double=D; 
  };
LPCSTR       MTagIOValue::getString() const         
  { 
  switch (m_TIOType)
    {
    case MTagType_Long: 
      //return m_Long; 
    case MTagType_Double: //return m_Double; 
      _asm int 3;
      return "???";
    case MTagType_String: return m_String; 
    };
  return 0;
  }
void         MTagIOValue::putString(LPCSTR S)
  {
  m_TIOType=MTagType_String;
  m_String=S;
  };

void MTagIOValue::Reset()
  {
  switch (m_TIOType)
    {
    case MTagType_Long:   m_Long    = 0;   break;
    case MTagType_Double: m_Double  = 0.0; break;
    case MTagType_String: m_String  = "";  break;
    }
  };
CString MTagIOValue::AsString() const       
  { 
  CString S;
  switch (m_TIOType)
    {
    case MTagType_Long:   S.Format("%i", m_Long);     break;   
    case MTagType_Double: S.Format("%.5f", m_Double); break;
    case MTagType_String: S=m_String;                 break;
    }
  return S; 
  };

//=========================================================================
//
//                                                          //
//=========================================================================

