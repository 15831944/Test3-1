//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __DATATYPE_H
#define  __DATATYPE_H

#ifndef __AFXTEMPL_H__
  #include <afxtempl.h> // "xafxtempl.h"
#endif

#include "scdver.h"
#include "..\..\..\SMDK\Include\md_share0.h"
#include "..\..\..\SMDK\Include\md_share2.h"
#include "scdtempl.h"
#include "errorlog.h"
#include "datacnvs.h"

#undef DllImportExport

#if defined(__DATATYPE_CPP)
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

_FWDDEF(DataUnion)
_FWDDEF(PkString)
_FWDDEF(PkDataUnion)
_FWDDEF(DataChangeBlk)
_FWDDEF(FilingControlBlock)

typedef ULONG32 TABOptions32;
typedef ULONG32 DDEF_Flags32;
typedef ULONG64 DDEF_Flags64;

typedef ULONG64 TABOptions;

typedef DWORD DDEF_NElements;

/** Data Definition **/
// bits 16-19 are for width
// bits 20-23 are for precision
// NB these Mode flags must fit into the lobyte of TABOptions
// defined by TABOpt_ModesReserved

#define isVar               0
#define isParm              DDEF_PARAM
#define isParmStopped       (DDEF_PARAMSTOPPED|DDEF_PARAM)
#define isParmConstruct     DDEF_PARAMCONSTRUCT
#define isTagParm           (DDEF_TAGPARM|DDEF_PARAM)
#define isFunctParm         (DDEF_FUNCTPARM|DDEF_PARAM)
#define isTag               DDEF_TAGPARM
#define isFunct             DDEF_FUNCTPARM
#define isResult            DDEF_RESULT
#define hasNoGrf            DDEF_TAGNOGRF
#define noFile              DDEF_NOFILE
#define noView              DDEF_NOVIEW
#define noSnap              DDEF_NOSNAPSHOT
#define noScenario          DDEF_NOSCENARIO
#define isVisible           DDEF_VISIBLE
#define AffectsStruct       DDEF_AFFECTSSTRUCT
#define HideIfZero          DDEF_HIDEIFZERO
#define SmallValue          DDEF_SMALLVALUE

#define noFileAtAll         DDEF_NOFILE|DDEF_NOSNAPSHOT|DDEF_NOSCENARIO

#define Marker              DDEF_TREENODE
#define MarkerClosed        DDEF_TREECLOSED
#define InitHidden          DDEF_HIDDEN
#define NAN_OK              DDEF_NAN_OK
//#define ButtonFld           DDEF_BUTTON // Name Conflict
//#define CheckBoxFld         DDEF_CHECKBOX
#define DupHandlesOk        DDEF_DUPHANDLES_OK
#define MultiLineStr        DDEF_MULTILINESTR
#define isDriverTag         DDEF_DRIVERTAG
#define SetOnChange         DDEF_SETONCHANGE
#define isArchiveTag        DDEF_ARCHIVETAG
//#define TopologyData        DDEF_TOPOLOGYDATA

//--------------------------------------------------------------------------

const TABOptions TABOpt_ModesReserved    = 0x000000000000ffff;
const TABOptions TABOpt_Parms            = 0x0000000000010000;

// These Values in the range 0x20000.. 0x80000
const TABOptions TABOpt_ForNDB           = DDEF_NONDB;
const TABOptions TABOpt_ForFile          = DDEF_NOFILE;
const TABOptions TABOpt_ForView          = DDEF_NOVIEW;
const TABOptions TABOpt_ForSnapShot      = DDEF_NOSNAPSHOT;
const TABOptions TABOpt_ForScenario      = DDEF_NOSCENARIO;

const TABOptions TABOpt_ForViewConstruct = 0x0000000001000000;
const TABOptions TABOpt_Exists           = 0x0000000002000000;
const TABOptions TABOpt_XRefStatus       = 0x0000000004000000;
const TABOptions TABOPT_ValueStatus      = 0x0000000008000000;
                                               
const TABOptions TABOpt_ValCnvs          = 0x0000000010000000;
const TABOptions TABOpt_StrList          = 0x0000000020000000;
const TABOptions TABOpt_AllInfo          = 0x0000000040000000;
const TABOptions TABOpt_Once             = 0x0000000080000000;
const TABOptions TABOpt_ValCnvsOnce      = TABOpt_Once|TABOpt_ValCnvs;
const TABOptions TABOpt_StrListOnce      = TABOpt_Once|TABOpt_StrList;
const TABOptions TABOpt_AllInfoOnce      = TABOpt_Once|TABOpt_AllInfo;

const TABOptions TABOPT_DisplayOnly      = 0x0000001000000000;

//---------------------------------------------------------------------------

const LPSTR StdSepUnderbar = "________________________________________";
const LPSTR StdSepDash     = "----------------------------------------";
const LPSTR StdSepEquals   = "========================================";

//---------------------------------------------------------------------------
//data types in md_share0.h

