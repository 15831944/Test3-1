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
using namespace System::Diagnostics;

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

    try
    {
      // Shell execute Service with parameters projectPath & configPath.
      Process ^proc = gcnew Process();
      proc->EnableRaisingEvents = false;

      String ^ progFiles = gcnew String(ProgFiles());

      String ^ stencilPath = gcnew String(BaseCfgFiles());
      stencilPath = stencilPath + "Stencils";

      proc->StartInfo->FileName = progFiles + "Service.exe";
      proc->StartInfo->Arguments = "\" " + projectPath + " \" \" " + configPath + " \" \" " + stencilPath + " \"";
      proc->Start();
      //proc->WaitForExit();
    }
    catch (Exception ^e)
    {
      // Handle this exception here, basically means the app didn't exist.
    }

    config = gcnew Config;
    protocol = gcnew EngineProtocol;

    bool success = false;
    int i=0;
    while ((!success)&&(i++ < 20)) //_MUST_ find a better way to handle this! (but only temporary...)
    {
      // Basically need to wait until service is ready.
      Sleep(i*i*i); // Last wait will be 1sec.

      delete config;
      config = gcnew Config;
      // Connect to config data.
      success = config->TestUrl(gcnew Uri("ipc://SysCAD.Service/Global"));
    }
    if (success)
    {
      config->Syncxxx();


      success = false;
      i=0;
      while ((!success)&&(i++ < 20)) //_MUST_ find a better way to handle this! (but only temporary...)
      {
        // Basically need to wait until service is ready.
        Sleep(i*i*i); // Last wait will be 1sec.

        delete protocol;
        protocol = gcnew EngineProtocol;

        // Connect to graphic data.
        success = protocol->TestUrl(gcnew Uri("ipc://SysCAD.Service/Engine/" + 
          Path::GetFileNameWithoutExtension(Path::GetDirectoryName(projectPath))));
      }
      if (success)
      {
        protocol->Connect();


        ////////////////////////////////
        ////////////////////////////////

        // Keith:
        // The lines above start the Service, connect to it, and sync the data.
        // You can see below the for each that will be needed to read through each item/link/thing.
        // (things are probably not required a the moment.)

        // Obviously these are empty until we 'bootstrap' the values from the current 9 graphics.
        // The Connect() tries to load the 10 graphics, and if it faile then the Count() of each 
        // dictionary will be 0.
        // A 'save' has to be done first to fill this, and then the 10 graphics can be used.

        for each (GraphicItem ^ item in protocol->graphicItems->Values)
        {
          // 'Go To Definition' on GraphicItem doesn't go to the source but does show the
          // ObjectBrowser with all the available members.
          // e.g.
          // item->Angle
          // item->X
        }

        for each (GraphicLink ^ link in protocol->graphicLinks->Values)
        {
        }

        for each (GraphicThing ^ thing in protocol->graphicThings->Values)
        {
        }

        // GraphicItems is a dictionary of all the items.  The key is the guid, and the value is the item itself.
        // The dictionary class contains all the usual functions for extracting particular elements based on key, etc.
        // protocol->graphicItems->Keys returns a collection of keys.

        // For the 'save' we'll need to do a series of {Delete/Create/Modify}{Item/Link/Thing} to update the service.
        // Then call the Save function in protocol (and later config.)
        // This will cause the data service is holding to be serialized to disk (this might not work at first run.)

        // N.B. Strings are a bastard...  Converting from char* -> String is simply a case of declaring
        // 'String ^ a = gcnew String(charStarVar);'
        // But going the other way as far as I can tell can only be acheived by iterating through the entire String 
        // character by character and popping the null on the end manually.

        // N.B. There are two paths (here and in service.exe) that refer to the full path of the Service.exe
        // (search for /pkh/ and you'll find them.)
        // (I couldn't work out exactly how all the output-path stuff was working,
        // so I reverted back to a static path for now...)

        ///////////////////////////////
        ///////////////////////////////
      }
    }
  };

  void Shutdown()
  {
    delete config;//

    LogNote("CNETServerThread", 0, "Shutdown");
  };

  void Load(String^ filename)
  {
    engineProtocol->Load(filename);
  };

  void Save(String^ filename)
  {
    engineProtocol->Save(filename);
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

  CNETServerThreadGlbl::gs_SrvrThread->Load("");
};

void CNETServer::Save()
{
  LogNote("CNETServer", 0, "Save");

  CNETServerThreadGlbl::gs_SrvrThread->Save("");
};


//========================================================================

#endif

//========================================================================
