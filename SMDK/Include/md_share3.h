//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#if !defined(MD_SHARE3_H)
#define MD_SHARE3_H

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#include "md_share1.h"

#undef DllImportExport
#if defined(__MD_SHARE3_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

// ======================================================================
//
//
//
// ======================================================================

#undef DllImportExport
#if defined(__GPFUNCS_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

DllImportExport LPTSTR   TemporaryFiles();
DllImportExport LPTSTR   CfgFile();
DllImportExport LPTSTR   CfgFiles();
DllImportExport LPTSTR   CfgName();
DllImportExport LPTSTR   PrjFile();
DllImportExport LPTSTR   PrjFiles();
DllImportExport LPTSTR   PrjName();
DllImportExport LPTSTR   PrjNameAlias();

DllImportExport BOOL     FileExists(LPCTSTR FileName, WIN32_FIND_DATA & fd);
DllImportExport BOOL     FileExists(LPCTSTR FileName);
DllImportExport BOOL     FileAttributes(LPCTSTR FileName, DWORD & Attributes);
DllImportExport BOOL     FileWriteTime(LPCTSTR FileName, FILETIME & FileTime);

// ======================================================================

const short MaxCSVCols = 256;
typedef char * CSVColArray[MaxCSVCols];
DllImportExport int      ParseCSVTokens(LPTSTR Buff, CSVColArray &C, int& Quote, LPCTSTR MoreSepars=NULL, bool TabsAsWhitespace=false);
DllImportExport int      ParseCSVFunction(LPTSTR Buff, CSVColArray &C, int& Quote);
DllImportExport int      ParseCSVTokens(LPTSTR Buff, CSVColArray &C, LPCTSTR MoreSepars=NULL, bool TabsAsWhitespace=false);
DllImportExport int      ParseCSVFunction(LPTSTR Buff, CSVColArray &C);
DllImportExport int      ParseTokens(LPTSTR Buff, CSVColArray &C, char Token);
DllImportExport int      ParseTabTokens(LPTSTR Buff, CSVColArray &C);

// ======================================================================

DllImportExport BOOL     CopyTextToClipboard(LPCTSTR Text);
DllImportExport BOOL     AddTextToClipboard(LPCTSTR Text, LPCTSTR Separator=NULL);

// ======================================================================
//
//
//
// ======================================================================

#undef DllImportExport
#if defined(__MATHLIB_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

DllImportExport int SafeAOtoI(LPCTSTR Str, int Default=0);
DllImportExport double ParseNANFlag(LPCTSTR Str, double Val=dNAN);
DllImportExport double SafeAtoF(LPCTSTR Str, double Default=0.0);
DllImportExport long SafeAtoL(LPCTSTR Str, long Default=0);
DllImportExport int SafeAtoI(LPCTSTR Str, int Default=0);
DllImportExport int SafeAlphatoI(LPCTSTR Str, int Default=0, int Base=26);

// ======================================================================
//
//
//
// ======================================================================

#undef DllImportExport
#if defined(__REGISTRY_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

class DllImportExport CRegistry
  {
  public:
    CRegistry(void);
    ~CRegistry(void);

    void     SetRegistryKey(HKEY hKey, LPCTSTR lpszRegistryKey, LPCTSTR lpszProfile);
    HKEY     GetAppRegistryKey();
    HKEY     GetSectionKey(LPCTSTR lpszSection);
    UINT     GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,	int nDefault);
    CString  GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,	LPCTSTR lpszDefault);
    BOOL     GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, BYTE** ppData, UINT* pBytes);
    BOOL     WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
    BOOL     WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
    BOOL     WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes);


  protected:
    HKEY    m_hKey;
    LPCTSTR m_pszProfileName;
	  LPCTSTR m_pszRegistryKey;
	  LPCTSTR m_pszAppName;

  };

// -------------------------------------------------------------------------
/*#C:This class is used to access INI type files. It provides more than the
standard "Set/Get Profile" options.*/

//SetDllImportExport(__GPFUNCS_CPP, SCDLIB)               
#undef DllImportExport
#if defined(__GPFUNCS_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