inline flag IsData(byte iType)        { return (iType>=tt_Bool && iType <= tt_Strng); };
inline flag IsNumData(byte iType)     { return (iType>=tt_Bool && iType <= tt_Generic); };
inline flag IsIntData(byte iType)     { return (iType>=tt_Bool && iType <= tt_Long); };
inline flag IsUnSgnData(byte iType)   { return (iType>=tt_Byte && iType <= tt_DWord); };
inline flag IsFloatData(byte iType)   { return (iType>=tt_Flt16 && iType <= tt_Double); };
inline flag IsStrng(byte iType)       { return (iType>=tt_pChar && iType <= tt_Strng); };
inline flag IsStructure(byte iType)   { return (iType>=tt_Struct && iType <= tt_Array_E); };
inline flag IsStructStart(byte iType) { return (iType>=tt_Struct && iType <= tt_Array); };
inline flag IsStructEnd(byte iType)   { return (iType>=tt_Struct_E && iType <= tt_Array_E); };
inline flag IsBlockStart(byte iType)  { return (iType>=tt_Struct && iType <= tt_Object); };
inline flag IsBlockEnd(byte iType)    { return (iType>=tt_Struct_E && iType <= tt_Object_E); };
inline flag IsObjectStart(byte iType) { return (iType==tt_Object); };
inline flag IsObjectEnd(byte iType)   { return (iType==tt_Object_E); };
inline flag IsArrayStart(byte iType)  { return (iType==tt_Array); };
inline flag IsArrayEnd(byte iType)    { return (iType==tt_Array_E); };
inline flag IsLayout(byte iType)      { return (iType>=tt_RqdPage && iType<=tt_Text); };
inline byte StructEndType(byte iType) { switch (iType) {case tt_Struct: return tt_Struct_E;case tt_Element: return tt_Element_E;case tt_Object: return tt_Object_E;case tt_Array: return tt_Array_E;};return tt_NULL;};
inline byte IsBlob(byte iType)        { return (iType==tt_Blob); };

// Data Status            
const byte ds_OK          = 0;
const byte ds_UnderFlow   = 1;
const byte ds_OverFlow    = 2;
const byte ds_NAN         = 3;

// --------------------------------------------------------------------------

inline const LPCTSTR tt_TypeString(byte tt) 
  { 
  const TagObjTypeStruct *p=&TagObjTypeNames[0]; 
  while (p->Str) 
    if (p->t==tt) 
      return p->Str; 
    else p++; 
  return "?";
  };

inline const byte tt_StringType(LPCTSTR tStr) 
  { 
  const TagObjTypeStruct *p=&TagObjTypeNames[0]; 
  while (p->Str) 
    if (_stricmp(p->Str, tStr)==0) 
      return p->t; 
    else p++; 
  return tt_NULL;
  };

// --------------------------------------------------------------------------

//const dword MDD_Hidden  = 0x00000001;
//const dword MDD_Default = 0x00000002;
//NBNB MDDValueLst maps onto this struct
//struct DDBValueLst { long iVal; LPTSTR pStr; dword dwFlags; };
typedef MDDValueLst DDBValueLst;
  
class DllImportExport DDBValueLstMem
  {
  int Len;
  CArray <DDBValueLst, DDBValueLst&> Lst;
  Strng_List Str;

  public:
    DDBValueLstMem() { Len=0; };
    ~DDBValueLstMem() { };

    void Empty () { Len=0; Str.Clear(); };
    void Add(int i, LPCSTR S, dword flags=0) 
      { 
      Lst.SetSize(Max(Len+2, Lst.GetSize()));
      Str.Append(S);
      DDBValueLst D1={i, Str.Last()->Str(), flags};
      Lst.SetAtGrow(Len, D1);
      DDBValueLst D2={-1, NULL};
      Lst.SetAtGrow(Len+1, D2);
      Len++;
      };
    void Add(LPCSTR S, dword flags=0) { Add(Len, S, flags); };

    //    DDBValueLst* operator()(int i=0) { return &Lst[i]; };
    DDBValueLstMem* operator()() { return this; };
    DDBValueLst* List() { return &Lst[0]; };
    DDBValueLst* Item(int i=0) { return &Lst[i]; };
    DDBValueLst & operator[](int i) { return Lst[i]; };
    int Length() { return Len; };
    int Find(LPCSTR Str) 
      { 
      if (Str)
        for (int i=0; i<Len; i++)
          if (_stricmp(Lst[i].m_pStr, Str)==0)
            return i;
      return -1;
      };
    int Find(long Index) 
      { 
      for (int i=0; i<Len; i++)
        if (Lst[i].m_lVal == Index)
          return i;
      return -1;
      };
    DDBValueLstMem & operator=(const DDBValueLstMem & VL);
    DDBValueLstMem & operator=(const DDBValueLst & VL);

    void LoadFromCSV(Strng &S); 
    void SaveToCSV(Strng &S, bool QuoteBlanks); 
  };

extern DllImportExport DDBValueLst DDBOnOff[];
extern DllImportExport DDBValueLst DDBOnBlank[];
extern DllImportExport DDBValueLst DDBYesNo[];
extern DllImportExport DDBValueLst DDBYesBlank[];

extern DllImportExport int DDBAltArrayAddress;

extern DllImportExport DDBValueLst DDBNAN_NotSpecd[];
extern DllImportExport DDBValueLst DDBNAN_Floating[];
extern DllImportExport DDBValueLst DDBNAN_Ignore[];
extern DllImportExport DDBValueLst DDBNAN_Off[];
extern DllImportExport DDBValueLst DDBNAN_AtmosP[];
extern DllImportExport DDBValueLst DDBNAN_BlkPassP[];

// --------------------------------------------------------------------------
// All the Packed structures must have packing of 1 byte
#pragma pack(push, 1)  

