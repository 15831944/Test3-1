
#pragma once

//**************************************************************************
//
//
//
//**************************************************************************
//
//  Filename   :  PLC5Em
//  $Author    :  Cnm
//
//**************************************************************************

typedef unsigned char byte;
typedef unsigned short word;

#pragma pack (1)
#define WithTestXCode  0
#define WithFRC        01

#if _MSC_VER >= 1300
#define DotNetAfxTempl 1
#else
#define DotNetAfxTempl 0
#endif

#define INCOMPLETECODE()     { ASSERT(FALSE); };//char Buff[2000]; sprintf(Buff, "INCOMPLETECODE\n\n%s[%i]", __FILE__, __LINE__); DoAssert1(Buff); DoBreak() ; }

// ===========================================================================
//  . Net stuff
// ===========================================================================
#if DotNetAfxTempl 

//#pragma warning(disable : 4995) // .Net deprecated
//#pragma warning(disable : 4996) // .Net deprecated
//#pragma warning(disable : 4244) // Loss of data

template <class TYPE> class CSmartPtrAllocate
  {
  public:
    CSmartPtrAllocate()
      {
      m_pI = NULL;
      Alloc();
      }

    ~CSmartPtrAllocate()
      {
      Release();
      }

    void  Alloc()
      {
      if(NULL == m_pI)
        {
        m_pI = new TYPE;
        }
      }
    void  Release()
      {
      if(NULL != m_pI)
        {
        //  Make sure to clear the local variable so it does not come
        //  here again on the destructor if explicitly released.
        TYPE * pTemp = m_pI;
        pTemp->TYPE::~TYPE();
        m_pI = NULL;
        }
      }

    //    Operators
    TYPE * operator->() const { return m_pI; }
    TYPE & operator*()  const { return *m_pI; }
    TYPE * operator&()  const { return m_pI; }
    TYPE * operator=(const TYPE* X) { m_pI=X; return m_pI; }
    bool operator==(const TYPE* X) const { return m_pI==X; }
    bool IsNull() const { return m_pI==NULL; }

    TYPE * Ptr()  const { return m_pI; }

    operator CSmartPtrAllocate<TYPE>()  { return *this; }

  protected:
    TYPE *   m_pI;
  };

#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

const int MinConnWide   = 3;
const int MinFuncWide   = 8;
const int ConnectWide   = 3;

const int MaxJSRParms   = 8;
const int MaxTables     = 1001;
const int MaxTablesList = 256;

extern FILE * dbgf;

//===========================================================================
//
//
//
//===========================================================================

const TT_Null          = 0;
const TT_Numeric       = 1;
const TT_Alpha         = 2;
//const TT_AlphaNum      = 3;
const TT_Other         = 3;
const TT_Code          = 4;

const int BigBuffSize=4096;
class CTknFile
  {
  public:
    char    m_Buff[BigBuffSize];
    int     m_Pos;
    int     m_Len;
    //char *  m_pTkn;
    char *  m_PrvTkn;
    bool    m_UsePrv;
    int     m_LineNo;
    byte    m_TknType;

    CTknFile()
      {
      m_Pos=0;
      m_Len=0;
      m_Buff[0]=0;
      m_UsePrv=false;
      m_PrvTkn=NULL;
      m_LineNo=0;
      m_TknType=TT_Null;
      };

    char *  operator()(int i=0) { return &m_Buff[i]; };
    long    ReadALine(FILE *f);
    long    Length(){return m_Len;};
    char *  StrToken();
    char *  StrQToken();
    long    IntToken();
    float   FltToken();
    //char *  Current() { return m_pTkn; };
    void    PushToken() { m_UsePrv=true; };

    byte    Type()   { return m_TknType; }
    bool    TypeIs(byte Type) { return m_TknType==Type; }

  };

//===========================================================================
//
//
//
//===========================================================================

const int MaxLadders=64;
const int MaxFnRows=32;
const int MaxFnCols=32;

enum LoadTypes {Load_Nothing, Load_Input, Load_All};

//===========================================================================

template <class T> T Min(const T x, const T y) { return (x < y) ? x : y; };
template <class T> T Max(const T x, const T y) { return (x > y) ? x : y; };
template <class T> T Sqr(const T x) { return (x * x); };
template <class T> T Sqrt(const T x) { return sqrt(x); };
template <class T> T Range(const T n, const T x, const T m) { return (x >= n) ? ((x <= m) ? x : m ) : n; };

//===========================================================================

inline void CheckThat(bool OK, const char * Msg)
  {
  if (!OK)
    {
    CString S;
    S.Format("Problem : can not continue\n\n%s",Msg);
    TRACE("%s\n", S);
//    AfxMessageBox(S);
    //Sleep(10000);
    INCOMPLETECODE();
    }
  }

//===========================================================================
//
//
//
//===========================================================================

class CExpression;
class CAddress;
class CTable;
class CAddInfo;
class CTableRoot;
class CPLC5;
class CRungImage;
class CMtxRow;
class CMtxElement;
class CTable;
class CLadder;
class CRung;
class CPLC5;
class CFnInfoItem;

//===========================================================================
//
//
//
//===========================================================================

class CChgHistory
  {
  public:
    long  m_Position;
    long  m_Length;
    CStringArray m_Strings;
    long  m_nIterNo;
    long  m_nIterNoOut;

    CChgHistory();
    long Length();
    void Add(LPCTSTR Str);
    LPCTSTR Value(long Pos);
    void DumpChange(bool Val, CAddress & Add, CRung * pRung, LPCTSTR pText);
    void DumpChange(bool Val, CExpression & Exp, CRung * pRung, LPCTSTR pText);
  protected:
    void AddIt(LPCTSTR Str);
  };

extern CChgHistory m_History;

//===========================================================================
//
//
//
//===========================================================================

typedef CTable * CTablePtr;

class CTableRoot
  {
  public:
    const char *  m_sRootName;
    short         m_TbNumDef,
                  m_TbNumLo,
                  m_TbNumHi;

    bool          m_OctalAdd;
    bool          m_IsFlt;
    bool          m_HexDispLoad;
    VARTYPE       m_VarType;

    int           m_ValsPerColumn;
    int           m_ColumnWidth;
    int           m_nStructVals;

    const char *  m_cReqdFmt;
    const char *  m_pLoadFmt;

    // Must Be At End
//    int         m_nTables;
//    CTablePtr * m_Tables;
  };

//CTablePtr * sm_Tables[MaxTables];

//===========================================================================
//
//
//
//===========================================================================

union CUIndAdd
  {
  int         m_iIndex;
  CAddress *  m_pAdd;
  };

