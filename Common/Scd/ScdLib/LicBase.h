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
const int CK_WarnDays = 21;            //no of days to give clear warning before license expires
const int CK_UrgentWarnDays = 7;       //no of days to give urgent warning (dialog) before license expires
//no of units...
const int CK_Demo_ProbalUnits = 40;    //no of units (nodes+links) allowed in demo mode
const int CK_Demo_DynUnits = 16;       //no of units (nodes+links) allowed in demo mode
const int CK_Lite_ProbalUnits = 100;   //no of units allowed for "probal testing" with Full Dynamic license
const int CK_Lite_DynUnits = 40;       //no of units allowed for "dynamic testing" with Full ProBal license
const int CK_Trial_ProbalUnits = 400;  //no of units allowed while trial license has been issued
const int CK_Trial_DynUnits = 160;     //no of units allowed while trial license has been issued
//historian...
const long CK_DemoHistSize = 640;      //max historian data file size (multiple of 64kb) in demo mode
const long CK_TrialHistSize = 2048;    //max historian data file size (multiple of 64kb) for trial license
const long CK_NormalHistSize = 2048;   //max historian data file size (multiple of 64kb) for normal license
const UINT CK_DemoHistFiles = 3;       //no of historian data files allowed in demo mode (min = 2)
const UINT CK_TrialHistFiles = 3;      //no of historian data files allowed for trial license (min = 2)
const UINT CK_NormalHistFiles = 6;     //no of historian data files allowed for normal license (min = 2)
//trend windows...
const int CK_DemoTrends = 6;           //max no of trend windows allowed in demo mode
const int CK_TrialTrends = 10;         //max no of trend windows allowed for trial license
//graphics windows...
const int CK_DemoGrfs = 3;             //max no of graphics windows allowed in demo mode
const int CK_TrialGrfs = 8;            //max no of graphics windows allowed for trial license
//levels...
const byte CK_TrialLevel = 0;          //level
const byte CK_NormalLevel = 1;         //level
const byte CK_Unused2Level = 2;        //level
const byte CK_Unused3Level = 3;        //level

extern DllImportExport const char* LicINISection;

#if CK_ALLOWDEMOMODE
const int CK_NoOfTrialDays = 12;       //Number of days trial license lasts for
#else
const int CK_NoOfTrialDays = 45;       //Number of days trial license lasts for
#endif

const int CK_InfiniteDays = 99999;        //constant representing no time license limit
const int CK_InfiniteUnits = 999999;      //constant representing no units limit
const int CK_InfiniteHistSize = 999999999;//constant representing no maximum historian file size
const UINT CK_InfiniteHistFiles = 99999;  //constant representing no maximum number of historian files
const int CK_InfiniteTrends = 99999;      //constant representing no maximum number of trend windows
const int CK_InfiniteGrfs = 99999;        //constant representing no maximum number of graphics windows

//===========================================================================
//=== Generic Code...