// --------------------------------------------------------------------------
//
// The range of flt16 is approx 0.001 to 262144.0 with 3 and a bit significant digits
//  e=Exp-6; 
//  v=(Mant+0.5)/2048.0; 
//  if (e>0) 
//    v*=(1 << 2*e); 
//  else if (e<0) 
//    v/=(1 << 2*abs(e)); 
//
class DllImportExport flt16        // NBNB No virtual methods
  {
  public:
    word Exp:4, Mant:11, Sgn:1;
    operator       double() const;
    flt16          operator=(double v);
    static void    Test();
  };

// --------------------------------------------------------------------------

class DllImportExport DataUnionCom // NBNB No virtual methods
  {
  public:
    DataUnionCom() {iType=tt_NULL; iStatus=ds_OK; };

    byte           Type() const { return iType; };
    byte           Status() const { return iStatus; };
    void           SetStatus(byte Status) { iStatus=Status; };
    word           Size() const;
    word           TypeSize() const;
    flag           IsNAN() const;
    flag           IsZero() const;

    void           Set(DataUnionCom &V);

    flag           Equal(DataUnionCom &V);
    flag           EqualNANOK(DataUnionCom &V);

    long           GetLong() const;
    long           GetLong(CCnvIndex iCnv, char*pCnvTxt) const;
    void           PutLong(long L);
    void           PutLong(long L, CCnvIndex iCnv, char*pCnvTxt);
    double         GetDouble() const;
    double         GetDouble(CCnvIndex iCnv, char*pCnvTxt) const;
    void           PutDouble(double D);
    void           PutDouble(double D, CCnvIndex iCnv, char*pCnvTxt);
    char*          GetString(char*IFmt="%i", char*FFmt="%g", CCnvIndex iCnv=0, char*pCnvTxt=NULL) const;
    
    flag           SetTypeString(byte Type, char*pStrData, CCnvIndex iCnv=0, char*pCnvTxt=NULL);

  public:
    byte           iType:6, iStatus:2;
    union 
      {
      //byte         Bit; 
      byte         Byte; 
      word         Word;
      dword        DWord;
      int          Int;  
      char         Char;
      short        Short;
      long         Long; 
      float        Float;
      double       Double;
      flt16        Flt16;
      pchar        pChar;
      short        Len;    // Length of PkString
      };
  };

// --------------------------------------------------------------------------

class DllImportExport DataUnion : public DataUnionCom // NBNB No virtual methods
  {
  public:
    DataUnion()         {};

    DataUnion(char   V) {iType=tt_Char; Char=V;};
    DataUnion(byte   V) {iType=tt_Byte; Byte=V;};
    DataUnion(word   V) {iType=tt_Word; Word=V;};
    DataUnion(dword  V) {iType=tt_DWord; DWord=V;};
    DataUnion(int    V) {iType=tt_Int; Int=V;};
    DataUnion(short  V) {iType=tt_Short; Short=V;};
    DataUnion(long   V) {iType=tt_Long; Long=V;};
    DataUnion(float  V) {iType=tt_Float; Float=V;};
    DataUnion(double V) {iType=tt_Double; Double=V;};

    ~DataUnion()        { Clear(); };

    word           Size() const;

    void           Clear() {if (iType==tt_pChar) delete [] pChar; iType=tt_NULL;};
                   
    void           Set(byte   V) {Clear(); iType=tt_Byte; iStatus=ds_OK; Byte=V;};
    void           Set(word   V) {Clear(); iType=tt_Word; iStatus=ds_OK; Word=V;};
    void           Set(dword  V) {Clear(); iType=tt_DWord; iStatus=ds_OK; DWord=V;};
    void           Set(int    V) {Clear(); iType=tt_Int; iStatus=ds_OK; Int=V;};
    void           Set(char   V) {Clear(); iType=tt_Char; iStatus=ds_OK; Char=V;};
    void           Set(short  V) {Clear(); iType=tt_Short; iStatus=ds_OK; Short=V;};
    void           Set(long   V) {Clear(); iType=tt_Long; iStatus=ds_OK; Long=V;};
    void           Set(flt16  V) {Clear(); iType=tt_Flt16; iStatus=ds_OK; Flt16=V;};
    void           Set(float  V) {Clear(); iType=tt_Float; iStatus=ds_OK; Float=V;};
    void           Set(double V) {Clear(); iType=tt_Double; iStatus=ds_OK; Double=V;};
    void           Set(pchar  V) {Clear(); iType=tt_pChar; iStatus=ds_OK; pChar=new char[strlen(V)+1], strcpy(pChar, V);};

    void           Set(DataUnion &V);
    void           Set(PkDataUnion &V);

    flag           Equal(DataUnion &V);
    flag           Equal(PkDataUnion &V);
    flag           EqualNANOK(DataUnion &V);
    flag           EqualNANOK(PkDataUnion &V);

    long           GetLong() const;
    long           GetLong(CCnvIndex iCnv, char*pCnvTxt) const;
    void           PutLong(long L);
    void           PutLong(long L, CCnvIndex iCnv, char*pCnvTxt);
    double         GetDouble() const;
    double         GetDouble(CCnvIndex iCnv, char*pCnvTxt) const;
    void           PutDouble(double D);
    void           PutDouble(double D, CCnvIndex iCnv, char*pCnvTxt);
    char*          GetString(char*IFmt="%i", char*FFmt="%g", CCnvIndex iCnv=0, char*pCnvTxt=NULL) const;

    flag           SetTypeString(byte Type, char*pStrData, CCnvIndex iCnv=0, char*pCnvTxt=NULL);
    void           SetTypeDouble(byte Type, double d) {Clear(); iType = Type; PutDouble(d); }; 
    void           SetTypeDouble(byte Type, double d, CCnvIndex iCnv, char*pCnvTxt) { Clear(); iType = Type; PutDouble(d, iCnv, pCnvTxt); }; 
    void           SetTypeLong(byte Type, long l) {Clear(); iType = Type; PutLong(l); }; 
    void           SetTypeLong(byte Type, long l, CCnvIndex iCnv, char*pCnvTxt) { Clear(); iType = Type; PutLong(l, iCnv, pCnvTxt); }; 

    void           CopyTo(void* p); // KGA
  };