class CAddress
  {
  protected:

  public:
    CPLC5     * m_Plc;
    CString     m_Txt;
    CUIndAdd    m_uFile;
    CUIndAdd    m_uWord;
    CUIndAdd    m_uBit;
    int         m_iOffInStruct;
    byte        m_IsBitAddRelToWrd:1,
                m_IsAddOfBit:1;
    byte        m_bIndFileNo,
                m_bIndWordNo,
                m_bIndBitNo,
                m_bIndexed:1,
                m_bSymbolic:1,
                m_IsValid:1;
    CAddInfo *  m_pInfo;
    CAddress(CPLC5 * Plc, LPCTSTR Tag=NULL, bool Complain=true)
      {
      m_Plc=Plc;
      //m_pTblRt=NULL;
      m_uFile.m_iIndex=-1;
      m_uWord.m_iIndex=-1;
      m_uBit.m_iIndex=-1;
      m_iOffInStruct=0;

      m_IsBitAddRelToWrd=0;
      m_IsAddOfBit=0;
      m_bIndFileNo=0;
      m_bIndWordNo=0;
      m_bIndBitNo=0;
      m_bIndexed=0;
      m_bSymbolic=0;
      m_pInfo=NULL;
      m_IsValid=false;
      if (Tag)
        Parse(Tag, Complain);
      };
    ~CAddress()
      {
      if (m_bIndFileNo) delete m_uFile.m_pAdd;
      if (m_bIndWordNo) delete m_uWord.m_pAdd;
      if (m_bIndBitNo) delete m_uBit.m_pAdd;
      };

    LPCTSTR ParseNo(LPCTSTR pTxt, CUIndAdd & uNo, byte &IsInd, int Default, CTableRoot * pTblRt, const char * Terms);
    bool    Parse(LPCTSTR Txt, bool Complain=true);
    bool    CheckValidAddress();
    bool    ValidAddress() { return m_IsValid; };

    LPCTSTR Text() { return m_Txt; };

    VARTYPE VarType();

    int   FileNo();
    int   WordNo(bool AddIndex);
    int   BitNo(bool AddIndex);

    CTable * Table();
    CAddInfo* CAddress::SymbolF(LPCTSTR pAddress);
    CAddInfo* CAddress::AddressF(LPCTSTR pSymbol);

    short BitMsk(bool AddIndex) { return 1 << BitNo(AddIndex); };

    bool  BValue();
    bool  IValueAsBit();
    short IValue();
    float FValue();

    short IValue(int iShift);
    float FValue(int iShift);
    short SetIValue(int iShift, short S);
    float SetFValue(int iShift, float F);

    bool  BValueFrc();
    short IValueFrc();
    bool  IsBValueFrc();
    bool  IsIValueFrc();

    bool IsFlt();// { return Table()->IsFlt(); };
    bool IsBit() { return m_uBit.m_iIndex>=0; };

    bool  SetBValue(bool On, CRung * pRung=NULL, LPCTSTR pText="");
    short SetIValue(short S);
    float SetFValue(float F);

    void  SetBValueFrc(bool On);
    void  SetIValueFrc(short S);
    void  ClrBValueFrc();
    void  ClrIValueFrc();

    void * StructAddress();

    float ValueAsFlt();
    void  SetValueAsFlt(float Val);

    float ValueAsFlt(int iOff);
    void  SetValueAsFlt(int iOff, float Val);

    int     DescCount();
    LPCTSTR DescText(int i);

  };

//===========================================================================
//
//
//
//===========================================================================

enum AOC_Types {AOC_Null, AOC_Int, AOC_Flt, AOC_Add };

class CAddOrImmed
  {
  public:
    CPLC5     * m_Plc;
    CString     m_Txt;
    AOC_Types   m_What;
    union {
      short     m_Int;
      float     m_Flt;
      CAddress *m_pAdd;
      };

    CAddOrImmed(CPLC5 * Plc=NULL)
      {
      m_Plc=Plc;
      m_What=AOC_Null;
      }
    ~CAddOrImmed()
      {
      if (m_What==AOC_Add)
        delete m_pAdd;
      }

    void Parse(char * pTxt)
      {
      m_Txt=pTxt;
      if (isdigit(pTxt[0]) || pTxt[0]=='-' || pTxt[0]=='+')
        {
        if (strchr(pTxt, '.'))
          {
          m_What=AOC_Flt;
          m_Flt=(float)atof(pTxt);
          }
        else
          {
          m_What=AOC_Int;
          m_Int=(short)atol(pTxt);
          }
        }
      else
        {
        m_What=AOC_Add;
        m_pAdd=new CAddress(m_Plc);
        m_pAdd->Parse(pTxt);
        }
      }
    const char * Text() { return m_Txt; };
    CAddress* Address() { return m_What==AOC_Add ? m_pAdd : NULL; };

    float ValueAsFlt()
      {
      switch (m_What)
        {
        case AOC_Add: return m_pAdd->ValueAsFlt();
        case AOC_Flt: return m_Flt;
        case AOC_Int: return m_Int;
        }
      CheckThat(false, "Unknown CAddOrImmed Parm Type");
      return 0;
      };

  };

//===========================================================================
//
//
//
//===========================================================================

// InOrder of Precedence - Highest Last
enum XO_OpCodes {
  XO_Null,
  XO_FConst, XO_IConst, XO_Address,

  XO_SQR, XO_LN, XO_LOG, XO_ACS, XO_ASN, XO_ATN, XO_SIN, XO_COS, XO_TAN,

  XO_FRD, XO_TOD, XO_DEG, XO_RAD,

  XO_OBrace, XO_CBrace,

  XO_GT, XO_GE, XO_EQ, XO_NE, XO_LE, XO_LT,

  XO_Or, XO_XOr, XO_And, XO_BOr, XO_BXOr, XO_BAnd,

  XO_Plus, XO_Minus,

  XO_Mult, XO_Div,

  XO_Exp,

  XO_Not, XO_BNot,

  };

// InFix Operators
enum XIF_Types  { XIF_Null, XIF_IdConst, XIF_IdVar, XIF_OBrace, XIF_CBrace, XIF_Operator, XIF_Funct, XIF_Cmp, XIF_Not };

const byte XM_CMP =0x01;
const byte XM_CPT =0x02;
const byte XM_FAL =0x04;

struct CXSymbol
  {
  const char * m_Txt;
  XO_OpCodes  m_Op;
  XIF_Types   m_InFix;
  byte        m_FnMsk;
  };

