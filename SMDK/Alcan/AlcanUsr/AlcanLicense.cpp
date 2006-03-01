//===========================================================================
//=== SysCAD SMDK - Hyprod Precipitator Model 2003 : Alcan, Kenwalt       ===
//===========================================================================

#include "stdafx.h"
#include "AlcanLicense.h"
#include "crypkey.h"
#include "md_headers.h" 

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//for example.exe...
//#define CK_USER_KEY   "D050 815C D1A2 A79D B1"
//#define CK_MASTER_KEY "2A5D 57C4 1B4C 135B F09E 17F7 600B 2D70 79E8 F275 C36A"
//const char* CK_KeyFile = "example.exe";
//for HYPROD2k.exe...
#define CK_USER_KEY   "DE12 CDBB 9DE0 A74C 6944 00BD 5A"
#define CK_MASTER_KEY "4C39 55C4 245E D5A9 3EC9 08CD FD4F 73A5 EB5E 4EAB AF28"
const char* CK_KeyFile = "HYPROD2k.exe";
const int CK_NetworkChecktime = 900;
const DWORD CK_ALCANUNITS_Mask4 = 0x10000000;    // 2^28 ==> The 4th bit
const DWORD CK_ALCANPROPS_Mask5 = 0x8000000;     // 2^27 ==> The 5th bit
 

bool CheckForLicense(DWORD mask)
  {
  //CWaitCursor Wait;
  char Buff[_MAX_PATH];
  char Msg[2048];
  CString sLicenseLoc;
  //sLicenseLoc = "c:\\test\\"; //probably want to get this from registry or ini file or let user select or ???
  sLicenseLoc = "C:\\Program Files\\HYPROD 2k\\"; //probably want to get this from registry or ini file or let user select or ???
  sprintf(Buff, "%s%s", (const char*)sLicenseLoc, CK_KeyFile);
  //todo:check that this file exists, if not, give an error message
  int err = InitCrypkey( Buff, CK_MASTER_KEY, CK_USER_KEY, FALSE, CK_NetworkChecktime);

  if (err==0)
    {
    DWORD dwOpLevel = 0;
    err = GetAuthorization(&dwOpLevel, 0); //check authorization, use up 0 runs
    EndCrypkey();

    if (err==0) //check this only if we think we are authorized
      {
      if ( ((dwOpLevel & mask) != 0) )
        {//legal license found
        if (0)
          {//examples to get some info
          char Buff[1024];
          int authopt = 0;
          int num_allowed = 0;
          int num_used = 0;
          ULONG start_date = 0;
          int ret = GetRestrictionInfo(&authopt, &start_date, &num_allowed, &num_used);
          if (ret==0)
            sprintf(Buff, "Date and time license last updated : %d", start_date);
          else
            sprintf(Buff, "Date and time license last updated : Unknown");
          AfxMessageBox(Buff);
          sprintf(Buff, "Number of Copies allowed from this site : %d", GetNumCopies());
          AfxMessageBox(Buff);
          sprintf(Buff, "Number of multi-users that the site has been granted : %d", GetNumMultiUsers());
          AfxMessageBox(Buff);
          if (Get1RestInfo(1)==0)
            sprintf(Buff, "Time Restrictions : None");
          else
            sprintf(Buff, "Time Restrictions : No of days allowed:%d  No of days used:%d", Get1RestInfo(2), Get1RestInfo(3));
          AfxMessageBox(Buff);
          int Ver = CrypkeyVersion();
          if (Ver>0)
            sprintf(Buff, "Using CrypKey Version : %d.%d", (int)floor(Ver/10.0), (int)(Ver - (floor(Ver/10.0)*10.0)));
          else
            sprintf(Buff, "CrypKey Version : Unknown");
          AfxMessageBox(Buff);
          }
        return(true);
        }
      else
        {//report error:
        AfxMessageBox("Alcan License:\nValid license not found!");
        }
      }
    else
      {//report error:
      if (err==-4)
        {//report error:
        AfxMessageBox("Alcan License:\nValid license not found!");
        }
      else
        {
        sprintf(Msg, "Alcan License:\nGetAuthorization Failure %d\n%s for %s", err, ExplainErr(EXP_INIT_ERR, err), Buff);
        AfxMessageBox(Msg);
        }
      }

    }
  else
    {//report error:
    sprintf(Msg, "Alcan License:\nInitialization Failure %d\n%s for %s", err, ExplainErr(EXP_INIT_ERR, err), Buff);
    AfxMessageBox(Msg);
    EndCrypkey();
    }

  return(false);
  }

//---------------------------------------------------------------------------

bool CheckForUnitsLicense()
  {
  return CheckForLicense(CK_ALCANUNITS_Mask4);
  }

//---------------------------------------------------------------------------

bool CheckForPropertiesLicense()
  {
  return CheckForLicense(CK_ALCANPROPS_Mask5);
  }

//===========================================================================