//========================================================================

const dword DataItemBuffSize=4096;

class DllImportExport PkDataUnion : public DataUnionCom // NBNB No virtual methods
  {
  public:
    PkDataUnion()         {iType=tt_NULL; };
    PkDataUnion(char   V) {iType=tt_Char; Char=V;};
    PkDataUnion(byte   V) {iType=tt_Byte; Byte=V;};
    PkDataUnion(word   V) {iType=tt_Word; Word=V;};
    PkDataUnion(dword  V) {iType=tt_DWord; DWord=V;};
    PkDataUnion(int    V) {iType=tt_Int; Int=V;};
    PkDataUnion(short  V) {iType=tt_Short; Short=V;};
    PkDataUnion(long   V) {iType=tt_Long; Long=V;};
    PkDataUnion(float  V) {iType=tt_Float; Float=V;};
    PkDataUnion(double V) {iType=tt_Double; Double=V;};

    PkDataUnion(pchar  V) {iType=tt_pChar; StringSet(V);};
    PkDataUnion(byte iType, char*pStrData, CCnvIndex iCnv=0, char*pCnvTxt=NULL) { SetTypeString(iType, pStrData, iCnv, pCnvTxt); };

    word           Size() const;

    void           Set(byte   V) {iType=tt_Byte; iStatus=ds_OK; Byte=V;};
    void           Set(word   V) {iType=tt_Word; iStatus=ds_OK; Word=V;};
    void           Set(dword  V) {iType=tt_DWord; iStatus=ds_OK; DWord=V;};
    void           Set(int    V) {iType=tt_Int; iStatus=ds_OK; Int=V;};
    void           Set(char   V) {iType=tt_Char; iStatus=ds_OK; Char=V;};
    void           Set(short  V) {iType=tt_Short; iStatus=ds_OK; Short=V;};
    void           Set(long   V) {iType=tt_Long; iStatus=ds_OK; Long=V;};
    void           Set(flt16  V) {iType=tt_Flt16; iStatus=ds_OK; Flt16=V;};
    void           Set(float  V) {iType=tt_Float; iStatus=ds_OK; Float=V;};
    void           Set(double V) {iType=tt_Double; iStatus=ds_OK; Double=V;};
    void           Set(pchar  V) {iType=tt_pChar; iStatus=ds_OK; StringSet(V);};
    
    void           Set(DataUnionCom &V);
    void           Set(DataUnion &V);
    void           Set(PkDataUnion &V);

//    flag           Equal(DataUnionCom &V);
//    flag           Equal(DataUnion &V);
    flag           Equal(PkDataUnion &V);


    long           GetLong() const;
    long           GetLong(CCnvIndex iCnv, char*pCnvTxt) const;
    void           PutLong(long L);
    void           PutLong(long L, CCnvIndex iCnv, char*pCnvTxt);
    double         GetDouble() const;
    double         GetDouble(CCnvIndex iCnv, char*pCnvTxt) const;
    void           PutDouble(double D);
    void           PutDouble(double D, CCnvIndex iCnv, char*pCnvTxt);
    char*          GetString(char*IFmt="%i", char*FFmt="%g", CCnvIndex iCnv=0, char*pCnvTxt=NULL) const;
    
    flag           SetTypeString(byte Type, char*pStrData, CCnvIndex iCnv=0, char*pCnvTxt=NULL);
    void           SetTypeDouble(byte Type, double d) { iType = Type; PutDouble(d); }; 
    void           SetTypeDouble(byte Type, double d, CCnvIndex iCnv, char*pCnvTxt) { iType = Type; PutDouble(d, iCnv, pCnvTxt); }; 
    void           SetTypeLong(byte Type, long l) { iType = Type; PutLong(l); }; 
    void           SetTypeLong(byte Type, long l, CCnvIndex iCnv, char*pCnvTxt) { iType = Type; PutLong(l, iCnv, pCnvTxt); }; 


    MTagIOType     TagType() const
      {
      switch (iType)
        {
        case tt_Byte: 
        case tt_Word: 
        case tt_DWord: 
        case tt_Int: 
        case tt_Char: 
        case tt_Short: 
        case tt_Long:
          return MTagType_Long; 
        
        case tt_Flt16: 
        case tt_Float: 
        case tt_Double: 
          return MTagType_Double; 
        
        case tt_pChar: 
        case tt_ppChar: 
        case tt_Strng: 
          return MTagType_String;
        
        default:
          return MTagType_Null;
        }
      }

  public:
    char           StringBuff[DataItemBuffSize];

    char *         String() const;
    void           StringSet(char * s);
    int            StringSize() const;

    void *         Blob();
    void           BlobSet(void * s, short iBlobLen);
    int            BlobSize() const;

  };

//------------------------------------------------------------------------

inline word DataUnion::Size() const
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

inline void DataUnion::Set(PkDataUnion &V)
  {
  if (IsStrng(V.iType))
    {
    Set(V.String());
    iType=V.iType;
    iStatus=V.iStatus;
    //iType=V.iType;
    }
  else
    DataUnionCom::Set(V);
  };

