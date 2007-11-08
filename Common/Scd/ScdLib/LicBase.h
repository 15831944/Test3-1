//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef __LICBASE_H
#define __LICBASE_H

#include "scdver.h"

//----------------------------------
#define CURTIN_ACADEMIC_LICENSE 0
#define FORCEMINESERVE          0 

//----------------------------------
#if (FORCEMINESERVE)
// (1) for mineserv debug only
#define ALSOALLOWMINESERVEMDLS  0
#define ALLOWCNMVERSION         01
//----------------------------------

#elif (_MSC_VER>=1400)
// (2) for VS2005
#define ALSOALLOWMINESERVEMDLS  0
#ifdef _DEBUG
#define BYPASSLICENSING         01
#elif  _RELEASE
#define BYPASSLICENSING         10 //MUST ALWAYS BE OFF
#else
#define BYPASSLICENSING         1
#endif
#define ALLOWCNMVERSION         0
//----------------------------------

#else
// (3) for VS2003
#define ALSOALLOWMINESERVEMDLS  0
#ifdef _DEBUG
#define BYPASSLICENSING         01
#elif  _RELEASE
#define BYPASSLICENSING         0 //MUST ALWAYS BE OFF
#else
#define BYPASSLICENSING         0
#endif
#define ALLOWCNMVERSION         0

#endif

//===========================================================================
//=== Override for Distribution version ===
#ifdef DISTRIBUTION
#undef  BYPASSLICENSING  
#define BYPASSLICENSING         0 //MUST ALWAYS BE OFF
#undef  ALLOWCNMVERSION 
#define ALLOWCNMVERSION         0
#undef  ALSOALLOWMINESERVEMDLS
#define ALSOALLOWMINESERVEMDLS  0
#endif
//===========================================================================
//===========================================================================
//===========================================================================

//Leave CK_LICENSINGON as 1, rather use BYPASSLICENSING to bypass crypkey licensing 
#define CK_LICENSINGON    1

#if CK_LICENSINGON
#define CK_ALLOWDEMOMODE  1   //set to 1 if SysCAD demo mode is allowed (no license)
#endif      
                 
#include "sc_defs.h"

#ifdef __LICBASE_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

//days...
const int CK_WarnDays             = 21;    //no of days to give clear warning before license expires
const int CK_UrgentWarnDays       = 7;     //no of days to give urgent warning (dialog) before license expires
//no of units...
const int CK_Demo_ProbalUnits     = 25;    //no of units (nodes, not links) allowed in demo mode
const int CK_Demo_DynUnits        = 10;    //no of units (nodes, not links) allowed in demo mode
const int CK_Lite_ProbalUnits     = 50;    //no of units allowed for "probal testing" with Full Dynamic license
const int CK_Lite_DynUnits        = 20;    //no of units allowed for "dynamic testing" with Full ProBal license

const int CK_Trial_ProbalUnits    = 200;   //no of units allowed while trial license has been issued
const int CK_Trial_DynUnits       = 50;    //no of units allowed while trial license has been issued
#if CURTIN_ACADEMIC_LICENSE
const int CK_Academic_ProbalUnits = 800;   //no of units allowed while Academic license has been issued
const int CK_Academic_DynUnits    = 20;    //no of units allowed while Academic license has been issued
#else
const int CK_Academic_ProbalUnits = 600;   //no of units allowed while Academic license has been issued
const int CK_Academic_DynUnits    = 200;   //no of units allowed while Academic license has been issued
#endif

//historian...
const long CK_DemoHistSize        = 640;   //max historian data file size (multiple of 64kb) in demo mode
const long CK_TrialHistSize       = 2048;  //max historian data file size (multiple of 64kb) for trial license
const long CK_NormalHistSize      = 2048;  //max historian data file size (multiple of 64kb) for normal license
const UINT CK_DemoHistFiles       = 3;     //no of historian data files allowed in demo mode (min            = 2)
const UINT CK_TrialHistFiles      = 3;     //no of historian data files allowed for trial license (min            = 2)
const UINT CK_NormalHistFiles     = 6;     //no of historian data files allowed for normal license (min            = 2)
//trend windows...
const int CK_DemoTrends           = 6;     //max no of trend windows allowed in demo mode
const int CK_TrialTrends          = 10;    //max no of trend windows allowed for trial license
//graphics windows...
const int CK_DemoGrfs             = 3;     //max no of graphics windows allowed in demo mode
const int CK_TrialGrfs            = 8;     //max no of graphics windows allowed for trial license
//levels...
const byte CK_TrialLevel          = 0;     //level
const byte CK_NormalLevel         = 1;     //level
const byte CK_Unused2Level        = 2;     //level
const byte CK_Unused3Level        = 3;     //level

extern DllImportExport const char* LicINISection;