static CXSymbol XSymbols []=
  {
    {"SQR", XO_SQR,     XIF_Funct,     XM_CMP|XM_CPT|XM_FAL,  },
    {"LN",  XO_LN ,     XIF_Funct,     XM_CMP|XM_CPT,         },
    {"LOG", XO_LOG,     XIF_Funct,     XM_CMP|XM_CPT,         },
    {"ACS", XO_ACS,     XIF_Funct,     XM_CMP|XM_CPT,         },
    {"ASN", XO_ASN,     XIF_Funct,     XM_CMP|XM_CPT,         },
    {"ATN", XO_ATN,     XIF_Funct,     XM_CMP|XM_CPT,         },
    {"SIN", XO_SIN,     XIF_Funct,     XM_CMP|XM_CPT,         },
    {"COS", XO_COS,     XIF_Funct,     XM_CMP|XM_CPT,         },
    {"TAN", XO_TAN,     XIF_Funct,     XM_CMP|XM_CPT,         },
    {"AND", XO_And,     XIF_Funct,     XM_CMP|XM_CPT|XM_FAL,  },
    {"OR",  XO_Or ,     XIF_Funct,     XM_CMP|XM_CPT|XM_FAL,  },
    {"XOR", XO_XOr,     XIF_Funct,     XM_CMP|XM_CPT|XM_FAL,  },
    {"NOT", XO_Not,     XIF_Funct,     XM_CMP|XM_CPT|XM_FAL,  },
    {"=",   XO_EQ,      XIF_Cmp,       XM_CMP,                },
    {"<>",  XO_NE,      XIF_Cmp,       XM_CMP,                },
    {"<",   XO_LT,      XIF_Cmp,       XM_CMP,                },
    {"<=",  XO_LE,      XIF_Cmp,       XM_CMP,                },
    {">",   XO_GT,      XIF_Cmp,       XM_CMP,                },
    {">=",  XO_GE,      XIF_Cmp,       XM_CMP,                },
    {"+",   XO_Plus,    XIF_Operator,  XM_CMP|XM_CPT|XM_FAL,  },
    {"-",   XO_Minus,   XIF_Operator,  XM_CMP|XM_CPT|XM_FAL,  },
    {"*",   XO_Mult,    XIF_Operator,  XM_CMP|XM_CPT|XM_FAL,  },
    {"|",   XO_Div,     XIF_Operator,  XM_CMP|XM_CPT|XM_FAL,  },
    {"**",  XO_Exp,     XIF_Operator,  XM_CMP|XM_CPT|XM_FAL,  },
    {"FRD", XO_FRD,     XIF_Funct,     XM_CMP|XM_CPT|XM_FAL,  },
    {"TOD", XO_TOD,     XIF_Funct,     XM_CMP|XM_CPT|XM_FAL,  },
    {"DEG", XO_DEG,     XIF_Funct,     XM_CMP|XM_CPT,         },
    {"RAD", XO_RAD,     XIF_Funct,     XM_CMP|XM_CPT,         },
    {"(",   XO_OBrace,  XIF_OBrace,    XM_CMP|XM_CPT|XM_FAL,  },
    {")",   XO_CBrace,  XIF_CBrace,    XM_CMP|XM_CPT|XM_FAL,  },
    {NULL}
  };

const int nXSymbols = sizeof(XSymbols)/sizeof(XSymbols[0]);

struct CXOperator
  {
  XO_OpCodes m_Op;
  byte       m_iSym;
  union {
    int      m_Int;
    float    m_Flt;
    CAddress *m_pAdd;
    };

  CXOperator()
    {
    m_Op=XO_Null;
    m_Flt=0;
    m_iSym=nXSymbols;
    }
  };

class CXValue
  {
  protected:
    bool m_IsFlt;
    union
      {
      int    m_Int;
      float  m_Flt;
      };
  public:
    void SetF(float f) { m_IsFlt=true; m_Flt=f; };
    void SetI(short s) { m_IsFlt=false; m_Int=s; };
    float F() { return m_IsFlt ? m_Flt : m_Int; };
    short I() { return m_IsFlt ? (short)m_Flt : m_Int; };
    bool  IsFlt() { return m_IsFlt; };
  };

class CExpression
  {
  public:
    CPLC5    * m_Plc;
    CString    m_sTxt;

    int        m_nOps;
    CXOperator *m_pOps;
    int        m_nAddOps;

    CExpression(CPLC5 * Plc)
      {
      m_Plc=Plc;
      m_nOps=0;
      m_pOps=NULL;
      m_nAddOps=0;
      }

    ~CExpression()
      {
      for (int i=0; i<m_nOps; i++)
        if (m_pOps[i].m_Op==XO_Address)
          delete m_pOps[i].m_pAdd;
      delete [] m_pOps;
      }

    void  Extend(const char * p) { if (m_sTxt.GetLength()) m_sTxt+=" "; m_sTxt+=p; };

    const char * Text () { return m_sTxt; };
    void  Parse(CTknFile & Tkns, byte FnMsk);
    CXValue Evaluate();

    CAddress* AddressFld(int Index);
    long      NAddressFlds() { return m_nAddOps; };
  };

//===========================================================================
//
//
//
//===========================================================================

//---------------------------------------------------------------------------

class CAddInfo
  {
  public:
    CString   m_sAddress;
    long      m_lSymFile;
    CString   m_sSymbol;
    CString   m_sDesc[5];
    CString   m_sFullDesc;
    long      m_lDev;
    CString   m_sAbv;
    CString   m_sBlw;
    int       m_nDesc;
    int       m_AddInfoInx;
    bool      m_bDumpChanges;

    CAddress *m_pAddress;

    CAddInfo()
      {
      m_lSymFile=0;
      m_lDev=0;
      m_nDesc=0;
      m_pAddress=NULL;
      m_bDumpChanges=true;
      }

    void Prepare()
      {
      if (m_sAddress[0]!='I' && m_sAddress[0]!='O')
        {
        for (int i=0; i<(m_sAddress.GetLength()-2); i++)
          {
          while (i<(m_sAddress.GetLength()-2) && !isdigit(m_sAddress[i]) && m_sAddress[i+1]=='0' && isdigit(m_sAddress[i+2]))
            m_sAddress.Delete(i+1,1);
          }
        }

      for (m_nDesc=0; m_nDesc<5; m_nDesc++)
        {
        //m_Desc[m_nDesc].TrimLeft()
        m_sFullDesc+=m_sDesc[m_nDesc];
        if (m_sDesc[m_nDesc].GetLength()==0)
          break;
        }

      };
    int     DescCount()     { return m_nDesc; };
    LPCTSTR DescText(int i) { return m_sDesc[i]; };
    LPCTSTR FullDescText()  { return m_sFullDesc; };
    LPCTSTR Address()       { return m_sAddress; };
  };