inline flag DataUnion::Equal(DataUnion &V)
  {
  if (iType!=V.iType)
    return false;
  if (IsStrng(iType))
    {
    return strcmp(pChar, V.pChar)==0;
    }
  return DataUnionCom::Equal(V);
  };

inline flag DataUnion::Equal(PkDataUnion &V)
  {
  if (iType!=V.iType)
    return false;
  if (IsStrng(iType))
    {
    return strcmp(pChar, V.String())==0;
    }
  return DataUnionCom::Equal(V);
  };
inline flag DataUnion::EqualNANOK(DataUnion &V)
  {
  if (iType!=V.iType)
    return false;
  if (IsStrng(iType))
    {
    return strcmp(pChar, V.pChar)==0;
    }
  return DataUnionCom::EqualNANOK(V);
  };

inline flag DataUnion::EqualNANOK(PkDataUnion &V)
  {
  if (iType!=V.iType)
    return false;
  if (IsStrng(iType))
    {
    return strcmp(pChar, V.String())==0;
    }
  return DataUnionCom::EqualNANOK(V);
  };

//------------------------------------------------------------------------

inline long DataUnion::GetLong() const
  {
  if (IsStrng(iType))
    {
    VERIFY(0);
    return 0;
    }
  else 
    return DataUnionCom::GetLong();
  };

inline long DataUnion::GetLong(CCnvIndex iCnv, char*pCnvTxt) const
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

inline void DataUnion::PutLong(long L)
  {
  if (IsStrng(iType))
    {
    VERIFY(0);
    }
  else 
    DataUnionCom::PutLong(L);
  };

inline void DataUnion::PutLong(long L, CCnvIndex iCnv, char*pCnvTxt)
  {
  if (IsStrng(iType))
    {
    VERIFY(0);
    }
  else 
    DataUnionCom::PutLong(L, iCnv, pCnvTxt);
  };

//------------------------------------------------------------------------

inline double DataUnion::GetDouble() const
  {
  if (IsStrng(iType))
    {
    VERIFY(0);
    return 0.0;
    }
  else 
    return DataUnionCom::GetDouble();
  };

inline double DataUnion::GetDouble(CCnvIndex iCnv, char*pCnvTxt) const
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

inline void DataUnion::PutDouble(double D)
  {
  if (IsStrng(iType))
    {
    VERIFY(0);
    }
  else 
    DataUnionCom::PutDouble(D);
  };

inline void DataUnion::PutDouble(double D, CCnvIndex iCnv, char*pCnvTxt)
  {
  if (IsStrng(iType))
    {
    VERIFY(0);
    }
  else 
    DataUnionCom::PutDouble(D, iCnv, pCnvTxt);
  };

//------------------------------------------------------------------------

inline char* DataUnion::GetString(char*IFmt/*="%i"*/, char*FFmt/*="%g"*/, CCnvIndex iCnv/*=0*/, char*pCnvTxt/*=NULL*/) const
  {
  if (IsStrng(iType))
    return pChar;
  else 
    return DataUnionCom::GetString(IFmt, FFmt, iCnv, pCnvTxt);
  }

//------------------------------------------------------------------------

inline flag DataUnion::SetTypeString(byte iTypeRqd, char*pStrData, CCnvIndex iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  if (IsStrng(iTypeRqd))
    {
    Set(pStrData);
    iType=iTypeRqd;
    iStatus=ds_OK;
    }
  else 
    DataUnionCom::SetTypeString(iTypeRqd, pStrData, iCnv, pCnvTxt);
  return True;
  }

//========================================================================

inline char * PkDataUnion::String() const
  { 
  return (char*)&Len+sizeof(Len);
  };

//------------------------------------------------------------------------

inline void PkDataUnion::StringSet(char * s)  
  { 
  Len = strlen(s); 
  if (Len>sizeof(StringBuff)-8) 
    {
    Len=sizeof(StringBuff)-8;
    char * p=String();
    strncpy(p, s, Len); 
    p[Len]=0;
    LogError("Utils", 0, "StringSet - String truncated [%-80.80s]", p);
    }
  else
    strcpy(String(), s); 
  };

//------------------------------------------------------------------------

inline int PkDataUnion::StringSize() const
  { 
  return sizeof(Len)+Len+1; 
  };

//------------------------------------------------------------------------

inline void * PkDataUnion::Blob()     
  { 
  return (char*)&Len+sizeof(Len);
  };

//------------------------------------------------------------------------

inline void PkDataUnion::BlobSet(void * s, short iBlobLen)  
  { 
  Len = iBlobLen; 
  ASSERT (Len<sizeof(StringBuff)); 
  memcpy(Blob(), s, iBlobLen); 
  };

//------------------------------------------------------------------------

inline int PkDataUnion::BlobSize() const 
  { 
  return sizeof(Len)+Len+1; 
  };

//------------------------------------------------------------------------

inline word PkDataUnion::Size()  const
  {
  if (IsStrng(iType) || IsStructure(iType) || IsLayout(iType) || iType==tt_Generic)
    return StringSize()+sizeof(byte);
  else if (IsBlob(iType))
    return BlobSize()+sizeof(byte);
  else 
    return DataUnionCom::Size();
  };

//------------------------------------------------------------------------

inline void PkDataUnion::Set(DataUnionCom &V)
  {
  DataUnionCom::Set(V);
  };

//------------------------------------------------------------------------

