//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __HSTDATYP_H
#define  __HSTDATYP_H

#ifndef ForSysCAD
#define ForSysCAD 1
#endif

#if !ForSysCAD
//========================================================================
//========================================================================
//========================================================================
//from datatyp.h...
//========================================================================
//========================================================================
//========================================================================

#undef DllImportExport

#if defined(__HSTDATYP_CPP)
  #define DllImportExport DllExport
#elif !defined(SCHIST)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

_FWDDEF(DataUnion)
//_FWDDEF(PkString)
_FWDDEF(PkDataUnion)
//_FWDDEF(DataChangeBlk)
//_FWDDEF(FilingControlBlock)

typedef unsigned long TABOptions;
typedef unsigned long DDEF_Flags;
typedef unsigned long DDEF_NElements;

//---------------------------------------------------------------------------

// DataTypes
const byte tt_NULL        =  0;
const byte tt_Bool        =  1;
const byte tt_Bit         =  2;
const byte tt_Byte        =  3;
const byte tt_Word        =  4;
const byte tt_DWord       =  5;
const byte tt_Char        =  6;
const byte tt_Int         =  7;
const byte tt_Short       =  8;
const byte tt_Long        =  9;
const byte tt_Flt16       = 10;
const byte tt_Float       = 11;
const byte tt_Double      = 12;
const byte tt_Generic     = 13;
const byte tt_pChar       = 14;
const byte tt_ppChar      = 15;
const byte tt_Strng       = 16;
const byte tt_RqdPage     = 17;
const byte tt_OptPage     = 18;
const byte tt_Column      = 19;
const byte tt_Text        = 20;
const byte tt_Struct      = 21;
const byte tt_Element     = 22;
const byte tt_Object      = 23;
const byte tt_Array       = 24;
const byte tt_Struct_E    = 25;
const byte tt_Element_E   = 26;
const byte tt_Object_E    = 27;
const byte tt_Array_E     = 28;
const byte tt_Blob        = 29;
                          
// Data Status            
const byte ds_OK          = 0;
const byte ds_UnderFlow   = 1;
const byte ds_OverFlow    = 2;
const byte ds_NAN         = 3;

// --------------------------------------------------------------------------

inline flag IsData(byte iType)        { return (iType>=tt_Bool && iType <= tt_Strng);};
inline flag IsNumData(byte iType)     { return (iType>=tt_Bool && iType <= tt_Generic);};
inline flag IsIntData(byte iType)     { return (iType>=tt_Bool && iType <= tt_Long);};
inline flag IsUnSgnData(byte iType)   { return (iType>=tt_Byte && iType <= tt_DWord);};
inline flag IsFloatData(byte iType)   { return (iType>=tt_Flt16 && iType <= tt_Double);};
inline flag IsStrng(byte iType)       { return (iType>=tt_pChar && iType <= tt_Strng);};
inline flag IsStructure(byte iType)   { return (iType>=tt_Struct && iType <= tt_Array_E);};
inline flag IsStructStart(byte iType) { return (iType>=tt_Struct && iType <= tt_Array);};
inline flag IsStructEnd(byte iType)   { return (iType>=tt_Struct_E && iType <= tt_Array_E);};
inline flag IsBlockStart(byte iType)  { return (iType>=tt_Struct && iType <= tt_Object);};
inline flag IsBlockEnd(byte iType)    { return (iType>=tt_Struct_E && iType <= tt_Object_E);};
inline flag IsObjectStart(byte iType) { return (iType==tt_Object);};
inline flag IsObjectEnd(byte iType)   { return (iType==tt_Object_E);};
inline flag IsArrayStart(byte iType)  { return (iType==tt_Array);};
inline flag IsArrayEnd(byte iType)    { return (iType==tt_Array_E);};
inline flag IsLayout(byte iType)      { return (iType>=tt_RqdPage && iType<=tt_Text);};
inline byte StructEndType(byte iType) { switch (iType) {case tt_Struct: return tt_Struct_E;case tt_Element: return tt_Element_E;case tt_Object: return tt_Object_E;case tt_Array: return tt_Array_E;};return tt_NULL;};
inline byte IsBlob(byte iType)        { return (iType==tt_Blob);};

