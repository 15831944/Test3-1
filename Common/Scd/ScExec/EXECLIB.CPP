//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#define __EXECLIB_CPP
#include "execlib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define dbgSmallBuffer   0


IMPLEMENT_SPARES(CXM_DataRequest, 1000)
IMPLEMENT_SPARES(CXM_DataAvailable, 1000)
IMPLEMENT_SPARES(CXM_TagNotAvail, 1000)
IMPLEMENT_SPARES(CXM_ObjectTag, 1000)
IMPLEMENT_SPARES(CXM_ObjectData, 1000)
IMPLEMENT_SPARES(CXM_Route, 1000)
IMPLEMENT_SPARES(CXM_ReadIndexedData, 1000)
IMPLEMENT_SPARES(CXM_HistoryExists, 1000)
IMPLEMENT_SPARES(CXM_KeepHistory, 1000)
IMPLEMENT_SPARES(CXM_KeepHistoryFile, 100)
IMPLEMENT_SPARES(CXM_QueryHistory, 1000)
IMPLEMENT_SPARES(CXM_QueryHistoryOther, 1000)
IMPLEMENT_SPARES(CXM_QueryString, 1000)
IMPLEMENT_SPARES(CXM_QueryRow, 1000)
IMPLEMENT_SPARES(CXM_QueryRowEx, 1000)
IMPLEMENT_SPARES(CXM_HistorySlotDlg, 1000)
IMPLEMENT_SPARES(CXM_HistoryData, 1000)
IMPLEMENT_SPARES(CXM_HistoryDataError, 1000)
IMPLEMENT_SPARES(CXM_HistRecordingOn, 1000)
IMPLEMENT_SPARES(CXM_QueryTime, 1000)
IMPLEMENT_SPARES(CXM_Execute, 1000)
IMPLEMENT_SPARES(CXM_TimeControl, 1000)
IMPLEMENT_SPARES(CXM_Long, 1000)
IMPLEMENT_SPARES(CXM_Boolean, 1000)
IMPLEMENT_SPARES(CXM_Double, 1000)
IMPLEMENT_SPARES(CXM_String, 1000)
#if WITHDDEREPORTS
IMPLEMENT_SPARES(CXM_DDEReport, 1000)
IMPLEMENT_SPARES(CXM_DDEErrorCode, 1000)
#endif
IMPLEMENT_SPARES(CXM_DrvShowTagInfo, 1000)
IMPLEMENT_SPARES(CXM_OleExcelReport, 1000)
IMPLEMENT_SPARES(CXM_OleErrorCode, 1000)
IMPLEMENT_SPARES(CXM_RepTrendDB, 1000)
IMPLEMENT_SPARES(CXM_ArchiveExists, 1000)
IMPLEMENT_SPARES(CXM_ArcShowTagInfo, 1000)
IMPLEMENT_SPARES(CXM_KeepArchive, 1000)

IMPLEMENT_SPARES(CXMsgLst, 100)


//===========================================================================

static long CXM_HeaderCnt=0;

CXM_Header::CXM_Header(byte MsgId/*=XM_Null*/)
  {
  Id=MsgId;
  m_dwLength=0;// Invalid Size - Must be set in derived classes

  //dbgpln(">>>> %4i) 0x%08x %3i", ++CXM_HeaderCnt, this, Id);
  }

//---------------------------------------------------------------------------

CXM_Header::CXM_Header(const CXM_Header& Cpy)
  {
  memcpy(this, &Cpy, Cpy.Length());
  //dbgpln(">>>> %4i) 0x%08x %3i", ++CXM_HeaderCnt, this, Id);
  };

//---------------------------------------------------------------------------

CXM_Header::~CXM_Header()
  {
  //dbgpln("  <<     0x%08x %3i", this, Id);
  };

//---------------------------------------------------------------------------

CXM_Header& CXM_Header::operator=(const CXM_Header& Cpy)
  {
  memcpy(this, &Cpy, Cpy.Length());
  return *this;
  };

//===========================================================================

CXM_DataRequest::CXM_DataRequest(long Inx, char * pTag, TABOptions Opts, XIOAction Act)
  {
  Index=Inx;
  Options=Opts;
  Action=Act;
  strcpy(Tag, pTag);
  // CXM_Header
  Id=XM_DataRequest;
  m_dwLength=(&Tag[0]-(char*)this) +strlen(Tag) + 1;
  }

//---------------------------------------------------------------------------

void CXM_DataRequest::Set(long Inx, char * pTag, TABOptions Opts, XIOAction Act)
  {
  Index=Inx;
  Options=Opts;
  Action=Act;
  strcpy(Tag, pTag);
  // CXM_Header
  Id=XM_DataRequest;
  m_dwLength=(&Tag[0]-(char*)this) +strlen(Tag) + 1;
  };

//===========================================================================

CXM_DataAvailable::CXM_DataAvailable(long Inx, char * pTag, XIOAction Act)
  {
  Index=Inx;
  Action=Act;
  strcpy(Tag, pTag);
  // CXM_Header
  Id=XM_DataAvailable;
  m_dwLength=(&Tag[0]-(char*)this) +strlen(Tag) + 1;
  }

//===========================================================================

CXM_TagNotAvail::CXM_TagNotAvail(long Inx, char * pTag)
  {
  Index=Inx;
  strcpy(Tag, pTag);
  // CXM_Header
  Id=XM_TagNotAvail;
  m_dwLength=(&Tag[0]-(char*)this) +strlen(Tag) + 1;
  }