//typedef char * CParmArray[50];

class CFnBase
  {
  public:
    CPLC5 * m_Plc;
    CRung * m_pRung;
    byte m_Cd, m_Wide;

    CFnBase(CPLC5 * Plc, byte Cd, CTknFile & Tkns)
      {
      m_Plc=Plc;
      m_pRung=NULL;
      m_Cd=Cd;
      m_Wide=3;
      }
    virtual ~CFnBase()
      {
      }
    virtual byte    PictWide()                    { return m_Wide;};
    virtual byte    PictHeight()                  { return 1; };
    virtual byte    PictOffset()                  { return 0; };
    virtual byte    PictDescCount()               { return 0; };
    virtual LPCTSTR PictDescText(int i)           { return ""; };

    static  void SBuildTxtImage(CRungImage & Img, CMtxRow &R, CMtxElement & E);
    virtual void VBuildTxtImage(CRungImage & Img, CMtxRow &R, CMtxElement & E);

    virtual LPCTSTR AddressText()                 { return ""; };
    virtual int     DescCount()                   { return 0; };
    virtual LPCTSTR DescText(int i)               { return ""; };
    //virtual bool Enabled()                        { ASSERT(0); return 0; } ;
    virtual bool SetEnabled(bool On) { /*if (dbgf) fprintf(dbgf, "SetEnabled Missing %i", m_Cd);*/ return 0; } ;
    virtual CAddress* AddressFld(int Index)       { return NULL; } ;
    virtual long      NAddressFlds()              { return 0; };
    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt)    { return NULL; } ;
    virtual CLadder* TargetLadder(CPoint pt, CPoint cpt)      { return NULL; } ;

    CRung   & MyRung();//{ return *m_pRung; };
    CLadder & MyLadder();//{ return *m_pRung->m_pLadder; };
    CPLC5   & MyPLC();//{ return *m_pRung->m_pLadder->m_pPLC; };
    CFnInfoItem & MyInfo();//{ return *m_pRung->m_pLadder->m_pPLC; };

  };

class CFnBlk : public CFnBase
  {
  public:
    CFnBlk(CPLC5 * Plc, byte Cd, CTknFile & Tkns): CFnBase(Plc, Cd, Tkns)
      {
      }
    virtual ~CFnBlk()
      {
      }
    virtual byte    PictWide()          { return 22; };
    virtual byte    PictHeight()        { return 4+PictDescCount(); };
    virtual byte    PictOffset()        { return 1+PictDescCount(); };
    virtual bool    PictText(int Row, int Wide, CString &S) { return false; };
  };

//===========================================================================
//
//
//
//===========================================================================


const int MaxRowElements=20;
const int MaxImgRows=120;
const int MaxImgCols=256;

class CCoOrd
  {
  public:
    int m_Row, m_Col;

    CCoOrd() { m_Row=0; m_Col=0; };
    CCoOrd(int r, int c, int x, int y) { m_Row=r; m_Col=c; };
  };
class CRungImage
  {
  public:
    char m_C[MaxImgRows][MaxImgCols];
    char m_Colour[MaxImgRows];
    int  m_Width, m_Height;
    int  m_iBranch;
    CCoOrd m_Branches[20];

    CRungImage()
      {
      Clear();
      };
    void Clear()
      {
      m_Width=0;
      m_Height=0;
      m_iBranch=0;
      m_Branches[m_iBranch].m_Row=0;
      m_Branches[m_iBranch].m_Col=0;
      memset(m_C, ' ', sizeof(m_C));
      for (int i=0; i<MaxImgRows; i++)
        {
        m_C[i][MaxImgCols-1]=0;
        m_Colour[i]=0;
        }
      };
  };

class CMtxElement;
class CMtxRow;

//---------------------------------------------------------------------------

typedef CFnBase * LPCFnBase;
class CMtxElement
  {
  public:
    byte  m_Cd, m_Wd, m_AWd, m_Ht, m_Of;
    byte  m_CdA, m_CdB, m_CdL, m_CdR;
    byte  m_LftRail:1,
          m_RgtRail:1,
          m_LftPwr:1,
          m_RgtPwr:1;

    int   m_X, m_Y;
    CFnBase * m_pFn;

    virtual CAddress* TargetAddress(CPoint pt, CPoint cpt) { return m_pFn? m_pFn->TargetAddress(pt, cpt) : NULL; } ;
    virtual CLadder* TargetLadder(CPoint pt, CPoint cpt)   { return m_pFn? m_pFn->TargetLadder(pt, cpt) : NULL; } ;
    bool  HasFn()  { return m_pFn!=NULL; };
    CFnBase & Fn() { return *m_pFn; };
    CFnBlk  & FnBlk() { return *(CFnBlk*)m_pFn; };

  };

struct CLadderDisplayRec
  {
  CRect       m_Pos;
  CMtxElement * m_pE;
  };

class CLadderDisplayList
  {
  public:
    CArray <CLadderDisplayRec, CLadderDisplayRec&> m_Elems;
    int  m_ActLen;

    CLadderDisplayRec & operator[](int i)
      {
      if (i>=m_Elems.GetSize())
        m_Elems.SetSize(i+1, 32);
      return m_Elems[i];
      };
  };

class CTFnMatrix
  {
  public:
    CMtxElement m_Elements[MaxFnRows][MaxFnCols];
    int m_Yt[MaxFnRows];
    int m_Yr[MaxFnRows];
    int m_Ht[MaxFnRows];
    int m_iWide, m_iHigh;

    CTFnMatrix()
      {
      m_iWide=0;
      m_iHigh=0;
      memset(m_Yt, 0, sizeof(m_Yt));
      memset(m_Yr, 0, sizeof(m_Yr));
      memset(m_Ht, 0, sizeof(m_Ht));
      memset(m_Elements, 0, sizeof(m_Elements));
      }

    CMtxElement & operator()(int r, int c) { ASSERT(r<MaxFnRows && c<MaxFnCols); return m_Elements[r][c]; };
    CMtxElement & Mtx(int r, int c)    { ASSERT(r<MaxFnRows && c<MaxFnCols); return m_Elements[r][c]; };
    LPCFnBase     & Fn(int r, int c) { ASSERT(r<MaxFnRows && c<MaxFnCols); return m_Elements[r][c].m_pFn; };
    byte          & Cd(int r, int c) { ASSERT(r<MaxFnRows && c<MaxFnCols); return m_Elements[r][c].m_Cd; };

  };
