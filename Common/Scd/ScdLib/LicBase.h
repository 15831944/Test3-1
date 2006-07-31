//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __LICBASE_H
#define __LICBASE_H

#include "scdver.h"

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
const int CK_WarnDays           = 21;    //no of days to give clear warning before license expires
const int CK_UrgentWarnDays     = 7;     //no of days to give urgent warning (dialog) before license expires
//no of units...
const int CK_Demo_ProbalUnits   = 25;    //no of units (nodes, not links) allowed in demo mode
const int CK_Demo_DynUnits      = 10;    //no of units (nodes, not links) allowed in demo mode
const int CK_Lite_ProbalUnits   = 50;    //no of units allowed for "probal testing" with Full Dynamic license
const int CK_Lite_DynUnits      = 20;    //no of units allowed for "dynamic testing" with Full ProBal license
const int CK_Trial_ProbalUnits  = 200;   //no of units allowed while trial license has been issued
const int CK_Trial_DynUnits     = 50;    //no of units allowed while trial license has been issued
//historian...
const long CK_DemoHistSize      = 640;   //max historian data file size (multiple of 64kb) in demo mode
const long CK_TrialHistSize     = 2048;  //max historian data file size (multiple of 64kb) for trial license
const long CK_NormalHistSize    = 2048;  //max historian data file size (multiple of 64kb) for normal license
const UINT CK_DemoHistFiles     = 3;     //no of historian data files allowed in demo mode (min            = 2)
const UINT CK_TrialHistFiles    = 3;     //no of historian data files allowed for trial license (min            = 2)
const UINT CK_NormalHistFiles   = 6;     //no of historian data files allowed for normal license (min            = 2)
//trend windows...
const int CK_DemoTrends         = 6;     //max no of trend windows allowed in demo mode
const int CK_TrialTrends        = 10;    //max no of trend windows allowed for trial license
//graphics windows...
const int CK_DemoGrfs           = 3;     //max no of graphics windows allowed in demo mode
const int CK_TrialGrfs          = 8;     //max no of graphics windows allowed for trial license
//levels...
const byte CK_TrialLevel        = 0;     //level
const byte CK_NormalLevel       = 1;     //level
const byte CK_Unused2Level      = 2;     //level
const byte CK_Unused3Level      = 3;     //level

extern DllImportExport const char* LicINISection;

#if CK_ALLOWDEMOMODE
const int CK_NoOfTrialDays      = 12;       //Number of days trial license lasts for
#else
const int CK_NoOfTrialDays      = 45;       //Number of days trial license lasts for
#endif

const int CK_InfiniteDays       = 99999;      //constant representing no time license limit
const int CK_InfiniteUnits      = 999999;     //constant representing no units limit
const int CK_InfiniteHistSize   = 999999999;  //constant representing no maximum historian file size
const UINT CK_InfiniteHistFiles = 99999;      //constant representing no maximum number of historian files
const int CK_InfiniteTrends     = 99999;      //constant representing no maximum number of trend windows
const int CK_InfiniteGrfs       = 99999;      //constant representing no maximum number of graphics windows

//===========================================================================
//=== Generic Code...