class DllImportExport CProfINIFile : protected CRegistry
  {
  public:
    CProfINIFile(LPCSTR pFilename = "");
    CProfINIFile(LPCSTR pDir, LPCSTR pFilename);
    ~CProfINIFile();

    __int64       RdInt64(LPCTSTR lpszSection, LPCTSTR lpszEntry, __int64 iDefault);
    void          WrInt64(LPCTSTR lpszSection, LPCTSTR lpszEntry, __int64 iValue);
    double        RdDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dDefault);
    void          WrDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dValue);
    long          RdLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nDefault);
    void          WrLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nValue);
    int           RdInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
    void          WrInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
    LPCSTR        RdStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCSTR pDefault="");
    void          WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCSTR pValue);
    DWORD         RdSection(LPCTSTR lpszSection, LPSTR pBuff, int BuffLen);
    void          WrSection(LPCTSTR lpszSection, LPSTR pBuff);
    DWORD         RdSectionNames(LPSTR pBuff, int BuffLen);
    void          Flush();
    static void   GetProfItem(LPSTR &pBuff, LPSTR &pName, LPSTR &pExp);
    void          SetUseRegistry(bool On, HKEY hKey, LPCTSTR lpszRegistryKey, LPCTSTR lpszProfile);
    void          SetUseRegistry(const CProfINIFile & Other);
    void          SetProfFilename(LPCSTR pFilename)   { m_sFilename = pFilename; };
    LPCTSTR       Filename()                          { return m_sFilename.GetLength()>0 ? (LPCTSTR)m_sFilename : NULL; }

  protected:
    bool          m_bUseRegistry;
    CString       m_sFilename;
    CString       m_sTmpRdStr;
  };


inline void CProfINIFile::SetUseRegistry(bool On, HKEY hKey, LPCTSTR lpszRegistryKey, LPCTSTR lpszProfile)
  {
  m_bUseRegistry=On;
  if (m_bUseRegistry)
    SetRegistryKey(hKey, lpszRegistryKey, lpszProfile);
  };
inline void CProfINIFile::SetUseRegistry(const CProfINIFile & Other)
  {
  m_bUseRegistry=Other.m_bUseRegistry;
  if (m_bUseRegistry)
    SetRegistryKey(Other.m_hKey, Other.m_pszRegistryKey, Other.m_pszProfileName);
  };

extern DllImportExport CProfINIFile ScdPFUser;
extern DllImportExport CProfINIFile ScdPFMachine;

// ========================================================================
//
//
//
// ========================================================================

#undef DllImportExport
#if defined(__MD_SHARE3_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

// ========================================================================

// remote forward's
class TaggedObject;
class TagObjClass;
class DataDefnBlk;
class DataChangeBlk;
class ValidateDataBlk;

// ========================================================================

// Log Interface
const long MaxUserCI   = 16;
const long FirstUserCI = 8;

enum MMessageType { MMsg_None, MMsg_Error, MMsg_Warning, MMsg_Note };

class DllImportExport MLog
  {
  public:
    MLog()                           { m_pObj = NULL; };
    void         Init(TaggedObject * pObj)     { m_pObj = pObj; };

    void         Message(MMessageType Type, LPCSTR pFmt, ...); //send mesage note/warning/error
    void         SetCondition(bool On, long index, MMessageType Type, LPCSTR pFmt, ...); //set condition note/warning/error
    void         SetCondition(long index, MMessageType Type, LPCSTR pFmt, ...); //set condition note/warning/error
    void         ClearCondition(long index);   //clear condition message
  
  public:
    TaggedObject * m_pObj;
  };

extern DllImportExport MLog gs_Log;

//===========================================================================
//
//
//
//===========================================================================
// Debug Interface
class DllImportExport MDebug
  {
  public:
    MDebug();
    
    void         Init(TaggedObject * pObj=NULL);

    bool         Marked();                    // true if DoDbgBrk is Set
    void         Print(LPCSTR pFmt, ...);     //print to debug file
    void         PrintLn(LPCSTR pFmt, ...);   //print line to debug file
    void         Indent(int iIndent);         //Indent/Undent by n Chars
    void         Lock();
    void         UnLock();

  public:
    TaggedObject * m_pObj;
  };