typedef struct {byte t; char* Str;} TagObjTypeStruct;
const TagObjTypeStruct TagObjTypeNames[] = 
  { 
    { tt_NULL,      "NULL" },
    { tt_Bool,      "Bool" },
    { tt_Bit,       "Bit" },
    { tt_Byte,      "Byte" },
    { tt_Word,      "Word" },
    { tt_DWord,     "DWord" },
    { tt_Char,      "Char" },
    { tt_Int,       "Int" },
    { tt_Short,     "Short" },
    { tt_Long,      "Long" },
    { tt_Flt16,     "Flt16" },
    { tt_Float,     "Float" },
    { tt_Double,    "Double" },
    { tt_Generic,   "Generic" },
    { tt_pChar,     "pChar" },
    { tt_ppChar,    "ppChar" },
    { tt_Strng,     "Strng" },
    { tt_RqdPage,   "RqdPage" },
    { tt_OptPage,   "OptPage" },
    { tt_Column,    "Column" },
    { tt_Text,      "Text" },
    { tt_Struct,    "{" },
    { tt_Element,   "(" },
    { tt_Object,    "<" },
    { tt_Array,     "[" },
    { tt_Struct_E,  "}" },
    { tt_Element_E, ")" },
    { tt_Object_E,  ">" },
    { tt_Array_E,   "]" },
    { tt_Blob,      "BLOB" },
    { tt_NULL,      NULL}
  };

inline const char* tt_TypeString(byte tt) 
  { 
  const TagObjTypeStruct *p=&TagObjTypeNames[0]; 
  while (p->Str) 
    if (p->t==tt) 
      return p->Str; 
    else p++; 
  return "?";
  };
          
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// All the Packed structures must have packing of 1 byte
#pragma pack(push, 1)  

// --------------------------------------------------------------------------

class DllImportExport flt16        // NBNB No virtual methods
  {
  public:
    unsigned short Exp:4, Mant:11, Sgn:1;
    operator       double();
    flt16          operator=(double v);
    static void    Test();
  };

// --------------------------------------------------------------------------

class DllImportExport DataUnionCom // NBNB No virtual methods
  {
  public:
    byte          iType:5, iStatus:2, bSpare:1;
    union 
      {
      //byte         Bit; 
      byte            Byte; 
      unsigned short  Word;
      unsigned long   DWord;
      int             Int;  
      char            Char;
      short           Short;
      long            Long; 
      float           Float;
      double          Double;
      flt16           Flt16;
      char*           pChar;
      short           Len;    // Length of PkString
      };

  public:
    DataUnionCom() {iType=tt_NULL; iStatus=ds_OK; bSpare=0; };

    byte           Type() { return iType; };
    byte           Status() { return iStatus; };
    void           SetStatus(byte Status) { iStatus=Status; };
    unsigned short Size();
    unsigned short TypeSize();
    flag           IsNAN();

    void           Set(DataUnionCom &V);
    //void           Set(DataUnion &V);
    //void           Set(PkDataUnion &V);

    flag           Equal(DataUnionCom &V);
//    flag           Equal(PkDataUnion &V);

    long           GetLong(int iCnv=0, char*pCnvTxt=NULL);
    void           PutLong(long L, int iCnv=0, char*pCnvTxt=NULL);
    double         GetDouble(int iCnv=0, char*pCnvTxt=NULL);
    void           PutDouble(double D, int iCnv=0, char*pCnvTxt=NULL);
    char*          GetString(char*IFmt="%i", char*FFmt="%g", int iCnv=0, char*pCnvTxt=NULL);
    
    flag           SetTypeString(byte Type, char*pStrData, int iCnv=0, char*pCnvTxt=NULL);
  };

// --------------------------------------------------------------------------