inline void PkDataUnion::Set(DataUnion &V)
  {
  if (IsStrng(V.iType))
    {
    StringSet(V.pChar);
    iType=V.iType;
    iStatus=V.iStatus;
    //iType=V.iType;
    }
  else
    DataUnionCom::Set(V);
  };

//------------------------------------------------------------------------

inline void PkDataUnion::Set(PkDataUnion &V)
  {
  if (IsStrng(V.iType))
    {
    StringSet(V.String());
    iType=V.iType;
    iStatus=V.iStatus;
    //iType=V.iType;
    }
  else if (IsBlob(V.iType))
    {
    BlobSet(V.Blob(), V.BlobSize());
    iType=V.iType;
    iStatus=V.iStatus;
    //iType=V.iType;
    }
  else
    DataUnionCom::Set(V);
  };

//------------------------------------------------------------------------

inline long PkDataUnion::GetLong()  const
  {
  if (IsStrng(iType))
    {
    VERIFY(0);
    return 0;
    }
  else 
    return DataUnionCom::GetLong();
  };

inline long PkDataUnion::GetLong(CCnvIndex iCnv, char*pCnvTxt)  const
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

inline void PkDataUnion::PutLong(long L)
  {
  if (IsStrng(iType))
    {
    VERIFY(0);
    }
  else 
    DataUnionCom::PutLong(L);
  };

inline void PkDataUnion::PutLong(long L, CCnvIndex iCnv, char*pCnvTxt)
  {
  if (IsStrng(iType))
    {
    VERIFY(0);
    }
  else 
    DataUnionCom::PutLong(L, iCnv, pCnvTxt);
  };

//------------------------------------------------------------------------

inline double PkDataUnion::GetDouble() const
  {
  if (IsStrng(iType) || IsBlob(iType))
    {
    VERIFY(0);
    return 0.0;
    }
  else 
    return DataUnionCom::GetDouble();
  };

inline double PkDataUnion::GetDouble(CCnvIndex iCnv, char*pCnvTxt) const
  {
  if (IsStrng(iType) || IsBlob(iType))
    {
    VERIFY(0);
    return 0.0;
    }
  else 
    return DataUnionCom::GetDouble(iCnv, pCnvTxt);
  };

//------------------------------------------------------------------------

inline void PkDataUnion::PutDouble(double D)
  {
  if (IsStrng(iType) || IsBlob(iType))
    {
    VERIFY(0);
    }
  else 
    DataUnionCom::PutDouble(D);
  };

inline void PkDataUnion::PutDouble(double D, CCnvIndex iCnv, char*pCnvTxt)
  {
  if (IsStrng(iType) || IsBlob(iType))
    {
    VERIFY(0);
    }
  else
    DataUnionCom::PutDouble(D, iCnv, pCnvTxt);
  };

//------------------------------------------------------------------------

inline char* PkDataUnion::GetString(char*IFmt/*="%i"*/, char*FFmt/*="%g"*/, CCnvIndex iCnv/*=0*/, char*pCnvTxt/*=NULL*/) const
  {
  if (IsStrng(iType) || IsStructure(iType) || IsLayout(iType) || iType==tt_Generic)
    {
    if (IsStrng(iType) && IsTimeCnv(iCnv))
      {
      CTimeValue V(String());
      return (LPTSTR)V.Format(pCnvTxt);
      }
    return String();//?pChar;
    }
  else 
    return DataUnionCom::GetString(IFmt, FFmt, iCnv, pCnvTxt);
  }

//------------------------------------------------------------------------

inline flag PkDataUnion::SetTypeString(byte iTypeRqd, char*pStrData, CCnvIndex iCnv/*=0*/, char*pCnvTxt/*=NULL*/)
  {
  if (IsStrng(iTypeRqd) || IsStructure(iTypeRqd) || IsLayout(iTypeRqd) || iTypeRqd==tt_Generic)
    {
    StringSet(pStrData ? pStrData : "");
    iType=iTypeRqd;
    iStatus=ds_OK;
    }
  else 
    DataUnionCom::SetTypeString(iTypeRqd, pStrData, iCnv, pCnvTxt);
  return True;
  }

inline flag PkDataUnion::Equal(PkDataUnion &V)
  {
  if (iType!=V.iType)
    return false;
  if (IsStrng(iType))
    {
    return strcmp(String(), V.String())==0;
    }
  return DataUnionCom::Equal(V);
  };
// --------------------------------------------------------------------------

typedef UCHAR CTagRefStatus;

#if WITHEQUIPSPECS
typedef UCHAR CTagValueStatus;
#endif

// --------------------------------------------------------------------------

class DllImportExport PkCharArrayHdr
  {
  public:
    LPSTR   m_pStart;
    long    m_iLen;

    PkCharArrayHdr()
      {
      m_pStart  = NULL;
      m_iLen    = 0;
      }
    PkCharArrayHdr(const PkCharArrayHdr & n)
      {
      m_pStart  = n.m_pStart;
      m_iLen    = n.m_iLen;
      }
    PkCharArrayHdr(LPSTR pStart, long iLen)
      {
      m_pStart  = pStart;
      m_iLen    = iLen;
      }
    PkCharArrayHdr & operator=(const PkCharArrayHdr &n)
      {
      m_pStart  = n.m_pStart;
      m_iLen    = n.m_iLen;
      return *this;
      }
    BOOL AFXAPI operator==(const PkCharArrayHdr &X) const
      {
      return m_iLen==X.m_iLen && memcmp(m_pStart, X.m_pStart, m_iLen)==0;
      }
  };

// --------------------------------------------------------------------------