#if CK_ALLOWDEMOMODE
const int CK_NoOfTrialDays        = 12;       //Number of days trial license lasts for
#else
const int CK_NoOfTrialDays        = 45;       //Number of days trial license lasts for
#endif

const int CK_InfiniteDays       = 99999;      //constant representing no time license limit
const int CK_InfiniteUnits      = 999999;     //constant representing no units limit
const int CK_InfiniteHistSize   = 999999999;  //constant representing no maximum historian file size
const UINT CK_InfiniteHistFiles = 99999;      //constant representing no maximum number of historian files
const int CK_InfiniteTrends     = 99999;      //constant representing no maximum number of trend windows
const int CK_InfiniteGrfs       = 99999;      //constant representing no maximum number of graphics windows

const int LicInit_OK          = 0;
const int LicInit_GoDemo      = 1;
const int LicInit_GoAcademic  = 2;
const int LicInit_ChgLoc      = 3;
const int LicInit_ExitReqd    = 4;

//===========================================================================
//=== Generic Code...
//generic base class for CrypKey Licensing
class DllImportExport CLicense
  {
  public:
    CLicense();
    virtual ~CLicense();
    int             Init(char* path = NULL, char* ProgFiles=NULL);
#if CURTIN_ACADEMIC_LICENSE
    BOOL            InitFromScdLicense();
#endif
    int             CheckLicenseVersionOK(LPCTSTR SysCADEXEPath);
    BOOL            Check(BOOL Prompt = FALSE);
    BOOL            QuickCheck(UCHAR CheckLevel = 0); //0,1 or 2
    void            Exit();
    int             Copies();
    int             MultiUsers();
    BOOL            StartDialog();
    BOOL            IssueTrial(int NoOfDays, BOOL Prompt);
    bool            ChangeLocation();
    void            SaveLocation();
    int             SetLocation();
    void            SetUseCOM(bool On=false);
    BOOL            UseCOM() { return m_bUseCOM; };
    BOOL            IssueLicense();
    BOOL            DoIssue(char* key);
    BOOL            DoRegisterTransfer();
    BOOL            DoTransferOut();
    BOOL            DoTransferIn();
    BOOL            DoDirectTransfer();
    BOOL            Kill(CString& ConfirmCode);
    int             CheckService();
    BOOL            ResetService(BOOL StopAndStart);
    void            SetDemoMode();
    void            SetAcademicMode();
    BOOL            NetworkUsersInfo();
    BOOL            NetworkUsersInfo(char* Buff);
    char *          SGTime(ULONG ltime);
    virtual char*   GetAppVersion() = 0;
    virtual DWORD   GetDemoOptions() = 0;
    virtual DWORD   GetAcademicOptions() = 0;
    virtual DWORD   GetTrialOptions() = 0;
    virtual void    Info();

    virtual dword   FixOptions(dword dwOpLevel) { return dwOpLevel; };
    virtual void    CheckForLiteModes() {};

    void            DumpState(LPCTSTR Where);
    void            PushState();
    void            PopState();

    inline BOOL     DidInitCrypkey()    { return m_bDidInitCrypkey; };
    inline BOOL     UsingSecurity()     { return 1; };
    inline DWORD    OpLevel()           { return m_State.m_dwOpLevel; };
    inline BOOL     Licensed()          { return m_State.m_bLicensed; };
    inline BOOL     MultiUserFailure()  { return m_State.m_bMultiUserFailure; };
#if CURTIN_ACADEMIC_LICENSE
    inline BOOL     HashDefineCurtin()  { return true; };
    inline BOOL     AcademicMode()      { return m_State.m_bAcademicMode; };
    inline LPCTSTR  AcademicName()      { return m_sAcademicName; };
#else
    inline BOOL     HashDefineCurtin(){ return false; };
    inline BOOL     AcademicMode()      { return false; };
    inline LPCTSTR  AcademicName()      { return ""; };
#endif
    inline BOOL     DemoMode()          { return m_State.m_bDemoMode; };
    inline BOOL     TrialMode()         { return m_State.m_bTrialMode; };
    inline BOOL     TrialFailed()       { return m_State.m_bTrialFailed; };
    inline void     SetBlocked(BOOL Block=TRUE);
    inline BOOL     Blocked();
    inline int      DaysLeft()           { return m_iDaysLeft; };
    inline void     SetAppPath(char* p);
    inline char*    GetAppPath()       { return (m_sAppPath.GetLength()==0 ? NULL : (char*)(const char*)m_sAppPath); };
    inline void     SetEmbedded(bool On) { m_bEmbedded=On; }
    void            Error(char * fmt, ...);
  protected:
    long            Challenge32(long companyNum, long passNumDiv2, long random1, long random2);
    int             UpdateCrypkeyINI(char* path);

  protected:
    CString m_sLastPath;        //path used for transfers
    CString m_sAppPath;         //optional path and name of app for license location
    int     m_iDaysLeft;        //number of days left for license (CK_InfiniteDays = infinite)
    BOOL    m_bDidInitCrypkey;  //true if CrypKey was initialized properly
    CString m_sProgFiles;       //path of binaries
#if CURTIN_ACADEMIC_LICENSE
    CString m_sLicFile;         //SysCAD license file
    CString m_sAcademicName;
#endif
    struct
      {
      DWORD m_dwOpLevel;        //current value of 32bit security options
      BOOL  m_bMultiUserFailure,//true if GetAuth failed due to too many users
            m_bLicensed,        //true if the application is currently licensed
            m_bAcademicMode,     // Licensed Using a LicenseFile
            m_bDemoMode,        //true if in DemoMode ie unlicensed
            m_bTrialMode,       //true if current license is operating in trial mode
            m_bTrialFailed,     //true if attempt to issue trial license has been made AND failed
            m_bBlocked,         //set true if license condition failed and all functionality must be blocked
            m_bCOMMineServeOn;
      } m_State, m_StateStack[10];

    int  m_iStackLvl;

    BOOL m_bUseCOM;
    BOOL m_bEmbedded;

  };