//generic base class for CrypKey Licensing
class DllImportExport CLicense
  {
//#if CK_LICENSINGON
  protected:
    CString sLastPath;       //path used for transfers
    CString sAppPath;        //optional path and name of app for license location
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

  public:
    CLicense();
    virtual ~CLicense();
    BOOL Init(char* path = NULL);
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
    inline void SetAppPath(char* p) { sAppPath = p; };
    inline char* GetAppPath()       { return (sAppPath.GetLength()==0 ? NULL : (char*)(const char*)sAppPath); };
  protected:
    void Error(char * fmt, ...);
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
/*  DWORD Level            : 2,  //allows 4 levels
        Spare0           : 1,  // 3
        Spare1           : 1,  // 4
        Spare2           : 1,  // 5
        Spare3           : 1,  // 6
        Spare4           : 1,  // 7
        Spare5           : 1,  // 8
        Spare6           : 1,  // 9
        Spare7           : 1,  //10
        Spare8           : 1,  //11
        COMInterface     : 1,  //12 allow access to VB COM interface
        RTTSMdls         : 1,  //13 allow Rio Tinto Technical Services owned models
        IritronMdls      : 1,  //14 allow Iritron owned models
        AlcanMdls        : 1,  //15 allow Alcan owned models
        OPCServer        : 1,  //16 allow OPC Server funcionality
        QALMdlsExtra     : 1,  //17 allow additional QAL owned models
        QALMdls          : 1,  //18 allow standard QAL owned models
        KaiserMdls       : 1,  //19 allow Kaiser owned models
        UserMdls         : 1,  //20 allow custom user/client models
        AluminaMdls      : 1,  //21 allow Kenwalt alumina models
        SizeDistMdls     : 1,  //22 allow Kenwalt size-distribution/communition models
        Dynamic          : 1,  //23 allow dynamic solver mode for Version 8.0 & 8.1
        Probal           : 1,  //24 allow probal solver mode for Version 8.0 & 8.1
        Analyse          : 1,  //25 allow Design modules / Analyse solver mode
        FullHistorian    : 1,  //26 allow full historian
        Drivers          : 1,  //27 allow drivers including OPC client
        Dynamic82        : 1,  //28 allow dynamic solver mode for Version 8.2
        Probal82         : 1,  //29 allow probal solver mode for Version 8.2
        FullLic          : 1,  //30 allow full license (if false then run-time)
        BlackBoxMdls     : 1,  //31 allow Kenwalt BlackBox models  (was probal solver mode for Version 7.x)
        HeatBalMdls      : 1;  //32 allow Kenwalt HeatEx models  (was dynamic solver mode for Version 7.x)
  } CK_SysCADSecurityOpts;*/
  DWORD Level            : 2,  //allows 4 levels
        Ver82            : 1,  // 3 allow SysCAD 8.2
        Ver90            : 1,  // 4 allow SysCAD 9.0
        Spare_Ver91      : 1,  // 5
        Spare_Ver92      : 1,  // 6
        Mode_ProBal      : 1,  // 7 allow ProBal mode
        Mode_DynamicFlow : 1,  // 8 allow Dynamic flow based/mathand mode
        Mode_DynamicFull : 1,  // 9 allow detailed Dynamic pressure based mode
        Mode_Electrical  : 1,  //10 allow Electrical design & simulation mode
        Spare1           : 1,
        //Only_SteadyState : 1,  //11 if set then integrators and similar are put into Hold mode
        Func_FullEdit    : 1,  //12 allow full license (if false then run-time)
        Func_COM         : 1,  //13 allow access to full VB COM interface
        Func_COMProp     : 1,  //14 allow COM interface to object properties
        Func_Drivers     : 1,  //15 allow drivers including OPC client 
        Func_OPCServer   : 1,  //16 allow OPC Server funcionality
        Mdls_Electrical  : 1,  //17 allow Kenwalt Electrical & Power models
        Mdls_SMDKRuntime : 1,  //18 allow SMDK runtime (BlackBox) models
        Mdls_HeatExtra   : 1,  //19 allow Kenwalt HeatEx PowerPlant models
        Mdls_HeatBal     : 1,  //20 allow Kenwalt HeatEx models
        Mdls_Alumina     : 1,  //21 allow Kenwalt alumina models
        Mdls_SizeDist    : 1,  //22 allow Kenwalt size-distribution/communition models
        Old_Dynamic      : 1,  //23 unused (was dynamic solver mode for Version 8.0 & 8.1)
        Old_Probal       : 1,  //24 unused (was probal solver mode for Version 8.0 & 8.1)
        Client_MineServe : 1,  //25 GMSI MineServe Mdls
        Client_Alcan     : 1,  //26 allow Alcan owned models
        Client_RTTS      : 1,  //27 allow Rio Tinto Technical Services owned models
        Old_Dynamic82    : 1,  //28 keep 0 (was dynamic solver mode for Version 8.2)
        Old_Probal82     : 1,  //29 keep 0 (was probal solver mode for Version 8.2)
        Client_QALExtra  : 1,  //30 allow additional QAL owned models
        Client_QAL       : 1,  //31 allow standard QAL owned models
        Client_Other     : 1;  //32 allow custom user/client models
  } CK_SysCADSecurityOpts;
