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

// SpModel SMFnRanges Checks
enum eSpFns { eSpFn_Cp, eSpFn_H, eSpFn_S, eSpFn_Rho, eSpFn_SatP, eSpFn_SatT, eSpFn_BPE, eSpFn_DynVisc, eSpFn_ThermCond, eFn_Last };
const LPCTSTR eSpFnsNames[] = {"Cp", "H", "S", "Rho", "SatP", "SatT", "BPE", "DynVisc", "ThermCond" };

class DllImportExport MSMFnRanges
  {
  friend class SpModel;
  friend class SpModelOwner;

  public:
    MSMFnRanges();
    void Clear();
    void SetInRange(eSpFns FnInx, bool OK);
    bool InRange(eSpFns FnInx);
    bool Valid(eSpFns FnInx);
    bool AllInRange();
    static void BumpGlblIter();

  protected:
    long          m_Flags;
    long          m_Masks;
    long          m_Iter;
    static long   sm_Iter;
  };

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
const long MDrw_Poly   = 0x80000001; //poly-line: list of x,y co-ordinates
const long MDrw_Arc    = 0x80000002; //circle: x,y centre co-ordinates and radius
const long MDrw_Arc3   = 0x80000003; //curve: set of 3 x,y co-ordinates
const long MDrw_End    = 0x80000004; //end: end of graphics symbol definition

/*A drawing symbol is defined arround the origin 0,0.
Examples:
long Drw_SimpleHeater[] = { MDrw_Arc, 0,0, 5,
MDrw_Poly, -5,0, -3,0, -3,-2, 3,2, 3,0, 5,0,
MDrw_Poly, -5,1, -6,1, -6,0, -5,0, -5,1,
MDrw_Poly, 5,-1, 6,-1, 6,0, 5,0, 5,-1,
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
                                                                                            