//generic base class for CrypKey Licensing
class DllImportExport CLicense
  {
//#if CK_LICENSINGON
  protected:
    CString m_sLastPath;     //path used for transfers
    CString m_sAppPath;      //optional path and name of app for license location
    DWORD dwOpLevel;         //current value of 32bit security options
    int iDaysLeft;           //number of days left for license (CK_InfiniteDays = infinite)
    BYTE bDidInitCrypkey:1,  //true if CrypKey was initialized properly
         bMultiUserFailure:1,//true if GetAuth failed due to too many users
         bLicensed:1,        //true if the application is currently licensed
         bDemoMode:1,        //true if in DemoMode ie unlicensed
         bTrialMode:1,       //true if current license is operating in trial mode
         bTrialFailed:1,     //true if attempt to issue trial license has been made AND failed
         bBlocked:1;         //set true if license condition failed and all functionality must be blocked

    BOOL bCOMMineServeOn;

    BOOL m_bUseCOM;
    BOOL m_bEmbedded;

  public:
    CLicense();
    virtual ~CLicense();
    BOOL Init(char* path = NULL);
    static BOOL CheckLicenseVersionOK(LPCTSTR SysCADEXEPath);
    BOOL Check(BOOL Prompt = FALSE);
    BOOL QuickCheck(UCHAR CheckLevel = 0); //0,1 or 2
    void Exit();
    int Copies();
    int MultiUsers();
    BOOL StartDialog();
    BOOL IssueTrial(int NoOfDays, BOOL Prompt);
    int SetLocation(BOOL CheckAndInit = true);
    void SetUseCOM(BOOL On=false);
    BOOL UseCOM() { return m_bUseCOM; };
    BOOL IssueLicense();
    BOOL DoIssue(char* key);
    BOOL DoRegisterTransfer();
    BOOL DoTransferOut();
    BOOL DoTransferIn();
    BOOL DoDirectTransfer();
    BOOL Kill(CString& ConfirmCode);
    int CheckService();
    BOOL ResetService(BOOL StopAndStart);
    void SetDemoMode();
    BOOL NetworkUsersInfo();
    BOOL NetworkUsersInfo(char* Buff);
    char* SGTime(ULONG ltime);
    virtual char* GetAppVersion() = 0;
    virtual DWORD GetDemoOptions() = 0;
    virtual DWORD GetTrialOptions() = 0;
    virtual void Info();

    virtual dword FixOptions(dword dwOpLevel) { return dwOpLevel; };
    virtual void CheckForLiteModes() {};

    inline BOOL DidInitCrypkey()    { return bDidInitCrypkey; };
    inline BOOL UsingSecurity()     { return 1; };
    inline DWORD OpLevel()          { return dwOpLevel; };
    inline BOOL Licensed()          { return bLicensed; };
    inline BOOL MultiUserFailure()  { return bMultiUserFailure; };
    inline BOOL DemoMode()          { return bDemoMode; };
    inline BOOL TrialMode()         { return bTrialMode; };
    inline BOOL TrialFailed()       { return bTrialFailed; };
    inline void SetBlocked(BOOL Block=TRUE) { bBlocked = Block; };
    inline BOOL Blocked()           { return bBlocked; };
    inline int DaysLeft()           { return iDaysLeft; };
    inline void SetAppPath(char* p);
    inline char* GetAppPath()       { return (m_sAppPath.GetLength()==0 ? NULL : (char*)(const char*)m_sAppPath); };
    inline void SetEmbedded(bool On) { m_bEmbedded=On; }
    void Error(char * fmt, ...);
  protected:
    //UINT Challenge(ULONG companyNum, ULONG passNum, UINT random1, UINT random2);
    long Challenge32(long companyNum, long passNumDiv2, long random1, long random2);
    int UpdateCrypkeyINI(char* path);
//#else
//  public:
//    inline BOOL UsingSecurity() { return 0; };
//    inline BOOL Blocked() { return 0; };
//    void Exit() {};
//#endif
  };

//===========================================================================
//=== SysCAD Specific Code...

//bit31 = Option1
//bit30 = Option2 etc
//bit0 and bit1 are used for levels