//---------------------------------------------------------------------------

void CXM_TagNotAvail::Set(long Inx, char * pTag)
  {
  Index=Inx;
  strcpy(Tag, pTag);
  // CXM_Header
  Id=XM_TagNotAvail;
  m_dwLength=(&Tag[0]-(char*)this) +strlen(Tag) + 1;
  };

//===========================================================================

CXM_ObjectTag::CXM_ObjectTag(char * pTag, TABOptions Opt)
  {
  Options=Opt;
  strcpy(Tag, pTag);
  // CXM_Header
  Id=XM_ObjectTag;
  m_dwLength=(&Tag[0]-(char*)this) +strlen(Tag) + 1;
  }

//===========================================================================

#if _DEBUG
long CXM_ObjectData::m_nAllocs=0;
long CXM_ObjectData::m_LRange1=-1;
long CXM_ObjectData::m_LRange2=-1;
#endif


CXM_ObjectData::CXM_ObjectData(long Inx /*=0*/, dword Options/*=0*/)
  {
  Index=Inx;
  dwOptions=Options;
  // CXM_Header
  Id=XM_ObjectData;

  //m_pList=new CPkDataList;
  m_dwLength=((char*)&m_List-(char*)this) + sizeof(m_List);
//  m_dwLength=0;// still need to be set once Obj is Set;
#if _DEBUG
  m_AllocNo=++m_nAllocs;
  //dbgpln(" CXM_ObjectData [%5i] 0x%08x", m_AllocNo, this);
  if (m_AllocNo>=m_LRange1 && m_AllocNo<=m_LRange2)
    DoBreak();
#endif
  }

//---------------------------------------------------------------------------

CXM_ObjectData::CXM_ObjectData(long Inx, dword Options, CPkDataItem * pItem)
  {

  Index=Inx;
  dwOptions=Options;
  //m_pList=new CPkDataList;
  if (pItem)
    {
    m_List.AddTail(CPkDataItem::Create(*pItem));
    //memcpy(&m_List, pList, pList->Size());
    }
  else
    List.Clear();
  // CXM_Header
  Id=XM_ObjectData;

  m_dwLength=((char*)&m_List-(char*)this) + sizeof(m_List);

#if _DEBUG
  m_AllocNo=++m_nAllocs;
  //dbgpln(" CXM_ObjectData [%5i] 0x%08x", m_AllocNo, this);
  if (m_AllocNo>=m_LRange1 && m_AllocNo<=m_LRange2)
    DoBreak();
#endif
  //m_dwLength=((char*)&List-(char*)this) + List.Size();
  }

//---------------------------------------------------------------------------

CXM_ObjectData::CXM_ObjectData(long Inx, dword Options, char *pTag, PkDataUnion &Data)
  {
  dwOptions=Options;
  m_dwLength=((char*)&m_List-(char*)this) + sizeof(m_List);

  //m_pList=new CPkDataList;
  //CPkDataItem * pItem=List.FirstItem();
  List.SetDataValue(pTag, 0, Data);
  //SetSize();
#if _DEBUG
  m_AllocNo=++m_nAllocs;
  //dbgpln(" CXM_ObjectData [%5i] 0x%08x", m_AllocNo, this);
  if (m_AllocNo>=m_LRange1 && m_AllocNo<=m_LRange2)
    DoBreak();
#endif
  };

//---------------------------------------------------------------------------

CXM_ObjectData::CXM_ObjectData(long Inx, dword Options, char *pTag, DDEF_Flags iFlags, PkDataUnion &Data)
  {
  dwOptions=Options;
  m_dwLength=((char*)&m_List-(char*)this) + sizeof(m_List);
  // m_pList=new CPkDataList;
  //CPkDataItem * pItem=List.FirstItem();
  List.SetDataValue(pTag, iFlags, Data);
  //SetSize();
#if _DEBUG
  m_AllocNo=++m_nAllocs;
//  dbgpln(" CXM_ObjectData [%5i] 0x%08x", m_AllocNo, this);
  if (m_AllocNo>=m_LRange1 && m_AllocNo<=m_LRange2)
    DoBreak();
#endif
  };

//---------------------------------------------------------------------------

CXM_ObjectData::~CXM_ObjectData()
  {
#if _DEBUG
//  dbgpln("~CXM_ObjectData [%5i] 0x%08x", m_AllocNo, this);
#endif
  List.Clear();
  };

//===========================================================================

CXM_Route::CXM_Route() { Clear(); };
CXM_Route::CXM_Route(const CXM_Route & X) :  CXM_Header(X)
  {
  wEnd           = X.wEnd;
  bAcrossNetwork = X.bAcrossNetwork;
  //wCurrentPos    = X.wCurrentPos;
  nNodes         = X.nNodes;
  memcpy(cBuffer, X.cBuffer, sizeof(cBuffer));
  };
CXM_Route::~CXM_Route() {};

//---------------------------------------------------------------------------

void CXM_Route::Clear()
  {
  /*wCurrentPos=*/wEnd=0;
  bAcrossNetwork=0;
  nNodes=0;

  Id=XM_Route;
  m_dwLength=&cBuffer[wEnd]-(char*)this;
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

    m_dwLength=&cBuffer[wEnd]-(char*)this;
    return True;
    }
  return False;
  };

