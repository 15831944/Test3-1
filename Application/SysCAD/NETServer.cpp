#include <stdafx.h> //pkh
#include <afxwin.h>         // MFC core and standard components
#include "gpfuncs.h"
#include "errorlog.h"
#include "flwnode.h"
#include "gpwfuncs.h"
#include "neutralcommon.h"

#include "NETServer.h"
//#include "NETServerU.h"

//========================================================================

#if SYSCAD10

//========================================================================

using namespace System;
using namespace System::IO;
using namespace System::Text;

using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Collections::Specialized;

using namespace System::Runtime::Remoting;
using namespace System::Runtime::Remoting::Channels;
using namespace System::Runtime::Remoting::Channels::Ipc;
using namespace System::Runtime::Remoting::Channels::Http;
using namespace System::Runtime::Remoting::Channels::Tcp;

using namespace System::Data;
using namespace System::Data::OleDb;
using namespace System::Drawing;

using namespace SysCAD::Protocol;
using namespace System::Runtime::Serialization::Formatters;
using namespace System::Runtime::Serialization::Formatters::Soap;

using namespace System::Runtime::InteropServices;//;::Marshal;

#pragma managed


//========================================================================
//
//
//
//========================================================================

ref class CNETServerThread
{
public:
	EngineProtocol ^ engineProtocol;

  CNETServerThread()
  {
  };
  ~CNETServerThread()
  {
  };

  void Startup(String^ projectPath, String^ configPath)
  {
    LogNote("CNETServerThread", 0, "Startup");

		// TODO: Shell execute Service with parameters projectPath & configPath.

    config = gcnew Config;
    config->TestUrl(gcnew Uri("ipc://SysCAD.Service/Global"));

		protocol = gcnew EngineProtocol;
		protocol->TestUrl(gcnew Uri("ipc://SysCAD.Service/Global/project"));
  };

  void Shutdown()
  {
    delete config;

    LogNote("CNETServerThread", 0, "Shutdown");
  };

  void Load()
  {
    engineProtocol->Load();
  };

  void Save()
  {
    engineProtocol->Save();
  };
  
protected:
  SysCAD::Protocol::Config ^ config;
	SysCAD::Protocol::EngineProtocol ^ protocol;
};


ref class CNETServerThreadGlbl
{
public:
  static CNETServerThread ^ gs_SrvrThread;// = gcnew CNETServerThread;
};

//========================================================================
//
//
//
//========================================================================

CNETServer::CNETServer(void)
{
}

CNETServer::~CNETServer(void)
{
}

void CNETServer::Startup(char* projectPath, char* configPath)
{
	String^ projectPathString = gcnew String(projectPath);
	String^ configPathString = gcnew String(configPath);

  LogNote("CNETServer", 0, "Startup");

  CNETServerThreadGlbl::gs_SrvrThread = gcnew CNETServerThread;//.Startup("");
  CNETServerThreadGlbl::gs_SrvrThread->Startup(projectPathString, configPathString);   

  //System::Threading::S
};

void CNETServer::Shutdown()
{
  CNETServerThreadGlbl::gs_SrvrThread->Shutdown();
  delete CNETServerThreadGlbl::gs_SrvrThread;

  LogNote("CNETServer", 0, "Shutdown");
};

void CNETServer::Load()
{
  LogNote("CNETServer", 0, "Load");

  CNETServerThreadGlbl::gs_SrvrThread->Load();
};

void CNETServer::Save()
{
  LogNote("CNETServer", 0, "Save");

  CNETServerThreadGlbl::gs_SrvrThread->Save();
};


//========================================================================

#endif

//========================================================================