typedef struct
  {
  DWORD Level            : 2,  //allows 4 levels
        Client_Other     : 1,  // Option30 = Custom user/client models                |
        Client_QAL       : 1,  // Option29 = QAL Standard                             |
        Client_RTTS      : 1,  // Option28 = RTTS models                              |
        Client_Alcan     : 1,  // Option27 = Alcan models                             |
        Client_MineServe : 1,  // Option26 = Gijima MineServe                         |
        Spare_22         : 1,  // Option25                                            |
        Spare_23         : 1,  // Option24                                            |
        Spare_25         : 1,  // Option23                                            |
        Spare_26         : 1,  // Option22                                            |
        Spare_27         : 1,  // Option21                                            |
        Mdls_SMDKRuntime : 1,  // Option20 = SMDK Runtime (BlackBox) models           |       
        Mdls_Electrical  : 1,  // Option19 = KW Electrical                            |       
        Mdls_Alumina     : 1,  // Option18 = KW Alumina                               |       
        Mdls_SizeDist    : 1,  // Option17 = KW Size-distribution/communition models  |       
        Mdls_HeatExtra   : 1,  // Option16 = KW HeatEx PowerPlant models              |       
        Mdls_HeatBal     : 1,  // Option15 = KW HeatEx models!                        |       
        Func_Spare2      : 1,  // Option14 =                                          |
        Func_Spare1      : 1,  // Option13 =                                          |
        Func_OPCServer   : 1,  // Option12 = OPC Server                               |     
        Func_Drivers     : 1,  // Option11 = Drivers + OPC client                     |     
        Func_COMProp     : 1,  // Option10 = COM Properties                           |
        Func_COM         : 1,  // Option9  = Full VB COM interface                    |     
        Func_Academic    : 1,  // Option8  = Academic License                         |
        Func_FullEdit    : 1,  // Option7  = Full license!                            |
        Mode_Spare       : 1,  // Option6                                             |
        Only_SteadyState : 1,  // Option5  = Steady State Only / No Integration       |  if set then integrators and similar are put into Hold mode
        Mode_Electrical  : 1,  // Option4  = Electrical                               |
        Mode_DynamicFull : 1,  // Option3  = Dynamic Full                             |
        Mode_DynamicFlow : 1,  // Option2  = Dynamic Flow                             |
        Mode_ProBal      : 1;  // Option1  = ProBal mode!                             |
  } CK_SysCADSecurityOpts;
