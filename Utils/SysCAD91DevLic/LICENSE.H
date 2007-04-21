//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

//set to 1 if licensing/security is required
#define CK_LICENSINGON    1

#if CK_LICENSINGON
#define CK_ALLOWDEMOMODE 1        //set to 1 if SysCAD demo mode is allowed (no license)
#endif

//---------------------------------------------------------------------------

#if CK_LICENSINGON

//days...
const int CK_WarnDays = 21;            //no of days to give clear warning before license expires
const int CK_UrgentWarnDays = 7;       //no of days to give urgent warning (dialog) before license expires
#if CK_ALLOWDEMOMODE
const int CK_NoOfTrialDays = 12;       //Number of days trial license lasts for
#else
const int CK_NoOfTrialDays = 45;       //Number of days trial license lasts for
#endif
#endif

const int CK_InfiniteDays = 99999;        //constant representing no time license limit

//===========================================================================
//=== Generic Code...

//generic base class for CrypKey Licensing
class CLicense
  {
#if CK_LICENSINGON
  public:
    CLicense();
    virtual ~CLicense();
    BOOL            Init(char* path = NULL);
    BOOL            Check(BOOL Prompt = FALSE);
    BOOL            QuickCheck(UCHAR CheckLevel = 0); //0,1 or 2
    BOOL            CheckSMDK();
    void            Exit();
    int             Copies();
    int             MultiUsers();
    BOOL            StartDialog();
    BOOL            IssueTrial(int NoOfDays, BOOL Prompt);
    bool            ChangeLocation();
    int             SetLocation(BOOL CheckAndInit = true);
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
    BOOL            NetworkUsersInfo();
    BOOL            NetworkUsersInfo(char* Buff);
    char*           SGTime(ULONG ltime);
    virtual         char* GetAppVersion() = 0;
    virtual         DWORD GetDemoOptions() = 0;
    virtual         DWORD GetTrialOptions() = 0;
    virtual         void Info();

    inline BOOL     DidInitCrypkey()    { return bDidInitCrypkey; };
    inline BOOL     UsingSecurity()     { return 1; };
    inline DWORD    OpLevel()           { return dwOpLevel; };
    inline BOOL     Licensed()          { return bLicensed; };
    inline BOOL     MultiUserFailure()  { return bMultiUserFailure; };
    inline BOOL     DemoMode()          { return bDemoMode; };
    inline BOOL     TrialMode()         { return bTrialMode; };
    inline BOOL     TrialFailed()       { return bTrialFailed; };
    inline void     SetBlocked(BOOL Block=TRUE) { bBlocked = Block; };
    inline BOOL     Blocked()           { return bBlocked; };
    inline int      DaysLeft()          { return iDaysLeft; };
    inline void     SetAppPath(char* p) { sAppPath = p; };
    inline char*    GetAppPath()        { return (sAppPath.GetLength()==0 ? NULL : (char*)(const char*)sAppPath); };

    inline BOOL     SMDK_Units()        { return bSMDK_Units; };
    inline BOOL     SMDK_Props()        { return bSMDK_Props; };

  protected:
    void            Error(char * fmt, ...);
    long            Challenge32(long companyNum, long passNumDiv2, long random1, long random2);
    int             UpdateCrypkeyINI(char* path);

  protected:
    CString sLastPath;       //path used for transfers
    CString sAppPath;        //optional path and name of app for license location
    DWORD dwOpLevel;         //current value of 32bit security options
    int iDaysLeft;           //number of days left for license (CK_InfiniteDays = infinite)
    BYTE bDidInitCrypkey,    //true if CrypKey was initialized properly
         bMultiUserFailure,  //true if GetAuth failed due to too many users
         bLicensed,          //true if the application is currently licensed
         bSMDK_Units,
         bSMDK_Props,
         bLicFileMode,       // Licensed Using a LicenseFile
         bDemoMode,          //true if in DemoMode ie unlicensed
         bTrialMode,         //true if current license is operating in trial mode
         bTrialFailed,       //true if attempt to issue trial license has been made AND failed
         bBlocked;           //set true if license condition failed and all functionality must be blocked

#else
  public:
    inline BOOL     UsingSecurity() { return 0; };
    inline BOOL     Blocked() { return 0; };
#endif
  };

//===========================================================================
//=== KWA Specific Code...

#if CK_LICENSINGON
//bit31 = Option1
//bit30 = Option2 etc
//no bits are used for levels

typedef struct
  {
  DWORD Spare0           : 1,
        Spare1           : 1,
        Spare2           : 1,
        Spare3           : 1,
        Spare4           : 1,
        Spare5           : 1,
        Spare6           : 1,
        Spare7           : 1,
        Spare8           : 1,
        Spare9           : 1,
        Spare10          : 1,
        Spare11          : 1,
        Spare12          : 1,
        Spare13          : 1,
        Spare14          : 1,
        Spare15          : 1,
        Spare16          : 1,
        Spare17          : 1,
        Spare18          : 1,
        Spare19          : 1,
        InfoStream       : 1,
        ELMS_Inst        : 1,
        ELMS_Elec        : 1,
        Spare23          : 1,
        Spare24          : 1,
        Spare25          : 1,
        Spare26          : 1,
        IntelliLink      : 1,
        ELMS_3D          : 1,
        ELMS_Sizing      : 1,
        PBOpt2           : 1,  //PhotoBuilder option
        PBOpt1           : 1;  //PhotoBuilder option
  } CK_KWASecurityOpts;

typedef struct
  {
  union
    {
    CK_KWASecurityOpts Opts;
    DWORD OpLevel;
    };
  } CK_KWASecurity;

#endif

class CKWALicense : public CLicense
  {
#if CK_LICENSINGON
  public:
    CKWALicense();
    virtual char* GetAppVersion();
    virtual DWORD GetDemoOptions();
    virtual DWORD GetTrialOptions();
    virtual void Info();

  protected:
    CK_KWASecurity* pSecOpt;
#else
  public:
#endif
  };

extern CKWALicense Lic;

//===========================================================================
