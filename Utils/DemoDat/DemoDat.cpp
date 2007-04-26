// DemoDat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DemoDat.h"
#include "crc32static.h"

#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

// DemoDat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "string.h"

void CRCFiles(char* folder, char* extension, DWORD &dwCrc32)
{
  size_t len = strlen(folder)+4;
  char* Tmp= new char[len];
  strcpy_s(Tmp, len, folder);
  strcat_s(Tmp, len, "*.*");

  WIN32_FIND_DATA fd;
  HANDLE H = FindFirstFile(Tmp, &fd);
  bool AllDone = (H==INVALID_HANDLE_VALUE);
  while (!AllDone)
  {
    if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0) 
    {
      if ((_stricmp(fd.cFileName, ".")!=0)&&(_stricmp(fd.cFileName, "..")!=0))
      {
        size_t len = strlen(folder)+strlen(fd.cFileName)+2;
        char* temp = new char[len];
        strcpy_s(temp, len, folder);
        strcat_s(temp, len, fd.cFileName);
        strcat_s(temp, len, "\\");
        CRCFiles(temp, extension, dwCrc32);
      }
    }
    else
    {
      size_t len = strlen(extension)+1;
      char* temp = new char[len];
      strcpy_s(temp, len, &fd.cFileName[strlen(fd.cFileName)-strlen(extension)]);
      _strlwr_s(temp, len);
      if (_stricmp(temp, extension)==0)
      {
        size_t len = strlen(folder)+strlen(fd.cFileName)+2;
        char* Tmp = new char[len];
        strcpy_s(Tmp, len, folder);
        strcat_s(Tmp, len, fd.cFileName);

        CCrc32Static::FileCrc32Streams(Tmp, dwCrc32);
      }
    }
    AllDone = !FindNextFile(H, &fd);
  }
  FindClose(H);
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
    DWORD dwCrc32_dxf = 0xD5452A29;
    DWORD dwCrc32_rct = 0x33CAEEBF;
    DWORD dwCrc32_pgm = 0x6E5415D4;
    DWORD dwCrc32_mdb = 0x6A0696B1;
    DWORD dwCrc32_trn = 0x4F4AEE73;
    DWORD dwCrc32_dat = 0xDF3297AC;

    DWORD multiplier_dxf = 0x2BAE3564;
    DWORD multiplier_rct = 0x76777938;
    DWORD multiplier_pgm = 0x2DCD0605;
    DWORD multiplier_mdb = 0xEFA6BF10;
    DWORD multiplier_trn = 0xBD367A8F;
    DWORD multiplier_dat = 0xD4F8A26A;

    CRCFiles("", ".dxf", dwCrc32_dxf);
    CRCFiles("", ".rct", dwCrc32_rct);
    CRCFiles("", ".pgm", dwCrc32_pgm);
    CRCFiles("", ".mdb", dwCrc32_mdb);
    CRCFiles("", ".trn", dwCrc32_trn);
    CRCFiles("", ".dat", dwCrc32_dat);

    CRCFiles("", ".dxf.x", dwCrc32_dxf);
    CRCFiles("", ".rct.x", dwCrc32_rct);
    CRCFiles("", ".pgm.x", dwCrc32_pgm);
    CRCFiles("", ".mdb.x", dwCrc32_mdb);
    CRCFiles("", ".trn.x", dwCrc32_trn);
    CRCFiles("", ".dat.x", dwCrc32_dat);

    DWORD dwCrc32 =    dwCrc32_dxf * multiplier_dxf
                     ^ dwCrc32_rct * multiplier_rct
                     ^ dwCrc32_pgm * multiplier_pgm
                     ^ dwCrc32_mdb * multiplier_mdb
                     ^ dwCrc32_trn * multiplier_trn
                     ^ dwCrc32_dat * multiplier_dat;

    ofstream demodat;
    demodat.open("demo.dat", ios::out);
    demodat.write(reinterpret_cast<char *>(&dwCrc32), sizeof(DWORD));
	}

	return nRetCode;
}