class CFnArray : public CArray <CFnBase*, CFnBase*> {};
class CMtxRow
  {
  public:
    CArray <CMtxElement, CMtxElement&> m_Cols;
    int m_Yt;
    int m_Yr;
    int m_Ht;
  };

class CFnMatrix
  {
  public:
    CArray <CMtxRow, CMtxRow&> m_Rows;
    int m_iWide, m_iHigh;

    CFnMatrix & operator=(const CTFnMatrix & M);

    CMtxElement & Mtx(int r, int c)    { return m_Rows[r].m_Cols[c]; };
    CMtxElement & operator()(int r, int c) { return m_Rows[r].m_Cols[c]; };

    void CalculateSpacing(int & MaxWidth);
    void ReCalculateSpacing(int MaxWidth);

    void Evaluate();
  };

class CRung
  {
  public:
    int       m_iRung;
    CLadder  *m_pLadder;
    CFnArray  m_Fns;
    CFnMatrix m_Matrix;

    CRung()
      {
      m_iRung=-1;
      m_pLadder=NULL;
      m_Fns.SetSize(0, 4);
      }

    virtual ~CRung()
      {
      for (int i=0; i<m_Fns.GetSize(); i++)
        delete m_Fns[i];
      }
    int  BuildFnMatrix(int iFnNo, CCoOrd & RC, CCoOrd & RCEnd, CTFnMatrix & Mtx);
    int  BuildFnMatrixBr(int iFnNo, CCoOrd & RC, CCoOrd & RCEnd, CTFnMatrix & Mtx);
    void BuildFnMatrixFn(byte Cd, int iFnNo, CCoOrd & RC, CTFnMatrix & Mtx);
    int  BuildTxtImage(CRungImage & Img, CLadderDisplayList & Lst, int LnRungStart);

    void Evaluate() { m_Matrix.Evaluate(); };
  };

#if DotNetAfxTempl
typedef CSmartPtrAllocate <CRung> CSPRung;
typedef CArray <CSPRung, CSPRung&> CRungArray;
#else
typedef CArray <CRung*, CRung*> CRungArray;
inline void AFXAPI ConstructElements(CRung **pItem, int nCount)
  { for ( int i = 0; i < nCount; i++, pItem++ ) *pItem=new CRung; };
inline void AFXAPI DestructElements(CRung **pItem, int nCount)
  { for ( int i = 0; i < nCount; i++, pItem++ ) delete *pItem; };
#endif
//---------------------------------------------------------------------------

class CLadder
  {
  public:
    CPLC5  *m_pPLC;
    int     m_iIndex;
    CString m_sName;

    int     m_MaxWidth;
    int     m_Line1;
    int     m_CallLevel;

    long    m_FindRung;
    long    m_FindFnct;

    int     m_nXferParms;
    float   m_XferParms[MaxJSRParms];

    int     m_NextRung; // Used by JMP LBL
    int     m_Lbl2Rung[256];

    CRungArray m_Rungs;
    CLadder()
      {
      m_pPLC=NULL;
      m_iIndex=-1;
      m_Line1=0;
      m_Rungs.SetSize(0,16);
      m_CallLevel=0;
      m_nXferParms=0;
      m_NextRung=-1;
      memset(m_Lbl2Rung, -1, sizeof(m_Lbl2Rung));
      m_FindRung=-1;
      m_FindFnct=-1;
     }
    virtual ~CLadder()
      {
      }
    void BuildFnMatrix();
    void Evaluate();
    void SetNextRung(int No)
      {
      CheckThat(No>=0 && No<256, "Bad Rung Label");
      CheckThat(m_Lbl2Rung[No]>=0, "Missing Rung Label");
      m_NextRung=m_Lbl2Rung[No];
      };

    long FindRungWithAddress(LPCTSTR pAdd);
    long NRungs() { return m_Rungs.GetSize(); };
    long NFns(long r) { return m_Rungs[r]->m_Fns.GetSize(); };
    CRung & Rung(long r) { return *m_Rungs[r]; };
    CFnBase & Function(long r, long f) { return *m_Rungs[r]->m_Fns[f]; };

  };

/////////////////////////////////////////////////////////////////////////////


class CTable
  {
  public:
    CTableRoot * m_pRoot;
    int     m_iIndex;
    CString m_sName;
    CString m_sTxt;
    CString m_sNum;

    byte   m_HexDispReqd:1;
    long   m_nRangeChkErrors;

    int m_Line1;
    int m_nVals;
    bool m_bDumpSets;

    CArray <short, short&> m_IVals,  m_IValsPrev, m_IValsOn, m_IValsOff;
    CArray <float, float&> m_FVals, m_FValsPrev;

    CTable()
      {
      m_pRoot=NULL;
      m_iIndex=-1;
      m_Line1=0;
      m_nVals=0;
      m_HexDispReqd=false;
      m_bDumpSets=true;
      m_nRangeChkErrors=0;
      }

    void Initialise(CTableRoot * pTbRt, const int TabNum, const char* Nm, const char* NmTxt, const char* NmNo, int nVals);

    void Reset();

    bool OctalAdd()                                 { return m_pRoot->m_OctalAdd; };
    bool IsFlt()                                    { return m_pRoot->m_IsFlt; };
    bool HexDispLoad()                              { return m_pRoot->m_HexDispLoad; };

    int ValsPerColumn()                             { return m_pRoot->m_ValsPerColumn; };
    int ColumnWidth()                               { return m_pRoot->m_ColumnWidth; };
    int NStructVals()                               { return m_pRoot->m_nStructVals; };
    int NVals()                                     { return m_nVals; };

    const char &ReqdFmt(int i)                      { return m_pRoot->m_cReqdFmt[i]; };
    const char *LoadFmt()                           { return m_pRoot->m_pLoadFmt; };
    
    bool TestIndexInRange(bool IVals, long Index);

    bool  BValue(int bit);
    short IValue(int i, int Off=0);
    word  UIValue(int i, int Off=0);
    float FValue(int i, int Off=0);
    short IValuePrev(int i, int Off=0);
    float FValuePrev(int i, int Off=0);

    bool  SetBValue(int bit, bool Val, CAddress & m_Add, CRung * pRung=NULL, LPCTSTR pText="");
    bool  BValueFrc(int bit);
    short IValueFrc(int i, int Off=0);
    bool  IsBValueFrc(int bit);
    bool  IsBValueFrc(int wrd, int bit);
    bool IsIValueFrc(int i, int Off=0);
    void SetBValueFrc(int bit, bool Val);
    void ClrBValueFrc(int bit);

    short SetIValue(int i, int Off, short Val);
    void SetIValueFrc(int i, int Off, short Val);
    void ClrIValueFrc(int i, int Off);
    word  SetUIValue(int i, int Off, word  Val);
    float SetFValue(int i, int Off, float Val);
    short SetIValuePrev(int i, int Off, short Val);
    float SetFValuePrev(int i, int Off, float Val);
    void * IValuePtr(int i, int Off);
    float ValueAsFlt(int i, int Off);
    void  SetValueAsFlt(int i, int Off, float Val);

  };