class DllImportExport DataUnion : public DataUnionCom // NBNB No virtual methods
  {
  public:
    DataUnion()         {};

    DataUnion(char           V) {iType=tt_Char; Char=V;};
    DataUnion(byte           V) {iType=tt_Byte; Byte=V;};
    DataUnion(unsigned short V) {iType=tt_Word; Word=V;};
    DataUnion(unsigned long  V) {iType=tt_DWord; DWord=V;};
    DataUnion(int            V) {iType=tt_Int; Int=V;};
    DataUnion(short          V) {iType=tt_Short; Short=V;};
    DataUnion(long           V) {iType=tt_Long; Long=V;};
    DataUnion(float          V) {iType=tt_Float; Float=V;};
    DataUnion(double         V) {iType=tt_Double; Double=V;};

    ~DataUnion()        { Clear(); };

    unsigned short Size();

    void           Clear() {if (iType==tt_pChar) delete [] pChar; iType=tt_NULL;};
                   
    void           Set(byte           V) {Clear(); iType=tt_Byte; iStatus=ds_OK; Byte=V;};
    void           Set(unsigned short V) {Clear(); iType=tt_Word; iStatus=ds_OK; Word=V;};
    void           Set(unsigned long  V) {Clear(); iType=tt_DWord; iStatus=ds_OK; DWord=V;};
    void           Set(int            V) {Clear(); iType=tt_Int; iStatus=ds_OK; Int=V;};
    void           Set(char           V) {Clear(); iType=tt_Char; iStatus=ds_OK; Char=V;};
    void           Set(short          V) {Clear(); iType=tt_Short; iStatus=ds_OK; Short=V;};
    void           Set(long           V) {Clear(); iType=tt_Long; iStatus=ds_OK; Long=V;};
    void           Set(flt16          V) {Clear(); iType=tt_Flt16; iStatus=ds_OK; Flt16=V;};
    void           Set(float          V) {Clear(); iType=tt_Float; iStatus=ds_OK; Float=V;};
    void           Set(double         V) {Clear(); iType=tt_Double; iStatus=ds_OK; Double=V;};
    void           Set(char*          V) {Clear(); iType=tt_pChar; iStatus=ds_OK; pChar=new char[strlen(V)+1], strcpy(pChar, V);};

    void           Set(DataUnion &V);
    void           Set(PkDataUnion &V);

    //flag           Equal(DataUnion &V);
    //flag           Equal(PkDataUnion &V);

    long           GetLong(int iCnv=0, char*pCnvTxt=NULL);
    void           PutLong(long L, int iCnv=0, char*pCnvTxt=NULL);
    double         GetDouble(int iCnv=0, char*pCnvTxt=NULL);
    void           PutDouble(double D, int iCnv=0, char*pCnvTxt=NULL);
    char*          GetString(char*IFmt="%i", char*FFmt="%g", int iCnv=0, char*pCnvTxt=NULL);

    flag           SetTypeString(byte Type, char*pStrData, int iCnv=0, char*pCnvTxt=NULL);
    void           SetTypeDouble(byte Type, double d, int iCnv=0, char*pCnvTxt=NULL) {Clear(); iType = Type; PutDouble(d, iCnv, pCnvTxt); }; 
    void           SetTypeLong(byte Type, long l, int iCnv=0, char*pCnvTxt=NULL) {Clear(); iType = Type; PutLong(l, iCnv, pCnvTxt); }; 

    void           CopyTo(void* p); // KGA
  };

//========================================================================
//------------------------------------------------------------------------

inline unsigned short DataUnion::Size()
  {
  if (IsStrng(iType))
    return sizeof(pChar)+sizeof(byte);
  else
    return DataUnionCom::Size();
  };

//------------------------------------------------------------------------

inline void DataUnion::Set(DataUnion &V)
  {
  if (IsStrng(V.iType))
    {
    Set(V.pChar);
    iType=V.iType;
    iStatus=V.iStatus;
    }
  else
    DataUnionCom::Set(V);
  };

//------------------------------------------------------------------------

inline long DataUnion::GetLong(int iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  if (IsStrng(iType))
    {
    VERIFY(0);
    return 0;
    }
  else 
    return DataUnionCom::GetLong(iCnv, pCnvTxt);
  };

//------------------------------------------------------------------------

inline void DataUnion::PutLong(long L, int iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  if (IsStrng(iType))
    {
    VERIFY(0);
    }
  else 
    DataUnionCom::PutLong(L, iCnv, pCnvTxt);
  };

//------------------------------------------------------------------------

inline double DataUnion::GetDouble(int iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  if (IsStrng(iType))
    {
    VERIFY(0);
    return 0.0;
    }
  else 
    return DataUnionCom::GetDouble(iCnv, pCnvTxt);
  };

//------------------------------------------------------------------------

inline void DataUnion::PutDouble(double D, int iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  if (IsStrng(iType))
    {
    VERIFY(0);
    }
  else 
    DataUnionCom::PutDouble(D, iCnv, pCnvTxt);
  };

//------------------------------------------------------------------------

inline char* DataUnion::GetString(char*IFmt/*="%i"*/, char*FFmt/*="%g"*/, int iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  if (IsStrng(iType))
    return pChar;
  else 
    return DataUnionCom::GetString(IFmt, FFmt, iCnv, pCnvTxt);
  }

