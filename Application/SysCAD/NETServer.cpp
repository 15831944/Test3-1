//#include "stdafxclr.h"
#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions
//#include <afxdisp.h>        // MFC Automation classes
//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls


//#include <afxcmn.h>         // MFC extensions
//#include <afxdlgs.h>         // MFC extensions
//#include <afxext.h>         // MFC extensions
//#include <afxole.h>         // MFC extensions
//#include <afxdisp.h>        // MFC Automation classes
//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

//#include "..\..\common\scd\scdlib\sc_defs.h"
//#include "..\..\smdk\include\scdif.h"
#include "..\..\common\scd\scdlib\gpfuncs.h"
//#include ".\grfdoc\grfdoc.h"
#include "errorlog.h"

#include "NETServer.h"
#include "NETServerU.h"


using namespace System;
using namespace System::IO;
//using namespace System::IO::Directory;

using namespace System::Collections;
//using namespace System::Collections::Generic;
using namespace System::Collections::Specialized;

using namespace System::Runtime::Remoting;
using namespace System::Runtime::Remoting::Channels;
using namespace System::Runtime::Remoting::Channels::Ipc;
using namespace System::Runtime::Remoting::Channels::Http;
using namespace System::Runtime::Remoting::Channels::Tcp;

using namespace System::Data;
using namespace System::Data::OleDb;
using namespace System::Drawing;

using namespace SysCAD::Interface;
using namespace System::Runtime::Serialization::Formatters;
using namespace System::Runtime::Serialization::Formatters::Soap;

using namespace System::Runtime::InteropServices;//;::Marshal;

#pragma managed


//========================================================================
//
//
//
//========================================================================

class CSrvrConfig   
  {
  public:
    CSrvrConfig()
      {
      //m_pConfigData = new gcroot<ConfigData^>; 
      //m_pConfigData = gcnew ConfigData; 
      };

    //gcroot<ConfigData^> m_pConfigData;
    //ConfigData^ m_pConfigData;
  };
//    CSrvrProject  * m_pProject;

//static CSrvrConfig XXX;


//========================================================================
//
//
//
//========================================================================