//===========================================================================
//
//
//
//===========================================================================

inline void StripAddressZeros(CString & S)
  {
  int i=0;
  char * s=S.GetBuffer(0)+1;
  while (*s)
    {
    if ((*(s-1)==':' || *(s-1)=='/') && *s=='0' && isdigit(*(s+1)))
      memmove(s, s+1, strlen(s+1)+1);
    else
      s++;
    }
  S.ReleaseBuffer();
  };

//===========================================================================
//
//
//
//===========================================================================

const byte Vw_NULL   = 0;
const byte Vw_Table  = 1;
const byte Vw_Ladder = 2;
const byte Vw_Symbol = 3;
const byte Vw_Force  = 4;

const byte State_Stopped  = 0;
const byte State_Idling   = 1;
const byte State_Running  = 2;
const byte State_Stepping = 3;

static const char * States[] = {"Stopped", "Idling", "Running", "Stepping",};

//===========================================================================
//
//   A PLC
//
//===========================================================================

#pragma warning (disable: 4355)  // 'this' : used in base member initializer list

class CPLC5
  {
  public:
    CString         m_sPlcName;

    CString         m_sPrjName;

    long            m_nTables;
    CTable *        m_Table[MaxTables];

    //CTable *        m_TableL[MaxTablesList];
    //int             m_nTablesL;

    long            m_nLadders;
    CLadder         m_Ladder[MaxLadders];
    CRung         * m_pWkRung;
    CLadder       * m_pWkLadder;

	  BOOL            m_SymLoaded;
	  BOOL            m_LadLoaded;

    long            m_nUnImpOpCd;
    long            m_nInCompOpCd;

    long            m_IterNo;
    bool            m_bReset;

    LoadTypes       m_LoadWhat;

    CAddress        m_Status_ForcesPresent;
    CAddress        m_Status_Remote;
    CAddress        m_Status_ScanOne;
    CAddress        m_Status_MinorFault;
    CAddress        m_Status_MajorFault;

    short         * m_pS24; // Indirect Add Register
    int             m_StepMS; // Stepsize;
    long            m_MinTimerPre; // Shortest timer ms


    CList <CAddInfo*, CAddInfo*> m_AddInfoList;
    CArray <CAddInfo*, CAddInfo*> m_AddInfo;
    CMapStringToPtr m_SymbolMap;
    CMapStringToPtr m_AddressMap;

#if WithTestXCode
    CExpression     m_TstXs[10];
#endif

    CPLC5() :
      m_SymbolMap(64),
      m_AddressMap(64),
      m_Status_ForcesPresent(this),
      m_Status_Remote(this),
      m_Status_ScanOne(this),
      m_Status_MinorFault(this),
      m_Status_MajorFault(this)
      {
      m_pWkRung = NULL;
      m_pWkLadder = NULL;
	    m_SymLoaded=false;
	    m_LadLoaded=false;
      m_nUnImpOpCd=0;
      m_nInCompOpCd=0;
      m_IterNo=0;
      m_bReset=false;

      m_nTables=0;
      memset(m_Table, 0, sizeof(m_Table));
      m_nLadders=0;
      };

    ~CPLC5()
      {
      for (int i=0; i<m_nTables; i++)
        if (m_Table[i])
          delete m_Table[i];

      while (!m_AddInfoList.IsEmpty())
        {
        CAddInfo *p=m_AddInfoList.RemoveHead();
        delete p->m_pAddress;
        delete p;
        }
      };

    BOOL Load(LPCTSTR PlcName, LPCTSTR FnLad, LPCTSTR FnSym, LPCTSTR FnSymOut, LPCTSTR FnOptProfile);
    BOOL LoadSymbols(LPCSTR FnSym, LPCSTR FnSymOut, LPCTSTR FnOptProfile);
    void LoadATable(CTknFile & Tkns, FILE *f, LPCTSTR FnOptProfile);
    void LoadARung(CTknFile & Tkns, FILE *f);
    void LoadALadder(CTknFile & Tkns, FILE *f, LPCTSTR FnOptProfile);
    BOOL LoadLadder(LPCSTR Fn, LPCTSTR FnOptProfile);
    void GetProjectInfo(CTknFile & Tkns, FILE *f);

    int  LoadSnapshot(LPCTSTR Fn);
    int  SaveSnapshot(LPCTSTR Fn);
    int  LoadBacktrack(LPCTSTR Fn);
    int  SaveBacktrack(LPCTSTR Fn);

#if WithTestXCode
    void LoadTextXs(CTknFile & Tkns, FILE *f);
#endif

    //void InitTree(CTreeCtrl & m_Tree);

    void SaveData();
    void Reset();
    void Evaluate(int StepMS, bool DoReset, bool InRemote);

    CLadder * CurrentLadder();
    CRung   * CurrentRung();

    CTable &  Table(int i) { return *m_Table[i]; };
    short SetIndexRegister(short i);

    CAddInfo * FindSymbol(LPCTSTR pSym)
      {
      void * p;
      return pSym && m_SymbolMap.Lookup(pSym, p) ? (CAddInfo*)p : NULL;
      };
    CAddInfo * FindAddress(LPCTSTR pAdd)
      {
      if (!pAdd)
        return NULL;
      void * p;
      CString S=pAdd;
      StripAddressZeros(S);
      return m_AddressMap.Lookup(S, p) ? (CAddInfo*)p : NULL;
      };
    void DumpAdressOn(LPCTSTR Value, bool On)
      {
      CAddInfo * pInfo=FindAddress(Value);
      if (pInfo)
        {
        pInfo->m_bDumpChanges=On;
        char Buff[128];
        sprintf(Buff, "%s Changes %s", Value, On ?"ON":"Off");
        m_History.Add(Buff);
        }
      };

  };

//===========================================================================
//
//  Inlines
//
//===========================================================================