template<> inline UINT AFXAPI HashKey(PkCharArrayHdr & Key)
  { 
  UINT nHash = 0;
  char * KC=Key.m_pStart;
  for (int i=0; i<Key.m_iLen; i++)
	  nHash = (nHash<<5) + nHash + *KC++;
	return nHash;
  };

template<> inline BOOL AFXAPI CompareElements(const PkCharArrayHdr * E1, const PkCharArrayHdr * E2)
  { 
  return (E1->m_iLen==E2->m_iLen) && (memcmp(E1->m_pStart, E2->m_pStart, E1->m_iLen)==0);
  };

//========================================================================

////const dword PkDataListLength=0x40000; // 0x20000 // CNM
////const dword PkDataListLength=0x48000; // 0x40000 // KGA : 29/10/02
////const dword PkDataListLength=0x60000; // 0x48000 // KGA : 9/10/03 buffer still not big enough for large RTTS projects!!!
//const dword PkDataListLength=0x70000; // 0x60000 // KGA : 25/1/04 buffer still not big enough for large RTTS projects!!!

const word PDI_Flags       = 0x0001;      //      3
const word PDI_Value       = 0x0002;      // was  1
const word PDI_Tag         = 0x0004;      //      2
const word PDI_TagOfParent = 0x0008;      //      4
const word PDI_CnvInfo     = 0x0010;      //      9
const word PDI_NElements   = 0x0020;      //      7
const word PDI_CnvInfo2    = 0x0040;      // for enlarged  CnvIndex
// Extras
const word PDI_XRefStatus  = 0x0080;      // 
const word PDI_Range       = 0x0100;      //      8
const word PDI_StrList     = 0x0200;      //      10
const word PDI_Description = 0x0400;      //      5
const word PDI_TagComment  = 0x0800;      //      6
const word PDI_SubClass    = 0x1000;      //      
const word PDI_PrimaryCfg  = 0x2000;      //      
const word PDI_Flags64     = 0x8000;      // Marks 64 bit Flags      

class DllImportExport CPkDataItemOld; 
class DllImportExport CPkDataItem 
  {
  public:
    CPkDataItem();
    virtual ~CPkDataItem();
    void Clear();

    static CPkDataItem * Create(const CPkDataItem & C);
    static CPkDataItem * Create(const CPkDataItemOld & C);
    static CPkDataItem * Create(long Size);
  
    word           ItemSize() const { return m_wSize+((LPSTR)m_Data-(LPSTR)&m_wContents); };
    word           DataSize() const { return m_wSize; };
    word           MaxDataSize() const { return m_wMaxSize; };
    LPSTR          Buffer() const { return (LPSTR)&m_Data[0]; };

    bool           operator==(const CPkDataItem &C);
    bool           operator!=(const CPkDataItem &C);
    CPkDataItem &  operator=(const CPkDataItem &C);

    void           AppendValue( DDEF_Flags iFlags, PkDataUnion &PData);
    void           AppendTag( char * pTag, char * pSym);
    void           AppendTagOfParent( char * pTag);
    void           AppendCnvInfo( CCnvIndex iCnv, char * pCnvTxt, char *pCnvFam);
    void           AppendNElements( DDEF_NElements nElems);
#if WITHEQUIPSPECS
    void           AppendItemStatus( CTagRefStatus S, CTagValueStatus V);
#else
    void           AppendItemStatus( CTagRefStatus S);
#endif
    void           AppendRange( DataUnion &DMin, DataUnion &DMax);
    void           AppendStrListItem( char * pStr, long Index, flag Indexed, pword &pStrLstLen);
    void           AppendDescription( char * pDesc);
    void           AppendTagComment( char * pTagComment);
    void           AppendSubClass( char * pSubClass);
    void           AppendPrimaryCfg( char * pPrimaryCfg);

    flag           Empty() {return (m_wContents==0);};
    flag           Contains(dword What) {return ((m_wContents & What)!=0);};

    void*          AddressOf(word what);
    byte           Type();
    PkDataUnion*   Value();
    char*          Class();      // Class String in Value.String
    char*          SubClass();   // SubClass String in Value.String
    char*          PrimaryCfg(); // PrimaryCfg String in Value.String
    char*          SymOrTag();
    char*          TagStr();
    char*          SymStr();
    DDEF_Flags     Flags();
    char*          TagOfParent();
    char*          Description();
    char*          TagComment();
    DDEF_NElements NElements();
    CTagRefStatus  GetTagRefStatus();
#if WITHEQUIPSPECS
    CTagValueStatus GetTagValueStatus();
#endif
    flag           GetRange(DataUnion &DMin, DataUnion &DMax);
    flag           GetRange(pPkDataUnion &pDMin, pPkDataUnion &pDMax);
    CCnvIndex      CnvIndex();
    char*          CnvTxt();
    char*          CnvFam();
    flag           IndexedStrList();
    flag           GetStrList(Strng_List & Values, flag & Indexed);
    flag           GetStrList(Strng_List & Values) { flag Indexed; return GetStrList(Values, Indexed); }; 

    void           AppendLayout( byte iType, pchar pTag, DDEF_Flags iFlags);
    void           AppendStructureS( byte iType, pchar pTag, pchar pClass, pchar pSubClass, pchar pPrimaryCfg, DDEF_Flags iFlags, pchar pTagOfParent, pchar pDescription, pchar pTagComment, int nElem);
    void           AppendStructureE( byte iType, pchar pTag, DDEF_Flags iFlags);
    void           AppendDataValue( TABOptions Opts, pchar pTag, pchar pSym, DDEF_Flags iFlags, PkDataUnion &PData, CCnvIndex iCnv=0, char *pCnvTxt="", char *pCnvFam=""); 
    void           AppendBlob( pchar pTag, void * pBlob, short iBlobLen);
    void           AppendValueLst( DDBValueLst *pValueLst, flag Indexed); 

    int            CompactAStr( LPSTR pStr);
    LPSTR          ExtractAStr(LPSTR p);
    LPSTR          SkipAStr(LPSTR p);
    inline LPSTR   CurrentPtr() { return &m_Data[m_wSize]; }

  public:
    word           m_wContents;
    word           m_wSize;
    word           m_wMaxSize;
    char           m_Data[4];
  };