extern DllImportExport MDebug gs_Dbg;

// ======================================================================
//
//
//
// ======================================================================

class MSpModelBase;  // Forward define for SMDK - used in SpModel

// These two shared classes allow dynamic_cast<> to work to find a SpModel or SpQuality
class MXSpModel
  {
  public:
    MXSpModel() {};
    virtual ~MXSpModel() {};
  };

class MXSpQuality
  {
  public:
    MXSpQuality() {};
    virtual ~MXSpQuality() {};
  };

// ======================================================================
//
//
//
// ======================================================================

#define WITHMDLRANGECHECKS  1

// Always define these two
const int MaxRngChksPerNode = 16;
const int FirstRngChkCI     = 4;

#if WITHMDLRANGECHECKS

const long RngChk_T         = 0x00000001;
const long RngChk_TExtn     = 0x00000002;
const long RngChk_P         = 0x00000004;
const long RngChk_Rho       = 0x00000008;
const long RngChk_Comp      = 0x00000010;
const long RngChk_Other     = 0x00000020;

class MRngChkKey
  {
  public:
    MRngChkKey(LPCSTR Cntxt, LPCSTR Name) 
      {
      m_sCntxt  = Cntxt;
      m_sName   = Name;
      }

    MRngChkKey(const MRngChkKey & K) 
      {
      *this = K;
      }
    MRngChkKey & operator=(const MRngChkKey & K) 
      {
      m_sCntxt  = K.m_sCntxt;
      m_sName   = K.m_sName;
      return *this;
      }
    bool operator==(const MRngChkKey & K) 
      {
      return (_stricmp(m_sCntxt, K.m_sCntxt)==0 && _stricmp(m_sName, K.m_sName)==0);
      }
    CString     m_sCntxt;
    CString     m_sName;
  };

//-----------------------------------------------------------------------

class DllImportExport MRngChkItem
  {
  friend class MRngChkCntxt;
  friend class MRngChkMngr;
  public:
    MRngChkItem(const MRngChkKey & Key);
    ~MRngChkItem();

    LPCSTR        FormatDisplay(int FormatNo);

  protected:

    MRngChkKey    m_Key;

    long          m_Iter;  // Last Iter where a range check occurred

    bool          m_IsMsg;
    bool          m_LTMin;
    bool          m_GTMax;
    double        m_MinValue;
    double        m_MaxValue;
    double        m_Value;

    POSITION      m_Position; // In m_List
    CString       m_sNameTrim;
    CString       m_sCnvTxt;
    int           m_iCnv;
    CString       m_sMsg;
    CString       m_sDisplay;

  };

//-----------------------------------------------------------------------

struct CRangeListItem { long m_IdMask; LPCSTR m_Name; double m_MinValue; double m_MaxValue; double m_Value; };

class TaggedObject; // forward

class DllImportExport MRngChkCntxt
  {
  friend class MRngChkItem;
  friend class MRngChkMngr;

  public:
    static const byte CRC_None  = 0;
    static const byte CRC_Basic = 1;
    static const byte CRC_All   = 2;

    MRngChkCntxt(TaggedObject * pObject, LPCSTR Context);
    virtual ~MRngChkCntxt();
    
    void   AddRngChk(LPCSTR Name, TaggedObject * pOwner, bool LTMin, bool GTMax, double MinValue, double MaxValue, double Value);
    void   AddRngChkMsg(LPCSTR Name, TaggedObject * pOwner, LPCSTR Msg);
    void   CheckRangeList(CRangeListItem Checks[], TaggedObject * pOwner);
 
    long   RngChkCount();
    LPCSTR RngChkDisplay(long Index);
 
    static byte     sm_iCndRngChks;
 
  protected:
    MRngChkItem *  FindItem(LPCSTR Name, LPCSTR Tag);
  
  protected:
    TaggedObject  * m_pObject;
    CString         m_sContext;
    POSITION        m_Position;
    long            m_Iter;  // Last Iter where a range check occurred
    long            m_ValidCount;
    
    CList<MRngChkItem*, MRngChkItem*> m_List;
    CMap<MRngChkKey*, MRngChkKey*, MRngChkItem*, MRngChkItem*> m_Map;
  };