inline CRung   & CFnBase::MyRung()              { return *m_pRung; };
inline CLadder & CFnBase::MyLadder()            { return *m_pRung->m_pLadder; };
inline CPLC5   & CFnBase::MyPLC()               { return *m_pRung->m_pLadder->m_pPLC; };
//inline CFnInfoItem & CFnBase::MyInfo()          { return FnList[m_Cd]; };
inline CTable * CAddress::Table()               { return m_Plc->m_Table[FileNo()]; };
inline bool     CAddress::IsFlt()               { return Table()->IsFlt(); };
inline CAddInfo* CAddress::SymbolF(LPCTSTR pAddress)  { void * p; return m_Plc->m_AddressMap.Lookup(pAddress, p) ? (CAddInfo*)p : NULL; };
inline CAddInfo* CAddress::AddressF(LPCTSTR pSymbol)  { void * p; return m_Plc->m_SymbolMap.Lookup(pSymbol, p) ? (CAddInfo*)p : NULL; };
inline int      CAddress::DescCount()           { return m_pInfo ? m_pInfo->DescCount() : 0; };
inline LPCTSTR  CAddress::DescText(int i)       { return m_pInfo ? m_pInfo->DescText(i) : ""; };

//===========================================================================
//
//
//
//===========================================================================

class /*DllImportExport*/ CStopWatch
  {
  protected:
    LARGE_INTEGER Freq;
    bool   bRunning;
    DWORD  dwLaps;
    double dCurrent;
    double dAccum;
  public:
    double       Time()      { LARGE_INTEGER Tmp; QueryPerformanceCounter(&Tmp); return ((double)(Tmp.QuadPart))/Freq.QuadPart; }

    void         Start()     { dCurrent -= Time(); dwLaps++; bRunning=1; };
    void         Stop()      { dCurrent += Time(); bRunning=0; };
    double       Secs()      { return dCurrent + (bRunning ? Time(): 0.0); };
    void         ReStart()   { dCurrent=dAccum=0.0; Start();};
    double       Lap()       { double d1=Secs(); double d2=d1-dAccum; dAccum=d1; dwLaps++; return d2; };
    void         Clear()     { dCurrent=dAccum=0.0; dwLaps=0; bRunning=0; };
    DWORD        Laps()      { return dwLaps;};
    //char*        MinSecDesc(Strng& s);
    //char*        MinSecHunDesc(Strng& s);

    CStopWatch()    { Clear(); QueryPerformanceFrequency(&Freq); }
  };

//===========================================================================
//
//
//
//===========================================================================

inline int CAddress::FileNo()
  {
  return m_bIndFileNo ? m_uFile.m_pAdd->IValue() : m_uFile.m_iIndex;
  };
inline int CAddress::WordNo(bool AddIndex)
  {
  int w;
  //ASSERT(!m_IsBitAdd);
  /*
  if (m_IsBitAdd)
    w=(m_bIndBitNo ? m_uBit.m_pAdd->IValue() : m_uBit.m_iIndex)/16;
  else
  */
  w=m_bIndWordNo ? m_uWord.m_pAdd->IValue() : m_uWord.m_iIndex;
  if (m_bIndexed && AddIndex)
    w+=(*m_Plc->m_pS24);
  return w;
  };
inline int CAddress::BitNo(bool AddIndex)
  {
  int b=0;

  if (m_IsBitAddRelToWrd)
    b=(m_bIndWordNo ? m_uWord.m_pAdd->IValue() : m_uWord.m_iIndex)*16;
  b+=m_bIndBitNo  ? m_uBit.m_pAdd->IValue()  : m_uBit.m_iIndex ;
  if (m_bIndexed && AddIndex)
    b+=*(m_Plc->m_pS24);
  return b;
  };

inline short CPLC5::SetIndexRegister(short i)
  {
  short old=*(m_pS24); *(m_pS24)=i;
  return old;
  };

//===========================================================================
//
//
//
//===========================================================================

class CPLC5Array : public CArray <CPLC5, CPLC5&>
  {
  public:
    void InitTree(CTreeCtrl & m_Tree);
    CPLC5 & PLC(int i) { return ElementAt(i); }
    long   NPLCs() { return GetSize(); }
    long   Find(LPCTSTR Name)
      {
      for (int i=GetSize()-1; i>=0; i--)
        if (stricmp(ElementAt(i).m_sPlcName, Name)==0)
          break;
      return i;
      }
  };

//===========================================================================
//
//
//
//===========================================================================

inline DWORD MakeItemData(DWORD Vw, DWORD Pc, DWORD I)  { return (Vw&0xff)<<24 | (Pc&0xff) << 16 | (I&0xffff); };
inline DWORD ExtractItemVw(DWORD D)                     { return (D>>24) & 0xff; };
inline DWORD ExtractItemPc(DWORD D)                     { return (D>>16) & 0xff; };
inline DWORD ExtractItemData(DWORD D)                   { return D & 0xffff; };

inline bool CTable::TestIndexInRange(bool IVals, long Index)
  {
  long Size = IVals ? m_IVals.GetSize() : m_FVals.GetSize();
  if (Index>=0 && Index < Size)
    return true;

  if (m_nRangeChkErrors++>50)
    return false;

  char buff[1000];
  sprintf(buff,"======= Range Error %s %i <0 or >= %i", (LPCTSTR)m_sName, Index, Size);

  m_History.Add(buff);

  //fprintf(dbgf, "%s\n", buff);

  return false;
  }

inline bool CTable::BValue(int bit)
  {
  int n=NStructVals();
  int b=(1<<bit%16);
  int x=(bit/16)*NStructVals();
  int y=m_IVals[x];
  int z=y & b;
  int i=(bit/16)*NStructVals();
  return !TestIndexInRange(true, i) ? 0 : (m_IVals[i] & (1<<bit%16))!=0;
  };
inline short CTable::IValue(int i, int Off)      
  { 
  i=i*NStructVals()+Off; 
  return !TestIndexInRange(true, i) ? 0 : m_IVals[i];
  };
inline word  CTable::UIValue(int i, int Off) 
  {
  i=i*NStructVals()+Off;
  return !TestIndexInRange(true, i) ? 0 : *((word*)(&m_IVals[i]));
  };
inline float CTable::FValue(int i, int Off)
  { 
  i=i*NStructVals()+Off; 
  return !TestIndexInRange(false, i) ? 0 : m_FVals[i];
  };
inline short CTable::IValuePrev(int i, int Off)
  { 
  i=i*NStructVals()+Off;
  return !TestIndexInRange(true, i) ? 0 : m_IValsPrev[i]; 
  };
inline float CTable::FValuePrev(int i, int Off)  
  { 
  i=i*NStructVals()+Off;
  return !TestIndexInRange(false, i) ? 0 : m_FValsPrev[i];
  };