//------------------------------------------------------------------------

inline flag DataUnion::SetTypeString(byte iTypeRqd, char*pStrData, int iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  if (IsStrng(iTypeRqd))
    {
    Set(pStrData);
    iType=iTypeRqd;
    iStatus=ds_OK;
    }
  else 
    DataUnionCom::SetTypeString(iTypeRqd, pStrData, iCnv, pCnvTxt);
  return true;
  }

//========================================================================
// --------------------------------------------------------------------------

//const unsigned long PkDataListLength = 0x40000; // 0x20000 // CNM

//========================================================================
//========================================================================
//========================================================================
//from execlib.h...
//========================================================================
//========================================================================
//========================================================================

const byte XM_Null              = 0;
const byte XM_Route             = 3;

const byte XM_HistoryExists     = 11;  // historian messages
const byte XM_KeepHistory       = 12;
const byte XM_QueryRow          = 13; //no longer used, replaced with XM_QueryRowEx
const byte XM_QueryHistory      = 14;
const byte XM_HistorySlotDlg    = 15;
const byte XM_HistoryData       = 16;
const byte XM_HistoryDataError  = 17;
const byte XM_QueryHistoryOther = 18;
const byte XM_QueryString       = 19;
const byte XM_QueryRowEx        = 38;
const byte XM_HistRecordingOn   = 39;

//-------------------------------------------------------------------------

_FWDDEF(CXM_Header)
class DllImportExport CXM_Header
  {
  public:
    byte           Id;
    unsigned long  wLength;
    CXM_Header(byte MsgId=XM_Null);

    CXM_Header(CXM_Header& Cpy);
    CXM_Header& operator=(CXM_Header& Cpy);

    unsigned long  Length() { return wLength; };
  };

//-------------------------------------------------------------------------
/*
_FWDDEF(CXM_ObjectData)
class DllImportExport CXM_ObjectData : public CXM_Header
  {
  public:
    long           Index;
    //CPkDataList     List;

    CXM_ObjectData(long Inx=0);
    //CXM_ObjectData(long Inx, CPkDataList * pObj);
    //CXM_ObjectData(long Inx, char *pTag, PkDataUnion &Data);
    //CXM_ObjectData(long Inx, char *pTag, DDEF_Flags iFlags, PkDataUnion &Data);
    void          SetSize();                                                      
    
    // Helper Methods
    //CPkDataItem*   FirstItem() { return List.FirstItem(); };
    //CPkDataItem*   NextItem(CPkDataItem* pItem) { List.AdvanceRead(pItem); return pItem; };
  };
*/
//-------------------------------------------------------------------------

const long MaxCXMsgLstSize=sizeof(CXM_Header)+128;//sizeof(CXM_ObjectData);

const int CXM_RouteBuffMax=1024;
typedef char NodeNameBuff[MAX_COMPUTERNAME_LENGTH + 1 + 100];

_FWDDEF(CXM_Route)
class DllImportExport CXM_Route : public CXM_Header
  {
  private:
    unsigned short wEnd;
    flag           bAcrossNetwork;
    //word           wCurrentPos;
    byte           nNodes;
    char           cBuffer[CXM_RouteBuffMax];

  public:
    CXM_Route();
    ~CXM_Route();

    void           Clear();
    flag           AddNodeFwd(long NodeObjId, char* NodeName);
    flag           AddNodeRev(long NodeObjId, char* NodeName);
    flag           RemoveNode(int n);
    long           NodeObjId(int n);
    char*          NodeName(int n);
    long           NoNodes();
    //flag           SameRoute(CXM_Route &Other);
    flag           operator==(CXM_Route& Other);
    CXM_Route&      operator=(CXM_Route& Other);
    
    flag           AcrossNetwork() { return bAcrossNetwork; };
    void           SetAcrossNetwork(flag On) { bAcrossNetwork=On; };

    void           ReverseRoute(CXM_Route &Other);
    static char*   ComputerName();
    static char*   MakeNodeName(char * Node, char * Where);
    void           dbgDump(char *Where="");

  };

//-------------------------------------------------------------------------

_FWDDEF(CXM_HistoryData)
class DllImportExport CXM_HistoryData : public CXM_Header
  {
  public:
    double         dTime;
    double         dVal;
    byte           iStatus;
    int            iTrndNo;
    long           iRqstNo;           
  
  CXM_HistoryData(int TrndNo, long RqstNo, double Time, double Val, byte iStatus);
  long Length() { return sizeof(*this);};
  };