//===========================================================================
//=== SysCAD Specific Code...

//bit31 = Option1
//bit30 = Option2 etc
//bit0 and bit1 are used for levels

typedef struct
  {
  DWORD Level            : 2,  //allows 4 levels
        Reserved82       : 1,  // Option30  0x0001   = Reserved Version 8.2                     |
        Reserved90       : 1,  // Option29  0x0001   = Reserved Version 9.0                     |
        Spare_28         : 1,  // Option28  0x0001                                              |
        Client_Other     : 1,  // Option27  0x0001   = Custom user/client models                |
        Client_RTTS      : 1,  // Option26  0x0001   = RTTS models                              |
        Client_QAL       : 1,  // Option25  0x0001   = QAL Standard                             |
        Client_MineServe : 1,  // Option24  0x0001   = Gijima MineServe                         |
        Client_Alcan     : 1,  // Option23  0x0001   = Alcan models                             |
        Spare_22         : 1,  // Option22  0x0001                                              |
        Spare_21         : 1,  // Option21  0x0001                                              |
        Mdls_SMDKRuntime : 1,  // Option20  0x0001   = SMDK Runtime (BlackBox) models           |       
        Mdls_Electrical  : 1,  // Option19  0x0001   = KW Electrical                            |       
        Mdls_Alumina     : 1,  // Option18  0x0001   = KW Alumina                               |       
        Mdls_SizeDist    : 1,  // Option17  0x0001   = KW Size-distribution/communition models  |       
        Mdls_HeatExtra   : 1,  // Option16  0x0001   = KW HeatEx PowerPlant models              |       
        Mdls_HeatBal     : 1,  // Option15  0x0001   = KW HeatEx models!                        |       
        Func_Spare2      : 1,  // Option14  0x0001   =                                          |
        Func_Spare1      : 1,  // Option13  0x0001   =                                          |
        Func_OPCServer   : 1,  // Option12  0x0001   = OPC Server                               |     
        Func_Drivers     : 1,  // Option11  0x0001   = Drivers + OPC client                     |     
        Func_COMProp     : 1,  // Option10  0x0001   = COM Properties                           |
        Func_COM         : 1,  // Option9   0x0001   = Full VB COM interface                    |     
        Func_Academic    : 1,  // Option8   0x0001   = Academic License                         |
        Func_FullEdit    : 1,  // Option7   0x0001   = Full license!                            |
        Mode_Spare       : 1,  // Option6   0x0001                                              |
        Only_SteadyState : 1,  // Option5   0x0001   = Steady State Only / No Integration       |  if set then integrators and similar are put into Hold mode
        Mode_Electrical  : 1,  // Option4   0x0001   = Electrical                               |
        Mode_DynamicFull : 1,  // Option3   0x0001   = Dynamic Full                             |
        Mode_DynamicFlow : 1,  // Option2   0x0001   = Dynamic Flow                             |
        Mode_ProBal      : 1;  // Option1   0x0001   = ProBal mode!                             |
  } CK_SysCADSecurityOpts;

typedef struct
  {
  union
    {
    CK_SysCADSecurityOpts m_Opts;
    DWORD m_OpLevel;
    };
  } CK_SysCADSecurity;

//#endif