//-----------------------------------------------------------------------

class DllImportExport MRngChks
  {
  friend class MRngChkItem;
  friend class MRngChkMngr;

  public:
    MRngChks(TaggedObject * pSrc, LPCSTR Context);
    virtual ~MRngChks();

    bool        Selected(long IdMask);
    void        CheckMin(long IdMask, LPCSTR Name, double MinValue, double Value);  
    void        CheckMax(long IdMask, LPCSTR Name, double MaxValue, double Value); 
    void        CheckRange(long IdMask, LPCSTR Name, double MinValue, double MaxValue, double Value);
    void        AddCheck(long IdMask, LPCSTR Name, bool LTMin, bool GTMax, double MinValue, double MaxValue, double Value);
    void        AddMessage(long IdMask, LPCSTR Name, LPCSTR Fmt, ...);          
    void        CheckRangeList(CRangeListItem Checks[]);


    MRngChkCntxt  * Context();

  public:
    TaggedObject  * m_pSrc;
    //MRngChkCntxt  * m_pContext;
  };

//-----------------------------------------------------------------------

class DllImportExport MRngChkMngr
  {
  friend class MRngChks;
  friend class MRngChkItem;
  friend class MRngChkCntxt;

  public:
    MRngChkMngr();

    void BumpGlblIter();
    void Dump();

    bool Selected(long Mask) const { return (m_SelectMask&Mask)!=0; };
    void SetSelect(long Which, long Reqd)      { m_SelectMask = (m_SelectMask & ~Which) | Reqd; };

  protected:
    CList<MRngChkCntxt*, MRngChkCntxt*> m_List;
    long            m_Iter;              
    long            m_SelectMask;
  };

//-----------------------------------------------------------------------

extern DllImportExport MRngChkMngr gs_RngChecks;

//-----------------------------------------------------------------------

inline bool MRngChks::Selected(long IdMask) 
  { 
  return (gs_RngChecks.m_SelectMask & IdMask) !=0;
  }

inline void MRngChks::CheckMin(long IdMask, LPCSTR Name, double MinValue, double Value)  
  {
  if ((gs_RngChecks.m_SelectMask & IdMask) && Value<MinValue) 
    {   
    MRngChkCntxt  *pCntxt=Context();
    if (pCntxt)
      pCntxt->AddRngChk(Name, m_pSrc, true, false, MinValue, dNAN, Value); 
    else
      {
      //dbgpln("MRngChks::CheckMin - No Context");
      }
    }
  };
inline void MRngChks::CheckMax(long IdMask, LPCSTR Name, double MaxValue, double Value)  
  { 
  if ((gs_RngChecks.m_SelectMask & IdMask) && Value>MaxValue) 
    {   
    MRngChkCntxt  *pCntxt=Context();
    if (pCntxt)
      pCntxt->AddRngChk(Name, m_pSrc, false, true, dNAN, MaxValue, Value);
    else
      {
      //dbgpln("MRngChks::CheckMax - No Context");
      }
    }
  };
inline void MRngChks::CheckRange(long IdMask, LPCSTR Name, double MinValue, double MaxValue, double Value)
  {
  if (gs_RngChecks.m_SelectMask & IdMask)
    {   
    MRngChkCntxt  *pCntxt=Context();
    if (pCntxt)
      {
      if (Value<MinValue) 
        pCntxt->AddRngChk(Name, m_pSrc, true, false, MinValue, MaxValue, Value); 
      else if (Value>MaxValue) 
        pCntxt->AddRngChk(Name, m_pSrc, false, true, MinValue, MaxValue, Value); 
      }
    else
      {
      //dbgpln("MRngChks::CheckRange - No Context");
      }
    }
  };
inline void MRngChks::AddCheck(long IdMask, LPCSTR Name, bool LTMin, bool GTMax, double MinValue, double MaxValue, double Value) 
  { 
  if (gs_RngChecks.m_SelectMask & IdMask)
    {   
    MRngChkCntxt  *pCntxt=Context();
    if (pCntxt)
      pCntxt->AddRngChk(Name, m_pSrc, LTMin, GTMax, MinValue, MaxValue, Value);
    else
      {
      //dbgpln("MRngChks::AddCheck - No Context");
      }
    }
  }