//---------------------------------------------------------------------------

flag CXM_Route::AddNodeRev(long NodeObjId, char* NodeName)
  {
  Id=XM_Route;
  word l=sizeof(long)+strlen(NodeName)+1;
  if (wEnd+l<CXM_RouteBuffMax)
    {
    memmove(&cBuffer[l], &cBuffer[0], wEnd);
    wEnd+=l;

    *((long*)(&cBuffer[0])) = NodeObjId;
    strcpy(&cBuffer[sizeof(long)], NodeName);
    nNodes++;
    m_dwLength=&cBuffer[wEnd]-(char*)this;
    return True;
    }
  return False;
  };

//---------------------------------------------------------------------------

flag CXM_Route::RemoveNode(int n)
  {
  word p=0;
  while (n-->0)
    p+=strlen(&cBuffer[p+sizeof(long)])+1;
  if (p<wEnd)
    {
    int l=sizeof(long) + strlen(&cBuffer[p+sizeof(long)])+1;
    memmove(&cBuffer[p], &cBuffer[p+l], wEnd-(p+l));
    nNodes--;
    return True;
    }
  return False;
  };

//---------------------------------------------------------------------------

long CXM_Route::NodeObjId(int n)
  {
  word p=0;
  while (n-->0)
    p+=sizeof(long)+strlen(&cBuffer[p+sizeof(long)])+1;
  return p<wEnd ? *((long*)(&cBuffer[p])) : -1;
  }

//---------------------------------------------------------------------------

char* CXM_Route::NodeName(int n)
  {
  word p=0;
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
  return False;
  };

//---------------------------------------------------------------------------