inline bool  CTable::SetBValue(int bit, bool Val, CAddress & m_Add, CRung * pRung, LPCTSTR pText)
  {
  long inx=bit/16*NStructVals();
  short & w=m_IVals[inx];
  short wmem=w;
  short m=(1<<(bit%16));
  if (Val) w|=m; else w&=~m;
#if WithFRC
  w=(w &~m_IValsOff[inx])|m_IValsOn[inx];
#endif

  if (dbgf &&m_bDumpSets)
    {
    if (((w ^ wmem) & m))
      {
      m_History.DumpChange((w&m)!=0, m_Add, pRung, pText);
      //
      //CAddInfo * p=m_Add.m_Plc->FindAddress(m_Add.m_Txt);
      //if (p->m_bDumpChanges)
      //  {
      //  char buff1[1000];
      //  char buff2[1000];
      //  if (pRung)
      //    sprintf(buff1,"%-6s:%2i:%4i",
      //      (LPCTSTR)pRung->m_pLadder->m_pPLC->m_sPlcName,
      //        pRung->m_pLadder->m_iIndex, 
      //        pRung->m_iRung);
      //
      //  sprintf(buff2,"[%-14.14s %-10.10s] %-12s = %s : %-15s %s",
      //    pRung ? buff1 : pText, pRung ? pText : "",
      //    (LPCTSTR)m_Add.m_Txt, (w&m)?"1":".",
      //    p?(LPCTSTR)p->m_sSymbol:"",
      //    p?(LPCTSTR)p->FullDescText():"");
      //
      //  m_History.Add(buff2);
      //  }
      }
    }


  return (w&m)!=0;
  };
inline bool  CTable::BValueFrc(int bit)
  {
  int inx =(bit/16)*NStructVals();
  short m=(1<<(bit%16));
  //int inx=wrd*NStructVals();
  return ((m_IValsOn[inx] & ~m_IValsOff[inx])&m)!=0;
  };
inline short CTable::IValueFrc(int i, int Off)
  {
  int inx=i*NStructVals()+Off;
  return m_IValsOn[inx] & ~m_IValsOff[inx];
  };

inline bool  CTable::IsBValueFrc(int bit)
  {
  int inx =(bit/16)*NStructVals();
  short m=(1<<(bit%16));
  //int inx=wrd*NStructVals();
  return ((m_IValsOn[inx] | m_IValsOff[inx])&m)!=0;
  //return (m_IVals[(bit/16)*NStructVals()] & (1<<bit%16))!=0;
  };
inline bool  CTable::IsBValueFrc(int wrd, int bit)
  {
  short m=(1<<bit);
  int inx=wrd*NStructVals();
  return ((m_IValsOn[inx] | m_IValsOff[inx])&m)!=0;
  };
inline bool CTable::IsIValueFrc(int i, int Off)
  {
  int inx=i*NStructVals()+Off;
  return (m_IValsOn[inx] | m_IValsOff[inx])!=0;
  };
inline void CTable::SetBValueFrc(int bit, bool Val)
  {
  long inx=bit/16*NStructVals();
  short & w=m_IVals[inx];
  short m=(1<<(bit%16));
  if (Val)
    {
    m_IValsOn[inx]|=m;
    m_IValsOff[inx]&=~m;
    }
  else
    {
    m_IValsOff[inx]|=m;
    m_IValsOn[inx]&=~m;
    }
  w=(w &~m_IValsOff[inx])|m_IValsOn[inx];
  };
inline void CTable::ClrBValueFrc(int bit)
  {
  long inx=bit/16*NStructVals();
  short & w=m_IVals[inx];
  short m=(1<<(bit%16));
  m_IValsOn[inx]&=~m;
  m_IValsOff[inx]&=~m;
  };

inline short CTable::SetIValue(int i, int Off, short Val)
  {
  long inx=i*NStructVals()+Off;
  if (TestIndexInRange(true, inx)) 
    {
    #if WithFRC
    Val=(Val &~m_IValsOff[inx])|m_IValsOn[inx];
    #endif
    return m_IVals[inx]=Val;
    }
  return 0;
  };
inline void CTable::SetIValueFrc(int i, int Off, short Val)
  {
  long inx=i*NStructVals()+Off;
  m_IValsOn[inx]=Val;
  m_IValsOff[inx]=~Val;
#if WithFRC
  m_IValsOn[inx]=(m_IValsOn[inx]&~m_IValsOff[inx])|m_IValsOn[inx];
#endif
  //return m_IVals[inx]=Val;
  };
inline void CTable::ClrIValueFrc(int i, int Off)
  {
  long inx=i*NStructVals()+Off;
  m_IValsOn[inx]=0;
  m_IValsOff[inx]=0;
  //return m_IVals[inx]=Val;
  };
inline word  CTable::SetUIValue(int i, int Off, word  Val)
  {
  long inx=i*NStructVals()+Off;
  if (TestIndexInRange(true, inx)) 
    {
    #if WithFRC
    Val=(Val &~m_IValsOff[inx])|m_IValsOn[inx];
    #endif
    return m_IVals[inx]=(short)Val;
    }
  return 0;
  };
inline float CTable::SetFValue(int i, int Off, float Val)     
  {
  i=i*NStructVals()+Off;
  if (TestIndexInRange(false, i)) 
    return m_FVals[i]=Val;
  return 0;
  };
inline short CTable::SetIValuePrev(int i, int Off, short Val) 
  {
  i=i*NStructVals()+Off;
  if (TestIndexInRange(true, i)) 
    {
    return m_IValsPrev[i]=Val;
    }
  return 0;
  };
inline float CTable::SetFValuePrev(int i, int Off, float Val) 
  {
  i=i*NStructVals()+Off;
  if (TestIndexInRange(false, i)) 
    return m_FValsPrev[i]=Val;
  return 0;
  };

inline void* CTable::IValuePtr(int i, int Off)              
  {
  i=i*NStructVals()+Off;
  if (TestIndexInRange(true, i)) 
    return &m_IVals[i]; 
  return 0;
  };

inline float CTable::ValueAsFlt(int i, int Off) 
  {
  i=i*NStructVals()+Off;
  if (!TestIndexInRange(!IsFlt(), i))
    return 0;
  return IsFlt() ? m_FVals[i] : m_IVals[i];
  };

inline void  CTable::SetValueAsFlt(int i, int Off, float Val)
  {
  i=i*NStructVals()+Off;
  if (!TestIndexInRange(!IsFlt(), i))
    return;
  
  if (IsFlt()) m_FVals[i]=Val; else m_IVals[i]=short(Val); 
  };