//-------------------------------------------------------------------------

_FWDDEF(CXM_HistoryDataError)
class DllImportExport CXM_HistoryDataError : public CXM_Header
  {
  public:
    long           lErrorNumber;
    double         dTimeMissingData;
    long           iRqstNo;
    char           cFileName[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/];
  
  CXM_HistoryDataError(long ErrorNumber, long RqstNo, double TimeMissingData, char *ReqdFileName);
  };

//-------------------------------------------------------------------------

_FWDDEF(CXM_QueryString)
class DllImportExport CXM_QueryString : public CXM_Header
  {
  public:
    long           iSrcID;
    char           cValue[MaxCXMsgLstSize-sizeof(CXM_Header)-100 /*Space for data members*/];
    
    CXM_QueryString(long SrcID, char* Value = NULL)
      { 
      Id=XM_QueryString;
      iSrcID=SrcID;
      Set(Value);
      };
    void Set(char* Value, ...)
      { 
      wLength=&cValue[0]-(char*)this;
      if (Value==NULL)
        return;
      va_list argptr;
      va_start(argptr, Value);
      VERIFY(vsprintf(cValue, Value, argptr) < sizeof(cValue));
      va_end(argptr);
      wLength+=strlen(cValue)+1;
      };
    char* String() { return cValue; };
  };

//-------------------------------------------------------------------------

const int MaxQueryRowValues = (MaxCXMsgLstSize-sizeof(CXM_Header)-100) / sizeof(double);
_FWDDEF(CXM_QueryRow)
class DllImportExport CXM_QueryRow : public CXM_Header
  {
  public:
    long           iSrcID;
    double         dTime;
    int            nPts;
    double         dValue[MaxQueryRowValues];
    
    CXM_QueryRow(long SrcID, double Time)
      { 
      Id=XM_QueryRow;
      iSrcID=SrcID;
      dTime=Time;
      nPts=0;
      wLength=(char*)(&dValue[0])-(char*)this;
      };
    flag AddValue(double Value, ...)
      {
      if (nPts>=MaxQueryRowValues)
        return false;
      dValue[nPts++] = Value;
      wLength += sizeof(double);
      return true;
      };
    double         Value(int i) { ASSERT(i<nPts); return dValue[i]; };
  };

//-------------------------------------------------------------------------

const byte QueryRowExType_Null   = 0;
const byte QueryRowExType_Double = 1;
const byte QueryRowExType_Long   = 2;
const byte QueryRowExType_Str    = 3;
const int QueryRowExDataLen = MaxCXMsgLstSize-sizeof(CXM_Header)-100; //Space for data members
_FWDDEF(CXM_QueryRowEx)
class DllImportExport CXM_QueryRowEx : public CXM_Header
  {
  public:
    long           iSrcID;
    double         dTime;
    int            nPts;
    char           cData[QueryRowExDataLen];
    
    CXM_QueryRowEx(long SrcID, double Time)
      { 
      Id=XM_QueryRowEx;
      iSrcID=SrcID;
      dTime=Time;
      nPts=0;
      wLength=(char*)(&cData[0])-(char*)this;
      };
    flag           AddValue(double Value);
    flag           AddValue(long Value);
    flag           AddValue(char* Value);
    byte           FirstValTyp(int & Pos);
    byte           NextValTyp(int & Pos);
    double         DValue(int & Pos);
    long           LValue(int & Pos);
    char*          SValue(int & Pos);
  };

//-------------------------------------------------------------------------