CXM_Route &CXM_Route::operator=(CXM_Route &Other)
  {
  Id=Other.Id;
  m_dwLength=Other.m_dwLength;
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

//===========================================================================

CXM_ReadIndexedData::CXM_ReadIndexedData(flag Strt, flag RdAll, long LastInx)
  {
  Start = Strt;
  ReadAll=RdAll;
  LastIndex=LastInx;
  // CXM_Header
  Id=XM_ReadIndexedData;
  m_dwLength=sizeof(*this);
  }

//===========================================================================

CXM_HistoryExists::CXM_HistoryExists(long Index, char * pNewTag)
  {
  char * p=cTag;
  strcpy(p, pNewTag);
  p+=strlen(pNewTag)+1;
  *((long *)p)=Index;
  p+=sizeof(long );
  *((dword*)p)=0;
  p+=sizeof(dword);
  strcpy(p, ""); // terminator
  p+=1;
  Id=XM_HistoryExists;
  m_dwLength=(p-(char*)this);
  };
CXM_HistoryExists::CXM_HistoryExists()
  {
  strcpy(cTag, ""); // terminator
  // CXM_Header
  Id=XM_HistoryExists;
  m_dwLength=(&cTag[0]-(char*)this)+strlen(cTag)+1;
  };
flag CXM_HistoryExists::xAddTag(long Index, char * pNewTag)//, char * pPosition)
  {
  int l=strlen(pNewTag)+1+sizeof(long)+sizeof(dword)+1;
  if (l+m_dwLength > sizeof(*this))
    return False;
  char * p=(char*)this+m_dwLength-1;
  strcpy(p, pNewTag);
  p+=strlen(pNewTag)+1;
  *((long*)p)=Index;
  p+=sizeof(long);
  *((dword*)p)=0;
  p+=sizeof(dword);
  strcpy(p, "");
  p+=1;
  m_dwLength=(p-(char*)this);
  return True;
  }
char * CXM_HistoryExists::FirstTag()
  {
  return strlen(cTag)>0 ? cTag : NULL;
  };
char * CXM_HistoryExists::NextTag(char * pPrevTag)
  {
  char *p=pPrevTag+strlen(pPrevTag)+1+sizeof(long)+sizeof(dword);
  return strlen(p)>0 ? p : NULL;
  };
long CXM_HistoryExists::GetIndex(char * pTag)
  {
  char* pVal=pTag+strlen(pTag)+1;
  return *((long*)pVal);
  };
void  CXM_HistoryExists::SetFlags(char * pTag, dword Flags)
  {
  char* pVal=pTag+strlen(pTag)+1+sizeof(long);
  *((dword*)pVal)=Flags;
  };
dword CXM_HistoryExists::GetFlags(char * pTag)
  {
  char* pVal=pTag+strlen(pTag)+1+sizeof(long);
  return *((dword*)pVal);
  };

flag CXM_HistoryExists::Empty()
  {
  return (long)m_dwLength <= (&cTag[0]-(char*)this)+1;
  };

//---------------------------------------------------------------------------

//CXM_KeepHistory::CXM_KeepHistory(long DataIndex,
//                               char tt_Type, flag bRealTime_, char * Tag, byte CnvNo, char * CnvTxt, char * Desc,
//                               double FilterTau, double WinFltPeriod, int WinFltCount, double FltDelta,
//                               double Min, double Max, double Decrease, double DeltaLo, double DeltaHi,
//                               int NoRec, int NoNotRec, int FilterAlg, int BoxcarAlg)
CXM_KeepHistory::CXM_KeepHistory(DataUnion* pVal, long DataIndex,
                           char tt_Type, char * Tag, CCnvIndex CnvNo, char * CnvTxt, char * Desc,
                           double FilterTau, double WinFltPeriod, int WinFltCount, double FltDelta,
                           double Change, double DeltaLo, double DeltaHi,
                           int NoRec, int NoNotRec, int FilterAlg, int BoxcarAlg,
                           flag RecordIt, flag Driver)
  {
  Val.Set(*pVal);
  lDataIndex=DataIndex;
  cType=tt_Type;
  dFltTau=FilterTau;
  dWinFltPeriod=WinFltPeriod;
  iWinFltCount=WinFltCount;
  dFltDelta=FltDelta;
  dChange=Change;
  dDeltaLo=DeltaLo;
  dDeltaHi=DeltaHi;
  iNoRec=NoRec;
  iNoNotRec=NoNotRec;
  iFilterAlg=FilterAlg;
  iBoxcarAlg=BoxcarAlg;
  iCnv=CnvNo;
  bRecordIt=RecordIt;
  bDriver=Driver;

  if (CnvTxt==NULL)
    CnvTxt = "";
  if (Desc==NULL)
    Desc = "";
  char * p=cTagEndDesc;
  strcpy(p, Tag);
  p+=strlen(Tag)+1;
  strcpy(p, CnvTxt);
  p+=strlen(CnvTxt)+1;
  strcpy(p, Desc);
  p+=strlen(Desc)+1;
  iCharLen=p-cTagEndDesc;

  // CXM_Header
  Id=XM_KeepHistory;
  m_dwLength=p-(char*)this;//(&Tag[0]-(char*)this) +strlen(Tag) + 1;
  };

//===========================================================================

CXM_QueryHistory::CXM_QueryHistory(double StartTime, double EndTime, long RqstNo, long SrcID)
  {
  dStartTime=StartTime;
  dEndTime=EndTime;
  iRqstNo=RqstNo;
  iSrcID=SrcID;
  nTags=0;
  // CXM_Header
  Id=XM_QueryHistory;
  cTags[0]=0;
  m_dwLength=&cTags[0]-(char*)this+1;
  };

//---------------------------------------------------------------------------

flag CXM_QueryHistory::xAddTag(int iTrndNo, char* pNewTag)
  {
  int l=strlen(pNewTag)+1+sizeof(long)+1;
  if (l+m_dwLength > sizeof(*this))
    return False;
  char * p=(char*)this+m_dwLength-1;
  strcpy(p, pNewTag);
  p+=strlen(pNewTag)+1;
  *((long*)p)=iTrndNo;
  p+=sizeof(long);
  strcpy(p, "");
  p+=1;
  m_dwLength=(p-(char*)this);
  nTags++;
  return True;
  //ASSERT_ALWAYS((dword)(m_dwLength)<sizeof(cTags)-10-strlen(pNewTag), "CXM_QueryHistory::xAddTag Tag Overflow");
  //char * p=(char*)this+m_dwLength;
  //int l=sizeof(iTrndNo)+strlen(pNewTag)+1;
  //if (p+l-(char*)this > sizeof(*this))
  //  return False;
  //*((int*)p)=iTrndNo;
  //p+=sizeof(iTrndNo);
  //strcpy(p, pNewTag);
  //m_dwLength+=l;
  //nTags++;
  //return True;
  };

//---------------------------------------------------------------------------

char* CXM_QueryHistory::FirstTag(int &iTrndNo)
  {
  if (strlen(cTags)>0)
    {
    char* pTag=cTags;
    iTrndNo=*((int*)(pTag+strlen(pTag)+1));
    return pTag;
    }
  else
    return NULL;
  };

//---------------------------------------------------------------------------

char* CXM_QueryHistory::NextTag(int &iTrndNo, char* pPrevTag)
  {
  char *p=pPrevTag+strlen(pPrevTag)+1+sizeof(long);
  if (strlen(p)>0)
    {
    iTrndNo=*((int*)(p+strlen(p)+1));
    return p;
    }
  return NULL;
  };

//---------------------------------------------------------------------------

flag CXM_QueryHistory::Empty()
  {
  return (long)m_dwLength <= (&cTags[0]-(char*)this)+1;
  };

//===========================================================================

CXM_QueryHistoryOther::CXM_QueryHistoryOther(double StartTime, double EndTime, long SrcID, byte Opt, byte TimeOptUnits, flag TimeOptFull, flag Headings, long NoPts, double Invalid, byte QryDest, char* Filename1, char* Filename2, byte QryFileMode, double RepTimeOffset)
  {
  dStartTime=StartTime;
  dEndTime=EndTime;
  iSrcID=SrcID;
  iOpt=Opt;
  iTimeOptUnits=TimeOptUnits;
  iQryDest=QryDest;
  bTimeOptFull=TimeOptFull;
  bHeadings=Headings;
  iNoPts=NoPts;
  dInvalid=Invalid;
  dRepTimeOffset=RepTimeOffset;
  //memset(cInvalid, sizeof(cInvalid), 0);
  //strncpy(cInvalid, Invalid, Min(strlen(Invalid), sizeof(cInvalid)-1));
  iFileMode = QryFileMode;
  if (Filename1)
    strcpy(cFilename1, Filename1);
  else
    cFilename1[0]=0;
  if (Filename2)
    strcpy(cFilename2, Filename2);
  else
    cFilename2[0]=0;
  nTags=0;
  cTags[0]=0;
  // CXM_Header
  Id=XM_QueryHistoryOther;
  m_dwLength=&cTags[0]-(char*)this+1;
  };

//---------------------------------------------------------------------------

flag CXM_QueryHistoryOther::xAddTag(char* pNewTag)
  {
  int l=strlen(pNewTag)+1+1;
  if (l+m_dwLength > sizeof(*this))
    return False;
  char * p=(char*)this+m_dwLength-1;
  strcpy(p, pNewTag);
  p+=strlen(pNewTag)+1;
  strcpy(p, "");
  p+=1;
  m_dwLength=(p-(char*)this);
  nTags++;
  return True;
  };

//---------------------------------------------------------------------------

char* CXM_QueryHistoryOther::FirstTag()
  {
  return (strlen(cTags)>0) ? cTags : NULL;
  };

//---------------------------------------------------------------------------

char* CXM_QueryHistoryOther::NextTag(char* pPrevTag)
  {
  char *p=pPrevTag+strlen(pPrevTag)+1;
  return strlen(p)>0 ? p : NULL;
  };

//---------------------------------------------------------------------------

flag CXM_QueryHistoryOther::Empty()
  {
  return (long)m_dwLength <= (&cTags[0]-(char*)this)+1;
  };

//===========================================================================

flag CXM_QueryRowEx::AddValue(double Value)
  {
  const int Pos = m_dwLength-((char*)(&cData[0])-(char*)this);
  if (Pos+2+sizeof(double)>=QueryRowExDataLen)
    return False;
  nPts++;
  cData[Pos] = QueryRowExType_Double;
  *((double*)&cData[Pos+1]) = Value;
  m_dwLength += (sizeof(double)+1);
  return True;
  };

//---------------------------------------------------------------------------

flag CXM_QueryRowEx::AddValue(long Value)
  {
  const int Pos = m_dwLength-((char*)(&cData[0])-(char*)this);
  if (Pos+2+sizeof(long)>=QueryRowExDataLen)
    return False;
  nPts++;
  cData[Pos] = QueryRowExType_Long;
  *((long*)&cData[Pos+1]) = Value;
  m_dwLength += (sizeof(long)+1);
  return True;
  };

//---------------------------------------------------------------------------

flag CXM_QueryRowEx::AddValue(char* Value)
  {
  const int Pos = m_dwLength-((char*)(&cData[0])-(char*)this);
  const int len = (Value ? strlen(Value) : 0);
  if (Pos+3+len>=QueryRowExDataLen)
    return False;
  nPts++;
  cData[Pos] = QueryRowExType_Str;
  if (Value)
    strcpy(&cData[Pos+1], Value);
  else
    cData[Pos+1] = 0;
  m_dwLength += (len + 2);
  return True;
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
  if (Pos>=(int)m_dwLength-((char*)(&cData[0])-(char*)this))
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

CXM_HistorySlotDlg::CXM_HistorySlotDlg(char * Tag, byte WhichDlg)
  {
  iDlg = WhichDlg;
  strcpy(cTag, Tag);
  // CXM_Header
  Id=XM_HistorySlotDlg;
  m_dwLength=(&cTag[0]-(char*)this) +strlen(cTag) + 1;
  };

//===========================================================================

#if _DEBUG
long CXM_HistoryData::m_nAllocs=0;
#endif

CXM_HistoryData::CXM_HistoryData(int TrndNo, long RqstNo, double Time, double Val, byte Status)
  {
  dTime=Time;
  dVal=Val;
  iStatus=Status;
  iTrndNo=TrndNo;
  iRqstNo=RqstNo;
  Id=XM_HistoryData;
  m_dwLength=sizeof(*this);

#if _DEBUG
  m_AllocNo=++m_nAllocs;
  //dbgpln("CXM_HistoryData [%5i]", m_AllocNo);
#endif
  };
CXM_HistoryData::~CXM_HistoryData()
  {
#if _DEBUG
  //dbgpln("~CXM_HistoryData [%5i]", m_AllocNo);
#endif
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
  m_dwLength=(&cFileName[0]-(char*)this) +strlen(cFileName) + 1;
  }

//===========================================================================

CXM_HistRecordingOn::CXM_HistRecordingOn(char * Tag, flag RecordingOn)
  {
  strcpy(cTag, Tag);
  bRecordingOn=RecordingOn;
  // CXM_Header
  Id=XM_HistRecordingOn;
  m_dwLength=(&cTag[0]-(char*)this) + strlen(cTag) + 1;
  }

//===========================================================================

CXM_QueryTime::CXM_QueryTime()
  {
  TimeRqd=dNAN;
  dTimeRqd=dNAN;
  //CXM_Header
  Id=XM_QueryTime;
  m_dwLength=sizeof(*this);
  };

//-------------------------------------------------------------------------

CXM_QueryTime::CXM_QueryTime(double TimeRqd_, double dTimeRqd_)
  {
  TimeRqd=TimeRqd_;
  dTimeRqd=dTimeRqd_;
  //CXM_Header
  Id=XM_QueryTime;
  m_dwLength=sizeof(*this);
  };

//===========================================================================

CXM_Execute::CXM_Execute()
  {
  Time=dNAN;
  dTimeNext=dNAN;
  // CXM_Header
  Id=XM_Execute;
  m_dwLength=sizeof(*this);
  };

//-------------------------------------------------------------------------

CXM_Execute::CXM_Execute(double Time_, double dTimeNext_)
  {
  Time=Time_;
  dTimeNext=dTimeNext_;
  // CXM_Header
  Id=XM_Execute;
  m_dwLength=sizeof(*this);
  };

//===========================================================================

CXM_TimeControl::CXM_TimeControl()
  {
  // CXM_Header
  Id=XM_TimeControl;
  m_dwLength=sizeof(*this);
  };

//-------------------------------------------------------------------------

CXM_TimeControl::CXM_TimeControl(CXM_TimeControl &TC)
  {
  m_bRealTime=TC.m_bRealTime;
  m_bHoldAdv=TC.m_bHoldAdv;
  m_Time=TC.m_Time;
  m_StepSize=TC.m_StepSize;
  m_StepSizeMax=TC.m_StepSizeMax;
  m_StepSizeNxt=TC.m_StepSizeNxt;
  m_StartTime=TC.m_StartTime;
  m_StopTime=TC.m_StopTime;
  m_RealTimeMult=TC.m_RealTimeMult;
  m_nSteps=TC.m_nSteps;
  m_iRunTerm=TC.m_iRunTerm;
  m_dTimeUntilStop=TC.m_dTimeUntilStop;
  m_EqnCB=TC.m_EqnCB;

  // CXM_Header
  Id=XM_TimeControl;
  m_dwLength=sizeof(*this);
  };

//===========================================================================

#if WITHDDEREPORTS
CXM_DDEReport::CXM_DDEReport(byte Opt, char* FileName, char* ReportName)
  {
  iOpt = Opt;
  strcpy(cBuff, FileName);
  iReportNamePos=strlen(FileName)+1;
  strcpy(&cBuff[iReportNamePos], ReportName);
  // CXM_Header
  Id=XM_DDEReport;
  m_dwLength=(&cBuff[0]-(char*)this) + iReportNamePos + strlen(ReportName) + 1;
  }

//===========================================================================

CXM_DDEErrorCode::CXM_DDEErrorCode(long ErrorNumber, char *Msg)
  {
  lErrorNumber = ErrorNumber;
  strcpy(cMsg, Msg);
  // CXM_Header
  Id=XM_DDEErrorCode;
  m_dwLength=(&cMsg[0]-(char*)this) + strlen(cMsg) + 1;
  }
#endif

//===========================================================================

CXM_DrvShowTagInfo::CXM_DrvShowTagInfo(char * Tag, WORD DrvOptMask)
  {
  //0x0001  show in status bar
  //0x0002  show in message window (LogNote)
  //0x0004  show in driver slot dialog
  iDrvOptMask = DrvOptMask;
  strcpy(cTag, Tag);
  // CXM_Header
  Id=XM_DrvShowTagInfo;
  m_dwLength=(&cTag[0]-(char*)this) + strlen(cTag) + 1;
  };

//===========================================================================

CXM_OleExcelReport::CXM_OleExcelReport(CScdCOCmdBlk *ComCmdBlk, char* FileName, char* ReportName, short Opt)
  {
  iOpt = Opt;
  pComCmdBlk=ComCmdBlk;
  strcpy(cBuff, FileName);
  iReportNamePos=strlen(FileName)+1;
  int RepNameLen=0;
  if (ReportName)
    {
    strcpy(&cBuff[iReportNamePos], ReportName);
    RepNameLen=strlen(ReportName);
    }
  else
    cBuff[iReportNamePos] = 0;
  // CXM_Header
  Id=XM_OleExcelReport;
  m_dwLength=(&cBuff[0]-(char*)this) + iReportNamePos + RepNameLen + 1;
  }

//===========================================================================

CXM_OleErrorCode::CXM_OleErrorCode(long ErrorNumber, char *Msg)
  {
  lErrorNumber = ErrorNumber;
  strcpy(cMsg, Msg);
  // CXM_Header
  Id=XM_OleErrorCode;
  m_dwLength=(&cMsg[0]-(char*)this) + strlen(cMsg) + 1;
  }

//===========================================================================

CXM_RepTrendDB::CXM_RepTrendDB(char* FileName, char* TableName, double EndTime, double Duration, long NoOfPts)
  {
  dEndTime = EndTime;
  dDuration = Duration;
  iNoOfPts = NoOfPts;
  strcpy(cBuff, FileName);
  iTableNamePos=strlen(FileName)+1;
  int TableNameLen=0;
  if (TableName)
    {
    strcpy(&cBuff[iTableNamePos], TableName);
    TableNameLen=strlen(TableName);
    }
  else
    cBuff[iTableNamePos] = 0;
  iTagsPos=iTableNamePos+TableNameLen+1;
  nTags=0;
  // CXM_Header
  Id=XM_RepTrendDB;
  m_dwLength=(&cBuff[0]-(char*)this) + iTableNamePos + TableNameLen + 1;
  }

//---------------------------------------------------------------------------

flag CXM_RepTrendDB::xAddTag(char* pNewTag)
  {
  char * p=(char*)this+m_dwLength;
  int l=strlen(pNewTag)+1;
  if (p+l-(char*)this > sizeof(*this))
    return False;
  strcpy(p, pNewTag);
  m_dwLength+=l;
  nTags++;
  return True;
  };

//---------------------------------------------------------------------------

char* CXM_RepTrendDB::FirstTag()
  {
  if (nTags>0)
    {
    char* pTag=&cBuff[iTagsPos];
    return pTag;
    }
  else
    return NULL;
  };

//---------------------------------------------------------------------------

char* CXM_RepTrendDB::NextTag(char* pPrevTag)
  {
  if (pPrevTag==NULL)
    return NULL;

  pPrevTag+=strlen(pPrevTag)+1;

  if (pPrevTag < (char*)this+m_dwLength)
    return pPrevTag;
  else
    return NULL;
  };

//===========================================================================

LPCTSTR ADBFuncTags[] =
  {
  "",
  "CUR",
  "MIN",
  "MAX",
  "AVG",
  "RUNAVG",
  "CNT",
  NULL,
  };

CXM_ArchiveExists::CXM_ArchiveExists(char * Tag)
  {
  char *pPosition=cTag;
  strcpy(pPosition, Tag);
  pPosition+=strlen(Tag)+1;
  *((dword*)pPosition)=0;
  pPosition+=sizeof(dword);
  strcpy(pPosition, ""); // space holder for Return
  pPosition+=0+1;
  // CXM_Header
  Id=XM_ArchiveExists;
  m_dwLength=(&cTag[0]-(char*)this)+(pPosition-&cTag[0]);
  };
CXM_ArchiveExists::CXM_ArchiveExists()
  {
  strcpy(cTag, "");
  // CXM_Header
  Id=XM_ArchiveExists;
  m_dwLength=(&cTag[0]-(char*)this)+strlen(cTag)+1;
  };
flag CXM_ArchiveExists::xAddTag(char * pNewTag)//, char * pPosition)
  {
INCOMPLETECODE();
  //strcpy(pPosition, pNewTag);
  //pPosition+=strlen(pNewTag)+1;
  //*((dword*)pPosition)=0;
  //pPosition+=sizeof(dword);
  //strcpy(pPosition, "");
  //pPosition+=0+1;
  //// CXM_Header
  //m_dwLength=(&cTag[0]-(char*)this)+(pPosition-&cTag[0]);
  return false;//pPosition-1;
  }
char * CXM_ArchiveExists::FirstTag()
  {
  return cTag;
  };
char * CXM_ArchiveExists::NextTag(char * pPrevTag)
  {
  return pPrevTag+strlen(pPrevTag)+1+sizeof(dword);
  };
void  CXM_ArchiveExists::SetFlags(char * pTag, dword Flags)
  {
  char* pVal=pTag+strlen(pTag)+1;
  *((dword*)pVal)=Flags;
  };
dword CXM_ArchiveExists::GetFlags(char * pTag)
  {
  char* pVal=pTag+strlen(pTag)+1;
  return *((dword*)pVal);
  };

//---------------------------------------------------------------------------

CXM_ArcShowTagInfo::CXM_ArcShowTagInfo(char * Tag, WORD ArcOptMask)
  {
  if (Tag==NULL)
    Tag= "";

  iArcOptMask=ArcOptMask;
  strcpy(&cTag[0], Tag);
  int iCharLen=0+strlen(Tag)+1;

  // CXM_Header
  Id=XM_ArcShowTagInfo;
  m_dwLength=&cTag[iCharLen]-(char*)this;
  };

//---------------------------------------------------------------------------

CXM_KeepArchive::CXM_KeepArchive(DataUnion* pVal, long DataIndex,
                   char tt_Type, char * Tag, CCnvIndex CnvNo, char * CnvTxt,
                   char * Table, byte Fn, char * Desciption)
  {
  if (Tag==NULL)
    Tag= "";
  if (CnvTxt==NULL)
    CnvTxt = "";
  if (Table==NULL)
    Table = "";
  if (Desciption==NULL)
    Desciption = "";

  Val.Set(*pVal);
  lDataIndex=DataIndex;
  cType=tt_Type;
  iCnv=CnvNo;
  iFn=Fn;

  char * p=cBuff;
  strcpy(p, Tag);
  iCnvPos=strlen(Tag)+1;
  strcpy(&cBuff[iCnvPos], CnvTxt);
  iTablePos=iCnvPos+strlen(CnvTxt)+1;
  strcpy(&cBuff[iTablePos], Table);
  iDescPos=iTablePos+strlen(Table)+1;
  strcpy(&cBuff[iDescPos], Desciption);
  iCharLen=iDescPos+strlen(Desciption)+1;

  // CXM_Header
  Id=XM_KeepArchive;
  m_dwLength=&cBuff[iCharLen]-(char*)this;//(&Tag[0]-(char*)this) +strlen(Tag) + 1;
  };

LPCTSTR CXM_KeepArchive::TableName() { return &cBuff[iTablePos]; };
LPCTSTR CXM_KeepArchive::Tag()       { return &cBuff[0]; };
LPCTSTR CXM_KeepArchive::CnvTxt()    { return &cBuff[iCnvPos]; };
LPCTSTR CXM_KeepArchive::Description() { return &cBuff[iDescPos]; };

//===========================================================================

#if _DEBUG
long CXMsgLst::m_nAllocs=0;
long CXMsgLst::m_LRange1=-1;
long CXMsgLst::m_LRange2=-1;
#endif

CXMsgLst::CXMsgLst()     
  { 
  m_bOnHeap=true;
  m_RdPos=NULL; 
  m_bEOF=true; 
//#if _DEBUG
//  m_AllocNo=++m_nAllocs;
//  dbgpln(" CXMsgLst [%5i] 0x%08x", m_AllocNo, this);
//  if (m_AllocNo>=m_LRange1 && m_AllocNo<=m_LRange2)
//    DoBreak();
//#endif
  };

CXMsgLst::~CXMsgLst()     
  { 
//#if _DEBUG
//  dbgpln("~CXMsgLst [%5i] 0x%08x", m_AllocNo, this);
//#endif
  Clear();
  };

void CXMsgLst::Clear()
  {
  m_RdPos=NULL; 
  m_bEOF=true; 
  if (m_bOnHeap)
    {
    while (!IsEmpty())
      delete RemoveHead();
    }
  else
    RemoveAll();
  };

//===========================================================================

inline flag CXMsgLst::PackMsg(CXM_Header *pMsg)//, flag KeepSpaceForRoute/*=True*/)
  {
  //#if _DEBUG
  //  long xxx;
  //  ASSERT(m_bOnHeap != (((char*)pMsg-(char*)&xxx)>0 && ((char*)pMsg-(char*)&xxx)<100000));
  //#endif
  AddTail(pMsg);
  m_bEOF=false;
  return True;
  };

#ifdef MOVED_INLINE
flag CXMsgLst::PackMsg(rXB_Header Msg, flag KeepSpaceForRoute/*=True*/)
  {
  ASSERT(Msg.m_dwLength>0); // Size Must be > 0
  long l=Msg.m_dwLength;

  #if dbgSmallBuffer
  //dbgpln("dbgSmallBuffer");
  if (Number>=4)  // Limit to Max of 4 Messages per buffer
    return False;
  #endif

  if (WrtPos+l < (long)(sizeof(Data) - (KeepSpaceForRoute ? sizeof(CXM_Route): 0)))//MaxTABBuffLen)
    {
//    pXMsgHdr pXHdr=(pXMsgHdr)&Data[WrtPos];
//    pXHdr->iMsg=XMsg;
    //pXHdr->bFlags=XFlags;
    //pXHdr->lIndex=Index;
//    pXHdr->lLength=DataLen;
    //memcpy(&pXHdr->pData, pData, DataLen); // Append DataBlock;
    //WrtPos+=sizeof(*pXHdr)-sizeof(pXHdr->pData)+pXHdr->lLength;

    memcpy(&Data[WrtPos], &Msg, Msg.m_dwLength); // Append Message
    WrtPos+=Msg.m_dwLength;
    //Len=WrtPos;
    Number++;
    return True;
    }
  else
    return False;
  };

//---------------------------------------------------------------------------
/*
flag CXMsgLst::UnpackMsg(rXMsgHdr rXHdr)
  {
  if (RdPos<Len)
    {
    pXMsgHdr pXHdr=(pXMsgHdr)&Data[RdPos];
    memcpy(&rXHdr, pXHdr, sizeof(rXHdr));
    rXHdr.pData=&pXHdr->pData;
    RdPos+=sizeof(*pXHdr)-sizeof(pXHdr->pData)+pXHdr->lLength;
    return True;
    }
  return False;
  };
*/
//---------------------------------------------------------------------------

void * CXMsgLst::MsgPtr(byte RqdMsgId/*=XM_Null*/)
  {
  if (RdPos<WrtPos)
    {
    pXB_Header p=(pXB_Header)&Data[RdPos];
    RdPos+=p->m_dwLength;
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
    return True;
    }
  return False;
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
      RdPos+=p->m_dwLength;
      return True;
      }
    }
  return False;
  };
