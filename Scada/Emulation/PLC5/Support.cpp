// Support.cpp : implementation file
//

#include "stdafx.h"
#include "support.h"

#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

//==========================================================================
//
//
//
//==========================================================================

CSCDFileDialog::CSCDFileDialog( BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
  CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
    {
    m_ForOpen=bOpenFileDialog;
    m_DoLCTests=true;
    m_DoingPrjOpen=false;
    };

CSCDFileDialog::~CSCDFileDialog()
  {
  setlocale(LC_ALL, "C");
  };

int CSCDFileDialog::DoModal( )
  {
  int Ret=CFileDialog::DoModal( );
  setlocale(LC_ALL, "C");
  return Ret;
  };

//BOOL CSCDFileDialog::OnFileNameOK( )
//  {
//  if (m_DoLCTests)
//    {
//    CString Tst;
//    CString Fn=GetPathName();
//    //CString FnP=GetPathName();
//    Fn.MakeLower();
//    //FnP.FnDrivePath();
//    //
//    //Tst=FnP+"*.spj";
//    //flag PrjFileEx=FileExists(Tst());
//
//    Tst.Set("\\%s\\",BackupFilesFolderName());
//    Tst.Lower();
//    flag BkUpInPath=(Fn.StrStr(Tst())!=NULL);
//
//    Tst=PrjFiles();
//    flag InPrjFiles=Tst.Length()>0 && (Fn.XStrNICmp(Tst(), Tst.Length())==0);
//
//    Tst=PrjRmtCpyFiles();
//    flag InPrjRmtCpyFiles=Tst.Length()>0 && (Fn.StrNICmp(Tst(), Tst.Length())==0);
//
//    if (m_ForOpen)
//      {
//      CString Ext=GetFileExt();
//      if (m_DoingPrjOpen && Ext.XStrICmp("spj")==0)
//        {
//        if (BkUpInPath)
//          {
//          LogNote("SysCAD", LF_Exclamation|LF_NoSkip, "May not Open File within %s",Fn());
//          return 1;
//          }
//        }
//      else if (UsingPrjLclFiles())
//        {
//        if (!InPrjFiles)
//          {
//          LogNote("SysCAD", LF_Exclamation|LF_NoSkip, "Must select file within %s",PrjFiles());
//          return 1;
//          }
//        }
//      else
//        {
//        if (InPrjRmtCpyFiles || BkUpInPath)
//          {
//          LogNote("SysCAD", LF_Exclamation|LF_NoSkip, "May not select file within %s",PrjRmtCpyFiles());
//          return 1;
//          }
//        }
//      }
//    else // SaveAs
//      {
//      if (m_DoingPrjOpen)
//        {
//        if (PrjFileEx || BkUpInPath)
//          {
//          LogNote("SysCAD", LF_Exclamation|LF_NoSkip, "May not Create Project within %s",Fn());
//          return 1;
//          }
//        }
//      }
//    }
//  return 0;
//  };

//==========================================================================
//
//
//
//==========================================================================
/*
CProfINIFile::CProfINIFile(char* pFilename)
  {
  sFilename = pFilename;
  }

//---------------------------------------------------------------------------

CProfINIFile::CProfINIFile(char* pDir, char* pFilename)
  {
  sFilename = pDir;
  sFilename += pFilename;
  }

//---------------------------------------------------------------------------

CProfINIFile::~CProfINIFile()
  {
  if (!IsWinNT && sFilename.Len()>0)
    Flush();
  }

//---------------------------------------------------------------------------

double CProfINIFile::RdDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dDefault)
  {
  char Buff[256];
  DWORD Len = GetPrivateProfileString(lpszSection, lpszEntry, "", Buff, sizeof(Buff), sFilename());
  if (Len==0)
    return dDefault;
  return SafeAtoF(Buff, dDefault);
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dValue)
  {
  if (Valid(dValue))
    {
    char Buff[256];
    sprintf(Buff, "%.20g", dValue);
    WritePrivateProfileString(lpszSection, lpszEntry, Buff, sFilename());
    }
  else
    WritePrivateProfileString(lpszSection, lpszEntry, "*", sFilename());
  }

//---------------------------------------------------------------------------

long CProfINIFile::RdLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nDefault)
  {
  char Buff[256];
  DWORD Len = GetPrivateProfileString(lpszSection, lpszEntry, "", Buff, sizeof(Buff), sFilename());
  if (Len==0)
    return nDefault;
  return atol(Buff);
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nValue)
  {
  char Buff[256];
  sprintf(Buff, "%li", nValue);
  WritePrivateProfileString(lpszSection, lpszEntry, Buff, sFilename());
  }

//---------------------------------------------------------------------------

int CProfINIFile::RdInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
  {
  char Buff[256];
  DWORD Len = GetPrivateProfileString(lpszSection, lpszEntry, "", Buff, sizeof(Buff), sFilename());
  if (Len==0)
    return nDefault;
  return atoi(Buff);
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
  {
  char Buff[256];
  sprintf(Buff, "%i", nValue);
  WritePrivateProfileString(lpszSection, lpszEntry, Buff, sFilename());
  }

//---------------------------------------------------------------------------

CString &CProfINIFile::RdStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, char* pDefault)
  {
  char Buff[1024];
  //CString s;
  DWORD Len = GetPrivateProfileString(lpszSection, lpszEntry, "", Buff, sizeof(Buff), sFilename());
  if (Len==0)
    sTmpRdStr = pDefault;
  else
    sTmpRdStr = Buff;
  return sTmpRdStr;
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, char* pValue)
  {
  WritePrivateProfileString(lpszSection, lpszEntry, pValue, sFilename());
  }

//---------------------------------------------------------------------------

DWORD CProfINIFile::RdSection(LPCTSTR lpszSection, char* pBuff, int BuffLen)
  {
  DWORD Len = GetPrivateProfileSection(lpszSection, pBuff, BuffLen, sFilename());
  return Len;
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrSection(LPCTSTR lpszSection, char* pBuff)
  {
  WritePrivateProfileSection(lpszSection, pBuff, sFilename());
  }

//---------------------------------------------------------------------------

DWORD CProfINIFile::RdSectionNames(char* pBuff, int BuffLen)
  {
  DWORD Len = GetPrivateProfileSectionNames(pBuff, BuffLen, sFilename());
  return Len;
  }

//---------------------------------------------------------------------------

void CProfINIFile::Flush()
  {
  if (!IsWinNT)
    WritePrivateProfileString(NULL, NULL, NULL, sFilename());
  }

//---------------------------------------------------------------------------

void CProfINIFile::GetProfItem(char* &pBuff, char* &pName, char* &pExp)
  {
  pName=pBuff;
  pExp=strchr(pBuff, '=');
  pBuff+=strlen(pBuff)+1;
  if(pExp)
    {
    *pExp=0;
    pExp++;
    }
  else
    pExp="";
  }
*/
//==========================================================================
//
//
//
//==========================================================================
