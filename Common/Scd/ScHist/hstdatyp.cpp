//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"

#if !ForSysCAD
#include <math.h>
#include <float.h>
#define  __HSTDATYP_CPP
#include "hstdatyp.h"
#include "hstvect.h"

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
  Mant=((unsigned short)floor(m*2048.0));
  return *this;
  };

//------------------------------------------------------------------------

flt16::operator double() 
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
  flt16 f;
  int kk;
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
  
    }
  dbgpln("Mean     Error over %i values = %g%%", nn, 100.0*MeanError/nn);
  dbgpln("Mean Abs Error over %i values = %g%%", nn, 100.0*MeanAbsError/nn);
  f=MaxX; 
  dbgpln("Max      Error @ %14g > %14g  %14g%%", MaxX, f, 100.0*(f-MaxX)/NZ(MaxX));
  }

//===========================================================================
//
//
//
//===========================================================================

unsigned short DataUnionCom::Size()
  {
  switch (iType)
    {
    case tt_Char     : return sizeof(char)+sizeof(byte);
    case tt_Bool     :     
    case tt_Bit      : 
    case tt_Byte     : return sizeof(byte)+sizeof(byte);
    case tt_Word     : return sizeof(unsigned short)+sizeof(byte);
    case tt_DWord    : return sizeof(unsigned long)+sizeof(byte);
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

unsigned short DataUnionCom::TypeSize()
  {
  switch (iType)
    {
    case tt_Bool     :     
    case tt_Bit      : 
    case tt_Byte     : return sizeof(byte);
    case tt_Word     : return sizeof(unsigned short);
    case tt_DWord    : return sizeof(unsigned long);
    case tt_Int      : return sizeof(int);
    case tt_Char     : return sizeof(char);
    case tt_Short    : return sizeof(short);
    case tt_Long     : return sizeof(long);
    case tt_Flt16    : return sizeof(flt16);
    case tt_Float    : return sizeof(float);
    case tt_Double   : return sizeof(double);
    };
  VERIFY(0);
  return 0;
  };

//------------------------------------------------------------------------

flag DataUnionCom::IsNAN()
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

////------------------------------------------------------------------------
//
//flag DataUnion::Equal(DataUnion &V)
//  {
//  return DataUnionCom::Equal(V);
//  }

//------------------------------------------------------------------------

long DataUnionCom::GetLong(int iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  switch (iType)
    {
    case tt_Char   : return Char;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : return Byte;
    case tt_Word   : return Word;
    case tt_DWord  : return DWord;
    case tt_Int    : return Int;
    case tt_Short  : return Short;
    case tt_Long   : return Long;
    //case tt_Flt16  : return (long)Cnvs[iCnv]->Human(Flt16, pCnvTxt);
    //case tt_Float  : return (long)Cnvs[iCnv]->Human(Float, pCnvTxt);
    //case tt_Double : return (long)Cnvs[iCnv]->Human(Double, pCnvTxt);
    case tt_Flt16  : return (long)Flt16;
    case tt_Float  : return (long)Float;
    case tt_Double : return (long)Double;
    default :        VERIFY(0); return 0;
    }
  }

//------------------------------------------------------------------------

void DataUnionCom::PutLong(long L, int iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  iStatus=ds_OK;
  switch (iType)
    {
    case tt_Char   : Char=(char)L; break;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : Byte=(byte)L; break;
    case tt_Word   : Word=(unsigned short)L; break;
    case tt_DWord  : DWord=(unsigned long)L; break;
    case tt_Int    : Int=(int)L; break;
    case tt_Short  : Short=(short)L; break;
    case tt_Long   : Long=(long)L; break;
    //case tt_Flt16  : Flt16=Cnvs[iCnv]->Normal(L, pCnvTxt); break;
    //case tt_Float  : Float=(float)Cnvs[iCnv]->Normal(L, pCnvTxt); break;
    //case tt_Double : Double=Cnvs[iCnv]->Normal(L, pCnvTxt); break;
    case tt_Flt16  : Flt16=(double)L; break;
    case tt_Float  : Float=(float)L; break;
    case tt_Double : Double=(double)L; break;
    default :        VERIFY(0); break;
    }
  };

//------------------------------------------------------------------------

double DataUnionCom::GetDouble(int iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  switch (iType)
    {
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
    //case tt_Flt16  : return Cnvs[iCnv]->Human(Flt16, pCnvTxt);
    //case tt_Float  : return Cnvs[iCnv]->Human(Float, pCnvTxt);
    //case tt_Double : return Cnvs[iCnv]->Human(Double, pCnvTxt);
    default :        VERIFY(0); return dNAN;
    }
  }

//------------------------------------------------------------------------

void DataUnionCom::PutDouble(double D, int iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  iStatus=ds_OK;
  switch (iType)
    {
    case tt_Char   : Char=(char)D; break;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : Byte=(byte)D; break;
    case tt_Word   : Word=(unsigned short)D; break;
    case tt_DWord  : DWord=(unsigned long)D; break;
    case tt_Int    : Int=(int)D; break;
    case tt_Short  : Short=(short)D; break;
    case tt_Long   : Long=(long)D; break;
    //case tt_Flt16  : Flt16=Cnvs[iCnv]->Normal(D, pCnvTxt); break;
    //case tt_Float  : Float=(float)Cnvs[iCnv]->Normal(D, pCnvTxt); break;
    //case tt_Double : Double=Cnvs[iCnv]->Normal(D, pCnvTxt); break;
    case tt_Flt16  : Flt16=D; break;
    case tt_Float  : Float=(float)D; break;
    case tt_Double : Double=D; break;
    default :        VERIFY(0); break;
    }
  };

//------------------------------------------------------------------------

char GetStringBuffer[2048];
int GetStringBufferPos=0;

char* DataUnionCom::GetString(char*IFmt/*="%i"*/, char*FFmt/*="%g"*/, int iCnv/*=0*/, char* pCnvTxt/*=NULL*/)
  {
  char*p = &GetStringBuffer[GetStringBufferPos];
  switch (iType)
    {
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
                       //sprintf(p, FFmt, Cnvs[iCnv]->Human(Flt16, pCnvTxt)); 
                       sprintf(p, FFmt, Flt16); 
                     else 
                       strcpy(p, "*"); 
                     break;
    case tt_Float  : if (Valid(Float)) 
                       //sprintf(p, FFmt, Cnvs[iCnv]->Human(Float, pCnvTxt)); 
                       sprintf(p, FFmt, Float); 
                     else 
                       strcpy(p, "*"); 
                     break;
    case tt_Double : if (Valid(Double)) 
                       //sprintf(p, FFmt, Cnvs[iCnv]->Human(Double, pCnvTxt)); 
                       sprintf(p, FFmt, Double); 
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

flag DataUnionCom::SetTypeString(byte iTypeRqd, char*pStrData, int iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  iStatus=ds_OK;
  switch (iTypeRqd)
    {
    case tt_Char   : Char=(char)atol(pStrData);  break;
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : Byte=(byte)atol(pStrData);  break;
    case tt_Word   : Word=(unsigned short)atol(pStrData);  break;
    case tt_DWord  : DWord=(unsigned long)atol(pStrData);  break;
    case tt_Int    : Int=(int)atol(pStrData);   break;
    case tt_Short  : Short=(short)atol(pStrData); break;
    case tt_Long   : Long=(long)atol(pStrData);  break;
    case tt_Flt16  : 
      while (*pStrData==' ') pStrData++;
      if (*pStrData=='*') 
        SetStatus(ds_NAN);
      else
        //Flt16=Cnvs[iCnv]->Normal(atof(pStrData), pCnvTxt); 
        Flt16=atof(pStrData); 
      break;
    case tt_Float  : 
      while (*pStrData==' ') pStrData++;
      if (*pStrData=='*') 
        Float=fNAN;
      else
        //Float=(float)Cnvs[iCnv]->Normal(atof(pStrData), pCnvTxt); 
        Float=(float)atof(pStrData); 
      break;
    case tt_Double : 
      while (*pStrData==' ') pStrData++;
      if (*pStrData=='*') 
        Double=dNAN;
      else
        //Double=Cnvs[iCnv]->Normal(atof(pStrData), pCnvTxt); 
        Double=atof(pStrData); 
      break;
    default        : VERIFY(0); break;
    }
  iType=iTypeRqd;
  return true;
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
    case tt_Bool   : 
    case tt_Bit    : 
    case tt_Byte   : *((byte*)p) = Byte; break;
    case tt_Word   : *((unsigned short*)p) = Word; break;
    case tt_DWord  : *((unsigned long*)p) = DWord; break;
    case tt_Int    : *((int*)p) = Int; break;
    case tt_Char   : *((char*)p) = Char; break;
    case tt_Short  : *((short*)p) =  Short; break;
    case tt_Long   : *((long*)p) =  Long; break;
    case tt_Flt16  : *((flt16*)p) = Flt16; break;
    case tt_Float  : *((float*)p) = Float; break;
    case tt_Double : *((double*)p) = Double; break;
    case tt_pChar  : strcpy((char*)p, pChar); break;
    default : ASSERT(0);
    }
  };

//========================================================================
//
//
//
//========================================================================

//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================

CXM_Header::CXM_Header(byte MsgId/*=XM_Null*/)
  {
  Id=MsgId;
  wLength=0;// Invalid Size - Must be set in derived classes
  }

//---------------------------------------------------------------------------

CXM_Header::CXM_Header(CXM_Header& Cpy) 
  { 
  memcpy(this, &Cpy, Cpy.Length());
  };

//---------------------------------------------------------------------------

CXM_Header& CXM_Header::operator=(CXM_Header& Cpy) 
  { 
  memcpy(this, &Cpy, Cpy.Length());
  return *this;
  };

//===========================================================================

CXM_Route::CXM_Route() { Clear(); };
CXM_Route::~CXM_Route() {};

//---------------------------------------------------------------------------

void CXM_Route::Clear() 
  { 
  /*wCurrentPos=*/wEnd=0; 
  bAcrossNetwork=0;
  nNodes=0; 

  Id=XM_Route;
  wLength=&cBuffer[wEnd]-(char*)this; 
  };

//---------------------------------------------------------------------------

flag CXM_Route::AddNodeFwd(long NodeObjId, char* NodeName)
  {
  Id=XM_Route;
  if (wEnd+sizeof(long)+strlen(NodeName)+1<CXM_RouteBuffMax) 
    {
    *((long*)(&cBuffer[wEnd])) = NodeObjId;
    wEnd+=sizeof(long);
    strcpy(&cBuffer[wEnd], NodeName);
    wEnd+=strlen(NodeName)+1;
    nNodes++;
    
    wLength=&cBuffer[wEnd]-(char*)this; 
    return true;
    }
  return false;
  };

//---------------------------------------------------------------------------

flag CXM_Route::AddNodeRev(long NodeObjId, char* NodeName)
  {
  Id=XM_Route;
  unsigned short l=sizeof(long)+strlen(NodeName)+1;
  if (wEnd+l<CXM_RouteBuffMax) 
    {
    memmove(&cBuffer[l], &cBuffer[0], wEnd);
    wEnd+=l;
    
    *((long*)(&cBuffer[0])) = NodeObjId;
    strcpy(&cBuffer[sizeof(long)], NodeName);
    nNodes++;
    wLength=&cBuffer[wEnd]-(char*)this; 
    return true;
    }
  return false;
  };

//---------------------------------------------------------------------------

flag CXM_Route::RemoveNode(int n)
  {
  unsigned short p=0;
  while (n-->0)
    p+=strlen(&cBuffer[p+sizeof(long)])+1;
  if (p<wEnd)
    {
    int l=sizeof(long) + strlen(&cBuffer[p+sizeof(long)])+1;
    memmove(&cBuffer[p], &cBuffer[p+l], wEnd-(p+l));
    nNodes--;
    return true;
    }
  return false;
  };

//---------------------------------------------------------------------------

long CXM_Route::NodeObjId(int n)
  {
  unsigned short p=0;
  while (n-->0)
    p+=sizeof(long)+strlen(&cBuffer[p+sizeof(long)])+1;
  return p<wEnd ? *((long*)(&cBuffer[p])) : -1;  
  }

//---------------------------------------------------------------------------

char* CXM_Route::NodeName(int n)
  {
  unsigned short p=0;
  while (n-->0)
    p+=sizeof(long)+strlen(&cBuffer[p+sizeof(long)])+1;
  return p<wEnd ? &cBuffer[p+sizeof(long)] : NULL;
  }

//---------------------------------------------------------------------------

long CXM_Route::NoNodes() { return nNodes; };

//---------------------------------------------------------------------------

flag CXM_Route::operator==(CXM_Route &Other)
  {
  if (wEnd==Other.wEnd && nNodes==Other.nNodes)
    return (memcmp(this, &Other, (char*)&cBuffer[wEnd]-(char*)this)==0);
  return false;
  };

//---------------------------------------------------------------------------

CXM_Route &CXM_Route::operator=(CXM_Route &Other)
  {
  Id=Other.Id;
  wLength=Other.wLength;
  wEnd=Other.wEnd;
  nNodes=Other.nNodes;
  bAcrossNetwork=Other.bAcrossNetwork;
  memcpy(cBuffer, Other.cBuffer, wEnd);

  return *this;
  };

//---------------------------------------------------------------------------

void CXM_Route::ReverseRoute(CXM_Route &Other)
  {
  Clear();
  for (int i=0; i<Other.NoNodes(); i++)
    {
    flag Ok=AddNodeRev(Other.NodeObjId(i),Other.NodeName(i));
    ASSERT(Ok);
    }
  Id=Other.Id;
  bAcrossNetwork=Other.bAcrossNetwork;
  };

//---------------------------------------------------------------------------

char* CXM_Route::ComputerName() 
  {
  static char CompName[MAX_COMPUTERNAME_LENGTH + 1 + 100]; 
  DWORD CompNameLen=sizeof(CompName);
  flag OK=GetComputerName(CompName, &CompNameLen); 
  ASSERT(OK);
  return CompName;
  }

//---------------------------------------------------------------------------

char* CXM_Route::MakeNodeName(char * Node, char * Where) 
  {
  strcpy(Where, ComputerName());
  strcat(Where, "(");
  strcat(Where, Node);
  strcat(Where, ")");
  return Where;
  }

//---------------------------------------------------------------------------

void CXM_Route::dbgDump(char * where/*=""*/) 
  {
  dbgp("Route [%i] {%s}", NoNodes(), where);
  for (int i=0; i<NoNodes(); i++)
    dbgp(" - %s[%i]",NodeName(i),NodeObjId(i));
  dbgpln("");
  };

//---------------------------------------------------------------------------
//===========================================================================

CXM_HistoryData::CXM_HistoryData(int TrndNo, long RqstNo, double Time, double Val, byte Status)
  {
  dTime=Time;
  dVal=Val;
  iStatus=Status;
  iTrndNo=TrndNo;
  iRqstNo=RqstNo;
  Id=XM_HistoryData;
  wLength=sizeof(*this);
  };

//===========================================================================

CXM_HistoryDataError::CXM_HistoryDataError(long ErrorNumber, long RqstNo, double TimeMissingData, char *ReqdFileName)
  {
  lErrorNumber     = ErrorNumber;    
  dTimeMissingData = TimeMissingData;
  iRqstNo          = RqstNo;         
  strcpy(cFileName, ReqdFileName);
  // CXM_Header
  Id=XM_HistoryDataError;
  wLength=(&cFileName[0]-(char*)this) +strlen(cFileName) + 1;
  }

//===========================================================================

flag CXM_QueryRowEx::AddValue(double Value)
  {
  const int Pos = wLength-((char*)(&cData[0])-(char*)this);
  if (Pos+2+sizeof(double)>=QueryRowExDataLen)
    return false;
  nPts++;
  cData[Pos] = QueryRowExType_Double; 
  *((double*)&cData[Pos+1]) = Value;
  wLength += (sizeof(double)+1);
  return true;
  };

//---------------------------------------------------------------------------

flag CXM_QueryRowEx::AddValue(long Value)
  {
  const int Pos = wLength-((char*)(&cData[0])-(char*)this);
  if (Pos+2+sizeof(long)>=QueryRowExDataLen)
    return false;
  nPts++;
  cData[Pos] = QueryRowExType_Long; 
  *((long*)&cData[Pos+1]) = Value;
  wLength += (sizeof(long)+1);
  return true;
  };

//---------------------------------------------------------------------------

flag CXM_QueryRowEx::AddValue(char* Value)
  {
  const int Pos = wLength-((char*)(&cData[0])-(char*)this);
  const int len = (Value ? strlen(Value) : 0);
  if (Pos+3+len>=QueryRowExDataLen)
    return false;
  nPts++;
  cData[Pos] = QueryRowExType_Str; 
  if (Value)
    strcpy(&cData[Pos+1], Value);
  else
    cData[Pos+1] = 0;
  wLength += (len + 2);
  return true;
  };

//---------------------------------------------------------------------------

byte CXM_QueryRowEx::FirstValTyp(int & Pos)
  {
  Pos = 0;
  if (nPts==0)
    return QueryRowExType_Null;
  return cData[Pos];
  };

//---------------------------------------------------------------------------

byte CXM_QueryRowEx::NextValTyp(int & Pos)
  {
  if (Pos>=(int)wLength-((char*)(&cData[0])-(char*)this))
    return QueryRowExType_Null;
  if (cData[Pos]==QueryRowExType_Double)
    Pos += sizeof(double);
  else if (cData[Pos]==QueryRowExType_Long)
    Pos += sizeof(long);
  else
    Pos += (strlen(&cData[Pos+1])+1);
  Pos++;
  return cData[Pos];
  };

//---------------------------------------------------------------------------

double CXM_QueryRowEx::DValue(int & Pos)
  {
  ASSERT(cData[Pos]==QueryRowExType_Double);
  return *((double*)(&cData[Pos+1]));
  };

//---------------------------------------------------------------------------

long CXM_QueryRowEx::LValue(int & Pos)
  {
  ASSERT(cData[Pos]==QueryRowExType_Long);
  return *((long*)(&cData[Pos+1]));
  };

//---------------------------------------------------------------------------

char* CXM_QueryRowEx::SValue(int & Pos)
  {
  ASSERT(cData[Pos]==QueryRowExType_Str);
  return &cData[Pos+1];
  };

//===========================================================================

flag CXMsgLst::PackMsg(rXB_Header Msg, flag KeepSpaceForRoute/*=True*/)
  {
  ASSERT(Msg.wLength>0); // Size Must be > 0
  long l=Msg.wLength;
  
  #if dbgSmallBuffer
  if (Number>=4)  // Limit to Max of 4 Messages per buffer
    return False;
  #endif

  if (WrtPos+l < (long)(sizeof(Data) - (KeepSpaceForRoute ? sizeof(CXM_Route): 0)))//MaxTABBuffLen)
    {
    memcpy(&Data[WrtPos], &Msg, Msg.wLength); // Append Message
    WrtPos+=Msg.wLength;
    Number++;
    return true;
    }
  else
    return false;
  };

//---------------------------------------------------------------------------

void * CXMsgLst::MsgPtr(byte RqdMsgId/*=XM_Null*/)
  {
  if (RdPos<WrtPos)
    {
    pXB_Header p=(pXB_Header)&Data[RdPos];
    RdPos+=p->wLength;
    if (RqdMsgId!=XM_Null)
      VERIFY(RqdMsgId==p->Id);
    return p;
    }
  return NULL;
  };

//---------------------------------------------------------------------------

flag CXMsgLst::MsgAvail(byte RqdMsgId/*=XM_Null*/)
  {
  if (RdPos<WrtPos)
    {
    if (RqdMsgId!=XM_Null)
      {
      pXB_Header p=(pXB_Header)&Data[RdPos];
      return (RqdMsgId==p->Id);
      }
    return true;
    }
  return false;
  };

//---------------------------------------------------------------------------

byte CXMsgLst::MsgId()
  {
  if (RdPos<WrtPos)
    {
    pXB_Header p=(pXB_Header)&Data[RdPos];
    return (p->Id);
    }
  return XM_Null;
  };

//---------------------------------------------------------------------------

flag CXMsgLst::MsgIsSkip(byte RqdMsgId)
  {
  if (RdPos<WrtPos)
    {
    pXB_Header p=(pXB_Header)&Data[RdPos];
    if (RqdMsgId==p->Id)
      {
      RdPos+=p->wLength;
      return true;
      }
    }
  return false;
  };

//---------------------------------------------------------------------------

void CXMsgLst::dbgDump(flag Full, char * Hd1/*=NULL*/, char*Hd2/*=NULL*/, char*Hd3/*=NULL*/)
  {
/*  if (MsgAvail())
    {
    dword RdPosMem=RdPos;
    dbglock();
    dbgp("CXMsgLst : [No:%4u WrtPos:%6u]", Number, WrtPos);
    if (Hd1) dbgp(" %s", Hd1);
    if (Hd2) dbgp(" %s", Hd2);
    if (Hd3) dbgp(" %s", Hd3);
    dbgpln("");
    dbgindent(2);
    dword n=0;
    while (MsgAvail())
      {
      pXB_Header p=(pXB_Header)&Data[RdPos];
      dbgp(" Id:%2u Length:%6u ", p->Id,p->wLength);
      switch (p->Id)
        {
        case XM_Null           : 
          dbgpln("Null           "); 
          break;
        case XM_DataRequest    : 
          {
          CXM_DataRequest * pb=(CXM_DataRequest *)p;
          dbgpln("DataRequest    Inx:%4i <%06x> %2i %s",pb->Index,pb->Options,pb->Action,pb->Tag); 
          break;
          }
        case XM_DataAvailable  : 
          {
          CXM_DataAvailable * pb=(CXM_DataAvailable *)p;
          dbgpln("DataAvailable  Inx:%4i %2i %s",pb->Index,pb->Action,pb->Tag); 
          break;
          }
        case XM_TagNotAvail    : 
          {
          CXM_TagNotAvail * pb=(CXM_TagNotAvail *)p;
          dbgpln("TagNotAvail    Inx:%4i %s",pb->Index,pb->Tag); 
          break;
          }
        case XM_ObjectTag      : 
          {
          CXM_ObjectTag * pb=(CXM_ObjectTag *)p;
          dbgpln("ObjectTag      <%06x> %s",pb->Options,pb->Tag); 
          break;
          }
        case XM_ObjectData     : 
          {
          CXM_ObjectData *pb=(CXM_ObjectData *)p;
          dbgp("ObjectData  [%5i]",pb->Index); 
          pb->List.dbgDump(Full, "");
          break;
          }
        case XM_Route          : 
          {
          CXM_Route * pb=(CXM_Route *)p;
          pb->dbgDump();
          break;
          }
        case XM_ReadIndexedData: 
          {
          CXM_ReadIndexedData * pb=(CXM_ReadIndexedData *)p;
          dbgpln("ReadIndexedData %5i %s %s",pb->LastIndex, pb->Start ? "Start":"", pb->ReadAll ? "ReadAll":""); 
          break;
          }
        case XM_HistoryExists  : 
          dbgpln("HistoryExists  "); 
          break;
        case XM_KeepHistory    : 
          {
          CXM_KeepHistory * pb=(CXM_KeepHistory *)p;
          dbgpln("KeepHistory   %s",pb->cTagEndDesc);
          }
          break;
        case XM_QueryHistory  : 
          {
          CXM_QueryHistory * pb=(CXM_QueryHistory *)p;
          int iTrnd;
          dbgp("QueryHistory %g > %g", pb->dStartTime, pb->dEndTime); 
          for (char*pc=pb->FirstTag(iTrnd); pc; pc=pb->NextTag(iTrnd, pc))
            dbgp(" [%i]%s", iTrnd, pc);
          dbgpln(""); 
          }
          break;
        case XM_HistorySlotDlg  : 
          dbgpln("HistorySlotDlg  "); 
          break;
        case XM_HistoryData    : 
          {
          CXM_HistoryData * pb=(CXM_HistoryData *)p;
          dbgpln("HistoryData <%4i>[%2i] %16.2f %g",pb->iRqstNo,pb->iTrndNo,pb->dTime,pb->dVal);
          }
          break;
        case XM_HistoryDataError:
          {
          CXM_HistoryDataError * pb=(CXM_HistoryDataError *)p;
          dbgpln("HistoryDataError <%4i>[%2i] %16.2f %s",pb->iRqstNo,pb->lErrorNumber,pb->dTimeMissingData,pb->cFileName);
          }
          break;
        }

      RdPos+=p->wLength;
      }
  
    dbgindent(-2);
    dbgunlock();
    RdPos=RdPosMem;
    }*/
  };

//===========================================================================
#endif