ref class CNETServerThread
  {
  public:
    CNETServerThread()
      {
      };
    ~CNETServerThread()
      {
      };

    void CreateConnects()
      {
      BinaryServerFormatterSinkProvider ^ serverProv = gcnew BinaryServerFormatterSinkProvider;
      serverProv->TypeFilterLevel = System::Runtime::Serialization::Formatters::TypeFilterLevel::Full;

      BinaryClientFormatterSinkProvider ^ clientProv = gcnew BinaryClientFormatterSinkProvider();

      IDictionary ^ ipcProps = gcnew Hashtable();
      ipcProps["portName"] = "SysCAD.Service";
      //ipcProps["typeFilterLevel"] = TypeFilterLevel.Full;
      IpcChannel ^ ipcChannel = gcnew IpcChannel(ipcProps, clientProv, serverProv);
      ChannelServices::RegisterChannel(ipcChannel, false);
      }

    void GetStencils()
      {
      int iStencil = 0;
      LogNote("Srvr", 0, "ModelStencils:");
      array<String^>^dirs = Directory::GetFiles(m_StencilPath, "*.modelstencil");
      for (int i=0; i<dirs->GetLength(0); i++)
        {
        String ^ fullpath = dirs[i];
        SoapFormatter ^ sf = gcnew SoapFormatter;
        StreamReader ^ streamRdr = gcnew StreamReader(fullpath);
        Stream ^ stream = streamRdr->BaseStream;
        ModelStencil ^ modelStencil = (ModelStencil^)sf->Deserialize(stream);
        modelStencil->Tag = Path::GetFileNameWithoutExtension(fullpath);
        m_Config->modelStencils->Add(Path::GetFileNameWithoutExtension(fullpath), modelStencil);
        stream->Close();
        //Console::WriteLine("  {0}] {1}", iStencil++, Path::GetFileNameWithoutExtension(fullpath));
        LogNote("Srvr", 0, "  %i] %s", iStencil++, Path::GetFileNameWithoutExtension(fullpath));
        }

      Console::WriteLine("\n");

      iStencil = 0;
      LogNote("Srvr", 0, "GraphicStencils:");
      dirs = Directory::GetFiles(m_StencilPath, "*.graphicstencil");
      for (int i=0; i<dirs->GetLength(0); i++)
        {
        String ^ fullpath = dirs[i];
        SoapFormatter ^ sf = gcnew SoapFormatter;
        StreamReader ^ streamRdr = gcnew StreamReader(fullpath);
        Stream ^ stream = streamRdr->BaseStream;
        //Stream ^ stream = (gcnew StreamReader(fullpath))->BaseStream;
        GraphicStencil ^ graphicStencil = (GraphicStencil^)sf->Deserialize(stream);
        graphicStencil->Tag = Path::GetFileNameWithoutExtension(fullpath);
        m_Config->graphicStencils->Add(Path::GetFileNameWithoutExtension(fullpath), graphicStencil);
        stream->Close();
        //Console::WriteLine("  {0}] {1}", iStencil++, Path::GetFileNameWithoutExtension(fullpath));
        LogNote("Srvr", 0, "  %i] %s", iStencil++, Path::GetFileNameWithoutExtension(fullpath));
        }
      }

    void MarshalConfig()
      {
      RemotingServices::Marshal(m_Config, "Global");
      }

    void MarshalGraphics()
      {
      String ^ filename;
      Graphic ^ graphic = gcnew Graphic;
      filename = gcnew String(m_pUnmanaged->m_PrjName);

      POSITION Pos=m_pUnmanaged->m_Guids.GetHeadPosition();
      while (Pos)
        {
        CNSGuidItem * pGuid = m_pUnmanaged->m_Guids.GetNext(Pos);
        if (pGuid->m_IsLink)
          {
          CNSMdlLink * pLink = dynamic_cast<CNSMdlLink *>(pGuid); 
          GraphicLink ^ graphicLink = gcnew GraphicLink(Guid(gcnew String(pLink->m_Guid)), gcnew String(pLink->m_Tag));

          CArray <CNSGrfLink::CPt, CNSGrfLink::CPt&> &Pts=pLink->m_pGrf->m_Pts;
          Generic::List<PointF> ^ ControlPoints = gcnew Generic::List<PointF>;

          // Why negative Y ??????
          for (int i=0; i<Pts.GetCount(); i++)
            ControlPoints->Add(PointF(Pts[i].m_X, -Pts[i].m_Y));

          graphicLink->Populate(filename, gcnew String(pLink->m_pGrf->m_Page), 
            gcnew String(pLink->m_Guid), gcnew String(pLink->m_ClassID), 
            gcnew String(pLink->m_SrcGuid), gcnew String(pLink->m_DstGuid),
            gcnew String(pLink->m_SrcPort), gcnew String(pLink->m_DstPort),
            ControlPoints ,
            graphic->graphicItems);

          graphic->graphicLinks->Add(graphicLink->Guid, graphicLink);
          }
        else
          {
          CNSMdlNode * pNode = dynamic_cast<CNSMdlNode *>(pGuid); 
          GraphicItem ^ graphicItem = gcnew GraphicItem(Guid(gcnew String(pNode->m_Guid)), gcnew String(pNode->m_Tag));
          graphicItem->Populate(filename, gcnew String(pNode->m_pGrfs[0]->m_Page),
            gcnew String(pNode->m_Guid), gcnew String(pNode->m_ClassID), 
            pNode->m_pGrfs[0]->m_InsertX,
            pNode->m_pGrfs[0]->m_InsertY,
            pNode->m_pGrfs[0]->m_ScaleX,
            pNode->m_pGrfs[0]->m_ScaleY,
            pNode->m_pGrfs[0]->m_Rotation);
          graphic->graphicItems->Add(graphicItem->Guid, graphicItem);
          }
        }

      RemotingServices::Marshal(graphic, filename);
      m_Config->projectList->Add(filename);
      LogNote("Srvr", 0, "Added project %s to ProjectList.", filename);
      };


    void Startup()
      {
      LogNote("CNETServerThread", 0, "Startup");

      m_Config = gcnew SysCAD::Interface::ConfigData;
      m_StencilPath = gcnew String(BaseCfgFiles());
      m_StencilPath = m_StencilPath + "Stencils\\";

      m_pUnmanaged = new CNETServerU;

      m_pUnmanaged->Startup();

      Console::WriteLine("Startup");
      CreateConnects();
      GetStencils();
      MarshalGraphics();
      MarshalConfig();
      };

    void Shutdown()
      {
      m_pUnmanaged->Shutdown();

      delete m_pUnmanaged;
      m_pUnmanaged=NULL;

      delete m_Config;

      LogNote("CNETServerThread", 0, "Shutdown");
      };


  protected:
    SysCAD::Interface::ConfigData ^ m_Config;
    String ^ m_StencilPath;

    CNETServerU  * m_pUnmanaged;
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

void CNETServer::Startup()
  {
  LogNote("CNETServer", 0, "Startup");

  CNETServerThreadGlbl::gs_SrvrThread = gcnew CNETServerThread;//.Startup("");
  CNETServerThreadGlbl::gs_SrvrThread->Startup();   

  //System::Threading::S
  };

void CNETServer::Shutdown()
  {
  CNETServerThreadGlbl::gs_SrvrThread->Shutdown();
  delete CNETServerThreadGlbl::gs_SrvrThread;

  LogNote("CNETServer", 0, "Shutdown");
  };