#endif

//---------------------------------------------------------------------------

void CXMsgLst::dbgDump(flag Full, char * Hd1/*=NULL*/, char*Hd2/*=NULL*/, char*Hd3/*=NULL*/)
  {
  if (MsgAvail())
    {
    POSITION RdPosMem=m_RdPos;
    bool     bEOFMem=m_bEOF;
    dbglock();
    dbgp("CXMsgLst : [No:%4u ]", GetCount());
    if (Hd1) dbgp(" %s", Hd1);
    if (Hd2) dbgp(" %s", Hd2);
    if (Hd3) dbgp(" %s", Hd3);
    dbgpln("");
    dbgindent(2);
    dword n=0;
    while (MsgAvail())
      {
      CXM_Header *p=MsgPtr();
      dbgp(" @0x%08x Id:%2u Length:%6u ", p, p->Id,p->m_dwLength);
      switch (p->Id)
        {
        case XM_Null           :
          dbgpln("Null           ");
          break;
        case XM_DataRequest    :
          {
          CXM_DataRequest * pb=(CXM_DataRequest *)p;
          dbgpln("DataRequest    Inx:%4i <%08x%08x> %2i %s",pb->Index,
            (long)((pb->Options>>32)&0xFFFFFFFF),(long)(pb->Options&0xFFFFFFFF),pb->Action,pb->Tag);
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
          dbgpln("ObjectTag      <%08x%08x> %s",(long)((pb->Options>>32)&0xFFFFFFFF),(long)(pb->Options&0xFFFFFFFF),pb->Tag);
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

      }

    dbgindent(-2);
    dbgunlock();
    m_RdPos=RdPosMem;
    m_bEOF=bEOFMem;
    }
  };

//===========================================================================