enum eLicOptions { eLic_None, eLic_Msg, eLic_MsgBox };
class DllImportExport CSysCADLicense : public CLicense
  {
  public:
    CSysCADLicense();
    virtual ~CSysCADLicense();
    virtual char*     GetAppVersion();
    virtual DWORD     GetDemoOptions();
    virtual DWORD     GetAcademicOptions();
    virtual DWORD     GetTrialOptions();
    virtual void      Info();
    virtual dword     FixOptions(dword dwOpLevel);
    virtual void      CheckForLiteModes();

    void              BumpNodeCount(int Count, LPTSTR Tag);
    int               NodeCount();
    BOOL              NodeCountExceeded(int Extra=0, eLicOptions Opts=eLic_None);
    void              BumpIllegalNodeCount(int Count, LPTSTR ClassId, eLicOptions Opts=eLic_None);
    int               IllegalNodeCount(eLicOptions Opt=eLic_None);
    void              BumpIllegalModelCount(int Count, LPTSTR ClassId, eLicOptions Opts=eLic_None);
    int               IllegalModelCount(eLicOptions Opt=eLic_None);
    int               IllegalNodeModelCount(eLicOptions Opt=eLic_None);

    int               MaxNodesAllowed(BOOL ForDynamic);
    int               MaxNodesAllowed();                                 
    long              MaxHistSizeAllowed();
    UINT              MaxHistFilesAllowed();
    int               TrendWindowsAllowed();
    int               GraphicWindowsAllowed();
    DWORD             LicCatagories();

    inline void       SetAsRunTime();
    inline BOOL       IsRunTime();
    //inline BOOL       AllowVer90();
    inline BOOL       AllowMdlsRTTS();
    inline BOOL       AllowMineServe();
    inline BOOL       AllowMineServeCOM();
    inline void       SetForMineServe(bool On);
    inline void       SetForMineServeMsg(WPARAM wParam, LPARAM lParam);
    inline BOOL       AllowMdlsAlcan();
    //inline BOOL       AllowMdlsQALExtra();
    inline BOOL       AllowMdlsQAL();
    inline BOOL       AllowMdlsUser();
    inline BOOL       AllowMdlsSMDKRuntime();
    inline BOOL       AllowMdlsHeatExtra();
    inline BOOL       AllowMdlsHeatBal();
    inline BOOL       AllowMdlsAlumina();
    inline BOOL       AllowMdlsSizeDist();
    inline BOOL       AllowMdlsElec();
    inline BOOL       AllowProBal();
    inline BOOL       AllowDynamicFlow();
    inline BOOL       AllowDynamicFull();
    inline BOOL       AllowProBalLite();
    inline BOOL       AllowDynamicLite();
    inline BOOL       OnlySteadyState();
    inline BOOL       IsAcademic();
    inline BOOL       AllowFullLic();
    inline BOOL       AllowFullLicFlag();
    inline BOOL       AllowDrivers();
    inline BOOL       AllowOPCServer();
    inline BOOL       AllowCOMInterface();
    inline BOOL       AllowCOMProps();
    inline UCHAR      Level();



    BOOL              ProbalLiteMode() { return m_bProbalLiteMode; };
    BOOL              DynLiteMode()    { return m_bDynLiteMode;    };

    BOOL              AllowDynamic();
    BOOL              ProBalOK();
    BOOL              DynamicOK();


#if CK_SCDCOMLIC
    inline BOOL       AllowCOMMdl()     ;//{ return AllowCOMProps() || ForMineServe(); };
    inline BOOL       AllowCOMSlv()     ;//{ return AllowCOMProps() || ForMineServe(); };
    inline BOOL       AllowCOMApp()     ;//{ return AllowCOMInterface() || ForMineServe(); };
#else
    inline BOOL       AllowCOMMdl()     ;//{ return 1; };
    inline BOOL       AllowCOMSlv()     ;//{ return 1; };
    inline BOOL       AllowCOMApp()     ;//{ return 1; };
#endif

    void              CheckLicense(BOOL StartingSolve, bool WithIOOn, int TrendWndCount, int GraphicWndCount);
    void              DoCheckLicense(WPARAM wParam, LPARAM lParam);
    void              CheckLicenseDate();
    void              CheckLicenseConditions(bool IOMOn, int TrendWndCount, int GraphicWndCount);

  protected:
    CK_SysCADSecurity* m_pSecOpt;

    BOOL              m_bProbalLiteMode;
    BOOL              m_bDynLiteMode;

    int               m_NodeCount;
    int               m_IllegalNodeCount;
    int               m_IllegalModelCount;

    //bool              m_bIOMOn;
    //int               m_TrendWndCount;
    //int               m_GraphicWndCount;
    DWORD             m_PrevCheckTicks;
    DWORD             m_PrevCrypkeyTicks;

  };

extern DllImportExport CSysCADLicense gs_License;

class DllImportExport CEncryptNDemos
{
public:
  int encryptedPGM;
  int encryptedRCT;

  bool isDemo;

  CEncryptNDemos(); 
  ~CEncryptNDemos(); 

  void CheckForDemo(char* projectFolder);

  bool IsEncrypted();
};

extern DllImportExport CEncryptNDemos gs_EncryptNDemos;

#undef DllImportExport 

//===========================================================================
#endif