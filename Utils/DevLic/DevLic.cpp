// ScdLic.cpp : Defines the class behaviors for the application.
//

//Command Line Examples
// Quiet Mode :- /Q /L"C:\Program Files\SMDK License\" /f"C:\SysCAD81b44\bin\OSM_Methods.dll"
// Info Mode :- /L"C:\Program Files\SMDK License\" /f:"C:\SysCAD81b44\bin\OSM_Methods.dll"
//Post Link example:
// "c:\program files\SMDK License\devlic.exe" /q /f:"$(TargetName)"

#include "stdafx.h"
#include "resource.h"
#include "DevLic.h"
#include "LicDlg.h"
#include "Scribble.h"
#include "crypkey.h"
#include "winsvc.h"
#include "math.h"
#include "float.h"
#include "DLLLicenseDialog.h"
#include "..\..\common\scd\scdlib\ScdVerLic.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================

#define SKIP_LICENSE_CHECK 0 /*(BYPASSLICENSING)*/

//===========================================================================

//for kwa.exe
#define CK_USER_KEY   "DE5E 81ED E83A 12EB 817E B523 1B"
#define CK_MASTER_KEY "E89D 55C4 4D68 EEA4 3A2D D38E D1AD 0C8B 66E8 0C8B DD43"

const DWORD CK_SMDK_Mask10 = 0x4000000;         // The 26th bit
const DWORD CK_SMDK_Mask20Units = 0x2000000;    // The 25th bit
const DWORD CK_SMDK_Mask20Props = 0x1000000;    // The 24th bit

const int CK_NetworkChecktime = 900;

char* CK_KeyFile = "kwa.exe";
char* CK_AppName = "KWA";

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDevLicApp, CWinApp)
  //{{AFX_MSG_MAP(CDevLicApp)
  //}}AFX_MSG
  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
// CDevLicApp construction

CDevLicApp::CDevLicApp()
  {
  }

//---------------------------------------------------------------------------
// The one and only CDevLicApp object

CDevLicApp theApp;

//---------------------------------------------------------------------------

BOOL CDevLicApp::InitInstance()
  {
  // Standard initialization
  // If you are not using these features and wish to reduce the size
  //  of your final executable, you should remove from the following
  //  the specific initialization routines you do not need.

//#ifdef _AFXDLL
//  Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//  Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif

  // Initialise our INI handling stuff
  DoInit();

  bool bAltLicenseLoc = false;
  bool bCmdLineOnly = false;
  bool bQuiet = false;
  CString sDLLName;
  sCmdLine = "";
  if (m_lpCmdLine && m_lpCmdLine != '\0')
    {
    if ( SKIP_LICENSE_CHECK )
      return FALSE; //do nothing

    sCmdLine = m_lpCmdLine; //keep copy of command line
    char CmdLineCopy[2048]; //work on copy!!!
    strcpy(CmdLineCopy, m_lpCmdLine);
    char* p = strtok(CmdLineCopy, " \n");
    BOOL IsOneCmd=false;
    while (p && !IsOneCmd)
      {
      CString OptStr, Opt = p;
      int l = Opt.GetLength(), OptWide=2;
      if ((l>OptWide) && (Opt[2]==':'))
        OptWide = 3;
      OptStr = Opt.Right(l-OptWide);
      Opt = Opt.Left(2);
      if (OptStr.GetLength()>0 && OptStr[0]=='"' && (Opt=="/l" || Opt=="/L" || Opt=="/f" || Opt=="/F"))
        {//find closing "
        OptStr = OptStr.Mid(1, 1024);
        if (OptStr.GetLength()>0 && OptStr[OptStr.GetLength()-1]=='"')
          OptStr = OptStr.Left(OptStr.GetLength()-1);
        else
          {
          OptStr += " ";
          p = strtok(NULL, "\"\n");
          if (p)
            OptStr += p;
          }
        }

      if (Opt=="/l" || Opt=="/L")
        {
        sLicenseLoc = OptStr;
        //if (sLicenseLoc.GetLength()>0)
        bAltLicenseLoc = true;
        }

      if (Opt=="/f" || Opt=="/F")
        {
        // Command Line Option to license file
        bCmdLineOnly = true;
        sDLLName = OptStr;
        }

      if (Opt=="/q" || Opt=="/Q")
        {
        bQuiet = true;
        }


      p = strtok(NULL, " \n");
      }
    }


  // Check to see if we can initialise License

  if ( sLicenseLoc == "" )
    sLicenseLoc = AppPF.RdStr("Developers License 2.0", "SMDK License Location" , "");

  if ( sDLLName == "" )
    sDLLName = AppPF.RdStr("Developers License 2.0", "SMDK DLL Name" , "");;


  // Developers License Exists - We can continue
  if ( bCmdLineOnly )
    {
    
    // Example command line params to test
    // /L"C:\WINNT\SysCAD82License\" /F"C:\SysCAD81b44\bin\OSM_Methods.dll"
    // /Q /s"C:\SysCAD81b44\bin\OSM_Methods.dll"

    bool bOK = false;
    CString sReason;
    if ( CheckLicense() )
      {
      // Command Line Operation
      if ( CScribble::ReadScribble(sDLLName) )
        {
        // Already Licensed so replace existing license
        if ( CScribble::AddScribble(sDLLName,true) )
          {
          // Successfully Licensed
          bOK = true;
          if (!bQuiet) 
            AfxMessageBox("Replaced license in file!");
          }
        else
          {
          sReason = "License existed but failed to replace!";
          }
        }
      else
        {
        // License does not already exist so add a new license
        if ( CScribble::AddScribble(sDLLName,false) )
          {
          // Successfully Licensed
          bOK = true;
          if (!bQuiet) 
            AfxMessageBox("Added license to file!");
          }
        else
          {// Problem
          sReason = "Failed to license file!";
          }
        }
      }
    else
      {
      sReason = "Developer License NOT found!";
      }

    if (!bOK)
      {
      char Buff[1024];
      sprintf(Buff, "SMDK DLL will not be able to be used in SysCAD!\n\n%s", sReason);
      AfxMessageBox(Buff);
      }
    }
  else
    {
    // Dialog Mode
    // Use Dialog Application
    CDevLicDlg dlg;
    m_pMainWnd = &dlg;
    dlg.bForcedLoc = bAltLicenseLoc;
    dlg.sForcedLoc = sLicenseLoc;

    // Operate with Dialogs
    int nResponse = dlg.DoModal();
    }


  // Since the dialog has been closed, return FALSE so that we exit the
  //  application, rather than start the application's message pump.
  return FALSE;
  }

//---------------------------------------------------------------------------

bool CDevLicApp::CheckLicense()
  {
  CWaitCursor Wait;
  char Buff[_MAX_PATH];
  sprintf(Buff, "%s%s", (const char*)sLicenseLoc, CK_KeyFile);
  int err = InitCrypkey( Buff, CK_MASTER_KEY, CK_USER_KEY, FALSE, CK_NetworkChecktime);

  if (err==0)
    {
    DWORD dwOpLevel = 0;
    err = GetAuthorization(&dwOpLevel, 0); //check authorization, use up 0 runs
    EndCrypkey();

    if (err==0) //check this only if we think we are authorized
      {

      if ( ((dwOpLevel & CK_SMDK_Mask20Units) != 0) || ((dwOpLevel & CK_SMDK_Mask20Props) != 0) )
        {
        return(true);
        }

      }

    }
  else
    {
    EndCrypkey();
    }

  return(false);
  }

//---------------------------------------------------------------------------