inline void MRngChks::AddMessage(long IdMask, LPCSTR Name, LPCSTR Fmt, ...)          
  { 
  if (gs_RngChecks.m_SelectMask & IdMask)
    {   
    MRngChkCntxt  *pCntxt=Context();
    if (pCntxt)
      {
      char Buff[4096];
      va_list argptr;
      va_start(argptr,Fmt);
#if (_MSC_VER >= 1400)
    vsprintf_s(Buff, sizeof(Buff), Fmt, argptr);
#else
    vsprintf(Buff, Fmt, argptr);
#endif
      va_end(argptr);

      pCntxt->AddRngChkMsg(Name, m_pSrc, Buff); 
      }
    else
      {
      //dbgpln("MRngChks::AddMessage - No Context");
      }
    }
  };
inline void MRngChks::CheckRangeList(CRangeListItem Checks[])
  {
  MRngChkCntxt  *pCntxt=Context();
  if (pCntxt)
    pCntxt->CheckRangeList(Checks, m_pSrc);
  else
    {
    //dbgpln("MRngChks::CheckRangeList - No Context");
    }
  };

#endif

// ======================================================================
//
//
//
// ======================================================================

class TaggedObject; // fwd
class MSubConstructBase
  {
  public:
    MSubConstructBase() {};
    virtual ~MSubConstructBase() {};
  };
class MSubConstructBaseDef
  {
  public:
    MSubConstructBaseDef() {};
    virtual ~MSubConstructBaseDef() {};
    virtual MSubConstructBase * Construct(TaggedObject * pNd) = 0;
  protected:
  };

// ======================================================================
//
//
//
// ======================================================================

//Constants for basic drawing primatives for defining a unit graphics symbol...
const long MDrw_Poly      = 0x80000001; //poly-line: list of x,y co-ordinates
const long MDrw_Arc       = 0x80000002; //circle: x,y centre co-ordinates and radius
const long MDrw_Arc3      = 0x80000003; //curve: set of 3 x,y co-ordinates
const long MDrw_TagPos    = 0x80000004; //tag position: a set of x,y co-ordinates (bottom centre)
const long MDrw_ValuePos  = 0x80000005; //value position: a set of x,y co-ordinates (not used)
const long MDrw_End       = 0x80000006; //end: end of graphics symbol definition

/*A drawing symbol is defined arround the origin 0,0.
Examples:
long Drw_SimpleHeater[] = { MDrw_Arc, 0,0, 5,
MDrw_Poly, -5,0, -3,0, -3,-2, 3,2, 3,0, 5,0,
MDrw_Poly, -5,1, -6,1, -6,0, -5,0, -5,1,
MDrw_Poly, 5,-1, 6,-1, 6,0, 5,0, 5,-1,
MDrw_TagPos, 0,-3,
MDrw_End };
long Drw_FlashTank[] = { MDrw_Poly, -5,10, -5,-5, 0,-10, 5,-5, 5,10,
MDrw_Poly, -5,-2, 5,-2,
MDrw_Arc3, -5,10, 0,13, 5,10,
MDrw_End }; 
*/
// ======================================================================
//
//
//
// ======================================================================

class TagObjClass;   // Forward define for SMDK - used in SpModel

class MInitialiseTest
  {
  public:
    MInitialiseTest(TagObjClass * C)  { m_pObjClass=C; };
    MInitialiseTest(LPCTSTR Name)     { m_pObjClass=NULL; m_sName=Name; };
    ~MInitialiseTest()                { };

    TagObjClass * ObjClass()          { return m_pObjClass; };
    LPCTSTR       Name()              { return m_sName; };

  protected :
    TagObjClass * m_pObjClass;
    CString       m_sName;
  };

// ======================================================================
//
//
//
// ======================================================================

#undef DllImportExport

#endif // !defined(MD_SHARE3_H)
                                                                                            