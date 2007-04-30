// DemoDat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DemoDat.h"
//#include "DemoDatConfig.h"

#include <iostream>
#include <fstream>

#include "CalculateDemoKey.h"

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

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

  //DemoDatConfig config = InitialiseDemoDatConfig();

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
    char* folder = "";
    DWORD dwCrc32 = CalculateDemoKey::DoCalculateDemoKey(folder);

    ofstream demodat;
    demodat.open("demo.dat", ios::out);
    demodat.write(reinterpret_cast<char *>(&dwCrc32), sizeof(DWORD));
	}

	return nRetCode;
}