// --------------------------------------------------------------------------

class DllImportExport CPkDataItemOld
  {
  friend class CPkDataList;
  public:
    word           m_wContents;
    word           m_wSize;
    char           m_cData[1024];
  };

// --------------------------------------------------------------------------

class DllImportExport CPkDataIter
  {
  public:
    CPkDataList  * m_pList;
    POSITION       m_RdPos;
    bool           m_bEOF;

    CPkDataIter() { m_pList=NULL; m_RdPos=0; m_bEOF=false; };
  };

class DllImportExport CPkDataList : public CList <CPkDataItem*, CPkDataItem*> //
  {
  friend class CXM_ObjectData;
  private:

    CPkDataItem   &m_WorkItem;

  public:
    CPkDataList();
    ~CPkDataList();
    void           Clear();
    CPkDataItem   *FirstItem();
    CPkDataItem   *FirstItem(CPkDataIter & Iter);
    CPkDataItem   *CurrentItem(CPkDataIter & Iter);
    CPkDataItem   *NextItem(CPkDataIter & Iter);
    void           AdvanceWrite();
    bool           EndOfList(CPkDataIter & Iter);

    void           SetStructureS(byte iType, pchar pTag, pchar pClass, pchar pSubClass, pchar pPrimaryCfg, DDEF_Flags iFlags, pchar pTagOfParent, pchar pDescription, pchar pTagComment, int nElem);
    void           SetStructureE(byte iType, pchar pTag, DDEF_Flags iFlags);
    void           SetDataValue(pchar pTag, DDEF_Flags iFlags, PkDataUnion &PData); 
    
    void           SetStructureS(byte iType, pchar pTag, pchar pClass, pchar pSubClass, pchar pPrimaryCfg, pchar pTagOfParent, pchar pDescription, pchar pTagComment, int nElem)
      { SetStructureS(/*pPItem,*/ iType, pTag, pClass, pSubClass, pPrimaryCfg, 0, pTagOfParent, pDescription, pTagComment, nElem); };
    void           SetStructureE(byte iType, pchar pTag)
      { SetStructureE(/*pPItem,*/ iType, pTag, 0); };
    void           SetDataValue(pchar pTag, PkDataUnion &PData) 
      { SetDataValue(/*pPItem,*/ pTag, 0, PData); };
    void           SetDataValueAll(pchar pTag, pchar pTagComment, PkDataUnion &PData, DDEF_Flags iFlags,
                                   DataUnion &DataMin, DataUnion &DataMax, CCnvIndex iCnv, pchar pCnvTxt, pchar pCnvFam, 
                                   pStrng_List pValueList, flag bIndexedStr, pchar pDesc); 
    void           SetBlob(pchar pTag, void * pBlob, short iBlobLen); 

    void dbgDump(flag Full, char* pWhere);

    CPkDataItem   & getItem() { return m_WorkItem; };
    __declspec(property(get=getItem)) CPkDataItem   & Item;

  };

//------------------------------------------------------------------------

inline void CPkDataList::Clear()                                  { while (!IsEmpty()) delete RemoveHead(); }
inline bool CPkDataList::EndOfList(CPkDataIter & Iter)            { return Iter.m_bEOF!=0;  };

inline CPkDataItem* CPkDataList::FirstItem(CPkDataIter & Iter) 
  { 
  Iter.m_pList=this;
  Iter.m_RdPos=GetHeadPosition();
  Iter.m_bEOF=(Iter.m_RdPos==NULL);
  return Iter.m_RdPos ? GetAt(Iter.m_RdPos):NULL;
  }; 

inline CPkDataItem* CPkDataList::FirstItem() 
  { 
  POSITION RdPos=GetHeadPosition();
  return RdPos ? GetAt(RdPos):NULL;
  }; 

inline CPkDataItem* CPkDataList::CurrentItem(CPkDataIter & Iter) 
  { 
  if (Iter.m_pList!=this)
    return FirstItem(Iter);
  return !Iter.m_bEOF && Iter.m_RdPos ? GetAt(Iter.m_RdPos) : NULL;
  }; 

inline CPkDataItem* CPkDataList::NextItem(CPkDataIter & Iter) 
  { 
  if (Iter.m_pList!=this)
    return FirstItem(Iter);
  if (!Iter.m_bEOF)
    {
    if (Iter.m_RdPos)
      GetNext(Iter.m_RdPos);
    Iter.m_bEOF=(Iter.m_RdPos==NULL);
    return Iter.m_RdPos ? GetAt(Iter.m_RdPos) : NULL;
    }
  return NULL; 
  };

//// --------------------------------------------------------------------------
//// Pop Packed structures Packing
#pragma pack(pop)  
//// --------------------------------------------------------------------------

#undef DllImportExport

#endif