/*  DWORD Level            : 2,  //allows 4 levels
        Mode_ProBal      : 1,  // 3 allow ProBal mode
        Mode_DynamicFlow : 1,  // 4 allow Dynamic flow based/mathand mode
        Mode_DynamicFull : 1,  // 5 allow detailed Dynamic pressure based mode
        Mode_Electrical  : 1,  // 6 allow Electrical design & simulation mode
        Only_SteadyState : 1,  // 7 if set then integrators and similar are put into Hold mode
        Spare_8          : 1,  // 8
        Func_FullEdit    : 1,  // 9 allow full license (if false then run-time)
        Func_COM         : 1,  //10 allow access to full VB COM interface
        Func_COMProp     : 1,  //11 allow COM interface to object properties (not implemented)
        Func_Drivers     : 1,  //12 allow drivers including OPC client 
        Func_OPCServer   : 1,  //13 allow OPC Server funcionality
        Spare_14         : 1,  //14
        Spare_15         : 1,  //15
        Mdls_HeatBal     : 1,  //16 allow Kenwalt HeatEx models
        Mdls_HeatExtra   : 1,  //17 allow Kenwalt HeatEx PowerPlant models
        Mdls_SizeDist    : 1,  //18 allow Kenwalt size-distribution/communition models
        Mdls_Alumina     : 1,  //19 allow Kenwalt alumina models
        Mdls_Electrical  : 1,  //20 allow Kenwalt Electrical & Power models
        Mdls_SMDKRuntime : 1,  //21 allow SMDK runtime (BlackBox) models
        Spare_22         : 1,  //22
        Spare_23         : 1,  //23
        Academic         : 1,  //24
        Spare_25         : 1,  //25
        Spare_26         : 1,  //26
        Spare_27         : 1,  //27
        Client_MineServe : 1,  //28 GMSI MineServe Mdls
        Client_Alcan     : 1,  //29 allow Alcan owned models
        Client_RTTS      : 1,  //30 allow Rio Tinto Technical Services owned models
        Client_QAL       : 1,  //31 allow standard QAL owned models
        Client_Other     : 1;  //32 allow custom user/client models
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

class DllImportExport CSysCADLicense : public CLicense
  {
  protected:
    BYTE bProbalLiteMode,
         bDynLiteMode;
    CK_SysCADSecurity* pSecOpt;

  public:
    CSysCADLicense();
    virtual ~CSysCADLicense();
    virtual char* GetAppVersion();
    virtual DWORD GetDemoOptions();
    virtual DWORD GetTrialOptions();
    virtual void Info();
    virtual dword FixOptions(dword dwOpLevel);
    virtual void CheckForLiteModes();

    int ProbalUnitsAllowed();
    int DynUnitsAllowed();
    long MaxHistSizeAllowed();
    UINT MaxHistFilesAllowed();
    int TrendWindowsAllowed();
    int GraphicWindowsAllowed();
    DWORD LicCatagories();

    inline void SetAsRunTime();
    inline BOOL IsRunTime();
    inline BOOL AllowVer90();
    inline BOOL AllowMdlsRTTS();
    inline BOOL ForMineServe();
    inline void SetForMineServe(bool On);
    inline BOOL AllowMdlsAlcan();
    inline BOOL AllowMdlsQALExtra();
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
    //inline BOOL OnlySteadyState();
    inline BOOL AllowFullLic();
    inline BOOL AllowFullLicFlag();
    inline BOOL AllowDrivers();
    inline BOOL AllowOPCServer();
    inline BOOL AllowCOMInterface();
    inline BOOL AllowCOMProps();
    inline UCHAR Level();

#if CK_SCDCOMLIC
    inline BOOL AllowCOMMdl()     ;//{ return AllowCOMProps() || ForMineServe(); };
    inline BOOL AllowCOMSlv()     ;//{ return AllowCOMProps() || ForMineServe(); };
    inline BOOL AllowCOMApp()     ;//{ return AllowCOMInterface() || ForMineServe(); };
#else
    inline BOOL AllowCOMMdl()     ;//{ return 1; };
    inline BOOL AllowCOMSlv()     ;//{ return 1; };
    inline BOOL AllowCOMApp()     ;//{ return 1; };
#endif

  };

extern DllImportExport CSysCADLicense gs_License;

#undef DllImportExport 

//===========================================================================
#endif