_FWDDEF(CXMsgLst)
class DllImportExport CXMsgLst
  {
  private:
    long           Number;
    long           WrtPos;
    long           RdPos;
    char           Data[MaxCXMsgLstSize];

  public :
    CXMsgLst()        { Init(); };
    void           Init() { Number=0; WrtPos=RdPos=0; Data[0]=0; };
    void           ClearBuff() { Number=0; WrtPos=RdPos=0; Data[0]=0; };
    long           NoMsgs() { return Number;};

    flag           PackMsg(rXB_Header Msg, flag KeepSpaceForRoute=true);
    void *         MsgPtr(byte RqdMsgId=XM_Null);
    flag           MsgAvail(byte RqdMsgId=XM_Null);
    byte           MsgId();
    flag           MsgIsSkip(byte RqdMsgId);

    unsigned long  Length() { return (char*)&Data[WrtPos]-(char*)this; };

    void           RewindReadPosition() { RdPos=0; };

    void           dbgDump(flag Full, char* Hd1=NULL, char* Hd2=NULL, char* Hd3=NULL);

  // Helper Methods
    //CXM_ObjectTag *         ObjectTag        () { return (CXM_ObjectTag *        )MsgPtr(XM_ObjectTag       ); };
    //CXM_ObjectData *       ObjectData       () { return (CXM_ObjectData *      )MsgPtr(XM_ObjectData      ); };
    CXM_Route *             Route            () { return (CXM_Route *            )MsgPtr(XM_Route           ); };
    //CXM_ReadIndexedData *   ReadIndexedData  () { return (CXM_ReadIndexedData *  )MsgPtr(XM_ReadIndexedData ); };
    //CXM_DataRequest *       DataRequest      () { return (CXM_DataRequest *      )MsgPtr(XM_DataRequest     ); };
    //CXM_DataAvailable *     DataAvailable    () { return (CXM_DataAvailable *    )MsgPtr(XM_DataAvailable   ); };
    //CXM_TagNotAvail *       TagNotAvail      () { return (CXM_TagNotAvail *      )MsgPtr(XM_TagNotAvail     ); };
    //pXB_TimeControl       TimeControl      () { return (pXB_TimeControl      )MsgPtr(XM_TimeControl     ); };
    //pXB_QueryTime         QueryTime        () { return (pXB_QueryTime        )MsgPtr(XM_QueryTime       ); };
    //pXB_Execute           Execute          () { return (pXB_Execute          )MsgPtr(XM_Execute         ); };

    //pXB_HistoryExists     HistoryExists    () { return (pXB_HistoryExists    )MsgPtr(XM_HistoryExists   ); };
    //CXM_KeepHistory *       KeepHistory      () { return (CXM_KeepHistory *      )MsgPtr(XM_KeepHistory     ); };
    //pXB_HistorySlotDlg    HistorySlotDlg   () { return (pXB_HistorySlotDlg   )MsgPtr(XM_HistorySlotDlg  ); };
    CXM_HistoryData *       HistoryData      () { return (CXM_HistoryData *      )MsgPtr(XM_HistoryData     ); };
    CXM_HistoryDataError *  HistoryDataError () { return (CXM_HistoryDataError * )MsgPtr(XM_HistoryDataError); };
    //CXM_QueryHistory *      QueryHistory     () { return (CXM_QueryHistory *     )MsgPtr(XM_QueryHistory    ); };
    //pXB_QueryHistoryOther QueryHistoryOther() { return (pXB_QueryHistoryOther)MsgPtr(XM_QueryHistoryOther); };
    pXB_QueryString       QueryString      () { return (pXB_QueryString      )MsgPtr(XM_QueryString     ); };
    pXB_QueryRow          QueryRow         () { return (pXB_QueryRow         )MsgPtr(XM_QueryRow        ); };
    pXB_QueryRowEx        QueryRowEx       () { return (pXB_QueryRowEx       )MsgPtr(XM_QueryRowEx      ); };
    //pXB_HistRecordingOn   HistRecordingOn  () { return (pXB_HistRecordingOn  )MsgPtr(XM_HistRecordingOn ); };
    
    //pXB_Long              Long             () { return (pXB_Long             )MsgPtr(XM_Long            ); };
    //pXB_Boolean           Boolean          () { return (pXB_Boolean          )MsgPtr(XM_Boolean         ); };
    //pXB_Double            Double           () { return (pXB_Double           )MsgPtr(XM_Double          ); };
    //pXB_String            String           () { return (pXB_String           )MsgPtr(XM_String          ); };

    //pXB_DrvShowTagInfo    DrvShowTagInfo   () { return (pXB_DrvShowTagInfo   )MsgPtr(XM_DrvShowTagInfo  ); };

    //pXB_OleExcelReport    OleExcelReport   () { return (pXB_OleExcelReport   )MsgPtr(XM_OleExcelReport  ); };
    //pXB_OleErrorCode      OleErrorCode     () { return (pXB_OleErrorCode     )MsgPtr(XM_OleErrorCode    ); };
  };




//// --------------------------------------------------------------------------
//// Pop Packed structures Packing
#pragma pack(pop)  
//// --------------------------------------------------------------------------

#undef DllImportExport 

#endif

#endif