/*  DWORD Level          : 2,  //allows 4 levels
        Mode_ProBal      : 1,  // allow ProBal mode
        Mode_DynamicFlow : 1,  // allow Dynamic flow based/mathand mode
        Mode_DynamicFull : 1,  // allow detailed Dynamic pressure based mode
        Mode_Electrical  : 1,  // allow Electrical design & simulation mode
        Only_SteadyState : 1,  // if set then integrators and similar are put into Hold mode
        Spare_8          : 1,  //
        Func_FullEdit    : 1,  // allow full license (if false then run-time)
        Func_COM         : 1,  // allow access to full VB COM interface
        Func_COMProp     : 1,  // allow COM interface to object properties (not implemented)
        Func_Drivers     : 1,  // allow drivers including OPC client 
        Func_OPCServer   : 1,  // allow OPC Server funcionality
        Spare_14         : 1,  //
        Spare_15         : 1,  //
        Mdls_HeatBal     : 1,  // allow Kenwalt HeatEx models
        Mdls_HeatExtra   : 1,  // allow Kenwalt HeatEx PowerPlant models
        Mdls_SizeDist    : 1,  // allow Kenwalt size-distribution/communition models
        Mdls_Alumina     : 1,  // allow Kenwalt alumina models
        Mdls_Electrical  : 1,  // allow Kenwalt Electrical & Power models
        Mdls_SMDKRuntime : 1,  // allow SMDK runtime (BlackBox) models
        Spare_22         : 1,  //
        Spare_23         : 1,  //
        Academic         : 1,  //
        Spare_25         : 1,  //
        Spare_26         : 1,  //
        Spare_27         : 1,  //
        Client_MineServe : 1,  // GMSI MineServe Mdls
        Client_Alcan     : 1,  // allow Alcan owned models
        Client_RTTS      : 1,  // allow Rio Tinto Technical Services owned models
        Client_QAL       : 1,  // allow standard QAL owned models
        Client_Other     : 1;  // allow custom user/client models
  } CK_SysCADSecurityOpts;*/


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
    virtual char* GetAppVersion();
    virtual DWORD GetDemoOptions();
    virtual DWORD GetTrialOptions();
    virtual void Info();
    virtual dword FixOptions(dword dwOpLevel);
    virtual void CheckForLiteModes();

    void      BumpNodeCount(int Count, LPTSTR Tag);
    int       NodeCount();
    BOOL      NodeCountExceeded(int Extra=0, eLicOptions Opts=eLic_None);
    void      BumpIllegalNodeCount(int Count, LPTSTR ClassId, eLicOptions Opts=eLic_None);
    int       IllegalNodeCount(eLicOptions Opt=eLic_None);
    void      BumpIllegalModelCount(int Count, LPTSTR ClassId, eLicOptions Opts=eLic_None);
    int       IllegalModelCount(eLicOptions Opt=eLic_None);

    int       MaxNodesAllowed(BOOL ForDynamic);
    int       MaxNodesAllowed();                                 
    long      MaxHistSizeAllowed();
    UINT      MaxHistFilesAllowed();
    int       TrendWindowsAllowed();
    int       GraphicWindowsAllowed();
    DWORD     LicCatagories();

    inline void SetAsRunTime();
    inline BOOL IsRunTime();
    //inline BOOL AllowVer90();
    inline BOOL AllowMdlsRTTS();
    inline BOOL ForMineServe();
    inline void SetForMineServe(bool On);
    inline BOOL AllowMdlsAlcan();
    //inline BOOL AllowMdlsQALExtra();
    inline BOOL AllowMdlsQAL();
    inline BOOL AllowMdlsUser();
    inline BOOL AllowMdlsSMDKRuntime();
    inline BOOL AllowMdlsHeatExtra();
    inline BOOL AllowMdlsHeatBal();
    inline BOOL AllowMdlsAlumina();
    inline BOOL AllowMdlsSizeDist();
    inline BOOL AllowMdlsElec();
    inline BOOL AllowProBal();
    inline BOOL AllowDynamicFlow();
    inline BOOL AllowDynamicFull();
    inline BOOL AllowProBalLite();
    inline BOOL AllowDynamicLite();
    inline BOOL OnlySteadyState();
    inline BOOL IsAcademic();
    inline BOOL AllowFullLic();
    inline BOOL AllowFullLicFlag();
    inline BOOL AllowDrivers();
    inline BOOL AllowOPCServer();
    inline BOOL AllowCOMInterface();
    inline BOOL AllowCOMProps();
    inline UCHAR Level();



    BOOL       ProbalLiteMode() { return m_bProbalLiteMode; };
    BOOL       DynLiteMode()    { return m_bDynLiteMode;    };

    BOOL       AllowDynamic();
    BOOL       ProBalOK();
    BOOL       DynamicOK();


#if CK_SCDCOMLIC
    inline BOOL AllowCOMMdl()     ;//{ return AllowCOMProps() || ForMineServe(); };
    inline BOOL AllowCOMSlv()     ;//{ return AllowCOMProps() || ForMineServe(); };
    inline BOOL AllowCOMApp()     ;//{ return AllowCOMInterface() || ForMineServe(); };
#else
    inline BOOL AllowCOMMdl()     ;//{ return 1; };
    inline BOOL AllowCOMSlv()     ;//{ return 1; };
    inline BOOL AllowCOMApp()     ;//{ return 1; };
#endif

  protected:
    CK_SysCADSecurity* pSecOpt;

    BOOL        m_bProbalLiteMode,
                m_bDynLiteMode;

    int         m_NodeCount;
    int         m_IllegalNodeCount;
    int         m_IllegalModelCount;

  };

extern DllImportExport CSysCADLicense gs_License;

#undef DllImportExport 

//===========================================================================
#endif